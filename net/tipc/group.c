/*
 * net/tipc/group.c: TIPC group messaging code
 *
 * Copyright (c) 2017, Ericsson AB
 * Copyright (c) 2020, Red Hat Inc
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the names of the copyright holders nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * Alternatively, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") version 2 as published by the Free
 * Software Foundation.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "core.h"
#include "addr.h"
#include "group.h"
#include "bcast.h"
#include "topsrv.h"
#include "msg.h"
#include "socket.h"
#include "node.h"
#include "name_table.h"
#include "subscr.h"

#define ADV_UNIT (((MAX_MSG_SIZE + MAX_H_SIZE) / FLOWCTL_BLK_SZ) + 1)
#define ADV_IDLE ADV_UNIT
#define ADV_ACTIVE (ADV_UNIT * 12)

enum mbr_state {
	MBR_JOINING,
	MBR_PUBLISHED,
	MBR_JOINED,
	MBR_PENDING,
	MBR_ACTIVE,
	MBR_RECLAIMING,
	MBR_REMITTED,
	MBR_LEAVING
};

struct tipc_member {
	struct rb_node tree_node;
	struct list_head list;
	struct list_head small_win;
	struct sk_buff_head deferredq;
	struct tipc_group *group;
	u32 node;
	u32 port;
	u32 instance;
	enum mbr_state state;
	u16 advertised;
	u16 window;
	u16 bc_rcv_nxt;
	u16 bc_syncpt;
	u16 bc_acked;
};

struct tipc_group {
	struct rb_root members;
	struct list_head small_win;
	struct list_head pending;
	struct list_head active;
	struct tipc_nlist dests;
	struct net *net;
	int subid;
	u32 type;
	u32 instance;
	u32 scope;
	u32 portid;
	u16 member_cnt;
	u16 active_cnt;
	u16 max_active;
	u16 bc_snd_nxt;
	u16 bc_ackers;
	bool *open;
	bool loopback;
	bool events;
};

static void tipc_group_proto_xmit(struct tipc_group *grp, struct tipc_member *m,
				  int mtyp, struct sk_buff_head *xmitq);

static void tipc_group_open(struct tipc_member *m, bool *wakeup)
{
	*wakeup = false;
	if (list_empty(&m->small_win))
		return;
	list_del_init(&m->small_win);
	*m->group->open = true;
	*wakeup = true;
}

static void tipc_group_decr_active(struct tipc_group *grp,
				   struct tipc_member *m)
{
	if (m->state == MBR_ACTIVE || m->state == MBR_RECLAIMING ||
	    m->state == MBR_REMITTED)
		grp->active_cnt--;
}

static int tipc_group_rcvbuf_limit(struct tipc_group *grp)
{
	int max_active, active_pool, idle_pool;
	int mcnt = grp->member_cnt + 1;

	/* Limit simultaneous reception from other members */
	max_active = min(mcnt / 8, 64);
	max_active = max(max_active, 16);
	grp->max_active = max_active;

	/* Reserve blocks for active and idle members */
	active_pool = max_active * ADV_ACTIVE;
	idle_pool = (mcnt - max_active) * ADV_IDLE;

	/* Scale to bytes, considering worst-case truesize/msgsize ratio */
	return (active_pool + idle_pool) * FLOWCTL_BLK_SZ * 4;
}

u16 tipc_group_bc_snd_nxt(struct tipc_group *grp)
{
	return grp->bc_snd_nxt;
}

static bool tipc_group_is_receiver(struct tipc_member *m)
{
	return m && m->state != MBR_JOINING && m->state != MBR_LEAVING;
}

static bool tipc_group_is_sender(struct tipc_member *m)
{
	return m && m->state != MBR_JOINING && m->state != MBR_PUBLISHED;
}

u32 tipc_group_exclude(struct tipc_group *grp)
{
	if (!grp->loopback)
		return grp->portid;
	return 0;
}

