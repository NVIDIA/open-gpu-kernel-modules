// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * INET		An implementation of the TCP/IP protocol suite for the LINUX
 *		operating system.  INET is implemented using the  BSD Socket
 *		interface as the means of communication with the user level.
 *
 *		"Ping" sockets
 *
 * Based on ipv4/ping.c code.
 *
 * Authors:	Lorenzo Colitti (IPv6 support)
 *		Vasiliy Kulikov / Openwall (IPv4 implementation, for Linux 2.6),
 *		Pavel Kankovsky (IPv4 implementation, for Linux 2.4.32)
 */

#include <net/addrconf.h>
#include <net/ipv6.h>
#include <net/ip6_route.h>
#include <net/protocol.h>
#include <net/udp.h>
#include <net/transp_v6.h>
#include <linux/proc_fs.h>
#include <net/ping.h>

/* Compatibility glue so we can support IPv6 when it's compiled as a module */
static int dummy_ipv6_recv_error(struct sock *sk, struct msghdr *msg, int len,
				 int *addr_len)
{
	return -EAFNOSUPPORT;
}
static void dummy_ip6_datagram_recv_ctl(struct sock *sk, struct msghdr *msg,
				       struct sk_buff *skb)
{
}
static int dummy_icmpv6_err_convert(u8 type, u8 code, int *err)
{
	return -EAFNOSUPPORT;
}
static void dummy_ipv6_icmp_error(struct sock *sk, struct sk_buff *skb, int err,
				  __be16 port, u32 info, u8 *payload) {}
static int dummy_ipv6_chk_addr(struct net *net, const struct in6_addr *addr,
			       const struct net_device *dev, int strict)
{
	return 0;
}

static int ping_v6_sendmsg(struct sock *sk, struct msghdr *msg, size_t len)
{
	struct inet_sock *inet = inet_sk(sk);
	struct ipv6_pinfo *np = inet6_sk(sk);
	struct icmp6hdr user_icmph;
	int addr_type;
	struct in6_addr *daddr;
	int oif = 0;
	struct flowi6 fl6;
	int err;
	struct dst_entry *dst;
	struct rt6_info *rt;
	struct pingfakehdr pfh;
	struct ipcm6_cookie ipc6;

	pr_debug("ping_v6_sendmsg(sk=%p,sk->num=%u)\n", inet, inet->inet_num);

	err = ping_common_sendmsg(AF_INET6, msg, len, &user_icmph,
				  sizeof(user_icmph));
	if (err)
		return err;

	if (msg->msg_name) {
		DECLARE_SOCKADDR(struct sockaddr_in6 *, u, msg->msg_name);
		if (msg->msg_namelen < sizeof(*u))
			return -EINVAL;
		if (u->sin6_family != AF_INET6) {
			return -EAFNOSUPPORT;
		}
		daddr = &(u->sin6_addr);
		if (__ipv6_addr_needs_scope_id(ipv6_addr_type(daddr)))
			oif = u->sin6_scope_id;
	} else {
		if (sk->sk_state != TCP_ESTABLISHED)
			return -EDESTADDRREQ;
		daddr = &sk->sk_v6_daddr;
	}

	if (!oif)
		oif = sk->sk_bound_dev_if;

	if (!oif)
		oif = np->sticky_pktinfo.ipi6_ifindex;

	if (!oif && ipv6_addr_is_multicast(daddr))
		oif = np->mcast_oif;
	else if (!oif)
		oif = np->ucast_oif;

	addr_type = ipv6_addr_type(daddr);
	if ((__ipv6_addr_needs_scope_id(addr_type) && !oif) ||
	    (addr_type & IPV6_ADDR_MAPPED) ||
	    (oif && sk->sk_bound_dev_if && oif != sk->sk_bound_dev_if))
		return -EINVAL;

	/* TODO: use ip6_datagram_send_ctl to get options from cmsg */

	memset(&fl6, 0, sizeof(fl6));

	fl6.flowi6_proto = IPPROTO_ICMPV6;
	fl6.saddr = np->saddr;
	fl6.daddr = *daddr;
	fl6.flowi6_oif = oif;
	fl6.flowi6_mark = sk->sk_mark;
	fl6.flowi6_uid = sk->sk_uid;
	fl6.fl6_icmp_type = user_icmph.icmp6_type;
	fl6.fl6_icmp_code = user_icmph.icmp6_code;
	security_sk_classify_flow(sk, flowi6_to_flowi_common(&fl6));

	ipcm6_init_sk(&ipc6, np);
	ipc6.sockc.mark = sk->sk_mark;
	fl6.flowlabel = ip6_make_flowinfo(ipc6.tclass, fl6.flowlabel);

	dst = ip6_sk_dst_lookup_flow(sk, &fl6, daddr, false);
	if (IS_ERR(dst))
		return PTR_ERR(dst);
	rt = (struct rt6_info *) dst;

	if (!fl6.flowi6_oif && ipv6_addr_is_multicast(&fl6.daddr))
		fl6.flowi6_oif = np->mcast_oif;
	else if (!fl6.flowi6_oif)
		fl6.flowi6_oif = np->ucast_oif;

	pfh.icmph.type = user_icmph.icmp6_type;
	pfh.icmph.code = user_icmph.icmp6_code;
	pfh.icmph.checksum = 0;
	pfh.icmph.un.echo.id = inet->inet_sport;
	pfh.icmph.un.echo.sequence = user_icmph.icmp6_sequence;
	pfh.msg = msg;
	pfh.wcheck = 0;
	pfh.family = AF_INET6;

	ipc6.hlimit = ip6_sk_dst_hoplimit(np, &fl6, dst);

	lock_sock(sk);
	err = ip6_append_data(sk, ping_getfrag, &pfh, len,
			      0, &ipc6, &fl6, rt,
			      MSG_DONTWAIT);

	if (err) {
		ICMP6_INC_STATS(sock_net(sk), rt->rt6i_idev,
				ICMP6_MIB_OUTERRORS);
		ip6_flush_pending_frames(sk);
	} else {
		icmpv6_push_pending_frames(sk, &fl6,
					   (struct icmp6hdr *)&pfh.icmph, len);
	}
	release_sock(sk);

	dst_release(dst);

	if (err)
		return err;

	return len;
}

