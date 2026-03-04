/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef _DPCD_H_
#define _DPCD_H_

#define NV_DPCD_CAP_LEGACY_BASE                                  (0x00000000)

#define NV_DPCD_REV                                              (0x00000000) /* R-XUR */
#define NV_DPCD_REV_MAJOR                                                7:4  /* R-XUF */
#define NV_DPCD_REV_MAJOR_1                                      (0x00000001) /* R-XUV */
#define NV_DPCD_REV_MINOR                                                3:0  /* R-XUF */
#define NV_DPCD_REV_MINOR_0                                      (0x00000000) /* R-XUV */
#define NV_DPCD_REV_MINOR_1                                      (0x00000001) /* R-XUV */
#define NV_DPCD_REV_MINOR_2                                      (0x00000002) /* R-XUV */
#define NV_DPCD_REV_MINOR_4                                      (0x00000004) /* R-XUV */

#define NV_DPCD_MAX_LINK_BANDWIDTH                               (0x00000001) /* R-XUR */
#define NV_DPCD_MAX_LINK_BANDWIDTH_VAL                                   4:0  /* R-XUF */
#define NV_DPCD_MAX_LINK_BANDWIDTH_VAL_1_62_GBPS                 (0x00000006) /* R-XUV */
#define NV_DPCD_MAX_LINK_BANDWIDTH_VAL_2_70_GBPS                 (0x0000000a) /* R-XUV */
#define NV_DPCD_MAX_LINK_BANDWIDTH_VAL_5_40_GBPS                 (0x00000014) /* R-XUV */

#define NV_DPCD_MAX_LANE_COUNT                                   (0x00000002) /* R-XUR */
#define NV_DPCD_MAX_LANE_COUNT_LANE                                      4:0  /* R-XUF */
#define NV_DPCD_MAX_LANE_COUNT_LANE_1                            (0x00000001) /* R-XUV */
#define NV_DPCD_MAX_LANE_COUNT_LANE_2                            (0x00000002) /* R-XUV */
#define NV_DPCD_MAX_LANE_COUNT_LANE_4                            (0x00000004) /* R-XUV */
#define NV_DPCD_MAX_LANE_COUNT_LANE_8                            (0x00000008) /* R-XUV */
#define NV_DPCD_MAX_LANE_COUNT_POST_LT_ADJ_REQ_SUPPORT                   5:5  /* R-XUF */
#define NV_DPCD_MAX_LANE_COUNT_POST_LT_ADJ_REQ_SUPPORT_NO        (0x00000000) /* R-XUV */
#define NV_DPCD_MAX_LANE_COUNT_POST_LT_ADJ_REQ_SUPPORT_YES       (0x00000001) /* R-XUV */
#define NV_DPCD_MAX_LANE_COUNT_ENHANCED_FRAMING                          7:7  /* R-XUF */
#define NV_DPCD_MAX_LANE_COUNT_ENHANCED_FRAMING_NO               (0x00000000) /* R-XUV */
#define NV_DPCD_MAX_LANE_COUNT_ENHANCED_FRAMING_YES              (0x00000001) /* R-XUV */
#define NV_DPCD_MAX_LANE_COUNT_TPS3_SUPPORTED                            6:6  /* R-XUF */
#define NV_DPCD_MAX_LANE_COUNT_TPS3_SUPPORTED_NO                 (0x00000000) /* R-XUV */
#define NV_DPCD_MAX_LANE_COUNT_TPS3_SUPPORTED_YES                (0x00000001) /* R-XUV */

#define NV_DPCD_MAX_DOWNSPREAD                                   (0x00000003) /* R-XUR */
#define NV_DPCD_MAX_DOWNSPREAD_VAL                                       0:0  /* R-XUF */
#define NV_DPCD_MAX_DOWNSPREAD_VAL_NONE                          (0x00000000) /* R-XUV */
#define NV_DPCD_MAX_DOWNSPREAD_VAL_0_5_PCT                       (0x00000001) /* R-XUV */
#define NV_DPCD_MAX_DOWNSPREAD_NO_AUX_HANDSHAKE_LT                       6:6  /* R-XUF */
#define NV_DPCD_MAX_DOWNSPREAD_NO_AUX_HANDSHAKE_LT_FALSE         (0x00000000) /* R-XUV */
#define NV_DPCD_MAX_DOWNSPREAD_NO_AUX_HANDSHAKE_LT_TRUE          (0x00000001) /* R-XUV */

// NORP = Number of Receiver Ports = Value + 1
#define NV_DPCD_NORP                                             (0x00000004) /* R-XUR */
#define NV_DPCD_NORP_VAL                                                 0:0  /* R-XUF */
#define NV_DPCD_NORP_VAL_ONE                                     (0x00000000) /* R-XUV */
#define NV_DPCD_NORP_VAL_TWO                                     (0x00000001) /* R-XUV */
#define NV_DPCD_NORP_VAL_SST_MAX                                 (0x00000001) /* R-XUV */
#define NV_DPCD_NORP_DP_PWR_CAP_5V                                       5:5  /* R-XUF */
#define NV_DPCD_NORP_DP_PWR_CAP_12V                                      6:6  /* R-XUF */
#define NV_DPCD_NORP_DP_PWR_CAP_18V                                      7:7  /* R-XUF */

#define NV_DPCD_DOWNSTREAMPORT                                   (0x00000005) /* R-XUR */
#define NV_DPCD_DOWNSTREAMPORT_PRESENT                                   0:0  /* R-XUF */
#define NV_DPCD_DOWNSTREAMPORT_PRESENT_NO                        (0x00000000) /* R-XUV */
#define NV_DPCD_DOWNSTREAMPORT_PRESENT_YES                       (0x00000001) /* R-XUV */
#define NV_DPCD_DOWNSTREAMPORT_TYPE                                      2:1  /* R-XUF */
#define NV_DPCD_DOWNSTREAMPORT_TYPE_DISPLAYPORT                  (0x00000000) /* R-XUV */
#define NV_DPCD_DOWNSTREAMPORT_TYPE_ANALOG                       (0x00000001) /* R-XUV */
#define NV_DPCD_DOWNSTREAMPORT_TYPE_HDMI_DVI                     (0x00000002) /* R-XUV */
#define NV_DPCD_DOWNSTREAMPORT_TYPE_OTHERS                       (0x00000003) /* R-XUV */
#define NV_DPCD_DOWNSTREAMPORT_FORMAT_CONVERSION                         3:3  /* R-XUF */
#define NV_DPCD_DOWNSTREAMPORT_FORMAT_CONVERSION_NO              (0x00000000) /* R-XUV */
#define NV_DPCD_DOWNSTREAMPORT_FORMAT_CONVERSION_YES             (0x00000001) /* R-XUV */
#define NV_DPCD_DOWNSTREAMPORT_DETAILED_CAP_INFO_AVAILABLE               4:4  /* R-XUF */
#define NV_DPCD_DOWNSTREAMPORT_DETAILED_CAP_INFO_AVAILABLE_NO    (0x00000000) /* R-XUV */
#define NV_DPCD_DOWNSTREAMPORT_DETAILED_CAP_INFO_AVAILABLE_YES   (0x00000001) /* R-XUV */

#define NV_DPCD_MAIN_LINK_CHANNEL_CODING                         (0x00000006) /* R-XUR */
#define NV_DPCD_MAIN_LINK_CHANNEL_CODING_ANSI_8B_10B                     0:0  /* R-XUF */
#define NV_DPCD_MAIN_LINK_CHANNEL_CODING_ANSI_8B_10B_NO          (0x00000000) /* R-XUV */
#define NV_DPCD_MAIN_LINK_CHANNEL_CODING_ANSI_8B_10B_YES         (0x00000001) /* R-XUV */
#define NV_DPCD_MAIN_LINK_CHANNEL_CODING_ANSI_128B_132B                  1:1  /* R-XUF */
#define NV_DPCD_MAIN_LINK_CHANNEL_CODING_ANSI_128B_132B_NO       (0x00000000) /* R-XUV */
#define NV_DPCD_MAIN_LINK_CHANNEL_CODING_ANSI_128B_132B_YES      (0x00000001) /* R-XUV */

#define NV_DPCD_DOWN_STREAM_PORT                                 (0x00000007) /* R-XUR */
#define NV_DPCD_DOWN_STREAM_PORT_COUNT                                   3:0  /* R-XUF */
#define NV_DPCD_DOWN_STREAM_PORT_MSA_TIMING_PAR_IGNORED                  6:6  /* R-XUF */
#define NV_DPCD_DOWN_STREAM_PORT_MSA_TIMING_PAR_IGNORED_NO       (0x00000000) /* R-XUV */
#define NV_DPCD_DOWN_STREAM_PORT_MSA_TIMING_PAR_IGNORED_YES      (0x00000001) /* R-XUV */
#define NV_DPCD_DOWN_STREAM_PORT_OUI_SUPPORT                             7:7  /* R-XUF */
#define NV_DPCD_DOWN_STREAM_PORT_OUI_SUPPORT_NO                  (0x00000000) /* R-XUV */
#define NV_DPCD_DOWN_STREAM_PORT_OUI_SUPPORT_YES                 (0x00000001) /* R-XUV */

#define NV_DPCD_RECEIVE_PORT0_CAP_0                              (0x00000008) /* R-XUR */
#define NV_DPCD_RECEIVE_PORT1_CAP_0                              (0x0000000A) /* R-XUR */
#define NV_DPCD_RECEIVE_PORTX_CAP_0_LOCAL_EDID                           1:1  /* R-XUF */
#define NV_DPCD_RECEIVE_PORTX_CAP_0_LOCAL_EDID_NO                (0x00000000) /* R-XUV */
#define NV_DPCD_RECEIVE_PORTX_CAP_0_LOCAL_EDID_YES               (0x00000001) /* R-XUV */
#define NV_DPCD_RECEIVE_PORTX_CAP_0_ASSO_TO_PRECEDING_PORT               2:2  /* R-XUF */
#define NV_DPCD_RECEIVE_PORTX_CAP_0_ASSO_TO_PRECEDING_PORT_NO    (0x00000000) /* R-XUV */
#define NV_DPCD_RECEIVE_PORTX_CAP_0_ASSO_TO_PRECEDING_PORT_YES   (0x00000001) /* R-XUV */

#define NV_DPCD_RECEIVE_PORT0_CAP_1                              (0x00000009) /* R-XUR */
#define NV_DPCD_RECEIVE_PORT1_CAP_1                              (0x0000000B) /* R-XUR */
#define NV_DPCD_RECEIVE_PORTX_CAP_1_BUFFER_SIZE                          7:0  /* R-XUF */

#define NV_DPCD_I2C_CTRL_CAP                                     (0x0000000C) /* R-XUR */
#define NV_DPCD_I2C_CTRL_CAP_SPEED                                       7:0  /* R-XUF */
#define NV_DPCD_I2C_CTRL_CAP_SPEED_1K                            (0x00000001) /* R-XUV */
#define NV_DPCD_I2C_CTRL_CAP_SPEED_5K                            (0x00000002) /* R-XUV */
#define NV_DPCD_I2C_CTRL_CAP_SPEED_10K                           (0x00000004) /* R-XUV */
#define NV_DPCD_I2C_CTRL_CAP_SPEED_100K                          (0x00000008) /* R-XUV */
#define NV_DPCD_I2C_CTRL_CAP_SPEED_400K                          (0x00000010) /* R-XUV */
#define NV_DPCD_I2C_CTRL_CAP_SPEED_1M                            (0x00000020) /* R-XUV */

#define NV_DPCD_EDP_CONFIG_CAP                                   (0x0000000D) /* R-XUR */
#define NV_DPCD_EDP_CONFIG_CAP_ALTERNATE_SCRAMBLER_RESET                 0:0  /* R-XUF */
#define NV_DPCD_EDP_CONFIG_CAP_ALTERNATE_SCRAMBLER_RESET_NO      (0x00000000) /* R-XUV */
#define NV_DPCD_EDP_CONFIG_CAP_ALTERNATE_SCRAMBLER_RESET_YES     (0x00000001) /* R-XUV */
#define NV_DPCD_EDP_CONFIG_CAP_FRAMING_CHANGE                            1:1  /* R-XUF */
#define NV_DPCD_EDP_CONFIG_CAP_FRAMING_CHANGE_NO                 (0x00000000) /* R-XUV */
#define NV_DPCD_EDP_CONFIG_CAP_FRAMING_CHANGE_YES                (0x00000001) /* R-XUV */
#define NV_DPCD_EDP_CONFIG_CAP_INVERTED_TRAINING_BIT                     2:2  /* R-XUF */
#define NV_DPCD_EDP_CONFIG_CAP_INVERTED_TRAINING_BIT_NO          (0x00000000) /* R-XUV */
#define NV_DPCD_EDP_CONFIG_CAP_INVERTED_TRAINING_BIT_YES         (0x00000001) /* R-XUV */
#define NV_DPCD_EDP_CONFIG_CAP_DISPLAY_CONTROL_CAPABLE                   3:3  /* R-XUF */
#define NV_DPCD_EDP_CONFIG_CAP_DISPLAY_CONTROL_CAPABLE_NO        (0x00000000) /* R-XUV */
#define NV_DPCD_EDP_CONFIG_CAP_DISPLAY_CONTROL_CAPABLE_YES       (0x00000001) /* R-XUV */

#define NV_DPCD_TRAINING_AUX_RD_INTERVAL                         (0x0000000E) /* R-XUR */
#define NV_DPCD_TRAINING_AUX_RD_INTERVAL_VAL                             6:0  /* R-XUF */
#define NV_DPCD_TRAINING_AUX_RD_INTERVAL_VAL_DEFAULT             (0x00000000) /* R-XUV */
#define NV_DPCD_TRAINING_AUX_RD_INTERVAL_VAL_4MS                 (0x00000001) /* R-XUV */
#define NV_DPCD_TRAINING_AUX_RD_INTERVAL_VAL_8MS                 (0x00000002) /* R-XUV */
#define NV_DPCD_TRAINING_AUX_RD_INTERVAL_VAL_12MS                (0x00000003) /* R-XUV */
#define NV_DPCD_TRAINING_AUX_RD_INTERVAL_VAL_16MS                (0x00000004) /* R-XUV */

#define NV_DPCD_ADAPTER_CAP                                      (0x0000000F) /* R-XUR */
#define NV_DPCD_ADAPTER_CAP_FORCE_LOAD_SENSE                             0:0  /* R-XUF */
#define NV_DPCD_ADAPTER_CAP_FORCE_LOAD_SENSE_NO                  (0x00000000) /* R-XUV */
#define NV_DPCD_ADAPTER_CAP_FORCE_LOAD_SENSE_YES                 (0x00000001) /* R-XUV */
#define NV_DPCD_ADAPTER_CAP_ALT_I2C_PATTERN                              1:1  /* R-XUF */
#define NV_DPCD_ADAPTER_CAP_ALT_I2C_PATTERN_NO                   (0x00000000) /* R-XUV */
#define NV_DPCD_ADAPTER_CAP_ALT_I2C_PATTERN_YES                  (0x00000001) /* R-XUV */

#define NV_DPCD_SUPPORTED_LINK_RATES(i)                          (0x00000010+(i)*2) /* R--2A */
#define NV_DPCD_SUPPORTED_LINK_RATES__SIZE                       (0x00000008) /* R---S */

// 00010h-0001Fh: RESERVED. Reads all 0s

#define NV_DPCD_SINK_VIDEO_FALLBACK_FORMATS                      (0x00000020) /* R-XUR */
#define NV_DPCD_SINK_VIDEO_FALLBACK_FORMATS_1024_768                     0:0  /* R-XUF */
#define NV_DPCD_SINK_VIDEO_FALLBACK_FORMATS_1024_768_NO          (0X00000000) /* R-XUF */
#define NV_DPCD_SINK_VIDEO_FALLBACK_FORMATS_1024_768_YES         (0X00000001) /* R-XUF */
#define NV_DPCD_SINK_VIDEO_FALLBACK_FORMATS_1280_720                     1:1  /* R-XUV */
#define NV_DPCD_SINK_VIDEO_FALLBACK_FORMATS_1280_720_NO          (0X00000000) /* R-XUF */
#define NV_DPCD_SINK_VIDEO_FALLBACK_FORMATS_1280_720_YES         (0X00000001) /* R-XUF */
#define NV_DPCD_SINK_VIDEO_FALLBACK_FORMATS_1920_1080                    2:2  /* R-XUV */
#define NV_DPCD_SINK_VIDEO_FALLBACK_FORMATS_1920_1080_NO         (0X00000000) /* R-XUF */
#define NV_DPCD_SINK_VIDEO_FALLBACK_FORMATS_1920_1080_YES        (0X00000001) /* R-XUF */

#define NV_DPCD_MSTM                                             (0x00000021) /* R-XUR */
#define NV_DPCD_MSTM_CAP                                                 0:0  /* R-XUF */
#define NV_DPCD_MSTM_CAP_NO                                      (0x00000000) /* R-XUV */
#define NV_DPCD_MSTM_CAP_YES                                     (0x00000001) /* R-XUV */

#define NV_DPCD_NUMBER_OF_AUDIO_ENDPOINTS                        (0x00000022) /* R-XUR */
#define NV_DPCD_NUMBER_OF_AUDIO_ENDPOINTS_VALUE                          7:0  /* R-XUF */

#define NV_DPCD_AV_SYNC_DATA_BLOCK_AV_GRANULARITY                           (0x00000023) /* R-XUR */
#define NV_DPCD_AV_SYNC_DATA_BLOCK_AV_GRANULARITY_AG_FACTOR                         3:0  /* R-XUF */
#define NV_DPCD_AV_SYNC_DATA_BLOCK_AV_GRANULARITY_AG_FACTOR_3MS             (0x00000000) /* R-XUV */
#define NV_DPCD_AV_SYNC_DATA_BLOCK_AV_GRANULARITY_AG_FACTOR_2MS             (0x00000001) /* R-XUV */
#define NV_DPCD_AV_SYNC_DATA_BLOCK_AV_GRANULARITY_AG_FACTOR_1MS             (0x00000002) /* R-XUV */
#define NV_DPCD_AV_SYNC_DATA_BLOCK_AV_GRANULARITY_AG_FACTOR_500US           (0x00000003) /* R-XUV */
#define NV_DPCD_AV_SYNC_DATA_BLOCK_AV_GRANULARITY_AG_FACTOR_200US           (0x00000004) /* R-XUV */
#define NV_DPCD_AV_SYNC_DATA_BLOCK_AV_GRANULARITY_AG_FACTOR_100US           (0x00000005) /* R-XUV */
#define NV_DPCD_AV_SYNC_DATA_BLOCK_AV_GRANULARITY_AG_FACTOR_10US            (0x00000006) /* R-XUV */
#define NV_DPCD_AV_SYNC_DATA_BLOCK_AV_GRANULARITY_AG_FACTOR_1US             (0x00000007) /* R-XUV */
#define NV_DPCD_AV_SYNC_DATA_BLOCK_AV_GRANULARITY_AG_FACTOR_DEFAULT         NV_DPCD_AV_SYNC_DATA_BLOCK_AV_GRANULARITY_AG_FACTOR_2MS
#define NV_DPCD_AV_SYNC_DATA_BLOCK_AV_GRANULARITY_VG_FACTOR                         7:4  /* R-XUF */
#define NV_DPCD_AV_SYNC_DATA_BLOCK_AV_GRANULARITY_VG_FACTOR_3MS             (0x00000000) /* R-XUV */
#define NV_DPCD_AV_SYNC_DATA_BLOCK_AV_GRANULARITY_VG_FACTOR_2MS             (0x00000001) /* R-XUV */
#define NV_DPCD_AV_SYNC_DATA_BLOCK_AV_GRANULARITY_VG_FACTOR_1MS             (0x00000002) /* R-XUV */
#define NV_DPCD_AV_SYNC_DATA_BLOCK_AV_GRANULARITY_VG_FACTOR_500US           (0x00000003) /* R-XUV */
#define NV_DPCD_AV_SYNC_DATA_BLOCK_AV_GRANULARITY_VG_FACTOR_200US           (0x00000004) /* R-XUV */
#define NV_DPCD_AV_SYNC_DATA_BLOCK_AV_GRANULARITY_VG_FACTOR_100US           (0x00000005) /* R-XUV */
#define NV_DPCD_AV_SYNC_DATA_BLOCK_AV_GRANULARITY_VG_FACTOR_DEFAULT         NV_DPCD_AV_SYNC_DATA_BLOCK_AV_GRANULARITY_VG_FACTOR_2MS

#define NV_DPCD_AV_SYNC_DATA_BLOCK_AUD_DEC_LAT_0                 (0x00000024) /* R-XUR */
#define NV_DPCD_AV_SYNC_DATA_BLOCK_AUD_DEC_LAT_1                 (0x00000025) /* R-XUR */
#define NV_DPCD_AV_SYNC_DATA_BLOCK_AUD_PP_LAT_0                  (0x00000026) /* R-XUR */
#define NV_DPCD_AV_SYNC_DATA_BLOCK_AUD_PP_LAT_1                  (0x00000027) /* R-XUR */
#define NV_DPCD_AV_SYNC_DATA_BLOCK_VID_INTER_LAT                 (0x00000028) /* R-XUR */
#define NV_DPCD_AV_SYNC_DATA_BLOCK_VID_PROG_LAT                  (0x00000029) /* R-XUR */
#define NV_DPCD_AV_SYNC_DATA_BLOCK_REP_LAT                       (0x0000002A) /* R-XUR */
#define NV_DPCD_AV_SYNC_DATA_BLOCK_AUD_DEL_INS_0                 (0x0000002B) /* R-XUR */
#define NV_DPCD_AV_SYNC_DATA_BLOCK_AUD_DEL_INS_1                 (0x0000002C) /* R-XUR */
#define NV_DPCD_AV_SYNC_DATA_BLOCK_AUD_DEL_INS_2                 (0x0000002D) /* R-XUR */

