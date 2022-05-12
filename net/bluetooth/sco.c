/*
   BlueZ - Bluetooth protocol stack for Linux
   Copyright (C) 2000-2001 Qualcomm Incorporated

   Written 2000,2001 by Maxim Krasnyansky <maxk@qualcomm.com>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License version 2 as
   published by the Free Software Foundation;

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS.
   IN NO EVENT SHALL THE COPYRIGHT HOLDER(S) AND AUTHOR(S) BE LIABLE FOR ANY
   CLAIM, OR ANY SPECIAL INDIRECT OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES
   WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
   ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
   OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

   ALL LIABILITY, INCLUDING LIABILITY FOR INFRINGEMENT OF ANY PATENTS,
   COPYRIGHTS, TRADEMARKS OR OTHER RIGHTS, RELATING TO USE OF THIS
   SOFTWARE IS DISCLAIMED.
*/

/* Bluetooth SCO sockets. */

#include <linux/module.h>
#include <linux/debugfs.h>
#include <linux/seq_file.h>
#include <linux/sched/signal.h>

#include <net/bluetooth/bluetooth.h>
#include <net/bluetooth/hci_core.h>
#include <net/bluetooth/sco.h>

static bool disable_esco;

static const struct proto_ops sco_sock_ops;

static struct bt_sock_list sco_sk_list = {
	.lock = __RW_LOCK_UNLOCKED(sco_sk_list.lock)
};

/* ---- SCO connections ---- */
struct sco_conn {
	struct hci_conn	*hcon;

	spinlock_t	lock;
	struct sock	*sk;

	unsigned int    mtu;
};

#define sco_conn_lock(c)	spin_lock(&c->lock)
#define sco_conn_unlock(c)	spin_unlock(&c->lock)

static void sco_sock_close(struct sock *sk);
static void sco_sock_kill(struct sock *sk);

/* ----- SCO socket info ----- */
#define sco_pi(sk) ((struct sco_pinfo *) sk)

struct sco_pinfo {
	struct bt_sock	bt;
	bdaddr_t	src;
	bdaddr_t	dst;
	__u32		flags;
	__u16		setting;
	__u8		cmsg_mask;
	struct sco_conn	*conn;
};

/* ---- SCO timers ---- */
#define SCO_CONN_TIMEOUT	(HZ * 40)
#define SCO_DISCONN_TIMEOUT	(HZ * 2)

static void sco_sock_timeout(struct timer_list *t)
{
	struct sock *sk = from_timer(sk, t, sk_timer);

	BT_DBG("sock %p state %d", sk, sk->sk_state);

	bh_lock_sock(sk);
	sk->sk_err = ETIMEDOUT;
	sk->sk_state_change(sk);
	bh_unlock_sock(sk);

	sco_sock_kill(sk);
	sock_put(sk);
}

static void sco_sock_set_timer(struct sock *sk, long timeout)
{
	BT_DBG("sock %p state %d timeout %ld", sk, sk->sk_state, timeout);
	sk_reset_timer(sk, &sk->sk_timer, jiffies + timeout);
}

static void sco_sock_clear_timer(struct sock *sk)
{
	BT_DBG("sock %p state %d", sk, sk->sk_state);
	sk_stop_timer(sk, &sk->sk_timer);
}

/* ---- SCO connections ---- */
static struct sco_conn *sco_conn_add(struct hci_conn *hcon)
{
	struct hci_dev *hdev = hcon->hdev;
	struct sco_conn *conn = hcon->sco_data;

	if (conn)
		return conn;

	conn = kzalloc(sizeof(struct sco_conn), GFP_KERNEL);
	if (!conn)
		return NULL;

	spin_lock_init(&conn->lock);

	hcon->sco_data = conn;
	conn->hcon = hcon;

	if (hdev->sco_mtu > 0)
		conn->mtu = hdev->sco_mtu;
	else
		conn->mtu = 60;

	BT_DBG("hcon %p conn %p", hcon, conn);

	return conn;
}

/* Delete channel.
 * Must be called on the locked socket. */
static void sco_chan_del(struct sock *sk, int err)
{
	struct sco_conn *conn;

	conn = sco_pi(sk)->conn;

	BT_DBG("sk %p, conn %p, err %d", sk, conn, err);

	if (conn) {
		sco_conn_lock(conn);
		conn->sk = NULL;
		sco_pi(sk)->conn = NULL;
		sco_conn_unlock(conn);

		if (conn->hcon)
			hci_conn_drop(conn->hcon);
	}

	sk->sk_state = BT_CLOSED;
	sk->sk_err   = err;
	sk->sk_state_change(sk);

	sock_set_flag(sk, SOCK_ZAPPED);
}

