// SPDX-License-Identifier: GPL-2.0-or-later
/* SCTP kernel implementation
 * Copyright (c) 1999-2000 Cisco, Inc.
 * Copyright (c) 1999-2001 Motorola, Inc.
 * Copyright (c) 2001-2003 International Business Machines Corp.
 * Copyright (c) 2001 Intel Corp.
 * Copyright (c) 2001 La Monte H.P. Yarroll
 *
 * This file is part of the SCTP kernel implementation
 *
 * This module provides the abstraction for an SCTP transport representing
 * a remote transport address.  For local transport addresses, we just use
 * union sctp_addr.
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
 *    Ardelle Fan	    <ardelle.fan@intel.com>
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/slab.h>
#include <linux/types.h>
#include <linux/random.h>
#include <net/sctp/sctp.h>
#include <net/sctp/sm.h>

/* 1st Level Abstractions.  */

/* Initialize a new transport from provided memory.  */
static struct sctp_transport *sctp_transport_init(struct net *net,
						  struct sctp_transport *peer,
						  const union sctp_addr *addr,
						  gfp_t gfp)
{
	/* Copy in the address.  */
	peer->af_specific = sctp_get_af_specific(addr->sa.sa_family);
	memcpy(&peer->ipaddr, addr, peer->af_specific->sockaddr_len);
	memset(&peer->saddr, 0, sizeof(union sctp_addr));

	peer->sack_generation = 0;

	/* From 6.3.1 RTO Calculation:
	 *
	 * C1) Until an RTT measurement has been made for a packet sent to the
	 * given destination transport address, set RTO to the protocol
	 * parameter 'RTO.Initial'.
	 */
	peer->rto = msecs_to_jiffies(net->sctp.rto_initial);

	peer->last_time_heard = 0;
	peer->last_time_ecne_reduced = jiffies;

	peer->param_flags = SPP_HB_DISABLE |
			    SPP_PMTUD_ENABLE |
			    SPP_SACKDELAY_ENABLE;

	/* Initialize the default path max_retrans.  */
	peer->pathmaxrxt  = net->sctp.max_retrans_path;
	peer->pf_retrans  = net->sctp.pf_retrans;

	INIT_LIST_HEAD(&peer->transmitted);
	INIT_LIST_HEAD(&peer->send_ready);
	INIT_LIST_HEAD(&peer->transports);

	timer_setup(&peer->T3_rtx_timer, sctp_generate_t3_rtx_event, 0);
	timer_setup(&peer->hb_timer, sctp_generate_heartbeat_event, 0);
	timer_setup(&peer->reconf_timer, sctp_generate_reconf_event, 0);
	timer_setup(&peer->proto_unreach_timer,
		    sctp_generate_proto_unreach_event, 0);

	/* Initialize the 64-bit random nonce sent with heartbeat. */
	get_random_bytes(&peer->hb_nonce, sizeof(peer->hb_nonce));

	refcount_set(&peer->refcnt, 1);

	return peer;
}

/* Allocate and initialize a new transport.  */
struct sctp_transport *sctp_transport_new(struct net *net,
					  const union sctp_addr *addr,
					  gfp_t gfp)
{
	struct sctp_transport *transport;

	transport = kzalloc(sizeof(*transport), gfp);
	if (!transport)
		goto fail;

	if (!sctp_transport_init(net, transport, addr, gfp))
		goto fail_init;

	SCTP_DBG_OBJCNT_INC(transport);

	return transport;

fail_init:
	kfree(transport);

fail:
	return NULL;
}

/* This transport is no longer needed.  Free up if possible, or
 * delay until it last reference count.
 */
void sctp_transport_free(struct sctp_transport *transport)
{
	/* Try to delete the heartbeat timer.  */
	if (del_timer(&transport->hb_timer))
		sctp_transport_put(transport);

	/* Delete the T3_rtx timer if it's active.
	 * There is no point in not doing this now and letting
	 * structure hang around in memory since we know
	 * the transport is going away.
	 */
	if (del_timer(&transport->T3_rtx_timer))
		sctp_transport_put(transport);

	if (del_timer(&transport->reconf_timer))
		sctp_transport_put(transport);

	/* Delete the ICMP proto unreachable timer if it's active. */
	if (del_timer(&transport->proto_unreach_timer))
		sctp_transport_put(transport);

	sctp_transport_put(transport);
}

