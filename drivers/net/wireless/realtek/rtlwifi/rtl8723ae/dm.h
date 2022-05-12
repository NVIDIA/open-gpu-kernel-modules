/* SPDX-License-Identifier: GPL-2.0 */
/* Copyright(c) 2009-2012  Realtek Corporation.*/

#ifndef	__RTL8723E_DM_H__
#define __RTL8723E_DM_H__

#define HAL_DM_DIG_DISABLE			BIT(0)
#define HAL_DM_HIPWR_DISABLE			BIT(1)

#define OFDM_TABLE_LENGTH			37
#define CCK_TABLE_LENGTH			33

#define OFDM_TABLE_SIZE				37
#define CCK_TABLE_SIZE				33

#define BW_AUTO_SWITCH_HIGH_LOW			25
#define BW_AUTO_SWITCH_LOW_HIGH			30

#define DM_DIG_FA_UPPER				0x32
#define DM_DIG_FA_LOWER				0x20
#define DM_DIG_FA_TH0				0x20
#define DM_DIG_FA_TH1				0x100
#define DM_DIG_FA_TH2				0x200

#define RXPATHSELECTION_SS_TH_LOW		30
#define RXPATHSELECTION_DIFF_TH			18

#define DM_RATR_STA_INIT			0
#define DM_RATR_STA_HIGH			1
#define DM_RATR_STA_MIDDLE			2
#define DM_RATR_STA_LOW				3

#define CTS2SELF_THVAL				30
#define REGC38_TH				20

#define WAIOTTHVAL				25

#define TXHIGHPWRLEVEL_NORMAL			0
#define TXHIGHPWRLEVEL_LEVEL1			1
#define TXHIGHPWRLEVEL_LEVEL2			2
#define TXHIGHPWRLEVEL_BT1			3
#define TXHIGHPWRLEVEL_BT2			4

#define DM_TYPE_BYFW				0
#define DM_TYPE_BYDRIVER			1

#define TX_POWER_NEAR_FIELD_THRESH_LVL2		74
#define TX_POWER_NEAR_FIELD_THRESH_LVL1		67

struct swat_t {
	u8 failure_cnt;
	u8 try_flag;
	u8 stop_trying;
	long pre_rssi;
	long trying_threshold;
	u8 cur_antenna;
	u8 pre_antenna;

};

enum tag_dynamic_init_gain_operation_type_definition {
	DIG_TYPE_THRESH_HIGH = 0,
	DIG_TYPE_THRESH_LOW = 1,
	DIG_TYPE_BACKOFF = 2,
	DIG_TYPE_RX_GAIN_MIN = 3,
	DIG_TYPE_RX_GAIN_MAX = 4,
	DIG_TYPE_ENABLE = 5,
	DIG_TYPE_DISABLE = 6,
	DIG_OP_TYPE_MAX
};

enum dm_1r_cca_e {
	CCA_1R = 0,
	CCA_2R = 1,
	CCA_MAX = 2,
};

enum dm_rf_e {
	RF_SAVE = 0,
	RF_NORMAL = 1,
	RF_MAX = 2,
};

enum dm_sw_ant_switch_e {
	ANS_ANTENNA_B = 1,
	ANS_ANTENNA_A = 2,
	ANS_ANTENNA_MAX = 3,
};

#define BT_RSSI_STATE_NORMAL_POWER      BIT_OFFSET_LEN_MASK_32(0, 1)
#define BT_RSSI_STATE_AMDPU_OFF         BIT_OFFSET_LEN_MASK_32(1, 1)
#define BT_RSSI_STATE_SPECIAL_LOW       BIT_OFFSET_LEN_MASK_32(2, 1)
#define BT_RSSI_STATE_BG_EDCA_LOW       BIT_OFFSET_LEN_MASK_32(3, 1)
#define BT_RSSI_STATE_TXPOWER_LOW       BIT_OFFSET_LEN_MASK_32(4, 1)
#define GET_UNDECORATED_AVERAGE_RSSI(_priv)     \
	( \
	(((struct rtl_priv *)(_priv))->mac80211.opmode ==		\
			     NL80211_IFTYPE_ADHOC) ?			\
	(((struct rtl_priv *)(_priv))->dm.entry_min_undec_sm_pwdb) :	\
	(((struct rtl_priv *)(_priv))->dm.undec_sm_pwdb)		\
	)

void rtl8723e_dm_init(struct ieee80211_hw *hw);
void rtl8723e_dm_watchdog(struct ieee80211_hw *hw);
void rtl8723e_dm_write_dig(struct ieee80211_hw *hw);
void rtl8723e_dm_check_txpower_tracking(struct ieee80211_hw *hw);
void rtl8723e_dm_init_rate_adaptive_mask(struct ieee80211_hw *hw);
void rtl8723e_dm_rf_saving(struct ieee80211_hw *hw, u8 bforce_in_normal);
void rtl8723e_dm_bt_coexist(struct ieee80211_hw *hw);
#endif
