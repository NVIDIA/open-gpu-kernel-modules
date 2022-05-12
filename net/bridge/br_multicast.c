// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Bridge multicast support.
 *
 * Copyright (c) 2010 Herbert Xu <herbert@gondor.apana.org.au>
 */

#include <linux/err.h>
#include <linux/export.h>
#include <linux/if_ether.h>
#include <linux/igmp.h>
#include <linux/in.h>
#include <linux/jhash.h>
#include <linux/kernel.h>
#include <linux/log2.h>
#include <linux/netdevice.h>
#include <linux/netfilter_bridge.h>
#include <linux/random.h>
#include <linux/rculist.h>
#include <linux/skbuff.h>
#include <linux/slab.h>
#include <linux/timer.h>
#include <linux/inetdevice.h>
#include <linux/mroute.h>
#include <net/ip.h>
#include <net/switchdev.h>
#if IS_ENABLED(CONFIG_IPV6)
#include <linux/icmpv6.h>
#include <net/ipv6.h>
#include <net/mld.h>
#include <net/ip6_checksum.h>
#include <net/addrconf.h>
#endif

#include "br_private.h"
#include "br_private_mcast_eht.h"

static const struct rhashtable_params br_mdb_rht_params = {
	.head_offset = offsetof(struct net_bridge_mdb_entry, rhnode),
	.key_offset = offsetof(struct net_bridge_mdb_entry, addr),
	.key_len = sizeof(struct br_ip),
	.automatic_shrinking = true,
};

static const struct rhashtable_params br_sg_port_rht_params = {
	.head_offset = offsetof(struct net_bridge_port_group, rhnode),
	.key_offset = offsetof(struct net_bridge_port_group, key),
	.key_len = sizeof(struct net_bridge_port_group_sg_key),
	.automatic_shrinking = true,
};

static void br_multicast_start_querier(struct net_bridge *br,
				       struct bridge_mcast_own_query *query);
static void br_multicast_add_router(struct net_bridge *br,
				    struct net_bridge_port *port);
static void br_ip4_multicast_leave_group(struct net_bridge *br,
					 struct net_bridge_port *port,
					 __be32 group,
					 __u16 vid,
					 const unsigned char *src);
static void br_multicast_port_group_rexmit(struct timer_list *t);

static void __del_port_router(struct net_bridge_port *p);
#if IS_ENABLED(CONFIG_IPV6)
static void br_ip6_multicast_leave_group(struct net_bridge *br,
					 struct net_bridge_port *port,
					 const struct in6_addr *group,
					 __u16 vid, const unsigned char *src);
#endif
static struct net_bridge_port_group *
__br_multicast_add_group(struct net_bridge *br,
			 struct net_bridge_port *port,
			 struct br_ip *group,
			 const unsigned char *src,
			 u8 filter_mode,
			 bool igmpv2_mldv1,
			 bool blocked);
static void br_multicast_find_del_pg(struct net_bridge *br,
				     struct net_bridge_port_group *pg);

static struct net_bridge_port_group *
br_sg_port_find(struct net_bridge *br,
		struct net_bridge_port_group_sg_key *sg_p)
{
	lockdep_assert_held_once(&br->multicast_lock);

	return rhashtable_lookup_fast(&br->sg_port_tbl, sg_p,
				      br_sg_port_rht_params);
}

static struct net_bridge_mdb_entry *br_mdb_ip_get_rcu(struct net_bridge *br,
						      struct br_ip *dst)
{
	return rhashtable_lookup(&br->mdb_hash_tbl, dst, br_mdb_rht_params);
}

struct net_bridge_mdb_entry *br_mdb_ip_get(struct net_bridge *br,
					   struct br_ip *dst)
{
	struct net_bridge_mdb_entry *ent;

	lockdep_assert_held_once(&br->multicast_lock);

	rcu_read_lock();
	ent = rhashtable_lookup(&br->mdb_hash_tbl, dst, br_mdb_rht_params);
	rcu_read_unlock();

	return ent;
}

static struct net_bridge_mdb_entry *br_mdb_ip4_get(struct net_bridge *br,
						   __be32 dst, __u16 vid)
{
	struct br_ip br_dst;

	memset(&br_dst, 0, sizeof(br_dst));
	br_dst.dst.ip4 = dst;
	br_dst.proto = htons(ETH_P_IP);
	br_dst.vid = vid;

	return br_mdb_ip_get(br, &br_dst);
}

#if IS_ENABLED(CONFIG_IPV6)
static struct net_bridge_mdb_entry *br_mdb_ip6_get(struct net_bridge *br,
						   const struct in6_addr *dst,
						   __u16 vid)
{
	struct br_ip br_dst;

	memset(&br_dst, 0, sizeof(br_dst));
	br_dst.dst.ip6 = *dst;
	br_dst.proto = htons(ETH_P_IPV6);
	br_dst.vid = vid;

	return br_mdb_ip_get(br, &br_dst);
}
#endif

struct net_bridge_mdb_entry *br_mdb_get(struct net_bridge *br,
					struct sk_buff *skb, u16 vid)
{
	struct br_ip ip;

	if (!br_opt_get(br, BROPT_MULTICAST_ENABLED))
		return NULL;

	if (BR_INPUT_SKB_CB(skb)->igmp)
		return NULL;

	memset(&ip, 0, sizeof(ip));
	ip.proto = skb->protocol;
	ip.vid = vid;

	switch (skb->protocol) {
	case htons(ETH_P_IP):
		ip.dst.ip4 = ip_hdr(skb)->daddr;
		if (br->multicast_igmp_version == 3) {
			struct net_bridge_mdb_entry *mdb;

			ip.src.ip4 = ip_hdr(skb)->saddr;
			mdb = br_mdb_ip_get_rcu(br, &ip);
			if (mdb)
				return mdb;
			ip.src.ip4 = 0;
		}
		break;
#if IS_ENABLED(CONFIG_IPV6)
	case htons(ETH_P_IPV6):
		ip.dst.ip6 = ipv6_hdr(skb)->daddr;
		if (br->multicast_mld_version == 2) {
			struct net_bridge_mdb_entry *mdb;

			ip.src.ip6 = ipv6_hdr(skb)->saddr;
			mdb = br_mdb_ip_get_rcu(br, &ip);
			if (mdb)
				return mdb;
			memset(&ip.src.ip6, 0, sizeof(ip.src.ip6));
		}
		break;
#endif
	default:
		ip.proto = 0;
		ether_addr_copy(ip.dst.mac_addr, eth_hdr(skb)->h_dest);
	}

	return br_mdb_ip_get_rcu(br, &ip);
}

static bool br_port_group_equal(struct net_bridge_port_group *p,
				struct net_bridge_port *port,
				const unsigned char *src)
{
	if (p->key.port != port)
		return false;

	if (!(port->flags & BR_MULTICAST_TO_UNICAST))
		return true;

	return ether_addr_equal(src, p->eth_addr);
}

static void __fwd_add_star_excl(struct net_bridge_port_group *pg,
				struct br_ip *sg_ip)
{
	struct net_bridge_port_group_sg_key sg_key;
	struct net_bridge *br = pg->key.port->br;
	struct net_bridge_port_group *src_pg;

	memset(&sg_key, 0, sizeof(sg_key));
	sg_key.port = pg->key.port;
	sg_key.addr = *sg_ip;
	if (br_sg_port_find(br, &sg_key))
		return;

	src_pg = __br_multicast_add_group(br, pg->key.port, sg_ip, pg->eth_addr,
					  MCAST_INCLUDE, false, false);
	if (IS_ERR_OR_NULL(src_pg) ||
	    src_pg->rt_protocol != RTPROT_KERNEL)
		return;

	src_pg->flags |= MDB_PG_FLAGS_STAR_EXCL;
}

static void __fwd_del_star_excl(struct net_bridge_port_group *pg,
				struct br_ip *sg_ip)
{
	struct net_bridge_port_group_sg_key sg_key;
	struct net_bridge *br = pg->key.port->br;
	struct net_bridge_port_group *src_pg;

	memset(&sg_key, 0, sizeof(sg_key));
	sg_key.port = pg->key.port;
	sg_key.addr = *sg_ip;
	src_pg = br_sg_port_find(br, &sg_key);
	if (!src_pg || !(src_pg->flags & MDB_PG_FLAGS_STAR_EXCL) ||
	    src_pg->rt_protocol != RTPROT_KERNEL)
		return;

	br_multicast_find_del_pg(br, src_pg);
}

/* When a port group transitions to (or is added as) EXCLUDE we need to add it
 * to all other ports' S,G entries which are not blocked by the current group
 * for proper replication, the assumption is that any S,G blocked entries
 * are already added so the S,G,port lookup should skip them.
 * When a port group transitions from EXCLUDE -> INCLUDE mode or is being
 * deleted we need to remove it from all ports' S,G entries where it was
 * automatically installed before (i.e. where it's MDB_PG_FLAGS_STAR_EXCL).
 */
void br_multicast_star_g_handle_mode(struct net_bridge_port_group *pg,
				     u8 filter_mode)
{
	struct net_bridge *br = pg->key.port->br;
	struct net_bridge_port_group *pg_lst;
	struct net_bridge_mdb_entry *mp;
	struct br_ip sg_ip;

	if (WARN_ON(!br_multicast_is_star_g(&pg->key.addr)))
		return;

	mp = br_mdb_ip_get(br, &pg->key.addr);
	if (!mp)
		return;

	memset(&sg_ip, 0, sizeof(sg_ip));
	sg_ip = pg->key.addr;
	for (pg_lst = mlock_dereference(mp->ports, br);
	     pg_lst;
	     pg_lst = mlock_dereference(pg_lst->next, br)) {
		struct net_bridge_group_src *src_ent;

		if (pg_lst == pg)
			continue;
		hlist_for_each_entry(src_ent, &pg_lst->src_list, node) {
			if (!(src_ent->flags & BR_SGRP_F_INSTALLED))
				continue;
			sg_ip.src = src_ent->addr.src;
			switch (filter_mode) {
			case MCAST_INCLUDE:
				__fwd_del_star_excl(pg, &sg_ip);
				break;
			case MCAST_EXCLUDE:
				__fwd_add_star_excl(pg, &sg_ip);
				break;
			}
		}
	}
}

/* called when adding a new S,G with host_joined == false by default */
static void br_multicast_sg_host_state(struct net_bridge_mdb_entry *star_mp,
				       struct net_bridge_port_group *sg)
{
	struct net_bridge_mdb_entry *sg_mp;

	if (WARN_ON(!br_multicast_is_star_g(&star_mp->addr)))
		return;
	if (!star_mp->host_joined)
		return;

	sg_mp = br_mdb_ip_get(star_mp->br, &sg->key.addr);
	if (!sg_mp)
		return;
	sg_mp->host_joined = true;
}

/* set the host_joined state of all of *,G's S,G entries */
static void br_multicast_star_g_host_state(struct net_bridge_mdb_entry *star_mp)
{
	struct net_bridge *br = star_mp->br;
	struct net_bridge_mdb_entry *sg_mp;
	struct net_bridge_port_group *pg;
	struct br_ip sg_ip;

	if (WARN_ON(!br_multicast_is_star_g(&star_mp->addr)))
		return;

	memset(&sg_ip, 0, sizeof(sg_ip));
	sg_ip = star_mp->addr;
	for (pg = mlock_dereference(star_mp->ports, br);
	     pg;
	     pg = mlock_dereference(pg->next, br)) {
		struct net_bridge_group_src *src_ent;

		hlist_for_each_entry(src_ent, &pg->src_list, node) {
			if (!(src_ent->flags & BR_SGRP_F_INSTALLED))
				continue;
			sg_ip.src = src_ent->addr.src;
			sg_mp = br_mdb_ip_get(br, &sg_ip);
			if (!sg_mp)
				continue;
			sg_mp->host_joined = star_mp->host_joined;
		}
	}
}

static void br_multicast_sg_del_exclude_ports(struct net_bridge_mdb_entry *sgmp)
{
	struct net_bridge_port_group __rcu **pp;
	struct net_bridge_port_group *p;

	/* *,G exclude ports are only added to S,G entries */
	if (WARN_ON(br_multicast_is_star_g(&sgmp->addr)))
		return;

	/* we need the STAR_EXCLUDE ports if there are non-STAR_EXCLUDE ports
	 * we should ignore perm entries since they're managed by user-space
	 */
	for (pp = &sgmp->ports;
	     (p = mlock_dereference(*pp, sgmp->br)) != NULL;
	     pp = &p->next)
		if (!(p->flags & (MDB_PG_FLAGS_STAR_EXCL |
				  MDB_PG_FLAGS_PERMANENT)))
			return;

	/* currently the host can only have joined the *,G which means
	 * we treat it as EXCLUDE {}, so for an S,G it's considered a
	 * STAR_EXCLUDE entry and we can safely leave it
	 */
	sgmp->host_joined = false;

	for (pp = &sgmp->ports;
	     (p = mlock_dereference(*pp, sgmp->br)) != NULL;) {
		if (!(p->flags & MDB_PG_FLAGS_PERMANENT))
			br_multicast_del_pg(sgmp, p, pp);
		else
			pp = &p->next;
	}
}

void br_multicast_sg_add_exclude_ports(struct net_bridge_mdb_entry *star_mp,
				       struct net_bridge_port_group *sg)
{
	struct net_bridge_port_group_sg_key sg_key;
	struct net_bridge *br = star_mp->br;
	struct net_bridge_port_group *pg;

	if (WARN_ON(br_multicast_is_star_g(&sg->key.addr)))
		return;
	if (WARN_ON(!br_multicast_is_star_g(&star_mp->addr)))
		return;

	br_multicast_sg_host_state(star_mp, sg);
	memset(&sg_key, 0, sizeof(sg_key));
	sg_key.addr = sg->key.addr;
	/* we need to add all exclude ports to the S,G */
	for (pg = mlock_dereference(star_mp->ports, br);
	     pg;
	     pg = mlock_dereference(pg->next, br)) {
		struct net_bridge_port_group *src_pg;

		if (pg == sg || pg->filter_mode == MCAST_INCLUDE)
			continue;

		sg_key.port = pg->key.port;
		if (br_sg_port_find(br, &sg_key))
			continue;

		src_pg = __br_multicast_add_group(br, pg->key.port,
						  &sg->key.addr,
						  sg->eth_addr,
						  MCAST_INCLUDE, false, false);
		if (IS_ERR_OR_NULL(src_pg) ||
		    src_pg->rt_protocol != RTPROT_KERNEL)
			continue;
		src_pg->flags |= MDB_PG_FLAGS_STAR_EXCL;
	}
}

static void br_multicast_fwd_src_add(struct net_bridge_group_src *src)
{
	struct net_bridge_mdb_entry *star_mp;
	struct net_bridge_port_group *sg;
	struct br_ip sg_ip;

	if (src->flags & BR_SGRP_F_INSTALLED)
		return;

	memset(&sg_ip, 0, sizeof(sg_ip));
	sg_ip = src->pg->key.addr;
	sg_ip.src = src->addr.src;
	sg = __br_multicast_add_group(src->br, src->pg->key.port, &sg_ip,
				      src->pg->eth_addr, MCAST_INCLUDE, false,
				      !timer_pending(&src->timer));
	if (IS_ERR_OR_NULL(sg))
		return;
	src->flags |= BR_SGRP_F_INSTALLED;
	sg->flags &= ~MDB_PG_FLAGS_STAR_EXCL;

	/* if it was added by user-space as perm we can skip next steps */
	if (sg->rt_protocol != RTPROT_KERNEL &&
	    (sg->flags & MDB_PG_FLAGS_PERMANENT))
		return;

	/* the kernel is now responsible for removing this S,G */
	del_timer(&sg->timer);
	star_mp = br_mdb_ip_get(src->br, &src->pg->key.addr);
	if (!star_mp)
		return;

	br_multicast_sg_add_exclude_ports(star_mp, sg);
}

static void br_multicast_fwd_src_remove(struct net_bridge_group_src *src,
					bool fastleave)
{
	struct net_bridge_port_group *p, *pg = src->pg;
	struct net_bridge_port_group __rcu **pp;
	struct net_bridge_mdb_entry *mp;
	struct br_ip sg_ip;

	memset(&sg_ip, 0, sizeof(sg_ip));
	sg_ip = pg->key.addr;
	sg_ip.src = src->addr.src;

	mp = br_mdb_ip_get(src->br, &sg_ip);
	if (!mp)
		return;

	for (pp = &mp->ports;
	     (p = mlock_dereference(*pp, src->br)) != NULL;
	     pp = &p->next) {
		if (!br_port_group_equal(p, pg->key.port, pg->eth_addr))
			continue;

		if (p->rt_protocol != RTPROT_KERNEL &&
		    (p->flags & MDB_PG_FLAGS_PERMANENT))
			break;

		if (fastleave)
			p->flags |= MDB_PG_FLAGS_FAST_LEAVE;
		br_multicast_del_pg(mp, p, pp);
		break;
	}
	src->flags &= ~BR_SGRP_F_INSTALLED;
}