struct tipc_group *tipc_group_create(struct net *net, u32 portid,
				     struct tipc_group_req *mreq,
				     bool *group_is_open)
{
	u32 filter = TIPC_SUB_PORTS | TIPC_SUB_NO_STATUS;
	bool global = mreq->scope != TIPC_NODE_SCOPE;
	struct tipc_group *grp;
	u32 type = mreq->type;

	grp = kzalloc(sizeof(*grp), GFP_ATOMIC);
	if (!grp)
		return NULL;
	tipc_nlist_init(&grp->dests, tipc_own_addr(net));
	INIT_LIST_HEAD(&grp->small_win);
	INIT_LIST_HEAD(&grp->active);
	INIT_LIST_HEAD(&grp->pending);
	grp->members = RB_ROOT;
	grp->net = net;
	grp->portid = portid;
	grp->type = type;
	grp->instance = mreq->instance;
	grp->scope = mreq->scope;
	grp->loopback = mreq->flags & TIPC_GROUP_LOOPBACK;
	grp->events = mreq->flags & TIPC_GROUP_MEMBER_EVTS;
	grp->open = group_is_open;
	*grp->open = false;
	filter |= global ? TIPC_SUB_CLUSTER_SCOPE : TIPC_SUB_NODE_SCOPE;
	if (tipc_topsrv_kern_subscr(net, portid, type, 0, ~0,
				    filter, &grp->subid))
		return grp;
	kfree(grp);
	return NULL;
}

void tipc_group_join(struct net *net, struct tipc_group *grp, int *sk_rcvbuf)
{
	struct rb_root *tree = &grp->members;
	struct tipc_member *m, *tmp;
	struct sk_buff_head xmitq;

	__skb_queue_head_init(&xmitq);
	rbtree_postorder_for_each_entry_safe(m, tmp, tree, tree_node) {
		tipc_group_proto_xmit(grp, m, GRP_JOIN_MSG, &xmitq);
		tipc_group_update_member(m, 0);
	}
	tipc_node_distr_xmit(net, &xmitq);
	*sk_rcvbuf = tipc_group_rcvbuf_limit(grp);
}

void tipc_group_delete(struct net *net, struct tipc_group *grp)
{
	struct rb_root *tree = &grp->members;
	struct tipc_member *m, *tmp;
	struct sk_buff_head xmitq;

	__skb_queue_head_init(&xmitq);

	rbtree_postorder_for_each_entry_safe(m, tmp, tree, tree_node) {
		tipc_group_proto_xmit(grp, m, GRP_LEAVE_MSG, &xmitq);
		__skb_queue_purge(&m->deferredq);
		list_del(&m->list);
		kfree(m);
	}
	tipc_node_distr_xmit(net, &xmitq);
	tipc_nlist_purge(&grp->dests);
	tipc_topsrv_kern_unsubscr(net, grp->subid);
	kfree(grp);
}

static struct tipc_member *tipc_group_find_member(struct tipc_group *grp,
						  u32 node, u32 port)
{
	struct rb_node *n = grp->members.rb_node;
	u64 nkey, key = (u64)node << 32 | port;
	struct tipc_member *m;

	while (n) {
		m = container_of(n, struct tipc_member, tree_node);
		nkey = (u64)m->node << 32 | m->port;
		if (key < nkey)
			n = n->rb_left;
		else if (key > nkey)
			n = n->rb_right;
		else
			return m;
	}
	return NULL;
}

static struct tipc_member *tipc_group_find_dest(struct tipc_group *grp,
						u32 node, u32 port)
{
	struct tipc_member *m;

	m = tipc_group_find_member(grp, node, port);
	if (m && tipc_group_is_receiver(m))
		return m;
	return NULL;
}

