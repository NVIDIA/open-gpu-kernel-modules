/* SPDX-License-Identifier: Apache-2.0 */
/*
 * WFx hardware interface definitions
 *
 * Copyright (c) 2018-2020, Silicon Laboratories Inc.
 */

#ifndef WFX_HIF_API_CMD_H
#define WFX_HIF_API_CMD_H

#include <linux/ieee80211.h>

#include "hif_api_general.h"

enum hif_requests_ids {
	HIF_REQ_ID_RESET                = 0x0a,
	HIF_REQ_ID_READ_MIB             = 0x05,
	HIF_REQ_ID_WRITE_MIB            = 0x06,
	HIF_REQ_ID_START_SCAN           = 0x07,
	HIF_REQ_ID_STOP_SCAN            = 0x08,
	HIF_REQ_ID_TX                   = 0x04,
	HIF_REQ_ID_JOIN                 = 0x0b,
	HIF_REQ_ID_SET_PM_MODE          = 0x10,
	HIF_REQ_ID_SET_BSS_PARAMS       = 0x11,
	HIF_REQ_ID_ADD_KEY              = 0x0c,
	HIF_REQ_ID_REMOVE_KEY           = 0x0d,
	HIF_REQ_ID_EDCA_QUEUE_PARAMS    = 0x13,
	HIF_REQ_ID_START                = 0x17,
	HIF_REQ_ID_BEACON_TRANSMIT      = 0x18,
	HIF_REQ_ID_UPDATE_IE            = 0x1b,
	HIF_REQ_ID_MAP_LINK             = 0x1c,
};

enum hif_confirmations_ids {
	HIF_CNF_ID_RESET                = 0x0a,
	HIF_CNF_ID_READ_MIB             = 0x05,
	HIF_CNF_ID_WRITE_MIB            = 0x06,
	HIF_CNF_ID_START_SCAN           = 0x07,
	HIF_CNF_ID_STOP_SCAN            = 0x08,
	HIF_CNF_ID_TX                   = 0x04,
	HIF_CNF_ID_MULTI_TRANSMIT       = 0x1e,
	HIF_CNF_ID_JOIN                 = 0x0b,
	HIF_CNF_ID_SET_PM_MODE          = 0x10,
	HIF_CNF_ID_SET_BSS_PARAMS       = 0x11,
	HIF_CNF_ID_ADD_KEY              = 0x0c,
	HIF_CNF_ID_REMOVE_KEY           = 0x0d,
	HIF_CNF_ID_EDCA_QUEUE_PARAMS    = 0x13,
	HIF_CNF_ID_START                = 0x17,
	HIF_CNF_ID_BEACON_TRANSMIT      = 0x18,
	HIF_CNF_ID_UPDATE_IE            = 0x1b,
	HIF_CNF_ID_MAP_LINK             = 0x1c,
};

enum hif_indications_ids {
	HIF_IND_ID_RX                   = 0x84,
	HIF_IND_ID_SCAN_CMPL            = 0x86,
	HIF_IND_ID_JOIN_COMPLETE        = 0x8f,
	HIF_IND_ID_SET_PM_MODE_CMPL     = 0x89,
	HIF_IND_ID_SUSPEND_RESUME_TX    = 0x8c,
	HIF_IND_ID_EVENT                = 0x85
};

struct hif_req_reset {
	u8     reset_stat:1;
	u8     reset_all_int:1;
	u8     reserved1:6;
	u8     reserved2[3];
} __packed;

struct hif_cnf_reset {
	__le32 status;
} __packed;

struct hif_req_read_mib {
	__le16 mib_id;
	__le16 reserved;
} __packed;

struct hif_cnf_read_mib {
	__le32 status;
	__le16 mib_id;
	__le16 length;
	u8     mib_data[];
} __packed;

struct hif_req_write_mib {
	__le16 mib_id;
	__le16 length;
	u8     mib_data[];
} __packed;

