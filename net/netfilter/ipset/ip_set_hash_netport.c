// SPDX-License-Identifier: GPL-2.0-only
/* Copyright (C) 2003-2013 Jozsef Kadlecsik <kadlec@netfilter.org> */

/* Kernel module implementing an IP set type: the hash:net,port type */

#include <linux/jhash.h>
#include <linux/module.h>
#include <linux/ip.h>
#include <linux/skbuff.h>
#include <linux/errno.h>
#include <linux/random.h>
#include <net/ip.h>
#include <net/ipv6.h>
#include <net/netlink.h>

#include <linux/netfilter.h>
#include <linux/netfilter/ipset/pfxlen.h>
#include <linux/netfilter/ipset/ip_set.h>
#include <linux/netfilter/ipset/ip_set_getport.h>
#include <linux/netfilter/ipset/ip_set_hash.h>

#define IPSET_TYPE_REV_MIN	0
/*				1    SCTP and UDPLITE support added */
/*				2    Range as input support for IPv4 added */
/*				3    nomatch flag support added */
/*				4    Counters support added */
/*				5    Comments support added */
/*				6    Forceadd support added */
/*				7    skbinfo support added */
#define IPSET_TYPE_REV_MAX	8 /* bucketsize, initval support added */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jozsef Kadlecsik <kadlec@netfilter.org>");
IP_SET_MODULE_DESC("hash:net,port", IPSET_TYPE_REV_MIN, IPSET_TYPE_REV_MAX);
MODULE_ALIAS("ip_set_hash:net,port");

/* Type specific function prefix */
#define HTYPE		hash_netport
#define IP_SET_HASH_WITH_PROTO
#define IP_SET_HASH_WITH_NETS

/* We squeeze the "nomatch" flag into cidr: we don't support cidr == 0
 * However this way we have to store internally cidr - 1,
 * dancing back and forth.
 */
#define IP_SET_HASH_WITH_NETS_PACKED

/* IPv4 variant */

/* Member elements */
struct hash_netport4_elem {
	__be32 ip;
	__be16 port;
	u8 proto;
	u8 cidr:7;
	u8 nomatch:1;
};

/* Common functions */

static bool
hash_netport4_data_equal(const struct hash_netport4_elem *ip1,
			 const struct hash_netport4_elem *ip2,
			 u32 *multi)
{
	return ip1->ip == ip2->ip &&
	       ip1->port == ip2->port &&
	       ip1->proto == ip2->proto &&
	       ip1->cidr == ip2->cidr;
}

static int
hash_netport4_do_data_match(const struct hash_netport4_elem *elem)
{
	return elem->nomatch ? -ENOTEMPTY : 1;
}

static void
hash_netport4_data_set_flags(struct hash_netport4_elem *elem, u32 flags)
{
	elem->nomatch = !!((flags >> 16) & IPSET_FLAG_NOMATCH);
}

static void
hash_netport4_data_reset_flags(struct hash_netport4_elem *elem, u8 *flags)
{
	swap(*flags, elem->nomatch);
}

static void
hash_netport4_data_netmask(struct hash_netport4_elem *elem, u8 cidr)
{
	elem->ip &= ip_set_netmask(cidr);
	elem->cidr = cidr - 1;
}

static bool
hash_netport4_data_list(struct sk_buff *skb,
			const struct hash_netport4_elem *data)
{
	u32 flags = data->nomatch ? IPSET_FLAG_NOMATCH : 0;

	if (nla_put_ipaddr4(skb, IPSET_ATTR_IP, data->ip) ||
	    nla_put_net16(skb, IPSET_ATTR_PORT, data->port) ||
	    nla_put_u8(skb, IPSET_ATTR_CIDR, data->cidr + 1) ||
	    nla_put_u8(skb, IPSET_ATTR_PROTO, data->proto) ||
	    (flags &&
	     nla_put_net32(skb, IPSET_ATTR_CADT_FLAGS, htonl(flags))))
		goto nla_put_failure;
	return false;

nla_put_failure:
	return true;
}

