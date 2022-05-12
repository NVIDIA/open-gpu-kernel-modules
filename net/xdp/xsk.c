// SPDX-License-Identifier: GPL-2.0
/* XDP sockets
 *
 * AF_XDP sockets allows a channel between XDP programs and userspace
 * applications.
 * Copyright(c) 2018 Intel Corporation.
 *
 * Author(s): Björn Töpel <bjorn.topel@intel.com>
 *	      Magnus Karlsson <magnus.karlsson@intel.com>
 */

#define pr_fmt(fmt) "AF_XDP: %s: " fmt, __func__

#include <linux/if_xdp.h>
#include <linux/init.h>
#include <linux/sched/mm.h>
#include <linux/sched/signal.h>
#include <linux/sched/task.h>
#include <linux/socket.h>
#include <linux/file.h>
#include <linux/uaccess.h>
#include <linux/net.h>
#include <linux/netdevice.h>
#include <linux/rculist.h>
#include <net/xdp_sock_drv.h>
#include <net/busy_poll.h>
#include <net/xdp.h>

#include "xsk_queue.h"
#include "xdp_umem.h"
#include "xsk.h"

#define TX_BATCH_SIZE 32

static DEFINE_PER_CPU(struct list_head, xskmap_flush_list);

void xsk_set_rx_need_wakeup(struct xsk_buff_pool *pool)
{
	if (pool->cached_need_wakeup & XDP_WAKEUP_RX)
		return;

	pool->fq->ring->flags |= XDP_RING_NEED_WAKEUP;
	pool->cached_need_wakeup |= XDP_WAKEUP_RX;
}
EXPORT_SYMBOL(xsk_set_rx_need_wakeup);

void xsk_set_tx_need_wakeup(struct xsk_buff_pool *pool)
{
	struct xdp_sock *xs;

	if (pool->cached_need_wakeup & XDP_WAKEUP_TX)
		return;

	rcu_read_lock();
	list_for_each_entry_rcu(xs, &pool->xsk_tx_list, tx_list) {
		xs->tx->ring->flags |= XDP_RING_NEED_WAKEUP;
	}
	rcu_read_unlock();

	pool->cached_need_wakeup |= XDP_WAKEUP_TX;
}
EXPORT_SYMBOL(xsk_set_tx_need_wakeup);

void xsk_clear_rx_need_wakeup(struct xsk_buff_pool *pool)
{
	if (!(pool->cached_need_wakeup & XDP_WAKEUP_RX))
		return;

	pool->fq->ring->flags &= ~XDP_RING_NEED_WAKEUP;
	pool->cached_need_wakeup &= ~XDP_WAKEUP_RX;
}
EXPORT_SYMBOL(xsk_clear_rx_need_wakeup);

void xsk_clear_tx_need_wakeup(struct xsk_buff_pool *pool)
{
	struct xdp_sock *xs;

	if (!(pool->cached_need_wakeup & XDP_WAKEUP_TX))
		return;

	rcu_read_lock();
	list_for_each_entry_rcu(xs, &pool->xsk_tx_list, tx_list) {
		xs->tx->ring->flags &= ~XDP_RING_NEED_WAKEUP;
	}
	rcu_read_unlock();

	pool->cached_need_wakeup &= ~XDP_WAKEUP_TX;
}
EXPORT_SYMBOL(xsk_clear_tx_need_wakeup);

bool xsk_uses_need_wakeup(struct xsk_buff_pool *pool)
{
	return pool->uses_need_wakeup;
}
EXPORT_SYMBOL(xsk_uses_need_wakeup);

struct xsk_buff_pool *xsk_get_pool_from_qid(struct net_device *dev,
					    u16 queue_id)
{
	if (queue_id < dev->real_num_rx_queues)
		return dev->_rx[queue_id].pool;
	if (queue_id < dev->real_num_tx_queues)
		return dev->_tx[queue_id].pool;

	return NULL;
}
EXPORT_SYMBOL(xsk_get_pool_from_qid);

void xsk_clear_pool_at_qid(struct net_device *dev, u16 queue_id)
{
	if (queue_id < dev->num_rx_queues)
		dev->_rx[queue_id].pool = NULL;
	if (queue_id < dev->num_tx_queues)
		dev->_tx[queue_id].pool = NULL;
}

/* The buffer pool is stored both in the _rx struct and the _tx struct as we do
 * not know if the device has more tx queues than rx, or the opposite.
 * This might also change during run time.
 */
int xsk_reg_pool_at_qid(struct net_device *dev, struct xsk_buff_pool *pool,
			u16 queue_id)
{
	if (queue_id >= max_t(unsigned int,
			      dev->real_num_rx_queues,
			      dev->real_num_tx_queues))
		return -EINVAL;

	if (queue_id < dev->real_num_rx_queues)
		dev->_rx[queue_id].pool = pool;
	if (queue_id < dev->real_num_tx_queues)
		dev->_tx[queue_id].pool = pool;

	return 0;
}

void xp_release(struct xdp_buff_xsk *xskb)
{
	xskb->pool->free_heads[xskb->pool->free_heads_cnt++] = xskb;
}

static u64 xp_get_handle(struct xdp_buff_xsk *xskb)
{
	u64 offset = xskb->xdp.data - xskb->xdp.data_hard_start;

	offset += xskb->pool->headroom;
	if (!xskb->pool->unaligned)
		return xskb->orig_addr + offset;
	return xskb->orig_addr + (offset << XSK_UNALIGNED_BUF_OFFSET_SHIFT);
}

static int __xsk_rcv_zc(struct xdp_sock *xs, struct xdp_buff *xdp, u32 len)
{
	struct xdp_buff_xsk *xskb = container_of(xdp, struct xdp_buff_xsk, xdp);
	u64 addr;
	int err;

	addr = xp_get_handle(xskb);
	err = xskq_prod_reserve_desc(xs->rx, addr, len);
	if (err) {
		xs->rx_queue_full++;
		return err;
	}

	xp_release(xskb);
	return 0;
}

