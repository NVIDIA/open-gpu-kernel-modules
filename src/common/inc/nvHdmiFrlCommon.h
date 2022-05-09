/*
 * SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/** @file  nvHdmiFrlCommon.h 
 *  @brief This file defines data needed for and returned by HDMI 2.1 spec FRL calculations
 *         It meant to be a spec layer within HDMI lib, without carrying any
 *         driver/hw related information
 */ 

#ifndef _NVHDMIFRLCOMMON_H_
#define _NVHDMIFRLCOMMON_H_

#include "nvmisc.h"

//******************************************************************************
// Constants/Structures
//******************************************************************************
#define MAX_RECONSTRUCTED_HACTIVE_PIXELS        2720

// HDMI_BPC: Bits per component enums.
typedef enum tagHDMI_BPC
{
    HDMI_BPC8  = 8,
    HDMI_BPC10 = 10,
    HDMI_BPC12 = 12,
    HDMI_BPC16 = 16
} HDMI_BPC;

// HDMI_PIXEL_PACKING: Pixel packing type enums
typedef enum tagHDMI_PIXEL_PACKING
{
    HDMI_PIXEL_PACKING_RGB = 0,
    HDMI_PIXEL_PACKING_YCbCr444,
    HDMI_PIXEL_PACKING_YCbCr422,
    HDMI_PIXEL_PACKING_YCbCr420
} HDMI_PIXEL_PACKING;

// HDMI_FRL_DATA_RATE: FRL mode enums
typedef enum tagHDMI_FRL_DATA_RATE
{
    HDMI_FRL_DATA_RATE_NONE,
    HDMI_FRL_DATA_RATE_3LANES_3GBPS,
    HDMI_FRL_DATA_RATE_3LANES_6GBPS,
    HDMI_FRL_DATA_RATE_4LANES_6GBPS,
    HDMI_FRL_DATA_RATE_4LANES_8GBPS,
    HDMI_FRL_DATA_RATE_4LANES_10GBPS,
    HDMI_FRL_DATA_RATE_4LANES_12GBPS,
    HDMI_FRL_DATA_RATE_UNSPECIFIED
} HDMI_FRL_DATA_RATE;

typedef enum tagAUDIO_PKTTYPE
{
    AUDIO_PKTTYPE_LPCM_SAMPLE = 0,
    AUDIO_PKTTYPE_ONE_BIT_LPCM_SAMPLE,
    AUDIO_PKTTYPE_DST_AUDIO,
    AUDIO_PKTTYPE_HBR_AUDIO,
    AUDIO_PKTTYPE_MULTI_STREAM_AUDIO,
    AUDIO_PKTTYPE_ONE_BIT_MULTI_STREAM_AUDIO,
    AUDIO_PKTTYPE_3D_AUDIO,
    AUDIO_PKTTYPE_ONE_BIT_3D_AUDIO,
    NO_AUDIO
} AUDIO_PKTTYPE;

typedef struct tagFRL_CAPACITY_COMPUTATION_PARAMS
{
    NvU32         numLanes;
    NvU32         frlBitRateGbps;
    NvU32         pclk10KHz;
    NvU32         hTotal;
    NvU32         hActive;
    NvU32         bpc;
    HDMI_PIXEL_PACKING pixelPacking;
    AUDIO_PKTTYPE audioType;
    NvU32         numAudioChannels;
    NvU32         audioFreqKHz;

    struct
    {
        NvU32 bppTargetx16;
        NvU32 hSlices;
        NvU32 sliceWidth;
        NvU32 dscTotalChunkKBytes;
    } compressionInfo;

} FRL_CAPACITY_COMPUTATION_PARAMS;

typedef struct tagFRL_COMPUTATION_RESULT
{
    HDMI_FRL_DATA_RATE frlRate;
    NvU32 bppTargetx16;

    NvBool engageCompression;
    NvBool isAudioSupported;
    NvBool dataFlowDisparityReqMet;
    NvBool dataFlowMeteringReqMet;
    NvBool isVideoTransportSupported;
    NvU32 triBytesBorrowed;      // uncompressed mode: num of active Tri-bytes to be transmitted at HBlank
    NvU32 hcActiveBytes;         // compressed mode:   num of FRL character bytes in active region
    NvU32 hcActiveTriBytes;      // compressed mode:   num of FRL tri-bytes in active region
    NvU32 hcBlankTriBytes;       // compressed mode:   num of FRL tri-bytes in blanking region
    NvU32 tBlankToTTotalX1k;     // compressed mode:   ratio of time spent on blanking to the total line time
} FRL_COMPUTATION_RESULT;

typedef struct tagFRL_PRE_CALC_CONFIG
{
    NvU32 vic;
    HDMI_PIXEL_PACKING packing;
    HDMI_BPC bpc;
    HDMI_FRL_DATA_RATE frlRate;
    NvU32 bppX16;
    NvBool bCompressedMode;
} FRL_PRE_CALC_CONFIG;

#endif // _NVHDMIFRLCOMMON_H_