// 0002Eh - 0002Fh: RESERVED. Reads all 0s

#define NV_DPCD_GUID                                             (0x00000030) /* R-XUR */

// 00040h - 00053h: RESERVED. Reads all 0s

#define NV_DPCD_RX_GTC_VALUE(i)                              (0x00000054+(i)) /* R--1A */
#define NV_DPCD_RX_GTC_VALUE__SIZE                                         4  /* R---S */

#define NV_DPCD_RX_GTC_REQ                                       (0x00000058) /* R-XUR */
#define NV_DPCD_RX_GTC_REQ_RX_GTC_MSTR_REQ                               0:0  /* R-XUF */
#define NV_DPCD_RX_GTC_REQ_RX_GTC_MSTR_REQ_NO                    (0x00000000) /* R-XUV */
#define NV_DPCD_RX_GTC_REQ_RX_GTC_MSTR_REQ_YES                   (0x00000001) /* R-XUV */
#define NV_DPCD_RX_GTC_REQ_TX_GTC_VALUE_PHASE_SKEW_EN                    1:1  /* R-XUF */
#define NV_DPCD_RX_GTC_REQ_TX_GTC_VALUE_PHASE_SKEW_EN_NO         (0x00000000) /* R-XUV */
#define NV_DPCD_RX_GTC_REQ_TX_GTC_VALUE_PHASE_SKEW_EN_YES        (0x00000001) /* R-XUV */

#define NV_DPCD_RX_GTC_FREQ_LOCK                                 (0x00000059) /* R-XUR */
#define NV_DPCD_RX_GTC_FREQ_LOCK_DONE                                    0:0  /* R-XUF */
#define NV_DPCD_RX_GTC_FREQ_LOCK_DONE_NO                         (0x00000000) /* R-XUV */
#define NV_DPCD_RX_GTC_FREQ_LOCK_DONE_YES                        (0x00000001) /* R-XUV */

// 0005Ah - 0006Fh: RESERVED Read all 0s

#define NV_DPCD_EDP_PSR_VERSION                                  (0x00000070) /* R-XUR */

#define NV_DPCD_EDP_PSR_CAP                                      (0x00000071) /* R-XUR */
#define NV_DPCD_EDP_PSR_CAP_LT_NEEDED                                    0:0  /* R-XUF */
#define NV_DPCD_EDP_PSR_CAP_LT_NEEDED_YES                        (0x00000000) /* R-XUV */
#define NV_DPCD_EDP_PSR_CAP_LT_NEEDED_NO                         (0x00000001) /* R-XUV */
#define NV_DPCD_EDP_PSR_CAP_SETUP_TIME                                   3:1  /* R-XUF */
#define NV_DPCD_EDP_PSR_CAP_SETUP_TIME_330US                     (0x00000000) /* R-XUV */
#define NV_DPCD_EDP_PSR_CAP_SETUP_TIME_275US                     (0x00000001) /* R-XUV */
#define NV_DPCD_EDP_PSR_CAP_SETUP_TIME_220US                     (0x00000002) /* R-XUV */
#define NV_DPCD_EDP_PSR_CAP_SETUP_TIME_165US                     (0x00000003) /* R-XUV */
#define NV_DPCD_EDP_PSR_CAP_SETUP_TIME_110US                     (0x00000004) /* R-XUV */
#define NV_DPCD_EDP_PSR_CAP_SETUP_TIME_55US                      (0x00000005) /* R-XUV */
#define NV_DPCD_EDP_PSR_CAP_SETUP_TIME_0US                       (0x00000006) /* R-XUV */
#define NV_DPCD_EDP_PSR_CAP_Y_COORD_NEEDED                               4:4  /* R-XUF */
#define NV_DPCD_EDP_PSR_CAP_Y_COORD_NEEDED_NO                    (0x00000000) /* R-XUF */
#define NV_DPCD_EDP_PSR_CAP_Y_COORD_NEEDED_YES                   (0x00000001) /* R-XUF */
#define NV_DPCD_EDP_PSR_CAP_GRAN_REQUIRED                                5:5  /* R-XUF */
#define NV_DPCD_EDP_PSR_CAP_GRAN_REQUIRED_NO                     (0x00000000) /* R-XUF */
#define NV_DPCD_EDP_PSR_CAP_GRAN_REQUIRED_YES                    (0x00000001) /* R-XUF*/

#define NV_DPCD_EDP_PSR2_X_GRANULARITY_L                         (0x00000072) /* R-XUR */
#define NV_DPCD_EDP_PSR2_X_GRANULARITY_H                         (0x00000073) /* R-XUR */
#define NV_DPCD_EDP_PSR2_Y_GRANULARITY                           (0x00000074) /* R-XUR */

// 00072h - 0007Fh: RESERVED Read all 0s

/*
 * When DETAILED_CAP_INFO_AVAILABLE = 0, 1 byte info per port.
 * When DETAILED_CAP_INFO_AVAILABLE = 1, 4 bytes info per port.
 * DETAILED_CAP_INFO_AVAILABLE located at 0x05h (DOWNSTREAMPORT_PRESENT), bit 5
 *
 * Byte 0 definition.
*/

#define NV_DPCD_DETAILED_CAP_INFO_DWNSTRM_PORT(i)                     (0x00000080+(i)*4) /* R--1A */
#define NV_DPCD_DETAILED_CAP_INFO_DWNSTRM_PORT__SIZE                                  4  /* R---S */
#define NV_DPCD_DETAILED_CAP_INFO_DWNSTRM_PORT_TX_TYPE                              2:0  /* R-XUF */
#define NV_DPCD_DETAILED_CAP_INFO_DWNSTRM_PORT_TX_TYPE_DISPLAYPORT          (0x00000000) /* R-XUV */
#define NV_DPCD_DETAILED_CAP_INFO_DWNSTRM_PORT_TX_TYPE_ANALOG               (0x00000001) /* R-XUV */
#define NV_DPCD_DETAILED_CAP_INFO_DWNSTRM_PORT_TX_TYPE_DVI                  (0x00000002) /* R-XUV */
#define NV_DPCD_DETAILED_CAP_INFO_DWNSTRM_PORT_TX_TYPE_HDMI                 (0x00000003) /* R-XUV */
#define NV_DPCD_DETAILED_CAP_INFO_DWNSTRM_PORT_TX_TYPE_OTHERS_NO_EDID       (0x00000004) /* R-XUV */
#define NV_DPCD_DETAILED_CAP_INFO_DWNSTRM_PORT_TX_TYPE_DP_PLUSPLUS          (0x00000005) /* R-XUV */
#define NV_DPCD_DETAILED_CAP_INFO_DWNSTRM_PORT_HPD                                  3:3  /* R-XUF */
#define NV_DPCD_DETAILED_CAP_INFO_DWNSTRM_PORT_HPD_NOT_AWARE                (0x00000000) /* R-XUV */
#define NV_DPCD_DETAILED_CAP_INFO_DWNSTRM_PORT_HPD_AWARE                    (0x00000001) /* R-XUV */
#define NV_DPCD_DETAILED_CAP_INFO_DWNSTRM_PORT_NON_EDID_ATTR                        7:4  /* R-XUF */
#define NV_DPCD_DETAILED_CAP_INFO_DWNSTRM_PORT_NON_EDID_480I_60HZ           (0x00000001) /* R-XUV */    // 720x480i
#define NV_DPCD_DETAILED_CAP_INFO_DWNSTRM_PORT_NON_EDID_480I_50HZ           (0x00000002) /* R-XUV */    // 720x480i
#define NV_DPCD_DETAILED_CAP_INFO_DWNSTRM_PORT_NON_EDID_1080I_60HZ          (0x00000003) /* R-XUV */
#define NV_DPCD_DETAILED_CAP_INFO_DWNSTRM_PORT_NON_EDID_1080I_50HZ          (0x00000004) /* R-XUV */
#define NV_DPCD_DETAILED_CAP_INFO_DWNSTRM_PORT_NON_EDID_720P_60HZ           (0x00000005) /* R-XUV */
#define NV_DPCD_DETAILED_CAP_INFO_DWNSTRM_PORT_NON_EDID_720P_50HZ           (0x00000007) /* R-XUV */

/*
 * Byte 1, Reserved for DisplayPort.
 */

#define NV_DPCD_DETAILED_CAP_INFO_ONE(i)                                    (0x00000081+(i)*4) /* R--1A */
#define NV_DPCD_DETAILED_CAP_INFO__SIZE                                     NV_DPCD_DETAILED_CAP_INFO_DWNSTRM_PORT__SIZE
#define NV_DPCD_DETAILED_CAP_INFO_ONE__SIZE                                                 4  /* R---S */
// For Analog VGA Donwstream Port. Maximum Pixel Rate in Mpixels per sec divided by 8
#define NV_DPCD_DETAILED_CAP_INFO_VGA_MAX_PIXEL_RATE                                      7:0 /* R-XUF */
/*
 * For DVI/HDMI/DP++ Downstream Port, Maximum TMDS clock rate supported in Mbps divided by 2.5
 * e.g. 66 (0x42) for 165 MHz, 90 (0x5a) for 225 MHz
 */
#define NV_DPCD_DETAILED_CAP_INFO_TMDS_MAX_CLOCK_RATE                                     7:0 /* R-XUF */

// Byte 2, for VGA/DVI/HDMI/DP++ Downstream Port, reserved for DisplayPort.
#define NV_DPCD_DETAILED_CAP_INFO_TWO(i)                                    (0x00000082+(i)*4) /* R--1A */
#define NV_DPCD_DETAILED_CAP_INFO_TWO__SIZE                                                 4  /* R---S */
#define NV_DPCD_DETAILED_CAP_INFO_MAX_BITS_PER_COMPONENT_DEF                              1:0  /* R-XUF */
#define NV_DPCD_DETAILED_CAP_INFO_MAX_BITS_PER_COMPONENT_DEF_8BPC                 (0x00000000) /* R-XUV */
#define NV_DPCD_DETAILED_CAP_INFO_MAX_BITS_PER_COMPONENT_DEF_10BPC                (0x00000001) /* R-XUV */
#define NV_DPCD_DETAILED_CAP_INFO_MAX_BITS_PER_COMPONENT_DEF_12BPC                (0x00000002) /* R-XUV */
#define NV_DPCD_DETAILED_CAP_INFO_MAX_BITS_PER_COMPONENT_DEF_16BPC                (0x00000003) /* R-XUV */
#define NV_DPCD_DETAILED_CAP_INFO_MAX_FRL_LINK_BW_SUPPORT                                 4:2  /* R-XUF */
#define NV_DPCD_DETAILED_CAP_INFO_MAX_FRL_LINK_BW_SUPPORT_ZERO                    (0x00000000) /* R-XUV */
#define NV_DPCD_DETAILED_CAP_INFO_MAX_FRL_LINK_BW_SUPPORT_9G                      (0x00000001) /* R-XUV */
#define NV_DPCD_DETAILED_CAP_INFO_MAX_FRL_LINK_BW_SUPPORT_18G                     (0x00000002) /* R-XUV */
#define NV_DPCD_DETAILED_CAP_INFO_MAX_FRL_LINK_BW_SUPPORT_24G                     (0x00000003) /* R-XUV */
#define NV_DPCD_DETAILED_CAP_INFO_MAX_FRL_LINK_BW_SUPPORT_32G                     (0x00000004) /* R-XUV */
#define NV_DPCD_DETAILED_CAP_INFO_MAX_FRL_LINK_BW_SUPPORT_40G                     (0x00000005) /* R-XUV */
#define NV_DPCD_DETAILED_CAP_INFO_MAX_FRL_LINK_BW_SUPPORT_48G                     (0x00000006) /* R-XUV */
#define NV_DPCD_DETAILED_CAP_INFO_SRC_CONTROL_MODE_SUPPORT                                5:5  /* R-XUF */
#define NV_DPCD_DETAILED_CAP_INFO_SRC_CONTROL_MODE_SUPPORT_NO                     (0x00000000) /* R-XUV */
#define NV_DPCD_DETAILED_CAP_INFO_SRC_CONTROL_MODE_SUPPORT_YES                    (0x00000001) /* R-XUV */
#define NV_DPCD_DETAILED_CAP_INFO_CONCURRENT_LT_SUPPORT                                   6:6  /* R-XUF */
#define NV_DPCD_DETAILED_CAP_INFO_CONCURRENT_LT_SUPPORT_NO                        (0x00000000) /* R-XUV */
#define NV_DPCD_DETAILED_CAP_INFO_CONCURRENT_LT_SUPPORT_YES                       (0x00000001) /* R-XUV */

#define NV_MAX_BPC_8                                                                        8
#define NV_MAX_BPC_10                                                                      10
#define NV_MAX_BPC_12                                                                      12
#define NV_MAX_BPC_16                                                                      16

// Byte 3, Reserved for DisplayPort and VGA
#define NV_DPCD_DETAILED_CAP_INFO_THREE(i)                                  (0x00000083+(i)*4) /* R--1A */
#define NV_DPCD_DETAILED_CAP_INFO_THREE__SIZE                                               4  /* R---S */
// For DVI
    #define NV_DPCD_DETAILED_CAP_INFO_DVI_DUAL_LINK                                       1:1  /* R-XUF */
    #define NV_DPCD_DETAILED_CAP_INFO_DVI_DUAL_LINK_NO                            (0x00000000) /* R-XUV */
    #define NV_DPCD_DETAILED_CAP_INFO_DVI_DUAL_LINK_YES                           (0x00000001) /* R-XUV */
    #define NV_DPCD_DETAILED_CAP_INFO_DVI_HIGH_COLOR_DEPTH                                2:2  /* R-XUF */
    #define NV_DPCD_DETAILED_CAP_INFO_DVI_HIGH_COLOR_DEPTH_NO                     (0x00000000) /* R-XUV */
    #define NV_DPCD_DETAILED_CAP_INFO_DVI_HIGH_COLOR_DEPTH_YES                    (0x00000001) /* R-XUV */
// For HDMI and DP++
    #define NV_DPCD_DETAILED_CAP_INFO_FRAME_SEQ_TO_FRAME_PACK                             0:0  /* R-XUF */
    #define NV_DPCD_DETAILED_CAP_INFO_FRAME_SEQ_TO_FRAME_PACK_NO                  (0x00000000) /* R-XUV */
    #define NV_DPCD_DETAILED_CAP_INFO_FRAME_SEQ_TO_FRAME_PACK_YES                 (0x00000001) /* R-XUV */
// For HDMI-PCon
    #define NV_DPCD_DETAILED_CAP_YCBCR422_PASS_THRU_SUPPORTED                             1:1  /* R-XUF */
    #define NV_DPCD_DETAILED_CAP_YCBCR422_PASS_THRU_SUPPORTED_NO                  (0x00000000) /* R-XUV */
    #define NV_DPCD_DETAILED_CAP_YCBCR422_PASS_THRU_SUPPORTED_YES                 (0x00000001) /* R-XUV */
    #define NV_DPCD_DETAILED_CAP_YCBCR420_PASS_THRU_SUPPORTED                             2:2  /* R-XUF */
    #define NV_DPCD_DETAILED_CAP_YCBCR420_PASS_THRU_SUPPORTED_NO                  (0x00000000) /* R-XUV */
    #define NV_DPCD_DETAILED_CAP_YCBCR420_PASS_THRU_SUPPORTED_YES                 (0x00000001) /* R-XUV */
    #define NV_DPCD_DETAILED_CAP_CONV_YCBCR444_TO_YCBCR422_SUPPORTED                      3:3  /* R-XUF */
    #define NV_DPCD_DETAILED_CAP_CONV_YCBCR444_TO_YCBCR422_SUPPORTED_NO           (0x00000000) /* R-XUV */
    #define NV_DPCD_DETAILED_CAP_CONV_YCBCR444_TO_YCBCR422_SUPPORTED_YES          (0x00000001) /* R-XUV */
    #define NV_DPCD_DETAILED_CAP_CONV_YCBCR444_TO_YCBCR420_SUPPORTED                      4:4  /* R-XUF */
    #define NV_DPCD_DETAILED_CAP_CONV_YCBCR444_TO_YCBCR420_SUPPORTED_NO           (0x00000000) /* R-XUV */
    #define NV_DPCD_DETAILED_CAP_CONV_YCBCR444_TO_YCBCR420_SUPPORTED_YES          (0x00000001) /* R-XUV */
    #define NV_DPCD_DETAILED_CAP_CONV_RGB601_TO_YCBCR601_SUPPORTED                        5:5  /* R-XUF */
    #define NV_DPCD_DETAILED_CAP_CONV_RGB601_TO_YCBCR601_SUPPORTED_NO             (0x00000000) /* R-XUV */
    #define NV_DPCD_DETAILED_CAP_CONV_RGB601_TO_YCBCR601_SUPPORTED_YES            (0x00000001) /* R-XUV */
    #define NV_DPCD_DETAILED_CAP_CONV_RGB709_TO_YCBCR709_SUPPORTED                        6:6  /* R-XUF */
    #define NV_DPCD_DETAILED_CAP_CONV_RGB709_TO_YCBCR709_SUPPORTED_NO             (0x00000000) /* R-XUV */
    #define NV_DPCD_DETAILED_CAP_CONV_RGB709_TO_YCBCR709_SUPPORTED_YES            (0x00000001) /* R-XUV */
    #define NV_DPCD_DETAILED_CAP_CONV_RGBBT2020_TO_YCBCRBT2020_SUPPORTED                  7:7  /* R-XUF */
    #define NV_DPCD_DETAILED_CAP_CONV_RGBBT2020_TO_YCBCRBT2020_SUPPORTED_NO       (0x00000000) /* R-XUV */
    #define NV_DPCD_DETAILED_CAP_CONV_RGBBT2020_TO_YCBCRBT2020_SUPPORTED_YES      (0x00000001) /* R-XUV */

/*
00090h - 000FFh: RESERVED for supporting up to 127 Downstream devices per Branch device. Read all 0s
Note: When DETAILED_CAP_INFO_AVAILABLE bit is set to 1, the maximum
number of Downstream ports will be limited to 32.
*/

#define NV_DPCD_LINK_BANDWIDTH_SET                               (0x00000100) /* RWXUR */
#define NV_DPCD_LINK_BANDWIDTH_SET_VAL                                   7:0  /* RWXUF */
#define NV_DPCD_LINK_BANDWIDTH_SET_VAL_1_62_GPBS                 (0x00000006) /* RWXUV */
#define NV_DPCD_LINK_BANDWIDTH_SET_VAL_2_70_GPBS                 (0x0000000a) /* RWXUV */
#define NV_DPCD_LINK_BANDWIDTH_SET_VAL_5_40_GPBS                 (0x00000014) /* RWXUV */

#define NV_DPCD_LANE_COUNT_SET                                   (0x00000101) /* RWXUR */
#define NV_DPCD_LANE_COUNT_SET_LANE                                      4:0  /* RWXUF */
#define NV_DPCD_LANE_COUNT_SET_LANE_1                            (0x00000001) /* RWXUV */
#define NV_DPCD_LANE_COUNT_SET_LANE_2                            (0x00000002) /* RWXUV */
#define NV_DPCD_LANE_COUNT_SET_LANE_4                            (0x00000004) /* RWXUV */
#define NV_DPCD_LANE_COUNT_SET_POST_LT_ADJ_REQ_GRANTED                   5:5  /* RWXUF */
#define NV_DPCD_LANE_COUNT_SET_POST_LT_ADJ_REQ_GRANTED_NO        (0x00000000) /* RWXUV */
#define NV_DPCD_LANE_COUNT_SET_POST_LT_ADJ_REQ_GRANTED_YES       (0x00000001) /* RWXUV */
#define NV_DPCD_LANE_COUNT_SET_ENHANCEDFRAMING                           7:7  /* RWXUF */
#define NV_DPCD_LANE_COUNT_SET_ENHANCEDFRAMING_FALSE             (0x00000000) /* RWXUV */
#define NV_DPCD_LANE_COUNT_SET_ENHANCEDFRAMING_TRUE              (0x00000001) /* RWXUV */

