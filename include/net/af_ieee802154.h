/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * IEEE 802.15.4 interface for userspace
 *
 * Copyright 2007, 2008 Siemens AG
 *
 * Written by:
 * Sergey Lapin <slapin@ossfans.org>
 * Dmitry Eremin-Solenikov <dbaryshkov@gmail.com>
 */

#ifndef _AF_IEEE802154_H
#define _AF_IEEE802154_H

#include <linux/socket.h> /* for sa_family_t */

enum {
	IEEE802154_ADDR_NONE = 0x0,
	/* RESERVED = 0x01, */
	IEEE802154_ADDR_SHORT = 0x2, /* 16-bit address + PANid */
	IEEE802154_ADDR_LONG = 0x3, /* 64-bit address + PANid */
};

/* address length, octets */
#define IEEE802154_ADDR_LEN	8

struct ieee802154_addr_sa {
	int addr_type;
	u16 pan_id;
	union {
		u8 hwaddr[IEEE802154_ADDR_LEN];
		u16 short_addr;
	};
};

#define IEEE802154_PANID_BROADCAST	0xffff
#define IEEE802154_ADDR_BROADCAST	0xffff
#define IEEE802154_ADDR_UNDEF		0xfffe

struct sockaddr_ieee802154 {
	sa_family_t family; /* AF_IEEE802154 */
	struct ieee802154_addr_sa addr;
};

/* get/setsockopt */
#define SOL_IEEE802154	0

#define WPAN_WANTACK		0
#define WPAN_SECURITY		1
#define WPAN_SECURITY_LEVEL	2
#define WPAN_WANTLQI		3

#define WPAN_SECURITY_DEFAULT	0
#define WPAN_SECURITY_OFF	1
#define WPAN_SECURITY_ON	2

#define WPAN_SECURITY_LEVEL_DEFAULT	(-1)

#endif
