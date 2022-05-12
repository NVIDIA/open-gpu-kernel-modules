// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *
 * Copyright (C) 2004 Oracle.  All rights reserved.
 *
 * ----
 *
 * Callers for this were originally written against a very simple synchronus
 * API.  This implementation reflects those simple callers.  Some day I'm sure
 * we'll need to move to a more robust posting/callback mechanism.
 *
 * Transmit calls pass in kernel virtual addresses and block copying this into
 * the socket's tx buffers via a usual blocking sendmsg.  They'll block waiting
 * for a failed socket to timeout.  TX callers can also pass in a poniter to an
 * 'int' which gets filled with an errno off the wire in response to the
 * message they send.
 *
 * Handlers for unsolicited messages are registered.  Each socket has a page
 * that incoming data is copied into.  First the header, then the data.
 * Handlers are called from only one thread with a reference to this per-socket
 * page.  This page is destroyed after the handler call, so it can't be
 * referenced beyond the call.  Handlers may block but are discouraged from
 * doing so.
 *
 * Any framing errors (bad magic, large payload lengths) close a connection.
 *
 * Our sock_container holds the state we associate with a socket.  It's current
 * framing state is held there as well as the refcounting we do around when it
 * is safe to tear down the socket.  The socket is only finally torn down from
 * the container when the container loses all of its references -- so as long
 * as you hold a ref on the container you can trust that the socket is valid
 * for use with kernel socket APIs.
 *
 * Connections are initiated between a pair of nodes when the node with the
 * higher node number gets a heartbeat callback which indicates that the lower
 * numbered node has started heartbeating.  The lower numbered node is passive
 * and only accepts the connection if the higher numbered node is heartbeating.
 */

#include <linux/kernel.h>
#include <linux/sched/mm.h>
#include <linux/jiffies.h>
#include <linux/slab.h>
#include <linux/idr.h>
#include <linux/kref.h>
#include <linux/net.h>
#include <linux/export.h>
#include <net/tcp.h>

#include <linux/uaccess.h>

#include "heartbeat.h"
#include "tcp.h"
#include "nodemanager.h"
#define MLOG_MASK_PREFIX ML_TCP
#include "masklog.h"
#include "quorum.h"

#include "tcp_internal.h"

#define SC_NODEF_FMT "node %s (num %u) at %pI4:%u"
#define SC_NODEF_ARGS(sc) sc->sc_node->nd_name, sc->sc_node->nd_num,	\
			  &sc->sc_node->nd_ipv4_address,		\
			  ntohs(sc->sc_node->nd_ipv4_port)

/*
 * In the following two log macros, the whitespace after the ',' just
 * before ##args is intentional. Otherwise, gcc 2.95 will eat the
 * previous token if args expands to nothing.
 */
#define msglog(hdr, fmt, args...) do {					\
	typeof(hdr) __hdr = (hdr);					\
	mlog(ML_MSG, "[mag %u len %u typ %u stat %d sys_stat %d "	\
	     "key %08x num %u] " fmt,					\
	     be16_to_cpu(__hdr->magic), be16_to_cpu(__hdr->data_len), 	\
	     be16_to_cpu(__hdr->msg_type), be32_to_cpu(__hdr->status),	\
	     be32_to_cpu(__hdr->sys_status), be32_to_cpu(__hdr->key),	\
	     be32_to_cpu(__hdr->msg_num) ,  ##args);			\
} while (0)

#define sclog(sc, fmt, args...) do {					\
	typeof(sc) __sc = (sc);						\
	mlog(ML_SOCKET, "[sc %p refs %d sock %p node %u page %p "	\
	     "pg_off %zu] " fmt, __sc,					\
	     kref_read(&__sc->sc_kref), __sc->sc_sock,	\
	    __sc->sc_node->nd_num, __sc->sc_page, __sc->sc_page_off ,	\
	    ##args);							\
} while (0)

static DEFINE_RWLOCK(o2net_handler_lock);
static struct rb_root o2net_handler_tree = RB_ROOT;

static struct o2net_node o2net_nodes[O2NM_MAX_NODES];

/* XXX someday we'll need better accounting */
static struct socket *o2net_listen_sock;

/*
 * listen work is only queued by the listening socket callbacks on the
 * o2net_wq.  teardown detaches the callbacks before destroying the workqueue.
 * quorum work is queued as sock containers are shutdown.. stop_listening
 * tears down all the node's sock containers, preventing future shutdowns
 * and queued quroum work, before canceling delayed quorum work and
 * destroying the work queue.
 */
static struct workqueue_struct *o2net_wq;
static struct work_struct o2net_listen_work;

static struct o2hb_callback_func o2net_hb_up, o2net_hb_down;
#define O2NET_HB_PRI 0x1

static struct o2net_handshake *o2net_hand;
static struct o2net_msg *o2net_keep_req, *o2net_keep_resp;

static int o2net_sys_err_translations[O2NET_ERR_MAX] =
		{[O2NET_ERR_NONE]	= 0,
		 [O2NET_ERR_NO_HNDLR]	= -ENOPROTOOPT,
		 [O2NET_ERR_OVERFLOW]	= -EOVERFLOW,
		 [O2NET_ERR_DIED]	= -EHOSTDOWN,};

/* can't quite avoid *all* internal declarations :/ */
static void o2net_sc_connect_completed(struct work_struct *work);
static void o2net_rx_until_empty(struct work_struct *work);
static void o2net_shutdown_sc(struct work_struct *work);
static void o2net_listen_data_ready(struct sock *sk);
static void o2net_sc_send_keep_req(struct work_struct *work);
static void o2net_idle_timer(struct timer_list *t);
static void o2net_sc_postpone_idle(struct o2net_sock_container *sc);
static void o2net_sc_reset_idle_timer(struct o2net_sock_container *sc);

#ifdef CONFIG_DEBUG_FS
static void o2net_init_nst(struct o2net_send_tracking *nst, u32 msgtype,
			   u32 msgkey, struct task_struct *task, u8 node)
{
	INIT_LIST_HEAD(&nst->st_net_debug_item);
	nst->st_task = task;
	nst->st_msg_type = msgtype;
	nst->st_msg_key = msgkey;
	nst->st_node = node;
}

static inline void o2net_set_nst_sock_time(struct o2net_send_tracking *nst)
{
	nst->st_sock_time = ktime_get();
}

static inline void o2net_set_nst_send_time(struct o2net_send_tracking *nst)
{
	nst->st_send_time = ktime_get();
}

static inline void o2net_set_nst_status_time(struct o2net_send_tracking *nst)
{
	nst->st_status_time = ktime_get();
}

static inline void o2net_set_nst_sock_container(struct o2net_send_tracking *nst,
						struct o2net_sock_container *sc)
{
	nst->st_sc = sc;
}

static inline void o2net_set_nst_msg_id(struct o2net_send_tracking *nst,
					u32 msg_id)
{
	nst->st_id = msg_id;
}

static inline void o2net_set_sock_timer(struct o2net_sock_container *sc)
{
	sc->sc_tv_timer = ktime_get();
}

static inline void o2net_set_data_ready_time(struct o2net_sock_container *sc)
{
	sc->sc_tv_data_ready = ktime_get();
}

static inline void o2net_set_advance_start_time(struct o2net_sock_container *sc)
{
	sc->sc_tv_advance_start = ktime_get();
}

static inline void o2net_set_advance_stop_time(struct o2net_sock_container *sc)
{
	sc->sc_tv_advance_stop = ktime_get();
}

static inline void o2net_set_func_start_time(struct o2net_sock_container *sc)
{
	sc->sc_tv_func_start = ktime_get();
}

static inline void o2net_set_func_stop_time(struct o2net_sock_container *sc)
{
	sc->sc_tv_func_stop = ktime_get();
}

#else  /* CONFIG_DEBUG_FS */
# define o2net_init_nst(a, b, c, d, e)
# define o2net_set_nst_sock_time(a)
# define o2net_set_nst_send_time(a)
# define o2net_set_nst_status_time(a)
# define o2net_set_nst_sock_container(a, b)
# define o2net_set_nst_msg_id(a, b)
# define o2net_set_sock_timer(a)
# define o2net_set_data_ready_time(a)
# define o2net_set_advance_start_time(a)
# define o2net_set_advance_stop_time(a)
# define o2net_set_func_start_time(a)
# define o2net_set_func_stop_time(a)
#endif /* CONFIG_DEBUG_FS */

#ifdef CONFIG_OCFS2_FS_STATS
static ktime_t o2net_get_func_run_time(struct o2net_sock_container *sc)
{
	return ktime_sub(sc->sc_tv_func_stop, sc->sc_tv_func_start);
}

static void o2net_update_send_stats(struct o2net_send_tracking *nst,
				    struct o2net_sock_container *sc)
{
	sc->sc_tv_status_total = ktime_add(sc->sc_tv_status_total,
					   ktime_sub(ktime_get(),
						     nst->st_status_time));
	sc->sc_tv_send_total = ktime_add(sc->sc_tv_send_total,
					 ktime_sub(nst->st_status_time,
						   nst->st_send_time));
	sc->sc_tv_acquiry_total = ktime_add(sc->sc_tv_acquiry_total,
					    ktime_sub(nst->st_send_time,
						      nst->st_sock_time));
	sc->sc_send_count++;
}

static void o2net_update_recv_stats(struct o2net_sock_container *sc)
{
	sc->sc_tv_process_total = ktime_add(sc->sc_tv_process_total,
					    o2net_get_func_run_time(sc));
	sc->sc_recv_count++;
}

