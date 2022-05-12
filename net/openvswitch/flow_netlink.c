// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2007-2017 Nicira, Inc.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include "flow.h"
#include "datapath.h"
#include <linux/uaccess.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/if_ether.h>
#include <linux/if_vlan.h>
#include <net/llc_pdu.h>
#include <linux/kernel.h>
#include <linux/jhash.h>
#include <linux/jiffies.h>
#include <linux/llc.h>
#include <linux/module.h>
#include <linux/in.h>
#include <linux/rcupdate.h>
#include <linux/if_arp.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/sctp.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/icmp.h>
#include <linux/icmpv6.h>
#include <linux/rculist.h>
#include <net/geneve.h>
#include <net/ip.h>
#include <net/ipv6.h>
#include <net/ndisc.h>
#include <net/mpls.h>
#include <net/vxlan.h>
#include <net/tun_proto.h>
#include <net/erspan.h>

#include "flow_netlink.h"

struct ovs_len_tbl {
	int len;
	const struct ovs_len_tbl *next;
};

#define OVS_ATTR_NESTED -1
#define OVS_ATTR_VARIABLE -2

static bool actions_may_change_flow(const struct nlattr *actions)
{
	struct nlattr *nla;
	int rem;

	nla_for_each_nested(nla, actions, rem) {
		u16 action = nla_type(nla);

		switch (action) {
		case OVS_ACTION_ATTR_OUTPUT:
		case OVS_ACTION_ATTR_RECIRC:
		case OVS_ACTION_ATTR_TRUNC:
		case OVS_ACTION_ATTR_USERSPACE:
			break;

		case OVS_ACTION_ATTR_CT:
		case OVS_ACTION_ATTR_CT_CLEAR:
		case OVS_ACTION_ATTR_HASH:
		case OVS_ACTION_ATTR_POP_ETH:
		case OVS_ACTION_ATTR_POP_MPLS:
		case OVS_ACTION_ATTR_POP_NSH:
		case OVS_ACTION_ATTR_POP_VLAN:
		case OVS_ACTION_ATTR_PUSH_ETH:
		case OVS_ACTION_ATTR_PUSH_MPLS:
		case OVS_ACTION_ATTR_PUSH_NSH:
		case OVS_ACTION_ATTR_PUSH_VLAN:
		case OVS_ACTION_ATTR_SAMPLE:
		case OVS_ACTION_ATTR_SET:
		case OVS_ACTION_ATTR_SET_MASKED:
		case OVS_ACTION_ATTR_METER:
		case OVS_ACTION_ATTR_CHECK_PKT_LEN:
		case OVS_ACTION_ATTR_ADD_MPLS:
		case OVS_ACTION_ATTR_DEC_TTL:
		default:
			return true;
		}
	}
	return false;
}

static void update_range(struct sw_flow_match *match,
			 size_t offset, size_t size, bool is_mask)
{
	struct sw_flow_key_range *range;
	size_t start = rounddown(offset, sizeof(long));
	size_t end = roundup(offset + size, sizeof(long));

	if (!is_mask)
		range = &match->range;
	else
		range = &match->mask->range;

	if (range->start == range->end) {
		range->start = start;
		range->end = end;
		return;
	}

	if (range->start > start)
		range->start = start;

	if (range->end < end)
		range->end = end;
}

#define SW_FLOW_KEY_PUT(match, field, value, is_mask) \
	do { \
		update_range(match, offsetof(struct sw_flow_key, field),    \
			     sizeof((match)->key->field), is_mask);	    \
		if (is_mask)						    \
			(match)->mask->key.field = value;		    \
		else							    \
			(match)->key->field = value;		            \
	} while (0)

#define SW_FLOW_KEY_MEMCPY_OFFSET(match, offset, value_p, len, is_mask)	    \
	do {								    \
		update_range(match, offset, len, is_mask);		    \
		if (is_mask)						    \
			memcpy((u8 *)&(match)->mask->key + offset, value_p, \
			       len);					   \
		else							    \
			memcpy((u8 *)(match)->key + offset, value_p, len);  \
	} while (0)

#define SW_FLOW_KEY_MEMCPY(match, field, value_p, len, is_mask)		      \
	SW_FLOW_KEY_MEMCPY_OFFSET(match, offsetof(struct sw_flow_key, field), \
				  value_p, len, is_mask)

#define SW_FLOW_KEY_MEMSET_FIELD(match, field, value, is_mask)		    \
	do {								    \
		update_range(match, offsetof(struct sw_flow_key, field),    \
			     sizeof((match)->key->field), is_mask);	    \
		if (is_mask)						    \
			memset((u8 *)&(match)->mask->key.field, value,      \
			       sizeof((match)->mask->key.field));	    \
		else							    \
			memset((u8 *)&(match)->key->field, value,           \
			       sizeof((match)->key->field));                \
	} while (0)

static bool match_validate(const struct sw_flow_match *match,
			   u64 key_attrs, u64 mask_attrs, bool log)
{
	u64 key_expected = 0;
	u64 mask_allowed = key_attrs;  /* At most allow all key attributes */

	/* The following mask attributes allowed only if they
	 * pass the validation tests. */
	mask_allowed &= ~((1 << OVS_KEY_ATTR_IPV4)
			| (1 << OVS_KEY_ATTR_CT_ORIG_TUPLE_IPV4)
			| (1 << OVS_KEY_ATTR_IPV6)
			| (1 << OVS_KEY_ATTR_CT_ORIG_TUPLE_IPV6)
			| (1 << OVS_KEY_ATTR_TCP)
			| (1 << OVS_KEY_ATTR_TCP_FLAGS)
			| (1 << OVS_KEY_ATTR_UDP)
			| (1 << OVS_KEY_ATTR_SCTP)
			| (1 << OVS_KEY_ATTR_ICMP)
			| (1 << OVS_KEY_ATTR_ICMPV6)
			| (1 << OVS_KEY_ATTR_ARP)
			| (1 << OVS_KEY_ATTR_ND)
			| (1 << OVS_KEY_ATTR_MPLS)
			| (1 << OVS_KEY_ATTR_NSH));

	/* Always allowed mask fields. */
	mask_allowed |= ((1 << OVS_KEY_ATTR_TUNNEL)
		       | (1 << OVS_KEY_ATTR_IN_PORT)
		       | (1 << OVS_KEY_ATTR_ETHERTYPE));

	/* Check key attributes. */
	if (match->key->eth.type == htons(ETH_P_ARP)
			|| match->key->eth.type == htons(ETH_P_RARP)) {
		key_expected |= 1 << OVS_KEY_ATTR_ARP;
		if (match->mask && (match->mask->key.eth.type == htons(0xffff)))
			mask_allowed |= 1 << OVS_KEY_ATTR_ARP;
	}

	if (eth_p_mpls(match->key->eth.type)) {
		key_expected |= 1 << OVS_KEY_ATTR_MPLS;
		if (match->mask && (match->mask->key.eth.type == htons(0xffff)))
			mask_allowed |= 1 << OVS_KEY_ATTR_MPLS;
	}

	if (match->key->eth.type == htons(ETH_P_IP)) {
		key_expected |= 1 << OVS_KEY_ATTR_IPV4;
		if (match->mask && match->mask->key.eth.type == htons(0xffff)) {
			mask_allowed |= 1 << OVS_KEY_ATTR_IPV4;
			mask_allowed |= 1 << OVS_KEY_ATTR_CT_ORIG_TUPLE_IPV4;
		}

		if (match->key->ip.frag != OVS_FRAG_TYPE_LATER) {
			if (match->key->ip.proto == IPPROTO_UDP) {
				key_expected |= 1 << OVS_KEY_ATTR_UDP;
				if (match->mask && (match->mask->key.ip.proto == 0xff))
					mask_allowed |= 1 << OVS_KEY_ATTR_UDP;
			}

			if (match->key->ip.proto == IPPROTO_SCTP) {
				key_expected |= 1 << OVS_KEY_ATTR_SCTP;
				if (match->mask && (match->mask->key.ip.proto == 0xff))
					mask_allowed |= 1 << OVS_KEY_ATTR_SCTP;
			}

			if (match->key->ip.proto == IPPROTO_TCP) {
				key_expected |= 1 << OVS_KEY_ATTR_TCP;
				key_expected |= 1 << OVS_KEY_ATTR_TCP_FLAGS;
				if (match->mask && (match->mask->key.ip.proto == 0xff)) {
					mask_allowed |= 1 << OVS_KEY_ATTR_TCP;
					mask_allowed |= 1 << OVS_KEY_ATTR_TCP_FLAGS;
				}
			}

			if (match->key->ip.proto == IPPROTO_ICMP) {
				key_expected |= 1 << OVS_KEY_ATTR_ICMP;
				if (match->mask && (match->mask->key.ip.proto == 0xff))
					mask_allowed |= 1 << OVS_KEY_ATTR_ICMP;
			}
		}
	}

	if (match->key->eth.type == htons(ETH_P_IPV6)) {
		key_expected |= 1 << OVS_KEY_ATTR_IPV6;
		if (match->mask && match->mask->key.eth.type == htons(0xffff)) {
			mask_allowed |= 1 << OVS_KEY_ATTR_IPV6;
			mask_allowed |= 1 << OVS_KEY_ATTR_CT_ORIG_TUPLE_IPV6;
		}

		if (match->key->ip.frag != OVS_FRAG_TYPE_LATER) {
			if (match->key->ip.proto == IPPROTO_UDP) {
				key_expected |= 1 << OVS_KEY_ATTR_UDP;
				if (match->mask && (match->mask->key.ip.proto == 0xff))
					mask_allowed |= 1 << OVS_KEY_ATTR_UDP;
			}

			if (match->key->ip.proto == IPPROTO_SCTP) {
				key_expected |= 1 << OVS_KEY_ATTR_SCTP;
				if (match->mask && (match->mask->key.ip.proto == 0xff))
					mask_allowed |= 1 << OVS_KEY_ATTR_SCTP;
			}

			if (match->key->ip.proto == IPPROTO_TCP) {
				key_expected |= 1 << OVS_KEY_ATTR_TCP;
				key_expected |= 1 << OVS_KEY_ATTR_TCP_FLAGS;
				if (match->mask && (match->mask->key.ip.proto == 0xff)) {
					mask_allowed |= 1 << OVS_KEY_ATTR_TCP;
					mask_allowed |= 1 << OVS_KEY_ATTR_TCP_FLAGS;
				}
			}

			if (match->key->ip.proto == IPPROTO_ICMPV6) {
				key_expected |= 1 << OVS_KEY_ATTR_ICMPV6;
				if (match->mask && (match->mask->key.ip.proto == 0xff))
					mask_allowed |= 1 << OVS_KEY_ATTR_ICMPV6;

				if (match->key->tp.src ==
						htons(NDISC_NEIGHBOUR_SOLICITATION) ||
				    match->key->tp.src == htons(NDISC_NEIGHBOUR_ADVERTISEMENT)) {
					key_expected |= 1 << OVS_KEY_ATTR_ND;
					/* Original direction conntrack tuple
					 * uses the same space as the ND fields
					 * in the key, so both are not allowed
					 * at the same time.
					 */
					mask_allowed &= ~(1ULL << OVS_KEY_ATTR_CT_ORIG_TUPLE_IPV6);
					if (match->mask && (match->mask->key.tp.src == htons(0xff)))
						mask_allowed |= 1 << OVS_KEY_ATTR_ND;
				}
			}
		}
	}

	if (match->key->eth.type == htons(ETH_P_NSH)) {
		key_expected |= 1 << OVS_KEY_ATTR_NSH;
		if (match->mask &&
		    match->mask->key.eth.type == htons(0xffff)) {
			mask_allowed |= 1 << OVS_KEY_ATTR_NSH;
		}
	}

	if ((key_attrs & key_expected) != key_expected) {
		/* Key attributes check failed. */
		OVS_NLERR(log, "Missing key (keys=%llx, expected=%llx)",
			  (unsigned long long)key_attrs,
			  (unsigned long long)key_expected);
		return false;
	}

	if ((mask_attrs & mask_allowed) != mask_attrs) {
		/* Mask attributes check failed. */
		OVS_NLERR(log, "Unexpected mask (mask=%llx, allowed=%llx)",
			  (unsigned long long)mask_attrs,
			  (unsigned long long)mask_allowed);
		return false;
	}

	return true;
}

size_t ovs_tun_key_attr_size(void)
{
	/* Whenever adding new OVS_TUNNEL_KEY_ FIELDS, we should consider
	 * updating this function.
	 */
	return    nla_total_size_64bit(8) /* OVS_TUNNEL_KEY_ATTR_ID */
		+ nla_total_size(16)   /* OVS_TUNNEL_KEY_ATTR_IPV[46]_SRC */
		+ nla_total_size(16)   /* OVS_TUNNEL_KEY_ATTR_IPV[46]_DST */
		+ nla_total_size(1)    /* OVS_TUNNEL_KEY_ATTR_TOS */
		+ nla_total_size(1)    /* OVS_TUNNEL_KEY_ATTR_TTL */
		+ nla_total_size(0)    /* OVS_TUNNEL_KEY_ATTR_DONT_FRAGMENT */
		+ nla_total_size(0)    /* OVS_TUNNEL_KEY_ATTR_CSUM */
		+ nla_total_size(0)    /* OVS_TUNNEL_KEY_ATTR_OAM */
		+ nla_total_size(256)  /* OVS_TUNNEL_KEY_ATTR_GENEVE_OPTS */
		/* OVS_TUNNEL_KEY_ATTR_VXLAN_OPTS and
		 * OVS_TUNNEL_KEY_ATTR_ERSPAN_OPTS is mutually exclusive with
		 * OVS_TUNNEL_KEY_ATTR_GENEVE_OPTS and covered by it.
		 */
		+ nla_total_size(2)    /* OVS_TUNNEL_KEY_ATTR_TP_SRC */
		+ nla_total_size(2);   /* OVS_TUNNEL_KEY_ATTR_TP_DST */
}

static size_t ovs_nsh_key_attr_size(void)
{
	/* Whenever adding new OVS_NSH_KEY_ FIELDS, we should consider
	 * updating this function.
	 */
	return  nla_total_size(NSH_BASE_HDR_LEN) /* OVS_NSH_KEY_ATTR_BASE */
		/* OVS_NSH_KEY_ATTR_MD1 and OVS_NSH_KEY_ATTR_MD2 are
		 * mutually exclusive, so the bigger one can cover
		 * the small one.
		 */
		+ nla_total_size(NSH_CTX_HDRS_MAX_LEN);
}

size_t ovs_key_attr_size(void)
{
	/* Whenever adding new OVS_KEY_ FIELDS, we should consider
	 * updating this function.
	 */
	BUILD_BUG_ON(OVS_KEY_ATTR_TUNNEL_INFO != 29);

	return    nla_total_size(4)   /* OVS_KEY_ATTR_PRIORITY */
		+ nla_total_size(0)   /* OVS_KEY_ATTR_TUNNEL */
		  + ovs_tun_key_attr_size()
		+ nla_total_size(4)   /* OVS_KEY_ATTR_IN_PORT */
		+ nla_total_size(4)   /* OVS_KEY_ATTR_SKB_MARK */
		+ nla_total_size(4)   /* OVS_KEY_ATTR_DP_HASH */
		+ nla_total_size(4)   /* OVS_KEY_ATTR_RECIRC_ID */
		+ nla_total_size(4)   /* OVS_KEY_ATTR_CT_STATE */
		+ nla_total_size(2)   /* OVS_KEY_ATTR_CT_ZONE */
		+ nla_total_size(4)   /* OVS_KEY_ATTR_CT_MARK */
		+ nla_total_size(16)  /* OVS_KEY_ATTR_CT_LABELS */
		+ nla_total_size(40)  /* OVS_KEY_ATTR_CT_ORIG_TUPLE_IPV6 */
		+ nla_total_size(0)   /* OVS_KEY_ATTR_NSH */
		  + ovs_nsh_key_attr_size()
		+ nla_total_size(12)  /* OVS_KEY_ATTR_ETHERNET */
		+ nla_total_size(2)   /* OVS_KEY_ATTR_ETHERTYPE */
		+ nla_total_size(4)   /* OVS_KEY_ATTR_VLAN */
		+ nla_total_size(0)   /* OVS_KEY_ATTR_ENCAP */
		+ nla_total_size(2)   /* OVS_KEY_ATTR_ETHERTYPE */
		+ nla_total_size(40)  /* OVS_KEY_ATTR_IPV6 */
		+ nla_total_size(2)   /* OVS_KEY_ATTR_ICMPV6 */
		+ nla_total_size(28); /* OVS_KEY_ATTR_ND */
}

static const struct ovs_len_tbl ovs_vxlan_ext_key_lens[OVS_VXLAN_EXT_MAX + 1] = {
	[OVS_VXLAN_EXT_GBP]	    = { .len = sizeof(u32) },
};

