/* SPDX-License-Identifier: GPL-2.0 */
/******************************************************************************
 *
 * Copyright(c) 2007 - 2011 Realtek Corporation. All rights reserved.
 *
 ******************************************************************************/
#ifndef __RTW_MLME_EXT_H_
#define __RTW_MLME_EXT_H_

#include <osdep_service.h>
#include <drv_types.h>
#include <wlan_bssdef.h>

/*	Commented by Albert 20101105 */
/*	Increase the SURVEY_TO value from 100 to 150  ( 100ms to 150ms ) */
/*	The Realtek 8188CE SoftAP will spend around 100ms to send the probe response after receiving the probe request. */
/*	So, this driver tried to extend the dwell time for each scanning channel. */
/*	This will increase the chance to receive the probe response from SoftAP. */

#define SURVEY_TO		(100)
#define REAUTH_TO		(300) /* 50) */
#define REASSOC_TO		(300) /* 50) */
/* define DISCONNECT_TO	(3000) */
#define ADDBA_TO			(2000)

#define LINKED_TO (1) /* unit:2 sec, 1x2=2 sec */

#define REAUTH_LIMIT	(4)
#define REASSOC_LIMIT	(4)
#define READDBA_LIMIT	(2)

#define ROAMING_LIMIT	8

#define	DYNAMIC_FUNC_DISABLE			(0x0)

/*  ====== ODM_ABILITY_E ======== */
/*  BB ODM section BIT 0-15 */
#define	DYNAMIC_BB_DIG				BIT(0)
#define	DYNAMIC_BB_RA_MASK			BIT(1)
#define	DYNAMIC_BB_DYNAMIC_TXPWR	BIT(2)
#define	DYNAMIC_BB_BB_FA_CNT			BIT(3)

#define		DYNAMIC_BB_RSSI_MONITOR		BIT(4)
#define		DYNAMIC_BB_CCK_PD			BIT(5)
#define		DYNAMIC_BB_ANT_DIV			BIT(6)
#define		DYNAMIC_BB_PWR_SAVE			BIT(7)
#define		DYNAMIC_BB_PWR_TRA			BIT(8)
#define		DYNAMIC_BB_RATE_ADAPTIVE		BIT(9)
#define		DYNAMIC_BB_PATH_DIV			BIT(10)
#define		DYNAMIC_BB_PSD				BIT(11)

/*  MAC DM section BIT 16-23 */
#define		DYNAMIC_MAC_EDCA_TURBO		BIT(16)
#define		DYNAMIC_MAC_EARLY_MODE		BIT(17)

/*  RF ODM section BIT 24-31 */
#define		DYNAMIC_RF_TX_PWR_TRACK		BIT(24)
#define		DYNAMIC_RF_RX_GAIN_TRACK		BIT(25)
#define		DYNAMIC_RF_CALIBRATION		BIT(26)

#define		DYNAMIC_ALL_FUNC_ENABLE		0xFFFFFFF

#define _HW_STATE_NOLINK_		0x00
#define _HW_STATE_ADHOC_		0x01
#define _HW_STATE_STATION_	0x02
#define _HW_STATE_AP_			0x03

#define		_1M_RATE_	0
#define		_2M_RATE_	1
#define		_5M_RATE_	2
#define		_11M_RATE_	3
#define		_6M_RATE_	4
#define		_9M_RATE_	5
#define		_12M_RATE_	6
#define		_18M_RATE_	7
#define		_24M_RATE_	8
#define		_36M_RATE_	9
#define		_48M_RATE_	10
#define		_54M_RATE_	11

extern const u8 RTW_WPA_OUI[];
extern const u8 WPS_OUI[];

/*  Channel Plan Type. */
/*  Note: */
/*	We just add new channel plan when the new channel plan is different
 *      from any of the following channel plan.
 */
/*	If you just want to customize the actions(scan period or join actions)
 *      about one of the channel plan,
 */
