// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (c) 1996, 2003 VIA Networking Technologies, Inc.
 * All rights reserved.
 *
 * File: key.c
 *
 * Purpose: Implement functions for 802.11i Key management
 *
 * Author: Jerry Chen
 *
 * Date: May 29, 2003
 *
 */

#include "tmacro.h"
#include "key.h"
#include "mac.h"

static int vnt_set_keymode(struct ieee80211_hw *hw, u8 *mac_addr,
			   struct ieee80211_key_conf *key, u32 key_type,
			   u32 mode, bool onfly_latch)
{
	struct vnt_private *priv = hw->priv;
	u8 broadcast[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	u16 key_mode = 0;
	u32 entry = 0;
	u8 *bssid;
	u8 key_inx = key->keyidx;
	u8 i;

	if (mac_addr)
		bssid = mac_addr;
	else
		bssid = &broadcast[0];

	if (key_type != VNT_KEY_DEFAULTKEY) {
		for (i = 0; i < (MAX_KEY_TABLE - 1); i++) {
			if (!test_bit(i, &priv->key_entry_inuse)) {
				set_bit(i, &priv->key_entry_inuse);

				key->hw_key_idx = i;
				entry = key->hw_key_idx;
				break;
			}
		}
	}

	switch (key_type) {
	case VNT_KEY_DEFAULTKEY:
		/* default key last entry */
		entry = MAX_KEY_TABLE - 1;
		key->hw_key_idx = entry;
		fallthrough;
	case VNT_KEY_ALLGROUP:
		key_mode |= VNT_KEY_ALLGROUP;
		if (onfly_latch)
			key_mode |= VNT_KEY_ONFLY_ALL;
		fallthrough;
	case VNT_KEY_GROUP_ADDRESS:
		key_mode |= mode;
		fallthrough;
	case VNT_KEY_GROUP:
		key_mode |= (mode << 4);
		key_mode |= VNT_KEY_GROUP;
		break;
	case  VNT_KEY_PAIRWISE:
		key_mode |= mode;
		key_inx = 4;
		break;
	default:
		return -EINVAL;
	}

	if (onfly_latch)
		key_mode |= VNT_KEY_ONFLY;

	if (mode == KEY_CTL_WEP) {
		if (key->keylen == WLAN_KEY_LEN_WEP40)
			key->key[15] &= 0x7f;
		if (key->keylen == WLAN_KEY_LEN_WEP104)
			key->key[15] |= 0x80;
	}

	MACvSetKeyEntry(priv, key_mode, entry, key_inx,
			bssid, (u32 *)key->key, priv->byLocalID);

	return 0;
}

int vnt_set_keys(struct ieee80211_hw *hw, struct ieee80211_sta *sta,
		 struct ieee80211_vif *vif, struct ieee80211_key_conf *key)
{
	struct ieee80211_bss_conf *conf = &vif->bss_conf;
	struct vnt_private *priv = hw->priv;
	u8 *mac_addr = NULL;
	u8 key_dec_mode = 0;
	int ret = 0;
	u32 u;

	if (sta)
		mac_addr = &sta->addr[0];

	switch (key->cipher) {
	case 0:
		for (u = 0 ; u < MAX_KEY_TABLE; u++)
			MACvDisableKeyEntry(priv, u);
		return ret;

	case WLAN_CIPHER_SUITE_WEP40:
	case WLAN_CIPHER_SUITE_WEP104:
		for (u = 0; u < MAX_KEY_TABLE; u++)
			MACvDisableKeyEntry(priv, u);

		vnt_set_keymode(hw, mac_addr,
				key, VNT_KEY_DEFAULTKEY, KEY_CTL_WEP, true);

		key->flags |= IEEE80211_KEY_FLAG_GENERATE_IV;

		return ret;
	case WLAN_CIPHER_SUITE_TKIP:
		key->flags |= IEEE80211_KEY_FLAG_GENERATE_MMIC;
		key->flags |= IEEE80211_KEY_FLAG_GENERATE_IV;

		key_dec_mode = KEY_CTL_TKIP;

		break;
	case WLAN_CIPHER_SUITE_CCMP:
		key_dec_mode = KEY_CTL_CCMP;

		key->flags |= IEEE80211_KEY_FLAG_GENERATE_IV;
	}

	if (key->flags & IEEE80211_KEY_FLAG_PAIRWISE) {
		vnt_set_keymode(hw, mac_addr,
				key, VNT_KEY_PAIRWISE, key_dec_mode, true);
	} else {
		vnt_set_keymode(hw, mac_addr,
				key, VNT_KEY_DEFAULTKEY, key_dec_mode, true);

		vnt_set_keymode(hw, (u8 *)conf->bssid,
				key, VNT_KEY_GROUP_ADDRESS, key_dec_mode, true);
	}

	return 0;
}
