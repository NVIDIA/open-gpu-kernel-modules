// SPDX-License-Identifier: ISC
/*
 * Copyright (C) 2016 Felix Fietkau <nbd@nbd.name>
 * Copyright (C) 2018 Lorenzo Bianconi <lorenzo.bianconi83@gmail.com>
 */

#include "mt76x2.h"
#include "eeprom.h"
#include "../mt76x02_phy.h"

static void
mt76x2_set_wlan_state(struct mt76x02_dev *dev, bool enable)
{
	u32 val = mt76_rr(dev, MT_WLAN_FUN_CTRL);

	if (enable)
		val |= (MT_WLAN_FUN_CTRL_WLAN_EN |
			MT_WLAN_FUN_CTRL_WLAN_CLK_EN);
	else
		val &= ~(MT_WLAN_FUN_CTRL_WLAN_EN |
			 MT_WLAN_FUN_CTRL_WLAN_CLK_EN);

	mt76_wr(dev, MT_WLAN_FUN_CTRL, val);
	udelay(20);
}

void mt76x2_reset_wlan(struct mt76x02_dev *dev, bool enable)
{
	u32 val;

	if (!enable)
		goto out;

	val = mt76_rr(dev, MT_WLAN_FUN_CTRL);

	val &= ~MT_WLAN_FUN_CTRL_FRC_WL_ANT_SEL;

	if (val & MT_WLAN_FUN_CTRL_WLAN_EN) {
		val |= MT_WLAN_FUN_CTRL_WLAN_RESET_RF;
		mt76_wr(dev, MT_WLAN_FUN_CTRL, val);
		udelay(20);

		val &= ~MT_WLAN_FUN_CTRL_WLAN_RESET_RF;
	}

	mt76_wr(dev, MT_WLAN_FUN_CTRL, val);
	udelay(20);

out:
	mt76x2_set_wlan_state(dev, enable);
}
EXPORT_SYMBOL_GPL(mt76x2_reset_wlan);

