/*
 * Generic platform device PATA driver
 *
 * Copyright (C) 2006 - 2007  Paul Mundt
 *
 * Based on pata_pcmcia:
 *
 *   Copyright 2005-2006 Red Hat Inc, all rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/blkdev.h>
#include <scsi/scsi_host.h>
#include <linux/ata.h>
#include <linux/libata.h>
#include <linux/platform_device.h>
#include <linux/ata_platform.h>

#define DRV_NAME "pata_platform"
#define DRV_VERSION "1.2"

static int pio_mask = 1;
module_param(pio_mask, int, 0);
MODULE_PARM_DESC(pio_mask, "PIO modes supported, mode 0 only by default");

/*
 * Provide our own set_mode() as we don't want to change anything that has
 * already been configured..
 */
static int pata_platform_set_mode(struct ata_link *link, struct ata_device **unused)
{
	struct ata_device *dev;

	ata_for_each_dev(dev, link, ENABLED) {
		/* We don't really care */
		dev->pio_mode = dev->xfer_mode = XFER_PIO_0;
		dev->xfer_shift = ATA_SHIFT_PIO;
		dev->flags |= ATA_DFLAG_PIO;
		ata_dev_info(dev, "configured for PIO\n");
	}
	return 0;
}

static struct scsi_host_template pata_platform_sht = {
	ATA_PIO_SHT(DRV_NAME),
};

static void pata_platform_setup_port(struct ata_ioports *ioaddr,
				     unsigned int shift)
{
	/* Fixup the port shift for platforms that need it */
	ioaddr->data_addr	= ioaddr->cmd_addr + (ATA_REG_DATA    << shift);
	ioaddr->error_addr	= ioaddr->cmd_addr + (ATA_REG_ERR     << shift);
	ioaddr->feature_addr	= ioaddr->cmd_addr + (ATA_REG_FEATURE << shift);
	ioaddr->nsect_addr	= ioaddr->cmd_addr + (ATA_REG_NSECT   << shift);
	ioaddr->lbal_addr	= ioaddr->cmd_addr + (ATA_REG_LBAL    << shift);
	ioaddr->lbam_addr	= ioaddr->cmd_addr + (ATA_REG_LBAM    << shift);
	ioaddr->lbah_addr	= ioaddr->cmd_addr + (ATA_REG_LBAH    << shift);
	ioaddr->device_addr	= ioaddr->cmd_addr + (ATA_REG_DEVICE  << shift);
	ioaddr->status_addr	= ioaddr->cmd_addr + (ATA_REG_STATUS  << shift);
	ioaddr->command_addr	= ioaddr->cmd_addr + (ATA_REG_CMD     << shift);
}

/**
 *	__pata_platform_probe		-	attach a platform interface
 *	@dev: device
 *	@io_res: Resource representing I/O base
 *	@ctl_res: Resource representing CTL base
 *	@irq_res: Resource representing IRQ and its flags
 *	@ioport_shift: I/O port shift
 *	@__pio_mask: PIO mask
 *	@sht: scsi_host_template to use when registering
 *	@use16bit: Flag to indicate 16-bit IO instead of 32-bit
 *
 *	Register a platform bus IDE interface. Such interfaces are PIO and we
 *	assume do not support IRQ sharing.
 *
 *	Platform devices are expected to contain at least 2 resources per port:
 *
 *		- I/O Base (IORESOURCE_IO or IORESOURCE_MEM)
 *		- CTL Base (IORESOURCE_IO or IORESOURCE_MEM)
 *
 *	and optionally:
 *
 *		- IRQ	   (IORESOURCE_IRQ)
 *
 *	If the base resources are both mem types, the ioremap() is handled
 *	here. For IORESOURCE_IO, it's assumed that there's no remapping
 *	necessary.
 *
 *	If no IRQ resource is present, PIO polling mode is used instead.
 */
int __pata_platform_probe(struct device *dev, struct resource *io_res,
			  struct resource *ctl_res, struct resource *irq_res,
			  unsigned int ioport_shift, int __pio_mask,
			  struct scsi_host_template *sht, bool use16bit)
{
	struct ata_host *host;
	struct ata_port *ap;
	unsigned int mmio;
	int irq = 0;
	int irq_flags = 0;

	/*
	 * Check for MMIO
	 */
	mmio = (( io_res->flags == IORESOURCE_MEM) &&
		(ctl_res->flags == IORESOURCE_MEM));

