/* SPDX-License-Identifier: GPL-2.0 */
/* Copyright (C) B.A.T.M.A.N. contributors:
 *
 * Antonio Quartulli
 */

#ifndef _NET_BATMAN_ADV_BAT_V_OGM_H_
#define _NET_BATMAN_ADV_BAT_V_OGM_H_

#include "main.h"

#include <linux/skbuff.h>
#include <linux/types.h>
#include <linux/workqueue.h>

int batadv_v_ogm_init(struct batadv_priv *bat_priv);
void batadv_v_ogm_free(struct batadv_priv *bat_priv);
void batadv_v_ogm_aggr_work(struct work_struct *work);
int batadv_v_ogm_iface_enable(struct batadv_hard_iface *hard_iface);
void batadv_v_ogm_iface_disable(struct batadv_hard_iface *hard_iface);
struct batadv_orig_node *batadv_v_ogm_orig_get(struct batadv_priv *bat_priv,
					       const u8 *addr);
void batadv_v_ogm_primary_iface_set(struct batadv_hard_iface *primary_iface);
int batadv_v_ogm_packet_recv(struct sk_buff *skb,
			     struct batadv_hard_iface *if_incoming);

#endif /* _NET_BATMAN_ADV_BAT_V_OGM_H_ */
