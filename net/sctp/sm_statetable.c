// SPDX-License-Identifier: GPL-2.0-or-later
/* SCTP kernel implementation
 * (C) Copyright IBM Corp. 2001, 2004
 * Copyright (c) 1999-2000 Cisco, Inc.
 * Copyright (c) 1999-2001 Motorola, Inc.
 * Copyright (c) 2001 Intel Corp.
 * Copyright (c) 2001 Nokia, Inc.
 *
 * This file is part of the SCTP kernel implementation
 *
 * These are the state tables for the SCTP state machine.
 *
 * Please send any bug reports or fixes you make to the
 * email address(es):
 *    lksctp developers <linux-sctp@vger.kernel.org>
 *
 * Written or modified by:
 *    La Monte H.P. Yarroll <piggy@acm.org>
 *    Karl Knutson          <karl@athena.chicago.il.us>
 *    Jon Grimm             <jgrimm@us.ibm.com>
 *    Hui Huang		    <hui.huang@nokia.com>
 *    Daisy Chang	    <daisyc@us.ibm.com>
 *    Ardelle Fan	    <ardelle.fan@intel.com>
 *    Sridhar Samudrala	    <sri@us.ibm.com>
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/skbuff.h>
#include <net/sctp/sctp.h>
#include <net/sctp/sm.h>

static const struct sctp_sm_table_entry
primitive_event_table[SCTP_NUM_PRIMITIVE_TYPES][SCTP_STATE_NUM_STATES];
static const struct sctp_sm_table_entry
other_event_table[SCTP_NUM_OTHER_TYPES][SCTP_STATE_NUM_STATES];
static const struct sctp_sm_table_entry
timeout_event_table[SCTP_NUM_TIMEOUT_TYPES][SCTP_STATE_NUM_STATES];

static const struct sctp_sm_table_entry *sctp_chunk_event_lookup(
						struct net *net,
						enum sctp_cid cid,
						enum sctp_state state);


static const struct sctp_sm_table_entry bug = {
	.fn = sctp_sf_bug,
	.name = "sctp_sf_bug"
};

#define DO_LOOKUP(_max, _type, _table)					\
({									\
	const struct sctp_sm_table_entry *rtn;				\
									\
	if ((event_subtype._type > (_max))) {				\
		pr_warn("table %p possible attack: event %d exceeds max %d\n", \
			_table, event_subtype._type, _max);		\
		rtn = &bug;						\
	} else								\
		rtn = &_table[event_subtype._type][(int)state];		\
									\
	rtn;								\
})

const struct sctp_sm_table_entry *sctp_sm_lookup_event(
					struct net *net,
					enum sctp_event_type event_type,
					enum sctp_state state,
					union sctp_subtype event_subtype)
{
	switch (event_type) {
	case SCTP_EVENT_T_CHUNK:
		return sctp_chunk_event_lookup(net, event_subtype.chunk, state);
	case SCTP_EVENT_T_TIMEOUT:
		return DO_LOOKUP(SCTP_EVENT_TIMEOUT_MAX, timeout,
				 timeout_event_table);
	case SCTP_EVENT_T_OTHER:
		return DO_LOOKUP(SCTP_EVENT_OTHER_MAX, other,
				 other_event_table);
	case SCTP_EVENT_T_PRIMITIVE:
		return DO_LOOKUP(SCTP_EVENT_PRIMITIVE_MAX, primitive,
				 primitive_event_table);
	default:
		/* Yikes!  We got an illegal event type.  */
		return &bug;
	}
}

#define TYPE_SCTP_FUNC(func) {.fn = func, .name = #func}

#define TYPE_SCTP_DATA { \
	/* SCTP_STATE_CLOSED */ \
	TYPE_SCTP_FUNC(sctp_sf_ootb), \
	/* SCTP_STATE_COOKIE_WAIT */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
	/* SCTP_STATE_COOKIE_ECHOED */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
	/* SCTP_STATE_ESTABLISHED */ \
	TYPE_SCTP_FUNC(sctp_sf_eat_data_6_2), \
	/* SCTP_STATE_SHUTDOWN_PENDING */ \
	TYPE_SCTP_FUNC(sctp_sf_eat_data_6_2), \
	/* SCTP_STATE_SHUTDOWN_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_eat_data_fast_4_4), \
	/* SCTP_STATE_SHUTDOWN_RECEIVED */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
	/* SCTP_STATE_SHUTDOWN_ACK_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
} /* TYPE_SCTP_DATA */

#define TYPE_SCTP_INIT { \
	/* SCTP_STATE_CLOSED */ \
	TYPE_SCTP_FUNC(sctp_sf_do_5_1B_init), \
	/* SCTP_STATE_COOKIE_WAIT */ \
	TYPE_SCTP_FUNC(sctp_sf_do_5_2_1_siminit), \
	/* SCTP_STATE_COOKIE_ECHOED */ \
	TYPE_SCTP_FUNC(sctp_sf_do_5_2_1_siminit), \
	/* SCTP_STATE_ESTABLISHED */ \
	TYPE_SCTP_FUNC(sctp_sf_do_5_2_2_dupinit), \
	/* SCTP_STATE_SHUTDOWN_PENDING */ \
	TYPE_SCTP_FUNC(sctp_sf_do_5_2_2_dupinit), \
	/* SCTP_STATE_SHUTDOWN_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_do_5_2_2_dupinit), \
	/* SCTP_STATE_SHUTDOWN_RECEIVED */ \
	TYPE_SCTP_FUNC(sctp_sf_do_5_2_2_dupinit), \
	/* SCTP_STATE_SHUTDOWN_ACK_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_do_9_2_reshutack), \
} /* TYPE_SCTP_INIT */

#define TYPE_SCTP_INIT_ACK { \
	/* SCTP_STATE_CLOSED */ \
	TYPE_SCTP_FUNC(sctp_sf_do_5_2_3_initack), \
	/* SCTP_STATE_COOKIE_WAIT */ \
	TYPE_SCTP_FUNC(sctp_sf_do_5_1C_ack), \
	/* SCTP_STATE_COOKIE_ECHOED */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
	/* SCTP_STATE_ESTABLISHED */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
	/* SCTP_STATE_SHUTDOWN_PENDING */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
	/* SCTP_STATE_SHUTDOWN_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
	/* SCTP_STATE_SHUTDOWN_RECEIVED */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
	/* SCTP_STATE_SHUTDOWN_ACK_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
} /* TYPE_SCTP_INIT_ACK */

