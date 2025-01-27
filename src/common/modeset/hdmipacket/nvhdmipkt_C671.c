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
 *
 * File:      nvhdmipkt_C671.c
 * 
 * Purpose:   Provides packet write functions for HDMI library  for Ampere+ chips
 */

#include <stddef.h>
#include "nvhdmipkt_common.h"
#include "nvhdmipkt_class.h"

#include "nvhdmipkt_internal.h"
#include "nvHdmiFrlCommon.h"

#include "../timing/nvt_dsc_pps.h"
#include "ctrl/ctrl0073/ctrl0073system.h"

#include "class/clc671.h"
#include "ctrl/ctrl0073/ctrl0073dp.h"
#include "ctrl/ctrl0073/ctrl0073specific.h"

#define MULTIPLIER_1G                           1000000000
#define PCLK_VARIANCE_10MHZ                     1000

// In HDMI case, for PPS set, HDMI2.1 spec expects source to set this field to 13, decoder capability is assumed
// Note, in DP case, DSC decoder is allowed to report line buffer depth capability through DPCD registers
#define HDMI_DSC_DECODER_LINE_BUFFER_BIT_DEPTH_CAP      13
#define NVHDMIPKT_C671_INVALID_PKT_TYPE         ((NVC671_SF_HDMI_INFO_IDX_VSI) + 1)

extern NVHDMIPKT_RESULT hdmiPacketWrite0073(NVHDMIPKT_CLASS*   pThis,
                                            NvU32              subDevice,
                                            NvU32              displayId,
                                            NvU32              head,
                                            NVHDMIPKT_TYPE     packetType,
                                            NVHDMIPKT_TC       transmitControl, 
                                            NvU32              packetLen, 
                                            NvU8 const *const  pPacket);

extern NVHDMIPKT_RESULT hdmiPacketCtrl0073(NVHDMIPKT_CLASS*  pThis,
                                           NvU32             subDevice,
                                           NvU32             displayId,
                                           NvU32             head,
                                           NVHDMIPKT_TYPE    packetType,
                                           NVHDMIPKT_TC      transmitControl);

extern NVHDMIPKT_RESULT hdmiPacketWrite9171(NVHDMIPKT_CLASS*   pThis,
                                            NvU32              subDevice,
                                            NvU32              displayId,
                                            NvU32              head,
                                            NVHDMIPKT_TYPE     packetType,
                                            NVHDMIPKT_TC       transmitControl, 
                                            NvU32              packetLen, 
                                            NvU8 const *const  pPacket);

static NVHDMIPKT_RESULT hdmiClearFRLConfigC671(NVHDMIPKT_CLASS    *pThis,
                                               NvU32               subDevice,
                                               NvU32               displayId);


// translate FRL rate to RM control param
static NvU32 translateFRLRateToNv0073SetHdmiFrlConfig(HDMI_FRL_DATA_RATE frlRate)
{
    switch(frlRate)
    {
    case HDMI_FRL_DATA_RATE_NONE          : return NV0073_CTRL_HDMI_FRL_DATA_SET_FRL_RATE_NONE; 
    case HDMI_FRL_DATA_RATE_3LANES_3GBPS  : return NV0073_CTRL_HDMI_FRL_DATA_SET_FRL_RATE_3LANES_3G;
    case HDMI_FRL_DATA_RATE_3LANES_6GBPS  : return NV0073_CTRL_HDMI_FRL_DATA_SET_FRL_RATE_3LANES_6G;
    case HDMI_FRL_DATA_RATE_4LANES_6GBPS  : return NV0073_CTRL_HDMI_FRL_DATA_SET_FRL_RATE_4LANES_6G;
    case HDMI_FRL_DATA_RATE_4LANES_8GBPS  : return NV0073_CTRL_HDMI_FRL_DATA_SET_FRL_RATE_4LANES_8G;
    case HDMI_FRL_DATA_RATE_4LANES_10GBPS : return NV0073_CTRL_HDMI_FRL_DATA_SET_FRL_RATE_4LANES_10G;
    case HDMI_FRL_DATA_RATE_4LANES_12GBPS : return NV0073_CTRL_HDMI_FRL_DATA_SET_FRL_RATE_4LANES_12G;
    default:
        break;
    }
    return NV0073_CTRL_HDMI_FRL_DATA_SET_FRL_RATE_NONE;
}

/*
 * Both DSC_Max_FRL_Rate and Max_FRL_Rate have same translation
 * 
 */
static HDMI_FRL_DATA_RATE translateFRLCapToFRLDataRate(NvU32 sinkFRLcap)
{
    switch(sinkFRLcap)
    {
    case 6: return HDMI_FRL_DATA_RATE_4LANES_12GBPS;
    case 5: return HDMI_FRL_DATA_RATE_4LANES_10GBPS;
    case 4: return HDMI_FRL_DATA_RATE_4LANES_8GBPS;
    case 3: return HDMI_FRL_DATA_RATE_4LANES_6GBPS;
    case 2: return HDMI_FRL_DATA_RATE_3LANES_6GBPS;
    case 1: return HDMI_FRL_DATA_RATE_3LANES_3GBPS;
    case 0: // fall through
    default: break;
    }

    if (sinkFRLcap > 6 && sinkFRLcap <= 15)
    {
        return HDMI_FRL_DATA_RATE_4LANES_12GBPS;
    }

    return HDMI_FRL_DATA_RATE_NONE;
}

// If we want to force 2ch48KHz fill it in as default, if not, 
// Lookup sink short audio descriptor blocks to see max supported audio
static void populateAudioCaps(NVT_EDID_CEA861_INFO const * const p861ExtBlock,
                              HDMI_SINK_CAPS             * pSinkCaps)
{
    NvU32 i;

    for (i = 0; i < p861ExtBlock->total_sad; i++)
    {
        NvU32 data = p861ExtBlock->audio[i].byte1;
        data = (data & NVT_CEA861_AUDIO_FORMAT_MASK) >> NVT_CEA861_AUDIO_FORMAT_SHIFT;

        // unsupported
        if ((data == NVT_CEA861_AUDIO_FORMAT_RSVD) ||
            (data == NVT_CEA861_AUDIO_FORMAT_RSVD15))
        {
            continue;
        }

        // check for HBR audio support. We don't support any other packet types
        if ((data == NVT_CEA861_AUDIO_FORMAT_DTS_HD) ||
            (data == NVT_CEA861_AUDIO_FORMAT_MAT))
        {
            pSinkCaps->bHBRAudio = NV_TRUE;
        }

        // num of channels for this audio format
        data = p861ExtBlock->audio[i].byte1;
        NvU32 numChannels = ((data & NVT_CEA861_AUDIO_MAX_CHANNEL_MASK) >> NVT_CEA861_AUDIO_MAX_CHANNEL_SHIFT) + 1;
        if (pSinkCaps->maxAudioChannels < numChannels)
        {
            pSinkCaps->maxAudioChannels = numChannels;
        }

        // get max sampling frequency
        data = p861ExtBlock->audio[i].byte2;
        NvU32 sampleFreq = (data & NVT_CEA861_AUDIO_SAMPLE_RATE_192KHZ) ? 192 :
                           (data & NVT_CEA861_AUDIO_SAMPLE_RATE_176KHZ) ? 176 :
                           (data & NVT_CEA861_AUDIO_SAMPLE_RATE_96KHZ)  ?  96 :
                           (data & NVT_CEA861_AUDIO_SAMPLE_RATE_88KHZ)  ?  88 :
                           (data & NVT_CEA861_AUDIO_SAMPLE_RATE_48KHZ)  ?  48 :
                           (data & NVT_CEA861_AUDIO_SAMPLE_RATE_44KHZ)  ?  44 :
                           (data & NVT_CEA861_AUDIO_SAMPLE_RATE_32KHZ)  ?  32 : 0;
        if (pSinkCaps->maxAudioFreqKHz < sampleFreq)
        {
            pSinkCaps->maxAudioFreqKHz = sampleFreq;
        }
    }
}

/*
 * hdmiAssessLinkCapabilities
 *
 * 1. Try physical link training to determine max link capacity
 * 2. Calculate max audio capabilities
 * 3. Limit connector max to what the source can support
 * AssesssLinkCapabilities is expected to be called at hotplug time. Ideally, srcCaps need to be calculated one time,
 * but for now, no incentive to do so. In future move it out to better place as need arises
 */
