/*
 * Copyright (c) 2004-2011 Atheros Communications Inc.
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

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/moduleparam.h>
#include <linux/inetdevice.h>
#include <linux/export.h>
#include <linux/sched/signal.h>

#include "core.h"
#include "cfg80211.h"
#include "debug.h"
#include "hif-ops.h"
#include "testmode.h"

#define RATETAB_ENT(_rate, _rateid, _flags) {   \
	.bitrate    = (_rate),                  \
	.flags      = (_flags),                 \
	.hw_value   = (_rateid),                \
}

#define CHAN2G(_channel, _freq, _flags) {   \
	.band           = NL80211_BAND_2GHZ,  \
	.hw_value       = (_channel),           \
	.center_freq    = (_freq),              \
	.flags          = (_flags),             \
	.max_antenna_gain   = 0,                \
	.max_power      = 30,                   \
}

#define CHAN5G(_channel, _flags) {		    \
	.band           = NL80211_BAND_5GHZ,      \
	.hw_value       = (_channel),               \
	.center_freq    = 5000 + (5 * (_channel)),  \
	.flags          = (_flags),                 \
	.max_antenna_gain   = 0,                    \
	.max_power      = 30,                       \
}

#define DEFAULT_BG_SCAN_PERIOD 60

struct ath6kl_cfg80211_match_probe_ssid {
	struct cfg80211_ssid ssid;
	u8 flag;
};

static struct ieee80211_rate ath6kl_rates[] = {
	RATETAB_ENT(10, 0x1, 0),
	RATETAB_ENT(20, 0x2, 0),
	RATETAB_ENT(55, 0x4, 0),
	RATETAB_ENT(110, 0x8, 0),
	RATETAB_ENT(60, 0x10, 0),
	RATETAB_ENT(90, 0x20, 0),
	RATETAB_ENT(120, 0x40, 0),
	RATETAB_ENT(180, 0x80, 0),
	RATETAB_ENT(240, 0x100, 0),
	RATETAB_ENT(360, 0x200, 0),
	RATETAB_ENT(480, 0x400, 0),
	RATETAB_ENT(540, 0x800, 0),
};

#define ath6kl_a_rates     (ath6kl_rates + 4)
#define ath6kl_a_rates_size    8
#define ath6kl_g_rates     (ath6kl_rates + 0)
#define ath6kl_g_rates_size    12

#define ath6kl_g_htcap IEEE80211_HT_CAP_SGI_20
#define ath6kl_a_htcap (IEEE80211_HT_CAP_SUP_WIDTH_20_40 | \
			IEEE80211_HT_CAP_SGI_20		 | \
			IEEE80211_HT_CAP_SGI_40)

static struct ieee80211_channel ath6kl_2ghz_channels[] = {
	CHAN2G(1, 2412, 0),
	CHAN2G(2, 2417, 0),
	CHAN2G(3, 2422, 0),
	CHAN2G(4, 2427, 0),
	CHAN2G(5, 2432, 0),
	CHAN2G(6, 2437, 0),
	CHAN2G(7, 2442, 0),
	CHAN2G(8, 2447, 0),
	CHAN2G(9, 2452, 0),
	CHAN2G(10, 2457, 0),
	CHAN2G(11, 2462, 0),
	CHAN2G(12, 2467, 0),
	CHAN2G(13, 2472, 0),
	CHAN2G(14, 2484, 0),
};

static struct ieee80211_channel ath6kl_5ghz_a_channels[] = {
	CHAN5G(36, 0), CHAN5G(40, 0),
	CHAN5G(44, 0), CHAN5G(48, 0),
	CHAN5G(52, 0), CHAN5G(56, 0),
	CHAN5G(60, 0), CHAN5G(64, 0),
	CHAN5G(100, 0), CHAN5G(104, 0),
	CHAN5G(108, 0), CHAN5G(112, 0),
	CHAN5G(116, 0), CHAN5G(120, 0),
	CHAN5G(124, 0), CHAN5G(128, 0),
	CHAN5G(132, 0), CHAN5G(136, 0),
	CHAN5G(140, 0), CHAN5G(149, 0),
	CHAN5G(153, 0), CHAN5G(157, 0),
	CHAN5G(161, 0), CHAN5G(165, 0),
	CHAN5G(184, 0), CHAN5G(188, 0),
	CHAN5G(192, 0), CHAN5G(196, 0),
	CHAN5G(200, 0), CHAN5G(204, 0),
	CHAN5G(208, 0), CHAN5G(212, 0),
	CHAN5G(216, 0),
};

static struct ieee80211_supported_band ath6kl_band_2ghz = {
	.n_channels = ARRAY_SIZE(ath6kl_2ghz_channels),
	.channels = ath6kl_2ghz_channels,
	.n_bitrates = ath6kl_g_rates_size,
	.bitrates = ath6kl_g_rates,
	.ht_cap.cap = ath6kl_g_htcap,
	.ht_cap.ht_supported = true,
};

static struct ieee80211_supported_band ath6kl_band_5ghz = {
	.n_channels = ARRAY_SIZE(ath6kl_5ghz_a_channels),
	.channels = ath6kl_5ghz_a_channels,
	.n_bitrates = ath6kl_a_rates_size,
	.bitrates = ath6kl_a_rates,
	.ht_cap.cap = ath6kl_a_htcap,
	.ht_cap.ht_supported = true,
};

#define CCKM_KRK_CIPHER_SUITE 0x004096ff /* use for KRK */

/* returns true if scheduled scan was stopped */
static bool __ath6kl_cfg80211_sscan_stop(struct ath6kl_vif *vif)
{
	struct ath6kl *ar = vif->ar;

	if (!test_and_clear_bit(SCHED_SCANNING, &vif->flags))
		return false;

	del_timer_sync(&vif->sched_scan_timer);

	if (ar->state == ATH6KL_STATE_RECOVERY)
		return true;

	ath6kl_wmi_enable_sched_scan_cmd(ar->wmi, vif->fw_vif_idx, false);

	return true;
}

static void ath6kl_cfg80211_sscan_disable(struct ath6kl_vif *vif)
{
	struct ath6kl *ar = vif->ar;
	bool stopped;

	stopped = __ath6kl_cfg80211_sscan_stop(vif);

	if (!stopped)
		return;

	cfg80211_sched_scan_stopped(ar->wiphy, 0);
}

static int ath6kl_set_wpa_version(struct ath6kl_vif *vif,
				  enum nl80211_wpa_versions wpa_version)
{
	ath6kl_dbg(ATH6KL_DBG_WLAN_CFG, "%s: %u\n", __func__, wpa_version);

	if (!wpa_version) {
		vif->auth_mode = NONE_AUTH;
	} else if (wpa_version & NL80211_WPA_VERSION_2) {
		vif->auth_mode = WPA2_AUTH;
	} else if (wpa_version & NL80211_WPA_VERSION_1) {
		vif->auth_mode = WPA_AUTH;
	} else {
		ath6kl_err("%s: %u not supported\n", __func__, wpa_version);
		return -ENOTSUPP;
	}

	return 0;
}

static int ath6kl_set_auth_type(struct ath6kl_vif *vif,
				enum nl80211_auth_type auth_type)
{
	ath6kl_dbg(ATH6KL_DBG_WLAN_CFG, "%s: 0x%x\n", __func__, auth_type);

	switch (auth_type) {
	case NL80211_AUTHTYPE_OPEN_SYSTEM:
		vif->dot11_auth_mode = OPEN_AUTH;
		break;
	case NL80211_AUTHTYPE_SHARED_KEY:
		vif->dot11_auth_mode = SHARED_AUTH;
		break;
	case NL80211_AUTHTYPE_NETWORK_EAP:
		vif->dot11_auth_mode = LEAP_AUTH;
		break;

	case NL80211_AUTHTYPE_AUTOMATIC:
		vif->dot11_auth_mode = OPEN_AUTH | SHARED_AUTH;
		break;

	default:
		ath6kl_err("%s: 0x%x not supported\n", __func__, auth_type);
		return -ENOTSUPP;
	}

	return 0;
}

static int ath6kl_set_cipher(struct ath6kl_vif *vif, u32 cipher, bool ucast)
{
	u8 *ar_cipher = ucast ? &vif->prwise_crypto : &vif->grp_crypto;
	u8 *ar_cipher_len = ucast ? &vif->prwise_crypto_len :
		&vif->grp_crypto_len;

	ath6kl_dbg(ATH6KL_DBG_WLAN_CFG, "%s: cipher 0x%x, ucast %u\n",
		   __func__, cipher, ucast);

	switch (cipher) {
	case 0:
		/* our own hack to use value 0 as no crypto used */
		*ar_cipher = NONE_CRYPT;
		*ar_cipher_len = 0;
		break;
	case WLAN_CIPHER_SUITE_WEP40:
		*ar_cipher = WEP_CRYPT;
		*ar_cipher_len = 5;
		break;
	case WLAN_CIPHER_SUITE_WEP104:
		*ar_cipher = WEP_CRYPT;
		*ar_cipher_len = 13;
		break;
	case WLAN_CIPHER_SUITE_TKIP:
		*ar_cipher = TKIP_CRYPT;
		*ar_cipher_len = 0;
		break;
	case WLAN_CIPHER_SUITE_CCMP:
		*ar_cipher = AES_CRYPT;
		*ar_cipher_len = 0;
		break;
	case WLAN_CIPHER_SUITE_SMS4:
		*ar_cipher = WAPI_CRYPT;
		*ar_cipher_len = 0;
		break;
	default:
		ath6kl_err("cipher 0x%x not supported\n", cipher);
		return -ENOTSUPP;
	}

	return 0;
}

static void ath6kl_set_key_mgmt(struct ath6kl_vif *vif, u32 key_mgmt)
{
	ath6kl_dbg(ATH6KL_DBG_WLAN_CFG, "%s: 0x%x\n", __func__, key_mgmt);

	if (key_mgmt == WLAN_AKM_SUITE_PSK) {
		if (vif->auth_mode == WPA_AUTH)
			vif->auth_mode = WPA_PSK_AUTH;
		else if (vif->auth_mode == WPA2_AUTH)
			vif->auth_mode = WPA2_PSK_AUTH;
	} else if (key_mgmt == 0x00409600) {
		if (vif->auth_mode == WPA_AUTH)
			vif->auth_mode = WPA_AUTH_CCKM;
		else if (vif->auth_mode == WPA2_AUTH)
			vif->auth_mode = WPA2_AUTH_CCKM;
	} else if (key_mgmt != WLAN_AKM_SUITE_8021X) {
		vif->auth_mode = NONE_AUTH;
	}
}

static bool ath6kl_cfg80211_ready(struct ath6kl_vif *vif)
{
	struct ath6kl *ar = vif->ar;

	if (!test_bit(WMI_READY, &ar->flag)) {
		ath6kl_err("wmi is not ready\n");
		return false;
	}

	if (!test_bit(WLAN_ENABLED, &vif->flags)) {
		ath6kl_dbg(ATH6KL_DBG_WLAN_CFG, "wlan disabled\n");
		return false;
	}

	return true;
}

static bool ath6kl_is_wpa_ie(const u8 *pos)
{
	return pos[0] == WLAN_EID_VENDOR_SPECIFIC && pos[1] >= 4 &&
		pos[2] == 0x00 && pos[3] == 0x50 &&
		pos[4] == 0xf2 && pos[5] == 0x01;
}

static bool ath6kl_is_rsn_ie(const u8 *pos)
{
	return pos[0] == WLAN_EID_RSN;
}

static bool ath6kl_is_wps_ie(const u8 *pos)
{
	return (pos[0] == WLAN_EID_VENDOR_SPECIFIC &&
		pos[1] >= 4 &&
		pos[2] == 0x00 && pos[3] == 0x50 && pos[4] == 0xf2 &&
		pos[5] == 0x04);
}

static int ath6kl_set_assoc_req_ies(struct ath6kl_vif *vif, const u8 *ies,
				    size_t ies_len)
{
	struct ath6kl *ar = vif->ar;
	const u8 *pos;
	u8 *buf = NULL;
	size_t len = 0;
	int ret;

	/*
	 * Clear previously set flag
	 */

	ar->connect_ctrl_flags &= ~CONNECT_WPS_FLAG;

	/*
	 * Filter out RSN/WPA IE(s)
	 */

	if (ies && ies_len) {
		buf = kmalloc(ies_len, GFP_KERNEL);
		if (buf == NULL)
			return -ENOMEM;
		pos = ies;

		while (pos + 1 < ies + ies_len) {
			if (pos + 2 + pos[1] > ies + ies_len)
				break;
			if (!(ath6kl_is_wpa_ie(pos) || ath6kl_is_rsn_ie(pos))) {
				memcpy(buf + len, pos, 2 + pos[1]);
				len += 2 + pos[1];
			}

			if (ath6kl_is_wps_ie(pos))
				ar->connect_ctrl_flags |= CONNECT_WPS_FLAG;

			pos += 2 + pos[1];
		}
	}

	ret = ath6kl_wmi_set_appie_cmd(ar->wmi, vif->fw_vif_idx,
				       WMI_FRAME_ASSOC_REQ, buf, len);
	kfree(buf);
	return ret;
}

static int ath6kl_nliftype_to_drv_iftype(enum nl80211_iftype type, u8 *nw_type)
{
	switch (type) {
	case NL80211_IFTYPE_STATION:
	case NL80211_IFTYPE_P2P_CLIENT:
		*nw_type = INFRA_NETWORK;
		break;
	case NL80211_IFTYPE_ADHOC:
		*nw_type = ADHOC_NETWORK;
		break;
	case NL80211_IFTYPE_AP:
	case NL80211_IFTYPE_P2P_GO:
		*nw_type = AP_NETWORK;
		break;
	default:
		ath6kl_err("invalid interface type %u\n", type);
		return -ENOTSUPP;
	}

	return 0;
}

static bool ath6kl_is_valid_iftype(struct ath6kl *ar, enum nl80211_iftype type,
				   u8 *if_idx, u8 *nw_type)
{
	int i;

	if (ath6kl_nliftype_to_drv_iftype(type, nw_type))
		return false;

	if (ar->ibss_if_active || ((type == NL80211_IFTYPE_ADHOC) &&
				   ar->num_vif))
		return false;

	if (type == NL80211_IFTYPE_STATION ||
	    type == NL80211_IFTYPE_AP || type == NL80211_IFTYPE_ADHOC) {
		for (i = 0; i < ar->vif_max; i++) {
			if ((ar->avail_idx_map) & BIT(i)) {
				*if_idx = i;
				return true;
			}
		}
	}

	if (type == NL80211_IFTYPE_P2P_CLIENT ||
	    type == NL80211_IFTYPE_P2P_GO) {
		for (i = ar->max_norm_iface; i < ar->vif_max; i++) {
			if ((ar->avail_idx_map) & BIT(i)) {
				*if_idx = i;
				return true;
			}
		}
	}

	return false;
}

static bool ath6kl_is_tx_pending(struct ath6kl *ar)
{
	return ar->tx_pending[ath6kl_wmi_get_control_ep(ar->wmi)] == 0;
}

static void ath6kl_cfg80211_sta_bmiss_enhance(struct ath6kl_vif *vif,
					      bool enable)
{
	int err;

	if (WARN_ON(!test_bit(WMI_READY, &vif->ar->flag)))
		return;

	if (vif->nw_type != INFRA_NETWORK)
		return;

	if (!test_bit(ATH6KL_FW_CAPABILITY_BMISS_ENHANCE,
		      vif->ar->fw_capabilities))
		return;

	ath6kl_dbg(ATH6KL_DBG_WLAN_CFG, "%s fw bmiss enhance\n",
		   enable ? "enable" : "disable");

	err = ath6kl_wmi_sta_bmiss_enhance_cmd(vif->ar->wmi,
					       vif->fw_vif_idx, enable);
	if (err)
		ath6kl_err("failed to %s enhanced bmiss detection: %d\n",
			   enable ? "enable" : "disable", err);
}

static int ath6kl_cfg80211_connect(struct wiphy *wiphy, struct net_device *dev,
				   struct cfg80211_connect_params *sme)
{
	struct ath6kl *ar = ath6kl_priv(dev);
	struct ath6kl_vif *vif = netdev_priv(dev);
	int status;
	u8 nw_subtype = (ar->p2p) ? SUBTYPE_P2PDEV : SUBTYPE_NONE;
	u16 interval;

	ath6kl_cfg80211_sscan_disable(vif);

	vif->sme_state = SME_CONNECTING;

	if (!ath6kl_cfg80211_ready(vif))
		return -EIO;

	if (test_bit(DESTROY_IN_PROGRESS, &ar->flag)) {
		ath6kl_err("destroy in progress\n");
		return -EBUSY;
	}

	if (test_bit(SKIP_SCAN, &ar->flag) &&
	    ((sme->channel && sme->channel->center_freq == 0) ||
	     (sme->bssid && is_zero_ether_addr(sme->bssid)))) {
		ath6kl_err("SkipScan: channel or bssid invalid\n");
		return -EINVAL;
	}

	if (down_interruptible(&ar->sem)) {
		ath6kl_err("busy, couldn't get access\n");
		return -ERESTARTSYS;
	}

	if (test_bit(DESTROY_IN_PROGRESS, &ar->flag)) {
		ath6kl_err("busy, destroy in progress\n");
		up(&ar->sem);
		return -EBUSY;
	}

	if (ar->tx_pending[ath6kl_wmi_get_control_ep(ar->wmi)]) {
		/*
		 * sleep until the command queue drains
		 */
		wait_event_interruptible_timeout(ar->event_wq,
						 ath6kl_is_tx_pending(ar),
						 WMI_TIMEOUT);
		if (signal_pending(current)) {
			ath6kl_err("cmd queue drain timeout\n");
			up(&ar->sem);
			return -EINTR;
		}
	}

	status = ath6kl_set_assoc_req_ies(vif, sme->ie, sme->ie_len);
	if (status) {
		up(&ar->sem);
		return status;
	}

	if (sme->ie == NULL || sme->ie_len == 0)
		ar->connect_ctrl_flags &= ~CONNECT_WPS_FLAG;

