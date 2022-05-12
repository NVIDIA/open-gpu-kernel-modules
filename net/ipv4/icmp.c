// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *	NET3:	Implementation of the ICMP protocol layer.
 *
 *		Alan Cox, <alan@lxorguk.ukuu.org.uk>
 *
 *	Some of the function names and the icmp unreach table for this
 *	module were derived from [icmp.c 1.0.11 06/02/93] by
 *	Ross Biro, Fred N. van Kempen, Mark Evans, Alan Cox, Gerhard Koerting.
 *	Other than that this module is a complete rewrite.
 *
 *	Fixes:
 *	Clemens Fruhwirth	:	introduce global icmp rate limiting
 *					with icmp type masking ability instead
 *					of broken per type icmp timeouts.
 *		Mike Shaver	:	RFC1122 checks.
 *		Alan Cox	:	Multicast ping reply as self.
 *		Alan Cox	:	Fix atomicity lockup in ip_build_xmit
 *					call.
 *		Alan Cox	:	Added 216,128 byte paths to the MTU
 *					code.
 *		Martin Mares	:	RFC1812 checks.
 *		Martin Mares	:	Can be configured to follow redirects
 *					if acting as a router _without_ a
 *					routing protocol (RFC 1812).
 *		Martin Mares	:	Echo requests may be configured to
 *					be ignored (RFC 1812).
 *		Martin Mares	:	Limitation of ICMP error message
 *					transmit rate (RFC 1812).
 *		Martin Mares	:	TOS and Precedence set correctly
 *					(RFC 1812).
 *		Martin Mares	:	Now copying as much data from the
 *					original packet as we can without
 *					exceeding 576 bytes (RFC 1812).
 *	Willy Konynenberg	:	Transparent proxying support.
 *		Keith Owens	:	RFC1191 correction for 4.2BSD based
 *					path MTU bug.
 *		Thomas Quinot	:	ICMP Dest Unreach codes up to 15 are
 *					valid (RFC 1812).
 *		Andi Kleen	:	Check all packet lengths properly
 *					and moved all kfree_skb() up to
 *					icmp_rcv.
 *		Andi Kleen	:	Move the rate limit bookkeeping
 *					into the dest entry and use a token
 *					bucket filter (thanks to ANK). Make
 *					the rates sysctl configurable.
 *		Yu Tianli	:	Fixed two ugly bugs in icmp_send
 *					- IP option length was accounted wrongly
 *					- ICMP header length was not accounted
 *					  at all.
 *              Tristan Greaves :       Added sysctl option to ignore bogus
 *              			broadcast responses from broken routers.
 *
 * To Fix:
 *
 *	- Should use skb_pull() instead of all the manual checking.
 *	  This would also greatly simply some upper layer error handlers. --AK
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/module.h>
#include <linux/types.h>
#include <linux/jiffies.h>
#include <linux/kernel.h>
#include <linux/fcntl.h>
#include <linux/socket.h>
#include <linux/in.h>
#include <linux/inet.h>
#include <linux/inetdevice.h>
#include <linux/netdevice.h>
#include <linux/string.h>
#include <linux/netfilter_ipv4.h>
#include <linux/slab.h>
#include <net/snmp.h>
#include <net/ip.h>
#include <net/route.h>
#include <net/protocol.h>
#include <net/icmp.h>
#include <net/tcp.h>
#include <net/udp.h>
#include <net/raw.h>
#include <net/ping.h>
#include <linux/skbuff.h>
#include <net/sock.h>
#include <linux/errno.h>
#include <linux/timer.h>
#include <linux/init.h>
#include <linux/uaccess.h>
#include <net/checksum.h>
#include <net/xfrm.h>
#include <net/inet_common.h>
#include <net/ip_fib.h>
#include <net/l3mdev.h>

/*
 *	Build xmit assembly blocks
 */

struct icmp_bxm {
	struct sk_buff *skb;
	int offset;
	int data_len;

	struct {
		struct icmphdr icmph;
		__be32	       times[3];
	} data;
	int head_len;
	struct ip_options_data replyopts;
};

/* An array of errno for error messages from dest unreach. */
/* RFC 1122: 3.2.2.1 States that NET_UNREACH, HOST_UNREACH and SR_FAILED MUST be considered 'transient errs'. */

const struct icmp_err icmp_err_convert[] = {
	{
		.errno = ENETUNREACH,	/* ICMP_NET_UNREACH */
		.fatal = 0,
	},
	{
		.errno = EHOSTUNREACH,	/* ICMP_HOST_UNREACH */
		.fatal = 0,
	},
	{
		.errno = ENOPROTOOPT	/* ICMP_PROT_UNREACH */,
		.fatal = 1,
	},
	{
		.errno = ECONNREFUSED,	/* ICMP_PORT_UNREACH */
		.fatal = 1,
	},
	{
		.errno = EMSGSIZE,	/* ICMP_FRAG_NEEDED */
		.fatal = 0,
	},
	{
		.errno = EOPNOTSUPP,	/* ICMP_SR_FAILED */
		.fatal = 0,
	},
	{
		.errno = ENETUNREACH,	/* ICMP_NET_UNKNOWN */
		.fatal = 1,
	},
	{
		.errno = EHOSTDOWN,	/* ICMP_HOST_UNKNOWN */
		.fatal = 1,
	},
	{
		.errno = ENONET,	/* ICMP_HOST_ISOLATED */
		.fatal = 1,
	},
	{
		.errno = ENETUNREACH,	/* ICMP_NET_ANO	*/
		.fatal = 1,
	},
	{
		.errno = EHOSTUNREACH,	/* ICMP_HOST_ANO */
		.fatal = 1,
	},
	{
		.errno = ENETUNREACH,	/* ICMP_NET_UNR_TOS */
		.fatal = 0,
	},
	{
		.errno = EHOSTUNREACH,	/* ICMP_HOST_UNR_TOS */
		.fatal = 0,
	},
	{
		.errno = EHOSTUNREACH,	/* ICMP_PKT_FILTERED */
		.fatal = 1,
	},
	{
		.errno = EHOSTUNREACH,	/* ICMP_PREC_VIOLATION */
		.fatal = 1,
	},
	{
		.errno = EHOSTUNREACH,	/* ICMP_PREC_CUTOFF */
		.fatal = 1,
	},
};
EXPORT_SYMBOL(icmp_err_convert);

/*
 *	ICMP control array. This specifies what to do with each ICMP.
 */

