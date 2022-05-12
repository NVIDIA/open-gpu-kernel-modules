/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2015 Cavium, Inc.
 */

#ifndef NIC_H
#define	NIC_H

#include <linux/netdevice.h>
#include <linux/interrupt.h>
#include <linux/pci.h>
#include "thunder_bgx.h"

/* PCI device IDs */
#define	PCI_DEVICE_ID_THUNDER_NIC_PF		0xA01E
#define	PCI_DEVICE_ID_THUNDER_PASS1_NIC_VF	0x0011
#define	PCI_DEVICE_ID_THUNDER_NIC_VF		0xA034
#define	PCI_DEVICE_ID_THUNDER_BGX		0xA026

/* Subsystem device IDs */
#define PCI_SUBSYS_DEVID_88XX_NIC_PF		0xA11E
#define PCI_SUBSYS_DEVID_81XX_NIC_PF		0xA21E
#define PCI_SUBSYS_DEVID_83XX_NIC_PF		0xA31E

#define PCI_SUBSYS_DEVID_88XX_PASS1_NIC_VF	0xA11E
#define PCI_SUBSYS_DEVID_88XX_NIC_VF		0xA134
#define PCI_SUBSYS_DEVID_81XX_NIC_VF		0xA234
#define PCI_SUBSYS_DEVID_83XX_NIC_VF		0xA334


/* PCI BAR nos */
#define	PCI_CFG_REG_BAR_NUM		0
#define	PCI_MSIX_REG_BAR_NUM		4

/* NIC SRIOV VF count */
#define	MAX_NUM_VFS_SUPPORTED		128
#define	DEFAULT_NUM_VF_ENABLED		8

#define	NIC_TNS_BYPASS_MODE		0
#define	NIC_TNS_MODE			1

/* NIC priv flags */
#define	NIC_SRIOV_ENABLED		BIT(0)

/* Min/Max packet size */
#define	NIC_HW_MIN_FRS			64
#define	NIC_HW_MAX_FRS			9190 /* Excluding L2 header and FCS */

/* Max pkinds */
#define	NIC_MAX_PKIND			16

/* Max when CPI_ALG is IP diffserv */
#define	NIC_MAX_CPI_PER_LMAC		64

/* NIC VF Interrupts */
#define	NICVF_INTR_CQ			0
#define	NICVF_INTR_SQ			1
#define	NICVF_INTR_RBDR			2
#define	NICVF_INTR_PKT_DROP		3
#define	NICVF_INTR_TCP_TIMER		4
#define	NICVF_INTR_MBOX			5
#define	NICVF_INTR_QS_ERR		6

#define	NICVF_INTR_CQ_SHIFT		0
#define	NICVF_INTR_SQ_SHIFT		8
#define	NICVF_INTR_RBDR_SHIFT		16
#define	NICVF_INTR_PKT_DROP_SHIFT	20
#define	NICVF_INTR_TCP_TIMER_SHIFT	21
#define	NICVF_INTR_MBOX_SHIFT		22
#define	NICVF_INTR_QS_ERR_SHIFT		23

#define	NICVF_INTR_CQ_MASK		(0xFF << NICVF_INTR_CQ_SHIFT)
#define	NICVF_INTR_SQ_MASK		(0xFF << NICVF_INTR_SQ_SHIFT)
#define	NICVF_INTR_RBDR_MASK		(0x03 << NICVF_INTR_RBDR_SHIFT)
#define	NICVF_INTR_PKT_DROP_MASK	BIT(NICVF_INTR_PKT_DROP_SHIFT)
#define	NICVF_INTR_TCP_TIMER_MASK	BIT(NICVF_INTR_TCP_TIMER_SHIFT)
#define	NICVF_INTR_MBOX_MASK		BIT(NICVF_INTR_MBOX_SHIFT)
#define	NICVF_INTR_QS_ERR_MASK		BIT(NICVF_INTR_QS_ERR_SHIFT)

/* MSI-X interrupts */
#define	NIC_PF_MSIX_VECTORS		10
#define	NIC_VF_MSIX_VECTORS		20