#define NV_DPCD_TRAINING_PATTERN_SET                             (0x00000102) /* RWXUR */
#define NV_DPCD_TRAINING_PATTERN_SET_TPS                                 1:0  /* RWXUF */
#define NV_DPCD_TRAINING_PATTERN_SET_TPS_NONE                    (0x00000000) /* RWXUV */
#define NV_DPCD_TRAINING_PATTERN_SET_TPS_TP1                     (0x00000001) /* RWXUV */
#define NV_DPCD_TRAINING_PATTERN_SET_TPS_TP2                     (0x00000002) /* RWXUV */
#define NV_DPCD_TRAINING_PATTERN_SET_TPS_TP3                     (0x00000003) /* RWXUV */
#define NV_DPCD_TRAINING_PATTERN_SET_LQPS                                3:2  /* R-XUF */
#define NV_DPCD_TRAINING_PATTERN_SET_LQPS_NO                     (0x00000000) /* R-XUV */
#define NV_DPCD_TRAINING_PATTERN_SET_LQPS_D10_2_TP               (0x00000001) /* R-XUV */
#define NV_DPCD_TRAINING_PATTERN_SET_LQPS_SYM_ERR_RATE_TP        (0x00000002) /* R-XUV */
#define NV_DPCD_TRAINING_PATTERN_SET_LQPS_PRBS7                  (0x00000003) /* R-XUV */
#define NV_DPCD_TRAINING_PATTERN_SET_RECOVERED_CLOCK_OUT_EN              4:4  /* RWXUF */
#define NV_DPCD_TRAINING_PATTERN_SET_RECOVERED_CLOCK_OUT_EN_NO   (0x00000000) /* RWXUV */
#define NV_DPCD_TRAINING_PATTERN_SET_RECOVERED_CLOCK_OUT_EN_YES  (0x00000001) /* RWXUV */
#define NV_DPCD_TRAINING_PATTERN_SET_SCRAMBLING_DISABLED                 5:5  /* RWXUF */
#define NV_DPCD_TRAINING_PATTERN_SET_SCRAMBLING_DISABLED_FALSE   (0x00000000) /* RWXUV */
#define NV_DPCD_TRAINING_PATTERN_SET_SCRAMBLING_DISABLED_TRUE    (0x00000001) /* RWXUV */
#define NV_DPCD_TRAINING_PATTERN_SET_SYM_ERR_SEL                         7:6  /* RWXUF */
#define NV_DPCD_TRAINING_PATTERN_SET_SYM_ERR_SEL_DISPARITY_ILLEGAL_SYMBOL_ERROR     (0x00000000) /* RWXUV */
#define NV_DPCD_TRAINING_PATTERN_SET_SYM_ERR_SEL_DISPARITY_ERROR                    (0x00000001) /* RWXUV */
#define NV_DPCD_TRAINING_PATTERN_SET_SYM_ERR_SEL_ILLEGAL_SYMBOL_ERROR               (0x00000002) /* RWXUV */

#define NV_DPCD_TRAINING_LANE_SET(i)                                (0x00000103+(i)) /* RW-1A */
#define NV_DPCD_TRAINING_LANE_SET__SIZE                                           4  /* RW--S */
#define NV_DPCD_TRAINING_LANE_SET_VOLTAGE_SWING                                 1:0  /* RWXUF */
#define NV_DPCD_TRAINING_LANE_SET_VOLTAGE_SWING_MAX_REACHED                     2:2  /* RWXUF */
#define NV_DPCD_TRAINING_LANE_SET_VOLTAGE_SWING_MAX_REACHED_TRUE        (0x00000001) /* RWXUV */
#define NV_DPCD_TRAINING_LANE_SET_PREEMPHASIS                                   4:3  /* RWXUF */
#define NV_DPCD_TRAINING_LANE_SET_PREEMPHASIS_MAX_REACHED                       5:5  /* RWXUF */
#define NV_DPCD_TRAINING_LANE_SET_PREEMPHASIS_MAX_REACHED_TRUE          (0x00000001) /* RWXUV */
#define NV_DPCD_TRAINING_LANE0_SET                               (0x00000103) /* RWXUR */

#define NV_DPCD_MAX_VOLTAGE_SWING                                (0x00000003) /* RWXUV */
#define NV_DPCD_MAX_VOLTAGE_PREEMPHASIS                          (0x00000003) /* RWXUV */

#define NV_DPCD_TRAINING_LANE1_SET                               (0x00000104) /* RWXUR */
#define NV_DPCD_TRAINING_LANE2_SET                               (0x00000105) /* RWXUR */
#define NV_DPCD_TRAINING_LANE3_SET                               (0x00000106) /* RWXUR */
#define NV_DPCD_TRAINING_LANEX_SET_DRIVE_CURRENT                         1:0  /* RWXUF */
#define NV_DPCD_TRAINING_LANEX_SET_DRIVE_CURRENT_MAX_REACHED             2:2  /* RWXUF */
#define NV_DPCD_TRAINING_LANEX_SET_DRIVE_CURRENT_MAX_REACHED_TRUE  (0x00000001) /* RWXUV */
#define NV_DPCD_TRAINING_LANEX_SET_PREEMPHASIS                           4:3  /* RWXUF */
#define NV_DPCD_TRAINING_LANEX_SET_PREEMPHASIS_MAX_REACHED               5:5  /* RWXUF */
#define NV_DPCD_TRAINING_LANEX_SET_PREEMPHASIS_MAX_REACHED_TRUE  (0x00000001) /* RWXUV */

#define NV_DPCD_DOWNSPREAD_CTRL                                               (0x00000107) /* RWXUR */
#define NV_DPCD_DOWNSPREAD_CTRL_SPREAD_AMP                                            4:4  /* RWXUF */
#define NV_DPCD_DOWNSPREAD_CTRL_SPREAD_AMP_NONE                               (0x00000000) /* RWXUV */
#define NV_DPCD_DOWNSPREAD_CTRL_SPREAD_AMP_LESS_THAN_0_5                      (0x00000001) /* RWXUV */
#define NV_DPCD_DOWNSPREAD_CTRL_FIXED_VTOTAL_AS_SDP_EN_IN_PR_ACTIVE                   6:6  /* RWXUF */
#define NV_DPCD_DOWNSPREAD_CTRL_FIXED_VTOTAL_AS_SDP_EN_IN_PR_ACTIVE_NO        (0x00000000) /* RWXUV */
#define NV_DPCD_DOWNSPREAD_CTRL_FIXED_VTOTAL_AS_SDP_EN_IN_PR_ACTIVE_YES       (0x00000001) /* RWXUV */
#define NV_DPCD_DOWNSPREAD_CTRL_MSA_TIMING_PAR_IGNORED                                7:7  /* RWXUF */
#define NV_DPCD_DOWNSPREAD_CTRL_MSA_TIMING_PAR_IGNORED_FALSE                  (0x00000000) /* RWXUV */
#define NV_DPCD_DOWNSPREAD_CTRL_MSA_TIMING_PAR_IGNORED_TRUE                   (0x00000001) /* RWXUV */


#define NV_DPCD_MAIN_LINK_CHANNEL_CODING_SET                      (0x00000108) /* RWXUR */
#define NV_DPCD_MAIN_LINK_CHANNEL_CODING_SET_ANSI_8B_10B                  0:0  /* RWXUF */
#define NV_DPCD_MAIN_LINK_CHANNEL_CODING_SET_ANSI_8B_10B_FALSE    (0x00000000) /* RWXUV */
#define NV_DPCD_MAIN_LINK_CHANNEL_CODING_SET_ANSI_8B_10B_TRUE     (0x00000001) /* RWXUV */
#define NV_DPCD_MAIN_LINK_CHANNEL_CODING_SET_ANSI_128B_132B               1:1  /* RWXUF */
#define NV_DPCD_MAIN_LINK_CHANNEL_CODING_SET_ANSI_128B_132B_FALSE (0x00000000) /* RWXUV */
#define NV_DPCD_MAIN_LINK_CHANNEL_CODING_SET_ANSI_128B_132B_TRUE  (0x00000001) /* RWXUV */

#define NV_DPCD_I2C_CTRL_SET                                     (0x00000109) /* RWXUR */
#define NV_DPCD_I2C_CTRL_SET_SPEED                                       7:0  /* RWXUF */
#define NV_DPCD_I2C_CTRL_SET_SPEED_DEFAULT                       (0x00000000) /* RWXUV */
#define NV_DPCD_I2C_CTRL_SET_SPEED_1K                            (0x00000001) /* RWXUV */
#define NV_DPCD_I2C_CTRL_SET_SPEED_5K                            (0x00000002) /* RWXUV */
#define NV_DPCD_I2C_CTRL_SET_SPEED_10K                           (0x00000004) /* RWXUV */
#define NV_DPCD_I2C_CTRL_SET_SPEED_100K                          (0x00000008) /* RWXUV */
#define NV_DPCD_I2C_CTRL_SET_SPEED_400K                          (0x00000010) /* RWXUV */
#define NV_DPCD_I2C_CTRL_SET_SPEED_1M                            (0x00000020) /* RWXUV */

#define NV_DPCD_EDP_CONFIG_SET                                   (0x0000010A) /* RWXUR */
#define NV_DPCD_EDP_CONFIG_SET_ALTERNATE_SCRAMBLER_RESET                 0:0  /* RWXUF */
#define NV_DPCD_EDP_CONFIG_SET_ALTERNATE_SCRAMBLER_RESET_DISABLE (0x00000000) /* RWXUV */
#define NV_DPCD_EDP_CONFIG_SET_ALTERNATE_SCRAMBLER_RESET_ENABLE  (0x00000001) /* RWXUV */
#define NV_DPCD_EDP_CONFIG_SET_FRAMING_CHANGE                            1:1  /* RWXUF */
#define NV_DPCD_EDP_CONFIG_SET_FRAMING_CHANGE_DISABLE            (0x00000000) /* RWXUV */
#define NV_DPCD_EDP_CONFIG_SET_FRAMING_CHANGE_ENABLE             (0x00000001) /* RWXUV */
#define NV_DPCD_EDP_CONFIG_SET_INVERTED_TRAINING_BIT                     2:2  /* RWXUF */
#define NV_DPCD_EDP_CONFIG_SET_INVERTED_TRAINING_BIT_DISABLE     (0x00000000) /* RWXUV */
#define NV_DPCD_EDP_CONFIG_SET_INVERTED_TRAINING_BIT_ENABLE      (0x00000001) /* RWXUV */
#define NV_DPCD_EDP_CONFIG_SET_PANEL_SELF_TEST                           7:7  /* RWXUF */
#define NV_DPCD_EDP_CONFIG_SET_PANEL_SELF_TEST_DISABLE           (0x00000000) /* RWXUV */
#define NV_DPCD_EDP_CONFIG_SET_PANEL_SELF_TEST_ENABLE            (0x00000001) /* RWXUV */

#define NV_DPCD_LINK_QUAL_LANE_SET(i)                        (0x0000010B+(i)) /* RW-1A */
#define NV_DPCD_LINK_QUAL_LANE_SET__SIZE                                   4  /* RW--S */
#define NV_DPCD_LINK_QUAL_LANE_SET_LQS                                   2:0  /* RWXUF */
#define NV_DPCD_LINK_QUAL_LANE_SET_LQS_NO                        (0x00000000) /* RWXUV */
#define NV_DPCD_LINK_QUAL_LANE_SET_LQS_D10_2                     (0x00000001) /* RWXUV */
#define NV_DPCD_LINK_QUAL_LANE_SET_LQS_SYM_ERR_MEASUREMENT_CNT   (0x00000002) /* RWXUV */
#define NV_DPCD_LINK_QUAL_LANE_SET_LQS_PRBS7                     (0x00000003) /* RWXUV */
#define NV_DPCD_LINK_QUAL_LANE_SET_LQS_80_BIT_CUSTOM             (0x00000004) /* RWXUV */
#define NV_DPCD_LINK_QUAL_LANE_SET_LQS_HBR2                      (0x00000005) /* RWXUV */

#define NV_DPCD_TRAINING_LANE0_1_SET2                            (0x0000010F) /* RWXUR */
#define NV_DPCD_TRAINING_LANE2_3_SET2                            (0x00000110) /* RWXUR */
#define NV_DPCD_LANEX_XPLUS1_TRAINING_LANEX_SET2_POST_CURSOR2                                1:0  /* RWXUF */
#define NV_DPCD_LANEX_XPLUS1_TRAINING_LANEX_SET2_POST_CURSOR2_MAX_REACHED                    2:2  /* RWXUF */
#define NV_DPCD_LANEX_XPLUS1_TRAINING_LANEX_SET2_POST_CURSOR2_MAX_REACHED_TRUE       (0x00000001) /* RWXUV */
#define NV_DPCD_LANEX_XPLUS1_TRAINING_LANEXPLUS1_SET2_POST_CURSOR2                           5:4  /* RWXUF */
#define NV_DPCD_LANEX_XPLUS1_TRAINING_LANEXPLUS1_SET2_POST_CURSOR2_MAX_REACHED               6:6  /* RWXUF */
#define NV_DPCD_LANEX_XPLUS1_TRAINING_LANEXPLUS1_SET2_POST_CURSOR2_MAX_REACHED_TRUE  (0x00000001) /* RWXUV */

#define NV_DPCD_MSTM_CTRL                                        (0x00000111) /* RWXUR */
#define NV_DPCD_MSTM_CTRL_EN                                             0:0  /* RWXUF */
#define NV_DPCD_MSTM_CTRL_EN_NO                                  (0x00000000) /* RWXUV */
#define NV_DPCD_MSTM_CTRL_EN_YES                                 (0x00000001) /* RWXUV */
#define NV_DPCD_MSTM_CTRL_UP_REQ_EN                                      1:1  /* RWXUF */
#define NV_DPCD_MSTM_CTRL_UP_REQ_EN_NO                           (0x00000000) /* RWXUV */
#define NV_DPCD_MSTM_CTRL_UP_REQ_EN_YES                          (0x00000001) /* RWXUV */
#define NV_DPCD_MSTM_CTRL_UPSTREAM_IS_SRC                                2:2  /* RWXUF */
#define NV_DPCD_MSTM_CTRL_UPSTREAM_IS_SRC_NO                     (0x00000000) /* RWXUV */
#define NV_DPCD_MSTM_CTRL_UPSTREAM_IS_SRC_YES                    (0x00000001) /* RWXUV */

#define NV_DPCD_AUDIO_DELAY(i)                               (0x00000112+(i)) /* RW-1A */
#define NV_DPCD_AUDIO_DELAY__SIZE                                          3  /* NNNNS */

#define NV_DPCD_LINK_RATE_SET                                    (0x00000115) /* RWXUR */
#define NV_DPCD_LINK_RATE_SET_VAL                                        2:0  /* RWXUF */

// 00115h - 00117h: RESERVED. Reads all 0s

#define NV_DPCD_UPSTREAM_DEV_DP_PWR                              (0x00000118) /* RWXUR */
#define NV_DPCD_UPSTREAM_DEV_DP_PWR_NOT_NEEDED                           0:0  /* RWXUF */
#define NV_DPCD_UPSTREAM_DEV_DP_PWR_NOT_NEEDED_FALSE             (0x00000000) /* RWXUV */
#define NV_DPCD_UPSTREAM_DEV_DP_PWR_NOT_NEEDED_TRUE              (0x00000001) /* RWXUV */

#define NV_DPCD_EXTENDED_DPRX_WAKE_TIMEOUT                       (0x00000119) /* RWXUR */
#define NV_DPCD_EXTENDED_DPRX_WAKE_TIMEOUT_PERIOD_GRANTED                0:0  /* RWXUF */
#define NV_DPCD_EXTENDED_DPRX_WAKE_TIMEOUT_PERIOD_GRANTED_NO     (0x00000000) /* RWXUV */
#define NV_DPCD_EXTENDED_DPRX_WAKE_TIMEOUT_PERIOD_GRANTED_YES    (0x00000001) /* RWXUV */

// 0011Ah - 0011Fh: RESERVED. Reads all 0s
// 00126h - 00153h: RESERVED. Reads all 0s

#define NV_DPCD_TX_GTC_VALUE(i)                              (0x00000154+(i)) /* RW-1A */
#define NV_DPCD_TX_GTC_VALUE__SIZE                                         4  /* R---S */

#define NV_DPCD_RX_GTC_VALUE_PHASE_SKEW                          (0x00000158) /* RWXUR */
#define NV_DPCD_RX_GTC_VALUE_PHASE_SKEW_EN                               0:0  /* RWXUF */
#define NV_DPCD_RX_GTC_VALUE_PHASE_SKEW_EN_NO                    (0x00000000) /* RWXUV */
#define NV_DPCD_RX_GTC_VALUE_PHASE_SKEW_EN_YES                   (0x00000001) /* RWXUV */

#define NV_DPCD_TX_GTC_FREQ_LOCK                                 (0x00000159) /* RWXUR */
#define NV_DPCD_TX_GTC_FREQ_LOCK_DONE                                    0:0  /* RWXUF */
#define NV_DPCD_TX_GTC_FREQ_LOCK_DONE_NO                         (0x00000000) /* RWXUV */
#define NV_DPCD_TX_GTC_FREQ_LOCK_DONE_YES                        (0x00000001) /* RWXUV */

// 0015Ah - 0016Fh: RESERVED. Read all 0s

#define NV_DPCD_EDP_PSR_CONFIG                                   (0x00000170)  /* RWXUR */
#define NV_DPCD_EDP_PSR_CONFIG_SINK_ENABLE                               0:0   /* RWXUF */
#define NV_DPCD_EDP_PSR_CONFIG_SINK_ENABLE_NO                    (0x00000000)  /* RWXUV */
#define NV_DPCD_EDP_PSR_CONFIG_SINK_ENABLE_YES                   (0x00000001)  /* RWXUV */
#define NV_DPCD_EDP_PSR_CONFIG_SOURCE_LINK_ACTIVE                        1:1   /* RWXUF */
#define NV_DPCD_EDP_PSR_CONFIG_SOURCE_LINK_ACTIVE_NO             (0x00000000)  /* RWXUV */
#define NV_DPCD_EDP_PSR_CONFIG_SOURCE_LINK_ACTIVE_YES            (0x00000001)  /* RWXUV */
#define NV_DPCD_EDP_PSR_CONFIG_CRC_VERIFICATION_ACTIVE                   2:2   /* RWXUF */
#define NV_DPCD_EDP_PSR_CONFIG_CRC_VERIFICATION_ACTIVE_NO        (0x00000000)  /* RWXUV */
#define NV_DPCD_EDP_PSR_CONFIG_CRC_VERIFICATION_ACTIVE_YES       (0x00000001)  /* RWXUV */
#define NV_DPCD_EDP_PSR_CONFIG_FRAME_CAPTURE_INDICATION                  3:3   /* RWXUF */
#define NV_DPCD_EDP_PSR_CONFIG_FRAME_CAPTURE_INDICATION_IMM      (0x00000000)  /* RWXUV */
#define NV_DPCD_EDP_PSR_CONFIG_FRAME_CAPTURE_INDICATION_SECOND   (0x00000001)  /* RWXUV */
#define NV_DPCD_EDP_PSR_CONFIG_SU_LINE_CAPTURE_INDICATION                4:4   /* RWXUF */
#define NV_DPCD_EDP_PSR_CONFIG_SU_LINE_CAPTURE_INDICATION_IMM    (0x00000000)  /* RWXUV */
#define NV_DPCD_EDP_PSR_CONFIG_SU_LINE_CAPTURE_INDICATION_SECOND (0x00000001)  /* RWXUV */
#define NV_DPCD_EDP_PSR_CONFIG_HPD_IRQ_ON_CRC_ERROR                      5:5   /* RWXUF */
#define NV_DPCD_EDP_PSR_CONFIG_HPD_IRQ_ON_CRC_ERROR_NO           (0x00000000)  /* RWXUV */
#define NV_DPCD_EDP_PSR_CONFIG_HPD_IRQ_ON_CRC_ERROR_YES          (0x00000001)  /* RWXUV */
#define NV_DPCD_EDP_PSR_CONFIG_ENABLE_PSR2                               6:6   /* RWXUF */
#define NV_DPCD_EDP_PSR_CONFIG_ENABLE_PSR2_NO                    (0x00000000)  /* RWXUV */
#define NV_DPCD_EDP_PSR_CONFIG_ENABLE_PSR2_YES                   (0x00000001)  /* RWXUV */

// 00171h - 0019Fh: RESERVED. Read all 0s


#define NV_DPCD_ADAPTER_CTRL                                     (0x000001A0) /* RWXUR */
#define NV_DPCD_ADAPTER_CTRL_FORCE_LOAD_SENSE                            0:0  /* RWXUF */
#define NV_DPCD_ADAPTER_CTRL_FORCE_LOAD_SENSE_NO                 (0x00000000) /* RWXUV */
#define NV_DPCD_ADAPTER_CTRL_FORCE_LOAD_SENSE_YES                (0x00000001) /* RWXUV */

#define NV_DPCD_BRANCH_DEV_CTRL                                  (0x000001A1) /* RWXUR */
#define NV_DPCD_BRANCH_DEV_CTRL_HOTPLUG_EVENT_TYPE                       0:0  /* RWXUF */
#define NV_DPCD_BRANCH_DEV_CTRL_HOTPLUG_EVENT_TYPE_LONGPULSE     (0x00000000) /* RWXUV */
#define NV_DPCD_BRANCH_DEV_CTRL_HOTPLUG_EVENT_TYPE_IRQ_HPD       (0x00000001) /* RWXUV */
#define NV_DPCD_BRANCH_DEV_CTRL_HOTPLUG_EVENT_TYPE_DEFAULT       NV_DPCD_BRANCH_DEV_CTRL_HOTPLUG_EVENT_TYPE_LONGPULSE

// 001A2h - 0019Fh: RESERVED. Read all 0s

#define NV_DPCD_PAYLOAD_ALLOC_SET                                (0x000001C0) /* RWXUR */
#define NV_DPCD_PAYLOAD_ALLOC_SET_PAYLOAD_ID                             6:0  /* RWXUF */

#define NV_DPCD_PAYLOAD_ALLOC_START_TIME_SLOT                    (0x000001C1) /* RWXUR */
#define NV_DPCD_PAYLOAD_ALLOC_START_TIME_SLOT_VAL                        5:0  /* RWXUF */

#define NV_DPCD_PAYLOAD_ALLOC_TIME_SLOT_COUNT                    (0x000001C2) /* RWXUR */
#define NV_DPCD_PAYLOAD_ALLOC_TIME_SLOT_COUNT_VAL                        5:0  /* RWXUF */