#else

# define o2net_update_send_stats(a, b)

# define o2net_update_recv_stats(sc)

#endif /* CONFIG_OCFS2_FS_STATS */

static inline unsigned int o2net_reconnect_delay(void)
{
	return o2nm_single_cluster->cl_reconnect_delay_ms;
}

static inline unsigned int o2net_keepalive_delay(void)
{
	return o2nm_single_cluster->cl_keepalive_delay_ms;
}

static inline unsigned int o2net_idle_timeout(void)
{
	return o2nm_single_cluster->cl_idle_timeout_ms;
}

static inline int o2net_sys_err_to_errno(enum o2net_system_error err)
{
	int trans;
	BUG_ON(err >= O2NET_ERR_MAX);
	trans = o2net_sys_err_translations[err];

	/* Just in case we mess up the translation table above */
	BUG_ON(err != O2NET_ERR_NONE && trans == 0);
	return trans;
}

static struct o2net_node * o2net_nn_from_num(u8 node_num)
{
	BUG_ON(node_num >= ARRAY_SIZE(o2net_nodes));
	return &o2net_nodes[node_num];
}

static u8 o2net_num_from_nn(struct o2net_node *nn)
{
	BUG_ON(nn == NULL);
	return nn - o2net_nodes;
}

/* ------------------------------------------------------------ */

static int o2net_prep_nsw(struct o2net_node *nn, struct o2net_status_wait *nsw)
{
	int ret;

	spin_lock(&nn->nn_lock);
	ret = idr_alloc(&nn->nn_status_idr, nsw, 0, 0, GFP_ATOMIC);
	if (ret >= 0) {
		nsw->ns_id = ret;
		list_add_tail(&nsw->ns_node_item, &nn->nn_status_list);
	}
	spin_unlock(&nn->nn_lock);
	if (ret < 0)
		return ret;

	init_waitqueue_head(&nsw->ns_wq);
	nsw->ns_sys_status = O2NET_ERR_NONE;
	nsw->ns_status = 0;
	return 0;
}

static void o2net_complete_nsw_locked(struct o2net_node *nn,
				      struct o2net_status_wait *nsw,
				      enum o2net_system_error sys_status,
				      s32 status)
{
	assert_spin_locked(&nn->nn_lock);

	if (!list_empty(&nsw->ns_node_item)) {
		list_del_init(&nsw->ns_node_item);
		nsw->ns_sys_status = sys_status;
		nsw->ns_status = status;
		idr_remove(&nn->nn_status_idr, nsw->ns_id);
		wake_up(&nsw->ns_wq);
	}
}

static void o2net_complete_nsw(struct o2net_node *nn,
			       struct o2net_status_wait *nsw,
			       u64 id, enum o2net_system_error sys_status,
			       s32 status)
{
	spin_lock(&nn->nn_lock);
	if (nsw == NULL) {
		if (id > INT_MAX)
			goto out;

		nsw = idr_find(&nn->nn_status_idr, id);
		if (nsw == NULL)
			goto out;
	}

	o2net_complete_nsw_locked(nn, nsw, sys_status, status);

out:
	spin_unlock(&nn->nn_lock);
	return;
}

static void o2net_complete_nodes_nsw(struct o2net_node *nn)
{
	struct o2net_status_wait *nsw, *tmp;
	unsigned int num_kills = 0;

	assert_spin_locked(&nn->nn_lock);

	list_for_each_entry_safe(nsw, tmp, &nn->nn_status_list, ns_node_item) {
		o2net_complete_nsw_locked(nn, nsw, O2NET_ERR_DIED, 0);
		num_kills++;
	}

	mlog(0, "completed %d messages for node %u\n", num_kills,
	     o2net_num_from_nn(nn));
}

static int o2net_nsw_completed(struct o2net_node *nn,
			       struct o2net_status_wait *nsw)
{
	int completed;
	spin_lock(&nn->nn_lock);
	completed = list_empty(&nsw->ns_node_item);
	spin_unlock(&nn->nn_lock);
	return completed;
}

/* ------------------------------------------------------------ */

static void sc_kref_release(struct kref *kref)
{
	struct o2net_sock_container *sc = container_of(kref,
					struct o2net_sock_container, sc_kref);
	BUG_ON(timer_pending(&sc->sc_idle_timeout));

	sclog(sc, "releasing\n");

	if (sc->sc_sock) {
		sock_release(sc->sc_sock);
		sc->sc_sock = NULL;
	}

	o2nm_undepend_item(&sc->sc_node->nd_item);
	o2nm_node_put(sc->sc_node);
	sc->sc_node = NULL;

	o2net_debug_del_sc(sc);

	if (sc->sc_page)
		__free_page(sc->sc_page);
	kfree(sc);
}

static void sc_put(struct o2net_sock_container *sc)
{
	sclog(sc, "put\n");
	kref_put(&sc->sc_kref, sc_kref_release);
}
static void sc_get(struct o2net_sock_container *sc)
{
	sclog(sc, "get\n");
	kref_get(&sc->sc_kref);
}
static struct o2net_sock_container *sc_alloc(struct o2nm_node *node)
{
	struct o2net_sock_container *sc, *ret = NULL;
	struct page *page = NULL;
	int status = 0;

	page = alloc_page(GFP_NOFS);
	sc = kzalloc(sizeof(*sc), GFP_NOFS);
	if (sc == NULL || page == NULL)
		goto out;

	kref_init(&sc->sc_kref);
	o2nm_node_get(node);
	sc->sc_node = node;

	/* pin the node item of the remote node */
	status = o2nm_depend_item(&node->nd_item);
	if (status) {
		mlog_errno(status);
		o2nm_node_put(node);
		goto out;
	}
	INIT_WORK(&sc->sc_connect_work, o2net_sc_connect_completed);
	INIT_WORK(&sc->sc_rx_work, o2net_rx_until_empty);
	INIT_WORK(&sc->sc_shutdown_work, o2net_shutdown_sc);
	INIT_DELAYED_WORK(&sc->sc_keepalive_work, o2net_sc_send_keep_req);

	timer_setup(&sc->sc_idle_timeout, o2net_idle_timer, 0);

	sclog(sc, "alloced\n");

	ret = sc;
	sc->sc_page = page;
	o2net_debug_add_sc(sc);
	sc = NULL;
	page = NULL;

out:
	if (page)
		__free_page(page);
	kfree(sc);

	return ret;
}

/* ------------------------------------------------------------ */

static void o2net_sc_queue_work(struct o2net_sock_container *sc,
				struct work_struct *work)
{
	sc_get(sc);
	if (!queue_work(o2net_wq, work))
		sc_put(sc);
}
static void o2net_sc_queue_delayed_work(struct o2net_sock_container *sc,
					struct delayed_work *work,
					int delay)
{
	sc_get(sc);
	if (!queue_delayed_work(o2net_wq, work, delay))
		sc_put(sc);
}
static void o2net_sc_cancel_delayed_work(struct o2net_sock_container *sc,
					 struct delayed_work *work)
{
	if (cancel_delayed_work(work))
		sc_put(sc);
}

static atomic_t o2net_connected_peers = ATOMIC_INIT(0);

int o2net_num_connected_peers(void)
{
	return atomic_read(&o2net_connected_peers);
}

static void o2net_set_nn_state(struct o2net_node *nn,
			       struct o2net_sock_container *sc,
			       unsigned valid, int err)
{
	int was_valid = nn->nn_sc_valid;
	int was_err = nn->nn_persistent_error;
	struct o2net_sock_container *old_sc = nn->nn_sc;

	assert_spin_locked(&nn->nn_lock);

	if (old_sc && !sc)
		atomic_dec(&o2net_connected_peers);
	else if (!old_sc && sc)
		atomic_inc(&o2net_connected_peers);

	/* the node num comparison and single connect/accept path should stop
	 * an non-null sc from being overwritten with another */
	BUG_ON(sc && nn->nn_sc && nn->nn_sc != sc);
	mlog_bug_on_msg(err && valid, "err %d valid %u\n", err, valid);
	mlog_bug_on_msg(valid && !sc, "valid %u sc %p\n", valid, sc);

	if (was_valid && !valid && err == 0)
		err = -ENOTCONN;

	mlog(ML_CONN, "node %u sc: %p -> %p, valid %u -> %u, err %d -> %d\n",
	     o2net_num_from_nn(nn), nn->nn_sc, sc, nn->nn_sc_valid, valid,
	     nn->nn_persistent_error, err);

	nn->nn_sc = sc;
	nn->nn_sc_valid = valid ? 1 : 0;
	nn->nn_persistent_error = err;

	/* mirrors o2net_tx_can_proceed() */
	if (nn->nn_persistent_error || nn->nn_sc_valid)
		wake_up(&nn->nn_sc_wq);

	if (was_valid && !was_err && nn->nn_persistent_error) {
		o2quo_conn_err(o2net_num_from_nn(nn));
		queue_delayed_work(o2net_wq, &nn->nn_still_up,
				   msecs_to_jiffies(O2NET_QUORUM_DELAY_MS));
	}

	if (was_valid && !valid) {
		if (old_sc)
			printk(KERN_NOTICE "o2net: No longer connected to "
				SC_NODEF_FMT "\n", SC_NODEF_ARGS(old_sc));
		o2net_complete_nodes_nsw(nn);
	}

	if (!was_valid && valid) {
		o2quo_conn_up(o2net_num_from_nn(nn));
		cancel_delayed_work(&nn->nn_connect_expired);
		printk(KERN_NOTICE "o2net: %s " SC_NODEF_FMT "\n",
		       o2nm_this_node() > sc->sc_node->nd_num ?
		       "Connected to" : "Accepted connection from",
		       SC_NODEF_ARGS(sc));
	}

	/* trigger the connecting worker func as long as we're not valid,
	 * it will back off if it shouldn't connect.  This can be called
	 * from node config teardown and so needs to be careful about
	 * the work queue actually being up. */
	if (!valid && o2net_wq) {
		unsigned long delay;
		/* delay if we're within a RECONNECT_DELAY of the
		 * last attempt */
		delay = (nn->nn_last_connect_attempt +
			 msecs_to_jiffies(o2net_reconnect_delay()))
			- jiffies;
		if (delay > msecs_to_jiffies(o2net_reconnect_delay()))
			delay = 0;
		mlog(ML_CONN, "queueing conn attempt in %lu jiffies\n", delay);
		queue_delayed_work(o2net_wq, &nn->nn_connect_work, delay);

		/*
		 * Delay the expired work after idle timeout.
		 *
		 * We might have lots of failed connection attempts that run
		 * through here but we only cancel the connect_expired work when
		 * a connection attempt succeeds.  So only the first enqueue of
		 * the connect_expired work will do anything.  The rest will see
		 * that it's already queued and do nothing.
		 */
		delay += msecs_to_jiffies(o2net_idle_timeout());
		queue_delayed_work(o2net_wq, &nn->nn_connect_expired, delay);
	}

	/* keep track of the nn's sc ref for the caller */
	if ((old_sc == NULL) && sc)
		sc_get(sc);
	if (old_sc && (old_sc != sc)) {
		o2net_sc_queue_work(old_sc, &old_sc->sc_shutdown_work);
		sc_put(old_sc);
	}
}