static NVHDMIPKT_RESULT
hdmiAssessLinkCapabilitiesC671(NVHDMIPKT_CLASS       *pThis,
                               NvU32                  subDevice,
                               NvU32                  displayId,
                               NVT_EDID_INFO   const * const pSinkEdid,
                               HDMI_SRC_CAPS         *pSrcCaps,
                               HDMI_SINK_CAPS        *pSinkCaps)
{

    // Read DSC caps from RM - gpu caps for DSC are same across DP and HDMI FRL (HDMI 2.1+)
    // Hence use same RM control as DP case for reading this cap
    NV0073_CTRL_CMD_DP_GET_CAPS_PARAMS params;
    params.subDeviceInstance = 0;
    params.sorIndex = 0; // Passing SOR index as 0 since all SORs have the same capability.

#if NVHDMIPKT_RM_CALLS_INTERNAL
    if (CALL_DISP_RM(NvRmControl)(pThis->clientHandles.hClient,
                    pThis->clientHandles.hDisplay,
                    NV0073_CTRL_CMD_DP_GET_CAPS,
                    &params,
                    sizeof(params)) != NVOS_STATUS_SUCCESS)
#else // !NVHDMIPKT_RM_CALLS_INTERNAL
    NvBool bSuccess = pThis->callback.rmDispControl2(pThis->cbHandle,
                                                     params.subDeviceInstance,
                                                     NV0073_CTRL_CMD_DP_GET_CAPS, 
                                                     &params, 
                                                     sizeof(params));
    if (bSuccess == NV_FALSE)
#endif // NVHDMIPKT_RM_CALLS_INTERNAL
    {
        return NVHDMIPKT_FAIL;
    }

    pSrcCaps->dscCaps.dscCapable              = params.DSC.bDscSupported;
    pSrcCaps->dscCaps.encoderColorFormatMask  = params.DSC.encoderColorFormatMask;
    pSrcCaps->dscCaps.dualHeadBppTargetMaxX16 = 256; // Tu10x/GA10x HW DSC module allow max 16bpp in 2H1OR mode. 

    NV0073_CTRL_SPECIFIC_GET_HDMI_GPU_CAPS_PARAMS hdmiGpuCapsParams;
    NVMISC_MEMSET(&hdmiGpuCapsParams, 0, sizeof(NV0073_CTRL_SPECIFIC_GET_HDMI_GPU_CAPS_PARAMS));
#if NVHDMIPKT_RM_CALLS_INTERNAL
    if (CALL_DISP_RM(NvRmControl)(pThis->clientHandles.hClient,
                    pThis->clientHandles.hDisplay,
                    NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_GPU_CAPS,
                    &hdmiGpuCapsParams,
                    sizeof(hdmiGpuCapsParams)) != NVOS_STATUS_SUCCESS)
#else // !NVHDMIPKT_RM_CALLS_INTERNAL
    bSuccess = pThis->callback.rmDispControl2(pThis->cbHandle,
                                                     hdmiGpuCapsParams.subDeviceInstance,
                                                     NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_GPU_CAPS, 
                                                     &hdmiGpuCapsParams, 
                                                     sizeof(hdmiGpuCapsParams));
    if (bSuccess == NV_FALSE)
#endif // NVHDMIPKT_RM_CALLS_INTERNAL
    {
        NVMISC_MEMSET(&hdmiGpuCapsParams, 0, sizeof(NV0073_CTRL_SPECIFIC_GET_HDMI_GPU_CAPS_PARAMS));
    }

    pSrcCaps->linkMaxFRLRate                  = translateFRLCapToFRLDataRate(hdmiGpuCapsParams.caps);

    switch(params.DSC.bitsPerPixelPrecision)
    {
    case NV0073_CTRL_CMD_DP_GET_CAPS_DSC_BITS_PER_PIXEL_PRECISION_1_16:
        pSrcCaps->dscCaps.bppPrecision = DSC_BITS_PER_PIXEL_PRECISION_1_16; break;
    case NV0073_CTRL_CMD_DP_GET_CAPS_DSC_BITS_PER_PIXEL_PRECISION_1_8:
        pSrcCaps->dscCaps.bppPrecision = DSC_BITS_PER_PIXEL_PRECISION_1_8;  break;
    case NV0073_CTRL_CMD_DP_GET_CAPS_DSC_BITS_PER_PIXEL_PRECISION_1_4:
        pSrcCaps->dscCaps.bppPrecision = DSC_BITS_PER_PIXEL_PRECISION_1_4;  break;
    case NV0073_CTRL_CMD_DP_GET_CAPS_DSC_BITS_PER_PIXEL_PRECISION_1_2:
        pSrcCaps->dscCaps.bppPrecision = DSC_BITS_PER_PIXEL_PRECISION_1_2;  break;
    case NV0073_CTRL_CMD_DP_GET_CAPS_DSC_BITS_PER_PIXEL_PRECISION_1:
        pSrcCaps->dscCaps.bppPrecision = DSC_BITS_PER_PIXEL_PRECISION_1;    break;
    default: break;
    }

    pSrcCaps->dscCaps.lineBufferSizeKB    = params.DSC.lineBufferSizeKB;
    pSrcCaps->dscCaps.rateBufferSizeKB    = params.DSC.rateBufferSizeKB;
    pSrcCaps->dscCaps.maxNumHztSlices     = params.DSC.maxNumHztSlices;
    pSrcCaps->dscCaps.lineBufferBitDepth  = params.DSC.lineBufferBitDepth;
    pSrcCaps->dscCaps.maxWidthPerSlice    = 5120; // Max DSC buffer width per head is 5120, this can be chunks of 1/2/4 slices, so keep 5120 as the very max.

    pSinkCaps->pHdmiForumInfo = &pSinkEdid->hdmiForumInfo;
    populateAudioCaps(&pSinkEdid->ext861,   pSinkCaps);
    populateAudioCaps(&pSinkEdid->ext861_2, pSinkCaps);

    NvU32 setFRLRate = pSinkEdid->hdmiForumInfo.max_FRL_Rate;

    pSinkCaps->linkMaxFRLRate    = translateFRLCapToFRLDataRate(setFRLRate);
    pSinkCaps->linkMaxFRLRateDSC = (pSrcCaps->dscCaps.dscCapable &&
                                   (pSinkEdid->hdmiForumInfo.dsc_Max_FRL_Rate > setFRLRate)) ?
                                        pSinkCaps->linkMaxFRLRate :
                                        translateFRLCapToFRLDataRate(pSinkEdid->hdmiForumInfo.dsc_Max_FRL_Rate);

    return NVHDMIPKT_SUCCESS;
}

// Fill in basic params from Timing info etc
static void populateBaseFRLParams(HDMI_VIDEO_TRANSPORT_INFO                   const *pVidTransInfo,
                                  HDMI_SINK_CAPS                              const *pSinkCaps,
                                  NvBool                                             bForce2Ch48KHz,
                                  NV0073_CTRL_FRL_CAPACITY_COMPUTATION_PARAMS       *pFRLParams)
{
    pFRLParams->pclk10KHz    = pVidTransInfo->pTiming->pclk;
    pFRLParams->hTotal       = pVidTransInfo->pTiming->HTotal;
    pFRLParams->hActive      = pVidTransInfo->pTiming->HVisible;
    pFRLParams->bpc          = pVidTransInfo->bpc;
    pFRLParams->pixelPacking = pVidTransInfo->packing;

    pFRLParams->numAudioChannels = bForce2Ch48KHz ?  2 : pSinkCaps->maxAudioChannels;
    pFRLParams->audioFreqKHz     = bForce2Ch48KHz ? 48 : pSinkCaps->maxAudioFreqKHz;
    pFRLParams->audioType        = pSinkCaps->bHBRAudio ?  AUDIO_PKTTYPE_HBR_AUDIO :
                                                           AUDIO_PKTTYPE_LPCM_SAMPLE;

    pFRLParams->compressionInfo.dscTotalChunkKBytes = 1024 * (pSinkCaps->pHdmiForumInfo->dsc_totalChunkKBytes);
}


// Get next higher link rate
static HDMI_FRL_DATA_RATE getNextHigherLinkRate(HDMI_FRL_DATA_RATE frlRate)
{
    return (frlRate == HDMI_FRL_DATA_RATE_4LANES_12GBPS) ? HDMI_FRL_DATA_RATE_NONE : (frlRate + 1);
}

