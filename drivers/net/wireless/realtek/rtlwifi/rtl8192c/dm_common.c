// SPDX-License-Identifier: GPL-2.0
/* Copyright(c) 2009-2012  Realtek Corporation.*/

#include <linux/export.h>
#include "dm_common.h"
#include "phy_common.h"
#include "../pci.h"
#include "../base.h"
#include "../core.h"

#define BT_RSSI_STATE_NORMAL_POWER	BIT(0)
#define BT_RSSI_STATE_AMDPU_OFF		BIT(1)
#define BT_RSSI_STATE_SPECIAL_LOW	BIT(2)
#define BT_RSSI_STATE_BG_EDCA_LOW	BIT(3)
#define BT_RSSI_STATE_TXPOWER_LOW	BIT(4)
#define BT_MASK				0x00ffffff

#define RTLPRIV			(struct rtl_priv *)
#define GET_UNDECORATED_AVERAGE_RSSI(_priv)	\
	((RTLPRIV(_priv))->mac80211.opmode == \
			     NL80211_IFTYPE_ADHOC) ?	\
	((RTLPRIV(_priv))->dm.entry_min_undec_sm_pwdb) : \
	((RTLPRIV(_priv))->dm.undec_sm_pwdb)

static const u32 ofdmswing_table[OFDM_TABLE_SIZE] = {
	0x7f8001fe,
	0x788001e2,
	0x71c001c7,
	0x6b8001ae,
	0x65400195,
	0x5fc0017f,
	0x5a400169,
	0x55400155,
	0x50800142,
	0x4c000130,
	0x47c0011f,
	0x43c0010f,
	0x40000100,
	0x3c8000f2,
	0x390000e4,
	0x35c000d7,
	0x32c000cb,
	0x300000c0,
	0x2d4000b5,
	0x2ac000ab,
	0x288000a2,
	0x26000098,
	0x24000090,
	0x22000088,
	0x20000080,
	0x1e400079,
	0x1c800072,
	0x1b00006c,
	0x19800066,
	0x18000060,
	0x16c0005b,
	0x15800056,
	0x14400051,
	0x1300004c,
	0x12000048,
	0x11000044,
	0x10000040,
};

static const u8 cckswing_table_ch1ch13[CCK_TABLE_SIZE][8] = {
	{0x36, 0x35, 0x2e, 0x25, 0x1c, 0x12, 0x09, 0x04},
	{0x33, 0x32, 0x2b, 0x23, 0x1a, 0x11, 0x08, 0x04},
	{0x30, 0x2f, 0x29, 0x21, 0x19, 0x10, 0x08, 0x03},
	{0x2d, 0x2d, 0x27, 0x1f, 0x18, 0x0f, 0x08, 0x03},
	{0x2b, 0x2a, 0x25, 0x1e, 0x16, 0x0e, 0x07, 0x03},
	{0x28, 0x28, 0x22, 0x1c, 0x15, 0x0d, 0x07, 0x03},
	{0x26, 0x25, 0x21, 0x1b, 0x14, 0x0d, 0x06, 0x03},
	{0x24, 0x23, 0x1f, 0x19, 0x13, 0x0c, 0x06, 0x03},
	{0x22, 0x21, 0x1d, 0x18, 0x11, 0x0b, 0x06, 0x02},
	{0x20, 0x20, 0x1b, 0x16, 0x11, 0x08, 0x05, 0x02},
	{0x1f, 0x1e, 0x1a, 0x15, 0x10, 0x0a, 0x05, 0x02},
	{0x1d, 0x1c, 0x18, 0x14, 0x0f, 0x0a, 0x05, 0x02},
	{0x1b, 0x1a, 0x17, 0x13, 0x0e, 0x09, 0x04, 0x02},
	{0x1a, 0x19, 0x16, 0x12, 0x0d, 0x09, 0x04, 0x02},
	{0x18, 0x17, 0x15, 0x11, 0x0c, 0x08, 0x04, 0x02},
	{0x17, 0x16, 0x13, 0x10, 0x0c, 0x08, 0x04, 0x02},
	{0x16, 0x15, 0x12, 0x0f, 0x0b, 0x07, 0x04, 0x01},
	{0x14, 0x14, 0x11, 0x0e, 0x0b, 0x07, 0x03, 0x02},
	{0x13, 0x13, 0x10, 0x0d, 0x0a, 0x06, 0x03, 0x01},
	{0x12, 0x12, 0x0f, 0x0c, 0x09, 0x06, 0x03, 0x01},
	{0x11, 0x11, 0x0f, 0x0c, 0x09, 0x06, 0x03, 0x01},
	{0x10, 0x10, 0x0e, 0x0b, 0x08, 0x05, 0x03, 0x01},
	{0x0f, 0x0f, 0x0d, 0x0b, 0x08, 0x05, 0x03, 0x01},
	{0x0e, 0x0e, 0x0c, 0x0a, 0x08, 0x05, 0x02, 0x01},
	{0x0d, 0x0d, 0x0c, 0x0a, 0x07, 0x05, 0x02, 0x01},
	{0x0d, 0x0c, 0x0b, 0x09, 0x07, 0x04, 0x02, 0x01},
	{0x0c, 0x0c, 0x0a, 0x09, 0x06, 0x04, 0x02, 0x01},
	{0x0b, 0x0b, 0x0a, 0x08, 0x06, 0x04, 0x02, 0x01},
	{0x0b, 0x0a, 0x09, 0x08, 0x06, 0x04, 0x02, 0x01},
	{0x0a, 0x0a, 0x09, 0x07, 0x05, 0x03, 0x02, 0x01},
	{0x0a, 0x09, 0x08, 0x07, 0x05, 0x03, 0x02, 0x01},
	{0x09, 0x09, 0x08, 0x06, 0x05, 0x03, 0x01, 0x01},
	{0x09, 0x08, 0x07, 0x06, 0x04, 0x03, 0x01, 0x01}
};

static const u8 cckswing_table_ch14[CCK_TABLE_SIZE][8] = {
	{0x36, 0x35, 0x2e, 0x1b, 0x00, 0x00, 0x00, 0x00},
	{0x33, 0x32, 0x2b, 0x19, 0x00, 0x00, 0x00, 0x00},
	{0x30, 0x2f, 0x29, 0x18, 0x00, 0x00, 0x00, 0x00},
	{0x2d, 0x2d, 0x17, 0x17, 0x00, 0x00, 0x00, 0x00},
	{0x2b, 0x2a, 0x25, 0x15, 0x00, 0x00, 0x00, 0x00},
	{0x28, 0x28, 0x24, 0x14, 0x00, 0x00, 0x00, 0x00},
	{0x26, 0x25, 0x21, 0x13, 0x00, 0x00, 0x00, 0x00},
	{0x24, 0x23, 0x1f, 0x12, 0x00, 0x00, 0x00, 0x00},
	{0x22, 0x21, 0x1d, 0x11, 0x00, 0x00, 0x00, 0x00},
	{0x20, 0x20, 0x1b, 0x10, 0x00, 0x00, 0x00, 0x00},
	{0x1f, 0x1e, 0x1a, 0x0f, 0x00, 0x00, 0x00, 0x00},
	{0x1d, 0x1c, 0x18, 0x0e, 0x00, 0x00, 0x00, 0x00},
	{0x1b, 0x1a, 0x17, 0x0e, 0x00, 0x00, 0x00, 0x00},
	{0x1a, 0x19, 0x16, 0x0d, 0x00, 0x00, 0x00, 0x00},
	{0x18, 0x17, 0x15, 0x0c, 0x00, 0x00, 0x00, 0x00},
	{0x17, 0x16, 0x13, 0x0b, 0x00, 0x00, 0x00, 0x00},
	{0x16, 0x15, 0x12, 0x0b, 0x00, 0x00, 0x00, 0x00},
	{0x14, 0x14, 0x11, 0x0a, 0x00, 0x00, 0x00, 0x00},
	{0x13, 0x13, 0x10, 0x0a, 0x00, 0x00, 0x00, 0x00},
	{0x12, 0x12, 0x0f, 0x09, 0x00, 0x00, 0x00, 0x00},
	{0x11, 0x11, 0x0f, 0x09, 0x00, 0x00, 0x00, 0x00},
	{0x10, 0x10, 0x0e, 0x08, 0x00, 0x00, 0x00, 0x00},
	{0x0f, 0x0f, 0x0d, 0x08, 0x00, 0x00, 0x00, 0x00},
	{0x0e, 0x0e, 0x0c, 0x07, 0x00, 0x00, 0x00, 0x00},
	{0x0d, 0x0d, 0x0c, 0x07, 0x00, 0x00, 0x00, 0x00},
	{0x0d, 0x0c, 0x0b, 0x06, 0x00, 0x00, 0x00, 0x00},
	{0x0c, 0x0c, 0x0a, 0x06, 0x00, 0x00, 0x00, 0x00},
	{0x0b, 0x0b, 0x0a, 0x06, 0x00, 0x00, 0x00, 0x00},
	{0x0b, 0x0a, 0x09, 0x05, 0x00, 0x00, 0x00, 0x00},
	{0x0a, 0x0a, 0x09, 0x05, 0x00, 0x00, 0x00, 0x00},
	{0x0a, 0x09, 0x08, 0x05, 0x00, 0x00, 0x00, 0x00},
	{0x09, 0x09, 0x08, 0x05, 0x00, 0x00, 0x00, 0x00},
	{0x09, 0x08, 0x07, 0x04, 0x00, 0x00, 0x00, 0x00}
};

static u32 power_index_reg[6] = {0xc90, 0xc91, 0xc92, 0xc98, 0xc99, 0xc9a};

void dm_restorepowerindex(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	u8	index;

	for (index = 0; index < 6; index++)
		rtl_write_byte(rtlpriv, power_index_reg[index],
			       rtlpriv->dm.powerindex_backup[index]);
}
EXPORT_SYMBOL_GPL(dm_restorepowerindex);

