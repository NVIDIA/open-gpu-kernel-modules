// SPDX-License-Identifier: GPL-2.0-or-later
/* SCTP kernel implementation
 * (C) Copyright IBM Corp. 2001, 2004
 * Copyright (c) 1999-2000 Cisco, Inc.
 * Copyright (c) 1999-2001 Motorola, Inc.
 * Copyright (c) 2001-2003 Intel Corp.
 *
 * This file is part of the SCTP kernel implementation
 *
 * These functions implement the sctp_outq class.   The outqueue handles
 * bundling and queueing of outgoing SCTP chunks.
 *
 * Please send any bug reports or fixes you make to the
 * email address(es):
 *    lksctp developers <linux-sctp@vger.kernel.org>
 *
 * Written or modified by:
 *    La Monte H.P. Yarroll <piggy@acm.org>
 *    Karl Knutson          <karl@athena.chicago.il.us>
 *    Perry Melange         <pmelange@null.cc.uic.edu>
 *    Xingang Guo           <xingang.guo@intel.com>
 *    Hui Huang 	    <hui.huang@nokia.com>
 *    Sridhar Samudrala     <sri@us.ibm.com>
 *    Jon Grimm             <jgrimm@us.ibm.com>
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/types.h>
#include <linux/list.h>   /* For struct list_head */
#include <linux/socket.h>
#include <linux/ip.h>
#include <linux/slab.h>
#include <net/sock.h>	  /* For skb_set_owner_w */

#include <net/sctp/sctp.h>
#include <net/sctp/sm.h>
#include <net/sctp/stream_sched.h>
#include <trace/events/sctp.h>

/* Declare internal functions here.  */
static int sctp_acked(struct sctp_sackhdr *sack, __u32 tsn);
static void sctp_check_transmitted(struct sctp_outq *q,
				   struct list_head *transmitted_queue,
				   struct sctp_transport *transport,
				   union sctp_addr *saddr,
				   struct sctp_sackhdr *sack,
				   __u32 *highest_new_tsn);

static void sctp_mark_missing(struct sctp_outq *q,
			      struct list_head *transmitted_queue,
			      struct sctp_transport *transport,
			      __u32 highest_new_tsn,
			      int count_of_newacks);

static void sctp_outq_flush(struct sctp_outq *q, int rtx_timeout, gfp_t gfp);

/* Add data to the front of the queue. */
static inline void sctp_outq_head_data(struct sctp_outq *q,
				       struct sctp_chunk *ch)
{
	struct sctp_stream_out_ext *oute;
	__u16 stream;

	list_add(&ch->list, &q->out_chunk_list);
	q->out_qlen += ch->skb->len;

	stream = sctp_chunk_stream_no(ch);
	oute = SCTP_SO(&q->asoc->stream, stream)->ext;
	list_add(&ch->stream_list, &oute->outq);
}

/* Take data from the front of the queue. */
static inline struct sctp_chunk *sctp_outq_dequeue_data(struct sctp_outq *q)
{
	return q->sched->dequeue(q);
}

/* Add data chunk to the end of the queue. */
static inline void sctp_outq_tail_data(struct sctp_outq *q,
				       struct sctp_chunk *ch)
{
	struct sctp_stream_out_ext *oute;
	__u16 stream;

	list_add_tail(&ch->list, &q->out_chunk_list);
	q->out_qlen += ch->skb->len;

	stream = sctp_chunk_stream_no(ch);
	oute = SCTP_SO(&q->asoc->stream, stream)->ext;
	list_add_tail(&ch->stream_list, &oute->outq);
}

/*
 * SFR-CACC algorithm:
 * D) If count_of_newacks is greater than or equal to 2
 * and t was not sent to the current primary then the
 * sender MUST NOT increment missing report count for t.
 */
static inline int sctp_cacc_skip_3_1_d(struct sctp_transport *primary,
				       struct sctp_transport *transport,
				       int count_of_newacks)
{
	if (count_of_newacks >= 2 && transport != primary)
		return 1;
	return 0;
}

/*
 * SFR-CACC algorithm:
 * F) If count_of_newacks is less than 2, let d be the
 * destination to which t was sent. If cacc_saw_newack
 * is 0 for destination d, then the sender MUST NOT
 * increment missing report count for t.
 */
static inline int sctp_cacc_skip_3_1_f(struct sctp_transport *transport,
				       int count_of_newacks)
{
	if (count_of_newacks < 2 &&
			(transport && !transport->cacc.cacc_saw_newack))
		return 1;
	return 0;
}

/*
 * SFR-CACC algorithm:
 * 3.1) If CYCLING_CHANGEOVER is 0, the sender SHOULD
 * execute steps C, D, F.
 *
 * C has been implemented in sctp_outq_sack
 */
static inline int sctp_cacc_skip_3_1(struct sctp_transport *primary,
				     struct sctp_transport *transport,
				     int count_of_newacks)
{
	if (!primary->cacc.cycling_changeover) {
		if (sctp_cacc_skip_3_1_d(primary, transport, count_of_newacks))
			return 1;
		if (sctp_cacc_skip_3_1_f(transport, count_of_newacks))
			return 1;
		return 0;
	}
	return 0;
}

/*
 * SFR-CACC algorithm:
 * 3.2) Else if CYCLING_CHANGEOVER is 1, and t is less
 * than next_tsn_at_change of the current primary, then
 * the sender MUST NOT increment missing report count
 * for t.
 */
static inline int sctp_cacc_skip_3_2(struct sctp_transport *primary, __u32 tsn)
{
	if (primary->cacc.cycling_changeover &&
	    TSN_lt(tsn, primary->cacc.next_tsn_at_change))
		return 1;
	return 0;
}

/*
 * SFR-CACC algorithm:
 * 3) If the missing report count for TSN t is to be
 * incremented according to [RFC2960] and
 * [SCTP_STEWART-2002], and CHANGEOVER_ACTIVE is set,
 * then the sender MUST further execute steps 3.1 and
 * 3.2 to determine if the missing report count for
 * TSN t SHOULD NOT be incremented.
 *
 * 3.3) If 3.1 and 3.2 do not dictate that the missing
 * report count for t should not be incremented, then
 * the sender SHOULD increment missing report count for
 * t (according to [RFC2960] and [SCTP_STEWART_2002]).
 */
static inline int sctp_cacc_skip(struct sctp_transport *primary,
				 struct sctp_transport *transport,
				 int count_of_newacks,
				 __u32 tsn)
{
	if (primary->cacc.changeover_active &&
	    (sctp_cacc_skip_3_1(primary, transport, count_of_newacks) ||
	     sctp_cacc_skip_3_2(primary, tsn)))
		return 1;
	return 0;
}

/* Initialize an existing sctp_outq.  This does the boring stuff.
 * You still need to define handlers if you really want to DO
 * something with this structure...
 */
void sctp_outq_init(struct sctp_association *asoc, struct sctp_outq *q)
{
	memset(q, 0, sizeof(struct sctp_outq));

	q->asoc = asoc;
	INIT_LIST_HEAD(&q->out_chunk_list);
	INIT_LIST_HEAD(&q->control_chunk_list);
	INIT_LIST_HEAD(&q->retransmit);
	INIT_LIST_HEAD(&q->sacked);
	INIT_LIST_HEAD(&q->abandoned);
	sctp_sched_set_sched(asoc, sctp_sk(asoc->base.sk)->default_ss);
}

/* Free the outqueue structure and any related pending chunks.
 */
static void __sctp_outq_teardown(struct sctp_outq *q)
{
	struct sctp_transport *transport;
	struct list_head *lchunk, *temp;
	struct sctp_chunk *chunk, *tmp;

	/* Throw away unacknowledged chunks. */
	list_for_each_entry(transport, &q->asoc->peer.transport_addr_list,
			transports) {
		while ((lchunk = sctp_list_dequeue(&transport->transmitted)) != NULL) {
			chunk = list_entry(lchunk, struct sctp_chunk,
					   transmitted_list);
			/* Mark as part of a failed message. */
			sctp_chunk_fail(chunk, q->error);
			sctp_chunk_free(chunk);
		}
	}

	/* Throw away chunks that have been gap ACKed.  */
	list_for_each_safe(lchunk, temp, &q->sacked) {
		list_del_init(lchunk);
		chunk = list_entry(lchunk, struct sctp_chunk,
				   transmitted_list);
		sctp_chunk_fail(chunk, q->error);
		sctp_chunk_free(chunk);
	}

	/* Throw away any chunks in the retransmit queue. */
	list_for_each_safe(lchunk, temp, &q->retransmit) {
		list_del_init(lchunk);
		chunk = list_entry(lchunk, struct sctp_chunk,
				   transmitted_list);
		sctp_chunk_fail(chunk, q->error);
		sctp_chunk_free(chunk);
	}

	/* Throw away any chunks that are in the abandoned queue. */
	list_for_each_safe(lchunk, temp, &q->abandoned) {
		list_del_init(lchunk);
		chunk = list_entry(lchunk, struct sctp_chunk,
				   transmitted_list);
		sctp_chunk_fail(chunk, q->error);
		sctp_chunk_free(chunk);
	}

	/* Throw away any leftover data chunks. */
	while ((chunk = sctp_outq_dequeue_data(q)) != NULL) {
		sctp_sched_dequeue_done(q, chunk);

		/* Mark as send failure. */
		sctp_chunk_fail(chunk, q->error);
		sctp_chunk_free(chunk);
	}

	/* Throw away any leftover control chunks. */
	list_for_each_entry_safe(chunk, tmp, &q->control_chunk_list, list) {
		list_del_init(&chunk->list);
		sctp_chunk_free(chunk);
	}
}

