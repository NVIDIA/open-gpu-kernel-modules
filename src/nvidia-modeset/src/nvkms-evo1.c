/*
 * SPDX-FileCopyrightText: Copyright (c) 2010 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*
 * This file contains implementations of the EVO HAL methods for display class
 * 1.x, found in the Tesla and Fermi 1 (GF10x) chips.
 */

#include "nvkms-types.h"
#include "nvkms-rm.h"
#include "nvkms-rmapi.h"
#include "nvkms-evo1.h"
#include "nvkms-prealloc.h"
#include "nvkms-utils.h"

#include <ctrl/ctrl5070/ctrl5070chnc.h> // NV5070_CTRL_CMD_IS_MODE_POSSIBLE_PARAMS

/*!
 * Initialize head-independent IMP param fields.
 *
 * Initializes an NV5070_CTRL_CMD_IS_MODE_POSSIBLE_PARAMS structure.
 * IMP users should call this once, followed by per-head calls to
 * AssignPerHeadImpParams().
 *
 * \param pImp[in]  A pointer to a param structure.
 */
static void InitImpParams(NV5070_CTRL_CMD_IS_MODE_POSSIBLE_PARAMS *pImp)
{
    int i;

    nvkms_memset(pImp, 0, sizeof(*pImp));

    /* Initialize to not possible. */
    pImp->IsPossible = NV5070_CTRL_CMD_IS_MODE_POSSIBLE_IS_POSSIBLE_NO;

    /* Set all heads to inactive. */
    for (i = 0; i < NV5070_CTRL_CMD_MAX_HEADS; i++) {
        pImp->Head[i].HeadActive =
            NV5070_CTRL_CMD_IS_MODE_POSSIBLE_HEAD_ACTIVE_NO;
    }

    /* Set all ORs to no owner. */
    for (i = 0; i < NV5070_CTRL_CMD_MAX_DACS; i++) {
        pImp->Dac[i].owner = NV5070_CTRL_CMD_OR_OWNER_NONE;
    }

    pImp->bUseSorOwnerMask = TRUE;
    for (i = 0; i < NV5070_CTRL_CMD_MAX_SORS; i++) {
        pImp->Sor[i].ownerMask = NV5070_CTRL_CMD_SOR_OWNER_MASK_NONE;
    }

    for (i = 0; i < NV5070_CTRL_CMD_MAX_PIORS; i++) {
        pImp->Pior[i].owner = NV5070_CTRL_CMD_OR_OWNER_NONE;
    }
}

/*!
 * Initialize head-specific IMP param fields.
 *
 * Initialize the portion of the NV5070_CTRL_CMD_IS_MODE_POSSIBLE_PARAMS
 * structure that applies to a specific head, and the OR driven by
 * that head.
 *
 * The param structure should be initialized by InitImpParams()
 * before calling this per-head function.
 *
 * \param[out]  pImp       The param structure to initialize.
 * \param[in]   pTimings   The rastering timings and viewport configuration.
 * \param[in]   pUsage     The usage bounds that will be used for this head.
 * \param[in]   head       The number of the head that will be driven.
 * \param[in]   orNumber   The number of the OR driven by the head.
 * \param[in]   orType     The type of the OR driven by the head.
 */
