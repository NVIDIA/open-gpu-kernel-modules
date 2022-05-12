/* SPDX-License-Identifier: (GPL-2.0 OR MIT)
 * Google virtual Ethernet (gve) driver
 *
 * Copyright (C) 2015-2019 Google, Inc.
 */

#ifndef _GVE_H_
#define _GVE_H_

#include <linux/dma-mapping.h>
#include <linux/netdevice.h>
#include <linux/pci.h>
#include <linux/u64_stats_sync.h>
#include "gve_desc.h"

#ifndef PCI_VENDOR_ID_GOOGLE
#define PCI_VENDOR_ID_GOOGLE	0x1ae0
#endif

#define PCI_DEV_ID_GVNIC	0x0042

#define GVE_REGISTER_BAR	0
#define GVE_DOORBELL_BAR	2

/* Driver can alloc up to 2 segments for the header and 2 for the payload. */
#define GVE_TX_MAX_IOVEC	4
/* 1 for management, 1 for rx, 1 for tx */
#define GVE_MIN_MSIX 3

/* Numbers of gve tx/rx stats in stats report. */
#define GVE_TX_STATS_REPORT_NUM	5
#define GVE_RX_STATS_REPORT_NUM	2

/* Interval to schedule a stats report update, 20000ms. */
#define GVE_STATS_REPORT_TIMER_PERIOD	20000

/* Numbers of NIC tx/rx stats in stats report. */
#define NIC_TX_STATS_REPORT_NUM	0
#define NIC_RX_STATS_REPORT_NUM	4

#define GVE_DATA_SLOT_ADDR_PAGE_MASK (~(PAGE_SIZE - 1))

/* Each slot in the desc ring has a 1:1 mapping to a slot in the data ring */
struct gve_rx_desc_queue {
	struct gve_rx_desc *desc_ring; /* the descriptor ring */
	dma_addr_t bus; /* the bus for the desc_ring */
	u8 seqno; /* the next expected seqno for this desc*/
};

/* The page info for a single slot in the RX data queue */
struct gve_rx_slot_page_info {
	struct page *page;
	void *page_address;
	u8 page_offset; /* flipped to second half? */
	u8 can_flip;
};

/* A list of pages registered with the device during setup and used by a queue
 * as buffers
 */
struct gve_queue_page_list {
	u32 id; /* unique id */
	u32 num_entries;
	struct page **pages; /* list of num_entries pages */
	dma_addr_t *page_buses; /* the dma addrs of the pages */
};

/* Each slot in the data ring has a 1:1 mapping to a slot in the desc ring */
struct gve_rx_data_queue {
	union gve_rx_data_slot *data_ring; /* read by NIC */
	dma_addr_t data_bus; /* dma mapping of the slots */
	struct gve_rx_slot_page_info *page_info; /* page info of the buffers */
	struct gve_queue_page_list *qpl; /* qpl assigned to this queue */
	u8 raw_addressing; /* use raw_addressing? */
};

struct gve_priv;

/* An RX ring that contains a power-of-two sized desc and data ring. */
struct gve_rx_ring {
	struct gve_priv *gve;
	struct gve_rx_desc_queue desc;
	struct gve_rx_data_queue data;
	u64 rbytes; /* free-running bytes received */
	u64 rpackets; /* free-running packets received */
	u32 cnt; /* free-running total number of completed packets */
	u32 fill_cnt; /* free-running total number of descs and buffs posted */
	u32 mask; /* masks the cnt and fill_cnt to the size of the ring */
	u32 db_threshold; /* threshold for posting new buffs and descs */
	u64 rx_copybreak_pkt; /* free-running count of copybreak packets */
	u64 rx_copied_pkt; /* free-running total number of copied packets */
	u64 rx_skb_alloc_fail; /* free-running count of skb alloc fails */
	u64 rx_buf_alloc_fail; /* free-running count of buffer alloc fails */
	u64 rx_desc_err_dropped_pkt; /* free-running count of packets dropped by descriptor error */
	u32 q_num; /* queue index */
	u32 ntfy_id; /* notification block index */
	struct gve_queue_resources *q_resources; /* head and tail pointer idx */
	dma_addr_t q_resources_bus; /* dma address for the queue resources */
	struct u64_stats_sync statss; /* sync stats for 32bit archs */
};

/* A TX desc ring entry */
union gve_tx_desc {
	struct gve_tx_pkt_desc pkt; /* first desc for a packet */
	struct gve_tx_seg_desc seg; /* subsequent descs for a packet */
};

