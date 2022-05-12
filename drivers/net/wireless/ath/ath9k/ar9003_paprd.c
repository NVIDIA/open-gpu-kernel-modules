/*
 * Copyright (c) 2010-2011 Atheros Communications Inc.
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

#include <linux/export.h>
#include "hw.h"
#include "ar9003_phy.h"

void ar9003_paprd_enable(struct ath_hw *ah, bool val)
{
	struct ath9k_channel *chan = ah->curchan;
	struct ar9300_eeprom *eep = &ah->eeprom.ar9300_eep;

	/*
	 * 3 bits for modalHeader5G.papdRateMaskHt20
	 * is used for sub-band disabling of PAPRD.
	 * 5G band is divided into 3 sub-bands -- upper,
	 * middle, lower.
	 * if bit 30 of modalHeader5G.papdRateMaskHt20 is set
	 * -- disable PAPRD for upper band 5GHz
	 * if bit 29 of modalHeader5G.papdRateMaskHt20 is set
	 * -- disable PAPRD for middle band 5GHz
	 * if bit 28 of modalHeader5G.papdRateMaskHt20 is set
	 * -- disable PAPRD for lower band 5GHz
	 */

	if (IS_CHAN_5GHZ(chan)) {
		if (chan->channel >= UPPER_5G_SUB_BAND_START) {
			if (le32_to_cpu(eep->modalHeader5G.papdRateMaskHt20)
								  & BIT(30))
				val = false;
		} else if (chan->channel >= MID_5G_SUB_BAND_START) {
			if (le32_to_cpu(eep->modalHeader5G.papdRateMaskHt20)
								  & BIT(29))
				val = false;
		} else {
			if (le32_to_cpu(eep->modalHeader5G.papdRateMaskHt20)
								  & BIT(28))
				val = false;
		}
	}

	if (val) {
		ah->paprd_table_write_done = true;
		ath9k_hw_apply_txpower(ah, chan, false);
	}

	REG_RMW_FIELD(ah, AR_PHY_PAPRD_CTRL0_B0,
		      AR_PHY_PAPRD_CTRL0_PAPRD_ENABLE, !!val);
	if (ah->caps.tx_chainmask & BIT(1))
		REG_RMW_FIELD(ah, AR_PHY_PAPRD_CTRL0_B1,
			      AR_PHY_PAPRD_CTRL0_PAPRD_ENABLE, !!val);
	if (ah->caps.tx_chainmask & BIT(2))
		REG_RMW_FIELD(ah, AR_PHY_PAPRD_CTRL0_B2,
			      AR_PHY_PAPRD_CTRL0_PAPRD_ENABLE, !!val);
}
EXPORT_SYMBOL(ar9003_paprd_enable);

static int ar9003_get_training_power_2g(struct ath_hw *ah)
{
	struct ath9k_channel *chan = ah->curchan;
	unsigned int power, scale, delta;

	scale = ar9003_get_paprd_scale_factor(ah, chan);

	if (AR_SREV_9330(ah) || AR_SREV_9340(ah) ||
	    AR_SREV_9462(ah) || AR_SREV_9565(ah)) {
		power = ah->paprd_target_power + 2;
	} else if (AR_SREV_9485(ah)) {
		power = 25;
	} else {
		power = REG_READ_FIELD(ah, AR_PHY_POWERTX_RATE5,
				       AR_PHY_POWERTX_RATE5_POWERTXHT20_0);

		delta = abs((int) ah->paprd_target_power - (int) power);
		if (delta > scale)
			return -1;

		if (delta < 4)
			power -= 4 - delta;
	}

	return power;
}

static int ar9003_get_training_power_5g(struct ath_hw *ah)
{
	struct ath_common *common = ath9k_hw_common(ah);
	struct ath9k_channel *chan = ah->curchan;
	unsigned int power, scale, delta;

	scale = ar9003_get_paprd_scale_factor(ah, chan);

	if (IS_CHAN_HT40(chan))
		power = REG_READ_FIELD(ah, AR_PHY_POWERTX_RATE8,
			AR_PHY_POWERTX_RATE8_POWERTXHT40_5);
	else
		power = REG_READ_FIELD(ah, AR_PHY_POWERTX_RATE6,
			AR_PHY_POWERTX_RATE6_POWERTXHT20_5);

	power += scale;
	delta = abs((int) ah->paprd_target_power - (int) power);
	if (delta > scale)
		return -1;

	switch (get_streams(ah->txchainmask)) {
	case 1:
		delta = 6;
		break;
	case 2:
		delta = 4;
		break;
	case 3:
		delta = 2;
		break;
	default:
		delta = 0;
		ath_dbg(common, CALIBRATE, "Invalid tx-chainmask: %u\n",
			ah->txchainmask);
	}

	power += delta;
	return power;
}