static void sctp_transport_destroy_rcu(struct rcu_head *head)
{
	struct sctp_transport *transport;

	transport = container_of(head, struct sctp_transport, rcu);

	dst_release(transport->dst);
	kfree(transport);
	SCTP_DBG_OBJCNT_DEC(transport);
}

/* Destroy the transport data structure.
 * Assumes there are no more users of this structure.
 */
static void sctp_transport_destroy(struct sctp_transport *transport)
{
	if (unlikely(refcount_read(&transport->refcnt))) {
		WARN(1, "Attempt to destroy undead transport %p!\n", transport);
		return;
	}

	sctp_packet_free(&transport->packet);

	if (transport->asoc)
		sctp_association_put(transport->asoc);

	call_rcu(&transport->rcu, sctp_transport_destroy_rcu);
}

/* Start T3_rtx timer if it is not already running and update the heartbeat
 * timer.  This routine is called every time a DATA chunk is sent.
 */
void sctp_transport_reset_t3_rtx(struct sctp_transport *transport)
{
	/* RFC 2960 6.3.2 Retransmission Timer Rules
	 *
	 * R1) Every time a DATA chunk is sent to any address(including a
	 * retransmission), if the T3-rtx timer of that address is not running
	 * start it running so that it will expire after the RTO of that
	 * address.
	 */

	if (!timer_pending(&transport->T3_rtx_timer))
		if (!mod_timer(&transport->T3_rtx_timer,
			       jiffies + transport->rto))
			sctp_transport_hold(transport);
}

void sctp_transport_reset_hb_timer(struct sctp_transport *transport)
{
	unsigned long expires;

	/* When a data chunk is sent, reset the heartbeat interval.  */
	expires = jiffies + sctp_transport_timeout(transport);
	if ((time_before(transport->hb_timer.expires, expires) ||
	     !timer_pending(&transport->hb_timer)) &&
	    !mod_timer(&transport->hb_timer,
		       expires + prandom_u32_max(transport->rto)))
		sctp_transport_hold(transport);
}

void sctp_transport_reset_reconf_timer(struct sctp_transport *transport)
{
	if (!timer_pending(&transport->reconf_timer))
		if (!mod_timer(&transport->reconf_timer,
			       jiffies + transport->rto))
			sctp_transport_hold(transport);
}

/* This transport has been assigned to an association.
 * Initialize fields from the association or from the sock itself.
 * Register the reference count in the association.
 */
void sctp_transport_set_owner(struct sctp_transport *transport,
			      struct sctp_association *asoc)
{
	transport->asoc = asoc;
	sctp_association_hold(asoc);
}

/* Initialize the pmtu of a transport. */
void sctp_transport_pmtu(struct sctp_transport *transport, struct sock *sk)
{
	/* If we don't have a fresh route, look one up */
	if (!transport->dst || transport->dst->obsolete) {
		sctp_transport_dst_release(transport);
		transport->af_specific->get_dst(transport, &transport->saddr,
						&transport->fl, sk);
	}

	if (transport->param_flags & SPP_PMTUD_DISABLE) {
		struct sctp_association *asoc = transport->asoc;

		if (!transport->pathmtu && asoc && asoc->pathmtu)
			transport->pathmtu = asoc->pathmtu;
		if (transport->pathmtu)
			return;
	}

	if (transport->dst)
		transport->pathmtu = sctp_dst_mtu(transport->dst);
	else
		transport->pathmtu = SCTP_DEFAULT_MAXSEGMENT;
}