// 001C3h - 001FFh: RESERVED. Reads all 0s

#define NV_DPCD_SINK_COUNT                                       (0x00000200) /* R-XUR */
// Bits 7 and 5:0 = SINK_COUNT
#define NV_DPCD_SINK_COUNT_VAL_BIT_05_MASK                             (0x3F)
#define NV_DPCD_SINK_COUNT_VAL_BIT_7                                   (0x80)
#define NV_DPCD_SINK_COUNT_VAL(x)   ((x & NV_DPCD_SINK_COUNT_VAL_BIT_05_MASK) \
                                    | ((x & NV_DPCD_SINK_COUNT_VAL_BIT_7) >> 1))
#define NV_DPCD_SINK_COUNT_CP_READY                                      6:6  /* R-XUF */
#define NV_DPCD_SINK_COUNT_CP_READY_NO                           (0x00000000) /* R-XUV */
#define NV_DPCD_SINK_COUNT_CP_READY_YES                          (0x00000001) /* R-XUV */

#define NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR                        (0x00000201) /* RWXUR */
#define NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_REMOTE_CTRL                    0:0  /* RWXUF */
#define NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_REMOTE_CTRL_NO         (0x00000000) /* RWXUV */
#define NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_REMOTE_CTRL_YES        (0x00000001) /* RWXUV */
#define NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_AUTO_TEST                      1:1  /* RWXUF */
#define NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_AUTO_TEST_NO           (0x00000000) /* RWXUV */
#define NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_AUTO_TEST_YES          (0x00000001) /* RWXUV */
#define NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_CP                             2:2  /* RWXUF */
#define NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_CP_NO                  (0x00000000) /* RWXUV */
#define NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_CP_YES                 (0x00000001) /* RWXUV */
#define NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_MCCS_IRQ                       3:3  /* RWXUF */
#define NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_MCCS_IRQ_NO            (0x00000000) /* RWXUV */
#define NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_MCCS_IRQ_YES           (0x00000001) /* RWXUV */
#define NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_DOWN_REP_MSG_RDY               4:4  /* RWXUF */
#define NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_DOWN_REP_MSG_RDY_NO    (0x00000000) /* RWXUV */
#define NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_DOWN_REP_MSG_RDY_YES   (0x00000001) /* RWXUV */
#define NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_UP_REQ_MSG_RDY                 5:5  /* RWXUF */
#define NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_UP_REQ_MSG_RDY_NO      (0x00000000) /* RWXUV */
#define NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_UP_REQ_MSG_RDY_YES     (0x00000001) /* RWXUV */
#define NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_SINK_SPECIFIC_IRQ              6:6  /* RWXUF */
#define NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_SINK_SPECIFIC_IRQ_NO   (0x00000000) /* RWXUV */
#define NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_SINK_SPECIFIC_IRQ_YES  (0x00000001) /* RWXUV */

#define NV_DPCD_LANE0_1_STATUS                                   (0x00000202) /* R-XUR */

#define NV_DPCD_LANE2_3_STATUS                                   (0x00000203) /* R-XUR */
#define NV_DPCD_LANEX_XPLUS1_STATUS_LANEX_CR_DONE                        0:0  /* R-XUF */
#define NV_DPCD_LANEX_XPLUS1_STATUS_LANEX_CR_DONE_NO             (0x00000000) /* R-XUV */
#define NV_DPCD_LANEX_XPLUS1_STATUS_LANEX_CR_DONE_YES            (0x00000001) /* R-XUV */
#define NV_DPCD_LANEX_XPLUS1_STATUS_LANEX_CHN_EQ_DONE                    1:1  /* R-XUF */
#define NV_DPCD_LANEX_XPLUS1_STATUS_LANEX_CHN_EQ_DONE_NO         (0x00000000) /* R-XUV */
#define NV_DPCD_LANEX_XPLUS1_STATUS_LANEX_CHN_EQ_DONE_YES        (0x00000001) /* R-XUV */
#define NV_DPCD_LANEX_XPLUS1_STATUS_LANEX_SYMBOL_LOCKED                  2:2  /* R-XUF */
#define NV_DPCD_LANEX_XPLUS1_STATUS_LANEX_SYMBOL_LOCKED_NO       (0x00000000) /* R-XUV */
#define NV_DPCD_LANEX_XPLUS1_STATUS_LANEX_SYMBOL_LOCKED_YES      (0x00000001) /* R-XUV */
#define NV_DPCD_LANEX_XPLUS1_STATUS_LANEXPLUS1_CR_DONE                   4:4  /* R-XUF */
#define NV_DPCD_LANEX_XPLUS1_STATUS_LANEXPLUS1_CR_DONE_NO        (0x00000000) /* R-XUV */
#define NV_DPCD_LANEX_XPLUS1_STATUS_LANEXPLUS1_CR_DONE_YES       (0x00000001) /* R-XUV */
#define NV_DPCD_LANEX_XPLUS1_STATUS_LANEXPLUS1_CHN_EQ_DONE               5:5  /* R-XUF */
#define NV_DPCD_LANEX_XPLUS1_STATUS_LANEXPLUS1_CHN_EQ_DONE_NO    (0x00000000) /* R-XUV */
#define NV_DPCD_LANEX_XPLUS1_STATUS_LANEXPLUS1_CHN_EQ_DONE_YES   (0x00000001) /* R-XUV */
#define NV_DPCD_LANEX_XPLUS1_STATUS_LANEXPLUS1_SYMBOL_LOCKED             6:6  /* R-XUF */
#define NV_DPCD_LANEX_XPLUS1_STATUS_LANEXPLUS1_SYMBOL_LOCKED_NO  (0x00000000) /* R-XUV */
#define NV_DPCD_LANEX_XPLUS1_STATUS_LANEXPLUS1_SYMBOL_LOCKED_YES (0x00000001) /* R-XUV */

#define NV_DPCD_LANE_ALIGN_STATUS_UPDATED                                   (0x00000204) /* R-XUR */
#define NV_DPCD_LANE_ALIGN_STATUS_UPDATED_INTERLANE_ALIGN_DONE                      0:0  /* R-XUF */
#define NV_DPCD_LANE_ALIGN_STATUS_UPDATED_INTERLANE_ALIGN_DONE_NO           (0x00000000) /* R-XUV */
#define NV_DPCD_LANE_ALIGN_STATUS_UPDATED_INTERLANE_ALIGN_DONE_YES          (0x00000001) /* R-XUV */
#define NV_DPCD_LANE_ALIGN_STATUS_UPDATED_POST_LT_ADJ_REQ_IN_PROGRESS               1:1  /* R-XUF */
#define NV_DPCD_LANE_ALIGN_STATUS_UPDATED_POST_LT_ADJ_REQ_IN_PROGRESS_NO    (0x00000000) /* R-XUV */
#define NV_DPCD_LANE_ALIGN_STATUS_UPDATED_POST_LT_ADJ_REQ_IN_PROGRESS_YES   (0x00000001) /* R-XUV */
#define NV_DPCD_LANE_ALIGN_STATUS_UPDATED_D0WNSTRM_PORT_STATUS_DONE                 6:6  /* R-XUF */
#define NV_DPCD_LANE_ALIGN_STATUS_UPDATED_D0WNSTRM_PORT_STATUS_DONE_NO      (0x00000000) /* R-XUV */
#define NV_DPCD_LANE_ALIGN_STATUS_UPDATED_D0WNSTRM_PORT_STATUS_DONE_YES     (0x00000001) /* R-XUV */
#define NV_DPCD_LANE_ALIGN_STATUS_UPDATED_LINK_STATUS_UPDATED                       7:7  /* R-XUF */
#define NV_DPCD_LANE_ALIGN_STATUS_UPDATED_LINK_STATUS_UPDATED_NO            (0x00000000) /* R-XUV */
#define NV_DPCD_LANE_ALIGN_STATUS_UPDATED_LINK_STATUS_UPDATED_YES           (0x00000001) /* R-XUV */

#define NV_DPCD_SINK_STATUS                                      (0x00000205) /* R-XUR */
#define NV_DPCD_SINK_STATUS_RECEIVE_PORT_0_STATUS                        0:0  /* R-XUF */
#define NV_DPCD_SINK_STATUS_RECEIVE_PORT_0_STATUS_IN_SYNC_NO     (0x00000000) /* R-XUV */
#define NV_DPCD_SINK_STATUS_RECEIVE_PORT_0_STATUS_IN_SYNC_YES    (0x00000001) /* R-XUV */
#define NV_DPCD_SINK_STATUS_RECEIVE_PORT_1_STATUS                        1:1  /* R-XUF */
#define NV_DPCD_SINK_STATUS_RECEIVE_PORT_1_STATUS_IN_SYNC_NO     (0x00000000) /* R-XUV */
#define NV_DPCD_SINK_STATUS_RECEIVE_PORT_1_STATUS_IN_SYNC_YES    (0x00000001) /* R-XUV */

#define NV_DPCD_LANE0_1_ADJUST_REQ                               (0x00000206) /* R-XUR */
#define NV_DPCD_LANE2_3_ADJUST_REQ                               (0x00000207) /* R-XUR */
#define NV_DPCD_LANEX_XPLUS1_ADJUST_REQ_LANEX_DRIVE_CURRENT              1:0  /* R-XUF */
#define NV_DPCD_LANEX_XPLUS1_ADJUST_REQ_LANEX_PREEMPHASIS                3:2  /* R-XUF */
#define NV_DPCD_LANEX_XPLUS1_ADJUST_REQ_LANEXPLUS1_DRIVE_CURRENT         5:4  /* R-XUF */
#define NV_DPCD_LANEX_XPLUS1_ADJUST_REQ_LANEXPLUS1_PREEMPHASIS           7:6  /* R-XUF */

#define NV_DPCD_TRAINING_SCORE_LANE(i)                       (0x00000208+(i)) /* R--1A */
#define NV_DPCD_TRAINING_SCORE_LANE__SIZE                                  4  /* R---S */

#define NV_DPCD_ADJUST_REQ_POST_CURSOR2                          (0x0000020C) /* R-XUR */
#define NV_DPCD_ADJUST_REQ_POST_CURSOR2_LANE(i)                  i%4*2+1:i%4*2
#define NV_DPCD_ADJUST_REQ_POST_CURSOR2_LANE0                            1:0  /* R-XUF */
#define NV_DPCD_ADJUST_REQ_POST_CURSOR2_LANE1                            3:2  /* R-XUF */
#define NV_DPCD_ADJUST_REQ_POST_CURSOR2_LANE2                            5:4  /* R-XUF */
#define NV_DPCD_ADJUST_REQ_POST_CURSOR2_LANE3                            7:6  /* R-XUF */
#define NV_DPCD_ADJUST_REQ_POST_CURSOR2_LANE4                            1:0  /* R-XUF */
#define NV_DPCD_ADJUST_REQ_POST_CURSOR2_LANE5                            3:2  /* R-XUF */
#define NV_DPCD_ADJUST_REQ_POST_CURSOR2_LANE6                            5:4  /* R-XUF */
#define NV_DPCD_ADJUST_REQ_POST_CURSOR2_LANE7                            7:6  /* R-XUF */

#define NV_DPCD_EDP_LINK_CONFIG_STATUS                          (0x0000020c)  /* RWXUR */
#define NV_DPCD_EDP_LINK_CONFIG_STATUS_SET                               0:0  /* R-XUF */
#define NV_DPCD_EDP_LINK_CONFIG_STATUS_SET_LINK_BW              (0x00000000)  /* R-XUV */
#define NV_DPCD_EDP_LINK_CONFIG_STATUS_SET_LINK_RATE            (0x00000001)  /* R-XUV */
#define NV_DPCD_EDP_LINK_CONFIG_STATUS_VALID                             1:1  /* R-XUF */
#define NV_DPCD_EDP_LINK_CONFIG_STATUS_VALID_NO                 (0x00000000)  /* R-XUV */
#define NV_DPCD_EDP_LINK_CONFIG_STATUS_VALID_YES                (0x00000001)  /* R-XUV */

// 0020Fh: RESERVED. Read all 0s

#define NV_DPCD_SYMBOL_ERROR_COUNT_LANEX_BYTE0(i)          (0x00000210+(i)*2) /* R--1A */
#define NV_DPCD_SYMBOL_ERROR_COUNT_LANEX_BYTE0__SIZE                       4  /* R---S */
#define NV_DPCD_SYMBOL_ERROR_COUNT_LANEX_BYTE0_VALUE                     7:0  /* R-XUF */
#define NV_DPCD_SYMBOL_ERROR_COUNT_LANEX_BYTE1(i)          (0x00000211+(i)*2) /* R--1A */
#define NV_DPCD_SYMBOL_ERROR_COUNT_LANEX_BYTE1__SIZE                       4  /* R---S */
#define NV_DPCD_SYMBOL_ERROR_COUNT_LANEX_BYTE1_VALUE                     6:0  /* R-XUF */
#define NV_DPCD_SYMBOL_ERROR_COUNT_LANEX_BYTE1_VALID                     7:7  /* R-XUF */

#define NV_DPCD_TEST_REQUEST                                     (0x00000218) /* R-XUR */
#define NV_DPCD_TEST_REQUEST_TEST_LINK_TRAINING                          0:0  /* R-XUF */
#define NV_DPCD_TEST_REQUEST_TEST_LINK_TRAINING_NO               (0x00000000) /* R-XUV */
#define NV_DPCD_TEST_REQUEST_TEST_LINK_TRAINING_YES              (0x00000001) /* R-XUV */
#define NV_DPCD_TEST_REQUEST_TEST_PATTERN                                1:1  /* R-XUF */
#define NV_DPCD_TEST_REQUEST_TEST_PATTERN_NO                     (0x00000000) /* R-XUV */
#define NV_DPCD_TEST_REQUEST_TEST_PATTERN_YES                    (0x00000001) /* R-XUV */
#define NV_DPCD_TEST_REQUEST_TEST_EDID_READ                              2:2  /* R-XUF */
#define NV_DPCD_TEST_REQUEST_TEST_EDID_READ_NO                   (0x00000000) /* R-XUV */
#define NV_DPCD_TEST_REQUEST_TEST_EDID_READ_YES                  (0x00000001) /* R-XUV */
#define NV_DPCD_TEST_REQUEST_TEST_PHY_TEST_PATTERN                       3:3  /* R-XUF */
#define NV_DPCD_TEST_REQUEST_TEST_PHY_TEST_PATTERN_NO            (0x00000000) /* R-XUV */
#define NV_DPCD_TEST_REQUEST_TEST_PHY_TEST_PATTERN_YES           (0x00000001) /* R-XUV */

#define NV_DPCD_TEST_LINK_RATE                                   (0x00000219) /* R-XUR */
#define NV_DPCD_TEST_LINK_RATE_TYPE                                      7:0  /* R-XUF */
#define NV_DPCD_TEST_LINK_RATE_TYPE_1_62G                        (0x00000006) /* R-XUV */
#define NV_DPCD_TEST_LINK_RATE_TYPE_2_70G                        (0x0000000A) /* R-XUV */
#define NV_DPCD_TEST_LINK_RATE_TYPE_5_40G                        (0x00000014) /* R-XUV */
#define NV_DPCD_TEST_LINK_RATE_TYPE_8_10G                        (0x0000001E) /* R-XUV */

// 0021Ah - 0021Fh: RESERVED. Read all 0s

#define NV_DPCD_TEST_LANE_COUNT                                  (0x00000220) /* R-XUR */
#define NV_DPCD_TEST_LANE_COUNT_VALUE                                    4:0  /* R-XUF */

#define NV_DPCD_TEST_PATTERN                                     (0x00000221) /* R-XUR */
#define NV_DPCD_TEST_PATTERN_TYPE                                        1:0  /* R-XUF */
#define NV_DPCD_TEST_PATTERN_TYPE_NO                             (0x00000000) /* R-XUV */
#define NV_DPCD_TEST_PATTERN_TYPE_COLOR_RAMPS                    (0x00000001) /* R-XUV */
#define NV_DPCD_TEST_PATTERN_TYPE_BW_VERTICAL_LINES              (0x00000002) /* R-XUV */
#define NV_DPCD_TEST_PATTERN_TYPE_COLOR_SQUARES                  (0x00000003) /* R-XUV */

#define NV_DPCD_TEST_H_TOTAL_HIGH_BYTE                           (0x00000222) /* R-XUR */
#define NV_DPCD_TEST_H_TOTAL_LOW_BYTE                            (0x00000223) /* R-XUR */

#define NV_DPCD_TEST_V_TOTAL_HIGH_BYTE                           (0x00000224) /* R-XUR */
#define NV_DPCD_TEST_V_TOTAL_LOW_BYTE                            (0x00000225) /* R-XUR */

#define NV_DPCD_TEST_H_START_HIGH_BYTE                           (0x00000226) /* R-XUR */
#define NV_DPCD_TEST_H_START_LOW_BYTE                            (0x00000227) /* R-XUR */

#define NV_DPCD_TEST_V_START_HIGH_BYTE                           (0x00000228) /* R-XUR */
#define NV_DPCD_TEST_V_START_LOW_BYTE                            (0x00000229) /* R-XUR */

#define NV_DPCD_TEST_HSYNC_HIGH_BYTE                             (0x0000022A) /* R-XUR */
#define NV_DPCD_TEST_HSYNC_HIGH_BYTE_VALUE                               6:0  /* R-XUF */
#define NV_DPCD_TEST_HSYNC_HIGH_BYTE_POLARITY                            7:7  /* R-XUF */
#define NV_DPCD_TEST_HSYNC_LOW_BYTE                              (0x0000022B) /* R-XUR */

#define NV_DPCD_TEST_VSYNC_HIGH_BYTE                             (0x0000022C) /* R-XUR */
#define NV_DPCD_TEST_VSYNC_HIGH_BYTE_VALUE                               6:0  /* R-XUF */
#define NV_DPCD_TEST_VSYNC_HIGH_BYTE_POLARITY                            7:7  /* R-XUF */
#define NV_DPCD_TEST_VSYNC_LOW_BYTE                              (0x0000022D) /* R-XUR */

#define NV_DPCD_TEST_H_WIDTH_HIGH_BYTE                           (0x0000022E) /* R-XUR */
#define NV_DPCD_TEST_H_WIDTH_LOW_BYTE                            (0x0000022F) /* R-XUR */

#define NV_DPCD_TEST_V_HEIGHT_HIGH_BYTE                          (0x00000230) /* R-XUR */
#define NV_DPCD_TEST_V_HEIGHT_LOW_BYTE                           (0x00000231) /* R-XUR */

#define NV_DPCD_TEST_MISC0                                       (0x00000232) /* R-XUR */
#define NV_DPCD_TEST_MISC0_TEST_SYNC_CLOCK                               0:0  /* R-XUF */
#define NV_DPCD_TEST_MISC0_TEST_COLOR_FORMAT                             2:1  /* R-XUF */
#define NV_DPCD_TEST_MISC0_TEST_COLOR_FORMAT_RGB                 (0x00000000) /* R-XUV */
#define NV_DPCD_TEST_MISC0_TEST_COLOR_FORMAT_4_2_2               (0x00000001) /* R-XUV */
#define NV_DPCD_TEST_MISC0_TEST_COLOR_FORMAT_4_4_4               (0x00000002) /* R-XUV */
#define NV_DPCD_TEST_MISC0_TEST_COLOR_FORMAT_RESERVED            (0x00000003) /* R-XUV */
#define NV_DPCD_TEST_MISC0_TEST_DYNAMIC_RANGE                            3:3  /* R-XUF */
#define NV_DPCD_TEST_MISC0_TEST_YCBCR_COEFF                              4:4  /* R-XUF */
#define NV_DPCD_TEST_MISC0_TEST_BIT_DEPTH                                7:5  /* R-XUF */
#define NV_DPCD_TEST_MISC0_TEST_BIT_DEPTH_6BITS                  (0x00000000) /* R-XUV */
#define NV_DPCD_TEST_MISC0_TEST_BIT_DEPTH_8BITS                  (0x00000001) /* R-XUV */
#define NV_DPCD_TEST_MISC0_TEST_BIT_DEPTH_10BITS                 (0x00000002) /* R-XUV */
#define NV_DPCD_TEST_MISC0_TEST_BIT_DEPTH_12BITS                 (0x00000003) /* R-XUV */
#define NV_DPCD_TEST_MISC0_TEST_BIT_DEPTH_16BITS                 (0x00000004) /* R-XUV */

#define NV_DPCD_TEST_MISC1                                       (0x00000233) /* R-XUR */
#define NV_DPCD_TEST_MISC1_TEST_REFRESH_DENOMINATOR                      0:0  /* R-XUF */
#define NV_DPCD_TEST_MISC1_TEST_INTERLACED                               1:1  /* R-XUF */

#define NV_DPCD_TEST_REFRESH_RATE_NUMERATOR                      (0x00000234)  /* R-XUR */

// 00235h - 0023Fh: RESERVED for test automation extensions. Reads all 0s

#define NV_DPCD_TEST_CRC_R_Cr_LOW_BYTE                           (0x00000240) /* R-XUR */
#define NV_DPCD_TEST_CRC_R_Cr_HIGH_BYTE                          (0x00000241) /* R-XUR */

#define NV_DPCD_TEST_CRC_G_Y_LOW_BYTE                            (0x00000242) /* R-XUR */
#define NV_DPCD_TEST_CRC_G_Y_HIGH_BYTE                           (0x00000243) /* R-XUR */