/* install S,G and based on src's timer enable or disable forwarding */
static void br_multicast_fwd_src_handle(struct net_bridge_group_src *src)
{
	struct net_bridge_port_group_sg_key sg_key;
	struct net_bridge_port_group *sg;
	u8 old_flags;

	br_multicast_fwd_src_add(src);

	memset(&sg_key, 0, sizeof(sg_key));
	sg_key.addr = src->pg->key.addr;
	sg_key.addr.src = src->addr.src;
	sg_key.port = src->pg->key.port;

	sg = br_sg_port_find(src->br, &sg_key);
	if (!sg || (sg->flags & MDB_PG_FLAGS_PERMANENT))
		return;

	old_flags = sg->flags;
	if (timer_pending(&src->timer))
		sg->flags &= ~MDB_PG_FLAGS_BLOCKED;
	else
		sg->flags |= MDB_PG_FLAGS_BLOCKED;

	if (old_flags != sg->flags) {
		struct net_bridge_mdb_entry *sg_mp;

		sg_mp = br_mdb_ip_get(src->br, &sg_key.addr);
		if (!sg_mp)
			return;
		br_mdb_notify(src->br->dev, sg_mp, sg, RTM_NEWMDB);
	}
}

static void br_multicast_destroy_mdb_entry(struct net_bridge_mcast_gc *gc)
{
	struct net_bridge_mdb_entry *mp;

	mp = container_of(gc, struct net_bridge_mdb_entry, mcast_gc);
	WARN_ON(!hlist_unhashed(&mp->mdb_node));
	WARN_ON(mp->ports);

	del_timer_sync(&mp->timer);
	kfree_rcu(mp, rcu);
}

static void br_multicast_del_mdb_entry(struct net_bridge_mdb_entry *mp)
{
	struct net_bridge *br = mp->br;

	rhashtable_remove_fast(&br->mdb_hash_tbl, &mp->rhnode,
			       br_mdb_rht_params);
	hlist_del_init_rcu(&mp->mdb_node);
	hlist_add_head(&mp->mcast_gc.gc_node, &br->mcast_gc_list);
	queue_work(system_long_wq, &br->mcast_gc_work);
}

static void br_multicast_group_expired(struct timer_list *t)
{
	struct net_bridge_mdb_entry *mp = from_timer(mp, t, timer);
	struct net_bridge *br = mp->br;

	spin_lock(&br->multicast_lock);
	if (hlist_unhashed(&mp->mdb_node) || !netif_running(br->dev) ||
	    timer_pending(&mp->timer))
		goto out;

	br_multicast_host_leave(mp, true);

	if (mp->ports)
		goto out;
	br_multicast_del_mdb_entry(mp);
out:
	spin_unlock(&br->multicast_lock);
}

static void br_multicast_destroy_group_src(struct net_bridge_mcast_gc *gc)
{
	struct net_bridge_group_src *src;

	src = container_of(gc, struct net_bridge_group_src, mcast_gc);
	WARN_ON(!hlist_unhashed(&src->node));

	del_timer_sync(&src->timer);
	kfree_rcu(src, rcu);
}

void br_multicast_del_group_src(struct net_bridge_group_src *src,
				bool fastleave)
{
	struct net_bridge *br = src->pg->key.port->br;

	br_multicast_fwd_src_remove(src, fastleave);
	hlist_del_init_rcu(&src->node);
	src->pg->src_ents--;
	hlist_add_head(&src->mcast_gc.gc_node, &br->mcast_gc_list);
	queue_work(system_long_wq, &br->mcast_gc_work);
}

static void br_multicast_destroy_port_group(struct net_bridge_mcast_gc *gc)
{
	struct net_bridge_port_group *pg;

	pg = container_of(gc, struct net_bridge_port_group, mcast_gc);
	WARN_ON(!hlist_unhashed(&pg->mglist));
	WARN_ON(!hlist_empty(&pg->src_list));

	del_timer_sync(&pg->rexmit_timer);
	del_timer_sync(&pg->timer);
	kfree_rcu(pg, rcu);
}

void br_multicast_del_pg(struct net_bridge_mdb_entry *mp,
			 struct net_bridge_port_group *pg,
			 struct net_bridge_port_group __rcu **pp)
{
	struct net_bridge *br = pg->key.port->br;
	struct net_bridge_group_src *ent;
	struct hlist_node *tmp;

	rcu_assign_pointer(*pp, pg->next);
	hlist_del_init(&pg->mglist);
	br_multicast_eht_clean_sets(pg);
	hlist_for_each_entry_safe(ent, tmp, &pg->src_list, node)
		br_multicast_del_group_src(ent, false);
	br_mdb_notify(br->dev, mp, pg, RTM_DELMDB);
	if (!br_multicast_is_star_g(&mp->addr)) {
		rhashtable_remove_fast(&br->sg_port_tbl, &pg->rhnode,
				       br_sg_port_rht_params);
		br_multicast_sg_del_exclude_ports(mp);
	} else {
		br_multicast_star_g_handle_mode(pg, MCAST_INCLUDE);
	}
	hlist_add_head(&pg->mcast_gc.gc_node, &br->mcast_gc_list);
	queue_work(system_long_wq, &br->mcast_gc_work);

	if (!mp->ports && !mp->host_joined && netif_running(br->dev))
		mod_timer(&mp->timer, jiffies);
}

static void br_multicast_find_del_pg(struct net_bridge *br,
				     struct net_bridge_port_group *pg)
{
	struct net_bridge_port_group __rcu **pp;
	struct net_bridge_mdb_entry *mp;
	struct net_bridge_port_group *p;

	mp = br_mdb_ip_get(br, &pg->key.addr);
	if (WARN_ON(!mp))
		return;

	for (pp = &mp->ports;
	     (p = mlock_dereference(*pp, br)) != NULL;
	     pp = &p->next) {
		if (p != pg)
			continue;

		br_multicast_del_pg(mp, pg, pp);
		return;
	}

	WARN_ON(1);
}

static void br_multicast_port_group_expired(struct timer_list *t)
{
	struct net_bridge_port_group *pg = from_timer(pg, t, timer);
	struct net_bridge_group_src *src_ent;
	struct net_bridge *br = pg->key.port->br;
	struct hlist_node *tmp;
	bool changed;

	spin_lock(&br->multicast_lock);
	if (!netif_running(br->dev) || timer_pending(&pg->timer) ||
	    hlist_unhashed(&pg->mglist) || pg->flags & MDB_PG_FLAGS_PERMANENT)
		goto out;

	changed = !!(pg->filter_mode == MCAST_EXCLUDE);
	pg->filter_mode = MCAST_INCLUDE;
	hlist_for_each_entry_safe(src_ent, tmp, &pg->src_list, node) {
		if (!timer_pending(&src_ent->timer)) {
			br_multicast_del_group_src(src_ent, false);
			changed = true;
		}
	}

	if (hlist_empty(&pg->src_list)) {
		br_multicast_find_del_pg(br, pg);
	} else if (changed) {
		struct net_bridge_mdb_entry *mp = br_mdb_ip_get(br, &pg->key.addr);

		if (changed && br_multicast_is_star_g(&pg->key.addr))
			br_multicast_star_g_handle_mode(pg, MCAST_INCLUDE);

		if (WARN_ON(!mp))
			goto out;
		br_mdb_notify(br->dev, mp, pg, RTM_NEWMDB);
	}
out:
	spin_unlock(&br->multicast_lock);
}

static void br_multicast_gc(struct hlist_head *head)
{
	struct net_bridge_mcast_gc *gcent;
	struct hlist_node *tmp;

	hlist_for_each_entry_safe(gcent, tmp, head, gc_node) {
		hlist_del_init(&gcent->gc_node);
		gcent->destroy(gcent);
	}
}

static struct sk_buff *br_ip4_multicast_alloc_query(struct net_bridge *br,
						    struct net_bridge_port_group *pg,
						    __be32 ip_dst, __be32 group,
						    bool with_srcs, bool over_lmqt,
						    u8 sflag, u8 *igmp_type,
						    bool *need_rexmit)
{
	struct net_bridge_port *p = pg ? pg->key.port : NULL;
	struct net_bridge_group_src *ent;
	size_t pkt_size, igmp_hdr_size;
	unsigned long now = jiffies;
	struct igmpv3_query *ihv3;
	void *csum_start = NULL;
	__sum16 *csum = NULL;
	struct sk_buff *skb;
	struct igmphdr *ih;
	struct ethhdr *eth;
	unsigned long lmqt;
	struct iphdr *iph;
	u16 lmqt_srcs = 0;

	igmp_hdr_size = sizeof(*ih);
	if (br->multicast_igmp_version == 3) {
		igmp_hdr_size = sizeof(*ihv3);
		if (pg && with_srcs) {
			lmqt = now + (br->multicast_last_member_interval *
				      br->multicast_last_member_count);
			hlist_for_each_entry(ent, &pg->src_list, node) {
				if (over_lmqt == time_after(ent->timer.expires,
							    lmqt) &&
				    ent->src_query_rexmit_cnt > 0)
					lmqt_srcs++;
			}

			if (!lmqt_srcs)
				return NULL;
			igmp_hdr_size += lmqt_srcs * sizeof(__be32);
		}
	}

	pkt_size = sizeof(*eth) + sizeof(*iph) + 4 + igmp_hdr_size;
	if ((p && pkt_size > p->dev->mtu) ||
	    pkt_size > br->dev->mtu)
		return NULL;

	skb = netdev_alloc_skb_ip_align(br->dev, pkt_size);
	if (!skb)
		goto out;

	skb->protocol = htons(ETH_P_IP);

	skb_reset_mac_header(skb);
	eth = eth_hdr(skb);

	ether_addr_copy(eth->h_source, br->dev->dev_addr);
	ip_eth_mc_map(ip_dst, eth->h_dest);
	eth->h_proto = htons(ETH_P_IP);
	skb_put(skb, sizeof(*eth));

	skb_set_network_header(skb, skb->len);
	iph = ip_hdr(skb);
	iph->tot_len = htons(pkt_size - sizeof(*eth));

	iph->version = 4;
	iph->ihl = 6;
	iph->tos = 0xc0;
	iph->id = 0;
	iph->frag_off = htons(IP_DF);
	iph->ttl = 1;
	iph->protocol = IPPROTO_IGMP;
	iph->saddr = br_opt_get(br, BROPT_MULTICAST_QUERY_USE_IFADDR) ?
		     inet_select_addr(br->dev, 0, RT_SCOPE_LINK) : 0;
	iph->daddr = ip_dst;
	((u8 *)&iph[1])[0] = IPOPT_RA;
	((u8 *)&iph[1])[1] = 4;
	((u8 *)&iph[1])[2] = 0;
	((u8 *)&iph[1])[3] = 0;
	ip_send_check(iph);
	skb_put(skb, 24);

	skb_set_transport_header(skb, skb->len);
	*igmp_type = IGMP_HOST_MEMBERSHIP_QUERY;

	switch (br->multicast_igmp_version) {
	case 2:
		ih = igmp_hdr(skb);
		ih->type = IGMP_HOST_MEMBERSHIP_QUERY;
		ih->code = (group ? br->multicast_last_member_interval :
				    br->multicast_query_response_interval) /
			   (HZ / IGMP_TIMER_SCALE);
		ih->group = group;
		ih->csum = 0;
		csum = &ih->csum;
		csum_start = (void *)ih;
		break;
	case 3:
		ihv3 = igmpv3_query_hdr(skb);
		ihv3->type = IGMP_HOST_MEMBERSHIP_QUERY;
		ihv3->code = (group ? br->multicast_last_member_interval :
				      br->multicast_query_response_interval) /
			     (HZ / IGMP_TIMER_SCALE);
		ihv3->group = group;
		ihv3->qqic = br->multicast_query_interval / HZ;
		ihv3->nsrcs = htons(lmqt_srcs);
		ihv3->resv = 0;
		ihv3->suppress = sflag;
		ihv3->qrv = 2;
		ihv3->csum = 0;
		csum = &ihv3->csum;
		csum_start = (void *)ihv3;
		if (!pg || !with_srcs)
			break;

		lmqt_srcs = 0;
		hlist_for_each_entry(ent, &pg->src_list, node) {
			if (over_lmqt == time_after(ent->timer.expires,
						    lmqt) &&
			    ent->src_query_rexmit_cnt > 0) {
				ihv3->srcs[lmqt_srcs++] = ent->addr.src.ip4;
				ent->src_query_rexmit_cnt--;
				if (need_rexmit && ent->src_query_rexmit_cnt)
					*need_rexmit = true;
			}
		}
		if (WARN_ON(lmqt_srcs != ntohs(ihv3->nsrcs))) {
			kfree_skb(skb);
			return NULL;
		}
		break;
	}

	if (WARN_ON(!csum || !csum_start)) {
		kfree_skb(skb);
		return NULL;
	}

	*csum = ip_compute_csum(csum_start, igmp_hdr_size);
	skb_put(skb, igmp_hdr_size);
	__skb_pull(skb, sizeof(*eth));

out:
	return skb;
}

#if IS_ENABLED(CONFIG_IPV6)
static struct sk_buff *br_ip6_multicast_alloc_query(struct net_bridge *br,
						    struct net_bridge_port_group *pg,
						    const struct in6_addr *ip6_dst,
						    const struct in6_addr *group,
						    bool with_srcs, bool over_llqt,
						    u8 sflag, u8 *igmp_type,
						    bool *need_rexmit)
{
	struct net_bridge_port *p = pg ? pg->key.port : NULL;
	struct net_bridge_group_src *ent;
	size_t pkt_size, mld_hdr_size;
	unsigned long now = jiffies;
	struct mld2_query *mld2q;
	void *csum_start = NULL;
	unsigned long interval;
	__sum16 *csum = NULL;
	struct ipv6hdr *ip6h;
	struct mld_msg *mldq;
	struct sk_buff *skb;
	unsigned long llqt;
	struct ethhdr *eth;
	u16 llqt_srcs = 0;
	u8 *hopopt;

	mld_hdr_size = sizeof(*mldq);
	if (br->multicast_mld_version == 2) {
		mld_hdr_size = sizeof(*mld2q);
		if (pg && with_srcs) {
			llqt = now + (br->multicast_last_member_interval *
				      br->multicast_last_member_count);
			hlist_for_each_entry(ent, &pg->src_list, node) {
				if (over_llqt == time_after(ent->timer.expires,
							    llqt) &&
				    ent->src_query_rexmit_cnt > 0)
					llqt_srcs++;
			}

			if (!llqt_srcs)
				return NULL;
			mld_hdr_size += llqt_srcs * sizeof(struct in6_addr);
		}
	}

	pkt_size = sizeof(*eth) + sizeof(*ip6h) + 8 + mld_hdr_size;
	if ((p && pkt_size > p->dev->mtu) ||
	    pkt_size > br->dev->mtu)
		return NULL;

	skb = netdev_alloc_skb_ip_align(br->dev, pkt_size);
	if (!skb)
		goto out;

	skb->protocol = htons(ETH_P_IPV6);

	/* Ethernet header */
	skb_reset_mac_header(skb);
	eth = eth_hdr(skb);

	ether_addr_copy(eth->h_source, br->dev->dev_addr);
	eth->h_proto = htons(ETH_P_IPV6);
	skb_put(skb, sizeof(*eth));

	/* IPv6 header + HbH option */
	skb_set_network_header(skb, skb->len);
	ip6h = ipv6_hdr(skb);

	*(__force __be32 *)ip6h = htonl(0x60000000);
	ip6h->payload_len = htons(8 + mld_hdr_size);
	ip6h->nexthdr = IPPROTO_HOPOPTS;
	ip6h->hop_limit = 1;
	ip6h->daddr = *ip6_dst;
	if (ipv6_dev_get_saddr(dev_net(br->dev), br->dev, &ip6h->daddr, 0,
			       &ip6h->saddr)) {
		kfree_skb(skb);
		br_opt_toggle(br, BROPT_HAS_IPV6_ADDR, false);
		return NULL;
	}

	br_opt_toggle(br, BROPT_HAS_IPV6_ADDR, true);
	ipv6_eth_mc_map(&ip6h->daddr, eth->h_dest);

	hopopt = (u8 *)(ip6h + 1);
	hopopt[0] = IPPROTO_ICMPV6;		/* next hdr */
	hopopt[1] = 0;				/* length of HbH */
	hopopt[2] = IPV6_TLV_ROUTERALERT;	/* Router Alert */
	hopopt[3] = 2;				/* Length of RA Option */
	hopopt[4] = 0;				/* Type = 0x0000 (MLD) */
	hopopt[5] = 0;
	hopopt[6] = IPV6_TLV_PAD1;		/* Pad1 */
	hopopt[7] = IPV6_TLV_PAD1;		/* Pad1 */

	skb_put(skb, sizeof(*ip6h) + 8);

	/* ICMPv6 */
	skb_set_transport_header(skb, skb->len);
	interval = ipv6_addr_any(group) ?
			br->multicast_query_response_interval :
			br->multicast_last_member_interval;
	*igmp_type = ICMPV6_MGM_QUERY;
	switch (br->multicast_mld_version) {
	case 1:
		mldq = (struct mld_msg *)icmp6_hdr(skb);
		mldq->mld_type = ICMPV6_MGM_QUERY;
		mldq->mld_code = 0;
		mldq->mld_cksum = 0;
		mldq->mld_maxdelay = htons((u16)jiffies_to_msecs(interval));
		mldq->mld_reserved = 0;
		mldq->mld_mca = *group;
		csum = &mldq->mld_cksum;
		csum_start = (void *)mldq;
		break;
	case 2:
		mld2q = (struct mld2_query *)icmp6_hdr(skb);
		mld2q->mld2q_mrc = htons((u16)jiffies_to_msecs(interval));
		mld2q->mld2q_type = ICMPV6_MGM_QUERY;
		mld2q->mld2q_code = 0;
		mld2q->mld2q_cksum = 0;
		mld2q->mld2q_resv1 = 0;
		mld2q->mld2q_resv2 = 0;
		mld2q->mld2q_suppress = sflag;
		mld2q->mld2q_qrv = 2;
		mld2q->mld2q_nsrcs = htons(llqt_srcs);
		mld2q->mld2q_qqic = br->multicast_query_interval / HZ;
		mld2q->mld2q_mca = *group;
		csum = &mld2q->mld2q_cksum;
		csum_start = (void *)mld2q;
		if (!pg || !with_srcs)
			break;

		llqt_srcs = 0;
		hlist_for_each_entry(ent, &pg->src_list, node) {
			if (over_llqt == time_after(ent->timer.expires,
						    llqt) &&
			    ent->src_query_rexmit_cnt > 0) {
				mld2q->mld2q_srcs[llqt_srcs++] = ent->addr.src.ip6;
				ent->src_query_rexmit_cnt--;
				if (need_rexmit && ent->src_query_rexmit_cnt)
					*need_rexmit = true;
			}
		}
		if (WARN_ON(llqt_srcs != ntohs(mld2q->mld2q_nsrcs))) {
			kfree_skb(skb);
			return NULL;
		}
		break;
	}

	if (WARN_ON(!csum || !csum_start)) {
		kfree_skb(skb);
		return NULL;
	}

	*csum = csum_ipv6_magic(&ip6h->saddr, &ip6h->daddr, mld_hdr_size,
				IPPROTO_ICMPV6,
				csum_partial(csum_start, mld_hdr_size, 0));
	skb_put(skb, mld_hdr_size);
	__skb_pull(skb, sizeof(*eth));

out:
	return skb;
}
#endif

