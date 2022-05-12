// SPDX-License-Identifier: GPL-2.0
/* Copyright(c) 2013 - 2019 Intel Corporation. */

#include "fm10k.h"
#include "fm10k_vf.h"
#include "fm10k_pf.h"

static s32 fm10k_iov_msg_error(struct fm10k_hw *hw, u32 **results,
			       struct fm10k_mbx_info *mbx)
{
	struct fm10k_vf_info *vf_info = (struct fm10k_vf_info *)mbx;
	struct fm10k_intfc *interface = hw->back;
	struct pci_dev *pdev = interface->pdev;

	dev_err(&pdev->dev, "Unknown message ID %u on VF %d\n",
		**results & FM10K_TLV_ID_MASK, vf_info->vf_idx);

	return fm10k_tlv_msg_error(hw, results, mbx);
}

/**
 *  fm10k_iov_msg_queue_mac_vlan - Message handler for MAC/VLAN request from VF
 *  @hw: Pointer to hardware structure
 *  @results: Pointer array to message, results[0] is pointer to message
 *  @mbx: Pointer to mailbox information structure
 *
 *  This function is a custom handler for MAC/VLAN requests from the VF. The
 *  assumption is that it is acceptable to directly hand off the message from
 *  the VF to the PF's switch manager. However, we use a MAC/VLAN message
 *  queue to avoid overloading the mailbox when a large number of requests
 *  come in.
 **/
static s32 fm10k_iov_msg_queue_mac_vlan(struct fm10k_hw *hw, u32 **results,
					struct fm10k_mbx_info *mbx)
{
	struct fm10k_vf_info *vf_info = (struct fm10k_vf_info *)mbx;
	struct fm10k_intfc *interface = hw->back;
	u8 mac[ETH_ALEN];
	u32 *result;
	int err = 0;
	bool set;
	u16 vlan;
	u32 vid;

	/* we shouldn't be updating rules on a disabled interface */
	if (!FM10K_VF_FLAG_ENABLED(vf_info))
		err = FM10K_ERR_PARAM;

	if (!err && !!results[FM10K_MAC_VLAN_MSG_VLAN]) {
		result = results[FM10K_MAC_VLAN_MSG_VLAN];

		/* record VLAN id requested */
		err = fm10k_tlv_attr_get_u32(result, &vid);
		if (err)
			return err;

		set = !(vid & FM10K_VLAN_CLEAR);
		vid &= ~FM10K_VLAN_CLEAR;

		/* if the length field has been set, this is a multi-bit
		 * update request. For multi-bit requests, simply disallow
		 * them when the pf_vid has been set. In this case, the PF
		 * should have already cleared the VLAN_TABLE, and if we
		 * allowed them, it could allow a rogue VF to receive traffic
		 * on a VLAN it was not assigned. In the single-bit case, we
		 * need to modify requests for VLAN 0 to use the default PF or
		 * SW vid when assigned.
		 */

		if (vid >> 16) {
			/* prevent multi-bit requests when PF has
			 * administratively set the VLAN for this VF
			 */
			if (vf_info->pf_vid)
				return FM10K_ERR_PARAM;
		} else {
			err = fm10k_iov_select_vid(vf_info, (u16)vid);
			if (err < 0)
				return err;

			vid = err;
		}

		/* update VSI info for VF in regards to VLAN table */
		err = hw->mac.ops.update_vlan(hw, vid, vf_info->vsi, set);
	}

	if (!err && !!results[FM10K_MAC_VLAN_MSG_MAC]) {
		result = results[FM10K_MAC_VLAN_MSG_MAC];

		/* record unicast MAC address requested */
		err = fm10k_tlv_attr_get_mac_vlan(result, mac, &vlan);
		if (err)
			return err;

		/* block attempts to set MAC for a locked device */
		if (is_valid_ether_addr(vf_info->mac) &&
		    !ether_addr_equal(mac, vf_info->mac))
			return FM10K_ERR_PARAM;

		set = !(vlan & FM10K_VLAN_CLEAR);
		vlan &= ~FM10K_VLAN_CLEAR;

		err = fm10k_iov_select_vid(vf_info, vlan);
		if (err < 0)
			return err;

		vlan = (u16)err;

		/* Add this request to the MAC/VLAN queue */
		err = fm10k_queue_mac_request(interface, vf_info->glort,
					      mac, vlan, set);
	}

