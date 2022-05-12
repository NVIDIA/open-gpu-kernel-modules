/*
 * NXP Wireless LAN device driver: functions for station ioctl
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
#include "cfg80211.h"

static int disconnect_on_suspend;
module_param(disconnect_on_suspend, int, 0644);

/*
 * Copies the multicast address list from device to driver.
 *
 * This function does not validate the destination memory for
 * size, and the calling function must ensure enough memory is
 * available.
 */
int mwifiex_copy_mcast_addr(struct mwifiex_multicast_list *mlist,
			    struct net_device *dev)
{
	int i = 0;
	struct netdev_hw_addr *ha;

	netdev_for_each_mc_addr(ha, dev)
		memcpy(&mlist->mac_list[i++], ha->addr, ETH_ALEN);

	return i;
}

/*
 * Wait queue completion handler.
 *
 * This function waits on a cmd wait queue. It also cancels the pending
 * request after waking up, in case of errors.
 */
int mwifiex_wait_queue_complete(struct mwifiex_adapter *adapter,
				struct cmd_ctrl_node *cmd_queued)
{
	int status;

	/* Wait for completion */
	status = wait_event_interruptible_timeout(adapter->cmd_wait_q.wait,
						  *(cmd_queued->condition),
						  (12 * HZ));
	if (status <= 0) {
		if (status == 0)
			status = -ETIMEDOUT;
		mwifiex_dbg(adapter, ERROR, "cmd_wait_q terminated: %d\n",
			    status);
		mwifiex_cancel_all_pending_cmd(adapter);
		return status;
	}

	status = adapter->cmd_wait_q.status;
	adapter->cmd_wait_q.status = 0;

	return status;
}

/*
 * This function prepares the correct firmware command and
 * issues it to set the multicast list.
 *
 * This function can be used to enable promiscuous mode, or enable all
 * multicast packets, or to enable selective multicast.
 */
int mwifiex_request_set_multicast_list(struct mwifiex_private *priv,
				struct mwifiex_multicast_list *mcast_list)
{
	int ret = 0;
	u16 old_pkt_filter;

	old_pkt_filter = priv->curr_pkt_filter;

	if (mcast_list->mode == MWIFIEX_PROMISC_MODE) {
		mwifiex_dbg(priv->adapter, INFO,
			    "info: Enable Promiscuous mode\n");
		priv->curr_pkt_filter |= HostCmd_ACT_MAC_PROMISCUOUS_ENABLE;
		priv->curr_pkt_filter &=
			~HostCmd_ACT_MAC_ALL_MULTICAST_ENABLE;
	} else {
		/* Multicast */
		priv->curr_pkt_filter &= ~HostCmd_ACT_MAC_PROMISCUOUS_ENABLE;
		if (mcast_list->mode == MWIFIEX_ALL_MULTI_MODE) {
			mwifiex_dbg(priv->adapter, INFO,
				    "info: Enabling All Multicast!\n");
			priv->curr_pkt_filter |=
				HostCmd_ACT_MAC_ALL_MULTICAST_ENABLE;
		} else {
			priv->curr_pkt_filter &=
				~HostCmd_ACT_MAC_ALL_MULTICAST_ENABLE;
			mwifiex_dbg(priv->adapter, INFO,
				    "info: Set multicast list=%d\n",
				    mcast_list->num_multicast_addr);
			/* Send multicast addresses to firmware */
			ret = mwifiex_send_cmd(priv,
					       HostCmd_CMD_MAC_MULTICAST_ADR,
					       HostCmd_ACT_GEN_SET, 0,
					       mcast_list, false);
		}
	}
	mwifiex_dbg(priv->adapter, INFO,
		    "info: old_pkt_filter=%#x, curr_pkt_filter=%#x\n",
		    old_pkt_filter, priv->curr_pkt_filter);
	if (old_pkt_filter != priv->curr_pkt_filter) {
		ret = mwifiex_send_cmd(priv, HostCmd_CMD_MAC_CONTROL,
				       HostCmd_ACT_GEN_SET,
				       0, &priv->curr_pkt_filter, false);
	}

	return ret;
}

/*
 * This function fills bss descriptor structure using provided
 * information.
 * beacon_ie buffer is allocated in this function. It is caller's
 * responsibility to free the memory.
 */
int mwifiex_fill_new_bss_desc(struct mwifiex_private *priv,
			      struct cfg80211_bss *bss,
			      struct mwifiex_bssdescriptor *bss_desc)
{
	u8 *beacon_ie;
	size_t beacon_ie_len;
	struct mwifiex_bss_priv *bss_priv = (void *)bss->priv;
	const struct cfg80211_bss_ies *ies;

	rcu_read_lock();
	ies = rcu_dereference(bss->ies);
	beacon_ie = kmemdup(ies->data, ies->len, GFP_ATOMIC);
	beacon_ie_len = ies->len;
	bss_desc->timestamp = ies->tsf;
	rcu_read_unlock();

	if (!beacon_ie) {
		mwifiex_dbg(priv->adapter, ERROR,
			    " failed to alloc beacon_ie\n");
		return -ENOMEM;
	}

	memcpy(bss_desc->mac_address, bss->bssid, ETH_ALEN);
	bss_desc->rssi = bss->signal;
	/* The caller of this function will free beacon_ie */
	bss_desc->beacon_buf = beacon_ie;
	bss_desc->beacon_buf_size = beacon_ie_len;
	bss_desc->beacon_period = bss->beacon_interval;
	bss_desc->cap_info_bitmap = bss->capability;
	bss_desc->bss_band = bss_priv->band;
	bss_desc->fw_tsf = bss_priv->fw_tsf;
	if (bss_desc->cap_info_bitmap & WLAN_CAPABILITY_PRIVACY) {
		mwifiex_dbg(priv->adapter, INFO,
			    "info: InterpretIE: AP WEP enabled\n");
		bss_desc->privacy = MWIFIEX_802_11_PRIV_FILTER_8021X_WEP;
	} else {
		bss_desc->privacy = MWIFIEX_802_11_PRIV_FILTER_ACCEPT_ALL;
	}
	if (bss_desc->cap_info_bitmap & WLAN_CAPABILITY_IBSS)
		bss_desc->bss_mode = NL80211_IFTYPE_ADHOC;
	else
		bss_desc->bss_mode = NL80211_IFTYPE_STATION;

	/* Disable 11ac by default. Enable it only where there
	 * exist VHT_CAP IE in AP beacon
	 */
	bss_desc->disable_11ac = true;

	if (bss_desc->cap_info_bitmap & WLAN_CAPABILITY_SPECTRUM_MGMT)
		bss_desc->sensed_11h = true;

	return mwifiex_update_bss_desc_with_ie(priv->adapter, bss_desc);
}

