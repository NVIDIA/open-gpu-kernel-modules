/*
 * SPDX-FileCopyrightText: Copyright (c) 2023-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _DISPLAYPORT2X_H_
#define _DISPLAYPORT2X_H_

#include "nvmisc.h"
#include "dpcd.h"
#include "dpcd14.h"
#include "dpcd20.h"
#include "displayport.h"

/**************** Resource Manager Defines and Structures ******************\
*                                                                           *
* Module: DISPLAYPORT2x.H                                                   *
*       Defines DISPLAYPORT V2.x                                            *
*                                                                           *
\***************************************************************************/

//
// 4 Legacy Link Rates: RBR, HBR, HBR2, HBR3
// 5 ILRs: 2.16G, 2.43G, 3.24G, 4.32G, 6.75G
// 3 UHBRs: 10G, 13.5G, 20G
// 2 Internal Test: 2.5G, 5G Do NOT use in any other use cases.
//
#define NV_SUPPORTED_DP2X_LINK_RATES__SIZE        14
//
// For 128b/132b link rate to data rate, linkRate * 128/132 * 1/8 * 10M -> ((linkRate * 4 * 1000000) / 33)
// For 128b/132b data rate to link rate, dataRate * 132/128 * 8 * 1/10M -> ((dataRate * 33) / (4 * 10000000))
// Data rates used here are in Bytes per second.
//
#define LINK_RATE_TO_DATA_RATE_128B_132B(linkRate)      ((linkRate * 4 * 10000000UL) / 33)
#define DATA_RATE_128B_132B_TO_LINK_RATE(dataRate)      (NV_UNSIGNED_DIV_CEIL((dataRate * 33ULL), (4 * 10000000ULL)))

// To calculate the effective link rate with channel encoding accounted
#define OVERHEAD_128B_132B(linkRate)                    ((linkRate * 128) / 132)

//
// 128b/132b precise Data Bandwidth Efficiency.
// Per Spec 3.5.2.18, effective BW with 128b/132b channel coding is linkRate * 0.9671.
// This covers Phy logial layer efficiency 52/1584 and link layer efficiency of 4/65540 as well. 
// Also add SSC margin of 0.5%.
// Additionally add another 0.1% for source to be slightly more conservative for DSC environments
// and provide maximum compatibility for LTTPR CDS LT sequence.
// 
// (1 - 52/1584) * (1 - 4/65540) * 0.994 = 0.9612
//
#define DATA_BW_EFF_128B_132B(linkRate)                 ((linkRate * 9612) / 10000)

// For channel equalization, max loop count is 20 when waiting CHANNEL_EQ_DONE set.
#define NV_DP2X_MAX_LOOP_COUNT_POLL_CHNL_EQ_DONE        (20U)

typedef enum
{
    linkBW_6_75Gbps                = 0x19
} DP2X_LINK_BANDWIDTH_270M;

// The definition here is to match HW register defines for link speed.
typedef enum
{
    dp2LinkSpeedId_1_62Gbps                = 0x00,
    dp2LinkSpeedId_2_70Gbps                = 0x01,
    dp2LinkSpeedId_5_40Gbps                = 0x02,
    dp2LinkSpeedId_8_10Gbps                = 0x03,
    dp2LinkSpeedId_2_16Gbps                = 0x04,
    dp2LinkSpeedId_2_43Gbps                = 0x05,
    dp2LinkSpeedId_3_24Gbps                = 0x06,
    dp2LinkSpeedId_4_32Gbps                = 0x07,
    dp2LinkSpeedId_6_75Gbps                = 0x08,
    dp2LinkSpeedId_10_0Gbps                = 0x12,
    dp2LinkSpeedId_13_5Gbps                = 0x13,
    dp2LinkSpeedId_20_0Gbps                = 0x14,
    dp2LinkSpeedId_UHBR_1_62Gbps           = 0x1C,
    dp2LinkSpeedId_UHBR_5_00Gbps           = 0x1D,
    dp2LinkSpeedId_UHBR_2_70Gbps           = 0x1E,
    dp2LinkSpeedId_UHBR_2_50Gbps           = 0x1F,
    dp2LinkSpeedId_Supported
} DP2X_LINK_SPEED_INDEX;

