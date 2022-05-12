/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2015-2019 Jason A. Donenfeld <Jason@zx2c4.com>. All Rights Reserved.
 */

#ifndef _WG_ALLOWEDIPS_H
#define _WG_ALLOWEDIPS_H

#include <linux/mutex.h>
#include <linux/ip.h>
#include <linux/ipv6.h>

struct wg_peer;

struct allowedips_node {
	struct wg_peer __rcu *peer;
	struct allowedips_node __rcu *bit[2];
	u8 cidr, bit_at_a, bit_at_b, bitlen;
	u8 bits[16] __aligned(__alignof(u64));

	/* Keep rarely used members at bottom to be beyond cache line. */
	unsigned long parent_bit_packed;
	union {
		struct list_head peer_list;
		struct rcu_head rcu;
	};
};

struct allowedips {
	struct allowedips_node __rcu *root4;
	struct allowedips_node __rcu *root6;
	u64 seq;
} __aligned(4); /* We pack the lower 2 bits of &root, but m68k only gives 16-bit alignment. */

void wg_allowedips_init(struct allowedips *table);
void wg_allowedips_free(struct allowedips *table, struct mutex *mutex);
int wg_allowedips_insert_v4(struct allowedips *table, const struct in_addr *ip,
			    u8 cidr, struct wg_peer *peer, struct mutex *lock);
int wg_allowedips_insert_v6(struct allowedips *table, const struct in6_addr *ip,
			    u8 cidr, struct wg_peer *peer, struct mutex *lock);
void wg_allowedips_remove_by_peer(struct allowedips *table,
				  struct wg_peer *peer, struct mutex *lock);
/* The ip input pointer should be __aligned(__alignof(u64))) */
int wg_allowedips_read_node(struct allowedips_node *node, u8 ip[16], u8 *cidr);

/* These return a strong reference to a peer: */
struct wg_peer *wg_allowedips_lookup_dst(struct allowedips *table,
					 struct sk_buff *skb);
struct wg_peer *wg_allowedips_lookup_src(struct allowedips *table,
					 struct sk_buff *skb);

#ifdef DEBUG
bool wg_allowedips_selftest(void);
#endif

int wg_allowedips_slab_init(void);
void wg_allowedips_slab_uninit(void);

#endif /* _WG_ALLOWEDIPS_H */