struct proto pingv6_prot = {
	.name =		"PINGv6",
	.owner =	THIS_MODULE,
	.init =		ping_init_sock,
	.close =	ping_close,
	.connect =	ip6_datagram_connect_v6_only,
	.disconnect =	__udp_disconnect,
	.setsockopt =	ipv6_setsockopt,
	.getsockopt =	ipv6_getsockopt,
	.sendmsg =	ping_v6_sendmsg,
	.recvmsg =	ping_recvmsg,
	.bind =		ping_bind,
	.backlog_rcv =	ping_queue_rcv_skb,
	.hash =		ping_hash,
	.unhash =	ping_unhash,
	.get_port =	ping_get_port,
	.obj_size =	sizeof(struct raw6_sock),
};
EXPORT_SYMBOL_GPL(pingv6_prot);

static struct inet_protosw pingv6_protosw = {
	.type =      SOCK_DGRAM,
	.protocol =  IPPROTO_ICMPV6,
	.prot =      &pingv6_prot,
	.ops =       &inet6_sockraw_ops,
	.flags =     INET_PROTOSW_REUSE,
};

#ifdef CONFIG_PROC_FS
static void *ping_v6_seq_start(struct seq_file *seq, loff_t *pos)
{
	return ping_seq_start(seq, pos, AF_INET6);
}

static int ping_v6_seq_show(struct seq_file *seq, void *v)
{
	if (v == SEQ_START_TOKEN) {
		seq_puts(seq, IPV6_SEQ_DGRAM_HEADER);
	} else {
		int bucket = ((struct ping_iter_state *) seq->private)->bucket;
		struct inet_sock *inet = inet_sk(v);
		__u16 srcp = ntohs(inet->inet_sport);
		__u16 destp = ntohs(inet->inet_dport);
		ip6_dgram_sock_seq_show(seq, v, srcp, destp, bucket);
	}
	return 0;
}

static const struct seq_operations ping_v6_seq_ops = {
	.start		= ping_v6_seq_start,
	.show		= ping_v6_seq_show,
	.next		= ping_seq_next,
	.stop		= ping_seq_stop,
};

static int __net_init ping_v6_proc_init_net(struct net *net)
{
	if (!proc_create_net("icmp6", 0444, net->proc_net, &ping_v6_seq_ops,
			sizeof(struct ping_iter_state)))
		return -ENOMEM;
	return 0;
}

static void __net_exit ping_v6_proc_exit_net(struct net *net)
{
	remove_proc_entry("icmp6", net->proc_net);
}

static struct pernet_operations ping_v6_net_ops = {
	.init = ping_v6_proc_init_net,
	.exit = ping_v6_proc_exit_net,
};
#endif

int __init pingv6_init(void)
{
#ifdef CONFIG_PROC_FS
	int ret = register_pernet_subsys(&ping_v6_net_ops);
	if (ret)
		return ret;
#endif
	pingv6_ops.ipv6_recv_error = ipv6_recv_error;
	pingv6_ops.ip6_datagram_recv_common_ctl = ip6_datagram_recv_common_ctl;
	pingv6_ops.ip6_datagram_recv_specific_ctl =
		ip6_datagram_recv_specific_ctl;
	pingv6_ops.icmpv6_err_convert = icmpv6_err_convert;
	pingv6_ops.ipv6_icmp_error = ipv6_icmp_error;
	pingv6_ops.ipv6_chk_addr = ipv6_chk_addr;
	return inet6_register_protosw(&pingv6_protosw);
}

/* This never gets called because it's not possible to unload the ipv6 module,
 * but just in case.
 */
void pingv6_exit(void)
{
	pingv6_ops.ipv6_recv_error = dummy_ipv6_recv_error;
	pingv6_ops.ip6_datagram_recv_common_ctl = dummy_ip6_datagram_recv_ctl;
	pingv6_ops.ip6_datagram_recv_specific_ctl = dummy_ip6_datagram_recv_ctl;
	pingv6_ops.icmpv6_err_convert = dummy_icmpv6_err_convert;
	pingv6_ops.ipv6_icmp_error = dummy_ipv6_icmp_error;
	pingv6_ops.ipv6_chk_addr = dummy_ipv6_chk_addr;
#ifdef CONFIG_PROC_FS
	unregister_pernet_subsys(&ping_v6_net_ops);
#endif
	inet6_unregister_protosw(&pingv6_protosw);
}
