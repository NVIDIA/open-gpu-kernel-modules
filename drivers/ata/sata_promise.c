// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  sata_promise.c - Promise SATA
 *
 *  Maintained by:  Tejun Heo <tj@kernel.org>
 *		    Mikael Pettersson
 *  		    Please ALWAYS copy linux-ide@vger.kernel.org
 *		    on emails.
 *
 *  Copyright 2003-2004 Red Hat, Inc.
 *
 *  libata documentation is available via 'make {ps|pdf}docs',
 *  as Documentation/driver-api/libata.rst
 *
 *  Hardware information only available under NDA.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/gfp.h>
#include <linux/pci.h>
#include <linux/blkdev.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/device.h>
#include <scsi/scsi.h>
#include <scsi/scsi_host.h>
#include <scsi/scsi_cmnd.h>
#include <linux/libata.h>
#include "sata_promise.h"

#define DRV_NAME	"sata_promise"
#define DRV_VERSION	"2.12"

enum {
	PDC_MAX_PORTS		= 4,
	PDC_MMIO_BAR		= 3,
	PDC_MAX_PRD		= LIBATA_MAX_PRD - 1, /* -1 for ASIC PRD bug workaround */

	/* host register offsets (from host->iomap[PDC_MMIO_BAR]) */
	PDC_INT_SEQMASK		= 0x40,	/* Mask of asserted SEQ INTs */
	PDC_FLASH_CTL		= 0x44, /* Flash control register */
	PDC_PCI_CTL		= 0x48, /* PCI control/status reg */
	PDC_SATA_PLUG_CSR	= 0x6C, /* SATA Plug control/status reg */
	PDC2_SATA_PLUG_CSR	= 0x60, /* SATAII Plug control/status reg */
	PDC_TBG_MODE		= 0x41C, /* TBG mode (not SATAII) */
	PDC_SLEW_CTL		= 0x470, /* slew rate control reg (not SATAII) */

	/* per-port ATA register offsets (from ap->ioaddr.cmd_addr) */
	PDC_FEATURE		= 0x04, /* Feature/Error reg (per port) */
	PDC_SECTOR_COUNT	= 0x08, /* Sector count reg (per port) */
	PDC_SECTOR_NUMBER	= 0x0C, /* Sector number reg (per port) */
	PDC_CYLINDER_LOW	= 0x10, /* Cylinder low reg (per port) */
	PDC_CYLINDER_HIGH	= 0x14, /* Cylinder high reg (per port) */
	PDC_DEVICE		= 0x18, /* Device/Head reg (per port) */
	PDC_COMMAND		= 0x1C, /* Command/status reg (per port) */
	PDC_ALTSTATUS		= 0x38, /* Alternate-status/device-control reg (per port) */
	PDC_PKT_SUBMIT		= 0x40, /* Command packet pointer addr */
	PDC_GLOBAL_CTL		= 0x48, /* Global control/status (per port) */
	PDC_CTLSTAT		= 0x60,	/* IDE control and status (per port) */

	/* per-port SATA register offsets (from ap->ioaddr.scr_addr) */
	PDC_SATA_ERROR		= 0x04,
	PDC_PHYMODE4		= 0x14,
	PDC_LINK_LAYER_ERRORS	= 0x6C,
	PDC_FPDMA_CTLSTAT	= 0xD8,
	PDC_INTERNAL_DEBUG_1	= 0xF8,	/* also used for PATA */
	PDC_INTERNAL_DEBUG_2	= 0xFC,	/* also used for PATA */

	/* PDC_FPDMA_CTLSTAT bit definitions */
	PDC_FPDMA_CTLSTAT_RESET			= 1 << 3,
	PDC_FPDMA_CTLSTAT_DMASETUP_INT_FLAG	= 1 << 10,
	PDC_FPDMA_CTLSTAT_SETDB_INT_FLAG	= 1 << 11,

	/* PDC_GLOBAL_CTL bit definitions */
	PDC_PH_ERR		= (1 <<  8), /* PCI error while loading packet */
	PDC_SH_ERR		= (1 <<  9), /* PCI error while loading S/G table */
	PDC_DH_ERR		= (1 << 10), /* PCI error while loading data */
	PDC2_HTO_ERR		= (1 << 12), /* host bus timeout */
	PDC2_ATA_HBA_ERR	= (1 << 13), /* error during SATA DATA FIS transmission */
	PDC2_ATA_DMA_CNT_ERR	= (1 << 14), /* DMA DATA FIS size differs from S/G count */
	PDC_OVERRUN_ERR		= (1 << 19), /* S/G byte count larger than HD requires */
	PDC_UNDERRUN_ERR	= (1 << 20), /* S/G byte count less than HD requires */
	PDC_DRIVE_ERR		= (1 << 21), /* drive error */
	PDC_PCI_SYS_ERR		= (1 << 22), /* PCI system error */
	PDC1_PCI_PARITY_ERR	= (1 << 23), /* PCI parity error (from SATA150 driver) */
	PDC1_ERR_MASK		= PDC1_PCI_PARITY_ERR,
	PDC2_ERR_MASK		= PDC2_HTO_ERR | PDC2_ATA_HBA_ERR |
				  PDC2_ATA_DMA_CNT_ERR,
	PDC_ERR_MASK		= PDC_PH_ERR | PDC_SH_ERR | PDC_DH_ERR |
				  PDC_OVERRUN_ERR | PDC_UNDERRUN_ERR |
				  PDC_DRIVE_ERR | PDC_PCI_SYS_ERR |
				  PDC1_ERR_MASK | PDC2_ERR_MASK,

	board_2037x		= 0,	/* FastTrak S150 TX2plus */
	board_2037x_pata	= 1,	/* FastTrak S150 TX2plus PATA port */
	board_20319		= 2,	/* FastTrak S150 TX4 */
	board_20619		= 3,	/* FastTrak TX4000 */
	board_2057x		= 4,	/* SATAII150 Tx2plus */
	board_2057x_pata	= 5,	/* SATAII150 Tx2plus PATA port */
	board_40518		= 6,	/* SATAII150 Tx4 */

	PDC_HAS_PATA		= (1 << 1), /* PDC20375/20575 has PATA */

	/* Sequence counter control registers bit definitions */
	PDC_SEQCNTRL_INT_MASK	= (1 << 5), /* Sequence Interrupt Mask */