static void xsk_copy_xdp(struct xdp_buff *to, struct xdp_buff *from, u32 len)
{
	void *from_buf, *to_buf;
	u32 metalen;

	if (unlikely(xdp_data_meta_unsupported(from))) {
		from_buf = from->data;
		to_buf = to->data;
		metalen = 0;
	} else {
		from_buf = from->data_meta;
		metalen = from->data - from->data_meta;
		to_buf = to->data - metalen;
	}

	memcpy(to_buf, from_buf, len + metalen);
}

static int __xsk_rcv(struct xdp_sock *xs, struct xdp_buff *xdp)
{
	struct xdp_buff *xsk_xdp;
	int err;
	u32 len;

	len = xdp->data_end - xdp->data;
	if (len > xsk_pool_get_rx_frame_size(xs->pool)) {
		xs->rx_dropped++;
		return -ENOSPC;
	}

	xsk_xdp = xsk_buff_alloc(xs->pool);
	if (!xsk_xdp) {
		xs->rx_dropped++;
		return -ENOSPC;
	}

	xsk_copy_xdp(xsk_xdp, xdp, len);
	err = __xsk_rcv_zc(xs, xsk_xdp, len);
	if (err) {
		xsk_buff_free(xsk_xdp);
		return err;
	}
	return 0;
}

static bool xsk_tx_writeable(struct xdp_sock *xs)
{
	if (xskq_cons_present_entries(xs->tx) > xs->tx->nentries / 2)
		return false;

	return true;
}

static bool xsk_is_bound(struct xdp_sock *xs)
{
	if (READ_ONCE(xs->state) == XSK_BOUND) {
		/* Matches smp_wmb() in bind(). */
		smp_rmb();
		return true;
	}
	return false;
}

static int xsk_rcv_check(struct xdp_sock *xs, struct xdp_buff *xdp)
{
	if (!xsk_is_bound(xs))
		return -EINVAL;

	if (xs->dev != xdp->rxq->dev || xs->queue_id != xdp->rxq->queue_index)
		return -EINVAL;

	sk_mark_napi_id_once_xdp(&xs->sk, xdp);
	return 0;
}

static void xsk_flush(struct xdp_sock *xs)
{
	xskq_prod_submit(xs->rx);
	__xskq_cons_release(xs->pool->fq);
	sock_def_readable(&xs->sk);
}

int xsk_generic_rcv(struct xdp_sock *xs, struct xdp_buff *xdp)
{
	int err;

	spin_lock_bh(&xs->rx_lock);
	err = xsk_rcv_check(xs, xdp);
	if (!err) {
		err = __xsk_rcv(xs, xdp);
		xsk_flush(xs);
	}
	spin_unlock_bh(&xs->rx_lock);
	return err;
}

static int xsk_rcv(struct xdp_sock *xs, struct xdp_buff *xdp)
{
	int err;
	u32 len;

	err = xsk_rcv_check(xs, xdp);
	if (err)
		return err;

	if (xdp->rxq->mem.type == MEM_TYPE_XSK_BUFF_POOL) {
		len = xdp->data_end - xdp->data;
		return __xsk_rcv_zc(xs, xdp, len);
	}

	err = __xsk_rcv(xs, xdp);
	if (!err)
		xdp_return_buff(xdp);
	return err;
}

int __xsk_map_redirect(struct xdp_sock *xs, struct xdp_buff *xdp)
{
	struct list_head *flush_list = this_cpu_ptr(&xskmap_flush_list);
	int err;

	err = xsk_rcv(xs, xdp);
	if (err)
		return err;

	if (!xs->flush_node.prev)
		list_add(&xs->flush_node, flush_list);

	return 0;
}

void __xsk_map_flush(void)
{
	struct list_head *flush_list = this_cpu_ptr(&xskmap_flush_list);
	struct xdp_sock *xs, *tmp;

	list_for_each_entry_safe(xs, tmp, flush_list, flush_node) {
		xsk_flush(xs);
		__list_del_clearprev(&xs->flush_node);
	}
}

void xsk_tx_completed(struct xsk_buff_pool *pool, u32 nb_entries)
{
	xskq_prod_submit_n(pool->cq, nb_entries);
}
EXPORT_SYMBOL(xsk_tx_completed);

void xsk_tx_release(struct xsk_buff_pool *pool)
{
	struct xdp_sock *xs;

	rcu_read_lock();
	list_for_each_entry_rcu(xs, &pool->xsk_tx_list, tx_list) {
		__xskq_cons_release(xs->tx);
		if (xsk_tx_writeable(xs))
			xs->sk.sk_write_space(&xs->sk);
	}
	rcu_read_unlock();
}
EXPORT_SYMBOL(xsk_tx_release);

bool xsk_tx_peek_desc(struct xsk_buff_pool *pool, struct xdp_desc *desc)
{
	struct xdp_sock *xs;

	rcu_read_lock();
	list_for_each_entry_rcu(xs, &pool->xsk_tx_list, tx_list) {
		if (!xskq_cons_peek_desc(xs->tx, desc, pool)) {
			xs->tx->queue_empty_descs++;
			continue;
		}

		/* This is the backpressure mechanism for the Tx path.
		 * Reserve space in the completion queue and only proceed
		 * if there is space in it. This avoids having to implement
		 * any buffering in the Tx path.
		 */
		if (xskq_prod_reserve_addr(pool->cq, desc->addr))
			goto out;

		xskq_cons_release(xs->tx);
		rcu_read_unlock();
		return true;
	}

out:
	rcu_read_unlock();
	return false;
}
EXPORT_SYMBOL(xsk_tx_peek_desc);