// Fill in GPU and Monitor caps for DSC PPS calculations
static void populateDscCaps(HDMI_SRC_CAPS         const * const pSrcCaps, 
                            HDMI_SINK_CAPS        const * const pSinkCaps,
                            DSC_INFO                    * pDscInfo)
{
    // populate src caps
    pDscInfo->gpuCaps.encoderColorFormatMask = pSrcCaps->dscCaps.encoderColorFormatMask;
    pDscInfo->gpuCaps.lineBufferSize         = pSrcCaps->dscCaps.lineBufferSizeKB;
    pDscInfo->gpuCaps.bitsPerPixelPrecision  = pSrcCaps->dscCaps.bppPrecision;
    pDscInfo->gpuCaps.maxNumHztSlices        = pSrcCaps->dscCaps.maxNumHztSlices;
    pDscInfo->gpuCaps.lineBufferBitDepth     = pSrcCaps->dscCaps.lineBufferBitDepth;

    // populate sink caps
    pDscInfo->sinkCaps.decoderColorFormatMask |= DSC_DECODER_COLOR_FORMAT_RGB;
    pDscInfo->sinkCaps.decoderColorFormatMask |= DSC_DECODER_COLOR_FORMAT_Y_CB_CR_444;
    pDscInfo->sinkCaps.decoderColorFormatMask |= DSC_DECODER_COLOR_FORMAT_Y_CB_CR_SIMPLE_422;
    pDscInfo->sinkCaps.decoderColorFormatMask |= DSC_DECODER_COLOR_FORMAT_Y_CB_CR_NATIVE_422;

    pDscInfo->sinkCaps.bitsPerPixelPrecision = DSC_BITS_PER_PIXEL_PRECISION_1_16;
    if (pSinkCaps->pHdmiForumInfo->dsc_Native_420)
    {
        pDscInfo->sinkCaps.decoderColorFormatMask |= DSC_DECODER_COLOR_FORMAT_Y_CB_CR_NATIVE_420;
    }

    // limited by spec
    pDscInfo->sinkCaps.maxSliceWidth = 2720;

    NvU32 sliceCountMask = 0;
    NvU32 maxNumHztSlices = pSinkCaps->pHdmiForumInfo->dsc_MaxSlices;
    NvU32 peakThroughput = (pSinkCaps->pHdmiForumInfo->dsc_MaxPclkPerSliceMHz == 400) ?
                                DSC_DECODER_PEAK_THROUGHPUT_MODE0_400 :
                                DSC_DECODER_PEAK_THROUGHPUT_MODE0_340;

    switch(pSinkCaps->pHdmiForumInfo->dsc_MaxSlices)
    {
    case 16: sliceCountMask |= DSC_DECODER_SLICES_PER_SINK_16; // fall-through
    case 12: sliceCountMask |= DSC_DECODER_SLICES_PER_SINK_12; // fall-through
    case  8: sliceCountMask |= DSC_DECODER_SLICES_PER_SINK_8;  // fall-through
    case  4: sliceCountMask |= DSC_DECODER_SLICES_PER_SINK_4;  // fall-through
    case  2: sliceCountMask |= DSC_DECODER_SLICES_PER_SINK_2;  // fall-through
    case  1: sliceCountMask |= DSC_DECODER_SLICES_PER_SINK_1; break;
    default: break;
    }

    pDscInfo->sinkCaps.sliceCountSupportedMask = sliceCountMask;
    pDscInfo->sinkCaps.maxNumHztSlices    = maxNumHztSlices;
    pDscInfo->sinkCaps.lineBufferBitDepth = HDMI_DSC_DECODER_LINE_BUFFER_BIT_DEPTH_CAP;

    // Color depth supported by DSC decoder of panel
    pDscInfo->sinkCaps.decoderColorDepthMask |= pSinkCaps->pHdmiForumInfo->dsc_16bpc ? DSC_DECODER_COLOR_DEPTH_CAPS_16_BITS : 0;
    pDscInfo->sinkCaps.decoderColorDepthMask |= pSinkCaps->pHdmiForumInfo->dsc_12bpc ? DSC_DECODER_COLOR_DEPTH_CAPS_12_BITS : 0;
    pDscInfo->sinkCaps.decoderColorDepthMask |= pSinkCaps->pHdmiForumInfo->dsc_10bpc ? DSC_DECODER_COLOR_DEPTH_CAPS_10_BITS : 0;
    pDscInfo->sinkCaps.decoderColorDepthMask |= DSC_DECODER_COLOR_DEPTH_CAPS_8_BITS;

    pDscInfo->sinkCaps.bBlockPrediction = 1;
    pDscInfo->sinkCaps.algorithmRevision.versionMajor = 1;
    pDscInfo->sinkCaps.algorithmRevision.versionMinor = 2;
    pDscInfo->sinkCaps.peakThroughputMode0 = peakThroughput;

    // Per DSC v1.2 spec, native 422/420 per-slice peak throughput is approximately twice of RGB/444 peak throughput
    // HDMI has only one throughput cap reporting, no separate 422/420 throughput cap unlike for DP, so just double 444's value here.
    pDscInfo->sinkCaps.peakThroughputMode1 = (peakThroughput == DSC_DECODER_PEAK_THROUGHPUT_MODE0_340) ? 
                                                 DSC_DECODER_PEAK_THROUGHPUT_MODE1_650 : // closest approximation to 680Mhz
                                                 DSC_DECODER_PEAK_THROUGHPUT_MODE1_800;
}

// Fill in mode related info for DSC lib
static void populateDscModesetInfo(HDMI_VIDEO_TRANSPORT_INFO const * const pVidTransInfo,
                                   MODESET_INFO                    * pDscModesetInfo)
{
    pDscModesetInfo->pixelClockHz = pVidTransInfo->pTiming->pclk * 10000;      // Requested pixel clock for the mode
    pDscModesetInfo->activeWidth  = pVidTransInfo->pTiming->HVisible;          // Active Width
    pDscModesetInfo->activeHeight = pVidTransInfo->pTiming->VVisible;          // Active Height
    pDscModesetInfo->bitsPerComponent = (NvU32)pVidTransInfo->bpc;             // BPC value to be used
    pDscModesetInfo->colorFormat = (pVidTransInfo->packing == HDMI_PIXEL_PACKING_RGB)      ? NVT_COLOR_FORMAT_RGB :
                                   (pVidTransInfo->packing == HDMI_PIXEL_PACKING_YCbCr444) ? NVT_COLOR_FORMAT_YCbCr444 :
                                   (pVidTransInfo->packing == HDMI_PIXEL_PACKING_YCbCr422) ? NVT_COLOR_FORMAT_YCbCr422 :
                                   (pVidTransInfo->packing == HDMI_PIXEL_PACKING_YCbCr420) ? NVT_COLOR_FORMAT_YCbCr420 : 0;
    pDscModesetInfo->bDualMode = pVidTransInfo->bDualHeadMode;
    pDscModesetInfo->bDropMode = NV_FALSE;
}

// Checks against source and sink caps whether DSC is possible
// Tries to determine slice width and slice count accounting for 2Head1Or, populates this info into FRL calculation structure
// if this calculation fails DSC cannot be enabled
static NvBool evaluateIsDSCPossible(NVHDMIPKT_CLASS             *pThis,
                                    HDMI_SRC_CAPS               const *pSrcCaps,
                                    HDMI_SINK_CAPS              const *pSinkCaps,
                                    HDMI_VIDEO_TRANSPORT_INFO   const * const pVidTransInfo,
                                    NV0073_CTRL_FRL_CAPACITY_COMPUTATION_PARAMS   *pFRLParams)
{
    const NvU32 numHeadsDrivingSink = pVidTransInfo->bDualHeadMode ? 2 : 1;

    if (!pSrcCaps->dscCaps.dscCapable)
    {
        return NV_FALSE;
    }

    if (!pSinkCaps->pHdmiForumInfo->dsc_1p2 ||
        !pSinkCaps->linkMaxFRLRateDSC ||
        (!pSinkCaps->pHdmiForumInfo->dsc_16bpc && (pFRLParams->bpc == HDMI_BPC16)) ||
        (!pSinkCaps->pHdmiForumInfo->dsc_12bpc && (pFRLParams->bpc == HDMI_BPC12)) ||
        (!pSinkCaps->pHdmiForumInfo->dsc_10bpc && (pFRLParams->bpc == HDMI_BPC10)))
    {
        return NV_FALSE;
    }

    // Disallow DSC if the source or sink don't support DSC with this mode's colorformat/packing.
    switch (pVidTransInfo->packing)
    {
    case HDMI_PIXEL_PACKING_RGB:
        if (!(pSrcCaps->dscCaps.encoderColorFormatMask & DSC_ENCODER_COLOR_FORMAT_RGB))
        {
            return NV_FALSE;
        }
        break;
    case HDMI_PIXEL_PACKING_YCbCr444:
        if (!(pSrcCaps->dscCaps.encoderColorFormatMask & DSC_ENCODER_COLOR_FORMAT_Y_CB_CR_444))
        {
            return NV_FALSE;
        }
        break;
    case HDMI_PIXEL_PACKING_YCbCr422:
        if (!(pSrcCaps->dscCaps.encoderColorFormatMask & DSC_ENCODER_COLOR_FORMAT_Y_CB_CR_NATIVE_422))
        {
            return NV_FALSE;
        }
        break;
    case HDMI_PIXEL_PACKING_YCbCr420:
        if (!(pSrcCaps->dscCaps.encoderColorFormatMask & DSC_ENCODER_COLOR_FORMAT_Y_CB_CR_NATIVE_420) ||
            !pSinkCaps->pHdmiForumInfo->dsc_Native_420)
        {
            return NV_FALSE;
        }
        break;
    }

    NV0073_CTRL_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION_PARAMS *pGetHdmiFrlCapacityComputationParams = NULL;
    pGetHdmiFrlCapacityComputationParams = pThis->callback.malloc(pThis->cbHandle, sizeof(NV0073_CTRL_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION_PARAMS));
    if (pGetHdmiFrlCapacityComputationParams)
    {
        NvBool bIsDSCPossible = NV_FALSE;
        NVMISC_MEMSET(pGetHdmiFrlCapacityComputationParams, 0, sizeof(*pGetHdmiFrlCapacityComputationParams));
        pGetHdmiFrlCapacityComputationParams->input = *pFRLParams;
        pGetHdmiFrlCapacityComputationParams->dsc.maxSliceCount = NV_MIN(pSrcCaps->dscCaps.maxNumHztSlices * numHeadsDrivingSink, pSinkCaps->pHdmiForumInfo->dsc_MaxSlices);
        pGetHdmiFrlCapacityComputationParams->dsc.maxSliceWidth = pSrcCaps->dscCaps.maxWidthPerSlice;
        pGetHdmiFrlCapacityComputationParams->cmd = NV0073_CTRL_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION_CMD_IS_FRL_DSC_POSSIBLE;
#if NVHDMIPKT_RM_CALLS_INTERNAL
        if (CALL_DISP_RM(NvRmControl)(pThis->clientHandles.hClient,
                        pThis->clientHandles.hDisplay,
                        NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION,
                        pGetHdmiFrlCapacityComputationParams,
                        sizeof(NV0073_CTRL_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION_PARAMS)) == NVOS_STATUS_SUCCESS)
#else // !NVHDMIPKT_RM_CALLS_INTERNAL
        NvBool bSuccess = pThis->callback.rmDispControl2(pThis->cbHandle,
                          0,
                          NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION, 
                          pGetHdmiFrlCapacityComputationParams, 
                          sizeof(NV0073_CTRL_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION_PARAMS));
        if (bSuccess == NV_TRUE)
#endif // NVHDMIPKT_RM_CALLS_INTERNAL
        {
            bIsDSCPossible = pGetHdmiFrlCapacityComputationParams->dsc.bIsDSCPossible;
            *pFRLParams = pGetHdmiFrlCapacityComputationParams->input;
        }

        pThis->callback.free(pThis->cbHandle, pGetHdmiFrlCapacityComputationParams);
        return bIsDSCPossible;
    }
    return NV_FALSE;

    return NV_TRUE;
}

