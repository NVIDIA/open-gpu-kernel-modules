// SPDX-License-Identifier: ISC
/*
 * Copyright (C) 2018 Lorenzo Bianconi <lorenzo.bianconi83@gmail.com>
 */

#include "mt76x2u.h"
#include "eeprom.h"
#include "../mt76x02_phy.h"

static void
mt76x2u_phy_channel_calibrate(struct mt76x02_dev *dev, bool mac_stopped)
{
	struct ieee80211_channel *chan = dev->mphy.chandef.chan;
	bool is_5ghz = chan->band == NL80211_BAND_5GHZ;

	if (dev->cal.channel_cal_done)
		return;

	if (mt76x2_channel_silent(dev))
		return;

	if (!mac_stopped)
		mt76x2u_mac_stop(dev);

	if (is_5ghz)
		mt76x02_mcu_calibrate(dev, MCU_CAL_LC, 0);

	mt76x02_mcu_calibrate(dev, MCU_CAL_TX_LOFT, is_5ghz);
	mt76x02_mcu_calibrate(dev, MCU_CAL_TXIQ, is_5ghz);
	mt76x02_mcu_calibrate(dev, MCU_CAL_RXIQC_FI, is_5ghz);
	mt76x02_mcu_calibrate(dev, MCU_CAL_TEMP_SENSOR, 0);
	mt76x02_mcu_calibrate(dev, MCU_CAL_TX_SHAPING, 0);

	if (!mac_stopped)
		mt76x2_mac_resume(dev);
	mt76x2_apply_gain_adj(dev);
	mt76x02_edcca_init(dev);

	dev->cal.channel_cal_done = true;
}

void mt76x2u_phy_calibrate(struct work_struct *work)
{
	struct mt76x02_dev *dev;

	dev = container_of(work, struct mt76x02_dev, cal_work.work);

	mutex_lock(&dev->mt76.mutex);

	mt76x2u_phy_channel_calibrate(dev, false);
	mt76x2_phy_tssi_compensate(dev);
	mt76x2_phy_update_channel_gain(dev);

	mutex_unlock(&dev->mt76.mutex);

	ieee80211_queue_delayed_work(mt76_hw(dev), &dev->cal_work,
				     MT_CALIBRATE_INTERVAL);
}

int mt76x2u_phy_set_channel(struct mt76x02_dev *dev,
			    struct cfg80211_chan_def *chandef)
{
	u32 ext_cca_chan[4] = {
		[0] = FIELD_PREP(MT_EXT_CCA_CFG_CCA0, 0) |
		      FIELD_PREP(MT_EXT_CCA_CFG_CCA1, 1) |
		      FIELD_PREP(MT_EXT_CCA_CFG_CCA2, 2) |
		      FIELD_PREP(MT_EXT_CCA_CFG_CCA3, 3) |
		      FIELD_PREP(MT_EXT_CCA_CFG_CCA_MASK, BIT(0)),
		[1] = FIELD_PREP(MT_EXT_CCA_CFG_CCA0, 1) |
		      FIELD_PREP(MT_EXT_CCA_CFG_CCA1, 0) |
		      FIELD_PREP(MT_EXT_CCA_CFG_CCA2, 2) |
		      FIELD_PREP(MT_EXT_CCA_CFG_CCA3, 3) |
		      FIELD_PREP(MT_EXT_CCA_CFG_CCA_MASK, BIT(1)),
		[2] = FIELD_PREP(MT_EXT_CCA_CFG_CCA0, 2) |
		      FIELD_PREP(MT_EXT_CCA_CFG_CCA1, 3) |
		      FIELD_PREP(MT_EXT_CCA_CFG_CCA2, 1) |
		      FIELD_PREP(MT_EXT_CCA_CFG_CCA3, 0) |
		      FIELD_PREP(MT_EXT_CCA_CFG_CCA_MASK, BIT(2)),
		[3] = FIELD_PREP(MT_EXT_CCA_CFG_CCA0, 3) |
		      FIELD_PREP(MT_EXT_CCA_CFG_CCA1, 2) |
		      FIELD_PREP(MT_EXT_CCA_CFG_CCA2, 1) |
		      FIELD_PREP(MT_EXT_CCA_CFG_CCA3, 0) |
		      FIELD_PREP(MT_EXT_CCA_CFG_CCA_MASK, BIT(3)),
	};
	bool scan = test_bit(MT76_SCANNING, &dev->mphy.state);
	struct ieee80211_channel *chan = chandef->chan;
	u8 channel = chan->hw_value, bw, bw_index;
	int ch_group_index, freq, freq1, ret;

