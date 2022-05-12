/*
 * NXP Wireless LAN device driver: HW/FW Initialization
 *
 * Copyright 2011-2020 NXP
 *
 * This software file (the "File") is distributed by NXP
 * under the terms of the GNU General Public License Version 2, June 1991
 * (the "License").  You may use, redistribute and/or modify this File in
 * accordance with the terms and conditions of the License, a copy of which
 * is available by writing to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA or on the
 * worldwide web at http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt.
 *
 * THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE
 * ARE EXPRESSLY DISCLAIMED.  The License provides additional details about
 * this warranty disclaimer.
 */

#include "decl.h"
#include "ioctl.h"
#include "util.h"
#include "fw.h"
#include "main.h"
#include "wmm.h"
#include "11n.h"

/*
 * This function adds a BSS priority table to the table list.
 *
 * The function allocates a new BSS priority table node and adds it to
 * the end of BSS priority table list, kept in driver memory.
 */
static int mwifiex_add_bss_prio_tbl(struct mwifiex_private *priv)
{
	struct mwifiex_adapter *adapter = priv->adapter;
	struct mwifiex_bss_prio_node *bss_prio;
	struct mwifiex_bss_prio_tbl *tbl = adapter->bss_prio_tbl;

	bss_prio = kzalloc(sizeof(struct mwifiex_bss_prio_node), GFP_KERNEL);
	if (!bss_prio)
		return -ENOMEM;

	bss_prio->priv = priv;
	INIT_LIST_HEAD(&bss_prio->list);

	spin_lock_bh(&tbl[priv->bss_priority].bss_prio_lock);
	list_add_tail(&bss_prio->list, &tbl[priv->bss_priority].bss_prio_head);
	spin_unlock_bh(&tbl[priv->bss_priority].bss_prio_lock);

	return 0;
}

static void wakeup_timer_fn(struct timer_list *t)
{
	struct mwifiex_adapter *adapter = from_timer(adapter, t, wakeup_timer);

	mwifiex_dbg(adapter, ERROR, "Firmware wakeup failed\n");
	adapter->hw_status = MWIFIEX_HW_STATUS_RESET;
	mwifiex_cancel_all_pending_cmd(adapter);

	if (adapter->if_ops.card_reset)
		adapter->if_ops.card_reset(adapter);
}

static void fw_dump_timer_fn(struct timer_list *t)
{
	struct mwifiex_adapter *adapter = from_timer(adapter, t, devdump_timer);

	mwifiex_upload_device_dump(adapter);
}

/*
 * This function initializes the private structure and sets default
 * values to the members.
 *
 * Additionally, it also initializes all the locks and sets up all the
 * lists.
 */