#define NV_DPCD_TEST_CRC_B_Cb_LOW_BYTE                           (0x00000244) /* R-XUR */
#define NV_DPCD_TEST_CRC_B_Cb_HIGH_BYTE                          (0x00000245) /* R-XUR */

#define NV_DPCD_TEST_SINK_MISC                                   (0x00000246) /* R-XUR */
#define NV_DPCD_TEST_SINK_TEST_CRC_COUNT                                 3:0  /* R-XUF */
#define NV_DPCD_TEST_SINK_TEST_CRC_SUPPORTED                             5:5  /* R-XUF */
#define NV_DPCD_TEST_SINK_TEST_CRC_SUPPORTED_NO                  (0X00000000) /* R-XUV */
#define NV_DPCD_TEST_SINK_TEST_CRC_SUPPORTED_YES                 (0X00000001) /* R-XUV */

//00247h: RESERVED for test automation extensions. Reads all 0s

#define NV_DPCD_PHY_TEST_PATTERN                                 (0x00000248) /* R-XUR */
#define NV_DPCD_PHY_TEST_PATTERN_SEL_DP11                                1:0  /* R-XUF */
#define NV_DPCD_PHY_TEST_PATTERN_SEL_DP12                                2:0  /* R-XUF */
#define NV_DPCD_PHY_TEST_PATTERN_SEL_NO                          (0x00000000) /* R-XUV */
#define NV_DPCD_PHY_TEST_PATTERN_SEL_D10_2                       (0x00000001) /* R-XUV */
#define NV_DPCD_PHY_TEST_PATTERN_SEL_SYM_ERR_MEASUREMENT_CNT     (0x00000002) /* R-XUV */
#define NV_DPCD_PHY_TEST_PATTERN_SEL_PRBS7                       (0x00000003) /* R-XUV */
#define NV_DPCD_PHY_TEST_PATTERN_SEL_80_BIT_CUSTOM               (0x00000004) /* R-XUV */
#define NV_DPCD_PHY_TEST_PATTERN_SEL_HBR2_COMPLIANCE_EYE         (0x00000005) /* R-XUV */

#define NV_DPCD_HBR2_COMPLIANCE_SCRAMBLER_RESET_LOW_BYTE         (0x0000024A) /* R-XUV */
#define NV_DPCD_HBR2_COMPLIANCE_SCRAMBLER_RESET_HIGH_BYTE        (0x0000024B) /* R-XUV */

// 0024Ch - 0024Fh RESERVED for test automation extensions. Reads all 0s

#define NV_DPCD_TEST_80BIT_CUSTOM_PATTERN(i)                 (0x00000250+(i)) /* R--1A */
#define NV_DPCD_TEST_80BIT_CUSTOM_PATTERN__SIZE                           10  /* R---S */

// 0025Ah - 0025Fh: RESERVED for test automation extensions. Reads all 0s

#define NV_DPCD_TEST_RESPONSE                                    (0x00000260) /* RWXUR */
#define NV_DPCD_TEST_RESPONSE_TEST_ACK                                   0:0  /* RWXUF */
#define NV_DPCD_TEST_RESPONSE_TEST_ACK_NO                        (0x00000000) /* RWXUV */
#define NV_DPCD_TEST_RESPONSE_TEST_ACK_YES                       (0x00000001) /* RWXUV */
#define NV_DPCD_TEST_RESPONSE_TEST_NACK                                  1:1  /* RWXUF */
#define NV_DPCD_TEST_RESPONSE_TEST_NACK_NO                       (0x00000000) /* RWXUV */
#define NV_DPCD_TEST_RESPONSE_TEST_NACK_YES                      (0x00000001) /* RWXUV */
#define NV_DPCD_TEST_RESPONSE_TEST_EDID_CHKSUM_WRITE                     2:2  /* RWXUF */
#define NV_DPCD_TEST_RESPONSE_TEST_EDID_CHKSUM_WRITE_NO          (0x00000000) /* RWXUV */
#define NV_DPCD_TEST_RESPONSE_TEST_EDID_CHKSUM_WRITE_YES         (0x00000001) /* RWXUV */

#define NV_DPCD_TEST_EDID_CHKSUM                                 (0x00000261) /* RWXUR */

// 00263h - 0026Fh: RESERVED for test automation extensions Read all 0s.

#define NV_DPCD_TEST_SINK                                        (0x00000270) /* RWXUR */
#define NV_DPCD_TEST_SINK_START                                          0:0  /* RWXUF */
#define NV_DPCD_TEST_SINK_START_NO                               (0x00000000) /* RWXUV */
#define NV_DPCD_TEST_SINK_START_YES                              (0x00000001) /* RWXUV */
#define NV_DPCD_TEST_SINK_PHY_SINK_TEST_LANE_SEL                         5:4  /* RWXUF */
#define NV_DPCD_TEST_SINK_PHY_SINK_TEST_LANE_EN                          7:7  /* RWXUF */
#define NV_DPCD_TEST_SINK_PHY_SINK_TEST_LANE_EN_DISABLE          (0x00000000) /* RWXUV */
#define NV_DPCD_TEST_SINK_PHY_SINK_TEST_LANE_EN_ENABLE           (0x00000001) /* RWXUV */

#define NV_DPCD_TEST_AUDIO_MODE                                  (0x00000271) /* R-XUR */
#define NV_DPCD_TEST_AUDIO_MODE_SAMPLING_RATE                            3:0  /* R-XUF */
#define NV_DPCD_TEST_AUDIO_MODE_SAMPLING_RATE_32_0KHZ            (0x00000000) /* R-XUV */
#define NV_DPCD_TEST_AUDIO_MODE_SAMPLING_RATE_44_1KHZ            (0x00000001) /* R-XUV */
#define NV_DPCD_TEST_AUDIO_MODE_SAMPLING_RATE_48_0KHZ            (0x00000002) /* R-XUV */
#define NV_DPCD_TEST_AUDIO_MODE_SAMPLING_RATE_88_2KHZ            (0x00000003) /* R-XUV */
#define NV_DPCD_TEST_AUDIO_MODE_SAMPLING_RATE_96_0KHZ            (0x00000004) /* R-XUV */
#define NV_DPCD_TEST_AUDIO_MODE_SAMPLING_RATE_176_4KHZ           (0x00000005) /* R-XUV */
#define NV_DPCD_TEST_AUDIO_MODE_SAMPLING_RATE_192_0KHZ           (0x00000006) /* R-XUV */
#define NV_DPCD_TEST_AUDIO_MODE_CHANNEL_COUNT                            7:4  /* R-XUF */
#define NV_DPCD_TEST_AUDIO_MODE_CHANNEL_COUNT_1                  (0x00000000) /* R-XUV */
#define NV_DPCD_TEST_AUDIO_MODE_CHANNEL_COUNT_2                  (0x00000001) /* R-XUV */
#define NV_DPCD_TEST_AUDIO_MODE_CHANNEL_COUNT_3                  (0x00000002) /* R-XUV */
#define NV_DPCD_TEST_AUDIO_MODE_CHANNEL_COUNT_4                  (0x00000003) /* R-XUV */
#define NV_DPCD_TEST_AUDIO_MODE_CHANNEL_COUNT_5                  (0x00000004) /* R-XUV */
#define NV_DPCD_TEST_AUDIO_MODE_CHANNEL_COUNT_6                  (0x00000005) /* R-XUV */
#define NV_DPCD_TEST_AUDIO_MODE_CHANNEL_COUNT_7                  (0x00000006) /* R-XUV */
#define NV_DPCD_TEST_AUDIO_MODE_CHANNEL_COUNT_8                  (0x00000007) /* R-XUV */

#define NV_DPCD_TEST_AUDIO_PATTERN                               (0x00000272) /* R-XUR */
#define NV_DPCD_TEST_AUDIO_PATTERN_TYPE                                  7:0  /* R-XUF */
#define NV_DPCD_TEST_AUDIO_PATTERN_TYPE_OP_DEFINED               (0x00000000) /* R-XUV */
#define NV_DPCD_TEST_AUDIO_PATTERN_TYPE_SAWTOOTH                 (0x00000001) /* R-XUV */

#define NV_DPCD_TEST_AUDIO_PERIOD_CH(i)                      (0x00000273+(i)) /* R--1A */
#define NV_DPCD_TEST_AUDIO_PERIOD_CH__SIZE                                 8  /* R---S */
#define NV_DPCD_TEST_AUDIO_PERIOD_CH_SAMPLES                             3:0  /* R-XUF */
#define NV_DPCD_TEST_AUDIO_PERIOD_CH_SAMPLES_UNUSED              (0x00000000) /* R-XUV */
#define NV_DPCD_TEST_AUDIO_PERIOD_CH_SAMPLES_3                   (0x00000001) /* R-XUV */
#define NV_DPCD_TEST_AUDIO_PERIOD_CH_SAMPLES_6                   (0x00000002) /* R-XUV */
#define NV_DPCD_TEST_AUDIO_PERIOD_CH_SAMPLES_12                  (0x00000003) /* R-XUV */
#define NV_DPCD_TEST_AUDIO_PERIOD_CH_SAMPLES_24                  (0x00000004) /* R-XUV */
#define NV_DPCD_TEST_AUDIO_PERIOD_CH_SAMPLES_48                  (0x00000005) /* R-XUV */
#define NV_DPCD_TEST_AUDIO_PERIOD_CH_SAMPLES_96                  (0x00000006) /* R-XUV */
#define NV_DPCD_TEST_AUDIO_PERIOD_CH_SAMPLES_192                 (0x00000007) /* R-XUV */
#define NV_DPCD_TEST_AUDIO_PERIOD_CH_SAMPLES_384                 (0x00000008) /* R-XUV */
#define NV_DPCD_TEST_AUDIO_PERIOD_CH_SAMPLES_768                 (0x00000009) /* R-XUV */
#define NV_DPCD_TEST_AUDIO_PERIOD_CH_SAMPLES_1536                (0x0000000A) /* R-XUV */

// 0027Bh - 0027Fh: RESERVED. Read all 0s

// For DP version 1.3 and above
#define NV_DPCD_FEC_STATUS                                       (0x00000280) /* R-XUR */
#define NV_DPCD_FEC_STATUS_DECODE_EN                                     0:0  /* R-XUF */
#define NV_DPCD_FEC_STATUS_DECODE_EN_NOT_DETECTED                (0x00000000) /* R-XUV */
#define NV_DPCD_FEC_STATUS_DECODE_EN_DETECTED                    (0x00000001) /* R-XUV */
#define NV_DPCD_FEC_STATUS_DECODE_DIS                                    1:1  /* R-XUF */
#define NV_DPCD_FEC_STATUS_DECODE_DIS_NOT_DETECTED               (0x00000000) /* R-XUV */
#define NV_DPCD_FEC_STATUS_DECODE_DIS_DETECTED                   (0x00000001) /* R-XUV */


// 00283h - 002BFh: RESERVED. Read all 0s.

#define NV_DPCD_PAYLOAD_TABLE_UPDATE_STATUS                         (0x000002C0) /* R-XUR */
#define NV_DPCD_PAYLOAD_TABLE_UPDATE_STATUS_UPDATED                         0:0  /* R-XUF */
#define NV_DPCD_PAYLOAD_TABLE_UPDATE_STATUS_UPDATED_NO              (0x00000000) /* R-XUV */
#define NV_DPCD_PAYLOAD_TABLE_UPDATE_STATUS_UPDATED_YES             (0x00000001) /* R-XUV */
#define NV_DPCD_PAYLOAD_TABLE_UPDATE_STATUS_ACT_HANDLED                     1:1  /* R-XUF */
#define NV_DPCD_PAYLOAD_TABLE_UPDATE_STATUS_ACT_HANDLED_NO          (0x00000000) /* R-XUV */
#define NV_DPCD_PAYLOAD_TABLE_UPDATE_STATUS_ACT_HANDLED_YES         (0x00000001) /* R-XUV */
#define NV_DPCD_PAYLOAD_TABLE_UPDATE_STATUS_VC_PAYLOAD_ID_SLOT0_5_0         7:2  /* R-XUF */

#define NV_DPCD_VC_PAYLOAD_ID_SLOT1                                 (0x000002C1) /* R-XUR */
#define NV_DPCD_VC_PAYLOAD_ID_SLOT1_VC_PAYLOAD_ID_SLOT0_6                   7:7  /* R-XUF */

#define NV_DPCD_VC_PAYLOAD_ID_SLOT(i)                           (0x000002C1+(i)) /* R--1A */
#define NV_DPCD_VC_PAYLOAD_ID_SLOT__SIZE                                     63  /* R---S */

// Source Device-Specific Field, Burst write for 00300h-0030Bh
// 6 hex digits: 0x300~0x302.
#define NV_DPCD_SOURCE_IEEE_OUI                                  (0x00000300) /* RWXUR */
#define NV_DPCD_OUI_NVIDIA_LITTLE_ENDIAN                             0x4B0400

// 6 bytes: 0x303~0x308
#define NV_DPCD_SOURCE_DEV_ID_STRING(i)                      (0x00000303+(i)) /* RW-1A */
#define NV_DPCD_SOURCE_DEV_ID_STRING__SIZE                                 6  /* RW--S */

#define NV_DPCD_SOURCE_HARDWARE_REV                              (0x00000309) /* RWXUR */
#define NV_DPCD_SOURCE_HARDWARE_REV_MINOR                                3:0  /* RWXUF */
#define NV_DPCD_SOURCE_HARDWARE_REV_MAJOR                                7:4  /* RWXUF */

#define NV_DPCD_SOURCE_SOFTWARE_REV_MAJOR                        (0x0000030A) /* RWXUR */
#define NV_DPCD_SOURCE_SOFTWARE_REV_MINOR                        (0x0000030B) /* RWXUR */

// Sink Device-Specific Field. Read Only
// 6 hex digits: 0x400~0x402
#define NV_DPCD_SINK_IEEE_OUI                                    (0x00000400) /* R-XUR */

// 6 bytes: 0x403~0x408
#define NV_DPCD_SINK_DEV_ID_STRING(i)                        (0x00000403+(i)) /* R--1A */
#define NV_DPCD_SINK_DEV_ID_STRING__SIZE                                   6  /* R---S */

#define NV_DPCD_SINK_HARDWARE_REV                                (0x00000409) /* R-XUR */
#define NV_DPCD_SINK_HARDWARE_REV_MINOR                                  3:0  /* R-XUF */
#define NV_DPCD_SINK_HARDWARE_REV_MAJOR                                  7:4  /* R-XUF */

#define NV_DPCD_SINK_SOFTWARE_REV_MAJOR                          (0x0000040A) /* R-XUR */
#define NV_DPCD_SINK_SOFTWARE_REV_MINOR                          (0x0000040B) /* R-XUR */

// Branch Device-Specific Field
// 6 hex digits: 0x500~0x502

#define NV_DPCD_BRANCH_IEEE_OUI                                  (0x00000500) /* R-XUR */

// 6 bytes: 0x503~0x508
#define NV_DPCD_BRANCH_DEV_ID_STRING                         (0x00000503+(i)) /* R--1A */
#define NV_DPCD_BRANCH_DEV_ID_STRING__SIZE                                 6  /* R---S */

#define NV_DPCD_BRANCH_HARDWARE_REV                              (0x00000509) /* R-XUR */
#define NV_DPCD_BRANCH_HARDWARE_REV_MINOR                                3:0  /* R-XUF */
#define NV_DPCD_BRANCH_HARDWARE_REV_MAJOR                                7:4  /* R-XUF */

#define NV_DPCD_BRANCH_SOFTWARE_REV_MAJOR                        (0x0000050A) /* R-XUR */
#define NV_DPCD_BRANCH_SOFTWARE_REV_MINOR                        (0x0000050B) /* R-XUR */

// Sink Control Field
#define NV_DPCD_SET_POWER                                        (0x00000600) /* RWXUR */
#define NV_DPCD_SET_POWER_VAL                                            2:0  /* RWXUF */
#define NV_DPCD_SET_POWER_VAL_RESERVED                           (0x00000000) /* RWXUV */
#define NV_DPCD_SET_POWER_VAL_D0_NORMAL                          (0x00000001) /* RWXUV */
#define NV_DPCD_SET_POWER_VAL_D3_PWRDWN                          (0x00000002) /* RWXUV */
#define NV_DPCD_SET_POWER_VAL_D3_AUX_ON                          (0x00000005) /* RWXUV */

/*
 * 00601h - 006FFh: RESERVED. Read all 0s
 */