#define TYPE_SCTP_SACK { \
	/* SCTP_STATE_CLOSED */ \
	TYPE_SCTP_FUNC(sctp_sf_ootb), \
	/* SCTP_STATE_COOKIE_WAIT */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
	/* SCTP_STATE_COOKIE_ECHOED */ \
	TYPE_SCTP_FUNC(sctp_sf_eat_sack_6_2), \
	/* SCTP_STATE_ESTABLISHED */ \
	TYPE_SCTP_FUNC(sctp_sf_eat_sack_6_2), \
	/* SCTP_STATE_SHUTDOWN_PENDING */ \
	TYPE_SCTP_FUNC(sctp_sf_eat_sack_6_2), \
	/* SCTP_STATE_SHUTDOWN_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
	/* SCTP_STATE_SHUTDOWN_RECEIVED */ \
	TYPE_SCTP_FUNC(sctp_sf_eat_sack_6_2), \
	/* SCTP_STATE_SHUTDOWN_ACK_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
} /* TYPE_SCTP_SACK */

#define TYPE_SCTP_HEARTBEAT { \
	/* SCTP_STATE_CLOSED */ \
	TYPE_SCTP_FUNC(sctp_sf_ootb), \
	/* SCTP_STATE_COOKIE_WAIT */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
	/* SCTP_STATE_COOKIE_ECHOED */ \
	TYPE_SCTP_FUNC(sctp_sf_beat_8_3), \
	/* SCTP_STATE_ESTABLISHED */ \
	TYPE_SCTP_FUNC(sctp_sf_beat_8_3), \
	/* SCTP_STATE_SHUTDOWN_PENDING */ \
	TYPE_SCTP_FUNC(sctp_sf_beat_8_3), \
	/* SCTP_STATE_SHUTDOWN_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_beat_8_3), \
	/* SCTP_STATE_SHUTDOWN_RECEIVED */ \
	TYPE_SCTP_FUNC(sctp_sf_beat_8_3), \
	/* SCTP_STATE_SHUTDOWN_ACK_SENT */ \
	/* This should not happen, but we are nice.  */ \
	TYPE_SCTP_FUNC(sctp_sf_beat_8_3), \
} /* TYPE_SCTP_HEARTBEAT */

#define TYPE_SCTP_HEARTBEAT_ACK { \
	/* SCTP_STATE_CLOSED */ \
	TYPE_SCTP_FUNC(sctp_sf_ootb), \
	/* SCTP_STATE_COOKIE_WAIT */ \
	TYPE_SCTP_FUNC(sctp_sf_violation), \
	/* SCTP_STATE_COOKIE_ECHOED */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
	/* SCTP_STATE_ESTABLISHED */ \
	TYPE_SCTP_FUNC(sctp_sf_backbeat_8_3), \
	/* SCTP_STATE_SHUTDOWN_PENDING */ \
	TYPE_SCTP_FUNC(sctp_sf_backbeat_8_3), \
	/* SCTP_STATE_SHUTDOWN_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_backbeat_8_3), \
	/* SCTP_STATE_SHUTDOWN_RECEIVED */ \
	TYPE_SCTP_FUNC(sctp_sf_backbeat_8_3), \
	/* SCTP_STATE_SHUTDOWN_ACK_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
} /* TYPE_SCTP_HEARTBEAT_ACK */

#define TYPE_SCTP_ABORT { \
	/* SCTP_STATE_CLOSED */ \
	TYPE_SCTP_FUNC(sctp_sf_pdiscard), \
	/* SCTP_STATE_COOKIE_WAIT */ \
	TYPE_SCTP_FUNC(sctp_sf_cookie_wait_abort), \
	/* SCTP_STATE_COOKIE_ECHOED */ \
	TYPE_SCTP_FUNC(sctp_sf_cookie_echoed_abort), \
	/* SCTP_STATE_ESTABLISHED */ \
	TYPE_SCTP_FUNC(sctp_sf_do_9_1_abort), \
	/* SCTP_STATE_SHUTDOWN_PENDING */ \
	TYPE_SCTP_FUNC(sctp_sf_shutdown_pending_abort), \
	/* SCTP_STATE_SHUTDOWN_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_shutdown_sent_abort), \
	/* SCTP_STATE_SHUTDOWN_RECEIVED */ \
	TYPE_SCTP_FUNC(sctp_sf_do_9_1_abort), \
	/* SCTP_STATE_SHUTDOWN_ACK_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_shutdown_ack_sent_abort), \
} /* TYPE_SCTP_ABORT */

#define TYPE_SCTP_SHUTDOWN { \
	/* SCTP_STATE_CLOSED */ \
	TYPE_SCTP_FUNC(sctp_sf_ootb), \
	/* SCTP_STATE_COOKIE_WAIT */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
	/* SCTP_STATE_COOKIE_ECHOED */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
	/* SCTP_STATE_ESTABLISHED */ \
	TYPE_SCTP_FUNC(sctp_sf_do_9_2_shutdown), \
	/* SCTP_STATE_SHUTDOWN_PENDING */ \
	TYPE_SCTP_FUNC(sctp_sf_do_9_2_shutdown), \
	/* SCTP_STATE_SHUTDOWN_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_do_9_2_shutdown_ack), \
	/* SCTP_STATE_SHUTDOWN_RECEIVED */ \
	TYPE_SCTP_FUNC(sctp_sf_do_9_2_shut_ctsn), \
	/* SCTP_STATE_SHUTDOWN_ACK_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
} /* TYPE_SCTP_SHUTDOWN */

