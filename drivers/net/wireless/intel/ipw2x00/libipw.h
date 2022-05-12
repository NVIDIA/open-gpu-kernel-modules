/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Merged with mainline ieee80211.h in Aug 2004.  Original ieee802_11
 * remains copyright by the original authors
 *
 * Portions of the merged code are based on Host AP (software wireless
 * LAN access point) driver for Intersil Prism2/2.5/3.
 *
 * Copyright (c) 2001-2002, SSH Communications Security Corp and Jouni Malinen
 * <j@w1.fi>
 * Copyright (c) 2002-2003, Jouni Malinen <j@w1.fi>
 *
 * Adaption to a generic IEEE 802.11 stack by James Ketrenos
 * <jketreno@linux.intel.com>
 * Copyright (c) 2004-2005, Intel Corporation
 *
 * API Version History
 * 1.0.x -- Initial version
 * 1.1.x -- Added radiotap, QoS, TIM, libipw_geo APIs,
 *          various structure changes, and crypto API init method
 */
#ifndef LIBIPW_H
#define LIBIPW_H
#include <linux/if_ether.h>	/* ETH_ALEN */
#include <linux/kernel.h>	/* ARRAY_SIZE */
#include <linux/wireless.h>
#include <linux/ieee80211.h>

#include <net/lib80211.h>
#include <net/cfg80211.h>

#define LIBIPW_VERSION "git-1.1.13"

#define LIBIPW_DATA_LEN		2304
/* Maximum size for the MA-UNITDATA primitive, 802.11 standard section
   6.2.1.1.2.

   The figure in section 7.1.2 suggests a body size of up to 2312
   bytes is allowed, which is a bit confusing, I suspect this
   represents the 2304 bytes of real data, plus a possible 8 bytes of
   WEP IV and ICV. (this interpretation suggested by Ramiro Barreiro) */

#define LIBIPW_1ADDR_LEN 10
#define LIBIPW_2ADDR_LEN 16
#define LIBIPW_3ADDR_LEN 24
#define LIBIPW_4ADDR_LEN 30
#define LIBIPW_FCS_LEN    4
#define LIBIPW_HLEN			(LIBIPW_4ADDR_LEN)
#define LIBIPW_FRAME_LEN		(LIBIPW_DATA_LEN + LIBIPW_HLEN)

#define MIN_FRAG_THRESHOLD     256U
#define	MAX_FRAG_THRESHOLD     2346U

/* QOS control */
#define LIBIPW_QCTL_TID		0x000F

/* debug macros */

