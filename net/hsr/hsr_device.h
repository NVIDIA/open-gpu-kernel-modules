/* SPDX-License-Identifier: GPL-2.0 */
/* Copyright 2011-2014 Autronica Fire and Security AS
 *
 * Author(s):
 *	2011-2014 Arvid Brodin, arvid.brodin@alten.se
 *
 * include file for HSR and PRP.
 */

#ifndef __HSR_DEVICE_H
#define __HSR_DEVICE_H

#include <linux/netdevice.h>
#include "hsr_main.h"

void hsr_del_ports(struct hsr_priv *hsr);
void hsr_dev_setup(struct net_device *dev);
int hsr_dev_finalize(struct net_device *hsr_dev, struct net_device *slave[2],
		     unsigned char multicast_spec, u8 protocol_version,
		     struct netlink_ext_ack *extack);
void hsr_check_carrier_and_operstate(struct hsr_priv *hsr);
int hsr_get_max_mtu(struct hsr_priv *hsr);
#endif /* __HSR_DEVICE_H */
