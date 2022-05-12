// SPDX-License-Identifier: GPL-2.0-only
/*
 * linux/net/sunrpc/svcsock.c
 *
 * These are the RPC server socket internals.
 *
 * The server scheduling algorithm does not always distribute the load
 * evenly when servicing a single client. May need to modify the
 * svc_xprt_enqueue procedure...
 *
 * TCP support is largely untested and may be a little slow. The problem
 * is that we currently do two separate recvfrom's, one for the 4-byte
 * record length, and the second for the actual record. This could possibly
 * be improved by always reading a minimum size of around 100 bytes and
 * tucking any superfluous bytes away in a temporary store. Still, that
 * leaves write requests out in the rain. An alternative may be to peek at
 * the first skb in the queue, and if it matches the next TCP sequence
 * number, to extract the record marker. Yuck.
 *
 * Copyright (C) 1995, 1996 Olaf Kirch <okir@monad.swb.de>
 */

#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/fcntl.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/inet.h>
#include <linux/udp.h>
#include <linux/tcp.h>
#include <linux/unistd.h>
#include <linux/slab.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/file.h>
#include <linux/freezer.h>
#include <net/sock.h>
#include <net/checksum.h>
#include <net/ip.h>
#include <net/ipv6.h>
#include <net/udp.h>
#include <net/tcp.h>
#include <net/tcp_states.h>
#include <linux/uaccess.h>
#include <linux/highmem.h>
#include <asm/ioctls.h>

#include <linux/sunrpc/types.h>
#include <linux/sunrpc/clnt.h>
#include <linux/sunrpc/xdr.h>
#include <linux/sunrpc/msg_prot.h>
#include <linux/sunrpc/svcsock.h>
#include <linux/sunrpc/stats.h>
#include <linux/sunrpc/xprt.h>

#include <trace/events/sunrpc.h>

#include "socklib.h"
#include "sunrpc.h"

#define RPCDBG_FACILITY	RPCDBG_SVCXPRT


static struct svc_sock *svc_setup_socket(struct svc_serv *, struct socket *,
					 int flags);
static int		svc_udp_recvfrom(struct svc_rqst *);
static int		svc_udp_sendto(struct svc_rqst *);
static void		svc_sock_detach(struct svc_xprt *);
static void		svc_tcp_sock_detach(struct svc_xprt *);
static void		svc_sock_free(struct svc_xprt *);

static struct svc_xprt *svc_create_socket(struct svc_serv *, int,
					  struct net *, struct sockaddr *,
					  int, int);
#ifdef CONFIG_DEBUG_LOCK_ALLOC
static struct lock_class_key svc_key[2];
static struct lock_class_key svc_slock_key[2];

static void svc_reclassify_socket(struct socket *sock)
{
	struct sock *sk = sock->sk;

	if (WARN_ON_ONCE(!sock_allow_reclassification(sk)))
		return;

	switch (sk->sk_family) {
	case AF_INET:
		sock_lock_init_class_and_name(sk, "slock-AF_INET-NFSD",
					      &svc_slock_key[0],
					      "sk_xprt.xpt_lock-AF_INET-NFSD",
					      &svc_key[0]);
		break;

	case AF_INET6:
		sock_lock_init_class_and_name(sk, "slock-AF_INET6-NFSD",
					      &svc_slock_key[1],
					      "sk_xprt.xpt_lock-AF_INET6-NFSD",
					      &svc_key[1]);
		break;

	default:
		BUG();
	}
}
#else
static void svc_reclassify_socket(struct socket *sock)
{
}
#endif

/**
 * svc_tcp_release_rqst - Release transport-related resources
 * @rqstp: request structure with resources to be released
 *
 */
static void svc_tcp_release_rqst(struct svc_rqst *rqstp)
{
	struct sk_buff *skb = rqstp->rq_xprt_ctxt;

	if (skb) {
		struct svc_sock *svsk =
			container_of(rqstp->rq_xprt, struct svc_sock, sk_xprt);

		rqstp->rq_xprt_ctxt = NULL;
		skb_free_datagram_locked(svsk->sk_sk, skb);
	}
}

/**
 * svc_udp_release_rqst - Release transport-related resources
 * @rqstp: request structure with resources to be released
 *
 */
static void svc_udp_release_rqst(struct svc_rqst *rqstp)
{
	struct sk_buff *skb = rqstp->rq_xprt_ctxt;

	if (skb) {
		rqstp->rq_xprt_ctxt = NULL;
		consume_skb(skb);
	}
}

union svc_pktinfo_u {
	struct in_pktinfo pkti;
	struct in6_pktinfo pkti6;
};
#define SVC_PKTINFO_SPACE \
	CMSG_SPACE(sizeof(union svc_pktinfo_u))

static void svc_set_cmsg_data(struct svc_rqst *rqstp, struct cmsghdr *cmh)
{
	struct svc_sock *svsk =
		container_of(rqstp->rq_xprt, struct svc_sock, sk_xprt);
	switch (svsk->sk_sk->sk_family) {
	case AF_INET: {
			struct in_pktinfo *pki = CMSG_DATA(cmh);

			cmh->cmsg_level = SOL_IP;
			cmh->cmsg_type = IP_PKTINFO;
			pki->ipi_ifindex = 0;
			pki->ipi_spec_dst.s_addr =
				 svc_daddr_in(rqstp)->sin_addr.s_addr;
			cmh->cmsg_len = CMSG_LEN(sizeof(*pki));
		}
		break;

	case AF_INET6: {
			struct in6_pktinfo *pki = CMSG_DATA(cmh);
			struct sockaddr_in6 *daddr = svc_daddr_in6(rqstp);

			cmh->cmsg_level = SOL_IPV6;
			cmh->cmsg_type = IPV6_PKTINFO;
			pki->ipi6_ifindex = daddr->sin6_scope_id;
			pki->ipi6_addr = daddr->sin6_addr;
			cmh->cmsg_len = CMSG_LEN(sizeof(*pki));
		}
		break;
	}
}

static int svc_sock_result_payload(struct svc_rqst *rqstp, unsigned int offset,
				   unsigned int length)
{
	return 0;
}

/*
 * Report socket names for nfsdfs
 */
static int svc_one_sock_name(struct svc_sock *svsk, char *buf, int remaining)
{
	const struct sock *sk = svsk->sk_sk;
	const char *proto_name = sk->sk_protocol == IPPROTO_UDP ?
							"udp" : "tcp";
	int len;

	switch (sk->sk_family) {
	case PF_INET:
		len = snprintf(buf, remaining, "ipv4 %s %pI4 %d\n",
				proto_name,
				&inet_sk(sk)->inet_rcv_saddr,
				inet_sk(sk)->inet_num);
		break;
#if IS_ENABLED(CONFIG_IPV6)
	case PF_INET6:
		len = snprintf(buf, remaining, "ipv6 %s %pI6 %d\n",
				proto_name,
				&sk->sk_v6_rcv_saddr,
				inet_sk(sk)->inet_num);
		break;
#endif
	default:
		len = snprintf(buf, remaining, "*unknown-%d*\n",
				sk->sk_family);
	}

	if (len >= remaining) {
		*buf = '\0';
		return -ENAMETOOLONG;
	}
	return len;
}

