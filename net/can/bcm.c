// SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause)
/*
 * bcm.c - Broadcast Manager to filter/send (cyclic) CAN content
 *
 * Copyright (c) 2002-2017 Volkswagen Group Electronic Research
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of Volkswagen nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * Alternatively, provided that this notice is retained in full, this
 * software may be distributed under the terms of the GNU General
 * Public License ("GPL") version 2, in which case the provisions of the
 * GPL apply INSTEAD OF those given above.
 *
 * The provided data structures and external interfaces from this code
 * are not restricted to be used by modules with a GPL compatible license.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 *
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/hrtimer.h>
#include <linux/list.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uio.h>
#include <linux/net.h>
#include <linux/netdevice.h>
#include <linux/socket.h>
#include <linux/if_arp.h>
#include <linux/skbuff.h>
#include <linux/can.h>
#include <linux/can/core.h>
#include <linux/can/skb.h>
#include <linux/can/bcm.h>
#include <linux/slab.h>
#include <net/sock.h>
#include <net/net_namespace.h>

/*
 * To send multiple CAN frame content within TX_SETUP or to filter
 * CAN messages with multiplex index within RX_SETUP, the number of
 * different filters is limited to 256 due to the one byte index value.
 */
#define MAX_NFRAMES 256

/* limit timers to 400 days for sending/timeouts */
#define BCM_TIMER_SEC_MAX (400 * 24 * 60 * 60)

/* use of last_frames[index].flags */
#define RX_RECV    0x40 /* received data for this element */
#define RX_THR     0x80 /* element not been sent due to throttle feature */
#define BCM_CAN_FLAGS_MASK 0x3F /* to clean private flags after usage */

/* get best masking value for can_rx_register() for a given single can_id */
#define REGMASK(id) ((id & CAN_EFF_FLAG) ? \
		     (CAN_EFF_MASK | CAN_EFF_FLAG | CAN_RTR_FLAG) : \
		     (CAN_SFF_MASK | CAN_EFF_FLAG | CAN_RTR_FLAG))

MODULE_DESCRIPTION("PF_CAN broadcast manager protocol");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Oliver Hartkopp <oliver.hartkopp@volkswagen.de>");
MODULE_ALIAS("can-proto-2");

#define BCM_MIN_NAMELEN CAN_REQUIRED_SIZE(struct sockaddr_can, can_ifindex)

/*
 * easy access to the first 64 bit of can(fd)_frame payload. cp->data is
 * 64 bit aligned so the offset has to be multiples of 8 which is ensured
 * by the only callers in bcm_rx_cmp_to_index() bcm_rx_handler().
 */
static inline u64 get_u64(const struct canfd_frame *cp, int offset)
{
	return *(u64 *)(cp->data + offset);
}

struct bcm_op {
	struct list_head list;
	int ifindex;
	canid_t can_id;
	u32 flags;
	unsigned long frames_abs, frames_filtered;
	struct bcm_timeval ival1, ival2;
	struct hrtimer timer, thrtimer;
	ktime_t rx_stamp, kt_ival1, kt_ival2, kt_lastmsg;
	int rx_ifindex;
	int cfsiz;
	u32 count;
	u32 nframes;
	u32 currframe;
	/* void pointers to arrays of struct can[fd]_frame */
	void *frames;
	void *last_frames;
	struct canfd_frame sframe;
	struct canfd_frame last_sframe;
	struct sock *sk;
	struct net_device *rx_reg_dev;
};

struct bcm_sock {
	struct sock sk;
	int bound;
	int ifindex;
	struct list_head notifier;
	struct list_head rx_ops;
	struct list_head tx_ops;
	unsigned long dropped_usr_msgs;
	struct proc_dir_entry *bcm_proc_read;
	char procname [32]; /* inode number in decimal with \0 */
};

static LIST_HEAD(bcm_notifier_list);
static DEFINE_SPINLOCK(bcm_notifier_lock);
static struct bcm_sock *bcm_busy_notifier;

static inline struct bcm_sock *bcm_sk(const struct sock *sk)
{
	return (struct bcm_sock *)sk;
}

static inline ktime_t bcm_timeval_to_ktime(struct bcm_timeval tv)
{
	return ktime_set(tv.tv_sec, tv.tv_usec * NSEC_PER_USEC);
}

/* check limitations for timeval provided by user */
static bool bcm_is_invalid_tv(struct bcm_msg_head *msg_head)
{
	if ((msg_head->ival1.tv_sec < 0) ||
	    (msg_head->ival1.tv_sec > BCM_TIMER_SEC_MAX) ||
	    (msg_head->ival1.tv_usec < 0) ||
	    (msg_head->ival1.tv_usec >= USEC_PER_SEC) ||
	    (msg_head->ival2.tv_sec < 0) ||
	    (msg_head->ival2.tv_sec > BCM_TIMER_SEC_MAX) ||
	    (msg_head->ival2.tv_usec < 0) ||
	    (msg_head->ival2.tv_usec >= USEC_PER_SEC))
		return true;

	return false;
}

#define CFSIZ(flags) ((flags & CAN_FD_FRAME) ? CANFD_MTU : CAN_MTU)
#define OPSIZ sizeof(struct bcm_op)
#define MHSIZ sizeof(struct bcm_msg_head)

/*
 * procfs functions
 */
#if IS_ENABLED(CONFIG_PROC_FS)
static char *bcm_proc_getifname(struct net *net, char *result, int ifindex)
{
	struct net_device *dev;

	if (!ifindex)
		return "any";

	rcu_read_lock();
	dev = dev_get_by_index_rcu(net, ifindex);
	if (dev)
		strcpy(result, dev->name);
	else
		strcpy(result, "???");
	rcu_read_unlock();

	return result;
}

