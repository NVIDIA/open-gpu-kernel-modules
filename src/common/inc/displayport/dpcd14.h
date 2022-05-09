/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _DISPLAYPORT14_H_
#define _DISPLAYPORT14_H_

#define NV_DPCD14_EXTEND_CAP_BASE                                                   (0x00002200)

#define NV_DPCD14_MAX_LINK_BANDWIDTH                                                (0x00000001) /* R-XUR */
#define NV_DPCD14_MAX_LINK_BANDWIDTH_VAL                                                     7:0 /* R-XUF */
#define NV_DPCD14_MAX_LINK_BANDWIDTH_VAL_8_10_GBPS                                  (0x0000001E) /* R-XUV */

#define NV_DPCD14_MAX_DOWNSPREAD                                                    (0x00000003) /* R-XUR */
#define NV_DPCD14_MAX_DOWNSPREAD_TPS4_SUPPORTED                                              7:7 /* R-XUF */
#define NV_DPCD14_MAX_DOWNSPREAD_TPS4_SUPPORTED_NO                                  (0x00000000) /* R-XUV */
#define NV_DPCD14_MAX_DOWNSPREAD_TPS4_SUPPORTED_YES                                 (0x00000001) /* R-XUV */

#define NV_DPCD14_TRAINING_AUX_RD_INTERVAL                                          (0x0000000E) /* R-XUR */
#define NV_DPCD14_TRAINING_AUX_RD_INTERVAL_EXTENDED_RX_CAP                                   7:7 /* R-XUF */
#define NV_DPCD14_TRAINING_AUX_RD_INTERVAL_EXTENDED_RX_CAP_NO                       (0x00000000) /* R-XUV */
#define NV_DPCD14_TRAINING_AUX_RD_INTERVAL_EXTENDED_RX_CAP_YES                      (0x00000001) /* R-XUV */

#define NV_DPCD14_DSC_SUPPORT                                                       (0x00000060) /* R-XUR */
#define NV_DPCD14_DSC_SUPPORT_DSC_SUPPORT                                                    0:0 /* R-XUF */
#define NV_DPCD14_DSC_SUPPORT_DSC_SUPPORT_NO                                        (0x00000000) /* R-XUV */
#define NV_DPCD14_DSC_SUPPORT_DSC_SUPPORT_YES                                       (0x00000001) /* R-XUV */

#define NV_DPCD14_DSC_ALGORITHM_REVISION                                            (0x00000061) /* R-XUR */
#define NV_DPCD14_DSC_ALGORITHM_REVISION_MAJOR                                               3:0 /* R-XUF */
#define NV_DPCD14_DSC_ALGORITHM_REVISION_MINOR                                               7:4 /* R-XUF */

#define NV_DPCD14_DSC_RC_BUFFER_BLOCK                                               (0x00000062) /* R-XUR */
#define NV_DPCD14_DSC_RC_BUFFER_BLOCK_SIZE                                                   1:0 /* R-XUF */
#define NV_DPCD14_DSC_RC_BUFFER_BLOCK_SIZE_1KB                                      (0x00000000) /* R-XUV */
#define NV_DPCD14_DSC_RC_BUFFER_BLOCK_SIZE_4KB                                      (0x00000001) /* R-XUV */
#define NV_DPCD14_DSC_RC_BUFFER_BLOCK_SIZE_16KB                                     (0x00000002) /* R-XUV */
#define NV_DPCD14_DSC_RC_BUFFER_BLOCK_SIZE_64KB                                     (0x00000003) /* R-XUV */

#define NV_DPCD14_DSC_RC_BUFFER                                                     (0x00000063) /* R-XUR */
#define NV_DPCD14_DSC_RC_BUFFER_SIZE                                                         7:0 /* R-XUF */

#define NV_DPCD14_DSC_SLICE_CAPABILITIES_1                                          (0x00000064) /* R-XUR */
#define NV_DPCD14_DSC_SLICE_CAPABILITIES_1_SLICES_PER_SINK_1                                 0:0 /* R-XUF */
#define NV_DPCD14_DSC_SLICE_CAPABILITIES_1_SLICES_PER_SINK_1_NO                     (0x00000000) /* R-XUV */
#define NV_DPCD14_DSC_SLICE_CAPABILITIES_1_SLICES_PER_SINK_1_YES                    (0x00000001) /* R-XUV */
#define NV_DPCD14_DSC_SLICE_CAPABILITIES_1_SLICES_PER_SINK_2                                 1:1 /* R-XUF */
#define NV_DPCD14_DSC_SLICE_CAPABILITIES_1_SLICES_PER_SINK_2_NO                     (0x00000000) /* R-XUV */
#define NV_DPCD14_DSC_SLICE_CAPABILITIES_1_SLICES_PER_SINK_2_YES                    (0x00000001) /* R-XUV */
#define NV_DPCD14_DSC_SLICE_CAPABILITIES_1_SLICES_PER_SINK_4                                 3:3 /* R-XUF */
#define NV_DPCD14_DSC_SLICE_CAPABILITIES_1_SLICES_PER_SINK_4_NO                     (0x00000000) /* R-XUV */
#define NV_DPCD14_DSC_SLICE_CAPABILITIES_1_SLICES_PER_SINK_4_YES                    (0x00000001) /* R-XUV */
#define NV_DPCD14_DSC_SLICE_CAPABILITIES_1_SLICES_PER_SINK_6                                 4:4 /* R-XUF */
#define NV_DPCD14_DSC_SLICE_CAPABILITIES_1_SLICES_PER_SINK_6_NO                     (0x00000000) /* R-XUV */
#define NV_DPCD14_DSC_SLICE_CAPABILITIES_1_SLICES_PER_SINK_6_YES                    (0x00000001) /* R-XUV */
#define NV_DPCD14_DSC_SLICE_CAPABILITIES_1_SLICES_PER_SINK_8                                 5:5 /* R-XUF */
#define NV_DPCD14_DSC_SLICE_CAPABILITIES_1_SLICES_PER_SINK_8_NO                     (0x00000000) /* R-XUV */
#define NV_DPCD14_DSC_SLICE_CAPABILITIES_1_SLICES_PER_SINK_8_YES                    (0x00000001) /* R-XUV */
#define NV_DPCD14_DSC_SLICE_CAPABILITIES_1_SLICES_PER_SINK_10                                6:6 /* R-XUF */
#define NV_DPCD14_DSC_SLICE_CAPABILITIES_1_SLICES_PER_SINK_10_NO                    (0x00000000) /* R-XUV */
#define NV_DPCD14_DSC_SLICE_CAPABILITIES_1_SLICES_PER_SINK_10_YES                   (0x00000001) /* R-XUV */
#define NV_DPCD14_DSC_SLICE_CAPABILITIES_1_SLICES_PER_SINK_12                                7:7 /* R-XUF */
#define NV_DPCD14_DSC_SLICE_CAPABILITIES_1_SLICES_PER_SINK_12_NO                    (0x00000000) /* R-XUV */
#define NV_DPCD14_DSC_SLICE_CAPABILITIES_1_SLICES_PER_SINK_12_YES                   (0x00000001) /* R-XUV */

#define NV_DPCD14_DSC_LINE_BUFFER                                                   (0x00000065) /* R-XUR */
#define NV_DPCD14_DSC_LINE_BUFFER_BIT_DEPTH                                                  3:0 /* R-XUF */
#define NV_DPCD14_DSC_LINE_BUFFER_BIT_DEPTH_9                                       (0x00000000) /* R-XUV */
#define NV_DPCD14_DSC_LINE_BUFFER_BIT_DEPTH_10                                      (0x00000001) /* R-XUV */
#define NV_DPCD14_DSC_LINE_BUFFER_BIT_DEPTH_11                                      (0x00000002) /* R-XUV */
#define NV_DPCD14_DSC_LINE_BUFFER_BIT_DEPTH_12                                      (0x00000003) /* R-XUV */
#define NV_DPCD14_DSC_LINE_BUFFER_BIT_DEPTH_13                                      (0x00000004) /* R-XUV */
#define NV_DPCD14_DSC_LINE_BUFFER_BIT_DEPTH_14                                      (0x00000005) /* R-XUV */
#define NV_DPCD14_DSC_LINE_BUFFER_BIT_DEPTH_15                                      (0x00000006) /* R-XUV */
#define NV_DPCD14_DSC_LINE_BUFFER_BIT_DEPTH_16                                      (0x00000007) /* R-XUV */
#define NV_DPCD14_DSC_LINE_BUFFER_BIT_DEPTH_8                                       (0x00000008) /* R-XUV */

#define NV_DPCD14_DSC_BLOCK_PREDICTION                                              (0x00000066) /* R-XUR */
#define NV_DPCD14_DSC_BLOCK_PREDICTION_SUPPORT                                               0:0 /* R-XUF */
#define NV_DPCD14_DSC_BLOCK_PREDICTION_SUPPORT_NO                                   (0x00000000) /* R-XUV */
#define NV_DPCD14_DSC_BLOCK_PREDICTION_SUPPORT_YES                                  (0x00000001) /* R-XUV */

#define NV_DPCD14_DSC_MAXIMUM_BITS_PER_PIXEL_1                                      (0x00000067) /* R-XUR */
#define NV_DPCD14_DSC_MAXIMUM_BITS_PER_PIXEL_1_LSB                                           7:0 /* R-XUF */

#define NV_DPCD14_DSC_MAXIMUM_BITS_PER_PIXEL_2                                      (0x00000068) /* R-XUR */
#define NV_DPCD14_DSC_MAXIMUM_BITS_PER_PIXEL_2_MSB                                           1:0 /* R-XUF */

#define NV_DPCD14_DSC_DECODER_COLOR_FORMAT_CAPABILITIES                             (0x00000069) /* R-XUR */
#define NV_DPCD14_DSC_DECODER_COLOR_FORMAT_CAPABILITIES_RGB                                  0:0 /* R-XUF */
#define NV_DPCD14_DSC_DECODER_COLOR_FORMAT_CAPABILITIES_RGB_NO                      (0x00000000) /* R-XUV */
#define NV_DPCD14_DSC_DECODER_COLOR_FORMAT_CAPABILITIES_RGB_YES                     (0x00000001) /* R-XUV */
#define NV_DPCD14_DSC_DECODER_COLOR_FORMAT_CAPABILITIES_YCbCr_444                            1:1 /* R-XUF */
#define NV_DPCD14_DSC_DECODER_COLOR_FORMAT_CAPABILITIES_YCbCr_444_NO                (0x00000000) /* R-XUV */
#define NV_DPCD14_DSC_DECODER_COLOR_FORMAT_CAPABILITIES_YCbCr_444_YES               (0x00000001) /* R-XUV */
#define NV_DPCD14_DSC_DECODER_COLOR_FORMAT_CAPABILITIES_YCbCr_SIMPLE_422                     2:2 /* R-XUF */
#define NV_DPCD14_DSC_DECODER_COLOR_FORMAT_CAPABILITIES_YCbCr_SIMPLE_422_NO         (0x00000000) /* R-XUV */
#define NV_DPCD14_DSC_DECODER_COLOR_FORMAT_CAPABILITIES_YCbCr_SIMPLE_422_YES        (0x00000001) /* R-XUV */
#define NV_DPCD14_DSC_DECODER_COLOR_FORMAT_CAPABILITIES_YCbCr_NATIVE_422                     3:3 /* R-XUF */
#define NV_DPCD14_DSC_DECODER_COLOR_FORMAT_CAPABILITIES_YCbCr_NATIVE_422_NO         (0x00000000) /* R-XUV */
#define NV_DPCD14_DSC_DECODER_COLOR_FORMAT_CAPABILITIES_YCbCr_NATIVE_422_YES        (0x00000001) /* R-XUV */
#define NV_DPCD14_DSC_DECODER_COLOR_FORMAT_CAPABILITIES_YCbCr_NATIVE_420                     4:4 /* R-XUF */
#define NV_DPCD14_DSC_DECODER_COLOR_FORMAT_CAPABILITIES_YCbCr_NATIVE_420_NO         (0x00000000) /* R-XUV */
#define NV_DPCD14_DSC_DECODER_COLOR_FORMAT_CAPABILITIES_YCbCr_NATIVE_420_YES        (0x00000001) /* R-XUV */

