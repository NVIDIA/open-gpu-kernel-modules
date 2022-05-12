// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *	common UDP/RAW code
 *	Linux INET6 implementation
 *
 *	Authors:
 *	Pedro Roque		<roque@di.fc.ul.pt>
 */

#include <linux/capability.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/socket.h>
#include <linux/sockios.h>
#include <linux/in6.h>
#include <linux/ipv6.h>
#include <linux/route.h>
#include <linux/slab.h>
#include <linux/export.h>
#include <linux/icmp.h>

#include <net/ipv6.h>
#include <net/ndisc.h>
#include <net/addrconf.h>
#include <net/transp_v6.h>
#include <net/ip6_route.h>
#include <net/tcp_states.h>
#include <net/dsfield.h>
#include <net/sock_reuseport.h>

#include <linux/errqueue.h>
#include <linux/uaccess.h>

static bool ipv6_mapped_addr_any(const struct in6_addr *a)
{
	return ipv6_addr_v4mapped(a) && (a->s6_addr32[3] == 0);
}

static void ip6_datagram_flow_key_init(struct flowi6 *fl6, struct sock *sk)
{
	struct inet_sock *inet = inet_sk(sk);
	struct ipv6_pinfo *np = inet6_sk(sk);

	memset(fl6, 0, sizeof(*fl6));
	fl6->flowi6_proto = sk->sk_protocol;
	fl6->daddr = sk->sk_v6_daddr;
	fl6->saddr = np->saddr;
	fl6->flowi6_oif = sk->sk_bound_dev_if;
	fl6->flowi6_mark = sk->sk_mark;
	fl6->fl6_dport = inet->inet_dport;
	fl6->fl6_sport = inet->inet_sport;
	fl6->flowlabel = np->flow_label;
	fl6->flowi6_uid = sk->sk_uid;

	if (!fl6->flowi6_oif)
		fl6->flowi6_oif = np->sticky_pktinfo.ipi6_ifindex;

	if (!fl6->flowi6_oif && ipv6_addr_is_multicast(&fl6->daddr))
		fl6->flowi6_oif = np->mcast_oif;

	security_sk_classify_flow(sk, flowi6_to_flowi_common(fl6));
}

int ip6_datagram_dst_update(struct sock *sk, bool fix_sk_saddr)
{
	struct ip6_flowlabel *flowlabel = NULL;
	struct in6_addr *final_p, final;
	struct ipv6_txoptions *opt;
	struct dst_entry *dst;
	struct inet_sock *inet = inet_sk(sk);
	struct ipv6_pinfo *np = inet6_sk(sk);
	struct flowi6 fl6;
	int err = 0;

	if (np->sndflow && (np->flow_label & IPV6_FLOWLABEL_MASK)) {
		flowlabel = fl6_sock_lookup(sk, np->flow_label);
		if (IS_ERR(flowlabel))
			return -EINVAL;
	}
	ip6_datagram_flow_key_init(&fl6, sk);

	rcu_read_lock();
	opt = flowlabel ? flowlabel->opt : rcu_dereference(np->opt);
	final_p = fl6_update_dst(&fl6, opt, &final);
	rcu_read_unlock();

	dst = ip6_dst_lookup_flow(sock_net(sk), sk, &fl6, final_p);
	if (IS_ERR(dst)) {
		err = PTR_ERR(dst);
		goto out;
	}

	if (fix_sk_saddr) {
		if (ipv6_addr_any(&np->saddr))
			np->saddr = fl6.saddr;

		if (ipv6_addr_any(&sk->sk_v6_rcv_saddr)) {
			sk->sk_v6_rcv_saddr = fl6.saddr;
			inet->inet_rcv_saddr = LOOPBACK4_IPV6;
			if (sk->sk_prot->rehash)
				sk->sk_prot->rehash(sk);
		}
	}

	ip6_sk_dst_store_flow(sk, dst, &fl6);

out:
	fl6_sock_release(flowlabel);
	return err;
}

void ip6_datagram_release_cb(struct sock *sk)
{
	struct dst_entry *dst;

	if (ipv6_addr_v4mapped(&sk->sk_v6_daddr))
		return;

	rcu_read_lock();
	dst = __sk_dst_get(sk);
	if (!dst || !dst->obsolete ||
	    dst->ops->check(dst, inet6_sk(sk)->dst_cookie)) {
		rcu_read_unlock();
		return;
	}
	rcu_read_unlock();

	ip6_datagram_dst_update(sk, false);
}
EXPORT_SYMBOL_GPL(ip6_datagram_release_cb);