#ifdef CONFIG_LIBIPW_DEBUG
extern u32 libipw_debug_level;
#define LIBIPW_DEBUG(level, fmt, args...) \
do { if (libipw_debug_level & (level)) \
  printk(KERN_DEBUG "libipw: %s " fmt, __func__ , ## args); } while (0)
#else
#define LIBIPW_DEBUG(level, fmt, args...) do {} while (0)
#endif				/* CONFIG_LIBIPW_DEBUG */

/*
 * To use the debug system:
 *
 * If you are defining a new debug classification, simply add it to the #define
 * list here in the form of:
 *
 * #define LIBIPW_DL_xxxx VALUE
 *
 * shifting value to the left one bit from the previous entry.  xxxx should be
 * the name of the classification (for example, WEP)
 *
 * You then need to either add a LIBIPW_xxxx_DEBUG() macro definition for your
 * classification, or use LIBIPW_DEBUG(LIBIPW_DL_xxxx, ...) whenever you want
 * to send output to that classification.
 *
 * To add your debug level to the list of levels seen when you perform
 *
 * % cat /proc/net/ieee80211/debug_level
 *
 * you simply need to add your entry to the libipw_debug_level array.
 *
 * If you do not see debug_level in /proc/net/ieee80211 then you do not have
 * CONFIG_LIBIPW_DEBUG defined in your kernel configuration
 *
 */

#define LIBIPW_DL_INFO          (1<<0)
#define LIBIPW_DL_WX            (1<<1)
#define LIBIPW_DL_SCAN          (1<<2)
#define LIBIPW_DL_STATE         (1<<3)
#define LIBIPW_DL_MGMT          (1<<4)
#define LIBIPW_DL_FRAG          (1<<5)
#define LIBIPW_DL_DROP          (1<<7)

#define LIBIPW_DL_TX            (1<<8)
#define LIBIPW_DL_RX            (1<<9)
#define LIBIPW_DL_QOS           (1<<31)

#define LIBIPW_ERROR(f, a...) printk(KERN_ERR "libipw: " f, ## a)
#define LIBIPW_WARNING(f, a...) printk(KERN_WARNING "libipw: " f, ## a)
#define LIBIPW_DEBUG_INFO(f, a...)   LIBIPW_DEBUG(LIBIPW_DL_INFO, f, ## a)

#define LIBIPW_DEBUG_WX(f, a...)     LIBIPW_DEBUG(LIBIPW_DL_WX, f, ## a)
#define LIBIPW_DEBUG_SCAN(f, a...)   LIBIPW_DEBUG(LIBIPW_DL_SCAN, f, ## a)
#define LIBIPW_DEBUG_STATE(f, a...)  LIBIPW_DEBUG(LIBIPW_DL_STATE, f, ## a)
#define LIBIPW_DEBUG_MGMT(f, a...)  LIBIPW_DEBUG(LIBIPW_DL_MGMT, f, ## a)
#define LIBIPW_DEBUG_FRAG(f, a...)  LIBIPW_DEBUG(LIBIPW_DL_FRAG, f, ## a)
#define LIBIPW_DEBUG_DROP(f, a...)  LIBIPW_DEBUG(LIBIPW_DL_DROP, f, ## a)
#define LIBIPW_DEBUG_TX(f, a...)  LIBIPW_DEBUG(LIBIPW_DL_TX, f, ## a)
#define LIBIPW_DEBUG_RX(f, a...)  LIBIPW_DEBUG(LIBIPW_DL_RX, f, ## a)
#define LIBIPW_DEBUG_QOS(f, a...)  LIBIPW_DEBUG(LIBIPW_DL_QOS, f, ## a)
#include <linux/netdevice.h>
#include <linux/if_arp.h>	/* ARPHRD_ETHER */

#ifndef WIRELESS_SPY
#define WIRELESS_SPY		/* enable iwspy support */
#endif
#include <net/iw_handler.h>	/* new driver API */

#define ETH_P_PREAUTH 0x88C7	/* IEEE 802.11i pre-authentication */

#ifndef ETH_P_80211_RAW
#define ETH_P_80211_RAW (ETH_P_ECONET + 1)
#endif

/* IEEE 802.11 defines */

#define P80211_OUI_LEN 3

struct libipw_snap_hdr {

	u8 dsap;		/* always 0xAA */
	u8 ssap;		/* always 0xAA */
	u8 ctrl;		/* always 0x03 */
	u8 oui[P80211_OUI_LEN];	/* organizational universal id */

} __packed;

#define SNAP_SIZE sizeof(struct libipw_snap_hdr)

#define WLAN_FC_GET_VERS(fc) ((fc) & IEEE80211_FCTL_VERS)
#define WLAN_FC_GET_TYPE(fc) ((fc) & IEEE80211_FCTL_FTYPE)
#define WLAN_FC_GET_STYPE(fc) ((fc) & IEEE80211_FCTL_STYPE)

#define WLAN_GET_SEQ_FRAG(seq) ((seq) & IEEE80211_SCTL_FRAG)
#define WLAN_GET_SEQ_SEQ(seq)  (((seq) & IEEE80211_SCTL_SEQ) >> 4)

#define LIBIPW_STATMASK_SIGNAL (1<<0)
#define LIBIPW_STATMASK_RSSI (1<<1)
#define LIBIPW_STATMASK_NOISE (1<<2)
#define LIBIPW_STATMASK_RATE (1<<3)
#define LIBIPW_STATMASK_WEMASK 0x7

#define LIBIPW_CCK_MODULATION    (1<<0)
#define LIBIPW_OFDM_MODULATION   (1<<1)

#define LIBIPW_24GHZ_BAND     (1<<0)
#define LIBIPW_52GHZ_BAND     (1<<1)

#define LIBIPW_CCK_RATE_1MB		        0x02
#define LIBIPW_CCK_RATE_2MB		        0x04
#define LIBIPW_CCK_RATE_5MB		        0x0B
#define LIBIPW_CCK_RATE_11MB		        0x16
#define LIBIPW_OFDM_RATE_6MB		        0x0C
#define LIBIPW_OFDM_RATE_9MB		        0x12
#define LIBIPW_OFDM_RATE_12MB		0x18
#define LIBIPW_OFDM_RATE_18MB		0x24
#define LIBIPW_OFDM_RATE_24MB		0x30
#define LIBIPW_OFDM_RATE_36MB		0x48
#define LIBIPW_OFDM_RATE_48MB		0x60
#define LIBIPW_OFDM_RATE_54MB		0x6C
#define LIBIPW_BASIC_RATE_MASK		0x80

#define LIBIPW_CCK_RATE_1MB_MASK		(1<<0)
#define LIBIPW_CCK_RATE_2MB_MASK		(1<<1)
#define LIBIPW_CCK_RATE_5MB_MASK		(1<<2)
#define LIBIPW_CCK_RATE_11MB_MASK		(1<<3)
#define LIBIPW_OFDM_RATE_6MB_MASK		(1<<4)
#define LIBIPW_OFDM_RATE_9MB_MASK		(1<<5)
#define LIBIPW_OFDM_RATE_12MB_MASK		(1<<6)
#define LIBIPW_OFDM_RATE_18MB_MASK		(1<<7)
#define LIBIPW_OFDM_RATE_24MB_MASK		(1<<8)
#define LIBIPW_OFDM_RATE_36MB_MASK		(1<<9)
#define LIBIPW_OFDM_RATE_48MB_MASK		(1<<10)
#define LIBIPW_OFDM_RATE_54MB_MASK		(1<<11)

#define LIBIPW_CCK_RATES_MASK	        0x0000000F
#define LIBIPW_CCK_BASIC_RATES_MASK	(LIBIPW_CCK_RATE_1MB_MASK | \
	LIBIPW_CCK_RATE_2MB_MASK)
#define LIBIPW_CCK_DEFAULT_RATES_MASK	(LIBIPW_CCK_BASIC_RATES_MASK | \
        LIBIPW_CCK_RATE_5MB_MASK | \
        LIBIPW_CCK_RATE_11MB_MASK)