#define NV_DPCD14_DSC_DECODER_COLOR_DEPTH_CAPABILITIES                              (0x0000006A) /* R-XUR */
#define NV_DPCD14_DSC_DECODER_COLOR_DEPTH_CAPABILITIES_8_BITS_PER_COLOR                      1:1 /* R-XUF */
#define NV_DPCD14_DSC_DECODER_COLOR_DEPTH_CAPABILITIES_8_BITS_PER_COLOR_NO          (0x00000000) /* R-XUV */
#define NV_DPCD14_DSC_DECODER_COLOR_DEPTH_CAPABILITIES_8_BITS_PER_COLOR_YES         (0x00000001) /* R-XUV */
#define NV_DPCD14_DSC_DECODER_COLOR_DEPTH_CAPABILITIES_10_BITS_PER_COLOR                     2:2 /* R-XUF */
#define NV_DPCD14_DSC_DECODER_COLOR_DEPTH_CAPABILITIES_10_BITS_PER_COLOR_NO         (0x00000000) /* R-XUV */
#define NV_DPCD14_DSC_DECODER_COLOR_DEPTH_CAPABILITIES_10_BITS_PER_COLOR_YES        (0x00000001) /* R-XUV */
#define NV_DPCD14_DSC_DECODER_COLOR_DEPTH_CAPABILITIES_12_BITS_PER_COLOR                     3:3 /* R-XUF */
#define NV_DPCD14_DSC_DECODER_COLOR_DEPTH_CAPABILITIES_12_BITS_PER_COLOR_NO         (0x00000000) /* R-XUV */
#define NV_DPCD14_DSC_DECODER_COLOR_DEPTH_CAPABILITIES_12_BITS_PER_COLOR_YES        (0x00000001) /* R-XUV */

#define NV_DPCD14_DSC_PEAK_THROUGHPUT                                               (0x0000006B) /* R-XUR */
#define NV_DPCD14_DSC_PEAK_THROUGHPUT_MODE0                                                 3:0  /* R-XUF */
#define NV_DPCD14_DSC_PEAK_THROUGHPUT_MODE0_340                                     (0x00000001) /* R-XUV */
#define NV_DPCD14_DSC_PEAK_THROUGHPUT_MODE0_400                                     (0x00000002) /* R-XUV */
#define NV_DPCD14_DSC_PEAK_THROUGHPUT_MODE0_450                                     (0x00000003) /* R-XUV */
#define NV_DPCD14_DSC_PEAK_THROUGHPUT_MODE0_500                                     (0x00000004) /* R-XUV */
#define NV_DPCD14_DSC_PEAK_THROUGHPUT_MODE0_550                                     (0x00000005) /* R-XUV */
#define NV_DPCD14_DSC_PEAK_THROUGHPUT_MODE0_600                                     (0x00000006) /* R-XUV */
#define NV_DPCD14_DSC_PEAK_THROUGHPUT_MODE0_650                                     (0x00000007) /* R-XUV */
#define NV_DPCD14_DSC_PEAK_THROUGHPUT_MODE0_700                                     (0x00000008) /* R-XUV */
#define NV_DPCD14_DSC_PEAK_THROUGHPUT_MODE0_750                                     (0x00000009) /* R-XUV */
#define NV_DPCD14_DSC_PEAK_THROUGHPUT_MODE0_800                                     (0x0000000A) /* R-XUV */
#define NV_DPCD14_DSC_PEAK_THROUGHPUT_MODE0_850                                     (0x0000000B) /* R-XUV */
#define NV_DPCD14_DSC_PEAK_THROUGHPUT_MODE0_900                                     (0x0000000C) /* R-XUV */
#define NV_DPCD14_DSC_PEAK_THROUGHPUT_MODE0_950                                     (0x0000000D) /* R-XUV */
#define NV_DPCD14_DSC_PEAK_THROUGHPUT_MODE0_1000                                    (0x0000000E) /* R-XUV */
#define NV_DPCD14_DSC_PEAK_THROUGHPUT_MODE1                                                 7:4  /* R-XUF */
#define NV_DPCD14_DSC_PEAK_THROUGHPUT_MODE1_340                                     (0x00000001) /* R-XUV */
#define NV_DPCD14_DSC_PEAK_THROUGHPUT_MODE1_400                                     (0x00000002) /* R-XUV */
#define NV_DPCD14_DSC_PEAK_THROUGHPUT_MODE1_450                                     (0x00000003) /* R-XUV */
#define NV_DPCD14_DSC_PEAK_THROUGHPUT_MODE1_500                                     (0x00000004) /* R-XUV */
#define NV_DPCD14_DSC_PEAK_THROUGHPUT_MODE1_550                                     (0x00000005) /* R-XUV */
#define NV_DPCD14_DSC_PEAK_THROUGHPUT_MODE1_600                                     (0x00000006) /* R-XUV */
#define NV_DPCD14_DSC_PEAK_THROUGHPUT_MODE1_650                                     (0x00000007) /* R-XUV */
#define NV_DPCD14_DSC_PEAK_THROUGHPUT_MODE1_700                                     (0x00000008) /* R-XUV */
#define NV_DPCD14_DSC_PEAK_THROUGHPUT_MODE1_750                                     (0x00000009) /* R-XUV */
#define NV_DPCD14_DSC_PEAK_THROUGHPUT_MODE1_800                                     (0x0000000A) /* R-XUV */
#define NV_DPCD14_DSC_PEAK_THROUGHPUT_MODE1_850                                     (0x0000000B) /* R-XUV */
#define NV_DPCD14_DSC_PEAK_THROUGHPUT_MODE1_900                                     (0x0000000C) /* R-XUV */
#define NV_DPCD14_DSC_PEAK_THROUGHPUT_MODE1_950                                     (0x0000000D) /* R-XUV */
#define NV_DPCD14_DSC_PEAK_THROUGHPUT_MODE1_1000                                    (0x0000000E) /* R-XUV */

#define NV_DPCD14_DSC_MAXIMUM_SLICE_WIDTH                                           (0x0000006C) /* R-XUR */
#define NV_DPCD14_DSC_MAXIMUM_SLICE_WIDTH_MAX                                                7:0 /* R-XUF */

#define NV_DPCD14_DSC_SLICE_CAPABILITIES_2                                          (0x0000006D) /* R-XUR */
#define NV_DPCD14_DSC_SLICE_CAPABILITIES_2_SLICES_PER_SINK_16                                0:0 /* R-XUF */
#define NV_DPCD14_DSC_SLICE_CAPABILITIES_2_SLICES_PER_SINK_16_NO                    (0x00000000) /* R-XUV */
#define NV_DPCD14_DSC_SLICE_CAPABILITIES_2_SLICES_PER_SINK_16_YES                   (0x00000001) /* R-XUV */
#define NV_DPCD14_DSC_SLICE_CAPABILITIES_2_SLICES_PER_SINK_20                                1:1 /* R-XUF */
#define NV_DPCD14_DSC_SLICE_CAPABILITIES_2_SLICES_PER_SINK_20_NO                    (0x00000000) /* R-XUV */
#define NV_DPCD14_DSC_SLICE_CAPABILITIES_2_SLICES_PER_SINK_20_YES                   (0x00000001) /* R-XUV */
#define NV_DPCD14_DSC_SLICE_CAPABILITIES_2_SLICES_PER_SINK_24                                2:2 /* R-XUF */
#define NV_DPCD14_DSC_SLICE_CAPABILITIES_2_SLICES_PER_SINK_24_NO                    (0x00000000) /* R-XUV */
#define NV_DPCD14_DSC_SLICE_CAPABILITIES_2_SLICES_PER_SINK_24_YES                   (0x00000001) /* R-XUV */

#define NV_DPCD14_DSC_BITS_PER_PIXEL_INCREMENT                                      (0x0000006F) /* R-XUR */
#define NV_DPCD14_DSC_BITS_PER_PIXEL_INCREMENT_SUPPORTED                                     2:0 /* R-XUF */
#define NV_DPCD14_DSC_BITS_PER_PIXEL_INCREMENT_SUPPORTED_1_16                       (0x00000000) /* R-XUV */
#define NV_DPCD14_DSC_BITS_PER_PIXEL_INCREMENT_SUPPORTED_1_8                        (0x00000001) /* R-XUV */
#define NV_DPCD14_DSC_BITS_PER_PIXEL_INCREMENT_SUPPORTED_1_4                        (0x00000002) /* R-XUV */
#define NV_DPCD14_DSC_BITS_PER_PIXEL_INCREMENT_SUPPORTED_1_2                        (0x00000003) /* R-XUV */
#define NV_DPCD14_DSC_BITS_PER_PIXEL_INCREMENT_SUPPORTED_1                          (0x00000004) /* R-XUV */

// Field definition only used only with 128b/132b for DP2.0+
#define NV_DPCD20_TRAINING_LANE_SET(i)                                          (0x00000103+(i)) /* RW-1A */
#define NV_DPCD20_TRAINING_LANE_SET__SIZE                                                     4  /* RW--S */
#define NV_DPCD20_TRAINING_LANE_SET_TX_FFE_PRESET_VALUE                                     3:0  /* RWXUF */

#define NV_DPCD14_DSC_ENABLE                                                        (0x00000160) /* R-XUR */
#define NV_DPCD14_DSC_ENABLE_SINK                                                            0:0 /* R-XUF */
#define NV_DPCD14_DSC_ENABLE_SINK_NO                                                (0x00000000) /* R-XUV */
#define NV_DPCD14_DSC_ENABLE_SINK_YES                                               (0x00000001) /* R-XUV */

#define NV_DPCD14_FEC_CAPABILITY                                                    (0x00000090) /* R-XUR */
#define NV_DPCD14_FEC_CAPABILITY_FEC_CAPABLE                                                 0:0 /* R-XUF */
#define NV_DPCD14_FEC_CAPABILITY_FEC_CAPABLE_NO                                     (0x00000000) /* R-XUV */
#define NV_DPCD14_FEC_CAPABILITY_FEC_CAPABLE_YES                                    (0x00000001) /* R-XUV */
#define NV_DPCD14_FEC_CAPABILITY_UNCORRECTED_BLOCK_ERROR_COUNT_CAPABLE                       1:1 /* R-XUF */
#define NV_DPCD14_FEC_CAPABILITY_UNCORRECTED_BLOCK_ERROR_COUNT_CAPABLE_NO           (0x00000000) /* R-XUV */
#define NV_DPCD14_FEC_CAPABILITY_UNCORRECTED_BLOCK_ERROR_COUNT_CAPABLE_YES          (0x00000001) /* R-XUV */
#define NV_DPCD14_FEC_CAPABILITY_CORRECTED_BLOCK_ERROR_COUNT_CAPABLE                         2:2 /* R-XUF */
#define NV_DPCD14_FEC_CAPABILITY_CORRECTED_BLOCK_ERROR_COUNT_CAPABLE_NO             (0x00000000) /* R-XUV */
#define NV_DPCD14_FEC_CAPABILITY_CORRECTED_BLOCK_ERROR_COUNT_CAPABLE_YES            (0x00000001) /* R-XUV */
#define NV_DPCD14_FEC_CAPABILITY_BIT_ERROR_COUNT_CAPABLE                                     3:3 /* R-XUF */
#define NV_DPCD14_FEC_CAPABILITY_BIT_ERROR_COUNT_CAPABLE_NO                         (0x00000000) /* R-XUV */
#define NV_DPCD14_FEC_CAPABILITY_BIT_ERROR_COUNT_CAPABLE_YES                        (0x00000001) /* R-XUV */
#define NV_DPCD14_FEC_CAPABILITY_PARITY_BLOCK_ERROR_COUNT_CAPABLE                            4:4 /* R-XUF */
#define NV_DPCD14_FEC_CAPABILITY_PARITY_BLOCK_ERROR_COUNT_CAPABLE_NO                (0x00000000) /* R-XUV */
#define NV_DPCD14_FEC_CAPABILITY_PARITY_BLOCK_ERROR_COUNT_CAPABLE_YES               (0x00000001) /* R-XUV */
#define NV_DPCD14_FEC_CAPABILITY_PARITY_ERROR_COUNT_CAPABLE                                  5:5 /* R-XUF */
#define NV_DPCD14_FEC_CAPABILITY_PARITY_ERROR_COUNT_CAPABLE_NO                      (0x00000000) /* R-XUV */
#define NV_DPCD14_FEC_CAPABILITY_PARITY_ERROR_COUNT_CAPABLE_YES                     (0x00000001) /* R-XUV */
// Bit 6 : RESERVED. Read 0
#define NV_DPCD14_FEC_CAPABILITY_FEC_ERROR_REPORTING_POLICY_SUPPORTED                        7:7 /* R-XUF */
#define NV_DPCD14_FEC_CAPABILITY_FEC_ERROR_REPORTING_POLICY_SUPPORTED_NO            (0x00000000) /* R-XUV */
#define NV_DPCD14_FEC_CAPABILITY_FEC_ERROR_REPORTING_POLICY_SUPPORTED_YES           (0x00000001) /* R-XUV */