/*	customize them in struct rt_channel_info in the RT_CHANNEL_LIST. */
enum RT_CHANNEL_DOMAIN {
	/*  old channel plan mapping =====  */
	RT_CHANNEL_DOMAIN_FCC = 0x00,
	RT_CHANNEL_DOMAIN_IC = 0x01,
	RT_CHANNEL_DOMAIN_ETSI = 0x02,
	RT_CHANNEL_DOMAIN_SPAIN = 0x03,
	RT_CHANNEL_DOMAIN_FRANCE = 0x04,
	RT_CHANNEL_DOMAIN_MKK = 0x05,
	RT_CHANNEL_DOMAIN_MKK1 = 0x06,
	RT_CHANNEL_DOMAIN_ISRAEL = 0x07,
	RT_CHANNEL_DOMAIN_TELEC = 0x08,
	RT_CHANNEL_DOMAIN_GLOBAL_DOAMIN = 0x09,
	RT_CHANNEL_DOMAIN_WORLD_WIDE_13 = 0x0A,
	RT_CHANNEL_DOMAIN_TAIWAN = 0x0B,
	RT_CHANNEL_DOMAIN_CHINA = 0x0C,
	RT_CHANNEL_DOMAIN_SINGAPORE_INDIA_MEXICO = 0x0D,
	RT_CHANNEL_DOMAIN_KOREA = 0x0E,
	RT_CHANNEL_DOMAIN_TURKEY = 0x0F,
	RT_CHANNEL_DOMAIN_JAPAN = 0x10,
	RT_CHANNEL_DOMAIN_FCC_NO_DFS = 0x11,
	RT_CHANNEL_DOMAIN_JAPAN_NO_DFS = 0x12,
	RT_CHANNEL_DOMAIN_WORLD_WIDE_5G = 0x13,
	RT_CHANNEL_DOMAIN_TAIWAN_NO_DFS = 0x14,

	/*  new channel plan mapping, (2GDOMAIN_5GDOMAIN) ===== */
	RT_CHANNEL_DOMAIN_WORLD_NULL = 0x20,
	RT_CHANNEL_DOMAIN_ETSI1_NULL = 0x21,
	RT_CHANNEL_DOMAIN_FCC1_NULL = 0x22,
	RT_CHANNEL_DOMAIN_MKK1_NULL = 0x23,
	RT_CHANNEL_DOMAIN_ETSI2_NULL = 0x24,
	RT_CHANNEL_DOMAIN_FCC1_FCC1 = 0x25,
	RT_CHANNEL_DOMAIN_WORLD_ETSI1 = 0x26,
	RT_CHANNEL_DOMAIN_MKK1_MKK1 = 0x27,
	RT_CHANNEL_DOMAIN_WORLD_KCC1 = 0x28,
	RT_CHANNEL_DOMAIN_WORLD_FCC2 = 0x29,
	RT_CHANNEL_DOMAIN_WORLD_FCC3 = 0x30,
	RT_CHANNEL_DOMAIN_WORLD_FCC4 = 0x31,
	RT_CHANNEL_DOMAIN_WORLD_FCC5 = 0x32,
	RT_CHANNEL_DOMAIN_WORLD_FCC6 = 0x33,
	RT_CHANNEL_DOMAIN_FCC1_FCC7 = 0x34,
	RT_CHANNEL_DOMAIN_WORLD_ETSI2 = 0x35,
	RT_CHANNEL_DOMAIN_WORLD_ETSI3 = 0x36,
	RT_CHANNEL_DOMAIN_MKK1_MKK2 = 0x37,
	RT_CHANNEL_DOMAIN_MKK1_MKK3 = 0x38,
	RT_CHANNEL_DOMAIN_FCC1_NCC1 = 0x39,
	RT_CHANNEL_DOMAIN_FCC1_NCC2 = 0x40,
	RT_CHANNEL_DOMAIN_GLOBAL_DOAMIN_2G = 0x41,
	/*  Add new channel plan above this line=============== */
	RT_CHANNEL_DOMAIN_MAX,
	RT_CHANNEL_DOMAIN_REALTEK_DEFINE = 0x7F,
};