struct hif_cnf_write_mib {
	__le32 status;
} __packed;

struct hif_req_update_ie {
	u8     beacon:1;
	u8     probe_resp:1;
	u8     probe_req:1;
	u8     reserved1:5;
	u8     reserved2;
	__le16 num_ies;
	u8     ie[];
} __packed;

struct hif_cnf_update_ie {
	__le32 status;
} __packed;

struct hif_ssid_def {
	__le32 ssid_length;
	u8     ssid[IEEE80211_MAX_SSID_LEN];
} __packed;

#define HIF_API_MAX_NB_SSIDS                           2
#define HIF_API_MAX_NB_CHANNELS                       14

struct hif_req_start_scan_alt {
	u8     band;
	u8     maintain_current_bss:1;
	u8     periodic:1;
	u8     reserved1:6;
	u8     disallow_ps:1;
	u8     reserved2:1;
	u8     short_preamble:1;
	u8     reserved3:5;
	u8     max_transmit_rate;
	__le16 periodic_interval;
	u8     reserved4;
	s8     periodic_rssi_thr;
	u8     num_of_probe_requests;
	u8     probe_delay;
	u8     num_of_ssids;
	u8     num_of_channels;
	__le32 min_channel_time;
	__le32 max_channel_time;
	__le32 tx_power_level; // signed value
	struct hif_ssid_def ssid_def[HIF_API_MAX_NB_SSIDS];
	u8     channel_list[];
} __packed;

struct hif_cnf_start_scan {
	__le32 status;
} __packed;

struct hif_cnf_stop_scan {
	__le32 status;
} __packed;

enum hif_pm_mode_status {
	HIF_PM_MODE_ACTIVE                         = 0x0,
	HIF_PM_MODE_PS                             = 0x1,
	HIF_PM_MODE_UNDETERMINED                   = 0x2
};

struct hif_ind_scan_cmpl {
	__le32 status;
	u8     pm_mode;
	u8     num_channels_completed;
	__le16 reserved;
} __packed;

enum hif_queue_id {
	HIF_QUEUE_ID_BACKGROUND                    = 0x0,
	HIF_QUEUE_ID_BESTEFFORT                    = 0x1,
	HIF_QUEUE_ID_VIDEO                         = 0x2,
	HIF_QUEUE_ID_VOICE                         = 0x3
};

enum hif_frame_format {
	HIF_FRAME_FORMAT_NON_HT                    = 0x0,
	HIF_FRAME_FORMAT_MIXED_FORMAT_HT           = 0x1,
	HIF_FRAME_FORMAT_GF_HT_11N                 = 0x2
};

struct hif_req_tx {
	// packet_id is not interpreted by the device, so it is not necessary to
	// declare it little endian
	u32    packet_id;
	u8     max_tx_rate;
	u8     queue_id:2;
	u8     peer_sta_id:4;
	u8     reserved1:2;
	u8     more:1;
	u8     fc_offset:3;
	u8     after_dtim:1;
	u8     reserved2:3;
	u8     start_exp:1;
	u8     reserved3:3;
	u8     retry_policy_index:4;
	__le32 reserved4;
	__le32 expire_time;
	u8     frame_format:4;
	u8     fec_coding:1;
	u8     short_gi:1;
	u8     reserved5:1;
	u8     stbc:1;
	u8     reserved6;
	u8     aggregation:1;
	u8     reserved7:7;
	u8     reserved8;
	u8     frame[];
} __packed;

enum hif_qos_ackplcy {
	HIF_QOS_ACKPLCY_NORMAL                         = 0x0,
	HIF_QOS_ACKPLCY_TXNOACK                        = 0x1,
	HIF_QOS_ACKPLCY_NOEXPACK                       = 0x2,
	HIF_QOS_ACKPLCY_BLCKACK                        = 0x3
};