#define NV_DPCD14_TRAINING_PATTERN_SET                                              (0x00000102) /* RWXUR */
#define NV_DPCD14_TRAINING_PATTERN_SET_TPS                                                   3:0 /* RWXUF */
#define NV_DPCD14_TRAINING_PATTERN_SET_TPS_NONE                                     (0x00000000) /* RWXUV */
#define NV_DPCD14_TRAINING_PATTERN_SET_TPS_TP1                                      (0x00000001) /* RWXUV */
#define NV_DPCD14_TRAINING_PATTERN_SET_TPS_TP2                                      (0x00000002) /* RWXUV */
#define NV_DPCD14_TRAINING_PATTERN_SET_TPS_TP3                                      (0x00000003) /* RWXUV */
#define NV_DPCD14_TRAINING_PATTERN_SET_TPS_TP4                                      (0x00000007) /* RWXUV */
#define NV_DPCD14_TRAINING_PATTERN_SET_RECOVERED_CLOCK_OUT_EN                                4:4 /* RWXUF */
#define NV_DPCD14_TRAINING_PATTERN_SET_RECOVERED_CLOCK_OUT_EN_NO                    (0x00000000) /* RWXUV */
#define NV_DPCD14_TRAINING_PATTERN_SET_RECOVERED_CLOCK_OUT_EN_YES                   (0x00000001) /* RWXUV */
#define NV_DPCD14_TRAINING_PATTERN_SET_SCRAMBLING_DISABLED                                   5:5 /* RWXUF */
#define NV_DPCD14_TRAINING_PATTERN_SET_SCRAMBLING_DISABLED_FALSE                    (0x00000000) /* RWXUV */
#define NV_DPCD14_TRAINING_PATTERN_SET_SCRAMBLING_DISABLED_TRUE                     (0x00000001) /* RWXUV */
#define NV_DPCD14_TRAINING_PATTERN_SET_SYM_ERR_SEL                                           7:6 /* RWXUF */
#define NV_DPCD14_TRAINING_PATTERN_SET_SYM_ERR_SEL_DISPARITY_ILLEGAL_SYMBOL_ERROR   (0x00000000) /* RWXUV */
#define NV_DPCD14_TRAINING_PATTERN_SET_SYM_ERR_SEL_DISPARITY_ERROR                  (0x00000001) /* RWXUV */
#define NV_DPCD14_TRAINING_PATTERN_SET_SYM_ERR_SEL_ILLEGAL_SYMBOL_ERROR             (0x00000002) /* RWXUV */

// Field definition only used only with 128b/132b for DP2.0+
#define NV_DPCD20_128B_132B_TRAINING_PATTERN                                        (0x00000102) /* RWXUR */
#define NV_DPCD20_128B_132B_TRAINING_PATTERN_SELECT                                          3:0 /* RWXUF */
#define NV_DPCD20_128B_132B_TRAINING_PATTERN_SELECT_NONE                            (0x00000000) /* RWXUV */
#define NV_DPCD20_128B_132B_TRAINING_PATTERN_SELECT_TPS1                            (0x00000001) /* RWXUV */
#define NV_DPCD20_128B_132B_TRAINING_PATTERN_SELECT_TPS2                            (0x00000002) /* RWXUV */
#define NV_DPCD20_128B_132B_TRAINING_PATTERN_SELECT_TPS2_CDS                        (0x00000003) /* RWXUV */
// Note: Bit 7:4 are reserved for 128b/132b. Driver should keep them 0

#define NV_DPCD14_LINK_QUAL_LANE_SET(i)                                         (0x0000010B+(i)) /* RW-1A */
#define NV_DPCD14_LINK_QUAL_LANE_SET__SIZE                                                    4  /* R---S */
#define NV_DPCD14_LINK_QUAL_LANE_SET_LQS                                                    2:0  /* RWXUF */
#define NV_DPCD14_LINK_QUAL_LANE_SET_LQS_CP2520PAT3                                 (0x00000007) /* RWXUV */

#define NV_DPCD14_FEC_CONFIGURATION                                                     (0x00000120) /* RWXUR */
#define NV_DPCD14_FEC_CONFIGURATION_FEC_READY                                                    0:0 /* RWXUF */
#define NV_DPCD14_FEC_CONFIGURATION_FEC_READY_NO                                        (0x00000000) /* RWXUV */
#define NV_DPCD14_FEC_CONFIGURATION_FEC_READY_YES                                       (0x00000001) /* RWXUV */
#define NV_DPCD14_FEC_CONFIGURATION_FEC_ERROR_COUNT_SEL                                          3:1 /* RWXUF */
#define NV_DPCD14_FEC_CONFIGURATION_FEC_ERROR_COUNT_SEL_FEC_ERROR_COUNT_DIS             (0x00000000) /* RWXUV */
#define NV_DPCD14_FEC_CONFIGURATION_FEC_ERROR_COUNT_SEL_UNCORRECTED_BLOCK_ERROR_COUNT   (0x00000001) /* RWXUV */
#define NV_DPCD14_FEC_CONFIGURATION_FEC_ERROR_COUNT_SEL_CORRECTED_BLOCK_ERROR_COUNT     (0x00000002) /* RWXUV */
#define NV_DPCD14_FEC_CONFIGURATION_FEC_ERROR_COUNT_SEL_BIT_ERROR_COUNT                 (0x00000003) /* RWXUV */
#define NV_DPCD14_FEC_CONFIGURATION_FEC_ERROR_COUNT_SEL_PARITY_BLOCK_ERROR_COUNT        (0x00000004) /* RWXUV */
#define NV_DPCD14_FEC_CONFIGURATION_FEC_ERROR_COUNT_SEL_PARITY_BIT_ERROR_COUNT          (0x00000005) /* RWXUV */
#define NV_DPCD14_FEC_CONFIGURATION_LANE_SELECT                                                  5:4 /* RWXUF */
#define NV_DPCD14_FEC_CONFIGURATION_LANE_SELECT_LANE_0                                  (0x00000000) /* RWXUV */
#define NV_DPCD14_FEC_CONFIGURATION_LANE_SELECT_LANE_1                                  (0x00000001) /* RWXUV */
#define NV_DPCD14_FEC_CONFIGURATION_LANE_SELECT_LANE_2                                  (0x00000002) /* RWXUV */
#define NV_DPCD14_FEC_CONFIGURATION_LANE_SELECT_LANE_3                                  (0x00000003) /* RWXUV */

// Field definition only used only with 128b/132b for DP2.0+
#define NV_DPCD20_LANE_ALIGN_STATUS_UPDATED                                                  (0x00000204) /* R-XUR */
#define NV_DPCD20_LANE_ALIGN_STATUS_UPDATED_128B_132B_DPRX_EQ_INTERLANE_ALIGN_DONE                   2:2  /* R-XUF */
#define NV_DPCD20_LANE_ALIGN_STATUS_UPDATED_128B_132B_DPRX_EQ_INTERLANE_ALIGN_DONE_NO        (0x00000000) /* R-XUV */
#define NV_DPCD20_LANE_ALIGN_STATUS_UPDATED_128B_132B_DPRX_EQ_INTERLANE_ALIGN_DONE_YES       (0x00000001) /* R-XUV */
#define NV_DPCD20_LANE_ALIGN_STATUS_UPDATED_128B_132B_DPRX_CDS_INTERLANE_ALIGN_DONE                  3:3  /* R-XUF */
#define NV_DPCD20_LANE_ALIGN_STATUS_UPDATED_128B_132B_DPRX_CDS_INTERLANE_ALIGN_DONE_NO       (0x00000000) /* R-XUV */
#define NV_DPCD20_LANE_ALIGN_STATUS_UPDATED_128B_132B_DPRX_CDS_INTERLANE_ALIGN_DONE_YES      (0x00000001) /* R-XUV */
#define NV_DPCD20_LANE_ALIGN_STATUS_UPDATED_128B_132B_LT_FAILED                                      4:4  /* R-XUF */
#define NV_DPCD20_LANE_ALIGN_STATUS_UPDATED_128B_132B_LT_FAILED_NO                           (0x00000000) /* R-XUV */
#define NV_DPCD20_LANE_ALIGN_STATUS_UPDATED_128B_132B_LT_FAILED_YES                          (0x00000001) /* R-XUV */

// Field definition for 0x0206/0x0207h (ADJUST_REQUEST_LANEX), only used only with 128b/132b for DP2.0+
#define NV_DPCD20_LANEX_XPLUS1_ADJUST_REQ_LANEX_TX_FFE_PRESET_VALUE              3:0  /* R-XUF */
#define NV_DPCD20_LANEX_XPLUS1_ADJUST_REQ_LANEXPLUS1_TX_FFE_PRESET_VALUE         7:4  /* R-XUF */

// PANEL REPLAY RELATED DPCD
#define NV_DPCD20_PANEL_REPLAY_CAPABILITY                                         (0x000000B0)
#define NV_DPCD20_PANEL_REPLAY_CAPABILITY_SUPPORTED                                       0:0
#define NV_DPCD20_PANEL_REPLAY_CAPABILITY_SUPPORTED_NO                            (0x00000000)
#define NV_DPCD20_PANEL_REPLAY_CAPABILITY_SUPPORTED_YES                           (0x00000001)
#define NV_DPCD20_PANEL_REPLAY_CAPABILITY_SEL_UPDATE                                      1:1
#define NV_DPCD20_PANEL_REPLAY_CAPABILITY_SEL_UPDATE_NO                           (0x00000000)
#define NV_DPCD20_PANEL_REPLAY_CAPABILITY_SEL_UPDATE_YES                          (0x00000001)

#define NV_DPCD20_PANEL_REPLAY_CONFIGURATION                                      (0x000001B0)
#define NV_DPCD20_PANEL_REPLAY_CONFIGURATION_ENABLE_PR_MODE                               0:0
#define NV_DPCD20_PANEL_REPLAY_CONFIGURATION_ENABLE_PR_MODE_NO                    (0x00000000)
#define NV_DPCD20_PANEL_REPLAY_CONFIGURATION_ENABLE_PR_MODE_YES                   (0x00000001)

#define NV_DPCD14_PHY_TEST_PATTERN                                                  (0x00000248) /* R-XUR */
#define NV_DPCD14_PHY_TEST_PATTERN_SEL_CP2520PAT3                                   (0x00000007) /* R-XUV */

#define NV_DPCD14_DSC_CRC_0                                                         (0x00000262) /* R-XUR */
#define NV_DPCD14_DSC_CRC_0_LOW_BYTE                                         NV_DPCD14_DSC_CRC_0
#define NV_DPCD14_DSC_CRC_0_HIGH_BYTE                                               (0x00000263) /* R-XUR */
#define NV_DPCD14_DSC_CRC_1                                                         (0x00000264) /* R-XUR */
#define NV_DPCD14_DSC_CRC_1_LOW_BYTE                                         NV_DPCD14_DSC_CRC_1
#define NV_DPCD14_DSC_CRC_1_HIGH_BYTE                                               (0x00000265) /* R-XUR */
#define NV_DPCD14_DSC_CRC_2                                                         (0x00000266) /* R-XUR */
#define NV_DPCD14_DSC_CRC_2_LOW_BYTE                                         NV_DPCD14_DSC_CRC_2
#define NV_DPCD14_DSC_CRC_2_HIGH_BYTE                                               (0x00000267) /* R-XUR */

