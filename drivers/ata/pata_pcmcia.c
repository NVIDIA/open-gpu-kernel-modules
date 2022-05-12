// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *   pata_pcmcia.c - PCMCIA PATA controller driver.
 *   Copyright 2005-2006 Red Hat Inc, all rights reserved.
 *   PCMCIA ident update Copyright 2006 Marcin Juszkiewicz
 *						<openembedded@hrw.one.pl>
 *
 *   Heavily based upon ide-cs.c
 *   The initial developer of the original code is David A. Hinds
 *   <dahinds@users.sourceforge.net>.  Portions created by David A. Hinds
 *   are Copyright (C) 1999 David A. Hinds.  All Rights Reserved.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/blkdev.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <scsi/scsi_host.h>
#include <linux/ata.h>
#include <linux/libata.h>

#include <pcmcia/cistpl.h>
#include <pcmcia/ds.h>
#include <pcmcia/cisreg.h>
#include <pcmcia/ciscode.h>


#define DRV_NAME "pata_pcmcia"
#define DRV_VERSION "0.3.5"

/**
 *	pcmcia_set_mode	-	PCMCIA specific mode setup
 *	@link: link
 *	@r_failed_dev: Return pointer for failed device
 *
 *	Perform the tuning and setup of the devices and timings, which
 *	for PCMCIA is the same as any other controller. We wrap it however
 *	as we need to spot hardware with incorrect or missing master/slave
 *	decode, which alas is embarrassingly common in the PC world
 */

static int pcmcia_set_mode(struct ata_link *link, struct ata_device **r_failed_dev)
{
	struct ata_device *master = &link->device[0];
	struct ata_device *slave = &link->device[1];

	if (!ata_dev_enabled(master) || !ata_dev_enabled(slave))
		return ata_do_set_mode(link, r_failed_dev);

	if (memcmp(master->id + ATA_ID_FW_REV,  slave->id + ATA_ID_FW_REV,
			   ATA_ID_FW_REV_LEN + ATA_ID_PROD_LEN) == 0) {
		/* Suspicious match, but could be two cards from
		   the same vendor - check serial */
		if (memcmp(master->id + ATA_ID_SERNO, slave->id + ATA_ID_SERNO,
			   ATA_ID_SERNO_LEN) == 0 && master->id[ATA_ID_SERNO] >> 8) {
			ata_dev_warn(slave, "is a ghost device, ignoring\n");
			ata_dev_disable(slave);
		}
	}
	return ata_do_set_mode(link, r_failed_dev);
}

/**
 *	pcmcia_set_mode_8bit	-	PCMCIA specific mode setup
 *	@link: link
 *	@r_failed_dev: Return pointer for failed device
 *
 *	For the simple emulated 8bit stuff the less we do the better.
 */

static int pcmcia_set_mode_8bit(struct ata_link *link,
				struct ata_device **r_failed_dev)
{
	return 0;
}

/**
 *	ata_data_xfer_8bit	 -	Transfer data by 8bit PIO
 *	@qc: queued command
 *	@buf: data buffer
 *	@buflen: buffer length
 *	@rw: read/write
 *
 *	Transfer data from/to the device data register by 8 bit PIO.
 *
 *	LOCKING:
 *	Inherited from caller.
 */

static unsigned int ata_data_xfer_8bit(struct ata_queued_cmd *qc,
				unsigned char *buf, unsigned int buflen, int rw)
{
	struct ata_port *ap = qc->dev->link->ap;

	if (rw == READ)
		ioread8_rep(ap->ioaddr.data_addr, buf, buflen);
	else
		iowrite8_rep(ap->ioaddr.data_addr, buf, buflen);

	return buflen;
}

/**
 *	pcmcia_8bit_drain_fifo - Stock FIFO drain logic for SFF controllers
 *	@qc: command
 *
 *	Drain the FIFO and device of any stuck data following a command
 *	failing to complete. In some cases this is necessary before a
 *	reset will recover the device.
 *
 */

