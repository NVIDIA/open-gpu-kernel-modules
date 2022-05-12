// SPDX-License-Identifier: GPL-2.0-only
/*
 * This file is part of wl1271
 *
 * Copyright (C) 2009 Nokia Corporation
 *
 * Contact: Luciano Coelho <luciano.coelho@nokia.com>
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/etherdevice.h>
#include <linux/pm_runtime.h>
#include <linux/spinlock.h>

#include "wlcore.h"
#include "debug.h"
#include "io.h"
#include "ps.h"
#include "tx.h"
#include "event.h"
#include "hw_ops.h"

/*
 * TODO: this is here just for now, it must be removed when the data
 * operations are in place.
 */
#include "../wl12xx/reg.h"

static int wl1271_set_default_wep_key(struct wl1271 *wl,
				      struct wl12xx_vif *wlvif, u8 id)
{
	int ret;
	bool is_ap = (wlvif->bss_type == BSS_TYPE_AP_BSS);

	if (is_ap)
		ret = wl12xx_cmd_set_default_wep_key(wl, id,
						     wlvif->ap.bcast_hlid);
	else
		ret = wl12xx_cmd_set_default_wep_key(wl, id, wlvif->sta.hlid);

	if (ret < 0)
		return ret;

	wl1271_debug(DEBUG_CRYPT, "default wep key idx: %d", (int)id);
	return 0;
}

static int wl1271_alloc_tx_id(struct wl1271 *wl, struct sk_buff *skb)
{
	int id;

	id = find_first_zero_bit(wl->tx_frames_map, wl->num_tx_desc);
	if (id >= wl->num_tx_desc)
		return -EBUSY;

	__set_bit(id, wl->tx_frames_map);
	wl->tx_frames[id] = skb;
	wl->tx_frames_cnt++;
	return id;
}

void wl1271_free_tx_id(struct wl1271 *wl, int id)
{
	if (__test_and_clear_bit(id, wl->tx_frames_map)) {
		if (unlikely(wl->tx_frames_cnt == wl->num_tx_desc))
			clear_bit(WL1271_FLAG_FW_TX_BUSY, &wl->flags);

		wl->tx_frames[id] = NULL;
		wl->tx_frames_cnt--;
	}
}
EXPORT_SYMBOL(wl1271_free_tx_id);

static void wl1271_tx_ap_update_inconnection_sta(struct wl1271 *wl,
						 struct wl12xx_vif *wlvif,
						 struct sk_buff *skb)
{
	struct ieee80211_hdr *hdr;

	hdr = (struct ieee80211_hdr *)(skb->data +
				       sizeof(struct wl1271_tx_hw_descr));
	if (!ieee80211_is_auth(hdr->frame_control))
		return;

	/*
	 * add the station to the known list before transmitting the
	 * authentication response. this way it won't get de-authed by FW
	 * when transmitting too soon.
	 */
	wl1271_acx_set_inconnection_sta(wl, wlvif, hdr->addr1);

	/*
	 * ROC for 1 second on the AP channel for completing the connection.
	 * Note the ROC will be continued by the update_sta_state callbacks
	 * once the station reaches the associated state.
	 */
	wlcore_update_inconn_sta(wl, wlvif, NULL, true);
	wlvif->pending_auth_reply_time = jiffies;
	cancel_delayed_work(&wlvif->pending_auth_complete_work);
	ieee80211_queue_delayed_work(wl->hw,
				&wlvif->pending_auth_complete_work,
				msecs_to_jiffies(WLCORE_PEND_AUTH_ROC_TIMEOUT));
}

static void wl1271_tx_regulate_link(struct wl1271 *wl,
				    struct wl12xx_vif *wlvif,
				    u8 hlid)
{
	bool fw_ps;
	u8 tx_pkts;

	if (WARN_ON(!test_bit(hlid, wlvif->links_map)))
		return;

	fw_ps = test_bit(hlid, &wl->ap_fw_ps_map);
	tx_pkts = wl->links[hlid].allocated_pkts;

	/*
	 * if in FW PS and there is enough data in FW we can put the link
	 * into high-level PS and clean out its TX queues.
	 * Make an exception if this is the only connected link. In this
	 * case FW-memory congestion is less of a problem.
	 * Note that a single connected STA means 2*ap_count + 1 active links,
	 * since we must account for the global and broadcast AP links
	 * for each AP. The "fw_ps" check assures us the other link is a STA
	 * connected to the AP. Otherwise the FW would not set the PSM bit.
	 */
	if (wl->active_link_count > (wl->ap_count*2 + 1) && fw_ps &&
	    tx_pkts >= WL1271_PS_STA_MAX_PACKETS)
		wl12xx_ps_link_start(wl, wlvif, hlid, true);
}

bool wl12xx_is_dummy_packet(struct wl1271 *wl, struct sk_buff *skb)
{
	return wl->dummy_packet == skb;
}
EXPORT_SYMBOL(wl12xx_is_dummy_packet);

static u8 wl12xx_tx_get_hlid_ap(struct wl1271 *wl, struct wl12xx_vif *wlvif,
				struct sk_buff *skb, struct ieee80211_sta *sta)
{
	if (sta) {
		struct wl1271_station *wl_sta;

		wl_sta = (struct wl1271_station *)sta->drv_priv;
		return wl_sta->hlid;
	} else {
		struct ieee80211_hdr *hdr;

		if (!test_bit(WLVIF_FLAG_AP_STARTED, &wlvif->flags))
			return wl->system_hlid;

		hdr = (struct ieee80211_hdr *)skb->data;
		if (is_multicast_ether_addr(ieee80211_get_DA(hdr)))
			return wlvif->ap.bcast_hlid;
		else
			return wlvif->ap.global_hlid;
	}
}

u8 wl12xx_tx_get_hlid(struct wl1271 *wl, struct wl12xx_vif *wlvif,
		      struct sk_buff *skb, struct ieee80211_sta *sta)
{
	struct ieee80211_tx_info *control;