static const struct ovs_len_tbl ovs_tunnel_key_lens[OVS_TUNNEL_KEY_ATTR_MAX + 1] = {
	[OVS_TUNNEL_KEY_ATTR_ID]	    = { .len = sizeof(u64) },
	[OVS_TUNNEL_KEY_ATTR_IPV4_SRC]	    = { .len = sizeof(u32) },
	[OVS_TUNNEL_KEY_ATTR_IPV4_DST]	    = { .len = sizeof(u32) },
	[OVS_TUNNEL_KEY_ATTR_TOS]	    = { .len = 1 },
	[OVS_TUNNEL_KEY_ATTR_TTL]	    = { .len = 1 },
	[OVS_TUNNEL_KEY_ATTR_DONT_FRAGMENT] = { .len = 0 },
	[OVS_TUNNEL_KEY_ATTR_CSUM]	    = { .len = 0 },
	[OVS_TUNNEL_KEY_ATTR_TP_SRC]	    = { .len = sizeof(u16) },
	[OVS_TUNNEL_KEY_ATTR_TP_DST]	    = { .len = sizeof(u16) },
	[OVS_TUNNEL_KEY_ATTR_OAM]	    = { .len = 0 },
	[OVS_TUNNEL_KEY_ATTR_GENEVE_OPTS]   = { .len = OVS_ATTR_VARIABLE },
	[OVS_TUNNEL_KEY_ATTR_VXLAN_OPTS]    = { .len = OVS_ATTR_NESTED,
						.next = ovs_vxlan_ext_key_lens },
	[OVS_TUNNEL_KEY_ATTR_IPV6_SRC]      = { .len = sizeof(struct in6_addr) },
	[OVS_TUNNEL_KEY_ATTR_IPV6_DST]      = { .len = sizeof(struct in6_addr) },
	[OVS_TUNNEL_KEY_ATTR_ERSPAN_OPTS]   = { .len = OVS_ATTR_VARIABLE },
	[OVS_TUNNEL_KEY_ATTR_IPV4_INFO_BRIDGE]   = { .len = 0 },
};

static const struct ovs_len_tbl
ovs_nsh_key_attr_lens[OVS_NSH_KEY_ATTR_MAX + 1] = {
	[OVS_NSH_KEY_ATTR_BASE] = { .len = sizeof(struct ovs_nsh_key_base) },
	[OVS_NSH_KEY_ATTR_MD1]  = { .len = sizeof(struct ovs_nsh_key_md1) },
	[OVS_NSH_KEY_ATTR_MD2]  = { .len = OVS_ATTR_VARIABLE },
};

/* The size of the argument for each %OVS_KEY_ATTR_* Netlink attribute.  */
static const struct ovs_len_tbl ovs_key_lens[OVS_KEY_ATTR_MAX + 1] = {
	[OVS_KEY_ATTR_ENCAP]	 = { .len = OVS_ATTR_NESTED },
	[OVS_KEY_ATTR_PRIORITY]	 = { .len = sizeof(u32) },
	[OVS_KEY_ATTR_IN_PORT]	 = { .len = sizeof(u32) },
	[OVS_KEY_ATTR_SKB_MARK]	 = { .len = sizeof(u32) },
	[OVS_KEY_ATTR_ETHERNET]	 = { .len = sizeof(struct ovs_key_ethernet) },
	[OVS_KEY_ATTR_VLAN]	 = { .len = sizeof(__be16) },
	[OVS_KEY_ATTR_ETHERTYPE] = { .len = sizeof(__be16) },
	[OVS_KEY_ATTR_IPV4]	 = { .len = sizeof(struct ovs_key_ipv4) },
	[OVS_KEY_ATTR_IPV6]	 = { .len = sizeof(struct ovs_key_ipv6) },
	[OVS_KEY_ATTR_TCP]	 = { .len = sizeof(struct ovs_key_tcp) },
	[OVS_KEY_ATTR_TCP_FLAGS] = { .len = sizeof(__be16) },
	[OVS_KEY_ATTR_UDP]	 = { .len = sizeof(struct ovs_key_udp) },
	[OVS_KEY_ATTR_SCTP]	 = { .len = sizeof(struct ovs_key_sctp) },
	[OVS_KEY_ATTR_ICMP]	 = { .len = sizeof(struct ovs_key_icmp) },
	[OVS_KEY_ATTR_ICMPV6]	 = { .len = sizeof(struct ovs_key_icmpv6) },
	[OVS_KEY_ATTR_ARP]	 = { .len = sizeof(struct ovs_key_arp) },
	[OVS_KEY_ATTR_ND]	 = { .len = sizeof(struct ovs_key_nd) },
	[OVS_KEY_ATTR_RECIRC_ID] = { .len = sizeof(u32) },
	[OVS_KEY_ATTR_DP_HASH]	 = { .len = sizeof(u32) },
	[OVS_KEY_ATTR_TUNNEL]	 = { .len = OVS_ATTR_NESTED,
				     .next = ovs_tunnel_key_lens, },
	[OVS_KEY_ATTR_MPLS]	 = { .len = OVS_ATTR_VARIABLE },
	[OVS_KEY_ATTR_CT_STATE]	 = { .len = sizeof(u32) },
	[OVS_KEY_ATTR_CT_ZONE]	 = { .len = sizeof(u16) },
	[OVS_KEY_ATTR_CT_MARK]	 = { .len = sizeof(u32) },
	[OVS_KEY_ATTR_CT_LABELS] = { .len = sizeof(struct ovs_key_ct_labels) },
	[OVS_KEY_ATTR_CT_ORIG_TUPLE_IPV4] = {
		.len = sizeof(struct ovs_key_ct_tuple_ipv4) },
	[OVS_KEY_ATTR_CT_ORIG_TUPLE_IPV6] = {
		.len = sizeof(struct ovs_key_ct_tuple_ipv6) },
	[OVS_KEY_ATTR_NSH]       = { .len = OVS_ATTR_NESTED,
				     .next = ovs_nsh_key_attr_lens, },
};

static bool check_attr_len(unsigned int attr_len, unsigned int expected_len)
{
	return expected_len == attr_len ||
	       expected_len == OVS_ATTR_NESTED ||
	       expected_len == OVS_ATTR_VARIABLE;
}

static bool is_all_zero(const u8 *fp, size_t size)
{
	int i;

	if (!fp)
		return false;

	for (i = 0; i < size; i++)
		if (fp[i])
			return false;

	return true;
}

static int __parse_flow_nlattrs(const struct nlattr *attr,
				const struct nlattr *a[],
				u64 *attrsp, bool log, bool nz)
{
	const struct nlattr *nla;
	u64 attrs;
	int rem;

	attrs = *attrsp;
	nla_for_each_nested(nla, attr, rem) {
		u16 type = nla_type(nla);
		int expected_len;

		if (type > OVS_KEY_ATTR_MAX) {
			OVS_NLERR(log, "Key type %d is out of range max %d",
				  type, OVS_KEY_ATTR_MAX);
			return -EINVAL;
		}

		if (attrs & (1 << type)) {
			OVS_NLERR(log, "Duplicate key (type %d).", type);
			return -EINVAL;
		}

		expected_len = ovs_key_lens[type].len;
		if (!check_attr_len(nla_len(nla), expected_len)) {
			OVS_NLERR(log, "Key %d has unexpected len %d expected %d",
				  type, nla_len(nla), expected_len);
			return -EINVAL;
		}

		if (!nz || !is_all_zero(nla_data(nla), nla_len(nla))) {
			attrs |= 1 << type;
			a[type] = nla;
		}
	}
	if (rem) {
		OVS_NLERR(log, "Message has %d unknown bytes.", rem);
		return -EINVAL;
	}

	*attrsp = attrs;
	return 0;
}

static int parse_flow_mask_nlattrs(const struct nlattr *attr,
				   const struct nlattr *a[], u64 *attrsp,
				   bool log)
{
	return __parse_flow_nlattrs(attr, a, attrsp, log, true);
}

int parse_flow_nlattrs(const struct nlattr *attr, const struct nlattr *a[],
		       u64 *attrsp, bool log)
{
	return __parse_flow_nlattrs(attr, a, attrsp, log, false);
}

static int genev_tun_opt_from_nlattr(const struct nlattr *a,
				     struct sw_flow_match *match, bool is_mask,
				     bool log)
{
	unsigned long opt_key_offset;

	if (nla_len(a) > sizeof(match->key->tun_opts)) {
		OVS_NLERR(log, "Geneve option length err (len %d, max %zu).",
			  nla_len(a), sizeof(match->key->tun_opts));
		return -EINVAL;
	}

	if (nla_len(a) % 4 != 0) {
		OVS_NLERR(log, "Geneve opt len %d is not a multiple of 4.",
			  nla_len(a));
		return -EINVAL;
	}

	/* We need to record the length of the options passed
	 * down, otherwise packets with the same format but
	 * additional options will be silently matched.
	 */
	if (!is_mask) {
		SW_FLOW_KEY_PUT(match, tun_opts_len, nla_len(a),
				false);
	} else {
		/* This is somewhat unusual because it looks at
		 * both the key and mask while parsing the
		 * attributes (and by extension assumes the key
		 * is parsed first). Normally, we would verify
		 * that each is the correct length and that the
		 * attributes line up in the validate function.
		 * However, that is difficult because this is
		 * variable length and we won't have the
		 * information later.
		 */
		if (match->key->tun_opts_len != nla_len(a)) {
			OVS_NLERR(log, "Geneve option len %d != mask len %d",
				  match->key->tun_opts_len, nla_len(a));
			return -EINVAL;
		}

		SW_FLOW_KEY_PUT(match, tun_opts_len, 0xff, true);
	}

	opt_key_offset = TUN_METADATA_OFFSET(nla_len(a));
	SW_FLOW_KEY_MEMCPY_OFFSET(match, opt_key_offset, nla_data(a),
				  nla_len(a), is_mask);
	return 0;
}

static int vxlan_tun_opt_from_nlattr(const struct nlattr *attr,
				     struct sw_flow_match *match, bool is_mask,
				     bool log)
{
	struct nlattr *a;
	int rem;
	unsigned long opt_key_offset;
	struct vxlan_metadata opts;

	BUILD_BUG_ON(sizeof(opts) > sizeof(match->key->tun_opts));

	memset(&opts, 0, sizeof(opts));
	nla_for_each_nested(a, attr, rem) {
		int type = nla_type(a);

		if (type > OVS_VXLAN_EXT_MAX) {
			OVS_NLERR(log, "VXLAN extension %d out of range max %d",
				  type, OVS_VXLAN_EXT_MAX);
			return -EINVAL;
		}

		if (!check_attr_len(nla_len(a),
				    ovs_vxlan_ext_key_lens[type].len)) {
			OVS_NLERR(log, "VXLAN extension %d has unexpected len %d expected %d",
				  type, nla_len(a),
				  ovs_vxlan_ext_key_lens[type].len);
			return -EINVAL;
		}

		switch (type) {
		case OVS_VXLAN_EXT_GBP:
			opts.gbp = nla_get_u32(a);
			break;
		default:
			OVS_NLERR(log, "Unknown VXLAN extension attribute %d",
				  type);
			return -EINVAL;
		}
	}
	if (rem) {
		OVS_NLERR(log, "VXLAN extension message has %d unknown bytes.",
			  rem);
		return -EINVAL;
	}

	if (!is_mask)
		SW_FLOW_KEY_PUT(match, tun_opts_len, sizeof(opts), false);
	else
		SW_FLOW_KEY_PUT(match, tun_opts_len, 0xff, true);

	opt_key_offset = TUN_METADATA_OFFSET(sizeof(opts));
	SW_FLOW_KEY_MEMCPY_OFFSET(match, opt_key_offset, &opts, sizeof(opts),
				  is_mask);
	return 0;
}

static int erspan_tun_opt_from_nlattr(const struct nlattr *a,
				      struct sw_flow_match *match, bool is_mask,
				      bool log)
{
	unsigned long opt_key_offset;

	BUILD_BUG_ON(sizeof(struct erspan_metadata) >
		     sizeof(match->key->tun_opts));

	if (nla_len(a) > sizeof(match->key->tun_opts)) {
		OVS_NLERR(log, "ERSPAN option length err (len %d, max %zu).",
			  nla_len(a), sizeof(match->key->tun_opts));
		return -EINVAL;
	}

	if (!is_mask)
		SW_FLOW_KEY_PUT(match, tun_opts_len,
				sizeof(struct erspan_metadata), false);
	else
		SW_FLOW_KEY_PUT(match, tun_opts_len, 0xff, true);

	opt_key_offset = TUN_METADATA_OFFSET(nla_len(a));
	SW_FLOW_KEY_MEMCPY_OFFSET(match, opt_key_offset, nla_data(a),
				  nla_len(a), is_mask);
	return 0;
}

static int ip_tun_from_nlattr(const struct nlattr *attr,
			      struct sw_flow_match *match, bool is_mask,
			      bool log)
{
	bool ttl = false, ipv4 = false, ipv6 = false;
	bool info_bridge_mode = false;
	__be16 tun_flags = 0;
	int opts_type = 0;
	struct nlattr *a;
	int rem;

	nla_for_each_nested(a, attr, rem) {
		int type = nla_type(a);
		int err;

		if (type > OVS_TUNNEL_KEY_ATTR_MAX) {
			OVS_NLERR(log, "Tunnel attr %d out of range max %d",
				  type, OVS_TUNNEL_KEY_ATTR_MAX);
			return -EINVAL;
		}

		if (!check_attr_len(nla_len(a),
				    ovs_tunnel_key_lens[type].len)) {
			OVS_NLERR(log, "Tunnel attr %d has unexpected len %d expected %d",
				  type, nla_len(a), ovs_tunnel_key_lens[type].len);
			return -EINVAL;
		}

		switch (type) {
		case OVS_TUNNEL_KEY_ATTR_ID:
			SW_FLOW_KEY_PUT(match, tun_key.tun_id,
					nla_get_be64(a), is_mask);
			tun_flags |= TUNNEL_KEY;
			break;
		case OVS_TUNNEL_KEY_ATTR_IPV4_SRC:
			SW_FLOW_KEY_PUT(match, tun_key.u.ipv4.src,
					nla_get_in_addr(a), is_mask);
			ipv4 = true;
			break;
		case OVS_TUNNEL_KEY_ATTR_IPV4_DST:
			SW_FLOW_KEY_PUT(match, tun_key.u.ipv4.dst,
					nla_get_in_addr(a), is_mask);
			ipv4 = true;
			break;
		case OVS_TUNNEL_KEY_ATTR_IPV6_SRC:
			SW_FLOW_KEY_PUT(match, tun_key.u.ipv6.src,
					nla_get_in6_addr(a), is_mask);
			ipv6 = true;
			break;
		case OVS_TUNNEL_KEY_ATTR_IPV6_DST:
			SW_FLOW_KEY_PUT(match, tun_key.u.ipv6.dst,
					nla_get_in6_addr(a), is_mask);
			ipv6 = true;
			break;
		case OVS_TUNNEL_KEY_ATTR_TOS:
			SW_FLOW_KEY_PUT(match, tun_key.tos,
					nla_get_u8(a), is_mask);
			break;
		case OVS_TUNNEL_KEY_ATTR_TTL:
			SW_FLOW_KEY_PUT(match, tun_key.ttl,
					nla_get_u8(a), is_mask);
			ttl = true;
			break;
		case OVS_TUNNEL_KEY_ATTR_DONT_FRAGMENT:
			tun_flags |= TUNNEL_DONT_FRAGMENT;
			break;
		case OVS_TUNNEL_KEY_ATTR_CSUM:
			tun_flags |= TUNNEL_CSUM;
			break;
		case OVS_TUNNEL_KEY_ATTR_TP_SRC:
			SW_FLOW_KEY_PUT(match, tun_key.tp_src,
					nla_get_be16(a), is_mask);
			break;
		case OVS_TUNNEL_KEY_ATTR_TP_DST:
			SW_FLOW_KEY_PUT(match, tun_key.tp_dst,
					nla_get_be16(a), is_mask);
			break;
		case OVS_TUNNEL_KEY_ATTR_OAM:
			tun_flags |= TUNNEL_OAM;
			break;
		case OVS_TUNNEL_KEY_ATTR_GENEVE_OPTS:
			if (opts_type) {
				OVS_NLERR(log, "Multiple metadata blocks provided");
				return -EINVAL;
			}

			err = genev_tun_opt_from_nlattr(a, match, is_mask, log);
			if (err)
				return err;

			tun_flags |= TUNNEL_GENEVE_OPT;
			opts_type = type;
			break;
		case OVS_TUNNEL_KEY_ATTR_VXLAN_OPTS:
			if (opts_type) {
				OVS_NLERR(log, "Multiple metadata blocks provided");
				return -EINVAL;
			}

			err = vxlan_tun_opt_from_nlattr(a, match, is_mask, log);
			if (err)
				return err;

			tun_flags |= TUNNEL_VXLAN_OPT;
			opts_type = type;
			break;
		case OVS_TUNNEL_KEY_ATTR_PAD:
			break;
		case OVS_TUNNEL_KEY_ATTR_ERSPAN_OPTS:
			if (opts_type) {
				OVS_NLERR(log, "Multiple metadata blocks provided");
				return -EINVAL;
			}

			err = erspan_tun_opt_from_nlattr(a, match, is_mask,
							 log);
			if (err)
				return err;

			tun_flags |= TUNNEL_ERSPAN_OPT;
			opts_type = type;
			break;
		case OVS_TUNNEL_KEY_ATTR_IPV4_INFO_BRIDGE:
			info_bridge_mode = true;
			ipv4 = true;
			break;
		default:
			OVS_NLERR(log, "Unknown IP tunnel attribute %d",
				  type);
			return -EINVAL;
		}
	}

	SW_FLOW_KEY_PUT(match, tun_key.tun_flags, tun_flags, is_mask);
	if (is_mask)
		SW_FLOW_KEY_MEMSET_FIELD(match, tun_proto, 0xff, true);
	else
		SW_FLOW_KEY_PUT(match, tun_proto, ipv6 ? AF_INET6 : AF_INET,
				false);

	if (rem > 0) {
		OVS_NLERR(log, "IP tunnel attribute has %d unknown bytes.",
			  rem);
		return -EINVAL;
	}

	if (ipv4 && ipv6) {
		OVS_NLERR(log, "Mixed IPv4 and IPv6 tunnel attributes");
		return -EINVAL;
	}

	if (!is_mask) {
		if (!ipv4 && !ipv6) {
			OVS_NLERR(log, "IP tunnel dst address not specified");
			return -EINVAL;
		}
		if (ipv4) {
			if (info_bridge_mode) {
				if (match->key->tun_key.u.ipv4.src ||
				    match->key->tun_key.u.ipv4.dst ||
				    match->key->tun_key.tp_src ||
				    match->key->tun_key.tp_dst ||
				    match->key->tun_key.ttl ||
				    match->key->tun_key.tos ||
				    tun_flags & ~TUNNEL_KEY) {
					OVS_NLERR(log, "IPv4 tun info is not correct");
					return -EINVAL;
				}
			} else if (!match->key->tun_key.u.ipv4.dst) {
				OVS_NLERR(log, "IPv4 tunnel dst address is zero");
				return -EINVAL;
			}
		}
		if (ipv6 && ipv6_addr_any(&match->key->tun_key.u.ipv6.dst)) {
			OVS_NLERR(log, "IPv6 tunnel dst address is zero");
			return -EINVAL;
		}

		if (!ttl && !info_bridge_mode) {
			OVS_NLERR(log, "IP tunnel TTL not specified.");
			return -EINVAL;
		}
	}

	return opts_type;
}