static int ar9003_paprd_setup_single_table(struct ath_hw *ah)
{
	struct ath_common *common = ath9k_hw_common(ah);
	static const u32 ctrl0[3] = {
		AR_PHY_PAPRD_CTRL0_B0,
		AR_PHY_PAPRD_CTRL0_B1,
		AR_PHY_PAPRD_CTRL0_B2
	};
	static const u32 ctrl1[3] = {
		AR_PHY_PAPRD_CTRL1_B0,
		AR_PHY_PAPRD_CTRL1_B1,
		AR_PHY_PAPRD_CTRL1_B2
	};
	int training_power;
	int i, val;
	u32 am2pm_mask = ah->paprd_ratemask;

	if (IS_CHAN_2GHZ(ah->curchan))
		training_power = ar9003_get_training_power_2g(ah);
	else
		training_power = ar9003_get_training_power_5g(ah);

	ath_dbg(common, CALIBRATE, "Training power: %d, Target power: %d\n",
		training_power, ah->paprd_target_power);

	if (training_power < 0) {
		ath_dbg(common, CALIBRATE,
			"PAPRD target power delta out of range\n");
		return -ERANGE;
	}
	ah->paprd_training_power = training_power;

	if (AR_SREV_9330(ah))
		am2pm_mask = 0;

	REG_RMW_FIELD(ah, AR_PHY_PAPRD_AM2AM, AR_PHY_PAPRD_AM2AM_MASK,
		      ah->paprd_ratemask);
	REG_RMW_FIELD(ah, AR_PHY_PAPRD_AM2PM, AR_PHY_PAPRD_AM2PM_MASK,
		      am2pm_mask);
	REG_RMW_FIELD(ah, AR_PHY_PAPRD_HT40, AR_PHY_PAPRD_HT40_MASK,
		      ah->paprd_ratemask_ht40);

	ath_dbg(common, CALIBRATE, "PAPRD HT20 mask: 0x%x, HT40 mask: 0x%x\n",
		ah->paprd_ratemask, ah->paprd_ratemask_ht40);

	for (i = 0; i < ah->caps.max_txchains; i++) {
		REG_RMW_FIELD(ah, ctrl0[i],
			      AR_PHY_PAPRD_CTRL0_USE_SINGLE_TABLE_MASK, 1);
		REG_RMW_FIELD(ah, ctrl1[i],
			      AR_PHY_PAPRD_CTRL1_ADAPTIVE_AM2PM_ENABLE, 1);
		REG_RMW_FIELD(ah, ctrl1[i],
			      AR_PHY_PAPRD_CTRL1_ADAPTIVE_AM2AM_ENABLE, 1);
		REG_RMW_FIELD(ah, ctrl1[i],
			      AR_PHY_PAPRD_CTRL1_ADAPTIVE_SCALING_ENA, 0);
		REG_RMW_FIELD(ah, ctrl1[i],
			      AR_PHY_PAPRD_CTRL1_PA_GAIN_SCALE_FACT_MASK, 181);
		REG_RMW_FIELD(ah, ctrl1[i],
			      AR_PHY_PAPRD_CTRL1_PAPRD_MAG_SCALE_FACT, 361);
		REG_RMW_FIELD(ah, ctrl1[i],
			      AR_PHY_PAPRD_CTRL1_ADAPTIVE_SCALING_ENA, 0);
		REG_RMW_FIELD(ah, ctrl0[i],
			      AR_PHY_PAPRD_CTRL0_PAPRD_MAG_THRSH, 3);
	}

	ar9003_paprd_enable(ah, false);

	REG_RMW_FIELD(ah, AR_PHY_PAPRD_TRAINER_CNTL1,
		      AR_PHY_PAPRD_TRAINER_CNTL1_CF_PAPRD_LB_SKIP, 0x30);
	REG_RMW_FIELD(ah, AR_PHY_PAPRD_TRAINER_CNTL1,
		      AR_PHY_PAPRD_TRAINER_CNTL1_CF_PAPRD_LB_ENABLE, 1);
	REG_RMW_FIELD(ah, AR_PHY_PAPRD_TRAINER_CNTL1,
		      AR_PHY_PAPRD_TRAINER_CNTL1_CF_PAPRD_TX_GAIN_FORCE, 1);
	REG_RMW_FIELD(ah, AR_PHY_PAPRD_TRAINER_CNTL1,
		      AR_PHY_PAPRD_TRAINER_CNTL1_CF_PAPRD_RX_BB_GAIN_FORCE, 0);
	REG_RMW_FIELD(ah, AR_PHY_PAPRD_TRAINER_CNTL1,
		      AR_PHY_PAPRD_TRAINER_CNTL1_CF_PAPRD_IQCORR_ENABLE, 0);
	REG_RMW_FIELD(ah, AR_PHY_PAPRD_TRAINER_CNTL1,
		      AR_PHY_PAPRD_TRAINER_CNTL1_CF_PAPRD_AGC2_SETTLING, 28);
	REG_RMW_FIELD(ah, AR_PHY_PAPRD_TRAINER_CNTL1,
		      AR_PHY_PAPRD_TRAINER_CNTL1_CF_CF_PAPRD_TRAIN_ENABLE, 1);

	if (AR_SREV_9485(ah)) {
		val = 148;
	} else {
		if (IS_CHAN_2GHZ(ah->curchan)) {
			if (AR_SREV_9462(ah) || AR_SREV_9565(ah))
				val = 145;
			else
				val = 147;
		} else {
			val = 137;
		}
	}

	REG_RMW_FIELD(ah, AR_PHY_PAPRD_TRAINER_CNTL2,
		      AR_PHY_PAPRD_TRAINER_CNTL2_CF_PAPRD_INIT_RX_BB_GAIN, val);
	REG_RMW_FIELD(ah, AR_PHY_PAPRD_TRAINER_CNTL3,
		      AR_PHY_PAPRD_TRAINER_CNTL3_CF_PAPRD_FINE_CORR_LEN, 4);
	REG_RMW_FIELD(ah, AR_PHY_PAPRD_TRAINER_CNTL3,
		      AR_PHY_PAPRD_TRAINER_CNTL3_CF_PAPRD_COARSE_CORR_LEN, 4);
	REG_RMW_FIELD(ah, AR_PHY_PAPRD_TRAINER_CNTL3,
		      AR_PHY_PAPRD_TRAINER_CNTL3_CF_PAPRD_NUM_CORR_STAGES, 7);
	REG_RMW_FIELD(ah, AR_PHY_PAPRD_TRAINER_CNTL3,
		      AR_PHY_PAPRD_TRAINER_CNTL3_CF_PAPRD_MIN_LOOPBACK_DEL, 1);

	if (AR_SREV_9485(ah) ||
	    AR_SREV_9462(ah) ||
	    AR_SREV_9565(ah) ||
	    AR_SREV_9550(ah) ||
	    AR_SREV_9330(ah) ||
	    AR_SREV_9340(ah))
		REG_RMW_FIELD(ah, AR_PHY_PAPRD_TRAINER_CNTL3,
			      AR_PHY_PAPRD_TRAINER_CNTL3_CF_PAPRD_QUICK_DROP, -3);
	else
		REG_RMW_FIELD(ah, AR_PHY_PAPRD_TRAINER_CNTL3,
			      AR_PHY_PAPRD_TRAINER_CNTL3_CF_PAPRD_QUICK_DROP, -6);

	val = -10;

	if (IS_CHAN_2GHZ(ah->curchan) && !AR_SREV_9462(ah) && !AR_SREV_9565(ah))
		val = -15;

	REG_RMW_FIELD(ah, AR_PHY_PAPRD_TRAINER_CNTL3,
		      AR_PHY_PAPRD_TRAINER_CNTL3_CF_PAPRD_ADC_DESIRED_SIZE,
		      val);
	REG_RMW_FIELD(ah, AR_PHY_PAPRD_TRAINER_CNTL3,
		      AR_PHY_PAPRD_TRAINER_CNTL3_CF_PAPRD_BBTXMIX_DISABLE, 1);
	REG_RMW_FIELD(ah, AR_PHY_PAPRD_TRAINER_CNTL4,
		      AR_PHY_PAPRD_TRAINER_CNTL4_CF_PAPRD_SAFETY_DELTA, 0);
	REG_RMW_FIELD(ah, AR_PHY_PAPRD_TRAINER_CNTL4,
		      AR_PHY_PAPRD_TRAINER_CNTL4_CF_PAPRD_MIN_CORR, 400);
	REG_RMW_FIELD(ah, AR_PHY_PAPRD_TRAINER_CNTL4,
		      AR_PHY_PAPRD_TRAINER_CNTL4_CF_PAPRD_NUM_TRAIN_SAMPLES,
		      100);
	REG_RMW_FIELD(ah, AR_PHY_PAPRD_PRE_POST_SCALE_0_B0,
		      AR_PHY_PAPRD_PRE_POST_SCALING, 261376);
	REG_RMW_FIELD(ah, AR_PHY_PAPRD_PRE_POST_SCALE_1_B0,
		      AR_PHY_PAPRD_PRE_POST_SCALING, 248079);
	REG_RMW_FIELD(ah, AR_PHY_PAPRD_PRE_POST_SCALE_2_B0,
		      AR_PHY_PAPRD_PRE_POST_SCALING, 233759);
	REG_RMW_FIELD(ah, AR_PHY_PAPRD_PRE_POST_SCALE_3_B0,
		      AR_PHY_PAPRD_PRE_POST_SCALING, 220464);
	REG_RMW_FIELD(ah, AR_PHY_PAPRD_PRE_POST_SCALE_4_B0,
		      AR_PHY_PAPRD_PRE_POST_SCALING, 208194);
	REG_RMW_FIELD(ah, AR_PHY_PAPRD_PRE_POST_SCALE_5_B0,
		      AR_PHY_PAPRD_PRE_POST_SCALING, 196949);
	REG_RMW_FIELD(ah, AR_PHY_PAPRD_PRE_POST_SCALE_6_B0,
		      AR_PHY_PAPRD_PRE_POST_SCALING, 185706);
	REG_RMW_FIELD(ah, AR_PHY_PAPRD_PRE_POST_SCALE_7_B0,
		      AR_PHY_PAPRD_PRE_POST_SCALING, 175487);
	return 0;
}

