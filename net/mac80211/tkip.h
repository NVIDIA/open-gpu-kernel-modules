/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright 2002-2004, Instant802 Networks, Inc.
 */

#ifndef TKIP_H
#define TKIP_H

#include <linux/types.h>
#include <linux/crypto.h>
#include "key.h"

int ieee80211_tkip_encrypt_data(struct arc4_ctx *ctx,
				struct ieee80211_key *key,
				struct sk_buff *skb,
				u8 *payload, size_t payload_len);

enum {
	TKIP_DECRYPT_OK = 0,
	TKIP_DECRYPT_NO_EXT_IV = -1,
	TKIP_DECRYPT_INVALID_KEYIDX = -2,
	TKIP_DECRYPT_REPLAY = -3,
};
int ieee80211_tkip_decrypt_data(struct arc4_ctx *ctx,
				struct ieee80211_key *key,
				u8 *payload, size_t payload_len, u8 *ta,
				u8 *ra, int only_iv, int queue,
				u32 *out_iv32, u16 *out_iv16);

#endif /* TKIP_H */