	if (wlvif->bss_type == BSS_TYPE_AP_BSS)
		return wl12xx_tx_get_hlid_ap(wl, wlvif, skb, sta);

	control = IEEE80211_SKB_CB(skb);
	if (control->flags & IEEE80211_TX_CTL_TX_OFFCHAN) {
		wl1271_debug(DEBUG_TX, "tx offchannel");
		return wlvif->dev_hlid;
	}

	return wlvif->sta.hlid;
}

unsigned int wlcore_calc_packet_alignment(struct wl1271 *wl,
					  unsigned int packet_length)
{
	if ((wl->quirks & WLCORE_QUIRK_TX_PAD_LAST_FRAME) ||
	    !(wl->quirks & WLCORE_QUIRK_TX_BLOCKSIZE_ALIGN))
		return ALIGN(packet_length, WL1271_TX_ALIGN_TO);
	else
		return ALIGN(packet_length, WL12XX_BUS_BLOCK_SIZE);
}
EXPORT_SYMBOL(wlcore_calc_packet_alignment);

static int wl1271_tx_allocate(struct wl1271 *wl, struct wl12xx_vif *wlvif,
			      struct sk_buff *skb, u32 extra, u32 buf_offset,
			      u8 hlid, bool is_gem)
{
	struct wl1271_tx_hw_descr *desc;
	u32 total_len = skb->len + sizeof(struct wl1271_tx_hw_descr) + extra;
	u32 total_blocks;
	int id, ret = -EBUSY, ac;
	u32 spare_blocks;

	if (buf_offset + total_len > wl->aggr_buf_size)
		return -EAGAIN;

	spare_blocks = wlcore_hw_get_spare_blocks(wl, is_gem);

	/* allocate free identifier for the packet */
	id = wl1271_alloc_tx_id(wl, skb);
	if (id < 0)
		return id;

	total_blocks = wlcore_hw_calc_tx_blocks(wl, total_len, spare_blocks);

	if (total_blocks <= wl->tx_blocks_available) {
		desc = skb_push(skb, total_len - skb->len);

		wlcore_hw_set_tx_desc_blocks(wl, desc, total_blocks,
					     spare_blocks);

		desc->id = id;

		wl->tx_blocks_available -= total_blocks;
		wl->tx_allocated_blocks += total_blocks;

		/*
		 * If the FW was empty before, arm the Tx watchdog. Also do
		 * this on the first Tx after resume, as we always cancel the
		 * watchdog on suspend.
		 */
		if (wl->tx_allocated_blocks == total_blocks ||
		    test_and_clear_bit(WL1271_FLAG_REINIT_TX_WDOG, &wl->flags))
			wl12xx_rearm_tx_watchdog_locked(wl);

		ac = wl1271_tx_get_queue(skb_get_queue_mapping(skb));
		wl->tx_allocated_pkts[ac]++;

		if (test_bit(hlid, wl->links_map))
			wl->links[hlid].allocated_pkts++;

		ret = 0;

		wl1271_debug(DEBUG_TX,
			     "tx_allocate: size: %d, blocks: %d, id: %d",
			     total_len, total_blocks, id);
	} else {
		wl1271_free_tx_id(wl, id);
	}

	return ret;
}

static void wl1271_tx_fill_hdr(struct wl1271 *wl, struct wl12xx_vif *wlvif,
			       struct sk_buff *skb, u32 extra,
			       struct ieee80211_tx_info *control, u8 hlid)
{
	struct wl1271_tx_hw_descr *desc;
	int ac, rate_idx;
	s64 hosttime;
	u16 tx_attr = 0;
	__le16 frame_control;
	struct ieee80211_hdr *hdr;
	u8 *frame_start;
	bool is_dummy;

	desc = (struct wl1271_tx_hw_descr *) skb->data;
	frame_start = (u8 *)(desc + 1);
	hdr = (struct ieee80211_hdr *)(frame_start + extra);
	frame_control = hdr->frame_control;

	/* relocate space for security header */
	if (extra) {
		int hdrlen = ieee80211_hdrlen(frame_control);
		memmove(frame_start, hdr, hdrlen);
		skb_set_network_header(skb, skb_network_offset(skb) + extra);
	}

	/* configure packet life time */
	hosttime = (ktime_get_boottime_ns() >> 10);
	desc->start_time = cpu_to_le32(hosttime - wl->time_offset);

	is_dummy = wl12xx_is_dummy_packet(wl, skb);
	if (is_dummy || !wlvif || wlvif->bss_type != BSS_TYPE_AP_BSS)
		desc->life_time = cpu_to_le16(TX_HW_MGMT_PKT_LIFETIME_TU);
	else
		desc->life_time = cpu_to_le16(TX_HW_AP_MODE_PKT_LIFETIME_TU);

	/* queue */
	ac = wl1271_tx_get_queue(skb_get_queue_mapping(skb));
	desc->tid = skb->priority;

	if (is_dummy) {
		/*
		 * FW expects the dummy packet to have an invalid session id -
		 * any session id that is different than the one set in the join
		 */
		tx_attr = (SESSION_COUNTER_INVALID <<
			   TX_HW_ATTR_OFST_SESSION_COUNTER) &
			   TX_HW_ATTR_SESSION_COUNTER;

		tx_attr |= TX_HW_ATTR_TX_DUMMY_REQ;
	} else if (wlvif) {
		u8 session_id = wl->session_ids[hlid];

		if ((wl->quirks & WLCORE_QUIRK_AP_ZERO_SESSION_ID) &&
		    (wlvif->bss_type == BSS_TYPE_AP_BSS))
			session_id = 0;

		/* configure the tx attributes */
		tx_attr = session_id << TX_HW_ATTR_OFST_SESSION_COUNTER;
	}