static void ar9003_paprd_get_gain_table(struct ath_hw *ah)
{
	u32 *entry = ah->paprd_gain_table_entries;
	u8 *index = ah->paprd_gain_table_index;
	u32 reg = AR_PHY_TXGAIN_TABLE;
	int i;

	for (i = 0; i < PAPRD_GAIN_TABLE_ENTRIES; i++) {
		entry[i] = REG_READ(ah, reg);
		index[i] = (entry[i] >> 24) & 0xff;
		reg += 4;
	}
}

static unsigned int ar9003_get_desired_gain(struct ath_hw *ah, int chain,
					    int target_power)
{
	int olpc_gain_delta = 0, cl_gain_mod;
	int alpha_therm, alpha_volt;
	int therm_cal_value, volt_cal_value;
	int therm_value, volt_value;
	int thermal_gain_corr, voltage_gain_corr;
	int desired_scale, desired_gain = 0;
	u32 reg_olpc  = 0, reg_cl_gain  = 0;

	REG_CLR_BIT(ah, AR_PHY_PAPRD_TRAINER_STAT1,
		    AR_PHY_PAPRD_TRAINER_STAT1_PAPRD_TRAIN_DONE);
	desired_scale = REG_READ_FIELD(ah, AR_PHY_TPC_12,
				       AR_PHY_TPC_12_DESIRED_SCALE_HT40_5);
	alpha_therm = REG_READ_FIELD(ah, AR_PHY_TPC_19,
				     AR_PHY_TPC_19_ALPHA_THERM);
	alpha_volt = REG_READ_FIELD(ah, AR_PHY_TPC_19,
				    AR_PHY_TPC_19_ALPHA_VOLT);
	therm_cal_value = REG_READ_FIELD(ah, AR_PHY_TPC_18,
					 AR_PHY_TPC_18_THERM_CAL_VALUE);
	volt_cal_value = REG_READ_FIELD(ah, AR_PHY_TPC_18,
					AR_PHY_TPC_18_VOLT_CAL_VALUE);
	therm_value = REG_READ_FIELD(ah, AR_PHY_BB_THERM_ADC_4,
				     AR_PHY_BB_THERM_ADC_4_LATEST_THERM_VALUE);
	volt_value = REG_READ_FIELD(ah, AR_PHY_BB_THERM_ADC_4,
				    AR_PHY_BB_THERM_ADC_4_LATEST_VOLT_VALUE);

	switch (chain) {
	case 0:
		reg_olpc = AR_PHY_TPC_11_B0;
		reg_cl_gain = AR_PHY_CL_TAB_0;
		break;
	case 1:
		reg_olpc = AR_PHY_TPC_11_B1;
		reg_cl_gain = AR_PHY_CL_TAB_1;
		break;
	case 2:
		reg_olpc = AR_PHY_TPC_11_B2;
		reg_cl_gain = AR_PHY_CL_TAB_2;
		break;
	default:
		ath_dbg(ath9k_hw_common(ah), CALIBRATE,
			"Invalid chainmask: %d\n", chain);
		break;
	}

	olpc_gain_delta = REG_READ_FIELD(ah, reg_olpc,
					 AR_PHY_TPC_11_OLPC_GAIN_DELTA);
	cl_gain_mod = REG_READ_FIELD(ah, reg_cl_gain,
					 AR_PHY_CL_TAB_CL_GAIN_MOD);

	if (olpc_gain_delta >= 128)
		olpc_gain_delta = olpc_gain_delta - 256;

	thermal_gain_corr = (alpha_therm * (therm_value - therm_cal_value) +
			     (256 / 2)) / 256;
	voltage_gain_corr = (alpha_volt * (volt_value - volt_cal_value) +
			     (128 / 2)) / 128;
	desired_gain = target_power - olpc_gain_delta - thermal_gain_corr -
	    voltage_gain_corr + desired_scale + cl_gain_mod;

	return desired_gain;
}