#if ARCH_IMPLEMENTS_FLUSH_DCACHE_PAGE
static void svc_flush_bvec(const struct bio_vec *bvec, size_t size, size_t seek)
{
	struct bvec_iter bi = {
		.bi_size	= size + seek,
	};
	struct bio_vec bv;

	bvec_iter_advance(bvec, &bi, seek & PAGE_MASK);
	for_each_bvec(bv, bvec, bi, bi)
		flush_dcache_page(bv.bv_page);
}
#else
static inline void svc_flush_bvec(const struct bio_vec *bvec, size_t size,
				  size_t seek)
{
}
#endif

/*
 * Read from @rqstp's transport socket. The incoming message fills whole
 * pages in @rqstp's rq_pages array until the last page of the message
 * has been received into a partial page.
 */
static ssize_t svc_tcp_read_msg(struct svc_rqst *rqstp, size_t buflen,
				size_t seek)
{
	struct svc_sock *svsk =
		container_of(rqstp->rq_xprt, struct svc_sock, sk_xprt);
	struct bio_vec *bvec = rqstp->rq_bvec;
	struct msghdr msg = { NULL };
	unsigned int i;
	ssize_t len;
	size_t t;

	rqstp->rq_xprt_hlen = 0;

	clear_bit(XPT_DATA, &svsk->sk_xprt.xpt_flags);

	for (i = 0, t = 0; t < buflen; i++, t += PAGE_SIZE) {
		bvec[i].bv_page = rqstp->rq_pages[i];
		bvec[i].bv_len = PAGE_SIZE;
		bvec[i].bv_offset = 0;
	}
	rqstp->rq_respages = &rqstp->rq_pages[i];
	rqstp->rq_next_page = rqstp->rq_respages + 1;

	iov_iter_bvec(&msg.msg_iter, READ, bvec, i, buflen);
	if (seek) {
		iov_iter_advance(&msg.msg_iter, seek);
		buflen -= seek;
	}
	len = sock_recvmsg(svsk->sk_sock, &msg, MSG_DONTWAIT);
	if (len > 0)
		svc_flush_bvec(bvec, len, seek);

	/* If we read a full record, then assume there may be more
	 * data to read (stream based sockets only!)
	 */
	if (len == buflen)
		set_bit(XPT_DATA, &svsk->sk_xprt.xpt_flags);

	return len;
}

/*
 * Set socket snd and rcv buffer lengths
 */
static void svc_sock_setbufsize(struct svc_sock *svsk, unsigned int nreqs)
{
	unsigned int max_mesg = svsk->sk_xprt.xpt_server->sv_max_mesg;
	struct socket *sock = svsk->sk_sock;

	nreqs = min(nreqs, INT_MAX / 2 / max_mesg);

	lock_sock(sock->sk);
	sock->sk->sk_sndbuf = nreqs * max_mesg * 2;
	sock->sk->sk_rcvbuf = nreqs * max_mesg * 2;
	sock->sk->sk_write_space(sock->sk);
	release_sock(sock->sk);
}

static void svc_sock_secure_port(struct svc_rqst *rqstp)
{
	if (svc_port_is_privileged(svc_addr(rqstp)))
		set_bit(RQ_SECURE, &rqstp->rq_flags);
	else
		clear_bit(RQ_SECURE, &rqstp->rq_flags);
}

/*
 * INET callback when data has been received on the socket.
 */
static void svc_data_ready(struct sock *sk)
{
	struct svc_sock	*svsk = (struct svc_sock *)sk->sk_user_data;

	if (svsk) {
		/* Refer to svc_setup_socket() for details. */
		rmb();
		svsk->sk_odata(sk);
		trace_svcsock_data_ready(&svsk->sk_xprt, 0);
		if (!test_and_set_bit(XPT_DATA, &svsk->sk_xprt.xpt_flags))
			svc_xprt_enqueue(&svsk->sk_xprt);
	}
}

/*
 * INET callback when space is newly available on the socket.
 */
static void svc_write_space(struct sock *sk)
{
	struct svc_sock	*svsk = (struct svc_sock *)(sk->sk_user_data);

	if (svsk) {
		/* Refer to svc_setup_socket() for details. */
		rmb();
		trace_svcsock_write_space(&svsk->sk_xprt, 0);
		svsk->sk_owspace(sk);
		svc_xprt_enqueue(&svsk->sk_xprt);
	}
}

static int svc_tcp_has_wspace(struct svc_xprt *xprt)
{
	struct svc_sock *svsk = container_of(xprt, struct svc_sock, sk_xprt);

	if (test_bit(XPT_LISTENER, &xprt->xpt_flags))
		return 1;
	return !test_bit(SOCK_NOSPACE, &svsk->sk_sock->flags);
}

static void svc_tcp_kill_temp_xprt(struct svc_xprt *xprt)
{
	struct svc_sock *svsk = container_of(xprt, struct svc_sock, sk_xprt);

	sock_no_linger(svsk->sk_sock->sk);
}

/*
 * See net/ipv6/ip_sockglue.c : ip_cmsg_recv_pktinfo
 */
static int svc_udp_get_dest_address4(struct svc_rqst *rqstp,
				     struct cmsghdr *cmh)
{
	struct in_pktinfo *pki = CMSG_DATA(cmh);
	struct sockaddr_in *daddr = svc_daddr_in(rqstp);

	if (cmh->cmsg_type != IP_PKTINFO)
		return 0;

	daddr->sin_family = AF_INET;
	daddr->sin_addr.s_addr = pki->ipi_spec_dst.s_addr;
	return 1;
}

/*
 * See net/ipv6/datagram.c : ip6_datagram_recv_ctl
 */
static int svc_udp_get_dest_address6(struct svc_rqst *rqstp,
				     struct cmsghdr *cmh)
{
	struct in6_pktinfo *pki = CMSG_DATA(cmh);
	struct sockaddr_in6 *daddr = svc_daddr_in6(rqstp);

	if (cmh->cmsg_type != IPV6_PKTINFO)
		return 0;

	daddr->sin6_family = AF_INET6;
	daddr->sin6_addr = pki->ipi6_addr;
	daddr->sin6_scope_id = pki->ipi6_ifindex;
	return 1;
}

/*
 * Copy the UDP datagram's destination address to the rqstp structure.
 * The 'destination' address in this case is the address to which the
 * peer sent the datagram, i.e. our local address. For multihomed
 * hosts, this can change from msg to msg. Note that only the IP
 * address changes, the port number should remain the same.
 */