static u32 xsk_tx_peek_release_fallback(struct xsk_buff_pool *pool, struct xdp_desc *descs,
					u32 max_entries)
{
	u32 nb_pkts = 0;

	while (nb_pkts < max_entries && xsk_tx_peek_desc(pool, &descs[nb_pkts]))
		nb_pkts++;

	xsk_tx_release(pool);
	return nb_pkts;
}

u32 xsk_tx_peek_release_desc_batch(struct xsk_buff_pool *pool, struct xdp_desc *descs,
				   u32 max_entries)
{
	struct xdp_sock *xs;
	u32 nb_pkts;

	rcu_read_lock();
	if (!list_is_singular(&pool->xsk_tx_list)) {
		/* Fallback to the non-batched version */
		rcu_read_unlock();
		return xsk_tx_peek_release_fallback(pool, descs, max_entries);
	}

	xs = list_first_or_null_rcu(&pool->xsk_tx_list, struct xdp_sock, tx_list);
	if (!xs) {
		nb_pkts = 0;
		goto out;
	}

	nb_pkts = xskq_cons_peek_desc_batch(xs->tx, descs, pool, max_entries);
	if (!nb_pkts) {
		xs->tx->queue_empty_descs++;
		goto out;
	}

	/* This is the backpressure mechanism for the Tx path. Try to
	 * reserve space in the completion queue for all packets, but
	 * if there are fewer slots available, just process that many
	 * packets. This avoids having to implement any buffering in
	 * the Tx path.
	 */
	nb_pkts = xskq_prod_reserve_addr_batch(pool->cq, descs, nb_pkts);
	if (!nb_pkts)
		goto out;

	xskq_cons_release_n(xs->tx, nb_pkts);
	__xskq_cons_release(xs->tx);
	xs->sk.sk_write_space(&xs->sk);

out:
	rcu_read_unlock();
	return nb_pkts;
}
EXPORT_SYMBOL(xsk_tx_peek_release_desc_batch);

static int xsk_wakeup(struct xdp_sock *xs, u8 flags)
{
	struct net_device *dev = xs->dev;
	int err;

	rcu_read_lock();
	err = dev->netdev_ops->ndo_xsk_wakeup(dev, xs->queue_id, flags);
	rcu_read_unlock();

	return err;
}

static int xsk_zc_xmit(struct xdp_sock *xs)
{
	return xsk_wakeup(xs, XDP_WAKEUP_TX);
}

static void xsk_destruct_skb(struct sk_buff *skb)
{
	u64 addr = (u64)(long)skb_shinfo(skb)->destructor_arg;
	struct xdp_sock *xs = xdp_sk(skb->sk);
	unsigned long flags;

	spin_lock_irqsave(&xs->pool->cq_lock, flags);
	xskq_prod_submit_addr(xs->pool->cq, addr);
	spin_unlock_irqrestore(&xs->pool->cq_lock, flags);

	sock_wfree(skb);
}

static struct sk_buff *xsk_build_skb_zerocopy(struct xdp_sock *xs,
					      struct xdp_desc *desc)
{
	struct xsk_buff_pool *pool = xs->pool;
	u32 hr, len, ts, offset, copy, copied;
	struct sk_buff *skb;
	struct page *page;
	void *buffer;
	int err, i;
	u64 addr;

	hr = max(NET_SKB_PAD, L1_CACHE_ALIGN(xs->dev->needed_headroom));

	skb = sock_alloc_send_skb(&xs->sk, hr, 1, &err);
	if (unlikely(!skb))
		return ERR_PTR(err);

	skb_reserve(skb, hr);

	addr = desc->addr;
	len = desc->len;
	ts = pool->unaligned ? len : pool->chunk_size;

	buffer = xsk_buff_raw_get_data(pool, addr);
	offset = offset_in_page(buffer);
	addr = buffer - pool->addrs;

	for (copied = 0, i = 0; copied < len; i++) {
		page = pool->umem->pgs[addr >> PAGE_SHIFT];
		get_page(page);

		copy = min_t(u32, PAGE_SIZE - offset, len - copied);
		skb_fill_page_desc(skb, i, page, offset, copy);

		copied += copy;
		addr += copy;
		offset = 0;
	}

	skb->len += len;
	skb->data_len += len;
	skb->truesize += ts;

	refcount_add(ts, &xs->sk.sk_wmem_alloc);

	return skb;
}

static struct sk_buff *xsk_build_skb(struct xdp_sock *xs,
				     struct xdp_desc *desc)
{
	struct net_device *dev = xs->dev;
	struct sk_buff *skb;

	if (dev->priv_flags & IFF_TX_SKB_NO_LINEAR) {
		skb = xsk_build_skb_zerocopy(xs, desc);
		if (IS_ERR(skb))
			return skb;
	} else {
		u32 hr, tr, len;
		void *buffer;
		int err;

		hr = max(NET_SKB_PAD, L1_CACHE_ALIGN(dev->needed_headroom));
		tr = dev->needed_tailroom;
		len = desc->len;

		skb = sock_alloc_send_skb(&xs->sk, hr + len + tr, 1, &err);
		if (unlikely(!skb))
			return ERR_PTR(err);

		skb_reserve(skb, hr);
		skb_put(skb, len);

		buffer = xsk_buff_raw_get_data(xs->pool, desc->addr);
		err = skb_store_bits(skb, 0, buffer, len);
		if (unlikely(err)) {
			kfree_skb(skb);
			return ERR_PTR(err);
		}
	}

	skb->dev = dev;
	skb->priority = xs->sk.sk_priority;
	skb->mark = xs->sk.sk_mark;
	skb_shinfo(skb)->destructor_arg = (void *)(long)desc->addr;
	skb->destructor = xsk_destruct_skb;

	return skb;
}

