// SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause
/*
 * Copyright (C) 2012-2014, 2018-2020 Intel Corporation
 * Copyright (C) 2013-2015 Intel Mobile Communications GmbH
 * Copyright (C) 2016-2017 Intel Deutschland GmbH
 */
#include <asm/unaligned.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include "iwl-trans.h"
#include "mvm.h"
#include "fw-api.h"

/*
 * iwl_mvm_rx_rx_phy_cmd - REPLY_RX_PHY_CMD handler
 *
 * Copies the phy information in mvm->last_phy_info, it will be used when the
 * actual data will come from the fw in the next packet.
 */
void iwl_mvm_rx_rx_phy_cmd(struct iwl_mvm *mvm, struct iwl_rx_cmd_buffer *rxb)
{
	struct iwl_rx_packet *pkt = rxb_addr(rxb);
	unsigned int pkt_len = iwl_rx_packet_payload_len(pkt);

	if (unlikely(pkt_len < sizeof(mvm->last_phy_info)))
		return;

	memcpy(&mvm->last_phy_info, pkt->data, sizeof(mvm->last_phy_info));
	mvm->ampdu_ref++;

#ifdef CONFIG_IWLWIFI_DEBUGFS
	if (mvm->last_phy_info.phy_flags & cpu_to_le16(RX_RES_PHY_FLAGS_AGG)) {
		spin_lock(&mvm->drv_stats_lock);
		mvm->drv_rx_stats.ampdu_count++;
		spin_unlock(&mvm->drv_stats_lock);
	}
#endif
}

/*
 * iwl_mvm_pass_packet_to_mac80211 - builds the packet for mac80211
 *
 * Adds the rxb to a new skb and give it to mac80211
 */
static void iwl_mvm_pass_packet_to_mac80211(struct iwl_mvm *mvm,
					    struct ieee80211_sta *sta,
					    struct napi_struct *napi,
					    struct sk_buff *skb,
					    struct ieee80211_hdr *hdr, u16 len,
					    u8 crypt_len,
					    struct iwl_rx_cmd_buffer *rxb)
{
	unsigned int hdrlen = ieee80211_hdrlen(hdr->frame_control);
	unsigned int fraglen;

	/*
	 * The 'hdrlen' (plus the 8 bytes for the SNAP and the crypt_len,
	 * but those are all multiples of 4 long) all goes away, but we
	 * want the *end* of it, which is going to be the start of the IP
	 * header, to be aligned when it gets pulled in.
	 * The beginning of the skb->data is aligned on at least a 4-byte
	 * boundary after allocation. Everything here is aligned at least
	 * on a 2-byte boundary so we can just take hdrlen & 3 and pad by
	 * the result.
	 */
	skb_reserve(skb, hdrlen & 3);

	/* If frame is small enough to fit in skb->head, pull it completely.
	 * If not, only pull ieee80211_hdr (including crypto if present, and
	 * an additional 8 bytes for SNAP/ethertype, see below) so that
	 * splice() or TCP coalesce are more efficient.
	 *
	 * Since, in addition, ieee80211_data_to_8023() always pull in at
	 * least 8 bytes (possibly more for mesh) we can do the same here
	 * to save the cost of doing it later. That still doesn't pull in
	 * the actual IP header since the typical case has a SNAP header.
	 * If the latter changes (there are efforts in the standards group
	 * to do so) we should revisit this and ieee80211_data_to_8023().
	 */
	hdrlen = (len <= skb_tailroom(skb)) ? len : hdrlen + crypt_len + 8;

	skb_put_data(skb, hdr, hdrlen);
	fraglen = len - hdrlen;

	if (fraglen) {
		int offset = (void *)hdr + hdrlen -
			     rxb_addr(rxb) + rxb_offset(rxb);

		skb_add_rx_frag(skb, 0, rxb_steal_page(rxb), offset,
				fraglen, rxb->truesize);
	}

	ieee80211_rx_napi(mvm->hw, sta, skb, napi);
}

/*
 * iwl_mvm_get_signal_strength - use new rx PHY INFO API
 * values are reported by the fw as positive values - need to negate
 * to obtain their dBM.  Account for missing antennas by replacing 0
 * values by -256dBm: practically 0 power and a non-feasible 8 bit value.
 */
static void iwl_mvm_get_signal_strength(struct iwl_mvm *mvm,
					struct iwl_rx_phy_info *phy_info,
					struct ieee80211_rx_status *rx_status)
{
	int energy_a, energy_b, energy_c, max_energy;
	u32 val;

	val =
	    le32_to_cpu(phy_info->non_cfg_phy[IWL_RX_INFO_ENERGY_ANT_ABC_IDX]);
	energy_a = (val & IWL_RX_INFO_ENERGY_ANT_A_MSK) >>
						IWL_RX_INFO_ENERGY_ANT_A_POS;
	energy_a = energy_a ? -energy_a : S8_MIN;
	energy_b = (val & IWL_RX_INFO_ENERGY_ANT_B_MSK) >>
						IWL_RX_INFO_ENERGY_ANT_B_POS;
	energy_b = energy_b ? -energy_b : S8_MIN;
	energy_c = (val & IWL_RX_INFO_ENERGY_ANT_C_MSK) >>
						IWL_RX_INFO_ENERGY_ANT_C_POS;
	energy_c = energy_c ? -energy_c : S8_MIN;
	max_energy = max(energy_a, energy_b);
	max_energy = max(max_energy, energy_c);

