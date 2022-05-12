/* SPDX-License-Identifier: GPL-2.0 */
/* Copyright (C) B.A.T.M.A.N. contributors:
 *
 * Marek Lindner, Simon Wunderlich
 */

#ifndef _NET_BATMAN_ADV_MAIN_H_
#define _NET_BATMAN_ADV_MAIN_H_

#define BATADV_DRIVER_AUTHOR "Marek Lindner <mareklindner@neomailbox.ch>, " \
			     "Simon Wunderlich <sw@simonwunderlich.de>"
#define BATADV_DRIVER_DESC   "B.A.T.M.A.N. advanced"
#define BATADV_DRIVER_DEVICE "batman-adv"

#ifndef BATADV_SOURCE_VERSION
#define BATADV_SOURCE_VERSION "2021.1"
#endif

/* B.A.T.M.A.N. parameters */

#define BATADV_TQ_MAX_VALUE 255
#define BATADV_THROUGHPUT_MAX_VALUE 0xFFFFFFFF
#define BATADV_JITTER 20

/* Time To Live of broadcast messages */
#define BATADV_TTL 50

/* maximum sequence number age of broadcast messages */
#define BATADV_BCAST_MAX_AGE 64

/* purge originators after time in seconds if no valid packet comes in
 * -> TODO: check influence on BATADV_TQ_LOCAL_WINDOW_SIZE
 */
#define BATADV_PURGE_TIMEOUT 200000 /* 200 seconds */
#define BATADV_TT_LOCAL_TIMEOUT 600000 /* in milliseconds */
#define BATADV_TT_CLIENT_ROAM_TIMEOUT 600000 /* in milliseconds */
#define BATADV_TT_CLIENT_TEMP_TIMEOUT 600000 /* in milliseconds */
#define BATADV_TT_WORK_PERIOD 5000 /* 5 seconds */
#define BATADV_ORIG_WORK_PERIOD 1000 /* 1 second */
#define BATADV_MCAST_WORK_PERIOD 500 /* 0.5 seconds */
#define BATADV_DAT_ENTRY_TIMEOUT (5 * 60000) /* 5 mins in milliseconds */
/* sliding packet range of received originator messages in sequence numbers
 * (should be a multiple of our word size)
 */
#define BATADV_TQ_LOCAL_WINDOW_SIZE 64
/* milliseconds we have to keep pending tt_req */
#define BATADV_TT_REQUEST_TIMEOUT 3000

#define BATADV_TQ_GLOBAL_WINDOW_SIZE 5
#define BATADV_TQ_LOCAL_BIDRECT_SEND_MINIMUM 1
#define BATADV_TQ_LOCAL_BIDRECT_RECV_MINIMUM 1
#define BATADV_TQ_TOTAL_BIDRECT_LIMIT 1

/* B.A.T.M.A.N. V */
#define BATADV_THROUGHPUT_DEFAULT_VALUE 10 /* 1 Mbps */
#define BATADV_ELP_PROBES_PER_NODE 2
#define BATADV_ELP_MIN_PROBE_SIZE 200 /* bytes */
#define BATADV_ELP_PROBE_MAX_TX_DIFF 100 /* milliseconds */
#define BATADV_ELP_MAX_AGE 64
#define BATADV_OGM_MAX_ORIGDIFF 5
#define BATADV_OGM_MAX_AGE 64

/* number of OGMs sent with the last tt diff */
#define BATADV_TT_OGM_APPEND_MAX 3

/* Time in which a client can roam at most ROAMING_MAX_COUNT times in
 * milliseconds
 */
#define BATADV_ROAMING_MAX_TIME 20000
#define BATADV_ROAMING_MAX_COUNT 5

#define BATADV_NO_FLAGS 0

#define BATADV_NULL_IFINDEX 0 /* dummy ifindex used to avoid iface checks */

#define BATADV_NO_MARK 0

/* default interface for multi interface operation. The default interface is
 * used for communication which originated locally (i.e. is not forwarded)
 * or where special forwarding is not desired/necessary.
 */