	if (!err && !!results[FM10K_MAC_VLAN_MSG_MULTICAST]) {
		result = results[FM10K_MAC_VLAN_MSG_MULTICAST];

		/* record multicast MAC address requested */
		err = fm10k_tlv_attr_get_mac_vlan(result, mac, &vlan);
		if (err)
			return err;

		/* verify that the VF is allowed to request multicast */
		if (!(vf_info->vf_flags & FM10K_VF_FLAG_MULTI_ENABLED))
			return FM10K_ERR_PARAM;

		set = !(vlan & FM10K_VLAN_CLEAR);
		vlan &= ~FM10K_VLAN_CLEAR;

		err = fm10k_iov_select_vid(vf_info, vlan);
		if (err < 0)
			return err;

		vlan = (u16)err;

		/* Add this request to the MAC/VLAN queue */
		err = fm10k_queue_mac_request(interface, vf_info->glort,
					      mac, vlan, set);
	}

	return err;
}

static const struct fm10k_msg_data iov_mbx_data[] = {
	FM10K_TLV_MSG_TEST_HANDLER(fm10k_tlv_msg_test),
	FM10K_VF_MSG_MSIX_HANDLER(fm10k_iov_msg_msix_pf),
	FM10K_VF_MSG_MAC_VLAN_HANDLER(fm10k_iov_msg_queue_mac_vlan),
	FM10K_VF_MSG_LPORT_STATE_HANDLER(fm10k_iov_msg_lport_state_pf),
	FM10K_TLV_MSG_ERROR_HANDLER(fm10k_iov_msg_error),
};

s32 fm10k_iov_event(struct fm10k_intfc *interface)
{
	struct fm10k_hw *hw = &interface->hw;
	struct fm10k_iov_data *iov_data;
	s64 vflre;
	int i;

	/* if there is no iov_data then there is no mailbox to process */
	if (!READ_ONCE(interface->iov_data))
		return 0;

	rcu_read_lock();

	iov_data = interface->iov_data;

	/* check again now that we are in the RCU block */
	if (!iov_data)
		goto read_unlock;

	if (!(fm10k_read_reg(hw, FM10K_EICR) & FM10K_EICR_VFLR))
		goto read_unlock;

	/* read VFLRE to determine if any VFs have been reset */
	vflre = fm10k_read_reg(hw, FM10K_PFVFLRE(1));
	vflre <<= 32;
	vflre |= fm10k_read_reg(hw, FM10K_PFVFLRE(0));

	i = iov_data->num_vfs;

	for (vflre <<= 64 - i; vflre && i--; vflre += vflre) {
		struct fm10k_vf_info *vf_info = &iov_data->vf_info[i];

		if (vflre >= 0)
			continue;

		hw->iov.ops.reset_resources(hw, vf_info);
		vf_info->mbx.ops.connect(hw, &vf_info->mbx);
	}

read_unlock:
	rcu_read_unlock();

	return 0;
}