	if (test_bit(CONNECTED, &vif->flags) &&
	    vif->ssid_len == sme->ssid_len &&
	    !memcmp(vif->ssid, sme->ssid, vif->ssid_len)) {
		vif->reconnect_flag = true;
		status = ath6kl_wmi_reconnect_cmd(ar->wmi, vif->fw_vif_idx,
						  vif->req_bssid,
						  vif->ch_hint);

		up(&ar->sem);
		if (status) {
			ath6kl_err("wmi_reconnect_cmd failed\n");
			return -EIO;
		}
		return 0;
	} else if (vif->ssid_len == sme->ssid_len &&
		   !memcmp(vif->ssid, sme->ssid, vif->ssid_len)) {
		ath6kl_disconnect(vif);
	}

	memset(vif->ssid, 0, sizeof(vif->ssid));
	vif->ssid_len = sme->ssid_len;
	memcpy(vif->ssid, sme->ssid, sme->ssid_len);

	if (sme->channel)
		vif->ch_hint = sme->channel->center_freq;

	memset(vif->req_bssid, 0, sizeof(vif->req_bssid));
	if (sme->bssid && !is_broadcast_ether_addr(sme->bssid))
		memcpy(vif->req_bssid, sme->bssid, sizeof(vif->req_bssid));

	ath6kl_set_wpa_version(vif, sme->crypto.wpa_versions);

	status = ath6kl_set_auth_type(vif, sme->auth_type);
	if (status) {
		up(&ar->sem);
		return status;
	}

	if (sme->crypto.n_ciphers_pairwise)
		ath6kl_set_cipher(vif, sme->crypto.ciphers_pairwise[0], true);
	else
		ath6kl_set_cipher(vif, 0, true);

	ath6kl_set_cipher(vif, sme->crypto.cipher_group, false);

	if (sme->crypto.n_akm_suites)
		ath6kl_set_key_mgmt(vif, sme->crypto.akm_suites[0]);

	if ((sme->key_len) &&
	    (vif->auth_mode == NONE_AUTH) &&
	    (vif->prwise_crypto == WEP_CRYPT)) {
		struct ath6kl_key *key = NULL;

		if (sme->key_idx > WMI_MAX_KEY_INDEX) {
			ath6kl_err("key index %d out of bounds\n",
				   sme->key_idx);
			up(&ar->sem);
			return -ENOENT;
		}

		key = &vif->keys[sme->key_idx];
		key->key_len = sme->key_len;
		memcpy(key->key, sme->key, key->key_len);
		key->cipher = vif->prwise_crypto;
		vif->def_txkey_index = sme->key_idx;

		ath6kl_wmi_addkey_cmd(ar->wmi, vif->fw_vif_idx, sme->key_idx,
				      vif->prwise_crypto,
				      GROUP_USAGE | TX_USAGE,
				      key->key_len,
				      NULL, 0,
				      key->key, KEY_OP_INIT_VAL, NULL,
				      NO_SYNC_WMIFLAG);
	}

	if (!ar->usr_bss_filter) {
		clear_bit(CLEAR_BSSFILTER_ON_BEACON, &vif->flags);
		if (ath6kl_wmi_bssfilter_cmd(ar->wmi, vif->fw_vif_idx,
					     ALL_BSS_FILTER, 0) != 0) {
			ath6kl_err("couldn't set bss filtering\n");
			up(&ar->sem);
			return -EIO;
		}
	}

	vif->nw_type = vif->next_mode;

	/* enable enhanced bmiss detection if applicable */
	ath6kl_cfg80211_sta_bmiss_enhance(vif, true);

	if (vif->wdev.iftype == NL80211_IFTYPE_P2P_CLIENT)
		nw_subtype = SUBTYPE_P2PCLIENT;

	ath6kl_dbg(ATH6KL_DBG_WLAN_CFG,
		   "%s: connect called with authmode %d dot11 auth %d"
		   " PW crypto %d PW crypto len %d GRP crypto %d"
		   " GRP crypto len %d channel hint %u\n",
		   __func__,
		   vif->auth_mode, vif->dot11_auth_mode, vif->prwise_crypto,
		   vif->prwise_crypto_len, vif->grp_crypto,
		   vif->grp_crypto_len, vif->ch_hint);

	vif->reconnect_flag = 0;

	if (vif->nw_type == INFRA_NETWORK) {
		interval = max_t(u16, vif->listen_intvl_t,
				 ATH6KL_MAX_WOW_LISTEN_INTL);
		status = ath6kl_wmi_listeninterval_cmd(ar->wmi, vif->fw_vif_idx,
						       interval,
						       0);
		if (status) {
			ath6kl_err("couldn't set listen intervel\n");
			up(&ar->sem);
			return status;
		}
	}

	status = ath6kl_wmi_connect_cmd(ar->wmi, vif->fw_vif_idx, vif->nw_type,
					vif->dot11_auth_mode, vif->auth_mode,
					vif->prwise_crypto,
					vif->prwise_crypto_len,
					vif->grp_crypto, vif->grp_crypto_len,
					vif->ssid_len, vif->ssid,
					vif->req_bssid, vif->ch_hint,
					ar->connect_ctrl_flags, nw_subtype);

	if (sme->bg_scan_period == 0) {
		/* disable background scan if period is 0 */
		sme->bg_scan_period = 0xffff;
	} else if (sme->bg_scan_period == -1) {
		/* configure default value if not specified */
		sme->bg_scan_period = DEFAULT_BG_SCAN_PERIOD;
	}

	ath6kl_wmi_scanparams_cmd(ar->wmi, vif->fw_vif_idx, 0, 0,
				  sme->bg_scan_period, 0, 0, 0, 3, 0, 0, 0);

	up(&ar->sem);

	if (status == -EINVAL) {
		memset(vif->ssid, 0, sizeof(vif->ssid));
		vif->ssid_len = 0;
		ath6kl_err("invalid request\n");
		return -ENOENT;
	} else if (status) {
		ath6kl_err("ath6kl_wmi_connect_cmd failed\n");
		return -EIO;
	}

	if ((!(ar->connect_ctrl_flags & CONNECT_DO_WPA_OFFLOAD)) &&
	    ((vif->auth_mode == WPA_PSK_AUTH) ||
	     (vif->auth_mode == WPA2_PSK_AUTH))) {
		mod_timer(&vif->disconnect_timer,
			  jiffies + msecs_to_jiffies(DISCON_TIMER_INTVAL));
	}

	ar->connect_ctrl_flags &= ~CONNECT_DO_WPA_OFFLOAD;
	set_bit(CONNECT_PEND, &vif->flags);

	return 0;
}

static struct cfg80211_bss *
ath6kl_add_bss_if_needed(struct ath6kl_vif *vif,
			 enum network_type nw_type,
			 const u8 *bssid,
			 struct ieee80211_channel *chan,
			 const u8 *beacon_ie,
			 size_t beacon_ie_len)
{
	struct ath6kl *ar = vif->ar;
	struct cfg80211_bss *bss;
	u16 cap_val;
	enum ieee80211_bss_type bss_type;
	u8 *ie;

	if (nw_type & ADHOC_NETWORK) {
		cap_val = WLAN_CAPABILITY_IBSS;
		bss_type = IEEE80211_BSS_TYPE_IBSS;
	} else {
		cap_val = WLAN_CAPABILITY_ESS;
		bss_type = IEEE80211_BSS_TYPE_ESS;
	}

	bss = cfg80211_get_bss(ar->wiphy, chan, bssid,
			       vif->ssid, vif->ssid_len,
			       bss_type, IEEE80211_PRIVACY_ANY);
	if (bss == NULL) {
		/*
		 * Since cfg80211 may not yet know about the BSS,
		 * generate a partial entry until the first BSS info
		 * event becomes available.
		 *
		 * Prepend SSID element since it is not included in the Beacon
		 * IEs from the target.
		 */
		ie = kmalloc(2 + vif->ssid_len + beacon_ie_len, GFP_KERNEL);
		if (ie == NULL)
			return NULL;
		ie[0] = WLAN_EID_SSID;
		ie[1] = vif->ssid_len;
		memcpy(ie + 2, vif->ssid, vif->ssid_len);
		memcpy(ie + 2 + vif->ssid_len, beacon_ie, beacon_ie_len);
		bss = cfg80211_inform_bss(ar->wiphy, chan,
					  CFG80211_BSS_FTYPE_UNKNOWN,
					  bssid, 0, cap_val, 100,
					  ie, 2 + vif->ssid_len + beacon_ie_len,
					  0, GFP_KERNEL);
		if (bss)
			ath6kl_dbg(ATH6KL_DBG_WLAN_CFG,
				   "added bss %pM to cfg80211\n", bssid);
		kfree(ie);
	} else {
		ath6kl_dbg(ATH6KL_DBG_WLAN_CFG, "cfg80211 already has a bss\n");
	}

	return bss;
}

void ath6kl_cfg80211_connect_event(struct ath6kl_vif *vif, u16 channel,
				   u8 *bssid, u16 listen_intvl,
				   u16 beacon_intvl,
				   enum network_type nw_type,
				   u8 beacon_ie_len, u8 assoc_req_len,
				   u8 assoc_resp_len, u8 *assoc_info)
{
	struct ieee80211_channel *chan;
	struct ath6kl *ar = vif->ar;
	struct cfg80211_bss *bss;

	/* capinfo + listen interval */
	u8 assoc_req_ie_offset = sizeof(u16) + sizeof(u16);

	/* capinfo + status code +  associd */
	u8 assoc_resp_ie_offset = sizeof(u16) + sizeof(u16) + sizeof(u16);

	u8 *assoc_req_ie = assoc_info + beacon_ie_len + assoc_req_ie_offset;
	u8 *assoc_resp_ie = assoc_info + beacon_ie_len + assoc_req_len +
	    assoc_resp_ie_offset;

	assoc_req_len -= assoc_req_ie_offset;
	assoc_resp_len -= assoc_resp_ie_offset;

	/*
	 * Store Beacon interval here; DTIM period will be available only once
	 * a Beacon frame from the AP is seen.
	 */
	vif->assoc_bss_beacon_int = beacon_intvl;
	clear_bit(DTIM_PERIOD_AVAIL, &vif->flags);

	if (nw_type & ADHOC_NETWORK) {
		if (vif->wdev.iftype != NL80211_IFTYPE_ADHOC) {
			ath6kl_dbg(ATH6KL_DBG_WLAN_CFG,
				   "%s: ath6k not in ibss mode\n", __func__);
			return;
		}
	}

	if (nw_type & INFRA_NETWORK) {
		if (vif->wdev.iftype != NL80211_IFTYPE_STATION &&
		    vif->wdev.iftype != NL80211_IFTYPE_P2P_CLIENT) {
			ath6kl_dbg(ATH6KL_DBG_WLAN_CFG,
				   "%s: ath6k not in station mode\n", __func__);
			return;
		}
	}

	chan = ieee80211_get_channel(ar->wiphy, (int) channel);

	bss = ath6kl_add_bss_if_needed(vif, nw_type, bssid, chan,
				       assoc_info, beacon_ie_len);
	if (!bss) {
		ath6kl_err("could not add cfg80211 bss entry\n");
		return;
	}

	if (nw_type & ADHOC_NETWORK) {
		ath6kl_dbg(ATH6KL_DBG_WLAN_CFG, "ad-hoc %s selected\n",
			   nw_type & ADHOC_CREATOR ? "creator" : "joiner");
		cfg80211_ibss_joined(vif->ndev, bssid, chan, GFP_KERNEL);
		cfg80211_put_bss(ar->wiphy, bss);
		return;
	}

	if (vif->sme_state == SME_CONNECTING) {
		/* inform connect result to cfg80211 */
		vif->sme_state = SME_CONNECTED;
		cfg80211_connect_result(vif->ndev, bssid,
					assoc_req_ie, assoc_req_len,
					assoc_resp_ie, assoc_resp_len,
					WLAN_STATUS_SUCCESS, GFP_KERNEL);
		cfg80211_put_bss(ar->wiphy, bss);
	} else if (vif->sme_state == SME_CONNECTED) {
		struct cfg80211_roam_info roam_info = {
			.bss = bss,
			.req_ie = assoc_req_ie,
			.req_ie_len = assoc_req_len,
			.resp_ie = assoc_resp_ie,
			.resp_ie_len = assoc_resp_len,
		};
		/* inform roam event to cfg80211 */
		cfg80211_roamed(vif->ndev, &roam_info, GFP_KERNEL);
	}
}

static int ath6kl_cfg80211_disconnect(struct wiphy *wiphy,
				      struct net_device *dev, u16 reason_code)
{
	struct ath6kl *ar = ath6kl_priv(dev);
	struct ath6kl_vif *vif = netdev_priv(dev);

	ath6kl_dbg(ATH6KL_DBG_WLAN_CFG, "%s: reason=%u\n", __func__,
		   reason_code);

	ath6kl_cfg80211_sscan_disable(vif);

	if (!ath6kl_cfg80211_ready(vif))
		return -EIO;

	if (test_bit(DESTROY_IN_PROGRESS, &ar->flag)) {
		ath6kl_err("busy, destroy in progress\n");
		return -EBUSY;
	}

	if (down_interruptible(&ar->sem)) {
		ath6kl_err("busy, couldn't get access\n");
		return -ERESTARTSYS;
	}

	vif->reconnect_flag = 0;
	ath6kl_disconnect(vif);
	memset(vif->ssid, 0, sizeof(vif->ssid));
	vif->ssid_len = 0;

	if (!test_bit(SKIP_SCAN, &ar->flag))
		memset(vif->req_bssid, 0, sizeof(vif->req_bssid));

	up(&ar->sem);

	return 0;
}

void ath6kl_cfg80211_disconnect_event(struct ath6kl_vif *vif, u8 reason,
				      u8 *bssid, u8 assoc_resp_len,
				      u8 *assoc_info, u16 proto_reason)
{
	struct ath6kl *ar = vif->ar;

	if (vif->scan_req) {
		struct cfg80211_scan_info info = {
			.aborted = true,
		};

		cfg80211_scan_done(vif->scan_req, &info);
		vif->scan_req = NULL;
	}

	if (vif->nw_type & ADHOC_NETWORK) {
		if (vif->wdev.iftype != NL80211_IFTYPE_ADHOC)
			ath6kl_dbg(ATH6KL_DBG_WLAN_CFG,
				   "%s: ath6k not in ibss mode\n", __func__);
		return;
	}

	if (vif->nw_type & INFRA_NETWORK) {
		if (vif->wdev.iftype != NL80211_IFTYPE_STATION &&
		    vif->wdev.iftype != NL80211_IFTYPE_P2P_CLIENT) {
			ath6kl_dbg(ATH6KL_DBG_WLAN_CFG,
				   "%s: ath6k not in station mode\n", __func__);
			return;
		}
	}

	clear_bit(CONNECT_PEND, &vif->flags);

	if (vif->sme_state == SME_CONNECTING) {
		cfg80211_connect_result(vif->ndev,
					bssid, NULL, 0,
					NULL, 0,
					WLAN_STATUS_UNSPECIFIED_FAILURE,
					GFP_KERNEL);
	} else if (vif->sme_state == SME_CONNECTED) {
		cfg80211_disconnected(vif->ndev, proto_reason,
				      NULL, 0, false, GFP_KERNEL);
	}

	vif->sme_state = SME_DISCONNECTED;

	/*
	 * Send a disconnect command to target when a disconnect event is
	 * received with reason code other than 3 (DISCONNECT_CMD - disconnect
	 * request from host) to make the firmware stop trying to connect even
	 * after giving disconnect event. There will be one more disconnect
	 * event for this disconnect command with reason code DISCONNECT_CMD
	 * which won't be notified to cfg80211.
	 */
	if (reason != DISCONNECT_CMD)
		ath6kl_wmi_disconnect_cmd(ar->wmi, vif->fw_vif_idx);
}

static int ath6kl_set_probed_ssids(struct ath6kl *ar,
				   struct ath6kl_vif *vif,
				   struct cfg80211_ssid *ssids, int n_ssids,
				   struct cfg80211_match_set *match_set,
				   int n_match_ssid)
{
	u8 i, j, index_to_add, ssid_found = false;
	struct ath6kl_cfg80211_match_probe_ssid ssid_list[MAX_PROBED_SSIDS];

	memset(ssid_list, 0, sizeof(ssid_list));

	if (n_ssids > MAX_PROBED_SSIDS ||
	    n_match_ssid > MAX_PROBED_SSIDS)
		return -EINVAL;

	for (i = 0; i < n_ssids; i++) {
		memcpy(ssid_list[i].ssid.ssid,
		       ssids[i].ssid,
		       ssids[i].ssid_len);
		ssid_list[i].ssid.ssid_len = ssids[i].ssid_len;

		if (ssids[i].ssid_len)
			ssid_list[i].flag = SPECIFIC_SSID_FLAG;
		else
			ssid_list[i].flag = ANY_SSID_FLAG;

		if (ar->wiphy->max_match_sets != 0 && n_match_ssid == 0)
			ssid_list[i].flag |= MATCH_SSID_FLAG;
	}

	index_to_add = i;

	for (i = 0; i < n_match_ssid; i++) {
		ssid_found = false;

		for (j = 0; j < n_ssids; j++) {
			if ((match_set[i].ssid.ssid_len ==
			     ssid_list[j].ssid.ssid_len) &&
			    (!memcmp(ssid_list[j].ssid.ssid,
				     match_set[i].ssid.ssid,
				     match_set[i].ssid.ssid_len))) {
				ssid_list[j].flag |= MATCH_SSID_FLAG;
				ssid_found = true;
				break;
			}
		}

		if (ssid_found)
			continue;

		if (index_to_add >= MAX_PROBED_SSIDS)
			continue;

		ssid_list[index_to_add].ssid.ssid_len =
			match_set[i].ssid.ssid_len;
		memcpy(ssid_list[index_to_add].ssid.ssid,
		       match_set[i].ssid.ssid,
		       match_set[i].ssid.ssid_len);
		ssid_list[index_to_add].flag |= MATCH_SSID_FLAG;
		index_to_add++;
	}

	for (i = 0; i < index_to_add; i++) {
		ath6kl_wmi_probedssid_cmd(ar->wmi, vif->fw_vif_idx, i,
					  ssid_list[i].flag,
					  ssid_list[i].ssid.ssid_len,
					  ssid_list[i].ssid.ssid);
	}

	/* Make sure no old entries are left behind */
	for (i = index_to_add; i < MAX_PROBED_SSIDS; i++) {
		ath6kl_wmi_probedssid_cmd(ar->wmi, vif->fw_vif_idx, i,
					  DISABLE_SSID_FLAG, 0, NULL);
	}

	return 0;
}

