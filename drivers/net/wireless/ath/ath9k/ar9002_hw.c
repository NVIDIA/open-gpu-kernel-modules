/*
 * Copyright (c) 2008-2011 Atheros Communications Inc.
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

#include <linux/moduleparam.h>
#include "hw.h"
#include "ar5008_initvals.h"
#include "ar9001_initvals.h"
#include "ar9002_initvals.h"
#include "ar9002_phy.h"

/* General hardware code for the A5008/AR9001/AR9002 hadware families */

static int ar9002_hw_init_mode_regs(struct ath_hw *ah)
{
	if (AR_SREV_9271(ah)) {
		INIT_INI_ARRAY(&ah->iniModes, ar9271Modes_9271);
		INIT_INI_ARRAY(&ah->iniCommon, ar9271Common_9271);
		INIT_INI_ARRAY(&ah->iniModes_9271_ANI_reg, ar9271Modes_9271_ANI_reg);
		return 0;
	}

	INIT_INI_ARRAY(&ah->iniPcieSerdes,
		       ar9280PciePhy_clkreq_always_on_L1_9280);

	if (AR_SREV_9287_11_OR_LATER(ah)) {
		INIT_INI_ARRAY(&ah->iniModes, ar9287Modes_9287_1_1);
		INIT_INI_ARRAY(&ah->iniCommon, ar9287Common_9287_1_1);
	} else if (AR_SREV_9285_12_OR_LATER(ah)) {
		INIT_INI_ARRAY(&ah->iniModes, ar9285Modes_9285_1_2);
		INIT_INI_ARRAY(&ah->iniCommon, ar9285Common_9285_1_2);
	} else if (AR_SREV_9280_20_OR_LATER(ah)) {
		INIT_INI_ARRAY(&ah->iniModes, ar9280Modes_9280_2);
		INIT_INI_ARRAY(&ah->iniCommon, ar9280Common_9280_2);

		INIT_INI_ARRAY(&ah->iniModesFastClock,
			       ar9280Modes_fast_clock_9280_2);
	} else if (AR_SREV_9160_10_OR_LATER(ah)) {
		INIT_INI_ARRAY(&ah->iniModes, ar5416Modes_9160);
		INIT_INI_ARRAY(&ah->iniCommon, ar5416Common_9160);
		if (AR_SREV_9160_11(ah)) {
			INIT_INI_ARRAY(&ah->iniAddac,
				       ar5416Addac_9160_1_1);
		} else {
			INIT_INI_ARRAY(&ah->iniAddac, ar5416Addac_9160);
		}
	} else if (AR_SREV_9100_OR_LATER(ah)) {
		INIT_INI_ARRAY(&ah->iniModes, ar5416Modes_9100);
		INIT_INI_ARRAY(&ah->iniCommon, ar5416Common_9100);
		INIT_INI_ARRAY(&ah->iniAddac, ar5416Addac_9100);
	} else {
		INIT_INI_ARRAY(&ah->iniModes, ar5416Modes);
		INIT_INI_ARRAY(&ah->iniCommon, ar5416Common);
		INIT_INI_ARRAY(&ah->iniAddac, ar5416Addac);
	}

	if (!AR_SREV_9280_20_OR_LATER(ah)) {
		/* Common for AR5416, AR913x, AR9160 */
		INIT_INI_ARRAY(&ah->iniBB_RfGain, ar5416BB_RfGain);

		/* Common for AR913x, AR9160 */
		if (!AR_SREV_5416(ah))
			INIT_INI_ARRAY(&ah->iniBank6, ar5416Bank6TPC_9100);
		else
			INIT_INI_ARRAY(&ah->iniBank6, ar5416Bank6TPC);
	}

	/* iniAddac needs to be modified for these chips */
	if (AR_SREV_9160(ah) || !AR_SREV_5416_22_OR_LATER(ah)) {
		struct ar5416IniArray *addac = &ah->iniAddac;
		u32 size = sizeof(u32) * addac->ia_rows * addac->ia_columns;
		u32 *data;

		data = devm_kzalloc(ah->dev, size, GFP_KERNEL);
		if (!data)
			return -ENOMEM;

		memcpy(data, addac->ia_array, size);
		addac->ia_array = data;

		if (!AR_SREV_5416_22_OR_LATER(ah)) {
			/* override CLKDRV value */
			INI_RA(addac, 31,1) = 0;
		}
	}
	if (AR_SREV_9287_11_OR_LATER(ah)) {
		INIT_INI_ARRAY(&ah->iniCckfirNormal,
		       ar9287Common_normal_cck_fir_coeff_9287_1_1);
		INIT_INI_ARRAY(&ah->iniCckfirJapan2484,
		       ar9287Common_japan_2484_cck_fir_coeff_9287_1_1);
	}
	return 0;
}