void sctp_outq_teardown(struct sctp_outq *q)
{
	__sctp_outq_teardown(q);
	sctp_outq_init(q->asoc, q);
}

/* Free the outqueue structure and any related pending chunks.  */
void sctp_outq_free(struct sctp_outq *q)
{
	/* Throw away leftover chunks. */
	__sctp_outq_teardown(q);
}

/* Put a new chunk in an sctp_outq.  */
void sctp_outq_tail(struct sctp_outq *q, struct sctp_chunk *chunk, gfp_t gfp)
{
	struct net *net = q->asoc->base.net;

	pr_debug("%s: outq:%p, chunk:%p[%s]\n", __func__, q, chunk,
		 chunk && chunk->chunk_hdr ?
		 sctp_cname(SCTP_ST_CHUNK(chunk->chunk_hdr->type)) :
		 "illegal chunk");

	/* If it is data, queue it up, otherwise, send it
	 * immediately.
	 */
	if (sctp_chunk_is_data(chunk)) {
		pr_debug("%s: outqueueing: outq:%p, chunk:%p[%s])\n",
			 __func__, q, chunk, chunk && chunk->chunk_hdr ?
			 sctp_cname(SCTP_ST_CHUNK(chunk->chunk_hdr->type)) :
			 "illegal chunk");

		sctp_outq_tail_data(q, chunk);
		if (chunk->asoc->peer.prsctp_capable &&
		    SCTP_PR_PRIO_ENABLED(chunk->sinfo.sinfo_flags))
			chunk->asoc->sent_cnt_removable++;
		if (chunk->chunk_hdr->flags & SCTP_DATA_UNORDERED)
			SCTP_INC_STATS(net, SCTP_MIB_OUTUNORDERCHUNKS);
		else
			SCTP_INC_STATS(net, SCTP_MIB_OUTORDERCHUNKS);
	} else {
		list_add_tail(&chunk->list, &q->control_chunk_list);
		SCTP_INC_STATS(net, SCTP_MIB_OUTCTRLCHUNKS);
	}

	if (!q->cork)
		sctp_outq_flush(q, 0, gfp);
}

/* Insert a chunk into the sorted list based on the TSNs.  The retransmit list
 * and the abandoned list are in ascending order.
 */
static void sctp_insert_list(struct list_head *head, struct list_head *new)
{
	struct list_head *pos;
	struct sctp_chunk *nchunk, *lchunk;
	__u32 ntsn, ltsn;
	int done = 0;

	nchunk = list_entry(new, struct sctp_chunk, transmitted_list);
	ntsn = ntohl(nchunk->subh.data_hdr->tsn);

	list_for_each(pos, head) {
		lchunk = list_entry(pos, struct sctp_chunk, transmitted_list);
		ltsn = ntohl(lchunk->subh.data_hdr->tsn);
		if (TSN_lt(ntsn, ltsn)) {
			list_add(new, pos->prev);
			done = 1;
			break;
		}
	}
	if (!done)
		list_add_tail(new, head);
}

static int sctp_prsctp_prune_sent(struct sctp_association *asoc,
				  struct sctp_sndrcvinfo *sinfo,
				  struct list_head *queue, int msg_len)
{
	struct sctp_chunk *chk, *temp;

	list_for_each_entry_safe(chk, temp, queue, transmitted_list) {
		struct sctp_stream_out *streamout;

		if (!chk->msg->abandoned &&
		    (!SCTP_PR_PRIO_ENABLED(chk->sinfo.sinfo_flags) ||
		     chk->sinfo.sinfo_timetolive <= sinfo->sinfo_timetolive))
			continue;

		chk->msg->abandoned = 1;
		list_del_init(&chk->transmitted_list);
		sctp_insert_list(&asoc->outqueue.abandoned,
				 &chk->transmitted_list);

		streamout = SCTP_SO(&asoc->stream, chk->sinfo.sinfo_stream);
		asoc->sent_cnt_removable--;
		asoc->abandoned_sent[SCTP_PR_INDEX(PRIO)]++;
		streamout->ext->abandoned_sent[SCTP_PR_INDEX(PRIO)]++;

		if (queue != &asoc->outqueue.retransmit &&
		    !chk->tsn_gap_acked) {
			if (chk->transport)
				chk->transport->flight_size -=
						sctp_data_size(chk);
			asoc->outqueue.outstanding_bytes -= sctp_data_size(chk);
		}

		msg_len -= chk->skb->truesize + sizeof(struct sctp_chunk);
		if (msg_len <= 0)
			break;
	}

	return msg_len;
}

static int sctp_prsctp_prune_unsent(struct sctp_association *asoc,
				    struct sctp_sndrcvinfo *sinfo, int msg_len)
{
	struct sctp_outq *q = &asoc->outqueue;
	struct sctp_chunk *chk, *temp;

	q->sched->unsched_all(&asoc->stream);

	list_for_each_entry_safe(chk, temp, &q->out_chunk_list, list) {
		if (!chk->msg->abandoned &&
		    (!(chk->chunk_hdr->flags & SCTP_DATA_FIRST_FRAG) ||
		     !SCTP_PR_PRIO_ENABLED(chk->sinfo.sinfo_flags) ||
		     chk->sinfo.sinfo_timetolive <= sinfo->sinfo_timetolive))
			continue;

		chk->msg->abandoned = 1;
		sctp_sched_dequeue_common(q, chk);
		asoc->sent_cnt_removable--;
		asoc->abandoned_unsent[SCTP_PR_INDEX(PRIO)]++;
		if (chk->sinfo.sinfo_stream < asoc->stream.outcnt) {
			struct sctp_stream_out *streamout =
				SCTP_SO(&asoc->stream, chk->sinfo.sinfo_stream);

			streamout->ext->abandoned_unsent[SCTP_PR_INDEX(PRIO)]++;
		}

		msg_len -= chk->skb->truesize + sizeof(struct sctp_chunk);
		sctp_chunk_free(chk);
		if (msg_len <= 0)
			break;
	}

	q->sched->sched_all(&asoc->stream);

	return msg_len;
}

/* Abandon the chunks according their priorities */
void sctp_prsctp_prune(struct sctp_association *asoc,
		       struct sctp_sndrcvinfo *sinfo, int msg_len)
{
	struct sctp_transport *transport;

	if (!asoc->peer.prsctp_capable || !asoc->sent_cnt_removable)
		return;

	msg_len = sctp_prsctp_prune_sent(asoc, sinfo,
					 &asoc->outqueue.retransmit,
					 msg_len);
	if (msg_len <= 0)
		return;

	list_for_each_entry(transport, &asoc->peer.transport_addr_list,
			    transports) {
		msg_len = sctp_prsctp_prune_sent(asoc, sinfo,
						 &transport->transmitted,
						 msg_len);
		if (msg_len <= 0)
			return;
	}

	sctp_prsctp_prune_unsent(asoc, sinfo, msg_len);
}