enum RT_CHANNEL_DOMAIN_2G {
	RT_CHANNEL_DOMAIN_2G_WORLD = 0x00,		/* Worldwide 13 */
	RT_CHANNEL_DOMAIN_2G_ETSI1 = 0x01,		/* Europe */
	RT_CHANNEL_DOMAIN_2G_FCC1 = 0x02,		/* US */
	RT_CHANNEL_DOMAIN_2G_MKK1 = 0x03,		/* Japan */
	RT_CHANNEL_DOMAIN_2G_ETSI2 = 0x04,		/* France */
	RT_CHANNEL_DOMAIN_2G_NULL = 0x05,
	/*  Add new channel plan above this line=============== */
	RT_CHANNEL_DOMAIN_2G_MAX,
};

#define rtw_is_channel_plan_valid(chplan)			\
	(chplan < RT_CHANNEL_DOMAIN_MAX ||			\
	 chplan == RT_CHANNEL_DOMAIN_REALTEK_DEFINE)

struct rt_channel_plan {
	unsigned char	Channel[MAX_CHANNEL_NUM];
	unsigned char	Len;
};

struct rt_channel_plan_2g {
	unsigned char	Channel[MAX_CHANNEL_NUM_2G];
	unsigned char	Len;
};

struct rt_channel_plan_map {
	unsigned char	Index2G;
};

enum Associated_AP {
	atherosAP	= 0,
	broadcomAP	= 1,
	ciscoAP		= 2,
	marvellAP	= 3,
	ralinkAP	= 4,
	realtekAP	= 5,
	airgocapAP	= 6,
	unknownAP	= 7,
	maxAP,
};

enum HT_IOT_PEER {
	HT_IOT_PEER_UNKNOWN		= 0,
	HT_IOT_PEER_REALTEK		= 1,
	HT_IOT_PEER_REALTEK_92SE	= 2,
	HT_IOT_PEER_BROADCOM		= 3,
	HT_IOT_PEER_RALINK		= 4,
	HT_IOT_PEER_ATHEROS		= 5,
	HT_IOT_PEER_CISCO		= 6,
	HT_IOT_PEER_MERU		= 7,
	HT_IOT_PEER_MARVELL		= 8,
	HT_IOT_PEER_REALTEK_SOFTAP	= 9,/*  peer is RealTek SOFT_AP */
	HT_IOT_PEER_SELF_SOFTAP		= 10, /*  Self is SoftAP */
	HT_IOT_PEER_AIRGO		= 11,
	HT_IOT_PEER_INTEL		= 12,
	HT_IOT_PEER_RTK_APCLIENT	= 13,
	HT_IOT_PEER_REALTEK_81XX	= 14,
	HT_IOT_PEER_REALTEK_WOW		= 15,
	HT_IOT_PEER_TENDA		= 16,
	HT_IOT_PEER_MAX			= 17
};

enum SCAN_STATE {
	SCAN_DISABLE = 0,
	SCAN_START = 1,
	SCAN_TXNULL = 2,
	SCAN_PROCESS = 3,
	SCAN_COMPLETE = 4,
	SCAN_STATE_MAX,
};

struct mlme_handler {
	unsigned int num;
	const char *str;
	unsigned int (*func)(struct adapter *adapt, struct recv_frame *frame);
};

struct action_handler {
	unsigned int num;
	const char *str;
	unsigned int (*func)(struct adapter *adapt, struct recv_frame *frame);
};

struct ss_res {
	int state;
	int bss_cnt;
	int channel_idx;
	int scan_mode;
	u8 ssid_num;
	u8 ch_num;
	struct ndis_802_11_ssid ssid[RTW_SSID_SCAN_AMOUNT];
	struct rtw_ieee80211_channel ch[RTW_CHANNEL_SCAN_AMOUNT];
};