int __ip6_datagram_connect(struct sock *sk, struct sockaddr *uaddr,
			   int addr_len)
{
	struct sockaddr_in6	*usin = (struct sockaddr_in6 *) uaddr;
	struct inet_sock	*inet = inet_sk(sk);
	struct ipv6_pinfo	*np = inet6_sk(sk);
	struct in6_addr		*daddr, old_daddr;
	__be32			fl6_flowlabel = 0;
	__be32			old_fl6_flowlabel;
	__be16			old_dport;
	int			addr_type;
	int			err;

	if (usin->sin6_family == AF_INET) {
		if (__ipv6_only_sock(sk))
			return -EAFNOSUPPORT;
		err = __ip4_datagram_connect(sk, uaddr, addr_len);
		goto ipv4_connected;
	}

	if (addr_len < SIN6_LEN_RFC2133)
		return -EINVAL;

	if (usin->sin6_family != AF_INET6)
		return -EAFNOSUPPORT;

	if (np->sndflow)
		fl6_flowlabel = usin->sin6_flowinfo & IPV6_FLOWINFO_MASK;

	if (ipv6_addr_any(&usin->sin6_addr)) {
		/*
		 *	connect to self
		 */
		if (ipv6_addr_v4mapped(&sk->sk_v6_rcv_saddr))
			ipv6_addr_set_v4mapped(htonl(INADDR_LOOPBACK),
					       &usin->sin6_addr);
		else
			usin->sin6_addr = in6addr_loopback;
	}

	addr_type = ipv6_addr_type(&usin->sin6_addr);

	daddr = &usin->sin6_addr;

	if (addr_type & IPV6_ADDR_MAPPED) {
		struct sockaddr_in sin;

		if (__ipv6_only_sock(sk)) {
			err = -ENETUNREACH;
			goto out;
		}
		sin.sin_family = AF_INET;
		sin.sin_addr.s_addr = daddr->s6_addr32[3];
		sin.sin_port = usin->sin6_port;

		err = __ip4_datagram_connect(sk,
					     (struct sockaddr *) &sin,
					     sizeof(sin));

ipv4_connected:
		if (err)
			goto out;

		ipv6_addr_set_v4mapped(inet->inet_daddr, &sk->sk_v6_daddr);

		if (ipv6_addr_any(&np->saddr) ||
		    ipv6_mapped_addr_any(&np->saddr))
			ipv6_addr_set_v4mapped(inet->inet_saddr, &np->saddr);

		if (ipv6_addr_any(&sk->sk_v6_rcv_saddr) ||
		    ipv6_mapped_addr_any(&sk->sk_v6_rcv_saddr)) {
			ipv6_addr_set_v4mapped(inet->inet_rcv_saddr,
					       &sk->sk_v6_rcv_saddr);
			if (sk->sk_prot->rehash)
				sk->sk_prot->rehash(sk);
		}

		goto out;
	}

	if (__ipv6_addr_needs_scope_id(addr_type)) {
		if (addr_len >= sizeof(struct sockaddr_in6) &&
		    usin->sin6_scope_id) {
			if (!sk_dev_equal_l3scope(sk, usin->sin6_scope_id)) {
				err = -EINVAL;
				goto out;
			}
			sk->sk_bound_dev_if = usin->sin6_scope_id;
		}

		if (!sk->sk_bound_dev_if && (addr_type & IPV6_ADDR_MULTICAST))
			sk->sk_bound_dev_if = np->mcast_oif;

		/* Connect to link-local address requires an interface */
		if (!sk->sk_bound_dev_if) {
			err = -EINVAL;
			goto out;
		}
	}

	/* save the current peer information before updating it */
	old_daddr = sk->sk_v6_daddr;
	old_fl6_flowlabel = np->flow_label;
	old_dport = inet->inet_dport;

	sk->sk_v6_daddr = *daddr;
	np->flow_label = fl6_flowlabel;
	inet->inet_dport = usin->sin6_port;

	/*
	 *	Check for a route to destination an obtain the
	 *	destination cache for it.
	 */

	err = ip6_datagram_dst_update(sk, true);
	if (err) {
		/* Restore the socket peer info, to keep it consistent with
		 * the old socket state
		 */
		sk->sk_v6_daddr = old_daddr;
		np->flow_label = old_fl6_flowlabel;
		inet->inet_dport = old_dport;
		goto out;
	}

	reuseport_has_conns(sk, true);
	sk->sk_state = TCP_ESTABLISHED;
	sk_set_txhash(sk);
out:
	return err;
}
EXPORT_SYMBOL_GPL(__ip6_datagram_connect);

int ip6_datagram_connect(struct sock *sk, struct sockaddr *uaddr, int addr_len)
{
	int res;

	lock_sock(sk);
	res = __ip6_datagram_connect(sk, uaddr, addr_len);
	release_sock(sk);
	return res;
}
EXPORT_SYMBOL_GPL(ip6_datagram_connect);

int ip6_datagram_connect_v6_only(struct sock *sk, struct sockaddr *uaddr,
				 int addr_len)
{
	DECLARE_SOCKADDR(struct sockaddr_in6 *, sin6, uaddr);
	if (sin6->sin6_family != AF_INET6)
		return -EAFNOSUPPORT;
	return ip6_datagram_connect(sk, uaddr, addr_len);
}
EXPORT_SYMBOL_GPL(ip6_datagram_connect_v6_only);