	IWL_DEBUG_STATS(mvm, "energy In A %d B %d C %d , and max %d\n",
			energy_a, energy_b, energy_c, max_energy);

	rx_status->signal = max_energy;
	rx_status->chains = (le16_to_cpu(phy_info->phy_flags) &
				RX_RES_PHY_FLAGS_ANTENNA)
					>> RX_RES_PHY_FLAGS_ANTENNA_POS;
	rx_status->chain_signal[0] = energy_a;
	rx_status->chain_signal[1] = energy_b;
	rx_status->chain_signal[2] = energy_c;
}

/*
 * iwl_mvm_set_mac80211_rx_flag - translate fw status to mac80211 format
 * @mvm: the mvm object
 * @hdr: 80211 header
 * @stats: status in mac80211's format
 * @rx_pkt_status: status coming from fw
 *
 * returns non 0 value if the packet should be dropped
 */
static u32 iwl_mvm_set_mac80211_rx_flag(struct iwl_mvm *mvm,
					struct ieee80211_hdr *hdr,
					struct ieee80211_rx_status *stats,
					u32 rx_pkt_status,
					u8 *crypt_len)
{
	if (!ieee80211_has_protected(hdr->frame_control) ||
	    (rx_pkt_status & RX_MPDU_RES_STATUS_SEC_ENC_MSK) ==
			     RX_MPDU_RES_STATUS_SEC_NO_ENC)
		return 0;

	/* packet was encrypted with unknown alg */
	if ((rx_pkt_status & RX_MPDU_RES_STATUS_SEC_ENC_MSK) ==
					RX_MPDU_RES_STATUS_SEC_ENC_ERR)
		return 0;

	switch (rx_pkt_status & RX_MPDU_RES_STATUS_SEC_ENC_MSK) {
	case RX_MPDU_RES_STATUS_SEC_CCM_ENC:
		/* alg is CCM: check MIC only */
		if (!(rx_pkt_status & RX_MPDU_RES_STATUS_MIC_OK))
			return -1;

		stats->flag |= RX_FLAG_DECRYPTED;
		*crypt_len = IEEE80211_CCMP_HDR_LEN;
		return 0;

	case RX_MPDU_RES_STATUS_SEC_TKIP_ENC:
		/* Don't drop the frame and decrypt it in SW */
		if (!fw_has_api(&mvm->fw->ucode_capa,
				IWL_UCODE_TLV_API_DEPRECATE_TTAK) &&
		    !(rx_pkt_status & RX_MPDU_RES_STATUS_TTAK_OK))
			return 0;
		*crypt_len = IEEE80211_TKIP_IV_LEN;
		fallthrough;

	case RX_MPDU_RES_STATUS_SEC_WEP_ENC:
		if (!(rx_pkt_status & RX_MPDU_RES_STATUS_ICV_OK))
			return -1;

		stats->flag |= RX_FLAG_DECRYPTED;
		if ((rx_pkt_status & RX_MPDU_RES_STATUS_SEC_ENC_MSK) ==
				RX_MPDU_RES_STATUS_SEC_WEP_ENC)
			*crypt_len = IEEE80211_WEP_IV_LEN;
		return 0;

	case RX_MPDU_RES_STATUS_SEC_EXT_ENC:
		if (!(rx_pkt_status & RX_MPDU_RES_STATUS_MIC_OK))
			return -1;
		stats->flag |= RX_FLAG_DECRYPTED;
		return 0;

	default:
		/* Expected in monitor (not having the keys) */
		if (!mvm->monitor_on)
			IWL_ERR(mvm, "Unhandled alg: 0x%x\n", rx_pkt_status);
	}

	return 0;
}

