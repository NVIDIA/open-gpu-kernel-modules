// SPDX-License-Identifier: ISC

#include <linux/etherdevice.h>
#include <linux/timekeeping.h>
#include "mt7603.h"
#include "mac.h"
#include "../trace.h"

#define MT_PSE_PAGE_SIZE	128

static u32
mt7603_ac_queue_mask0(u32 mask)
{
	u32 ret = 0;

	ret |= GENMASK(3, 0) * !!(mask & BIT(0));
	ret |= GENMASK(8, 5) * !!(mask & BIT(1));
	ret |= GENMASK(13, 10) * !!(mask & BIT(2));
	ret |= GENMASK(19, 16) * !!(mask & BIT(3));
	return ret;
}

static void
mt76_stop_tx_ac(struct mt7603_dev *dev, u32 mask)
{
	mt76_set(dev, MT_WF_ARB_TX_STOP_0, mt7603_ac_queue_mask0(mask));
}

static void
mt76_start_tx_ac(struct mt7603_dev *dev, u32 mask)
{
	mt76_set(dev, MT_WF_ARB_TX_START_0, mt7603_ac_queue_mask0(mask));
}

void mt7603_mac_reset_counters(struct mt7603_dev *dev)
{
	int i;

	for (i = 0; i < 2; i++)
		mt76_rr(dev, MT_TX_AGG_CNT(i));

	memset(dev->mt76.aggr_stats, 0, sizeof(dev->mt76.aggr_stats));
}

void mt7603_mac_set_timing(struct mt7603_dev *dev)
{
	u32 cck = FIELD_PREP(MT_TIMEOUT_VAL_PLCP, 231) |
		  FIELD_PREP(MT_TIMEOUT_VAL_CCA, 48);
	u32 ofdm = FIELD_PREP(MT_TIMEOUT_VAL_PLCP, 60) |
		   FIELD_PREP(MT_TIMEOUT_VAL_CCA, 24);
	int offset = 3 * dev->coverage_class;
	u32 reg_offset = FIELD_PREP(MT_TIMEOUT_VAL_PLCP, offset) |
			 FIELD_PREP(MT_TIMEOUT_VAL_CCA, offset);
	bool is_5ghz = dev->mphy.chandef.chan->band == NL80211_BAND_5GHZ;
	int sifs;
	u32 val;

	if (is_5ghz)
		sifs = 16;
	else
		sifs = 10;

	mt76_set(dev, MT_ARB_SCR,
		 MT_ARB_SCR_TX_DISABLE | MT_ARB_SCR_RX_DISABLE);
	udelay(1);

	mt76_wr(dev, MT_TIMEOUT_CCK, cck + reg_offset);
	mt76_wr(dev, MT_TIMEOUT_OFDM, ofdm + reg_offset);
	mt76_wr(dev, MT_IFS,
		FIELD_PREP(MT_IFS_EIFS, 360) |
		FIELD_PREP(MT_IFS_RIFS, 2) |
		FIELD_PREP(MT_IFS_SIFS, sifs) |
		FIELD_PREP(MT_IFS_SLOT, dev->slottime));

	if (dev->slottime < 20 || is_5ghz)
		val = MT7603_CFEND_RATE_DEFAULT;
	else
		val = MT7603_CFEND_RATE_11B;

	mt76_rmw_field(dev, MT_AGG_CONTROL, MT_AGG_CONTROL_CFEND_RATE, val);

	mt76_clear(dev, MT_ARB_SCR,
		   MT_ARB_SCR_TX_DISABLE | MT_ARB_SCR_RX_DISABLE);
}

static void
mt7603_wtbl_update(struct mt7603_dev *dev, int idx, u32 mask)
{
	mt76_rmw(dev, MT_WTBL_UPDATE, MT_WTBL_UPDATE_WLAN_IDX,
		 FIELD_PREP(MT_WTBL_UPDATE_WLAN_IDX, idx) | mask);

	mt76_poll(dev, MT_WTBL_UPDATE, MT_WTBL_UPDATE_BUSY, 0, 5000);
}

static u32
mt7603_wtbl1_addr(int idx)
{
	return MT_WTBL1_BASE + idx * MT_WTBL1_SIZE;
}

static u32
mt7603_wtbl2_addr(int idx)
{
	/* Mapped to WTBL2 */
	return MT_PCIE_REMAP_BASE_1 + idx * MT_WTBL2_SIZE;
}

static u32
mt7603_wtbl3_addr(int idx)
{
	u32 base = mt7603_wtbl2_addr(MT7603_WTBL_SIZE);

	return base + idx * MT_WTBL3_SIZE;
}

static u32
mt7603_wtbl4_addr(int idx)
{
	u32 base = mt7603_wtbl3_addr(MT7603_WTBL_SIZE);

	return base + idx * MT_WTBL4_SIZE;
}

void mt7603_wtbl_init(struct mt7603_dev *dev, int idx, int vif,
		      const u8 *mac_addr)
{
	const void *_mac = mac_addr;
	u32 addr = mt7603_wtbl1_addr(idx);
	u32 w0 = 0, w1 = 0;
	int i;

	if (_mac) {
		w0 = FIELD_PREP(MT_WTBL1_W0_ADDR_HI,
				get_unaligned_le16(_mac + 4));
		w1 = FIELD_PREP(MT_WTBL1_W1_ADDR_LO,
				get_unaligned_le32(_mac));
	}

	if (vif < 0)
		vif = 0;
	else
		w0 |= MT_WTBL1_W0_RX_CHECK_A1;
	w0 |= FIELD_PREP(MT_WTBL1_W0_MUAR_IDX, vif);

	mt76_poll(dev, MT_WTBL_UPDATE, MT_WTBL_UPDATE_BUSY, 0, 5000);

	mt76_set(dev, addr + 0 * 4, w0);
	mt76_set(dev, addr + 1 * 4, w1);
	mt76_set(dev, addr + 2 * 4, MT_WTBL1_W2_ADMISSION_CONTROL);

	mt76_stop_tx_ac(dev, GENMASK(3, 0));
	addr = mt7603_wtbl2_addr(idx);
	for (i = 0; i < MT_WTBL2_SIZE; i += 4)
		mt76_wr(dev, addr + i, 0);
	mt7603_wtbl_update(dev, idx, MT_WTBL_UPDATE_WTBL2);
	mt76_start_tx_ac(dev, GENMASK(3, 0));

	addr = mt7603_wtbl3_addr(idx);
	for (i = 0; i < MT_WTBL3_SIZE; i += 4)
		mt76_wr(dev, addr + i, 0);

	addr = mt7603_wtbl4_addr(idx);
	for (i = 0; i < MT_WTBL4_SIZE; i += 4)
		mt76_wr(dev, addr + i, 0);

	mt7603_wtbl_update(dev, idx, MT_WTBL_UPDATE_ADM_COUNT_CLEAR);
}

static void
mt7603_wtbl_set_skip_tx(struct mt7603_dev *dev, int idx, bool enabled)
{
	u32 addr = mt7603_wtbl1_addr(idx);
	u32 val = mt76_rr(dev, addr + 3 * 4);

	val &= ~MT_WTBL1_W3_SKIP_TX;
	val |= enabled * MT_WTBL1_W3_SKIP_TX;

	mt76_wr(dev, addr + 3 * 4, val);
}

void mt7603_filter_tx(struct mt7603_dev *dev, int idx, bool abort)
{
	int i, port, queue;

	if (abort) {
		port = 3; /* PSE */
		queue = 8; /* free queue */
	} else {
		port = 0; /* HIF */
		queue = 1; /* MCU queue */
	}

	mt7603_wtbl_set_skip_tx(dev, idx, true);

	mt76_wr(dev, MT_TX_ABORT, MT_TX_ABORT_EN |
			FIELD_PREP(MT_TX_ABORT_WCID, idx));

	for (i = 0; i < 4; i++) {
		mt76_wr(dev, MT_DMA_FQCR0, MT_DMA_FQCR0_BUSY |
			FIELD_PREP(MT_DMA_FQCR0_TARGET_WCID, idx) |
			FIELD_PREP(MT_DMA_FQCR0_TARGET_QID, i) |
			FIELD_PREP(MT_DMA_FQCR0_DEST_PORT_ID, port) |
			FIELD_PREP(MT_DMA_FQCR0_DEST_QUEUE_ID, queue));

		WARN_ON_ONCE(!mt76_poll(dev, MT_DMA_FQCR0, MT_DMA_FQCR0_BUSY,
					0, 5000));
	}

	mt76_wr(dev, MT_TX_ABORT, 0);

	mt7603_wtbl_set_skip_tx(dev, idx, false);
}

void mt7603_wtbl_set_smps(struct mt7603_dev *dev, struct mt7603_sta *sta,
			  bool enabled)
{
	u32 addr = mt7603_wtbl1_addr(sta->wcid.idx);

	if (sta->smps == enabled)
		return;

	mt76_rmw_field(dev, addr + 2 * 4, MT_WTBL1_W2_SMPS, enabled);
	sta->smps = enabled;
}

