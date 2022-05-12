// SPDX-License-Identifier: GPL-2.0-only
/*
 * lib80211 crypt: host-based TKIP encryption implementation for lib80211
 *
 * Copyright (c) 2003-2004, Jouni Malinen <j@w1.fi>
 * Copyright (c) 2008, John W. Linville <linville@tuxdriver.com>
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/err.h>
#include <linux/fips.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/random.h>
#include <linux/scatterlist.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/mm.h>
#include <linux/if_ether.h>
#include <linux/if_arp.h>
#include <asm/string.h>

#include <linux/wireless.h>
#include <linux/ieee80211.h>
#include <net/iw_handler.h>

#include <crypto/arc4.h>
#include <crypto/hash.h>
#include <linux/crypto.h>
#include <linux/crc32.h>

#include <net/lib80211.h>

MODULE_AUTHOR("Jouni Malinen");
MODULE_DESCRIPTION("lib80211 crypt: TKIP");
MODULE_LICENSE("GPL");

#define TKIP_HDR_LEN 8

struct lib80211_tkip_data {
#define TKIP_KEY_LEN 32
	u8 key[TKIP_KEY_LEN];
	int key_set;

	u32 tx_iv32;
	u16 tx_iv16;
	u16 tx_ttak[5];
	int tx_phase1_done;

	u32 rx_iv32;
	u16 rx_iv16;
	u16 rx_ttak[5];
	int rx_phase1_done;
	u32 rx_iv32_new;
	u16 rx_iv16_new;

	u32 dot11RSNAStatsTKIPReplays;
	u32 dot11RSNAStatsTKIPICVErrors;
	u32 dot11RSNAStatsTKIPLocalMICFailures;

	int key_idx;

	struct arc4_ctx rx_ctx_arc4;
	struct arc4_ctx tx_ctx_arc4;
	struct crypto_shash *rx_tfm_michael;
	struct crypto_shash *tx_tfm_michael;

	/* scratch buffers for virt_to_page() (crypto API) */
	u8 rx_hdr[16], tx_hdr[16];

	unsigned long flags;
};

static unsigned long lib80211_tkip_set_flags(unsigned long flags, void *priv)
{
	struct lib80211_tkip_data *_priv = priv;
	unsigned long old_flags = _priv->flags;
	_priv->flags = flags;
	return old_flags;
}

static unsigned long lib80211_tkip_get_flags(void *priv)
{
	struct lib80211_tkip_data *_priv = priv;
	return _priv->flags;
}

static void *lib80211_tkip_init(int key_idx)
{
	struct lib80211_tkip_data *priv;

	if (fips_enabled)
		return NULL;

	priv = kzalloc(sizeof(*priv), GFP_ATOMIC);
	if (priv == NULL)
		goto fail;

	priv->key_idx = key_idx;

	priv->tx_tfm_michael = crypto_alloc_shash("michael_mic", 0, 0);
	if (IS_ERR(priv->tx_tfm_michael)) {
		priv->tx_tfm_michael = NULL;
		goto fail;
	}

	priv->rx_tfm_michael = crypto_alloc_shash("michael_mic", 0, 0);
	if (IS_ERR(priv->rx_tfm_michael)) {
		priv->rx_tfm_michael = NULL;
		goto fail;
	}

	return priv;

      fail:
	if (priv) {
		crypto_free_shash(priv->tx_tfm_michael);
		crypto_free_shash(priv->rx_tfm_michael);
		kfree(priv);
	}

	return NULL;
}

static void lib80211_tkip_deinit(void *priv)
{
	struct lib80211_tkip_data *_priv = priv;
	if (_priv) {
		crypto_free_shash(_priv->tx_tfm_michael);
		crypto_free_shash(_priv->rx_tfm_michael);
	}
	kfree_sensitive(priv);
}

static inline u16 RotR1(u16 val)
{
	return (val >> 1) | (val << 15);
}

static inline u8 Lo8(u16 val)
{
	return val & 0xff;
}