int mwifiex_init_priv(struct mwifiex_private *priv)
{
	u32 i;

	priv->media_connected = false;
	eth_broadcast_addr(priv->curr_addr);
	priv->port_open = false;
	priv->usb_port = MWIFIEX_USB_EP_DATA;
	priv->pkt_tx_ctrl = 0;
	priv->bss_mode = NL80211_IFTYPE_UNSPECIFIED;
	priv->data_rate = 0;	/* Initially indicate the rate as auto */
	priv->is_data_rate_auto = true;
	priv->bcn_avg_factor = DEFAULT_BCN_AVG_FACTOR;
	priv->data_avg_factor = DEFAULT_DATA_AVG_FACTOR;

	priv->sec_info.wep_enabled = 0;
	priv->sec_info.authentication_mode = NL80211_AUTHTYPE_OPEN_SYSTEM;
	priv->sec_info.encryption_mode = 0;
	for (i = 0; i < ARRAY_SIZE(priv->wep_key); i++)
		memset(&priv->wep_key[i], 0, sizeof(struct mwifiex_wep_key));
	priv->wep_key_curr_index = 0;
	priv->curr_pkt_filter = HostCmd_ACT_MAC_DYNAMIC_BW_ENABLE |
				HostCmd_ACT_MAC_RX_ON | HostCmd_ACT_MAC_TX_ON |
				HostCmd_ACT_MAC_ETHERNETII_ENABLE;

	priv->beacon_period = 100; /* beacon interval */
	priv->attempted_bss_desc = NULL;
	memset(&priv->curr_bss_params, 0, sizeof(priv->curr_bss_params));
	priv->listen_interval = MWIFIEX_DEFAULT_LISTEN_INTERVAL;

	memset(&priv->prev_ssid, 0, sizeof(priv->prev_ssid));
	memset(&priv->prev_bssid, 0, sizeof(priv->prev_bssid));
	memset(&priv->assoc_rsp_buf, 0, sizeof(priv->assoc_rsp_buf));
	priv->assoc_rsp_size = 0;
	priv->adhoc_channel = DEFAULT_AD_HOC_CHANNEL;
	priv->atim_window = 0;
	priv->adhoc_state = ADHOC_IDLE;
	priv->tx_power_level = 0;
	priv->max_tx_power_level = 0;
	priv->min_tx_power_level = 0;
	priv->tx_ant = 0;
	priv->rx_ant = 0;
	priv->tx_rate = 0;
	priv->rxpd_htinfo = 0;
	priv->rxpd_rate = 0;
	priv->rate_bitmap = 0;
	priv->data_rssi_last = 0;
	priv->data_rssi_avg = 0;
	priv->data_nf_avg = 0;
	priv->data_nf_last = 0;
	priv->bcn_rssi_last = 0;
	priv->bcn_rssi_avg = 0;
	priv->bcn_nf_avg = 0;
	priv->bcn_nf_last = 0;
	memset(&priv->wpa_ie, 0, sizeof(priv->wpa_ie));
	memset(&priv->aes_key, 0, sizeof(priv->aes_key));
	priv->wpa_ie_len = 0;
	priv->wpa_is_gtk_set = false;

	memset(&priv->assoc_tlv_buf, 0, sizeof(priv->assoc_tlv_buf));
	priv->assoc_tlv_buf_len = 0;
	memset(&priv->wps, 0, sizeof(priv->wps));
	memset(&priv->gen_ie_buf, 0, sizeof(priv->gen_ie_buf));
	priv->gen_ie_buf_len = 0;
	memset(priv->vs_ie, 0, sizeof(priv->vs_ie));

	priv->wmm_required = true;
	priv->wmm_enabled = false;
	priv->wmm_qosinfo = 0;
	priv->curr_bcn_buf = NULL;
	priv->curr_bcn_size = 0;
	priv->wps_ie = NULL;
	priv->wps_ie_len = 0;
	priv->ap_11n_enabled = 0;
	memset(&priv->roc_cfg, 0, sizeof(priv->roc_cfg));

	priv->scan_block = false;

	priv->csa_chan = 0;
	priv->csa_expire_time = 0;
	priv->del_list_idx = 0;
	priv->hs2_enabled = false;
	priv->check_tdls_tx = false;
	memcpy(priv->tos_to_tid_inv, tos_to_tid_inv, MAX_NUM_TID);

	mwifiex_init_11h_params(priv);

	return mwifiex_add_bss_prio_tbl(priv);
}

/*
 * This function allocates buffers for members of the adapter
 * structure.
 *
 * The memory allocated includes scan table, command buffers, and
 * sleep confirm command buffer. In addition, the queues are
 * also initialized.
 */
static int mwifiex_allocate_adapter(struct mwifiex_adapter *adapter)
{
	int ret;

	/* Allocate command buffer */
	ret = mwifiex_alloc_cmd_buffer(adapter);
	if (ret) {
		mwifiex_dbg(adapter, ERROR,
			    "%s: failed to alloc cmd buffer\n",
			    __func__);
		return -1;
	}

	adapter->sleep_cfm =
		dev_alloc_skb(sizeof(struct mwifiex_opt_sleep_confirm)
			      + INTF_HEADER_LEN);

	if (!adapter->sleep_cfm) {
		mwifiex_dbg(adapter, ERROR,
			    "%s: failed to alloc sleep cfm\t"
			    " cmd buffer\n", __func__);
		return -1;
	}
	skb_reserve(adapter->sleep_cfm, INTF_HEADER_LEN);

	return 0;
}

