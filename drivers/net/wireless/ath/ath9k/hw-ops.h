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

#ifndef ATH9K_HW_OPS_H
#define ATH9K_HW_OPS_H

#include "hw.h"

/* Hardware core and driver accessible callbacks */

static inline void ath9k_hw_configpcipowersave(struct ath_hw *ah,
					       bool power_off)
{
	if (!ah->aspm_enabled)
		return;

	ath9k_hw_ops(ah)->config_pci_powersave(ah, power_off);
}

static inline void ath9k_hw_rxena(struct ath_hw *ah)
{
	ath9k_hw_ops(ah)->rx_enable(ah);
}

static inline void ath9k_hw_set_desc_link(struct ath_hw *ah, void *ds,
					  u32 link)
{
	ath9k_hw_ops(ah)->set_desc_link(ds, link);
}

static inline int ath9k_hw_calibrate(struct ath_hw *ah,
				     struct ath9k_channel *chan,
				     u8 rxchainmask, bool longcal)
{
	return ath9k_hw_ops(ah)->calibrate(ah, chan, rxchainmask, longcal);
}

static inline bool ath9k_hw_getisr(struct ath_hw *ah, enum ath9k_int *masked,
				   u32 *sync_cause_p)
{
	return ath9k_hw_ops(ah)->get_isr(ah, masked, sync_cause_p);
}

static inline void ath9k_hw_set_txdesc(struct ath_hw *ah, void *ds,
				       struct ath_tx_info *i)
{
	return ath9k_hw_ops(ah)->set_txdesc(ah, ds, i);
}

static inline int ath9k_hw_txprocdesc(struct ath_hw *ah, void *ds,
				      struct ath_tx_status *ts)
{
	return ath9k_hw_ops(ah)->proc_txdesc(ah, ds, ts);
}

static inline int ath9k_hw_get_duration(struct ath_hw *ah, const void *ds,
					int index)
{
	return ath9k_hw_ops(ah)->get_duration(ah, ds, index);
}

static inline void ath9k_hw_antdiv_comb_conf_get(struct ath_hw *ah,
		struct ath_hw_antcomb_conf *antconf)
{
	ath9k_hw_ops(ah)->antdiv_comb_conf_get(ah, antconf);
}

static inline void ath9k_hw_antdiv_comb_conf_set(struct ath_hw *ah,
		struct ath_hw_antcomb_conf *antconf)
{
	ath9k_hw_ops(ah)->antdiv_comb_conf_set(ah, antconf);
}

static inline void ath9k_hw_tx99_start(struct ath_hw *ah, u32 qnum)
{
	ath9k_hw_ops(ah)->tx99_start(ah, qnum);
}

static inline void ath9k_hw_tx99_stop(struct ath_hw *ah)
{
	ath9k_hw_ops(ah)->tx99_stop(ah);
}

static inline void ath9k_hw_tx99_set_txpower(struct ath_hw *ah, u8 power)
{
	if (ath9k_hw_ops(ah)->tx99_set_txpower)
		ath9k_hw_ops(ah)->tx99_set_txpower(ah, power);
}

#ifdef CONFIG_ATH9K_BTCOEX_SUPPORT

static inline void ath9k_hw_set_bt_ant_diversity(struct ath_hw *ah, bool enable)
{
	if (ath9k_hw_ops(ah)->set_bt_ant_diversity)
		ath9k_hw_ops(ah)->set_bt_ant_diversity(ah, enable);
}

static inline bool ath9k_hw_is_aic_enabled(struct ath_hw *ah)
{
	if (ath9k_hw_private_ops(ah)->is_aic_enabled)
		return ath9k_hw_private_ops(ah)->is_aic_enabled(ah);

	return false;
}

#endif

/* Private hardware call ops */

static inline void ath9k_hw_init_hang_checks(struct ath_hw *ah)
{
	ath9k_hw_private_ops(ah)->init_hang_checks(ah);
}

static inline bool ath9k_hw_detect_mac_hang(struct ath_hw *ah)
{
	return ath9k_hw_private_ops(ah)->detect_mac_hang(ah);
}

static inline bool ath9k_hw_detect_bb_hang(struct ath_hw *ah)
{
	return ath9k_hw_private_ops(ah)->detect_bb_hang(ah);
}

/* PHY ops */

static inline int ath9k_hw_rf_set_freq(struct ath_hw *ah,
				       struct ath9k_channel *chan)
{
	return ath9k_hw_private_ops(ah)->rf_set_freq(ah, chan);
}

static inline void ath9k_hw_spur_mitigate_freq(struct ath_hw *ah,
					       struct ath9k_channel *chan)
{
	ath9k_hw_private_ops(ah)->spur_mitigate_freq(ah, chan);
}

