// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *	Adaptec AAC series RAID controller driver
 *
 * based on the old aacraid driver that is..
 * Adaptec aacraid device driver for Linux.
 *
 * Copyright (c) 2000-2010 Adaptec, Inc.
 *               2010-2015 PMC-Sierra, Inc. (aacraid@pmc-sierra.com)
 *		 2016-2017 Microsemi Corp. (aacraid@microsemi.com)
 *
 * Module Name:
 *  nark.c
 *
 * Abstract: Hardware Device Interface for NEMER/ARK
 */

#include <linux/pci.h>
#include <linux/blkdev.h>

#include <scsi/scsi_host.h>

#include "aacraid.h"

/**
 *	aac_nark_ioremap
 *	@dev: device to ioremap
 *	@size: mapping resize request
 *
 */
static int aac_nark_ioremap(struct aac_dev * dev, u32 size)
{
	if (!size) {
		iounmap(dev->regs.rx);
		dev->regs.rx = NULL;
		iounmap(dev->base);
		dev->base = NULL;
		return 0;
	}
	dev->base_start = pci_resource_start(dev->pdev, 2);
	dev->regs.rx = ioremap((u64)pci_resource_start(dev->pdev, 0) |
	  ((u64)pci_resource_start(dev->pdev, 1) << 32),
	  sizeof(struct rx_registers) - sizeof(struct rx_inbound));
	dev->base = NULL;
	if (dev->regs.rx == NULL)
		return -1;
	dev->base = ioremap(dev->base_start, size);
	if (dev->base == NULL) {
		iounmap(dev->regs.rx);
		dev->regs.rx = NULL;
		return -1;
	}
	dev->IndexRegs = &((struct rx_registers __iomem *)dev->base)->IndexRegs;
	return 0;
}

/**
 *	aac_nark_init	-	initialize an NEMER/ARK Split Bar card
 *	@dev: device to configure
 *
 */

int aac_nark_init(struct aac_dev * dev)
{
	/*
	 *	Fill in the function dispatch table.
	 */
	dev->a_ops.adapter_ioremap = aac_nark_ioremap;
	dev->a_ops.adapter_comm = aac_rx_select_comm;

	return _aac_rx_init(dev);
}