#define TYPE_SCTP_SHUTDOWN_ACK { \
	/* SCTP_STATE_CLOSED */ \
	TYPE_SCTP_FUNC(sctp_sf_ootb), \
	/* SCTP_STATE_COOKIE_WAIT */ \
	TYPE_SCTP_FUNC(sctp_sf_do_8_5_1_E_sa), \
	/* SCTP_STATE_COOKIE_ECHOED */ \
	TYPE_SCTP_FUNC(sctp_sf_do_8_5_1_E_sa), \
	/* SCTP_STATE_ESTABLISHED */ \
	TYPE_SCTP_FUNC(sctp_sf_violation), \
	/* SCTP_STATE_SHUTDOWN_PENDING */ \
	TYPE_SCTP_FUNC(sctp_sf_violation), \
	/* SCTP_STATE_SHUTDOWN_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_do_9_2_final), \
	/* SCTP_STATE_SHUTDOWN_RECEIVED */ \
	TYPE_SCTP_FUNC(sctp_sf_violation), \
	/* SCTP_STATE_SHUTDOWN_ACK_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_do_9_2_final), \
} /* TYPE_SCTP_SHUTDOWN_ACK */

#define TYPE_SCTP_ERROR { \
	/* SCTP_STATE_CLOSED */ \
	TYPE_SCTP_FUNC(sctp_sf_ootb), \
	/* SCTP_STATE_COOKIE_WAIT */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
	/* SCTP_STATE_COOKIE_ECHOED */ \
	TYPE_SCTP_FUNC(sctp_sf_cookie_echoed_err), \
	/* SCTP_STATE_ESTABLISHED */ \
	TYPE_SCTP_FUNC(sctp_sf_operr_notify), \
	/* SCTP_STATE_SHUTDOWN_PENDING */ \
	TYPE_SCTP_FUNC(sctp_sf_operr_notify), \
	/* SCTP_STATE_SHUTDOWN_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
	/* SCTP_STATE_SHUTDOWN_RECEIVED */ \
	TYPE_SCTP_FUNC(sctp_sf_operr_notify), \
	/* SCTP_STATE_SHUTDOWN_ACK_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
} /* TYPE_SCTP_ERROR */

#define TYPE_SCTP_COOKIE_ECHO { \
	/* SCTP_STATE_CLOSED */ \
	TYPE_SCTP_FUNC(sctp_sf_do_5_1D_ce), \
	/* SCTP_STATE_COOKIE_WAIT */ \
	TYPE_SCTP_FUNC(sctp_sf_do_5_2_4_dupcook), \
	/* SCTP_STATE_COOKIE_ECHOED */ \
	TYPE_SCTP_FUNC(sctp_sf_do_5_2_4_dupcook), \
	/* SCTP_STATE_ESTABLISHED */ \
	TYPE_SCTP_FUNC(sctp_sf_do_5_2_4_dupcook), \
	/* SCTP_STATE_SHUTDOWN_PENDING */ \
	TYPE_SCTP_FUNC(sctp_sf_do_5_2_4_dupcook), \
	/* SCTP_STATE_SHUTDOWN_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_do_5_2_4_dupcook), \
	/* SCTP_STATE_SHUTDOWN_RECEIVED */ \
	TYPE_SCTP_FUNC(sctp_sf_do_5_2_4_dupcook), \
	/* SCTP_STATE_SHUTDOWN_ACK_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_do_5_2_4_dupcook), \
} /* TYPE_SCTP_COOKIE_ECHO */

#define TYPE_SCTP_COOKIE_ACK { \
	/* SCTP_STATE_CLOSED */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
	/* SCTP_STATE_COOKIE_WAIT */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
	/* SCTP_STATE_COOKIE_ECHOED */ \
	TYPE_SCTP_FUNC(sctp_sf_do_5_1E_ca), \
	/* SCTP_STATE_ESTABLISHED */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
	/* SCTP_STATE_SHUTDOWN_PENDING */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
	/* SCTP_STATE_SHUTDOWN_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
	/* SCTP_STATE_SHUTDOWN_RECEIVED */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
	/* SCTP_STATE_SHUTDOWN_ACK_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
} /* TYPE_SCTP_COOKIE_ACK */

#define TYPE_SCTP_ECN_ECNE { \
	/* SCTP_STATE_CLOSED */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
	/* SCTP_STATE_COOKIE_WAIT */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
	/* SCTP_STATE_COOKIE_ECHOED */ \
	TYPE_SCTP_FUNC(sctp_sf_do_ecne), \
	/* SCTP_STATE_ESTABLISHED */ \
	TYPE_SCTP_FUNC(sctp_sf_do_ecne), \
	/* SCTP_STATE_SHUTDOWN_PENDING */ \
	TYPE_SCTP_FUNC(sctp_sf_do_ecne), \
	/* SCTP_STATE_SHUTDOWN_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_do_ecne), \
	/* SCTP_STATE_SHUTDOWN_RECEIVED */ \
	TYPE_SCTP_FUNC(sctp_sf_do_ecne), \
	/* SCTP_STATE_SHUTDOWN_ACK_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
} /* TYPE_SCTP_ECN_ECNE */

#define TYPE_SCTP_ECN_CWR { \
	/* SCTP_STATE_CLOSED */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
	/* SCTP_STATE_COOKIE_WAIT */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
	/* SCTP_STATE_COOKIE_ECHOED */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
	/* SCTP_STATE_ESTABLISHED */ \
	TYPE_SCTP_FUNC(sctp_sf_do_ecn_cwr), \
	/* SCTP_STATE_SHUTDOWN_PENDING */ \
	TYPE_SCTP_FUNC(sctp_sf_do_ecn_cwr), \
	/* SCTP_STATE_SHUTDOWN_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_do_ecn_cwr), \
	/* SCTP_STATE_SHUTDOWN_RECEIVED */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
	/* SCTP_STATE_SHUTDOWN_ACK_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
} /* TYPE_SCTP_ECN_CWR */

#define TYPE_SCTP_SHUTDOWN_COMPLETE { \
	/* SCTP_STATE_CLOSED */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
	/* SCTP_STATE_COOKIE_WAIT */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
	/* SCTP_STATE_COOKIE_ECHOED */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
	/* SCTP_STATE_ESTABLISHED */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
	/* SCTP_STATE_SHUTDOWN_PENDING */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
	/* SCTP_STATE_SHUTDOWN_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
	/* SCTP_STATE_SHUTDOWN_RECEIVED */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
	/* SCTP_STATE_SHUTDOWN_ACK_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_do_4_C), \
} /* TYPE_SCTP_SHUTDOWN_COMPLETE */

/* The primary index for this table is the chunk type.
 * The secondary index for this table is the state.
 *
 * For base protocol (RFC 2960).
 */
