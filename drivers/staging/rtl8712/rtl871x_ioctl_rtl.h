/* SPDX-License-Identifier: GPL-2.0 */
/******************************************************************************
 *
 * Copyright(c) 2007 - 2010 Realtek Corporation. All rights reserved.
 *
 * Modifications for inclusion into the Linux staging tree are
 * Copyright(c) 2010 Larry Finger. All rights reserved.
 *
 * Contact information:
 * WLAN FAE <wlanfae@realtek.com>
 * Larry Finger <Larry.Finger@lwfinger.net>
 *
 ******************************************************************************/
#ifndef _RTL871X_IOCTL_RTL_H
#define _RTL871X_IOCTL_RTL_H

#include "osdep_service.h"
#include "drv_types.h"

/*************** oid_rtl_seg_01_01 **************/
uint oid_rt_get_signal_quality_hdl(
			struct oid_par_priv *poid_par_priv);/*84*/
uint oid_rt_get_small_packet_crc_hdl(
			struct oid_par_priv *poid_par_priv);
uint oid_rt_get_middle_packet_crc_hdl(
			struct oid_par_priv *poid_par_priv);
uint oid_rt_get_large_packet_crc_hdl(
			struct oid_par_priv *poid_par_priv);
uint oid_rt_get_tx_retry_hdl(
			struct oid_par_priv *poid_par_priv);
uint oid_rt_get_rx_retry_hdl(
			struct oid_par_priv *poid_par_priv);
uint oid_rt_get_rx_total_packet_hdl(
			struct oid_par_priv *poid_par_priv);
uint oid_rt_get_tx_beacon_ok_hdl(
			struct oid_par_priv *poid_par_priv);
uint oid_rt_get_tx_beacon_err_hdl(
			struct oid_par_priv *poid_par_priv);
uint oid_rt_get_rx_icv_err_hdl(
			struct oid_par_priv *poid_par_priv);/*93*/
uint oid_rt_set_encryption_algorithm_hdl(
			struct oid_par_priv *poid_par_priv);
uint oid_rt_get_preamble_mode_hdl(
			struct oid_par_priv *poid_par_priv);
uint oid_rt_get_ap_ip_hdl(
			struct oid_par_priv *poid_par_priv);
uint oid_rt_get_channelplan_hdl(
			struct oid_par_priv *poid_par_priv);
uint oid_rt_set_channelplan_hdl(
			struct oid_par_priv *poid_par_priv);
uint oid_rt_set_preamble_mode_hdl(
			struct oid_par_priv *poid_par_priv);
uint oid_rt_set_bcn_intvl_hdl(
			struct oid_par_priv *poid_par_priv);
uint oid_rt_dedicate_probe_hdl(
			struct oid_par_priv *poid_par_priv);
uint oid_rt_get_total_tx_bytes_hdl(
			struct oid_par_priv *poid_par_priv);
uint oid_rt_get_total_rx_bytes_hdl(
			struct oid_par_priv *poid_par_priv);
uint oid_rt_current_tx_power_level_hdl(
			struct oid_par_priv *poid_par_priv);
uint oid_rt_get_enc_key_mismatch_count_hdl(
			struct oid_par_priv *poid_par_priv);
uint oid_rt_get_enc_key_match_count_hdl(
			struct oid_par_priv *poid_par_priv);
uint oid_rt_get_channel_hdl(
			struct oid_par_priv *poid_par_priv);
uint oid_rt_get_hardware_radio_off_hdl(
			struct oid_par_priv *poid_par_priv);
uint oid_rt_get_key_mismatch_hdl(
			struct oid_par_priv *poid_par_priv);
uint oid_rt_supported_wireless_mode_hdl(
			struct oid_par_priv *poid_par_priv);
uint oid_rt_get_channel_list_hdl(
			struct oid_par_priv *poid_par_priv);
uint oid_rt_get_scan_in_progress_hdl(
			struct oid_par_priv *poid_par_priv);
uint oid_rt_forced_data_rate_hdl(
			struct oid_par_priv *poid_par_priv);
uint oid_rt_wireless_mode_for_scan_list_hdl(
			struct oid_par_priv *poid_par_priv);
uint oid_rt_get_bss_wireless_mode_hdl(
			struct oid_par_priv *poid_par_priv);
uint oid_rt_scan_with_magic_packet_hdl(
			struct oid_par_priv *poid_par_priv);

/**************  oid_rtl_seg_01_03 section start **************/
uint oid_rt_ap_get_associated_station_list_hdl(
			struct oid_par_priv *poid_par_priv);
uint oid_rt_ap_switch_into_ap_mode_hdl(
			struct oid_par_priv *poid_par_priv);
uint oid_rt_ap_supported_hdl(
			struct oid_par_priv *poid_par_priv);
uint oid_rt_ap_set_passphrase_hdl(
			struct oid_par_priv *poid_par_priv);
/* oid_rtl_seg_01_11 */
uint oid_rt_pro_rf_write_registry_hdl(
			struct oid_par_priv *poid_par_priv);
uint oid_rt_pro_rf_read_registry_hdl(
			struct oid_par_priv *poid_par_priv);
/***************  oid_rtl_seg_03_00 section start **************/
uint oid_rt_get_connect_state_hdl(
			struct oid_par_priv *poid_par_priv);
uint oid_rt_set_default_key_id_hdl(
			struct oid_par_priv *poid_par_priv);

#endif