// * 00700h - 007FFh: RESERVED for eDP, see eDP v1.4 and above
#define NV_DPCD_EDP_REV                                             (0x00000700) /* R-XUR */
#define NV_DPCD_EDP_REV_VAL                                                 7:0  /* R-XUF */
#define NV_DPCD_EDP_REV_VAL_1_1_OR_LOWER                            (0x00000000) /* R-XUV */
#define NV_DPCD_EDP_REV_VAL_1_2                                     (0x00000001) /* R-XUV */
#define NV_DPCD_EDP_REV_VAL_1_3                                     (0x00000002) /* R-XUV */
#define NV_DPCD_EDP_REV_VAL_1_4                                     (0x00000003) /* R-XUV */
#define NV_DPCD_EDP_REV_VAL_1_4A                                    (0x00000004) /* R-XUV */
#define NV_DPCD_EDP_REV_VAL_1_4B                                    (0x00000005) /* R-XUV */
#define NV_DPCD_EDP_REV_VAL_1_5                                     (0x00000006) /* R-XUV */
#define NV_DPCD_EDP_REV_VAL_1_5A                                    (0x00000006) /* R-XUV */
#define NV_DPCD_EDP_GENERAL_CAP1                                    (0x00000701) /* R-XUR */
#define NV_DPCD_EDP_GENERAL_CAP1_TCON_BKLGHT_ADJUST_CAP                     0:0  /* R-XUF */
#define NV_DPCD_EDP_GENERAL_CAP1_TCON_BKLGHT_ADJUST_CAP_YES         (0x00000001) /* R-XUV */
#define NV_DPCD_EDP_GENERAL_CAP1_TCON_BKLGHT_ADJUST_CAP_NO          (0x00000000) /* R-XUV */
#define NV_DPCD_EDP_GENERAL_CAP1_BKLGHT_PIN_EN_CAP                          1:1  /* R-XUF */
#define NV_DPCD_EDP_GENERAL_CAP1_BKLGHT_PIN_EN_CAP_YES              (0x00000001) /* R-XUV */
#define NV_DPCD_EDP_GENERAL_CAP1_BKLGHT_PIN_EN_CAP_NO               (0x00000000) /* R-XUV */
#define NV_DPCD_EDP_GENERAL_CAP1_BKLGHT_AUX_EN_CAP                          2:2  /* R-XUF */
#define NV_DPCD_EDP_GENERAL_CAP1_BKLGHT_AUX_EN_CAP_YES              (0x00000001) /* R-XUV */
#define NV_DPCD_EDP_GENERAL_CAP1_BKLGHT_AUX_EN_CAP_NO               (0x00000000) /* R-XUV */
#define NV_DPCD_EDP_GENERAL_CAP1_PANEL_SELF_TEST_PIN_EN_CAP                 3:3  /* R-XUF */
#define NV_DPCD_EDP_GENERAL_CAP1_PANEL_SELF_TEST_PIN_EN_CAP_YES     (0x00000001) /* R-XUV */
#define NV_DPCD_EDP_GENERAL_CAP1_PANEL_SELF_TEST_PIN_EN_CAP_NO      (0x00000000) /* R-XUV */
#define NV_DPCD_EDP_GENERAL_CAP1_PANEL_SELF_TEST_AUX_EN_CAP                 4:4  /* R-XUF */
#define NV_DPCD_EDP_GENERAL_CAP1_PANEL_SELF_TEST_AUX_EN_CAP_YES     (0x00000001) /* R-XUV */
#define NV_DPCD_EDP_GENERAL_CAP1_PANEL_SELF_TEST_AUX_EN_CAP_NO      (0x00000000) /* R-XUV */
#define NV_DPCD_EDP_GENERAL_CAP1_FRC_EN_CAP                                 5:5  /* R-XUF */
#define NV_DPCD_EDP_GENERAL_CAP1_FRC_EN_CAP_YES                     (0x00000001) /* R-XUV */
#define NV_DPCD_EDP_GENERAL_CAP1_FRC_EN_CAP_NO                      (0x00000000) /* R-XUV */
#define NV_DPCD_EDP_GENERAL_CAP1_COLOR_ENGINE_CAP                           6:6  /* R-XUF */
#define NV_DPCD_EDP_GENERAL_CAP1_COLOR_ENGINE_CAP_YES               (0x00000001) /* R-XUV */
#define NV_DPCD_EDP_GENERAL_CAP1_COLOR_ENGINE_CAP_NO                (0x00000000) /* R-XUV */
#define NV_DPCD_EDP_GENERAL_CAP1_SET_POWER_CAP                              7:7  /* R-XUF */
#define NV_DPCD_EDP_GENERAL_CAP1_SET_POWER_CAP_YES                  (0x00000001) /* R-XUV */
#define NV_DPCD_EDP_GENERAL_CAP1_SET_POWER_CAP_NO                   (0x00000000) /* R-XUV */
#define NV_DPCD_EDP_BKLGHT_ADJUST_CAP                               (0x00000702) /* R-XUR */
#define NV_DPCD_EDP_BKLGHT_ADJUST_CAP_BRIGHT_PWM_PIN_CAP                    0:0  /* R-XUF */
#define NV_DPCD_EDP_BKLGHT_ADJUST_CAP_BRIGHT_PWM_PIN_CAP_YES        (0x00000001) /* R-XUV */
#define NV_DPCD_EDP_BKLGHT_ADJUST_CAP_BRIGHT_PWM_PIN_CAP_NO         (0x00000000) /* R-XUV */
#define NV_DPCD_EDP_BKLGHT_ADJUST_CAP_BRIGHT_AUX_SET_CAP                    1:1  /* R-XUF */
#define NV_DPCD_EDP_BKLGHT_ADJUST_CAP_BRIGHT_AUX_SET_CAP_YES        (0x00000001) /* R-XUV */
#define NV_DPCD_EDP_BKLGHT_ADJUST_CAP_BRIGHT_AUX_SET_CAP_NO         (0x00000000) /* R-XUV */
#define NV_DPCD_EDP_BKLGHT_ADJUST_CAP_BRIGHT_AUX_BYTE_CNT                   2:2  /* R-XUF */
#define NV_DPCD_EDP_BKLGHT_ADJUST_CAP_BRIGHT_AUX_BYTE_CNT_2B        (0x00000001) /* R-XUV */
#define NV_DPCD_EDP_BKLGHT_ADJUST_CAP_BRIGHT_AUX_BYTE_CNT_1B        (0x00000000) /* R-XUV */
#define NV_DPCD_EDP_BKLGHT_ADJUST_CAP_AUX_PWM_PRODUCT_CAP                   3:3  /* R-XUF */
#define NV_DPCD_EDP_BKLGHT_ADJUST_CAP_AUX_PWM_PRODUCT_CAP_YES       (0x00000001) /* R-XUV */
#define NV_DPCD_EDP_BKLGHT_ADJUST_CAP_AUX_PWM_PRODUCT_CAP_NO        (0x00000000) /* R-XUV */
#define NV_DPCD_EDP_BKLGHT_ADJUST_CAP_FREQ_PWM_PIN_PASSTHRU_CAP             4:4  /* R-XUF */
#define NV_DPCD_EDP_BKLGHT_ADJUST_CAP_FREQ_PWM_PIN_PASSTHRU_CAP_YES (0x00000001) /* R-XUV */
#define NV_DPCD_EDP_BKLGHT_ADJUST_CAP_FREQ_PWM_PIN_PASSTHRU_CAP_NO  (0x00000000) /* R-XUV */
#define NV_DPCD_EDP_BKLGHT_ADJUST_CAP_FREQ_AUX_SET_CAP                      5:5  /* R-XUF */
#define NV_DPCD_EDP_BKLGHT_ADJUST_CAP_FREQ_AUX_SET_CAP_YES          (0x00000001) /* R-XUV */
#define NV_DPCD_EDP_BKLGHT_ADJUST_CAP_FREQ_AUX_SET_CAP_NO           (0x00000000) /* R-XUV */
#define NV_DPCD_EDP_BKLGHT_ADJUST_CAP_DYNAMIC_BKLGHT_CAP                    6:6  /* R-XUF */
#define NV_DPCD_EDP_BKLGHT_ADJUST_CAP_DYNAMIC_BKLGHT_CAP_YES        (0x00000001) /* R-XUV */
#define NV_DPCD_EDP_BKLGHT_ADJUST_CAP_DYNAMIC_BKLGHT_CAP_NO         (0x00000000) /* R-XUV */
#define NV_DPCD_EDP_BKLGHT_ADJUST_CAP_VBLANK_BKLGHT_UPDATE_CAP              7:7  /* R-XUF */
#define NV_DPCD_EDP_BKLGHT_ADJUST_CAP_VBLANK_BKLGHT_UPDATE_CAP_VBL  (0x00000001) /* R-XUV */
#define NV_DPCD_EDP_BKLGHT_ADJUST_CAP_VBLANK_BKLGHT_UPDATE_CAP_IMM  (0x00000000) /* R-XUV */
#define NV_DPCP_EDP_GENERAL_CAP2                                    (0x00000703) /* R-XUR */
#define NV_DPCD_EDP_GENERAL_CAP2_OVERDRIVE_ENGINE_CAP                       0:0  /* R-XUF */
#define NV_DPCD_EDP_GENERAL_CAP2_OVERDRIVE_ENGINE_CAP_YES           (0x00000001) /* R-XUV */
#define NV_DPCD_EDP_GENERAL_CAP2_OVERDRIVE_ENGINE_CAP_NO            (0x00000000) /* R-XUV */
#define NV_DPCD_EDP_GENERAL_CAP2_BKLGHT_BRIGHT_BIT_ALIGNMENT                2:1  /* R-XUF */
#define NV_DPCD_EDP_GENERAL_CAP2_BKLGHT_BRIGHT_BIT_ALIGNMENT_NO     (0x00000000) /* R-XUV */
#define NV_DPCD_EDP_GENERAL_CAP2_BKLGHT_BRIGHT_BIT_ALIGNMENT_MSB    (0x00000001) /* R-XUV */
#define NV_DPCD_EDP_GENERAL_CAP2_BKLGHT_BRIGHT_BIT_ALIGNMENT_LSB    (0x00000002) /* R-XUV */
#define NV_DPCD_EDP_GENERAL_CAP2_OVERDRIVE_CONTROL_CAP                      3:3  /* R-XUF */
#define NV_DPCD_EDP_GENERAL_CAP2_OVERDRIVE_CONTROL_CAP_NO           (0x00000000) /* R-XUV */
#define NV_DPCD_EDP_GENERAL_CAP2_OVERDRIVE_CONTROL_CAP_YES          (0x00000001) /* R-XUV */
#define NV_DPCD_EDP_GENERAL_CAP2_PANEL_LUMINANCE_CONTROL_CAP                4:4  /* R-XUF */
#define NV_DPCD_EDP_GENERAL_CAP2_PANEL_LUMINANCE_CONTROL_CAP_NO     (0x00000000) /* R-XUV */
#define NV_DPCD_EDP_GENERAL_CAP2_PANEL_LUMINANCE_CONTROL_CAP_YES    (0x00000001) /* R-XUV */
#define NV_DPCD_EDP_GENERAL_CAP2_VARIABLE_BKLGHT_CONTROL_CAP                5:5  /* R-XUF */
#define NV_DPCD_EDP_GENERAL_CAP2_VARIABLE_BKLGHT_CONTROL_CAP_NO     (0x00000000) /* R-XUV */
#define NV_DPCD_EDP_GENERAL_CAP2_VARIABLE_BKLGHT_CONTROL_CAP_YES    (0x00000001) /* R-XUV */
#define NV_DPCD_EDP_GENERAL_CAP3                                    (0x00000704) /* R-XUV */
#define NV_DPCD_EDP_GENERAL_CAP3_X_REGION_CAP                               3:0  /* R-XUF */
#define NV_DPCD_EDP_GENERAL_CAP3_X_REGION_CAP_NOT_SUPPORTED         (0x00000000) /* R-XUV */
#define NV_DPCD_EDP_GENERAL_CAP3_Y_REGION_CAP                               7:4  /* R-XUF */
#define NV_DPCD_EDP_GENERAL_CAP3_Y_REGION_CAP_NOT_SUPPORTED         (0x00000000) /* R-XUV */
#define NV_DPCD_EDP_DISPLAY_CTL                                     (0x00000720) /* RWXUR */
#define NV_DPCD_EDP_DISPLAY_CTL_BKLGHT_EN                                   0:0  /* RWXUF */
#define NV_DPCD_EDP_DISPLAY_CTL_BKLGHT_EN_INIT                      (0x00000000) /* RWXUV */
#define NV_DPCD_EDP_DISPLAY_CTL_BKLGHT_EN_ENABLED                   (0x00000001) /* RWXUV */
#define NV_DPCD_EDP_DISPLAY_CTL_BKLGHT_EN_DISABLED                  (0x00000000) /* RWXUV */
#define NV_DPCD_EDP_DISPLAY_CTL_BLACK_VIDEO_EN                              1:1  /* RWXUF */
#define NV_DPCD_EDP_DISPLAY_CTL_BLACK_VIDEO_EN_INIT                 (0x00000000) /* RWXUV */
#define NV_DPCD_EDP_DISPLAY_CTL_BLACK_VIDEO_EN_ENABLED              (0x00000001) /* RWXUV */
#define NV_DPCD_EDP_DISPLAY_CTL_BLACK_VIDEO_EN_DISABLED             (0x00000000) /* RWXUV */
#define NV_DPCD_EDP_DISPLAY_CTL_FRC_EN                                      2:2  /* RWXUF */
#define NV_DPCD_EDP_DISPLAY_CTL_FRC_EN_2BIT                         (0x00000001) /* RWXUV */
#define NV_DPCD_EDP_DISPLAY_CTL_COLOR_ENGINE_EN                             3:3  /* RWXUF */
#define NV_DPCD_EDP_DISPLAY_CTL_COLOR_ENGINE_EN_INIT                (0x00000000) /* RWXUV */
#define NV_DPCD_EDP_DISPLAY_CTL_COLOR_ENGINE_EN_ENABLED             (0x00000001) /* RWXUV */
#define NV_DPCD_EDP_DISPLAY_CTL_COLOR_ENGINE_EN_DISABLED            (0x00000000) /* RWXUV */
#define NV_DPCD_EDP_DISPLAY_CTL_OVERDRIVE_CTL                               5:4  /* RWXUF */
#define NV_DPCD_EDP_DISPLAY_CTL_OVERDRIVE_CTL_AUTONOMOUS            (0x00000000) /* RWXUV */
#define NV_DPCD_EDP_DISPLAY_CTL_OVERDRIVE_CTL_AUTONOMOUS_1          (0x00000001) /* RWXUV */
#define NV_DPCD_EDP_DISPLAY_CTL_OVERDRIVE_CTL_DISABLE               (0x00000002) /* RWXUV */
#define NV_DPCD_EDP_DISPLAY_CTL_OVERDRIVE_CTL_ENABLE                (0x00000003) /* RWXUV */
#define NV_DPCD_EDP_DISPLAY_CTL_VARIABLE_BKLGHT_CTRL_DISABLE                6:6  /* RWXUF */
#define NV_DPCD_EDP_DISPLAY_CTL_VARIABLE_BKLGHT_CTRL_DISABLE_YES    (0x00000001) /* RWXUV */
#define NV_DPCD_EDP_DISPLAY_CTL_VARIABLE_BKLGHT_CTRL_DISABLE_NO     (0x00000000) /* RWXUV */
#define NV_DPCD_EDP_DISPLAY_CTL_VBLANK_BKLGHT_UPDATE_EN                     7:7  /* RWXUF */
#define NV_DPCD_EDP_DISPLAY_CTL_VBLANK_BKLGHT_UPDATE_EN_ENABLED     (0x00000001) /* RWXUV */
#define NV_DPCD_EDP_DISPLAY_CTL_VBLANK_BKLGHT_UPDATE_EN_DISABLED    (0x00000000) /* RWXUV */
#define NV_DPCD_EDP_BKLGHT_MODE_SET                                 (0x00000721) /* RWXUR */
#define NV_DPCD_EDP_BKLGHT_MODE_SET_BRIGHT_CTL_MODE                         1:0  /* RWXUF */
#define NV_DPCD_EDP_BKLGHT_MODE_SET_BRIGHT_CTL_MODE_INIT            (0x00000000) /* RWXUV */
#define NV_DPCD_EDP_BKLGHT_MODE_SET_BRIGHT_CTL_MODE_PWM_PIN         (0x00000000) /* RWXUV */
#define NV_DPCD_EDP_BKLGHT_MODE_SET_BRIGHT_CTL_MODE_PRESET_LV       (0x00000001) /* RWXUV */
#define NV_DPCD_EDP_BKLGHT_MODE_SET_BRIGHT_CTL_MODE_AUX             (0x00000002) /* RWXUV */
#define NV_DPCD_EDP_BKLGHT_MODE_SET_BRIGHT_CTL_MODE_PWM_AND_AUX     (0x00000003) /* RWXUV */
#define NV_DPCD_EDP_BKLGHT_MODE_SET_FREQ_PWM_PIN_PASSTHRU_EN                2:2  /* RWXUF */
#define NV_DPCD_EDP_BKLGHT_MODE_SET_FREQ_PWM_PIN_PASSTHRU_EN_INIT     (0x00000000) /* RWXUV */
#define NV_DPCD_EDP_BKLGHT_MODE_SET_FREQ_PWM_PIN_PASSTHRU_EN_ENABLED  (0x00000001) /* RWXUV */
#define NV_DPCD_EDP_BKLGHT_MODE_SET_FREQ_PWM_PIN_PASSTHRU_EN_DISABLED (0x00000000) /* RWXUV */
#define NV_DPCD_EDP_BKLGHT_MODE_SET_FREQ_AUX_SET_EN                         3:3  /* RWXUF */
#define NV_DPCD_EDP_BKLGHT_MODE_SET_FREQ_AUX_SET_EN_INIT            (0x00000000) /* RWXUV */
#define NV_DPCD_EDP_BKLGHT_MODE_SET_FREQ_AUX_SET_EN_ENABLED         (0x00000001) /* RWXUV */
#define NV_DPCD_EDP_BKLGHT_MODE_SET_FREQ_AUX_SET_EN_DISABLED        (0x00000000) /* RWXUV */
#define NV_DPCD_EDP_BKLGHT_MODE_SET_DYNAMIC_BKLGHT_EN                       4:4  /* RWXUF */
#define NV_DPCD_EDP_BKLGHT_MODE_SET_DYNAMIC_BKLGHT_EN_INIT          (0x00000000) /* RWXUV */
#define NV_DPCD_EDP_BKLGHT_MODE_SET_DYNAMIC_BKLGHT_EN_ENABLED       (0x00000001) /* RWXUV */
#define NV_DPCD_EDP_BKLGHT_MODE_SET_DYNAMIC_BKLGHT_EN_DISABLED      (0x00000000) /* RWXUV */
#define NV_DPCD_EDP_BKLGHT_MODE_SET_REGIONAL_BKLGHT_EN                      5:5  /* RWXUF */
#define NV_DPCD_EDP_BKLGHT_MODE_SET_REGIONAL_BKLGHT_EN_INIT         (0x00000000) /* RWXUV */
#define NV_DPCD_EDP_BKLGHT_MODE_SET_REGIONAL_BKLGHT_EN_ENABLED      (0x00000001) /* RWXUV */
#define NV_DPCD_EDP_BKLGHT_MODE_SET_REGIONAL_BKLGHT_EN_DISABLED     (0x00000000) /* RWXUV */
#define NV_DPCD_EDP_BKLGHT_MODE_SET_UPDATE_REGION_BRIGHTNESS                6:6  /* RWXUF */
#define NV_DPCD_EDP_BKLGHT_MODE_SET_UPDATE_REGION_BRIGHTNESS_ENABLED  (0x00000001) /* RWXUV */
#define NV_DPCD_EDP_BKLGHT_MODE_SET_UPDATE_REGION_BRIGHTNESS_DISABLED (0x00000000) /* RWXUV */
#define NV_DPCD_EDP_BKLGHT_MODE_SET_PANEL_LUMINANCE_CONTROL_ENABLE          7:7  /* RWXUF */
#define NV_DPCD_EDP_BKLGHT_MODE_SET_PANEL_LUMINANCE_CONTROL_ENABLE_YES (0x00000001) /* RWXUV */
#define NV_DPCD_EDP_BKLGHT_MODE_SET_PANEL_LUMINANCE_CONTROL_ENABLE_NO  (0x00000000) /* RWXUV */
#define NV_DPCD_EDP_BKLGHT_BRIGHTNESS_MSB                           (0x00000722) /* RWXUR */
#define NV_DPCD_EDP_BKLGHT_BRIGHTNESS_MSB_VAL                               7:0  /* RWXUF */
#define NV_DPCD_EDP_BKLGHT_BRIGHTNESS_MSB_VAL_INIT                  (0x00000000) /* RWXUV */
#define NV_DPCD_EDP_BKLGHT_BRIGHTNESS_LSB                           (0x00000723) /* RWXUR */
#define NV_DPCD_EDP_BKLGHT_BRIGHTNESS_LSB_VAL                               7:0  /* RWXUF */
#define NV_DPCD_EDP_BKLGHT_BRIGHTNESS_LSB_VAL_INIT                  (0x00000000) /* RWXUV */
#define NV_DPCD_EDP_PWMGEN_BIT_CNT                                  (0x00000724) /* RWXUR */
#define NV_DPCD_EDP_PWMGEN_BIT_CNT_VAL                                      4:0  /* RWXUF */
#define NV_DPCD_EDP_PWMGEN_BIT_CNT_VAL_INIT                         (0x00000000) /* RWXUV */
#define NV_DPCD_EDP_PWMGEN_BIT_CNT_CAP_MIN                          (0x00000725) /* R-XUR */
#define NV_DPCD_EDP_PWMGEN_BIT_CNT_CAP_MIN_VAL                              4:0  /* R-XUF */
#define NV_DPCD_EDP_PWMGEN_BIT_CNT_CAP_MIN_VAL_INIT                 (0x00000000) /* R-XUV */
#define NV_DPCD_EDP_PWMGEN_BIT_CNT_CAP_MAX                          (0x00000726) /* R-XUR */
#define NV_DPCD_EDP_PWMGEN_BIT_CNT_CAP_MAX_VAL                              4:0  /* R-XUF */
#define NV_DPCD_EDP_PWMGEN_BIT_CNT_CAP_MAX_VAL_INIT                 (0x00000000) /* R-XUV */
#define NV_DPCD_EDP_BKLGHT_CTL_STATUS                               (0x00000727) /* R-XUR */
#define NV_DPCD_EDP_BKLGHT_CTL_STATUS_FAULT_CONDITION                       0:0  /* R-XUF */
#define NV_DPCD_EDP_BKLGHT_CTL_STATUS_FAULT_CONDITION_INIT          (0x00000000) /* R-XUV */
#define NV_DPCD_EDP_BKLGHT_CTL_STATUS_FAULT_CONDITION_FAULT         (0x00000001) /* R-XUV */
#define NV_DPCD_EDP_BKLGHT_CTL_STATUS_FAULT_CONDITION_NORMAL        (0x00000000) /* R-XUV */
#define NV_DPCD_EDP_BKLGHT_CTL_STATUS_VARIABLE_BKLGHT_STATUS                   1:1  /* R-XUF */
#define NV_DPCD_EDP_BKLGHT_CTL_STATUS_VARIABLE_BKLGHT_STATUS_DISABLED  (0x00000000) /* R-XUV */
#define NV_DPCD_EDP_BKLGHT_CTL_STATUS_VARIABLE_BKLGHT_STATUS_ENABLED   (0x00000001) /* R-XUV */
#define NV_DPCD_EDP_BKLGHT_FREQ_SET                                 (0x00000728) /* RWXUR */
#define NV_DPCD_EDP_BKLGHT_FREQ_SET_VAL                                     7:0  /* RWXUF */
#define NV_DPCD_EDP_BKLGHT_FREQ_SET_VAL_INIT                        (0x00000000) /* RWXUV */
#define NV_DPCD_EDP_BKLGHT_FREQ_CAP_MIN_MSB                         (0x0000072A) /* R-XUR */
#define NV_DPCD_EDP_BKLGHT_FREQ_CAP_MIN_MSB_VAL                             7:0  /* R-XUF */
#define NV_DPCD_EDP_BKLGHT_FREQ_CAP_MIN_MSB_VAL_INIT                (0x00000000) /* R-XUV */
#define NV_DPCD_EDP_BKLGHT_FREQ_CAP_MIN_MID                         (0x0000072B) /* R-XUR */
#define NV_DPCD_EDP_BKLGHT_FREQ_CAP_MIN_MID_VAL                             7:0  /* R-XUF */
#define NV_DPCD_EDP_BKLGHT_FREQ_CAP_MIN_MID_VAL_INIT                (0x00000000) /* R-XUV */
#define NV_DPCD_EDP_BKLGHT_FREQ_CAP_MIN_LSB                         (0x0000072C) /* R-XUR */
#define NV_DPCD_EDP_BKLGHT_FREQ_CAP_MIN_LSB_VAL                             1:0  /* R-XUF */
#define NV_DPCD_EDP_BKLGHT_FREQ_CAP_MIN_LSB_VAL_INIT                (0x00000000) /* R-XUV */
#define NV_DPCD_EDP_BKLGHT_FREQ_CAP_MAX_MSB                         (0x0000072D) /* R-XUR */
#define NV_DPCD_EDP_BKLGHT_FREQ_CAP_MAX_MSB_VAL                             7:0  /* R-XUF */
#define NV_DPCD_EDP_BKLGHT_FREQ_CAP_MAX_MSB_VAL_INIT                (0x00000000) /* R-XUV */
#define NV_DPCD_EDP_BKLGHT_FREQ_CAP_MAX_MID                         (0x0000072E) /* R-XUR */
#define NV_DPCD_EDP_BKLGHT_FREQ_CAP_MAX_MID_VAL                             7:0  /* R-XUF */
#define NV_DPCD_EDP_BKLGHT_FREQ_CAP_MAX_MID_VAL_INIT                (0x00000000) /* R-XUV */
#define NV_DPCD_EDP_BKLGHT_FREQ_CAP_MAX_LSB                         (0x0000072F) /* R-XUR */
#define NV_DPCD_EDP_BKLGHT_FREQ_CAP_MAX_LSB_VAL                             1:0  /* R-XUF */
#define NV_DPCD_EDP_BKLGHT_FREQ_CAP_MAX_LSB_VAL_INIT                (0x00000000) /* R-XUV */
#define NV_DPCD_EDP_DBC_MINIMUM_BRIGHTNESS_SET                      (0x00000732) /* RWXUR */
#define NV_DPCD_EDP_DBC_MINIMUM_BRIGHTNESS_SET_VAL                          4:0  /* RWXUF */
#define NV_DPCD_EDP_DBC_MINIMUM_BRIGHTNESS_SET_VAL_INIT             (0x00000000) /* RWXUV */
#define NV_DPCD_EDP_DBC_MAXIMUM_BRIGHTNESS_SET                      (0x00000733) /* RWXUR */
#define NV_DPCD_EDP_DBC_MAXIMUM_BRIGHTNESS_CAP_VAL                          4:0  /* RWXUF */
#define NV_DPCD_EDP_DBC_MAXIMUM_BRIGHTNESS_CAP_VAL_INIT             (0x00000000) /* RWXUV */
#define NV_DPCD_PANEL_TARGET_LUMINANCE_LSB                          (0x00000734) /* RWXUR */
#define NV_DPCD_PANEL_TARGET_LUMINANCE_LSB_VAL                              7:0  /* RWXUF */
#define NV_DPCD_PANEL_TARGET_LUMINANCE_LSB_VAL_INIT                 (0x00000000) /* RWXUV */
#define NV_DPCD_PANEL_TARGET_LUMINANCE_MID                          (0x00000735) /* RWXUR */
#define NV_DPCD_PANEL_TARGET_LUMINANCE_MID_VAL                              7:0  /* RWXUF */
#define NV_DPCD_PANEL_TARGET_LUMINANCE_MID_VAL_INIT                  0x00000000) /* RWXUV */
#define NV_DPCD_PANEL_TARGET_LUMINANCE_MSB                          (0x00000736) /* RWXUR */
#define NV_DPCD_PANEL_TARGET_LUMINANCE_MSB_VAL                              7:0  /* RWXUF */
#define NV_DPCD_PANEL_TARGET_LUMINANCE_MSB_VAL_INIT                  0x00000000) /* RWXUV */
#define NV_DPCD_EDP_REGIONAL_BKLGHT_BASE                            (0x00000740) /* RWXUR */
#define NV_DPCD_EDP_REGIONAL_BKLGHT_BASE_INDEX_OFFSET_VAL                   7:0  /* RWXUF */
#define NV_DPCD_EDP_REGIONAL_BKLGHT_BASE_INDEX_OFFSET_VAL_INIT      (0x00000000) /* RWXUV */
#define NV_DPCD_EDP_REGIONAL_BACKLIGHT_BRIGHTNESS_0                 (0x00000741) /* RWXUR */
#define NV_DPCD_EDP_REGIONAL_BACKLIGHT_BRIGHTNESS_0_VAL                     7:0  /* RWXUF */
#define NV_DPCD_EDP_REGIONAL_BACKLIGHT_BRIGHTNESS_0_VAL_INIT        (0x00000000) /* RWXUV */
#define NV_DPCD_EDP_REGIONAL_BACKLIGHT_BRIGHTNESS_1                 (0x00000742) /* RWXUR */
#define NV_DPCD_EDP_REGIONAL_BACKLIGHT_BRIGHTNESS_1_VAL                     7:0  /* RWXUF */
#define NV_DPCD_EDP_REGIONAL_BACKLIGHT_BRIGHTNESS_1_VAL_INIT        (0x00000000) /* RWXUV */
#define NV_DPCD_EDP_REGIONAL_BACKLIGHT_BRIGHTNESS_2                 (0x00000743) /* RWXUR */
#define NV_DPCD_EDP_REGIONAL_BACKLIGHT_BRIGHTNESS_2_VAL                     7:0  /* RWXUF */
#define NV_DPCD_EDP_REGIONAL_BACKLIGHT_BRIGHTNESS_2_VAL_INIT        (0x00000000) /* RWXUV */
#define NV_DPCD_EDP_REGIONAL_BACKLIGHT_BRIGHTNESS_3                 (0x00000744) /* RWXUR */
#define NV_DPCD_EDP_REGIONAL_BACKLIGHT_BRIGHTNESS_3_VAL                     7:0  /* RWXUF */
#define NV_DPCD_EDP_REGIONAL_BACKLIGHT_BRIGHTNESS_3_VAL_INIT        (0x00000000) /* RWXUV */
#define NV_DPCD_EDP_REGIONAL_BACKLIGHT_BRIGHTNESS_4                 (0x00000745) /* RWXUR */
#define NV_DPCD_EDP_REGIONAL_BACKLIGHT_BRIGHTNESS_4_VAL                     7:0  /* RWXUF */
#define NV_DPCD_EDP_REGIONAL_BACKLIGHT_BRIGHTNESS_4_VAL_INIT        (0x00000000) /* RWXUV */
#define NV_DPCD_EDP_REGIONAL_BACKLIGHT_BRIGHTNESS_5                 (0x00000746) /* RWXUR */
#define NV_DPCD_EDP_REGIONAL_BACKLIGHT_BRIGHTNESS_5_VAL                     7:0  /* RWXUF */
#define NV_DPCD_EDP_REGIONAL_BACKLIGHT_BRIGHTNESS_5_VAL_INIT        (0x00000000) /* RWXUV */
#define NV_DPCD_EDP_REGIONAL_BACKLIGHT_BRIGHTNESS_6                 (0x00000747) /* RWXUR */
#define NV_DPCD_EDP_REGIONAL_BACKLIGHT_BRIGHTNESS_6_VAL                     7:0  /* RWXUF */
#define NV_DPCD_EDP_REGIONAL_BACKLIGHT_BRIGHTNESS_6_VAL_INIT        (0x00000000) /* RWXUV */
#define NV_DPCD_EDP_REGIONAL_BACKLIGHT_BRIGHTNESS_7                 (0x00000748) /* RWXUR */
#define NV_DPCD_EDP_REGIONAL_BACKLIGHT_BRIGHTNESS_7_VAL                     7:0  /* RWXUF */
#define NV_DPCD_EDP_REGIONAL_BACKLIGHT_BRIGHTNESS_7_VAL_INIT        (0x00000000) /* RWXUV */
#define NV_DPCD_EDP_REGIONAL_BACKLIGHT_BRIGHTNESS_8                 (0x00000749) /* RWXUR */
#define NV_DPCD_EDP_REGIONAL_BACKLIGHT_BRIGHTNESS_8_VAL                     7:0  /* RWXUF */
#define NV_DPCD_EDP_REGIONAL_BACKLIGHT_BRIGHTNESS_8_VAL_INIT        (0x00000000) /* RWXUV */
#define NV_DPCD_EDP_REGIONAL_BACKLIGHT_BRIGHTNESS_9                 (0x0000074A) /* RWXUR */
#define NV_DPCD_EDP_REGIONAL_BACKLIGHT_BRIGHTNESS_9_VAL                     7:0  /* RWXUF */
#define NV_DPCD_EDP_REGIONAL_BACKLIGHT_BRIGHTNESS_9_VAL_INIT        (0x00000000) /* RWXUV */
#define NV_DPCD_EDP_REGIONAL_BACKLIGHT_BRIGHTNESS_10                (0x0000074B) /* RWXUR */
#define NV_DPCD_EDP_REGIONAL_BACKLIGHT_BRIGHTNESS_10_VAL                    7:0  /* RWXUF */
#define NV_DPCD_EDP_REGIONAL_BACKLIGHT_BRIGHTNESS_10_VAL_INIT       (0x00000000) /* RWXUV */
#define NV_DPCD_EDP_REGIONAL_BACKLIGHT_BRIGHTNESS_11                (0x0000074C) /* RWXUR */
#define NV_DPCD_EDP_REGIONAL_BACKLIGHT_BRIGHTNESS_11_VAL                    7:0  /* RWXUF */
#define NV_DPCD_EDP_REGIONAL_BACKLIGHT_BRIGHTNESS_11_VAL_INIT       (0x00000000) /* RWXUV */
#define NV_DPCD_EDP_REGIONAL_BACKLIGHT_BRIGHTNESS_12                (0x0000074D) /* RWXUR */
#define NV_DPCD_EDP_REGIONAL_BACKLIGHT_BRIGHTNESS_12_VAL                    7:0  /* RWXUF */
#define NV_DPCD_EDP_REGIONAL_BACKLIGHT_BRIGHTNESS_12_VAL_INIT       (0x00000000) /* RWXUV */
#define NV_DPCD_EDP_REGIONAL_BACKLIGHT_BRIGHTNESS_13                (0x0000074E) /* RWXUR */
#define NV_DPCD_EDP_REGIONAL_BACKLIGHT_BRIGHTNESS_13_VAL                    7:0  /* RWXUF */
#define NV_DPCD_EDP_REGIONAL_BACKLIGHT_BRIGHTNESS_13_VAL_INIT       (0x00000000) /* RWXUV */
#define NV_DPCD_EDP_REGIONAL_BACKLIGHT_BRIGHTNESS_14                (0x0000074F) /* RWXUR */
#define NV_DPCD_EDP_REGIONAL_BACKLIGHT_BRIGHTNESS_14_VAL                    7:0  /* RWXUF */
#define NV_DPCD_EDP_REGIONAL_BACKLIGHT_BRIGHTNESS_14_VAL_INIT       (0x00000000) /* RWXUV */