void mwifiex_dnld_txpwr_table(struct mwifiex_private *priv)
{
	if (priv->adapter->dt_node) {
		char txpwr[] = {"marvell,00_txpwrlimit"};

		memcpy(&txpwr[8], priv->adapter->country_code, 2);
		mwifiex_dnld_dt_cfgdata(priv, priv->adapter->dt_node, txpwr);
	}
}

static int mwifiex_process_country_ie(struct mwifiex_private *priv,
				      struct cfg80211_bss *bss)
{
	const u8 *country_ie;
	u8 country_ie_len;
	struct mwifiex_802_11d_domain_reg *domain_info =
					&priv->adapter->domain_reg;

	rcu_read_lock();
	country_ie = ieee80211_bss_get_ie(bss, WLAN_EID_COUNTRY);
	if (!country_ie) {
		rcu_read_unlock();
		return 0;
	}

	country_ie_len = country_ie[1];
	if (country_ie_len < IEEE80211_COUNTRY_IE_MIN_LEN) {
		rcu_read_unlock();
		return 0;
	}

	if (!strncmp(priv->adapter->country_code, &country_ie[2], 2)) {
		rcu_read_unlock();
		mwifiex_dbg(priv->adapter, INFO,
			    "11D: skip setting domain info in FW\n");
		return 0;
	}

	if (country_ie_len >
	    (IEEE80211_COUNTRY_STRING_LEN + MWIFIEX_MAX_TRIPLET_802_11D)) {
		rcu_read_unlock();
		mwifiex_dbg(priv->adapter, ERROR,
			    "11D: country_ie_len overflow!, deauth AP\n");
		return -EINVAL;
	}

	memcpy(priv->adapter->country_code, &country_ie[2], 2);

	domain_info->country_code[0] = country_ie[2];
	domain_info->country_code[1] = country_ie[3];
	domain_info->country_code[2] = ' ';

	country_ie_len -= IEEE80211_COUNTRY_STRING_LEN;

	domain_info->no_of_triplet =
		country_ie_len / sizeof(struct ieee80211_country_ie_triplet);

	memcpy((u8 *)domain_info->triplet,
	       &country_ie[2] + IEEE80211_COUNTRY_STRING_LEN, country_ie_len);

	rcu_read_unlock();

	if (mwifiex_send_cmd(priv, HostCmd_CMD_802_11D_DOMAIN_INFO,
			     HostCmd_ACT_GEN_SET, 0, NULL, false)) {
		mwifiex_dbg(priv->adapter, ERROR,
			    "11D: setting domain info in FW fail\n");
		return -1;
	}

	mwifiex_dnld_txpwr_table(priv);

	return 0;
}

/*
 * In Ad-Hoc mode, the IBSS is created if not found in scan list.
 * In both Ad-Hoc and infra mode, an deauthentication is performed
 * first.
 */
int mwifiex_bss_start(struct mwifiex_private *priv, struct cfg80211_bss *bss,
		      struct cfg80211_ssid *req_ssid)
{
	int ret;
	struct mwifiex_adapter *adapter = priv->adapter;
	struct mwifiex_bssdescriptor *bss_desc = NULL;

	priv->scan_block = false;

	if (bss) {
		if (adapter->region_code == 0x00 &&
		    mwifiex_process_country_ie(priv, bss))
			return -EINVAL;

		/* Allocate and fill new bss descriptor */
		bss_desc = kzalloc(sizeof(struct mwifiex_bssdescriptor),
				   GFP_KERNEL);
		if (!bss_desc)
			return -ENOMEM;

		ret = mwifiex_fill_new_bss_desc(priv, bss, bss_desc);
		if (ret)
			goto done;
	}

	if (priv->bss_mode == NL80211_IFTYPE_STATION ||
	    priv->bss_mode == NL80211_IFTYPE_P2P_CLIENT) {
		u8 config_bands;

		if (!bss_desc)
			return -1;

		if (mwifiex_band_to_radio_type(bss_desc->bss_band) ==
						HostCmd_SCAN_RADIO_TYPE_BG) {
			config_bands = BAND_B | BAND_G | BAND_GN;
		} else {
			config_bands = BAND_A | BAND_AN;
			if (adapter->fw_bands & BAND_AAC)
				config_bands |= BAND_AAC;
		}

		if (!((config_bands | adapter->fw_bands) & ~adapter->fw_bands))
			adapter->config_bands = config_bands;

		ret = mwifiex_check_network_compatibility(priv, bss_desc);
		if (ret)
			goto done;

		if (mwifiex_11h_get_csa_closed_channel(priv) ==
							(u8)bss_desc->channel) {
			mwifiex_dbg(adapter, ERROR,
				    "Attempt to reconnect on csa closed chan(%d)\n",
				    bss_desc->channel);
			ret = -1;
			goto done;
		}

		mwifiex_dbg(adapter, INFO,
			    "info: SSID found in scan list ...\t"
			    "associating...\n");

		mwifiex_stop_net_dev_queue(priv->netdev, adapter);
		if (netif_carrier_ok(priv->netdev))
			netif_carrier_off(priv->netdev);

		/* Clear any past association response stored for
		 * application retrieval */
		priv->assoc_rsp_size = 0;
		ret = mwifiex_associate(priv, bss_desc);

		/* If auth type is auto and association fails using open mode,
		 * try to connect using shared mode */
		if (ret == WLAN_STATUS_NOT_SUPPORTED_AUTH_ALG &&
		    priv->sec_info.is_authtype_auto &&
		    priv->sec_info.wep_enabled) {
			priv->sec_info.authentication_mode =
						NL80211_AUTHTYPE_SHARED_KEY;
			ret = mwifiex_associate(priv, bss_desc);
		}

		if (bss)
			cfg80211_put_bss(priv->adapter->wiphy, bss);
	} else {
		/* Adhoc mode */
		/* If the requested SSID matches current SSID, return */
		if (bss_desc && bss_desc->ssid.ssid_len &&
		    (!mwifiex_ssid_cmp(&priv->curr_bss_params.bss_descriptor.
				       ssid, &bss_desc->ssid))) {
			ret = 0;
			goto done;
		}

		priv->adhoc_is_link_sensed = false;

		ret = mwifiex_check_network_compatibility(priv, bss_desc);

		mwifiex_stop_net_dev_queue(priv->netdev, adapter);
		if (netif_carrier_ok(priv->netdev))
			netif_carrier_off(priv->netdev);

		if (!ret) {
			mwifiex_dbg(adapter, INFO,
				    "info: network found in scan\t"
				    " list. Joining...\n");
			ret = mwifiex_adhoc_join(priv, bss_desc);
			if (bss)
				cfg80211_put_bss(priv->adapter->wiphy, bss);
		} else {
			mwifiex_dbg(adapter, INFO,
				    "info: Network not found in\t"
				    "the list, creating adhoc with ssid = %s\n",
				    req_ssid->ssid);
			ret = mwifiex_adhoc_start(priv, req_ssid);
		}
	}

done:
	/* beacon_ie buffer was allocated in function
	 * mwifiex_fill_new_bss_desc(). Free it now.
	 */
	if (bss_desc)
		kfree(bss_desc->beacon_buf);
	kfree(bss_desc);

