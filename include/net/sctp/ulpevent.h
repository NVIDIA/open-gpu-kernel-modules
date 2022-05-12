/* SPDX-License-Identifier: GPL-2.0-or-later */
/* SCTP kernel implementation
 * (C) Copyright IBM Corp. 2001, 2004
 * Copyright (c) 1999-2000 Cisco, Inc.
 * Copyright (c) 1999-2001 Motorola, Inc.
 * Copyright (c) 2001 Intel Corp.
 * Copyright (c) 2001 Nokia, Inc.
 * Copyright (c) 2001 La Monte H.P. Yarroll
 *
 * These are the definitions needed for the sctp_ulpevent type.  The
 * sctp_ulpevent type is used to carry information from the state machine
 * upwards to the ULP.
 *
 * This file is part of the SCTP kernel implementation
 *
 * Please send any bug reports or fixes you make to the
 * email address(es):
 *    lksctp developers <linux-sctp@vger.kernel.org>
 *
 * Written or modified by:
 *   Jon Grimm             <jgrimm@us.ibm.com>
 *   La Monte H.P. Yarroll <piggy@acm.org>
 *   Karl Knutson          <karl@athena.chicago.il.us>
 *   Sridhar Samudrala     <sri@us.ibm.com>
 */

#ifndef __sctp_ulpevent_h__
#define __sctp_ulpevent_h__

/* A structure to carry information to the ULP (e.g. Sockets API) */
/* Warning: This sits inside an skb.cb[] area.  Be very careful of
 * growing this structure as it is at the maximum limit now.
 *
 * sctp_ulpevent is saved in sk->cb(48 bytes), whose last 4 bytes
 * have been taken by sock_skb_cb, So here it has to use 'packed'
 * to make sctp_ulpevent fit into the rest 44 bytes.
 */
struct sctp_ulpevent {
	struct sctp_association *asoc;
	struct sctp_chunk *chunk;
	unsigned int rmem_len;
	union {
		__u32 mid;
		__u16 ssn;
	};
	union {
		__u32 ppid;
		__u32 fsn;
	};
	__u32 tsn;
	__u32 cumtsn;
	__u16 stream;
	__u16 flags;
	__u16 msg_flags;
} __packed;

/* Retrieve the skb this event sits inside of. */
static inline struct sk_buff *sctp_event2skb(const struct sctp_ulpevent *ev)
{
	return container_of((void *)ev, struct sk_buff, cb);
}

/* Retrieve & cast the event sitting inside the skb. */
static inline struct sctp_ulpevent *sctp_skb2event(struct sk_buff *skb)
{
	return (struct sctp_ulpevent *)skb->cb;
}

void sctp_ulpevent_free(struct sctp_ulpevent *);
int sctp_ulpevent_is_notification(const struct sctp_ulpevent *);
unsigned int sctp_queue_purge_ulpevents(struct sk_buff_head *list);

struct sctp_ulpevent *sctp_ulpevent_make_assoc_change(
	const struct sctp_association *asoc,
	__u16 flags,
	__u16 state,
	__u16 error,
	__u16 outbound,
	__u16 inbound,
	struct sctp_chunk *chunk,
	gfp_t gfp);

void sctp_ulpevent_notify_peer_addr_change(struct sctp_transport *transport,
					   int state, int error);

struct sctp_ulpevent *sctp_ulpevent_make_remote_error(
	const struct sctp_association *asoc,
	struct sctp_chunk *chunk,
	__u16 flags,
	gfp_t gfp);
struct sctp_ulpevent *sctp_ulpevent_make_send_failed(
	const struct sctp_association *asoc,
	struct sctp_chunk *chunk,
	__u16 flags,
	__u32 error,
	gfp_t gfp);

struct sctp_ulpevent *sctp_ulpevent_make_send_failed_event(
	const struct sctp_association *asoc,
	struct sctp_chunk *chunk,
	__u16 flags,
	__u32 error,
	gfp_t gfp);

struct sctp_ulpevent *sctp_ulpevent_make_shutdown_event(
	const struct sctp_association *asoc,
	__u16 flags,
	gfp_t gfp);

struct sctp_ulpevent *sctp_ulpevent_make_pdapi(
	const struct sctp_association *asoc,
	__u32 indication, __u32 sid, __u32 seq,
	__u32 flags, gfp_t gfp);

struct sctp_ulpevent *sctp_ulpevent_make_adaptation_indication(
	const struct sctp_association *asoc, gfp_t gfp);

struct sctp_ulpevent *sctp_ulpevent_make_rcvmsg(struct sctp_association *asoc,
	struct sctp_chunk *chunk,
	gfp_t gfp);

struct sctp_ulpevent *sctp_ulpevent_make_authkey(
	const struct sctp_association *asoc, __u16 key_id,
	__u32 indication, gfp_t gfp);

struct sctp_ulpevent *sctp_ulpevent_make_sender_dry_event(
	const struct sctp_association *asoc, gfp_t gfp);

struct sctp_ulpevent *sctp_ulpevent_make_stream_reset_event(
	const struct sctp_association *asoc, __u16 flags,
	__u16 stream_num, __be16 *stream_list, gfp_t gfp);

struct sctp_ulpevent *sctp_ulpevent_make_assoc_reset_event(
	const struct sctp_association *asoc, __u16 flags,
	 __u32 local_tsn, __u32 remote_tsn, gfp_t gfp);

struct sctp_ulpevent *sctp_ulpevent_make_stream_change_event(
	const struct sctp_association *asoc, __u16 flags,
	__u32 strchange_instrms, __u32 strchange_outstrms, gfp_t gfp);

struct sctp_ulpevent *sctp_make_reassembled_event(
	struct net *net, struct sk_buff_head *queue,
	struct sk_buff *f_frag, struct sk_buff *l_frag);

void sctp_ulpevent_read_sndrcvinfo(const struct sctp_ulpevent *event,
				   struct msghdr *);
void sctp_ulpevent_read_rcvinfo(const struct sctp_ulpevent *event,
				struct msghdr *);
void sctp_ulpevent_read_nxtinfo(const struct sctp_ulpevent *event,
				struct msghdr *, struct sock *sk);

__u16 sctp_ulpevent_get_notification_type(const struct sctp_ulpevent *event);

static inline void sctp_ulpevent_type_set(__u16 *subscribe,
					  __u16 sn_type, __u8 on)
{
	if (sn_type > SCTP_SN_TYPE_MAX)
		return;

	if (on)
		*subscribe |=  (1 << (sn_type - SCTP_SN_TYPE_BASE));
	else
		*subscribe &= ~(1 << (sn_type - SCTP_SN_TYPE_BASE));
}

/* Is this event type enabled? */
static inline bool sctp_ulpevent_type_enabled(__u16 subscribe, __u16 sn_type)
{
	if (sn_type > SCTP_SN_TYPE_MAX)
		return false;

	return subscribe & (1 << (sn_type - SCTP_SN_TYPE_BASE));
}

/* Given an event subscription, is this event enabled? */
static inline bool sctp_ulpevent_is_enabled(const struct sctp_ulpevent *event,
					    __u16 subscribe)
{
	__u16 sn_type;

	if (!sctp_ulpevent_is_notification(event))
		return true;

	sn_type = sctp_ulpevent_get_notification_type(event);

	return sctp_ulpevent_type_enabled(subscribe, sn_type);
}

#endif /* __sctp_ulpevent_h__ */