static int vxlan_opt_to_nlattr(struct sk_buff *skb,
			       const void *tun_opts, int swkey_tun_opts_len)
{
	const struct vxlan_metadata *opts = tun_opts;
	struct nlattr *nla;

	nla = nla_nest_start_noflag(skb, OVS_TUNNEL_KEY_ATTR_VXLAN_OPTS);
	if (!nla)
		return -EMSGSIZE;

	if (nla_put_u32(skb, OVS_VXLAN_EXT_GBP, opts->gbp) < 0)
		return -EMSGSIZE;

	nla_nest_end(skb, nla);
	return 0;
}

static int __ip_tun_to_nlattr(struct sk_buff *skb,
			      const struct ip_tunnel_key *output,
			      const void *tun_opts, int swkey_tun_opts_len,
			      unsigned short tun_proto, u8 mode)
{
	if (output->tun_flags & TUNNEL_KEY &&
	    nla_put_be64(skb, OVS_TUNNEL_KEY_ATTR_ID, output->tun_id,
			 OVS_TUNNEL_KEY_ATTR_PAD))
		return -EMSGSIZE;

	if (mode & IP_TUNNEL_INFO_BRIDGE)
		return nla_put_flag(skb, OVS_TUNNEL_KEY_ATTR_IPV4_INFO_BRIDGE)
		       ? -EMSGSIZE : 0;

	switch (tun_proto) {
	case AF_INET:
		if (output->u.ipv4.src &&
		    nla_put_in_addr(skb, OVS_TUNNEL_KEY_ATTR_IPV4_SRC,
				    output->u.ipv4.src))
			return -EMSGSIZE;
		if (output->u.ipv4.dst &&
		    nla_put_in_addr(skb, OVS_TUNNEL_KEY_ATTR_IPV4_DST,
				    output->u.ipv4.dst))
			return -EMSGSIZE;
		break;
	case AF_INET6:
		if (!ipv6_addr_any(&output->u.ipv6.src) &&
		    nla_put_in6_addr(skb, OVS_TUNNEL_KEY_ATTR_IPV6_SRC,
				     &output->u.ipv6.src))
			return -EMSGSIZE;
		if (!ipv6_addr_any(&output->u.ipv6.dst) &&
		    nla_put_in6_addr(skb, OVS_TUNNEL_KEY_ATTR_IPV6_DST,
				     &output->u.ipv6.dst))
			return -EMSGSIZE;
		break;
	}
	if (output->tos &&
	    nla_put_u8(skb, OVS_TUNNEL_KEY_ATTR_TOS, output->tos))
		return -EMSGSIZE;
	if (nla_put_u8(skb, OVS_TUNNEL_KEY_ATTR_TTL, output->ttl))
		return -EMSGSIZE;
	if ((output->tun_flags & TUNNEL_DONT_FRAGMENT) &&
	    nla_put_flag(skb, OVS_TUNNEL_KEY_ATTR_DONT_FRAGMENT))
		return -EMSGSIZE;
	if ((output->tun_flags & TUNNEL_CSUM) &&
	    nla_put_flag(skb, OVS_TUNNEL_KEY_ATTR_CSUM))
		return -EMSGSIZE;
	if (output->tp_src &&
	    nla_put_be16(skb, OVS_TUNNEL_KEY_ATTR_TP_SRC, output->tp_src))
		return -EMSGSIZE;
	if (output->tp_dst &&
	    nla_put_be16(skb, OVS_TUNNEL_KEY_ATTR_TP_DST, output->tp_dst))
		return -EMSGSIZE;
	if ((output->tun_flags & TUNNEL_OAM) &&
	    nla_put_flag(skb, OVS_TUNNEL_KEY_ATTR_OAM))
		return -EMSGSIZE;
	if (swkey_tun_opts_len) {
		if (output->tun_flags & TUNNEL_GENEVE_OPT &&
		    nla_put(skb, OVS_TUNNEL_KEY_ATTR_GENEVE_OPTS,
			    swkey_tun_opts_len, tun_opts))
			return -EMSGSIZE;
		else if (output->tun_flags & TUNNEL_VXLAN_OPT &&
			 vxlan_opt_to_nlattr(skb, tun_opts, swkey_tun_opts_len))
			return -EMSGSIZE;
		else if (output->tun_flags & TUNNEL_ERSPAN_OPT &&
			 nla_put(skb, OVS_TUNNEL_KEY_ATTR_ERSPAN_OPTS,
				 swkey_tun_opts_len, tun_opts))
			return -EMSGSIZE;
	}

	return 0;
}

static int ip_tun_to_nlattr(struct sk_buff *skb,
			    const struct ip_tunnel_key *output,
			    const void *tun_opts, int swkey_tun_opts_len,
			    unsigned short tun_proto, u8 mode)
{
	struct nlattr *nla;
	int err;

	nla = nla_nest_start_noflag(skb, OVS_KEY_ATTR_TUNNEL);
	if (!nla)
		return -EMSGSIZE;

	err = __ip_tun_to_nlattr(skb, output, tun_opts, swkey_tun_opts_len,
				 tun_proto, mode);
	if (err)
		return err;

	nla_nest_end(skb, nla);
	return 0;
}

int ovs_nla_put_tunnel_info(struct sk_buff *skb,
			    struct ip_tunnel_info *tun_info)
{
	return __ip_tun_to_nlattr(skb, &tun_info->key,
				  ip_tunnel_info_opts(tun_info),
				  tun_info->options_len,
				  ip_tunnel_info_af(tun_info), tun_info->mode);
}

static int encode_vlan_from_nlattrs(struct sw_flow_match *match,
				    const struct nlattr *a[],
				    bool is_mask, bool inner)
{
	__be16 tci = 0;
	__be16 tpid = 0;

	if (a[OVS_KEY_ATTR_VLAN])
		tci = nla_get_be16(a[OVS_KEY_ATTR_VLAN]);

	if (a[OVS_KEY_ATTR_ETHERTYPE])
		tpid = nla_get_be16(a[OVS_KEY_ATTR_ETHERTYPE]);

	if (likely(!inner)) {
		SW_FLOW_KEY_PUT(match, eth.vlan.tpid, tpid, is_mask);
		SW_FLOW_KEY_PUT(match, eth.vlan.tci, tci, is_mask);
	} else {
		SW_FLOW_KEY_PUT(match, eth.cvlan.tpid, tpid, is_mask);
		SW_FLOW_KEY_PUT(match, eth.cvlan.tci, tci, is_mask);
	}
	return 0;
}

static int validate_vlan_from_nlattrs(const struct sw_flow_match *match,
				      u64 key_attrs, bool inner,
				      const struct nlattr **a, bool log)
{
	__be16 tci = 0;

	if (!((key_attrs & (1 << OVS_KEY_ATTR_ETHERNET)) &&
	      (key_attrs & (1 << OVS_KEY_ATTR_ETHERTYPE)) &&
	       eth_type_vlan(nla_get_be16(a[OVS_KEY_ATTR_ETHERTYPE])))) {
		/* Not a VLAN. */
		return 0;
	}

	if (!((key_attrs & (1 << OVS_KEY_ATTR_VLAN)) &&
	      (key_attrs & (1 << OVS_KEY_ATTR_ENCAP)))) {
		OVS_NLERR(log, "Invalid %s frame", (inner) ? "C-VLAN" : "VLAN");
		return -EINVAL;
	}

	if (a[OVS_KEY_ATTR_VLAN])
		tci = nla_get_be16(a[OVS_KEY_ATTR_VLAN]);

	if (!(tci & htons(VLAN_CFI_MASK))) {
		if (tci) {
			OVS_NLERR(log, "%s TCI does not have VLAN_CFI_MASK bit set.",
				  (inner) ? "C-VLAN" : "VLAN");
			return -EINVAL;
		} else if (nla_len(a[OVS_KEY_ATTR_ENCAP])) {
			/* Corner case for truncated VLAN header. */
			OVS_NLERR(log, "Truncated %s header has non-zero encap attribute.",
				  (inner) ? "C-VLAN" : "VLAN");
			return -EINVAL;
		}
	}

	return 1;
}

static int validate_vlan_mask_from_nlattrs(const struct sw_flow_match *match,
					   u64 key_attrs, bool inner,
					   const struct nlattr **a, bool log)
{
	__be16 tci = 0;
	__be16 tpid = 0;
	bool encap_valid = !!(match->key->eth.vlan.tci &
			      htons(VLAN_CFI_MASK));
	bool i_encap_valid = !!(match->key->eth.cvlan.tci &
				htons(VLAN_CFI_MASK));

	if (!(key_attrs & (1 << OVS_KEY_ATTR_ENCAP))) {
		/* Not a VLAN. */
		return 0;
	}

	if ((!inner && !encap_valid) || (inner && !i_encap_valid)) {
		OVS_NLERR(log, "Encap mask attribute is set for non-%s frame.",
			  (inner) ? "C-VLAN" : "VLAN");
		return -EINVAL;
	}

	if (a[OVS_KEY_ATTR_VLAN])
		tci = nla_get_be16(a[OVS_KEY_ATTR_VLAN]);

	if (a[OVS_KEY_ATTR_ETHERTYPE])
		tpid = nla_get_be16(a[OVS_KEY_ATTR_ETHERTYPE]);

	if (tpid != htons(0xffff)) {
		OVS_NLERR(log, "Must have an exact match on %s TPID (mask=%x).",
			  (inner) ? "C-VLAN" : "VLAN", ntohs(tpid));
		return -EINVAL;
	}
	if (!(tci & htons(VLAN_CFI_MASK))) {
		OVS_NLERR(log, "%s TCI mask does not have exact match for VLAN_CFI_MASK bit.",
			  (inner) ? "C-VLAN" : "VLAN");
		return -EINVAL;
	}

	return 1;
}

static int __parse_vlan_from_nlattrs(struct sw_flow_match *match,
				     u64 *key_attrs, bool inner,
				     const struct nlattr **a, bool is_mask,
				     bool log)
{
	int err;
	const struct nlattr *encap;

	if (!is_mask)
		err = validate_vlan_from_nlattrs(match, *key_attrs, inner,
						 a, log);
	else
		err = validate_vlan_mask_from_nlattrs(match, *key_attrs, inner,
						      a, log);
	if (err <= 0)
		return err;

	err = encode_vlan_from_nlattrs(match, a, is_mask, inner);
	if (err)
		return err;

	*key_attrs &= ~(1 << OVS_KEY_ATTR_ENCAP);
	*key_attrs &= ~(1 << OVS_KEY_ATTR_VLAN);
	*key_attrs &= ~(1 << OVS_KEY_ATTR_ETHERTYPE);

	encap = a[OVS_KEY_ATTR_ENCAP];

	if (!is_mask)
		err = parse_flow_nlattrs(encap, a, key_attrs, log);
	else
		err = parse_flow_mask_nlattrs(encap, a, key_attrs, log);

	return err;
}

static int parse_vlan_from_nlattrs(struct sw_flow_match *match,
				   u64 *key_attrs, const struct nlattr **a,
				   bool is_mask, bool log)
{
	int err;
	bool encap_valid = false;

	err = __parse_vlan_from_nlattrs(match, key_attrs, false, a,
					is_mask, log);
	if (err)
		return err;

	encap_valid = !!(match->key->eth.vlan.tci & htons(VLAN_CFI_MASK));
	if (encap_valid) {
		err = __parse_vlan_from_nlattrs(match, key_attrs, true, a,
						is_mask, log);
		if (err)
			return err;
	}

	return 0;
}

static int parse_eth_type_from_nlattrs(struct sw_flow_match *match,
				       u64 *attrs, const struct nlattr **a,
				       bool is_mask, bool log)
{
	__be16 eth_type;

	eth_type = nla_get_be16(a[OVS_KEY_ATTR_ETHERTYPE]);
	if (is_mask) {
		/* Always exact match EtherType. */
		eth_type = htons(0xffff);
	} else if (!eth_proto_is_802_3(eth_type)) {
		OVS_NLERR(log, "EtherType %x is less than min %x",
				ntohs(eth_type), ETH_P_802_3_MIN);
		return -EINVAL;
	}

	SW_FLOW_KEY_PUT(match, eth.type, eth_type, is_mask);
	*attrs &= ~(1 << OVS_KEY_ATTR_ETHERTYPE);
	return 0;
}

static int metadata_from_nlattrs(struct net *net, struct sw_flow_match *match,
				 u64 *attrs, const struct nlattr **a,
				 bool is_mask, bool log)
{
	u8 mac_proto = MAC_PROTO_ETHERNET;

	if (*attrs & (1 << OVS_KEY_ATTR_DP_HASH)) {
		u32 hash_val = nla_get_u32(a[OVS_KEY_ATTR_DP_HASH]);

		SW_FLOW_KEY_PUT(match, ovs_flow_hash, hash_val, is_mask);
		*attrs &= ~(1 << OVS_KEY_ATTR_DP_HASH);
	}

	if (*attrs & (1 << OVS_KEY_ATTR_RECIRC_ID)) {
		u32 recirc_id = nla_get_u32(a[OVS_KEY_ATTR_RECIRC_ID]);

		SW_FLOW_KEY_PUT(match, recirc_id, recirc_id, is_mask);
		*attrs &= ~(1 << OVS_KEY_ATTR_RECIRC_ID);
	}

	if (*attrs & (1 << OVS_KEY_ATTR_PRIORITY)) {
		SW_FLOW_KEY_PUT(match, phy.priority,
			  nla_get_u32(a[OVS_KEY_ATTR_PRIORITY]), is_mask);
		*attrs &= ~(1 << OVS_KEY_ATTR_PRIORITY);
	}

	if (*attrs & (1 << OVS_KEY_ATTR_IN_PORT)) {
		u32 in_port = nla_get_u32(a[OVS_KEY_ATTR_IN_PORT]);

		if (is_mask) {
			in_port = 0xffffffff; /* Always exact match in_port. */
		} else if (in_port >= DP_MAX_PORTS) {
			OVS_NLERR(log, "Port %d exceeds max allowable %d",
				  in_port, DP_MAX_PORTS);
			return -EINVAL;
		}

		SW_FLOW_KEY_PUT(match, phy.in_port, in_port, is_mask);
		*attrs &= ~(1 << OVS_KEY_ATTR_IN_PORT);
	} else if (!is_mask) {
		SW_FLOW_KEY_PUT(match, phy.in_port, DP_MAX_PORTS, is_mask);
	}

	if (*attrs & (1 << OVS_KEY_ATTR_SKB_MARK)) {
		uint32_t mark = nla_get_u32(a[OVS_KEY_ATTR_SKB_MARK]);

		SW_FLOW_KEY_PUT(match, phy.skb_mark, mark, is_mask);
		*attrs &= ~(1 << OVS_KEY_ATTR_SKB_MARK);
	}
	if (*attrs & (1 << OVS_KEY_ATTR_TUNNEL)) {
		if (ip_tun_from_nlattr(a[OVS_KEY_ATTR_TUNNEL], match,
				       is_mask, log) < 0)
			return -EINVAL;
		*attrs &= ~(1 << OVS_KEY_ATTR_TUNNEL);
	}

	if (*attrs & (1 << OVS_KEY_ATTR_CT_STATE) &&
	    ovs_ct_verify(net, OVS_KEY_ATTR_CT_STATE)) {
		u32 ct_state = nla_get_u32(a[OVS_KEY_ATTR_CT_STATE]);

		if (ct_state & ~CT_SUPPORTED_MASK) {
			OVS_NLERR(log, "ct_state flags %08x unsupported",
				  ct_state);
			return -EINVAL;
		}

		SW_FLOW_KEY_PUT(match, ct_state, ct_state, is_mask);
		*attrs &= ~(1ULL << OVS_KEY_ATTR_CT_STATE);
	}
	if (*attrs & (1 << OVS_KEY_ATTR_CT_ZONE) &&
	    ovs_ct_verify(net, OVS_KEY_ATTR_CT_ZONE)) {
		u16 ct_zone = nla_get_u16(a[OVS_KEY_ATTR_CT_ZONE]);

		SW_FLOW_KEY_PUT(match, ct_zone, ct_zone, is_mask);
		*attrs &= ~(1ULL << OVS_KEY_ATTR_CT_ZONE);
	}
	if (*attrs & (1 << OVS_KEY_ATTR_CT_MARK) &&
	    ovs_ct_verify(net, OVS_KEY_ATTR_CT_MARK)) {
		u32 mark = nla_get_u32(a[OVS_KEY_ATTR_CT_MARK]);

		SW_FLOW_KEY_PUT(match, ct.mark, mark, is_mask);
		*attrs &= ~(1ULL << OVS_KEY_ATTR_CT_MARK);
	}
	if (*attrs & (1 << OVS_KEY_ATTR_CT_LABELS) &&
	    ovs_ct_verify(net, OVS_KEY_ATTR_CT_LABELS)) {
		const struct ovs_key_ct_labels *cl;

		cl = nla_data(a[OVS_KEY_ATTR_CT_LABELS]);
		SW_FLOW_KEY_MEMCPY(match, ct.labels, cl->ct_labels,
				   sizeof(*cl), is_mask);
		*attrs &= ~(1ULL << OVS_KEY_ATTR_CT_LABELS);
	}
	if (*attrs & (1ULL << OVS_KEY_ATTR_CT_ORIG_TUPLE_IPV4)) {
		const struct ovs_key_ct_tuple_ipv4 *ct;

		ct = nla_data(a[OVS_KEY_ATTR_CT_ORIG_TUPLE_IPV4]);

		SW_FLOW_KEY_PUT(match, ipv4.ct_orig.src, ct->ipv4_src, is_mask);
		SW_FLOW_KEY_PUT(match, ipv4.ct_orig.dst, ct->ipv4_dst, is_mask);
		SW_FLOW_KEY_PUT(match, ct.orig_tp.src, ct->src_port, is_mask);
		SW_FLOW_KEY_PUT(match, ct.orig_tp.dst, ct->dst_port, is_mask);
		SW_FLOW_KEY_PUT(match, ct_orig_proto, ct->ipv4_proto, is_mask);
		*attrs &= ~(1ULL << OVS_KEY_ATTR_CT_ORIG_TUPLE_IPV4);
	}
	if (*attrs & (1ULL << OVS_KEY_ATTR_CT_ORIG_TUPLE_IPV6)) {
		const struct ovs_key_ct_tuple_ipv6 *ct;

		ct = nla_data(a[OVS_KEY_ATTR_CT_ORIG_TUPLE_IPV6]);

		SW_FLOW_KEY_MEMCPY(match, ipv6.ct_orig.src, &ct->ipv6_src,
				   sizeof(match->key->ipv6.ct_orig.src),
				   is_mask);
		SW_FLOW_KEY_MEMCPY(match, ipv6.ct_orig.dst, &ct->ipv6_dst,
				   sizeof(match->key->ipv6.ct_orig.dst),
				   is_mask);
		SW_FLOW_KEY_PUT(match, ct.orig_tp.src, ct->src_port, is_mask);
		SW_FLOW_KEY_PUT(match, ct.orig_tp.dst, ct->dst_port, is_mask);
		SW_FLOW_KEY_PUT(match, ct_orig_proto, ct->ipv6_proto, is_mask);
		*attrs &= ~(1ULL << OVS_KEY_ATTR_CT_ORIG_TUPLE_IPV6);
	}

	/* For layer 3 packets the Ethernet type is provided
	 * and treated as metadata but no MAC addresses are provided.
	 */
	if (!(*attrs & (1ULL << OVS_KEY_ATTR_ETHERNET)) &&
	    (*attrs & (1ULL << OVS_KEY_ATTR_ETHERTYPE)))
		mac_proto = MAC_PROTO_NONE;

	/* Always exact match mac_proto */
	SW_FLOW_KEY_PUT(match, mac_proto, is_mask ? 0xff : mac_proto, is_mask);

	if (mac_proto == MAC_PROTO_NONE)
		return parse_eth_type_from_nlattrs(match, attrs, a, is_mask,
						   log);

	return 0;
}

