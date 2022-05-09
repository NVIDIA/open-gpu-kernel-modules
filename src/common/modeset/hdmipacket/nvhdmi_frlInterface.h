/*
 * SPDX-FileCopyrightText: Copyright (c) 2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/** @file  nvhdmi_frlInterface.h 
 *  @brief This file provides FRL related interfaces between client and HDMI lib
 */ 


#ifndef _NVHDMI_FRLINTERFACE_H_
#define _NVHDMI_FRLINTERFACE_H_

#include "nvhdmipkt.h"
#include "nvHdmiFrlCommon.h"

#include "../timing/nvtiming.h"

#ifdef __cplusplus
extern "C" {
#endif


// DSC encoder color format bitmasks (these match DSC lib & RM ctrl 0073 fields)
typedef enum tagHDMI_DSC_ENCODER_COLOR_FORMAT
{
    HDMI_DSC_ENCODER_COLOR_FORMAT_RGB            = 1,
    HDMI_DSC_ENCODER_COLOR_FORMAT_YCBCR444       = 2,
    HDMI_DSC_ENCODER_COLOR_FORMAT_YCBCRNATIVE422 = 4,
    HDMI_DSC_ENCODER_COLOR_FORMAT_YCBCRNATIVE420 = 8
} HDMI_DSC_ENCODER_COLOR_FORMAT;

// Options for QueryFRLConfig interface
typedef enum tagHDMI_QUERY_FRL_OPTION
{
    HDMI_QUERY_FRL_ANY_CONFIG = 0,               // any FRL config that supports mode
    HDMI_QUERY_FRL_OPTIMUM_CONFIG,               // find best fit config for this mode
    HDMI_QUERY_FRL_LOWEST_BANDWIDTH,             // min bw
    HDMI_QUERY_FRL_HIGHEST_PIXEL_QUALITY,        // trade off bandwidth for pixel quality
    HDMI_QUERY_FRL_HIGHEST_BANDWIDTH
} HDMI_QUERY_FRL_OPTION;

/*************************************************************************************************
* HDMI_VIDEO_TRANSPORT_INFO:                                                                     *
* Video transport format - a combination of timing, bpc, packing represents what goes on the link*
* client passes this in, lib uses this for bandwidth calculations to decide required FRL rate    *
**************************************************************************************************/
typedef struct tagHDMI_VIDEO_TRANSPORT_INFO
{
    const NVT_TIMING  *pTiming;           // backend timing
    HDMI_BPC           bpc;
    HDMI_PIXEL_PACKING packing;
    NvBool             bDualHeadMode;     // 2H1OR
} HDMI_VIDEO_TRANSPORT_INFO;

/************************************************************************************************
* HDMI_QUERY_FRL_CLIENT_CONTROL:                                                                *
* Allow client to force request DSC/FRL configurations. For testing purpose or otherwise        *
* eg, client could query for any fitting FRL config instead of most optimum. It could trade off *
* bandwidth for pixel quality.                                                                  *
*************************************************************************************************/
typedef struct tagHDMI_QUERY_FRL_CLIENT_CONTROL
{
    HDMI_QUERY_FRL_OPTION   option;

    NvU32 forceFRLRate        :  1;
    NvU32 forceAudio2Ch48KHz  :  1;
    NvU32 enableDSC           :  1;
    NvU32 forceSliceCount     :  1;
    NvU32 forceSliceWidth     :  1;
    NvU32 forceBppx16         :  1;
    NvU32 skipGeneratePPS     :  1;
    NvU32 reserved            : 25;

    // client can set below params if respective force flag is set
    NvU32              sliceCount;
    NvU32              sliceWidth;
    NvU32              bitsPerPixelX16;
    HDMI_FRL_DATA_RATE frlRate;

} HDMI_QUERY_FRL_CLIENT_CONTROL;

/************************************************************************************************
* HDMI_SRC_CAPS:                                                                                *
* Input to HDMI lib.                                                                            *
*                                                                                               *
* Client gives info about GPU capabilities - DSC related caps                                   *
*************************************************************************************************/
typedef struct tagHDMI_SRC_CAPS
{
    struct
    {
        NvU32 dscCapable                 : 1;
        NvU32 bppPrecision               : 8;
        NvU32 encoderColorFormatMask     : 8;
        NvU32 lineBufferSizeKB           : 8;
        NvU32 rateBufferSizeKB           : 8;
        NvU32 maxNumHztSlices            : 8;
        NvU32 lineBufferBitDepth         : 8;
        NvU32 dualHeadBppTargetMaxX16    : 16;
        NvU32 maxWidthPerSlice;
    } dscCaps;

    HDMI_FRL_DATA_RATE linkMaxFRLRate;
} HDMI_SRC_CAPS;

/************************************************************************************************
* HDMI_SINK_CAPS:                                                                               *
* Input to HDMI lib.                                                                            *
*                                                                                               *
* Client gives info from EDID, HDMI lib uses DSC related info to call DSC lib to generate PPS   *
* Audio information from CEA861 block is used for bandwidth calculations                        *
* linkMaxFRLRate and linkMaxFRLRateDSC are max link rates determined from physical link         *
* training.                                                                                     *
*************************************************************************************************/
typedef struct tagHDMI_SINK_CAPS
{
    const NVT_HDMI_FORUM_INFO *pHdmiForumInfo;
    NvU32                      audioType;
    NvU32                      maxAudioChannels;
    NvU32                      maxAudioFreqKHz;
    NvBool                     bHBRAudio;
    HDMI_FRL_DATA_RATE         linkMaxFRLRate;
    HDMI_FRL_DATA_RATE         linkMaxFRLRateDSC;
} HDMI_SINK_CAPS;

/************************************************************************************************
* HDMI_FRL_CONFIG:                                                                              *
* Output from HDMI lib. Client uses this info for modeset                                       *
*                                                                                               *
* maxSupportedAudioCh, maxSupportedAudioFreqKHz - max possible audio settings at the chosen     *
*                      FRL rate, though the sink caps may have reported higher caps             *
*                                                                                               *
* dscInfo - if current timing requires DSC, lib returns PPS information here                    *
*                                                                                               *
* bitsPerPixelx16    - optimum bpp value calculated per spec                                    *
* dscHActiveBytes    - in compressed video transport mode, number of bytes in 1 line            *
* dscHActiveTriBytes - in compressed video transport mode, number of tri-bytes in 1 line        *
* dscHBlankTriBytes  - in compressed video transport mode, number of tri-bytes to be sent       *
*                                 to represent horizontal blanking                              *
*                                                                                               *
* pps[32]            - PPS data. HDMI lib calls DSC lib to fill it in                           *
*************************************************************************************************/
#define HDMI_DSC_MAX_PPS_SIZE_DWORD 32
typedef struct tagHDMI_FRL_CONFIG
{
    HDMI_FRL_DATA_RATE   frlRate;
    NvU32                maxSupportedAudioCh;
    NvU32                maxSupportedAudioFreqKHz;

    // DSC info client will use for core channel modeset
    struct
    {
        NvU32            bEnableDSC      :  1;
        NvU32            reserved        : 31;

        NvU32            bitsPerPixelX16;
        NvU32            sliceCount;
        NvU32            sliceWidth;
        NvU32            pps[HDMI_DSC_MAX_PPS_SIZE_DWORD];
        NvU32            dscHActiveBytes;
        NvU32            dscHActiveTriBytes;
        NvU32            dscHBlankTriBytes;
		NvU32            dscTBlankToTTotalRatioX1k;
    } dscInfo;

} HDMI_FRL_CONFIG;

/************************************************************************************************
* NvHdmi_AssessLinkCapabilities:                                                                *
*                                                                                               *
* Input parameters:                                                                             *
*    subDevice        - Sub Device ID.                                                          *
*    displayId        - Display ID.                                                             *
*    pSinkEdid        - EDID of sink                                                            *
*                                                                                               *
* Output parameters:                                                                            *
*    pSrcCaps         - src capabilities - DSC caps                                             *
*    pSinkCaps        - sink capabilities - actual caps calculated from link training           *
*                                                                                               *
* Calls RM to get DSC related src side caps. Performs physical link training to determine if    *
* sink reported max FRL rate can actually be supported on the physical link                     *
*************************************************************************************************/
NVHDMIPKT_RESULT
NvHdmi_AssessLinkCapabilities(NvHdmiPkt_Handle             libHandle,
                              NvU32                        subDevice,
                              NvU32                        displayId,
                              NVT_EDID_INFO         const * const pSinkEdid,
                              HDMI_SRC_CAPS               *pSrcCaps,
                              HDMI_SINK_CAPS              *pSinkCaps);

/************************************************************************************************
* NvHdmi_QueryFRLConfig:                                                                        *
*                                                                                               *
* Input parameters:                                                                             *
*    libHandle        - Hdmi library handle, provided on initializing the library.              *
*    pVidTransInfo    - information about timing, bpc and packing                               *
*    pClientCtrl      - settings client wants to see set. HDMI lib tries to honor these         *
*    pSinkCaps        - sink capabilities                                                       *
*                                                                                               *
* Output parameters:                                                                            *
*    pFRLConfig       - chosen FRL rate and DSC configuration                                   *
*                                                                                               *
*************************************************************************************************/
NVHDMIPKT_RESULT
NvHdmi_QueryFRLConfig(NvHdmiPkt_Handle                            libHandle,
                      HDMI_VIDEO_TRANSPORT_INFO     const * const pVidTransInfo,
                      HDMI_QUERY_FRL_CLIENT_CONTROL const * const pClientCtrl,
                      HDMI_SRC_CAPS                 const * const pSrcCaps,
                      HDMI_SINK_CAPS                const * const pSinkCaps,
                      HDMI_FRL_CONFIG                     *pFRLConfig);

/************************************************************************************************
* NvHdmi_SetFRLConfig:                                                                          *
*                                                                                               *
* Input parameters:                                                                             *
*    libHandle        - Hdmi library handle, provided on initializing the library.              *
*    subDevice        - Sub Device ID.                                                          *
*    displayId        - Display ID.                                                             *
*    bFakeLt          - Indicates that the GPU's link configuration should be forced and that   *
*                       configuration of the sink device should be skipped.                     *
*    pFRLConfig       - Link configuration to set.                                              *
*                                                                                               *
************************************************************************************************/
NVHDMIPKT_RESULT
NvHdmi_SetFRLConfig(NvHdmiPkt_Handle                   libHandle,
                    NvU32                              subDevice,
                    NvU32                              displayId,
                    NvBool                             bFakeLt,
                    HDMI_FRL_CONFIG                   *pFRLConfig);

/************************************************************************************************
* NvHdmi_ClearFRLConfig:                                                                        *
*                                                                                               *
* Input parameters:                                                                             *
*   libHandle - Hdmi library handle, provided on initializing the library.                      *
*   subDevice - Sub Device ID.                                                                  *
*   displayId - Display ID to change the settings on.                                           *
*                                                                                               *
************************************************************************************************/
NVHDMIPKT_RESULT
NvHdmi_ClearFRLConfig(NvHdmiPkt_Handle    libHandle,
                      NvU32               subDevice,
                      NvU32               displayId);

#ifdef __cplusplus
}
#endif

#endif // _NVHDMI_FRLINTERFACE_H_