s32 fm10k_iov_mbx(struct fm10k_intfc *interface)
{
	struct fm10k_hw *hw = &interface->hw;
	struct fm10k_iov_data *iov_data;
	int i;

	/* if there is no iov_data then there is no mailbox to process */
	if (!READ_ONCE(interface->iov_data))
		return 0;

	rcu_read_lock();

	iov_data = interface->iov_data;

	/* check again now that we are in the RCU block */
	if (!iov_data)
		goto read_unlock;

	/* lock the mailbox for transmit and receive */
	fm10k_mbx_lock(interface);

	/* Most VF messages sent to the PF cause the PF to respond by
	 * requesting from the SM mailbox. This means that too many VF
	 * messages processed at once could cause a mailbox timeout on the PF.
	 * To prevent this, store a pointer to the next VF mbx to process. Use
	 * that as the start of the loop so that we don't starve whichever VF
	 * got ignored on the previous run.
	 */
process_mbx:
	for (i = iov_data->next_vf_mbx ? : iov_data->num_vfs; i--;) {
		struct fm10k_vf_info *vf_info = &iov_data->vf_info[i];
		struct fm10k_mbx_info *mbx = &vf_info->mbx;
		u16 glort = vf_info->glort;

		/* process the SM mailbox first to drain outgoing messages */
		hw->mbx.ops.process(hw, &hw->mbx);

		/* verify port mapping is valid, if not reset port */
		if (vf_info->vf_flags && !fm10k_glort_valid_pf(hw, glort)) {
			hw->iov.ops.reset_lport(hw, vf_info);
			fm10k_clear_macvlan_queue(interface, glort, false);
		}

		/* reset VFs that have mailbox timed out */
		if (!mbx->timeout) {
			hw->iov.ops.reset_resources(hw, vf_info);
			mbx->ops.connect(hw, mbx);
		}

		/* guarantee we have free space in the SM mailbox */
		if (hw->mbx.state == FM10K_STATE_OPEN &&
		    !hw->mbx.ops.tx_ready(&hw->mbx, FM10K_VFMBX_MSG_MTU)) {
			/* keep track of how many times this occurs */
			interface->hw_sm_mbx_full++;

			/* make sure we try again momentarily */
			fm10k_service_event_schedule(interface);

			break;
		}

		/* cleanup mailbox and process received messages */
		mbx->ops.process(hw, mbx);
	}

	/* if we stopped processing mailboxes early, update next_vf_mbx.
	 * Otherwise, reset next_vf_mbx, and restart loop so that we process
	 * the remaining mailboxes we skipped at the start.
	 */
	if (i >= 0) {
		iov_data->next_vf_mbx = i + 1;
	} else if (iov_data->next_vf_mbx) {
		iov_data->next_vf_mbx = 0;
		goto process_mbx;
	}

	/* free the lock */
	fm10k_mbx_unlock(interface);

read_unlock:
	rcu_read_unlock();

	return 0;
}

void fm10k_iov_suspend(struct pci_dev *pdev)
{
	struct fm10k_intfc *interface = pci_get_drvdata(pdev);
	struct fm10k_iov_data *iov_data = interface->iov_data;
	struct fm10k_hw *hw = &interface->hw;
	int num_vfs, i;

	/* pull out num_vfs from iov_data */
	num_vfs = iov_data ? iov_data->num_vfs : 0;

	/* shut down queue mapping for VFs */
	fm10k_write_reg(hw, FM10K_DGLORTMAP(fm10k_dglort_vf_rss),
			FM10K_DGLORTMAP_NONE);

	/* Stop any active VFs and reset their resources */
	for (i = 0; i < num_vfs; i++) {
		struct fm10k_vf_info *vf_info = &iov_data->vf_info[i];

		hw->iov.ops.reset_resources(hw, vf_info);
		hw->iov.ops.reset_lport(hw, vf_info);
		fm10k_clear_macvlan_queue(interface, vf_info->glort, false);
	}
}

static void fm10k_mask_aer_comp_abort(struct pci_dev *pdev)
{
	u32 err_mask;
	int pos;

	pos = pci_find_ext_capability(pdev, PCI_EXT_CAP_ID_ERR);
	if (!pos)
		return;

	/* Mask the completion abort bit in the ERR_UNCOR_MASK register,
	 * preventing the device from reporting these errors to the upstream
	 * PCIe root device. This avoids bringing down platforms which upgrade
	 * non-fatal completer aborts into machine check exceptions. Completer
	 * aborts can occur whenever a VF reads a queue it doesn't own.
	 */
	pci_read_config_dword(pdev, pos + PCI_ERR_UNCOR_MASK, &err_mask);
	err_mask |= PCI_ERR_UNC_COMP_ABORT;
	pci_write_config_dword(pdev, pos + PCI_ERR_UNCOR_MASK, err_mask);
}