void mt7603_wtbl_set_ps(struct mt7603_dev *dev, struct mt7603_sta *sta,
			bool enabled)
{
	int idx = sta->wcid.idx;
	u32 addr;

	spin_lock_bh(&dev->ps_lock);

	if (sta->ps == enabled)
		goto out;

	mt76_wr(dev, MT_PSE_RTA,
		FIELD_PREP(MT_PSE_RTA_TAG_ID, idx) |
		FIELD_PREP(MT_PSE_RTA_PORT_ID, 0) |
		FIELD_PREP(MT_PSE_RTA_QUEUE_ID, 1) |
		FIELD_PREP(MT_PSE_RTA_REDIRECT_EN, enabled) |
		MT_PSE_RTA_WRITE | MT_PSE_RTA_BUSY);

	mt76_poll(dev, MT_PSE_RTA, MT_PSE_RTA_BUSY, 0, 5000);

	if (enabled)
		mt7603_filter_tx(dev, idx, false);

	addr = mt7603_wtbl1_addr(idx);
	mt76_set(dev, MT_WTBL1_OR, MT_WTBL1_OR_PSM_WRITE);
	mt76_rmw(dev, addr + 3 * 4, MT_WTBL1_W3_POWER_SAVE,
		 enabled * MT_WTBL1_W3_POWER_SAVE);
	mt76_clear(dev, MT_WTBL1_OR, MT_WTBL1_OR_PSM_WRITE);
	sta->ps = enabled;

out:
	spin_unlock_bh(&dev->ps_lock);
}

void mt7603_wtbl_clear(struct mt7603_dev *dev, int idx)
{
	int wtbl2_frame_size = MT_PSE_PAGE_SIZE / MT_WTBL2_SIZE;
	int wtbl2_frame = idx / wtbl2_frame_size;
	int wtbl2_entry = idx % wtbl2_frame_size;

	int wtbl3_base_frame = MT_WTBL3_OFFSET / MT_PSE_PAGE_SIZE;
	int wtbl3_frame_size = MT_PSE_PAGE_SIZE / MT_WTBL3_SIZE;
	int wtbl3_frame = wtbl3_base_frame + idx / wtbl3_frame_size;
	int wtbl3_entry = (idx % wtbl3_frame_size) * 2;

	int wtbl4_base_frame = MT_WTBL4_OFFSET / MT_PSE_PAGE_SIZE;
	int wtbl4_frame_size = MT_PSE_PAGE_SIZE / MT_WTBL4_SIZE;
	int wtbl4_frame = wtbl4_base_frame + idx / wtbl4_frame_size;
	int wtbl4_entry = idx % wtbl4_frame_size;

	u32 addr = MT_WTBL1_BASE + idx * MT_WTBL1_SIZE;
	int i;

	mt76_poll(dev, MT_WTBL_UPDATE, MT_WTBL_UPDATE_BUSY, 0, 5000);

	mt76_wr(dev, addr + 0 * 4,
		MT_WTBL1_W0_RX_CHECK_A1 |
		MT_WTBL1_W0_RX_CHECK_A2 |
		MT_WTBL1_W0_RX_VALID);
	mt76_wr(dev, addr + 1 * 4, 0);
	mt76_wr(dev, addr + 2 * 4, 0);

	mt76_set(dev, MT_WTBL1_OR, MT_WTBL1_OR_PSM_WRITE);

	mt76_wr(dev, addr + 3 * 4,
		FIELD_PREP(MT_WTBL1_W3_WTBL2_FRAME_ID, wtbl2_frame) |
		FIELD_PREP(MT_WTBL1_W3_WTBL2_ENTRY_ID, wtbl2_entry) |
		FIELD_PREP(MT_WTBL1_W3_WTBL4_FRAME_ID, wtbl4_frame) |
		MT_WTBL1_W3_I_PSM | MT_WTBL1_W3_KEEP_I_PSM);
	mt76_wr(dev, addr + 4 * 4,
		FIELD_PREP(MT_WTBL1_W4_WTBL3_FRAME_ID, wtbl3_frame) |
		FIELD_PREP(MT_WTBL1_W4_WTBL3_ENTRY_ID, wtbl3_entry) |
		FIELD_PREP(MT_WTBL1_W4_WTBL4_ENTRY_ID, wtbl4_entry));

	mt76_clear(dev, MT_WTBL1_OR, MT_WTBL1_OR_PSM_WRITE);

	addr = mt7603_wtbl2_addr(idx);

	/* Clear BA information */
	mt76_wr(dev, addr + (15 * 4), 0);

	mt76_stop_tx_ac(dev, GENMASK(3, 0));
	for (i = 2; i <= 4; i++)
		mt76_wr(dev, addr + (i * 4), 0);
	mt7603_wtbl_update(dev, idx, MT_WTBL_UPDATE_WTBL2);
	mt76_start_tx_ac(dev, GENMASK(3, 0));

	mt7603_wtbl_update(dev, idx, MT_WTBL_UPDATE_RX_COUNT_CLEAR);
	mt7603_wtbl_update(dev, idx, MT_WTBL_UPDATE_TX_COUNT_CLEAR);
	mt7603_wtbl_update(dev, idx, MT_WTBL_UPDATE_ADM_COUNT_CLEAR);
}

void mt7603_wtbl_update_cap(struct mt7603_dev *dev, struct ieee80211_sta *sta)
{
	struct mt7603_sta *msta = (struct mt7603_sta *)sta->drv_priv;
	int idx = msta->wcid.idx;
	u8 ampdu_density;
	u32 addr;
	u32 val;

	addr = mt7603_wtbl1_addr(idx);

	ampdu_density = sta->ht_cap.ampdu_density;
	if (ampdu_density < IEEE80211_HT_MPDU_DENSITY_4)
		ampdu_density = IEEE80211_HT_MPDU_DENSITY_4;

	val = mt76_rr(dev, addr + 2 * 4);
	val &= MT_WTBL1_W2_KEY_TYPE | MT_WTBL1_W2_ADMISSION_CONTROL;
	val |= FIELD_PREP(MT_WTBL1_W2_AMPDU_FACTOR, sta->ht_cap.ampdu_factor) |
	       FIELD_PREP(MT_WTBL1_W2_MPDU_DENSITY, sta->ht_cap.ampdu_density) |
	       MT_WTBL1_W2_TXS_BAF_REPORT;

	if (sta->ht_cap.cap)
		val |= MT_WTBL1_W2_HT;
	if (sta->vht_cap.cap)
		val |= MT_WTBL1_W2_VHT;

	mt76_wr(dev, addr + 2 * 4, val);

	addr = mt7603_wtbl2_addr(idx);
	val = mt76_rr(dev, addr + 9 * 4);
	val &= ~(MT_WTBL2_W9_SHORT_GI_20 | MT_WTBL2_W9_SHORT_GI_40 |
		 MT_WTBL2_W9_SHORT_GI_80);
	if (sta->ht_cap.cap & IEEE80211_HT_CAP_SGI_20)
		val |= MT_WTBL2_W9_SHORT_GI_20;
	if (sta->ht_cap.cap & IEEE80211_HT_CAP_SGI_40)
		val |= MT_WTBL2_W9_SHORT_GI_40;
	mt76_wr(dev, addr + 9 * 4, val);
}

void mt7603_mac_rx_ba_reset(struct mt7603_dev *dev, void *addr, u8 tid)
{
	mt76_wr(dev, MT_BA_CONTROL_0, get_unaligned_le32(addr));
	mt76_wr(dev, MT_BA_CONTROL_1,
		(get_unaligned_le16(addr + 4) |
		 FIELD_PREP(MT_BA_CONTROL_1_TID, tid) |
		 MT_BA_CONTROL_1_RESET));
}

void mt7603_mac_tx_ba_reset(struct mt7603_dev *dev, int wcid, int tid,
			    int ba_size)
{
	u32 addr = mt7603_wtbl2_addr(wcid);
	u32 tid_mask = FIELD_PREP(MT_WTBL2_W15_BA_EN_TIDS, BIT(tid)) |
		       (MT_WTBL2_W15_BA_WIN_SIZE <<
			(tid * MT_WTBL2_W15_BA_WIN_SIZE_SHIFT));
	u32 tid_val;
	int i;

	if (ba_size < 0) {
		/* disable */
		mt76_clear(dev, addr + (15 * 4), tid_mask);
		return;
	}

	for (i = 7; i > 0; i--) {
		if (ba_size >= MT_AGG_SIZE_LIMIT(i))
			break;
	}

	tid_val = FIELD_PREP(MT_WTBL2_W15_BA_EN_TIDS, BIT(tid)) |
		  i << (tid * MT_WTBL2_W15_BA_WIN_SIZE_SHIFT);

	mt76_rmw(dev, addr + (15 * 4), tid_mask, tid_val);
}

