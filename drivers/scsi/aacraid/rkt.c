// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *	Adaptec AAC series RAID controller driver
 *	(c) Copyright 2001 Red Hat Inc.
 *
 * based on the old aacraid driver that is..
 * Adaptec aacraid device driver for Linux.
 *
 * Copyright (c) 2000-2010 Adaptec, Inc.
 *               2010-2015 PMC-Sierra, Inc. (aacraid@pmc-sierra.com)
 *		 2016-2017 Microsemi Corp. (aacraid@microsemi.com)
 *
 * Module Name:
 *  rkt.c
 *
 * Abstract: Hardware miniport for Drawbridge specific hardware functions.
 */

#include <linux/blkdev.h>

#include <scsi/scsi_host.h>

#include "aacraid.h"

#define AAC_NUM_IO_FIB_RKT      (246 - AAC_NUM_MGT_FIB)

/**
 *	aac_rkt_select_comm	-	Select communications method
 *	@dev: Adapter
 *	@comm: communications method
 */

static int aac_rkt_select_comm(struct aac_dev *dev, int comm)
{
	int retval;
	retval = aac_rx_select_comm(dev, comm);
	if (comm == AAC_COMM_MESSAGE) {
		/*
		 * FIB Setup has already been done, but we can minimize the
		 * damage by at least ensuring the OS never issues more
		 * commands than we can handle. The Rocket adapters currently
		 * can only handle 246 commands and 8 AIFs at the same time,
		 * and in fact do notify us accordingly if we negotiate the
		 * FIB size. The problem that causes us to add this check is
		 * to ensure that we do not overdo it with the adapter when a
		 * hard coded FIB override is being utilized. This special
		 * case warrants this half baked, but convenient, check here.
		 */
		if (dev->scsi_host_ptr->can_queue > AAC_NUM_IO_FIB_RKT) {
			dev->init->r7.max_io_commands =
				cpu_to_le32(AAC_NUM_IO_FIB_RKT + AAC_NUM_MGT_FIB);
			dev->scsi_host_ptr->can_queue = AAC_NUM_IO_FIB_RKT;
		}
	}
	return retval;
}

/**
 *	aac_rkt_ioremap
 *	@dev: device to ioremap
 *	@size: mapping resize request
 *
 */
static int aac_rkt_ioremap(struct aac_dev * dev, u32 size)
{
	if (!size) {
		iounmap(dev->regs.rkt);
		return 0;
	}
	dev->base = dev->regs.rkt = ioremap(dev->base_start, size);
	if (dev->base == NULL)
		return -1;
	dev->IndexRegs = &dev->regs.rkt->IndexRegs;
	return 0;
}

/**
 *	aac_rkt_init	-	initialize an i960 based AAC card
 *	@dev: device to configure
 *
 *	Allocate and set up resources for the i960 based AAC variants. The
 *	device_interface in the commregion will be allocated and linked
 *	to the comm region.
 */

int aac_rkt_init(struct aac_dev *dev)
{
	/*
	 *	Fill in the function dispatch table.
	 */
	dev->a_ops.adapter_ioremap = aac_rkt_ioremap;
	dev->a_ops.adapter_comm = aac_rkt_select_comm;

	return _aac_rx_init(dev);
}