/* define AP_MODE				0x0C */
/* define STATION_MODE	0x08 */
/* define AD_HOC_MODE		0x04 */
/* define NO_LINK_MODE	0x00 */

#define WIFI_FW_NULL_STATE		_HW_STATE_NOLINK_
#define	WIFI_FW_STATION_STATE		_HW_STATE_STATION_
#define	WIFI_FW_AP_STATE		_HW_STATE_AP_
#define	WIFI_FW_ADHOC_STATE		_HW_STATE_ADHOC_

#define	WIFI_FW_AUTH_NULL		0x00000100
#define	WIFI_FW_AUTH_STATE		0x00000200
#define	WIFI_FW_AUTH_SUCCESS		0x00000400

#define	WIFI_FW_ASSOC_STATE		0x00002000
#define	WIFI_FW_ASSOC_SUCCESS		0x00004000

#define	WIFI_FW_LINKING_STATE		(WIFI_FW_AUTH_NULL |		\
					WIFI_FW_AUTH_STATE |		\
					WIFI_FW_AUTH_SUCCESS |		\
					WIFI_FW_ASSOC_STATE)

struct FW_Sta_Info {
	struct sta_info	*psta;
	u32	status;
	u32	rx_pkt;
	u32	retry;
	unsigned char SupportedRates[NDIS_802_11_LENGTH_RATES_EX];
};

/*
 * Usage:
 * When one iface acted as AP mode and the other iface is STA mode and scanning,
 * it should switch back to AP's operating channel periodically.
 * Parameters info:
 * When the driver scanned RTW_SCAN_NUM_OF_CH channels, it would switch back to
 * AP's operating channel for
 * RTW_STAY_AP_CH_MILLISECOND * SURVEY_TO milliseconds.
 * Example:
 * For chip supports 2.4G + 5GHz and AP mode is operating in channel 1,
 * RTW_SCAN_NUM_OF_CH is 8, RTW_STAY_AP_CH_MS is 3 and SURVEY_TO is 100.
 * When it's STA mode gets set_scan command,
 * it would
 * 1. Doing the scan on channel 1.2.3.4.5.6.7.8
 * 2. Back to channel 1 for 300 milliseconds
 * 3. Go through doing site survey on channel 9.10.11.36.40.44.48.52
 * 4. Back to channel 1 for 300 milliseconds
 * 5. ... and so on, till survey done.
 */

struct mlme_ext_info {
	u32	state;
	u32	reauth_count;
	u32	reassoc_count;
	u32	link_count;
	u32	auth_seq;
	u32	auth_algo;	/*  802.11 auth, could be open, shared, auto */
	u32	authModeToggle;
	u32	enc_algo;/* encrypt algorithm; */
	u32	key_index;	/*  this is only valid for legacy wep,
				 *  0~3 for key id.
				 */
	u32	iv;
	u8	chg_txt[128];
	u16	aid;
	u16	bcn_interval;
	u16	capability;
	u8	assoc_AP_vendor;
	u8	slotTime;
	u8	preamble_mode;
	u8	WMM_enable;
	u8	ERP_enable;
	u8	ERP_IE;
	u8	HT_enable;
	u8	HT_caps_enable;
	u8	HT_info_enable;
	u8	HT_protection;
	u8	turboMode_cts2self;
	u8	turboMode_rtsen;
	u8	SM_PS;
	u8	agg_enable_bitmap;
	u8	ADDBA_retry_count;
	u8	candidate_tid_bitmap;
	u8	dialogToken;
	/*  Accept ADDBA Request */
	bool accept_addba_req;
	u8	bwmode_updated;
	u8	hidden_ssid_mode;

	struct ADDBA_request	ADDBA_req;
	struct WMM_para_element	WMM_param;
	struct ieee80211_ht_cap HT_caps;
	struct HT_info_element	HT_info;
	struct wlan_bssid_ex	network;/* join network or bss_network,
					 * if in ap mode, it is the same
					 * as cur_network.network
					 */
	struct FW_Sta_Info	FW_sta_info[NUM_STA];
};