static int ath6kl_cfg80211_scan(struct wiphy *wiphy,
				struct cfg80211_scan_request *request)
{
	struct ath6kl_vif *vif = ath6kl_vif_from_wdev(request->wdev);
	struct ath6kl *ar = ath6kl_priv(vif->ndev);
	s8 n_channels = 0;
	u16 *channels = NULL;
	int ret = 0;
	u32 force_fg_scan = 0;

	if (!ath6kl_cfg80211_ready(vif))
		return -EIO;

	ath6kl_cfg80211_sscan_disable(vif);

	if (!ar->usr_bss_filter) {
		clear_bit(CLEAR_BSSFILTER_ON_BEACON, &vif->flags);
		ret = ath6kl_wmi_bssfilter_cmd(ar->wmi, vif->fw_vif_idx,
					       ALL_BSS_FILTER, 0);
		if (ret) {
			ath6kl_err("couldn't set bss filtering\n");
			return ret;
		}
	}

	ret = ath6kl_set_probed_ssids(ar, vif, request->ssids,
				      request->n_ssids, NULL, 0);
	if (ret < 0)
		return ret;

	/* this also clears IE in fw if it's not set */
	ret = ath6kl_wmi_set_appie_cmd(ar->wmi, vif->fw_vif_idx,
				       WMI_FRAME_PROBE_REQ,
				       request->ie, request->ie_len);
	if (ret) {
		ath6kl_err("failed to set Probe Request appie for scan\n");
		return ret;
	}

	/*
	 * Scan only the requested channels if the request specifies a set of
	 * channels. If the list is longer than the target supports, do not
	 * configure the list and instead, scan all available channels.
	 */
	if (request->n_channels > 0 &&
	    request->n_channels <= WMI_MAX_CHANNELS) {
		u8 i;

		n_channels = request->n_channels;

		channels = kcalloc(n_channels, sizeof(u16), GFP_KERNEL);
		if (channels == NULL) {
			ath6kl_warn("failed to set scan channels, scan all channels");
			n_channels = 0;
		}

		for (i = 0; i < n_channels; i++)
			channels[i] = request->channels[i]->center_freq;
	}

	if (test_bit(CONNECTED, &vif->flags))
		force_fg_scan = 1;

	vif->scan_req = request;

	ret = ath6kl_wmi_beginscan_cmd(ar->wmi, vif->fw_vif_idx,
				       WMI_LONG_SCAN, force_fg_scan,
				       false, 0,
				       ATH6KL_FG_SCAN_INTERVAL,
				       n_channels, channels,
				       request->no_cck,
				       request->rates);
	if (ret) {
		ath6kl_err("failed to start scan: %d\n", ret);
		vif->scan_req = NULL;
	}

	kfree(channels);

	return ret;
}

void ath6kl_cfg80211_scan_complete_event(struct ath6kl_vif *vif, bool aborted)
{
	struct ath6kl *ar = vif->ar;
	struct cfg80211_scan_info info = {
		.aborted = aborted,
	};
	int i;

	ath6kl_dbg(ATH6KL_DBG_WLAN_CFG, "%s: status%s\n", __func__,
		   aborted ? " aborted" : "");

	if (!vif->scan_req)
		return;

	if (aborted)
		goto out;

	if (vif->scan_req->n_ssids && vif->scan_req->ssids[0].ssid_len) {
		for (i = 0; i < vif->scan_req->n_ssids; i++) {
			ath6kl_wmi_probedssid_cmd(ar->wmi, vif->fw_vif_idx,
						  i, DISABLE_SSID_FLAG,
						  0, NULL);
		}
	}

out:
	cfg80211_scan_done(vif->scan_req, &info);
	vif->scan_req = NULL;
}

void ath6kl_cfg80211_ch_switch_notify(struct ath6kl_vif *vif, int freq,
				      enum wmi_phy_mode mode)
{
	struct cfg80211_chan_def chandef;

	ath6kl_dbg(ATH6KL_DBG_WLAN_CFG,
		   "channel switch notify nw_type %d freq %d mode %d\n",
		   vif->nw_type, freq, mode);

	cfg80211_chandef_create(&chandef,
				ieee80211_get_channel(vif->ar->wiphy, freq),
				(mode == WMI_11G_HT20 &&
				 ath6kl_band_2ghz.ht_cap.ht_supported) ?
					NL80211_CHAN_HT20 : NL80211_CHAN_NO_HT);

	mutex_lock(&vif->wdev.mtx);
	cfg80211_ch_switch_notify(vif->ndev, &chandef);
	mutex_unlock(&vif->wdev.mtx);
}

static int ath6kl_cfg80211_add_key(struct wiphy *wiphy, struct net_device *ndev,
				   u8 key_index, bool pairwise,
				   const u8 *mac_addr,
				   struct key_params *params)
{
	struct ath6kl *ar = ath6kl_priv(ndev);
	struct ath6kl_vif *vif = netdev_priv(ndev);
	struct ath6kl_key *key = NULL;
	int seq_len;
	u8 key_usage;
	u8 key_type;

	if (!ath6kl_cfg80211_ready(vif))
		return -EIO;

	if (params->cipher == CCKM_KRK_CIPHER_SUITE) {
		if (params->key_len != WMI_KRK_LEN)
			return -EINVAL;
		return ath6kl_wmi_add_krk_cmd(ar->wmi, vif->fw_vif_idx,
					      params->key);
	}

	if (key_index > WMI_MAX_KEY_INDEX) {
		ath6kl_dbg(ATH6KL_DBG_WLAN_CFG,
			   "%s: key index %d out of bounds\n", __func__,
			   key_index);
		return -ENOENT;
	}

	key = &vif->keys[key_index];
	memset(key, 0, sizeof(struct ath6kl_key));

	if (pairwise)
		key_usage = PAIRWISE_USAGE;
	else
		key_usage = GROUP_USAGE;

	seq_len = params->seq_len;
	if (params->cipher == WLAN_CIPHER_SUITE_SMS4 &&
	    seq_len > ATH6KL_KEY_SEQ_LEN) {
		/* Only first half of the WPI PN is configured */
		seq_len = ATH6KL_KEY_SEQ_LEN;
	}
	if (params->key_len > WLAN_MAX_KEY_LEN ||
	    seq_len > sizeof(key->seq))
		return -EINVAL;

	key->key_len = params->key_len;
	memcpy(key->key, params->key, key->key_len);
	key->seq_len = seq_len;
	memcpy(key->seq, params->seq, key->seq_len);
	key->cipher = params->cipher;

	switch (key->cipher) {
	case WLAN_CIPHER_SUITE_WEP40:
	case WLAN_CIPHER_SUITE_WEP104:
		key_type = WEP_CRYPT;
		break;

	case WLAN_CIPHER_SUITE_TKIP:
		key_type = TKIP_CRYPT;
		break;

	case WLAN_CIPHER_SUITE_CCMP:
		key_type = AES_CRYPT;
		break;
	case WLAN_CIPHER_SUITE_SMS4:
		key_type = WAPI_CRYPT;
		break;

	default:
		return -ENOTSUPP;
	}

	if (((vif->auth_mode == WPA_PSK_AUTH) ||
	     (vif->auth_mode == WPA2_PSK_AUTH)) &&
	    (key_usage & GROUP_USAGE))
		del_timer(&vif->disconnect_timer);

	ath6kl_dbg(ATH6KL_DBG_WLAN_CFG,
		   "%s: index %d, key_len %d, key_type 0x%x, key_usage 0x%x, seq_len %d\n",
		   __func__, key_index, key->key_len, key_type,
		   key_usage, key->seq_len);

	if (vif->nw_type == AP_NETWORK && !pairwise &&
	    (key_type == TKIP_CRYPT || key_type == AES_CRYPT ||
	     key_type == WAPI_CRYPT)) {
		ar->ap_mode_bkey.valid = true;
		ar->ap_mode_bkey.key_index = key_index;
		ar->ap_mode_bkey.key_type = key_type;
		ar->ap_mode_bkey.key_len = key->key_len;
		memcpy(ar->ap_mode_bkey.key, key->key, key->key_len);
		if (!test_bit(CONNECTED, &vif->flags)) {
			ath6kl_dbg(ATH6KL_DBG_WLAN_CFG,
				   "Delay initial group key configuration until AP mode has been started\n");
			/*
			 * The key will be set in ath6kl_connect_ap_mode() once
			 * the connected event is received from the target.
			 */
			return 0;
		}
	}

	if (vif->next_mode == AP_NETWORK && key_type == WEP_CRYPT &&
	    !test_bit(CONNECTED, &vif->flags)) {
		/*
		 * Store the key locally so that it can be re-configured after
		 * the AP mode has properly started
		 * (ath6kl_install_statioc_wep_keys).
		 */
		ath6kl_dbg(ATH6KL_DBG_WLAN_CFG,
			   "Delay WEP key configuration until AP mode has been started\n");
		vif->wep_key_list[key_index].key_len = key->key_len;
		memcpy(vif->wep_key_list[key_index].key, key->key,
		       key->key_len);
		return 0;
	}

	return ath6kl_wmi_addkey_cmd(ar->wmi, vif->fw_vif_idx, key_index,
				     key_type, key_usage, key->key_len,
				     key->seq, key->seq_len, key->key,
				     KEY_OP_INIT_VAL,
				     (u8 *) mac_addr, SYNC_BOTH_WMIFLAG);
}

static int ath6kl_cfg80211_del_key(struct wiphy *wiphy, struct net_device *ndev,
				   u8 key_index, bool pairwise,
				   const u8 *mac_addr)
{
	struct ath6kl *ar = ath6kl_priv(ndev);
	struct ath6kl_vif *vif = netdev_priv(ndev);

	ath6kl_dbg(ATH6KL_DBG_WLAN_CFG, "%s: index %d\n", __func__, key_index);

	if (!ath6kl_cfg80211_ready(vif))
		return -EIO;

	if (key_index > WMI_MAX_KEY_INDEX) {
		ath6kl_dbg(ATH6KL_DBG_WLAN_CFG,
			   "%s: key index %d out of bounds\n", __func__,
			   key_index);
		return -ENOENT;
	}

	if (!vif->keys[key_index].key_len) {
		ath6kl_dbg(ATH6KL_DBG_WLAN_CFG,
			   "%s: index %d is empty\n", __func__, key_index);
		return 0;
	}

	vif->keys[key_index].key_len = 0;

	return ath6kl_wmi_deletekey_cmd(ar->wmi, vif->fw_vif_idx, key_index);
}

static int ath6kl_cfg80211_get_key(struct wiphy *wiphy, struct net_device *ndev,
				   u8 key_index, bool pairwise,
				   const u8 *mac_addr, void *cookie,
				   void (*callback) (void *cookie,
						     struct key_params *))
{
	struct ath6kl_vif *vif = netdev_priv(ndev);
	struct ath6kl_key *key = NULL;
	struct key_params params;

	ath6kl_dbg(ATH6KL_DBG_WLAN_CFG, "%s: index %d\n", __func__, key_index);

	if (!ath6kl_cfg80211_ready(vif))
		return -EIO;

	if (key_index > WMI_MAX_KEY_INDEX) {
		ath6kl_dbg(ATH6KL_DBG_WLAN_CFG,
			   "%s: key index %d out of bounds\n", __func__,
			   key_index);
		return -ENOENT;
	}

	key = &vif->keys[key_index];
	memset(&params, 0, sizeof(params));
	params.cipher = key->cipher;
	params.key_len = key->key_len;
	params.seq_len = key->seq_len;
	params.seq = key->seq;
	params.key = key->key;

	callback(cookie, &params);

	return key->key_len ? 0 : -ENOENT;
}

static int ath6kl_cfg80211_set_default_key(struct wiphy *wiphy,
					   struct net_device *ndev,
					   u8 key_index, bool unicast,
					   bool multicast)
{
	struct ath6kl *ar = ath6kl_priv(ndev);
	struct ath6kl_vif *vif = netdev_priv(ndev);
	struct ath6kl_key *key = NULL;
	u8 key_usage;
	enum ath6kl_crypto_type key_type = NONE_CRYPT;

	ath6kl_dbg(ATH6KL_DBG_WLAN_CFG, "%s: index %d\n", __func__, key_index);

	if (!ath6kl_cfg80211_ready(vif))
		return -EIO;

	if (key_index > WMI_MAX_KEY_INDEX) {
		ath6kl_dbg(ATH6KL_DBG_WLAN_CFG,
			   "%s: key index %d out of bounds\n",
			   __func__, key_index);
		return -ENOENT;
	}

	if (!vif->keys[key_index].key_len) {
		ath6kl_dbg(ATH6KL_DBG_WLAN_CFG, "%s: invalid key index %d\n",
			   __func__, key_index);
		return -EINVAL;
	}

	vif->def_txkey_index = key_index;
	key = &vif->keys[vif->def_txkey_index];
	key_usage = GROUP_USAGE;
	if (vif->prwise_crypto == WEP_CRYPT)
		key_usage |= TX_USAGE;
	if (unicast)
		key_type = vif->prwise_crypto;
	if (multicast)
		key_type = vif->grp_crypto;

	if (vif->next_mode == AP_NETWORK && !test_bit(CONNECTED, &vif->flags))
		return 0; /* Delay until AP mode has been started */

	return ath6kl_wmi_addkey_cmd(ar->wmi, vif->fw_vif_idx,
				     vif->def_txkey_index,
				     key_type, key_usage,
				     key->key_len, key->seq, key->seq_len,
				     key->key,
				     KEY_OP_INIT_VAL, NULL,
				     SYNC_BOTH_WMIFLAG);
}

void ath6kl_cfg80211_tkip_micerr_event(struct ath6kl_vif *vif, u8 keyid,
				       bool ismcast)
{
	ath6kl_dbg(ATH6KL_DBG_WLAN_CFG,
		   "%s: keyid %d, ismcast %d\n", __func__, keyid, ismcast);

	cfg80211_michael_mic_failure(vif->ndev, vif->bssid,
				     (ismcast ? NL80211_KEYTYPE_GROUP :
				      NL80211_KEYTYPE_PAIRWISE), keyid, NULL,
				     GFP_KERNEL);
}

static int ath6kl_cfg80211_set_wiphy_params(struct wiphy *wiphy, u32 changed)
{
	struct ath6kl *ar = (struct ath6kl *)wiphy_priv(wiphy);
	struct ath6kl_vif *vif;
	int ret;

	ath6kl_dbg(ATH6KL_DBG_WLAN_CFG, "%s: changed 0x%x\n", __func__,
		   changed);

	vif = ath6kl_vif_first(ar);
	if (!vif)
		return -EIO;

	if (!ath6kl_cfg80211_ready(vif))
		return -EIO;

	if (changed & WIPHY_PARAM_RTS_THRESHOLD) {
		ret = ath6kl_wmi_set_rts_cmd(ar->wmi, wiphy->rts_threshold);
		if (ret != 0) {
			ath6kl_err("ath6kl_wmi_set_rts_cmd failed\n");
			return -EIO;
		}
	}

	return 0;
}

static int ath6kl_cfg80211_set_txpower(struct wiphy *wiphy,
				       struct wireless_dev *wdev,
				       enum nl80211_tx_power_setting type,
				       int mbm)
{
	struct ath6kl *ar = (struct ath6kl *)wiphy_priv(wiphy);
	struct ath6kl_vif *vif;
	int dbm = MBM_TO_DBM(mbm);

	ath6kl_dbg(ATH6KL_DBG_WLAN_CFG, "%s: type 0x%x, dbm %d\n", __func__,
		   type, dbm);

	vif = ath6kl_vif_first(ar);
	if (!vif)
		return -EIO;

	if (!ath6kl_cfg80211_ready(vif))
		return -EIO;

	switch (type) {
	case NL80211_TX_POWER_AUTOMATIC:
		return 0;
	case NL80211_TX_POWER_LIMITED:
		ar->tx_pwr = dbm;
		break;
	default:
		ath6kl_dbg(ATH6KL_DBG_WLAN_CFG, "%s: type 0x%x not supported\n",
			   __func__, type);
		return -EOPNOTSUPP;
	}

	ath6kl_wmi_set_tx_pwr_cmd(ar->wmi, vif->fw_vif_idx, dbm);

	return 0;
}

static int ath6kl_cfg80211_get_txpower(struct wiphy *wiphy,
				       struct wireless_dev *wdev,
				       int *dbm)
{
	struct ath6kl *ar = (struct ath6kl *)wiphy_priv(wiphy);
	struct ath6kl_vif *vif;

	vif = ath6kl_vif_first(ar);
	if (!vif)
		return -EIO;

	if (!ath6kl_cfg80211_ready(vif))
		return -EIO;

	if (test_bit(CONNECTED, &vif->flags)) {
		ar->tx_pwr = 255;

		if (ath6kl_wmi_get_tx_pwr_cmd(ar->wmi, vif->fw_vif_idx) != 0) {
			ath6kl_err("ath6kl_wmi_get_tx_pwr_cmd failed\n");
			return -EIO;
		}

		wait_event_interruptible_timeout(ar->event_wq, ar->tx_pwr != 255,
						 5 * HZ);

		if (signal_pending(current)) {
			ath6kl_err("target did not respond\n");
			return -EINTR;
		}
	}

	*dbm = ar->tx_pwr;
	return 0;
}

static int ath6kl_cfg80211_set_power_mgmt(struct wiphy *wiphy,
					  struct net_device *dev,
					  bool pmgmt, int timeout)
{
	struct ath6kl *ar = ath6kl_priv(dev);
	struct wmi_power_mode_cmd mode;
	struct ath6kl_vif *vif = netdev_priv(dev);

	ath6kl_dbg(ATH6KL_DBG_WLAN_CFG, "%s: pmgmt %d, timeout %d\n",
		   __func__, pmgmt, timeout);

	if (!ath6kl_cfg80211_ready(vif))
		return -EIO;

	if (pmgmt) {
		ath6kl_dbg(ATH6KL_DBG_WLAN_CFG, "%s: rec power\n", __func__);
		mode.pwr_mode = REC_POWER;
	} else {
		ath6kl_dbg(ATH6KL_DBG_WLAN_CFG, "%s: max perf\n", __func__);
		mode.pwr_mode = MAX_PERF_POWER;
	}

	if (ath6kl_wmi_powermode_cmd(ar->wmi, vif->fw_vif_idx,
				     mode.pwr_mode) != 0) {
		ath6kl_err("wmi_powermode_cmd failed\n");
		return -EIO;
	}

	return 0;
}

