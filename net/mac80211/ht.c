// SPDX-License-Identifier: GPL-2.0-only
/*
 * HT handling
 *
 * Copyright 2003, Jouni Malinen <jkmaline@cc.hut.fi>
 * Copyright 2002-2005, Instant802 Networks, Inc.
 * Copyright 2005-2006, Devicescape Software, Inc.
 * Copyright 2006-2007	Jiri Benc <jbenc@suse.cz>
 * Copyright 2007, Michael Wu <flamingice@sourmilk.net>
 * Copyright 2007-2010, Intel Corporation
 * Copyright 2017	Intel Deutschland GmbH
 * Copyright(c) 2020 Intel Corporation
 */

#include <linux/ieee80211.h>
#include <linux/export.h>
#include <net/mac80211.h>
#include "ieee80211_i.h"
#include "rate.h"

static void __check_htcap_disable(struct ieee80211_ht_cap *ht_capa,
				  struct ieee80211_ht_cap *ht_capa_mask,
				  struct ieee80211_sta_ht_cap *ht_cap,
				  u16 flag)
{
	__le16 le_flag = cpu_to_le16(flag);
	if (ht_capa_mask->cap_info & le_flag) {
		if (!(ht_capa->cap_info & le_flag))
			ht_cap->cap &= ~flag;
	}
}

static void __check_htcap_enable(struct ieee80211_ht_cap *ht_capa,
				  struct ieee80211_ht_cap *ht_capa_mask,
				  struct ieee80211_sta_ht_cap *ht_cap,
				  u16 flag)
{
	__le16 le_flag = cpu_to_le16(flag);

	if ((ht_capa_mask->cap_info & le_flag) &&
	    (ht_capa->cap_info & le_flag))
		ht_cap->cap |= flag;
}

void ieee80211_apply_htcap_overrides(struct ieee80211_sub_if_data *sdata,
				     struct ieee80211_sta_ht_cap *ht_cap)
{
	struct ieee80211_ht_cap *ht_capa, *ht_capa_mask;
	u8 *scaps, *smask;
	int i;

	if (!ht_cap->ht_supported)
		return;

	switch (sdata->vif.type) {
	case NL80211_IFTYPE_STATION:
		ht_capa = &sdata->u.mgd.ht_capa;
		ht_capa_mask = &sdata->u.mgd.ht_capa_mask;
		break;
	case NL80211_IFTYPE_ADHOC:
		ht_capa = &sdata->u.ibss.ht_capa;
		ht_capa_mask = &sdata->u.ibss.ht_capa_mask;
		break;
	default:
		WARN_ON_ONCE(1);
		return;
	}

	scaps = (u8 *)(&ht_capa->mcs.rx_mask);
	smask = (u8 *)(&ht_capa_mask->mcs.rx_mask);

	/* NOTE:  If you add more over-rides here, update register_hw
	 * ht_capa_mod_mask logic in main.c as well.
	 * And, if this method can ever change ht_cap.ht_supported, fix
	 * the check in ieee80211_add_ht_ie.
	 */

	/* check for HT over-rides, MCS rates first. */
	for (i = 0; i < IEEE80211_HT_MCS_MASK_LEN; i++) {
		u8 m = smask[i];
		ht_cap->mcs.rx_mask[i] &= ~m; /* turn off all masked bits */
		/* Add back rates that are supported */
		ht_cap->mcs.rx_mask[i] |= (m & scaps[i]);
	}

	/* Force removal of HT-40 capabilities? */
	__check_htcap_disable(ht_capa, ht_capa_mask, ht_cap,
			      IEEE80211_HT_CAP_SUP_WIDTH_20_40);
	__check_htcap_disable(ht_capa, ht_capa_mask, ht_cap,
			      IEEE80211_HT_CAP_SGI_40);

	/* Allow user to disable SGI-20 (SGI-40 is handled above) */
	__check_htcap_disable(ht_capa, ht_capa_mask, ht_cap,
			      IEEE80211_HT_CAP_SGI_20);

	/* Allow user to disable the max-AMSDU bit. */
	__check_htcap_disable(ht_capa, ht_capa_mask, ht_cap,
			      IEEE80211_HT_CAP_MAX_AMSDU);

	/* Allow user to disable LDPC */
	__check_htcap_disable(ht_capa, ht_capa_mask, ht_cap,
			      IEEE80211_HT_CAP_LDPC_CODING);

