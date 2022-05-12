// SPDX-License-Identifier: GPL-2.0-only
/*
 * INET		An implementation of the TCP/IP protocol suite for the LINUX
 *		operating system.  INET is implemented using the  BSD Socket
 *		interface as the means of communication with the user level.
 *
 *		The Internet Protocol (IP) output module.
 *
 * Authors:	Ross Biro
 *		Fred N. van Kempen, <waltje@uWalt.NL.Mugnet.ORG>
 *		Donald Becker, <becker@super.org>
 *		Alan Cox, <Alan.Cox@linux.org>
 *		Richard Underwood
 *		Stefan Becker, <stefanb@yello.ping.de>
 *		Jorge Cwik, <jorge@laser.satlink.net>
 *		Arnt Gulbrandsen, <agulbra@nvg.unit.no>
 *		Hirokazu Takahashi, <taka@valinux.co.jp>
 *
 *	See ip_input.c for original log
 *
 *	Fixes:
 *		Alan Cox	:	Missing nonblock feature in ip_build_xmit.
 *		Mike Kilburn	:	htons() missing in ip_build_xmit.
 *		Bradford Johnson:	Fix faulty handling of some frames when
 *					no route is found.
 *		Alexander Demenshin:	Missing sk/skb free in ip_queue_xmit
 *					(in case if packet not accepted by
 *					output firewall rules)
 *		Mike McLagan	:	Routing by source
 *		Alexey Kuznetsov:	use new route cache
 *		Andi Kleen:		Fix broken PMTU recovery and remove
 *					some redundant tests.
 *	Vitaly E. Lavrov	:	Transparent proxy revived after year coma.
 *		Andi Kleen	: 	Replace ip_reply with ip_send_reply.
 *		Andi Kleen	:	Split fast and slow ip_build_xmit path
 *					for decreased register pressure on x86
 *					and more readability.
 *		Marc Boucher	:	When call_out_firewall returns FW_QUEUE,
 *					silently drop skb instead of failing with -EPERM.
 *		Detlev Wengorz	:	Copy protocol for fragments.
 *		Hirokazu Takahashi:	HW checksumming for outgoing UDP
 *					datagrams.
 *		Hirokazu Takahashi:	sendfile() on UDP works now.
 */

#include <linux/uaccess.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/highmem.h>
#include <linux/slab.h>

#include <linux/socket.h>
#include <linux/sockios.h>
#include <linux/in.h>
#include <linux/inet.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/proc_fs.h>
#include <linux/stat.h>
#include <linux/init.h>

#include <net/snmp.h>
#include <net/ip.h>
#include <net/protocol.h>
#include <net/route.h>
#include <net/xfrm.h>
#include <linux/skbuff.h>
#include <net/sock.h>
#include <net/arp.h>
#include <net/icmp.h>
#include <net/checksum.h>
#include <net/inetpeer.h>
#include <net/inet_ecn.h>
#include <net/lwtunnel.h>
#include <linux/bpf-cgroup.h>
#include <linux/igmp.h>
#include <linux/netfilter_ipv4.h>
#include <linux/netfilter_bridge.h>
#include <linux/netlink.h>
#include <linux/tcp.h>

static int
ip_fragment(struct net *net, struct sock *sk, struct sk_buff *skb,
	    unsigned int mtu,
	    int (*output)(struct net *, struct sock *, struct sk_buff *));

/* Generate a checksum for an outgoing IP datagram. */
void ip_send_check(struct iphdr *iph)
{
	iph->check = 0;
	iph->check = ip_fast_csum((unsigned char *)iph, iph->ihl);
}
EXPORT_SYMBOL(ip_send_check);

int __ip_local_out(struct net *net, struct sock *sk, struct sk_buff *skb)
{
	struct iphdr *iph = ip_hdr(skb);

	iph->tot_len = htons(skb->len);
	ip_send_check(iph);

	/* if egress device is enslaved to an L3 master device pass the
	 * skb to its handler for processing
	 */
	skb = l3mdev_ip_out(sk, skb);
	if (unlikely(!skb))
		return 0;

	skb->protocol = htons(ETH_P_IP);

	return nf_hook(NFPROTO_IPV4, NF_INET_LOCAL_OUT,
		       net, sk, skb, NULL, skb_dst(skb)->dev,
		       dst_output);
}

int ip_local_out(struct net *net, struct sock *sk, struct sk_buff *skb)
{
	int err;

	err = __ip_local_out(net, sk, skb);
	if (likely(err == 1))
		err = dst_output(net, sk, skb);

	return err;
}
EXPORT_SYMBOL_GPL(ip_local_out);

static inline int ip_select_ttl(struct inet_sock *inet, struct dst_entry *dst)
{
	int ttl = inet->uc_ttl;

	if (ttl < 0)
		ttl = ip4_dst_hoplimit(dst);
	return ttl;
}

/*
 *		Add an ip header to a skbuff and send it out.
 *
 */
int ip_build_and_send_pkt(struct sk_buff *skb, const struct sock *sk,
			  __be32 saddr, __be32 daddr, struct ip_options_rcu *opt,
			  u8 tos)
{
	struct inet_sock *inet = inet_sk(sk);
	struct rtable *rt = skb_rtable(skb);
	struct net *net = sock_net(sk);
	struct iphdr *iph;

	/* Build the IP header. */
	skb_push(skb, sizeof(struct iphdr) + (opt ? opt->opt.optlen : 0));
	skb_reset_network_header(skb);
	iph = ip_hdr(skb);
	iph->version  = 4;
	iph->ihl      = 5;
	iph->tos      = tos;
	iph->ttl      = ip_select_ttl(inet, &rt->dst);
	iph->daddr    = (opt && opt->opt.srr ? opt->opt.faddr : daddr);
	iph->saddr    = saddr;
	iph->protocol = sk->sk_protocol;
	if (ip_dont_fragment(sk, &rt->dst)) {
		iph->frag_off = htons(IP_DF);
		iph->id = 0;
	} else {
		iph->frag_off = 0;
		__ip_select_ident(net, iph, 1);
	}

	if (opt && opt->opt.optlen) {
		iph->ihl += opt->opt.optlen>>2;
		ip_options_build(skb, &opt->opt, daddr, rt, 0);
	}

	skb->priority = sk->sk_priority;
	if (!skb->mark)
		skb->mark = sk->sk_mark;

	/* Send it out. */
	return ip_local_out(net, skb->sk, skb);
}
EXPORT_SYMBOL_GPL(ip_build_and_send_pkt);

