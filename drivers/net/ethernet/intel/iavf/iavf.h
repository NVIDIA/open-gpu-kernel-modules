/* SPDX-License-Identifier: GPL-2.0 */
/* Copyright(c) 2013 - 2018 Intel Corporation. */

#ifndef _IAVF_H_
#define _IAVF_H_

#include <linux/module.h>
#include <linux/pci.h>
#include <linux/aer.h>
#include <linux/netdevice.h>
#include <linux/vmalloc.h>
#include <linux/interrupt.h>
#include <linux/ethtool.h>
#include <linux/if_vlan.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/sctp.h>
#include <linux/ipv6.h>
#include <linux/kernel.h>
#include <linux/bitops.h>
#include <linux/timer.h>
#include <linux/workqueue.h>
#include <linux/wait.h>
#include <linux/delay.h>
#include <linux/gfp.h>
#include <linux/skbuff.h>
#include <linux/dma-mapping.h>
#include <linux/etherdevice.h>
#include <linux/socket.h>
#include <linux/jiffies.h>
#include <net/ip6_checksum.h>
#include <net/pkt_cls.h>
#include <net/udp.h>
#include <net/tc_act/tc_gact.h>
#include <net/tc_act/tc_mirred.h>

#include "iavf_type.h"
#include <linux/avf/virtchnl.h>
#include "iavf_txrx.h"
#include "iavf_fdir.h"
#include "iavf_adv_rss.h"

#define DEFAULT_DEBUG_LEVEL_SHIFT 3
#define PFX "iavf: "

/* VSI state flags shared with common code */
enum iavf_vsi_state_t {
	__IAVF_VSI_DOWN,
	/* This must be last as it determines the size of the BITMAP */
	__IAVF_VSI_STATE_SIZE__,
};

/* dummy struct to make common code less painful */
struct iavf_vsi {
	struct iavf_adapter *back;
	struct net_device *netdev;
	unsigned long active_vlans[BITS_TO_LONGS(VLAN_N_VID)];
	u16 seid;
	u16 id;
	DECLARE_BITMAP(state, __IAVF_VSI_STATE_SIZE__);
	int base_vector;
	u16 work_limit;
	u16 qs_handle;
	void *priv;     /* client driver data reference. */
};

/* How many Rx Buffers do we bundle into one write to the hardware ? */
#define IAVF_RX_BUFFER_WRITE	16	/* Must be power of 2 */
#define IAVF_DEFAULT_TXD	512
#define IAVF_DEFAULT_RXD	512
#define IAVF_MAX_TXD		4096
#define IAVF_MIN_TXD		64
#define IAVF_MAX_RXD		4096
#define IAVF_MIN_RXD		64
#define IAVF_REQ_DESCRIPTOR_MULTIPLE	32
#define IAVF_MAX_AQ_BUF_SIZE	4096
#define IAVF_AQ_LEN		32
#define IAVF_AQ_MAX_ERR	20 /* times to try before resetting AQ */

#define MAXIMUM_ETHERNET_VLAN_SIZE (VLAN_ETH_FRAME_LEN + ETH_FCS_LEN)

#define IAVF_RX_DESC(R, i) (&(((union iavf_32byte_rx_desc *)((R)->desc))[i]))
#define IAVF_TX_DESC(R, i) (&(((struct iavf_tx_desc *)((R)->desc))[i]))
#define IAVF_TX_CTXTDESC(R, i) \
	(&(((struct iavf_tx_context_desc *)((R)->desc))[i]))
#define IAVF_MAX_REQ_QUEUES 16

#define IAVF_HKEY_ARRAY_SIZE ((IAVF_VFQF_HKEY_MAX_INDEX + 1) * 4)
#define IAVF_HLUT_ARRAY_SIZE ((IAVF_VFQF_HLUT_MAX_INDEX + 1) * 4)
#define IAVF_MBPS_DIVISOR	125000 /* divisor to convert to Mbps */