static void ar9280_20_hw_init_rxgain_ini(struct ath_hw *ah)
{
	u32 rxgain_type;

	if (ah->eep_ops->get_eeprom_rev(ah) >= AR5416_EEP_MINOR_VER_17) {
		rxgain_type = ah->eep_ops->get_eeprom(ah, EEP_RXGAIN_TYPE);

		if (rxgain_type == AR5416_EEP_RXGAIN_13DB_BACKOFF)
			INIT_INI_ARRAY(&ah->iniModesRxGain,
				       ar9280Modes_backoff_13db_rxgain_9280_2);
		else if (rxgain_type == AR5416_EEP_RXGAIN_23DB_BACKOFF)
			INIT_INI_ARRAY(&ah->iniModesRxGain,
				       ar9280Modes_backoff_23db_rxgain_9280_2);
		else
			INIT_INI_ARRAY(&ah->iniModesRxGain,
				       ar9280Modes_original_rxgain_9280_2);
	} else {
		INIT_INI_ARRAY(&ah->iniModesRxGain,
			       ar9280Modes_original_rxgain_9280_2);
	}
}

static void ar9280_20_hw_init_txgain_ini(struct ath_hw *ah, u32 txgain_type)
{
	if (ah->eep_ops->get_eeprom_rev(ah) >= AR5416_EEP_MINOR_VER_19) {
		if (txgain_type == AR5416_EEP_TXGAIN_HIGH_POWER)
			INIT_INI_ARRAY(&ah->iniModesTxGain,
				       ar9280Modes_high_power_tx_gain_9280_2);
		else
			INIT_INI_ARRAY(&ah->iniModesTxGain,
				       ar9280Modes_original_tx_gain_9280_2);
	} else {
		INIT_INI_ARRAY(&ah->iniModesTxGain,
			       ar9280Modes_original_tx_gain_9280_2);
	}
}

static void ar9271_hw_init_txgain_ini(struct ath_hw *ah, u32 txgain_type)
{
	if (txgain_type == AR5416_EEP_TXGAIN_HIGH_POWER)
		INIT_INI_ARRAY(&ah->iniModesTxGain,
			       ar9271Modes_high_power_tx_gain_9271);
	else
		INIT_INI_ARRAY(&ah->iniModesTxGain,
			       ar9271Modes_normal_power_tx_gain_9271);
}