/* see o2net_register_callbacks() */
static void o2net_data_ready(struct sock *sk)
{
	void (*ready)(struct sock *sk);
	struct o2net_sock_container *sc;

	read_lock_bh(&sk->sk_callback_lock);
	sc = sk->sk_user_data;
	if (sc) {
		sclog(sc, "data_ready hit\n");
		o2net_set_data_ready_time(sc);
		o2net_sc_queue_work(sc, &sc->sc_rx_work);
		ready = sc->sc_data_ready;
	} else {
		ready = sk->sk_data_ready;
	}
	read_unlock_bh(&sk->sk_callback_lock);

	ready(sk);
}

/* see o2net_register_callbacks() */
static void o2net_state_change(struct sock *sk)
{
	void (*state_change)(struct sock *sk);
	struct o2net_sock_container *sc;

	read_lock_bh(&sk->sk_callback_lock);
	sc = sk->sk_user_data;
	if (sc == NULL) {
		state_change = sk->sk_state_change;
		goto out;
	}

	sclog(sc, "state_change to %d\n", sk->sk_state);

	state_change = sc->sc_state_change;

	switch(sk->sk_state) {
	/* ignore connecting sockets as they make progress */
	case TCP_SYN_SENT:
	case TCP_SYN_RECV:
		break;
	case TCP_ESTABLISHED:
		o2net_sc_queue_work(sc, &sc->sc_connect_work);
		break;
	default:
		printk(KERN_INFO "o2net: Connection to " SC_NODEF_FMT
			" shutdown, state %d\n",
			SC_NODEF_ARGS(sc), sk->sk_state);
		o2net_sc_queue_work(sc, &sc->sc_shutdown_work);
		break;
	}
out:
	read_unlock_bh(&sk->sk_callback_lock);
	state_change(sk);
}

/*
 * we register callbacks so we can queue work on events before calling
 * the original callbacks.  our callbacks our careful to test user_data
 * to discover when they've reaced with o2net_unregister_callbacks().
 */
static void o2net_register_callbacks(struct sock *sk,
				     struct o2net_sock_container *sc)
{
	write_lock_bh(&sk->sk_callback_lock);

	/* accepted sockets inherit the old listen socket data ready */
	if (sk->sk_data_ready == o2net_listen_data_ready) {
		sk->sk_data_ready = sk->sk_user_data;
		sk->sk_user_data = NULL;
	}

	BUG_ON(sk->sk_user_data != NULL);
	sk->sk_user_data = sc;
	sc_get(sc);

	sc->sc_data_ready = sk->sk_data_ready;
	sc->sc_state_change = sk->sk_state_change;
	sk->sk_data_ready = o2net_data_ready;
	sk->sk_state_change = o2net_state_change;

	mutex_init(&sc->sc_send_lock);

	write_unlock_bh(&sk->sk_callback_lock);
}

static int o2net_unregister_callbacks(struct sock *sk,
			           struct o2net_sock_container *sc)
{
	int ret = 0;

	write_lock_bh(&sk->sk_callback_lock);
	if (sk->sk_user_data == sc) {
		ret = 1;
		sk->sk_user_data = NULL;
		sk->sk_data_ready = sc->sc_data_ready;
		sk->sk_state_change = sc->sc_state_change;
	}
	write_unlock_bh(&sk->sk_callback_lock);

	return ret;
}

/*
 * this is a little helper that is called by callers who have seen a problem
 * with an sc and want to detach it from the nn if someone already hasn't beat
 * them to it.  if an error is given then the shutdown will be persistent
 * and pending transmits will be canceled.
 */
static void o2net_ensure_shutdown(struct o2net_node *nn,
			           struct o2net_sock_container *sc,
				   int err)
{
	spin_lock(&nn->nn_lock);
	if (nn->nn_sc == sc)
		o2net_set_nn_state(nn, NULL, 0, err);
	spin_unlock(&nn->nn_lock);
}

/*
 * This work queue function performs the blocking parts of socket shutdown.  A
 * few paths lead here.  set_nn_state will trigger this callback if it sees an
 * sc detached from the nn.  state_change will also trigger this callback
 * directly when it sees errors.  In that case we need to call set_nn_state
 * ourselves as state_change couldn't get the nn_lock and call set_nn_state
 * itself.
 */
static void o2net_shutdown_sc(struct work_struct *work)
{
	struct o2net_sock_container *sc =
		container_of(work, struct o2net_sock_container,
			     sc_shutdown_work);
	struct o2net_node *nn = o2net_nn_from_num(sc->sc_node->nd_num);

	sclog(sc, "shutting down\n");

	/* drop the callbacks ref and call shutdown only once */
	if (o2net_unregister_callbacks(sc->sc_sock->sk, sc)) {
		/* we shouldn't flush as we're in the thread, the
		 * races with pending sc work structs are harmless */
		del_timer_sync(&sc->sc_idle_timeout);
		o2net_sc_cancel_delayed_work(sc, &sc->sc_keepalive_work);
		sc_put(sc);
		kernel_sock_shutdown(sc->sc_sock, SHUT_RDWR);
	}

	/* not fatal so failed connects before the other guy has our
	 * heartbeat can be retried */
	o2net_ensure_shutdown(nn, sc, 0);
	sc_put(sc);
}

/* ------------------------------------------------------------ */

static int o2net_handler_cmp(struct o2net_msg_handler *nmh, u32 msg_type,
			     u32 key)
{
	int ret = memcmp(&nmh->nh_key, &key, sizeof(key));

	if (ret == 0)
		ret = memcmp(&nmh->nh_msg_type, &msg_type, sizeof(msg_type));

	return ret;
}

static struct o2net_msg_handler *
o2net_handler_tree_lookup(u32 msg_type, u32 key, struct rb_node ***ret_p,
			  struct rb_node **ret_parent)
{
	struct rb_node **p = &o2net_handler_tree.rb_node;
	struct rb_node *parent = NULL;
	struct o2net_msg_handler *nmh, *ret = NULL;
	int cmp;

	while (*p) {
		parent = *p;
		nmh = rb_entry(parent, struct o2net_msg_handler, nh_node);
		cmp = o2net_handler_cmp(nmh, msg_type, key);

		if (cmp < 0)
			p = &(*p)->rb_left;
		else if (cmp > 0)
			p = &(*p)->rb_right;
		else {
			ret = nmh;
			break;
		}
	}

	if (ret_p != NULL)
		*ret_p = p;
	if (ret_parent != NULL)
		*ret_parent = parent;

	return ret;
}

static void o2net_handler_kref_release(struct kref *kref)
{
	struct o2net_msg_handler *nmh;
	nmh = container_of(kref, struct o2net_msg_handler, nh_kref);

	kfree(nmh);
}

static void o2net_handler_put(struct o2net_msg_handler *nmh)
{
	kref_put(&nmh->nh_kref, o2net_handler_kref_release);
}

/* max_len is protection for the handler func.  incoming messages won't
 * be given to the handler if their payload is longer than the max. */
int o2net_register_handler(u32 msg_type, u32 key, u32 max_len,
			   o2net_msg_handler_func *func, void *data,
			   o2net_post_msg_handler_func *post_func,
			   struct list_head *unreg_list)
{
	struct o2net_msg_handler *nmh = NULL;
	struct rb_node **p, *parent;
	int ret = 0;

	if (max_len > O2NET_MAX_PAYLOAD_BYTES) {
		mlog(0, "max_len for message handler out of range: %u\n",
			max_len);
		ret = -EINVAL;
		goto out;
	}

	if (!msg_type) {
		mlog(0, "no message type provided: %u, %p\n", msg_type, func);
		ret = -EINVAL;
		goto out;

	}
	if (!func) {
		mlog(0, "no message handler provided: %u, %p\n",
		       msg_type, func);
		ret = -EINVAL;
		goto out;
	}