static struct wireless_dev *ath6kl_cfg80211_add_iface(struct wiphy *wiphy,
						      const char *name,
						      unsigned char name_assign_type,
						      enum nl80211_iftype type,
						      struct vif_params *params)
{
	struct ath6kl *ar = wiphy_priv(wiphy);
	struct wireless_dev *wdev;
	u8 if_idx, nw_type;

	if (ar->num_vif == ar->vif_max) {
		ath6kl_err("Reached maximum number of supported vif\n");
		return ERR_PTR(-EINVAL);
	}

	if (!ath6kl_is_valid_iftype(ar, type, &if_idx, &nw_type)) {
		ath6kl_err("Not a supported interface type\n");
		return ERR_PTR(-EINVAL);
	}

	wdev = ath6kl_interface_add(ar, name, name_assign_type, type, if_idx, nw_type);
	if (!wdev)
		return ERR_PTR(-ENOMEM);

	ar->num_vif++;

	return wdev;
}

static int ath6kl_cfg80211_del_iface(struct wiphy *wiphy,
				     struct wireless_dev *wdev)
{
	struct ath6kl *ar = wiphy_priv(wiphy);
	struct ath6kl_vif *vif = netdev_priv(wdev->netdev);

	spin_lock_bh(&ar->list_lock);
	list_del(&vif->list);
	spin_unlock_bh(&ar->list_lock);

	ath6kl_cfg80211_vif_stop(vif, test_bit(WMI_READY, &ar->flag));

	rtnl_lock();
	ath6kl_cfg80211_vif_cleanup(vif);
	rtnl_unlock();

	return 0;
}

static int ath6kl_cfg80211_change_iface(struct wiphy *wiphy,
					struct net_device *ndev,
					enum nl80211_iftype type,
					struct vif_params *params)
{
	struct ath6kl_vif *vif = netdev_priv(ndev);
	int i;

	ath6kl_dbg(ATH6KL_DBG_WLAN_CFG, "%s: type %u\n", __func__, type);

	/*
	 * Don't bring up p2p on an interface which is not initialized
	 * for p2p operation where fw does not have capability to switch
	 * dynamically between non-p2p and p2p type interface.
	 */
	if (!test_bit(ATH6KL_FW_CAPABILITY_STA_P2PDEV_DUPLEX,
		      vif->ar->fw_capabilities) &&
	    (type == NL80211_IFTYPE_P2P_CLIENT ||
	     type == NL80211_IFTYPE_P2P_GO)) {
		if (vif->ar->vif_max == 1) {
			if (vif->fw_vif_idx != 0)
				return -EINVAL;
			else
				goto set_iface_type;
		}

		for (i = vif->ar->max_norm_iface; i < vif->ar->vif_max; i++) {
			if (i == vif->fw_vif_idx)
				break;
		}

		if (i == vif->ar->vif_max) {
			ath6kl_err("Invalid interface to bring up P2P\n");
			return -EINVAL;
		}
	}

	/* need to clean up enhanced bmiss detection fw state */
	ath6kl_cfg80211_sta_bmiss_enhance(vif, false);

set_iface_type:
	switch (type) {
	case NL80211_IFTYPE_STATION:
	case NL80211_IFTYPE_P2P_CLIENT:
		vif->next_mode = INFRA_NETWORK;
		break;
	case NL80211_IFTYPE_ADHOC:
		vif->next_mode = ADHOC_NETWORK;
		break;
	case NL80211_IFTYPE_AP:
	case NL80211_IFTYPE_P2P_GO:
		vif->next_mode = AP_NETWORK;
		break;
	default:
		ath6kl_err("invalid interface type %u\n", type);
		return -EOPNOTSUPP;
	}

	vif->wdev.iftype = type;

	return 0;
}

static int ath6kl_cfg80211_join_ibss(struct wiphy *wiphy,
				     struct net_device *dev,
				     struct cfg80211_ibss_params *ibss_param)
{
	struct ath6kl *ar = ath6kl_priv(dev);
	struct ath6kl_vif *vif = netdev_priv(dev);
	int status;

	if (!ath6kl_cfg80211_ready(vif))
		return -EIO;

	vif->ssid_len = ibss_param->ssid_len;
	memcpy(vif->ssid, ibss_param->ssid, vif->ssid_len);

	if (ibss_param->chandef.chan)
		vif->ch_hint = ibss_param->chandef.chan->center_freq;

	if (ibss_param->channel_fixed) {
		/*
		 * TODO: channel_fixed: The channel should be fixed, do not
		 * search for IBSSs to join on other channels. Target
		 * firmware does not support this feature, needs to be
		 * updated.
		 */
		return -EOPNOTSUPP;
	}

	memset(vif->req_bssid, 0, sizeof(vif->req_bssid));
	if (ibss_param->bssid && !is_broadcast_ether_addr(ibss_param->bssid))
		memcpy(vif->req_bssid, ibss_param->bssid,
		       sizeof(vif->req_bssid));

	ath6kl_set_wpa_version(vif, 0);

	status = ath6kl_set_auth_type(vif, NL80211_AUTHTYPE_OPEN_SYSTEM);
	if (status)
		return status;

	if (ibss_param->privacy) {
		ath6kl_set_cipher(vif, WLAN_CIPHER_SUITE_WEP40, true);
		ath6kl_set_cipher(vif, WLAN_CIPHER_SUITE_WEP40, false);
	} else {
		ath6kl_set_cipher(vif, 0, true);
		ath6kl_set_cipher(vif, 0, false);
	}

	vif->nw_type = vif->next_mode;

	ath6kl_dbg(ATH6KL_DBG_WLAN_CFG,
		   "%s: connect called with authmode %d dot11 auth %d"
		   " PW crypto %d PW crypto len %d GRP crypto %d"
		   " GRP crypto len %d channel hint %u\n",
		   __func__,
		   vif->auth_mode, vif->dot11_auth_mode, vif->prwise_crypto,
		   vif->prwise_crypto_len, vif->grp_crypto,
		   vif->grp_crypto_len, vif->ch_hint);

	status = ath6kl_wmi_connect_cmd(ar->wmi, vif->fw_vif_idx, vif->nw_type,
					vif->dot11_auth_mode, vif->auth_mode,
					vif->prwise_crypto,
					vif->prwise_crypto_len,
					vif->grp_crypto, vif->grp_crypto_len,
					vif->ssid_len, vif->ssid,
					vif->req_bssid, vif->ch_hint,
					ar->connect_ctrl_flags, SUBTYPE_NONE);
	set_bit(CONNECT_PEND, &vif->flags);

	return 0;
}

static int ath6kl_cfg80211_leave_ibss(struct wiphy *wiphy,
				      struct net_device *dev)
{
	struct ath6kl_vif *vif = netdev_priv(dev);

	if (!ath6kl_cfg80211_ready(vif))
		return -EIO;

	ath6kl_disconnect(vif);
	memset(vif->ssid, 0, sizeof(vif->ssid));
	vif->ssid_len = 0;

	return 0;
}

static const u32 cipher_suites[] = {
	WLAN_CIPHER_SUITE_WEP40,
	WLAN_CIPHER_SUITE_WEP104,
	WLAN_CIPHER_SUITE_TKIP,
	WLAN_CIPHER_SUITE_CCMP,
	CCKM_KRK_CIPHER_SUITE,
	WLAN_CIPHER_SUITE_SMS4,
};

static bool is_rate_legacy(s32 rate)
{
	static const s32 legacy[] = { 1000, 2000, 5500, 11000,
		6000, 9000, 12000, 18000, 24000,
		36000, 48000, 54000
	};
	u8 i;

	for (i = 0; i < ARRAY_SIZE(legacy); i++)
		if (rate == legacy[i])
			return true;

	return false;
}

static bool is_rate_ht20(s32 rate, u8 *mcs, bool *sgi)
{
	static const s32 ht20[] = { 6500, 13000, 19500, 26000, 39000,
		52000, 58500, 65000, 72200
	};
	u8 i;

	for (i = 0; i < ARRAY_SIZE(ht20); i++) {
		if (rate == ht20[i]) {
			if (i == ARRAY_SIZE(ht20) - 1)
				/* last rate uses sgi */
				*sgi = true;
			else
				*sgi = false;

			*mcs = i;
			return true;
		}
	}
	return false;
}

static bool is_rate_ht40(s32 rate, u8 *mcs, bool *sgi)
{
	static const s32 ht40[] = { 13500, 27000, 40500, 54000,
		81000, 108000, 121500, 135000,
		150000
	};
	u8 i;

	for (i = 0; i < ARRAY_SIZE(ht40); i++) {
		if (rate == ht40[i]) {
			if (i == ARRAY_SIZE(ht40) - 1)
				/* last rate uses sgi */
				*sgi = true;
			else
				*sgi = false;

			*mcs = i;
			return true;
		}
	}

	return false;
}

static int ath6kl_get_station(struct wiphy *wiphy, struct net_device *dev,
			      const u8 *mac, struct station_info *sinfo)
{
	struct ath6kl *ar = ath6kl_priv(dev);
	struct ath6kl_vif *vif = netdev_priv(dev);
	long left;
	bool sgi;
	s32 rate;
	int ret;
	u8 mcs;

	if (memcmp(mac, vif->bssid, ETH_ALEN) != 0)
		return -ENOENT;

	if (down_interruptible(&ar->sem))
		return -EBUSY;

	set_bit(STATS_UPDATE_PEND, &vif->flags);

	ret = ath6kl_wmi_get_stats_cmd(ar->wmi, vif->fw_vif_idx);

	if (ret != 0) {
		up(&ar->sem);
		return -EIO;
	}

	left = wait_event_interruptible_timeout(ar->event_wq,
						!test_bit(STATS_UPDATE_PEND,
							  &vif->flags),
						WMI_TIMEOUT);

	up(&ar->sem);

	if (left == 0)
		return -ETIMEDOUT;
	else if (left < 0)
		return left;

	if (vif->target_stats.rx_byte) {
		sinfo->rx_bytes = vif->target_stats.rx_byte;
		sinfo->filled |= BIT_ULL(NL80211_STA_INFO_RX_BYTES64);
		sinfo->rx_packets = vif->target_stats.rx_pkt;
		sinfo->filled |= BIT_ULL(NL80211_STA_INFO_RX_PACKETS);
	}

	if (vif->target_stats.tx_byte) {
		sinfo->tx_bytes = vif->target_stats.tx_byte;
		sinfo->filled |= BIT_ULL(NL80211_STA_INFO_TX_BYTES64);
		sinfo->tx_packets = vif->target_stats.tx_pkt;
		sinfo->filled |= BIT_ULL(NL80211_STA_INFO_TX_PACKETS);
	}

	sinfo->signal = vif->target_stats.cs_rssi;
	sinfo->filled |= BIT_ULL(NL80211_STA_INFO_SIGNAL);

	rate = vif->target_stats.tx_ucast_rate;

	if (is_rate_legacy(rate)) {
		sinfo->txrate.legacy = rate / 100;
	} else if (is_rate_ht20(rate, &mcs, &sgi)) {
		if (sgi) {
			sinfo->txrate.flags |= RATE_INFO_FLAGS_SHORT_GI;
			sinfo->txrate.mcs = mcs - 1;
		} else {
			sinfo->txrate.mcs = mcs;
		}

		sinfo->txrate.flags |= RATE_INFO_FLAGS_MCS;
		sinfo->txrate.bw = RATE_INFO_BW_20;
	} else if (is_rate_ht40(rate, &mcs, &sgi)) {
		if (sgi) {
			sinfo->txrate.flags |= RATE_INFO_FLAGS_SHORT_GI;
			sinfo->txrate.mcs = mcs - 1;
		} else {
			sinfo->txrate.mcs = mcs;
		}

		sinfo->txrate.bw = RATE_INFO_BW_40;
		sinfo->txrate.flags |= RATE_INFO_FLAGS_MCS;
	} else {
		ath6kl_dbg(ATH6KL_DBG_WLAN_CFG,
			   "invalid rate from stats: %d\n", rate);
		ath6kl_debug_war(ar, ATH6KL_WAR_INVALID_RATE);
		return 0;
	}

	sinfo->filled |= BIT_ULL(NL80211_STA_INFO_TX_BITRATE);

	if (test_bit(CONNECTED, &vif->flags) &&
	    test_bit(DTIM_PERIOD_AVAIL, &vif->flags) &&
	    vif->nw_type == INFRA_NETWORK) {
		sinfo->filled |= BIT_ULL(NL80211_STA_INFO_BSS_PARAM);
		sinfo->bss_param.flags = 0;
		sinfo->bss_param.dtim_period = vif->assoc_bss_dtim_period;
		sinfo->bss_param.beacon_interval = vif->assoc_bss_beacon_int;
	}

	return 0;
}

static int ath6kl_set_pmksa(struct wiphy *wiphy, struct net_device *netdev,
			    struct cfg80211_pmksa *pmksa)
{
	struct ath6kl *ar = ath6kl_priv(netdev);
	struct ath6kl_vif *vif = netdev_priv(netdev);

	return ath6kl_wmi_setpmkid_cmd(ar->wmi, vif->fw_vif_idx, pmksa->bssid,
				       pmksa->pmkid, true);
}

static int ath6kl_del_pmksa(struct wiphy *wiphy, struct net_device *netdev,
			    struct cfg80211_pmksa *pmksa)
{
	struct ath6kl *ar = ath6kl_priv(netdev);
	struct ath6kl_vif *vif = netdev_priv(netdev);

	return ath6kl_wmi_setpmkid_cmd(ar->wmi, vif->fw_vif_idx, pmksa->bssid,
				       pmksa->pmkid, false);
}

static int ath6kl_flush_pmksa(struct wiphy *wiphy, struct net_device *netdev)
{
	struct ath6kl *ar = ath6kl_priv(netdev);
	struct ath6kl_vif *vif = netdev_priv(netdev);

	if (test_bit(CONNECTED, &vif->flags))
		return ath6kl_wmi_setpmkid_cmd(ar->wmi, vif->fw_vif_idx,
					       vif->bssid, NULL, false);
	return 0;
}

static int ath6kl_wow_usr(struct ath6kl *ar, struct ath6kl_vif *vif,
			  struct cfg80211_wowlan *wow, u32 *filter)
{
	int ret, pos;
	u8 mask[WOW_PATTERN_SIZE];
	u16 i;

	/* Configure the patterns that we received from the user. */
	for (i = 0; i < wow->n_patterns; i++) {
		/*
		 * Convert given nl80211 specific mask value to equivalent
		 * driver specific mask value and send it to the chip along
		 * with patterns. For example, If the mask value defined in
		 * struct cfg80211_wowlan is 0xA (equivalent binary is 1010),
		 * then equivalent driver specific mask value is
		 * "0xFF 0x00 0xFF 0x00".
		 */
		memset(&mask, 0, sizeof(mask));
		for (pos = 0; pos < wow->patterns[i].pattern_len; pos++) {
			if (wow->patterns[i].mask[pos / 8] & (0x1 << (pos % 8)))
				mask[pos] = 0xFF;
		}
		/*
		 * Note: Pattern's offset is not passed as part of wowlan
		 * parameter from CFG layer. So it's always passed as ZERO
		 * to the firmware. It means, given WOW patterns are always
		 * matched from the first byte of received pkt in the firmware.
		 */
		ret = ath6kl_wmi_add_wow_pattern_cmd(ar->wmi,
				vif->fw_vif_idx, WOW_LIST_ID,
				wow->patterns[i].pattern_len,
				0 /* pattern offset */,
				wow->patterns[i].pattern, mask);
		if (ret)
			return ret;
	}

	if (wow->disconnect)
		*filter |= WOW_FILTER_OPTION_NWK_DISASSOC;

	if (wow->magic_pkt)
		*filter |= WOW_FILTER_OPTION_MAGIC_PACKET;

	if (wow->gtk_rekey_failure)
		*filter |= WOW_FILTER_OPTION_GTK_ERROR;

	if (wow->eap_identity_req)
		*filter |= WOW_FILTER_OPTION_EAP_REQ;

	if (wow->four_way_handshake)
		*filter |= WOW_FILTER_OPTION_8021X_4WAYHS;

	return 0;
}

static int ath6kl_wow_ap(struct ath6kl *ar, struct ath6kl_vif *vif)
{
	static const u8 unicst_pattern[] = { 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x08 };
	static const u8 unicst_mask[] = { 0x01, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x7f };
	u8 unicst_offset = 0;
	static const u8 arp_pattern[] = { 0x08, 0x06 };
	static const u8 arp_mask[] = { 0xff, 0xff };
	u8 arp_offset = 20;
	static const u8 discvr_pattern[] = { 0xe0, 0x00, 0x00, 0xf8 };
	static const u8 discvr_mask[] = { 0xf0, 0x00, 0x00, 0xf8 };
	u8 discvr_offset = 38;
	static const u8 dhcp_pattern[] = { 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x43 /* port 67 */ };
	static const u8 dhcp_mask[] = { 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0xff, 0xff /* port 67 */ };
	u8 dhcp_offset = 0;
	int ret;

	/* Setup unicast IP, EAPOL-like and ARP pkt pattern */
	ret = ath6kl_wmi_add_wow_pattern_cmd(ar->wmi,
			vif->fw_vif_idx, WOW_LIST_ID,
			sizeof(unicst_pattern), unicst_offset,
			unicst_pattern, unicst_mask);
	if (ret) {
		ath6kl_err("failed to add WOW unicast IP pattern\n");
		return ret;
	}

	/* Setup all ARP pkt pattern */
	ret = ath6kl_wmi_add_wow_pattern_cmd(ar->wmi,
			vif->fw_vif_idx, WOW_LIST_ID,
			sizeof(arp_pattern), arp_offset,
			arp_pattern, arp_mask);
	if (ret) {
		ath6kl_err("failed to add WOW ARP pattern\n");
		return ret;
	}

	/*
	 * Setup multicast pattern for mDNS 224.0.0.251,
	 * SSDP 239.255.255.250 and LLMNR  224.0.0.252
	 */
	ret = ath6kl_wmi_add_wow_pattern_cmd(ar->wmi,
			vif->fw_vif_idx, WOW_LIST_ID,
			sizeof(discvr_pattern), discvr_offset,
			discvr_pattern, discvr_mask);
	if (ret) {
		ath6kl_err("failed to add WOW mDNS/SSDP/LLMNR pattern\n");
		return ret;
	}

	/* Setup all DHCP broadcast pkt pattern */
	ret = ath6kl_wmi_add_wow_pattern_cmd(ar->wmi,
			vif->fw_vif_idx, WOW_LIST_ID,
			sizeof(dhcp_pattern), dhcp_offset,
			dhcp_pattern, dhcp_mask);
	if (ret) {
		ath6kl_err("failed to add WOW DHCP broadcast pattern\n");
		return ret;
	}

	return 0;
}