static int ip_finish_output2(struct net *net, struct sock *sk, struct sk_buff *skb)
{
	struct dst_entry *dst = skb_dst(skb);
	struct rtable *rt = (struct rtable *)dst;
	struct net_device *dev = dst->dev;
	unsigned int hh_len = LL_RESERVED_SPACE(dev);
	struct neighbour *neigh;
	bool is_v6gw = false;

	if (rt->rt_type == RTN_MULTICAST) {
		IP_UPD_PO_STATS(net, IPSTATS_MIB_OUTMCAST, skb->len);
	} else if (rt->rt_type == RTN_BROADCAST)
		IP_UPD_PO_STATS(net, IPSTATS_MIB_OUTBCAST, skb->len);

	/* Be paranoid, rather than too clever. */
	if (unlikely(skb_headroom(skb) < hh_len && dev->header_ops)) {
		struct sk_buff *skb2;

		skb2 = skb_realloc_headroom(skb, LL_RESERVED_SPACE(dev));
		if (!skb2) {
			kfree_skb(skb);
			return -ENOMEM;
		}
		if (skb->sk)
			skb_set_owner_w(skb2, skb->sk);
		consume_skb(skb);
		skb = skb2;
	}

	if (lwtunnel_xmit_redirect(dst->lwtstate)) {
		int res = lwtunnel_xmit(skb);

		if (res < 0 || res == LWTUNNEL_XMIT_DONE)
			return res;
	}

	rcu_read_lock_bh();
	neigh = ip_neigh_for_gw(rt, skb, &is_v6gw);
	if (!IS_ERR(neigh)) {
		int res;

		sock_confirm_neigh(skb, neigh);
		/* if crossing protocols, can not use the cached header */
		res = neigh_output(neigh, skb, is_v6gw);
		rcu_read_unlock_bh();
		return res;
	}
	rcu_read_unlock_bh();

	net_dbg_ratelimited("%s: No header cache and no neighbour!\n",
			    __func__);
	kfree_skb(skb);
	return -EINVAL;
}

static int ip_finish_output_gso(struct net *net, struct sock *sk,
				struct sk_buff *skb, unsigned int mtu)
{
	struct sk_buff *segs, *nskb;
	netdev_features_t features;
	int ret = 0;

	/* common case: seglen is <= mtu
	 */
	if (skb_gso_validate_network_len(skb, mtu))
		return ip_finish_output2(net, sk, skb);

	/* Slowpath -  GSO segment length exceeds the egress MTU.
	 *
	 * This can happen in several cases:
	 *  - Forwarding of a TCP GRO skb, when DF flag is not set.
	 *  - Forwarding of an skb that arrived on a virtualization interface
	 *    (virtio-net/vhost/tap) with TSO/GSO size set by other network
	 *    stack.
	 *  - Local GSO skb transmitted on an NETIF_F_TSO tunnel stacked over an
	 *    interface with a smaller MTU.
	 *  - Arriving GRO skb (or GSO skb in a virtualized environment) that is
	 *    bridged to a NETIF_F_TSO tunnel stacked over an interface with an
	 *    insufficient MTU.
	 */
	features = netif_skb_features(skb);
	BUILD_BUG_ON(sizeof(*IPCB(skb)) > SKB_GSO_CB_OFFSET);
	segs = skb_gso_segment(skb, features & ~NETIF_F_GSO_MASK);
	if (IS_ERR_OR_NULL(segs)) {
		kfree_skb(skb);
		return -ENOMEM;
	}

	consume_skb(skb);

	skb_list_walk_safe(segs, segs, nskb) {
		int err;

		skb_mark_not_on_list(segs);
		err = ip_fragment(net, sk, segs, mtu, ip_finish_output2);

		if (err && ret == 0)
			ret = err;
	}

	return ret;
}

static int __ip_finish_output(struct net *net, struct sock *sk, struct sk_buff *skb)
{
	unsigned int mtu;

#if defined(CONFIG_NETFILTER) && defined(CONFIG_XFRM)
	/* Policy lookup after SNAT yielded a new policy */
	if (skb_dst(skb)->xfrm) {
		IPCB(skb)->flags |= IPSKB_REROUTED;
		return dst_output(net, sk, skb);
	}
#endif
	mtu = ip_skb_dst_mtu(sk, skb);
	if (skb_is_gso(skb))
		return ip_finish_output_gso(net, sk, skb, mtu);

	if (skb->len > mtu || IPCB(skb)->frag_max_size)
		return ip_fragment(net, sk, skb, mtu, ip_finish_output2);

	return ip_finish_output2(net, sk, skb);
}

static int ip_finish_output(struct net *net, struct sock *sk, struct sk_buff *skb)
{
	int ret;

	ret = BPF_CGROUP_RUN_PROG_INET_EGRESS(sk, skb);
	switch (ret) {
	case NET_XMIT_SUCCESS:
		return __ip_finish_output(net, sk, skb);
	case NET_XMIT_CN:
		return __ip_finish_output(net, sk, skb) ? : ret;
	default:
		kfree_skb(skb);
		return ret;
	}
}

static int ip_mc_finish_output(struct net *net, struct sock *sk,
			       struct sk_buff *skb)
{
	struct rtable *new_rt;
	bool do_cn = false;
	int ret, err;

	ret = BPF_CGROUP_RUN_PROG_INET_EGRESS(sk, skb);
	switch (ret) {
	case NET_XMIT_CN:
		do_cn = true;
		fallthrough;
	case NET_XMIT_SUCCESS:
		break;
	default:
		kfree_skb(skb);
		return ret;
	}

	/* Reset rt_iif so that inet_iif() will return skb->skb_iif. Setting
	 * this to non-zero causes ipi_ifindex in in_pktinfo to be overwritten,
	 * see ipv4_pktinfo_prepare().
	 */
	new_rt = rt_dst_clone(net->loopback_dev, skb_rtable(skb));
	if (new_rt) {
		new_rt->rt_iif = 0;
		skb_dst_drop(skb);
		skb_dst_set(skb, &new_rt->dst);
	}

	err = dev_loopback_xmit(net, sk, skb);
	return (do_cn && err) ? ret : err;
}

int ip_mc_output(struct net *net, struct sock *sk, struct sk_buff *skb)
{
	struct rtable *rt = skb_rtable(skb);
	struct net_device *dev = rt->dst.dev;

	/*
	 *	If the indicated interface is up and running, send the packet.
	 */
	IP_UPD_PO_STATS(net, IPSTATS_MIB_OUT, skb->len);

	skb->dev = dev;
	skb->protocol = htons(ETH_P_IP);

	/*
	 *	Multicasts are looped back for other local users
	 */

	if (rt->rt_flags&RTCF_MULTICAST) {
		if (sk_mc_loop(sk)
#ifdef CONFIG_IP_MROUTE
		/* Small optimization: do not loopback not local frames,
		   which returned after forwarding; they will be  dropped
		   by ip_mr_input in any case.
		   Note, that local frames are looped back to be delivered
		   to local recipients.

		   This check is duplicated in ip_mr_input at the moment.
		 */
		    &&
		    ((rt->rt_flags & RTCF_LOCAL) ||
		     !(IPCB(skb)->flags & IPSKB_FORWARDED))
#endif
		   ) {
			struct sk_buff *newskb = skb_clone(skb, GFP_ATOMIC);
			if (newskb)
				NF_HOOK(NFPROTO_IPV4, NF_INET_POST_ROUTING,
					net, sk, newskb, NULL, newskb->dev,
					ip_mc_finish_output);
		}

		/* Multicasts with ttl 0 must not go beyond the host */

		if (ip_hdr(skb)->ttl == 0) {
			kfree_skb(skb);
			return 0;
		}
	}

	if (rt->rt_flags&RTCF_BROADCAST) {
		struct sk_buff *newskb = skb_clone(skb, GFP_ATOMIC);
		if (newskb)
			NF_HOOK(NFPROTO_IPV4, NF_INET_POST_ROUTING,
				net, sk, newskb, NULL, newskb->dev,
				ip_mc_finish_output);
	}

	return NF_HOOK_COND(NFPROTO_IPV4, NF_INET_POST_ROUTING,
			    net, sk, skb, NULL, skb->dev,
			    ip_finish_output,
			    !(IPCB(skb)->flags & IPSKB_REROUTED));
}

