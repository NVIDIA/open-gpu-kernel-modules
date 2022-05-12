/* SPDX-License-Identifier: GPL-2.0 */
/* Copyright(c) 2009-2012  Realtek Corporation.*/

#ifndef __RTL_BASE_H__
#define __RTL_BASE_H__

enum ap_peer {
	PEER_UNKNOWN = 0,
	PEER_RTL = 1,
	PEER_RTL_92SE = 2,
	PEER_BROAD = 3,
	PEER_RAL = 4,
	PEER_ATH = 5,
	PEER_CISCO = 6,
	PEER_MARV = 7,
	PEER_AIRGO = 9,
	PEER_MAX = 10,
};

#define RTL_DUMMY_OFFSET	0
#define RTL_DUMMY_UNIT		8
#define RTL_TX_DUMMY_SIZE	(RTL_DUMMY_OFFSET * RTL_DUMMY_UNIT)
#define RTL_TX_DESC_SIZE	32
#define RTL_TX_HEADER_SIZE	(RTL_TX_DESC_SIZE + RTL_TX_DUMMY_SIZE)

#define MAX_BIT_RATE_40MHZ_MCS15	300	/* Mbps */
#define MAX_BIT_RATE_40MHZ_MCS7		150	/* Mbps */

#define MAX_BIT_RATE_SHORT_GI_2NSS_80MHZ_MCS9	867	/* Mbps */
#define MAX_BIT_RATE_SHORT_GI_2NSS_80MHZ_MCS7	650	/* Mbps */
#define MAX_BIT_RATE_LONG_GI_2NSS_80MHZ_MCS9	780	/* Mbps */
#define MAX_BIT_RATE_LONG_GI_2NSS_80MHZ_MCS7	585	/* Mbps */

#define MAX_BIT_RATE_SHORT_GI_1NSS_80MHZ_MCS9	434	/* Mbps */
#define MAX_BIT_RATE_SHORT_GI_1NSS_80MHZ_MCS7	325	/* Mbps */
#define MAX_BIT_RATE_LONG_GI_1NSS_80MHZ_MCS9	390	/* Mbps */
#define MAX_BIT_RATE_LONG_GI_1NSS_80MHZ_MCS7	293	/* Mbps */

#define FRAME_OFFSET_FRAME_CONTROL	0
#define FRAME_OFFSET_DURATION		2
#define FRAME_OFFSET_ADDRESS1		4
#define FRAME_OFFSET_ADDRESS2		10
#define FRAME_OFFSET_ADDRESS3		16
#define FRAME_OFFSET_SEQUENCE		22
#define FRAME_OFFSET_ADDRESS4		24
#define MAX_LISTEN_INTERVAL		10
#define MAX_RATE_TRIES			4

#define SET_80211_PS_POLL_AID(_hdr, _val)		\
	(*(u16 *)((u8 *)(_hdr) + 2) = _val)
#define SET_80211_PS_POLL_BSSID(_hdr, _val)		\
	ether_addr_copy(((u8 *)(_hdr)) + 4, (u8 *)(_val))
#define SET_80211_PS_POLL_TA(_hdr, _val)		\
	ether_addr_copy(((u8 *)(_hdr))+10, (u8 *)(_val))

#define SET_80211_HDR_ADDRESS1(_hdr, _val)	\
	CP_MACADDR((u8 *)(_hdr)+FRAME_OFFSET_ADDRESS1, (u8 *)(_val))
#define SET_80211_HDR_ADDRESS2(_hdr, _val)	\
	CP_MACADDR((u8 *)(_hdr)+FRAME_OFFSET_ADDRESS2, (u8 *)(_val))
#define SET_80211_HDR_ADDRESS3(_hdr, _val)	\
	CP_MACADDR((u8 *)(_hdr)+FRAME_OFFSET_ADDRESS3, (u8 *)(_val))

#define SET_TX_DESC_SPE_RPT(__pdesc, __val)			\
	le32p_replace_bits((__le32 *)(__pdesc + 8), __val, BIT(19))
#define SET_TX_DESC_SW_DEFINE(__pdesc, __val)	\
	le32p_replace_bits((__le32 *)(__pdesc + 24), __val, GENMASK(11, 0))