struct icmp_control {
	bool (*handler)(struct sk_buff *skb);
	short   error;		/* This ICMP is classed as an error message */
};

static const struct icmp_control icmp_pointers[NR_ICMP_TYPES+1];

/*
 *	The ICMP socket(s). This is the most convenient way to flow control
 *	our ICMP output as well as maintain a clean interface throughout
 *	all layers. All Socketless IP sends will soon be gone.
 *
 *	On SMP we have one ICMP socket per-cpu.
 */
static struct sock *icmp_sk(struct net *net)
{
	return this_cpu_read(*net->ipv4.icmp_sk);
}

/* Called with BH disabled */
static inline struct sock *icmp_xmit_lock(struct net *net)
{
	struct sock *sk;

	sk = icmp_sk(net);

	if (unlikely(!spin_trylock(&sk->sk_lock.slock))) {
		/* This can happen if the output path signals a
		 * dst_link_failure() for an outgoing ICMP packet.
		 */
		return NULL;
	}
	return sk;
}

static inline void icmp_xmit_unlock(struct sock *sk)
{
	spin_unlock(&sk->sk_lock.slock);
}

int sysctl_icmp_msgs_per_sec __read_mostly = 1000;
int sysctl_icmp_msgs_burst __read_mostly = 50;

static struct {
	spinlock_t	lock;
	u32		credit;
	u32		stamp;
} icmp_global = {
	.lock		= __SPIN_LOCK_UNLOCKED(icmp_global.lock),
};

/**
 * icmp_global_allow - Are we allowed to send one more ICMP message ?
 *
 * Uses a token bucket to limit our ICMP messages to ~sysctl_icmp_msgs_per_sec.
 * Returns false if we reached the limit and can not send another packet.
 * Note: called with BH disabled
 */
bool icmp_global_allow(void)
{
	u32 credit, delta, incr = 0, now = (u32)jiffies;
	bool rc = false;

	/* Check if token bucket is empty and cannot be refilled
	 * without taking the spinlock. The READ_ONCE() are paired
	 * with the following WRITE_ONCE() in this same function.
	 */
	if (!READ_ONCE(icmp_global.credit)) {
		delta = min_t(u32, now - READ_ONCE(icmp_global.stamp), HZ);
		if (delta < HZ / 50)
			return false;
	}

	spin_lock(&icmp_global.lock);
	delta = min_t(u32, now - icmp_global.stamp, HZ);
	if (delta >= HZ / 50) {
		incr = sysctl_icmp_msgs_per_sec * delta / HZ ;
		if (incr)
			WRITE_ONCE(icmp_global.stamp, now);
	}
	credit = min_t(u32, icmp_global.credit + incr, sysctl_icmp_msgs_burst);
	if (credit) {
		/* We want to use a credit of one in average, but need to randomize
		 * it for security reasons.
		 */
		credit = max_t(int, credit - prandom_u32_max(3), 0);
		rc = true;
	}
	WRITE_ONCE(icmp_global.credit, credit);
	spin_unlock(&icmp_global.lock);
	return rc;
}
EXPORT_SYMBOL(icmp_global_allow);

static bool icmpv4_mask_allow(struct net *net, int type, int code)
{
	if (type > NR_ICMP_TYPES)
		return true;

	/* Don't limit PMTU discovery. */
	if (type == ICMP_DEST_UNREACH && code == ICMP_FRAG_NEEDED)
		return true;

	/* Limit if icmp type is enabled in ratemask. */
	if (!((1 << type) & net->ipv4.sysctl_icmp_ratemask))
		return true;

	return false;
}

static bool icmpv4_global_allow(struct net *net, int type, int code)
{
	if (icmpv4_mask_allow(net, type, code))
		return true;

	if (icmp_global_allow())
		return true;

	return false;
}

/*
 *	Send an ICMP frame.
 */

static bool icmpv4_xrlim_allow(struct net *net, struct rtable *rt,
			       struct flowi4 *fl4, int type, int code)
{
	struct dst_entry *dst = &rt->dst;
	struct inet_peer *peer;
	bool rc = true;
	int vif;

	if (icmpv4_mask_allow(net, type, code))
		goto out;

	/* No rate limit on loopback */
	if (dst->dev && (dst->dev->flags&IFF_LOOPBACK))
		goto out;

	vif = l3mdev_master_ifindex(dst->dev);
	peer = inet_getpeer_v4(net->ipv4.peers, fl4->daddr, vif, 1);
	rc = inet_peer_xrlim_allow(peer, net->ipv4.sysctl_icmp_ratelimit);
	if (peer)
		inet_putpeer(peer);
out:
	return rc;
}

/*
 *	Maintain the counters used in the SNMP statistics for outgoing ICMP
 */
void icmp_out_count(struct net *net, unsigned char type)
{
	ICMPMSGOUT_INC_STATS(net, type);
	ICMP_INC_STATS(net, ICMP_MIB_OUTMSGS);
}

/*
 *	Checksum each fragment, and on the first include the headers and final
 *	checksum.
 */
static int icmp_glue_bits(void *from, char *to, int offset, int len, int odd,
			  struct sk_buff *skb)
{
	struct icmp_bxm *icmp_param = (struct icmp_bxm *)from;
	__wsum csum;

	csum = skb_copy_and_csum_bits(icmp_param->skb,
				      icmp_param->offset + offset,
				      to, len);

	skb->csum = csum_block_add(skb->csum, csum, odd);
	if (icmp_pointers[icmp_param->data.icmph.type].error)
		nf_ct_attach(skb, icmp_param->skb);
	return 0;
}

static void icmp_push_reply(struct icmp_bxm *icmp_param,
			    struct flowi4 *fl4,
			    struct ipcm_cookie *ipc, struct rtable **rt)
{
	struct sock *sk;
	struct sk_buff *skb;

	sk = icmp_sk(dev_net((*rt)->dst.dev));
	if (ip_append_data(sk, fl4, icmp_glue_bits, icmp_param,
			   icmp_param->data_len+icmp_param->head_len,
			   icmp_param->head_len,
			   ipc, rt, MSG_DONTWAIT) < 0) {
		__ICMP_INC_STATS(sock_net(sk), ICMP_MIB_OUTERRORS);
		ip_flush_pending_frames(sk);
	} else if ((skb = skb_peek(&sk->sk_write_queue)) != NULL) {
		struct icmphdr *icmph = icmp_hdr(skb);
		__wsum csum;
		struct sk_buff *skb1;

		csum = csum_partial_copy_nocheck((void *)&icmp_param->data,
						 (char *)icmph,
						 icmp_param->head_len);
		skb_queue_walk(&sk->sk_write_queue, skb1) {
			csum = csum_add(csum, skb1->csum);
		}
		icmph->checksum = csum_fold(csum);
		skb->ip_summed = CHECKSUM_NONE;
		ip_push_pending_frames(sk, fl4);
	}
}

