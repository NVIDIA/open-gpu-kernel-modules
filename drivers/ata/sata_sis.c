// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  sata_sis.c - Silicon Integrated Systems SATA
 *
 *  Maintained by:  Uwe Koziolek
 *  		    Please ALWAYS copy linux-ide@vger.kernel.org
 *		    on emails.
 *
 *  Copyright 2004 Uwe Koziolek
 *
 *  libata documentation is available via 'make {ps|pdf}docs',
 *  as Documentation/driver-api/libata.rst
 *
 *  Hardware documentation available under NDA.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/blkdev.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/device.h>
#include <scsi/scsi_host.h>
#include <linux/libata.h>
#include "sis.h"

#define DRV_NAME	"sata_sis"
#define DRV_VERSION	"1.0"

enum {
	sis_180			= 0,
	SIS_SCR_PCI_BAR		= 5,

	/* PCI configuration registers */
	SIS_GENCTL		= 0x54, /* IDE General Control register */
	SIS_SCR_BASE		= 0xc0, /* sata0 phy SCR registers */
	SIS180_SATA1_OFS	= 0x10, /* offset from sata0->sata1 phy regs */
	SIS182_SATA1_OFS	= 0x20, /* offset from sata0->sata1 phy regs */
	SIS_PMR			= 0x90, /* port mapping register */
	SIS_PMR_COMBINED	= 0x30,

	/* random bits */
	SIS_FLAG_CFGSCR		= (1 << 30), /* host flag: SCRs via PCI cfg */

	GENCTL_IOMAPPED_SCR	= (1 << 26), /* if set, SCRs are in IO space */
};

static int sis_init_one(struct pci_dev *pdev, const struct pci_device_id *ent);
static int sis_scr_read(struct ata_link *link, unsigned int sc_reg, u32 *val);
static int sis_scr_write(struct ata_link *link, unsigned int sc_reg, u32 val);

static const struct pci_device_id sis_pci_tbl[] = {
	{ PCI_VDEVICE(SI, 0x0180), sis_180 },	/* SiS 964/180 */
	{ PCI_VDEVICE(SI, 0x0181), sis_180 },	/* SiS 964/180 */
	{ PCI_VDEVICE(SI, 0x0182), sis_180 },	/* SiS 965/965L */
	{ PCI_VDEVICE(SI, 0x0183), sis_180 },	/* SiS 965/965L */
	{ PCI_VDEVICE(SI, 0x1182), sis_180 },	/* SiS 966/680 */
	{ PCI_VDEVICE(SI, 0x1183), sis_180 },	/* SiS 966/966L/968/680 */

	{ }	/* terminate list */
};

static struct pci_driver sis_pci_driver = {
	.name			= DRV_NAME,
	.id_table		= sis_pci_tbl,
	.probe			= sis_init_one,
	.remove			= ata_pci_remove_one,
#ifdef CONFIG_PM_SLEEP
	.suspend		= ata_pci_device_suspend,
	.resume			= ata_pci_device_resume,
#endif
};

static struct scsi_host_template sis_sht = {
	ATA_BMDMA_SHT(DRV_NAME),
};

static struct ata_port_operations sis_ops = {
	.inherits		= &ata_bmdma_port_ops,
	.scr_read		= sis_scr_read,
	.scr_write		= sis_scr_write,
};

static const struct ata_port_info sis_port_info = {
	.flags		= ATA_FLAG_SATA,
	.pio_mask	= ATA_PIO4,
	.mwdma_mask	= ATA_MWDMA2,
	.udma_mask	= ATA_UDMA6,
	.port_ops	= &sis_ops,
};

MODULE_AUTHOR("Uwe Koziolek");
MODULE_DESCRIPTION("low-level driver for Silicon Integrated Systems SATA controller");
MODULE_LICENSE("GPL");
MODULE_DEVICE_TABLE(pci, sis_pci_tbl);
MODULE_VERSION(DRV_VERSION);

static unsigned int get_scr_cfg_addr(struct ata_link *link, unsigned int sc_reg)
{
	struct ata_port *ap = link->ap;
	struct pci_dev *pdev = to_pci_dev(ap->host->dev);
	unsigned int addr = SIS_SCR_BASE + (4 * sc_reg);
	u8 pmr;

	if (ap->port_no)  {
		switch (pdev->device) {
		case 0x0180:
		case 0x0181:
			pci_read_config_byte(pdev, SIS_PMR, &pmr);
			if ((pmr & SIS_PMR_COMBINED) == 0)
				addr += SIS180_SATA1_OFS;
			break;

		case 0x0182:
		case 0x0183:
		case 0x1182:
			addr += SIS182_SATA1_OFS;
			break;
		}
	}
	if (link->pmp)
		addr += 0x10;

	return addr;
}

static u32 sis_scr_cfg_read(struct ata_link *link,
			    unsigned int sc_reg, u32 *val)
{
	struct pci_dev *pdev = to_pci_dev(link->ap->host->dev);
	unsigned int cfg_addr = get_scr_cfg_addr(link, sc_reg);

	if (sc_reg == SCR_ERROR) /* doesn't exist in PCI cfg space */
		return -EINVAL;

	pci_read_config_dword(pdev, cfg_addr, val);
	return 0;
}

static int sis_scr_cfg_write(struct ata_link *link,
			     unsigned int sc_reg, u32 val)
{
	struct pci_dev *pdev = to_pci_dev(link->ap->host->dev);
	unsigned int cfg_addr = get_scr_cfg_addr(link, sc_reg);

	pci_write_config_dword(pdev, cfg_addr, val);
	return 0;
}

