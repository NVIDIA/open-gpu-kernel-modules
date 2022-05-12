/* SPDX-License-Identifier: GPL-2.0 */
/* Copyright (c) 2018, Intel Corporation. */

#ifndef _ICE_VIRTCHNL_PF_H_
#define _ICE_VIRTCHNL_PF_H_
#include "ice.h"
#include "ice_virtchnl_fdir.h"

/* Restrict number of MAC Addr and VLAN that non-trusted VF can programmed */
#define ICE_MAX_VLAN_PER_VF		8
/* MAC filters: 1 is reserved for the VF's default/perm_addr/LAA MAC, 1 for
 * broadcast, and 16 for additional unicast/multicast filters
 */
#define ICE_MAX_MACADDR_PER_VF		18

/* Malicious Driver Detection */
#define ICE_DFLT_NUM_INVAL_MSGS_ALLOWED		10
#define ICE_MDD_EVENTS_THRESHOLD		30

/* Static VF transaction/status register def */
#define VF_DEVICE_STATUS		0xAA
#define VF_TRANS_PENDING_M		0x20

/* wait defines for polling PF_PCI_CIAD register status */
#define ICE_PCI_CIAD_WAIT_COUNT		100
#define ICE_PCI_CIAD_WAIT_DELAY_US	1

/* VF resource constraints */
#define ICE_MAX_VF_COUNT		256
#define ICE_MIN_QS_PER_VF		1
#define ICE_NONQ_VECS_VF		1
#define ICE_MAX_SCATTER_QS_PER_VF	16
#define ICE_MAX_RSS_QS_PER_VF		16
#define ICE_NUM_VF_MSIX_MED		17
#define ICE_NUM_VF_MSIX_SMALL		5
#define ICE_NUM_VF_MSIX_MULTIQ_MIN	3
#define ICE_MIN_INTR_PER_VF		(ICE_MIN_QS_PER_VF + 1)
#define ICE_MAX_VF_RESET_TRIES		40
#define ICE_MAX_VF_RESET_SLEEP_MS	20

#define ice_for_each_vf(pf, i) \
	for ((i) = 0; (i) < (pf)->num_alloc_vfs; (i)++)

/* Specific VF states */
enum ice_vf_states {
	ICE_VF_STATE_INIT = 0,		/* PF is initializing VF */
	ICE_VF_STATE_ACTIVE,		/* VF resources are allocated for use */
	ICE_VF_STATE_QS_ENA,		/* VF queue(s) enabled */
	ICE_VF_STATE_DIS,
	ICE_VF_STATE_MC_PROMISC,
	ICE_VF_STATE_UC_PROMISC,
	ICE_VF_STATES_NBITS
};

/* VF capabilities */
enum ice_virtchnl_cap {
	ICE_VIRTCHNL_VF_CAP_L2 = 0,
	ICE_VIRTCHNL_VF_CAP_PRIVILEGE,
};

/* VF MDD events print structure */
struct ice_mdd_vf_events {
	u16 count;			/* total count of Rx|Tx events */
	/* count number of the last printed event */
	u16 last_printed;
};

/* VF information structure */
struct ice_vf {
	struct ice_pf *pf;

	u16 vf_id;			/* VF ID in the PF space */
	u16 lan_vsi_idx;		/* index into PF struct */
	u16 ctrl_vsi_idx;
	struct ice_vf_fdir fdir;
	/* first vector index of this VF in the PF space */
	int first_vector_idx;
	struct ice_sw *vf_sw_id;	/* switch ID the VF VSIs connect to */
	struct virtchnl_version_info vf_ver;
	u32 driver_caps;		/* reported by VF driver */
	struct virtchnl_ether_addr dflt_lan_addr;
	DECLARE_BITMAP(txq_ena, ICE_MAX_RSS_QS_PER_VF);
	DECLARE_BITMAP(rxq_ena, ICE_MAX_RSS_QS_PER_VF);
	u16 port_vlan_info;		/* Port VLAN ID and QoS */
	u8 pf_set_mac:1;		/* VF MAC address set by VMM admin */
	u8 trusted:1;
	u8 spoofchk:1;
	u8 link_forced:1;
	u8 link_up:1;			/* only valid if VF link is forced */
	/* VSI indices - actual VSI pointers are maintained in the PF structure
	 * When assigned, these will be non-zero, because VSI 0 is always
	 * the main LAN VSI for the PF.
	 */
	u16 lan_vsi_num;		/* ID as used by firmware */
	unsigned int tx_rate;		/* Tx bandwidth limit in Mbps */
	DECLARE_BITMAP(vf_states, ICE_VF_STATES_NBITS);	/* VF runtime states */

	u64 num_inval_msgs;		/* number of continuous invalid msgs */
	u64 num_valid_msgs;		/* number of valid msgs detected */
	unsigned long vf_caps;		/* VF's adv. capabilities */
	u8 num_req_qs;			/* num of queue pairs requested by VF */
	u16 num_mac;
	u16 num_vf_qs;			/* num of queue configured per VF */
	struct ice_mdd_vf_events mdd_rx_events;
	struct ice_mdd_vf_events mdd_tx_events;
	DECLARE_BITMAP(opcodes_allowlist, VIRTCHNL_OP_MAX);
};

#ifdef CONFIG_PCI_IOV
void ice_process_vflr_event(struct ice_pf *pf);
int ice_sriov_configure(struct pci_dev *pdev, int num_vfs);
int ice_set_vf_mac(struct net_device *netdev, int vf_id, u8 *mac);
int
ice_get_vf_cfg(struct net_device *netdev, int vf_id, struct ifla_vf_info *ivi);