int ip_output(struct net *net, struct sock *sk, struct sk_buff *skb)
{
	struct net_device *dev = skb_dst(skb)->dev, *indev = skb->dev;

	IP_UPD_PO_STATS(net, IPSTATS_MIB_OUT, skb->len);

	skb->dev = dev;
	skb->protocol = htons(ETH_P_IP);

	return NF_HOOK_COND(NFPROTO_IPV4, NF_INET_POST_ROUTING,
			    net, sk, skb, indev, dev,
			    ip_finish_output,
			    !(IPCB(skb)->flags & IPSKB_REROUTED));
}
EXPORT_SYMBOL(ip_output);

/*
 * copy saddr and daddr, possibly using 64bit load/stores
 * Equivalent to :
 *   iph->saddr = fl4->saddr;
 *   iph->daddr = fl4->daddr;
 */
static void ip_copy_addrs(struct iphdr *iph, const struct flowi4 *fl4)
{
	BUILD_BUG_ON(offsetof(typeof(*fl4), daddr) !=
		     offsetof(typeof(*fl4), saddr) + sizeof(fl4->saddr));
	memcpy(&iph->saddr, &fl4->saddr,
	       sizeof(fl4->saddr) + sizeof(fl4->daddr));
}

/* Note: skb->sk can be different from sk, in case of tunnels */
int __ip_queue_xmit(struct sock *sk, struct sk_buff *skb, struct flowi *fl,
		    __u8 tos)
{
	struct inet_sock *inet = inet_sk(sk);
	struct net *net = sock_net(sk);
	struct ip_options_rcu *inet_opt;
	struct flowi4 *fl4;
	struct rtable *rt;
	struct iphdr *iph;
	int res;

	/* Skip all of this if the packet is already routed,
	 * f.e. by something like SCTP.
	 */
	rcu_read_lock();
	inet_opt = rcu_dereference(inet->inet_opt);
	fl4 = &fl->u.ip4;
	rt = skb_rtable(skb);
	if (rt)
		goto packet_routed;

	/* Make sure we can route this packet. */
	rt = (struct rtable *)__sk_dst_check(sk, 0);
	if (!rt) {
		__be32 daddr;

		/* Use correct destination address if we have options. */
		daddr = inet->inet_daddr;
		if (inet_opt && inet_opt->opt.srr)
			daddr = inet_opt->opt.faddr;

		/* If this fails, retransmit mechanism of transport layer will
		 * keep trying until route appears or the connection times
		 * itself out.
		 */
		rt = ip_route_output_ports(net, fl4, sk,
					   daddr, inet->inet_saddr,
					   inet->inet_dport,
					   inet->inet_sport,
					   sk->sk_protocol,
					   RT_CONN_FLAGS_TOS(sk, tos),
					   sk->sk_bound_dev_if);
		if (IS_ERR(rt))
			goto no_route;
		sk_setup_caps(sk, &rt->dst);
	}
	skb_dst_set_noref(skb, &rt->dst);

packet_routed:
	if (inet_opt && inet_opt->opt.is_strictroute && rt->rt_uses_gateway)
		goto no_route;

	/* OK, we know where to send it, allocate and build IP header. */
	skb_push(skb, sizeof(struct iphdr) + (inet_opt ? inet_opt->opt.optlen : 0));
	skb_reset_network_header(skb);
	iph = ip_hdr(skb);
	*((__be16 *)iph) = htons((4 << 12) | (5 << 8) | (tos & 0xff));
	if (ip_dont_fragment(sk, &rt->dst) && !skb->ignore_df)
		iph->frag_off = htons(IP_DF);
	else
		iph->frag_off = 0;
	iph->ttl      = ip_select_ttl(inet, &rt->dst);
	iph->protocol = sk->sk_protocol;
	ip_copy_addrs(iph, fl4);

	/* Transport layer set skb->h.foo itself. */

	if (inet_opt && inet_opt->opt.optlen) {
		iph->ihl += inet_opt->opt.optlen >> 2;
		ip_options_build(skb, &inet_opt->opt, inet->inet_daddr, rt, 0);
	}

	ip_select_ident_segs(net, skb, sk,
			     skb_shinfo(skb)->gso_segs ?: 1);

	/* TODO : should we use skb->sk here instead of sk ? */
	skb->priority = sk->sk_priority;
	skb->mark = sk->sk_mark;

	res = ip_local_out(net, sk, skb);
	rcu_read_unlock();
	return res;

no_route:
	rcu_read_unlock();
	IP_INC_STATS(net, IPSTATS_MIB_OUTNOROUTES);
	kfree_skb(skb);
	return -EHOSTUNREACH;
}
EXPORT_SYMBOL(__ip_queue_xmit);

int ip_queue_xmit(struct sock *sk, struct sk_buff *skb, struct flowi *fl)
{
	return __ip_queue_xmit(sk, skb, fl, inet_sk(sk)->tos);
}
EXPORT_SYMBOL(ip_queue_xmit);

static void ip_copy_metadata(struct sk_buff *to, struct sk_buff *from)
{
	to->pkt_type = from->pkt_type;
	to->priority = from->priority;
	to->protocol = from->protocol;
	to->skb_iif = from->skb_iif;
	skb_dst_drop(to);
	skb_dst_copy(to, from);
	to->dev = from->dev;
	to->mark = from->mark;

	skb_copy_hash(to, from);

#ifdef CONFIG_NET_SCHED
	to->tc_index = from->tc_index;
#endif
	nf_copy(to, from);
	skb_ext_copy(to, from);
#if IS_ENABLED(CONFIG_IP_VS)
	to->ipvs_property = from->ipvs_property;
#endif
	skb_copy_secmark(to, from);
}

static int ip_fragment(struct net *net, struct sock *sk, struct sk_buff *skb,
		       unsigned int mtu,
		       int (*output)(struct net *, struct sock *, struct sk_buff *))
{
	struct iphdr *iph = ip_hdr(skb);

	if ((iph->frag_off & htons(IP_DF)) == 0)
		return ip_do_fragment(net, sk, skb, output);

	if (unlikely(!skb->ignore_df ||
		     (IPCB(skb)->frag_max_size &&
		      IPCB(skb)->frag_max_size > mtu))) {
		IP_INC_STATS(net, IPSTATS_MIB_FRAGFAILS);
		icmp_send(skb, ICMP_DEST_UNREACH, ICMP_FRAG_NEEDED,
			  htonl(mtu));
		kfree_skb(skb);
		return -EMSGSIZE;
	}

	return ip_do_fragment(net, sk, skb, output);
}

void ip_fraglist_init(struct sk_buff *skb, struct iphdr *iph,
		      unsigned int hlen, struct ip_fraglist_iter *iter)
{
	unsigned int first_len = skb_pagelen(skb);

	iter->frag = skb_shinfo(skb)->frag_list;
	skb_frag_list_init(skb);

	iter->offset = 0;
	iter->iph = iph;
	iter->hlen = hlen;

	skb->data_len = first_len - skb_headlen(skb);
	skb->len = first_len;
	iph->tot_len = htons(first_len);
	iph->frag_off = htons(IP_MF);
	ip_send_check(iph);
}
EXPORT_SYMBOL(ip_fraglist_init);