int nsh_hdr_from_nlattr(const struct nlattr *attr,
			struct nshhdr *nh, size_t size)
{
	struct nlattr *a;
	int rem;
	u8 flags = 0;
	u8 ttl = 0;
	int mdlen = 0;

	/* validate_nsh has check this, so we needn't do duplicate check here
	 */
	if (size < NSH_BASE_HDR_LEN)
		return -ENOBUFS;

	nla_for_each_nested(a, attr, rem) {
		int type = nla_type(a);

		switch (type) {
		case OVS_NSH_KEY_ATTR_BASE: {
			const struct ovs_nsh_key_base *base = nla_data(a);

			flags = base->flags;
			ttl = base->ttl;
			nh->np = base->np;
			nh->mdtype = base->mdtype;
			nh->path_hdr = base->path_hdr;
			break;
		}
		case OVS_NSH_KEY_ATTR_MD1:
			mdlen = nla_len(a);
			if (mdlen > size - NSH_BASE_HDR_LEN)
				return -ENOBUFS;
			memcpy(&nh->md1, nla_data(a), mdlen);
			break;

		case OVS_NSH_KEY_ATTR_MD2:
			mdlen = nla_len(a);
			if (mdlen > size - NSH_BASE_HDR_LEN)
				return -ENOBUFS;
			memcpy(&nh->md2, nla_data(a), mdlen);
			break;

		default:
			return -EINVAL;
		}
	}

	/* nsh header length  = NSH_BASE_HDR_LEN + mdlen */
	nh->ver_flags_ttl_len = 0;
	nsh_set_flags_ttl_len(nh, flags, ttl, NSH_BASE_HDR_LEN + mdlen);

	return 0;
}

int nsh_key_from_nlattr(const struct nlattr *attr,
			struct ovs_key_nsh *nsh, struct ovs_key_nsh *nsh_mask)
{
	struct nlattr *a;
	int rem;

	/* validate_nsh has check this, so we needn't do duplicate check here
	 */
	nla_for_each_nested(a, attr, rem) {
		int type = nla_type(a);

		switch (type) {
		case OVS_NSH_KEY_ATTR_BASE: {
			const struct ovs_nsh_key_base *base = nla_data(a);
			const struct ovs_nsh_key_base *base_mask = base + 1;

			nsh->base = *base;
			nsh_mask->base = *base_mask;
			break;
		}
		case OVS_NSH_KEY_ATTR_MD1: {
			const struct ovs_nsh_key_md1 *md1 = nla_data(a);
			const struct ovs_nsh_key_md1 *md1_mask = md1 + 1;

			memcpy(nsh->context, md1->context, sizeof(*md1));
			memcpy(nsh_mask->context, md1_mask->context,
			       sizeof(*md1_mask));
			break;
		}
		case OVS_NSH_KEY_ATTR_MD2:
			/* Not supported yet */
			return -ENOTSUPP;
		default:
			return -EINVAL;
		}
	}

	return 0;
}

static int nsh_key_put_from_nlattr(const struct nlattr *attr,
				   struct sw_flow_match *match, bool is_mask,
				   bool is_push_nsh, bool log)
{
	struct nlattr *a;
	int rem;
	bool has_base = false;
	bool has_md1 = false;
	bool has_md2 = false;
	u8 mdtype = 0;
	int mdlen = 0;

	if (WARN_ON(is_push_nsh && is_mask))
		return -EINVAL;

	nla_for_each_nested(a, attr, rem) {
		int type = nla_type(a);
		int i;

		if (type > OVS_NSH_KEY_ATTR_MAX) {
			OVS_NLERR(log, "nsh attr %d is out of range max %d",
				  type, OVS_NSH_KEY_ATTR_MAX);
			return -EINVAL;
		}

		if (!check_attr_len(nla_len(a),
				    ovs_nsh_key_attr_lens[type].len)) {
			OVS_NLERR(
			    log,
			    "nsh attr %d has unexpected len %d expected %d",
			    type,
			    nla_len(a),
			    ovs_nsh_key_attr_lens[type].len
			);
			return -EINVAL;
		}

		switch (type) {
		case OVS_NSH_KEY_ATTR_BASE: {
			const struct ovs_nsh_key_base *base = nla_data(a);

			has_base = true;
			mdtype = base->mdtype;
			SW_FLOW_KEY_PUT(match, nsh.base.flags,
					base->flags, is_mask);
			SW_FLOW_KEY_PUT(match, nsh.base.ttl,
					base->ttl, is_mask);
			SW_FLOW_KEY_PUT(match, nsh.base.mdtype,
					base->mdtype, is_mask);
			SW_FLOW_KEY_PUT(match, nsh.base.np,
					base->np, is_mask);
			SW_FLOW_KEY_PUT(match, nsh.base.path_hdr,
					base->path_hdr, is_mask);
			break;
		}
		case OVS_NSH_KEY_ATTR_MD1: {
			const struct ovs_nsh_key_md1 *md1 = nla_data(a);

			has_md1 = true;
			for (i = 0; i < NSH_MD1_CONTEXT_SIZE; i++)
				SW_FLOW_KEY_PUT(match, nsh.context[i],
						md1->context[i], is_mask);
			break;
		}
		case OVS_NSH_KEY_ATTR_MD2:
			if (!is_push_nsh) /* Not supported MD type 2 yet */
				return -ENOTSUPP;

			has_md2 = true;
			mdlen = nla_len(a);
			if (mdlen > NSH_CTX_HDRS_MAX_LEN || mdlen <= 0) {
				OVS_NLERR(
				    log,
				    "Invalid MD length %d for MD type %d",
				    mdlen,
				    mdtype
				);
				return -EINVAL;
			}
			break;
		default:
			OVS_NLERR(log, "Unknown nsh attribute %d",
				  type);
			return -EINVAL;
		}
	}

	if (rem > 0) {
		OVS_NLERR(log, "nsh attribute has %d unknown bytes.", rem);
		return -EINVAL;
	}

	if (has_md1 && has_md2) {
		OVS_NLERR(
		    1,
		    "invalid nsh attribute: md1 and md2 are exclusive."
		);
		return -EINVAL;
	}

	if (!is_mask) {
		if ((has_md1 && mdtype != NSH_M_TYPE1) ||
		    (has_md2 && mdtype != NSH_M_TYPE2)) {
			OVS_NLERR(1, "nsh attribute has unmatched MD type %d.",
				  mdtype);
			return -EINVAL;
		}

		if (is_push_nsh &&
		    (!has_base || (!has_md1 && !has_md2))) {
			OVS_NLERR(
			    1,
			    "push_nsh: missing base or metadata attributes"
			);
			return -EINVAL;
		}
	}

	return 0;
}

static int ovs_key_from_nlattrs(struct net *net, struct sw_flow_match *match,
				u64 attrs, const struct nlattr **a,
				bool is_mask, bool log)
{
	int err;

	err = metadata_from_nlattrs(net, match, &attrs, a, is_mask, log);
	if (err)
		return err;

	if (attrs & (1 << OVS_KEY_ATTR_ETHERNET)) {
		const struct ovs_key_ethernet *eth_key;

		eth_key = nla_data(a[OVS_KEY_ATTR_ETHERNET]);
		SW_FLOW_KEY_MEMCPY(match, eth.src,
				eth_key->eth_src, ETH_ALEN, is_mask);
		SW_FLOW_KEY_MEMCPY(match, eth.dst,
				eth_key->eth_dst, ETH_ALEN, is_mask);
		attrs &= ~(1 << OVS_KEY_ATTR_ETHERNET);

		if (attrs & (1 << OVS_KEY_ATTR_VLAN)) {
			/* VLAN attribute is always parsed before getting here since it
			 * may occur multiple times.
			 */
			OVS_NLERR(log, "VLAN attribute unexpected.");
			return -EINVAL;
		}

		if (attrs & (1 << OVS_KEY_ATTR_ETHERTYPE)) {
			err = parse_eth_type_from_nlattrs(match, &attrs, a, is_mask,
							  log);
			if (err)
				return err;
		} else if (!is_mask) {
			SW_FLOW_KEY_PUT(match, eth.type, htons(ETH_P_802_2), is_mask);
		}
	} else if (!match->key->eth.type) {
		OVS_NLERR(log, "Either Ethernet header or EtherType is required.");
		return -EINVAL;
	}

	if (attrs & (1 << OVS_KEY_ATTR_IPV4)) {
		const struct ovs_key_ipv4 *ipv4_key;

		ipv4_key = nla_data(a[OVS_KEY_ATTR_IPV4]);
		if (!is_mask && ipv4_key->ipv4_frag > OVS_FRAG_TYPE_MAX) {
			OVS_NLERR(log, "IPv4 frag type %d is out of range max %d",
				  ipv4_key->ipv4_frag, OVS_FRAG_TYPE_MAX);
			return -EINVAL;
		}
		SW_FLOW_KEY_PUT(match, ip.proto,
				ipv4_key->ipv4_proto, is_mask);
		SW_FLOW_KEY_PUT(match, ip.tos,
				ipv4_key->ipv4_tos, is_mask);
		SW_FLOW_KEY_PUT(match, ip.ttl,
				ipv4_key->ipv4_ttl, is_mask);
		SW_FLOW_KEY_PUT(match, ip.frag,
				ipv4_key->ipv4_frag, is_mask);
		SW_FLOW_KEY_PUT(match, ipv4.addr.src,
				ipv4_key->ipv4_src, is_mask);
		SW_FLOW_KEY_PUT(match, ipv4.addr.dst,
				ipv4_key->ipv4_dst, is_mask);
		attrs &= ~(1 << OVS_KEY_ATTR_IPV4);
	}

	if (attrs & (1 << OVS_KEY_ATTR_IPV6)) {
		const struct ovs_key_ipv6 *ipv6_key;

		ipv6_key = nla_data(a[OVS_KEY_ATTR_IPV6]);
		if (!is_mask && ipv6_key->ipv6_frag > OVS_FRAG_TYPE_MAX) {
			OVS_NLERR(log, "IPv6 frag type %d is out of range max %d",
				  ipv6_key->ipv6_frag, OVS_FRAG_TYPE_MAX);
			return -EINVAL;
		}

		if (!is_mask && ipv6_key->ipv6_label & htonl(0xFFF00000)) {
			OVS_NLERR(log, "IPv6 flow label %x is out of range (max=%x)",
				  ntohl(ipv6_key->ipv6_label), (1 << 20) - 1);
			return -EINVAL;
		}

		SW_FLOW_KEY_PUT(match, ipv6.label,
				ipv6_key->ipv6_label, is_mask);
		SW_FLOW_KEY_PUT(match, ip.proto,
				ipv6_key->ipv6_proto, is_mask);
		SW_FLOW_KEY_PUT(match, ip.tos,
				ipv6_key->ipv6_tclass, is_mask);
		SW_FLOW_KEY_PUT(match, ip.ttl,
				ipv6_key->ipv6_hlimit, is_mask);
		SW_FLOW_KEY_PUT(match, ip.frag,
				ipv6_key->ipv6_frag, is_mask);
		SW_FLOW_KEY_MEMCPY(match, ipv6.addr.src,
				ipv6_key->ipv6_src,
				sizeof(match->key->ipv6.addr.src),
				is_mask);
		SW_FLOW_KEY_MEMCPY(match, ipv6.addr.dst,
				ipv6_key->ipv6_dst,
				sizeof(match->key->ipv6.addr.dst),
				is_mask);

		attrs &= ~(1 << OVS_KEY_ATTR_IPV6);
	}

	if (attrs & (1 << OVS_KEY_ATTR_ARP)) {
		const struct ovs_key_arp *arp_key;

		arp_key = nla_data(a[OVS_KEY_ATTR_ARP]);
		if (!is_mask && (arp_key->arp_op & htons(0xff00))) {
			OVS_NLERR(log, "Unknown ARP opcode (opcode=%d).",
				  arp_key->arp_op);
			return -EINVAL;
		}

		SW_FLOW_KEY_PUT(match, ipv4.addr.src,
				arp_key->arp_sip, is_mask);
		SW_FLOW_KEY_PUT(match, ipv4.addr.dst,
			arp_key->arp_tip, is_mask);
		SW_FLOW_KEY_PUT(match, ip.proto,
				ntohs(arp_key->arp_op), is_mask);
		SW_FLOW_KEY_MEMCPY(match, ipv4.arp.sha,
				arp_key->arp_sha, ETH_ALEN, is_mask);
		SW_FLOW_KEY_MEMCPY(match, ipv4.arp.tha,
				arp_key->arp_tha, ETH_ALEN, is_mask);

		attrs &= ~(1 << OVS_KEY_ATTR_ARP);
	}

	if (attrs & (1 << OVS_KEY_ATTR_NSH)) {
		if (nsh_key_put_from_nlattr(a[OVS_KEY_ATTR_NSH], match,
					    is_mask, false, log) < 0)
			return -EINVAL;
		attrs &= ~(1 << OVS_KEY_ATTR_NSH);
	}

	if (attrs & (1 << OVS_KEY_ATTR_MPLS)) {
		const struct ovs_key_mpls *mpls_key;
		u32 hdr_len;
		u32 label_count, label_count_mask, i;

		mpls_key = nla_data(a[OVS_KEY_ATTR_MPLS]);
		hdr_len = nla_len(a[OVS_KEY_ATTR_MPLS]);
		label_count = hdr_len / sizeof(struct ovs_key_mpls);

		if (label_count == 0 || label_count > MPLS_LABEL_DEPTH ||
		    hdr_len % sizeof(struct ovs_key_mpls))
			return -EINVAL;

		label_count_mask =  GENMASK(label_count - 1, 0);

		for (i = 0 ; i < label_count; i++)
			SW_FLOW_KEY_PUT(match, mpls.lse[i],
					mpls_key[i].mpls_lse, is_mask);

		SW_FLOW_KEY_PUT(match, mpls.num_labels_mask,
				label_count_mask, is_mask);

		attrs &= ~(1 << OVS_KEY_ATTR_MPLS);
	 }

	if (attrs & (1 << OVS_KEY_ATTR_TCP)) {
		const struct ovs_key_tcp *tcp_key;

		tcp_key = nla_data(a[OVS_KEY_ATTR_TCP]);
		SW_FLOW_KEY_PUT(match, tp.src, tcp_key->tcp_src, is_mask);
		SW_FLOW_KEY_PUT(match, tp.dst, tcp_key->tcp_dst, is_mask);
		attrs &= ~(1 << OVS_KEY_ATTR_TCP);
	}

	if (attrs & (1 << OVS_KEY_ATTR_TCP_FLAGS)) {
		SW_FLOW_KEY_PUT(match, tp.flags,
				nla_get_be16(a[OVS_KEY_ATTR_TCP_FLAGS]),
				is_mask);
		attrs &= ~(1 << OVS_KEY_ATTR_TCP_FLAGS);
	}

	if (attrs & (1 << OVS_KEY_ATTR_UDP)) {
		const struct ovs_key_udp *udp_key;

		udp_key = nla_data(a[OVS_KEY_ATTR_UDP]);
		SW_FLOW_KEY_PUT(match, tp.src, udp_key->udp_src, is_mask);
		SW_FLOW_KEY_PUT(match, tp.dst, udp_key->udp_dst, is_mask);
		attrs &= ~(1 << OVS_KEY_ATTR_UDP);
	}

	if (attrs & (1 << OVS_KEY_ATTR_SCTP)) {
		const struct ovs_key_sctp *sctp_key;

		sctp_key = nla_data(a[OVS_KEY_ATTR_SCTP]);
		SW_FLOW_KEY_PUT(match, tp.src, sctp_key->sctp_src, is_mask);
		SW_FLOW_KEY_PUT(match, tp.dst, sctp_key->sctp_dst, is_mask);
		attrs &= ~(1 << OVS_KEY_ATTR_SCTP);
	}

	if (attrs & (1 << OVS_KEY_ATTR_ICMP)) {
		const struct ovs_key_icmp *icmp_key;

		icmp_key = nla_data(a[OVS_KEY_ATTR_ICMP]);
		SW_FLOW_KEY_PUT(match, tp.src,
				htons(icmp_key->icmp_type), is_mask);
		SW_FLOW_KEY_PUT(match, tp.dst,
				htons(icmp_key->icmp_code), is_mask);
		attrs &= ~(1 << OVS_KEY_ATTR_ICMP);
	}

	if (attrs & (1 << OVS_KEY_ATTR_ICMPV6)) {
		const struct ovs_key_icmpv6 *icmpv6_key;

		icmpv6_key = nla_data(a[OVS_KEY_ATTR_ICMPV6]);
		SW_FLOW_KEY_PUT(match, tp.src,
				htons(icmpv6_key->icmpv6_type), is_mask);
		SW_FLOW_KEY_PUT(match, tp.dst,
				htons(icmpv6_key->icmpv6_code), is_mask);
		attrs &= ~(1 << OVS_KEY_ATTR_ICMPV6);
	}

	if (attrs & (1 << OVS_KEY_ATTR_ND)) {
		const struct ovs_key_nd *nd_key;

		nd_key = nla_data(a[OVS_KEY_ATTR_ND]);
		SW_FLOW_KEY_MEMCPY(match, ipv6.nd.target,
			nd_key->nd_target,
			sizeof(match->key->ipv6.nd.target),
			is_mask);
		SW_FLOW_KEY_MEMCPY(match, ipv6.nd.sll,
			nd_key->nd_sll, ETH_ALEN, is_mask);
		SW_FLOW_KEY_MEMCPY(match, ipv6.nd.tll,
				nd_key->nd_tll, ETH_ALEN, is_mask);
		attrs &= ~(1 << OVS_KEY_ATTR_ND);
	}

	if (attrs != 0) {
		OVS_NLERR(log, "Unknown key attributes %llx",
			  (unsigned long long)attrs);
		return -EINVAL;
	}

	return 0;
}