	/* Allow user to enable 40 MHz intolerant bit. */
	__check_htcap_enable(ht_capa, ht_capa_mask, ht_cap,
			     IEEE80211_HT_CAP_40MHZ_INTOLERANT);

	/* Allow user to enable TX STBC bit  */
	__check_htcap_enable(ht_capa, ht_capa_mask, ht_cap,
			     IEEE80211_HT_CAP_TX_STBC);

	/* Allow user to configure RX STBC bits */
	if (ht_capa_mask->cap_info & cpu_to_le16(IEEE80211_HT_CAP_RX_STBC))
		ht_cap->cap |= le16_to_cpu(ht_capa->cap_info) &
					IEEE80211_HT_CAP_RX_STBC;

	/* Allow user to decrease AMPDU factor */
	if (ht_capa_mask->ampdu_params_info &
	    IEEE80211_HT_AMPDU_PARM_FACTOR) {
		u8 n = ht_capa->ampdu_params_info &
		       IEEE80211_HT_AMPDU_PARM_FACTOR;
		if (n < ht_cap->ampdu_factor)
			ht_cap->ampdu_factor = n;
	}

	/* Allow the user to increase AMPDU density. */
	if (ht_capa_mask->ampdu_params_info &
	    IEEE80211_HT_AMPDU_PARM_DENSITY) {
		u8 n = (ht_capa->ampdu_params_info &
			IEEE80211_HT_AMPDU_PARM_DENSITY)
			>> IEEE80211_HT_AMPDU_PARM_DENSITY_SHIFT;
		if (n > ht_cap->ampdu_density)
			ht_cap->ampdu_density = n;
	}
}


bool ieee80211_ht_cap_ie_to_sta_ht_cap(struct ieee80211_sub_if_data *sdata,
				       struct ieee80211_supported_band *sband,
				       const struct ieee80211_ht_cap *ht_cap_ie,
				       struct sta_info *sta)
{
	struct ieee80211_sta_ht_cap ht_cap, own_cap;
	u8 ampdu_info, tx_mcs_set_cap;
	int i, max_tx_streams;
	bool changed;
	enum ieee80211_sta_rx_bandwidth bw;

	memset(&ht_cap, 0, sizeof(ht_cap));

	if (!ht_cap_ie || !sband->ht_cap.ht_supported)
		goto apply;

	ht_cap.ht_supported = true;

	own_cap = sband->ht_cap;

	/*
	 * If user has specified capability over-rides, take care
	 * of that if the station we're setting up is the AP or TDLS peer that
	 * we advertised a restricted capability set to. Override
	 * our own capabilities and then use those below.
	 */
	if (sdata->vif.type == NL80211_IFTYPE_STATION ||
	    sdata->vif.type == NL80211_IFTYPE_ADHOC)
		ieee80211_apply_htcap_overrides(sdata, &own_cap);

	/*
	 * The bits listed in this expression should be
	 * the same for the peer and us, if the station
	 * advertises more then we can't use those thus
	 * we mask them out.
	 */
	ht_cap.cap = le16_to_cpu(ht_cap_ie->cap_info) &
		(own_cap.cap | ~(IEEE80211_HT_CAP_LDPC_CODING |
				 IEEE80211_HT_CAP_SUP_WIDTH_20_40 |
				 IEEE80211_HT_CAP_GRN_FLD |
				 IEEE80211_HT_CAP_SGI_20 |
				 IEEE80211_HT_CAP_SGI_40 |
				 IEEE80211_HT_CAP_DSSSCCK40));

	/*
	 * The STBC bits are asymmetric -- if we don't have
	 * TX then mask out the peer's RX and vice versa.
	 */
	if (!(own_cap.cap & IEEE80211_HT_CAP_TX_STBC))
		ht_cap.cap &= ~IEEE80211_HT_CAP_RX_STBC;
	if (!(own_cap.cap & IEEE80211_HT_CAP_RX_STBC))
		ht_cap.cap &= ~IEEE80211_HT_CAP_TX_STBC;

	ampdu_info = ht_cap_ie->ampdu_params_info;
	ht_cap.ampdu_factor =
		ampdu_info & IEEE80211_HT_AMPDU_PARM_FACTOR;
	ht_cap.ampdu_density =
		(ampdu_info & IEEE80211_HT_AMPDU_PARM_DENSITY) >> 2;

