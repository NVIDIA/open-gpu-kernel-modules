// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 1997-1998	Mark Lord <mlord@pobox.com>
 * Copyright (C) 1998		Eddie C. Dost <ecd@skynet.be>
 * Copyright (C) 1999-2000	Andre Hedrick <andre@linux-ide.org>
 * Copyright (C) 2004		Grant Grundler <grundler at parisc-linux.org>
 *
 * Inspired by an earlier effort from David S. Miller <davem@redhat.com>
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/pci.h>
#include <linux/delay.h>
#include <linux/ide.h>
#include <linux/init.h>

#include <asm/io.h>

#define DRV_NAME "ns87415"

#ifdef CONFIG_SUPERIO
/* SUPERIO 87560 is a PoS chip that NatSem denies exists.
 * Unfortunately, it's built-in on all Astro-based PA-RISC workstations
 * which use the integrated NS87514 cell for CD-ROM support.
 * i.e we have to support for CD-ROM installs.
 * See drivers/parisc/superio.c for more gory details.
 */
#include <asm/superio.h>

#define SUPERIO_IDE_MAX_RETRIES 25

/* Because of a defect in Super I/O, all reads of the PCI DMA status 
 * registers, IDE status register and the IDE select register need to be 
 * retried
 */
static u8 superio_ide_inb (unsigned long port)
{
	u8 tmp;
	int retries = SUPERIO_IDE_MAX_RETRIES;

	/* printk(" [ reading port 0x%x with retry ] ", port); */

	do {
		tmp = inb(port);
		if (tmp == 0)
			udelay(50);
	} while (tmp == 0 && retries-- > 0);

	return tmp;
}

static u8 superio_read_status(ide_hwif_t *hwif)
{
	return superio_ide_inb(hwif->io_ports.status_addr);
}

static u8 superio_dma_sff_read_status(ide_hwif_t *hwif)
{
	return superio_ide_inb(hwif->dma_base + ATA_DMA_STATUS);
}

static void superio_tf_read(ide_drive_t *drive, struct ide_taskfile *tf,
			    u8 valid)
{
	struct ide_io_ports *io_ports = &drive->hwif->io_ports;

	if (valid & IDE_VALID_ERROR)
		tf->error  = inb(io_ports->feature_addr);
	if (valid & IDE_VALID_NSECT)
		tf->nsect  = inb(io_ports->nsect_addr);
	if (valid & IDE_VALID_LBAL)
		tf->lbal   = inb(io_ports->lbal_addr);
	if (valid & IDE_VALID_LBAM)
		tf->lbam   = inb(io_ports->lbam_addr);
	if (valid & IDE_VALID_LBAH)
		tf->lbah   = inb(io_ports->lbah_addr);
	if (valid & IDE_VALID_DEVICE)
		tf->device = superio_ide_inb(io_ports->device_addr);
}

static void ns87415_dev_select(ide_drive_t *drive);

static const struct ide_tp_ops superio_tp_ops = {
	.exec_command		= ide_exec_command,
	.read_status		= superio_read_status,
	.read_altstatus		= ide_read_altstatus,
	.write_devctl		= ide_write_devctl,

	.dev_select		= ns87415_dev_select,
	.tf_load		= ide_tf_load,
	.tf_read		= superio_tf_read,

	.input_data		= ide_input_data,
	.output_data		= ide_output_data,
};

static void superio_init_iops(struct hwif_s *hwif)
{
	struct pci_dev *pdev = to_pci_dev(hwif->dev);
	u32 dma_stat;
	u8 port = hwif->channel, tmp;

	dma_stat = (pci_resource_start(pdev, 4) & ~3) + (!port ? 2 : 0xa);

	/* Clear error/interrupt, enable dma */
	tmp = superio_ide_inb(dma_stat);
	outb(tmp | 0x66, dma_stat);
}
#else
#define superio_dma_sff_read_status ide_dma_sff_read_status
#endif

static unsigned int ns87415_count = 0, ns87415_control[MAX_HWIFS] = { 0 };

/*
 * This routine either enables/disables (according to IDE_DFLAG_PRESENT)
 * the IRQ associated with the port,
 * and selects either PIO or DMA handshaking for the next I/O operation.
 */