	desc->hlid = hlid;
	if (is_dummy || !wlvif)
		rate_idx = 0;
	else if (wlvif->bss_type != BSS_TYPE_AP_BSS) {
		/*
		 * if the packets are data packets
		 * send them with AP rate policies (EAPOLs are an exception),
		 * otherwise use default basic rates
		 */
		if (skb->protocol == cpu_to_be16(ETH_P_PAE))
			rate_idx = wlvif->sta.basic_rate_idx;
		else if (control->flags & IEEE80211_TX_CTL_NO_CCK_RATE)
			rate_idx = wlvif->sta.p2p_rate_idx;
		else if (ieee80211_is_data(frame_control))
			rate_idx = wlvif->sta.ap_rate_idx;
		else
			rate_idx = wlvif->sta.basic_rate_idx;
	} else {
		if (hlid == wlvif->ap.global_hlid)
			rate_idx = wlvif->ap.mgmt_rate_idx;
		else if (hlid == wlvif->ap.bcast_hlid ||
			 skb->protocol == cpu_to_be16(ETH_P_PAE) ||
			 !ieee80211_is_data(frame_control))
			/*
			 * send non-data, bcast and EAPOLs using the
			 * min basic rate
			 */
			rate_idx = wlvif->ap.bcast_rate_idx;
		else
			rate_idx = wlvif->ap.ucast_rate_idx[ac];
	}

	tx_attr |= rate_idx << TX_HW_ATTR_OFST_RATE_POLICY;

	/* for WEP shared auth - no fw encryption is needed */
	if (ieee80211_is_auth(frame_control) &&
	    ieee80211_has_protected(frame_control))
		tx_attr |= TX_HW_ATTR_HOST_ENCRYPT;

	/* send EAPOL frames as voice */
	if (control->control.flags & IEEE80211_TX_CTRL_PORT_CTRL_PROTO)
		tx_attr |= TX_HW_ATTR_EAPOL_FRAME;

	desc->tx_attr = cpu_to_le16(tx_attr);

	wlcore_hw_set_tx_desc_csum(wl, desc, skb);
	wlcore_hw_set_tx_desc_data_len(wl, desc, skb);
}

/* caller must hold wl->mutex */
static int wl1271_prepare_tx_frame(struct wl1271 *wl, struct wl12xx_vif *wlvif,
				   struct sk_buff *skb, u32 buf_offset, u8 hlid)
{
	struct ieee80211_tx_info *info;
	u32 extra = 0;
	int ret = 0;
	u32 total_len;
	bool is_dummy;
	bool is_gem = false;

	if (!skb) {
		wl1271_error("discarding null skb");
		return -EINVAL;
	}

	if (hlid == WL12XX_INVALID_LINK_ID) {
		wl1271_error("invalid hlid. dropping skb 0x%p", skb);
		return -EINVAL;
	}

	info = IEEE80211_SKB_CB(skb);

	is_dummy = wl12xx_is_dummy_packet(wl, skb);

	if ((wl->quirks & WLCORE_QUIRK_TKIP_HEADER_SPACE) &&
	    info->control.hw_key &&
	    info->control.hw_key->cipher == WLAN_CIPHER_SUITE_TKIP)
		extra = WL1271_EXTRA_SPACE_TKIP;

	if (info->control.hw_key) {
		bool is_wep;
		u8 idx = info->control.hw_key->hw_key_idx;
		u32 cipher = info->control.hw_key->cipher;

		is_wep = (cipher == WLAN_CIPHER_SUITE_WEP40) ||
			 (cipher == WLAN_CIPHER_SUITE_WEP104);

		if (WARN_ON(is_wep && wlvif && wlvif->default_key != idx)) {
			ret = wl1271_set_default_wep_key(wl, wlvif, idx);
			if (ret < 0)
				return ret;
			wlvif->default_key = idx;
		}

		is_gem = (cipher == WL1271_CIPHER_SUITE_GEM);
	}

	ret = wl1271_tx_allocate(wl, wlvif, skb, extra, buf_offset, hlid,
				 is_gem);
	if (ret < 0)
		return ret;

	wl1271_tx_fill_hdr(wl, wlvif, skb, extra, info, hlid);

	if (!is_dummy && wlvif && wlvif->bss_type == BSS_TYPE_AP_BSS) {
		wl1271_tx_ap_update_inconnection_sta(wl, wlvif, skb);
		wl1271_tx_regulate_link(wl, wlvif, hlid);
	}

	/*
	 * The length of each packet is stored in terms of
	 * words. Thus, we must pad the skb data to make sure its
	 * length is aligned.  The number of padding bytes is computed
	 * and set in wl1271_tx_fill_hdr.
	 * In special cases, we want to align to a specific block size
	 * (eg. for wl128x with SDIO we align to 256).
	 */
	total_len = wlcore_calc_packet_alignment(wl, skb->len);

	memcpy(wl->aggr_buf + buf_offset, skb->data, skb->len);
	memset(wl->aggr_buf + buf_offset + skb->len, 0, total_len - skb->len);

	/* Revert side effects in the dummy packet skb, so it can be reused */
	if (is_dummy)
		skb_pull(skb, sizeof(struct wl1271_tx_hw_descr));

	return total_len;
}

u32 wl1271_tx_enabled_rates_get(struct wl1271 *wl, u32 rate_set,
				enum nl80211_band rate_band)
{
	struct ieee80211_supported_band *band;
	u32 enabled_rates = 0;
	int bit;

	band = wl->hw->wiphy->bands[rate_band];
	for (bit = 0; bit < band->n_bitrates; bit++) {
		if (rate_set & 0x1)
			enabled_rates |= band->bitrates[bit].hw_value;
		rate_set >>= 1;
	}

	/* MCS rates indication are on bits 16 - 31 */
	rate_set >>= HW_HT_RATES_OFFSET - band->n_bitrates;

	for (bit = 0; bit < 16; bit++) {
		if (rate_set & 0x1)
			enabled_rates |= (CONF_HW_BIT_RATE_MCS_0 << bit);
		rate_set >>= 1;
	}

	return enabled_rates;
}