static int svc_udp_get_dest_address(struct svc_rqst *rqstp,
				    struct cmsghdr *cmh)
{
	switch (cmh->cmsg_level) {
	case SOL_IP:
		return svc_udp_get_dest_address4(rqstp, cmh);
	case SOL_IPV6:
		return svc_udp_get_dest_address6(rqstp, cmh);
	}

	return 0;
}

/**
 * svc_udp_recvfrom - Receive a datagram from a UDP socket.
 * @rqstp: request structure into which to receive an RPC Call
 *
 * Called in a loop when XPT_DATA has been set.
 *
 * Returns:
 *   On success, the number of bytes in a received RPC Call, or
 *   %0 if a complete RPC Call message was not ready to return
 */
static int svc_udp_recvfrom(struct svc_rqst *rqstp)
{
	struct svc_sock	*svsk =
		container_of(rqstp->rq_xprt, struct svc_sock, sk_xprt);
	struct svc_serv	*serv = svsk->sk_xprt.xpt_server;
	struct sk_buff	*skb;
	union {
		struct cmsghdr	hdr;
		long		all[SVC_PKTINFO_SPACE / sizeof(long)];
	} buffer;
	struct cmsghdr *cmh = &buffer.hdr;
	struct msghdr msg = {
		.msg_name = svc_addr(rqstp),
		.msg_control = cmh,
		.msg_controllen = sizeof(buffer),
		.msg_flags = MSG_DONTWAIT,
	};
	size_t len;
	int err;

	if (test_and_clear_bit(XPT_CHNGBUF, &svsk->sk_xprt.xpt_flags))
	    /* udp sockets need large rcvbuf as all pending
	     * requests are still in that buffer.  sndbuf must
	     * also be large enough that there is enough space
	     * for one reply per thread.  We count all threads
	     * rather than threads in a particular pool, which
	     * provides an upper bound on the number of threads
	     * which will access the socket.
	     */
	    svc_sock_setbufsize(svsk, serv->sv_nrthreads + 3);

	clear_bit(XPT_DATA, &svsk->sk_xprt.xpt_flags);
	err = kernel_recvmsg(svsk->sk_sock, &msg, NULL,
			     0, 0, MSG_PEEK | MSG_DONTWAIT);
	if (err < 0)
		goto out_recv_err;
	skb = skb_recv_udp(svsk->sk_sk, 0, 1, &err);
	if (!skb)
		goto out_recv_err;

	len = svc_addr_len(svc_addr(rqstp));
	rqstp->rq_addrlen = len;
	if (skb->tstamp == 0) {
		skb->tstamp = ktime_get_real();
		/* Don't enable netstamp, sunrpc doesn't
		   need that much accuracy */
	}
	sock_write_timestamp(svsk->sk_sk, skb->tstamp);
	set_bit(XPT_DATA, &svsk->sk_xprt.xpt_flags); /* there may be more data... */

	len = skb->len;
	rqstp->rq_arg.len = len;
	trace_svcsock_udp_recv(&svsk->sk_xprt, len);

	rqstp->rq_prot = IPPROTO_UDP;

	if (!svc_udp_get_dest_address(rqstp, cmh))
		goto out_cmsg_err;
	rqstp->rq_daddrlen = svc_addr_len(svc_daddr(rqstp));

	if (skb_is_nonlinear(skb)) {
		/* we have to copy */
		local_bh_disable();
		if (csum_partial_copy_to_xdr(&rqstp->rq_arg, skb))
			goto out_bh_enable;
		local_bh_enable();
		consume_skb(skb);
	} else {
		/* we can use it in-place */
		rqstp->rq_arg.head[0].iov_base = skb->data;
		rqstp->rq_arg.head[0].iov_len = len;
		if (skb_checksum_complete(skb))
			goto out_free;
		rqstp->rq_xprt_ctxt = skb;
	}

	rqstp->rq_arg.page_base = 0;
	if (len <= rqstp->rq_arg.head[0].iov_len) {
		rqstp->rq_arg.head[0].iov_len = len;
		rqstp->rq_arg.page_len = 0;
		rqstp->rq_respages = rqstp->rq_pages+1;
	} else {
		rqstp->rq_arg.page_len = len - rqstp->rq_arg.head[0].iov_len;
		rqstp->rq_respages = rqstp->rq_pages + 1 +
			DIV_ROUND_UP(rqstp->rq_arg.page_len, PAGE_SIZE);
	}
	rqstp->rq_next_page = rqstp->rq_respages+1;

	if (serv->sv_stats)
		serv->sv_stats->netudpcnt++;

	svc_xprt_received(rqstp->rq_xprt);
	return len;

out_recv_err:
	if (err != -EAGAIN) {
		/* possibly an icmp error */
		set_bit(XPT_DATA, &svsk->sk_xprt.xpt_flags);
	}
	trace_svcsock_udp_recv_err(&svsk->sk_xprt, err);
	goto out_clear_busy;
out_cmsg_err:
	net_warn_ratelimited("svc: received unknown control message %d/%d; dropping RPC reply datagram\n",
			     cmh->cmsg_level, cmh->cmsg_type);
	goto out_free;
out_bh_enable:
	local_bh_enable();
out_free:
	kfree_skb(skb);
out_clear_busy:
	svc_xprt_received(rqstp->rq_xprt);
	return 0;
}

/**
 * svc_udp_sendto - Send out a reply on a UDP socket
 * @rqstp: completed svc_rqst
 *
 * xpt_mutex ensures @rqstp's whole message is written to the socket
 * without interruption.
 *
 * Returns the number of bytes sent, or a negative errno.
 */
static int svc_udp_sendto(struct svc_rqst *rqstp)
{
	struct svc_xprt *xprt = rqstp->rq_xprt;
	struct svc_sock	*svsk = container_of(xprt, struct svc_sock, sk_xprt);
	struct xdr_buf *xdr = &rqstp->rq_res;
	union {
		struct cmsghdr	hdr;
		long		all[SVC_PKTINFO_SPACE / sizeof(long)];
	} buffer;
	struct cmsghdr *cmh = &buffer.hdr;
	struct msghdr msg = {
		.msg_name	= &rqstp->rq_addr,
		.msg_namelen	= rqstp->rq_addrlen,
		.msg_control	= cmh,
		.msg_controllen	= sizeof(buffer),
	};
	unsigned int sent;
	int err;

	svc_udp_release_rqst(rqstp);

	svc_set_cmsg_data(rqstp, cmh);

	mutex_lock(&xprt->xpt_mutex);

	if (svc_xprt_is_dead(xprt))
		goto out_notconn;

	err = xprt_sock_sendmsg(svsk->sk_sock, &msg, xdr, 0, 0, &sent);
	xdr_free_bvec(xdr);
	if (err == -ECONNREFUSED) {
		/* ICMP error on earlier request. */
		err = xprt_sock_sendmsg(svsk->sk_sock, &msg, xdr, 0, 0, &sent);
		xdr_free_bvec(xdr);
	}
	trace_svcsock_udp_send(xprt, err);

	mutex_unlock(&xprt->xpt_mutex);
	if (err < 0)
		return err;
	return sent;

out_notconn:
	mutex_unlock(&xprt->xpt_mutex);
	return -ENOTCONN;
}