	/* Feature register values */
	PDC_FEATURE_ATAPI_PIO	= 0x00, /* ATAPI data xfer by PIO */
	PDC_FEATURE_ATAPI_DMA	= 0x01, /* ATAPI data xfer by DMA */

	/* Device/Head register values */
	PDC_DEVICE_SATA		= 0xE0, /* Device/Head value for SATA devices */

	/* PDC_CTLSTAT bit definitions */
	PDC_DMA_ENABLE		= (1 << 7),
	PDC_IRQ_DISABLE		= (1 << 10),
	PDC_RESET		= (1 << 11), /* HDMA reset */

	PDC_COMMON_FLAGS	= ATA_FLAG_PIO_POLLING,

	/* ap->flags bits */
	PDC_FLAG_GEN_II		= (1 << 24),
	PDC_FLAG_SATA_PATA	= (1 << 25), /* supports SATA + PATA */
	PDC_FLAG_4_PORTS	= (1 << 26), /* 4 ports */
};

struct pdc_port_priv {
	u8			*pkt;
	dma_addr_t		pkt_dma;
};

struct pdc_host_priv {
	spinlock_t hard_reset_lock;
};

static int pdc_sata_scr_read(struct ata_link *link, unsigned int sc_reg, u32 *val);
static int pdc_sata_scr_write(struct ata_link *link, unsigned int sc_reg, u32 val);
static int pdc_ata_init_one(struct pci_dev *pdev, const struct pci_device_id *ent);
static int pdc_common_port_start(struct ata_port *ap);
static int pdc_sata_port_start(struct ata_port *ap);
static enum ata_completion_errors pdc_qc_prep(struct ata_queued_cmd *qc);
static void pdc_tf_load_mmio(struct ata_port *ap, const struct ata_taskfile *tf);
static void pdc_exec_command_mmio(struct ata_port *ap, const struct ata_taskfile *tf);
static int pdc_check_atapi_dma(struct ata_queued_cmd *qc);
static int pdc_old_sata_check_atapi_dma(struct ata_queued_cmd *qc);
static void pdc_irq_clear(struct ata_port *ap);
static unsigned int pdc_qc_issue(struct ata_queued_cmd *qc);
static void pdc_freeze(struct ata_port *ap);
static void pdc_sata_freeze(struct ata_port *ap);
static void pdc_thaw(struct ata_port *ap);
static void pdc_sata_thaw(struct ata_port *ap);
static int pdc_pata_softreset(struct ata_link *link, unsigned int *class,
			      unsigned long deadline);
static int pdc_sata_hardreset(struct ata_link *link, unsigned int *class,
			      unsigned long deadline);
static void pdc_error_handler(struct ata_port *ap);
static void pdc_post_internal_cmd(struct ata_queued_cmd *qc);
static int pdc_pata_cable_detect(struct ata_port *ap);

static struct scsi_host_template pdc_ata_sht = {
	ATA_BASE_SHT(DRV_NAME),
	.sg_tablesize		= PDC_MAX_PRD,
	.dma_boundary		= ATA_DMA_BOUNDARY,
};

static const struct ata_port_operations pdc_common_ops = {
	.inherits		= &ata_sff_port_ops,

	.sff_tf_load		= pdc_tf_load_mmio,
	.sff_exec_command	= pdc_exec_command_mmio,
	.check_atapi_dma	= pdc_check_atapi_dma,
	.qc_prep		= pdc_qc_prep,
	.qc_issue		= pdc_qc_issue,

	.sff_irq_clear		= pdc_irq_clear,
	.lost_interrupt		= ATA_OP_NULL,

	.post_internal_cmd	= pdc_post_internal_cmd,
	.error_handler		= pdc_error_handler,
};

static struct ata_port_operations pdc_sata_ops = {
	.inherits		= &pdc_common_ops,
	.cable_detect		= ata_cable_sata,
	.freeze			= pdc_sata_freeze,
	.thaw			= pdc_sata_thaw,
	.scr_read		= pdc_sata_scr_read,
	.scr_write		= pdc_sata_scr_write,
	.port_start		= pdc_sata_port_start,
	.hardreset		= pdc_sata_hardreset,
};

/* First-generation chips need a more restrictive ->check_atapi_dma op,
   and ->freeze/thaw that ignore the hotplug controls. */
static struct ata_port_operations pdc_old_sata_ops = {
	.inherits		= &pdc_sata_ops,
	.freeze			= pdc_freeze,
	.thaw			= pdc_thaw,
	.check_atapi_dma	= pdc_old_sata_check_atapi_dma,
};

static struct ata_port_operations pdc_pata_ops = {
	.inherits		= &pdc_common_ops,
	.cable_detect		= pdc_pata_cable_detect,
	.freeze			= pdc_freeze,
	.thaw			= pdc_thaw,
	.port_start		= pdc_common_port_start,
	.softreset		= pdc_pata_softreset,
};

static const struct ata_port_info pdc_port_info[] = {
	[board_2037x] =
	{
		.flags		= PDC_COMMON_FLAGS | ATA_FLAG_SATA |
				  PDC_FLAG_SATA_PATA,
		.pio_mask	= ATA_PIO4,
		.mwdma_mask	= ATA_MWDMA2,
		.udma_mask	= ATA_UDMA6,
		.port_ops	= &pdc_old_sata_ops,
	},

	[board_2037x_pata] =
	{
		.flags		= PDC_COMMON_FLAGS | ATA_FLAG_SLAVE_POSS,
		.pio_mask	= ATA_PIO4,
		.mwdma_mask	= ATA_MWDMA2,
		.udma_mask	= ATA_UDMA6,
		.port_ops	= &pdc_pata_ops,
	},

	[board_20319] =
	{
		.flags		= PDC_COMMON_FLAGS | ATA_FLAG_SATA |
				  PDC_FLAG_4_PORTS,
		.pio_mask	= ATA_PIO4,
		.mwdma_mask	= ATA_MWDMA2,
		.udma_mask	= ATA_UDMA6,
		.port_ops	= &pdc_old_sata_ops,
	},

	[board_20619] =
	{
		.flags		= PDC_COMMON_FLAGS | ATA_FLAG_SLAVE_POSS |
				  PDC_FLAG_4_PORTS,
		.pio_mask	= ATA_PIO4,
		.mwdma_mask	= ATA_MWDMA2,
		.udma_mask	= ATA_UDMA6,
		.port_ops	= &pdc_pata_ops,
	},

	[board_2057x] =
	{
		.flags		= PDC_COMMON_FLAGS | ATA_FLAG_SATA |
				  PDC_FLAG_GEN_II | PDC_FLAG_SATA_PATA,
		.pio_mask	= ATA_PIO4,
		.mwdma_mask	= ATA_MWDMA2,
		.udma_mask	= ATA_UDMA6,
		.port_ops	= &pdc_sata_ops,
	},

	[board_2057x_pata] =
	{
		.flags		= PDC_COMMON_FLAGS | ATA_FLAG_SLAVE_POSS |
				  PDC_FLAG_GEN_II,
		.pio_mask	= ATA_PIO4,
		.mwdma_mask	= ATA_MWDMA2,
		.udma_mask	= ATA_UDMA6,
		.port_ops	= &pdc_pata_ops,
	},

	[board_40518] =
	{
		.flags		= PDC_COMMON_FLAGS | ATA_FLAG_SATA |
				  PDC_FLAG_GEN_II | PDC_FLAG_4_PORTS,
		.pio_mask	= ATA_PIO4,
		.mwdma_mask	= ATA_MWDMA2,
		.udma_mask	= ATA_UDMA6,
		.port_ops	= &pdc_sata_ops,
	},
};