#define NV_DPCD14_FEC_STATUS                                                        (0x00000280) /* R-XUR */
#define NV_DPCD14_FEC_STATUS_FEC_DECODE_EN_DETECTED                                         0:0  /* R-XUF */
#define NV_DPCD14_FEC_STATUS_FEC_DECODE_EN_DETECTED_NO                              (0x00000000) /* R-XUV */
#define NV_DPCD14_FEC_STATUS_FEC_DECODE_EN_DETECTED_YES                             (0x00000001) /* R-XUV */
#define NV_DPCD14_FEC_STATUS_FEC_DECODE_DIS_DETECTED                                        1:1  /* R-XUF */
#define NV_DPCD14_FEC_STATUS_FEC_DECODE_DIS_DETECTED_NO                             (0x00000000) /* R-XUV */
#define NV_DPCD14_FEC_STATUS_FEC_DECODE_DIS_DETECTED_YES                            (0x00000001) /* R-XUV */
// Bits 7-2: RESERVED.
#define NV_DPCD14_FEC_STATUS_CLEAR                                                  (0x00000001)

#define NV_DPCD14_FEC_ERROR_COUNT                                                   (0x00000281) /* R-XUR */
#define NV_DPCD14_FEC_ERROR_COUNT_FEC_ERROR_COUNT_LOW_BYTE             NV_DPCD14_FEC_ERROR_COUNT
#define NV_DPCD14_FEC_ERROR_COUNT_FEC_ERROR_COUNT_HIGH_BYTE                         (0x00000282) /* R-XUR */
#define NV_DPCD14_FEC_ERROR_COUNT_FEC_ERROR_COUNT_VALID                                     7:7  /* R-XUF */
#define NV_DPCD14_FEC_ERROR_COUNT_FEC_ERROR_COUNT_VALID_NO                          (0x00000000) /* R-XUV */
#define NV_DPCD14_FEC_ERROR_COUNT_FEC_ERROR_COUNT_VALID_YES                         (0x00000001) /* R-XUV */

// Field definition for 0x0200E (LANE_ALIGN_STATUS_UPDATED_ESI), used only when DP2.0+ 128b/132b is enabled.
#define NV_DPCD20_LANE_ALIGN_STATUS_UPDATED_ESI                                              (0x0000200E) /* R-XUR */
#define NV_DPCD20_LANE_ALIGN_STATUS_UPDATED_ESI_128B_132B_DPRX_EQ_INTERLANE_ALIGN_DONE               2:2  /* R-XUF */
#define NV_DPCD20_LANE_ALIGN_STATUS_UPDATED_ESI_128B_132B_DPRX_EQ_INTERLANE_ALIGN_DONE_NO    (0x00000000) /* R-XUV */
#define NV_DPCD20_LANE_ALIGN_STATUS_UPDATED_ESI_128B_132B_DPRX_EQ_INTERLANE_ALIGN_DONE_YES   (0x00000001) /* R-XUV */
#define NV_DPCD20_LANE_ALIGN_STATUS_UPDATED_ESI_128B_132B_DPRX_CDS_INTERLANE_ALIGN_DONE              3:3  /* R-XUF */
#define NV_DPCD20_LANE_ALIGN_STATUS_UPDATED_ESI_128B_132B_DPRX_CDS_INTERLANE_ALIGN_DONE_NO   (0x00000000) /* R-XUV */
#define NV_DPCD20_LANE_ALIGN_STATUS_UPDATED_ESI_128B_132B_DPRX_CDS_INTERLANE_ALIGN_DONE_YES  (0x00000001) /* R-XUV */
#define NV_DPCD20_LANE_ALIGN_STATUS_UPDATED_ESI_128B_132B_LT_FAILED                                  4:4  /* R-XUF */
#define NV_DPCD20_LANE_ALIGN_STATUS_UPDATED_ESI_128B_132B_LT_FAILED_NO                       (0x00000000) /* R-XUV */
#define NV_DPCD20_LANE_ALIGN_STATUS_UPDATED_ESI_128B_132B_LT_FAILED_YES                      (0x00000001) /* R-XUV */

// Field definition for 0x0200F (SINK_STATUS_ESI), used only when DP2.0+ 128b/132b is enabled.
#define NV_DPCD20_SINK_STATUS_ESI                                         (0x0000200F) /* R-XUR */
#define NV_DPCD20_SINK_STATUS_ESI_INTRA_HOP_AUX_REPLY                             3:3  /* R-XUF */
#define NV_DPCD20_SINK_STATUS_ESI_INTRA_HOP_AUX_REPLY_DPRX                (0x00000000) /* R-XUV */
#define NV_DPCD20_SINK_STATUS_ESI_INTRA_HOP_AUX_REPLY_LTTPR               (0x00000001) /* R-XUV */

#define NV_DPCD14_EXTENDED_REV                                                      (0x00002200) /* R-XUR */
#define NV_DPCD14_EXTENDED_REV_MAJOR                                                         7:4 /* R-XUF */
#define NV_DPCD14_EXTENDED_REV_MAJOR_1                                              (0x00000001) /* R-XUV */
#define NV_DPCD14_EXTENDED_REV_MINOR                                                         3:0 /* R-XUF */
#define NV_DPCD14_EXTENDED_REV_MINOR_4                                              (0x00000004) /* R-XUV */

#define NV_DPCD14_EXTENDED_MAX_LINK_BANDWIDTH                                       (0x00002201) /* R-XUR */
#define NV_DPCD14_EXTENDED_MAX_LINK_BANDWIDTH_VAL                                            7:0 /* R-XUF */
#define NV_DPCD14_EXTENDED_MAX_LINK_BANDWIDTH_VAL_8_10_GBPS                         (0x0000001E) /* R-XUV */

#define NV_DPCD14_EXTENDED_MAX_LANE_COUNT                                           (0x00002202) /* R-XUR */
#define NV_DPCD14_EXTENDED_MAX_LANE_COUNT_LANE                                               4:0 /* R-XUF */
#define NV_DPCD14_EXTENDED_MAX_LANE_COUNT_LANE_1                                    (0x00000001) /* R-XUV */
#define NV_DPCD14_EXTENDED_MAX_LANE_COUNT_LANE_2                                    (0x00000002) /* R-XUV */
#define NV_DPCD14_EXTENDED_MAX_LANE_COUNT_LANE_4                                    (0x00000004) /* R-XUV */
#define NV_DPCD14_EXTENDED_MAX_LANE_COUNT_POST_LT_ADJ_REQ_SUPPORT                            5:5 /* R-XUF */
#define NV_DPCD14_EXTENDED_MAX_LANE_COUNT_POST_LT_ADJ_REQ_SUPPORT_NO                (0x00000000) /* R-XUV */
#define NV_DPCD14_EXTENDED_MAX_LANE_COUNT_POST_LT_ADJ_REQ_SUPPORT_YES               (0x00000001) /* R-XUV */
#define NV_DPCD14_EXTENDED_MAX_LANE_COUNT_TPS3_SUPPORTED                                     6:6 /* R-XUF */
#define NV_DPCD14_EXTENDED_MAX_LANE_COUNT_TPS3_SUPPORTED_NO                         (0x00000000) /* R-XUV */
#define NV_DPCD14_EXTENDED_MAX_LANE_COUNT_TPS3_SUPPORTED_YES                        (0x00000001) /* R-XUV */
#define NV_DPCD14_EXTENDED_MAX_LANE_COUNT_ENHANCED_FRAMING                                   7:7 /* R-XUF */
#define NV_DPCD14_EXTENDED_MAX_LANE_COUNT_ENHANCED_FRAMING_NO                       (0x00000000) /* R-XUV */
#define NV_DPCD14_EXTENDED_MAX_LANE_COUNT_ENHANCED_FRAMING_YES                      (0x00000001) /* R-XUV */

#define NV_DPCD14_EXTENDED_MAX_DOWNSPREAD                                           (0x00002203) /* R-XUR */
#define NV_DPCD14_EXTENDED_MAX_DOWNSPREAD_VAL                                                0:0 /* R-XUF */
#define NV_DPCD14_EXTENDED_MAX_DOWNSPREAD_VAL_NONE                                  (0x00000000) /* R-XUV */
#define NV_DPCD14_EXTENDED_MAX_DOWNSPREAD_VAL_0_5_PCT                               (0x00000001) /* R-XUV */
#define NV_DPCD14_EXTENDED_MAX_DOWNSPREAD_NO_AUX_HANDSHAKE_LT                                6:6 /* R-XUF */
#define NV_DPCD14_EXTENDED_MAX_DOWNSPREAD_NO_AUX_HANDSHAKE_LT_FALSE                 (0x00000000) /* R-XUV */
#define NV_DPCD14_EXTENDED_MAX_DOWNSPREAD_NO_AUX_HANDSHAKE_LT_TRUE                  (0x00000001) /* R-XUV */
#define NV_DPCD14_EXTENDED_MAX_DOWNSPREAD_TPS4_SUPPORTED                                     7:7 /* R-XUF */
#define NV_DPCD14_EXTENDED_MAX_DOWNSPREAD_TPS4_SUPPORTED_NO                         (0x00000000) /* R-XUV */
#define NV_DPCD14_EXTENDED_MAX_DOWNSPREAD_TPS4_SUPPORTED_YES                        (0x00000001) /* R-XUV */

// NORP = Number of Receiver Ports = Value + 1
#define NV_DPCD14_EXTENDED_NORP                                                     (0x00002204) /* R-XUR */
#define NV_DPCD14_EXTENDED_NORP_VAL                                                          0:0 /* R-XUF */
#define NV_DPCD14_EXTENDED_NORP_VAL_ONE                                             (0x00000000) /* R-XUV */
#define NV_DPCD14_EXTENDED_NORP_VAL_TWO                                             (0x00000001) /* R-XUV */
#define NV_DPCD14_EXTENDED_NORP_VAL_SST_MAX                                         (0x00000001) /* R-XUV */
#define NV_DPCD14_EXTENDED_NORP_DP_PWR_CAP_5V                                                5:5 /* R-XUF */
#define NV_DPCD14_EXTENDED_NORP_DP_PWR_CAP_12V                                               6:6 /* R-XUF */
#define NV_DPCD14_EXTENDED_NORP_DP_PWR_CAP_18V                                               7:7 /* R-XUF */

#define NV_DPCD14_EXTENDED_DOWNSTREAMPORT                                           (0x00002205) /* R-XUR */
#define NV_DPCD14_EXTENDED_DOWNSTREAMPORT_PRESENT                                            0:0 /* R-XUF */
#define NV_DPCD14_EXTENDED_DOWNSTREAMPORT_PRESENT_NO                                (0x00000000) /* R-XUV */
#define NV_DPCD14_EXTENDED_DOWNSTREAMPORT_PRESENT_YES                               (0x00000001) /* R-XUV */
#define NV_DPCD14_EXTENDED_DOWNSTREAMPORT_TYPE                                               2:1 /* R-XUF */
#define NV_DPCD14_EXTENDED_DOWNSTREAMPORT_TYPE_DISPLAYPORT                          (0x00000000) /* R-XUV */
#define NV_DPCD14_EXTENDED_DOWNSTREAMPORT_TYPE_ANALOG                               (0x00000001) /* R-XUV */
#define NV_DPCD14_EXTENDED_DOWNSTREAMPORT_TYPE_HDMI_DVI                             (0x00000002) /* R-XUV */
#define NV_DPCD14_EXTENDED_DOWNSTREAMPORT_TYPE_OTHERS                               (0x00000003) /* R-XUV */
#define NV_DPCD14_EXTENDED_DOWNSTREAMPORT_FORMAT_CONVERSION                                  3:3 /* R-XUF */
#define NV_DPCD14_EXTENDED_DOWNSTREAMPORT_FORMAT_CONVERSION_NO                      (0x00000000) /* R-XUV */
#define NV_DPCD14_EXTENDED_DOWNSTREAMPORT_FORMAT_CONVERSION_YES                     (0x00000001) /* R-XUV */
#define NV_DPCD14_EXTENDED_DOWNSTREAMPORT_DETAILED_CAP_INFO_AVAILABLE                        4:4 /* R-XUF */
#define NV_DPCD14_EXTENDED_DOWNSTREAMPORT_DETAILED_CAP_INFO_AVAILABLE_NO            (0x00000000) /* R-XUV */
#define NV_DPCD14_EXTENDED_DOWNSTREAMPORT_DETAILED_CAP_INFO_AVAILABLE_YES           (0x00000001) /* R-XUV */