static struct sk_buff *br_multicast_alloc_query(struct net_bridge *br,
						struct net_bridge_port_group *pg,
						struct br_ip *ip_dst,
						struct br_ip *group,
						bool with_srcs, bool over_lmqt,
						u8 sflag, u8 *igmp_type,
						bool *need_rexmit)
{
	__be32 ip4_dst;

	switch (group->proto) {
	case htons(ETH_P_IP):
		ip4_dst = ip_dst ? ip_dst->dst.ip4 : htonl(INADDR_ALLHOSTS_GROUP);
		return br_ip4_multicast_alloc_query(br, pg,
						    ip4_dst, group->dst.ip4,
						    with_srcs, over_lmqt,
						    sflag, igmp_type,
						    need_rexmit);
#if IS_ENABLED(CONFIG_IPV6)
	case htons(ETH_P_IPV6): {
		struct in6_addr ip6_dst;

		if (ip_dst)
			ip6_dst = ip_dst->dst.ip6;
		else
			ipv6_addr_set(&ip6_dst, htonl(0xff020000), 0, 0,
				      htonl(1));

		return br_ip6_multicast_alloc_query(br, pg,
						    &ip6_dst, &group->dst.ip6,
						    with_srcs, over_lmqt,
						    sflag, igmp_type,
						    need_rexmit);
	}
#endif
	}
	return NULL;
}

struct net_bridge_mdb_entry *br_multicast_new_group(struct net_bridge *br,
						    struct br_ip *group)
{
	struct net_bridge_mdb_entry *mp;
	int err;

	mp = br_mdb_ip_get(br, group);
	if (mp)
		return mp;

	if (atomic_read(&br->mdb_hash_tbl.nelems) >= br->hash_max) {
		br_opt_toggle(br, BROPT_MULTICAST_ENABLED, false);
		return ERR_PTR(-E2BIG);
	}

	mp = kzalloc(sizeof(*mp), GFP_ATOMIC);
	if (unlikely(!mp))
		return ERR_PTR(-ENOMEM);

	mp->br = br;
	mp->addr = *group;
	mp->mcast_gc.destroy = br_multicast_destroy_mdb_entry;
	timer_setup(&mp->timer, br_multicast_group_expired, 0);
	err = rhashtable_lookup_insert_fast(&br->mdb_hash_tbl, &mp->rhnode,
					    br_mdb_rht_params);
	if (err) {
		kfree(mp);
		mp = ERR_PTR(err);
	} else {
		hlist_add_head_rcu(&mp->mdb_node, &br->mdb_list);
	}

	return mp;
}

static void br_multicast_group_src_expired(struct timer_list *t)
{
	struct net_bridge_group_src *src = from_timer(src, t, timer);
	struct net_bridge_port_group *pg;
	struct net_bridge *br = src->br;

	spin_lock(&br->multicast_lock);
	if (hlist_unhashed(&src->node) || !netif_running(br->dev) ||
	    timer_pending(&src->timer))
		goto out;

	pg = src->pg;
	if (pg->filter_mode == MCAST_INCLUDE) {
		br_multicast_del_group_src(src, false);
		if (!hlist_empty(&pg->src_list))
			goto out;
		br_multicast_find_del_pg(br, pg);
	} else {
		br_multicast_fwd_src_handle(src);
	}

out:
	spin_unlock(&br->multicast_lock);
}

struct net_bridge_group_src *
br_multicast_find_group_src(struct net_bridge_port_group *pg, struct br_ip *ip)
{
	struct net_bridge_group_src *ent;

	switch (ip->proto) {
	case htons(ETH_P_IP):
		hlist_for_each_entry(ent, &pg->src_list, node)
			if (ip->src.ip4 == ent->addr.src.ip4)
				return ent;
		break;
#if IS_ENABLED(CONFIG_IPV6)
	case htons(ETH_P_IPV6):
		hlist_for_each_entry(ent, &pg->src_list, node)
			if (!ipv6_addr_cmp(&ent->addr.src.ip6, &ip->src.ip6))
				return ent;
		break;
#endif
	}

	return NULL;
}

static struct net_bridge_group_src *
br_multicast_new_group_src(struct net_bridge_port_group *pg, struct br_ip *src_ip)
{
	struct net_bridge_group_src *grp_src;

	if (unlikely(pg->src_ents >= PG_SRC_ENT_LIMIT))
		return NULL;

	switch (src_ip->proto) {
	case htons(ETH_P_IP):
		if (ipv4_is_zeronet(src_ip->src.ip4) ||
		    ipv4_is_multicast(src_ip->src.ip4))
			return NULL;
		break;
#if IS_ENABLED(CONFIG_IPV6)
	case htons(ETH_P_IPV6):
		if (ipv6_addr_any(&src_ip->src.ip6) ||
		    ipv6_addr_is_multicast(&src_ip->src.ip6))
			return NULL;
		break;
#endif
	}

	grp_src = kzalloc(sizeof(*grp_src), GFP_ATOMIC);
	if (unlikely(!grp_src))
		return NULL;

	grp_src->pg = pg;
	grp_src->br = pg->key.port->br;
	grp_src->addr = *src_ip;
	grp_src->mcast_gc.destroy = br_multicast_destroy_group_src;
	timer_setup(&grp_src->timer, br_multicast_group_src_expired, 0);

	hlist_add_head_rcu(&grp_src->node, &pg->src_list);
	pg->src_ents++;

	return grp_src;
}

struct net_bridge_port_group *br_multicast_new_port_group(
			struct net_bridge_port *port,
			struct br_ip *group,
			struct net_bridge_port_group __rcu *next,
			unsigned char flags,
			const unsigned char *src,
			u8 filter_mode,
			u8 rt_protocol)
{
	struct net_bridge_port_group *p;

	p = kzalloc(sizeof(*p), GFP_ATOMIC);
	if (unlikely(!p))
		return NULL;

	p->key.addr = *group;
	p->key.port = port;
	p->flags = flags;
	p->filter_mode = filter_mode;
	p->rt_protocol = rt_protocol;
	p->eht_host_tree = RB_ROOT;
	p->eht_set_tree = RB_ROOT;
	p->mcast_gc.destroy = br_multicast_destroy_port_group;
	INIT_HLIST_HEAD(&p->src_list);

	if (!br_multicast_is_star_g(group) &&
	    rhashtable_lookup_insert_fast(&port->br->sg_port_tbl, &p->rhnode,
					  br_sg_port_rht_params)) {
		kfree(p);
		return NULL;
	}

	rcu_assign_pointer(p->next, next);
	timer_setup(&p->timer, br_multicast_port_group_expired, 0);
	timer_setup(&p->rexmit_timer, br_multicast_port_group_rexmit, 0);
	hlist_add_head(&p->mglist, &port->mglist);

	if (src)
		memcpy(p->eth_addr, src, ETH_ALEN);
	else
		eth_broadcast_addr(p->eth_addr);

	return p;
}

void br_multicast_host_join(struct net_bridge_mdb_entry *mp, bool notify)
{
	if (!mp->host_joined) {
		mp->host_joined = true;
		if (br_multicast_is_star_g(&mp->addr))
			br_multicast_star_g_host_state(mp);
		if (notify)
			br_mdb_notify(mp->br->dev, mp, NULL, RTM_NEWMDB);
	}

	if (br_group_is_l2(&mp->addr))
		return;

	mod_timer(&mp->timer, jiffies + mp->br->multicast_membership_interval);
}

void br_multicast_host_leave(struct net_bridge_mdb_entry *mp, bool notify)
{
	if (!mp->host_joined)
		return;

	mp->host_joined = false;
	if (br_multicast_is_star_g(&mp->addr))
		br_multicast_star_g_host_state(mp);
	if (notify)
		br_mdb_notify(mp->br->dev, mp, NULL, RTM_DELMDB);
}

static struct net_bridge_port_group *
__br_multicast_add_group(struct net_bridge *br,
			 struct net_bridge_port *port,
			 struct br_ip *group,
			 const unsigned char *src,
			 u8 filter_mode,
			 bool igmpv2_mldv1,
			 bool blocked)
{
	struct net_bridge_port_group __rcu **pp;
	struct net_bridge_port_group *p = NULL;
	struct net_bridge_mdb_entry *mp;
	unsigned long now = jiffies;

	if (!netif_running(br->dev) ||
	    (port && port->state == BR_STATE_DISABLED))
		goto out;

	mp = br_multicast_new_group(br, group);
	if (IS_ERR(mp))
		return ERR_CAST(mp);

	if (!port) {
		br_multicast_host_join(mp, true);
		goto out;
	}

	for (pp = &mp->ports;
	     (p = mlock_dereference(*pp, br)) != NULL;
	     pp = &p->next) {
		if (br_port_group_equal(p, port, src))
			goto found;
		if ((unsigned long)p->key.port < (unsigned long)port)
			break;
	}

	p = br_multicast_new_port_group(port, group, *pp, 0, src,
					filter_mode, RTPROT_KERNEL);
	if (unlikely(!p)) {
		p = ERR_PTR(-ENOMEM);
		goto out;
	}
	rcu_assign_pointer(*pp, p);
	if (blocked)
		p->flags |= MDB_PG_FLAGS_BLOCKED;
	br_mdb_notify(br->dev, mp, p, RTM_NEWMDB);

found:
	if (igmpv2_mldv1)
		mod_timer(&p->timer, now + br->multicast_membership_interval);

out:
	return p;
}

static int br_multicast_add_group(struct net_bridge *br,
				  struct net_bridge_port *port,
				  struct br_ip *group,
				  const unsigned char *src,
				  u8 filter_mode,
				  bool igmpv2_mldv1)
{
	struct net_bridge_port_group *pg;
	int err;

	spin_lock(&br->multicast_lock);
	pg = __br_multicast_add_group(br, port, group, src, filter_mode,
				      igmpv2_mldv1, false);
	/* NULL is considered valid for host joined groups */
	err = PTR_ERR_OR_ZERO(pg);
	spin_unlock(&br->multicast_lock);

	return err;
}

static int br_ip4_multicast_add_group(struct net_bridge *br,
				      struct net_bridge_port *port,
				      __be32 group,
				      __u16 vid,
				      const unsigned char *src,
				      bool igmpv2)
{
	struct br_ip br_group;
	u8 filter_mode;

	if (ipv4_is_local_multicast(group))
		return 0;

	memset(&br_group, 0, sizeof(br_group));
	br_group.dst.ip4 = group;
	br_group.proto = htons(ETH_P_IP);
	br_group.vid = vid;
	filter_mode = igmpv2 ? MCAST_EXCLUDE : MCAST_INCLUDE;

	return br_multicast_add_group(br, port, &br_group, src, filter_mode,
				      igmpv2);
}

#if IS_ENABLED(CONFIG_IPV6)
static int br_ip6_multicast_add_group(struct net_bridge *br,
				      struct net_bridge_port *port,
				      const struct in6_addr *group,
				      __u16 vid,
				      const unsigned char *src,
				      bool mldv1)
{
	struct br_ip br_group;
	u8 filter_mode;

	if (ipv6_addr_is_ll_all_nodes(group))
		return 0;

	memset(&br_group, 0, sizeof(br_group));
	br_group.dst.ip6 = *group;
	br_group.proto = htons(ETH_P_IPV6);
	br_group.vid = vid;
	filter_mode = mldv1 ? MCAST_EXCLUDE : MCAST_INCLUDE;

	return br_multicast_add_group(br, port, &br_group, src, filter_mode,
				      mldv1);
}
#endif

static void br_multicast_router_expired(struct timer_list *t)
{
	struct net_bridge_port *port =
			from_timer(port, t, multicast_router_timer);
	struct net_bridge *br = port->br;

	spin_lock(&br->multicast_lock);
	if (port->multicast_router == MDB_RTR_TYPE_DISABLED ||
	    port->multicast_router == MDB_RTR_TYPE_PERM ||
	    timer_pending(&port->multicast_router_timer))
		goto out;

	__del_port_router(port);
out:
	spin_unlock(&br->multicast_lock);
}

static void br_mc_router_state_change(struct net_bridge *p,
				      bool is_mc_router)
{
	struct switchdev_attr attr = {
		.orig_dev = p->dev,
		.id = SWITCHDEV_ATTR_ID_BRIDGE_MROUTER,
		.flags = SWITCHDEV_F_DEFER,
		.u.mrouter = is_mc_router,
	};

	switchdev_port_attr_set(p->dev, &attr, NULL);
}

static void br_multicast_local_router_expired(struct timer_list *t)
{
	struct net_bridge *br = from_timer(br, t, multicast_router_timer);

	spin_lock(&br->multicast_lock);
	if (br->multicast_router == MDB_RTR_TYPE_DISABLED ||
	    br->multicast_router == MDB_RTR_TYPE_PERM ||
	    timer_pending(&br->multicast_router_timer))
		goto out;

	br_mc_router_state_change(br, false);
out:
	spin_unlock(&br->multicast_lock);
}

static void br_multicast_querier_expired(struct net_bridge *br,
					 struct bridge_mcast_own_query *query)
{
	spin_lock(&br->multicast_lock);
	if (!netif_running(br->dev) || !br_opt_get(br, BROPT_MULTICAST_ENABLED))
		goto out;

	br_multicast_start_querier(br, query);

out:
	spin_unlock(&br->multicast_lock);
}

static void br_ip4_multicast_querier_expired(struct timer_list *t)
{
	struct net_bridge *br = from_timer(br, t, ip4_other_query.timer);

	br_multicast_querier_expired(br, &br->ip4_own_query);
}

#if IS_ENABLED(CONFIG_IPV6)
static void br_ip6_multicast_querier_expired(struct timer_list *t)
{
	struct net_bridge *br = from_timer(br, t, ip6_other_query.timer);

	br_multicast_querier_expired(br, &br->ip6_own_query);
}
#endif

static void br_multicast_select_own_querier(struct net_bridge *br,
					    struct br_ip *ip,
					    struct sk_buff *skb)
{
	if (ip->proto == htons(ETH_P_IP))
		br->ip4_querier.addr.src.ip4 = ip_hdr(skb)->saddr;
#if IS_ENABLED(CONFIG_IPV6)
	else
		br->ip6_querier.addr.src.ip6 = ipv6_hdr(skb)->saddr;
#endif
}

static void __br_multicast_send_query(struct net_bridge *br,
				      struct net_bridge_port *port,
				      struct net_bridge_port_group *pg,
				      struct br_ip *ip_dst,
				      struct br_ip *group,
				      bool with_srcs,
				      u8 sflag,
				      bool *need_rexmit)
{
	bool over_lmqt = !!sflag;
	struct sk_buff *skb;
	u8 igmp_type;

again_under_lmqt:
	skb = br_multicast_alloc_query(br, pg, ip_dst, group, with_srcs,
				       over_lmqt, sflag, &igmp_type,
				       need_rexmit);
	if (!skb)
		return;

	if (port) {
		skb->dev = port->dev;
		br_multicast_count(br, port, skb, igmp_type,
				   BR_MCAST_DIR_TX);
		NF_HOOK(NFPROTO_BRIDGE, NF_BR_LOCAL_OUT,
			dev_net(port->dev), NULL, skb, NULL, skb->dev,
			br_dev_queue_push_xmit);

		if (over_lmqt && with_srcs && sflag) {
			over_lmqt = false;
			goto again_under_lmqt;
		}
	} else {
		br_multicast_select_own_querier(br, group, skb);
		br_multicast_count(br, port, skb, igmp_type,
				   BR_MCAST_DIR_RX);
		netif_rx(skb);
	}
}