static void ar9002_hw_init_mode_gain_regs(struct ath_hw *ah)
{
	u32 txgain_type = ah->eep_ops->get_eeprom(ah, EEP_TXGAIN_TYPE);

	if (AR_SREV_9287_11_OR_LATER(ah))
		INIT_INI_ARRAY(&ah->iniModesRxGain,
			       ar9287Modes_rx_gain_9287_1_1);
	else if (AR_SREV_9280_20(ah))
		ar9280_20_hw_init_rxgain_ini(ah);

	if (AR_SREV_9271(ah)) {
		ar9271_hw_init_txgain_ini(ah, txgain_type);
	} else if (AR_SREV_9287_11_OR_LATER(ah)) {
		INIT_INI_ARRAY(&ah->iniModesTxGain,
			       ar9287Modes_tx_gain_9287_1_1);
	} else if (AR_SREV_9280_20(ah)) {
		ar9280_20_hw_init_txgain_ini(ah, txgain_type);
	} else if (AR_SREV_9285_12_OR_LATER(ah)) {
		/* txgain table */
		if (txgain_type == AR5416_EEP_TXGAIN_HIGH_POWER) {
			if (AR_SREV_9285E_20(ah)) {
				INIT_INI_ARRAY(&ah->iniModesTxGain,
					       ar9285Modes_XE2_0_high_power);
			} else {
				INIT_INI_ARRAY(&ah->iniModesTxGain,
					ar9285Modes_high_power_tx_gain_9285_1_2);
			}
		} else {
			if (AR_SREV_9285E_20(ah)) {
				INIT_INI_ARRAY(&ah->iniModesTxGain,
					       ar9285Modes_XE2_0_normal_power);
			} else {
				INIT_INI_ARRAY(&ah->iniModesTxGain,
					ar9285Modes_original_tx_gain_9285_1_2);
			}
		}
	}
}

/*
 * Helper for ASPM support.
 *
 * Disable PLL when in L0s as well as receiver clock when in L1.
 * This power saving option must be enabled through the SerDes.
 *
 * Programming the SerDes must go through the same 288 bit serial shift
 * register as the other analog registers.  Hence the 9 writes.
 */
static void ar9002_hw_configpcipowersave(struct ath_hw *ah,
					 bool power_off)
{
	u8 i;
	u32 val;

	/* Nothing to do on restore for 11N */
	if (!power_off /* !restore */) {
		if (AR_SREV_9280_20_OR_LATER(ah)) {
			/*
			 * AR9280 2.0 or later chips use SerDes values from the
			 * initvals.h initialized depending on chipset during
			 * __ath9k_hw_init()
			 */
			for (i = 0; i < ah->iniPcieSerdes.ia_rows; i++) {
				REG_WRITE(ah, INI_RA(&ah->iniPcieSerdes, i, 0),
					  INI_RA(&ah->iniPcieSerdes, i, 1));
			}
		} else {
			ENABLE_REGWRITE_BUFFER(ah);

			REG_WRITE(ah, AR_PCIE_SERDES, 0x9248fc00);
			REG_WRITE(ah, AR_PCIE_SERDES, 0x24924924);

			/* RX shut off when elecidle is asserted */
			REG_WRITE(ah, AR_PCIE_SERDES, 0x28000039);
			REG_WRITE(ah, AR_PCIE_SERDES, 0x53160824);
			REG_WRITE(ah, AR_PCIE_SERDES, 0xe5980579);

			/*
			 * Ignore ah->ah_config.pcie_clock_req setting for
			 * pre-AR9280 11n
			 */
			REG_WRITE(ah, AR_PCIE_SERDES, 0x001defff);

			REG_WRITE(ah, AR_PCIE_SERDES, 0x1aaabe40);
			REG_WRITE(ah, AR_PCIE_SERDES, 0xbe105554);
			REG_WRITE(ah, AR_PCIE_SERDES, 0x000e3007);

			/* Load the new settings */
			REG_WRITE(ah, AR_PCIE_SERDES2, 0x00000000);

			REGWRITE_BUFFER_FLUSH(ah);
		}

		udelay(1000);
	}

	if (power_off) {
		/* clear bit 19 to disable L1 */
		REG_CLR_BIT(ah, AR_PCIE_PM_CTRL, AR_PCIE_PM_CTRL_ENA);

		val = REG_READ(ah, AR_WA);

		/*
		 * Set PCIe workaround bits
		 * In AR9280 and AR9285, bit 14 in WA register (disable L1)
		 * should only  be set when device enters D3 and be
		 * cleared when device comes back to D0.
		 */
		if (ah->config.pcie_waen) {
			if (ah->config.pcie_waen & AR_WA_D3_L1_DISABLE)
				val |= AR_WA_D3_L1_DISABLE;
		} else {
			if (AR_SREV_9285(ah) || AR_SREV_9271(ah) || AR_SREV_9287(ah)) {
				if (AR9285_WA_DEFAULT & AR_WA_D3_L1_DISABLE)
					val |= AR_WA_D3_L1_DISABLE;
			} else if (AR_SREV_9280(ah)) {
				if (AR9280_WA_DEFAULT & AR_WA_D3_L1_DISABLE)
					val |= AR_WA_D3_L1_DISABLE;
			}
		}

		if (AR_SREV_9280(ah) || AR_SREV_9285(ah) || AR_SREV_9287(ah)) {
			/*
			 * Disable bit 6 and 7 before entering D3 to
			 * prevent system hang.
			 */
			val &= ~(AR_WA_BIT6 | AR_WA_BIT7);
		}

		if (AR_SREV_9280(ah))
			val |= AR_WA_BIT22;

		if (AR_SREV_9285E_20(ah))
			val |= AR_WA_BIT23;

		REG_WRITE(ah, AR_WA, val);
	} else {
		if (ah->config.pcie_waen) {
			val = ah->config.pcie_waen;
			val &= (~AR_WA_D3_L1_DISABLE);
		} else {
			if (AR_SREV_9285(ah) || AR_SREV_9271(ah) || AR_SREV_9287(ah)) {
				val = AR9285_WA_DEFAULT;
				val &= (~AR_WA_D3_L1_DISABLE);
			} else if (AR_SREV_9280(ah)) {
				/*
				 * For AR9280 chips, bit 22 of 0x4004
				 * needs to be set.
				 */
				val = AR9280_WA_DEFAULT;
				val &= (~AR_WA_D3_L1_DISABLE);
			} else {
				val = AR_WA_DEFAULT;
			}
		}

		/* WAR for ASPM system hang */
		if (AR_SREV_9285(ah) || AR_SREV_9287(ah))
			val |= (AR_WA_BIT6 | AR_WA_BIT7);

		if (AR_SREV_9285E_20(ah))
			val |= AR_WA_BIT23;

		REG_WRITE(ah, AR_WA, val);

		/* set bit 19 to allow forcing of pcie core into L1 state */
		REG_SET_BIT(ah, AR_PCIE_PM_CTRL, AR_PCIE_PM_CTRL_ENA);
	}
}