static inline u8 Hi8(u16 val)
{
	return val >> 8;
}

static inline u16 Lo16(u32 val)
{
	return val & 0xffff;
}

static inline u16 Hi16(u32 val)
{
	return val >> 16;
}

static inline u16 Mk16(u8 hi, u8 lo)
{
	return lo | (((u16) hi) << 8);
}

static inline u16 Mk16_le(__le16 * v)
{
	return le16_to_cpu(*v);
}

static const u16 Sbox[256] = {
	0xC6A5, 0xF884, 0xEE99, 0xF68D, 0xFF0D, 0xD6BD, 0xDEB1, 0x9154,
	0x6050, 0x0203, 0xCEA9, 0x567D, 0xE719, 0xB562, 0x4DE6, 0xEC9A,
	0x8F45, 0x1F9D, 0x8940, 0xFA87, 0xEF15, 0xB2EB, 0x8EC9, 0xFB0B,
	0x41EC, 0xB367, 0x5FFD, 0x45EA, 0x23BF, 0x53F7, 0xE496, 0x9B5B,
	0x75C2, 0xE11C, 0x3DAE, 0x4C6A, 0x6C5A, 0x7E41, 0xF502, 0x834F,
	0x685C, 0x51F4, 0xD134, 0xF908, 0xE293, 0xAB73, 0x6253, 0x2A3F,
	0x080C, 0x9552, 0x4665, 0x9D5E, 0x3028, 0x37A1, 0x0A0F, 0x2FB5,
	0x0E09, 0x2436, 0x1B9B, 0xDF3D, 0xCD26, 0x4E69, 0x7FCD, 0xEA9F,
	0x121B, 0x1D9E, 0x5874, 0x342E, 0x362D, 0xDCB2, 0xB4EE, 0x5BFB,
	0xA4F6, 0x764D, 0xB761, 0x7DCE, 0x527B, 0xDD3E, 0x5E71, 0x1397,
	0xA6F5, 0xB968, 0x0000, 0xC12C, 0x4060, 0xE31F, 0x79C8, 0xB6ED,
	0xD4BE, 0x8D46, 0x67D9, 0x724B, 0x94DE, 0x98D4, 0xB0E8, 0x854A,
	0xBB6B, 0xC52A, 0x4FE5, 0xED16, 0x86C5, 0x9AD7, 0x6655, 0x1194,
	0x8ACF, 0xE910, 0x0406, 0xFE81, 0xA0F0, 0x7844, 0x25BA, 0x4BE3,
	0xA2F3, 0x5DFE, 0x80C0, 0x058A, 0x3FAD, 0x21BC, 0x7048, 0xF104,
	0x63DF, 0x77C1, 0xAF75, 0x4263, 0x2030, 0xE51A, 0xFD0E, 0xBF6D,
	0x814C, 0x1814, 0x2635, 0xC32F, 0xBEE1, 0x35A2, 0x88CC, 0x2E39,
	0x9357, 0x55F2, 0xFC82, 0x7A47, 0xC8AC, 0xBAE7, 0x322B, 0xE695,
	0xC0A0, 0x1998, 0x9ED1, 0xA37F, 0x4466, 0x547E, 0x3BAB, 0x0B83,
	0x8CCA, 0xC729, 0x6BD3, 0x283C, 0xA779, 0xBCE2, 0x161D, 0xAD76,
	0xDB3B, 0x6456, 0x744E, 0x141E, 0x92DB, 0x0C0A, 0x486C, 0xB8E4,
	0x9F5D, 0xBD6E, 0x43EF, 0xC4A6, 0x39A8, 0x31A4, 0xD337, 0xF28B,
	0xD532, 0x8B43, 0x6E59, 0xDAB7, 0x018C, 0xB164, 0x9CD2, 0x49E0,
	0xD8B4, 0xACFA, 0xF307, 0xCF25, 0xCAAF, 0xF48E, 0x47E9, 0x1018,
	0x6FD5, 0xF088, 0x4A6F, 0x5C72, 0x3824, 0x57F1, 0x73C7, 0x9751,
	0xCB23, 0xA17C, 0xE89C, 0x3E21, 0x96DD, 0x61DC, 0x0D86, 0x0F85,
	0xE090, 0x7C42, 0x71C4, 0xCCAA, 0x90D8, 0x0605, 0xF701, 0x1C12,
	0xC2A3, 0x6A5F, 0xAEF9, 0x69D0, 0x1791, 0x9958, 0x3A27, 0x27B9,
	0xD938, 0xEB13, 0x2BB3, 0x2233, 0xD2BB, 0xA970, 0x0789, 0x33A7,
	0x2DB6, 0x3C22, 0x1592, 0xC920, 0x8749, 0xAAFF, 0x5078, 0xA57A,
	0x038F, 0x59F8, 0x0980, 0x1A17, 0x65DA, 0xD731, 0x84C6, 0xD0B8,
	0x82C3, 0x29B0, 0x5A77, 0x1E11, 0x7BCB, 0xA8FC, 0x6DD6, 0x2C3A,
};

