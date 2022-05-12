/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Linux network driver for QLogic BR-series Converged Network Adapter.
 */
/*
 * Copyright (c) 2005-2014 Brocade Communications Systems, Inc.
 * Copyright (c) 2014-2015 QLogic Corporation
 * All rights reserved
 * www.qlogic.com
 */
#ifndef __BNAD_H__
#define __BNAD_H__

#include <linux/rtnetlink.h>
#include <linux/workqueue.h>
#include <linux/ipv6.h>
#include <linux/etherdevice.h>
#include <linux/mutex.h>
#include <linux/firmware.h>
#include <linux/if_vlan.h>

/* Fix for IA64 */
#include <asm/checksum.h>
#include <net/ip6_checksum.h>

#include <net/ip.h>
#include <net/tcp.h>

#include "bna.h"

#define BNAD_TXQ_DEPTH		2048
#define BNAD_RXQ_DEPTH		2048

#define BNAD_MAX_TX		1
#define BNAD_MAX_TXQ_PER_TX	8	/* 8 priority queues */
#define BNAD_TXQ_NUM		1

#define BNAD_MAX_RX		1
#define BNAD_MAX_RXP_PER_RX	16
#define BNAD_MAX_RXQ_PER_RXP	2

/*
 * Control structure pointed to ccb->ctrl, which
 * determines the NAPI / LRO behavior CCB
 * There is 1:1 corres. between ccb & ctrl
 */
struct bnad_rx_ctrl {
	struct bna_ccb *ccb;
	struct bnad *bnad;
	unsigned long  flags;
	struct napi_struct	napi;
	u64		rx_intr_ctr;
	u64		rx_poll_ctr;
	u64		rx_schedule;
	u64		rx_keep_poll;
	u64		rx_complete;
};

#define BNAD_RXMODE_PROMISC_DEFAULT	BNA_RXMODE_PROMISC

/*
 * GLOBAL #defines (CONSTANTS)
 */
#define BNAD_NAME			"bna"
#define BNAD_NAME_LEN			64

#define BNAD_MAILBOX_MSIX_INDEX		0
#define BNAD_MAILBOX_MSIX_VECTORS	1
#define BNAD_INTX_TX_IB_BITMASK		0x1
#define BNAD_INTX_RX_IB_BITMASK		0x2

#define BNAD_STATS_TIMER_FREQ		1000	/* in msecs */
#define BNAD_DIM_TIMER_FREQ		1000	/* in msecs */

#define BNAD_IOCETH_TIMEOUT	     10000

#define BNAD_MIN_Q_DEPTH		512
#define BNAD_MAX_RXQ_DEPTH		16384
#define BNAD_MAX_TXQ_DEPTH		2048

#define BNAD_JUMBO_MTU			9000

#define BNAD_NETIF_WAKE_THRESHOLD	8

#define BNAD_RXQ_REFILL_THRESHOLD_SHIFT	3

/* Bit positions for tcb->flags */
#define BNAD_TXQ_FREE_SENT		0
#define BNAD_TXQ_TX_STARTED		1

/* Bit positions for rcb->flags */
#define BNAD_RXQ_STARTED		0
#define BNAD_RXQ_POST_OK		1

/* Resource limits */
#define BNAD_NUM_TXQ			(bnad->num_tx * bnad->num_txq_per_tx)
#define BNAD_NUM_RXP			(bnad->num_rx * bnad->num_rxp_per_rx)

#define BNAD_FRAME_SIZE(_mtu) \
	(ETH_HLEN + VLAN_HLEN + (_mtu) + ETH_FCS_LEN)

/*
 * DATA STRUCTURES
 */

/* enums */
enum bnad_intr_source {
	BNAD_INTR_TX		= 1,
	BNAD_INTR_RX		= 2
};

enum bnad_link_state {
	BNAD_LS_DOWN		= 0,
	BNAD_LS_UP		= 1
};

struct bnad_iocmd_comp {
	struct bnad		*bnad;
	struct completion	comp;
	int			comp_status;
};

struct bnad_completion {
	struct completion	ioc_comp;
	struct completion	ucast_comp;
	struct completion	mcast_comp;
	struct completion	tx_comp;
	struct completion	rx_comp;
	struct completion	stats_comp;
	struct completion	enet_comp;
	struct completion	mtu_comp;

	u8			ioc_comp_status;
	u8			ucast_comp_status;
	u8			mcast_comp_status;
	u8			tx_comp_status;
	u8			rx_comp_status;
	u8			stats_comp_status;
	u8			port_comp_status;
	u8			mtu_comp_status;
};