void dm_writepowerindex(struct ieee80211_hw *hw, u8 value)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	u8 index;

	for (index = 0; index < 6; index++)
		rtl_write_byte(rtlpriv, power_index_reg[index], value);
}
EXPORT_SYMBOL_GPL(dm_writepowerindex);

void dm_savepowerindex(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	u8 index;
	u8 tmp;

	for (index = 0; index < 6; index++) {
		tmp = rtl_read_byte(rtlpriv, power_index_reg[index]);
		rtlpriv->dm.powerindex_backup[index] = tmp;
	}
}
EXPORT_SYMBOL_GPL(dm_savepowerindex);

static u8 rtl92c_dm_initial_gain_min_pwdb(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct dig_t *dm_digtable = &rtlpriv->dm_digtable;
	long rssi_val_min = 0;

	if ((dm_digtable->curmultista_cstate == DIG_MULTISTA_CONNECT) &&
	    (dm_digtable->cursta_cstate == DIG_STA_CONNECT)) {
		if (rtlpriv->dm.entry_min_undec_sm_pwdb != 0)
			rssi_val_min =
			    (rtlpriv->dm.entry_min_undec_sm_pwdb >
			     rtlpriv->dm.undec_sm_pwdb) ?
			    rtlpriv->dm.undec_sm_pwdb :
			    rtlpriv->dm.entry_min_undec_sm_pwdb;
		else
			rssi_val_min = rtlpriv->dm.undec_sm_pwdb;
	} else if (dm_digtable->cursta_cstate == DIG_STA_CONNECT ||
		   dm_digtable->cursta_cstate == DIG_STA_BEFORE_CONNECT) {
		rssi_val_min = rtlpriv->dm.undec_sm_pwdb;
	} else if (dm_digtable->curmultista_cstate == DIG_MULTISTA_CONNECT) {
		rssi_val_min = rtlpriv->dm.entry_min_undec_sm_pwdb;
	}

	if (rssi_val_min > 100)
		rssi_val_min = 100;
	return (u8)rssi_val_min;
}

static void rtl92c_dm_false_alarm_counter_statistics(struct ieee80211_hw *hw)
{
	u32 ret_value;
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct false_alarm_statistics *falsealm_cnt = &(rtlpriv->falsealm_cnt);

	ret_value = rtl_get_bbreg(hw, ROFDM_PHYCOUNTER1, MASKDWORD);
	falsealm_cnt->cnt_parity_fail = ((ret_value & 0xffff0000) >> 16);

	ret_value = rtl_get_bbreg(hw, ROFDM_PHYCOUNTER2, MASKDWORD);
	falsealm_cnt->cnt_rate_illegal = (ret_value & 0xffff);
	falsealm_cnt->cnt_crc8_fail = ((ret_value & 0xffff0000) >> 16);

	ret_value = rtl_get_bbreg(hw, ROFDM_PHYCOUNTER3, MASKDWORD);
	falsealm_cnt->cnt_mcs_fail = (ret_value & 0xffff);

	ret_value = rtl_get_bbreg(hw, ROFDM0_FRAMESYNC, MASKDWORD);
	falsealm_cnt->cnt_fast_fsync_fail = (ret_value & 0xffff);
	falsealm_cnt->cnt_sb_search_fail = ((ret_value & 0xffff0000) >> 16);

	falsealm_cnt->cnt_ofdm_fail = falsealm_cnt->cnt_parity_fail +
				      falsealm_cnt->cnt_rate_illegal +
				      falsealm_cnt->cnt_crc8_fail +
				      falsealm_cnt->cnt_mcs_fail +
				      falsealm_cnt->cnt_fast_fsync_fail +
				      falsealm_cnt->cnt_sb_search_fail;

	rtl_set_bbreg(hw, RCCK0_FALSEALARMREPORT, BIT(14), 1);
	ret_value = rtl_get_bbreg(hw, RCCK0_FACOUNTERLOWER, MASKBYTE0);
	falsealm_cnt->cnt_cck_fail = ret_value;

	ret_value = rtl_get_bbreg(hw, RCCK0_FACOUNTERUPPER, MASKBYTE3);
	falsealm_cnt->cnt_cck_fail += (ret_value & 0xff) << 8;
	falsealm_cnt->cnt_all = (falsealm_cnt->cnt_parity_fail +
				 falsealm_cnt->cnt_rate_illegal +
				 falsealm_cnt->cnt_crc8_fail +
				 falsealm_cnt->cnt_mcs_fail +
				 falsealm_cnt->cnt_cck_fail);

	rtl_set_bbreg(hw, ROFDM1_LSTF, 0x08000000, 1);
	rtl_set_bbreg(hw, ROFDM1_LSTF, 0x08000000, 0);
	rtl_set_bbreg(hw, RCCK0_FALSEALARMREPORT, 0x0000c000, 0);
	rtl_set_bbreg(hw, RCCK0_FALSEALARMREPORT, 0x0000c000, 2);

	rtl_dbg(rtlpriv, COMP_DIG, DBG_TRACE,
		"cnt_parity_fail = %d, cnt_rate_illegal = %d, cnt_crc8_fail = %d, cnt_mcs_fail = %d\n",
		falsealm_cnt->cnt_parity_fail,
		falsealm_cnt->cnt_rate_illegal,
		falsealm_cnt->cnt_crc8_fail, falsealm_cnt->cnt_mcs_fail);

	rtl_dbg(rtlpriv, COMP_DIG, DBG_TRACE,
		"cnt_ofdm_fail = %x, cnt_cck_fail = %x, cnt_all = %x\n",
		falsealm_cnt->cnt_ofdm_fail,
		falsealm_cnt->cnt_cck_fail, falsealm_cnt->cnt_all);
}

static void rtl92c_dm_ctrl_initgain_by_fa(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct dig_t *dm_digtable = &rtlpriv->dm_digtable;
	u8 value_igi = dm_digtable->cur_igvalue;

	if (rtlpriv->falsealm_cnt.cnt_all < DM_DIG_FA_TH0)
		value_igi--;
	else if (rtlpriv->falsealm_cnt.cnt_all < DM_DIG_FA_TH1)
		value_igi += 0;
	else if (rtlpriv->falsealm_cnt.cnt_all < DM_DIG_FA_TH2)
		value_igi++;
	else if (rtlpriv->falsealm_cnt.cnt_all >= DM_DIG_FA_TH2)
		value_igi += 2;

	if (value_igi > DM_DIG_FA_UPPER)
		value_igi = DM_DIG_FA_UPPER;
	else if (value_igi < DM_DIG_FA_LOWER)
		value_igi = DM_DIG_FA_LOWER;

	if (rtlpriv->falsealm_cnt.cnt_all > 10000)
		value_igi = DM_DIG_FA_UPPER;

	dm_digtable->cur_igvalue = value_igi;
	rtl92c_dm_write_dig(hw);
}

static void rtl92c_dm_ctrl_initgain_by_rssi(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct dig_t *digtable = &rtlpriv->dm_digtable;
	u32 isbt;

	/* modify DIG lower bound, deal with abnormally large false alarm */
	if (rtlpriv->falsealm_cnt.cnt_all > 10000) {
		digtable->large_fa_hit++;
		if (digtable->forbidden_igi < digtable->cur_igvalue) {
			digtable->forbidden_igi = digtable->cur_igvalue;
			digtable->large_fa_hit = 1;
		}

		if (digtable->large_fa_hit >= 3) {
			if ((digtable->forbidden_igi + 1) >
			    digtable->rx_gain_max)
				digtable->rx_gain_min = digtable->rx_gain_max;
			else
				digtable->rx_gain_min = (digtable->forbidden_igi + 1);
			digtable->recover_cnt = 3600; /* 3600=2hr */
		}
	} else {
		/* Recovery mechanism for IGI lower bound */
		if (digtable->recover_cnt != 0) {
			digtable->recover_cnt--;
		} else {
			if (digtable->large_fa_hit == 0) {
				if ((digtable->forbidden_igi-1) < DM_DIG_MIN) {
					digtable->forbidden_igi = DM_DIG_MIN;
					digtable->rx_gain_min = DM_DIG_MIN;
				} else {
					digtable->forbidden_igi--;
					digtable->rx_gain_min = digtable->forbidden_igi + 1;
				}
			} else if (digtable->large_fa_hit == 3) {
				digtable->large_fa_hit = 0;
			}
		}
	}
	if (rtlpriv->falsealm_cnt.cnt_all < 250) {
		isbt = rtl_read_byte(rtlpriv, 0x4fd) & 0x01;

		if (!isbt) {
			if (rtlpriv->falsealm_cnt.cnt_all >
			    digtable->fa_lowthresh) {
				if ((digtable->back_val - 2) <
				   digtable->back_range_min)
					digtable->back_val = digtable->back_range_min;
				else
					digtable->back_val -= 2;
			} else if (rtlpriv->falsealm_cnt.cnt_all <
				   digtable->fa_lowthresh) {
				if ((digtable->back_val + 2) >
				    digtable->back_range_max)
					digtable->back_val = digtable->back_range_max;
				else
					digtable->back_val += 2;
			}
		} else {
			digtable->back_val = DM_DIG_BACKOFF_DEFAULT;
		}
	} else {
		/* Adjust initial gain by false alarm */
		if (rtlpriv->falsealm_cnt.cnt_all > 1000)
			digtable->cur_igvalue = digtable->pre_igvalue + 2;
		else if (rtlpriv->falsealm_cnt.cnt_all > 750)
			digtable->cur_igvalue = digtable->pre_igvalue + 1;
		else if (rtlpriv->falsealm_cnt.cnt_all < 500)
			digtable->cur_igvalue = digtable->pre_igvalue - 1;
	}

	/* Check initial gain by upper/lower bound */
	if (digtable->cur_igvalue > digtable->rx_gain_max)
		digtable->cur_igvalue = digtable->rx_gain_max;

	if (digtable->cur_igvalue < digtable->rx_gain_min)
		digtable->cur_igvalue = digtable->rx_gain_min;

	rtl92c_dm_write_dig(hw);
}

