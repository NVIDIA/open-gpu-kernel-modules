/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * This file is part of wl18xx
 *
 * Copyright (C) 2012 Texas Instruments. All rights reserved.
 */

#ifndef __WL18XX_SCAN_H__
#define __WL18XX_SCAN_H__

#include "../wlcore/wlcore.h"
#include "../wlcore/cmd.h"
#include "../wlcore/scan.h"

struct tracking_ch_params {
	struct conn_scan_ch_params channel;

	__le32 bssid_lsb;
	__le16 bssid_msb;

	u8 padding[2];
} __packed;

/* probe request rate */
enum
{
	WL18XX_SCAN_RATE_1	= 0,
	WL18XX_SCAN_RATE_5_5	= 1,
	WL18XX_SCAN_RATE_6	= 2,
};

#define WL18XX_MAX_CHANNELS_5GHZ 32

struct wl18xx_cmd_scan_params {
	struct wl1271_cmd_header header;

	u8 role_id;
	u8 scan_type;

	s8 rssi_threshold; /* for filtering (in dBm) */
	s8 snr_threshold;  /* for filtering (in dB) */

	u8 bss_type;	   /* for filtering */
	u8 ssid_from_list; /* use ssid from configured ssid list */
	u8 filter;	   /* forward only results with matching ssids */

	/*
	 * add broadcast ssid in addition to the configured ssids.
	 * the driver should add dummy entry for it (?).
	 */
	u8 add_broadcast;

	u8 urgency;
	u8 protect;	 /* ??? */
	u8 n_probe_reqs;    /* Number of probes requests per channel */
	u8 terminate_after; /* early terminate scan operation */

	u8 passive[SCAN_MAX_BANDS]; /* number of passive scan channels */
	u8 active[SCAN_MAX_BANDS];  /* number of active scan channels */
	u8 dfs;		   /* number of dfs channels in 5ghz */
	u8 passive_active; /* number of passive before active channels 2.4ghz */

	__le16 short_cycles_msec;
	__le16 long_cycles_msec;
	u8 short_cycles_count;
	u8 total_cycles; /* 0 - infinite */
	u8 padding[2];

	union {
		struct {
			struct conn_scan_ch_params channels_2[MAX_CHANNELS_2GHZ];
			struct conn_scan_ch_params channels_5[WL18XX_MAX_CHANNELS_5GHZ];
			struct conn_scan_ch_params channels_4[MAX_CHANNELS_4GHZ];
		};
		struct tracking_ch_params channels_tracking[WL1271_SCAN_MAX_CHANNELS];
	} ;

	u8 ssid[IEEE80211_MAX_SSID_LEN];
	u8 ssid_len;	 /* For SCAN_SSID_FILTER_SPECIFIC */
	u8 tag;
	u8 rate;

	/* send SCAN_REPORT_EVENT in periodic scans after each cycle
	* if number of results >= report_threshold. Must be 0 for
	* non periodic scans
	*/
	u8 report_threshold;

	/* Should periodic scan stop after a report event was created.
	* Must be 0 for non periodic scans.
	*/
	u8 terminate_on_report;

	u8 padding1[3];
} __packed;

struct wl18xx_cmd_scan_stop {
	struct wl1271_cmd_header header;

	u8 role_id;
	u8 scan_type;
	u8 padding[2];
} __packed;

int wl18xx_scan_start(struct wl1271 *wl, struct wl12xx_vif *wlvif,
		      struct cfg80211_scan_request *req);
int wl18xx_scan_stop(struct wl1271 *wl, struct wl12xx_vif *wlvif);
void wl18xx_scan_completed(struct wl1271 *wl, struct wl12xx_vif *wlvif);
int wl18xx_sched_scan_start(struct wl1271 *wl, struct wl12xx_vif *wlvif,
			    struct cfg80211_sched_scan_request *req,
			    struct ieee80211_scan_ies *ies);
void wl18xx_scan_sched_scan_stop(struct wl1271 *wl, struct wl12xx_vif *wlvif);
#endif