struct hif_cnf_tx {
	__le32 status;
	// packet_id is copied from struct hif_req_tx without been interpreted
	// by the device, so it is not necessary to declare it little endian
	u32    packet_id;
	u8     txed_rate;
	u8     ack_failures;
	u8     aggr:1;
	u8     requeue:1;
	u8     ack_policy:2;
	u8     txop_limit:1;
	u8     reserved1:3;
	u8     reserved2;
	__le32 media_delay;
	__le32 tx_queue_delay;
} __packed;

struct hif_cnf_multi_transmit {
	u8     num_tx_confs;
	u8     reserved[3];
	struct hif_cnf_tx tx_conf_payload[];
} __packed;

enum hif_ri_flags_encrypt {
	HIF_RI_FLAGS_UNENCRYPTED                   = 0x0,
	HIF_RI_FLAGS_WEP_ENCRYPTED                 = 0x1,
	HIF_RI_FLAGS_TKIP_ENCRYPTED                = 0x2,
	HIF_RI_FLAGS_AES_ENCRYPTED                 = 0x3,
	HIF_RI_FLAGS_WAPI_ENCRYPTED                = 0x4
};

struct hif_ind_rx {
	__le32 status;
	u8     channel_number;
	u8     reserved1;
	u8     rxed_rate;
	u8     rcpi_rssi;
	u8     encryp:3;
	u8     in_aggr:1;
	u8     first_aggr:1;
	u8     last_aggr:1;
	u8     defrag:1;
	u8     beacon:1;
	u8     tim:1;
	u8     bitmap:1;
	u8     match_ssid:1;
	u8     match_bssid:1;
	u8     more:1;
	u8     reserved2:1;
	u8     ht:1;
	u8     stbc:1;
	u8     match_uc_addr:1;
	u8     match_mc_addr:1;
	u8     match_bc_addr:1;
	u8     key_type:1;
	u8     key_index:4;
	u8     reserved3:1;
	u8     peer_sta_id:4;
	u8     reserved4:2;
	u8     reserved5:1;
	u8     frame[];
} __packed;

struct hif_req_edca_queue_params {
	u8     queue_id;
	u8     reserved1;
	u8     aifsn;
	u8     reserved2;
	__le16 cw_min;
	__le16 cw_max;
	__le16 tx_op_limit;
	__le16 allowed_medium_time;
	__le32 reserved3;
} __packed;

struct hif_cnf_edca_queue_params {
	__le32 status;
} __packed;

struct hif_req_join {
	u8     infrastructure_bss_mode:1;
	u8     reserved1:7;
	u8     band;
	u8     channel_number;
	u8     reserved2;
	u8     bssid[ETH_ALEN];
	__le16 atim_window;
	u8     short_preamble:1;
	u8     reserved3:7;
	u8     probe_for_join;
	u8     reserved4;
	u8     reserved5:2;
	u8     force_no_beacon:1;
	u8     force_with_ind:1;
	u8     reserved6:4;
	__le32 ssid_length;
	u8     ssid[IEEE80211_MAX_SSID_LEN];
	__le32 beacon_interval;
	__le32 basic_rate_set;
} __packed;

struct hif_cnf_join {
	__le32 status;
} __packed;

struct hif_ind_join_complete {
	__le32 status;
} __packed;

struct hif_req_set_bss_params {
	u8     lost_count_only:1;
	u8     reserved:7;
	u8     beacon_lost_count;
	__le16 aid;
	__le32 operational_rate_set;
} __packed;

struct hif_cnf_set_bss_params {
	__le32 status;
} __packed;

struct hif_req_set_pm_mode {
	u8     enter_psm:1;
	u8     reserved:6;
	u8     fast_psm:1;
	u8     fast_psm_idle_period;
	u8     ap_psm_change_period;
	u8     min_auto_ps_poll_period;
} __packed;

struct hif_cnf_set_pm_mode {
	__le32 status;
} __packed;