#define NIC_PF_INTR_ID_ECC0_SBE		0
#define NIC_PF_INTR_ID_ECC0_DBE		1
#define NIC_PF_INTR_ID_ECC1_SBE		2
#define NIC_PF_INTR_ID_ECC1_DBE		3
#define NIC_PF_INTR_ID_ECC2_SBE		4
#define NIC_PF_INTR_ID_ECC2_DBE		5
#define NIC_PF_INTR_ID_ECC3_SBE		6
#define NIC_PF_INTR_ID_ECC3_DBE		7
#define NIC_PF_INTR_ID_MBOX0		8
#define NIC_PF_INTR_ID_MBOX1		9

/* Minimum FIFO level before all packets for the CQ are dropped
 *
 * This value ensures that once a packet has been "accepted"
 * for reception it will not get dropped due to non-availability
 * of CQ descriptor. An errata in HW mandates this value to be
 * atleast 0x100.
 */
#define NICPF_CQM_MIN_DROP_LEVEL       0x100

/* Global timer for CQ timer thresh interrupts
 * Calculated for SCLK of 700Mhz
 * value written should be a 1/16th of what is expected
 *
 * 1 tick per 0.025usec
 */
#define NICPF_CLK_PER_INT_TICK		1

/* Time to wait before we decide that a SQ is stuck.
 *
 * Since both pkt rx and tx notifications are done with same CQ,
 * when packets are being received at very high rate (eg: L2 forwarding)
 * then freeing transmitted skbs will be delayed and watchdog
 * will kick in, resetting interface. Hence keeping this value high.
 */
#define	NICVF_TX_TIMEOUT		(50 * HZ)

struct nicvf_cq_poll {
	struct  nicvf *nicvf;
	u8	cq_idx;		/* Completion queue index */
	struct	napi_struct napi;
};

#define NIC_MAX_RSS_HASH_BITS		8
#define NIC_MAX_RSS_IDR_TBL_SIZE	(1 << NIC_MAX_RSS_HASH_BITS)
#define RSS_HASH_KEY_SIZE		5 /* 320 bit key */

struct nicvf_rss_info {
	bool enable;
#define	RSS_L2_EXTENDED_HASH_ENA	BIT(0)
#define	RSS_IP_HASH_ENA			BIT(1)
#define	RSS_TCP_HASH_ENA		BIT(2)
#define	RSS_TCP_SYN_DIS			BIT(3)
#define	RSS_UDP_HASH_ENA		BIT(4)
#define RSS_L4_EXTENDED_HASH_ENA	BIT(5)
#define	RSS_ROCE_ENA			BIT(6)
#define	RSS_L3_BI_DIRECTION_ENA		BIT(7)
#define	RSS_L4_BI_DIRECTION_ENA		BIT(8)
	u64 cfg;
	u8  hash_bits;
	u16 rss_size;
	u8  ind_tbl[NIC_MAX_RSS_IDR_TBL_SIZE];
	u64 key[RSS_HASH_KEY_SIZE];
} ____cacheline_aligned_in_smp;

struct nicvf_pfc {
	u8    autoneg;
	u8    fc_rx;
	u8    fc_tx;
};

enum rx_stats_reg_offset {
	RX_OCTS = 0x0,
	RX_UCAST = 0x1,
	RX_BCAST = 0x2,
	RX_MCAST = 0x3,
	RX_RED = 0x4,
	RX_RED_OCTS = 0x5,
	RX_ORUN = 0x6,
	RX_ORUN_OCTS = 0x7,
	RX_FCS = 0x8,
	RX_L2ERR = 0x9,
	RX_DRP_BCAST = 0xa,
	RX_DRP_MCAST = 0xb,
	RX_DRP_L3BCAST = 0xc,
	RX_DRP_L3MCAST = 0xd,
	RX_STATS_ENUM_LAST,
};

enum tx_stats_reg_offset {
	TX_OCTS = 0x0,
	TX_UCAST = 0x1,
	TX_BCAST = 0x2,
	TX_MCAST = 0x3,
	TX_DROP = 0x4,
	TX_STATS_ENUM_LAST,
};

struct nicvf_hw_stats {
	u64 rx_bytes;
	u64 rx_frames;
	u64 rx_ucast_frames;
	u64 rx_bcast_frames;
	u64 rx_mcast_frames;
	u64 rx_drops;
	u64 rx_drop_red;
	u64 rx_drop_red_bytes;
	u64 rx_drop_overrun;
	u64 rx_drop_overrun_bytes;
	u64 rx_drop_bcast;
	u64 rx_drop_mcast;
	u64 rx_drop_l3_bcast;
	u64 rx_drop_l3_mcast;
	u64 rx_fcs_errors;
	u64 rx_l2_errors;