int rtl_init_core(struct ieee80211_hw *hw);
void rtl_deinit_core(struct ieee80211_hw *hw);
void rtl_init_rx_config(struct ieee80211_hw *hw);
void rtl_init_rfkill(struct ieee80211_hw *hw);
void rtl_deinit_rfkill(struct ieee80211_hw *hw);

void rtl_watch_dog_timer_callback(struct timer_list *t);
void rtl_deinit_deferred_work(struct ieee80211_hw *hw, bool ips_wq);

bool rtl_action_proc(struct ieee80211_hw *hw, struct sk_buff *skb, u8 is_tx);
int rtlwifi_rate_mapping(struct ieee80211_hw *hw, bool isht,
			 bool isvht, u8 desc_rate);
bool rtl_tx_mgmt_proc(struct ieee80211_hw *hw, struct sk_buff *skb);
u8 rtl_is_special_data(struct ieee80211_hw *hw, struct sk_buff *skb, u8 is_tx,
		       bool is_enc);

void rtl_tx_ackqueue(struct ieee80211_hw *hw, struct sk_buff *skb);
bool rtl_is_tx_report_skb(struct ieee80211_hw *hw, struct sk_buff *skb);
void rtl_set_tx_report(struct rtl_tcb_desc *ptcb_desc, u8 *pdesc,
		       struct ieee80211_hw *hw, struct rtlwifi_tx_info *info);
void rtl_tx_report_handler(struct ieee80211_hw *hw, u8 *tmp_buf,
			   u8 c2h_cmd_len);
bool rtl_check_tx_report_acked(struct ieee80211_hw *hw);
void rtl_wait_tx_report_acked(struct ieee80211_hw *hw, u32 wait_ms);
u32 rtl_get_hal_edca_param(struct ieee80211_hw *hw,
			   struct ieee80211_vif *vif,
			   enum wireless_mode wirelessmode,
			   struct ieee80211_tx_queue_params *param);

void rtl_beacon_statistic(struct ieee80211_hw *hw, struct sk_buff *skb);
void rtl_collect_scan_list(struct ieee80211_hw *hw, struct sk_buff *skb);
void rtl_scan_list_expire(struct ieee80211_hw *hw);
int rtl_tx_agg_start(struct ieee80211_hw *hw, struct ieee80211_vif *vif,
	struct ieee80211_sta *sta, u16 tid, u16 *ssn);
int rtl_tx_agg_stop(struct ieee80211_hw *hw, struct ieee80211_vif *vif,
	struct ieee80211_sta *sta, u16 tid);
int rtl_tx_agg_oper(struct ieee80211_hw *hw,
		    struct ieee80211_sta *sta, u16 tid);
int rtl_rx_agg_start(struct ieee80211_hw *hw,
		     struct ieee80211_sta *sta, u16 tid);
int rtl_rx_agg_stop(struct ieee80211_hw *hw,
		    struct ieee80211_sta *sta, u16 tid);
void rtl_rx_ampdu_apply(struct rtl_priv *rtlpriv);
void rtl_c2hcmd_launcher(struct ieee80211_hw *hw, int exec);
void rtl_c2hcmd_enqueue(struct ieee80211_hw *hw, struct sk_buff *skb);

u8 rtl_mrate_idx_to_arfr_id(struct ieee80211_hw *hw, u8 rate_index,
			    enum wireless_mode wirelessmode);
void rtl_get_tcb_desc(struct ieee80211_hw *hw,
		      struct ieee80211_tx_info *info,
		      struct ieee80211_sta *sta,
		      struct sk_buff *skb, struct rtl_tcb_desc *tcb_desc);

int rtl_send_smps_action(struct ieee80211_hw *hw,
		struct ieee80211_sta *sta,
		enum ieee80211_smps_mode smps);
u8 *rtl_find_ie(u8 *data, unsigned int len, u8 ie);
void rtl_recognize_peer(struct ieee80211_hw *hw, u8 *data, unsigned int len);
u8 rtl_tid_to_ac(u8 tid);
void rtl_easy_concurrent_retrytimer_callback(struct timer_list *t);
extern struct rtl_global_var rtl_global_var;
void rtl_phy_scan_operation_backup(struct ieee80211_hw *hw, u8 operation);

#endif