       	nmh = kzalloc(sizeof(struct o2net_msg_handler), GFP_NOFS);
	if (nmh == NULL) {
		ret = -ENOMEM;
		goto out;
	}

	nmh->nh_func = func;
	nmh->nh_func_data = data;
	nmh->nh_post_func = post_func;
	nmh->nh_msg_type = msg_type;
	nmh->nh_max_len = max_len;
	nmh->nh_key = key;
	/* the tree and list get this ref.. they're both removed in
	 * unregister when this ref is dropped */
	kref_init(&nmh->nh_kref);
	INIT_LIST_HEAD(&nmh->nh_unregister_item);

	write_lock(&o2net_handler_lock);
	if (o2net_handler_tree_lookup(msg_type, key, &p, &parent))
		ret = -EEXIST;
	else {
	        rb_link_node(&nmh->nh_node, parent, p);
		rb_insert_color(&nmh->nh_node, &o2net_handler_tree);
		list_add_tail(&nmh->nh_unregister_item, unreg_list);

		mlog(ML_TCP, "registered handler func %p type %u key %08x\n",
		     func, msg_type, key);
		/* we've had some trouble with handlers seemingly vanishing. */
		mlog_bug_on_msg(o2net_handler_tree_lookup(msg_type, key, &p,
							  &parent) == NULL,
			        "couldn't find handler we *just* registered "
				"for type %u key %08x\n", msg_type, key);
	}
	write_unlock(&o2net_handler_lock);

out:
	if (ret)
		kfree(nmh);

	return ret;
}
EXPORT_SYMBOL_GPL(o2net_register_handler);

void o2net_unregister_handler_list(struct list_head *list)
{
	struct o2net_msg_handler *nmh, *n;

	write_lock(&o2net_handler_lock);
	list_for_each_entry_safe(nmh, n, list, nh_unregister_item) {
		mlog(ML_TCP, "unregistering handler func %p type %u key %08x\n",
		     nmh->nh_func, nmh->nh_msg_type, nmh->nh_key);
		rb_erase(&nmh->nh_node, &o2net_handler_tree);
		list_del_init(&nmh->nh_unregister_item);
		kref_put(&nmh->nh_kref, o2net_handler_kref_release);
	}
	write_unlock(&o2net_handler_lock);
}
EXPORT_SYMBOL_GPL(o2net_unregister_handler_list);

static struct o2net_msg_handler *o2net_handler_get(u32 msg_type, u32 key)
{
	struct o2net_msg_handler *nmh;

	read_lock(&o2net_handler_lock);
	nmh = o2net_handler_tree_lookup(msg_type, key, NULL, NULL);
	if (nmh)
		kref_get(&nmh->nh_kref);
	read_unlock(&o2net_handler_lock);

	return nmh;
}

/* ------------------------------------------------------------ */

static int o2net_recv_tcp_msg(struct socket *sock, void *data, size_t len)
{
	struct kvec vec = { .iov_len = len, .iov_base = data, };
	struct msghdr msg = { .msg_flags = MSG_DONTWAIT, };
	iov_iter_kvec(&msg.msg_iter, READ, &vec, 1, len);
	return sock_recvmsg(sock, &msg, MSG_DONTWAIT);
}

static int o2net_send_tcp_msg(struct socket *sock, struct kvec *vec,
			      size_t veclen, size_t total)
{
	int ret;
	struct msghdr msg = {.msg_flags = 0,};

	if (sock == NULL) {
		ret = -EINVAL;
		goto out;
	}

	ret = kernel_sendmsg(sock, &msg, vec, veclen, total);
	if (likely(ret == total))
		return 0;
	mlog(ML_ERROR, "sendmsg returned %d instead of %zu\n", ret, total);
	if (ret >= 0)
		ret = -EPIPE; /* should be smarter, I bet */
out:
	mlog(0, "returning error: %d\n", ret);
	return ret;
}

static void o2net_sendpage(struct o2net_sock_container *sc,
			   void *kmalloced_virt,
			   size_t size)
{
	struct o2net_node *nn = o2net_nn_from_num(sc->sc_node->nd_num);
	ssize_t ret;

	while (1) {
		mutex_lock(&sc->sc_send_lock);
		ret = sc->sc_sock->ops->sendpage(sc->sc_sock,
						 virt_to_page(kmalloced_virt),
						 offset_in_page(kmalloced_virt),
						 size, MSG_DONTWAIT);
		mutex_unlock(&sc->sc_send_lock);
		if (ret == size)
			break;
		if (ret == (ssize_t)-EAGAIN) {
			mlog(0, "sendpage of size %zu to " SC_NODEF_FMT
			     " returned EAGAIN\n", size, SC_NODEF_ARGS(sc));
			cond_resched();
			continue;
		}
		mlog(ML_ERROR, "sendpage of size %zu to " SC_NODEF_FMT
		     " failed with %zd\n", size, SC_NODEF_ARGS(sc), ret);
		o2net_ensure_shutdown(nn, sc, 0);
		break;
	}
}

static void o2net_init_msg(struct o2net_msg *msg, u16 data_len, u16 msg_type, u32 key)
{
	memset(msg, 0, sizeof(struct o2net_msg));
	msg->magic = cpu_to_be16(O2NET_MSG_MAGIC);
	msg->data_len = cpu_to_be16(data_len);
	msg->msg_type = cpu_to_be16(msg_type);
	msg->sys_status = cpu_to_be32(O2NET_ERR_NONE);
	msg->status = 0;
	msg->key = cpu_to_be32(key);
}

static int o2net_tx_can_proceed(struct o2net_node *nn,
			        struct o2net_sock_container **sc_ret,
				int *error)
{
	int ret = 0;

	spin_lock(&nn->nn_lock);
	if (nn->nn_persistent_error) {
		ret = 1;
		*sc_ret = NULL;
		*error = nn->nn_persistent_error;
	} else if (nn->nn_sc_valid) {
		kref_get(&nn->nn_sc->sc_kref);

		ret = 1;
		*sc_ret = nn->nn_sc;
		*error = 0;
	}
	spin_unlock(&nn->nn_lock);

	return ret;
}

/* Get a map of all nodes to which this node is currently connected to */
void o2net_fill_node_map(unsigned long *map, unsigned bytes)
{
	struct o2net_sock_container *sc;
	int node, ret;

	BUG_ON(bytes < (BITS_TO_LONGS(O2NM_MAX_NODES) * sizeof(unsigned long)));

	memset(map, 0, bytes);
	for (node = 0; node < O2NM_MAX_NODES; ++node) {
		if (!o2net_tx_can_proceed(o2net_nn_from_num(node), &sc, &ret))
			continue;
		if (!ret) {
			set_bit(node, map);
			sc_put(sc);
		}
	}
}
EXPORT_SYMBOL_GPL(o2net_fill_node_map);

int o2net_send_message_vec(u32 msg_type, u32 key, struct kvec *caller_vec,
			   size_t caller_veclen, u8 target_node, int *status)
{
	int ret = 0;
	struct o2net_msg *msg = NULL;
	size_t veclen, caller_bytes = 0;
	struct kvec *vec = NULL;
	struct o2net_sock_container *sc = NULL;
	struct o2net_node *nn = o2net_nn_from_num(target_node);
	struct o2net_status_wait nsw = {
		.ns_node_item = LIST_HEAD_INIT(nsw.ns_node_item),
	};
	struct o2net_send_tracking nst;

	o2net_init_nst(&nst, msg_type, key, current, target_node);

	if (o2net_wq == NULL) {
		mlog(0, "attempt to tx without o2netd running\n");
		ret = -ESRCH;
		goto out;
	}

	if (caller_veclen == 0) {
		mlog(0, "bad kvec array length\n");
		ret = -EINVAL;
		goto out;
	}

	caller_bytes = iov_length((struct iovec *)caller_vec, caller_veclen);
	if (caller_bytes > O2NET_MAX_PAYLOAD_BYTES) {
		mlog(0, "total payload len %zu too large\n", caller_bytes);
		ret = -EINVAL;
		goto out;
	}

	if (target_node == o2nm_this_node()) {
		ret = -ELOOP;
		goto out;
	}

	o2net_debug_add_nst(&nst);

	o2net_set_nst_sock_time(&nst);

	wait_event(nn->nn_sc_wq, o2net_tx_can_proceed(nn, &sc, &ret));
	if (ret)
		goto out;

	o2net_set_nst_sock_container(&nst, sc);

	veclen = caller_veclen + 1;
	vec = kmalloc_array(veclen, sizeof(struct kvec), GFP_ATOMIC);
	if (vec == NULL) {
		mlog(0, "failed to %zu element kvec!\n", veclen);
		ret = -ENOMEM;
		goto out;
	}

	msg = kmalloc(sizeof(struct o2net_msg), GFP_ATOMIC);
	if (!msg) {
		mlog(0, "failed to allocate a o2net_msg!\n");
		ret = -ENOMEM;
		goto out;
	}

	o2net_init_msg(msg, caller_bytes, msg_type, key);

	vec[0].iov_len = sizeof(struct o2net_msg);
	vec[0].iov_base = msg;
	memcpy(&vec[1], caller_vec, caller_veclen * sizeof(struct kvec));

	ret = o2net_prep_nsw(nn, &nsw);
	if (ret)
		goto out;

	msg->msg_num = cpu_to_be32(nsw.ns_id);
	o2net_set_nst_msg_id(&nst, nsw.ns_id);

	o2net_set_nst_send_time(&nst);

	/* finally, convert the message header to network byte-order
	 * and send */
	mutex_lock(&sc->sc_send_lock);
	ret = o2net_send_tcp_msg(sc->sc_sock, vec, veclen,
				 sizeof(struct o2net_msg) + caller_bytes);
	mutex_unlock(&sc->sc_send_lock);
	msglog(msg, "sending returned %d\n", ret);
	if (ret < 0) {
		mlog(0, "error returned from o2net_send_tcp_msg=%d\n", ret);
		goto out;
	}

	/* wait on other node's handler */
	o2net_set_nst_status_time(&nst);
	wait_event(nsw.ns_wq, o2net_nsw_completed(nn, &nsw));

	o2net_update_send_stats(&nst, sc);

	/* Note that we avoid overwriting the callers status return
	 * variable if a system error was reported on the other
	 * side. Callers beware. */
	ret = o2net_sys_err_to_errno(nsw.ns_sys_status);
	if (status && !ret)
		*status = nsw.ns_status;

	mlog(0, "woken, returning system status %d, user status %d\n",
	     ret, nsw.ns_status);
out:
	o2net_debug_del_nst(&nst); /* must be before dropping sc and node */
	if (sc)
		sc_put(sc);
	kfree(vec);
	kfree(msg);
	o2net_complete_nsw(nn, &nsw, 0, 0, 0);
	return ret;
}
EXPORT_SYMBOL_GPL(o2net_send_message_vec);