typedef enum
{
    dp2xTxFFEPresetId_0             = 0,
    dp2xTxFFEPresetId_1             = 1,
    dp2xTxFFEPresetId_2             = 2,
    dp2xTxFFEPresetId_3             = 3,
    dp2xTxFFEPresetId_4             = 4,
    dp2xTxFFEPresetId_5             = 5,
    dp2xTxFFEPresetId_6             = 6,
    dp2xTxFFEPresetId_7             = 7,
    dp2xTxFFEPresetId_8             = 8,
    dp2xTxFFEPresetId_9             = 9,
    dp2xTxFFEPresetId_10            = 10,
    dp2xTxFFEPresetId_11            = 11,
    dp2xTxFFEPresetId_12            = 12,
    dp2xTxFFEPresetId_13            = 13,
    dp2xTxFFEPresetId_14            = 14,
    dp2xTxFFEPresetId_15            = 15,
    dp2xTxFFEPresetId_Supported
} DP2X_TXFFE_PRESET_INDEX;

// Link Training stages for 128b/132b channel coding.
typedef enum
{
    DP2X_LT_Set_ResetLink                = 0,
    DP2X_LT_Poll_ResetLink               = 1,
    DP2X_LT_Set_PreLT                    = 2,
    DP2X_LT_Set_ChnlEq                   = 3,
    DP2X_LT_Poll_ChnlEq_Done             = 4,
    DP2X_LT_Poll_ChnlEq_InterlaneAlign   = 5,
    DP2X_LT_Set_CDS                      = 6,
    DP2X_LT_Poll_CDS                     = 7,
    DP2X_LT_Set_PostLT                   = 8,
    DP2X_LT_StageSupported
} DP2X_LT_STAGES;

typedef enum
{
    DP2X_ResetLinkForPreLT,
    DP2X_ResetLinkForFallback,
    DP2X_ResetLinkForChannelCoding
} DP2X_RESET_LINK_REASON;

//
// Multiplier constant to get link frequency (multiplier of 10MHz) in MBps with 128b/132b channel coding.
//   a * 10 * 1000 * 1000(10Mhz) * (128 / 132)(128b/132b) / 8(Byte)
//
#define DP_LINK_BW_FREQ_MULTI_10M_TO_MBPS               (10 * 1000 * 1000 * 128 / (132 * 8))

//
// Multiplier constant to get DP2X link frequency in KHZ
// Maximum link rate of Main Link lanes = Value x 10M.
// To get it to KHz unit, we need to multiply 10K.
//
#define DP_LINK_BW_FREQUENCY_MULTIPLIER_10MHZ_TO_KHZ    (10*1000)

//
// Multiplier constant to get link frequency (multiplier of 270MHz) in MBps
// a * 10 * 1000 * 1000(10Mhz) * (8 / 10)(8b/10b) / 8(Byte)
// = a * 1000000
//
#define DP_LINK_BW_FREQUENCY_MULTIPLIER_10MHZ_TO_10HZ   (1000*1000)

#define IS_STANDARD_DP2_X_LINKBW(val) (((NvU32)(val)==dp2LinkRate_1_62Gbps) || \
                                       ((NvU32)(val)==dp2LinkRate_2_70Gbps) || \
                                       ((NvU32)(val)==dp2LinkRate_5_40Gbps) || \
                                       ((NvU32)(val)==dp2LinkRate_8_10Gbps))

#define IS_INTERMEDIATE_DP2_X_LINKBW(val) (((NvU32)(val)==dp2LinkRate_2_16Gbps) || \
                                           ((NvU32)(val)==dp2LinkRate_2_43Gbps) || \
                                           ((NvU32)(val)==dp2LinkRate_3_24Gbps) || \
                                           ((NvU32)(val)==dp2LinkRate_4_32Gbps) || \
                                           ((NvU32)(val)==dp2LinkRate_6_75Gbps))

#define IS_DP2_X_UHBR_LINKBW(val)     (((NvU32)(val)==dp2LinkRate_2_50Gbps) || \
                                       ((NvU32)(val)==dp2LinkRate_5_00Gbps) || \
                                       ((NvU32)(val)==dp2LinkRate_10_0Gbps) || \
                                       ((NvU32)(val)==dp2LinkRate_13_5Gbps) || \
                                       ((NvU32)(val)==dp2LinkRate_20_0Gbps))

#define IS_VALID_DP2_X_LINKBW(val) (IS_STANDARD_DP2_X_LINKBW(val)     || \
                                    IS_INTERMEDIATE_DP2_X_LINKBW(val) || \
                                    IS_DP2_X_UHBR_LINKBW(val))

#define IS_LEGACY_INTERMEDIATE_LINKBW(val) (((NvU32)(val)==linkBW_2_16Gbps) || \
                                            ((NvU32)(val)==linkBW_2_43Gbps) || \
                                            ((NvU32)(val)==linkBW_3_24Gbps) || \
                                            ((NvU32)(val)==linkBW_4_32Gbps) || \
                                            ((NvU32)(val)==linkBW_6_75Gbps))

#endif  // #ifndef _DISPLAYPORT2X_H_