void mt7603_mac_sta_poll(struct mt7603_dev *dev)
{
	static const u8 ac_to_tid[4] = {
		[IEEE80211_AC_BE] = 0,
		[IEEE80211_AC_BK] = 1,
		[IEEE80211_AC_VI] = 4,
		[IEEE80211_AC_VO] = 6
	};
	struct ieee80211_sta *sta;
	struct mt7603_sta *msta;
	u32 total_airtime = 0;
	u32 airtime[4];
	u32 addr;
	int i;

	rcu_read_lock();

	while (1) {
		bool clear = false;

		spin_lock_bh(&dev->sta_poll_lock);
		if (list_empty(&dev->sta_poll_list)) {
			spin_unlock_bh(&dev->sta_poll_lock);
			break;
		}

		msta = list_first_entry(&dev->sta_poll_list, struct mt7603_sta,
					poll_list);
		list_del_init(&msta->poll_list);
		spin_unlock_bh(&dev->sta_poll_lock);

		addr = mt7603_wtbl4_addr(msta->wcid.idx);
		for (i = 0; i < 4; i++) {
			u32 airtime_last = msta->tx_airtime_ac[i];

			msta->tx_airtime_ac[i] = mt76_rr(dev, addr + i * 8);
			airtime[i] = msta->tx_airtime_ac[i] - airtime_last;
			airtime[i] *= 32;
			total_airtime += airtime[i];

			if (msta->tx_airtime_ac[i] & BIT(22))
				clear = true;
		}

		if (clear) {
			mt7603_wtbl_update(dev, msta->wcid.idx,
					   MT_WTBL_UPDATE_ADM_COUNT_CLEAR);
			memset(msta->tx_airtime_ac, 0,
			       sizeof(msta->tx_airtime_ac));
		}

		if (!msta->wcid.sta)
			continue;

		sta = container_of((void *)msta, struct ieee80211_sta, drv_priv);
		for (i = 0; i < 4; i++) {
			struct mt76_queue *q = dev->mphy.q_tx[i];
			u8 qidx = q->hw_idx;
			u8 tid = ac_to_tid[i];
			u32 txtime = airtime[qidx];

			if (!txtime)
				continue;

			ieee80211_sta_register_airtime(sta, tid, txtime, 0);
		}
	}

	rcu_read_unlock();

	if (!total_airtime)
		return;

	spin_lock_bh(&dev->mt76.cc_lock);
	dev->mphy.chan_state->cc_tx += total_airtime;
	spin_unlock_bh(&dev->mt76.cc_lock);
}

static struct mt76_wcid *
mt7603_rx_get_wcid(struct mt7603_dev *dev, u8 idx, bool unicast)
{
	struct mt7603_sta *sta;
	struct mt76_wcid *wcid;

	if (idx >= MT7603_WTBL_SIZE)
		return NULL;

	wcid = rcu_dereference(dev->mt76.wcid[idx]);
	if (unicast || !wcid)
		return wcid;

	if (!wcid->sta)
		return NULL;

	sta = container_of(wcid, struct mt7603_sta, wcid);
	if (!sta->vif)
		return NULL;

	return &sta->vif->sta.wcid;
}

int
mt7603_mac_fill_rx(struct mt7603_dev *dev, struct sk_buff *skb)
{
	struct mt76_rx_status *status = (struct mt76_rx_status *)skb->cb;
	struct ieee80211_supported_band *sband;
	struct ieee80211_hdr *hdr;
	__le32 *rxd = (__le32 *)skb->data;
	u32 rxd0 = le32_to_cpu(rxd[0]);
	u32 rxd1 = le32_to_cpu(rxd[1]);
	u32 rxd2 = le32_to_cpu(rxd[2]);
	bool unicast = rxd1 & MT_RXD1_NORMAL_U2M;
	bool insert_ccmp_hdr = false;
	bool remove_pad;
	int idx;
	int i;

	memset(status, 0, sizeof(*status));

	i = FIELD_GET(MT_RXD1_NORMAL_CH_FREQ, rxd1);
	sband = (i & 1) ? &dev->mphy.sband_5g.sband : &dev->mphy.sband_2g.sband;
	i >>= 1;

	idx = FIELD_GET(MT_RXD2_NORMAL_WLAN_IDX, rxd2);
	status->wcid = mt7603_rx_get_wcid(dev, idx, unicast);

	status->band = sband->band;
	if (i < sband->n_channels)
		status->freq = sband->channels[i].center_freq;

	if (rxd2 & MT_RXD2_NORMAL_FCS_ERR)
		status->flag |= RX_FLAG_FAILED_FCS_CRC;

	if (rxd2 & MT_RXD2_NORMAL_TKIP_MIC_ERR)
		status->flag |= RX_FLAG_MMIC_ERROR;

	if (FIELD_GET(MT_RXD2_NORMAL_SEC_MODE, rxd2) != 0 &&
	    !(rxd2 & (MT_RXD2_NORMAL_CLM | MT_RXD2_NORMAL_CM))) {
		status->flag |= RX_FLAG_DECRYPTED;
		status->flag |= RX_FLAG_IV_STRIPPED;
		status->flag |= RX_FLAG_MMIC_STRIPPED | RX_FLAG_MIC_STRIPPED;
	}

	remove_pad = rxd1 & MT_RXD1_NORMAL_HDR_OFFSET;

	if (rxd2 & MT_RXD2_NORMAL_MAX_LEN_ERROR)
		return -EINVAL;

	if (!sband->channels)
		return -EINVAL;

	rxd += 4;
	if (rxd0 & MT_RXD0_NORMAL_GROUP_4) {
		rxd += 4;
		if ((u8 *)rxd - skb->data >= skb->len)
			return -EINVAL;
	}
	if (rxd0 & MT_RXD0_NORMAL_GROUP_1) {
		u8 *data = (u8 *)rxd;

		if (status->flag & RX_FLAG_DECRYPTED) {
			status->iv[0] = data[5];
			status->iv[1] = data[4];
			status->iv[2] = data[3];
			status->iv[3] = data[2];
			status->iv[4] = data[1];
			status->iv[5] = data[0];

			insert_ccmp_hdr = FIELD_GET(MT_RXD2_NORMAL_FRAG, rxd2);
		}

		rxd += 4;
		if ((u8 *)rxd - skb->data >= skb->len)
			return -EINVAL;
	}
	if (rxd0 & MT_RXD0_NORMAL_GROUP_2) {
		status->timestamp = le32_to_cpu(rxd[0]);
		status->flag |= RX_FLAG_MACTIME_START;

		if (!(rxd2 & (MT_RXD2_NORMAL_NON_AMPDU_SUB |
			      MT_RXD2_NORMAL_NON_AMPDU))) {
			status->flag |= RX_FLAG_AMPDU_DETAILS;

			/* all subframes of an A-MPDU have the same timestamp */
			if (dev->rx_ampdu_ts != status->timestamp) {
				if (!++dev->ampdu_ref)
					dev->ampdu_ref++;
			}
			dev->rx_ampdu_ts = status->timestamp;

			status->ampdu_ref = dev->ampdu_ref;
		}

		rxd += 2;
		if ((u8 *)rxd - skb->data >= skb->len)
			return -EINVAL;
	}
	if (rxd0 & MT_RXD0_NORMAL_GROUP_3) {
		u32 rxdg0 = le32_to_cpu(rxd[0]);
		u32 rxdg3 = le32_to_cpu(rxd[3]);
		bool cck = false;

		i = FIELD_GET(MT_RXV1_TX_RATE, rxdg0);
		switch (FIELD_GET(MT_RXV1_TX_MODE, rxdg0)) {
		case MT_PHY_TYPE_CCK:
			cck = true;
			fallthrough;
		case MT_PHY_TYPE_OFDM:
			i = mt76_get_rate(&dev->mt76, sband, i, cck);
			break;
		case MT_PHY_TYPE_HT_GF:
		case MT_PHY_TYPE_HT:
			status->encoding = RX_ENC_HT;
			if (i > 15)
				return -EINVAL;
			break;
		default:
			return -EINVAL;
		}

		if (rxdg0 & MT_RXV1_HT_SHORT_GI)
			status->enc_flags |= RX_ENC_FLAG_SHORT_GI;
		if (rxdg0 & MT_RXV1_HT_AD_CODE)
			status->enc_flags |= RX_ENC_FLAG_LDPC;

		status->enc_flags |= RX_ENC_FLAG_STBC_MASK *
				    FIELD_GET(MT_RXV1_HT_STBC, rxdg0);

		status->rate_idx = i;

		status->chains = dev->mphy.antenna_mask;
		status->chain_signal[0] = FIELD_GET(MT_RXV4_IB_RSSI0, rxdg3) +
					  dev->rssi_offset[0];
		status->chain_signal[1] = FIELD_GET(MT_RXV4_IB_RSSI1, rxdg3) +
					  dev->rssi_offset[1];

		status->signal = status->chain_signal[0];
		if (status->chains & BIT(1))
			status->signal = max(status->signal,
					     status->chain_signal[1]);

		if (FIELD_GET(MT_RXV1_FRAME_MODE, rxdg0) == 1)
			status->bw = RATE_INFO_BW_40;

		rxd += 6;
		if ((u8 *)rxd - skb->data >= skb->len)
			return -EINVAL;
	} else {
		return -EINVAL;
	}

	skb_pull(skb, (u8 *)rxd - skb->data + 2 * remove_pad);

	if (insert_ccmp_hdr) {
		u8 key_id = FIELD_GET(MT_RXD1_NORMAL_KEY_ID, rxd1);

		mt76_insert_ccmp_hdr(skb, key_id);
	}

	hdr = (struct ieee80211_hdr *)skb->data;
	if (!status->wcid || !ieee80211_is_data_qos(hdr->frame_control))
		return 0;

	status->aggr = unicast &&
		       !ieee80211_is_qos_nullfunc(hdr->frame_control);
	status->qos_ctl = *ieee80211_get_qos_ctl(hdr);
	status->seqno = IEEE80211_SEQ_TO_SN(le16_to_cpu(hdr->seq_ctrl));

	return 0;
}