bool sctp_transport_update_pmtu(struct sctp_transport *t, u32 pmtu)
{
	struct dst_entry *dst = sctp_transport_dst_check(t);
	struct sock *sk = t->asoc->base.sk;
	bool change = true;

	if (unlikely(pmtu < SCTP_DEFAULT_MINSEGMENT)) {
		pr_warn_ratelimited("%s: Reported pmtu %d too low, using default minimum of %d\n",
				    __func__, pmtu, SCTP_DEFAULT_MINSEGMENT);
		/* Use default minimum segment instead */
		pmtu = SCTP_DEFAULT_MINSEGMENT;
	}
	pmtu = SCTP_TRUNC4(pmtu);

	if (dst) {
		struct sctp_pf *pf = sctp_get_pf_specific(dst->ops->family);
		union sctp_addr addr;

		pf->af->from_sk(&addr, sk);
		pf->to_sk_daddr(&t->ipaddr, sk);
		dst->ops->update_pmtu(dst, sk, NULL, pmtu, true);
		pf->to_sk_daddr(&addr, sk);

		dst = sctp_transport_dst_check(t);
	}

	if (!dst) {
		t->af_specific->get_dst(t, &t->saddr, &t->fl, sk);
		dst = t->dst;
	}

	if (dst) {
		/* Re-fetch, as under layers may have a higher minimum size */
		pmtu = sctp_dst_mtu(dst);
		change = t->pathmtu != pmtu;
	}
	t->pathmtu = pmtu;

	return change;
}

/* Caches the dst entry and source address for a transport's destination
 * address.
 */
void sctp_transport_route(struct sctp_transport *transport,
			  union sctp_addr *saddr, struct sctp_sock *opt)
{
	struct sctp_association *asoc = transport->asoc;
	struct sctp_af *af = transport->af_specific;

	sctp_transport_dst_release(transport);
	af->get_dst(transport, saddr, &transport->fl, sctp_opt2sk(opt));

	if (saddr)
		memcpy(&transport->saddr, saddr, sizeof(union sctp_addr));
	else
		af->get_saddr(opt, transport, &transport->fl);

	sctp_transport_pmtu(transport, sctp_opt2sk(opt));

	/* Initialize sk->sk_rcv_saddr, if the transport is the
	 * association's active path for getsockname().
	 */
	if (transport->dst && asoc &&
	    (!asoc->peer.primary_path || transport == asoc->peer.active_path))
		opt->pf->to_sk_saddr(&transport->saddr, asoc->base.sk);
}

/* Hold a reference to a transport.  */
int sctp_transport_hold(struct sctp_transport *transport)
{
	return refcount_inc_not_zero(&transport->refcnt);
}

/* Release a reference to a transport and clean up
 * if there are no more references.
 */
void sctp_transport_put(struct sctp_transport *transport)
{
	if (refcount_dec_and_test(&transport->refcnt))
		sctp_transport_destroy(transport);
}

/* Update transport's RTO based on the newly calculated RTT. */
void sctp_transport_update_rto(struct sctp_transport *tp, __u32 rtt)
{
	if (unlikely(!tp->rto_pending))
		/* We should not be doing any RTO updates unless rto_pending is set.  */
		pr_debug("%s: rto_pending not set on transport %p!\n", __func__, tp);

	if (tp->rttvar || tp->srtt) {
		struct net *net = tp->asoc->base.net;
		/* 6.3.1 C3) When a new RTT measurement R' is made, set
		 * RTTVAR <- (1 - RTO.Beta) * RTTVAR + RTO.Beta * |SRTT - R'|
		 * SRTT <- (1 - RTO.Alpha) * SRTT + RTO.Alpha * R'
		 */

		/* Note:  The above algorithm has been rewritten to
		 * express rto_beta and rto_alpha as inverse powers
		 * of two.
		 * For example, assuming the default value of RTO.Alpha of
		 * 1/8, rto_alpha would be expressed as 3.
		 */
		tp->rttvar = tp->rttvar - (tp->rttvar >> net->sctp.rto_beta)
			+ (((__u32)abs((__s64)tp->srtt - (__s64)rtt)) >> net->sctp.rto_beta);
		tp->srtt = tp->srtt - (tp->srtt >> net->sctp.rto_alpha)
			+ (rtt >> net->sctp.rto_alpha);
	} else {
		/* 6.3.1 C2) When the first RTT measurement R is made, set
		 * SRTT <- R, RTTVAR <- R/2.
		 */
		tp->srtt = rtt;
		tp->rttvar = rtt >> 1;
	}

	/* 6.3.1 G1) Whenever RTTVAR is computed, if RTTVAR = 0, then
	 * adjust RTTVAR <- G, where G is the CLOCK GRANULARITY.
	 */
	if (tp->rttvar == 0)
		tp->rttvar = SCTP_CLOCK_GRANULARITY;

	/* 6.3.1 C3) After the computation, update RTO <- SRTT + 4 * RTTVAR. */
	tp->rto = tp->srtt + (tp->rttvar << 2);

	/* 6.3.1 C6) Whenever RTO is computed, if it is less than RTO.Min
	 * seconds then it is rounded up to RTO.Min seconds.
	 */
	if (tp->rto < tp->asoc->rto_min)
		tp->rto = tp->asoc->rto_min;

	/* 6.3.1 C7) A maximum value may be placed on RTO provided it is
	 * at least RTO.max seconds.
	 */
	if (tp->rto > tp->asoc->rto_max)
		tp->rto = tp->asoc->rto_max;

	sctp_max_rto(tp->asoc, tp);
	tp->rtt = rtt;

	/* Reset rto_pending so that a new RTT measurement is started when a
	 * new data chunk is sent.
	 */
	tp->rto_pending = 0;

	pr_debug("%s: transport:%p, rtt:%d, srtt:%d rttvar:%d, rto:%ld\n",
		 __func__, tp, rtt, tp->srtt, tp->rttvar, tp->rto);
}