static inline u16 _S_(u16 v)
{
	u16 t = Sbox[Hi8(v)];
	return Sbox[Lo8(v)] ^ ((t << 8) | (t >> 8));
}

#define PHASE1_LOOP_COUNT 8

static void tkip_mixing_phase1(u16 * TTAK, const u8 * TK, const u8 * TA,
			       u32 IV32)
{
	int i, j;

	/* Initialize the 80-bit TTAK from TSC (IV32) and TA[0..5] */
	TTAK[0] = Lo16(IV32);
	TTAK[1] = Hi16(IV32);
	TTAK[2] = Mk16(TA[1], TA[0]);
	TTAK[3] = Mk16(TA[3], TA[2]);
	TTAK[4] = Mk16(TA[5], TA[4]);

	for (i = 0; i < PHASE1_LOOP_COUNT; i++) {
		j = 2 * (i & 1);
		TTAK[0] += _S_(TTAK[4] ^ Mk16(TK[1 + j], TK[0 + j]));
		TTAK[1] += _S_(TTAK[0] ^ Mk16(TK[5 + j], TK[4 + j]));
		TTAK[2] += _S_(TTAK[1] ^ Mk16(TK[9 + j], TK[8 + j]));
		TTAK[3] += _S_(TTAK[2] ^ Mk16(TK[13 + j], TK[12 + j]));
		TTAK[4] += _S_(TTAK[3] ^ Mk16(TK[1 + j], TK[0 + j])) + i;
	}
}

static void tkip_mixing_phase2(u8 * WEPSeed, const u8 * TK, const u16 * TTAK,
			       u16 IV16)
{
	/* Make temporary area overlap WEP seed so that the final copy can be
	 * avoided on little endian hosts. */
	u16 *PPK = (u16 *) & WEPSeed[4];

	/* Step 1 - make copy of TTAK and bring in TSC */
	PPK[0] = TTAK[0];
	PPK[1] = TTAK[1];
	PPK[2] = TTAK[2];
	PPK[3] = TTAK[3];
	PPK[4] = TTAK[4];
	PPK[5] = TTAK[4] + IV16;

	/* Step 2 - 96-bit bijective mixing using S-box */
	PPK[0] += _S_(PPK[5] ^ Mk16_le((__le16 *) & TK[0]));
	PPK[1] += _S_(PPK[0] ^ Mk16_le((__le16 *) & TK[2]));
	PPK[2] += _S_(PPK[1] ^ Mk16_le((__le16 *) & TK[4]));
	PPK[3] += _S_(PPK[2] ^ Mk16_le((__le16 *) & TK[6]));
	PPK[4] += _S_(PPK[3] ^ Mk16_le((__le16 *) & TK[8]));
	PPK[5] += _S_(PPK[4] ^ Mk16_le((__le16 *) & TK[10]));

	PPK[0] += RotR1(PPK[5] ^ Mk16_le((__le16 *) & TK[12]));
	PPK[1] += RotR1(PPK[0] ^ Mk16_le((__le16 *) & TK[14]));
	PPK[2] += RotR1(PPK[1]);
	PPK[3] += RotR1(PPK[2]);
	PPK[4] += RotR1(PPK[3]);
	PPK[5] += RotR1(PPK[4]);

	/* Step 3 - bring in last of TK bits, assign 24-bit WEP IV value
	 * WEPSeed[0..2] is transmitted as WEP IV */
	WEPSeed[0] = Hi8(IV16);
	WEPSeed[1] = (Hi8(IV16) | 0x20) & 0x7F;
	WEPSeed[2] = Lo8(IV16);
	WEPSeed[3] = Lo8((PPK[5] ^ Mk16_le((__le16 *) & TK[0])) >> 1);

#ifdef __BIG_ENDIAN
	{
		int i;
		for (i = 0; i < 6; i++)
			PPK[i] = (PPK[i] << 8) | (PPK[i] >> 8);
	}
#endif
}