static void rtl92c_dm_initial_gain_multi_sta(struct ieee80211_hw *hw)
{
	static u8 initialized; /* initialized to false */
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct dig_t *dm_digtable = &rtlpriv->dm_digtable;
	struct rtl_mac *mac = rtl_mac(rtl_priv(hw));
	long rssi_strength = rtlpriv->dm.entry_min_undec_sm_pwdb;
	bool multi_sta = false;

	if (mac->opmode == NL80211_IFTYPE_ADHOC)
		multi_sta = true;

	if (!multi_sta ||
	    dm_digtable->cursta_cstate == DIG_STA_DISCONNECT) {
		initialized = false;
		dm_digtable->dig_ext_port_stage = DIG_EXT_PORT_STAGE_MAX;
		return;
	} else if (!initialized) {
		initialized = true;
		dm_digtable->dig_ext_port_stage = DIG_EXT_PORT_STAGE_0;
		dm_digtable->cur_igvalue = 0x20;
		rtl92c_dm_write_dig(hw);
	}

	if (dm_digtable->curmultista_cstate == DIG_MULTISTA_CONNECT) {
		if ((rssi_strength < dm_digtable->rssi_lowthresh) &&
		    (dm_digtable->dig_ext_port_stage != DIG_EXT_PORT_STAGE_1)) {

			if (dm_digtable->dig_ext_port_stage ==
			    DIG_EXT_PORT_STAGE_2) {
				dm_digtable->cur_igvalue = 0x20;
				rtl92c_dm_write_dig(hw);
			}

			dm_digtable->dig_ext_port_stage = DIG_EXT_PORT_STAGE_1;
		} else if (rssi_strength > dm_digtable->rssi_highthresh) {
			dm_digtable->dig_ext_port_stage = DIG_EXT_PORT_STAGE_2;
			rtl92c_dm_ctrl_initgain_by_fa(hw);
		}
	} else if (dm_digtable->dig_ext_port_stage != DIG_EXT_PORT_STAGE_0) {
		dm_digtable->dig_ext_port_stage = DIG_EXT_PORT_STAGE_0;
		dm_digtable->cur_igvalue = 0x20;
		rtl92c_dm_write_dig(hw);
	}

	rtl_dbg(rtlpriv, COMP_DIG, DBG_TRACE,
		"curmultista_cstate = %x dig_ext_port_stage %x\n",
		dm_digtable->curmultista_cstate,
		dm_digtable->dig_ext_port_stage);
}

static void rtl92c_dm_initial_gain_sta(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct dig_t *dm_digtable = &rtlpriv->dm_digtable;

	rtl_dbg(rtlpriv, COMP_DIG, DBG_TRACE,
		"presta_cstate = %x, cursta_cstate = %x\n",
		dm_digtable->presta_cstate, dm_digtable->cursta_cstate);
	if (dm_digtable->presta_cstate == dm_digtable->cursta_cstate ||
	    dm_digtable->cursta_cstate == DIG_STA_BEFORE_CONNECT ||
	    dm_digtable->cursta_cstate == DIG_STA_CONNECT) {
		if (dm_digtable->cursta_cstate != DIG_STA_DISCONNECT) {
			dm_digtable->rssi_val_min =
			    rtl92c_dm_initial_gain_min_pwdb(hw);
			if (dm_digtable->rssi_val_min > 100)
				dm_digtable->rssi_val_min = 100;
			rtl92c_dm_ctrl_initgain_by_rssi(hw);
		}
	} else {
		dm_digtable->rssi_val_min = 0;
		dm_digtable->dig_ext_port_stage = DIG_EXT_PORT_STAGE_MAX;
		dm_digtable->back_val = DM_DIG_BACKOFF_DEFAULT;
		dm_digtable->cur_igvalue = 0x20;
		dm_digtable->pre_igvalue = 0;
		rtl92c_dm_write_dig(hw);
	}
}

static void rtl92c_dm_cck_packet_detection_thresh(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct dig_t *dm_digtable = &rtlpriv->dm_digtable;

	if (dm_digtable->cursta_cstate == DIG_STA_CONNECT) {
		dm_digtable->rssi_val_min = rtl92c_dm_initial_gain_min_pwdb(hw);
		if (dm_digtable->rssi_val_min > 100)
			dm_digtable->rssi_val_min = 100;

		if (dm_digtable->pre_cck_pd_state == CCK_PD_STAGE_LOWRSSI) {
			if (dm_digtable->rssi_val_min <= 25)
				dm_digtable->cur_cck_pd_state =
				    CCK_PD_STAGE_LOWRSSI;
			else
				dm_digtable->cur_cck_pd_state =
				    CCK_PD_STAGE_HIGHRSSI;
		} else {
			if (dm_digtable->rssi_val_min <= 20)
				dm_digtable->cur_cck_pd_state =
				    CCK_PD_STAGE_LOWRSSI;
			else
				dm_digtable->cur_cck_pd_state =
				    CCK_PD_STAGE_HIGHRSSI;
		}
	} else {
		dm_digtable->cur_cck_pd_state = CCK_PD_STAGE_MAX;
	}

	if (dm_digtable->pre_cck_pd_state != dm_digtable->cur_cck_pd_state) {
		if ((dm_digtable->cur_cck_pd_state == CCK_PD_STAGE_LOWRSSI) ||
		    (dm_digtable->cur_cck_pd_state == CCK_PD_STAGE_MAX))
			rtl_set_bbreg(hw, RCCK0_CCA, MASKBYTE2, 0x83);
		else
			rtl_set_bbreg(hw, RCCK0_CCA, MASKBYTE2, 0xcd);

		dm_digtable->pre_cck_pd_state = dm_digtable->cur_cck_pd_state;
	}
}

static void rtl92c_dm_ctrl_initgain_by_twoport(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct dig_t *dm_digtable = &rtlpriv->dm_digtable;
	struct rtl_mac *mac = rtl_mac(rtl_priv(hw));

	if (mac->act_scanning)
		return;

	if (mac->link_state >= MAC80211_LINKED)
		dm_digtable->cursta_cstate = DIG_STA_CONNECT;
	else
		dm_digtable->cursta_cstate = DIG_STA_DISCONNECT;

	dm_digtable->curmultista_cstate = DIG_MULTISTA_DISCONNECT;

	rtl92c_dm_initial_gain_sta(hw);
	rtl92c_dm_initial_gain_multi_sta(hw);
	rtl92c_dm_cck_packet_detection_thresh(hw);

	dm_digtable->presta_cstate = dm_digtable->cursta_cstate;
}

static void rtl92c_dm_dig(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);

	if (!rtlpriv->dm.dm_initialgain_enable)
		return;
	if (!(rtlpriv->dm.dm_flag & DYNAMIC_FUNC_DIG))
		return;

	rtl92c_dm_ctrl_initgain_by_twoport(hw);
}

static void rtl92c_dm_init_dynamic_txpower(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);

	if (rtlpriv->rtlhal.interface == INTF_USB &&
	    rtlpriv->rtlhal.board_type & 0x1) {
		dm_savepowerindex(hw);
		rtlpriv->dm.dynamic_txpower_enable = true;
	} else {
		rtlpriv->dm.dynamic_txpower_enable = false;
	}
	rtlpriv->dm.last_dtp_lvl = TXHIGHPWRLEVEL_NORMAL;
	rtlpriv->dm.dynamic_txhighpower_lvl = TXHIGHPWRLEVEL_NORMAL;
}

void rtl92c_dm_write_dig(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct dig_t *dm_digtable = &rtlpriv->dm_digtable;

	rtl_dbg(rtlpriv, COMP_DIG, DBG_LOUD,
		"cur_igvalue = 0x%x, pre_igvalue = 0x%x, back_val = %d\n",
		dm_digtable->cur_igvalue, dm_digtable->pre_igvalue,
		dm_digtable->back_val);

	if (rtlpriv->rtlhal.interface == INTF_USB &&
	    !dm_digtable->dig_enable_flag) {
		dm_digtable->pre_igvalue = 0x17;
		return;
	}
	dm_digtable->cur_igvalue -= 1;
	if (dm_digtable->cur_igvalue < DM_DIG_MIN)
		dm_digtable->cur_igvalue = DM_DIG_MIN;

	if (dm_digtable->pre_igvalue != dm_digtable->cur_igvalue) {
		rtl_set_bbreg(hw, ROFDM0_XAAGCCORE1, 0x7f,
			      dm_digtable->cur_igvalue);
		rtl_set_bbreg(hw, ROFDM0_XBAGCCORE1, 0x7f,
			      dm_digtable->cur_igvalue);

		dm_digtable->pre_igvalue = dm_digtable->cur_igvalue;
	}
	rtl_dbg(rtlpriv, COMP_DIG, DBG_WARNING,
		"dig values 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",
		dm_digtable->cur_igvalue, dm_digtable->pre_igvalue,
		dm_digtable->rssi_val_min, dm_digtable->back_val,
		dm_digtable->rx_gain_max, dm_digtable->rx_gain_min,
		dm_digtable->large_fa_hit, dm_digtable->forbidden_igi);
}
EXPORT_SYMBOL(rtl92c_dm_write_dig);