/* Mark all the eligible packets on a transport for retransmission.  */
void sctp_retransmit_mark(struct sctp_outq *q,
			  struct sctp_transport *transport,
			  __u8 reason)
{
	struct list_head *lchunk, *ltemp;
	struct sctp_chunk *chunk;

	/* Walk through the specified transmitted queue.  */
	list_for_each_safe(lchunk, ltemp, &transport->transmitted) {
		chunk = list_entry(lchunk, struct sctp_chunk,
				   transmitted_list);

		/* If the chunk is abandoned, move it to abandoned list. */
		if (sctp_chunk_abandoned(chunk)) {
			list_del_init(lchunk);
			sctp_insert_list(&q->abandoned, lchunk);

			/* If this chunk has not been previousely acked,
			 * stop considering it 'outstanding'.  Our peer
			 * will most likely never see it since it will
			 * not be retransmitted
			 */
			if (!chunk->tsn_gap_acked) {
				if (chunk->transport)
					chunk->transport->flight_size -=
							sctp_data_size(chunk);
				q->outstanding_bytes -= sctp_data_size(chunk);
				q->asoc->peer.rwnd += sctp_data_size(chunk);
			}
			continue;
		}

		/* If we are doing  retransmission due to a timeout or pmtu
		 * discovery, only the  chunks that are not yet acked should
		 * be added to the retransmit queue.
		 */
		if ((reason == SCTP_RTXR_FAST_RTX  &&
			    (chunk->fast_retransmit == SCTP_NEED_FRTX)) ||
		    (reason != SCTP_RTXR_FAST_RTX  && !chunk->tsn_gap_acked)) {
			/* RFC 2960 6.2.1 Processing a Received SACK
			 *
			 * C) Any time a DATA chunk is marked for
			 * retransmission (via either T3-rtx timer expiration
			 * (Section 6.3.3) or via fast retransmit
			 * (Section 7.2.4)), add the data size of those
			 * chunks to the rwnd.
			 */
			q->asoc->peer.rwnd += sctp_data_size(chunk);
			q->outstanding_bytes -= sctp_data_size(chunk);
			if (chunk->transport)
				transport->flight_size -= sctp_data_size(chunk);

			/* sctpimpguide-05 Section 2.8.2
			 * M5) If a T3-rtx timer expires, the
			 * 'TSN.Missing.Report' of all affected TSNs is set
			 * to 0.
			 */
			chunk->tsn_missing_report = 0;

			/* If a chunk that is being used for RTT measurement
			 * has to be retransmitted, we cannot use this chunk
			 * anymore for RTT measurements. Reset rto_pending so
			 * that a new RTT measurement is started when a new
			 * data chunk is sent.
			 */
			if (chunk->rtt_in_progress) {
				chunk->rtt_in_progress = 0;
				transport->rto_pending = 0;
			}

			/* Move the chunk to the retransmit queue. The chunks
			 * on the retransmit queue are always kept in order.
			 */
			list_del_init(lchunk);
			sctp_insert_list(&q->retransmit, lchunk);
		}
	}

	pr_debug("%s: transport:%p, reason:%d, cwnd:%d, ssthresh:%d, "
		 "flight_size:%d, pba:%d\n", __func__, transport, reason,
		 transport->cwnd, transport->ssthresh, transport->flight_size,
		 transport->partial_bytes_acked);
}

/* Mark all the eligible packets on a transport for retransmission and force
 * one packet out.
 */
void sctp_retransmit(struct sctp_outq *q, struct sctp_transport *transport,
		     enum sctp_retransmit_reason reason)
{
	struct net *net = q->asoc->base.net;

	switch (reason) {
	case SCTP_RTXR_T3_RTX:
		SCTP_INC_STATS(net, SCTP_MIB_T3_RETRANSMITS);
		sctp_transport_lower_cwnd(transport, SCTP_LOWER_CWND_T3_RTX);
		/* Update the retran path if the T3-rtx timer has expired for
		 * the current retran path.
		 */
		if (transport == transport->asoc->peer.retran_path)
			sctp_assoc_update_retran_path(transport->asoc);
		transport->asoc->rtx_data_chunks +=
			transport->asoc->unack_data;
		break;
	case SCTP_RTXR_FAST_RTX:
		SCTP_INC_STATS(net, SCTP_MIB_FAST_RETRANSMITS);
		sctp_transport_lower_cwnd(transport, SCTP_LOWER_CWND_FAST_RTX);
		q->fast_rtx = 1;
		break;
	case SCTP_RTXR_PMTUD:
		SCTP_INC_STATS(net, SCTP_MIB_PMTUD_RETRANSMITS);
		break;
	case SCTP_RTXR_T1_RTX:
		SCTP_INC_STATS(net, SCTP_MIB_T1_RETRANSMITS);
		transport->asoc->init_retries++;
		break;
	default:
		BUG();
	}

	sctp_retransmit_mark(q, transport, reason);

	/* PR-SCTP A5) Any time the T3-rtx timer expires, on any destination,
	 * the sender SHOULD try to advance the "Advanced.Peer.Ack.Point" by
	 * following the procedures outlined in C1 - C5.
	 */
	if (reason == SCTP_RTXR_T3_RTX)
		q->asoc->stream.si->generate_ftsn(q, q->asoc->ctsn_ack_point);

	/* Flush the queues only on timeout, since fast_rtx is only
	 * triggered during sack processing and the queue
	 * will be flushed at the end.
	 */
	if (reason != SCTP_RTXR_FAST_RTX)
		sctp_outq_flush(q, /* rtx_timeout */ 1, GFP_ATOMIC);
}

/*
 * Transmit DATA chunks on the retransmit queue.  Upon return from
 * __sctp_outq_flush_rtx() the packet 'pkt' may contain chunks which
 * need to be transmitted by the caller.
 * We assume that pkt->transport has already been set.
 *
 * The return value is a normal kernel error return value.
 */
static int __sctp_outq_flush_rtx(struct sctp_outq *q, struct sctp_packet *pkt,
				 int rtx_timeout, int *start_timer, gfp_t gfp)
{
	struct sctp_transport *transport = pkt->transport;
	struct sctp_chunk *chunk, *chunk1;
	struct list_head *lqueue;
	enum sctp_xmit status;
	int error = 0;
	int timer = 0;
	int done = 0;
	int fast_rtx;

	lqueue = &q->retransmit;
	fast_rtx = q->fast_rtx;

	/* This loop handles time-out retransmissions, fast retransmissions,
	 * and retransmissions due to opening of whindow.
	 *
	 * RFC 2960 6.3.3 Handle T3-rtx Expiration
	 *
	 * E3) Determine how many of the earliest (i.e., lowest TSN)
	 * outstanding DATA chunks for the address for which the
	 * T3-rtx has expired will fit into a single packet, subject
	 * to the MTU constraint for the path corresponding to the
	 * destination transport address to which the retransmission
	 * is being sent (this may be different from the address for
	 * which the timer expires [see Section 6.4]). Call this value
	 * K. Bundle and retransmit those K DATA chunks in a single
	 * packet to the destination endpoint.
	 *
	 * [Just to be painfully clear, if we are retransmitting
	 * because a timeout just happened, we should send only ONE
	 * packet of retransmitted data.]
	 *
	 * For fast retransmissions we also send only ONE packet.  However,
	 * if we are just flushing the queue due to open window, we'll
	 * try to send as much as possible.
	 */
	list_for_each_entry_safe(chunk, chunk1, lqueue, transmitted_list) {
		/* If the chunk is abandoned, move it to abandoned list. */
		if (sctp_chunk_abandoned(chunk)) {
			list_del_init(&chunk->transmitted_list);
			sctp_insert_list(&q->abandoned,
					 &chunk->transmitted_list);
			continue;
		}

		/* Make sure that Gap Acked TSNs are not retransmitted.  A
		 * simple approach is just to move such TSNs out of the
		 * way and into a 'transmitted' queue and skip to the
		 * next chunk.
		 */
		if (chunk->tsn_gap_acked) {
			list_move_tail(&chunk->transmitted_list,
				       &transport->transmitted);
			continue;
		}

		/* If we are doing fast retransmit, ignore non-fast_rtransmit
		 * chunks
		 */
		if (fast_rtx && !chunk->fast_retransmit)
			continue;

redo:
		/* Attempt to append this chunk to the packet. */
		status = sctp_packet_append_chunk(pkt, chunk);

		switch (status) {
		case SCTP_XMIT_PMTU_FULL:
			if (!pkt->has_data && !pkt->has_cookie_echo) {
				/* If this packet did not contain DATA then
				 * retransmission did not happen, so do it
				 * again.  We'll ignore the error here since
				 * control chunks are already freed so there
				 * is nothing we can do.
				 */
				sctp_packet_transmit(pkt, gfp);
				goto redo;
			}

			/* Send this packet.  */
			error = sctp_packet_transmit(pkt, gfp);

			/* If we are retransmitting, we should only
			 * send a single packet.
			 * Otherwise, try appending this chunk again.
			 */
			if (rtx_timeout || fast_rtx)
				done = 1;
			else
				goto redo;

			/* Bundle next chunk in the next round.  */
			break;

		case SCTP_XMIT_RWND_FULL:
			/* Send this packet. */
			error = sctp_packet_transmit(pkt, gfp);

			/* Stop sending DATA as there is no more room
			 * at the receiver.
			 */
			done = 1;
			break;

		case SCTP_XMIT_DELAY:
			/* Send this packet. */
			error = sctp_packet_transmit(pkt, gfp);

			/* Stop sending DATA because of nagle delay. */
			done = 1;
			break;

		default:
			/* The append was successful, so add this chunk to
			 * the transmitted list.
			 */
			list_move_tail(&chunk->transmitted_list,
				       &transport->transmitted);

			/* Mark the chunk as ineligible for fast retransmit
			 * after it is retransmitted.
			 */
			if (chunk->fast_retransmit == SCTP_NEED_FRTX)
				chunk->fast_retransmit = SCTP_DONT_FRTX;

			q->asoc->stats.rtxchunks++;
			break;
		}

		/* Set the timer if there were no errors */
		if (!error && !timer)
			timer = 1;

		if (done)
			break;
	}

	/* If we are here due to a retransmit timeout or a fast
	 * retransmit and if there are any chunks left in the retransmit
	 * queue that could not fit in the PMTU sized packet, they need
	 * to be marked as ineligible for a subsequent fast retransmit.
	 */
	if (rtx_timeout || fast_rtx) {
		list_for_each_entry(chunk1, lqueue, transmitted_list) {
			if (chunk1->fast_retransmit == SCTP_NEED_FRTX)
				chunk1->fast_retransmit = SCTP_DONT_FRTX;
		}
	}

	*start_timer = timer;

	/* Clear fast retransmit hint */
	if (fast_rtx)
		q->fast_rtx = 0;

	return error;
}