static void nlattr_set(struct nlattr *attr, u8 val,
		       const struct ovs_len_tbl *tbl)
{
	struct nlattr *nla;
	int rem;

	/* The nlattr stream should already have been validated */
	nla_for_each_nested(nla, attr, rem) {
		if (tbl[nla_type(nla)].len == OVS_ATTR_NESTED)
			nlattr_set(nla, val, tbl[nla_type(nla)].next ? : tbl);
		else
			memset(nla_data(nla), val, nla_len(nla));

		if (nla_type(nla) == OVS_KEY_ATTR_CT_STATE)
			*(u32 *)nla_data(nla) &= CT_SUPPORTED_MASK;
	}
}

static void mask_set_nlattr(struct nlattr *attr, u8 val)
{
	nlattr_set(attr, val, ovs_key_lens);
}

/**
 * ovs_nla_get_match - parses Netlink attributes into a flow key and
 * mask. In case the 'mask' is NULL, the flow is treated as exact match
 * flow. Otherwise, it is treated as a wildcarded flow, except the mask
 * does not include any don't care bit.
 * @net: Used to determine per-namespace field support.
 * @match: receives the extracted flow match information.
 * @nla_key: Netlink attribute holding nested %OVS_KEY_ATTR_* Netlink attribute
 * sequence. The fields should of the packet that triggered the creation
 * of this flow.
 * @nla_mask: Optional. Netlink attribute holding nested %OVS_KEY_ATTR_*
 * Netlink attribute specifies the mask field of the wildcarded flow.
 * @log: Boolean to allow kernel error logging.  Normally true, but when
 * probing for feature compatibility this should be passed in as false to
 * suppress unnecessary error logging.
 */
int ovs_nla_get_match(struct net *net, struct sw_flow_match *match,
		      const struct nlattr *nla_key,
		      const struct nlattr *nla_mask,
		      bool log)
{
	const struct nlattr *a[OVS_KEY_ATTR_MAX + 1];
	struct nlattr *newmask = NULL;
	u64 key_attrs = 0;
	u64 mask_attrs = 0;
	int err;

	err = parse_flow_nlattrs(nla_key, a, &key_attrs, log);
	if (err)
		return err;

	err = parse_vlan_from_nlattrs(match, &key_attrs, a, false, log);
	if (err)
		return err;

	err = ovs_key_from_nlattrs(net, match, key_attrs, a, false, log);
	if (err)
		return err;

	if (match->mask) {
		if (!nla_mask) {
			/* Create an exact match mask. We need to set to 0xff
			 * all the 'match->mask' fields that have been touched
			 * in 'match->key'. We cannot simply memset
			 * 'match->mask', because padding bytes and fields not
			 * specified in 'match->key' should be left to 0.
			 * Instead, we use a stream of netlink attributes,
			 * copied from 'key' and set to 0xff.
			 * ovs_key_from_nlattrs() will take care of filling
			 * 'match->mask' appropriately.
			 */
			newmask = kmemdup(nla_key,
					  nla_total_size(nla_len(nla_key)),
					  GFP_KERNEL);
			if (!newmask)
				return -ENOMEM;

			mask_set_nlattr(newmask, 0xff);

			/* The userspace does not send tunnel attributes that
			 * are 0, but we should not wildcard them nonetheless.
			 */
			if (match->key->tun_proto)
				SW_FLOW_KEY_MEMSET_FIELD(match, tun_key,
							 0xff, true);

			nla_mask = newmask;
		}

		err = parse_flow_mask_nlattrs(nla_mask, a, &mask_attrs, log);
		if (err)
			goto free_newmask;

		/* Always match on tci. */
		SW_FLOW_KEY_PUT(match, eth.vlan.tci, htons(0xffff), true);
		SW_FLOW_KEY_PUT(match, eth.cvlan.tci, htons(0xffff), true);

		err = parse_vlan_from_nlattrs(match, &mask_attrs, a, true, log);
		if (err)
			goto free_newmask;

		err = ovs_key_from_nlattrs(net, match, mask_attrs, a, true,
					   log);
		if (err)
			goto free_newmask;
	}

	if (!match_validate(match, key_attrs, mask_attrs, log))
		err = -EINVAL;

free_newmask:
	kfree(newmask);
	return err;
}

static size_t get_ufid_len(const struct nlattr *attr, bool log)
{
	size_t len;

	if (!attr)
		return 0;

	len = nla_len(attr);
	if (len < 1 || len > MAX_UFID_LENGTH) {
		OVS_NLERR(log, "ufid size %u bytes exceeds the range (1, %d)",
			  nla_len(attr), MAX_UFID_LENGTH);
		return 0;
	}

	return len;
}

/* Initializes 'flow->ufid', returning true if 'attr' contains a valid UFID,
 * or false otherwise.
 */
bool ovs_nla_get_ufid(struct sw_flow_id *sfid, const struct nlattr *attr,
		      bool log)
{
	sfid->ufid_len = get_ufid_len(attr, log);
	if (sfid->ufid_len)
		memcpy(sfid->ufid, nla_data(attr), sfid->ufid_len);

	return sfid->ufid_len;
}

int ovs_nla_get_identifier(struct sw_flow_id *sfid, const struct nlattr *ufid,
			   const struct sw_flow_key *key, bool log)
{
	struct sw_flow_key *new_key;

	if (ovs_nla_get_ufid(sfid, ufid, log))
		return 0;

	/* If UFID was not provided, use unmasked key. */
	new_key = kmalloc(sizeof(*new_key), GFP_KERNEL);
	if (!new_key)
		return -ENOMEM;
	memcpy(new_key, key, sizeof(*key));
	sfid->unmasked_key = new_key;

	return 0;
}

u32 ovs_nla_get_ufid_flags(const struct nlattr *attr)
{
	return attr ? nla_get_u32(attr) : 0;
}

/**
 * ovs_nla_get_flow_metadata - parses Netlink attributes into a flow key.
 * @net: Network namespace.
 * @key: Receives extracted in_port, priority, tun_key, skb_mark and conntrack
 * metadata.
 * @a: Array of netlink attributes holding parsed %OVS_KEY_ATTR_* Netlink
 * attributes.
 * @attrs: Bit mask for the netlink attributes included in @a.
 * @log: Boolean to allow kernel error logging.  Normally true, but when
 * probing for feature compatibility this should be passed in as false to
 * suppress unnecessary error logging.
 *
 * This parses a series of Netlink attributes that form a flow key, which must
 * take the same form accepted by flow_from_nlattrs(), but only enough of it to
 * get the metadata, that is, the parts of the flow key that cannot be
 * extracted from the packet itself.
 *
 * This must be called before the packet key fields are filled in 'key'.
 */

int ovs_nla_get_flow_metadata(struct net *net,
			      const struct nlattr *a[OVS_KEY_ATTR_MAX + 1],
			      u64 attrs, struct sw_flow_key *key, bool log)
{
	struct sw_flow_match match;

	memset(&match, 0, sizeof(match));
	match.key = key;

	key->ct_state = 0;
	key->ct_zone = 0;
	key->ct_orig_proto = 0;
	memset(&key->ct, 0, sizeof(key->ct));
	memset(&key->ipv4.ct_orig, 0, sizeof(key->ipv4.ct_orig));
	memset(&key->ipv6.ct_orig, 0, sizeof(key->ipv6.ct_orig));

	key->phy.in_port = DP_MAX_PORTS;

	return metadata_from_nlattrs(net, &match, &attrs, a, false, log);
}

static int ovs_nla_put_vlan(struct sk_buff *skb, const struct vlan_head *vh,
			    bool is_mask)
{
	__be16 eth_type = !is_mask ? vh->tpid : htons(0xffff);

	if (nla_put_be16(skb, OVS_KEY_ATTR_ETHERTYPE, eth_type) ||
	    nla_put_be16(skb, OVS_KEY_ATTR_VLAN, vh->tci))
		return -EMSGSIZE;
	return 0;
}

static int nsh_key_to_nlattr(const struct ovs_key_nsh *nsh, bool is_mask,
			     struct sk_buff *skb)
{
	struct nlattr *start;

	start = nla_nest_start_noflag(skb, OVS_KEY_ATTR_NSH);
	if (!start)
		return -EMSGSIZE;

	if (nla_put(skb, OVS_NSH_KEY_ATTR_BASE, sizeof(nsh->base), &nsh->base))
		goto nla_put_failure;

	if (is_mask || nsh->base.mdtype == NSH_M_TYPE1) {
		if (nla_put(skb, OVS_NSH_KEY_ATTR_MD1,
			    sizeof(nsh->context), nsh->context))
			goto nla_put_failure;
	}

	/* Don't support MD type 2 yet */

	nla_nest_end(skb, start);

	return 0;

nla_put_failure:
	return -EMSGSIZE;
}

static int __ovs_nla_put_key(const struct sw_flow_key *swkey,
			     const struct sw_flow_key *output, bool is_mask,
			     struct sk_buff *skb)
{
	struct ovs_key_ethernet *eth_key;
	struct nlattr *nla;
	struct nlattr *encap = NULL;
	struct nlattr *in_encap = NULL;

	if (nla_put_u32(skb, OVS_KEY_ATTR_RECIRC_ID, output->recirc_id))
		goto nla_put_failure;

	if (nla_put_u32(skb, OVS_KEY_ATTR_DP_HASH, output->ovs_flow_hash))
		goto nla_put_failure;

	if (nla_put_u32(skb, OVS_KEY_ATTR_PRIORITY, output->phy.priority))
		goto nla_put_failure;

	if ((swkey->tun_proto || is_mask)) {
		const void *opts = NULL;

		if (output->tun_key.tun_flags & TUNNEL_OPTIONS_PRESENT)
			opts = TUN_METADATA_OPTS(output, swkey->tun_opts_len);

		if (ip_tun_to_nlattr(skb, &output->tun_key, opts,
				     swkey->tun_opts_len, swkey->tun_proto, 0))
			goto nla_put_failure;
	}

	if (swkey->phy.in_port == DP_MAX_PORTS) {
		if (is_mask && (output->phy.in_port == 0xffff))
			if (nla_put_u32(skb, OVS_KEY_ATTR_IN_PORT, 0xffffffff))
				goto nla_put_failure;
	} else {
		u16 upper_u16;
		upper_u16 = !is_mask ? 0 : 0xffff;

		if (nla_put_u32(skb, OVS_KEY_ATTR_IN_PORT,
				(upper_u16 << 16) | output->phy.in_port))
			goto nla_put_failure;
	}

	if (nla_put_u32(skb, OVS_KEY_ATTR_SKB_MARK, output->phy.skb_mark))
		goto nla_put_failure;

	if (ovs_ct_put_key(swkey, output, skb))
		goto nla_put_failure;

	if (ovs_key_mac_proto(swkey) == MAC_PROTO_ETHERNET) {
		nla = nla_reserve(skb, OVS_KEY_ATTR_ETHERNET, sizeof(*eth_key));
		if (!nla)
			goto nla_put_failure;

		eth_key = nla_data(nla);
		ether_addr_copy(eth_key->eth_src, output->eth.src);
		ether_addr_copy(eth_key->eth_dst, output->eth.dst);

		if (swkey->eth.vlan.tci || eth_type_vlan(swkey->eth.type)) {
			if (ovs_nla_put_vlan(skb, &output->eth.vlan, is_mask))
				goto nla_put_failure;
			encap = nla_nest_start_noflag(skb, OVS_KEY_ATTR_ENCAP);
			if (!swkey->eth.vlan.tci)
				goto unencap;

			if (swkey->eth.cvlan.tci || eth_type_vlan(swkey->eth.type)) {
				if (ovs_nla_put_vlan(skb, &output->eth.cvlan, is_mask))
					goto nla_put_failure;
				in_encap = nla_nest_start_noflag(skb,
								 OVS_KEY_ATTR_ENCAP);
				if (!swkey->eth.cvlan.tci)
					goto unencap;
			}
		}

		if (swkey->eth.type == htons(ETH_P_802_2)) {
			/*
			* Ethertype 802.2 is represented in the netlink with omitted
			* OVS_KEY_ATTR_ETHERTYPE in the flow key attribute, and
			* 0xffff in the mask attribute.  Ethertype can also
			* be wildcarded.
			*/
			if (is_mask && output->eth.type)
				if (nla_put_be16(skb, OVS_KEY_ATTR_ETHERTYPE,
							output->eth.type))
					goto nla_put_failure;
			goto unencap;
		}
	}

	if (nla_put_be16(skb, OVS_KEY_ATTR_ETHERTYPE, output->eth.type))
		goto nla_put_failure;

	if (eth_type_vlan(swkey->eth.type)) {
		/* There are 3 VLAN tags, we don't know anything about the rest
		 * of the packet, so truncate here.
		 */
		WARN_ON_ONCE(!(encap && in_encap));
		goto unencap;
	}

