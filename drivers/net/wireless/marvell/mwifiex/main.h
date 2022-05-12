/*
 * NXP Wireless LAN device driver: major data structures and prototypes
 *
 * Copyright 2011-2020 NXP
 *
 * This software file (the "File") is distributed by NXP
 * under the terms of the GNU General Public License Version 2, June 1991
 * (the "License").  You may use, redistribute and/or modify this File in
 * accordance with the terms and conditions of the License, a copy of which
 * is available by writing to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA or on the
 * worldwide web at http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt.
 *
 * THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE
 * ARE EXPRESSLY DISCLAIMED.  The License provides additional details about
 * this warranty disclaimer.
 */

#ifndef _MWIFIEX_MAIN_H_
#define _MWIFIEX_MAIN_H_

#include <linux/completion.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/semaphore.h>
#include <linux/ip.h>
#include <linux/skbuff.h>
#include <linux/if_arp.h>
#include <linux/etherdevice.h>
#include <net/sock.h>
#include <net/lib80211.h>
#include <linux/vmalloc.h>
#include <linux/firmware.h>
#include <linux/ctype.h>
#include <linux/of.h>
#include <linux/idr.h>
#include <linux/inetdevice.h>
#include <linux/devcoredump.h>
#include <linux/err.h>
#include <linux/gpio.h>
#include <linux/gfp.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/of_gpio.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/pm_runtime.h>
#include <linux/slab.h>
#include <linux/of_irq.h>

#include "decl.h"
#include "ioctl.h"
#include "util.h"
#include "fw.h"
#include "pcie.h"
#include "usb.h"
#include "sdio.h"

extern const char driver_version[];
extern bool mfg_mode;
extern bool aggr_ctrl;

struct mwifiex_adapter;
struct mwifiex_private;

enum {
	MWIFIEX_ASYNC_CMD,
	MWIFIEX_SYNC_CMD
};

#define MWIFIEX_DRIVER_MODE_STA			BIT(0)
#define MWIFIEX_DRIVER_MODE_UAP			BIT(1)
#define MWIFIEX_DRIVER_MODE_P2P			BIT(2)
#define MWIFIEX_DRIVER_MODE_BITMASK		(BIT(0) | BIT(1) | BIT(2))

#define MWIFIEX_MAX_AP				64

#define MWIFIEX_MAX_PKTS_TXQ			16

#define MWIFIEX_DEFAULT_WATCHDOG_TIMEOUT	(5 * HZ)

#define MWIFIEX_TIMER_10S			10000
#define MWIFIEX_TIMER_1S			1000

#define MAX_TX_PENDING      400
#define LOW_TX_PENDING      380

#define HIGH_RX_PENDING     50
#define LOW_RX_PENDING      20

#define MWIFIEX_UPLD_SIZE               (2312)

#define MAX_EVENT_SIZE                  2048

#define MWIFIEX_FW_DUMP_SIZE       (2 * 1024 * 1024)

#define ARP_FILTER_MAX_BUF_SIZE         68

#define MWIFIEX_KEY_BUFFER_SIZE			16
#define MWIFIEX_DEFAULT_LISTEN_INTERVAL 10
#define MWIFIEX_MAX_REGION_CODE         9

#define DEFAULT_BCN_AVG_FACTOR          8
#define DEFAULT_DATA_AVG_FACTOR         8

#define FIRST_VALID_CHANNEL				0xff
#define DEFAULT_AD_HOC_CHANNEL			6
#define DEFAULT_AD_HOC_CHANNEL_A		36

#define DEFAULT_BCN_MISS_TIMEOUT		5

#define MAX_SCAN_BEACON_BUFFER			8000

#define SCAN_BEACON_ENTRY_PAD			6

#define MWIFIEX_PASSIVE_SCAN_CHAN_TIME	110
#define MWIFIEX_ACTIVE_SCAN_CHAN_TIME	40
#define MWIFIEX_SPECIFIC_SCAN_CHAN_TIME	40
#define MWIFIEX_DEF_SCAN_CHAN_GAP_TIME  50

#define SCAN_RSSI(RSSI)					(0x100 - ((u8)(RSSI)))

#define MWIFIEX_MAX_TOTAL_SCAN_TIME	(MWIFIEX_TIMER_10S - MWIFIEX_TIMER_1S)

#define WPA_GTK_OUI_OFFSET				2
#define RSN_GTK_OUI_OFFSET				2

#define MWIFIEX_OUI_NOT_PRESENT			0
#define MWIFIEX_OUI_PRESENT				1

#define PKT_TYPE_MGMT	0xE5

/*
 * Do not check for data_received for USB, as data_received
 * is handled in mwifiex_usb_recv for USB
 */
#define IS_CARD_RX_RCVD(adapter) (adapter->cmd_resp_received || \
				adapter->event_received || \
				adapter->data_received)

#define MWIFIEX_TYPE_CMD				1
#define MWIFIEX_TYPE_DATA				0
#define MWIFIEX_TYPE_AGGR_DATA				10
#define MWIFIEX_TYPE_EVENT				3

#define MAX_BITMAP_RATES_SIZE			18

#define MAX_CHANNEL_BAND_BG     14
#define MAX_CHANNEL_BAND_A      165

#define MAX_FREQUENCY_BAND_BG   2484

#define MWIFIEX_EVENT_HEADER_LEN           4
#define MWIFIEX_UAP_EVENT_EXTRA_HEADER	   2

#define MWIFIEX_TYPE_LEN			4
#define MWIFIEX_USB_TYPE_CMD			0xF00DFACE
#define MWIFIEX_USB_TYPE_DATA			0xBEADC0DE
#define MWIFIEX_USB_TYPE_EVENT			0xBEEFFACE

/* Threshold for tx_timeout_cnt before we trigger a card reset */
#define TX_TIMEOUT_THRESHOLD	6

#define MWIFIEX_DRV_INFO_SIZE_MAX 0x40000

/* Address alignment */
#define MWIFIEX_ALIGN_ADDR(p, a) (((long)(p) + (a) - 1) & ~((a) - 1))

#define MWIFIEX_MAC_LOCAL_ADMIN_BIT		41

/**
 *enum mwifiex_debug_level  -  marvell wifi debug level
 */
enum MWIFIEX_DEBUG_LEVEL {
	MWIFIEX_DBG_MSG		= 0x00000001,
	MWIFIEX_DBG_FATAL	= 0x00000002,
	MWIFIEX_DBG_ERROR	= 0x00000004,
	MWIFIEX_DBG_DATA	= 0x00000008,
	MWIFIEX_DBG_CMD		= 0x00000010,
	MWIFIEX_DBG_EVENT	= 0x00000020,
	MWIFIEX_DBG_INTR	= 0x00000040,
	MWIFIEX_DBG_IOCTL	= 0x00000080,

	MWIFIEX_DBG_MPA_D	= 0x00008000,
	MWIFIEX_DBG_DAT_D	= 0x00010000,
	MWIFIEX_DBG_CMD_D	= 0x00020000,
	MWIFIEX_DBG_EVT_D	= 0x00040000,
	MWIFIEX_DBG_FW_D	= 0x00080000,
	MWIFIEX_DBG_IF_D	= 0x00100000,

	MWIFIEX_DBG_ENTRY	= 0x10000000,
	MWIFIEX_DBG_WARN	= 0x20000000,
	MWIFIEX_DBG_INFO	= 0x40000000,
	MWIFIEX_DBG_DUMP	= 0x80000000,

	MWIFIEX_DBG_ANY		= 0xffffffff
};

#define MWIFIEX_DEFAULT_DEBUG_MASK	(MWIFIEX_DBG_MSG | \
					MWIFIEX_DBG_FATAL | \
					MWIFIEX_DBG_ERROR)

__printf(3, 4)
void _mwifiex_dbg(const struct mwifiex_adapter *adapter, int mask,
		  const char *fmt, ...);