static int xsk_generic_xmit(struct sock *sk)
{
	struct xdp_sock *xs = xdp_sk(sk);
	u32 max_batch = TX_BATCH_SIZE;
	bool sent_frame = false;
	struct xdp_desc desc;
	struct sk_buff *skb;
	unsigned long flags;
	int err = 0;

	mutex_lock(&xs->mutex);

	if (xs->queue_id >= xs->dev->real_num_tx_queues)
		goto out;

	while (xskq_cons_peek_desc(xs->tx, &desc, xs->pool)) {
		if (max_batch-- == 0) {
			err = -EAGAIN;
			goto out;
		}

		skb = xsk_build_skb(xs, &desc);
		if (IS_ERR(skb)) {
			err = PTR_ERR(skb);
			goto out;
		}

		/* This is the backpressure mechanism for the Tx path.
		 * Reserve space in the completion queue and only proceed
		 * if there is space in it. This avoids having to implement
		 * any buffering in the Tx path.
		 */
		spin_lock_irqsave(&xs->pool->cq_lock, flags);
		if (xskq_prod_reserve(xs->pool->cq)) {
			spin_unlock_irqrestore(&xs->pool->cq_lock, flags);
			kfree_skb(skb);
			goto out;
		}
		spin_unlock_irqrestore(&xs->pool->cq_lock, flags);

		err = __dev_direct_xmit(skb, xs->queue_id);
		if  (err == NETDEV_TX_BUSY) {
			/* Tell user-space to retry the send */
			skb->destructor = sock_wfree;
			spin_lock_irqsave(&xs->pool->cq_lock, flags);
			xskq_prod_cancel(xs->pool->cq);
			spin_unlock_irqrestore(&xs->pool->cq_lock, flags);
			/* Free skb without triggering the perf drop trace */
			consume_skb(skb);
			err = -EAGAIN;
			goto out;
		}

		xskq_cons_release(xs->tx);
		/* Ignore NET_XMIT_CN as packet might have been sent */
		if (err == NET_XMIT_DROP) {
			/* SKB completed but not sent */
			err = -EBUSY;
			goto out;
		}

		sent_frame = true;
	}

	xs->tx->queue_empty_descs++;

out:
	if (sent_frame)
		if (xsk_tx_writeable(xs))
			sk->sk_write_space(sk);

	mutex_unlock(&xs->mutex);
	return err;
}

static int __xsk_sendmsg(struct sock *sk)
{
	struct xdp_sock *xs = xdp_sk(sk);

	if (unlikely(!(xs->dev->flags & IFF_UP)))
		return -ENETDOWN;
	if (unlikely(!xs->tx))
		return -ENOBUFS;

	return xs->zc ? xsk_zc_xmit(xs) : xsk_generic_xmit(sk);
}

static bool xsk_no_wakeup(struct sock *sk)
{
#ifdef CONFIG_NET_RX_BUSY_POLL
	/* Prefer busy-polling, skip the wakeup. */
	return READ_ONCE(sk->sk_prefer_busy_poll) && READ_ONCE(sk->sk_ll_usec) &&
		READ_ONCE(sk->sk_napi_id) >= MIN_NAPI_ID;
#else
	return false;
#endif
}

static int xsk_sendmsg(struct socket *sock, struct msghdr *m, size_t total_len)
{
	bool need_wait = !(m->msg_flags & MSG_DONTWAIT);
	struct sock *sk = sock->sk;
	struct xdp_sock *xs = xdp_sk(sk);
	struct xsk_buff_pool *pool;

	if (unlikely(!xsk_is_bound(xs)))
		return -ENXIO;
	if (unlikely(need_wait))
		return -EOPNOTSUPP;

	if (sk_can_busy_loop(sk))
		sk_busy_loop(sk, 1); /* only support non-blocking sockets */

	if (xsk_no_wakeup(sk))
		return 0;

	pool = xs->pool;
	if (pool->cached_need_wakeup & XDP_WAKEUP_TX)
		return __xsk_sendmsg(sk);
	return 0;
}

static int xsk_recvmsg(struct socket *sock, struct msghdr *m, size_t len, int flags)
{
	bool need_wait = !(flags & MSG_DONTWAIT);
	struct sock *sk = sock->sk;
	struct xdp_sock *xs = xdp_sk(sk);

	if (unlikely(!xsk_is_bound(xs)))
		return -ENXIO;
	if (unlikely(!(xs->dev->flags & IFF_UP)))
		return -ENETDOWN;
	if (unlikely(!xs->rx))
		return -ENOBUFS;
	if (unlikely(need_wait))
		return -EOPNOTSUPP;

	if (sk_can_busy_loop(sk))
		sk_busy_loop(sk, 1); /* only support non-blocking sockets */

	if (xsk_no_wakeup(sk))
		return 0;

	if (xs->pool->cached_need_wakeup & XDP_WAKEUP_RX && xs->zc)
		return xsk_wakeup(xs, XDP_WAKEUP_RX);
	return 0;
}

static __poll_t xsk_poll(struct file *file, struct socket *sock,
			     struct poll_table_struct *wait)
{
	__poll_t mask = 0;
	struct sock *sk = sock->sk;
	struct xdp_sock *xs = xdp_sk(sk);
	struct xsk_buff_pool *pool;

	sock_poll_wait(file, sock, wait);

	if (unlikely(!xsk_is_bound(xs)))
		return mask;

	pool = xs->pool;

	if (pool->cached_need_wakeup) {
		if (xs->zc)
			xsk_wakeup(xs, pool->cached_need_wakeup);
		else
			/* Poll needs to drive Tx also in copy mode */
			__xsk_sendmsg(sk);
	}

	if (xs->rx && !xskq_prod_is_empty(xs->rx))
		mask |= EPOLLIN | EPOLLRDNORM;
	if (xs->tx && xsk_tx_writeable(xs))
		mask |= EPOLLOUT | EPOLLWRNORM;

	return mask;
}