#define NV_DPCD14_EXTENDED_MAIN_LINK_CHANNEL_CODING                                 (0x00002206) /* R-XUR */
#define NV_DPCD14_EXTENDED_MAIN_LINK_CHANNEL_CODING_ANSI_8B_10B                              0:0 /* R-XUF */
#define NV_DPCD14_EXTENDED_MAIN_LINK_CHANNEL_CODING_ANSI_8B_10B_NO                  (0x00000000) /* R-XUV */
#define NV_DPCD14_EXTENDED_MAIN_LINK_CHANNEL_CODING_ANSI_8B_10B_YES                 (0x00000001) /* R-XUV */
#define NV_DPCD14_EXTENDED_MAIN_LINK_CHANNEL_CODING_ANSI_128B_132B                           1:1 /* R-XUF */
#define NV_DPCD14_EXTENDED_MAIN_LINK_CHANNEL_CODING_ANSI_128B_132B_NO               (0x00000000) /* R-XUV */
#define NV_DPCD14_EXTENDED_MAIN_LINK_CHANNEL_CODING_ANSI_128B_132B_YES              (0x00000001) /* R-XUV */

#define NV_DPCD14_EXTENDED_DOWN_STREAM_PORT                                         (0x00002207) /* R-XUR */
#define NV_DPCD14_EXTENDED_DOWN_STREAM_PORT_COUNT                                            3:0 /* R-XUF */
#define NV_DPCD14_EXTENDED_DOWN_STREAM_PORT_MSA_TIMING_PAR_IGNORED                           6:6 /* R-XUF */
#define NV_DPCD14_EXTENDED_DOWN_STREAM_PORT_MSA_TIMING_PAR_IGNORED_NO               (0x00000000) /* R-XUV */
#define NV_DPCD14_EXTENDED_DOWN_STREAM_PORT_MSA_TIMING_PAR_IGNORED_YES              (0x00000001) /* R-XUV */
#define NV_DPCD14_EXTENDED_DOWN_STREAM_PORT_OUI_SUPPORT                                      7:7 /* R-XUF */
#define NV_DPCD14_EXTENDED_DOWN_STREAM_PORT_OUI_SUPPORT_NO                          (0x00000000) /* R-XUV */
#define NV_DPCD14_EXTENDED_DOWN_STREAM_PORT_OUI_SUPPORT_YES                         (0x00000001) /* R-XUV */

#define NV_DPCD14_EXTENDED_RECEIVE_PORT0_CAP_0                                      (0x00002208) /* R-XUR */
#define NV_DPCD14_EXTENDED_RECEIVE_PORT1_CAP_0                                      (0x0000220A) /* R-XUR */
#define NV_DPCD14_EXTENDED_RECEIVE_PORTX_CAP_0_LOCAL_EDID                                    1:1 /* R-XUF */
#define NV_DPCD14_EXTENDED_RECEIVE_PORTX_CAP_0_LOCAL_EDID_NO                        (0x00000000) /* R-XUV */
#define NV_DPCD14_EXTENDED_RECEIVE_PORTX_CAP_0_LOCAL_EDID_YES                       (0x00000001) /* R-XUV */
#define NV_DPCD14_EXTENDED_RECEIVE_PORTX_CAP_0_ASSO_TO_PRECEDING_PORT                        2:2 /* R-XUF */
#define NV_DPCD14_EXTENDED_RECEIVE_PORTX_CAP_0_ASSO_TO_PRECEDING_PORT_NO            (0x00000000) /* R-XUV */
#define NV_DPCD14_EXTENDED_RECEIVE_PORTX_CAP_0_ASSO_TO_PRECEDING_PORT_YES           (0x00000001) /* R-XUV */
#define NV_DPCD14_EXTENDED_RECEIVE_PORTX_CAP_0_HBLANK_EXPANSION_CAPABLE                      3:3 /* R-XUF */
#define NV_DPCD14_EXTENDED_RECEIVE_PORTX_CAP_0_HBLANK_EXPANSION_CAPABLE_NO          (0x00000000) /* R-XUV */
#define NV_DPCD14_EXTENDED_RECEIVE_PORTX_CAP_0_HBLANK_EXPANSION_CAPABLE_YES         (0x00000001) /* R-XUV */
#define NV_DPCD14_EXTENDED_RECEIVE_PORTX_CAP_0_BUFFER_SIZE_UNIT                              4:4 /* R-XUF */
#define NV_DPCD14_EXTENDED_RECEIVE_PORTX_CAP_0_BUFFER_SIZE_UNIT_PIXEL               (0x00000000) /* R-XUV */
#define NV_DPCD14_EXTENDED_RECEIVE_PORTX_CAP_0_BUFFER_SIZE_UNIT_BYTE                (0x00000001) /* R-XUV */
#define NV_DPCD14_EXTENDED_RECEIVE_PORTX_CAP_0_BUFFER_SIZE_PER_PORT                          5:5 /* R-XUF */
#define NV_DPCD14_EXTENDED_RECEIVE_PORTX_CAP_0_BUFFER_SIZE_PER_PORT_NO              (0x00000000) /* R-XUV */
#define NV_DPCD14_EXTENDED_RECEIVE_PORTX_CAP_0_BUFFER_SIZE_PER_PORT_YES             (0x00000001) /* R-XUV */

#define NV_DPCD14_EXTENDED_RECEIVE_PORT0_CAP_1                                      (0x00002209) /* R-XUR */
#define NV_DPCD14_EXTENDED_RECEIVE_PORT1_CAP_1                                      (0x0000220B) /* R-XUR */
#define NV_DPCD14_EXTENDED_RECEIVE_PORTX_CAP_1_BUFFER_SIZE                                   7:0 /* R-XUF */

#define NV_DPCD14_EXTENDED_I2C_CTRL_CAP                                             (0x0000220C) /* R-XUR */
#define NV_DPCD14_EXTENDED_I2C_CTRL_CAP_SPEED                                                7:0 /* R-XUF */
#define NV_DPCD14_EXTENDED_I2C_CTRL_CAP_SPEED_1K                                    (0x00000001) /* R-XUV */
#define NV_DPCD14_EXTENDED_I2C_CTRL_CAP_SPEED_5K                                    (0x00000002) /* R-XUV */
#define NV_DPCD14_EXTENDED_I2C_CTRL_CAP_SPEED_10K                                   (0x00000004) /* R-XUV */
#define NV_DPCD14_EXTENDED_I2C_CTRL_CAP_SPEED_100K                                  (0x00000008) /* R-XUV */
#define NV_DPCD14_EXTENDED_I2C_CTRL_CAP_SPEED_400K                                  (0x00000010) /* R-XUV */
#define NV_DPCD14_EXTENDED_I2C_CTRL_CAP_SPEED_1M                                    (0x00000020) /* R-XUV */

#define NV_DPCD14_EXTENDED_EDP_CONFIG_CAP                                           (0x0000220D) /* R-XUR */
#define NV_DPCD14_EXTENDED_EDP_CONFIG_CAP_ALTERNATE_SCRAMBLER_RESET                          0:0 /* R-XUF */
#define NV_DPCD14_EXTENDED_EDP_CONFIG_CAP_ALTERNATE_SCRAMBLER_RESET_NO              (0x00000000) /* R-XUV */
#define NV_DPCD14_EXTENDED_EDP_CONFIG_CAP_ALTERNATE_SCRAMBLER_RESET_YES             (0x00000001) /* R-XUV */

#define NV_DPCD14_EXTENDED_TRAINING_AUX_RD_INTERVAL                                 (0x0000220E) /* R-XUR */
#define NV_DPCD14_EXTENDED_TRAINING_AUX_RD_INTERVAL_VAL                                      6:0 /* R-XUF */
#define NV_DPCD14_EXTENDED_TRAINING_AUX_RD_INTERVAL_VAL_DEFAULT                     (0x00000000) /* R-XUV */
#define NV_DPCD14_EXTENDED_TRAINING_AUX_RD_INTERVAL_VAL_4MS                         (0x00000001) /* R-XUV */
#define NV_DPCD14_EXTENDED_TRAINING_AUX_RD_INTERVAL_VAL_8MS                         (0x00000002) /* R-XUV */
#define NV_DPCD14_EXTENDED_TRAINING_AUX_RD_INTERVAL_VAL_12MS                        (0x00000003) /* R-XUV */
#define NV_DPCD14_EXTENDED_TRAINING_AUX_RD_INTERVAL_VAL_16MS                        (0x00000004) /* R-XUV */
#define NV_DPCD14_EXTENDED_TRAINING_AUX_RD_INTERVAL_EXTENDED_RECEIVER_CAP                    7:7 /* R-XUF */
#define NV_DPCD14_EXTENDED_TRAINING_AUX_RD_INTERVAL_EXTENDED_RECEIVER_CAP_NO        (0x00000000) /* R-XUV */
#define NV_DPCD14_EXTENDED_TRAINING_AUX_RD_INTERVAL_EXTENDED_RECEIVER_CAP_YES       (0x00000001) /* R-XUV */

#define NV_DPCD14_EXTENDED_ADAPTER_CAP                                              (0x0000220F) /* R-XUR */
#define NV_DPCD14_EXTENDED_ADAPTER_CAP_FORCE_LOAD_SENSE                                      0:0 /* R-XUF */
#define NV_DPCD14_EXTENDED_ADAPTER_CAP_FORCE_LOAD_SENSE_NO                          (0x00000000) /* R-XUV */
#define NV_DPCD14_EXTENDED_ADAPTER_CAP_FORCE_LOAD_SENSE_YES                         (0x00000001) /* R-XUV */
#define NV_DPCD14_EXTENDED_ADAPTER_CAP_ALT_I2C_PATTERN                                       1:1 /* R-XUF */
#define NV_DPCD14_EXTENDED_ADAPTER_CAP_ALT_I2C_PATTERN_NO                           (0x00000000) /* R-XUV */
#define NV_DPCD14_EXTENDED_ADAPTER_CAP_ALT_I2C_PATTERN_YES                          (0x00000001) /* R-XUV */

#define NV_DPCD14_EXTENDED_DPRX_FEATURE_ENUM_LIST                                   (0x00002210) /* R-XUR */
#define NV_DPCD14_EXTENDED_DPRX_FEATURE_ENUM_LIST_GTC_CAP                                    0:0 /* R-XUF */
#define NV_DPCD14_EXTENDED_DPRX_FEATURE_ENUM_LIST_GTC_CAP_NO                        (0x00000000) /* R-XUV */
#define NV_DPCD14_EXTENDED_DPRX_FEATURE_ENUM_LIST_GTC_CAP_YES                       (0x00000001) /* R-XUV */
#define NV_DPCD14_EXTENDED_DPRX_FEATURE_ENUM_LIST_AV_SYNC_CAP                                2:2 /* R-XUF */
#define NV_DPCD14_EXTENDED_DPRX_FEATURE_ENUM_LIST_AV_SYNC_CAP_NO                    (0x00000000) /* R-XUV */
#define NV_DPCD14_EXTENDED_DPRX_FEATURE_ENUM_LIST_AV_SYNC_CAP_YES                   (0x00000001) /* R-XUV */
#define NV_DPCD14_EXTENDED_DPRX_FEATURE_ENUM_LIST_VSC_SDP_EXT_FOR_COLORIMETRY                3:3 /* R-XUF */
#define NV_DPCD14_EXTENDED_DPRX_FEATURE_ENUM_LIST_VSC_SDP_EXT_FOR_COLORIMETRY_NO    (0x00000000) /* R-XUV */
#define NV_DPCD14_EXTENDED_DPRX_FEATURE_ENUM_LIST_VSC_SDP_EXT_FOR_COLORIMETRY_YES   (0x00000001) /* R-XUV */
#define NV_DPCD14_EXTENDED_DPRX_FEATURE_ENUM_LIST_VSC_EXT_VESA_SDP                           4:4 /* R-XUF */
#define NV_DPCD14_EXTENDED_DPRX_FEATURE_ENUM_LIST_VSC_EXT_VESA_SDP_NO               (0x00000000) /* R-XUV */
#define NV_DPCD14_EXTENDED_DPRX_FEATURE_ENUM_LIST_VSC_EXT_VESA_SDP_YES              (0x00000001) /* R-XUV */
#define NV_DPCD14_EXTENDED_DPRX_FEATURE_ENUM_LIST_VSC_EXT_VESA_SDP_CHAINING                  5:5 /* R-XUF */
#define NV_DPCD14_EXTENDED_DPRX_FEATURE_ENUM_LIST_VSC_EXT_VESA_SDP_CHAINING_NO      (0x00000000) /* R-XUV */
#define NV_DPCD14_EXTENDED_DPRX_FEATURE_ENUM_LIST_VSC_EXT_VESA_SDP_CHAINING_YES     (0x00000001) /* R-XUV */
#define NV_DPCD14_EXTENDED_DPRX_FEATURE_ENUM_LIST_VSC_EXT_CTA_SDP                            6:6 /* R-XUF */
#define NV_DPCD14_EXTENDED_DPRX_FEATURE_ENUM_LIST_VSC_EXT_CTA_SDP_NO                (0x00000000) /* R-XUV */
#define NV_DPCD14_EXTENDED_DPRX_FEATURE_ENUM_LIST_VSC_EXT_CTA_SDP_YES               (0x00000001) /* R-XUV */
#define NV_DPCD14_EXTENDED_DPRX_FEATURE_ENUM_LIST_VSC_EXT_CTA_SDP_CHAINING                   7:7 /* R-XUF */
#define NV_DPCD14_EXTENDED_DPRX_FEATURE_ENUM_LIST_VSC_EXT_CTA_SDP_CHAINING_NO       (0x00000000) /* R-XUV */
#define NV_DPCD14_EXTENDED_DPRX_FEATURE_ENUM_LIST_VSC_EXT_CTA_SDP_CHAINING_YES      (0x00000001) /* R-XUV */