static void iwl_mvm_rx_handle_tcm(struct iwl_mvm *mvm,
				  struct ieee80211_sta *sta,
				  struct ieee80211_hdr *hdr, u32 len,
				  struct iwl_rx_phy_info *phy_info,
				  u32 rate_n_flags)
{
	struct iwl_mvm_sta *mvmsta;
	struct iwl_mvm_tcm_mac *mdata;
	int mac;
	int ac = IEEE80211_AC_BE; /* treat non-QoS as BE */
	struct iwl_mvm_vif *mvmvif;
	/* expected throughput in 100Kbps, single stream, 20 MHz */
	static const u8 thresh_tpt[] = {
		9, 18, 30, 42, 60, 78, 90, 96, 120, 135,
	};
	u16 thr;

	if (ieee80211_is_data_qos(hdr->frame_control))
		ac = tid_to_mac80211_ac[ieee80211_get_tid(hdr)];

	mvmsta = iwl_mvm_sta_from_mac80211(sta);
	mac = mvmsta->mac_id_n_color & FW_CTXT_ID_MSK;

	if (time_after(jiffies, mvm->tcm.ts + MVM_TCM_PERIOD))
		schedule_delayed_work(&mvm->tcm.work, 0);
	mdata = &mvm->tcm.data[mac];
	mdata->rx.pkts[ac]++;

	/* count the airtime only once for each ampdu */
	if (mdata->rx.last_ampdu_ref != mvm->ampdu_ref) {
		mdata->rx.last_ampdu_ref = mvm->ampdu_ref;
		mdata->rx.airtime += le16_to_cpu(phy_info->frame_time);
	}

	if (!(rate_n_flags & (RATE_MCS_HT_MSK | RATE_MCS_VHT_MSK)))
		return;

	mvmvif = iwl_mvm_vif_from_mac80211(mvmsta->vif);

	if (mdata->opened_rx_ba_sessions ||
	    mdata->uapsd_nonagg_detect.detected ||
	    (!mvmvif->queue_params[IEEE80211_AC_VO].uapsd &&
	     !mvmvif->queue_params[IEEE80211_AC_VI].uapsd &&
	     !mvmvif->queue_params[IEEE80211_AC_BE].uapsd &&
	     !mvmvif->queue_params[IEEE80211_AC_BK].uapsd) ||
	    mvmsta->sta_id != mvmvif->ap_sta_id)
		return;

	if (rate_n_flags & RATE_MCS_HT_MSK) {
		thr = thresh_tpt[rate_n_flags & RATE_HT_MCS_RATE_CODE_MSK];
		thr *= 1 + ((rate_n_flags & RATE_HT_MCS_NSS_MSK) >>
					RATE_HT_MCS_NSS_POS);
	} else {
		if (WARN_ON((rate_n_flags & RATE_VHT_MCS_RATE_CODE_MSK) >=
				ARRAY_SIZE(thresh_tpt)))
			return;
		thr = thresh_tpt[rate_n_flags & RATE_VHT_MCS_RATE_CODE_MSK];
		thr *= 1 + ((rate_n_flags & RATE_VHT_MCS_NSS_MSK) >>
					RATE_VHT_MCS_NSS_POS);
	}

	thr <<= ((rate_n_flags & RATE_MCS_CHAN_WIDTH_MSK) >>
				RATE_MCS_CHAN_WIDTH_POS);

	mdata->uapsd_nonagg_detect.rx_bytes += len;
	ewma_rate_add(&mdata->uapsd_nonagg_detect.rate, thr);
}

static void iwl_mvm_rx_csum(struct ieee80211_sta *sta,
			    struct sk_buff *skb,
			    u32 status)
{
	struct iwl_mvm_sta *mvmsta = iwl_mvm_sta_from_mac80211(sta);
	struct iwl_mvm_vif *mvmvif = iwl_mvm_vif_from_mac80211(mvmsta->vif);

	if (mvmvif->features & NETIF_F_RXCSUM &&
	    status & RX_MPDU_RES_STATUS_CSUM_DONE &&
	    status & RX_MPDU_RES_STATUS_CSUM_OK)
		skb->ip_summed = CHECKSUM_UNNECESSARY;
}

/*
 * iwl_mvm_rx_rx_mpdu - REPLY_RX_MPDU_CMD handler
 *
 * Handles the actual data of the Rx packet from the fw
 */
void iwl_mvm_rx_rx_mpdu(struct iwl_mvm *mvm, struct napi_struct *napi,
			struct iwl_rx_cmd_buffer *rxb)
{
	struct ieee80211_hdr *hdr;
	struct ieee80211_rx_status *rx_status;
	struct iwl_rx_packet *pkt = rxb_addr(rxb);
	struct iwl_rx_phy_info *phy_info;
	struct iwl_rx_mpdu_res_start *rx_res;
	struct ieee80211_sta *sta = NULL;
	struct sk_buff *skb;
	u32 len, pkt_len = iwl_rx_packet_payload_len(pkt);
	u32 rate_n_flags;
	u32 rx_pkt_status;
	u8 crypt_len = 0;

	if (unlikely(pkt_len < sizeof(*rx_res))) {
		IWL_DEBUG_DROP(mvm, "Bad REPLY_RX_MPDU_CMD size\n");
		return;
	}

	phy_info = &mvm->last_phy_info;
	rx_res = (struct iwl_rx_mpdu_res_start *)pkt->data;
	hdr = (struct ieee80211_hdr *)(pkt->data + sizeof(*rx_res));
	len = le16_to_cpu(rx_res->byte_count);

	if (unlikely(len + sizeof(*rx_res) + sizeof(__le32) > pkt_len)) {
		IWL_DEBUG_DROP(mvm, "FW lied about packet len\n");
		return;
	}

	rx_pkt_status = get_unaligned_le32((__le32 *)
		(pkt->data + sizeof(*rx_res) + len));

	/* Dont use dev_alloc_skb(), we'll have enough headroom once
	 * ieee80211_hdr pulled.
	 */
	skb = alloc_skb(128, GFP_ATOMIC);
	if (!skb) {
		IWL_ERR(mvm, "alloc_skb failed\n");
		return;
	}

	rx_status = IEEE80211_SKB_RXCB(skb);

	/*
	 * drop the packet if it has failed being decrypted by HW
	 */
	if (iwl_mvm_set_mac80211_rx_flag(mvm, hdr, rx_status, rx_pkt_status,
					 &crypt_len)) {
		IWL_DEBUG_DROP(mvm, "Bad decryption results 0x%08x\n",
			       rx_pkt_status);
		kfree_skb(skb);
		return;
	}