	dev->cal.channel_cal_done = false;
	freq = chandef->chan->center_freq;
	freq1 = chandef->center_freq1;

	switch (chandef->width) {
	case NL80211_CHAN_WIDTH_40:
		bw = 1;
		if (freq1 > freq) {
			bw_index = 1;
			ch_group_index = 0;
		} else {
			bw_index = 3;
			ch_group_index = 1;
		}
		channel += 2 - ch_group_index * 4;
		break;
	case NL80211_CHAN_WIDTH_80:
		ch_group_index = (freq - freq1 + 30) / 20;
		if (WARN_ON(ch_group_index < 0 || ch_group_index > 3))
			ch_group_index = 0;
		bw = 2;
		bw_index = ch_group_index;
		channel += 6 - ch_group_index * 4;
		break;
	default:
		bw = 0;
		bw_index = 0;
		ch_group_index = 0;
		break;
	}

	mt76x2_read_rx_gain(dev);
	mt76x2_phy_set_txpower_regs(dev, chan->band);
	mt76x2_configure_tx_delay(dev, chan->band, bw);
	mt76x2_phy_set_txpower(dev);

	mt76x02_phy_set_band(dev, chan->band, ch_group_index & 1);
	mt76x02_phy_set_bw(dev, chandef->width, ch_group_index);

	mt76_rmw(dev, MT_EXT_CCA_CFG,
		 (MT_EXT_CCA_CFG_CCA0 |
		  MT_EXT_CCA_CFG_CCA1 |
		  MT_EXT_CCA_CFG_CCA2 |
		  MT_EXT_CCA_CFG_CCA3 |
		  MT_EXT_CCA_CFG_CCA_MASK),
		 ext_cca_chan[ch_group_index]);

	ret = mt76x2_mcu_set_channel(dev, channel, bw, bw_index, scan);
	if (ret)
		return ret;

	mt76x2_mcu_init_gain(dev, channel, dev->cal.rx.mcu_gain, true);

	/* Enable LDPC Rx */
	if (mt76xx_rev(dev) >= MT76XX_REV_E3)
		mt76_set(dev, MT_BBP(RXO, 13), BIT(10));

	if (!dev->cal.init_cal_done) {
		u8 val = mt76x02_eeprom_get(dev, MT_EE_BT_RCAL_RESULT);

		if (val != 0xff)
			mt76x02_mcu_calibrate(dev, MCU_CAL_R, 0);
	}

	mt76x02_mcu_calibrate(dev, MCU_CAL_RXDCOC, channel);

	/* Rx LPF calibration */
	if (!dev->cal.init_cal_done)
		mt76x02_mcu_calibrate(dev, MCU_CAL_RC, 0);
	dev->cal.init_cal_done = true;

	mt76_wr(dev, MT_BBP(AGC, 61), 0xff64a4e2);
	mt76_wr(dev, MT_BBP(AGC, 7), 0x08081010);
	mt76_wr(dev, MT_BBP(AGC, 11), 0x00000404);
	mt76_wr(dev, MT_BBP(AGC, 2), 0x00007070);
	mt76_wr(dev, MT_TXOP_CTRL_CFG, 0X04101b3f);

	mt76_set(dev, MT_BBP(TXO, 4), BIT(25));
	mt76_set(dev, MT_BBP(RXO, 13), BIT(8));

	if (scan)
		return 0;

	mt76x2u_phy_channel_calibrate(dev, true);
	mt76x02_init_agc_gain(dev);

	if (mt76x2_tssi_enabled(dev)) {
		/* init default values for temp compensation */
		mt76_rmw_field(dev, MT_TX_ALC_CFG_1, MT_TX_ALC_CFG_1_TEMP_COMP,
			       0x38);
		mt76_rmw_field(dev, MT_TX_ALC_CFG_2, MT_TX_ALC_CFG_2_TEMP_COMP,
			       0x38);

		/* init tssi calibration */
		if (!mt76x2_channel_silent(dev)) {
			struct ieee80211_channel *chan;
			u32 flag = 0;

			chan = dev->mphy.chandef.chan;
			if (chan->band == NL80211_BAND_5GHZ)
				flag |= BIT(0);
			if (mt76x02_ext_pa_enabled(dev, chan->band))
				flag |= BIT(8);
			mt76x02_mcu_calibrate(dev, MCU_CAL_TSSI, flag);
			dev->cal.tssi_cal_done = true;
		}
	}

	ieee80211_queue_delayed_work(mt76_hw(dev), &dev->cal_work,
				     MT_CALIBRATE_INTERVAL);
	return 0;
}