static void ip_fraglist_ipcb_prepare(struct sk_buff *skb,
				     struct ip_fraglist_iter *iter)
{
	struct sk_buff *to = iter->frag;

	/* Copy the flags to each fragment. */
	IPCB(to)->flags = IPCB(skb)->flags;

	if (iter->offset == 0)
		ip_options_fragment(to);
}

void ip_fraglist_prepare(struct sk_buff *skb, struct ip_fraglist_iter *iter)
{
	unsigned int hlen = iter->hlen;
	struct iphdr *iph = iter->iph;
	struct sk_buff *frag;

	frag = iter->frag;
	frag->ip_summed = CHECKSUM_NONE;
	skb_reset_transport_header(frag);
	__skb_push(frag, hlen);
	skb_reset_network_header(frag);
	memcpy(skb_network_header(frag), iph, hlen);
	iter->iph = ip_hdr(frag);
	iph = iter->iph;
	iph->tot_len = htons(frag->len);
	ip_copy_metadata(frag, skb);
	iter->offset += skb->len - hlen;
	iph->frag_off = htons(iter->offset >> 3);
	if (frag->next)
		iph->frag_off |= htons(IP_MF);
	/* Ready, complete checksum */
	ip_send_check(iph);
}
EXPORT_SYMBOL(ip_fraglist_prepare);

void ip_frag_init(struct sk_buff *skb, unsigned int hlen,
		  unsigned int ll_rs, unsigned int mtu, bool DF,
		  struct ip_frag_state *state)
{
	struct iphdr *iph = ip_hdr(skb);

	state->DF = DF;
	state->hlen = hlen;
	state->ll_rs = ll_rs;
	state->mtu = mtu;

	state->left = skb->len - hlen;	/* Space per frame */
	state->ptr = hlen;		/* Where to start from */

	state->offset = (ntohs(iph->frag_off) & IP_OFFSET) << 3;
	state->not_last_frag = iph->frag_off & htons(IP_MF);
}
EXPORT_SYMBOL(ip_frag_init);

static void ip_frag_ipcb(struct sk_buff *from, struct sk_buff *to,
			 bool first_frag, struct ip_frag_state *state)
{
	/* Copy the flags to each fragment. */
	IPCB(to)->flags = IPCB(from)->flags;

	/* ANK: dirty, but effective trick. Upgrade options only if
	 * the segment to be fragmented was THE FIRST (otherwise,
	 * options are already fixed) and make it ONCE
	 * on the initial skb, so that all the following fragments
	 * will inherit fixed options.
	 */
	if (first_frag)
		ip_options_fragment(from);
}

struct sk_buff *ip_frag_next(struct sk_buff *skb, struct ip_frag_state *state)
{
	unsigned int len = state->left;
	struct sk_buff *skb2;
	struct iphdr *iph;

	len = state->left;
	/* IF: it doesn't fit, use 'mtu' - the data space left */
	if (len > state->mtu)
		len = state->mtu;
	/* IF: we are not sending up to and including the packet end
	   then align the next start on an eight byte boundary */
	if (len < state->left)	{
		len &= ~7;
	}

	/* Allocate buffer */
	skb2 = alloc_skb(len + state->hlen + state->ll_rs, GFP_ATOMIC);
	if (!skb2)
		return ERR_PTR(-ENOMEM);

	/*
	 *	Set up data on packet
	 */

	ip_copy_metadata(skb2, skb);
	skb_reserve(skb2, state->ll_rs);
	skb_put(skb2, len + state->hlen);
	skb_reset_network_header(skb2);
	skb2->transport_header = skb2->network_header + state->hlen;

	/*
	 *	Charge the memory for the fragment to any owner
	 *	it might possess
	 */

	if (skb->sk)
		skb_set_owner_w(skb2, skb->sk);

	/*
	 *	Copy the packet header into the new buffer.
	 */

	skb_copy_from_linear_data(skb, skb_network_header(skb2), state->hlen);

	/*
	 *	Copy a block of the IP datagram.
	 */
	if (skb_copy_bits(skb, state->ptr, skb_transport_header(skb2), len))
		BUG();
	state->left -= len;

	/*
	 *	Fill in the new header fields.
	 */
	iph = ip_hdr(skb2);
	iph->frag_off = htons((state->offset >> 3));
	if (state->DF)
		iph->frag_off |= htons(IP_DF);

	/*
	 *	Added AC : If we are fragmenting a fragment that's not the
	 *		   last fragment then keep MF on each bit
	 */
	if (state->left > 0 || state->not_last_frag)
		iph->frag_off |= htons(IP_MF);
	state->ptr += len;
	state->offset += len;

	iph->tot_len = htons(len + state->hlen);

	ip_send_check(iph);

	return skb2;
}
EXPORT_SYMBOL(ip_frag_next);

/*
 *	This IP datagram is too large to be sent in one piece.  Break it up into
 *	smaller pieces (each of size equal to IP header plus
 *	a block of the data of the original IP data part) that will yet fit in a
 *	single device frame, and queue such a frame for sending.
 */