static int svc_udp_has_wspace(struct svc_xprt *xprt)
{
	struct svc_sock *svsk = container_of(xprt, struct svc_sock, sk_xprt);
	struct svc_serv	*serv = xprt->xpt_server;
	unsigned long required;

	/*
	 * Set the SOCK_NOSPACE flag before checking the available
	 * sock space.
	 */
	set_bit(SOCK_NOSPACE, &svsk->sk_sock->flags);
	required = atomic_read(&svsk->sk_xprt.xpt_reserved) + serv->sv_max_mesg;
	if (required*2 > sock_wspace(svsk->sk_sk))
		return 0;
	clear_bit(SOCK_NOSPACE, &svsk->sk_sock->flags);
	return 1;
}

static struct svc_xprt *svc_udp_accept(struct svc_xprt *xprt)
{
	BUG();
	return NULL;
}

static void svc_udp_kill_temp_xprt(struct svc_xprt *xprt)
{
}

static struct svc_xprt *svc_udp_create(struct svc_serv *serv,
				       struct net *net,
				       struct sockaddr *sa, int salen,
				       int flags)
{
	return svc_create_socket(serv, IPPROTO_UDP, net, sa, salen, flags);
}

static const struct svc_xprt_ops svc_udp_ops = {
	.xpo_create = svc_udp_create,
	.xpo_recvfrom = svc_udp_recvfrom,
	.xpo_sendto = svc_udp_sendto,
	.xpo_result_payload = svc_sock_result_payload,
	.xpo_release_rqst = svc_udp_release_rqst,
	.xpo_detach = svc_sock_detach,
	.xpo_free = svc_sock_free,
	.xpo_has_wspace = svc_udp_has_wspace,
	.xpo_accept = svc_udp_accept,
	.xpo_secure_port = svc_sock_secure_port,
	.xpo_kill_temp_xprt = svc_udp_kill_temp_xprt,
};

static struct svc_xprt_class svc_udp_class = {
	.xcl_name = "udp",
	.xcl_owner = THIS_MODULE,
	.xcl_ops = &svc_udp_ops,
	.xcl_max_payload = RPCSVC_MAXPAYLOAD_UDP,
	.xcl_ident = XPRT_TRANSPORT_UDP,
};

static void svc_udp_init(struct svc_sock *svsk, struct svc_serv *serv)
{
	svc_xprt_init(sock_net(svsk->sk_sock->sk), &svc_udp_class,
		      &svsk->sk_xprt, serv);
	clear_bit(XPT_CACHE_AUTH, &svsk->sk_xprt.xpt_flags);
	svsk->sk_sk->sk_data_ready = svc_data_ready;
	svsk->sk_sk->sk_write_space = svc_write_space;

	/* initialise setting must have enough space to
	 * receive and respond to one request.
	 * svc_udp_recvfrom will re-adjust if necessary
	 */
	svc_sock_setbufsize(svsk, 3);

	/* data might have come in before data_ready set up */
	set_bit(XPT_DATA, &svsk->sk_xprt.xpt_flags);
	set_bit(XPT_CHNGBUF, &svsk->sk_xprt.xpt_flags);

	/* make sure we get destination address info */
	switch (svsk->sk_sk->sk_family) {
	case AF_INET:
		ip_sock_set_pktinfo(svsk->sk_sock->sk);
		break;
	case AF_INET6:
		ip6_sock_set_recvpktinfo(svsk->sk_sock->sk);
		break;
	default:
		BUG();
	}
}

/*
 * A data_ready event on a listening socket means there's a connection
 * pending. Do not use state_change as a substitute for it.
 */
static void svc_tcp_listen_data_ready(struct sock *sk)
{
	struct svc_sock	*svsk = (struct svc_sock *)sk->sk_user_data;

	if (svsk) {
		/* Refer to svc_setup_socket() for details. */
		rmb();
		svsk->sk_odata(sk);
	}

	/*
	 * This callback may called twice when a new connection
	 * is established as a child socket inherits everything
	 * from a parent LISTEN socket.
	 * 1) data_ready method of the parent socket will be called
	 *    when one of child sockets become ESTABLISHED.
	 * 2) data_ready method of the child socket may be called
	 *    when it receives data before the socket is accepted.
	 * In case of 2, we should ignore it silently.
	 */
	if (sk->sk_state == TCP_LISTEN) {
		if (svsk) {
			set_bit(XPT_CONN, &svsk->sk_xprt.xpt_flags);
			svc_xprt_enqueue(&svsk->sk_xprt);
		}
	}
}

/*
 * A state change on a connected socket means it's dying or dead.
 */
static void svc_tcp_state_change(struct sock *sk)
{
	struct svc_sock	*svsk = (struct svc_sock *)sk->sk_user_data;

	if (svsk) {
		/* Refer to svc_setup_socket() for details. */
		rmb();
		svsk->sk_ostate(sk);
		trace_svcsock_tcp_state(&svsk->sk_xprt, svsk->sk_sock);
		if (sk->sk_state != TCP_ESTABLISHED)
			svc_xprt_deferred_close(&svsk->sk_xprt);
	}
}

/*
 * Accept a TCP connection
 */
static struct svc_xprt *svc_tcp_accept(struct svc_xprt *xprt)
{
	struct svc_sock *svsk = container_of(xprt, struct svc_sock, sk_xprt);
	struct sockaddr_storage addr;
	struct sockaddr	*sin = (struct sockaddr *) &addr;
	struct svc_serv	*serv = svsk->sk_xprt.xpt_server;
	struct socket	*sock = svsk->sk_sock;
	struct socket	*newsock;
	struct svc_sock	*newsvsk;
	int		err, slen;

	if (!sock)
		return NULL;

	clear_bit(XPT_CONN, &svsk->sk_xprt.xpt_flags);
	err = kernel_accept(sock, &newsock, O_NONBLOCK);
	if (err < 0) {
		if (err == -ENOMEM)
			printk(KERN_WARNING "%s: no more sockets!\n",
			       serv->sv_name);
		else if (err != -EAGAIN)
			net_warn_ratelimited("%s: accept failed (err %d)!\n",
					     serv->sv_name, -err);
		trace_svcsock_accept_err(xprt, serv->sv_name, err);
		return NULL;
	}
	set_bit(XPT_CONN, &svsk->sk_xprt.xpt_flags);

	err = kernel_getpeername(newsock, sin);
	if (err < 0) {
		trace_svcsock_getpeername_err(xprt, serv->sv_name, err);
		goto failed;		/* aborted connection or whatever */
	}
	slen = err;