static void
hash_netport4_data_next(struct hash_netport4_elem *next,
			const struct hash_netport4_elem *d)
{
	next->ip = d->ip;
	next->port = d->port;
}

#define MTYPE		hash_netport4
#define HOST_MASK	32
#include "ip_set_hash_gen.h"

static int
hash_netport4_kadt(struct ip_set *set, const struct sk_buff *skb,
		   const struct xt_action_param *par,
		   enum ipset_adt adt, struct ip_set_adt_opt *opt)
{
	const struct hash_netport4 *h = set->data;
	ipset_adtfn adtfn = set->variant->adt[adt];
	struct hash_netport4_elem e = {
		.cidr = INIT_CIDR(h->nets[0].cidr[0], HOST_MASK),
	};
	struct ip_set_ext ext = IP_SET_INIT_KEXT(skb, opt, set);

	if (adt == IPSET_TEST)
		e.cidr = HOST_MASK - 1;

	if (!ip_set_get_ip4_port(skb, opt->flags & IPSET_DIM_TWO_SRC,
				 &e.port, &e.proto))
		return -EINVAL;

	ip4addrptr(skb, opt->flags & IPSET_DIM_ONE_SRC, &e.ip);
	e.ip &= ip_set_netmask(e.cidr + 1);

	return adtfn(set, &e, &ext, &opt->ext, opt->cmdflags);
}

static int
hash_netport4_uadt(struct ip_set *set, struct nlattr *tb[],
		   enum ipset_adt adt, u32 *lineno, u32 flags, bool retried)
{
	const struct hash_netport4 *h = set->data;
	ipset_adtfn adtfn = set->variant->adt[adt];
	struct hash_netport4_elem e = { .cidr = HOST_MASK - 1 };
	struct ip_set_ext ext = IP_SET_INIT_UEXT(set);
	u32 port, port_to, p = 0, ip = 0, ip_to = 0;
	bool with_ports = false;
	u8 cidr;
	int ret;

	if (tb[IPSET_ATTR_LINENO])
		*lineno = nla_get_u32(tb[IPSET_ATTR_LINENO]);

	if (unlikely(!tb[IPSET_ATTR_IP] ||
		     !ip_set_attr_netorder(tb, IPSET_ATTR_PORT) ||
		     !ip_set_optattr_netorder(tb, IPSET_ATTR_PORT_TO) ||
		     !ip_set_optattr_netorder(tb, IPSET_ATTR_CADT_FLAGS)))
		return -IPSET_ERR_PROTOCOL;

	ret = ip_set_get_hostipaddr4(tb[IPSET_ATTR_IP], &ip);
	if (ret)
		return ret;

	ret = ip_set_get_extensions(set, tb, &ext);
	if (ret)
		return ret;

	if (tb[IPSET_ATTR_CIDR]) {
		cidr = nla_get_u8(tb[IPSET_ATTR_CIDR]);
		if (!cidr || cidr > HOST_MASK)
			return -IPSET_ERR_INVALID_CIDR;
		e.cidr = cidr - 1;
	}

	e.port = nla_get_be16(tb[IPSET_ATTR_PORT]);

	if (tb[IPSET_ATTR_PROTO]) {
		e.proto = nla_get_u8(tb[IPSET_ATTR_PROTO]);
		with_ports = ip_set_proto_with_ports(e.proto);

		if (e.proto == 0)
			return -IPSET_ERR_INVALID_PROTO;
	} else {
		return -IPSET_ERR_MISSING_PROTO;
	}

	if (!(with_ports || e.proto == IPPROTO_ICMP))
		e.port = 0;

	with_ports = with_ports && tb[IPSET_ATTR_PORT_TO];