static int xsk_init_queue(u32 entries, struct xsk_queue **queue,
			  bool umem_queue)
{
	struct xsk_queue *q;

	if (entries == 0 || *queue || !is_power_of_2(entries))
		return -EINVAL;

	q = xskq_create(entries, umem_queue);
	if (!q)
		return -ENOMEM;

	/* Make sure queue is ready before it can be seen by others */
	smp_wmb();
	WRITE_ONCE(*queue, q);
	return 0;
}

static void xsk_unbind_dev(struct xdp_sock *xs)
{
	struct net_device *dev = xs->dev;

	if (xs->state != XSK_BOUND)
		return;
	WRITE_ONCE(xs->state, XSK_UNBOUND);

	/* Wait for driver to stop using the xdp socket. */
	xp_del_xsk(xs->pool, xs);
	xs->dev = NULL;
	synchronize_net();
	dev_put(dev);
}

static struct xsk_map *xsk_get_map_list_entry(struct xdp_sock *xs,
					      struct xdp_sock ***map_entry)
{
	struct xsk_map *map = NULL;
	struct xsk_map_node *node;

	*map_entry = NULL;

	spin_lock_bh(&xs->map_list_lock);
	node = list_first_entry_or_null(&xs->map_list, struct xsk_map_node,
					node);
	if (node) {
		bpf_map_inc(&node->map->map);
		map = node->map;
		*map_entry = node->map_entry;
	}
	spin_unlock_bh(&xs->map_list_lock);
	return map;
}

static void xsk_delete_from_maps(struct xdp_sock *xs)
{
	/* This function removes the current XDP socket from all the
	 * maps it resides in. We need to take extra care here, due to
	 * the two locks involved. Each map has a lock synchronizing
	 * updates to the entries, and each socket has a lock that
	 * synchronizes access to the list of maps (map_list). For
	 * deadlock avoidance the locks need to be taken in the order
	 * "map lock"->"socket map list lock". We start off by
	 * accessing the socket map list, and take a reference to the
	 * map to guarantee existence between the
	 * xsk_get_map_list_entry() and xsk_map_try_sock_delete()
	 * calls. Then we ask the map to remove the socket, which
	 * tries to remove the socket from the map. Note that there
	 * might be updates to the map between
	 * xsk_get_map_list_entry() and xsk_map_try_sock_delete().
	 */
	struct xdp_sock **map_entry = NULL;
	struct xsk_map *map;

	while ((map = xsk_get_map_list_entry(xs, &map_entry))) {
		xsk_map_try_sock_delete(map, xs, map_entry);
		bpf_map_put(&map->map);
	}
}

static int xsk_release(struct socket *sock)
{
	struct sock *sk = sock->sk;
	struct xdp_sock *xs = xdp_sk(sk);
	struct net *net;

	if (!sk)
		return 0;

	net = sock_net(sk);

	mutex_lock(&net->xdp.lock);
	sk_del_node_init_rcu(sk);
	mutex_unlock(&net->xdp.lock);

	local_bh_disable();
	sock_prot_inuse_add(net, sk->sk_prot, -1);
	local_bh_enable();

	xsk_delete_from_maps(xs);
	mutex_lock(&xs->mutex);
	xsk_unbind_dev(xs);
	mutex_unlock(&xs->mutex);

	xskq_destroy(xs->rx);
	xskq_destroy(xs->tx);
	xskq_destroy(xs->fq_tmp);
	xskq_destroy(xs->cq_tmp);

	sock_orphan(sk);
	sock->sk = NULL;

	sk_refcnt_debug_release(sk);
	sock_put(sk);

	return 0;
}

static struct socket *xsk_lookup_xsk_from_fd(int fd)
{
	struct socket *sock;
	int err;

	sock = sockfd_lookup(fd, &err);
	if (!sock)
		return ERR_PTR(-ENOTSOCK);

	if (sock->sk->sk_family != PF_XDP) {
		sockfd_put(sock);
		return ERR_PTR(-ENOPROTOOPT);
	}

	return sock;
}

static bool xsk_validate_queues(struct xdp_sock *xs)
{
	return xs->fq_tmp && xs->cq_tmp;
}