static void AssignPerHeadImpParams(const NVDevEvoRec *pDevEvo,
                                   NV5070_CTRL_CMD_IS_MODE_POSSIBLE_PARAMS *pImp,
                                   const NVHwModeTimingsEvo *pTimings,
                                   const enum nvKmsPixelDepth pixelDepth,
                                   const struct NvKmsUsageBounds *pUsage,
                                   const int head,
                                   const int orNumber,
                                   const int orType)
{
    const NVHwModeViewPortEvo *pViewPort = &pTimings->viewPort;
    NvU64 overlayFormats = 0;
    NvU32 protocol;

    nvkms_memset(&pImp->Head[head], 0, sizeof(pImp->Head[head]));

    nvAssert(head < NV5070_CTRL_CMD_MAX_HEADS);
    pImp->Head[head].HeadActive = TRUE;

    nvAssert(orType == NV0073_CTRL_SPECIFIC_OR_TYPE_NONE ||
             orNumber != NV_INVALID_OR);

    /* raster timings */

    pImp->Head[head].PixelClock.Frequency = pTimings->pixelClock;

    pImp->Head[head].PixelClock.Adj1000Div1001 =
        NV5070_CTRL_CMD_IS_MODE_POSSIBLE_PIXEL_CLOCK_ADJ1000DIV1001_NO;

    pImp->Head[head].RasterSize.Width           = pTimings->rasterSize.x;
    pImp->Head[head].RasterSize.Height          = pTimings->rasterSize.y;
    pImp->Head[head].RasterBlankStart.X         = pTimings->rasterBlankStart.x;
    pImp->Head[head].RasterBlankStart.Y         = pTimings->rasterBlankStart.y;
    pImp->Head[head].RasterBlankEnd.X           = pTimings->rasterBlankEnd.x;
    pImp->Head[head].RasterBlankEnd.Y           = pTimings->rasterBlankEnd.y;
    pImp->Head[head].RasterVertBlank2.YStart    = pTimings->rasterVertBlank2Start;
    pImp->Head[head].RasterVertBlank2.YEnd      = pTimings->rasterVertBlank2End;
    pImp->Head[head].Control.Structure =
        pTimings->interlaced ?
        NV5070_CTRL_CMD_IS_MODE_POSSIBLE_CONTROL_STRUCTURE_INTERLACED :
        NV5070_CTRL_CMD_IS_MODE_POSSIBLE_CONTROL_STRUCTURE_PROGRESSIVE;

    if (orType == NV0073_CTRL_SPECIFIC_OR_TYPE_DAC) {
        nvAssert(orNumber < ARRAY_LEN(pImp->Dac));
        nvAssert(pImp->Dac[orNumber].owner == NV5070_CTRL_CMD_OR_OWNER_NONE);
        pImp->Dac[orNumber].owner = NV5070_CTRL_CMD_OR_OWNER_HEAD(head);
        nvAssert(pTimings->protocol == NVKMS_PROTOCOL_DAC_RGB);
        pImp->Dac[orNumber].protocol = NV5070_CTRL_CMD_DAC_PROTOCOL_RGB_CRT;
    } else if (orType == NV0073_CTRL_SPECIFIC_OR_TYPE_SOR) {
        nvAssert(orNumber < ARRAY_LEN(pImp->Sor));
        pImp->Sor[orNumber].ownerMask |= NV5070_CTRL_CMD_SOR_OWNER_MASK_HEAD(head);
        switch (pTimings->protocol) {
            default:
                nvAssert(!"Unknown protocol");
                /* fall through */
            case NVKMS_PROTOCOL_SOR_LVDS_CUSTOM:
                protocol = NV5070_CTRL_CMD_SOR_PROTOCOL_LVDS_CUSTOM;
                break;
            case NVKMS_PROTOCOL_SOR_SINGLE_TMDS_A:
                protocol = NV5070_CTRL_CMD_SOR_PROTOCOL_SINGLE_TMDS_A;
                break;
            case NVKMS_PROTOCOL_SOR_SINGLE_TMDS_B:
                protocol = NV5070_CTRL_CMD_SOR_PROTOCOL_SINGLE_TMDS_B;
                break;
            case NVKMS_PROTOCOL_SOR_DUAL_TMDS:
                protocol = NV5070_CTRL_CMD_SOR_PROTOCOL_DUAL_TMDS;
                break;
            case NVKMS_PROTOCOL_SOR_DP_A:
                protocol = NV5070_CTRL_CMD_SOR_PROTOCOL_DP_A;
                break;
            case NVKMS_PROTOCOL_SOR_DP_B:
                protocol = NV5070_CTRL_CMD_SOR_PROTOCOL_DP_B;
                break;
        }
        pImp->Sor[orNumber].protocol = protocol;
        pImp->Sor[orNumber].pixelReplicateMode =
            NV5070_CTRL_IS_MODE_POSSIBLE_PIXEL_REPLICATE_MODE_OFF;
    } else if (orType == NV0073_CTRL_SPECIFIC_OR_TYPE_PIOR) {
        nvAssert(orNumber < ARRAY_LEN(pImp->Pior));
        nvAssert(pImp->Pior[orNumber].owner == NV5070_CTRL_CMD_OR_OWNER_NONE);
        pImp->Pior[orNumber].owner = NV5070_CTRL_CMD_OR_OWNER_HEAD(head);
        switch (pTimings->protocol) {
            default:
                nvAssert(!"Unknown protocol");
                /* fall through */
            case NVKMS_PROTOCOL_PIOR_EXT_TMDS_ENC:
                protocol = NV5070_CTRL_CMD_PIOR_PROTOCOL_EXT_TMDS_ENC;
                break;
        }
        pImp->Pior[orNumber].protocol = protocol;
    } else {
        nvAssert(orType == NV0073_CTRL_SPECIFIC_OR_TYPE_NONE);
    }

    /* viewport out */

    pImp->Head[head].OutputScaler.VerticalTaps =
        NVEvoScalerTapsToNum(pViewPort->vTaps);

    pImp->Head[head].OutputScaler.HorizontalTaps =
        NVEvoScalerTapsToNum(pViewPort->hTaps);

    pImp->Head[head].ViewportSizeOut.Width = pViewPort->out.width;
    pImp->Head[head].ViewportSizeOut.Height = pViewPort->out.height;

    pImp->Head[head].ViewportSizeOutMin.Width =
        pImp->Head[head].ViewportSizeOut.Width;

    pImp->Head[head].ViewportSizeOutMin.Height =
        pImp->Head[head].ViewportSizeOut.Height;

    pImp->Head[head].ViewportSizeOutMax.Width =
        pImp->Head[head].ViewportSizeOut.Width;

    pImp->Head[head].ViewportSizeOutMax.Height =
        pImp->Head[head].ViewportSizeOut.Height;

    /* viewport in */

    pImp->Head[head].ViewportSizeIn.Width = pViewPort->in.width;
    pImp->Head[head].ViewportSizeIn.Height = pViewPort->in.height;

    /*
     * The actual format doesn't really matter, since RM just
     * converts it back to bits per pixel for its IMP calculation anyway.  The
     * hardware doesn't have a "usage bound" for core -- changing the format
     * of the core surface will always incur a supervisor interrupt and rerun
     * IMP (XXX if we change the core surface as part of a flip to one of a
     * different depth, should we force the pre/post IMP update path?).
     *
     * EVO2 hal uses surfaces of the same format in the core and base channels,
     * see needToReprogramCoreSurface() in nvkms-evo2.c.
     */
    if (pUsage->layer[NVKMS_MAIN_LAYER].usable) {
        if (pUsage->layer[NVKMS_MAIN_LAYER].supportedSurfaceMemoryFormats &
            NVKMS_SURFACE_MEMORY_FORMATS_RGB_PACKED8BPP) {
            pImp->Head[head].Params.Format =
                NV5070_CTRL_CMD_IS_MODE_POSSIBLE_PARAMS_FORMAT_RF16_GF16_BF16_AF16;
        } else if (pUsage->layer[NVKMS_MAIN_LAYER].supportedSurfaceMemoryFormats &
            NVKMS_SURFACE_MEMORY_FORMATS_RGB_PACKED4BPP) {
            pImp->Head[head].Params.Format =
                NV5070_CTRL_CMD_IS_MODE_POSSIBLE_PARAMS_FORMAT_A8R8G8B8;
        } else if (pUsage->layer[NVKMS_MAIN_LAYER].supportedSurfaceMemoryFormats &
            NVKMS_SURFACE_MEMORY_FORMATS_RGB_PACKED2BPP) {
            pImp->Head[head].Params.Format =
                NV5070_CTRL_CMD_IS_MODE_POSSIBLE_PARAMS_FORMAT_R5G6B5;
        } else if (pUsage->layer[NVKMS_MAIN_LAYER].supportedSurfaceMemoryFormats &
            NVKMS_SURFACE_MEMORY_FORMATS_RGB_PACKED1BPP) {
            pImp->Head[head].Params.Format =
                NV5070_CTRL_CMD_IS_MODE_POSSIBLE_PARAMS_FORMAT_I8;
        } else { /* default to RGB 4BPP */
            nvAssert(!"Unknown core format");
            pImp->Head[head].Params.Format =
                NV5070_CTRL_CMD_IS_MODE_POSSIBLE_PARAMS_FORMAT_A8R8G8B8;
        }
    } else {
        pImp->Head[head].Params.Format =
            NV5070_CTRL_CMD_IS_MODE_POSSIBLE_PARAMS_FORMAT_A8R8G8B8;
    }

    pImp->Head[head].Params.SuperSample =
        NV5070_CTRL_CMD_IS_MODE_POSSIBLE_PARAMS_SUPER_SAMPLE_X1AA;

    /* base usage bounds */

    if (pUsage->layer[NVKMS_MAIN_LAYER].usable) {
        pImp->Head[head].BaseUsageBounds.Usable =
            NV5070_CTRL_CMD_IS_MODE_POSSIBLE_BASE_USAGE_BOUNDS_USABLE_YES;

        if (pUsage->layer[NVKMS_MAIN_LAYER].supportedSurfaceMemoryFormats &
            NVKMS_SURFACE_MEMORY_FORMATS_RGB_PACKED8BPP) {
            pImp->Head[head].BaseUsageBounds.PixelDepth =
                NV5070_CTRL_CMD_IS_MODE_POSSIBLE_BASE_USAGE_BOUNDS_PIXEL_DEPTH_64;
        } else if (pUsage->layer[NVKMS_MAIN_LAYER].supportedSurfaceMemoryFormats &
            NVKMS_SURFACE_MEMORY_FORMATS_RGB_PACKED4BPP) {
            pImp->Head[head].BaseUsageBounds.PixelDepth =
                NV5070_CTRL_CMD_IS_MODE_POSSIBLE_BASE_USAGE_BOUNDS_PIXEL_DEPTH_32;
        } else if (pUsage->layer[NVKMS_MAIN_LAYER].supportedSurfaceMemoryFormats &
            NVKMS_SURFACE_MEMORY_FORMATS_RGB_PACKED2BPP) {
            pImp->Head[head].BaseUsageBounds.PixelDepth =
                NV5070_CTRL_CMD_IS_MODE_POSSIBLE_BASE_USAGE_BOUNDS_PIXEL_DEPTH_16;
        } else if (pUsage->layer[NVKMS_MAIN_LAYER].supportedSurfaceMemoryFormats &
            NVKMS_SURFACE_MEMORY_FORMATS_RGB_PACKED1BPP) {
            pImp->Head[head].BaseUsageBounds.PixelDepth =
                NV5070_CTRL_CMD_IS_MODE_POSSIBLE_BASE_USAGE_BOUNDS_PIXEL_DEPTH_8;
        } else { /* default to RGB 8BPP */
            nvAssert(!"Unknown base channel usage bound format");
            pImp->Head[head].BaseUsageBounds.PixelDepth =
                NV5070_CTRL_CMD_IS_MODE_POSSIBLE_BASE_USAGE_BOUNDS_PIXEL_DEPTH_64;
        }

        pImp->Head[head].BaseUsageBounds.SuperSample =
            NV5070_CTRL_CMD_IS_MODE_POSSIBLE_BASE_USAGE_BOUNDS_SUPER_SAMPLE_X1AA;
    } else {
        pImp->Head[head].BaseUsageBounds.Usable =
            NV5070_CTRL_CMD_IS_MODE_POSSIBLE_BASE_USAGE_BOUNDS_USABLE_NO;
    }

    /* overlay usage bounds */

    pImp->Head[head].OverlayUsageBounds.Usable =
        pUsage->layer[NVKMS_OVERLAY_LAYER].usable
        ? NV5070_CTRL_CMD_IS_MODE_POSSIBLE_OVERLAY_USAGE_BOUNDS_USABLE_YES
        : NV5070_CTRL_CMD_IS_MODE_POSSIBLE_OVERLAY_USAGE_BOUNDS_USABLE_NO;

    overlayFormats = pUsage->layer[NVKMS_OVERLAY_LAYER].usable ?
        pUsage->layer[NVKMS_OVERLAY_LAYER].supportedSurfaceMemoryFormats :
        NVKMS_SURFACE_MEMORY_FORMATS_RGB_PACKED2BPP;

    if (overlayFormats & NVKMS_SURFACE_MEMORY_FORMATS_RGB_PACKED4BPP) {
        pImp->Head[head].OverlayUsageBounds.PixelDepth =
        NV5070_CTRL_CMD_IS_MODE_POSSIBLE_OVERLAY_USAGE_BOUNDS_PIXEL_DEPTH_32;
    } else if (overlayFormats & NVKMS_SURFACE_MEMORY_FORMATS_RGB_PACKED2BPP) {
        pImp->Head[head].OverlayUsageBounds.PixelDepth =
        NV5070_CTRL_CMD_IS_MODE_POSSIBLE_OVERLAY_USAGE_BOUNDS_PIXEL_DEPTH_16;
    } else {
        nvAssert(!"Unknown overlay channel usage bound format");
        pImp->Head[head].OverlayUsageBounds.PixelDepth =
        NV5070_CTRL_CMD_IS_MODE_POSSIBLE_OVERLAY_USAGE_BOUNDS_PIXEL_DEPTH_32;
    }

    /* pixel depth */

    switch (pixelDepth) {
    case NVKMS_PIXEL_DEPTH_18_444:
        pImp->Head[head].outputResourcePixelDepthBPP =
            NV5070_CTRL_IS_MODE_POSSIBLE_OUTPUT_RESOURCE_PIXEL_DEPTH_BPP_18_444;
        break;
    case NVKMS_PIXEL_DEPTH_24_444:
        pImp->Head[head].outputResourcePixelDepthBPP =
            NV5070_CTRL_IS_MODE_POSSIBLE_OUTPUT_RESOURCE_PIXEL_DEPTH_BPP_24_444;
        break;
    case NVKMS_PIXEL_DEPTH_30_444:
        pImp->Head[head].outputResourcePixelDepthBPP =
            NV5070_CTRL_IS_MODE_POSSIBLE_OUTPUT_RESOURCE_PIXEL_DEPTH_BPP_30_444;
        break;
    case NVKMS_PIXEL_DEPTH_16_422:
        pImp->Head[head].outputResourcePixelDepthBPP =
            NV5070_CTRL_IS_MODE_POSSIBLE_OUTPUT_RESOURCE_PIXEL_DEPTH_BPP_16_422;
        break;
    case NVKMS_PIXEL_DEPTH_20_422:
        pImp->Head[head].outputResourcePixelDepthBPP =
            NV5070_CTRL_IS_MODE_POSSIBLE_OUTPUT_RESOURCE_PIXEL_DEPTH_BPP_20_422;
        break;
    }
}