/*  The channel information about this channel including joining,
 *  scanning, and power constraints.
 */
struct rt_channel_info {
	u8	ChannelNum;	/*  The channel number. */
	enum rt_scan_type ScanType;	/*  Scan type such as passive
					 *  or active scan.
					 */
	u32	rx_count;
};

int rtw_ch_set_search_ch(struct rt_channel_info *ch_set, const u32 ch);

/*  P2P_MAX_REG_CLASSES - Maximum number of regulatory classes */
#define P2P_MAX_REG_CLASSES 10

/*  P2P_MAX_REG_CLASS_CHANNELS - Maximum number of chan per regulatory class */
#define P2P_MAX_REG_CLASS_CHANNELS 20

/*   struct p2p_channels - List of supported channels */
struct p2p_channels {
	/*  struct p2p_reg_class - Supported regulatory class */
	struct p2p_reg_class {
		/*  reg_class - Regulatory class (IEEE 802.11-2007, Annex J) */
		u8 reg_class;

		/*  channel - Supported channels */
		u8 channel[P2P_MAX_REG_CLASS_CHANNELS];

		/*  channels - Number of channel entries in use */
		size_t channels;
	} reg_class[P2P_MAX_REG_CLASSES];

	/*  reg_classes - Number of reg_class entries in use */
	size_t reg_classes;
};

struct p2p_oper_class_map {
	enum hw_mode {IEEE80211G} mode;
	u8 op_class;
	u8 min_chan;
	u8 max_chan;
	u8 inc;
	enum {BW20, BW40PLUS, BW40MINUS} bw;
};

struct mlme_ext_priv {
	struct adapter	*padapter;
	u8	mlmeext_init;
	atomic_t	event_seq;
	u16	mgnt_seq;

	unsigned char	cur_channel;
	unsigned char	cur_bwmode;
	unsigned char	cur_ch_offset;/* PRIME_CHNL_OFFSET */
	unsigned char	cur_wireless_mode;	/*  NETWORK_TYPE */

	unsigned char	oper_channel; /* saved chan info when call
				       * set_channel_bw
				       */
	unsigned char	oper_bwmode;
	unsigned char	oper_ch_offset;/* PRIME_CHNL_OFFSET */

	unsigned char	max_chan_nums;
	struct rt_channel_info channel_set[MAX_CHANNEL_NUM];
	struct p2p_channels channel_list;
	unsigned char	basicrate[NumRates];
	unsigned char	datarate[NumRates];

	struct ss_res		sitesurvey_res;
	struct mlme_ext_info	mlmext_info;/* for sta/adhoc mode, including
					     * current scan/connecting/connected
					     * related info. For ap mode,
					     * network includes ap's cap_info
					     */
	struct timer_list survey_timer;
	struct timer_list link_timer;
	u16	chan_scan_time;

	u8	scan_abort;
	u8	tx_rate; /*  TXRATE when USERATE is set. */

	u32	retry; /* retry for issue probereq */

	u64 TSFValue;

#ifdef CONFIG_88EU_AP_MODE
	unsigned char bstart_bss;
#endif
	u8 update_channel_plan_by_ap_done;
	/* recv_decache check for Action_public frame */
	u8 action_public_dialog_token;
	u16	 action_public_rxseq;
	u8 active_keep_alive_check;
};

int init_mlme_ext_priv(struct adapter *adapter);
int init_hw_mlme_ext(struct adapter *padapter);
void free_mlme_ext_priv(struct mlme_ext_priv *pmlmeext);
void init_mlme_ext_timer(struct adapter *padapter);
void init_addba_retry_timer(struct adapter *adapt, struct sta_info *sta);
struct xmit_frame *alloc_mgtxmitframe(struct xmit_priv *pxmitpriv);