static void sco_conn_del(struct hci_conn *hcon, int err)
{
	struct sco_conn *conn = hcon->sco_data;
	struct sock *sk;

	if (!conn)
		return;

	BT_DBG("hcon %p conn %p, err %d", hcon, conn, err);

	/* Kill socket */
	sco_conn_lock(conn);
	sk = conn->sk;
	sco_conn_unlock(conn);

	if (sk) {
		sock_hold(sk);
		bh_lock_sock(sk);
		sco_sock_clear_timer(sk);
		sco_chan_del(sk, err);
		bh_unlock_sock(sk);
		sco_sock_kill(sk);
		sock_put(sk);
	}

	hcon->sco_data = NULL;
	kfree(conn);
}

static void __sco_chan_add(struct sco_conn *conn, struct sock *sk,
			   struct sock *parent)
{
	BT_DBG("conn %p", conn);

	sco_pi(sk)->conn = conn;
	conn->sk = sk;

	if (parent)
		bt_accept_enqueue(parent, sk, true);
}

static int sco_chan_add(struct sco_conn *conn, struct sock *sk,
			struct sock *parent)
{
	int err = 0;

	sco_conn_lock(conn);
	if (conn->sk)
		err = -EBUSY;
	else
		__sco_chan_add(conn, sk, parent);

	sco_conn_unlock(conn);
	return err;
}

static int sco_connect(struct sock *sk)
{
	struct sco_conn *conn;
	struct hci_conn *hcon;
	struct hci_dev  *hdev;
	int err, type;

	BT_DBG("%pMR -> %pMR", &sco_pi(sk)->src, &sco_pi(sk)->dst);

	hdev = hci_get_route(&sco_pi(sk)->dst, &sco_pi(sk)->src, BDADDR_BREDR);
	if (!hdev)
		return -EHOSTUNREACH;

	hci_dev_lock(hdev);

	if (lmp_esco_capable(hdev) && !disable_esco)
		type = ESCO_LINK;
	else
		type = SCO_LINK;

	if (sco_pi(sk)->setting == BT_VOICE_TRANSPARENT &&
	    (!lmp_transp_capable(hdev) || !lmp_esco_capable(hdev))) {
		err = -EOPNOTSUPP;
		goto done;
	}

	hcon = hci_connect_sco(hdev, type, &sco_pi(sk)->dst,
			       sco_pi(sk)->setting);
	if (IS_ERR(hcon)) {
		err = PTR_ERR(hcon);
		goto done;
	}

	conn = sco_conn_add(hcon);
	if (!conn) {
		hci_conn_drop(hcon);
		err = -ENOMEM;
		goto done;
	}

	/* Update source addr of the socket */
	bacpy(&sco_pi(sk)->src, &hcon->src);

	err = sco_chan_add(conn, sk, NULL);
	if (err)
		goto done;

	if (hcon->state == BT_CONNECTED) {
		sco_sock_clear_timer(sk);
		sk->sk_state = BT_CONNECTED;
	} else {
		sk->sk_state = BT_CONNECT;
		sco_sock_set_timer(sk, sk->sk_sndtimeo);
	}

done:
	hci_dev_unlock(hdev);
	hci_dev_put(hdev);
	return err;
}

static int sco_send_frame(struct sock *sk, struct msghdr *msg, int len)
{
	struct sco_conn *conn = sco_pi(sk)->conn;
	struct sk_buff *skb;
	int err;

	/* Check outgoing MTU */
	if (len > conn->mtu)
		return -EINVAL;

	BT_DBG("sk %p len %d", sk, len);

	skb = bt_skb_send_alloc(sk, len, msg->msg_flags & MSG_DONTWAIT, &err);
	if (!skb)
		return err;

	if (memcpy_from_msg(skb_put(skb, len), msg, len)) {
		kfree_skb(skb);
		return -EFAULT;
	}

	hci_send_sco(conn->hcon, skb);

	return len;
}

static void sco_recv_frame(struct sco_conn *conn, struct sk_buff *skb)
{
	struct sock *sk;

	sco_conn_lock(conn);
	sk = conn->sk;
	sco_conn_unlock(conn);

	if (!sk)
		goto drop;

	BT_DBG("sk %p len %d", sk, skb->len);

	if (sk->sk_state != BT_CONNECTED)
		goto drop;

	if (!sock_queue_rcv_skb(sk, skb))
		return;

drop:
	kfree_skb(skb);
}

/* -------- Socket interface ---------- */
static struct sock *__sco_get_sock_listen_by_addr(bdaddr_t *ba)
{
	struct sock *sk;

	sk_for_each(sk, &sco_sk_list.head) {
		if (sk->sk_state != BT_LISTEN)
			continue;

		if (!bacmp(&sco_pi(sk)->src, ba))
			return sk;
	}

