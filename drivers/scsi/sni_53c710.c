// SPDX-License-Identifier: GPL-2.0-or-later

/* SNI RM driver
 *
 * Copyright (C) 2001 by James.Bottomley@HansenPartnership.com
**-----------------------------------------------------------------------------
**
**
**-----------------------------------------------------------------------------
 */

/*
 * Based on lasi700.c
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/stat.h>
#include <linux/mm.h>
#include <linux/blkdev.h>
#include <linux/sched.h>
#include <linux/ioport.h>
#include <linux/dma-mapping.h>
#include <linux/platform_device.h>

#include <asm/page.h>
#include <asm/irq.h>
#include <asm/delay.h>

#include <scsi/scsi_host.h>
#include <scsi/scsi_device.h>
#include <scsi/scsi_transport.h>
#include <scsi/scsi_transport_spi.h>

#include "53c700.h"

MODULE_AUTHOR("Thomas Bogendörfer");
MODULE_DESCRIPTION("SNI RM 53c710 SCSI Driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:snirm_53c710");

#define SNIRM710_CLOCK	32

static struct scsi_host_template snirm710_template = {
	.name		= "SNI RM SCSI 53c710",
	.proc_name	= "snirm_53c710",
	.this_id	= 7,
	.module		= THIS_MODULE,
};

static int snirm710_probe(struct platform_device *dev)
{
	unsigned long base;
	struct NCR_700_Host_Parameters *hostdata;
	struct Scsi_Host *host;
	struct  resource *res;
	int rc;

	res = platform_get_resource(dev, IORESOURCE_MEM, 0);
	if (!res)
		return -ENODEV;

	base = res->start;
	hostdata = kzalloc(sizeof(*hostdata), GFP_KERNEL);
	if (!hostdata)
		return -ENOMEM;

	hostdata->dev = &dev->dev;
	dma_set_mask(&dev->dev, DMA_BIT_MASK(32));
	hostdata->base = ioremap(base, 0x100);
	hostdata->differential = 0;

	hostdata->clock = SNIRM710_CLOCK;
	hostdata->force_le_on_be = 1;
	hostdata->chip710 = 1;
	hostdata->burst_length = 4;

	host = NCR_700_detect(&snirm710_template, hostdata, &dev->dev);
	if (!host)
		goto out_kfree;
	host->this_id = 7;
	host->base = base;
	host->irq = rc = platform_get_irq(dev, 0);
	if (rc < 0)
		goto out_put_host;
	if(request_irq(host->irq, NCR_700_intr, IRQF_SHARED, "snirm710", host)) {
		printk(KERN_ERR "snirm710: request_irq failed!\n");
		goto out_put_host;
	}

	dev_set_drvdata(&dev->dev, host);
	scsi_scan_host(host);

	return 0;

 out_put_host:
	scsi_host_put(host);
 out_kfree:
	iounmap(hostdata->base);
	kfree(hostdata);
	return -ENODEV;
}

static int snirm710_driver_remove(struct platform_device *dev)
{
	struct Scsi_Host *host = dev_get_drvdata(&dev->dev);
	struct NCR_700_Host_Parameters *hostdata =
		(struct NCR_700_Host_Parameters *)host->hostdata[0];

	scsi_remove_host(host);
	NCR_700_release(host);
	free_irq(host->irq, host);
	iounmap(hostdata->base);
	kfree(hostdata);

	return 0;
}

static struct platform_driver snirm710_driver = {
	.probe	= snirm710_probe,
	.remove	= snirm710_driver_remove,
	.driver	= {
		.name	= "snirm_53c710",
	},
};
module_platform_driver(snirm710_driver);