/* Cork the outqueue so queued chunks are really queued. */
void sctp_outq_uncork(struct sctp_outq *q, gfp_t gfp)
{
	if (q->cork)
		q->cork = 0;

	sctp_outq_flush(q, 0, gfp);
}

static int sctp_packet_singleton(struct sctp_transport *transport,
				 struct sctp_chunk *chunk, gfp_t gfp)
{
	const struct sctp_association *asoc = transport->asoc;
	const __u16 sport = asoc->base.bind_addr.port;
	const __u16 dport = asoc->peer.port;
	const __u32 vtag = asoc->peer.i.init_tag;
	struct sctp_packet singleton;

	sctp_packet_init(&singleton, transport, sport, dport);
	sctp_packet_config(&singleton, vtag, 0);
	sctp_packet_append_chunk(&singleton, chunk);
	return sctp_packet_transmit(&singleton, gfp);
}

/* Struct to hold the context during sctp outq flush */
struct sctp_flush_ctx {
	struct sctp_outq *q;
	/* Current transport being used. It's NOT the same as curr active one */
	struct sctp_transport *transport;
	/* These transports have chunks to send. */
	struct list_head transport_list;
	struct sctp_association *asoc;
	/* Packet on the current transport above */
	struct sctp_packet *packet;
	gfp_t gfp;
};

/* transport: current transport */
static void sctp_outq_select_transport(struct sctp_flush_ctx *ctx,
				       struct sctp_chunk *chunk)
{
	struct sctp_transport *new_transport = chunk->transport;

	if (!new_transport) {
		if (!sctp_chunk_is_data(chunk)) {
			/* If we have a prior transport pointer, see if
			 * the destination address of the chunk
			 * matches the destination address of the
			 * current transport.  If not a match, then
			 * try to look up the transport with a given
			 * destination address.  We do this because
			 * after processing ASCONFs, we may have new
			 * transports created.
			 */
			if (ctx->transport && sctp_cmp_addr_exact(&chunk->dest,
							&ctx->transport->ipaddr))
				new_transport = ctx->transport;
			else
				new_transport = sctp_assoc_lookup_paddr(ctx->asoc,
								  &chunk->dest);
		}

		/* if we still don't have a new transport, then
		 * use the current active path.
		 */
		if (!new_transport)
			new_transport = ctx->asoc->peer.active_path;
	} else {
		__u8 type;

		switch (new_transport->state) {
		case SCTP_INACTIVE:
		case SCTP_UNCONFIRMED:
		case SCTP_PF:
			/* If the chunk is Heartbeat or Heartbeat Ack,
			 * send it to chunk->transport, even if it's
			 * inactive.
			 *
			 * 3.3.6 Heartbeat Acknowledgement:
			 * ...
			 * A HEARTBEAT ACK is always sent to the source IP
			 * address of the IP datagram containing the
			 * HEARTBEAT chunk to which this ack is responding.
			 * ...
			 *
			 * ASCONF_ACKs also must be sent to the source.
			 */
			type = chunk->chunk_hdr->type;
			if (type != SCTP_CID_HEARTBEAT &&
			    type != SCTP_CID_HEARTBEAT_ACK &&
			    type != SCTP_CID_ASCONF_ACK)
				new_transport = ctx->asoc->peer.active_path;
			break;
		default:
			break;
		}
	}

	/* Are we switching transports? Take care of transport locks. */
	if (new_transport != ctx->transport) {
		ctx->transport = new_transport;
		ctx->packet = &ctx->transport->packet;

		if (list_empty(&ctx->transport->send_ready))
			list_add_tail(&ctx->transport->send_ready,
				      &ctx->transport_list);

		sctp_packet_config(ctx->packet,
				   ctx->asoc->peer.i.init_tag,
				   ctx->asoc->peer.ecn_capable);
		/* We've switched transports, so apply the
		 * Burst limit to the new transport.
		 */
		sctp_transport_burst_limited(ctx->transport);
	}
}

static void sctp_outq_flush_ctrl(struct sctp_flush_ctx *ctx)
{
	struct sctp_chunk *chunk, *tmp;
	enum sctp_xmit status;
	int one_packet, error;

	list_for_each_entry_safe(chunk, tmp, &ctx->q->control_chunk_list, list) {
		one_packet = 0;

		/* RFC 5061, 5.3
		 * F1) This means that until such time as the ASCONF
		 * containing the add is acknowledged, the sender MUST
		 * NOT use the new IP address as a source for ANY SCTP
		 * packet except on carrying an ASCONF Chunk.
		 */
		if (ctx->asoc->src_out_of_asoc_ok &&
		    chunk->chunk_hdr->type != SCTP_CID_ASCONF)
			continue;

		list_del_init(&chunk->list);

		/* Pick the right transport to use. Should always be true for
		 * the first chunk as we don't have a transport by then.
		 */
		sctp_outq_select_transport(ctx, chunk);

		switch (chunk->chunk_hdr->type) {
		/* 6.10 Bundling
		 *   ...
		 *   An endpoint MUST NOT bundle INIT, INIT ACK or SHUTDOWN
		 *   COMPLETE with any other chunks.  [Send them immediately.]
		 */
		case SCTP_CID_INIT:
		case SCTP_CID_INIT_ACK:
		case SCTP_CID_SHUTDOWN_COMPLETE:
			error = sctp_packet_singleton(ctx->transport, chunk,
						      ctx->gfp);
			if (error < 0) {
				ctx->asoc->base.sk->sk_err = -error;
				return;
			}
			break;

		case SCTP_CID_ABORT:
			if (sctp_test_T_bit(chunk))
				ctx->packet->vtag = ctx->asoc->c.my_vtag;
			fallthrough;

		/* The following chunks are "response" chunks, i.e.
		 * they are generated in response to something we
		 * received.  If we are sending these, then we can
		 * send only 1 packet containing these chunks.
		 */
		case SCTP_CID_HEARTBEAT_ACK:
		case SCTP_CID_SHUTDOWN_ACK:
		case SCTP_CID_COOKIE_ACK:
		case SCTP_CID_COOKIE_ECHO:
		case SCTP_CID_ERROR:
		case SCTP_CID_ECN_CWR:
		case SCTP_CID_ASCONF_ACK:
			one_packet = 1;
			fallthrough;

		case SCTP_CID_SACK:
		case SCTP_CID_HEARTBEAT:
		case SCTP_CID_SHUTDOWN:
		case SCTP_CID_ECN_ECNE:
		case SCTP_CID_ASCONF:
		case SCTP_CID_FWD_TSN:
		case SCTP_CID_I_FWD_TSN:
		case SCTP_CID_RECONF:
			status = sctp_packet_transmit_chunk(ctx->packet, chunk,
							    one_packet, ctx->gfp);
			if (status != SCTP_XMIT_OK) {
				/* put the chunk back */
				list_add(&chunk->list, &ctx->q->control_chunk_list);
				break;
			}

			ctx->asoc->stats.octrlchunks++;
			/* PR-SCTP C5) If a FORWARD TSN is sent, the
			 * sender MUST assure that at least one T3-rtx
			 * timer is running.
			 */
			if (chunk->chunk_hdr->type == SCTP_CID_FWD_TSN ||
			    chunk->chunk_hdr->type == SCTP_CID_I_FWD_TSN) {
				sctp_transport_reset_t3_rtx(ctx->transport);
				ctx->transport->last_time_sent = jiffies;
			}

			if (chunk == ctx->asoc->strreset_chunk)
				sctp_transport_reset_reconf_timer(ctx->transport);

			break;

		default:
			/* We built a chunk with an illegal type! */
			BUG();
		}
	}
}

