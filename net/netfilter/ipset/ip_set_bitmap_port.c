// SPDX-License-Identifier: GPL-2.0-only
/* Copyright (C) 2003-2013 Jozsef Kadlecsik <kadlec@netfilter.org> */

/* Kernel module implementing an IP set type: the bitmap:port type */

#include <linux/module.h>
#include <linux/ip.h>
#include <linux/skbuff.h>
#include <linux/errno.h>
#include <linux/netlink.h>
#include <linux/jiffies.h>
#include <linux/timer.h>
#include <net/netlink.h>

#include <linux/netfilter/ipset/ip_set.h>
#include <linux/netfilter/ipset/ip_set_bitmap.h>
#include <linux/netfilter/ipset/ip_set_getport.h>

#define IPSET_TYPE_REV_MIN	0
/*				1	   Counter support added */
/*				2	   Comment support added */
#define IPSET_TYPE_REV_MAX	3	/* skbinfo support added */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jozsef Kadlecsik <kadlec@netfilter.org>");
IP_SET_MODULE_DESC("bitmap:port", IPSET_TYPE_REV_MIN, IPSET_TYPE_REV_MAX);
MODULE_ALIAS("ip_set_bitmap:port");

#define MTYPE		bitmap_port

/* Type structure */
struct bitmap_port {
	unsigned long *members;	/* the set members */
	u16 first_port;		/* host byte order, included in range */
	u16 last_port;		/* host byte order, included in range */
	u32 elements;		/* number of max elements in the set */
	size_t memsize;		/* members size */
	struct timer_list gc;	/* garbage collection */
	struct ip_set *set;	/* attached to this ip_set */
	unsigned char extensions[]	/* data extensions */
		__aligned(__alignof__(u64));
};

/* ADT structure for generic function args */
struct bitmap_port_adt_elem {
	u16 id;
};

static u16
port_to_id(const struct bitmap_port *m, u16 port)
{
	return port - m->first_port;
}

/* Common functions */

static int
bitmap_port_do_test(const struct bitmap_port_adt_elem *e,
		    const struct bitmap_port *map, size_t dsize)
{
	return !!test_bit(e->id, map->members);
}

static int
bitmap_port_gc_test(u16 id, const struct bitmap_port *map, size_t dsize)
{
	return !!test_bit(id, map->members);
}

static int
bitmap_port_do_add(const struct bitmap_port_adt_elem *e,
		   struct bitmap_port *map, u32 flags, size_t dsize)
{
	return !!test_bit(e->id, map->members);
}

static int
bitmap_port_do_del(const struct bitmap_port_adt_elem *e,
		   struct bitmap_port *map)
{
	return !test_and_clear_bit(e->id, map->members);
}

static int
bitmap_port_do_list(struct sk_buff *skb, const struct bitmap_port *map, u32 id,
		    size_t dsize)
{
	return nla_put_net16(skb, IPSET_ATTR_PORT,
			     htons(map->first_port + id));
}

static int
bitmap_port_do_head(struct sk_buff *skb, const struct bitmap_port *map)
{
	return nla_put_net16(skb, IPSET_ATTR_PORT, htons(map->first_port)) ||
	       nla_put_net16(skb, IPSET_ATTR_PORT_TO, htons(map->last_port));
}

static bool
ip_set_get_ip_port(const struct sk_buff *skb, u8 pf, bool src, __be16 *port)
{
	bool ret;
	u8 proto;

	switch (pf) {
	case NFPROTO_IPV4:
		ret = ip_set_get_ip4_port(skb, src, port, &proto);
		break;
	case NFPROTO_IPV6:
		ret = ip_set_get_ip6_port(skb, src, port, &proto);
		break;
	default:
		return false;
	}
	if (!ret)
		return ret;
	switch (proto) {
	case IPPROTO_TCP:
	case IPPROTO_UDP:
		return true;
	default:
		return false;
	}
}

static int
bitmap_port_kadt(struct ip_set *set, const struct sk_buff *skb,
		 const struct xt_action_param *par,
		 enum ipset_adt adt, struct ip_set_adt_opt *opt)
{
	struct bitmap_port *map = set->data;
	ipset_adtfn adtfn = set->variant->adt[adt];
	struct bitmap_port_adt_elem e = { .id = 0 };
	struct ip_set_ext ext = IP_SET_INIT_KEXT(skb, opt, set);
	__be16 __port;
	u16 port = 0;

	if (!ip_set_get_ip_port(skb, opt->family,
				opt->flags & IPSET_DIM_ONE_SRC, &__port))
		return -EINVAL;

	port = ntohs(__port);

	if (port < map->first_port || port > map->last_port)
		return -IPSET_ERR_BITMAP_RANGE;

	e.id = port_to_id(map, port);

	return adtfn(set, &e, &ext, &opt->ext, opt->cmdflags);
}

static int
bitmap_port_uadt(struct ip_set *set, struct nlattr *tb[],
		 enum ipset_adt adt, u32 *lineno, u32 flags, bool retried)
{
	struct bitmap_port *map = set->data;
	ipset_adtfn adtfn = set->variant->adt[adt];
	struct bitmap_port_adt_elem e = { .id = 0 };
	struct ip_set_ext ext = IP_SET_INIT_UEXT(set);
	u32 port;	/* wraparound */
	u16 port_to;
	int ret = 0;

	if (tb[IPSET_ATTR_LINENO])
		*lineno = nla_get_u32(tb[IPSET_ATTR_LINENO]);