static void translateBitRate(HDMI_FRL_DATA_RATE frlRate, NvU32 *pFrlBitRateGbps, NvU32 *pNumLanes)
{
    switch(frlRate)
    {
    case HDMI_FRL_DATA_RATE_4LANES_12GBPS : { *pFrlBitRateGbps = 12; *pNumLanes = 4; break; }
    case HDMI_FRL_DATA_RATE_4LANES_10GBPS : { *pFrlBitRateGbps = 10; *pNumLanes = 4; break; }
    case HDMI_FRL_DATA_RATE_4LANES_8GBPS  : { *pFrlBitRateGbps =  8; *pNumLanes = 4; break; }
    case HDMI_FRL_DATA_RATE_4LANES_6GBPS  : { *pFrlBitRateGbps =  6; *pNumLanes = 4; break; }
    case HDMI_FRL_DATA_RATE_3LANES_6GBPS  : { *pFrlBitRateGbps =  6; *pNumLanes = 3; break; }
    case HDMI_FRL_DATA_RATE_3LANES_3GBPS  : // fall through
    default                               : { *pFrlBitRateGbps =  3; *pNumLanes = 3; break; }
    }
}

// Determine if video transport is possible at any FRL rate in the specified range
// Iterate from min rate to max rate
static NVHDMIPKT_RESULT
determineUncompressedFRLConfig(NVHDMIPKT_CLASS                             *pThis,
                               NV0073_CTRL_FRL_CAPACITY_COMPUTATION_PARAMS *pFRLParams,
                               HDMI_FRL_DATA_RATE                           minFRLRate,
                               HDMI_FRL_DATA_RATE                           maxFRLRate,
                               NV0073_CTRL_FRL_CAPACITY_COMPUTATION_RESULT *pResults)
{
    NV0073_CTRL_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION_PARAMS *pGetHdmiFrlCapacityComputationParams = NULL;
    HDMI_FRL_DATA_RATE frlRate = minFRLRate;
    NVHDMIPKT_RESULT status = NVHDMIPKT_INSUFFICIENT_BANDWIDTH;

    pGetHdmiFrlCapacityComputationParams = pThis->callback.malloc(pThis->cbHandle, sizeof(NV0073_CTRL_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION_PARAMS));

    while (frlRate != HDMI_FRL_DATA_RATE_NONE)
    {
        translateBitRate(frlRate, &pFRLParams->frlBitRateGbps, &pFRLParams->numLanes);

        if (pGetHdmiFrlCapacityComputationParams)
        {
            NVMISC_MEMSET(pGetHdmiFrlCapacityComputationParams, 0, sizeof(*pGetHdmiFrlCapacityComputationParams));
            pGetHdmiFrlCapacityComputationParams->input = *pFRLParams;
            pGetHdmiFrlCapacityComputationParams->cmd = NV0073_CTRL_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION_CMD_UNCOMPRESSED_VIDEO;
#if NVHDMIPKT_RM_CALLS_INTERNAL
            if (CALL_DISP_RM(NvRmControl)(pThis->clientHandles.hClient,
                            pThis->clientHandles.hDisplay,
                            NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION,
                            pGetHdmiFrlCapacityComputationParams,
                            sizeof(NV0073_CTRL_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION_PARAMS)) == NVOS_STATUS_SUCCESS)
#else // !NVHDMIPKT_RM_CALLS_INTERNAL
            NvBool bSuccess = pThis->callback.rmDispControl2(pThis->cbHandle,
                              0,
                              NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION, 
                              pGetHdmiFrlCapacityComputationParams, 
                              sizeof(NV0073_CTRL_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION_PARAMS));
            if (bSuccess == NV_TRUE)
#endif // NVHDMIPKT_RM_CALLS_INTERNAL
            {
                *pResults = pGetHdmiFrlCapacityComputationParams->result;
            }
        }
        else
        {
            status = NVHDMIPKT_FAIL;
            goto uncompressedQuery_exit;
        }

        status = (pResults->isVideoTransportSupported && pResults->isAudioSupported) ? NVHDMIPKT_SUCCESS : status;

        if ((status == NVHDMIPKT_SUCCESS) ||
            (frlRate == maxFRLRate))
        {
            break;
        }

        // try again at next link rate
        frlRate = getNextHigherLinkRate(frlRate);
    }

    pResults->frlRate = frlRate;

uncompressedQuery_exit:
    if (pGetHdmiFrlCapacityComputationParams)
    {
        pThis->callback.free(pThis->cbHandle, pGetHdmiFrlCapacityComputationParams);
    }
    return status;
}

// Determines the absolute min n max Bpp settings we can use with DSC. This is irrespective of FRL rate
static void calcBppMinMax(HDMI_SRC_CAPS               const *pSrcCaps,
                          HDMI_SINK_CAPS              const *pSinkCaps,
                          HDMI_VIDEO_TRANSPORT_INFO   const *pVidTransInfo,
                          NvU32                             *pBppMinX16,
                          NvU32                             *pBppMaxX16)
{

    NvU32 bppMinX16 = 0;
    NvU32 bppMaxX16 = 0;

    switch(pVidTransInfo->packing)
    {
    case HDMI_PIXEL_PACKING_YCbCr420: { bppMinX16 = 6 * 16; bppMaxX16 = (3 * pVidTransInfo->bpc *  8 - 1); break; }
    case HDMI_PIXEL_PACKING_YCbCr422: { bppMinX16 = 7 * 16; bppMaxX16 = (2 * pVidTransInfo->bpc * 16 - 1); break; }
    case HDMI_PIXEL_PACKING_RGB:
    case HDMI_PIXEL_PACKING_YCbCr444: 
    default:                          { bppMinX16 = 8 * 16; bppMaxX16 = (3 * pVidTransInfo->bpc * 16 - 1); break; }
    }

    // cap to 12 if DSC_All_Bpp is not set
    if (!pSinkCaps->pHdmiForumInfo->dsc_All_bpp)
    {
        bppMaxX16 = (bppMaxX16 > 12*16) ? 12*16 : bppMaxX16;
    }

    if (pVidTransInfo->bDualHeadMode && (bppMaxX16 > pSrcCaps->dscCaps.dualHeadBppTargetMaxX16))
    {
        bppMaxX16 = pSrcCaps->dscCaps.dualHeadBppTargetMaxX16;
    }

    *pBppMinX16 = bppMinX16;
    *pBppMaxX16 = bppMaxX16;
}


// Determine minimum FRL rate at which Video Transport is possible at given min bpp
// Once FRL rate is found, determine the max bpp possible at this FRL rate
// To determine Primary Compressed Format using this function caller must pass in the full range of min, max FRL and min, max Bpp
// For any optimizations on top of the Primary Compressed Format, caller must adjust the range of these