/* Tracks the memory in the fifo occupied by a segment of a packet */
struct gve_tx_iovec {
	u32 iov_offset; /* offset into this segment */
	u32 iov_len; /* length */
	u32 iov_padding; /* padding associated with this segment */
};

struct gve_tx_dma_buf {
	DEFINE_DMA_UNMAP_ADDR(dma);
	DEFINE_DMA_UNMAP_LEN(len);
};

/* Tracks the memory in the fifo occupied by the skb. Mapped 1:1 to a desc
 * ring entry but only used for a pkt_desc not a seg_desc
 */
struct gve_tx_buffer_state {
	struct sk_buff *skb; /* skb for this pkt */
	union {
		struct gve_tx_iovec iov[GVE_TX_MAX_IOVEC]; /* segments of this pkt */
		struct gve_tx_dma_buf buf;
	};
};

/* A TX buffer - each queue has one */
struct gve_tx_fifo {
	void *base; /* address of base of FIFO */
	u32 size; /* total size */
	atomic_t available; /* how much space is still available */
	u32 head; /* offset to write at */
	struct gve_queue_page_list *qpl; /* QPL mapped into this FIFO */
};

/* A TX ring that contains a power-of-two sized desc ring and a FIFO buffer */
struct gve_tx_ring {
	/* Cacheline 0 -- Accessed & dirtied during transmit */
	struct gve_tx_fifo tx_fifo;
	u32 req; /* driver tracked head pointer */
	u32 done; /* driver tracked tail pointer */

	/* Cacheline 1 -- Accessed & dirtied during gve_clean_tx_done */
	__be32 last_nic_done ____cacheline_aligned; /* NIC tail pointer */
	u64 pkt_done; /* free-running - total packets completed */
	u64 bytes_done; /* free-running - total bytes completed */
	u64 dropped_pkt; /* free-running - total packets dropped */
	u64 dma_mapping_error; /* count of dma mapping errors */

	/* Cacheline 2 -- Read-mostly fields */
	union gve_tx_desc *desc ____cacheline_aligned;
	struct gve_tx_buffer_state *info; /* Maps 1:1 to a desc */
	struct netdev_queue *netdev_txq;
	struct gve_queue_resources *q_resources; /* head and tail pointer idx */
	struct device *dev;
	u32 mask; /* masks req and done down to queue size */
	u8 raw_addressing; /* use raw_addressing? */

	/* Slow-path fields */
	u32 q_num ____cacheline_aligned; /* queue idx */
	u32 stop_queue; /* count of queue stops */
	u32 wake_queue; /* count of queue wakes */
	u32 ntfy_id; /* notification block index */
	dma_addr_t bus; /* dma address of the descr ring */
	dma_addr_t q_resources_bus; /* dma address of the queue resources */
	struct u64_stats_sync statss; /* sync stats for 32bit archs */
} ____cacheline_aligned;

/* Wraps the info for one irq including the napi struct and the queues
 * associated with that irq.
 */
struct gve_notify_block {
	__be32 irq_db_index; /* idx into Bar2 - set by device, must be 1st */
	char name[IFNAMSIZ + 16]; /* name registered with the kernel */
	struct napi_struct napi; /* kernel napi struct for this block */
	struct gve_priv *priv;
	struct gve_tx_ring *tx; /* tx rings on this block */
	struct gve_rx_ring *rx; /* rx rings on this block */
} ____cacheline_aligned;

/* Tracks allowed and current queue settings */
struct gve_queue_config {
	u16 max_queues;
	u16 num_queues; /* current */
};

/* Tracks the available and used qpl IDs */
struct gve_qpl_config {
	u32 qpl_map_size; /* map memory size */
	unsigned long *qpl_id_map; /* bitmap of used qpl ids */
};

struct gve_priv {
	struct net_device *dev;
	struct gve_tx_ring *tx; /* array of tx_cfg.num_queues */
	struct gve_rx_ring *rx; /* array of rx_cfg.num_queues */
	struct gve_queue_page_list *qpls; /* array of num qpls */
	struct gve_notify_block *ntfy_blocks; /* array of num_ntfy_blks */
	dma_addr_t ntfy_block_bus;
	struct msix_entry *msix_vectors; /* array of num_ntfy_blks + 1 */
	char mgmt_msix_name[IFNAMSIZ + 16];
	u32 mgmt_msix_idx;
	__be32 *counter_array; /* array of num_event_counters */
	dma_addr_t counter_array_bus;

