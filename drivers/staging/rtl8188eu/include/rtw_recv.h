/* SPDX-License-Identifier: GPL-2.0 */
/******************************************************************************
 *
 * Copyright(c) 2007 - 2012 Realtek Corporation. All rights reserved.
 *
 ******************************************************************************/
#ifndef _RTW_RECV_H_
#define _RTW_RECV_H_

#include <osdep_service.h>
#include <drv_types.h>

#define NR_RECVFRAME 256

#define RXFRAME_ALIGN	8
#define RXFRAME_ALIGN_SZ	(1 << RXFRAME_ALIGN)

#define MAX_RXFRAME_CNT	512
#define MAX_RX_NUMBLKS		(32)
#define RECVFRAME_HDR_ALIGN 128

#define SNAP_SIZE sizeof(struct ieee80211_snap_hdr)

#define MAX_SUBFRAME_COUNT	64

/* for Rx reordering buffer control */
struct recv_reorder_ctrl {
	struct adapter	*padapter;
	bool enable;
	u16 indicate_seq;/* wstart_b, init_value=0xffff */
	u16 wend_b;
	u8 wsize_b;
	struct __queue pending_recvframe_queue;
	struct timer_list reordering_ctrl_timer;
};

struct	stainfo_rxcache	{
	u16	tid_rxseq[16];
/*
 *	unsigned short	tid0_rxseq;
 *	unsigned short	tid1_rxseq;
 *	unsigned short	tid2_rxseq;
 *	unsigned short	tid3_rxseq;
 *	unsigned short	tid4_rxseq;
 *	unsigned short	tid5_rxseq;
 *	unsigned short	tid6_rxseq;
 *	unsigned short	tid7_rxseq;
 *	unsigned short	tid8_rxseq;
 *	unsigned short	tid9_rxseq;
 *	unsigned short	tid10_rxseq;
 *	unsigned short	tid11_rxseq;
 *	unsigned short	tid12_rxseq;
 *	unsigned short	tid13_rxseq;
 *	unsigned short	tid14_rxseq;
 *	unsigned short	tid15_rxseq;
 */
};

struct signal_stat {
	u8	update_req;		/* used to indicate */
	u8	avg_val;		/* avg of valid elements */
	u32	total_num;		/* num of valid elements */
	u32	total_val;		/* sum of valid elements */
};

#define MAX_PATH_NUM_92CS		3

struct phy_info {
	u8	RxPWDBAll;
	u8	SignalQuality;	 /*  in 0-100 index. */
	u8	RxMIMOSignalQuality[MAX_PATH_NUM_92CS]; /* EVM */
	u8	RxMIMOSignalStrength[MAX_PATH_NUM_92CS];/*  in 0~100 index */
	s8	RxPower; /*  in dBm Translate from PWdB */
/*  Real power in dBm for this packet, no beautification and aggregation.
 * Keep this raw info to be used for the other procedures.
 */
	s8	recvpower;
	u8	BTRxRSSIPercentage;
	u8	SignalStrength; /*  in 0-100 index. */
	u8	RxPwr[MAX_PATH_NUM_92CS];/* per-path's pwdb */
	u8	RxSNR[MAX_PATH_NUM_92CS];/* per-path's SNR */
};

struct rx_pkt_attrib {
	u16	pkt_len;
	u8	physt;
	u8	drvinfo_sz;
	u8	shift_sz;
	u8	hdrlen; /* the WLAN Header Len */
	u8	to_fr_ds;
	u8	amsdu;
	u8	qos;
	u8	priority;
	u8	pw_save;
	u8	mdata;
	u16	seq_num;
	u8	frag_num;
	u8	mfrag;
	u8	order;
	u8	privacy; /* in frame_ctrl field */
	u8	bdecrypted;
	u8	encrypt; /* when 0 indicate no encrypt. when non-zero,
			  * indicate the encrypt algorithm
			  */
	u8	iv_len;
	u8	icv_len;
	u8	crc_err;
	u8	icv_err;

	u16 eth_type;

	u8	dst[ETH_ALEN];
	u8	src[ETH_ALEN];
	u8	ta[ETH_ALEN];
	u8	ra[ETH_ALEN];
	u8	bssid[ETH_ALEN];

	u8 ack_policy;

	u8	key_index;

	u8	mcs_rate;
	u8	rxht;
	u8	sgi;
	u8	pkt_rpt_type;
	u32	MacIDValidEntry[2];	/*  64 bits present 64 entry. */

	struct phy_info phy_info;
};