/*
 * This function initializes the adapter structure and sets default
 * values to the members of adapter.
 *
 * This also initializes the WMM related parameters in the driver private
 * structures.
 */
static void mwifiex_init_adapter(struct mwifiex_adapter *adapter)
{
	struct mwifiex_opt_sleep_confirm *sleep_cfm_buf = NULL;

	skb_put(adapter->sleep_cfm, sizeof(struct mwifiex_opt_sleep_confirm));

	adapter->cmd_sent = false;

	if (adapter->iface_type == MWIFIEX_SDIO)
		adapter->data_sent = true;
	else
		adapter->data_sent = false;

	if (adapter->iface_type == MWIFIEX_USB)
		adapter->intf_hdr_len = 0;
	else
		adapter->intf_hdr_len = INTF_HEADER_LEN;

	adapter->cmd_resp_received = false;
	adapter->event_received = false;
	adapter->data_received = false;

	clear_bit(MWIFIEX_SURPRISE_REMOVED, &adapter->work_flags);

	adapter->hw_status = MWIFIEX_HW_STATUS_INITIALIZING;

	adapter->ps_mode = MWIFIEX_802_11_POWER_MODE_CAM;
	adapter->ps_state = PS_STATE_AWAKE;
	adapter->need_to_wakeup = false;

	adapter->scan_mode = HostCmd_BSS_MODE_ANY;
	adapter->specific_scan_time = MWIFIEX_SPECIFIC_SCAN_CHAN_TIME;
	adapter->active_scan_time = MWIFIEX_ACTIVE_SCAN_CHAN_TIME;
	adapter->passive_scan_time = MWIFIEX_PASSIVE_SCAN_CHAN_TIME;
	adapter->scan_chan_gap_time = MWIFIEX_DEF_SCAN_CHAN_GAP_TIME;

	adapter->scan_probes = 1;

	adapter->multiple_dtim = 1;

	adapter->local_listen_interval = 0;	/* default value in firmware
						   will be used */

	adapter->is_deep_sleep = false;

	adapter->delay_null_pkt = false;
	adapter->delay_to_ps = 1000;
	adapter->enhanced_ps_mode = PS_MODE_AUTO;

	adapter->gen_null_pkt = false;	/* Disable NULL Pkg generation by
					   default */
	adapter->pps_uapsd_mode = false; /* Disable pps/uapsd mode by
					   default */
	adapter->pm_wakeup_card_req = false;

	adapter->pm_wakeup_fw_try = false;

	adapter->curr_tx_buf_size = MWIFIEX_TX_DATA_BUF_SIZE_2K;

	clear_bit(MWIFIEX_IS_HS_CONFIGURED, &adapter->work_flags);
	adapter->hs_cfg.conditions = cpu_to_le32(HS_CFG_COND_DEF);
	adapter->hs_cfg.gpio = HS_CFG_GPIO_DEF;
	adapter->hs_cfg.gap = HS_CFG_GAP_DEF;
	adapter->hs_activated = false;

	memset(adapter->event_body, 0, sizeof(adapter->event_body));
	adapter->hw_dot_11n_dev_cap = 0;
	adapter->hw_dev_mcs_support = 0;
	adapter->sec_chan_offset = 0;
	adapter->adhoc_11n_enabled = false;

	mwifiex_wmm_init(adapter);
	atomic_set(&adapter->tx_hw_pending, 0);

	sleep_cfm_buf = (struct mwifiex_opt_sleep_confirm *)
					adapter->sleep_cfm->data;
	memset(sleep_cfm_buf, 0, adapter->sleep_cfm->len);
	sleep_cfm_buf->command = cpu_to_le16(HostCmd_CMD_802_11_PS_MODE_ENH);
	sleep_cfm_buf->size = cpu_to_le16(adapter->sleep_cfm->len);
	sleep_cfm_buf->result = 0;
	sleep_cfm_buf->action = cpu_to_le16(SLEEP_CONFIRM);
	sleep_cfm_buf->resp_ctrl = cpu_to_le16(RESP_NEEDED);

	memset(&adapter->sleep_params, 0, sizeof(adapter->sleep_params));
	memset(&adapter->sleep_period, 0, sizeof(adapter->sleep_period));
	adapter->tx_lock_flag = false;
	adapter->null_pkt_interval = 0;
	adapter->fw_bands = 0;
	adapter->config_bands = 0;
	adapter->adhoc_start_band = 0;
	adapter->scan_channels = NULL;
	adapter->fw_release_number = 0;
	adapter->fw_cap_info = 0;
	memset(&adapter->upld_buf, 0, sizeof(adapter->upld_buf));
	adapter->event_cause = 0;
	adapter->region_code = 0;
	adapter->bcn_miss_time_out = DEFAULT_BCN_MISS_TIMEOUT;
	adapter->adhoc_awake_period = 0;
	memset(&adapter->arp_filter, 0, sizeof(adapter->arp_filter));
	adapter->arp_filter_size = 0;
	adapter->max_mgmt_ie_index = MAX_MGMT_IE_INDEX;
	adapter->mfg_mode = mfg_mode;
	adapter->key_api_major_ver = 0;
	adapter->key_api_minor_ver = 0;
	eth_broadcast_addr(adapter->perm_addr);
	adapter->iface_limit.sta_intf = MWIFIEX_MAX_STA_NUM;
	adapter->iface_limit.uap_intf = MWIFIEX_MAX_UAP_NUM;
	adapter->iface_limit.p2p_intf = MWIFIEX_MAX_P2P_NUM;
	adapter->active_scan_triggered = false;
	timer_setup(&adapter->wakeup_timer, wakeup_timer_fn, 0);
	adapter->devdump_len = 0;
	timer_setup(&adapter->devdump_timer, fw_dump_timer_fn, 0);
}