static void ipv6_icmp_error_rfc4884(const struct sk_buff *skb,
				    struct sock_ee_data_rfc4884 *out)
{
	switch (icmp6_hdr(skb)->icmp6_type) {
	case ICMPV6_TIME_EXCEED:
	case ICMPV6_DEST_UNREACH:
		ip_icmp_error_rfc4884(skb, out, sizeof(struct icmp6hdr),
				      icmp6_hdr(skb)->icmp6_datagram_len * 8);
	}
}

void ipv6_icmp_error(struct sock *sk, struct sk_buff *skb, int err,
		     __be16 port, u32 info, u8 *payload)
{
	struct ipv6_pinfo *np  = inet6_sk(sk);
	struct icmp6hdr *icmph = icmp6_hdr(skb);
	struct sock_exterr_skb *serr;

	if (!np->recverr)
		return;

	skb = skb_clone(skb, GFP_ATOMIC);
	if (!skb)
		return;

	skb->protocol = htons(ETH_P_IPV6);

	serr = SKB_EXT_ERR(skb);
	serr->ee.ee_errno = err;
	serr->ee.ee_origin = SO_EE_ORIGIN_ICMP6;
	serr->ee.ee_type = icmph->icmp6_type;
	serr->ee.ee_code = icmph->icmp6_code;
	serr->ee.ee_pad = 0;
	serr->ee.ee_info = info;
	serr->ee.ee_data = 0;
	serr->addr_offset = (u8 *)&(((struct ipv6hdr *)(icmph + 1))->daddr) -
				  skb_network_header(skb);
	serr->port = port;

	__skb_pull(skb, payload - skb->data);

	if (inet6_sk(sk)->recverr_rfc4884)
		ipv6_icmp_error_rfc4884(skb, &serr->ee.ee_rfc4884);

	skb_reset_transport_header(skb);

	if (sock_queue_err_skb(sk, skb))
		kfree_skb(skb);
}

void ipv6_local_error(struct sock *sk, int err, struct flowi6 *fl6, u32 info)
{
	const struct ipv6_pinfo *np = inet6_sk(sk);
	struct sock_exterr_skb *serr;
	struct ipv6hdr *iph;
	struct sk_buff *skb;

	if (!np->recverr)
		return;

	skb = alloc_skb(sizeof(struct ipv6hdr), GFP_ATOMIC);
	if (!skb)
		return;

	skb->protocol = htons(ETH_P_IPV6);

	skb_put(skb, sizeof(struct ipv6hdr));
	skb_reset_network_header(skb);
	iph = ipv6_hdr(skb);
	iph->daddr = fl6->daddr;
	ip6_flow_hdr(iph, 0, 0);

	serr = SKB_EXT_ERR(skb);
	serr->ee.ee_errno = err;
	serr->ee.ee_origin = SO_EE_ORIGIN_LOCAL;
	serr->ee.ee_type = 0;
	serr->ee.ee_code = 0;
	serr->ee.ee_pad = 0;
	serr->ee.ee_info = info;
	serr->ee.ee_data = 0;
	serr->addr_offset = (u8 *)&iph->daddr - skb_network_header(skb);
	serr->port = fl6->fl6_dport;

	__skb_pull(skb, skb_tail_pointer(skb) - skb->data);
	skb_reset_transport_header(skb);

	if (sock_queue_err_skb(sk, skb))
		kfree_skb(skb);
}

void ipv6_local_rxpmtu(struct sock *sk, struct flowi6 *fl6, u32 mtu)
{
	struct ipv6_pinfo *np = inet6_sk(sk);
	struct ipv6hdr *iph;
	struct sk_buff *skb;
	struct ip6_mtuinfo *mtu_info;

	if (!np->rxopt.bits.rxpmtu)
		return;

	skb = alloc_skb(sizeof(struct ipv6hdr), GFP_ATOMIC);
	if (!skb)
		return;

	skb_put(skb, sizeof(struct ipv6hdr));
	skb_reset_network_header(skb);
	iph = ipv6_hdr(skb);
	iph->daddr = fl6->daddr;

	mtu_info = IP6CBMTU(skb);

	mtu_info->ip6m_mtu = mtu;
	mtu_info->ip6m_addr.sin6_family = AF_INET6;
	mtu_info->ip6m_addr.sin6_port = 0;
	mtu_info->ip6m_addr.sin6_flowinfo = 0;
	mtu_info->ip6m_addr.sin6_scope_id = fl6->flowi6_oif;
	mtu_info->ip6m_addr.sin6_addr = ipv6_hdr(skb)->daddr;

	__skb_pull(skb, skb_tail_pointer(skb) - skb->data);
	skb_reset_transport_header(skb);

	skb = xchg(&np->rxpmtu, skb);
	kfree_skb(skb);
}