/*
 *	Driving logic for building and sending ICMP messages.
 */

static void icmp_reply(struct icmp_bxm *icmp_param, struct sk_buff *skb)
{
	struct ipcm_cookie ipc;
	struct rtable *rt = skb_rtable(skb);
	struct net *net = dev_net(rt->dst.dev);
	struct flowi4 fl4;
	struct sock *sk;
	struct inet_sock *inet;
	__be32 daddr, saddr;
	u32 mark = IP4_REPLY_MARK(net, skb->mark);
	int type = icmp_param->data.icmph.type;
	int code = icmp_param->data.icmph.code;

	if (ip_options_echo(net, &icmp_param->replyopts.opt.opt, skb))
		return;

	/* Needed by both icmp_global_allow and icmp_xmit_lock */
	local_bh_disable();

	/* global icmp_msgs_per_sec */
	if (!icmpv4_global_allow(net, type, code))
		goto out_bh_enable;

	sk = icmp_xmit_lock(net);
	if (!sk)
		goto out_bh_enable;
	inet = inet_sk(sk);

	icmp_param->data.icmph.checksum = 0;

	ipcm_init(&ipc);
	inet->tos = ip_hdr(skb)->tos;
	ipc.sockc.mark = mark;
	daddr = ipc.addr = ip_hdr(skb)->saddr;
	saddr = fib_compute_spec_dst(skb);

	if (icmp_param->replyopts.opt.opt.optlen) {
		ipc.opt = &icmp_param->replyopts.opt;
		if (ipc.opt->opt.srr)
			daddr = icmp_param->replyopts.opt.opt.faddr;
	}
	memset(&fl4, 0, sizeof(fl4));
	fl4.daddr = daddr;
	fl4.saddr = saddr;
	fl4.flowi4_mark = mark;
	fl4.flowi4_uid = sock_net_uid(net, NULL);
	fl4.flowi4_tos = RT_TOS(ip_hdr(skb)->tos);
	fl4.flowi4_proto = IPPROTO_ICMP;
	fl4.flowi4_oif = l3mdev_master_ifindex(skb->dev);
	security_skb_classify_flow(skb, flowi4_to_flowi_common(&fl4));
	rt = ip_route_output_key(net, &fl4);
	if (IS_ERR(rt))
		goto out_unlock;
	if (icmpv4_xrlim_allow(net, rt, &fl4, type, code))
		icmp_push_reply(icmp_param, &fl4, &ipc, &rt);
	ip_rt_put(rt);
out_unlock:
	icmp_xmit_unlock(sk);
out_bh_enable:
	local_bh_enable();
}

/*
 * The device used for looking up which routing table to use for sending an ICMP
 * error is preferably the source whenever it is set, which should ensure the
 * icmp error can be sent to the source host, else lookup using the routing
 * table of the destination device, else use the main routing table (index 0).
 */
static struct net_device *icmp_get_route_lookup_dev(struct sk_buff *skb)
{
	struct net_device *route_lookup_dev = NULL;

	if (skb->dev)
		route_lookup_dev = skb->dev;
	else if (skb_dst(skb))
		route_lookup_dev = skb_dst(skb)->dev;
	return route_lookup_dev;
}

static struct rtable *icmp_route_lookup(struct net *net,
					struct flowi4 *fl4,
					struct sk_buff *skb_in,
					const struct iphdr *iph,
					__be32 saddr, u8 tos, u32 mark,
					int type, int code,
					struct icmp_bxm *param)
{
	struct net_device *route_lookup_dev;
	struct rtable *rt, *rt2;
	struct flowi4 fl4_dec;
	int err;

	memset(fl4, 0, sizeof(*fl4));
	fl4->daddr = (param->replyopts.opt.opt.srr ?
		      param->replyopts.opt.opt.faddr : iph->saddr);
	fl4->saddr = saddr;
	fl4->flowi4_mark = mark;
	fl4->flowi4_uid = sock_net_uid(net, NULL);
	fl4->flowi4_tos = RT_TOS(tos);
	fl4->flowi4_proto = IPPROTO_ICMP;
	fl4->fl4_icmp_type = type;
	fl4->fl4_icmp_code = code;
	route_lookup_dev = icmp_get_route_lookup_dev(skb_in);
	fl4->flowi4_oif = l3mdev_master_ifindex(route_lookup_dev);

	security_skb_classify_flow(skb_in, flowi4_to_flowi_common(fl4));
	rt = ip_route_output_key_hash(net, fl4, skb_in);
	if (IS_ERR(rt))
		return rt;

	/* No need to clone since we're just using its address. */
	rt2 = rt;

	rt = (struct rtable *) xfrm_lookup(net, &rt->dst,
					   flowi4_to_flowi(fl4), NULL, 0);
	if (!IS_ERR(rt)) {
		if (rt != rt2)
			return rt;
	} else if (PTR_ERR(rt) == -EPERM) {
		rt = NULL;
	} else
		return rt;

	err = xfrm_decode_session_reverse(skb_in, flowi4_to_flowi(&fl4_dec), AF_INET);
	if (err)
		goto relookup_failed;

	if (inet_addr_type_dev_table(net, route_lookup_dev,
				     fl4_dec.saddr) == RTN_LOCAL) {
		rt2 = __ip_route_output_key(net, &fl4_dec);
		if (IS_ERR(rt2))
			err = PTR_ERR(rt2);
	} else {
		struct flowi4 fl4_2 = {};
		unsigned long orefdst;

		fl4_2.daddr = fl4_dec.saddr;
		rt2 = ip_route_output_key(net, &fl4_2);
		if (IS_ERR(rt2)) {
			err = PTR_ERR(rt2);
			goto relookup_failed;
		}
		/* Ugh! */
		orefdst = skb_in->_skb_refdst; /* save old refdst */
		skb_dst_set(skb_in, NULL);
		err = ip_route_input(skb_in, fl4_dec.daddr, fl4_dec.saddr,
				     RT_TOS(tos), rt2->dst.dev);

		dst_release(&rt2->dst);
		rt2 = skb_rtable(skb_in);
		skb_in->_skb_refdst = orefdst; /* restore old refdst */
	}