#define IAVF_VIRTCHNL_VF_RESOURCE_SIZE (sizeof(struct virtchnl_vf_resource) + \
					(IAVF_MAX_VF_VSI * \
					 sizeof(struct virtchnl_vsi_resource)))

/* MAX_MSIX_Q_VECTORS of these are allocated,
 * but we only use one per queue-specific vector.
 */
struct iavf_q_vector {
	struct iavf_adapter *adapter;
	struct iavf_vsi *vsi;
	struct napi_struct napi;
	struct iavf_ring_container rx;
	struct iavf_ring_container tx;
	u32 ring_mask;
	u8 itr_countdown;	/* when 0 should adjust adaptive ITR */
	u8 num_ringpairs;	/* total number of ring pairs in vector */
	u16 v_idx;		/* index in the vsi->q_vector array. */
	u16 reg_idx;		/* register index of the interrupt */
	char name[IFNAMSIZ + 15];
	bool arm_wb_state;
	cpumask_t affinity_mask;
	struct irq_affinity_notify affinity_notify;
};

/* Helper macros to switch between ints/sec and what the register uses.
 * And yes, it's the same math going both ways.  The lowest value
 * supported by all of the iavf hardware is 8.
 */
#define EITR_INTS_PER_SEC_TO_REG(_eitr) \
	((_eitr) ? (1000000000 / ((_eitr) * 256)) : 8)
#define EITR_REG_TO_INTS_PER_SEC EITR_INTS_PER_SEC_TO_REG

#define IAVF_DESC_UNUSED(R) \
	((((R)->next_to_clean > (R)->next_to_use) ? 0 : (R)->count) + \
	(R)->next_to_clean - (R)->next_to_use - 1)

#define OTHER_VECTOR 1
#define NONQ_VECS (OTHER_VECTOR)

#define MIN_MSIX_Q_VECTORS 1
#define MIN_MSIX_COUNT (MIN_MSIX_Q_VECTORS + NONQ_VECS)

#define IAVF_QUEUE_END_OF_LIST 0x7FF
#define IAVF_FREE_VECTOR 0x7FFF
struct iavf_mac_filter {
	struct list_head list;
	u8 macaddr[ETH_ALEN];
	bool remove;		/* filter needs to be removed */
	bool add;		/* filter needs to be added */
};

struct iavf_vlan_filter {
	struct list_head list;
	u16 vlan;
	bool remove;		/* filter needs to be removed */
	bool add;		/* filter needs to be added */
};

#define IAVF_MAX_TRAFFIC_CLASS	4
/* State of traffic class creation */
enum iavf_tc_state_t {
	__IAVF_TC_INVALID, /* no traffic class, default state */
	__IAVF_TC_RUNNING, /* traffic classes have been created */
};

/* channel info */
struct iavf_channel_config {
	struct virtchnl_channel_info ch_info[IAVF_MAX_TRAFFIC_CLASS];
	enum iavf_tc_state_t state;
	u8 total_qps;
};

/* State of cloud filter */
enum iavf_cloud_filter_state_t {
	__IAVF_CF_INVALID,	 /* cloud filter not added */
	__IAVF_CF_ADD_PENDING, /* cloud filter pending add by the PF */
	__IAVF_CF_DEL_PENDING, /* cloud filter pending del by the PF */
	__IAVF_CF_ACTIVE,	 /* cloud filter is active */
};

/* Driver state. The order of these is important! */
enum iavf_state_t {
	__IAVF_STARTUP,		/* driver loaded, probe complete */
	__IAVF_REMOVE,		/* driver is being unloaded */
	__IAVF_INIT_VERSION_CHECK,	/* aq msg sent, awaiting reply */
	__IAVF_INIT_GET_RESOURCES,	/* aq msg sent, awaiting reply */
	__IAVF_INIT_SW,		/* got resources, setting up structs */
	__IAVF_RESETTING,		/* in reset */
	__IAVF_COMM_FAILED,		/* communication with PF failed */
	/* Below here, watchdog is running */
	__IAVF_DOWN,			/* ready, can be opened */
	__IAVF_DOWN_PENDING,		/* descending, waiting for watchdog */
	__IAVF_TESTING,		/* in ethtool self-test */
	__IAVF_RUNNING,		/* opened, working */
};