#define LIBIPW_OFDM_RATES_MASK		0x00000FF0
#define LIBIPW_OFDM_BASIC_RATES_MASK	(LIBIPW_OFDM_RATE_6MB_MASK | \
	LIBIPW_OFDM_RATE_12MB_MASK | \
	LIBIPW_OFDM_RATE_24MB_MASK)
#define LIBIPW_OFDM_DEFAULT_RATES_MASK	(LIBIPW_OFDM_BASIC_RATES_MASK | \
	LIBIPW_OFDM_RATE_9MB_MASK  | \
	LIBIPW_OFDM_RATE_18MB_MASK | \
	LIBIPW_OFDM_RATE_36MB_MASK | \
	LIBIPW_OFDM_RATE_48MB_MASK | \
	LIBIPW_OFDM_RATE_54MB_MASK)
#define LIBIPW_DEFAULT_RATES_MASK (LIBIPW_OFDM_DEFAULT_RATES_MASK | \
                                LIBIPW_CCK_DEFAULT_RATES_MASK)

#define LIBIPW_NUM_OFDM_RATES	    8
#define LIBIPW_NUM_CCK_RATES	            4
#define LIBIPW_OFDM_SHIFT_MASK_A         4

/* NOTE: This data is for statistical purposes; not all hardware provides this
 *       information for frames received.
 *       For libipw_rx_mgt, you need to set at least the 'len' parameter.
 */
struct libipw_rx_stats {
	u32 mac_time;
	s8 rssi;
	u8 signal;
	u8 noise;
	u16 rate;		/* in 100 kbps */
	u8 received_channel;
	u8 control;
	u8 mask;
	u8 freq;
	u16 len;
	u64 tsf;
	u32 beacon_time;
};

/* IEEE 802.11 requires that STA supports concurrent reception of at least
 * three fragmented frames. This define can be increased to support more
 * concurrent frames, but it should be noted that each entry can consume about
 * 2 kB of RAM and increasing cache size will slow down frame reassembly. */
#define LIBIPW_FRAG_CACHE_LEN 4

struct libipw_frag_entry {
	unsigned long first_frag_time;
	unsigned int seq;
	unsigned int last_frag;
	struct sk_buff *skb;
	u8 src_addr[ETH_ALEN];
	u8 dst_addr[ETH_ALEN];
};

struct libipw_stats {
	unsigned int tx_unicast_frames;
	unsigned int tx_multicast_frames;
	unsigned int tx_fragments;
	unsigned int tx_unicast_octets;
	unsigned int tx_multicast_octets;
	unsigned int tx_deferred_transmissions;
	unsigned int tx_single_retry_frames;
	unsigned int tx_multiple_retry_frames;
	unsigned int tx_retry_limit_exceeded;
	unsigned int tx_discards;
	unsigned int rx_unicast_frames;
	unsigned int rx_multicast_frames;
	unsigned int rx_fragments;
	unsigned int rx_unicast_octets;
	unsigned int rx_multicast_octets;
	unsigned int rx_fcs_errors;
	unsigned int rx_discards_no_buffer;
	unsigned int tx_discards_wrong_sa;
	unsigned int rx_discards_undecryptable;
	unsigned int rx_message_in_msg_fragments;
	unsigned int rx_message_in_bad_msg_fragments;
};

struct libipw_device;

#define SEC_KEY_1		(1<<0)
#define SEC_KEY_2		(1<<1)
#define SEC_KEY_3		(1<<2)
#define SEC_KEY_4		(1<<3)
#define SEC_ACTIVE_KEY		(1<<4)
#define SEC_AUTH_MODE		(1<<5)
#define SEC_UNICAST_GROUP	(1<<6)
#define SEC_LEVEL		(1<<7)
#define SEC_ENABLED		(1<<8)
#define SEC_ENCRYPT		(1<<9)