static void rtl92c_dm_pwdb_monitor(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_mac *mac = rtl_mac(rtl_priv(hw));
	long tmpentry_max_pwdb = 0, tmpentry_min_pwdb = 0xff;

	if (mac->link_state != MAC80211_LINKED)
		return;

	if (mac->opmode == NL80211_IFTYPE_ADHOC ||
	    mac->opmode == NL80211_IFTYPE_AP) {
		/* TODO: Handle ADHOC and AP Mode */
	}

	if (tmpentry_max_pwdb != 0)
		rtlpriv->dm.entry_max_undec_sm_pwdb = tmpentry_max_pwdb;
	else
		rtlpriv->dm.entry_max_undec_sm_pwdb = 0;

	if (tmpentry_min_pwdb != 0xff)
		rtlpriv->dm.entry_min_undec_sm_pwdb = tmpentry_min_pwdb;
	else
		rtlpriv->dm.entry_min_undec_sm_pwdb = 0;

/* TODO:
 *	if (mac->opmode == NL80211_IFTYPE_STATION) {
 *		if (rtlpriv->rtlhal.fw_ready) {
 *			u32 param = (u32)(rtlpriv->dm.undec_sm_pwdb << 16);
 *			rtl8192c_set_rssi_cmd(hw, param);
 *		}
 *	}
 */
}

void rtl92c_dm_init_edca_turbo(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);

	rtlpriv->dm.current_turbo_edca = false;
	rtlpriv->dm.is_any_nonbepkts = false;
	rtlpriv->dm.is_cur_rdlstate = false;
}
EXPORT_SYMBOL(rtl92c_dm_init_edca_turbo);

static void rtl92c_dm_check_edca_turbo(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_mac *mac = rtl_mac(rtl_priv(hw));

	static u64 last_txok_cnt;
	static u64 last_rxok_cnt;
	static u32 last_bt_edca_ul;
	static u32 last_bt_edca_dl;
	u64 cur_txok_cnt = 0;
	u64 cur_rxok_cnt = 0;
	u32 edca_be_ul = 0x5ea42b;
	u32 edca_be_dl = 0x5ea42b;
	bool bt_change_edca = false;

	if ((last_bt_edca_ul != rtlpriv->btcoexist.bt_edca_ul) ||
	    (last_bt_edca_dl != rtlpriv->btcoexist.bt_edca_dl)) {
		rtlpriv->dm.current_turbo_edca = false;
		last_bt_edca_ul = rtlpriv->btcoexist.bt_edca_ul;
		last_bt_edca_dl = rtlpriv->btcoexist.bt_edca_dl;
	}

	if (rtlpriv->btcoexist.bt_edca_ul != 0) {
		edca_be_ul = rtlpriv->btcoexist.bt_edca_ul;
		bt_change_edca = true;
	}

	if (rtlpriv->btcoexist.bt_edca_dl != 0) {
		edca_be_ul = rtlpriv->btcoexist.bt_edca_dl;
		bt_change_edca = true;
	}

	if (mac->link_state != MAC80211_LINKED) {
		rtlpriv->dm.current_turbo_edca = false;
		return;
	}

	if ((!mac->ht_enable) && (!rtlpriv->btcoexist.bt_coexistence)) {
		if (!(edca_be_ul & 0xffff0000))
			edca_be_ul |= 0x005e0000;

		if (!(edca_be_dl & 0xffff0000))
			edca_be_dl |= 0x005e0000;
	}

	if ((bt_change_edca) || ((!rtlpriv->dm.is_any_nonbepkts) &&
	     (!rtlpriv->dm.disable_framebursting))) {
		cur_txok_cnt = rtlpriv->stats.txbytesunicast - last_txok_cnt;
		cur_rxok_cnt = rtlpriv->stats.rxbytesunicast - last_rxok_cnt;

		if (cur_rxok_cnt > 4 * cur_txok_cnt) {
			if (!rtlpriv->dm.is_cur_rdlstate ||
			    !rtlpriv->dm.current_turbo_edca) {
				rtl_write_dword(rtlpriv,
						REG_EDCA_BE_PARAM,
						edca_be_dl);
				rtlpriv->dm.is_cur_rdlstate = true;
			}
		} else {
			if (rtlpriv->dm.is_cur_rdlstate ||
			    !rtlpriv->dm.current_turbo_edca) {
				rtl_write_dword(rtlpriv,
						REG_EDCA_BE_PARAM,
						edca_be_ul);
				rtlpriv->dm.is_cur_rdlstate = false;
			}
		}
		rtlpriv->dm.current_turbo_edca = true;
	} else {
		if (rtlpriv->dm.current_turbo_edca) {
			u8 tmp = AC0_BE;

			rtlpriv->cfg->ops->set_hw_reg(hw, HW_VAR_AC_PARAM,
						      &tmp);
			rtlpriv->dm.current_turbo_edca = false;
		}
	}

	rtlpriv->dm.is_any_nonbepkts = false;
	last_txok_cnt = rtlpriv->stats.txbytesunicast;
	last_rxok_cnt = rtlpriv->stats.rxbytesunicast;
}