	if (swkey->eth.type == htons(ETH_P_IP)) {
		struct ovs_key_ipv4 *ipv4_key;

		nla = nla_reserve(skb, OVS_KEY_ATTR_IPV4, sizeof(*ipv4_key));
		if (!nla)
			goto nla_put_failure;
		ipv4_key = nla_data(nla);
		ipv4_key->ipv4_src = output->ipv4.addr.src;
		ipv4_key->ipv4_dst = output->ipv4.addr.dst;
		ipv4_key->ipv4_proto = output->ip.proto;
		ipv4_key->ipv4_tos = output->ip.tos;
		ipv4_key->ipv4_ttl = output->ip.ttl;
		ipv4_key->ipv4_frag = output->ip.frag;
	} else if (swkey->eth.type == htons(ETH_P_IPV6)) {
		struct ovs_key_ipv6 *ipv6_key;

		nla = nla_reserve(skb, OVS_KEY_ATTR_IPV6, sizeof(*ipv6_key));
		if (!nla)
			goto nla_put_failure;
		ipv6_key = nla_data(nla);
		memcpy(ipv6_key->ipv6_src, &output->ipv6.addr.src,
				sizeof(ipv6_key->ipv6_src));
		memcpy(ipv6_key->ipv6_dst, &output->ipv6.addr.dst,
				sizeof(ipv6_key->ipv6_dst));
		ipv6_key->ipv6_label = output->ipv6.label;
		ipv6_key->ipv6_proto = output->ip.proto;
		ipv6_key->ipv6_tclass = output->ip.tos;
		ipv6_key->ipv6_hlimit = output->ip.ttl;
		ipv6_key->ipv6_frag = output->ip.frag;
	} else if (swkey->eth.type == htons(ETH_P_NSH)) {
		if (nsh_key_to_nlattr(&output->nsh, is_mask, skb))
			goto nla_put_failure;
	} else if (swkey->eth.type == htons(ETH_P_ARP) ||
		   swkey->eth.type == htons(ETH_P_RARP)) {
		struct ovs_key_arp *arp_key;

		nla = nla_reserve(skb, OVS_KEY_ATTR_ARP, sizeof(*arp_key));
		if (!nla)
			goto nla_put_failure;
		arp_key = nla_data(nla);
		memset(arp_key, 0, sizeof(struct ovs_key_arp));
		arp_key->arp_sip = output->ipv4.addr.src;
		arp_key->arp_tip = output->ipv4.addr.dst;
		arp_key->arp_op = htons(output->ip.proto);
		ether_addr_copy(arp_key->arp_sha, output->ipv4.arp.sha);
		ether_addr_copy(arp_key->arp_tha, output->ipv4.arp.tha);
	} else if (eth_p_mpls(swkey->eth.type)) {
		u8 i, num_labels;
		struct ovs_key_mpls *mpls_key;

		num_labels = hweight_long(output->mpls.num_labels_mask);
		nla = nla_reserve(skb, OVS_KEY_ATTR_MPLS,
				  num_labels * sizeof(*mpls_key));
		if (!nla)
			goto nla_put_failure;

		mpls_key = nla_data(nla);
		for (i = 0; i < num_labels; i++)
			mpls_key[i].mpls_lse = output->mpls.lse[i];
	}

	if ((swkey->eth.type == htons(ETH_P_IP) ||
	     swkey->eth.type == htons(ETH_P_IPV6)) &&
	     swkey->ip.frag != OVS_FRAG_TYPE_LATER) {

		if (swkey->ip.proto == IPPROTO_TCP) {
			struct ovs_key_tcp *tcp_key;

			nla = nla_reserve(skb, OVS_KEY_ATTR_TCP, sizeof(*tcp_key));
			if (!nla)
				goto nla_put_failure;
			tcp_key = nla_data(nla);
			tcp_key->tcp_src = output->tp.src;
			tcp_key->tcp_dst = output->tp.dst;
			if (nla_put_be16(skb, OVS_KEY_ATTR_TCP_FLAGS,
					 output->tp.flags))
				goto nla_put_failure;
		} else if (swkey->ip.proto == IPPROTO_UDP) {
			struct ovs_key_udp *udp_key;

			nla = nla_reserve(skb, OVS_KEY_ATTR_UDP, sizeof(*udp_key));
			if (!nla)
				goto nla_put_failure;
			udp_key = nla_data(nla);
			udp_key->udp_src = output->tp.src;
			udp_key->udp_dst = output->tp.dst;
		} else if (swkey->ip.proto == IPPROTO_SCTP) {
			struct ovs_key_sctp *sctp_key;

			nla = nla_reserve(skb, OVS_KEY_ATTR_SCTP, sizeof(*sctp_key));
			if (!nla)
				goto nla_put_failure;
			sctp_key = nla_data(nla);
			sctp_key->sctp_src = output->tp.src;
			sctp_key->sctp_dst = output->tp.dst;
		} else if (swkey->eth.type == htons(ETH_P_IP) &&
			   swkey->ip.proto == IPPROTO_ICMP) {
			struct ovs_key_icmp *icmp_key;

			nla = nla_reserve(skb, OVS_KEY_ATTR_ICMP, sizeof(*icmp_key));
			if (!nla)
				goto nla_put_failure;
			icmp_key = nla_data(nla);
			icmp_key->icmp_type = ntohs(output->tp.src);
			icmp_key->icmp_code = ntohs(output->tp.dst);
		} else if (swkey->eth.type == htons(ETH_P_IPV6) &&
			   swkey->ip.proto == IPPROTO_ICMPV6) {
			struct ovs_key_icmpv6 *icmpv6_key;

			nla = nla_reserve(skb, OVS_KEY_ATTR_ICMPV6,
						sizeof(*icmpv6_key));
			if (!nla)
				goto nla_put_failure;
			icmpv6_key = nla_data(nla);
			icmpv6_key->icmpv6_type = ntohs(output->tp.src);
			icmpv6_key->icmpv6_code = ntohs(output->tp.dst);

			if (icmpv6_key->icmpv6_type == NDISC_NEIGHBOUR_SOLICITATION ||
			    icmpv6_key->icmpv6_type == NDISC_NEIGHBOUR_ADVERTISEMENT) {
				struct ovs_key_nd *nd_key;

				nla = nla_reserve(skb, OVS_KEY_ATTR_ND, sizeof(*nd_key));
				if (!nla)
					goto nla_put_failure;
				nd_key = nla_data(nla);
				memcpy(nd_key->nd_target, &output->ipv6.nd.target,
							sizeof(nd_key->nd_target));
				ether_addr_copy(nd_key->nd_sll, output->ipv6.nd.sll);
				ether_addr_copy(nd_key->nd_tll, output->ipv6.nd.tll);
			}
		}
	}

unencap:
	if (in_encap)
		nla_nest_end(skb, in_encap);
	if (encap)
		nla_nest_end(skb, encap);

	return 0;

nla_put_failure:
	return -EMSGSIZE;
}

int ovs_nla_put_key(const struct sw_flow_key *swkey,
		    const struct sw_flow_key *output, int attr, bool is_mask,
		    struct sk_buff *skb)
{
	int err;
	struct nlattr *nla;

	nla = nla_nest_start_noflag(skb, attr);
	if (!nla)
		return -EMSGSIZE;
	err = __ovs_nla_put_key(swkey, output, is_mask, skb);
	if (err)
		return err;
	nla_nest_end(skb, nla);

	return 0;
}

/* Called with ovs_mutex or RCU read lock. */
int ovs_nla_put_identifier(const struct sw_flow *flow, struct sk_buff *skb)
{
	if (ovs_identifier_is_ufid(&flow->id))
		return nla_put(skb, OVS_FLOW_ATTR_UFID, flow->id.ufid_len,
			       flow->id.ufid);

	return ovs_nla_put_key(flow->id.unmasked_key, flow->id.unmasked_key,
			       OVS_FLOW_ATTR_KEY, false, skb);
}

/* Called with ovs_mutex or RCU read lock. */
int ovs_nla_put_masked_key(const struct sw_flow *flow, struct sk_buff *skb)
{
	return ovs_nla_put_key(&flow->key, &flow->key,
				OVS_FLOW_ATTR_KEY, false, skb);
}

/* Called with ovs_mutex or RCU read lock. */
int ovs_nla_put_mask(const struct sw_flow *flow, struct sk_buff *skb)
{
	return ovs_nla_put_key(&flow->key, &flow->mask->key,
				OVS_FLOW_ATTR_MASK, true, skb);
}

#define MAX_ACTIONS_BUFSIZE	(32 * 1024)

static struct sw_flow_actions *nla_alloc_flow_actions(int size)
{
	struct sw_flow_actions *sfa;

	WARN_ON_ONCE(size > MAX_ACTIONS_BUFSIZE);

	sfa = kmalloc(sizeof(*sfa) + size, GFP_KERNEL);
	if (!sfa)
		return ERR_PTR(-ENOMEM);

	sfa->actions_len = 0;
	return sfa;
}

static void ovs_nla_free_set_action(const struct nlattr *a)
{
	const struct nlattr *ovs_key = nla_data(a);
	struct ovs_tunnel_info *ovs_tun;

	switch (nla_type(ovs_key)) {
	case OVS_KEY_ATTR_TUNNEL_INFO:
		ovs_tun = nla_data(ovs_key);
		dst_release((struct dst_entry *)ovs_tun->tun_dst);
		break;
	}
}

void ovs_nla_free_flow_actions(struct sw_flow_actions *sf_acts)
{
	const struct nlattr *a;
	int rem;

	if (!sf_acts)
		return;

	nla_for_each_attr(a, sf_acts->actions, sf_acts->actions_len, rem) {
		switch (nla_type(a)) {
		case OVS_ACTION_ATTR_SET:
			ovs_nla_free_set_action(a);
			break;
		case OVS_ACTION_ATTR_CT:
			ovs_ct_free_action(a);
			break;
		}
	}

	kfree(sf_acts);
}

static void __ovs_nla_free_flow_actions(struct rcu_head *head)
{
	ovs_nla_free_flow_actions(container_of(head, struct sw_flow_actions, rcu));
}

/* Schedules 'sf_acts' to be freed after the next RCU grace period.
 * The caller must hold rcu_read_lock for this to be sensible. */
void ovs_nla_free_flow_actions_rcu(struct sw_flow_actions *sf_acts)
{
	call_rcu(&sf_acts->rcu, __ovs_nla_free_flow_actions);
}

static struct nlattr *reserve_sfa_size(struct sw_flow_actions **sfa,
				       int attr_len, bool log)
{

	struct sw_flow_actions *acts;
	int new_acts_size;
	size_t req_size = NLA_ALIGN(attr_len);
	int next_offset = offsetof(struct sw_flow_actions, actions) +
					(*sfa)->actions_len;

	if (req_size <= (ksize(*sfa) - next_offset))
		goto out;

	new_acts_size = max(next_offset + req_size, ksize(*sfa) * 2);

	if (new_acts_size > MAX_ACTIONS_BUFSIZE) {
		if ((MAX_ACTIONS_BUFSIZE - next_offset) < req_size) {
			OVS_NLERR(log, "Flow action size exceeds max %u",
				  MAX_ACTIONS_BUFSIZE);
			return ERR_PTR(-EMSGSIZE);
		}
		new_acts_size = MAX_ACTIONS_BUFSIZE;
	}

	acts = nla_alloc_flow_actions(new_acts_size);
	if (IS_ERR(acts))
		return (void *)acts;

	memcpy(acts->actions, (*sfa)->actions, (*sfa)->actions_len);
	acts->actions_len = (*sfa)->actions_len;
	acts->orig_len = (*sfa)->orig_len;
	kfree(*sfa);
	*sfa = acts;

out:
	(*sfa)->actions_len += req_size;
	return  (struct nlattr *) ((unsigned char *)(*sfa) + next_offset);
}

static struct nlattr *__add_action(struct sw_flow_actions **sfa,
				   int attrtype, void *data, int len, bool log)
{
	struct nlattr *a;

	a = reserve_sfa_size(sfa, nla_attr_size(len), log);
	if (IS_ERR(a))
		return a;

	a->nla_type = attrtype;
	a->nla_len = nla_attr_size(len);

	if (data)
		memcpy(nla_data(a), data, len);
	memset((unsigned char *) a + a->nla_len, 0, nla_padlen(len));

	return a;
}

int ovs_nla_add_action(struct sw_flow_actions **sfa, int attrtype, void *data,
		       int len, bool log)
{
	struct nlattr *a;

	a = __add_action(sfa, attrtype, data, len, log);

	return PTR_ERR_OR_ZERO(a);
}

static inline int add_nested_action_start(struct sw_flow_actions **sfa,
					  int attrtype, bool log)
{
	int used = (*sfa)->actions_len;
	int err;

	err = ovs_nla_add_action(sfa, attrtype, NULL, 0, log);
	if (err)
		return err;

	return used;
}

static inline void add_nested_action_end(struct sw_flow_actions *sfa,
					 int st_offset)
{
	struct nlattr *a = (struct nlattr *) ((unsigned char *)sfa->actions +
							       st_offset);

	a->nla_len = sfa->actions_len - st_offset;
}

static int __ovs_nla_copy_actions(struct net *net, const struct nlattr *attr,
				  const struct sw_flow_key *key,
				  struct sw_flow_actions **sfa,
				  __be16 eth_type, __be16 vlan_tci,
				  u32 mpls_label_count, bool log);

static int validate_and_copy_sample(struct net *net, const struct nlattr *attr,
				    const struct sw_flow_key *key,
				    struct sw_flow_actions **sfa,
				    __be16 eth_type, __be16 vlan_tci,
				    u32 mpls_label_count, bool log, bool last)
{
	const struct nlattr *attrs[OVS_SAMPLE_ATTR_MAX + 1];
	const struct nlattr *probability, *actions;
	const struct nlattr *a;
	int rem, start, err;
	struct sample_arg arg;

	memset(attrs, 0, sizeof(attrs));
	nla_for_each_nested(a, attr, rem) {
		int type = nla_type(a);
		if (!type || type > OVS_SAMPLE_ATTR_MAX || attrs[type])
			return -EINVAL;
		attrs[type] = a;
	}
	if (rem)
		return -EINVAL;

	probability = attrs[OVS_SAMPLE_ATTR_PROBABILITY];
	if (!probability || nla_len(probability) != sizeof(u32))
		return -EINVAL;

	actions = attrs[OVS_SAMPLE_ATTR_ACTIONS];
	if (!actions || (nla_len(actions) && nla_len(actions) < NLA_HDRLEN))
		return -EINVAL;

	/* validation done, copy sample action. */
	start = add_nested_action_start(sfa, OVS_ACTION_ATTR_SAMPLE, log);
	if (start < 0)
		return start;

	/* When both skb and flow may be changed, put the sample
	 * into a deferred fifo. On the other hand, if only skb
	 * may be modified, the actions can be executed in place.
	 *
	 * Do this analysis at the flow installation time.
	 * Set 'clone_action->exec' to true if the actions can be
	 * executed without being deferred.
	 *
	 * If the sample is the last action, it can always be excuted
	 * rather than deferred.
	 */
	arg.exec = last || !actions_may_change_flow(actions);
	arg.probability = nla_get_u32(probability);

	err = ovs_nla_add_action(sfa, OVS_SAMPLE_ATTR_ARG, &arg, sizeof(arg),
				 log);
	if (err)
		return err;

	err = __ovs_nla_copy_actions(net, actions, key, sfa,
				     eth_type, vlan_tci, mpls_label_count, log);

	if (err)
		return err;

	add_nested_action_end(*sfa, start);

	return 0;
}

static int validate_and_copy_dec_ttl(struct net *net,
				     const struct nlattr *attr,
				     const struct sw_flow_key *key,
				     struct sw_flow_actions **sfa,
				     __be16 eth_type, __be16 vlan_tci,
				     u32 mpls_label_count, bool log)
{
	const struct nlattr *attrs[OVS_DEC_TTL_ATTR_MAX + 1];
	int start, action_start, err, rem;
	const struct nlattr *a, *actions;

	memset(attrs, 0, sizeof(attrs));
	nla_for_each_nested(a, attr, rem) {
		int type = nla_type(a);

		/* Ignore unknown attributes to be future proof. */
		if (type > OVS_DEC_TTL_ATTR_MAX)
			continue;

		if (!type || attrs[type]) {
			OVS_NLERR(log, "Duplicate or invalid key (type %d).",
				  type);
			return -EINVAL;
		}

		attrs[type] = a;
	}

	if (rem) {
		OVS_NLERR(log, "Message has %d unknown bytes.", rem);
		return -EINVAL;
	}

	actions = attrs[OVS_DEC_TTL_ATTR_ACTION];
	if (!actions || (nla_len(actions) && nla_len(actions) < NLA_HDRLEN)) {
		OVS_NLERR(log, "Missing valid actions attribute.");
		return -EINVAL;
	}

	start = add_nested_action_start(sfa, OVS_ACTION_ATTR_DEC_TTL, log);
	if (start < 0)
		return start;

	action_start = add_nested_action_start(sfa, OVS_DEC_TTL_ATTR_ACTION, log);
	if (action_start < 0)
		return action_start;

	err = __ovs_nla_copy_actions(net, actions, key, sfa, eth_type,
				     vlan_tci, mpls_label_count, log);
	if (err)
		return err;

	add_nested_action_end(*sfa, action_start);
	add_nested_action_end(*sfa, start);
	return 0;
}

static int validate_and_copy_clone(struct net *net,
				   const struct nlattr *attr,
				   const struct sw_flow_key *key,
				   struct sw_flow_actions **sfa,
				   __be16 eth_type, __be16 vlan_tci,
				   u32 mpls_label_count, bool log, bool last)
{
	int start, err;
	u32 exec;

	if (nla_len(attr) && nla_len(attr) < NLA_HDRLEN)
		return -EINVAL;

	start = add_nested_action_start(sfa, OVS_ACTION_ATTR_CLONE, log);
	if (start < 0)
		return start;

	exec = last || !actions_may_change_flow(attr);

	err = ovs_nla_add_action(sfa, OVS_CLONE_ATTR_EXEC, &exec,
				 sizeof(exec), log);
	if (err)
		return err;

	err = __ovs_nla_copy_actions(net, attr, key, sfa,
				     eth_type, vlan_tci, mpls_label_count, log);
	if (err)
		return err;

	add_nested_action_end(*sfa, start);

	return 0;
}