static int xsk_bind(struct socket *sock, struct sockaddr *addr, int addr_len)
{
	struct sockaddr_xdp *sxdp = (struct sockaddr_xdp *)addr;
	struct sock *sk = sock->sk;
	struct xdp_sock *xs = xdp_sk(sk);
	struct net_device *dev;
	u32 flags, qid;
	int err = 0;

	if (addr_len < sizeof(struct sockaddr_xdp))
		return -EINVAL;
	if (sxdp->sxdp_family != AF_XDP)
		return -EINVAL;

	flags = sxdp->sxdp_flags;
	if (flags & ~(XDP_SHARED_UMEM | XDP_COPY | XDP_ZEROCOPY |
		      XDP_USE_NEED_WAKEUP))
		return -EINVAL;

	rtnl_lock();
	mutex_lock(&xs->mutex);
	if (xs->state != XSK_READY) {
		err = -EBUSY;
		goto out_release;
	}

	dev = dev_get_by_index(sock_net(sk), sxdp->sxdp_ifindex);
	if (!dev) {
		err = -ENODEV;
		goto out_release;
	}

	if (!xs->rx && !xs->tx) {
		err = -EINVAL;
		goto out_unlock;
	}

	qid = sxdp->sxdp_queue_id;

	if (flags & XDP_SHARED_UMEM) {
		struct xdp_sock *umem_xs;
		struct socket *sock;

		if ((flags & XDP_COPY) || (flags & XDP_ZEROCOPY) ||
		    (flags & XDP_USE_NEED_WAKEUP)) {
			/* Cannot specify flags for shared sockets. */
			err = -EINVAL;
			goto out_unlock;
		}

		if (xs->umem) {
			/* We have already our own. */
			err = -EINVAL;
			goto out_unlock;
		}

		sock = xsk_lookup_xsk_from_fd(sxdp->sxdp_shared_umem_fd);
		if (IS_ERR(sock)) {
			err = PTR_ERR(sock);
			goto out_unlock;
		}

		umem_xs = xdp_sk(sock->sk);
		if (!xsk_is_bound(umem_xs)) {
			err = -EBADF;
			sockfd_put(sock);
			goto out_unlock;
		}

		if (umem_xs->queue_id != qid || umem_xs->dev != dev) {
			/* Share the umem with another socket on another qid
			 * and/or device.
			 */
			xs->pool = xp_create_and_assign_umem(xs,
							     umem_xs->umem);
			if (!xs->pool) {
				err = -ENOMEM;
				sockfd_put(sock);
				goto out_unlock;
			}

			err = xp_assign_dev_shared(xs->pool, umem_xs->umem,
						   dev, qid);
			if (err) {
				xp_destroy(xs->pool);
				xs->pool = NULL;
				sockfd_put(sock);
				goto out_unlock;
			}
		} else {
			/* Share the buffer pool with the other socket. */
			if (xs->fq_tmp || xs->cq_tmp) {
				/* Do not allow setting your own fq or cq. */
				err = -EINVAL;
				sockfd_put(sock);
				goto out_unlock;
			}

			xp_get_pool(umem_xs->pool);
			xs->pool = umem_xs->pool;
		}

		xdp_get_umem(umem_xs->umem);
		WRITE_ONCE(xs->umem, umem_xs->umem);
		sockfd_put(sock);
	} else if (!xs->umem || !xsk_validate_queues(xs)) {
		err = -EINVAL;
		goto out_unlock;
	} else {
		/* This xsk has its own umem. */
		xs->pool = xp_create_and_assign_umem(xs, xs->umem);
		if (!xs->pool) {
			err = -ENOMEM;
			goto out_unlock;
		}

		err = xp_assign_dev(xs->pool, dev, qid, flags);
		if (err) {
			xp_destroy(xs->pool);
			xs->pool = NULL;
			goto out_unlock;
		}
	}

	/* FQ and CQ are now owned by the buffer pool and cleaned up with it. */
	xs->fq_tmp = NULL;
	xs->cq_tmp = NULL;

	xs->dev = dev;
	xs->zc = xs->umem->zc;
	xs->queue_id = qid;
	xp_add_xsk(xs->pool, xs);

out_unlock:
	if (err) {
		dev_put(dev);
	} else {
		/* Matches smp_rmb() in bind() for shared umem
		 * sockets, and xsk_is_bound().
		 */
		smp_wmb();
		WRITE_ONCE(xs->state, XSK_BOUND);
	}
out_release:
	mutex_unlock(&xs->mutex);
	rtnl_unlock();
	return err;
}

struct xdp_umem_reg_v1 {
	__u64 addr; /* Start of packet data area */
	__u64 len; /* Length of packet data area */
	__u32 chunk_size;
	__u32 headroom;
};

static int xsk_setsockopt(struct socket *sock, int level, int optname,
			  sockptr_t optval, unsigned int optlen)
{
	struct sock *sk = sock->sk;
	struct xdp_sock *xs = xdp_sk(sk);
	int err;

	if (level != SOL_XDP)
		return -ENOPROTOOPT;

	switch (optname) {
	case XDP_RX_RING:
	case XDP_TX_RING:
	{
		struct xsk_queue **q;
		int entries;

		if (optlen < sizeof(entries))
			return -EINVAL;
		if (copy_from_sockptr(&entries, optval, sizeof(entries)))
			return -EFAULT;

		mutex_lock(&xs->mutex);
		if (xs->state != XSK_READY) {
			mutex_unlock(&xs->mutex);
			return -EBUSY;
		}
		q = (optname == XDP_TX_RING) ? &xs->tx : &xs->rx;
		err = xsk_init_queue(entries, q, false);
		if (!err && optname == XDP_TX_RING)
			/* Tx needs to be explicitly woken up the first time */
			xs->tx->ring->flags |= XDP_RING_NEED_WAKEUP;
		mutex_unlock(&xs->mutex);
		return err;
	}
	case XDP_UMEM_REG:
	{
		size_t mr_size = sizeof(struct xdp_umem_reg);
		struct xdp_umem_reg mr = {};
		struct xdp_umem *umem;

		if (optlen < sizeof(struct xdp_umem_reg_v1))
			return -EINVAL;
		else if (optlen < sizeof(mr))
			mr_size = sizeof(struct xdp_umem_reg_v1);

		if (copy_from_sockptr(&mr, optval, mr_size))
			return -EFAULT;

		mutex_lock(&xs->mutex);
		if (xs->state != XSK_READY || xs->umem) {
			mutex_unlock(&xs->mutex);
			return -EBUSY;
		}

		umem = xdp_umem_create(&mr);
		if (IS_ERR(umem)) {
			mutex_unlock(&xs->mutex);
			return PTR_ERR(umem);
		}

		/* Make sure umem is ready before it can be seen by others */
		smp_wmb();
		WRITE_ONCE(xs->umem, umem);
		mutex_unlock(&xs->mutex);
		return 0;
	}
	case XDP_UMEM_FILL_RING:
	case XDP_UMEM_COMPLETION_RING:
	{
		struct xsk_queue **q;
		int entries;

		if (copy_from_sockptr(&entries, optval, sizeof(entries)))
			return -EFAULT;

		mutex_lock(&xs->mutex);
		if (xs->state != XSK_READY) {
			mutex_unlock(&xs->mutex);
			return -EBUSY;
		}

		q = (optname == XDP_UMEM_FILL_RING) ? &xs->fq_tmp :
			&xs->cq_tmp;
		err = xsk_init_queue(entries, q, true);
		mutex_unlock(&xs->mutex);
		return err;
	}
	default:
		break;
	}

	return -ENOPROTOOPT;
}

