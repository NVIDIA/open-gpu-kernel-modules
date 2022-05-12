// SPDX-License-Identifier: GPL-2.0-only
/* Copyright (C) 2014 Jozsef Kadlecsik <kadlec@netfilter.org> */

/* Kernel module implementing an IP set type: the hash:mac type */

#include <linux/jhash.h>
#include <linux/module.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/errno.h>
#include <linux/if_ether.h>
#include <net/netlink.h>

#include <linux/netfilter.h>
#include <linux/netfilter/ipset/ip_set.h>
#include <linux/netfilter/ipset/ip_set_hash.h>

#define IPSET_TYPE_REV_MIN	0
#define IPSET_TYPE_REV_MAX	1	/* bucketsize, initval support */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jozsef Kadlecsik <kadlec@netfilter.org>");
IP_SET_MODULE_DESC("hash:mac", IPSET_TYPE_REV_MIN, IPSET_TYPE_REV_MAX);
MODULE_ALIAS("ip_set_hash:mac");

/* Type specific function prefix */
#define HTYPE		hash_mac

/* Member elements */
struct hash_mac4_elem {
	/* Zero valued IP addresses cannot be stored */
	union {
		unsigned char ether[ETH_ALEN];
		__be32 foo[2];
	};
};

/* Common functions */

static bool
hash_mac4_data_equal(const struct hash_mac4_elem *e1,
		     const struct hash_mac4_elem *e2,
		     u32 *multi)
{
	return ether_addr_equal(e1->ether, e2->ether);
}

static bool
hash_mac4_data_list(struct sk_buff *skb, const struct hash_mac4_elem *e)
{
	if (nla_put(skb, IPSET_ATTR_ETHER, ETH_ALEN, e->ether))
		goto nla_put_failure;
	return false;

nla_put_failure:
	return true;
}

static void
hash_mac4_data_next(struct hash_mac4_elem *next,
		    const struct hash_mac4_elem *e)
{
}

#define MTYPE		hash_mac4
#define HOST_MASK	32
#define IP_SET_EMIT_CREATE
#define IP_SET_PROTO_UNDEF
#include "ip_set_hash_gen.h"

static int
hash_mac4_kadt(struct ip_set *set, const struct sk_buff *skb,
	       const struct xt_action_param *par,
	       enum ipset_adt adt, struct ip_set_adt_opt *opt)
{
	ipset_adtfn adtfn = set->variant->adt[adt];
	struct hash_mac4_elem e = { { .foo[0] = 0, .foo[1] = 0 } };
	struct ip_set_ext ext = IP_SET_INIT_KEXT(skb, opt, set);

	if (skb_mac_header(skb) < skb->head ||
	    (skb_mac_header(skb) + ETH_HLEN) > skb->data)
		return -EINVAL;

	if (opt->flags & IPSET_DIM_ONE_SRC)
		ether_addr_copy(e.ether, eth_hdr(skb)->h_source);
	else
		ether_addr_copy(e.ether, eth_hdr(skb)->h_dest);

	if (is_zero_ether_addr(e.ether))
		return -EINVAL;
	return adtfn(set, &e, &ext, &opt->ext, opt->cmdflags);
}

static int
hash_mac4_uadt(struct ip_set *set, struct nlattr *tb[],
	       enum ipset_adt adt, u32 *lineno, u32 flags, bool retried)
{
	ipset_adtfn adtfn = set->variant->adt[adt];
	struct hash_mac4_elem e = { { .foo[0] = 0, .foo[1] = 0 } };
	struct ip_set_ext ext = IP_SET_INIT_UEXT(set);
	int ret;

	if (tb[IPSET_ATTR_LINENO])
		*lineno = nla_get_u32(tb[IPSET_ATTR_LINENO]);

	if (unlikely(!tb[IPSET_ATTR_ETHER] ||
		     nla_len(tb[IPSET_ATTR_ETHER]) != ETH_ALEN))
		return -IPSET_ERR_PROTOCOL;

	ret = ip_set_get_extensions(set, tb, &ext);
	if (ret)
		return ret;
	ether_addr_copy(e.ether, nla_data(tb[IPSET_ATTR_ETHER]));
	if (is_zero_ether_addr(e.ether))
		return -IPSET_ERR_HASH_ELEM;

	return adtfn(set, &e, &ext, &ext, flags);
}

static struct ip_set_type hash_mac_type __read_mostly = {
	.name		= "hash:mac",
	.protocol	= IPSET_PROTOCOL,
	.features	= IPSET_TYPE_MAC,
	.dimension	= IPSET_DIM_ONE,
	.family		= NFPROTO_UNSPEC,
	.revision_min	= IPSET_TYPE_REV_MIN,
	.revision_max	= IPSET_TYPE_REV_MAX,
	.create_flags[IPSET_TYPE_REV_MAX] = IPSET_CREATE_FLAG_BUCKETSIZE,
	.create		= hash_mac_create,
	.create_policy	= {
		[IPSET_ATTR_HASHSIZE]	= { .type = NLA_U32 },
		[IPSET_ATTR_MAXELEM]	= { .type = NLA_U32 },
		[IPSET_ATTR_INITVAL]	= { .type = NLA_U32 },
		[IPSET_ATTR_BUCKETSIZE]	= { .type = NLA_U8 },
		[IPSET_ATTR_RESIZE]	= { .type = NLA_U8  },
		[IPSET_ATTR_TIMEOUT]	= { .type = NLA_U32 },
		[IPSET_ATTR_CADT_FLAGS]	= { .type = NLA_U32 },
	},
	.adt_policy	= {
		[IPSET_ATTR_ETHER]	= { .type = NLA_BINARY,
					    .len  = ETH_ALEN },
		[IPSET_ATTR_TIMEOUT]	= { .type = NLA_U32 },
		[IPSET_ATTR_LINENO]	= { .type = NLA_U32 },
		[IPSET_ATTR_BYTES]	= { .type = NLA_U64 },
		[IPSET_ATTR_PACKETS]	= { .type = NLA_U64 },
		[IPSET_ATTR_COMMENT]	= { .type = NLA_NUL_STRING,
					    .len  = IPSET_MAX_COMMENT_SIZE },
		[IPSET_ATTR_SKBMARK]	= { .type = NLA_U64 },
		[IPSET_ATTR_SKBPRIO]	= { .type = NLA_U32 },
		[IPSET_ATTR_SKBQUEUE]	= { .type = NLA_U16 },
	},
	.me		= THIS_MODULE,
};

static int __init
hash_mac_init(void)
{
	return ip_set_type_register(&hash_mac_type);
}

static void __exit
hash_mac_fini(void)
{
	rcu_barrier();
	ip_set_type_unregister(&hash_mac_type);
}

module_init(hash_mac_init);
module_exit(hash_mac_fini);