static int bcm_proc_show(struct seq_file *m, void *v)
{
	char ifname[IFNAMSIZ];
	struct net *net = m->private;
	struct sock *sk = (struct sock *)PDE_DATA(m->file->f_inode);
	struct bcm_sock *bo = bcm_sk(sk);
	struct bcm_op *op;

	seq_printf(m, ">>> socket %pK", sk->sk_socket);
	seq_printf(m, " / sk %pK", sk);
	seq_printf(m, " / bo %pK", bo);
	seq_printf(m, " / dropped %lu", bo->dropped_usr_msgs);
	seq_printf(m, " / bound %s", bcm_proc_getifname(net, ifname, bo->ifindex));
	seq_printf(m, " <<<\n");

	list_for_each_entry(op, &bo->rx_ops, list) {

		unsigned long reduction;

		/* print only active entries & prevent division by zero */
		if (!op->frames_abs)
			continue;

		seq_printf(m, "rx_op: %03X %-5s ", op->can_id,
			   bcm_proc_getifname(net, ifname, op->ifindex));

		if (op->flags & CAN_FD_FRAME)
			seq_printf(m, "(%u)", op->nframes);
		else
			seq_printf(m, "[%u]", op->nframes);

		seq_printf(m, "%c ", (op->flags & RX_CHECK_DLC) ? 'd' : ' ');

		if (op->kt_ival1)
			seq_printf(m, "timeo=%lld ",
				   (long long)ktime_to_us(op->kt_ival1));

		if (op->kt_ival2)
			seq_printf(m, "thr=%lld ",
				   (long long)ktime_to_us(op->kt_ival2));

		seq_printf(m, "# recv %ld (%ld) => reduction: ",
			   op->frames_filtered, op->frames_abs);

		reduction = 100 - (op->frames_filtered * 100) / op->frames_abs;

		seq_printf(m, "%s%ld%%\n",
			   (reduction == 100) ? "near " : "", reduction);
	}

	list_for_each_entry(op, &bo->tx_ops, list) {

		seq_printf(m, "tx_op: %03X %s ", op->can_id,
			   bcm_proc_getifname(net, ifname, op->ifindex));

		if (op->flags & CAN_FD_FRAME)
			seq_printf(m, "(%u) ", op->nframes);
		else
			seq_printf(m, "[%u] ", op->nframes);

		if (op->kt_ival1)
			seq_printf(m, "t1=%lld ",
				   (long long)ktime_to_us(op->kt_ival1));

		if (op->kt_ival2)
			seq_printf(m, "t2=%lld ",
				   (long long)ktime_to_us(op->kt_ival2));

		seq_printf(m, "# sent %ld\n", op->frames_abs);
	}
	seq_putc(m, '\n');
	return 0;
}
#endif /* CONFIG_PROC_FS */

/*
 * bcm_can_tx - send the (next) CAN frame to the appropriate CAN interface
 *              of the given bcm tx op
 */
static void bcm_can_tx(struct bcm_op *op)
{
	struct sk_buff *skb;
	struct net_device *dev;
	struct canfd_frame *cf = op->frames + op->cfsiz * op->currframe;

	/* no target device? => exit */
	if (!op->ifindex)
		return;

	dev = dev_get_by_index(sock_net(op->sk), op->ifindex);
	if (!dev) {
		/* RFC: should this bcm_op remove itself here? */
		return;
	}

	skb = alloc_skb(op->cfsiz + sizeof(struct can_skb_priv), gfp_any());
	if (!skb)
		goto out;

	can_skb_reserve(skb);
	can_skb_prv(skb)->ifindex = dev->ifindex;
	can_skb_prv(skb)->skbcnt = 0;

	skb_put_data(skb, cf, op->cfsiz);

	/* send with loopback */
	skb->dev = dev;
	can_skb_set_owner(skb, op->sk);
	can_send(skb, 1);

	/* update statistics */
	op->currframe++;
	op->frames_abs++;

	/* reached last frame? */
	if (op->currframe >= op->nframes)
		op->currframe = 0;
out:
	dev_put(dev);
}

/*
 * bcm_send_to_user - send a BCM message to the userspace
 *                    (consisting of bcm_msg_head + x CAN frames)
 */
static void bcm_send_to_user(struct bcm_op *op, struct bcm_msg_head *head,
			     struct canfd_frame *frames, int has_timestamp)
{
	struct sk_buff *skb;
	struct canfd_frame *firstframe;
	struct sockaddr_can *addr;
	struct sock *sk = op->sk;
	unsigned int datalen = head->nframes * op->cfsiz;
	int err;

	skb = alloc_skb(sizeof(*head) + datalen, gfp_any());
	if (!skb)
		return;

	skb_put_data(skb, head, sizeof(*head));

	if (head->nframes) {
		/* CAN frames starting here */
		firstframe = (struct canfd_frame *)skb_tail_pointer(skb);

		skb_put_data(skb, frames, datalen);

		/*
		 * the BCM uses the flags-element of the canfd_frame
		 * structure for internal purposes. This is only
		 * relevant for updates that are generated by the
		 * BCM, where nframes is 1
		 */
		if (head->nframes == 1)
			firstframe->flags &= BCM_CAN_FLAGS_MASK;
	}

	if (has_timestamp) {
		/* restore rx timestamp */
		skb->tstamp = op->rx_stamp;
	}

	/*
	 *  Put the datagram to the queue so that bcm_recvmsg() can
	 *  get it from there.  We need to pass the interface index to
	 *  bcm_recvmsg().  We pass a whole struct sockaddr_can in skb->cb
	 *  containing the interface index.
	 */

	sock_skb_cb_check_size(sizeof(struct sockaddr_can));
	addr = (struct sockaddr_can *)skb->cb;
	memset(addr, 0, sizeof(*addr));
	addr->can_family  = AF_CAN;
	addr->can_ifindex = op->rx_ifindex;

	err = sock_queue_rcv_skb(sk, skb);
	if (err < 0) {
		struct bcm_sock *bo = bcm_sk(sk);

		kfree_skb(skb);
		/* don't care about overflows in this statistic */
		bo->dropped_usr_msgs++;
	}
}

static bool bcm_tx_set_expiry(struct bcm_op *op, struct hrtimer *hrt)
{
	ktime_t ival;

	if (op->kt_ival1 && op->count)
		ival = op->kt_ival1;
	else if (op->kt_ival2)
		ival = op->kt_ival2;
	else
		return false;

	hrtimer_set_expires(hrt, ktime_add(ktime_get(), ival));
	return true;
}

static void bcm_tx_start_timer(struct bcm_op *op)
{
	if (bcm_tx_set_expiry(op, &op->timer))
		hrtimer_start_expires(&op->timer, HRTIMER_MODE_ABS_SOFT);
}

/* bcm_tx_timeout_handler - performs cyclic CAN frame transmissions */
static enum hrtimer_restart bcm_tx_timeout_handler(struct hrtimer *hrtimer)
{
	struct bcm_op *op = container_of(hrtimer, struct bcm_op, timer);
	struct bcm_msg_head msg_head;

	if (op->kt_ival1 && (op->count > 0)) {
		op->count--;
		if (!op->count && (op->flags & TX_COUNTEVT)) {

			/* create notification to user */
			memset(&msg_head, 0, sizeof(msg_head));
			msg_head.opcode  = TX_EXPIRED;
			msg_head.flags   = op->flags;
			msg_head.count   = op->count;
			msg_head.ival1   = op->ival1;
			msg_head.ival2   = op->ival2;
			msg_head.can_id  = op->can_id;
			msg_head.nframes = 0;

			bcm_send_to_user(op, &msg_head, NULL, 0);
		}
		bcm_can_tx(op);

	} else if (op->kt_ival2) {
		bcm_can_tx(op);
	}