static NVHDMIPKT_RESULT
determineCompressedFRLConfig(NVHDMIPKT_CLASS                             *pThis,
                             NV0073_CTRL_FRL_CAPACITY_COMPUTATION_PARAMS *pFRLParams,
                             HDMI_FRL_DATA_RATE                           minFRLRate,
                             HDMI_FRL_DATA_RATE                           maxFRLRate,
                             NvU32                                        bppMinX16,
                             NvU32                                        bppMaxX16,
                             NV0073_CTRL_FRL_CAPACITY_COMPUTATION_RESULT *pResults)
{
    NV0073_CTRL_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION_PARAMS *pGetHdmiFrlCapacityComputationParams = NULL;
    HDMI_FRL_DATA_RATE frlRate = minFRLRate;
    NvU32 bppTargetX16         = bppMinX16;
    NVHDMIPKT_RESULT status = NVHDMIPKT_INSUFFICIENT_BANDWIDTH;

    pGetHdmiFrlCapacityComputationParams = pThis->callback.malloc(pThis->cbHandle, sizeof(NV0073_CTRL_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION_PARAMS));

    // Set bppTarget to min and iterate over FRL rates
    pFRLParams->compressionInfo.bppTargetx16 = bppMinX16;
    while (frlRate != HDMI_FRL_DATA_RATE_NONE)
    {
        translateBitRate(frlRate, &pFRLParams->frlBitRateGbps, &pFRLParams->numLanes);
        if (pGetHdmiFrlCapacityComputationParams)
        {
            NVMISC_MEMSET(pGetHdmiFrlCapacityComputationParams, 0, sizeof(*pGetHdmiFrlCapacityComputationParams));
            pGetHdmiFrlCapacityComputationParams->input = *pFRLParams;
            pGetHdmiFrlCapacityComputationParams->cmd = NV0073_CTRL_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION_CMD_COMPRESSED_VIDEO;
#if NVHDMIPKT_RM_CALLS_INTERNAL
            if (CALL_DISP_RM(NvRmControl)(pThis->clientHandles.hClient,
                            pThis->clientHandles.hDisplay,
                            NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION,
                            pGetHdmiFrlCapacityComputationParams,
                            sizeof(NV0073_CTRL_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION_PARAMS)) == NVOS_STATUS_SUCCESS)
#else // !NVHDMIPKT_RM_CALLS_INTERNAL
            NvBool bSuccess = pThis->callback.rmDispControl2(pThis->cbHandle,
                              0,
                              NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION, 
                              pGetHdmiFrlCapacityComputationParams, 
                              sizeof(NV0073_CTRL_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION_PARAMS));
            if (bSuccess == NV_TRUE)
#endif // NVHDMIPKT_RM_CALLS_INTERNAL
            {
                *pResults = pGetHdmiFrlCapacityComputationParams->result;
            }
        }
        else
        {
            status = NVHDMIPKT_FAIL;
            goto compressedQuery_exit;
        }

        status = (pResults->isVideoTransportSupported && pResults->isAudioSupported) ? NVHDMIPKT_SUCCESS : status;

        if ((status == NVHDMIPKT_SUCCESS) ||
            (frlRate == maxFRLRate))
        {
            break;
        }

        frlRate = getNextHigherLinkRate(frlRate);
    }

    if (status != NVHDMIPKT_SUCCESS)
    {
        goto compressedQuery_exit;
    }

    // We now have the base FRL rate. Iterate over bppTarget to find the max supported bpp
    status = NVHDMIPKT_INSUFFICIENT_BANDWIDTH;
    bppTargetX16 = bppMaxX16;
    NvU32 stepSize = 16;

    while (status != NVHDMIPKT_SUCCESS)
    {
        pFRLParams->compressionInfo.bppTargetx16 = bppTargetX16;
        if (pGetHdmiFrlCapacityComputationParams)
        {
            NVMISC_MEMSET(pGetHdmiFrlCapacityComputationParams, 0, sizeof(*pGetHdmiFrlCapacityComputationParams));
            pGetHdmiFrlCapacityComputationParams->input = *pFRLParams;
            pGetHdmiFrlCapacityComputationParams->cmd = NV0073_CTRL_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION_CMD_COMPRESSED_VIDEO;
#if NVHDMIPKT_RM_CALLS_INTERNAL
            if (CALL_DISP_RM(NvRmControl)(pThis->clientHandles.hClient,
                            pThis->clientHandles.hDisplay,
                            NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION,
                            pGetHdmiFrlCapacityComputationParams,
                            sizeof(NV0073_CTRL_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION_PARAMS)) == NVOS_STATUS_SUCCESS)
#else // !NVHDMIPKT_RM_CALLS_INTERNAL
            NvBool bSuccess = pThis->callback.rmDispControl2(pThis->cbHandle,
                              0,
                              NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION, 
                              pGetHdmiFrlCapacityComputationParams, 
                              sizeof(NV0073_CTRL_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION_PARAMS));
            if (bSuccess == NV_TRUE)
#endif // NVHDMIPKT_RM_CALLS_INTERNAL
            {
                *pResults = pGetHdmiFrlCapacityComputationParams->result;
            }
        }
        else
        {
            status = NVHDMIPKT_FAIL;
            goto compressedQuery_exit;
        }

        status = (pResults->isVideoTransportSupported && pResults->isAudioSupported) ? NVHDMIPKT_SUCCESS : status;

        if (status == NVHDMIPKT_SUCCESS)
        {
            // If this is the maxBpp nothing else to try
            if (bppTargetX16 == bppMaxX16)
            {
                break;
            }

            // If we detected a successful bppTarget value, go up a step size,
            // and iterate by decrementing bppTarget by 1/16 to reach a finer tuned bpp value
            if (stepSize == 16)
            {
                status = NVHDMIPKT_RETRY;
                bppTargetX16 = bppTargetX16 + stepSize - 1;
                stepSize = 1;
            }
        }
        else
        {
            bppTargetX16 = bppTargetX16 - stepSize;
            // bppTargetX16 is guaranteed to be >= bppMinX16
        }
    }

    pResults->frlRate = frlRate;
    pResults->bppTargetx16 = bppTargetX16;

compressedQuery_exit:
    if (pGetHdmiFrlCapacityComputationParams)
    {
        pThis->callback.free(pThis->cbHandle, pGetHdmiFrlCapacityComputationParams);
    }

    return status;
}

/*
 * hdmiQueryFRLConfigC671
 *
 * This function uses below logic:
 *  Verify if force params from client are in expected range
 *  If client is not asking for optimum config or force enable DSC, try uncompressed first
 *  For DSC enabled, honor all choices client has made for slice count/width. Determine the primary compressed format (PCF) first.
 *  For any other items client wants to control do this as optimization on top of the PCF
 *  Call DSC library for PPS generation unless specified otherwise. 
 */