static void pcmcia_8bit_drain_fifo(struct ata_queued_cmd *qc)
{
	int count;
	struct ata_port *ap;

	/* We only need to flush incoming data when a command was running */
	if (qc == NULL || qc->dma_dir == DMA_TO_DEVICE)
		return;

	ap = qc->ap;

	/* Drain up to 64K of data before we give up this recovery method */
	for (count = 0; (ap->ops->sff_check_status(ap) & ATA_DRQ)
							&& count++ < 65536;)
		ioread8(ap->ioaddr.data_addr);

	if (count)
		ata_port_warn(ap, "drained %d bytes to clear DRQ\n", count);

}

static struct scsi_host_template pcmcia_sht = {
	ATA_PIO_SHT(DRV_NAME),
};

static struct ata_port_operations pcmcia_port_ops = {
	.inherits	= &ata_sff_port_ops,
	.sff_data_xfer	= ata_sff_data_xfer32,
	.cable_detect	= ata_cable_40wire,
	.set_mode	= pcmcia_set_mode,
};

static struct ata_port_operations pcmcia_8bit_port_ops = {
	.inherits	= &ata_sff_port_ops,
	.sff_data_xfer	= ata_data_xfer_8bit,
	.cable_detect	= ata_cable_40wire,
	.set_mode	= pcmcia_set_mode_8bit,
	.sff_drain_fifo	= pcmcia_8bit_drain_fifo,
};


static int pcmcia_check_one_config(struct pcmcia_device *pdev, void *priv_data)
{
	int *is_kme = priv_data;

	if ((pdev->resource[0]->flags & IO_DATA_PATH_WIDTH)
	    != IO_DATA_PATH_WIDTH_8) {
		pdev->resource[0]->flags &= ~IO_DATA_PATH_WIDTH;
		pdev->resource[0]->flags |= IO_DATA_PATH_WIDTH_AUTO;
	}
	pdev->resource[1]->flags &= ~IO_DATA_PATH_WIDTH;
	pdev->resource[1]->flags |= IO_DATA_PATH_WIDTH_8;

	if (pdev->resource[1]->end) {
		pdev->resource[0]->end = 8;
		pdev->resource[1]->end = (*is_kme) ? 2 : 1;
	} else {
		if (pdev->resource[0]->end < 16)
			return -ENODEV;
	}

	return pcmcia_request_io(pdev);
}

/**
 *	pcmcia_init_one		-	attach a PCMCIA interface
 *	@pdev: pcmcia device
 *
 *	Register a PCMCIA IDE interface. Such interfaces are PIO 0 and
 *	shared IRQ.
 */