/* Returns false if new data shouldn't be sent */
static bool sctp_outq_flush_rtx(struct sctp_flush_ctx *ctx,
				int rtx_timeout)
{
	int error, start_timer = 0;

	if (ctx->asoc->peer.retran_path->state == SCTP_UNCONFIRMED)
		return false;

	if (ctx->transport != ctx->asoc->peer.retran_path) {
		/* Switch transports & prepare the packet.  */
		ctx->transport = ctx->asoc->peer.retran_path;
		ctx->packet = &ctx->transport->packet;

		if (list_empty(&ctx->transport->send_ready))
			list_add_tail(&ctx->transport->send_ready,
				      &ctx->transport_list);

		sctp_packet_config(ctx->packet, ctx->asoc->peer.i.init_tag,
				   ctx->asoc->peer.ecn_capable);
	}

	error = __sctp_outq_flush_rtx(ctx->q, ctx->packet, rtx_timeout,
				      &start_timer, ctx->gfp);
	if (error < 0)
		ctx->asoc->base.sk->sk_err = -error;

	if (start_timer) {
		sctp_transport_reset_t3_rtx(ctx->transport);
		ctx->transport->last_time_sent = jiffies;
	}

	/* This can happen on COOKIE-ECHO resend.  Only
	 * one chunk can get bundled with a COOKIE-ECHO.
	 */
	if (ctx->packet->has_cookie_echo)
		return false;

	/* Don't send new data if there is still data
	 * waiting to retransmit.
	 */
	if (!list_empty(&ctx->q->retransmit))
		return false;

	return true;
}

static void sctp_outq_flush_data(struct sctp_flush_ctx *ctx,
				 int rtx_timeout)
{
	struct sctp_chunk *chunk;
	enum sctp_xmit status;

	/* Is it OK to send data chunks?  */
	switch (ctx->asoc->state) {
	case SCTP_STATE_COOKIE_ECHOED:
		/* Only allow bundling when this packet has a COOKIE-ECHO
		 * chunk.
		 */
		if (!ctx->packet || !ctx->packet->has_cookie_echo)
			return;

		fallthrough;
	case SCTP_STATE_ESTABLISHED:
	case SCTP_STATE_SHUTDOWN_PENDING:
	case SCTP_STATE_SHUTDOWN_RECEIVED:
		break;

	default:
		/* Do nothing. */
		return;
	}

	/* RFC 2960 6.1  Transmission of DATA Chunks
	 *
	 * C) When the time comes for the sender to transmit,
	 * before sending new DATA chunks, the sender MUST
	 * first transmit any outstanding DATA chunks which
	 * are marked for retransmission (limited by the
	 * current cwnd).
	 */
	if (!list_empty(&ctx->q->retransmit) &&
	    !sctp_outq_flush_rtx(ctx, rtx_timeout))
		return;

	/* Apply Max.Burst limitation to the current transport in
	 * case it will be used for new data.  We are going to
	 * rest it before we return, but we want to apply the limit
	 * to the currently queued data.
	 */
	if (ctx->transport)
		sctp_transport_burst_limited(ctx->transport);

	/* Finally, transmit new packets.  */
	while ((chunk = sctp_outq_dequeue_data(ctx->q)) != NULL) {
		__u32 sid = ntohs(chunk->subh.data_hdr->stream);
		__u8 stream_state = SCTP_SO(&ctx->asoc->stream, sid)->state;

		/* Has this chunk expired? */
		if (sctp_chunk_abandoned(chunk)) {
			sctp_sched_dequeue_done(ctx->q, chunk);
			sctp_chunk_fail(chunk, 0);
			sctp_chunk_free(chunk);
			continue;
		}

		if (stream_state == SCTP_STREAM_CLOSED) {
			sctp_outq_head_data(ctx->q, chunk);
			break;
		}

		sctp_outq_select_transport(ctx, chunk);

		pr_debug("%s: outq:%p, chunk:%p[%s], tx-tsn:0x%x skb->head:%p skb->users:%d\n",
			 __func__, ctx->q, chunk, chunk && chunk->chunk_hdr ?
			 sctp_cname(SCTP_ST_CHUNK(chunk->chunk_hdr->type)) :
			 "illegal chunk", ntohl(chunk->subh.data_hdr->tsn),
			 chunk->skb ? chunk->skb->head : NULL, chunk->skb ?
			 refcount_read(&chunk->skb->users) : -1);

		/* Add the chunk to the packet.  */
		status = sctp_packet_transmit_chunk(ctx->packet, chunk, 0,
						    ctx->gfp);
		if (status != SCTP_XMIT_OK) {
			/* We could not append this chunk, so put
			 * the chunk back on the output queue.
			 */
			pr_debug("%s: could not transmit tsn:0x%x, status:%d\n",
				 __func__, ntohl(chunk->subh.data_hdr->tsn),
				 status);

			sctp_outq_head_data(ctx->q, chunk);
			break;
		}

		/* The sender is in the SHUTDOWN-PENDING state,
		 * The sender MAY set the I-bit in the DATA
		 * chunk header.
		 */
		if (ctx->asoc->state == SCTP_STATE_SHUTDOWN_PENDING)
			chunk->chunk_hdr->flags |= SCTP_DATA_SACK_IMM;
		if (chunk->chunk_hdr->flags & SCTP_DATA_UNORDERED)
			ctx->asoc->stats.ouodchunks++;
		else
			ctx->asoc->stats.oodchunks++;

		/* Only now it's safe to consider this
		 * chunk as sent, sched-wise.
		 */
		sctp_sched_dequeue_done(ctx->q, chunk);

		list_add_tail(&chunk->transmitted_list,
			      &ctx->transport->transmitted);

		sctp_transport_reset_t3_rtx(ctx->transport);
		ctx->transport->last_time_sent = jiffies;

		/* Only let one DATA chunk get bundled with a
		 * COOKIE-ECHO chunk.
		 */
		if (ctx->packet->has_cookie_echo)
			break;
	}
}

static void sctp_outq_flush_transports(struct sctp_flush_ctx *ctx)
{
	struct sock *sk = ctx->asoc->base.sk;
	struct list_head *ltransport;
	struct sctp_packet *packet;
	struct sctp_transport *t;
	int error = 0;

	while ((ltransport = sctp_list_dequeue(&ctx->transport_list)) != NULL) {
		t = list_entry(ltransport, struct sctp_transport, send_ready);
		packet = &t->packet;
		if (!sctp_packet_empty(packet)) {
			rcu_read_lock();
			if (t->dst && __sk_dst_get(sk) != t->dst) {
				dst_hold(t->dst);
				sk_setup_caps(sk, t->dst);
			}
			rcu_read_unlock();
			error = sctp_packet_transmit(packet, ctx->gfp);
			if (error < 0)
				ctx->q->asoc->base.sk->sk_err = -error;
		}

		/* Clear the burst limited state, if any */
		sctp_transport_burst_reset(t);
	}
}

/* Try to flush an outqueue.
 *
 * Description: Send everything in q which we legally can, subject to
 * congestion limitations.
 * * Note: This function can be called from multiple contexts so appropriate
 * locking concerns must be made.  Today we use the sock lock to protect
 * this function.
 */

static void sctp_outq_flush(struct sctp_outq *q, int rtx_timeout, gfp_t gfp)
{
	struct sctp_flush_ctx ctx = {
		.q = q,
		.transport = NULL,
		.transport_list = LIST_HEAD_INIT(ctx.transport_list),
		.asoc = q->asoc,
		.packet = NULL,
		.gfp = gfp,
	};

	/* 6.10 Bundling
	 *   ...
	 *   When bundling control chunks with DATA chunks, an
	 *   endpoint MUST place control chunks first in the outbound
	 *   SCTP packet.  The transmitter MUST transmit DATA chunks
	 *   within a SCTP packet in increasing order of TSN.
	 *   ...
	 */

	sctp_outq_flush_ctrl(&ctx);

	if (q->asoc->src_out_of_asoc_ok)
		goto sctp_flush_out;

	sctp_outq_flush_data(&ctx, rtx_timeout);

sctp_flush_out:

	sctp_outq_flush_transports(&ctx);
}

/* Update unack_data based on the incoming SACK chunk */
static void sctp_sack_update_unack_data(struct sctp_association *assoc,
					struct sctp_sackhdr *sack)
{
	union sctp_sack_variable *frags;
	__u16 unack_data;
	int i;