	if (ret < 0)
		priv->attempted_bss_desc = NULL;

	return ret;
}

/*
 * IOCTL request handler to set host sleep configuration.
 *
 * This function prepares the correct firmware command and
 * issues it.
 */
int mwifiex_set_hs_params(struct mwifiex_private *priv, u16 action,
			  int cmd_type, struct mwifiex_ds_hs_cfg *hs_cfg)

{
	struct mwifiex_adapter *adapter = priv->adapter;
	int status = 0;
	u32 prev_cond = 0;

	if (!hs_cfg)
		return -ENOMEM;

	switch (action) {
	case HostCmd_ACT_GEN_SET:
		if (adapter->pps_uapsd_mode) {
			mwifiex_dbg(adapter, INFO,
				    "info: Host Sleep IOCTL\t"
				    "is blocked in UAPSD/PPS mode\n");
			status = -1;
			break;
		}
		if (hs_cfg->is_invoke_hostcmd) {
			if (hs_cfg->conditions == HS_CFG_CANCEL) {
				if (!test_bit(MWIFIEX_IS_HS_CONFIGURED,
					      &adapter->work_flags))
					/* Already cancelled */
					break;
				/* Save previous condition */
				prev_cond = le32_to_cpu(adapter->hs_cfg
							.conditions);
				adapter->hs_cfg.conditions =
						cpu_to_le32(hs_cfg->conditions);
			} else if (hs_cfg->conditions) {
				adapter->hs_cfg.conditions =
						cpu_to_le32(hs_cfg->conditions);
				adapter->hs_cfg.gpio = (u8)hs_cfg->gpio;
				if (hs_cfg->gap)
					adapter->hs_cfg.gap = (u8)hs_cfg->gap;
			} else if (adapter->hs_cfg.conditions ==
				   cpu_to_le32(HS_CFG_CANCEL)) {
				/* Return failure if no parameters for HS
				   enable */
				status = -1;
				break;
			}

			status = mwifiex_send_cmd(priv,
						  HostCmd_CMD_802_11_HS_CFG_ENH,
						  HostCmd_ACT_GEN_SET, 0,
						  &adapter->hs_cfg,
						  cmd_type == MWIFIEX_SYNC_CMD);

			if (hs_cfg->conditions == HS_CFG_CANCEL)
				/* Restore previous condition */
				adapter->hs_cfg.conditions =
						cpu_to_le32(prev_cond);
		} else {
			adapter->hs_cfg.conditions =
						cpu_to_le32(hs_cfg->conditions);
			adapter->hs_cfg.gpio = (u8)hs_cfg->gpio;
			adapter->hs_cfg.gap = (u8)hs_cfg->gap;
		}
		break;
	case HostCmd_ACT_GEN_GET:
		hs_cfg->conditions = le32_to_cpu(adapter->hs_cfg.conditions);
		hs_cfg->gpio = adapter->hs_cfg.gpio;
		hs_cfg->gap = adapter->hs_cfg.gap;
		break;
	default:
		status = -1;
		break;
	}

	return status;
}

/*
 * Sends IOCTL request to cancel the existing Host Sleep configuration.
 *
 * This function allocates the IOCTL request buffer, fills it
 * with requisite parameters and calls the IOCTL handler.
 */
int mwifiex_cancel_hs(struct mwifiex_private *priv, int cmd_type)
{
	struct mwifiex_ds_hs_cfg hscfg;

	hscfg.conditions = HS_CFG_CANCEL;
	hscfg.is_invoke_hostcmd = true;

	return mwifiex_set_hs_params(priv, HostCmd_ACT_GEN_SET,
				    cmd_type, &hscfg);
}
EXPORT_SYMBOL_GPL(mwifiex_cancel_hs);

/*
 * Sends IOCTL request to cancel the existing Host Sleep configuration.
 *
 * This function allocates the IOCTL request buffer, fills it
 * with requisite parameters and calls the IOCTL handler.
 */
int mwifiex_enable_hs(struct mwifiex_adapter *adapter)
{
	struct mwifiex_ds_hs_cfg hscfg;
	struct mwifiex_private *priv;
	int i;

	if (disconnect_on_suspend) {
		for (i = 0; i < adapter->priv_num; i++) {
			priv = adapter->priv[i];
			if (priv)
				mwifiex_deauthenticate(priv, NULL);
		}
	}

	priv = mwifiex_get_priv(adapter, MWIFIEX_BSS_ROLE_STA);

	if (priv && priv->sched_scanning) {
#ifdef CONFIG_PM
		if (priv->wdev.wiphy->wowlan_config &&
		    !priv->wdev.wiphy->wowlan_config->nd_config) {
#endif
			mwifiex_dbg(adapter, CMD, "aborting bgscan!\n");
			mwifiex_stop_bg_scan(priv);
			cfg80211_sched_scan_stopped(priv->wdev.wiphy, 0);
#ifdef CONFIG_PM
		}
#endif
	}

	if (adapter->hs_activated) {
		mwifiex_dbg(adapter, CMD,
			    "cmd: HS Already activated\n");
		return true;
	}

	adapter->hs_activate_wait_q_woken = false;

	memset(&hscfg, 0, sizeof(hscfg));
	hscfg.is_invoke_hostcmd = true;

	set_bit(MWIFIEX_IS_HS_ENABLING, &adapter->work_flags);
	mwifiex_cancel_all_pending_cmd(adapter);

	if (mwifiex_set_hs_params(mwifiex_get_priv(adapter,
						   MWIFIEX_BSS_ROLE_STA),
				  HostCmd_ACT_GEN_SET, MWIFIEX_SYNC_CMD,
				  &hscfg)) {
		mwifiex_dbg(adapter, ERROR,
			    "IOCTL request HS enable failed\n");
		return false;
	}

	if (wait_event_interruptible_timeout(adapter->hs_activate_wait_q,
					     adapter->hs_activate_wait_q_woken,
					     (10 * HZ)) <= 0) {
		mwifiex_dbg(adapter, ERROR,
			    "hs_activate_wait_q terminated\n");
		return false;
	}

	return true;
}
EXPORT_SYMBOL_GPL(mwifiex_enable_hs);

/*
 * IOCTL request handler to get BSS information.
 *
 * This function collates the information from different driver structures
 * to send to the user.
 */
int mwifiex_get_bss_info(struct mwifiex_private *priv,
			 struct mwifiex_bss_info *info)
{
	struct mwifiex_adapter *adapter = priv->adapter;
	struct mwifiex_bssdescriptor *bss_desc;