	return NULL;
}

/* Find socket listening on source bdaddr.
 * Returns closest match.
 */
static struct sock *sco_get_sock_listen(bdaddr_t *src)
{
	struct sock *sk = NULL, *sk1 = NULL;

	read_lock(&sco_sk_list.lock);

	sk_for_each(sk, &sco_sk_list.head) {
		if (sk->sk_state != BT_LISTEN)
			continue;

		/* Exact match. */
		if (!bacmp(&sco_pi(sk)->src, src))
			break;

		/* Closest match */
		if (!bacmp(&sco_pi(sk)->src, BDADDR_ANY))
			sk1 = sk;
	}

	read_unlock(&sco_sk_list.lock);

	return sk ? sk : sk1;
}

static void sco_sock_destruct(struct sock *sk)
{
	BT_DBG("sk %p", sk);

	skb_queue_purge(&sk->sk_receive_queue);
	skb_queue_purge(&sk->sk_write_queue);
}

static void sco_sock_cleanup_listen(struct sock *parent)
{
	struct sock *sk;

	BT_DBG("parent %p", parent);

	/* Close not yet accepted channels */
	while ((sk = bt_accept_dequeue(parent, NULL))) {
		sco_sock_close(sk);
		sco_sock_kill(sk);
	}

	parent->sk_state  = BT_CLOSED;
	sock_set_flag(parent, SOCK_ZAPPED);
}

/* Kill socket (only if zapped and orphan)
 * Must be called on unlocked socket.
 */
static void sco_sock_kill(struct sock *sk)
{
	if (!sock_flag(sk, SOCK_ZAPPED) || sk->sk_socket ||
	    sock_flag(sk, SOCK_DEAD))
		return;

	BT_DBG("sk %p state %d", sk, sk->sk_state);

	/* Kill poor orphan */
	bt_sock_unlink(&sco_sk_list, sk);
	sock_set_flag(sk, SOCK_DEAD);
	sock_put(sk);
}

static void __sco_sock_close(struct sock *sk)
{
	BT_DBG("sk %p state %d socket %p", sk, sk->sk_state, sk->sk_socket);

	switch (sk->sk_state) {
	case BT_LISTEN:
		sco_sock_cleanup_listen(sk);
		break;

	case BT_CONNECTED:
	case BT_CONFIG:
		if (sco_pi(sk)->conn->hcon) {
			sk->sk_state = BT_DISCONN;
			sco_sock_set_timer(sk, SCO_DISCONN_TIMEOUT);
			sco_conn_lock(sco_pi(sk)->conn);
			hci_conn_drop(sco_pi(sk)->conn->hcon);
			sco_pi(sk)->conn->hcon = NULL;
			sco_conn_unlock(sco_pi(sk)->conn);
		} else
			sco_chan_del(sk, ECONNRESET);
		break;

	case BT_CONNECT2:
	case BT_CONNECT:
	case BT_DISCONN:
		sco_chan_del(sk, ECONNRESET);
		break;

	default:
		sock_set_flag(sk, SOCK_ZAPPED);
		break;
	}
}

/* Must be called on unlocked socket. */
static void sco_sock_close(struct sock *sk)
{
	sco_sock_clear_timer(sk);
	lock_sock(sk);
	__sco_sock_close(sk);
	release_sock(sk);
	sco_sock_kill(sk);
}

static void sco_skb_put_cmsg(struct sk_buff *skb, struct msghdr *msg,
			     struct sock *sk)
{
	if (sco_pi(sk)->cmsg_mask & SCO_CMSG_PKT_STATUS)
		put_cmsg(msg, SOL_BLUETOOTH, BT_SCM_PKT_STATUS,
			 sizeof(bt_cb(skb)->sco.pkt_status),
			 &bt_cb(skb)->sco.pkt_status);
}

static void sco_sock_init(struct sock *sk, struct sock *parent)
{
	BT_DBG("sk %p", sk);

	if (parent) {
		sk->sk_type = parent->sk_type;
		bt_sk(sk)->flags = bt_sk(parent)->flags;
		security_sk_clone(parent, sk);
	} else {
		bt_sk(sk)->skb_put_cmsg = sco_skb_put_cmsg;
	}
}

static struct proto sco_proto = {
	.name		= "SCO",
	.owner		= THIS_MODULE,
	.obj_size	= sizeof(struct sco_pinfo)
};