	/*
	 * Keep packets with CRC errors (and with overrun) for monitor mode
	 * (otherwise the firmware discards them) but mark them as bad.
	 */
	if (!(rx_pkt_status & RX_MPDU_RES_STATUS_CRC_OK) ||
	    !(rx_pkt_status & RX_MPDU_RES_STATUS_OVERRUN_OK)) {
		IWL_DEBUG_RX(mvm, "Bad CRC or FIFO: 0x%08X.\n", rx_pkt_status);
		rx_status->flag |= RX_FLAG_FAILED_FCS_CRC;
	}

	/* This will be used in several places later */
	rate_n_flags = le32_to_cpu(phy_info->rate_n_flags);

	/* rx_status carries information about the packet to mac80211 */
	rx_status->mactime = le64_to_cpu(phy_info->timestamp);
	rx_status->device_timestamp = le32_to_cpu(phy_info->system_timestamp);
	rx_status->band =
		(phy_info->phy_flags & cpu_to_le16(RX_RES_PHY_FLAGS_BAND_24)) ?
				NL80211_BAND_2GHZ : NL80211_BAND_5GHZ;
	rx_status->freq =
		ieee80211_channel_to_frequency(le16_to_cpu(phy_info->channel),
					       rx_status->band);

	/* TSF as indicated by the firmware  is at INA time */
	rx_status->flag |= RX_FLAG_MACTIME_PLCP_START;

	iwl_mvm_get_signal_strength(mvm, phy_info, rx_status);

	IWL_DEBUG_STATS_LIMIT(mvm, "Rssi %d, TSF %llu\n", rx_status->signal,
			      (unsigned long long)rx_status->mactime);

	rcu_read_lock();
	if (rx_pkt_status & RX_MPDU_RES_STATUS_SRC_STA_FOUND) {
		u32 id = rx_pkt_status & RX_MPDU_RES_STATUS_STA_ID_MSK;

		id >>= RX_MDPU_RES_STATUS_STA_ID_SHIFT;

		if (!WARN_ON_ONCE(id >= mvm->fw->ucode_capa.num_stations)) {
			sta = rcu_dereference(mvm->fw_id_to_mac_id[id]);
			if (IS_ERR(sta))
				sta = NULL;
		}
	} else if (!is_multicast_ether_addr(hdr->addr2)) {
		/* This is fine since we prevent two stations with the same
		 * address from being added.
		 */
		sta = ieee80211_find_sta_by_ifaddr(mvm->hw, hdr->addr2, NULL);
	}

	if (sta) {
		struct iwl_mvm_sta *mvmsta = iwl_mvm_sta_from_mac80211(sta);
		struct ieee80211_vif *tx_blocked_vif =
			rcu_dereference(mvm->csa_tx_blocked_vif);
		struct iwl_fw_dbg_trigger_tlv *trig;
		struct ieee80211_vif *vif = mvmsta->vif;

		/* We have tx blocked stations (with CS bit). If we heard
		 * frames from a blocked station on a new channel we can
		 * TX to it again.
		 */
		if (unlikely(tx_blocked_vif) && vif == tx_blocked_vif) {
			struct iwl_mvm_vif *mvmvif =
				iwl_mvm_vif_from_mac80211(tx_blocked_vif);

			if (mvmvif->csa_target_freq == rx_status->freq)
				iwl_mvm_sta_modify_disable_tx_ap(mvm, sta,
								 false);
		}

		rs_update_last_rssi(mvm, mvmsta, rx_status);

		trig = iwl_fw_dbg_trigger_on(&mvm->fwrt,
					     ieee80211_vif_to_wdev(vif),
					     FW_DBG_TRIGGER_RSSI);

		if (trig && ieee80211_is_beacon(hdr->frame_control)) {
			struct iwl_fw_dbg_trigger_low_rssi *rssi_trig;
			s32 rssi;

			rssi_trig = (void *)trig->data;
			rssi = le32_to_cpu(rssi_trig->rssi);

			if (rx_status->signal < rssi)
				iwl_fw_dbg_collect_trig(&mvm->fwrt, trig,
							NULL);
		}

		if (!mvm->tcm.paused && len >= sizeof(*hdr) &&
		    !is_multicast_ether_addr(hdr->addr1) &&
		    ieee80211_is_data(hdr->frame_control))
			iwl_mvm_rx_handle_tcm(mvm, sta, hdr, len, phy_info,
					      rate_n_flags);

		if (ieee80211_is_data(hdr->frame_control))
			iwl_mvm_rx_csum(sta, skb, rx_pkt_status);
	}
	rcu_read_unlock();

	/* set the preamble flag if appropriate */
	if (phy_info->phy_flags & cpu_to_le16(RX_RES_PHY_FLAGS_SHORT_PREAMBLE))
		rx_status->enc_flags |= RX_ENC_FLAG_SHORTPRE;