static int lib80211_tkip_hdr(struct sk_buff *skb, int hdr_len,
			      u8 * rc4key, int keylen, void *priv)
{
	struct lib80211_tkip_data *tkey = priv;
	u8 *pos;
	struct ieee80211_hdr *hdr;

	hdr = (struct ieee80211_hdr *)skb->data;

	if (skb_headroom(skb) < TKIP_HDR_LEN || skb->len < hdr_len)
		return -1;

	if (rc4key == NULL || keylen < 16)
		return -1;

	if (!tkey->tx_phase1_done) {
		tkip_mixing_phase1(tkey->tx_ttak, tkey->key, hdr->addr2,
				   tkey->tx_iv32);
		tkey->tx_phase1_done = 1;
	}
	tkip_mixing_phase2(rc4key, tkey->key, tkey->tx_ttak, tkey->tx_iv16);

	pos = skb_push(skb, TKIP_HDR_LEN);
	memmove(pos, pos + TKIP_HDR_LEN, hdr_len);
	pos += hdr_len;

	*pos++ = *rc4key;
	*pos++ = *(rc4key + 1);
	*pos++ = *(rc4key + 2);
	*pos++ = (tkey->key_idx << 6) | (1 << 5) /* Ext IV included */ ;
	*pos++ = tkey->tx_iv32 & 0xff;
	*pos++ = (tkey->tx_iv32 >> 8) & 0xff;
	*pos++ = (tkey->tx_iv32 >> 16) & 0xff;
	*pos++ = (tkey->tx_iv32 >> 24) & 0xff;

	tkey->tx_iv16++;
	if (tkey->tx_iv16 == 0) {
		tkey->tx_phase1_done = 0;
		tkey->tx_iv32++;
	}

	return TKIP_HDR_LEN;
}

static int lib80211_tkip_encrypt(struct sk_buff *skb, int hdr_len, void *priv)
{
	struct lib80211_tkip_data *tkey = priv;
	int len;
	u8 rc4key[16], *pos, *icv;
	u32 crc;

	if (tkey->flags & IEEE80211_CRYPTO_TKIP_COUNTERMEASURES) {
		struct ieee80211_hdr *hdr = (struct ieee80211_hdr *)skb->data;
		net_dbg_ratelimited("TKIP countermeasures: dropped TX packet to %pM\n",
				    hdr->addr1);
		return -1;
	}

	if (skb_tailroom(skb) < 4 || skb->len < hdr_len)
		return -1;

	len = skb->len - hdr_len;
	pos = skb->data + hdr_len;

	if ((lib80211_tkip_hdr(skb, hdr_len, rc4key, 16, priv)) < 0)
		return -1;

	crc = ~crc32_le(~0, pos, len);
	icv = skb_put(skb, 4);
	icv[0] = crc;
	icv[1] = crc >> 8;
	icv[2] = crc >> 16;
	icv[3] = crc >> 24;

	arc4_setkey(&tkey->tx_ctx_arc4, rc4key, 16);
	arc4_crypt(&tkey->tx_ctx_arc4, pos, pos, len + 4);

	return 0;
}