	unack_data = assoc->next_tsn - assoc->ctsn_ack_point - 1;

	frags = sack->variable;
	for (i = 0; i < ntohs(sack->num_gap_ack_blocks); i++) {
		unack_data -= ((ntohs(frags[i].gab.end) -
				ntohs(frags[i].gab.start) + 1));
	}

	assoc->unack_data = unack_data;
}

/* This is where we REALLY process a SACK.
 *
 * Process the SACK against the outqueue.  Mostly, this just frees
 * things off the transmitted queue.
 */
int sctp_outq_sack(struct sctp_outq *q, struct sctp_chunk *chunk)
{
	struct sctp_association *asoc = q->asoc;
	struct sctp_sackhdr *sack = chunk->subh.sack_hdr;
	struct sctp_transport *transport;
	struct sctp_chunk *tchunk = NULL;
	struct list_head *lchunk, *transport_list, *temp;
	union sctp_sack_variable *frags = sack->variable;
	__u32 sack_ctsn, ctsn, tsn;
	__u32 highest_tsn, highest_new_tsn;
	__u32 sack_a_rwnd;
	unsigned int outstanding;
	struct sctp_transport *primary = asoc->peer.primary_path;
	int count_of_newacks = 0;
	int gap_ack_blocks;
	u8 accum_moved = 0;

	/* Grab the association's destination address list. */
	transport_list = &asoc->peer.transport_addr_list;

	/* SCTP path tracepoint for congestion control debugging. */
	if (trace_sctp_probe_path_enabled()) {
		list_for_each_entry(transport, transport_list, transports)
			trace_sctp_probe_path(transport, asoc);
	}

	sack_ctsn = ntohl(sack->cum_tsn_ack);
	gap_ack_blocks = ntohs(sack->num_gap_ack_blocks);
	asoc->stats.gapcnt += gap_ack_blocks;
	/*
	 * SFR-CACC algorithm:
	 * On receipt of a SACK the sender SHOULD execute the
	 * following statements.
	 *
	 * 1) If the cumulative ack in the SACK passes next tsn_at_change
	 * on the current primary, the CHANGEOVER_ACTIVE flag SHOULD be
	 * cleared. The CYCLING_CHANGEOVER flag SHOULD also be cleared for
	 * all destinations.
	 * 2) If the SACK contains gap acks and the flag CHANGEOVER_ACTIVE
	 * is set the receiver of the SACK MUST take the following actions:
	 *
	 * A) Initialize the cacc_saw_newack to 0 for all destination
	 * addresses.
	 *
	 * Only bother if changeover_active is set. Otherwise, this is
	 * totally suboptimal to do on every SACK.
	 */
	if (primary->cacc.changeover_active) {
		u8 clear_cycling = 0;

		if (TSN_lte(primary->cacc.next_tsn_at_change, sack_ctsn)) {
			primary->cacc.changeover_active = 0;
			clear_cycling = 1;
		}

		if (clear_cycling || gap_ack_blocks) {
			list_for_each_entry(transport, transport_list,
					transports) {
				if (clear_cycling)
					transport->cacc.cycling_changeover = 0;
				if (gap_ack_blocks)
					transport->cacc.cacc_saw_newack = 0;
			}
		}
	}

	/* Get the highest TSN in the sack. */
	highest_tsn = sack_ctsn;
	if (gap_ack_blocks)
		highest_tsn += ntohs(frags[gap_ack_blocks - 1].gab.end);

	if (TSN_lt(asoc->highest_sacked, highest_tsn))
		asoc->highest_sacked = highest_tsn;

	highest_new_tsn = sack_ctsn;

	/* Run through the retransmit queue.  Credit bytes received
	 * and free those chunks that we can.
	 */
	sctp_check_transmitted(q, &q->retransmit, NULL, NULL, sack, &highest_new_tsn);

	/* Run through the transmitted queue.
	 * Credit bytes received and free those chunks which we can.
	 *
	 * This is a MASSIVE candidate for optimization.
	 */
	list_for_each_entry(transport, transport_list, transports) {
		sctp_check_transmitted(q, &transport->transmitted,
				       transport, &chunk->source, sack,
				       &highest_new_tsn);
		/*
		 * SFR-CACC algorithm:
		 * C) Let count_of_newacks be the number of
		 * destinations for which cacc_saw_newack is set.
		 */
		if (transport->cacc.cacc_saw_newack)
			count_of_newacks++;
	}

	/* Move the Cumulative TSN Ack Point if appropriate.  */
	if (TSN_lt(asoc->ctsn_ack_point, sack_ctsn)) {
		asoc->ctsn_ack_point = sack_ctsn;
		accum_moved = 1;
	}

	if (gap_ack_blocks) {

		if (asoc->fast_recovery && accum_moved)
			highest_new_tsn = highest_tsn;

		list_for_each_entry(transport, transport_list, transports)
			sctp_mark_missing(q, &transport->transmitted, transport,
					  highest_new_tsn, count_of_newacks);
	}

	/* Update unack_data field in the assoc. */
	sctp_sack_update_unack_data(asoc, sack);

	ctsn = asoc->ctsn_ack_point;

	/* Throw away stuff rotting on the sack queue.  */
	list_for_each_safe(lchunk, temp, &q->sacked) {
		tchunk = list_entry(lchunk, struct sctp_chunk,
				    transmitted_list);
		tsn = ntohl(tchunk->subh.data_hdr->tsn);
		if (TSN_lte(tsn, ctsn)) {
			list_del_init(&tchunk->transmitted_list);
			if (asoc->peer.prsctp_capable &&
			    SCTP_PR_PRIO_ENABLED(chunk->sinfo.sinfo_flags))
				asoc->sent_cnt_removable--;
			sctp_chunk_free(tchunk);
		}
	}

	/* ii) Set rwnd equal to the newly received a_rwnd minus the
	 *     number of bytes still outstanding after processing the
	 *     Cumulative TSN Ack and the Gap Ack Blocks.
	 */

	sack_a_rwnd = ntohl(sack->a_rwnd);
	asoc->peer.zero_window_announced = !sack_a_rwnd;
	outstanding = q->outstanding_bytes;

	if (outstanding < sack_a_rwnd)
		sack_a_rwnd -= outstanding;
	else
		sack_a_rwnd = 0;

	asoc->peer.rwnd = sack_a_rwnd;

	asoc->stream.si->generate_ftsn(q, sack_ctsn);

	pr_debug("%s: sack cumulative tsn ack:0x%x\n", __func__, sack_ctsn);
	pr_debug("%s: cumulative tsn ack of assoc:%p is 0x%x, "
		 "advertised peer ack point:0x%x\n", __func__, asoc, ctsn,
		 asoc->adv_peer_ack_point);

	return sctp_outq_is_empty(q);
}

/* Is the outqueue empty?
 * The queue is empty when we have not pending data, no in-flight data
 * and nothing pending retransmissions.
 */
int sctp_outq_is_empty(const struct sctp_outq *q)
{
	return q->out_qlen == 0 && q->outstanding_bytes == 0 &&
	       list_empty(&q->retransmit);
}

/********************************************************************
 * 2nd Level Abstractions
 ********************************************************************/

/* Go through a transport's transmitted list or the association's retransmit
 * list and move chunks that are acked by the Cumulative TSN Ack to q->sacked.
 * The retransmit list will not have an associated transport.
 *
 * I added coherent debug information output.	--xguo
 *
 * Instead of printing 'sacked' or 'kept' for each TSN on the
 * transmitted_queue, we print a range: SACKED: TSN1-TSN2, TSN3, TSN4-TSN5.
 * KEPT TSN6-TSN7, etc.
 */