	if (phy_info->phy_flags & cpu_to_le16(RX_RES_PHY_FLAGS_AGG)) {
		/*
		 * We know which subframes of an A-MPDU belong
		 * together since we get a single PHY response
		 * from the firmware for all of them
		 */
		rx_status->flag |= RX_FLAG_AMPDU_DETAILS;
		rx_status->ampdu_reference = mvm->ampdu_ref;
	}

	/* Set up the HT phy flags */
	switch (rate_n_flags & RATE_MCS_CHAN_WIDTH_MSK) {
	case RATE_MCS_CHAN_WIDTH_20:
		break;
	case RATE_MCS_CHAN_WIDTH_40:
		rx_status->bw = RATE_INFO_BW_40;
		break;
	case RATE_MCS_CHAN_WIDTH_80:
		rx_status->bw = RATE_INFO_BW_80;
		break;
	case RATE_MCS_CHAN_WIDTH_160:
		rx_status->bw = RATE_INFO_BW_160;
		break;
	}
	if (!(rate_n_flags & RATE_MCS_CCK_MSK) &&
	    rate_n_flags & RATE_MCS_SGI_MSK)
		rx_status->enc_flags |= RX_ENC_FLAG_SHORT_GI;
	if (rate_n_flags & RATE_HT_MCS_GF_MSK)
		rx_status->enc_flags |= RX_ENC_FLAG_HT_GF;
	if (rate_n_flags & RATE_MCS_LDPC_MSK)
		rx_status->enc_flags |= RX_ENC_FLAG_LDPC;
	if (rate_n_flags & RATE_MCS_HT_MSK) {
		u8 stbc = (rate_n_flags & RATE_MCS_STBC_MSK) >>
				RATE_MCS_STBC_POS;
		rx_status->encoding = RX_ENC_HT;
		rx_status->rate_idx = rate_n_flags & RATE_HT_MCS_INDEX_MSK;
		rx_status->enc_flags |= stbc << RX_ENC_FLAG_STBC_SHIFT;
	} else if (rate_n_flags & RATE_MCS_VHT_MSK) {
		u8 stbc = (rate_n_flags & RATE_MCS_STBC_MSK) >>
				RATE_MCS_STBC_POS;
		rx_status->nss =
			((rate_n_flags & RATE_VHT_MCS_NSS_MSK) >>
						RATE_VHT_MCS_NSS_POS) + 1;
		rx_status->rate_idx = rate_n_flags & RATE_VHT_MCS_RATE_CODE_MSK;
		rx_status->encoding = RX_ENC_VHT;
		rx_status->enc_flags |= stbc << RX_ENC_FLAG_STBC_SHIFT;
		if (rate_n_flags & RATE_MCS_BF_MSK)
			rx_status->enc_flags |= RX_ENC_FLAG_BF;
	} else {
		int rate = iwl_mvm_legacy_rate_to_mac80211_idx(rate_n_flags,
							       rx_status->band);

		if (WARN(rate < 0 || rate > 0xFF,
			 "Invalid rate flags 0x%x, band %d,\n",
			 rate_n_flags, rx_status->band)) {
			kfree_skb(skb);
			return;
		}
		rx_status->rate_idx = rate;
	}

#ifdef CONFIG_IWLWIFI_DEBUGFS
	iwl_mvm_update_frame_stats(mvm, rate_n_flags,
				   rx_status->flag & RX_FLAG_AMPDU_DETAILS);
#endif

	if (unlikely((ieee80211_is_beacon(hdr->frame_control) ||
		      ieee80211_is_probe_resp(hdr->frame_control)) &&
		     mvm->sched_scan_pass_all == SCHED_SCAN_PASS_ALL_ENABLED))
		mvm->sched_scan_pass_all = SCHED_SCAN_PASS_ALL_FOUND;

	if (unlikely(ieee80211_is_beacon(hdr->frame_control) ||
		     ieee80211_is_probe_resp(hdr->frame_control)))
		rx_status->boottime_ns = ktime_get_boottime_ns();

	iwl_mvm_pass_packet_to_mac80211(mvm, sta, napi, skb, hdr, len,
					crypt_len, rxb);
}

struct iwl_mvm_stat_data {
	struct iwl_mvm *mvm;
	__le32 flags;
	__le32 mac_id;
	u8 beacon_filter_average_energy;
	__le32 *beacon_counter;
	u8 *beacon_average_energy;
};

static void iwl_mvm_stat_iterator(void *_data, u8 *mac,
				  struct ieee80211_vif *vif)
{
	struct iwl_mvm_stat_data *data = _data;
	struct iwl_mvm *mvm = data->mvm;
	int sig = -data->beacon_filter_average_energy;
	int last_event;
	int thold = vif->bss_conf.cqm_rssi_thold;
	int hyst = vif->bss_conf.cqm_rssi_hyst;
	u16 id = le32_to_cpu(data->mac_id);
	struct iwl_mvm_vif *mvmvif = iwl_mvm_vif_from_mac80211(vif);
	u16 vif_id = mvmvif->id;

	/* This doesn't need the MAC ID check since it's not taking the
	 * data copied into the "data" struct, but rather the data from
	 * the notification directly.
	 */
	mvmvif->beacon_stats.num_beacons =
		le32_to_cpu(data->beacon_counter[vif_id]);
	mvmvif->beacon_stats.avg_signal =
		-data->beacon_average_energy[vif_id];