static u16
mt7603_mac_tx_rate_val(struct mt7603_dev *dev,
		       const struct ieee80211_tx_rate *rate, bool stbc, u8 *bw)
{
	u8 phy, nss, rate_idx;
	u16 rateval;

	*bw = 0;
	if (rate->flags & IEEE80211_TX_RC_MCS) {
		rate_idx = rate->idx;
		nss = 1 + (rate->idx >> 3);
		phy = MT_PHY_TYPE_HT;
		if (rate->flags & IEEE80211_TX_RC_GREEN_FIELD)
			phy = MT_PHY_TYPE_HT_GF;
		if (rate->flags & IEEE80211_TX_RC_40_MHZ_WIDTH)
			*bw = 1;
	} else {
		const struct ieee80211_rate *r;
		int band = dev->mphy.chandef.chan->band;
		u16 val;

		nss = 1;
		r = &mt76_hw(dev)->wiphy->bands[band]->bitrates[rate->idx];
		if (rate->flags & IEEE80211_TX_RC_USE_SHORT_PREAMBLE)
			val = r->hw_value_short;
		else
			val = r->hw_value;

		phy = val >> 8;
		rate_idx = val & 0xff;
	}

	rateval = (FIELD_PREP(MT_TX_RATE_IDX, rate_idx) |
		   FIELD_PREP(MT_TX_RATE_MODE, phy));

	if (stbc && nss == 1)
		rateval |= MT_TX_RATE_STBC;

	return rateval;
}

void mt7603_wtbl_set_rates(struct mt7603_dev *dev, struct mt7603_sta *sta,
			   struct ieee80211_tx_rate *probe_rate,
			   struct ieee80211_tx_rate *rates)
{
	struct ieee80211_tx_rate *ref;
	int wcid = sta->wcid.idx;
	u32 addr = mt7603_wtbl2_addr(wcid);
	bool stbc = false;
	int n_rates = sta->n_rates;
	u8 bw, bw_prev, bw_idx = 0;
	u16 val[4];
	u16 probe_val;
	u32 w9 = mt76_rr(dev, addr + 9 * 4);
	bool rateset;
	int i, k;

	if (!mt76_poll(dev, MT_WTBL_UPDATE, MT_WTBL_UPDATE_BUSY, 0, 5000))
		return;

	for (i = n_rates; i < 4; i++)
		rates[i] = rates[n_rates - 1];

	rateset = !(sta->rate_set_tsf & BIT(0));
	memcpy(sta->rateset[rateset].rates, rates,
	       sizeof(sta->rateset[rateset].rates));
	if (probe_rate) {
		sta->rateset[rateset].probe_rate = *probe_rate;
		ref = &sta->rateset[rateset].probe_rate;
	} else {
		sta->rateset[rateset].probe_rate.idx = -1;
		ref = &sta->rateset[rateset].rates[0];
	}

	rates = sta->rateset[rateset].rates;
	for (i = 0; i < ARRAY_SIZE(sta->rateset[rateset].rates); i++) {
		/*
		 * We don't support switching between short and long GI
		 * within the rate set. For accurate tx status reporting, we
		 * need to make sure that flags match.
		 * For improved performance, avoid duplicate entries by
		 * decrementing the MCS index if necessary
		 */
		if ((ref->flags ^ rates[i].flags) & IEEE80211_TX_RC_SHORT_GI)
			rates[i].flags ^= IEEE80211_TX_RC_SHORT_GI;

		for (k = 0; k < i; k++) {
			if (rates[i].idx != rates[k].idx)
				continue;
			if ((rates[i].flags ^ rates[k].flags) &
			    IEEE80211_TX_RC_40_MHZ_WIDTH)
				continue;

			if (!rates[i].idx)
				continue;

			rates[i].idx--;
		}
	}

	w9 &= MT_WTBL2_W9_SHORT_GI_20 | MT_WTBL2_W9_SHORT_GI_40 |
	      MT_WTBL2_W9_SHORT_GI_80;

	val[0] = mt7603_mac_tx_rate_val(dev, &rates[0], stbc, &bw);
	bw_prev = bw;

	if (probe_rate) {
		probe_val = mt7603_mac_tx_rate_val(dev, probe_rate, stbc, &bw);
		if (bw)
			bw_idx = 1;
		else
			bw_prev = 0;
	} else {
		probe_val = val[0];
	}

	w9 |= FIELD_PREP(MT_WTBL2_W9_CC_BW_SEL, bw);
	w9 |= FIELD_PREP(MT_WTBL2_W9_BW_CAP, bw);

	val[1] = mt7603_mac_tx_rate_val(dev, &rates[1], stbc, &bw);
	if (bw_prev) {
		bw_idx = 3;
		bw_prev = bw;
	}

	val[2] = mt7603_mac_tx_rate_val(dev, &rates[2], stbc, &bw);
	if (bw_prev) {
		bw_idx = 5;
		bw_prev = bw;
	}

	val[3] = mt7603_mac_tx_rate_val(dev, &rates[3], stbc, &bw);
	if (bw_prev)
		bw_idx = 7;

	w9 |= FIELD_PREP(MT_WTBL2_W9_CHANGE_BW_RATE,
		       bw_idx ? bw_idx - 1 : 7);

	mt76_wr(dev, MT_WTBL_RIUCR0, w9);

	mt76_wr(dev, MT_WTBL_RIUCR1,
		FIELD_PREP(MT_WTBL_RIUCR1_RATE0, probe_val) |
		FIELD_PREP(MT_WTBL_RIUCR1_RATE1, val[0]) |
		FIELD_PREP(MT_WTBL_RIUCR1_RATE2_LO, val[1]));

	mt76_wr(dev, MT_WTBL_RIUCR2,
		FIELD_PREP(MT_WTBL_RIUCR2_RATE2_HI, val[1] >> 8) |
		FIELD_PREP(MT_WTBL_RIUCR2_RATE3, val[1]) |
		FIELD_PREP(MT_WTBL_RIUCR2_RATE4, val[2]) |
		FIELD_PREP(MT_WTBL_RIUCR2_RATE5_LO, val[2]));

	mt76_wr(dev, MT_WTBL_RIUCR3,
		FIELD_PREP(MT_WTBL_RIUCR3_RATE5_HI, val[2] >> 4) |
		FIELD_PREP(MT_WTBL_RIUCR3_RATE6, val[3]) |
		FIELD_PREP(MT_WTBL_RIUCR3_RATE7, val[3]));

	mt76_set(dev, MT_LPON_T0CR, MT_LPON_T0CR_MODE); /* TSF read */
	sta->rate_set_tsf = (mt76_rr(dev, MT_LPON_UTTR0) & ~BIT(0)) | rateset;

	mt76_wr(dev, MT_WTBL_UPDATE,
		FIELD_PREP(MT_WTBL_UPDATE_WLAN_IDX, wcid) |
		MT_WTBL_UPDATE_RATE_UPDATE |
		MT_WTBL_UPDATE_TX_COUNT_CLEAR);

	if (!(sta->wcid.tx_info & MT_WCID_TX_INFO_SET))
		mt76_poll(dev, MT_WTBL_UPDATE, MT_WTBL_UPDATE_BUSY, 0, 5000);

	sta->rate_count = 2 * MT7603_RATE_RETRY * n_rates;
	sta->wcid.tx_info |= MT_WCID_TX_INFO_SET;
}

static enum mt7603_cipher_type
mt7603_mac_get_key_info(struct ieee80211_key_conf *key, u8 *key_data)
{
	memset(key_data, 0, 32);
	if (!key)
		return MT_CIPHER_NONE;

	if (key->keylen > 32)
		return MT_CIPHER_NONE;

	memcpy(key_data, key->key, key->keylen);

	switch (key->cipher) {
	case WLAN_CIPHER_SUITE_WEP40:
		return MT_CIPHER_WEP40;
	case WLAN_CIPHER_SUITE_WEP104:
		return MT_CIPHER_WEP104;
	case WLAN_CIPHER_SUITE_TKIP:
		/* Rx/Tx MIC keys are swapped */
		memcpy(key_data + 16, key->key + 24, 8);
		memcpy(key_data + 24, key->key + 16, 8);
		return MT_CIPHER_TKIP;
	case WLAN_CIPHER_SUITE_CCMP:
		return MT_CIPHER_AES_CCMP;
	default:
		return MT_CIPHER_NONE;
	}
}

int mt7603_wtbl_set_key(struct mt7603_dev *dev, int wcid,
			struct ieee80211_key_conf *key)
{
	enum mt7603_cipher_type cipher;
	u32 addr = mt7603_wtbl3_addr(wcid);
	u8 key_data[32];
	int key_len = sizeof(key_data);

	cipher = mt7603_mac_get_key_info(key, key_data);
	if (cipher == MT_CIPHER_NONE && key)
		return -EOPNOTSUPP;

	if (key && (cipher == MT_CIPHER_WEP40 || cipher == MT_CIPHER_WEP104)) {
		addr += key->keyidx * 16;
		key_len = 16;
	}

	mt76_wr_copy(dev, addr, key_data, key_len);

	addr = mt7603_wtbl1_addr(wcid);
	mt76_rmw_field(dev, addr + 2 * 4, MT_WTBL1_W2_KEY_TYPE, cipher);
	if (key)
		mt76_rmw_field(dev, addr, MT_WTBL1_W0_KEY_IDX, key->keyidx);
	mt76_rmw_field(dev, addr, MT_WTBL1_W0_RX_KEY_VALID, !!key);

	return 0;
}