void wl1271_handle_tx_low_watermark(struct wl1271 *wl)
{
	int i;
	struct wl12xx_vif *wlvif;

	wl12xx_for_each_wlvif(wl, wlvif) {
		for (i = 0; i < NUM_TX_QUEUES; i++) {
			if (wlcore_is_queue_stopped_by_reason(wl, wlvif, i,
					WLCORE_QUEUE_STOP_REASON_WATERMARK) &&
			    wlvif->tx_queue_count[i] <=
					WL1271_TX_QUEUE_LOW_WATERMARK)
				/* firmware buffer has space, restart queues */
				wlcore_wake_queue(wl, wlvif, i,
					WLCORE_QUEUE_STOP_REASON_WATERMARK);
		}
	}
}

static int wlcore_select_ac(struct wl1271 *wl)
{
	int i, q = -1, ac;
	u32 min_pkts = 0xffffffff;

	/*
	 * Find a non-empty ac where:
	 * 1. There are packets to transmit
	 * 2. The FW has the least allocated blocks
	 *
	 * We prioritize the ACs according to VO>VI>BE>BK
	 */
	for (i = 0; i < NUM_TX_QUEUES; i++) {
		ac = wl1271_tx_get_queue(i);
		if (wl->tx_queue_count[ac] &&
		    wl->tx_allocated_pkts[ac] < min_pkts) {
			q = ac;
			min_pkts = wl->tx_allocated_pkts[q];
		}
	}

	return q;
}

static struct sk_buff *wlcore_lnk_dequeue(struct wl1271 *wl,
					  struct wl1271_link *lnk, u8 q)
{
	struct sk_buff *skb;
	unsigned long flags;

	skb = skb_dequeue(&lnk->tx_queue[q]);
	if (skb) {
		spin_lock_irqsave(&wl->wl_lock, flags);
		WARN_ON_ONCE(wl->tx_queue_count[q] <= 0);
		wl->tx_queue_count[q]--;
		if (lnk->wlvif) {
			WARN_ON_ONCE(lnk->wlvif->tx_queue_count[q] <= 0);
			lnk->wlvif->tx_queue_count[q]--;
		}
		spin_unlock_irqrestore(&wl->wl_lock, flags);
	}

	return skb;
}

static struct sk_buff *wlcore_lnk_dequeue_high_prio(struct wl1271 *wl,
						    u8 hlid, u8 ac,
						    u8 *low_prio_hlid)
{
	struct wl1271_link *lnk = &wl->links[hlid];

	if (!wlcore_hw_lnk_high_prio(wl, hlid, lnk)) {
		if (*low_prio_hlid == WL12XX_INVALID_LINK_ID &&
		    !skb_queue_empty(&lnk->tx_queue[ac]) &&
		    wlcore_hw_lnk_low_prio(wl, hlid, lnk))
			/* we found the first non-empty low priority queue */
			*low_prio_hlid = hlid;

		return NULL;
	}

	return wlcore_lnk_dequeue(wl, lnk, ac);
}

static struct sk_buff *wlcore_vif_dequeue_high_prio(struct wl1271 *wl,
						    struct wl12xx_vif *wlvif,
						    u8 ac, u8 *hlid,
						    u8 *low_prio_hlid)
{
	struct sk_buff *skb = NULL;
	int i, h, start_hlid;

	/* start from the link after the last one */
	start_hlid = (wlvif->last_tx_hlid + 1) % wl->num_links;

	/* dequeue according to AC, round robin on each link */
	for (i = 0; i < wl->num_links; i++) {
		h = (start_hlid + i) % wl->num_links;

		/* only consider connected stations */
		if (!test_bit(h, wlvif->links_map))
			continue;

		skb = wlcore_lnk_dequeue_high_prio(wl, h, ac,
						   low_prio_hlid);
		if (!skb)
			continue;

		wlvif->last_tx_hlid = h;
		break;
	}

	if (!skb)
		wlvif->last_tx_hlid = 0;

	*hlid = wlvif->last_tx_hlid;
	return skb;
}

static struct sk_buff *wl1271_skb_dequeue(struct wl1271 *wl, u8 *hlid)
{
	unsigned long flags;
	struct wl12xx_vif *wlvif = wl->last_wlvif;
	struct sk_buff *skb = NULL;
	int ac;
	u8 low_prio_hlid = WL12XX_INVALID_LINK_ID;

	ac = wlcore_select_ac(wl);
	if (ac < 0)
		goto out;

	/* continue from last wlvif (round robin) */
	if (wlvif) {
		wl12xx_for_each_wlvif_continue(wl, wlvif) {
			if (!wlvif->tx_queue_count[ac])
				continue;

			skb = wlcore_vif_dequeue_high_prio(wl, wlvif, ac, hlid,
							   &low_prio_hlid);
			if (!skb)
				continue;

			wl->last_wlvif = wlvif;
			break;
		}
	}

	/* dequeue from the system HLID before the restarting wlvif list */
	if (!skb) {
		skb = wlcore_lnk_dequeue_high_prio(wl, wl->system_hlid,
						   ac, &low_prio_hlid);
		if (skb) {
			*hlid = wl->system_hlid;
			wl->last_wlvif = NULL;
		}
	}

	/* Do a new pass over the wlvif list. But no need to continue
	 * after last_wlvif. The previous pass should have found it. */
	if (!skb) {
		wl12xx_for_each_wlvif(wl, wlvif) {
			if (!wlvif->tx_queue_count[ac])
				goto next;

			skb = wlcore_vif_dequeue_high_prio(wl, wlvif, ac, hlid,
							   &low_prio_hlid);
			if (skb) {
				wl->last_wlvif = wlvif;
				break;
			}

next:
			if (wlvif == wl->last_wlvif)
				break;
		}
	}