static void rtl92c_dm_txpower_tracking_callback_thermalmeter(struct ieee80211_hw
							     *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_hal *rtlhal = rtl_hal(rtl_priv(hw));
	struct rtl_phy *rtlphy = &(rtlpriv->phy);
	struct rtl_efuse *rtlefuse = rtl_efuse(rtl_priv(hw));
	u8 thermalvalue, delta, delta_lck, delta_iqk;
	long ele_a, ele_d, temp_cck, val_x, value32;
	long val_y, ele_c = 0;
	u8 ofdm_index[2], ofdm_index_old[2] = {0, 0}, cck_index_old = 0;
	s8 cck_index = 0;
	int i;
	bool is2t = IS_92C_SERIAL(rtlhal->version);
	s8 txpwr_level[3] = {0, 0, 0};
	u8 ofdm_min_index = 6, rf;

	rtlpriv->dm.txpower_trackinginit = true;
	rtl_dbg(rtlpriv, COMP_POWER_TRACKING, DBG_LOUD,
		"%s\n", __func__);

	thermalvalue = (u8) rtl_get_rfreg(hw, RF90_PATH_A, RF_T_METER, 0x1f);

	rtl_dbg(rtlpriv, COMP_POWER_TRACKING, DBG_LOUD,
		"Readback Thermal Meter = 0x%x pre thermal meter 0x%x eeprom_thermalmeter 0x%x\n",
		thermalvalue, rtlpriv->dm.thermalvalue,
		rtlefuse->eeprom_thermalmeter);

	rtl92c_phy_ap_calibrate(hw, (thermalvalue -
				     rtlefuse->eeprom_thermalmeter));
	if (is2t)
		rf = 2;
	else
		rf = 1;

	if (thermalvalue) {
		ele_d = rtl_get_bbreg(hw, ROFDM0_XATXIQIMBALANCE,
				      MASKDWORD) & MASKOFDM_D;

		for (i = 0; i < OFDM_TABLE_LENGTH; i++) {
			if (ele_d == (ofdmswing_table[i] & MASKOFDM_D)) {
				ofdm_index_old[0] = (u8) i;

				rtl_dbg(rtlpriv, COMP_POWER_TRACKING, DBG_LOUD,
					"Initial pathA ele_d reg0x%x = 0x%lx, ofdm_index=0x%x\n",
					ROFDM0_XATXIQIMBALANCE,
					ele_d, ofdm_index_old[0]);
				break;
			}
		}

		if (is2t) {
			ele_d = rtl_get_bbreg(hw, ROFDM0_XBTXIQIMBALANCE,
					      MASKDWORD) & MASKOFDM_D;

			for (i = 0; i < OFDM_TABLE_LENGTH; i++) {
				if (ele_d == (ofdmswing_table[i] &
				    MASKOFDM_D)) {
					ofdm_index_old[1] = (u8) i;
					rtl_dbg(rtlpriv, COMP_POWER_TRACKING,
						DBG_LOUD,
						"Initial pathB ele_d reg0x%x = 0x%lx, ofdm_index=0x%x\n",
						ROFDM0_XBTXIQIMBALANCE, ele_d,
						ofdm_index_old[1]);
					break;
				}
			}
		}

		temp_cck =
		    rtl_get_bbreg(hw, RCCK0_TXFILTER2, MASKDWORD) & MASKCCK;

		for (i = 0; i < CCK_TABLE_LENGTH; i++) {
			if (rtlpriv->dm.cck_inch14) {
				if (memcmp((void *)&temp_cck,
					   (void *)&cckswing_table_ch14[i][2],
					   4) == 0) {
					cck_index_old = (u8) i;

					rtl_dbg(rtlpriv, COMP_POWER_TRACKING,
						DBG_LOUD,
						"Initial reg0x%x = 0x%lx, cck_index=0x%x, ch 14 %d\n",
						RCCK0_TXFILTER2, temp_cck,
						cck_index_old,
						rtlpriv->dm.cck_inch14);
					break;
				}
			} else {
				if (memcmp((void *)&temp_cck,
					   (void *)
					   &cckswing_table_ch1ch13[i][2],
					   4) == 0) {
					cck_index_old = (u8) i;

					rtl_dbg(rtlpriv, COMP_POWER_TRACKING,
						DBG_LOUD,
						"Initial reg0x%x = 0x%lx, cck_index=0x%x, ch14 %d\n",
						RCCK0_TXFILTER2, temp_cck,
						cck_index_old,
						rtlpriv->dm.cck_inch14);
					break;
				}
			}
		}

		if (!rtlpriv->dm.thermalvalue) {
			rtlpriv->dm.thermalvalue =
			    rtlefuse->eeprom_thermalmeter;
			rtlpriv->dm.thermalvalue_lck = thermalvalue;
			rtlpriv->dm.thermalvalue_iqk = thermalvalue;
			for (i = 0; i < rf; i++)
				rtlpriv->dm.ofdm_index[i] = ofdm_index_old[i];
			rtlpriv->dm.cck_index = cck_index_old;
		}
		/* Handle USB High PA boards */

		delta = (thermalvalue > rtlpriv->dm.thermalvalue) ?
		    (thermalvalue - rtlpriv->dm.thermalvalue) :
		    (rtlpriv->dm.thermalvalue - thermalvalue);

		delta_lck = (thermalvalue > rtlpriv->dm.thermalvalue_lck) ?
		    (thermalvalue - rtlpriv->dm.thermalvalue_lck) :
		    (rtlpriv->dm.thermalvalue_lck - thermalvalue);

		delta_iqk = (thermalvalue > rtlpriv->dm.thermalvalue_iqk) ?
		    (thermalvalue - rtlpriv->dm.thermalvalue_iqk) :
		    (rtlpriv->dm.thermalvalue_iqk - thermalvalue);

		rtl_dbg(rtlpriv, COMP_POWER_TRACKING, DBG_LOUD,
			"Readback Thermal Meter = 0x%x pre thermal meter 0x%x eeprom_thermalmeter 0x%x delta 0x%x delta_lck 0x%x delta_iqk 0x%x\n",
			thermalvalue, rtlpriv->dm.thermalvalue,
			rtlefuse->eeprom_thermalmeter, delta, delta_lck,
			delta_iqk);

		if (delta_lck > 1) {
			rtlpriv->dm.thermalvalue_lck = thermalvalue;
			rtl92c_phy_lc_calibrate(hw);
		}

		if (delta > 0 && rtlpriv->dm.txpower_track_control) {
			if (thermalvalue > rtlpriv->dm.thermalvalue) {
				for (i = 0; i < rf; i++)
					rtlpriv->dm.ofdm_index[i] -= delta;
				rtlpriv->dm.cck_index -= delta;
			} else {
				for (i = 0; i < rf; i++)
					rtlpriv->dm.ofdm_index[i] += delta;
				rtlpriv->dm.cck_index += delta;
			}

			if (is2t) {
				rtl_dbg(rtlpriv, COMP_POWER_TRACKING, DBG_LOUD,
					"temp OFDM_A_index=0x%x, OFDM_B_index=0x%x, cck_index=0x%x\n",
					rtlpriv->dm.ofdm_index[0],
					rtlpriv->dm.ofdm_index[1],
					rtlpriv->dm.cck_index);
			} else {
				rtl_dbg(rtlpriv, COMP_POWER_TRACKING, DBG_LOUD,
					"temp OFDM_A_index=0x%x, cck_index=0x%x\n",
					rtlpriv->dm.ofdm_index[0],
					rtlpriv->dm.cck_index);
			}

			if (thermalvalue > rtlefuse->eeprom_thermalmeter) {
				for (i = 0; i < rf; i++)
					ofdm_index[i] =
					    rtlpriv->dm.ofdm_index[i]
					    + 1;
				cck_index = rtlpriv->dm.cck_index + 1;
			} else {
				for (i = 0; i < rf; i++)
					ofdm_index[i] =
					    rtlpriv->dm.ofdm_index[i];
				cck_index = rtlpriv->dm.cck_index;
			}

			for (i = 0; i < rf; i++) {
				if (txpwr_level[i] >= 0 &&
				    txpwr_level[i] <= 26) {
					if (thermalvalue >
					    rtlefuse->eeprom_thermalmeter) {
						if (delta < 5)
							ofdm_index[i] -= 1;

						else
							ofdm_index[i] -= 2;
					} else if (delta > 5 && thermalvalue <
						   rtlefuse->
						   eeprom_thermalmeter) {
						ofdm_index[i] += 1;
					}
				} else if (txpwr_level[i] >= 27 &&
					   txpwr_level[i] <= 32
					   && thermalvalue >
					   rtlefuse->eeprom_thermalmeter) {
					if (delta < 5)
						ofdm_index[i] -= 1;

					else
						ofdm_index[i] -= 2;
				} else if (txpwr_level[i] >= 32 &&
					   txpwr_level[i] <= 38 &&
					   thermalvalue >
					   rtlefuse->eeprom_thermalmeter
					   && delta > 5) {
					ofdm_index[i] -= 1;
				}
			}

			if (txpwr_level[i] >= 0 && txpwr_level[i] <= 26) {
				if (thermalvalue >
				    rtlefuse->eeprom_thermalmeter) {
					if (delta < 5)
						cck_index -= 1;

					else
						cck_index -= 2;
				} else if (delta > 5 && thermalvalue <
					   rtlefuse->eeprom_thermalmeter) {
					cck_index += 1;
				}
			} else if (txpwr_level[i] >= 27 &&
				   txpwr_level[i] <= 32 &&
				   thermalvalue >
				   rtlefuse->eeprom_thermalmeter) {
				if (delta < 5)
					cck_index -= 1;

				else
					cck_index -= 2;
			} else if (txpwr_level[i] >= 32 &&
				   txpwr_level[i] <= 38 &&
				   thermalvalue > rtlefuse->eeprom_thermalmeter
				   && delta > 5) {
				cck_index -= 1;
			}

			for (i = 0; i < rf; i++) {
				if (ofdm_index[i] > OFDM_TABLE_SIZE - 1)
					ofdm_index[i] = OFDM_TABLE_SIZE - 1;

				else if (ofdm_index[i] < ofdm_min_index)
					ofdm_index[i] = ofdm_min_index;
			}

			if (cck_index > CCK_TABLE_SIZE - 1)
				cck_index = CCK_TABLE_SIZE - 1;
			else if (cck_index < 0)
				cck_index = 0;

			if (is2t) {
				rtl_dbg(rtlpriv, COMP_POWER_TRACKING, DBG_LOUD,
					"new OFDM_A_index=0x%x, OFDM_B_index=0x%x, cck_index=0x%x\n",
					ofdm_index[0], ofdm_index[1],
					cck_index);
			} else {
				rtl_dbg(rtlpriv, COMP_POWER_TRACKING, DBG_LOUD,
					"new OFDM_A_index=0x%x, cck_index=0x%x\n",
					ofdm_index[0], cck_index);
			}
		}

		if (rtlpriv->dm.txpower_track_control && delta != 0) {
			ele_d =
			    (ofdmswing_table[ofdm_index[0]] & 0xFFC00000) >> 22;
			val_x = rtlphy->reg_e94;
			val_y = rtlphy->reg_e9c;

			if (val_x != 0) {
				if ((val_x & 0x00000200) != 0)
					val_x = val_x | 0xFFFFFC00;
				ele_a = ((val_x * ele_d) >> 8) & 0x000003FF;

				if ((val_y & 0x00000200) != 0)
					val_y = val_y | 0xFFFFFC00;
				ele_c = ((val_y * ele_d) >> 8) & 0x000003FF;

				value32 = (ele_d << 22) |
				    ((ele_c & 0x3F) << 16) | ele_a;

				rtl_set_bbreg(hw, ROFDM0_XATXIQIMBALANCE,
					      MASKDWORD, value32);

				value32 = (ele_c & 0x000003C0) >> 6;
				rtl_set_bbreg(hw, ROFDM0_XCTXAFE, MASKH4BITS,
					      value32);

				value32 = ((val_x * ele_d) >> 7) & 0x01;
				rtl_set_bbreg(hw, ROFDM0_ECCATHRESHOLD,
					      BIT(31), value32);

				value32 = ((val_y * ele_d) >> 7) & 0x01;
				rtl_set_bbreg(hw, ROFDM0_ECCATHRESHOLD,
					      BIT(29), value32);
			} else {
				rtl_set_bbreg(hw, ROFDM0_XATXIQIMBALANCE,
					      MASKDWORD,
					      ofdmswing_table[ofdm_index[0]]);

				rtl_set_bbreg(hw, ROFDM0_XCTXAFE, MASKH4BITS,
					      0x00);
				rtl_set_bbreg(hw, ROFDM0_ECCATHRESHOLD,
					      BIT(31) | BIT(29), 0x00);
			}

			if (!rtlpriv->dm.cck_inch14) {
				rtl_write_byte(rtlpriv, 0xa22,
					       cckswing_table_ch1ch13[cck_index]
					       [0]);
				rtl_write_byte(rtlpriv, 0xa23,
					       cckswing_table_ch1ch13[cck_index]
					       [1]);
				rtl_write_byte(rtlpriv, 0xa24,
					       cckswing_table_ch1ch13[cck_index]
					       [2]);
				rtl_write_byte(rtlpriv, 0xa25,
					       cckswing_table_ch1ch13[cck_index]
					       [3]);
				rtl_write_byte(rtlpriv, 0xa26,
					       cckswing_table_ch1ch13[cck_index]
					       [4]);
				rtl_write_byte(rtlpriv, 0xa27,
					       cckswing_table_ch1ch13[cck_index]
					       [5]);
				rtl_write_byte(rtlpriv, 0xa28,
					       cckswing_table_ch1ch13[cck_index]
					       [6]);
				rtl_write_byte(rtlpriv, 0xa29,
					       cckswing_table_ch1ch13[cck_index]
					       [7]);
			} else {
				rtl_write_byte(rtlpriv, 0xa22,
					       cckswing_table_ch14[cck_index]
					       [0]);
				rtl_write_byte(rtlpriv, 0xa23,
					       cckswing_table_ch14[cck_index]
					       [1]);
				rtl_write_byte(rtlpriv, 0xa24,
					       cckswing_table_ch14[cck_index]
					       [2]);
				rtl_write_byte(rtlpriv, 0xa25,
					       cckswing_table_ch14[cck_index]
					       [3]);
				rtl_write_byte(rtlpriv, 0xa26,
					       cckswing_table_ch14[cck_index]
					       [4]);
				rtl_write_byte(rtlpriv, 0xa27,
					       cckswing_table_ch14[cck_index]
					       [5]);
				rtl_write_byte(rtlpriv, 0xa28,
					       cckswing_table_ch14[cck_index]
					       [6]);
				rtl_write_byte(rtlpriv, 0xa29,
					       cckswing_table_ch14[cck_index]
					       [7]);
			}

			if (is2t) {
				ele_d = (ofdmswing_table[ofdm_index[1]] &
					 0xFFC00000) >> 22;

				val_x = rtlphy->reg_eb4;
				val_y = rtlphy->reg_ebc;

				if (val_x != 0) {
					if ((val_x & 0x00000200) != 0)
						val_x = val_x | 0xFFFFFC00;
					ele_a = ((val_x * ele_d) >> 8) &
					    0x000003FF;

					if ((val_y & 0x00000200) != 0)
						val_y = val_y | 0xFFFFFC00;
					ele_c = ((val_y * ele_d) >> 8) &
					    0x00003FF;

					value32 = (ele_d << 22) |
					    ((ele_c & 0x3F) << 16) | ele_a;
					rtl_set_bbreg(hw,
						      ROFDM0_XBTXIQIMBALANCE,
						      MASKDWORD, value32);

					value32 = (ele_c & 0x000003C0) >> 6;
					rtl_set_bbreg(hw, ROFDM0_XDTXAFE,
						      MASKH4BITS, value32);

					value32 = ((val_x * ele_d) >> 7) & 0x01;
					rtl_set_bbreg(hw, ROFDM0_ECCATHRESHOLD,
						      BIT(27), value32);

					value32 = ((val_y * ele_d) >> 7) & 0x01;
					rtl_set_bbreg(hw, ROFDM0_ECCATHRESHOLD,
						      BIT(25), value32);
				} else {
					rtl_set_bbreg(hw,
						      ROFDM0_XBTXIQIMBALANCE,
						      MASKDWORD,
						      ofdmswing_table[ofdm_index
								      [1]]);
					rtl_set_bbreg(hw, ROFDM0_XDTXAFE,
						      MASKH4BITS, 0x00);
					rtl_set_bbreg(hw, ROFDM0_ECCATHRESHOLD,
						      BIT(27) | BIT(25), 0x00);
				}

			}
		}

		if (delta_iqk > 3) {
			rtlpriv->dm.thermalvalue_iqk = thermalvalue;
			rtl92c_phy_iq_calibrate(hw, false);
		}

		if (rtlpriv->dm.txpower_track_control)
			rtlpriv->dm.thermalvalue = thermalvalue;
	}

	rtl_dbg(rtlpriv, COMP_POWER_TRACKING, DBG_LOUD, "<===\n");

}