/* For some errors we have valid addr_offset even with zero payload and
 * zero port. Also, addr_offset should be supported if port is set.
 */
static inline bool ipv6_datagram_support_addr(struct sock_exterr_skb *serr)
{
	return serr->ee.ee_origin == SO_EE_ORIGIN_ICMP6 ||
	       serr->ee.ee_origin == SO_EE_ORIGIN_ICMP ||
	       serr->ee.ee_origin == SO_EE_ORIGIN_LOCAL || serr->port;
}

/* IPv6 supports cmsg on all origins aside from SO_EE_ORIGIN_LOCAL.
 *
 * At one point, excluding local errors was a quick test to identify icmp/icmp6
 * errors. This is no longer true, but the test remained, so the v6 stack,
 * unlike v4, also honors cmsg requests on all wifi and timestamp errors.
 */
static bool ip6_datagram_support_cmsg(struct sk_buff *skb,
				      struct sock_exterr_skb *serr)
{
	if (serr->ee.ee_origin == SO_EE_ORIGIN_ICMP ||
	    serr->ee.ee_origin == SO_EE_ORIGIN_ICMP6)
		return true;

	if (serr->ee.ee_origin == SO_EE_ORIGIN_LOCAL)
		return false;

	if (!IP6CB(skb)->iif)
		return false;

	return true;
}

/*
 *	Handle MSG_ERRQUEUE
 */
int ipv6_recv_error(struct sock *sk, struct msghdr *msg, int len, int *addr_len)
{
	struct ipv6_pinfo *np = inet6_sk(sk);
	struct sock_exterr_skb *serr;
	struct sk_buff *skb;
	DECLARE_SOCKADDR(struct sockaddr_in6 *, sin, msg->msg_name);
	struct {
		struct sock_extended_err ee;
		struct sockaddr_in6	 offender;
	} errhdr;
	int err;
	int copied;

	err = -EAGAIN;
	skb = sock_dequeue_err_skb(sk);
	if (!skb)
		goto out;

	copied = skb->len;
	if (copied > len) {
		msg->msg_flags |= MSG_TRUNC;
		copied = len;
	}
	err = skb_copy_datagram_msg(skb, 0, msg, copied);
	if (unlikely(err)) {
		kfree_skb(skb);
		return err;
	}
	sock_recv_timestamp(msg, sk, skb);

	serr = SKB_EXT_ERR(skb);

	if (sin && ipv6_datagram_support_addr(serr)) {
		const unsigned char *nh = skb_network_header(skb);
		sin->sin6_family = AF_INET6;
		sin->sin6_flowinfo = 0;
		sin->sin6_port = serr->port;
		if (skb->protocol == htons(ETH_P_IPV6)) {
			const struct ipv6hdr *ip6h = container_of((struct in6_addr *)(nh + serr->addr_offset),
								  struct ipv6hdr, daddr);
			sin->sin6_addr = ip6h->daddr;
			if (np->sndflow)
				sin->sin6_flowinfo = ip6_flowinfo(ip6h);
			sin->sin6_scope_id =
				ipv6_iface_scope_id(&sin->sin6_addr,
						    IP6CB(skb)->iif);
		} else {
			ipv6_addr_set_v4mapped(*(__be32 *)(nh + serr->addr_offset),
					       &sin->sin6_addr);
			sin->sin6_scope_id = 0;
		}
		*addr_len = sizeof(*sin);
	}

	memcpy(&errhdr.ee, &serr->ee, sizeof(struct sock_extended_err));
	sin = &errhdr.offender;
	memset(sin, 0, sizeof(*sin));

	if (ip6_datagram_support_cmsg(skb, serr)) {
		sin->sin6_family = AF_INET6;
		if (np->rxopt.all)
			ip6_datagram_recv_common_ctl(sk, msg, skb);
		if (skb->protocol == htons(ETH_P_IPV6)) {
			sin->sin6_addr = ipv6_hdr(skb)->saddr;
			if (np->rxopt.all)
				ip6_datagram_recv_specific_ctl(sk, msg, skb);
			sin->sin6_scope_id =
				ipv6_iface_scope_id(&sin->sin6_addr,
						    IP6CB(skb)->iif);
		} else {
			ipv6_addr_set_v4mapped(ip_hdr(skb)->saddr,
					       &sin->sin6_addr);
			if (inet_sk(sk)->cmsg_flags)
				ip_cmsg_recv(msg, skb);
		}
	}

	put_cmsg(msg, SOL_IPV6, IPV6_RECVERR, sizeof(errhdr), &errhdr);

	/* Now we could try to dump offended packet options */

	msg->msg_flags |= MSG_ERRQUEUE;
	err = copied;

	consume_skb(skb);
out:
	return err;
}
EXPORT_SYMBOL_GPL(ipv6_recv_error);