int o2net_send_message(u32 msg_type, u32 key, void *data, u32 len,
		       u8 target_node, int *status)
{
	struct kvec vec = {
		.iov_base = data,
		.iov_len = len,
	};
	return o2net_send_message_vec(msg_type, key, &vec, 1,
				      target_node, status);
}
EXPORT_SYMBOL_GPL(o2net_send_message);

static int o2net_send_status_magic(struct socket *sock, struct o2net_msg *hdr,
				   enum o2net_system_error syserr, int err)
{
	struct kvec vec = {
		.iov_base = hdr,
		.iov_len = sizeof(struct o2net_msg),
	};

	BUG_ON(syserr >= O2NET_ERR_MAX);

	/* leave other fields intact from the incoming message, msg_num
	 * in particular */
	hdr->sys_status = cpu_to_be32(syserr);
	hdr->status = cpu_to_be32(err);
	hdr->magic = cpu_to_be16(O2NET_MSG_STATUS_MAGIC);  // twiddle the magic
	hdr->data_len = 0;

	msglog(hdr, "about to send status magic %d\n", err);
	/* hdr has been in host byteorder this whole time */
	return o2net_send_tcp_msg(sock, &vec, 1, sizeof(struct o2net_msg));
}

/* this returns -errno if the header was unknown or too large, etc.
 * after this is called the buffer us reused for the next message */
static int o2net_process_message(struct o2net_sock_container *sc,
				 struct o2net_msg *hdr)
{
	struct o2net_node *nn = o2net_nn_from_num(sc->sc_node->nd_num);
	int ret = 0, handler_status;
	enum  o2net_system_error syserr;
	struct o2net_msg_handler *nmh = NULL;
	void *ret_data = NULL;

	msglog(hdr, "processing message\n");

	o2net_sc_postpone_idle(sc);

	switch(be16_to_cpu(hdr->magic)) {
		case O2NET_MSG_STATUS_MAGIC:
			/* special type for returning message status */
			o2net_complete_nsw(nn, NULL,
					   be32_to_cpu(hdr->msg_num),
					   be32_to_cpu(hdr->sys_status),
					   be32_to_cpu(hdr->status));
			goto out;
		case O2NET_MSG_KEEP_REQ_MAGIC:
			o2net_sendpage(sc, o2net_keep_resp,
				       sizeof(*o2net_keep_resp));
			goto out;
		case O2NET_MSG_KEEP_RESP_MAGIC:
			goto out;
		case O2NET_MSG_MAGIC:
			break;
		default:
			msglog(hdr, "bad magic\n");
			ret = -EINVAL;
			goto out;
	}

	/* find a handler for it */
	handler_status = 0;
	nmh = o2net_handler_get(be16_to_cpu(hdr->msg_type),
				be32_to_cpu(hdr->key));
	if (!nmh) {
		mlog(ML_TCP, "couldn't find handler for type %u key %08x\n",
		     be16_to_cpu(hdr->msg_type), be32_to_cpu(hdr->key));
		syserr = O2NET_ERR_NO_HNDLR;
		goto out_respond;
	}

	syserr = O2NET_ERR_NONE;

	if (be16_to_cpu(hdr->data_len) > nmh->nh_max_len)
		syserr = O2NET_ERR_OVERFLOW;

	if (syserr != O2NET_ERR_NONE)
		goto out_respond;

	o2net_set_func_start_time(sc);
	sc->sc_msg_key = be32_to_cpu(hdr->key);
	sc->sc_msg_type = be16_to_cpu(hdr->msg_type);
	handler_status = (nmh->nh_func)(hdr, sizeof(struct o2net_msg) +
					     be16_to_cpu(hdr->data_len),
					nmh->nh_func_data, &ret_data);
	o2net_set_func_stop_time(sc);

	o2net_update_recv_stats(sc);

out_respond:
	/* this destroys the hdr, so don't use it after this */
	mutex_lock(&sc->sc_send_lock);
	ret = o2net_send_status_magic(sc->sc_sock, hdr, syserr,
				      handler_status);
	mutex_unlock(&sc->sc_send_lock);
	hdr = NULL;
	mlog(0, "sending handler status %d, syserr %d returned %d\n",
	     handler_status, syserr, ret);

	if (nmh) {
		BUG_ON(ret_data != NULL && nmh->nh_post_func == NULL);
		if (nmh->nh_post_func)
			(nmh->nh_post_func)(handler_status, nmh->nh_func_data,
					    ret_data);
	}

out:
	if (nmh)
		o2net_handler_put(nmh);
	return ret;
}

static int o2net_check_handshake(struct o2net_sock_container *sc)
{
	struct o2net_handshake *hand = page_address(sc->sc_page);
	struct o2net_node *nn = o2net_nn_from_num(sc->sc_node->nd_num);

	if (hand->protocol_version != cpu_to_be64(O2NET_PROTOCOL_VERSION)) {
		printk(KERN_NOTICE "o2net: " SC_NODEF_FMT " Advertised net "
		       "protocol version %llu but %llu is required. "
		       "Disconnecting.\n", SC_NODEF_ARGS(sc),
		       (unsigned long long)be64_to_cpu(hand->protocol_version),
		       O2NET_PROTOCOL_VERSION);

		/* don't bother reconnecting if its the wrong version. */
		o2net_ensure_shutdown(nn, sc, -ENOTCONN);
		return -1;
	}

	/*
	 * Ensure timeouts are consistent with other nodes, otherwise
	 * we can end up with one node thinking that the other must be down,
	 * but isn't. This can ultimately cause corruption.
	 */
	if (be32_to_cpu(hand->o2net_idle_timeout_ms) !=
				o2net_idle_timeout()) {
		printk(KERN_NOTICE "o2net: " SC_NODEF_FMT " uses a network "
		       "idle timeout of %u ms, but we use %u ms locally. "
		       "Disconnecting.\n", SC_NODEF_ARGS(sc),
		       be32_to_cpu(hand->o2net_idle_timeout_ms),
		       o2net_idle_timeout());
		o2net_ensure_shutdown(nn, sc, -ENOTCONN);
		return -1;
	}

	if (be32_to_cpu(hand->o2net_keepalive_delay_ms) !=
			o2net_keepalive_delay()) {
		printk(KERN_NOTICE "o2net: " SC_NODEF_FMT " uses a keepalive "
		       "delay of %u ms, but we use %u ms locally. "
		       "Disconnecting.\n", SC_NODEF_ARGS(sc),
		       be32_to_cpu(hand->o2net_keepalive_delay_ms),
		       o2net_keepalive_delay());
		o2net_ensure_shutdown(nn, sc, -ENOTCONN);
		return -1;
	}

	if (be32_to_cpu(hand->o2hb_heartbeat_timeout_ms) !=
			O2HB_MAX_WRITE_TIMEOUT_MS) {
		printk(KERN_NOTICE "o2net: " SC_NODEF_FMT " uses a heartbeat "
		       "timeout of %u ms, but we use %u ms locally. "
		       "Disconnecting.\n", SC_NODEF_ARGS(sc),
		       be32_to_cpu(hand->o2hb_heartbeat_timeout_ms),
		       O2HB_MAX_WRITE_TIMEOUT_MS);
		o2net_ensure_shutdown(nn, sc, -ENOTCONN);
		return -1;
	}

	sc->sc_handshake_ok = 1;

	spin_lock(&nn->nn_lock);
	/* set valid and queue the idle timers only if it hasn't been
	 * shut down already */
	if (nn->nn_sc == sc) {
		o2net_sc_reset_idle_timer(sc);
		atomic_set(&nn->nn_timeout, 0);
		o2net_set_nn_state(nn, sc, 1, 0);
	}
	spin_unlock(&nn->nn_lock);

	/* shift everything up as though it wasn't there */
	sc->sc_page_off -= sizeof(struct o2net_handshake);
	if (sc->sc_page_off)
		memmove(hand, hand + 1, sc->sc_page_off);

	return 0;
}

/* this demuxes the queued rx bytes into header or payload bits and calls
 * handlers as each full message is read off the socket.  it returns -error,
 * == 0 eof, or > 0 for progress made.*/