static int ath6kl_wow_sta(struct ath6kl *ar, struct ath6kl_vif *vif)
{
	struct net_device *ndev = vif->ndev;
	static const u8 discvr_pattern[] = { 0xe0, 0x00, 0x00, 0xf8 };
	static const u8 discvr_mask[] = { 0xf0, 0x00, 0x00, 0xf8 };
	u8 discvr_offset = 38;
	u8 mac_mask[ETH_ALEN];
	int ret;

	/* Setup unicast pkt pattern */
	eth_broadcast_addr(mac_mask);
	ret = ath6kl_wmi_add_wow_pattern_cmd(ar->wmi,
				vif->fw_vif_idx, WOW_LIST_ID,
				ETH_ALEN, 0, ndev->dev_addr,
				mac_mask);
	if (ret) {
		ath6kl_err("failed to add WOW unicast pattern\n");
		return ret;
	}

	/*
	 * Setup multicast pattern for mDNS 224.0.0.251,
	 * SSDP 239.255.255.250 and LLMNR 224.0.0.252
	 */
	if ((ndev->flags & IFF_ALLMULTI) ||
	    (ndev->flags & IFF_MULTICAST && netdev_mc_count(ndev) > 0)) {
		ret = ath6kl_wmi_add_wow_pattern_cmd(ar->wmi,
				vif->fw_vif_idx, WOW_LIST_ID,
				sizeof(discvr_pattern), discvr_offset,
				discvr_pattern, discvr_mask);
		if (ret) {
			ath6kl_err("failed to add WOW mDNS/SSDP/LLMNR pattern\n");
			return ret;
		}
	}

	return 0;
}

static int is_hsleep_mode_procsed(struct ath6kl_vif *vif)
{
	return test_bit(HOST_SLEEP_MODE_CMD_PROCESSED, &vif->flags);
}

static bool is_ctrl_ep_empty(struct ath6kl *ar)
{
	return !ar->tx_pending[ar->ctrl_ep];
}

static int ath6kl_cfg80211_host_sleep(struct ath6kl *ar, struct ath6kl_vif *vif)
{
	int ret, left;

	clear_bit(HOST_SLEEP_MODE_CMD_PROCESSED, &vif->flags);

	ret = ath6kl_wmi_set_host_sleep_mode_cmd(ar->wmi, vif->fw_vif_idx,
						 ATH6KL_HOST_MODE_ASLEEP);
	if (ret)
		return ret;

	left = wait_event_interruptible_timeout(ar->event_wq,
						is_hsleep_mode_procsed(vif),
						WMI_TIMEOUT);
	if (left == 0) {
		ath6kl_warn("timeout, didn't get host sleep cmd processed event\n");
		ret = -ETIMEDOUT;
	} else if (left < 0) {
		ath6kl_warn("error while waiting for host sleep cmd processed event %d\n",
			    left);
		ret = left;
	}

	if (ar->tx_pending[ar->ctrl_ep]) {
		left = wait_event_interruptible_timeout(ar->event_wq,
							is_ctrl_ep_empty(ar),
							WMI_TIMEOUT);
		if (left == 0) {
			ath6kl_warn("clear wmi ctrl data timeout\n");
			ret = -ETIMEDOUT;
		} else if (left < 0) {
			ath6kl_warn("clear wmi ctrl data failed: %d\n", left);
			ret = left;
		}
	}

	return ret;
}

static int ath6kl_wow_suspend_vif(struct ath6kl_vif *vif,
				  struct cfg80211_wowlan *wow, u32 *filter)
{
	struct ath6kl *ar = vif->ar;
	struct in_device *in_dev;
	struct in_ifaddr *ifa;
	int ret;
	u16 i, bmiss_time;
	__be32 ips[MAX_IP_ADDRS];
	u8 index = 0;

	if (!test_bit(NETDEV_MCAST_ALL_ON, &vif->flags) &&
	    test_bit(ATH6KL_FW_CAPABILITY_WOW_MULTICAST_FILTER,
		     ar->fw_capabilities)) {
		ret = ath6kl_wmi_mcast_filter_cmd(vif->ar->wmi,
						vif->fw_vif_idx, false);
		if (ret)
			return ret;
	}

	/* Clear existing WOW patterns */
	for (i = 0; i < WOW_MAX_FILTERS_PER_LIST; i++)
		ath6kl_wmi_del_wow_pattern_cmd(ar->wmi, vif->fw_vif_idx,
					       WOW_LIST_ID, i);

	/*
	 * Skip the default WOW pattern configuration
	 * if the driver receives any WOW patterns from
	 * the user.
	 */
	if (wow)
		ret = ath6kl_wow_usr(ar, vif, wow, filter);
	else if (vif->nw_type == AP_NETWORK)
		ret = ath6kl_wow_ap(ar, vif);
	else
		ret = ath6kl_wow_sta(ar, vif);

	if (ret)
		return ret;

	netif_stop_queue(vif->ndev);

	if (vif->nw_type != AP_NETWORK) {
		ret = ath6kl_wmi_listeninterval_cmd(ar->wmi, vif->fw_vif_idx,
						    ATH6KL_MAX_WOW_LISTEN_INTL,
						    0);
		if (ret)
			return ret;

		/* Set listen interval x 15 times as bmiss time */
		bmiss_time = ATH6KL_MAX_WOW_LISTEN_INTL * 15;
		if (bmiss_time > ATH6KL_MAX_BMISS_TIME)
			bmiss_time = ATH6KL_MAX_BMISS_TIME;

		ret = ath6kl_wmi_bmisstime_cmd(ar->wmi, vif->fw_vif_idx,
					       bmiss_time, 0);
		if (ret)
			return ret;

		ret = ath6kl_wmi_scanparams_cmd(ar->wmi, vif->fw_vif_idx,
						0xFFFF, 0, 0xFFFF, 0, 0, 0,
						0, 0, 0, 0);
		if (ret)
			return ret;
	}

	/* Setup own IP addr for ARP agent. */
	in_dev = __in_dev_get_rtnl(vif->ndev);
	if (!in_dev)
		return 0;

	ifa = rtnl_dereference(in_dev->ifa_list);
	memset(&ips, 0, sizeof(ips));

	/* Configure IP addr only if IP address count < MAX_IP_ADDRS */
	while (index < MAX_IP_ADDRS && ifa) {
		ips[index] = ifa->ifa_local;
		ifa = rtnl_dereference(ifa->ifa_next);
		index++;
	}

	if (ifa) {
		ath6kl_err("total IP addr count is exceeding fw limit\n");
		return -EINVAL;
	}

	ret = ath6kl_wmi_set_ip_cmd(ar->wmi, vif->fw_vif_idx, ips[0], ips[1]);
	if (ret) {
		ath6kl_err("fail to setup ip for arp agent\n");
		return ret;
	}

	return ret;
}

static int ath6kl_wow_suspend(struct ath6kl *ar, struct cfg80211_wowlan *wow)
{
	struct ath6kl_vif *first_vif, *vif;
	int ret = 0;
	u32 filter = 0;
	bool connected = false;

	/* enter / leave wow suspend on first vif always */
	first_vif = ath6kl_vif_first(ar);
	if (WARN_ON(!first_vif) ||
	    !ath6kl_cfg80211_ready(first_vif))
		return -EIO;

	if (wow && (wow->n_patterns > WOW_MAX_FILTERS_PER_LIST))
		return -EINVAL;

	/* install filters for each connected vif */
	spin_lock_bh(&ar->list_lock);
	list_for_each_entry(vif, &ar->vif_list, list) {
		if (!test_bit(CONNECTED, &vif->flags) ||
		    !ath6kl_cfg80211_ready(vif))
			continue;
		connected = true;

		ret = ath6kl_wow_suspend_vif(vif, wow, &filter);
		if (ret)
			break;
	}
	spin_unlock_bh(&ar->list_lock);

	if (!connected)
		return -ENOTCONN;
	else if (ret)
		return ret;

	ar->state = ATH6KL_STATE_SUSPENDING;

	ret = ath6kl_wmi_set_wow_mode_cmd(ar->wmi, first_vif->fw_vif_idx,
					  ATH6KL_WOW_MODE_ENABLE,
					  filter,
					  WOW_HOST_REQ_DELAY);
	if (ret)
		return ret;

	return ath6kl_cfg80211_host_sleep(ar, first_vif);
}

static int ath6kl_wow_resume_vif(struct ath6kl_vif *vif)
{
	struct ath6kl *ar = vif->ar;
	int ret;

	if (vif->nw_type != AP_NETWORK) {
		ret = ath6kl_wmi_scanparams_cmd(ar->wmi, vif->fw_vif_idx,
						0, 0, 0, 0, 0, 0, 3, 0, 0, 0);
		if (ret)
			return ret;

		ret = ath6kl_wmi_listeninterval_cmd(ar->wmi, vif->fw_vif_idx,
						    vif->listen_intvl_t, 0);
		if (ret)
			return ret;

		ret = ath6kl_wmi_bmisstime_cmd(ar->wmi, vif->fw_vif_idx,
					       vif->bmiss_time_t, 0);
		if (ret)
			return ret;
	}

	if (!test_bit(NETDEV_MCAST_ALL_OFF, &vif->flags) &&
	    test_bit(ATH6KL_FW_CAPABILITY_WOW_MULTICAST_FILTER,
		     ar->fw_capabilities)) {
		ret = ath6kl_wmi_mcast_filter_cmd(vif->ar->wmi,
						  vif->fw_vif_idx, true);
		if (ret)
			return ret;
	}

	netif_wake_queue(vif->ndev);

	return 0;
}

static int ath6kl_wow_resume(struct ath6kl *ar)
{
	struct ath6kl_vif *vif;
	int ret;

	vif = ath6kl_vif_first(ar);
	if (WARN_ON(!vif) ||
	    !ath6kl_cfg80211_ready(vif))
		return -EIO;

	ar->state = ATH6KL_STATE_RESUMING;

	ret = ath6kl_wmi_set_host_sleep_mode_cmd(ar->wmi, vif->fw_vif_idx,
						 ATH6KL_HOST_MODE_AWAKE);
	if (ret) {
		ath6kl_warn("Failed to configure host sleep mode for wow resume: %d\n",
			    ret);
		goto cleanup;
	}

	spin_lock_bh(&ar->list_lock);
	list_for_each_entry(vif, &ar->vif_list, list) {
		if (!test_bit(CONNECTED, &vif->flags) ||
		    !ath6kl_cfg80211_ready(vif))
			continue;
		ret = ath6kl_wow_resume_vif(vif);
		if (ret)
			break;
	}
	spin_unlock_bh(&ar->list_lock);

	if (ret)
		goto cleanup;

	ar->state = ATH6KL_STATE_ON;
	return 0;

cleanup:
	ar->state = ATH6KL_STATE_WOW;
	return ret;
}

static int ath6kl_cfg80211_deepsleep_suspend(struct ath6kl *ar)
{
	struct ath6kl_vif *vif;
	int ret;

	vif = ath6kl_vif_first(ar);
	if (!vif)
		return -EIO;

	if (!test_bit(WMI_READY, &ar->flag)) {
		ath6kl_err("deepsleep failed as wmi is not ready\n");
		return -EIO;
	}

	ath6kl_cfg80211_stop_all(ar);

	/* Save the current power mode before enabling power save */
	ar->wmi->saved_pwr_mode = ar->wmi->pwr_mode;

	ret = ath6kl_wmi_powermode_cmd(ar->wmi, 0, REC_POWER);
	if (ret)
		return ret;

	/* Disable WOW mode */
	ret = ath6kl_wmi_set_wow_mode_cmd(ar->wmi, vif->fw_vif_idx,
					  ATH6KL_WOW_MODE_DISABLE,
					  0, 0);
	if (ret)
		return ret;

	/* Flush all non control pkts in TX path */
	ath6kl_tx_data_cleanup(ar);

	ret = ath6kl_cfg80211_host_sleep(ar, vif);
	if (ret)
		return ret;

	return 0;
}

static int ath6kl_cfg80211_deepsleep_resume(struct ath6kl *ar)
{
	struct ath6kl_vif *vif;
	int ret;

	vif = ath6kl_vif_first(ar);

	if (!vif)
		return -EIO;

	if (ar->wmi->pwr_mode != ar->wmi->saved_pwr_mode) {
		ret = ath6kl_wmi_powermode_cmd(ar->wmi, 0,
					       ar->wmi->saved_pwr_mode);
		if (ret)
			return ret;
	}

	ret = ath6kl_wmi_set_host_sleep_mode_cmd(ar->wmi, vif->fw_vif_idx,
						 ATH6KL_HOST_MODE_AWAKE);
	if (ret)
		return ret;

	ar->state = ATH6KL_STATE_ON;

	/* Reset scan parameter to default values */
	ret = ath6kl_wmi_scanparams_cmd(ar->wmi, vif->fw_vif_idx,
					0, 0, 0, 0, 0, 0, 3, 0, 0, 0);
	if (ret)
		return ret;

	return 0;
}

int ath6kl_cfg80211_suspend(struct ath6kl *ar,
			    enum ath6kl_cfg_suspend_mode mode,
			    struct cfg80211_wowlan *wow)
{
	struct ath6kl_vif *vif;
	enum ath6kl_state prev_state;
	int ret;

	switch (mode) {
	case ATH6KL_CFG_SUSPEND_WOW:

		ath6kl_dbg(ATH6KL_DBG_SUSPEND, "wow mode suspend\n");

		/* Flush all non control pkts in TX path */
		ath6kl_tx_data_cleanup(ar);

		prev_state = ar->state;

		ret = ath6kl_wow_suspend(ar, wow);
		if (ret) {
			ar->state = prev_state;
			return ret;
		}

		ar->state = ATH6KL_STATE_WOW;
		break;

	case ATH6KL_CFG_SUSPEND_DEEPSLEEP:

		ath6kl_dbg(ATH6KL_DBG_SUSPEND, "deep sleep suspend\n");

		ret = ath6kl_cfg80211_deepsleep_suspend(ar);
		if (ret) {
			ath6kl_err("deepsleep suspend failed: %d\n", ret);
			return ret;
		}

		ar->state = ATH6KL_STATE_DEEPSLEEP;

		break;

	case ATH6KL_CFG_SUSPEND_CUTPOWER:

		ath6kl_cfg80211_stop_all(ar);

		if (ar->state == ATH6KL_STATE_OFF) {
			ath6kl_dbg(ATH6KL_DBG_SUSPEND,
				   "suspend hw off, no action for cutpower\n");
			break;
		}

		ath6kl_dbg(ATH6KL_DBG_SUSPEND, "suspend cutting power\n");

		ret = ath6kl_init_hw_stop(ar);
		if (ret) {
			ath6kl_warn("failed to stop hw during suspend: %d\n",
				    ret);
		}

		ar->state = ATH6KL_STATE_CUTPOWER;

		break;

	default:
		break;
	}

	list_for_each_entry(vif, &ar->vif_list, list)
		ath6kl_cfg80211_scan_complete_event(vif, true);

	return 0;
}
EXPORT_SYMBOL(ath6kl_cfg80211_suspend);

int ath6kl_cfg80211_resume(struct ath6kl *ar)
{
	int ret;

	switch (ar->state) {
	case  ATH6KL_STATE_WOW:
		ath6kl_dbg(ATH6KL_DBG_SUSPEND, "wow mode resume\n");

		ret = ath6kl_wow_resume(ar);
		if (ret) {
			ath6kl_warn("wow mode resume failed: %d\n", ret);
			return ret;
		}

		break;

	case ATH6KL_STATE_DEEPSLEEP:
		ath6kl_dbg(ATH6KL_DBG_SUSPEND, "deep sleep resume\n");

		ret = ath6kl_cfg80211_deepsleep_resume(ar);
		if (ret) {
			ath6kl_warn("deep sleep resume failed: %d\n", ret);
			return ret;
		}
		break;

	case ATH6KL_STATE_CUTPOWER:
		ath6kl_dbg(ATH6KL_DBG_SUSPEND, "resume restoring power\n");

		ret = ath6kl_init_hw_start(ar);
		if (ret) {
			ath6kl_warn("Failed to boot hw in resume: %d\n", ret);
			return ret;
		}
		break;

	default:
		break;
	}

	return 0;
}
EXPORT_SYMBOL(ath6kl_cfg80211_resume);

#ifdef CONFIG_PM

/* hif layer decides what suspend mode to use */
static int __ath6kl_cfg80211_suspend(struct wiphy *wiphy,
				 struct cfg80211_wowlan *wow)
{
	struct ath6kl *ar = wiphy_priv(wiphy);

	ath6kl_recovery_suspend(ar);

	return ath6kl_hif_suspend(ar, wow);
}

static int __ath6kl_cfg80211_resume(struct wiphy *wiphy)
{
	struct ath6kl *ar = wiphy_priv(wiphy);
	int err;

	err = ath6kl_hif_resume(ar);
	if (err)
		return err;

	ath6kl_recovery_resume(ar);

	return 0;
}

/*
 * FIXME: WOW suspend mode is selected if the host sdio controller supports
 * both sdio irq wake up and keep power. The target pulls sdio data line to
 * wake up the host when WOW pattern matches. This causes sdio irq handler
 * is being called in the host side which internally hits ath6kl's RX path.
 *
 * Since sdio interrupt is not disabled, RX path executes even before
 * the host executes the actual resume operation from PM module.
 *
 * In the current scenario, WOW resume should happen before start processing
 * any data from the target. So It's required to perform WOW resume in RX path.
 * Ideally we should perform WOW resume only in the actual platform
 * resume path. This area needs bit rework to avoid WOW resume in RX path.
 *
 * ath6kl_check_wow_status() is called from ath6kl_rx().
 */
void ath6kl_check_wow_status(struct ath6kl *ar)
{
	if (ar->state == ATH6KL_STATE_SUSPENDING)
		return;

	if (ar->state == ATH6KL_STATE_WOW)
		ath6kl_cfg80211_resume(ar);
}

#else

void ath6kl_check_wow_status(struct ath6kl *ar)
{
}
#endif