	return bcm_tx_set_expiry(op, &op->timer) ?
		HRTIMER_RESTART : HRTIMER_NORESTART;
}

/*
 * bcm_rx_changed - create a RX_CHANGED notification due to changed content
 */
static void bcm_rx_changed(struct bcm_op *op, struct canfd_frame *data)
{
	struct bcm_msg_head head;

	/* update statistics */
	op->frames_filtered++;

	/* prevent statistics overflow */
	if (op->frames_filtered > ULONG_MAX/100)
		op->frames_filtered = op->frames_abs = 0;

	/* this element is not throttled anymore */
	data->flags &= (BCM_CAN_FLAGS_MASK|RX_RECV);

	memset(&head, 0, sizeof(head));
	head.opcode  = RX_CHANGED;
	head.flags   = op->flags;
	head.count   = op->count;
	head.ival1   = op->ival1;
	head.ival2   = op->ival2;
	head.can_id  = op->can_id;
	head.nframes = 1;

	bcm_send_to_user(op, &head, data, 1);
}

/*
 * bcm_rx_update_and_send - process a detected relevant receive content change
 *                          1. update the last received data
 *                          2. send a notification to the user (if possible)
 */
static void bcm_rx_update_and_send(struct bcm_op *op,
				   struct canfd_frame *lastdata,
				   const struct canfd_frame *rxdata)
{
	memcpy(lastdata, rxdata, op->cfsiz);

	/* mark as used and throttled by default */
	lastdata->flags |= (RX_RECV|RX_THR);

	/* throttling mode inactive ? */
	if (!op->kt_ival2) {
		/* send RX_CHANGED to the user immediately */
		bcm_rx_changed(op, lastdata);
		return;
	}

	/* with active throttling timer we are just done here */
	if (hrtimer_active(&op->thrtimer))
		return;

	/* first reception with enabled throttling mode */
	if (!op->kt_lastmsg)
		goto rx_changed_settime;

	/* got a second frame inside a potential throttle period? */
	if (ktime_us_delta(ktime_get(), op->kt_lastmsg) <
	    ktime_to_us(op->kt_ival2)) {
		/* do not send the saved data - only start throttle timer */
		hrtimer_start(&op->thrtimer,
			      ktime_add(op->kt_lastmsg, op->kt_ival2),
			      HRTIMER_MODE_ABS_SOFT);
		return;
	}

	/* the gap was that big, that throttling was not needed here */
rx_changed_settime:
	bcm_rx_changed(op, lastdata);
	op->kt_lastmsg = ktime_get();
}

/*
 * bcm_rx_cmp_to_index - (bit)compares the currently received data to formerly
 *                       received data stored in op->last_frames[]
 */
static void bcm_rx_cmp_to_index(struct bcm_op *op, unsigned int index,
				const struct canfd_frame *rxdata)
{
	struct canfd_frame *cf = op->frames + op->cfsiz * index;
	struct canfd_frame *lcf = op->last_frames + op->cfsiz * index;
	int i;

	/*
	 * no one uses the MSBs of flags for comparison,
	 * so we use it here to detect the first time of reception
	 */

	if (!(lcf->flags & RX_RECV)) {
		/* received data for the first time => send update to user */
		bcm_rx_update_and_send(op, lcf, rxdata);
		return;
	}

	/* do a real check in CAN frame data section */
	for (i = 0; i < rxdata->len; i += 8) {
		if ((get_u64(cf, i) & get_u64(rxdata, i)) !=
		    (get_u64(cf, i) & get_u64(lcf, i))) {
			bcm_rx_update_and_send(op, lcf, rxdata);
			return;
		}
	}

	if (op->flags & RX_CHECK_DLC) {
		/* do a real check in CAN frame length */
		if (rxdata->len != lcf->len) {
			bcm_rx_update_and_send(op, lcf, rxdata);
			return;
		}
	}
}

/*
 * bcm_rx_starttimer - enable timeout monitoring for CAN frame reception
 */
static void bcm_rx_starttimer(struct bcm_op *op)
{
	if (op->flags & RX_NO_AUTOTIMER)
		return;

	if (op->kt_ival1)
		hrtimer_start(&op->timer, op->kt_ival1, HRTIMER_MODE_REL_SOFT);
}

/* bcm_rx_timeout_handler - when the (cyclic) CAN frame reception timed out */
static enum hrtimer_restart bcm_rx_timeout_handler(struct hrtimer *hrtimer)
{
	struct bcm_op *op = container_of(hrtimer, struct bcm_op, timer);
	struct bcm_msg_head msg_head;

	/* if user wants to be informed, when cyclic CAN-Messages come back */
	if ((op->flags & RX_ANNOUNCE_RESUME) && op->last_frames) {
		/* clear received CAN frames to indicate 'nothing received' */
		memset(op->last_frames, 0, op->nframes * op->cfsiz);
	}

	/* create notification to user */
	memset(&msg_head, 0, sizeof(msg_head));
	msg_head.opcode  = RX_TIMEOUT;
	msg_head.flags   = op->flags;
	msg_head.count   = op->count;
	msg_head.ival1   = op->ival1;
	msg_head.ival2   = op->ival2;
	msg_head.can_id  = op->can_id;
	msg_head.nframes = 0;

	bcm_send_to_user(op, &msg_head, NULL, 0);

	return HRTIMER_NORESTART;
}

/*
 * bcm_rx_do_flush - helper for bcm_rx_thr_flush
 */
static inline int bcm_rx_do_flush(struct bcm_op *op, unsigned int index)
{
	struct canfd_frame *lcf = op->last_frames + op->cfsiz * index;

	if ((op->last_frames) && (lcf->flags & RX_THR)) {
		bcm_rx_changed(op, lcf);
		return 1;
	}
	return 0;
}

/*
 * bcm_rx_thr_flush - Check for throttled data and send it to the userspace
 */
static int bcm_rx_thr_flush(struct bcm_op *op)
{
	int updated = 0;

	if (op->nframes > 1) {
		unsigned int i;

		/* for MUX filter we start at index 1 */
		for (i = 1; i < op->nframes; i++)
			updated += bcm_rx_do_flush(op, i);

	} else {
		/* for RX_FILTER_ID and simple filter */
		updated += bcm_rx_do_flush(op, 0);
	}

	return updated;
}

/*
 * bcm_rx_thr_handler - the time for blocked content updates is over now:
 *                      Check for throttled data and send it to the userspace
 */
static enum hrtimer_restart bcm_rx_thr_handler(struct hrtimer *hrtimer)
{
	struct bcm_op *op = container_of(hrtimer, struct bcm_op, thrtimer);

	if (bcm_rx_thr_flush(op)) {
		hrtimer_forward(hrtimer, ktime_get(), op->kt_ival2);
		return HRTIMER_RESTART;
	} else {
		/* rearm throttle handling */
		op->kt_lastmsg = 0;
		return HRTIMER_NORESTART;
	}
}