int fm10k_iov_resume(struct pci_dev *pdev)
{
	struct fm10k_intfc *interface = pci_get_drvdata(pdev);
	struct fm10k_iov_data *iov_data = interface->iov_data;
	struct fm10k_dglort_cfg dglort = { 0 };
	struct fm10k_hw *hw = &interface->hw;
	int num_vfs, i;

	/* pull out num_vfs from iov_data */
	num_vfs = iov_data ? iov_data->num_vfs : 0;

	/* return error if iov_data is not already populated */
	if (!iov_data)
		return -ENOMEM;

	/* Lower severity of completer abort error reporting as
	 * the VFs can trigger this any time they read a queue
	 * that they don't own.
	 */
	fm10k_mask_aer_comp_abort(pdev);

	/* allocate hardware resources for the VFs */
	hw->iov.ops.assign_resources(hw, num_vfs, num_vfs);

	/* configure DGLORT mapping for RSS */
	dglort.glort = hw->mac.dglort_map & FM10K_DGLORTMAP_NONE;
	dglort.idx = fm10k_dglort_vf_rss;
	dglort.inner_rss = 1;
	dglort.rss_l = fls(fm10k_queues_per_pool(hw) - 1);
	dglort.queue_b = fm10k_vf_queue_index(hw, 0);
	dglort.vsi_l = fls(hw->iov.total_vfs - 1);
	dglort.vsi_b = 1;

	hw->mac.ops.configure_dglort_map(hw, &dglort);

	/* assign resources to the device */
	for (i = 0; i < num_vfs; i++) {
		struct fm10k_vf_info *vf_info = &iov_data->vf_info[i];

		/* allocate all but the last GLORT to the VFs */
		if (i == (~hw->mac.dglort_map >> FM10K_DGLORTMAP_MASK_SHIFT))
			break;

		/* assign GLORT to VF, and restrict it to multicast */
		hw->iov.ops.set_lport(hw, vf_info, i,
				      FM10K_VF_FLAG_MULTI_CAPABLE);

		/* mailbox is disconnected so we don't send a message */
		hw->iov.ops.assign_default_mac_vlan(hw, vf_info);

		/* now we are ready so we can connect */
		vf_info->mbx.ops.connect(hw, &vf_info->mbx);
	}

	return 0;
}

s32 fm10k_iov_update_pvid(struct fm10k_intfc *interface, u16 glort, u16 pvid)
{
	struct fm10k_iov_data *iov_data = interface->iov_data;
	struct fm10k_hw *hw = &interface->hw;
	struct fm10k_vf_info *vf_info;
	u16 vf_idx = (glort - hw->mac.dglort_map) & FM10K_DGLORTMAP_NONE;

	/* no IOV support, not our message to process */
	if (!iov_data)
		return FM10K_ERR_PARAM;

	/* glort outside our range, not our message to process */
	if (vf_idx >= iov_data->num_vfs)
		return FM10K_ERR_PARAM;

	/* determine if an update has occurred and if so notify the VF */
	vf_info = &iov_data->vf_info[vf_idx];
	if (vf_info->sw_vid != pvid) {
		vf_info->sw_vid = pvid;
		hw->iov.ops.assign_default_mac_vlan(hw, vf_info);
	}

	return 0;
}

static void fm10k_iov_free_data(struct pci_dev *pdev)
{
	struct fm10k_intfc *interface = pci_get_drvdata(pdev);

	if (!interface->iov_data)
		return;

	/* reclaim hardware resources */
	fm10k_iov_suspend(pdev);

	/* drop iov_data from interface */
	kfree_rcu(interface->iov_data, rcu);
	interface->iov_data = NULL;
}

static s32 fm10k_iov_alloc_data(struct pci_dev *pdev, int num_vfs)
{
	struct fm10k_intfc *interface = pci_get_drvdata(pdev);
	struct fm10k_iov_data *iov_data = interface->iov_data;
	struct fm10k_hw *hw = &interface->hw;
	size_t size;
	int i;

	/* return error if iov_data is already populated */
	if (iov_data)
		return -EBUSY;

	/* The PF should always be able to assign resources */
	if (!hw->iov.ops.assign_resources)
		return -ENODEV;

	/* nothing to do if no VFs are requested */
	if (!num_vfs)
		return 0;

	/* allocate memory for VF storage */
	size = offsetof(struct fm10k_iov_data, vf_info[num_vfs]);
	iov_data = kzalloc(size, GFP_KERNEL);
	if (!iov_data)
		return -ENOMEM;

	/* record number of VFs */
	iov_data->num_vfs = num_vfs;

	/* loop through vf_info structures initializing each entry */
	for (i = 0; i < num_vfs; i++) {
		struct fm10k_vf_info *vf_info = &iov_data->vf_info[i];
		int err;

		/* Record VF VSI value */
		vf_info->vsi = i + 1;
		vf_info->vf_idx = i;

		/* initialize mailbox memory */
		err = fm10k_pfvf_mbx_init(hw, &vf_info->mbx, iov_mbx_data, i);
		if (err) {
			dev_err(&pdev->dev,
				"Unable to initialize SR-IOV mailbox\n");
			kfree(iov_data);
			return err;
		}
	}

	/* assign iov_data to interface */
	interface->iov_data = iov_data;

	/* allocate hardware resources for the VFs */
	fm10k_iov_resume(pdev);

	return 0;
}