/* Tx Rx Control Stats */
struct bnad_drv_stats {
	u64		netif_queue_stop;
	u64		netif_queue_wakeup;
	u64		netif_queue_stopped;
	u64		tso4;
	u64		tso6;
	u64		tso_err;
	u64		tcpcsum_offload;
	u64		udpcsum_offload;
	u64		csum_help;
	u64		tx_skb_too_short;
	u64		tx_skb_stopping;
	u64		tx_skb_max_vectors;
	u64		tx_skb_mss_too_long;
	u64		tx_skb_tso_too_short;
	u64		tx_skb_tso_prepare;
	u64		tx_skb_non_tso_too_long;
	u64		tx_skb_tcp_hdr;
	u64		tx_skb_udp_hdr;
	u64		tx_skb_csum_err;
	u64		tx_skb_headlen_too_long;
	u64		tx_skb_headlen_zero;
	u64		tx_skb_frag_zero;
	u64		tx_skb_len_mismatch;
	u64		tx_skb_map_failed;

	u64		hw_stats_updates;
	u64		netif_rx_dropped;

	u64		link_toggle;
	u64		cee_toggle;

	u64		rxp_info_alloc_failed;
	u64		mbox_intr_disabled;
	u64		mbox_intr_enabled;
	u64		tx_unmap_q_alloc_failed;
	u64		rx_unmap_q_alloc_failed;

	u64		rxbuf_alloc_failed;
	u64		rxbuf_map_failed;
};

/* Complete driver stats */
struct bnad_stats {
	struct bnad_drv_stats drv_stats;
	struct bna_stats *bna_stats;
};

/* Tx / Rx Resources */
struct bnad_tx_res_info {
	struct bna_res_info res_info[BNA_TX_RES_T_MAX];
};

struct bnad_rx_res_info {
	struct bna_res_info res_info[BNA_RX_RES_T_MAX];
};

struct bnad_tx_info {
	struct bna_tx *tx; /* 1:1 between tx_info & tx */
	struct bna_tcb *tcb[BNAD_MAX_TXQ_PER_TX];
	u32 tx_id;
	struct delayed_work tx_cleanup_work;
} ____cacheline_aligned;

struct bnad_rx_info {
	struct bna_rx *rx; /* 1:1 between rx_info & rx */

	struct bnad_rx_ctrl rx_ctrl[BNAD_MAX_RXP_PER_RX];
	u32 rx_id;
	struct work_struct rx_cleanup_work;
} ____cacheline_aligned;

struct bnad_tx_vector {
	DEFINE_DMA_UNMAP_ADDR(dma_addr);
	DEFINE_DMA_UNMAP_LEN(dma_len);
};

struct bnad_tx_unmap {
	struct sk_buff		*skb;
	u32			nvecs;
	struct bnad_tx_vector	vectors[BFI_TX_MAX_VECTORS_PER_WI];
};

struct bnad_rx_vector {
	DEFINE_DMA_UNMAP_ADDR(dma_addr);
	u32			len;
};

struct bnad_rx_unmap {
	struct page		*page;
	struct sk_buff		*skb;
	struct bnad_rx_vector	vector;
	u32			page_offset;
};

enum bnad_rxbuf_type {
	BNAD_RXBUF_NONE		= 0,
	BNAD_RXBUF_SK_BUFF	= 1,
	BNAD_RXBUF_PAGE		= 2,
	BNAD_RXBUF_MULTI_BUFF	= 3
};

#define BNAD_RXBUF_IS_SK_BUFF(_type)	((_type) == BNAD_RXBUF_SK_BUFF)
#define BNAD_RXBUF_IS_MULTI_BUFF(_type)	((_type) == BNAD_RXBUF_MULTI_BUFF)

struct bnad_rx_unmap_q {
	int			reuse_pi;
	int			alloc_order;
	u32			map_size;
	enum bnad_rxbuf_type	type;
	struct bnad_rx_unmap	unmap[] ____cacheline_aligned;
};

#define BNAD_PCI_DEV_IS_CAT2(_bnad) \
	((_bnad)->pcidev->device == BFA_PCI_DEVICE_ID_CT2)

/* Bit mask values for bnad->cfg_flags */
#define	BNAD_CF_DIM_ENABLED		0x01	/* DIM */
#define	BNAD_CF_PROMISC			0x02
#define BNAD_CF_ALLMULTI		0x04
#define	BNAD_CF_DEFAULT			0x08
#define	BNAD_CF_MSIX			0x10	/* If in MSIx mode */

/* Defines for run_flags bit-mask */
/* Set, tested & cleared using xxx_bit() functions */
/* Values indicated bit positions */
#define BNAD_RF_CEE_RUNNING		0
#define BNAD_RF_MTU_SET		1
#define BNAD_RF_MBOX_IRQ_DISABLED	2
#define BNAD_RF_NETDEV_REGISTERED	3
#define BNAD_RF_DIM_TIMER_RUNNING	4
#define BNAD_RF_STATS_TIMER_RUNNING	5
#define BNAD_RF_TX_PRIO_SET		6

struct bnad {
	struct net_device	*netdev;
	u32			id;

	/* Data path */
	struct bnad_tx_info tx_info[BNAD_MAX_TX];
	struct bnad_rx_info rx_info[BNAD_MAX_RX];