#define SEC_LEVEL_0		0	/* None */
#define SEC_LEVEL_1		1	/* WEP 40 and 104 bit */
#define SEC_LEVEL_2		2	/* Level 1 + TKIP */
#define SEC_LEVEL_2_CKIP	3	/* Level 1 + CKIP */
#define SEC_LEVEL_3		4	/* Level 2 + CCMP */

#define SEC_ALG_NONE		0
#define SEC_ALG_WEP		1
#define SEC_ALG_TKIP		2
#define SEC_ALG_CCMP		3

#define WEP_KEYS		4
#define WEP_KEY_LEN		13
#define SCM_KEY_LEN		32
#define SCM_TEMPORAL_KEY_LENGTH	16

struct libipw_security {
	u16 active_key:2, enabled:1, unicast_uses_group:1, encrypt:1;
	u8 auth_mode;
	u8 encode_alg[WEP_KEYS];
	u8 key_sizes[WEP_KEYS];
	u8 keys[WEP_KEYS][SCM_KEY_LEN];
	u8 level;
	u16 flags;
} __packed;

/*

 802.11 data frame from AP

      ,-------------------------------------------------------------------.
Bytes |  2   |  2   |    6    |    6    |    6    |  2   | 0..2312 |   4  |
      |------|------|---------|---------|---------|------|---------|------|
Desc. | ctrl | dura |  DA/RA  |   TA    |    SA   | Sequ |  frame  |  fcs |
      |      | tion | (BSSID) |         |         | ence |  data   |      |
      `-------------------------------------------------------------------'

Total: 28-2340 bytes

*/

#define BEACON_PROBE_SSID_ID_POSITION 12

struct libipw_hdr_1addr {
	__le16 frame_ctl;
	__le16 duration_id;
	u8 addr1[ETH_ALEN];
	u8 payload[];
} __packed;

struct libipw_hdr_2addr {
	__le16 frame_ctl;
	__le16 duration_id;
	u8 addr1[ETH_ALEN];
	u8 addr2[ETH_ALEN];
	u8 payload[];
} __packed;

struct libipw_hdr_3addr {
	__le16 frame_ctl;
	__le16 duration_id;
	u8 addr1[ETH_ALEN];
	u8 addr2[ETH_ALEN];
	u8 addr3[ETH_ALEN];
	__le16 seq_ctl;
	u8 payload[];
} __packed;

struct libipw_hdr_4addr {
	__le16 frame_ctl;
	__le16 duration_id;
	u8 addr1[ETH_ALEN];
	u8 addr2[ETH_ALEN];
	u8 addr3[ETH_ALEN];
	__le16 seq_ctl;
	u8 addr4[ETH_ALEN];
	u8 payload[];
} __packed;

struct libipw_hdr_3addrqos {
	__le16 frame_ctl;
	__le16 duration_id;
	u8 addr1[ETH_ALEN];
	u8 addr2[ETH_ALEN];
	u8 addr3[ETH_ALEN];
	__le16 seq_ctl;
	u8 payload[0];
	__le16 qos_ctl;
} __packed;

struct libipw_info_element {
	u8 id;
	u8 len;
	u8 data[];
} __packed;

/*
 * These are the data types that can make up management packets
 *
	u16 auth_algorithm;
	u16 auth_sequence;
	u16 beacon_interval;
	u16 capability;
	u8 current_ap[ETH_ALEN];
	u16 listen_interval;
	struct {
		u16 association_id:14, reserved:2;
	} __packed;
	u32 time_stamp[2];
	u16 reason;
	u16 status;
*/

struct libipw_auth {
	struct libipw_hdr_3addr header;
	__le16 algorithm;
	__le16 transaction;
	__le16 status;
	/* challenge */
	struct libipw_info_element info_element[];
} __packed;

struct libipw_channel_switch {
	u8 id;
	u8 len;
	u8 mode;
	u8 channel;
	u8 count;
} __packed;

struct libipw_action {
	struct libipw_hdr_3addr header;
	u8 category;
	u8 action;
	union {
		struct libipw_action_exchange {
			u8 token;
			struct libipw_info_element info_element[0];
		} exchange;
		struct libipw_channel_switch channel_switch;

	} format;
} __packed;

struct libipw_disassoc {
	struct libipw_hdr_3addr header;
	__le16 reason;
} __packed;

/* Alias deauth for disassoc */
#define libipw_deauth libipw_disassoc

struct libipw_probe_request {
	struct libipw_hdr_3addr header;
	/* SSID, supported rates */
	struct libipw_info_element info_element[];
} __packed;

struct libipw_probe_response {
	struct libipw_hdr_3addr header;
	__le32 time_stamp[2];
	__le16 beacon_interval;
	__le16 capability;
	/* SSID, supported rates, FH params, DS params,
	 * CF params, IBSS params, TIM (if beacon), RSN */
	struct libipw_info_element info_element[];
} __packed;

/* Alias beacon for probe_response */
#define libipw_beacon libipw_probe_response