	u16 num_event_counters;
	u16 tx_desc_cnt; /* num desc per ring */
	u16 rx_desc_cnt; /* num desc per ring */
	u16 tx_pages_per_qpl; /* tx buffer length */
	u16 rx_data_slot_cnt; /* rx buffer length */
	u64 max_registered_pages;
	u64 num_registered_pages; /* num pages registered with NIC */
	u32 rx_copybreak; /* copy packets smaller than this */
	u16 default_num_queues; /* default num queues to set up */
	u8 raw_addressing; /* 1 if this dev supports raw addressing, 0 otherwise */

	struct gve_queue_config tx_cfg;
	struct gve_queue_config rx_cfg;
	struct gve_qpl_config qpl_cfg; /* map used QPL ids */
	u32 num_ntfy_blks; /* spilt between TX and RX so must be even */

	struct gve_registers __iomem *reg_bar0; /* see gve_register.h */
	__be32 __iomem *db_bar2; /* "array" of doorbells */
	u32 msg_enable;	/* level for netif* netdev print macros	*/
	struct pci_dev *pdev;

	/* metrics */
	u32 tx_timeo_cnt;

	/* Admin queue - see gve_adminq.h*/
	union gve_adminq_command *adminq;
	dma_addr_t adminq_bus_addr;
	u32 adminq_mask; /* masks prod_cnt to adminq size */
	u32 adminq_prod_cnt; /* free-running count of AQ cmds executed */
	u32 adminq_cmd_fail; /* free-running count of AQ cmds failed */
	u32 adminq_timeouts; /* free-running count of AQ cmds timeouts */
	/* free-running count of per AQ cmd executed */
	u32 adminq_describe_device_cnt;
	u32 adminq_cfg_device_resources_cnt;
	u32 adminq_register_page_list_cnt;
	u32 adminq_unregister_page_list_cnt;
	u32 adminq_create_tx_queue_cnt;
	u32 adminq_create_rx_queue_cnt;
	u32 adminq_destroy_tx_queue_cnt;
	u32 adminq_destroy_rx_queue_cnt;
	u32 adminq_dcfg_device_resources_cnt;
	u32 adminq_set_driver_parameter_cnt;
	u32 adminq_report_stats_cnt;
	u32 adminq_report_link_speed_cnt;

	/* Global stats */
	u32 interface_up_cnt; /* count of times interface turned up since last reset */
	u32 interface_down_cnt; /* count of times interface turned down since last reset */
	u32 reset_cnt; /* count of reset */
	u32 page_alloc_fail; /* count of page alloc fails */
	u32 dma_mapping_error; /* count of dma mapping errors */
	u32 stats_report_trigger_cnt; /* count of device-requested stats-reports since last reset */
	struct workqueue_struct *gve_wq;
	struct work_struct service_task;
	struct work_struct stats_report_task;
	unsigned long service_task_flags;
	unsigned long state_flags;

	struct gve_stats_report *stats_report;
	u64 stats_report_len;
	dma_addr_t stats_report_bus; /* dma address for the stats report */
	unsigned long ethtool_flags;

	unsigned long stats_report_timer_period;
	struct timer_list stats_report_timer;

	/* Gvnic device link speed from hypervisor. */
	u64 link_speed;
};

enum gve_service_task_flags_bit {
	GVE_PRIV_FLAGS_DO_RESET			= 1,
	GVE_PRIV_FLAGS_RESET_IN_PROGRESS	= 2,
	GVE_PRIV_FLAGS_PROBE_IN_PROGRESS	= 3,
	GVE_PRIV_FLAGS_DO_REPORT_STATS = 4,
};

enum gve_state_flags_bit {
	GVE_PRIV_FLAGS_ADMIN_QUEUE_OK		= 1,
	GVE_PRIV_FLAGS_DEVICE_RESOURCES_OK	= 2,
	GVE_PRIV_FLAGS_DEVICE_RINGS_OK		= 3,
	GVE_PRIV_FLAGS_NAPI_ENABLED		= 4,
};

enum gve_ethtool_flags_bit {
	GVE_PRIV_FLAGS_REPORT_STATS		= 0,
};

static inline bool gve_get_do_reset(struct gve_priv *priv)
{
	return test_bit(GVE_PRIV_FLAGS_DO_RESET, &priv->service_task_flags);
}

static inline void gve_set_do_reset(struct gve_priv *priv)
{
	set_bit(GVE_PRIV_FLAGS_DO_RESET, &priv->service_task_flags);
}

static inline void gve_clear_do_reset(struct gve_priv *priv)
{
	clear_bit(GVE_PRIV_FLAGS_DO_RESET, &priv->service_task_flags);
}