/*
 * This function sets trans_start per tx_queue
 */
void mwifiex_set_trans_start(struct net_device *dev)
{
	int i;

	for (i = 0; i < dev->num_tx_queues; i++)
		netdev_get_tx_queue(dev, i)->trans_start = jiffies;

	netif_trans_update(dev);
}

/*
 * This function wakes up all queues in net_device
 */
void mwifiex_wake_up_net_dev_queue(struct net_device *netdev,
					struct mwifiex_adapter *adapter)
{
	spin_lock_bh(&adapter->queue_lock);
	netif_tx_wake_all_queues(netdev);
	spin_unlock_bh(&adapter->queue_lock);
}

/*
 * This function stops all queues in net_device
 */
void mwifiex_stop_net_dev_queue(struct net_device *netdev,
					struct mwifiex_adapter *adapter)
{
	spin_lock_bh(&adapter->queue_lock);
	netif_tx_stop_all_queues(netdev);
	spin_unlock_bh(&adapter->queue_lock);
}

/*
 * This function invalidates the list heads.
 */
static void mwifiex_invalidate_lists(struct mwifiex_adapter *adapter)
{
	struct mwifiex_private *priv;
	s32 i, j;

	list_del(&adapter->cmd_free_q);
	list_del(&adapter->cmd_pending_q);
	list_del(&adapter->scan_pending_q);

	for (i = 0; i < adapter->priv_num; i++)
		list_del(&adapter->bss_prio_tbl[i].bss_prio_head);

	for (i = 0; i < adapter->priv_num; i++) {
		if (adapter->priv[i]) {
			priv = adapter->priv[i];
			for (j = 0; j < MAX_NUM_TID; ++j)
				list_del(&priv->wmm.tid_tbl_ptr[j].ra_list);
			list_del(&priv->tx_ba_stream_tbl_ptr);
			list_del(&priv->rx_reorder_tbl_ptr);
			list_del(&priv->sta_list);
			list_del(&priv->auto_tdls_list);
		}
	}
}