	/* make sure that beacon statistics don't go backwards with TCM
	 * request to clear statistics
	 */
	if (le32_to_cpu(data->flags) & IWL_STATISTICS_REPLY_FLG_CLEAR)
		mvmvif->beacon_stats.accu_num_beacons +=
			mvmvif->beacon_stats.num_beacons;

	if (mvmvif->id != id)
		return;

	if (vif->type != NL80211_IFTYPE_STATION)
		return;

	if (sig == 0) {
		IWL_DEBUG_RX(mvm, "RSSI is 0 - skip signal based decision\n");
		return;
	}

	mvmvif->bf_data.ave_beacon_signal = sig;

	/* BT Coex */
	if (mvmvif->bf_data.bt_coex_min_thold !=
	    mvmvif->bf_data.bt_coex_max_thold) {
		last_event = mvmvif->bf_data.last_bt_coex_event;
		if (sig > mvmvif->bf_data.bt_coex_max_thold &&
		    (last_event <= mvmvif->bf_data.bt_coex_min_thold ||
		     last_event == 0)) {
			mvmvif->bf_data.last_bt_coex_event = sig;
			IWL_DEBUG_RX(mvm, "cqm_iterator bt coex high %d\n",
				     sig);
			iwl_mvm_bt_rssi_event(mvm, vif, RSSI_EVENT_HIGH);
		} else if (sig < mvmvif->bf_data.bt_coex_min_thold &&
			   (last_event >= mvmvif->bf_data.bt_coex_max_thold ||
			    last_event == 0)) {
			mvmvif->bf_data.last_bt_coex_event = sig;
			IWL_DEBUG_RX(mvm, "cqm_iterator bt coex low %d\n",
				     sig);
			iwl_mvm_bt_rssi_event(mvm, vif, RSSI_EVENT_LOW);
		}
	}

	if (!(vif->driver_flags & IEEE80211_VIF_SUPPORTS_CQM_RSSI))
		return;

	/* CQM Notification */
	last_event = mvmvif->bf_data.last_cqm_event;
	if (thold && sig < thold && (last_event == 0 ||
				     sig < last_event - hyst)) {
		mvmvif->bf_data.last_cqm_event = sig;
		IWL_DEBUG_RX(mvm, "cqm_iterator cqm low %d\n",
			     sig);
		ieee80211_cqm_rssi_notify(
			vif,
			NL80211_CQM_RSSI_THRESHOLD_EVENT_LOW,
			sig,
			GFP_KERNEL);
	} else if (sig > thold &&
		   (last_event == 0 || sig > last_event + hyst)) {
		mvmvif->bf_data.last_cqm_event = sig;
		IWL_DEBUG_RX(mvm, "cqm_iterator cqm high %d\n",
			     sig);
		ieee80211_cqm_rssi_notify(
			vif,
			NL80211_CQM_RSSI_THRESHOLD_EVENT_HIGH,
			sig,
			GFP_KERNEL);
	}
}

static inline void
iwl_mvm_rx_stats_check_trigger(struct iwl_mvm *mvm, struct iwl_rx_packet *pkt)
{
	struct iwl_fw_dbg_trigger_tlv *trig;
	struct iwl_fw_dbg_trigger_stats *trig_stats;
	u32 trig_offset, trig_thold;

	trig = iwl_fw_dbg_trigger_on(&mvm->fwrt, NULL, FW_DBG_TRIGGER_STATS);
	if (!trig)
		return;

	trig_stats = (void *)trig->data;

	trig_offset = le32_to_cpu(trig_stats->stop_offset);
	trig_thold = le32_to_cpu(trig_stats->stop_threshold);

	if (WARN_ON_ONCE(trig_offset >= iwl_rx_packet_payload_len(pkt)))
		return;

	if (le32_to_cpup((__le32 *) (pkt->data + trig_offset)) < trig_thold)
		return;

	iwl_fw_dbg_collect_trig(&mvm->fwrt, trig, NULL);
}

static void iwl_mvm_stats_energy_iter(void *_data,
				      struct ieee80211_sta *sta)
{
	struct iwl_mvm_sta *mvmsta = iwl_mvm_sta_from_mac80211(sta);
	u8 *energy = _data;
	u32 sta_id = mvmsta->sta_id;

	if (WARN_ONCE(sta_id >= IWL_MVM_STATION_COUNT_MAX, "sta_id %d >= %d",
		      sta_id, IWL_MVM_STATION_COUNT_MAX))
		return;

	if (energy[sta_id])
		mvmsta->avg_energy = energy[sta_id];

}

static void
iwl_mvm_update_tcm_from_stats(struct iwl_mvm *mvm, __le32 *air_time_le,
			      __le32 *rx_bytes_le)
{
	int i;

	spin_lock(&mvm->tcm.lock);
	for (i = 0; i < NUM_MAC_INDEX_DRIVER; i++) {
		struct iwl_mvm_tcm_mac *mdata = &mvm->tcm.data[i];
		u32 rx_bytes = le32_to_cpu(rx_bytes_le[i]);
		u32 airtime = le32_to_cpu(air_time_le[i]);

		mdata->rx.airtime += airtime;
		mdata->uapsd_nonagg_detect.rx_bytes += rx_bytes;
		if (airtime) {
			/* re-init every time to store rate from FW */
			ewma_rate_init(&mdata->uapsd_nonagg_detect.rate);
			ewma_rate_add(&mdata->uapsd_nonagg_detect.rate,
				      rx_bytes * 8 / airtime);
		}
	}
	spin_unlock(&mvm->tcm.lock);
}