	if (err)
		goto relookup_failed;

	rt2 = (struct rtable *) xfrm_lookup(net, &rt2->dst,
					    flowi4_to_flowi(&fl4_dec), NULL,
					    XFRM_LOOKUP_ICMP);
	if (!IS_ERR(rt2)) {
		dst_release(&rt->dst);
		memcpy(fl4, &fl4_dec, sizeof(*fl4));
		rt = rt2;
	} else if (PTR_ERR(rt2) == -EPERM) {
		if (rt)
			dst_release(&rt->dst);
		return rt2;
	} else {
		err = PTR_ERR(rt2);
		goto relookup_failed;
	}
	return rt;

relookup_failed:
	if (rt)
		return rt;
	return ERR_PTR(err);
}

/*
 *	Send an ICMP message in response to a situation
 *
 *	RFC 1122: 3.2.2	MUST send at least the IP header and 8 bytes of header.
 *		  MAY send more (we do).
 *			MUST NOT change this header information.
 *			MUST NOT reply to a multicast/broadcast IP address.
 *			MUST NOT reply to a multicast/broadcast MAC address.
 *			MUST reply to only the first fragment.
 */

void __icmp_send(struct sk_buff *skb_in, int type, int code, __be32 info,
		 const struct ip_options *opt)
{
	struct iphdr *iph;
	int room;
	struct icmp_bxm icmp_param;
	struct rtable *rt = skb_rtable(skb_in);
	struct ipcm_cookie ipc;
	struct flowi4 fl4;
	__be32 saddr;
	u8  tos;
	u32 mark;
	struct net *net;
	struct sock *sk;

	if (!rt)
		goto out;

	if (rt->dst.dev)
		net = dev_net(rt->dst.dev);
	else if (skb_in->dev)
		net = dev_net(skb_in->dev);
	else
		goto out;

	/*
	 *	Find the original header. It is expected to be valid, of course.
	 *	Check this, icmp_send is called from the most obscure devices
	 *	sometimes.
	 */
	iph = ip_hdr(skb_in);

	if ((u8 *)iph < skb_in->head ||
	    (skb_network_header(skb_in) + sizeof(*iph)) >
	    skb_tail_pointer(skb_in))
		goto out;

	/*
	 *	No replies to physical multicast/broadcast
	 */
	if (skb_in->pkt_type != PACKET_HOST)
		goto out;

	/*
	 *	Now check at the protocol level
	 */
	if (rt->rt_flags & (RTCF_BROADCAST | RTCF_MULTICAST))
		goto out;

	/*
	 *	Only reply to fragment 0. We byte re-order the constant
	 *	mask for efficiency.
	 */
	if (iph->frag_off & htons(IP_OFFSET))
		goto out;

	/*
	 *	If we send an ICMP error to an ICMP error a mess would result..
	 */
	if (icmp_pointers[type].error) {
		/*
		 *	We are an error, check if we are replying to an
		 *	ICMP error
		 */
		if (iph->protocol == IPPROTO_ICMP) {
			u8 _inner_type, *itp;

			itp = skb_header_pointer(skb_in,
						 skb_network_header(skb_in) +
						 (iph->ihl << 2) +
						 offsetof(struct icmphdr,
							  type) -
						 skb_in->data,
						 sizeof(_inner_type),
						 &_inner_type);
			if (!itp)
				goto out;

			/*
			 *	Assume any unknown ICMP type is an error. This
			 *	isn't specified by the RFC, but think about it..
			 */
			if (*itp > NR_ICMP_TYPES ||
			    icmp_pointers[*itp].error)
				goto out;
		}
	}

	/* Needed by both icmp_global_allow and icmp_xmit_lock */
	local_bh_disable();

	/* Check global sysctl_icmp_msgs_per_sec ratelimit, unless
	 * incoming dev is loopback.  If outgoing dev change to not be
	 * loopback, then peer ratelimit still work (in icmpv4_xrlim_allow)
	 */
	if (!(skb_in->dev && (skb_in->dev->flags&IFF_LOOPBACK)) &&
	      !icmpv4_global_allow(net, type, code))
		goto out_bh_enable;

	sk = icmp_xmit_lock(net);
	if (!sk)
		goto out_bh_enable;

	/*
	 *	Construct source address and options.
	 */

	saddr = iph->daddr;
	if (!(rt->rt_flags & RTCF_LOCAL)) {
		struct net_device *dev = NULL;

		rcu_read_lock();
		if (rt_is_input_route(rt) &&
		    net->ipv4.sysctl_icmp_errors_use_inbound_ifaddr)
			dev = dev_get_by_index_rcu(net, inet_iif(skb_in));

		if (dev)
			saddr = inet_select_addr(dev, iph->saddr,
						 RT_SCOPE_LINK);
		else
			saddr = 0;
		rcu_read_unlock();
	}

	tos = icmp_pointers[type].error ? (RT_TOS(iph->tos) |
					   IPTOS_PREC_INTERNETCONTROL) :
					   iph->tos;
	mark = IP4_REPLY_MARK(net, skb_in->mark);

	if (__ip_options_echo(net, &icmp_param.replyopts.opt.opt, skb_in, opt))
		goto out_unlock;


	/*
	 *	Prepare data for ICMP header.
	 */

	icmp_param.data.icmph.type	 = type;
	icmp_param.data.icmph.code	 = code;
	icmp_param.data.icmph.un.gateway = info;
	icmp_param.data.icmph.checksum	 = 0;
	icmp_param.skb	  = skb_in;
	icmp_param.offset = skb_network_offset(skb_in);
	inet_sk(sk)->tos = tos;
	ipcm_init(&ipc);
	ipc.addr = iph->saddr;
	ipc.opt = &icmp_param.replyopts.opt;
	ipc.sockc.mark = mark;

	rt = icmp_route_lookup(net, &fl4, skb_in, iph, saddr, tos, mark,
			       type, code, &icmp_param);
	if (IS_ERR(rt))
		goto out_unlock;

	/* peer icmp_ratelimit */
	if (!icmpv4_xrlim_allow(net, rt, &fl4, type, code))
		goto ende;

	/* RFC says return as much as we can without exceeding 576 bytes. */

	room = dst_mtu(&rt->dst);
	if (room > 576)
		room = 576;
	room -= sizeof(struct iphdr) + icmp_param.replyopts.opt.opt.optlen;
	room -= sizeof(struct icmphdr);

	icmp_param.data_len = skb_in->len - icmp_param.offset;
	if (icmp_param.data_len > room)
		icmp_param.data_len = room;
	icmp_param.head_len = sizeof(struct icmphdr);

	/* if we don't have a source address at this point, fall back to the
	 * dummy address instead of sending out a packet with a source address
	 * of 0.0.0.0
	 */
	if (!fl4.saddr)
		fl4.saddr = htonl(INADDR_DUMMY);

	icmp_push_reply(&icmp_param, &fl4, &ipc, &rt);
ende:
	ip_rt_put(rt);
out_unlock:
	icmp_xmit_unlock(sk);
out_bh_enable:
	local_bh_enable();
out:;
}
EXPORT_SYMBOL(__icmp_send);