	/* own MCS TX capabilities */
	tx_mcs_set_cap = own_cap.mcs.tx_params;

	/* Copy peer MCS TX capabilities, the driver might need them. */
	ht_cap.mcs.tx_params = ht_cap_ie->mcs.tx_params;

	/* can we TX with MCS rates? */
	if (!(tx_mcs_set_cap & IEEE80211_HT_MCS_TX_DEFINED))
		goto apply;

	/* Counting from 0, therefore +1 */
	if (tx_mcs_set_cap & IEEE80211_HT_MCS_TX_RX_DIFF)
		max_tx_streams =
			((tx_mcs_set_cap & IEEE80211_HT_MCS_TX_MAX_STREAMS_MASK)
				>> IEEE80211_HT_MCS_TX_MAX_STREAMS_SHIFT) + 1;
	else
		max_tx_streams = IEEE80211_HT_MCS_TX_MAX_STREAMS;

	/*
	 * 802.11n-2009 20.3.5 / 20.6 says:
	 * - indices 0 to 7 and 32 are single spatial stream
	 * - 8 to 31 are multiple spatial streams using equal modulation
	 *   [8..15 for two streams, 16..23 for three and 24..31 for four]
	 * - remainder are multiple spatial streams using unequal modulation
	 */
	for (i = 0; i < max_tx_streams; i++)
		ht_cap.mcs.rx_mask[i] =
			own_cap.mcs.rx_mask[i] & ht_cap_ie->mcs.rx_mask[i];

	if (tx_mcs_set_cap & IEEE80211_HT_MCS_TX_UNEQUAL_MODULATION)
		for (i = IEEE80211_HT_MCS_UNEQUAL_MODULATION_START_BYTE;
		     i < IEEE80211_HT_MCS_MASK_LEN; i++)
			ht_cap.mcs.rx_mask[i] =
				own_cap.mcs.rx_mask[i] &
					ht_cap_ie->mcs.rx_mask[i];

	/* handle MCS rate 32 too */
	if (own_cap.mcs.rx_mask[32/8] & ht_cap_ie->mcs.rx_mask[32/8] & 1)
		ht_cap.mcs.rx_mask[32/8] |= 1;

	/* set Rx highest rate */
	ht_cap.mcs.rx_highest = ht_cap_ie->mcs.rx_highest;

	if (ht_cap.cap & IEEE80211_HT_CAP_MAX_AMSDU)
		sta->sta.max_amsdu_len = IEEE80211_MAX_MPDU_LEN_HT_7935;
	else
		sta->sta.max_amsdu_len = IEEE80211_MAX_MPDU_LEN_HT_3839;

 apply:
	changed = memcmp(&sta->sta.ht_cap, &ht_cap, sizeof(ht_cap));

	memcpy(&sta->sta.ht_cap, &ht_cap, sizeof(ht_cap));

	switch (sdata->vif.bss_conf.chandef.width) {
	default:
		WARN_ON_ONCE(1);
		fallthrough;
	case NL80211_CHAN_WIDTH_20_NOHT:
	case NL80211_CHAN_WIDTH_20:
		bw = IEEE80211_STA_RX_BW_20;
		break;
	case NL80211_CHAN_WIDTH_40:
	case NL80211_CHAN_WIDTH_80:
	case NL80211_CHAN_WIDTH_80P80:
	case NL80211_CHAN_WIDTH_160:
		bw = ht_cap.cap & IEEE80211_HT_CAP_SUP_WIDTH_20_40 ?
				IEEE80211_STA_RX_BW_40 : IEEE80211_STA_RX_BW_20;
		break;
	}

	sta->sta.bandwidth = bw;

	sta->cur_max_bandwidth =
		ht_cap.cap & IEEE80211_HT_CAP_SUP_WIDTH_20_40 ?
				IEEE80211_STA_RX_BW_40 : IEEE80211_STA_RX_BW_20;