	/* no high priority skbs found - but maybe a low priority one? */
	if (!skb && low_prio_hlid != WL12XX_INVALID_LINK_ID) {
		struct wl1271_link *lnk = &wl->links[low_prio_hlid];
		skb = wlcore_lnk_dequeue(wl, lnk, ac);

		WARN_ON(!skb); /* we checked this before */
		*hlid = low_prio_hlid;

		/* ensure proper round robin in the vif/link levels */
		wl->last_wlvif = lnk->wlvif;
		if (lnk->wlvif)
			lnk->wlvif->last_tx_hlid = low_prio_hlid;

	}

out:
	if (!skb &&
	    test_and_clear_bit(WL1271_FLAG_DUMMY_PACKET_PENDING, &wl->flags)) {
		int q;

		skb = wl->dummy_packet;
		*hlid = wl->system_hlid;
		q = wl1271_tx_get_queue(skb_get_queue_mapping(skb));
		spin_lock_irqsave(&wl->wl_lock, flags);
		WARN_ON_ONCE(wl->tx_queue_count[q] <= 0);
		wl->tx_queue_count[q]--;
		spin_unlock_irqrestore(&wl->wl_lock, flags);
	}

	return skb;
}

static void wl1271_skb_queue_head(struct wl1271 *wl, struct wl12xx_vif *wlvif,
				  struct sk_buff *skb, u8 hlid)
{
	unsigned long flags;
	int q = wl1271_tx_get_queue(skb_get_queue_mapping(skb));

	if (wl12xx_is_dummy_packet(wl, skb)) {
		set_bit(WL1271_FLAG_DUMMY_PACKET_PENDING, &wl->flags);
	} else {
		skb_queue_head(&wl->links[hlid].tx_queue[q], skb);

		/* make sure we dequeue the same packet next time */
		wlvif->last_tx_hlid = (hlid + wl->num_links - 1) %
				      wl->num_links;
	}

	spin_lock_irqsave(&wl->wl_lock, flags);
	wl->tx_queue_count[q]++;
	if (wlvif)
		wlvif->tx_queue_count[q]++;
	spin_unlock_irqrestore(&wl->wl_lock, flags);
}

static bool wl1271_tx_is_data_present(struct sk_buff *skb)
{
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *)(skb->data);

	return ieee80211_is_data_present(hdr->frame_control);
}

void wl12xx_rearm_rx_streaming(struct wl1271 *wl, unsigned long *active_hlids)
{
	struct wl12xx_vif *wlvif;
	u32 timeout;
	u8 hlid;

	if (!wl->conf.rx_streaming.interval)
		return;

	if (!wl->conf.rx_streaming.always &&
	    !test_bit(WL1271_FLAG_SOFT_GEMINI, &wl->flags))
		return;

	timeout = wl->conf.rx_streaming.duration;
	wl12xx_for_each_wlvif_sta(wl, wlvif) {
		bool found = false;
		for_each_set_bit(hlid, active_hlids, wl->num_links) {
			if (test_bit(hlid, wlvif->links_map)) {
				found  = true;
				break;
			}
		}

		if (!found)
			continue;

		/* enable rx streaming */
		if (!test_bit(WLVIF_FLAG_RX_STREAMING_STARTED, &wlvif->flags))
			ieee80211_queue_work(wl->hw,
					     &wlvif->rx_streaming_enable_work);

		mod_timer(&wlvif->rx_streaming_timer,
			  jiffies + msecs_to_jiffies(timeout));
	}
}

/*
 * Returns failure values only in case of failed bus ops within this function.
 * wl1271_prepare_tx_frame retvals won't be returned in order to avoid
 * triggering recovery by higher layers when not necessary.
 * In case a FW command fails within wl1271_prepare_tx_frame fails a recovery
 * will be queued in wl1271_cmd_send. -EAGAIN/-EBUSY from prepare_tx_frame
 * can occur and are legitimate so don't propagate. -EINVAL will emit a WARNING
 * within prepare_tx_frame code but there's nothing we should do about those
 * as well.
 */
int wlcore_tx_work_locked(struct wl1271 *wl)
{
	struct wl12xx_vif *wlvif;
	struct sk_buff *skb;
	struct wl1271_tx_hw_descr *desc;
	u32 buf_offset = 0, last_len = 0;
	bool sent_packets = false;
	unsigned long active_hlids[BITS_TO_LONGS(WLCORE_MAX_LINKS)] = {0};
	int ret = 0;
	int bus_ret = 0;
	u8 hlid;

	if (unlikely(wl->state != WLCORE_STATE_ON))
		return 0;

	while ((skb = wl1271_skb_dequeue(wl, &hlid))) {
		struct ieee80211_tx_info *info = IEEE80211_SKB_CB(skb);
		bool has_data = false;

		wlvif = NULL;
		if (!wl12xx_is_dummy_packet(wl, skb))
			wlvif = wl12xx_vif_to_data(info->control.vif);
		else
			hlid = wl->system_hlid;

		has_data = wlvif && wl1271_tx_is_data_present(skb);
		ret = wl1271_prepare_tx_frame(wl, wlvif, skb, buf_offset,
					      hlid);
		if (ret == -EAGAIN) {
			/*
			 * Aggregation buffer is full.
			 * Flush buffer and try again.
			 */
			wl1271_skb_queue_head(wl, wlvif, skb, hlid);

			buf_offset = wlcore_hw_pre_pkt_send(wl, buf_offset,
							    last_len);
			bus_ret = wlcore_write_data(wl, REG_SLV_MEM_DATA,
					     wl->aggr_buf, buf_offset, true);
			if (bus_ret < 0)
				goto out;

			sent_packets = true;
			buf_offset = 0;
			continue;
		} else if (ret == -EBUSY) {
			/*
			 * Firmware buffer is full.
			 * Queue back last skb, and stop aggregating.
			 */
			wl1271_skb_queue_head(wl, wlvif, skb, hlid);
			/* No work left, avoid scheduling redundant tx work */
			set_bit(WL1271_FLAG_FW_TX_BUSY, &wl->flags);
			goto out_ack;
		} else if (ret < 0) {
			if (wl12xx_is_dummy_packet(wl, skb))
				/*
				 * fw still expects dummy packet,
				 * so re-enqueue it
				 */
				wl1271_skb_queue_head(wl, wlvif, skb, hlid);
			else
				ieee80211_free_txskb(wl->hw, skb);
			goto out_ack;
		}
		last_len = ret;
		buf_offset += last_len;
		wl->tx_packets_count++;
		if (has_data) {
			desc = (struct wl1271_tx_hw_descr *) skb->data;
			__set_bit(desc->hlid, active_hlids);
		}
	}

out_ack:
	if (buf_offset) {
		buf_offset = wlcore_hw_pre_pkt_send(wl, buf_offset, last_len);
		bus_ret = wlcore_write_data(wl, REG_SLV_MEM_DATA, wl->aggr_buf,
					     buf_offset, true);
		if (bus_ret < 0)
			goto out;

		sent_packets = true;
	}
	if (sent_packets) {
		/*
		 * Interrupt the firmware with the new packets. This is only
		 * required for older hardware revisions
		 */
		if (wl->quirks & WLCORE_QUIRK_END_OF_TRANSACTION) {
			bus_ret = wlcore_write32(wl, WL12XX_HOST_WR_ACCESS,
					     wl->tx_packets_count);
			if (bus_ret < 0)
				goto out;
		}

		wl1271_handle_tx_low_watermark(wl);
	}
	wl12xx_rearm_rx_streaming(wl, active_hlids);

out:
	return bus_ret;
}

