/* SPDX-License-Identifier: GPL-2.0 */
/* Copyright (C) B.A.T.M.A.N. contributors:
 *
 * Simon Wunderlich
 */

#ifndef _NET_BATMAN_ADV_BLA_H_
#define _NET_BATMAN_ADV_BLA_H_

#include "main.h"

#include <linux/compiler.h>
#include <linux/netdevice.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>
#include <linux/stddef.h>
#include <linux/types.h>

/**
 * batadv_bla_is_loopdetect_mac() - check if the mac address is from a loop
 *  detect frame sent by bridge loop avoidance
 * @mac: mac address to check
 *
 * Return: true if the it looks like a loop detect frame
 * (mac starts with BA:BE), false otherwise
 */
static inline bool batadv_bla_is_loopdetect_mac(const uint8_t *mac)
{
	if (mac[0] == 0xba && mac[1] == 0xbe)
		return true;

	return false;
}

#ifdef CONFIG_BATMAN_ADV_BLA
bool batadv_bla_rx(struct batadv_priv *bat_priv, struct sk_buff *skb,
		   unsigned short vid, int packet_type);
bool batadv_bla_tx(struct batadv_priv *bat_priv, struct sk_buff *skb,
		   unsigned short vid);
bool batadv_bla_is_backbone_gw(struct sk_buff *skb,
			       struct batadv_orig_node *orig_node,
			       int hdr_size);
int batadv_bla_claim_dump(struct sk_buff *msg, struct netlink_callback *cb);
int batadv_bla_backbone_dump(struct sk_buff *msg, struct netlink_callback *cb);
bool batadv_bla_is_backbone_gw_orig(struct batadv_priv *bat_priv, u8 *orig,
				    unsigned short vid);
bool batadv_bla_check_bcast_duplist(struct batadv_priv *bat_priv,
				    struct sk_buff *skb);
void batadv_bla_update_orig_address(struct batadv_priv *bat_priv,
				    struct batadv_hard_iface *primary_if,
				    struct batadv_hard_iface *oldif);
void batadv_bla_status_update(struct net_device *net_dev);
int batadv_bla_init(struct batadv_priv *bat_priv);
void batadv_bla_free(struct batadv_priv *bat_priv);
int batadv_bla_claim_dump(struct sk_buff *msg, struct netlink_callback *cb);
#ifdef CONFIG_BATMAN_ADV_DAT
bool batadv_bla_check_claim(struct batadv_priv *bat_priv, u8 *addr,
			    unsigned short vid);
#endif
#define BATADV_BLA_CRC_INIT	0
#else /* ifdef CONFIG_BATMAN_ADV_BLA */

static inline bool batadv_bla_rx(struct batadv_priv *bat_priv,
				 struct sk_buff *skb, unsigned short vid,
				 int packet_type)
{
	return false;
}

static inline bool batadv_bla_tx(struct batadv_priv *bat_priv,
				 struct sk_buff *skb, unsigned short vid)
{
	return false;
}

static inline bool batadv_bla_is_backbone_gw(struct sk_buff *skb,
					     struct batadv_orig_node *orig_node,
					     int hdr_size)
{
	return false;
}

static inline bool batadv_bla_is_backbone_gw_orig(struct batadv_priv *bat_priv,
						  u8 *orig, unsigned short vid)
{
	return false;
}

static inline bool
batadv_bla_check_bcast_duplist(struct batadv_priv *bat_priv,
			       struct sk_buff *skb)
{
	return false;
}

static inline void
batadv_bla_update_orig_address(struct batadv_priv *bat_priv,
			       struct batadv_hard_iface *primary_if,
			       struct batadv_hard_iface *oldif)
{
}

static inline int batadv_bla_init(struct batadv_priv *bat_priv)
{
	return 1;
}

static inline void batadv_bla_free(struct batadv_priv *bat_priv)
{
}

static inline int batadv_bla_claim_dump(struct sk_buff *msg,
					struct netlink_callback *cb)
{
	return -EOPNOTSUPP;
}

static inline int batadv_bla_backbone_dump(struct sk_buff *msg,
					   struct netlink_callback *cb)
{
	return -EOPNOTSUPP;
}

static inline
bool batadv_bla_check_claim(struct batadv_priv *bat_priv, u8 *addr,
			    unsigned short vid)
{
	return true;
}

#endif /* ifdef CONFIG_BATMAN_ADV_BLA */

#endif /* ifndef _NET_BATMAN_ADV_BLA_H_ */