static void br_multicast_send_query(struct net_bridge *br,
				    struct net_bridge_port *port,
				    struct bridge_mcast_own_query *own_query)
{
	struct bridge_mcast_other_query *other_query = NULL;
	struct br_ip br_group;
	unsigned long time;

	if (!netif_running(br->dev) ||
	    !br_opt_get(br, BROPT_MULTICAST_ENABLED) ||
	    !br_opt_get(br, BROPT_MULTICAST_QUERIER))
		return;

	memset(&br_group.dst, 0, sizeof(br_group.dst));

	if (port ? (own_query == &port->ip4_own_query) :
		   (own_query == &br->ip4_own_query)) {
		other_query = &br->ip4_other_query;
		br_group.proto = htons(ETH_P_IP);
#if IS_ENABLED(CONFIG_IPV6)
	} else {
		other_query = &br->ip6_other_query;
		br_group.proto = htons(ETH_P_IPV6);
#endif
	}

	if (!other_query || timer_pending(&other_query->timer))
		return;

	__br_multicast_send_query(br, port, NULL, NULL, &br_group, false, 0,
				  NULL);

	time = jiffies;
	time += own_query->startup_sent < br->multicast_startup_query_count ?
		br->multicast_startup_query_interval :
		br->multicast_query_interval;
	mod_timer(&own_query->timer, time);
}

static void
br_multicast_port_query_expired(struct net_bridge_port *port,
				struct bridge_mcast_own_query *query)
{
	struct net_bridge *br = port->br;

	spin_lock(&br->multicast_lock);
	if (port->state == BR_STATE_DISABLED ||
	    port->state == BR_STATE_BLOCKING)
		goto out;

	if (query->startup_sent < br->multicast_startup_query_count)
		query->startup_sent++;

	br_multicast_send_query(port->br, port, query);

out:
	spin_unlock(&br->multicast_lock);
}

static void br_ip4_multicast_port_query_expired(struct timer_list *t)
{
	struct net_bridge_port *port = from_timer(port, t, ip4_own_query.timer);

	br_multicast_port_query_expired(port, &port->ip4_own_query);
}

#if IS_ENABLED(CONFIG_IPV6)
static void br_ip6_multicast_port_query_expired(struct timer_list *t)
{
	struct net_bridge_port *port = from_timer(port, t, ip6_own_query.timer);

	br_multicast_port_query_expired(port, &port->ip6_own_query);
}
#endif

static void br_multicast_port_group_rexmit(struct timer_list *t)
{
	struct net_bridge_port_group *pg = from_timer(pg, t, rexmit_timer);
	struct bridge_mcast_other_query *other_query = NULL;
	struct net_bridge *br = pg->key.port->br;
	bool need_rexmit = false;

	spin_lock(&br->multicast_lock);
	if (!netif_running(br->dev) || hlist_unhashed(&pg->mglist) ||
	    !br_opt_get(br, BROPT_MULTICAST_ENABLED) ||
	    !br_opt_get(br, BROPT_MULTICAST_QUERIER))
		goto out;

	if (pg->key.addr.proto == htons(ETH_P_IP))
		other_query = &br->ip4_other_query;
#if IS_ENABLED(CONFIG_IPV6)
	else
		other_query = &br->ip6_other_query;
#endif

	if (!other_query || timer_pending(&other_query->timer))
		goto out;

	if (pg->grp_query_rexmit_cnt) {
		pg->grp_query_rexmit_cnt--;
		__br_multicast_send_query(br, pg->key.port, pg, &pg->key.addr,
					  &pg->key.addr, false, 1, NULL);
	}
	__br_multicast_send_query(br, pg->key.port, pg, &pg->key.addr,
				  &pg->key.addr, true, 0, &need_rexmit);

	if (pg->grp_query_rexmit_cnt || need_rexmit)
		mod_timer(&pg->rexmit_timer, jiffies +
					     br->multicast_last_member_interval);
out:
	spin_unlock(&br->multicast_lock);
}

static int br_mc_disabled_update(struct net_device *dev, bool value,
				 struct netlink_ext_ack *extack)
{
	struct switchdev_attr attr = {
		.orig_dev = dev,
		.id = SWITCHDEV_ATTR_ID_BRIDGE_MC_DISABLED,
		.flags = SWITCHDEV_F_DEFER,
		.u.mc_disabled = !value,
	};

	return switchdev_port_attr_set(dev, &attr, extack);
}

int br_multicast_add_port(struct net_bridge_port *port)
{
	int err;

	port->multicast_router = MDB_RTR_TYPE_TEMP_QUERY;
	port->multicast_eht_hosts_limit = BR_MCAST_DEFAULT_EHT_HOSTS_LIMIT;

	timer_setup(&port->multicast_router_timer,
		    br_multicast_router_expired, 0);
	timer_setup(&port->ip4_own_query.timer,
		    br_ip4_multicast_port_query_expired, 0);
#if IS_ENABLED(CONFIG_IPV6)
	timer_setup(&port->ip6_own_query.timer,
		    br_ip6_multicast_port_query_expired, 0);
#endif
	err = br_mc_disabled_update(port->dev,
				    br_opt_get(port->br,
					       BROPT_MULTICAST_ENABLED),
				    NULL);
	if (err && err != -EOPNOTSUPP)
		return err;

	port->mcast_stats = netdev_alloc_pcpu_stats(struct bridge_mcast_stats);
	if (!port->mcast_stats)
		return -ENOMEM;

	return 0;
}

void br_multicast_del_port(struct net_bridge_port *port)
{
	struct net_bridge *br = port->br;
	struct net_bridge_port_group *pg;
	HLIST_HEAD(deleted_head);
	struct hlist_node *n;

	/* Take care of the remaining groups, only perm ones should be left */
	spin_lock_bh(&br->multicast_lock);
	hlist_for_each_entry_safe(pg, n, &port->mglist, mglist)
		br_multicast_find_del_pg(br, pg);
	hlist_move_list(&br->mcast_gc_list, &deleted_head);
	spin_unlock_bh(&br->multicast_lock);
	br_multicast_gc(&deleted_head);
	del_timer_sync(&port->multicast_router_timer);
	free_percpu(port->mcast_stats);
}

static void br_multicast_enable(struct bridge_mcast_own_query *query)
{
	query->startup_sent = 0;

	if (try_to_del_timer_sync(&query->timer) >= 0 ||
	    del_timer(&query->timer))
		mod_timer(&query->timer, jiffies);
}

static void __br_multicast_enable_port(struct net_bridge_port *port)
{
	struct net_bridge *br = port->br;

	if (!br_opt_get(br, BROPT_MULTICAST_ENABLED) || !netif_running(br->dev))
		return;

	br_multicast_enable(&port->ip4_own_query);
#if IS_ENABLED(CONFIG_IPV6)
	br_multicast_enable(&port->ip6_own_query);
#endif
	if (port->multicast_router == MDB_RTR_TYPE_PERM &&
	    hlist_unhashed(&port->rlist))
		br_multicast_add_router(br, port);
}

void br_multicast_enable_port(struct net_bridge_port *port)
{
	struct net_bridge *br = port->br;

	spin_lock(&br->multicast_lock);
	__br_multicast_enable_port(port);
	spin_unlock(&br->multicast_lock);
}

void br_multicast_disable_port(struct net_bridge_port *port)
{
	struct net_bridge *br = port->br;
	struct net_bridge_port_group *pg;
	struct hlist_node *n;

	spin_lock(&br->multicast_lock);
	hlist_for_each_entry_safe(pg, n, &port->mglist, mglist)
		if (!(pg->flags & MDB_PG_FLAGS_PERMANENT))
			br_multicast_find_del_pg(br, pg);

	__del_port_router(port);

	del_timer(&port->multicast_router_timer);
	del_timer(&port->ip4_own_query.timer);
#if IS_ENABLED(CONFIG_IPV6)
	del_timer(&port->ip6_own_query.timer);
#endif
	spin_unlock(&br->multicast_lock);
}

static int __grp_src_delete_marked(struct net_bridge_port_group *pg)
{
	struct net_bridge_group_src *ent;
	struct hlist_node *tmp;
	int deleted = 0;

	hlist_for_each_entry_safe(ent, tmp, &pg->src_list, node)
		if (ent->flags & BR_SGRP_F_DELETE) {
			br_multicast_del_group_src(ent, false);
			deleted++;
		}

	return deleted;
}

static void __grp_src_mod_timer(struct net_bridge_group_src *src,
				unsigned long expires)
{
	mod_timer(&src->timer, expires);
	br_multicast_fwd_src_handle(src);
}

static void __grp_src_query_marked_and_rexmit(struct net_bridge_port_group *pg)
{
	struct bridge_mcast_other_query *other_query = NULL;
	struct net_bridge *br = pg->key.port->br;
	u32 lmqc = br->multicast_last_member_count;
	unsigned long lmqt, lmi, now = jiffies;
	struct net_bridge_group_src *ent;

	if (!netif_running(br->dev) ||
	    !br_opt_get(br, BROPT_MULTICAST_ENABLED))
		return;

	if (pg->key.addr.proto == htons(ETH_P_IP))
		other_query = &br->ip4_other_query;
#if IS_ENABLED(CONFIG_IPV6)
	else
		other_query = &br->ip6_other_query;
#endif

	lmqt = now + br_multicast_lmqt(br);
	hlist_for_each_entry(ent, &pg->src_list, node) {
		if (ent->flags & BR_SGRP_F_SEND) {
			ent->flags &= ~BR_SGRP_F_SEND;
			if (ent->timer.expires > lmqt) {
				if (br_opt_get(br, BROPT_MULTICAST_QUERIER) &&
				    other_query &&
				    !timer_pending(&other_query->timer))
					ent->src_query_rexmit_cnt = lmqc;
				__grp_src_mod_timer(ent, lmqt);
			}
		}
	}

	if (!br_opt_get(br, BROPT_MULTICAST_QUERIER) ||
	    !other_query || timer_pending(&other_query->timer))
		return;

	__br_multicast_send_query(br, pg->key.port, pg, &pg->key.addr,
				  &pg->key.addr, true, 1, NULL);

	lmi = now + br->multicast_last_member_interval;
	if (!timer_pending(&pg->rexmit_timer) ||
	    time_after(pg->rexmit_timer.expires, lmi))
		mod_timer(&pg->rexmit_timer, lmi);
}

static void __grp_send_query_and_rexmit(struct net_bridge_port_group *pg)
{
	struct bridge_mcast_other_query *other_query = NULL;
	struct net_bridge *br = pg->key.port->br;
	unsigned long now = jiffies, lmi;

	if (!netif_running(br->dev) ||
	    !br_opt_get(br, BROPT_MULTICAST_ENABLED))
		return;

	if (pg->key.addr.proto == htons(ETH_P_IP))
		other_query = &br->ip4_other_query;
#if IS_ENABLED(CONFIG_IPV6)
	else
		other_query = &br->ip6_other_query;
#endif

	if (br_opt_get(br, BROPT_MULTICAST_QUERIER) &&
	    other_query && !timer_pending(&other_query->timer)) {
		lmi = now + br->multicast_last_member_interval;
		pg->grp_query_rexmit_cnt = br->multicast_last_member_count - 1;
		__br_multicast_send_query(br, pg->key.port, pg, &pg->key.addr,
					  &pg->key.addr, false, 0, NULL);
		if (!timer_pending(&pg->rexmit_timer) ||
		    time_after(pg->rexmit_timer.expires, lmi))
			mod_timer(&pg->rexmit_timer, lmi);
	}

	if (pg->filter_mode == MCAST_EXCLUDE &&
	    (!timer_pending(&pg->timer) ||
	     time_after(pg->timer.expires, now + br_multicast_lmqt(br))))
		mod_timer(&pg->timer, now + br_multicast_lmqt(br));
}

/* State          Msg type      New state                Actions
 * INCLUDE (A)    IS_IN (B)     INCLUDE (A+B)            (B)=GMI
 * INCLUDE (A)    ALLOW (B)     INCLUDE (A+B)            (B)=GMI
 * EXCLUDE (X,Y)  ALLOW (A)     EXCLUDE (X+A,Y-A)        (A)=GMI
 */
static bool br_multicast_isinc_allow(struct net_bridge_port_group *pg, void *h_addr,
				     void *srcs, u32 nsrcs, size_t addr_size,
				     int grec_type)
{
	struct net_bridge *br = pg->key.port->br;
	struct net_bridge_group_src *ent;
	unsigned long now = jiffies;
	bool changed = false;
	struct br_ip src_ip;
	u32 src_idx;

	memset(&src_ip, 0, sizeof(src_ip));
	src_ip.proto = pg->key.addr.proto;
	for (src_idx = 0; src_idx < nsrcs; src_idx++) {
		memcpy(&src_ip.src, srcs + (src_idx * addr_size), addr_size);
		ent = br_multicast_find_group_src(pg, &src_ip);
		if (!ent) {
			ent = br_multicast_new_group_src(pg, &src_ip);
			if (ent)
				changed = true;
		}

		if (ent)
			__grp_src_mod_timer(ent, now + br_multicast_gmi(br));
	}

	if (br_multicast_eht_handle(pg, h_addr, srcs, nsrcs, addr_size, grec_type))
		changed = true;

	return changed;
}

/* State          Msg type      New state                Actions
 * INCLUDE (A)    IS_EX (B)     EXCLUDE (A*B,B-A)        (B-A)=0
 *                                                       Delete (A-B)
 *                                                       Group Timer=GMI
 */
static void __grp_src_isexc_incl(struct net_bridge_port_group *pg, void *h_addr,
				 void *srcs, u32 nsrcs, size_t addr_size,
				 int grec_type)
{
	struct net_bridge_group_src *ent;
	struct br_ip src_ip;
	u32 src_idx;

	hlist_for_each_entry(ent, &pg->src_list, node)
		ent->flags |= BR_SGRP_F_DELETE;

	memset(&src_ip, 0, sizeof(src_ip));
	src_ip.proto = pg->key.addr.proto;
	for (src_idx = 0; src_idx < nsrcs; src_idx++) {
		memcpy(&src_ip.src, srcs + (src_idx * addr_size), addr_size);
		ent = br_multicast_find_group_src(pg, &src_ip);
		if (ent)
			ent->flags &= ~BR_SGRP_F_DELETE;
		else
			ent = br_multicast_new_group_src(pg, &src_ip);
		if (ent)
			br_multicast_fwd_src_handle(ent);
	}

	br_multicast_eht_handle(pg, h_addr, srcs, nsrcs, addr_size, grec_type);

	__grp_src_delete_marked(pg);
}

/* State          Msg type      New state                Actions
 * EXCLUDE (X,Y)  IS_EX (A)     EXCLUDE (A-Y,Y*A)        (A-X-Y)=GMI
 *                                                       Delete (X-A)
 *                                                       Delete (Y-A)
 *                                                       Group Timer=GMI
 */
static bool __grp_src_isexc_excl(struct net_bridge_port_group *pg, void *h_addr,
				 void *srcs, u32 nsrcs, size_t addr_size,
				 int grec_type)
{
	struct net_bridge *br = pg->key.port->br;
	struct net_bridge_group_src *ent;
	unsigned long now = jiffies;
	bool changed = false;
	struct br_ip src_ip;
	u32 src_idx;

	hlist_for_each_entry(ent, &pg->src_list, node)
		ent->flags |= BR_SGRP_F_DELETE;

	memset(&src_ip, 0, sizeof(src_ip));
	src_ip.proto = pg->key.addr.proto;
	for (src_idx = 0; src_idx < nsrcs; src_idx++) {
		memcpy(&src_ip.src, srcs + (src_idx * addr_size), addr_size);
		ent = br_multicast_find_group_src(pg, &src_ip);
		if (ent) {
			ent->flags &= ~BR_SGRP_F_DELETE;
		} else {
			ent = br_multicast_new_group_src(pg, &src_ip);
			if (ent) {
				__grp_src_mod_timer(ent,
						    now + br_multicast_gmi(br));
				changed = true;
			}
		}
	}

	if (br_multicast_eht_handle(pg, h_addr, srcs, nsrcs, addr_size, grec_type))
		changed = true;

	if (__grp_src_delete_marked(pg))
		changed = true;

	return changed;
}

static bool br_multicast_isexc(struct net_bridge_port_group *pg, void *h_addr,
			       void *srcs, u32 nsrcs, size_t addr_size,
			       int grec_type)
{
	struct net_bridge *br = pg->key.port->br;
	bool changed = false;

	switch (pg->filter_mode) {
	case MCAST_INCLUDE:
		__grp_src_isexc_incl(pg, h_addr, srcs, nsrcs, addr_size,
				     grec_type);
		br_multicast_star_g_handle_mode(pg, MCAST_EXCLUDE);
		changed = true;
		break;
	case MCAST_EXCLUDE:
		changed = __grp_src_isexc_excl(pg, h_addr, srcs, nsrcs, addr_size,
					       grec_type);
		break;
	}

	pg->filter_mode = MCAST_EXCLUDE;
	mod_timer(&pg->timer, jiffies + br_multicast_gmi(br));

	return changed;
}

