// SPDX-License-Identifier: GPL-2.0-only
/*
 * lib80211 crypt: host-based CCMP encryption implementation for lib80211
 *
 * Copyright (c) 2003-2004, Jouni Malinen <j@w1.fi>
 * Copyright (c) 2008, John W. Linville <linville@tuxdriver.com>
 */

#include <linux/kernel.h>
#include <linux/err.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/random.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/if_ether.h>
#include <linux/if_arp.h>
#include <asm/string.h>
#include <linux/wireless.h>

#include <linux/ieee80211.h>

#include <linux/crypto.h>
#include <crypto/aead.h>

#include <net/lib80211.h>

MODULE_AUTHOR("Jouni Malinen");
MODULE_DESCRIPTION("Host AP crypt: CCMP");
MODULE_LICENSE("GPL");

#define AES_BLOCK_LEN 16
#define CCMP_HDR_LEN 8
#define CCMP_MIC_LEN 8
#define CCMP_TK_LEN 16
#define CCMP_PN_LEN 6

struct lib80211_ccmp_data {
	u8 key[CCMP_TK_LEN];
	int key_set;

	u8 tx_pn[CCMP_PN_LEN];
	u8 rx_pn[CCMP_PN_LEN];

	u32 dot11RSNAStatsCCMPFormatErrors;
	u32 dot11RSNAStatsCCMPReplays;
	u32 dot11RSNAStatsCCMPDecryptErrors;

	int key_idx;

	struct crypto_aead *tfm;

	/* scratch buffers for virt_to_page() (crypto API) */
	u8 tx_aad[2 * AES_BLOCK_LEN];
	u8 rx_aad[2 * AES_BLOCK_LEN];
};

static void *lib80211_ccmp_init(int key_idx)
{
	struct lib80211_ccmp_data *priv;

	priv = kzalloc(sizeof(*priv), GFP_ATOMIC);
	if (priv == NULL)
		goto fail;
	priv->key_idx = key_idx;

	priv->tfm = crypto_alloc_aead("ccm(aes)", 0, CRYPTO_ALG_ASYNC);
	if (IS_ERR(priv->tfm)) {
		priv->tfm = NULL;
		goto fail;
	}

	return priv;

      fail:
	if (priv) {
		if (priv->tfm)
			crypto_free_aead(priv->tfm);
		kfree(priv);
	}

	return NULL;
}

static void lib80211_ccmp_deinit(void *priv)
{
	struct lib80211_ccmp_data *_priv = priv;
	if (_priv && _priv->tfm)
		crypto_free_aead(_priv->tfm);
	kfree(priv);
}

static int ccmp_init_iv_and_aad(const struct ieee80211_hdr *hdr,
				const u8 *pn, u8 *iv, u8 *aad)
{
	u8 *pos, qc = 0;
	size_t aad_len;
	int a4_included, qc_included;

	a4_included = ieee80211_has_a4(hdr->frame_control);
	qc_included = ieee80211_is_data_qos(hdr->frame_control);

	aad_len = 22;
	if (a4_included)
		aad_len += 6;
	if (qc_included) {
		pos = (u8 *) & hdr->addr4;
		if (a4_included)
			pos += 6;
		qc = *pos & 0x0f;
		aad_len += 2;
	}

	/* In CCM, the initial vectors (IV) used for CTR mode encryption and CBC
	 * mode authentication are not allowed to collide, yet both are derived
	 * from the same vector. We only set L := 1 here to indicate that the
	 * data size can be represented in (L+1) bytes. The CCM layer will take
	 * care of storing the data length in the top (L+1) bytes and setting
	 * and clearing the other bits as is required to derive the two IVs.
	 */
	iv[0] = 0x1;

	/* Nonce: QC | A2 | PN */
	iv[1] = qc;
	memcpy(iv + 2, hdr->addr2, ETH_ALEN);
	memcpy(iv + 8, pn, CCMP_PN_LEN);

	/* AAD:
	 * FC with bits 4..6 and 11..13 masked to zero; 14 is always one
	 * A1 | A2 | A3
	 * SC with bits 4..15 (seq#) masked to zero
	 * A4 (if present)
	 * QC (if present)
	 */
	pos = (u8 *) hdr;
	aad[0] = pos[0] & 0x8f;
	aad[1] = pos[1] & 0xc7;
	memcpy(aad + 2, hdr->addr1, 3 * ETH_ALEN);
	pos = (u8 *) & hdr->seq_ctrl;
	aad[20] = pos[0] & 0x0f;
	aad[21] = 0;		/* all bits masked */
	memset(aad + 22, 0, 8);
	if (a4_included)
		memcpy(aad + 22, hdr->addr4, ETH_ALEN);
	if (qc_included) {
		aad[a4_included ? 28 : 22] = qc;
		/* rest of QC masked */
	}
	return aad_len;
}