/*
 *	Handle IPV6_RECVPATHMTU
 */
int ipv6_recv_rxpmtu(struct sock *sk, struct msghdr *msg, int len,
		     int *addr_len)
{
	struct ipv6_pinfo *np = inet6_sk(sk);
	struct sk_buff *skb;
	struct ip6_mtuinfo mtu_info;
	DECLARE_SOCKADDR(struct sockaddr_in6 *, sin, msg->msg_name);
	int err;
	int copied;

	err = -EAGAIN;
	skb = xchg(&np->rxpmtu, NULL);
	if (!skb)
		goto out;

	copied = skb->len;
	if (copied > len) {
		msg->msg_flags |= MSG_TRUNC;
		copied = len;
	}
	err = skb_copy_datagram_msg(skb, 0, msg, copied);
	if (err)
		goto out_free_skb;

	sock_recv_timestamp(msg, sk, skb);

	memcpy(&mtu_info, IP6CBMTU(skb), sizeof(mtu_info));

	if (sin) {
		sin->sin6_family = AF_INET6;
		sin->sin6_flowinfo = 0;
		sin->sin6_port = 0;
		sin->sin6_scope_id = mtu_info.ip6m_addr.sin6_scope_id;
		sin->sin6_addr = mtu_info.ip6m_addr.sin6_addr;
		*addr_len = sizeof(*sin);
	}

	put_cmsg(msg, SOL_IPV6, IPV6_PATHMTU, sizeof(mtu_info), &mtu_info);

	err = copied;

out_free_skb:
	kfree_skb(skb);
out:
	return err;
}


void ip6_datagram_recv_common_ctl(struct sock *sk, struct msghdr *msg,
				 struct sk_buff *skb)
{
	struct ipv6_pinfo *np = inet6_sk(sk);
	bool is_ipv6 = skb->protocol == htons(ETH_P_IPV6);

	if (np->rxopt.bits.rxinfo) {
		struct in6_pktinfo src_info;

		if (is_ipv6) {
			src_info.ipi6_ifindex = IP6CB(skb)->iif;
			src_info.ipi6_addr = ipv6_hdr(skb)->daddr;
		} else {
			src_info.ipi6_ifindex =
				PKTINFO_SKB_CB(skb)->ipi_ifindex;
			ipv6_addr_set_v4mapped(ip_hdr(skb)->daddr,
					       &src_info.ipi6_addr);
		}

		if (src_info.ipi6_ifindex >= 0)
			put_cmsg(msg, SOL_IPV6, IPV6_PKTINFO,
				 sizeof(src_info), &src_info);
	}
}

void ip6_datagram_recv_specific_ctl(struct sock *sk, struct msghdr *msg,
				    struct sk_buff *skb)
{
	struct ipv6_pinfo *np = inet6_sk(sk);
	struct inet6_skb_parm *opt = IP6CB(skb);
	unsigned char *nh = skb_network_header(skb);

	if (np->rxopt.bits.rxhlim) {
		int hlim = ipv6_hdr(skb)->hop_limit;
		put_cmsg(msg, SOL_IPV6, IPV6_HOPLIMIT, sizeof(hlim), &hlim);
	}

	if (np->rxopt.bits.rxtclass) {
		int tclass = ipv6_get_dsfield(ipv6_hdr(skb));
		put_cmsg(msg, SOL_IPV6, IPV6_TCLASS, sizeof(tclass), &tclass);
	}

	if (np->rxopt.bits.rxflow) {
		__be32 flowinfo = ip6_flowinfo((struct ipv6hdr *)nh);
		if (flowinfo)
			put_cmsg(msg, SOL_IPV6, IPV6_FLOWINFO, sizeof(flowinfo), &flowinfo);
	}

	/* HbH is allowed only once */
	if (np->rxopt.bits.hopopts && (opt->flags & IP6SKB_HOPBYHOP)) {
		u8 *ptr = nh + sizeof(struct ipv6hdr);
		put_cmsg(msg, SOL_IPV6, IPV6_HOPOPTS, (ptr[1]+1)<<3, ptr);
	}