	if (unlikely(!ip_set_attr_netorder(tb, IPSET_ATTR_PORT) ||
		     !ip_set_optattr_netorder(tb, IPSET_ATTR_PORT_TO)))
		return -IPSET_ERR_PROTOCOL;

	port = ip_set_get_h16(tb[IPSET_ATTR_PORT]);
	if (port < map->first_port || port > map->last_port)
		return -IPSET_ERR_BITMAP_RANGE;
	ret = ip_set_get_extensions(set, tb, &ext);
	if (ret)
		return ret;

	if (adt == IPSET_TEST) {
		e.id = port_to_id(map, port);
		return adtfn(set, &e, &ext, &ext, flags);
	}

	if (tb[IPSET_ATTR_PORT_TO]) {
		port_to = ip_set_get_h16(tb[IPSET_ATTR_PORT_TO]);
		if (port > port_to) {
			swap(port, port_to);
			if (port < map->first_port)
				return -IPSET_ERR_BITMAP_RANGE;
		}
	} else {
		port_to = port;
	}

	if (port_to > map->last_port)
		return -IPSET_ERR_BITMAP_RANGE;

	for (; port <= port_to; port++) {
		e.id = port_to_id(map, port);
		ret = adtfn(set, &e, &ext, &ext, flags);

		if (ret && !ip_set_eexist(ret, flags))
			return ret;

		ret = 0;
	}
	return ret;
}

static bool
bitmap_port_same_set(const struct ip_set *a, const struct ip_set *b)
{
	const struct bitmap_port *x = a->data;
	const struct bitmap_port *y = b->data;

	return x->first_port == y->first_port &&
	       x->last_port == y->last_port &&
	       a->timeout == b->timeout &&
	       a->extensions == b->extensions;
}

/* Plain variant */

struct bitmap_port_elem {
};

#include "ip_set_bitmap_gen.h"

/* Create bitmap:ip type of sets */

static bool
init_map_port(struct ip_set *set, struct bitmap_port *map,
	      u16 first_port, u16 last_port)
{
	map->members = bitmap_zalloc(map->elements, GFP_KERNEL | __GFP_NOWARN);
	if (!map->members)
		return false;
	map->first_port = first_port;
	map->last_port = last_port;
	set->timeout = IPSET_NO_TIMEOUT;

	map->set = set;
	set->data = map;
	set->family = NFPROTO_UNSPEC;

	return true;
}

static int
bitmap_port_create(struct net *net, struct ip_set *set, struct nlattr *tb[],
		   u32 flags)
{
	struct bitmap_port *map;
	u16 first_port, last_port;
	u32 elements;

	if (unlikely(!ip_set_attr_netorder(tb, IPSET_ATTR_PORT) ||
		     !ip_set_attr_netorder(tb, IPSET_ATTR_PORT_TO) ||
		     !ip_set_optattr_netorder(tb, IPSET_ATTR_TIMEOUT) ||
		     !ip_set_optattr_netorder(tb, IPSET_ATTR_CADT_FLAGS)))
		return -IPSET_ERR_PROTOCOL;

	first_port = ip_set_get_h16(tb[IPSET_ATTR_PORT]);
	last_port = ip_set_get_h16(tb[IPSET_ATTR_PORT_TO]);
	if (first_port > last_port)
		swap(first_port, last_port);

	elements = last_port - first_port + 1;
	set->dsize = ip_set_elem_len(set, tb, 0, 0);
	map = ip_set_alloc(sizeof(*map) + elements * set->dsize);
	if (!map)
		return -ENOMEM;

	map->elements = elements;
	map->memsize = BITS_TO_LONGS(elements) * sizeof(unsigned long);
	set->variant = &bitmap_port;
	if (!init_map_port(set, map, first_port, last_port)) {
		ip_set_free(map);
		return -ENOMEM;
	}
	if (tb[IPSET_ATTR_TIMEOUT]) {
		set->timeout = ip_set_timeout_uget(tb[IPSET_ATTR_TIMEOUT]);
		bitmap_port_gc_init(set, bitmap_port_gc);
	}
	return 0;
}

static struct ip_set_type bitmap_port_type = {
	.name		= "bitmap:port",
	.protocol	= IPSET_PROTOCOL,
	.features	= IPSET_TYPE_PORT,
	.dimension	= IPSET_DIM_ONE,
	.family		= NFPROTO_UNSPEC,
	.revision_min	= IPSET_TYPE_REV_MIN,
	.revision_max	= IPSET_TYPE_REV_MAX,
	.create		= bitmap_port_create,
	.create_policy	= {
		[IPSET_ATTR_PORT]	= { .type = NLA_U16 },
		[IPSET_ATTR_PORT_TO]	= { .type = NLA_U16 },
		[IPSET_ATTR_TIMEOUT]	= { .type = NLA_U32 },
		[IPSET_ATTR_CADT_FLAGS]	= { .type = NLA_U32 },
	},
	.adt_policy	= {
		[IPSET_ATTR_PORT]	= { .type = NLA_U16 },
		[IPSET_ATTR_PORT_TO]	= { .type = NLA_U16 },
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
bitmap_port_init(void)
{
	return ip_set_type_register(&bitmap_port_type);
}

static void __exit
bitmap_port_fini(void)
{
	rcu_barrier();
	ip_set_type_unregister(&bitmap_port_type);
}

module_init(bitmap_port_init);
module_exit(bitmap_port_fini);