static void xsk_enter_rxtx_offsets(struct xdp_ring_offset_v1 *ring)
{
	ring->producer = offsetof(struct xdp_rxtx_ring, ptrs.producer);
	ring->consumer = offsetof(struct xdp_rxtx_ring, ptrs.consumer);
	ring->desc = offsetof(struct xdp_rxtx_ring, desc);
}

static void xsk_enter_umem_offsets(struct xdp_ring_offset_v1 *ring)
{
	ring->producer = offsetof(struct xdp_umem_ring, ptrs.producer);
	ring->consumer = offsetof(struct xdp_umem_ring, ptrs.consumer);
	ring->desc = offsetof(struct xdp_umem_ring, desc);
}

struct xdp_statistics_v1 {
	__u64 rx_dropped;
	__u64 rx_invalid_descs;
	__u64 tx_invalid_descs;
};

static int xsk_getsockopt(struct socket *sock, int level, int optname,
			  char __user *optval, int __user *optlen)
{
	struct sock *sk = sock->sk;
	struct xdp_sock *xs = xdp_sk(sk);
	int len;

	if (level != SOL_XDP)
		return -ENOPROTOOPT;

	if (get_user(len, optlen))
		return -EFAULT;
	if (len < 0)
		return -EINVAL;

	switch (optname) {
	case XDP_STATISTICS:
	{
		struct xdp_statistics stats = {};
		bool extra_stats = true;
		size_t stats_size;

		if (len < sizeof(struct xdp_statistics_v1)) {
			return -EINVAL;
		} else if (len < sizeof(stats)) {
			extra_stats = false;
			stats_size = sizeof(struct xdp_statistics_v1);
		} else {
			stats_size = sizeof(stats);
		}

		mutex_lock(&xs->mutex);
		stats.rx_dropped = xs->rx_dropped;
		if (extra_stats) {
			stats.rx_ring_full = xs->rx_queue_full;
			stats.rx_fill_ring_empty_descs =
				xs->pool ? xskq_nb_queue_empty_descs(xs->pool->fq) : 0;
			stats.tx_ring_empty_descs = xskq_nb_queue_empty_descs(xs->tx);
		} else {
			stats.rx_dropped += xs->rx_queue_full;
		}
		stats.rx_invalid_descs = xskq_nb_invalid_descs(xs->rx);
		stats.tx_invalid_descs = xskq_nb_invalid_descs(xs->tx);
		mutex_unlock(&xs->mutex);

		if (copy_to_user(optval, &stats, stats_size))
			return -EFAULT;
		if (put_user(stats_size, optlen))
			return -EFAULT;

		return 0;
	}
	case XDP_MMAP_OFFSETS:
	{
		struct xdp_mmap_offsets off;
		struct xdp_mmap_offsets_v1 off_v1;
		bool flags_supported = true;
		void *to_copy;

		if (len < sizeof(off_v1))
			return -EINVAL;
		else if (len < sizeof(off))
			flags_supported = false;

		if (flags_supported) {
			/* xdp_ring_offset is identical to xdp_ring_offset_v1
			 * except for the flags field added to the end.
			 */
			xsk_enter_rxtx_offsets((struct xdp_ring_offset_v1 *)
					       &off.rx);
			xsk_enter_rxtx_offsets((struct xdp_ring_offset_v1 *)
					       &off.tx);
			xsk_enter_umem_offsets((struct xdp_ring_offset_v1 *)
					       &off.fr);
			xsk_enter_umem_offsets((struct xdp_ring_offset_v1 *)
					       &off.cr);
			off.rx.flags = offsetof(struct xdp_rxtx_ring,
						ptrs.flags);
			off.tx.flags = offsetof(struct xdp_rxtx_ring,
						ptrs.flags);
			off.fr.flags = offsetof(struct xdp_umem_ring,
						ptrs.flags);
			off.cr.flags = offsetof(struct xdp_umem_ring,
						ptrs.flags);

			len = sizeof(off);
			to_copy = &off;
		} else {
			xsk_enter_rxtx_offsets(&off_v1.rx);
			xsk_enter_rxtx_offsets(&off_v1.tx);
			xsk_enter_umem_offsets(&off_v1.fr);
			xsk_enter_umem_offsets(&off_v1.cr);

			len = sizeof(off_v1);
			to_copy = &off_v1;
		}

		if (copy_to_user(optval, to_copy, len))
			return -EFAULT;
		if (put_user(len, optlen))
			return -EFAULT;

		return 0;
	}
	case XDP_OPTIONS:
	{
		struct xdp_options opts = {};

		if (len < sizeof(opts))
			return -EINVAL;

		mutex_lock(&xs->mutex);
		if (xs->zc)
			opts.flags |= XDP_OPTIONS_ZEROCOPY;
		mutex_unlock(&xs->mutex);

		len = sizeof(opts);
		if (copy_to_user(optval, &opts, len))
			return -EFAULT;
		if (put_user(len, optlen))
			return -EFAULT;

		return 0;
	}
	default:
		break;
	}

	return -EOPNOTSUPP;
}

static int xsk_mmap(struct file *file, struct socket *sock,
		    struct vm_area_struct *vma)
{
	loff_t offset = (loff_t)vma->vm_pgoff << PAGE_SHIFT;
	unsigned long size = vma->vm_end - vma->vm_start;
	struct xdp_sock *xs = xdp_sk(sock->sk);
	struct xsk_queue *q = NULL;
	unsigned long pfn;
	struct page *qpg;

	if (READ_ONCE(xs->state) != XSK_READY)
		return -EBUSY;

	if (offset == XDP_PGOFF_RX_RING) {
		q = READ_ONCE(xs->rx);
	} else if (offset == XDP_PGOFF_TX_RING) {
		q = READ_ONCE(xs->tx);
	} else {
		/* Matches the smp_wmb() in XDP_UMEM_REG */
		smp_rmb();
		if (offset == XDP_UMEM_PGOFF_FILL_RING)
			q = READ_ONCE(xs->fq_tmp);
		else if (offset == XDP_UMEM_PGOFF_COMPLETION_RING)
			q = READ_ONCE(xs->cq_tmp);
	}