static int lib80211_ccmp_hdr(struct sk_buff *skb, int hdr_len,
			      u8 *aeskey, int keylen, void *priv)
{
	struct lib80211_ccmp_data *key = priv;
	int i;
	u8 *pos;

	if (skb_headroom(skb) < CCMP_HDR_LEN || skb->len < hdr_len)
		return -1;

	if (aeskey != NULL && keylen >= CCMP_TK_LEN)
		memcpy(aeskey, key->key, CCMP_TK_LEN);

	pos = skb_push(skb, CCMP_HDR_LEN);
	memmove(pos, pos + CCMP_HDR_LEN, hdr_len);
	pos += hdr_len;

	i = CCMP_PN_LEN - 1;
	while (i >= 0) {
		key->tx_pn[i]++;
		if (key->tx_pn[i] != 0)
			break;
		i--;
	}

	*pos++ = key->tx_pn[5];
	*pos++ = key->tx_pn[4];
	*pos++ = 0;
	*pos++ = (key->key_idx << 6) | (1 << 5) /* Ext IV included */ ;
	*pos++ = key->tx_pn[3];
	*pos++ = key->tx_pn[2];
	*pos++ = key->tx_pn[1];
	*pos++ = key->tx_pn[0];

	return CCMP_HDR_LEN;
}

static int lib80211_ccmp_encrypt(struct sk_buff *skb, int hdr_len, void *priv)
{
	struct lib80211_ccmp_data *key = priv;
	struct ieee80211_hdr *hdr;
	struct aead_request *req;
	struct scatterlist sg[2];
	u8 *aad = key->tx_aad;
	u8 iv[AES_BLOCK_LEN];
	int len, data_len, aad_len;
	int ret;

	if (skb_tailroom(skb) < CCMP_MIC_LEN || skb->len < hdr_len)
		return -1;

	data_len = skb->len - hdr_len;
	len = lib80211_ccmp_hdr(skb, hdr_len, NULL, 0, priv);
	if (len < 0)
		return -1;

	req = aead_request_alloc(key->tfm, GFP_ATOMIC);
	if (!req)
		return -ENOMEM;

	hdr = (struct ieee80211_hdr *)skb->data;
	aad_len = ccmp_init_iv_and_aad(hdr, key->tx_pn, iv, aad);

	skb_put(skb, CCMP_MIC_LEN);

	sg_init_table(sg, 2);
	sg_set_buf(&sg[0], aad, aad_len);
	sg_set_buf(&sg[1], skb->data + hdr_len + CCMP_HDR_LEN,
		   data_len + CCMP_MIC_LEN);

	aead_request_set_callback(req, 0, NULL, NULL);
	aead_request_set_ad(req, aad_len);
	aead_request_set_crypt(req, sg, sg, data_len, iv);

	ret = crypto_aead_encrypt(req);
	aead_request_free(req);

	return ret;
}

/*
 * deal with seq counter wrapping correctly.
 * refer to timer_after() for jiffies wrapping handling
 */