static const struct pci_device_id pdc_ata_pci_tbl[] = {
	{ PCI_VDEVICE(PROMISE, 0x3371), board_2037x },
	{ PCI_VDEVICE(PROMISE, 0x3373), board_2037x },
	{ PCI_VDEVICE(PROMISE, 0x3375), board_2037x },
	{ PCI_VDEVICE(PROMISE, 0x3376), board_2037x },
	{ PCI_VDEVICE(PROMISE, 0x3570), board_2057x },
	{ PCI_VDEVICE(PROMISE, 0x3571), board_2057x },
	{ PCI_VDEVICE(PROMISE, 0x3574), board_2057x },
	{ PCI_VDEVICE(PROMISE, 0x3577), board_2057x },
	{ PCI_VDEVICE(PROMISE, 0x3d73), board_2057x },
	{ PCI_VDEVICE(PROMISE, 0x3d75), board_2057x },

	{ PCI_VDEVICE(PROMISE, 0x3318), board_20319 },
	{ PCI_VDEVICE(PROMISE, 0x3319), board_20319 },
	{ PCI_VDEVICE(PROMISE, 0x3515), board_40518 },
	{ PCI_VDEVICE(PROMISE, 0x3519), board_40518 },
	{ PCI_VDEVICE(PROMISE, 0x3d17), board_40518 },
	{ PCI_VDEVICE(PROMISE, 0x3d18), board_40518 },

	{ PCI_VDEVICE(PROMISE, 0x6629), board_20619 },

	{ }	/* terminate list */
};

static struct pci_driver pdc_ata_pci_driver = {
	.name			= DRV_NAME,
	.id_table		= pdc_ata_pci_tbl,
	.probe			= pdc_ata_init_one,
	.remove			= ata_pci_remove_one,
};

static int pdc_common_port_start(struct ata_port *ap)
{
	struct device *dev = ap->host->dev;
	struct pdc_port_priv *pp;
	int rc;

	/* we use the same prd table as bmdma, allocate it */
	rc = ata_bmdma_port_start(ap);
	if (rc)
		return rc;

	pp = devm_kzalloc(dev, sizeof(*pp), GFP_KERNEL);
	if (!pp)
		return -ENOMEM;

	pp->pkt = dmam_alloc_coherent(dev, 128, &pp->pkt_dma, GFP_KERNEL);
	if (!pp->pkt)
		return -ENOMEM;

	ap->private_data = pp;

	return 0;
}

static int pdc_sata_port_start(struct ata_port *ap)
{
	int rc;

	rc = pdc_common_port_start(ap);
	if (rc)
		return rc;

	/* fix up PHYMODE4 align timing */
	if (ap->flags & PDC_FLAG_GEN_II) {
		void __iomem *sata_mmio = ap->ioaddr.scr_addr;
		unsigned int tmp;

		tmp = readl(sata_mmio + PDC_PHYMODE4);
		tmp = (tmp & ~3) | 1;	/* set bits 1:0 = 0:1 */
		writel(tmp, sata_mmio + PDC_PHYMODE4);
	}

	return 0;
}

static void pdc_fpdma_clear_interrupt_flag(struct ata_port *ap)
{
	void __iomem *sata_mmio = ap->ioaddr.scr_addr;
	u32 tmp;

	tmp = readl(sata_mmio + PDC_FPDMA_CTLSTAT);
	tmp |= PDC_FPDMA_CTLSTAT_DMASETUP_INT_FLAG;
	tmp |= PDC_FPDMA_CTLSTAT_SETDB_INT_FLAG;

	/* It's not allowed to write to the entire FPDMA_CTLSTAT register
	   when NCQ is running. So do a byte-sized write to bits 10 and 11. */
	writeb(tmp >> 8, sata_mmio + PDC_FPDMA_CTLSTAT + 1);
	readb(sata_mmio + PDC_FPDMA_CTLSTAT + 1); /* flush */
}

static void pdc_fpdma_reset(struct ata_port *ap)
{
	void __iomem *sata_mmio = ap->ioaddr.scr_addr;
	u8 tmp;

	tmp = (u8)readl(sata_mmio + PDC_FPDMA_CTLSTAT);
	tmp &= 0x7F;
	tmp |= PDC_FPDMA_CTLSTAT_RESET;
	writeb(tmp, sata_mmio + PDC_FPDMA_CTLSTAT);
	readl(sata_mmio + PDC_FPDMA_CTLSTAT); /* flush */
	udelay(100);
	tmp &= ~PDC_FPDMA_CTLSTAT_RESET;
	writeb(tmp, sata_mmio + PDC_FPDMA_CTLSTAT);
	readl(sata_mmio + PDC_FPDMA_CTLSTAT); /* flush */

	pdc_fpdma_clear_interrupt_flag(ap);
}

static void pdc_not_at_command_packet_phase(struct ata_port *ap)
{
	void __iomem *sata_mmio = ap->ioaddr.scr_addr;
	unsigned int i;
	u32 tmp;

	/* check not at ASIC packet command phase */
	for (i = 0; i < 100; ++i) {
		writel(0, sata_mmio + PDC_INTERNAL_DEBUG_1);
		tmp = readl(sata_mmio + PDC_INTERNAL_DEBUG_2);
		if ((tmp & 0xF) != 1)
			break;
		udelay(100);
	}
}