	if (!info)
		return -1;

	bss_desc = &priv->curr_bss_params.bss_descriptor;

	info->bss_mode = priv->bss_mode;

	memcpy(&info->ssid, &bss_desc->ssid, sizeof(struct cfg80211_ssid));

	memcpy(&info->bssid, &bss_desc->mac_address, ETH_ALEN);

	info->bss_chan = bss_desc->channel;

	memcpy(info->country_code, adapter->country_code,
	       IEEE80211_COUNTRY_STRING_LEN);

	info->media_connected = priv->media_connected;

	info->max_power_level = priv->max_tx_power_level;
	info->min_power_level = priv->min_tx_power_level;

	info->adhoc_state = priv->adhoc_state;

	info->bcn_nf_last = priv->bcn_nf_last;

	if (priv->sec_info.wep_enabled)
		info->wep_status = true;
	else
		info->wep_status = false;

	info->is_hs_configured = test_bit(MWIFIEX_IS_HS_CONFIGURED,
					  &adapter->work_flags);
	info->is_deep_sleep = adapter->is_deep_sleep;

	return 0;
}

/*
 * The function disables auto deep sleep mode.
 */
int mwifiex_disable_auto_ds(struct mwifiex_private *priv)
{
	struct mwifiex_ds_auto_ds auto_ds = {
		.auto_ds = DEEP_SLEEP_OFF,
	};

	return mwifiex_send_cmd(priv, HostCmd_CMD_802_11_PS_MODE_ENH,
				DIS_AUTO_PS, BITMAP_AUTO_DS, &auto_ds, true);
}
EXPORT_SYMBOL_GPL(mwifiex_disable_auto_ds);

/*
 * Sends IOCTL request to get the data rate.
 *
 * This function allocates the IOCTL request buffer, fills it
 * with requisite parameters and calls the IOCTL handler.
 */
int mwifiex_drv_get_data_rate(struct mwifiex_private *priv, u32 *rate)
{
	int ret;

	ret = mwifiex_send_cmd(priv, HostCmd_CMD_802_11_TX_RATE_QUERY,
			       HostCmd_ACT_GEN_GET, 0, NULL, true);

	if (!ret) {
		if (priv->is_data_rate_auto)
			*rate = mwifiex_index_to_data_rate(priv, priv->tx_rate,
							   priv->tx_htinfo);
		else
			*rate = priv->data_rate;
	}

	return ret;
}

/*
 * IOCTL request handler to set tx power configuration.
 *
 * This function prepares the correct firmware command and
 * issues it.
 *
 * For non-auto power mode, all the following power groups are set -
 *      - Modulation class HR/DSSS
 *      - Modulation class OFDM
 *      - Modulation class HTBW20
 *      - Modulation class HTBW40
 */
int mwifiex_set_tx_power(struct mwifiex_private *priv,
			 struct mwifiex_power_cfg *power_cfg)
{
	int ret;
	struct host_cmd_ds_txpwr_cfg *txp_cfg;
	struct mwifiex_types_power_group *pg_tlv;
	struct mwifiex_power_group *pg;
	u8 *buf;
	u16 dbm = 0;