void nvEvo1IsModePossible(NVDispEvoPtr pDispEvo,
                          const NVEvoIsModePossibleDispInput *pInput,
                          NVEvoIsModePossibleDispOutput *pOutput)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NV5070_CTRL_CMD_IS_MODE_POSSIBLE_PARAMS *pImp =
        nvPreallocGet(pDevEvo, PREALLOC_TYPE_IMP_PARAMS, sizeof(*pImp));
    NvBool result = FALSE;
    NvU32 head;
    NvU32 ret;

    InitImpParams(pImp);

    pImp->RequestedOperation =
        NV5070_CTRL_CMD_IS_MODE_POSSIBLE_REQUESTED_OPERATION_QUERY;

    for (head = 0; head < NVKMS_MAX_HEADS_PER_DISP; head++) {
        if (pInput->head[head].pTimings == NULL) {
            continue;
        }

        AssignPerHeadImpParams(pDevEvo, pImp,
                               pInput->head[head].pTimings,
                               pInput->head[head].pixelDepth,
                               pInput->head[head].pUsage,
                               head,
                               pInput->head[head].orIndex,
                               pInput->head[head].orType);
    }

    pImp->base.subdeviceIndex = pDispEvo->displayOwner;

    if (pInput->requireBootClocks) {
        // XXX TODO: IMP requires lock pin information if pstate information is
        // requested. For now, just assume no locking.
        pImp->MinPState = NV5070_CTRL_IS_MODE_POSSIBLE_NEED_MIN_PSTATE;
    }

    for (head = 0; head < NVKMS_MAX_HEADS_PER_DISP; head++) {
        pImp->Head[head].displayId[0] = pInput->head[head].displayId;
    }

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->displayHandle,
                         NV5070_CTRL_CMD_IS_MODE_POSSIBLE,
                         pImp, sizeof(*pImp));

    if (ret != NV_OK || !pImp->IsPossible ||
        (pInput->requireBootClocks &&
         // P8 = "boot clocks"
         (pImp->MinPState < NV5070_CTRL_IS_MODE_POSSIBLE_PSTATES_P8 &&
          // XXX TODO: With PStates 3.0, only a "v-pstate" is returned in
          // impParams.minPerfLevel. We need to correlate that with "boot
          // clocks" somehow.
          pImp->MinPState != NV5070_CTRL_IS_MODE_POSSIBLE_PSTATES_UNDEFINED))) {
        goto done;
    }

    result = TRUE;

