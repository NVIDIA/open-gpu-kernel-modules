/* SPDX-License-Identifier: GPL-2.0 */
/*
 * This header is used to share core functionality between the
 * standalone connection tracking module, and the compatibility layer's use
 * of connection tracking.
 *
 * 16 Dec 2003: Yasuyuki Kozakai @USAGI <yasuyuki.kozakai@toshiba.co.jp>
 *	- generalize L3 protocol dependent part.
 *
 * Derived from include/linux/netfiter_ipv4/ip_conntrack_core.h
 */

#ifndef _NF_CONNTRACK_CORE_H
#define _NF_CONNTRACK_CORE_H

#include <linux/netfilter.h>
#include <net/netfilter/nf_conntrack.h>
#include <net/netfilter/nf_conntrack_ecache.h>
#include <net/netfilter/nf_conntrack_l4proto.h>

/* This header is used to share core functionality between the
   standalone connection tracking module, and the compatibility layer's use
   of connection tracking. */

unsigned int nf_conntrack_in(struct sk_buff *skb,
			     const struct nf_hook_state *state);

int nf_conntrack_init_net(struct net *net);
void nf_conntrack_cleanup_net(struct net *net);
void nf_conntrack_cleanup_net_list(struct list_head *net_exit_list);

void nf_conntrack_proto_pernet_init(struct net *net);
void nf_conntrack_proto_pernet_fini(struct net *net);

int nf_conntrack_proto_init(void);
void nf_conntrack_proto_fini(void);

int nf_conntrack_init_start(void);
void nf_conntrack_cleanup_start(void);

void nf_conntrack_init_end(void);
void nf_conntrack_cleanup_end(void);

bool nf_ct_invert_tuple(struct nf_conntrack_tuple *inverse,
			const struct nf_conntrack_tuple *orig);

/* Find a connection corresponding to a tuple. */
struct nf_conntrack_tuple_hash *
nf_conntrack_find_get(struct net *net,
		      const struct nf_conntrack_zone *zone,
		      const struct nf_conntrack_tuple *tuple);

int __nf_conntrack_confirm(struct sk_buff *skb);

/* Confirm a connection: returns NF_DROP if packet must be dropped. */
static inline int nf_conntrack_confirm(struct sk_buff *skb)
{
	struct nf_conn *ct = (struct nf_conn *)skb_nfct(skb);
	int ret = NF_ACCEPT;

	if (ct) {
		if (!nf_ct_is_confirmed(ct))
			ret = __nf_conntrack_confirm(skb);
		if (likely(ret == NF_ACCEPT))
			nf_ct_deliver_cached_events(ct);
	}
	return ret;
}

unsigned int nf_confirm(struct sk_buff *skb, unsigned int protoff,
			struct nf_conn *ct, enum ip_conntrack_info ctinfo);

void print_tuple(struct seq_file *s, const struct nf_conntrack_tuple *tuple,
		 const struct nf_conntrack_l4proto *proto);

#define CONNTRACK_LOCKS 1024

extern spinlock_t nf_conntrack_locks[CONNTRACK_LOCKS];
void nf_conntrack_lock(spinlock_t *lock);

extern spinlock_t nf_conntrack_expect_lock;

#endif /* _NF_CONNTRACK_CORE_H */