/*
 * deal with seq counter wrapping correctly.
 * refer to timer_after() for jiffies wrapping handling
 */
static inline int tkip_replay_check(u32 iv32_n, u16 iv16_n,
				    u32 iv32_o, u16 iv16_o)
{
	if ((s32)iv32_n - (s32)iv32_o < 0 ||
	    (iv32_n == iv32_o && iv16_n <= iv16_o))
		return 1;
	return 0;
}

static int lib80211_tkip_decrypt(struct sk_buff *skb, int hdr_len, void *priv)
{
	struct lib80211_tkip_data *tkey = priv;
	u8 rc4key[16];
	u8 keyidx, *pos;
	u32 iv32;
	u16 iv16;
	struct ieee80211_hdr *hdr;
	u8 icv[4];
	u32 crc;
	int plen;

	hdr = (struct ieee80211_hdr *)skb->data;

	if (tkey->flags & IEEE80211_CRYPTO_TKIP_COUNTERMEASURES) {
		net_dbg_ratelimited("TKIP countermeasures: dropped received packet from %pM\n",
				    hdr->addr2);
		return -1;
	}

	if (skb->len < hdr_len + TKIP_HDR_LEN + 4)
		return -1;

	pos = skb->data + hdr_len;
	keyidx = pos[3];
	if (!(keyidx & (1 << 5))) {
		net_dbg_ratelimited("TKIP: received packet without ExtIV flag from %pM\n",
				    hdr->addr2);
		return -2;
	}
	keyidx >>= 6;
	if (tkey->key_idx != keyidx) {
		net_dbg_ratelimited("TKIP: RX tkey->key_idx=%d frame keyidx=%d\n",
				    tkey->key_idx, keyidx);
		return -6;
	}
	if (!tkey->key_set) {
		net_dbg_ratelimited("TKIP: received packet from %pM with keyid=%d that does not have a configured key\n",
				    hdr->addr2, keyidx);
		return -3;
	}
	iv16 = (pos[0] << 8) | pos[2];
	iv32 = pos[4] | (pos[5] << 8) | (pos[6] << 16) | (pos[7] << 24);
	pos += TKIP_HDR_LEN;

	if (tkip_replay_check(iv32, iv16, tkey->rx_iv32, tkey->rx_iv16)) {
#ifdef CONFIG_LIB80211_DEBUG
		net_dbg_ratelimited("TKIP: replay detected: STA=%pM previous TSC %08x%04x received TSC %08x%04x\n",
				    hdr->addr2, tkey->rx_iv32, tkey->rx_iv16,
				    iv32, iv16);
#endif
		tkey->dot11RSNAStatsTKIPReplays++;
		return -4;
	}

	if (iv32 != tkey->rx_iv32 || !tkey->rx_phase1_done) {
		tkip_mixing_phase1(tkey->rx_ttak, tkey->key, hdr->addr2, iv32);
		tkey->rx_phase1_done = 1;
	}
	tkip_mixing_phase2(rc4key, tkey->key, tkey->rx_ttak, iv16);

	plen = skb->len - hdr_len - 12;

	arc4_setkey(&tkey->rx_ctx_arc4, rc4key, 16);
	arc4_crypt(&tkey->rx_ctx_arc4, pos, pos, plen + 4);

	crc = ~crc32_le(~0, pos, plen);
	icv[0] = crc;
	icv[1] = crc >> 8;
	icv[2] = crc >> 16;
	icv[3] = crc >> 24;
	if (memcmp(icv, pos + plen, 4) != 0) {
		if (iv32 != tkey->rx_iv32) {
			/* Previously cached Phase1 result was already lost, so
			 * it needs to be recalculated for the next packet. */
			tkey->rx_phase1_done = 0;
		}
#ifdef CONFIG_LIB80211_DEBUG
		net_dbg_ratelimited("TKIP: ICV error detected: STA=%pM\n",
				    hdr->addr2);
#endif
		tkey->dot11RSNAStatsTKIPICVErrors++;
		return -5;
	}

	/* Update real counters only after Michael MIC verification has
	 * completed */
	tkey->rx_iv32_new = iv32;
	tkey->rx_iv16_new = iv16;

	/* Remove IV and ICV */
	memmove(skb->data + TKIP_HDR_LEN, skb->data, hdr_len);
	skb_pull(skb, TKIP_HDR_LEN);
	skb_trim(skb, skb->len - 4);

	return keyidx;
}