void fm10k_iov_disable(struct pci_dev *pdev)
{
	if (pci_num_vf(pdev) && pci_vfs_assigned(pdev))
		dev_err(&pdev->dev,
			"Cannot disable SR-IOV while VFs are assigned\n");
	else
		pci_disable_sriov(pdev);

	fm10k_iov_free_data(pdev);
}

int fm10k_iov_configure(struct pci_dev *pdev, int num_vfs)
{
	int current_vfs = pci_num_vf(pdev);
	int err = 0;

	if (current_vfs && pci_vfs_assigned(pdev)) {
		dev_err(&pdev->dev,
			"Cannot modify SR-IOV while VFs are assigned\n");
		num_vfs = current_vfs;
	} else {
		pci_disable_sriov(pdev);
		fm10k_iov_free_data(pdev);
	}

	/* allocate resources for the VFs */
	err = fm10k_iov_alloc_data(pdev, num_vfs);
	if (err)
		return err;

	/* allocate VFs if not already allocated */
	if (num_vfs && num_vfs != current_vfs) {
		err = pci_enable_sriov(pdev, num_vfs);
		if (err) {
			dev_err(&pdev->dev,
				"Enable PCI SR-IOV failed: %d\n", err);
			return err;
		}
	}

	return num_vfs;
}

/**
 * fm10k_iov_update_stats - Update stats for all VFs
 * @interface: device private structure
 *
 * Updates the VF statistics for all enabled VFs. Expects to be called by
 * fm10k_update_stats and assumes that locking via the __FM10K_UPDATING_STATS
 * bit is already handled.
 */
void fm10k_iov_update_stats(struct fm10k_intfc *interface)
{
	struct fm10k_iov_data *iov_data = interface->iov_data;
	struct fm10k_hw *hw = &interface->hw;
	int i;

	if (!iov_data)
		return;

	for (i = 0; i < iov_data->num_vfs; i++)
		hw->iov.ops.update_stats(hw, iov_data->vf_info[i].stats, i);
}

static inline void fm10k_reset_vf_info(struct fm10k_intfc *interface,
				       struct fm10k_vf_info *vf_info)
{
	struct fm10k_hw *hw = &interface->hw;

	/* assigning the MAC address will send a mailbox message */
	fm10k_mbx_lock(interface);

	/* disable LPORT for this VF which clears switch rules */
	hw->iov.ops.reset_lport(hw, vf_info);

	fm10k_clear_macvlan_queue(interface, vf_info->glort, false);

	/* assign new MAC+VLAN for this VF */
	hw->iov.ops.assign_default_mac_vlan(hw, vf_info);

	/* re-enable the LPORT for this VF */
	hw->iov.ops.set_lport(hw, vf_info, vf_info->vf_idx,
			      FM10K_VF_FLAG_MULTI_CAPABLE);

	fm10k_mbx_unlock(interface);
}

int fm10k_ndo_set_vf_mac(struct net_device *netdev, int vf_idx, u8 *mac)
{
	struct fm10k_intfc *interface = netdev_priv(netdev);
	struct fm10k_iov_data *iov_data = interface->iov_data;
	struct fm10k_vf_info *vf_info;

	/* verify SR-IOV is active and that vf idx is valid */
	if (!iov_data || vf_idx >= iov_data->num_vfs)
		return -EINVAL;

	/* verify MAC addr is valid */
	if (!is_zero_ether_addr(mac) && !is_valid_ether_addr(mac))
		return -EINVAL;

	/* record new MAC address */
	vf_info = &iov_data->vf_info[vf_idx];
	ether_addr_copy(vf_info->mac, mac);

	fm10k_reset_vf_info(interface, vf_info);

	return 0;
}