static void sctp_check_transmitted(struct sctp_outq *q,
				   struct list_head *transmitted_queue,
				   struct sctp_transport *transport,
				   union sctp_addr *saddr,
				   struct sctp_sackhdr *sack,
				   __u32 *highest_new_tsn_in_sack)
{
	struct list_head *lchunk;
	struct sctp_chunk *tchunk;
	struct list_head tlist;
	__u32 tsn;
	__u32 sack_ctsn;
	__u32 rtt;
	__u8 restart_timer = 0;
	int bytes_acked = 0;
	int migrate_bytes = 0;
	bool forward_progress = false;

	sack_ctsn = ntohl(sack->cum_tsn_ack);

	INIT_LIST_HEAD(&tlist);

	/* The while loop will skip empty transmitted queues. */
	while (NULL != (lchunk = sctp_list_dequeue(transmitted_queue))) {
		tchunk = list_entry(lchunk, struct sctp_chunk,
				    transmitted_list);

		if (sctp_chunk_abandoned(tchunk)) {
			/* Move the chunk to abandoned list. */
			sctp_insert_list(&q->abandoned, lchunk);

			/* If this chunk has not been acked, stop
			 * considering it as 'outstanding'.
			 */
			if (transmitted_queue != &q->retransmit &&
			    !tchunk->tsn_gap_acked) {
				if (tchunk->transport)
					tchunk->transport->flight_size -=
							sctp_data_size(tchunk);
				q->outstanding_bytes -= sctp_data_size(tchunk);
			}
			continue;
		}

		tsn = ntohl(tchunk->subh.data_hdr->tsn);
		if (sctp_acked(sack, tsn)) {
			/* If this queue is the retransmit queue, the
			 * retransmit timer has already reclaimed
			 * the outstanding bytes for this chunk, so only
			 * count bytes associated with a transport.
			 */
			if (transport && !tchunk->tsn_gap_acked) {
				/* If this chunk is being used for RTT
				 * measurement, calculate the RTT and update
				 * the RTO using this value.
				 *
				 * 6.3.1 C5) Karn's algorithm: RTT measurements
				 * MUST NOT be made using packets that were
				 * retransmitted (and thus for which it is
				 * ambiguous whether the reply was for the
				 * first instance of the packet or a later
				 * instance).
				 */
				if (!sctp_chunk_retransmitted(tchunk) &&
				    tchunk->rtt_in_progress) {
					tchunk->rtt_in_progress = 0;
					rtt = jiffies - tchunk->sent_at;
					sctp_transport_update_rto(transport,
								  rtt);
				}

				if (TSN_lte(tsn, sack_ctsn)) {
					/*
					 * SFR-CACC algorithm:
					 * 2) If the SACK contains gap acks
					 * and the flag CHANGEOVER_ACTIVE is
					 * set the receiver of the SACK MUST
					 * take the following action:
					 *
					 * B) For each TSN t being acked that
					 * has not been acked in any SACK so
					 * far, set cacc_saw_newack to 1 for
					 * the destination that the TSN was
					 * sent to.
					 */
					if (sack->num_gap_ack_blocks &&
					    q->asoc->peer.primary_path->cacc.
					    changeover_active)
						transport->cacc.cacc_saw_newack
							= 1;
				}
			}

			/* If the chunk hasn't been marked as ACKED,
			 * mark it and account bytes_acked if the
			 * chunk had a valid transport (it will not
			 * have a transport if ASCONF had deleted it
			 * while DATA was outstanding).
			 */
			if (!tchunk->tsn_gap_acked) {
				tchunk->tsn_gap_acked = 1;
				if (TSN_lt(*highest_new_tsn_in_sack, tsn))
					*highest_new_tsn_in_sack = tsn;
				bytes_acked += sctp_data_size(tchunk);
				if (!tchunk->transport)
					migrate_bytes += sctp_data_size(tchunk);
				forward_progress = true;
			}

			if (TSN_lte(tsn, sack_ctsn)) {
				/* RFC 2960  6.3.2 Retransmission Timer Rules
				 *
				 * R3) Whenever a SACK is received
				 * that acknowledges the DATA chunk
				 * with the earliest outstanding TSN
				 * for that address, restart T3-rtx
				 * timer for that address with its
				 * current RTO.
				 */
				restart_timer = 1;
				forward_progress = true;

				list_add_tail(&tchunk->transmitted_list,
					      &q->sacked);
			} else {
				/* RFC2960 7.2.4, sctpimpguide-05 2.8.2
				 * M2) Each time a SACK arrives reporting
				 * 'Stray DATA chunk(s)' record the highest TSN
				 * reported as newly acknowledged, call this
				 * value 'HighestTSNinSack'. A newly
				 * acknowledged DATA chunk is one not
				 * previously acknowledged in a SACK.
				 *
				 * When the SCTP sender of data receives a SACK
				 * chunk that acknowledges, for the first time,
				 * the receipt of a DATA chunk, all the still
				 * unacknowledged DATA chunks whose TSN is
				 * older than that newly acknowledged DATA
				 * chunk, are qualified as 'Stray DATA chunks'.
				 */
				list_add_tail(lchunk, &tlist);
			}
		} else {
			if (tchunk->tsn_gap_acked) {
				pr_debug("%s: receiver reneged on data TSN:0x%x\n",
					 __func__, tsn);

				tchunk->tsn_gap_acked = 0;

				if (tchunk->transport)
					bytes_acked -= sctp_data_size(tchunk);

				/* RFC 2960 6.3.2 Retransmission Timer Rules
				 *
				 * R4) Whenever a SACK is received missing a
				 * TSN that was previously acknowledged via a
				 * Gap Ack Block, start T3-rtx for the
				 * destination address to which the DATA
				 * chunk was originally
				 * transmitted if it is not already running.
				 */
				restart_timer = 1;
			}

			list_add_tail(lchunk, &tlist);
		}
	}

	if (transport) {
		if (bytes_acked) {
			struct sctp_association *asoc = transport->asoc;

			/* We may have counted DATA that was migrated
			 * to this transport due to DEL-IP operation.
			 * Subtract those bytes, since the were never
			 * send on this transport and shouldn't be
			 * credited to this transport.
			 */
			bytes_acked -= migrate_bytes;

			/* 8.2. When an outstanding TSN is acknowledged,
			 * the endpoint shall clear the error counter of
			 * the destination transport address to which the
			 * DATA chunk was last sent.
			 * The association's overall error counter is
			 * also cleared.
			 */
			transport->error_count = 0;
			transport->asoc->overall_error_count = 0;
			forward_progress = true;

			/*
			 * While in SHUTDOWN PENDING, we may have started
			 * the T5 shutdown guard timer after reaching the
			 * retransmission limit. Stop that timer as soon
			 * as the receiver acknowledged any data.
			 */
			if (asoc->state == SCTP_STATE_SHUTDOWN_PENDING &&
			    del_timer(&asoc->timers
				[SCTP_EVENT_TIMEOUT_T5_SHUTDOWN_GUARD]))
					sctp_association_put(asoc);

			/* Mark the destination transport address as
			 * active if it is not so marked.
			 */
			if ((transport->state == SCTP_INACTIVE ||
			     transport->state == SCTP_UNCONFIRMED) &&
			    sctp_cmp_addr_exact(&transport->ipaddr, saddr)) {
				sctp_assoc_control_transport(
					transport->asoc,
					transport,
					SCTP_TRANSPORT_UP,
					SCTP_RECEIVED_SACK);
			}

			sctp_transport_raise_cwnd(transport, sack_ctsn,
						  bytes_acked);

			transport->flight_size -= bytes_acked;
			if (transport->flight_size == 0)
				transport->partial_bytes_acked = 0;
			q->outstanding_bytes -= bytes_acked + migrate_bytes;
		} else {
			/* RFC 2960 6.1, sctpimpguide-06 2.15.2
			 * When a sender is doing zero window probing, it
			 * should not timeout the association if it continues
			 * to receive new packets from the receiver. The
			 * reason is that the receiver MAY keep its window
			 * closed for an indefinite time.
			 * A sender is doing zero window probing when the
			 * receiver's advertised window is zero, and there is
			 * only one data chunk in flight to the receiver.
			 *
			 * Allow the association to timeout while in SHUTDOWN
			 * PENDING or SHUTDOWN RECEIVED in case the receiver
			 * stays in zero window mode forever.
			 */
			if (!q->asoc->peer.rwnd &&
			    !list_empty(&tlist) &&
			    (sack_ctsn+2 == q->asoc->next_tsn) &&
			    q->asoc->state < SCTP_STATE_SHUTDOWN_PENDING) {
				pr_debug("%s: sack received for zero window "
					 "probe:%u\n", __func__, sack_ctsn);

				q->asoc->overall_error_count = 0;
				transport->error_count = 0;
			}
		}

		/* RFC 2960 6.3.2 Retransmission Timer Rules
		 *
		 * R2) Whenever all outstanding data sent to an address have
		 * been acknowledged, turn off the T3-rtx timer of that
		 * address.
		 */
		if (!transport->flight_size) {
			if (del_timer(&transport->T3_rtx_timer))
				sctp_transport_put(transport);
		} else if (restart_timer) {
			if (!mod_timer(&transport->T3_rtx_timer,
				       jiffies + transport->rto))
				sctp_transport_hold(transport);
		}

		if (forward_progress) {
			if (transport->dst)
				sctp_transport_dst_confirm(transport);
		}
	}

	list_splice(&tlist, transmitted_queue);
}