unsigned char networktype_to_raid(unsigned char network_type);
u8 judge_network_type(struct adapter *padapter, unsigned char *rate);
void get_rate_set(struct adapter *padapter, unsigned char *pbssrate, int *len);
void UpdateBrateTbl(struct adapter *padapter, u8 *mBratesOS);
void UpdateBrateTblForSoftAP(u8 *bssrateset, u32 bssratelen);

void Save_DM_Func_Flag(struct adapter *padapter);
void Restore_DM_Func_Flag(struct adapter *padapter);
void Switch_DM_Func(struct adapter *padapter, u32 mode, u8 enable);

void Set_MSR(struct adapter *padapter, u8 type);

u8 rtw_get_oper_ch(struct adapter *adapter);
void rtw_set_oper_ch(struct adapter *adapter, u8 ch);
void rtw_set_oper_bw(struct adapter *adapter, u8 bw);
void rtw_set_oper_choffset(struct adapter *adapter, u8 offset);

void set_channel_bwmode(struct adapter *padapter, unsigned char channel,
			unsigned char channel_offset, unsigned short bwmode);
void SelectChannel(struct adapter *padapter, unsigned char channel);
void SetBWMode(struct adapter *padapter, unsigned short bwmode,
	       unsigned char channel_offset);

unsigned int decide_wait_for_beacon_timeout(unsigned int bcn_interval);

void write_cam(struct adapter *padapter, u8 entry, u16 ctrl, u8 *mac, u8 *key);
void clear_cam_entry(struct adapter *padapter, u8 entry);

void invalidate_cam_all(struct adapter *padapter);

int allocate_fw_sta_entry(struct adapter *padapter);
void flush_all_cam_entry(struct adapter *padapter);

void update_network(struct wlan_bssid_ex *dst, struct wlan_bssid_ex *src,
		    struct adapter *adapter, bool update_ie);

u16 get_beacon_interval(struct wlan_bssid_ex *bss);

int is_client_associated_to_ap(struct adapter *padapter);
int is_client_associated_to_ibss(struct adapter *padapter);
int is_IBSS_empty(struct adapter *padapter);

unsigned char check_assoc_AP(u8 *pframe, uint len);

int WMM_param_handler(struct adapter *padapter, struct ndis_802_11_var_ie *pIE);
void WMMOnAssocRsp(struct adapter *padapter);

void HT_caps_handler(struct adapter *padapter, struct ndis_802_11_var_ie *pIE);
void HT_info_handler(struct adapter *padapter, struct ndis_802_11_var_ie *pIE);
void HTOnAssocRsp(struct adapter *padapter);

void ERP_IE_handler(struct adapter *padapter, struct ndis_802_11_var_ie *pIE);
void VCS_update(struct adapter *padapter, struct sta_info *psta);

void update_beacon_info(struct adapter *padapter, u8 *pframe, uint len,
			struct sta_info *psta);
int rtw_check_bcn_info(struct adapter  *Adapter, u8 *pframe, u32 packet_len);
void update_IOT_info(struct adapter *padapter);
void update_capinfo(struct adapter *adapter, u16 updatecap);
void update_wireless_mode(struct adapter *padapter);
void update_tx_basic_rate(struct adapter *padapter, u8 modulation);
void update_bmc_sta_support_rate(struct adapter *padapter, u32 mac_id);
int update_sta_support_rate(struct adapter *padapter, u8 *pvar_ie,
			    uint var_ie_len, int cam_idx);

/* for sta/adhoc mode */
void update_sta_info(struct adapter *padapter, struct sta_info *psta);
unsigned int update_basic_rate(unsigned char *ptn, unsigned int ptn_sz);
unsigned int update_supported_rate(unsigned char *ptn, unsigned int ptn_sz);
unsigned int update_MSC_rate(struct ieee80211_ht_cap *pHT_caps);
void Update_RA_Entry(struct adapter *padapter, u32 mac_id);
void set_sta_rate(struct adapter *padapter, struct sta_info *psta);