struct hif_ind_set_pm_mode_cmpl {
	__le32 status;
	u8     pm_mode;
	u8     reserved[3];
} __packed;

struct hif_req_start {
	u8     mode;
	u8     band;
	u8     channel_number;
	u8     reserved1;
	__le32 reserved2;
	__le32 beacon_interval;
	u8     dtim_period;
	u8     short_preamble:1;
	u8     reserved3:7;
	u8     reserved4;
	u8     ssid_length;
	u8     ssid[IEEE80211_MAX_SSID_LEN];
	__le32 basic_rate_set;
} __packed;

struct hif_cnf_start {
	__le32 status;
} __packed;

struct hif_req_beacon_transmit {
	u8     enable_beaconing;
	u8     reserved[3];
} __packed;

struct hif_cnf_beacon_transmit {
	__le32 status;
} __packed;

#define HIF_LINK_ID_MAX            14
#define HIF_LINK_ID_NOT_ASSOCIATED (HIF_LINK_ID_MAX + 1)

struct hif_req_map_link {
	u8     mac_addr[ETH_ALEN];
	u8     unmap:1;
	u8     mfpc:1;
	u8     reserved:6;
	u8     peer_sta_id;
} __packed;

struct hif_cnf_map_link {
	__le32 status;
} __packed;

struct hif_ind_suspend_resume_tx {
	u8     resume:1;
	u8     reserved1:2;
	u8     bc_mc_only:1;
	u8     reserved2:4;
	u8     reserved3;
	__le16 peer_sta_set;
} __packed;


#define MAX_KEY_ENTRIES         24
#define HIF_API_WEP_KEY_DATA_SIZE                       16
#define HIF_API_TKIP_KEY_DATA_SIZE                      16
#define HIF_API_RX_MIC_KEY_SIZE                         8
#define HIF_API_TX_MIC_KEY_SIZE                         8
#define HIF_API_AES_KEY_DATA_SIZE                       16
#define HIF_API_WAPI_KEY_DATA_SIZE                      16
#define HIF_API_MIC_KEY_DATA_SIZE                       16
#define HIF_API_IGTK_KEY_DATA_SIZE                      16
#define HIF_API_RX_SEQUENCE_COUNTER_SIZE                8
#define HIF_API_IPN_SIZE                                8

enum hif_key_type {
	HIF_KEY_TYPE_WEP_DEFAULT                   = 0x0,
	HIF_KEY_TYPE_WEP_PAIRWISE                  = 0x1,
	HIF_KEY_TYPE_TKIP_GROUP                    = 0x2,
	HIF_KEY_TYPE_TKIP_PAIRWISE                 = 0x3,
	HIF_KEY_TYPE_AES_GROUP                     = 0x4,
	HIF_KEY_TYPE_AES_PAIRWISE                  = 0x5,
	HIF_KEY_TYPE_WAPI_GROUP                    = 0x6,
	HIF_KEY_TYPE_WAPI_PAIRWISE                 = 0x7,
	HIF_KEY_TYPE_IGTK_GROUP                    = 0x8,
	HIF_KEY_TYPE_NONE                          = 0x9
};

struct hif_wep_pairwise_key {
	u8     peer_address[ETH_ALEN];
	u8     reserved;
	u8     key_length;
	u8     key_data[HIF_API_WEP_KEY_DATA_SIZE];
} __packed;

struct hif_wep_group_key {
	u8     key_id;
	u8     key_length;
	u8     reserved[2];
	u8     key_data[HIF_API_WEP_KEY_DATA_SIZE];
} __packed;

struct hif_tkip_pairwise_key {
	u8     peer_address[ETH_ALEN];
	u8     reserved[2];
	u8     tkip_key_data[HIF_API_TKIP_KEY_DATA_SIZE];
	u8     rx_mic_key[HIF_API_RX_MIC_KEY_SIZE];
	u8     tx_mic_key[HIF_API_TX_MIC_KEY_SIZE];
} __packed;

