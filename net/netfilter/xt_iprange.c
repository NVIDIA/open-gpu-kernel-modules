// SPDX-License-Identifier: GPL-2.0-only
/*
 *	xt_iprange - Netfilter module to match IP address ranges
 *
 *	(C) 2003 Jozsef Kadlecsik <kadlec@netfilter.org>
 *	(C) CC Computer Consultants GmbH, 2008
 */
#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt
#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/netfilter/x_tables.h>
#include <linux/netfilter/xt_iprange.h>

static bool
iprange_mt4(const struct sk_buff *skb, struct xt_action_param *par)
{
	const struct xt_iprange_mtinfo *info = par->matchinfo;
	const struct iphdr *iph = ip_hdr(skb);
	bool m;

	if (info->flags & IPRANGE_SRC) {
		m  = ntohl(iph->saddr) < ntohl(info->src_min.ip);
		m |= ntohl(iph->saddr) > ntohl(info->src_max.ip);
		m ^= !!(info->flags & IPRANGE_SRC_INV);
		if (m) {
			pr_debug("src IP %pI4 NOT in range %s%pI4-%pI4\n",
			         &iph->saddr,
			         (info->flags & IPRANGE_SRC_INV) ? "(INV) " : "",
			         &info->src_min.ip,
			         &info->src_max.ip);
			return false;
		}
	}
	if (info->flags & IPRANGE_DST) {
		m  = ntohl(iph->daddr) < ntohl(info->dst_min.ip);
		m |= ntohl(iph->daddr) > ntohl(info->dst_max.ip);
		m ^= !!(info->flags & IPRANGE_DST_INV);
		if (m) {
			pr_debug("dst IP %pI4 NOT in range %s%pI4-%pI4\n",
			         &iph->daddr,
			         (info->flags & IPRANGE_DST_INV) ? "(INV) " : "",
			         &info->dst_min.ip,
			         &info->dst_max.ip);
			return false;
		}
	}
	return true;
}

static inline int
iprange_ipv6_lt(const struct in6_addr *a, const struct in6_addr *b)
{
	unsigned int i;

	for (i = 0; i < 4; ++i) {
		if (a->s6_addr32[i] != b->s6_addr32[i])
			return ntohl(a->s6_addr32[i]) < ntohl(b->s6_addr32[i]);
	}

	return 0;
}

static bool
iprange_mt6(const struct sk_buff *skb, struct xt_action_param *par)
{
	const struct xt_iprange_mtinfo *info = par->matchinfo;
	const struct ipv6hdr *iph = ipv6_hdr(skb);
	bool m;

	if (info->flags & IPRANGE_SRC) {
		m  = iprange_ipv6_lt(&iph->saddr, &info->src_min.in6);
		m |= iprange_ipv6_lt(&info->src_max.in6, &iph->saddr);
		m ^= !!(info->flags & IPRANGE_SRC_INV);
		if (m) {
			pr_debug("src IP %pI6 NOT in range %s%pI6-%pI6\n",
				 &iph->saddr,
				 (info->flags & IPRANGE_SRC_INV) ? "(INV) " : "",
				 &info->src_min.in6,
				 &info->src_max.in6);
			return false;
		}
	}
	if (info->flags & IPRANGE_DST) {
		m  = iprange_ipv6_lt(&iph->daddr, &info->dst_min.in6);
		m |= iprange_ipv6_lt(&info->dst_max.in6, &iph->daddr);
		m ^= !!(info->flags & IPRANGE_DST_INV);
		if (m) {
			pr_debug("dst IP %pI6 NOT in range %s%pI6-%pI6\n",
				 &iph->daddr,
				 (info->flags & IPRANGE_DST_INV) ? "(INV) " : "",
				 &info->dst_min.in6,
				 &info->dst_max.in6);
			return false;
		}
	}
	return true;
}

static struct xt_match iprange_mt_reg[] __read_mostly = {
	{
		.name      = "iprange",
		.revision  = 1,
		.family    = NFPROTO_IPV4,
		.match     = iprange_mt4,
		.matchsize = sizeof(struct xt_iprange_mtinfo),
		.me        = THIS_MODULE,
	},
	{
		.name      = "iprange",
		.revision  = 1,
		.family    = NFPROTO_IPV6,
		.match     = iprange_mt6,
		.matchsize = sizeof(struct xt_iprange_mtinfo),
		.me        = THIS_MODULE,
	},
};

static int __init iprange_mt_init(void)
{
	return xt_register_matches(iprange_mt_reg, ARRAY_SIZE(iprange_mt_reg));
}

static void __exit iprange_mt_exit(void)
{
	xt_unregister_matches(iprange_mt_reg, ARRAY_SIZE(iprange_mt_reg));
}

module_init(iprange_mt_init);
module_exit(iprange_mt_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jozsef Kadlecsik <kadlec@netfilter.org>");
MODULE_AUTHOR("Jan Engelhardt <jengelh@medozas.de>");
MODULE_DESCRIPTION("Xtables: arbitrary IPv4 range matching");
MODULE_ALIAS("ipt_iprange");
MODULE_ALIAS("ip6t_iprange");