static void ar9003_tx_force_gain(struct ath_hw *ah, unsigned int gain_index)
{
	int selected_gain_entry, txbb1dbgain, txbb6dbgain, txmxrgain;
	int padrvgnA, padrvgnB, padrvgnC, padrvgnD;
	u32 *gain_table_entries = ah->paprd_gain_table_entries;

	selected_gain_entry = gain_table_entries[gain_index];
	txbb1dbgain = selected_gain_entry & 0x7;
	txbb6dbgain = (selected_gain_entry >> 3) & 0x3;
	txmxrgain = (selected_gain_entry >> 5) & 0xf;
	padrvgnA = (selected_gain_entry >> 9) & 0xf;
	padrvgnB = (selected_gain_entry >> 13) & 0xf;
	padrvgnC = (selected_gain_entry >> 17) & 0xf;
	padrvgnD = (selected_gain_entry >> 21) & 0x3;

	REG_RMW_FIELD(ah, AR_PHY_TX_FORCED_GAIN,
		      AR_PHY_TX_FORCED_GAIN_FORCED_TXBB1DBGAIN, txbb1dbgain);
	REG_RMW_FIELD(ah, AR_PHY_TX_FORCED_GAIN,
		      AR_PHY_TX_FORCED_GAIN_FORCED_TXBB6DBGAIN, txbb6dbgain);
	REG_RMW_FIELD(ah, AR_PHY_TX_FORCED_GAIN,
		      AR_PHY_TX_FORCED_GAIN_FORCED_TXMXRGAIN, txmxrgain);
	REG_RMW_FIELD(ah, AR_PHY_TX_FORCED_GAIN,
		      AR_PHY_TX_FORCED_GAIN_FORCED_PADRVGNA, padrvgnA);
	REG_RMW_FIELD(ah, AR_PHY_TX_FORCED_GAIN,
		      AR_PHY_TX_FORCED_GAIN_FORCED_PADRVGNB, padrvgnB);
	REG_RMW_FIELD(ah, AR_PHY_TX_FORCED_GAIN,
		      AR_PHY_TX_FORCED_GAIN_FORCED_PADRVGNC, padrvgnC);
	REG_RMW_FIELD(ah, AR_PHY_TX_FORCED_GAIN,
		      AR_PHY_TX_FORCED_GAIN_FORCED_PADRVGND, padrvgnD);
	REG_RMW_FIELD(ah, AR_PHY_TX_FORCED_GAIN,
		      AR_PHY_TX_FORCED_GAIN_FORCED_ENABLE_PAL, 0);
	REG_RMW_FIELD(ah, AR_PHY_TX_FORCED_GAIN,
		      AR_PHY_TX_FORCED_GAIN_FORCE_TX_GAIN, 0);
	REG_RMW_FIELD(ah, AR_PHY_TPC_1, AR_PHY_TPC_1_FORCED_DAC_GAIN, 0);
	REG_RMW_FIELD(ah, AR_PHY_TPC_1, AR_PHY_TPC_1_FORCE_DAC_GAIN, 0);
}

static inline int find_expn(int num)
{
	return fls(num) - 1;
}

static inline int find_proper_scale(int expn, int N)
{
	return (expn > N) ? expn - 10 : 0;
}

#define NUM_BIN 23