static inline bool gve_get_reset_in_progress(struct gve_priv *priv)
{
	return test_bit(GVE_PRIV_FLAGS_RESET_IN_PROGRESS,
			&priv->service_task_flags);
}

static inline void gve_set_reset_in_progress(struct gve_priv *priv)
{
	set_bit(GVE_PRIV_FLAGS_RESET_IN_PROGRESS, &priv->service_task_flags);
}

static inline void gve_clear_reset_in_progress(struct gve_priv *priv)
{
	clear_bit(GVE_PRIV_FLAGS_RESET_IN_PROGRESS, &priv->service_task_flags);
}

static inline bool gve_get_probe_in_progress(struct gve_priv *priv)
{
	return test_bit(GVE_PRIV_FLAGS_PROBE_IN_PROGRESS,
			&priv->service_task_flags);
}

static inline void gve_set_probe_in_progress(struct gve_priv *priv)
{
	set_bit(GVE_PRIV_FLAGS_PROBE_IN_PROGRESS, &priv->service_task_flags);
}

static inline void gve_clear_probe_in_progress(struct gve_priv *priv)
{
	clear_bit(GVE_PRIV_FLAGS_PROBE_IN_PROGRESS, &priv->service_task_flags);
}

static inline bool gve_get_do_report_stats(struct gve_priv *priv)
{
	return test_bit(GVE_PRIV_FLAGS_DO_REPORT_STATS,
			&priv->service_task_flags);
}

static inline void gve_set_do_report_stats(struct gve_priv *priv)
{
	set_bit(GVE_PRIV_FLAGS_DO_REPORT_STATS, &priv->service_task_flags);
}

static inline void gve_clear_do_report_stats(struct gve_priv *priv)
{
	clear_bit(GVE_PRIV_FLAGS_DO_REPORT_STATS, &priv->service_task_flags);
}

static inline bool gve_get_admin_queue_ok(struct gve_priv *priv)
{
	return test_bit(GVE_PRIV_FLAGS_ADMIN_QUEUE_OK, &priv->state_flags);
}

static inline void gve_set_admin_queue_ok(struct gve_priv *priv)
{
	set_bit(GVE_PRIV_FLAGS_ADMIN_QUEUE_OK, &priv->state_flags);
}

static inline void gve_clear_admin_queue_ok(struct gve_priv *priv)
{
	clear_bit(GVE_PRIV_FLAGS_ADMIN_QUEUE_OK, &priv->state_flags);
}

static inline bool gve_get_device_resources_ok(struct gve_priv *priv)
{
	return test_bit(GVE_PRIV_FLAGS_DEVICE_RESOURCES_OK, &priv->state_flags);
}

static inline void gve_set_device_resources_ok(struct gve_priv *priv)
{
	set_bit(GVE_PRIV_FLAGS_DEVICE_RESOURCES_OK, &priv->state_flags);
}

static inline void gve_clear_device_resources_ok(struct gve_priv *priv)
{
	clear_bit(GVE_PRIV_FLAGS_DEVICE_RESOURCES_OK, &priv->state_flags);
}

static inline bool gve_get_device_rings_ok(struct gve_priv *priv)
{
	return test_bit(GVE_PRIV_FLAGS_DEVICE_RINGS_OK, &priv->state_flags);
}

static inline void gve_set_device_rings_ok(struct gve_priv *priv)
{
	set_bit(GVE_PRIV_FLAGS_DEVICE_RINGS_OK, &priv->state_flags);
}

static inline void gve_clear_device_rings_ok(struct gve_priv *priv)
{
	clear_bit(GVE_PRIV_FLAGS_DEVICE_RINGS_OK, &priv->state_flags);
}

static inline bool gve_get_napi_enabled(struct gve_priv *priv)
{
	return test_bit(GVE_PRIV_FLAGS_NAPI_ENABLED, &priv->state_flags);
}

static inline void gve_set_napi_enabled(struct gve_priv *priv)
{
	set_bit(GVE_PRIV_FLAGS_NAPI_ENABLED, &priv->state_flags);
}

static inline void gve_clear_napi_enabled(struct gve_priv *priv)
{
	clear_bit(GVE_PRIV_FLAGS_NAPI_ENABLED, &priv->state_flags);
}

static inline bool gve_get_report_stats(struct gve_priv *priv)
{
	return test_bit(GVE_PRIV_FLAGS_REPORT_STATS, &priv->ethtool_flags);
}

static inline void gve_clear_report_stats(struct gve_priv *priv)
{
	clear_bit(GVE_PRIV_FLAGS_REPORT_STATS, &priv->ethtool_flags);
}