	if (sta->sdata->vif.type == NL80211_IFTYPE_AP ||
	    sta->sdata->vif.type == NL80211_IFTYPE_AP_VLAN) {
		enum ieee80211_smps_mode smps_mode;

		switch ((ht_cap.cap & IEEE80211_HT_CAP_SM_PS)
				>> IEEE80211_HT_CAP_SM_PS_SHIFT) {
		case WLAN_HT_CAP_SM_PS_INVALID:
		case WLAN_HT_CAP_SM_PS_STATIC:
			smps_mode = IEEE80211_SMPS_STATIC;
			break;
		case WLAN_HT_CAP_SM_PS_DYNAMIC:
			smps_mode = IEEE80211_SMPS_DYNAMIC;
			break;
		case WLAN_HT_CAP_SM_PS_DISABLED:
			smps_mode = IEEE80211_SMPS_OFF;
			break;
		}

		if (smps_mode != sta->sta.smps_mode)
			changed = true;
		sta->sta.smps_mode = smps_mode;
	} else {
		sta->sta.smps_mode = IEEE80211_SMPS_OFF;
	}
	return changed;
}

void ieee80211_sta_tear_down_BA_sessions(struct sta_info *sta,
					 enum ieee80211_agg_stop_reason reason)
{
	int i;

	mutex_lock(&sta->ampdu_mlme.mtx);
	for (i = 0; i <  IEEE80211_NUM_TIDS; i++)
		___ieee80211_stop_rx_ba_session(sta, i, WLAN_BACK_RECIPIENT,
						WLAN_REASON_QSTA_LEAVE_QBSS,
						reason != AGG_STOP_DESTROY_STA &&
						reason != AGG_STOP_PEER_REQUEST);

	for (i = 0; i <  IEEE80211_NUM_TIDS; i++)
		___ieee80211_stop_tx_ba_session(sta, i, reason);
	mutex_unlock(&sta->ampdu_mlme.mtx);

	/*
	 * In case the tear down is part of a reconfigure due to HW restart
	 * request, it is possible that the low level driver requested to stop
	 * the BA session, so handle it to properly clean tid_tx data.
	 */
	if(reason == AGG_STOP_DESTROY_STA) {
		cancel_work_sync(&sta->ampdu_mlme.work);

		mutex_lock(&sta->ampdu_mlme.mtx);
		for (i = 0; i < IEEE80211_NUM_TIDS; i++) {
			struct tid_ampdu_tx *tid_tx =
				rcu_dereference_protected_tid_tx(sta, i);

			if (!tid_tx)
				continue;

			if (test_and_clear_bit(HT_AGG_STATE_STOP_CB, &tid_tx->state))
				ieee80211_stop_tx_ba_cb(sta, i, tid_tx);
		}
		mutex_unlock(&sta->ampdu_mlme.mtx);
	}
}

void ieee80211_ba_session_work(struct work_struct *work)
{
	struct sta_info *sta =
		container_of(work, struct sta_info, ampdu_mlme.work);
	struct tid_ampdu_tx *tid_tx;
	bool blocked;
	int tid;

	/* When this flag is set, new sessions should be blocked. */
	blocked = test_sta_flag(sta, WLAN_STA_BLOCK_BA);

	mutex_lock(&sta->ampdu_mlme.mtx);
	for (tid = 0; tid < IEEE80211_NUM_TIDS; tid++) {
		if (test_and_clear_bit(tid, sta->ampdu_mlme.tid_rx_timer_expired))
			___ieee80211_stop_rx_ba_session(
				sta, tid, WLAN_BACK_RECIPIENT,
				WLAN_REASON_QSTA_TIMEOUT, true);

		if (test_and_clear_bit(tid,
				       sta->ampdu_mlme.tid_rx_stop_requested))
			___ieee80211_stop_rx_ba_session(
				sta, tid, WLAN_BACK_RECIPIENT,
				WLAN_REASON_UNSPECIFIED, true);

		if (!blocked &&
		    test_and_clear_bit(tid,
				       sta->ampdu_mlme.tid_rx_manage_offl))
			___ieee80211_start_rx_ba_session(sta, 0, 0, 0, 1, tid,
							 IEEE80211_MAX_AMPDU_BUF_HT,
							 false, true, NULL);

		if (test_and_clear_bit(tid + IEEE80211_NUM_TIDS,
				       sta->ampdu_mlme.tid_rx_manage_offl))
			___ieee80211_stop_rx_ba_session(
				sta, tid, WLAN_BACK_RECIPIENT,
				0, false);

		spin_lock_bh(&sta->lock);

		tid_tx = sta->ampdu_mlme.tid_start_tx[tid];
		if (!blocked && tid_tx) {
			/*
			 * Assign it over to the normal tid_tx array
			 * where it "goes live".
			 */

			sta->ampdu_mlme.tid_start_tx[tid] = NULL;
			/* could there be a race? */
			if (sta->ampdu_mlme.tid_tx[tid])
				kfree(tid_tx);
			else
				ieee80211_assign_tid_tx(sta, tid, tid_tx);
			spin_unlock_bh(&sta->lock);

			ieee80211_tx_ba_session_handle_start(sta, tid);
			continue;
		}
		spin_unlock_bh(&sta->lock);

		tid_tx = rcu_dereference_protected_tid_tx(sta, tid);
		if (!tid_tx)
			continue;

		if (!blocked &&
		    test_and_clear_bit(HT_AGG_STATE_START_CB, &tid_tx->state))
			ieee80211_start_tx_ba_cb(sta, tid, tid_tx);
		if (test_and_clear_bit(HT_AGG_STATE_WANT_STOP, &tid_tx->state))
			___ieee80211_stop_tx_ba_session(sta, tid,
							AGG_STOP_LOCAL_REQUEST);
		if (test_and_clear_bit(HT_AGG_STATE_STOP_CB, &tid_tx->state))
			ieee80211_stop_tx_ba_cb(sta, tid, tid_tx);
	}
	mutex_unlock(&sta->ampdu_mlme.mtx);
}