static inline int ccmp_replay_check(u8 *pn_n, u8 *pn_o)
{
	u32 iv32_n, iv16_n;
	u32 iv32_o, iv16_o;

	iv32_n = (pn_n[0] << 24) | (pn_n[1] << 16) | (pn_n[2] << 8) | pn_n[3];
	iv16_n = (pn_n[4] << 8) | pn_n[5];

	iv32_o = (pn_o[0] << 24) | (pn_o[1] << 16) | (pn_o[2] << 8) | pn_o[3];
	iv16_o = (pn_o[4] << 8) | pn_o[5];

	if ((s32)iv32_n - (s32)iv32_o < 0 ||
	    (iv32_n == iv32_o && iv16_n <= iv16_o))
		return 1;
	return 0;
}

static int lib80211_ccmp_decrypt(struct sk_buff *skb, int hdr_len, void *priv)
{
	struct lib80211_ccmp_data *key = priv;
	u8 keyidx, *pos;
	struct ieee80211_hdr *hdr;
	struct aead_request *req;
	struct scatterlist sg[2];
	u8 *aad = key->rx_aad;
	u8 iv[AES_BLOCK_LEN];
	u8 pn[6];
	int aad_len, ret;
	size_t data_len = skb->len - hdr_len - CCMP_HDR_LEN;

	if (skb->len < hdr_len + CCMP_HDR_LEN + CCMP_MIC_LEN) {
		key->dot11RSNAStatsCCMPFormatErrors++;
		return -1;
	}

	hdr = (struct ieee80211_hdr *)skb->data;
	pos = skb->data + hdr_len;
	keyidx = pos[3];
	if (!(keyidx & (1 << 5))) {
		net_dbg_ratelimited("CCMP: received packet without ExtIV flag from %pM\n",
				    hdr->addr2);
		key->dot11RSNAStatsCCMPFormatErrors++;
		return -2;
	}
	keyidx >>= 6;
	if (key->key_idx != keyidx) {
		net_dbg_ratelimited("CCMP: RX tkey->key_idx=%d frame keyidx=%d\n",
				    key->key_idx, keyidx);
		return -6;
	}
	if (!key->key_set) {
		net_dbg_ratelimited("CCMP: received packet from %pM with keyid=%d that does not have a configured key\n",
				    hdr->addr2, keyidx);
		return -3;
	}

	pn[0] = pos[7];
	pn[1] = pos[6];
	pn[2] = pos[5];
	pn[3] = pos[4];
	pn[4] = pos[1];
	pn[5] = pos[0];
	pos += 8;

	if (ccmp_replay_check(pn, key->rx_pn)) {
#ifdef CONFIG_LIB80211_DEBUG
		net_dbg_ratelimited("CCMP: replay detected: STA=%pM previous PN %02x%02x%02x%02x%02x%02x received PN %02x%02x%02x%02x%02x%02x\n",
				    hdr->addr2,
				    key->rx_pn[0], key->rx_pn[1], key->rx_pn[2],
				    key->rx_pn[3], key->rx_pn[4], key->rx_pn[5],
				    pn[0], pn[1], pn[2], pn[3], pn[4], pn[5]);
#endif
		key->dot11RSNAStatsCCMPReplays++;
		return -4;
	}

	req = aead_request_alloc(key->tfm, GFP_ATOMIC);
	if (!req)
		return -ENOMEM;

	aad_len = ccmp_init_iv_and_aad(hdr, pn, iv, aad);

	sg_init_table(sg, 2);
	sg_set_buf(&sg[0], aad, aad_len);
	sg_set_buf(&sg[1], pos, data_len);

	aead_request_set_callback(req, 0, NULL, NULL);
	aead_request_set_ad(req, aad_len);
	aead_request_set_crypt(req, sg, sg, data_len, iv);

	ret = crypto_aead_decrypt(req);
	aead_request_free(req);

	if (ret) {
		net_dbg_ratelimited("CCMP: decrypt failed: STA=%pM (%d)\n",
				    hdr->addr2, ret);
		key->dot11RSNAStatsCCMPDecryptErrors++;
		return -5;
	}

	memcpy(key->rx_pn, pn, CCMP_PN_LEN);

	/* Remove hdr and MIC */
	memmove(skb->data + CCMP_HDR_LEN, skb->data, hdr_len);
	skb_pull(skb, CCMP_HDR_LEN);
	skb_trim(skb, skb->len - CCMP_MIC_LEN);

	return keyidx;
}