static struct tipc_member *tipc_group_find_node(struct tipc_group *grp,
						u32 node)
{
	struct tipc_member *m;
	struct rb_node *n;

	for (n = rb_first(&grp->members); n; n = rb_next(n)) {
		m = container_of(n, struct tipc_member, tree_node);
		if (m->node == node)
			return m;
	}
	return NULL;
}

static int tipc_group_add_to_tree(struct tipc_group *grp,
				  struct tipc_member *m)
{
	u64 nkey, key = (u64)m->node << 32 | m->port;
	struct rb_node **n, *parent = NULL;
	struct tipc_member *tmp;

	n = &grp->members.rb_node;
	while (*n) {
		tmp = container_of(*n, struct tipc_member, tree_node);
		parent = *n;
		tmp = container_of(parent, struct tipc_member, tree_node);
		nkey = (u64)tmp->node << 32 | tmp->port;
		if (key < nkey)
			n = &(*n)->rb_left;
		else if (key > nkey)
			n = &(*n)->rb_right;
		else
			return -EEXIST;
	}
	rb_link_node(&m->tree_node, parent, n);
	rb_insert_color(&m->tree_node, &grp->members);
	return 0;
}

static struct tipc_member *tipc_group_create_member(struct tipc_group *grp,
						    u32 node, u32 port,
						    u32 instance, int state)
{
	struct tipc_member *m;
	int ret;

	m = kzalloc(sizeof(*m), GFP_ATOMIC);
	if (!m)
		return NULL;
	INIT_LIST_HEAD(&m->list);
	INIT_LIST_HEAD(&m->small_win);
	__skb_queue_head_init(&m->deferredq);
	m->group = grp;
	m->node = node;
	m->port = port;
	m->instance = instance;
	m->bc_acked = grp->bc_snd_nxt - 1;
	ret = tipc_group_add_to_tree(grp, m);
	if (ret < 0) {
		kfree(m);
		return NULL;
	}
	grp->member_cnt++;
	tipc_nlist_add(&grp->dests, m->node);
	m->state = state;
	return m;
}

void tipc_group_add_member(struct tipc_group *grp, u32 node,
			   u32 port, u32 instance)
{
	tipc_group_create_member(grp, node, port, instance, MBR_PUBLISHED);
}

static void tipc_group_delete_member(struct tipc_group *grp,
				     struct tipc_member *m)
{
	rb_erase(&m->tree_node, &grp->members);
	grp->member_cnt--;

	/* Check if we were waiting for replicast ack from this member */
	if (grp->bc_ackers && less(m->bc_acked, grp->bc_snd_nxt - 1))
		grp->bc_ackers--;

	list_del_init(&m->list);
	list_del_init(&m->small_win);
	tipc_group_decr_active(grp, m);

	/* If last member on a node, remove node from dest list */
	if (!tipc_group_find_node(grp, m->node))
		tipc_nlist_del(&grp->dests, m->node);

	kfree(m);
}

struct tipc_nlist *tipc_group_dests(struct tipc_group *grp)
{
	return &grp->dests;
}

void tipc_group_self(struct tipc_group *grp, struct tipc_service_range *seq,
		     int *scope)
{
	seq->type = grp->type;
	seq->lower = grp->instance;
	seq->upper = grp->instance;
	*scope = grp->scope;
}

void tipc_group_update_member(struct tipc_member *m, int len)
{
	struct tipc_group *grp = m->group;
	struct tipc_member *_m, *tmp;

	if (!tipc_group_is_receiver(m))
		return;

	m->window -= len;

	if (m->window >= ADV_IDLE)
		return;

	list_del_init(&m->small_win);

	/* Sort member into small_window members' list */
	list_for_each_entry_safe(_m, tmp, &grp->small_win, small_win) {
		if (_m->window > m->window)
			break;
	}
	list_add_tail(&m->small_win, &_m->small_win);
}