static void pdc_clear_internal_debug_record_error_register(struct ata_port *ap)
{
	void __iomem *sata_mmio = ap->ioaddr.scr_addr;

	writel(0xffffffff, sata_mmio + PDC_SATA_ERROR);
	writel(0xffff0000, sata_mmio + PDC_LINK_LAYER_ERRORS);
}

static void pdc_reset_port(struct ata_port *ap)
{
	void __iomem *ata_ctlstat_mmio = ap->ioaddr.cmd_addr + PDC_CTLSTAT;
	unsigned int i;
	u32 tmp;

	if (ap->flags & PDC_FLAG_GEN_II)
		pdc_not_at_command_packet_phase(ap);

	tmp = readl(ata_ctlstat_mmio);
	tmp |= PDC_RESET;
	writel(tmp, ata_ctlstat_mmio);

	for (i = 11; i > 0; i--) {
		tmp = readl(ata_ctlstat_mmio);
		if (tmp & PDC_RESET)
			break;

		udelay(100);

		tmp |= PDC_RESET;
		writel(tmp, ata_ctlstat_mmio);
	}

	tmp &= ~PDC_RESET;
	writel(tmp, ata_ctlstat_mmio);
	readl(ata_ctlstat_mmio);	/* flush */

	if (sata_scr_valid(&ap->link) && (ap->flags & PDC_FLAG_GEN_II)) {
		pdc_fpdma_reset(ap);
		pdc_clear_internal_debug_record_error_register(ap);
	}
}

static int pdc_pata_cable_detect(struct ata_port *ap)
{
	u8 tmp;
	void __iomem *ata_mmio = ap->ioaddr.cmd_addr;

	tmp = readb(ata_mmio + PDC_CTLSTAT + 3);
	if (tmp & 0x01)
		return ATA_CBL_PATA40;
	return ATA_CBL_PATA80;
}

static int pdc_sata_scr_read(struct ata_link *link,
			     unsigned int sc_reg, u32 *val)
{
	if (sc_reg > SCR_CONTROL)
		return -EINVAL;
	*val = readl(link->ap->ioaddr.scr_addr + (sc_reg * 4));
	return 0;
}

static int pdc_sata_scr_write(struct ata_link *link,
			      unsigned int sc_reg, u32 val)
{
	if (sc_reg > SCR_CONTROL)
		return -EINVAL;
	writel(val, link->ap->ioaddr.scr_addr + (sc_reg * 4));
	return 0;
}

static void pdc_atapi_pkt(struct ata_queued_cmd *qc)
{
	struct ata_port *ap = qc->ap;
	dma_addr_t sg_table = ap->bmdma_prd_dma;
	unsigned int cdb_len = qc->dev->cdb_len;
	u8 *cdb = qc->cdb;
	struct pdc_port_priv *pp = ap->private_data;
	u8 *buf = pp->pkt;
	__le32 *buf32 = (__le32 *) buf;
	unsigned int dev_sel, feature;

	/* set control bits (byte 0), zero delay seq id (byte 3),
	 * and seq id (byte 2)
	 */
	switch (qc->tf.protocol) {
	case ATAPI_PROT_DMA:
		if (!(qc->tf.flags & ATA_TFLAG_WRITE))
			buf32[0] = cpu_to_le32(PDC_PKT_READ);
		else
			buf32[0] = 0;
		break;
	case ATAPI_PROT_NODATA:
		buf32[0] = cpu_to_le32(PDC_PKT_NODATA);
		break;
	default:
		BUG();
		break;
	}
	buf32[1] = cpu_to_le32(sg_table);	/* S/G table addr */
	buf32[2] = 0;				/* no next-packet */

	/* select drive */
	if (sata_scr_valid(&ap->link))
		dev_sel = PDC_DEVICE_SATA;
	else
		dev_sel = qc->tf.device;

	buf[12] = (1 << 5) | ATA_REG_DEVICE;
	buf[13] = dev_sel;
	buf[14] = (1 << 5) | ATA_REG_DEVICE | PDC_PKT_CLEAR_BSY;
	buf[15] = dev_sel; /* once more, waiting for BSY to clear */

	buf[16] = (1 << 5) | ATA_REG_NSECT;
	buf[17] = qc->tf.nsect;
	buf[18] = (1 << 5) | ATA_REG_LBAL;
	buf[19] = qc->tf.lbal;

	/* set feature and byte counter registers */
	if (qc->tf.protocol != ATAPI_PROT_DMA)
		feature = PDC_FEATURE_ATAPI_PIO;
	else
		feature = PDC_FEATURE_ATAPI_DMA;

	buf[20] = (1 << 5) | ATA_REG_FEATURE;
	buf[21] = feature;
	buf[22] = (1 << 5) | ATA_REG_BYTEL;
	buf[23] = qc->tf.lbam;
	buf[24] = (1 << 5) | ATA_REG_BYTEH;
	buf[25] = qc->tf.lbah;

	/* send ATAPI packet command 0xA0 */
	buf[26] = (1 << 5) | ATA_REG_CMD;
	buf[27] = qc->tf.command;

	/* select drive and check DRQ */
	buf[28] = (1 << 5) | ATA_REG_DEVICE | PDC_PKT_WAIT_DRDY;
	buf[29] = dev_sel;

	/* we can represent cdb lengths 2/4/6/8/10/12/14/16 */
	BUG_ON(cdb_len & ~0x1E);

	/* append the CDB as the final part */
	buf[30] = (((cdb_len >> 1) & 7) << 5) | ATA_REG_DATA | PDC_LAST_REG;
	memcpy(buf+31, cdb, cdb_len);
}

/**
 *	pdc_fill_sg - Fill PCI IDE PRD table
 *	@qc: Metadata associated with taskfile to be transferred
 *
 *	Fill PCI IDE PRD (scatter-gather) table with segments
 *	associated with the current disk command.
 *	Make sure hardware does not choke on it.
 *
 *	LOCKING:
 *	spin_lock_irqsave(host lock)
 *
 */
