/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * This file is part of wl18xx
 *
 * Copyright (C) 2011 Texas Instruments. All rights reserved.
 */

#ifndef __WL18XX_ACX_H__
#define __WL18XX_ACX_H__

#include "../wlcore/wlcore.h"
#include "../wlcore/acx.h"

enum {
	ACX_NS_IPV6_FILTER		 = 0x0050,
	ACX_PEER_HT_OPERATION_MODE_CFG	 = 0x0051,
	ACX_CSUM_CONFIG			 = 0x0052,
	ACX_SIM_CONFIG			 = 0x0053,
	ACX_CLEAR_STATISTICS		 = 0x0054,
	ACX_AUTO_RX_STREAMING		 = 0x0055,
	ACX_PEER_CAP			 = 0x0056,
	ACX_INTERRUPT_NOTIFY		 = 0x0057,
	ACX_RX_BA_FILTER		 = 0x0058,
	ACX_AP_SLEEP_CFG                 = 0x0059,
	ACX_DYNAMIC_TRACES_CFG		 = 0x005A,
	ACX_TIME_SYNC_CFG		 = 0x005B,
};

/* numbers of bits the length field takes (add 1 for the actual number) */
#define WL18XX_HOST_IF_LEN_SIZE_FIELD 15

#define WL18XX_ACX_EVENTS_VECTOR	(WL1271_ACX_INTR_WATCHDOG	| \
					 WL1271_ACX_INTR_INIT_COMPLETE	| \
					 WL1271_ACX_INTR_EVENT_A	| \
					 WL1271_ACX_INTR_EVENT_B	| \
					 WL1271_ACX_INTR_CMD_COMPLETE	| \
					 WL1271_ACX_INTR_HW_AVAILABLE	| \
					 WL1271_ACX_INTR_DATA		| \
					 WL1271_ACX_SW_INTR_WATCHDOG)

#define WL18XX_INTR_MASK		(WL1271_ACX_INTR_WATCHDOG	| \
					 WL1271_ACX_INTR_EVENT_A	| \
					 WL1271_ACX_INTR_EVENT_B	| \
					 WL1271_ACX_INTR_HW_AVAILABLE	| \
					 WL1271_ACX_INTR_DATA		| \
					 WL1271_ACX_SW_INTR_WATCHDOG)

struct wl18xx_acx_host_config_bitmap {
	struct acx_header header;

	__le32 host_cfg_bitmap;

	__le32 host_sdio_block_size;

	/* extra mem blocks per frame in TX. */
	__le32 extra_mem_blocks;

	/*
	 * number of bits of the length field in the first TX word
	 * (up to 15 - for using the entire 16 bits).
	 */
	__le32 length_field_size;

} __packed;

enum {
	CHECKSUM_OFFLOAD_DISABLED = 0,
	CHECKSUM_OFFLOAD_ENABLED  = 1,
	CHECKSUM_OFFLOAD_FAKE_RX  = 2,
	CHECKSUM_OFFLOAD_INVALID  = 0xFF
};

struct wl18xx_acx_checksum_state {
	struct acx_header header;

	 /* enum acx_checksum_state */
	u8 checksum_state;
	u8 pad[3];
} __packed;


struct wl18xx_acx_error_stats {
	u32 error_frame_non_ctrl;
	u32 error_frame_ctrl;
	u32 error_frame_during_protection;
	u32 null_frame_tx_start;
	u32 null_frame_cts_start;
	u32 bar_retry;
	u32 num_frame_cts_nul_flid;
	u32 tx_abort_failure;
	u32 tx_resume_failure;
	u32 rx_cmplt_db_overflow_cnt;
	u32 elp_while_rx_exch;
	u32 elp_while_tx_exch;
	u32 elp_while_tx;
	u32 elp_while_nvic_pending;
	u32 rx_excessive_frame_len;
	u32 burst_mismatch;
	u32 tbc_exch_mismatch;
} __packed;

#define NUM_OF_RATES_INDEXES 30
struct wl18xx_acx_tx_stats {
	u32 tx_prepared_descs;
	u32 tx_cmplt;
	u32 tx_template_prepared;
	u32 tx_data_prepared;
	u32 tx_template_programmed;
	u32 tx_data_programmed;
	u32 tx_burst_programmed;
	u32 tx_starts;
	u32 tx_stop;
	u32 tx_start_templates;
	u32 tx_start_int_templates;
	u32 tx_start_fw_gen;
	u32 tx_start_data;
	u32 tx_start_null_frame;
	u32 tx_exch;
	u32 tx_retry_template;
	u32 tx_retry_data;
	u32 tx_retry_per_rate[NUM_OF_RATES_INDEXES];
	u32 tx_exch_pending;
	u32 tx_exch_expiry;
	u32 tx_done_template;
	u32 tx_done_data;
	u32 tx_done_int_template;
	u32 tx_cfe1;
	u32 tx_cfe2;
	u32 frag_called;
	u32 frag_mpdu_alloc_failed;
	u32 frag_init_called;
	u32 frag_in_process_called;
	u32 frag_tkip_called;
	u32 frag_key_not_found;
	u32 frag_need_fragmentation;
	u32 frag_bad_mblk_num;
	u32 frag_failed;
	u32 frag_cache_hit;
	u32 frag_cache_miss;
} __packed;