void mt76_write_mac_initvals(struct mt76x02_dev *dev)
{
#define DEFAULT_PROT_CFG_CCK				\
	(FIELD_PREP(MT_PROT_CFG_RATE, 0x3) |		\
	 FIELD_PREP(MT_PROT_CFG_NAV, 1) |		\
	 FIELD_PREP(MT_PROT_CFG_TXOP_ALLOW, 0x3f) |	\
	 MT_PROT_CFG_RTS_THRESH)

#define DEFAULT_PROT_CFG_OFDM				\
	(FIELD_PREP(MT_PROT_CFG_RATE, 0x2004) |		\
	 FIELD_PREP(MT_PROT_CFG_NAV, 1) |			\
	 FIELD_PREP(MT_PROT_CFG_TXOP_ALLOW, 0x3f) |	\
	 MT_PROT_CFG_RTS_THRESH)

#define DEFAULT_PROT_CFG_20				\
	(FIELD_PREP(MT_PROT_CFG_RATE, 0x2004) |		\
	 FIELD_PREP(MT_PROT_CFG_CTRL, 1) |		\
	 FIELD_PREP(MT_PROT_CFG_NAV, 1) |			\
	 FIELD_PREP(MT_PROT_CFG_TXOP_ALLOW, 0x17))

#define DEFAULT_PROT_CFG_40				\
	(FIELD_PREP(MT_PROT_CFG_RATE, 0x2084) |		\
	 FIELD_PREP(MT_PROT_CFG_CTRL, 1) |		\
	 FIELD_PREP(MT_PROT_CFG_NAV, 1) |			\
	 FIELD_PREP(MT_PROT_CFG_TXOP_ALLOW, 0x3f))

	static const struct mt76_reg_pair vals[] = {
		/* Copied from MediaTek reference source */
		{ MT_PBF_SYS_CTRL,		0x00080c00 },
		{ MT_PBF_CFG,			0x1efebcff },
		{ MT_FCE_PSE_CTRL,		0x00000001 },
		{ MT_MAC_SYS_CTRL,		0x00000000 },
		{ MT_MAX_LEN_CFG,		0x003e3f00 },
		{ MT_AMPDU_MAX_LEN_20M1S,	0xaaa99887 },
		{ MT_AMPDU_MAX_LEN_20M2S,	0x000000aa },
		{ MT_XIFS_TIME_CFG,		0x33a40d0a },
		{ MT_BKOFF_SLOT_CFG,		0x00000209 },
		{ MT_TBTT_SYNC_CFG,		0x00422010 },
		{ MT_PWR_PIN_CFG,		0x00000000 },
		{ 0x1238,			0x001700c8 },
		{ MT_TX_SW_CFG0,		0x00101001 },
		{ MT_TX_SW_CFG1,		0x00010000 },
		{ MT_TX_SW_CFG2,		0x00000000 },
		{ MT_TXOP_CTRL_CFG,		0x0400583f },
		{ MT_TX_RTS_CFG,		0x00ffff20 },
		{ MT_TX_TIMEOUT_CFG,		0x000a2290 },
		{ MT_TX_RETRY_CFG,		0x47f01f0f },
		{ MT_EXP_ACK_TIME,		0x002c00dc },
		{ MT_TX_PROT_CFG6,		0xe3f42004 },
		{ MT_TX_PROT_CFG7,		0xe3f42084 },
		{ MT_TX_PROT_CFG8,		0xe3f42104 },
		{ MT_PIFS_TX_CFG,		0x00060fff },
		{ MT_RX_FILTR_CFG,		0x00015f97 },
		{ MT_LEGACY_BASIC_RATE,		0x0000017f },
		{ MT_HT_BASIC_RATE,		0x00004003 },
		{ MT_PN_PAD_MODE,		0x00000003 },
		{ MT_TXOP_HLDR_ET,		0x00000002 },
		{ 0xa44,			0x00000000 },
		{ MT_HEADER_TRANS_CTRL_REG,	0x00000000 },
		{ MT_TSO_CTRL,			0x00000000 },
		{ MT_AUX_CLK_CFG,		0x00000000 },
		{ MT_DACCLK_EN_DLY_CFG,		0x00000000 },
		{ MT_TX_ALC_CFG_4,		0x00000000 },
		{ MT_TX_ALC_VGA3,		0x00000000 },
		{ MT_TX_PWR_CFG_0,		0x3a3a3a3a },
		{ MT_TX_PWR_CFG_1,		0x3a3a3a3a },
		{ MT_TX_PWR_CFG_2,		0x3a3a3a3a },
		{ MT_TX_PWR_CFG_3,		0x3a3a3a3a },
		{ MT_TX_PWR_CFG_4,		0x3a3a3a3a },
		{ MT_TX_PWR_CFG_7,		0x3a3a3a3a },
		{ MT_TX_PWR_CFG_8,		0x0000003a },
		{ MT_TX_PWR_CFG_9,		0x0000003a },
		{ MT_EFUSE_CTRL,		0x0000d000 },
		{ MT_PAUSE_ENABLE_CONTROL1,	0x0000000a },
		{ MT_FCE_WLAN_FLOW_CONTROL1,	0x60401c18 },
		{ MT_WPDMA_DELAY_INT_CFG,	0x94ff0000 },
		{ MT_TX_SW_CFG3,		0x00000004 },
		{ MT_HT_FBK_TO_LEGACY,		0x00001818 },
		{ MT_VHT_HT_FBK_CFG1,		0xedcba980 },
		{ MT_PROT_AUTO_TX_CFG,		0x00830083 },
		{ MT_HT_CTRL_CFG,		0x000001ff },
		{ MT_TX_LINK_CFG,		0x00001020 },
	};
	struct mt76_reg_pair prot_vals[] = {
		{ MT_CCK_PROT_CFG,		DEFAULT_PROT_CFG_CCK },
		{ MT_OFDM_PROT_CFG,		DEFAULT_PROT_CFG_OFDM },
		{ MT_MM20_PROT_CFG,		DEFAULT_PROT_CFG_20 },
		{ MT_MM40_PROT_CFG,		DEFAULT_PROT_CFG_40 },
		{ MT_GF20_PROT_CFG,		DEFAULT_PROT_CFG_20 },
		{ MT_GF40_PROT_CFG,		DEFAULT_PROT_CFG_40 },
	};

	mt76_wr_rp(dev, 0, vals, ARRAY_SIZE(vals));
	mt76_wr_rp(dev, 0, prot_vals, ARRAY_SIZE(prot_vals));
}
EXPORT_SYMBOL_GPL(mt76_write_mac_initvals);

void mt76x2_init_txpower(struct mt76x02_dev *dev,
			 struct ieee80211_supported_band *sband)
{
	struct ieee80211_channel *chan;
	struct mt76x2_tx_power_info txp;
	struct mt76_rate_power t = {};
	int i;

	for (i = 0; i < sband->n_channels; i++) {
		chan = &sband->channels[i];

		mt76x2_get_power_info(dev, &txp, chan);
		mt76x2_get_rate_power(dev, &t, chan);

		chan->orig_mpwr = mt76x02_get_max_rate_power(&t) +
				  txp.target_power;
		chan->orig_mpwr = DIV_ROUND_UP(chan->orig_mpwr, 2);

		/* convert to combined output power on 2x2 devices */
		chan->orig_mpwr += 3;
		chan->max_power = min_t(int, chan->max_reg_power,
					chan->orig_mpwr);
	}
}
EXPORT_SYMBOL_GPL(mt76x2_init_txpower);