	if (opt->lastopt &&
	    (np->rxopt.bits.dstopts || np->rxopt.bits.srcrt)) {
		/*
		 * Silly enough, but we need to reparse in order to
		 * report extension headers (except for HbH)
		 * in order.
		 *
		 * Also note that IPV6_RECVRTHDRDSTOPTS is NOT
		 * (and WILL NOT be) defined because
		 * IPV6_RECVDSTOPTS is more generic. --yoshfuji
		 */
		unsigned int off = sizeof(struct ipv6hdr);
		u8 nexthdr = ipv6_hdr(skb)->nexthdr;

		while (off <= opt->lastopt) {
			unsigned int len;
			u8 *ptr = nh + off;

			switch (nexthdr) {
			case IPPROTO_DSTOPTS:
				nexthdr = ptr[0];
				len = (ptr[1] + 1) << 3;
				if (np->rxopt.bits.dstopts)
					put_cmsg(msg, SOL_IPV6, IPV6_DSTOPTS, len, ptr);
				break;
			case IPPROTO_ROUTING:
				nexthdr = ptr[0];
				len = (ptr[1] + 1) << 3;
				if (np->rxopt.bits.srcrt)
					put_cmsg(msg, SOL_IPV6, IPV6_RTHDR, len, ptr);
				break;
			case IPPROTO_AH:
				nexthdr = ptr[0];
				len = (ptr[1] + 2) << 2;
				break;
			default:
				nexthdr = ptr[0];
				len = (ptr[1] + 1) << 3;
				break;
			}

			off += len;
		}
	}

	/* socket options in old style */
	if (np->rxopt.bits.rxoinfo) {
		struct in6_pktinfo src_info;

		src_info.ipi6_ifindex = opt->iif;
		src_info.ipi6_addr = ipv6_hdr(skb)->daddr;
		put_cmsg(msg, SOL_IPV6, IPV6_2292PKTINFO, sizeof(src_info), &src_info);
	}
	if (np->rxopt.bits.rxohlim) {
		int hlim = ipv6_hdr(skb)->hop_limit;
		put_cmsg(msg, SOL_IPV6, IPV6_2292HOPLIMIT, sizeof(hlim), &hlim);
	}
	if (np->rxopt.bits.ohopopts && (opt->flags & IP6SKB_HOPBYHOP)) {
		u8 *ptr = nh + sizeof(struct ipv6hdr);
		put_cmsg(msg, SOL_IPV6, IPV6_2292HOPOPTS, (ptr[1]+1)<<3, ptr);
	}
	if (np->rxopt.bits.odstopts && opt->dst0) {
		u8 *ptr = nh + opt->dst0;
		put_cmsg(msg, SOL_IPV6, IPV6_2292DSTOPTS, (ptr[1]+1)<<3, ptr);
	}
	if (np->rxopt.bits.osrcrt && opt->srcrt) {
		struct ipv6_rt_hdr *rthdr = (struct ipv6_rt_hdr *)(nh + opt->srcrt);
		put_cmsg(msg, SOL_IPV6, IPV6_2292RTHDR, (rthdr->hdrlen+1) << 3, rthdr);
	}
	if (np->rxopt.bits.odstopts && opt->dst1) {
		u8 *ptr = nh + opt->dst1;
		put_cmsg(msg, SOL_IPV6, IPV6_2292DSTOPTS, (ptr[1]+1)<<3, ptr);
	}
	if (np->rxopt.bits.rxorigdstaddr) {
		struct sockaddr_in6 sin6;
		__be16 _ports[2], *ports;

		ports = skb_header_pointer(skb, skb_transport_offset(skb),
					   sizeof(_ports), &_ports);
		if (ports) {
			/* All current transport protocols have the port numbers in the
			 * first four bytes of the transport header and this function is
			 * written with this assumption in mind.
			 */
			sin6.sin6_family = AF_INET6;
			sin6.sin6_addr = ipv6_hdr(skb)->daddr;
			sin6.sin6_port = ports[1];
			sin6.sin6_flowinfo = 0;
			sin6.sin6_scope_id =
				ipv6_iface_scope_id(&ipv6_hdr(skb)->daddr,
						    opt->iif);

			put_cmsg(msg, SOL_IPV6, IPV6_ORIGDSTADDR, sizeof(sin6), &sin6);
		}
	}
	if (np->rxopt.bits.recvfragsize && opt->frag_max_size) {
		int val = opt->frag_max_size;

		put_cmsg(msg, SOL_IPV6, IPV6_RECVFRAGSIZE, sizeof(val), &val);
	}
}

void ip6_datagram_recv_ctl(struct sock *sk, struct msghdr *msg,
			  struct sk_buff *skb)
{
	ip6_datagram_recv_common_ctl(sk, msg, skb);
	ip6_datagram_recv_specific_ctl(sk, msg, skb);
}
EXPORT_SYMBOL_GPL(ip6_datagram_recv_ctl);

int ip6_datagram_send_ctl(struct net *net, struct sock *sk,
			  struct msghdr *msg, struct flowi6 *fl6,
			  struct ipcm6_cookie *ipc6)
{
	struct in6_pktinfo *src_info;
	struct cmsghdr *cmsg;
	struct ipv6_rt_hdr *rthdr;
	struct ipv6_opt_hdr *hdr;
	struct ipv6_txoptions *opt = ipc6->opt;
	int len;
	int err = 0;