static void
iwl_mvm_handle_rx_statistics_tlv(struct iwl_mvm *mvm,
				 struct iwl_rx_packet *pkt)
{
	struct iwl_mvm_stat_data data = {
		.mvm = mvm,
	};
	u8 beacon_average_energy[MAC_INDEX_AUX];
	u8 average_energy[IWL_MVM_STATION_COUNT_MAX];
	struct iwl_statistics_operational_ntfy *stats;
	int expected_size;
	__le32 flags;
	int i;

	expected_size = sizeof(*stats);
	if (WARN_ONCE(iwl_rx_packet_payload_len(pkt) < expected_size,
		      "received invalid statistics size (%d)!, expected_size: %d\n",
		      iwl_rx_packet_payload_len(pkt), expected_size))
		return;

	stats = (void *)&pkt->data;

	if (WARN_ONCE(stats->hdr.type != FW_STATISTICS_OPERATIONAL ||
		      stats->hdr.version !=
		      iwl_fw_lookup_notif_ver(mvm->fw, LONG_GROUP, STATISTICS_CMD, 0),
		      "received unsupported hdr type %d, version %d\n",
		      stats->hdr.type, stats->hdr.version))
		return;

	flags = stats->flags;
	mvm->radio_stats.rx_time = le64_to_cpu(stats->rx_time);
	mvm->radio_stats.tx_time = le64_to_cpu(stats->tx_time);
	mvm->radio_stats.on_time_rf = le64_to_cpu(stats->on_time_rf);
	mvm->radio_stats.on_time_scan = le64_to_cpu(stats->on_time_scan);

	iwl_mvm_rx_stats_check_trigger(mvm, pkt);

	data.mac_id = stats->mac_id;
	data.beacon_filter_average_energy =
		le32_to_cpu(stats->beacon_filter_average_energy);
	data.flags = flags;
	data.beacon_counter = stats->beacon_counter;
	for (i = 0; i < ARRAY_SIZE(beacon_average_energy); i++)
		beacon_average_energy[i] =
			le32_to_cpu(stats->beacon_average_energy[i]);

	data.beacon_average_energy = beacon_average_energy;

	ieee80211_iterate_active_interfaces(mvm->hw,
					    IEEE80211_IFACE_ITER_NORMAL,
					    iwl_mvm_stat_iterator,
					    &data);

	for (i = 0; i < ARRAY_SIZE(average_energy); i++)
		average_energy[i] = le32_to_cpu(stats->average_energy[i]);
	ieee80211_iterate_stations_atomic(mvm->hw, iwl_mvm_stats_energy_iter,
					  average_energy);
	/*
	 * Don't update in case the statistics are not cleared, since
	 * we will end up counting twice the same airtime, once in TCM
	 * request and once in statistics notification.
	 */
	if (le32_to_cpu(flags) & IWL_STATISTICS_REPLY_FLG_CLEAR)
		iwl_mvm_update_tcm_from_stats(mvm, stats->air_time,
					      stats->rx_bytes);
}

void iwl_mvm_handle_rx_statistics(struct iwl_mvm *mvm,
				  struct iwl_rx_packet *pkt)
{
	struct iwl_mvm_stat_data data = {
		.mvm = mvm,
	};
	__le32 *bytes, *air_time, flags;
	int expected_size;
	u8 *energy;

	/* From ver 14 and up we use TLV statistics format */
	if (iwl_fw_lookup_notif_ver(mvm->fw, LONG_GROUP,
				    STATISTICS_CMD, 0) >= 14)
		return iwl_mvm_handle_rx_statistics_tlv(mvm, pkt);

	if (!iwl_mvm_has_new_rx_stats_api(mvm)) {
		if (iwl_mvm_has_new_rx_api(mvm))
			expected_size = sizeof(struct iwl_notif_statistics_v11);
		else
			expected_size = sizeof(struct iwl_notif_statistics_v10);
	} else {
		expected_size = sizeof(struct iwl_notif_statistics);
	}

	if (WARN_ONCE(iwl_rx_packet_payload_len(pkt) != expected_size,
		      "received invalid statistics size (%d)!\n",
		      iwl_rx_packet_payload_len(pkt)))
		return;

