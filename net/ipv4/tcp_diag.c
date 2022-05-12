// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * tcp_diag.c	Module for monitoring TCP transport protocols sockets.
 *
 * Authors:	Alexey Kuznetsov, <kuznet@ms2.inr.ac.ru>
 */

#include <linux/module.h>
#include <linux/net.h>
#include <linux/sock_diag.h>
#include <linux/inet_diag.h>

#include <linux/tcp.h>

#include <net/netlink.h>
#include <net/tcp.h>

static void tcp_diag_get_info(struct sock *sk, struct inet_diag_msg *r,
			      void *_info)
{
	struct tcp_info *info = _info;

	if (inet_sk_state_load(sk) == TCP_LISTEN) {
		r->idiag_rqueue = READ_ONCE(sk->sk_ack_backlog);
		r->idiag_wqueue = READ_ONCE(sk->sk_max_ack_backlog);
	} else if (sk->sk_type == SOCK_STREAM) {
		const struct tcp_sock *tp = tcp_sk(sk);

		r->idiag_rqueue = max_t(int, READ_ONCE(tp->rcv_nxt) -
					     READ_ONCE(tp->copied_seq), 0);
		r->idiag_wqueue = READ_ONCE(tp->write_seq) - tp->snd_una;
	}
	if (info)
		tcp_get_info(sk, info);
}

#ifdef CONFIG_TCP_MD5SIG
static void tcp_diag_md5sig_fill(struct tcp_diag_md5sig *info,
				 const struct tcp_md5sig_key *key)
{
	info->tcpm_family = key->family;
	info->tcpm_prefixlen = key->prefixlen;
	info->tcpm_keylen = key->keylen;
	memcpy(info->tcpm_key, key->key, key->keylen);

	if (key->family == AF_INET)
		info->tcpm_addr[0] = key->addr.a4.s_addr;
	#if IS_ENABLED(CONFIG_IPV6)
	else if (key->family == AF_INET6)
		memcpy(&info->tcpm_addr, &key->addr.a6,
		       sizeof(info->tcpm_addr));
	#endif
}

static int tcp_diag_put_md5sig(struct sk_buff *skb,
			       const struct tcp_md5sig_info *md5sig)
{
	const struct tcp_md5sig_key *key;
	struct tcp_diag_md5sig *info;
	struct nlattr *attr;
	int md5sig_count = 0;

	hlist_for_each_entry_rcu(key, &md5sig->head, node)
		md5sig_count++;
	if (md5sig_count == 0)
		return 0;

	attr = nla_reserve(skb, INET_DIAG_MD5SIG,
			   md5sig_count * sizeof(struct tcp_diag_md5sig));
	if (!attr)
		return -EMSGSIZE;

	info = nla_data(attr);
	memset(info, 0, md5sig_count * sizeof(struct tcp_diag_md5sig));
	hlist_for_each_entry_rcu(key, &md5sig->head, node) {
		tcp_diag_md5sig_fill(info++, key);
		if (--md5sig_count == 0)
			break;
	}

	return 0;
}
#endif

static int tcp_diag_put_ulp(struct sk_buff *skb, struct sock *sk,
			    const struct tcp_ulp_ops *ulp_ops)
{
	struct nlattr *nest;
	int err;

	nest = nla_nest_start_noflag(skb, INET_DIAG_ULP_INFO);
	if (!nest)
		return -EMSGSIZE;

	err = nla_put_string(skb, INET_ULP_INFO_NAME, ulp_ops->name);
	if (err)
		goto nla_failure;

	if (ulp_ops->get_info)
		err = ulp_ops->get_info(sk, skb);
	if (err)
		goto nla_failure;

	nla_nest_end(skb, nest);
	return 0;

nla_failure:
	nla_nest_cancel(skb, nest);
	return err;
}