int ip_do_fragment(struct net *net, struct sock *sk, struct sk_buff *skb,
		   int (*output)(struct net *, struct sock *, struct sk_buff *))
{
	struct iphdr *iph;
	struct sk_buff *skb2;
	struct rtable *rt = skb_rtable(skb);
	unsigned int mtu, hlen, ll_rs;
	struct ip_fraglist_iter iter;
	ktime_t tstamp = skb->tstamp;
	struct ip_frag_state state;
	int err = 0;

	/* for offloaded checksums cleanup checksum before fragmentation */
	if (skb->ip_summed == CHECKSUM_PARTIAL &&
	    (err = skb_checksum_help(skb)))
		goto fail;

	/*
	 *	Point into the IP datagram header.
	 */

	iph = ip_hdr(skb);

	mtu = ip_skb_dst_mtu(sk, skb);
	if (IPCB(skb)->frag_max_size && IPCB(skb)->frag_max_size < mtu)
		mtu = IPCB(skb)->frag_max_size;

	/*
	 *	Setup starting values.
	 */

	hlen = iph->ihl * 4;
	mtu = mtu - hlen;	/* Size of data space */
	IPCB(skb)->flags |= IPSKB_FRAG_COMPLETE;
	ll_rs = LL_RESERVED_SPACE(rt->dst.dev);

	/* When frag_list is given, use it. First, check its validity:
	 * some transformers could create wrong frag_list or break existing
	 * one, it is not prohibited. In this case fall back to copying.
	 *
	 * LATER: this step can be merged to real generation of fragments,
	 * we can switch to copy when see the first bad fragment.
	 */
	if (skb_has_frag_list(skb)) {
		struct sk_buff *frag, *frag2;
		unsigned int first_len = skb_pagelen(skb);

		if (first_len - hlen > mtu ||
		    ((first_len - hlen) & 7) ||
		    ip_is_fragment(iph) ||
		    skb_cloned(skb) ||
		    skb_headroom(skb) < ll_rs)
			goto slow_path;

		skb_walk_frags(skb, frag) {
			/* Correct geometry. */
			if (frag->len > mtu ||
			    ((frag->len & 7) && frag->next) ||
			    skb_headroom(frag) < hlen + ll_rs)
				goto slow_path_clean;

			/* Partially cloned skb? */
			if (skb_shared(frag))
				goto slow_path_clean;

			BUG_ON(frag->sk);
			if (skb->sk) {
				frag->sk = skb->sk;
				frag->destructor = sock_wfree;
			}
			skb->truesize -= frag->truesize;
		}

		/* Everything is OK. Generate! */
		ip_fraglist_init(skb, iph, hlen, &iter);

		for (;;) {
			/* Prepare header of the next frame,
			 * before previous one went down. */
			if (iter.frag) {
				ip_fraglist_ipcb_prepare(skb, &iter);
				ip_fraglist_prepare(skb, &iter);
			}

			skb->tstamp = tstamp;
			err = output(net, sk, skb);

			if (!err)
				IP_INC_STATS(net, IPSTATS_MIB_FRAGCREATES);
			if (err || !iter.frag)
				break;

			skb = ip_fraglist_next(&iter);
		}

		if (err == 0) {
			IP_INC_STATS(net, IPSTATS_MIB_FRAGOKS);
			return 0;
		}

		kfree_skb_list(iter.frag);

		IP_INC_STATS(net, IPSTATS_MIB_FRAGFAILS);
		return err;

slow_path_clean:
		skb_walk_frags(skb, frag2) {
			if (frag2 == frag)
				break;
			frag2->sk = NULL;
			frag2->destructor = NULL;
			skb->truesize += frag2->truesize;
		}
	}

slow_path:
	/*
	 *	Fragment the datagram.
	 */

	ip_frag_init(skb, hlen, ll_rs, mtu, IPCB(skb)->flags & IPSKB_FRAG_PMTU,
		     &state);

	/*
	 *	Keep copying data until we run out.
	 */

	while (state.left > 0) {
		bool first_frag = (state.offset == 0);

		skb2 = ip_frag_next(skb, &state);
		if (IS_ERR(skb2)) {
			err = PTR_ERR(skb2);
			goto fail;
		}
		ip_frag_ipcb(skb, skb2, first_frag, &state);

		/*
		 *	Put this fragment into the sending queue.
		 */
		skb2->tstamp = tstamp;
		err = output(net, sk, skb2);
		if (err)
			goto fail;

		IP_INC_STATS(net, IPSTATS_MIB_FRAGCREATES);
	}
	consume_skb(skb);
	IP_INC_STATS(net, IPSTATS_MIB_FRAGOKS);
	return err;

fail:
	kfree_skb(skb);
	IP_INC_STATS(net, IPSTATS_MIB_FRAGFAILS);
	return err;
}
EXPORT_SYMBOL(ip_do_fragment);

int
ip_generic_getfrag(void *from, char *to, int offset, int len, int odd, struct sk_buff *skb)
{
	struct msghdr *msg = from;

	if (skb->ip_summed == CHECKSUM_PARTIAL) {
		if (!copy_from_iter_full(to, len, &msg->msg_iter))
			return -EFAULT;
	} else {
		__wsum csum = 0;
		if (!csum_and_copy_from_iter_full(to, len, &csum, &msg->msg_iter))
			return -EFAULT;
		skb->csum = csum_block_add(skb->csum, csum, odd);
	}
	return 0;
}
EXPORT_SYMBOL(ip_generic_getfrag);

static inline __wsum
csum_page(struct page *page, int offset, int copy)
{
	char *kaddr;
	__wsum csum;
	kaddr = kmap(page);
	csum = csum_partial(kaddr + offset, copy, 0);
	kunmap(page);
	return csum;
}