/* State          Msg type      New state                Actions
 * INCLUDE (A)    TO_IN (B)     INCLUDE (A+B)            (B)=GMI
 *                                                       Send Q(G,A-B)
 */
static bool __grp_src_toin_incl(struct net_bridge_port_group *pg, void *h_addr,
				void *srcs, u32 nsrcs, size_t addr_size,
				int grec_type)
{
	struct net_bridge *br = pg->key.port->br;
	u32 src_idx, to_send = pg->src_ents;
	struct net_bridge_group_src *ent;
	unsigned long now = jiffies;
	bool changed = false;
	struct br_ip src_ip;

	hlist_for_each_entry(ent, &pg->src_list, node)
		ent->flags |= BR_SGRP_F_SEND;

	memset(&src_ip, 0, sizeof(src_ip));
	src_ip.proto = pg->key.addr.proto;
	for (src_idx = 0; src_idx < nsrcs; src_idx++) {
		memcpy(&src_ip.src, srcs + (src_idx * addr_size), addr_size);
		ent = br_multicast_find_group_src(pg, &src_ip);
		if (ent) {
			ent->flags &= ~BR_SGRP_F_SEND;
			to_send--;
		} else {
			ent = br_multicast_new_group_src(pg, &src_ip);
			if (ent)
				changed = true;
		}
		if (ent)
			__grp_src_mod_timer(ent, now + br_multicast_gmi(br));
	}

	if (br_multicast_eht_handle(pg, h_addr, srcs, nsrcs, addr_size, grec_type))
		changed = true;

	if (to_send)
		__grp_src_query_marked_and_rexmit(pg);

	return changed;
}

/* State          Msg type      New state                Actions
 * EXCLUDE (X,Y)  TO_IN (A)     EXCLUDE (X+A,Y-A)        (A)=GMI
 *                                                       Send Q(G,X-A)
 *                                                       Send Q(G)
 */
static bool __grp_src_toin_excl(struct net_bridge_port_group *pg, void *h_addr,
				void *srcs, u32 nsrcs, size_t addr_size,
				int grec_type)
{
	struct net_bridge *br = pg->key.port->br;
	u32 src_idx, to_send = pg->src_ents;
	struct net_bridge_group_src *ent;
	unsigned long now = jiffies;
	bool changed = false;
	struct br_ip src_ip;

	hlist_for_each_entry(ent, &pg->src_list, node)
		if (timer_pending(&ent->timer))
			ent->flags |= BR_SGRP_F_SEND;

	memset(&src_ip, 0, sizeof(src_ip));
	src_ip.proto = pg->key.addr.proto;
	for (src_idx = 0; src_idx < nsrcs; src_idx++) {
		memcpy(&src_ip.src, srcs + (src_idx * addr_size), addr_size);
		ent = br_multicast_find_group_src(pg, &src_ip);
		if (ent) {
			if (timer_pending(&ent->timer)) {
				ent->flags &= ~BR_SGRP_F_SEND;
				to_send--;
			}
		} else {
			ent = br_multicast_new_group_src(pg, &src_ip);
			if (ent)
				changed = true;
		}
		if (ent)
			__grp_src_mod_timer(ent, now + br_multicast_gmi(br));
	}

	if (br_multicast_eht_handle(pg, h_addr, srcs, nsrcs, addr_size, grec_type))
		changed = true;

	if (to_send)
		__grp_src_query_marked_and_rexmit(pg);

	__grp_send_query_and_rexmit(pg);

	return changed;
}

static bool br_multicast_toin(struct net_bridge_port_group *pg, void *h_addr,
			      void *srcs, u32 nsrcs, size_t addr_size,
			      int grec_type)
{
	bool changed = false;

	switch (pg->filter_mode) {
	case MCAST_INCLUDE:
		changed = __grp_src_toin_incl(pg, h_addr, srcs, nsrcs, addr_size,
					      grec_type);
		break;
	case MCAST_EXCLUDE:
		changed = __grp_src_toin_excl(pg, h_addr, srcs, nsrcs, addr_size,
					      grec_type);
		break;
	}

	if (br_multicast_eht_should_del_pg(pg)) {
		pg->flags |= MDB_PG_FLAGS_FAST_LEAVE;
		br_multicast_find_del_pg(pg->key.port->br, pg);
		/* a notification has already been sent and we shouldn't
		 * access pg after the delete so we have to return false
		 */
		changed = false;
	}

	return changed;
}

/* State          Msg type      New state                Actions
 * INCLUDE (A)    TO_EX (B)     EXCLUDE (A*B,B-A)        (B-A)=0
 *                                                       Delete (A-B)
 *                                                       Send Q(G,A*B)
 *                                                       Group Timer=GMI
 */
static void __grp_src_toex_incl(struct net_bridge_port_group *pg, void *h_addr,
				void *srcs, u32 nsrcs, size_t addr_size,
				int grec_type)
{
	struct net_bridge_group_src *ent;
	u32 src_idx, to_send = 0;
	struct br_ip src_ip;

	hlist_for_each_entry(ent, &pg->src_list, node)
		ent->flags = (ent->flags & ~BR_SGRP_F_SEND) | BR_SGRP_F_DELETE;

	memset(&src_ip, 0, sizeof(src_ip));
	src_ip.proto = pg->key.addr.proto;
	for (src_idx = 0; src_idx < nsrcs; src_idx++) {
		memcpy(&src_ip.src, srcs + (src_idx * addr_size), addr_size);
		ent = br_multicast_find_group_src(pg, &src_ip);
		if (ent) {
			ent->flags = (ent->flags & ~BR_SGRP_F_DELETE) |
				     BR_SGRP_F_SEND;
			to_send++;
		} else {
			ent = br_multicast_new_group_src(pg, &src_ip);
		}
		if (ent)
			br_multicast_fwd_src_handle(ent);
	}

	br_multicast_eht_handle(pg, h_addr, srcs, nsrcs, addr_size, grec_type);

	__grp_src_delete_marked(pg);
	if (to_send)
		__grp_src_query_marked_and_rexmit(pg);
}

/* State          Msg type      New state                Actions
 * EXCLUDE (X,Y)  TO_EX (A)     EXCLUDE (A-Y,Y*A)        (A-X-Y)=Group Timer
 *                                                       Delete (X-A)
 *                                                       Delete (Y-A)
 *                                                       Send Q(G,A-Y)
 *                                                       Group Timer=GMI
 */
static bool __grp_src_toex_excl(struct net_bridge_port_group *pg, void *h_addr,
				void *srcs, u32 nsrcs, size_t addr_size,
				int grec_type)
{
	struct net_bridge_group_src *ent;
	u32 src_idx, to_send = 0;
	bool changed = false;
	struct br_ip src_ip;

	hlist_for_each_entry(ent, &pg->src_list, node)
		ent->flags = (ent->flags & ~BR_SGRP_F_SEND) | BR_SGRP_F_DELETE;

	memset(&src_ip, 0, sizeof(src_ip));
	src_ip.proto = pg->key.addr.proto;
	for (src_idx = 0; src_idx < nsrcs; src_idx++) {
		memcpy(&src_ip.src, srcs + (src_idx * addr_size), addr_size);
		ent = br_multicast_find_group_src(pg, &src_ip);
		if (ent) {
			ent->flags &= ~BR_SGRP_F_DELETE;
		} else {
			ent = br_multicast_new_group_src(pg, &src_ip);
			if (ent) {
				__grp_src_mod_timer(ent, pg->timer.expires);
				changed = true;
			}
		}
		if (ent && timer_pending(&ent->timer)) {
			ent->flags |= BR_SGRP_F_SEND;
			to_send++;
		}
	}

	if (br_multicast_eht_handle(pg, h_addr, srcs, nsrcs, addr_size, grec_type))
		changed = true;

	if (__grp_src_delete_marked(pg))
		changed = true;
	if (to_send)
		__grp_src_query_marked_and_rexmit(pg);

	return changed;
}

static bool br_multicast_toex(struct net_bridge_port_group *pg, void *h_addr,
			      void *srcs, u32 nsrcs, size_t addr_size,
			      int grec_type)
{
	struct net_bridge *br = pg->key.port->br;
	bool changed = false;

	switch (pg->filter_mode) {
	case MCAST_INCLUDE:
		__grp_src_toex_incl(pg, h_addr, srcs, nsrcs, addr_size,
				    grec_type);
		br_multicast_star_g_handle_mode(pg, MCAST_EXCLUDE);
		changed = true;
		break;
	case MCAST_EXCLUDE:
		changed = __grp_src_toex_excl(pg, h_addr, srcs, nsrcs, addr_size,
					      grec_type);
		break;
	}

	pg->filter_mode = MCAST_EXCLUDE;
	mod_timer(&pg->timer, jiffies + br_multicast_gmi(br));

	return changed;
}

/* State          Msg type      New state                Actions
 * INCLUDE (A)    BLOCK (B)     INCLUDE (A)              Send Q(G,A*B)
 */
static bool __grp_src_block_incl(struct net_bridge_port_group *pg, void *h_addr,
				 void *srcs, u32 nsrcs, size_t addr_size, int grec_type)
{
	struct net_bridge_group_src *ent;
	u32 src_idx, to_send = 0;
	bool changed = false;
	struct br_ip src_ip;

	hlist_for_each_entry(ent, &pg->src_list, node)
		ent->flags &= ~BR_SGRP_F_SEND;

	memset(&src_ip, 0, sizeof(src_ip));
	src_ip.proto = pg->key.addr.proto;
	for (src_idx = 0; src_idx < nsrcs; src_idx++) {
		memcpy(&src_ip.src, srcs + (src_idx * addr_size), addr_size);
		ent = br_multicast_find_group_src(pg, &src_ip);
		if (ent) {
			ent->flags |= BR_SGRP_F_SEND;
			to_send++;
		}
	}

	if (br_multicast_eht_handle(pg, h_addr, srcs, nsrcs, addr_size, grec_type))
		changed = true;

	if (to_send)
		__grp_src_query_marked_and_rexmit(pg);

	return changed;
}

/* State          Msg type      New state                Actions
 * EXCLUDE (X,Y)  BLOCK (A)     EXCLUDE (X+(A-Y),Y)      (A-X-Y)=Group Timer
 *                                                       Send Q(G,A-Y)
 */
static bool __grp_src_block_excl(struct net_bridge_port_group *pg, void *h_addr,
				 void *srcs, u32 nsrcs, size_t addr_size, int grec_type)
{
	struct net_bridge_group_src *ent;
	u32 src_idx, to_send = 0;
	bool changed = false;
	struct br_ip src_ip;

	hlist_for_each_entry(ent, &pg->src_list, node)
		ent->flags &= ~BR_SGRP_F_SEND;

	memset(&src_ip, 0, sizeof(src_ip));
	src_ip.proto = pg->key.addr.proto;
	for (src_idx = 0; src_idx < nsrcs; src_idx++) {
		memcpy(&src_ip.src, srcs + (src_idx * addr_size), addr_size);
		ent = br_multicast_find_group_src(pg, &src_ip);
		if (!ent) {
			ent = br_multicast_new_group_src(pg, &src_ip);
			if (ent) {
				__grp_src_mod_timer(ent, pg->timer.expires);
				changed = true;
			}
		}
		if (ent && timer_pending(&ent->timer)) {
			ent->flags |= BR_SGRP_F_SEND;
			to_send++;
		}
	}

	if (br_multicast_eht_handle(pg, h_addr, srcs, nsrcs, addr_size, grec_type))
		changed = true;

	if (to_send)
		__grp_src_query_marked_and_rexmit(pg);

	return changed;
}

static bool br_multicast_block(struct net_bridge_port_group *pg, void *h_addr,
			       void *srcs, u32 nsrcs, size_t addr_size, int grec_type)
{
	bool changed = false;

	switch (pg->filter_mode) {
	case MCAST_INCLUDE:
		changed = __grp_src_block_incl(pg, h_addr, srcs, nsrcs, addr_size,
					       grec_type);
		break;
	case MCAST_EXCLUDE:
		changed = __grp_src_block_excl(pg, h_addr, srcs, nsrcs, addr_size,
					       grec_type);
		break;
	}

	if ((pg->filter_mode == MCAST_INCLUDE && hlist_empty(&pg->src_list)) ||
	    br_multicast_eht_should_del_pg(pg)) {
		if (br_multicast_eht_should_del_pg(pg))
			pg->flags |= MDB_PG_FLAGS_FAST_LEAVE;
		br_multicast_find_del_pg(pg->key.port->br, pg);
		/* a notification has already been sent and we shouldn't
		 * access pg after the delete so we have to return false
		 */
		changed = false;
	}

	return changed;
}

static struct net_bridge_port_group *
br_multicast_find_port(struct net_bridge_mdb_entry *mp,
		       struct net_bridge_port *p,
		       const unsigned char *src)
{
	struct net_bridge *br __maybe_unused = mp->br;
	struct net_bridge_port_group *pg;

	for (pg = mlock_dereference(mp->ports, br);
	     pg;
	     pg = mlock_dereference(pg->next, br))
		if (br_port_group_equal(pg, p, src))
			return pg;

	return NULL;
}

static int br_ip4_multicast_igmp3_report(struct net_bridge *br,
					 struct net_bridge_port *port,
					 struct sk_buff *skb,
					 u16 vid)
{
	bool igmpv2 = br->multicast_igmp_version == 2;
	struct net_bridge_mdb_entry *mdst;
	struct net_bridge_port_group *pg;
	const unsigned char *src;
	struct igmpv3_report *ih;
	struct igmpv3_grec *grec;
	int i, len, num, type;
	__be32 group, *h_addr;
	bool changed = false;
	int err = 0;
	u16 nsrcs;

	ih = igmpv3_report_hdr(skb);
	num = ntohs(ih->ngrec);
	len = skb_transport_offset(skb) + sizeof(*ih);

	for (i = 0; i < num; i++) {
		len += sizeof(*grec);
		if (!ip_mc_may_pull(skb, len))
			return -EINVAL;

		grec = (void *)(skb->data + len - sizeof(*grec));
		group = grec->grec_mca;
		type = grec->grec_type;
		nsrcs = ntohs(grec->grec_nsrcs);

		len += nsrcs * 4;
		if (!ip_mc_may_pull(skb, len))
			return -EINVAL;

		switch (type) {
		case IGMPV3_MODE_IS_INCLUDE:
		case IGMPV3_MODE_IS_EXCLUDE:
		case IGMPV3_CHANGE_TO_INCLUDE:
		case IGMPV3_CHANGE_TO_EXCLUDE:
		case IGMPV3_ALLOW_NEW_SOURCES:
		case IGMPV3_BLOCK_OLD_SOURCES:
			break;

		default:
			continue;
		}

		src = eth_hdr(skb)->h_source;
		if (nsrcs == 0 &&
		    (type == IGMPV3_CHANGE_TO_INCLUDE ||
		     type == IGMPV3_MODE_IS_INCLUDE)) {
			if (!port || igmpv2) {
				br_ip4_multicast_leave_group(br, port, group, vid, src);
				continue;
			}
		} else {
			err = br_ip4_multicast_add_group(br, port, group, vid,
							 src, igmpv2);
			if (err)
				break;
		}

		if (!port || igmpv2)
			continue;

		spin_lock_bh(&br->multicast_lock);
		mdst = br_mdb_ip4_get(br, group, vid);
		if (!mdst)
			goto unlock_continue;
		pg = br_multicast_find_port(mdst, port, src);
		if (!pg || (pg->flags & MDB_PG_FLAGS_PERMANENT))
			goto unlock_continue;
		/* reload grec and host addr */
		grec = (void *)(skb->data + len - sizeof(*grec) - (nsrcs * 4));
		h_addr = &ip_hdr(skb)->saddr;
		switch (type) {
		case IGMPV3_ALLOW_NEW_SOURCES:
			changed = br_multicast_isinc_allow(pg, h_addr, grec->grec_src,
							   nsrcs, sizeof(__be32), type);
			break;
		case IGMPV3_MODE_IS_INCLUDE:
			changed = br_multicast_isinc_allow(pg, h_addr, grec->grec_src,
							   nsrcs, sizeof(__be32), type);
			break;
		case IGMPV3_MODE_IS_EXCLUDE:
			changed = br_multicast_isexc(pg, h_addr, grec->grec_src,
						     nsrcs, sizeof(__be32), type);
			break;
		case IGMPV3_CHANGE_TO_INCLUDE:
			changed = br_multicast_toin(pg, h_addr, grec->grec_src,
						    nsrcs, sizeof(__be32), type);
			break;
		case IGMPV3_CHANGE_TO_EXCLUDE:
			changed = br_multicast_toex(pg, h_addr, grec->grec_src,
						    nsrcs, sizeof(__be32), type);
			break;
		case IGMPV3_BLOCK_OLD_SOURCES:
			changed = br_multicast_block(pg, h_addr, grec->grec_src,
						     nsrcs, sizeof(__be32), type);
			break;
		}
		if (changed)
			br_mdb_notify(br->dev, mdst, pg, RTM_NEWMDB);
unlock_continue:
		spin_unlock_bh(&br->multicast_lock);
	}

	return err;
}