/*
 * bcm_rx_handler - handle a CAN frame reception
 */
static void bcm_rx_handler(struct sk_buff *skb, void *data)
{
	struct bcm_op *op = (struct bcm_op *)data;
	const struct canfd_frame *rxframe = (struct canfd_frame *)skb->data;
	unsigned int i;

	if (op->can_id != rxframe->can_id)
		return;

	/* make sure to handle the correct frame type (CAN / CAN FD) */
	if (skb->len != op->cfsiz)
		return;

	/* disable timeout */
	hrtimer_cancel(&op->timer);

	/* save rx timestamp */
	op->rx_stamp = skb->tstamp;
	/* save originator for recvfrom() */
	op->rx_ifindex = skb->dev->ifindex;
	/* update statistics */
	op->frames_abs++;

	if (op->flags & RX_RTR_FRAME) {
		/* send reply for RTR-request (placed in op->frames[0]) */
		bcm_can_tx(op);
		return;
	}

	if (op->flags & RX_FILTER_ID) {
		/* the easiest case */
		bcm_rx_update_and_send(op, op->last_frames, rxframe);
		goto rx_starttimer;
	}

	if (op->nframes == 1) {
		/* simple compare with index 0 */
		bcm_rx_cmp_to_index(op, 0, rxframe);
		goto rx_starttimer;
	}

	if (op->nframes > 1) {
		/*
		 * multiplex compare
		 *
		 * find the first multiplex mask that fits.
		 * Remark: The MUX-mask is stored in index 0 - but only the
		 * first 64 bits of the frame data[] are relevant (CAN FD)
		 */

		for (i = 1; i < op->nframes; i++) {
			if ((get_u64(op->frames, 0) & get_u64(rxframe, 0)) ==
			    (get_u64(op->frames, 0) &
			     get_u64(op->frames + op->cfsiz * i, 0))) {
				bcm_rx_cmp_to_index(op, i, rxframe);
				break;
			}
		}
	}

rx_starttimer:
	bcm_rx_starttimer(op);
}

/*
 * helpers for bcm_op handling: find & delete bcm [rx|tx] op elements
 */
static struct bcm_op *bcm_find_op(struct list_head *ops,
				  struct bcm_msg_head *mh, int ifindex)
{
	struct bcm_op *op;

	list_for_each_entry(op, ops, list) {
		if ((op->can_id == mh->can_id) && (op->ifindex == ifindex) &&
		    (op->flags & CAN_FD_FRAME) == (mh->flags & CAN_FD_FRAME))
			return op;
	}

	return NULL;
}

static void bcm_remove_op(struct bcm_op *op)
{
	hrtimer_cancel(&op->timer);
	hrtimer_cancel(&op->thrtimer);

	if ((op->frames) && (op->frames != &op->sframe))
		kfree(op->frames);

	if ((op->last_frames) && (op->last_frames != &op->last_sframe))
		kfree(op->last_frames);

	kfree(op);
}

static void bcm_rx_unreg(struct net_device *dev, struct bcm_op *op)
{
	if (op->rx_reg_dev == dev) {
		can_rx_unregister(dev_net(dev), dev, op->can_id,
				  REGMASK(op->can_id), bcm_rx_handler, op);

		/* mark as removed subscription */
		op->rx_reg_dev = NULL;
	} else
		printk(KERN_ERR "can-bcm: bcm_rx_unreg: registered device "
		       "mismatch %p %p\n", op->rx_reg_dev, dev);
}

/*
 * bcm_delete_rx_op - find and remove a rx op (returns number of removed ops)
 */
static int bcm_delete_rx_op(struct list_head *ops, struct bcm_msg_head *mh,
			    int ifindex)
{
	struct bcm_op *op, *n;

	list_for_each_entry_safe(op, n, ops, list) {
		if ((op->can_id == mh->can_id) && (op->ifindex == ifindex) &&
		    (op->flags & CAN_FD_FRAME) == (mh->flags & CAN_FD_FRAME)) {

			/*
			 * Don't care if we're bound or not (due to netdev
			 * problems) can_rx_unregister() is always a save
			 * thing to do here.
			 */
			if (op->ifindex) {
				/*
				 * Only remove subscriptions that had not
				 * been removed due to NETDEV_UNREGISTER
				 * in bcm_notifier()
				 */
				if (op->rx_reg_dev) {
					struct net_device *dev;

					dev = dev_get_by_index(sock_net(op->sk),
							       op->ifindex);
					if (dev) {
						bcm_rx_unreg(dev, op);
						dev_put(dev);
					}
				}
			} else
				can_rx_unregister(sock_net(op->sk), NULL,
						  op->can_id,
						  REGMASK(op->can_id),
						  bcm_rx_handler, op);

			list_del(&op->list);
			bcm_remove_op(op);
			return 1; /* done */
		}
	}

	return 0; /* not found */
}

/*
 * bcm_delete_tx_op - find and remove a tx op (returns number of removed ops)
 */
static int bcm_delete_tx_op(struct list_head *ops, struct bcm_msg_head *mh,
			    int ifindex)
{
	struct bcm_op *op, *n;

	list_for_each_entry_safe(op, n, ops, list) {
		if ((op->can_id == mh->can_id) && (op->ifindex == ifindex) &&
		    (op->flags & CAN_FD_FRAME) == (mh->flags & CAN_FD_FRAME)) {
			list_del(&op->list);
			bcm_remove_op(op);
			return 1; /* done */
		}
	}

	return 0; /* not found */
}

/*
 * bcm_read_op - read out a bcm_op and send it to the user (for bcm_sendmsg)
 */
static int bcm_read_op(struct list_head *ops, struct bcm_msg_head *msg_head,
		       int ifindex)
{
	struct bcm_op *op = bcm_find_op(ops, msg_head, ifindex);

	if (!op)
		return -EINVAL;

	/* put current values into msg_head */
	msg_head->flags   = op->flags;
	msg_head->count   = op->count;
	msg_head->ival1   = op->ival1;
	msg_head->ival2   = op->ival2;
	msg_head->nframes = op->nframes;

	bcm_send_to_user(op, msg_head, op->frames, 0);

	return MHSIZ;
}

/*
 * bcm_tx_setup - create or update a bcm tx op (for bcm_sendmsg)
 */