/*
 * This function performs cleanup for adapter structure.
 *
 * The cleanup is done recursively, by canceling all pending
 * commands, freeing the member buffers previously allocated
 * (command buffers, scan table buffer, sleep confirm command
 * buffer), stopping the timers and calling the cleanup routines
 * for every interface.
 */
static void
mwifiex_adapter_cleanup(struct mwifiex_adapter *adapter)
{
	del_timer(&adapter->wakeup_timer);
	del_timer_sync(&adapter->devdump_timer);
	mwifiex_cancel_all_pending_cmd(adapter);
	wake_up_interruptible(&adapter->cmd_wait_q.wait);
	wake_up_interruptible(&adapter->hs_activate_wait_q);
}

void mwifiex_free_cmd_buffers(struct mwifiex_adapter *adapter)
{
	mwifiex_invalidate_lists(adapter);

	/* Free command buffer */
	mwifiex_dbg(adapter, INFO, "info: free cmd buffer\n");
	mwifiex_free_cmd_buffer(adapter);

	if (adapter->sleep_cfm)
		dev_kfree_skb_any(adapter->sleep_cfm);
}

/*
 *  This function intializes the lock variables and
 *  the list heads.
 */
int mwifiex_init_lock_list(struct mwifiex_adapter *adapter)
{
	struct mwifiex_private *priv;
	s32 i, j;

	spin_lock_init(&adapter->int_lock);
	spin_lock_init(&adapter->main_proc_lock);
	spin_lock_init(&adapter->mwifiex_cmd_lock);
	spin_lock_init(&adapter->queue_lock);
	for (i = 0; i < adapter->priv_num; i++) {
		if (adapter->priv[i]) {
			priv = adapter->priv[i];
			spin_lock_init(&priv->wmm.ra_list_spinlock);
			spin_lock_init(&priv->curr_bcn_buf_lock);
			spin_lock_init(&priv->sta_list_spinlock);
			spin_lock_init(&priv->auto_tdls_lock);
		}
	}

	/* Initialize cmd_free_q */
	INIT_LIST_HEAD(&adapter->cmd_free_q);
	/* Initialize cmd_pending_q */
	INIT_LIST_HEAD(&adapter->cmd_pending_q);
	/* Initialize scan_pending_q */
	INIT_LIST_HEAD(&adapter->scan_pending_q);

	spin_lock_init(&adapter->cmd_free_q_lock);
	spin_lock_init(&adapter->cmd_pending_q_lock);
	spin_lock_init(&adapter->scan_pending_q_lock);
	spin_lock_init(&adapter->rx_proc_lock);

	skb_queue_head_init(&adapter->rx_data_q);
	skb_queue_head_init(&adapter->tx_data_q);

	for (i = 0; i < adapter->priv_num; ++i) {
		INIT_LIST_HEAD(&adapter->bss_prio_tbl[i].bss_prio_head);
		spin_lock_init(&adapter->bss_prio_tbl[i].bss_prio_lock);
	}

	for (i = 0; i < adapter->priv_num; i++) {
		if (!adapter->priv[i])
			continue;
		priv = adapter->priv[i];
		for (j = 0; j < MAX_NUM_TID; ++j)
			INIT_LIST_HEAD(&priv->wmm.tid_tbl_ptr[j].ra_list);
		INIT_LIST_HEAD(&priv->tx_ba_stream_tbl_ptr);
		INIT_LIST_HEAD(&priv->rx_reorder_tbl_ptr);
		INIT_LIST_HEAD(&priv->sta_list);
		INIT_LIST_HEAD(&priv->auto_tdls_list);
		skb_queue_head_init(&priv->tdls_txq);
		skb_queue_head_init(&priv->bypass_txq);

		spin_lock_init(&priv->tx_ba_stream_tbl_lock);
		spin_lock_init(&priv->rx_reorder_tbl_lock);

		spin_lock_init(&priv->ack_status_lock);
		idr_init(&priv->ack_status_frames);
	}

	return 0;
}