static void rtl92c_dm_initialize_txpower_tracking_thermalmeter(
						struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);

	rtlpriv->dm.txpower_tracking = true;
	rtlpriv->dm.txpower_trackinginit = false;

	rtl_dbg(rtlpriv, COMP_POWER_TRACKING, DBG_LOUD,
		"pMgntInfo->txpower_tracking = %d\n",
		rtlpriv->dm.txpower_tracking);
}

static void rtl92c_dm_initialize_txpower_tracking(struct ieee80211_hw *hw)
{
	rtl92c_dm_initialize_txpower_tracking_thermalmeter(hw);
}

static void rtl92c_dm_txpower_tracking_directcall(struct ieee80211_hw *hw)
{
	rtl92c_dm_txpower_tracking_callback_thermalmeter(hw);
}

static void rtl92c_dm_check_txpower_tracking_thermal_meter(
						struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);

	if (!rtlpriv->dm.txpower_tracking)
		return;

	if (!rtlpriv->dm.tm_trigger) {
		rtl_set_rfreg(hw, RF90_PATH_A, RF_T_METER, RFREG_OFFSET_MASK,
			      0x60);
		rtl_dbg(rtlpriv, COMP_POWER_TRACKING, DBG_LOUD,
			"Trigger 92S Thermal Meter!!\n");
		rtlpriv->dm.tm_trigger = 1;
		return;
	} else {
		rtl_dbg(rtlpriv, COMP_POWER_TRACKING, DBG_LOUD,
			"Schedule TxPowerTracking direct call!!\n");
		rtl92c_dm_txpower_tracking_directcall(hw);
		rtlpriv->dm.tm_trigger = 0;
	}
}

void rtl92c_dm_check_txpower_tracking(struct ieee80211_hw *hw)
{
	rtl92c_dm_check_txpower_tracking_thermal_meter(hw);
}
EXPORT_SYMBOL(rtl92c_dm_check_txpower_tracking);

void rtl92c_dm_init_rate_adaptive_mask(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rate_adaptive *p_ra = &(rtlpriv->ra);

	p_ra->ratr_state = DM_RATR_STA_INIT;
	p_ra->pre_ratr_state = DM_RATR_STA_INIT;

	if (rtlpriv->dm.dm_type == DM_TYPE_BYDRIVER)
		rtlpriv->dm.useramask = true;
	else
		rtlpriv->dm.useramask = false;

}
EXPORT_SYMBOL(rtl92c_dm_init_rate_adaptive_mask);

static void rtl92c_dm_init_dynamic_bb_powersaving(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct ps_t *dm_pstable = &rtlpriv->dm_pstable;

	dm_pstable->pre_ccastate = CCA_MAX;
	dm_pstable->cur_ccasate = CCA_MAX;
	dm_pstable->pre_rfstate = RF_MAX;
	dm_pstable->cur_rfstate = RF_MAX;
	dm_pstable->rssi_val_min = 0;
}

void rtl92c_dm_rf_saving(struct ieee80211_hw *hw, u8 bforce_in_normal)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct ps_t *dm_pstable = &rtlpriv->dm_pstable;

	if (!rtlpriv->reg_init) {
		rtlpriv->reg_874 = (rtl_get_bbreg(hw,
						  RFPGA0_XCD_RFINTERFACESW,
						  MASKDWORD) & 0x1CC000) >> 14;

		rtlpriv->reg_c70 = (rtl_get_bbreg(hw, ROFDM0_AGCPARAMETER1,
				    MASKDWORD) & BIT(3)) >> 3;

		rtlpriv->reg_85c = (rtl_get_bbreg(hw, RFPGA0_XCD_SWITCHCONTROL,
				    MASKDWORD) & 0xFF000000) >> 24;

		rtlpriv->reg_a74 = (rtl_get_bbreg(hw, 0xa74, MASKDWORD) &
				    0xF000) >> 12;

		rtlpriv->reg_init = true;
	}

	if (!bforce_in_normal) {
		if (dm_pstable->rssi_val_min != 0) {
			if (dm_pstable->pre_rfstate == RF_NORMAL) {
				if (dm_pstable->rssi_val_min >= 30)
					dm_pstable->cur_rfstate = RF_SAVE;
				else
					dm_pstable->cur_rfstate = RF_NORMAL;
			} else {
				if (dm_pstable->rssi_val_min <= 25)
					dm_pstable->cur_rfstate = RF_NORMAL;
				else
					dm_pstable->cur_rfstate = RF_SAVE;
			}
		} else {
			dm_pstable->cur_rfstate = RF_MAX;
		}
	} else {
		dm_pstable->cur_rfstate = RF_NORMAL;
	}

	if (dm_pstable->pre_rfstate != dm_pstable->cur_rfstate) {
		if (dm_pstable->cur_rfstate == RF_SAVE) {
			rtl_set_bbreg(hw, RFPGA0_XCD_RFINTERFACESW,
				      0x1C0000, 0x2);
			rtl_set_bbreg(hw, ROFDM0_AGCPARAMETER1, BIT(3), 0);
			rtl_set_bbreg(hw, RFPGA0_XCD_SWITCHCONTROL,
				      0xFF000000, 0x63);
			rtl_set_bbreg(hw, RFPGA0_XCD_RFINTERFACESW,
				      0xC000, 0x2);
			rtl_set_bbreg(hw, 0xa74, 0xF000, 0x3);
			rtl_set_bbreg(hw, 0x818, BIT(28), 0x0);
			rtl_set_bbreg(hw, 0x818, BIT(28), 0x1);
		} else {
			rtl_set_bbreg(hw, RFPGA0_XCD_RFINTERFACESW,
				      0x1CC000, rtlpriv->reg_874);
			rtl_set_bbreg(hw, ROFDM0_AGCPARAMETER1, BIT(3),
				      rtlpriv->reg_c70);
			rtl_set_bbreg(hw, RFPGA0_XCD_SWITCHCONTROL, 0xFF000000,
				      rtlpriv->reg_85c);
			rtl_set_bbreg(hw, 0xa74, 0xF000, rtlpriv->reg_a74);
			rtl_set_bbreg(hw, 0x818, BIT(28), 0x0);
		}

		dm_pstable->pre_rfstate = dm_pstable->cur_rfstate;
	}
}
EXPORT_SYMBOL(rtl92c_dm_rf_saving);

static void rtl92c_dm_dynamic_bb_powersaving(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct ps_t *dm_pstable = &rtlpriv->dm_pstable;
	struct rtl_mac *mac = rtl_mac(rtl_priv(hw));
	struct rtl_hal *rtlhal = rtl_hal(rtl_priv(hw));

	/* Determine the minimum RSSI */
	if (((mac->link_state == MAC80211_NOLINK)) &&
	    (rtlpriv->dm.entry_min_undec_sm_pwdb == 0)) {
		dm_pstable->rssi_val_min = 0;
		rtl_dbg(rtlpriv, DBG_LOUD, DBG_LOUD, "Not connected to any\n");
	}

	if (mac->link_state == MAC80211_LINKED) {
		if (mac->opmode == NL80211_IFTYPE_ADHOC) {
			dm_pstable->rssi_val_min =
			    rtlpriv->dm.entry_min_undec_sm_pwdb;
			rtl_dbg(rtlpriv, DBG_LOUD, DBG_LOUD,
				"AP Client PWDB = 0x%lx\n",
				dm_pstable->rssi_val_min);
		} else {
			dm_pstable->rssi_val_min = rtlpriv->dm.undec_sm_pwdb;
			rtl_dbg(rtlpriv, DBG_LOUD, DBG_LOUD,
				"STA Default Port PWDB = 0x%lx\n",
				dm_pstable->rssi_val_min);
		}
	} else {
		dm_pstable->rssi_val_min =
		    rtlpriv->dm.entry_min_undec_sm_pwdb;

		rtl_dbg(rtlpriv, DBG_LOUD, DBG_LOUD,
			"AP Ext Port PWDB = 0x%lx\n",
			dm_pstable->rssi_val_min);
	}

	/* Power Saving for 92C */
	if (IS_92C_SERIAL(rtlhal->version))
		;/* rtl92c_dm_1r_cca(hw); */
	else
		rtl92c_dm_rf_saving(hw, false);
}