struct libipw_assoc_request {
	struct libipw_hdr_3addr header;
	__le16 capability;
	__le16 listen_interval;
	/* SSID, supported rates, RSN */
	struct libipw_info_element info_element[];
} __packed;

struct libipw_reassoc_request {
	struct libipw_hdr_3addr header;
	__le16 capability;
	__le16 listen_interval;
	u8 current_ap[ETH_ALEN];
	struct libipw_info_element info_element[];
} __packed;

struct libipw_assoc_response {
	struct libipw_hdr_3addr header;
	__le16 capability;
	__le16 status;
	__le16 aid;
	/* supported rates */
	struct libipw_info_element info_element[];
} __packed;

struct libipw_txb {
	u8 nr_frags;
	u8 encrypted;
	u8 rts_included;
	u8 reserved;
	u16 frag_size;
	u16 payload_size;
	struct sk_buff *fragments[];
};

/* SWEEP TABLE ENTRIES NUMBER */
#define MAX_SWEEP_TAB_ENTRIES		  42
#define MAX_SWEEP_TAB_ENTRIES_PER_PACKET  7
/* MAX_RATES_LENGTH needs to be 12.  The spec says 8, and many APs
 * only use 8, and then use extended rates for the remaining supported
 * rates.  Other APs, however, stick all of their supported rates on the
 * main rates information element... */
#define MAX_RATES_LENGTH                  ((u8)12)
#define MAX_RATES_EX_LENGTH               ((u8)16)
#define MAX_NETWORK_COUNT                  128

#define CRC_LENGTH                 4U

#define MAX_WPA_IE_LEN 64

#define NETWORK_HAS_OFDM       (1<<1)
#define NETWORK_HAS_CCK        (1<<2)

/* QoS structure */
#define NETWORK_HAS_QOS_PARAMETERS      (1<<3)
#define NETWORK_HAS_QOS_INFORMATION     (1<<4)
#define NETWORK_HAS_QOS_MASK            (NETWORK_HAS_QOS_PARAMETERS | \
					 NETWORK_HAS_QOS_INFORMATION)

/* 802.11h */
#define NETWORK_HAS_POWER_CONSTRAINT    (1<<5)
#define NETWORK_HAS_CSA                 (1<<6)
#define NETWORK_HAS_QUIET               (1<<7)
#define NETWORK_HAS_IBSS_DFS            (1<<8)
#define NETWORK_HAS_TPC_REPORT          (1<<9)

#define NETWORK_HAS_ERP_VALUE           (1<<10)

#define QOS_QUEUE_NUM                   4
#define QOS_OUI_LEN                     3
#define QOS_OUI_TYPE                    2
#define QOS_ELEMENT_ID                  221
#define QOS_OUI_INFO_SUB_TYPE           0
#define QOS_OUI_PARAM_SUB_TYPE          1
#define QOS_VERSION_1                   1
#define QOS_AIFSN_MIN_VALUE             2

struct libipw_qos_information_element {
	u8 elementID;
	u8 length;
	u8 qui[QOS_OUI_LEN];
	u8 qui_type;
	u8 qui_subtype;
	u8 version;
	u8 ac_info;
} __packed;

struct libipw_qos_ac_parameter {
	u8 aci_aifsn;
	u8 ecw_min_max;
	__le16 tx_op_limit;
} __packed;

struct libipw_qos_parameter_info {
	struct libipw_qos_information_element info_element;
	u8 reserved;
	struct libipw_qos_ac_parameter ac_params_record[QOS_QUEUE_NUM];
} __packed;

struct libipw_qos_parameters {
	__le16 cw_min[QOS_QUEUE_NUM];
	__le16 cw_max[QOS_QUEUE_NUM];
	u8 aifs[QOS_QUEUE_NUM];
	u8 flag[QOS_QUEUE_NUM];
	__le16 tx_op_limit[QOS_QUEUE_NUM];
} __packed;

struct libipw_qos_data {
	struct libipw_qos_parameters parameters;
	int active;
	int supported;
	u8 param_count;
	u8 old_param_count;
};

struct libipw_tim_parameters {
	u8 tim_count;
	u8 tim_period;
} __packed;

/*******************************************************/

struct libipw_tpc_report {
	u8 transmit_power;
	u8 link_margin;
} __packed;

struct libipw_channel_map {
	u8 channel;
	u8 map;
} __packed;

struct libipw_ibss_dfs {
	struct libipw_info_element ie;
	u8 owner[ETH_ALEN];
	u8 recovery_interval;
	struct libipw_channel_map channel_map[];
};

struct libipw_csa {
	u8 mode;
	u8 channel;
	u8 count;
} __packed;

struct libipw_quiet {
	u8 count;
	u8 period;
	u8 duration;
	u8 offset;
} __packed;