#if IS_ENABLED(CONFIG_NF_NAT)
#include <net/netfilter/nf_conntrack.h>
void icmp_ndo_send(struct sk_buff *skb_in, int type, int code, __be32 info)
{
	struct sk_buff *cloned_skb = NULL;
	struct ip_options opts = { 0 };
	enum ip_conntrack_info ctinfo;
	struct nf_conn *ct;
	__be32 orig_ip;

	ct = nf_ct_get(skb_in, &ctinfo);
	if (!ct || !(ct->status & IPS_SRC_NAT)) {
		__icmp_send(skb_in, type, code, info, &opts);
		return;
	}

	if (skb_shared(skb_in))
		skb_in = cloned_skb = skb_clone(skb_in, GFP_ATOMIC);

	if (unlikely(!skb_in || skb_network_header(skb_in) < skb_in->head ||
	    (skb_network_header(skb_in) + sizeof(struct iphdr)) >
	    skb_tail_pointer(skb_in) || skb_ensure_writable(skb_in,
	    skb_network_offset(skb_in) + sizeof(struct iphdr))))
		goto out;

	orig_ip = ip_hdr(skb_in)->saddr;
	ip_hdr(skb_in)->saddr = ct->tuplehash[0].tuple.src.u3.ip;
	__icmp_send(skb_in, type, code, info, &opts);
	ip_hdr(skb_in)->saddr = orig_ip;
out:
	consume_skb(cloned_skb);
}
EXPORT_SYMBOL(icmp_ndo_send);
#endif

static void icmp_socket_deliver(struct sk_buff *skb, u32 info)
{
	const struct iphdr *iph = (const struct iphdr *)skb->data;
	const struct net_protocol *ipprot;
	int protocol = iph->protocol;

	/* Checkin full IP header plus 8 bytes of protocol to
	 * avoid additional coding at protocol handlers.
	 */
	if (!pskb_may_pull(skb, iph->ihl * 4 + 8)) {
		__ICMP_INC_STATS(dev_net(skb->dev), ICMP_MIB_INERRORS);
		return;
	}

	raw_icmp_error(skb, protocol, info);

	ipprot = rcu_dereference(inet_protos[protocol]);
	if (ipprot && ipprot->err_handler)
		ipprot->err_handler(skb, info);
}

static bool icmp_tag_validation(int proto)
{
	bool ok;

	rcu_read_lock();
	ok = rcu_dereference(inet_protos[proto])->icmp_strict_tag_validation;
	rcu_read_unlock();
	return ok;
}

/*
 *	Handle ICMP_DEST_UNREACH, ICMP_TIME_EXCEEDED, ICMP_QUENCH, and
 *	ICMP_PARAMETERPROB.
 */

static bool icmp_unreach(struct sk_buff *skb)
{
	const struct iphdr *iph;
	struct icmphdr *icmph;
	struct net *net;
	u32 info = 0;

	net = dev_net(skb_dst(skb)->dev);

	/*
	 *	Incomplete header ?
	 * 	Only checks for the IP header, there should be an
	 *	additional check for longer headers in upper levels.
	 */

	if (!pskb_may_pull(skb, sizeof(struct iphdr)))
		goto out_err;

	icmph = icmp_hdr(skb);
	iph   = (const struct iphdr *)skb->data;

	if (iph->ihl < 5) /* Mangled header, drop. */
		goto out_err;

	switch (icmph->type) {
	case ICMP_DEST_UNREACH:
		switch (icmph->code & 15) {
		case ICMP_NET_UNREACH:
		case ICMP_HOST_UNREACH:
		case ICMP_PROT_UNREACH:
		case ICMP_PORT_UNREACH:
			break;
		case ICMP_FRAG_NEEDED:
			/* for documentation of the ip_no_pmtu_disc
			 * values please see
			 * Documentation/networking/ip-sysctl.rst
			 */
			switch (net->ipv4.sysctl_ip_no_pmtu_disc) {
			default:
				net_dbg_ratelimited("%pI4: fragmentation needed and DF set\n",
						    &iph->daddr);
				break;
			case 2:
				goto out;
			case 3:
				if (!icmp_tag_validation(iph->protocol))
					goto out;
				fallthrough;
			case 0:
				info = ntohs(icmph->un.frag.mtu);
			}
			break;
		case ICMP_SR_FAILED:
			net_dbg_ratelimited("%pI4: Source Route Failed\n",
					    &iph->daddr);
			break;
		default:
			break;
		}
		if (icmph->code > NR_ICMP_UNREACH)
			goto out;
		break;
	case ICMP_PARAMETERPROB:
		info = ntohl(icmph->un.gateway) >> 24;
		break;
	case ICMP_TIME_EXCEEDED:
		__ICMP_INC_STATS(net, ICMP_MIB_INTIMEEXCDS);
		if (icmph->code == ICMP_EXC_FRAGTIME)
			goto out;
		break;
	}

	/*
	 *	Throw it at our lower layers
	 *
	 *	RFC 1122: 3.2.2 MUST extract the protocol ID from the passed
	 *		  header.
	 *	RFC 1122: 3.2.2.1 MUST pass ICMP unreach messages to the
	 *		  transport layer.
	 *	RFC 1122: 3.2.2.2 MUST pass ICMP time expired messages to
	 *		  transport layer.
	 */

	/*
	 *	Check the other end isn't violating RFC 1122. Some routers send
	 *	bogus responses to broadcast frames. If you see this message
	 *	first check your netmask matches at both ends, if it does then
	 *	get the other vendor to fix their kit.
	 */

	if (!net->ipv4.sysctl_icmp_ignore_bogus_error_responses &&
	    inet_addr_type_dev_table(net, skb->dev, iph->daddr) == RTN_BROADCAST) {
		net_warn_ratelimited("%pI4 sent an invalid ICMP type %u, code %u error to a broadcast: %pI4 on %s\n",
				     &ip_hdr(skb)->saddr,
				     icmph->type, icmph->code,
				     &iph->daddr, skb->dev->name);
		goto out;
	}

	icmp_socket_deliver(skb, info);

out:
	return true;
out_err:
	__ICMP_INC_STATS(net, ICMP_MIB_INERRORS);
	return false;
}