static inline bool ath9k_hw_set_rf_regs(struct ath_hw *ah,
					struct ath9k_channel *chan,
					u16 modesIndex)
{
	if (!ath9k_hw_private_ops(ah)->set_rf_regs)
		return true;

	return ath9k_hw_private_ops(ah)->set_rf_regs(ah, chan, modesIndex);
}

static inline void ath9k_hw_init_bb(struct ath_hw *ah,
				    struct ath9k_channel *chan)
{
	return ath9k_hw_private_ops(ah)->init_bb(ah, chan);
}

static inline void ath9k_hw_set_channel_regs(struct ath_hw *ah,
					     struct ath9k_channel *chan)
{
	return ath9k_hw_private_ops(ah)->set_channel_regs(ah, chan);
}

static inline int ath9k_hw_process_ini(struct ath_hw *ah,
					struct ath9k_channel *chan)
{
	return ath9k_hw_private_ops(ah)->process_ini(ah, chan);
}

static inline void ath9k_olc_init(struct ath_hw *ah)
{
	if (!ath9k_hw_private_ops(ah)->olc_init)
		return;

	return ath9k_hw_private_ops(ah)->olc_init(ah);
}

static inline void ath9k_hw_set_rfmode(struct ath_hw *ah,
				       struct ath9k_channel *chan)
{
	return ath9k_hw_private_ops(ah)->set_rfmode(ah, chan);
}

static inline void ath9k_hw_mark_phy_inactive(struct ath_hw *ah)
{
	return ath9k_hw_private_ops(ah)->mark_phy_inactive(ah);
}

static inline void ath9k_hw_set_delta_slope(struct ath_hw *ah,
					    struct ath9k_channel *chan)
{
	return ath9k_hw_private_ops(ah)->set_delta_slope(ah, chan);
}

static inline bool ath9k_hw_rfbus_req(struct ath_hw *ah)
{
	return ath9k_hw_private_ops(ah)->rfbus_req(ah);
}

static inline void ath9k_hw_rfbus_done(struct ath_hw *ah)
{
	return ath9k_hw_private_ops(ah)->rfbus_done(ah);
}

static inline void ath9k_hw_restore_chainmask(struct ath_hw *ah)
{
	if (!ath9k_hw_private_ops(ah)->restore_chainmask)
		return;

	return ath9k_hw_private_ops(ah)->restore_chainmask(ah);
}

static inline bool ath9k_hw_ani_control(struct ath_hw *ah,
					enum ath9k_ani_cmd cmd, int param)
{
	return ath9k_hw_private_ops(ah)->ani_control(ah, cmd, param);
}

static inline void ath9k_hw_do_getnf(struct ath_hw *ah,
				     int16_t nfarray[NUM_NF_READINGS])
{
	ath9k_hw_private_ops(ah)->do_getnf(ah, nfarray);
}

static inline bool ath9k_hw_init_cal(struct ath_hw *ah,
				     struct ath9k_channel *chan)
{
	return ath9k_hw_private_ops(ah)->init_cal(ah, chan);
}

static inline void ath9k_hw_setup_calibration(struct ath_hw *ah,
					      struct ath9k_cal_list *currCal)
{
	ath9k_hw_private_ops(ah)->setup_calibration(ah, currCal);
}

static inline int ath9k_hw_fast_chan_change(struct ath_hw *ah,
					    struct ath9k_channel *chan,
					    u8 *ini_reloaded)
{
	return ath9k_hw_private_ops(ah)->fast_chan_change(ah, chan,
							  ini_reloaded);
}

static inline void ath9k_hw_set_radar_params(struct ath_hw *ah)
{
	if (!ath9k_hw_private_ops(ah)->set_radar_params)
		return;

	ath9k_hw_private_ops(ah)->set_radar_params(ah, &ah->radar_conf);
}

static inline void ath9k_hw_init_cal_settings(struct ath_hw *ah)
{
	ath9k_hw_private_ops(ah)->init_cal_settings(ah);
}

static inline u32 ath9k_hw_compute_pll_control(struct ath_hw *ah,
					       struct ath9k_channel *chan)
{
	return ath9k_hw_private_ops(ah)->compute_pll_control(ah, chan);
}

static inline void ath9k_hw_init_mode_gain_regs(struct ath_hw *ah)
{
	if (!ath9k_hw_private_ops(ah)->init_mode_gain_regs)
		return;

	ath9k_hw_private_ops(ah)->init_mode_gain_regs(ah);
}

static inline void ath9k_hw_ani_cache_ini_regs(struct ath_hw *ah)
{
	if (!ath9k_hw_private_ops(ah)->ani_cache_ini_regs)
		return;

	ath9k_hw_private_ops(ah)->ani_cache_ini_regs(ah);
}

#endif /* ATH9K_HW_OPS_H */