void rtl92c_dm_init(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);

	rtlpriv->dm.dm_type = DM_TYPE_BYDRIVER;
	rtlpriv->dm.dm_flag = DYNAMIC_FUNC_DISABLE | DYNAMIC_FUNC_DIG;
	rtlpriv->dm.undec_sm_pwdb = -1;
	rtlpriv->dm.undec_sm_cck = -1;
	rtlpriv->dm.dm_initialgain_enable = true;
	rtl_dm_diginit(hw, 0x20);

	rtlpriv->dm.dm_flag |= HAL_DM_HIPWR_DISABLE;
	rtl92c_dm_init_dynamic_txpower(hw);

	rtl92c_dm_init_edca_turbo(hw);
	rtl92c_dm_init_rate_adaptive_mask(hw);
	rtlpriv->dm.dm_flag |= DYNAMIC_FUNC_SS;
	rtl92c_dm_initialize_txpower_tracking(hw);
	rtl92c_dm_init_dynamic_bb_powersaving(hw);

	rtlpriv->dm.ofdm_pkt_cnt = 0;
	rtlpriv->dm.dm_rssi_sel = RSSI_DEFAULT;
}
EXPORT_SYMBOL(rtl92c_dm_init);

void rtl92c_dm_dynamic_txpower(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_phy *rtlphy = &(rtlpriv->phy);
	struct rtl_mac *mac = rtl_mac(rtl_priv(hw));
	long undec_sm_pwdb;

	if (!rtlpriv->dm.dynamic_txpower_enable)
		return;

	if (rtlpriv->dm.dm_flag & HAL_DM_HIPWR_DISABLE) {
		rtlpriv->dm.dynamic_txhighpower_lvl = TXHIGHPWRLEVEL_NORMAL;
		return;
	}

	if ((mac->link_state < MAC80211_LINKED) &&
	    (rtlpriv->dm.entry_min_undec_sm_pwdb == 0)) {
		rtl_dbg(rtlpriv, COMP_POWER, DBG_TRACE,
			"Not connected to any\n");

		rtlpriv->dm.dynamic_txhighpower_lvl = TXHIGHPWRLEVEL_NORMAL;

		rtlpriv->dm.last_dtp_lvl = TXHIGHPWRLEVEL_NORMAL;
		return;
	}

	if (mac->link_state >= MAC80211_LINKED) {
		if (mac->opmode == NL80211_IFTYPE_ADHOC) {
			undec_sm_pwdb = rtlpriv->dm.entry_min_undec_sm_pwdb;
			rtl_dbg(rtlpriv, COMP_POWER, DBG_LOUD,
				"AP Client PWDB = 0x%lx\n",
				undec_sm_pwdb);
		} else {
			undec_sm_pwdb = rtlpriv->dm.undec_sm_pwdb;
			rtl_dbg(rtlpriv, COMP_POWER, DBG_LOUD,
				"STA Default Port PWDB = 0x%lx\n",
				undec_sm_pwdb);
		}
	} else {
		undec_sm_pwdb = rtlpriv->dm.entry_min_undec_sm_pwdb;

		rtl_dbg(rtlpriv, COMP_POWER, DBG_LOUD,
			"AP Ext Port PWDB = 0x%lx\n",
			undec_sm_pwdb);
	}

	if (undec_sm_pwdb >= TX_POWER_NEAR_FIELD_THRESH_LVL2) {
		rtlpriv->dm.dynamic_txhighpower_lvl = TXHIGHPWRLEVEL_LEVEL2;
		rtl_dbg(rtlpriv, COMP_POWER, DBG_LOUD,
			"TXHIGHPWRLEVEL_LEVEL1 (TxPwr=0x0)\n");
	} else if ((undec_sm_pwdb < (TX_POWER_NEAR_FIELD_THRESH_LVL2 - 3)) &&
		   (undec_sm_pwdb >= TX_POWER_NEAR_FIELD_THRESH_LVL1)) {

		rtlpriv->dm.dynamic_txhighpower_lvl = TXHIGHPWRLEVEL_LEVEL1;
		rtl_dbg(rtlpriv, COMP_POWER, DBG_LOUD,
			"TXHIGHPWRLEVEL_LEVEL1 (TxPwr=0x10)\n");
	} else if (undec_sm_pwdb < (TX_POWER_NEAR_FIELD_THRESH_LVL1 - 5)) {
		rtlpriv->dm.dynamic_txhighpower_lvl = TXHIGHPWRLEVEL_NORMAL;
		rtl_dbg(rtlpriv, COMP_POWER, DBG_LOUD,
			"TXHIGHPWRLEVEL_NORMAL\n");
	}

	if ((rtlpriv->dm.dynamic_txhighpower_lvl != rtlpriv->dm.last_dtp_lvl)) {
		rtl_dbg(rtlpriv, COMP_POWER, DBG_LOUD,
			"PHY_SetTxPowerLevel8192S() Channel = %d\n",
			 rtlphy->current_channel);
		rtl92c_phy_set_txpower_level(hw, rtlphy->current_channel);
		if (rtlpriv->dm.dynamic_txhighpower_lvl ==
		    TXHIGHPWRLEVEL_NORMAL)
			dm_restorepowerindex(hw);
		else if (rtlpriv->dm.dynamic_txhighpower_lvl ==
			 TXHIGHPWRLEVEL_LEVEL1)
			dm_writepowerindex(hw, 0x14);
		else if (rtlpriv->dm.dynamic_txhighpower_lvl ==
			 TXHIGHPWRLEVEL_LEVEL2)
			dm_writepowerindex(hw, 0x10);
	}
	rtlpriv->dm.last_dtp_lvl = rtlpriv->dm.dynamic_txhighpower_lvl;
}

void rtl92c_dm_watchdog(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_ps_ctl *ppsc = rtl_psc(rtl_priv(hw));
	bool fw_current_inpsmode = false;
	bool fw_ps_awake = true;

	rtlpriv->cfg->ops->get_hw_reg(hw, HW_VAR_FW_PSMODE_STATUS,
				      (u8 *) (&fw_current_inpsmode));
	rtlpriv->cfg->ops->get_hw_reg(hw, HW_VAR_FWLPS_RF_ON,
				      (u8 *) (&fw_ps_awake));

	if (ppsc->p2p_ps_info.p2p_ps_mode)
		fw_ps_awake = false;

	if ((ppsc->rfpwr_state == ERFON) && ((!fw_current_inpsmode) &&
					     fw_ps_awake)
	    && (!ppsc->rfchange_inprogress)) {
		rtl92c_dm_pwdb_monitor(hw);
		rtl92c_dm_dig(hw);
		rtl92c_dm_false_alarm_counter_statistics(hw);
		rtl92c_dm_dynamic_bb_powersaving(hw);
		rtl92c_dm_dynamic_txpower(hw);
		rtl92c_dm_check_txpower_tracking(hw);
		/* rtl92c_dm_refresh_rate_adaptive_mask(hw); */
		rtl92c_dm_bt_coexist(hw);
		rtl92c_dm_check_edca_turbo(hw);
	}
}
EXPORT_SYMBOL(rtl92c_dm_watchdog);

u8 rtl92c_bt_rssi_state_change(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	long undec_sm_pwdb;
	u8 curr_bt_rssi_state = 0x00;

	if (rtlpriv->mac80211.link_state == MAC80211_LINKED) {
		undec_sm_pwdb = GET_UNDECORATED_AVERAGE_RSSI(rtlpriv);
	} else {
		if (rtlpriv->dm.entry_min_undec_sm_pwdb == 0)
			undec_sm_pwdb = 100;
		else
			undec_sm_pwdb = rtlpriv->dm.entry_min_undec_sm_pwdb;
	}

	/* Check RSSI to determine HighPower/NormalPower state for
	 * BT coexistence. */
	if (undec_sm_pwdb >= 67)
		curr_bt_rssi_state &= (~BT_RSSI_STATE_NORMAL_POWER);
	else if (undec_sm_pwdb < 62)
		curr_bt_rssi_state |= BT_RSSI_STATE_NORMAL_POWER;

	/* Check RSSI to determine AMPDU setting for BT coexistence. */
	if (undec_sm_pwdb >= 40)
		curr_bt_rssi_state &= (~BT_RSSI_STATE_AMDPU_OFF);
	else if (undec_sm_pwdb <= 32)
		curr_bt_rssi_state |= BT_RSSI_STATE_AMDPU_OFF;

	/* Marked RSSI state. It will be used to determine BT coexistence
	 * setting later. */
	if (undec_sm_pwdb < 35)
		curr_bt_rssi_state |=  BT_RSSI_STATE_SPECIAL_LOW;
	else
		curr_bt_rssi_state &= (~BT_RSSI_STATE_SPECIAL_LOW);

	/* Check BT state related to BT_Idle in B/G mode. */
	if (undec_sm_pwdb < 15)
		curr_bt_rssi_state |=  BT_RSSI_STATE_BG_EDCA_LOW;
	else
		curr_bt_rssi_state &= (~BT_RSSI_STATE_BG_EDCA_LOW);

	if (curr_bt_rssi_state != rtlpriv->btcoexist.bt_rssi_state) {
		rtlpriv->btcoexist.bt_rssi_state = curr_bt_rssi_state;
		return true;
	} else {
		return false;
	}
}
EXPORT_SYMBOL(rtl92c_bt_rssi_state_change);