static int pcmcia_init_one(struct pcmcia_device *pdev)
{
	struct ata_host *host;
	struct ata_port *ap;
	int is_kme = 0, ret = -ENOMEM, p;
	unsigned long io_base, ctl_base;
	void __iomem *io_addr, *ctl_addr;
	int n_ports = 1;
	struct ata_port_operations *ops = &pcmcia_port_ops;

	/* Set up attributes in order to probe card and get resources */
	pdev->config_flags |= CONF_ENABLE_IRQ | CONF_AUTO_SET_IO |
		CONF_AUTO_SET_VPP | CONF_AUTO_CHECK_VCC;

	/* See if we have a manufacturer identifier. Use it to set is_kme for
	   vendor quirks */
	is_kme = ((pdev->manf_id == MANFID_KME) &&
		  ((pdev->card_id == PRODID_KME_KXLC005_A) ||
		   (pdev->card_id == PRODID_KME_KXLC005_B)));

	if (pcmcia_loop_config(pdev, pcmcia_check_one_config, &is_kme)) {
		pdev->config_flags &= ~CONF_AUTO_CHECK_VCC;
		if (pcmcia_loop_config(pdev, pcmcia_check_one_config, &is_kme))
			goto failed; /* No suitable config found */
	}
	io_base = pdev->resource[0]->start;
	if (pdev->resource[1]->end)
		ctl_base = pdev->resource[1]->start;
	else
		ctl_base = pdev->resource[0]->start + 0x0e;

	if (!pdev->irq)
		goto failed;

	ret = pcmcia_enable_device(pdev);
	if (ret)
		goto failed;

	/* iomap */
	ret = -ENOMEM;
	io_addr = devm_ioport_map(&pdev->dev, io_base, 8);
	ctl_addr = devm_ioport_map(&pdev->dev, ctl_base, 1);
	if (!io_addr || !ctl_addr)
		goto failed;

	/* Success. Disable the IRQ nIEN line, do quirks */
	iowrite8(0x02, ctl_addr);
	if (is_kme)
		iowrite8(0x81, ctl_addr + 0x01);

	/* FIXME: Could be more ports at base + 0x10 but we only deal with
	   one right now */
	if (resource_size(pdev->resource[0]) >= 0x20)
		n_ports = 2;

	if (pdev->manf_id == 0x0097 && pdev->card_id == 0x1620)
		ops = &pcmcia_8bit_port_ops;
	/*
	 *	Having done the PCMCIA plumbing the ATA side is relatively
	 *	sane.
	 */
	ret = -ENOMEM;
	host = ata_host_alloc(&pdev->dev, n_ports);
	if (!host)
		goto failed;

	for (p = 0; p < n_ports; p++) {
		ap = host->ports[p];

		ap->ops = ops;
		ap->pio_mask = ATA_PIO0;	/* ISA so PIO 0 cycles */
		ap->flags |= ATA_FLAG_SLAVE_POSS;
		ap->ioaddr.cmd_addr = io_addr + 0x10 * p;
		ap->ioaddr.altstatus_addr = ctl_addr + 0x10 * p;
		ap->ioaddr.ctl_addr = ctl_addr + 0x10 * p;
		ata_sff_std_ports(&ap->ioaddr);

		ata_port_desc(ap, "cmd 0x%lx ctl 0x%lx", io_base, ctl_base);
	}

	/* activate */
	ret = ata_host_activate(host, pdev->irq, ata_sff_interrupt,
				IRQF_SHARED, &pcmcia_sht);
	if (ret)
		goto failed;

	pdev->priv = host;
	return 0;

failed:
	pcmcia_disable_device(pdev);
	return ret;
}

/**
 *	pcmcia_remove_one	-	unplug an pcmcia interface
 *	@pdev: pcmcia device
 *
 *	A PCMCIA ATA device has been unplugged. Perform the needed
 *	cleanup. Also called on module unload for any active devices.
 */

static void pcmcia_remove_one(struct pcmcia_device *pdev)
{
	struct ata_host *host = pdev->priv;

	if (host)
		ata_host_detach(host);

	pcmcia_disable_device(pdev);
}