static void ns87415_prepare_drive (ide_drive_t *drive, unsigned int use_dma)
{
	ide_hwif_t *hwif = drive->hwif;
	struct pci_dev *dev = to_pci_dev(hwif->dev);
	unsigned int bit, other, new, *old = (unsigned int *) hwif->select_data;
	unsigned long flags;

	local_irq_save(flags);
	new = *old;

	/* Adjust IRQ enable bit */
	bit = 1 << (8 + hwif->channel);

	if (drive->dev_flags & IDE_DFLAG_PRESENT)
		new &= ~bit;
	else
		new |= bit;

	/* Select PIO or DMA, DMA may only be selected for one drive/channel. */
	bit   = 1 << (20 + (drive->dn & 1) + (hwif->channel << 1));
	other = 1 << (20 + (1 - (drive->dn & 1)) + (hwif->channel << 1));
	new = use_dma ? ((new & ~other) | bit) : (new & ~bit);

	if (new != *old) {
		unsigned char stat;

		/*
		 * Don't change DMA engine settings while Write Buffers
		 * are busy.
		 */
		(void) pci_read_config_byte(dev, 0x43, &stat);
		while (stat & 0x03) {
			udelay(1);
			(void) pci_read_config_byte(dev, 0x43, &stat);
		}

		*old = new;
		(void) pci_write_config_dword(dev, 0x40, new);

		/*
		 * And let things settle...
		 */
		udelay(10);
	}

	local_irq_restore(flags);
}

static void ns87415_dev_select(ide_drive_t *drive)
{
	ns87415_prepare_drive(drive,
			      !!(drive->dev_flags & IDE_DFLAG_USING_DMA));

	outb(drive->select | ATA_DEVICE_OBS, drive->hwif->io_ports.device_addr);
}

static void ns87415_dma_start(ide_drive_t *drive)
{
	ns87415_prepare_drive(drive, 1);
	ide_dma_start(drive);
}

static int ns87415_dma_end(ide_drive_t *drive)
{
	ide_hwif_t *hwif = drive->hwif;
	u8 dma_stat = 0, dma_cmd = 0;

	dma_stat = hwif->dma_ops->dma_sff_read_status(hwif);
	/* get DMA command mode */
	dma_cmd = inb(hwif->dma_base + ATA_DMA_CMD);
	/* stop DMA */
	outb(dma_cmd & ~1, hwif->dma_base + ATA_DMA_CMD);
	/* from ERRATA: clear the INTR & ERROR bits */
	dma_cmd = inb(hwif->dma_base + ATA_DMA_CMD);
	outb(dma_cmd | 6, hwif->dma_base + ATA_DMA_CMD);

	ns87415_prepare_drive(drive, 0);

	/* verify good DMA status */
	return (dma_stat & 7) != 4;
}

static void init_hwif_ns87415 (ide_hwif_t *hwif)
{
	struct pci_dev *dev = to_pci_dev(hwif->dev);
	unsigned int ctrl, using_inta;
	u8 progif;
#ifdef __sparc_v9__
	int timeout;
	u8 stat;
#endif

	/*
	 * We cannot probe for IRQ: both ports share common IRQ on INTA.
	 * Also, leave IRQ masked during drive probing, to prevent infinite
	 * interrupts from a potentially floating INTA..
	 *
	 * IRQs get unmasked in dev_select() when drive is first used.
	 */
	(void) pci_read_config_dword(dev, 0x40, &ctrl);
	(void) pci_read_config_byte(dev, 0x09, &progif);
	/* is irq in "native" mode? */
	using_inta = progif & (1 << (hwif->channel << 1));
	if (!using_inta)
		using_inta = ctrl & (1 << (4 + hwif->channel));
	if (hwif->mate) {
		hwif->select_data = hwif->mate->select_data;
	} else {
		hwif->select_data = (unsigned long)
					&ns87415_control[ns87415_count++];
		ctrl |= (1 << 8) | (1 << 9);	/* mask both IRQs */
		if (using_inta)
			ctrl &= ~(1 << 6);	/* unmask INTA */
		*((unsigned int *)hwif->select_data) = ctrl;
		(void) pci_write_config_dword(dev, 0x40, ctrl);

		/*
		 * Set prefetch size to 512 bytes for both ports,
		 * but don't turn on/off prefetching here.
		 */
		pci_write_config_byte(dev, 0x55, 0xee);

#ifdef __sparc_v9__
		/*
		 * XXX: Reset the device, if we don't it will not respond to
		 *      dev_select() properly during first ide_probe_port().
		 */
		timeout = 10000;
		outb(12, hwif->io_ports.ctl_addr);
		udelay(10);
		outb(8, hwif->io_ports.ctl_addr);
		do {
			udelay(50);
			stat = hwif->tp_ops->read_status(hwif);
			if (stat == 0xff)
				break;
		} while ((stat & ATA_BUSY) && --timeout);
#endif
	}

	if (!using_inta)
		hwif->irq = pci_get_legacy_ide_irq(dev, hwif->channel);

	if (!hwif->dma_base)
		return;

	outb(0x60, hwif->dma_base + ATA_DMA_STATUS);
}