void tipc_group_update_bc_members(struct tipc_group *grp, int len, bool ack)
{
	u16 prev = grp->bc_snd_nxt - 1;
	struct tipc_member *m;
	struct rb_node *n;
	u16 ackers = 0;

	for (n = rb_first(&grp->members); n; n = rb_next(n)) {
		m = container_of(n, struct tipc_member, tree_node);
		if (tipc_group_is_receiver(m)) {
			tipc_group_update_member(m, len);
			m->bc_acked = prev;
			ackers++;
		}
	}

	/* Mark number of acknowledges to expect, if any */
	if (ack)
		grp->bc_ackers = ackers;
	grp->bc_snd_nxt++;
}

bool tipc_group_cong(struct tipc_group *grp, u32 dnode, u32 dport,
		     int len, struct tipc_member **mbr)
{
	struct sk_buff_head xmitq;
	struct tipc_member *m;
	int adv, state;

	m = tipc_group_find_dest(grp, dnode, dport);
	if (!tipc_group_is_receiver(m)) {
		*mbr = NULL;
		return false;
	}
	*mbr = m;

	if (m->window >= len)
		return false;

	*grp->open = false;

	/* If not fully advertised, do it now to prevent mutual blocking */
	adv = m->advertised;
	state = m->state;
	if (state == MBR_JOINED && adv == ADV_IDLE)
		return true;
	if (state == MBR_ACTIVE && adv == ADV_ACTIVE)
		return true;
	if (state == MBR_PENDING && adv == ADV_IDLE)
		return true;
	__skb_queue_head_init(&xmitq);
	tipc_group_proto_xmit(grp, m, GRP_ADV_MSG, &xmitq);
	tipc_node_distr_xmit(grp->net, &xmitq);
	return true;
}

bool tipc_group_bc_cong(struct tipc_group *grp, int len)
{
	struct tipc_member *m = NULL;

	/* If prev bcast was replicast, reject until all receivers have acked */
	if (grp->bc_ackers) {
		*grp->open = false;
		return true;
	}
	if (list_empty(&grp->small_win))
		return false;

	m = list_first_entry(&grp->small_win, struct tipc_member, small_win);
	if (m->window >= len)
		return false;

	return tipc_group_cong(grp, m->node, m->port, len, &m);
}

/* tipc_group_sort_msg() - sort msg into queue by bcast sequence number
 */
static void tipc_group_sort_msg(struct sk_buff *skb, struct sk_buff_head *defq)
{
	struct tipc_msg *_hdr, *hdr = buf_msg(skb);
	u16 bc_seqno = msg_grp_bc_seqno(hdr);
	struct sk_buff *_skb, *tmp;
	int mtyp = msg_type(hdr);

	/* Bcast/mcast may be bypassed by ucast or other bcast, - sort it in */
	if (mtyp == TIPC_GRP_BCAST_MSG || mtyp == TIPC_GRP_MCAST_MSG) {
		skb_queue_walk_safe(defq, _skb, tmp) {
			_hdr = buf_msg(_skb);
			if (!less(bc_seqno, msg_grp_bc_seqno(_hdr)))
				continue;
			__skb_queue_before(defq, _skb, skb);
			return;
		}
		/* Bcast was not bypassed, - add to tail */
	}
	/* Unicasts are never bypassed, - always add to tail */
	__skb_queue_tail(defq, skb);
}

/* tipc_group_filter_msg() - determine if we should accept arriving message
 */
void tipc_group_filter_msg(struct tipc_group *grp, struct sk_buff_head *inputq,
			   struct sk_buff_head *xmitq)
{
	struct sk_buff *skb = __skb_dequeue(inputq);
	bool ack, deliver, update, leave = false;
	struct sk_buff_head *defq;
	struct tipc_member *m;
	struct tipc_msg *hdr;
	u32 node, port;
	int mtyp, blks;

	if (!skb)
		return;

	hdr = buf_msg(skb);
	node =  msg_orignode(hdr);
	port = msg_origport(hdr);

	if (!msg_in_group(hdr))
		goto drop;