enum iavf_critical_section_t {
	__IAVF_IN_CRITICAL_TASK,	/* cannot be interrupted */
	__IAVF_IN_CLIENT_TASK,
	__IAVF_IN_REMOVE_TASK,	/* device being removed */
};

#define IAVF_CLOUD_FIELD_OMAC		0x01
#define IAVF_CLOUD_FIELD_IMAC		0x02
#define IAVF_CLOUD_FIELD_IVLAN	0x04
#define IAVF_CLOUD_FIELD_TEN_ID	0x08
#define IAVF_CLOUD_FIELD_IIP		0x10

#define IAVF_CF_FLAGS_OMAC	IAVF_CLOUD_FIELD_OMAC
#define IAVF_CF_FLAGS_IMAC	IAVF_CLOUD_FIELD_IMAC
#define IAVF_CF_FLAGS_IMAC_IVLAN	(IAVF_CLOUD_FIELD_IMAC |\
					 IAVF_CLOUD_FIELD_IVLAN)
#define IAVF_CF_FLAGS_IMAC_TEN_ID	(IAVF_CLOUD_FIELD_IMAC |\
					 IAVF_CLOUD_FIELD_TEN_ID)
#define IAVF_CF_FLAGS_OMAC_TEN_ID_IMAC	(IAVF_CLOUD_FIELD_OMAC |\
						 IAVF_CLOUD_FIELD_IMAC |\
						 IAVF_CLOUD_FIELD_TEN_ID)
#define IAVF_CF_FLAGS_IMAC_IVLAN_TEN_ID	(IAVF_CLOUD_FIELD_IMAC |\
						 IAVF_CLOUD_FIELD_IVLAN |\
						 IAVF_CLOUD_FIELD_TEN_ID)
#define IAVF_CF_FLAGS_IIP	IAVF_CLOUD_FIELD_IIP

/* bookkeeping of cloud filters */
struct iavf_cloud_filter {
	enum iavf_cloud_filter_state_t state;
	struct list_head list;
	struct virtchnl_filter f;
	unsigned long cookie;
	bool del;		/* filter needs to be deleted */
	bool add;		/* filter needs to be added */
};

#define IAVF_RESET_WAIT_MS 10
#define IAVF_RESET_WAIT_DETECTED_COUNT 500
#define IAVF_RESET_WAIT_COMPLETE_COUNT 2000

/* board specific private data structure */
struct iavf_adapter {
	struct work_struct reset_task;
	struct work_struct adminq_task;
	struct delayed_work client_task;
	struct delayed_work init_task;
	wait_queue_head_t down_waitqueue;
	struct iavf_q_vector *q_vectors;
	struct list_head vlan_filter_list;
	struct list_head mac_filter_list;
	/* Lock to protect accesses to MAC and VLAN lists */
	spinlock_t mac_vlan_list_lock;
	char misc_vector_name[IFNAMSIZ + 9];
	int num_active_queues;
	int num_req_queues;

	/* TX */
	struct iavf_ring *tx_rings;
	u32 tx_timeout_count;
	u32 tx_desc_count;

	/* RX */
	struct iavf_ring *rx_rings;
	u64 hw_csum_rx_error;
	u32 rx_desc_count;
	int num_msix_vectors;
	int num_iwarp_msix;
	int iwarp_base_vector;
	u32 client_pending;
	struct iavf_client_instance *cinst;
	struct msix_entry *msix_entries;