static int ath6kl_set_htcap(struct ath6kl_vif *vif, enum nl80211_band band,
			    bool ht_enable)
{
	struct ath6kl_htcap *htcap = &vif->htcap[band];

	if (htcap->ht_enable == ht_enable)
		return 0;

	if (ht_enable) {
		/* Set default ht capabilities */
		htcap->ht_enable = true;
		htcap->cap_info = (band == NL80211_BAND_2GHZ) ?
				   ath6kl_g_htcap : ath6kl_a_htcap;
		htcap->ampdu_factor = IEEE80211_HT_MAX_AMPDU_16K;
	} else /* Disable ht */
		memset(htcap, 0, sizeof(*htcap));

	return ath6kl_wmi_set_htcap_cmd(vif->ar->wmi, vif->fw_vif_idx,
					band, htcap);
}

static int ath6kl_restore_htcap(struct ath6kl_vif *vif)
{
	struct wiphy *wiphy = vif->ar->wiphy;
	int band, ret = 0;

	for (band = 0; band < NUM_NL80211_BANDS; band++) {
		if (!wiphy->bands[band])
			continue;

		ret = ath6kl_set_htcap(vif, band,
				wiphy->bands[band]->ht_cap.ht_supported);
		if (ret)
			return ret;
	}

	return ret;
}

static bool ath6kl_is_p2p_ie(const u8 *pos)
{
	return pos[0] == WLAN_EID_VENDOR_SPECIFIC && pos[1] >= 4 &&
		pos[2] == 0x50 && pos[3] == 0x6f &&
		pos[4] == 0x9a && pos[5] == 0x09;
}

static int ath6kl_set_ap_probe_resp_ies(struct ath6kl_vif *vif,
					const u8 *ies, size_t ies_len)
{
	struct ath6kl *ar = vif->ar;
	const u8 *pos;
	u8 *buf = NULL;
	size_t len = 0;
	int ret;

	/*
	 * Filter out P2P IE(s) since they will be included depending on
	 * the Probe Request frame in ath6kl_send_go_probe_resp().
	 */

	if (ies && ies_len) {
		buf = kmalloc(ies_len, GFP_KERNEL);
		if (buf == NULL)
			return -ENOMEM;
		pos = ies;
		while (pos + 1 < ies + ies_len) {
			if (pos + 2 + pos[1] > ies + ies_len)
				break;
			if (!ath6kl_is_p2p_ie(pos)) {
				memcpy(buf + len, pos, 2 + pos[1]);
				len += 2 + pos[1];
			}
			pos += 2 + pos[1];
		}
	}

	ret = ath6kl_wmi_set_appie_cmd(ar->wmi, vif->fw_vif_idx,
				       WMI_FRAME_PROBE_RESP, buf, len);
	kfree(buf);
	return ret;
}

static int ath6kl_set_ies(struct ath6kl_vif *vif,
			  struct cfg80211_beacon_data *info)
{
	struct ath6kl *ar = vif->ar;
	int res;

	/* this also clears IE in fw if it's not set */
	res = ath6kl_wmi_set_appie_cmd(ar->wmi, vif->fw_vif_idx,
				       WMI_FRAME_BEACON,
				       info->beacon_ies,
				       info->beacon_ies_len);
	if (res)
		return res;

	/* this also clears IE in fw if it's not set */
	res = ath6kl_set_ap_probe_resp_ies(vif, info->proberesp_ies,
					   info->proberesp_ies_len);
	if (res)
		return res;

	/* this also clears IE in fw if it's not set */
	res = ath6kl_wmi_set_appie_cmd(ar->wmi, vif->fw_vif_idx,
				       WMI_FRAME_ASSOC_RESP,
				       info->assocresp_ies,
				       info->assocresp_ies_len);
	if (res)
		return res;

	return 0;
}

static int ath6kl_get_rsn_capab(struct cfg80211_beacon_data *beacon,
				u8 *rsn_capab)
{
	const u8 *rsn_ie;
	size_t rsn_ie_len;
	u16 cnt;

	if (!beacon->tail)
		return -EINVAL;

	rsn_ie = cfg80211_find_ie(WLAN_EID_RSN, beacon->tail, beacon->tail_len);
	if (!rsn_ie)
		return -EINVAL;

	rsn_ie_len = *(rsn_ie + 1);
	/* skip element id and length */
	rsn_ie += 2;

	/* skip version */
	if (rsn_ie_len < 2)
		return -EINVAL;
	rsn_ie +=  2;
	rsn_ie_len -= 2;

	/* skip group cipher suite */
	if (rsn_ie_len < 4)
		return 0;
	rsn_ie +=  4;
	rsn_ie_len -= 4;

	/* skip pairwise cipher suite */
	if (rsn_ie_len < 2)
		return 0;
	cnt = get_unaligned_le16(rsn_ie);
	rsn_ie += (2 + cnt * 4);
	rsn_ie_len -= (2 + cnt * 4);

	/* skip akm suite */
	if (rsn_ie_len < 2)
		return 0;
	cnt = get_unaligned_le16(rsn_ie);
	rsn_ie += (2 + cnt * 4);
	rsn_ie_len -= (2 + cnt * 4);

	if (rsn_ie_len < 2)
		return 0;

	memcpy(rsn_capab, rsn_ie, 2);

	return 0;
}

static int ath6kl_start_ap(struct wiphy *wiphy, struct net_device *dev,
			   struct cfg80211_ap_settings *info)
{
	struct ath6kl *ar = ath6kl_priv(dev);
	struct ath6kl_vif *vif = netdev_priv(dev);
	struct ieee80211_mgmt *mgmt;
	bool hidden = false;
	u8 *ies;
	struct wmi_connect_cmd p;
	int res;
	int i, ret;
	u16 rsn_capab = 0;
	int inactivity_timeout = 0;

	ath6kl_dbg(ATH6KL_DBG_WLAN_CFG, "%s:\n", __func__);

	if (!ath6kl_cfg80211_ready(vif))
		return -EIO;

	if (vif->next_mode != AP_NETWORK)
		return -EOPNOTSUPP;

	res = ath6kl_set_ies(vif, &info->beacon);

	ar->ap_mode_bkey.valid = false;

	ret = ath6kl_wmi_ap_set_beacon_intvl_cmd(ar->wmi, vif->fw_vif_idx,
						 info->beacon_interval);

	if (ret)
		ath6kl_warn("Failed to set beacon interval: %d\n", ret);

	ret = ath6kl_wmi_ap_set_dtim_cmd(ar->wmi, vif->fw_vif_idx,
					 info->dtim_period);

	/* ignore error, just print a warning and continue normally */
	if (ret)
		ath6kl_warn("Failed to set dtim_period in beacon: %d\n", ret);

	if (info->beacon.head == NULL)
		return -EINVAL;
	mgmt = (struct ieee80211_mgmt *) info->beacon.head;
	ies = mgmt->u.beacon.variable;
	if (ies > info->beacon.head + info->beacon.head_len)
		return -EINVAL;

	if (info->ssid == NULL)
		return -EINVAL;
	memcpy(vif->ssid, info->ssid, info->ssid_len);
	vif->ssid_len = info->ssid_len;
	if (info->hidden_ssid != NL80211_HIDDEN_SSID_NOT_IN_USE)
		hidden = true;

	res = ath6kl_wmi_ap_hidden_ssid(ar->wmi, vif->fw_vif_idx, hidden);
	if (res)
		return res;

	ret = ath6kl_set_auth_type(vif, info->auth_type);
	if (ret)
		return ret;

	memset(&p, 0, sizeof(p));

	for (i = 0; i < info->crypto.n_akm_suites; i++) {
		switch (info->crypto.akm_suites[i]) {
		case WLAN_AKM_SUITE_8021X:
			if (info->crypto.wpa_versions & NL80211_WPA_VERSION_1)
				p.auth_mode |= WPA_AUTH;
			if (info->crypto.wpa_versions & NL80211_WPA_VERSION_2)
				p.auth_mode |= WPA2_AUTH;
			break;
		case WLAN_AKM_SUITE_PSK:
			if (info->crypto.wpa_versions & NL80211_WPA_VERSION_1)
				p.auth_mode |= WPA_PSK_AUTH;
			if (info->crypto.wpa_versions & NL80211_WPA_VERSION_2)
				p.auth_mode |= WPA2_PSK_AUTH;
			break;
		}
	}
	if (p.auth_mode == 0)
		p.auth_mode = NONE_AUTH;
	vif->auth_mode = p.auth_mode;

	for (i = 0; i < info->crypto.n_ciphers_pairwise; i++) {
		switch (info->crypto.ciphers_pairwise[i]) {
		case WLAN_CIPHER_SUITE_WEP40:
		case WLAN_CIPHER_SUITE_WEP104:
			p.prwise_crypto_type |= WEP_CRYPT;
			break;
		case WLAN_CIPHER_SUITE_TKIP:
			p.prwise_crypto_type |= TKIP_CRYPT;
			break;
		case WLAN_CIPHER_SUITE_CCMP:
			p.prwise_crypto_type |= AES_CRYPT;
			break;
		case WLAN_CIPHER_SUITE_SMS4:
			p.prwise_crypto_type |= WAPI_CRYPT;
			break;
		}
	}
	if (p.prwise_crypto_type == 0) {
		p.prwise_crypto_type = NONE_CRYPT;
		ath6kl_set_cipher(vif, 0, true);
	} else if (info->crypto.n_ciphers_pairwise == 1) {
		ath6kl_set_cipher(vif, info->crypto.ciphers_pairwise[0], true);
	}

	switch (info->crypto.cipher_group) {
	case WLAN_CIPHER_SUITE_WEP40:
	case WLAN_CIPHER_SUITE_WEP104:
		p.grp_crypto_type = WEP_CRYPT;
		break;
	case WLAN_CIPHER_SUITE_TKIP:
		p.grp_crypto_type = TKIP_CRYPT;
		break;
	case WLAN_CIPHER_SUITE_CCMP:
		p.grp_crypto_type = AES_CRYPT;
		break;
	case WLAN_CIPHER_SUITE_SMS4:
		p.grp_crypto_type = WAPI_CRYPT;
		break;
	default:
		p.grp_crypto_type = NONE_CRYPT;
		break;
	}
	ath6kl_set_cipher(vif, info->crypto.cipher_group, false);

	p.nw_type = AP_NETWORK;
	vif->nw_type = vif->next_mode;

	p.ssid_len = vif->ssid_len;
	memcpy(p.ssid, vif->ssid, vif->ssid_len);
	p.dot11_auth_mode = vif->dot11_auth_mode;
	p.ch = cpu_to_le16(info->chandef.chan->center_freq);

	/* Enable uAPSD support by default */
	res = ath6kl_wmi_ap_set_apsd(ar->wmi, vif->fw_vif_idx, true);
	if (res < 0)
		return res;

	if (vif->wdev.iftype == NL80211_IFTYPE_P2P_GO) {
		p.nw_subtype = SUBTYPE_P2PGO;
	} else {
		/*
		 * Due to firmware limitation, it is not possible to
		 * do P2P mgmt operations in AP mode
		 */
		p.nw_subtype = SUBTYPE_NONE;
	}

	if (info->inactivity_timeout) {
		inactivity_timeout = info->inactivity_timeout;

		if (test_bit(ATH6KL_FW_CAPABILITY_AP_INACTIVITY_MINS,
			     ar->fw_capabilities))
			inactivity_timeout = DIV_ROUND_UP(inactivity_timeout,
							  60);

		res = ath6kl_wmi_set_inact_period(ar->wmi, vif->fw_vif_idx,
						  inactivity_timeout);
		if (res < 0)
			return res;
	}

	if (ath6kl_set_htcap(vif, info->chandef.chan->band,
			     cfg80211_get_chandef_type(&info->chandef)
					!= NL80211_CHAN_NO_HT))
		return -EIO;

	/*
	 * Get the PTKSA replay counter in the RSN IE. Supplicant
	 * will use the RSN IE in M3 message and firmware has to
	 * advertise the same in beacon/probe response. Send
	 * the complete RSN IE capability field to firmware
	 */
	if (!ath6kl_get_rsn_capab(&info->beacon, (u8 *) &rsn_capab) &&
	    test_bit(ATH6KL_FW_CAPABILITY_RSN_CAP_OVERRIDE,
		     ar->fw_capabilities)) {
		res = ath6kl_wmi_set_ie_cmd(ar->wmi, vif->fw_vif_idx,
					    WLAN_EID_RSN, WMI_RSN_IE_CAPB,
					    (const u8 *) &rsn_capab,
					    sizeof(rsn_capab));
		vif->rsn_capab = rsn_capab;
		if (res < 0)
			return res;
	}

	memcpy(&vif->profile, &p, sizeof(p));
	res = ath6kl_wmi_ap_profile_commit(ar->wmi, vif->fw_vif_idx, &p);
	if (res < 0)
		return res;

	return 0;
}

static int ath6kl_change_beacon(struct wiphy *wiphy, struct net_device *dev,
				struct cfg80211_beacon_data *beacon)
{
	struct ath6kl_vif *vif = netdev_priv(dev);

	if (!ath6kl_cfg80211_ready(vif))
		return -EIO;

	if (vif->next_mode != AP_NETWORK)
		return -EOPNOTSUPP;

	return ath6kl_set_ies(vif, beacon);
}

static int ath6kl_stop_ap(struct wiphy *wiphy, struct net_device *dev)
{
	struct ath6kl *ar = ath6kl_priv(dev);
	struct ath6kl_vif *vif = netdev_priv(dev);

	if (vif->nw_type != AP_NETWORK)
		return -EOPNOTSUPP;
	if (!test_bit(CONNECTED, &vif->flags))
		return -ENOTCONN;

	ath6kl_wmi_disconnect_cmd(ar->wmi, vif->fw_vif_idx);
	clear_bit(CONNECTED, &vif->flags);
	netif_carrier_off(vif->ndev);

	/* Restore ht setting in firmware */
	return ath6kl_restore_htcap(vif);
}

static const u8 bcast_addr[ETH_ALEN] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

static int ath6kl_del_station(struct wiphy *wiphy, struct net_device *dev,
			      struct station_del_parameters *params)
{
	struct ath6kl *ar = ath6kl_priv(dev);
	struct ath6kl_vif *vif = netdev_priv(dev);
	const u8 *addr = params->mac ? params->mac : bcast_addr;

	return ath6kl_wmi_ap_set_mlme(ar->wmi, vif->fw_vif_idx, WMI_AP_DEAUTH,
				      addr, WLAN_REASON_PREV_AUTH_NOT_VALID);
}

static int ath6kl_change_station(struct wiphy *wiphy, struct net_device *dev,
				 const u8 *mac,
				 struct station_parameters *params)
{
	struct ath6kl *ar = ath6kl_priv(dev);
	struct ath6kl_vif *vif = netdev_priv(dev);
	int err;

	if (vif->nw_type != AP_NETWORK)
		return -EOPNOTSUPP;

	err = cfg80211_check_station_change(wiphy, params,
					    CFG80211_STA_AP_MLME_CLIENT);
	if (err)
		return err;

	if (params->sta_flags_set & BIT(NL80211_STA_FLAG_AUTHORIZED))
		return ath6kl_wmi_ap_set_mlme(ar->wmi, vif->fw_vif_idx,
					      WMI_AP_MLME_AUTHORIZE, mac, 0);
	return ath6kl_wmi_ap_set_mlme(ar->wmi, vif->fw_vif_idx,
				      WMI_AP_MLME_UNAUTHORIZE, mac, 0);
}

static int ath6kl_remain_on_channel(struct wiphy *wiphy,
				    struct wireless_dev *wdev,
				    struct ieee80211_channel *chan,
				    unsigned int duration,
				    u64 *cookie)
{
	struct ath6kl_vif *vif = ath6kl_vif_from_wdev(wdev);
	struct ath6kl *ar = ath6kl_priv(vif->ndev);
	u32 id;

	/* TODO: if already pending or ongoing remain-on-channel,
	 * return -EBUSY */
	id = ++vif->last_roc_id;
	if (id == 0) {
		/* Do not use 0 as the cookie value */
		id = ++vif->last_roc_id;
	}
	*cookie = id;

	return ath6kl_wmi_remain_on_chnl_cmd(ar->wmi, vif->fw_vif_idx,
					     chan->center_freq, duration);
}

static int ath6kl_cancel_remain_on_channel(struct wiphy *wiphy,
					   struct wireless_dev *wdev,
					   u64 cookie)
{
	struct ath6kl_vif *vif = ath6kl_vif_from_wdev(wdev);
	struct ath6kl *ar = ath6kl_priv(vif->ndev);

	if (cookie != vif->last_roc_id)
		return -ENOENT;
	vif->last_cancel_roc_id = cookie;

	return ath6kl_wmi_cancel_remain_on_chnl_cmd(ar->wmi, vif->fw_vif_idx);
}

static int ath6kl_send_go_probe_resp(struct ath6kl_vif *vif,
				     const u8 *buf, size_t len,
				     unsigned int freq)
{
	struct ath6kl *ar = vif->ar;
	const u8 *pos;
	u8 *p2p;
	int p2p_len;
	int ret;
	const struct ieee80211_mgmt *mgmt;

	mgmt = (const struct ieee80211_mgmt *) buf;

	/* Include P2P IE(s) from the frame generated in user space. */

	p2p = kmalloc(len, GFP_KERNEL);
	if (p2p == NULL)
		return -ENOMEM;
	p2p_len = 0;

	pos = mgmt->u.probe_resp.variable;
	while (pos + 1 < buf + len) {
		if (pos + 2 + pos[1] > buf + len)
			break;
		if (ath6kl_is_p2p_ie(pos)) {
			memcpy(p2p + p2p_len, pos, 2 + pos[1]);
			p2p_len += 2 + pos[1];
		}
		pos += 2 + pos[1];
	}

	ret = ath6kl_wmi_send_probe_response_cmd(ar->wmi, vif->fw_vif_idx, freq,
						 mgmt->da, p2p, p2p_len);
	kfree(p2p);
	return ret;
}