static const struct sctp_sm_table_entry
chunk_event_table[SCTP_NUM_BASE_CHUNK_TYPES][SCTP_STATE_NUM_STATES] = {
	TYPE_SCTP_DATA,
	TYPE_SCTP_INIT,
	TYPE_SCTP_INIT_ACK,
	TYPE_SCTP_SACK,
	TYPE_SCTP_HEARTBEAT,
	TYPE_SCTP_HEARTBEAT_ACK,
	TYPE_SCTP_ABORT,
	TYPE_SCTP_SHUTDOWN,
	TYPE_SCTP_SHUTDOWN_ACK,
	TYPE_SCTP_ERROR,
	TYPE_SCTP_COOKIE_ECHO,
	TYPE_SCTP_COOKIE_ACK,
	TYPE_SCTP_ECN_ECNE,
	TYPE_SCTP_ECN_CWR,
	TYPE_SCTP_SHUTDOWN_COMPLETE,
}; /* state_fn_t chunk_event_table[][] */

#define TYPE_SCTP_ASCONF { \
	/* SCTP_STATE_CLOSED */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
	/* SCTP_STATE_COOKIE_WAIT */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
	/* SCTP_STATE_COOKIE_ECHOED */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
	/* SCTP_STATE_ESTABLISHED */ \
	TYPE_SCTP_FUNC(sctp_sf_do_asconf), \
	/* SCTP_STATE_SHUTDOWN_PENDING */ \
	TYPE_SCTP_FUNC(sctp_sf_do_asconf), \
	/* SCTP_STATE_SHUTDOWN_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_do_asconf), \
	/* SCTP_STATE_SHUTDOWN_RECEIVED */ \
	TYPE_SCTP_FUNC(sctp_sf_do_asconf), \
	/* SCTP_STATE_SHUTDOWN_ACK_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
} /* TYPE_SCTP_ASCONF */

#define TYPE_SCTP_ASCONF_ACK { \
	/* SCTP_STATE_CLOSED */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
	/* SCTP_STATE_COOKIE_WAIT */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
	/* SCTP_STATE_COOKIE_ECHOED */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
	/* SCTP_STATE_ESTABLISHED */ \
	TYPE_SCTP_FUNC(sctp_sf_do_asconf_ack), \
	/* SCTP_STATE_SHUTDOWN_PENDING */ \
	TYPE_SCTP_FUNC(sctp_sf_do_asconf_ack), \
	/* SCTP_STATE_SHUTDOWN_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_do_asconf_ack), \
	/* SCTP_STATE_SHUTDOWN_RECEIVED */ \
	TYPE_SCTP_FUNC(sctp_sf_do_asconf_ack), \
	/* SCTP_STATE_SHUTDOWN_ACK_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
} /* TYPE_SCTP_ASCONF_ACK */

/* The primary index for this table is the chunk type.
 * The secondary index for this table is the state.
 */
static const struct sctp_sm_table_entry
addip_chunk_event_table[SCTP_NUM_ADDIP_CHUNK_TYPES][SCTP_STATE_NUM_STATES] = {
	TYPE_SCTP_ASCONF,
	TYPE_SCTP_ASCONF_ACK,
}; /*state_fn_t addip_chunk_event_table[][] */

#define TYPE_SCTP_FWD_TSN { \
	/* SCTP_STATE_CLOSED */ \
	TYPE_SCTP_FUNC(sctp_sf_ootb), \
	/* SCTP_STATE_COOKIE_WAIT */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
	/* SCTP_STATE_COOKIE_ECHOED */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
	/* SCTP_STATE_ESTABLISHED */ \
	TYPE_SCTP_FUNC(sctp_sf_eat_fwd_tsn), \
	/* SCTP_STATE_SHUTDOWN_PENDING */ \
	TYPE_SCTP_FUNC(sctp_sf_eat_fwd_tsn), \
	/* SCTP_STATE_SHUTDOWN_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_eat_fwd_tsn_fast), \
	/* SCTP_STATE_SHUTDOWN_RECEIVED */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
	/* SCTP_STATE_SHUTDOWN_ACK_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
} /* TYPE_SCTP_FWD_TSN */

/* The primary index for this table is the chunk type.
 * The secondary index for this table is the state.
 */
static const struct sctp_sm_table_entry
prsctp_chunk_event_table[SCTP_NUM_PRSCTP_CHUNK_TYPES][SCTP_STATE_NUM_STATES] = {
	TYPE_SCTP_FWD_TSN,
}; /*state_fn_t prsctp_chunk_event_table[][] */

#define TYPE_SCTP_RECONF { \
	/* SCTP_STATE_CLOSED */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
	/* SCTP_STATE_COOKIE_WAIT */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
	/* SCTP_STATE_COOKIE_ECHOED */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
	/* SCTP_STATE_ESTABLISHED */ \
	TYPE_SCTP_FUNC(sctp_sf_do_reconf), \
	/* SCTP_STATE_SHUTDOWN_PENDING */ \
	TYPE_SCTP_FUNC(sctp_sf_do_reconf), \
	/* SCTP_STATE_SHUTDOWN_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
	/* SCTP_STATE_SHUTDOWN_RECEIVED */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
	/* SCTP_STATE_SHUTDOWN_ACK_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
} /* TYPE_SCTP_RECONF */

/* The primary index for this table is the chunk type.
 * The secondary index for this table is the state.
 */
static const struct sctp_sm_table_entry
reconf_chunk_event_table[SCTP_NUM_RECONF_CHUNK_TYPES][SCTP_STATE_NUM_STATES] = {
	TYPE_SCTP_RECONF,
}; /*state_fn_t reconf_chunk_event_table[][] */

#define TYPE_SCTP_AUTH { \
	/* SCTP_STATE_CLOSED */ \
	TYPE_SCTP_FUNC(sctp_sf_ootb), \
	/* SCTP_STATE_COOKIE_WAIT */ \
	TYPE_SCTP_FUNC(sctp_sf_discard_chunk), \
	/* SCTP_STATE_COOKIE_ECHOED */ \
	TYPE_SCTP_FUNC(sctp_sf_eat_auth), \
	/* SCTP_STATE_ESTABLISHED */ \
	TYPE_SCTP_FUNC(sctp_sf_eat_auth), \
	/* SCTP_STATE_SHUTDOWN_PENDING */ \
	TYPE_SCTP_FUNC(sctp_sf_eat_auth), \
	/* SCTP_STATE_SHUTDOWN_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_eat_auth), \
	/* SCTP_STATE_SHUTDOWN_RECEIVED */ \
	TYPE_SCTP_FUNC(sctp_sf_eat_auth), \
	/* SCTP_STATE_SHUTDOWN_ACK_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_eat_auth), \
} /* TYPE_SCTP_AUTH */

