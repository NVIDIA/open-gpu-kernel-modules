/*
 * net/tipc/node.h: Include file for TIPC node management routines
 *
 * Copyright (c) 2000-2006, 2014-2016, Ericsson AB
 * Copyright (c) 2005, 2010-2014, Wind River Systems
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the names of the copyright holders nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * Alternatively, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") version 2 as published by the Free
 * Software Foundation.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _TIPC_NODE_H
#define _TIPC_NODE_H

#include "addr.h"
#include "net.h"
#include "bearer.h"
#include "msg.h"

/* Optional capabilities supported by this code version
 */
enum {
	TIPC_SYN_BIT          = (1),
	TIPC_BCAST_SYNCH      = (1 << 1),
	TIPC_BCAST_STATE_NACK = (1 << 2),
	TIPC_BLOCK_FLOWCTL    = (1 << 3),
	TIPC_BCAST_RCAST      = (1 << 4),
	TIPC_NODE_ID128       = (1 << 5),
	TIPC_LINK_PROTO_SEQNO = (1 << 6),
	TIPC_MCAST_RBCTL      = (1 << 7),
	TIPC_GAP_ACK_BLOCK    = (1 << 8),
	TIPC_TUNNEL_ENHANCED  = (1 << 9),
	TIPC_NAGLE            = (1 << 10),
	TIPC_NAMED_BCAST      = (1 << 11)
};

#define TIPC_NODE_CAPABILITIES (TIPC_SYN_BIT           |  \
				TIPC_BCAST_SYNCH       |   \
				TIPC_BCAST_STATE_NACK  |   \
				TIPC_BCAST_RCAST       |   \
				TIPC_BLOCK_FLOWCTL     |   \
				TIPC_NODE_ID128        |   \
				TIPC_LINK_PROTO_SEQNO  |   \
				TIPC_MCAST_RBCTL       |   \
				TIPC_GAP_ACK_BLOCK     |   \
				TIPC_TUNNEL_ENHANCED   |   \
				TIPC_NAGLE             |   \
				TIPC_NAMED_BCAST)

#define INVALID_BEARER_ID -1

void tipc_node_stop(struct net *net);
bool tipc_node_get_id(struct net *net, u32 addr, u8 *id);
u32 tipc_node_get_addr(struct tipc_node *node);
char *tipc_node_get_id_str(struct tipc_node *node);
void tipc_node_put(struct tipc_node *node);
void tipc_node_get(struct tipc_node *node);
struct tipc_node *tipc_node_create(struct net *net, u32 addr, u8 *peer_id,
				   u16 capabilities, u32 hash_mixes,
				   bool preliminary);
#ifdef CONFIG_TIPC_CRYPTO
struct tipc_crypto *tipc_node_crypto_rx(struct tipc_node *__n);
struct tipc_crypto *tipc_node_crypto_rx_by_list(struct list_head *pos);
struct tipc_crypto *tipc_node_crypto_rx_by_addr(struct net *net, u32 addr);
#endif
u32 tipc_node_try_addr(struct net *net, u8 *id, u32 addr);
void tipc_node_check_dest(struct net *net, u32 onode, u8 *peer_id128,
			  struct tipc_bearer *bearer,
			  u16 capabilities, u32 signature, u32 hash_mixes,
			  struct tipc_media_addr *maddr,
			  bool *respond, bool *dupl_addr);
void tipc_node_delete_links(struct net *net, int bearer_id);
void tipc_node_apply_property(struct net *net, struct tipc_bearer *b, int prop);
int tipc_node_get_linkname(struct net *net, u32 bearer_id, u32 node,
			   char *linkname, size_t len);
int tipc_node_xmit(struct net *net, struct sk_buff_head *list, u32 dnode,
		   int selector);
int tipc_node_distr_xmit(struct net *net, struct sk_buff_head *list);
int tipc_node_xmit_skb(struct net *net, struct sk_buff *skb, u32 dest,
		       u32 selector);
void tipc_node_subscribe(struct net *net, struct list_head *subscr, u32 addr);
void tipc_node_unsubscribe(struct net *net, struct list_head *subscr, u32 addr);
void tipc_node_broadcast(struct net *net, struct sk_buff *skb, int rc_dests);
int tipc_node_add_conn(struct net *net, u32 dnode, u32 port, u32 peer_port);
void tipc_node_remove_conn(struct net *net, u32 dnode, u32 port);
int tipc_node_get_mtu(struct net *net, u32 addr, u32 sel, bool connected);
bool tipc_node_is_up(struct net *net, u32 addr);
u16 tipc_node_get_capabilities(struct net *net, u32 addr);
int tipc_nl_node_dump(struct sk_buff *skb, struct netlink_callback *cb);
int tipc_nl_node_dump_link(struct sk_buff *skb, struct netlink_callback *cb);
int tipc_nl_node_reset_link_stats(struct sk_buff *skb, struct genl_info *info);
int tipc_nl_node_get_link(struct sk_buff *skb, struct genl_info *info);
int tipc_nl_node_set_link(struct sk_buff *skb, struct genl_info *info);
int tipc_nl_peer_rm(struct sk_buff *skb, struct genl_info *info);

int tipc_nl_node_set_monitor(struct sk_buff *skb, struct genl_info *info);
int tipc_nl_node_get_monitor(struct sk_buff *skb, struct genl_info *info);
int tipc_nl_node_dump_monitor(struct sk_buff *skb, struct netlink_callback *cb);
int tipc_nl_node_dump_monitor_peer(struct sk_buff *skb,
				   struct netlink_callback *cb);
#ifdef CONFIG_TIPC_CRYPTO
int tipc_nl_node_set_key(struct sk_buff *skb, struct genl_info *info);
int tipc_nl_node_flush_key(struct sk_buff *skb, struct genl_info *info);
#endif
void tipc_node_pre_cleanup_net(struct net *exit_net);
#endif