/* These definition is used for Rx packet reordering. */
#define SN_LESS(a, b)		(((a - b) & 0x800) != 0)
#define SN_EQUAL(a, b)	(a == b)
#define REORDER_WAIT_TIME	(50) /*  (ms) */

#define RXDESC_SIZE	24
#define RXDESC_OFFSET RXDESC_SIZE

struct recv_stat {
	__le32 rxdw0;
	__le32 rxdw1;
	__le32 rxdw2;
	__le32 rxdw3;
	__le32 rxdw4;
	__le32 rxdw5;
};

/*
 * accesser of recv_priv: rtw_recv_entry(dispatch / passive level);
 * recv_thread(passive) ; returnpkt(dispatch)
 * ; halt(passive) ;
 *
 * using enter_critical section to protect
 */
struct recv_priv {
	struct __queue free_recv_queue;
	struct __queue recv_pending_queue;
	struct __queue uc_swdec_pending_queue;
	void *pallocated_frame_buf;
	struct adapter	*adapter;
	u32	bIsAnyNonBEPkts;
	u64	rx_bytes;
	u64	rx_pkts;
	u64	rx_drop;
	u64	last_rx_bytes;

	struct tasklet_struct irq_prepare_beacon_tasklet;
	struct tasklet_struct recv_tasklet;
	struct sk_buff_head free_recv_skb_queue;
	struct sk_buff_head rx_skb_queue;
	struct recv_buf *precv_buf;    /*  4 alignment */
	struct __queue free_recv_buf_queue;
	/* For display the phy information */
	s8 rssi;
	s8 rxpwdb;
	u8 signal_strength;
	u8 signal_qual;
	u8 noise;
	s8 RxRssi[2];

	struct timer_list signal_stat_timer;
	u32 signal_stat_sampling_interval;
	struct signal_stat signal_qual_data;
	struct signal_stat signal_strength_data;
};

#define rtw_set_signal_stat_timer(recvpriv)			\
	mod_timer(&(recvpriv)->signal_stat_timer, jiffies +	\
		  msecs_to_jiffies((recvpriv)->signal_stat_sampling_interval))

struct sta_recv_priv {
	spinlock_t lock;
	int	option;
	struct __queue defrag_q; /* keeping the fragment frame until defrag */
	struct	stainfo_rxcache rxcache;
};

struct recv_buf {
	struct adapter *adapter;
	struct urb *purb;
	struct sk_buff *pskb;
	u8	reuse;
};

/*
 *	head  ----->
 *
 *		data  ----->
 *
 *			payload
 *
 *		tail  ----->
 *
 *
 *	end   ----->
 *
 *	len = (unsigned int )(tail - data);
 *
 */
struct recv_frame {
	struct list_head list;
	struct sk_buff	 *pkt;
	struct adapter  *adapter;
	struct rx_pkt_attrib attrib;
	struct sta_info *psta;
	/* for A-MPDU Rx reordering buffer control */
	struct recv_reorder_ctrl *preorder_ctrl;
};

struct recv_frame *_rtw_alloc_recvframe(struct __queue *pfree_recv_queue);
struct recv_frame *rtw_alloc_recvframe(struct __queue *pfree_recv_queue);
void rtw_init_recvframe(struct recv_frame *precvframe,
			struct recv_priv *precvpriv);
int  rtw_free_recvframe(struct recv_frame *precvframe,
			struct __queue *pfree_recv_queue);
#define rtw_dequeue_recvframe(queue) rtw_alloc_recvframe(queue)
int _rtw_enqueue_recvframe(struct recv_frame *precvframe,
			   struct __queue *queue);
int rtw_enqueue_recvframe(struct recv_frame *precvframe, struct __queue *queue);
void rtw_free_recvframe_queue(struct __queue *pframequeue,
			      struct __queue *pfree_recv_queue);
u32 rtw_free_uc_swdec_pending_queue(struct adapter *adapter);

void rtw_reordering_ctrl_timeout_handler(struct timer_list *t);

static inline s32 translate_percentage_to_dbm(u32 sig_stren_index)
{
	s32	power; /*  in dBm. */

	/*  Translate to dBm (x=0.5y-95). */
	power = (s32)((sig_stren_index + 1) >> 1);
	power -= 95;

	return power;
}

struct sta_info;

void _rtw_init_sta_recv_priv(struct sta_recv_priv *psta_recvpriv);

void  mgt_dispatcher(struct adapter *padapter, struct recv_frame *precv_frame);

#endif