struct wl18xx_acx_rx_stats {
	u32 rx_beacon_early_term;
	u32 rx_out_of_mpdu_nodes;
	u32 rx_hdr_overflow;
	u32 rx_dropped_frame;
	u32 rx_done_stage;
	u32 rx_done;
	u32 rx_defrag;
	u32 rx_defrag_end;
	u32 rx_cmplt;
	u32 rx_pre_complt;
	u32 rx_cmplt_task;
	u32 rx_phy_hdr;
	u32 rx_timeout;
	u32 rx_rts_timeout;
	u32 rx_timeout_wa;
	u32 defrag_called;
	u32 defrag_init_called;
	u32 defrag_in_process_called;
	u32 defrag_tkip_called;
	u32 defrag_need_defrag;
	u32 defrag_decrypt_failed;
	u32 decrypt_key_not_found;
	u32 defrag_need_decrypt;
	u32 rx_tkip_replays;
	u32 rx_xfr;
} __packed;

struct wl18xx_acx_isr_stats {
	u32 irqs;
} __packed;

#define PWR_STAT_MAX_CONT_MISSED_BCNS_SPREAD 10

struct wl18xx_acx_pwr_stats {
	u32 missing_bcns_cnt;
	u32 rcvd_bcns_cnt;
	u32 connection_out_of_sync;
	u32 cont_miss_bcns_spread[PWR_STAT_MAX_CONT_MISSED_BCNS_SPREAD];
	u32 rcvd_awake_bcns_cnt;
	u32 sleep_time_count;
	u32 sleep_time_avg;
	u32 sleep_cycle_avg;
	u32 sleep_percent;
	u32 ap_sleep_active_conf;
	u32 ap_sleep_user_conf;
	u32 ap_sleep_counter;
} __packed;

struct wl18xx_acx_rx_filter_stats {
	u32 beacon_filter;
	u32 arp_filter;
	u32 mc_filter;
	u32 dup_filter;
	u32 data_filter;
	u32 ibss_filter;
	u32 protection_filter;
	u32 accum_arp_pend_requests;
	u32 max_arp_queue_dep;
} __packed;

struct wl18xx_acx_rx_rate_stats {
	u32 rx_frames_per_rates[50];
} __packed;

#define AGGR_STATS_TX_AGG	16
#define AGGR_STATS_RX_SIZE_LEN	16

struct wl18xx_acx_aggr_stats {
	u32 tx_agg_rate[AGGR_STATS_TX_AGG];
	u32 tx_agg_len[AGGR_STATS_TX_AGG];
	u32 rx_size[AGGR_STATS_RX_SIZE_LEN];
} __packed;

#define PIPE_STATS_HW_FIFO	11

struct wl18xx_acx_pipeline_stats {
	u32 hs_tx_stat_fifo_int;
	u32 hs_rx_stat_fifo_int;
	u32 enc_tx_stat_fifo_int;
	u32 enc_rx_stat_fifo_int;
	u32 rx_complete_stat_fifo_int;
	u32 pre_proc_swi;
	u32 post_proc_swi;
	u32 sec_frag_swi;
	u32 pre_to_defrag_swi;
	u32 defrag_to_rx_xfer_swi;
	u32 dec_packet_in;
	u32 dec_packet_in_fifo_full;
	u32 dec_packet_out;
	u16 pipeline_fifo_full[PIPE_STATS_HW_FIFO];
	u16 padding;
} __packed;

#define DIVERSITY_STATS_NUM_OF_ANT	2

struct wl18xx_acx_diversity_stats {
	u32 num_of_packets_per_ant[DIVERSITY_STATS_NUM_OF_ANT];
	u32 total_num_of_toggles;
} __packed;

struct wl18xx_acx_thermal_stats {
	u16 irq_thr_low;
	u16 irq_thr_high;
	u16 tx_stop;
	u16 tx_resume;
	u16 false_irq;
	u16 adc_source_unexpected;
} __packed;

#define WL18XX_NUM_OF_CALIBRATIONS_ERRORS 18
struct wl18xx_acx_calib_failure_stats {
	u16 fail_count[WL18XX_NUM_OF_CALIBRATIONS_ERRORS];
	u32 calib_count;
} __packed;

struct wl18xx_roaming_stats {
	s32 rssi_level;
} __packed;

struct wl18xx_dfs_stats {
	u32 num_of_radar_detections;
} __packed;