/* Returns the address of the ntfy_blocks irq doorbell
 */
static inline __be32 __iomem *gve_irq_doorbell(struct gve_priv *priv,
					       struct gve_notify_block *block)
{
	return &priv->db_bar2[be32_to_cpu(block->irq_db_index)];
}

/* Returns the index into ntfy_blocks of the given tx ring's block
 */
static inline u32 gve_tx_idx_to_ntfy(struct gve_priv *priv, u32 queue_idx)
{
	return queue_idx;
}

/* Returns the index into ntfy_blocks of the given rx ring's block
 */
static inline u32 gve_rx_idx_to_ntfy(struct gve_priv *priv, u32 queue_idx)
{
	return (priv->num_ntfy_blks / 2) + queue_idx;
}

/* Returns the number of tx queue page lists
 */
static inline u32 gve_num_tx_qpls(struct gve_priv *priv)
{
	return priv->raw_addressing ? 0 : priv->tx_cfg.num_queues;
}

/* Returns the number of rx queue page lists
 */
static inline u32 gve_num_rx_qpls(struct gve_priv *priv)
{
	return priv->raw_addressing ? 0 : priv->rx_cfg.num_queues;
}

/* Returns a pointer to the next available tx qpl in the list of qpls
 */
static inline
struct gve_queue_page_list *gve_assign_tx_qpl(struct gve_priv *priv)
{
	int id = find_first_zero_bit(priv->qpl_cfg.qpl_id_map,
				     priv->qpl_cfg.qpl_map_size);

	/* we are out of tx qpls */
	if (id >= gve_num_tx_qpls(priv))
		return NULL;

	set_bit(id, priv->qpl_cfg.qpl_id_map);
	return &priv->qpls[id];
}

/* Returns a pointer to the next available rx qpl in the list of qpls
 */
static inline
struct gve_queue_page_list *gve_assign_rx_qpl(struct gve_priv *priv)
{
	int id = find_next_zero_bit(priv->qpl_cfg.qpl_id_map,
				    priv->qpl_cfg.qpl_map_size,
				    gve_num_tx_qpls(priv));

	/* we are out of rx qpls */
	if (id == priv->qpl_cfg.qpl_map_size)
		return NULL;

	set_bit(id, priv->qpl_cfg.qpl_id_map);
	return &priv->qpls[id];
}

/* Unassigns the qpl with the given id
 */
static inline void gve_unassign_qpl(struct gve_priv *priv, int id)
{
	clear_bit(id, priv->qpl_cfg.qpl_id_map);
}

/* Returns the correct dma direction for tx and rx qpls
 */
static inline enum dma_data_direction gve_qpl_dma_dir(struct gve_priv *priv,
						      int id)
{
	if (id < gve_num_tx_qpls(priv))
		return DMA_TO_DEVICE;
	else
		return DMA_FROM_DEVICE;
}

/* buffers */
int gve_alloc_page(struct gve_priv *priv, struct device *dev,
		   struct page **page, dma_addr_t *dma,
		   enum dma_data_direction);
void gve_free_page(struct device *dev, struct page *page, dma_addr_t dma,
		   enum dma_data_direction);
/* tx handling */
netdev_tx_t gve_tx(struct sk_buff *skb, struct net_device *dev);
bool gve_tx_poll(struct gve_notify_block *block, int budget);
int gve_tx_alloc_rings(struct gve_priv *priv);
void gve_tx_free_rings(struct gve_priv *priv);
__be32 gve_tx_load_event_counter(struct gve_priv *priv,
				 struct gve_tx_ring *tx);
/* rx handling */
void gve_rx_write_doorbell(struct gve_priv *priv, struct gve_rx_ring *rx);
bool gve_rx_poll(struct gve_notify_block *block, int budget);
int gve_rx_alloc_rings(struct gve_priv *priv);
void gve_rx_free_rings(struct gve_priv *priv);
bool gve_clean_rx_done(struct gve_rx_ring *rx, int budget,
		       netdev_features_t feat);
/* Reset */
void gve_schedule_reset(struct gve_priv *priv);
int gve_reset(struct gve_priv *priv, bool attempt_teardown);
int gve_adjust_queues(struct gve_priv *priv,
		      struct gve_queue_config new_rx_config,
		      struct gve_queue_config new_tx_config);
/* report stats handling */
void gve_handle_report_stats(struct gve_priv *priv);
/* exported by ethtool.c */
extern const struct ethtool_ops gve_ethtool_ops;
/* needed by ethtool */
extern const char gve_version_str[];
#endif /* _GVE_H_ */
