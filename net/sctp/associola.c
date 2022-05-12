// SPDX-License-Identifier: GPL-2.0-or-later
/* SCTP kernel implementation
 * (C) Copyright IBM Corp. 2001, 2004
 * Copyright (c) 1999-2000 Cisco, Inc.
 * Copyright (c) 1999-2001 Motorola, Inc.
 * Copyright (c) 2001 Intel Corp.
 * Copyright (c) 2001 La Monte H.P. Yarroll
 *
 * This file is part of the SCTP kernel implementation
 *
 * This module provides the abstraction for an SCTP association.
 *
 * Please send any bug reports or fixes you make to the
 * email address(es):
 *    lksctp developers <linux-sctp@vger.kernel.org>
 *
 * Written or modified by:
 *    La Monte H.P. Yarroll <piggy@acm.org>
 *    Karl Knutson          <karl@athena.chicago.il.us>
 *    Jon Grimm             <jgrimm@us.ibm.com>
 *    Xingang Guo           <xingang.guo@intel.com>
 *    Hui Huang             <hui.huang@nokia.com>
 *    Sridhar Samudrala	    <sri@us.ibm.com>
 *    Daisy Chang	    <daisyc@us.ibm.com>
 *    Ryan Layer	    <rmlayer@us.ibm.com>
 *    Kevin Gao             <kevin.gao@intel.com>
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/poll.h>
#include <linux/init.h>

#include <linux/slab.h>
#include <linux/in.h>
#include <net/ipv6.h>
#include <net/sctp/sctp.h>
#include <net/sctp/sm.h>

/* Forward declarations for internal functions. */
static void sctp_select_active_and_retran_path(struct sctp_association *asoc);
static void sctp_assoc_bh_rcv(struct work_struct *work);
static void sctp_assoc_free_asconf_acks(struct sctp_association *asoc);
static void sctp_assoc_free_asconf_queue(struct sctp_association *asoc);

/* 1st Level Abstractions. */

/* Initialize a new association from provided memory. */
static struct sctp_association *sctp_association_init(
					struct sctp_association *asoc,
					const struct sctp_endpoint *ep,
					const struct sock *sk,
					enum sctp_scope scope, gfp_t gfp)
{
	struct sctp_sock *sp;
	struct sctp_paramhdr *p;
	int i;

	/* Retrieve the SCTP per socket area.  */
	sp = sctp_sk((struct sock *)sk);

	/* Discarding const is appropriate here.  */
	asoc->ep = (struct sctp_endpoint *)ep;
	asoc->base.sk = (struct sock *)sk;
	asoc->base.net = sock_net(sk);

	sctp_endpoint_hold(asoc->ep);
	sock_hold(asoc->base.sk);

	/* Initialize the common base substructure.  */
	asoc->base.type = SCTP_EP_TYPE_ASSOCIATION;

	/* Initialize the object handling fields.  */
	refcount_set(&asoc->base.refcnt, 1);

	/* Initialize the bind addr area.  */
	sctp_bind_addr_init(&asoc->base.bind_addr, ep->base.bind_addr.port);

	asoc->state = SCTP_STATE_CLOSED;
	asoc->cookie_life = ms_to_ktime(sp->assocparams.sasoc_cookie_life);
	asoc->user_frag = sp->user_frag;

	/* Set the association max_retrans and RTO values from the
	 * socket values.
	 */
	asoc->max_retrans = sp->assocparams.sasoc_asocmaxrxt;
	asoc->pf_retrans  = sp->pf_retrans;
	asoc->ps_retrans  = sp->ps_retrans;
	asoc->pf_expose   = sp->pf_expose;

	asoc->rto_initial = msecs_to_jiffies(sp->rtoinfo.srto_initial);
	asoc->rto_max = msecs_to_jiffies(sp->rtoinfo.srto_max);
	asoc->rto_min = msecs_to_jiffies(sp->rtoinfo.srto_min);

	/* Initialize the association's heartbeat interval based on the
	 * sock configured value.
	 */
	asoc->hbinterval = msecs_to_jiffies(sp->hbinterval);

	asoc->encap_port = sp->encap_port;

	/* Initialize path max retrans value. */
	asoc->pathmaxrxt = sp->pathmaxrxt;

	asoc->flowlabel = sp->flowlabel;
	asoc->dscp = sp->dscp;

	/* Set association default SACK delay */
	asoc->sackdelay = msecs_to_jiffies(sp->sackdelay);
	asoc->sackfreq = sp->sackfreq;

	/* Set the association default flags controlling
	 * Heartbeat, SACK delay, and Path MTU Discovery.
	 */
	asoc->param_flags = sp->param_flags;

	/* Initialize the maximum number of new data packets that can be sent
	 * in a burst.
	 */
	asoc->max_burst = sp->max_burst;

	asoc->subscribe = sp->subscribe;

	/* initialize association timers */
	asoc->timeouts[SCTP_EVENT_TIMEOUT_T1_COOKIE] = asoc->rto_initial;
	asoc->timeouts[SCTP_EVENT_TIMEOUT_T1_INIT] = asoc->rto_initial;
	asoc->timeouts[SCTP_EVENT_TIMEOUT_T2_SHUTDOWN] = asoc->rto_initial;

	/* sctpimpguide Section 2.12.2
	 * If the 'T5-shutdown-guard' timer is used, it SHOULD be set to the
	 * recommended value of 5 times 'RTO.Max'.
	 */
	asoc->timeouts[SCTP_EVENT_TIMEOUT_T5_SHUTDOWN_GUARD]
		= 5 * asoc->rto_max;

	asoc->timeouts[SCTP_EVENT_TIMEOUT_SACK] = asoc->sackdelay;
	asoc->timeouts[SCTP_EVENT_TIMEOUT_AUTOCLOSE] = sp->autoclose * HZ;

	/* Initializes the timers */
	for (i = SCTP_EVENT_TIMEOUT_NONE; i < SCTP_NUM_TIMEOUT_TYPES; ++i)
		timer_setup(&asoc->timers[i], sctp_timer_events[i], 0);

	/* Pull default initialization values from the sock options.
	 * Note: This assumes that the values have already been
	 * validated in the sock.
	 */
	asoc->c.sinit_max_instreams = sp->initmsg.sinit_max_instreams;
	asoc->c.sinit_num_ostreams  = sp->initmsg.sinit_num_ostreams;
	asoc->max_init_attempts	= sp->initmsg.sinit_max_attempts;

	asoc->max_init_timeo =
		 msecs_to_jiffies(sp->initmsg.sinit_max_init_timeo);

	/* Set the local window size for receive.
	 * This is also the rcvbuf space per association.
	 * RFC 6 - A SCTP receiver MUST be able to receive a minimum of
	 * 1500 bytes in one SCTP packet.
	 */
	if ((sk->sk_rcvbuf/2) < SCTP_DEFAULT_MINWINDOW)
		asoc->rwnd = SCTP_DEFAULT_MINWINDOW;
	else
		asoc->rwnd = sk->sk_rcvbuf/2;

	asoc->a_rwnd = asoc->rwnd;

	/* Use my own max window until I learn something better.  */
	asoc->peer.rwnd = SCTP_DEFAULT_MAXWINDOW;

	/* Initialize the receive memory counter */
	atomic_set(&asoc->rmem_alloc, 0);

	init_waitqueue_head(&asoc->wait);

	asoc->c.my_vtag = sctp_generate_tag(ep);
	asoc->c.my_port = ep->base.bind_addr.port;

	asoc->c.initial_tsn = sctp_generate_tsn(ep);

	asoc->next_tsn = asoc->c.initial_tsn;