	m = tipc_group_find_member(grp, node, port);
	if (!tipc_group_is_sender(m))
		goto drop;

	if (less(msg_grp_bc_seqno(hdr), m->bc_rcv_nxt))
		goto drop;

	TIPC_SKB_CB(skb)->orig_member = m->instance;
	defq = &m->deferredq;
	tipc_group_sort_msg(skb, defq);

	while ((skb = skb_peek(defq))) {
		hdr = buf_msg(skb);
		mtyp = msg_type(hdr);
		blks = msg_blocks(hdr);
		deliver = true;
		ack = false;
		update = false;

		if (more(msg_grp_bc_seqno(hdr), m->bc_rcv_nxt))
			break;

		/* Decide what to do with message */
		switch (mtyp) {
		case TIPC_GRP_MCAST_MSG:
			if (msg_nameinst(hdr) != grp->instance) {
				update = true;
				deliver = false;
			}
			fallthrough;
		case TIPC_GRP_BCAST_MSG:
			m->bc_rcv_nxt++;
			ack = msg_grp_bc_ack_req(hdr);
			break;
		case TIPC_GRP_UCAST_MSG:
			break;
		case TIPC_GRP_MEMBER_EVT:
			if (m->state == MBR_LEAVING)
				leave = true;
			if (!grp->events)
				deliver = false;
			break;
		default:
			break;
		}

		/* Execute decisions */
		__skb_dequeue(defq);
		if (deliver)
			__skb_queue_tail(inputq, skb);
		else
			kfree_skb(skb);

		if (ack)
			tipc_group_proto_xmit(grp, m, GRP_ACK_MSG, xmitq);

		if (leave) {
			__skb_queue_purge(defq);
			tipc_group_delete_member(grp, m);
			break;
		}
		if (!update)
			continue;

		tipc_group_update_rcv_win(grp, blks, node, port, xmitq);
	}
	return;
drop:
	kfree_skb(skb);
}

void tipc_group_update_rcv_win(struct tipc_group *grp, int blks, u32 node,
			       u32 port, struct sk_buff_head *xmitq)
{
	struct list_head *active = &grp->active;
	int max_active = grp->max_active;
	int reclaim_limit = max_active * 3 / 4;
	int active_cnt = grp->active_cnt;
	struct tipc_member *m, *rm, *pm;

	m = tipc_group_find_member(grp, node, port);
	if (!m)
		return;

	m->advertised -= blks;

	switch (m->state) {
	case MBR_JOINED:
		/* First, decide if member can go active */
		if (active_cnt <= max_active) {
			m->state = MBR_ACTIVE;
			list_add_tail(&m->list, active);
			grp->active_cnt++;
			tipc_group_proto_xmit(grp, m, GRP_ADV_MSG, xmitq);
		} else {
			m->state = MBR_PENDING;
			list_add_tail(&m->list, &grp->pending);
		}

		if (active_cnt < reclaim_limit)
			break;

		/* Reclaim from oldest active member, if possible */
		if (!list_empty(active)) {
			rm = list_first_entry(active, struct tipc_member, list);
			rm->state = MBR_RECLAIMING;
			list_del_init(&rm->list);
			tipc_group_proto_xmit(grp, rm, GRP_RECLAIM_MSG, xmitq);
			break;
		}
		/* Nobody to reclaim from; - revert oldest pending to JOINED */
		pm = list_first_entry(&grp->pending, struct tipc_member, list);
		list_del_init(&pm->list);
		pm->state = MBR_JOINED;
		tipc_group_proto_xmit(grp, pm, GRP_ADV_MSG, xmitq);
		break;
	case MBR_ACTIVE:
		if (!list_is_last(&m->list, &grp->active))
			list_move_tail(&m->list, &grp->active);
		if (m->advertised > (ADV_ACTIVE * 3 / 4))
			break;
		tipc_group_proto_xmit(grp, m, GRP_ADV_MSG, xmitq);
		break;
	case MBR_REMITTED:
		if (m->advertised > ADV_IDLE)
			break;
		m->state = MBR_JOINED;
		grp->active_cnt--;
		if (m->advertised < ADV_IDLE) {
			pr_warn_ratelimited("Rcv unexpected msg after REMIT\n");
			tipc_group_proto_xmit(grp, m, GRP_ADV_MSG, xmitq);
		}

		if (list_empty(&grp->pending))
			return;

		/* Set oldest pending member to active and advertise */
		pm = list_first_entry(&grp->pending, struct tipc_member, list);
		pm->state = MBR_ACTIVE;
		list_move_tail(&pm->list, &grp->active);
		grp->active_cnt++;
		tipc_group_proto_xmit(grp, pm, GRP_ADV_MSG, xmitq);
		break;
	case MBR_RECLAIMING:
	case MBR_JOINING:
	case MBR_LEAVING:
	default:
		break;
	}
}