static int o2net_advance_rx(struct o2net_sock_container *sc)
{
	struct o2net_msg *hdr;
	int ret = 0;
	void *data;
	size_t datalen;

	sclog(sc, "receiving\n");
	o2net_set_advance_start_time(sc);

	if (unlikely(sc->sc_handshake_ok == 0)) {
		if(sc->sc_page_off < sizeof(struct o2net_handshake)) {
			data = page_address(sc->sc_page) + sc->sc_page_off;
			datalen = sizeof(struct o2net_handshake) - sc->sc_page_off;
			ret = o2net_recv_tcp_msg(sc->sc_sock, data, datalen);
			if (ret > 0)
				sc->sc_page_off += ret;
		}

		if (sc->sc_page_off == sizeof(struct o2net_handshake)) {
			o2net_check_handshake(sc);
			if (unlikely(sc->sc_handshake_ok == 0))
				ret = -EPROTO;
		}
		goto out;
	}

	/* do we need more header? */
	if (sc->sc_page_off < sizeof(struct o2net_msg)) {
		data = page_address(sc->sc_page) + sc->sc_page_off;
		datalen = sizeof(struct o2net_msg) - sc->sc_page_off;
		ret = o2net_recv_tcp_msg(sc->sc_sock, data, datalen);
		if (ret > 0) {
			sc->sc_page_off += ret;
			/* only swab incoming here.. we can
			 * only get here once as we cross from
			 * being under to over */
			if (sc->sc_page_off == sizeof(struct o2net_msg)) {
				hdr = page_address(sc->sc_page);
				if (be16_to_cpu(hdr->data_len) >
				    O2NET_MAX_PAYLOAD_BYTES)
					ret = -EOVERFLOW;
			}
		}
		if (ret <= 0)
			goto out;
	}

	if (sc->sc_page_off < sizeof(struct o2net_msg)) {
		/* oof, still don't have a header */
		goto out;
	}

	/* this was swabbed above when we first read it */
	hdr = page_address(sc->sc_page);

	msglog(hdr, "at page_off %zu\n", sc->sc_page_off);

	/* do we need more payload? */
	if (sc->sc_page_off - sizeof(struct o2net_msg) < be16_to_cpu(hdr->data_len)) {
		/* need more payload */
		data = page_address(sc->sc_page) + sc->sc_page_off;
		datalen = (sizeof(struct o2net_msg) + be16_to_cpu(hdr->data_len)) -
			  sc->sc_page_off;
		ret = o2net_recv_tcp_msg(sc->sc_sock, data, datalen);
		if (ret > 0)
			sc->sc_page_off += ret;
		if (ret <= 0)
			goto out;
	}

	if (sc->sc_page_off - sizeof(struct o2net_msg) == be16_to_cpu(hdr->data_len)) {
		/* we can only get here once, the first time we read
		 * the payload.. so set ret to progress if the handler
		 * works out. after calling this the message is toast */
		ret = o2net_process_message(sc, hdr);
		if (ret == 0)
			ret = 1;
		sc->sc_page_off = 0;
	}

out:
	sclog(sc, "ret = %d\n", ret);
	o2net_set_advance_stop_time(sc);
	return ret;
}

/* this work func is triggerd by data ready.  it reads until it can read no
 * more.  it interprets 0, eof, as fatal.  if data_ready hits while we're doing
 * our work the work struct will be marked and we'll be called again. */
static void o2net_rx_until_empty(struct work_struct *work)
{
	struct o2net_sock_container *sc =
		container_of(work, struct o2net_sock_container, sc_rx_work);
	int ret;

	do {
		ret = o2net_advance_rx(sc);
	} while (ret > 0);

	if (ret <= 0 && ret != -EAGAIN) {
		struct o2net_node *nn = o2net_nn_from_num(sc->sc_node->nd_num);
		sclog(sc, "saw error %d, closing\n", ret);
		/* not permanent so read failed handshake can retry */
		o2net_ensure_shutdown(nn, sc, 0);
	}

	sc_put(sc);
}

static void o2net_initialize_handshake(void)
{
	o2net_hand->o2hb_heartbeat_timeout_ms = cpu_to_be32(
		O2HB_MAX_WRITE_TIMEOUT_MS);
	o2net_hand->o2net_idle_timeout_ms = cpu_to_be32(o2net_idle_timeout());
	o2net_hand->o2net_keepalive_delay_ms = cpu_to_be32(
		o2net_keepalive_delay());
	o2net_hand->o2net_reconnect_delay_ms = cpu_to_be32(
		o2net_reconnect_delay());
}

/* ------------------------------------------------------------ */

/* called when a connect completes and after a sock is accepted.  the
 * rx path will see the response and mark the sc valid */
static void o2net_sc_connect_completed(struct work_struct *work)
{
	struct o2net_sock_container *sc =
		container_of(work, struct o2net_sock_container,
			     sc_connect_work);

	mlog(ML_MSG, "sc sending handshake with ver %llu id %llx\n",
              (unsigned long long)O2NET_PROTOCOL_VERSION,
	      (unsigned long long)be64_to_cpu(o2net_hand->connector_id));

	o2net_initialize_handshake();
	o2net_sendpage(sc, o2net_hand, sizeof(*o2net_hand));
	sc_put(sc);
}

/* this is called as a work_struct func. */
static void o2net_sc_send_keep_req(struct work_struct *work)
{
	struct o2net_sock_container *sc =
		container_of(work, struct o2net_sock_container,
			     sc_keepalive_work.work);

	o2net_sendpage(sc, o2net_keep_req, sizeof(*o2net_keep_req));
	sc_put(sc);
}

/* socket shutdown does a del_timer_sync against this as it tears down.
 * we can't start this timer until we've got to the point in sc buildup
 * where shutdown is going to be involved */
static void o2net_idle_timer(struct timer_list *t)
{
	struct o2net_sock_container *sc = from_timer(sc, t, sc_idle_timeout);
	struct o2net_node *nn = o2net_nn_from_num(sc->sc_node->nd_num);
#ifdef CONFIG_DEBUG_FS
	unsigned long msecs = ktime_to_ms(ktime_get()) -
		ktime_to_ms(sc->sc_tv_timer);
#else
	unsigned long msecs = o2net_idle_timeout();
#endif

	printk(KERN_NOTICE "o2net: Connection to " SC_NODEF_FMT " has been "
	       "idle for %lu.%lu secs.\n",
	       SC_NODEF_ARGS(sc), msecs / 1000, msecs % 1000);

	/* idle timerout happen, don't shutdown the connection, but
	 * make fence decision. Maybe the connection can recover before
	 * the decision is made.
	 */
	atomic_set(&nn->nn_timeout, 1);
	o2quo_conn_err(o2net_num_from_nn(nn));
	queue_delayed_work(o2net_wq, &nn->nn_still_up,
			msecs_to_jiffies(O2NET_QUORUM_DELAY_MS));

	o2net_sc_reset_idle_timer(sc);

}

static void o2net_sc_reset_idle_timer(struct o2net_sock_container *sc)
{
	o2net_sc_cancel_delayed_work(sc, &sc->sc_keepalive_work);
	o2net_sc_queue_delayed_work(sc, &sc->sc_keepalive_work,
		      msecs_to_jiffies(o2net_keepalive_delay()));
	o2net_set_sock_timer(sc);
	mod_timer(&sc->sc_idle_timeout,
	       jiffies + msecs_to_jiffies(o2net_idle_timeout()));
}

static void o2net_sc_postpone_idle(struct o2net_sock_container *sc)
{
	struct o2net_node *nn = o2net_nn_from_num(sc->sc_node->nd_num);

	/* clear fence decision since the connection recover from timeout*/
	if (atomic_read(&nn->nn_timeout)) {
		o2quo_conn_up(o2net_num_from_nn(nn));
		cancel_delayed_work(&nn->nn_still_up);
		atomic_set(&nn->nn_timeout, 0);
	}

	/* Only push out an existing timer */
	if (timer_pending(&sc->sc_idle_timeout))
		o2net_sc_reset_idle_timer(sc);
}

/* this work func is kicked whenever a path sets the nn state which doesn't
 * have valid set.  This includes seeing hb come up, losing a connection,
 * having a connect attempt fail, etc. This centralizes the logic which decides
 * if a connect attempt should be made or if we should give up and all future
 * transmit attempts should fail */