	asoc->ctsn_ack_point = asoc->next_tsn - 1;
	asoc->adv_peer_ack_point = asoc->ctsn_ack_point;
	asoc->highest_sacked = asoc->ctsn_ack_point;
	asoc->last_cwr_tsn = asoc->ctsn_ack_point;

	/* ADDIP Section 4.1 Asconf Chunk Procedures
	 *
	 * When an endpoint has an ASCONF signaled change to be sent to the
	 * remote endpoint it should do the following:
	 * ...
	 * A2) a serial number should be assigned to the chunk. The serial
	 * number SHOULD be a monotonically increasing number. The serial
	 * numbers SHOULD be initialized at the start of the
	 * association to the same value as the initial TSN.
	 */
	asoc->addip_serial = asoc->c.initial_tsn;
	asoc->strreset_outseq = asoc->c.initial_tsn;

	INIT_LIST_HEAD(&asoc->addip_chunk_list);
	INIT_LIST_HEAD(&asoc->asconf_ack_list);

	/* Make an empty list of remote transport addresses.  */
	INIT_LIST_HEAD(&asoc->peer.transport_addr_list);

	/* RFC 2960 5.1 Normal Establishment of an Association
	 *
	 * After the reception of the first data chunk in an
	 * association the endpoint must immediately respond with a
	 * sack to acknowledge the data chunk.  Subsequent
	 * acknowledgements should be done as described in Section
	 * 6.2.
	 *
	 * [We implement this by telling a new association that it
	 * already received one packet.]
	 */
	asoc->peer.sack_needed = 1;
	asoc->peer.sack_generation = 1;

	/* Create an input queue.  */
	sctp_inq_init(&asoc->base.inqueue);
	sctp_inq_set_th_handler(&asoc->base.inqueue, sctp_assoc_bh_rcv);

	/* Create an output queue.  */
	sctp_outq_init(asoc, &asoc->outqueue);

	if (!sctp_ulpq_init(&asoc->ulpq, asoc))
		goto fail_init;

	if (sctp_stream_init(&asoc->stream, asoc->c.sinit_num_ostreams,
			     0, gfp))
		goto fail_init;

	/* Initialize default path MTU. */
	asoc->pathmtu = sp->pathmtu;
	sctp_assoc_update_frag_point(asoc);

	/* Assume that peer would support both address types unless we are
	 * told otherwise.
	 */
	asoc->peer.ipv4_address = 1;
	if (asoc->base.sk->sk_family == PF_INET6)
		asoc->peer.ipv6_address = 1;
	INIT_LIST_HEAD(&asoc->asocs);

	asoc->default_stream = sp->default_stream;
	asoc->default_ppid = sp->default_ppid;
	asoc->default_flags = sp->default_flags;
	asoc->default_context = sp->default_context;
	asoc->default_timetolive = sp->default_timetolive;
	asoc->default_rcv_context = sp->default_rcv_context;

	/* AUTH related initializations */
	INIT_LIST_HEAD(&asoc->endpoint_shared_keys);
	if (sctp_auth_asoc_copy_shkeys(ep, asoc, gfp))
		goto stream_free;

	asoc->active_key_id = ep->active_key_id;
	asoc->strreset_enable = ep->strreset_enable;

	/* Save the hmacs and chunks list into this association */
	if (ep->auth_hmacs_list)
		memcpy(asoc->c.auth_hmacs, ep->auth_hmacs_list,
			ntohs(ep->auth_hmacs_list->param_hdr.length));
	if (ep->auth_chunk_list)
		memcpy(asoc->c.auth_chunks, ep->auth_chunk_list,
			ntohs(ep->auth_chunk_list->param_hdr.length));

	/* Get the AUTH random number for this association */
	p = (struct sctp_paramhdr *)asoc->c.auth_random;
	p->type = SCTP_PARAM_RANDOM;
	p->length = htons(sizeof(*p) + SCTP_AUTH_RANDOM_LENGTH);
	get_random_bytes(p+1, SCTP_AUTH_RANDOM_LENGTH);

	return asoc;

stream_free:
	sctp_stream_free(&asoc->stream);
fail_init:
	sock_put(asoc->base.sk);
	sctp_endpoint_put(asoc->ep);
	return NULL;
}

/* Allocate and initialize a new association */
struct sctp_association *sctp_association_new(const struct sctp_endpoint *ep,
					      const struct sock *sk,
					      enum sctp_scope scope, gfp_t gfp)
{
	struct sctp_association *asoc;

	asoc = kzalloc(sizeof(*asoc), gfp);
	if (!asoc)
		goto fail;

	if (!sctp_association_init(asoc, ep, sk, scope, gfp))
		goto fail_init;

	SCTP_DBG_OBJCNT_INC(assoc);

	pr_debug("Created asoc %p\n", asoc);

	return asoc;

fail_init:
	kfree(asoc);
fail:
	return NULL;
}

/* Free this association if possible.  There may still be users, so
 * the actual deallocation may be delayed.
 */
void sctp_association_free(struct sctp_association *asoc)
{
	struct sock *sk = asoc->base.sk;
	struct sctp_transport *transport;
	struct list_head *pos, *temp;
	int i;

	/* Only real associations count against the endpoint, so
	 * don't bother for if this is a temporary association.
	 */
	if (!list_empty(&asoc->asocs)) {
		list_del(&asoc->asocs);

		/* Decrement the backlog value for a TCP-style listening
		 * socket.
		 */
		if (sctp_style(sk, TCP) && sctp_sstate(sk, LISTENING))
			sk_acceptq_removed(sk);
	}

	/* Mark as dead, so other users can know this structure is
	 * going away.
	 */
	asoc->base.dead = true;

	/* Dispose of any data lying around in the outqueue. */
	sctp_outq_free(&asoc->outqueue);

	/* Dispose of any pending messages for the upper layer. */
	sctp_ulpq_free(&asoc->ulpq);

	/* Dispose of any pending chunks on the inqueue. */
	sctp_inq_free(&asoc->base.inqueue);

	sctp_tsnmap_free(&asoc->peer.tsn_map);

	/* Free stream information. */
	sctp_stream_free(&asoc->stream);

	if (asoc->strreset_chunk)
		sctp_chunk_free(asoc->strreset_chunk);

	/* Clean up the bound address list. */
	sctp_bind_addr_free(&asoc->base.bind_addr);

	/* Do we need to go through all of our timers and
	 * delete them?   To be safe we will try to delete all, but we
	 * should be able to go through and make a guess based
	 * on our state.
	 */
	for (i = SCTP_EVENT_TIMEOUT_NONE; i < SCTP_NUM_TIMEOUT_TYPES; ++i) {
		if (del_timer(&asoc->timers[i]))
			sctp_association_put(asoc);
	}

	/* Free peer's cached cookie. */
	kfree(asoc->peer.cookie);
	kfree(asoc->peer.peer_random);
	kfree(asoc->peer.peer_chunks);
	kfree(asoc->peer.peer_hmacs);

	/* Release the transport structures. */
	list_for_each_safe(pos, temp, &asoc->peer.transport_addr_list) {
		transport = list_entry(pos, struct sctp_transport, transports);
		list_del_rcu(pos);
		sctp_unhash_transport(transport);
		sctp_transport_free(transport);
	}

	asoc->peer.transport_count = 0;

	sctp_asconf_queue_teardown(asoc);

	/* Free pending address space being deleted */
	kfree(asoc->asconf_addr_del_pending);

	/* AUTH - Free the endpoint shared keys */
	sctp_auth_destroy_keys(&asoc->endpoint_shared_keys);

	/* AUTH - Free the association shared key */
	sctp_auth_key_put(asoc->asoc_shared_key);

	sctp_association_put(asoc);
}