static int bcm_tx_setup(struct bcm_msg_head *msg_head, struct msghdr *msg,
			int ifindex, struct sock *sk)
{
	struct bcm_sock *bo = bcm_sk(sk);
	struct bcm_op *op;
	struct canfd_frame *cf;
	unsigned int i;
	int err;

	/* we need a real device to send frames */
	if (!ifindex)
		return -ENODEV;

	/* check nframes boundaries - we need at least one CAN frame */
	if (msg_head->nframes < 1 || msg_head->nframes > MAX_NFRAMES)
		return -EINVAL;

	/* check timeval limitations */
	if ((msg_head->flags & SETTIMER) && bcm_is_invalid_tv(msg_head))
		return -EINVAL;

	/* check the given can_id */
	op = bcm_find_op(&bo->tx_ops, msg_head, ifindex);
	if (op) {
		/* update existing BCM operation */

		/*
		 * Do we need more space for the CAN frames than currently
		 * allocated? -> This is a _really_ unusual use-case and
		 * therefore (complexity / locking) it is not supported.
		 */
		if (msg_head->nframes > op->nframes)
			return -E2BIG;

		/* update CAN frames content */
		for (i = 0; i < msg_head->nframes; i++) {

			cf = op->frames + op->cfsiz * i;
			err = memcpy_from_msg((u8 *)cf, msg, op->cfsiz);

			if (op->flags & CAN_FD_FRAME) {
				if (cf->len > 64)
					err = -EINVAL;
			} else {
				if (cf->len > 8)
					err = -EINVAL;
			}

			if (err < 0)
				return err;

			if (msg_head->flags & TX_CP_CAN_ID) {
				/* copy can_id into frame */
				cf->can_id = msg_head->can_id;
			}
		}
		op->flags = msg_head->flags;

	} else {
		/* insert new BCM operation for the given can_id */

		op = kzalloc(OPSIZ, GFP_KERNEL);
		if (!op)
			return -ENOMEM;

		op->can_id = msg_head->can_id;
		op->cfsiz = CFSIZ(msg_head->flags);
		op->flags = msg_head->flags;

		/* create array for CAN frames and copy the data */
		if (msg_head->nframes > 1) {
			op->frames = kmalloc_array(msg_head->nframes,
						   op->cfsiz,
						   GFP_KERNEL);
			if (!op->frames) {
				kfree(op);
				return -ENOMEM;
			}
		} else
			op->frames = &op->sframe;

		for (i = 0; i < msg_head->nframes; i++) {

			cf = op->frames + op->cfsiz * i;
			err = memcpy_from_msg((u8 *)cf, msg, op->cfsiz);

			if (op->flags & CAN_FD_FRAME) {
				if (cf->len > 64)
					err = -EINVAL;
			} else {
				if (cf->len > 8)
					err = -EINVAL;
			}

			if (err < 0) {
				if (op->frames != &op->sframe)
					kfree(op->frames);
				kfree(op);
				return err;
			}

			if (msg_head->flags & TX_CP_CAN_ID) {
				/* copy can_id into frame */
				cf->can_id = msg_head->can_id;
			}
		}

		/* tx_ops never compare with previous received messages */
		op->last_frames = NULL;

		/* bcm_can_tx / bcm_tx_timeout_handler needs this */
		op->sk = sk;
		op->ifindex = ifindex;

		/* initialize uninitialized (kzalloc) structure */
		hrtimer_init(&op->timer, CLOCK_MONOTONIC,
			     HRTIMER_MODE_REL_SOFT);
		op->timer.function = bcm_tx_timeout_handler;

		/* currently unused in tx_ops */
		hrtimer_init(&op->thrtimer, CLOCK_MONOTONIC,
			     HRTIMER_MODE_REL_SOFT);

		/* add this bcm_op to the list of the tx_ops */
		list_add(&op->list, &bo->tx_ops);

	} /* if ((op = bcm_find_op(&bo->tx_ops, msg_head->can_id, ifindex))) */

	if (op->nframes != msg_head->nframes) {
		op->nframes   = msg_head->nframes;
		/* start multiple frame transmission with index 0 */
		op->currframe = 0;
	}

	/* check flags */

	if (op->flags & TX_RESET_MULTI_IDX) {
		/* start multiple frame transmission with index 0 */
		op->currframe = 0;
	}

	if (op->flags & SETTIMER) {
		/* set timer values */
		op->count = msg_head->count;
		op->ival1 = msg_head->ival1;
		op->ival2 = msg_head->ival2;
		op->kt_ival1 = bcm_timeval_to_ktime(msg_head->ival1);
		op->kt_ival2 = bcm_timeval_to_ktime(msg_head->ival2);

		/* disable an active timer due to zero values? */
		if (!op->kt_ival1 && !op->kt_ival2)
			hrtimer_cancel(&op->timer);
	}

	if (op->flags & STARTTIMER) {
		hrtimer_cancel(&op->timer);
		/* spec: send CAN frame when starting timer */
		op->flags |= TX_ANNOUNCE;
	}

	if (op->flags & TX_ANNOUNCE) {
		bcm_can_tx(op);
		if (op->count)
			op->count--;
	}

	if (op->flags & STARTTIMER)
		bcm_tx_start_timer(op);

	return msg_head->nframes * op->cfsiz + MHSIZ;
}

/*
 * bcm_rx_setup - create or update a bcm rx op (for bcm_sendmsg)
 */