	if (!q)
		return -EINVAL;

	/* Matches the smp_wmb() in xsk_init_queue */
	smp_rmb();
	qpg = virt_to_head_page(q->ring);
	if (size > page_size(qpg))
		return -EINVAL;

	pfn = virt_to_phys(q->ring) >> PAGE_SHIFT;
	return remap_pfn_range(vma, vma->vm_start, pfn,
			       size, vma->vm_page_prot);
}

static int xsk_notifier(struct notifier_block *this,
			unsigned long msg, void *ptr)
{
	struct net_device *dev = netdev_notifier_info_to_dev(ptr);
	struct net *net = dev_net(dev);
	struct sock *sk;

	switch (msg) {
	case NETDEV_UNREGISTER:
		mutex_lock(&net->xdp.lock);
		sk_for_each(sk, &net->xdp.list) {
			struct xdp_sock *xs = xdp_sk(sk);

			mutex_lock(&xs->mutex);
			if (xs->dev == dev) {
				sk->sk_err = ENETDOWN;
				if (!sock_flag(sk, SOCK_DEAD))
					sk->sk_error_report(sk);

				xsk_unbind_dev(xs);

				/* Clear device references. */
				xp_clear_dev(xs->pool);
			}
			mutex_unlock(&xs->mutex);
		}
		mutex_unlock(&net->xdp.lock);
		break;
	}
	return NOTIFY_DONE;
}

static struct proto xsk_proto = {
	.name =		"XDP",
	.owner =	THIS_MODULE,
	.obj_size =	sizeof(struct xdp_sock),
};

static const struct proto_ops xsk_proto_ops = {
	.family		= PF_XDP,
	.owner		= THIS_MODULE,
	.release	= xsk_release,
	.bind		= xsk_bind,
	.connect	= sock_no_connect,
	.socketpair	= sock_no_socketpair,
	.accept		= sock_no_accept,
	.getname	= sock_no_getname,
	.poll		= xsk_poll,
	.ioctl		= sock_no_ioctl,
	.listen		= sock_no_listen,
	.shutdown	= sock_no_shutdown,
	.setsockopt	= xsk_setsockopt,
	.getsockopt	= xsk_getsockopt,
	.sendmsg	= xsk_sendmsg,
	.recvmsg	= xsk_recvmsg,
	.mmap		= xsk_mmap,
	.sendpage	= sock_no_sendpage,
};

static void xsk_destruct(struct sock *sk)
{
	struct xdp_sock *xs = xdp_sk(sk);

	if (!sock_flag(sk, SOCK_DEAD))
		return;

	if (!xp_put_pool(xs->pool))
		xdp_put_umem(xs->umem, !xs->pool);

	sk_refcnt_debug_dec(sk);
}

static int xsk_create(struct net *net, struct socket *sock, int protocol,
		      int kern)
{
	struct xdp_sock *xs;
	struct sock *sk;

	if (!ns_capable(net->user_ns, CAP_NET_RAW))
		return -EPERM;
	if (sock->type != SOCK_RAW)
		return -ESOCKTNOSUPPORT;

	if (protocol)
		return -EPROTONOSUPPORT;

	sock->state = SS_UNCONNECTED;

	sk = sk_alloc(net, PF_XDP, GFP_KERNEL, &xsk_proto, kern);
	if (!sk)
		return -ENOBUFS;

	sock->ops = &xsk_proto_ops;

	sock_init_data(sock, sk);

	sk->sk_family = PF_XDP;

	sk->sk_destruct = xsk_destruct;
	sk_refcnt_debug_inc(sk);

	sock_set_flag(sk, SOCK_RCU_FREE);

	xs = xdp_sk(sk);
	xs->state = XSK_READY;
	mutex_init(&xs->mutex);
	spin_lock_init(&xs->rx_lock);

	INIT_LIST_HEAD(&xs->map_list);
	spin_lock_init(&xs->map_list_lock);

	mutex_lock(&net->xdp.lock);
	sk_add_node_rcu(sk, &net->xdp.list);
	mutex_unlock(&net->xdp.lock);

	local_bh_disable();
	sock_prot_inuse_add(net, &xsk_proto, 1);
	local_bh_enable();

	return 0;
}

static const struct net_proto_family xsk_family_ops = {
	.family = PF_XDP,
	.create = xsk_create,
	.owner	= THIS_MODULE,
};

static struct notifier_block xsk_netdev_notifier = {
	.notifier_call	= xsk_notifier,
};

static int __net_init xsk_net_init(struct net *net)
{
	mutex_init(&net->xdp.lock);
	INIT_HLIST_HEAD(&net->xdp.list);
	return 0;
}

static void __net_exit xsk_net_exit(struct net *net)
{
	WARN_ON_ONCE(!hlist_empty(&net->xdp.list));
}

static struct pernet_operations xsk_net_ops = {
	.init = xsk_net_init,
	.exit = xsk_net_exit,
};

static int __init xsk_init(void)
{
	int err, cpu;

	err = proto_register(&xsk_proto, 0 /* no slab */);
	if (err)
		goto out;

	err = sock_register(&xsk_family_ops);
	if (err)
		goto out_proto;

	err = register_pernet_subsys(&xsk_net_ops);
	if (err)
		goto out_sk;

	err = register_netdevice_notifier(&xsk_netdev_notifier);
	if (err)
		goto out_pernet;

	for_each_possible_cpu(cpu)
		INIT_LIST_HEAD(&per_cpu(xskmap_flush_list, cpu));
	return 0;

out_pernet:
	unregister_pernet_subsys(&xsk_net_ops);
out_sk:
	sock_unregister(PF_XDP);
out_proto:
	proto_unregister(&xsk_proto);
out:
	return err;
}

fs_initcall(xsk_init);