unsigned char get_highest_rate_idx(u32 mask);
int support_short_GI(struct adapter *padapter, struct ieee80211_ht_cap *caps);
unsigned int is_ap_in_tkip(struct adapter *padapter);

void report_join_res(struct adapter *padapter, int res);
void report_survey_event(struct adapter *padapter,
			 struct recv_frame *precv_frame);
void report_surveydone_event(struct adapter *padapter);
void report_del_sta_event(struct adapter *padapter,
			  unsigned char *addr, unsigned short reason);
void report_add_sta_event(struct adapter *padapter, unsigned char *addr,
			  int cam_idx);

u8 set_tx_beacon_cmd(struct adapter *padapter);
unsigned int setup_beacon_frame(struct adapter *padapter,
				unsigned char *beacon_frame);
void update_mgnt_tx_rate(struct adapter *padapter, u8 rate);
void update_mgntframe_attrib(struct adapter *padapter,
			     struct pkt_attrib *pattrib);

int issue_nulldata(struct adapter *padapter, unsigned char *da,
		   unsigned int power_mode, int try_cnt, int wait_ms);
int issue_qos_nulldata(struct adapter *padapter, unsigned char *da,
		       u16 tid, int try_cnt, int wait_ms);
int issue_deauth(struct adapter *padapter, unsigned char *da,
		 unsigned short reason);
unsigned int send_delba(struct adapter *padapter, u8 initiator, u8 *addr);
unsigned int send_beacon(struct adapter *padapter);

void mlmeext_joinbss_event_callback(struct adapter *padapter, int join_res);
void mlmeext_sta_del_event_callback(struct adapter *padapter);
void mlmeext_sta_add_event_callback(struct adapter *padapter,
				    struct sta_info *psta);

void linked_status_chk(struct adapter *padapter);

void survey_timer_hdl(struct timer_list *t);
void link_timer_hdl(struct timer_list *t);
void addba_timer_hdl(struct timer_list *t);

#define set_survey_timer(mlmeext, ms) \
	mod_timer(&mlmeext->survey_timer, jiffies +	\
		  msecs_to_jiffies(ms))

#define set_link_timer(mlmeext, ms) \
	mod_timer(&mlmeext->link_timer, jiffies +	\
		  msecs_to_jiffies(ms))

void process_addba_req(struct adapter *padapter, u8 *paddba_req, u8 *addr);

void update_TSF(struct mlme_ext_priv *pmlmeext, u8 *pframe, uint len);
void correct_TSF(struct adapter *padapter, struct mlme_ext_priv *pmlmeext);

struct cmd_hdl {
	uint	parmsize;
	u8 (*h2cfuns)(struct adapter  *padapter, u8 *pbuf);
};

u8 read_macreg_hdl(struct adapter *padapter, u8 *pbuf);
u8 write_macreg_hdl(struct adapter *padapter, u8 *pbuf);
u8 read_bbreg_hdl(struct adapter *padapter, u8 *pbuf);
u8 write_bbreg_hdl(struct adapter *padapter, u8 *pbuf);
u8 read_rfreg_hdl(struct adapter *padapter, u8 *pbuf);
u8 write_rfreg_hdl(struct adapter *padapter, u8 *pbuf);
u8 join_cmd_hdl(struct adapter *padapter, u8 *pbuf);
u8 disconnect_hdl(struct adapter *padapter, u8 *pbuf);
u8 createbss_hdl(struct adapter *padapter, u8 *pbuf);
u8 setopmode_hdl(struct adapter *padapter, u8 *pbuf);
u8 sitesurvey_cmd_hdl(struct adapter *padapter, u8 *pbuf);
u8 setauth_hdl(struct adapter *padapter, u8 *pbuf);
u8 setkey_hdl(struct adapter *padapter, u8 *pbuf);
u8 set_stakey_hdl(struct adapter *padapter, u8 *pbuf);
u8 set_assocsta_hdl(struct adapter *padapter, u8 *pbuf);
u8 del_assocsta_hdl(struct adapter *padapter, u8 *pbuf);
u8 add_ba_hdl(struct adapter *padapter, unsigned char *pbuf);