static void tipc_group_create_event(struct tipc_group *grp,
				    struct tipc_member *m,
				    u32 event, u16 seqno,
				    struct sk_buff_head *inputq)
{	u32 dnode = tipc_own_addr(grp->net);
	struct tipc_event evt;
	struct sk_buff *skb;
	struct tipc_msg *hdr;

	memset(&evt, 0, sizeof(evt));
	evt.event = event;
	evt.found_lower = m->instance;
	evt.found_upper = m->instance;
	evt.port.ref = m->port;
	evt.port.node = m->node;
	evt.s.seq.type = grp->type;
	evt.s.seq.lower = m->instance;
	evt.s.seq.upper = m->instance;

	skb = tipc_msg_create(TIPC_CRITICAL_IMPORTANCE, TIPC_GRP_MEMBER_EVT,
			      GROUP_H_SIZE, sizeof(evt), dnode, m->node,
			      grp->portid, m->port, 0);
	if (!skb)
		return;

	hdr = buf_msg(skb);
	msg_set_nametype(hdr, grp->type);
	msg_set_grp_evt(hdr, event);
	msg_set_dest_droppable(hdr, true);
	msg_set_grp_bc_seqno(hdr, seqno);
	memcpy(msg_data(hdr), &evt, sizeof(evt));
	TIPC_SKB_CB(skb)->orig_member = m->instance;
	__skb_queue_tail(inputq, skb);
}

static void tipc_group_proto_xmit(struct tipc_group *grp, struct tipc_member *m,
				  int mtyp, struct sk_buff_head *xmitq)
{
	struct tipc_msg *hdr;
	struct sk_buff *skb;
	int adv = 0;

	skb = tipc_msg_create(GROUP_PROTOCOL, mtyp, INT_H_SIZE, 0,
			      m->node, tipc_own_addr(grp->net),
			      m->port, grp->portid, 0);
	if (!skb)
		return;

	if (m->state == MBR_ACTIVE)
		adv = ADV_ACTIVE - m->advertised;
	else if (m->state == MBR_JOINED || m->state == MBR_PENDING)
		adv = ADV_IDLE - m->advertised;

	hdr = buf_msg(skb);

	if (mtyp == GRP_JOIN_MSG) {
		msg_set_grp_bc_syncpt(hdr, grp->bc_snd_nxt);
		msg_set_adv_win(hdr, adv);
		m->advertised += adv;
	} else if (mtyp == GRP_LEAVE_MSG) {
		msg_set_grp_bc_syncpt(hdr, grp->bc_snd_nxt);
	} else if (mtyp == GRP_ADV_MSG) {
		msg_set_adv_win(hdr, adv);
		m->advertised += adv;
	} else if (mtyp == GRP_ACK_MSG) {
		msg_set_grp_bc_acked(hdr, m->bc_rcv_nxt);
	} else if (mtyp == GRP_REMIT_MSG) {
		msg_set_grp_remitted(hdr, m->window);
	}
	msg_set_dest_droppable(hdr, true);
	__skb_queue_tail(xmitq, skb);
}