#define NV_DPCD14_EXTENDED_DPRX_SLEEP_WAKE_TIMEOUT_REQUEST                          (0x00002211) /* R-XUR */
#define NV_DPCD14_EXTENDED_DPRX_SLEEP_WAKE_TIMEOUT_REQUEST_PERIOD                            7:0 /* R-XUF */
#define NV_DPCD14_EXTENDED_DPRX_SLEEP_WAKE_TIMEOUT_REQUEST_PERIOD_1MS               (0x00000000) /* R-XUV */
#define NV_DPCD14_EXTENDED_DPRX_SLEEP_WAKE_TIMEOUT_REQUEST_PERIOD_20MS              (0x00000001) /* R-XUV */
#define NV_DPCD14_EXTENDED_DPRX_SLEEP_WAKE_TIMEOUT_REQUEST_PERIOD_40MS              (0x00000002) /* R-XUV */
#define NV_DPCD14_EXTENDED_DPRX_SLEEP_WAKE_TIMEOUT_REQUEST_PERIOD_60MS              (0x00000003) /* R-XUV */
#define NV_DPCD14_EXTENDED_DPRX_SLEEP_WAKE_TIMEOUT_REQUEST_PERIOD_80MS              (0x00000004) /* R-XUV */
#define NV_DPCD14_EXTENDED_DPRX_SLEEP_WAKE_TIMEOUT_REQUEST_PERIOD_100MS             (0x00000005) /* R-XUV */

#define NV_DPCD14_EXTENDED_VSC_EXT_VESA_SDP_MAX_CHAINING                            (0x00002212) /* R-XUR */
#define NV_DPCD14_EXTENDED_VSC_EXT_VESA_SDP_MAX_CHAINING_VAL                                7:0  /* R-XUF */

#define NV_DPCD14_EXTENDED_VSC_EXT_CTA_SDP_MAX_CHAINING                             (0x00002213) /* R-XUR */
#define NV_DPCD14_EXTENDED_VSC_EXT_CTA_SDP_MAX_CHAINING_VAL                                 7:0  /* R-XUF */

#define NV_DPCD14_DPRX_FEATURE_ENUM_LIST                                            (0x00002214) /* R-XUR */
#define NV_DPCD14_DPRX_FEATURE_ENUM_LIST_ADAPTIVE_SYNC_SDP_SUPPORTED                        0:0  /* R-XUF */
#define NV_DPCD14_DPRX_FEATURE_ENUM_LIST_ADAPTIVE_SYNC_SDP_SUPPORTED_NO             (0x00000000) /* R-XUV */
#define NV_DPCD14_DPRX_FEATURE_ENUM_LIST_ADAPTIVE_SYNC_SDP_SUPPORTED_YES            (0x00000001) /* R-XUV */
#define NV_DPCD14_DPRX_FEATURE_ENUM_LIST_VSC_EXT_FRAMEWORK_V1_SUPPORTED                     4:4  /* R-XUF */
#define NV_DPCD14_DPRX_FEATURE_ENUM_LIST_VSC_EXT_FRAMEWORK_V1_SUPPORTED_NO          (0x00000000) /* R-XUV */
#define NV_DPCD14_DPRX_FEATURE_ENUM_LIST_VSC_EXT_FRAMEWORK_V1_SUPPORTED_YES         (0x00000001) /* R-XUV */

#define NV_DPCD20_128B_132B_SUPPORTED_LINK_RATES                                    (0x00002215) /* R-XUR */
#define NV_DPCD20_128B_132B_SUPPORTED_LINK_RATES_UHBR10                                     0:0  /* R-XUF */
#define NV_DPCD20_128B_132B_SUPPORTED_LINK_RATES_UHBR10_NO                          (0x00000000) /* R-XUV */
#define NV_DPCD20_128B_132B_SUPPORTED_LINK_RATES_UHBR10_YES                         (0x00000001) /* R-XUV */
#define NV_DPCD20_128B_132B_SUPPORTED_LINK_RATES_UHBR20                                     1:1  /* R-XUF */
#define NV_DPCD20_128B_132B_SUPPORTED_LINK_RATES_UHBR20_NO                          (0x00000000) /* R-XUV */
#define NV_DPCD20_128B_132B_SUPPORTED_LINK_RATES_UHBR20_YES                         (0x00000001) /* R-XUV */
#define NV_DPCD20_128B_132B_SUPPORTED_LINK_RATES_UHBR13_5                                   2:2  /* R-XUF */
#define NV_DPCD20_128B_132B_SUPPORTED_LINK_RATES_UHBR13_5_NO                        (0x00000000) /* R-XUV */
#define NV_DPCD20_128B_132B_SUPPORTED_LINK_RATES_UHBR13_5_YES                       (0x00000001) /* R-XUV */

//
// The interval is (128b/132b_TRAINING_AUX_RD_INTERVAL value + 1) * INTERVAL_UNIT.
// The maximum is 256 ms.
//
#define NV_DPCD20_128B_132B_TRAINING_AUX_RD_INTERVAL                                (0x00002216) /* R-XUR */
#define NV_DPCD20_128B_132B_TRAINING_AUX_RD_INTERVAL_VAL                                    6:0  /* R-XUF */
#define NV_DPCD20_128B_132B_TRAINING_AUX_RD_INTERVAL_UNIT                                   7:7  /* R-XUF */
#define NV_DPCD20_128B_132B_TRAINING_AUX_RD_INTERVAL_UNIT_2MS                       (0x00000000) /* R-XUV */
#define NV_DPCD20_128B_132B_TRAINING_AUX_RD_INTERVAL_UNIT_1MS                       (0x00000001) /* R-XUV */
#define NV_DPCD20_128B_132B_TRAINING_AUX_RD_INTERVAL_MAX_MS                                 256

#define NV_DPCD20_PCON_HDMI_LINK_CONFIG_STATUS                                      (0x00003036) /* R-XUR */
#define NV_DPCD20_PCON_HDMI_LINK_CONFIG_STATUS_MODE                                         0:0  /* R-XUF */
#define NV_DPCD20_PCON_HDMI_LINK_CONFIG_STATUS_MODE_TMDS                            (0x00000000) /* R-XUV */
#define NV_DPCD20_PCON_HDMI_LINK_CONFIG_STATUS_MODE_FRL                             (0x00000001) /* R-XUV */
#define NV_DPCD20_PCON_HDMI_LINK_CONFIG_STATUS_LT_RESULT                                    6:1  /* R-XUF */
#define NV_DPCD20_PCON_HDMI_LINK_CONFIG_STATUS_LT_RES_9G                                    1:1  /* R-XUF */
#define NV_DPCD20_PCON_HDMI_LINK_CONFIG_STATUS_LT_RES_9G_NO                         (0x00000000) /* R-XUV */
#define NV_DPCD20_PCON_HDMI_LINK_CONFIG_STATUS_LT_RES_9G_YES                        (0x00000001) /* R-XUV */
#define NV_DPCD20_PCON_HDMI_LINK_CONFIG_STATUS_LT_RES_18G                                   2:2  /* R-XUF */
#define NV_DPCD20_PCON_HDMI_LINK_CONFIG_STATUS_LT_RES_18G_NO                        (0x00000000) /* R-XUV */
#define NV_DPCD20_PCON_HDMI_LINK_CONFIG_STATUS_LT_RES_18G_YES                       (0x00000001) /* R-XUV */
#define NV_DPCD20_PCON_HDMI_LINK_CONFIG_STATUS_LT_RES_24G                                   3:3  /* R-XUF */
#define NV_DPCD20_PCON_HDMI_LINK_CONFIG_STATUS_LT_RES_24G_NO                        (0x00000000) /* R-XUV */
#define NV_DPCD20_PCON_HDMI_LINK_CONFIG_STATUS_LT_RES_24G_YES                       (0x00000001) /* R-XUV */
#define NV_DPCD20_PCON_HDMI_LINK_CONFIG_STATUS_LT_RES_32G                                   4:4  /* R-XUF */
#define NV_DPCD20_PCON_HDMI_LINK_CONFIG_STATUS_LT_RES_32G_NO                        (0x00000000) /* R-XUV */
#define NV_DPCD20_PCON_HDMI_LINK_CONFIG_STATUS_LT_RES_32G_YES                       (0x00000001) /* R-XUV */
#define NV_DPCD20_PCON_HDMI_LINK_CONFIG_STATUS_LT_RES_40G                                   5:5  /* R-XUF */
#define NV_DPCD20_PCON_HDMI_LINK_CONFIG_STATUS_LT_RES_40G_NO                        (0x00000000) /* R-XUV */
#define NV_DPCD20_PCON_HDMI_LINK_CONFIG_STATUS_LT_RES_40G_YES                       (0x00000001) /* R-XUV */
#define NV_DPCD20_PCON_HDMI_LINK_CONFIG_STATUS_LT_RES_48G                                   6:6  /* R-XUF */
#define NV_DPCD20_PCON_HDMI_LINK_CONFIG_STATUS_LT_RES_48G_NO                        (0x00000000) /* R-XUV */
#define NV_DPCD20_PCON_HDMI_LINK_CONFIG_STATUS_LT_RES_48G_YES                       (0x00000001) /* R-XUV */

#define NV_DPCD20_PCON_DOWNSTREAM_LINK_ERROR_LANE(i)                            (0x00003037+(i)) /* RW-1A */
#define NV_DPCD20_PCON_DOWNSTREAM_LINK_ERROR_LANE__SIZE                                       4  /* R---S */
#define NV_DPCD20_PCON_DOWNSTREAM_LINK_ERROR_LANE_COUNT                                     3:0  /* R-XUF */
#define NV_DPCD20_PCON_DOWNSTREAM_LINK_ERROR_LANE_COUNT_ZERO                        (0x00000000) /* R-XUV */
#define NV_DPCD20_PCON_DOWNSTREAM_LINK_ERROR_LANE_COUNT_THREE                       (0x00000001) /* R-XUV */
#define NV_DPCD20_PCON_DOWNSTREAM_LINK_ERROR_LANE_COUNT_TEN                         (0x00000002) /* R-XUV */
#define NV_DPCD20_PCON_DOWNSTREAM_LINK_ERROR_LANE_COUNT_HUNDRED                     (0x00000004) /* R-XUV */