done:
    nvPreallocRelease(pDevEvo, PREALLOC_TYPE_IMP_PARAMS);
    pOutput->possible = result;
}

void nvEvo1PrePostIMP(NVDispEvoPtr pDispEvo, NvBool isPre)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NV5070_CTRL_CMD_IS_MODE_POSSIBLE_PARAMS *pImp =
        nvPreallocGet(pDevEvo, PREALLOC_TYPE_IMP_PARAMS, sizeof(*pImp));
    NvU32 ret;

    if (isPre) {
        /*
         * Sync the core channel for pre-modeset IMP to ensure that the state
         * cache reflects all of the methods we've pushed
         */
        ret = nvRMSyncEvoChannel(pDevEvo, pDevEvo->core, __LINE__);
        if (!ret) {
            nvAssert(!"nvRMSyncEvoChannel failed during PreModesetIMP");
        }
    }

    nvkms_memset(pImp, 0, sizeof(*pImp));

    pImp->RequestedOperation = isPre ?
        NV5070_CTRL_CMD_IS_MODE_POSSIBLE_REQUESTED_OPERATION_PRE_MODESET_USE_SC :
        NV5070_CTRL_CMD_IS_MODE_POSSIBLE_REQUESTED_OPERATION_POST_MODESET_USE_SC;

    pImp->base.subdeviceIndex = pDispEvo->displayOwner;

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->displayHandle,
                         NV5070_CTRL_CMD_IS_MODE_POSSIBLE,
                         pImp, sizeof(*pImp));
    if ((ret != NVOS_STATUS_SUCCESS) || !pImp->IsPossible) {
        nvAssert(!"NV5070_CTRL_CMD_IS_MODE_POSSIBLE failed");
    }

    nvPreallocRelease(pDevEvo, PREALLOC_TYPE_IMP_PARAMS);
}