void tipc_group_proto_rcv(struct tipc_group *grp, bool *usr_wakeup,
			  struct tipc_msg *hdr, struct sk_buff_head *inputq,
			  struct sk_buff_head *xmitq)
{
	u32 node = msg_orignode(hdr);
	u32 port = msg_origport(hdr);
	struct tipc_member *m, *pm;
	u16 remitted, in_flight;

	if (!grp)
		return;

	if (grp->scope == TIPC_NODE_SCOPE && node != tipc_own_addr(grp->net))
		return;

	m = tipc_group_find_member(grp, node, port);

	switch (msg_type(hdr)) {
	case GRP_JOIN_MSG:
		if (!m)
			m = tipc_group_create_member(grp, node, port,
						     0, MBR_JOINING);
		if (!m)
			return;
		m->bc_syncpt = msg_grp_bc_syncpt(hdr);
		m->bc_rcv_nxt = m->bc_syncpt;
		m->window += msg_adv_win(hdr);

		/* Wait until PUBLISH event is received if necessary */
		if (m->state != MBR_PUBLISHED)
			return;

		/* Member can be taken into service */
		m->state = MBR_JOINED;
		tipc_group_open(m, usr_wakeup);
		tipc_group_update_member(m, 0);
		tipc_group_proto_xmit(grp, m, GRP_ADV_MSG, xmitq);
		tipc_group_create_event(grp, m, TIPC_PUBLISHED,
					m->bc_syncpt, inputq);
		return;
	case GRP_LEAVE_MSG:
		if (!m)
			return;
		m->bc_syncpt = msg_grp_bc_syncpt(hdr);
		list_del_init(&m->list);
		tipc_group_open(m, usr_wakeup);
		tipc_group_decr_active(grp, m);
		m->state = MBR_LEAVING;
		tipc_group_create_event(grp, m, TIPC_WITHDRAWN,
					m->bc_syncpt, inputq);
		return;
	case GRP_ADV_MSG:
		if (!m)
			return;
		m->window += msg_adv_win(hdr);
		tipc_group_open(m, usr_wakeup);
		return;
	case GRP_ACK_MSG:
		if (!m)
			return;
		m->bc_acked = msg_grp_bc_acked(hdr);
		if (--grp->bc_ackers)
			return;
		list_del_init(&m->small_win);
		*m->group->open = true;
		*usr_wakeup = true;
		tipc_group_update_member(m, 0);
		return;
	case GRP_RECLAIM_MSG:
		if (!m)
			return;
		tipc_group_proto_xmit(grp, m, GRP_REMIT_MSG, xmitq);
		m->window = ADV_IDLE;
		tipc_group_open(m, usr_wakeup);
		return;
	case GRP_REMIT_MSG:
		if (!m || m->state != MBR_RECLAIMING)
			return;

		remitted = msg_grp_remitted(hdr);

		/* Messages preceding the REMIT still in receive queue */
		if (m->advertised > remitted) {
			m->state = MBR_REMITTED;
			in_flight = m->advertised - remitted;
			m->advertised = ADV_IDLE + in_flight;
			return;
		}
		/* This should never happen */
		if (m->advertised < remitted)
			pr_warn_ratelimited("Unexpected REMIT msg\n");

		/* All messages preceding the REMIT have been read */
		m->state = MBR_JOINED;
		grp->active_cnt--;
		m->advertised = ADV_IDLE;

		/* Set oldest pending member to active and advertise */
		if (list_empty(&grp->pending))
			return;
		pm = list_first_entry(&grp->pending, struct tipc_member, list);
		pm->state = MBR_ACTIVE;
		list_move_tail(&pm->list, &grp->active);
		grp->active_cnt++;
		if (pm->advertised <= (ADV_ACTIVE * 3 / 4))
			tipc_group_proto_xmit(grp, pm, GRP_ADV_MSG, xmitq);
		return;
	default:
		pr_warn("Received unknown GROUP_PROTO message\n");
	}
}