#if IS_ENABLED(CONFIG_IPV6)
static int br_ip6_multicast_mld2_report(struct net_bridge *br,
					struct net_bridge_port *port,
					struct sk_buff *skb,
					u16 vid)
{
	bool mldv1 = br->multicast_mld_version == 1;
	struct net_bridge_mdb_entry *mdst;
	struct net_bridge_port_group *pg;
	unsigned int nsrcs_offset;
	const unsigned char *src;
	struct icmp6hdr *icmp6h;
	struct in6_addr *h_addr;
	struct mld2_grec *grec;
	unsigned int grec_len;
	bool changed = false;
	int i, len, num;
	int err = 0;

	if (!ipv6_mc_may_pull(skb, sizeof(*icmp6h)))
		return -EINVAL;

	icmp6h = icmp6_hdr(skb);
	num = ntohs(icmp6h->icmp6_dataun.un_data16[1]);
	len = skb_transport_offset(skb) + sizeof(*icmp6h);

	for (i = 0; i < num; i++) {
		__be16 *_nsrcs, __nsrcs;
		u16 nsrcs;

		nsrcs_offset = len + offsetof(struct mld2_grec, grec_nsrcs);

		if (skb_transport_offset(skb) + ipv6_transport_len(skb) <
		    nsrcs_offset + sizeof(__nsrcs))
			return -EINVAL;

		_nsrcs = skb_header_pointer(skb, nsrcs_offset,
					    sizeof(__nsrcs), &__nsrcs);
		if (!_nsrcs)
			return -EINVAL;

		nsrcs = ntohs(*_nsrcs);
		grec_len = struct_size(grec, grec_src, nsrcs);

		if (!ipv6_mc_may_pull(skb, len + grec_len))
			return -EINVAL;

		grec = (struct mld2_grec *)(skb->data + len);
		len += grec_len;

		switch (grec->grec_type) {
		case MLD2_MODE_IS_INCLUDE:
		case MLD2_MODE_IS_EXCLUDE:
		case MLD2_CHANGE_TO_INCLUDE:
		case MLD2_CHANGE_TO_EXCLUDE:
		case MLD2_ALLOW_NEW_SOURCES:
		case MLD2_BLOCK_OLD_SOURCES:
			break;

		default:
			continue;
		}

		src = eth_hdr(skb)->h_source;
		if ((grec->grec_type == MLD2_CHANGE_TO_INCLUDE ||
		     grec->grec_type == MLD2_MODE_IS_INCLUDE) &&
		    nsrcs == 0) {
			if (!port || mldv1) {
				br_ip6_multicast_leave_group(br, port,
							     &grec->grec_mca,
							     vid, src);
				continue;
			}
		} else {
			err = br_ip6_multicast_add_group(br, port,
							 &grec->grec_mca, vid,
							 src, mldv1);
			if (err)
				break;
		}

		if (!port || mldv1)
			continue;

		spin_lock_bh(&br->multicast_lock);
		mdst = br_mdb_ip6_get(br, &grec->grec_mca, vid);
		if (!mdst)
			goto unlock_continue;
		pg = br_multicast_find_port(mdst, port, src);
		if (!pg || (pg->flags & MDB_PG_FLAGS_PERMANENT))
			goto unlock_continue;
		h_addr = &ipv6_hdr(skb)->saddr;
		switch (grec->grec_type) {
		case MLD2_ALLOW_NEW_SOURCES:
			changed = br_multicast_isinc_allow(pg, h_addr,
							   grec->grec_src, nsrcs,
							   sizeof(struct in6_addr),
							   grec->grec_type);
			break;
		case MLD2_MODE_IS_INCLUDE:
			changed = br_multicast_isinc_allow(pg, h_addr,
							   grec->grec_src, nsrcs,
							   sizeof(struct in6_addr),
							   grec->grec_type);
			break;
		case MLD2_MODE_IS_EXCLUDE:
			changed = br_multicast_isexc(pg, h_addr,
						     grec->grec_src, nsrcs,
						     sizeof(struct in6_addr),
						     grec->grec_type);
			break;
		case MLD2_CHANGE_TO_INCLUDE:
			changed = br_multicast_toin(pg, h_addr,
						    grec->grec_src, nsrcs,
						    sizeof(struct in6_addr),
						    grec->grec_type);
			break;
		case MLD2_CHANGE_TO_EXCLUDE:
			changed = br_multicast_toex(pg, h_addr,
						    grec->grec_src, nsrcs,
						    sizeof(struct in6_addr),
						    grec->grec_type);
			break;
		case MLD2_BLOCK_OLD_SOURCES:
			changed = br_multicast_block(pg, h_addr,
						     grec->grec_src, nsrcs,
						     sizeof(struct in6_addr),
						     grec->grec_type);
			break;
		}
		if (changed)
			br_mdb_notify(br->dev, mdst, pg, RTM_NEWMDB);
unlock_continue:
		spin_unlock_bh(&br->multicast_lock);
	}

	return err;
}
#endif

static bool br_ip4_multicast_select_querier(struct net_bridge *br,
					    struct net_bridge_port *port,
					    __be32 saddr)
{
	if (!timer_pending(&br->ip4_own_query.timer) &&
	    !timer_pending(&br->ip4_other_query.timer))
		goto update;

	if (!br->ip4_querier.addr.src.ip4)
		goto update;

	if (ntohl(saddr) <= ntohl(br->ip4_querier.addr.src.ip4))
		goto update;

	return false;

update:
	br->ip4_querier.addr.src.ip4 = saddr;

	/* update protected by general multicast_lock by caller */
	rcu_assign_pointer(br->ip4_querier.port, port);

	return true;
}

#if IS_ENABLED(CONFIG_IPV6)
static bool br_ip6_multicast_select_querier(struct net_bridge *br,
					    struct net_bridge_port *port,
					    struct in6_addr *saddr)
{
	if (!timer_pending(&br->ip6_own_query.timer) &&
	    !timer_pending(&br->ip6_other_query.timer))
		goto update;

	if (ipv6_addr_cmp(saddr, &br->ip6_querier.addr.src.ip6) <= 0)
		goto update;

	return false;

update:
	br->ip6_querier.addr.src.ip6 = *saddr;

	/* update protected by general multicast_lock by caller */
	rcu_assign_pointer(br->ip6_querier.port, port);

	return true;
}
#endif

static bool br_multicast_select_querier(struct net_bridge *br,
					struct net_bridge_port *port,
					struct br_ip *saddr)
{
	switch (saddr->proto) {
	case htons(ETH_P_IP):
		return br_ip4_multicast_select_querier(br, port, saddr->src.ip4);
#if IS_ENABLED(CONFIG_IPV6)
	case htons(ETH_P_IPV6):
		return br_ip6_multicast_select_querier(br, port, &saddr->src.ip6);
#endif
	}

	return false;
}

static void
br_multicast_update_query_timer(struct net_bridge *br,
				struct bridge_mcast_other_query *query,
				unsigned long max_delay)
{
	if (!timer_pending(&query->timer))
		query->delay_time = jiffies + max_delay;

	mod_timer(&query->timer, jiffies + br->multicast_querier_interval);
}

static void br_port_mc_router_state_change(struct net_bridge_port *p,
					   bool is_mc_router)
{
	struct switchdev_attr attr = {
		.orig_dev = p->dev,
		.id = SWITCHDEV_ATTR_ID_PORT_MROUTER,
		.flags = SWITCHDEV_F_DEFER,
		.u.mrouter = is_mc_router,
	};

	switchdev_port_attr_set(p->dev, &attr, NULL);
}

/*
 * Add port to router_list
 *  list is maintained ordered by pointer value
 *  and locked by br->multicast_lock and RCU
 */
static void br_multicast_add_router(struct net_bridge *br,
				    struct net_bridge_port *port)
{
	struct net_bridge_port *p;
	struct hlist_node *slot = NULL;

	if (!hlist_unhashed(&port->rlist))
		return;

	hlist_for_each_entry(p, &br->router_list, rlist) {
		if ((unsigned long) port >= (unsigned long) p)
			break;
		slot = &p->rlist;
	}

	if (slot)
		hlist_add_behind_rcu(&port->rlist, slot);
	else
		hlist_add_head_rcu(&port->rlist, &br->router_list);
	br_rtr_notify(br->dev, port, RTM_NEWMDB);
	br_port_mc_router_state_change(port, true);
}

static void br_multicast_mark_router(struct net_bridge *br,
				     struct net_bridge_port *port)
{
	unsigned long now = jiffies;

	if (!port) {
		if (br->multicast_router == MDB_RTR_TYPE_TEMP_QUERY) {
			if (!timer_pending(&br->multicast_router_timer))
				br_mc_router_state_change(br, true);
			mod_timer(&br->multicast_router_timer,
				  now + br->multicast_querier_interval);
		}
		return;
	}

	if (port->multicast_router == MDB_RTR_TYPE_DISABLED ||
	    port->multicast_router == MDB_RTR_TYPE_PERM)
		return;

	br_multicast_add_router(br, port);

	mod_timer(&port->multicast_router_timer,
		  now + br->multicast_querier_interval);
}

static void br_multicast_query_received(struct net_bridge *br,
					struct net_bridge_port *port,
					struct bridge_mcast_other_query *query,
					struct br_ip *saddr,
					unsigned long max_delay)
{
	if (!br_multicast_select_querier(br, port, saddr))
		return;

	br_multicast_update_query_timer(br, query, max_delay);
	br_multicast_mark_router(br, port);
}

static void br_ip4_multicast_query(struct net_bridge *br,
				   struct net_bridge_port *port,
				   struct sk_buff *skb,
				   u16 vid)
{
	unsigned int transport_len = ip_transport_len(skb);
	const struct iphdr *iph = ip_hdr(skb);
	struct igmphdr *ih = igmp_hdr(skb);
	struct net_bridge_mdb_entry *mp;
	struct igmpv3_query *ih3;
	struct net_bridge_port_group *p;
	struct net_bridge_port_group __rcu **pp;
	struct br_ip saddr;
	unsigned long max_delay;
	unsigned long now = jiffies;
	__be32 group;

	spin_lock(&br->multicast_lock);
	if (!netif_running(br->dev) ||
	    (port && port->state == BR_STATE_DISABLED))
		goto out;

	group = ih->group;

	if (transport_len == sizeof(*ih)) {
		max_delay = ih->code * (HZ / IGMP_TIMER_SCALE);

		if (!max_delay) {
			max_delay = 10 * HZ;
			group = 0;
		}
	} else if (transport_len >= sizeof(*ih3)) {
		ih3 = igmpv3_query_hdr(skb);
		if (ih3->nsrcs ||
		    (br->multicast_igmp_version == 3 && group && ih3->suppress))
			goto out;

		max_delay = ih3->code ?
			    IGMPV3_MRC(ih3->code) * (HZ / IGMP_TIMER_SCALE) : 1;
	} else {
		goto out;
	}

	if (!group) {
		saddr.proto = htons(ETH_P_IP);
		saddr.src.ip4 = iph->saddr;

		br_multicast_query_received(br, port, &br->ip4_other_query,
					    &saddr, max_delay);
		goto out;
	}

	mp = br_mdb_ip4_get(br, group, vid);
	if (!mp)
		goto out;

	max_delay *= br->multicast_last_member_count;

	if (mp->host_joined &&
	    (timer_pending(&mp->timer) ?
	     time_after(mp->timer.expires, now + max_delay) :
	     try_to_del_timer_sync(&mp->timer) >= 0))
		mod_timer(&mp->timer, now + max_delay);

	for (pp = &mp->ports;
	     (p = mlock_dereference(*pp, br)) != NULL;
	     pp = &p->next) {
		if (timer_pending(&p->timer) ?
		    time_after(p->timer.expires, now + max_delay) :
		    try_to_del_timer_sync(&p->timer) >= 0 &&
		    (br->multicast_igmp_version == 2 ||
		     p->filter_mode == MCAST_EXCLUDE))
			mod_timer(&p->timer, now + max_delay);
	}

out:
	spin_unlock(&br->multicast_lock);
}

#if IS_ENABLED(CONFIG_IPV6)
static int br_ip6_multicast_query(struct net_bridge *br,
				  struct net_bridge_port *port,
				  struct sk_buff *skb,
				  u16 vid)
{
	unsigned int transport_len = ipv6_transport_len(skb);
	struct mld_msg *mld;
	struct net_bridge_mdb_entry *mp;
	struct mld2_query *mld2q;
	struct net_bridge_port_group *p;
	struct net_bridge_port_group __rcu **pp;
	struct br_ip saddr;
	unsigned long max_delay;
	unsigned long now = jiffies;
	unsigned int offset = skb_transport_offset(skb);
	const struct in6_addr *group = NULL;
	bool is_general_query;
	int err = 0;

	spin_lock(&br->multicast_lock);
	if (!netif_running(br->dev) ||
	    (port && port->state == BR_STATE_DISABLED))
		goto out;

	if (transport_len == sizeof(*mld)) {
		if (!pskb_may_pull(skb, offset + sizeof(*mld))) {
			err = -EINVAL;
			goto out;
		}
		mld = (struct mld_msg *) icmp6_hdr(skb);
		max_delay = msecs_to_jiffies(ntohs(mld->mld_maxdelay));
		if (max_delay)
			group = &mld->mld_mca;
	} else {
		if (!pskb_may_pull(skb, offset + sizeof(*mld2q))) {
			err = -EINVAL;
			goto out;
		}
		mld2q = (struct mld2_query *)icmp6_hdr(skb);
		if (!mld2q->mld2q_nsrcs)
			group = &mld2q->mld2q_mca;
		if (br->multicast_mld_version == 2 &&
		    !ipv6_addr_any(&mld2q->mld2q_mca) &&
		    mld2q->mld2q_suppress)
			goto out;

		max_delay = max(msecs_to_jiffies(mldv2_mrc(mld2q)), 1UL);
	}

	is_general_query = group && ipv6_addr_any(group);

	if (is_general_query) {
		saddr.proto = htons(ETH_P_IPV6);
		saddr.src.ip6 = ipv6_hdr(skb)->saddr;

		br_multicast_query_received(br, port, &br->ip6_other_query,
					    &saddr, max_delay);
		goto out;
	} else if (!group) {
		goto out;
	}

	mp = br_mdb_ip6_get(br, group, vid);
	if (!mp)
		goto out;

	max_delay *= br->multicast_last_member_count;
	if (mp->host_joined &&
	    (timer_pending(&mp->timer) ?
	     time_after(mp->timer.expires, now + max_delay) :
	     try_to_del_timer_sync(&mp->timer) >= 0))
		mod_timer(&mp->timer, now + max_delay);

	for (pp = &mp->ports;
	     (p = mlock_dereference(*pp, br)) != NULL;
	     pp = &p->next) {
		if (timer_pending(&p->timer) ?
		    time_after(p->timer.expires, now + max_delay) :
		    try_to_del_timer_sync(&p->timer) >= 0 &&
		    (br->multicast_mld_version == 1 ||
		     p->filter_mode == MCAST_EXCLUDE))
			mod_timer(&p->timer, now + max_delay);
	}

out:
	spin_unlock(&br->multicast_lock);
	return err;
}
#endif

static void
br_multicast_leave_group(struct net_bridge *br,
			 struct net_bridge_port *port,
			 struct br_ip *group,
			 struct bridge_mcast_other_query *other_query,
			 struct bridge_mcast_own_query *own_query,
			 const unsigned char *src)
{
	struct net_bridge_mdb_entry *mp;
	struct net_bridge_port_group *p;
	unsigned long now;
	unsigned long time;

	spin_lock(&br->multicast_lock);
	if (!netif_running(br->dev) ||
	    (port && port->state == BR_STATE_DISABLED))
		goto out;

	mp = br_mdb_ip_get(br, group);
	if (!mp)
		goto out;

	if (port && (port->flags & BR_MULTICAST_FAST_LEAVE)) {
		struct net_bridge_port_group __rcu **pp;

		for (pp = &mp->ports;
		     (p = mlock_dereference(*pp, br)) != NULL;
		     pp = &p->next) {
			if (!br_port_group_equal(p, port, src))
				continue;

			if (p->flags & MDB_PG_FLAGS_PERMANENT)
				break;

			p->flags |= MDB_PG_FLAGS_FAST_LEAVE;
			br_multicast_del_pg(mp, p, pp);
		}
		goto out;
	}

	if (timer_pending(&other_query->timer))
		goto out;

	if (br_opt_get(br, BROPT_MULTICAST_QUERIER)) {
		__br_multicast_send_query(br, port, NULL, NULL, &mp->addr,
					  false, 0, NULL);

		time = jiffies + br->multicast_last_member_count *
				 br->multicast_last_member_interval;

		mod_timer(&own_query->timer, time);

		for (p = mlock_dereference(mp->ports, br);
		     p != NULL;
		     p = mlock_dereference(p->next, br)) {
			if (!br_port_group_equal(p, port, src))
				continue;

			if (!hlist_unhashed(&p->mglist) &&
			    (timer_pending(&p->timer) ?
			     time_after(p->timer.expires, time) :
			     try_to_del_timer_sync(&p->timer) >= 0)) {
				mod_timer(&p->timer, time);
			}

			break;
		}
	}

	now = jiffies;
	time = now + br->multicast_last_member_count *
		     br->multicast_last_member_interval;

	if (!port) {
		if (mp->host_joined &&
		    (timer_pending(&mp->timer) ?
		     time_after(mp->timer.expires, time) :
		     try_to_del_timer_sync(&mp->timer) >= 0)) {
			mod_timer(&mp->timer, time);
		}

		goto out;
	}

	for (p = mlock_dereference(mp->ports, br);
	     p != NULL;
	     p = mlock_dereference(p->next, br)) {
		if (p->key.port != port)
			continue;

		if (!hlist_unhashed(&p->mglist) &&
		    (timer_pending(&p->timer) ?
		     time_after(p->timer.expires, time) :
		     try_to_del_timer_sync(&p->timer) >= 0)) {
			mod_timer(&p->timer, time);
		}

		break;
	}
out:
	spin_unlock(&br->multicast_lock);
}