/*
 * This function initializes the firmware.
 *
 * The following operations are performed sequentially -
 *      - Allocate adapter structure
 *      - Initialize the adapter structure
 *      - Initialize the private structure
 *      - Add BSS priority tables to the adapter structure
 *      - For each interface, send the init commands to firmware
 *      - Send the first command in command pending queue, if available
 */
int mwifiex_init_fw(struct mwifiex_adapter *adapter)
{
	int ret;
	struct mwifiex_private *priv;
	u8 i, first_sta = true;
	int is_cmd_pend_q_empty;

	adapter->hw_status = MWIFIEX_HW_STATUS_INITIALIZING;

	/* Allocate memory for member of adapter structure */
	ret = mwifiex_allocate_adapter(adapter);
	if (ret)
		return -1;

	/* Initialize adapter structure */
	mwifiex_init_adapter(adapter);

	for (i = 0; i < adapter->priv_num; i++) {
		if (adapter->priv[i]) {
			priv = adapter->priv[i];

			/* Initialize private structure */
			ret = mwifiex_init_priv(priv);
			if (ret)
				return -1;
		}
	}
	if (adapter->mfg_mode) {
		adapter->hw_status = MWIFIEX_HW_STATUS_READY;
		ret = -EINPROGRESS;
	} else {
		for (i = 0; i < adapter->priv_num; i++) {
			if (adapter->priv[i]) {
				ret = mwifiex_sta_init_cmd(adapter->priv[i],
							   first_sta, true);
				if (ret == -1)
					return -1;

				first_sta = false;
			}



		}
	}

	spin_lock_bh(&adapter->cmd_pending_q_lock);
	is_cmd_pend_q_empty = list_empty(&adapter->cmd_pending_q);
	spin_unlock_bh(&adapter->cmd_pending_q_lock);
	if (!is_cmd_pend_q_empty) {
		/* Send the first command in queue and return */
		if (mwifiex_main_process(adapter) != -1)
			ret = -EINPROGRESS;
	} else {
		adapter->hw_status = MWIFIEX_HW_STATUS_READY;
	}

	return ret;
}

/*
 * This function deletes the BSS priority tables.
 *
 * The function traverses through all the allocated BSS priority nodes
 * in every BSS priority table and frees them.
 */
static void mwifiex_delete_bss_prio_tbl(struct mwifiex_private *priv)
{
	int i;
	struct mwifiex_adapter *adapter = priv->adapter;
	struct mwifiex_bss_prio_node *bssprio_node, *tmp_node;
	struct list_head *head;
	spinlock_t *lock; /* bss priority lock */

	for (i = 0; i < adapter->priv_num; ++i) {
		head = &adapter->bss_prio_tbl[i].bss_prio_head;
		lock = &adapter->bss_prio_tbl[i].bss_prio_lock;
		mwifiex_dbg(adapter, INFO,
			    "info: delete BSS priority table,\t"
			    "bss_type = %d, bss_num = %d, i = %d,\t"
			    "head = %p\n",
			    priv->bss_type, priv->bss_num, i, head);

		{
			spin_lock_bh(lock);
			list_for_each_entry_safe(bssprio_node, tmp_node, head,
						 list) {
				if (bssprio_node->priv == priv) {
					mwifiex_dbg(adapter, INFO,
						    "info: Delete\t"
						    "node %p, next = %p\n",
						    bssprio_node, tmp_node);
					list_del(&bssprio_node->list);
					kfree(bssprio_node);
				}
			}
			spin_unlock_bh(lock);
		}
	}
}

/*
 * This function frees the private structure, including cleans
 * up the TX and RX queues and frees the BSS priority tables.
 */
void mwifiex_free_priv(struct mwifiex_private *priv)
{
	mwifiex_clean_txrx(priv);
	mwifiex_delete_bss_prio_tbl(priv);
	mwifiex_free_curr_bcn(priv);
}

/*
 * This function is used to shutdown the driver.
 *
 * The following operations are performed sequentially -
 *      - Check if already shut down
 *      - Make sure the main process has stopped
 *      - Clean up the Tx and Rx queues
 *      - Delete BSS priority tables
 *      - Free the adapter
 *      - Notify completion
 */
