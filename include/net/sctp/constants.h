/* SPDX-License-Identifier: GPL-2.0-or-later */
/* SCTP kernel implementation
 * (C) Copyright IBM Corp. 2001, 2004
 * Copyright (c) 1999-2000 Cisco, Inc.
 * Copyright (c) 1999-2001 Motorola, Inc.
 * Copyright (c) 2001 Intel Corp.
 *
 * This file is part of the SCTP kernel implementation
 *
 * Please send any bug reports or fixes you make to the
 * email address(es):
 *    lksctp developers <linux-sctp@vger.kernel.org>
 *
 * Written or modified by:
 *   La Monte H.P. Yarroll <piggy@acm.org>
 *   Karl Knutson          <karl@athena.chicago.il.us>
 *   Randall Stewart       <randall@stewart.chicago.il.us>
 *   Ken Morneau           <kmorneau@cisco.com>
 *   Qiaobing Xie          <qxie1@motorola.com>
 *   Xingang Guo           <xingang.guo@intel.com>
 *   Sridhar Samudrala     <samudrala@us.ibm.com>
 *   Daisy Chang           <daisyc@us.ibm.com>
 */

#ifndef __sctp_constants_h__
#define __sctp_constants_h__

#include <linux/sctp.h>
#include <linux/ipv6.h> /* For ipv6hdr. */
#include <net/tcp_states.h>  /* For TCP states used in enum sctp_sock_state */

/* Value used for stream negotiation. */
enum { SCTP_MAX_STREAM = 0xffff };
enum { SCTP_DEFAULT_OUTSTREAMS = 10 };
enum { SCTP_DEFAULT_INSTREAMS = SCTP_MAX_STREAM };

/* Since CIDs are sparse, we need all four of the following
 * symbols.  CIDs are dense through SCTP_CID_BASE_MAX.
 */
#define SCTP_CID_BASE_MAX		SCTP_CID_SHUTDOWN_COMPLETE

#define SCTP_NUM_BASE_CHUNK_TYPES	(SCTP_CID_BASE_MAX + 1)

#define SCTP_NUM_ADDIP_CHUNK_TYPES	2

#define SCTP_NUM_PRSCTP_CHUNK_TYPES	1

#define SCTP_NUM_RECONF_CHUNK_TYPES	1

#define SCTP_NUM_AUTH_CHUNK_TYPES	1

#define SCTP_NUM_CHUNK_TYPES		(SCTP_NUM_BASE_CHUNK_TYPES + \
					 SCTP_NUM_ADDIP_CHUNK_TYPES +\
					 SCTP_NUM_PRSCTP_CHUNK_TYPES +\
					 SCTP_NUM_RECONF_CHUNK_TYPES +\
					 SCTP_NUM_AUTH_CHUNK_TYPES)

/* These are the different flavours of event.  */
enum sctp_event_type {
	SCTP_EVENT_T_CHUNK = 1,
	SCTP_EVENT_T_TIMEOUT,
	SCTP_EVENT_T_OTHER,
	SCTP_EVENT_T_PRIMITIVE
};

/* As a convenience for the state machine, we append SCTP_EVENT_* and
 * SCTP_ULP_* to the list of possible chunks.
 */

enum sctp_event_timeout {
	SCTP_EVENT_TIMEOUT_NONE = 0,
	SCTP_EVENT_TIMEOUT_T1_COOKIE,
	SCTP_EVENT_TIMEOUT_T1_INIT,
	SCTP_EVENT_TIMEOUT_T2_SHUTDOWN,
	SCTP_EVENT_TIMEOUT_T3_RTX,
	SCTP_EVENT_TIMEOUT_T4_RTO,
	SCTP_EVENT_TIMEOUT_T5_SHUTDOWN_GUARD,
	SCTP_EVENT_TIMEOUT_HEARTBEAT,
	SCTP_EVENT_TIMEOUT_RECONF,
	SCTP_EVENT_TIMEOUT_SACK,
	SCTP_EVENT_TIMEOUT_AUTOCLOSE,
};