static struct sock *sco_sock_alloc(struct net *net, struct socket *sock,
				   int proto, gfp_t prio, int kern)
{
	struct sock *sk;

	sk = sk_alloc(net, PF_BLUETOOTH, prio, &sco_proto, kern);
	if (!sk)
		return NULL;

	sock_init_data(sock, sk);
	INIT_LIST_HEAD(&bt_sk(sk)->accept_q);

	sk->sk_destruct = sco_sock_destruct;
	sk->sk_sndtimeo = SCO_CONN_TIMEOUT;

	sock_reset_flag(sk, SOCK_ZAPPED);

	sk->sk_protocol = proto;
	sk->sk_state    = BT_OPEN;

	sco_pi(sk)->setting = BT_VOICE_CVSD_16BIT;

	timer_setup(&sk->sk_timer, sco_sock_timeout, 0);

	bt_sock_link(&sco_sk_list, sk);
	return sk;
}

static int sco_sock_create(struct net *net, struct socket *sock, int protocol,
			   int kern)
{
	struct sock *sk;

	BT_DBG("sock %p", sock);

	sock->state = SS_UNCONNECTED;

	if (sock->type != SOCK_SEQPACKET)
		return -ESOCKTNOSUPPORT;

	sock->ops = &sco_sock_ops;

	sk = sco_sock_alloc(net, sock, protocol, GFP_ATOMIC, kern);
	if (!sk)
		return -ENOMEM;

	sco_sock_init(sk, NULL);
	return 0;
}

static int sco_sock_bind(struct socket *sock, struct sockaddr *addr,
			 int addr_len)
{
	struct sockaddr_sco *sa = (struct sockaddr_sco *) addr;
	struct sock *sk = sock->sk;
	int err = 0;

	if (!addr || addr_len < sizeof(struct sockaddr_sco) ||
	    addr->sa_family != AF_BLUETOOTH)
		return -EINVAL;

	BT_DBG("sk %p %pMR", sk, &sa->sco_bdaddr);

	lock_sock(sk);

	if (sk->sk_state != BT_OPEN) {
		err = -EBADFD;
		goto done;
	}

	if (sk->sk_type != SOCK_SEQPACKET) {
		err = -EINVAL;
		goto done;
	}

	bacpy(&sco_pi(sk)->src, &sa->sco_bdaddr);

	sk->sk_state = BT_BOUND;

done:
	release_sock(sk);
	return err;
}

static int sco_sock_connect(struct socket *sock, struct sockaddr *addr, int alen, int flags)
{
	struct sockaddr_sco *sa = (struct sockaddr_sco *) addr;
	struct sock *sk = sock->sk;
	int err;

	BT_DBG("sk %p", sk);

	if (alen < sizeof(struct sockaddr_sco) ||
	    addr->sa_family != AF_BLUETOOTH)
		return -EINVAL;

	if (sk->sk_state != BT_OPEN && sk->sk_state != BT_BOUND)
		return -EBADFD;

	if (sk->sk_type != SOCK_SEQPACKET)
		return -EINVAL;

	lock_sock(sk);

	/* Set destination address and psm */
	bacpy(&sco_pi(sk)->dst, &sa->sco_bdaddr);

	err = sco_connect(sk);
	if (err)
		goto done;

	err = bt_sock_wait_state(sk, BT_CONNECTED,
				 sock_sndtimeo(sk, flags & O_NONBLOCK));

done:
	release_sock(sk);
	return err;
}

static int sco_sock_listen(struct socket *sock, int backlog)
{
	struct sock *sk = sock->sk;
	bdaddr_t *src = &sco_pi(sk)->src;
	int err = 0;

	BT_DBG("sk %p backlog %d", sk, backlog);

	lock_sock(sk);

	if (sk->sk_state != BT_BOUND) {
		err = -EBADFD;
		goto done;
	}

	if (sk->sk_type != SOCK_SEQPACKET) {
		err = -EINVAL;
		goto done;
	}

	write_lock(&sco_sk_list.lock);

	if (__sco_get_sock_listen_by_addr(src)) {
		err = -EADDRINUSE;
		goto unlock;
	}

	sk->sk_max_ack_backlog = backlog;
	sk->sk_ack_backlog = 0;

	sk->sk_state = BT_LISTEN;

unlock:
	write_unlock(&sco_sk_list.lock);

done:
	release_sock(sk);
	return err;
}

