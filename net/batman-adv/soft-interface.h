/* SPDX-License-Identifier: GPL-2.0 */
/* Copyright (C) B.A.T.M.A.N. contributors:
 *
 * Marek Lindner
 */

#ifndef _NET_BATMAN_ADV_SOFT_INTERFACE_H_
#define _NET_BATMAN_ADV_SOFT_INTERFACE_H_

#include "main.h"

#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/types.h>
#include <net/net_namespace.h>
#include <net/rtnetlink.h>

int batadv_skb_head_push(struct sk_buff *skb, unsigned int len);
void batadv_interface_rx(struct net_device *soft_iface,
			 struct sk_buff *skb, int hdr_size,
			 struct batadv_orig_node *orig_node);
struct net_device *batadv_softif_create(struct net *net, const char *name);
bool batadv_softif_is_valid(const struct net_device *net_dev);
extern struct rtnl_link_ops batadv_link_ops;
int batadv_softif_create_vlan(struct batadv_priv *bat_priv, unsigned short vid);
void batadv_softif_vlan_put(struct batadv_softif_vlan *softif_vlan);
struct batadv_softif_vlan *batadv_softif_vlan_get(struct batadv_priv *bat_priv,
						  unsigned short vid);

#endif /* _NET_BATMAN_ADV_SOFT_INTERFACE_H_ */