	if (!power_cfg->is_power_auto) {
		dbm = (u16) power_cfg->power_level;
		if ((dbm < priv->min_tx_power_level) ||
		    (dbm > priv->max_tx_power_level)) {
			mwifiex_dbg(priv->adapter, ERROR,
				    "txpower value %d dBm\t"
				    "is out of range (%d dBm-%d dBm)\n",
				    dbm, priv->min_tx_power_level,
				    priv->max_tx_power_level);
			return -1;
		}
	}
	buf = kzalloc(MWIFIEX_SIZE_OF_CMD_BUFFER, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	txp_cfg = (struct host_cmd_ds_txpwr_cfg *) buf;
	txp_cfg->action = cpu_to_le16(HostCmd_ACT_GEN_SET);
	if (!power_cfg->is_power_auto) {
		u16 dbm_min = power_cfg->is_power_fixed ?
			      dbm : priv->min_tx_power_level;

		txp_cfg->mode = cpu_to_le32(1);
		pg_tlv = (struct mwifiex_types_power_group *)
			 (buf + sizeof(struct host_cmd_ds_txpwr_cfg));
		pg_tlv->type = cpu_to_le16(TLV_TYPE_POWER_GROUP);
		pg_tlv->length =
			cpu_to_le16(4 * sizeof(struct mwifiex_power_group));
		pg = (struct mwifiex_power_group *)
		     (buf + sizeof(struct host_cmd_ds_txpwr_cfg)
		      + sizeof(struct mwifiex_types_power_group));
		/* Power group for modulation class HR/DSSS */
		pg->first_rate_code = 0x00;
		pg->last_rate_code = 0x03;
		pg->modulation_class = MOD_CLASS_HR_DSSS;
		pg->power_step = 0;
		pg->power_min = (s8) dbm_min;
		pg->power_max = (s8) dbm;
		pg++;
		/* Power group for modulation class OFDM */
		pg->first_rate_code = 0x00;
		pg->last_rate_code = 0x07;
		pg->modulation_class = MOD_CLASS_OFDM;
		pg->power_step = 0;
		pg->power_min = (s8) dbm_min;
		pg->power_max = (s8) dbm;
		pg++;
		/* Power group for modulation class HTBW20 */
		pg->first_rate_code = 0x00;
		pg->last_rate_code = 0x20;
		pg->modulation_class = MOD_CLASS_HT;
		pg->power_step = 0;
		pg->power_min = (s8) dbm_min;
		pg->power_max = (s8) dbm;
		pg->ht_bandwidth = HT_BW_20;
		pg++;
		/* Power group for modulation class HTBW40 */
		pg->first_rate_code = 0x00;
		pg->last_rate_code = 0x20;
		pg->modulation_class = MOD_CLASS_HT;
		pg->power_step = 0;
		pg->power_min = (s8) dbm_min;
		pg->power_max = (s8) dbm;
		pg->ht_bandwidth = HT_BW_40;
	}
	ret = mwifiex_send_cmd(priv, HostCmd_CMD_TXPWR_CFG,
			       HostCmd_ACT_GEN_SET, 0, buf, true);

	kfree(buf);
	return ret;
}

/*
 * IOCTL request handler to get power save mode.
 *
 * This function prepares the correct firmware command and
 * issues it.
 */
int mwifiex_drv_set_power(struct mwifiex_private *priv, u32 *ps_mode)
{
	int ret;
	struct mwifiex_adapter *adapter = priv->adapter;
	u16 sub_cmd;

	if (*ps_mode)
		adapter->ps_mode = MWIFIEX_802_11_POWER_MODE_PSP;
	else
		adapter->ps_mode = MWIFIEX_802_11_POWER_MODE_CAM;
	sub_cmd = (*ps_mode) ? EN_AUTO_PS : DIS_AUTO_PS;
	ret = mwifiex_send_cmd(priv, HostCmd_CMD_802_11_PS_MODE_ENH,
			       sub_cmd, BITMAP_STA_PS, NULL, true);
	if ((!ret) && (sub_cmd == DIS_AUTO_PS))
		ret = mwifiex_send_cmd(priv, HostCmd_CMD_802_11_PS_MODE_ENH,
				       GET_PS, 0, NULL, false);

	return ret;
}

/*
 * IOCTL request handler to set/reset WPA IE.
 *
 * The supplied WPA IE is treated as a opaque buffer. Only the first field
 * is checked to determine WPA version. If buffer length is zero, the existing
 * WPA IE is reset.
 */
static int mwifiex_set_wpa_ie(struct mwifiex_private *priv,
			      u8 *ie_data_ptr, u16 ie_len)
{
	if (ie_len) {
		if (ie_len > sizeof(priv->wpa_ie)) {
			mwifiex_dbg(priv->adapter, ERROR,
				    "failed to copy WPA IE, too big\n");
			return -1;
		}
		memcpy(priv->wpa_ie, ie_data_ptr, ie_len);
		priv->wpa_ie_len = ie_len;
		mwifiex_dbg(priv->adapter, CMD,
			    "cmd: Set Wpa_ie_len=%d IE=%#x\n",
			    priv->wpa_ie_len, priv->wpa_ie[0]);

		if (priv->wpa_ie[0] == WLAN_EID_VENDOR_SPECIFIC) {
			priv->sec_info.wpa_enabled = true;
		} else if (priv->wpa_ie[0] == WLAN_EID_RSN) {
			priv->sec_info.wpa2_enabled = true;
		} else {
			priv->sec_info.wpa_enabled = false;
			priv->sec_info.wpa2_enabled = false;
		}
	} else {
		memset(priv->wpa_ie, 0, sizeof(priv->wpa_ie));
		priv->wpa_ie_len = 0;
		mwifiex_dbg(priv->adapter, INFO,
			    "info: reset wpa_ie_len=%d IE=%#x\n",
			    priv->wpa_ie_len, priv->wpa_ie[0]);
		priv->sec_info.wpa_enabled = false;
		priv->sec_info.wpa2_enabled = false;
	}

	return 0;
}

/*
 * IOCTL request handler to set/reset WAPI IE.
 *
 * The supplied WAPI IE is treated as a opaque buffer. Only the first field
 * is checked to internally enable WAPI. If buffer length is zero, the existing
 * WAPI IE is reset.
 */
static int mwifiex_set_wapi_ie(struct mwifiex_private *priv,
			       u8 *ie_data_ptr, u16 ie_len)
{
	if (ie_len) {
		if (ie_len > sizeof(priv->wapi_ie)) {
			mwifiex_dbg(priv->adapter, ERROR,
				    "info: failed to copy WAPI IE, too big\n");
			return -1;
		}
		memcpy(priv->wapi_ie, ie_data_ptr, ie_len);
		priv->wapi_ie_len = ie_len;
		mwifiex_dbg(priv->adapter, CMD,
			    "cmd: Set wapi_ie_len=%d IE=%#x\n",
			    priv->wapi_ie_len, priv->wapi_ie[0]);

		if (priv->wapi_ie[0] == WLAN_EID_BSS_AC_ACCESS_DELAY)
			priv->sec_info.wapi_enabled = true;
	} else {
		memset(priv->wapi_ie, 0, sizeof(priv->wapi_ie));
		priv->wapi_ie_len = ie_len;
		mwifiex_dbg(priv->adapter, INFO,
			    "info: Reset wapi_ie_len=%d IE=%#x\n",
			    priv->wapi_ie_len, priv->wapi_ie[0]);
		priv->sec_info.wapi_enabled = false;
	}
	return 0;
}

/*
 * IOCTL request handler to set/reset WPS IE.
 *
 * The supplied WPS IE is treated as a opaque buffer. Only the first field
 * is checked to internally enable WPS. If buffer length is zero, the existing
 * WPS IE is reset.
 */
static int mwifiex_set_wps_ie(struct mwifiex_private *priv,
			       u8 *ie_data_ptr, u16 ie_len)
{
	if (ie_len) {
		if (ie_len > MWIFIEX_MAX_VSIE_LEN) {
			mwifiex_dbg(priv->adapter, ERROR,
				    "info: failed to copy WPS IE, too big\n");
			return -1;
		}

		priv->wps_ie = kzalloc(MWIFIEX_MAX_VSIE_LEN, GFP_KERNEL);
		if (!priv->wps_ie)
			return -ENOMEM;

		memcpy(priv->wps_ie, ie_data_ptr, ie_len);
		priv->wps_ie_len = ie_len;
		mwifiex_dbg(priv->adapter, CMD,
			    "cmd: Set wps_ie_len=%d IE=%#x\n",
			    priv->wps_ie_len, priv->wps_ie[0]);
	} else {
		kfree(priv->wps_ie);
		priv->wps_ie_len = ie_len;
		mwifiex_dbg(priv->adapter, INFO,
			    "info: Reset wps_ie_len=%d\n", priv->wps_ie_len);
	}
	return 0;
}

/*
 * IOCTL request handler to set WAPI key.
 *
 * This function prepares the correct firmware command and
 * issues it.
 */
static int mwifiex_sec_ioctl_set_wapi_key(struct mwifiex_private *priv,
			       struct mwifiex_ds_encrypt_key *encrypt_key)
{

	return mwifiex_send_cmd(priv, HostCmd_CMD_802_11_KEY_MATERIAL,
				HostCmd_ACT_GEN_SET, KEY_INFO_ENABLED,
				encrypt_key, true);
}

/*
 * IOCTL request handler to set WEP network key.
 *
 * This function prepares the correct firmware command and
 * issues it, after validation checks.
 */
static int mwifiex_sec_ioctl_set_wep_key(struct mwifiex_private *priv,
			      struct mwifiex_ds_encrypt_key *encrypt_key)
{
	struct mwifiex_adapter *adapter = priv->adapter;
	int ret;
	struct mwifiex_wep_key *wep_key;
	int index;