struct libipw_network {
	/* These entries are used to identify a unique network */
	u8 bssid[ETH_ALEN];
	u8 channel;
	/* Ensure null-terminated for any debug msgs */
	u8 ssid[IW_ESSID_MAX_SIZE + 1];
	u8 ssid_len;

	struct libipw_qos_data qos_data;

	/* These are network statistics */
	struct libipw_rx_stats stats;
	u16 capability;
	u8 rates[MAX_RATES_LENGTH];
	u8 rates_len;
	u8 rates_ex[MAX_RATES_EX_LENGTH];
	u8 rates_ex_len;
	unsigned long last_scanned;
	u8 mode;
	u32 flags;
	u32 last_associate;
	u32 time_stamp[2];
	u16 beacon_interval;
	u16 listen_interval;
	u16 atim_window;
	u8 erp_value;
	u8 wpa_ie[MAX_WPA_IE_LEN];
	size_t wpa_ie_len;
	u8 rsn_ie[MAX_WPA_IE_LEN];
	size_t rsn_ie_len;
	struct libipw_tim_parameters tim;

	/* 802.11h info */

	/* Power Constraint - mandatory if spctrm mgmt required */
	u8 power_constraint;

	/* TPC Report - mandatory if spctrm mgmt required */
	struct libipw_tpc_report tpc_report;

	/* Channel Switch Announcement - optional if spctrm mgmt required */
	struct libipw_csa csa;

	/* Quiet - optional if spctrm mgmt required */
	struct libipw_quiet quiet;

	struct list_head list;
};

enum libipw_state {
	LIBIPW_UNINITIALIZED = 0,
	LIBIPW_INITIALIZED,
	LIBIPW_ASSOCIATING,
	LIBIPW_ASSOCIATED,
	LIBIPW_AUTHENTICATING,
	LIBIPW_AUTHENTICATED,
	LIBIPW_SHUTDOWN
};

#define DEFAULT_MAX_SCAN_AGE (15 * HZ)
#define DEFAULT_FTS 2346

#define CFG_LIBIPW_RESERVE_FCS (1<<0)
#define CFG_LIBIPW_COMPUTE_FCS (1<<1)
#define CFG_LIBIPW_RTS (1<<2)

#define LIBIPW_24GHZ_MIN_CHANNEL 1
#define LIBIPW_24GHZ_MAX_CHANNEL 14
#define LIBIPW_24GHZ_CHANNELS (LIBIPW_24GHZ_MAX_CHANNEL - \
				  LIBIPW_24GHZ_MIN_CHANNEL + 1)

#define LIBIPW_52GHZ_MIN_CHANNEL 34
#define LIBIPW_52GHZ_MAX_CHANNEL 165
#define LIBIPW_52GHZ_CHANNELS (LIBIPW_52GHZ_MAX_CHANNEL - \
				  LIBIPW_52GHZ_MIN_CHANNEL + 1)

enum {
	LIBIPW_CH_PASSIVE_ONLY = (1 << 0),
	LIBIPW_CH_80211H_RULES = (1 << 1),
	LIBIPW_CH_B_ONLY = (1 << 2),
	LIBIPW_CH_NO_IBSS = (1 << 3),
	LIBIPW_CH_UNIFORM_SPREADING = (1 << 4),
	LIBIPW_CH_RADAR_DETECT = (1 << 5),
	LIBIPW_CH_INVALID = (1 << 6),
};

struct libipw_channel {
	u32 freq;	/* in MHz */
	u8 channel;
	u8 flags;
	u8 max_power;	/* in dBm */
};

struct libipw_geo {
	u8 name[4];
	u8 bg_channels;
	u8 a_channels;
	struct libipw_channel bg[LIBIPW_24GHZ_CHANNELS];
	struct libipw_channel a[LIBIPW_52GHZ_CHANNELS];
};

struct libipw_device {
	struct net_device *dev;
	struct wireless_dev wdev;
	struct libipw_security sec;

	/* Bookkeeping structures */
	struct libipw_stats ieee_stats;

	struct libipw_geo geo;
	struct ieee80211_supported_band bg_band;
	struct ieee80211_supported_band a_band;

	/* Probe / Beacon management */
	struct list_head network_free_list;
	struct list_head network_list;
	struct libipw_network *networks[MAX_NETWORK_COUNT];
	int scans;
	int scan_age;

	int iw_mode;		/* operating mode (IW_MODE_*) */
	struct iw_spy_data spy_data;	/* iwspy support */

	spinlock_t lock;

	int tx_headroom;	/* Set to size of any additional room needed at front
				 * of allocated Tx SKBs */
	u32 config;

	/* WEP and other encryption related settings at the device level */
	int open_wep;		/* Set to 1 to allow unencrypted frames */

	/* If the host performs {en,de}cryption, then set to 1 */
	int host_encrypt;
	int host_encrypt_msdu;
	int host_decrypt;
	/* host performs multicast decryption */
	int host_mc_decrypt;