/* The primary index for this table is the chunk type.
 * The secondary index for this table is the state.
 */
static const struct sctp_sm_table_entry
auth_chunk_event_table[SCTP_NUM_AUTH_CHUNK_TYPES][SCTP_STATE_NUM_STATES] = {
	TYPE_SCTP_AUTH,
}; /*state_fn_t auth_chunk_event_table[][] */

static const struct sctp_sm_table_entry
chunk_event_table_unknown[SCTP_STATE_NUM_STATES] = {
	/* SCTP_STATE_CLOSED */
	TYPE_SCTP_FUNC(sctp_sf_ootb),
	/* SCTP_STATE_COOKIE_WAIT */
	TYPE_SCTP_FUNC(sctp_sf_unk_chunk),
	/* SCTP_STATE_COOKIE_ECHOED */
	TYPE_SCTP_FUNC(sctp_sf_unk_chunk),
	/* SCTP_STATE_ESTABLISHED */
	TYPE_SCTP_FUNC(sctp_sf_unk_chunk),
	/* SCTP_STATE_SHUTDOWN_PENDING */
	TYPE_SCTP_FUNC(sctp_sf_unk_chunk),
	/* SCTP_STATE_SHUTDOWN_SENT */
	TYPE_SCTP_FUNC(sctp_sf_unk_chunk),
	/* SCTP_STATE_SHUTDOWN_RECEIVED */
	TYPE_SCTP_FUNC(sctp_sf_unk_chunk),
	/* SCTP_STATE_SHUTDOWN_ACK_SENT */
	TYPE_SCTP_FUNC(sctp_sf_unk_chunk),
};	/* chunk unknown */


#define TYPE_SCTP_PRIMITIVE_ASSOCIATE  { \
	/* SCTP_STATE_CLOSED */ \
	TYPE_SCTP_FUNC(sctp_sf_do_prm_asoc), \
	/* SCTP_STATE_COOKIE_WAIT */ \
	TYPE_SCTP_FUNC(sctp_sf_not_impl), \
	/* SCTP_STATE_COOKIE_ECHOED */ \
	TYPE_SCTP_FUNC(sctp_sf_not_impl), \
	/* SCTP_STATE_ESTABLISHED */ \
	TYPE_SCTP_FUNC(sctp_sf_not_impl), \
	/* SCTP_STATE_SHUTDOWN_PENDING */ \
	TYPE_SCTP_FUNC(sctp_sf_not_impl), \
	/* SCTP_STATE_SHUTDOWN_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_not_impl), \
	/* SCTP_STATE_SHUTDOWN_RECEIVED */ \
	TYPE_SCTP_FUNC(sctp_sf_not_impl), \
	/* SCTP_STATE_SHUTDOWN_ACK_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_not_impl), \
} /* TYPE_SCTP_PRIMITIVE_ASSOCIATE */

#define TYPE_SCTP_PRIMITIVE_SHUTDOWN  { \
	/* SCTP_STATE_CLOSED */ \
	TYPE_SCTP_FUNC(sctp_sf_error_closed), \
	/* SCTP_STATE_COOKIE_WAIT */ \
	TYPE_SCTP_FUNC(sctp_sf_cookie_wait_prm_shutdown), \
	/* SCTP_STATE_COOKIE_ECHOED */ \
	TYPE_SCTP_FUNC(sctp_sf_cookie_echoed_prm_shutdown),\
	/* SCTP_STATE_ESTABLISHED */ \
	TYPE_SCTP_FUNC(sctp_sf_do_9_2_prm_shutdown), \
	/* SCTP_STATE_SHUTDOWN_PENDING */ \
	TYPE_SCTP_FUNC(sctp_sf_ignore_primitive), \
	/* SCTP_STATE_SHUTDOWN_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_ignore_primitive), \
	/* SCTP_STATE_SHUTDOWN_RECEIVED */ \
	TYPE_SCTP_FUNC(sctp_sf_ignore_primitive), \
	/* SCTP_STATE_SHUTDOWN_ACK_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_ignore_primitive), \
} /* TYPE_SCTP_PRIMITIVE_SHUTDOWN */

#define TYPE_SCTP_PRIMITIVE_ABORT  { \
	/* SCTP_STATE_CLOSED */ \
	TYPE_SCTP_FUNC(sctp_sf_error_closed), \
	/* SCTP_STATE_COOKIE_WAIT */ \
	TYPE_SCTP_FUNC(sctp_sf_cookie_wait_prm_abort), \
	/* SCTP_STATE_COOKIE_ECHOED */ \
	TYPE_SCTP_FUNC(sctp_sf_cookie_echoed_prm_abort), \
	/* SCTP_STATE_ESTABLISHED */ \
	TYPE_SCTP_FUNC(sctp_sf_do_9_1_prm_abort), \
	/* SCTP_STATE_SHUTDOWN_PENDING */ \
	TYPE_SCTP_FUNC(sctp_sf_shutdown_pending_prm_abort), \
	/* SCTP_STATE_SHUTDOWN_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_shutdown_sent_prm_abort), \
	/* SCTP_STATE_SHUTDOWN_RECEIVED */ \
	TYPE_SCTP_FUNC(sctp_sf_do_9_1_prm_abort), \
	/* SCTP_STATE_SHUTDOWN_ACK_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_shutdown_ack_sent_prm_abort), \
} /* TYPE_SCTP_PRIMITIVE_ABORT */