void ice_free_vfs(struct ice_pf *pf);
void ice_vc_process_vf_msg(struct ice_pf *pf, struct ice_rq_event_info *event);
void ice_vc_notify_link_state(struct ice_pf *pf);
void ice_vc_notify_reset(struct ice_pf *pf);
bool ice_reset_all_vfs(struct ice_pf *pf, bool is_vflr);
bool ice_reset_vf(struct ice_vf *vf, bool is_vflr);
void ice_restore_all_vfs_msi_state(struct pci_dev *pdev);
bool
ice_is_malicious_vf(struct ice_pf *pf, struct ice_rq_event_info *event,
		    u16 num_msg_proc, u16 num_msg_pending);

int
ice_set_vf_port_vlan(struct net_device *netdev, int vf_id, u16 vlan_id, u8 qos,
		     __be16 vlan_proto);

int ice_set_vf_trust(struct net_device *netdev, int vf_id, bool trusted);

int ice_set_vf_link_state(struct net_device *netdev, int vf_id, int link_state);

int ice_set_vf_spoofchk(struct net_device *netdev, int vf_id, bool ena);

int ice_calc_vf_reg_idx(struct ice_vf *vf, struct ice_q_vector *q_vector);

void ice_set_vf_state_qs_dis(struct ice_vf *vf);
int
ice_get_vf_stats(struct net_device *netdev, int vf_id,
		 struct ifla_vf_stats *vf_stats);
bool ice_is_any_vf_in_promisc(struct ice_pf *pf);
void
ice_vf_lan_overflow_event(struct ice_pf *pf, struct ice_rq_event_info *event);
void ice_print_vfs_mdd_events(struct ice_pf *pf);
void ice_print_vf_rx_mdd_event(struct ice_vf *vf);
struct ice_vsi *ice_vf_ctrl_vsi_setup(struct ice_vf *vf);
int
ice_vc_send_msg_to_vf(struct ice_vf *vf, u32 v_opcode,
		      enum virtchnl_status_code v_retval, u8 *msg, u16 msglen);
bool ice_vc_isvalid_vsi_id(struct ice_vf *vf, u16 vsi_id);
#else /* CONFIG_PCI_IOV */
#define ice_process_vflr_event(pf) do {} while (0)
#define ice_free_vfs(pf) do {} while (0)
#define ice_vc_process_vf_msg(pf, event) do {} while (0)
#define ice_vc_notify_link_state(pf) do {} while (0)
#define ice_vc_notify_reset(pf) do {} while (0)
#define ice_set_vf_state_qs_dis(vf) do {} while (0)
#define ice_vf_lan_overflow_event(pf, event) do {} while (0)
#define ice_print_vfs_mdd_events(pf) do {} while (0)
#define ice_print_vf_rx_mdd_event(vf) do {} while (0)
#define ice_restore_all_vfs_msi_state(pdev) do {} while (0)

static inline bool
ice_is_malicious_vf(struct ice_pf __always_unused *pf,
		    struct ice_rq_event_info __always_unused *event,
		    u16 __always_unused num_msg_proc,
		    u16 __always_unused num_msg_pending)
{
	return false;
}

static inline bool
ice_reset_all_vfs(struct ice_pf __always_unused *pf,
		  bool __always_unused is_vflr)
{
	return true;
}

static inline bool
ice_reset_vf(struct ice_vf __always_unused *vf, bool __always_unused is_vflr)
{
	return true;
}

static inline int
ice_sriov_configure(struct pci_dev __always_unused *pdev,
		    int __always_unused num_vfs)
{
	return -EOPNOTSUPP;
}

static inline int
ice_set_vf_mac(struct net_device __always_unused *netdev,
	       int __always_unused vf_id, u8 __always_unused *mac)
{
	return -EOPNOTSUPP;
}

static inline int
ice_get_vf_cfg(struct net_device __always_unused *netdev,
	       int __always_unused vf_id,
	       struct ifla_vf_info __always_unused *ivi)
{
	return -EOPNOTSUPP;
}

static inline int
ice_set_vf_trust(struct net_device __always_unused *netdev,
		 int __always_unused vf_id, bool __always_unused trusted)
{
	return -EOPNOTSUPP;
}

static inline int
ice_set_vf_port_vlan(struct net_device __always_unused *netdev,
		     int __always_unused vf_id, u16 __always_unused vid,
		     u8 __always_unused qos, __be16 __always_unused v_proto)
{
	return -EOPNOTSUPP;
}

static inline int
ice_set_vf_spoofchk(struct net_device __always_unused *netdev,
		    int __always_unused vf_id, bool __always_unused ena)
{
	return -EOPNOTSUPP;
}

static inline int
ice_set_vf_link_state(struct net_device __always_unused *netdev,
		      int __always_unused vf_id, int __always_unused link_state)
{
	return -EOPNOTSUPP;
}

static inline int
ice_calc_vf_reg_idx(struct ice_vf __always_unused *vf,
		    struct ice_q_vector __always_unused *q_vector)
{
	return 0;
}

static inline int
ice_get_vf_stats(struct net_device __always_unused *netdev,
		 int __always_unused vf_id,
		 struct ifla_vf_stats __always_unused *vf_stats)
{
	return -EOPNOTSUPP;
}

static inline bool ice_is_any_vf_in_promisc(struct ice_pf __always_unused *pf)
{
	return false;
}
#endif /* CONFIG_PCI_IOV */
#endif /* _ICE_VIRTCHNL_PF_H_ */