	/* Reset the inherited callbacks before calling svc_setup_socket */
	newsock->sk->sk_state_change = svsk->sk_ostate;
	newsock->sk->sk_data_ready = svsk->sk_odata;
	newsock->sk->sk_write_space = svsk->sk_owspace;

	/* make sure that a write doesn't block forever when
	 * low on memory
	 */
	newsock->sk->sk_sndtimeo = HZ*30;

	newsvsk = svc_setup_socket(serv, newsock,
				 (SVC_SOCK_ANONYMOUS | SVC_SOCK_TEMPORARY));
	if (IS_ERR(newsvsk))
		goto failed;
	svc_xprt_set_remote(&newsvsk->sk_xprt, sin, slen);
	err = kernel_getsockname(newsock, sin);
	slen = err;
	if (unlikely(err < 0))
		slen = offsetof(struct sockaddr, sa_data);
	svc_xprt_set_local(&newsvsk->sk_xprt, sin, slen);

	if (sock_is_loopback(newsock->sk))
		set_bit(XPT_LOCAL, &newsvsk->sk_xprt.xpt_flags);
	else
		clear_bit(XPT_LOCAL, &newsvsk->sk_xprt.xpt_flags);
	if (serv->sv_stats)
		serv->sv_stats->nettcpconn++;

	return &newsvsk->sk_xprt;

failed:
	sock_release(newsock);
	return NULL;
}

static size_t svc_tcp_restore_pages(struct svc_sock *svsk,
				    struct svc_rqst *rqstp)
{
	size_t len = svsk->sk_datalen;
	unsigned int i, npages;

	if (!len)
		return 0;
	npages = (len + PAGE_SIZE - 1) >> PAGE_SHIFT;
	for (i = 0; i < npages; i++) {
		if (rqstp->rq_pages[i] != NULL)
			put_page(rqstp->rq_pages[i]);
		BUG_ON(svsk->sk_pages[i] == NULL);
		rqstp->rq_pages[i] = svsk->sk_pages[i];
		svsk->sk_pages[i] = NULL;
	}
	rqstp->rq_arg.head[0].iov_base = page_address(rqstp->rq_pages[0]);
	return len;
}

static void svc_tcp_save_pages(struct svc_sock *svsk, struct svc_rqst *rqstp)
{
	unsigned int i, len, npages;

	if (svsk->sk_datalen == 0)
		return;
	len = svsk->sk_datalen;
	npages = (len + PAGE_SIZE - 1) >> PAGE_SHIFT;
	for (i = 0; i < npages; i++) {
		svsk->sk_pages[i] = rqstp->rq_pages[i];
		rqstp->rq_pages[i] = NULL;
	}
}

static void svc_tcp_clear_pages(struct svc_sock *svsk)
{
	unsigned int i, len, npages;

	if (svsk->sk_datalen == 0)
		goto out;
	len = svsk->sk_datalen;
	npages = (len + PAGE_SIZE - 1) >> PAGE_SHIFT;
	for (i = 0; i < npages; i++) {
		if (svsk->sk_pages[i] == NULL) {
			WARN_ON_ONCE(1);
			continue;
		}
		put_page(svsk->sk_pages[i]);
		svsk->sk_pages[i] = NULL;
	}
out:
	svsk->sk_tcplen = 0;
	svsk->sk_datalen = 0;
}

/*
 * Receive fragment record header into sk_marker.
 */
static ssize_t svc_tcp_read_marker(struct svc_sock *svsk,
				   struct svc_rqst *rqstp)
{
	ssize_t want, len;

	/* If we haven't gotten the record length yet,
	 * get the next four bytes.
	 */
	if (svsk->sk_tcplen < sizeof(rpc_fraghdr)) {
		struct msghdr	msg = { NULL };
		struct kvec	iov;

		want = sizeof(rpc_fraghdr) - svsk->sk_tcplen;
		iov.iov_base = ((char *)&svsk->sk_marker) + svsk->sk_tcplen;
		iov.iov_len  = want;
		iov_iter_kvec(&msg.msg_iter, READ, &iov, 1, want);
		len = sock_recvmsg(svsk->sk_sock, &msg, MSG_DONTWAIT);
		if (len < 0)
			return len;
		svsk->sk_tcplen += len;
		if (len < want) {
			/* call again to read the remaining bytes */
			goto err_short;
		}
		trace_svcsock_marker(&svsk->sk_xprt, svsk->sk_marker);
		if (svc_sock_reclen(svsk) + svsk->sk_datalen >
		    svsk->sk_xprt.xpt_server->sv_max_mesg)
			goto err_too_large;
	}
	return svc_sock_reclen(svsk);

err_too_large:
	net_notice_ratelimited("svc: %s %s RPC fragment too large: %d\n",
			       __func__, svsk->sk_xprt.xpt_server->sv_name,
			       svc_sock_reclen(svsk));
	svc_xprt_deferred_close(&svsk->sk_xprt);
err_short:
	return -EAGAIN;
}

static int receive_cb_reply(struct svc_sock *svsk, struct svc_rqst *rqstp)
{
	struct rpc_xprt *bc_xprt = svsk->sk_xprt.xpt_bc_xprt;
	struct rpc_rqst *req = NULL;
	struct kvec *src, *dst;
	__be32 *p = (__be32 *)rqstp->rq_arg.head[0].iov_base;
	__be32 xid;
	__be32 calldir;

	xid = *p++;
	calldir = *p;

	if (!bc_xprt)
		return -EAGAIN;
	spin_lock(&bc_xprt->queue_lock);
	req = xprt_lookup_rqst(bc_xprt, xid);
	if (!req)
		goto unlock_notfound;

	memcpy(&req->rq_private_buf, &req->rq_rcv_buf, sizeof(struct xdr_buf));
	/*
	 * XXX!: cheating for now!  Only copying HEAD.
	 * But we know this is good enough for now (in fact, for any
	 * callback reply in the forseeable future).
	 */
	dst = &req->rq_private_buf.head[0];
	src = &rqstp->rq_arg.head[0];
	if (dst->iov_len < src->iov_len)
		goto unlock_eagain; /* whatever; just giving up. */
	memcpy(dst->iov_base, src->iov_base, src->iov_len);
	xprt_complete_rqst(req->rq_task, rqstp->rq_arg.len);
	rqstp->rq_arg.len = 0;
	spin_unlock(&bc_xprt->queue_lock);
	return 0;
unlock_notfound:
	printk(KERN_NOTICE
		"%s: Got unrecognized reply: "
		"calldir 0x%x xpt_bc_xprt %p xid %08x\n",
		__func__, ntohl(calldir),
		bc_xprt, ntohl(xid));
unlock_eagain:
	spin_unlock(&bc_xprt->queue_lock);
	return -EAGAIN;
}