#define BATADV_IF_DEFAULT	((struct batadv_hard_iface *)NULL)

#define BATADV_NUM_WORDS BITS_TO_LONGS(BATADV_TQ_LOCAL_WINDOW_SIZE)

#define BATADV_LOG_BUF_LEN 8192	  /* has to be a power of 2 */

/* number of packets to send for broadcasts on different interface types */
#define BATADV_NUM_BCASTS_DEFAULT 1
#define BATADV_NUM_BCASTS_WIRELESS 3
#define BATADV_NUM_BCASTS_MAX 3

/* length of the single packet used by the TP meter */
#define BATADV_TP_PACKET_LEN ETH_DATA_LEN

/* msecs after which an ARP_REQUEST is sent in broadcast as fallback */
#define ARP_REQ_DELAY 250
/* numbers of originator to contact for any PUT/GET DHT operation */
#define BATADV_DAT_CANDIDATES_NUM 3

/* BATADV_TQ_SIMILARITY_THRESHOLD - TQ points that a secondary metric can differ
 * at most from the primary one in order to be still considered acceptable
 */
#define BATADV_TQ_SIMILARITY_THRESHOLD 50

/* should not be bigger than 512 bytes or change the size of
 * forw_packet->direct_link_flags
 */
#define BATADV_MAX_AGGREGATION_BYTES 512
#define BATADV_MAX_AGGREGATION_MS 100

#define BATADV_BLA_PERIOD_LENGTH	10000	/* 10 seconds */
#define BATADV_BLA_BACKBONE_TIMEOUT	(BATADV_BLA_PERIOD_LENGTH * 6)
#define BATADV_BLA_CLAIM_TIMEOUT	(BATADV_BLA_PERIOD_LENGTH * 10)
#define BATADV_BLA_WAIT_PERIODS		3
#define BATADV_BLA_LOOPDETECT_PERIODS	6
#define BATADV_BLA_LOOPDETECT_TIMEOUT	3000	/* 3 seconds */

#define BATADV_DUPLIST_SIZE		16
#define BATADV_DUPLIST_TIMEOUT		500	/* 500 ms */
/* don't reset again within 30 seconds */
#define BATADV_RESET_PROTECTION_MS 30000
#define BATADV_EXPECTED_SEQNO_RANGE	65536

#define BATADV_NC_NODE_TIMEOUT 10000 /* Milliseconds */

/**
 * BATADV_TP_MAX_NUM - maximum number of simultaneously active tp sessions
 */
#define BATADV_TP_MAX_NUM 5

/**
 * enum batadv_mesh_state - State of a soft interface
 */
enum batadv_mesh_state {
	/** @BATADV_MESH_INACTIVE: soft interface is not yet running */
	BATADV_MESH_INACTIVE,

	/** @BATADV_MESH_ACTIVE: interface is up and running */
	BATADV_MESH_ACTIVE,

	/** @BATADV_MESH_DEACTIVATING: interface is getting shut down */
	BATADV_MESH_DEACTIVATING,
};

#define BATADV_BCAST_QUEUE_LEN		256
#define BATADV_BATMAN_QUEUE_LEN	256

/**
 * enum batadv_uev_action - action type of uevent
 */
enum batadv_uev_action {
	/** @BATADV_UEV_ADD: gateway was selected (after none was selected) */
	BATADV_UEV_ADD = 0,

	/**
	 * @BATADV_UEV_DEL: selected gateway was removed and none is selected
	 * anymore
	 */
	BATADV_UEV_DEL,

	/**
	 * @BATADV_UEV_CHANGE: a different gateway was selected as based gateway
	 */
	BATADV_UEV_CHANGE,

	/**
	 * @BATADV_UEV_LOOPDETECT: loop was detected which cannot be handled by
	 * bridge loop avoidance
	 */
	BATADV_UEV_LOOPDETECT,
};

/**
 * enum batadv_uev_type - Type of uevent
 */
