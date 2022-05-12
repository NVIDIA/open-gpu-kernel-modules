// SPDX-License-Identifier: ISC
/*
 * Copyright (C) 2016 Felix Fietkau <nbd@nbd.name>
 * Copyright (C) 2018 Lorenzo Bianconi <lorenzo.bianconi83@gmail.com>
 */

#include "mt76x2.h"
#include "eeprom.h"
#include "mcu.h"
#include "../mt76x02_phy.h"

static void
mt76x2_adjust_high_lna_gain(struct mt76x02_dev *dev, int reg, s8 offset)
{
	s8 gain;

	gain = FIELD_GET(MT_BBP_AGC_LNA_HIGH_GAIN,
			 mt76_rr(dev, MT_BBP(AGC, reg)));
	gain -= offset / 2;
	mt76_rmw_field(dev, MT_BBP(AGC, reg), MT_BBP_AGC_LNA_HIGH_GAIN, gain);
}

static void
mt76x2_adjust_agc_gain(struct mt76x02_dev *dev, int reg, s8 offset)
{
	s8 gain;

	gain = FIELD_GET(MT_BBP_AGC_GAIN, mt76_rr(dev, MT_BBP(AGC, reg)));
	gain += offset;
	mt76_rmw_field(dev, MT_BBP(AGC, reg), MT_BBP_AGC_GAIN, gain);
}

void mt76x2_apply_gain_adj(struct mt76x02_dev *dev)
{
	s8 *gain_adj = dev->cal.rx.high_gain;

	mt76x2_adjust_high_lna_gain(dev, 4, gain_adj[0]);
	mt76x2_adjust_high_lna_gain(dev, 5, gain_adj[1]);

	mt76x2_adjust_agc_gain(dev, 8, gain_adj[0]);
	mt76x2_adjust_agc_gain(dev, 9, gain_adj[1]);
}
EXPORT_SYMBOL_GPL(mt76x2_apply_gain_adj);

void mt76x2_phy_set_txpower_regs(struct mt76x02_dev *dev,
				 enum nl80211_band band)
{
	u32 pa_mode[2];
	u32 pa_mode_adj;

	if (band == NL80211_BAND_2GHZ) {
		pa_mode[0] = 0x010055ff;
		pa_mode[1] = 0x00550055;

		mt76_wr(dev, MT_TX_ALC_CFG_2, 0x35160a00);
		mt76_wr(dev, MT_TX_ALC_CFG_3, 0x35160a06);

		if (mt76x02_ext_pa_enabled(dev, band)) {
			mt76_wr(dev, MT_RF_PA_MODE_ADJ0, 0x0000ec00);
			mt76_wr(dev, MT_RF_PA_MODE_ADJ1, 0x0000ec00);
		} else {
			mt76_wr(dev, MT_RF_PA_MODE_ADJ0, 0xf4000200);
			mt76_wr(dev, MT_RF_PA_MODE_ADJ1, 0xfa000200);
		}
	} else {
		pa_mode[0] = 0x0000ffff;
		pa_mode[1] = 0x00ff00ff;

		if (mt76x02_ext_pa_enabled(dev, band)) {
			mt76_wr(dev, MT_TX_ALC_CFG_2, 0x2f0f0400);
			mt76_wr(dev, MT_TX_ALC_CFG_3, 0x2f0f0476);
		} else {
			mt76_wr(dev, MT_TX_ALC_CFG_2, 0x1b0f0400);
			mt76_wr(dev, MT_TX_ALC_CFG_3, 0x1b0f0476);
		}

		if (mt76x02_ext_pa_enabled(dev, band))
			pa_mode_adj = 0x04000000;
		else
			pa_mode_adj = 0;

		mt76_wr(dev, MT_RF_PA_MODE_ADJ0, pa_mode_adj);
		mt76_wr(dev, MT_RF_PA_MODE_ADJ1, pa_mode_adj);
	}

	mt76_wr(dev, MT_BB_PA_MODE_CFG0, pa_mode[0]);
	mt76_wr(dev, MT_BB_PA_MODE_CFG1, pa_mode[1]);
	mt76_wr(dev, MT_RF_PA_MODE_CFG0, pa_mode[0]);
	mt76_wr(dev, MT_RF_PA_MODE_CFG1, pa_mode[1]);

	if (mt76x02_ext_pa_enabled(dev, band)) {
		u32 val;

		if (band == NL80211_BAND_2GHZ)
			val = 0x3c3c023c;
		else
			val = 0x363c023c;

		mt76_wr(dev, MT_TX0_RF_GAIN_CORR, val);
		mt76_wr(dev, MT_TX1_RF_GAIN_CORR, val);
		mt76_wr(dev, MT_TX_ALC_CFG_4, 0x00001818);
	} else {
		if (band == NL80211_BAND_2GHZ) {
			u32 val = 0x0f3c3c3c;

			mt76_wr(dev, MT_TX0_RF_GAIN_CORR, val);
			mt76_wr(dev, MT_TX1_RF_GAIN_CORR, val);
			mt76_wr(dev, MT_TX_ALC_CFG_4, 0x00000606);
		} else {
			mt76_wr(dev, MT_TX0_RF_GAIN_CORR, 0x383c023c);
			mt76_wr(dev, MT_TX1_RF_GAIN_CORR, 0x24282e28);
			mt76_wr(dev, MT_TX_ALC_CFG_4, 0);
		}
	}
}
EXPORT_SYMBOL_GPL(mt76x2_phy_set_txpower_regs);