static bool create_pa_curve(u32 *data_L, u32 *data_U, u32 *pa_table, u16 *gain)
{
	unsigned int thresh_accum_cnt;
	int x_est[NUM_BIN + 1], Y[NUM_BIN + 1], theta[NUM_BIN + 1];
	int PA_in[NUM_BIN + 1];
	int B1_tmp[NUM_BIN + 1], B2_tmp[NUM_BIN + 1];
	unsigned int B1_abs_max, B2_abs_max;
	int max_index, scale_factor;
	int y_est[NUM_BIN + 1];
	int x_est_fxp1_nonlin, x_tilde[NUM_BIN + 1];
	unsigned int x_tilde_abs;
	int G_fxp, Y_intercept, order_x_by_y, M, I, L, sum_y_sqr, sum_y_quad;
	int Q_x, Q_B1, Q_B2, beta_raw, alpha_raw, scale_B;
	int Q_scale_B, Q_beta, Q_alpha, alpha, beta, order_1, order_2;
	int order1_5x, order2_3x, order1_5x_rem, order2_3x_rem;
	int y5, y3, tmp;
	int theta_low_bin = 0;
	int i;

	/* disregard any bin that contains <= 16 samples */
	thresh_accum_cnt = 16;
	scale_factor = 5;
	max_index = 0;
	memset(theta, 0, sizeof(theta));
	memset(x_est, 0, sizeof(x_est));
	memset(Y, 0, sizeof(Y));
	memset(y_est, 0, sizeof(y_est));
	memset(x_tilde, 0, sizeof(x_tilde));

	for (i = 0; i < NUM_BIN; i++) {
		s32 accum_cnt, accum_tx, accum_rx, accum_ang;

		/* number of samples */
		accum_cnt = data_L[i] & 0xffff;

		if (accum_cnt <= thresh_accum_cnt)
			continue;

		max_index++;

		/* sum(tx amplitude) */
		accum_tx = ((data_L[i] >> 16) & 0xffff) |
		    ((data_U[i] & 0x7ff) << 16);

		/* sum(rx amplitude distance to lower bin edge) */
		accum_rx = ((data_U[i] >> 11) & 0x1f) |
		    ((data_L[i + 23] & 0xffff) << 5);

		/* sum(angles) */
		accum_ang = ((data_L[i + 23] >> 16) & 0xffff) |
		    ((data_U[i + 23] & 0x7ff) << 16);

		accum_tx <<= scale_factor;
		accum_rx <<= scale_factor;
		x_est[max_index] =
			(((accum_tx + accum_cnt) / accum_cnt) + 32) >>
			scale_factor;

		Y[max_index] =
			((((accum_rx + accum_cnt) / accum_cnt) + 32) >>
			    scale_factor) +
			(1 << scale_factor) * i + 16;

		if (accum_ang >= (1 << 26))
			accum_ang -= 1 << 27;

		theta[max_index] =
			((accum_ang * (1 << scale_factor)) + accum_cnt) /
			accum_cnt;
	}

	/*
	 * Find average theta of first 5 bin and all of those to same value.
	 * Curve is linear at that range.
	 */
	for (i = 1; i < 6; i++)
		theta_low_bin += theta[i];

	theta_low_bin = theta_low_bin / 5;
	for (i = 1; i < 6; i++)
		theta[i] = theta_low_bin;

	/* Set values at origin */
	theta[0] = theta_low_bin;
	for (i = 0; i <= max_index; i++)
		theta[i] -= theta_low_bin;

	x_est[0] = 0;
	Y[0] = 0;
	scale_factor = 8;

	/* low signal gain */
	if (x_est[6] == x_est[3])
		return false;

	G_fxp =
	    (((Y[6] - Y[3]) * 1 << scale_factor) +
	     (x_est[6] - x_est[3])) / (x_est[6] - x_est[3]);

	/* prevent division by zero */
	if (G_fxp == 0)
		return false;

	Y_intercept =
	    (G_fxp * (x_est[0] - x_est[3]) +
	     (1 << scale_factor)) / (1 << scale_factor) + Y[3];

	for (i = 0; i <= max_index; i++)
		y_est[i] = Y[i] - Y_intercept;

	for (i = 0; i <= 3; i++) {
		y_est[i] = i * 32;
		x_est[i] = ((y_est[i] * 1 << scale_factor) + G_fxp) / G_fxp;
	}

	if (y_est[max_index] == 0)
		return false;

	x_est_fxp1_nonlin =
	    x_est[max_index] - ((1 << scale_factor) * y_est[max_index] +
				G_fxp) / G_fxp;

	order_x_by_y =
	    (x_est_fxp1_nonlin + y_est[max_index]) / y_est[max_index];

	if (order_x_by_y == 0)
		M = 10;
	else if (order_x_by_y == 1)
		M = 9;
	else
		M = 8;

	I = (max_index > 15) ? 7 : max_index >> 1;
	L = max_index - I;
	scale_factor = 8;
	sum_y_sqr = 0;
	sum_y_quad = 0;
	x_tilde_abs = 0;

	for (i = 0; i <= L; i++) {
		unsigned int y_sqr;
		unsigned int y_quad;
		unsigned int tmp_abs;

		/* prevent division by zero */
		if (y_est[i + I] == 0)
			return false;

		x_est_fxp1_nonlin =
		    x_est[i + I] - ((1 << scale_factor) * y_est[i + I] +
				    G_fxp) / G_fxp;

		x_tilde[i] =
		    (x_est_fxp1_nonlin * (1 << M) + y_est[i + I]) / y_est[i +
									  I];
		x_tilde[i] =
		    (x_tilde[i] * (1 << M) + y_est[i + I]) / y_est[i + I];
		x_tilde[i] =
		    (x_tilde[i] * (1 << M) + y_est[i + I]) / y_est[i + I];
		y_sqr =
		    (y_est[i + I] * y_est[i + I] +
		     (scale_factor * scale_factor)) / (scale_factor *
						       scale_factor);
		tmp_abs = abs(x_tilde[i]);
		if (tmp_abs > x_tilde_abs)
			x_tilde_abs = tmp_abs;

		y_quad = y_sqr * y_sqr;
		sum_y_sqr = sum_y_sqr + y_sqr;
		sum_y_quad = sum_y_quad + y_quad;
		B1_tmp[i] = y_sqr * (L + 1);
		B2_tmp[i] = y_sqr;
	}

	B1_abs_max = 0;
	B2_abs_max = 0;
	for (i = 0; i <= L; i++) {
		int abs_val;

		B1_tmp[i] -= sum_y_sqr;
		B2_tmp[i] = sum_y_quad - sum_y_sqr * B2_tmp[i];

		abs_val = abs(B1_tmp[i]);
		if (abs_val > B1_abs_max)
			B1_abs_max = abs_val;

		abs_val = abs(B2_tmp[i]);
		if (abs_val > B2_abs_max)
			B2_abs_max = abs_val;
	}

	Q_x = find_proper_scale(find_expn(x_tilde_abs), 10);
	Q_B1 = find_proper_scale(find_expn(B1_abs_max), 10);
	Q_B2 = find_proper_scale(find_expn(B2_abs_max), 10);

	beta_raw = 0;
	alpha_raw = 0;
	for (i = 0; i <= L; i++) {
		x_tilde[i] = x_tilde[i] / (1 << Q_x);
		B1_tmp[i] = B1_tmp[i] / (1 << Q_B1);
		B2_tmp[i] = B2_tmp[i] / (1 << Q_B2);
		beta_raw = beta_raw + B1_tmp[i] * x_tilde[i];
		alpha_raw = alpha_raw + B2_tmp[i] * x_tilde[i];
	}

	scale_B =
	    ((sum_y_quad / scale_factor) * (L + 1) -
	     (sum_y_sqr / scale_factor) * sum_y_sqr) * scale_factor;

	Q_scale_B = find_proper_scale(find_expn(abs(scale_B)), 10);
	scale_B = scale_B / (1 << Q_scale_B);
	if (scale_B == 0)
		return false;
	Q_beta = find_proper_scale(find_expn(abs(beta_raw)), 10);
	Q_alpha = find_proper_scale(find_expn(abs(alpha_raw)), 10);
	beta_raw = beta_raw / (1 << Q_beta);
	alpha_raw = alpha_raw / (1 << Q_alpha);
	alpha = (alpha_raw << 10) / scale_B;
	beta = (beta_raw << 10) / scale_B;
	order_1 = 3 * M - Q_x - Q_B1 - Q_beta + 10 + Q_scale_B;
	order_2 = 3 * M - Q_x - Q_B2 - Q_alpha + 10 + Q_scale_B;
	order1_5x = order_1 / 5;
	order2_3x = order_2 / 3;
	order1_5x_rem = order_1 - 5 * order1_5x;
	order2_3x_rem = order_2 - 3 * order2_3x;

	for (i = 0; i < PAPRD_TABLE_SZ; i++) {
		tmp = i * 32;
		y5 = ((beta * tmp) >> 6) >> order1_5x;
		y5 = (y5 * tmp) >> order1_5x;
		y5 = (y5 * tmp) >> order1_5x;
		y5 = (y5 * tmp) >> order1_5x;
		y5 = (y5 * tmp) >> order1_5x;
		y5 = y5 >> order1_5x_rem;
		y3 = (alpha * tmp) >> order2_3x;
		y3 = (y3 * tmp) >> order2_3x;
		y3 = (y3 * tmp) >> order2_3x;
		y3 = y3 >> order2_3x_rem;
		PA_in[i] = y5 + y3 + (256 * tmp) / G_fxp;

		if (i >= 2) {
			tmp = PA_in[i] - PA_in[i - 1];
			if (tmp < 0)
				PA_in[i] =
				    PA_in[i - 1] + (PA_in[i - 1] -
						    PA_in[i - 2]);
		}

		PA_in[i] = (PA_in[i] < 1400) ? PA_in[i] : 1400;
	}

	beta_raw = 0;
	alpha_raw = 0;

	for (i = 0; i <= L; i++) {
		int theta_tilde =
		    ((theta[i + I] << M) + y_est[i + I]) / y_est[i + I];
		theta_tilde =
		    ((theta_tilde << M) + y_est[i + I]) / y_est[i + I];
		theta_tilde =
		    ((theta_tilde << M) + y_est[i + I]) / y_est[i + I];
		beta_raw = beta_raw + B1_tmp[i] * theta_tilde;
		alpha_raw = alpha_raw + B2_tmp[i] * theta_tilde;
	}

	Q_beta = find_proper_scale(find_expn(abs(beta_raw)), 10);
	Q_alpha = find_proper_scale(find_expn(abs(alpha_raw)), 10);
	beta_raw = beta_raw / (1 << Q_beta);
	alpha_raw = alpha_raw / (1 << Q_alpha);

	alpha = (alpha_raw << 10) / scale_B;
	beta = (beta_raw << 10) / scale_B;
	order_1 = 3 * M - Q_x - Q_B1 - Q_beta + 10 + Q_scale_B + 5;
	order_2 = 3 * M - Q_x - Q_B2 - Q_alpha + 10 + Q_scale_B + 5;
	order1_5x = order_1 / 5;
	order2_3x = order_2 / 3;
	order1_5x_rem = order_1 - 5 * order1_5x;
	order2_3x_rem = order_2 - 3 * order2_3x;

	for (i = 0; i < PAPRD_TABLE_SZ; i++) {
		int PA_angle;

		/* pa_table[4] is calculated from PA_angle for i=5 */
		if (i == 4)
			continue;

		tmp = i * 32;
		if (beta > 0)
			y5 = (((beta * tmp - 64) >> 6) -
			      (1 << order1_5x)) / (1 << order1_5x);
		else
			y5 = ((((beta * tmp - 64) >> 6) +
			       (1 << order1_5x)) / (1 << order1_5x));

		y5 = (y5 * tmp) / (1 << order1_5x);
		y5 = (y5 * tmp) / (1 << order1_5x);
		y5 = (y5 * tmp) / (1 << order1_5x);
		y5 = (y5 * tmp) / (1 << order1_5x);
		y5 = y5 / (1 << order1_5x_rem);

		if (beta > 0)
			y3 = (alpha * tmp -
			      (1 << order2_3x)) / (1 << order2_3x);
		else
			y3 = (alpha * tmp +
			      (1 << order2_3x)) / (1 << order2_3x);
		y3 = (y3 * tmp) / (1 << order2_3x);
		y3 = (y3 * tmp) / (1 << order2_3x);
		y3 = y3 / (1 << order2_3x_rem);

		if (i < 4) {
			PA_angle = 0;
		} else {
			PA_angle = y5 + y3;
			if (PA_angle < -150)
				PA_angle = -150;
			else if (PA_angle > 150)
				PA_angle = 150;
		}

		pa_table[i] = ((PA_in[i] & 0x7ff) << 11) + (PA_angle & 0x7ff);
		if (i == 5) {
			PA_angle = (PA_angle + 2) >> 1;
			pa_table[i - 1] = ((PA_in[i - 1] & 0x7ff) << 11) +
			    (PA_angle & 0x7ff);
		}
	}

	*gain = G_fxp;
	return true;
}