#define SCTP_EVENT_TIMEOUT_MAX		SCTP_EVENT_TIMEOUT_AUTOCLOSE
#define SCTP_NUM_TIMEOUT_TYPES		(SCTP_EVENT_TIMEOUT_MAX + 1)

enum sctp_event_other {
	SCTP_EVENT_NO_PENDING_TSN = 0,
	SCTP_EVENT_ICMP_PROTO_UNREACH,
};

#define SCTP_EVENT_OTHER_MAX		SCTP_EVENT_ICMP_PROTO_UNREACH
#define SCTP_NUM_OTHER_TYPES		(SCTP_EVENT_OTHER_MAX + 1)

/* These are primitive requests from the ULP.  */
enum sctp_event_primitive {
	SCTP_PRIMITIVE_ASSOCIATE = 0,
	SCTP_PRIMITIVE_SHUTDOWN,
	SCTP_PRIMITIVE_ABORT,
	SCTP_PRIMITIVE_SEND,
	SCTP_PRIMITIVE_REQUESTHEARTBEAT,
	SCTP_PRIMITIVE_ASCONF,
	SCTP_PRIMITIVE_RECONF,
};

#define SCTP_EVENT_PRIMITIVE_MAX	SCTP_PRIMITIVE_RECONF
#define SCTP_NUM_PRIMITIVE_TYPES	(SCTP_EVENT_PRIMITIVE_MAX + 1)

/* We define here a utility type for manipulating subtypes.
 * The subtype constructors all work like this:
 *
 *   union sctp_subtype foo = SCTP_ST_CHUNK(SCTP_CID_INIT);
 */

union sctp_subtype {
	enum sctp_cid chunk;
	enum sctp_event_timeout timeout;
	enum sctp_event_other other;
	enum sctp_event_primitive primitive;
};

#define SCTP_SUBTYPE_CONSTRUCTOR(_name, _type, _elt) \
static inline union sctp_subtype	\
SCTP_ST_## _name (_type _arg)		\
{ union sctp_subtype _retval; _retval._elt = _arg; return _retval; }

SCTP_SUBTYPE_CONSTRUCTOR(CHUNK,		enum sctp_cid,		chunk)
SCTP_SUBTYPE_CONSTRUCTOR(TIMEOUT,	enum sctp_event_timeout, timeout)
SCTP_SUBTYPE_CONSTRUCTOR(OTHER,		enum sctp_event_other,	other)
SCTP_SUBTYPE_CONSTRUCTOR(PRIMITIVE,	enum sctp_event_primitive, primitive)


#define sctp_chunk_is_data(a) (a->chunk_hdr->type == SCTP_CID_DATA || \
			       a->chunk_hdr->type == SCTP_CID_I_DATA)

/* Internal error codes */
enum sctp_ierror {
	SCTP_IERROR_NO_ERROR	        = 0,
	SCTP_IERROR_BASE		= 1000,
	SCTP_IERROR_NO_COOKIE,
	SCTP_IERROR_BAD_SIG,
	SCTP_IERROR_STALE_COOKIE,
	SCTP_IERROR_NOMEM,
	SCTP_IERROR_MALFORMED,
	SCTP_IERROR_BAD_TAG,
	SCTP_IERROR_BIG_GAP,
	SCTP_IERROR_DUP_TSN,
	SCTP_IERROR_HIGH_TSN,
	SCTP_IERROR_IGNORE_TSN,
	SCTP_IERROR_NO_DATA,
	SCTP_IERROR_BAD_STREAM,
	SCTP_IERROR_BAD_PORTS,
	SCTP_IERROR_AUTH_BAD_HMAC,
	SCTP_IERROR_AUTH_BAD_KEYID,
	SCTP_IERROR_PROTO_VIOLATION,
	SCTP_IERROR_ERROR,
	SCTP_IERROR_ABORT,
};



/* SCTP state defines for internal state machine */
enum sctp_state {

	SCTP_STATE_CLOSED		= 0,
	SCTP_STATE_COOKIE_WAIT		= 1,
	SCTP_STATE_COOKIE_ECHOED	= 2,
	SCTP_STATE_ESTABLISHED		= 3,
	SCTP_STATE_SHUTDOWN_PENDING	= 4,
	SCTP_STATE_SHUTDOWN_SENT	= 5,
	SCTP_STATE_SHUTDOWN_RECEIVED	= 6,
	SCTP_STATE_SHUTDOWN_ACK_SENT	= 7,

};