	u32 flags;
#define IAVF_FLAG_RX_CSUM_ENABLED		BIT(0)
#define IAVF_FLAG_PF_COMMS_FAILED		BIT(3)
#define IAVF_FLAG_RESET_PENDING		BIT(4)
#define IAVF_FLAG_RESET_NEEDED		BIT(5)
#define IAVF_FLAG_WB_ON_ITR_CAPABLE		BIT(6)
#define IAVF_FLAG_SERVICE_CLIENT_REQUESTED	BIT(9)
#define IAVF_FLAG_CLIENT_NEEDS_OPEN		BIT(10)
#define IAVF_FLAG_CLIENT_NEEDS_CLOSE		BIT(11)
#define IAVF_FLAG_CLIENT_NEEDS_L2_PARAMS	BIT(12)
#define IAVF_FLAG_PROMISC_ON			BIT(13)
#define IAVF_FLAG_ALLMULTI_ON			BIT(14)
#define IAVF_FLAG_LEGACY_RX			BIT(15)
#define IAVF_FLAG_REINIT_ITR_NEEDED		BIT(16)
#define IAVF_FLAG_QUEUES_DISABLED		BIT(17)
/* duplicates for common code */
#define IAVF_FLAG_DCB_ENABLED			0
	/* flags for admin queue service task */
	u32 aq_required;
#define IAVF_FLAG_AQ_ENABLE_QUEUES		BIT(0)
#define IAVF_FLAG_AQ_DISABLE_QUEUES		BIT(1)
#define IAVF_FLAG_AQ_ADD_MAC_FILTER		BIT(2)
#define IAVF_FLAG_AQ_ADD_VLAN_FILTER		BIT(3)
#define IAVF_FLAG_AQ_DEL_MAC_FILTER		BIT(4)
#define IAVF_FLAG_AQ_DEL_VLAN_FILTER		BIT(5)
#define IAVF_FLAG_AQ_CONFIGURE_QUEUES		BIT(6)
#define IAVF_FLAG_AQ_MAP_VECTORS		BIT(7)
#define IAVF_FLAG_AQ_HANDLE_RESET		BIT(8)
#define IAVF_FLAG_AQ_CONFIGURE_RSS		BIT(9) /* direct AQ config */
#define IAVF_FLAG_AQ_GET_CONFIG		BIT(10)
/* Newer style, RSS done by the PF so we can ignore hardware vagaries. */
#define IAVF_FLAG_AQ_GET_HENA			BIT(11)
#define IAVF_FLAG_AQ_SET_HENA			BIT(12)
#define IAVF_FLAG_AQ_SET_RSS_KEY		BIT(13)
#define IAVF_FLAG_AQ_SET_RSS_LUT		BIT(14)
#define IAVF_FLAG_AQ_REQUEST_PROMISC		BIT(15)
#define IAVF_FLAG_AQ_RELEASE_PROMISC		BIT(16)
#define IAVF_FLAG_AQ_REQUEST_ALLMULTI		BIT(17)
#define IAVF_FLAG_AQ_RELEASE_ALLMULTI		BIT(18)
#define IAVF_FLAG_AQ_ENABLE_VLAN_STRIPPING	BIT(19)
#define IAVF_FLAG_AQ_DISABLE_VLAN_STRIPPING	BIT(20)
#define IAVF_FLAG_AQ_ENABLE_CHANNELS		BIT(21)
#define IAVF_FLAG_AQ_DISABLE_CHANNELS		BIT(22)
#define IAVF_FLAG_AQ_ADD_CLOUD_FILTER		BIT(23)
#define IAVF_FLAG_AQ_DEL_CLOUD_FILTER		BIT(24)
#define IAVF_FLAG_AQ_ADD_FDIR_FILTER		BIT(25)
#define IAVF_FLAG_AQ_DEL_FDIR_FILTER		BIT(26)
#define IAVF_FLAG_AQ_ADD_ADV_RSS_CFG		BIT(27)
#define IAVF_FLAG_AQ_DEL_ADV_RSS_CFG		BIT(28)

	/* OS defined structs */
	struct net_device *netdev;
	struct pci_dev *pdev;

	struct iavf_hw hw; /* defined in iavf_type.h */

	enum iavf_state_t state;
	unsigned long crit_section;