	/* host should strip IV and ICV from protected frames */
	/* meaningful only when hardware decryption is being used */
	int host_strip_iv_icv;

	int host_open_frag;
	int ieee802_1x;		/* is IEEE 802.1X used */

	/* WPA data */
	int wpa_enabled;
	int drop_unencrypted;
	int privacy_invoked;
	size_t wpa_ie_len;
	u8 *wpa_ie;

	struct lib80211_crypt_info crypt_info;

	int bcrx_sta_key;	/* use individual keys to override default keys even
				 * with RX of broad/multicast frames */

	/* Fragmentation structures */
	struct libipw_frag_entry frag_cache[LIBIPW_FRAG_CACHE_LEN];
	unsigned int frag_next_idx;
	u16 fts;		/* Fragmentation Threshold */
	u16 rts;		/* RTS threshold */

	/* Association info */
	u8 bssid[ETH_ALEN];

	enum libipw_state state;

	int mode;		/* A, B, G */
	int modulation;		/* CCK, OFDM */
	int freq_band;		/* 2.4Ghz, 5.2Ghz, Mixed */
	int abg_true;		/* ABG flag              */

	int perfect_rssi;
	int worst_rssi;

	u16 prev_seq_ctl;	/* used to drop duplicate frames */

	/* Callback functions */
	void (*set_security) (struct net_device * dev,
			      struct libipw_security * sec);
	netdev_tx_t (*hard_start_xmit) (struct libipw_txb * txb,
					struct net_device * dev, int pri);
	int (*is_queue_full) (struct net_device * dev, int pri);

	int (*handle_management) (struct net_device * dev,
				  struct libipw_network * network, u16 type);
	int (*is_qos_active) (struct net_device *dev, struct sk_buff *skb);

	/* Typical STA methods */
	int (*handle_auth) (struct net_device * dev,
			    struct libipw_auth * auth);
	int (*handle_deauth) (struct net_device * dev,
			      struct libipw_deauth * auth);
	int (*handle_action) (struct net_device * dev,
			      struct libipw_action * action,
			      struct libipw_rx_stats * stats);
	int (*handle_disassoc) (struct net_device * dev,
				struct libipw_disassoc * assoc);
	int (*handle_beacon) (struct net_device * dev,
			      struct libipw_beacon * beacon,
			      struct libipw_network * network);
	int (*handle_probe_response) (struct net_device * dev,
				      struct libipw_probe_response * resp,
				      struct libipw_network * network);
	int (*handle_probe_request) (struct net_device * dev,
				     struct libipw_probe_request * req,
				     struct libipw_rx_stats * stats);
	int (*handle_assoc_response) (struct net_device * dev,
				      struct libipw_assoc_response * resp,
				      struct libipw_network * network);

	/* Typical AP methods */
	int (*handle_assoc_request) (struct net_device * dev);
	int (*handle_reassoc_request) (struct net_device * dev,
				       struct libipw_reassoc_request * req);

	/* This must be the last item so that it points to the data
	 * allocated beyond this structure by alloc_libipw */
	u8 priv[];
};

#define IEEE_A            (1<<0)
#define IEEE_B            (1<<1)
#define IEEE_G            (1<<2)
#define IEEE_MODE_MASK    (IEEE_A|IEEE_B|IEEE_G)

static inline void *libipw_priv(struct net_device *dev)
{
	return ((struct libipw_device *)netdev_priv(dev))->priv;
}

static inline int libipw_is_valid_mode(struct libipw_device *ieee,
					  int mode)
{
	/*
	 * It is possible for both access points and our device to support
	 * combinations of modes, so as long as there is one valid combination
	 * of ap/device supported modes, then return success
	 *
	 */
	if ((mode & IEEE_A) &&
	    (ieee->modulation & LIBIPW_OFDM_MODULATION) &&
	    (ieee->freq_band & LIBIPW_52GHZ_BAND))
		return 1;

	if ((mode & IEEE_G) &&
	    (ieee->modulation & LIBIPW_OFDM_MODULATION) &&
	    (ieee->freq_band & LIBIPW_24GHZ_BAND))
		return 1;

	if ((mode & IEEE_B) &&
	    (ieee->modulation & LIBIPW_CCK_MODULATION) &&
	    (ieee->freq_band & LIBIPW_24GHZ_BAND))
		return 1;

	return 0;
}

static inline int libipw_get_hdrlen(u16 fc)
{
	int hdrlen = LIBIPW_3ADDR_LEN;
	u16 stype = WLAN_FC_GET_STYPE(fc);

	switch (WLAN_FC_GET_TYPE(fc)) {
	case IEEE80211_FTYPE_DATA:
		if ((fc & IEEE80211_FCTL_FROMDS) && (fc & IEEE80211_FCTL_TODS))
			hdrlen = LIBIPW_4ADDR_LEN;
		if (stype & IEEE80211_STYPE_QOS_DATA)
			hdrlen += 2;
		break;
	case IEEE80211_FTYPE_CTL:
		switch (WLAN_FC_GET_STYPE(fc)) {
		case IEEE80211_STYPE_CTS:
		case IEEE80211_STYPE_ACK:
			hdrlen = LIBIPW_1ADDR_LEN;
			break;
		default:
			hdrlen = LIBIPW_2ADDR_LEN;
			break;
		}
		break;
	}

	return hdrlen;
}