static bool ath6kl_mgmt_powersave_ap(struct ath6kl_vif *vif,
				     u32 id,
				     u32 freq,
				     u32 wait,
				     const u8 *buf,
				     size_t len,
				     bool *more_data,
				     bool no_cck)
{
	struct ieee80211_mgmt *mgmt;
	struct ath6kl_sta *conn;
	bool is_psq_empty = false;
	struct ath6kl_mgmt_buff *mgmt_buf;
	size_t mgmt_buf_size;
	struct ath6kl *ar = vif->ar;

	mgmt = (struct ieee80211_mgmt *) buf;
	if (is_multicast_ether_addr(mgmt->da))
		return false;

	conn = ath6kl_find_sta(vif, mgmt->da);
	if (!conn)
		return false;

	if (conn->sta_flags & STA_PS_SLEEP) {
		if (!(conn->sta_flags & STA_PS_POLLED)) {
			/* Queue the frames if the STA is sleeping */
			mgmt_buf_size = len + sizeof(struct ath6kl_mgmt_buff);
			mgmt_buf = kmalloc(mgmt_buf_size, GFP_KERNEL);
			if (!mgmt_buf)
				return false;

			INIT_LIST_HEAD(&mgmt_buf->list);
			mgmt_buf->id = id;
			mgmt_buf->freq = freq;
			mgmt_buf->wait = wait;
			mgmt_buf->len = len;
			mgmt_buf->no_cck = no_cck;
			memcpy(mgmt_buf->buf, buf, len);
			spin_lock_bh(&conn->psq_lock);
			is_psq_empty = skb_queue_empty(&conn->psq) &&
					(conn->mgmt_psq_len == 0);
			list_add_tail(&mgmt_buf->list, &conn->mgmt_psq);
			conn->mgmt_psq_len++;
			spin_unlock_bh(&conn->psq_lock);

			/*
			 * If this is the first pkt getting queued
			 * for this STA, update the PVB for this
			 * STA.
			 */
			if (is_psq_empty)
				ath6kl_wmi_set_pvb_cmd(ar->wmi, vif->fw_vif_idx,
						       conn->aid, 1);
			return true;
		}

		/*
		 * This tx is because of a PsPoll.
		 * Determine if MoreData bit has to be set.
		 */
		spin_lock_bh(&conn->psq_lock);
		if (!skb_queue_empty(&conn->psq) || (conn->mgmt_psq_len != 0))
			*more_data = true;
		spin_unlock_bh(&conn->psq_lock);
	}

	return false;
}

/* Check if SSID length is greater than DIRECT- */
static bool ath6kl_is_p2p_go_ssid(const u8 *buf, size_t len)
{
	const struct ieee80211_mgmt *mgmt;
	mgmt = (const struct ieee80211_mgmt *) buf;

	/* variable[1] contains the SSID tag length */
	if (buf + len >= &mgmt->u.probe_resp.variable[1] &&
	    (mgmt->u.probe_resp.variable[1] > P2P_WILDCARD_SSID_LEN)) {
		return true;
	}

	return false;
}

static int ath6kl_mgmt_tx(struct wiphy *wiphy, struct wireless_dev *wdev,
			  struct cfg80211_mgmt_tx_params *params, u64 *cookie)
{
	struct ath6kl_vif *vif = ath6kl_vif_from_wdev(wdev);
	struct ath6kl *ar = ath6kl_priv(vif->ndev);
	struct ieee80211_channel *chan = params->chan;
	const u8 *buf = params->buf;
	size_t len = params->len;
	unsigned int wait = params->wait;
	bool no_cck = params->no_cck;
	u32 id, freq;
	const struct ieee80211_mgmt *mgmt;
	bool more_data, queued;

	/* default to the current channel, but use the one specified as argument
	 * if any
	 */
	freq = vif->ch_hint;
	if (chan)
		freq = chan->center_freq;

	/* never send freq zero to the firmware */
	if (WARN_ON(freq == 0))
		return -EINVAL;

	mgmt = (const struct ieee80211_mgmt *) buf;
	if (vif->nw_type == AP_NETWORK && test_bit(CONNECTED, &vif->flags) &&
	    ieee80211_is_probe_resp(mgmt->frame_control) &&
	    ath6kl_is_p2p_go_ssid(buf, len)) {
		/*
		 * Send Probe Response frame in GO mode using a separate WMI
		 * command to allow the target to fill in the generic IEs.
		 */
		*cookie = 0; /* TX status not supported */
		return ath6kl_send_go_probe_resp(vif, buf, len, freq);
	}

	id = vif->send_action_id++;
	if (id == 0) {
		/*
		 * 0 is a reserved value in the WMI command and shall not be
		 * used for the command.
		 */
		id = vif->send_action_id++;
	}

	*cookie = id;

	/* AP mode Power saving processing */
	if (vif->nw_type == AP_NETWORK) {
		queued = ath6kl_mgmt_powersave_ap(vif, id, freq, wait, buf, len,
						  &more_data, no_cck);
		if (queued)
			return 0;
	}

	return ath6kl_wmi_send_mgmt_cmd(ar->wmi, vif->fw_vif_idx, id, freq,
					wait, buf, len, no_cck);
}

static int ath6kl_get_antenna(struct wiphy *wiphy,
			      u32 *tx_ant, u32 *rx_ant)
{
	struct ath6kl *ar = wiphy_priv(wiphy);
	*tx_ant = ar->hw.tx_ant;
	*rx_ant = ar->hw.rx_ant;
	return 0;
}

static void ath6kl_update_mgmt_frame_registrations(struct wiphy *wiphy,
						   struct wireless_dev *wdev,
						   struct mgmt_frame_regs *upd)
{
	struct ath6kl_vif *vif = ath6kl_vif_from_wdev(wdev);

	/*
	 * FIXME: send WMI_PROBE_REQ_REPORT_CMD here instead of hardcoding
	 *	  the reporting in the target all the time, this callback
	 *	  *is* allowed to sleep after all.
	 */
	vif->probe_req_report =
		upd->interface_stypes & BIT(IEEE80211_STYPE_PROBE_REQ >> 4);
}

static int ath6kl_cfg80211_sscan_start(struct wiphy *wiphy,
			struct net_device *dev,
			struct cfg80211_sched_scan_request *request)
{
	struct ath6kl *ar = ath6kl_priv(dev);
	struct ath6kl_vif *vif = netdev_priv(dev);
	u16 interval;
	int ret, rssi_thold;
	int n_match_sets = request->n_match_sets;

	/*
	 * If there's a matchset w/o an SSID, then assume it's just for
	 * the RSSI (nothing else is currently supported) and ignore it.
	 * The device only supports a global RSSI filter that we set below.
	 */
	if (n_match_sets == 1 && !request->match_sets[0].ssid.ssid_len)
		n_match_sets = 0;

	if (ar->state != ATH6KL_STATE_ON)
		return -EIO;

	if (vif->sme_state != SME_DISCONNECTED)
		return -EBUSY;

	ath6kl_cfg80211_scan_complete_event(vif, true);

	ret = ath6kl_set_probed_ssids(ar, vif, request->ssids,
				      request->n_ssids,
				      request->match_sets,
				      n_match_sets);
	if (ret < 0)
		return ret;

	if (!n_match_sets) {
		ret = ath6kl_wmi_bssfilter_cmd(ar->wmi, vif->fw_vif_idx,
					       ALL_BSS_FILTER, 0);
		if (ret < 0)
			return ret;
	} else {
		 ret = ath6kl_wmi_bssfilter_cmd(ar->wmi, vif->fw_vif_idx,
						MATCHED_SSID_FILTER, 0);
		if (ret < 0)
			return ret;
	}

	if (test_bit(ATH6KL_FW_CAPABILITY_RSSI_SCAN_THOLD,
		     ar->fw_capabilities)) {
		if (request->min_rssi_thold <= NL80211_SCAN_RSSI_THOLD_OFF)
			rssi_thold = 0;
		else if (request->min_rssi_thold < -127)
			rssi_thold = -127;
		else
			rssi_thold = request->min_rssi_thold;

		ret = ath6kl_wmi_set_rssi_filter_cmd(ar->wmi, vif->fw_vif_idx,
						     rssi_thold);
		if (ret) {
			ath6kl_err("failed to set RSSI threshold for scan\n");
			return ret;
		}
	}

	/* fw uses seconds, also make sure that it's >0 */
	interval = max_t(u16, 1, request->scan_plans[0].interval);

	ath6kl_wmi_scanparams_cmd(ar->wmi, vif->fw_vif_idx,
				  interval, interval,
				  vif->bg_scan_period, 0, 0, 0, 3, 0, 0, 0);

	/* this also clears IE in fw if it's not set */
	ret = ath6kl_wmi_set_appie_cmd(ar->wmi, vif->fw_vif_idx,
				       WMI_FRAME_PROBE_REQ,
				       request->ie, request->ie_len);
	if (ret) {
		ath6kl_warn("Failed to set probe request IE for scheduled scan: %d\n",
			    ret);
		return ret;
	}

	ret = ath6kl_wmi_enable_sched_scan_cmd(ar->wmi, vif->fw_vif_idx, true);
	if (ret)
		return ret;

	set_bit(SCHED_SCANNING, &vif->flags);

	return 0;
}

static int ath6kl_cfg80211_sscan_stop(struct wiphy *wiphy,
				      struct net_device *dev, u64 reqid)
{
	struct ath6kl_vif *vif = netdev_priv(dev);
	bool stopped;

	stopped = __ath6kl_cfg80211_sscan_stop(vif);

	if (!stopped)
		return -EIO;

	return 0;
}

static int ath6kl_cfg80211_set_bitrate(struct wiphy *wiphy,
				       struct net_device *dev,
				       const u8 *addr,
				       const struct cfg80211_bitrate_mask *mask)
{
	struct ath6kl *ar = ath6kl_priv(dev);
	struct ath6kl_vif *vif = netdev_priv(dev);

	return ath6kl_wmi_set_bitrate_mask(ar->wmi, vif->fw_vif_idx,
					   mask);
}

static int ath6kl_cfg80211_set_txe_config(struct wiphy *wiphy,
					  struct net_device *dev,
					  u32 rate, u32 pkts, u32 intvl)
{
	struct ath6kl *ar = ath6kl_priv(dev);
	struct ath6kl_vif *vif = netdev_priv(dev);

	if (vif->nw_type != INFRA_NETWORK ||
	    !test_bit(ATH6KL_FW_CAPABILITY_TX_ERR_NOTIFY, ar->fw_capabilities))
		return -EOPNOTSUPP;

	if (vif->sme_state != SME_CONNECTED)
		return -ENOTCONN;

	/* save this since the firmware won't report the interval */
	vif->txe_intvl = intvl;

	return ath6kl_wmi_set_txe_notify(ar->wmi, vif->fw_vif_idx,
					 rate, pkts, intvl);
}

static const struct ieee80211_txrx_stypes
ath6kl_mgmt_stypes[NUM_NL80211_IFTYPES] = {
	[NL80211_IFTYPE_STATION] = {
		.tx = BIT(IEEE80211_STYPE_ACTION >> 4) |
		BIT(IEEE80211_STYPE_PROBE_RESP >> 4),
		.rx = BIT(IEEE80211_STYPE_ACTION >> 4) |
		BIT(IEEE80211_STYPE_PROBE_REQ >> 4)
	},
	[NL80211_IFTYPE_AP] = {
		.tx = BIT(IEEE80211_STYPE_ACTION >> 4) |
		BIT(IEEE80211_STYPE_PROBE_RESP >> 4),
		.rx = BIT(IEEE80211_STYPE_ACTION >> 4) |
		BIT(IEEE80211_STYPE_PROBE_REQ >> 4)
	},
	[NL80211_IFTYPE_P2P_CLIENT] = {
		.tx = BIT(IEEE80211_STYPE_ACTION >> 4) |
		BIT(IEEE80211_STYPE_PROBE_RESP >> 4),
		.rx = BIT(IEEE80211_STYPE_ACTION >> 4) |
		BIT(IEEE80211_STYPE_PROBE_REQ >> 4)
	},
	[NL80211_IFTYPE_P2P_GO] = {
		.tx = BIT(IEEE80211_STYPE_ACTION >> 4) |
		BIT(IEEE80211_STYPE_PROBE_RESP >> 4),
		.rx = BIT(IEEE80211_STYPE_ACTION >> 4) |
		BIT(IEEE80211_STYPE_PROBE_REQ >> 4)
	},
};

static struct cfg80211_ops ath6kl_cfg80211_ops = {
	.add_virtual_intf = ath6kl_cfg80211_add_iface,
	.del_virtual_intf = ath6kl_cfg80211_del_iface,
	.change_virtual_intf = ath6kl_cfg80211_change_iface,
	.scan = ath6kl_cfg80211_scan,
	.connect = ath6kl_cfg80211_connect,
	.disconnect = ath6kl_cfg80211_disconnect,
	.add_key = ath6kl_cfg80211_add_key,
	.get_key = ath6kl_cfg80211_get_key,
	.del_key = ath6kl_cfg80211_del_key,
	.set_default_key = ath6kl_cfg80211_set_default_key,
	.set_wiphy_params = ath6kl_cfg80211_set_wiphy_params,
	.set_tx_power = ath6kl_cfg80211_set_txpower,
	.get_tx_power = ath6kl_cfg80211_get_txpower,
	.set_power_mgmt = ath6kl_cfg80211_set_power_mgmt,
	.join_ibss = ath6kl_cfg80211_join_ibss,
	.leave_ibss = ath6kl_cfg80211_leave_ibss,
	.get_station = ath6kl_get_station,
	.set_pmksa = ath6kl_set_pmksa,
	.del_pmksa = ath6kl_del_pmksa,
	.flush_pmksa = ath6kl_flush_pmksa,
	CFG80211_TESTMODE_CMD(ath6kl_tm_cmd)
#ifdef CONFIG_PM
	.suspend = __ath6kl_cfg80211_suspend,
	.resume = __ath6kl_cfg80211_resume,
#endif
	.start_ap = ath6kl_start_ap,
	.change_beacon = ath6kl_change_beacon,
	.stop_ap = ath6kl_stop_ap,
	.del_station = ath6kl_del_station,
	.change_station = ath6kl_change_station,
	.remain_on_channel = ath6kl_remain_on_channel,
	.cancel_remain_on_channel = ath6kl_cancel_remain_on_channel,
	.mgmt_tx = ath6kl_mgmt_tx,
	.update_mgmt_frame_registrations =
		ath6kl_update_mgmt_frame_registrations,
	.get_antenna = ath6kl_get_antenna,
	.sched_scan_start = ath6kl_cfg80211_sscan_start,
	.sched_scan_stop = ath6kl_cfg80211_sscan_stop,
	.set_bitrate_mask = ath6kl_cfg80211_set_bitrate,
	.set_cqm_txe_config = ath6kl_cfg80211_set_txe_config,
};

void ath6kl_cfg80211_stop(struct ath6kl_vif *vif)
{
	ath6kl_cfg80211_sscan_disable(vif);

	switch (vif->sme_state) {
	case SME_DISCONNECTED:
		break;
	case SME_CONNECTING:
		cfg80211_connect_result(vif->ndev, vif->bssid, NULL, 0,
					NULL, 0,
					WLAN_STATUS_UNSPECIFIED_FAILURE,
					GFP_KERNEL);
		break;
	case SME_CONNECTED:
		cfg80211_disconnected(vif->ndev, 0, NULL, 0, true, GFP_KERNEL);
		break;
	}

	if (vif->ar->state != ATH6KL_STATE_RECOVERY &&
	    (test_bit(CONNECTED, &vif->flags) ||
	    test_bit(CONNECT_PEND, &vif->flags)))
		ath6kl_wmi_disconnect_cmd(vif->ar->wmi, vif->fw_vif_idx);

	vif->sme_state = SME_DISCONNECTED;
	clear_bit(CONNECTED, &vif->flags);
	clear_bit(CONNECT_PEND, &vif->flags);

	/* Stop netdev queues, needed during recovery */
	netif_stop_queue(vif->ndev);
	netif_carrier_off(vif->ndev);

	/* disable scanning */
	if (vif->ar->state != ATH6KL_STATE_RECOVERY &&
	    ath6kl_wmi_scanparams_cmd(vif->ar->wmi, vif->fw_vif_idx, 0xFFFF,
				      0, 0, 0, 0, 0, 0, 0, 0, 0) != 0)
		ath6kl_warn("failed to disable scan during stop\n");

	ath6kl_cfg80211_scan_complete_event(vif, true);
}

void ath6kl_cfg80211_stop_all(struct ath6kl *ar)
{
	struct ath6kl_vif *vif;

	vif = ath6kl_vif_first(ar);
	if (!vif && ar->state != ATH6KL_STATE_RECOVERY) {
		/* save the current power mode before enabling power save */
		ar->wmi->saved_pwr_mode = ar->wmi->pwr_mode;

		if (ath6kl_wmi_powermode_cmd(ar->wmi, 0, REC_POWER) != 0)
			ath6kl_warn("ath6kl_deep_sleep_enable: wmi_powermode_cmd failed\n");
		return;
	}

	/*
	 * FIXME: we should take ar->list_lock to protect changes in the
	 * vif_list, but that's not trivial to do as ath6kl_cfg80211_stop()
	 * sleeps.
	 */
	list_for_each_entry(vif, &ar->vif_list, list)
		ath6kl_cfg80211_stop(vif);
}

static void ath6kl_cfg80211_reg_notify(struct wiphy *wiphy,
				       struct regulatory_request *request)
{
	struct ath6kl *ar = wiphy_priv(wiphy);
	u32 rates[NUM_NL80211_BANDS];
	int ret, i;

	ath6kl_dbg(ATH6KL_DBG_WLAN_CFG,
		   "cfg reg_notify %c%c%s%s initiator %d hint_type %d\n",
		   request->alpha2[0], request->alpha2[1],
		   request->intersect ? " intersect" : "",
		   request->processed ? " processed" : "",
		   request->initiator, request->user_reg_hint_type);

	if (request->user_reg_hint_type != NL80211_USER_REG_HINT_CELL_BASE)
		return;

	ret = ath6kl_wmi_set_regdomain_cmd(ar->wmi, request->alpha2);
	if (ret) {
		ath6kl_err("failed to set regdomain: %d\n", ret);
		return;
	}

	/*
	 * Firmware will apply the regdomain change only after a scan is
	 * issued and it will send a WMI_REGDOMAIN_EVENTID when it has been
	 * changed.
	 */

	for (i = 0; i < NUM_NL80211_BANDS; i++)
		if (wiphy->bands[i])
			rates[i] = (1 << wiphy->bands[i]->n_bitrates) - 1;


	ret = ath6kl_wmi_beginscan_cmd(ar->wmi, 0, WMI_LONG_SCAN, false,
				       false, 0, ATH6KL_FG_SCAN_INTERVAL,
				       0, NULL, false, rates);
	if (ret) {
		ath6kl_err("failed to start scan for a regdomain change: %d\n",
			   ret);
		return;
	}
}