	unsigned long active_vlans[BITS_TO_LONGS(VLAN_N_VID)];
	/*
	 * These q numbers are global only because
	 * they are used to calculate MSIx vectors.
	 * Actually the exact # of queues are per Tx/Rx
	 * object.
	 */
	u32		num_tx;
	u32		num_rx;
	u32		num_txq_per_tx;
	u32		num_rxp_per_rx;

	u32		txq_depth;
	u32		rxq_depth;

	u8			tx_coalescing_timeo;
	u8			rx_coalescing_timeo;

	struct bna_rx_config rx_config[BNAD_MAX_RX] ____cacheline_aligned;
	struct bna_tx_config tx_config[BNAD_MAX_TX] ____cacheline_aligned;

	void __iomem		*bar0;	/* BAR0 address */

	struct bna bna;

	u32		cfg_flags;
	unsigned long		run_flags;

	struct pci_dev		*pcidev;
	u64		mmio_start;
	u64		mmio_len;

	u32		msix_num;
	struct msix_entry	*msix_table;

	struct mutex		conf_mutex;
	spinlock_t		bna_lock ____cacheline_aligned;

	/* Timers */
	struct timer_list	ioc_timer;
	struct timer_list	dim_timer;
	struct timer_list	stats_timer;

	/* Control path resources, memory & irq */
	struct bna_res_info res_info[BNA_RES_T_MAX];
	struct bna_res_info mod_res_info[BNA_MOD_RES_T_MAX];
	struct bnad_tx_res_info tx_res_info[BNAD_MAX_TX];
	struct bnad_rx_res_info rx_res_info[BNAD_MAX_RX];

	struct bnad_completion bnad_completions;

	/* Burnt in MAC address */
	u8			perm_addr[ETH_ALEN];

	struct workqueue_struct *work_q;

	/* Statistics */
	struct bnad_stats stats;

	struct bnad_diag *diag;

	char			adapter_name[BNAD_NAME_LEN];
	char			port_name[BNAD_NAME_LEN];
	char			mbox_irq_name[BNAD_NAME_LEN];
	char			wq_name[BNAD_NAME_LEN];

	/* debugfs specific data */
	char	*regdata;
	u32	reglen;
	struct dentry *bnad_dentry_files[5];
	struct dentry *port_debugfs_root;
};

struct bnad_drvinfo {
	struct bfa_ioc_attr  ioc_attr;
	struct bfa_cee_attr  cee_attr;
	struct bfa_flash_attr flash_attr;
	u32	cee_status;
	u32	flash_status;
};

/*
 * EXTERN VARIABLES
 */
extern const struct firmware *bfi_fw;

/*
 * EXTERN PROTOTYPES
 */
u32 *cna_get_firmware_buf(struct pci_dev *pdev);
/* Netdev entry point prototypes */
void bnad_set_rx_mode(struct net_device *netdev);
struct net_device_stats *bnad_get_netdev_stats(struct net_device *netdev);
int bnad_mac_addr_set_locked(struct bnad *bnad, const u8 *mac_addr);
int bnad_enable_default_bcast(struct bnad *bnad);
void bnad_restore_vlans(struct bnad *bnad, u32 rx_id);
void bnad_set_ethtool_ops(struct net_device *netdev);
void bnad_cb_completion(void *arg, enum bfa_status status);

/* Configuration & setup */
void bnad_tx_coalescing_timeo_set(struct bnad *bnad);
void bnad_rx_coalescing_timeo_set(struct bnad *bnad);

int bnad_setup_rx(struct bnad *bnad, u32 rx_id);
int bnad_setup_tx(struct bnad *bnad, u32 tx_id);
void bnad_destroy_tx(struct bnad *bnad, u32 tx_id);
void bnad_destroy_rx(struct bnad *bnad, u32 rx_id);

/* Timer start/stop protos */
void bnad_dim_timer_start(struct bnad *bnad);

/* Statistics */
void bnad_netdev_qstats_fill(struct bnad *bnad,
			     struct rtnl_link_stats64 *stats);
void bnad_netdev_hwstats_fill(struct bnad *bnad,
			      struct rtnl_link_stats64 *stats);

/* Debugfs */
void bnad_debugfs_init(struct bnad *bnad);
void bnad_debugfs_uninit(struct bnad *bnad);

/* MACROS */
/* To set & get the stats counters */
#define BNAD_UPDATE_CTR(_bnad, _ctr)				\
				(((_bnad)->stats.drv_stats._ctr)++)

#define BNAD_GET_CTR(_bnad, _ctr) ((_bnad)->stats.drv_stats._ctr)

#define bnad_enable_rx_irq_unsafe(_ccb)			\
{							\
	if (likely(test_bit(BNAD_RXQ_STARTED, &(_ccb)->rcb[0]->flags))) {\
		bna_ib_coalescing_timer_set((_ccb)->i_dbell,	\
			(_ccb)->rx_coalescing_timeo);		\
		bna_ib_ack((_ccb)->i_dbell, 0);			\
	}							\
}

#endif /* __BNAD_H__ */