static const struct pcmcia_device_id pcmcia_devices[] = {
	PCMCIA_DEVICE_FUNC_ID(4),
	PCMCIA_DEVICE_MANF_CARD(0x0000, 0x0000),	/* Corsair */
	PCMCIA_DEVICE_MANF_CARD(0x0007, 0x0000),	/* Hitachi */
	PCMCIA_DEVICE_MANF_CARD(0x000a, 0x0000),	/* I-O Data CFA */
	PCMCIA_DEVICE_MANF_CARD(0x001c, 0x0001),	/* Mitsubishi CFA */
	PCMCIA_DEVICE_MANF_CARD(0x0032, 0x0704),
	PCMCIA_DEVICE_MANF_CARD(0x0032, 0x2904),
	PCMCIA_DEVICE_MANF_CARD(0x0045, 0x0401),	/* SanDisk CFA */
	PCMCIA_DEVICE_MANF_CARD(0x004f, 0x0000),	/* Kingston */
	PCMCIA_DEVICE_MANF_CARD(0x0097, 0x1620), 	/* TI emulated */
	PCMCIA_DEVICE_MANF_CARD(0x0098, 0x0000),	/* Toshiba */
	PCMCIA_DEVICE_MANF_CARD(0x00a4, 0x002d),
	PCMCIA_DEVICE_MANF_CARD(0x00ce, 0x0000),	/* Samsung */
	PCMCIA_DEVICE_MANF_CARD(0x00f1, 0x0101),	/* SanDisk High (>8G) CFA */
	PCMCIA_DEVICE_MANF_CARD(0x0319, 0x0000),	/* Hitachi */
	PCMCIA_DEVICE_MANF_CARD(0x2080, 0x0001),
	PCMCIA_DEVICE_MANF_CARD(0x4e01, 0x0100),	/* Viking CFA */
	PCMCIA_DEVICE_MANF_CARD(0x4e01, 0x0200),	/* Lexar, Viking CFA */
	PCMCIA_DEVICE_PROD_ID123("Caravelle", "PSC-IDE ", "PSC000", 0x8c36137c, 0xd0693ab8, 0x2768a9f0),
	PCMCIA_DEVICE_PROD_ID123("CDROM", "IDE", "MCD-601p", 0x1b9179ca, 0xede88951, 0x0d902f74),
	PCMCIA_DEVICE_PROD_ID123("PCMCIA", "IDE CARD", "F1", 0x281f1c5d, 0x1907960c, 0xf7fde8b9),
	PCMCIA_DEVICE_PROD_ID12("ARGOSY", "CD-ROM", 0x78f308dc, 0x66536591),
	PCMCIA_DEVICE_PROD_ID12("ARGOSY", "PnPIDE", 0x78f308dc, 0x0c694728),
	PCMCIA_DEVICE_PROD_ID12("CNF   ", "CD-ROM", 0x46d7db81, 0x66536591),
	PCMCIA_DEVICE_PROD_ID12("CNF CD-M", "CD-ROM", 0x7d93b852, 0x66536591),
	PCMCIA_DEVICE_PROD_ID12("Creative Technology Ltd.", "PCMCIA CD-ROM Interface Card", 0xff8c8a45, 0xfe8020c4),
	PCMCIA_DEVICE_PROD_ID12("Digital Equipment Corporation.", "Digital Mobile Media CD-ROM", 0x17692a66, 0xef1dcbde),
	PCMCIA_DEVICE_PROD_ID12("EXP", "CD+GAME", 0x6f58c983, 0x63c13aaf),
	PCMCIA_DEVICE_PROD_ID12("EXP   ", "CD-ROM", 0x0a5c52fd, 0x66536591),
	PCMCIA_DEVICE_PROD_ID12("EXP   ", "PnPIDE", 0x0a5c52fd, 0x0c694728),
	PCMCIA_DEVICE_PROD_ID12("FREECOM", "PCCARD-IDE", 0x5714cbf7, 0x48e0ab8e),
	PCMCIA_DEVICE_PROD_ID12("HITACHI", "FLASH", 0xf4f43949, 0x9eb86aae),
	PCMCIA_DEVICE_PROD_ID12("HITACHI", "microdrive", 0xf4f43949, 0xa6d76178),
	PCMCIA_DEVICE_PROD_ID12("Hyperstone", "Model1", 0x3d5b9ef5, 0xca6ab420),
	PCMCIA_DEVICE_PROD_ID12("IBM", "microdrive", 0xb569a6e5, 0xa6d76178),
	PCMCIA_DEVICE_PROD_ID12("IBM", "IBM17JSSFP20", 0xb569a6e5, 0xf2508753),
	PCMCIA_DEVICE_PROD_ID12("KINGSTON", "CF CARD 1GB", 0x2e6d1829, 0x55d5bffb),
	PCMCIA_DEVICE_PROD_ID12("KINGSTON", "CF CARD 4GB", 0x2e6d1829, 0x531e7d10),
	PCMCIA_DEVICE_PROD_ID12("KINGSTON", "CF8GB", 0x2e6d1829, 0xacbe682e),
	PCMCIA_DEVICE_PROD_ID12("IO DATA", "CBIDE2      ", 0x547e66dc, 0x8671043b),
	PCMCIA_DEVICE_PROD_ID12("IO DATA", "PCIDE", 0x547e66dc, 0x5c5ab149),
	PCMCIA_DEVICE_PROD_ID12("IO DATA", "PCIDEII", 0x547e66dc, 0xb3662674),
	PCMCIA_DEVICE_PROD_ID12("LOOKMEET", "CBIDE2      ", 0xe37be2b5, 0x8671043b),
	PCMCIA_DEVICE_PROD_ID12("M-Systems", "CF300", 0x7ed2ad87, 0x7e9e78ee),
	PCMCIA_DEVICE_PROD_ID12("M-Systems", "CF500", 0x7ed2ad87, 0x7a13045c),
	PCMCIA_DEVICE_PROD_ID2("NinjaATA-", 0xebe0bd79),
	PCMCIA_DEVICE_PROD_ID12("PCMCIA", "CD-ROM", 0x281f1c5d, 0x66536591),
	PCMCIA_DEVICE_PROD_ID12("PCMCIA", "PnPIDE", 0x281f1c5d, 0x0c694728),
	PCMCIA_DEVICE_PROD_ID12("SHUTTLE TECHNOLOGY LTD.", "PCCARD-IDE/ATAPI Adapter", 0x4a3f0ba0, 0x322560e1),
	PCMCIA_DEVICE_PROD_ID12("SEAGATE", "ST1", 0x87c1b330, 0xe1f30883),
	PCMCIA_DEVICE_PROD_ID12("SAMSUNG", "04/05/06", 0x43d74cb4, 0x6a22777d),
	PCMCIA_DEVICE_PROD_ID12("SMI VENDOR", "SMI PRODUCT", 0x30896c92, 0x703cc5f6),
	PCMCIA_DEVICE_PROD_ID12("TOSHIBA", "MK2001MPL", 0xb4585a1a, 0x3489e003),
	PCMCIA_DEVICE_PROD_ID1("TRANSCEND    512M   ", 0xd0909443),
	PCMCIA_DEVICE_PROD_ID12("TRANSCEND", "TS1GCF45", 0x709b1bf1, 0xf68b6f32),
	PCMCIA_DEVICE_PROD_ID12("TRANSCEND", "TS1GCF80", 0x709b1bf1, 0x2a54d4b1),
	PCMCIA_DEVICE_PROD_ID12("TRANSCEND", "TS2GCF120", 0x709b1bf1, 0x969aa4f2),
	PCMCIA_DEVICE_PROD_ID12("TRANSCEND", "TS4GCF120", 0x709b1bf1, 0xf54a91c8),
	PCMCIA_DEVICE_PROD_ID12("TRANSCEND", "TS4GCF133", 0x709b1bf1, 0x7558f133),
	PCMCIA_DEVICE_PROD_ID12("TRANSCEND", "TS8GCF133", 0x709b1bf1, 0xb2f89b47),
	PCMCIA_DEVICE_PROD_ID12("WIT", "IDE16", 0x244e5994, 0x3e232852),
	PCMCIA_DEVICE_PROD_ID12("WEIDA", "TWTTI", 0xcc7cf69c, 0x212bb918),
	PCMCIA_DEVICE_PROD_ID1("STI Flash", 0xe4a13209),
	PCMCIA_DEVICE_PROD_ID12("STI", "Flash 5.0", 0xbf2df18d, 0x8cb57a0e),
	PCMCIA_MFC_DEVICE_PROD_ID12(1, "SanDisk", "ConnectPlus", 0x7a954bd9, 0x74be00c6),
	PCMCIA_DEVICE_PROD_ID2("Flash Card", 0x5a362506),
	PCMCIA_DEVICE_NULL,
};

MODULE_DEVICE_TABLE(pcmcia, pcmcia_devices);

static struct pcmcia_driver pcmcia_driver = {
	.owner		= THIS_MODULE,
	.name		= DRV_NAME,
	.id_table	= pcmcia_devices,
	.probe		= pcmcia_init_one,
	.remove		= pcmcia_remove_one,
};
module_pcmcia_driver(pcmcia_driver);

MODULE_AUTHOR("Alan Cox");
MODULE_DESCRIPTION("low-level driver for PCMCIA ATA");
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_VERSION);