#define SCTP_STATE_MAX			SCTP_STATE_SHUTDOWN_ACK_SENT
#define SCTP_STATE_NUM_STATES		(SCTP_STATE_MAX + 1)

/* These are values for sk->state.
 * For a UDP-style SCTP socket, the states are defined as follows
 * - A socket in SCTP_SS_CLOSED state indicates that it is not willing to
 *   accept new associations, but it can initiate the creation of new ones.
 * - A socket in SCTP_SS_LISTENING state indicates that it is willing to
 *   accept new  associations and can initiate the creation of new ones.
 * - A socket in SCTP_SS_ESTABLISHED state indicates that it is a peeled off
 *   socket with one association.
 * For a TCP-style SCTP socket, the states are defined as follows
 * - A socket in SCTP_SS_CLOSED state indicates that it is not willing to
 *   accept new associations, but it can initiate the creation of new ones.
 * - A socket in SCTP_SS_LISTENING state indicates that it is willing to
 *   accept new associations, but cannot initiate the creation of new ones.
 * - A socket in SCTP_SS_ESTABLISHED state indicates that it has a single 
 *   association.
 */
enum sctp_sock_state {
	SCTP_SS_CLOSED         = TCP_CLOSE,
	SCTP_SS_LISTENING      = TCP_LISTEN,
	SCTP_SS_ESTABLISHING   = TCP_SYN_SENT,
	SCTP_SS_ESTABLISHED    = TCP_ESTABLISHED,
	SCTP_SS_CLOSING        = TCP_CLOSE_WAIT,
};

/* These functions map various type to printable names.  */
const char *sctp_cname(const union sctp_subtype id);	/* chunk types */
const char *sctp_oname(const union sctp_subtype id);	/* other events */
const char *sctp_tname(const union sctp_subtype id);	/* timeouts */
const char *sctp_pname(const union sctp_subtype id);	/* primitives */

/* This is a table of printable names of sctp_state_t's.  */
extern const char *const sctp_state_tbl[];
extern const char *const sctp_evttype_tbl[];
extern const char *const sctp_status_tbl[];

/* Maximum chunk length considering padding requirements. */
enum { SCTP_MAX_CHUNK_LEN = ((1<<16) - sizeof(__u32)) };

/* Encourage Cookie-Echo bundling by pre-fragmenting chunks a little
 * harder (until reaching ESTABLISHED state).
 */
enum { SCTP_ARBITRARY_COOKIE_ECHO_LEN = 200 };

/* Guess at how big to make the TSN mapping array.
 * We guarantee that we can handle at least this big a gap between the
 * cumulative ACK and the highest TSN.  In practice, we can often
 * handle up to twice this value.
 *
 * NEVER make this more than 32767 (2^15-1).  The Gap Ack Blocks in a
 * SACK (see  section 3.3.4) are only 16 bits, so 2*SCTP_TSN_MAP_SIZE
 * must be less than 65535 (2^16 - 1), or we will have overflow
 * problems creating SACK's.
 */
#define SCTP_TSN_MAP_INITIAL BITS_PER_LONG
#define SCTP_TSN_MAP_INCREMENT SCTP_TSN_MAP_INITIAL
#define SCTP_TSN_MAP_SIZE 4096

/* We will not record more than this many duplicate TSNs between two
 * SACKs.  The minimum PMTU is 512.  Remove all the headers and there
 * is enough room for 117 duplicate reports.  Round down to the
 * nearest power of 2.
 */
enum { SCTP_MAX_DUP_TSNS = 16 };
enum { SCTP_MAX_GABS = 16 };

/* Heartbeat interval - 30 secs */
#define SCTP_DEFAULT_TIMEOUT_HEARTBEAT	(30*1000)

/* Delayed sack timer - 200ms */
#define SCTP_DEFAULT_TIMEOUT_SACK	(200)