static void svc_tcp_fragment_received(struct svc_sock *svsk)
{
	/* If we have more data, signal svc_xprt_enqueue() to try again */
	svsk->sk_tcplen = 0;
	svsk->sk_marker = xdr_zero;
}

/**
 * svc_tcp_recvfrom - Receive data from a TCP socket
 * @rqstp: request structure into which to receive an RPC Call
 *
 * Called in a loop when XPT_DATA has been set.
 *
 * Read the 4-byte stream record marker, then use the record length
 * in that marker to set up exactly the resources needed to receive
 * the next RPC message into @rqstp.
 *
 * Returns:
 *   On success, the number of bytes in a received RPC Call, or
 *   %0 if a complete RPC Call message was not ready to return
 *
 * The zero return case handles partial receives and callback Replies.
 * The state of a partial receive is preserved in the svc_sock for
 * the next call to svc_tcp_recvfrom.
 */
static int svc_tcp_recvfrom(struct svc_rqst *rqstp)
{
	struct svc_sock	*svsk =
		container_of(rqstp->rq_xprt, struct svc_sock, sk_xprt);
	struct svc_serv	*serv = svsk->sk_xprt.xpt_server;
	size_t want, base;
	ssize_t len;
	__be32 *p;
	__be32 calldir;

	clear_bit(XPT_DATA, &svsk->sk_xprt.xpt_flags);
	len = svc_tcp_read_marker(svsk, rqstp);
	if (len < 0)
		goto error;

	base = svc_tcp_restore_pages(svsk, rqstp);
	want = len - (svsk->sk_tcplen - sizeof(rpc_fraghdr));
	len = svc_tcp_read_msg(rqstp, base + want, base);
	if (len >= 0) {
		trace_svcsock_tcp_recv(&svsk->sk_xprt, len);
		svsk->sk_tcplen += len;
		svsk->sk_datalen += len;
	}
	if (len != want || !svc_sock_final_rec(svsk))
		goto err_incomplete;
	if (svsk->sk_datalen < 8)
		goto err_nuts;

	rqstp->rq_arg.len = svsk->sk_datalen;
	rqstp->rq_arg.page_base = 0;
	if (rqstp->rq_arg.len <= rqstp->rq_arg.head[0].iov_len) {
		rqstp->rq_arg.head[0].iov_len = rqstp->rq_arg.len;
		rqstp->rq_arg.page_len = 0;
	} else
		rqstp->rq_arg.page_len = rqstp->rq_arg.len - rqstp->rq_arg.head[0].iov_len;

	rqstp->rq_xprt_ctxt   = NULL;
	rqstp->rq_prot	      = IPPROTO_TCP;
	if (test_bit(XPT_LOCAL, &svsk->sk_xprt.xpt_flags))
		set_bit(RQ_LOCAL, &rqstp->rq_flags);
	else
		clear_bit(RQ_LOCAL, &rqstp->rq_flags);

	p = (__be32 *)rqstp->rq_arg.head[0].iov_base;
	calldir = p[1];
	if (calldir)
		len = receive_cb_reply(svsk, rqstp);

	/* Reset TCP read info */
	svsk->sk_datalen = 0;
	svc_tcp_fragment_received(svsk);

	if (len < 0)
		goto error;

	svc_xprt_copy_addrs(rqstp, &svsk->sk_xprt);
	if (serv->sv_stats)
		serv->sv_stats->nettcpcnt++;

	svc_xprt_received(rqstp->rq_xprt);
	return rqstp->rq_arg.len;

err_incomplete:
	svc_tcp_save_pages(svsk, rqstp);
	if (len < 0 && len != -EAGAIN)
		goto err_delete;
	if (len == want)
		svc_tcp_fragment_received(svsk);
	else
		trace_svcsock_tcp_recv_short(&svsk->sk_xprt,
				svc_sock_reclen(svsk),
				svsk->sk_tcplen - sizeof(rpc_fraghdr));
	goto err_noclose;
error:
	if (len != -EAGAIN)
		goto err_delete;
	trace_svcsock_tcp_recv_eagain(&svsk->sk_xprt, 0);
	goto err_noclose;
err_nuts:
	svsk->sk_datalen = 0;
err_delete:
	trace_svcsock_tcp_recv_err(&svsk->sk_xprt, len);
	svc_xprt_deferred_close(&svsk->sk_xprt);
err_noclose:
	svc_xprt_received(rqstp->rq_xprt);
	return 0;	/* record not complete */
}

static int svc_tcp_send_kvec(struct socket *sock, const struct kvec *vec,
			      int flags)
{
	return kernel_sendpage(sock, virt_to_page(vec->iov_base),
			       offset_in_page(vec->iov_base),
			       vec->iov_len, flags);
}

/*
 * kernel_sendpage() is used exclusively to reduce the number of
 * copy operations in this path. Therefore the caller must ensure
 * that the pages backing @xdr are unchanging.
 *
 * In addition, the logic assumes that * .bv_len is never larger
 * than PAGE_SIZE.
 */
static int svc_tcp_sendmsg(struct socket *sock, struct xdr_buf *xdr,
			   rpc_fraghdr marker, unsigned int *sentp)
{
	const struct kvec *head = xdr->head;
	const struct kvec *tail = xdr->tail;
	struct kvec rm = {
		.iov_base	= &marker,
		.iov_len	= sizeof(marker),
	};
	struct msghdr msg = {
		.msg_flags	= 0,
	};
	int ret;

	*sentp = 0;
	xdr_alloc_bvec(xdr, GFP_KERNEL);

	ret = kernel_sendmsg(sock, &msg, &rm, 1, rm.iov_len);
	if (ret < 0)
		return ret;
	*sentp += ret;
	if (ret != rm.iov_len)
		return -EAGAIN;

	ret = svc_tcp_send_kvec(sock, head, 0);
	if (ret < 0)
		return ret;
	*sentp += ret;
	if (ret != head->iov_len)
		goto out;

	if (xdr->page_len) {
		unsigned int offset, len, remaining;
		struct bio_vec *bvec;

		bvec = xdr->bvec + (xdr->page_base >> PAGE_SHIFT);
		offset = offset_in_page(xdr->page_base);
		remaining = xdr->page_len;
		while (remaining > 0) {
			len = min(remaining, bvec->bv_len - offset);
			ret = kernel_sendpage(sock, bvec->bv_page,
					      bvec->bv_offset + offset,
					      len, 0);
			if (ret < 0)
				return ret;
			*sentp += ret;
			if (ret != len)
				goto out;
			remaining -= len;
			offset = 0;
			bvec++;
		}
	}

	if (tail->iov_len) {
		ret = svc_tcp_send_kvec(sock, tail, 0);
		if (ret < 0)
			return ret;
		*sentp += ret;
	}

out:
	return 0;
}

/**
 * svc_tcp_sendto - Send out a reply on a TCP socket
 * @rqstp: completed svc_rqst
 *
 * xpt_mutex ensures @rqstp's whole message is written to the socket
 * without interruption.
 *
 * Returns the number of bytes sent, or a negative errno.
 */