enum batadv_uev_type {
	/** @BATADV_UEV_GW: selected gateway was modified */
	BATADV_UEV_GW = 0,

	/** @BATADV_UEV_BLA: bridge loop avoidance event */
	BATADV_UEV_BLA,
};

#define BATADV_GW_THRESHOLD	50

/* Number of fragment chains for each orig_node */
#define BATADV_FRAG_BUFFER_COUNT 8
/* Maximum number of fragments for one packet */
#define BATADV_FRAG_MAX_FRAGMENTS 16
/* Maxumim size of each fragment */
#define BATADV_FRAG_MAX_FRAG_SIZE 1280
/* Time to keep fragments while waiting for rest of the fragments */
#define BATADV_FRAG_TIMEOUT 10000

#define BATADV_DAT_CANDIDATE_NOT_FOUND	0
#define BATADV_DAT_CANDIDATE_ORIG	1

/* Debug Messages */
#ifdef pr_fmt
#undef pr_fmt
#endif
/* Append 'batman-adv: ' before kernel messages */
#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

/* Kernel headers */

#include <linux/atomic.h>
#include <linux/compiler.h>
#include <linux/etherdevice.h>
#include <linux/if_vlan.h>
#include <linux/jiffies.h>
#include <linux/netdevice.h>
#include <linux/percpu.h>
#include <linux/skbuff.h>
#include <linux/types.h>
#include <uapi/linux/batadv_packet.h>

#include "types.h"
#include "main.h"

/**
 * batadv_print_vid() - return printable version of vid information
 * @vid: the VLAN identifier
 *
 * Return: -1 when no VLAN is used, VLAN id otherwise
 */
static inline int batadv_print_vid(unsigned short vid)
{
	if (vid & BATADV_VLAN_HAS_TAG)
		return (int)(vid & VLAN_VID_MASK);
	else
		return -1;
}

extern struct list_head batadv_hardif_list;
extern unsigned int batadv_hardif_generation;

extern unsigned char batadv_broadcast_addr[];
extern struct workqueue_struct *batadv_event_workqueue;

int batadv_mesh_init(struct net_device *soft_iface);
void batadv_mesh_free(struct net_device *soft_iface);
bool batadv_is_my_mac(struct batadv_priv *bat_priv, const u8 *addr);
int batadv_max_header_len(void);
void batadv_skb_set_priority(struct sk_buff *skb, int offset);
int batadv_batman_skb_recv(struct sk_buff *skb, struct net_device *dev,
			   struct packet_type *ptype,
			   struct net_device *orig_dev);
int
batadv_recv_handler_register(u8 packet_type,
			     int (*recv_handler)(struct sk_buff *,
						 struct batadv_hard_iface *));
void batadv_recv_handler_unregister(u8 packet_type);
__be32 batadv_skb_crc32(struct sk_buff *skb, u8 *payload_ptr);

/**
 * batadv_compare_eth() - Compare two not u16 aligned Ethernet addresses
 * @data1: Pointer to a six-byte array containing the Ethernet address
 * @data2: Pointer other six-byte array containing the Ethernet address
 *
 * note: can't use ether_addr_equal() as it requires aligned memory
 *
 * Return: true if they are the same ethernet addr
 */
static inline bool batadv_compare_eth(const void *data1, const void *data2)
{
	return ether_addr_equal_unaligned(data1, data2);
}

/**
 * batadv_has_timed_out() - compares current time (jiffies) and timestamp +
 *  timeout
 * @timestamp:		base value to compare with (in jiffies)
 * @timeout:		added to base value before comparing (in milliseconds)
 *
 * Return: true if current time is after timestamp + timeout
 */
static inline bool batadv_has_timed_out(unsigned long timestamp,
					unsigned int timeout)
{
	return time_is_before_jiffies(timestamp + msecs_to_jiffies(timeout));
}

/**
 * batadv_atomic_dec_not_zero() - Decrease unless the number is 0
 * @v: pointer of type atomic_t
 *
 * Return: non-zero if v was not 0, and zero otherwise.
 */