static void pdc_fill_sg(struct ata_queued_cmd *qc)
{
	struct ata_port *ap = qc->ap;
	struct ata_bmdma_prd *prd = ap->bmdma_prd;
	struct scatterlist *sg;
	const u32 SG_COUNT_ASIC_BUG = 41*4;
	unsigned int si, idx;
	u32 len;

	if (!(qc->flags & ATA_QCFLAG_DMAMAP))
		return;

	idx = 0;
	for_each_sg(qc->sg, sg, qc->n_elem, si) {
		u32 addr, offset;
		u32 sg_len;

		/* determine if physical DMA addr spans 64K boundary.
		 * Note h/w doesn't support 64-bit, so we unconditionally
		 * truncate dma_addr_t to u32.
		 */
		addr = (u32) sg_dma_address(sg);
		sg_len = sg_dma_len(sg);

		while (sg_len) {
			offset = addr & 0xffff;
			len = sg_len;
			if ((offset + sg_len) > 0x10000)
				len = 0x10000 - offset;

			prd[idx].addr = cpu_to_le32(addr);
			prd[idx].flags_len = cpu_to_le32(len & 0xffff);
			VPRINTK("PRD[%u] = (0x%X, 0x%X)\n", idx, addr, len);

			idx++;
			sg_len -= len;
			addr += len;
		}
	}

	len = le32_to_cpu(prd[idx - 1].flags_len);

	if (len > SG_COUNT_ASIC_BUG) {
		u32 addr;

		VPRINTK("Splitting last PRD.\n");

		addr = le32_to_cpu(prd[idx - 1].addr);
		prd[idx - 1].flags_len = cpu_to_le32(len - SG_COUNT_ASIC_BUG);
		VPRINTK("PRD[%u] = (0x%X, 0x%X)\n", idx - 1, addr, SG_COUNT_ASIC_BUG);

		addr = addr + len - SG_COUNT_ASIC_BUG;
		len = SG_COUNT_ASIC_BUG;
		prd[idx].addr = cpu_to_le32(addr);
		prd[idx].flags_len = cpu_to_le32(len);
		VPRINTK("PRD[%u] = (0x%X, 0x%X)\n", idx, addr, len);

		idx++;
	}

	prd[idx - 1].flags_len |= cpu_to_le32(ATA_PRD_EOT);
}

static enum ata_completion_errors pdc_qc_prep(struct ata_queued_cmd *qc)
{
	struct pdc_port_priv *pp = qc->ap->private_data;
	unsigned int i;

	VPRINTK("ENTER\n");

	switch (qc->tf.protocol) {
	case ATA_PROT_DMA:
		pdc_fill_sg(qc);
		fallthrough;
	case ATA_PROT_NODATA:
		i = pdc_pkt_header(&qc->tf, qc->ap->bmdma_prd_dma,
				   qc->dev->devno, pp->pkt);
		if (qc->tf.flags & ATA_TFLAG_LBA48)
			i = pdc_prep_lba48(&qc->tf, pp->pkt, i);
		else
			i = pdc_prep_lba28(&qc->tf, pp->pkt, i);
		pdc_pkt_footer(&qc->tf, pp->pkt, i);
		break;
	case ATAPI_PROT_PIO:
		pdc_fill_sg(qc);
		break;
	case ATAPI_PROT_DMA:
		pdc_fill_sg(qc);
		fallthrough;
	case ATAPI_PROT_NODATA:
		pdc_atapi_pkt(qc);
		break;
	default:
		break;
	}

	return AC_ERR_OK;
}

static int pdc_is_sataii_tx4(unsigned long flags)
{
	const unsigned long mask = PDC_FLAG_GEN_II | PDC_FLAG_4_PORTS;
	return (flags & mask) == mask;
}

static unsigned int pdc_port_no_to_ata_no(unsigned int port_no,
					  int is_sataii_tx4)
{
	static const unsigned char sataii_tx4_port_remap[4] = { 3, 1, 0, 2};
	return is_sataii_tx4 ? sataii_tx4_port_remap[port_no] : port_no;
}

static unsigned int pdc_sata_nr_ports(const struct ata_port *ap)
{
	return (ap->flags & PDC_FLAG_4_PORTS) ? 4 : 2;
}

static unsigned int pdc_sata_ata_port_to_ata_no(const struct ata_port *ap)
{
	const struct ata_host *host = ap->host;
	unsigned int nr_ports = pdc_sata_nr_ports(ap);
	unsigned int i;

	for (i = 0; i < nr_ports && host->ports[i] != ap; ++i)
		;
	BUG_ON(i >= nr_ports);
	return pdc_port_no_to_ata_no(i, pdc_is_sataii_tx4(ap->flags));
}

static void pdc_freeze(struct ata_port *ap)
{
	void __iomem *ata_mmio = ap->ioaddr.cmd_addr;
	u32 tmp;

	tmp = readl(ata_mmio + PDC_CTLSTAT);
	tmp |= PDC_IRQ_DISABLE;
	tmp &= ~PDC_DMA_ENABLE;
	writel(tmp, ata_mmio + PDC_CTLSTAT);
	readl(ata_mmio + PDC_CTLSTAT); /* flush */
}

static void pdc_sata_freeze(struct ata_port *ap)
{
	struct ata_host *host = ap->host;
	void __iomem *host_mmio = host->iomap[PDC_MMIO_BAR];
	unsigned int hotplug_offset = PDC2_SATA_PLUG_CSR;
	unsigned int ata_no = pdc_sata_ata_port_to_ata_no(ap);
	u32 hotplug_status;

	/* Disable hotplug events on this port.
	 *
	 * Locking:
	 * 1) hotplug register accesses must be serialised via host->lock
	 * 2) ap->lock == &ap->host->lock
	 * 3) ->freeze() and ->thaw() are called with ap->lock held
	 */
	hotplug_status = readl(host_mmio + hotplug_offset);
	hotplug_status |= 0x11 << (ata_no + 16);
	writel(hotplug_status, host_mmio + hotplug_offset);
	readl(host_mmio + hotplug_offset); /* flush */

	pdc_freeze(ap);
}

static void pdc_thaw(struct ata_port *ap)
{
	void __iomem *ata_mmio = ap->ioaddr.cmd_addr;
	u32 tmp;

	/* clear IRQ */
	readl(ata_mmio + PDC_COMMAND);

	/* turn IRQ back on */
	tmp = readl(ata_mmio + PDC_CTLSTAT);
	tmp &= ~PDC_IRQ_DISABLE;
	writel(tmp, ata_mmio + PDC_CTLSTAT);
	readl(ata_mmio + PDC_CTLSTAT); /* flush */
}

