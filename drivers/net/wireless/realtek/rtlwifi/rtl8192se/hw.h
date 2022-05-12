/* SPDX-License-Identifier: GPL-2.0 */
/* Copyright(c) 2009-2012  Realtek Corporation.*/

#ifndef __REALTEK_PCI92SE_HW_H__
#define __REALTEK_PCI92SE_HW_H__

#define MSR_LINK_MANAGED   2
#define MSR_LINK_NONE      0
#define MSR_LINK_SHIFT     0
#define MSR_LINK_ADHOC     1
#define MSR_LINK_MASTER    3

enum WIRELESS_NETWORK_TYPE {
	WIRELESS_11B = 1,
	WIRELESS_11G = 2,
	WIRELESS_11A = 4,
	WIRELESS_11N = 8
};

void rtl92se_get_hw_reg(struct ieee80211_hw *hw,
			u8 variable, u8 *val);
void rtl92se_read_eeprom_info(struct ieee80211_hw *hw);
void rtl92se_interrupt_recognized(struct ieee80211_hw *hw,
				  struct rtl_int *int_vec);
int rtl92se_hw_init(struct ieee80211_hw *hw);
void rtl92se_card_disable(struct ieee80211_hw *hw);
void rtl92se_enable_interrupt(struct ieee80211_hw *hw);
void rtl92se_disable_interrupt(struct ieee80211_hw *hw);
int rtl92se_set_network_type(struct ieee80211_hw *hw,
			     enum nl80211_iftype type);
void rtl92se_set_check_bssid(struct ieee80211_hw *hw, bool check_bssid);
void rtl92se_set_mac_addr(struct rtl_io *io, const u8 *addr);
void rtl92se_set_qos(struct ieee80211_hw *hw, int aci);
void rtl92se_set_beacon_related_registers(struct ieee80211_hw *hw);
void rtl92se_set_beacon_interval(struct ieee80211_hw *hw);
void rtl92se_update_interrupt_mask(struct ieee80211_hw *hw,
				   u32 add_msr, u32 rm_msr);
void rtl92se_set_hw_reg(struct ieee80211_hw *hw, u8 variable,
			u8 *val);
void rtl92se_update_hal_rate_tbl(struct ieee80211_hw *hw,
		struct ieee80211_sta *sta, u8 rssi_level, bool update_bw);
void rtl92se_update_channel_access_setting(struct ieee80211_hw *hw);
bool rtl92se_gpio_radio_on_off_checking(struct ieee80211_hw *hw,
					u8 *valid);
void rtl8192se_gpiobit3_cfg_inputmode(struct ieee80211_hw *hw);
void rtl92se_enable_hw_security_config(struct ieee80211_hw *hw);
void rtl92se_set_key(struct ieee80211_hw *hw,
		     u32 key_index, u8 *macaddr, bool is_group,
		     u8 enc_algo, bool is_wepkey, bool clear_all);
void rtl92se_suspend(struct ieee80211_hw *hw);
void rtl92se_resume(struct ieee80211_hw *hw);

#endif