	u64 tx_bytes;
	u64 tx_frames;
	u64 tx_ucast_frames;
	u64 tx_bcast_frames;
	u64 tx_mcast_frames;
	u64 tx_drops;
};

struct nicvf_drv_stats {
	/* CQE Rx errs */
	u64 rx_bgx_truncated_pkts;
	u64 rx_jabber_errs;
	u64 rx_fcs_errs;
	u64 rx_bgx_errs;
	u64 rx_prel2_errs;
	u64 rx_l2_hdr_malformed;
	u64 rx_oversize;
	u64 rx_undersize;
	u64 rx_l2_len_mismatch;
	u64 rx_l2_pclp;
	u64 rx_ip_ver_errs;
	u64 rx_ip_csum_errs;
	u64 rx_ip_hdr_malformed;
	u64 rx_ip_payload_malformed;
	u64 rx_ip_ttl_errs;
	u64 rx_l3_pclp;
	u64 rx_l4_malformed;
	u64 rx_l4_csum_errs;
	u64 rx_udp_len_errs;
	u64 rx_l4_port_errs;
	u64 rx_tcp_flag_errs;
	u64 rx_tcp_offset_errs;
	u64 rx_l4_pclp;
	u64 rx_truncated_pkts;

	/* CQE Tx errs */
	u64 tx_desc_fault;
	u64 tx_hdr_cons_err;
	u64 tx_subdesc_err;
	u64 tx_max_size_exceeded;
	u64 tx_imm_size_oflow;
	u64 tx_data_seq_err;
	u64 tx_mem_seq_err;
	u64 tx_lock_viol;
	u64 tx_data_fault;
	u64 tx_tstmp_conflict;
	u64 tx_tstmp_timeout;
	u64 tx_mem_fault;
	u64 tx_csum_overlap;
	u64 tx_csum_overflow;

	/* driver debug stats */
	u64 tx_tso;
	u64 tx_timeout;
	u64 txq_stop;
	u64 txq_wake;

	u64 rcv_buffer_alloc_failures;
	u64 page_alloc;

	struct u64_stats_sync   syncp;
};

struct cavium_ptp;

struct xcast_addr_list {
	int              count;
	u64              mc[];
};

struct nicvf_work {
	struct work_struct     work;
	u8                     mode;
	struct xcast_addr_list *mc;
};

struct nicvf {
	struct nicvf		*pnicvf;
	struct net_device	*netdev;
	struct pci_dev		*pdev;
	void __iomem		*reg_base;
	struct bpf_prog         *xdp_prog;
#define	MAX_QUEUES_PER_QSET			8
	struct queue_set	*qs;
	void			*iommu_domain;
	u8			vf_id;
	u8			sqs_id;
	bool                    sqs_mode;
	bool			hw_tso;
	bool			t88;

	/* Receive buffer alloc */
	u32			rb_page_offset;
	u16			rb_pageref;
	bool			rb_alloc_fail;
	bool			rb_work_scheduled;
	struct page		*rb_page;
	struct delayed_work	rbdr_work;
	struct tasklet_struct	rbdr_task;

	/* Secondary Qset */
	u8			sqs_count;
#define	MAX_SQS_PER_VF_SINGLE_NODE		5
#define	MAX_SQS_PER_VF				11
	struct nicvf		*snicvf[MAX_SQS_PER_VF];

	/* Queue count */
	u8			rx_queues;
	u8			tx_queues;
	u8			xdp_tx_queues;
	u8			max_queues;

