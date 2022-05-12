/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * INET		An implementation of the TCP/IP protocol suite for the LINUX
 *		operating system.  INET  is implemented using the  BSD Socket
 *		interface as the means of communication with the user level.
 *
 *		Definitions for the IP router.
 *
 * Version:	@(#)route.h	1.0.4	05/27/93
 *
 * Authors:	Ross Biro
 *		Fred N. van Kempen, <waltje@uWalt.NL.Mugnet.ORG>
 * Fixes:
 *		Alan Cox	:	Reformatted. Added ip_rt_local()
 *		Alan Cox	:	Support for TCP parameters.
 *		Alexey Kuznetsov:	Major changes for new routing code.
 *		Mike McLagan    :	Routing by source
 *		Robert Olsson   :	Added rt_cache statistics
 */
#ifndef _ROUTE_H
#define _ROUTE_H

#include <net/dst.h>
#include <net/inetpeer.h>
#include <net/flow.h>
#include <net/inet_sock.h>
#include <net/ip_fib.h>
#include <net/arp.h>
#include <net/ndisc.h>
#include <linux/in_route.h>
#include <linux/rtnetlink.h>
#include <linux/rcupdate.h>
#include <linux/route.h>
#include <linux/ip.h>
#include <linux/cache.h>
#include <linux/security.h>

/* IPv4 datagram length is stored into 16bit field (tot_len) */
#define IP_MAX_MTU	0xFFFFU

#define RTO_ONLINK	0x01

#define RT_CONN_FLAGS(sk)   (RT_TOS(inet_sk(sk)->tos) | sock_flag(sk, SOCK_LOCALROUTE))
#define RT_CONN_FLAGS_TOS(sk,tos)   (RT_TOS(tos) | sock_flag(sk, SOCK_LOCALROUTE))

struct fib_nh;
struct fib_info;
struct uncached_list;
struct rtable {
	struct dst_entry	dst;

	int			rt_genid;
	unsigned int		rt_flags;
	__u16			rt_type;
	__u8			rt_is_input;
	__u8			rt_uses_gateway;

	int			rt_iif;

	u8			rt_gw_family;
	/* Info on neighbour */
	union {
		__be32		rt_gw4;
		struct in6_addr	rt_gw6;
	};

	/* Miscellaneous cached information */
	u32			rt_mtu_locked:1,
				rt_pmtu:31;

	struct list_head	rt_uncached;
	struct uncached_list	*rt_uncached_list;
};

static inline bool rt_is_input_route(const struct rtable *rt)
{
	return rt->rt_is_input != 0;
}

static inline bool rt_is_output_route(const struct rtable *rt)
{
	return rt->rt_is_input == 0;
}

static inline __be32 rt_nexthop(const struct rtable *rt, __be32 daddr)
{
	if (rt->rt_gw_family == AF_INET)
		return rt->rt_gw4;
	return daddr;
}

struct ip_rt_acct {
	__u32 	o_bytes;
	__u32 	o_packets;
	__u32 	i_bytes;
	__u32 	i_packets;
};

struct rt_cache_stat {
        unsigned int in_slow_tot;
        unsigned int in_slow_mc;
        unsigned int in_no_route;
        unsigned int in_brd;
        unsigned int in_martian_dst;
        unsigned int in_martian_src;
        unsigned int out_slow_tot;
        unsigned int out_slow_mc;
};

extern struct ip_rt_acct __percpu *ip_rt_acct;

struct in_device;

int ip_rt_init(void);
void rt_cache_flush(struct net *net);
void rt_flush_dev(struct net_device *dev);
struct rtable *ip_route_output_key_hash(struct net *net, struct flowi4 *flp,
					const struct sk_buff *skb);
struct rtable *ip_route_output_key_hash_rcu(struct net *net, struct flowi4 *flp,
					    struct fib_result *res,
					    const struct sk_buff *skb);

static inline struct rtable *__ip_route_output_key(struct net *net,
						   struct flowi4 *flp)
{
	return ip_route_output_key_hash(net, flp, NULL);
}

struct rtable *ip_route_output_flow(struct net *, struct flowi4 *flp,
				    const struct sock *sk);
struct rtable *ip_route_output_tunnel(struct sk_buff *skb,
				      struct net_device *dev,
				      struct net *net, __be32 *saddr,
				      const struct ip_tunnel_info *info,
				      u8 protocol, bool use_cache);

struct dst_entry *ipv4_blackhole_route(struct net *net,
				       struct dst_entry *dst_orig);

static inline struct rtable *ip_route_output_key(struct net *net, struct flowi4 *flp)
{
	return ip_route_output_flow(net, flp, NULL);
}

static inline struct rtable *ip_route_output(struct net *net, __be32 daddr,
					     __be32 saddr, u8 tos, int oif)
{
	struct flowi4 fl4 = {
		.flowi4_oif = oif,
		.flowi4_tos = tos,
		.daddr = daddr,
		.saddr = saddr,
	};
	return ip_route_output_key(net, &fl4);
}

static inline struct rtable *ip_route_output_ports(struct net *net, struct flowi4 *fl4,
						   struct sock *sk,
						   __be32 daddr, __be32 saddr,
						   __be16 dport, __be16 sport,
						   __u8 proto, __u8 tos, int oif)
{
	flowi4_init_output(fl4, oif, sk ? sk->sk_mark : 0, tos,
			   RT_SCOPE_UNIVERSE, proto,
			   sk ? inet_sk_flowi_flags(sk) : 0,
			   daddr, saddr, dport, sport, sock_net_uid(net, sk));
	if (sk)
		security_sk_classify_flow(sk, flowi4_to_flowi_common(fl4));
	return ip_route_output_flow(net, fl4, sk);
}

static inline struct rtable *ip_route_output_gre(struct net *net, struct flowi4 *fl4,
						 __be32 daddr, __be32 saddr,
						 __be32 gre_key, __u8 tos, int oif)
{
	memset(fl4, 0, sizeof(*fl4));
	fl4->flowi4_oif = oif;
	fl4->daddr = daddr;
	fl4->saddr = saddr;
	fl4->flowi4_tos = tos;
	fl4->flowi4_proto = IPPROTO_GRE;
	fl4->fl4_gre_key = gre_key;
	return ip_route_output_key(net, fl4);
}
int ip_mc_validate_source(struct sk_buff *skb, __be32 daddr, __be32 saddr,
			  u8 tos, struct net_device *dev,
			  struct in_device *in_dev, u32 *itag);
int ip_route_input_noref(struct sk_buff *skb, __be32 dst, __be32 src,
			 u8 tos, struct net_device *devin);
int ip_route_input_rcu(struct sk_buff *skb, __be32 dst, __be32 src,
		       u8 tos, struct net_device *devin,
		       struct fib_result *res);

int ip_route_use_hint(struct sk_buff *skb, __be32 dst, __be32 src,
		      u8 tos, struct net_device *devin,
		      const struct sk_buff *hint);

static inline int ip_route_input(struct sk_buff *skb, __be32 dst, __be32 src,
				 u8 tos, struct net_device *devin)
{
	int err;

	rcu_read_lock();
	err = ip_route_input_noref(skb, dst, src, tos, devin);
	if (!err) {
		skb_dst_force(skb);
		if (!skb_dst(skb))
			err = -EINVAL;
	}
	rcu_read_unlock();

	return err;
}

void ipv4_update_pmtu(struct sk_buff *skb, struct net *net, u32 mtu, int oif,
		      u8 protocol);
void ipv4_sk_update_pmtu(struct sk_buff *skb, struct sock *sk, u32 mtu);
void ipv4_redirect(struct sk_buff *skb, struct net *net, int oif, u8 protocol);
void ipv4_sk_redirect(struct sk_buff *skb, struct sock *sk);
void ip_rt_send_redirect(struct sk_buff *skb);

unsigned int inet_addr_type(struct net *net, __be32 addr);
unsigned int inet_addr_type_table(struct net *net, __be32 addr, u32 tb_id);
unsigned int inet_dev_addr_type(struct net *net, const struct net_device *dev,
				__be32 addr);
unsigned int inet_addr_type_dev_table(struct net *net,
				      const struct net_device *dev,
				      __be32 addr);
void ip_rt_multicast_event(struct in_device *);
int ip_rt_ioctl(struct net *, unsigned int cmd, struct rtentry *rt);
void ip_rt_get_source(u8 *src, struct sk_buff *skb, struct rtable *rt);
struct rtable *rt_dst_alloc(struct net_device *dev,
			     unsigned int flags, u16 type,
			     bool nopolicy, bool noxfrm);
struct rtable *rt_dst_clone(struct net_device *dev, struct rtable *rt);

struct in_ifaddr;
void fib_add_ifaddr(struct in_ifaddr *);
void fib_del_ifaddr(struct in_ifaddr *, struct in_ifaddr *);
void fib_modify_prefix_metric(struct in_ifaddr *ifa, u32 new_metric);

void rt_add_uncached_list(struct rtable *rt);
void rt_del_uncached_list(struct rtable *rt);

int fib_dump_info_fnhe(struct sk_buff *skb, struct netlink_callback *cb,
		       u32 table_id, struct fib_info *fi,
		       int *fa_index, int fa_start, unsigned int flags);

static inline void ip_rt_put(struct rtable *rt)
{
	/* dst_release() accepts a NULL parameter.
	 * We rely on dst being first structure in struct rtable
	 */
	BUILD_BUG_ON(offsetof(struct rtable, dst) != 0);
	dst_release(&rt->dst);
}

#define IPTOS_RT_MASK	(IPTOS_TOS_MASK & ~3)

extern const __u8 ip_tos2prio[16];

static inline char rt_tos2priority(u8 tos)
{
	return ip_tos2prio[IPTOS_TOS(tos)>>1];
}

/* ip_route_connect() and ip_route_newports() work in tandem whilst
 * binding a socket for a new outgoing connection.
 *
 * In order to use IPSEC properly, we must, in the end, have a
 * route that was looked up using all available keys including source
 * and destination ports.
 *
 * However, if a source port needs to be allocated (the user specified
 * a wildcard source port) we need to obtain addressing information
 * in order to perform that allocation.
 *
 * So ip_route_connect() looks up a route using wildcarded source and
 * destination ports in the key, simply so that we can get a pair of
 * addresses to use for port allocation.
 *
 * Later, once the ports are allocated, ip_route_newports() will make
 * another route lookup if needed to make sure we catch any IPSEC
 * rules keyed on the port information.
 *
 * The callers allocate the flow key on their stack, and must pass in
 * the same flowi4 object to both the ip_route_connect() and the
 * ip_route_newports() calls.
 */

static inline void ip_route_connect_init(struct flowi4 *fl4, __be32 dst, __be32 src,
					 u32 tos, int oif, u8 protocol,
					 __be16 sport, __be16 dport,
					 struct sock *sk)
{
	__u8 flow_flags = 0;

	if (inet_sk(sk)->transparent)
		flow_flags |= FLOWI_FLAG_ANYSRC;

	flowi4_init_output(fl4, oif, sk->sk_mark, tos, RT_SCOPE_UNIVERSE,
			   protocol, flow_flags, dst, src, dport, sport,
			   sk->sk_uid);
}

static inline struct rtable *ip_route_connect(struct flowi4 *fl4,
					      __be32 dst, __be32 src, u32 tos,
					      int oif, u8 protocol,
					      __be16 sport, __be16 dport,
					      struct sock *sk)
{
	struct net *net = sock_net(sk);
	struct rtable *rt;

	ip_route_connect_init(fl4, dst, src, tos, oif, protocol,
			      sport, dport, sk);

	if (!dst || !src) {
		rt = __ip_route_output_key(net, fl4);
		if (IS_ERR(rt))
			return rt;
		ip_rt_put(rt);
		flowi4_update_output(fl4, oif, tos, fl4->daddr, fl4->saddr);
	}
	security_sk_classify_flow(sk, flowi4_to_flowi_common(fl4));
	return ip_route_output_flow(net, fl4, sk);
}

static inline struct rtable *ip_route_newports(struct flowi4 *fl4, struct rtable *rt,
					       __be16 orig_sport, __be16 orig_dport,
					       __be16 sport, __be16 dport,
					       struct sock *sk)
{
	if (sport != orig_sport || dport != orig_dport) {
		fl4->fl4_dport = dport;
		fl4->fl4_sport = sport;
		ip_rt_put(rt);
		flowi4_update_output(fl4, sk->sk_bound_dev_if,
				     RT_CONN_FLAGS(sk), fl4->daddr,
				     fl4->saddr);
		security_sk_classify_flow(sk, flowi4_to_flowi_common(fl4));
		return ip_route_output_flow(sock_net(sk), fl4, sk);
	}
	return rt;
}

static inline int inet_iif(const struct sk_buff *skb)
{
	struct rtable *rt = skb_rtable(skb);

	if (rt && rt->rt_iif)
		return rt->rt_iif;

	return skb->skb_iif;
}

static inline int ip4_dst_hoplimit(const struct dst_entry *dst)
{
	int hoplimit = dst_metric_raw(dst, RTAX_HOPLIMIT);
	struct net *net = dev_net(dst->dev);

	if (hoplimit == 0)
		hoplimit = net->ipv4.sysctl_ip_default_ttl;
	return hoplimit;
}

static inline struct neighbour *ip_neigh_gw4(struct net_device *dev,
					     __be32 daddr)
{
	struct neighbour *neigh;

	neigh = __ipv4_neigh_lookup_noref(dev, daddr);
	if (unlikely(!neigh))
		neigh = __neigh_create(&arp_tbl, &daddr, dev, false);

	return neigh;
}

static inline struct neighbour *ip_neigh_for_gw(struct rtable *rt,
						struct sk_buff *skb,
						bool *is_v6gw)
{
	struct net_device *dev = rt->dst.dev;
	struct neighbour *neigh;

	if (likely(rt->rt_gw_family == AF_INET)) {
		neigh = ip_neigh_gw4(dev, rt->rt_gw4);
	} else if (rt->rt_gw_family == AF_INET6) {
		neigh = ip_neigh_gw6(dev, &rt->rt_gw6);
		*is_v6gw = true;
	} else {
		neigh = ip_neigh_gw4(dev, ip_hdr(skb)->daddr);
	}
	return neigh;
}

#endif	/* _ROUTE_H */