/* Cleanup and free up an association. */
static void sctp_association_destroy(struct sctp_association *asoc)
{
	if (unlikely(!asoc->base.dead)) {
		WARN(1, "Attempt to destroy undead association %p!\n", asoc);
		return;
	}

	sctp_endpoint_put(asoc->ep);
	sock_put(asoc->base.sk);

	if (asoc->assoc_id != 0) {
		spin_lock_bh(&sctp_assocs_id_lock);
		idr_remove(&sctp_assocs_id, asoc->assoc_id);
		spin_unlock_bh(&sctp_assocs_id_lock);
	}

	WARN_ON(atomic_read(&asoc->rmem_alloc));

	kfree_rcu(asoc, rcu);
	SCTP_DBG_OBJCNT_DEC(assoc);
}

/* Change the primary destination address for the peer. */
void sctp_assoc_set_primary(struct sctp_association *asoc,
			    struct sctp_transport *transport)
{
	int changeover = 0;

	/* it's a changeover only if we already have a primary path
	 * that we are changing
	 */
	if (asoc->peer.primary_path != NULL &&
	    asoc->peer.primary_path != transport)
		changeover = 1 ;

	asoc->peer.primary_path = transport;
	sctp_ulpevent_notify_peer_addr_change(transport,
					      SCTP_ADDR_MADE_PRIM, 0);

	/* Set a default msg_name for events. */
	memcpy(&asoc->peer.primary_addr, &transport->ipaddr,
	       sizeof(union sctp_addr));

	/* If the primary path is changing, assume that the
	 * user wants to use this new path.
	 */
	if ((transport->state == SCTP_ACTIVE) ||
	    (transport->state == SCTP_UNKNOWN))
		asoc->peer.active_path = transport;

	/*
	 * SFR-CACC algorithm:
	 * Upon the receipt of a request to change the primary
	 * destination address, on the data structure for the new
	 * primary destination, the sender MUST do the following:
	 *
	 * 1) If CHANGEOVER_ACTIVE is set, then there was a switch
	 * to this destination address earlier. The sender MUST set
	 * CYCLING_CHANGEOVER to indicate that this switch is a
	 * double switch to the same destination address.
	 *
	 * Really, only bother is we have data queued or outstanding on
	 * the association.
	 */
	if (!asoc->outqueue.outstanding_bytes && !asoc->outqueue.out_qlen)
		return;

	if (transport->cacc.changeover_active)
		transport->cacc.cycling_changeover = changeover;

	/* 2) The sender MUST set CHANGEOVER_ACTIVE to indicate that
	 * a changeover has occurred.
	 */
	transport->cacc.changeover_active = changeover;

	/* 3) The sender MUST store the next TSN to be sent in
	 * next_tsn_at_change.
	 */
	transport->cacc.next_tsn_at_change = asoc->next_tsn;
}

/* Remove a transport from an association.  */
void sctp_assoc_rm_peer(struct sctp_association *asoc,
			struct sctp_transport *peer)
{
	struct sctp_transport *transport;
	struct list_head *pos;
	struct sctp_chunk *ch;

	pr_debug("%s: association:%p addr:%pISpc\n",
		 __func__, asoc, &peer->ipaddr.sa);

	/* If we are to remove the current retran_path, update it
	 * to the next peer before removing this peer from the list.
	 */
	if (asoc->peer.retran_path == peer)
		sctp_assoc_update_retran_path(asoc);

	/* Remove this peer from the list. */
	list_del_rcu(&peer->transports);
	/* Remove this peer from the transport hashtable */
	sctp_unhash_transport(peer);

	/* Get the first transport of asoc. */
	pos = asoc->peer.transport_addr_list.next;
	transport = list_entry(pos, struct sctp_transport, transports);

	/* Update any entries that match the peer to be deleted. */
	if (asoc->peer.primary_path == peer)
		sctp_assoc_set_primary(asoc, transport);
	if (asoc->peer.active_path == peer)
		asoc->peer.active_path = transport;
	if (asoc->peer.retran_path == peer)
		asoc->peer.retran_path = transport;
	if (asoc->peer.last_data_from == peer)
		asoc->peer.last_data_from = transport;

	if (asoc->strreset_chunk &&
	    asoc->strreset_chunk->transport == peer) {
		asoc->strreset_chunk->transport = transport;
		sctp_transport_reset_reconf_timer(transport);
	}

	/* If we remove the transport an INIT was last sent to, set it to
	 * NULL. Combined with the update of the retran path above, this
	 * will cause the next INIT to be sent to the next available
	 * transport, maintaining the cycle.
	 */
	if (asoc->init_last_sent_to == peer)
		asoc->init_last_sent_to = NULL;

	/* If we remove the transport an SHUTDOWN was last sent to, set it
	 * to NULL. Combined with the update of the retran path above, this
	 * will cause the next SHUTDOWN to be sent to the next available
	 * transport, maintaining the cycle.
	 */
	if (asoc->shutdown_last_sent_to == peer)
		asoc->shutdown_last_sent_to = NULL;

	/* If we remove the transport an ASCONF was last sent to, set it to
	 * NULL.
	 */
	if (asoc->addip_last_asconf &&
	    asoc->addip_last_asconf->transport == peer)
		asoc->addip_last_asconf->transport = NULL;

	/* If we have something on the transmitted list, we have to
	 * save it off.  The best place is the active path.
	 */
	if (!list_empty(&peer->transmitted)) {
		struct sctp_transport *active = asoc->peer.active_path;

		/* Reset the transport of each chunk on this list */
		list_for_each_entry(ch, &peer->transmitted,
					transmitted_list) {
			ch->transport = NULL;
			ch->rtt_in_progress = 0;
		}

		list_splice_tail_init(&peer->transmitted,
					&active->transmitted);

		/* Start a T3 timer here in case it wasn't running so
		 * that these migrated packets have a chance to get
		 * retransmitted.
		 */
		if (!timer_pending(&active->T3_rtx_timer))
			if (!mod_timer(&active->T3_rtx_timer,
					jiffies + active->rto))
				sctp_transport_hold(active);
	}

	list_for_each_entry(ch, &asoc->outqueue.out_chunk_list, list)
		if (ch->transport == peer)
			ch->transport = NULL;

	asoc->peer.transport_count--;

	sctp_ulpevent_notify_peer_addr_change(peer, SCTP_ADDR_REMOVED, 0);
	sctp_transport_free(peer);
}