u8 mlme_evt_hdl(struct adapter *padapter, unsigned char *pbuf);
u8 h2c_msg_hdl(struct adapter *padapter, unsigned char *pbuf);
u8 tx_beacon_hdl(struct adapter *padapter, unsigned char *pbuf);
u8 set_ch_hdl(struct adapter *padapter, u8 *pbuf);
u8 set_chplan_hdl(struct adapter *padapter, unsigned char *pbuf);
u8 led_blink_hdl(struct adapter *padapter, unsigned char *pbuf);
/* Handling DFS channel switch announcement ie. */
u8 set_csa_hdl(struct adapter *padapter, unsigned char *pbuf);
u8 tdls_hdl(struct adapter *padapter, unsigned char *pbuf);

struct C2HEvent_Header {
#ifdef __LITTLE_ENDIAN
	unsigned int len:16;
	unsigned int ID:8;
	unsigned int seq:8;
#elif defined(__BIG_ENDIAN)
	unsigned int seq:8;
	unsigned int ID:8;
	unsigned int len:16;
#endif
	unsigned int rsvd;
};

void rtw_dummy_event_callback(struct adapter *adapter, u8 *pbuf);
void rtw_fwdbg_event_callback(struct adapter *adapter, u8 *pbuf);

enum rtw_c2h_event {
	_Read_MACREG_EVT_ = 0, /*0*/
	_Read_BBREG_EVT_,
	_Read_RFREG_EVT_,
	_Read_EEPROM_EVT_,
	_Read_EFUSE_EVT_,
	_Read_CAM_EVT_,	/*5*/
	_Get_BasicRate_EVT_,
	_Get_DataRate_EVT_,
	_Survey_EVT_,	 /*8*/
	_SurveyDone_EVT_,	 /*9*/

	_JoinBss_EVT_, /*10*/
	_AddSTA_EVT_,
	_DelSTA_EVT_,
	_AtimDone_EVT_,
	_TX_Report_EVT_,
	_CCX_Report_EVT_,		/*15*/
	_DTM_Report_EVT_,
	_TX_Rate_Statistics_EVT_,
	_C2HLBK_EVT_,
	_FWDBG_EVT_,
	_C2HFEEDBACK_EVT_,             /*20*/
	_ADDBA_EVT_,
	_C2HBCN_EVT_,
	_ReportPwrState_EVT_,	/* filen: only for PCIE, USB */
	_CloseRF_EVT_,		/* filen: only for PCIE,
				 * work around ASPM
				 */
	MAX_C2HEVT
};

#ifdef _RTW_MLME_EXT_C_

static struct fwevent wlanevents[] = {
	{0, rtw_dummy_event_callback},	/*0*/
	{0, NULL},
	{0, NULL},
	{0, NULL},
	{0, NULL},
	{0, NULL},
	{0, NULL},
	{0, NULL},
	{0, &rtw_survey_event_callback},		/*8*/
	{sizeof(struct surveydone_event), &rtw_surveydone_event_callback},/*9*/
	{0, &rtw_joinbss_event_callback},		/*10*/
	{sizeof(struct stassoc_event), &rtw_stassoc_event_callback},
	{sizeof(struct stadel_event), &rtw_stadel_event_callback},
	{0, &rtw_atimdone_event_callback},
	{0, rtw_dummy_event_callback},
	{0, NULL},	/*15*/
	{0, NULL},
	{0, NULL},
	{0, NULL},
	{0, rtw_fwdbg_event_callback},
	{0, NULL},	 /*20*/
	{0, NULL},
	{0, NULL},
	{0, &rtw_cpwm_event_callback},
	{0, NULL},
};

#endif/* _RTL_MLME_EXT_C_ */

#endif /* __RTW_MLME_EXT_H_ */