static NVHDMIPKT_RESULT
hdmiQueryFRLConfigC671(NVHDMIPKT_CLASS                         *pThis,
                       HDMI_VIDEO_TRANSPORT_INFO         const * const pVidTransInfo,
                       HDMI_QUERY_FRL_CLIENT_CONTROL     const * const pClientCtrl,
                       HDMI_SRC_CAPS                     const * const pSrcCaps,
                       HDMI_SINK_CAPS                    const * const pSinkCaps,
                       HDMI_FRL_CONFIG                   *pFRLConfig)
{
    NV0073_CTRL_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION_PARAMS *pGetHdmiFrlCapacityComputationParams = NULL;
    NVHDMIPKT_RESULT result = NVHDMIPKT_SUCCESS;

    NVMISC_MEMSET(pFRLConfig, 0, sizeof(HDMI_FRL_CONFIG));

    NV0073_CTRL_FRL_CAPACITY_COMPUTATION_PARAMS frlParams;
    NV0073_CTRL_FRL_CAPACITY_COMPUTATION_RESULT frlComputeResult;
    NvU32 bppMinX16, bppMaxX16;

    NVMISC_MEMSET(&frlParams, 0, sizeof(NV0073_CTRL_FRL_CAPACITY_COMPUTATION_PARAMS));
    NVMISC_MEMSET(&frlComputeResult, 0, sizeof(NV0073_CTRL_FRL_CAPACITY_COMPUTATION_RESULT));

    NvU32 vic = NVT_GET_CEA_FORMAT(pVidTransInfo->pTiming->etc.status);
    NvBool bTryUncompressedMode, bCanUseDSC;

    populateBaseFRLParams(pVidTransInfo,
                          pSinkCaps,
                          pClientCtrl->forceAudio2Ch48KHz ? NV_TRUE : NV_FALSE,
                          &frlParams);

    calcBppMinMax(pSrcCaps, pSinkCaps, pVidTransInfo, &bppMinX16, &bppMaxX16);
    bCanUseDSC = evaluateIsDSCPossible(pThis, pSrcCaps, pSinkCaps, pVidTransInfo, &frlParams);
    const NvU32 numHeadsDrivingSink = pVidTransInfo->bDualHeadMode ? 2 : 1;

    // Input validation
    // Note, maxNumHztSlices src cap is per head. account for total number of heads driving the sink
    if ((pClientCtrl->forceFRLRate    && (pClientCtrl->frlRate > pSinkCaps->linkMaxFRLRate)) ||
        (pClientCtrl->enableDSC       && !bCanUseDSC) ||
        (pClientCtrl->forceSliceCount && (pClientCtrl->sliceCount > 
                                          (NvU32)(NV_MIN(pSrcCaps->dscCaps.maxNumHztSlices * numHeadsDrivingSink,
                                                         pSinkCaps->pHdmiForumInfo->dsc_MaxSlices)))) ||
        (pClientCtrl->forceSliceWidth && (pClientCtrl->sliceWidth > NV_MIN(pSrcCaps->dscCaps.maxWidthPerSlice, MAX_RECONSTRUCTED_HACTIVE_PIXELS))) ||
        (pClientCtrl->forceBppx16     && ((pClientCtrl->bitsPerPixelX16 < bppMinX16) || (pClientCtrl->bitsPerPixelX16 > bppMaxX16)))  ||
        (pClientCtrl->forceBppx16     && !pSinkCaps->pHdmiForumInfo->dsc_All_bpp))
    {
        return NVHDMIPKT_INVALID_ARG;
    }

    bTryUncompressedMode = (bCanUseDSC && (pClientCtrl->enableDSC ||
                            (pClientCtrl->option == HDMI_QUERY_FRL_LOWEST_BANDWIDTH))) ?
                            NV_FALSE : NV_TRUE;

    HDMI_FRL_DATA_RATE maxRate = NV_MIN(pSinkCaps->linkMaxFRLRate, pSrcCaps->linkMaxFRLRate);

    pGetHdmiFrlCapacityComputationParams = pThis->callback.malloc(pThis->cbHandle, sizeof(NV0073_CTRL_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION_PARAMS));

    if (bTryUncompressedMode)
    {
        HDMI_FRL_DATA_RATE minFRLRate = HDMI_FRL_DATA_RATE_NONE, maxFRLRate = HDMI_FRL_DATA_RATE_NONE;
        NvBool bHasPreCalcFRLData = NV_FALSE;

        if (pGetHdmiFrlCapacityComputationParams)
        {
            NVMISC_MEMSET(pGetHdmiFrlCapacityComputationParams, 0, sizeof(*pGetHdmiFrlCapacityComputationParams));
            pGetHdmiFrlCapacityComputationParams->preCalc.vic = vic;
            pGetHdmiFrlCapacityComputationParams->cmd = NV0073_CTRL_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION_CMD_HAS_PRECAL_FRL_DATA;
#if NVHDMIPKT_RM_CALLS_INTERNAL
            if (CALL_DISP_RM(NvRmControl)(pThis->clientHandles.hClient,
                            pThis->clientHandles.hDisplay,
                            NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION,
                            pGetHdmiFrlCapacityComputationParams,
                            sizeof(NV0073_CTRL_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION_PARAMS)) == NVOS_STATUS_SUCCESS)
#else // !NVHDMIPKT_RM_CALLS_INTERNAL
            NvBool bSuccess = pThis->callback.rmDispControl2(pThis->cbHandle,
                              0,
                              NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION, 
                              pGetHdmiFrlCapacityComputationParams, 
                              sizeof(NV0073_CTRL_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION_PARAMS));
            if (bSuccess == NV_TRUE)
#endif // NVHDMIPKT_RM_CALLS_INTERNAL
            {
                bHasPreCalcFRLData = pGetHdmiFrlCapacityComputationParams->preCalc.bHasPreCalcFRLData;
            }
            else
            {
                result = NVHDMIPKT_FAIL;
                goto frlQuery_fail;
            }
        }
        else
        {
            result = NVHDMIPKT_FAIL;
            goto frlQuery_fail;
        }
        // We iterate over a range of FRL rates to see if timing is supported in uncompressed manner
        // adjust the min and max range here according to what we aim for: if client wants to force a single FRL rate,
        // min n max point to just this one rate. If client just wants any config, just try max supported rate.
        // For everything else, iterate from lowest to highest FRL rate
        if (pClientCtrl->forceFRLRate)
        {
            minFRLRate = pClientCtrl->frlRate;
            maxFRLRate = pClientCtrl->frlRate;
        }
        else if (pClientCtrl->option == HDMI_QUERY_FRL_HIGHEST_BANDWIDTH)
        {
            minFRLRate = maxRate;
            maxFRLRate = maxRate;
        }
        else if (bHasPreCalcFRLData)
        {
            HDMI_FRL_DATA_RATE preCalcFrlRate;
            pGetHdmiFrlCapacityComputationParams->preCalc.packing = pVidTransInfo->packing;
            pGetHdmiFrlCapacityComputationParams->preCalc.bpc = pVidTransInfo->bpc;
            pGetHdmiFrlCapacityComputationParams->cmd = NV0073_CTRL_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION_CMD_GET_PRECAL_UNCOMPRESSED_FRL_CONFIG;
#if NVHDMIPKT_RM_CALLS_INTERNAL
            if (CALL_DISP_RM(NvRmControl)(pThis->clientHandles.hClient,
                            pThis->clientHandles.hDisplay,
                            NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION,
                            pGetHdmiFrlCapacityComputationParams,
                            sizeof(NV0073_CTRL_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION_PARAMS)) == NVOS_STATUS_SUCCESS)
#else // !NVHDMIPKT_RM_CALLS_INTERNAL
            NvBool bSuccess = pThis->callback.rmDispControl2(pThis->cbHandle,
                              0,
                              NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION, 
                              pGetHdmiFrlCapacityComputationParams, 
                              sizeof(NV0073_CTRL_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION_PARAMS));
            if (bSuccess == NV_TRUE)
#endif // NVHDMIPKT_RM_CALLS_INTERNAL
            {
                preCalcFrlRate = pGetHdmiFrlCapacityComputationParams->preCalc.frlRate;
            }
            else
            {
                result = NVHDMIPKT_FAIL;
                goto frlQuery_fail;
            }
            if (preCalcFrlRate <= maxRate)
            {
                minFRLRate = preCalcFrlRate;
                maxFRLRate = preCalcFrlRate;
            }
            else if (!bCanUseDSC)
            {
                result = NVHDMIPKT_FAIL;
                goto frlQuery_fail;
            }
        }
        else if (pClientCtrl->option == HDMI_QUERY_FRL_ANY_CONFIG)
        {
            minFRLRate = maxRate;
            maxFRLRate = maxRate;
        }
        else // HDMI_QUERY_FRL_OPTIMUM_CONFIG or HDMI_QUERY_FRL_LOWEST_BANDWIDTH
        {
            minFRLRate = HDMI_FRL_DATA_RATE_3LANES_3GBPS;
            maxFRLRate = maxRate;
        }

        result = determineUncompressedFRLConfig(pThis, &frlParams, minFRLRate, maxFRLRate, &frlComputeResult);
        if (result == NVHDMIPKT_SUCCESS)
        {
            goto frlQuery_Success;
        }
        // If we could not find a FRL rate and DSC is not allowed, try using min audio see if it gets us a pass result
        else if (!bCanUseDSC)
        {
            frlParams.numAudioChannels = 2;
            frlParams.audioFreqKHz     = 48;
            frlParams.audioType        = AUDIO_PKTTYPE_LPCM_SAMPLE;
            result = determineUncompressedFRLConfig(pThis, &frlParams, minFRLRate, maxFRLRate, &frlComputeResult);
            // If still not found return failure. Nothing more to try
            if (result != NVHDMIPKT_SUCCESS)
            {
                goto frlQuery_fail;
            }
        }
    }

    if (bCanUseDSC)
    {
        HDMI_FRL_DATA_RATE minFRLRateItr, maxFRLRateItr;
        HDMI_FRL_DATA_RATE dscMaxFRLRate = NV_MIN(pSinkCaps->linkMaxFRLRateDSC, pSrcCaps->linkMaxFRLRate);
        NvU32 bppMinX16Itr, bppMaxX16Itr;
        NvBool bHasPreCalcFRLData = NV_FALSE;

        // DSC_All_bpp = 1:
        //     Lower the compression ratio better the pixel quality, hence a high bppTarget value will be ideal
        //     DSC_All_bpp = 1 allows us the flexibility to use a bppTarget setting different from the primary compressed format
        // DSC_All_bpp = 0:
        //     Per spec, this supports only the bppTarget from primary compressed format - {minimum FRL rate, bpp, HCactive, HCblank}

        minFRLRateItr = HDMI_FRL_DATA_RATE_3LANES_3GBPS;
        maxFRLRateItr = dscMaxFRLRate;
        bppMinX16Itr = bppMinX16;
        bppMaxX16Itr = bppMaxX16;

        if (pGetHdmiFrlCapacityComputationParams)
        {
            NVMISC_MEMSET(pGetHdmiFrlCapacityComputationParams, 0, sizeof(*pGetHdmiFrlCapacityComputationParams));
            pGetHdmiFrlCapacityComputationParams->preCalc.vic = vic;
            pGetHdmiFrlCapacityComputationParams->cmd = NV0073_CTRL_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION_CMD_HAS_PRECAL_FRL_DATA;
#if NVHDMIPKT_RM_CALLS_INTERNAL
            if (CALL_DISP_RM(NvRmControl)(pThis->clientHandles.hClient,
                            pThis->clientHandles.hDisplay,
                            NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION,
                            pGetHdmiFrlCapacityComputationParams,
                            sizeof(NV0073_CTRL_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION_PARAMS)) == NVOS_STATUS_SUCCESS)
#else // !NVHDMIPKT_RM_CALLS_INTERNAL
            NvBool bSuccess = pThis->callback.rmDispControl2(pThis->cbHandle,
                              0,
                              NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION, 
                              pGetHdmiFrlCapacityComputationParams, 
                              sizeof(NV0073_CTRL_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION_PARAMS));
            if (bSuccess == NV_TRUE)
#endif // NVHDMIPKT_RM_CALLS_INTERNAL
            {
                bHasPreCalcFRLData = pGetHdmiFrlCapacityComputationParams->preCalc.bHasPreCalcFRLData;
            }
            else
            {
                result = NVHDMIPKT_FAIL;
                goto frlQuery_fail;
            }
        }
        else
        {
            result = NVHDMIPKT_FAIL;
            goto frlQuery_fail;
        }
        if (bHasPreCalcFRLData)
        {
            HDMI_FRL_DATA_RATE preCalcFrlRate;
            NvU32 preCalcBppx16;

            if (pGetHdmiFrlCapacityComputationParams)
            {
                pGetHdmiFrlCapacityComputationParams->preCalc.packing = pVidTransInfo->packing;
                pGetHdmiFrlCapacityComputationParams->cmd = NV0073_CTRL_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION_CMD_GET_PRECAL_COMPRESSED_FRL_CONFIG;
#if NVHDMIPKT_RM_CALLS_INTERNAL
                if (CALL_DISP_RM(NvRmControl)(pThis->clientHandles.hClient,
                                pThis->clientHandles.hDisplay,
                                NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION,
                                pGetHdmiFrlCapacityComputationParams,
                                sizeof(NV0073_CTRL_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION_PARAMS)) == NVOS_STATUS_SUCCESS)
#else // !NVHDMIPKT_RM_CALLS_INTERNAL
                NvBool bSuccess = pThis->callback.rmDispControl2(pThis->cbHandle,
                                  0,
                                  NV0073_CTRL_CMD_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION, 
                                  pGetHdmiFrlCapacityComputationParams, 
                                  sizeof(NV0073_CTRL_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION_PARAMS));
                if (bSuccess == NV_TRUE)
#endif // NVHDMIPKT_RM_CALLS_INTERNAL
                {
                    preCalcFrlRate = pGetHdmiFrlCapacityComputationParams->preCalc.frlRate;
                    preCalcBppx16 = pGetHdmiFrlCapacityComputationParams->preCalc.bppX16;
                }
                else
                {
                    result = NVHDMIPKT_FAIL;
                    goto frlQuery_fail;
                }
            }
            else
            {
                result = NVHDMIPKT_FAIL;
                goto frlQuery_fail;
            }

            if (preCalcFrlRate != HDMI_FRL_DATA_RATE_UNSPECIFIED)
            {
                if (preCalcFrlRate > dscMaxFRLRate)
                {
                    result = NVHDMIPKT_FAIL;
                    goto frlQuery_fail;
                }
                maxFRLRateItr = minFRLRateItr = preCalcFrlRate;
                bppMaxX16Itr = bppMinX16Itr = preCalcBppx16;
            }
        }

        // force SliceWidth and count if requested
        if (pClientCtrl->forceSliceCount)
        {
            frlParams.compressionInfo.hSlices    = pClientCtrl->sliceCount;
            frlParams.compressionInfo.sliceWidth = NV_UNSIGNED_DIV_CEIL(pVidTransInfo->pTiming->HVisible, pClientCtrl->sliceCount);
        }
        else if (pClientCtrl->forceSliceWidth)
        {
            frlParams.compressionInfo.sliceWidth = pClientCtrl->sliceWidth;
            frlParams.compressionInfo.hSlices    = NV_UNSIGNED_DIV_CEIL(pVidTransInfo->pTiming->HVisible, pClientCtrl->sliceWidth);
        }

        if (pClientCtrl->forceFRLRate)
        {
            if (pClientCtrl->frlRate > dscMaxFRLRate)
            {
                result = NVHDMIPKT_FAIL;
                goto frlQuery_fail;
            }

            minFRLRateItr = pClientCtrl->frlRate;
            maxFRLRateItr = pClientCtrl->frlRate;
        }

        if (pClientCtrl->forceBppx16)
        {
            bppMinX16Itr = pClientCtrl->bitsPerPixelX16;
            bppMaxX16Itr = pClientCtrl->bitsPerPixelX16;
        }

        // Determine Primary Compressed Format
        // First determine the FRL rate at which video transport is possible even at bppMin
        // Then iterate over bppTarget - start at max n decrement until we hit bppMin. The max bpp for which 
        // video transport is possible together with the FRL rate is the primary compressed format
    
        result = determineCompressedFRLConfig(pThis, &frlParams,
                                              minFRLRateItr, maxFRLRateItr,
                                              bppMinX16Itr, bppMaxX16Itr,
                                              &frlComputeResult);

        
        // there are no FRL rates at which video transport is possible even at min bpp
        // Could not even determine PCF. Cannot support this mode
        if (result != NVHDMIPKT_SUCCESS)
        {
            goto frlQuery_fail;
        }

        // Any other optimizations we want to do over the Primary Compressed Format?
        {
            NvBool bRedoDSCCalc = NV_FALSE;

            if (pClientCtrl->option == HDMI_QUERY_FRL_HIGHEST_BANDWIDTH)
            {
                NvBool bHasPreCalcFRLData = NV_TRUE;

                if (bHasPreCalcFRLData)
                {
                    frlComputeResult.frlRate = dscMaxFRLRate; 
                }
                else
                {
                    // Keep bppTgt calculated as Primary Compressed Format and use FRL rate the highest availableLinkBw
                    // redo DSC calculations to recalculate TBlanktoTTotal ratio and HCblank/active to suit the new rate
                    // The hw method setting matters and may cause blank screen if not recalculated - see Bug 3458295 #9
                    minFRLRateItr = maxFRLRateItr = dscMaxFRLRate;
                    bppMinX16Itr = bppMaxX16Itr = frlComputeResult.bppTargetx16;
                    bRedoDSCCalc = NV_TRUE;
                }
            }

            if (pSinkCaps->pHdmiForumInfo->dsc_All_bpp)
            {
                if ((pClientCtrl->option == HDMI_QUERY_FRL_HIGHEST_PIXEL_QUALITY) &&
                    (frlComputeResult.frlRate < (NvU32)dscMaxFRLRate))
                {
                    // Increase FRL rate if possible and iterate over primary compressed format bppTarget to max Bpp
                    minFRLRateItr = getNextHigherLinkRate(frlComputeResult.frlRate);
                    bppMinX16Itr = frlComputeResult.bppTargetx16;
                    bppMaxX16Itr = bppMaxX16;
                    bRedoDSCCalc = NV_TRUE;
                }

                if (pClientCtrl->option == HDMI_QUERY_FRL_LOWEST_BANDWIDTH)
                {
                    // Keep FRL rate as the primary compressed format rate and force Bpp to Min
                    minFRLRateItr = maxFRLRateItr = frlComputeResult.frlRate;
                    bppMinX16Itr = bppMaxX16Itr = bppMinX16;
                    bRedoDSCCalc = NV_TRUE;
                }
            }

            if (bRedoDSCCalc)
            {
                NV0073_CTRL_FRL_CAPACITY_COMPUTATION_PARAMS optQueryParams;
                NV0073_CTRL_FRL_CAPACITY_COMPUTATION_RESULT optQueryResult;
                NVMISC_MEMCPY(&optQueryParams, &frlParams, sizeof(NV0073_CTRL_FRL_CAPACITY_COMPUTATION_PARAMS));

                // If optimization is successful copy over new results. If not, no need to fail, keep Primary Compressed Format
                if(determineCompressedFRLConfig(pThis, &optQueryParams, minFRLRateItr, maxFRLRateItr,
                                                bppMinX16Itr, bppMaxX16Itr,
                                                &optQueryResult) == NVHDMIPKT_SUCCESS)
                {
                    NVMISC_MEMCPY(&frlParams, &optQueryParams, sizeof(NV0073_CTRL_FRL_CAPACITY_COMPUTATION_PARAMS));
                    NVMISC_MEMCPY(&frlComputeResult, &optQueryResult, sizeof(NV0073_CTRL_FRL_CAPACITY_COMPUTATION_RESULT));
                }
            }
        }
    }

frlQuery_Success:
    pFRLConfig->maxSupportedAudioCh = frlParams.numAudioChannels;
    pFRLConfig->maxSupportedAudioFreqKHz = frlParams.audioFreqKHz;
    pFRLConfig->dscInfo.sliceCount = frlParams.compressionInfo.hSlices;
    pFRLConfig->dscInfo.sliceWidth = frlParams.compressionInfo.sliceWidth;

    pFRLConfig->frlRate                 = frlComputeResult.frlRate;
    pFRLConfig->dscInfo.bEnableDSC      = frlComputeResult.engageCompression;
    pFRLConfig->dscInfo.bitsPerPixelX16 = frlComputeResult.bppTargetx16;
    pFRLConfig->dscInfo.dscHActiveBytes = frlComputeResult.hcActiveBytes;
    pFRLConfig->dscInfo.dscHActiveTriBytes = frlComputeResult.hcActiveTriBytes;
    pFRLConfig->dscInfo.dscHBlankTriBytes  = frlComputeResult.hcBlankTriBytes;
    pFRLConfig->dscInfo.dscTBlankToTTotalRatioX1k = frlComputeResult.tBlankToTTotalX1k;

    if (pFRLConfig->dscInfo.bEnableDSC && !pClientCtrl->skipGeneratePPS)
    {
        DSC_INFO     dscInfo;
        MODESET_INFO dscModesetInfo;
        WAR_DATA     warData;

        NVMISC_MEMSET(&dscInfo       , 0, sizeof(DSC_INFO));
        NVMISC_MEMSET(&dscModesetInfo, 0, sizeof(MODESET_INFO));
        NVMISC_MEMSET(&warData       , 0, sizeof(WAR_DATA));

        populateDscCaps(pSrcCaps, pSinkCaps, &dscInfo);
        populateDscModesetInfo(pVidTransInfo, &dscModesetInfo);

        dscInfo.forcedDscParams.sliceWidth = pFRLConfig->dscInfo.sliceWidth;
        dscInfo.forcedDscParams.dscRevision.versionMajor = 1;
        dscInfo.forcedDscParams.dscRevision.versionMinor = 2;

        NvU32 bitsPerPixelX16 = pFRLConfig->dscInfo.bitsPerPixelX16;
        NvU32 frlBitRateGbps = 0, numLanes = 0;
        translateBitRate(pFRLConfig->frlRate, &frlBitRateGbps, &numLanes);
        NvU64 availableLinkBw = (NvU64)(frlBitRateGbps) * (NvU64)(numLanes) * MULTIPLIER_1G;
        warData.connectorType = DSC_HDMI;

        DSC_GENERATE_PPS_OPAQUE_WORKAREA *pDscScratchBuffer = NULL;
        pDscScratchBuffer = (DSC_GENERATE_PPS_OPAQUE_WORKAREA*)pThis->callback.malloc(pThis->cbHandle, 
                                                                      sizeof(DSC_GENERATE_PPS_OPAQUE_WORKAREA));
        if ((DSC_GeneratePPS(&dscInfo,
                             &dscModesetInfo,
                             &warData,
                             availableLinkBw,
                             pDscScratchBuffer,
                             pFRLConfig->dscInfo.pps,
                             &bitsPerPixelX16)) != NVT_STATUS_SUCCESS)
        {
            NvHdmiPkt_Print(pThis, "ERROR - DSC PPS calculation failed.");
            NvHdmiPkt_Assert(0);
            result = NVHDMIPKT_DSC_PPS_ERROR;
        }

        if (pDscScratchBuffer != NULL)
        {
            pThis->callback.free(pThis->cbHandle, pDscScratchBuffer);
            pDscScratchBuffer = NULL;
        }

        // DSC lib should honor the bpp setting passed from client, assert here just in case
        NvHdmiPkt_Assert(bitsPerPixelX16 == pFRLConfig->dscInfo.bitsPerPixelX16);
    }

frlQuery_fail:
    if (pGetHdmiFrlCapacityComputationParams)
    {
        pThis->callback.free(pThis->cbHandle, pGetHdmiFrlCapacityComputationParams);
    }

    return result;
}