	if (tb[IPSET_ATTR_CADT_FLAGS]) {
		u32 cadt_flags = ip_set_get_h32(tb[IPSET_ATTR_CADT_FLAGS]);

		if (cadt_flags & IPSET_FLAG_NOMATCH)
			flags |= (IPSET_FLAG_NOMATCH << 16);
	}

	if (adt == IPSET_TEST || !(with_ports || tb[IPSET_ATTR_IP_TO])) {
		e.ip = htonl(ip & ip_set_hostmask(e.cidr + 1));
		ret = adtfn(set, &e, &ext, &ext, flags);
		return ip_set_enomatch(ret, flags, adt, set) ? -ret :
		       ip_set_eexist(ret, flags) ? 0 : ret;
	}

	port = port_to = ntohs(e.port);
	if (tb[IPSET_ATTR_PORT_TO]) {
		port_to = ip_set_get_h16(tb[IPSET_ATTR_PORT_TO]);
		if (port_to < port)
			swap(port, port_to);
	}
	if (tb[IPSET_ATTR_IP_TO]) {
		ret = ip_set_get_hostipaddr4(tb[IPSET_ATTR_IP_TO], &ip_to);
		if (ret)
			return ret;
		if (ip_to < ip)
			swap(ip, ip_to);
		if (ip + UINT_MAX == ip_to)
			return -IPSET_ERR_HASH_RANGE;
	} else {
		ip_set_mask_from_to(ip, ip_to, e.cidr + 1);
	}

	if (retried) {
		ip = ntohl(h->next.ip);
		p = ntohs(h->next.port);
	} else {
		p = port;
	}
	do {
		e.ip = htonl(ip);
		ip = ip_set_range_to_cidr(ip, ip_to, &cidr);
		e.cidr = cidr - 1;
		for (; p <= port_to; p++) {
			e.port = htons(p);
			ret = adtfn(set, &e, &ext, &ext, flags);
			if (ret && !ip_set_eexist(ret, flags))
				return ret;

			ret = 0;
		}
		p = port;
	} while (ip++ < ip_to);
	return ret;
}

/* IPv6 variant */

struct hash_netport6_elem {
	union nf_inet_addr ip;
	__be16 port;
	u8 proto;
	u8 cidr:7;
	u8 nomatch:1;
};

/* Common functions */

static bool
hash_netport6_data_equal(const struct hash_netport6_elem *ip1,
			 const struct hash_netport6_elem *ip2,
			 u32 *multi)
{
	return ipv6_addr_equal(&ip1->ip.in6, &ip2->ip.in6) &&
	       ip1->port == ip2->port &&
	       ip1->proto == ip2->proto &&
	       ip1->cidr == ip2->cidr;
}

static int
hash_netport6_do_data_match(const struct hash_netport6_elem *elem)
{
	return elem->nomatch ? -ENOTEMPTY : 1;
}

static void
hash_netport6_data_set_flags(struct hash_netport6_elem *elem, u32 flags)
{
	elem->nomatch = !!((flags >> 16) & IPSET_FLAG_NOMATCH);
}

static void
hash_netport6_data_reset_flags(struct hash_netport6_elem *elem, u8 *flags)
{
	swap(*flags, elem->nomatch);
}

static void
hash_netport6_data_netmask(struct hash_netport6_elem *elem, u8 cidr)
{
	ip6_netmask(&elem->ip, cidr);
	elem->cidr = cidr - 1;
}

static bool
hash_netport6_data_list(struct sk_buff *skb,
			const struct hash_netport6_elem *data)
{
	u32 flags = data->nomatch ? IPSET_FLAG_NOMATCH : 0;

	if (nla_put_ipaddr6(skb, IPSET_ATTR_IP, &data->ip.in6) ||
	    nla_put_net16(skb, IPSET_ATTR_PORT, data->port) ||
	    nla_put_u8(skb, IPSET_ATTR_CIDR, data->cidr + 1) ||
	    nla_put_u8(skb, IPSET_ATTR_PROTO, data->proto) ||
	    (flags &&
	     nla_put_net32(skb, IPSET_ATTR_CADT_FLAGS, htonl(flags))))
		goto nla_put_failure;
	return false;

nla_put_failure:
	return true;
}