static const struct ide_tp_ops ns87415_tp_ops = {
	.exec_command		= ide_exec_command,
	.read_status		= ide_read_status,
	.read_altstatus		= ide_read_altstatus,
	.write_devctl		= ide_write_devctl,

	.dev_select		= ns87415_dev_select,
	.tf_load		= ide_tf_load,
	.tf_read		= ide_tf_read,

	.input_data		= ide_input_data,
	.output_data		= ide_output_data,
};

static const struct ide_dma_ops ns87415_dma_ops = {
	.dma_host_set		= ide_dma_host_set,
	.dma_setup		= ide_dma_setup,
	.dma_start		= ns87415_dma_start,
	.dma_end		= ns87415_dma_end,
	.dma_test_irq		= ide_dma_test_irq,
	.dma_lost_irq		= ide_dma_lost_irq,
	.dma_timer_expiry	= ide_dma_sff_timer_expiry,
	.dma_sff_read_status	= superio_dma_sff_read_status,
};

static const struct ide_port_info ns87415_chipset = {
	.name		= DRV_NAME,
	.init_hwif	= init_hwif_ns87415,
	.tp_ops 	= &ns87415_tp_ops,
	.dma_ops	= &ns87415_dma_ops,
	.host_flags	= IDE_HFLAG_TRUST_BIOS_FOR_DMA |
			  IDE_HFLAG_NO_ATAPI_DMA,
};

static int ns87415_init_one(struct pci_dev *dev, const struct pci_device_id *id)
{
	struct ide_port_info d = ns87415_chipset;

#ifdef CONFIG_SUPERIO
	if (PCI_SLOT(dev->devfn) == 0xE) {
		/* Built-in - assume it's under superio. */
		d.init_iops = superio_init_iops;
		d.tp_ops = &superio_tp_ops;
	}
#endif
	return ide_pci_init_one(dev, &d, NULL);
}

static const struct pci_device_id ns87415_pci_tbl[] = {
	{ PCI_VDEVICE(NS, PCI_DEVICE_ID_NS_87415), 0 },
	{ 0, },
};
MODULE_DEVICE_TABLE(pci, ns87415_pci_tbl);

static struct pci_driver ns87415_pci_driver = {
	.name		= "NS87415_IDE",
	.id_table	= ns87415_pci_tbl,
	.probe		= ns87415_init_one,
	.remove		= ide_pci_remove,
	.suspend	= ide_pci_suspend,
	.resume		= ide_pci_resume,
};

static int __init ns87415_ide_init(void)
{
	return ide_pci_register_driver(&ns87415_pci_driver);
}

static void __exit ns87415_ide_exit(void)
{
	pci_unregister_driver(&ns87415_pci_driver);
}

module_init(ns87415_ide_init);
module_exit(ns87415_ide_exit);

MODULE_AUTHOR("Mark Lord, Eddie Dost, Andre Hedrick");
MODULE_DESCRIPTION("PCI driver module for NS87415 IDE");
MODULE_LICENSE("GPL");