/* Add a transport address to an association.  */
struct sctp_transport *sctp_assoc_add_peer(struct sctp_association *asoc,
					   const union sctp_addr *addr,
					   const gfp_t gfp,
					   const int peer_state)
{
	struct sctp_transport *peer;
	struct sctp_sock *sp;
	unsigned short port;

	sp = sctp_sk(asoc->base.sk);

	/* AF_INET and AF_INET6 share common port field. */
	port = ntohs(addr->v4.sin_port);

	pr_debug("%s: association:%p addr:%pISpc state:%d\n", __func__,
		 asoc, &addr->sa, peer_state);

	/* Set the port if it has not been set yet.  */
	if (0 == asoc->peer.port)
		asoc->peer.port = port;

	/* Check to see if this is a duplicate. */
	peer = sctp_assoc_lookup_paddr(asoc, addr);
	if (peer) {
		/* An UNKNOWN state is only set on transports added by
		 * user in sctp_connectx() call.  Such transports should be
		 * considered CONFIRMED per RFC 4960, Section 5.4.
		 */
		if (peer->state == SCTP_UNKNOWN) {
			peer->state = SCTP_ACTIVE;
		}
		return peer;
	}

	peer = sctp_transport_new(asoc->base.net, addr, gfp);
	if (!peer)
		return NULL;

	sctp_transport_set_owner(peer, asoc);

	/* Initialize the peer's heartbeat interval based on the
	 * association configured value.
	 */
	peer->hbinterval = asoc->hbinterval;

	peer->encap_port = asoc->encap_port;

	/* Set the path max_retrans.  */
	peer->pathmaxrxt = asoc->pathmaxrxt;

	/* And the partial failure retrans threshold */
	peer->pf_retrans = asoc->pf_retrans;
	/* And the primary path switchover retrans threshold */
	peer->ps_retrans = asoc->ps_retrans;

	/* Initialize the peer's SACK delay timeout based on the
	 * association configured value.
	 */
	peer->sackdelay = asoc->sackdelay;
	peer->sackfreq = asoc->sackfreq;

	if (addr->sa.sa_family == AF_INET6) {
		__be32 info = addr->v6.sin6_flowinfo;

		if (info) {
			peer->flowlabel = ntohl(info & IPV6_FLOWLABEL_MASK);
			peer->flowlabel |= SCTP_FLOWLABEL_SET_MASK;
		} else {
			peer->flowlabel = asoc->flowlabel;
		}
	}
	peer->dscp = asoc->dscp;

	/* Enable/disable heartbeat, SACK delay, and path MTU discovery
	 * based on association setting.
	 */
	peer->param_flags = asoc->param_flags;

	/* Initialize the pmtu of the transport. */
	sctp_transport_route(peer, NULL, sp);

	/* If this is the first transport addr on this association,
	 * initialize the association PMTU to the peer's PMTU.
	 * If not and the current association PMTU is higher than the new
	 * peer's PMTU, reset the association PMTU to the new peer's PMTU.
	 */
	sctp_assoc_set_pmtu(asoc, asoc->pathmtu ?
				  min_t(int, peer->pathmtu, asoc->pathmtu) :
				  peer->pathmtu);

	peer->pmtu_pending = 0;

	/* The asoc->peer.port might not be meaningful yet, but
	 * initialize the packet structure anyway.
	 */
	sctp_packet_init(&peer->packet, peer, asoc->base.bind_addr.port,
			 asoc->peer.port);

	/* 7.2.1 Slow-Start
	 *
	 * o The initial cwnd before DATA transmission or after a sufficiently
	 *   long idle period MUST be set to
	 *      min(4*MTU, max(2*MTU, 4380 bytes))
	 *
	 * o The initial value of ssthresh MAY be arbitrarily high
	 *   (for example, implementations MAY use the size of the
	 *   receiver advertised window).
	 */
	peer->cwnd = min(4*asoc->pathmtu, max_t(__u32, 2*asoc->pathmtu, 4380));

	/* At this point, we may not have the receiver's advertised window,
	 * so initialize ssthresh to the default value and it will be set
	 * later when we process the INIT.
	 */
	peer->ssthresh = SCTP_DEFAULT_MAXWINDOW;

	peer->partial_bytes_acked = 0;
	peer->flight_size = 0;
	peer->burst_limited = 0;

	/* Set the transport's RTO.initial value */
	peer->rto = asoc->rto_initial;
	sctp_max_rto(asoc, peer);

	/* Set the peer's active state. */
	peer->state = peer_state;

	/* Add this peer into the transport hashtable */
	if (sctp_hash_transport(peer)) {
		sctp_transport_free(peer);
		return NULL;
	}

	/* Attach the remote transport to our asoc.  */
	list_add_tail_rcu(&peer->transports, &asoc->peer.transport_addr_list);
	asoc->peer.transport_count++;

	sctp_ulpevent_notify_peer_addr_change(peer, SCTP_ADDR_ADDED, 0);

	/* If we do not yet have a primary path, set one.  */
	if (!asoc->peer.primary_path) {
		sctp_assoc_set_primary(asoc, peer);
		asoc->peer.retran_path = peer;
	}

	if (asoc->peer.active_path == asoc->peer.retran_path &&
	    peer->state != SCTP_UNCONFIRMED) {
		asoc->peer.retran_path = peer;
	}

	return peer;
}

/* Delete a transport address from an association.  */
void sctp_assoc_del_peer(struct sctp_association *asoc,
			 const union sctp_addr *addr)
{
	struct list_head	*pos;
	struct list_head	*temp;
	struct sctp_transport	*transport;

	list_for_each_safe(pos, temp, &asoc->peer.transport_addr_list) {
		transport = list_entry(pos, struct sctp_transport, transports);
		if (sctp_cmp_addr_exact(addr, &transport->ipaddr)) {
			/* Do book keeping for removing the peer and free it. */
			sctp_assoc_rm_peer(asoc, transport);
			break;
		}
	}
}

/* Lookup a transport by address. */
struct sctp_transport *sctp_assoc_lookup_paddr(
					const struct sctp_association *asoc,
					const union sctp_addr *address)
{
	struct sctp_transport *t;

	/* Cycle through all transports searching for a peer address. */

	list_for_each_entry(t, &asoc->peer.transport_addr_list,
			transports) {
		if (sctp_cmp_addr_exact(address, &t->ipaddr))
			return t;
	}

	return NULL;
}

/* Remove all transports except a give one */
void sctp_assoc_del_nonprimary_peers(struct sctp_association *asoc,
				     struct sctp_transport *primary)
{
	struct sctp_transport	*temp;
	struct sctp_transport	*t;

	list_for_each_entry_safe(t, temp, &asoc->peer.transport_addr_list,
				 transports) {
		/* if the current transport is not the primary one, delete it */
		if (t != primary)
			sctp_assoc_rm_peer(asoc, t);
	}
}

/* Engage in transport control operations.
 * Mark the transport up or down and send a notification to the user.
 * Select and update the new active and retran paths.
 */
void sctp_assoc_control_transport(struct sctp_association *asoc,
				  struct sctp_transport *transport,
				  enum sctp_transport_cmd command,
				  sctp_sn_error_t error)
{
	int spc_state = SCTP_ADDR_AVAILABLE;
	bool ulp_notify = true;

	/* Record the transition on the transport.  */
	switch (command) {
	case SCTP_TRANSPORT_UP:
		/* If we are moving from UNCONFIRMED state due
		 * to heartbeat success, report the SCTP_ADDR_CONFIRMED
		 * state to the user, otherwise report SCTP_ADDR_AVAILABLE.
		 */
		if (transport->state == SCTP_PF &&
		    asoc->pf_expose != SCTP_PF_EXPOSE_ENABLE)
			ulp_notify = false;
		else if (transport->state == SCTP_UNCONFIRMED &&
			 error == SCTP_HEARTBEAT_SUCCESS)
			spc_state = SCTP_ADDR_CONFIRMED;

		transport->state = SCTP_ACTIVE;
		break;

	case SCTP_TRANSPORT_DOWN:
		/* If the transport was never confirmed, do not transition it
		 * to inactive state.  Also, release the cached route since
		 * there may be a better route next time.
		 */
		if (transport->state != SCTP_UNCONFIRMED) {
			transport->state = SCTP_INACTIVE;
			spc_state = SCTP_ADDR_UNREACHABLE;
		} else {
			sctp_transport_dst_release(transport);
			ulp_notify = false;
		}
		break;

	case SCTP_TRANSPORT_PF:
		transport->state = SCTP_PF;
		if (asoc->pf_expose != SCTP_PF_EXPOSE_ENABLE)
			ulp_notify = false;
		else
			spc_state = SCTP_ADDR_POTENTIALLY_FAILED;
		break;

	default:
		return;
	}

	/* Generate and send a SCTP_PEER_ADDR_CHANGE notification
	 * to the user.
	 */
	if (ulp_notify)
		sctp_ulpevent_notify_peer_addr_change(transport,
						      spc_state, error);

	/* Select new active and retran paths. */
	sctp_select_active_and_retran_path(asoc);
}