	/*
	 * And the IRQ
	 */
	if (irq_res && irq_res->start > 0) {
		irq = irq_res->start;
		irq_flags = (irq_res->flags & IRQF_TRIGGER_MASK) | IRQF_SHARED;
	}

	/*
	 * Now that that's out of the way, wire up the port..
	 */
	host = ata_host_alloc(dev, 1);
	if (!host)
		return -ENOMEM;
	ap = host->ports[0];

	ap->ops = devm_kzalloc(dev, sizeof(*ap->ops), GFP_KERNEL);
	ap->ops->inherits = &ata_sff_port_ops;
	ap->ops->cable_detect = ata_cable_unknown;
	ap->ops->set_mode = pata_platform_set_mode;
	if (use16bit)
		ap->ops->sff_data_xfer = ata_sff_data_xfer;
	else
		ap->ops->sff_data_xfer = ata_sff_data_xfer32;

	ap->pio_mask = __pio_mask;
	ap->flags |= ATA_FLAG_SLAVE_POSS;

	/*
	 * Use polling mode if there's no IRQ
	 */
	if (!irq) {
		ap->flags |= ATA_FLAG_PIO_POLLING;
		ata_port_desc(ap, "no IRQ, using PIO polling");
	}

	/*
	 * Handle the MMIO case
	 */
	if (mmio) {
		ap->ioaddr.cmd_addr = devm_ioremap(dev, io_res->start,
				resource_size(io_res));
		ap->ioaddr.ctl_addr = devm_ioremap(dev, ctl_res->start,
				resource_size(ctl_res));
	} else {
		ap->ioaddr.cmd_addr = devm_ioport_map(dev, io_res->start,
				resource_size(io_res));
		ap->ioaddr.ctl_addr = devm_ioport_map(dev, ctl_res->start,
				resource_size(ctl_res));
	}
	if (!ap->ioaddr.cmd_addr || !ap->ioaddr.ctl_addr) {
		dev_err(dev, "failed to map IO/CTL base\n");
		return -ENOMEM;
	}

	ap->ioaddr.altstatus_addr = ap->ioaddr.ctl_addr;

	pata_platform_setup_port(&ap->ioaddr, ioport_shift);

	ata_port_desc(ap, "%s cmd 0x%llx ctl 0x%llx", mmio ? "mmio" : "ioport",
		      (unsigned long long)io_res->start,
		      (unsigned long long)ctl_res->start);

	/* activate */
	return ata_host_activate(host, irq, irq ? ata_sff_interrupt : NULL,
				 irq_flags, sht);
}
EXPORT_SYMBOL_GPL(__pata_platform_probe);

static int pata_platform_probe(struct platform_device *pdev)
{
	struct resource *io_res;
	struct resource *ctl_res;
	struct resource *irq_res;
	struct pata_platform_info *pp_info = dev_get_platdata(&pdev->dev);

	/*
	 * Simple resource validation ..
	 */
	if ((pdev->num_resources != 3) && (pdev->num_resources != 2)) {
		dev_err(&pdev->dev, "invalid number of resources\n");
		return -EINVAL;
	}

	/*
	 * Get the I/O base first
	 */
	io_res = platform_get_resource(pdev, IORESOURCE_IO, 0);
	if (io_res == NULL) {
		io_res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
		if (unlikely(io_res == NULL))
			return -EINVAL;
	}

	/*
	 * Then the CTL base
	 */
	ctl_res = platform_get_resource(pdev, IORESOURCE_IO, 1);
	if (ctl_res == NULL) {
		ctl_res = platform_get_resource(pdev, IORESOURCE_MEM, 1);
		if (unlikely(ctl_res == NULL))
			return -EINVAL;
	}

	/*
	 * And the IRQ
	 */
	irq_res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);

	return __pata_platform_probe(&pdev->dev, io_res, ctl_res, irq_res,
				     pp_info ? pp_info->ioport_shift : 0,
				     pio_mask, &pata_platform_sht, false);
}

static struct platform_driver pata_platform_driver = {
	.probe		= pata_platform_probe,
	.remove		= ata_platform_remove_one,
	.driver = {
		.name		= DRV_NAME,
	},
};

module_platform_driver(pata_platform_driver);

MODULE_AUTHOR("Paul Mundt");
MODULE_DESCRIPTION("low-level driver for platform device ATA");
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_VERSION);
MODULE_ALIAS("platform:" DRV_NAME);