void ovs_match_init(struct sw_flow_match *match,
		    struct sw_flow_key *key,
		    bool reset_key,
		    struct sw_flow_mask *mask)
{
	memset(match, 0, sizeof(*match));
	match->key = key;
	match->mask = mask;

	if (reset_key)
		memset(key, 0, sizeof(*key));

	if (mask) {
		memset(&mask->key, 0, sizeof(mask->key));
		mask->range.start = mask->range.end = 0;
	}
}

static int validate_geneve_opts(struct sw_flow_key *key)
{
	struct geneve_opt *option;
	int opts_len = key->tun_opts_len;
	bool crit_opt = false;

	option = (struct geneve_opt *)TUN_METADATA_OPTS(key, key->tun_opts_len);
	while (opts_len > 0) {
		int len;

		if (opts_len < sizeof(*option))
			return -EINVAL;

		len = sizeof(*option) + option->length * 4;
		if (len > opts_len)
			return -EINVAL;

		crit_opt |= !!(option->type & GENEVE_CRIT_OPT_TYPE);

		option = (struct geneve_opt *)((u8 *)option + len);
		opts_len -= len;
	}

	key->tun_key.tun_flags |= crit_opt ? TUNNEL_CRIT_OPT : 0;

	return 0;
}

static int validate_and_copy_set_tun(const struct nlattr *attr,
				     struct sw_flow_actions **sfa, bool log)
{
	struct sw_flow_match match;
	struct sw_flow_key key;
	struct metadata_dst *tun_dst;
	struct ip_tunnel_info *tun_info;
	struct ovs_tunnel_info *ovs_tun;
	struct nlattr *a;
	int err = 0, start, opts_type;
	__be16 dst_opt_type;

	dst_opt_type = 0;
	ovs_match_init(&match, &key, true, NULL);
	opts_type = ip_tun_from_nlattr(nla_data(attr), &match, false, log);
	if (opts_type < 0)
		return opts_type;

	if (key.tun_opts_len) {
		switch (opts_type) {
		case OVS_TUNNEL_KEY_ATTR_GENEVE_OPTS:
			err = validate_geneve_opts(&key);
			if (err < 0)
				return err;
			dst_opt_type = TUNNEL_GENEVE_OPT;
			break;
		case OVS_TUNNEL_KEY_ATTR_VXLAN_OPTS:
			dst_opt_type = TUNNEL_VXLAN_OPT;
			break;
		case OVS_TUNNEL_KEY_ATTR_ERSPAN_OPTS:
			dst_opt_type = TUNNEL_ERSPAN_OPT;
			break;
		}
	}

	start = add_nested_action_start(sfa, OVS_ACTION_ATTR_SET, log);
	if (start < 0)
		return start;

	tun_dst = metadata_dst_alloc(key.tun_opts_len, METADATA_IP_TUNNEL,
				     GFP_KERNEL);

	if (!tun_dst)
		return -ENOMEM;

	err = dst_cache_init(&tun_dst->u.tun_info.dst_cache, GFP_KERNEL);
	if (err) {
		dst_release((struct dst_entry *)tun_dst);
		return err;
	}

	a = __add_action(sfa, OVS_KEY_ATTR_TUNNEL_INFO, NULL,
			 sizeof(*ovs_tun), log);
	if (IS_ERR(a)) {
		dst_release((struct dst_entry *)tun_dst);
		return PTR_ERR(a);
	}

	ovs_tun = nla_data(a);
	ovs_tun->tun_dst = tun_dst;

	tun_info = &tun_dst->u.tun_info;
	tun_info->mode = IP_TUNNEL_INFO_TX;
	if (key.tun_proto == AF_INET6)
		tun_info->mode |= IP_TUNNEL_INFO_IPV6;
	else if (key.tun_proto == AF_INET && key.tun_key.u.ipv4.dst == 0)
		tun_info->mode |= IP_TUNNEL_INFO_BRIDGE;
	tun_info->key = key.tun_key;

	/* We need to store the options in the action itself since
	 * everything else will go away after flow setup. We can append
	 * it to tun_info and then point there.
	 */
	ip_tunnel_info_opts_set(tun_info,
				TUN_METADATA_OPTS(&key, key.tun_opts_len),
				key.tun_opts_len, dst_opt_type);
	add_nested_action_end(*sfa, start);

	return err;
}

static bool validate_nsh(const struct nlattr *attr, bool is_mask,
			 bool is_push_nsh, bool log)
{
	struct sw_flow_match match;
	struct sw_flow_key key;
	int ret = 0;

	ovs_match_init(&match, &key, true, NULL);
	ret = nsh_key_put_from_nlattr(attr, &match, is_mask,
				      is_push_nsh, log);
	return !ret;
}

/* Return false if there are any non-masked bits set.
 * Mask follows data immediately, before any netlink padding.
 */
static bool validate_masked(u8 *data, int len)
{
	u8 *mask = data + len;

	while (len--)
		if (*data++ & ~*mask++)
			return false;

	return true;
}

static int validate_set(const struct nlattr *a,
			const struct sw_flow_key *flow_key,
			struct sw_flow_actions **sfa, bool *skip_copy,
			u8 mac_proto, __be16 eth_type, bool masked, bool log)
{
	const struct nlattr *ovs_key = nla_data(a);
	int key_type = nla_type(ovs_key);
	size_t key_len;

	/* There can be only one key in a action */
	if (nla_total_size(nla_len(ovs_key)) != nla_len(a))
		return -EINVAL;

	key_len = nla_len(ovs_key);
	if (masked)
		key_len /= 2;

	if (key_type > OVS_KEY_ATTR_MAX ||
	    !check_attr_len(key_len, ovs_key_lens[key_type].len))
		return -EINVAL;

	if (masked && !validate_masked(nla_data(ovs_key), key_len))
		return -EINVAL;

	switch (key_type) {
	case OVS_KEY_ATTR_PRIORITY:
	case OVS_KEY_ATTR_SKB_MARK:
	case OVS_KEY_ATTR_CT_MARK:
	case OVS_KEY_ATTR_CT_LABELS:
		break;

	case OVS_KEY_ATTR_ETHERNET:
		if (mac_proto != MAC_PROTO_ETHERNET)
			return -EINVAL;
		break;

	case OVS_KEY_ATTR_TUNNEL: {
		int err;

		if (masked)
			return -EINVAL; /* Masked tunnel set not supported. */

		*skip_copy = true;
		err = validate_and_copy_set_tun(a, sfa, log);
		if (err)
			return err;
		break;
	}
	case OVS_KEY_ATTR_IPV4: {
		const struct ovs_key_ipv4 *ipv4_key;

		if (eth_type != htons(ETH_P_IP))
			return -EINVAL;

		ipv4_key = nla_data(ovs_key);

		if (masked) {
			const struct ovs_key_ipv4 *mask = ipv4_key + 1;

			/* Non-writeable fields. */
			if (mask->ipv4_proto || mask->ipv4_frag)
				return -EINVAL;
		} else {
			if (ipv4_key->ipv4_proto != flow_key->ip.proto)
				return -EINVAL;

			if (ipv4_key->ipv4_frag != flow_key->ip.frag)
				return -EINVAL;
		}
		break;
	}
	case OVS_KEY_ATTR_IPV6: {
		const struct ovs_key_ipv6 *ipv6_key;

		if (eth_type != htons(ETH_P_IPV6))
			return -EINVAL;

		ipv6_key = nla_data(ovs_key);

		if (masked) {
			const struct ovs_key_ipv6 *mask = ipv6_key + 1;

			/* Non-writeable fields. */
			if (mask->ipv6_proto || mask->ipv6_frag)
				return -EINVAL;

			/* Invalid bits in the flow label mask? */
			if (ntohl(mask->ipv6_label) & 0xFFF00000)
				return -EINVAL;
		} else {
			if (ipv6_key->ipv6_proto != flow_key->ip.proto)
				return -EINVAL;

			if (ipv6_key->ipv6_frag != flow_key->ip.frag)
				return -EINVAL;
		}
		if (ntohl(ipv6_key->ipv6_label) & 0xFFF00000)
			return -EINVAL;

		break;
	}
	case OVS_KEY_ATTR_TCP:
		if ((eth_type != htons(ETH_P_IP) &&
		     eth_type != htons(ETH_P_IPV6)) ||
		    flow_key->ip.proto != IPPROTO_TCP)
			return -EINVAL;

		break;

	case OVS_KEY_ATTR_UDP:
		if ((eth_type != htons(ETH_P_IP) &&
		     eth_type != htons(ETH_P_IPV6)) ||
		    flow_key->ip.proto != IPPROTO_UDP)
			return -EINVAL;

		break;

	case OVS_KEY_ATTR_MPLS:
		if (!eth_p_mpls(eth_type))
			return -EINVAL;
		break;

	case OVS_KEY_ATTR_SCTP:
		if ((eth_type != htons(ETH_P_IP) &&
		     eth_type != htons(ETH_P_IPV6)) ||
		    flow_key->ip.proto != IPPROTO_SCTP)
			return -EINVAL;

		break;

	case OVS_KEY_ATTR_NSH:
		if (eth_type != htons(ETH_P_NSH))
			return -EINVAL;
		if (!validate_nsh(nla_data(a), masked, false, log))
			return -EINVAL;
		break;

	default:
		return -EINVAL;
	}

	/* Convert non-masked non-tunnel set actions to masked set actions. */
	if (!masked && key_type != OVS_KEY_ATTR_TUNNEL) {
		int start, len = key_len * 2;
		struct nlattr *at;

		*skip_copy = true;

		start = add_nested_action_start(sfa,
						OVS_ACTION_ATTR_SET_TO_MASKED,
						log);
		if (start < 0)
			return start;

		at = __add_action(sfa, key_type, NULL, len, log);
		if (IS_ERR(at))
			return PTR_ERR(at);

		memcpy(nla_data(at), nla_data(ovs_key), key_len); /* Key. */
		memset(nla_data(at) + key_len, 0xff, key_len);    /* Mask. */
		/* Clear non-writeable bits from otherwise writeable fields. */
		if (key_type == OVS_KEY_ATTR_IPV6) {
			struct ovs_key_ipv6 *mask = nla_data(at) + key_len;

			mask->ipv6_label &= htonl(0x000FFFFF);
		}
		add_nested_action_end(*sfa, start);
	}

	return 0;
}

static int validate_userspace(const struct nlattr *attr)
{
	static const struct nla_policy userspace_policy[OVS_USERSPACE_ATTR_MAX + 1] = {
		[OVS_USERSPACE_ATTR_PID] = {.type = NLA_U32 },
		[OVS_USERSPACE_ATTR_USERDATA] = {.type = NLA_UNSPEC },
		[OVS_USERSPACE_ATTR_EGRESS_TUN_PORT] = {.type = NLA_U32 },
	};
	struct nlattr *a[OVS_USERSPACE_ATTR_MAX + 1];
	int error;

	error = nla_parse_nested_deprecated(a, OVS_USERSPACE_ATTR_MAX, attr,
					    userspace_policy, NULL);
	if (error)
		return error;

	if (!a[OVS_USERSPACE_ATTR_PID] ||
	    !nla_get_u32(a[OVS_USERSPACE_ATTR_PID]))
		return -EINVAL;

	return 0;
}

static const struct nla_policy cpl_policy[OVS_CHECK_PKT_LEN_ATTR_MAX + 1] = {
	[OVS_CHECK_PKT_LEN_ATTR_PKT_LEN] = {.type = NLA_U16 },
	[OVS_CHECK_PKT_LEN_ATTR_ACTIONS_IF_GREATER] = {.type = NLA_NESTED },
	[OVS_CHECK_PKT_LEN_ATTR_ACTIONS_IF_LESS_EQUAL] = {.type = NLA_NESTED },
};

static int validate_and_copy_check_pkt_len(struct net *net,
					   const struct nlattr *attr,
					   const struct sw_flow_key *key,
					   struct sw_flow_actions **sfa,
					   __be16 eth_type, __be16 vlan_tci,
					   u32 mpls_label_count,
					   bool log, bool last)
{
	const struct nlattr *acts_if_greater, *acts_if_lesser_eq;
	struct nlattr *a[OVS_CHECK_PKT_LEN_ATTR_MAX + 1];
	struct check_pkt_len_arg arg;
	int nested_acts_start;
	int start, err;

	err = nla_parse_deprecated_strict(a, OVS_CHECK_PKT_LEN_ATTR_MAX,
					  nla_data(attr), nla_len(attr),
					  cpl_policy, NULL);
	if (err)
		return err;

	if (!a[OVS_CHECK_PKT_LEN_ATTR_PKT_LEN] ||
	    !nla_get_u16(a[OVS_CHECK_PKT_LEN_ATTR_PKT_LEN]))
		return -EINVAL;

	acts_if_lesser_eq = a[OVS_CHECK_PKT_LEN_ATTR_ACTIONS_IF_LESS_EQUAL];
	acts_if_greater = a[OVS_CHECK_PKT_LEN_ATTR_ACTIONS_IF_GREATER];

	/* Both the nested action should be present. */
	if (!acts_if_greater || !acts_if_lesser_eq)
		return -EINVAL;

	/* validation done, copy the nested actions. */
	start = add_nested_action_start(sfa, OVS_ACTION_ATTR_CHECK_PKT_LEN,
					log);
	if (start < 0)
		return start;

	arg.pkt_len = nla_get_u16(a[OVS_CHECK_PKT_LEN_ATTR_PKT_LEN]);
	arg.exec_for_lesser_equal =
		last || !actions_may_change_flow(acts_if_lesser_eq);
	arg.exec_for_greater =
		last || !actions_may_change_flow(acts_if_greater);

	err = ovs_nla_add_action(sfa, OVS_CHECK_PKT_LEN_ATTR_ARG, &arg,
				 sizeof(arg), log);
	if (err)
		return err;

	nested_acts_start = add_nested_action_start(sfa,
		OVS_CHECK_PKT_LEN_ATTR_ACTIONS_IF_LESS_EQUAL, log);
	if (nested_acts_start < 0)
		return nested_acts_start;

	err = __ovs_nla_copy_actions(net, acts_if_lesser_eq, key, sfa,
				     eth_type, vlan_tci, mpls_label_count, log);

	if (err)
		return err;

	add_nested_action_end(*sfa, nested_acts_start);

	nested_acts_start = add_nested_action_start(sfa,
		OVS_CHECK_PKT_LEN_ATTR_ACTIONS_IF_GREATER, log);
	if (nested_acts_start < 0)
		return nested_acts_start;

	err = __ovs_nla_copy_actions(net, acts_if_greater, key, sfa,
				     eth_type, vlan_tci, mpls_label_count, log);

	if (err)
		return err;

	add_nested_action_end(*sfa, nested_acts_start);
	add_nested_action_end(*sfa, start);
	return 0;
}

static int copy_action(const struct nlattr *from,
		       struct sw_flow_actions **sfa, bool log)
{
	int totlen = NLA_ALIGN(from->nla_len);
	struct nlattr *to;

	to = reserve_sfa_size(sfa, from->nla_len, log);
	if (IS_ERR(to))
		return PTR_ERR(to);

	memcpy(to, from, totlen);
	return 0;
}