/* Hold a reference to an association. */
void sctp_association_hold(struct sctp_association *asoc)
{
	refcount_inc(&asoc->base.refcnt);
}

/* Release a reference to an association and cleanup
 * if there are no more references.
 */
void sctp_association_put(struct sctp_association *asoc)
{
	if (refcount_dec_and_test(&asoc->base.refcnt))
		sctp_association_destroy(asoc);
}

/* Allocate the next TSN, Transmission Sequence Number, for the given
 * association.
 */
__u32 sctp_association_get_next_tsn(struct sctp_association *asoc)
{
	/* From Section 1.6 Serial Number Arithmetic:
	 * Transmission Sequence Numbers wrap around when they reach
	 * 2**32 - 1.  That is, the next TSN a DATA chunk MUST use
	 * after transmitting TSN = 2*32 - 1 is TSN = 0.
	 */
	__u32 retval = asoc->next_tsn;
	asoc->next_tsn++;
	asoc->unack_data++;

	return retval;
}

/* Compare two addresses to see if they match.  Wildcard addresses
 * only match themselves.
 */
int sctp_cmp_addr_exact(const union sctp_addr *ss1,
			const union sctp_addr *ss2)
{
	struct sctp_af *af;

	af = sctp_get_af_specific(ss1->sa.sa_family);
	if (unlikely(!af))
		return 0;

	return af->cmp_addr(ss1, ss2);
}

/* Return an ecne chunk to get prepended to a packet.
 * Note:  We are sly and return a shared, prealloced chunk.  FIXME:
 * No we don't, but we could/should.
 */
struct sctp_chunk *sctp_get_ecne_prepend(struct sctp_association *asoc)
{
	if (!asoc->need_ecne)
		return NULL;

	/* Send ECNE if needed.
	 * Not being able to allocate a chunk here is not deadly.
	 */
	return sctp_make_ecne(asoc, asoc->last_ecne_tsn);
}

/*
 * Find which transport this TSN was sent on.
 */
struct sctp_transport *sctp_assoc_lookup_tsn(struct sctp_association *asoc,
					     __u32 tsn)
{
	struct sctp_transport *active;
	struct sctp_transport *match;
	struct sctp_transport *transport;
	struct sctp_chunk *chunk;
	__be32 key = htonl(tsn);

	match = NULL;

	/*
	 * FIXME: In general, find a more efficient data structure for
	 * searching.
	 */

	/*
	 * The general strategy is to search each transport's transmitted
	 * list.   Return which transport this TSN lives on.
	 *
	 * Let's be hopeful and check the active_path first.
	 * Another optimization would be to know if there is only one
	 * outbound path and not have to look for the TSN at all.
	 *
	 */

	active = asoc->peer.active_path;

	list_for_each_entry(chunk, &active->transmitted,
			transmitted_list) {

		if (key == chunk->subh.data_hdr->tsn) {
			match = active;
			goto out;
		}
	}

	/* If not found, go search all the other transports. */
	list_for_each_entry(transport, &asoc->peer.transport_addr_list,
			transports) {

		if (transport == active)
			continue;
		list_for_each_entry(chunk, &transport->transmitted,
				transmitted_list) {
			if (key == chunk->subh.data_hdr->tsn) {
				match = transport;
				goto out;
			}
		}
	}
out:
	return match;
}

/* Do delayed input processing.  This is scheduled by sctp_rcv(). */
static void sctp_assoc_bh_rcv(struct work_struct *work)
{
	struct sctp_association *asoc =
		container_of(work, struct sctp_association,
			     base.inqueue.immediate);
	struct net *net = asoc->base.net;
	union sctp_subtype subtype;
	struct sctp_endpoint *ep;
	struct sctp_chunk *chunk;
	struct sctp_inq *inqueue;
	int first_time = 1;	/* is this the first time through the loop */
	int error = 0;
	int state;

	/* The association should be held so we should be safe. */
	ep = asoc->ep;

	inqueue = &asoc->base.inqueue;
	sctp_association_hold(asoc);
	while (NULL != (chunk = sctp_inq_pop(inqueue))) {
		state = asoc->state;
		subtype = SCTP_ST_CHUNK(chunk->chunk_hdr->type);

		/* If the first chunk in the packet is AUTH, do special
		 * processing specified in Section 6.3 of SCTP-AUTH spec
		 */
		if (first_time && subtype.chunk == SCTP_CID_AUTH) {
			struct sctp_chunkhdr *next_hdr;

			next_hdr = sctp_inq_peek(inqueue);
			if (!next_hdr)
				goto normal;

			/* If the next chunk is COOKIE-ECHO, skip the AUTH
			 * chunk while saving a pointer to it so we can do
			 * Authentication later (during cookie-echo
			 * processing).
			 */
			if (next_hdr->type == SCTP_CID_COOKIE_ECHO) {
				chunk->auth_chunk = skb_clone(chunk->skb,
							      GFP_ATOMIC);
				chunk->auth = 1;
				continue;
			}
		}

normal:
		/* SCTP-AUTH, Section 6.3:
		 *    The receiver has a list of chunk types which it expects
		 *    to be received only after an AUTH-chunk.  This list has
		 *    been sent to the peer during the association setup.  It
		 *    MUST silently discard these chunks if they are not placed
		 *    after an AUTH chunk in the packet.
		 */
		if (sctp_auth_recv_cid(subtype.chunk, asoc) && !chunk->auth)
			continue;

		/* Remember where the last DATA chunk came from so we
		 * know where to send the SACK.
		 */
		if (sctp_chunk_is_data(chunk))
			asoc->peer.last_data_from = chunk->transport;
		else {
			SCTP_INC_STATS(net, SCTP_MIB_INCTRLCHUNKS);
			asoc->stats.ictrlchunks++;
			if (chunk->chunk_hdr->type == SCTP_CID_SACK)
				asoc->stats.isacks++;
		}

		if (chunk->transport)
			chunk->transport->last_time_heard = ktime_get();

		/* Run through the state machine. */
		error = sctp_do_sm(net, SCTP_EVENT_T_CHUNK, subtype,
				   state, ep, asoc, chunk, GFP_ATOMIC);

		/* Check to see if the association is freed in response to
		 * the incoming chunk.  If so, get out of the while loop.
		 */
		if (asoc->base.dead)
			break;

		/* If there is an error on chunk, discard this packet. */
		if (error && chunk)
			chunk->pdiscard = 1;

		if (first_time)
			first_time = 0;
	}
	sctp_association_put(asoc);
}

/* This routine moves an association from its old sk to a new sk.  */
void sctp_assoc_migrate(struct sctp_association *assoc, struct sock *newsk)
{
	struct sctp_sock *newsp = sctp_sk(newsk);
	struct sock *oldsk = assoc->base.sk;

	/* Delete the association from the old endpoint's list of
	 * associations.
	 */
	list_del_init(&assoc->asocs);

	/* Decrement the backlog value for a TCP-style socket. */
	if (sctp_style(oldsk, TCP))
		sk_acceptq_removed(oldsk);

	/* Release references to the old endpoint and the sock.  */
	sctp_endpoint_put(assoc->ep);
	sock_put(assoc->base.sk);

	/* Get a reference to the new endpoint.  */
	assoc->ep = newsp->ep;
	sctp_endpoint_hold(assoc->ep);

	/* Get a reference to the new sock.  */
	assoc->base.sk = newsk;
	sock_hold(assoc->base.sk);

	/* Add the association to the new endpoint's list of associations.  */
	sctp_endpoint_add_asoc(newsp->ep, assoc);
}