/*
 * hdmiSetFRLConfigC671
 */
static NVHDMIPKT_RESULT
hdmiSetFRLConfigC671(NVHDMIPKT_CLASS             *pThis,
                     NvU32                        subDevice,
                     NvU32                        displayId,
                     NvBool                       bFakeLt,
                     HDMI_FRL_CONFIG             *pFRLConfig)
{
    NV0073_CTRL_SPECIFIC_SET_HDMI_FRL_LINK_CONFIG_PARAMS params = {0};
    NVMISC_MEMSET(&params, 0, sizeof(params));
    params.subDeviceInstance = subDevice;
    params.displayId = displayId;
    params.data = translateFRLRateToNv0073SetHdmiFrlConfig(pFRLConfig->frlRate);
    params.bFakeLt = bFakeLt;

#if NVHDMIPKT_RM_CALLS_INTERNAL
    if (CALL_DISP_RM(NvRmControl)(pThis->clientHandles.hClient,
                    pThis->clientHandles.hDisplay,
                    NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_FRL_CONFIG,
                    &params,
                    sizeof(params)) != NVOS_STATUS_SUCCESS)

#else // !NVHDMIPKT_RM_CALLS_INTERNAL
    NvBool bSuccess = pThis->callback.rmDispControl2(pThis->cbHandle,
                                                     params.subDeviceInstance,
                                                     NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_FRL_CONFIG, 
                                                     &params, 
                                                     sizeof(params));
    if (bSuccess == NV_FALSE)
#endif // NVHDMIPKT_RM_CALLS_INTERNAL
    {
        NvHdmiPkt_Print(pThis, "ERROR - RM call to set HDMI FRL failed.");
        NvHdmiPkt_Assert(0);

        return NVHDMIPKT_FAIL;
    }

    return NVHDMIPKT_SUCCESS;
}