static int bcm_rx_setup(struct bcm_msg_head *msg_head, struct msghdr *msg,
			int ifindex, struct sock *sk)
{
	struct bcm_sock *bo = bcm_sk(sk);
	struct bcm_op *op;
	int do_rx_register;
	int err = 0;

	if ((msg_head->flags & RX_FILTER_ID) || (!(msg_head->nframes))) {
		/* be robust against wrong usage ... */
		msg_head->flags |= RX_FILTER_ID;
		/* ignore trailing garbage */
		msg_head->nframes = 0;
	}

	/* the first element contains the mux-mask => MAX_NFRAMES + 1  */
	if (msg_head->nframes > MAX_NFRAMES + 1)
		return -EINVAL;

	if ((msg_head->flags & RX_RTR_FRAME) &&
	    ((msg_head->nframes != 1) ||
	     (!(msg_head->can_id & CAN_RTR_FLAG))))
		return -EINVAL;

	/* check timeval limitations */
	if ((msg_head->flags & SETTIMER) && bcm_is_invalid_tv(msg_head))
		return -EINVAL;

	/* check the given can_id */
	op = bcm_find_op(&bo->rx_ops, msg_head, ifindex);
	if (op) {
		/* update existing BCM operation */

		/*
		 * Do we need more space for the CAN frames than currently
		 * allocated? -> This is a _really_ unusual use-case and
		 * therefore (complexity / locking) it is not supported.
		 */
		if (msg_head->nframes > op->nframes)
			return -E2BIG;

		if (msg_head->nframes) {
			/* update CAN frames content */
			err = memcpy_from_msg(op->frames, msg,
					      msg_head->nframes * op->cfsiz);
			if (err < 0)
				return err;

			/* clear last_frames to indicate 'nothing received' */
			memset(op->last_frames, 0, msg_head->nframes * op->cfsiz);
		}

		op->nframes = msg_head->nframes;
		op->flags = msg_head->flags;

		/* Only an update -> do not call can_rx_register() */
		do_rx_register = 0;

	} else {
		/* insert new BCM operation for the given can_id */
		op = kzalloc(OPSIZ, GFP_KERNEL);
		if (!op)
			return -ENOMEM;

		op->can_id = msg_head->can_id;
		op->nframes = msg_head->nframes;
		op->cfsiz = CFSIZ(msg_head->flags);
		op->flags = msg_head->flags;

		if (msg_head->nframes > 1) {
			/* create array for CAN frames and copy the data */
			op->frames = kmalloc_array(msg_head->nframes,
						   op->cfsiz,
						   GFP_KERNEL);
			if (!op->frames) {
				kfree(op);
				return -ENOMEM;
			}

			/* create and init array for received CAN frames */
			op->last_frames = kcalloc(msg_head->nframes,
						  op->cfsiz,
						  GFP_KERNEL);
			if (!op->last_frames) {
				kfree(op->frames);
				kfree(op);
				return -ENOMEM;
			}

		} else {
			op->frames = &op->sframe;
			op->last_frames = &op->last_sframe;
		}

		if (msg_head->nframes) {
			err = memcpy_from_msg(op->frames, msg,
					      msg_head->nframes * op->cfsiz);
			if (err < 0) {
				if (op->frames != &op->sframe)
					kfree(op->frames);
				if (op->last_frames != &op->last_sframe)
					kfree(op->last_frames);
				kfree(op);
				return err;
			}
		}

		/* bcm_can_tx / bcm_tx_timeout_handler needs this */
		op->sk = sk;
		op->ifindex = ifindex;

		/* ifindex for timeout events w/o previous frame reception */
		op->rx_ifindex = ifindex;

		/* initialize uninitialized (kzalloc) structure */
		hrtimer_init(&op->timer, CLOCK_MONOTONIC,
			     HRTIMER_MODE_REL_SOFT);
		op->timer.function = bcm_rx_timeout_handler;

		hrtimer_init(&op->thrtimer, CLOCK_MONOTONIC,
			     HRTIMER_MODE_REL_SOFT);
		op->thrtimer.function = bcm_rx_thr_handler;

		/* add this bcm_op to the list of the rx_ops */
		list_add(&op->list, &bo->rx_ops);

		/* call can_rx_register() */
		do_rx_register = 1;

	} /* if ((op = bcm_find_op(&bo->rx_ops, msg_head->can_id, ifindex))) */

	/* check flags */

	if (op->flags & RX_RTR_FRAME) {
		struct canfd_frame *frame0 = op->frames;

		/* no timers in RTR-mode */
		hrtimer_cancel(&op->thrtimer);
		hrtimer_cancel(&op->timer);

		/*
		 * funny feature in RX(!)_SETUP only for RTR-mode:
		 * copy can_id into frame BUT without RTR-flag to
		 * prevent a full-load-loopback-test ... ;-]
		 */
		if ((op->flags & TX_CP_CAN_ID) ||
		    (frame0->can_id == op->can_id))
			frame0->can_id = op->can_id & ~CAN_RTR_FLAG;

	} else {
		if (op->flags & SETTIMER) {

			/* set timer value */
			op->ival1 = msg_head->ival1;
			op->ival2 = msg_head->ival2;
			op->kt_ival1 = bcm_timeval_to_ktime(msg_head->ival1);
			op->kt_ival2 = bcm_timeval_to_ktime(msg_head->ival2);

			/* disable an active timer due to zero value? */
			if (!op->kt_ival1)
				hrtimer_cancel(&op->timer);

			/*
			 * In any case cancel the throttle timer, flush
			 * potentially blocked msgs and reset throttle handling
			 */
			op->kt_lastmsg = 0;
			hrtimer_cancel(&op->thrtimer);
			bcm_rx_thr_flush(op);
		}

		if ((op->flags & STARTTIMER) && op->kt_ival1)
			hrtimer_start(&op->timer, op->kt_ival1,
				      HRTIMER_MODE_REL_SOFT);
	}

	/* now we can register for can_ids, if we added a new bcm_op */
	if (do_rx_register) {
		if (ifindex) {
			struct net_device *dev;

			dev = dev_get_by_index(sock_net(sk), ifindex);
			if (dev) {
				err = can_rx_register(sock_net(sk), dev,
						      op->can_id,
						      REGMASK(op->can_id),
						      bcm_rx_handler, op,
						      "bcm", sk);

				op->rx_reg_dev = dev;
				dev_put(dev);
			}

		} else
			err = can_rx_register(sock_net(sk), NULL, op->can_id,
					      REGMASK(op->can_id),
					      bcm_rx_handler, op, "bcm", sk);
		if (err) {
			/* this bcm rx op is broken -> remove it */
			list_del(&op->list);
			bcm_remove_op(op);
			return err;
		}
	}

	return msg_head->nframes * op->cfsiz + MHSIZ;
}

/*
 * bcm_tx_send - send a single CAN frame to the CAN interface (for bcm_sendmsg)
 */
static int bcm_tx_send(struct msghdr *msg, int ifindex, struct sock *sk,
		       int cfsiz)
{
	struct sk_buff *skb;
	struct net_device *dev;
	int err;

	/* we need a real device to send frames */
	if (!ifindex)
		return -ENODEV;

	skb = alloc_skb(cfsiz + sizeof(struct can_skb_priv), GFP_KERNEL);
	if (!skb)
		return -ENOMEM;

	can_skb_reserve(skb);

	err = memcpy_from_msg(skb_put(skb, cfsiz), msg, cfsiz);
	if (err < 0) {
		kfree_skb(skb);
		return err;
	}

	dev = dev_get_by_index(sock_net(sk), ifindex);
	if (!dev) {
		kfree_skb(skb);
		return -ENODEV;
	}

	can_skb_prv(skb)->ifindex = dev->ifindex;
	can_skb_prv(skb)->skbcnt = 0;
	skb->dev = dev;
	can_skb_set_owner(skb, sk);
	err = can_send(skb, 1); /* send with loopback */
	dev_put(dev);

	if (err)
		return err;

	return cfsiz + MHSIZ;
}

/*
 * bcm_sendmsg - process BCM commands (opcodes) from the userspace
 */