static int svc_tcp_sendto(struct svc_rqst *rqstp)
{
	struct svc_xprt *xprt = rqstp->rq_xprt;
	struct svc_sock	*svsk = container_of(xprt, struct svc_sock, sk_xprt);
	struct xdr_buf *xdr = &rqstp->rq_res;
	rpc_fraghdr marker = cpu_to_be32(RPC_LAST_STREAM_FRAGMENT |
					 (u32)xdr->len);
	unsigned int sent;
	int err;

	svc_tcp_release_rqst(rqstp);

	atomic_inc(&svsk->sk_sendqlen);
	mutex_lock(&xprt->xpt_mutex);
	if (svc_xprt_is_dead(xprt))
		goto out_notconn;
	tcp_sock_set_cork(svsk->sk_sk, true);
	err = svc_tcp_sendmsg(svsk->sk_sock, xdr, marker, &sent);
	xdr_free_bvec(xdr);
	trace_svcsock_tcp_send(xprt, err < 0 ? (long)err : sent);
	if (err < 0 || sent != (xdr->len + sizeof(marker)))
		goto out_close;
	if (atomic_dec_and_test(&svsk->sk_sendqlen))
		tcp_sock_set_cork(svsk->sk_sk, false);
	mutex_unlock(&xprt->xpt_mutex);
	return sent;

out_notconn:
	atomic_dec(&svsk->sk_sendqlen);
	mutex_unlock(&xprt->xpt_mutex);
	return -ENOTCONN;
out_close:
	pr_notice("rpc-srv/tcp: %s: %s %d when sending %d bytes - shutting down socket\n",
		  xprt->xpt_server->sv_name,
		  (err < 0) ? "got error" : "sent",
		  (err < 0) ? err : sent, xdr->len);
	svc_xprt_deferred_close(xprt);
	atomic_dec(&svsk->sk_sendqlen);
	mutex_unlock(&xprt->xpt_mutex);
	return -EAGAIN;
}

static struct svc_xprt *svc_tcp_create(struct svc_serv *serv,
				       struct net *net,
				       struct sockaddr *sa, int salen,
				       int flags)
{
	return svc_create_socket(serv, IPPROTO_TCP, net, sa, salen, flags);
}

static const struct svc_xprt_ops svc_tcp_ops = {
	.xpo_create = svc_tcp_create,
	.xpo_recvfrom = svc_tcp_recvfrom,
	.xpo_sendto = svc_tcp_sendto,
	.xpo_result_payload = svc_sock_result_payload,
	.xpo_release_rqst = svc_tcp_release_rqst,
	.xpo_detach = svc_tcp_sock_detach,
	.xpo_free = svc_sock_free,
	.xpo_has_wspace = svc_tcp_has_wspace,
	.xpo_accept = svc_tcp_accept,
	.xpo_secure_port = svc_sock_secure_port,
	.xpo_kill_temp_xprt = svc_tcp_kill_temp_xprt,
};

static struct svc_xprt_class svc_tcp_class = {
	.xcl_name = "tcp",
	.xcl_owner = THIS_MODULE,
	.xcl_ops = &svc_tcp_ops,
	.xcl_max_payload = RPCSVC_MAXPAYLOAD_TCP,
	.xcl_ident = XPRT_TRANSPORT_TCP,
};

void svc_init_xprt_sock(void)
{
	svc_reg_xprt_class(&svc_tcp_class);
	svc_reg_xprt_class(&svc_udp_class);
}

void svc_cleanup_xprt_sock(void)
{
	svc_unreg_xprt_class(&svc_tcp_class);
	svc_unreg_xprt_class(&svc_udp_class);
}

static void svc_tcp_init(struct svc_sock *svsk, struct svc_serv *serv)
{
	struct sock	*sk = svsk->sk_sk;

	svc_xprt_init(sock_net(svsk->sk_sock->sk), &svc_tcp_class,
		      &svsk->sk_xprt, serv);
	set_bit(XPT_CACHE_AUTH, &svsk->sk_xprt.xpt_flags);
	set_bit(XPT_CONG_CTRL, &svsk->sk_xprt.xpt_flags);
	if (sk->sk_state == TCP_LISTEN) {
		strcpy(svsk->sk_xprt.xpt_remotebuf, "listener");
		set_bit(XPT_LISTENER, &svsk->sk_xprt.xpt_flags);
		sk->sk_data_ready = svc_tcp_listen_data_ready;
		set_bit(XPT_CONN, &svsk->sk_xprt.xpt_flags);
	} else {
		sk->sk_state_change = svc_tcp_state_change;
		sk->sk_data_ready = svc_data_ready;
		sk->sk_write_space = svc_write_space;

		svsk->sk_marker = xdr_zero;
		svsk->sk_tcplen = 0;
		svsk->sk_datalen = 0;
		memset(&svsk->sk_pages[0], 0, sizeof(svsk->sk_pages));

		tcp_sock_set_nodelay(sk);

		set_bit(XPT_DATA, &svsk->sk_xprt.xpt_flags);
		switch (sk->sk_state) {
		case TCP_SYN_RECV:
		case TCP_ESTABLISHED:
			break;
		default:
			svc_xprt_deferred_close(&svsk->sk_xprt);
		}
	}
}

void svc_sock_update_bufs(struct svc_serv *serv)
{
	/*
	 * The number of server threads has changed. Update
	 * rcvbuf and sndbuf accordingly on all sockets
	 */
	struct svc_sock *svsk;

	spin_lock_bh(&serv->sv_lock);
	list_for_each_entry(svsk, &serv->sv_permsocks, sk_xprt.xpt_list)
		set_bit(XPT_CHNGBUF, &svsk->sk_xprt.xpt_flags);
	spin_unlock_bh(&serv->sv_lock);
}
EXPORT_SYMBOL_GPL(svc_sock_update_bufs);

/*
 * Initialize socket for RPC use and create svc_sock struct
 */
static struct svc_sock *svc_setup_socket(struct svc_serv *serv,
						struct socket *sock,
						int flags)
{
	struct svc_sock	*svsk;
	struct sock	*inet;
	int		pmap_register = !(flags & SVC_SOCK_ANONYMOUS);
	int		err = 0;

	svsk = kzalloc(sizeof(*svsk), GFP_KERNEL);
	if (!svsk)
		return ERR_PTR(-ENOMEM);

	inet = sock->sk;

	/* Register socket with portmapper */
	if (pmap_register)
		err = svc_register(serv, sock_net(sock->sk), inet->sk_family,
				     inet->sk_protocol,
				     ntohs(inet_sk(inet)->inet_sport));

	if (err < 0) {
		kfree(svsk);
		return ERR_PTR(err);
	}

	svsk->sk_sock = sock;
	svsk->sk_sk = inet;
	svsk->sk_ostate = inet->sk_state_change;
	svsk->sk_odata = inet->sk_data_ready;
	svsk->sk_owspace = inet->sk_write_space;
	/*
	 * This barrier is necessary in order to prevent race condition
	 * with svc_data_ready(), svc_listen_data_ready() and others
	 * when calling callbacks above.
	 */
	wmb();
	inet->sk_user_data = svsk;

	/* Initialize the socket */
	if (sock->type == SOCK_DGRAM)
		svc_udp_init(svsk, serv);
	else
		svc_tcp_init(svsk, serv);

	trace_svcsock_new_socket(sock);
	return svsk;
}