/* This routine updates the transport's cwnd and partial_bytes_acked
 * parameters based on the bytes acked in the received SACK.
 */
void sctp_transport_raise_cwnd(struct sctp_transport *transport,
			       __u32 sack_ctsn, __u32 bytes_acked)
{
	struct sctp_association *asoc = transport->asoc;
	__u32 cwnd, ssthresh, flight_size, pba, pmtu;

	cwnd = transport->cwnd;
	flight_size = transport->flight_size;

	/* See if we need to exit Fast Recovery first */
	if (asoc->fast_recovery &&
	    TSN_lte(asoc->fast_recovery_exit, sack_ctsn))
		asoc->fast_recovery = 0;

	ssthresh = transport->ssthresh;
	pba = transport->partial_bytes_acked;
	pmtu = transport->asoc->pathmtu;

	if (cwnd <= ssthresh) {
		/* RFC 4960 7.2.1
		 * o  When cwnd is less than or equal to ssthresh, an SCTP
		 *    endpoint MUST use the slow-start algorithm to increase
		 *    cwnd only if the current congestion window is being fully
		 *    utilized, an incoming SACK advances the Cumulative TSN
		 *    Ack Point, and the data sender is not in Fast Recovery.
		 *    Only when these three conditions are met can the cwnd be
		 *    increased; otherwise, the cwnd MUST not be increased.
		 *    If these conditions are met, then cwnd MUST be increased
		 *    by, at most, the lesser of 1) the total size of the
		 *    previously outstanding DATA chunk(s) acknowledged, and
		 *    2) the destination's path MTU.  This upper bound protects
		 *    against the ACK-Splitting attack outlined in [SAVAGE99].
		 */
		if (asoc->fast_recovery)
			return;

		/* The appropriate cwnd increase algorithm is performed
		 * if, and only if the congestion window is being fully
		 * utilized.  Note that RFC4960 Errata 3.22 removed the
		 * other condition on ctsn moving.
		 */
		if (flight_size < cwnd)
			return;

		if (bytes_acked > pmtu)
			cwnd += pmtu;
		else
			cwnd += bytes_acked;

		pr_debug("%s: slow start: transport:%p, bytes_acked:%d, "
			 "cwnd:%d, ssthresh:%d, flight_size:%d, pba:%d\n",
			 __func__, transport, bytes_acked, cwnd, ssthresh,
			 flight_size, pba);
	} else {
		/* RFC 2960 7.2.2 Whenever cwnd is greater than ssthresh,
		 * upon each SACK arrival, increase partial_bytes_acked
		 * by the total number of bytes of all new chunks
		 * acknowledged in that SACK including chunks
		 * acknowledged by the new Cumulative TSN Ack and by Gap
		 * Ack Blocks. (updated by RFC4960 Errata 3.22)
		 *
		 * When partial_bytes_acked is greater than cwnd and
		 * before the arrival of the SACK the sender had less
		 * bytes of data outstanding than cwnd (i.e., before
		 * arrival of the SACK, flightsize was less than cwnd),
		 * reset partial_bytes_acked to cwnd. (RFC 4960 Errata
		 * 3.26)
		 *
		 * When partial_bytes_acked is equal to or greater than
		 * cwnd and before the arrival of the SACK the sender
		 * had cwnd or more bytes of data outstanding (i.e.,
		 * before arrival of the SACK, flightsize was greater
		 * than or equal to cwnd), partial_bytes_acked is reset
		 * to (partial_bytes_acked - cwnd). Next, cwnd is
		 * increased by MTU. (RFC 4960 Errata 3.12)
		 */
		pba += bytes_acked;
		if (pba > cwnd && flight_size < cwnd)
			pba = cwnd;
		if (pba >= cwnd && flight_size >= cwnd) {
			pba = pba - cwnd;
			cwnd += pmtu;
		}

		pr_debug("%s: congestion avoidance: transport:%p, "
			 "bytes_acked:%d, cwnd:%d, ssthresh:%d, "
			 "flight_size:%d, pba:%d\n", __func__,
			 transport, bytes_acked, cwnd, ssthresh,
			 flight_size, pba);
	}

	transport->cwnd = cwnd;
	transport->partial_bytes_acked = pba;
}