static int __ip_append_data(struct sock *sk,
			    struct flowi4 *fl4,
			    struct sk_buff_head *queue,
			    struct inet_cork *cork,
			    struct page_frag *pfrag,
			    int getfrag(void *from, char *to, int offset,
					int len, int odd, struct sk_buff *skb),
			    void *from, int length, int transhdrlen,
			    unsigned int flags)
{
	struct inet_sock *inet = inet_sk(sk);
	struct ubuf_info *uarg = NULL;
	struct sk_buff *skb;

	struct ip_options *opt = cork->opt;
	int hh_len;
	int exthdrlen;
	int mtu;
	int copy;
	int err;
	int offset = 0;
	unsigned int maxfraglen, fragheaderlen, maxnonfragsize;
	int csummode = CHECKSUM_NONE;
	struct rtable *rt = (struct rtable *)cork->dst;
	unsigned int wmem_alloc_delta = 0;
	bool paged, extra_uref = false;
	u32 tskey = 0;

	skb = skb_peek_tail(queue);

	exthdrlen = !skb ? rt->dst.header_len : 0;
	mtu = cork->gso_size ? IP_MAX_MTU : cork->fragsize;
	paged = !!cork->gso_size;

	if (cork->tx_flags & SKBTX_ANY_SW_TSTAMP &&
	    sk->sk_tsflags & SOF_TIMESTAMPING_OPT_ID)
		tskey = sk->sk_tskey++;

	hh_len = LL_RESERVED_SPACE(rt->dst.dev);

	fragheaderlen = sizeof(struct iphdr) + (opt ? opt->optlen : 0);
	maxfraglen = ((mtu - fragheaderlen) & ~7) + fragheaderlen;
	maxnonfragsize = ip_sk_ignore_df(sk) ? IP_MAX_MTU : mtu;

	if (cork->length + length > maxnonfragsize - fragheaderlen) {
		ip_local_error(sk, EMSGSIZE, fl4->daddr, inet->inet_dport,
			       mtu - (opt ? opt->optlen : 0));
		return -EMSGSIZE;
	}

	/*
	 * transhdrlen > 0 means that this is the first fragment and we wish
	 * it won't be fragmented in the future.
	 */
	if (transhdrlen &&
	    length + fragheaderlen <= mtu &&
	    rt->dst.dev->features & (NETIF_F_HW_CSUM | NETIF_F_IP_CSUM) &&
	    (!(flags & MSG_MORE) || cork->gso_size) &&
	    (!exthdrlen || (rt->dst.dev->features & NETIF_F_HW_ESP_TX_CSUM)))
		csummode = CHECKSUM_PARTIAL;

	if (flags & MSG_ZEROCOPY && length && sock_flag(sk, SOCK_ZEROCOPY)) {
		uarg = msg_zerocopy_realloc(sk, length, skb_zcopy(skb));
		if (!uarg)
			return -ENOBUFS;
		extra_uref = !skb_zcopy(skb);	/* only ref on new uarg */
		if (rt->dst.dev->features & NETIF_F_SG &&
		    csummode == CHECKSUM_PARTIAL) {
			paged = true;
		} else {
			uarg->zerocopy = 0;
			skb_zcopy_set(skb, uarg, &extra_uref);
		}
	}

	cork->length += length;

	/* So, what's going on in the loop below?
	 *
	 * We use calculated fragment length to generate chained skb,
	 * each of segments is IP fragment ready for sending to network after
	 * adding appropriate IP header.
	 */

	if (!skb)
		goto alloc_new_skb;

	while (length > 0) {
		/* Check if the remaining data fits into current packet. */
		copy = mtu - skb->len;
		if (copy < length)
			copy = maxfraglen - skb->len;
		if (copy <= 0) {
			char *data;
			unsigned int datalen;
			unsigned int fraglen;
			unsigned int fraggap;
			unsigned int alloclen;
			unsigned int pagedlen;
			struct sk_buff *skb_prev;
alloc_new_skb:
			skb_prev = skb;
			if (skb_prev)
				fraggap = skb_prev->len - maxfraglen;
			else
				fraggap = 0;

			/*
			 * If remaining data exceeds the mtu,
			 * we know we need more fragment(s).
			 */
			datalen = length + fraggap;
			if (datalen > mtu - fragheaderlen)
				datalen = maxfraglen - fragheaderlen;
			fraglen = datalen + fragheaderlen;
			pagedlen = 0;

			if ((flags & MSG_MORE) &&
			    !(rt->dst.dev->features&NETIF_F_SG))
				alloclen = mtu;
			else if (!paged)
				alloclen = fraglen;
			else {
				alloclen = min_t(int, fraglen, MAX_HEADER);
				pagedlen = fraglen - alloclen;
			}

			alloclen += exthdrlen;

			/* The last fragment gets additional space at tail.
			 * Note, with MSG_MORE we overallocate on fragments,
			 * because we have no idea what fragment will be
			 * the last.
			 */
			if (datalen == length + fraggap)
				alloclen += rt->dst.trailer_len;

			if (transhdrlen) {
				skb = sock_alloc_send_skb(sk,
						alloclen + hh_len + 15,
						(flags & MSG_DONTWAIT), &err);
			} else {
				skb = NULL;
				if (refcount_read(&sk->sk_wmem_alloc) + wmem_alloc_delta <=
				    2 * sk->sk_sndbuf)
					skb = alloc_skb(alloclen + hh_len + 15,
							sk->sk_allocation);
				if (unlikely(!skb))
					err = -ENOBUFS;
			}
			if (!skb)
				goto error;

			/*
			 *	Fill in the control structures
			 */
			skb->ip_summed = csummode;
			skb->csum = 0;
			skb_reserve(skb, hh_len);

			/*
			 *	Find where to start putting bytes.
			 */
			data = skb_put(skb, fraglen + exthdrlen - pagedlen);
			skb_set_network_header(skb, exthdrlen);
			skb->transport_header = (skb->network_header +
						 fragheaderlen);
			data += fragheaderlen + exthdrlen;

			if (fraggap) {
				skb->csum = skb_copy_and_csum_bits(
					skb_prev, maxfraglen,
					data + transhdrlen, fraggap);
				skb_prev->csum = csum_sub(skb_prev->csum,
							  skb->csum);
				data += fraggap;
				pskb_trim_unique(skb_prev, maxfraglen);
			}

			copy = datalen - transhdrlen - fraggap - pagedlen;
			if (copy > 0 && getfrag(from, data + transhdrlen, offset, copy, fraggap, skb) < 0) {
				err = -EFAULT;
				kfree_skb(skb);
				goto error;
			}

			offset += copy;
			length -= copy + transhdrlen;
			transhdrlen = 0;
			exthdrlen = 0;
			csummode = CHECKSUM_NONE;

			/* only the initial fragment is time stamped */
			skb_shinfo(skb)->tx_flags = cork->tx_flags;
			cork->tx_flags = 0;
			skb_shinfo(skb)->tskey = tskey;
			tskey = 0;
			skb_zcopy_set(skb, uarg, &extra_uref);

			if ((flags & MSG_CONFIRM) && !skb_prev)
				skb_set_dst_pending_confirm(skb, 1);

			/*
			 * Put the packet on the pending queue.
			 */
			if (!skb->destructor) {
				skb->destructor = sock_wfree;
				skb->sk = sk;
				wmem_alloc_delta += skb->truesize;
			}
			__skb_queue_tail(queue, skb);
			continue;
		}

		if (copy > length)
			copy = length;

		if (!(rt->dst.dev->features&NETIF_F_SG) &&
		    skb_tailroom(skb) >= copy) {
			unsigned int off;

			off = skb->len;
			if (getfrag(from, skb_put(skb, copy),
					offset, copy, off, skb) < 0) {
				__skb_trim(skb, off);
				err = -EFAULT;
				goto error;
			}
		} else if (!uarg || !uarg->zerocopy) {
			int i = skb_shinfo(skb)->nr_frags;

			err = -ENOMEM;
			if (!sk_page_frag_refill(sk, pfrag))
				goto error;

			if (!skb_can_coalesce(skb, i, pfrag->page,
					      pfrag->offset)) {
				err = -EMSGSIZE;
				if (i == MAX_SKB_FRAGS)
					goto error;

				__skb_fill_page_desc(skb, i, pfrag->page,
						     pfrag->offset, 0);
				skb_shinfo(skb)->nr_frags = ++i;
				get_page(pfrag->page);
			}
			copy = min_t(int, copy, pfrag->size - pfrag->offset);
			if (getfrag(from,
				    page_address(pfrag->page) + pfrag->offset,
				    offset, copy, skb->len, skb) < 0)
				goto error_efault;

			pfrag->offset += copy;
			skb_frag_size_add(&skb_shinfo(skb)->frags[i - 1], copy);
			skb->len += copy;
			skb->data_len += copy;
			skb->truesize += copy;
			wmem_alloc_delta += copy;
		} else {
			err = skb_zerocopy_iter_dgram(skb, from, copy);
			if (err < 0)
				goto error;
		}
		offset += copy;
		length -= copy;
	}

	if (wmem_alloc_delta)
		refcount_add(wmem_alloc_delta, &sk->sk_wmem_alloc);
	return 0;

error_efault:
	err = -EFAULT;
error:
	net_zcopy_put_abort(uarg, extra_uref);
	cork->length -= length;
	IP_INC_STATS(sock_net(sk), IPSTATS_MIB_OUTDISCARDS);
	refcount_add(wmem_alloc_delta, &sk->sk_wmem_alloc);
	return err;
}

static int ip_setup_cork(struct sock *sk, struct inet_cork *cork,
			 struct ipcm_cookie *ipc, struct rtable **rtp)
{
	struct ip_options_rcu *opt;
	struct rtable *rt;

	rt = *rtp;
	if (unlikely(!rt))
		return -EFAULT;

	/*
	 * setup for corking.
	 */
	opt = ipc->opt;
	if (opt) {
		if (!cork->opt) {
			cork->opt = kmalloc(sizeof(struct ip_options) + 40,
					    sk->sk_allocation);
			if (unlikely(!cork->opt))
				return -ENOBUFS;
		}
		memcpy(cork->opt, &opt->opt, sizeof(struct ip_options) + opt->opt.optlen);
		cork->flags |= IPCORK_OPT;
		cork->addr = ipc->addr;
	}

	cork->fragsize = ip_sk_use_pmtu(sk) ?
			 dst_mtu(&rt->dst) : READ_ONCE(rt->dst.dev->mtu);

	if (!inetdev_valid_mtu(cork->fragsize))
		return -ENETUNREACH;

	cork->gso_size = ipc->gso_size;

	cork->dst = &rt->dst;
	/* We stole this route, caller should not release it. */
	*rtp = NULL;

	cork->length = 0;
	cork->ttl = ipc->ttl;
	cork->tos = ipc->tos;
	cork->mark = ipc->sockc.mark;
	cork->priority = ipc->priority;
	cork->transmit_time = ipc->sockc.transmit_time;
	cork->tx_flags = 0;
	sock_tx_timestamp(sk, ipc->sockc.tsflags, &cork->tx_flags);

	return 0;
}