#define NV_DPCD20_PCON_HDMI_TX_LINK_STATUS                                          (0x0000303B) /* R-XUR */
#define NV_DPCD20_PCON_HDMI_TX_LINK_STATUS_LINK_ACTIVE                                      0:0  /* R-XUF */
#define NV_DPCD20_PCON_HDMI_TX_LINK_STATUS_LINK_ACTIVE_NO                           (0x00000000) /* R-XUV */
#define NV_DPCD20_PCON_HDMI_TX_LINK_STATUS_LINK_ACTIVE_YES                          (0x00000001) /* R-XUV */
#define NV_DPCD20_PCON_HDMI_TX_LINK_STATUS_LINK_READY                                       1:1  /* R-XUF */
#define NV_DPCD20_PCON_HDMI_TX_LINK_STATUS_LINK_READY_NO                            (0x00000000) /* R-XUV */
#define NV_DPCD20_PCON_HDMI_TX_LINK_STATUS_LINK_READY_YES                           (0x00000001) /* R-XUV */

#define NV_DPCD20_PCON_CONTROL_0                                                    (0x00003050) /* RWXUR */
#define NV_DPCD20_PCON_CONTROL_0_OUTPUT_CONFIG                                              0:0  /* RWXUF */
#define NV_DPCD20_PCON_CONTROL_0_OUTPUT_CONFIG_DVI                                  (0x00000000) /* RWXUV */
#define NV_DPCD20_PCON_CONTROL_0_OUTPUT_CONFIG_HDMI                                 (0x00000001) /* RWXUV */

#define NV_DPCD20_PCON_CONTROL_1                                                    (0x00003051) /* RWXUR */
#define NV_DPCD20_PCON_CONTROL_1_CONVERT_YCBCR420                                           0:0  /* RWXUF */
#define NV_DPCD20_PCON_CONTROL_1_CONVERT_YCBCR420_DISABLE                           (0x00000000) /* RWXUV */
#define NV_DPCD20_PCON_CONTROL_1_CONVERT_YCBCR420_ENABLE                            (0x00000001) /* RWXUV */
#define NV_DPCD20_PCON_CONTROL_1_DISABLE_HDMI_EDID_PROCESS                                  1:1  /* RWXUF */
#define NV_DPCD20_PCON_CONTROL_1_DISABLE_HDMI_EDID_PROCESS_NO                       (0x00000000) /* RWXUV */
#define NV_DPCD20_PCON_CONTROL_1_DISABLE_HDMI_EDID_PROCESS_YES                      (0x00000001) /* RWXUV */
#define NV_DPCD20_PCON_CONTROL_1_DISABLE_HDMI_AUTO_SCRAMBLING                               2:2  /* RWXUF */
#define NV_DPCD20_PCON_CONTROL_1_DISABLE_HDMI_AUTO_SCRAMBLING_NO                    (0x00000000) /* RWXUV */
#define NV_DPCD20_PCON_CONTROL_1_DISABLE_HDMI_AUTO_SCRAMBLING_YES                   (0x00000001) /* RWXUV */
#define NV_DPCD20_PCON_CONTROL_1_DISABLE_HDMI_FORCE_SCRAMBLING                              3:3  /* RWXUF */
#define NV_DPCD20_PCON_CONTROL_1_DISABLE_HDMI_FORCE_SCRAMBLING_NO                   (0x00000000) /* RWXUV */
#define NV_DPCD20_PCON_CONTROL_1_DISABLE_HDMI_FORCE_SCRAMBLING_YES                  (0x00000001) /* RWXUV */

#define NV_DPCD20_PCON_CONTROL_2                                                    (0x00003052) /* RWXUR */
#define NV_DPCD20_PCON_CONTROL_2_CONVERT_YCBCR422                                           0:0  /* RWXUF */
#define NV_DPCD20_PCON_CONTROL_2_CONVERT_YCBCR422_DISABLE                           (0x00000000) /* RWXUV */
#define NV_DPCD20_PCON_CONTROL_2_CONVERT_YCBCR422_ENABLE                            (0x00000001) /* RWXUV */

#define NV_DPCD20_PCON_CONTROL_3                                                    (0x00003053) /* RWXUR */
#define NV_DPCD20_PCON_CONTROL_3_COMPONENT_BIT_DEPTH                                        1:0  /* RWXUF */
#define NV_DPCD20_PCON_CONTROL_3_COMPONENT_BIT_DEPTH_SAME_AS_INC                    (0x00000000) /* RWXUV */
#define NV_DPCD20_PCON_CONTROL_3_COMPONENT_BIT_DEPTH_8BPC                           (0x00000001) /* RWXUV */
#define NV_DPCD20_PCON_CONTROL_3_COMPONENT_BIT_DEPTH_10BPC                          (0x00000002) /* RWXUV */
#define NV_DPCD20_PCON_CONTROL_3_COMPONENT_BIT_DEPTH_12BPC                          (0x00000003) /* RWXUV */

#define NV_DPCD14_OUTPUT_HTOTAL_LOW                                                 (0x00003054) /* RWXUR */
#define NV_DPCD14_OUTPUT_HTOTAL_HIGH                                                (0x00003055) /* RWXUR */

#define NV_DPCD14_OUTPUT_HSTART_LOW                                                 (0x00003056) /* RWXUR */
#define NV_DPCD14_OUTPUT_HSTART_HIGH                                                (0x00003057) /* RWXUR */

#define NV_DPCD14_OUTPUT_HSP_HSW_LOW                                                (0x00003056) /* RWXUR */
#define NV_DPCD14_OUTPUT_HSP_HSW_HIGH                                               (0x00003057) /* RWXUR */
#define NV_DPCD14_OUTPUT_HSP_HSW_HIGH_VAL                                                   6:0  /* RWXUF */
#define NV_DPCD14_OUTPUT_HSP_HSW_HIGH_OUTPUT_HSP                                            7:7  /* RWXUF */
#define NV_DPCD14_OUTPUT_HSP_HSW_HIGH_OUTPUT_HSP_POSITIVE                           (0x00000000) /* RWXUV */
#define NV_DPCD14_OUTPUT_HSP_HSW_HIGH_OUTPUT_HSP_NEGATIVE                           (0x00000001) /* RWXUV */

#define NV_DPCD20_PCON_FRL_LINK_CONFIG_1                                            (0x0000305A) /* RWXUR */
#define NV_DPCD20_PCON_FRL_LINK_CONFIG_1_MAX_LINK_BW                                        2:0  /* RWXUF */
#define NV_DPCD20_PCON_FRL_LINK_CONFIG_1_MAX_LINK_BW_ZERO                           (0x00000000) /* RWXUV */
#define NV_DPCD20_PCON_FRL_LINK_CONFIG_1_MAX_LINK_BW_9G                             (0x00000001) /* RWXUV */
#define NV_DPCD20_PCON_FRL_LINK_CONFIG_1_MAX_LINK_BW_18G                            (0x00000002) /* RWXUV */
#define NV_DPCD20_PCON_FRL_LINK_CONFIG_1_MAX_LINK_BW_24G                            (0x00000003) /* RWXUV */
#define NV_DPCD20_PCON_FRL_LINK_CONFIG_1_MAX_LINK_BW_32G                            (0x00000004) /* RWXUV */
#define NV_DPCD20_PCON_FRL_LINK_CONFIG_1_MAX_LINK_BW_40G                            (0x00000005) /* RWXUV */
#define NV_DPCD20_PCON_FRL_LINK_CONFIG_1_MAX_LINK_BW_48G                            (0x00000006) /* RWXUV */
#define NV_DPCD20_PCON_FRL_LINK_CONFIG_1_SRC_CONTROL_MODE                                   3:3  /* RWXUF */
#define NV_DPCD20_PCON_FRL_LINK_CONFIG_1_SRC_CONTROL_MODE_DISABLE                   (0x00000000) /* RWXUV */
#define NV_DPCD20_PCON_FRL_LINK_CONFIG_1_SRC_CONTROL_MODE_ENABLE                    (0x00000001) /* RWXUV */
#define NV_DPCD20_PCON_FRL_LINK_CONFIG_1_CONCURRENT_LT_MODE                                 4:4  /* RWXUF */
#define NV_DPCD20_PCON_FRL_LINK_CONFIG_1_CONCURRENT_LT_MODE_DISABLE                 (0x00000000) /* RWXUV */
#define NV_DPCD20_PCON_FRL_LINK_CONFIG_1_CONCURRENT_LT_MODE_ENABLE                  (0x00000001) /* RWXUV */
#define NV_DPCD20_PCON_FRL_LINK_CONFIG_1_LINK_FRL_MODE                                      5:5  /* RWXUF */
#define NV_DPCD20_PCON_FRL_LINK_CONFIG_1_LINK_FRL_MODE_DISABLE                      (0x00000000) /* RWXUV */
#define NV_DPCD20_PCON_FRL_LINK_CONFIG_1_LINK_FRL_MODE_ENABLE                       (0x00000001) /* RWXUV */
#define NV_DPCD20_PCON_FRL_LINK_CONFIG_1_IRQ_LINK_FRL_MODE                                  6:6  /* RWXUF */
#define NV_DPCD20_PCON_FRL_LINK_CONFIG_1_IRQ_LINK_FRL_MODE_DISABLE                  (0x00000000) /* RWXUV */
#define NV_DPCD20_PCON_FRL_LINK_CONFIG_1_IRQ_LINK_FRL_MODE_ENABLE                   (0x00000001) /* RWXUV */
#define NV_DPCD20_PCON_FRL_LINK_CONFIG_1_HDMI_LINK                                          7:7  /* RWXUF */
#define NV_DPCD20_PCON_FRL_LINK_CONFIG_1_HDMI_LINK_DISABLE                          (0x00000000) /* RWXUV */
#define NV_DPCD20_PCON_FRL_LINK_CONFIG_1_HDMI_LINK_ENABLE                           (0x00000001) /* RWXUV */

#define NV_DPCD20_PCON_FRL_LINK_CONFIG_2                                            (0x0000305B) /* RWXUR */
#define NV_DPCD20_PCON_FRL_LINK_CONFIG_2_LINK_BW_MASK                                       5:0  /* RWXUF */
#define NV_DPCD20_PCON_FRL_LINK_CONFIG_2_LINK_BW_MASK_9G                            (0x00000001) /* RWXUV */
#define NV_DPCD20_PCON_FRL_LINK_CONFIG_2_LINK_BW_MASK_18G                           (0x00000002) /* RWXUV */
#define NV_DPCD20_PCON_FRL_LINK_CONFIG_2_LINK_BW_MASK_24G                           (0x00000004) /* RWXUV */
#define NV_DPCD20_PCON_FRL_LINK_CONFIG_2_LINK_BW_MASK_32G                           (0x00000008) /* RWXUV */
#define NV_DPCD20_PCON_FRL_LINK_CONFIG_2_LINK_BW_MASK_40G                           (0x00000010) /* RWXUV */
#define NV_DPCD20_PCON_FRL_LINK_CONFIG_2_LINK_BW_MASK_48G                           (0x00000020) /* RWXUV */
#define NV_DPCD20_PCON_FRL_LINK_CONFIG_2_FRL_LT_CONTROL                                     6:6  /* RWXUF */
#define NV_DPCD20_PCON_FRL_LINK_CONFIG_2_FRL_LT_CONTROL_NORMAL                      (0x00000000) /* RWXUV */
#define NV_DPCD20_PCON_FRL_LINK_CONFIG_2_FRL_LT_CONTROL_EXTENDED                    (0x00000001) /* RWXUV */

// LT Tunable Repeater Related offsets

#define NV_DPCD14_LT_TUNABLE_PHY_REPEATER_REV                      (0x000F0000) /* R-XUR */
#define NV_DPCD14_LT_TUNABLE_PHY_REPEATER_REV_MINOR                        3:0  /* R-XUF */
#define NV_DPCD14_LT_TUNABLE_PHY_REPEATER_REV_MINOR_0              (0x00000000) /* R-XUV */
#define NV_DPCD14_LT_TUNABLE_PHY_REPEATER_REV_MAJOR                        7:4  /* R-XUF */
#define NV_DPCD14_LT_TUNABLE_PHY_REPEATER_REV_MAJOR_1              (0x00000001) /* R-XUV */

#define NV_DPCD14_MAX_LINK_RATE_PHY_REPEATER                       (0x000F0001) /* R-XUR */
#define NV_DPCD14_MAX_LINK_RATE_PHY_REPEATER_VAL                           7:0  /* R-XUF */
#define NV_DPCD14_MAX_LINK_RATE_PHY_REPEATER_VAL_1_62_GBPS         (0x00000006) /* R-XUV */
#define NV_DPCD14_MAX_LINK_RATE_PHY_REPEATER_VAL_2_70_GBPS         (0x0000000A) /* R-XUV */
#define NV_DPCD14_MAX_LINK_RATE_PHY_REPEATER_VAL_5_40_GBPS         (0x00000014) /* R-XUV */
#define NV_DPCD14_MAX_LINK_RATE_PHY_REPEATER_VAL_8_10_GBPS         (0x0000001E) /* R-XUV */