static int sco_sock_accept(struct socket *sock, struct socket *newsock,
			   int flags, bool kern)
{
	DEFINE_WAIT_FUNC(wait, woken_wake_function);
	struct sock *sk = sock->sk, *ch;
	long timeo;
	int err = 0;

	lock_sock(sk);

	timeo = sock_rcvtimeo(sk, flags & O_NONBLOCK);

	BT_DBG("sk %p timeo %ld", sk, timeo);

	/* Wait for an incoming connection. (wake-one). */
	add_wait_queue_exclusive(sk_sleep(sk), &wait);
	while (1) {
		if (sk->sk_state != BT_LISTEN) {
			err = -EBADFD;
			break;
		}

		ch = bt_accept_dequeue(sk, newsock);
		if (ch)
			break;

		if (!timeo) {
			err = -EAGAIN;
			break;
		}

		if (signal_pending(current)) {
			err = sock_intr_errno(timeo);
			break;
		}

		release_sock(sk);

		timeo = wait_woken(&wait, TASK_INTERRUPTIBLE, timeo);
		lock_sock(sk);
	}
	remove_wait_queue(sk_sleep(sk), &wait);

	if (err)
		goto done;

	newsock->state = SS_CONNECTED;

	BT_DBG("new socket %p", ch);

done:
	release_sock(sk);
	return err;
}

static int sco_sock_getname(struct socket *sock, struct sockaddr *addr,
			    int peer)
{
	struct sockaddr_sco *sa = (struct sockaddr_sco *) addr;
	struct sock *sk = sock->sk;

	BT_DBG("sock %p, sk %p", sock, sk);

	addr->sa_family = AF_BLUETOOTH;

	if (peer)
		bacpy(&sa->sco_bdaddr, &sco_pi(sk)->dst);
	else
		bacpy(&sa->sco_bdaddr, &sco_pi(sk)->src);

	return sizeof(struct sockaddr_sco);
}

static int sco_sock_sendmsg(struct socket *sock, struct msghdr *msg,
			    size_t len)
{
	struct sock *sk = sock->sk;
	int err;

	BT_DBG("sock %p, sk %p", sock, sk);

	err = sock_error(sk);
	if (err)
		return err;

	if (msg->msg_flags & MSG_OOB)
		return -EOPNOTSUPP;

	lock_sock(sk);

	if (sk->sk_state == BT_CONNECTED)
		err = sco_send_frame(sk, msg, len);
	else
		err = -ENOTCONN;

	release_sock(sk);
	return err;
}

static void sco_conn_defer_accept(struct hci_conn *conn, u16 setting)
{
	struct hci_dev *hdev = conn->hdev;

	BT_DBG("conn %p", conn);

	conn->state = BT_CONFIG;

	if (!lmp_esco_capable(hdev)) {
		struct hci_cp_accept_conn_req cp;

		bacpy(&cp.bdaddr, &conn->dst);
		cp.role = 0x00; /* Ignored */

		hci_send_cmd(hdev, HCI_OP_ACCEPT_CONN_REQ, sizeof(cp), &cp);
	} else {
		struct hci_cp_accept_sync_conn_req cp;

		bacpy(&cp.bdaddr, &conn->dst);
		cp.pkt_type = cpu_to_le16(conn->pkt_type);

		cp.tx_bandwidth   = cpu_to_le32(0x00001f40);
		cp.rx_bandwidth   = cpu_to_le32(0x00001f40);
		cp.content_format = cpu_to_le16(setting);

		switch (setting & SCO_AIRMODE_MASK) {
		case SCO_AIRMODE_TRANSP:
			if (conn->pkt_type & ESCO_2EV3)
				cp.max_latency = cpu_to_le16(0x0008);
			else
				cp.max_latency = cpu_to_le16(0x000D);
			cp.retrans_effort = 0x02;
			break;
		case SCO_AIRMODE_CVSD:
			cp.max_latency = cpu_to_le16(0xffff);
			cp.retrans_effort = 0xff;
			break;
		}

		hci_send_cmd(hdev, HCI_OP_ACCEPT_SYNC_CONN_REQ,
			     sizeof(cp), &cp);
	}
}

static int sco_sock_recvmsg(struct socket *sock, struct msghdr *msg,
			    size_t len, int flags)
{
	struct sock *sk = sock->sk;
	struct sco_pinfo *pi = sco_pi(sk);

	lock_sock(sk);

	if (sk->sk_state == BT_CONNECT2 &&
	    test_bit(BT_SK_DEFER_SETUP, &bt_sk(sk)->flags)) {
		sco_conn_defer_accept(pi->conn->hcon, pi->setting);
		sk->sk_state = BT_CONFIG;

		release_sock(sk);
		return 0;
	}

	release_sock(sk);

	return bt_sock_recvmsg(sock, msg, len, flags);
}