static int
mt76x2_get_min_rate_power(struct mt76_rate_power *r)
{
	int i;
	s8 ret = 0;

	for (i = 0; i < sizeof(r->all); i++) {
		if (!r->all[i])
			continue;

		if (ret)
			ret = min(ret, r->all[i]);
		else
			ret = r->all[i];
	}

	return ret;
}

void mt76x2_phy_set_txpower(struct mt76x02_dev *dev)
{
	enum nl80211_chan_width width = dev->mphy.chandef.width;
	struct ieee80211_channel *chan = dev->mphy.chandef.chan;
	struct mt76x2_tx_power_info txp;
	int txp_0, txp_1, delta = 0;
	struct mt76_rate_power t = {};
	int base_power, gain;

	mt76x2_get_power_info(dev, &txp, chan);

	if (width == NL80211_CHAN_WIDTH_40)
		delta = txp.delta_bw40;
	else if (width == NL80211_CHAN_WIDTH_80)
		delta = txp.delta_bw80;

	mt76x2_get_rate_power(dev, &t, chan);
	mt76x02_add_rate_power_offset(&t, txp.target_power + delta);
	mt76x02_limit_rate_power(&t, dev->txpower_conf);
	dev->mphy.txpower_cur = mt76x02_get_max_rate_power(&t);

	base_power = mt76x2_get_min_rate_power(&t);
	delta = base_power - txp.target_power;
	txp_0 = txp.chain[0].target_power + txp.chain[0].delta + delta;
	txp_1 = txp.chain[1].target_power + txp.chain[1].delta + delta;

	gain = min(txp_0, txp_1);
	if (gain < 0) {
		base_power -= gain;
		txp_0 -= gain;
		txp_1 -= gain;
	} else if (gain > 0x2f) {
		base_power -= gain - 0x2f;
		txp_0 = 0x2f;
		txp_1 = 0x2f;
	}

	mt76x02_add_rate_power_offset(&t, -base_power);
	dev->target_power = txp.target_power;
	dev->target_power_delta[0] = txp_0 - txp.chain[0].target_power;
	dev->target_power_delta[1] = txp_1 - txp.chain[0].target_power;
	dev->mt76.rate_power = t;

	mt76x02_phy_set_txpower(dev, txp_0, txp_1);
}
EXPORT_SYMBOL_GPL(mt76x2_phy_set_txpower);

void mt76x2_configure_tx_delay(struct mt76x02_dev *dev,
			       enum nl80211_band band, u8 bw)
{
	u32 cfg0, cfg1;

	if (mt76x02_ext_pa_enabled(dev, band)) {
		cfg0 = bw ? 0x000b0c01 : 0x00101101;
		cfg1 = 0x00011414;
	} else {
		cfg0 = bw ? 0x000b0b01 : 0x00101001;
		cfg1 = 0x00021414;
	}
	mt76_wr(dev, MT_TX_SW_CFG0, cfg0);
	mt76_wr(dev, MT_TX_SW_CFG1, cfg1);

	mt76_rmw_field(dev, MT_XIFS_TIME_CFG, MT_XIFS_TIME_CFG_OFDM_SIFS, 15);
}
EXPORT_SYMBOL_GPL(mt76x2_configure_tx_delay);

void mt76x2_phy_tssi_compensate(struct mt76x02_dev *dev)
{
	struct ieee80211_channel *chan = dev->mphy.chandef.chan;
	struct mt76x2_tx_power_info txp;
	struct mt76x2_tssi_comp t = {};

	if (!dev->cal.tssi_cal_done)
		return;

	if (!dev->cal.tssi_comp_pending) {
		/* TSSI trigger */
		t.cal_mode = BIT(0);
		mt76x2_mcu_tssi_comp(dev, &t);
		dev->cal.tssi_comp_pending = true;
	} else {
		if (mt76_rr(dev, MT_BBP(CORE, 34)) & BIT(4))
			return;

		dev->cal.tssi_comp_pending = false;
		mt76x2_get_power_info(dev, &txp, chan);

		if (mt76x02_ext_pa_enabled(dev, chan->band))
			t.pa_mode = 1;

		t.cal_mode = BIT(1);
		t.slope0 = txp.chain[0].tssi_slope;
		t.offset0 = txp.chain[0].tssi_offset;
		t.slope1 = txp.chain[1].tssi_slope;
		t.offset1 = txp.chain[1].tssi_offset;
		mt76x2_mcu_tssi_comp(dev, &t);

		if (t.pa_mode || dev->cal.dpd_cal_done || dev->ed_tx_blocked)
			return;

		usleep_range(10000, 20000);
		mt76x02_mcu_calibrate(dev, MCU_CAL_DPD, chan->hw_value);
		dev->cal.dpd_cal_done = true;
	}
}
EXPORT_SYMBOL_GPL(mt76x2_phy_tssi_compensate);