#define NV_DPCD14_PHY_REPEATER_CNT                                 (0x000F0002) /* R-XUR */
#define NV_DPCD14_PHY_REPEATER_CNT_VAL                                     7:0  /* R-XUF */
#define NV_DPCD14_PHY_REPEATER_CNT_VAL_0                           (0x00000000) /* R-XUV */
#define NV_DPCD14_PHY_REPEATER_CNT_VAL_1                           (0x00000080) /* R-XUV */
#define NV_DPCD14_PHY_REPEATER_CNT_VAL_2                           (0x00000040) /* R-XUV */
#define NV_DPCD14_PHY_REPEATER_CNT_VAL_3                           (0x00000020) /* R-XUV */
#define NV_DPCD14_PHY_REPEATER_CNT_VAL_4                           (0x00000010) /* R-XUV */
#define NV_DPCD14_PHY_REPEATER_CNT_VAL_5                           (0x00000008) /* R-XUV */
#define NV_DPCD14_PHY_REPEATER_CNT_VAL_6                           (0x00000004) /* R-XUV */
#define NV_DPCD14_PHY_REPEATER_CNT_VAL_7                           (0x00000002) /* R-XUV */
#define NV_DPCD14_PHY_REPEATER_CNT_VAL_8                           (0x00000001) /* R-XUV */
#define NV_DPCD14_PHY_REPEATER_CNT_MAX                                       8

#define NV_DPCD14_PHY_REPEATER_MODE                                (0x000F0003) /* R-XUR */
#define NV_DPCD14_PHY_REPEATER_MODE_VAL_TRANSPARENT                (0x00000055) /* R-XUV */
#define NV_DPCD14_PHY_REPEATER_MODE_VAL_NON_TRANSPARENT            (0x000000AA) /* R-XUV */

#define NV_DPCD14_MAX_LANE_COUNT_PHY_REPEATER                      (0x000F0004) /* R-XUR */
#define NV_DPCD14_MAX_LANE_COUNT_PHY_REPEATER_VAL                          4:0  /* R-XUF */

#define NV_DPCD14_PHY_REPEATER_EXTENDED_WAKE_TIMEOUT               (0x000F0005) /* RWXUR */
#define NV_DPCD14_PHY_REPEATER_EXTENDED_WAKE_TIMEOUT_REQ                   6:0  /* R-XUF */
#define NV_DPCD14_PHY_REPEATER_EXTENDED_WAKE_TIMEOUT_GRANT                 7:7  /* RWXUF */

#define NV_DPCD14_PHY_REPEATER_MAIN_LINK_CHANNEL_CODING                         (0x000F0006) /* RWXUR */
#define NV_DPCD14_PHY_REPEATER_MAIN_LINK_CHANNEL_CODING_128B_132B_SUPPORTED             0:0  /* R-XUF */
#define NV_DPCD14_PHY_REPEATER_MAIN_LINK_CHANNEL_CODING_128B_132B_SUPPORTED_NO  (0x00000000) /* RWXUF */
#define NV_DPCD14_PHY_REPEATER_MAIN_LINK_CHANNEL_CODING_128B_132B_SUPPORTED_YES (0x00000001) /* RWXUF */

#define NV_DPCD14_PHY_REPEATER_128B_132B_RATES                     (0x000F0007) /* R-XUR */
#define NV_DPCD14_PHY_REPEATER_128B_132B_RATES_10G_SUPPORTED               0:0  /* R-XUF */
#define NV_DPCD14_PHY_REPEATER_128B_132B_RATES_10G_SUPPORTED_NO    (0x00000000) /* R-XUF */
#define NV_DPCD14_PHY_REPEATER_128B_132B_RATES_10G_SUPPORTED_YES   (0x00000001) /* R-XUF */
#define NV_DPCD14_PHY_REPEATER_128B_132B_RATES_20G_SUPPORTED               1:1  /* R-XUF */
#define NV_DPCD14_PHY_REPEATER_128B_132B_RATES_20G_SUPPORTED_NO    (0x00000000) /* R-XUF */
#define NV_DPCD14_PHY_REPEATER_128B_132B_RATES_20G_SUPPORTED_YES   (0x00000001) /* R-XUF */
#define NV_DPCD14_PHY_REPEATER_128B_132B_RATES_13_5G_SUPPORTED             2:2  /* R-XUF */
#define NV_DPCD14_PHY_REPEATER_128B_132B_RATES_13_5G_SUPPORTED_NO  (0x00000000) /* R-XUF */
#define NV_DPCD14_PHY_REPEATER_128B_132B_RATES_13_5G_SUPPORTED_YES (0x00000001) /* R-XUF */

#define NV_DPCD14_PHY_REPEATER_EQ_DONE                             (0x000F0008) /* R-XUR */
#define NV_DPCD14_PHY_REPEATER_EQ_DONE_LTTPR(i)                        (i):(i)  /* R-XUF */
#define NV_DPCD14_PHY_REPEATER_EQ_DONE_LTTPR_NO                    (0x00000000) /* R-XUF */
#define NV_DPCD14_PHY_REPEATER_EQ_DONE_LTTPR_YES                   (0x00000001) /* R-XUF */
#define NV_DPCD14_PHY_REPEATER_EQ_DONE_LTTPR_0                             0:0  /* R-XUF */
#define NV_DPCD14_PHY_REPEATER_EQ_DONE_LTTPR_0_NO                  (0x00000000) /* R-XUF */
#define NV_DPCD14_PHY_REPEATER_EQ_DONE_LTTPR_0_YES                 (0x00000001) /* R-XUF */
#define NV_DPCD14_PHY_REPEATER_EQ_DONE_LTTPR_1                             1:1  /* R-XUF */
#define NV_DPCD14_PHY_REPEATER_EQ_DONE_LTTPR_1_NO                  (0x00000000) /* R-XUF */
#define NV_DPCD14_PHY_REPEATER_EQ_DONE_LTTPR_1_YES                 (0x00000001) /* R-XUF */
#define NV_DPCD14_PHY_REPEATER_EQ_DONE_LTTPR_2                             2:2  /* R-XUF */
#define NV_DPCD14_PHY_REPEATER_EQ_DONE_LTTPR_2_NO                  (0x00000000) /* R-XUF */
#define NV_DPCD14_PHY_REPEATER_EQ_DONE_LTTPR_2_YES                 (0x00000001) /* R-XUF */
#define NV_DPCD14_PHY_REPEATER_EQ_DONE_LTTPR_3                             3:3  /* R-XUF */
#define NV_DPCD14_PHY_REPEATER_EQ_DONE_LTTPR_3_NO                  (0x00000000) /* R-XUF */
#define NV_DPCD14_PHY_REPEATER_EQ_DONE_LTTPR_3_YES                 (0x00000001) /* R-XUF */
#define NV_DPCD14_PHY_REPEATER_EQ_DONE_LTTPR_4                             4:4  /* R-XUF */
#define NV_DPCD14_PHY_REPEATER_EQ_DONE_LTTPR_4_NO                  (0x00000000) /* R-XUF */
#define NV_DPCD14_PHY_REPEATER_EQ_DONE_LTTPR_4_YES                 (0x00000001) /* R-XUF */
#define NV_DPCD14_PHY_REPEATER_EQ_DONE_LTTPR_5                             5:5  /* R-XUF */
#define NV_DPCD14_PHY_REPEATER_EQ_DONE_LTTPR_5_NO                  (0x00000000) /* R-XUF */
#define NV_DPCD14_PHY_REPEATER_EQ_DONE_LTTPR_5_YES                 (0x00000001) /* R-XUF */
#define NV_DPCD14_PHY_REPEATER_EQ_DONE_LTTPR_6                             6:6  /* R-XUF */
#define NV_DPCD14_PHY_REPEATER_EQ_DONE_LTTPR_6_NO                  (0x00000000) /* R-XUF */
#define NV_DPCD14_PHY_REPEATER_EQ_DONE_LTTPR_6_YES                 (0x00000001) /* R-XUF */
#define NV_DPCD14_PHY_REPEATER_EQ_DONE_LTTPR_7                             7:7  /* R-XUF */
#define NV_DPCD14_PHY_REPEATER_EQ_DONE_LTTPR_7_NO                  (0x00000000) /* R-XUF */
#define NV_DPCD14_PHY_REPEATER_EQ_DONE_LTTPR_7_YES                 (0x00000001) /* R-XUF */


#define NV_DPCD14_PHY_REPEATER_START(i)                            (0x000F0010+(i)*0x50)  /* RW-1A */
#define NV_DPCD14_PHY_REPEATER_START__SIZE                                            8   /* R---S */
// Following defines are offsets
#define NV_DPCD14_TRAINING_PATTERN_SET_PHY_REPEATER                (0x00000000) /* RWXUV */
#define NV_DPCD14_TRAINING_LANE0_SET_PHY_REPEATER                  (0x00000001) /* RWXUV */
#define NV_DPCD14_TRAINING_LANE1_SET_PHY_REPEATER                  (0x00000002) /* RWXUV */
#define NV_DPCD14_TRAINING_LANE2_SET_PHY_REPEATER                  (0x00000003) /* RWXUV */
#define NV_DPCD14_TRAINING_LANE3_SET_PHY_REPEATER                  (0x00000004) /* RWXUV */

#define NV_DPCD14_TRAINING_AUX_RD_INTERVAL_PHY_REPEATER            (0x00000010) /* R-XUR */
#define NV_DPCD14_TRAINING_AUX_RD_INTERVAL_PHY_REPEATER_VAL                6:0  /* R-XUF */
#define NV_DPCD14_TRAINING_AUX_RD_INTERVAL_PHY_REPEATER_VAL_4MS    (0x00000001) /* R-XUV */
#define NV_DPCD14_TRAINING_AUX_RD_INTERVAL_PHY_REPEATER_VAL_8MS    (0x00000002) /* R-XUV */
#define NV_DPCD14_TRAINING_AUX_RD_INTERVAL_PHY_REPEATER_VAL_12MS   (0x00000003) /* R-XUV */
#define NV_DPCD14_TRAINING_AUX_RD_INTERVAL_PHY_REPEATER_VAL_16MS   (0x00000004) /* R-XUV */

#define NV_DPCD14_TRANSMITTER_CAP_PHY_REPEATER                     (0x00000011) /* R-XUR */
#define NV_DPCD14_TRANSMITTER_CAP_PHY_REPEATER_VOLTAGE_SWING_3             0:0  /* R-XUF */
#define NV_DPCD14_TRANSMITTER_CAP_PHY_REPEATER_VOLTAGE_SWING_3_NO  (0x00000000) /* R-XUV */
#define NV_DPCD14_TRANSMITTER_CAP_PHY_REPEATER_VOLTAGE_SWING_3_YES (0x00000001) /* R-XUV */
#define NV_DPCD14_TRANSMITTER_CAP_PHY_REPEATER_PRE_EMPHASIS_3              1:1  /* R-XUF */
#define NV_DPCD14_TRANSMITTER_CAP_PHY_REPEATER_PRE_EMPHASIS_3_NO   (0x00000000) /* R-XUV */
#define NV_DPCD14_TRANSMITTER_CAP_PHY_REPEATER_PRE_EMPHASIS_3_YES  (0x00000001) /* R-XUV */

#define NV_DPCD14_LANE0_1_STATUS_PHY_REPEATER                      (0x00000020) /* R-XUR */
#define NV_DPCD14_LANE2_3_STATUS_PHY_REPEATER                      (0x00000021) /* R-XUR */
#define NV_DPCD14_LANE_ALIGN_STATUS_UPDATED_PHY_REPEATER           (0x00000022) /* R-XUR */
#define NV_DPCD14_ADJUST_REQUEST_LANE0_1_PHY_REPEATER              (0x00000023) /* R-XUR */
#define NV_DPCD14_ADJUST_REQUEST_LANE2_3_PHY_REPEATER              (0x00000024) /* R-XUR */

#endif // #ifndef _DISPLAYPORT14_H_