static int sco_sock_setsockopt(struct socket *sock, int level, int optname,
			       sockptr_t optval, unsigned int optlen)
{
	struct sock *sk = sock->sk;
	int len, err = 0;
	struct bt_voice voice;
	u32 opt;

	BT_DBG("sk %p", sk);

	lock_sock(sk);

	switch (optname) {

	case BT_DEFER_SETUP:
		if (sk->sk_state != BT_BOUND && sk->sk_state != BT_LISTEN) {
			err = -EINVAL;
			break;
		}

		if (copy_from_sockptr(&opt, optval, sizeof(u32))) {
			err = -EFAULT;
			break;
		}

		if (opt)
			set_bit(BT_SK_DEFER_SETUP, &bt_sk(sk)->flags);
		else
			clear_bit(BT_SK_DEFER_SETUP, &bt_sk(sk)->flags);
		break;

	case BT_VOICE:
		if (sk->sk_state != BT_OPEN && sk->sk_state != BT_BOUND &&
		    sk->sk_state != BT_CONNECT2) {
			err = -EINVAL;
			break;
		}

		voice.setting = sco_pi(sk)->setting;

		len = min_t(unsigned int, sizeof(voice), optlen);
		if (copy_from_sockptr(&voice, optval, len)) {
			err = -EFAULT;
			break;
		}

		/* Explicitly check for these values */
		if (voice.setting != BT_VOICE_TRANSPARENT &&
		    voice.setting != BT_VOICE_CVSD_16BIT) {
			err = -EINVAL;
			break;
		}

		sco_pi(sk)->setting = voice.setting;
		break;

	case BT_PKT_STATUS:
		if (copy_from_sockptr(&opt, optval, sizeof(u32))) {
			err = -EFAULT;
			break;
		}

		if (opt)
			sco_pi(sk)->cmsg_mask |= SCO_CMSG_PKT_STATUS;
		else
			sco_pi(sk)->cmsg_mask &= SCO_CMSG_PKT_STATUS;
		break;

	default:
		err = -ENOPROTOOPT;
		break;
	}

	release_sock(sk);
	return err;
}

static int sco_sock_getsockopt_old(struct socket *sock, int optname,
				   char __user *optval, int __user *optlen)
{
	struct sock *sk = sock->sk;
	struct sco_options opts;
	struct sco_conninfo cinfo;
	int len, err = 0;

	BT_DBG("sk %p", sk);

	if (get_user(len, optlen))
		return -EFAULT;

	lock_sock(sk);

	switch (optname) {
	case SCO_OPTIONS:
		if (sk->sk_state != BT_CONNECTED &&
		    !(sk->sk_state == BT_CONNECT2 &&
		      test_bit(BT_SK_DEFER_SETUP, &bt_sk(sk)->flags))) {
			err = -ENOTCONN;
			break;
		}

		opts.mtu = sco_pi(sk)->conn->mtu;

		BT_DBG("mtu %d", opts.mtu);

		len = min_t(unsigned int, len, sizeof(opts));
		if (copy_to_user(optval, (char *)&opts, len))
			err = -EFAULT;

		break;

	case SCO_CONNINFO:
		if (sk->sk_state != BT_CONNECTED &&
		    !(sk->sk_state == BT_CONNECT2 &&
		      test_bit(BT_SK_DEFER_SETUP, &bt_sk(sk)->flags))) {
			err = -ENOTCONN;
			break;
		}

		memset(&cinfo, 0, sizeof(cinfo));
		cinfo.hci_handle = sco_pi(sk)->conn->hcon->handle;
		memcpy(cinfo.dev_class, sco_pi(sk)->conn->hcon->dev_class, 3);

		len = min_t(unsigned int, len, sizeof(cinfo));
		if (copy_to_user(optval, (char *)&cinfo, len))
			err = -EFAULT;

		break;

	default:
		err = -ENOPROTOOPT;
		break;
	}

	release_sock(sk);
	return err;
}

static int sco_sock_getsockopt(struct socket *sock, int level, int optname,
			       char __user *optval, int __user *optlen)
{
	struct sock *sk = sock->sk;
	int len, err = 0;
	struct bt_voice voice;
	u32 phys;
	int pkt_status;

	BT_DBG("sk %p", sk);

	if (level == SOL_SCO)
		return sco_sock_getsockopt_old(sock, optname, optval, optlen);

	if (get_user(len, optlen))
		return -EFAULT;

	lock_sock(sk);

	switch (optname) {

	case BT_DEFER_SETUP:
		if (sk->sk_state != BT_BOUND && sk->sk_state != BT_LISTEN) {
			err = -EINVAL;
			break;
		}

		if (put_user(test_bit(BT_SK_DEFER_SETUP, &bt_sk(sk)->flags),
			     (u32 __user *)optval))
			err = -EFAULT;

		break;

	case BT_VOICE:
		voice.setting = sco_pi(sk)->setting;

		len = min_t(unsigned int, len, sizeof(voice));
		if (copy_to_user(optval, (char *)&voice, len))
			err = -EFAULT;

		break;

	case BT_PHY:
		if (sk->sk_state != BT_CONNECTED) {
			err = -ENOTCONN;
			break;
		}

		phys = hci_conn_get_phy(sco_pi(sk)->conn->hcon);

		if (put_user(phys, (u32 __user *) optval))
			err = -EFAULT;
		break;

	case BT_PKT_STATUS:
		pkt_status = (sco_pi(sk)->cmsg_mask & SCO_CMSG_PKT_STATUS);

		if (put_user(pkt_status, (int __user *)optval))
			err = -EFAULT;
		break;

	case BT_SNDMTU:
	case BT_RCVMTU:
		if (sk->sk_state != BT_CONNECTED) {
			err = -ENOTCONN;
			break;
		}

		if (put_user(sco_pi(sk)->conn->mtu, (u32 __user *)optval))
			err = -EFAULT;
		break;

	default:
		err = -ENOPROTOOPT;
		break;
	}

	release_sock(sk);
	return err;
}