static void o2net_start_connect(struct work_struct *work)
{
	struct o2net_node *nn =
		container_of(work, struct o2net_node, nn_connect_work.work);
	struct o2net_sock_container *sc = NULL;
	struct o2nm_node *node = NULL, *mynode = NULL;
	struct socket *sock = NULL;
	struct sockaddr_in myaddr = {0, }, remoteaddr = {0, };
	int ret = 0, stop;
	unsigned int timeout;
	unsigned int nofs_flag;

	/*
	 * sock_create allocates the sock with GFP_KERNEL. We must
	 * prevent the filesystem from being reentered by memory reclaim.
	 */
	nofs_flag = memalloc_nofs_save();
	/* if we're greater we initiate tx, otherwise we accept */
	if (o2nm_this_node() <= o2net_num_from_nn(nn))
		goto out;

	/* watch for racing with tearing a node down */
	node = o2nm_get_node_by_num(o2net_num_from_nn(nn));
	if (node == NULL)
		goto out;

	mynode = o2nm_get_node_by_num(o2nm_this_node());
	if (mynode == NULL)
		goto out;

	spin_lock(&nn->nn_lock);
	/*
	 * see if we already have one pending or have given up.
	 * For nn_timeout, it is set when we close the connection
	 * because of the idle time out. So it means that we have
	 * at least connected to that node successfully once,
	 * now try to connect to it again.
	 */
	timeout = atomic_read(&nn->nn_timeout);
	stop = (nn->nn_sc ||
		(nn->nn_persistent_error &&
		(nn->nn_persistent_error != -ENOTCONN || timeout == 0)));
	spin_unlock(&nn->nn_lock);
	if (stop)
		goto out;

	nn->nn_last_connect_attempt = jiffies;

	sc = sc_alloc(node);
	if (sc == NULL) {
		mlog(0, "couldn't allocate sc\n");
		ret = -ENOMEM;
		goto out;
	}

	ret = sock_create(PF_INET, SOCK_STREAM, IPPROTO_TCP, &sock);
	if (ret < 0) {
		mlog(0, "can't create socket: %d\n", ret);
		goto out;
	}
	sc->sc_sock = sock; /* freed by sc_kref_release */

	sock->sk->sk_allocation = GFP_ATOMIC;

	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = mynode->nd_ipv4_address;
	myaddr.sin_port = htons(0); /* any port */

	ret = sock->ops->bind(sock, (struct sockaddr *)&myaddr,
			      sizeof(myaddr));
	if (ret) {
		mlog(ML_ERROR, "bind failed with %d at address %pI4\n",
		     ret, &mynode->nd_ipv4_address);
		goto out;
	}

	tcp_sock_set_nodelay(sc->sc_sock->sk);
	tcp_sock_set_user_timeout(sock->sk, O2NET_TCP_USER_TIMEOUT);

	o2net_register_callbacks(sc->sc_sock->sk, sc);

	spin_lock(&nn->nn_lock);
	/* handshake completion will set nn->nn_sc_valid */
	o2net_set_nn_state(nn, sc, 0, 0);
	spin_unlock(&nn->nn_lock);

	remoteaddr.sin_family = AF_INET;
	remoteaddr.sin_addr.s_addr = node->nd_ipv4_address;
	remoteaddr.sin_port = node->nd_ipv4_port;

	ret = sc->sc_sock->ops->connect(sc->sc_sock,
					(struct sockaddr *)&remoteaddr,
					sizeof(remoteaddr),
					O_NONBLOCK);
	if (ret == -EINPROGRESS)
		ret = 0;

out:
	if (ret && sc) {
		printk(KERN_NOTICE "o2net: Connect attempt to " SC_NODEF_FMT
		       " failed with errno %d\n", SC_NODEF_ARGS(sc), ret);
		/* 0 err so that another will be queued and attempted
		 * from set_nn_state */
		o2net_ensure_shutdown(nn, sc, 0);
	}
	if (sc)
		sc_put(sc);
	if (node)
		o2nm_node_put(node);
	if (mynode)
		o2nm_node_put(mynode);

	memalloc_nofs_restore(nofs_flag);
	return;
}

static void o2net_connect_expired(struct work_struct *work)
{
	struct o2net_node *nn =
		container_of(work, struct o2net_node, nn_connect_expired.work);

	spin_lock(&nn->nn_lock);
	if (!nn->nn_sc_valid) {
		printk(KERN_NOTICE "o2net: No connection established with "
		       "node %u after %u.%u seconds, check network and"
		       " cluster configuration.\n",
		     o2net_num_from_nn(nn),
		     o2net_idle_timeout() / 1000,
		     o2net_idle_timeout() % 1000);

		o2net_set_nn_state(nn, NULL, 0, 0);
	}
	spin_unlock(&nn->nn_lock);
}

static void o2net_still_up(struct work_struct *work)
{
	struct o2net_node *nn =
		container_of(work, struct o2net_node, nn_still_up.work);

	o2quo_hb_still_up(o2net_num_from_nn(nn));
}

/* ------------------------------------------------------------ */

void o2net_disconnect_node(struct o2nm_node *node)
{
	struct o2net_node *nn = o2net_nn_from_num(node->nd_num);

	/* don't reconnect until it's heartbeating again */
	spin_lock(&nn->nn_lock);
	atomic_set(&nn->nn_timeout, 0);
	o2net_set_nn_state(nn, NULL, 0, -ENOTCONN);
	spin_unlock(&nn->nn_lock);

	if (o2net_wq) {
		cancel_delayed_work(&nn->nn_connect_expired);
		cancel_delayed_work(&nn->nn_connect_work);
		cancel_delayed_work(&nn->nn_still_up);
		flush_workqueue(o2net_wq);
	}
}

static void o2net_hb_node_down_cb(struct o2nm_node *node, int node_num,
				  void *data)
{
	o2quo_hb_down(node_num);

	if (!node)
		return;

	if (node_num != o2nm_this_node())
		o2net_disconnect_node(node);

	BUG_ON(atomic_read(&o2net_connected_peers) < 0);
}

static void o2net_hb_node_up_cb(struct o2nm_node *node, int node_num,
				void *data)
{
	struct o2net_node *nn = o2net_nn_from_num(node_num);

	o2quo_hb_up(node_num);

	BUG_ON(!node);

	/* ensure an immediate connect attempt */
	nn->nn_last_connect_attempt = jiffies -
		(msecs_to_jiffies(o2net_reconnect_delay()) + 1);

	if (node_num != o2nm_this_node()) {
		/* believe it or not, accept and node heartbeating testing
		 * can succeed for this node before we got here.. so
		 * only use set_nn_state to clear the persistent error
		 * if that hasn't already happened */
		spin_lock(&nn->nn_lock);
		atomic_set(&nn->nn_timeout, 0);
		if (nn->nn_persistent_error)
			o2net_set_nn_state(nn, NULL, 0, 0);
		spin_unlock(&nn->nn_lock);
	}
}

void o2net_unregister_hb_callbacks(void)
{
	o2hb_unregister_callback(NULL, &o2net_hb_up);
	o2hb_unregister_callback(NULL, &o2net_hb_down);
}

int o2net_register_hb_callbacks(void)
{
	int ret;

	o2hb_setup_callback(&o2net_hb_down, O2HB_NODE_DOWN_CB,
			    o2net_hb_node_down_cb, NULL, O2NET_HB_PRI);
	o2hb_setup_callback(&o2net_hb_up, O2HB_NODE_UP_CB,
			    o2net_hb_node_up_cb, NULL, O2NET_HB_PRI);

	ret = o2hb_register_callback(NULL, &o2net_hb_up);
	if (ret == 0)
		ret = o2hb_register_callback(NULL, &o2net_hb_down);

	if (ret)
		o2net_unregister_hb_callbacks();

	return ret;
}

/* ------------------------------------------------------------ */

static int o2net_accept_one(struct socket *sock, int *more)
{
	int ret;
	struct sockaddr_in sin;
	struct socket *new_sock = NULL;
	struct o2nm_node *node = NULL;
	struct o2nm_node *local_node = NULL;
	struct o2net_sock_container *sc = NULL;
	struct o2net_node *nn;
	unsigned int nofs_flag;

	/*
	 * sock_create_lite allocates the sock with GFP_KERNEL. We must
	 * prevent the filesystem from being reentered by memory reclaim.
	 */
	nofs_flag = memalloc_nofs_save();

	BUG_ON(sock == NULL);
	*more = 0;
	ret = sock_create_lite(sock->sk->sk_family, sock->sk->sk_type,
			       sock->sk->sk_protocol, &new_sock);
	if (ret)
		goto out;

	new_sock->type = sock->type;
	new_sock->ops = sock->ops;
	ret = sock->ops->accept(sock, new_sock, O_NONBLOCK, false);
	if (ret < 0)
		goto out;

	*more = 1;
	new_sock->sk->sk_allocation = GFP_ATOMIC;

	tcp_sock_set_nodelay(new_sock->sk);
	tcp_sock_set_user_timeout(new_sock->sk, O2NET_TCP_USER_TIMEOUT);

	ret = new_sock->ops->getname(new_sock, (struct sockaddr *) &sin, 1);
	if (ret < 0)
		goto out;

	node = o2nm_get_node_by_ip(sin.sin_addr.s_addr);
	if (node == NULL) {
		printk(KERN_NOTICE "o2net: Attempt to connect from unknown "
		       "node at %pI4:%d\n", &sin.sin_addr.s_addr,
		       ntohs(sin.sin_port));
		ret = -EINVAL;
		goto out;
	}

	if (o2nm_this_node() >= node->nd_num) {
		local_node = o2nm_get_node_by_num(o2nm_this_node());
		if (local_node)
			printk(KERN_NOTICE "o2net: Unexpected connect attempt "
					"seen at node '%s' (%u, %pI4:%d) from "
					"node '%s' (%u, %pI4:%d)\n",
					local_node->nd_name, local_node->nd_num,
					&(local_node->nd_ipv4_address),
					ntohs(local_node->nd_ipv4_port),
					node->nd_name,
					node->nd_num, &sin.sin_addr.s_addr,
					ntohs(sin.sin_port));
		ret = -EINVAL;
		goto out;
	}

	/* this happens all the time when the other node sees our heartbeat
	 * and tries to connect before we see their heartbeat */
	if (!o2hb_check_node_heartbeating_from_callback(node->nd_num)) {
		mlog(ML_CONN, "attempt to connect from node '%s' at "
		     "%pI4:%d but it isn't heartbeating\n",
		     node->nd_name, &sin.sin_addr.s_addr,
		     ntohs(sin.sin_port));
		ret = -EINVAL;
		goto out;
	}

	nn = o2net_nn_from_num(node->nd_num);

	spin_lock(&nn->nn_lock);
	if (nn->nn_sc)
		ret = -EBUSY;
	else
		ret = 0;
	spin_unlock(&nn->nn_lock);
	if (ret) {
		printk(KERN_NOTICE "o2net: Attempt to connect from node '%s' "
		       "at %pI4:%d but it already has an open connection\n",
		       node->nd_name, &sin.sin_addr.s_addr,
		       ntohs(sin.sin_port));
		goto out;
	}

	sc = sc_alloc(node);
	if (sc == NULL) {
		ret = -ENOMEM;
		goto out;
	}

	sc->sc_sock = new_sock;
	new_sock = NULL;

	spin_lock(&nn->nn_lock);
	atomic_set(&nn->nn_timeout, 0);
	o2net_set_nn_state(nn, sc, 0, 0);
	spin_unlock(&nn->nn_lock);

	o2net_register_callbacks(sc->sc_sock->sk, sc);
	o2net_sc_queue_work(sc, &sc->sc_rx_work);

	o2net_initialize_handshake();
	o2net_sendpage(sc, o2net_hand, sizeof(*o2net_hand));

out:
	if (new_sock)
		sock_release(new_sock);
	if (node)
		o2nm_node_put(node);
	if (local_node)
		o2nm_node_put(local_node);
	if (sc)
		sc_put(sc);

	memalloc_nofs_restore(nofs_flag);
	return ret;
}