static bool rtl92c_bt_state_change(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);

	u32 polling, ratio_tx, ratio_pri;
	u32 bt_tx, bt_pri;
	u8 bt_state;
	u8 cur_service_type;

	if (rtlpriv->mac80211.link_state < MAC80211_LINKED)
		return false;

	bt_state = rtl_read_byte(rtlpriv, 0x4fd);
	bt_tx = rtl_read_dword(rtlpriv, 0x488) & BT_MASK;
	bt_pri = rtl_read_dword(rtlpriv, 0x48c) & BT_MASK;
	polling = rtl_read_dword(rtlpriv, 0x490);

	if (bt_tx == BT_MASK && bt_pri == BT_MASK &&
	    polling == 0xffffffff && bt_state == 0xff)
		return false;

	bt_state &= BIT(0);
	if (bt_state != rtlpriv->btcoexist.bt_cur_state) {
		rtlpriv->btcoexist.bt_cur_state = bt_state;

		if (rtlpriv->btcoexist.reg_bt_sco == 3) {
			rtlpriv->btcoexist.bt_service = BT_IDLE;

			bt_state = bt_state |
			  ((rtlpriv->btcoexist.bt_ant_isolation == 1) ?
			  0 : BIT(1)) | BIT(2);
			rtl_write_byte(rtlpriv, 0x4fd, bt_state);
		}
		return true;
	}

	ratio_tx = bt_tx * 1000 / polling;
	ratio_pri = bt_pri * 1000 / polling;
	rtlpriv->btcoexist.ratio_tx = ratio_tx;
	rtlpriv->btcoexist.ratio_pri = ratio_pri;

	if (bt_state && rtlpriv->btcoexist.reg_bt_sco == 3) {

		if ((ratio_tx < 30)  && (ratio_pri < 30))
			cur_service_type = BT_IDLE;
		else if ((ratio_pri > 110) && (ratio_pri < 250))
			cur_service_type = BT_SCO;
		else if ((ratio_tx >= 200) && (ratio_pri >= 200))
			cur_service_type = BT_BUSY;
		else if ((ratio_tx >= 350) && (ratio_tx < 500))
			cur_service_type = BT_OTHERBUSY;
		else if (ratio_tx >= 500)
			cur_service_type = BT_PAN;
		else
			cur_service_type = BT_OTHER_ACTION;

		if (cur_service_type != rtlpriv->btcoexist.bt_service) {
			rtlpriv->btcoexist.bt_service = cur_service_type;
			bt_state = bt_state |
			   ((rtlpriv->btcoexist.bt_ant_isolation == 1) ?
			   0 : BIT(1)) |
			   ((rtlpriv->btcoexist.bt_service != BT_IDLE) ?
			   0 : BIT(2));

			/* Add interrupt migration when bt is not ini
			 * idle state (no traffic). */
			if (rtlpriv->btcoexist.bt_service != BT_IDLE) {
				rtl_write_word(rtlpriv, 0x504, 0x0ccc);
				rtl_write_byte(rtlpriv, 0x506, 0x54);
				rtl_write_byte(rtlpriv, 0x507, 0x54);
			} else {
				rtl_write_byte(rtlpriv, 0x506, 0x00);
				rtl_write_byte(rtlpriv, 0x507, 0x00);
			}

			rtl_write_byte(rtlpriv, 0x4fd, bt_state);
			return true;
		}
	}

	return false;

}

static bool rtl92c_bt_wifi_connect_change(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	static bool media_connect;

	if (rtlpriv->mac80211.link_state < MAC80211_LINKED) {
		media_connect = false;
	} else {
		if (!media_connect) {
			media_connect = true;
			return true;
		}
		media_connect = true;
	}

	return false;
}

static void rtl92c_bt_set_normal(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);

	if (rtlpriv->btcoexist.bt_service == BT_OTHERBUSY) {
		rtlpriv->btcoexist.bt_edca_ul = 0x5ea72b;
		rtlpriv->btcoexist.bt_edca_dl = 0x5ea72b;
	} else if (rtlpriv->btcoexist.bt_service == BT_BUSY) {
		rtlpriv->btcoexist.bt_edca_ul = 0x5eb82f;
		rtlpriv->btcoexist.bt_edca_dl = 0x5eb82f;
	} else if (rtlpriv->btcoexist.bt_service == BT_SCO) {
		if (rtlpriv->btcoexist.ratio_tx > 160) {
			rtlpriv->btcoexist.bt_edca_ul = 0x5ea72f;
			rtlpriv->btcoexist.bt_edca_dl = 0x5ea72f;
		} else {
			rtlpriv->btcoexist.bt_edca_ul = 0x5ea32b;
			rtlpriv->btcoexist.bt_edca_dl = 0x5ea42b;
		}
	} else {
		rtlpriv->btcoexist.bt_edca_ul = 0;
		rtlpriv->btcoexist.bt_edca_dl = 0;
	}

	if ((rtlpriv->btcoexist.bt_service != BT_IDLE) &&
	    (rtlpriv->mac80211.mode == WIRELESS_MODE_G ||
	     (rtlpriv->mac80211.mode == (WIRELESS_MODE_G | WIRELESS_MODE_B))) &&
	    (rtlpriv->btcoexist.bt_rssi_state &
	     BT_RSSI_STATE_BG_EDCA_LOW)) {
		rtlpriv->btcoexist.bt_edca_ul = 0x5eb82b;
		rtlpriv->btcoexist.bt_edca_dl = 0x5eb82b;
	}
}

static void rtl92c_bt_ant_isolation(struct ieee80211_hw *hw, u8 tmp1byte)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);

	/* Only enable HW BT coexist when BT in "Busy" state. */
	if (rtlpriv->mac80211.vendor == PEER_CISCO &&
	    rtlpriv->btcoexist.bt_service == BT_OTHER_ACTION) {
		rtl_write_byte(rtlpriv, REG_GPIO_MUXCFG, 0xa0);
	} else {
		if ((rtlpriv->btcoexist.bt_service == BT_BUSY) &&
		    (rtlpriv->btcoexist.bt_rssi_state &
		     BT_RSSI_STATE_NORMAL_POWER)) {
			rtl_write_byte(rtlpriv, REG_GPIO_MUXCFG, 0xa0);
		} else if ((rtlpriv->btcoexist.bt_service ==
			    BT_OTHER_ACTION) && (rtlpriv->mac80211.mode <
			    WIRELESS_MODE_N_24G) &&
			    (rtlpriv->btcoexist.bt_rssi_state &
			    BT_RSSI_STATE_SPECIAL_LOW)) {
			rtl_write_byte(rtlpriv, REG_GPIO_MUXCFG, 0xa0);
		} else {
			rtl_write_byte(rtlpriv, REG_GPIO_MUXCFG, tmp1byte);
		}
	}

	if (rtlpriv->btcoexist.bt_service == BT_PAN)
		rtl_write_dword(rtlpriv, REG_GPIO_PIN_CTRL, 0x10100);
	else
		rtl_write_dword(rtlpriv, REG_GPIO_PIN_CTRL, 0x0);

	if (rtlpriv->btcoexist.bt_rssi_state &
	    BT_RSSI_STATE_NORMAL_POWER) {
		rtl92c_bt_set_normal(hw);
	} else {
		rtlpriv->btcoexist.bt_edca_ul = 0;
		rtlpriv->btcoexist.bt_edca_dl = 0;
	}

	if (rtlpriv->btcoexist.bt_service != BT_IDLE) {
		rtlpriv->cfg->ops->set_rfreg(hw,
				 RF90_PATH_A,
				 0x1e,
				 0xf0, 0xf);
	} else {
		rtlpriv->cfg->ops->set_rfreg(hw,
		     RF90_PATH_A, 0x1e, 0xf0,
		     rtlpriv->btcoexist.bt_rfreg_origin_1e);
	}

	if (!rtlpriv->dm.dynamic_txpower_enable) {
		if (rtlpriv->btcoexist.bt_service != BT_IDLE) {
			if (rtlpriv->btcoexist.bt_rssi_state &
				BT_RSSI_STATE_TXPOWER_LOW) {
				rtlpriv->dm.dynamic_txhighpower_lvl =
							TXHIGHPWRLEVEL_BT2;
			} else {
				rtlpriv->dm.dynamic_txhighpower_lvl =
					TXHIGHPWRLEVEL_BT1;
			}
		} else {
			rtlpriv->dm.dynamic_txhighpower_lvl =
				TXHIGHPWRLEVEL_NORMAL;
		}
		rtl92c_phy_set_txpower_level(hw,
			rtlpriv->phy.current_channel);
	}
}

static void rtl92c_check_bt_change(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_hal *rtlhal = rtl_hal(rtl_priv(hw));
	u8 tmp1byte = 0;

	if (IS_81XXC_VENDOR_UMC_B_CUT(rtlhal->version) &&
	    rtlpriv->btcoexist.bt_coexistence)
		tmp1byte |= BIT(5);
	if (rtlpriv->btcoexist.bt_cur_state) {
		if (rtlpriv->btcoexist.bt_ant_isolation)
			rtl92c_bt_ant_isolation(hw, tmp1byte);
	} else {
		rtl_write_byte(rtlpriv, REG_GPIO_MUXCFG, tmp1byte);
		rtlpriv->cfg->ops->set_rfreg(hw, RF90_PATH_A, 0x1e, 0xf0,
				rtlpriv->btcoexist.bt_rfreg_origin_1e);

		rtlpriv->btcoexist.bt_edca_ul = 0;
		rtlpriv->btcoexist.bt_edca_dl = 0;
	}
}

void rtl92c_dm_bt_coexist(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	bool wifi_connect_change;
	bool bt_state_change;
	bool rssi_state_change;

	if ((rtlpriv->btcoexist.bt_coexistence) &&
	    (rtlpriv->btcoexist.bt_coexist_type == BT_CSR_BC4)) {
		wifi_connect_change = rtl92c_bt_wifi_connect_change(hw);
		bt_state_change = rtl92c_bt_state_change(hw);
		rssi_state_change = rtl92c_bt_rssi_state_change(hw);

		if (wifi_connect_change || bt_state_change || rssi_state_change)
			rtl92c_check_bt_change(hw);
	}
}
EXPORT_SYMBOL(rtl92c_dm_bt_coexist);