static int sco_sock_shutdown(struct socket *sock, int how)
{
	struct sock *sk = sock->sk;
	int err = 0;

	BT_DBG("sock %p, sk %p", sock, sk);

	if (!sk)
		return 0;

	sock_hold(sk);
	lock_sock(sk);

	if (!sk->sk_shutdown) {
		sk->sk_shutdown = SHUTDOWN_MASK;
		sco_sock_clear_timer(sk);
		__sco_sock_close(sk);

		if (sock_flag(sk, SOCK_LINGER) && sk->sk_lingertime &&
		    !(current->flags & PF_EXITING))
			err = bt_sock_wait_state(sk, BT_CLOSED,
						 sk->sk_lingertime);
	}

	release_sock(sk);
	sock_put(sk);

	return err;
}

static int sco_sock_release(struct socket *sock)
{
	struct sock *sk = sock->sk;
	int err = 0;

	BT_DBG("sock %p, sk %p", sock, sk);

	if (!sk)
		return 0;

	sco_sock_close(sk);

	if (sock_flag(sk, SOCK_LINGER) && sk->sk_lingertime &&
	    !(current->flags & PF_EXITING)) {
		lock_sock(sk);
		err = bt_sock_wait_state(sk, BT_CLOSED, sk->sk_lingertime);
		release_sock(sk);
	}

	sock_orphan(sk);
	sco_sock_kill(sk);
	return err;
}

static void sco_conn_ready(struct sco_conn *conn)
{
	struct sock *parent;
	struct sock *sk = conn->sk;

	BT_DBG("conn %p", conn);

	if (sk) {
		sco_sock_clear_timer(sk);
		bh_lock_sock(sk);
		sk->sk_state = BT_CONNECTED;
		sk->sk_state_change(sk);
		bh_unlock_sock(sk);
	} else {
		sco_conn_lock(conn);

		if (!conn->hcon) {
			sco_conn_unlock(conn);
			return;
		}

		parent = sco_get_sock_listen(&conn->hcon->src);
		if (!parent) {
			sco_conn_unlock(conn);
			return;
		}

		bh_lock_sock(parent);

		sk = sco_sock_alloc(sock_net(parent), NULL,
				    BTPROTO_SCO, GFP_ATOMIC, 0);
		if (!sk) {
			bh_unlock_sock(parent);
			sco_conn_unlock(conn);
			return;
		}

		sco_sock_init(sk, parent);

		bacpy(&sco_pi(sk)->src, &conn->hcon->src);
		bacpy(&sco_pi(sk)->dst, &conn->hcon->dst);

		hci_conn_hold(conn->hcon);
		__sco_chan_add(conn, sk, parent);

		if (test_bit(BT_SK_DEFER_SETUP, &bt_sk(parent)->flags))
			sk->sk_state = BT_CONNECT2;
		else
			sk->sk_state = BT_CONNECTED;

		/* Wake up parent */
		parent->sk_data_ready(parent);

		bh_unlock_sock(parent);

		sco_conn_unlock(conn);
	}
}

/* ----- SCO interface with lower layer (HCI) ----- */
int sco_connect_ind(struct hci_dev *hdev, bdaddr_t *bdaddr, __u8 *flags)
{
	struct sock *sk;
	int lm = 0;

	BT_DBG("hdev %s, bdaddr %pMR", hdev->name, bdaddr);

	/* Find listening sockets */
	read_lock(&sco_sk_list.lock);
	sk_for_each(sk, &sco_sk_list.head) {
		if (sk->sk_state != BT_LISTEN)
			continue;

		if (!bacmp(&sco_pi(sk)->src, &hdev->bdaddr) ||
		    !bacmp(&sco_pi(sk)->src, BDADDR_ANY)) {
			lm |= HCI_LM_ACCEPT;

			if (test_bit(BT_SK_DEFER_SETUP, &bt_sk(sk)->flags))
				*flags |= HCI_PROTO_DEFER;
			break;
		}
	}
	read_unlock(&sco_sk_list.lock);

	return lm;
}