	for_each_cmsghdr(cmsg, msg) {
		int addr_type;

		if (!CMSG_OK(msg, cmsg)) {
			err = -EINVAL;
			goto exit_f;
		}

		if (cmsg->cmsg_level == SOL_SOCKET) {
			err = __sock_cmsg_send(sk, msg, cmsg, &ipc6->sockc);
			if (err)
				return err;
			continue;
		}

		if (cmsg->cmsg_level != SOL_IPV6)
			continue;

		switch (cmsg->cmsg_type) {
		case IPV6_PKTINFO:
		case IPV6_2292PKTINFO:
		    {
			struct net_device *dev = NULL;
			int src_idx;

			if (cmsg->cmsg_len < CMSG_LEN(sizeof(struct in6_pktinfo))) {
				err = -EINVAL;
				goto exit_f;
			}

			src_info = (struct in6_pktinfo *)CMSG_DATA(cmsg);
			src_idx = src_info->ipi6_ifindex;

			if (src_idx) {
				if (fl6->flowi6_oif &&
				    src_idx != fl6->flowi6_oif &&
				    (sk->sk_bound_dev_if != fl6->flowi6_oif ||
				     !sk_dev_equal_l3scope(sk, src_idx)))
					return -EINVAL;
				fl6->flowi6_oif = src_idx;
			}

			addr_type = __ipv6_addr_type(&src_info->ipi6_addr);

			rcu_read_lock();
			if (fl6->flowi6_oif) {
				dev = dev_get_by_index_rcu(net, fl6->flowi6_oif);
				if (!dev) {
					rcu_read_unlock();
					return -ENODEV;
				}
			} else if (addr_type & IPV6_ADDR_LINKLOCAL) {
				rcu_read_unlock();
				return -EINVAL;
			}

			if (addr_type != IPV6_ADDR_ANY) {
				int strict = __ipv6_addr_src_scope(addr_type) <= IPV6_ADDR_SCOPE_LINKLOCAL;
				if (!ipv6_can_nonlocal_bind(net, inet_sk(sk)) &&
				    !ipv6_chk_addr_and_flags(net, &src_info->ipi6_addr,
							     dev, !strict, 0,
							     IFA_F_TENTATIVE) &&
				    !ipv6_chk_acast_addr_src(net, dev,
							     &src_info->ipi6_addr))
					err = -EINVAL;
				else
					fl6->saddr = src_info->ipi6_addr;
			}

			rcu_read_unlock();

			if (err)
				goto exit_f;

			break;
		    }

		case IPV6_FLOWINFO:
			if (cmsg->cmsg_len < CMSG_LEN(4)) {
				err = -EINVAL;
				goto exit_f;
			}

			if (fl6->flowlabel&IPV6_FLOWINFO_MASK) {
				if ((fl6->flowlabel^*(__be32 *)CMSG_DATA(cmsg))&~IPV6_FLOWINFO_MASK) {
					err = -EINVAL;
					goto exit_f;
				}
			}
			fl6->flowlabel = IPV6_FLOWINFO_MASK & *(__be32 *)CMSG_DATA(cmsg);
			break;

		case IPV6_2292HOPOPTS:
		case IPV6_HOPOPTS:
			if (opt->hopopt || cmsg->cmsg_len < CMSG_LEN(sizeof(struct ipv6_opt_hdr))) {
				err = -EINVAL;
				goto exit_f;
			}

			hdr = (struct ipv6_opt_hdr *)CMSG_DATA(cmsg);
			len = ((hdr->hdrlen + 1) << 3);
			if (cmsg->cmsg_len < CMSG_LEN(len)) {
				err = -EINVAL;
				goto exit_f;
			}
			if (!ns_capable(net->user_ns, CAP_NET_RAW)) {
				err = -EPERM;
				goto exit_f;
			}
			opt->opt_nflen += len;
			opt->hopopt = hdr;
			break;

		case IPV6_2292DSTOPTS:
			if (cmsg->cmsg_len < CMSG_LEN(sizeof(struct ipv6_opt_hdr))) {
				err = -EINVAL;
				goto exit_f;
			}

			hdr = (struct ipv6_opt_hdr *)CMSG_DATA(cmsg);
			len = ((hdr->hdrlen + 1) << 3);
			if (cmsg->cmsg_len < CMSG_LEN(len)) {
				err = -EINVAL;
				goto exit_f;
			}
			if (!ns_capable(net->user_ns, CAP_NET_RAW)) {
				err = -EPERM;
				goto exit_f;
			}
			if (opt->dst1opt) {
				err = -EINVAL;
				goto exit_f;
			}
			opt->opt_flen += len;
			opt->dst1opt = hdr;
			break;

		case IPV6_DSTOPTS:
		case IPV6_RTHDRDSTOPTS:
			if (cmsg->cmsg_len < CMSG_LEN(sizeof(struct ipv6_opt_hdr))) {
				err = -EINVAL;
				goto exit_f;
			}

			hdr = (struct ipv6_opt_hdr *)CMSG_DATA(cmsg);
			len = ((hdr->hdrlen + 1) << 3);
			if (cmsg->cmsg_len < CMSG_LEN(len)) {
				err = -EINVAL;
				goto exit_f;
			}
			if (!ns_capable(net->user_ns, CAP_NET_RAW)) {
				err = -EPERM;
				goto exit_f;
			}
			if (cmsg->cmsg_type == IPV6_DSTOPTS) {
				opt->opt_flen += len;
				opt->dst1opt = hdr;
			} else {
				opt->opt_nflen += len;
				opt->dst0opt = hdr;
			}
			break;

		case IPV6_2292RTHDR:
		case IPV6_RTHDR:
			if (cmsg->cmsg_len < CMSG_LEN(sizeof(struct ipv6_rt_hdr))) {
				err = -EINVAL;
				goto exit_f;
			}

			rthdr = (struct ipv6_rt_hdr *)CMSG_DATA(cmsg);

			switch (rthdr->type) {
#if IS_ENABLED(CONFIG_IPV6_MIP6)
			case IPV6_SRCRT_TYPE_2:
				if (rthdr->hdrlen != 2 ||
				    rthdr->segments_left != 1) {
					err = -EINVAL;
					goto exit_f;
				}
				break;
#endif
			default:
				err = -EINVAL;
				goto exit_f;
			}

			len = ((rthdr->hdrlen + 1) << 3);

			if (cmsg->cmsg_len < CMSG_LEN(len)) {
				err = -EINVAL;
				goto exit_f;
			}

			/* segments left must also match */
			if ((rthdr->hdrlen >> 1) != rthdr->segments_left) {
				err = -EINVAL;
				goto exit_f;
			}

			opt->opt_nflen += len;
			opt->srcrt = rthdr;

			if (cmsg->cmsg_type == IPV6_2292RTHDR && opt->dst1opt) {
				int dsthdrlen = ((opt->dst1opt->hdrlen+1)<<3);

				opt->opt_nflen += dsthdrlen;
				opt->dst0opt = opt->dst1opt;
				opt->dst1opt = NULL;
				opt->opt_flen -= dsthdrlen;
			}

			break;

		case IPV6_2292HOPLIMIT:
		case IPV6_HOPLIMIT:
			if (cmsg->cmsg_len != CMSG_LEN(sizeof(int))) {
				err = -EINVAL;
				goto exit_f;
			}

			ipc6->hlimit = *(int *)CMSG_DATA(cmsg);
			if (ipc6->hlimit < -1 || ipc6->hlimit > 0xff) {
				err = -EINVAL;
				goto exit_f;
			}

			break;

		case IPV6_TCLASS:
		    {
			int tc;

			err = -EINVAL;
			if (cmsg->cmsg_len != CMSG_LEN(sizeof(int)))
				goto exit_f;

			tc = *(int *)CMSG_DATA(cmsg);
			if (tc < -1 || tc > 0xff)
				goto exit_f;

			err = 0;
			ipc6->tclass = tc;

			break;
		    }

		case IPV6_DONTFRAG:
		    {
			int df;

			err = -EINVAL;
			if (cmsg->cmsg_len != CMSG_LEN(sizeof(int)))
				goto exit_f;

			df = *(int *)CMSG_DATA(cmsg);
			if (df < 0 || df > 1)
				goto exit_f;

			err = 0;
			ipc6->dontfrag = df;

			break;
		    }
		default:
			net_dbg_ratelimited("invalid cmsg type: %d\n",
					    cmsg->cmsg_type);
			err = -EINVAL;
			goto exit_f;
		}
	}

exit_f:
	return err;
}
EXPORT_SYMBOL_GPL(ip6_datagram_send_ctl);

void __ip6_dgram_sock_seq_show(struct seq_file *seq, struct sock *sp,
			       __u16 srcp, __u16 destp, int rqueue, int bucket)
{
	const struct in6_addr *dest, *src;

	dest  = &sp->sk_v6_daddr;
	src   = &sp->sk_v6_rcv_saddr;
	seq_printf(seq,
		   "%5d: %08X%08X%08X%08X:%04X %08X%08X%08X%08X:%04X "
		   "%02X %08X:%08X %02X:%08lX %08X %5u %8d %lu %d %pK %u\n",
		   bucket,
		   src->s6_addr32[0], src->s6_addr32[1],
		   src->s6_addr32[2], src->s6_addr32[3], srcp,
		   dest->s6_addr32[0], dest->s6_addr32[1],
		   dest->s6_addr32[2], dest->s6_addr32[3], destp,
		   sp->sk_state,
		   sk_wmem_alloc_get(sp),
		   rqueue,
		   0, 0L, 0,
		   from_kuid_munged(seq_user_ns(seq), sock_i_uid(sp)),
		   0,
		   sock_i_ino(sp),
		   refcount_read(&sp->sk_refcnt), sp,
		   atomic_read(&sp->sk_drops));
}