static void
mt76x2_phy_set_gain_val(struct mt76x02_dev *dev)
{
	u32 val;
	u8 gain_val[2];

	gain_val[0] = dev->cal.agc_gain_cur[0] - dev->cal.agc_gain_adjust;
	gain_val[1] = dev->cal.agc_gain_cur[1] - dev->cal.agc_gain_adjust;

	val = 0x1836 << 16;
	if (!mt76x2_has_ext_lna(dev) &&
	    dev->mphy.chandef.width >= NL80211_CHAN_WIDTH_40)
		val = 0x1e42 << 16;

	if (mt76x2_has_ext_lna(dev) &&
	    dev->mphy.chandef.chan->band == NL80211_BAND_2GHZ &&
	    dev->mphy.chandef.width < NL80211_CHAN_WIDTH_40)
		val = 0x0f36 << 16;

	val |= 0xf8;

	mt76_wr(dev, MT_BBP(AGC, 8),
		val | FIELD_PREP(MT_BBP_AGC_GAIN, gain_val[0]));
	mt76_wr(dev, MT_BBP(AGC, 9),
		val | FIELD_PREP(MT_BBP_AGC_GAIN, gain_val[1]));

	if (dev->mphy.chandef.chan->flags & IEEE80211_CHAN_RADAR)
		mt76x02_phy_dfs_adjust_agc(dev);
}

void mt76x2_phy_update_channel_gain(struct mt76x02_dev *dev)
{
	u8 *gain = dev->cal.agc_gain_init;
	u8 low_gain_delta, gain_delta;
	u32 agc_35, agc_37;
	bool gain_change;
	int low_gain;
	u32 val;

	dev->cal.avg_rssi_all = mt76_get_min_avg_rssi(&dev->mt76, false);
	if (!dev->cal.avg_rssi_all)
		dev->cal.avg_rssi_all = -75;

	low_gain = (dev->cal.avg_rssi_all > mt76x02_get_rssi_gain_thresh(dev)) +
		(dev->cal.avg_rssi_all > mt76x02_get_low_rssi_gain_thresh(dev));

	gain_change = dev->cal.low_gain < 0 ||
		      (dev->cal.low_gain & 2) ^ (low_gain & 2);
	dev->cal.low_gain = low_gain;

	if (!gain_change) {
		if (mt76x02_phy_adjust_vga_gain(dev))
			mt76x2_phy_set_gain_val(dev);
		return;
	}

	if (dev->mphy.chandef.width == NL80211_CHAN_WIDTH_80) {
		mt76_wr(dev, MT_BBP(RXO, 14), 0x00560211);
		val = mt76_rr(dev, MT_BBP(AGC, 26)) & ~0xf;
		if (low_gain == 2)
			val |= 0x3;
		else
			val |= 0x5;
		mt76_wr(dev, MT_BBP(AGC, 26), val);
	} else {
		mt76_wr(dev, MT_BBP(RXO, 14), 0x00560423);
	}

	if (mt76x2_has_ext_lna(dev))
		low_gain_delta = 10;
	else
		low_gain_delta = 14;

	agc_37 = 0x2121262c;
	if (dev->mphy.chandef.chan->band == NL80211_BAND_2GHZ)
		agc_35 = 0x11111516;
	else if (low_gain == 2)
		agc_35 = agc_37 = 0x08080808;
	else if (dev->mphy.chandef.width == NL80211_CHAN_WIDTH_80)
		agc_35 = 0x10101014;
	else
		agc_35 = 0x11111116;

	if (low_gain == 2) {
		mt76_wr(dev, MT_BBP(RXO, 18), 0xf000a990);
		mt76_wr(dev, MT_BBP(AGC, 35), 0x08080808);
		mt76_wr(dev, MT_BBP(AGC, 37), 0x08080808);
		gain_delta = low_gain_delta;
		dev->cal.agc_gain_adjust = 0;
	} else {
		mt76_wr(dev, MT_BBP(RXO, 18), 0xf000a991);
		gain_delta = 0;
		dev->cal.agc_gain_adjust = low_gain_delta;
	}

	mt76_wr(dev, MT_BBP(AGC, 35), agc_35);
	mt76_wr(dev, MT_BBP(AGC, 37), agc_37);

	dev->cal.agc_gain_cur[0] = gain[0] - gain_delta;
	dev->cal.agc_gain_cur[1] = gain[1] - gain_delta;
	mt76x2_phy_set_gain_val(dev);

	/* clear false CCA counters */
	mt76_rr(dev, MT_RX_STAT_1);
}
EXPORT_SYMBOL_GPL(mt76x2_phy_update_channel_gain);