static void sco_connect_cfm(struct hci_conn *hcon, __u8 status)
{
	if (hcon->type != SCO_LINK && hcon->type != ESCO_LINK)
		return;

	BT_DBG("hcon %p bdaddr %pMR status %d", hcon, &hcon->dst, status);

	if (!status) {
		struct sco_conn *conn;

		conn = sco_conn_add(hcon);
		if (conn)
			sco_conn_ready(conn);
	} else
		sco_conn_del(hcon, bt_to_errno(status));
}

static void sco_disconn_cfm(struct hci_conn *hcon, __u8 reason)
{
	if (hcon->type != SCO_LINK && hcon->type != ESCO_LINK)
		return;

	BT_DBG("hcon %p reason %d", hcon, reason);

	sco_conn_del(hcon, bt_to_errno(reason));
}

void sco_recv_scodata(struct hci_conn *hcon, struct sk_buff *skb)
{
	struct sco_conn *conn = hcon->sco_data;

	if (!conn)
		goto drop;

	BT_DBG("conn %p len %d", conn, skb->len);

	if (skb->len) {
		sco_recv_frame(conn, skb);
		return;
	}

drop:
	kfree_skb(skb);
}

static struct hci_cb sco_cb = {
	.name		= "SCO",
	.connect_cfm	= sco_connect_cfm,
	.disconn_cfm	= sco_disconn_cfm,
};

static int sco_debugfs_show(struct seq_file *f, void *p)
{
	struct sock *sk;

	read_lock(&sco_sk_list.lock);

	sk_for_each(sk, &sco_sk_list.head) {
		seq_printf(f, "%pMR %pMR %d\n", &sco_pi(sk)->src,
			   &sco_pi(sk)->dst, sk->sk_state);
	}

	read_unlock(&sco_sk_list.lock);

	return 0;
}

DEFINE_SHOW_ATTRIBUTE(sco_debugfs);

static struct dentry *sco_debugfs;

static const struct proto_ops sco_sock_ops = {
	.family		= PF_BLUETOOTH,
	.owner		= THIS_MODULE,
	.release	= sco_sock_release,
	.bind		= sco_sock_bind,
	.connect	= sco_sock_connect,
	.listen		= sco_sock_listen,
	.accept		= sco_sock_accept,
	.getname	= sco_sock_getname,
	.sendmsg	= sco_sock_sendmsg,
	.recvmsg	= sco_sock_recvmsg,
	.poll		= bt_sock_poll,
	.ioctl		= bt_sock_ioctl,
	.gettstamp	= sock_gettstamp,
	.mmap		= sock_no_mmap,
	.socketpair	= sock_no_socketpair,
	.shutdown	= sco_sock_shutdown,
	.setsockopt	= sco_sock_setsockopt,
	.getsockopt	= sco_sock_getsockopt
};

static const struct net_proto_family sco_sock_family_ops = {
	.family	= PF_BLUETOOTH,
	.owner	= THIS_MODULE,
	.create	= sco_sock_create,
};

int __init sco_init(void)
{
	int err;

	BUILD_BUG_ON(sizeof(struct sockaddr_sco) > sizeof(struct sockaddr));

	err = proto_register(&sco_proto, 0);
	if (err < 0)
		return err;

	err = bt_sock_register(BTPROTO_SCO, &sco_sock_family_ops);
	if (err < 0) {
		BT_ERR("SCO socket registration failed");
		goto error;
	}

	err = bt_procfs_init(&init_net, "sco", &sco_sk_list, NULL);
	if (err < 0) {
		BT_ERR("Failed to create SCO proc file");
		bt_sock_unregister(BTPROTO_SCO);
		goto error;
	}

	BT_INFO("SCO socket layer initialized");

	hci_register_cb(&sco_cb);

	if (IS_ERR_OR_NULL(bt_debugfs))
		return 0;

	sco_debugfs = debugfs_create_file("sco", 0444, bt_debugfs,
					  NULL, &sco_debugfs_fops);

	return 0;

error:
	proto_unregister(&sco_proto);
	return err;
}

void sco_exit(void)
{
	bt_procfs_cleanup(&init_net, "sco");

	debugfs_remove(sco_debugfs);

	hci_unregister_cb(&sco_cb);

	bt_sock_unregister(BTPROTO_SCO);

	proto_unregister(&sco_proto);
}

module_param(disable_esco, bool, 0644);
MODULE_PARM_DESC(disable_esco, "Disable eSCO connection creation");