static void
hash_netport6_data_next(struct hash_netport6_elem *next,
			const struct hash_netport6_elem *d)
{
	next->port = d->port;
}

#undef MTYPE
#undef HOST_MASK

#define MTYPE		hash_netport6
#define HOST_MASK	128
#define IP_SET_EMIT_CREATE
#include "ip_set_hash_gen.h"

static int
hash_netport6_kadt(struct ip_set *set, const struct sk_buff *skb,
		   const struct xt_action_param *par,
		   enum ipset_adt adt, struct ip_set_adt_opt *opt)
{
	const struct hash_netport6 *h = set->data;
	ipset_adtfn adtfn = set->variant->adt[adt];
	struct hash_netport6_elem e = {
		.cidr = INIT_CIDR(h->nets[0].cidr[0], HOST_MASK),
	};
	struct ip_set_ext ext = IP_SET_INIT_KEXT(skb, opt, set);

	if (adt == IPSET_TEST)
		e.cidr = HOST_MASK - 1;

	if (!ip_set_get_ip6_port(skb, opt->flags & IPSET_DIM_TWO_SRC,
				 &e.port, &e.proto))
		return -EINVAL;

	ip6addrptr(skb, opt->flags & IPSET_DIM_ONE_SRC, &e.ip.in6);
	ip6_netmask(&e.ip, e.cidr + 1);

	return adtfn(set, &e, &ext, &opt->ext, opt->cmdflags);
}

static int
hash_netport6_uadt(struct ip_set *set, struct nlattr *tb[],
		   enum ipset_adt adt, u32 *lineno, u32 flags, bool retried)
{
	const struct hash_netport6 *h = set->data;
	ipset_adtfn adtfn = set->variant->adt[adt];
	struct hash_netport6_elem e = { .cidr = HOST_MASK  - 1 };
	struct ip_set_ext ext = IP_SET_INIT_UEXT(set);
	u32 port, port_to;
	bool with_ports = false;
	u8 cidr;
	int ret;

	if (tb[IPSET_ATTR_LINENO])
		*lineno = nla_get_u32(tb[IPSET_ATTR_LINENO]);

	if (unlikely(!tb[IPSET_ATTR_IP] ||
		     !ip_set_attr_netorder(tb, IPSET_ATTR_PORT) ||
		     !ip_set_optattr_netorder(tb, IPSET_ATTR_PORT_TO) ||
		     !ip_set_optattr_netorder(tb, IPSET_ATTR_CADT_FLAGS)))
		return -IPSET_ERR_PROTOCOL;
	if (unlikely(tb[IPSET_ATTR_IP_TO]))
		return -IPSET_ERR_HASH_RANGE_UNSUPPORTED;

	ret = ip_set_get_ipaddr6(tb[IPSET_ATTR_IP], &e.ip);
	if (ret)
		return ret;

	ret = ip_set_get_extensions(set, tb, &ext);
	if (ret)
		return ret;

	if (tb[IPSET_ATTR_CIDR]) {
		cidr = nla_get_u8(tb[IPSET_ATTR_CIDR]);
		if (!cidr || cidr > HOST_MASK)
			return -IPSET_ERR_INVALID_CIDR;
		e.cidr = cidr - 1;
	}
	ip6_netmask(&e.ip, e.cidr + 1);

	e.port = nla_get_be16(tb[IPSET_ATTR_PORT]);

	if (tb[IPSET_ATTR_PROTO]) {
		e.proto = nla_get_u8(tb[IPSET_ATTR_PROTO]);
		with_ports = ip_set_proto_with_ports(e.proto);

		if (e.proto == 0)
			return -IPSET_ERR_INVALID_PROTO;
	} else {
		return -IPSET_ERR_MISSING_PROTO;
	}