struct hif_tkip_group_key {
	u8     tkip_key_data[HIF_API_TKIP_KEY_DATA_SIZE];
	u8     rx_mic_key[HIF_API_RX_MIC_KEY_SIZE];
	u8     key_id;
	u8     reserved[3];
	u8     rx_sequence_counter[HIF_API_RX_SEQUENCE_COUNTER_SIZE];
} __packed;

struct hif_aes_pairwise_key {
	u8     peer_address[ETH_ALEN];
	u8     reserved[2];
	u8     aes_key_data[HIF_API_AES_KEY_DATA_SIZE];
} __packed;

struct hif_aes_group_key {
	u8     aes_key_data[HIF_API_AES_KEY_DATA_SIZE];
	u8     key_id;
	u8     reserved[3];
	u8     rx_sequence_counter[HIF_API_RX_SEQUENCE_COUNTER_SIZE];
} __packed;

struct hif_wapi_pairwise_key {
	u8     peer_address[ETH_ALEN];
	u8     key_id;
	u8     reserved;
	u8     wapi_key_data[HIF_API_WAPI_KEY_DATA_SIZE];
	u8     mic_key_data[HIF_API_MIC_KEY_DATA_SIZE];
} __packed;

struct hif_wapi_group_key {
	u8     wapi_key_data[HIF_API_WAPI_KEY_DATA_SIZE];
	u8     mic_key_data[HIF_API_MIC_KEY_DATA_SIZE];
	u8     key_id;
	u8     reserved[3];
} __packed;

struct hif_igtk_group_key {
	u8     igtk_key_data[HIF_API_IGTK_KEY_DATA_SIZE];
	u8     key_id;
	u8     reserved[3];
	u8     ipn[HIF_API_IPN_SIZE];
} __packed;

struct hif_req_add_key {
	u8     type;
	u8     entry_index;
	u8     int_id:2;
	u8     reserved1:6;
	u8     reserved2;
	union {
		struct hif_wep_pairwise_key  wep_pairwise_key;
		struct hif_wep_group_key     wep_group_key;
		struct hif_tkip_pairwise_key tkip_pairwise_key;
		struct hif_tkip_group_key    tkip_group_key;
		struct hif_aes_pairwise_key  aes_pairwise_key;
		struct hif_aes_group_key     aes_group_key;
		struct hif_wapi_pairwise_key wapi_pairwise_key;
		struct hif_wapi_group_key    wapi_group_key;
		struct hif_igtk_group_key    igtk_group_key;
	} key;
} __packed;

struct hif_cnf_add_key {
	__le32 status;
} __packed;

struct hif_req_remove_key {
	u8     entry_index;
	u8     reserved[3];
} __packed;

struct hif_cnf_remove_key {
	__le32 status;
} __packed;

enum hif_event_ind {
	HIF_EVENT_IND_BSSLOST                      = 0x1,
	HIF_EVENT_IND_BSSREGAINED                  = 0x2,
	HIF_EVENT_IND_RCPI_RSSI                    = 0x3,
	HIF_EVENT_IND_PS_MODE_ERROR                = 0x4,
	HIF_EVENT_IND_INACTIVITY                   = 0x5
};

enum hif_ps_mode_error {
	HIF_PS_ERROR_NO_ERROR                      = 0,
	HIF_PS_ERROR_AP_NOT_RESP_TO_POLL           = 1,
	HIF_PS_ERROR_AP_NOT_RESP_TO_UAPSD_TRIGGER  = 2,
	HIF_PS_ERROR_AP_SENT_UNICAST_IN_DOZE       = 3,
	HIF_PS_ERROR_AP_NO_DATA_AFTER_TIM          = 4
};

struct hif_ind_event {
	__le32 event_id;
	union {
		u8     rcpi_rssi;
		__le32 ps_mode_error;
		__le32 peer_sta_set;
	} event_data;
} __packed;

#endif