/*
 *	Handle ICMP_REDIRECT.
 */

static bool icmp_redirect(struct sk_buff *skb)
{
	if (skb->len < sizeof(struct iphdr)) {
		__ICMP_INC_STATS(dev_net(skb->dev), ICMP_MIB_INERRORS);
		return false;
	}

	if (!pskb_may_pull(skb, sizeof(struct iphdr))) {
		/* there aught to be a stat */
		return false;
	}

	icmp_socket_deliver(skb, ntohl(icmp_hdr(skb)->un.gateway));
	return true;
}

/*
 *	Handle ICMP_ECHO ("ping") and ICMP_EXT_ECHO ("PROBE") requests.
 *
 *	RFC 1122: 3.2.2.6 MUST have an echo server that answers ICMP echo
 *		  requests.
 *	RFC 1122: 3.2.2.6 Data received in the ICMP_ECHO request MUST be
 *		  included in the reply.
 *	RFC 1812: 4.3.3.6 SHOULD have a config option for silently ignoring
 *		  echo requests, MUST have default=NOT.
 *	RFC 8335: 8 MUST have a config option to enable/disable ICMP
 *		  Extended Echo Functionality, MUST be disabled by default
 *	See also WRT handling of options once they are done and working.
 */

static bool icmp_echo(struct sk_buff *skb)
{
	struct icmp_ext_hdr *ext_hdr, _ext_hdr;
	struct icmp_ext_echo_iio *iio, _iio;
	struct icmp_bxm icmp_param;
	struct net_device *dev;
	char buff[IFNAMSIZ];
	struct net *net;
	u16 ident_len;
	u8 status;

	net = dev_net(skb_dst(skb)->dev);
	/* should there be an ICMP stat for ignored echos? */
	if (net->ipv4.sysctl_icmp_echo_ignore_all)
		return true;

	icmp_param.data.icmph	   = *icmp_hdr(skb);
	icmp_param.skb		   = skb;
	icmp_param.offset	   = 0;
	icmp_param.data_len	   = skb->len;
	icmp_param.head_len	   = sizeof(struct icmphdr);

	if (icmp_param.data.icmph.type == ICMP_ECHO) {
		icmp_param.data.icmph.type = ICMP_ECHOREPLY;
		goto send_reply;
	}
	if (!net->ipv4.sysctl_icmp_echo_enable_probe)
		return true;
	/* We currently only support probing interfaces on the proxy node
	 * Check to ensure L-bit is set
	 */
	if (!(ntohs(icmp_param.data.icmph.un.echo.sequence) & 1))
		return true;
	/* Clear status bits in reply message */
	icmp_param.data.icmph.un.echo.sequence &= htons(0xFF00);
	icmp_param.data.icmph.type = ICMP_EXT_ECHOREPLY;
	ext_hdr = skb_header_pointer(skb, 0, sizeof(_ext_hdr), &_ext_hdr);
	/* Size of iio is class_type dependent.
	 * Only check header here and assign length based on ctype in the switch statement
	 */
	iio = skb_header_pointer(skb, sizeof(_ext_hdr), sizeof(iio->extobj_hdr), &_iio);
	if (!ext_hdr || !iio)
		goto send_mal_query;
	if (ntohs(iio->extobj_hdr.length) <= sizeof(iio->extobj_hdr))
		goto send_mal_query;
	ident_len = ntohs(iio->extobj_hdr.length) - sizeof(iio->extobj_hdr);
	status = 0;
	dev = NULL;
	switch (iio->extobj_hdr.class_type) {
	case ICMP_EXT_ECHO_CTYPE_NAME:
		iio = skb_header_pointer(skb, sizeof(_ext_hdr), sizeof(_iio), &_iio);
		if (ident_len >= IFNAMSIZ)
			goto send_mal_query;
		memset(buff, 0, sizeof(buff));
		memcpy(buff, &iio->ident.name, ident_len);
		dev = dev_get_by_name(net, buff);
		break;
	case ICMP_EXT_ECHO_CTYPE_INDEX:
		iio = skb_header_pointer(skb, sizeof(_ext_hdr), sizeof(iio->extobj_hdr) +
					 sizeof(iio->ident.ifindex), &_iio);
		if (ident_len != sizeof(iio->ident.ifindex))
			goto send_mal_query;
		dev = dev_get_by_index(net, ntohl(iio->ident.ifindex));
		break;
	case ICMP_EXT_ECHO_CTYPE_ADDR:
		if (ident_len != sizeof(iio->ident.addr.ctype3_hdr) +
				 iio->ident.addr.ctype3_hdr.addrlen)
			goto send_mal_query;
		switch (ntohs(iio->ident.addr.ctype3_hdr.afi)) {
		case ICMP_AFI_IP:
			iio = skb_header_pointer(skb, sizeof(_ext_hdr), sizeof(iio->extobj_hdr) +
						 sizeof(struct in_addr), &_iio);
			if (ident_len != sizeof(iio->ident.addr.ctype3_hdr) +
					 sizeof(struct in_addr))
				goto send_mal_query;
			dev = ip_dev_find(net, iio->ident.addr.ip_addr.ipv4_addr.s_addr);
			break;
#if IS_ENABLED(CONFIG_IPV6)
		case ICMP_AFI_IP6:
			iio = skb_header_pointer(skb, sizeof(_ext_hdr), sizeof(_iio), &_iio);
			if (ident_len != sizeof(iio->ident.addr.ctype3_hdr) +
					 sizeof(struct in6_addr))
				goto send_mal_query;
			dev = ipv6_stub->ipv6_dev_find(net, &iio->ident.addr.ip_addr.ipv6_addr, dev);
			if (dev)
				dev_hold(dev);
			break;
#endif
		default:
			goto send_mal_query;
		}
		break;
	default:
		goto send_mal_query;
	}
	if (!dev) {
		icmp_param.data.icmph.code = ICMP_EXT_CODE_NO_IF;
		goto send_reply;
	}
	/* Fill bits in reply message */
	if (dev->flags & IFF_UP)
		status |= ICMP_EXT_ECHOREPLY_ACTIVE;
	if (__in_dev_get_rcu(dev) && __in_dev_get_rcu(dev)->ifa_list)
		status |= ICMP_EXT_ECHOREPLY_IPV4;
	if (!list_empty(&rcu_dereference(dev->ip6_ptr)->addr_list))
		status |= ICMP_EXT_ECHOREPLY_IPV6;
	dev_put(dev);
	icmp_param.data.icmph.un.echo.sequence |= htons(status);
send_reply:
	icmp_reply(&icmp_param, skb);
		return true;
send_mal_query:
	icmp_param.data.icmph.code = ICMP_EXT_CODE_MAL_QUERY;
	goto send_reply;
}