/* This routine is used to lower the transport's cwnd when congestion is
 * detected.
 */
void sctp_transport_lower_cwnd(struct sctp_transport *transport,
			       enum sctp_lower_cwnd reason)
{
	struct sctp_association *asoc = transport->asoc;

	switch (reason) {
	case SCTP_LOWER_CWND_T3_RTX:
		/* RFC 2960 Section 7.2.3, sctpimpguide
		 * When the T3-rtx timer expires on an address, SCTP should
		 * perform slow start by:
		 *      ssthresh = max(cwnd/2, 4*MTU)
		 *      cwnd = 1*MTU
		 *      partial_bytes_acked = 0
		 */
		transport->ssthresh = max(transport->cwnd/2,
					  4*asoc->pathmtu);
		transport->cwnd = asoc->pathmtu;

		/* T3-rtx also clears fast recovery */
		asoc->fast_recovery = 0;
		break;

	case SCTP_LOWER_CWND_FAST_RTX:
		/* RFC 2960 7.2.4 Adjust the ssthresh and cwnd of the
		 * destination address(es) to which the missing DATA chunks
		 * were last sent, according to the formula described in
		 * Section 7.2.3.
		 *
		 * RFC 2960 7.2.3, sctpimpguide Upon detection of packet
		 * losses from SACK (see Section 7.2.4), An endpoint
		 * should do the following:
		 *      ssthresh = max(cwnd/2, 4*MTU)
		 *      cwnd = ssthresh
		 *      partial_bytes_acked = 0
		 */
		if (asoc->fast_recovery)
			return;

		/* Mark Fast recovery */
		asoc->fast_recovery = 1;
		asoc->fast_recovery_exit = asoc->next_tsn - 1;

		transport->ssthresh = max(transport->cwnd/2,
					  4*asoc->pathmtu);
		transport->cwnd = transport->ssthresh;
		break;

	case SCTP_LOWER_CWND_ECNE:
		/* RFC 2481 Section 6.1.2.
		 * If the sender receives an ECN-Echo ACK packet
		 * then the sender knows that congestion was encountered in the
		 * network on the path from the sender to the receiver. The
		 * indication of congestion should be treated just as a
		 * congestion loss in non-ECN Capable TCP. That is, the TCP
		 * source halves the congestion window "cwnd" and reduces the
		 * slow start threshold "ssthresh".
		 * A critical condition is that TCP does not react to
		 * congestion indications more than once every window of
		 * data (or more loosely more than once every round-trip time).
		 */
		if (time_after(jiffies, transport->last_time_ecne_reduced +
					transport->rtt)) {
			transport->ssthresh = max(transport->cwnd/2,
						  4*asoc->pathmtu);
			transport->cwnd = transport->ssthresh;
			transport->last_time_ecne_reduced = jiffies;
		}
		break;

	case SCTP_LOWER_CWND_INACTIVE:
		/* RFC 2960 Section 7.2.1, sctpimpguide
		 * When the endpoint does not transmit data on a given
		 * transport address, the cwnd of the transport address
		 * should be adjusted to max(cwnd/2, 4*MTU) per RTO.
		 * NOTE: Although the draft recommends that this check needs
		 * to be done every RTO interval, we do it every hearbeat
		 * interval.
		 */
		transport->cwnd = max(transport->cwnd/2,
					 4*asoc->pathmtu);
		/* RFC 4960 Errata 3.27.2: also adjust sshthresh */
		transport->ssthresh = transport->cwnd;
		break;
	}