#define TYPE_SCTP_PRIMITIVE_SEND  { \
	/* SCTP_STATE_CLOSED */ \
	TYPE_SCTP_FUNC(sctp_sf_error_closed), \
	/* SCTP_STATE_COOKIE_WAIT */ \
	TYPE_SCTP_FUNC(sctp_sf_do_prm_send), \
	/* SCTP_STATE_COOKIE_ECHOED */ \
	TYPE_SCTP_FUNC(sctp_sf_do_prm_send), \
	/* SCTP_STATE_ESTABLISHED */ \
	TYPE_SCTP_FUNC(sctp_sf_do_prm_send), \
	/* SCTP_STATE_SHUTDOWN_PENDING */ \
	TYPE_SCTP_FUNC(sctp_sf_error_shutdown), \
	/* SCTP_STATE_SHUTDOWN_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_error_shutdown), \
	/* SCTP_STATE_SHUTDOWN_RECEIVED */ \
	TYPE_SCTP_FUNC(sctp_sf_error_shutdown), \
	/* SCTP_STATE_SHUTDOWN_ACK_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_error_shutdown), \
} /* TYPE_SCTP_PRIMITIVE_SEND */

#define TYPE_SCTP_PRIMITIVE_REQUESTHEARTBEAT  { \
	/* SCTP_STATE_CLOSED */ \
	TYPE_SCTP_FUNC(sctp_sf_error_closed), \
	/* SCTP_STATE_COOKIE_WAIT */ \
	TYPE_SCTP_FUNC(sctp_sf_do_prm_requestheartbeat),          \
	/* SCTP_STATE_COOKIE_ECHOED */ \
	TYPE_SCTP_FUNC(sctp_sf_do_prm_requestheartbeat),          \
	/* SCTP_STATE_ESTABLISHED */ \
	TYPE_SCTP_FUNC(sctp_sf_do_prm_requestheartbeat),          \
	/* SCTP_STATE_SHUTDOWN_PENDING */ \
	TYPE_SCTP_FUNC(sctp_sf_do_prm_requestheartbeat),          \
	/* SCTP_STATE_SHUTDOWN_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_do_prm_requestheartbeat),          \
	/* SCTP_STATE_SHUTDOWN_RECEIVED */ \
	TYPE_SCTP_FUNC(sctp_sf_do_prm_requestheartbeat),          \
	/* SCTP_STATE_SHUTDOWN_ACK_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_do_prm_requestheartbeat),          \
} /* TYPE_SCTP_PRIMITIVE_REQUESTHEARTBEAT */

#define TYPE_SCTP_PRIMITIVE_ASCONF { \
	/* SCTP_STATE_CLOSED */ \
	TYPE_SCTP_FUNC(sctp_sf_error_closed), \
	/* SCTP_STATE_COOKIE_WAIT */ \
	TYPE_SCTP_FUNC(sctp_sf_error_closed), \
	/* SCTP_STATE_COOKIE_ECHOED */ \
	TYPE_SCTP_FUNC(sctp_sf_error_closed), \
	/* SCTP_STATE_ESTABLISHED */ \
	TYPE_SCTP_FUNC(sctp_sf_do_prm_asconf), \
	/* SCTP_STATE_SHUTDOWN_PENDING */ \
	TYPE_SCTP_FUNC(sctp_sf_do_prm_asconf), \
	/* SCTP_STATE_SHUTDOWN_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_do_prm_asconf), \
	/* SCTP_STATE_SHUTDOWN_RECEIVED */ \
	TYPE_SCTP_FUNC(sctp_sf_do_prm_asconf), \
	/* SCTP_STATE_SHUTDOWN_ACK_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_error_shutdown), \
} /* TYPE_SCTP_PRIMITIVE_ASCONF */

#define TYPE_SCTP_PRIMITIVE_RECONF { \
	/* SCTP_STATE_CLOSED */ \
	TYPE_SCTP_FUNC(sctp_sf_error_closed), \
	/* SCTP_STATE_COOKIE_WAIT */ \
	TYPE_SCTP_FUNC(sctp_sf_error_closed), \
	/* SCTP_STATE_COOKIE_ECHOED */ \
	TYPE_SCTP_FUNC(sctp_sf_error_closed), \
	/* SCTP_STATE_ESTABLISHED */ \
	TYPE_SCTP_FUNC(sctp_sf_do_prm_reconf), \
	/* SCTP_STATE_SHUTDOWN_PENDING */ \
	TYPE_SCTP_FUNC(sctp_sf_do_prm_reconf), \
	/* SCTP_STATE_SHUTDOWN_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_do_prm_reconf), \
	/* SCTP_STATE_SHUTDOWN_RECEIVED */ \
	TYPE_SCTP_FUNC(sctp_sf_do_prm_reconf), \
	/* SCTP_STATE_SHUTDOWN_ACK_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_error_shutdown), \
} /* TYPE_SCTP_PRIMITIVE_RECONF */

/* The primary index for this table is the primitive type.
 * The secondary index for this table is the state.
 */
static const struct sctp_sm_table_entry
primitive_event_table[SCTP_NUM_PRIMITIVE_TYPES][SCTP_STATE_NUM_STATES] = {
	TYPE_SCTP_PRIMITIVE_ASSOCIATE,
	TYPE_SCTP_PRIMITIVE_SHUTDOWN,
	TYPE_SCTP_PRIMITIVE_ABORT,
	TYPE_SCTP_PRIMITIVE_SEND,
	TYPE_SCTP_PRIMITIVE_REQUESTHEARTBEAT,
	TYPE_SCTP_PRIMITIVE_ASCONF,
	TYPE_SCTP_PRIMITIVE_RECONF,
};