static int __ovs_nla_copy_actions(struct net *net, const struct nlattr *attr,
				  const struct sw_flow_key *key,
				  struct sw_flow_actions **sfa,
				  __be16 eth_type, __be16 vlan_tci,
				  u32 mpls_label_count, bool log)
{
	u8 mac_proto = ovs_key_mac_proto(key);
	const struct nlattr *a;
	int rem, err;

	nla_for_each_nested(a, attr, rem) {
		/* Expected argument lengths, (u32)-1 for variable length. */
		static const u32 action_lens[OVS_ACTION_ATTR_MAX + 1] = {
			[OVS_ACTION_ATTR_OUTPUT] = sizeof(u32),
			[OVS_ACTION_ATTR_RECIRC] = sizeof(u32),
			[OVS_ACTION_ATTR_USERSPACE] = (u32)-1,
			[OVS_ACTION_ATTR_PUSH_MPLS] = sizeof(struct ovs_action_push_mpls),
			[OVS_ACTION_ATTR_POP_MPLS] = sizeof(__be16),
			[OVS_ACTION_ATTR_PUSH_VLAN] = sizeof(struct ovs_action_push_vlan),
			[OVS_ACTION_ATTR_POP_VLAN] = 0,
			[OVS_ACTION_ATTR_SET] = (u32)-1,
			[OVS_ACTION_ATTR_SET_MASKED] = (u32)-1,
			[OVS_ACTION_ATTR_SAMPLE] = (u32)-1,
			[OVS_ACTION_ATTR_HASH] = sizeof(struct ovs_action_hash),
			[OVS_ACTION_ATTR_CT] = (u32)-1,
			[OVS_ACTION_ATTR_CT_CLEAR] = 0,
			[OVS_ACTION_ATTR_TRUNC] = sizeof(struct ovs_action_trunc),
			[OVS_ACTION_ATTR_PUSH_ETH] = sizeof(struct ovs_action_push_eth),
			[OVS_ACTION_ATTR_POP_ETH] = 0,
			[OVS_ACTION_ATTR_PUSH_NSH] = (u32)-1,
			[OVS_ACTION_ATTR_POP_NSH] = 0,
			[OVS_ACTION_ATTR_METER] = sizeof(u32),
			[OVS_ACTION_ATTR_CLONE] = (u32)-1,
			[OVS_ACTION_ATTR_CHECK_PKT_LEN] = (u32)-1,
			[OVS_ACTION_ATTR_ADD_MPLS] = sizeof(struct ovs_action_add_mpls),
			[OVS_ACTION_ATTR_DEC_TTL] = (u32)-1,
		};
		const struct ovs_action_push_vlan *vlan;
		int type = nla_type(a);
		bool skip_copy;

		if (type > OVS_ACTION_ATTR_MAX ||
		    (action_lens[type] != nla_len(a) &&
		     action_lens[type] != (u32)-1))
			return -EINVAL;

		skip_copy = false;
		switch (type) {
		case OVS_ACTION_ATTR_UNSPEC:
			return -EINVAL;

		case OVS_ACTION_ATTR_USERSPACE:
			err = validate_userspace(a);
			if (err)
				return err;
			break;

		case OVS_ACTION_ATTR_OUTPUT:
			if (nla_get_u32(a) >= DP_MAX_PORTS)
				return -EINVAL;
			break;

		case OVS_ACTION_ATTR_TRUNC: {
			const struct ovs_action_trunc *trunc = nla_data(a);

			if (trunc->max_len < ETH_HLEN)
				return -EINVAL;
			break;
		}

		case OVS_ACTION_ATTR_HASH: {
			const struct ovs_action_hash *act_hash = nla_data(a);

			switch (act_hash->hash_alg) {
			case OVS_HASH_ALG_L4:
				break;
			default:
				return  -EINVAL;
			}

			break;
		}

		case OVS_ACTION_ATTR_POP_VLAN:
			if (mac_proto != MAC_PROTO_ETHERNET)
				return -EINVAL;
			vlan_tci = htons(0);
			break;

		case OVS_ACTION_ATTR_PUSH_VLAN:
			if (mac_proto != MAC_PROTO_ETHERNET)
				return -EINVAL;
			vlan = nla_data(a);
			if (!eth_type_vlan(vlan->vlan_tpid))
				return -EINVAL;
			if (!(vlan->vlan_tci & htons(VLAN_CFI_MASK)))
				return -EINVAL;
			vlan_tci = vlan->vlan_tci;
			break;

		case OVS_ACTION_ATTR_RECIRC:
			break;

		case OVS_ACTION_ATTR_ADD_MPLS: {
			const struct ovs_action_add_mpls *mpls = nla_data(a);

			if (!eth_p_mpls(mpls->mpls_ethertype))
				return -EINVAL;

			if (mpls->tun_flags & OVS_MPLS_L3_TUNNEL_FLAG_MASK) {
				if (vlan_tci & htons(VLAN_CFI_MASK) ||
				    (eth_type != htons(ETH_P_IP) &&
				     eth_type != htons(ETH_P_IPV6) &&
				     eth_type != htons(ETH_P_ARP) &&
				     eth_type != htons(ETH_P_RARP) &&
				     !eth_p_mpls(eth_type)))
					return -EINVAL;
				mpls_label_count++;
			} else {
				if (mac_proto == MAC_PROTO_ETHERNET) {
					mpls_label_count = 1;
					mac_proto = MAC_PROTO_NONE;
				} else {
					mpls_label_count++;
				}
			}
			eth_type = mpls->mpls_ethertype;
			break;
		}

		case OVS_ACTION_ATTR_PUSH_MPLS: {
			const struct ovs_action_push_mpls *mpls = nla_data(a);

			if (!eth_p_mpls(mpls->mpls_ethertype))
				return -EINVAL;
			/* Prohibit push MPLS other than to a white list
			 * for packets that have a known tag order.
			 */
			if (vlan_tci & htons(VLAN_CFI_MASK) ||
			    (eth_type != htons(ETH_P_IP) &&
			     eth_type != htons(ETH_P_IPV6) &&
			     eth_type != htons(ETH_P_ARP) &&
			     eth_type != htons(ETH_P_RARP) &&
			     !eth_p_mpls(eth_type)))
				return -EINVAL;
			eth_type = mpls->mpls_ethertype;
			mpls_label_count++;
			break;
		}

		case OVS_ACTION_ATTR_POP_MPLS: {
			__be16  proto;
			if (vlan_tci & htons(VLAN_CFI_MASK) ||
			    !eth_p_mpls(eth_type))
				return -EINVAL;

			/* Disallow subsequent L2.5+ set actions and mpls_pop
			 * actions once the last MPLS label in the packet is
			 * is popped as there is no check here to ensure that
			 * the new eth type is valid and thus set actions could
			 * write off the end of the packet or otherwise corrupt
			 * it.
			 *
			 * Support for these actions is planned using packet
			 * recirculation.
			 */
			proto = nla_get_be16(a);

			if (proto == htons(ETH_P_TEB) &&
			    mac_proto != MAC_PROTO_NONE)
				return -EINVAL;

			mpls_label_count--;

			if (!eth_p_mpls(proto) || !mpls_label_count)
				eth_type = htons(0);
			else
				eth_type =  proto;

			break;
		}

		case OVS_ACTION_ATTR_SET:
			err = validate_set(a, key, sfa,
					   &skip_copy, mac_proto, eth_type,
					   false, log);
			if (err)
				return err;
			break;

		case OVS_ACTION_ATTR_SET_MASKED:
			err = validate_set(a, key, sfa,
					   &skip_copy, mac_proto, eth_type,
					   true, log);
			if (err)
				return err;
			break;

		case OVS_ACTION_ATTR_SAMPLE: {
			bool last = nla_is_last(a, rem);

			err = validate_and_copy_sample(net, a, key, sfa,
						       eth_type, vlan_tci,
						       mpls_label_count,
						       log, last);
			if (err)
				return err;
			skip_copy = true;
			break;
		}

		case OVS_ACTION_ATTR_CT:
			err = ovs_ct_copy_action(net, a, key, sfa, log);
			if (err)
				return err;
			skip_copy = true;
			break;

		case OVS_ACTION_ATTR_CT_CLEAR:
			break;

		case OVS_ACTION_ATTR_PUSH_ETH:
			/* Disallow pushing an Ethernet header if one
			 * is already present */
			if (mac_proto != MAC_PROTO_NONE)
				return -EINVAL;
			mac_proto = MAC_PROTO_ETHERNET;
			break;

		case OVS_ACTION_ATTR_POP_ETH:
			if (mac_proto != MAC_PROTO_ETHERNET)
				return -EINVAL;
			if (vlan_tci & htons(VLAN_CFI_MASK))
				return -EINVAL;
			mac_proto = MAC_PROTO_NONE;
			break;

		case OVS_ACTION_ATTR_PUSH_NSH:
			if (mac_proto != MAC_PROTO_ETHERNET) {
				u8 next_proto;

				next_proto = tun_p_from_eth_p(eth_type);
				if (!next_proto)
					return -EINVAL;
			}
			mac_proto = MAC_PROTO_NONE;
			if (!validate_nsh(nla_data(a), false, true, true))
				return -EINVAL;
			break;

		case OVS_ACTION_ATTR_POP_NSH: {
			__be16 inner_proto;

			if (eth_type != htons(ETH_P_NSH))
				return -EINVAL;
			inner_proto = tun_p_to_eth_p(key->nsh.base.np);
			if (!inner_proto)
				return -EINVAL;
			if (key->nsh.base.np == TUN_P_ETHERNET)
				mac_proto = MAC_PROTO_ETHERNET;
			else
				mac_proto = MAC_PROTO_NONE;
			break;
		}

		case OVS_ACTION_ATTR_METER:
			/* Non-existent meters are simply ignored.  */
			break;

		case OVS_ACTION_ATTR_CLONE: {
			bool last = nla_is_last(a, rem);

			err = validate_and_copy_clone(net, a, key, sfa,
						      eth_type, vlan_tci,
						      mpls_label_count,
						      log, last);
			if (err)
				return err;
			skip_copy = true;
			break;
		}

		case OVS_ACTION_ATTR_CHECK_PKT_LEN: {
			bool last = nla_is_last(a, rem);

			err = validate_and_copy_check_pkt_len(net, a, key, sfa,
							      eth_type,
							      vlan_tci,
							      mpls_label_count,
							      log, last);
			if (err)
				return err;
			skip_copy = true;
			break;
		}

		case OVS_ACTION_ATTR_DEC_TTL:
			err = validate_and_copy_dec_ttl(net, a, key, sfa,
							eth_type, vlan_tci,
							mpls_label_count, log);
			if (err)
				return err;
			skip_copy = true;
			break;

		default:
			OVS_NLERR(log, "Unknown Action type %d", type);
			return -EINVAL;
		}
		if (!skip_copy) {
			err = copy_action(a, sfa, log);
			if (err)
				return err;
		}
	}

	if (rem > 0)
		return -EINVAL;

	return 0;
}

/* 'key' must be the masked key. */
int ovs_nla_copy_actions(struct net *net, const struct nlattr *attr,
			 const struct sw_flow_key *key,
			 struct sw_flow_actions **sfa, bool log)
{
	int err;
	u32 mpls_label_count = 0;

	*sfa = nla_alloc_flow_actions(min(nla_len(attr), MAX_ACTIONS_BUFSIZE));
	if (IS_ERR(*sfa))
		return PTR_ERR(*sfa);

	if (eth_p_mpls(key->eth.type))
		mpls_label_count = hweight_long(key->mpls.num_labels_mask);

	(*sfa)->orig_len = nla_len(attr);
	err = __ovs_nla_copy_actions(net, attr, key, sfa, key->eth.type,
				     key->eth.vlan.tci, mpls_label_count, log);
	if (err)
		ovs_nla_free_flow_actions(*sfa);

	return err;
}

static int sample_action_to_attr(const struct nlattr *attr,
				 struct sk_buff *skb)
{
	struct nlattr *start, *ac_start = NULL, *sample_arg;
	int err = 0, rem = nla_len(attr);
	const struct sample_arg *arg;
	struct nlattr *actions;

	start = nla_nest_start_noflag(skb, OVS_ACTION_ATTR_SAMPLE);
	if (!start)
		return -EMSGSIZE;

	sample_arg = nla_data(attr);
	arg = nla_data(sample_arg);
	actions = nla_next(sample_arg, &rem);

	if (nla_put_u32(skb, OVS_SAMPLE_ATTR_PROBABILITY, arg->probability)) {
		err = -EMSGSIZE;
		goto out;
	}

	ac_start = nla_nest_start_noflag(skb, OVS_SAMPLE_ATTR_ACTIONS);
	if (!ac_start) {
		err = -EMSGSIZE;
		goto out;
	}

	err = ovs_nla_put_actions(actions, rem, skb);

out:
	if (err) {
		nla_nest_cancel(skb, ac_start);
		nla_nest_cancel(skb, start);
	} else {
		nla_nest_end(skb, ac_start);
		nla_nest_end(skb, start);
	}

	return err;
}

static int clone_action_to_attr(const struct nlattr *attr,
				struct sk_buff *skb)
{
	struct nlattr *start;
	int err = 0, rem = nla_len(attr);

	start = nla_nest_start_noflag(skb, OVS_ACTION_ATTR_CLONE);
	if (!start)
		return -EMSGSIZE;

	err = ovs_nla_put_actions(nla_data(attr), rem, skb);

	if (err)
		nla_nest_cancel(skb, start);
	else
		nla_nest_end(skb, start);

	return err;
}

static int check_pkt_len_action_to_attr(const struct nlattr *attr,
					struct sk_buff *skb)
{
	struct nlattr *start, *ac_start = NULL;
	const struct check_pkt_len_arg *arg;
	const struct nlattr *a, *cpl_arg;
	int err = 0, rem = nla_len(attr);

	start = nla_nest_start_noflag(skb, OVS_ACTION_ATTR_CHECK_PKT_LEN);
	if (!start)
		return -EMSGSIZE;

	/* The first nested attribute in 'attr' is always
	 * 'OVS_CHECK_PKT_LEN_ATTR_ARG'.
	 */
	cpl_arg = nla_data(attr);
	arg = nla_data(cpl_arg);

	if (nla_put_u16(skb, OVS_CHECK_PKT_LEN_ATTR_PKT_LEN, arg->pkt_len)) {
		err = -EMSGSIZE;
		goto out;
	}

	/* Second nested attribute in 'attr' is always
	 * 'OVS_CHECK_PKT_LEN_ATTR_ACTIONS_IF_LESS_EQUAL'.
	 */
	a = nla_next(cpl_arg, &rem);
	ac_start =  nla_nest_start_noflag(skb,
					  OVS_CHECK_PKT_LEN_ATTR_ACTIONS_IF_LESS_EQUAL);
	if (!ac_start) {
		err = -EMSGSIZE;
		goto out;
	}

	err = ovs_nla_put_actions(nla_data(a), nla_len(a), skb);
	if (err) {
		nla_nest_cancel(skb, ac_start);
		goto out;
	} else {
		nla_nest_end(skb, ac_start);
	}

	/* Third nested attribute in 'attr' is always
	 * OVS_CHECK_PKT_LEN_ATTR_ACTIONS_IF_GREATER.
	 */
	a = nla_next(a, &rem);
	ac_start =  nla_nest_start_noflag(skb,
					  OVS_CHECK_PKT_LEN_ATTR_ACTIONS_IF_GREATER);
	if (!ac_start) {
		err = -EMSGSIZE;
		goto out;
	}

	err = ovs_nla_put_actions(nla_data(a), nla_len(a), skb);
	if (err) {
		nla_nest_cancel(skb, ac_start);
		goto out;
	} else {
		nla_nest_end(skb, ac_start);
	}

	nla_nest_end(skb, start);
	return 0;

out:
	nla_nest_cancel(skb, start);
	return err;
}

static int dec_ttl_action_to_attr(const struct nlattr *attr,
				  struct sk_buff *skb)
{
	struct nlattr *start, *action_start;
	const struct nlattr *a;
	int err = 0, rem;

	start = nla_nest_start_noflag(skb, OVS_ACTION_ATTR_DEC_TTL);
	if (!start)
		return -EMSGSIZE;

	nla_for_each_attr(a, nla_data(attr), nla_len(attr), rem) {
		switch (nla_type(a)) {
		case OVS_DEC_TTL_ATTR_ACTION:

			action_start = nla_nest_start_noflag(skb, OVS_DEC_TTL_ATTR_ACTION);
			if (!action_start) {
				err = -EMSGSIZE;
				goto out;
			}

			err = ovs_nla_put_actions(nla_data(a), nla_len(a), skb);
			if (err)
				goto out;

			nla_nest_end(skb, action_start);
			break;

		default:
			/* Ignore all other option to be future compatible */
			break;
		}
	}

	nla_nest_end(skb, start);
	return 0;

out:
	nla_nest_cancel(skb, start);
	return err;
}

static int set_action_to_attr(const struct nlattr *a, struct sk_buff *skb)
{
	const struct nlattr *ovs_key = nla_data(a);
	int key_type = nla_type(ovs_key);
	struct nlattr *start;
	int err;

	switch (key_type) {
	case OVS_KEY_ATTR_TUNNEL_INFO: {
		struct ovs_tunnel_info *ovs_tun = nla_data(ovs_key);
		struct ip_tunnel_info *tun_info = &ovs_tun->tun_dst->u.tun_info;

		start = nla_nest_start_noflag(skb, OVS_ACTION_ATTR_SET);
		if (!start)
			return -EMSGSIZE;

		err =  ip_tun_to_nlattr(skb, &tun_info->key,
					ip_tunnel_info_opts(tun_info),
					tun_info->options_len,
					ip_tunnel_info_af(tun_info), tun_info->mode);
		if (err)
			return err;
		nla_nest_end(skb, start);
		break;
	}
	default:
		if (nla_put(skb, OVS_ACTION_ATTR_SET, nla_len(a), ovs_key))
			return -EMSGSIZE;
		break;
	}

	return 0;
}

static int masked_set_action_to_set_action_attr(const struct nlattr *a,
						struct sk_buff *skb)
{
	const struct nlattr *ovs_key = nla_data(a);
	struct nlattr *nla;
	size_t key_len = nla_len(ovs_key) / 2;

	/* Revert the conversion we did from a non-masked set action to
	 * masked set action.
	 */
	nla = nla_nest_start_noflag(skb, OVS_ACTION_ATTR_SET);
	if (!nla)
		return -EMSGSIZE;

	if (nla_put(skb, nla_type(ovs_key), key_len, nla_data(ovs_key)))
		return -EMSGSIZE;

	nla_nest_end(skb, nla);
	return 0;
}

int ovs_nla_put_actions(const struct nlattr *attr, int len, struct sk_buff *skb)
{
	const struct nlattr *a;
	int rem, err;

	nla_for_each_attr(a, attr, len, rem) {
		int type = nla_type(a);

		switch (type) {
		case OVS_ACTION_ATTR_SET:
			err = set_action_to_attr(a, skb);
			if (err)
				return err;
			break;

		case OVS_ACTION_ATTR_SET_TO_MASKED:
			err = masked_set_action_to_set_action_attr(a, skb);
			if (err)
				return err;
			break;

		case OVS_ACTION_ATTR_SAMPLE:
			err = sample_action_to_attr(a, skb);
			if (err)
				return err;
			break;

		case OVS_ACTION_ATTR_CT:
			err = ovs_ct_action_to_attr(nla_data(a), skb);
			if (err)
				return err;
			break;

		case OVS_ACTION_ATTR_CLONE:
			err = clone_action_to_attr(a, skb);
			if (err)
				return err;
			break;

		case OVS_ACTION_ATTR_CHECK_PKT_LEN:
			err = check_pkt_len_action_to_attr(a, skb);
			if (err)
				return err;
			break;

		case OVS_ACTION_ATTR_DEC_TTL:
			err = dec_ttl_action_to_attr(a, skb);
			if (err)
				return err;
			break;

		default:
			if (nla_put(skb, type, nla_len(a), nla_data(a)))
				return -EMSGSIZE;
			break;
		}
	}

	return 0;
}
