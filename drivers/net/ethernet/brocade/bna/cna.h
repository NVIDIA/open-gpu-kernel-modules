/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Linux network driver for QLogic BR-series Converged Network Adapter.
 */
/*
 * Copyright (c) 2006-2014 Brocade Communications Systems, Inc.
 * Copyright (c) 2014-2015 QLogic Corporation
 * All rights reserved
 * www.qlogic.com
 */

#ifndef __CNA_H__
#define __CNA_H__

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/mutex.h>
#include <linux/pci.h>
#include <linux/delay.h>
#include <linux/bitops.h>
#include <linux/timer.h>
#include <linux/interrupt.h>
#include <linux/if_vlan.h>
#include <linux/if_ether.h>

#define bfa_sm_fault(__event)    do {                            \
	pr_err("SM Assertion failure: %s: %d: event = %d\n",	\
		 __FILE__, __LINE__, __event);			\
} while (0)

extern char bfa_version[];

#define CNA_FW_FILE_CT	"ctfw-3.2.5.1.bin"
#define CNA_FW_FILE_CT2	"ct2fw-3.2.5.1.bin"
#define FC_SYMNAME_MAX	256	/*!< max name server symbolic name size */

#endif /* __CNA_H__ */