static void pdc_sata_thaw(struct ata_port *ap)
{
	struct ata_host *host = ap->host;
	void __iomem *host_mmio = host->iomap[PDC_MMIO_BAR];
	unsigned int hotplug_offset = PDC2_SATA_PLUG_CSR;
	unsigned int ata_no = pdc_sata_ata_port_to_ata_no(ap);
	u32 hotplug_status;

	pdc_thaw(ap);

	/* Enable hotplug events on this port.
	 * Locking: see pdc_sata_freeze().
	 */
	hotplug_status = readl(host_mmio + hotplug_offset);
	hotplug_status |= 0x11 << ata_no;
	hotplug_status &= ~(0x11 << (ata_no + 16));
	writel(hotplug_status, host_mmio + hotplug_offset);
	readl(host_mmio + hotplug_offset); /* flush */
}

static int pdc_pata_softreset(struct ata_link *link, unsigned int *class,
			      unsigned long deadline)
{
	pdc_reset_port(link->ap);
	return ata_sff_softreset(link, class, deadline);
}

static unsigned int pdc_ata_port_to_ata_no(const struct ata_port *ap)
{
	void __iomem *ata_mmio = ap->ioaddr.cmd_addr;
	void __iomem *host_mmio = ap->host->iomap[PDC_MMIO_BAR];

	/* ata_mmio == host_mmio + 0x200 + ata_no * 0x80 */
	return (ata_mmio - host_mmio - 0x200) / 0x80;
}

static void pdc_hard_reset_port(struct ata_port *ap)
{
	void __iomem *host_mmio = ap->host->iomap[PDC_MMIO_BAR];
	void __iomem *pcictl_b1_mmio = host_mmio + PDC_PCI_CTL + 1;
	unsigned int ata_no = pdc_ata_port_to_ata_no(ap);
	struct pdc_host_priv *hpriv = ap->host->private_data;
	u8 tmp;

	spin_lock(&hpriv->hard_reset_lock);

	tmp = readb(pcictl_b1_mmio);
	tmp &= ~(0x10 << ata_no);
	writeb(tmp, pcictl_b1_mmio);
	readb(pcictl_b1_mmio); /* flush */
	udelay(100);
	tmp |= (0x10 << ata_no);
	writeb(tmp, pcictl_b1_mmio);
	readb(pcictl_b1_mmio); /* flush */

	spin_unlock(&hpriv->hard_reset_lock);
}

static int pdc_sata_hardreset(struct ata_link *link, unsigned int *class,
			      unsigned long deadline)
{
	if (link->ap->flags & PDC_FLAG_GEN_II)
		pdc_not_at_command_packet_phase(link->ap);
	/* hotplug IRQs should have been masked by pdc_sata_freeze() */
	pdc_hard_reset_port(link->ap);
	pdc_reset_port(link->ap);

	/* sata_promise can't reliably acquire the first D2H Reg FIS
	 * after hardreset.  Do non-waiting hardreset and request
	 * follow-up SRST.
	 */
	return sata_std_hardreset(link, class, deadline);
}

static void pdc_error_handler(struct ata_port *ap)
{
	if (!(ap->pflags & ATA_PFLAG_FROZEN))
		pdc_reset_port(ap);

	ata_sff_error_handler(ap);
}

static void pdc_post_internal_cmd(struct ata_queued_cmd *qc)
{
	struct ata_port *ap = qc->ap;

	/* make DMA engine forget about the failed command */
	if (qc->flags & ATA_QCFLAG_FAILED)
		pdc_reset_port(ap);
}

static void pdc_error_intr(struct ata_port *ap, struct ata_queued_cmd *qc,
			   u32 port_status, u32 err_mask)
{
	struct ata_eh_info *ehi = &ap->link.eh_info;
	unsigned int ac_err_mask = 0;

	ata_ehi_clear_desc(ehi);
	ata_ehi_push_desc(ehi, "port_status 0x%08x", port_status);
	port_status &= err_mask;

	if (port_status & PDC_DRIVE_ERR)
		ac_err_mask |= AC_ERR_DEV;
	if (port_status & (PDC_OVERRUN_ERR | PDC_UNDERRUN_ERR))
		ac_err_mask |= AC_ERR_OTHER;
	if (port_status & (PDC2_ATA_HBA_ERR | PDC2_ATA_DMA_CNT_ERR))
		ac_err_mask |= AC_ERR_ATA_BUS;
	if (port_status & (PDC_PH_ERR | PDC_SH_ERR | PDC_DH_ERR | PDC2_HTO_ERR
			   | PDC_PCI_SYS_ERR | PDC1_PCI_PARITY_ERR))
		ac_err_mask |= AC_ERR_HOST_BUS;

	if (sata_scr_valid(&ap->link)) {
		u32 serror;

		pdc_sata_scr_read(&ap->link, SCR_ERROR, &serror);
		ehi->serror |= serror;
	}

	qc->err_mask |= ac_err_mask;

	pdc_reset_port(ap);

	ata_port_abort(ap);
}

static unsigned int pdc_host_intr(struct ata_port *ap,
				  struct ata_queued_cmd *qc)
{
	unsigned int handled = 0;
	void __iomem *ata_mmio = ap->ioaddr.cmd_addr;
	u32 port_status, err_mask;

	err_mask = PDC_ERR_MASK;
	if (ap->flags & PDC_FLAG_GEN_II)
		err_mask &= ~PDC1_ERR_MASK;
	else
		err_mask &= ~PDC2_ERR_MASK;
	port_status = readl(ata_mmio + PDC_GLOBAL_CTL);
	if (unlikely(port_status & err_mask)) {
		pdc_error_intr(ap, qc, port_status, err_mask);
		return 1;
	}

	switch (qc->tf.protocol) {
	case ATA_PROT_DMA:
	case ATA_PROT_NODATA:
	case ATAPI_PROT_DMA:
	case ATAPI_PROT_NODATA:
		qc->err_mask |= ac_err_mask(ata_wait_idle(ap));
		ata_qc_complete(qc);
		handled = 1;
		break;
	default:
		ap->stats.idle_irq++;
		break;
	}

	return handled;
}

static void pdc_irq_clear(struct ata_port *ap)
{
	void __iomem *ata_mmio = ap->ioaddr.cmd_addr;

	readl(ata_mmio + PDC_COMMAND);
}