void wl1271_tx_work(struct work_struct *work)
{
	struct wl1271 *wl = container_of(work, struct wl1271, tx_work);
	int ret;

	mutex_lock(&wl->mutex);
	ret = pm_runtime_get_sync(wl->dev);
	if (ret < 0) {
		pm_runtime_put_noidle(wl->dev);
		goto out;
	}

	ret = wlcore_tx_work_locked(wl);
	if (ret < 0) {
		pm_runtime_put_noidle(wl->dev);
		wl12xx_queue_recovery_work(wl);
		goto out;
	}

	pm_runtime_mark_last_busy(wl->dev);
	pm_runtime_put_autosuspend(wl->dev);
out:
	mutex_unlock(&wl->mutex);
}

static u8 wl1271_tx_get_rate_flags(u8 rate_class_index)
{
	u8 flags = 0;

	/*
	 * TODO: use wl12xx constants when this code is moved to wl12xx, as
	 * only it uses Tx-completion.
	 */
	if (rate_class_index <= 8)
		flags |= IEEE80211_TX_RC_MCS;

	/*
	 * TODO: use wl12xx constants when this code is moved to wl12xx, as
	 * only it uses Tx-completion.
	 */
	if (rate_class_index == 0)
		flags |= IEEE80211_TX_RC_SHORT_GI;

	return flags;
}

static void wl1271_tx_complete_packet(struct wl1271 *wl,
				      struct wl1271_tx_hw_res_descr *result)
{
	struct ieee80211_tx_info *info;
	struct ieee80211_vif *vif;
	struct wl12xx_vif *wlvif;
	struct sk_buff *skb;
	int id = result->id;
	int rate = -1;
	u8 rate_flags = 0;
	u8 retries = 0;

	/* check for id legality */
	if (unlikely(id >= wl->num_tx_desc || wl->tx_frames[id] == NULL)) {
		wl1271_warning("TX result illegal id: %d", id);
		return;
	}

	skb = wl->tx_frames[id];
	info = IEEE80211_SKB_CB(skb);

	if (wl12xx_is_dummy_packet(wl, skb)) {
		wl1271_free_tx_id(wl, id);
		return;
	}

	/* info->control is valid as long as we don't update info->status */
	vif = info->control.vif;
	wlvif = wl12xx_vif_to_data(vif);

	/* update the TX status info */
	if (result->status == TX_SUCCESS) {
		if (!(info->flags & IEEE80211_TX_CTL_NO_ACK))
			info->flags |= IEEE80211_TX_STAT_ACK;
		rate = wlcore_rate_to_idx(wl, result->rate_class_index,
					  wlvif->band);
		rate_flags = wl1271_tx_get_rate_flags(result->rate_class_index);
		retries = result->ack_failures;
	} else if (result->status == TX_RETRY_EXCEEDED) {
		wl->stats.excessive_retries++;
		retries = result->ack_failures;
	}

	info->status.rates[0].idx = rate;
	info->status.rates[0].count = retries;
	info->status.rates[0].flags = rate_flags;
	info->status.ack_signal = -1;

	wl->stats.retry_count += result->ack_failures;

	/* remove private header from packet */
	skb_pull(skb, sizeof(struct wl1271_tx_hw_descr));

	/* remove TKIP header space if present */
	if ((wl->quirks & WLCORE_QUIRK_TKIP_HEADER_SPACE) &&
	    info->control.hw_key &&
	    info->control.hw_key->cipher == WLAN_CIPHER_SUITE_TKIP) {
		int hdrlen = ieee80211_get_hdrlen_from_skb(skb);
		memmove(skb->data + WL1271_EXTRA_SPACE_TKIP, skb->data,
			hdrlen);
		skb_pull(skb, WL1271_EXTRA_SPACE_TKIP);
	}

	wl1271_debug(DEBUG_TX, "tx status id %u skb 0x%p failures %u rate 0x%x"
		     " status 0x%x",
		     result->id, skb, result->ack_failures,
		     result->rate_class_index, result->status);

	/* return the packet to the stack */
	skb_queue_tail(&wl->deferred_tx_queue, skb);
	queue_work(wl->freezable_wq, &wl->netstack_work);
	wl1271_free_tx_id(wl, result->id);
}