	u8			node;
	u8			cpi_alg;
	bool			link_up;
	u8			mac_type;
	u8			duplex;
	u32			speed;
	bool			tns_mode;
	bool			loopback_supported;
	struct nicvf_rss_info	rss_info;
	struct nicvf_pfc	pfc;
	struct tasklet_struct	qs_err_task;
	struct work_struct	reset_task;
	struct nicvf_work       rx_mode_work;
	/* spinlock to protect workqueue arguments from concurrent access */
	spinlock_t              rx_mode_wq_lock;
	/* workqueue for handling kernel ndo_set_rx_mode() calls */
	struct workqueue_struct *nicvf_rx_mode_wq;
	/* mutex to protect VF's mailbox contents from concurrent access */
	struct mutex            rx_mode_mtx;
	struct delayed_work	link_change_work;
	/* PTP timestamp */
	struct cavium_ptp	*ptp_clock;
	/* Inbound timestamping is on */
	bool			hw_rx_tstamp;
	/* When the packet that requires timestamping is sent, hardware inserts
	 * two entries to the completion queue.  First is the regular
	 * CQE_TYPE_SEND entry that signals that the packet was sent.
	 * The second is CQE_TYPE_SEND_PTP that contains the actual timestamp
	 * for that packet.
	 * `ptp_skb` is initialized in the handler for the CQE_TYPE_SEND
	 * entry and is used and zeroed in the handler for the CQE_TYPE_SEND_PTP
	 * entry.
	 * So `ptp_skb` is used to hold the pointer to the packet between
	 * the calls to CQE_TYPE_SEND and CQE_TYPE_SEND_PTP handlers.
	 */
	struct sk_buff		*ptp_skb;
	/* `tx_ptp_skbs` is set when the hardware is sending a packet that
	 * requires timestamping.  Cavium hardware can not process more than one
	 * such packet at once so this is set each time the driver submits
	 * a packet that requires timestamping to the send queue and clears
	 * each time it receives the entry on the completion queue saying
	 * that such packet was sent.
	 * So `tx_ptp_skbs` prevents driver from submitting more than one
	 * packet that requires timestamping to the hardware for transmitting.
	 */
	atomic_t		tx_ptp_skbs;

	/* Interrupt coalescing settings */
	u32			cq_coalesce_usecs;
	u32			msg_enable;

	/* Stats */
	struct nicvf_hw_stats   hw_stats;
	struct nicvf_drv_stats  __percpu *drv_stats;
	struct bgx_stats	bgx_stats;

	/* Napi */
	struct nicvf_cq_poll	*napi[8];

	/* MSI-X  */
	u8			num_vec;
	char			irq_name[NIC_VF_MSIX_VECTORS][IFNAMSIZ + 15];
	bool			irq_allocated[NIC_VF_MSIX_VECTORS];
	cpumask_var_t		affinity_mask[NIC_VF_MSIX_VECTORS];

	/* VF <-> PF mailbox communication */
	bool			pf_acked;
	bool			pf_nacked;
	bool			set_mac_pending;
} ____cacheline_aligned_in_smp;

/* PF <--> VF Mailbox communication
 * Eight 64bit registers are shared between PF and VF.
 * Separate set for each VF.
 * Writing '1' into last register mbx7 means end of message.
 */

/* PF <--> VF mailbox communication */
#define	NIC_PF_VF_MAILBOX_SIZE		2
#define	NIC_MBOX_MSG_TIMEOUT		2000 /* ms */