#define TYPE_SCTP_OTHER_NO_PENDING_TSN  { \
	/* SCTP_STATE_CLOSED */ \
	TYPE_SCTP_FUNC(sctp_sf_ignore_other), \
	/* SCTP_STATE_COOKIE_WAIT */ \
	TYPE_SCTP_FUNC(sctp_sf_ignore_other), \
	/* SCTP_STATE_COOKIE_ECHOED */ \
	TYPE_SCTP_FUNC(sctp_sf_ignore_other), \
	/* SCTP_STATE_ESTABLISHED */ \
	TYPE_SCTP_FUNC(sctp_sf_do_no_pending_tsn), \
	/* SCTP_STATE_SHUTDOWN_PENDING */ \
	TYPE_SCTP_FUNC(sctp_sf_do_9_2_start_shutdown), \
	/* SCTP_STATE_SHUTDOWN_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_ignore_other), \
	/* SCTP_STATE_SHUTDOWN_RECEIVED */ \
	TYPE_SCTP_FUNC(sctp_sf_do_9_2_shutdown_ack), \
	/* SCTP_STATE_SHUTDOWN_ACK_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_ignore_other), \
}

#define TYPE_SCTP_OTHER_ICMP_PROTO_UNREACH  { \
	/* SCTP_STATE_CLOSED */ \
	TYPE_SCTP_FUNC(sctp_sf_ignore_other), \
	/* SCTP_STATE_COOKIE_WAIT */ \
	TYPE_SCTP_FUNC(sctp_sf_cookie_wait_icmp_abort), \
	/* SCTP_STATE_COOKIE_ECHOED */ \
	TYPE_SCTP_FUNC(sctp_sf_ignore_other), \
	/* SCTP_STATE_ESTABLISHED */ \
	TYPE_SCTP_FUNC(sctp_sf_ignore_other), \
	/* SCTP_STATE_SHUTDOWN_PENDING */ \
	TYPE_SCTP_FUNC(sctp_sf_ignore_other), \
	/* SCTP_STATE_SHUTDOWN_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_ignore_other), \
	/* SCTP_STATE_SHUTDOWN_RECEIVED */ \
	TYPE_SCTP_FUNC(sctp_sf_ignore_other), \
	/* SCTP_STATE_SHUTDOWN_ACK_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_ignore_other), \
}

static const struct sctp_sm_table_entry
other_event_table[SCTP_NUM_OTHER_TYPES][SCTP_STATE_NUM_STATES] = {
	TYPE_SCTP_OTHER_NO_PENDING_TSN,
	TYPE_SCTP_OTHER_ICMP_PROTO_UNREACH,
};

#define TYPE_SCTP_EVENT_TIMEOUT_NONE { \
	/* SCTP_STATE_CLOSED */ \
	TYPE_SCTP_FUNC(sctp_sf_bug), \
	/* SCTP_STATE_COOKIE_WAIT */ \
	TYPE_SCTP_FUNC(sctp_sf_bug), \
	/* SCTP_STATE_COOKIE_ECHOED */ \
	TYPE_SCTP_FUNC(sctp_sf_bug), \
	/* SCTP_STATE_ESTABLISHED */ \
	TYPE_SCTP_FUNC(sctp_sf_bug), \
	/* SCTP_STATE_SHUTDOWN_PENDING */ \
	TYPE_SCTP_FUNC(sctp_sf_bug), \
	/* SCTP_STATE_SHUTDOWN_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_bug), \
	/* SCTP_STATE_SHUTDOWN_RECEIVED */ \
	TYPE_SCTP_FUNC(sctp_sf_bug), \
	/* SCTP_STATE_SHUTDOWN_ACK_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_bug), \
}

#define TYPE_SCTP_EVENT_TIMEOUT_T1_COOKIE { \
	/* SCTP_STATE_CLOSED */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
	/* SCTP_STATE_COOKIE_WAIT */ \
	TYPE_SCTP_FUNC(sctp_sf_bug), \
	/* SCTP_STATE_COOKIE_ECHOED */ \
	TYPE_SCTP_FUNC(sctp_sf_t1_cookie_timer_expire), \
	/* SCTP_STATE_ESTABLISHED */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
	/* SCTP_STATE_SHUTDOWN_PENDING */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
	/* SCTP_STATE_SHUTDOWN_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
	/* SCTP_STATE_SHUTDOWN_RECEIVED */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
	/* SCTP_STATE_SHUTDOWN_ACK_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
}

#define TYPE_SCTP_EVENT_TIMEOUT_T1_INIT { \
	/* SCTP_STATE_CLOSED */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
	/* SCTP_STATE_COOKIE_WAIT */ \
	TYPE_SCTP_FUNC(sctp_sf_t1_init_timer_expire), \
	/* SCTP_STATE_COOKIE_ECHOED */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
	/* SCTP_STATE_ESTABLISHED */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
	/* SCTP_STATE_SHUTDOWN_PENDING */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
	/* SCTP_STATE_SHUTDOWN_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
	/* SCTP_STATE_SHUTDOWN_RECEIVED */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
	/* SCTP_STATE_SHUTDOWN_ACK_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
}

#define TYPE_SCTP_EVENT_TIMEOUT_T2_SHUTDOWN { \
	/* SCTP_STATE_CLOSED */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
	/* SCTP_STATE_COOKIE_WAIT */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
	/* SCTP_STATE_COOKIE_ECHOED */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
	/* SCTP_STATE_ESTABLISHED */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
	/* SCTP_STATE_SHUTDOWN_PENDING */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
	/* SCTP_STATE_SHUTDOWN_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_t2_timer_expire), \
	/* SCTP_STATE_SHUTDOWN_RECEIVED */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
	/* SCTP_STATE_SHUTDOWN_ACK_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_t2_timer_expire), \
}

#define TYPE_SCTP_EVENT_TIMEOUT_T3_RTX { \
	/* SCTP_STATE_CLOSED */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
	/* SCTP_STATE_COOKIE_WAIT */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
	/* SCTP_STATE_COOKIE_ECHOED */ \
	TYPE_SCTP_FUNC(sctp_sf_do_6_3_3_rtx), \
	/* SCTP_STATE_ESTABLISHED */ \
	TYPE_SCTP_FUNC(sctp_sf_do_6_3_3_rtx), \
	/* SCTP_STATE_SHUTDOWN_PENDING */ \
	TYPE_SCTP_FUNC(sctp_sf_do_6_3_3_rtx), \
	/* SCTP_STATE_SHUTDOWN_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
	/* SCTP_STATE_SHUTDOWN_RECEIVED */ \
	TYPE_SCTP_FUNC(sctp_sf_do_6_3_3_rtx), \
	/* SCTP_STATE_SHUTDOWN_ACK_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
}

#define TYPE_SCTP_EVENT_TIMEOUT_T4_RTO { \
	/* SCTP_STATE_CLOSED */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
	/* SCTP_STATE_COOKIE_WAIT */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
	/* SCTP_STATE_COOKIE_ECHOED */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
	/* SCTP_STATE_ESTABLISHED */ \
	TYPE_SCTP_FUNC(sctp_sf_t4_timer_expire), \
	/* SCTP_STATE_SHUTDOWN_PENDING */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
	/* SCTP_STATE_SHUTDOWN_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
	/* SCTP_STATE_SHUTDOWN_RECEIVED */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
	/* SCTP_STATE_SHUTDOWN_ACK_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
}

#define TYPE_SCTP_EVENT_TIMEOUT_T5_SHUTDOWN_GUARD { \
	/* SCTP_STATE_CLOSED */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
	/* SCTP_STATE_COOKIE_WAIT */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
	/* SCTP_STATE_COOKIE_ECHOED */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
	/* SCTP_STATE_ESTABLISHED */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
	/* SCTP_STATE_SHUTDOWN_PENDING */ \
	TYPE_SCTP_FUNC(sctp_sf_t5_timer_expire), \
	/* SCTP_STATE_SHUTDOWN_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_t5_timer_expire), \
	/* SCTP_STATE_SHUTDOWN_RECEIVED */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
	/* SCTP_STATE_SHUTDOWN_ACK_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
}

#define TYPE_SCTP_EVENT_TIMEOUT_HEARTBEAT { \
	/* SCTP_STATE_CLOSED */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
	/* SCTP_STATE_COOKIE_WAIT */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
	/* SCTP_STATE_COOKIE_ECHOED */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
	/* SCTP_STATE_ESTABLISHED */ \
	TYPE_SCTP_FUNC(sctp_sf_sendbeat_8_3), \
	/* SCTP_STATE_SHUTDOWN_PENDING */ \
	TYPE_SCTP_FUNC(sctp_sf_sendbeat_8_3), \
	/* SCTP_STATE_SHUTDOWN_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
	/* SCTP_STATE_SHUTDOWN_RECEIVED */ \
	TYPE_SCTP_FUNC(sctp_sf_sendbeat_8_3), \
	/* SCTP_STATE_SHUTDOWN_ACK_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
}

#define TYPE_SCTP_EVENT_TIMEOUT_SACK { \
	/* SCTP_STATE_CLOSED */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
	/* SCTP_STATE_COOKIE_WAIT */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
	/* SCTP_STATE_COOKIE_ECHOED */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
	/* SCTP_STATE_ESTABLISHED */ \
	TYPE_SCTP_FUNC(sctp_sf_do_6_2_sack), \
	/* SCTP_STATE_SHUTDOWN_PENDING */ \
	TYPE_SCTP_FUNC(sctp_sf_do_6_2_sack), \
	/* SCTP_STATE_SHUTDOWN_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_do_6_2_sack), \
	/* SCTP_STATE_SHUTDOWN_RECEIVED */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
	/* SCTP_STATE_SHUTDOWN_ACK_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
}

#define TYPE_SCTP_EVENT_TIMEOUT_AUTOCLOSE { \
	/* SCTP_STATE_CLOSED */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
	/* SCTP_STATE_COOKIE_WAIT */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
	/* SCTP_STATE_COOKIE_ECHOED */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
	/* SCTP_STATE_ESTABLISHED */ \
	TYPE_SCTP_FUNC(sctp_sf_autoclose_timer_expire), \
	/* SCTP_STATE_SHUTDOWN_PENDING */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
	/* SCTP_STATE_SHUTDOWN_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
	/* SCTP_STATE_SHUTDOWN_RECEIVED */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
	/* SCTP_STATE_SHUTDOWN_ACK_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
}

#define TYPE_SCTP_EVENT_TIMEOUT_RECONF { \
	/* SCTP_STATE_CLOSED */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
	/* SCTP_STATE_COOKIE_WAIT */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
	/* SCTP_STATE_COOKIE_ECHOED */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
	/* SCTP_STATE_ESTABLISHED */ \
	TYPE_SCTP_FUNC(sctp_sf_send_reconf), \
	/* SCTP_STATE_SHUTDOWN_PENDING */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
	/* SCTP_STATE_SHUTDOWN_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
	/* SCTP_STATE_SHUTDOWN_RECEIVED */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
	/* SCTP_STATE_SHUTDOWN_ACK_SENT */ \
	TYPE_SCTP_FUNC(sctp_sf_timer_ignore), \
}