static irqreturn_t pdc_interrupt(int irq, void *dev_instance)
{
	struct ata_host *host = dev_instance;
	struct ata_port *ap;
	u32 mask = 0;
	unsigned int i, tmp;
	unsigned int handled = 0;
	void __iomem *host_mmio;
	unsigned int hotplug_offset, ata_no;
	u32 hotplug_status;
	int is_sataii_tx4;

	VPRINTK("ENTER\n");

	if (!host || !host->iomap[PDC_MMIO_BAR]) {
		VPRINTK("QUICK EXIT\n");
		return IRQ_NONE;
	}

	host_mmio = host->iomap[PDC_MMIO_BAR];

	spin_lock(&host->lock);

	/* read and clear hotplug flags for all ports */
	if (host->ports[0]->flags & PDC_FLAG_GEN_II) {
		hotplug_offset = PDC2_SATA_PLUG_CSR;
		hotplug_status = readl(host_mmio + hotplug_offset);
		if (hotplug_status & 0xff)
			writel(hotplug_status | 0xff, host_mmio + hotplug_offset);
		hotplug_status &= 0xff;	/* clear uninteresting bits */
	} else
		hotplug_status = 0;

	/* reading should also clear interrupts */
	mask = readl(host_mmio + PDC_INT_SEQMASK);

	if (mask == 0xffffffff && hotplug_status == 0) {
		VPRINTK("QUICK EXIT 2\n");
		goto done_irq;
	}

	mask &= 0xffff;		/* only 16 SEQIDs possible */
	if (mask == 0 && hotplug_status == 0) {
		VPRINTK("QUICK EXIT 3\n");
		goto done_irq;
	}

	writel(mask, host_mmio + PDC_INT_SEQMASK);

	is_sataii_tx4 = pdc_is_sataii_tx4(host->ports[0]->flags);

	for (i = 0; i < host->n_ports; i++) {
		VPRINTK("port %u\n", i);
		ap = host->ports[i];

		/* check for a plug or unplug event */
		ata_no = pdc_port_no_to_ata_no(i, is_sataii_tx4);
		tmp = hotplug_status & (0x11 << ata_no);
		if (tmp) {
			struct ata_eh_info *ehi = &ap->link.eh_info;
			ata_ehi_clear_desc(ehi);
			ata_ehi_hotplugged(ehi);
			ata_ehi_push_desc(ehi, "hotplug_status %#x", tmp);
			ata_port_freeze(ap);
			++handled;
			continue;
		}

		/* check for a packet interrupt */
		tmp = mask & (1 << (i + 1));
		if (tmp) {
			struct ata_queued_cmd *qc;

			qc = ata_qc_from_tag(ap, ap->link.active_tag);
			if (qc && (!(qc->tf.flags & ATA_TFLAG_POLLING)))
				handled += pdc_host_intr(ap, qc);
		}
	}

	VPRINTK("EXIT\n");

done_irq:
	spin_unlock(&host->lock);
	return IRQ_RETVAL(handled);
}

static void pdc_packet_start(struct ata_queued_cmd *qc)
{
	struct ata_port *ap = qc->ap;
	struct pdc_port_priv *pp = ap->private_data;
	void __iomem *host_mmio = ap->host->iomap[PDC_MMIO_BAR];
	void __iomem *ata_mmio = ap->ioaddr.cmd_addr;
	unsigned int port_no = ap->port_no;
	u8 seq = (u8) (port_no + 1);

	VPRINTK("ENTER, ap %p\n", ap);

	writel(0x00000001, host_mmio + (seq * 4));
	readl(host_mmio + (seq * 4));	/* flush */

	pp->pkt[2] = seq;
	wmb();			/* flush PRD, pkt writes */
	writel(pp->pkt_dma, ata_mmio + PDC_PKT_SUBMIT);
	readl(ata_mmio + PDC_PKT_SUBMIT); /* flush */
}

static unsigned int pdc_qc_issue(struct ata_queued_cmd *qc)
{
	switch (qc->tf.protocol) {
	case ATAPI_PROT_NODATA:
		if (qc->dev->flags & ATA_DFLAG_CDB_INTR)
			break;
		fallthrough;
	case ATA_PROT_NODATA:
		if (qc->tf.flags & ATA_TFLAG_POLLING)
			break;
		fallthrough;
	case ATAPI_PROT_DMA:
	case ATA_PROT_DMA:
		pdc_packet_start(qc);
		return 0;
	default:
		break;
	}
	return ata_sff_qc_issue(qc);
}

static void pdc_tf_load_mmio(struct ata_port *ap, const struct ata_taskfile *tf)
{
	WARN_ON(tf->protocol == ATA_PROT_DMA || tf->protocol == ATAPI_PROT_DMA);
	ata_sff_tf_load(ap, tf);
}

static void pdc_exec_command_mmio(struct ata_port *ap,
				  const struct ata_taskfile *tf)
{
	WARN_ON(tf->protocol == ATA_PROT_DMA || tf->protocol == ATAPI_PROT_DMA);
	ata_sff_exec_command(ap, tf);
}

static int pdc_check_atapi_dma(struct ata_queued_cmd *qc)
{
	u8 *scsicmd = qc->scsicmd->cmnd;
	int pio = 1; /* atapi dma off by default */

	/* Whitelist commands that may use DMA. */
	switch (scsicmd[0]) {
	case WRITE_12:
	case WRITE_10:
	case WRITE_6:
	case READ_12:
	case READ_10:
	case READ_6:
	case 0xad: /* READ_DVD_STRUCTURE */
	case 0xbe: /* READ_CD */
		pio = 0;
	}
	/* -45150 (FFFF4FA2) to -1 (FFFFFFFF) shall use PIO mode */
	if (scsicmd[0] == WRITE_10) {
		unsigned int lba =
			(scsicmd[2] << 24) |
			(scsicmd[3] << 16) |
			(scsicmd[4] << 8) |
			scsicmd[5];
		if (lba >= 0xFFFF4FA2)
			pio = 1;
	}
	return pio;
}

static int pdc_old_sata_check_atapi_dma(struct ata_queued_cmd *qc)
{
	/* First generation chips cannot use ATAPI DMA on SATA ports */
	return 1;
}