/* Mark chunks as missing and consequently may get retransmitted. */
static void sctp_mark_missing(struct sctp_outq *q,
			      struct list_head *transmitted_queue,
			      struct sctp_transport *transport,
			      __u32 highest_new_tsn_in_sack,
			      int count_of_newacks)
{
	struct sctp_chunk *chunk;
	__u32 tsn;
	char do_fast_retransmit = 0;
	struct sctp_association *asoc = q->asoc;
	struct sctp_transport *primary = asoc->peer.primary_path;

	list_for_each_entry(chunk, transmitted_queue, transmitted_list) {

		tsn = ntohl(chunk->subh.data_hdr->tsn);

		/* RFC 2960 7.2.4, sctpimpguide-05 2.8.2 M3) Examine all
		 * 'Unacknowledged TSN's', if the TSN number of an
		 * 'Unacknowledged TSN' is smaller than the 'HighestTSNinSack'
		 * value, increment the 'TSN.Missing.Report' count on that
		 * chunk if it has NOT been fast retransmitted or marked for
		 * fast retransmit already.
		 */
		if (chunk->fast_retransmit == SCTP_CAN_FRTX &&
		    !chunk->tsn_gap_acked &&
		    TSN_lt(tsn, highest_new_tsn_in_sack)) {

			/* SFR-CACC may require us to skip marking
			 * this chunk as missing.
			 */
			if (!transport || !sctp_cacc_skip(primary,
						chunk->transport,
						count_of_newacks, tsn)) {
				chunk->tsn_missing_report++;

				pr_debug("%s: tsn:0x%x missing counter:%d\n",
					 __func__, tsn, chunk->tsn_missing_report);
			}
		}
		/*
		 * M4) If any DATA chunk is found to have a
		 * 'TSN.Missing.Report'
		 * value larger than or equal to 3, mark that chunk for
		 * retransmission and start the fast retransmit procedure.
		 */

		if (chunk->tsn_missing_report >= 3) {
			chunk->fast_retransmit = SCTP_NEED_FRTX;
			do_fast_retransmit = 1;
		}
	}

	if (transport) {
		if (do_fast_retransmit)
			sctp_retransmit(q, transport, SCTP_RTXR_FAST_RTX);

		pr_debug("%s: transport:%p, cwnd:%d, ssthresh:%d, "
			 "flight_size:%d, pba:%d\n",  __func__, transport,
			 transport->cwnd, transport->ssthresh,
			 transport->flight_size, transport->partial_bytes_acked);
	}
}

/* Is the given TSN acked by this packet?  */
static int sctp_acked(struct sctp_sackhdr *sack, __u32 tsn)
{
	__u32 ctsn = ntohl(sack->cum_tsn_ack);
	union sctp_sack_variable *frags;
	__u16 tsn_offset, blocks;
	int i;

	if (TSN_lte(tsn, ctsn))
		goto pass;

	/* 3.3.4 Selective Acknowledgment (SACK) (3):
	 *
	 * Gap Ack Blocks:
	 *  These fields contain the Gap Ack Blocks. They are repeated
	 *  for each Gap Ack Block up to the number of Gap Ack Blocks
	 *  defined in the Number of Gap Ack Blocks field. All DATA
	 *  chunks with TSNs greater than or equal to (Cumulative TSN
	 *  Ack + Gap Ack Block Start) and less than or equal to
	 *  (Cumulative TSN Ack + Gap Ack Block End) of each Gap Ack
	 *  Block are assumed to have been received correctly.
	 */

	frags = sack->variable;
	blocks = ntohs(sack->num_gap_ack_blocks);
	tsn_offset = tsn - ctsn;
	for (i = 0; i < blocks; ++i) {
		if (tsn_offset >= ntohs(frags[i].gab.start) &&
		    tsn_offset <= ntohs(frags[i].gab.end))
			goto pass;
	}

	return 0;
pass:
	return 1;
}

static inline int sctp_get_skip_pos(struct sctp_fwdtsn_skip *skiplist,
				    int nskips, __be16 stream)
{
	int i;

	for (i = 0; i < nskips; i++) {
		if (skiplist[i].stream == stream)
			return i;
	}
	return i;
}

/* Create and add a fwdtsn chunk to the outq's control queue if needed. */
void sctp_generate_fwdtsn(struct sctp_outq *q, __u32 ctsn)
{
	struct sctp_association *asoc = q->asoc;
	struct sctp_chunk *ftsn_chunk = NULL;
	struct sctp_fwdtsn_skip ftsn_skip_arr[10];
	int nskips = 0;
	int skip_pos = 0;
	__u32 tsn;
	struct sctp_chunk *chunk;
	struct list_head *lchunk, *temp;

	if (!asoc->peer.prsctp_capable)
		return;

	/* PR-SCTP C1) Let SackCumAck be the Cumulative TSN ACK carried in the
	 * received SACK.
	 *
	 * If (Advanced.Peer.Ack.Point < SackCumAck), then update
	 * Advanced.Peer.Ack.Point to be equal to SackCumAck.
	 */
	if (TSN_lt(asoc->adv_peer_ack_point, ctsn))
		asoc->adv_peer_ack_point = ctsn;

	/* PR-SCTP C2) Try to further advance the "Advanced.Peer.Ack.Point"
	 * locally, that is, to move "Advanced.Peer.Ack.Point" up as long as
	 * the chunk next in the out-queue space is marked as "abandoned" as
	 * shown in the following example:
	 *
	 * Assuming that a SACK arrived with the Cumulative TSN ACK 102
	 * and the Advanced.Peer.Ack.Point is updated to this value:
	 *
	 *   out-queue at the end of  ==>   out-queue after Adv.Ack.Point
	 *   normal SACK processing           local advancement
	 *                ...                           ...
	 *   Adv.Ack.Pt-> 102 acked                     102 acked
	 *                103 abandoned                 103 abandoned
	 *                104 abandoned     Adv.Ack.P-> 104 abandoned
	 *                105                           105
	 *                106 acked                     106 acked
	 *                ...                           ...
	 *
	 * In this example, the data sender successfully advanced the
	 * "Advanced.Peer.Ack.Point" from 102 to 104 locally.
	 */
	list_for_each_safe(lchunk, temp, &q->abandoned) {
		chunk = list_entry(lchunk, struct sctp_chunk,
					transmitted_list);
		tsn = ntohl(chunk->subh.data_hdr->tsn);

		/* Remove any chunks in the abandoned queue that are acked by
		 * the ctsn.
		 */
		if (TSN_lte(tsn, ctsn)) {
			list_del_init(lchunk);
			sctp_chunk_free(chunk);
		} else {
			if (TSN_lte(tsn, asoc->adv_peer_ack_point+1)) {
				asoc->adv_peer_ack_point = tsn;
				if (chunk->chunk_hdr->flags &
					 SCTP_DATA_UNORDERED)
					continue;
				skip_pos = sctp_get_skip_pos(&ftsn_skip_arr[0],
						nskips,
						chunk->subh.data_hdr->stream);
				ftsn_skip_arr[skip_pos].stream =
					chunk->subh.data_hdr->stream;
				ftsn_skip_arr[skip_pos].ssn =
					 chunk->subh.data_hdr->ssn;
				if (skip_pos == nskips)
					nskips++;
				if (nskips == 10)
					break;
			} else
				break;
		}
	}

	/* PR-SCTP C3) If, after step C1 and C2, the "Advanced.Peer.Ack.Point"
	 * is greater than the Cumulative TSN ACK carried in the received
	 * SACK, the data sender MUST send the data receiver a FORWARD TSN
	 * chunk containing the latest value of the
	 * "Advanced.Peer.Ack.Point".
	 *
	 * C4) For each "abandoned" TSN the sender of the FORWARD TSN SHOULD
	 * list each stream and sequence number in the forwarded TSN. This
	 * information will enable the receiver to easily find any
	 * stranded TSN's waiting on stream reorder queues. Each stream
	 * SHOULD only be reported once; this means that if multiple
	 * abandoned messages occur in the same stream then only the
	 * highest abandoned stream sequence number is reported. If the
	 * total size of the FORWARD TSN does NOT fit in a single MTU then
	 * the sender of the FORWARD TSN SHOULD lower the
	 * Advanced.Peer.Ack.Point to the last TSN that will fit in a
	 * single MTU.
	 */
	if (asoc->adv_peer_ack_point > ctsn)
		ftsn_chunk = sctp_make_fwdtsn(asoc, asoc->adv_peer_ack_point,
					      nskips, &ftsn_skip_arr[0]);

	if (ftsn_chunk) {
		list_add_tail(&ftsn_chunk->list, &q->control_chunk_list);
		SCTP_INC_STATS(asoc->base.net, SCTP_MIB_OUTCTRLCHUNKS);
	}
}