/*!
 * Return the value to use for HEAD_SET_STORAGE_PITCH.
 *
 * Per dispClass_02.mfs, the HEAD_SET_STORAGE_PITCH "units are blocks
 * if the layout is BLOCKLINEAR, the units are multiples of 256 bytes
 * if the layout is PITCH."
 *
 * \return   Returns 0 if the pitch is invalid.  Otherwise returns the
 *           HEAD_SET_STORAGE_PITCH value.
 */
NvU32 nvEvoGetHeadSetStoragePitchValue(const NVDevEvoRec *pDevEvo,
                                       enum NvKmsSurfaceMemoryLayout layout,
                                       NvU32 pitch)
{
    if (layout == NvKmsSurfaceMemoryLayoutBlockLinear) {
        /* pitch is already in units of blocks; nothing else needed. */
    } else {
        /* pitch is in units of bytes, and must be aligned to 0x100. */
        if ((pitch & 0xFF) != 0) {
            return 0;
        }

        pitch >>= 8;
    }

    if (pitch > pDevEvo->caps.maxPitchValue) {
        return 0;
    }

    return pitch;
}

static NvBool GetChannelState(NVDevEvoPtr pDevEvo,
                              NVEvoChannelPtr pChan,
                              NvU32 sd,
                              NvU32 *result)
{
    NV5070_CTRL_CMD_GET_CHANNEL_INFO_PARAMS info = { };
    NvU32 ret;

    info.base.subdeviceIndex = sd;
    info.channelClass = pChan->hwclass;
    info.channelInstance = pChan->instance;

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->displayHandle,
                         NV5070_CTRL_CMD_GET_CHANNEL_INFO,
                         &info, sizeof(info));
    if (ret != NVOS_STATUS_SUCCESS) {
        nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                    "Failed to query display engine channel state: 0x%08x:%d:%d:0x%08x",
                    pChan->hwclass, pChan->instance, sd, ret);
        return FALSE;
    }

    *result = info.channelState;

    return TRUE;
}