static int
mt7603_mac_write_txwi(struct mt7603_dev *dev, __le32 *txwi,
		      struct sk_buff *skb, enum mt76_txq_id qid,
		      struct mt76_wcid *wcid, struct ieee80211_sta *sta,
		      int pid, struct ieee80211_key_conf *key)
{
	struct ieee80211_tx_info *info = IEEE80211_SKB_CB(skb);
	struct ieee80211_tx_rate *rate = &info->control.rates[0];
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *)skb->data;
	struct ieee80211_bar *bar = (struct ieee80211_bar *)skb->data;
	struct ieee80211_vif *vif = info->control.vif;
	struct mt76_queue *q = dev->mphy.q_tx[qid];
	struct mt7603_vif *mvif;
	int wlan_idx;
	int hdr_len = ieee80211_get_hdrlen_from_skb(skb);
	int tx_count = 8;
	u8 frame_type, frame_subtype;
	u16 fc = le16_to_cpu(hdr->frame_control);
	u16 seqno = 0;
	u8 vif_idx = 0;
	u32 val;
	u8 bw;

	if (vif) {
		mvif = (struct mt7603_vif *)vif->drv_priv;
		vif_idx = mvif->idx;
		if (vif_idx && qid >= MT_TXQ_BEACON)
			vif_idx += 0x10;
	}

	if (sta) {
		struct mt7603_sta *msta = (struct mt7603_sta *)sta->drv_priv;

		tx_count = msta->rate_count;
	}

	if (wcid)
		wlan_idx = wcid->idx;
	else
		wlan_idx = MT7603_WTBL_RESERVED;

	frame_type = (fc & IEEE80211_FCTL_FTYPE) >> 2;
	frame_subtype = (fc & IEEE80211_FCTL_STYPE) >> 4;

	val = FIELD_PREP(MT_TXD0_TX_BYTES, skb->len + MT_TXD_SIZE) |
	      FIELD_PREP(MT_TXD0_Q_IDX, q->hw_idx);
	txwi[0] = cpu_to_le32(val);

	val = MT_TXD1_LONG_FORMAT |
	      FIELD_PREP(MT_TXD1_OWN_MAC, vif_idx) |
	      FIELD_PREP(MT_TXD1_TID,
			 skb->priority & IEEE80211_QOS_CTL_TID_MASK) |
	      FIELD_PREP(MT_TXD1_HDR_FORMAT, MT_HDR_FORMAT_802_11) |
	      FIELD_PREP(MT_TXD1_HDR_INFO, hdr_len / 2) |
	      FIELD_PREP(MT_TXD1_WLAN_IDX, wlan_idx) |
	      FIELD_PREP(MT_TXD1_PROTECTED, !!key);
	txwi[1] = cpu_to_le32(val);

	if (info->flags & IEEE80211_TX_CTL_NO_ACK)
		txwi[1] |= cpu_to_le32(MT_TXD1_NO_ACK);

	val = FIELD_PREP(MT_TXD2_FRAME_TYPE, frame_type) |
	      FIELD_PREP(MT_TXD2_SUB_TYPE, frame_subtype) |
	      FIELD_PREP(MT_TXD2_MULTICAST,
			 is_multicast_ether_addr(hdr->addr1));
	txwi[2] = cpu_to_le32(val);

	if (!(info->flags & IEEE80211_TX_CTL_AMPDU))
		txwi[2] |= cpu_to_le32(MT_TXD2_BA_DISABLE);

	txwi[4] = 0;

	val = MT_TXD5_TX_STATUS_HOST | MT_TXD5_SW_POWER_MGMT |
	      FIELD_PREP(MT_TXD5_PID, pid);
	txwi[5] = cpu_to_le32(val);

	txwi[6] = 0;

	if (rate->idx >= 0 && rate->count &&
	    !(info->flags & IEEE80211_TX_CTL_RATE_CTRL_PROBE)) {
		bool stbc = info->flags & IEEE80211_TX_CTL_STBC;
		u16 rateval = mt7603_mac_tx_rate_val(dev, rate, stbc, &bw);

		txwi[2] |= cpu_to_le32(MT_TXD2_FIX_RATE);

		val = MT_TXD6_FIXED_BW |
		      FIELD_PREP(MT_TXD6_BW, bw) |
		      FIELD_PREP(MT_TXD6_TX_RATE, rateval);
		txwi[6] |= cpu_to_le32(val);

		if (rate->flags & IEEE80211_TX_RC_SHORT_GI)
			txwi[6] |= cpu_to_le32(MT_TXD6_SGI);

		if (!(rate->flags & IEEE80211_TX_RC_MCS))
			txwi[2] |= cpu_to_le32(MT_TXD2_BA_DISABLE);

		tx_count = rate->count;
	}

	/* use maximum tx count for beacons and buffered multicast */
	if (qid >= MT_TXQ_BEACON)
		tx_count = 0x1f;

	val = FIELD_PREP(MT_TXD3_REM_TX_COUNT, tx_count) |
		  MT_TXD3_SN_VALID;

	if (ieee80211_is_data_qos(hdr->frame_control))
		seqno = le16_to_cpu(hdr->seq_ctrl);
	else if (ieee80211_is_back_req(hdr->frame_control))
		seqno = le16_to_cpu(bar->start_seq_num);
	else
		val &= ~MT_TXD3_SN_VALID;

	val |= FIELD_PREP(MT_TXD3_SEQ, seqno >> 4);

	txwi[3] = cpu_to_le32(val);

	if (key) {
		u64 pn = atomic64_inc_return(&key->tx_pn);

		txwi[3] |= cpu_to_le32(MT_TXD3_PN_VALID);
		txwi[4] = cpu_to_le32(pn & GENMASK(31, 0));
		txwi[5] |= cpu_to_le32(FIELD_PREP(MT_TXD5_PN_HIGH, pn >> 32));
	}

	txwi[7] = 0;

	return 0;
}

int mt7603_tx_prepare_skb(struct mt76_dev *mdev, void *txwi_ptr,
			  enum mt76_txq_id qid, struct mt76_wcid *wcid,
			  struct ieee80211_sta *sta,
			  struct mt76_tx_info *tx_info)
{
	struct mt7603_dev *dev = container_of(mdev, struct mt7603_dev, mt76);
	struct mt7603_sta *msta = container_of(wcid, struct mt7603_sta, wcid);
	struct ieee80211_tx_info *info = IEEE80211_SKB_CB(tx_info->skb);
	struct ieee80211_key_conf *key = info->control.hw_key;
	int pid;

	if (!wcid)
		wcid = &dev->global_sta.wcid;

	if (sta) {
		msta = (struct mt7603_sta *)sta->drv_priv;

		if ((info->flags & (IEEE80211_TX_CTL_NO_PS_BUFFER |
				    IEEE80211_TX_CTL_CLEAR_PS_FILT)) ||
		    (info->control.flags & IEEE80211_TX_CTRL_PS_RESPONSE))
			mt7603_wtbl_set_ps(dev, msta, false);

		mt76_tx_check_agg_ssn(sta, tx_info->skb);
	}

	pid = mt76_tx_status_skb_add(mdev, wcid, tx_info->skb);

	if (info->flags & IEEE80211_TX_CTL_RATE_CTRL_PROBE) {
		spin_lock_bh(&dev->mt76.lock);
		mt7603_wtbl_set_rates(dev, msta, &info->control.rates[0],
				      msta->rates);
		msta->rate_probe = true;
		spin_unlock_bh(&dev->mt76.lock);
	}

	mt7603_mac_write_txwi(dev, txwi_ptr, tx_info->skb, qid, wcid,
			      sta, pid, key);

	return 0;
}

static bool
mt7603_fill_txs(struct mt7603_dev *dev, struct mt7603_sta *sta,
		struct ieee80211_tx_info *info, __le32 *txs_data)
{
	struct ieee80211_supported_band *sband;
	struct mt7603_rate_set *rs;
	int first_idx = 0, last_idx;
	u32 rate_set_tsf;
	u32 final_rate;
	u32 final_rate_flags;
	bool rs_idx;
	bool ack_timeout;
	bool fixed_rate;
	bool probe;
	bool ampdu;
	bool cck = false;
	int count;
	u32 txs;
	int idx;
	int i;

	fixed_rate = info->status.rates[0].count;
	probe = !!(info->flags & IEEE80211_TX_CTL_RATE_CTRL_PROBE);

	txs = le32_to_cpu(txs_data[4]);
	ampdu = !fixed_rate && (txs & MT_TXS4_AMPDU);
	count = FIELD_GET(MT_TXS4_TX_COUNT, txs);
	last_idx = FIELD_GET(MT_TXS4_LAST_TX_RATE, txs);

	txs = le32_to_cpu(txs_data[0]);
	final_rate = FIELD_GET(MT_TXS0_TX_RATE, txs);
	ack_timeout = txs & MT_TXS0_ACK_TIMEOUT;

	if (!ampdu && (txs & MT_TXS0_RTS_TIMEOUT))
		return false;

	if (txs & MT_TXS0_QUEUE_TIMEOUT)
		return false;

	if (!ack_timeout)
		info->flags |= IEEE80211_TX_STAT_ACK;

	info->status.ampdu_len = 1;
	info->status.ampdu_ack_len = !!(info->flags &
					IEEE80211_TX_STAT_ACK);

	if (ampdu || (info->flags & IEEE80211_TX_CTL_AMPDU))
		info->flags |= IEEE80211_TX_STAT_AMPDU | IEEE80211_TX_CTL_AMPDU;

	first_idx = max_t(int, 0, last_idx - (count - 1) / MT7603_RATE_RETRY);

	if (fixed_rate && !probe) {
		info->status.rates[0].count = count;
		i = 0;
		goto out;
	}

