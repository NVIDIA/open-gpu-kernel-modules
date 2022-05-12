/*
 * Copyright (c) 2011 Atheros Communications Inc.
 * Copyright (c) 2011-2012 Qualcomm Atheros, Inc.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef ATH6KL_CFG80211_H
#define ATH6KL_CFG80211_H

enum ath6kl_cfg_suspend_mode {
	ATH6KL_CFG_SUSPEND_DEEPSLEEP,
	ATH6KL_CFG_SUSPEND_CUTPOWER,
	ATH6KL_CFG_SUSPEND_WOW,
};

struct wireless_dev *ath6kl_interface_add(struct ath6kl *ar, const char *name,
					  unsigned char name_assign_type,
					  enum nl80211_iftype type,
					  u8 fw_vif_idx, u8 nw_type);
void ath6kl_cfg80211_ch_switch_notify(struct ath6kl_vif *vif, int freq,
				      enum wmi_phy_mode mode);
void ath6kl_cfg80211_scan_complete_event(struct ath6kl_vif *vif, bool aborted);

void ath6kl_cfg80211_connect_event(struct ath6kl_vif *vif, u16 channel,
				   u8 *bssid, u16 listen_intvl,
				   u16 beacon_intvl,
				   enum network_type nw_type,
				   u8 beacon_ie_len, u8 assoc_req_len,
				   u8 assoc_resp_len, u8 *assoc_info);

void ath6kl_cfg80211_disconnect_event(struct ath6kl_vif *vif, u8 reason,
				      u8 *bssid, u8 assoc_resp_len,
				      u8 *assoc_info, u16 proto_reason);

void ath6kl_cfg80211_tkip_micerr_event(struct ath6kl_vif *vif, u8 keyid,
				     bool ismcast);

int ath6kl_cfg80211_suspend(struct ath6kl *ar,
			    enum ath6kl_cfg_suspend_mode mode,
			    struct cfg80211_wowlan *wow);

int ath6kl_cfg80211_resume(struct ath6kl *ar);

void ath6kl_cfg80211_vif_cleanup(struct ath6kl_vif *vif);

void ath6kl_cfg80211_stop(struct ath6kl_vif *vif);
void ath6kl_cfg80211_stop_all(struct ath6kl *ar);

int ath6kl_cfg80211_init(struct ath6kl *ar);
void ath6kl_cfg80211_cleanup(struct ath6kl *ar);

struct ath6kl *ath6kl_cfg80211_create(void);
void ath6kl_cfg80211_destroy(struct ath6kl *ar);

#endif /* ATH6KL_CFG80211_H */