void
mwifiex_shutdown_drv(struct mwifiex_adapter *adapter)
{
	struct mwifiex_private *priv;
	s32 i;
	struct sk_buff *skb;

	/* mwifiex already shutdown */
	if (adapter->hw_status == MWIFIEX_HW_STATUS_NOT_READY)
		return;

	/* cancel current command */
	if (adapter->curr_cmd) {
		mwifiex_dbg(adapter, WARN,
			    "curr_cmd is still in processing\n");
		del_timer_sync(&adapter->cmd_timer);
		mwifiex_recycle_cmd_node(adapter, adapter->curr_cmd);
		adapter->curr_cmd = NULL;
	}

	/* shut down mwifiex */
	mwifiex_dbg(adapter, MSG,
		    "info: shutdown mwifiex...\n");

	/* Clean up Tx/Rx queues and delete BSS priority table */
	for (i = 0; i < adapter->priv_num; i++) {
		if (adapter->priv[i]) {
			priv = adapter->priv[i];

			mwifiex_clean_auto_tdls(priv);
			mwifiex_abort_cac(priv);
			mwifiex_free_priv(priv);
		}
	}

	atomic_set(&adapter->tx_queued, 0);
	while ((skb = skb_dequeue(&adapter->tx_data_q)))
		mwifiex_write_data_complete(adapter, skb, 0, 0);

	spin_lock_bh(&adapter->rx_proc_lock);

	while ((skb = skb_dequeue(&adapter->rx_data_q))) {
		struct mwifiex_rxinfo *rx_info = MWIFIEX_SKB_RXCB(skb);

		atomic_dec(&adapter->rx_pending);
		priv = adapter->priv[rx_info->bss_num];
		if (priv)
			priv->stats.rx_dropped++;

		dev_kfree_skb_any(skb);
	}

	spin_unlock_bh(&adapter->rx_proc_lock);

	mwifiex_adapter_cleanup(adapter);

	adapter->hw_status = MWIFIEX_HW_STATUS_NOT_READY;
}

/*
 * This function downloads the firmware to the card.
 *
 * The actual download is preceded by two sanity checks -
 *      - Check if firmware is already running
 *      - Check if the interface is the winner to download the firmware
 *
 * ...and followed by another -
 *      - Check if the firmware is downloaded successfully
 *
 * After download is successfully completed, the host interrupts are enabled.
 */
int mwifiex_dnld_fw(struct mwifiex_adapter *adapter,
		    struct mwifiex_fw_image *pmfw)
{
	int ret;
	u32 poll_num = 1;

	/* check if firmware is already running */
	ret = adapter->if_ops.check_fw_status(adapter, poll_num);
	if (!ret) {
		mwifiex_dbg(adapter, MSG,
			    "WLAN FW already running! Skip FW dnld\n");
		return 0;
	}

	/* check if we are the winner for downloading FW */
	if (adapter->if_ops.check_winner_status) {
		adapter->winner = 0;
		ret = adapter->if_ops.check_winner_status(adapter);

		poll_num = MAX_FIRMWARE_POLL_TRIES;
		if (ret) {
			mwifiex_dbg(adapter, MSG,
				    "WLAN read winner status failed!\n");
			return ret;
		}

		if (!adapter->winner) {
			mwifiex_dbg(adapter, MSG,
				    "WLAN is not the winner! Skip FW dnld\n");
			goto poll_fw;
		}
	}

	if (pmfw) {
		/* Download firmware with helper */
		ret = adapter->if_ops.prog_fw(adapter, pmfw);
		if (ret) {
			mwifiex_dbg(adapter, ERROR,
				    "prog_fw failed ret=%#x\n", ret);
			return ret;
		}
	}

poll_fw:
	/* Check if the firmware is downloaded successfully or not */
	ret = adapter->if_ops.check_fw_status(adapter, poll_num);
	if (ret)
		mwifiex_dbg(adapter, ERROR,
			    "FW failed to be active in time\n");

	return ret;
}
EXPORT_SYMBOL_GPL(mwifiex_dnld_fw);