	if (priv->wep_key_curr_index >= NUM_WEP_KEYS)
		priv->wep_key_curr_index = 0;
	wep_key = &priv->wep_key[priv->wep_key_curr_index];
	index = encrypt_key->key_index;
	if (encrypt_key->key_disable) {
		priv->sec_info.wep_enabled = 0;
	} else if (!encrypt_key->key_len) {
		/* Copy the required key as the current key */
		wep_key = &priv->wep_key[index];
		if (!wep_key->key_length) {
			mwifiex_dbg(adapter, ERROR,
				    "key not set, so cannot enable it\n");
			return -1;
		}

		if (adapter->key_api_major_ver == KEY_API_VER_MAJOR_V2) {
			memcpy(encrypt_key->key_material,
			       wep_key->key_material, wep_key->key_length);
			encrypt_key->key_len = wep_key->key_length;
		}

		priv->wep_key_curr_index = (u16) index;
		priv->sec_info.wep_enabled = 1;
	} else {
		wep_key = &priv->wep_key[index];
		memset(wep_key, 0, sizeof(struct mwifiex_wep_key));
		/* Copy the key in the driver */
		memcpy(wep_key->key_material,
		       encrypt_key->key_material,
		       encrypt_key->key_len);
		wep_key->key_index = index;
		wep_key->key_length = encrypt_key->key_len;
		priv->sec_info.wep_enabled = 1;
	}
	if (wep_key->key_length) {
		void *enc_key;

		if (encrypt_key->key_disable) {
			memset(&priv->wep_key[index], 0,
			       sizeof(struct mwifiex_wep_key));
			goto done;
		}

		if (adapter->key_api_major_ver == KEY_API_VER_MAJOR_V2)
			enc_key = encrypt_key;
		else
			enc_key = NULL;

		/* Send request to firmware */
		ret = mwifiex_send_cmd(priv, HostCmd_CMD_802_11_KEY_MATERIAL,
				       HostCmd_ACT_GEN_SET, 0, enc_key, false);
		if (ret)
			return ret;
	}

done:
	if (priv->sec_info.wep_enabled)
		priv->curr_pkt_filter |= HostCmd_ACT_MAC_WEP_ENABLE;
	else
		priv->curr_pkt_filter &= ~HostCmd_ACT_MAC_WEP_ENABLE;

	ret = mwifiex_send_cmd(priv, HostCmd_CMD_MAC_CONTROL,
			       HostCmd_ACT_GEN_SET, 0,
			       &priv->curr_pkt_filter, true);

	return ret;
}

/*
 * IOCTL request handler to set WPA key.
 *
 * This function prepares the correct firmware command and
 * issues it, after validation checks.
 *
 * Current driver only supports key length of up to 32 bytes.
 *
 * This function can also be used to disable a currently set key.
 */
static int mwifiex_sec_ioctl_set_wpa_key(struct mwifiex_private *priv,
			      struct mwifiex_ds_encrypt_key *encrypt_key)
{
	int ret;
	u8 remove_key = false;
	struct host_cmd_ds_802_11_key_material *ibss_key;

	/* Current driver only supports key length of up to 32 bytes */
	if (encrypt_key->key_len > WLAN_MAX_KEY_LEN) {
		mwifiex_dbg(priv->adapter, ERROR,
			    "key length too long\n");
		return -1;
	}

	if (priv->bss_mode == NL80211_IFTYPE_ADHOC) {
		/*
		 * IBSS/WPA-None uses only one key (Group) for both receiving
		 * and sending unicast and multicast packets.
		 */
		/* Send the key as PTK to firmware */
		encrypt_key->key_index = MWIFIEX_KEY_INDEX_UNICAST;
		ret = mwifiex_send_cmd(priv, HostCmd_CMD_802_11_KEY_MATERIAL,
				       HostCmd_ACT_GEN_SET,
				       KEY_INFO_ENABLED, encrypt_key, false);
		if (ret)
			return ret;

		ibss_key = &priv->aes_key;
		memset(ibss_key, 0,
		       sizeof(struct host_cmd_ds_802_11_key_material));
		/* Copy the key in the driver */
		memcpy(ibss_key->key_param_set.key, encrypt_key->key_material,
		       encrypt_key->key_len);
		memcpy(&ibss_key->key_param_set.key_len, &encrypt_key->key_len,
		       sizeof(ibss_key->key_param_set.key_len));
		ibss_key->key_param_set.key_type_id
			= cpu_to_le16(KEY_TYPE_ID_TKIP);
		ibss_key->key_param_set.key_info = cpu_to_le16(KEY_ENABLED);

		/* Send the key as GTK to firmware */
		encrypt_key->key_index = ~MWIFIEX_KEY_INDEX_UNICAST;
	}

	if (!encrypt_key->key_index)
		encrypt_key->key_index = MWIFIEX_KEY_INDEX_UNICAST;

	if (remove_key)
		ret = mwifiex_send_cmd(priv, HostCmd_CMD_802_11_KEY_MATERIAL,
				       HostCmd_ACT_GEN_SET,
				       !KEY_INFO_ENABLED, encrypt_key, true);
	else
		ret = mwifiex_send_cmd(priv, HostCmd_CMD_802_11_KEY_MATERIAL,
				       HostCmd_ACT_GEN_SET,
				       KEY_INFO_ENABLED, encrypt_key, true);

	return ret;
}

/*
 * IOCTL request handler to set/get network keys.
 *
 * This is a generic key handling function which supports WEP, WPA
 * and WAPI.
 */
static int
mwifiex_sec_ioctl_encrypt_key(struct mwifiex_private *priv,
			      struct mwifiex_ds_encrypt_key *encrypt_key)
{
	int status;