static void br_ip4_multicast_leave_group(struct net_bridge *br,
					 struct net_bridge_port *port,
					 __be32 group,
					 __u16 vid,
					 const unsigned char *src)
{
	struct br_ip br_group;
	struct bridge_mcast_own_query *own_query;

	if (ipv4_is_local_multicast(group))
		return;

	own_query = port ? &port->ip4_own_query : &br->ip4_own_query;

	memset(&br_group, 0, sizeof(br_group));
	br_group.dst.ip4 = group;
	br_group.proto = htons(ETH_P_IP);
	br_group.vid = vid;

	br_multicast_leave_group(br, port, &br_group, &br->ip4_other_query,
				 own_query, src);
}

#if IS_ENABLED(CONFIG_IPV6)
static void br_ip6_multicast_leave_group(struct net_bridge *br,
					 struct net_bridge_port *port,
					 const struct in6_addr *group,
					 __u16 vid,
					 const unsigned char *src)
{
	struct br_ip br_group;
	struct bridge_mcast_own_query *own_query;

	if (ipv6_addr_is_ll_all_nodes(group))
		return;

	own_query = port ? &port->ip6_own_query : &br->ip6_own_query;

	memset(&br_group, 0, sizeof(br_group));
	br_group.dst.ip6 = *group;
	br_group.proto = htons(ETH_P_IPV6);
	br_group.vid = vid;

	br_multicast_leave_group(br, port, &br_group, &br->ip6_other_query,
				 own_query, src);
}
#endif

static void br_multicast_err_count(const struct net_bridge *br,
				   const struct net_bridge_port *p,
				   __be16 proto)
{
	struct bridge_mcast_stats __percpu *stats;
	struct bridge_mcast_stats *pstats;

	if (!br_opt_get(br, BROPT_MULTICAST_STATS_ENABLED))
		return;

	if (p)
		stats = p->mcast_stats;
	else
		stats = br->mcast_stats;
	if (WARN_ON(!stats))
		return;

	pstats = this_cpu_ptr(stats);

	u64_stats_update_begin(&pstats->syncp);
	switch (proto) {
	case htons(ETH_P_IP):
		pstats->mstats.igmp_parse_errors++;
		break;
#if IS_ENABLED(CONFIG_IPV6)
	case htons(ETH_P_IPV6):
		pstats->mstats.mld_parse_errors++;
		break;
#endif
	}
	u64_stats_update_end(&pstats->syncp);
}

static void br_multicast_pim(struct net_bridge *br,
			     struct net_bridge_port *port,
			     const struct sk_buff *skb)
{
	unsigned int offset = skb_transport_offset(skb);
	struct pimhdr *pimhdr, _pimhdr;

	pimhdr = skb_header_pointer(skb, offset, sizeof(_pimhdr), &_pimhdr);
	if (!pimhdr || pim_hdr_version(pimhdr) != PIM_VERSION ||
	    pim_hdr_type(pimhdr) != PIM_TYPE_HELLO)
		return;

	br_multicast_mark_router(br, port);
}

static int br_ip4_multicast_mrd_rcv(struct net_bridge *br,
				    struct net_bridge_port *port,
				    struct sk_buff *skb)
{
	if (ip_hdr(skb)->protocol != IPPROTO_IGMP ||
	    igmp_hdr(skb)->type != IGMP_MRDISC_ADV)
		return -ENOMSG;

	br_multicast_mark_router(br, port);

	return 0;
}

static int br_multicast_ipv4_rcv(struct net_bridge *br,
				 struct net_bridge_port *port,
				 struct sk_buff *skb,
				 u16 vid)
{
	const unsigned char *src;
	struct igmphdr *ih;
	int err;

	err = ip_mc_check_igmp(skb);

	if (err == -ENOMSG) {
		if (!ipv4_is_local_multicast(ip_hdr(skb)->daddr)) {
			BR_INPUT_SKB_CB(skb)->mrouters_only = 1;
		} else if (pim_ipv4_all_pim_routers(ip_hdr(skb)->daddr)) {
			if (ip_hdr(skb)->protocol == IPPROTO_PIM)
				br_multicast_pim(br, port, skb);
		} else if (ipv4_is_all_snoopers(ip_hdr(skb)->daddr)) {
			br_ip4_multicast_mrd_rcv(br, port, skb);
		}

		return 0;
	} else if (err < 0) {
		br_multicast_err_count(br, port, skb->protocol);
		return err;
	}

	ih = igmp_hdr(skb);
	src = eth_hdr(skb)->h_source;
	BR_INPUT_SKB_CB(skb)->igmp = ih->type;

	switch (ih->type) {
	case IGMP_HOST_MEMBERSHIP_REPORT:
	case IGMPV2_HOST_MEMBERSHIP_REPORT:
		BR_INPUT_SKB_CB(skb)->mrouters_only = 1;
		err = br_ip4_multicast_add_group(br, port, ih->group, vid, src,
						 true);
		break;
	case IGMPV3_HOST_MEMBERSHIP_REPORT:
		err = br_ip4_multicast_igmp3_report(br, port, skb, vid);
		break;
	case IGMP_HOST_MEMBERSHIP_QUERY:
		br_ip4_multicast_query(br, port, skb, vid);
		break;
	case IGMP_HOST_LEAVE_MESSAGE:
		br_ip4_multicast_leave_group(br, port, ih->group, vid, src);
		break;
	}

	br_multicast_count(br, port, skb, BR_INPUT_SKB_CB(skb)->igmp,
			   BR_MCAST_DIR_RX);

	return err;
}

#if IS_ENABLED(CONFIG_IPV6)
static void br_ip6_multicast_mrd_rcv(struct net_bridge *br,
				     struct net_bridge_port *port,
				     struct sk_buff *skb)
{
	if (icmp6_hdr(skb)->icmp6_type != ICMPV6_MRDISC_ADV)
		return;

	br_multicast_mark_router(br, port);
}

static int br_multicast_ipv6_rcv(struct net_bridge *br,
				 struct net_bridge_port *port,
				 struct sk_buff *skb,
				 u16 vid)
{
	const unsigned char *src;
	struct mld_msg *mld;
	int err;

	err = ipv6_mc_check_mld(skb);

	if (err == -ENOMSG || err == -ENODATA) {
		if (!ipv6_addr_is_ll_all_nodes(&ipv6_hdr(skb)->daddr))
			BR_INPUT_SKB_CB(skb)->mrouters_only = 1;
		if (err == -ENODATA &&
		    ipv6_addr_is_all_snoopers(&ipv6_hdr(skb)->daddr))
			br_ip6_multicast_mrd_rcv(br, port, skb);

		return 0;
	} else if (err < 0) {
		br_multicast_err_count(br, port, skb->protocol);
		return err;
	}

	mld = (struct mld_msg *)skb_transport_header(skb);
	BR_INPUT_SKB_CB(skb)->igmp = mld->mld_type;

	switch (mld->mld_type) {
	case ICMPV6_MGM_REPORT:
		src = eth_hdr(skb)->h_source;
		BR_INPUT_SKB_CB(skb)->mrouters_only = 1;
		err = br_ip6_multicast_add_group(br, port, &mld->mld_mca, vid,
						 src, true);
		break;
	case ICMPV6_MLD2_REPORT:
		err = br_ip6_multicast_mld2_report(br, port, skb, vid);
		break;
	case ICMPV6_MGM_QUERY:
		err = br_ip6_multicast_query(br, port, skb, vid);
		break;
	case ICMPV6_MGM_REDUCTION:
		src = eth_hdr(skb)->h_source;
		br_ip6_multicast_leave_group(br, port, &mld->mld_mca, vid, src);
		break;
	}

	br_multicast_count(br, port, skb, BR_INPUT_SKB_CB(skb)->igmp,
			   BR_MCAST_DIR_RX);

	return err;
}
#endif

int br_multicast_rcv(struct net_bridge *br, struct net_bridge_port *port,
		     struct sk_buff *skb, u16 vid)
{
	int ret = 0;

	BR_INPUT_SKB_CB(skb)->igmp = 0;
	BR_INPUT_SKB_CB(skb)->mrouters_only = 0;

	if (!br_opt_get(br, BROPT_MULTICAST_ENABLED))
		return 0;

	switch (skb->protocol) {
	case htons(ETH_P_IP):
		ret = br_multicast_ipv4_rcv(br, port, skb, vid);
		break;
#if IS_ENABLED(CONFIG_IPV6)
	case htons(ETH_P_IPV6):
		ret = br_multicast_ipv6_rcv(br, port, skb, vid);
		break;
#endif
	}

	return ret;
}

static void br_multicast_query_expired(struct net_bridge *br,
				       struct bridge_mcast_own_query *query,
				       struct bridge_mcast_querier *querier)
{
	spin_lock(&br->multicast_lock);
	if (query->startup_sent < br->multicast_startup_query_count)
		query->startup_sent++;

	RCU_INIT_POINTER(querier->port, NULL);
	br_multicast_send_query(br, NULL, query);
	spin_unlock(&br->multicast_lock);
}

static void br_ip4_multicast_query_expired(struct timer_list *t)
{
	struct net_bridge *br = from_timer(br, t, ip4_own_query.timer);

	br_multicast_query_expired(br, &br->ip4_own_query, &br->ip4_querier);
}

#if IS_ENABLED(CONFIG_IPV6)
static void br_ip6_multicast_query_expired(struct timer_list *t)
{
	struct net_bridge *br = from_timer(br, t, ip6_own_query.timer);

	br_multicast_query_expired(br, &br->ip6_own_query, &br->ip6_querier);
}
#endif

static void br_multicast_gc_work(struct work_struct *work)
{
	struct net_bridge *br = container_of(work, struct net_bridge,
					     mcast_gc_work);
	HLIST_HEAD(deleted_head);

	spin_lock_bh(&br->multicast_lock);
	hlist_move_list(&br->mcast_gc_list, &deleted_head);
	spin_unlock_bh(&br->multicast_lock);

	br_multicast_gc(&deleted_head);
}

void br_multicast_init(struct net_bridge *br)
{
	br->hash_max = BR_MULTICAST_DEFAULT_HASH_MAX;

	br->multicast_router = MDB_RTR_TYPE_TEMP_QUERY;
	br->multicast_last_member_count = 2;
	br->multicast_startup_query_count = 2;

	br->multicast_last_member_interval = HZ;
	br->multicast_query_response_interval = 10 * HZ;
	br->multicast_startup_query_interval = 125 * HZ / 4;
	br->multicast_query_interval = 125 * HZ;
	br->multicast_querier_interval = 255 * HZ;
	br->multicast_membership_interval = 260 * HZ;

	br->ip4_other_query.delay_time = 0;
	br->ip4_querier.port = NULL;
	br->multicast_igmp_version = 2;
#if IS_ENABLED(CONFIG_IPV6)
	br->multicast_mld_version = 1;
	br->ip6_other_query.delay_time = 0;
	br->ip6_querier.port = NULL;
#endif
	br_opt_toggle(br, BROPT_MULTICAST_ENABLED, true);
	br_opt_toggle(br, BROPT_HAS_IPV6_ADDR, true);

	spin_lock_init(&br->multicast_lock);
	timer_setup(&br->multicast_router_timer,
		    br_multicast_local_router_expired, 0);
	timer_setup(&br->ip4_other_query.timer,
		    br_ip4_multicast_querier_expired, 0);
	timer_setup(&br->ip4_own_query.timer,
		    br_ip4_multicast_query_expired, 0);
#if IS_ENABLED(CONFIG_IPV6)
	timer_setup(&br->ip6_other_query.timer,
		    br_ip6_multicast_querier_expired, 0);
	timer_setup(&br->ip6_own_query.timer,
		    br_ip6_multicast_query_expired, 0);
#endif
	INIT_HLIST_HEAD(&br->mdb_list);
	INIT_HLIST_HEAD(&br->mcast_gc_list);
	INIT_WORK(&br->mcast_gc_work, br_multicast_gc_work);
}

static void br_ip4_multicast_join_snoopers(struct net_bridge *br)
{
	struct in_device *in_dev = in_dev_get(br->dev);

	if (!in_dev)
		return;

	__ip_mc_inc_group(in_dev, htonl(INADDR_ALLSNOOPERS_GROUP), GFP_ATOMIC);
	in_dev_put(in_dev);
}

#if IS_ENABLED(CONFIG_IPV6)
static void br_ip6_multicast_join_snoopers(struct net_bridge *br)
{
	struct in6_addr addr;

	ipv6_addr_set(&addr, htonl(0xff020000), 0, 0, htonl(0x6a));
	ipv6_dev_mc_inc(br->dev, &addr);
}
#else
static inline void br_ip6_multicast_join_snoopers(struct net_bridge *br)
{
}
#endif

void br_multicast_join_snoopers(struct net_bridge *br)
{
	br_ip4_multicast_join_snoopers(br);
	br_ip6_multicast_join_snoopers(br);
}

static void br_ip4_multicast_leave_snoopers(struct net_bridge *br)
{
	struct in_device *in_dev = in_dev_get(br->dev);

	if (WARN_ON(!in_dev))
		return;

	__ip_mc_dec_group(in_dev, htonl(INADDR_ALLSNOOPERS_GROUP), GFP_ATOMIC);
	in_dev_put(in_dev);
}

#if IS_ENABLED(CONFIG_IPV6)
static void br_ip6_multicast_leave_snoopers(struct net_bridge *br)
{
	struct in6_addr addr;

	ipv6_addr_set(&addr, htonl(0xff020000), 0, 0, htonl(0x6a));
	ipv6_dev_mc_dec(br->dev, &addr);
}
#else
static inline void br_ip6_multicast_leave_snoopers(struct net_bridge *br)
{
}
#endif

void br_multicast_leave_snoopers(struct net_bridge *br)
{
	br_ip4_multicast_leave_snoopers(br);
	br_ip6_multicast_leave_snoopers(br);
}

static void __br_multicast_open(struct net_bridge *br,
				struct bridge_mcast_own_query *query)
{
	query->startup_sent = 0;

	if (!br_opt_get(br, BROPT_MULTICAST_ENABLED))
		return;

	mod_timer(&query->timer, jiffies);
}

void br_multicast_open(struct net_bridge *br)
{
	__br_multicast_open(br, &br->ip4_own_query);
#if IS_ENABLED(CONFIG_IPV6)
	__br_multicast_open(br, &br->ip6_own_query);
#endif
}

void br_multicast_stop(struct net_bridge *br)
{
	del_timer_sync(&br->multicast_router_timer);
	del_timer_sync(&br->ip4_other_query.timer);
	del_timer_sync(&br->ip4_own_query.timer);
#if IS_ENABLED(CONFIG_IPV6)
	del_timer_sync(&br->ip6_other_query.timer);
	del_timer_sync(&br->ip6_own_query.timer);
#endif
}

void br_multicast_dev_del(struct net_bridge *br)
{
	struct net_bridge_mdb_entry *mp;
	HLIST_HEAD(deleted_head);
	struct hlist_node *tmp;

	spin_lock_bh(&br->multicast_lock);
	hlist_for_each_entry_safe(mp, tmp, &br->mdb_list, mdb_node)
		br_multicast_del_mdb_entry(mp);
	hlist_move_list(&br->mcast_gc_list, &deleted_head);
	spin_unlock_bh(&br->multicast_lock);

	br_multicast_gc(&deleted_head);
	cancel_work_sync(&br->mcast_gc_work);

	rcu_barrier();
}

int br_multicast_set_router(struct net_bridge *br, unsigned long val)
{
	int err = -EINVAL;

	spin_lock_bh(&br->multicast_lock);

	switch (val) {
	case MDB_RTR_TYPE_DISABLED:
	case MDB_RTR_TYPE_PERM:
		br_mc_router_state_change(br, val == MDB_RTR_TYPE_PERM);
		del_timer(&br->multicast_router_timer);
		br->multicast_router = val;
		err = 0;
		break;
	case MDB_RTR_TYPE_TEMP_QUERY:
		if (br->multicast_router != MDB_RTR_TYPE_TEMP_QUERY)
			br_mc_router_state_change(br, false);
		br->multicast_router = val;
		err = 0;
		break;
	}

	spin_unlock_bh(&br->multicast_lock);

	return err;
}

static void __del_port_router(struct net_bridge_port *p)
{
	if (hlist_unhashed(&p->rlist))
		return;
	hlist_del_init_rcu(&p->rlist);
	br_rtr_notify(p->br->dev, p, RTM_DELMDB);
	br_port_mc_router_state_change(p, false);

	/* don't allow timer refresh */
	if (p->multicast_router == MDB_RTR_TYPE_TEMP)
		p->multicast_router = MDB_RTR_TYPE_TEMP_QUERY;
}