void ar9003_paprd_populate_single_table(struct ath_hw *ah,
					struct ath9k_hw_cal_data *caldata,
					int chain)
{
	u32 *paprd_table_val = caldata->pa_table[chain];
	u32 small_signal_gain = caldata->small_signal_gain[chain];
	u32 training_power = ah->paprd_training_power;
	u32 reg = 0;
	int i;

	if (chain == 0)
		reg = AR_PHY_PAPRD_MEM_TAB_B0;
	else if (chain == 1)
		reg = AR_PHY_PAPRD_MEM_TAB_B1;
	else if (chain == 2)
		reg = AR_PHY_PAPRD_MEM_TAB_B2;

	for (i = 0; i < PAPRD_TABLE_SZ; i++) {
		REG_WRITE(ah, reg, paprd_table_val[i]);
		reg = reg + 4;
	}

	if (chain == 0)
		reg = AR_PHY_PA_GAIN123_B0;
	else if (chain == 1)
		reg = AR_PHY_PA_GAIN123_B1;
	else
		reg = AR_PHY_PA_GAIN123_B2;

	REG_RMW_FIELD(ah, reg, AR_PHY_PA_GAIN123_PA_GAIN1, small_signal_gain);

	REG_RMW_FIELD(ah, AR_PHY_PAPRD_CTRL1_B0,
		      AR_PHY_PAPRD_CTRL1_PAPRD_POWER_AT_AM2AM_CAL,
		      training_power);

	if (ah->caps.tx_chainmask & BIT(1))
		REG_RMW_FIELD(ah, AR_PHY_PAPRD_CTRL1_B1,
			      AR_PHY_PAPRD_CTRL1_PAPRD_POWER_AT_AM2AM_CAL,
			      training_power);

	if (ah->caps.tx_chainmask & BIT(2))
		/* val AR_PHY_PAPRD_CTRL1_PAPRD_POWER_AT_AM2AM_CAL correct? */
		REG_RMW_FIELD(ah, AR_PHY_PAPRD_CTRL1_B2,
			      AR_PHY_PAPRD_CTRL1_PAPRD_POWER_AT_AM2AM_CAL,
			      training_power);
}
EXPORT_SYMBOL(ar9003_paprd_populate_single_table);