/* Called upon reception of a TX complete interrupt */
int wlcore_tx_complete(struct wl1271 *wl)
{
	struct wl1271_acx_mem_map *memmap = wl->target_mem_map;
	u32 count, fw_counter;
	u32 i;
	int ret;

	/* read the tx results from the chipset */
	ret = wlcore_read(wl, le32_to_cpu(memmap->tx_result),
			  wl->tx_res_if, sizeof(*wl->tx_res_if), false);
	if (ret < 0)
		goto out;

	fw_counter = le32_to_cpu(wl->tx_res_if->tx_result_fw_counter);

	/* write host counter to chipset (to ack) */
	ret = wlcore_write32(wl, le32_to_cpu(memmap->tx_result) +
			     offsetof(struct wl1271_tx_hw_res_if,
				      tx_result_host_counter), fw_counter);
	if (ret < 0)
		goto out;

	count = fw_counter - wl->tx_results_count;
	wl1271_debug(DEBUG_TX, "tx_complete received, packets: %d", count);

	/* verify that the result buffer is not getting overrun */
	if (unlikely(count > TX_HW_RESULT_QUEUE_LEN))
		wl1271_warning("TX result overflow from chipset: %d", count);

	/* process the results */
	for (i = 0; i < count; i++) {
		struct wl1271_tx_hw_res_descr *result;
		u8 offset = wl->tx_results_count & TX_HW_RESULT_QUEUE_LEN_MASK;

		/* process the packet */
		result =  &(wl->tx_res_if->tx_results_queue[offset]);
		wl1271_tx_complete_packet(wl, result);

		wl->tx_results_count++;
	}

out:
	return ret;
}
EXPORT_SYMBOL(wlcore_tx_complete);

void wl1271_tx_reset_link_queues(struct wl1271 *wl, u8 hlid)
{
	struct sk_buff *skb;
	int i;
	unsigned long flags;
	struct ieee80211_tx_info *info;
	int total[NUM_TX_QUEUES];
	struct wl1271_link *lnk = &wl->links[hlid];

	for (i = 0; i < NUM_TX_QUEUES; i++) {
		total[i] = 0;
		while ((skb = skb_dequeue(&lnk->tx_queue[i]))) {
			wl1271_debug(DEBUG_TX, "link freeing skb 0x%p", skb);

			if (!wl12xx_is_dummy_packet(wl, skb)) {
				info = IEEE80211_SKB_CB(skb);
				info->status.rates[0].idx = -1;
				info->status.rates[0].count = 0;
				ieee80211_tx_status_ni(wl->hw, skb);
			}

			total[i]++;
		}
	}

	spin_lock_irqsave(&wl->wl_lock, flags);
	for (i = 0; i < NUM_TX_QUEUES; i++) {
		wl->tx_queue_count[i] -= total[i];
		if (lnk->wlvif)
			lnk->wlvif->tx_queue_count[i] -= total[i];
	}
	spin_unlock_irqrestore(&wl->wl_lock, flags);

	wl1271_handle_tx_low_watermark(wl);
}

/* caller must hold wl->mutex and TX must be stopped */
void wl12xx_tx_reset_wlvif(struct wl1271 *wl, struct wl12xx_vif *wlvif)
{
	int i;

	/* TX failure */
	for_each_set_bit(i, wlvif->links_map, wl->num_links) {
		if (wlvif->bss_type == BSS_TYPE_AP_BSS &&
		    i != wlvif->ap.bcast_hlid && i != wlvif->ap.global_hlid) {
			/* this calls wl12xx_free_link */
			wl1271_free_sta(wl, wlvif, i);
		} else {
			u8 hlid = i;
			wl12xx_free_link(wl, wlvif, &hlid);
		}
	}
	wlvif->last_tx_hlid = 0;

	for (i = 0; i < NUM_TX_QUEUES; i++)
		wlvif->tx_queue_count[i] = 0;
}
/* caller must hold wl->mutex and TX must be stopped */
void wl12xx_tx_reset(struct wl1271 *wl)
{
	int i;
	struct sk_buff *skb;
	struct ieee80211_tx_info *info;

	/* only reset the queues if something bad happened */
	if (wl1271_tx_total_queue_count(wl) != 0) {
		for (i = 0; i < wl->num_links; i++)
			wl1271_tx_reset_link_queues(wl, i);

		for (i = 0; i < NUM_TX_QUEUES; i++)
			wl->tx_queue_count[i] = 0;
	}

	/*
	 * Make sure the driver is at a consistent state, in case this
	 * function is called from a context other than interface removal.
	 * This call will always wake the TX queues.
	 */
	wl1271_handle_tx_low_watermark(wl);

	for (i = 0; i < wl->num_tx_desc; i++) {
		if (wl->tx_frames[i] == NULL)
			continue;

		skb = wl->tx_frames[i];
		wl1271_free_tx_id(wl, i);
		wl1271_debug(DEBUG_TX, "freeing skb 0x%p", skb);

		if (!wl12xx_is_dummy_packet(wl, skb)) {
			/*
			 * Remove private headers before passing the skb to
			 * mac80211
			 */
			info = IEEE80211_SKB_CB(skb);
			skb_pull(skb, sizeof(struct wl1271_tx_hw_descr));
			if ((wl->quirks & WLCORE_QUIRK_TKIP_HEADER_SPACE) &&
			    info->control.hw_key &&
			    info->control.hw_key->cipher ==
			    WLAN_CIPHER_SUITE_TKIP) {
				int hdrlen = ieee80211_get_hdrlen_from_skb(skb);
				memmove(skb->data + WL1271_EXTRA_SPACE_TKIP,
					skb->data, hdrlen);
				skb_pull(skb, WL1271_EXTRA_SPACE_TKIP);
			}

			info->status.rates[0].idx = -1;
			info->status.rates[0].count = 0;

			ieee80211_tx_status_ni(wl->hw, skb);
		}
	}
}

#define WL1271_TX_FLUSH_TIMEOUT 500000