/*
 *	Handle ICMP Timestamp requests.
 *	RFC 1122: 3.2.2.8 MAY implement ICMP timestamp requests.
 *		  SHOULD be in the kernel for minimum random latency.
 *		  MUST be accurate to a few minutes.
 *		  MUST be updated at least at 15Hz.
 */
static bool icmp_timestamp(struct sk_buff *skb)
{
	struct icmp_bxm icmp_param;
	/*
	 *	Too short.
	 */
	if (skb->len < 4)
		goto out_err;

	/*
	 *	Fill in the current time as ms since midnight UT:
	 */
	icmp_param.data.times[1] = inet_current_timestamp();
	icmp_param.data.times[2] = icmp_param.data.times[1];

	BUG_ON(skb_copy_bits(skb, 0, &icmp_param.data.times[0], 4));

	icmp_param.data.icmph	   = *icmp_hdr(skb);
	icmp_param.data.icmph.type = ICMP_TIMESTAMPREPLY;
	icmp_param.data.icmph.code = 0;
	icmp_param.skb		   = skb;
	icmp_param.offset	   = 0;
	icmp_param.data_len	   = 0;
	icmp_param.head_len	   = sizeof(struct icmphdr) + 12;
	icmp_reply(&icmp_param, skb);
	return true;

out_err:
	__ICMP_INC_STATS(dev_net(skb_dst(skb)->dev), ICMP_MIB_INERRORS);
	return false;
}

static bool icmp_discard(struct sk_buff *skb)
{
	/* pretend it was a success */
	return true;
}

/*
 *	Deal with incoming ICMP packets.
 */
int icmp_rcv(struct sk_buff *skb)
{
	struct icmphdr *icmph;
	struct rtable *rt = skb_rtable(skb);
	struct net *net = dev_net(rt->dst.dev);
	bool success;

	if (!xfrm4_policy_check(NULL, XFRM_POLICY_IN, skb)) {
		struct sec_path *sp = skb_sec_path(skb);
		int nh;

		if (!(sp && sp->xvec[sp->len - 1]->props.flags &
				 XFRM_STATE_ICMP))
			goto drop;

		if (!pskb_may_pull(skb, sizeof(*icmph) + sizeof(struct iphdr)))
			goto drop;

		nh = skb_network_offset(skb);
		skb_set_network_header(skb, sizeof(*icmph));

		if (!xfrm4_policy_check_reverse(NULL, XFRM_POLICY_IN, skb))
			goto drop;

		skb_set_network_header(skb, nh);
	}

	__ICMP_INC_STATS(net, ICMP_MIB_INMSGS);

	if (skb_checksum_simple_validate(skb))
		goto csum_error;

	if (!pskb_pull(skb, sizeof(*icmph)))
		goto error;

	icmph = icmp_hdr(skb);

	ICMPMSGIN_INC_STATS(net, icmph->type);

	/* Check for ICMP Extended Echo (PROBE) messages */
	if (icmph->type == ICMP_EXT_ECHO) {
		/* We can't use icmp_pointers[].handler() because it is an array of
		 * size NR_ICMP_TYPES + 1 (19 elements) and PROBE has code 42.
		 */
		success = icmp_echo(skb);
		goto success_check;
	}

	if (icmph->type == ICMP_EXT_ECHOREPLY) {
		success = ping_rcv(skb);
		goto success_check;
	}

	/*
	 *	18 is the highest 'known' ICMP type. Anything else is a mystery
	 *
	 *	RFC 1122: 3.2.2  Unknown ICMP messages types MUST be silently
	 *		  discarded.
	 */
	if (icmph->type > NR_ICMP_TYPES)
		goto error;

	/*
	 *	Parse the ICMP message
	 */

	if (rt->rt_flags & (RTCF_BROADCAST | RTCF_MULTICAST)) {
		/*
		 *	RFC 1122: 3.2.2.6 An ICMP_ECHO to broadcast MAY be
		 *	  silently ignored (we let user decide with a sysctl).
		 *	RFC 1122: 3.2.2.8 An ICMP_TIMESTAMP MAY be silently
		 *	  discarded if to broadcast/multicast.
		 */
		if ((icmph->type == ICMP_ECHO ||
		     icmph->type == ICMP_TIMESTAMP) &&
		    net->ipv4.sysctl_icmp_echo_ignore_broadcasts) {
			goto error;
		}
		if (icmph->type != ICMP_ECHO &&
		    icmph->type != ICMP_TIMESTAMP &&
		    icmph->type != ICMP_ADDRESS &&
		    icmph->type != ICMP_ADDRESSREPLY) {
			goto error;
		}
	}

	success = icmp_pointers[icmph->type].handler(skb);
success_check:
	if (success)  {
		consume_skb(skb);
		return NET_RX_SUCCESS;
	}

drop:
	kfree_skb(skb);
	return NET_RX_DROP;
csum_error:
	__ICMP_INC_STATS(net, ICMP_MIB_CSUMERRORS);
error:
	__ICMP_INC_STATS(net, ICMP_MIB_INERRORS);
	goto drop;
}

static bool ip_icmp_error_rfc4884_validate(const struct sk_buff *skb, int off)
{
	struct icmp_extobj_hdr *objh, _objh;
	struct icmp_ext_hdr *exth, _exth;
	u16 olen;

	exth = skb_header_pointer(skb, off, sizeof(_exth), &_exth);
	if (!exth)
		return false;
	if (exth->version != 2)
		return true;

	if (exth->checksum &&
	    csum_fold(skb_checksum(skb, off, skb->len - off, 0)))
		return false;

	off += sizeof(_exth);
	while (off < skb->len) {
		objh = skb_header_pointer(skb, off, sizeof(_objh), &_objh);
		if (!objh)
			return false;

		olen = ntohs(objh->length);
		if (olen < sizeof(_objh))
			return false;

		off += olen;
		if (off > skb->len)
			return false;
	}

	return true;
}