	rate_set_tsf = READ_ONCE(sta->rate_set_tsf);
	rs_idx = !((u32)(FIELD_GET(MT_TXS1_F0_TIMESTAMP, le32_to_cpu(txs_data[1])) -
			 rate_set_tsf) < 1000000);
	rs_idx ^= rate_set_tsf & BIT(0);
	rs = &sta->rateset[rs_idx];

	if (!first_idx && rs->probe_rate.idx >= 0) {
		info->status.rates[0] = rs->probe_rate;

		spin_lock_bh(&dev->mt76.lock);
		if (sta->rate_probe) {
			mt7603_wtbl_set_rates(dev, sta, NULL,
					      sta->rates);
			sta->rate_probe = false;
		}
		spin_unlock_bh(&dev->mt76.lock);
	} else {
		info->status.rates[0] = rs->rates[first_idx / 2];
	}
	info->status.rates[0].count = 0;

	for (i = 0, idx = first_idx; count && idx <= last_idx; idx++) {
		struct ieee80211_tx_rate *cur_rate;
		int cur_count;

		cur_rate = &rs->rates[idx / 2];
		cur_count = min_t(int, MT7603_RATE_RETRY, count);
		count -= cur_count;

		if (idx && (cur_rate->idx != info->status.rates[i].idx ||
			    cur_rate->flags != info->status.rates[i].flags)) {
			i++;
			if (i == ARRAY_SIZE(info->status.rates)) {
				i--;
				break;
			}

			info->status.rates[i] = *cur_rate;
			info->status.rates[i].count = 0;
		}

		info->status.rates[i].count += cur_count;
	}

out:
	final_rate_flags = info->status.rates[i].flags;

	switch (FIELD_GET(MT_TX_RATE_MODE, final_rate)) {
	case MT_PHY_TYPE_CCK:
		cck = true;
		fallthrough;
	case MT_PHY_TYPE_OFDM:
		if (dev->mphy.chandef.chan->band == NL80211_BAND_5GHZ)
			sband = &dev->mphy.sband_5g.sband;
		else
			sband = &dev->mphy.sband_2g.sband;
		final_rate &= GENMASK(5, 0);
		final_rate = mt76_get_rate(&dev->mt76, sband, final_rate,
					   cck);
		final_rate_flags = 0;
		break;
	case MT_PHY_TYPE_HT_GF:
	case MT_PHY_TYPE_HT:
		final_rate_flags |= IEEE80211_TX_RC_MCS;
		final_rate &= GENMASK(5, 0);
		if (final_rate > 15)
			return false;
		break;
	default:
		return false;
	}

	info->status.rates[i].idx = final_rate;
	info->status.rates[i].flags = final_rate_flags;

	return true;
}

static bool
mt7603_mac_add_txs_skb(struct mt7603_dev *dev, struct mt7603_sta *sta, int pid,
		       __le32 *txs_data)
{
	struct mt76_dev *mdev = &dev->mt76;
	struct sk_buff_head list;
	struct sk_buff *skb;

	if (pid < MT_PACKET_ID_FIRST)
		return false;

	trace_mac_txdone(mdev, sta->wcid.idx, pid);

	mt76_tx_status_lock(mdev, &list);
	skb = mt76_tx_status_skb_get(mdev, &sta->wcid, pid, &list);
	if (skb) {
		struct ieee80211_tx_info *info = IEEE80211_SKB_CB(skb);

		if (!mt7603_fill_txs(dev, sta, info, txs_data)) {
			ieee80211_tx_info_clear_status(info);
			info->status.rates[0].idx = -1;
		}

		mt76_tx_status_skb_done(mdev, skb, &list);
	}
	mt76_tx_status_unlock(mdev, &list);

	return !!skb;
}

void mt7603_mac_add_txs(struct mt7603_dev *dev, void *data)
{
	struct ieee80211_tx_info info = {};
	struct ieee80211_sta *sta = NULL;
	struct mt7603_sta *msta = NULL;
	struct mt76_wcid *wcid;
	__le32 *txs_data = data;
	u32 txs;
	u8 wcidx;
	u8 pid;

	txs = le32_to_cpu(txs_data[4]);
	pid = FIELD_GET(MT_TXS4_PID, txs);
	txs = le32_to_cpu(txs_data[3]);
	wcidx = FIELD_GET(MT_TXS3_WCID, txs);

	if (pid == MT_PACKET_ID_NO_ACK)
		return;

	if (wcidx >= MT7603_WTBL_SIZE)
		return;

	rcu_read_lock();

	wcid = rcu_dereference(dev->mt76.wcid[wcidx]);
	if (!wcid)
		goto out;

	msta = container_of(wcid, struct mt7603_sta, wcid);
	sta = wcid_to_sta(wcid);

	if (list_empty(&msta->poll_list)) {
		spin_lock_bh(&dev->sta_poll_lock);
		list_add_tail(&msta->poll_list, &dev->sta_poll_list);
		spin_unlock_bh(&dev->sta_poll_lock);
	}

	if (mt7603_mac_add_txs_skb(dev, msta, pid, txs_data))
		goto out;

	if (wcidx >= MT7603_WTBL_STA || !sta)
		goto out;

	if (mt7603_fill_txs(dev, msta, &info, txs_data))
		ieee80211_tx_status_noskb(mt76_hw(dev), sta, &info);

out:
	rcu_read_unlock();
}

void mt7603_tx_complete_skb(struct mt76_dev *mdev, struct mt76_queue_entry *e)
{
	struct mt7603_dev *dev = container_of(mdev, struct mt7603_dev, mt76);
	struct sk_buff *skb = e->skb;

	if (!e->txwi) {
		dev_kfree_skb_any(skb);
		return;
	}

	dev->tx_hang_check = 0;
	mt76_tx_complete_skb(mdev, e->wcid, skb);
}

static bool
wait_for_wpdma(struct mt7603_dev *dev)
{
	return mt76_poll(dev, MT_WPDMA_GLO_CFG,
			 MT_WPDMA_GLO_CFG_TX_DMA_BUSY |
			 MT_WPDMA_GLO_CFG_RX_DMA_BUSY,
			 0, 1000);
}

static void mt7603_pse_reset(struct mt7603_dev *dev)
{
	/* Clear previous reset result */
	if (!dev->reset_cause[RESET_CAUSE_RESET_FAILED])
		mt76_clear(dev, MT_MCU_DEBUG_RESET, MT_MCU_DEBUG_RESET_PSE_S);

	/* Reset PSE */
	mt76_set(dev, MT_MCU_DEBUG_RESET, MT_MCU_DEBUG_RESET_PSE);

	if (!mt76_poll_msec(dev, MT_MCU_DEBUG_RESET,
			    MT_MCU_DEBUG_RESET_PSE_S,
			    MT_MCU_DEBUG_RESET_PSE_S, 500)) {
		dev->reset_cause[RESET_CAUSE_RESET_FAILED]++;
		mt76_clear(dev, MT_MCU_DEBUG_RESET, MT_MCU_DEBUG_RESET_PSE);
	} else {
		dev->reset_cause[RESET_CAUSE_RESET_FAILED] = 0;
		mt76_clear(dev, MT_MCU_DEBUG_RESET, MT_MCU_DEBUG_RESET_QUEUES);
	}

	if (dev->reset_cause[RESET_CAUSE_RESET_FAILED] >= 3)
		dev->reset_cause[RESET_CAUSE_RESET_FAILED] = 0;
}

void mt7603_mac_dma_start(struct mt7603_dev *dev)
{
	mt7603_mac_start(dev);

	wait_for_wpdma(dev);
	usleep_range(50, 100);

	mt76_set(dev, MT_WPDMA_GLO_CFG,
		 (MT_WPDMA_GLO_CFG_TX_DMA_EN |
		  MT_WPDMA_GLO_CFG_RX_DMA_EN |
		  FIELD_PREP(MT_WPDMA_GLO_CFG_DMA_BURST_SIZE, 3) |
		  MT_WPDMA_GLO_CFG_TX_WRITEBACK_DONE));

	mt7603_irq_enable(dev, MT_INT_RX_DONE_ALL | MT_INT_TX_DONE_ALL);
}

void mt7603_mac_start(struct mt7603_dev *dev)
{
	mt76_clear(dev, MT_ARB_SCR,
		   MT_ARB_SCR_TX_DISABLE | MT_ARB_SCR_RX_DISABLE);
	mt76_wr(dev, MT_WF_ARB_TX_START_0, ~0);
	mt76_set(dev, MT_WF_ARB_RQCR, MT_WF_ARB_RQCR_RX_START);
}

void mt7603_mac_stop(struct mt7603_dev *dev)
{
	mt76_set(dev, MT_ARB_SCR,
		 MT_ARB_SCR_TX_DISABLE | MT_ARB_SCR_RX_DISABLE);
	mt76_wr(dev, MT_WF_ARB_TX_START_0, 0);
	mt76_clear(dev, MT_WF_ARB_RQCR, MT_WF_ARB_RQCR_RX_START);
}