/* caller must *NOT* hold wl->mutex */
void wl1271_tx_flush(struct wl1271 *wl)
{
	unsigned long timeout, start_time;
	int i;
	start_time = jiffies;
	timeout = start_time + usecs_to_jiffies(WL1271_TX_FLUSH_TIMEOUT);

	/* only one flush should be in progress, for consistent queue state */
	mutex_lock(&wl->flush_mutex);

	mutex_lock(&wl->mutex);
	if (wl->tx_frames_cnt == 0 && wl1271_tx_total_queue_count(wl) == 0) {
		mutex_unlock(&wl->mutex);
		goto out;
	}

	wlcore_stop_queues(wl, WLCORE_QUEUE_STOP_REASON_FLUSH);

	while (!time_after(jiffies, timeout)) {
		wl1271_debug(DEBUG_MAC80211, "flushing tx buffer: %d %d",
			     wl->tx_frames_cnt,
			     wl1271_tx_total_queue_count(wl));

		/* force Tx and give the driver some time to flush data */
		mutex_unlock(&wl->mutex);
		if (wl1271_tx_total_queue_count(wl))
			wl1271_tx_work(&wl->tx_work);
		msleep(20);
		mutex_lock(&wl->mutex);

		if ((wl->tx_frames_cnt == 0) &&
		    (wl1271_tx_total_queue_count(wl) == 0)) {
			wl1271_debug(DEBUG_MAC80211, "tx flush took %d ms",
				     jiffies_to_msecs(jiffies - start_time));
			goto out_wake;
		}
	}

	wl1271_warning("Unable to flush all TX buffers, "
		       "timed out (timeout %d ms",
		       WL1271_TX_FLUSH_TIMEOUT / 1000);

	/* forcibly flush all Tx buffers on our queues */
	for (i = 0; i < wl->num_links; i++)
		wl1271_tx_reset_link_queues(wl, i);

out_wake:
	wlcore_wake_queues(wl, WLCORE_QUEUE_STOP_REASON_FLUSH);
	mutex_unlock(&wl->mutex);
out:
	mutex_unlock(&wl->flush_mutex);
}
EXPORT_SYMBOL_GPL(wl1271_tx_flush);

u32 wl1271_tx_min_rate_get(struct wl1271 *wl, u32 rate_set)
{
	if (WARN_ON(!rate_set))
		return 0;

	return BIT(__ffs(rate_set));
}
EXPORT_SYMBOL_GPL(wl1271_tx_min_rate_get);

void wlcore_stop_queue_locked(struct wl1271 *wl, struct wl12xx_vif *wlvif,
			      u8 queue, enum wlcore_queue_stop_reason reason)
{
	int hwq = wlcore_tx_get_mac80211_queue(wlvif, queue);
	bool stopped = !!wl->queue_stop_reasons[hwq];

	/* queue should not be stopped for this reason */
	WARN_ON_ONCE(test_and_set_bit(reason, &wl->queue_stop_reasons[hwq]));

	if (stopped)
		return;

	ieee80211_stop_queue(wl->hw, hwq);
}

void wlcore_stop_queue(struct wl1271 *wl, struct wl12xx_vif *wlvif, u8 queue,
		       enum wlcore_queue_stop_reason reason)
{
	unsigned long flags;

	spin_lock_irqsave(&wl->wl_lock, flags);
	wlcore_stop_queue_locked(wl, wlvif, queue, reason);
	spin_unlock_irqrestore(&wl->wl_lock, flags);
}

void wlcore_wake_queue(struct wl1271 *wl, struct wl12xx_vif *wlvif, u8 queue,
		       enum wlcore_queue_stop_reason reason)
{
	unsigned long flags;
	int hwq = wlcore_tx_get_mac80211_queue(wlvif, queue);

	spin_lock_irqsave(&wl->wl_lock, flags);

	/* queue should not be clear for this reason */
	WARN_ON_ONCE(!test_and_clear_bit(reason, &wl->queue_stop_reasons[hwq]));

	if (wl->queue_stop_reasons[hwq])
		goto out;

	ieee80211_wake_queue(wl->hw, hwq);

out:
	spin_unlock_irqrestore(&wl->wl_lock, flags);
}

void wlcore_stop_queues(struct wl1271 *wl,
			enum wlcore_queue_stop_reason reason)
{
	int i;
	unsigned long flags;

	spin_lock_irqsave(&wl->wl_lock, flags);

	/* mark all possible queues as stopped */
        for (i = 0; i < WLCORE_NUM_MAC_ADDRESSES * NUM_TX_QUEUES; i++)
                WARN_ON_ONCE(test_and_set_bit(reason,
					      &wl->queue_stop_reasons[i]));

	/* use the global version to make sure all vifs in mac80211 we don't
	 * know are stopped.
	 */
	ieee80211_stop_queues(wl->hw);

	spin_unlock_irqrestore(&wl->wl_lock, flags);
}

void wlcore_wake_queues(struct wl1271 *wl,
			enum wlcore_queue_stop_reason reason)
{
	int i;
	unsigned long flags;

	spin_lock_irqsave(&wl->wl_lock, flags);

	/* mark all possible queues as awake */
        for (i = 0; i < WLCORE_NUM_MAC_ADDRESSES * NUM_TX_QUEUES; i++)
		WARN_ON_ONCE(!test_and_clear_bit(reason,
						 &wl->queue_stop_reasons[i]));

	/* use the global version to make sure all vifs in mac80211 we don't
	 * know are woken up.
	 */
	ieee80211_wake_queues(wl->hw);

	spin_unlock_irqrestore(&wl->wl_lock, flags);
}

bool wlcore_is_queue_stopped_by_reason(struct wl1271 *wl,
				       struct wl12xx_vif *wlvif, u8 queue,
				       enum wlcore_queue_stop_reason reason)
{
	unsigned long flags;
	bool stopped;

	spin_lock_irqsave(&wl->wl_lock, flags);
	stopped = wlcore_is_queue_stopped_by_reason_locked(wl, wlvif, queue,
							   reason);
	spin_unlock_irqrestore(&wl->wl_lock, flags);

	return stopped;
}

bool wlcore_is_queue_stopped_by_reason_locked(struct wl1271 *wl,
				       struct wl12xx_vif *wlvif, u8 queue,
				       enum wlcore_queue_stop_reason reason)
{
	int hwq = wlcore_tx_get_mac80211_queue(wlvif, queue);

	assert_spin_locked(&wl->wl_lock);
	return test_bit(reason, &wl->queue_stop_reasons[hwq]);
}

bool wlcore_is_queue_stopped_locked(struct wl1271 *wl, struct wl12xx_vif *wlvif,
				    u8 queue)
{
	int hwq = wlcore_tx_get_mac80211_queue(wlvif, queue);

	assert_spin_locked(&wl->wl_lock);
	return !!wl->queue_stop_reasons[hwq];
}
