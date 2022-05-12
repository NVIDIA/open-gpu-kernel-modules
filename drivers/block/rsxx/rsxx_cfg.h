/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
* Filename: rsXX_cfg.h
*
* Authors: Joshua Morris <josh.h.morris@us.ibm.com>
*	Philip Kelleher <pjk1939@linux.vnet.ibm.com>
*
* (C) Copyright 2013 IBM Corporation
*/

#ifndef __RSXX_CFG_H__
#define __RSXX_CFG_H__

/* NOTE: Config values will be saved in network byte order (i.e. Big endian) */
#include <linux/types.h>

/*
 * The card config version must match the driver's expected version. If it does
 * not, the DMA interfaces will not be attached and the user will need to
 * initialize/upgrade the card configuration using the card config utility.
 */
#define RSXX_CFG_VERSION	4

struct card_cfg_hdr {
	__u32	version;
	__u32	crc;
};

struct card_cfg_data {
	__u32	block_size;
	__u32	stripe_size;
	__u32	vendor_id;
	__u32	cache_order;
	struct {
		__u32	mode;	/* Disabled, manual, auto-tune... */
		__u32	count;	/* Number of intr to coalesce     */
		__u32	latency;/* Max wait time (in ns)          */
	} intr_coal;
};

struct rsxx_card_cfg {
	struct card_cfg_hdr	hdr;
	struct card_cfg_data	data;
};

/* Vendor ID Values */
#define RSXX_VENDOR_ID_IBM		0
#define RSXX_VENDOR_ID_DSI		1
#define RSXX_VENDOR_COUNT		2

/* Interrupt Coalescing Values */
#define RSXX_INTR_COAL_DISABLED           0
#define RSXX_INTR_COAL_EXPLICIT           1
#define RSXX_INTR_COAL_AUTO_TUNE          2


#endif /* __RSXX_CFG_H__ */