	struct delayed_work watchdog_task;
	bool netdev_registered;
	bool link_up;
	enum virtchnl_link_speed link_speed;
	/* This is only populated if the VIRTCHNL_VF_CAP_ADV_LINK_SPEED is set
	 * in vf_res->vf_cap_flags. Use ADV_LINK_SUPPORT macro to determine if
	 * this field is valid. This field should be used going forward and the
	 * enum virtchnl_link_speed above should be considered the legacy way of
	 * storing/communicating link speeds.
	 */
	u32 link_speed_mbps;

	enum virtchnl_ops current_op;
#define CLIENT_ALLOWED(_a) ((_a)->vf_res ? \
			    (_a)->vf_res->vf_cap_flags & \
				VIRTCHNL_VF_OFFLOAD_IWARP : \
			    0)
#define CLIENT_ENABLED(_a) ((_a)->cinst)
/* RSS by the PF should be preferred over RSS via other methods. */
#define RSS_PF(_a) ((_a)->vf_res->vf_cap_flags & \
		    VIRTCHNL_VF_OFFLOAD_RSS_PF)
#define RSS_AQ(_a) ((_a)->vf_res->vf_cap_flags & \
		    VIRTCHNL_VF_OFFLOAD_RSS_AQ)
#define RSS_REG(_a) (!((_a)->vf_res->vf_cap_flags & \
		       (VIRTCHNL_VF_OFFLOAD_RSS_AQ | \
			VIRTCHNL_VF_OFFLOAD_RSS_PF)))
#define VLAN_ALLOWED(_a) ((_a)->vf_res->vf_cap_flags & \
			  VIRTCHNL_VF_OFFLOAD_VLAN)
#define ADV_LINK_SUPPORT(_a) ((_a)->vf_res->vf_cap_flags & \
			      VIRTCHNL_VF_CAP_ADV_LINK_SPEED)
#define FDIR_FLTR_SUPPORT(_a) ((_a)->vf_res->vf_cap_flags & \
			       VIRTCHNL_VF_OFFLOAD_FDIR_PF)
#define ADV_RSS_SUPPORT(_a) ((_a)->vf_res->vf_cap_flags & \
			     VIRTCHNL_VF_OFFLOAD_ADV_RSS_PF)
	struct virtchnl_vf_resource *vf_res; /* incl. all VSIs */
	struct virtchnl_vsi_resource *vsi_res; /* our LAN VSI */
	struct virtchnl_version_info pf_version;
#define PF_IS_V11(_a) (((_a)->pf_version.major == 1) && \
		       ((_a)->pf_version.minor == 1))
	u16 msg_enable;
	struct iavf_eth_stats current_stats;
	struct iavf_vsi vsi;
	u32 aq_wait_count;
	/* RSS stuff */
	u64 hena;
	u16 rss_key_size;
	u16 rss_lut_size;
	u8 *rss_key;
	u8 *rss_lut;
	/* ADQ related members */
	struct iavf_channel_config ch_config;
	u8 num_tc;
	struct list_head cloud_filter_list;
	/* lock to protect access to the cloud filter list */
	spinlock_t cloud_filter_list_lock;
	u16 num_cloud_filters;

#define IAVF_MAX_FDIR_FILTERS 128	/* max allowed Flow Director filters */
	u16 fdir_active_fltr;
	struct list_head fdir_list_head;
	spinlock_t fdir_fltr_lock;	/* protect the Flow Director filter list */

	struct list_head adv_rss_list_head;
	spinlock_t adv_rss_lock;	/* protect the RSS management list */
};


/* Ethtool Private Flags */

/* lan device, used by client interface */
struct iavf_device {
	struct list_head list;
	struct iavf_adapter *vf;
};

/* needed by iavf_ethtool.c */
extern char iavf_driver_name[];
extern struct workqueue_struct *iavf_wq;