static int sis_scr_read(struct ata_link *link, unsigned int sc_reg, u32 *val)
{
	struct ata_port *ap = link->ap;
	void __iomem *base = ap->ioaddr.scr_addr + link->pmp * 0x10;

	if (sc_reg > SCR_CONTROL)
		return -EINVAL;

	if (ap->flags & SIS_FLAG_CFGSCR)
		return sis_scr_cfg_read(link, sc_reg, val);

	*val = ioread32(base + sc_reg * 4);
	return 0;
}

static int sis_scr_write(struct ata_link *link, unsigned int sc_reg, u32 val)
{
	struct ata_port *ap = link->ap;
	void __iomem *base = ap->ioaddr.scr_addr + link->pmp * 0x10;

	if (sc_reg > SCR_CONTROL)
		return -EINVAL;

	if (ap->flags & SIS_FLAG_CFGSCR)
		return sis_scr_cfg_write(link, sc_reg, val);

	iowrite32(val, base + (sc_reg * 4));
	return 0;
}

static int sis_init_one(struct pci_dev *pdev, const struct pci_device_id *ent)
{
	struct ata_port_info pi = sis_port_info;
	const struct ata_port_info *ppi[] = { &pi, &pi };
	struct ata_host *host;
	u32 genctl, val;
	u8 pmr;
	u8 port2_start = 0x20;
	int i, rc;

	ata_print_version_once(&pdev->dev, DRV_VERSION);

	rc = pcim_enable_device(pdev);
	if (rc)
		return rc;

	/* check and see if the SCRs are in IO space or PCI cfg space */
	pci_read_config_dword(pdev, SIS_GENCTL, &genctl);
	if ((genctl & GENCTL_IOMAPPED_SCR) == 0)
		pi.flags |= SIS_FLAG_CFGSCR;

	/* if hardware thinks SCRs are in IO space, but there are
	 * no IO resources assigned, change to PCI cfg space.
	 */
	if ((!(pi.flags & SIS_FLAG_CFGSCR)) &&
	    ((pci_resource_start(pdev, SIS_SCR_PCI_BAR) == 0) ||
	     (pci_resource_len(pdev, SIS_SCR_PCI_BAR) < 128))) {
		genctl &= ~GENCTL_IOMAPPED_SCR;
		pci_write_config_dword(pdev, SIS_GENCTL, genctl);
		pi.flags |= SIS_FLAG_CFGSCR;
	}

	pci_read_config_byte(pdev, SIS_PMR, &pmr);
	switch (ent->device) {
	case 0x0180:
	case 0x0181:

		/* The PATA-handling is provided by pata_sis */
		switch (pmr & 0x30) {
		case 0x10:
			ppi[1] = &sis_info133_for_sata;
			break;

		case 0x30:
			ppi[0] = &sis_info133_for_sata;
			break;
		}
		if ((pmr & SIS_PMR_COMBINED) == 0) {
			dev_info(&pdev->dev,
				 "Detected SiS 180/181/964 chipset in SATA mode\n");
			port2_start = 64;
		} else {
			dev_info(&pdev->dev,
				 "Detected SiS 180/181 chipset in combined mode\n");
			port2_start = 0;
			pi.flags |= ATA_FLAG_SLAVE_POSS;
		}
		break;

	case 0x0182:
	case 0x0183:
		pci_read_config_dword(pdev, 0x6C, &val);
		if (val & (1L << 31)) {
			dev_info(&pdev->dev, "Detected SiS 182/965 chipset\n");
			pi.flags |= ATA_FLAG_SLAVE_POSS;
		} else {
			dev_info(&pdev->dev, "Detected SiS 182/965L chipset\n");
		}
		break;

	case 0x1182:
		dev_info(&pdev->dev,
			 "Detected SiS 1182/966/680 SATA controller\n");
		pi.flags |= ATA_FLAG_SLAVE_POSS;
		break;

	case 0x1183:
		dev_info(&pdev->dev,
			 "Detected SiS 1183/966/966L/968/680 controller in PATA mode\n");
		ppi[0] = &sis_info133_for_sata;
		ppi[1] = &sis_info133_for_sata;
		break;
	}

	rc = ata_pci_bmdma_prepare_host(pdev, ppi, &host);
	if (rc)
		return rc;

	for (i = 0; i < 2; i++) {
		struct ata_port *ap = host->ports[i];

		if (ap->flags & ATA_FLAG_SATA &&
		    ap->flags & ATA_FLAG_SLAVE_POSS) {
			rc = ata_slave_link_init(ap);
			if (rc)
				return rc;
		}
	}

	if (!(pi.flags & SIS_FLAG_CFGSCR)) {
		void __iomem *mmio;

		rc = pcim_iomap_regions(pdev, 1 << SIS_SCR_PCI_BAR, DRV_NAME);
		if (rc)
			return rc;
		mmio = host->iomap[SIS_SCR_PCI_BAR];

		host->ports[0]->ioaddr.scr_addr = mmio;
		host->ports[1]->ioaddr.scr_addr = mmio + port2_start;
	}

	pci_set_master(pdev);
	pci_intx(pdev, 1);
	return ata_host_activate(host, pdev->irq, ata_bmdma_interrupt,
				 IRQF_SHARED, &sis_sht);
}

module_pci_driver(sis_pci_driver);