/* tipc_group_member_evt() - receive and handle a member up/down event
 */
void tipc_group_member_evt(struct tipc_group *grp,
			   bool *usr_wakeup,
			   int *sk_rcvbuf,
			   struct tipc_msg *hdr,
			   struct sk_buff_head *inputq,
			   struct sk_buff_head *xmitq)
{
	struct tipc_event *evt = (void *)msg_data(hdr);
	u32 instance = evt->found_lower;
	u32 node = evt->port.node;
	u32 port = evt->port.ref;
	int event = evt->event;
	struct tipc_member *m;
	struct net *net;
	u32 self;

	if (!grp)
		return;

	net = grp->net;
	self = tipc_own_addr(net);
	if (!grp->loopback && node == self && port == grp->portid)
		return;

	m = tipc_group_find_member(grp, node, port);

	switch (event) {
	case TIPC_PUBLISHED:
		/* Send and wait for arrival of JOIN message if necessary */
		if (!m) {
			m = tipc_group_create_member(grp, node, port, instance,
						     MBR_PUBLISHED);
			if (!m)
				break;
			tipc_group_update_member(m, 0);
			tipc_group_proto_xmit(grp, m, GRP_JOIN_MSG, xmitq);
			break;
		}

		if (m->state != MBR_JOINING)
			break;

		/* Member can be taken into service */
		m->instance = instance;
		m->state = MBR_JOINED;
		tipc_group_open(m, usr_wakeup);
		tipc_group_update_member(m, 0);
		tipc_group_proto_xmit(grp, m, GRP_JOIN_MSG, xmitq);
		tipc_group_create_event(grp, m, TIPC_PUBLISHED,
					m->bc_syncpt, inputq);
		break;
	case TIPC_WITHDRAWN:
		if (!m)
			break;

		tipc_group_decr_active(grp, m);
		m->state = MBR_LEAVING;
		list_del_init(&m->list);
		tipc_group_open(m, usr_wakeup);

		/* Only send event if no LEAVE message can be expected */
		if (!tipc_node_is_up(net, node))
			tipc_group_create_event(grp, m, TIPC_WITHDRAWN,
						m->bc_rcv_nxt, inputq);
		break;
	default:
		break;
	}
	*sk_rcvbuf = tipc_group_rcvbuf_limit(grp);
}

int tipc_group_fill_sock_diag(struct tipc_group *grp, struct sk_buff *skb)
{
	struct nlattr *group = nla_nest_start_noflag(skb, TIPC_NLA_SOCK_GROUP);

	if (!group)
		return -EMSGSIZE;

	if (nla_put_u32(skb, TIPC_NLA_SOCK_GROUP_ID,
			grp->type) ||
	    nla_put_u32(skb, TIPC_NLA_SOCK_GROUP_INSTANCE,
			grp->instance) ||
	    nla_put_u32(skb, TIPC_NLA_SOCK_GROUP_BC_SEND_NEXT,
			grp->bc_snd_nxt))
		goto group_msg_cancel;

	if (grp->scope == TIPC_NODE_SCOPE)
		if (nla_put_flag(skb, TIPC_NLA_SOCK_GROUP_NODE_SCOPE))
			goto group_msg_cancel;

	if (grp->scope == TIPC_CLUSTER_SCOPE)
		if (nla_put_flag(skb, TIPC_NLA_SOCK_GROUP_CLUSTER_SCOPE))
			goto group_msg_cancel;

	if (*grp->open)
		if (nla_put_flag(skb, TIPC_NLA_SOCK_GROUP_OPEN))
			goto group_msg_cancel;

	nla_nest_end(skb, group);
	return 0;

group_msg_cancel:
	nla_nest_cancel(skb, group);
	return -1;
}