struct wl18xx_acx_statistics {
	struct acx_header header;

	struct wl18xx_acx_error_stats		error;
	struct wl18xx_acx_tx_stats		tx;
	struct wl18xx_acx_rx_stats		rx;
	struct wl18xx_acx_isr_stats		isr;
	struct wl18xx_acx_pwr_stats		pwr;
	struct wl18xx_acx_rx_filter_stats	rx_filter;
	struct wl18xx_acx_rx_rate_stats		rx_rate;
	struct wl18xx_acx_aggr_stats		aggr_size;
	struct wl18xx_acx_pipeline_stats	pipeline;
	struct wl18xx_acx_diversity_stats	diversity;
	struct wl18xx_acx_thermal_stats		thermal;
	struct wl18xx_acx_calib_failure_stats	calib;
	struct wl18xx_roaming_stats		roaming;
	struct wl18xx_dfs_stats			dfs;
} __packed;

struct wl18xx_acx_clear_statistics {
	struct acx_header header;
};

enum wlcore_bandwidth {
	WLCORE_BANDWIDTH_20MHZ,
	WLCORE_BANDWIDTH_40MHZ,
};

struct wlcore_peer_ht_operation_mode {
	struct acx_header header;

	u8 hlid;
	u8 bandwidth; /* enum wlcore_bandwidth */
	u8 padding[2];
};

/*
 * ACX_PEER_CAP
 * this struct is very similar to wl1271_acx_ht_capabilities, with the
 * addition of supported rates
 */
struct wlcore_acx_peer_cap {
	struct acx_header header;

	/* bitmask of capability bits supported by the peer */
	__le32 ht_capabilites;

	/* rates supported by the remote peer */
	__le32 supported_rates;

	/* Indicates to which link these capabilities apply. */
	u8 hlid;

	/*
	 * This the maximum A-MPDU length supported by the AP. The FW may not
	 * exceed this length when sending A-MPDUs
	 */
	u8 ampdu_max_length;

	/* This is the minimal spacing required when sending A-MPDUs to the AP*/
	u8 ampdu_min_spacing;

	u8 padding;
} __packed;

/*
 * ACX_INTERRUPT_NOTIFY
 * enable/disable fast-link/PSM notification from FW
 */
struct wl18xx_acx_interrupt_notify {
	struct acx_header header;
	u32 enable;
};

/*
 * ACX_RX_BA_FILTER
 * enable/disable RX BA filtering in FW
 */
struct wl18xx_acx_rx_ba_filter {
	struct acx_header header;
	u32 enable;
};

struct acx_ap_sleep_cfg {
	struct acx_header header;
	/* Duty Cycle (20-80% of staying Awake) for IDLE AP
	 * (0: disable)
	 */
	u8 idle_duty_cycle;
	/* Duty Cycle (20-80% of staying Awake) for Connected AP
	 * (0: disable)
	 */
	u8 connected_duty_cycle;
	/* Maximum stations that are allowed to be connected to AP
	 *  (255: no limit)
	 */
	u8 max_stations_thresh;
	/* Timeout till enabling the Sleep Mechanism after data stops
	 * [unit: 100 msec]
	 */
	u8 idle_conn_thresh;
} __packed;

/*
 * ACX_DYNAMIC_TRACES_CFG
 * configure the FW dynamic traces
 */
struct acx_dynamic_fw_traces_cfg {
	struct acx_header header;
	__le32 dynamic_fw_traces;
} __packed;

/*
 * ACX_TIME_SYNC_CFG
 * configure the time sync parameters
 */
struct acx_time_sync_cfg {
	struct acx_header header;
	u8 sync_mode;
	u8 zone_mac_addr[ETH_ALEN];
	u8 padding[1];
} __packed;

int wl18xx_acx_host_if_cfg_bitmap(struct wl1271 *wl, u32 host_cfg_bitmap,
				  u32 sdio_blk_size, u32 extra_mem_blks,
				  u32 len_field_size);
int wl18xx_acx_set_checksum_state(struct wl1271 *wl);
int wl18xx_acx_clear_statistics(struct wl1271 *wl);
int wl18xx_acx_peer_ht_operation_mode(struct wl1271 *wl, u8 hlid, bool wide);
int wl18xx_acx_set_peer_cap(struct wl1271 *wl,
			    struct ieee80211_sta_ht_cap *ht_cap,
			    bool allow_ht_operation,
			    u32 rate_set, u8 hlid);
int wl18xx_acx_interrupt_notify_config(struct wl1271 *wl, bool action);
int wl18xx_acx_rx_ba_filter(struct wl1271 *wl, bool action);
int wl18xx_acx_ap_sleep(struct wl1271 *wl);
int wl18xx_acx_dynamic_fw_traces(struct wl1271 *wl);
int wl18xx_acx_time_sync_cfg(struct wl1271 *wl);

#endif /* __WL18XX_ACX_H__ */