/* RTO.Initial              - 3  seconds
 * RTO.Min                  - 1  second
 * RTO.Max                  - 60 seconds
 * RTO.Alpha                - 1/8
 * RTO.Beta                 - 1/4
 */
#define SCTP_RTO_INITIAL	(3 * 1000)
#define SCTP_RTO_MIN		(1 * 1000)
#define SCTP_RTO_MAX		(60 * 1000)

#define SCTP_RTO_ALPHA          3   /* 1/8 when converted to right shifts. */
#define SCTP_RTO_BETA           2   /* 1/4 when converted to right shifts. */

/* Maximum number of new data packets that can be sent in a burst.  */
#define SCTP_DEFAULT_MAX_BURST		4

#define SCTP_CLOCK_GRANULARITY	1	/* 1 jiffy */

#define SCTP_DEFAULT_COOKIE_LIFE	(60 * 1000) /* 60 seconds */

#define SCTP_DEFAULT_MINWINDOW	1500	/* default minimum rwnd size */
#define SCTP_DEFAULT_MAXWINDOW	65535	/* default rwnd size */
#define SCTP_DEFAULT_RWND_SHIFT  4	/* by default, update on 1/16 of
					 * rcvbuf, which is 1/8 of initial
					 * window
					 */
#define SCTP_DEFAULT_MAXSEGMENT 1500	/* MTU size, this is the limit
                                         * to which we will raise the P-MTU.
					 */
#define SCTP_DEFAULT_MINSEGMENT 512	/* MTU size ... if no mtu disc */

#define SCTP_SECRET_SIZE 32		/* Number of octets in a 256 bits. */

#define SCTP_SIGNATURE_SIZE 20	        /* size of a SLA-1 signature */

#define SCTP_COOKIE_MULTIPLE 32 /* Pad out our cookie to make our hash
				 * functions simpler to write.
				 */

#define SCTP_DEFAULT_UDP_PORT 9899	/* default UDP tunneling port */

/* These are the values for pf exposure, UNUSED is to keep compatible with old
 * applications by default.
 */
enum {
	SCTP_PF_EXPOSE_UNSET,
	SCTP_PF_EXPOSE_DISABLE,
	SCTP_PF_EXPOSE_ENABLE,
};
#define SCTP_PF_EXPOSE_MAX	SCTP_PF_EXPOSE_ENABLE

#define SCTP_PS_RETRANS_MAX	0xffff

/* These return values describe the success or failure of a number of
 * routines which form the lower interface to SCTP_outqueue.
 */
enum sctp_xmit {
	SCTP_XMIT_OK,
	SCTP_XMIT_PMTU_FULL,
	SCTP_XMIT_RWND_FULL,
	SCTP_XMIT_DELAY,
};

/* These are the commands for manipulating transports.  */
enum sctp_transport_cmd {
	SCTP_TRANSPORT_UP,
	SCTP_TRANSPORT_DOWN,
	SCTP_TRANSPORT_PF,
};

/* These are the address scopes defined mainly for IPv4 addresses
 * based on draft of SCTP IPv4 scoping <draft-stewart-tsvwg-sctp-ipv4-00.txt>.
 * These scopes are hopefully generic enough to be used on scoping both
 * IPv4 and IPv6 addresses in SCTP.
 * At this point, the IPv6 scopes will be mapped to these internal scopes
 * as much as possible.
 */
enum sctp_scope {
	SCTP_SCOPE_GLOBAL,		/* IPv4 global addresses */
	SCTP_SCOPE_PRIVATE,		/* IPv4 private addresses */
	SCTP_SCOPE_LINK,		/* IPv4 link local address */
	SCTP_SCOPE_LOOPBACK,		/* IPv4 loopback address */
	SCTP_SCOPE_UNUSABLE,		/* IPv4 unusable addresses */
};

enum {
	SCTP_SCOPE_POLICY_DISABLE,	/* Disable IPv4 address scoping */
	SCTP_SCOPE_POLICY_ENABLE,	/* Enable IPv4 address scoping */
	SCTP_SCOPE_POLICY_PRIVATE,	/* Follow draft but allow IPv4 private addresses */
	SCTP_SCOPE_POLICY_LINK,		/* Follow draft but allow IPv4 link local addresses */
};