static int michael_mic(struct crypto_shash *tfm_michael, u8 *key, u8 *hdr,
		       u8 *data, size_t data_len, u8 *mic)
{
	SHASH_DESC_ON_STACK(desc, tfm_michael);
	int err;

	if (tfm_michael == NULL) {
		pr_warn("%s(): tfm_michael == NULL\n", __func__);
		return -1;
	}

	desc->tfm = tfm_michael;

	if (crypto_shash_setkey(tfm_michael, key, 8))
		return -1;

	err = crypto_shash_init(desc);
	if (err)
		goto out;
	err = crypto_shash_update(desc, hdr, 16);
	if (err)
		goto out;
	err = crypto_shash_update(desc, data, data_len);
	if (err)
		goto out;
	err = crypto_shash_final(desc, mic);

out:
	shash_desc_zero(desc);
	return err;
}

static void michael_mic_hdr(struct sk_buff *skb, u8 * hdr)
{
	struct ieee80211_hdr *hdr11;

	hdr11 = (struct ieee80211_hdr *)skb->data;

	switch (le16_to_cpu(hdr11->frame_control) &
		(IEEE80211_FCTL_FROMDS | IEEE80211_FCTL_TODS)) {
	case IEEE80211_FCTL_TODS:
		memcpy(hdr, hdr11->addr3, ETH_ALEN);	/* DA */
		memcpy(hdr + ETH_ALEN, hdr11->addr2, ETH_ALEN);	/* SA */
		break;
	case IEEE80211_FCTL_FROMDS:
		memcpy(hdr, hdr11->addr1, ETH_ALEN);	/* DA */
		memcpy(hdr + ETH_ALEN, hdr11->addr3, ETH_ALEN);	/* SA */
		break;
	case IEEE80211_FCTL_FROMDS | IEEE80211_FCTL_TODS:
		memcpy(hdr, hdr11->addr3, ETH_ALEN);	/* DA */
		memcpy(hdr + ETH_ALEN, hdr11->addr4, ETH_ALEN);	/* SA */
		break;
	default:
		memcpy(hdr, hdr11->addr1, ETH_ALEN);	/* DA */
		memcpy(hdr + ETH_ALEN, hdr11->addr2, ETH_ALEN);	/* SA */
		break;
	}

	if (ieee80211_is_data_qos(hdr11->frame_control)) {
		hdr[12] = le16_to_cpu(*((__le16 *)ieee80211_get_qos_ctl(hdr11)))
			& IEEE80211_QOS_CTL_TID_MASK;
	} else
		hdr[12] = 0;		/* priority */

	hdr[13] = hdr[14] = hdr[15] = 0;	/* reserved */
}

static int lib80211_michael_mic_add(struct sk_buff *skb, int hdr_len,
				     void *priv)
{
	struct lib80211_tkip_data *tkey = priv;
	u8 *pos;

	if (skb_tailroom(skb) < 8 || skb->len < hdr_len) {
		printk(KERN_DEBUG "Invalid packet for Michael MIC add "
		       "(tailroom=%d hdr_len=%d skb->len=%d)\n",
		       skb_tailroom(skb), hdr_len, skb->len);
		return -1;
	}

	michael_mic_hdr(skb, tkey->tx_hdr);
	pos = skb_put(skb, 8);
	if (michael_mic(tkey->tx_tfm_michael, &tkey->key[16], tkey->tx_hdr,
			skb->data + hdr_len, skb->len - 8 - hdr_len, pos))
		return -1;

	return 0;
}