void ar9003_paprd_setup_gain_table(struct ath_hw *ah, int chain)
{
	unsigned int i, desired_gain, gain_index;
	unsigned int train_power = ah->paprd_training_power;

	desired_gain = ar9003_get_desired_gain(ah, chain, train_power);

	gain_index = 0;
	for (i = 0; i < PAPRD_GAIN_TABLE_ENTRIES; i++) {
		if (ah->paprd_gain_table_index[i] >= desired_gain)
			break;
		gain_index++;
	}

	ar9003_tx_force_gain(ah, gain_index);

	REG_CLR_BIT(ah, AR_PHY_PAPRD_TRAINER_STAT1,
			AR_PHY_PAPRD_TRAINER_STAT1_PAPRD_TRAIN_DONE);
}
EXPORT_SYMBOL(ar9003_paprd_setup_gain_table);

static bool ar9003_paprd_retrain_pa_in(struct ath_hw *ah,
				       struct ath9k_hw_cal_data *caldata,
				       int chain)
{
	u32 *pa_in = caldata->pa_table[chain];
	int capdiv_offset, quick_drop_offset;
	int capdiv2g, quick_drop;
	int count = 0;
	int i;

	if (!AR_SREV_9485(ah) && !AR_SREV_9330(ah))
		return false;

	capdiv2g = REG_READ_FIELD(ah, AR_PHY_65NM_CH0_TXRF3,
				  AR_PHY_65NM_CH0_TXRF3_CAPDIV2G);

	quick_drop = REG_READ_FIELD(ah, AR_PHY_PAPRD_TRAINER_CNTL3,
				    AR_PHY_PAPRD_TRAINER_CNTL3_CF_PAPRD_QUICK_DROP);

	if (quick_drop)
		quick_drop -= 0x40;

	for (i = 0; i < NUM_BIN + 1; i++) {
		if (pa_in[i] == 1400)
			count++;
	}

	if (AR_SREV_9485(ah)) {
		if (pa_in[23] < 800) {
			capdiv_offset = (int)((1000 - pa_in[23] + 75) / 150);
			capdiv2g += capdiv_offset;
			if (capdiv2g > 7) {
				capdiv2g = 7;
				if (pa_in[23] < 600) {
					quick_drop++;
					if (quick_drop > 0)
						quick_drop = 0;
				}
			}
		} else if (pa_in[23] == 1400) {
			quick_drop_offset = min_t(int, count / 3, 2);
			quick_drop += quick_drop_offset;
			capdiv2g += quick_drop_offset / 2;

			if (capdiv2g > 7)
				capdiv2g = 7;

			if (quick_drop > 0) {
				quick_drop = 0;
				capdiv2g -= quick_drop_offset;
				if (capdiv2g < 0)
					capdiv2g = 0;
			}
		} else {
			return false;
		}
	} else if (AR_SREV_9330(ah)) {
		if (pa_in[23] < 1000) {
			capdiv_offset = (1000 - pa_in[23]) / 100;
			capdiv2g += capdiv_offset;
			if (capdiv_offset > 3) {
				capdiv_offset = 1;
				quick_drop--;
			}

			capdiv2g += capdiv_offset;
			if (capdiv2g > 6)
				capdiv2g = 6;
			if (quick_drop < -4)
				quick_drop = -4;
		} else if (pa_in[23] == 1400) {
			if (count > 3) {
				quick_drop++;
				capdiv2g -= count / 4;
				if (quick_drop > -2)
					quick_drop = -2;
			} else {
				capdiv2g--;
			}

			if (capdiv2g < 0)
				capdiv2g = 0;
		} else {
			return false;
		}
	}

	REG_RMW_FIELD(ah, AR_PHY_65NM_CH0_TXRF3,
		      AR_PHY_65NM_CH0_TXRF3_CAPDIV2G, capdiv2g);
	REG_RMW_FIELD(ah, AR_PHY_PAPRD_TRAINER_CNTL3,
		      AR_PHY_PAPRD_TRAINER_CNTL3_CF_PAPRD_QUICK_DROP,
		      quick_drop);

	return true;
}