static int lib80211_ccmp_set_key(void *key, int len, u8 * seq, void *priv)
{
	struct lib80211_ccmp_data *data = priv;
	int keyidx;
	struct crypto_aead *tfm = data->tfm;

	keyidx = data->key_idx;
	memset(data, 0, sizeof(*data));
	data->key_idx = keyidx;
	data->tfm = tfm;
	if (len == CCMP_TK_LEN) {
		memcpy(data->key, key, CCMP_TK_LEN);
		data->key_set = 1;
		if (seq) {
			data->rx_pn[0] = seq[5];
			data->rx_pn[1] = seq[4];
			data->rx_pn[2] = seq[3];
			data->rx_pn[3] = seq[2];
			data->rx_pn[4] = seq[1];
			data->rx_pn[5] = seq[0];
		}
		if (crypto_aead_setauthsize(data->tfm, CCMP_MIC_LEN) ||
		    crypto_aead_setkey(data->tfm, data->key, CCMP_TK_LEN))
			return -1;
	} else if (len == 0)
		data->key_set = 0;
	else
		return -1;

	return 0;
}

static int lib80211_ccmp_get_key(void *key, int len, u8 * seq, void *priv)
{
	struct lib80211_ccmp_data *data = priv;

	if (len < CCMP_TK_LEN)
		return -1;

	if (!data->key_set)
		return 0;
	memcpy(key, data->key, CCMP_TK_LEN);

	if (seq) {
		seq[0] = data->tx_pn[5];
		seq[1] = data->tx_pn[4];
		seq[2] = data->tx_pn[3];
		seq[3] = data->tx_pn[2];
		seq[4] = data->tx_pn[1];
		seq[5] = data->tx_pn[0];
	}

	return CCMP_TK_LEN;
}

static void lib80211_ccmp_print_stats(struct seq_file *m, void *priv)
{
	struct lib80211_ccmp_data *ccmp = priv;

	seq_printf(m,
		   "key[%d] alg=CCMP key_set=%d "
		   "tx_pn=%02x%02x%02x%02x%02x%02x "
		   "rx_pn=%02x%02x%02x%02x%02x%02x "
		   "format_errors=%d replays=%d decrypt_errors=%d\n",
		   ccmp->key_idx, ccmp->key_set,
		   ccmp->tx_pn[0], ccmp->tx_pn[1], ccmp->tx_pn[2],
		   ccmp->tx_pn[3], ccmp->tx_pn[4], ccmp->tx_pn[5],
		   ccmp->rx_pn[0], ccmp->rx_pn[1], ccmp->rx_pn[2],
		   ccmp->rx_pn[3], ccmp->rx_pn[4], ccmp->rx_pn[5],
		   ccmp->dot11RSNAStatsCCMPFormatErrors,
		   ccmp->dot11RSNAStatsCCMPReplays,
		   ccmp->dot11RSNAStatsCCMPDecryptErrors);
}

static struct lib80211_crypto_ops lib80211_crypt_ccmp = {
	.name = "CCMP",
	.init = lib80211_ccmp_init,
	.deinit = lib80211_ccmp_deinit,
	.encrypt_mpdu = lib80211_ccmp_encrypt,
	.decrypt_mpdu = lib80211_ccmp_decrypt,
	.encrypt_msdu = NULL,
	.decrypt_msdu = NULL,
	.set_key = lib80211_ccmp_set_key,
	.get_key = lib80211_ccmp_get_key,
	.print_stats = lib80211_ccmp_print_stats,
	.extra_mpdu_prefix_len = CCMP_HDR_LEN,
	.extra_mpdu_postfix_len = CCMP_MIC_LEN,
	.owner = THIS_MODULE,
};

static int __init lib80211_crypto_ccmp_init(void)
{
	return lib80211_register_crypto_ops(&lib80211_crypt_ccmp);
}

static void __exit lib80211_crypto_ccmp_exit(void)
{
	lib80211_unregister_crypto_ops(&lib80211_crypt_ccmp);
}

module_init(lib80211_crypto_ccmp_init);
module_exit(lib80211_crypto_ccmp_exit);