/*
 * 00800h - 00FFFh: RESERVED. Read all 0s
 */

// Sideband MSG Buffers
#define NV_DPCD_MBOX_DOWN_REQ                                    (0x00001000) /* RWXUR */
#define NV_DPCD_MBOX_UP_REP                                      (0x00001200) /* RWXUR */
#define NV_DPCD_MBOX_DOWN_REP                                    (0x00001400) /* R-XUR */
#define NV_DPCD_MBOX_UP_REQ                                      (0x00001600) /* R-XUR */

// 0x2000 & 0x2001 : RESERVED for USB-over-AUX

// ESI (Event Status Indicator) Field
#define NV_DPCD_SINK_COUNT_ESI                                   (0x00002002) /* R-XUR */
#define NV_DPCD_SINK_COUNT_ESI_SINK_COUNT                                5:0  /* R-XUF */
#define NV_DPCD_SINK_COUNT_ESI_CP_READY                                  6:6  /* R-XUF */
#define NV_DPCD_SINK_COUNT_ESI_CP_READY_NO                       (0x00000000) /* R-XUV */
#define NV_DPCD_SINK_COUNT_ESI_CP_READY_YES                      (0x00000001) /* R-XUV */

#define NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_ESI0                          (0x00002003) /* R-XUR */
#define NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_ESI0_REMOTE_CTRL                      0:0  /* R-XUF */
#define NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_ESI0_REMOTE_CTRL_NO           (0x00000000) /* R-XUV */
#define NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_ESI0_REMOTE_CTRL_YES          (0x00000001) /* R-XUV */
#define NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_ESI0_AUTO_TEST                        1:1  /* R-XUF */
#define NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_ESI0_AUTO_TEST_NO             (0x00000000) /* R-XUV */
#define NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_ESI0_AUTO_TEST_YES            (0x00000001) /* R-XUV */
// for eDP v1.4 & v1.4a only
#define NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_ESI0_TOUCH_IRQ                        1:1  /* R-XUF */
#define NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_ESI0_TOUCH_IRQ_NO             (0x00000000) /* R-XUV */
#define NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_ESI0_TOUCH_IRQ_YES            (0x00000001) /* R-XUV */

#define NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_ESI0_CP                               2:2  /* R-XUF */
#define NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_ESI0_CP_NO                    (0x00000000) /* R-XUV */
#define NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_ESI0_CP_YES                   (0x00000001) /* R-XUV */
#define NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_ESI0_MCCS_IRQ                         3:3  /* R-XUF */
#define NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_ESI0_MCCS_IRQ_NO              (0x00000000) /* R-XUV */
#define NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_ESI0_MCCS_IRQ_YES             (0x00000001) /* R-XUV */
#define NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_ESI0_DOWN_REP_MSG_RDY                 4:4  /* R-XUF */
#define NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_ESI0_DOWN_REP_MSG_RDY_NO      (0x00000000) /* R-XUV */
#define NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_ESI0_DOWN_REP_MSG_RDY_YES     (0x00000001) /* R-XUV */
#define NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_ESI0_UP_REQ_MSG_RDY                   5:5  /* R-XUF */
#define NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_ESI0_UP_REQ_MSG_RDY_NO        (0x00000000) /* R-XUV */
#define NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_ESI0_UP_REQ_MSG_RDY_YES       (0x00000001) /* R-XUV */
#define NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_ESI0_SINK_SPECIFIC_IRQ                6:6  /* R-XUF */
#define NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_ESI0_SINK_SPECIFIC_IRQ_NO     (0x00000000) /* R-XUV */
#define NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_ESI0_SINK_SPECIFIC_IRQ_YES    (0x00000001) /* R-XUV */

#define NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_ESI1                                      (0x00002004) /* R-XUR */
#define NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_ESI1_RX_GTC_MSTR_REQ_STATUS_CHANGE                0:0  /* R-XUF */
#define NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_ESI1_RX_GTC_MSTR_REQ_STATUS_CHANGE_NO     (0x00000000) /* R-XUV */
#define NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_ESI1_RX_GTC_MSTR_REQ_STATUS_CHANGE_YES    (0x00000001) /* R-XUV */
#define NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_ESI1_PANEL_REPLAY_ERROR_STATUS                    3:3  /* R-XUF */
#define NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_ESI1_PANEL_REPLAY_ERROR_STATUS_NO         (0x00000000) /* R-XUV */
#define NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_ESI1_PANEL_REPLAY_ERROR_STATUS_YES        (0x00000001) /* R-XUV */


#define NV_DPCD_LINK_SERVICE_IRQ_VECTOR_ESI0                                (0x00002005) /* R-XUR */
#define NV_DPCD_LINK_SERVICE_IRQ_VECTOR_ESI0_RX_CAP_CHANGED                         0:0  /* R-XUF */
#define NV_DPCD_LINK_SERVICE_IRQ_VECTOR_ESI0_RX_CAP_CHANGED_NO              (0x00000000) /* R-XUV */
#define NV_DPCD_LINK_SERVICE_IRQ_VECTOR_ESI0_RX_CAP_CHANGED_YES             (0x00000001) /* R-XUV */
#define NV_DPCD_LINK_SERVICE_IRQ_VECTOR_ESI0_LINK_STATUS_CHANGED                    1:1  /* R-XUF */
#define NV_DPCD_LINK_SERVICE_IRQ_VECTOR_ESI0_LINK_STATUS_CHANGED_NO         (0x00000000) /* R-XUV */
#define NV_DPCD_LINK_SERVICE_IRQ_VECTOR_ESI0_LINK_STATUS_CHANGED_YES        (0x00000001) /* R-XUV */
#define NV_DPCD_LINK_SERVICE_IRQ_VECTOR_ESI0_STREAM_STATUS_CHANGED                  2:2  /* R-XUF */
#define NV_DPCD_LINK_SERVICE_IRQ_VECTOR_ESI0_STREAM_STATUS_CHANGED_NO       (0x00000000) /* R-XUV */
#define NV_DPCD_LINK_SERVICE_IRQ_VECTOR_ESI0_STREAM_STATUS_CHANGED_YES      (0x00000001) /* R-XUV */
#define NV_DPCD_LINK_SERVICE_IRQ_VECTOR_ESI0_HDMI_LINK_STATUS_CHANGED               3:3  /* R-XUF */
#define NV_DPCD_LINK_SERVICE_IRQ_VECTOR_ESI0_HDMI_LINK_STATUS_CHANGED_NO    (0x00000000) /* R-XUV */
#define NV_DPCD_LINK_SERVICE_IRQ_VECTOR_ESI0_HDMI_LINK_STATUS_CHANGED_YES   (0x00000001) /* R-XUV */
#define NV_DPCD_LINK_SERVICE_IRQ_VECTOR_ESI0_CONNECTED_OFF_ENTRY_REQ                4:4  /* R-XUF */
#define NV_DPCD_LINK_SERVICE_IRQ_VECTOR_ESI0_CONNECTED_OFF_ENTRY_REQ_NO     (0x00000000) /* R-XUV */
#define NV_DPCD_LINK_SERVICE_IRQ_VECTOR_ESI0_CONNECTED_OFF_ENTRY_REQ_YES    (0x00000001) /* R-XUV */

#define NV_DPCD_PANEL_SELF_REFRESH_ERR_STATUS                               (0x00002006) /* R-XUR */
#define NV_DPCD_PANEL_SELF_REFRESH_ERR_STATUS_LINK_CRC_ERR                          0:0  /* R-XUF */
#define NV_DPCD_PANEL_SELF_REFRESH_ERR_STATUS_LINK_CRC_ERR_NO               (0x00000000) /* R-XUV */
#define NV_DPCD_PANEL_SELF_REFRESH_ERR_STATUS_LINK_CRC_ERR_YES              (0x00000001) /* R-XUV */
#define NV_DPCD_PANEL_SELF_REFRESH_ERR_STATUS_RFB_ERR                               1:1  /* R-XUF */
#define NV_DPCD_PANEL_SELF_REFRESH_ERR_STATUS_RFB_ERR_NO                    (0x00000000) /* R-XUV */
#define NV_DPCD_PANEL_SELF_REFRESH_ERR_STATUS_RFB_ERR_YES                   (0x00000001) /* R-XUV */
#define NV_DPCD_PANEL_SELF_REFRESH_ERR_STATUS_VSC_SDP_ERR                           2:2  /* R-XUF */
#define NV_DPCD_PANEL_SELF_REFRESH_ERR_STATUS_VSC_SDP_ERR_NO                (0x00000000) /* R-XUV */
#define NV_DPCD_PANEL_SELF_REFRESH_ERR_STATUS_VSC_SDP_ERR_YES               (0x00000001) /* R-XUV */

#define NV_DPCD_PANEL_SELF_REFRESH_EVENT_STATUS                             (0x00002007) /* R-XUR */
#define NV_DPCD_PANEL_SELF_REFRESH_EVENT_STATUS_CAP_CHANGE                          0:0  /* R-XUF */
#define NV_DPCD_PANEL_SELF_REFRESH_EVENT_STATUS_CAP_CHANGE_NO               (0x00000000) /* R-XUV */
#define NV_DPCD_PANEL_SELF_REFRESH_EVENT_STATUS_CAP_CHANGE_YES              (0x00000001) /* R-XUV */

#define NV_DPCD_PANEL_SELF_REFRESH_STATUS                                   (0x00002008) /* R-XUR */
#define NV_DPCD_PANEL_SELF_REFRESH_STATUS_VAL                                       2:0  /* R-XUF */
#define NV_DPCD_PANEL_SELF_REFRESH_STATUS_VAL_INACTIVE                      (0x00000000) /* R-XUV */
#define NV_DPCD_PANEL_SELF_REFRESH_STATUS_VAL_TRANSITION_TO_ACTIVE          (0x00000001) /* R-XUV */
#define NV_DPCD_PANEL_SELF_REFRESH_STATUS_VAL_ACTIVE_DISP_FROM_RFB          (0x00000002) /* R-XUV */
#define NV_DPCD_PANEL_SELF_REFRESH_STATUS_VAL_ACTIVE_SINK_DEV_TIMING        (0x00000003) /* R-XUV */
#define NV_DPCD_PANEL_SELF_REFRESH_STATUS_VAL_TRANSITION_TO_INACTIVE        (0x00000004) /* R-XUV */
#define NV_DPCD_PANEL_SELF_REFRESH_STATUS_VAL_RESERVED0                     (0x00000005) /* R-XUV */
#define NV_DPCD_PANEL_SELF_REFRESH_STATUS_VAL_RESERVED1                     (0x00000006) /* R-XUV */
#define NV_DPCD_PANEL_SELF_REFRESH_STATUS_VAL_SINK_DEV_INTERNAL_ERR         (0x00000007) /* R-XUV */

#define NV_DPCD_PANEL_SELF_REFRESH_DEBUG0                                   (0x00002009) /* R-XUR */
#define NV_DPCD_PANEL_SELF_REFRESH_DEBUG0_MAX_RESYNC_FRAME_CNT                      3:0  /* R-XUF */
#define NV_DPCD_PANEL_SELF_REFRESH_DEBUG0_LAST_RESYNC_FRAME_CNT                     7:4  /* R-XUF */

#define NV_DPCD_PANEL_SELF_REFRESH_DEBUG1                                   (0x0000200A) /* R-XUR */
#define NV_DPCD_PANEL_SELF_REFRESH_LAST_SDP                                 (0x0000200A) /* R-XUR */
#define NV_DPCD_PANEL_SELF_REFRESH_LAST_SDP_PSR_STATE_BIT                           0:0  /* R-XUF */
#define NV_DPCD_PANEL_SELF_REFRESH_LAST_SDP_RFB_BIT                                 1:1  /* R-XUF */
#define NV_DPCD_PANEL_SELF_REFRESH_LAST_SDP_CRC_VALID_BIT                           2:2  /* R-XUF */
#define NV_DPCD_PANEL_SELF_REFRESH_LAST_SDP_SU_VALID_BIT                            3:3  /* R-XUF */
#define NV_DPCD_PANEL_SELF_REFRESH_LAST_SDP_SU_FIRST_LINE_RCVD                      4:4  /* R-XUF */
#define NV_DPCD_PANEL_SELF_REFRESH_LAST_SDP_SU_LAST_LINE_RCVD                       5:5  /* R-XUF */
#define NV_DPCD_PANEL_SELF_REFRESH_LAST_SDP_Y_CORD_VALID                            6:6  /* R-XUF */