#define batadv_atomic_dec_not_zero(v)	atomic_add_unless((v), -1, 0)

/**
 * batadv_smallest_signed_int() - Returns the smallest signed integer in two's
 *  complement with the sizeof x
 * @x: type of integer
 *
 * Return: smallest signed integer of type
 */
#define batadv_smallest_signed_int(x) (1u << (7u + 8u * (sizeof(x) - 1u)))

/**
 * batadv_seq_before() - Checks if a sequence number x is a predecessor of y
 * @x: potential predecessor of @y
 * @y: value to compare @x against
 *
 * It handles overflows/underflows and can correctly check for a predecessor
 * unless the variable sequence number has grown by more than
 * 2**(bitwidth(x)-1)-1.
 *
 * This means that for a u8 with the maximum value 255, it would think:
 *
 * * when adding nothing - it is neither a predecessor nor a successor
 * * before adding more than 127 to the starting value - it is a predecessor,
 * * when adding 128 - it is neither a predecessor nor a successor,
 * * after adding more than 127 to the starting value - it is a successor
 *
 * Return: true when x is a predecessor of y, false otherwise
 */
#define batadv_seq_before(x, y) ({ \
	typeof(x)_d1 = (x); \
	typeof(y)_d2 = (y); \
	typeof(x)_dummy = (_d1 - _d2); \
	(void)(&_d1 == &_d2); \
	_dummy > batadv_smallest_signed_int(_dummy); \
})

/**
 * batadv_seq_after() - Checks if a sequence number x is a successor of y
 * @x: potential successor of @y
 * @y: value to compare @x against
 *
 * It handles overflows/underflows and can correctly check for a successor
 * unless the variable sequence number has grown by more than
 * 2**(bitwidth(x)-1)-1.
 *
 * This means that for a u8 with the maximum value 255, it would think:
 *
 * * when adding nothing - it is neither a predecessor nor a successor
 * * before adding more than 127 to the starting value - it is a predecessor,
 * * when adding 128 - it is neither a predecessor nor a successor,
 * * after adding more than 127 to the starting value - it is a successor
 *
 * Return: true when x is a successor of y, false otherwise
 */
#define batadv_seq_after(x, y) batadv_seq_before(y, x)

/**
 * batadv_add_counter() - Add to per cpu statistics counter of soft interface
 * @bat_priv: the bat priv with all the soft interface information
 * @idx: counter index which should be modified
 * @count: value to increase counter by
 *
 * Stop preemption on local cpu while incrementing the counter
 */
static inline void batadv_add_counter(struct batadv_priv *bat_priv, size_t idx,
				      size_t count)
{
	this_cpu_add(bat_priv->bat_counters[idx], count);
}

/**
 * batadv_inc_counter() - Increase per cpu statistics counter of soft interface
 * @b: the bat priv with all the soft interface information
 * @i: counter index which should be modified
 */
#define batadv_inc_counter(b, i) batadv_add_counter(b, i, 1)

/**
 * BATADV_SKB_CB() - Get batadv_skb_cb from skb control buffer
 * @__skb: skb holding the control buffer
 *
 * The members of the control buffer are defined in struct batadv_skb_cb in
 * types.h. The macro is inspired by the similar macro TCP_SKB_CB() in tcp.h.
 *
 * Return: pointer to the batadv_skb_cb of the skb
 */
#define BATADV_SKB_CB(__skb)       ((struct batadv_skb_cb *)&((__skb)->cb[0]))

unsigned short batadv_get_vid(struct sk_buff *skb, size_t header_len);
bool batadv_vlan_ap_isola_get(struct batadv_priv *bat_priv, unsigned short vid);
int batadv_throw_uevent(struct batadv_priv *bat_priv, enum batadv_uev_type type,
			enum batadv_uev_action action, const char *data);

#endif /* _NET_BATMAN_ADV_MAIN_H_ */