static void lib80211_michael_mic_failure(struct net_device *dev,
					  struct ieee80211_hdr *hdr,
					  int keyidx)
{
	union iwreq_data wrqu;
	struct iw_michaelmicfailure ev;

	/* TODO: needed parameters: count, keyid, key type, TSC */
	memset(&ev, 0, sizeof(ev));
	ev.flags = keyidx & IW_MICFAILURE_KEY_ID;
	if (hdr->addr1[0] & 0x01)
		ev.flags |= IW_MICFAILURE_GROUP;
	else
		ev.flags |= IW_MICFAILURE_PAIRWISE;
	ev.src_addr.sa_family = ARPHRD_ETHER;
	memcpy(ev.src_addr.sa_data, hdr->addr2, ETH_ALEN);
	memset(&wrqu, 0, sizeof(wrqu));
	wrqu.data.length = sizeof(ev);
	wireless_send_event(dev, IWEVMICHAELMICFAILURE, &wrqu, (char *)&ev);
}

static int lib80211_michael_mic_verify(struct sk_buff *skb, int keyidx,
					int hdr_len, void *priv)
{
	struct lib80211_tkip_data *tkey = priv;
	u8 mic[8];

	if (!tkey->key_set)
		return -1;

	michael_mic_hdr(skb, tkey->rx_hdr);
	if (michael_mic(tkey->rx_tfm_michael, &tkey->key[24], tkey->rx_hdr,
			skb->data + hdr_len, skb->len - 8 - hdr_len, mic))
		return -1;
	if (memcmp(mic, skb->data + skb->len - 8, 8) != 0) {
		struct ieee80211_hdr *hdr;
		hdr = (struct ieee80211_hdr *)skb->data;
		printk(KERN_DEBUG "%s: Michael MIC verification failed for "
		       "MSDU from %pM keyidx=%d\n",
		       skb->dev ? skb->dev->name : "N/A", hdr->addr2,
		       keyidx);
		if (skb->dev)
			lib80211_michael_mic_failure(skb->dev, hdr, keyidx);
		tkey->dot11RSNAStatsTKIPLocalMICFailures++;
		return -1;
	}

	/* Update TSC counters for RX now that the packet verification has
	 * completed. */
	tkey->rx_iv32 = tkey->rx_iv32_new;
	tkey->rx_iv16 = tkey->rx_iv16_new;

	skb_trim(skb, skb->len - 8);

	return 0;
}

static int lib80211_tkip_set_key(void *key, int len, u8 * seq, void *priv)
{
	struct lib80211_tkip_data *tkey = priv;
	int keyidx;
	struct crypto_shash *tfm = tkey->tx_tfm_michael;
	struct arc4_ctx *tfm2 = &tkey->tx_ctx_arc4;
	struct crypto_shash *tfm3 = tkey->rx_tfm_michael;
	struct arc4_ctx *tfm4 = &tkey->rx_ctx_arc4;

	keyidx = tkey->key_idx;
	memset(tkey, 0, sizeof(*tkey));
	tkey->key_idx = keyidx;
	tkey->tx_tfm_michael = tfm;
	tkey->tx_ctx_arc4 = *tfm2;
	tkey->rx_tfm_michael = tfm3;
	tkey->rx_ctx_arc4 = *tfm4;
	if (len == TKIP_KEY_LEN) {
		memcpy(tkey->key, key, TKIP_KEY_LEN);
		tkey->key_set = 1;
		tkey->tx_iv16 = 1;	/* TSC is initialized to 1 */
		if (seq) {
			tkey->rx_iv32 = (seq[5] << 24) | (seq[4] << 16) |
			    (seq[3] << 8) | seq[2];
			tkey->rx_iv16 = (seq[1] << 8) | seq[0];
		}
	} else if (len == 0)
		tkey->key_set = 0;
	else
		return -1;

	return 0;
}