static int tcp_diag_get_aux(struct sock *sk, bool net_admin,
			    struct sk_buff *skb)
{
	struct inet_connection_sock *icsk = inet_csk(sk);
	int err = 0;

#ifdef CONFIG_TCP_MD5SIG
	if (net_admin) {
		struct tcp_md5sig_info *md5sig;

		rcu_read_lock();
		md5sig = rcu_dereference(tcp_sk(sk)->md5sig_info);
		if (md5sig)
			err = tcp_diag_put_md5sig(skb, md5sig);
		rcu_read_unlock();
		if (err < 0)
			return err;
	}
#endif

	if (net_admin) {
		const struct tcp_ulp_ops *ulp_ops;

		ulp_ops = icsk->icsk_ulp_ops;
		if (ulp_ops)
			err = tcp_diag_put_ulp(skb, sk, ulp_ops);
		if (err)
			return err;
	}
	return 0;
}

static size_t tcp_diag_get_aux_size(struct sock *sk, bool net_admin)
{
	struct inet_connection_sock *icsk = inet_csk(sk);
	size_t size = 0;

#ifdef CONFIG_TCP_MD5SIG
	if (net_admin && sk_fullsock(sk)) {
		const struct tcp_md5sig_info *md5sig;
		const struct tcp_md5sig_key *key;
		size_t md5sig_count = 0;

		rcu_read_lock();
		md5sig = rcu_dereference(tcp_sk(sk)->md5sig_info);
		if (md5sig) {
			hlist_for_each_entry_rcu(key, &md5sig->head, node)
				md5sig_count++;
		}
		rcu_read_unlock();
		size += nla_total_size(md5sig_count *
				       sizeof(struct tcp_diag_md5sig));
	}
#endif

	if (net_admin && sk_fullsock(sk)) {
		const struct tcp_ulp_ops *ulp_ops;

		ulp_ops = icsk->icsk_ulp_ops;
		if (ulp_ops) {
			size += nla_total_size(0) +
				nla_total_size(TCP_ULP_NAME_MAX);
			if (ulp_ops->get_info_size)
				size += ulp_ops->get_info_size(sk);
		}
	}
	return size;
}

static void tcp_diag_dump(struct sk_buff *skb, struct netlink_callback *cb,
			  const struct inet_diag_req_v2 *r)
{
	inet_diag_dump_icsk(&tcp_hashinfo, skb, cb, r);
}

static int tcp_diag_dump_one(struct netlink_callback *cb,
			     const struct inet_diag_req_v2 *req)
{
	return inet_diag_dump_one_icsk(&tcp_hashinfo, cb, req);
}

#ifdef CONFIG_INET_DIAG_DESTROY
static int tcp_diag_destroy(struct sk_buff *in_skb,
			    const struct inet_diag_req_v2 *req)
{
	struct net *net = sock_net(in_skb->sk);
	struct sock *sk = inet_diag_find_one_icsk(net, &tcp_hashinfo, req);
	int err;

	if (IS_ERR(sk))
		return PTR_ERR(sk);

	err = sock_diag_destroy(sk, ECONNABORTED);

	sock_gen_put(sk);

	return err;
}
#endif

static const struct inet_diag_handler tcp_diag_handler = {
	.dump			= tcp_diag_dump,
	.dump_one		= tcp_diag_dump_one,
	.idiag_get_info		= tcp_diag_get_info,
	.idiag_get_aux		= tcp_diag_get_aux,
	.idiag_get_aux_size	= tcp_diag_get_aux_size,
	.idiag_type		= IPPROTO_TCP,
	.idiag_info_size	= sizeof(struct tcp_info),
#ifdef CONFIG_INET_DIAG_DESTROY
	.destroy		= tcp_diag_destroy,
#endif
};

static int __init tcp_diag_init(void)
{
	return inet_diag_register(&tcp_diag_handler);
}

static void __exit tcp_diag_exit(void)
{
	inet_diag_unregister(&tcp_diag_handler);
}

module_init(tcp_diag_init);
module_exit(tcp_diag_exit);
MODULE_LICENSE("GPL");
MODULE_ALIAS_NET_PF_PROTO_TYPE(PF_NETLINK, NETLINK_SOCK_DIAG, 2-6 /* AF_INET - IPPROTO_TCP */);