bool svc_alien_sock(struct net *net, int fd)
{
	int err;
	struct socket *sock = sockfd_lookup(fd, &err);
	bool ret = false;

	if (!sock)
		goto out;
	if (sock_net(sock->sk) != net)
		ret = true;
	sockfd_put(sock);
out:
	return ret;
}
EXPORT_SYMBOL_GPL(svc_alien_sock);

/**
 * svc_addsock - add a listener socket to an RPC service
 * @serv: pointer to RPC service to which to add a new listener
 * @fd: file descriptor of the new listener
 * @name_return: pointer to buffer to fill in with name of listener
 * @len: size of the buffer
 * @cred: credential
 *
 * Fills in socket name and returns positive length of name if successful.
 * Name is terminated with '\n'.  On error, returns a negative errno
 * value.
 */
int svc_addsock(struct svc_serv *serv, const int fd, char *name_return,
		const size_t len, const struct cred *cred)
{
	int err = 0;
	struct socket *so = sockfd_lookup(fd, &err);
	struct svc_sock *svsk = NULL;
	struct sockaddr_storage addr;
	struct sockaddr *sin = (struct sockaddr *)&addr;
	int salen;

	if (!so)
		return err;
	err = -EAFNOSUPPORT;
	if ((so->sk->sk_family != PF_INET) && (so->sk->sk_family != PF_INET6))
		goto out;
	err =  -EPROTONOSUPPORT;
	if (so->sk->sk_protocol != IPPROTO_TCP &&
	    so->sk->sk_protocol != IPPROTO_UDP)
		goto out;
	err = -EISCONN;
	if (so->state > SS_UNCONNECTED)
		goto out;
	err = -ENOENT;
	if (!try_module_get(THIS_MODULE))
		goto out;
	svsk = svc_setup_socket(serv, so, SVC_SOCK_DEFAULTS);
	if (IS_ERR(svsk)) {
		module_put(THIS_MODULE);
		err = PTR_ERR(svsk);
		goto out;
	}
	salen = kernel_getsockname(svsk->sk_sock, sin);
	if (salen >= 0)
		svc_xprt_set_local(&svsk->sk_xprt, sin, salen);
	svsk->sk_xprt.xpt_cred = get_cred(cred);
	svc_add_new_perm_xprt(serv, &svsk->sk_xprt);
	return svc_one_sock_name(svsk, name_return, len);
out:
	sockfd_put(so);
	return err;
}
EXPORT_SYMBOL_GPL(svc_addsock);

/*
 * Create socket for RPC service.
 */
static struct svc_xprt *svc_create_socket(struct svc_serv *serv,
					  int protocol,
					  struct net *net,
					  struct sockaddr *sin, int len,
					  int flags)
{
	struct svc_sock	*svsk;
	struct socket	*sock;
	int		error;
	int		type;
	struct sockaddr_storage addr;
	struct sockaddr *newsin = (struct sockaddr *)&addr;
	int		newlen;
	int		family;

	if (protocol != IPPROTO_UDP && protocol != IPPROTO_TCP) {
		printk(KERN_WARNING "svc: only UDP and TCP "
				"sockets supported\n");
		return ERR_PTR(-EINVAL);
	}

	type = (protocol == IPPROTO_UDP)? SOCK_DGRAM : SOCK_STREAM;
	switch (sin->sa_family) {
	case AF_INET6:
		family = PF_INET6;
		break;
	case AF_INET:
		family = PF_INET;
		break;
	default:
		return ERR_PTR(-EINVAL);
	}

	error = __sock_create(net, family, type, protocol, &sock, 1);
	if (error < 0)
		return ERR_PTR(error);

	svc_reclassify_socket(sock);

	/*
	 * If this is an PF_INET6 listener, we want to avoid
	 * getting requests from IPv4 remotes.  Those should
	 * be shunted to a PF_INET listener via rpcbind.
	 */
	if (family == PF_INET6)
		ip6_sock_set_v6only(sock->sk);
	if (type == SOCK_STREAM)
		sock->sk->sk_reuse = SK_CAN_REUSE; /* allow address reuse */
	error = kernel_bind(sock, sin, len);
	if (error < 0)
		goto bummer;

	error = kernel_getsockname(sock, newsin);
	if (error < 0)
		goto bummer;
	newlen = error;

	if (protocol == IPPROTO_TCP) {
		if ((error = kernel_listen(sock, 64)) < 0)
			goto bummer;
	}

	svsk = svc_setup_socket(serv, sock, flags);
	if (IS_ERR(svsk)) {
		error = PTR_ERR(svsk);
		goto bummer;
	}
	svc_xprt_set_local(&svsk->sk_xprt, newsin, newlen);
	return (struct svc_xprt *)svsk;
bummer:
	sock_release(sock);
	return ERR_PTR(error);
}

/*
 * Detach the svc_sock from the socket so that no
 * more callbacks occur.
 */
static void svc_sock_detach(struct svc_xprt *xprt)
{
	struct svc_sock *svsk = container_of(xprt, struct svc_sock, sk_xprt);
	struct sock *sk = svsk->sk_sk;

	/* put back the old socket callbacks */
	lock_sock(sk);
	sk->sk_state_change = svsk->sk_ostate;
	sk->sk_data_ready = svsk->sk_odata;
	sk->sk_write_space = svsk->sk_owspace;
	sk->sk_user_data = NULL;
	release_sock(sk);
}

/*
 * Disconnect the socket, and reset the callbacks
 */
static void svc_tcp_sock_detach(struct svc_xprt *xprt)
{
	struct svc_sock *svsk = container_of(xprt, struct svc_sock, sk_xprt);

	svc_sock_detach(xprt);

	if (!test_bit(XPT_LISTENER, &xprt->xpt_flags)) {
		svc_tcp_clear_pages(svsk);
		kernel_sock_shutdown(svsk->sk_sock, SHUT_RDWR);
	}
}

/*
 * Free the svc_sock's socket resources and the svc_sock itself.
 */
static void svc_sock_free(struct svc_xprt *xprt)
{
	struct svc_sock *svsk = container_of(xprt, struct svc_sock, sk_xprt);

	if (svsk->sk_sock->file)
		sockfd_put(svsk->sk_sock);
	else
		sock_release(svsk->sk_sock);
	kfree(svsk);
}