static int lib80211_tkip_get_key(void *key, int len, u8 * seq, void *priv)
{
	struct lib80211_tkip_data *tkey = priv;

	if (len < TKIP_KEY_LEN)
		return -1;

	if (!tkey->key_set)
		return 0;
	memcpy(key, tkey->key, TKIP_KEY_LEN);

	if (seq) {
		/* Return the sequence number of the last transmitted frame. */
		u16 iv16 = tkey->tx_iv16;
		u32 iv32 = tkey->tx_iv32;
		if (iv16 == 0)
			iv32--;
		iv16--;
		seq[0] = tkey->tx_iv16;
		seq[1] = tkey->tx_iv16 >> 8;
		seq[2] = tkey->tx_iv32;
		seq[3] = tkey->tx_iv32 >> 8;
		seq[4] = tkey->tx_iv32 >> 16;
		seq[5] = tkey->tx_iv32 >> 24;
	}

	return TKIP_KEY_LEN;
}

static void lib80211_tkip_print_stats(struct seq_file *m, void *priv)
{
	struct lib80211_tkip_data *tkip = priv;
	seq_printf(m,
		   "key[%d] alg=TKIP key_set=%d "
		   "tx_pn=%02x%02x%02x%02x%02x%02x "
		   "rx_pn=%02x%02x%02x%02x%02x%02x "
		   "replays=%d icv_errors=%d local_mic_failures=%d\n",
		   tkip->key_idx, tkip->key_set,
		   (tkip->tx_iv32 >> 24) & 0xff,
		   (tkip->tx_iv32 >> 16) & 0xff,
		   (tkip->tx_iv32 >> 8) & 0xff,
		   tkip->tx_iv32 & 0xff,
		   (tkip->tx_iv16 >> 8) & 0xff,
		   tkip->tx_iv16 & 0xff,
		   (tkip->rx_iv32 >> 24) & 0xff,
		   (tkip->rx_iv32 >> 16) & 0xff,
		   (tkip->rx_iv32 >> 8) & 0xff,
		   tkip->rx_iv32 & 0xff,
		   (tkip->rx_iv16 >> 8) & 0xff,
		   tkip->rx_iv16 & 0xff,
		   tkip->dot11RSNAStatsTKIPReplays,
		   tkip->dot11RSNAStatsTKIPICVErrors,
		   tkip->dot11RSNAStatsTKIPLocalMICFailures);
}

static struct lib80211_crypto_ops lib80211_crypt_tkip = {
	.name = "TKIP",
	.init = lib80211_tkip_init,
	.deinit = lib80211_tkip_deinit,
	.encrypt_mpdu = lib80211_tkip_encrypt,
	.decrypt_mpdu = lib80211_tkip_decrypt,
	.encrypt_msdu = lib80211_michael_mic_add,
	.decrypt_msdu = lib80211_michael_mic_verify,
	.set_key = lib80211_tkip_set_key,
	.get_key = lib80211_tkip_get_key,
	.print_stats = lib80211_tkip_print_stats,
	.extra_mpdu_prefix_len = 4 + 4,	/* IV + ExtIV */
	.extra_mpdu_postfix_len = 4,	/* ICV */
	.extra_msdu_postfix_len = 8,	/* MIC */
	.get_flags = lib80211_tkip_get_flags,
	.set_flags = lib80211_tkip_set_flags,
	.owner = THIS_MODULE,
};

static int __init lib80211_crypto_tkip_init(void)
{
	return lib80211_register_crypto_ops(&lib80211_crypt_tkip);
}

static void __exit lib80211_crypto_tkip_exit(void)
{
	lib80211_unregister_crypto_ops(&lib80211_crypt_tkip);
}

module_init(lib80211_crypto_tkip_init);
module_exit(lib80211_crypto_tkip_exit);