/*
 *	ip_append_data() and ip_append_page() can make one large IP datagram
 *	from many pieces of data. Each pieces will be holded on the socket
 *	until ip_push_pending_frames() is called. Each piece can be a page
 *	or non-page data.
 *
 *	Not only UDP, other transport protocols - e.g. raw sockets - can use
 *	this interface potentially.
 *
 *	LATER: length must be adjusted by pad at tail, when it is required.
 */
int ip_append_data(struct sock *sk, struct flowi4 *fl4,
		   int getfrag(void *from, char *to, int offset, int len,
			       int odd, struct sk_buff *skb),
		   void *from, int length, int transhdrlen,
		   struct ipcm_cookie *ipc, struct rtable **rtp,
		   unsigned int flags)
{
	struct inet_sock *inet = inet_sk(sk);
	int err;

	if (flags&MSG_PROBE)
		return 0;

	if (skb_queue_empty(&sk->sk_write_queue)) {
		err = ip_setup_cork(sk, &inet->cork.base, ipc, rtp);
		if (err)
			return err;
	} else {
		transhdrlen = 0;
	}

	return __ip_append_data(sk, fl4, &sk->sk_write_queue, &inet->cork.base,
				sk_page_frag(sk), getfrag,
				from, length, transhdrlen, flags);
}

ssize_t	ip_append_page(struct sock *sk, struct flowi4 *fl4, struct page *page,
		       int offset, size_t size, int flags)
{
	struct inet_sock *inet = inet_sk(sk);
	struct sk_buff *skb;
	struct rtable *rt;
	struct ip_options *opt = NULL;
	struct inet_cork *cork;
	int hh_len;
	int mtu;
	int len;
	int err;
	unsigned int maxfraglen, fragheaderlen, fraggap, maxnonfragsize;

	if (inet->hdrincl)
		return -EPERM;

	if (flags&MSG_PROBE)
		return 0;

	if (skb_queue_empty(&sk->sk_write_queue))
		return -EINVAL;

	cork = &inet->cork.base;
	rt = (struct rtable *)cork->dst;
	if (cork->flags & IPCORK_OPT)
		opt = cork->opt;

	if (!(rt->dst.dev->features & NETIF_F_SG))
		return -EOPNOTSUPP;

	hh_len = LL_RESERVED_SPACE(rt->dst.dev);
	mtu = cork->gso_size ? IP_MAX_MTU : cork->fragsize;

	fragheaderlen = sizeof(struct iphdr) + (opt ? opt->optlen : 0);
	maxfraglen = ((mtu - fragheaderlen) & ~7) + fragheaderlen;
	maxnonfragsize = ip_sk_ignore_df(sk) ? 0xFFFF : mtu;

	if (cork->length + size > maxnonfragsize - fragheaderlen) {
		ip_local_error(sk, EMSGSIZE, fl4->daddr, inet->inet_dport,
			       mtu - (opt ? opt->optlen : 0));
		return -EMSGSIZE;
	}

	skb = skb_peek_tail(&sk->sk_write_queue);
	if (!skb)
		return -EINVAL;

	cork->length += size;

	while (size > 0) {
		/* Check if the remaining data fits into current packet. */
		len = mtu - skb->len;
		if (len < size)
			len = maxfraglen - skb->len;

		if (len <= 0) {
			struct sk_buff *skb_prev;
			int alloclen;

			skb_prev = skb;
			fraggap = skb_prev->len - maxfraglen;

			alloclen = fragheaderlen + hh_len + fraggap + 15;
			skb = sock_wmalloc(sk, alloclen, 1, sk->sk_allocation);
			if (unlikely(!skb)) {
				err = -ENOBUFS;
				goto error;
			}

			/*
			 *	Fill in the control structures
			 */
			skb->ip_summed = CHECKSUM_NONE;
			skb->csum = 0;
			skb_reserve(skb, hh_len);

			/*
			 *	Find where to start putting bytes.
			 */
			skb_put(skb, fragheaderlen + fraggap);
			skb_reset_network_header(skb);
			skb->transport_header = (skb->network_header +
						 fragheaderlen);
			if (fraggap) {
				skb->csum = skb_copy_and_csum_bits(skb_prev,
								   maxfraglen,
						    skb_transport_header(skb),
								   fraggap);
				skb_prev->csum = csum_sub(skb_prev->csum,
							  skb->csum);
				pskb_trim_unique(skb_prev, maxfraglen);
			}

			/*
			 * Put the packet on the pending queue.
			 */
			__skb_queue_tail(&sk->sk_write_queue, skb);
			continue;
		}

		if (len > size)
			len = size;

		if (skb_append_pagefrags(skb, page, offset, len)) {
			err = -EMSGSIZE;
			goto error;
		}

		if (skb->ip_summed == CHECKSUM_NONE) {
			__wsum csum;
			csum = csum_page(page, offset, len);
			skb->csum = csum_block_add(skb->csum, csum, skb->len);
		}

		skb->len += len;
		skb->data_len += len;
		skb->truesize += len;
		refcount_add(len, &sk->sk_wmem_alloc);
		offset += len;
		size -= len;
	}
	return 0;

error:
	cork->length -= size;
	IP_INC_STATS(sock_net(sk), IPSTATS_MIB_OUTDISCARDS);
	return err;
}

static void ip_cork_release(struct inet_cork *cork)
{
	cork->flags &= ~IPCORK_OPT;
	kfree(cork->opt);
	cork->opt = NULL;
	dst_release(cork->dst);
	cork->dst = NULL;
}

/*
 *	Combined all pending IP fragments on the socket as one IP datagram
 *	and push them out.
 */
struct sk_buff *__ip_make_skb(struct sock *sk,
			      struct flowi4 *fl4,
			      struct sk_buff_head *queue,
			      struct inet_cork *cork)
{
	struct sk_buff *skb, *tmp_skb;
	struct sk_buff **tail_skb;
	struct inet_sock *inet = inet_sk(sk);
	struct net *net = sock_net(sk);
	struct ip_options *opt = NULL;
	struct rtable *rt = (struct rtable *)cork->dst;
	struct iphdr *iph;
	__be16 df = 0;
	__u8 ttl;

	skb = __skb_dequeue(queue);
	if (!skb)
		goto out;
	tail_skb = &(skb_shinfo(skb)->frag_list);

	/* move skb->data to ip header from ext header */
	if (skb->data < skb_network_header(skb))
		__skb_pull(skb, skb_network_offset(skb));
	while ((tmp_skb = __skb_dequeue(queue)) != NULL) {
		__skb_pull(tmp_skb, skb_network_header_len(skb));
		*tail_skb = tmp_skb;
		tail_skb = &(tmp_skb->next);
		skb->len += tmp_skb->len;
		skb->data_len += tmp_skb->len;
		skb->truesize += tmp_skb->truesize;
		tmp_skb->destructor = NULL;
		tmp_skb->sk = NULL;
	}

	/* Unless user demanded real pmtu discovery (IP_PMTUDISC_DO), we allow
	 * to fragment the frame generated here. No matter, what transforms
	 * how transforms change size of the packet, it will come out.
	 */
	skb->ignore_df = ip_sk_ignore_df(sk);

	/* DF bit is set when we want to see DF on outgoing frames.
	 * If ignore_df is set too, we still allow to fragment this frame
	 * locally. */
	if (inet->pmtudisc == IP_PMTUDISC_DO ||
	    inet->pmtudisc == IP_PMTUDISC_PROBE ||
	    (skb->len <= dst_mtu(&rt->dst) &&
	     ip_dont_fragment(sk, &rt->dst)))
		df = htons(IP_DF);