static void pdc_ata_setup_port(struct ata_port *ap,
			       void __iomem *base, void __iomem *scr_addr)
{
	ap->ioaddr.cmd_addr		= base;
	ap->ioaddr.data_addr		= base;
	ap->ioaddr.feature_addr		=
	ap->ioaddr.error_addr		= base + 0x4;
	ap->ioaddr.nsect_addr		= base + 0x8;
	ap->ioaddr.lbal_addr		= base + 0xc;
	ap->ioaddr.lbam_addr		= base + 0x10;
	ap->ioaddr.lbah_addr		= base + 0x14;
	ap->ioaddr.device_addr		= base + 0x18;
	ap->ioaddr.command_addr		=
	ap->ioaddr.status_addr		= base + 0x1c;
	ap->ioaddr.altstatus_addr	=
	ap->ioaddr.ctl_addr		= base + 0x38;
	ap->ioaddr.scr_addr		= scr_addr;
}

static void pdc_host_init(struct ata_host *host)
{
	void __iomem *host_mmio = host->iomap[PDC_MMIO_BAR];
	int is_gen2 = host->ports[0]->flags & PDC_FLAG_GEN_II;
	int hotplug_offset;
	u32 tmp;

	if (is_gen2)
		hotplug_offset = PDC2_SATA_PLUG_CSR;
	else
		hotplug_offset = PDC_SATA_PLUG_CSR;

	/*
	 * Except for the hotplug stuff, this is voodoo from the
	 * Promise driver.  Label this entire section
	 * "TODO: figure out why we do this"
	 */

	/* enable BMR_BURST, maybe change FIFO_SHD to 8 dwords */
	tmp = readl(host_mmio + PDC_FLASH_CTL);
	tmp |= 0x02000;	/* bit 13 (enable bmr burst) */
	if (!is_gen2)
		tmp |= 0x10000;	/* bit 16 (fifo threshold at 8 dw) */
	writel(tmp, host_mmio + PDC_FLASH_CTL);

	/* clear plug/unplug flags for all ports */
	tmp = readl(host_mmio + hotplug_offset);
	writel(tmp | 0xff, host_mmio + hotplug_offset);

	tmp = readl(host_mmio + hotplug_offset);
	if (is_gen2)	/* unmask plug/unplug ints */
		writel(tmp & ~0xff0000, host_mmio + hotplug_offset);
	else		/* mask plug/unplug ints */
		writel(tmp | 0xff0000, host_mmio + hotplug_offset);

	/* don't initialise TBG or SLEW on 2nd generation chips */
	if (is_gen2)
		return;

	/* reduce TBG clock to 133 Mhz. */
	tmp = readl(host_mmio + PDC_TBG_MODE);
	tmp &= ~0x30000; /* clear bit 17, 16*/
	tmp |= 0x10000;  /* set bit 17:16 = 0:1 */
	writel(tmp, host_mmio + PDC_TBG_MODE);

	readl(host_mmio + PDC_TBG_MODE);	/* flush */
	msleep(10);

	/* adjust slew rate control register. */
	tmp = readl(host_mmio + PDC_SLEW_CTL);
	tmp &= 0xFFFFF03F; /* clear bit 11 ~ 6 */
	tmp  |= 0x00000900; /* set bit 11-9 = 100b , bit 8-6 = 100 */
	writel(tmp, host_mmio + PDC_SLEW_CTL);
}

static int pdc_ata_init_one(struct pci_dev *pdev,
			    const struct pci_device_id *ent)
{
	const struct ata_port_info *pi = &pdc_port_info[ent->driver_data];
	const struct ata_port_info *ppi[PDC_MAX_PORTS];
	struct ata_host *host;
	struct pdc_host_priv *hpriv;
	void __iomem *host_mmio;
	int n_ports, i, rc;
	int is_sataii_tx4;

	ata_print_version_once(&pdev->dev, DRV_VERSION);

	/* enable and acquire resources */
	rc = pcim_enable_device(pdev);
	if (rc)
		return rc;

	rc = pcim_iomap_regions(pdev, 1 << PDC_MMIO_BAR, DRV_NAME);
	if (rc == -EBUSY)
		pcim_pin_device(pdev);
	if (rc)
		return rc;
	host_mmio = pcim_iomap_table(pdev)[PDC_MMIO_BAR];

	/* determine port configuration and setup host */
	n_ports = 2;
	if (pi->flags & PDC_FLAG_4_PORTS)
		n_ports = 4;
	for (i = 0; i < n_ports; i++)
		ppi[i] = pi;

	if (pi->flags & PDC_FLAG_SATA_PATA) {
		u8 tmp = readb(host_mmio + PDC_FLASH_CTL + 1);
		if (!(tmp & 0x80))
			ppi[n_ports++] = pi + 1;
	}

	host = ata_host_alloc_pinfo(&pdev->dev, ppi, n_ports);
	if (!host) {
		dev_err(&pdev->dev, "failed to allocate host\n");
		return -ENOMEM;
	}
	hpriv = devm_kzalloc(&pdev->dev, sizeof *hpriv, GFP_KERNEL);
	if (!hpriv)
		return -ENOMEM;
	spin_lock_init(&hpriv->hard_reset_lock);
	host->private_data = hpriv;
	host->iomap = pcim_iomap_table(pdev);

	is_sataii_tx4 = pdc_is_sataii_tx4(pi->flags);
	for (i = 0; i < host->n_ports; i++) {
		struct ata_port *ap = host->ports[i];
		unsigned int ata_no = pdc_port_no_to_ata_no(i, is_sataii_tx4);
		unsigned int ata_offset = 0x200 + ata_no * 0x80;
		unsigned int scr_offset = 0x400 + ata_no * 0x100;

		pdc_ata_setup_port(ap, host_mmio + ata_offset, host_mmio + scr_offset);

		ata_port_pbar_desc(ap, PDC_MMIO_BAR, -1, "mmio");
		ata_port_pbar_desc(ap, PDC_MMIO_BAR, ata_offset, "ata");
	}

	/* initialize adapter */
	pdc_host_init(host);

	rc = dma_set_mask_and_coherent(&pdev->dev, ATA_DMA_MASK);
	if (rc)
		return rc;

	/* start host, request IRQ and attach */
	pci_set_master(pdev);
	return ata_host_activate(host, pdev->irq, pdc_interrupt, IRQF_SHARED,
				 &pdc_ata_sht);
}

module_pci_driver(pdc_ata_pci_driver);

MODULE_AUTHOR("Jeff Garzik");
MODULE_DESCRIPTION("Promise ATA TX2/TX4/TX4000 low-level driver");
MODULE_LICENSE("GPL");
MODULE_DEVICE_TABLE(pci, pdc_ata_pci_tbl);
MODULE_VERSION(DRV_VERSION);