void ip_icmp_error_rfc4884(const struct sk_buff *skb,
			   struct sock_ee_data_rfc4884 *out,
			   int thlen, int off)
{
	int hlen;

	/* original datagram headers: end of icmph to payload (skb->data) */
	hlen = -skb_transport_offset(skb) - thlen;

	/* per rfc 4884: minimal datagram length of 128 bytes */
	if (off < 128 || off < hlen)
		return;

	/* kernel has stripped headers: return payload offset in bytes */
	off -= hlen;
	if (off + sizeof(struct icmp_ext_hdr) > skb->len)
		return;

	out->len = off;

	if (!ip_icmp_error_rfc4884_validate(skb, off))
		out->flags |= SO_EE_RFC4884_FLAG_INVALID;
}
EXPORT_SYMBOL_GPL(ip_icmp_error_rfc4884);

int icmp_err(struct sk_buff *skb, u32 info)
{
	struct iphdr *iph = (struct iphdr *)skb->data;
	int offset = iph->ihl<<2;
	struct icmphdr *icmph = (struct icmphdr *)(skb->data + offset);
	int type = icmp_hdr(skb)->type;
	int code = icmp_hdr(skb)->code;
	struct net *net = dev_net(skb->dev);

	/*
	 * Use ping_err to handle all icmp errors except those
	 * triggered by ICMP_ECHOREPLY which sent from kernel.
	 */
	if (icmph->type != ICMP_ECHOREPLY) {
		ping_err(skb, offset, info);
		return 0;
	}

	if (type == ICMP_DEST_UNREACH && code == ICMP_FRAG_NEEDED)
		ipv4_update_pmtu(skb, net, info, 0, IPPROTO_ICMP);
	else if (type == ICMP_REDIRECT)
		ipv4_redirect(skb, net, 0, IPPROTO_ICMP);

	return 0;
}

/*
 *	This table is the definition of how we handle ICMP.
 */
static const struct icmp_control icmp_pointers[NR_ICMP_TYPES + 1] = {
	[ICMP_ECHOREPLY] = {
		.handler = ping_rcv,
	},
	[1] = {
		.handler = icmp_discard,
		.error = 1,
	},
	[2] = {
		.handler = icmp_discard,
		.error = 1,
	},
	[ICMP_DEST_UNREACH] = {
		.handler = icmp_unreach,
		.error = 1,
	},
	[ICMP_SOURCE_QUENCH] = {
		.handler = icmp_unreach,
		.error = 1,
	},
	[ICMP_REDIRECT] = {
		.handler = icmp_redirect,
		.error = 1,
	},
	[6] = {
		.handler = icmp_discard,
		.error = 1,
	},
	[7] = {
		.handler = icmp_discard,
		.error = 1,
	},
	[ICMP_ECHO] = {
		.handler = icmp_echo,
	},
	[9] = {
		.handler = icmp_discard,
		.error = 1,
	},
	[10] = {
		.handler = icmp_discard,
		.error = 1,
	},
	[ICMP_TIME_EXCEEDED] = {
		.handler = icmp_unreach,
		.error = 1,
	},
	[ICMP_PARAMETERPROB] = {
		.handler = icmp_unreach,
		.error = 1,
	},
	[ICMP_TIMESTAMP] = {
		.handler = icmp_timestamp,
	},
	[ICMP_TIMESTAMPREPLY] = {
		.handler = icmp_discard,
	},
	[ICMP_INFO_REQUEST] = {
		.handler = icmp_discard,
	},
	[ICMP_INFO_REPLY] = {
		.handler = icmp_discard,
	},
	[ICMP_ADDRESS] = {
		.handler = icmp_discard,
	},
	[ICMP_ADDRESSREPLY] = {
		.handler = icmp_discard,
	},
};

static void __net_exit icmp_sk_exit(struct net *net)
{
	int i;

	for_each_possible_cpu(i)
		inet_ctl_sock_destroy(*per_cpu_ptr(net->ipv4.icmp_sk, i));
	free_percpu(net->ipv4.icmp_sk);
	net->ipv4.icmp_sk = NULL;
}

static int __net_init icmp_sk_init(struct net *net)
{
	int i, err;

	net->ipv4.icmp_sk = alloc_percpu(struct sock *);
	if (!net->ipv4.icmp_sk)
		return -ENOMEM;

	for_each_possible_cpu(i) {
		struct sock *sk;

		err = inet_ctl_sock_create(&sk, PF_INET,
					   SOCK_RAW, IPPROTO_ICMP, net);
		if (err < 0)
			goto fail;

		*per_cpu_ptr(net->ipv4.icmp_sk, i) = sk;

		/* Enough space for 2 64K ICMP packets, including
		 * sk_buff/skb_shared_info struct overhead.
		 */
		sk->sk_sndbuf =	2 * SKB_TRUESIZE(64 * 1024);

		/*
		 * Speedup sock_wfree()
		 */
		sock_set_flag(sk, SOCK_USE_WRITE_QUEUE);
		inet_sk(sk)->pmtudisc = IP_PMTUDISC_DONT;
	}

	/* Control parameters for ECHO replies. */
	net->ipv4.sysctl_icmp_echo_ignore_all = 0;
	net->ipv4.sysctl_icmp_echo_enable_probe = 0;
	net->ipv4.sysctl_icmp_echo_ignore_broadcasts = 1;

	/* Control parameter - ignore bogus broadcast responses? */
	net->ipv4.sysctl_icmp_ignore_bogus_error_responses = 1;

	/*
	 * 	Configurable global rate limit.
	 *
	 *	ratelimit defines tokens/packet consumed for dst->rate_token
	 *	bucket ratemask defines which icmp types are ratelimited by
	 *	setting	it's bit position.
	 *
	 *	default:
	 *	dest unreachable (3), source quench (4),
	 *	time exceeded (11), parameter problem (12)
	 */

	net->ipv4.sysctl_icmp_ratelimit = 1 * HZ;
	net->ipv4.sysctl_icmp_ratemask = 0x1818;
	net->ipv4.sysctl_icmp_errors_use_inbound_ifaddr = 0;

	return 0;

fail:
	icmp_sk_exit(net);
	return err;
}

static struct pernet_operations __net_initdata icmp_sk_ops = {
       .init = icmp_sk_init,
       .exit = icmp_sk_exit,
};

int __init icmp_init(void)
{
	return register_pernet_subsys(&icmp_sk_ops);
}