/*
 * This function is invoked in response to one or more
 * pending accepts at softIRQ level. We must drain the
 * entire que before returning.
 */

static void o2net_accept_many(struct work_struct *work)
{
	struct socket *sock = o2net_listen_sock;
	int	more;

	/*
	 * It is critical to note that due to interrupt moderation
	 * at the network driver level, we can't assume to get a
	 * softIRQ for every single conn since tcp SYN packets
	 * can arrive back-to-back, and therefore many pending
	 * accepts may result in just 1 softIRQ. If we terminate
	 * the o2net_accept_one() loop upon seeing an err, what happens
	 * to the rest of the conns in the queue? If no new SYN
	 * arrives for hours, no softIRQ  will be delivered,
	 * and the connections will just sit in the queue.
	 */

	for (;;) {
		o2net_accept_one(sock, &more);
		if (!more)
			break;
		cond_resched();
	}
}

static void o2net_listen_data_ready(struct sock *sk)
{
	void (*ready)(struct sock *sk);

	read_lock_bh(&sk->sk_callback_lock);
	ready = sk->sk_user_data;
	if (ready == NULL) { /* check for teardown race */
		ready = sk->sk_data_ready;
		goto out;
	}

	/* This callback may called twice when a new connection
	 * is  being established as a child socket inherits everything
	 * from a parent LISTEN socket, including the data_ready cb of
	 * the parent. This leads to a hazard. In o2net_accept_one()
	 * we are still initializing the child socket but have not
	 * changed the inherited data_ready callback yet when
	 * data starts arriving.
	 * We avoid this hazard by checking the state.
	 * For the listening socket,  the state will be TCP_LISTEN; for the new
	 * socket, will be  TCP_ESTABLISHED. Also, in this case,
	 * sk->sk_user_data is not a valid function pointer.
	 */

	if (sk->sk_state == TCP_LISTEN) {
		queue_work(o2net_wq, &o2net_listen_work);
	} else {
		ready = NULL;
	}

out:
	read_unlock_bh(&sk->sk_callback_lock);
	if (ready != NULL)
		ready(sk);
}

static int o2net_open_listening_sock(__be32 addr, __be16 port)
{
	struct socket *sock = NULL;
	int ret;
	struct sockaddr_in sin = {
		.sin_family = PF_INET,
		.sin_addr = { .s_addr = addr },
		.sin_port = port,
	};

	ret = sock_create(PF_INET, SOCK_STREAM, IPPROTO_TCP, &sock);
	if (ret < 0) {
		printk(KERN_ERR "o2net: Error %d while creating socket\n", ret);
		goto out;
	}

	sock->sk->sk_allocation = GFP_ATOMIC;

	write_lock_bh(&sock->sk->sk_callback_lock);
	sock->sk->sk_user_data = sock->sk->sk_data_ready;
	sock->sk->sk_data_ready = o2net_listen_data_ready;
	write_unlock_bh(&sock->sk->sk_callback_lock);

	o2net_listen_sock = sock;
	INIT_WORK(&o2net_listen_work, o2net_accept_many);

	sock->sk->sk_reuse = SK_CAN_REUSE;
	ret = sock->ops->bind(sock, (struct sockaddr *)&sin, sizeof(sin));
	if (ret < 0) {
		printk(KERN_ERR "o2net: Error %d while binding socket at "
		       "%pI4:%u\n", ret, &addr, ntohs(port)); 
		goto out;
	}

	ret = sock->ops->listen(sock, 64);
	if (ret < 0)
		printk(KERN_ERR "o2net: Error %d while listening on %pI4:%u\n",
		       ret, &addr, ntohs(port));

out:
	if (ret) {
		o2net_listen_sock = NULL;
		if (sock)
			sock_release(sock);
	}
	return ret;
}

/*
 * called from node manager when we should bring up our network listening
 * socket.  node manager handles all the serialization to only call this
 * once and to match it with o2net_stop_listening().  note,
 * o2nm_this_node() doesn't work yet as we're being called while it
 * is being set up.
 */
int o2net_start_listening(struct o2nm_node *node)
{
	int ret = 0;

	BUG_ON(o2net_wq != NULL);
	BUG_ON(o2net_listen_sock != NULL);

	mlog(ML_KTHREAD, "starting o2net thread...\n");
	o2net_wq = alloc_ordered_workqueue("o2net", WQ_MEM_RECLAIM);
	if (o2net_wq == NULL) {
		mlog(ML_ERROR, "unable to launch o2net thread\n");
		return -ENOMEM; /* ? */
	}

	ret = o2net_open_listening_sock(node->nd_ipv4_address,
					node->nd_ipv4_port);
	if (ret) {
		destroy_workqueue(o2net_wq);
		o2net_wq = NULL;
	} else
		o2quo_conn_up(node->nd_num);

	return ret;
}

/* again, o2nm_this_node() doesn't work here as we're involved in
 * tearing it down */
void o2net_stop_listening(struct o2nm_node *node)
{
	struct socket *sock = o2net_listen_sock;
	size_t i;

	BUG_ON(o2net_wq == NULL);
	BUG_ON(o2net_listen_sock == NULL);

	/* stop the listening socket from generating work */
	write_lock_bh(&sock->sk->sk_callback_lock);
	sock->sk->sk_data_ready = sock->sk->sk_user_data;
	sock->sk->sk_user_data = NULL;
	write_unlock_bh(&sock->sk->sk_callback_lock);

	for (i = 0; i < ARRAY_SIZE(o2net_nodes); i++) {
		struct o2nm_node *node = o2nm_get_node_by_num(i);
		if (node) {
			o2net_disconnect_node(node);
			o2nm_node_put(node);
		}
	}

	/* finish all work and tear down the work queue */
	mlog(ML_KTHREAD, "waiting for o2net thread to exit....\n");
	destroy_workqueue(o2net_wq);
	o2net_wq = NULL;

	sock_release(o2net_listen_sock);
	o2net_listen_sock = NULL;

	o2quo_conn_err(node->nd_num);
}

/* ------------------------------------------------------------ */

int o2net_init(void)
{
	unsigned long i;

	o2quo_init();

	o2net_debugfs_init();

	o2net_hand = kzalloc(sizeof(struct o2net_handshake), GFP_KERNEL);
	o2net_keep_req = kzalloc(sizeof(struct o2net_msg), GFP_KERNEL);
	o2net_keep_resp = kzalloc(sizeof(struct o2net_msg), GFP_KERNEL);
	if (!o2net_hand || !o2net_keep_req || !o2net_keep_resp)
		goto out;

	o2net_hand->protocol_version = cpu_to_be64(O2NET_PROTOCOL_VERSION);
	o2net_hand->connector_id = cpu_to_be64(1);

	o2net_keep_req->magic = cpu_to_be16(O2NET_MSG_KEEP_REQ_MAGIC);
	o2net_keep_resp->magic = cpu_to_be16(O2NET_MSG_KEEP_RESP_MAGIC);

	for (i = 0; i < ARRAY_SIZE(o2net_nodes); i++) {
		struct o2net_node *nn = o2net_nn_from_num(i);

		atomic_set(&nn->nn_timeout, 0);
		spin_lock_init(&nn->nn_lock);
		INIT_DELAYED_WORK(&nn->nn_connect_work, o2net_start_connect);
		INIT_DELAYED_WORK(&nn->nn_connect_expired,
				  o2net_connect_expired);
		INIT_DELAYED_WORK(&nn->nn_still_up, o2net_still_up);
		/* until we see hb from a node we'll return einval */
		nn->nn_persistent_error = -ENOTCONN;
		init_waitqueue_head(&nn->nn_sc_wq);
		idr_init(&nn->nn_status_idr);
		INIT_LIST_HEAD(&nn->nn_status_list);
	}

	return 0;

out:
	kfree(o2net_hand);
	kfree(o2net_keep_req);
	kfree(o2net_keep_resp);
	o2net_debugfs_exit();
	o2quo_exit();
	return -ENOMEM;
}

void o2net_exit(void)
{
	o2quo_exit();
	kfree(o2net_hand);
	kfree(o2net_keep_req);
	kfree(o2net_keep_resp);
	o2net_debugfs_exit();
}