/* Update an association (possibly from unexpected COOKIE-ECHO processing).  */
int sctp_assoc_update(struct sctp_association *asoc,
		      struct sctp_association *new)
{
	struct sctp_transport *trans;
	struct list_head *pos, *temp;

	/* Copy in new parameters of peer. */
	asoc->c = new->c;
	asoc->peer.rwnd = new->peer.rwnd;
	asoc->peer.sack_needed = new->peer.sack_needed;
	asoc->peer.auth_capable = new->peer.auth_capable;
	asoc->peer.i = new->peer.i;

	if (!sctp_tsnmap_init(&asoc->peer.tsn_map, SCTP_TSN_MAP_INITIAL,
			      asoc->peer.i.initial_tsn, GFP_ATOMIC))
		return -ENOMEM;

	/* Remove any peer addresses not present in the new association. */
	list_for_each_safe(pos, temp, &asoc->peer.transport_addr_list) {
		trans = list_entry(pos, struct sctp_transport, transports);
		if (!sctp_assoc_lookup_paddr(new, &trans->ipaddr)) {
			sctp_assoc_rm_peer(asoc, trans);
			continue;
		}

		if (asoc->state >= SCTP_STATE_ESTABLISHED)
			sctp_transport_reset(trans);
	}

	/* If the case is A (association restart), use
	 * initial_tsn as next_tsn. If the case is B, use
	 * current next_tsn in case data sent to peer
	 * has been discarded and needs retransmission.
	 */
	if (asoc->state >= SCTP_STATE_ESTABLISHED) {
		asoc->next_tsn = new->next_tsn;
		asoc->ctsn_ack_point = new->ctsn_ack_point;
		asoc->adv_peer_ack_point = new->adv_peer_ack_point;

		/* Reinitialize SSN for both local streams
		 * and peer's streams.
		 */
		sctp_stream_clear(&asoc->stream);

		/* Flush the ULP reassembly and ordered queue.
		 * Any data there will now be stale and will
		 * cause problems.
		 */
		sctp_ulpq_flush(&asoc->ulpq);

		/* reset the overall association error count so
		 * that the restarted association doesn't get torn
		 * down on the next retransmission timer.
		 */
		asoc->overall_error_count = 0;

	} else {
		/* Add any peer addresses from the new association. */
		list_for_each_entry(trans, &new->peer.transport_addr_list,
				    transports)
			if (!sctp_assoc_lookup_paddr(asoc, &trans->ipaddr) &&
			    !sctp_assoc_add_peer(asoc, &trans->ipaddr,
						 GFP_ATOMIC, trans->state))
				return -ENOMEM;

		asoc->ctsn_ack_point = asoc->next_tsn - 1;
		asoc->adv_peer_ack_point = asoc->ctsn_ack_point;

		if (sctp_state(asoc, COOKIE_WAIT))
			sctp_stream_update(&asoc->stream, &new->stream);

		/* get a new assoc id if we don't have one yet. */
		if (sctp_assoc_set_id(asoc, GFP_ATOMIC))
			return -ENOMEM;
	}

	/* SCTP-AUTH: Save the peer parameters from the new associations
	 * and also move the association shared keys over
	 */
	kfree(asoc->peer.peer_random);
	asoc->peer.peer_random = new->peer.peer_random;
	new->peer.peer_random = NULL;

	kfree(asoc->peer.peer_chunks);
	asoc->peer.peer_chunks = new->peer.peer_chunks;
	new->peer.peer_chunks = NULL;

	kfree(asoc->peer.peer_hmacs);
	asoc->peer.peer_hmacs = new->peer.peer_hmacs;
	new->peer.peer_hmacs = NULL;

	return sctp_auth_asoc_init_active_key(asoc, GFP_ATOMIC);
}

/* Update the retran path for sending a retransmitted packet.
 * See also RFC4960, 6.4. Multi-Homed SCTP Endpoints:
 *
 *   When there is outbound data to send and the primary path
 *   becomes inactive (e.g., due to failures), or where the
 *   SCTP user explicitly requests to send data to an
 *   inactive destination transport address, before reporting
 *   an error to its ULP, the SCTP endpoint should try to send
 *   the data to an alternate active destination transport
 *   address if one exists.
 *
 *   When retransmitting data that timed out, if the endpoint
 *   is multihomed, it should consider each source-destination
 *   address pair in its retransmission selection policy.
 *   When retransmitting timed-out data, the endpoint should
 *   attempt to pick the most divergent source-destination
 *   pair from the original source-destination pair to which
 *   the packet was transmitted.
 *
 *   Note: Rules for picking the most divergent source-destination
 *   pair are an implementation decision and are not specified
 *   within this document.
 *
 * Our basic strategy is to round-robin transports in priorities
 * according to sctp_trans_score() e.g., if no such
 * transport with state SCTP_ACTIVE exists, round-robin through
 * SCTP_UNKNOWN, etc. You get the picture.
 */
static u8 sctp_trans_score(const struct sctp_transport *trans)
{
	switch (trans->state) {
	case SCTP_ACTIVE:
		return 3;	/* best case */
	case SCTP_UNKNOWN:
		return 2;
	case SCTP_PF:
		return 1;
	default: /* case SCTP_INACTIVE */
		return 0;	/* worst case */
	}
}

static struct sctp_transport *sctp_trans_elect_tie(struct sctp_transport *trans1,
						   struct sctp_transport *trans2)
{
	if (trans1->error_count > trans2->error_count) {
		return trans2;
	} else if (trans1->error_count == trans2->error_count &&
		   ktime_after(trans2->last_time_heard,
			       trans1->last_time_heard)) {
		return trans2;
	} else {
		return trans1;
	}
}

static struct sctp_transport *sctp_trans_elect_best(struct sctp_transport *curr,
						    struct sctp_transport *best)
{
	u8 score_curr, score_best;

	if (best == NULL || curr == best)
		return curr;

	score_curr = sctp_trans_score(curr);
	score_best = sctp_trans_score(best);

	/* First, try a score-based selection if both transport states
	 * differ. If we're in a tie, lets try to make a more clever
	 * decision here based on error counts and last time heard.
	 */
	if (score_curr > score_best)
		return curr;
	else if (score_curr == score_best)
		return sctp_trans_elect_tie(best, curr);
	else
		return best;
}

void sctp_assoc_update_retran_path(struct sctp_association *asoc)
{
	struct sctp_transport *trans = asoc->peer.retran_path;
	struct sctp_transport *trans_next = NULL;

	/* We're done as we only have the one and only path. */
	if (asoc->peer.transport_count == 1)
		return;
	/* If active_path and retran_path are the same and active,
	 * then this is the only active path. Use it.
	 */
	if (asoc->peer.active_path == asoc->peer.retran_path &&
	    asoc->peer.active_path->state == SCTP_ACTIVE)
		return;

	/* Iterate from retran_path's successor back to retran_path. */
	for (trans = list_next_entry(trans, transports); 1;
	     trans = list_next_entry(trans, transports)) {
		/* Manually skip the head element. */
		if (&trans->transports == &asoc->peer.transport_addr_list)
			continue;
		if (trans->state == SCTP_UNCONFIRMED)
			continue;
		trans_next = sctp_trans_elect_best(trans, trans_next);
		/* Active is good enough for immediate return. */
		if (trans_next->state == SCTP_ACTIVE)
			break;
		/* We've reached the end, time to update path. */
		if (trans == asoc->peer.retran_path)
			break;
	}

	asoc->peer.retran_path = trans_next;

	pr_debug("%s: association:%p updated new path to addr:%pISpc\n",
		 __func__, asoc, &asoc->peer.retran_path->ipaddr.sa);
}