static int ar9002_hw_get_radiorev(struct ath_hw *ah)
{
	u32 val;
	int i;

	ENABLE_REGWRITE_BUFFER(ah);

	REG_WRITE(ah, AR_PHY(0x36), 0x00007058);
	for (i = 0; i < 8; i++)
		REG_WRITE(ah, AR_PHY(0x20), 0x00010000);

	REGWRITE_BUFFER_FLUSH(ah);

	val = (REG_READ(ah, AR_PHY(256)) >> 24) & 0xff;
	val = ((val & 0xf0) >> 4) | ((val & 0x0f) << 4);

	return ath9k_hw_reverse_bits(val, 8);
}

int ar9002_hw_rf_claim(struct ath_hw *ah)
{
	u32 val;

	REG_WRITE(ah, AR_PHY(0), 0x00000007);

	val = ar9002_hw_get_radiorev(ah);
	switch (val & AR_RADIO_SREV_MAJOR) {
	case 0:
		val = AR_RAD5133_SREV_MAJOR;
		break;
	case AR_RAD5133_SREV_MAJOR:
	case AR_RAD5122_SREV_MAJOR:
	case AR_RAD2133_SREV_MAJOR:
	case AR_RAD2122_SREV_MAJOR:
		break;
	default:
		ath_err(ath9k_hw_common(ah),
			"Radio Chip Rev 0x%02X not supported\n",
			val & AR_RADIO_SREV_MAJOR);
		return -EOPNOTSUPP;
	}

	ah->hw_version.analog5GhzRev = val;

	return 0;
}