#define mwifiex_dbg(adapter, mask, fmt, ...)				\
	_mwifiex_dbg(adapter, MWIFIEX_DBG_##mask, fmt, ##__VA_ARGS__)

#define DEBUG_DUMP_DATA_MAX_LEN		128
#define mwifiex_dbg_dump(adapter, dbg_mask, str, buf, len)	\
do {								\
	if ((adapter)->debug_mask & MWIFIEX_DBG_##dbg_mask)	\
		print_hex_dump(KERN_DEBUG, str,			\
			       DUMP_PREFIX_OFFSET, 16, 1,	\
			       buf, len, false);		\
} while (0)

/** Min BGSCAN interval 15 second */
#define MWIFIEX_BGSCAN_INTERVAL 15000
/** default repeat count */
#define MWIFIEX_BGSCAN_REPEAT_COUNT 6

struct mwifiex_dbg {
	u32 num_cmd_host_to_card_failure;
	u32 num_cmd_sleep_cfm_host_to_card_failure;
	u32 num_tx_host_to_card_failure;
	u32 num_event_deauth;
	u32 num_event_disassoc;
	u32 num_event_link_lost;
	u32 num_cmd_deauth;
	u32 num_cmd_assoc_success;
	u32 num_cmd_assoc_failure;
	u32 num_tx_timeout;
	u16 timeout_cmd_id;
	u16 timeout_cmd_act;
	u16 last_cmd_id[DBG_CMD_NUM];
	u16 last_cmd_act[DBG_CMD_NUM];
	u16 last_cmd_index;
	u16 last_cmd_resp_id[DBG_CMD_NUM];
	u16 last_cmd_resp_index;
	u16 last_event[DBG_CMD_NUM];
	u16 last_event_index;
	u32 last_mp_wr_bitmap[MWIFIEX_DBG_SDIO_MP_NUM];
	u32 last_mp_wr_ports[MWIFIEX_DBG_SDIO_MP_NUM];
	u32 last_mp_wr_len[MWIFIEX_DBG_SDIO_MP_NUM];
	u32 last_mp_curr_wr_port[MWIFIEX_DBG_SDIO_MP_NUM];
	u8 last_sdio_mp_index;
};

enum MWIFIEX_HARDWARE_STATUS {
	MWIFIEX_HW_STATUS_READY,
	MWIFIEX_HW_STATUS_INITIALIZING,
	MWIFIEX_HW_STATUS_INIT_DONE,
	MWIFIEX_HW_STATUS_RESET,
	MWIFIEX_HW_STATUS_NOT_READY
};

enum MWIFIEX_802_11_POWER_MODE {
	MWIFIEX_802_11_POWER_MODE_CAM,
	MWIFIEX_802_11_POWER_MODE_PSP
};

struct mwifiex_tx_param {
	u32 next_pkt_len;
};

enum MWIFIEX_PS_STATE {
	PS_STATE_AWAKE,
	PS_STATE_PRE_SLEEP,
	PS_STATE_SLEEP_CFM,
	PS_STATE_SLEEP
};

enum mwifiex_iface_type {
	MWIFIEX_SDIO,
	MWIFIEX_PCIE,
	MWIFIEX_USB
};

struct mwifiex_add_ba_param {
	u32 tx_win_size;
	u32 rx_win_size;
	u32 timeout;
	u8 tx_amsdu;
	u8 rx_amsdu;
};

struct mwifiex_tx_aggr {
	u8 ampdu_user;
	u8 ampdu_ap;
	u8 amsdu;
};

enum mwifiex_ba_status {
	BA_SETUP_NONE = 0,
	BA_SETUP_INPROGRESS,
	BA_SETUP_COMPLETE
};

struct mwifiex_ra_list_tbl {
	struct list_head list;
	struct sk_buff_head skb_head;
	u8 ra[ETH_ALEN];
	u32 is_11n_enabled;
	u16 max_amsdu;
	u16 ba_pkt_count;
	u8 ba_packet_thr;
	enum mwifiex_ba_status ba_status;
	u8 amsdu_in_ampdu;
	u16 total_pkt_count;
	bool tdls_link;
	bool tx_paused;
};

struct mwifiex_tid_tbl {
	struct list_head ra_list;
};

#define WMM_HIGHEST_PRIORITY		7
#define HIGH_PRIO_TID				7
#define LOW_PRIO_TID				0
#define NO_PKT_PRIO_TID				-1
#define MWIFIEX_WMM_DRV_DELAY_MAX 510

struct mwifiex_wmm_desc {
	struct mwifiex_tid_tbl tid_tbl_ptr[MAX_NUM_TID];
	u32 packets_out[MAX_NUM_TID];
	u32 pkts_paused[MAX_NUM_TID];
	/* spin lock to protect ra_list */
	spinlock_t ra_list_spinlock;
	struct mwifiex_wmm_ac_status ac_status[IEEE80211_NUM_ACS];
	enum mwifiex_wmm_ac_e ac_down_graded_vals[IEEE80211_NUM_ACS];
	u32 drv_pkt_delay_max;
	u8 queue_priority[IEEE80211_NUM_ACS];
	u32 user_pri_pkt_tx_ctrl[WMM_HIGHEST_PRIORITY + 1];	/* UP: 0 to 7 */
	/* Number of transmit packets queued */
	atomic_t tx_pkts_queued;
	/* Tracks highest priority with a packet queued */
	atomic_t highest_queued_prio;
};

struct mwifiex_802_11_security {
	u8 wpa_enabled;
	u8 wpa2_enabled;
	u8 wapi_enabled;
	u8 wapi_key_on;
	u8 wep_enabled;
	u32 authentication_mode;
	u8 is_authtype_auto;
	u32 encryption_mode;
};

struct ieee_types_header {
	u8 element_id;
	u8 len;
} __packed;

struct ieee_types_vendor_specific {
	struct ieee_types_vendor_header vend_hdr;
	u8 data[IEEE_MAX_IE_SIZE - sizeof(struct ieee_types_vendor_header)];
} __packed;

struct ieee_types_generic {
	struct ieee_types_header ieee_hdr;
	u8 data[IEEE_MAX_IE_SIZE - sizeof(struct ieee_types_header)];
} __packed;

struct ieee_types_bss_co_2040 {
	struct ieee_types_header ieee_hdr;
	u8 bss_2040co;
} __packed;

struct ieee_types_extcap {
	struct ieee_types_header ieee_hdr;
	u8 ext_capab[8];
} __packed;

struct ieee_types_vht_cap {
	struct ieee_types_header ieee_hdr;
	struct ieee80211_vht_cap vhtcap;
} __packed;

struct ieee_types_vht_oper {
	struct ieee_types_header ieee_hdr;
	struct ieee80211_vht_operation vhtoper;
} __packed;

struct ieee_types_aid {
	struct ieee_types_header ieee_hdr;
	u16 aid;
} __packed;

struct mwifiex_bssdescriptor {
	u8 mac_address[ETH_ALEN];
	struct cfg80211_ssid ssid;
	u32 privacy;
	s32 rssi;
	u32 channel;
	u32 freq;
	u16 beacon_period;
	u8 erp_flags;
	u32 bss_mode;
	u8 supported_rates[MWIFIEX_SUPPORTED_RATES];
	u8 data_rates[MWIFIEX_SUPPORTED_RATES];
	/* Network band.
	 * BAND_B(0x01): 'b' band
	 * BAND_G(0x02): 'g' band
	 * BAND_A(0X04): 'a' band
	 */
	u16 bss_band;
	u64 fw_tsf;
	u64 timestamp;
	union ieee_types_phy_param_set phy_param_set;
	union ieee_types_ss_param_set ss_param_set;
	u16 cap_info_bitmap;
	struct ieee_types_wmm_parameter wmm_ie;
	u8  disable_11n;
	struct ieee80211_ht_cap *bcn_ht_cap;
	u16 ht_cap_offset;
	struct ieee80211_ht_operation *bcn_ht_oper;
	u16 ht_info_offset;
	u8 *bcn_bss_co_2040;
	u16 bss_co_2040_offset;
	u8 *bcn_ext_cap;
	u16 ext_cap_offset;
	struct ieee80211_vht_cap *bcn_vht_cap;
	u16 vht_cap_offset;
	struct ieee80211_vht_operation *bcn_vht_oper;
	u16 vht_info_offset;
	struct ieee_types_oper_mode_ntf *oper_mode;
	u16 oper_mode_offset;
	u8 disable_11ac;
	struct ieee_types_vendor_specific *bcn_wpa_ie;
	u16 wpa_offset;
	struct ieee_types_generic *bcn_rsn_ie;
	u16 rsn_offset;
	struct ieee_types_generic *bcn_wapi_ie;
	u16 wapi_offset;
	u8 *beacon_buf;
	u32 beacon_buf_size;
	u8 sensed_11h;
	u8 local_constraint;
	u8 chan_sw_ie_present;
};

struct mwifiex_current_bss_params {
	struct mwifiex_bssdescriptor bss_descriptor;
	u8 wmm_enabled;
	u8 wmm_uapsd_enabled;
	u8 band;
	u32 num_of_rates;
	u8 data_rates[MWIFIEX_SUPPORTED_RATES];
};

struct mwifiex_sleep_params {
	u16 sp_error;
	u16 sp_offset;
	u16 sp_stable_time;
	u8 sp_cal_control;
	u8 sp_ext_sleep_clk;
	u16 sp_reserved;
};

struct mwifiex_sleep_period {
	u16 period;
	u16 reserved;
};

struct mwifiex_wep_key {
	u32 length;
	u32 key_index;
	u32 key_length;
	u8 key_material[MWIFIEX_KEY_BUFFER_SIZE];
};

#define MAX_REGION_CHANNEL_NUM  2

struct mwifiex_chan_freq_power {
	u16 channel;
	u32 freq;
	u16 max_tx_power;
	u8 unsupported;
};

enum state_11d_t {
	DISABLE_11D = 0,
	ENABLE_11D = 1,
};

#define MWIFIEX_MAX_TRIPLET_802_11D		83

struct mwifiex_802_11d_domain_reg {
	u8 country_code[IEEE80211_COUNTRY_STRING_LEN];
	u8 no_of_triplet;
	struct ieee80211_country_ie_triplet
		triplet[MWIFIEX_MAX_TRIPLET_802_11D];
};

struct mwifiex_vendor_spec_cfg_ie {
	u16 mask;
	u16 flag;
	u8 ie[MWIFIEX_MAX_VSIE_LEN];
};

struct wps {
	u8 session_enable;
};

struct mwifiex_roc_cfg {
	u64 cookie;
	struct ieee80211_channel chan;
};

enum mwifiex_iface_work_flags {
	MWIFIEX_IFACE_WORK_DEVICE_DUMP,
	MWIFIEX_IFACE_WORK_CARD_RESET,
};

enum mwifiex_adapter_work_flags {
	MWIFIEX_SURPRISE_REMOVED,
	MWIFIEX_IS_CMD_TIMEDOUT,
	MWIFIEX_IS_SUSPENDED,
	MWIFIEX_IS_HS_CONFIGURED,
	MWIFIEX_IS_HS_ENABLING,
};

struct mwifiex_band_config {
	u8 chan_band:2;
	u8 chan_width:2;
	u8 chan2_offset:2;
	u8 scan_mode:2;
} __packed;

struct mwifiex_channel_band {
	struct mwifiex_band_config band_config;
	u8 channel;
};

struct mwifiex_private {
	struct mwifiex_adapter *adapter;
	u8 bss_type;
	u8 bss_role;
	u8 bss_priority;
	u8 bss_num;
	u8 bss_started;
	u8 frame_type;
	u8 curr_addr[ETH_ALEN];
	u8 media_connected;
	u8 port_open;
	u8 usb_port;
	u32 num_tx_timeout;
	/* track consecutive timeout */
	u8 tx_timeout_cnt;
	struct net_device *netdev;
	struct net_device_stats stats;
	u32 curr_pkt_filter;
	u32 bss_mode;
	u32 pkt_tx_ctrl;
	u16 tx_power_level;
	u8 max_tx_power_level;
	u8 min_tx_power_level;
	u32 tx_ant;
	u32 rx_ant;
	u8 tx_rate;
	u8 tx_htinfo;
	u8 rxpd_htinfo;
	u8 rxpd_rate;
	u16 rate_bitmap;
	u16 bitmap_rates[MAX_BITMAP_RATES_SIZE];
	u32 data_rate;
	u8 is_data_rate_auto;
	u16 bcn_avg_factor;
	u16 data_avg_factor;
	s16 data_rssi_last;
	s16 data_nf_last;
	s16 data_rssi_avg;
	s16 data_nf_avg;
	s16 bcn_rssi_last;
	s16 bcn_nf_last;
	s16 bcn_rssi_avg;
	s16 bcn_nf_avg;
	struct mwifiex_bssdescriptor *attempted_bss_desc;
	struct cfg80211_ssid prev_ssid;
	u8 prev_bssid[ETH_ALEN];
	struct mwifiex_current_bss_params curr_bss_params;
	u16 beacon_period;
	u8 dtim_period;
	u16 listen_interval;
	u16 atim_window;
	u8 adhoc_channel;
	u8 adhoc_is_link_sensed;
	u8 adhoc_state;
	struct mwifiex_802_11_security sec_info;
	struct mwifiex_wep_key wep_key[NUM_WEP_KEYS];
	u16 wep_key_curr_index;
	u8 wpa_ie[256];
	u16 wpa_ie_len;
	u8 wpa_is_gtk_set;
	struct host_cmd_ds_802_11_key_material aes_key;
	struct host_cmd_ds_802_11_key_material_v2 aes_key_v2;
	u8 wapi_ie[256];
	u16 wapi_ie_len;
	u8 *wps_ie;
	u16 wps_ie_len;
	u8 wmm_required;
	u8 wmm_enabled;
	u8 wmm_qosinfo;
	struct mwifiex_wmm_desc wmm;
	atomic_t wmm_tx_pending[IEEE80211_NUM_ACS];
	struct list_head sta_list;
	/* spin lock for associated station/TDLS peers list */
	spinlock_t sta_list_spinlock;
	struct list_head auto_tdls_list;
	/* spin lock for auto TDLS peer list */
	spinlock_t auto_tdls_lock;
	struct list_head tx_ba_stream_tbl_ptr;
	/* spin lock for tx_ba_stream_tbl_ptr queue */
	spinlock_t tx_ba_stream_tbl_lock;
	struct mwifiex_tx_aggr aggr_prio_tbl[MAX_NUM_TID];
	struct mwifiex_add_ba_param add_ba_param;
	u16 rx_seq[MAX_NUM_TID];
	u8 tos_to_tid_inv[MAX_NUM_TID];
	struct list_head rx_reorder_tbl_ptr;
	/* spin lock for rx_reorder_tbl_ptr queue */
	spinlock_t rx_reorder_tbl_lock;
#define MWIFIEX_ASSOC_RSP_BUF_SIZE  500
	u8 assoc_rsp_buf[MWIFIEX_ASSOC_RSP_BUF_SIZE];
	u32 assoc_rsp_size;

#define MWIFIEX_GENIE_BUF_SIZE      256
	u8 gen_ie_buf[MWIFIEX_GENIE_BUF_SIZE];
	u8 gen_ie_buf_len;

	struct mwifiex_vendor_spec_cfg_ie vs_ie[MWIFIEX_MAX_VSIE_NUM];

#define MWIFIEX_ASSOC_TLV_BUF_SIZE  256
	u8 assoc_tlv_buf[MWIFIEX_ASSOC_TLV_BUF_SIZE];
	u8 assoc_tlv_buf_len;

	u8 *curr_bcn_buf;
	u32 curr_bcn_size;
	/* spin lock for beacon buffer */
	spinlock_t curr_bcn_buf_lock;
	struct wireless_dev wdev;
	struct mwifiex_chan_freq_power cfp;
	u32 versionstrsel;
	char version_str[128];
#ifdef CONFIG_DEBUG_FS
	struct dentry *dfs_dev_dir;
#endif
	u16 current_key_index;
	struct mutex async_mutex;
	struct cfg80211_scan_request *scan_request;
	u8 cfg_bssid[6];
	struct wps wps;
	u8 scan_block;
	s32 cqm_rssi_thold;
	u32 cqm_rssi_hyst;
	u8 subsc_evt_rssi_state;
	struct mwifiex_ds_misc_subsc_evt async_subsc_evt_storage;
	struct mwifiex_ie mgmt_ie[MAX_MGMT_IE_INDEX];
	u16 beacon_idx;
	u16 proberesp_idx;
	u16 assocresp_idx;
	u16 gen_idx;
	u8 ap_11n_enabled;
	u8 ap_11ac_enabled;
	u32 mgmt_frame_mask;
	struct mwifiex_roc_cfg roc_cfg;
	bool scan_aborting;
	u8 sched_scanning;
	u8 csa_chan;
	unsigned long csa_expire_time;
	u8 del_list_idx;
	bool hs2_enabled;
	struct mwifiex_uap_bss_param bss_cfg;
	struct cfg80211_chan_def bss_chandef;
	struct station_parameters *sta_params;
	struct sk_buff_head tdls_txq;
	u8 check_tdls_tx;
	struct timer_list auto_tdls_timer;
	bool auto_tdls_timer_active;
	struct idr ack_status_frames;
	/* spin lock for ack status */
	spinlock_t ack_status_lock;
	/** rx histogram data */
	struct mwifiex_histogram_data *hist_data;
	struct cfg80211_chan_def dfs_chandef;
	struct workqueue_struct *dfs_cac_workqueue;
	struct delayed_work dfs_cac_work;
	struct timer_list dfs_chan_switch_timer;
	struct workqueue_struct *dfs_chan_sw_workqueue;
	struct delayed_work dfs_chan_sw_work;
	struct cfg80211_beacon_data beacon_after;
	struct mwifiex_11h_intf_state state_11h;
	struct mwifiex_ds_mem_rw mem_rw;
	struct sk_buff_head bypass_txq;
	struct mwifiex_user_scan_chan hidden_chan[MWIFIEX_USER_SCAN_CHAN_MAX];
	u8 assoc_resp_ht_param;
	bool ht_param_present;
};


struct mwifiex_tx_ba_stream_tbl {
	struct list_head list;
	int tid;
	u8 ra[ETH_ALEN];
	enum mwifiex_ba_status ba_status;
	u8 amsdu;
};

struct mwifiex_rx_reorder_tbl;

struct reorder_tmr_cnxt {
	struct timer_list timer;
	struct mwifiex_rx_reorder_tbl *ptr;
	struct mwifiex_private *priv;
	u8 timer_is_set;
};

struct mwifiex_rx_reorder_tbl {
	struct list_head list;
	int tid;
	u8 ta[ETH_ALEN];
	int init_win;
	int start_win;
	int win_size;
	void **rx_reorder_ptr;
	struct reorder_tmr_cnxt timer_context;
	u8 amsdu;
	u8 flags;
};

struct mwifiex_bss_prio_node {
	struct list_head list;
	struct mwifiex_private *priv;
};

struct mwifiex_bss_prio_tbl {
	struct list_head bss_prio_head;
	/* spin lock for bss priority  */
	spinlock_t bss_prio_lock;
	struct mwifiex_bss_prio_node *bss_prio_cur;
};

struct cmd_ctrl_node {
	struct list_head list;
	struct mwifiex_private *priv;
	u32 cmd_no;
	u32 cmd_flag;
	struct sk_buff *cmd_skb;
	struct sk_buff *resp_skb;
	void *data_buf;
	u32 wait_q_enabled;
	struct sk_buff *skb;
	u8 *condition;
	u8 cmd_wait_q_woken;
};

struct mwifiex_bss_priv {
	u8 band;
	u64 fw_tsf;
};

struct mwifiex_tdls_capab {
	__le16 capab;
	u8 rates[32];
	u8 rates_len;
	u8 qos_info;
	u8 coex_2040;
	u16 aid;
	struct ieee80211_ht_cap ht_capb;
	struct ieee80211_ht_operation ht_oper;
	struct ieee_types_extcap extcap;
	struct ieee_types_generic rsn_ie;
	struct ieee80211_vht_cap vhtcap;
	struct ieee80211_vht_operation vhtoper;
};

struct mwifiex_station_stats {
	u64 last_rx;
	s8 rssi;
	u64 rx_bytes;
	u64 tx_bytes;
	u32 rx_packets;
	u32 tx_packets;
	u32 tx_failed;
	u8 last_tx_rate;
	u8 last_tx_htinfo;
};

/* This is AP/TDLS specific structure which stores information
 * about associated/peer STA
 */
struct mwifiex_sta_node {
	struct list_head list;
	u8 mac_addr[ETH_ALEN];
	u8 is_wmm_enabled;
	u8 is_11n_enabled;
	u8 is_11ac_enabled;
	u8 ampdu_sta[MAX_NUM_TID];
	u16 rx_seq[MAX_NUM_TID];
	u16 max_amsdu;
	u8 tdls_status;
	struct mwifiex_tdls_capab tdls_cap;
	struct mwifiex_station_stats stats;
	u8 tx_pause;
};

struct mwifiex_auto_tdls_peer {
	struct list_head list;
	u8 mac_addr[ETH_ALEN];
	u8 tdls_status;
	int rssi;
	long rssi_jiffies;
	u8 failure_count;
	u8 do_discover;
	u8 do_setup;
};

#define MWIFIEX_TYPE_AGGR_DATA_V2 11
#define MWIFIEX_BUS_AGGR_MODE_LEN_V2 (2)
#define MWIFIEX_BUS_AGGR_MAX_LEN 16000
#define MWIFIEX_BUS_AGGR_MAX_NUM 10
struct bus_aggr_params {
	u16 enable;
	u16 mode;
	u16 tx_aggr_max_size;
	u16 tx_aggr_max_num;
	u16 tx_aggr_align;
};

struct mwifiex_if_ops {
	int (*init_if) (struct mwifiex_adapter *);
	void (*cleanup_if) (struct mwifiex_adapter *);
	int (*check_fw_status) (struct mwifiex_adapter *, u32);
	int (*check_winner_status)(struct mwifiex_adapter *);
	int (*prog_fw) (struct mwifiex_adapter *, struct mwifiex_fw_image *);
	int (*register_dev) (struct mwifiex_adapter *);
	void (*unregister_dev) (struct mwifiex_adapter *);
	int (*enable_int) (struct mwifiex_adapter *);
	void (*disable_int) (struct mwifiex_adapter *);
	int (*process_int_status) (struct mwifiex_adapter *);
	int (*host_to_card) (struct mwifiex_adapter *, u8, struct sk_buff *,
			     struct mwifiex_tx_param *);
	int (*wakeup) (struct mwifiex_adapter *);
	int (*wakeup_complete) (struct mwifiex_adapter *);

	/* Interface specific functions */
	void (*update_mp_end_port) (struct mwifiex_adapter *, u16);
	void (*cleanup_mpa_buf) (struct mwifiex_adapter *);
	int (*cmdrsp_complete) (struct mwifiex_adapter *, struct sk_buff *);
	int (*event_complete) (struct mwifiex_adapter *, struct sk_buff *);
	int (*init_fw_port) (struct mwifiex_adapter *);
	int (*dnld_fw) (struct mwifiex_adapter *, struct mwifiex_fw_image *);
	void (*card_reset) (struct mwifiex_adapter *);
	int (*reg_dump)(struct mwifiex_adapter *, char *);
	void (*device_dump)(struct mwifiex_adapter *);
	int (*clean_pcie_ring) (struct mwifiex_adapter *adapter);
	void (*iface_work)(struct work_struct *work);
	void (*submit_rem_rx_urbs)(struct mwifiex_adapter *adapter);
	void (*deaggr_pkt)(struct mwifiex_adapter *, struct sk_buff *);
	void (*multi_port_resync)(struct mwifiex_adapter *);
	bool (*is_port_ready)(struct mwifiex_private *);
	void (*down_dev)(struct mwifiex_adapter *);
	void (*up_dev)(struct mwifiex_adapter *);
};

struct mwifiex_adapter {
	u8 iface_type;
	unsigned int debug_mask;
	struct mwifiex_iface_comb iface_limit;
	struct mwifiex_iface_comb curr_iface_comb;
	struct mwifiex_private *priv[MWIFIEX_MAX_BSS_NUM];
	u8 priv_num;
	const struct firmware *firmware;
	char fw_name[32];
	int winner;
	struct device *dev;
	struct wiphy *wiphy;
	u8 perm_addr[ETH_ALEN];
	unsigned long work_flags;
	u32 fw_release_number;
	u8 intf_hdr_len;
	u16 init_wait_q_woken;
	wait_queue_head_t init_wait_q;
	void *card;
	struct mwifiex_if_ops if_ops;
	atomic_t bypass_tx_pending;
	atomic_t rx_pending;
	atomic_t tx_pending;
	atomic_t cmd_pending;
	atomic_t tx_hw_pending;
	struct workqueue_struct *workqueue;
	struct work_struct main_work;
	struct workqueue_struct *rx_workqueue;
	struct work_struct rx_work;
	struct workqueue_struct *dfs_workqueue;
	struct work_struct dfs_work;
	bool rx_work_enabled;
	bool rx_processing;
	bool delay_main_work;
	bool rx_locked;
	bool main_locked;
	struct mwifiex_bss_prio_tbl bss_prio_tbl[MWIFIEX_MAX_BSS_NUM];
	/* spin lock for main process */
	spinlock_t main_proc_lock;
	u32 mwifiex_processing;
	u8 more_task_flag;
	u16 tx_buf_size;
	u16 curr_tx_buf_size;
	/* sdio single port rx aggregation capability */
	bool host_disable_sdio_rx_aggr;
	bool sdio_rx_aggr_enable;
	u16 sdio_rx_block_size;
	u32 ioport;
	enum MWIFIEX_HARDWARE_STATUS hw_status;
	u16 number_of_antenna;
	u32 fw_cap_info;
	/* spin lock for interrupt handling */
	spinlock_t int_lock;
	u8 int_status;
	u32 event_cause;
	struct sk_buff *event_skb;
	u8 upld_buf[MWIFIEX_UPLD_SIZE];
	u8 data_sent;
	u8 cmd_sent;
	u8 cmd_resp_received;
	u8 event_received;
	u8 data_received;
	u16 seq_num;
	struct cmd_ctrl_node *cmd_pool;
	struct cmd_ctrl_node *curr_cmd;
	/* spin lock for command */
	spinlock_t mwifiex_cmd_lock;
	u16 last_init_cmd;
	struct timer_list cmd_timer;
	struct list_head cmd_free_q;
	/* spin lock for cmd_free_q */
	spinlock_t cmd_free_q_lock;
	struct list_head cmd_pending_q;
	/* spin lock for cmd_pending_q */
	spinlock_t cmd_pending_q_lock;
	struct list_head scan_pending_q;
	/* spin lock for scan_pending_q */
	spinlock_t scan_pending_q_lock;
	/* spin lock for RX processing routine */
	spinlock_t rx_proc_lock;
	struct sk_buff_head tx_data_q;
	atomic_t tx_queued;
	u32 scan_processing;
	u16 region_code;
	struct mwifiex_802_11d_domain_reg domain_reg;
	u16 scan_probes;
	u32 scan_mode;
	u16 specific_scan_time;
	u16 active_scan_time;
	u16 passive_scan_time;
	u16 scan_chan_gap_time;
	u8 fw_bands;
	u8 adhoc_start_band;
	u8 config_bands;
	struct mwifiex_chan_scan_param_set *scan_channels;
	u8 tx_lock_flag;
	struct mwifiex_sleep_params sleep_params;
	struct mwifiex_sleep_period sleep_period;
	u16 ps_mode;
	u32 ps_state;
	u8 need_to_wakeup;
	u16 multiple_dtim;
	u16 local_listen_interval;
	u16 null_pkt_interval;
	struct sk_buff *sleep_cfm;
	u16 bcn_miss_time_out;
	u16 adhoc_awake_period;
	u8 is_deep_sleep;
	u8 delay_null_pkt;
	u16 delay_to_ps;
	u16 enhanced_ps_mode;
	u8 pm_wakeup_card_req;
	u16 gen_null_pkt;
	u16 pps_uapsd_mode;
	u32 pm_wakeup_fw_try;
	struct timer_list wakeup_timer;
	struct mwifiex_hs_config_param hs_cfg;
	u8 hs_activated;
	u16 hs_activate_wait_q_woken;
	wait_queue_head_t hs_activate_wait_q;
	u8 event_body[MAX_EVENT_SIZE];
	u32 hw_dot_11n_dev_cap;
	u8 hw_dev_mcs_support;
	u8 user_dev_mcs_support;
	u8 adhoc_11n_enabled;
	u8 sec_chan_offset;
	struct mwifiex_dbg dbg;
	u8 arp_filter[ARP_FILTER_MAX_BUF_SIZE];
	u32 arp_filter_size;
	struct mwifiex_wait_queue cmd_wait_q;
	u8 scan_wait_q_woken;
	spinlock_t queue_lock;		/* lock for tx queues */
	u8 country_code[IEEE80211_COUNTRY_STRING_LEN];
	u16 max_mgmt_ie_index;
	const struct firmware *cal_data;
	struct device_node *dt_node;

	/* 11AC */
	u32 is_hw_11ac_capable;
	u32 hw_dot_11ac_dev_cap;
	u32 hw_dot_11ac_mcs_support;
	u32 usr_dot_11ac_dev_cap_bg;
	u32 usr_dot_11ac_dev_cap_a;
	u32 usr_dot_11ac_mcs_support;

	atomic_t pending_bridged_pkts;

	/* For synchronizing FW initialization with device lifecycle. */
	struct completion *fw_done;
	bool is_up;

	bool ext_scan;
	u8 fw_api_ver;
	u8 key_api_major_ver, key_api_minor_ver;
	u8 max_p2p_conn, max_sta_conn;
	struct memory_type_mapping *mem_type_mapping_tbl;
	u8 num_mem_types;
	bool scan_chan_gap_enabled;
	struct sk_buff_head rx_data_q;
	bool mfg_mode;
	struct mwifiex_chan_stats *chan_stats;
	u32 num_in_chan_stats;
	int survey_idx;
	bool auto_tdls;
	u8 coex_scan;
	u8 coex_min_scan_time;
	u8 coex_max_scan_time;
	u8 coex_win_size;
	u8 coex_tx_win_size;
	u8 coex_rx_win_size;
	bool drcs_enabled;
	u8 active_scan_triggered;
	bool usb_mc_status;
	bool usb_mc_setup;
	struct cfg80211_wowlan_nd_info *nd_info;
	struct ieee80211_regdomain *regd;

	/* Wake-on-WLAN (WoWLAN) */
	int irq_wakeup;
	bool wake_by_wifi;
	/* Aggregation parameters*/
	struct bus_aggr_params bus_aggr;
	/* Device dump data/length */
	void *devdump_data;
	int devdump_len;
	struct timer_list devdump_timer;
};

void mwifiex_process_tx_queue(struct mwifiex_adapter *adapter);

int mwifiex_init_lock_list(struct mwifiex_adapter *adapter);

void mwifiex_set_trans_start(struct net_device *dev);

void mwifiex_stop_net_dev_queue(struct net_device *netdev,
		struct mwifiex_adapter *adapter);

void mwifiex_wake_up_net_dev_queue(struct net_device *netdev,
		struct mwifiex_adapter *adapter);

int mwifiex_init_priv(struct mwifiex_private *priv);
void mwifiex_free_priv(struct mwifiex_private *priv);

int mwifiex_init_fw(struct mwifiex_adapter *adapter);

int mwifiex_init_fw_complete(struct mwifiex_adapter *adapter);

void mwifiex_shutdown_drv(struct mwifiex_adapter *adapter);

int mwifiex_dnld_fw(struct mwifiex_adapter *, struct mwifiex_fw_image *);

int mwifiex_recv_packet(struct mwifiex_private *priv, struct sk_buff *skb);
int mwifiex_uap_recv_packet(struct mwifiex_private *priv,
			    struct sk_buff *skb);

int mwifiex_process_mgmt_packet(struct mwifiex_private *priv,
				struct sk_buff *skb);

int mwifiex_process_event(struct mwifiex_adapter *adapter);

int mwifiex_complete_cmd(struct mwifiex_adapter *adapter,
			 struct cmd_ctrl_node *cmd_node);

int mwifiex_send_cmd(struct mwifiex_private *priv, u16 cmd_no,
		     u16 cmd_action, u32 cmd_oid, void *data_buf, bool sync);

void mwifiex_cmd_timeout_func(struct timer_list *t);

int mwifiex_get_debug_info(struct mwifiex_private *,
			   struct mwifiex_debug_info *);

int mwifiex_alloc_cmd_buffer(struct mwifiex_adapter *adapter);
void mwifiex_free_cmd_buffer(struct mwifiex_adapter *adapter);
void mwifiex_free_cmd_buffers(struct mwifiex_adapter *adapter);
void mwifiex_cancel_all_pending_cmd(struct mwifiex_adapter *adapter);
void mwifiex_cancel_pending_scan_cmd(struct mwifiex_adapter *adapter);
void mwifiex_cancel_scan(struct mwifiex_adapter *adapter);

void mwifiex_recycle_cmd_node(struct mwifiex_adapter *adapter,
			      struct cmd_ctrl_node *cmd_node);

void mwifiex_insert_cmd_to_pending_q(struct mwifiex_adapter *adapter,
				     struct cmd_ctrl_node *cmd_node);

int mwifiex_exec_next_cmd(struct mwifiex_adapter *adapter);
int mwifiex_process_cmdresp(struct mwifiex_adapter *adapter);
int mwifiex_handle_rx_packet(struct mwifiex_adapter *adapter,
			     struct sk_buff *skb);
int mwifiex_process_tx(struct mwifiex_private *priv, struct sk_buff *skb,
		       struct mwifiex_tx_param *tx_param);
int mwifiex_send_null_packet(struct mwifiex_private *priv, u8 flags);
int mwifiex_write_data_complete(struct mwifiex_adapter *adapter,
				struct sk_buff *skb, int aggr, int status);
void mwifiex_clean_txrx(struct mwifiex_private *priv);
u8 mwifiex_check_last_packet_indication(struct mwifiex_private *priv);
void mwifiex_check_ps_cond(struct mwifiex_adapter *adapter);
void mwifiex_process_sleep_confirm_resp(struct mwifiex_adapter *, u8 *,
					u32);
int mwifiex_cmd_enh_power_mode(struct mwifiex_private *priv,
			       struct host_cmd_ds_command *cmd,
			       u16 cmd_action, uint16_t ps_bitmap,
			       struct mwifiex_ds_auto_ds *auto_ds);
int mwifiex_ret_enh_power_mode(struct mwifiex_private *priv,
			       struct host_cmd_ds_command *resp,
			       struct mwifiex_ds_pm_cfg *pm_cfg);
void mwifiex_process_hs_config(struct mwifiex_adapter *adapter);
void mwifiex_hs_activated_event(struct mwifiex_private *priv,
					u8 activated);
int mwifiex_set_hs_params(struct mwifiex_private *priv, u16 action,
			  int cmd_type, struct mwifiex_ds_hs_cfg *hs_cfg);
int mwifiex_ret_802_11_hs_cfg(struct mwifiex_private *priv,
			      struct host_cmd_ds_command *resp);
int mwifiex_process_rx_packet(struct mwifiex_private *priv,
			      struct sk_buff *skb);
int mwifiex_sta_prepare_cmd(struct mwifiex_private *, uint16_t cmd_no,
			    u16 cmd_action, u32 cmd_oid,
			    void *data_buf, void *cmd_buf);
int mwifiex_uap_prepare_cmd(struct mwifiex_private *priv, uint16_t cmd_no,
			    u16 cmd_action, u32 cmd_oid,
			    void *data_buf, void *cmd_buf);
int mwifiex_process_sta_cmdresp(struct mwifiex_private *, u16 cmdresp_no,
				struct host_cmd_ds_command *resp);
int mwifiex_process_sta_rx_packet(struct mwifiex_private *,
				  struct sk_buff *skb);
int mwifiex_process_uap_rx_packet(struct mwifiex_private *priv,
				  struct sk_buff *skb);
int mwifiex_handle_uap_rx_forward(struct mwifiex_private *priv,
				  struct sk_buff *skb);
int mwifiex_process_sta_event(struct mwifiex_private *);
int mwifiex_process_uap_event(struct mwifiex_private *);
void mwifiex_delete_all_station_list(struct mwifiex_private *priv);
void mwifiex_wmm_del_peer_ra_list(struct mwifiex_private *priv,
				  const u8 *ra_addr);
void *mwifiex_process_sta_txpd(struct mwifiex_private *, struct sk_buff *skb);
void *mwifiex_process_uap_txpd(struct mwifiex_private *, struct sk_buff *skb);
int mwifiex_sta_init_cmd(struct mwifiex_private *, u8 first_sta, bool init);
int mwifiex_cmd_802_11_scan(struct host_cmd_ds_command *cmd,
			    struct mwifiex_scan_cmd_config *scan_cfg);
void mwifiex_queue_scan_cmd(struct mwifiex_private *priv,
			    struct cmd_ctrl_node *cmd_node);
int mwifiex_ret_802_11_scan(struct mwifiex_private *priv,
			    struct host_cmd_ds_command *resp);
s32 mwifiex_ssid_cmp(struct cfg80211_ssid *ssid1, struct cfg80211_ssid *ssid2);
int mwifiex_associate(struct mwifiex_private *priv,
		      struct mwifiex_bssdescriptor *bss_desc);
int mwifiex_cmd_802_11_associate(struct mwifiex_private *priv,
				 struct host_cmd_ds_command *cmd,
				 struct mwifiex_bssdescriptor *bss_desc);
int mwifiex_ret_802_11_associate(struct mwifiex_private *priv,
				 struct host_cmd_ds_command *resp);
void mwifiex_reset_connect_state(struct mwifiex_private *priv, u16 reason,
				 bool from_ap);
u8 mwifiex_band_to_radio_type(u8 band);
int mwifiex_deauthenticate(struct mwifiex_private *priv, u8 *mac);
void mwifiex_deauthenticate_all(struct mwifiex_adapter *adapter);
int mwifiex_adhoc_start(struct mwifiex_private *priv,
			struct cfg80211_ssid *adhoc_ssid);
int mwifiex_adhoc_join(struct mwifiex_private *priv,
		       struct mwifiex_bssdescriptor *bss_desc);
int mwifiex_cmd_802_11_ad_hoc_start(struct mwifiex_private *priv,
				    struct host_cmd_ds_command *cmd,
				    struct cfg80211_ssid *req_ssid);
int mwifiex_cmd_802_11_ad_hoc_join(struct mwifiex_private *priv,
				   struct host_cmd_ds_command *cmd,
				   struct mwifiex_bssdescriptor *bss_desc);
int mwifiex_ret_802_11_ad_hoc(struct mwifiex_private *priv,
			      struct host_cmd_ds_command *resp);
int mwifiex_cmd_802_11_bg_scan_query(struct host_cmd_ds_command *cmd);
struct mwifiex_chan_freq_power *mwifiex_get_cfp(struct mwifiex_private *priv,
						u8 band, u16 channel, u32 freq);
u32 mwifiex_index_to_data_rate(struct mwifiex_private *priv,
			       u8 index, u8 ht_info);
u32 mwifiex_index_to_acs_data_rate(struct mwifiex_private *priv,
				   u8 index, u8 ht_info);
u32 mwifiex_find_freq_from_band_chan(u8, u8);
int mwifiex_cmd_append_vsie_tlv(struct mwifiex_private *priv, u16 vsie_mask,
				u8 **buffer);
u32 mwifiex_get_active_data_rates(struct mwifiex_private *priv,
				    u8 *rates);
u32 mwifiex_get_supported_rates(struct mwifiex_private *priv, u8 *rates);
u32 mwifiex_get_rates_from_cfg80211(struct mwifiex_private *priv,
				    u8 *rates, u8 radio_type);
u8 mwifiex_is_rate_auto(struct mwifiex_private *priv);
extern u16 region_code_index[MWIFIEX_MAX_REGION_CODE];
void mwifiex_save_curr_bcn(struct mwifiex_private *priv);
void mwifiex_free_curr_bcn(struct mwifiex_private *priv);
int mwifiex_cmd_get_hw_spec(struct mwifiex_private *priv,
			    struct host_cmd_ds_command *cmd);
int mwifiex_ret_get_hw_spec(struct mwifiex_private *priv,
			    struct host_cmd_ds_command *resp);
int is_command_pending(struct mwifiex_adapter *adapter);
void mwifiex_init_priv_params(struct mwifiex_private *priv,
						struct net_device *dev);
int mwifiex_set_secure_params(struct mwifiex_private *priv,
			      struct mwifiex_uap_bss_param *bss_config,
			      struct cfg80211_ap_settings *params);
void mwifiex_set_ht_params(struct mwifiex_private *priv,
			   struct mwifiex_uap_bss_param *bss_cfg,
			   struct cfg80211_ap_settings *params);
void mwifiex_set_vht_params(struct mwifiex_private *priv,
			    struct mwifiex_uap_bss_param *bss_cfg,
			    struct cfg80211_ap_settings *params);
void mwifiex_set_tpc_params(struct mwifiex_private *priv,
			    struct mwifiex_uap_bss_param *bss_cfg,
			    struct cfg80211_ap_settings *params);
void mwifiex_set_uap_rates(struct mwifiex_uap_bss_param *bss_cfg,
			   struct cfg80211_ap_settings *params);
void mwifiex_set_vht_width(struct mwifiex_private *priv,
			   enum nl80211_chan_width width,
			   bool ap_11ac_disable);
void
mwifiex_set_wmm_params(struct mwifiex_private *priv,
		       struct mwifiex_uap_bss_param *bss_cfg,
		       struct cfg80211_ap_settings *params);
void mwifiex_set_ba_params(struct mwifiex_private *priv);

void mwifiex_update_ampdu_txwinsize(struct mwifiex_adapter *pmadapter);
void mwifiex_bt_coex_wlan_param_update_event(struct mwifiex_private *priv,
					     struct sk_buff *event_skb);

void mwifiex_set_11ac_ba_params(struct mwifiex_private *priv);
int mwifiex_cmd_802_11_scan_ext(struct mwifiex_private *priv,
				struct host_cmd_ds_command *cmd,
				void *data_buf);
int mwifiex_ret_802_11_scan_ext(struct mwifiex_private *priv,
				struct host_cmd_ds_command *resp);
int mwifiex_handle_event_ext_scan_report(struct mwifiex_private *priv,
					 void *buf);
int mwifiex_cmd_802_11_bg_scan_config(struct mwifiex_private *priv,
				      struct host_cmd_ds_command *cmd,
				      void *data_buf);
int mwifiex_stop_bg_scan(struct mwifiex_private *priv);

/*
 * This function checks if the queuing is RA based or not.
 */
static inline u8
mwifiex_queuing_ra_based(struct mwifiex_private *priv)
{
	/*
	 * Currently we assume if we are in Infra, then DA=RA. This might not be
	 * true in the future
	 */
	if ((priv->bss_mode == NL80211_IFTYPE_STATION ||
	     priv->bss_mode == NL80211_IFTYPE_P2P_CLIENT) &&
	    (GET_BSS_ROLE(priv) == MWIFIEX_BSS_ROLE_STA))
		return false;

	return true;
}

/*
 * This function copies rates.
 */
static inline u32
mwifiex_copy_rates(u8 *dest, u32 pos, u8 *src, int len)
{
	int i;

	for (i = 0; i < len && src[i]; i++, pos++) {
		if (pos >= MWIFIEX_SUPPORTED_RATES)
			break;
		dest[pos] = src[i];
	}

	return pos;
}

/*
 * This function returns the correct private structure pointer based
 * upon the BSS type and BSS number.
 */
static inline struct mwifiex_private *
mwifiex_get_priv_by_id(struct mwifiex_adapter *adapter,
		       u8 bss_num, u8 bss_type)
{
	int i;

	for (i = 0; i < adapter->priv_num; i++) {
		if (adapter->priv[i]) {
			if ((adapter->priv[i]->bss_num == bss_num) &&
			    (adapter->priv[i]->bss_type == bss_type))
				break;
		}
	}
	return ((i < adapter->priv_num) ? adapter->priv[i] : NULL);
}

/*
 * This function returns the first available private structure pointer
 * based upon the BSS role.
 */
static inline struct mwifiex_private *
mwifiex_get_priv(struct mwifiex_adapter *adapter,
		 enum mwifiex_bss_role bss_role)
{
	int i;

	for (i = 0; i < adapter->priv_num; i++) {
		if (adapter->priv[i]) {
			if (bss_role == MWIFIEX_BSS_ROLE_ANY ||
			    GET_BSS_ROLE(adapter->priv[i]) == bss_role)
				break;
		}
	}

	return ((i < adapter->priv_num) ? adapter->priv[i] : NULL);
}

/*
 * This function checks available bss_num when adding new interface or
 * changing interface type.
 */
static inline u8
mwifiex_get_unused_bss_num(struct mwifiex_adapter *adapter, u8 bss_type)
{
	u8 i, j;
	int index[MWIFIEX_MAX_BSS_NUM];

	memset(index, 0, sizeof(index));
	for (i = 0; i < adapter->priv_num; i++)
		if (adapter->priv[i]) {
			if (adapter->priv[i]->bss_type == bss_type &&
			    !(adapter->priv[i]->bss_mode ==
			      NL80211_IFTYPE_UNSPECIFIED)) {
				index[adapter->priv[i]->bss_num] = 1;
			}
		}
	for (j = 0; j < MWIFIEX_MAX_BSS_NUM; j++)
		if (!index[j])
			return j;
	return -1;
}

/*
 * This function returns the first available unused private structure pointer.
 */
static inline struct mwifiex_private *
mwifiex_get_unused_priv_by_bss_type(struct mwifiex_adapter *adapter,
				    u8 bss_type)
{
	u8 i;

	for (i = 0; i < adapter->priv_num; i++)
		if (adapter->priv[i]->bss_mode ==
		   NL80211_IFTYPE_UNSPECIFIED) {
			adapter->priv[i]->bss_num =
			mwifiex_get_unused_bss_num(adapter, bss_type);
			break;
		}

	return ((i < adapter->priv_num) ? adapter->priv[i] : NULL);
}

/*
 * This function returns the driver private structure of a network device.
 */
static inline struct mwifiex_private *
mwifiex_netdev_get_priv(struct net_device *dev)
{
	return (struct mwifiex_private *) (*(unsigned long *) netdev_priv(dev));
}

/*
 * This function checks if a skb holds a management frame.
 */
static inline bool mwifiex_is_skb_mgmt_frame(struct sk_buff *skb)
{
	return (get_unaligned_le32(skb->data) == PKT_TYPE_MGMT);
}

/* This function retrieves channel closed for operation by Channel
 * Switch Announcement.
 */
static inline u8
mwifiex_11h_get_csa_closed_channel(struct mwifiex_private *priv)
{
	if (!priv->csa_chan)
		return 0;

	/* Clear csa channel, if DFS channel move time has passed */
	if (time_after(jiffies, priv->csa_expire_time)) {
		priv->csa_chan = 0;
		priv->csa_expire_time = 0;
	}

	return priv->csa_chan;
}

static inline u8 mwifiex_is_any_intf_active(struct mwifiex_private *priv)
{
	struct mwifiex_private *priv_num;
	int i;

	for (i = 0; i < priv->adapter->priv_num; i++) {
		priv_num = priv->adapter->priv[i];
		if (priv_num) {
			if ((GET_BSS_ROLE(priv_num) == MWIFIEX_BSS_ROLE_UAP &&
			     priv_num->bss_started) ||
			    (GET_BSS_ROLE(priv_num) == MWIFIEX_BSS_ROLE_STA &&
			     priv_num->media_connected))
				return 1;
		}
	}

	return 0;
}

static inline u8 mwifiex_is_tdls_link_setup(u8 status)
{
	switch (status) {
	case TDLS_SETUP_COMPLETE:
	case TDLS_CHAN_SWITCHING:
	case TDLS_IN_BASE_CHAN:
	case TDLS_IN_OFF_CHAN:
		return true;
	default:
		break;
	}

	return false;
}

/* Disable platform specific wakeup interrupt */
static inline void mwifiex_disable_wake(struct mwifiex_adapter *adapter)
{
	if (adapter->irq_wakeup >= 0) {
		disable_irq_wake(adapter->irq_wakeup);
		disable_irq(adapter->irq_wakeup);
		if (adapter->wake_by_wifi)
			/* Undo our disable, since interrupt handler already
			 * did this.
			 */
			enable_irq(adapter->irq_wakeup);

	}
}

/* Enable platform specific wakeup interrupt */
static inline void mwifiex_enable_wake(struct mwifiex_adapter *adapter)
{
	/* Enable platform specific wakeup interrupt */
	if (adapter->irq_wakeup >= 0) {
		adapter->wake_by_wifi = false;
		enable_irq(adapter->irq_wakeup);
		enable_irq_wake(adapter->irq_wakeup);
	}
}

int mwifiex_init_shutdown_fw(struct mwifiex_private *priv,
			     u32 func_init_shutdown);

int mwifiex_add_card(void *card, struct completion *fw_done,
		     struct mwifiex_if_ops *if_ops, u8 iface_type,
		     struct device *dev);
int mwifiex_remove_card(struct mwifiex_adapter *adapter);

void mwifiex_get_version(struct mwifiex_adapter *adapter, char *version,
			 int maxlen);
int mwifiex_request_set_multicast_list(struct mwifiex_private *priv,
			struct mwifiex_multicast_list *mcast_list);
int mwifiex_copy_mcast_addr(struct mwifiex_multicast_list *mlist,
			    struct net_device *dev);
int mwifiex_wait_queue_complete(struct mwifiex_adapter *adapter,
				struct cmd_ctrl_node *cmd_queued);
int mwifiex_bss_start(struct mwifiex_private *priv, struct cfg80211_bss *bss,
		      struct cfg80211_ssid *req_ssid);
int mwifiex_cancel_hs(struct mwifiex_private *priv, int cmd_type);
int mwifiex_enable_hs(struct mwifiex_adapter *adapter);
int mwifiex_disable_auto_ds(struct mwifiex_private *priv);
int mwifiex_drv_get_data_rate(struct mwifiex_private *priv, u32 *rate);
int mwifiex_request_scan(struct mwifiex_private *priv,
			 struct cfg80211_ssid *req_ssid);
int mwifiex_scan_networks(struct mwifiex_private *priv,
			  const struct mwifiex_user_scan_cfg *user_scan_in);
int mwifiex_set_radio(struct mwifiex_private *priv, u8 option);

int mwifiex_set_encode(struct mwifiex_private *priv, struct key_params *kp,
		       const u8 *key, int key_len, u8 key_index,
		       const u8 *mac_addr, int disable);

int mwifiex_set_gen_ie(struct mwifiex_private *priv, const u8 *ie, int ie_len);

int mwifiex_get_ver_ext(struct mwifiex_private *priv, u32 version_str_sel);

int mwifiex_remain_on_chan_cfg(struct mwifiex_private *priv, u16 action,
			       struct ieee80211_channel *chan,
			       unsigned int duration);

int mwifiex_get_stats_info(struct mwifiex_private *priv,
			   struct mwifiex_ds_get_stats *log);

int mwifiex_reg_write(struct mwifiex_private *priv, u32 reg_type,
		      u32 reg_offset, u32 reg_value);

int mwifiex_reg_read(struct mwifiex_private *priv, u32 reg_type,
		     u32 reg_offset, u32 *value);

int mwifiex_eeprom_read(struct mwifiex_private *priv, u16 offset, u16 bytes,
			u8 *value);

int mwifiex_set_11n_httx_cfg(struct mwifiex_private *priv, int data);

int mwifiex_get_11n_httx_cfg(struct mwifiex_private *priv, int *data);

int mwifiex_set_tx_rate_cfg(struct mwifiex_private *priv, int tx_rate_index);

int mwifiex_get_tx_rate_cfg(struct mwifiex_private *priv, int *tx_rate_index);

int mwifiex_drv_set_power(struct mwifiex_private *priv, u32 *ps_mode);

int mwifiex_drv_get_driver_version(struct mwifiex_adapter *adapter,
				   char *version, int max_len);

int mwifiex_set_tx_power(struct mwifiex_private *priv,
			 struct mwifiex_power_cfg *power_cfg);

int mwifiex_main_process(struct mwifiex_adapter *);

int mwifiex_queue_tx_pkt(struct mwifiex_private *priv, struct sk_buff *skb);

int mwifiex_get_bss_info(struct mwifiex_private *,
			 struct mwifiex_bss_info *);
int mwifiex_fill_new_bss_desc(struct mwifiex_private *priv,
			      struct cfg80211_bss *bss,
			      struct mwifiex_bssdescriptor *bss_desc);
int mwifiex_update_bss_desc_with_ie(struct mwifiex_adapter *adapter,
				    struct mwifiex_bssdescriptor *bss_entry);
int mwifiex_check_network_compatibility(struct mwifiex_private *priv,
					struct mwifiex_bssdescriptor *bss_desc);

u8 mwifiex_chan_type_to_sec_chan_offset(enum nl80211_channel_type chan_type);
u8 mwifiex_get_chan_type(struct mwifiex_private *priv);

struct wireless_dev *mwifiex_add_virtual_intf(struct wiphy *wiphy,
					      const char *name,
					      unsigned char name_assign_type,
					      enum nl80211_iftype type,
					      struct vif_params *params);
int mwifiex_del_virtual_intf(struct wiphy *wiphy, struct wireless_dev *wdev);

void mwifiex_set_sys_config_invalid_data(struct mwifiex_uap_bss_param *config);

int mwifiex_add_wowlan_magic_pkt_filter(struct mwifiex_adapter *adapter);

int mwifiex_set_mgmt_ies(struct mwifiex_private *priv,
			 struct cfg80211_beacon_data *data);
int mwifiex_del_mgmt_ies(struct mwifiex_private *priv);
u8 *mwifiex_11d_code_2_region(u8 code);
void mwifiex_uap_set_channel(struct mwifiex_private *priv,
			     struct mwifiex_uap_bss_param *bss_cfg,
			     struct cfg80211_chan_def chandef);
int mwifiex_config_start_uap(struct mwifiex_private *priv,
			     struct mwifiex_uap_bss_param *bss_cfg);
void mwifiex_uap_del_sta_data(struct mwifiex_private *priv,
			      struct mwifiex_sta_node *node);

void mwifiex_config_uap_11d(struct mwifiex_private *priv,
			    struct cfg80211_beacon_data *beacon_data);

void mwifiex_init_11h_params(struct mwifiex_private *priv);
int mwifiex_is_11h_active(struct mwifiex_private *priv);
int mwifiex_11h_activate(struct mwifiex_private *priv, bool flag);

void mwifiex_11h_process_join(struct mwifiex_private *priv, u8 **buffer,
			      struct mwifiex_bssdescriptor *bss_desc);
int mwifiex_11h_handle_event_chanswann(struct mwifiex_private *priv);
int mwifiex_dnld_dt_cfgdata(struct mwifiex_private *priv,
			    struct device_node *node, const char *prefix);
void mwifiex_dnld_txpwr_table(struct mwifiex_private *priv);

extern const struct ethtool_ops mwifiex_ethtool_ops;

void mwifiex_del_all_sta_list(struct mwifiex_private *priv);
void mwifiex_del_sta_entry(struct mwifiex_private *priv, const u8 *mac);
void
mwifiex_set_sta_ht_cap(struct mwifiex_private *priv, const u8 *ies,
		       int ies_len, struct mwifiex_sta_node *node);
struct mwifiex_sta_node *
mwifiex_add_sta_entry(struct mwifiex_private *priv, const u8 *mac);
struct mwifiex_sta_node *
mwifiex_get_sta_entry(struct mwifiex_private *priv, const u8 *mac);
u8 mwifiex_is_tdls_chan_switching(struct mwifiex_private *priv);
u8 mwifiex_is_tdls_off_chan(struct mwifiex_private *priv);
u8 mwifiex_is_send_cmd_allowed(struct mwifiex_private *priv);
int mwifiex_send_tdls_data_frame(struct mwifiex_private *priv, const u8 *peer,
				 u8 action_code, u8 dialog_token,
				 u16 status_code, const u8 *extra_ies,
				 size_t extra_ies_len);
int mwifiex_send_tdls_action_frame(struct mwifiex_private *priv, const u8 *peer,
				   u8 action_code, u8 dialog_token,
				   u16 status_code, const u8 *extra_ies,
				   size_t extra_ies_len);
void mwifiex_process_tdls_action_frame(struct mwifiex_private *priv,
				       u8 *buf, int len);
int mwifiex_tdls_oper(struct mwifiex_private *priv, const u8 *peer, u8 action);
int mwifiex_get_tdls_link_status(struct mwifiex_private *priv, const u8 *mac);
int mwifiex_get_tdls_list(struct mwifiex_private *priv,
			  struct tdls_peer_info *buf);
void mwifiex_disable_all_tdls_links(struct mwifiex_private *priv);
bool mwifiex_is_bss_in_11ac_mode(struct mwifiex_private *priv);
u8 mwifiex_get_center_freq_index(struct mwifiex_private *priv, u8 band,
				 u32 pri_chan, u8 chan_bw);
int mwifiex_init_channel_scan_gap(struct mwifiex_adapter *adapter);

int mwifiex_tdls_check_tx(struct mwifiex_private *priv, struct sk_buff *skb);
void mwifiex_flush_auto_tdls_list(struct mwifiex_private *priv);
void mwifiex_auto_tdls_update_peer_status(struct mwifiex_private *priv,
					  const u8 *mac, u8 link_status);
void mwifiex_auto_tdls_update_peer_signal(struct mwifiex_private *priv,
					  u8 *mac, s8 snr, s8 nflr);
void mwifiex_check_auto_tdls(struct timer_list *t);
void mwifiex_add_auto_tdls_peer(struct mwifiex_private *priv, const u8 *mac);
void mwifiex_setup_auto_tdls_timer(struct mwifiex_private *priv);
void mwifiex_clean_auto_tdls(struct mwifiex_private *priv);
int mwifiex_config_tdls_enable(struct mwifiex_private *priv);
int mwifiex_config_tdls_disable(struct mwifiex_private *priv);
int mwifiex_config_tdls_cs_params(struct mwifiex_private *priv);
int mwifiex_stop_tdls_cs(struct mwifiex_private *priv, const u8 *peer_mac);
int mwifiex_start_tdls_cs(struct mwifiex_private *priv, const u8 *peer_mac,
			  u8 primary_chan, u8 second_chan_offset, u8 band);

int mwifiex_cmd_issue_chan_report_request(struct mwifiex_private *priv,
					  struct host_cmd_ds_command *cmd,
					  void *data_buf);
int mwifiex_11h_handle_chanrpt_ready(struct mwifiex_private *priv,
				     struct sk_buff *skb);

void mwifiex_parse_tx_status_event(struct mwifiex_private *priv,
				   void *event_body);

struct sk_buff *
mwifiex_clone_skb_for_tx_status(struct mwifiex_private *priv,
				struct sk_buff *skb, u8 flag, u64 *cookie);
void mwifiex_dfs_cac_work_queue(struct work_struct *work);
void mwifiex_dfs_chan_sw_work_queue(struct work_struct *work);
void mwifiex_abort_cac(struct mwifiex_private *priv);
int mwifiex_stop_radar_detection(struct mwifiex_private *priv,
				 struct cfg80211_chan_def *chandef);
int mwifiex_11h_handle_radar_detected(struct mwifiex_private *priv,
				      struct sk_buff *skb);

void mwifiex_hist_data_set(struct mwifiex_private *priv, u8 rx_rate, s8 snr,
			   s8 nflr);
void mwifiex_hist_data_reset(struct mwifiex_private *priv);
void mwifiex_hist_data_add(struct mwifiex_private *priv,
			   u8 rx_rate, s8 snr, s8 nflr);
u8 mwifiex_adjust_data_rate(struct mwifiex_private *priv,
			    u8 rx_rate, u8 ht_info);

void mwifiex_drv_info_dump(struct mwifiex_adapter *adapter);
void mwifiex_prepare_fw_dump_info(struct mwifiex_adapter *adapter);
void mwifiex_upload_device_dump(struct mwifiex_adapter *adapter);
void *mwifiex_alloc_dma_align_buf(int rx_len, gfp_t flags);
void mwifiex_fw_dump_event(struct mwifiex_private *priv);
void mwifiex_queue_main_work(struct mwifiex_adapter *adapter);
int mwifiex_get_wakeup_reason(struct mwifiex_private *priv, u16 action,
			      int cmd_type,
			      struct mwifiex_ds_wakeup_reason *wakeup_reason);
int mwifiex_get_chan_info(struct mwifiex_private *priv,
			  struct mwifiex_channel_band *channel_band);
int mwifiex_ret_wakeup_reason(struct mwifiex_private *priv,
			      struct host_cmd_ds_command *resp,
			      struct host_cmd_ds_wakeup_reason *wakeup_reason);
void mwifiex_coex_ampdu_rxwinsize(struct mwifiex_adapter *adapter);
void mwifiex_11n_delba(struct mwifiex_private *priv, int tid);
int mwifiex_send_domain_info_cmd_fw(struct wiphy *wiphy);
void mwifiex_process_tx_pause_event(struct mwifiex_private *priv,
				    struct sk_buff *event);
void mwifiex_process_multi_chan_event(struct mwifiex_private *priv,
				      struct sk_buff *event_skb);
void mwifiex_multi_chan_resync(struct mwifiex_adapter *adapter);
int mwifiex_set_mac_address(struct mwifiex_private *priv,
			    struct net_device *dev,
			    bool external, u8 *new_mac);
void mwifiex_devdump_tmo_func(unsigned long function_context);

#ifdef CONFIG_DEBUG_FS
void mwifiex_debugfs_init(void);
void mwifiex_debugfs_remove(void);

void mwifiex_dev_debugfs_init(struct mwifiex_private *priv);
void mwifiex_dev_debugfs_remove(struct mwifiex_private *priv);
#endif
int mwifiex_reinit_sw(struct mwifiex_adapter *adapter);
int mwifiex_shutdown_sw(struct mwifiex_adapter *adapter);
#endif /* !_MWIFIEX_MAIN_H_ */