void ieee80211_send_delba(struct ieee80211_sub_if_data *sdata,
			  const u8 *da, u16 tid,
			  u16 initiator, u16 reason_code)
{
	struct ieee80211_local *local = sdata->local;
	struct sk_buff *skb;
	struct ieee80211_mgmt *mgmt;
	u16 params;

	skb = dev_alloc_skb(sizeof(*mgmt) + local->hw.extra_tx_headroom);
	if (!skb)
		return;

	skb_reserve(skb, local->hw.extra_tx_headroom);
	mgmt = skb_put_zero(skb, 24);
	memcpy(mgmt->da, da, ETH_ALEN);
	memcpy(mgmt->sa, sdata->vif.addr, ETH_ALEN);
	if (sdata->vif.type == NL80211_IFTYPE_AP ||
	    sdata->vif.type == NL80211_IFTYPE_AP_VLAN ||
	    sdata->vif.type == NL80211_IFTYPE_MESH_POINT)
		memcpy(mgmt->bssid, sdata->vif.addr, ETH_ALEN);
	else if (sdata->vif.type == NL80211_IFTYPE_STATION)
		memcpy(mgmt->bssid, sdata->u.mgd.bssid, ETH_ALEN);
	else if (sdata->vif.type == NL80211_IFTYPE_ADHOC)
		memcpy(mgmt->bssid, sdata->u.ibss.bssid, ETH_ALEN);

	mgmt->frame_control = cpu_to_le16(IEEE80211_FTYPE_MGMT |
					  IEEE80211_STYPE_ACTION);

	skb_put(skb, 1 + sizeof(mgmt->u.action.u.delba));

	mgmt->u.action.category = WLAN_CATEGORY_BACK;
	mgmt->u.action.u.delba.action_code = WLAN_ACTION_DELBA;
	params = (u16)(initiator << 11); 	/* bit 11 initiator */
	params |= (u16)(tid << 12); 		/* bit 15:12 TID number */

	mgmt->u.action.u.delba.params = cpu_to_le16(params);
	mgmt->u.action.u.delba.reason_code = cpu_to_le16(reason_code);

	ieee80211_tx_skb(sdata, skb);
}

void ieee80211_process_delba(struct ieee80211_sub_if_data *sdata,
			     struct sta_info *sta,
			     struct ieee80211_mgmt *mgmt, size_t len)
{
	u16 tid, params;
	u16 initiator;

	params = le16_to_cpu(mgmt->u.action.u.delba.params);
	tid = (params & IEEE80211_DELBA_PARAM_TID_MASK) >> 12;
	initiator = (params & IEEE80211_DELBA_PARAM_INITIATOR_MASK) >> 11;

	ht_dbg_ratelimited(sdata, "delba from %pM (%s) tid %d reason code %d\n",
			   mgmt->sa, initiator ? "initiator" : "recipient",
			   tid,
			   le16_to_cpu(mgmt->u.action.u.delba.reason_code));