static int bcm_sendmsg(struct socket *sock, struct msghdr *msg, size_t size)
{
	struct sock *sk = sock->sk;
	struct bcm_sock *bo = bcm_sk(sk);
	int ifindex = bo->ifindex; /* default ifindex for this bcm_op */
	struct bcm_msg_head msg_head;
	int cfsiz;
	int ret; /* read bytes or error codes as return value */

	if (!bo->bound)
		return -ENOTCONN;

	/* check for valid message length from userspace */
	if (size < MHSIZ)
		return -EINVAL;

	/* read message head information */
	ret = memcpy_from_msg((u8 *)&msg_head, msg, MHSIZ);
	if (ret < 0)
		return ret;

	cfsiz = CFSIZ(msg_head.flags);
	if ((size - MHSIZ) % cfsiz)
		return -EINVAL;

	/* check for alternative ifindex for this bcm_op */

	if (!ifindex && msg->msg_name) {
		/* no bound device as default => check msg_name */
		DECLARE_SOCKADDR(struct sockaddr_can *, addr, msg->msg_name);

		if (msg->msg_namelen < BCM_MIN_NAMELEN)
			return -EINVAL;

		if (addr->can_family != AF_CAN)
			return -EINVAL;

		/* ifindex from sendto() */
		ifindex = addr->can_ifindex;

		if (ifindex) {
			struct net_device *dev;

			dev = dev_get_by_index(sock_net(sk), ifindex);
			if (!dev)
				return -ENODEV;

			if (dev->type != ARPHRD_CAN) {
				dev_put(dev);
				return -ENODEV;
			}

			dev_put(dev);
		}
	}

	lock_sock(sk);

	switch (msg_head.opcode) {

	case TX_SETUP:
		ret = bcm_tx_setup(&msg_head, msg, ifindex, sk);
		break;

	case RX_SETUP:
		ret = bcm_rx_setup(&msg_head, msg, ifindex, sk);
		break;

	case TX_DELETE:
		if (bcm_delete_tx_op(&bo->tx_ops, &msg_head, ifindex))
			ret = MHSIZ;
		else
			ret = -EINVAL;
		break;

	case RX_DELETE:
		if (bcm_delete_rx_op(&bo->rx_ops, &msg_head, ifindex))
			ret = MHSIZ;
		else
			ret = -EINVAL;
		break;

	case TX_READ:
		/* reuse msg_head for the reply to TX_READ */
		msg_head.opcode  = TX_STATUS;
		ret = bcm_read_op(&bo->tx_ops, &msg_head, ifindex);
		break;

	case RX_READ:
		/* reuse msg_head for the reply to RX_READ */
		msg_head.opcode  = RX_STATUS;
		ret = bcm_read_op(&bo->rx_ops, &msg_head, ifindex);
		break;

	case TX_SEND:
		/* we need exactly one CAN frame behind the msg head */
		if ((msg_head.nframes != 1) || (size != cfsiz + MHSIZ))
			ret = -EINVAL;
		else
			ret = bcm_tx_send(msg, ifindex, sk, cfsiz);
		break;

	default:
		ret = -EINVAL;
		break;
	}

	release_sock(sk);

	return ret;
}

/*
 * notification handler for netdevice status changes
 */
static void bcm_notify(struct bcm_sock *bo, unsigned long msg,
		       struct net_device *dev)
{
	struct sock *sk = &bo->sk;
	struct bcm_op *op;
	int notify_enodev = 0;

	if (!net_eq(dev_net(dev), sock_net(sk)))
		return;

	switch (msg) {

	case NETDEV_UNREGISTER:
		lock_sock(sk);

		/* remove device specific receive entries */
		list_for_each_entry(op, &bo->rx_ops, list)
			if (op->rx_reg_dev == dev)
				bcm_rx_unreg(dev, op);

		/* remove device reference, if this is our bound device */
		if (bo->bound && bo->ifindex == dev->ifindex) {
			bo->bound   = 0;
			bo->ifindex = 0;
			notify_enodev = 1;
		}

		release_sock(sk);

		if (notify_enodev) {
			sk->sk_err = ENODEV;
			if (!sock_flag(sk, SOCK_DEAD))
				sk->sk_error_report(sk);
		}
		break;

	case NETDEV_DOWN:
		if (bo->bound && bo->ifindex == dev->ifindex) {
			sk->sk_err = ENETDOWN;
			if (!sock_flag(sk, SOCK_DEAD))
				sk->sk_error_report(sk);
		}
	}
}

static int bcm_notifier(struct notifier_block *nb, unsigned long msg,
			void *ptr)
{
	struct net_device *dev = netdev_notifier_info_to_dev(ptr);

	if (dev->type != ARPHRD_CAN)
		return NOTIFY_DONE;
	if (msg != NETDEV_UNREGISTER && msg != NETDEV_DOWN)
		return NOTIFY_DONE;
	if (unlikely(bcm_busy_notifier)) /* Check for reentrant bug. */
		return NOTIFY_DONE;

	spin_lock(&bcm_notifier_lock);
	list_for_each_entry(bcm_busy_notifier, &bcm_notifier_list, notifier) {
		spin_unlock(&bcm_notifier_lock);
		bcm_notify(bcm_busy_notifier, msg, dev);
		spin_lock(&bcm_notifier_lock);
	}
	bcm_busy_notifier = NULL;
	spin_unlock(&bcm_notifier_lock);
	return NOTIFY_DONE;
}

/*
 * initial settings for all BCM sockets to be set at socket creation time
 */
static int bcm_init(struct sock *sk)
{
	struct bcm_sock *bo = bcm_sk(sk);

	bo->bound            = 0;
	bo->ifindex          = 0;
	bo->dropped_usr_msgs = 0;
	bo->bcm_proc_read    = NULL;

	INIT_LIST_HEAD(&bo->tx_ops);
	INIT_LIST_HEAD(&bo->rx_ops);

	/* set notifier */
	spin_lock(&bcm_notifier_lock);
	list_add_tail(&bo->notifier, &bcm_notifier_list);
	spin_unlock(&bcm_notifier_lock);

	return 0;
}

/*
 * standard socket functions
 */
static int bcm_release(struct socket *sock)
{
	struct sock *sk = sock->sk;
	struct net *net;
	struct bcm_sock *bo;
	struct bcm_op *op, *next;

	if (!sk)
		return 0;

	net = sock_net(sk);
	bo = bcm_sk(sk);

	/* remove bcm_ops, timer, rx_unregister(), etc. */

	spin_lock(&bcm_notifier_lock);
	while (bcm_busy_notifier == bo) {
		spin_unlock(&bcm_notifier_lock);
		schedule_timeout_uninterruptible(1);
		spin_lock(&bcm_notifier_lock);
	}
	list_del(&bo->notifier);
	spin_unlock(&bcm_notifier_lock);

	lock_sock(sk);

	list_for_each_entry_safe(op, next, &bo->tx_ops, list)
		bcm_remove_op(op);

	list_for_each_entry_safe(op, next, &bo->rx_ops, list) {
		/*
		 * Don't care if we're bound or not (due to netdev problems)
		 * can_rx_unregister() is always a save thing to do here.
		 */
		if (op->ifindex) {
			/*
			 * Only remove subscriptions that had not
			 * been removed due to NETDEV_UNREGISTER
			 * in bcm_notifier()
			 */
			if (op->rx_reg_dev) {
				struct net_device *dev;

				dev = dev_get_by_index(net, op->ifindex);
				if (dev) {
					bcm_rx_unreg(dev, op);
					dev_put(dev);
				}
			}
		} else
			can_rx_unregister(net, NULL, op->can_id,
					  REGMASK(op->can_id),
					  bcm_rx_handler, op);

		bcm_remove_op(op);
	}

#if IS_ENABLED(CONFIG_PROC_FS)
	/* remove procfs entry */
	if (net->can.bcmproc_dir && bo->bcm_proc_read)
		remove_proc_entry(bo->procname, net->can.bcmproc_dir);
#endif /* CONFIG_PROC_FS */

	/* remove device reference */
	if (bo->bound) {
		bo->bound   = 0;
		bo->ifindex = 0;
	}

	sock_orphan(sk);
	sock->sk = NULL;

	release_sock(sk);
	sock_put(sk);

	return 0;
}