#define SCTP_SCOPE_POLICY_MAX	SCTP_SCOPE_POLICY_LINK

/* Based on IPv4 scoping <draft-stewart-tsvwg-sctp-ipv4-00.txt>,
 * SCTP IPv4 unusable addresses: 0.0.0.0/8, 224.0.0.0/4, 198.18.0.0/24,
 * 192.88.99.0/24.
 * Also, RFC 8.4, non-unicast addresses are not considered valid SCTP
 * addresses.
 */
#define IS_IPV4_UNUSABLE_ADDRESS(a)	    \
	((htonl(INADDR_BROADCAST) == a) ||  \
	 ipv4_is_multicast(a) ||	    \
	 ipv4_is_zeronet(a) ||		    \
	 ipv4_is_test_198(a) ||		    \
	 ipv4_is_anycast_6to4(a))

/* Flags used for the bind address copy functions.  */
#define SCTP_ADDR4_ALLOWED	0x00000001	/* IPv4 address is allowed by
						   local sock family */
#define SCTP_ADDR6_ALLOWED	0x00000002	/* IPv6 address is allowed by
						   local sock family */
#define SCTP_ADDR4_PEERSUPP	0x00000004	/* IPv4 address is supported by
						   peer */
#define SCTP_ADDR6_PEERSUPP	0x00000008	/* IPv6 address is supported by
						   peer */

/* Reasons to retransmit. */
enum sctp_retransmit_reason {
	SCTP_RTXR_T3_RTX,
	SCTP_RTXR_FAST_RTX,
	SCTP_RTXR_PMTUD,
	SCTP_RTXR_T1_RTX,
};

/* Reasons to lower cwnd. */
enum sctp_lower_cwnd {
	SCTP_LOWER_CWND_T3_RTX,
	SCTP_LOWER_CWND_FAST_RTX,
	SCTP_LOWER_CWND_ECNE,
	SCTP_LOWER_CWND_INACTIVE,
};


/* SCTP-AUTH Necessary constants */

/* SCTP-AUTH, Section 3.3
 *
 *  The following Table 2 shows the currently defined values for HMAC
 *  identifiers.
 *
 *  +-----------------+--------------------------+
 *  | HMAC Identifier | Message Digest Algorithm |
 *  +-----------------+--------------------------+
 *  | 0               | Reserved                 |
 *  | 1               | SHA-1 defined in [8]     |
 *  | 2               | Reserved                 |
 *  | 3               | SHA-256 defined in [8]   |
 *  +-----------------+--------------------------+
 */
enum {
	SCTP_AUTH_HMAC_ID_RESERVED_0,
	SCTP_AUTH_HMAC_ID_SHA1,
	SCTP_AUTH_HMAC_ID_RESERVED_2,
#if defined (CONFIG_CRYPTO_SHA256) || defined (CONFIG_CRYPTO_SHA256_MODULE)
	SCTP_AUTH_HMAC_ID_SHA256,
#endif
	__SCTP_AUTH_HMAC_MAX
};

#define SCTP_AUTH_HMAC_ID_MAX	__SCTP_AUTH_HMAC_MAX - 1
#define SCTP_AUTH_NUM_HMACS 	__SCTP_AUTH_HMAC_MAX
#define SCTP_SHA1_SIG_SIZE 20
#define SCTP_SHA256_SIG_SIZE 32

/*  SCTP-AUTH, Section 3.2
 *     The chunk types for INIT, INIT-ACK, SHUTDOWN-COMPLETE and AUTH chunks
 *     MUST NOT be listed in the CHUNKS parameter
 */
#define SCTP_NUM_NOAUTH_CHUNKS	4
#define SCTP_AUTH_MAX_CHUNKS	(SCTP_NUM_CHUNK_TYPES - SCTP_NUM_NOAUTH_CHUNKS)

/* SCTP-AUTH Section 6.1
 * The RANDOM parameter MUST contain a 32 byte random number.
 */
#define SCTP_AUTH_RANDOM_LENGTH 32

#endif /* __sctp_constants_h__ */