int ar9003_paprd_create_curve(struct ath_hw *ah,
			      struct ath9k_hw_cal_data *caldata, int chain)
{
	u16 *small_signal_gain = &caldata->small_signal_gain[chain];
	u32 *pa_table = caldata->pa_table[chain];
	u32 *data_L, *data_U;
	int i, status = 0;
	u32 *buf;
	u32 reg;

	memset(caldata->pa_table[chain], 0, sizeof(caldata->pa_table[chain]));

	buf = kmalloc_array(2 * 48, sizeof(u32), GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	data_L = &buf[0];
	data_U = &buf[48];

	REG_CLR_BIT(ah, AR_PHY_CHAN_INFO_MEMORY,
		    AR_PHY_CHAN_INFO_MEMORY_CHANINFOMEM_S2_READ);

	reg = AR_PHY_CHAN_INFO_TAB_0;
	for (i = 0; i < 48; i++)
		data_L[i] = REG_READ(ah, reg + (i << 2));

	REG_SET_BIT(ah, AR_PHY_CHAN_INFO_MEMORY,
		    AR_PHY_CHAN_INFO_MEMORY_CHANINFOMEM_S2_READ);

	for (i = 0; i < 48; i++)
		data_U[i] = REG_READ(ah, reg + (i << 2));

	if (!create_pa_curve(data_L, data_U, pa_table, small_signal_gain))
		status = -2;

	if (ar9003_paprd_retrain_pa_in(ah, caldata, chain))
		status = -EINPROGRESS;

	REG_CLR_BIT(ah, AR_PHY_PAPRD_TRAINER_STAT1,
		    AR_PHY_PAPRD_TRAINER_STAT1_PAPRD_TRAIN_DONE);

	kfree(buf);

	return status;
}
EXPORT_SYMBOL(ar9003_paprd_create_curve);

int ar9003_paprd_init_table(struct ath_hw *ah)
{
	int ret;

	ret = ar9003_paprd_setup_single_table(ah);
	if (ret < 0)
	    return ret;

	ar9003_paprd_get_gain_table(ah);
	return 0;
}
EXPORT_SYMBOL(ar9003_paprd_init_table);

bool ar9003_paprd_is_done(struct ath_hw *ah)
{
	int paprd_done, agc2_pwr;

	paprd_done = REG_READ_FIELD(ah, AR_PHY_PAPRD_TRAINER_STAT1,
				AR_PHY_PAPRD_TRAINER_STAT1_PAPRD_TRAIN_DONE);

	if (AR_SREV_9485(ah))
		goto exit;

	if (paprd_done == 0x1) {
		agc2_pwr = REG_READ_FIELD(ah, AR_PHY_PAPRD_TRAINER_STAT1,
				AR_PHY_PAPRD_TRAINER_STAT1_PAPRD_AGC2_PWR);

		ath_dbg(ath9k_hw_common(ah), CALIBRATE,
			"AGC2_PWR = 0x%x training done = 0x%x\n",
			agc2_pwr, paprd_done);
	/*
	 * agc2_pwr range should not be less than 'IDEAL_AGC2_PWR_CHANGE'
	 * when the training is completely done, otherwise retraining is
	 * done to make sure the value is in ideal range
	 */
		if (agc2_pwr <= PAPRD_IDEAL_AGC2_PWR_RANGE)
			paprd_done = 0;
	}
exit:
	return !!paprd_done;
}
EXPORT_SYMBOL(ar9003_paprd_is_done);

bool ar9003_is_paprd_enabled(struct ath_hw *ah)
{
	if ((ah->caps.hw_caps & ATH9K_HW_CAP_PAPRD) && ah->config.enable_paprd)
		return true;

	return false;
}
EXPORT_SYMBOL(ar9003_is_paprd_enabled);
