/* SPDX-License-Identifier: GPL-2.0 */
/* Copyright(c) 2009-2012  Realtek Corporation.*/

#ifndef __REALTEK_PCI92SE_TRX_H__
#define __REALTEK_PCI92SE_TRX_H__

void rtl92se_tx_fill_desc(struct ieee80211_hw *hw,
			  struct ieee80211_hdr *hdr, u8 *pdesc,
			  u8 *pbd_desc_tx, struct ieee80211_tx_info *info,
			  struct ieee80211_sta *sta,
			  struct sk_buff *skb, u8 hw_queue,
			  struct rtl_tcb_desc *ptcb_desc);
void rtl92se_tx_fill_cmddesc(struct ieee80211_hw *hw, u8 *pdesc, bool firstseg,
			     bool lastseg, struct sk_buff *skb);
bool rtl92se_rx_query_desc(struct ieee80211_hw *hw, struct rtl_stats *stats,
			   struct ieee80211_rx_status *rx_status, u8 *pdesc,
			   struct sk_buff *skb);
void rtl92se_set_desc(struct ieee80211_hw *hw, u8 *pdesc, bool istx,
		      u8 desc_name, u8 *val);
u64 rtl92se_get_desc(struct ieee80211_hw *hw,
		     u8 *desc, bool istx, u8 desc_name);
void rtl92se_tx_polling(struct ieee80211_hw *hw, u8 hw_queue);

#endif