// 0200Bh: RESERVED. Read all 0s

#define NV_DPCD_LANE0_1_STATUS_ESI                                      (0x0000200C) /* R-XUR */
#define NV_DPCD_LANE2_3_STATUS_ESI                                      (0x0000200D) /* R-XUR */
#define NV_DPCD_LANEX_XPLUS1_STATUS_ESI_LANEX_CR_DONE                           0:0  /* R-XUF */
#define NV_DPCD_LANEX_XPLUS1_STATUS_ESI_LANEX_CR_DONE_NO                (0x00000000) /* R-XUV */
#define NV_DPCD_LANEX_XPLUS1_STATUS_ESI_LANEX_CR_DONE_YES               (0x00000001) /* R-XUV */
#define NV_DPCD_LANEX_XPLUS1_STATUS_ESI_LANEX_CHN_EQ_DONE                       1:1  /* R-XUF */
#define NV_DPCD_LANEX_XPLUS1_STATUS_ESI_LANEX_CHN_EQ_DONE_NO            (0x00000000) /* R-XUV */
#define NV_DPCD_LANEX_XPLUS1_STATUS_ESI_LANEX_CHN_EQ_DONE_YES           (0x00000001) /* R-XUV */
#define NV_DPCD_LANEX_XPLUS1_STATUS_ESI_LANEX_SYMBOL_LOCKED                     2:2  /* R-XUF */
#define NV_DPCD_LANEX_XPLUS1_STATUS_ESI_LANEX_SYMBOL_LOCKED_NO          (0x00000000) /* R-XUV */
#define NV_DPCD_LANEX_XPLUS1_STATUS_ESI_LANEX_SYMBOL_LOCKED_YES         (0x00000001) /* R-XUV */
#define NV_DPCD_LANEX_XPLUS1_STATUS_ESI_LANEXPLUS1_CR_DONE                      4:4  /* R-XUF */
#define NV_DPCD_LANEX_XPLUS1_STATUS_ESI_LANEXPLUS1_CR_DONE_NO           (0x00000000) /* R-XUV */
#define NV_DPCD_LANEX_XPLUS1_STATUS_ESI_LANEXPLUS1_CR_DONE_YES          (0x00000001) /* R-XUV */
#define NV_DPCD_LANEX_XPLUS1_STATUS_ESI_LANEXPLUS1_CHN_EQ_DONE                  5:5  /* R-XUF */
#define NV_DPCD_LANEX_XPLUS1_STATUS_ESI_LANEXPLUS1_CHN_EQ_DONE_NO       (0x00000000) /* R-XUV */
#define NV_DPCD_LANEX_XPLUS1_STATUS_ESI_LANEXPLUS1_CHN_EQ_DONE_YES      (0x00000001) /* R-XUV */
#define NV_DPCD_LANEX_XPLUS1_STATUS_ESI_LANEXPLUS1_SYMBOL_LOCKED                6:6  /* R-XUF */
#define NV_DPCD_LANEX_XPLUS1_STATUS_ESI_LANEXPLUS1_SYMBOL_LOCKED_NO     (0x00000000) /* R-XUV */
#define NV_DPCD_LANEX_XPLUS1_STATUS_ESI_LANEXPLUS1_SYMBOL_LOCKED_YES    (0x00000001) /* R-XUV */

#define NV_DPCD_LANE_ALIGN_STATUS_UPDATED_ESI                               (0x0000200E) /* R-XUR */
#define NV_DPCD_LANE_ALIGN_STATUS_UPDATED_ESI_INTERLANE_ALIGN_DONE                  0:0  /* R-XUF */
#define NV_DPCD_LANE_ALIGN_STATUS_UPDATED_ESI_INTERLANE_ALIGN_DONE_NO       (0x00000000) /* R-XUV */
#define NV_DPCD_LANE_ALIGN_STATUS_UPDATED_ESI_INTERLANE_ALIGN_DONE_YES      (0x00000001) /* R-XUV */
#define NV_DPCD_LANE_ALIGN_STATUS_UPDATED_ESI_DOWNSTRM_PORT_STATUS_DONE             6:6  /* R-XUF */
#define NV_DPCD_LANE_ALIGN_STATUS_UPDATED_ESI_DOWNSTRM_PORT_STATUS_DONE_NO  (0x00000000) /* R-XUV */
#define NV_DPCD_LANE_ALIGN_STATUS_UPDATED_ESI_DOWNSTRM_PORT_STATUS_DONE_YES (0x00000001) /* R-XUV */
#define NV_DPCD_LANE_ALIGN_STATUS_UPDATED_ESI_LINK_STATUS_UPDATED                   7:7  /* R-XUF */
#define NV_DPCD_LANE_ALIGN_STATUS_UPDATED_ESI_LINK_STATUS_UPDATED_NO        (0x00000000) /* R-XUV */
#define NV_DPCD_LANE_ALIGN_STATUS_UPDATED_ESI_LINK_STATUS_UPDATED_YES       (0x00000001) /* R-XUV */

#define NV_DPCD_SINK_STATUS_ESI                                         (0x0000200F) /* R-XUR */
#define NV_DPCD_SINK_STATUS_ESI_RECEIVE_PORT_0_STATUS                           0:0  /* R-XUF */
#define NV_DPCD_SINK_STATUS_ESI_RECEIVE_PORT_0_STATUS_IN_SYNC_NO        (0x00000000) /* R-XUV */
#define NV_DPCD_SINK_STATUS_ESI_RECEIVE_PORT_0_STATUS_IN_SYNC_YES       (0x00000001) /* R-XUV */
#define NV_DPCD_SINK_STATUS_ESI_RECEIVE_PORT_1_STATUS                           1:1  /* R-XUF */
#define NV_DPCD_SINK_STATUS_ESI_RECEIVE_PORT_1_STATUS_IN_SYNC_NO        (0x00000000) /* R-XUV */
#define NV_DPCD_SINK_STATUS_ESI_RECEIVE_PORT_1_STATUS_IN_SYNC_YES       (0x00000001) /* R-XUV */

// 0x00002010-0x0002025: RESERVED. Read all 0s
#define NV_DPCD_OVERDRIVE_STATUS                                        (0x00002026) /* R-XUR */
#define NV_DPCD_OVERDRIVE_STATUS_OVERDRIVE_ENGINE_STATUS                        0:0  /* R-XUF */
#define NV_DPCD_OVERDRIVE_STATUS_OVERDRIVE_ENGINE_STATUS_NOT_ACTIVE     (0x00000000) /* R-XUV */
#define NV_DPCD_OVERDRIVE_STATUS_OVERDRIVE_ENGINE_STATUS_ACTIVE         (0x00000001) /* R-XUV */
// 0x00002027-0x00067FF: RESERVED. Read all 0s

#define NV_DPCD_HDCP_BKSV_OFFSET                                 (0x00068000) /* R-XUR */
#define NV_DPCD_HDCP_RPRIME_OFFSET                               (0x00068005) /* R-XUR */
#define NV_DPCD_HDCP_AKSV_OFFSET                                 (0x00068007) /* RWXUR */
#define NV_DPCD_HDCP_AN_OFFSET                                   (0x0006800C) /* RWXUR */
#define NV_DPCD_HDCP_BKSV_S_OFFSET                               (0x00000300) /* RWXUV */
#define NV_DPCD_HDCP_RPRIME_S_OFFSET                             (0x00000305) /* RWXUV */
#define NV_DPCD_HDCP_AKSV_S_OFFSET                               (0x00000307) /* RWXUV */
#define NV_DPCD_HDCP_AN_S_OFFSET                                 (0x0000030c) /* RWXUV */
#define NV_DPCD_HDCP_VPRIME_OFFSET                               (0x00068014) /* R-XUR */
#define NV_DPCD_HDCP_BCAPS_OFFSET                                (0x00068028) /* R-XUR */
#define NV_DPCD_HDCP_BCAPS_OFFSET_HDCP_CAPABLE                           0:0  /* R-XUF */
#define NV_DPCD_HDCP_BCAPS_OFFSET_HDCP_CAPABLE_NO                (0x00000000) /* R-XUV */
#define NV_DPCD_HDCP_BCAPS_OFFSET_HDCP_CAPABLE_YES               (0x00000001) /* R-XUV */
#define NV_DPCD_HDCP_BCAPS_OFFSET_HDCP_REPEATER                          1:1  /* R-XUF */
#define NV_DPCD_HDCP_BCAPS_OFFSET_HDCP_REPEATER_NO               (0x00000000) /* R-XUV */
#define NV_DPCD_HDCP_BCAPS_OFFSET_HDCP_REPEATER_YES              (0x00000001) /* R-XUV */
#define NV_DPCD_HDCP_BSTATUS_OFFSET                              (0x00068029) /* R-XUR */
#define NV_DPCD_HDCP_BSTATUS_REAUTHENTICATION_REQUESET                   3:3  /* R-XUF */
#define NV_DPCD_HDCP_BSTATUS_REAUTHENTICATION_REQUESET_FALSE     (0x00000000) /* R-XUV */
#define NV_DPCD_HDCP_BSTATUS_REAUTHENTICATION_REQUESET_TRUE      (0x00000001) /* R-XUV */
#define NV_DPCD_HDCP_BSTATUS_LINK_INTEGRITY_FAILURE                      2:2  /* R-XUF */
#define NV_DPCD_HDCP_BSTATUS_LINK_INTEGRITY_FAILURE_FALSE        (0x00000000) /* R-XUV */
#define NV_DPCD_HDCP_BSTATUS_LINK_INTEGRITY_FAILURE_TRUE         (0x00000001) /* R-XUV */
#define NV_DPCD_HDCP_BSTATUS_RPRIME_AVAILABLE                            1:1  /* R-XUF */
#define NV_DPCD_HDCP_BSTATUS_RPRIME_AVAILABLE_FALSE              (0x00000000) /* R-XUV */
#define NV_DPCD_HDCP_BSTATUS_RPRIME_AVAILABLE_TRUE               (0x00000001) /* R-XUV */
#define NV_DPCD_HDCP_BSTATUS_READY                                       0:0  /* R-XUF */
#define NV_DPCD_HDCP_BSTATUS_READY_FALSE                         (0x00000000) /* R-XUV */
#define NV_DPCD_HDCP_BSTATUS_READY_TRUE                          (0x00000001) /* R-XUV */
#define NV_DPCD_HDCP_BINFO_OFFSET                                (0x0006802A) /* R-XUR */
#define NV_DPCD_HDCP_BINFO_OFFSET_DEVICE_COUNT                           6:0  /* R-XUF */
#define NV_DPCD_HDCP_BINFO_OFFSET_MAX_DEVS_EXCEEDED                      7:7  /* R-XUF */
#define NV_DPCD_HDCP_BINFO_OFFSET_MAX_DEVS_EXCEEDED_FALSE        (0x00000000) /* R-XUV */
#define NV_DPCD_HDCP_BINFO_OFFSET_MAX_DEVS_EXCEEDED_TRUE         (0x00000001) /* R-XUV */
#define NV_DPCD_HDCP_BINFO_OFFSET_DEPTH                                 10:8  /* R-XUF */
#define NV_DPCD_HDCP_BINFO_OFFSET_MAX_CASCADE_EXCEEDED                 11:11  /* R-XUF */
#define NV_DPCD_HDCP_BINFO_OFFSET_MAX_CASCADE_EXCEEDED_FALSE     (0x00000000) /* R-XUV */
#define NV_DPCD_HDCP_BINFO_OFFSET_MAX_CASCADE_EXCEEDED_TRUE      (0x00000001) /* R-XUV */

#define NV_DPCD_HDCP_KSV_FIFO_OFFSET                             (0x0006802C) /* R-XUR */

#define NV_DPCD_HDCP_AINFO_OFFSET                                       (0x0006803B) /* RWXUR */
#define NV_DPCD_HDCP_AINFO_OFFSET_REAUTHENTICATION_ENABLE_IRQ_HPD               0:0  /* RWXUF */
#define NV_DPCD_HDCP_AINFO_OFFSET_REAUTHENTICATION_ENABLE_IRQ_HPD_NO    (0x00000000) /* RWXUV */
#define NV_DPCD_HDCP_AINFO_OFFSET_REAUTHENTICATION_ENABLE_IRQ_HPD_YES   (0x00000001) /* RWXUV */

// Eight-Lane DP Specific DPCD defines
#define NV_DPCD_SL_TRAINING_LANE0_1_SET2(baseAddr)               (baseAddr + 0x0000010E) /* RWXUR */
#define NV_DPCD_SL_TRAINING_LANE2_3_SET2(baseAddr)               (baseAddr + 0x0000010F) /* RWXUR */
#define NV_DPCD_SL_LANE4_5_STATUS(baseAddr)                      (baseAddr + 0x00000202) /* R-XUR */
#define NV_DPCD_SL_LANE6_7_STATUS(baseAddr)                      (baseAddr + 0x00000203) /* R-XUR */
#define NV_DPCD_DUAL_DP_CAP                                      (0x000003B0) /* RWXUR */ // Dual DP Capability Register
#define NV_DPCD_DUAL_DP_CAP_DDC                                  0:0          /* RWXUF */ // Dual DP Capability
#define NV_DPCD_DUAL_DP_CAP_DDC_DISABLE                          (0x00000000) /* RWXUV */
#define NV_DPCD_DUAL_DP_CAP_DDC_ENABLE                           (0x00000001) /* RWXUV */
#define NV_DPCD_DUAL_DP_CAP_DDCIC                                1:1          /* RWXUF */  // DDCIC : Dual DP Column Interleave Mode Capability
#define NV_DPCD_DUAL_DP_CAP_DDCIC_DISABLE                        (0x00000000) /* RWXUV */
#define NV_DPCD_DUAL_DP_CAP_DDCIC_ENABLE                         (0x00000001) /* RWXUV */
#define NV_DPCD_DUAL_DP_CAP_DDPSBSC                              2:2          /* RWXUF */  // DDPSBSC : Dual DP Pixel Side-by-Side Mode Capability
#define NV_DPCD_DUAL_DP_CAP_DDPSBSC_DISBALE                      (0x00000000) /* RWXUV */
#define NV_DPCD_DUAL_DP_CAP_DDPSBSC_ENABLE                       (0x00000001) /* RWXUV */

#define NV_DPCD_DUAL_DP_BASE_ADDRESS                             19:0  /* RWXUF */
#define NV_DPCD_DUAL_DP_COLUMN_WIDTH                             15:0  /* RWXUF */
#define NV_DPCD_DUAL_DP_MAX_LANECOUNT                            4:0   /* RWXUF */
#define NV_DPCD_DUAL_DP_MAX_LANECOUNT_1H                         0x1   /* RWXUV */
#define NV_DPCD_DUAL_DP_MAX_LANECOUNT_2H                         0x2   /* RWXUV */
#define NV_DPCD_DUAL_DP_MAX_LANECOUNT_4H                         0x4   /* RWXUV */
#define NV_DPCD_DUAL_DP_MAX_LANECOUNT_8H                         0x8   /* RWXUV */

#define NV_DPCD_DUAL_DP_DUAL_LINK_CONTROL(baseAddr)                 (baseAddr + 0x00000110) /* RWXUR */  // Dual Link Control Register
#define NV_DPCD_DUAL_DP_DUAL_LINK_CONTROL_PIX_MODE                  1:0          /* RWXUF */  // PIX_MODE : Pixel mode select
#define NV_DPCD_DUAL_DP_DUAL_LINK_CONTROL_PIX_MODE_SIDE_BY_SIDE     (0x00000000) /* RWXUV */  // Side by side Mode enabled
#define NV_DPCD_DUAL_DP_DUAL_LINK_CONTROL_PIX_MODE_COL_INTERLEAVE   (0x00000001) /* RWXUV */  // Column Interleave Mode enabled
#define NV_DPCD_DUAL_DP_DUAL_LINK_CONTROL_DD_ENABLE                 7:7          /* RWXUF */  // DD_ENABLE: Enable Dual DP mode.
#define NV_DPCD_DUAL_DP_DUAL_LINK_CONTROL_DD_ENABLE_TRUE            (0x00000001) /* RWXUV */
#define NV_DPCD_DUAL_DP_DUAL_LINK_CONTROL_DD_ENABLE_FALSE           (0x00000000) /* RWXUV */

#define NV_DPCD_DUAL_DP_PIXEL_OVERLAP(baseAddr)                     (baseAddr + 0x00000111) /* RWXUR */  // PIXEL_OVERLAP Register
#define NV_DPCD_DUAL_DP_PIXEL_OVERLAP_IGNORE_PIX_COUNT              6:0          /* RWXUF */  // Ignore Pix Count - Number of pixels to ignore

#define NV_DPCD_HDCP22_BCAPS_OFFSET                                 (0x0006921D) /* R-XUR */
#define NV_DPCD_HDCP22_BCAPS_SIZE                                   (0x00000003) /* R---S */
#define NV_DPCD_HDCP22_BCAPS_OFFSET_HDCP_REPEATER                            0:0 /* R-XUF */
#define NV_DPCD_HDCP22_BCAPS_OFFSET_HDCP_REPEATER_NO                (0x00000000) /* R-XUV */
#define NV_DPCD_HDCP22_BCAPS_OFFSET_HDCP_REPEATER_YES               (0x00000001) /* R-XUV */
#define NV_DPCD_HDCP22_BCAPS_OFFSET_HDCP_CAPABLE                             1:1 /* R-XUF */
#define NV_DPCD_HDCP22_BCAPS_OFFSET_HDCP_CAPABLE_NO                 (0x00000000) /* R-XUV */
#define NV_DPCD_HDCP22_BCAPS_OFFSET_HDCP_CAPABLE_YES                (0x00000001) /* R-XUV */
#define NV_DPCD_HDCP22_BCAPS_OFFSET_RECEIVER_CAPABILITY_MASK                15:2 /* R-XUF */
#define NV_DPCD_HDCP22_BCAPS_OFFSET_RECEIVER_CAPABILITY_MASK_RESERVED (0x00000000) /* R-XUV */
#define NV_DPCD_HDCP22_BCAPS_OFFSET_VERSION                                23:16 /* R-XUF */
#define NV_DPCD_HDCP22_BCAPS_OFFSET_VERSION_22                      (0x00000002) /* R-XUV */

#define NV_DPCD_HDCP22_BINFO_OFFSET                         (0x00069330) /* R-XUR */
#define NV_DPCD_HDCP22_BINFO_SIZE                           (0x00000002) /* R---S */

#define NV_DPCD_HDCP22_RX_STATUS                            (0x00069493) /* R-XUR */
#define NV_DPCD_HDCP22_RX_STATUS_SIZE                       (0x00000001) /* R---S */
#define NV_DPCD_HDCP22_RX_STATUS_READY                              0:0  /* R-XUF */
#define NV_DPCD_HDCP22_RX_STATUS_READY_YES                  (0x00000001) /* R-XUV */
#define NV_DPCD_HDCP22_RX_STATUS_READY_NO                   (0x00000000) /* R-XUV */
#define NV_DPCD_HDCP22_RX_STATUS_HPRIME_AVAILABLE                   1:1  /* R-XUF */
#define NV_DPCD_HDCP22_RX_STATUS_HPRIME_AVAILABLE_YES       (0x00000001) /* R-XUV */
#define NV_DPCD_HDCP22_RX_STATUS_HPRIME_AVAILABLE_NO        (0x00000000) /* R-XUV */
#define NV_DPCD_HDCP22_RX_STATUS_PAIRING_AVAILABLE                  2:2  /* R-XUF */
#define NV_DPCD_HDCP22_RX_STATUS_PAIRING_AVAILABLE_YES      (0x00000001) /* R-XUV */
#define NV_DPCD_HDCP22_RX_STATUS_PAIRING_AVAILABLE_NO       (0x00000000) /* R-XUV */
#define NV_DPCD_HDCP22_RX_STATUS_REAUTH_REQUEST                     3:3  /* R-XUF */
#define NV_DPCD_HDCP22_RX_STATUS_REAUTH_REQUEST_YES         (0x00000001) /* R-XUV */
#define NV_DPCD_HDCP22_RX_STATUS_REAUTH_REQUEST_NO          (0x00000000) /* R-XUV */
#define NV_DPCD_HDCP22_RX_STATUS_LINK_INTEGRITY_FAILURE             4:4  /* R-XUF */
#define NV_DPCD_HDCP22_RX_STATUS_LINK_INTEGRITY_FAILURE_YES (0x00000001) /* R-XUV */
#define NV_DPCD_HDCP22_RX_STATUS_LINK_INTEGRITY_FAILURE_NO  (0x00000000) /* R-XUV */

#define NV_DPCD_HDCP22_RTX_OFFSET                           (0x00069000) /* RWXUR */
#define NV_DPCD_HDCP22_RTX_SIZE                             (0x00000008) /* R---S */

#define NV_DPCD_HDCP22_TXCAPS_OFFSET                        (0x00069008) /* RWXUR */
#define NV_DPCD_HDCP22_TXCAPS_SIZE                          (0x00000003) /* R---S */

#define NV_DPCD_HDCP22_CERTRX                               (0x0006900B) /* R-XUR */
#define NV_DPCD_HDCP22_CERTRX_SIZE                          (0x0000020A) /* R---S */

#define NV_DPCD_HDCP22_RRX                                  (0x00069215) /* R-XUR */
#define NV_DPCD_HDCP22_RRX_SIZE                             (0x00000008) /* R---S */

#endif // #ifndef _DPCD_H_