	if (!iwl_mvm_has_new_rx_stats_api(mvm)) {
		struct iwl_notif_statistics_v11 *stats = (void *)&pkt->data;

		data.mac_id = stats->rx.general.mac_id;
		data.beacon_filter_average_energy =
			stats->general.common.beacon_filter_average_energy;

		mvm->rx_stats_v3 = stats->rx;

		mvm->radio_stats.rx_time =
			le64_to_cpu(stats->general.common.rx_time);
		mvm->radio_stats.tx_time =
			le64_to_cpu(stats->general.common.tx_time);
		mvm->radio_stats.on_time_rf =
			le64_to_cpu(stats->general.common.on_time_rf);
		mvm->radio_stats.on_time_scan =
			le64_to_cpu(stats->general.common.on_time_scan);

		data.beacon_counter = stats->general.beacon_counter;
		data.beacon_average_energy =
			stats->general.beacon_average_energy;
		flags = stats->flag;
	} else {
		struct iwl_notif_statistics *stats = (void *)&pkt->data;

		data.mac_id = stats->rx.general.mac_id;
		data.beacon_filter_average_energy =
			stats->general.common.beacon_filter_average_energy;

		mvm->rx_stats = stats->rx;

		mvm->radio_stats.rx_time =
			le64_to_cpu(stats->general.common.rx_time);
		mvm->radio_stats.tx_time =
			le64_to_cpu(stats->general.common.tx_time);
		mvm->radio_stats.on_time_rf =
			le64_to_cpu(stats->general.common.on_time_rf);
		mvm->radio_stats.on_time_scan =
			le64_to_cpu(stats->general.common.on_time_scan);

		data.beacon_counter = stats->general.beacon_counter;
		data.beacon_average_energy =
			stats->general.beacon_average_energy;
		flags = stats->flag;
	}
	data.flags = flags;

	iwl_mvm_rx_stats_check_trigger(mvm, pkt);

	ieee80211_iterate_active_interfaces(mvm->hw,
					    IEEE80211_IFACE_ITER_NORMAL,
					    iwl_mvm_stat_iterator,
					    &data);

	if (!iwl_mvm_has_new_rx_api(mvm))
		return;

	if (!iwl_mvm_has_new_rx_stats_api(mvm)) {
		struct iwl_notif_statistics_v11 *v11 = (void *)&pkt->data;

		energy = (void *)&v11->load_stats.avg_energy;
		bytes = (void *)&v11->load_stats.byte_count;
		air_time = (void *)&v11->load_stats.air_time;
	} else {
		struct iwl_notif_statistics *stats = (void *)&pkt->data;

		energy = (void *)&stats->load_stats.avg_energy;
		bytes = (void *)&stats->load_stats.byte_count;
		air_time = (void *)&stats->load_stats.air_time;
	}
	ieee80211_iterate_stations_atomic(mvm->hw, iwl_mvm_stats_energy_iter,
					  energy);

	/*
	 * Don't update in case the statistics are not cleared, since
	 * we will end up counting twice the same airtime, once in TCM
	 * request and once in statistics notification.
	 */
	if (le32_to_cpu(flags) & IWL_STATISTICS_REPLY_FLG_CLEAR)
		iwl_mvm_update_tcm_from_stats(mvm, air_time, bytes);

}

void iwl_mvm_rx_statistics(struct iwl_mvm *mvm, struct iwl_rx_cmd_buffer *rxb)
{
	iwl_mvm_handle_rx_statistics(mvm, rxb_addr(rxb));
}

void iwl_mvm_window_status_notif(struct iwl_mvm *mvm,
				 struct iwl_rx_cmd_buffer *rxb)
{
	struct iwl_rx_packet *pkt = rxb_addr(rxb);
	struct iwl_ba_window_status_notif *notif = (void *)pkt->data;
	int i;

	BUILD_BUG_ON(ARRAY_SIZE(notif->ra_tid) != BA_WINDOW_STREAMS_MAX);
	BUILD_BUG_ON(ARRAY_SIZE(notif->mpdu_rx_count) != BA_WINDOW_STREAMS_MAX);
	BUILD_BUG_ON(ARRAY_SIZE(notif->bitmap) != BA_WINDOW_STREAMS_MAX);
	BUILD_BUG_ON(ARRAY_SIZE(notif->start_seq_num) != BA_WINDOW_STREAMS_MAX);

	rcu_read_lock();
	for (i = 0; i < BA_WINDOW_STREAMS_MAX; i++) {
		struct ieee80211_sta *sta;
		u8 sta_id, tid;
		u64 bitmap;
		u32 ssn;
		u16 ratid;
		u16 received_mpdu;

		ratid = le16_to_cpu(notif->ra_tid[i]);
		/* check that this TID is valid */
		if (!(ratid & BA_WINDOW_STATUS_VALID_MSK))
			continue;

		received_mpdu = le16_to_cpu(notif->mpdu_rx_count[i]);
		if (received_mpdu == 0)
			continue;

		tid = ratid & BA_WINDOW_STATUS_TID_MSK;
		/* get the station */
		sta_id = (ratid & BA_WINDOW_STATUS_STA_ID_MSK)
			 >> BA_WINDOW_STATUS_STA_ID_POS;
		sta = rcu_dereference(mvm->fw_id_to_mac_id[sta_id]);
		if (IS_ERR_OR_NULL(sta))
			continue;
		bitmap = le64_to_cpu(notif->bitmap[i]);
		ssn = le32_to_cpu(notif->start_seq_num[i]);

		/* update mac80211 with the bitmap for the reordering buffer */
		ieee80211_mark_rx_ba_filtered_frames(sta, tid, ssn, bitmap,
						     received_mpdu);
	}
	rcu_read_unlock();
}
