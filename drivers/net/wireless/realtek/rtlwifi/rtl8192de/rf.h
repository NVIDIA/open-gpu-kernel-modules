/* SPDX-License-Identifier: GPL-2.0 */
/* Copyright(c) 2009-2012  Realtek Corporation.*/

#ifndef __RTL92D_RF_H__
#define __RTL92D_RF_H__

void rtl92d_phy_rf6052_set_bandwidth(struct ieee80211_hw *hw, u8 bandwidth);
void rtl92d_phy_rf6052_set_cck_txpower(struct ieee80211_hw *hw,
				       u8 *ppowerlevel);
void rtl92d_phy_rf6052_set_ofdm_txpower(struct ieee80211_hw *hw,
					u8 *ppowerlevel, u8 channel);
bool rtl92d_phy_rf6052_config(struct ieee80211_hw *hw);
bool rtl92d_phy_enable_anotherphy(struct ieee80211_hw *hw, bool bmac0);
void rtl92d_phy_powerdown_anotherphy(struct ieee80211_hw *hw, bool bmac0);

#endif