int br_multicast_set_port_router(struct net_bridge_port *p, unsigned long val)
{
	struct net_bridge *br = p->br;
	unsigned long now = jiffies;
	int err = -EINVAL;

	spin_lock(&br->multicast_lock);
	if (p->multicast_router == val) {
		/* Refresh the temp router port timer */
		if (p->multicast_router == MDB_RTR_TYPE_TEMP)
			mod_timer(&p->multicast_router_timer,
				  now + br->multicast_querier_interval);
		err = 0;
		goto unlock;
	}
	switch (val) {
	case MDB_RTR_TYPE_DISABLED:
		p->multicast_router = MDB_RTR_TYPE_DISABLED;
		__del_port_router(p);
		del_timer(&p->multicast_router_timer);
		break;
	case MDB_RTR_TYPE_TEMP_QUERY:
		p->multicast_router = MDB_RTR_TYPE_TEMP_QUERY;
		__del_port_router(p);
		break;
	case MDB_RTR_TYPE_PERM:
		p->multicast_router = MDB_RTR_TYPE_PERM;
		del_timer(&p->multicast_router_timer);
		br_multicast_add_router(br, p);
		break;
	case MDB_RTR_TYPE_TEMP:
		p->multicast_router = MDB_RTR_TYPE_TEMP;
		br_multicast_mark_router(br, p);
		break;
	default:
		goto unlock;
	}
	err = 0;
unlock:
	spin_unlock(&br->multicast_lock);

	return err;
}

static void br_multicast_start_querier(struct net_bridge *br,
				       struct bridge_mcast_own_query *query)
{
	struct net_bridge_port *port;

	__br_multicast_open(br, query);

	rcu_read_lock();
	list_for_each_entry_rcu(port, &br->port_list, list) {
		if (port->state == BR_STATE_DISABLED ||
		    port->state == BR_STATE_BLOCKING)
			continue;

		if (query == &br->ip4_own_query)
			br_multicast_enable(&port->ip4_own_query);
#if IS_ENABLED(CONFIG_IPV6)
		else
			br_multicast_enable(&port->ip6_own_query);
#endif
	}
	rcu_read_unlock();
}

int br_multicast_toggle(struct net_bridge *br, unsigned long val,
			struct netlink_ext_ack *extack)
{
	struct net_bridge_port *port;
	bool change_snoopers = false;
	int err = 0;

	spin_lock_bh(&br->multicast_lock);
	if (!!br_opt_get(br, BROPT_MULTICAST_ENABLED) == !!val)
		goto unlock;

	err = br_mc_disabled_update(br->dev, val, extack);
	if (err == -EOPNOTSUPP)
		err = 0;
	if (err)
		goto unlock;

	br_opt_toggle(br, BROPT_MULTICAST_ENABLED, !!val);
	if (!br_opt_get(br, BROPT_MULTICAST_ENABLED)) {
		change_snoopers = true;
		goto unlock;
	}

	if (!netif_running(br->dev))
		goto unlock;

	br_multicast_open(br);
	list_for_each_entry(port, &br->port_list, list)
		__br_multicast_enable_port(port);

	change_snoopers = true;

unlock:
	spin_unlock_bh(&br->multicast_lock);

	/* br_multicast_join_snoopers has the potential to cause
	 * an MLD Report/Leave to be delivered to br_multicast_rcv,
	 * which would in turn call br_multicast_add_group, which would
	 * attempt to acquire multicast_lock. This function should be
	 * called after the lock has been released to avoid deadlocks on
	 * multicast_lock.
	 *
	 * br_multicast_leave_snoopers does not have the problem since
	 * br_multicast_rcv first checks BROPT_MULTICAST_ENABLED, and
	 * returns without calling br_multicast_ipv4/6_rcv if it's not
	 * enabled. Moved both functions out just for symmetry.
	 */
	if (change_snoopers) {
		if (br_opt_get(br, BROPT_MULTICAST_ENABLED))
			br_multicast_join_snoopers(br);
		else
			br_multicast_leave_snoopers(br);
	}

	return err;
}

bool br_multicast_enabled(const struct net_device *dev)
{
	struct net_bridge *br = netdev_priv(dev);

	return !!br_opt_get(br, BROPT_MULTICAST_ENABLED);
}
EXPORT_SYMBOL_GPL(br_multicast_enabled);

bool br_multicast_router(const struct net_device *dev)
{
	struct net_bridge *br = netdev_priv(dev);
	bool is_router;

	spin_lock_bh(&br->multicast_lock);
	is_router = br_multicast_is_router(br);
	spin_unlock_bh(&br->multicast_lock);
	return is_router;
}
EXPORT_SYMBOL_GPL(br_multicast_router);

int br_multicast_set_querier(struct net_bridge *br, unsigned long val)
{
	unsigned long max_delay;

	val = !!val;

	spin_lock_bh(&br->multicast_lock);
	if (br_opt_get(br, BROPT_MULTICAST_QUERIER) == val)
		goto unlock;

	br_opt_toggle(br, BROPT_MULTICAST_QUERIER, !!val);
	if (!val)
		goto unlock;

	max_delay = br->multicast_query_response_interval;

	if (!timer_pending(&br->ip4_other_query.timer))
		br->ip4_other_query.delay_time = jiffies + max_delay;

	br_multicast_start_querier(br, &br->ip4_own_query);

#if IS_ENABLED(CONFIG_IPV6)
	if (!timer_pending(&br->ip6_other_query.timer))
		br->ip6_other_query.delay_time = jiffies + max_delay;

	br_multicast_start_querier(br, &br->ip6_own_query);
#endif

unlock:
	spin_unlock_bh(&br->multicast_lock);

	return 0;
}

int br_multicast_set_igmp_version(struct net_bridge *br, unsigned long val)
{
	/* Currently we support only version 2 and 3 */
	switch (val) {
	case 2:
	case 3:
		break;
	default:
		return -EINVAL;
	}

	spin_lock_bh(&br->multicast_lock);
	br->multicast_igmp_version = val;
	spin_unlock_bh(&br->multicast_lock);

	return 0;
}

#if IS_ENABLED(CONFIG_IPV6)
int br_multicast_set_mld_version(struct net_bridge *br, unsigned long val)
{
	/* Currently we support version 1 and 2 */
	switch (val) {
	case 1:
	case 2:
		break;
	default:
		return -EINVAL;
	}

	spin_lock_bh(&br->multicast_lock);
	br->multicast_mld_version = val;
	spin_unlock_bh(&br->multicast_lock);

	return 0;
}
#endif

/**
 * br_multicast_list_adjacent - Returns snooped multicast addresses
 * @dev:	The bridge port adjacent to which to retrieve addresses
 * @br_ip_list:	The list to store found, snooped multicast IP addresses in
 *
 * Creates a list of IP addresses (struct br_ip_list) sensed by the multicast
 * snooping feature on all bridge ports of dev's bridge device, excluding
 * the addresses from dev itself.
 *
 * Returns the number of items added to br_ip_list.
 *
 * Notes:
 * - br_ip_list needs to be initialized by caller
 * - br_ip_list might contain duplicates in the end
 *   (needs to be taken care of by caller)
 * - br_ip_list needs to be freed by caller
 */
int br_multicast_list_adjacent(struct net_device *dev,
			       struct list_head *br_ip_list)
{
	struct net_bridge *br;
	struct net_bridge_port *port;
	struct net_bridge_port_group *group;
	struct br_ip_list *entry;
	int count = 0;

	rcu_read_lock();
	if (!br_ip_list || !netif_is_bridge_port(dev))
		goto unlock;

	port = br_port_get_rcu(dev);
	if (!port || !port->br)
		goto unlock;

	br = port->br;

	list_for_each_entry_rcu(port, &br->port_list, list) {
		if (!port->dev || port->dev == dev)
			continue;

		hlist_for_each_entry_rcu(group, &port->mglist, mglist) {
			entry = kmalloc(sizeof(*entry), GFP_ATOMIC);
			if (!entry)
				goto unlock;

			entry->addr = group->key.addr;
			list_add(&entry->list, br_ip_list);
			count++;
		}
	}

unlock:
	rcu_read_unlock();
	return count;
}
EXPORT_SYMBOL_GPL(br_multicast_list_adjacent);

/**
 * br_multicast_has_querier_anywhere - Checks for a querier on a bridge
 * @dev: The bridge port providing the bridge on which to check for a querier
 * @proto: The protocol family to check for: IGMP -> ETH_P_IP, MLD -> ETH_P_IPV6
 *
 * Checks whether the given interface has a bridge on top and if so returns
 * true if a valid querier exists anywhere on the bridged link layer.
 * Otherwise returns false.
 */
bool br_multicast_has_querier_anywhere(struct net_device *dev, int proto)
{
	struct net_bridge *br;
	struct net_bridge_port *port;
	struct ethhdr eth;
	bool ret = false;

	rcu_read_lock();
	if (!netif_is_bridge_port(dev))
		goto unlock;

	port = br_port_get_rcu(dev);
	if (!port || !port->br)
		goto unlock;

	br = port->br;

	memset(&eth, 0, sizeof(eth));
	eth.h_proto = htons(proto);

	ret = br_multicast_querier_exists(br, &eth, NULL);

unlock:
	rcu_read_unlock();
	return ret;
}
EXPORT_SYMBOL_GPL(br_multicast_has_querier_anywhere);

/**
 * br_multicast_has_querier_adjacent - Checks for a querier behind a bridge port
 * @dev: The bridge port adjacent to which to check for a querier
 * @proto: The protocol family to check for: IGMP -> ETH_P_IP, MLD -> ETH_P_IPV6
 *
 * Checks whether the given interface has a bridge on top and if so returns
 * true if a selected querier is behind one of the other ports of this
 * bridge. Otherwise returns false.
 */
bool br_multicast_has_querier_adjacent(struct net_device *dev, int proto)
{
	struct net_bridge *br;
	struct net_bridge_port *port;
	bool ret = false;

	rcu_read_lock();
	if (!netif_is_bridge_port(dev))
		goto unlock;

	port = br_port_get_rcu(dev);
	if (!port || !port->br)
		goto unlock;

	br = port->br;

	switch (proto) {
	case ETH_P_IP:
		if (!timer_pending(&br->ip4_other_query.timer) ||
		    rcu_dereference(br->ip4_querier.port) == port)
			goto unlock;
		break;
#if IS_ENABLED(CONFIG_IPV6)
	case ETH_P_IPV6:
		if (!timer_pending(&br->ip6_other_query.timer) ||
		    rcu_dereference(br->ip6_querier.port) == port)
			goto unlock;
		break;
#endif
	default:
		goto unlock;
	}

	ret = true;
unlock:
	rcu_read_unlock();
	return ret;
}
EXPORT_SYMBOL_GPL(br_multicast_has_querier_adjacent);

static void br_mcast_stats_add(struct bridge_mcast_stats __percpu *stats,
			       const struct sk_buff *skb, u8 type, u8 dir)
{
	struct bridge_mcast_stats *pstats = this_cpu_ptr(stats);
	__be16 proto = skb->protocol;
	unsigned int t_len;

	u64_stats_update_begin(&pstats->syncp);
	switch (proto) {
	case htons(ETH_P_IP):
		t_len = ntohs(ip_hdr(skb)->tot_len) - ip_hdrlen(skb);
		switch (type) {
		case IGMP_HOST_MEMBERSHIP_REPORT:
			pstats->mstats.igmp_v1reports[dir]++;
			break;
		case IGMPV2_HOST_MEMBERSHIP_REPORT:
			pstats->mstats.igmp_v2reports[dir]++;
			break;
		case IGMPV3_HOST_MEMBERSHIP_REPORT:
			pstats->mstats.igmp_v3reports[dir]++;
			break;
		case IGMP_HOST_MEMBERSHIP_QUERY:
			if (t_len != sizeof(struct igmphdr)) {
				pstats->mstats.igmp_v3queries[dir]++;
			} else {
				unsigned int offset = skb_transport_offset(skb);
				struct igmphdr *ih, _ihdr;

				ih = skb_header_pointer(skb, offset,
							sizeof(_ihdr), &_ihdr);
				if (!ih)
					break;
				if (!ih->code)
					pstats->mstats.igmp_v1queries[dir]++;
				else
					pstats->mstats.igmp_v2queries[dir]++;
			}
			break;
		case IGMP_HOST_LEAVE_MESSAGE:
			pstats->mstats.igmp_leaves[dir]++;
			break;
		}
		break;
#if IS_ENABLED(CONFIG_IPV6)
	case htons(ETH_P_IPV6):
		t_len = ntohs(ipv6_hdr(skb)->payload_len) +
			sizeof(struct ipv6hdr);
		t_len -= skb_network_header_len(skb);
		switch (type) {
		case ICMPV6_MGM_REPORT:
			pstats->mstats.mld_v1reports[dir]++;
			break;
		case ICMPV6_MLD2_REPORT:
			pstats->mstats.mld_v2reports[dir]++;
			break;
		case ICMPV6_MGM_QUERY:
			if (t_len != sizeof(struct mld_msg))
				pstats->mstats.mld_v2queries[dir]++;
			else
				pstats->mstats.mld_v1queries[dir]++;
			break;
		case ICMPV6_MGM_REDUCTION:
			pstats->mstats.mld_leaves[dir]++;
			break;
		}
		break;
#endif /* CONFIG_IPV6 */
	}
	u64_stats_update_end(&pstats->syncp);
}

void br_multicast_count(struct net_bridge *br, const struct net_bridge_port *p,
			const struct sk_buff *skb, u8 type, u8 dir)
{
	struct bridge_mcast_stats __percpu *stats;

	/* if multicast_disabled is true then igmp type can't be set */
	if (!type || !br_opt_get(br, BROPT_MULTICAST_STATS_ENABLED))
		return;

	if (p)
		stats = p->mcast_stats;
	else
		stats = br->mcast_stats;
	if (WARN_ON(!stats))
		return;

	br_mcast_stats_add(stats, skb, type, dir);
}

int br_multicast_init_stats(struct net_bridge *br)
{
	br->mcast_stats = netdev_alloc_pcpu_stats(struct bridge_mcast_stats);
	if (!br->mcast_stats)
		return -ENOMEM;

	return 0;
}

void br_multicast_uninit_stats(struct net_bridge *br)
{
	free_percpu(br->mcast_stats);
}

/* noinline for https://bugs.llvm.org/show_bug.cgi?id=45802#c9 */
static noinline_for_stack void mcast_stats_add_dir(u64 *dst, u64 *src)
{
	dst[BR_MCAST_DIR_RX] += src[BR_MCAST_DIR_RX];
	dst[BR_MCAST_DIR_TX] += src[BR_MCAST_DIR_TX];
}

void br_multicast_get_stats(const struct net_bridge *br,
			    const struct net_bridge_port *p,
			    struct br_mcast_stats *dest)
{
	struct bridge_mcast_stats __percpu *stats;
	struct br_mcast_stats tdst;
	int i;

	memset(dest, 0, sizeof(*dest));
	if (p)
		stats = p->mcast_stats;
	else
		stats = br->mcast_stats;
	if (WARN_ON(!stats))
		return;

	memset(&tdst, 0, sizeof(tdst));
	for_each_possible_cpu(i) {
		struct bridge_mcast_stats *cpu_stats = per_cpu_ptr(stats, i);
		struct br_mcast_stats temp;
		unsigned int start;

		do {
			start = u64_stats_fetch_begin_irq(&cpu_stats->syncp);
			memcpy(&temp, &cpu_stats->mstats, sizeof(temp));
		} while (u64_stats_fetch_retry_irq(&cpu_stats->syncp, start));

		mcast_stats_add_dir(tdst.igmp_v1queries, temp.igmp_v1queries);
		mcast_stats_add_dir(tdst.igmp_v2queries, temp.igmp_v2queries);
		mcast_stats_add_dir(tdst.igmp_v3queries, temp.igmp_v3queries);
		mcast_stats_add_dir(tdst.igmp_leaves, temp.igmp_leaves);
		mcast_stats_add_dir(tdst.igmp_v1reports, temp.igmp_v1reports);
		mcast_stats_add_dir(tdst.igmp_v2reports, temp.igmp_v2reports);
		mcast_stats_add_dir(tdst.igmp_v3reports, temp.igmp_v3reports);
		tdst.igmp_parse_errors += temp.igmp_parse_errors;

		mcast_stats_add_dir(tdst.mld_v1queries, temp.mld_v1queries);
		mcast_stats_add_dir(tdst.mld_v2queries, temp.mld_v2queries);
		mcast_stats_add_dir(tdst.mld_leaves, temp.mld_leaves);
		mcast_stats_add_dir(tdst.mld_v1reports, temp.mld_v1reports);
		mcast_stats_add_dir(tdst.mld_v2reports, temp.mld_v2reports);
		tdst.mld_parse_errors += temp.mld_parse_errors;
	}
	memcpy(dest, &tdst, sizeof(*dest));
}

int br_mdb_hash_init(struct net_bridge *br)
{
	int err;

	err = rhashtable_init(&br->sg_port_tbl, &br_sg_port_rht_params);
	if (err)
		return err;

	err = rhashtable_init(&br->mdb_hash_tbl, &br_mdb_rht_params);
	if (err) {
		rhashtable_destroy(&br->sg_port_tbl);
		return err;
	}

	return 0;
}

void br_mdb_hash_fini(struct net_bridge *br)
{
	rhashtable_destroy(&br->sg_port_tbl);
	rhashtable_destroy(&br->mdb_hash_tbl);
}