void mt7603_pse_client_reset(struct mt7603_dev *dev)
{
	u32 addr;

	addr = mt7603_reg_map(dev, MT_CLIENT_BASE_PHYS_ADDR +
				   MT_CLIENT_RESET_TX);

	/* Clear previous reset state */
	mt76_clear(dev, addr,
		   MT_CLIENT_RESET_TX_R_E_1 |
		   MT_CLIENT_RESET_TX_R_E_2 |
		   MT_CLIENT_RESET_TX_R_E_1_S |
		   MT_CLIENT_RESET_TX_R_E_2_S);

	/* Start PSE client TX abort */
	mt76_set(dev, addr, MT_CLIENT_RESET_TX_R_E_1);
	mt76_poll_msec(dev, addr, MT_CLIENT_RESET_TX_R_E_1_S,
		       MT_CLIENT_RESET_TX_R_E_1_S, 500);

	mt76_set(dev, addr, MT_CLIENT_RESET_TX_R_E_2);
	mt76_set(dev, MT_WPDMA_GLO_CFG, MT_WPDMA_GLO_CFG_SW_RESET);

	/* Wait for PSE client to clear TX FIFO */
	mt76_poll_msec(dev, addr, MT_CLIENT_RESET_TX_R_E_2_S,
		       MT_CLIENT_RESET_TX_R_E_2_S, 500);

	/* Clear PSE client TX abort state */
	mt76_clear(dev, addr,
		   MT_CLIENT_RESET_TX_R_E_1 |
		   MT_CLIENT_RESET_TX_R_E_2);
}

static void mt7603_dma_sched_reset(struct mt7603_dev *dev)
{
	if (!is_mt7628(dev))
		return;

	mt76_set(dev, MT_SCH_4, MT_SCH_4_RESET);
	mt76_clear(dev, MT_SCH_4, MT_SCH_4_RESET);
}

static void mt7603_mac_watchdog_reset(struct mt7603_dev *dev)
{
	int beacon_int = dev->mt76.beacon_int;
	u32 mask = dev->mt76.mmio.irqmask;
	int i;

	ieee80211_stop_queues(dev->mt76.hw);
	set_bit(MT76_RESET, &dev->mphy.state);

	/* lock/unlock all queues to ensure that no tx is pending */
	mt76_txq_schedule_all(&dev->mphy);

	mt76_worker_disable(&dev->mt76.tx_worker);
	tasklet_disable(&dev->mt76.pre_tbtt_tasklet);
	napi_disable(&dev->mt76.napi[0]);
	napi_disable(&dev->mt76.napi[1]);
	napi_disable(&dev->mt76.tx_napi);

	mutex_lock(&dev->mt76.mutex);

	mt7603_beacon_set_timer(dev, -1, 0);

	if (dev->reset_cause[RESET_CAUSE_RESET_FAILED] ||
	    dev->cur_reset_cause == RESET_CAUSE_RX_PSE_BUSY ||
	    dev->cur_reset_cause == RESET_CAUSE_BEACON_STUCK ||
	    dev->cur_reset_cause == RESET_CAUSE_TX_HANG)
		mt7603_pse_reset(dev);

	if (dev->reset_cause[RESET_CAUSE_RESET_FAILED])
		goto skip_dma_reset;

	mt7603_mac_stop(dev);

	mt76_clear(dev, MT_WPDMA_GLO_CFG,
		   MT_WPDMA_GLO_CFG_RX_DMA_EN | MT_WPDMA_GLO_CFG_TX_DMA_EN |
		   MT_WPDMA_GLO_CFG_TX_WRITEBACK_DONE);
	usleep_range(1000, 2000);

	mt7603_irq_disable(dev, mask);

	mt76_set(dev, MT_WPDMA_GLO_CFG, MT_WPDMA_GLO_CFG_FORCE_TX_EOF);

	mt7603_pse_client_reset(dev);

	mt76_queue_tx_cleanup(dev, dev->mt76.q_mcu[MT_MCUQ_WM], true);
	for (i = 0; i < __MT_TXQ_MAX; i++)
		mt76_queue_tx_cleanup(dev, dev->mphy.q_tx[i], true);

	mt76_for_each_q_rx(&dev->mt76, i) {
		mt76_queue_rx_reset(dev, i);
	}

	mt76_tx_status_check(&dev->mt76, NULL, true);

	mt7603_dma_sched_reset(dev);

	mt7603_mac_dma_start(dev);

	mt7603_irq_enable(dev, mask);

skip_dma_reset:
	clear_bit(MT76_RESET, &dev->mphy.state);
	mutex_unlock(&dev->mt76.mutex);

	mt76_worker_enable(&dev->mt76.tx_worker);
	napi_enable(&dev->mt76.tx_napi);
	napi_schedule(&dev->mt76.tx_napi);

	tasklet_enable(&dev->mt76.pre_tbtt_tasklet);
	mt7603_beacon_set_timer(dev, -1, beacon_int);

	napi_enable(&dev->mt76.napi[0]);
	napi_schedule(&dev->mt76.napi[0]);

	napi_enable(&dev->mt76.napi[1]);
	napi_schedule(&dev->mt76.napi[1]);

	ieee80211_wake_queues(dev->mt76.hw);
	mt76_txq_schedule_all(&dev->mphy);
}

static u32 mt7603_dma_debug(struct mt7603_dev *dev, u8 index)
{
	u32 val;

	mt76_wr(dev, MT_WPDMA_DEBUG,
		FIELD_PREP(MT_WPDMA_DEBUG_IDX, index) |
		MT_WPDMA_DEBUG_SEL);

	val = mt76_rr(dev, MT_WPDMA_DEBUG);
	return FIELD_GET(MT_WPDMA_DEBUG_VALUE, val);
}

static bool mt7603_rx_fifo_busy(struct mt7603_dev *dev)
{
	if (is_mt7628(dev))
		return mt7603_dma_debug(dev, 9) & BIT(9);

	return mt7603_dma_debug(dev, 2) & BIT(8);
}

static bool mt7603_rx_dma_busy(struct mt7603_dev *dev)
{
	if (!(mt76_rr(dev, MT_WPDMA_GLO_CFG) & MT_WPDMA_GLO_CFG_RX_DMA_BUSY))
		return false;

	return mt7603_rx_fifo_busy(dev);
}

static bool mt7603_tx_dma_busy(struct mt7603_dev *dev)
{
	u32 val;

	if (!(mt76_rr(dev, MT_WPDMA_GLO_CFG) & MT_WPDMA_GLO_CFG_TX_DMA_BUSY))
		return false;

	val = mt7603_dma_debug(dev, 9);
	return (val & BIT(8)) && (val & 0xf) != 0xf;
}

static bool mt7603_tx_hang(struct mt7603_dev *dev)
{
	struct mt76_queue *q;
	u32 dma_idx, prev_dma_idx;
	int i;

	for (i = 0; i < 4; i++) {
		q = dev->mphy.q_tx[i];

		if (!q->queued)
			continue;

		prev_dma_idx = dev->tx_dma_idx[i];
		dma_idx = readl(&q->regs->dma_idx);
		dev->tx_dma_idx[i] = dma_idx;

		if (dma_idx == prev_dma_idx &&
		    dma_idx != readl(&q->regs->cpu_idx))
			break;
	}

	return i < 4;
}

static bool mt7603_rx_pse_busy(struct mt7603_dev *dev)
{
	u32 addr, val;

	if (mt76_rr(dev, MT_MCU_DEBUG_RESET) & MT_MCU_DEBUG_RESET_QUEUES)
		return true;

	if (mt7603_rx_fifo_busy(dev))
		return false;

	addr = mt7603_reg_map(dev, MT_CLIENT_BASE_PHYS_ADDR + MT_CLIENT_STATUS);
	mt76_wr(dev, addr, 3);
	val = mt76_rr(dev, addr) >> 16;

	if (is_mt7628(dev) && (val & 0x4001) == 0x4001)
		return true;

	return (val & 0x8001) == 0x8001 || (val & 0xe001) == 0xe001;
}

static bool
mt7603_watchdog_check(struct mt7603_dev *dev, u8 *counter,
		      enum mt7603_reset_cause cause,
		      bool (*check)(struct mt7603_dev *dev))
{
	if (dev->reset_test == cause + 1) {
		dev->reset_test = 0;
		goto trigger;
	}

	if (check) {
		if (!check(dev) && *counter < MT7603_WATCHDOG_TIMEOUT) {
			*counter = 0;
			return false;
		}

		(*counter)++;
	}

	if (*counter < MT7603_WATCHDOG_TIMEOUT)
		return false;
trigger:
	dev->cur_reset_cause = cause;
	dev->reset_cause[cause]++;
	return true;
}

void mt7603_update_channel(struct mt76_dev *mdev)
{
	struct mt7603_dev *dev = container_of(mdev, struct mt7603_dev, mt76);
	struct mt76_channel_state *state;

	state = mdev->phy.chan_state;
	state->cc_busy += mt76_rr(dev, MT_MIB_STAT_CCA);
}

void
mt7603_edcca_set_strict(struct mt7603_dev *dev, bool val)
{
	u32 rxtd_6 = 0xd7c80000;

	if (val == dev->ed_strict_mode)
		return;

	dev->ed_strict_mode = val;

	/* Ensure that ED/CCA does not trigger if disabled */
	if (!dev->ed_monitor)
		rxtd_6 |= FIELD_PREP(MT_RXTD_6_CCAED_TH, 0x34);
	else
		rxtd_6 |= FIELD_PREP(MT_RXTD_6_CCAED_TH, 0x7d);

	if (dev->ed_monitor && !dev->ed_strict_mode)
		rxtd_6 |= FIELD_PREP(MT_RXTD_6_ACI_TH, 0x0f);
	else
		rxtd_6 |= FIELD_PREP(MT_RXTD_6_ACI_TH, 0x10);

	mt76_wr(dev, MT_RXTD(6), rxtd_6);

	mt76_rmw_field(dev, MT_RXTD(13), MT_RXTD_13_ACI_TH_EN,
		       dev->ed_monitor && !dev->ed_strict_mode);
}