static const struct sctp_sm_table_entry
timeout_event_table[SCTP_NUM_TIMEOUT_TYPES][SCTP_STATE_NUM_STATES] = {
	TYPE_SCTP_EVENT_TIMEOUT_NONE,
	TYPE_SCTP_EVENT_TIMEOUT_T1_COOKIE,
	TYPE_SCTP_EVENT_TIMEOUT_T1_INIT,
	TYPE_SCTP_EVENT_TIMEOUT_T2_SHUTDOWN,
	TYPE_SCTP_EVENT_TIMEOUT_T3_RTX,
	TYPE_SCTP_EVENT_TIMEOUT_T4_RTO,
	TYPE_SCTP_EVENT_TIMEOUT_T5_SHUTDOWN_GUARD,
	TYPE_SCTP_EVENT_TIMEOUT_HEARTBEAT,
	TYPE_SCTP_EVENT_TIMEOUT_RECONF,
	TYPE_SCTP_EVENT_TIMEOUT_SACK,
	TYPE_SCTP_EVENT_TIMEOUT_AUTOCLOSE,
};

static const struct sctp_sm_table_entry *sctp_chunk_event_lookup(
						struct net *net,
						enum sctp_cid cid,
						enum sctp_state state)
{
	if (state > SCTP_STATE_MAX)
		return &bug;

	if (cid == SCTP_CID_I_DATA)
		cid = SCTP_CID_DATA;

	if (cid <= SCTP_CID_BASE_MAX)
		return &chunk_event_table[cid][state];

	switch ((u16)cid) {
	case SCTP_CID_FWD_TSN:
	case SCTP_CID_I_FWD_TSN:
		return &prsctp_chunk_event_table[0][state];

	case SCTP_CID_ASCONF:
		return &addip_chunk_event_table[0][state];

	case SCTP_CID_ASCONF_ACK:
		return &addip_chunk_event_table[1][state];

	case SCTP_CID_RECONF:
		return &reconf_chunk_event_table[0][state];

	case SCTP_CID_AUTH:
		return &auth_chunk_event_table[0][state];
	}

	return &chunk_event_table_unknown[state];
}
