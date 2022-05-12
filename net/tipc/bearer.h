/*
 * net/tipc/bearer.h: Include file for TIPC bearer code
 *
 * Copyright (c) 1996-2006, 2013-2016, Ericsson AB
 * Copyright (c) 2005, 2010-2011, Wind River Systems
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

#ifndef _TIPC_BEARER_H
#define _TIPC_BEARER_H

#include "netlink.h"
#include "core.h"
#include "msg.h"
#include <net/genetlink.h>

#define MAX_MEDIA	3

/* Identifiers associated with TIPC message header media address info
 * - address info field is 32 bytes long
 * - the field's actual content and length is defined per media
 * - remaining unused bytes in the field are set to zero
 */
#define TIPC_MEDIA_INFO_SIZE	32
#define TIPC_MEDIA_TYPE_OFFSET	3
#define TIPC_MEDIA_ADDR_OFFSET	4

/*
 * Identifiers of supported TIPC media types
 */
#define TIPC_MEDIA_TYPE_ETH	1
#define TIPC_MEDIA_TYPE_IB	2
#define TIPC_MEDIA_TYPE_UDP	3

/* Minimum bearer MTU */
#define TIPC_MIN_BEARER_MTU	(MAX_H_SIZE + INT_H_SIZE)

/* Identifiers for distinguishing between broadcast/multicast and replicast
 */
#define TIPC_BROADCAST_SUPPORT  1
#define TIPC_REPLICAST_SUPPORT  2

/**
 * struct tipc_media_addr - destination address used by TIPC bearers
 * @value: address info (format defined by media)
 * @media_id: TIPC media type identifier
 * @broadcast: non-zero if address is a broadcast address
 */
struct tipc_media_addr {
	u8 value[TIPC_MEDIA_INFO_SIZE];
	u8 media_id;
	u8 broadcast;
};

struct tipc_bearer;

/**
 * struct tipc_media - Media specific info exposed to generic bearer layer
 * @send_msg: routine which handles buffer transmission
 * @enable_media: routine which enables a media
 * @disable_media: routine which disables a media
 * @addr2str: convert media address format to string
 * @addr2msg: convert from media addr format to discovery msg addr format
 * @msg2addr: convert from discovery msg addr format to media addr format
 * @raw2addr: convert from raw addr format to media addr format
 * @priority: default link (and bearer) priority
 * @tolerance: default time (in ms) before declaring link failure
 * @min_win: minimum window (in packets) before declaring link congestion
 * @max_win: maximum window (in packets) before declaring link congestion
 * @mtu: max packet size bearer can support for media type not dependent on
 * underlying device MTU
 * @type_id: TIPC media identifier
 * @hwaddr_len: TIPC media address len
 * @name: media name
 */
struct tipc_media {
	int (*send_msg)(struct net *net, struct sk_buff *buf,
			struct tipc_bearer *b,
			struct tipc_media_addr *dest);
	int (*enable_media)(struct net *net, struct tipc_bearer *b,
			    struct nlattr *attr[]);
	void (*disable_media)(struct tipc_bearer *b);
	int (*addr2str)(struct tipc_media_addr *addr,
			char *strbuf,
			int bufsz);
	int (*addr2msg)(char *msg, struct tipc_media_addr *addr);
	int (*msg2addr)(struct tipc_bearer *b,
			struct tipc_media_addr *addr,
			char *msg);
	int (*raw2addr)(struct tipc_bearer *b,
			struct tipc_media_addr *addr,
			char *raw);
	u32 priority;
	u32 tolerance;
	u32 min_win;
	u32 max_win;
	u32 mtu;
	u32 type_id;
	u32 hwaddr_len;
	char name[TIPC_MAX_MEDIA_NAME];
};

/**
 * struct tipc_bearer - Generic TIPC bearer structure
 * @media_ptr: pointer to additional media-specific information about bearer
 * @mtu: max packet size bearer can support
 * @addr: media-specific address associated with bearer
 * @name: bearer name (format = media:interface)
 * @media: ptr to media structure associated with bearer
 * @bcast_addr: media address used in broadcasting
 * @pt: packet type for bearer
 * @rcu: rcu struct for tipc_bearer
 * @priority: default link priority for bearer
 * @min_win: minimum window (in packets) before declaring link congestion
 * @max_win: maximum window (in packets) before declaring link congestion
 * @tolerance: default link tolerance for bearer
 * @domain: network domain to which links can be established
 * @identity: array index of this bearer within TIPC bearer array
 * @disc: ptr to link setup request
 * @net_plane: network plane ('A' through 'H') currently associated with bearer
 * @up: bearer up flag (bit 0)
 * @refcnt: tipc_bearer reference counter
 *
 * Note: media-specific code is responsible for initialization of the fields
 * indicated below when a bearer is enabled; TIPC's generic bearer code takes
 * care of initializing all other fields.
 */