static void
mt7603_edcca_check(struct mt7603_dev *dev)
{
	u32 val = mt76_rr(dev, MT_AGC(41));
	ktime_t cur_time;
	int rssi0, rssi1;
	u32 active;
	u32 ed_busy;

	if (!dev->ed_monitor)
		return;

	rssi0 = FIELD_GET(MT_AGC_41_RSSI_0, val);
	if (rssi0 > 128)
		rssi0 -= 256;

	if (dev->mphy.antenna_mask & BIT(1)) {
		rssi1 = FIELD_GET(MT_AGC_41_RSSI_1, val);
		if (rssi1 > 128)
			rssi1 -= 256;
	} else {
		rssi1 = rssi0;
	}

	if (max(rssi0, rssi1) >= -40 &&
	    dev->ed_strong_signal < MT7603_EDCCA_BLOCK_TH)
		dev->ed_strong_signal++;
	else if (dev->ed_strong_signal > 0)
		dev->ed_strong_signal--;

	cur_time = ktime_get_boottime();
	ed_busy = mt76_rr(dev, MT_MIB_STAT_ED) & MT_MIB_STAT_ED_MASK;

	active = ktime_to_us(ktime_sub(cur_time, dev->ed_time));
	dev->ed_time = cur_time;

	if (!active)
		return;

	if (100 * ed_busy / active > 90) {
		if (dev->ed_trigger < 0)
			dev->ed_trigger = 0;
		dev->ed_trigger++;
	} else {
		if (dev->ed_trigger > 0)
			dev->ed_trigger = 0;
		dev->ed_trigger--;
	}

	if (dev->ed_trigger > MT7603_EDCCA_BLOCK_TH ||
	    dev->ed_strong_signal < MT7603_EDCCA_BLOCK_TH / 2) {
		mt7603_edcca_set_strict(dev, true);
	} else if (dev->ed_trigger < -MT7603_EDCCA_BLOCK_TH) {
		mt7603_edcca_set_strict(dev, false);
	}

	if (dev->ed_trigger > MT7603_EDCCA_BLOCK_TH)
		dev->ed_trigger = MT7603_EDCCA_BLOCK_TH;
	else if (dev->ed_trigger < -MT7603_EDCCA_BLOCK_TH)
		dev->ed_trigger = -MT7603_EDCCA_BLOCK_TH;
}

void mt7603_cca_stats_reset(struct mt7603_dev *dev)
{
	mt76_set(dev, MT_PHYCTRL(2), MT_PHYCTRL_2_STATUS_RESET);
	mt76_clear(dev, MT_PHYCTRL(2), MT_PHYCTRL_2_STATUS_RESET);
	mt76_set(dev, MT_PHYCTRL(2), MT_PHYCTRL_2_STATUS_EN);
}

static void
mt7603_adjust_sensitivity(struct mt7603_dev *dev)
{
	u32 agc0 = dev->agc0, agc3 = dev->agc3;
	u32 adj;

	if (!dev->sensitivity || dev->sensitivity < -100) {
		dev->sensitivity = 0;
	} else if (dev->sensitivity <= -84) {
		adj = 7 + (dev->sensitivity + 92) / 2;

		agc0 = 0x56f0076f;
		agc0 |= adj << 12;
		agc0 |= adj << 16;
		agc3 = 0x81d0d5e3;
	} else if (dev->sensitivity <= -72) {
		adj = 7 + (dev->sensitivity + 80) / 2;

		agc0 = 0x6af0006f;
		agc0 |= adj << 8;
		agc0 |= adj << 12;
		agc0 |= adj << 16;

		agc3 = 0x8181d5e3;
	} else {
		if (dev->sensitivity > -54)
			dev->sensitivity = -54;

		adj = 7 + (dev->sensitivity + 80) / 2;

		agc0 = 0x7ff0000f;
		agc0 |= adj << 4;
		agc0 |= adj << 8;
		agc0 |= adj << 12;
		agc0 |= adj << 16;

		agc3 = 0x818181e3;
	}

	mt76_wr(dev, MT_AGC(0), agc0);
	mt76_wr(dev, MT_AGC1(0), agc0);

	mt76_wr(dev, MT_AGC(3), agc3);
	mt76_wr(dev, MT_AGC1(3), agc3);
}

static void
mt7603_false_cca_check(struct mt7603_dev *dev)
{
	int pd_cck, pd_ofdm, mdrdy_cck, mdrdy_ofdm;
	int false_cca;
	int min_signal;
	u32 val;

	if (!dev->dynamic_sensitivity)
		return;

	val = mt76_rr(dev, MT_PHYCTRL_STAT_PD);
	pd_cck = FIELD_GET(MT_PHYCTRL_STAT_PD_CCK, val);
	pd_ofdm = FIELD_GET(MT_PHYCTRL_STAT_PD_OFDM, val);

	val = mt76_rr(dev, MT_PHYCTRL_STAT_MDRDY);
	mdrdy_cck = FIELD_GET(MT_PHYCTRL_STAT_MDRDY_CCK, val);
	mdrdy_ofdm = FIELD_GET(MT_PHYCTRL_STAT_MDRDY_OFDM, val);

	dev->false_cca_ofdm = pd_ofdm - mdrdy_ofdm;
	dev->false_cca_cck = pd_cck - mdrdy_cck;

	mt7603_cca_stats_reset(dev);

	min_signal = mt76_get_min_avg_rssi(&dev->mt76, false);
	if (!min_signal) {
		dev->sensitivity = 0;
		dev->last_cca_adj = jiffies;
		goto out;
	}

	min_signal -= 15;

	false_cca = dev->false_cca_ofdm + dev->false_cca_cck;
	if (false_cca > 600 &&
	    dev->sensitivity < -100 + dev->sensitivity_limit) {
		if (!dev->sensitivity)
			dev->sensitivity = -92;
		else
			dev->sensitivity += 2;
		dev->last_cca_adj = jiffies;
	} else if (false_cca < 100 ||
		   time_after(jiffies, dev->last_cca_adj + 10 * HZ)) {
		dev->last_cca_adj = jiffies;
		if (!dev->sensitivity)
			goto out;

		dev->sensitivity -= 2;
	}

	if (dev->sensitivity && dev->sensitivity > min_signal) {
		dev->sensitivity = min_signal;
		dev->last_cca_adj = jiffies;
	}

out:
	mt7603_adjust_sensitivity(dev);
}

void mt7603_mac_work(struct work_struct *work)
{
	struct mt7603_dev *dev = container_of(work, struct mt7603_dev,
					      mphy.mac_work.work);
	bool reset = false;
	int i, idx;

	mt76_tx_status_check(&dev->mt76, NULL, false);

	mutex_lock(&dev->mt76.mutex);

	dev->mphy.mac_work_count++;
	mt76_update_survey(&dev->mt76);
	mt7603_edcca_check(dev);

	for (i = 0, idx = 0; i < 2; i++) {
		u32 val = mt76_rr(dev, MT_TX_AGG_CNT(i));

		dev->mt76.aggr_stats[idx++] += val & 0xffff;
		dev->mt76.aggr_stats[idx++] += val >> 16;
	}

	if (dev->mphy.mac_work_count == 10)
		mt7603_false_cca_check(dev);

	if (mt7603_watchdog_check(dev, &dev->rx_pse_check,
				  RESET_CAUSE_RX_PSE_BUSY,
				  mt7603_rx_pse_busy) ||
	    mt7603_watchdog_check(dev, &dev->beacon_check,
				  RESET_CAUSE_BEACON_STUCK,
				  NULL) ||
	    mt7603_watchdog_check(dev, &dev->tx_hang_check,
				  RESET_CAUSE_TX_HANG,
				  mt7603_tx_hang) ||
	    mt7603_watchdog_check(dev, &dev->tx_dma_check,
				  RESET_CAUSE_TX_BUSY,
				  mt7603_tx_dma_busy) ||
	    mt7603_watchdog_check(dev, &dev->rx_dma_check,
				  RESET_CAUSE_RX_BUSY,
				  mt7603_rx_dma_busy) ||
	    mt7603_watchdog_check(dev, &dev->mcu_hang,
				  RESET_CAUSE_MCU_HANG,
				  NULL) ||
	    dev->reset_cause[RESET_CAUSE_RESET_FAILED]) {
		dev->beacon_check = 0;
		dev->tx_dma_check = 0;
		dev->tx_hang_check = 0;
		dev->rx_dma_check = 0;
		dev->rx_pse_check = 0;
		dev->mcu_hang = 0;
		dev->rx_dma_idx = ~0;
		memset(dev->tx_dma_idx, 0xff, sizeof(dev->tx_dma_idx));
		reset = true;
		dev->mphy.mac_work_count = 0;
	}

	if (dev->mphy.mac_work_count >= 10)
		dev->mphy.mac_work_count = 0;

	mutex_unlock(&dev->mt76.mutex);

	if (reset)
		mt7603_mac_watchdog_reset(dev);

	ieee80211_queue_delayed_work(mt76_hw(dev), &dev->mphy.mac_work,
				     msecs_to_jiffies(MT7603_WATCHDOG_TIME));
}