	if (!(with_ports || e.proto == IPPROTO_ICMPV6))
		e.port = 0;

	if (tb[IPSET_ATTR_CADT_FLAGS]) {
		u32 cadt_flags = ip_set_get_h32(tb[IPSET_ATTR_CADT_FLAGS]);

		if (cadt_flags & IPSET_FLAG_NOMATCH)
			flags |= (IPSET_FLAG_NOMATCH << 16);
	}

	if (adt == IPSET_TEST || !with_ports || !tb[IPSET_ATTR_PORT_TO]) {
		ret = adtfn(set, &e, &ext, &ext, flags);
		return ip_set_enomatch(ret, flags, adt, set) ? -ret :
		       ip_set_eexist(ret, flags) ? 0 : ret;
	}

	port = ntohs(e.port);
	port_to = ip_set_get_h16(tb[IPSET_ATTR_PORT_TO]);
	if (port > port_to)
		swap(port, port_to);

	if (retried)
		port = ntohs(h->next.port);
	for (; port <= port_to; port++) {
		e.port = htons(port);
		ret = adtfn(set, &e, &ext, &ext, flags);

		if (ret && !ip_set_eexist(ret, flags))
			return ret;

		ret = 0;
	}
	return ret;
}

static struct ip_set_type hash_netport_type __read_mostly = {
	.name		= "hash:net,port",
	.protocol	= IPSET_PROTOCOL,
	.features	= IPSET_TYPE_IP | IPSET_TYPE_PORT | IPSET_TYPE_NOMATCH,
	.dimension	= IPSET_DIM_TWO,
	.family		= NFPROTO_UNSPEC,
	.revision_min	= IPSET_TYPE_REV_MIN,
	.revision_max	= IPSET_TYPE_REV_MAX,
	.create_flags[IPSET_TYPE_REV_MAX] = IPSET_CREATE_FLAG_BUCKETSIZE,
	.create		= hash_netport_create,
	.create_policy	= {
		[IPSET_ATTR_HASHSIZE]	= { .type = NLA_U32 },
		[IPSET_ATTR_MAXELEM]	= { .type = NLA_U32 },
		[IPSET_ATTR_INITVAL]	= { .type = NLA_U32 },
		[IPSET_ATTR_BUCKETSIZE]	= { .type = NLA_U8 },
		[IPSET_ATTR_RESIZE]	= { .type = NLA_U8  },
		[IPSET_ATTR_PROTO]	= { .type = NLA_U8 },
		[IPSET_ATTR_TIMEOUT]	= { .type = NLA_U32 },
		[IPSET_ATTR_CADT_FLAGS]	= { .type = NLA_U32 },
	},
	.adt_policy	= {
		[IPSET_ATTR_IP]		= { .type = NLA_NESTED },
		[IPSET_ATTR_IP_TO]	= { .type = NLA_NESTED },
		[IPSET_ATTR_PORT]	= { .type = NLA_U16 },
		[IPSET_ATTR_PORT_TO]	= { .type = NLA_U16 },
		[IPSET_ATTR_PROTO]	= { .type = NLA_U8 },
		[IPSET_ATTR_CIDR]	= { .type = NLA_U8 },
		[IPSET_ATTR_TIMEOUT]	= { .type = NLA_U32 },
		[IPSET_ATTR_LINENO]	= { .type = NLA_U32 },
		[IPSET_ATTR_CADT_FLAGS]	= { .type = NLA_U32 },
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
hash_netport_init(void)
{
	return ip_set_type_register(&hash_netport_type);
}

static void __exit
hash_netport_fini(void)
{
	rcu_barrier();
	ip_set_type_unregister(&hash_netport_type);
}

module_init(hash_netport_init);
module_exit(hash_netport_fini);