/* Mailbox message types */
#define	NIC_MBOX_MSG_READY		0x01	/* Is PF ready to rcv msgs */
#define	NIC_MBOX_MSG_ACK		0x02	/* ACK the message received */
#define	NIC_MBOX_MSG_NACK		0x03	/* NACK the message received */
#define	NIC_MBOX_MSG_QS_CFG		0x04	/* Configure Qset */
#define	NIC_MBOX_MSG_RQ_CFG		0x05	/* Configure receive queue */
#define	NIC_MBOX_MSG_SQ_CFG		0x06	/* Configure Send queue */
#define	NIC_MBOX_MSG_RQ_DROP_CFG	0x07	/* Configure receive queue */
#define	NIC_MBOX_MSG_SET_MAC		0x08	/* Add MAC ID to DMAC filter */
#define	NIC_MBOX_MSG_SET_MAX_FRS	0x09	/* Set max frame size */
#define	NIC_MBOX_MSG_CPI_CFG		0x0A	/* Config CPI, RSSI */
#define	NIC_MBOX_MSG_RSS_SIZE		0x0B	/* Get RSS indir_tbl size */
#define	NIC_MBOX_MSG_RSS_CFG		0x0C	/* Config RSS table */
#define	NIC_MBOX_MSG_RSS_CFG_CONT	0x0D	/* RSS config continuation */
#define	NIC_MBOX_MSG_RQ_BP_CFG		0x0E	/* RQ backpressure config */
#define	NIC_MBOX_MSG_RQ_SW_SYNC		0x0F	/* Flush inflight pkts to RQ */
#define	NIC_MBOX_MSG_BGX_STATS		0x10	/* Get stats from BGX */
#define	NIC_MBOX_MSG_BGX_LINK_CHANGE	0x11	/* BGX:LMAC link status */
#define	NIC_MBOX_MSG_ALLOC_SQS		0x12	/* Allocate secondary Qset */
#define	NIC_MBOX_MSG_NICVF_PTR		0x13	/* Send nicvf ptr to PF */
#define	NIC_MBOX_MSG_PNICVF_PTR		0x14	/* Get primary qset nicvf ptr */
#define	NIC_MBOX_MSG_SNICVF_PTR		0x15	/* Send sqet nicvf ptr to PVF */
#define	NIC_MBOX_MSG_LOOPBACK		0x16	/* Set interface in loopback */
#define	NIC_MBOX_MSG_RESET_STAT_COUNTER 0x17	/* Reset statistics counters */
#define	NIC_MBOX_MSG_PFC		0x18	/* Pause frame control */
#define	NIC_MBOX_MSG_PTP_CFG		0x19	/* HW packet timestamp */
#define	NIC_MBOX_MSG_CFG_DONE		0xF0	/* VF configuration done */
#define	NIC_MBOX_MSG_SHUTDOWN		0xF1	/* VF is being shutdown */
#define	NIC_MBOX_MSG_RESET_XCAST	0xF2    /* Reset DCAM filtering mode */
#define	NIC_MBOX_MSG_ADD_MCAST		0xF3    /* Add MAC to DCAM filters */
#define	NIC_MBOX_MSG_SET_XCAST		0xF4    /* Set MCAST/BCAST RX mode */

struct nic_cfg_msg {
	u8    msg;
	u8    vf_id;
	u8    node_id;
	u8    tns_mode:1;
	u8    sqs_mode:1;
	u8    loopback_supported:1;
	u8    mac_addr[ETH_ALEN];
};

/* Qset configuration */
struct qs_cfg_msg {
	u8    msg;
	u8    num;
	u8    sqs_count;
	u64   cfg;
};

/* Receive queue configuration */
struct rq_cfg_msg {
	u8    msg;
	u8    qs_num;
	u8    rq_num;
	u64   cfg;
};

/* Send queue configuration */
struct sq_cfg_msg {
	u8    msg;
	u8    qs_num;
	u8    sq_num;
	bool  sqs_mode;
	u64   cfg;
};

/* Set VF's MAC address */
struct set_mac_msg {
	u8    msg;
	u8    vf_id;
	u8    mac_addr[ETH_ALEN];
};

/* Set Maximum frame size */
struct set_frs_msg {
	u8    msg;
	u8    vf_id;
	u16   max_frs;
};

/* Set CPI algorithm type */
struct cpi_cfg_msg {
	u8    msg;
	u8    vf_id;
	u8    rq_cnt;
	u8    cpi_alg;
};

/* Get RSS table size */
struct rss_sz_msg {
	u8    msg;
	u8    vf_id;
	u16   ind_tbl_size;
};

/* Set RSS configuration */
struct rss_cfg_msg {
	u8    msg;
	u8    vf_id;
	u8    hash_bits;
	u8    tbl_len;
	u8    tbl_offset;
#define RSS_IND_TBL_LEN_PER_MBX_MSG	8
	u8    ind_tbl[RSS_IND_TBL_LEN_PER_MBX_MSG];
};

struct bgx_stats_msg {
	u8    msg;
	u8    vf_id;
	u8    rx;
	u8    idx;
	u64   stats;
};

/* Physical interface link status */
struct bgx_link_status {
	u8    msg;
	u8    mac_type;
	u8    link_up;
	u8    duplex;
	u32   speed;
};

/* Get Extra Qset IDs */
struct sqs_alloc {
	u8    msg;
	u8    vf_id;
	u8    qs_count;
};

struct nicvf_ptr {
	u8    msg;
	u8    vf_id;
	bool  sqs_mode;
	u8    sqs_id;
	u64   nicvf;
};

/* Set interface in loopback mode */
struct set_loopback {
	u8    msg;
	u8    vf_id;
	bool  enable;
};