int fm10k_ndo_set_vf_vlan(struct net_device *netdev, int vf_idx, u16 vid,
			  u8 qos, __be16 vlan_proto)
{
	struct fm10k_intfc *interface = netdev_priv(netdev);
	struct fm10k_iov_data *iov_data = interface->iov_data;
	struct fm10k_hw *hw = &interface->hw;
	struct fm10k_vf_info *vf_info;

	/* verify SR-IOV is active and that vf idx is valid */
	if (!iov_data || vf_idx >= iov_data->num_vfs)
		return -EINVAL;

	/* QOS is unsupported and VLAN IDs accepted range 0-4094 */
	if (qos || (vid > (VLAN_VID_MASK - 1)))
		return -EINVAL;

	/* VF VLAN Protocol part to default is unsupported */
	if (vlan_proto != htons(ETH_P_8021Q))
		return -EPROTONOSUPPORT;

	vf_info = &iov_data->vf_info[vf_idx];

	/* exit if there is nothing to do */
	if (vf_info->pf_vid == vid)
		return 0;

	/* record default VLAN ID for VF */
	vf_info->pf_vid = vid;

	/* Clear the VLAN table for the VF */
	hw->mac.ops.update_vlan(hw, FM10K_VLAN_ALL, vf_info->vsi, false);

	fm10k_reset_vf_info(interface, vf_info);

	return 0;
}

int fm10k_ndo_set_vf_bw(struct net_device *netdev, int vf_idx,
			int __always_unused min_rate, int max_rate)
{
	struct fm10k_intfc *interface = netdev_priv(netdev);
	struct fm10k_iov_data *iov_data = interface->iov_data;
	struct fm10k_hw *hw = &interface->hw;

	/* verify SR-IOV is active and that vf idx is valid */
	if (!iov_data || vf_idx >= iov_data->num_vfs)
		return -EINVAL;

	/* rate limit cannot be less than 10Mbs or greater than link speed */
	if (max_rate &&
	    (max_rate < FM10K_VF_TC_MIN || max_rate > FM10K_VF_TC_MAX))
		return -EINVAL;

	/* store values */
	iov_data->vf_info[vf_idx].rate = max_rate;

	/* update hardware configuration */
	hw->iov.ops.configure_tc(hw, vf_idx, max_rate);

	return 0;
}

int fm10k_ndo_get_vf_config(struct net_device *netdev,
			    int vf_idx, struct ifla_vf_info *ivi)
{
	struct fm10k_intfc *interface = netdev_priv(netdev);
	struct fm10k_iov_data *iov_data = interface->iov_data;
	struct fm10k_vf_info *vf_info;

	/* verify SR-IOV is active and that vf idx is valid */
	if (!iov_data || vf_idx >= iov_data->num_vfs)
		return -EINVAL;

	vf_info = &iov_data->vf_info[vf_idx];

	ivi->vf = vf_idx;
	ivi->max_tx_rate = vf_info->rate;
	ivi->min_tx_rate = 0;
	ether_addr_copy(ivi->mac, vf_info->mac);
	ivi->vlan = vf_info->pf_vid;
	ivi->qos = 0;

	return 0;
}

int fm10k_ndo_get_vf_stats(struct net_device *netdev,
			   int vf_idx, struct ifla_vf_stats *stats)
{
	struct fm10k_intfc *interface = netdev_priv(netdev);
	struct fm10k_iov_data *iov_data = interface->iov_data;
	struct fm10k_hw *hw = &interface->hw;
	struct fm10k_hw_stats_q *hw_stats;
	u32 idx, qpp;

	/* verify SR-IOV is active and that vf idx is valid */
	if (!iov_data || vf_idx >= iov_data->num_vfs)
		return -EINVAL;

	qpp = fm10k_queues_per_pool(hw);
	hw_stats = iov_data->vf_info[vf_idx].stats;

	for (idx = 0; idx < qpp; idx++) {
		stats->rx_packets += hw_stats[idx].rx_packets.count;
		stats->tx_packets += hw_stats[idx].tx_packets.count;
		stats->rx_bytes += hw_stats[idx].rx_bytes.count;
		stats->tx_bytes += hw_stats[idx].tx_bytes.count;
		stats->rx_dropped += hw_stats[idx].rx_drops.count;
	}

	return 0;
}