	transport->partial_bytes_acked = 0;

	pr_debug("%s: transport:%p, reason:%d, cwnd:%d, ssthresh:%d\n",
		 __func__, transport, reason, transport->cwnd,
		 transport->ssthresh);
}

/* Apply Max.Burst limit to the congestion window:
 * sctpimpguide-05 2.14.2
 * D) When the time comes for the sender to
 * transmit new DATA chunks, the protocol parameter Max.Burst MUST
 * first be applied to limit how many new DATA chunks may be sent.
 * The limit is applied by adjusting cwnd as follows:
 * 	if ((flightsize+ Max.Burst * MTU) < cwnd)
 * 		cwnd = flightsize + Max.Burst * MTU
 */

void sctp_transport_burst_limited(struct sctp_transport *t)
{
	struct sctp_association *asoc = t->asoc;
	u32 old_cwnd = t->cwnd;
	u32 max_burst_bytes;

	if (t->burst_limited || asoc->max_burst == 0)
		return;

	max_burst_bytes = t->flight_size + (asoc->max_burst * asoc->pathmtu);
	if (max_burst_bytes < old_cwnd) {
		t->cwnd = max_burst_bytes;
		t->burst_limited = old_cwnd;
	}
}

/* Restore the old cwnd congestion window, after the burst had it's
 * desired effect.
 */
void sctp_transport_burst_reset(struct sctp_transport *t)
{
	if (t->burst_limited) {
		t->cwnd = t->burst_limited;
		t->burst_limited = 0;
	}
}

/* What is the next timeout value for this transport? */
unsigned long sctp_transport_timeout(struct sctp_transport *trans)
{
	/* RTO + timer slack +/- 50% of RTO */
	unsigned long timeout = trans->rto >> 1;

	if (trans->state != SCTP_UNCONFIRMED &&
	    trans->state != SCTP_PF)
		timeout += trans->hbinterval;

	return max_t(unsigned long, timeout, HZ / 5);
}

/* Reset transport variables to their initial values */
void sctp_transport_reset(struct sctp_transport *t)
{
	struct sctp_association *asoc = t->asoc;

	/* RFC 2960 (bis), Section 5.2.4
	 * All the congestion control parameters (e.g., cwnd, ssthresh)
	 * related to this peer MUST be reset to their initial values
	 * (see Section 6.2.1)
	 */
	t->cwnd = min(4*asoc->pathmtu, max_t(__u32, 2*asoc->pathmtu, 4380));
	t->burst_limited = 0;
	t->ssthresh = asoc->peer.i.a_rwnd;
	t->rto = asoc->rto_initial;
	sctp_max_rto(asoc, t);
	t->rtt = 0;
	t->srtt = 0;
	t->rttvar = 0;

	/* Reset these additional variables so that we have a clean slate. */
	t->partial_bytes_acked = 0;
	t->flight_size = 0;
	t->error_count = 0;
	t->rto_pending = 0;
	t->hb_sent = 0;

	/* Initialize the state information for SFR-CACC */
	t->cacc.changeover_active = 0;
	t->cacc.cycling_changeover = 0;
	t->cacc.next_tsn_at_change = 0;
	t->cacc.cacc_saw_newack = 0;
}

/* Schedule retransmission on the given transport */
void sctp_transport_immediate_rtx(struct sctp_transport *t)
{
	/* Stop pending T3_rtx_timer */
	if (del_timer(&t->T3_rtx_timer))
		sctp_transport_put(t);

	sctp_retransmit(&t->asoc->outqueue, t, SCTP_RTXR_T3_RTX);
	if (!timer_pending(&t->T3_rtx_timer)) {
		if (!mod_timer(&t->T3_rtx_timer, jiffies + t->rto))
			sctp_transport_hold(t);
	}
}

/* Drop dst */
void sctp_transport_dst_release(struct sctp_transport *t)
{
	dst_release(t->dst);
	t->dst = NULL;
	t->dst_pending_confirm = 0;
}

/* Schedule neighbour confirm */
void sctp_transport_dst_confirm(struct sctp_transport *t)
{
	t->dst_pending_confirm = 1;
}