NvBool nvEvo1IsChannelIdle(NVDevEvoPtr pDevEvo,
                           NVEvoChannelPtr pChan,
                           NvU32 sd,
                           NvBool *result)
{
    NvU32 channelState;

    if (!GetChannelState(pDevEvo, pChan, sd, &channelState)) {
        return FALSE;
    }

    *result = (channelState == NV5070_CTRL_GET_CHANNEL_INFO_STATE_IDLE);

    return TRUE;
}

/*
 * Result is false if an EVO channel is either one of NO_METHOD_PENDING or
 * UNCONNECTED, true o.w.
 *
 * NO_METHOD_PENDING is a mask for EMPTY | WRTIDLE | IDLE.
 *
 * If NVKMS hasn't grabbed the channel, it can be seen as UNCONNECTED.
 */
NvBool nvEvo1IsChannelMethodPending(NVDevEvoPtr pDevEvo,
                                    NVEvoChannelPtr pChan,
                                    NvU32 sd,
                                    NvBool *result)
{
    NvU32 channelState;

    if (!GetChannelState(pDevEvo, pChan, sd, &channelState)) {
        return FALSE;
    }

    *result = !(channelState &
              (NV5070_CTRL_GET_CHANNEL_INFO_STATE_NO_METHOD_PENDING |
               NV5070_CTRL_GET_CHANNEL_INFO_STATE_UNCONNECTED));

    return TRUE;
}

void nvEvo1SetDscParams(const NVDispEvoRec *pDispEvo,
                        const NvU32 head,
                        const NVDscInfoEvoRec *pDscInfo,
                        const enum nvKmsPixelDepth pixelDepth)
{
    nvAssert(pDscInfo->type == NV_DSC_INFO_EVO_TYPE_DISABLED);
}