static int bcm_connect(struct socket *sock, struct sockaddr *uaddr, int len,
		       int flags)
{
	struct sockaddr_can *addr = (struct sockaddr_can *)uaddr;
	struct sock *sk = sock->sk;
	struct bcm_sock *bo = bcm_sk(sk);
	struct net *net = sock_net(sk);
	int ret = 0;

	if (len < BCM_MIN_NAMELEN)
		return -EINVAL;

	lock_sock(sk);

	if (bo->bound) {
		ret = -EISCONN;
		goto fail;
	}

	/* bind a device to this socket */
	if (addr->can_ifindex) {
		struct net_device *dev;

		dev = dev_get_by_index(net, addr->can_ifindex);
		if (!dev) {
			ret = -ENODEV;
			goto fail;
		}
		if (dev->type != ARPHRD_CAN) {
			dev_put(dev);
			ret = -ENODEV;
			goto fail;
		}

		bo->ifindex = dev->ifindex;
		dev_put(dev);

	} else {
		/* no interface reference for ifindex = 0 ('any' CAN device) */
		bo->ifindex = 0;
	}

#if IS_ENABLED(CONFIG_PROC_FS)
	if (net->can.bcmproc_dir) {
		/* unique socket address as filename */
		sprintf(bo->procname, "%lu", sock_i_ino(sk));
		bo->bcm_proc_read = proc_create_net_single(bo->procname, 0644,
						     net->can.bcmproc_dir,
						     bcm_proc_show, sk);
		if (!bo->bcm_proc_read) {
			ret = -ENOMEM;
			goto fail;
		}
	}
#endif /* CONFIG_PROC_FS */

	bo->bound = 1;

fail:
	release_sock(sk);

	return ret;
}

static int bcm_recvmsg(struct socket *sock, struct msghdr *msg, size_t size,
		       int flags)
{
	struct sock *sk = sock->sk;
	struct sk_buff *skb;
	int error = 0;
	int noblock;
	int err;

	noblock =  flags & MSG_DONTWAIT;
	flags   &= ~MSG_DONTWAIT;
	skb = skb_recv_datagram(sk, flags, noblock, &error);
	if (!skb)
		return error;

	if (skb->len < size)
		size = skb->len;

	err = memcpy_to_msg(msg, skb->data, size);
	if (err < 0) {
		skb_free_datagram(sk, skb);
		return err;
	}

	sock_recv_ts_and_drops(msg, sk, skb);

	if (msg->msg_name) {
		__sockaddr_check_size(BCM_MIN_NAMELEN);
		msg->msg_namelen = BCM_MIN_NAMELEN;
		memcpy(msg->msg_name, skb->cb, msg->msg_namelen);
	}

	skb_free_datagram(sk, skb);

	return size;
}

static int bcm_sock_no_ioctlcmd(struct socket *sock, unsigned int cmd,
				unsigned long arg)
{
	/* no ioctls for socket layer -> hand it down to NIC layer */
	return -ENOIOCTLCMD;
}

static const struct proto_ops bcm_ops = {
	.family        = PF_CAN,
	.release       = bcm_release,
	.bind          = sock_no_bind,
	.connect       = bcm_connect,
	.socketpair    = sock_no_socketpair,
	.accept        = sock_no_accept,
	.getname       = sock_no_getname,
	.poll          = datagram_poll,
	.ioctl         = bcm_sock_no_ioctlcmd,
	.gettstamp     = sock_gettstamp,
	.listen        = sock_no_listen,
	.shutdown      = sock_no_shutdown,
	.sendmsg       = bcm_sendmsg,
	.recvmsg       = bcm_recvmsg,
	.mmap          = sock_no_mmap,
	.sendpage      = sock_no_sendpage,
};

static struct proto bcm_proto __read_mostly = {
	.name       = "CAN_BCM",
	.owner      = THIS_MODULE,
	.obj_size   = sizeof(struct bcm_sock),
	.init       = bcm_init,
};

static const struct can_proto bcm_can_proto = {
	.type       = SOCK_DGRAM,
	.protocol   = CAN_BCM,
	.ops        = &bcm_ops,
	.prot       = &bcm_proto,
};

static int canbcm_pernet_init(struct net *net)
{
#if IS_ENABLED(CONFIG_PROC_FS)
	/* create /proc/net/can-bcm directory */
	net->can.bcmproc_dir = proc_net_mkdir(net, "can-bcm", net->proc_net);
#endif /* CONFIG_PROC_FS */

	return 0;
}

static void canbcm_pernet_exit(struct net *net)
{
#if IS_ENABLED(CONFIG_PROC_FS)
	/* remove /proc/net/can-bcm directory */
	if (net->can.bcmproc_dir)
		remove_proc_entry("can-bcm", net->proc_net);
#endif /* CONFIG_PROC_FS */
}

static struct pernet_operations canbcm_pernet_ops __read_mostly = {
	.init = canbcm_pernet_init,
	.exit = canbcm_pernet_exit,
};

static struct notifier_block canbcm_notifier = {
	.notifier_call = bcm_notifier
};

static int __init bcm_module_init(void)
{
	int err;

	pr_info("can: broadcast manager protocol\n");

	err = can_proto_register(&bcm_can_proto);
	if (err < 0) {
		printk(KERN_ERR "can: registration of bcm protocol failed\n");
		return err;
	}

	register_pernet_subsys(&canbcm_pernet_ops);
	register_netdevice_notifier(&canbcm_notifier);
	return 0;
}

static void __exit bcm_module_exit(void)
{
	can_proto_unregister(&bcm_can_proto);
	unregister_netdevice_notifier(&canbcm_notifier);
	unregister_pernet_subsys(&canbcm_pernet_ops);
}

module_init(bcm_module_init);
module_exit(bcm_module_exit);