	if (cork->flags & IPCORK_OPT)
		opt = cork->opt;

	if (cork->ttl != 0)
		ttl = cork->ttl;
	else if (rt->rt_type == RTN_MULTICAST)
		ttl = inet->mc_ttl;
	else
		ttl = ip_select_ttl(inet, &rt->dst);

	iph = ip_hdr(skb);
	iph->version = 4;
	iph->ihl = 5;
	iph->tos = (cork->tos != -1) ? cork->tos : inet->tos;
	iph->frag_off = df;
	iph->ttl = ttl;
	iph->protocol = sk->sk_protocol;
	ip_copy_addrs(iph, fl4);
	ip_select_ident(net, skb, sk);

	if (opt) {
		iph->ihl += opt->optlen >> 2;
		ip_options_build(skb, opt, cork->addr, rt, 0);
	}

	skb->priority = (cork->tos != -1) ? cork->priority: sk->sk_priority;
	skb->mark = cork->mark;
	skb->tstamp = cork->transmit_time;
	/*
	 * Steal rt from cork.dst to avoid a pair of atomic_inc/atomic_dec
	 * on dst refcount
	 */
	cork->dst = NULL;
	skb_dst_set(skb, &rt->dst);

	if (iph->protocol == IPPROTO_ICMP)
		icmp_out_count(net, ((struct icmphdr *)
			skb_transport_header(skb))->type);

	ip_cork_release(cork);
out:
	return skb;
}

int ip_send_skb(struct net *net, struct sk_buff *skb)
{
	int err;

	err = ip_local_out(net, skb->sk, skb);
	if (err) {
		if (err > 0)
			err = net_xmit_errno(err);
		if (err)
			IP_INC_STATS(net, IPSTATS_MIB_OUTDISCARDS);
	}

	return err;
}

int ip_push_pending_frames(struct sock *sk, struct flowi4 *fl4)
{
	struct sk_buff *skb;

	skb = ip_finish_skb(sk, fl4);
	if (!skb)
		return 0;

	/* Netfilter gets whole the not fragmented skb. */
	return ip_send_skb(sock_net(sk), skb);
}

/*
 *	Throw away all pending data on the socket.
 */
static void __ip_flush_pending_frames(struct sock *sk,
				      struct sk_buff_head *queue,
				      struct inet_cork *cork)
{
	struct sk_buff *skb;

	while ((skb = __skb_dequeue_tail(queue)) != NULL)
		kfree_skb(skb);

	ip_cork_release(cork);
}

void ip_flush_pending_frames(struct sock *sk)
{
	__ip_flush_pending_frames(sk, &sk->sk_write_queue, &inet_sk(sk)->cork.base);
}

struct sk_buff *ip_make_skb(struct sock *sk,
			    struct flowi4 *fl4,
			    int getfrag(void *from, char *to, int offset,
					int len, int odd, struct sk_buff *skb),
			    void *from, int length, int transhdrlen,
			    struct ipcm_cookie *ipc, struct rtable **rtp,
			    struct inet_cork *cork, unsigned int flags)
{
	struct sk_buff_head queue;
	int err;

	if (flags & MSG_PROBE)
		return NULL;

	__skb_queue_head_init(&queue);

	cork->flags = 0;
	cork->addr = 0;
	cork->opt = NULL;
	err = ip_setup_cork(sk, cork, ipc, rtp);
	if (err)
		return ERR_PTR(err);

	err = __ip_append_data(sk, fl4, &queue, cork,
			       &current->task_frag, getfrag,
			       from, length, transhdrlen, flags);
	if (err) {
		__ip_flush_pending_frames(sk, &queue, cork);
		return ERR_PTR(err);
	}

	return __ip_make_skb(sk, fl4, &queue, cork);
}

/*
 *	Fetch data from kernel space and fill in checksum if needed.
 */
static int ip_reply_glue_bits(void *dptr, char *to, int offset,
			      int len, int odd, struct sk_buff *skb)
{
	__wsum csum;

	csum = csum_partial_copy_nocheck(dptr+offset, to, len);
	skb->csum = csum_block_add(skb->csum, csum, odd);
	return 0;
}

/*
 *	Generic function to send a packet as reply to another packet.
 *	Used to send some TCP resets/acks so far.
 */
void ip_send_unicast_reply(struct sock *sk, struct sk_buff *skb,
			   const struct ip_options *sopt,
			   __be32 daddr, __be32 saddr,
			   const struct ip_reply_arg *arg,
			   unsigned int len, u64 transmit_time)
{
	struct ip_options_data replyopts;
	struct ipcm_cookie ipc;
	struct flowi4 fl4;
	struct rtable *rt = skb_rtable(skb);
	struct net *net = sock_net(sk);
	struct sk_buff *nskb;
	int err;
	int oif;

	if (__ip_options_echo(net, &replyopts.opt.opt, skb, sopt))
		return;

	ipcm_init(&ipc);
	ipc.addr = daddr;
	ipc.sockc.transmit_time = transmit_time;

	if (replyopts.opt.opt.optlen) {
		ipc.opt = &replyopts.opt;

		if (replyopts.opt.opt.srr)
			daddr = replyopts.opt.opt.faddr;
	}

	oif = arg->bound_dev_if;
	if (!oif && netif_index_is_l3_master(net, skb->skb_iif))
		oif = skb->skb_iif;

	flowi4_init_output(&fl4, oif,
			   IP4_REPLY_MARK(net, skb->mark) ?: sk->sk_mark,
			   RT_TOS(arg->tos),
			   RT_SCOPE_UNIVERSE, ip_hdr(skb)->protocol,
			   ip_reply_arg_flowi_flags(arg),
			   daddr, saddr,
			   tcp_hdr(skb)->source, tcp_hdr(skb)->dest,
			   arg->uid);
	security_skb_classify_flow(skb, flowi4_to_flowi_common(&fl4));
	rt = ip_route_output_key(net, &fl4);
	if (IS_ERR(rt))
		return;

	inet_sk(sk)->tos = arg->tos & ~INET_ECN_MASK;

	sk->sk_protocol = ip_hdr(skb)->protocol;
	sk->sk_bound_dev_if = arg->bound_dev_if;
	sk->sk_sndbuf = sysctl_wmem_default;
	ipc.sockc.mark = fl4.flowi4_mark;
	err = ip_append_data(sk, &fl4, ip_reply_glue_bits, arg->iov->iov_base,
			     len, 0, &ipc, &rt, MSG_DONTWAIT);
	if (unlikely(err)) {
		ip_flush_pending_frames(sk);
		goto out;
	}

	nskb = skb_peek(&sk->sk_write_queue);
	if (nskb) {
		if (arg->csumoffset >= 0)
			*((__sum16 *)skb_transport_header(nskb) +
			  arg->csumoffset) = csum_fold(csum_add(nskb->csum,
								arg->csum));
		nskb->ip_summed = CHECKSUM_NONE;
		ip_push_pending_frames(sk, &fl4);
	}
out:
	ip_rt_put(rt);
}

void __init ip_init(void)
{
	ip_rt_init();
	inet_initpeers();

#if defined(CONFIG_IP_MULTICAST)
	igmp_mc_init();
#endif
}