int iavf_up(struct iavf_adapter *adapter);
void iavf_down(struct iavf_adapter *adapter);
int iavf_process_config(struct iavf_adapter *adapter);
void iavf_schedule_reset(struct iavf_adapter *adapter);
void iavf_reset(struct iavf_adapter *adapter);
void iavf_set_ethtool_ops(struct net_device *netdev);
void iavf_update_stats(struct iavf_adapter *adapter);
void iavf_reset_interrupt_capability(struct iavf_adapter *adapter);
int iavf_init_interrupt_scheme(struct iavf_adapter *adapter);
void iavf_irq_enable_queues(struct iavf_adapter *adapter, u32 mask);
void iavf_free_all_tx_resources(struct iavf_adapter *adapter);
void iavf_free_all_rx_resources(struct iavf_adapter *adapter);

void iavf_napi_add_all(struct iavf_adapter *adapter);
void iavf_napi_del_all(struct iavf_adapter *adapter);

int iavf_send_api_ver(struct iavf_adapter *adapter);
int iavf_verify_api_ver(struct iavf_adapter *adapter);
int iavf_send_vf_config_msg(struct iavf_adapter *adapter);
int iavf_get_vf_config(struct iavf_adapter *adapter);
void iavf_irq_enable(struct iavf_adapter *adapter, bool flush);
void iavf_configure_queues(struct iavf_adapter *adapter);
void iavf_deconfigure_queues(struct iavf_adapter *adapter);
void iavf_enable_queues(struct iavf_adapter *adapter);
void iavf_disable_queues(struct iavf_adapter *adapter);
void iavf_map_queues(struct iavf_adapter *adapter);
int iavf_request_queues(struct iavf_adapter *adapter, int num);
void iavf_add_ether_addrs(struct iavf_adapter *adapter);
void iavf_del_ether_addrs(struct iavf_adapter *adapter);
void iavf_add_vlans(struct iavf_adapter *adapter);
void iavf_del_vlans(struct iavf_adapter *adapter);
void iavf_set_promiscuous(struct iavf_adapter *adapter, int flags);
void iavf_request_stats(struct iavf_adapter *adapter);
void iavf_request_reset(struct iavf_adapter *adapter);
void iavf_get_hena(struct iavf_adapter *adapter);
void iavf_set_hena(struct iavf_adapter *adapter);
void iavf_set_rss_key(struct iavf_adapter *adapter);
void iavf_set_rss_lut(struct iavf_adapter *adapter);
void iavf_enable_vlan_stripping(struct iavf_adapter *adapter);
void iavf_disable_vlan_stripping(struct iavf_adapter *adapter);
void iavf_virtchnl_completion(struct iavf_adapter *adapter,
			      enum virtchnl_ops v_opcode,
			      enum iavf_status v_retval, u8 *msg, u16 msglen);
int iavf_config_rss(struct iavf_adapter *adapter);
int iavf_lan_add_device(struct iavf_adapter *adapter);
int iavf_lan_del_device(struct iavf_adapter *adapter);
void iavf_client_subtask(struct iavf_adapter *adapter);
void iavf_notify_client_message(struct iavf_vsi *vsi, u8 *msg, u16 len);
void iavf_notify_client_l2_params(struct iavf_vsi *vsi);
void iavf_notify_client_open(struct iavf_vsi *vsi);
void iavf_notify_client_close(struct iavf_vsi *vsi, bool reset);
void iavf_enable_channels(struct iavf_adapter *adapter);
void iavf_disable_channels(struct iavf_adapter *adapter);
void iavf_add_cloud_filter(struct iavf_adapter *adapter);
void iavf_del_cloud_filter(struct iavf_adapter *adapter);
void iavf_add_fdir_filter(struct iavf_adapter *adapter);
void iavf_del_fdir_filter(struct iavf_adapter *adapter);
void iavf_add_adv_rss_cfg(struct iavf_adapter *adapter);
void iavf_del_adv_rss_cfg(struct iavf_adapter *adapter);
struct iavf_mac_filter *iavf_add_filter(struct iavf_adapter *adapter,
					const u8 *macaddr);
#endif /* _IAVF_H_ */