static inline u8 *libipw_get_payload(struct ieee80211_hdr *hdr)
{
	switch (libipw_get_hdrlen(le16_to_cpu(hdr->frame_control))) {
	case LIBIPW_1ADDR_LEN:
		return ((struct libipw_hdr_1addr *)hdr)->payload;
	case LIBIPW_2ADDR_LEN:
		return ((struct libipw_hdr_2addr *)hdr)->payload;
	case LIBIPW_3ADDR_LEN:
		return ((struct libipw_hdr_3addr *)hdr)->payload;
	case LIBIPW_4ADDR_LEN:
		return ((struct libipw_hdr_4addr *)hdr)->payload;
	}
	return NULL;
}

static inline int libipw_is_ofdm_rate(u8 rate)
{
	switch (rate & ~LIBIPW_BASIC_RATE_MASK) {
	case LIBIPW_OFDM_RATE_6MB:
	case LIBIPW_OFDM_RATE_9MB:
	case LIBIPW_OFDM_RATE_12MB:
	case LIBIPW_OFDM_RATE_18MB:
	case LIBIPW_OFDM_RATE_24MB:
	case LIBIPW_OFDM_RATE_36MB:
	case LIBIPW_OFDM_RATE_48MB:
	case LIBIPW_OFDM_RATE_54MB:
		return 1;
	}
	return 0;
}

static inline int libipw_is_cck_rate(u8 rate)
{
	switch (rate & ~LIBIPW_BASIC_RATE_MASK) {
	case LIBIPW_CCK_RATE_1MB:
	case LIBIPW_CCK_RATE_2MB:
	case LIBIPW_CCK_RATE_5MB:
	case LIBIPW_CCK_RATE_11MB:
		return 1;
	}
	return 0;
}

/* libipw.c */
void free_libipw(struct net_device *dev, int monitor);
struct net_device *alloc_libipw(int sizeof_priv, int monitor);

void libipw_networks_age(struct libipw_device *ieee, unsigned long age_secs);

int libipw_set_encryption(struct libipw_device *ieee);

/* libipw_tx.c */
netdev_tx_t libipw_xmit(struct sk_buff *skb, struct net_device *dev);
void libipw_txb_free(struct libipw_txb *);

/* libipw_rx.c */
void libipw_rx_any(struct libipw_device *ieee, struct sk_buff *skb,
		   struct libipw_rx_stats *stats);
int libipw_rx(struct libipw_device *ieee, struct sk_buff *skb,
	      struct libipw_rx_stats *rx_stats);
/* make sure to set stats->len */
void libipw_rx_mgt(struct libipw_device *ieee, struct libipw_hdr_4addr *header,
		   struct libipw_rx_stats *stats);

/* libipw_geo.c */
const struct libipw_geo *libipw_get_geo(struct libipw_device *ieee);
void libipw_set_geo(struct libipw_device *ieee, const struct libipw_geo *geo);

int libipw_is_valid_channel(struct libipw_device *ieee, u8 channel);
int libipw_channel_to_index(struct libipw_device *ieee, u8 channel);
u8 libipw_freq_to_channel(struct libipw_device *ieee, u32 freq);
u8 libipw_get_channel_flags(struct libipw_device *ieee, u8 channel);
const struct libipw_channel *libipw_get_channel(struct libipw_device *ieee,
						u8 channel);
u32 libipw_channel_to_freq(struct libipw_device *ieee, u8 channel);

/* libipw_wx.c */
int libipw_wx_get_scan(struct libipw_device *ieee, struct iw_request_info *info,
		       union iwreq_data *wrqu, char *key);
int libipw_wx_set_encode(struct libipw_device *ieee,
			 struct iw_request_info *info, union iwreq_data *wrqu,
			 char *key);
int libipw_wx_get_encode(struct libipw_device *ieee,
			 struct iw_request_info *info, union iwreq_data *wrqu,
			 char *key);
int libipw_wx_set_encodeext(struct libipw_device *ieee,
			    struct iw_request_info *info,
			    union iwreq_data *wrqu, char *extra);
int libipw_wx_get_encodeext(struct libipw_device *ieee,
			    struct iw_request_info *info,
			    union iwreq_data *wrqu, char *extra);

static inline void libipw_increment_scans(struct libipw_device *ieee)
{
	ieee->scans++;
}

static inline int libipw_get_scans(struct libipw_device *ieee)
{
	return ieee->scans;
}

#endif				/* LIBIPW_H */