static void sctp_select_active_and_retran_path(struct sctp_association *asoc)
{
	struct sctp_transport *trans, *trans_pri = NULL, *trans_sec = NULL;
	struct sctp_transport *trans_pf = NULL;

	/* Look for the two most recently used active transports. */
	list_for_each_entry(trans, &asoc->peer.transport_addr_list,
			    transports) {
		/* Skip uninteresting transports. */
		if (trans->state == SCTP_INACTIVE ||
		    trans->state == SCTP_UNCONFIRMED)
			continue;
		/* Keep track of the best PF transport from our
		 * list in case we don't find an active one.
		 */
		if (trans->state == SCTP_PF) {
			trans_pf = sctp_trans_elect_best(trans, trans_pf);
			continue;
		}
		/* For active transports, pick the most recent ones. */
		if (trans_pri == NULL ||
		    ktime_after(trans->last_time_heard,
				trans_pri->last_time_heard)) {
			trans_sec = trans_pri;
			trans_pri = trans;
		} else if (trans_sec == NULL ||
			   ktime_after(trans->last_time_heard,
				       trans_sec->last_time_heard)) {
			trans_sec = trans;
		}
	}

	/* RFC 2960 6.4 Multi-Homed SCTP Endpoints
	 *
	 * By default, an endpoint should always transmit to the primary
	 * path, unless the SCTP user explicitly specifies the
	 * destination transport address (and possibly source transport
	 * address) to use. [If the primary is active but not most recent,
	 * bump the most recently used transport.]
	 */
	if ((asoc->peer.primary_path->state == SCTP_ACTIVE ||
	     asoc->peer.primary_path->state == SCTP_UNKNOWN) &&
	     asoc->peer.primary_path != trans_pri) {
		trans_sec = trans_pri;
		trans_pri = asoc->peer.primary_path;
	}

	/* We did not find anything useful for a possible retransmission
	 * path; either primary path that we found is the same as
	 * the current one, or we didn't generally find an active one.
	 */
	if (trans_sec == NULL)
		trans_sec = trans_pri;

	/* If we failed to find a usable transport, just camp on the
	 * active or pick a PF iff it's the better choice.
	 */
	if (trans_pri == NULL) {
		trans_pri = sctp_trans_elect_best(asoc->peer.active_path, trans_pf);
		trans_sec = trans_pri;
	}

	/* Set the active and retran transports. */
	asoc->peer.active_path = trans_pri;
	asoc->peer.retran_path = trans_sec;
}

struct sctp_transport *
sctp_assoc_choose_alter_transport(struct sctp_association *asoc,
				  struct sctp_transport *last_sent_to)
{
	/* If this is the first time packet is sent, use the active path,
	 * else use the retran path. If the last packet was sent over the
	 * retran path, update the retran path and use it.
	 */
	if (last_sent_to == NULL) {
		return asoc->peer.active_path;
	} else {
		if (last_sent_to == asoc->peer.retran_path)
			sctp_assoc_update_retran_path(asoc);

		return asoc->peer.retran_path;
	}
}

void sctp_assoc_update_frag_point(struct sctp_association *asoc)
{
	int frag = sctp_mtu_payload(sctp_sk(asoc->base.sk), asoc->pathmtu,
				    sctp_datachk_len(&asoc->stream));

	if (asoc->user_frag)
		frag = min_t(int, frag, asoc->user_frag);

	frag = min_t(int, frag, SCTP_MAX_CHUNK_LEN -
				sctp_datachk_len(&asoc->stream));

	asoc->frag_point = SCTP_TRUNC4(frag);
}

void sctp_assoc_set_pmtu(struct sctp_association *asoc, __u32 pmtu)
{
	if (asoc->pathmtu != pmtu) {
		asoc->pathmtu = pmtu;
		sctp_assoc_update_frag_point(asoc);
	}

	pr_debug("%s: asoc:%p, pmtu:%d, frag_point:%d\n", __func__, asoc,
		 asoc->pathmtu, asoc->frag_point);
}

/* Update the association's pmtu and frag_point by going through all the
 * transports. This routine is called when a transport's PMTU has changed.
 */
void sctp_assoc_sync_pmtu(struct sctp_association *asoc)
{
	struct sctp_transport *t;
	__u32 pmtu = 0;

	if (!asoc)
		return;

	/* Get the lowest pmtu of all the transports. */
	list_for_each_entry(t, &asoc->peer.transport_addr_list, transports) {
		if (t->pmtu_pending && t->dst) {
			sctp_transport_update_pmtu(t,
						   atomic_read(&t->mtu_info));
			t->pmtu_pending = 0;
		}
		if (!pmtu || (t->pathmtu < pmtu))
			pmtu = t->pathmtu;
	}

	sctp_assoc_set_pmtu(asoc, pmtu);
}

/* Should we send a SACK to update our peer? */
static inline bool sctp_peer_needs_update(struct sctp_association *asoc)
{
	struct net *net = asoc->base.net;

	switch (asoc->state) {
	case SCTP_STATE_ESTABLISHED:
	case SCTP_STATE_SHUTDOWN_PENDING:
	case SCTP_STATE_SHUTDOWN_RECEIVED:
	case SCTP_STATE_SHUTDOWN_SENT:
		if ((asoc->rwnd > asoc->a_rwnd) &&
		    ((asoc->rwnd - asoc->a_rwnd) >= max_t(__u32,
			   (asoc->base.sk->sk_rcvbuf >> net->sctp.rwnd_upd_shift),
			   asoc->pathmtu)))
			return true;
		break;
	default:
		break;
	}
	return false;
}

/* Increase asoc's rwnd by len and send any window update SACK if needed. */
void sctp_assoc_rwnd_increase(struct sctp_association *asoc, unsigned int len)
{
	struct sctp_chunk *sack;
	struct timer_list *timer;

	if (asoc->rwnd_over) {
		if (asoc->rwnd_over >= len) {
			asoc->rwnd_over -= len;
		} else {
			asoc->rwnd += (len - asoc->rwnd_over);
			asoc->rwnd_over = 0;
		}
	} else {
		asoc->rwnd += len;
	}

	/* If we had window pressure, start recovering it
	 * once our rwnd had reached the accumulated pressure
	 * threshold.  The idea is to recover slowly, but up
	 * to the initial advertised window.
	 */
	if (asoc->rwnd_press) {
		int change = min(asoc->pathmtu, asoc->rwnd_press);
		asoc->rwnd += change;
		asoc->rwnd_press -= change;
	}

	pr_debug("%s: asoc:%p rwnd increased by %d to (%u, %u) - %u\n",
		 __func__, asoc, len, asoc->rwnd, asoc->rwnd_over,
		 asoc->a_rwnd);

	/* Send a window update SACK if the rwnd has increased by at least the
	 * minimum of the association's PMTU and half of the receive buffer.
	 * The algorithm used is similar to the one described in
	 * Section 4.2.3.3 of RFC 1122.
	 */
	if (sctp_peer_needs_update(asoc)) {
		asoc->a_rwnd = asoc->rwnd;

		pr_debug("%s: sending window update SACK- asoc:%p rwnd:%u "
			 "a_rwnd:%u\n", __func__, asoc, asoc->rwnd,
			 asoc->a_rwnd);

		sack = sctp_make_sack(asoc);
		if (!sack)
			return;

		asoc->peer.sack_needed = 0;

		sctp_outq_tail(&asoc->outqueue, sack, GFP_ATOMIC);

		/* Stop the SACK timer.  */
		timer = &asoc->timers[SCTP_EVENT_TIMEOUT_SACK];
		if (del_timer(timer))
			sctp_association_put(asoc);
	}
}