struct tipc_bearer {
	void __rcu *media_ptr;			/* initialized by media */
	u32 mtu;				/* initialized by media */
	struct tipc_media_addr addr;		/* initialized by media */
	char name[TIPC_MAX_BEARER_NAME];
	struct tipc_media *media;
	struct tipc_media_addr bcast_addr;
	struct packet_type pt;
	struct rcu_head rcu;
	u32 priority;
	u32 min_win;
	u32 max_win;
	u32 tolerance;
	u32 domain;
	u32 identity;
	struct tipc_discoverer *disc;
	char net_plane;
	unsigned long up;
	refcount_t refcnt;
};

struct tipc_bearer_names {
	char media_name[TIPC_MAX_MEDIA_NAME];
	char if_name[TIPC_MAX_IF_NAME];
};

/*
 * TIPC routines available to supported media types
 */

void tipc_rcv(struct net *net, struct sk_buff *skb, struct tipc_bearer *b);

/*
 * Routines made available to TIPC by supported media types
 */
extern struct tipc_media eth_media_info;

#ifdef CONFIG_TIPC_MEDIA_IB
extern struct tipc_media ib_media_info;
#endif
#ifdef CONFIG_TIPC_MEDIA_UDP
extern struct tipc_media udp_media_info;
#endif

int tipc_nl_bearer_disable(struct sk_buff *skb, struct genl_info *info);
int __tipc_nl_bearer_disable(struct sk_buff *skb, struct genl_info *info);
int tipc_nl_bearer_enable(struct sk_buff *skb, struct genl_info *info);
int __tipc_nl_bearer_enable(struct sk_buff *skb, struct genl_info *info);
int tipc_nl_bearer_dump(struct sk_buff *skb, struct netlink_callback *cb);
int tipc_nl_bearer_get(struct sk_buff *skb, struct genl_info *info);
int tipc_nl_bearer_set(struct sk_buff *skb, struct genl_info *info);
int __tipc_nl_bearer_set(struct sk_buff *skb, struct genl_info *info);
int tipc_nl_bearer_add(struct sk_buff *skb, struct genl_info *info);

int tipc_nl_media_dump(struct sk_buff *skb, struct netlink_callback *cb);
int tipc_nl_media_get(struct sk_buff *skb, struct genl_info *info);
int tipc_nl_media_set(struct sk_buff *skb, struct genl_info *info);
int __tipc_nl_media_set(struct sk_buff *skb, struct genl_info *info);

int tipc_media_set_priority(const char *name, u32 new_value);
int tipc_media_set_window(const char *name, u32 new_value);
int tipc_media_addr_printf(char *buf, int len, struct tipc_media_addr *a);
int tipc_enable_l2_media(struct net *net, struct tipc_bearer *b,
			 struct nlattr *attrs[]);
bool tipc_bearer_hold(struct tipc_bearer *b);
void tipc_bearer_put(struct tipc_bearer *b);
void tipc_disable_l2_media(struct tipc_bearer *b);
int tipc_l2_send_msg(struct net *net, struct sk_buff *buf,
		     struct tipc_bearer *b, struct tipc_media_addr *dest);

void tipc_bearer_add_dest(struct net *net, u32 bearer_id, u32 dest);
void tipc_bearer_remove_dest(struct net *net, u32 bearer_id, u32 dest);
struct tipc_bearer *tipc_bearer_find(struct net *net, const char *name);
int tipc_bearer_get_name(struct net *net, char *name, u32 bearer_id);
struct tipc_media *tipc_media_find(const char *name);
int tipc_bearer_setup(void);
void tipc_bearer_cleanup(void);
void tipc_bearer_stop(struct net *net);
int tipc_bearer_mtu(struct net *net, u32 bearer_id);
bool tipc_bearer_bcast_support(struct net *net, u32 bearer_id);
void tipc_bearer_xmit_skb(struct net *net, u32 bearer_id,
			  struct sk_buff *skb,
			  struct tipc_media_addr *dest);
void tipc_bearer_xmit(struct net *net, u32 bearer_id,
		      struct sk_buff_head *xmitq,
		      struct tipc_media_addr *dst,
		      struct tipc_node *__dnode);
void tipc_bearer_bc_xmit(struct net *net, u32 bearer_id,
			 struct sk_buff_head *xmitq);
void tipc_clone_to_loopback(struct net *net, struct sk_buff_head *pkts);
int tipc_attach_loopback(struct net *net);
void tipc_detach_loopback(struct net *net);

static inline void tipc_loopback_trace(struct net *net,
				       struct sk_buff_head *pkts)
{
	if (unlikely(dev_nit_active(net->loopback_dev)))
		tipc_clone_to_loopback(net, pkts);
}

/* check if device MTU is too low for tipc headers */
static inline bool tipc_mtu_bad(struct net_device *dev, unsigned int reserve)
{
	if (dev->mtu >= TIPC_MIN_BEARER_MTU + reserve)
		return false;
	netdev_warn(dev, "MTU too low for tipc bearer\n");
	return true;
}

#endif	/* _TIPC_BEARER_H */
