/* SPDX-License-Identifier: GPL-2.0 */
/*
 * 25-Jul-1998 Major changes to allow for ip chain table
 *
 * 3-Jan-2000 Named tables to allow packet selection for different uses.
 */

/*
 * 	Format of an IP6 firewall descriptor
 *
 * 	src, dst, src_mask, dst_mask are always stored in network byte order.
 * 	flags are stored in host byte order (of course).
 * 	Port numbers are stored in HOST byte order.
 */
#ifndef _IP6_TABLES_H
#define _IP6_TABLES_H

#include <linux/if.h>
#include <linux/in6.h>
#include <linux/init.h>
#include <linux/ipv6.h>
#include <linux/skbuff.h>
#include <uapi/linux/netfilter_ipv6/ip6_tables.h>

extern void *ip6t_alloc_initial_table(const struct xt_table *);

int ip6t_register_table(struct net *net, const struct xt_table *table,
			const struct ip6t_replace *repl,
			const struct nf_hook_ops *ops);
void ip6t_unregister_table_pre_exit(struct net *net, const char *name);
void ip6t_unregister_table_exit(struct net *net, const char *name);
extern unsigned int ip6t_do_table(struct sk_buff *skb,
				  const struct nf_hook_state *state,
				  struct xt_table *table);

#ifdef CONFIG_NETFILTER_XTABLES_COMPAT
#include <net/compat.h>

struct compat_ip6t_entry {
	struct ip6t_ip6 ipv6;
	compat_uint_t nfcache;
	__u16 target_offset;
	__u16 next_offset;
	compat_uint_t comefrom;
	struct compat_xt_counters counters;
	unsigned char elems[];
};

static inline struct xt_entry_target *
compat_ip6t_get_target(struct compat_ip6t_entry *e)
{
	return (void *)e + e->target_offset;
}

#endif /* CONFIG_COMPAT */
#endif /* _IP6_TABLES_H */