/* Reset statistics counters */
struct reset_stat_cfg {
	u8    msg;
	/* Bitmap to select NIC_PF_VNIC(vf_id)_RX_STAT(0..13) */
	u16   rx_stat_mask;
	/* Bitmap to select NIC_PF_VNIC(vf_id)_TX_STAT(0..4) */
	u8    tx_stat_mask;
	/* Bitmap to select NIC_PF_QS(0..127)_RQ(0..7)_STAT(0..1)
	 * bit14, bit15 NIC_PF_QS(vf_id)_RQ7_STAT(0..1)
	 * bit12, bit13 NIC_PF_QS(vf_id)_RQ6_STAT(0..1)
	 * ..
	 * bit2, bit3 NIC_PF_QS(vf_id)_RQ1_STAT(0..1)
	 * bit0, bit1 NIC_PF_QS(vf_id)_RQ0_STAT(0..1)
	 */
	u16   rq_stat_mask;
	/* Bitmap to select NIC_PF_QS(0..127)_SQ(0..7)_STAT(0..1)
	 * bit14, bit15 NIC_PF_QS(vf_id)_SQ7_STAT(0..1)
	 * bit12, bit13 NIC_PF_QS(vf_id)_SQ6_STAT(0..1)
	 * ..
	 * bit2, bit3 NIC_PF_QS(vf_id)_SQ1_STAT(0..1)
	 * bit0, bit1 NIC_PF_QS(vf_id)_SQ0_STAT(0..1)
	 */
	u16   sq_stat_mask;
};

struct pfc {
	u8    msg;
	u8    get; /* Get or set PFC settings */
	u8    autoneg;
	u8    fc_rx;
	u8    fc_tx;
};

struct set_ptp {
	u8    msg;
	bool  enable;
};

struct xcast {
	u8    msg;
	u8    mode;
	u64   mac:48;
};

/* 128 bit shared memory between PF and each VF */
union nic_mbx {
	struct { u8 msg; }	msg;
	struct nic_cfg_msg	nic_cfg;
	struct qs_cfg_msg	qs;
	struct rq_cfg_msg	rq;
	struct sq_cfg_msg	sq;
	struct set_mac_msg	mac;
	struct set_frs_msg	frs;
	struct cpi_cfg_msg	cpi_cfg;
	struct rss_sz_msg	rss_size;
	struct rss_cfg_msg	rss_cfg;
	struct bgx_stats_msg    bgx_stats;
	struct bgx_link_status  link_status;
	struct sqs_alloc        sqs_alloc;
	struct nicvf_ptr	nicvf;
	struct set_loopback	lbk;
	struct reset_stat_cfg	reset_stat;
	struct pfc		pfc;
	struct set_ptp		ptp;
	struct xcast            xcast;
};

#define NIC_NODE_ID_MASK	0x03
#define NIC_NODE_ID_SHIFT	44

static inline int nic_get_node_id(struct pci_dev *pdev)
{
	u64 addr = pci_resource_start(pdev, PCI_CFG_REG_BAR_NUM);
	return ((addr >> NIC_NODE_ID_SHIFT) & NIC_NODE_ID_MASK);
}

static inline bool pass1_silicon(struct pci_dev *pdev)
{
	return (pdev->revision < 8) &&
		(pdev->subsystem_device == PCI_SUBSYS_DEVID_88XX_NIC_PF);
}

static inline bool pass2_silicon(struct pci_dev *pdev)
{
	return (pdev->revision >= 8) &&
		(pdev->subsystem_device == PCI_SUBSYS_DEVID_88XX_NIC_PF);
}

int nicvf_set_real_num_queues(struct net_device *netdev,
			      int tx_queues, int rx_queues);
int nicvf_open(struct net_device *netdev);
int nicvf_stop(struct net_device *netdev);
int nicvf_send_msg_to_pf(struct nicvf *vf, union nic_mbx *mbx);
void nicvf_config_rss(struct nicvf *nic);
void nicvf_set_rss_key(struct nicvf *nic);
void nicvf_set_ethtool_ops(struct net_device *netdev);
void nicvf_update_stats(struct nicvf *nic);
void nicvf_update_lmac_stats(struct nicvf *nic);

#endif /* NIC_H */