/*
 * hdmiClearFRLConfigC671
 */
static NVHDMIPKT_RESULT
hdmiClearFRLConfigC671(NVHDMIPKT_CLASS    *pThis,
                       NvU32               subDevice,
                       NvU32               displayId)
{
    NVHDMIPKT_RESULT result = NVHDMIPKT_SUCCESS;

    NV0073_CTRL_SPECIFIC_SET_HDMI_FRL_LINK_CONFIG_PARAMS params = {0};
    NVMISC_MEMSET(&params, 0, sizeof(params));
    params.subDeviceInstance = subDevice;
    params.displayId         = displayId;
    params.data              = NV0073_CTRL_HDMI_FRL_DATA_SET_FRL_RATE_NONE;

#if NVHDMIPKT_RM_CALLS_INTERNAL
    if (CALL_DISP_RM(NvRmControl)(pThis->clientHandles.hClient,
                    pThis->clientHandles.hDisplay,
                    NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_FRL_CONFIG,
                    &params,
                    sizeof(params)) != NVOS_STATUS_SUCCESS)
#else // !NVHDMIPKT_RM_CALLS_INTERNAL
    NvBool bSuccess = pThis->callback.rmDispControl2(pThis->cbHandle,
                                                     params.subDeviceInstance,
                                                     NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_FRL_CONFIG, 
                                                     &params, 
                                                     sizeof(params));
    
    if (bSuccess == NV_FALSE)
#endif // NVHDMIPKT_RM_CALLS_INTERNAL
    {
        NvHdmiPkt_Print(pThis, "WARNING - RM call to reset HDMI FRL failed.");
        result = NVHDMIPKT_FAIL;
    }
    return result;
}

static NVHDMIPKT_RESULT 
hdmiPacketWriteC671(NVHDMIPKT_CLASS*   pThis,
                    NvU32              subDevice,
                    NvU32              displayId,
                    NvU32              head,
                    NVHDMIPKT_TYPE     packetType,
                    NVHDMIPKT_TC       transmitControl, 
                    NvU32              packetLen, 
                    NvU8 const *const  pPacket)
{
    NVHDMIPKT_RESULT result = NVHDMIPKT_SUCCESS;
    NvU32  pktTypeC671      = pThis->translatePacketType(pThis, packetType);

    if (head >= NVC671_SF_HDMI_INFO_CTRL__SIZE_1          ||
        packetLen == 0                                    || 
        pPacket == 0                                      || 
        pktTypeC671 == NVHDMIPKT_C671_INVALID_PKT_TYPE)
    {
        result = NVHDMIPKT_INVALID_ARG;
        goto hdmiPacketWriteC671_exit;
    }

    if (pktTypeC671 == NVC671_SF_HDMI_INFO_IDX_GENERIC_INFOFRAME)
    {
        // In GA10X, we use Generic infoframe for ACR WAR. This RM ctrl is used to control if the WAR is enabled/not. 
        NV0073_CTRL_SYSTEM_GET_CAPS_V2_PARAMS dispCapsParams;

        NVMISC_MEMSET(&dispCapsParams, 0, sizeof(dispCapsParams));

#if NVHDMIPKT_RM_CALLS_INTERNAL
        if (NvRmControl(pThis->clientHandles.hClient,
                        pThis->clientHandles.hDisplay,
                        NV0073_CTRL_CMD_SYSTEM_GET_CAPS_V2,
                        &dispCapsParams,
                        sizeof(dispCapsParams)) != NVOS_STATUS_SUCCESS)
#else // !NVHDMIPKT_RM_CALLS_INTERNAL
        NvBool bSuccess =  pThis->callback.rmDispControl2(pThis->cbHandle,
                                                        subDevice,
                                                        NV0073_CTRL_CMD_SYSTEM_GET_CAPS_V2, 
                                                        &dispCapsParams, sizeof(dispCapsParams));
        if (bSuccess == NV_FALSE)
#endif // NVHDMIPKT_RM_CALLS_INTERNAL
        {
            NvHdmiPkt_Print(pThis, "ERROR - RM call to get caps failed.");
            NvHdmiPkt_Assert(0);
            result = NVHDMIPKT_FAIL;
            goto hdmiPacketWriteC671_exit;
        }

        NvBool bSwAcr = (NV0073_CTRL_SYSTEM_GET_CAP(dispCapsParams.capsTbl, NV0073_CTRL_SYSTEM_CAPS_HDMI21_SW_ACR_BUG_3275257)) ? NV_TRUE: NV_FALSE;

        if (bSwAcr)
        {
            // acquire mutex
            pThis->callback.acquireMutex(pThis->cbHandle);

            result = hdmiPacketWrite0073(pThis, subDevice, displayId, head, packetType, transmitControl, packetLen, pPacket);
            
            if (result == NVHDMIPKT_SUCCESS)
            {
                result = hdmiPacketCtrl0073(pThis, subDevice, displayId, head, packetType, transmitControl);
            }
            
            // release mutex
            pThis->callback.releaseMutex(pThis->cbHandle);
        }
        else
        {
            result = hdmiPacketWrite9171(pThis, subDevice, displayId, head, packetType, transmitControl, packetLen, pPacket);
        }
    }
    else
    {
        result = hdmiPacketWrite9171(pThis, subDevice, displayId, head, packetType, transmitControl, packetLen, pPacket);
    }

hdmiPacketWriteC671_exit:
    return result;    
}

// non-HW - class utility/maintenance functions
/*
 * hdmiConstructorC671
 */
NvBool 
hdmiConstructorC671(NVHDMIPKT_CLASS* pThis)
{
    NvBool result = NV_TRUE;

    return result;
}

/*
 * hdmiDestructorC671
 */
void 
hdmiDestructorC671(NVHDMIPKT_CLASS* pThis)

{
    return;
}

/*
 * initializeHdmiPktInterfaceC671
 */
void
initializeHdmiPktInterfaceC671(NVHDMIPKT_CLASS* pClass)
{
    pClass->dispSfUserClassId           = NVC671_DISP_SF_USER;
    pClass->hdmiAssessLinkCapabilities  = hdmiAssessLinkCapabilitiesC671;
    pClass->hdmiQueryFRLConfig          = hdmiQueryFRLConfigC671;
    pClass->hdmiSetFRLConfig            = hdmiSetFRLConfigC671;
    pClass->hdmiClearFRLConfig          = hdmiClearFRLConfigC671;
    pClass->hdmiPacketWrite             = hdmiPacketWriteC671;
}