	if (initiator == WLAN_BACK_INITIATOR)
		__ieee80211_stop_rx_ba_session(sta, tid, WLAN_BACK_INITIATOR, 0,
					       true);
	else
		__ieee80211_stop_tx_ba_session(sta, tid, AGG_STOP_PEER_REQUEST);
}

enum nl80211_smps_mode
ieee80211_smps_mode_to_smps_mode(enum ieee80211_smps_mode smps)
{
	switch (smps) {
	case IEEE80211_SMPS_OFF:
		return NL80211_SMPS_OFF;
	case IEEE80211_SMPS_STATIC:
		return NL80211_SMPS_STATIC;
	case IEEE80211_SMPS_DYNAMIC:
		return NL80211_SMPS_DYNAMIC;
	default:
		return NL80211_SMPS_OFF;
	}
}

int ieee80211_send_smps_action(struct ieee80211_sub_if_data *sdata,
			       enum ieee80211_smps_mode smps, const u8 *da,
			       const u8 *bssid)
{
	struct ieee80211_local *local = sdata->local;
	struct sk_buff *skb;
	struct ieee80211_mgmt *action_frame;

	/* 27 = header + category + action + smps mode */
	skb = dev_alloc_skb(27 + local->hw.extra_tx_headroom);
	if (!skb)
		return -ENOMEM;

	skb_reserve(skb, local->hw.extra_tx_headroom);
	action_frame = skb_put(skb, 27);
	memcpy(action_frame->da, da, ETH_ALEN);
	memcpy(action_frame->sa, sdata->dev->dev_addr, ETH_ALEN);
	memcpy(action_frame->bssid, bssid, ETH_ALEN);
	action_frame->frame_control = cpu_to_le16(IEEE80211_FTYPE_MGMT |
						  IEEE80211_STYPE_ACTION);
	action_frame->u.action.category = WLAN_CATEGORY_HT;
	action_frame->u.action.u.ht_smps.action = WLAN_HT_ACTION_SMPS;
	switch (smps) {
	case IEEE80211_SMPS_AUTOMATIC:
	case IEEE80211_SMPS_NUM_MODES:
		WARN_ON(1);
		fallthrough;
	case IEEE80211_SMPS_OFF:
		action_frame->u.action.u.ht_smps.smps_control =
				WLAN_HT_SMPS_CONTROL_DISABLED;
		break;
	case IEEE80211_SMPS_STATIC:
		action_frame->u.action.u.ht_smps.smps_control =
				WLAN_HT_SMPS_CONTROL_STATIC;
		break;
	case IEEE80211_SMPS_DYNAMIC:
		action_frame->u.action.u.ht_smps.smps_control =
				WLAN_HT_SMPS_CONTROL_DYNAMIC;
		break;
	}

	/* we'll do more on status of this frame */
	IEEE80211_SKB_CB(skb)->flags |= IEEE80211_TX_CTL_REQ_TX_STATUS;
	ieee80211_tx_skb(sdata, skb);

	return 0;
}

void ieee80211_request_smps_mgd_work(struct work_struct *work)
{
	struct ieee80211_sub_if_data *sdata =
		container_of(work, struct ieee80211_sub_if_data,
			     u.mgd.request_smps_work);

	sdata_lock(sdata);
	__ieee80211_request_smps_mgd(sdata, sdata->u.mgd.driver_smps_mode);
	sdata_unlock(sdata);
}

void ieee80211_request_smps(struct ieee80211_vif *vif,
			    enum ieee80211_smps_mode smps_mode)
{
	struct ieee80211_sub_if_data *sdata = vif_to_sdata(vif);

	if (WARN_ON_ONCE(vif->type != NL80211_IFTYPE_STATION &&
			 vif->type != NL80211_IFTYPE_AP))
		return;

	if (vif->type == NL80211_IFTYPE_STATION) {
		if (sdata->u.mgd.driver_smps_mode == smps_mode)
			return;
		sdata->u.mgd.driver_smps_mode = smps_mode;
		ieee80211_queue_work(&sdata->local->hw,
				     &sdata->u.mgd.request_smps_work);
	}
}
/* this might change ... don't want non-open drivers using it */
EXPORT_SYMBOL_GPL(ieee80211_request_smps);
