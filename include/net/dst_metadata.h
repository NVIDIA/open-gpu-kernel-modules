/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __NET_DST_METADATA_H
#define __NET_DST_METADATA_H 1

#include <linux/skbuff.h>
#include <net/ip_tunnels.h>
#include <net/dst.h>

enum metadata_type {
	METADATA_IP_TUNNEL,
	METADATA_HW_PORT_MUX,
};

struct hw_port_info {
	struct net_device *lower_dev;
	u32 port_id;
};

struct metadata_dst {
	struct dst_entry		dst;
	enum metadata_type		type;
	union {
		struct ip_tunnel_info	tun_info;
		struct hw_port_info	port_info;
	} u;
};

static inline struct metadata_dst *skb_metadata_dst(const struct sk_buff *skb)
{
	struct metadata_dst *md_dst = (struct metadata_dst *) skb_dst(skb);

	if (md_dst && md_dst->dst.flags & DST_METADATA)
		return md_dst;

	return NULL;
}

static inline struct ip_tunnel_info *
skb_tunnel_info(const struct sk_buff *skb)
{
	struct metadata_dst *md_dst = skb_metadata_dst(skb);
	struct dst_entry *dst;

	if (md_dst && md_dst->type == METADATA_IP_TUNNEL)
		return &md_dst->u.tun_info;

	dst = skb_dst(skb);
	if (dst && dst->lwtstate)
		return lwt_tun_info(dst->lwtstate);

	return NULL;
}

static inline bool skb_valid_dst(const struct sk_buff *skb)
{
	struct dst_entry *dst = skb_dst(skb);

	return dst && !(dst->flags & DST_METADATA);
}

static inline int skb_metadata_dst_cmp(const struct sk_buff *skb_a,
				       const struct sk_buff *skb_b)
{
	const struct metadata_dst *a, *b;

	if (!(skb_a->_skb_refdst | skb_b->_skb_refdst))
		return 0;

	a = (const struct metadata_dst *) skb_dst(skb_a);
	b = (const struct metadata_dst *) skb_dst(skb_b);

	if (!a != !b || a->type != b->type)
		return 1;

	switch (a->type) {
	case METADATA_HW_PORT_MUX:
		return memcmp(&a->u.port_info, &b->u.port_info,
			      sizeof(a->u.port_info));
	case METADATA_IP_TUNNEL:
		return memcmp(&a->u.tun_info, &b->u.tun_info,
			      sizeof(a->u.tun_info) +
					 a->u.tun_info.options_len);
	default:
		return 1;
	}
}

void metadata_dst_free(struct metadata_dst *);
struct metadata_dst *metadata_dst_alloc(u8 optslen, enum metadata_type type,
					gfp_t flags);
void metadata_dst_free_percpu(struct metadata_dst __percpu *md_dst);
struct metadata_dst __percpu *
metadata_dst_alloc_percpu(u8 optslen, enum metadata_type type, gfp_t flags);

static inline struct metadata_dst *tun_rx_dst(int md_size)
{
	struct metadata_dst *tun_dst;

	tun_dst = metadata_dst_alloc(md_size, METADATA_IP_TUNNEL, GFP_ATOMIC);
	if (!tun_dst)
		return NULL;

	tun_dst->u.tun_info.options_len = 0;
	tun_dst->u.tun_info.mode = 0;
	return tun_dst;
}

static inline struct metadata_dst *tun_dst_unclone(struct sk_buff *skb)
{
	struct metadata_dst *md_dst = skb_metadata_dst(skb);
	int md_size;
	struct metadata_dst *new_md;

	if (!md_dst || md_dst->type != METADATA_IP_TUNNEL)
		return ERR_PTR(-EINVAL);

	md_size = md_dst->u.tun_info.options_len;
	new_md = metadata_dst_alloc(md_size, METADATA_IP_TUNNEL, GFP_ATOMIC);
	if (!new_md)
		return ERR_PTR(-ENOMEM);

	memcpy(&new_md->u.tun_info, &md_dst->u.tun_info,
	       sizeof(struct ip_tunnel_info) + md_size);
	skb_dst_drop(skb);
	dst_hold(&new_md->dst);
	skb_dst_set(skb, &new_md->dst);
	return new_md;
}

static inline struct ip_tunnel_info *skb_tunnel_info_unclone(struct sk_buff *skb)
{
	struct metadata_dst *dst;

	dst = tun_dst_unclone(skb);
	if (IS_ERR(dst))
		return NULL;

	return &dst->u.tun_info;
}

static inline struct metadata_dst *__ip_tun_set_dst(__be32 saddr,
						    __be32 daddr,
						    __u8 tos, __u8 ttl,
						    __be16 tp_dst,
						    __be16 flags,
						    __be64 tunnel_id,
						    int md_size)
{
	struct metadata_dst *tun_dst;

	tun_dst = tun_rx_dst(md_size);
	if (!tun_dst)
		return NULL;

	ip_tunnel_key_init(&tun_dst->u.tun_info.key,
			   saddr, daddr, tos, ttl,
			   0, 0, tp_dst, tunnel_id, flags);
	return tun_dst;
}

static inline struct metadata_dst *ip_tun_rx_dst(struct sk_buff *skb,
						 __be16 flags,
						 __be64 tunnel_id,
						 int md_size)
{
	const struct iphdr *iph = ip_hdr(skb);

	return __ip_tun_set_dst(iph->saddr, iph->daddr, iph->tos, iph->ttl,
				0, flags, tunnel_id, md_size);
}

static inline struct metadata_dst *__ipv6_tun_set_dst(const struct in6_addr *saddr,
						      const struct in6_addr *daddr,
						      __u8 tos, __u8 ttl,
						      __be16 tp_dst,
						      __be32 label,
						      __be16 flags,
						      __be64 tunnel_id,
						      int md_size)
{
	struct metadata_dst *tun_dst;
	struct ip_tunnel_info *info;

	tun_dst = tun_rx_dst(md_size);
	if (!tun_dst)
		return NULL;

	info = &tun_dst->u.tun_info;
	info->mode = IP_TUNNEL_INFO_IPV6;
	info->key.tun_flags = flags;
	info->key.tun_id = tunnel_id;
	info->key.tp_src = 0;
	info->key.tp_dst = tp_dst;

	info->key.u.ipv6.src = *saddr;
	info->key.u.ipv6.dst = *daddr;

	info->key.tos = tos;
	info->key.ttl = ttl;
	info->key.label = label;

	return tun_dst;
}

static inline struct metadata_dst *ipv6_tun_rx_dst(struct sk_buff *skb,
						   __be16 flags,
						   __be64 tunnel_id,
						   int md_size)
{
	const struct ipv6hdr *ip6h = ipv6_hdr(skb);

	return __ipv6_tun_set_dst(&ip6h->saddr, &ip6h->daddr,
				  ipv6_get_dsfield(ip6h), ip6h->hop_limit,
				  0, ip6_flowlabel(ip6h), flags, tunnel_id,
				  md_size);
}
#endif /* __NET_DST_METADATA_H */