void ar9002_hw_enable_async_fifo(struct ath_hw *ah)
{
	if (AR_SREV_9287_13_OR_LATER(ah)) {
		REG_SET_BIT(ah, AR_MAC_PCU_ASYNC_FIFO_REG3,
				AR_MAC_PCU_ASYNC_FIFO_REG3_DATAPATH_SEL);
		REG_SET_BIT(ah, AR_PHY_MODE, AR_PHY_MODE_ASYNCFIFO);
		REG_CLR_BIT(ah, AR_MAC_PCU_ASYNC_FIFO_REG3,
				AR_MAC_PCU_ASYNC_FIFO_REG3_SOFT_RESET);
		REG_SET_BIT(ah, AR_MAC_PCU_ASYNC_FIFO_REG3,
				AR_MAC_PCU_ASYNC_FIFO_REG3_SOFT_RESET);
	}
}

static void ar9002_hw_init_hang_checks(struct ath_hw *ah)
{
	if (AR_SREV_9100(ah) || AR_SREV_9160(ah)) {
		ah->config.hw_hang_checks |= HW_BB_RIFS_HANG;
		ah->config.hw_hang_checks |= HW_BB_DFS_HANG;
	}

	if (AR_SREV_9280(ah))
		ah->config.hw_hang_checks |= HW_BB_RX_CLEAR_STUCK_HANG;

	if (AR_SREV_5416(ah) || AR_SREV_9100(ah) || AR_SREV_9160(ah))
		ah->config.hw_hang_checks |= HW_MAC_HANG;
}

/* Sets up the AR5008/AR9001/AR9002 hardware familiy callbacks */
int ar9002_hw_attach_ops(struct ath_hw *ah)
{
	struct ath_hw_private_ops *priv_ops = ath9k_hw_private_ops(ah);
	struct ath_hw_ops *ops = ath9k_hw_ops(ah);
	int ret;

	ret = ar9002_hw_init_mode_regs(ah);
	if (ret)
		return ret;

	priv_ops->init_mode_gain_regs = ar9002_hw_init_mode_gain_regs;
	priv_ops->init_hang_checks = ar9002_hw_init_hang_checks;

	ops->config_pci_powersave = ar9002_hw_configpcipowersave;

	ret = ar5008_hw_attach_phy_ops(ah);
	if (ret)
		return ret;

	if (AR_SREV_9280_20_OR_LATER(ah))
		ar9002_hw_attach_phy_ops(ah);

	ar9002_hw_attach_calib_ops(ah);
	ar9002_hw_attach_mac_ops(ah);
	return 0;
}

void ar9002_hw_load_ani_reg(struct ath_hw *ah, struct ath9k_channel *chan)
{
	u32 modesIndex;
	int i;

	if (IS_CHAN_5GHZ(chan))
		modesIndex = IS_CHAN_HT40(chan) ? 2 : 1;
	else
		modesIndex = IS_CHAN_HT40(chan) ? 3 : 4;

	ENABLE_REGWRITE_BUFFER(ah);

	for (i = 0; i < ah->iniModes_9271_ANI_reg.ia_rows; i++) {
		u32 reg = INI_RA(&ah->iniModes_9271_ANI_reg, i, 0);
		u32 val = INI_RA(&ah->iniModes_9271_ANI_reg, i, modesIndex);
		u32 val_orig;

		if (reg == AR_PHY_CCK_DETECT) {
			val_orig = REG_READ(ah, reg);
			val &= AR_PHY_CCK_DETECT_WEAK_SIG_THR_CCK;
			val_orig &= ~AR_PHY_CCK_DETECT_WEAK_SIG_THR_CCK;

			REG_WRITE(ah, reg, val|val_orig);
		} else
			REG_WRITE(ah, reg, val);
	}

	REGWRITE_BUFFER_FLUSH(ah);
}