	if (encrypt_key->is_wapi_key)
		status = mwifiex_sec_ioctl_set_wapi_key(priv, encrypt_key);
	else if (encrypt_key->key_len > WLAN_KEY_LEN_WEP104)
		status = mwifiex_sec_ioctl_set_wpa_key(priv, encrypt_key);
	else
		status = mwifiex_sec_ioctl_set_wep_key(priv, encrypt_key);
	return status;
}

/*
 * This function returns the driver version.
 */
int
mwifiex_drv_get_driver_version(struct mwifiex_adapter *adapter, char *version,
			       int max_len)
{
	union {
		__le32 l;
		u8 c[4];
	} ver;
	char fw_ver[32];

	ver.l = cpu_to_le32(adapter->fw_release_number);
	sprintf(fw_ver, "%u.%u.%u.p%u", ver.c[2], ver.c[1], ver.c[0], ver.c[3]);

	snprintf(version, max_len, driver_version, fw_ver);

	mwifiex_dbg(adapter, MSG, "info: MWIFIEX VERSION: %s\n", version);

	return 0;
}

/*
 * Sends IOCTL request to set encoding parameters.
 *
 * This function allocates the IOCTL request buffer, fills it
 * with requisite parameters and calls the IOCTL handler.
 */
int mwifiex_set_encode(struct mwifiex_private *priv, struct key_params *kp,
		       const u8 *key, int key_len, u8 key_index,
		       const u8 *mac_addr, int disable)
{
	struct mwifiex_ds_encrypt_key encrypt_key;

	memset(&encrypt_key, 0, sizeof(encrypt_key));
	encrypt_key.key_len = key_len;
	encrypt_key.key_index = key_index;

	if (kp && kp->cipher == WLAN_CIPHER_SUITE_AES_CMAC)
		encrypt_key.is_igtk_key = true;

	if (!disable) {
		if (key_len)
			memcpy(encrypt_key.key_material, key, key_len);
		else
			encrypt_key.is_current_wep_key = true;

		if (mac_addr)
			memcpy(encrypt_key.mac_addr, mac_addr, ETH_ALEN);
		if (kp && kp->seq && kp->seq_len) {
			memcpy(encrypt_key.pn, kp->seq, kp->seq_len);
			encrypt_key.pn_len = kp->seq_len;
			encrypt_key.is_rx_seq_valid = true;
		}
	} else {
		encrypt_key.key_disable = true;
		if (mac_addr)
			memcpy(encrypt_key.mac_addr, mac_addr, ETH_ALEN);
	}

	return mwifiex_sec_ioctl_encrypt_key(priv, &encrypt_key);
}

/*
 * Sends IOCTL request to get extended version.
 *
 * This function allocates the IOCTL request buffer, fills it
 * with requisite parameters and calls the IOCTL handler.
 */
int
mwifiex_get_ver_ext(struct mwifiex_private *priv, u32 version_str_sel)
{
	struct mwifiex_ver_ext ver_ext;

	memset(&ver_ext, 0, sizeof(ver_ext));
	ver_ext.version_str_sel = version_str_sel;
	if (mwifiex_send_cmd(priv, HostCmd_CMD_VERSION_EXT,
			     HostCmd_ACT_GEN_GET, 0, &ver_ext, true))
		return -1;

	return 0;
}

int
mwifiex_remain_on_chan_cfg(struct mwifiex_private *priv, u16 action,
			   struct ieee80211_channel *chan,
			   unsigned int duration)
{
	struct host_cmd_ds_remain_on_chan roc_cfg;
	u8 sc;

	memset(&roc_cfg, 0, sizeof(roc_cfg));
	roc_cfg.action = cpu_to_le16(action);
	if (action == HostCmd_ACT_GEN_SET) {
		roc_cfg.band_cfg = chan->band;
		sc = mwifiex_chan_type_to_sec_chan_offset(NL80211_CHAN_NO_HT);
		roc_cfg.band_cfg |= (sc << 2);

		roc_cfg.channel =
			ieee80211_frequency_to_channel(chan->center_freq);
		roc_cfg.duration = cpu_to_le32(duration);
	}
	if (mwifiex_send_cmd(priv, HostCmd_CMD_REMAIN_ON_CHAN,
			     action, 0, &roc_cfg, true)) {
		mwifiex_dbg(priv->adapter, ERROR,
			    "failed to remain on channel\n");
		return -1;
	}

	return roc_cfg.status;
}

/*
 * Sends IOCTL request to get statistics information.
 *
 * This function allocates the IOCTL request buffer, fills it
 * with requisite parameters and calls the IOCTL handler.
 */
int
mwifiex_get_stats_info(struct mwifiex_private *priv,
		       struct mwifiex_ds_get_stats *log)
{
	return mwifiex_send_cmd(priv, HostCmd_CMD_802_11_GET_LOG,
				HostCmd_ACT_GEN_GET, 0, log, true);
}

/*
 * IOCTL request handler to read/write register.
 *
 * This function prepares the correct firmware command and
 * issues it.
 *
 * Access to the following registers are supported -
 *      - MAC
 *      - BBP
 *      - RF
 *      - PMIC
 *      - CAU
 */
static int mwifiex_reg_mem_ioctl_reg_rw(struct mwifiex_private *priv,
					struct mwifiex_ds_reg_rw *reg_rw,
					u16 action)
{
	u16 cmd_no;

	switch (reg_rw->type) {
	case MWIFIEX_REG_MAC:
		cmd_no = HostCmd_CMD_MAC_REG_ACCESS;
		break;
	case MWIFIEX_REG_BBP:
		cmd_no = HostCmd_CMD_BBP_REG_ACCESS;
		break;
	case MWIFIEX_REG_RF:
		cmd_no = HostCmd_CMD_RF_REG_ACCESS;
		break;
	case MWIFIEX_REG_PMIC:
		cmd_no = HostCmd_CMD_PMIC_REG_ACCESS;
		break;
	case MWIFIEX_REG_CAU:
		cmd_no = HostCmd_CMD_CAU_REG_ACCESS;
		break;
	default:
		return -1;
	}

	return mwifiex_send_cmd(priv, cmd_no, action, 0, reg_rw, true);
}

/*
 * Sends IOCTL request to write to a register.
 *
 * This function allocates the IOCTL request buffer, fills it
 * with requisite parameters and calls the IOCTL handler.
 */
int
mwifiex_reg_write(struct mwifiex_private *priv, u32 reg_type,
		  u32 reg_offset, u32 reg_value)
{
	struct mwifiex_ds_reg_rw reg_rw;

	reg_rw.type = reg_type;
	reg_rw.offset = reg_offset;
	reg_rw.value = reg_value;

	return mwifiex_reg_mem_ioctl_reg_rw(priv, &reg_rw, HostCmd_ACT_GEN_SET);
}

/*
 * Sends IOCTL request to read from a register.
 *
 * This function allocates the IOCTL request buffer, fills it
 * with requisite parameters and calls the IOCTL handler.
 */
int
mwifiex_reg_read(struct mwifiex_private *priv, u32 reg_type,
		 u32 reg_offset, u32 *value)
{
	int ret;
	struct mwifiex_ds_reg_rw reg_rw;

	reg_rw.type = reg_type;
	reg_rw.offset = reg_offset;
	ret = mwifiex_reg_mem_ioctl_reg_rw(priv, &reg_rw, HostCmd_ACT_GEN_GET);

	if (ret)
		goto done;

	*value = reg_rw.value;

done:
	return ret;
}

/*
 * Sends IOCTL request to read from EEPROM.
 *
 * This function allocates the IOCTL request buffer, fills it
 * with requisite parameters and calls the IOCTL handler.
 */
int
mwifiex_eeprom_read(struct mwifiex_private *priv, u16 offset, u16 bytes,
		    u8 *value)
{
	int ret;
	struct mwifiex_ds_read_eeprom rd_eeprom;

	rd_eeprom.offset =  offset;
	rd_eeprom.byte_count = bytes;

	/* Send request to firmware */
	ret = mwifiex_send_cmd(priv, HostCmd_CMD_802_11_EEPROM_ACCESS,
			       HostCmd_ACT_GEN_GET, 0, &rd_eeprom, true);

	if (!ret)
		memcpy(value, rd_eeprom.value, min((u16)MAX_EEPROM_DATA,
		       rd_eeprom.byte_count));
	return ret;
}

/*
 * This function sets a generic IE. In addition to generic IE, it can
 * also handle WPA, WPA2 and WAPI IEs.
 */
static int
mwifiex_set_gen_ie_helper(struct mwifiex_private *priv, u8 *ie_data_ptr,
			  u16 ie_len)
{
	struct ieee_types_vendor_header *pvendor_ie;
	const u8 wpa_oui[] = { 0x00, 0x50, 0xf2, 0x01 };
	const u8 wps_oui[] = { 0x00, 0x50, 0xf2, 0x04 };
	u16 unparsed_len = ie_len, cur_ie_len;

	/* If the passed length is zero, reset the buffer */
	if (!ie_len) {
		priv->gen_ie_buf_len = 0;
		priv->wps.session_enable = false;
		return 0;
	} else if (!ie_data_ptr ||
		   ie_len <= sizeof(struct ieee_types_header)) {
		return -1;
	}
	pvendor_ie = (struct ieee_types_vendor_header *) ie_data_ptr;

	while (pvendor_ie) {
		cur_ie_len = pvendor_ie->len + sizeof(struct ieee_types_header);

		if (pvendor_ie->element_id == WLAN_EID_RSN) {
			/* IE is a WPA/WPA2 IE so call set_wpa function */
			mwifiex_set_wpa_ie(priv, (u8 *)pvendor_ie, cur_ie_len);
			priv->wps.session_enable = false;
			goto next_ie;
		}

		if (pvendor_ie->element_id == WLAN_EID_BSS_AC_ACCESS_DELAY) {
			/* IE is a WAPI IE so call set_wapi function */
			mwifiex_set_wapi_ie(priv, (u8 *)pvendor_ie,
					    cur_ie_len);
			goto next_ie;
		}

		if (pvendor_ie->element_id == WLAN_EID_VENDOR_SPECIFIC) {
			/* Test to see if it is a WPA IE, if not, then
			 * it is a gen IE
			 */
			if (!memcmp(&pvendor_ie->oui, wpa_oui,
				    sizeof(wpa_oui))) {
				/* IE is a WPA/WPA2 IE so call set_wpa function
				 */
				mwifiex_set_wpa_ie(priv, (u8 *)pvendor_ie,
						   cur_ie_len);
				priv->wps.session_enable = false;
				goto next_ie;
			}

			if (!memcmp(&pvendor_ie->oui, wps_oui,
				    sizeof(wps_oui))) {
				/* Test to see if it is a WPS IE,
				 * if so, enable wps session flag
				 */
				priv->wps.session_enable = true;
				mwifiex_dbg(priv->adapter, MSG,
					    "WPS Session Enabled.\n");
				mwifiex_set_wps_ie(priv, (u8 *)pvendor_ie,
						   cur_ie_len);
				goto next_ie;
			}
		}

		/* Saved in gen_ie, such as P2P IE.etc.*/

		/* Verify that the passed length is not larger than the
		 * available space remaining in the buffer
		 */
		if (cur_ie_len <
		    (sizeof(priv->gen_ie_buf) - priv->gen_ie_buf_len)) {
			/* Append the passed data to the end
			 * of the genIeBuffer
			 */
			memcpy(priv->gen_ie_buf + priv->gen_ie_buf_len,
			       (u8 *)pvendor_ie, cur_ie_len);
			/* Increment the stored buffer length by the
			 * size passed
			 */
			priv->gen_ie_buf_len += cur_ie_len;
		}

next_ie:
		unparsed_len -= cur_ie_len;

		if (unparsed_len <= sizeof(struct ieee_types_header))
			pvendor_ie = NULL;
		else
			pvendor_ie = (struct ieee_types_vendor_header *)
				(((u8 *)pvendor_ie) + cur_ie_len);
	}

	return 0;
}

/*
 * IOCTL request handler to set/get generic IE.
 *
 * In addition to various generic IEs, this function can also be
 * used to set the ARP filter.
 */
static int mwifiex_misc_ioctl_gen_ie(struct mwifiex_private *priv,
				     struct mwifiex_ds_misc_gen_ie *gen_ie,
				     u16 action)
{
	struct mwifiex_adapter *adapter = priv->adapter;

	switch (gen_ie->type) {
	case MWIFIEX_IE_TYPE_GEN_IE:
		if (action == HostCmd_ACT_GEN_GET) {
			gen_ie->len = priv->wpa_ie_len;
			memcpy(gen_ie->ie_data, priv->wpa_ie, gen_ie->len);
		} else {
			mwifiex_set_gen_ie_helper(priv, gen_ie->ie_data,
						  (u16) gen_ie->len);
		}
		break;
	case MWIFIEX_IE_TYPE_ARP_FILTER:
		memset(adapter->arp_filter, 0, sizeof(adapter->arp_filter));
		if (gen_ie->len > ARP_FILTER_MAX_BUF_SIZE) {
			adapter->arp_filter_size = 0;
			mwifiex_dbg(adapter, ERROR,
				    "invalid ARP filter size\n");
			return -1;
		} else {
			memcpy(adapter->arp_filter, gen_ie->ie_data,
			       gen_ie->len);
			adapter->arp_filter_size = gen_ie->len;
		}
		break;
	default:
		mwifiex_dbg(adapter, ERROR, "invalid IE type\n");
		return -1;
	}
	return 0;
}

/*
 * Sends IOCTL request to set a generic IE.
 *
 * This function allocates the IOCTL request buffer, fills it
 * with requisite parameters and calls the IOCTL handler.
 */
int
mwifiex_set_gen_ie(struct mwifiex_private *priv, const u8 *ie, int ie_len)
{
	struct mwifiex_ds_misc_gen_ie gen_ie;

	if (ie_len > IEEE_MAX_IE_SIZE)
		return -EFAULT;

	gen_ie.type = MWIFIEX_IE_TYPE_GEN_IE;
	gen_ie.len = ie_len;
	memcpy(gen_ie.ie_data, ie, ie_len);
	if (mwifiex_misc_ioctl_gen_ie(priv, &gen_ie, HostCmd_ACT_GEN_SET))
		return -EFAULT;

	return 0;
}

/* This function get Host Sleep wake up reason.
 *
 */
int mwifiex_get_wakeup_reason(struct mwifiex_private *priv, u16 action,
			      int cmd_type,
			      struct mwifiex_ds_wakeup_reason *wakeup_reason)
{
	int status = 0;

	status = mwifiex_send_cmd(priv, HostCmd_CMD_HS_WAKEUP_REASON,
				  HostCmd_ACT_GEN_GET, 0, wakeup_reason,
				  cmd_type == MWIFIEX_SYNC_CMD);

	return status;
}

int mwifiex_get_chan_info(struct mwifiex_private *priv,
			  struct mwifiex_channel_band *channel_band)
{
	int status = 0;

	status = mwifiex_send_cmd(priv, HostCmd_CMD_STA_CONFIGURE,
				  HostCmd_ACT_GEN_GET, 0, channel_band,
				  MWIFIEX_SYNC_CMD);

	return status;
}