/* Decrease asoc's rwnd by len. */
void sctp_assoc_rwnd_decrease(struct sctp_association *asoc, unsigned int len)
{
	int rx_count;
	int over = 0;

	if (unlikely(!asoc->rwnd || asoc->rwnd_over))
		pr_debug("%s: association:%p has asoc->rwnd:%u, "
			 "asoc->rwnd_over:%u!\n", __func__, asoc,
			 asoc->rwnd, asoc->rwnd_over);

	if (asoc->ep->rcvbuf_policy)
		rx_count = atomic_read(&asoc->rmem_alloc);
	else
		rx_count = atomic_read(&asoc->base.sk->sk_rmem_alloc);

	/* If we've reached or overflowed our receive buffer, announce
	 * a 0 rwnd if rwnd would still be positive.  Store the
	 * potential pressure overflow so that the window can be restored
	 * back to original value.
	 */
	if (rx_count >= asoc->base.sk->sk_rcvbuf)
		over = 1;

	if (asoc->rwnd >= len) {
		asoc->rwnd -= len;
		if (over) {
			asoc->rwnd_press += asoc->rwnd;
			asoc->rwnd = 0;
		}
	} else {
		asoc->rwnd_over += len - asoc->rwnd;
		asoc->rwnd = 0;
	}

	pr_debug("%s: asoc:%p rwnd decreased by %d to (%u, %u, %u)\n",
		 __func__, asoc, len, asoc->rwnd, asoc->rwnd_over,
		 asoc->rwnd_press);
}

/* Build the bind address list for the association based on info from the
 * local endpoint and the remote peer.
 */
int sctp_assoc_set_bind_addr_from_ep(struct sctp_association *asoc,
				     enum sctp_scope scope, gfp_t gfp)
{
	struct sock *sk = asoc->base.sk;
	int flags;

	/* Use scoping rules to determine the subset of addresses from
	 * the endpoint.
	 */
	flags = (PF_INET6 == sk->sk_family) ? SCTP_ADDR6_ALLOWED : 0;
	if (!inet_v6_ipv6only(sk))
		flags |= SCTP_ADDR4_ALLOWED;
	if (asoc->peer.ipv4_address)
		flags |= SCTP_ADDR4_PEERSUPP;
	if (asoc->peer.ipv6_address)
		flags |= SCTP_ADDR6_PEERSUPP;

	return sctp_bind_addr_copy(asoc->base.net,
				   &asoc->base.bind_addr,
				   &asoc->ep->base.bind_addr,
				   scope, gfp, flags);
}

/* Build the association's bind address list from the cookie.  */
int sctp_assoc_set_bind_addr_from_cookie(struct sctp_association *asoc,
					 struct sctp_cookie *cookie,
					 gfp_t gfp)
{
	int var_size2 = ntohs(cookie->peer_init->chunk_hdr.length);
	int var_size3 = cookie->raw_addr_list_len;
	__u8 *raw = (__u8 *)cookie->peer_init + var_size2;

	return sctp_raw_to_bind_addrs(&asoc->base.bind_addr, raw, var_size3,
				      asoc->ep->base.bind_addr.port, gfp);
}

/* Lookup laddr in the bind address list of an association. */
int sctp_assoc_lookup_laddr(struct sctp_association *asoc,
			    const union sctp_addr *laddr)
{
	int found = 0;

	if ((asoc->base.bind_addr.port == ntohs(laddr->v4.sin_port)) &&
	    sctp_bind_addr_match(&asoc->base.bind_addr, laddr,
				 sctp_sk(asoc->base.sk)))
		found = 1;

	return found;
}

/* Set an association id for a given association */
int sctp_assoc_set_id(struct sctp_association *asoc, gfp_t gfp)
{
	bool preload = gfpflags_allow_blocking(gfp);
	int ret;

	/* If the id is already assigned, keep it. */
	if (asoc->assoc_id)
		return 0;

	if (preload)
		idr_preload(gfp);
	spin_lock_bh(&sctp_assocs_id_lock);
	/* 0, 1, 2 are used as SCTP_FUTURE_ASSOC, SCTP_CURRENT_ASSOC and
	 * SCTP_ALL_ASSOC, so an available id must be > SCTP_ALL_ASSOC.
	 */
	ret = idr_alloc_cyclic(&sctp_assocs_id, asoc, SCTP_ALL_ASSOC + 1, 0,
			       GFP_NOWAIT);
	spin_unlock_bh(&sctp_assocs_id_lock);
	if (preload)
		idr_preload_end();
	if (ret < 0)
		return ret;

	asoc->assoc_id = (sctp_assoc_t)ret;
	return 0;
}

/* Free the ASCONF queue */
static void sctp_assoc_free_asconf_queue(struct sctp_association *asoc)
{
	struct sctp_chunk *asconf;
	struct sctp_chunk *tmp;

	list_for_each_entry_safe(asconf, tmp, &asoc->addip_chunk_list, list) {
		list_del_init(&asconf->list);
		sctp_chunk_free(asconf);
	}
}

/* Free asconf_ack cache */
static void sctp_assoc_free_asconf_acks(struct sctp_association *asoc)
{
	struct sctp_chunk *ack;
	struct sctp_chunk *tmp;

	list_for_each_entry_safe(ack, tmp, &asoc->asconf_ack_list,
				transmitted_list) {
		list_del_init(&ack->transmitted_list);
		sctp_chunk_free(ack);
	}
}

/* Clean up the ASCONF_ACK queue */
void sctp_assoc_clean_asconf_ack_cache(const struct sctp_association *asoc)
{
	struct sctp_chunk *ack;
	struct sctp_chunk *tmp;

	/* We can remove all the entries from the queue up to
	 * the "Peer-Sequence-Number".
	 */
	list_for_each_entry_safe(ack, tmp, &asoc->asconf_ack_list,
				transmitted_list) {
		if (ack->subh.addip_hdr->serial ==
				htonl(asoc->peer.addip_serial))
			break;

		list_del_init(&ack->transmitted_list);
		sctp_chunk_free(ack);
	}
}

/* Find the ASCONF_ACK whose serial number matches ASCONF */
struct sctp_chunk *sctp_assoc_lookup_asconf_ack(
					const struct sctp_association *asoc,
					__be32 serial)
{
	struct sctp_chunk *ack;

	/* Walk through the list of cached ASCONF-ACKs and find the
	 * ack chunk whose serial number matches that of the request.
	 */
	list_for_each_entry(ack, &asoc->asconf_ack_list, transmitted_list) {
		if (sctp_chunk_pending(ack))
			continue;
		if (ack->subh.addip_hdr->serial == serial) {
			sctp_chunk_hold(ack);
			return ack;
		}
	}

	return NULL;
}

void sctp_asconf_queue_teardown(struct sctp_association *asoc)
{
	/* Free any cached ASCONF_ACK chunk. */
	sctp_assoc_free_asconf_acks(asoc);

	/* Free the ASCONF queue. */
	sctp_assoc_free_asconf_queue(asoc);

	/* Free any cached ASCONF chunk. */
	if (asoc->addip_last_asconf)
		sctp_chunk_free(asoc->addip_last_asconf);
}