static int ath6kl_cfg80211_vif_init(struct ath6kl_vif *vif)
{
	vif->aggr_cntxt = aggr_init(vif);
	if (!vif->aggr_cntxt) {
		ath6kl_err("failed to initialize aggr\n");
		return -ENOMEM;
	}

	timer_setup(&vif->disconnect_timer, disconnect_timer_handler, 0);
	timer_setup(&vif->sched_scan_timer, ath6kl_wmi_sscan_timer, 0);

	set_bit(WMM_ENABLED, &vif->flags);
	spin_lock_init(&vif->if_lock);

	INIT_LIST_HEAD(&vif->mc_filter);

	return 0;
}

void ath6kl_cfg80211_vif_stop(struct ath6kl_vif *vif, bool wmi_ready)
{
	static u8 bcast_mac[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	bool discon_issued;

	netif_stop_queue(vif->ndev);

	clear_bit(WLAN_ENABLED, &vif->flags);

	if (wmi_ready) {
		discon_issued = test_bit(CONNECTED, &vif->flags) ||
				test_bit(CONNECT_PEND, &vif->flags);
		ath6kl_disconnect(vif);
		del_timer(&vif->disconnect_timer);

		if (discon_issued)
			ath6kl_disconnect_event(vif, DISCONNECT_CMD,
						(vif->nw_type & AP_NETWORK) ?
						bcast_mac : vif->bssid,
						0, NULL, 0);
	}

	if (vif->scan_req) {
		struct cfg80211_scan_info info = {
			.aborted = true,
		};

		cfg80211_scan_done(vif->scan_req, &info);
		vif->scan_req = NULL;
	}

	/* need to clean up enhanced bmiss detection fw state */
	ath6kl_cfg80211_sta_bmiss_enhance(vif, false);
}

void ath6kl_cfg80211_vif_cleanup(struct ath6kl_vif *vif)
{
	struct ath6kl *ar = vif->ar;
	struct ath6kl_mc_filter *mc_filter, *tmp;

	aggr_module_destroy(vif->aggr_cntxt);

	ar->avail_idx_map |= BIT(vif->fw_vif_idx);

	if (vif->nw_type == ADHOC_NETWORK)
		ar->ibss_if_active = false;

	list_for_each_entry_safe(mc_filter, tmp, &vif->mc_filter, list) {
		list_del(&mc_filter->list);
		kfree(mc_filter);
	}

	cfg80211_unregister_netdevice(vif->ndev);

	ar->num_vif--;
}

static const char ath6kl_gstrings_sta_stats[][ETH_GSTRING_LEN] = {
	/* Common stats names used by many drivers. */
	"tx_pkts_nic", "tx_bytes_nic", "rx_pkts_nic", "rx_bytes_nic",

	/* TX stats. */
	"d_tx_ucast_pkts", "d_tx_bcast_pkts",
	"d_tx_ucast_bytes", "d_tx_bcast_bytes",
	"d_tx_rts_ok", "d_tx_error", "d_tx_fail",
	"d_tx_retry", "d_tx_multi_retry", "d_tx_rts_fail",
	"d_tx_tkip_counter_measures",

	/* RX Stats. */
	"d_rx_ucast_pkts", "d_rx_ucast_rate", "d_rx_bcast_pkts",
	"d_rx_ucast_bytes", "d_rx_bcast_bytes", "d_rx_frag_pkt",
	"d_rx_error", "d_rx_crc_err", "d_rx_keycache_miss",
	"d_rx_decrypt_crc_err", "d_rx_duplicate_frames",
	"d_rx_mic_err", "d_rx_tkip_format_err", "d_rx_ccmp_format_err",
	"d_rx_ccmp_replay_err",

	/* Misc stats. */
	"d_beacon_miss", "d_num_connects", "d_num_disconnects",
	"d_beacon_avg_rssi", "d_arp_received", "d_arp_matched",
	"d_arp_replied"
};

#define ATH6KL_STATS_LEN	ARRAY_SIZE(ath6kl_gstrings_sta_stats)

static int ath6kl_get_sset_count(struct net_device *dev, int sset)
{
	int rv = 0;

	if (sset == ETH_SS_STATS)
		rv += ATH6KL_STATS_LEN;

	if (rv == 0)
		return -EOPNOTSUPP;
	return rv;
}

static void ath6kl_get_stats(struct net_device *dev,
			    struct ethtool_stats *stats,
			    u64 *data)
{
	struct ath6kl_vif *vif = netdev_priv(dev);
	struct ath6kl *ar = vif->ar;
	int i = 0;
	struct target_stats *tgt_stats;

	memset(data, 0, sizeof(u64) * ATH6KL_STATS_LEN);

	ath6kl_read_tgt_stats(ar, vif);

	tgt_stats = &vif->target_stats;

	data[i++] = tgt_stats->tx_ucast_pkt + tgt_stats->tx_bcast_pkt;
	data[i++] = tgt_stats->tx_ucast_byte + tgt_stats->tx_bcast_byte;
	data[i++] = tgt_stats->rx_ucast_pkt + tgt_stats->rx_bcast_pkt;
	data[i++] = tgt_stats->rx_ucast_byte + tgt_stats->rx_bcast_byte;

	data[i++] = tgt_stats->tx_ucast_pkt;
	data[i++] = tgt_stats->tx_bcast_pkt;
	data[i++] = tgt_stats->tx_ucast_byte;
	data[i++] = tgt_stats->tx_bcast_byte;
	data[i++] = tgt_stats->tx_rts_success_cnt;
	data[i++] = tgt_stats->tx_err;
	data[i++] = tgt_stats->tx_fail_cnt;
	data[i++] = tgt_stats->tx_retry_cnt;
	data[i++] = tgt_stats->tx_mult_retry_cnt;
	data[i++] = tgt_stats->tx_rts_fail_cnt;
	data[i++] = tgt_stats->tkip_cnter_measures_invoked;

	data[i++] = tgt_stats->rx_ucast_pkt;
	data[i++] = tgt_stats->rx_ucast_rate;
	data[i++] = tgt_stats->rx_bcast_pkt;
	data[i++] = tgt_stats->rx_ucast_byte;
	data[i++] = tgt_stats->rx_bcast_byte;
	data[i++] = tgt_stats->rx_frgment_pkt;
	data[i++] = tgt_stats->rx_err;
	data[i++] = tgt_stats->rx_crc_err;
	data[i++] = tgt_stats->rx_key_cache_miss;
	data[i++] = tgt_stats->rx_decrypt_err;
	data[i++] = tgt_stats->rx_dupl_frame;
	data[i++] = tgt_stats->tkip_local_mic_fail;
	data[i++] = tgt_stats->tkip_fmt_err;
	data[i++] = tgt_stats->ccmp_fmt_err;
	data[i++] = tgt_stats->ccmp_replays;

	data[i++] = tgt_stats->cs_bmiss_cnt;
	data[i++] = tgt_stats->cs_connect_cnt;
	data[i++] = tgt_stats->cs_discon_cnt;
	data[i++] = tgt_stats->cs_ave_beacon_rssi;
	data[i++] = tgt_stats->arp_received;
	data[i++] = tgt_stats->arp_matched;
	data[i++] = tgt_stats->arp_replied;

	if (i !=  ATH6KL_STATS_LEN) {
		WARN_ON_ONCE(1);
		ath6kl_err("ethtool stats error, i: %d  STATS_LEN: %d\n",
			   i, (int)ATH6KL_STATS_LEN);
	}
}

/* These stats are per NIC, not really per vdev, so we just ignore dev. */
static void ath6kl_get_strings(struct net_device *dev, u32 sset, u8 *data)
{
	int sz_sta_stats = 0;

	if (sset == ETH_SS_STATS) {
		sz_sta_stats = sizeof(ath6kl_gstrings_sta_stats);
		memcpy(data, ath6kl_gstrings_sta_stats, sz_sta_stats);
	}
}

static const struct ethtool_ops ath6kl_ethtool_ops = {
	.get_drvinfo = cfg80211_get_drvinfo,
	.get_link = ethtool_op_get_link,
	.get_strings = ath6kl_get_strings,
	.get_ethtool_stats = ath6kl_get_stats,
	.get_sset_count = ath6kl_get_sset_count,
};

struct wireless_dev *ath6kl_interface_add(struct ath6kl *ar, const char *name,
					  unsigned char name_assign_type,
					  enum nl80211_iftype type,
					  u8 fw_vif_idx, u8 nw_type)
{
	struct net_device *ndev;
	struct ath6kl_vif *vif;

	ndev = alloc_netdev(sizeof(*vif), name, name_assign_type, ether_setup);
	if (!ndev)
		return NULL;

	vif = netdev_priv(ndev);
	ndev->ieee80211_ptr = &vif->wdev;
	vif->wdev.wiphy = ar->wiphy;
	vif->ar = ar;
	vif->ndev = ndev;
	SET_NETDEV_DEV(ndev, wiphy_dev(vif->wdev.wiphy));
	vif->wdev.netdev = ndev;
	vif->wdev.iftype = type;
	vif->fw_vif_idx = fw_vif_idx;
	vif->nw_type = nw_type;
	vif->next_mode = nw_type;
	vif->listen_intvl_t = ATH6KL_DEFAULT_LISTEN_INTVAL;
	vif->bmiss_time_t = ATH6KL_DEFAULT_BMISS_TIME;
	vif->bg_scan_period = 0;
	vif->htcap[NL80211_BAND_2GHZ].ht_enable = true;
	vif->htcap[NL80211_BAND_5GHZ].ht_enable = true;

	memcpy(ndev->dev_addr, ar->mac_addr, ETH_ALEN);
	if (fw_vif_idx != 0) {
		ndev->dev_addr[0] = (ndev->dev_addr[0] ^ (1 << fw_vif_idx)) |
				     0x2;
		if (test_bit(ATH6KL_FW_CAPABILITY_CUSTOM_MAC_ADDR,
			     ar->fw_capabilities))
			ndev->dev_addr[4] ^= 0x80;
	}

	init_netdev(ndev);

	ath6kl_init_control_info(vif);

	if (ath6kl_cfg80211_vif_init(vif))
		goto err;

	netdev_set_default_ethtool_ops(ndev, &ath6kl_ethtool_ops);

	if (cfg80211_register_netdevice(ndev))
		goto err;

	ar->avail_idx_map &= ~BIT(fw_vif_idx);
	vif->sme_state = SME_DISCONNECTED;
	set_bit(WLAN_ENABLED, &vif->flags);
	ar->wlan_pwr_state = WLAN_POWER_STATE_ON;

	if (type == NL80211_IFTYPE_ADHOC)
		ar->ibss_if_active = true;

	spin_lock_bh(&ar->list_lock);
	list_add_tail(&vif->list, &ar->vif_list);
	spin_unlock_bh(&ar->list_lock);

	return &vif->wdev;

err:
	aggr_module_destroy(vif->aggr_cntxt);
	free_netdev(ndev);
	return NULL;
}

#ifdef CONFIG_PM
static const struct wiphy_wowlan_support ath6kl_wowlan_support = {
	.flags = WIPHY_WOWLAN_MAGIC_PKT |
		 WIPHY_WOWLAN_DISCONNECT |
		 WIPHY_WOWLAN_GTK_REKEY_FAILURE  |
		 WIPHY_WOWLAN_SUPPORTS_GTK_REKEY |
		 WIPHY_WOWLAN_EAP_IDENTITY_REQ   |
		 WIPHY_WOWLAN_4WAY_HANDSHAKE,
	.n_patterns = WOW_MAX_FILTERS_PER_LIST,
	.pattern_min_len = 1,
	.pattern_max_len = WOW_PATTERN_SIZE,
};
#endif

int ath6kl_cfg80211_init(struct ath6kl *ar)
{
	struct wiphy *wiphy = ar->wiphy;
	bool band_2gig = false, band_5gig = false, ht = false;
	int ret;

	wiphy->mgmt_stypes = ath6kl_mgmt_stypes;

	wiphy->max_remain_on_channel_duration = 5000;

	/* set device pointer for wiphy */
	set_wiphy_dev(wiphy, ar->dev);

	wiphy->interface_modes = BIT(NL80211_IFTYPE_STATION) |
				 BIT(NL80211_IFTYPE_ADHOC) |
				 BIT(NL80211_IFTYPE_AP);
	if (ar->p2p) {
		wiphy->interface_modes |= BIT(NL80211_IFTYPE_P2P_GO) |
					  BIT(NL80211_IFTYPE_P2P_CLIENT);
	}

	if (IS_ENABLED(CONFIG_ATH6KL_REGDOMAIN) &&
	    test_bit(ATH6KL_FW_CAPABILITY_REGDOMAIN, ar->fw_capabilities)) {
		wiphy->reg_notifier = ath6kl_cfg80211_reg_notify;
		ar->wiphy->features |= NL80211_FEATURE_CELL_BASE_REG_HINTS;
	}

	/* max num of ssids that can be probed during scanning */
	wiphy->max_scan_ssids = MAX_PROBED_SSIDS;

	/* max num of ssids that can be matched after scan */
	if (test_bit(ATH6KL_FW_CAPABILITY_SCHED_SCAN_MATCH_LIST,
		     ar->fw_capabilities))
		wiphy->max_match_sets = MAX_PROBED_SSIDS;

	wiphy->max_scan_ie_len = 1000; /* FIX: what is correct limit? */
	switch (ar->hw.cap) {
	case WMI_11AN_CAP:
		ht = true;
		fallthrough;
	case WMI_11A_CAP:
		band_5gig = true;
		break;
	case WMI_11GN_CAP:
		ht = true;
		fallthrough;
	case WMI_11G_CAP:
		band_2gig = true;
		break;
	case WMI_11AGN_CAP:
		ht = true;
		fallthrough;
	case WMI_11AG_CAP:
		band_2gig = true;
		band_5gig = true;
		break;
	default:
		ath6kl_err("invalid phy capability!\n");
		return -EINVAL;
	}

	/*
	 * Even if the fw has HT support, advertise HT cap only when
	 * the firmware has support to override RSN capability, otherwise
	 * 4-way handshake would fail.
	 */
	if (!(ht &&
	      test_bit(ATH6KL_FW_CAPABILITY_RSN_CAP_OVERRIDE,
		       ar->fw_capabilities))) {
		ath6kl_band_2ghz.ht_cap.cap = 0;
		ath6kl_band_2ghz.ht_cap.ht_supported = false;
		ath6kl_band_5ghz.ht_cap.cap = 0;
		ath6kl_band_5ghz.ht_cap.ht_supported = false;

		if (ht)
			ath6kl_err("Firmware lacks RSN-CAP-OVERRIDE, so HT (802.11n) is disabled.");
	}

	if (test_bit(ATH6KL_FW_CAPABILITY_64BIT_RATES,
		     ar->fw_capabilities)) {
		ath6kl_band_2ghz.ht_cap.mcs.rx_mask[0] = 0xff;
		ath6kl_band_5ghz.ht_cap.mcs.rx_mask[0] = 0xff;
		ath6kl_band_2ghz.ht_cap.mcs.rx_mask[1] = 0xff;
		ath6kl_band_5ghz.ht_cap.mcs.rx_mask[1] = 0xff;
		ar->hw.tx_ant = 0x3; /* mask, 2 antenna */
		ar->hw.rx_ant = 0x3;
	} else {
		ath6kl_band_2ghz.ht_cap.mcs.rx_mask[0] = 0xff;
		ath6kl_band_5ghz.ht_cap.mcs.rx_mask[0] = 0xff;
		ar->hw.tx_ant = 1;
		ar->hw.rx_ant = 1;
	}

	wiphy->available_antennas_tx = ar->hw.tx_ant;
	wiphy->available_antennas_rx = ar->hw.rx_ant;

	if (band_2gig)
		wiphy->bands[NL80211_BAND_2GHZ] = &ath6kl_band_2ghz;
	if (band_5gig)
		wiphy->bands[NL80211_BAND_5GHZ] = &ath6kl_band_5ghz;

	wiphy->signal_type = CFG80211_SIGNAL_TYPE_MBM;

	wiphy->cipher_suites = cipher_suites;
	wiphy->n_cipher_suites = ARRAY_SIZE(cipher_suites);

#ifdef CONFIG_PM
	wiphy->wowlan = &ath6kl_wowlan_support;
#endif

	wiphy->max_sched_scan_ssids = MAX_PROBED_SSIDS;

	ar->wiphy->flags |= WIPHY_FLAG_SUPPORTS_FW_ROAM |
			    WIPHY_FLAG_HAVE_AP_SME |
			    WIPHY_FLAG_HAS_REMAIN_ON_CHANNEL |
			    WIPHY_FLAG_AP_PROBE_RESP_OFFLOAD;

	if (test_bit(ATH6KL_FW_CAPABILITY_SCHED_SCAN_V2, ar->fw_capabilities))
		ar->wiphy->max_sched_scan_reqs = 1;

	if (test_bit(ATH6KL_FW_CAPABILITY_INACTIVITY_TIMEOUT,
		     ar->fw_capabilities))
		ar->wiphy->features |= NL80211_FEATURE_INACTIVITY_TIMER;

	ar->wiphy->probe_resp_offload =
		NL80211_PROBE_RESP_OFFLOAD_SUPPORT_WPS |
		NL80211_PROBE_RESP_OFFLOAD_SUPPORT_WPS2 |
		NL80211_PROBE_RESP_OFFLOAD_SUPPORT_P2P;

	ret = wiphy_register(wiphy);
	if (ret < 0) {
		ath6kl_err("couldn't register wiphy device\n");
		return ret;
	}

	ar->wiphy_registered = true;

	return 0;
}

void ath6kl_cfg80211_cleanup(struct ath6kl *ar)
{
	wiphy_unregister(ar->wiphy);

	ar->wiphy_registered = false;
}

struct ath6kl *ath6kl_cfg80211_create(void)
{
	struct ath6kl *ar;
	struct wiphy *wiphy;

	/* create a new wiphy for use with cfg80211 */
	wiphy = wiphy_new(&ath6kl_cfg80211_ops, sizeof(struct ath6kl));

	if (!wiphy) {
		ath6kl_err("couldn't allocate wiphy device\n");
		return NULL;
	}

	ar = wiphy_priv(wiphy);
	ar->wiphy = wiphy;

	return ar;
}

/* Note: ar variable must not be accessed after calling this! */
void ath6kl_cfg80211_destroy(struct ath6kl *ar)
{
	int i;

	for (i = 0; i < AP_MAX_NUM_STA; i++)
		kfree(ar->sta_list[i].aggr_conn);

	wiphy_free(ar->wiphy);
}

