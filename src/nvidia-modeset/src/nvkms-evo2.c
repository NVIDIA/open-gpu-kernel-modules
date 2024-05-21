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
 * 2.x.
 */

#include "nvkms-dma.h"
#include "nvkms-types.h"
#include "nvkms-rmapi.h"
#include "nvkms-surface.h"

#include "nvkms-evo.h"
#include "nvkms-evo1.h"
#include "nvkms-ctxdma.h"

#include <nvmisc.h>

#include <class/cl5070.h> // NV5070_NOTIFICATION_STATUS

#include <class/cl917c.h> // NV917C_BASE_CHANNEL_DMA
#include <class/cl917b.h> // GK104DispOverlayImmControlPio
#include <class/cl917e.h> // NV917E_OVERLAY_CHANNEL_DMA
#include <class/cl917cswspare.h> // NV917C_SET_SPARE_{PRE,POST}_UPDATE_TRAP

#include <class/cl917d.h>         // NV917D_CORE_CHANNEL_DMA
#include <class/cl917dcrcnotif.h> // NV917D_NOTIFIER_CRC
#include <class/cl927d.h>         // NV927D_CORE_CHANNEL_DMA
#include <class/cl977d.h>         // NV977D_CORE_CHANNEL_DMA
#include <class/cl947d.h>         // NV947D_CORE_CHANNEL_DMA
#include <class/cl907dswspare.h>

#include <ctrl/ctrl5070/ctrl5070chnc.h> // NV5070_CTRL_CMD_STOP_BASE_PARAMS

ct_assert(NV_EVO_LOCK_PIN_0 >
          NV917D_HEAD_SET_CONTROL_MASTER_LOCK_PIN_INTERNAL_SCAN_LOCK__SIZE_1);
ct_assert(NV_EVO_LOCK_PIN_0 >
          NV917D_HEAD_SET_CONTROL_MASTER_LOCK_PIN_INTERNAL_FLIP_LOCK__SIZE_1);

/** Number of CRCs supported by hardware on NV917D hardware (Comp and SF/SOR) */
#define NV_EVO2_NUM_CRC_FIELDS 2

/** Flags read from CRCNotifier on NV917D hardware (Comp, SF/SOR Ovf and count) */
#define NV_EVO2_NUM_CRC_FLAGS 3

#define NV_EVO2_SUPPORTED_DITHERING_MODES                               \
    ((1 << NV_KMS_DPY_ATTRIBUTE_REQUESTED_DITHERING_MODE_AUTO)        | \
     (1 << NV_KMS_DPY_ATTRIBUTE_REQUESTED_DITHERING_MODE_DYNAMIC_2X2) | \
     (1 << NV_KMS_DPY_ATTRIBUTE_REQUESTED_DITHERING_MODE_STATIC_2X2)  | \
     (1 << NV_KMS_DPY_ATTRIBUTE_REQUESTED_DITHERING_MODE_TEMPORAL))

#define NV_EVO2_SUPPORTED_CURSOR_COMP_BLEND_MODES      \
    ((1 << NVKMS_COMPOSITION_BLENDING_MODE_NON_PREMULT_ALPHA) | \
     (1 << NVKMS_COMPOSITION_BLENDING_MODE_PREMULT_ALPHA))

static void
EvoSetCursorImage(NVDevEvoPtr pDevEvo,
                  const int head,
                  const NVSurfaceEvoRec *pSurfaceEvo,
                  NVEvoUpdateState *updateState,
                  const struct NvKmsCompositionParams *pCursorCompParams);

static void
EvoPushSetLUTContextDmaMethodsForOneSd(NVDevEvoRec *pDevEvo,
                                       const NvU32 sd,
                                       const NvU32 head,
                                       const NvU32 ctxdma,
                                       NvBool enableBaseLut,
                                       const NvBool enableOutputLut,
                                       NVEvoUpdateState *updateState);
static void
EvoPushUpdateComposition(NVDevEvoPtr pDevEvo,
                         const int head,
                         const NVFlipChannelEvoHwState *pBaseHwState,
                         const NVFlipChannelEvoHwState *pOverlayHwState,
                         NVEvoUpdateState *updateState,
                         NvBool bypassComposition);

static void InitChannelCaps90(NVDevEvoPtr pDevEvo,
                              NVEvoChannelPtr pChannel)
{
    if ((pChannel->channelMask & NV_EVO_CHANNEL_MASK_OVERLAY_ALL) != 0) {
        static const NVEvoChannelCaps OverlayCaps = {
            /*
             * Overlay supports timestamp flips on class 9x7e, but error checks
             * that it doesn't exceed 61 bits.
             */
            .validTimeStampBits = 61,
            /* Overlay does not support tearing/immediate flips. */
            .tearingFlips = FALSE,
            .vrrTearingFlips = FALSE,
            /* Overlay does not support per-eye stereo flips. */
            .perEyeStereoFlips = FALSE,
        };

        pChannel->caps = OverlayCaps;
    }
    if ((pChannel->channelMask & NV_EVO_CHANNEL_MASK_BASE_ALL) != 0) {
        static const NVEvoChannelCaps BaseCaps = {
            /*
             * Base supports timestamp flips on class 9x7c, but error checks
             * that it doesn't exceed 61 bits.
             */
            .validTimeStampBits = 61,
            /* Base supports tearing/immediate flips. */
            .tearingFlips = TRUE,
            /* Base supports VRR tearing flips. */
            .vrrTearingFlips = TRUE,
            /* Base supports per-eye stereo flips. */
            .perEyeStereoFlips = TRUE,
        };

        pChannel->caps = BaseCaps;
    }
}

static void EvoInitChannel90(NVDevEvoPtr pDevEvo, NVEvoChannelPtr pChannel)
{
    const NvBool isCore =
            FLD_TEST_DRF64(_EVO, _CHANNEL_MASK, _CORE, _ENABLE,
                           pChannel->channelMask);

    InitChannelCaps90(pDevEvo, pChannel);

    /* Set up core channel state. */
    if (isCore) {
        NvU32 head;

        for (head = 0; head < pDevEvo->numHeads; head++) {
            nvDmaSetStartEvoMethod(pChannel, NV917D_HEAD_SET_DEFAULT_BASE_COLOR(head), 1);
            nvDmaSetEvoMethodData(pChannel,
                DRF_NUM(917D, _HEAD_SET_DEFAULT_BASE_COLOR, _RED,   0) |
                DRF_NUM(917D, _HEAD_SET_DEFAULT_BASE_COLOR, _GREEN, 0) |
                DRF_NUM(917D, _HEAD_SET_DEFAULT_BASE_COLOR, _BLUE,  0));
        }
    }

    /* Set up base channel state. */
    if ((pChannel->channelMask & NV_EVO_CHANNEL_MASK_BASE_ALL) != 0) {
        NvU32 head = NV_EVO_CHANNEL_MASK_BASE_HEAD_NUMBER(pChannel->channelMask);
        NvU32 sd;

        for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
            pDevEvo->pSubDevices[sd]->baseComp[head].initialized = FALSE;
        }

        // For now we only support USE_CORE_LUT mode, but sending this method every
        // flip causes an error check to fire for tearing flips even if the LUT mode
        // isn't changing.  So instead, program it here.  ApplyBaseFlipOverrides()
        // will force the first flip to be non-tearing.
        nvDmaSetStartEvoMethod(pChannel, NV917C_SET_BASE_LUT_LO, 1);
        nvDmaSetEvoMethodData(pChannel, DRF_DEF(917C, _SET_BASE_LUT_LO, _ENABLE,
                              _USE_CORE_LUT));
        nvDmaSetStartEvoMethod(pChannel, NV917C_SET_OUTPUT_LUT_LO, 1);
        nvDmaSetEvoMethodData(pChannel, DRF_DEF(917C, _SET_OUTPUT_LUT_LO, _ENABLE,
                              _USE_CORE_LUT));
    }

    if ((pChannel->channelMask & NV_EVO_CHANNEL_MASK_OVERLAY_ALL) != 0) {
        NvU32 head = NV_EVO_CHANNEL_MASK_OVERLAY_HEAD_NUMBER(pChannel->channelMask);
        NvU32 sd;

        for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
            pDevEvo->pSubDevices[sd]->overlayComp[head].initialized = FALSE;
        }
    }
}

static void EvoInitWindowMapping90(const NVDispEvoRec *pDispEvo,
                                   NVEvoModesetUpdateState *pModesetUpdateState)
{
    /* Fixed window mapping on EVO 2 -- nothing to do. */
}

/*
 * These values are the same between all overlay
 * (7E_SURFACE_SET_PARAMS_FORMAT_) EVO classes.
 *
 * Return 0 in the case of an unrecognized NvKmsSurfaceMemoryFormat.
 */
static NvU32 EvoOverlayFormatFromKmsFormat91(enum NvKmsSurfaceMemoryFormat format)
{
    switch (format) {
        case NvKmsSurfaceMemoryFormatI8:
            return 0;
        case NvKmsSurfaceMemoryFormatR5G6B5:
            return 0;
        case NvKmsSurfaceMemoryFormatA1R5G5B5:
        case NvKmsSurfaceMemoryFormatX1R5G5B5:
            return NV917E_SURFACE_SET_PARAMS_FORMAT_A1R5G5B5;
        case NvKmsSurfaceMemoryFormatA8R8G8B8:
        case NvKmsSurfaceMemoryFormatX8R8G8B8:
            return NV917E_SURFACE_SET_PARAMS_FORMAT_A8R8G8B8;
        case NvKmsSurfaceMemoryFormatA2B10G10R10:
        case NvKmsSurfaceMemoryFormatX2B10G10R10:
            return NV917E_SURFACE_SET_PARAMS_FORMAT_A2B10G10R10;
        case NvKmsSurfaceMemoryFormatRF16GF16BF16AF16:
        case NvKmsSurfaceMemoryFormatRF16GF16BF16XF16:
            return NV917E_SURFACE_SET_PARAMS_FORMAT_RF16_GF16_BF16_AF16;
        case NvKmsSurfaceMemoryFormatR16G16B16A16:
            return NV917E_SURFACE_SET_PARAMS_FORMAT_R16_G16_B16_A16;
        case NvKmsSurfaceMemoryFormatA8B8G8R8:
        case NvKmsSurfaceMemoryFormatX8B8G8R8:
        case NvKmsSurfaceMemoryFormatRF32GF32BF32AF32:
        case NvKmsSurfaceMemoryFormatY8_U8__Y8_V8_N422:
        case NvKmsSurfaceMemoryFormatU8_Y8__V8_Y8_N422:
        case NvKmsSurfaceMemoryFormatY8___U8V8_N444:
        case NvKmsSurfaceMemoryFormatY8___V8U8_N444:
        case NvKmsSurfaceMemoryFormatY8___U8V8_N422:
        case NvKmsSurfaceMemoryFormatY8___V8U8_N422:
        case NvKmsSurfaceMemoryFormatY8___U8V8_N420:
        case NvKmsSurfaceMemoryFormatY8___V8U8_N420:
        case NvKmsSurfaceMemoryFormatY10___U10V10_N444:
        case NvKmsSurfaceMemoryFormatY10___V10U10_N444:
        case NvKmsSurfaceMemoryFormatY10___U10V10_N422:
        case NvKmsSurfaceMemoryFormatY10___V10U10_N422:
        case NvKmsSurfaceMemoryFormatY10___U10V10_N420:
        case NvKmsSurfaceMemoryFormatY10___V10U10_N420:
        case NvKmsSurfaceMemoryFormatY12___U12V12_N444:
        case NvKmsSurfaceMemoryFormatY12___V12U12_N444:
        case NvKmsSurfaceMemoryFormatY12___U12V12_N422:
        case NvKmsSurfaceMemoryFormatY12___V12U12_N422:
        case NvKmsSurfaceMemoryFormatY12___U12V12_N420:
        case NvKmsSurfaceMemoryFormatY12___V12U12_N420:
        case NvKmsSurfaceMemoryFormatY8___U8___V8_N444:
        case NvKmsSurfaceMemoryFormatY8___U8___V8_N420:
            return 0;
    }

    return 0;
}

static void EvoSetRasterParams90(NVDevEvoPtr pDevEvo, int head,
                                 const NVHwModeTimingsEvo *pTimings,
                                 const NVEvoColorRec *pOverscanColor,
                                 NVEvoUpdateState *updateState)
{
    NVEvoChannelPtr pChannel = pDevEvo->core;
    NvU32 overscanColor =
        DRF_NUM(917D, _HEAD_SET_OVERSCAN_COLOR, _RED, pOverscanColor->red) |
        DRF_NUM(917D, _HEAD_SET_OVERSCAN_COLOR, _GRN, pOverscanColor->green) |
        DRF_NUM(917D, _HEAD_SET_OVERSCAN_COLOR, _BLU, pOverscanColor->blue);

    /* These methods should only apply to a single pDpy */
    nvAssert(pDevEvo->subDevMaskStackDepth > 0);

    nvUpdateUpdateState(pDevEvo, updateState, pChannel);

    // XXX[AGP]: These methods are sequential and could use an incrementing
    // method, but it's not clear if there's a bug in EVO that causes corruption
    // sometimes.  Play it safe and send methods with count=1.

    nvDmaSetStartEvoMethod(pChannel, NV917D_HEAD_SET_OVERSCAN_COLOR(head), 1);
    nvDmaSetEvoMethodData(pChannel, overscanColor);

    nvDmaSetStartEvoMethod(pChannel, NV917D_HEAD_SET_RASTER_SIZE(head), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(917D, _HEAD_SET_RASTER_SIZE, _WIDTH, pTimings->rasterSize.x) |
        DRF_NUM(917D, _HEAD_SET_RASTER_SIZE, _HEIGHT, pTimings->rasterSize.y));

    nvDmaSetStartEvoMethod(pChannel, NV917D_HEAD_SET_RASTER_SYNC_END(head), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(917D, _HEAD_SET_RASTER_SYNC_END, _X, pTimings->rasterSyncEnd.x) |
        DRF_NUM(917D, _HEAD_SET_RASTER_SYNC_END, _Y, pTimings->rasterSyncEnd.y));

    nvDmaSetStartEvoMethod(pChannel, NV917D_HEAD_SET_RASTER_BLANK_END(head), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(917D, _HEAD_SET_RASTER_BLANK_END, _X, pTimings->rasterBlankEnd.x) |
        DRF_NUM(917D, _HEAD_SET_RASTER_BLANK_END, _Y, pTimings->rasterBlankEnd.y));

    nvDmaSetStartEvoMethod(pChannel, NV917D_HEAD_SET_RASTER_BLANK_START(head), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(917D, _HEAD_SET_RASTER_BLANK_START, _X, pTimings->rasterBlankStart.x) |
        DRF_NUM(917D, _HEAD_SET_RASTER_BLANK_START, _Y, pTimings->rasterBlankStart.y));

    if (pTimings->interlaced) {
        nvDmaSetStartEvoMethod(pChannel, NV917D_HEAD_SET_RASTER_VERT_BLANK2(head), 1);
        nvDmaSetEvoMethodData(pChannel,
                DRF_NUM(917D, _HEAD_SET_RASTER_VERT_BLANK2, _YSTART,
                        pTimings->rasterVertBlank2Start) |
                DRF_NUM(917D, _HEAD_SET_RASTER_VERT_BLANK2, _YEND,
                        pTimings->rasterVertBlank2End));
    }

    nvDmaSetStartEvoMethod(pChannel, NV917D_HEAD_SET_PIXEL_CLOCK_FREQUENCY(head), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(917D, _HEAD_SET_PIXEL_CLOCK_FREQUENCY, _HERTZ,
                pTimings->pixelClock * 1000) |
        DRF_DEF(917D, _HEAD_SET_PIXEL_CLOCK_FREQUENCY, _ADJ1000DIV1001,_FALSE));

    nvDmaSetStartEvoMethod(pChannel, NV917D_HEAD_SET_PIXEL_CLOCK_CONFIGURATION(head), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_DEF(917D, _HEAD_SET_PIXEL_CLOCK_CONFIGURATION, _MODE, _CLK_CUSTOM) |
        DRF_DEF(917D, _HEAD_SET_PIXEL_CLOCK_CONFIGURATION, _NOT_DRIVER, _FALSE) |
        DRF_DEF(917D, _HEAD_SET_PIXEL_CLOCK_CONFIGURATION, _ENABLE_HOPPING, _FALSE) |
        DRF_DEF(917D, _HEAD_SET_PIXEL_CLOCK_CONFIGURATION, _HOPPING_MODE, _VBLANK));

    nvDmaSetStartEvoMethod(pChannel, NV917D_HEAD_SET_PIXEL_CLOCK_FREQUENCY_MAX(head), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(917D, _HEAD_SET_PIXEL_CLOCK_FREQUENCY_MAX, _HERTZ,
                pTimings->pixelClock * 1000) |
        DRF_DEF(917D, _HEAD_SET_PIXEL_CLOCK_FREQUENCY_MAX, _ADJ1000DIV1001,_FALSE));
}

/*
 * Wrapper for EvoSetRasterParams90 which additionally sends the HDMI 3D
 * control methods.
 */
static void EvoSetRasterParams91(NVDevEvoPtr pDevEvo, int head,
                                 const NVHwModeTimingsEvo *pTimings,
                                 const NvU8 tilePosition,
                                 const NVDscInfoEvoRec *pDscInfo,
                                 const NVEvoColorRec *pOverscanColor,
                                 NVEvoUpdateState *updateState)
{
    NVEvoChannelPtr pChannel = pDevEvo->core;
    NvU32 hdmiStereoCtrl =
        DRF_DEF(917D, _HEAD_SET_HDMI_CTRL, _STEREO3D_STRUCTURE, _FRAME_PACKED) |
        DRF_NUM(917D, _HEAD_SET_HDMI_CTRL, _HDMI_VIC, 0);

    nvAssert(tilePosition == 0);
    nvAssert(pDscInfo->type == NV_DSC_INFO_EVO_TYPE_DISABLED);

    EvoSetRasterParams90(pDevEvo, head,
                         pTimings,
                         pOverscanColor, updateState);

    if (pTimings->hdmi3D) {
        hdmiStereoCtrl |=
            DRF_DEF(917D, _HEAD_SET_HDMI_CTRL, _VIDEO_FORMAT, _STEREO3D);
    } else {
        hdmiStereoCtrl |=
            DRF_DEF(917D, _HEAD_SET_HDMI_CTRL, _VIDEO_FORMAT, _NORMAL);
    }

    nvDmaSetStartEvoMethod(pChannel,
        NV917D_HEAD_SET_VACTIVE_SPACE_COLOR(head), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(917D, _HEAD_SET_VACTIVE_SPACE_COLOR, _RED_CR, 0) |
#if defined(DEBUG)
        DRF_NUM(917D, _HEAD_SET_VACTIVE_SPACE_COLOR, _GRN_Y,  512) |
#else
        DRF_NUM(917D, _HEAD_SET_VACTIVE_SPACE_COLOR, _GRN_Y,  0) |
#endif
        DRF_NUM(917D, _HEAD_SET_VACTIVE_SPACE_COLOR, _BLU_CB, 0));

    nvDmaSetStartEvoMethod(pChannel,
        NV917D_HEAD_SET_HDMI_CTRL(head), 1);
    nvDmaSetEvoMethodData(pChannel, hdmiStereoCtrl);
}

static void EvoSetProcAmp90(NVDispEvoPtr pDispEvo, const NvU32 head,
                            NVEvoUpdateState *updateState)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NVEvoChannelPtr pChannel = pDevEvo->core;
    const NVDispHeadStateEvoRec *pHeadState = &pDispEvo->headState[head];
    NvU32 dynRange;

    /* These methods should only apply to a single pDpyEvo */
    nvAssert(pDevEvo->subDevMaskStackDepth > 0);

    nvUpdateUpdateState(pDevEvo, updateState, pChannel);

    // These NVT defines match the HEAD_SET_PROCAMP ones.
    ct_assert(NVT_COLORIMETRY_RGB == NV917D_HEAD_SET_PROCAMP_COLOR_SPACE_RGB);
    ct_assert(NVT_COLORIMETRY_YUV_601 == NV917D_HEAD_SET_PROCAMP_COLOR_SPACE_YUV_601);
    ct_assert(NVT_COLORIMETRY_YUV_709 == NV917D_HEAD_SET_PROCAMP_COLOR_SPACE_YUV_709);
    ct_assert(NVT_COLOR_RANGE_FULL == NV917D_HEAD_SET_PROCAMP_RANGE_COMPRESSION_DISABLE);
    ct_assert(NVT_COLOR_RANGE_LIMITED == NV917D_HEAD_SET_PROCAMP_RANGE_COMPRESSION_ENABLE);

    if (pHeadState->procAmp.colorRange == NVT_COLOR_RANGE_FULL) {
        dynRange = DRF_DEF(917D, _HEAD_SET_PROCAMP, _DYNAMIC_RANGE, _VESA);
    } else {
        nvAssert(pHeadState->procAmp.colorRange == NVT_COLOR_RANGE_LIMITED);
        dynRange = DRF_DEF(917D, _HEAD_SET_PROCAMP, _DYNAMIC_RANGE, _CEA);
    }

    nvDmaSetStartEvoMethod(pChannel, NV917D_HEAD_SET_PROCAMP(head), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(917D, _HEAD_SET_PROCAMP, _COLOR_SPACE,
                pHeadState->procAmp.colorimetry) |
        DRF_DEF(917D, _HEAD_SET_PROCAMP, _CHROMA_LPF, _AUTO) |
        DRF_NUM(917D, _HEAD_SET_PROCAMP, _SAT_COS,
                pHeadState->procAmp.satCos) |
        DRF_NUM(917D, _HEAD_SET_PROCAMP, _SAT_SINE, 0) |
        dynRange |
        DRF_NUM(917D, _HEAD_SET_PROCAMP, _RANGE_COMPRESSION,
                pHeadState->procAmp.colorRange));
}

static void EvoSetHeadControl90(NVDevEvoPtr pDevEvo, int sd, int head,
                                NVEvoUpdateState *updateState)
{
    NVEvoChannelPtr pChannel = pDevEvo->core;
    NVEvoSubDevPtr pEvoSubDev = &pDevEvo->gpus[sd];
    /*
     * NOTE: This function should only push state to the hardware based on data
     * in the pHC.  If not, then we may miss updates due to the memcmp of the
     * HeadControl structure in UpdateEvoLockState().
     */
    NVEvoHeadControlPtr pHC = &pEvoSubDev->headControl[head];
    NvU32 data = 0, pin;
    NvU32 serverLockMode, clientLockMode;

    /* These methods should only apply to a single subdevice */
    nvAssert(pDevEvo->subDevMaskStackDepth > 0);

    nvUpdateUpdateState(pDevEvo, updateState, pChannel);

    switch (pHC->serverLock) {
    case NV_EVO_NO_LOCK:
        serverLockMode = NV917D_HEAD_SET_CONTROL_MASTER_LOCK_MODE_NO_LOCK;
        break;
    case NV_EVO_FRAME_LOCK:
        serverLockMode = NV917D_HEAD_SET_CONTROL_MASTER_LOCK_MODE_FRAME_LOCK;
        break;
    case NV_EVO_RASTER_LOCK:
        serverLockMode = NV917D_HEAD_SET_CONTROL_MASTER_LOCK_MODE_RASTER_LOCK;
        break;
    default:
        nvAssert(!"Invalid server lock mode");
        return;
    }

    switch (pHC->clientLock) {
    case NV_EVO_NO_LOCK:
        clientLockMode = NV917D_HEAD_SET_CONTROL_SLAVE_LOCK_MODE_NO_LOCK;
        break;
    case NV_EVO_FRAME_LOCK:
        clientLockMode = NV917D_HEAD_SET_CONTROL_SLAVE_LOCK_MODE_FRAME_LOCK;
        break;
    case NV_EVO_RASTER_LOCK:
        clientLockMode = NV917D_HEAD_SET_CONTROL_SLAVE_LOCK_MODE_RASTER_LOCK;
        break;
    default:
        nvAssert(!"Invalid client lock mode");
        return;
    }

    // Convert head control state to EVO method values.
    if (pHC->interlaced) {
        data |= DRF_DEF(917D, _HEAD_SET_CONTROL, _STRUCTURE, _INTERLACED);
    } else {
        data |= DRF_DEF(917D, _HEAD_SET_CONTROL, _STRUCTURE, _PROGRESSIVE);
    }

    nvAssert(pHC->serverLockPin != NV_EVO_LOCK_PIN_ERROR);
    nvAssert(pHC->clientLockPin != NV_EVO_LOCK_PIN_ERROR);
    nvAssert(pHC->flipLockPin != NV_EVO_LOCK_PIN_ERROR);

    if (NV_EVO_LOCK_PIN_IS_INTERNAL(pHC->serverLockPin)) {
        pin = pHC->serverLockPin - NV_EVO_LOCK_PIN_INTERNAL_0;
        /*
         * dispClass_02.mfs says:
         * "master lock pin, if internal, must be set to the corresponding
         * internal pin for that head" (error check #12)
         * (Note that this is only enforced when scanlock master is enabled)
         */
        nvAssert(pHC->serverLock == NV_EVO_NO_LOCK || pin == head);
        data |= DRF_NUM(917D, _HEAD_SET_CONTROL, _MASTER_LOCK_PIN,
                        NV917D_HEAD_SET_CONTROL_MASTER_LOCK_PIN_INTERNAL_SCAN_LOCK(pin));
    } else {
        pin = pHC->serverLockPin - NV_EVO_LOCK_PIN_0;
        data |= DRF_NUM(917D, _HEAD_SET_CONTROL, _MASTER_LOCK_PIN,
                        NV917D_HEAD_SET_CONTROL_MASTER_LOCK_PIN_LOCK_PIN(pin));
    }
    data |= DRF_NUM(917D, _HEAD_SET_CONTROL, _MASTER_LOCK_MODE, serverLockMode);

    if (clientLockMode == NV917D_HEAD_SET_CONTROL_SLAVE_LOCK_MODE_NO_LOCK) {
        data |= DRF_NUM(917D, _HEAD_SET_CONTROL, _SLAVE_LOCK_PIN,
                        NV917D_HEAD_SET_CONTROL_SLAVE_LOCK_PIN_UNSPECIFIED);
    } else if (NV_EVO_LOCK_PIN_IS_INTERNAL(pHC->clientLockPin)) {
        pin = pHC->clientLockPin - NV_EVO_LOCK_PIN_INTERNAL_0;
        data |= DRF_NUM(917D, _HEAD_SET_CONTROL, _SLAVE_LOCK_PIN,
                        NV917D_HEAD_SET_CONTROL_SLAVE_LOCK_PIN_INTERNAL_SCAN_LOCK(pin));
    } else {
        pin = pHC->clientLockPin - NV_EVO_LOCK_PIN_0;
        data |= DRF_NUM(917D, _HEAD_SET_CONTROL, _SLAVE_LOCK_PIN,
                        NV917D_HEAD_SET_CONTROL_SLAVE_LOCK_PIN_LOCK_PIN(pin));
    }
    data |= DRF_NUM(917D, _HEAD_SET_CONTROL, _SLAVE_LOCK_MODE, clientLockMode);
    data |= DRF_NUM(917D, _HEAD_SET_CONTROL, _SLAVE_LOCKOUT_WINDOW,
                    pHC->clientLockoutWindow);

    /*
     * Interlaced with stereo lock mode is not supported.
     *
     * We always enable stereo lock when it's available and either framelock
     * or rasterlock is in use.
     */
    if (pHC->stereoLocked) {
        nvAssert(!pHC->interlaced);

        if (pHC->serverLock != NV_EVO_NO_LOCK) {
            data |= DRF_NUM(927D, _HEAD_SET_CONTROL, _MASTER_STEREO_LOCK_MODE,
                            NV927D_HEAD_SET_CONTROL_MASTER_STEREO_LOCK_MODE_ENABLE);
        }
        if (pHC->clientLock != NV_EVO_NO_LOCK) {
            data |= DRF_NUM(927D, _HEAD_SET_CONTROL, _SLAVE_STEREO_LOCK_MODE,
                            NV927D_HEAD_SET_CONTROL_SLAVE_STEREO_LOCK_MODE_ENABLE);
        }
    }

    /*
     * Changing the flip lock pin induces a head shutdown.  We want to avoid
     * this in two cases:
     *
     * 1) When transitioning from the UEFI console, the flip lock pin is
     *    currently set to UNSPECIFIED, so we want to preserve that setting
     *    if possible to avoid an unnecessary flicker.
     *
     * 2) While framelock is enabled, we need to avoid head shutdown when
     *    transitioning to and from fliplock to guarantee no loss of stereo
     *    sync.
     *
     * To guarantee stereo sync while also avoiding unnecessary flicker when
     * transitioning from UEFI, we'll set the flip lock pin to UNSPECIFIED
     * unless fliplock, frame lock, or raster lock are enabled.  Enabling
     * framelock may induce one head shutdown when transitioning away from
     * UNSPECIFIED, but then enabling/disabling fliplock after that will
     * have no effect on the fliplock pin.
     */
    if (!pHC->flipLock &&
        (pHC->serverLock == NV_EVO_NO_LOCK) &&
        (pHC->clientLock == NV_EVO_NO_LOCK)) {

        data |= DRF_NUM(917D, _HEAD_SET_CONTROL, _FLIP_LOCK_PIN,
                        NV917D_HEAD_SET_CONTROL_FLIP_LOCK_PIN_UNSPECIFIED);
    } else if (NV_EVO_LOCK_PIN_IS_INTERNAL(pHC->flipLockPin)) {
        pin = pHC->flipLockPin - NV_EVO_LOCK_PIN_INTERNAL_0;
        data |= DRF_NUM(917D, _HEAD_SET_CONTROL, _FLIP_LOCK_PIN,
                        NV917D_HEAD_SET_CONTROL_FLIP_LOCK_PIN_INTERNAL_FLIP_LOCK(pin));
    } else {
        pin = pHC->flipLockPin - NV_EVO_LOCK_PIN_0;
        data |= DRF_NUM(917D, _HEAD_SET_CONTROL, _FLIP_LOCK_PIN,
                        NV917D_HEAD_SET_CONTROL_FLIP_LOCK_PIN_LOCK_PIN(pin));
    }
    if (pHC->flipLock) {
        data |= DRF_DEF(917D, _HEAD_SET_CONTROL, _FLIP_LOCK, _ENABLE);
    }

    nvAssert(pHC->stereoPin != NV_EVO_LOCK_PIN_ERROR);
    if (NV_EVO_LOCK_PIN_IS_INTERNAL(pHC->stereoPin)) {
        /*
         * dispClass_02.mfs says:
         * "stereo pin, if internal, must be set to the corresponding internal
         * pin for that head" (error check #14)
         * So just ignore which pin we selected; no sense in wasting cycles
         * keeping track of it
         */
        data |= DRF_NUM(917D, _HEAD_SET_CONTROL, _STEREO_PIN,
                        NV917D_HEAD_SET_CONTROL_STEREO_PIN_INTERNAL_SCAN_LOCK(head));
    } else {
        pin = pHC->stereoPin - NV_EVO_LOCK_PIN_0;
        data |= DRF_NUM(917D, _HEAD_SET_CONTROL, _STEREO_PIN,
                        NV917D_HEAD_SET_CONTROL_STEREO_PIN_LOCK_PIN(pin));
    }

    // Send the method.
    nvDmaSetStartEvoMethod(pChannel, NV917D_HEAD_SET_CONTROL(head), 1);
    nvDmaSetEvoMethodData(pChannel, data);

    nvDmaSetStartEvoMethod(pChannel, NV917D_HEAD_SET_LOCK_CHAIN(head), 1);
    nvDmaSetEvoMethodData(pChannel, DRF_NUM(917D, _HEAD_SET_LOCK_CHAIN, _POSITION,
                                  pHC->lockChainPosition));
}

static void EvoSetHeadRefClk90(NVDevEvoPtr pDevEvo, int head, NvBool external,
                               NVEvoUpdateState *updateState)
{
    NVEvoChannelPtr pChannel = pDevEvo->core;

    /* These methods should only apply to a single subdevice */
    nvAssert(pDevEvo->subDevMaskStackDepth > 0);

    nvUpdateUpdateState(pDevEvo, updateState, pChannel);

    nvDmaSetStartEvoMethod(pChannel, NV917D_HEAD_SET_SW_SPARE_A(head), 1);
    nvDmaSetEvoMethodData(pChannel, external ?
                DRF_DEF(907D, _HEAD_SET_SW_SPARE_A_CODE, _VPLL_REF, _GSYNC) :
                DRF_DEF(907D, _HEAD_SET_SW_SPARE_A_CODE, _VPLL_REF, _NO_PREF));
}

static void EvoDACSetControl90(const NVConnectorEvoRec *pConnectorEvo,
                               const enum nvKmsTimingsProtocol protocol,
                               const NvU32 orIndex,
                               const NvU32 headMask)
{
    NVDevEvoPtr pDevEvo = pConnectorEvo->pDispEvo->pDevEvo;
    NVEvoChannelPtr pChannel = pDevEvo->core;

    /* These methods should only apply to a single pDpy */
    nvAssert(pDevEvo->subDevMaskStackDepth > 0);

    if (headMask != 0) {
        nvAssert(protocol == NVKMS_PROTOCOL_DAC_RGB);
    }

    nvDmaSetStartEvoMethod(pChannel, NV917D_DAC_SET_CONTROL(orIndex), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(917D, _DAC_SET_CONTROL, _OWNER_MASK, headMask) |
        DRF_DEF(917D, _DAC_SET_CONTROL, _PROTOCOL, _RGB_CRT));
}

static void EvoSORSetControl90(const NVConnectorEvoRec *pConnectorEvo,
                               const enum nvKmsTimingsProtocol protocol,
                               const NvU32 orIndex,
                               const NvU32 headMask)
{
    NVDevEvoPtr pDevEvo = pConnectorEvo->pDispEvo->pDevEvo;
    NVEvoChannelPtr pChannel = pDevEvo->core;
    NvU32 hwProtocol = 0;

    /* These methods should only apply to a single pDpy */
    nvAssert(pDevEvo->subDevMaskStackDepth > 0);
    nvAssert(orIndex != NV_INVALID_OR);

    if (headMask != 0) {
        switch (protocol) {
        default:
            nvAssert(!"unexpected protocol");
            /* fallthrough */
        case NVKMS_PROTOCOL_SOR_LVDS_CUSTOM:
            hwProtocol = NV917D_SOR_SET_CONTROL_PROTOCOL_LVDS_CUSTOM;
            break;
        case NVKMS_PROTOCOL_SOR_SINGLE_TMDS_A:
            hwProtocol = NV917D_SOR_SET_CONTROL_PROTOCOL_SINGLE_TMDS_A;
            break;
        case NVKMS_PROTOCOL_SOR_SINGLE_TMDS_B:
            hwProtocol = NV917D_SOR_SET_CONTROL_PROTOCOL_SINGLE_TMDS_B;
            break;
        case NVKMS_PROTOCOL_SOR_DUAL_TMDS:
            hwProtocol = NV917D_SOR_SET_CONTROL_PROTOCOL_DUAL_TMDS;
            break;
        case NVKMS_PROTOCOL_SOR_DP_A:
            hwProtocol = NV917D_SOR_SET_CONTROL_PROTOCOL_DP_A;
            break;
        case NVKMS_PROTOCOL_SOR_DP_B:
            hwProtocol = NV917D_SOR_SET_CONTROL_PROTOCOL_DP_B;
            break;
        }
    }

    nvDmaSetStartEvoMethod(pChannel, NV917D_SOR_SET_CONTROL(orIndex), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(917D, _SOR_SET_CONTROL, _OWNER_MASK, headMask) |
        DRF_NUM(917D, _SOR_SET_CONTROL, _PROTOCOL, hwProtocol) |
        DRF_DEF(917D, _SOR_SET_CONTROL, _DE_SYNC_POLARITY, _POSITIVE_TRUE) |
        DRF_DEF(917D, _SOR_SET_CONTROL, _PIXEL_REPLICATE_MODE, _OFF));
}

static void EvoPIORSetControl90(const NVConnectorEvoRec *pConnectorEvo,
                                const enum nvKmsTimingsProtocol protocol,
                                const NvU32 orIndex,
                                const NvU32 headMask)
{
    NVDevEvoPtr pDevEvo = pConnectorEvo->pDispEvo->pDevEvo;
    NVEvoChannelPtr pChannel = pDevEvo->core;

    /* These methods should only apply to a single pDpy */
    nvAssert(pDevEvo->subDevMaskStackDepth > 0);

    if (headMask != 0) {
        nvAssert(protocol == NVKMS_PROTOCOL_PIOR_EXT_TMDS_ENC);
    }

    nvDmaSetStartEvoMethod(pChannel, NV917D_PIOR_SET_CONTROL(orIndex), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(917D, _PIOR_SET_CONTROL, _OWNER_MASK, headMask) |
        DRF_DEF(917D, _PIOR_SET_CONTROL, _PROTOCOL, _EXT_TMDS_ENC) |
        DRF_DEF(917D, _PIOR_SET_CONTROL, _DE_SYNC_POLARITY, _POSITIVE_TRUE));
}

static NvU32 EvoGetPixelDepth90(const enum nvKmsPixelDepth pixelDepth)
{
    switch (pixelDepth) {
    case NVKMS_PIXEL_DEPTH_18_444:
        return NV917D_HEAD_SET_CONTROL_OUTPUT_RESOURCE_PIXEL_DEPTH_BPP_18_444;
    case NVKMS_PIXEL_DEPTH_24_444:
        return NV917D_HEAD_SET_CONTROL_OUTPUT_RESOURCE_PIXEL_DEPTH_BPP_24_444;
    case NVKMS_PIXEL_DEPTH_30_444:
        return NV917D_HEAD_SET_CONTROL_OUTPUT_RESOURCE_PIXEL_DEPTH_BPP_30_444;
    case NVKMS_PIXEL_DEPTH_16_422:
        return NV917D_HEAD_SET_CONTROL_OUTPUT_RESOURCE_PIXEL_DEPTH_BPP_16_422;
    case NVKMS_PIXEL_DEPTH_20_422:
        return NV917D_HEAD_SET_CONTROL_OUTPUT_RESOURCE_PIXEL_DEPTH_BPP_20_422;
    }
    nvAssert(!"Unexpected pixel depth");
    return NV917D_HEAD_SET_CONTROL_OUTPUT_RESOURCE_PIXEL_DEPTH_BPP_24_444;
}

static void EvoHeadSetControlOR90(NVDevEvoPtr pDevEvo,
                                  const int head,
                                  const NVHwModeTimingsEvo *pTimings,
                                  const enum nvKmsPixelDepth pixelDepth,
                                  const NvBool colorSpaceOverride,
                                  NVEvoUpdateState *updateState)
{
    const NvU32 hwPixelDepth = EvoGetPixelDepth90(pixelDepth);
    const NvU16 colorSpaceFlag = nvEvo1GetColorSpaceFlag(pDevEvo,
                                                         colorSpaceOverride);
    NVEvoChannelPtr pChannel = pDevEvo->core;

    /* These methods should only apply to a single pDpy */
    nvAssert(pDevEvo->subDevMaskStackDepth > 0);

    nvUpdateUpdateState(pDevEvo, updateState, pChannel);

    nvDmaSetStartEvoMethod(pChannel, NV917D_HEAD_SET_CONTROL_OUTPUT_RESOURCE(head), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_DEF(917D, _HEAD_SET_CONTROL_OUTPUT_RESOURCE, _CRC_MODE, _ACTIVE_RASTER) |
        (pTimings->hSyncPol ?
            DRF_DEF(917D, _HEAD_SET_CONTROL_OUTPUT_RESOURCE, _HSYNC_POLARITY, _NEGATIVE_TRUE) :
            DRF_DEF(917D, _HEAD_SET_CONTROL_OUTPUT_RESOURCE, _HSYNC_POLARITY, _POSITIVE_TRUE)) |
        (pTimings->vSyncPol ?
            DRF_DEF(917D, _HEAD_SET_CONTROL_OUTPUT_RESOURCE, _VSYNC_POLARITY, _NEGATIVE_TRUE) :
            DRF_DEF(917D, _HEAD_SET_CONTROL_OUTPUT_RESOURCE, _VSYNC_POLARITY, _POSITIVE_TRUE)) |
        (colorSpaceOverride ?
            (DRF_DEF(977D, _HEAD_SET_CONTROL_OUTPUT_RESOURCE, _COLOR_SPACE_OVERRIDE, _ENABLE) |
             DRF_NUM(977D, _HEAD_SET_CONTROL_OUTPUT_RESOURCE, _COLOR_SPACE_FLAG, colorSpaceFlag)) :
            DRF_DEF(977D, _HEAD_SET_CONTROL_OUTPUT_RESOURCE, _COLOR_SPACE_OVERRIDE, _DISABLE)) |
         DRF_NUM(917D, _HEAD_SET_CONTROL_OUTPUT_RESOURCE, _PIXEL_DEPTH, hwPixelDepth));
}

static void EvoORSetControl90(NVDevEvoPtr pDevEvo,
                              const NVConnectorEvoRec *pConnectorEvo,
                              const enum nvKmsTimingsProtocol protocol,
                              const NvU32 orIndex,
                              const NvU32 headMask,
                              NVEvoUpdateState *updateState)
{
    /* These methods should only apply to a single pDpy */
    nvAssert(pDevEvo->subDevMaskStackDepth > 0);

    nvUpdateUpdateState(pDevEvo, updateState, pDevEvo->core);

    switch (pConnectorEvo->or.type) {
    case NV0073_CTRL_SPECIFIC_OR_TYPE_DAC:
        EvoDACSetControl90(pConnectorEvo, protocol, orIndex, headMask);
        break;
    case NV0073_CTRL_SPECIFIC_OR_TYPE_SOR:
        EvoSORSetControl90(pConnectorEvo, protocol, orIndex, headMask);
        break;
    case NV0073_CTRL_SPECIFIC_OR_TYPE_PIOR:
        EvoPIORSetControl90(pConnectorEvo, protocol, orIndex, headMask);
        break;
    default:
        nvAssert(!"Invalid pConnectorEvo->or.type");
        break;
    }
}

static void EvoHeadSetDisplayId90(NVDevEvoPtr pDevEvo,
                                  const NvU32 head, const NvU32 displayId,
                                  NVEvoUpdateState *updateState)
{
    NVEvoChannelPtr pChannel = pDevEvo->core;

    /* These methods should only apply to a single pDpy */
    nvAssert(pDevEvo->subDevMaskStackDepth > 0);

    nvUpdateUpdateState(pDevEvo, updateState, pChannel);

    nvDmaSetStartEvoMethod(pChannel, NV917D_HEAD_SET_DISPLAY_ID(head, 0), 1);
    nvDmaSetEvoMethodData(pChannel, displayId);
}

static NvBool EvoSetUsageBounds90(NVDevEvoPtr pDevEvo, NvU32 sd, NvU32 head,
                                  const struct NvKmsUsageBounds *pUsage,
                                  NVEvoUpdateState *updateState)
{
    NVEvoChannelPtr pChannel = pDevEvo->core;
    const NVEvoSubDevHeadStateRec *pCurrentFlipState =
        &pDevEvo->gpus[sd].headState[head];
    const struct NvKmsUsageBounds *pCurrentUsage =
        &pCurrentFlipState->usage;
    NvU64 overlayFormats = 0;
    NvU32 baseUsage = 0, overlayUsage = 0;
    const NVSurfaceEvoRec *pCurrentBaseSurf =
        pCurrentFlipState->layer[NVKMS_MAIN_LAYER].pSurfaceEvo[NVKMS_LEFT];
    const NVSurfaceEvoRec *pCurrentOverlaySurf =
        pCurrentFlipState->layer[NVKMS_OVERLAY_LAYER].pSurfaceEvo[NVKMS_LEFT];

    /* These methods should only apply to a single pDpy */
    nvAssert(pDevEvo->subDevMaskStackDepth > 0);

    if (UsageBoundsEqual(pCurrentUsage, pUsage)) {
        return FALSE;
    }

    nvUpdateUpdateState(pDevEvo, updateState, pChannel);

    /*
     * Make sure to interlock usage bounds update with the base and overlay
     * channel updates, otherwise it ends up violating hardware error check for
     * the base/overlay channel blocking.
     *
     * // check for blocking violations
     * for (vlt_index = 0; vlt_index < NV_CHIP_DISP_TOTAL_HEADS_PRESENT_por; vlt_index++) {
     *   if ((wir_BlockBase[vlt_index] == TRUE)
     *     && (wir_BaseQuiescent[vlt_index] == FALSE)
     *     && ((ecv_GlobalHeadConnected[vlt_index] == TRUE) || (pri_ErrcheckWhenDisconnected == TRUE)))
     *     throw (vlt_index << NV_DISP_CORE_STATE_ERROR_HEAD_INDEX_SHIFT) | NV_DISP_CORE_STATE_ERROR_001;
     * }
     *
     * for (vlt_index = 0; vlt_index < NV_CHIP_DISP_TOTAL_HEADS_PRESENT_por; vlt_index++) {
     *   if ((wir_BlockOverlay[vlt_index] == TRUE)
     *     && (wir_OverlayQuiescent[vlt_index] == FALSE)
     *     && ((ecv_GlobalHeadConnected[vlt_index] == TRUE) || (pri_ErrcheckWhenDisconnected == TRUE)))
     *     throw (vlt_index << NV_DISP_CORE_STATE_ERROR_HEAD_INDEX_SHIFT) | NV_DISP_CORE_STATE_ERROR_002;
     */

    if (pCurrentBaseSurf != NULL &&
        !nvEvoLayerUsageBoundsEqual(pUsage, pCurrentUsage, NVKMS_MAIN_LAYER)) {
        nvUpdateUpdateState(pDevEvo, updateState, pDevEvo->base[head]);
    }

    if (pCurrentOverlaySurf != NULL &&
        !nvEvoLayerUsageBoundsEqual(pUsage, pCurrentUsage, NVKMS_OVERLAY_LAYER)) {
        nvUpdateUpdateState(pDevEvo, updateState, pDevEvo->overlay[head]);
    }


    if (pUsage->layer[NVKMS_MAIN_LAYER].usable) {
        baseUsage |= DRF_DEF(917D, _HEAD_SET_BASE_CHANNEL_USAGE_BOUNDS, _USABLE,
                             _TRUE);

        if (pUsage->layer[NVKMS_MAIN_LAYER].supportedSurfaceMemoryFormats &
            NVKMS_SURFACE_MEMORY_FORMATS_RGB_PACKED8BPP) {
            baseUsage |= DRF_DEF(917D, _HEAD_SET_BASE_CHANNEL_USAGE_BOUNDS,
                                 _PIXEL_DEPTH, _BPP_64);
        } else if (pUsage->layer[NVKMS_MAIN_LAYER].supportedSurfaceMemoryFormats &
            NVKMS_SURFACE_MEMORY_FORMATS_RGB_PACKED4BPP) {
            baseUsage |= DRF_DEF(917D, _HEAD_SET_BASE_CHANNEL_USAGE_BOUNDS,
                                 _PIXEL_DEPTH, _BPP_32);
        } else if (pUsage->layer[NVKMS_MAIN_LAYER].supportedSurfaceMemoryFormats &
            NVKMS_SURFACE_MEMORY_FORMATS_RGB_PACKED2BPP) {
            baseUsage |= DRF_DEF(917D, _HEAD_SET_BASE_CHANNEL_USAGE_BOUNDS,
                                 _PIXEL_DEPTH, _BPP_16);
        } else if (pUsage->layer[NVKMS_MAIN_LAYER].supportedSurfaceMemoryFormats &
            NVKMS_SURFACE_MEMORY_FORMATS_RGB_PACKED1BPP) {
            baseUsage |= DRF_DEF(917D, _HEAD_SET_BASE_CHANNEL_USAGE_BOUNDS,
                                 _PIXEL_DEPTH, _BPP_8);
        } else {
            nvAssert(!"Unexpected base pixel depth");
            return FALSE;
        }

        baseUsage |= DRF_DEF(917D, _HEAD_SET_BASE_CHANNEL_USAGE_BOUNDS,
                             _SUPER_SAMPLE, _X1_AA);
    }

    overlayUsage |= pUsage->layer[NVKMS_OVERLAY_LAYER].usable ?
        DRF_DEF(917D, _HEAD_SET_OVERLAY_USAGE_BOUNDS, _USABLE, _TRUE) :
        DRF_DEF(917D, _HEAD_SET_OVERLAY_USAGE_BOUNDS, _USABLE, _FALSE);

    overlayFormats = pUsage->layer[NVKMS_OVERLAY_LAYER].usable ?
        pUsage->layer[NVKMS_OVERLAY_LAYER].supportedSurfaceMemoryFormats :
        NVKMS_SURFACE_MEMORY_FORMATS_RGB_PACKED2BPP;

    if (overlayFormats & NVKMS_SURFACE_MEMORY_FORMATS_RGB_PACKED4BPP) {
        overlayUsage |= DRF_DEF(917D, _HEAD_SET_OVERLAY_USAGE_BOUNDS,
                                _PIXEL_DEPTH, _BPP_32);
    } else if (overlayFormats & NVKMS_SURFACE_MEMORY_FORMATS_RGB_PACKED2BPP) {
        overlayUsage |= DRF_DEF(917D, _HEAD_SET_OVERLAY_USAGE_BOUNDS,
                                _PIXEL_DEPTH, _BPP_16);
    } else {
        nvAssert(!"Unsupported overlay depth");
        overlayUsage |= DRF_DEF(917D, _HEAD_SET_OVERLAY_USAGE_BOUNDS,
                                _PIXEL_DEPTH, _BPP_16);
    }

    nvDmaSetStartEvoMethod(pChannel,
            NV917D_HEAD_SET_BASE_CHANNEL_USAGE_BOUNDS(head), 2);
    nvDmaSetEvoMethodData(pChannel, baseUsage);
    nvDmaSetEvoMethodData(pChannel, overlayUsage);

    return TRUE;
}

static void EvoSetNotifierMethods90(NVDevEvoRec *pDevEvo,
                                    NVEvoChannelPtr pChannel,
                                    NvBool notify,
                                    NvBool awaken,
                                    NvU32 notifier)
{
    ASSERT_DRF_NUM(917D, _SET_NOTIFIER_CONTROL, _OFFSET, notifier);

    if (notify) {
        NvU32 sd;
        for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
            if (nvPeekEvoSubDevMask(pDevEvo) & (1 << sd)) {
                nvPushEvoSubDevMask(pDevEvo, NVBIT(sd));
                nvDmaSetStartEvoMethod(pChannel,
                    NV917D_SET_CONTEXT_DMA_NOTIFIER, 1);
                nvDmaSetEvoMethodData(pChannel,
                    DRF_NUM(917D,
                            _SET_CONTEXT_DMA_NOTIFIER,
                            _HANDLE,
                            pDevEvo->core->notifiersDma[sd].surfaceDesc.ctxDmaHandle));
                nvPopEvoSubDevMask(pDevEvo);
            }
        }
    } else {
        nvDmaSetStartEvoMethod(pChannel,
            NV917D_SET_CONTEXT_DMA_NOTIFIER, 1);
        nvDmaSetEvoMethodData(pChannel,
            DRF_NUM(917D, _SET_CONTEXT_DMA_NOTIFIER, _HANDLE, 0));
    }

    nvDmaSetStartEvoMethod(pChannel, NV917D_SET_NOTIFIER_CONTROL, 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(917D, _SET_NOTIFIER_CONTROL, _OFFSET, notifier) |
        (awaken ?
            DRF_DEF(917D, _SET_NOTIFIER_CONTROL, _MODE, _WRITE_AWAKEN) :
            DRF_DEF(917D, _SET_NOTIFIER_CONTROL, _MODE, _WRITE)) |
        (notify ?
            DRF_DEF(917D, _SET_NOTIFIER_CONTROL, _NOTIFY, _ENABLE) :
            DRF_DEF(917D, _SET_NOTIFIER_CONTROL, _NOTIFY, _DISABLE)));
}

static void UpdateCore9x(NVEvoChannelPtr pChannel,
                         NVEvoChannelMask interlockChannelMask)
{
    NvU32 head, value = 0;

    ct_assert(NV_EVO_CHANNEL_MASK_BASE__SIZE ==
              NV_EVO_CHANNEL_MASK_OVERLAY__SIZE);
    for (head = 0; head < NV_EVO_CHANNEL_MASK_BASE__SIZE; head++) {
        if (FLD_IDX_TEST_DRF64(_EVO, _CHANNEL_MASK, _BASE, head, _ENABLE,
                               interlockChannelMask)) {
            value |= DRF_IDX_DEF(917D, _UPDATE,
                                 _INTERLOCK_WITH_BASE, head, _ENABLE);
        }
        if (FLD_IDX_TEST_DRF64(_EVO, _CHANNEL_MASK, _OVERLAY, head, _ENABLE,
                               interlockChannelMask)) {
            value |= DRF_IDX_DEF(917D, _UPDATE,
                                 _INTERLOCK_WITH_OVERLAY, head, _ENABLE);
        }
    }

    nvDmaSetStartEvoMethod(pChannel, NV917D_UPDATE, 1);
    nvDmaSetEvoMethodData(pChannel, value);

    nvDmaKickoffEvo(pChannel);
}

static void UpdateBase91(NVEvoChannelPtr pChannel,
                         NvBool interlockWithCore,
                         NvBool vrrTearing)
{
    NvU32 updateValue = 0;
    NvU32 trapParam = 0;

    if (interlockWithCore) {
        updateValue |= DRF_DEF(917C, _UPDATE, _INTERLOCK_WITH_CORE, _ENABLE);
    }

    nvDmaSetStartEvoMethod(pChannel, NV917C_SET_SPARE_PRE_UPDATE_TRAP, 1);
    nvDmaSetEvoMethodData(pChannel, trapParam);

    nvDmaSetStartEvoMethod(pChannel, NV917C_UPDATE, 1);
    nvDmaSetEvoMethodData(pChannel, updateValue);

    nvDmaSetStartEvoMethod(pChannel, NV917C_SET_SPARE_POST_UPDATE_TRAP, 1);
    nvDmaSetEvoMethodData(pChannel, trapParam);

    nvDmaKickoffEvo(pChannel);
}

static void UpdateOverlay9x(NVEvoChannelPtr pChannel,
                            NvBool interlockWithCore)
{
    NvU32 value = 0;

    if (interlockWithCore) {
        value |= DRF_DEF(917E, _UPDATE, _INTERLOCK_WITH_CORE, _ENABLE);
    }

    nvDmaSetStartEvoMethod(pChannel, NV917E_UPDATE, 1);
    nvDmaSetEvoMethodData(pChannel, value);

    nvDmaKickoffEvo(pChannel);
}

static void EvoUpdate91(NVDevEvoPtr pDevEvo,
                        const NVEvoUpdateState *updateState,
                        NvBool releaseElv)
{
    NvU32 sd;
    NVEvoChannelMask fliplockedBaseChannels[NVKMS_MAX_SUBDEVICES] = { };
    NvBool updateAllFliplockedBaseChannels = FALSE;

    /*
     * Multiple 'base + core channel interlocked' updates can create deadlock
     * if heads are flip locked.
     *
     * For example - if head-0 and head-1 are flip locked and you initiate two
     * 'base + core channel interlocked' updates separately for each of
     * the head then that creates deadlock:
     *
     *
     *      +--------+      +--------+      +--------+
     *      | BASE-0 |      |  CORE  |      | BASE-1 |
     *      +--------+      +--------+      +--------+
     *      |        |      |        |      |        |
     *      |        |      |        |      |        |
     *      +--------+------+--------+      |        |
     *      |      INTERLOCKED       |      |        |
     *      |        UPDATE-0        |      |        |
     *      +--------+------+--------+      |        |
     *      | Base   |      | Core   |      |        |
     *  <...| update |<.... | Update |      |        |
     *  :   | for    |      | for    |      |        |
     *  :   | head-0 |      | head-0 |      |        |
     *  :   +--------+------+--------+      |        |
     *  :   |        |      |   ^    |      |        |
     *  :   |        |      |   :    |      |        |
     *  :   +--------+      |   :    |      |        |
     *  :                   |   :    |      |        |
     *  :                   +---(----+------+--------+
     *  :                   |   :  INTERLOCKED       |
     *  :                   |   :    UPDATE-1        |
     *  :                   +--------+------+--------+
     *  V                   | Core   |      | Base   |
     *  :                   | update |<.... | Update |
     *  :                   | for    |      | for    |<...
     *  :                   | head-1 |      | head-1 |   :
     *  :                   +--------+------+--------+   :
     *  :                   |        |      |        |   ^
     *  :                   +--------+      +--------+   :
     *  :                                                :
     *  V...................>............................>
     *
     *            ^
     *            |
     *            |
     *  [ BASE-0 and BASE-1 are fliplocked ]
     *
     * Here you can follow the dotted arrow line and see how deadlock
     * has been formed. The dotted arrow line indicates the execution
     * dependency of the one update onto another, e.g. the core update
     * for head-1 can't get executed unless the core update for head-0
     * gets executed.
     *
     * To prevent this deadlock, initiate the base channel updates for all flip
     * locked heads if update state contains 'base + core channel interlocked'
     * for the flip locked head.
     */
    for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
        NVEvoChannelMask updateChannelMask = updateState->subdev[sd].channelMask;
        NVEvoChannelMask interlockChannelMask =
            updateChannelMask & ~updateState->subdev[sd].noCoreInterlockMask;
        NvU32 head;

        for (head = 0; head < NV_EVO_CHANNEL_MASK_BASE__SIZE; head++) {
            NVEvoChannelMask thisMask =
                DRF_IDX_DEF64(_EVO, _CHANNEL_MASK, _BASE, head, _ENABLE);
            NVEvoSubDevPtr pEvoSubDev = &pDevEvo->gpus[sd];
            NVEvoHeadControlPtr pHC = &pEvoSubDev->headControl[head];

            if (pHC->flipLock) {
                fliplockedBaseChannels[sd] |= thisMask;
            }

            /*
             * If this update is updating only one base channel without any core
             * interlock, in that case, we don't need to also update all flip
             * locked base channels.
             */
            if (NV_EVO_CHANNEL_MASK_POPCOUNT(interlockChannelMask) <= 1 &&
                !FLD_TEST_DRF64(_EVO, _CHANNEL_MASK, _CORE, _ENABLE,
                                updateChannelMask)) {
                continue;
            }

            if ((updateChannelMask & thisMask) != 0x0 && pHC->flipLock) {
                updateAllFliplockedBaseChannels = TRUE;
            }
        }
    }

    for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
        NVEvoChannelMask updateChannelMask = updateState->subdev[sd].channelMask |
            (updateAllFliplockedBaseChannels ? fliplockedBaseChannels[sd] : 0x0);
        NVEvoChannelMask interlockChannelMask =
            updateChannelMask & ~updateState->subdev[sd].noCoreInterlockMask;
        NvBool interlockWithCore = FALSE;
        const NvU32 subDeviceMask = (1 << sd);
        NvU32 head;

        nvPushEvoSubDevMask(pDevEvo, subDeviceMask);

        if (NV_EVO_CHANNEL_MASK_POPCOUNT(interlockChannelMask) > 1) {
            /* We can only interlock updates if core is included. */
            nvAssert(!FLD_TEST_DRF64(_EVO, _CHANNEL_MASK, _CORE, _ENABLE,
                                     updateState->subdev[sd].noCoreInterlockMask));
            updateChannelMask |= DRF_DEF64(_EVO, _CHANNEL_MASK, _CORE, _ENABLE);
            interlockChannelMask |=
                DRF_DEF64(_EVO, _CHANNEL_MASK, _CORE, _ENABLE);
            interlockWithCore = TRUE;
        }

        if (FLD_TEST_DRF64(_EVO, _CHANNEL_MASK, _CORE, _ENABLE,
                           updateChannelMask)) {
            UpdateCore9x(pDevEvo->core, updateChannelMask);
        }

        for (head = 0; head < NV_EVO_CHANNEL_MASK_OVERLAY__SIZE; head++) {
            NVEvoChannelMask thisMask =
                DRF_IDX_DEF64(_EVO, _CHANNEL_MASK, _OVERLAY, head, _ENABLE);
            if (updateChannelMask & thisMask) {
                NvBool thisInterlockWithCore = interlockWithCore &&
                    (interlockChannelMask & thisMask);
                UpdateOverlay9x(pDevEvo->overlay[head],
                                thisInterlockWithCore);
            }
        }

        for (head = 0; head < NV_EVO_CHANNEL_MASK_BASE__SIZE; head++) {
            NVEvoChannelMask thisMask =
                DRF_IDX_DEF64(_EVO, _CHANNEL_MASK, _BASE, head, _ENABLE);
            if (updateChannelMask & thisMask) {
                NvBool thisInterlockWithCore = interlockWithCore &&
                    (interlockChannelMask & thisMask);
                NvBool vrrTearing =
                    updateState->subdev[sd].base[head].vrrTearing;

                UpdateBase91(pDevEvo->base[head],
                             thisInterlockWithCore, vrrTearing);
            }
        }

        nvPopEvoSubDevMask(pDevEvo);
    }
}

static void EvoSetNotifier90(NVDevEvoRec *pDevEvo,
                             const NvBool notify,
                             const NvBool awaken,
                             const NvU32 notifier,
                             NVEvoUpdateState *updateState)
{
    /* These methods should only apply to a single subdevice */
    nvAssert(pDevEvo->subDevMaskStackDepth > 0);

    nvUpdateUpdateState(pDevEvo, updateState, pDevEvo->core);

    EvoSetNotifierMethods90(pDevEvo, pDevEvo->core, notify, awaken, notifier);
}

/*
 * Returns the data for the SET_STORAGE method.  The method data
 * format is the same between classes 90[CDE].
 */
static NvU32 EvoComputeSetStorage90(const NVDevEvoRec *pDevEvo,
                                    const NVSurfaceEvoRec *pSurfaceEvo)
{
    NvU32 setStorage;

    NvU32 pitch = nvEvoGetHeadSetStoragePitchValue(
                    pDevEvo,
                    pSurfaceEvo->layout,
                    pSurfaceEvo->planes[0].pitch);
    nvAssert(pitch != 0);

    if (pSurfaceEvo->layout == NvKmsSurfaceMemoryLayoutBlockLinear) {
        // 1 block = 1 * X Gobs; 1 Gob = 64B * 4Lines; X = 1 <<
        // blockHeightLog2Gobs
        NvU32 blockHeight = pSurfaceEvo->log2GobsPerBlockY;

        setStorage = DRF_NUM(917D, _HEAD_SET_STORAGE, _BLOCK_HEIGHT, blockHeight) |
                     DRF_DEF(917D, _HEAD_SET_STORAGE, _MEMORY_LAYOUT, _BLOCKLINEAR);
    } else {
        setStorage = DRF_DEF(917D, _HEAD_SET_STORAGE, _MEMORY_LAYOUT, _PITCH);
    }

    ASSERT_DRF_NUM(917D, _HEAD_SET_STORAGE, _PITCH, pitch);
    setStorage |= DRF_NUM(917D, _HEAD_SET_STORAGE, _PITCH, pitch);

    return setStorage;
}

static void SetCscMatrix(NVEvoChannelPtr pChannel, NvU32 method,
                         const struct NvKmsCscMatrix *matrix,
                         NvU32 extraFirstWordBits)
{
    int y;

    // The _COEFF fields are the same across all of the methods on all
    // channels.
    ct_assert(DRF_SHIFTMASK(NV917C_SET_CSC_RED2RED_COEFF) ==
              DRF_SHIFTMASK(NV917D_HEAD_SET_CSC_RED2RED_COEFF));
    ct_assert(DRF_SHIFTMASK(NV917C_SET_CSC_RED2RED_COEFF) ==
              DRF_SHIFTMASK(NV917E_SET_CSC_RED2RED_COEFF));

    for (y = 0; y < 3; y++) {
        int x;

        for (x = 0; x < 4; x++) {
            // Use DRF_NUM to truncate client-supplied values that are out of
            // range.
            NvU32 val = DRF_NUM(917C, _SET_CSC_RED2RED, _COEFF,
                                matrix->m[y][x]);

            if (x == 0 && y == 0) {
                val |= extraFirstWordBits;
            }

            nvDmaSetStartEvoMethod(pChannel, method, 1);
            nvDmaSetEvoMethodData(pChannel, val);

            method += 4;
        }
    }
}

/*
 * These values are the same between all base
 * (_SURFACE_SET_PARAMS_FORMAT_) and core (_HEAD_SET_PARAMS_FORMAT_)
 * EVO classes.
 *
 * Return 0 in the case of an unrecognized NvKmsSurfaceMemoryFormat.
 */
static NvU32 nvHwFormatFromKmsFormat90(
    const enum NvKmsSurfaceMemoryFormat format)
{
    switch (format) {
    case NvKmsSurfaceMemoryFormatI8:
        return NV917D_HEAD_SET_PARAMS_FORMAT_I8;
    case NvKmsSurfaceMemoryFormatA1R5G5B5:
    case NvKmsSurfaceMemoryFormatX1R5G5B5:
        return NV917D_HEAD_SET_PARAMS_FORMAT_A1R5G5B5;
    case NvKmsSurfaceMemoryFormatR5G6B5:
        return NV917D_HEAD_SET_PARAMS_FORMAT_R5G6B5;
    case NvKmsSurfaceMemoryFormatA8R8G8B8:
    case NvKmsSurfaceMemoryFormatX8R8G8B8:
        return NV917D_HEAD_SET_PARAMS_FORMAT_A8R8G8B8;
    case NvKmsSurfaceMemoryFormatA8B8G8R8:
    case NvKmsSurfaceMemoryFormatX8B8G8R8:
        return NV917D_HEAD_SET_PARAMS_FORMAT_A8B8G8R8;
    case NvKmsSurfaceMemoryFormatA2B10G10R10:
    case NvKmsSurfaceMemoryFormatX2B10G10R10:
        return NV917D_HEAD_SET_PARAMS_FORMAT_A2B10G10R10;
    case NvKmsSurfaceMemoryFormatRF16GF16BF16AF16:
    case NvKmsSurfaceMemoryFormatRF16GF16BF16XF16:
        return NV917D_HEAD_SET_PARAMS_FORMAT_RF16_GF16_BF16_AF16;
    case NvKmsSurfaceMemoryFormatR16G16B16A16:
        return NV917D_HEAD_SET_PARAMS_FORMAT_R16_G16_B16_A16;
    case NvKmsSurfaceMemoryFormatRF32GF32BF32AF32:
    case NvKmsSurfaceMemoryFormatY8_U8__Y8_V8_N422:
    case NvKmsSurfaceMemoryFormatU8_Y8__V8_Y8_N422:
    case NvKmsSurfaceMemoryFormatY8___U8V8_N444:
    case NvKmsSurfaceMemoryFormatY8___V8U8_N444:
    case NvKmsSurfaceMemoryFormatY8___U8V8_N422:
    case NvKmsSurfaceMemoryFormatY8___V8U8_N422:
    case NvKmsSurfaceMemoryFormatY8___U8V8_N420:
    case NvKmsSurfaceMemoryFormatY8___V8U8_N420:
    case NvKmsSurfaceMemoryFormatY10___U10V10_N444:
    case NvKmsSurfaceMemoryFormatY10___V10U10_N444:
    case NvKmsSurfaceMemoryFormatY10___U10V10_N422:
    case NvKmsSurfaceMemoryFormatY10___V10U10_N422:
    case NvKmsSurfaceMemoryFormatY10___U10V10_N420:
    case NvKmsSurfaceMemoryFormatY10___V10U10_N420:
    case NvKmsSurfaceMemoryFormatY12___U12V12_N444:
    case NvKmsSurfaceMemoryFormatY12___V12U12_N444:
    case NvKmsSurfaceMemoryFormatY12___U12V12_N422:
    case NvKmsSurfaceMemoryFormatY12___V12U12_N422:
    case NvKmsSurfaceMemoryFormatY12___U12V12_N420:
    case NvKmsSurfaceMemoryFormatY12___V12U12_N420:
    case NvKmsSurfaceMemoryFormatY8___U8___V8_N444:
    case NvKmsSurfaceMemoryFormatY8___U8___V8_N420:
        return 0;
    }

    return 0;
}

static void EvoSetSurface(NVDevEvoPtr pDevEvo,
                          const int head,
                          const NVSurfaceEvoRec *pSurfaceEvo,
                          const struct NvKmsCscMatrix *pCscMatrix,
                          NVEvoUpdateState *updateState)
{
    NVEvoChannelPtr pChannel = pDevEvo->core;
    NvU32 sd;

    /* These methods should only apply to a single pDpy */
    nvAssert(pDevEvo->subDevMaskStackDepth > 0);

    nvUpdateUpdateState(pDevEvo, updateState, pChannel);

    FOR_EACH_SUBDEV_IN_MASK(sd, nvPeekEvoSubDevMask(pDevEvo)) {
        /*
         * The EVO2 ->SetCursorImage() function programs cursor image surface
         * only if NVEvoSubDeviceRec::pCoreChannelSurface is non-null.
         */
        pDevEvo->pSubDevices[sd]->pCoreChannelSurface[head] = pSurfaceEvo;
    } FOR_EACH_SUBDEV_IN_MASK_END

    if (!pSurfaceEvo) {
        // Disable surface scanout on this head.  It will scan out the default
        // base color instead.
        nvDmaSetStartEvoMethod(pChannel, NV917D_HEAD_SET_CONTEXT_DMAS_ISO(head), 1);
        nvDmaSetEvoMethodData(pChannel, 0);
        return;
    }

    nvAssert(pSurfaceEvo->planes[0].surfaceDesc.ctxDmaHandle);

    // XXX[AGP]: These methods are sequential, but sending them with a single
    // count=7 method header sometimes causes EVO to throw an IsoViolation
    // exception.

    // Set the surface parameters.
    nvDmaSetStartEvoMethod(pChannel, NV917D_HEAD_SET_OFFSET(head), 1);
    nvDmaSetEvoMethodData(pChannel,
        nvCtxDmaOffsetFromBytes(pSurfaceEvo->planes[0].offset));

    nvDmaSetStartEvoMethod(pChannel, NV917D_HEAD_SET_SIZE(head), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(917D, _HEAD_SET_SIZE, _WIDTH, pSurfaceEvo->widthInPixels) |
        DRF_NUM(917D, _HEAD_SET_SIZE, _HEIGHT, pSurfaceEvo->heightInPixels));

    nvDmaSetStartEvoMethod(pChannel, NV917D_HEAD_SET_STORAGE(head), 1);
    nvDmaSetEvoMethodData(pChannel, EvoComputeSetStorage90(pDevEvo, pSurfaceEvo));

    nvDmaSetStartEvoMethod(pChannel, NV917D_HEAD_SET_PARAMS(head), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(917D, _HEAD_SET_PARAMS, _FORMAT,
        nvHwFormatFromKmsFormat90(pSurfaceEvo->format)) |
        DRF_DEF(917D, _HEAD_SET_PARAMS, _SUPER_SAMPLE, _X1_AA) |
        DRF_DEF(917D, _HEAD_SET_PARAMS, _GAMMA, _LINEAR));

    nvDmaSetStartEvoMethod(pChannel, NV917D_HEAD_SET_CONTEXT_DMAS_ISO(head), 1);
    nvDmaSetEvoMethodData(pChannel, pSurfaceEvo->planes[0].surfaceDesc.ctxDmaHandle);

    /* NULL => don't change the CSC. */
    if (pCscMatrix) {
        SetCscMatrix(pChannel, NV917D_HEAD_SET_CSC_RED2RED(head), pCscMatrix, 0);
    }
}

static void
EvoPushSetCoreSurfaceMethodsForOneSd(NVDevEvoRec *pDevEvo,
                                     const NvU32 sd,
                                     const int head,
                                     const NVSurfaceEvoRec *pSurfaceEvo,
                                     const struct NvKmsCscMatrix *pCscMatrix,
                                     NVEvoUpdateState *updateState)
{
    const NVEvoSubDevHeadStateRec *pSdHeadState =
                                   &pDevEvo->gpus[sd].headState[head];
    const NVFlipCursorEvoHwState *pSdCursorState = &pSdHeadState->cursor;

    const NVDispEvoRec *pDispEvo = pDevEvo->gpus[sd].pDispEvo;
    const NVDispHeadStateEvoRec *pHeadState = &pDispEvo->headState[head];

    NvBool  enableOutputLut = pHeadState->lut.outputLutEnabled;
    NvBool  enableBaseLut = pHeadState->lut.baseLutEnabled;

    NVLutSurfaceEvoPtr pCurLutSurfEvo = pHeadState->lut.pCurrSurface;
    NvU32 lutCtxdma = pCurLutSurfEvo != NULL ?
        pCurLutSurfEvo->surfaceDesc.ctxDmaHandle : 0x0;

    if (pSurfaceEvo == NULL || pCurLutSurfEvo == NULL) {
        enableOutputLut = FALSE;
        enableBaseLut = FALSE;
        lutCtxdma = 0x0;
    }

    nvPushEvoSubDevMask(pDevEvo, NVBIT(sd));

    EvoSetSurface(pDevEvo, head, pSurfaceEvo, pCscMatrix, updateState);

    EvoSetCursorImage(pDevEvo,
                      head,
                      pSurfaceEvo != NULL ?
                          pSdCursorState->pSurfaceEvo : NULL,
                      updateState,
                      &pSdCursorState->cursorCompParams);

    /*
     * EvoPushSetLUTContextDmaMethodsForOneSd() force enables base
     * Lut if core scanout surface depth is 8.
     */
    EvoPushSetLUTContextDmaMethodsForOneSd(
        pDevEvo, sd, head, lutCtxdma, enableBaseLut, enableOutputLut,
        updateState);

    nvPopEvoSubDevMask(pDevEvo);
}

static void
FlipBase90(NVDevEvoPtr pDevEvo,
           NVEvoChannelPtr pChannel,
           const NVFlipChannelEvoHwState *pHwState,
           NVEvoUpdateState *updateState)
{
    int eye;

    /* program notifier */

    if (pHwState->completionNotifier.surface.pSurfaceEvo == NULL) {

        nvDmaSetStartEvoMethod(pChannel, NV917C_SET_CONTEXT_DMA_NOTIFIER, 1);
        nvDmaSetEvoMethodData(pChannel, 0);
        nvDmaSetStartEvoMethod(pChannel, NV917C_SET_NOTIFIER_CONTROL, 1);
        nvDmaSetEvoMethodData(pChannel, 0);

    } else {
        const NVFlipNIsoSurfaceEvoHwState *pNIso =
            &pHwState->completionNotifier.surface;
        NvU32 value = 0;

        if (pNIso->format == NVKMS_NISO_FORMAT_LEGACY) {
            value = FLD_SET_DRF(917C, _SET_NOTIFIER_CONTROL, _FORMAT,
                                _LEGACY, value);
        } else {
            value = FLD_SET_DRF(917C, _SET_NOTIFIER_CONTROL, _FORMAT,
                                _FOUR_WORD, value);
        }

        value = FLD_SET_DRF_NUM(917C, _SET_NOTIFIER_CONTROL, _OFFSET,
                                pNIso->offsetInWords, value);

        value = FLD_SET_DRF_NUM(917C, _SET_NOTIFIER_CONTROL, _MODE,
                                pHwState->completionNotifier.awaken ?
                                NV917C_SET_NOTIFIER_CONTROL_MODE_WRITE_AWAKEN :
                                NV917C_SET_NOTIFIER_CONTROL_MODE_WRITE, value);

        nvDmaSetStartEvoMethod(pChannel, NV917C_SET_CONTEXT_DMA_NOTIFIER, 1);
        nvDmaSetEvoMethodData(pChannel, pNIso->pSurfaceEvo->planes[0].surfaceDesc.ctxDmaHandle);

        nvDmaSetStartEvoMethod(pChannel, NV917C_SET_NOTIFIER_CONTROL, 1);
        nvDmaSetEvoMethodData(pChannel, value);
    }

    /* program semaphore */
    nvAssertSameSemaphoreSurface(pHwState);

    if (pHwState->syncObject.u.semaphores.acquireSurface.pSurfaceEvo == NULL) {

        nvDmaSetStartEvoMethod(pChannel, NV917C_SET_CONTEXT_DMA_SEMAPHORE, 1);
        nvDmaSetEvoMethodData(pChannel, 0);

        nvDmaSetStartEvoMethod(pChannel, NV917C_SET_SEMAPHORE_CONTROL, 1);
        nvDmaSetEvoMethodData(pChannel, 0);

        nvDmaSetStartEvoMethod(pChannel, NV917C_SET_SEMAPHORE_ACQUIRE, 1);
        nvDmaSetEvoMethodData(pChannel, 0);

        nvDmaSetStartEvoMethod(pChannel, NV917C_SET_SEMAPHORE_RELEASE, 1);
        nvDmaSetEvoMethodData(pChannel, 0);
    } else {
        const NVFlipNIsoSurfaceEvoHwState *pNIso =
            &pHwState->syncObject.u.semaphores.acquireSurface;
        NvU32 value = 0;

        if (pNIso->format == NVKMS_NISO_FORMAT_LEGACY) {
            value = FLD_SET_DRF(917C, _SET_SEMAPHORE_CONTROL, _FORMAT,
                                _LEGACY, value);
        } else {
            value = FLD_SET_DRF(917C, _SET_SEMAPHORE_CONTROL, _FORMAT,
                                _FOUR_WORD, value);
        }

        value = FLD_SET_DRF_NUM(917C, _SET_SEMAPHORE_CONTROL, _OFFSET,
                                pNIso->offsetInWords, value);

        nvDmaSetStartEvoMethod(pChannel, NV917C_SET_CONTEXT_DMA_SEMAPHORE, 1);
        nvDmaSetEvoMethodData(pChannel, pNIso->pSurfaceEvo->planes[0].surfaceDesc.ctxDmaHandle);

        nvDmaSetStartEvoMethod(pChannel, NV917C_SET_SEMAPHORE_ACQUIRE, 1);
        nvDmaSetEvoMethodData(pChannel,
            pHwState->syncObject.u.semaphores.acquireValue);

        nvDmaSetStartEvoMethod(pChannel, NV917C_SET_SEMAPHORE_RELEASE, 1);
        nvDmaSetEvoMethodData(pChannel,
            pHwState->syncObject.u.semaphores.releaseValue);

        nvDmaSetStartEvoMethod(pChannel, NV917C_SET_SEMAPHORE_CONTROL, 1);
        nvDmaSetEvoMethodData(pChannel, value);
    }

    if (!pHwState->pSurfaceEvo[NVKMS_LEFT]) {
        nvAssert(!pHwState->pSurfaceEvo[NVKMS_RIGHT]);

        // Disable base on this head.
        nvDmaSetStartEvoMethod(pChannel, NV917C_SET_CONTEXT_DMAS_ISO(0), 1);
        nvDmaSetEvoMethodData(pChannel, 0);

        nvDmaSetStartEvoMethod(pChannel, NV917C_SET_CONTEXT_DMAS_ISO(1), 1);
        nvDmaSetEvoMethodData(pChannel, 0);

        nvDmaSetStartEvoMethod(pChannel, NV917C_SET_PRESENT_CONTROL, 1);
        nvDmaSetEvoMethodData(pChannel, 0);

        nvDmaSetStartEvoMethod(pChannel, NV917C_SET_CSC_RED2RED, 1);
        nvDmaSetEvoMethodData(pChannel, DRF_DEF(917C, _SET_CSC_RED2RED, _OWNER, _CORE));
        return;
    }

    NvU32 presentControl =
        DRF_NUM(917C, _SET_PRESENT_CONTROL, _MIN_PRESENT_INTERVAL,
                pHwState->minPresentInterval);

    if (pHwState->tearing) {
        presentControl = FLD_SET_DRF(917C, _SET_PRESENT_CONTROL, _BEGIN_MODE,
                                     _IMMEDIATE, presentControl);
        /*
         * This avoids an invalid state exception:
         *
         * if ((SetPresentControl.BeginMode != NON_TEARING) &&
         *     (SetPresentControl.BeginMode != AT_FRAME)
         *   && (wir_InterlockWithCore == ENABLE))
         *   throw NV_DISP_BASE_STATE_ERROR_001;
         */
        nvDisableCoreInterlockUpdateState(pDevEvo, updateState, pChannel);
    } else {
        presentControl = FLD_SET_DRF(917C, _SET_PRESENT_CONTROL, _BEGIN_MODE,
                                     _NON_TEARING, presentControl);
    }

    if (pHwState->pSurfaceEvo[NVKMS_RIGHT]) {
        if (pHwState->perEyeStereoFlip) {
            presentControl = FLD_SET_DRF(917C, _SET_PRESENT_CONTROL,
                                         _STEREO_FLIP_MODE, _AT_ANY_FRAME,
                                         presentControl);
        } else {
            presentControl = FLD_SET_DRF(917C, _SET_PRESENT_CONTROL,
                                         _STEREO_FLIP_MODE, _PAIR_FLIP,
                                         presentControl);
        }
        presentControl = FLD_SET_DRF(917C, _SET_PRESENT_CONTROL,
                                     _MODE, _STEREO, presentControl);
    } else {
        presentControl = FLD_SET_DRF(917C, _SET_PRESENT_CONTROL,
                                     _MODE, _MONO, presentControl);
    }

    // If we have a non-zero timestamp we need to enable timestamp mode
    if (pHwState->timeStamp == 0) {
        presentControl = FLD_SET_DRF(917C, _SET_PRESENT_CONTROL,
                                     _TIMESTAMP_MODE, _DISABLE, presentControl);
    } else {
        presentControl = FLD_SET_DRF(917C, _SET_PRESENT_CONTROL,
                                     _TIMESTAMP_MODE, _ENABLE, presentControl);
    }

    nvDmaSetStartEvoMethod(pChannel, NV917C_SET_TIMESTAMP_ORIGIN_LO, 2);
    nvDmaSetEvoMethodData(pChannel, 0);
    nvDmaSetEvoMethodData(pChannel, 0);

    nvDmaSetStartEvoMethod(pChannel, NV917C_SET_UPDATE_TIMESTAMP_LO, 2);
    nvDmaSetEvoMethodData(pChannel, NvU64_LO32(pHwState->timeStamp));
    nvDmaSetEvoMethodData(pChannel, NvU64_HI32(pHwState->timeStamp));

    nvDmaSetStartEvoMethod(pChannel, NV917C_SET_PRESENT_CONTROL, 1);
    nvDmaSetEvoMethodData(pChannel, presentControl);

    SetCscMatrix(pChannel, NV917C_SET_CSC_RED2RED, &pHwState->cscMatrix,
                 DRF_DEF(917C, _SET_CSC_RED2RED, _OWNER, _BASE));

    // Set the surface parameters.
    FOR_ALL_EYES(eye) {
        NvU32 ctxdma = 0;
        NvU64 offset = 0;

        if (pHwState->pSurfaceEvo[eye]) {
            ctxdma = pHwState->pSurfaceEvo[eye]->planes[0].surfaceDesc.ctxDmaHandle;
            offset = pHwState->pSurfaceEvo[eye]->planes[0].offset;
        }

        nvDmaSetStartEvoMethod(pChannel, NV917C_SURFACE_SET_OFFSET(0, eye), 1);
        nvDmaSetEvoMethodData(pChannel,
            DRF_NUM(917C, _SURFACE_SET_OFFSET, _ORIGIN,
                    nvCtxDmaOffsetFromBytes(offset)));

        nvDmaSetStartEvoMethod(pChannel, NV917C_SET_CONTEXT_DMAS_ISO(eye), 1);
        nvDmaSetEvoMethodData(pChannel, ctxdma);
    }

    ASSERT_EYES_MATCH(pHwState->pSurfaceEvo, widthInPixels);
    ASSERT_EYES_MATCH(pHwState->pSurfaceEvo, heightInPixels);
    nvDmaSetStartEvoMethod(pChannel, NV917C_SURFACE_SET_SIZE(0), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(917C, _SURFACE_SET_SIZE, _WIDTH,
                pHwState->pSurfaceEvo[NVKMS_LEFT]->widthInPixels) |
        DRF_NUM(917C, _SURFACE_SET_SIZE, _HEIGHT,
                pHwState->pSurfaceEvo[NVKMS_LEFT]->heightInPixels));

    nvAssert(pHwState->sizeIn.width == pHwState->pSurfaceEvo[NVKMS_LEFT]->widthInPixels);
    nvAssert(pHwState->sizeIn.height == pHwState->pSurfaceEvo[NVKMS_LEFT]->heightInPixels);
    nvAssert(pHwState->sizeIn.width == pHwState->sizeOut.width);
    nvAssert(pHwState->sizeIn.height == pHwState->sizeOut.height);

    nvAssert(!pHwState->pSurfaceEvo[NVKMS_RIGHT] ||
             (EvoComputeSetStorage90(pDevEvo, pHwState->pSurfaceEvo[NVKMS_LEFT]) ==
              EvoComputeSetStorage90(pDevEvo, pHwState->pSurfaceEvo[NVKMS_RIGHT])));
    nvDmaSetStartEvoMethod(pChannel, NV917C_SURFACE_SET_STORAGE(0), 1);
    nvDmaSetEvoMethodData(pChannel, EvoComputeSetStorage90(pDevEvo, pHwState->pSurfaceEvo[NVKMS_LEFT]));

    ASSERT_EYES_MATCH(pHwState->pSurfaceEvo, format);
    nvDmaSetStartEvoMethod(pChannel, NV917C_SURFACE_SET_PARAMS(0), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(917C, _SURFACE_SET_PARAMS, _FORMAT,
        nvHwFormatFromKmsFormat90(pHwState->pSurfaceEvo[NVKMS_LEFT]->format)) |
        DRF_DEF(917C, _SURFACE_SET_PARAMS, _SUPER_SAMPLE, _X1_AA) |
        DRF_DEF(917C, _SURFACE_SET_PARAMS, _GAMMA, _LINEAR));

    nvAssert(pHwState->inputLut.pLutSurfaceEvo == NULL);
}

static void
FlipOverlay90(NVDevEvoPtr pDevEvo,
              NVEvoChannelPtr pChannel,
              const NVFlipChannelEvoHwState *pHwState,
              NvBool *pInterlockwithCore)
{
    const NvU32 head =
        NV_EVO_CHANNEL_MASK_BASE_HEAD_NUMBER(pChannel->channelMask);
    const NVSurfaceEvoRec *pSurfaceEvo = pHwState->pSurfaceEvo[NVKMS_LEFT];
    NvU32 value;
    NvU32 sd;

    /* Overlay class 917E can't do stereo */
    nvAssert(!pHwState->pSurfaceEvo[NVKMS_RIGHT]);

    /*
     * The NVKMS driver enforces these conditions on its clients: 1) enable a
     * core-surface before enabling an overlay-surface, 2) disable an
     * overlay-surface before disabling a core-surface.
     *
     * Updates to enable/disable a core and an overlay surface execute
     * separately and are not interlocked. To avoid a race condition between a
     * core and an overlay channel, detect an overlay channel update which is
     * enabling/disabling an overlay-surface and interlock that update with a
     * core channel update.
     *
     * This makes sure that an update to disable an overlay-surface interlocked
     * with a core channel and a follow-on update to disable the core-surface
     * will wait for the previous overlay flip to complete. It also makes sure
     * that an update to enable an overlay-surface will wait for the previous
     * core channel flip to complete.
     */

    FOR_EACH_SUBDEV_IN_MASK(sd, nvPeekEvoSubDevMask(pDevEvo)) {
        NvBool prevCtxDmaIso =
            pDevEvo->pSubDevices[sd]->overlayContextDmaIso[head];

        if ((prevCtxDmaIso != 0x0 && pSurfaceEvo == NULL) ||
            (prevCtxDmaIso == 0x0 && pSurfaceEvo != NULL)) {
            *pInterlockwithCore = TRUE;
        }

        if (pSurfaceEvo != NULL) {
            pDevEvo->pSubDevices[sd]->overlayContextDmaIso[head] =
                pSurfaceEvo->planes[0].surfaceDesc.ctxDmaHandle;
            pDevEvo->pSubDevices[sd]->overlaySurfFormat[head] = pSurfaceEvo->format;

        } else {
            pDevEvo->pSubDevices[sd]->overlayContextDmaIso[head] = 0x0;
        }
    } FOR_EACH_SUBDEV_IN_MASK_END

    /* program notifier */

    if (pHwState->completionNotifier.surface.pSurfaceEvo == NULL) {

        nvDmaSetStartEvoMethod(pChannel, NV917E_SET_CONTEXT_DMA_NOTIFIER, 1);
        nvDmaSetEvoMethodData(pChannel, 0);
        nvDmaSetStartEvoMethod(pChannel, NV917E_SET_NOTIFIER_CONTROL, 1);
        nvDmaSetEvoMethodData(pChannel, 0);

    } else {
        const NVFlipNIsoSurfaceEvoHwState *pNIso =
            &pHwState->completionNotifier.surface;
        value = 0;

        if (pNIso->format == NVKMS_NISO_FORMAT_LEGACY) {
            value = FLD_SET_DRF(917E, _SET_NOTIFIER_CONTROL, _FORMAT,
                                _LEGACY, value);
        } else {
            value = FLD_SET_DRF(917E, _SET_NOTIFIER_CONTROL, _FORMAT,
                                _FOUR_WORD, value);
        }

        value = FLD_SET_DRF_NUM(917E, _SET_NOTIFIER_CONTROL, _OFFSET,
                                pNIso->offsetInWords, value);

        value = FLD_SET_DRF_NUM(917E, _SET_NOTIFIER_CONTROL, _MODE,
                                pHwState->completionNotifier.awaken ?
                                NV917E_SET_NOTIFIER_CONTROL_MODE_WRITE_AWAKEN :
                                NV917E_SET_NOTIFIER_CONTROL_MODE_WRITE, value);

        nvDmaSetStartEvoMethod(pChannel, NV917E_SET_CONTEXT_DMA_NOTIFIER, 1);
        nvDmaSetEvoMethodData(pChannel, pNIso->pSurfaceEvo->planes[0].surfaceDesc.ctxDmaHandle);

        nvDmaSetStartEvoMethod(pChannel, NV917E_SET_NOTIFIER_CONTROL, 1);
        nvDmaSetEvoMethodData(pChannel, value);
    }

    /* program semaphore */
    nvAssertSameSemaphoreSurface(pHwState);

    if (pHwState->syncObject.u.semaphores.acquireSurface.pSurfaceEvo == NULL) {

        nvDmaSetStartEvoMethod(pChannel, NV917E_SET_CONTEXT_DMA_SEMAPHORE, 1);
        nvDmaSetEvoMethodData(pChannel, 0);

        nvDmaSetStartEvoMethod(pChannel, NV917E_SET_SEMAPHORE_CONTROL, 1);
        nvDmaSetEvoMethodData(pChannel, 0);

    } else {
        const NVFlipNIsoSurfaceEvoHwState *pNIso =
         &pHwState->syncObject.u.semaphores.acquireSurface;
        value = 0;

        if (pNIso->format == NVKMS_NISO_FORMAT_LEGACY) {
            value = FLD_SET_DRF(917E, _SET_SEMAPHORE_CONTROL, _FORMAT,
                                _LEGACY, value);
        } else {
            value = FLD_SET_DRF(917E, _SET_SEMAPHORE_CONTROL, _FORMAT,
                                _FOUR_WORD, value);
        }

        value = FLD_SET_DRF_NUM(917E, _SET_SEMAPHORE_CONTROL, _OFFSET,
                                pNIso->offsetInWords, value);

        nvDmaSetStartEvoMethod(pChannel, NV917E_SET_CONTEXT_DMA_SEMAPHORE, 1);
        nvDmaSetEvoMethodData(pChannel, pNIso->pSurfaceEvo->planes[0].surfaceDesc.ctxDmaHandle);

        nvDmaSetStartEvoMethod(pChannel, NV917E_SET_SEMAPHORE_ACQUIRE, 1);
        nvDmaSetEvoMethodData(pChannel,
            pHwState->syncObject.u.semaphores.acquireValue);

        nvDmaSetStartEvoMethod(pChannel, NV917E_SET_SEMAPHORE_RELEASE, 1);
        nvDmaSetEvoMethodData(pChannel,
            pHwState->syncObject.u.semaphores.releaseValue);

        nvDmaSetStartEvoMethod(pChannel, NV917E_SET_SEMAPHORE_CONTROL, 1);
        nvDmaSetEvoMethodData(pChannel, value);
    }

    nvDmaSetStartEvoMethod(pChannel, NV917E_SET_TIMESTAMP_ORIGIN_LO, 2);
    nvDmaSetEvoMethodData(pChannel, 0);
    nvDmaSetEvoMethodData(pChannel, 0);

    nvDmaSetStartEvoMethod(pChannel, NV917E_SET_UPDATE_TIMESTAMP_LO, 2);
    nvDmaSetEvoMethodData(pChannel, NvU64_LO32(pHwState->timeStamp));
    nvDmaSetEvoMethodData(pChannel, NvU64_HI32(pHwState->timeStamp));

    if (pHwState->timeStamp == 0) {
        value = NV917E_SET_PRESENT_CONTROL_BEGIN_MODE_ASAP;
    } else {
        value = NV917E_SET_PRESENT_CONTROL_BEGIN_MODE_TIMESTAMP;
    }
    nvAssert(!pHwState->tearing);
    nvAssert(!pHwState->vrrTearing);
    nvAssert(!pHwState->perEyeStereoFlip);

    nvDmaSetStartEvoMethod(pChannel, NV917E_SET_PRESENT_CONTROL, 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(917E, _SET_PRESENT_CONTROL, _BEGIN_MODE, value) |
        DRF_NUM(917E, _SET_PRESENT_CONTROL, _MIN_PRESENT_INTERVAL,
                pHwState->minPresentInterval));

    if (!pSurfaceEvo) {
        // Disable overlay on this head.
        nvDmaSetStartEvoMethod(pChannel, NV917E_SET_CONTEXT_DMAS_ISO(NVKMS_LEFT), 1);
        nvDmaSetEvoMethodData(pChannel, 0);
        return;
    }

    nvAssert(pSurfaceEvo->planes[0].surfaceDesc.ctxDmaHandle);

    nvDmaSetStartEvoMethod(pChannel, NV917E_SET_SIZE_IN, 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(917E, _SET_SIZE_IN, _WIDTH, pHwState->sizeIn.width) |
        DRF_NUM(917E, _SET_SIZE_IN, _HEIGHT, pHwState->sizeIn.height));

    nvDmaSetStartEvoMethod(pChannel, NV917E_SET_SIZE_OUT, 1);
    nvDmaSetEvoMethodData(pChannel, DRF_NUM(917E, _SET_SIZE_OUT, _WIDTH,
                                     pHwState->sizeOut.width));

    // Set the surface parameters.
    nvDmaSetStartEvoMethod(pChannel, NV917E_SURFACE_SET_OFFSET(NVKMS_LEFT), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(917E, _SURFACE_SET_OFFSET, _ORIGIN,
                nvCtxDmaOffsetFromBytes(pSurfaceEvo->planes[0].offset)));

    nvDmaSetStartEvoMethod(pChannel, NV917E_SURFACE_SET_SIZE, 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(917E, _SURFACE_SET_SIZE, _WIDTH, pSurfaceEvo->widthInPixels) |
        DRF_NUM(917E, _SURFACE_SET_SIZE, _HEIGHT, pSurfaceEvo->heightInPixels));

    nvDmaSetStartEvoMethod(pChannel, NV917E_SURFACE_SET_STORAGE, 1);
    nvDmaSetEvoMethodData(pChannel, EvoComputeSetStorage90(pDevEvo, pSurfaceEvo));

    nvDmaSetStartEvoMethod(pChannel, NV917E_SURFACE_SET_PARAMS, 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(917E, _SURFACE_SET_PARAMS, _FORMAT,
        EvoOverlayFormatFromKmsFormat91(pSurfaceEvo->format)) |
        DRF_DEF(917E, _SURFACE_SET_PARAMS, _COLOR_SPACE, _RGB));

    SetCscMatrix(pChannel, NV917E_SET_CSC_RED2RED, &pHwState->cscMatrix, 0);

    nvDmaSetStartEvoMethod(pChannel, NV917E_SET_CONTEXT_DMAS_ISO(NVKMS_LEFT), 1);
    nvDmaSetEvoMethodData(pChannel, pSurfaceEvo->planes[0].surfaceDesc.ctxDmaHandle);

    nvAssert(pHwState->inputLut.pLutSurfaceEvo == NULL);
}

static NvBool
needToReprogramCoreSurface(NVDevEvoPtr pDevEvo,
                           const NvU32 sd,
                           const NvU32 head,
                           const NVSurfaceEvoRec *pNewSurfaceEvo)
{
    const NVDispEvoRec *pDispEvo = pDevEvo->gpus[sd].pDispEvo;
    const NVDispHeadStateEvoRec *pHeadState = &pDispEvo->headState[head];
    NvBool  enableBaseLut = pHeadState->lut.baseLutEnabled;

    const NVSurfaceEvoRec *pCurrCoreSurfaceEvo =
        pDevEvo->pSubDevices[sd]->pCoreChannelSurface[head];
    const NvBool currIsBaseSurfSpecified =
        pDevEvo->pSubDevices[sd]->isBaseSurfSpecified[head];
    const NvU32 currHeightInPixels = pCurrCoreSurfaceEvo != NULL ?
        pCurrCoreSurfaceEvo->heightInPixels : 0;
    const NvU32 currWidthInPixels = pCurrCoreSurfaceEvo != NULL ?
        pCurrCoreSurfaceEvo->widthInPixels : 0;
    const enum NvKmsSurfaceMemoryFormat currFormat =
        pCurrCoreSurfaceEvo != NULL ?
        pCurrCoreSurfaceEvo->format : NvKmsSurfaceMemoryFormatI8;

    const NvBool newIsBaseSurfSpecified = pNewSurfaceEvo != NULL;
    const NvU32 newHeightInPixels = pNewSurfaceEvo != NULL ?
        pNewSurfaceEvo->heightInPixels : 0;
    const NvU32 newWidthInPixels = pNewSurfaceEvo != NULL ?
        pNewSurfaceEvo->widthInPixels : 0;
    const enum NvKmsSurfaceMemoryFormat newFormat = pNewSurfaceEvo != NULL ?
        pNewSurfaceEvo->format : NvKmsSurfaceMemoryFormatI8;

    /* If base channel flips from NULL to non-NULL surface or vice-versa */
    if (currIsBaseSurfSpecified != newIsBaseSurfSpecified) {
        return TRUE;
    }

    /*
     * Reprogram the core surface if the current and new base surfaces have
     * different size or format. The format check is needed to enable/disable
     * the input lut if the input lut is not explicitly enabled/disabled by
     * client and the base surface if flipping to or flipping away from the I8
     * format.
     */
    if (newIsBaseSurfSpecified) {

        if (newWidthInPixels != currWidthInPixels ||
                newHeightInPixels != currHeightInPixels) {
            return TRUE;
        }

        if (!enableBaseLut &&
            newFormat != currFormat &&
            (currFormat == NvKmsSurfaceMemoryFormatI8 ||
                newFormat == NvKmsSurfaceMemoryFormatI8)) {
            return TRUE;
        }
    }

    return !currIsBaseSurfSpecified;
}

static void
EvoPushUpdateCompositionIfNeeded(NVDevEvoPtr pDevEvo,
                                 const NvU32 sd,
                                 NVEvoChannelPtr pChannel,
                                 const NVFlipChannelEvoHwState *pHwState,
                                 NVEvoUpdateState *updateState,
                                 NvBool bypassComposition)
{
    const NVSurfaceEvoRec *pNewSurfaceEvo = pHwState->pSurfaceEvo[NVKMS_LEFT];
    NvBool updateComposition = FALSE;
    const NVFlipChannelEvoHwState *pBaseHwState = NULL;
    const NVFlipChannelEvoHwState *pOverlayHwState = NULL;
    NvU32 head = NV_INVALID_HEAD;

    if (pNewSurfaceEvo == NULL) {
        return;
    }

    /*
     * Re-program the composition parameters if this is first layer update, or
     * if color key selection method is changed, or if layer is using source
     * color keying and color key is changed.
     */

    if ((pChannel->channelMask & NV_EVO_CHANNEL_MASK_BASE_ALL) != 0) {
        head = NV_EVO_CHANNEL_MASK_BASE_HEAD_NUMBER(pChannel->channelMask);
        pOverlayHwState =
            &pDevEvo->gpus[sd].headState[head].layer[NVKMS_OVERLAY_LAYER];
        pBaseHwState = pHwState;

        if ((!pDevEvo->pSubDevices[sd]->baseComp[head].initialized) ||

            (pHwState->composition.colorKeySelect !=
                pDevEvo->pSubDevices[sd]->baseComp[head].colorKeySelect) ||

            ((pHwState->composition.colorKeySelect ==
                  NVKMS_COMPOSITION_COLOR_KEY_SELECT_SRC) &&
             (pNewSurfaceEvo->format !=
                  pDevEvo->pSubDevices[sd]->baseSurfFormat[head] ||
              nvkms_memcmp(&pHwState->composition.colorKey,
                           &pDevEvo->pSubDevices[sd]->baseComp[head].colorKey,
                           sizeof(&pHwState->composition.colorKey)) != 0))) {

            pDevEvo->pSubDevices[sd]->baseComp[head].initialized = TRUE;
            pDevEvo->pSubDevices[sd]->baseComp[head].colorKeySelect =
                pHwState->composition.colorKeySelect;
            pDevEvo->pSubDevices[sd]->baseComp[head].colorKey =
                pHwState->composition.colorKey;
            updateComposition = TRUE;
        }
    }

    if ((pChannel->channelMask & NV_EVO_CHANNEL_MASK_OVERLAY_ALL) != 0) {
        head = NV_EVO_CHANNEL_MASK_OVERLAY_HEAD_NUMBER(pChannel->channelMask);
        pBaseHwState =
            &pDevEvo->gpus[sd].headState[head].layer[NVKMS_MAIN_LAYER];
        pOverlayHwState = pHwState;

        if ((!pDevEvo->pSubDevices[sd]->overlayComp[head].initialized) ||

            (pHwState->composition.colorKeySelect !=
                pDevEvo->pSubDevices[sd]->overlayComp[head].colorKeySelect) ||

            ((pHwState->composition.colorKeySelect ==
                  NVKMS_COMPOSITION_COLOR_KEY_SELECT_SRC) &&
             (pNewSurfaceEvo->format !=
                  pDevEvo->pSubDevices[sd]->overlaySurfFormat[head] ||
              nvkms_memcmp(&pHwState->composition.colorKey,
                           &pDevEvo->pSubDevices[sd]->overlayComp[head].colorKey,
                           sizeof(&pHwState->composition.colorKey)) != 0))) {

            pDevEvo->pSubDevices[sd]->overlayComp[head].initialized = TRUE;
            pDevEvo->pSubDevices[sd]->overlayComp[head].colorKeySelect =
                pHwState->composition.colorKeySelect;
            pDevEvo->pSubDevices[sd]->overlayComp[head].colorKey =
                pHwState->composition.colorKey;
            updateComposition = TRUE;
        }
    }

    if (updateComposition) {
        nvPushEvoSubDevMask(pDevEvo, NVBIT(sd));
        EvoPushUpdateComposition(pDevEvo, head, pBaseHwState, pOverlayHwState,
                                 updateState, bypassComposition);
        nvPopEvoSubDevMask(pDevEvo);
    }
}

static void EvoFlip90(NVDevEvoPtr pDevEvo,
                      NVEvoChannelPtr pChannel,
                      const NVFlipChannelEvoHwState *pHwState,
                      NVEvoUpdateState *updateState,
                      NvBool bypassComposition)
{
    NvU32 sd;

    FOR_EACH_SUBDEV_IN_MASK(sd, nvPeekEvoSubDevMask(pDevEvo)) {
        EvoPushUpdateCompositionIfNeeded(pDevEvo, sd, pChannel, pHwState,
                                         updateState, bypassComposition);
    } FOR_EACH_SUBDEV_IN_MASK_END

    if ((pChannel->channelMask & NV_EVO_CHANNEL_MASK_BASE_ALL) != 0) {
        const NvU32 head =
            NV_EVO_CHANNEL_MASK_BASE_HEAD_NUMBER(pChannel->channelMask);

        FOR_EACH_SUBDEV_IN_MASK(sd, nvPeekEvoSubDevMask(pDevEvo)) {
            if (needToReprogramCoreSurface(
                    pDevEvo,
                    sd,
                    head,
                    pHwState->pSurfaceEvo[NVKMS_LEFT])) {
                const struct NvKmsCscMatrix zeroCscMatrix = { };

                nvPushEvoSubDevMask(pDevEvo, NVBIT(sd));
                EvoPushSetCoreSurfaceMethodsForOneSd(pDevEvo, sd, head,
                    pHwState->pSurfaceEvo[NVKMS_LEFT],
                    &zeroCscMatrix, updateState);
                nvPopEvoSubDevMask(pDevEvo);
            }

            if (pHwState->pSurfaceEvo[NVKMS_LEFT] != NULL) {
                pDevEvo->pSubDevices[sd]->isBaseSurfSpecified[head] = TRUE;
                pDevEvo->pSubDevices[sd]->baseSurfFormat[head] =
                    pHwState->pSurfaceEvo[NVKMS_LEFT]->format;
            } else {
                pDevEvo->pSubDevices[sd]->isBaseSurfSpecified[head] = FALSE;
            }
        } FOR_EACH_SUBDEV_IN_MASK_END

        FlipBase90(pDevEvo, pChannel, pHwState, updateState);

        if (pHwState->vrrTearing) {
            int head = NV_EVO_CHANNEL_MASK_BASE_HEAD_NUMBER(pChannel->channelMask);
            NvU32 sd, subDeviceMask = nvPeekEvoSubDevMask(pDevEvo);

            for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
                if (subDeviceMask & (1 << sd)) {
                    updateState->subdev[sd].base[head].vrrTearing = TRUE;
                }
            }
        }
    } else if ((pChannel->channelMask & NV_EVO_CHANNEL_MASK_OVERLAY_ALL) != 0) {
        NvBool interlockWithCore = FALSE;

        FlipOverlay90(pDevEvo, pChannel, pHwState, &interlockWithCore);

        if (interlockWithCore) {
            nvUpdateUpdateState(pDevEvo, updateState, pDevEvo->core);
        }
    } else {
        nvAssert(!"Unknown channel mask in EvoFlip90");
    }

    nvUpdateUpdateState(pDevEvo, updateState, pChannel);
}

static void EvoFlipTransitionWAR90(NVDevEvoPtr pDevEvo, NvU32 sd, NvU32 head,
                                   const NVEvoSubDevHeadStateRec *pSdHeadState,
                                   const NVFlipEvoHwState *pFlipState,
                                   NVEvoUpdateState *updateState)
{
    /* Nothing to do pre-Turing */
}

/*!
 * Pack the given abstract color key into a key and mask as required
 * by the display engine.
 *
 * \param[in]   format  NVKMS format for the input surface
 * \param[in]   key     NVKMS representation of a color key
 * \param[out]  pValue  NV857E_SET_KEY_COLOR_COLOR value
 * \param[out]  pMask   NV857E_SET_KEY_COLOR_MASK value
 */
static void EvoPackColorKey91(enum NvKmsSurfaceMemoryFormat format,
                              const NVColorKey key,
                              NvU32 *pValue, NvU32 *pMask)
{
    NvU32 value = 0, mask = 0;
    switch (format) {
    case NvKmsSurfaceMemoryFormatR5G6B5:
        if (key.matchR) {
            mask  |=          0x1f  << 11;
            value |= (key.r & 0x1f) << 11;
        }
        if (key.matchG) {
            mask  |=          0x3f  << 5;
            value |= (key.g & 0x3f) << 5;
        }
        if (key.matchB) {
            mask  |=          0x1f  << 0;
            value |= (key.b & 0x1f) << 0;
        }
        break;
    case NvKmsSurfaceMemoryFormatA1R5G5B5:
    case NvKmsSurfaceMemoryFormatX1R5G5B5:
        if (key.matchA) {
            mask  |=          0x1  << 15;
            value |= (key.a & 0x1) << 15;
        }
        if (key.matchR) {
            mask  |=          0x1f  << 10;
            value |= (key.r & 0x1f) << 10;
        }
        if (key.matchG) {
            mask  |=          0x1f  << 5;
            value |= (key.g & 0x1f) << 5;
        }
        if (key.matchB) {
            mask  |=          0x1f  << 0;
            value |= (key.b & 0x1f) << 0;
        }
        break;
    case NvKmsSurfaceMemoryFormatA8R8G8B8:
    case NvKmsSurfaceMemoryFormatX8R8G8B8:
        if (key.matchA) {
            /* Only one bit of alpha is handled by the hw. */
            mask  |=          0x1  << 31;
            value |= (key.a ? 1:0) << 31;
        }
        if (key.matchR) {
            mask  |=          0xff  << 16;
            value |= (key.r & 0xff) << 16;
        }
        if (key.matchG) {
            mask  |=          0xff  << 8;
            value |= (key.g & 0xff) << 8;
        }
        if (key.matchB) {
            mask  |=          0xff  << 0;
            value |= (key.b & 0xff) << 0;
        }
        break;
    case NvKmsSurfaceMemoryFormatA8B8G8R8:
    case NvKmsSurfaceMemoryFormatX8B8G8R8:
        if (key.matchA) {
            /* Only one bit of alpha is handled by the hw. */
            mask  |=          0x1  << 31;
            value |= (key.a ? 1:0) << 31;
        }
        if (key.matchB) {
            mask  |=          0xff  << 16;
            value |= (key.b & 0xff) << 16;
        }
        if (key.matchG) {
            mask  |=          0xff  << 8;
            value |= (key.g & 0xff) << 8;
        }
        if (key.matchR) {
            mask  |=          0xff  << 0;
            value |= (key.r & 0xff) << 0;
        }
        break;
    case NvKmsSurfaceMemoryFormatA2B10G10R10:
    case NvKmsSurfaceMemoryFormatX2B10G10R10:
        if (key.matchA) {
            /* Only one bit of alpha is handled by the hw. */
            mask  |=          0x1  << 31;
            value |= (key.a ? 1:0) << 31;
        }
        if (key.matchB) {
            mask  |=          0x3ff  << 20;
            value |= (key.b & 0x3ff) << 20;
        }
        if (key.matchG) {
            mask  |=          0x3ff  << 10;
            value |= (key.g & 0x3ff) << 10;
        }
        if (key.matchR) {
            mask  |=          0x3ff  << 0;
            value |= (key.r & 0x3ff) << 0;
        }
        break;
    case NvKmsSurfaceMemoryFormatI8:
    case NvKmsSurfaceMemoryFormatRF16GF16BF16AF16:
    case NvKmsSurfaceMemoryFormatRF16GF16BF16XF16:
    case NvKmsSurfaceMemoryFormatR16G16B16A16:
    case NvKmsSurfaceMemoryFormatRF32GF32BF32AF32:
    case NvKmsSurfaceMemoryFormatY8_U8__Y8_V8_N422:
    case NvKmsSurfaceMemoryFormatU8_Y8__V8_Y8_N422:
    case NvKmsSurfaceMemoryFormatY8___U8V8_N444:
    case NvKmsSurfaceMemoryFormatY8___V8U8_N444:
    case NvKmsSurfaceMemoryFormatY8___U8V8_N422:
    case NvKmsSurfaceMemoryFormatY8___V8U8_N422:
    case NvKmsSurfaceMemoryFormatY8___U8V8_N420:
    case NvKmsSurfaceMemoryFormatY8___V8U8_N420:
    case NvKmsSurfaceMemoryFormatY10___U10V10_N444:
    case NvKmsSurfaceMemoryFormatY10___V10U10_N444:
    case NvKmsSurfaceMemoryFormatY10___U10V10_N422:
    case NvKmsSurfaceMemoryFormatY10___V10U10_N422:
    case NvKmsSurfaceMemoryFormatY10___U10V10_N420:
    case NvKmsSurfaceMemoryFormatY10___V10U10_N420:
    case NvKmsSurfaceMemoryFormatY12___U12V12_N444:
    case NvKmsSurfaceMemoryFormatY12___V12U12_N444:
    case NvKmsSurfaceMemoryFormatY12___U12V12_N422:
    case NvKmsSurfaceMemoryFormatY12___V12U12_N422:
    case NvKmsSurfaceMemoryFormatY12___U12V12_N420:
    case NvKmsSurfaceMemoryFormatY12___V12U12_N420:
    case NvKmsSurfaceMemoryFormatY8___U8___V8_N444:
    case NvKmsSurfaceMemoryFormatY8___U8___V8_N420:
        nvAssert(!"Unhandled format in nvEvo1PackColorKey");
        break;
    }

    *pMask = mask;
    *pValue = value;
}

static NvBool EvoOverlayCompositionControlFromNvKmsCompositionParams(
    const NVFlipChannelEvoHwState *pBaseHwState,
    const NVFlipChannelEvoHwState *pOverlayHwState,
    NvU32 *pMode,
    NvU32 *pColorKeyValue,
    NvU32 *pColorKeyMask)
{
    const struct NvKmsCompositionParams *pBaseCompParams =
        &pBaseHwState->composition;
    const struct NvKmsCompositionParams *pOverlayCompParams =
        &pOverlayHwState->composition;

    switch (pOverlayCompParams->colorKeySelect) {
    case NVKMS_COMPOSITION_COLOR_KEY_SELECT_DISABLE:
        if (pOverlayCompParams->blendingMode[1] == NVKMS_COMPOSITION_BLENDING_MODE_OPAQUE) {
            *pMode = NV917E_SET_COMPOSITION_CONTROL_MODE_OPAQUE;
            *pColorKeyValue = *pColorKeyMask = 0;
        } else {
            return FALSE;
        }
        break;
    case NVKMS_COMPOSITION_COLOR_KEY_SELECT_SRC:
        if ((pOverlayCompParams->blendingMode[0] ==
                NVKMS_COMPOSITION_BLENDING_MODE_OPAQUE) &&
            (pOverlayCompParams->blendingMode[1] ==
                NVKMS_COMPOSITION_BLENDING_MODE_TRANSPARENT)) {

            *pMode = NV917E_SET_COMPOSITION_CONTROL_MODE_SOURCE_COLOR_VALUE_KEYING;

            if (pOverlayHwState->pSurfaceEvo[NVKMS_LEFT] != NULL) {
                EvoPackColorKey91(pOverlayHwState->pSurfaceEvo[NVKMS_LEFT]->format,
                                  pOverlayCompParams->colorKey,
                                  pColorKeyValue,
                                  pColorKeyMask);
            } else {
                *pColorKeyValue = *pColorKeyMask = 0;
            }

        } else {
            return FALSE;
        }
        break;
    case NVKMS_COMPOSITION_COLOR_KEY_SELECT_DST:
        if ((pBaseCompParams->colorKeySelect ==
                NVKMS_COMPOSITION_COLOR_KEY_SELECT_DISABLE) &&
            (pOverlayCompParams->blendingMode[1] ==
                NVKMS_COMPOSITION_BLENDING_MODE_OPAQUE)) {
            *pMode = NV917E_SET_COMPOSITION_CONTROL_MODE_OPAQUE;
            *pColorKeyValue = *pColorKeyMask = 0;
        } else if ((pBaseCompParams->colorKeySelect ==
                        NVKMS_COMPOSITION_COLOR_KEY_SELECT_SRC) &&
                   (pOverlayCompParams->blendingMode[1] ==
                        NVKMS_COMPOSITION_BLENDING_MODE_OPAQUE) &&
                   (pOverlayCompParams->blendingMode[0] ==
                        NVKMS_COMPOSITION_BLENDING_MODE_TRANSPARENT)) {

            *pMode = NV917E_SET_COMPOSITION_CONTROL_MODE_DESTINATION_COLOR_VALUE_KEYING;

            if (pBaseHwState->pSurfaceEvo[NVKMS_LEFT] != NULL) {
                EvoPackColorKey91(pBaseHwState->pSurfaceEvo[NVKMS_LEFT]->format,
                                  pBaseCompParams->colorKey,
                                  pColorKeyValue,
                                  pColorKeyMask);
            } else {
                *pColorKeyValue = *pColorKeyMask = 0;
            }

        } else {
            return FALSE;
        }
        break;
    default:
        return FALSE;
    }

    return TRUE;
}

static void
EvoPushUpdateComposition(NVDevEvoPtr pDevEvo,
                         const int head,
                         const NVFlipChannelEvoHwState *pBaseHwState,
                         const NVFlipChannelEvoHwState *pOverlayHwState,
                         NVEvoUpdateState *updateState,
                         NvBool bypassComposition)
{

    /* Composition is always programmed through the overlay channel. */
    NVEvoChannelPtr pChannel = pDevEvo->overlay[head];
    NvU32 colorKeyValue = 0, colorKeyMask = 0;
    NvU32 compositionModeValue = 0;

    nvUpdateUpdateState(pDevEvo, updateState, pChannel);

    if (!EvoOverlayCompositionControlFromNvKmsCompositionParams(
            pBaseHwState, pOverlayHwState,
            &compositionModeValue,
            &colorKeyValue,
            &colorKeyMask)) {
        /*
         * composition mode is validated during
         * nvUpdateFlipEvoHwState(), so it should always be valid when
         * we get here.
        */
        nvAssert(!"Invalid composition params");
        return;
    }

    nvDmaSetStartEvoMethod(pChannel, NV917E_SET_COMPOSITION_CONTROL, 1);
    nvDmaSetEvoMethodData(pChannel, compositionModeValue);

    nvDmaSetStartEvoMethod(pChannel, NV917E_SET_KEY_COLOR_LO, 2);
    nvDmaSetEvoMethodData(pChannel, colorKeyValue);
    nvDmaSetEvoMethodData(pChannel, 0);

    nvDmaSetStartEvoMethod(pChannel, NV917E_SET_KEY_MASK_LO, 2);
    nvDmaSetEvoMethodData(pChannel, colorKeyMask);
    nvDmaSetEvoMethodData(pChannel, 0);
}

/*
 * The LUT entries in INDEX_1025_UNITY_RANGE have 16 bits, with the
 * black value at 24576, and the white at 49151. Since the effective
 * range is 16384, we treat this as a 14-bit LUT.  However, we need to
 * clear the low 3 bits to WAR hardware bug 813188.  This gives us
 * 14-bit LUT values, but only 11 bits of precision.
 */
static inline NvU16 ColorToLUTEntry(NvU16 val)
{
    const NvU16 val14bit = val >> 2;
    return (val14bit & ~7) + 24576;
}

/* In INDEX_1025_UNITY_RANGE, the LUT indices for color depths with less
 * than 10 bpc are the indices you'd have in 257-entry mode multiplied
 * by four. So, you under-replicate all but the two least significant bits.
 * Since when is EVO supposed to make sense?
 */
static void
EvoFillLUTSurface90(NVEvoLutEntryRec *pLUTBuffer,
                    const NvU16 *red,
                    const NvU16 *green,
                    const NvU16 *blue,
                    int nColorMapEntries, int depth)
{
    int i, lutIndex;

    switch (depth) {
    case 15:
        for (i = 0; i < nColorMapEntries; i++) {
            lutIndex = PALETTE_DEPTH_SHIFT(i, 5) << 2;
            pLUTBuffer[lutIndex].Red   = ColorToLUTEntry(red[i]);
            pLUTBuffer[lutIndex].Green = ColorToLUTEntry(green[i]);
            pLUTBuffer[lutIndex].Blue  = ColorToLUTEntry(blue[i]);
        }
        break;
    case 16:
        for (i = 0; i < nColorMapEntries; i++) {
            pLUTBuffer[PALETTE_DEPTH_SHIFT(i, 6) << 2].Green = ColorToLUTEntry(green[i]);
            if (i < 32) {
                lutIndex = PALETTE_DEPTH_SHIFT(i, 5) << 2;
                pLUTBuffer[lutIndex].Red  = ColorToLUTEntry(red[i]);
                pLUTBuffer[lutIndex].Blue = ColorToLUTEntry(blue[i]);
            }
        }
        break;
    case 8:
    case 24:
        for (i = 0; i < nColorMapEntries; i++) {
            lutIndex = i << 2;
            pLUTBuffer[lutIndex].Red   = ColorToLUTEntry(red[i]);
            pLUTBuffer[lutIndex].Green = ColorToLUTEntry(green[i]);
            pLUTBuffer[lutIndex].Blue  = ColorToLUTEntry(blue[i]);
        }
        break;
    case 30:
        for (i = 0; i < nColorMapEntries; i++) {
            pLUTBuffer[i].Red   = ColorToLUTEntry(red[i]);
            pLUTBuffer[i].Green = ColorToLUTEntry(green[i]);
            pLUTBuffer[i].Blue  = ColorToLUTEntry(blue[i]);
        }
        break;
    default:
        nvAssert(!"invalid depth");
        return;
    }
}

static void
EvoPushSetLUTContextDmaMethodsForOneSd(NVDevEvoRec *pDevEvo,
                                       const NvU32 sd,
                                       const NvU32 head,
                                       const NvU32 ctxdma,
                                       NvBool enableBaseLut,
                                       const NvBool enableOutputLut,
                                       NVEvoUpdateState *updateState)
{
    NVEvoChannelPtr pChannel = pDevEvo->core;
    NvU64 offset;
    const NVSurfaceEvoRec *pCoreSurfaceEvo =
        pDevEvo->pSubDevices[sd]->pCoreChannelSurface[head];
    const NvBool surfaceDepth8 = (pCoreSurfaceEvo != NULL) ?
        (pCoreSurfaceEvo->format == NvKmsSurfaceMemoryFormatI8) : FALSE;

    nvAssert(nvPeekEvoSubDevMask(pDevEvo) == NVBIT(sd));

    // Depth 8 requires the base LUT to be enabled.
    if (ctxdma && !enableBaseLut && surfaceDepth8) {
        // TODO: Is this still required?  Callers should specify the LUT at
        // modeset time now.
        enableBaseLut = TRUE;
    }

    nvAssert(ctxdma || (!enableBaseLut && !enableOutputLut));

    nvUpdateUpdateState(pDevEvo, updateState, pChannel);

    /* Program the base LUT */

    offset = offsetof(NVEvoLutDataRec, base);
    nvAssert((offset & 0xff) == 0);

    nvDmaSetStartEvoMethod(pChannel, NV917D_HEAD_SET_BASE_LUT_LO(head), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_DEF(917D, _HEAD_SET_BASE_LUT_LO, _MODE, _INDEX_1025_UNITY_RANGE) |
        (enableBaseLut ? DRF_DEF(917D, _HEAD_SET_BASE_LUT_LO, _ENABLE, _ENABLE) :
                         DRF_DEF(917D, _HEAD_SET_BASE_LUT_LO, _ENABLE, _DISABLE)) |
        DRF_DEF(917D, _HEAD_SET_BASE_LUT_LO, _NEVER_YIELD_TO_BASE, _DISABLE));

    nvDmaSetStartEvoMethod(pChannel, NV917D_HEAD_SET_BASE_LUT_HI(head), 1);
    nvDmaSetEvoMethodData(pChannel,
            DRF_NUM(917D, _HEAD_SET_BASE_LUT_HI, _ORIGIN, offset >> 8));

    /* Program the output LUT */

    offset = offsetof(NVEvoLutDataRec, output);
    nvAssert((offset & 0xff) == 0);

    nvDmaSetStartEvoMethod(pChannel, NV917D_HEAD_SET_OUTPUT_LUT_LO(head), 1);
    nvDmaSetEvoMethodData(pChannel,
        (enableOutputLut ? DRF_DEF(917D, _HEAD_SET_OUTPUT_LUT_LO, _ENABLE, _ENABLE) :
                           DRF_DEF(917D, _HEAD_SET_OUTPUT_LUT_LO, _ENABLE, _DISABLE)) |
        DRF_DEF(917D, _HEAD_SET_OUTPUT_LUT_LO, _MODE, _INTERPOLATE_1025_UNITY_RANGE) |
        DRF_DEF(917D, _HEAD_SET_OUTPUT_LUT_LO, _NEVER_YIELD_TO_BASE, _DISABLE));

    nvDmaSetStartEvoMethod(pChannel, NV917D_HEAD_SET_OUTPUT_LUT_HI(head), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(917D, _HEAD_SET_OUTPUT_LUT_HI, _ORIGIN, offset >> 8));

    /* Set the ctxdma that's used by both LUTs */

    nvDmaSetStartEvoMethod(pChannel, NV917D_HEAD_SET_CONTEXT_DMA_LUT(head), 1);
    nvDmaSetEvoMethodData(pChannel,
            DRF_NUM(917D, _HEAD_SET_CONTEXT_DMA_LUT, _HANDLE, ctxdma));

    /*
     * Use this backdoor to disable "wide pipe" underreplication during
     * expansion of color components into the display pipe.  Underreplication
     * of a non-zero 8-bit color to more than 8 bits causes lookups to fall
     * between LUT entries in a 256-entry LUT, which we don't want.  See bug
     * 734919 for details.
     * The "wide pipe" may also cause scanout of 8-bit data to an 8-bit OR to
     * not be a straight passthrough (bug 895401).
     */
    nvDmaSetStartEvoMethod(pChannel, NV917D_HEAD_SET_CRC_CONTROL(head), 1);
    nvDmaSetEvoMethodData(pChannel,
            DRF_DEF(917D, _HEAD_SET_CRC_CONTROL, _PRIMARY_OUTPUT, _NONE) |
            DRF_DEF(917D, _HEAD_SET_CRC_CONTROL, _SECONDARY_OUTPUT, _NONE) |
            DRF_DEF(917D, _HEAD_SET_CRC_CONTROL, _WIDE_PIPE_CRC, _DISABLE));
}

static void EvoSetLUTContextDma90(const NVDispEvoRec *pDispEvo,
                                  const int head,
                                  NVLutSurfaceEvoPtr pLutSurfEvo,
                                  NvBool enableBaseLut,
                                  NvBool enableOutputLut,
                                  NVEvoUpdateState *updateState,
                                  NvBool bypassComposition)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    const NvU32 sd = pDispEvo->displayOwner;

    const NvBool coreChannelCtxDmaNonNull =
        pDevEvo->pSubDevices[sd]->pCoreChannelSurface[head] != NULL;
    const NvU32 ctxdma = (pLutSurfEvo != NULL) ? pLutSurfEvo->surfaceDesc.ctxDmaHandle : 0;

    /*
     * If the core channel doesn't have a scanout surface set, then setting the
     * LUT context DMA will cause an exception.
     */
    if (!coreChannelCtxDmaNonNull && ctxdma) {
        return;
    }

    nvPushEvoSubDevMask(pDevEvo, NVBIT(sd));
    EvoPushSetLUTContextDmaMethodsForOneSd(
        pDevEvo, sd, head, ctxdma, enableBaseLut, enableOutputLut,
        updateState);
    nvPopEvoSubDevMask(pDevEvo);
}

#define NV_EVO2_CAP_GET_PIN(cl, n, pEvoCaps, word, name, idx, pCaps) \
    (pEvoCaps)->pin[(idx)].flipLock = \
        FLD_TEST_DRF(cl##_CORE_NOTIFIER_##n, _CAPABILITIES_##word, \
                     _LOCK_PIN##name##USAGE, _FLIP_LOCK, \
                     (pCaps)[NV##cl##_CORE_NOTIFIER_##n##_CAPABILITIES_##word]); \
    (pEvoCaps)->pin[(idx)].stereo = \
        FLD_TEST_DRF(cl##_CORE_NOTIFIER_##n, _CAPABILITIES_##word, \
                     _LOCK_PIN##name##USAGE, _STEREO, \
                     (pCaps)[NV##cl##_CORE_NOTIFIER_##n##_CAPABILITIES_##word]); \
    (pEvoCaps)->pin[(idx)].scanLock = \
        FLD_TEST_DRF(cl##_CORE_NOTIFIER_##n, _CAPABILITIES_##word, \
                     _LOCK_PIN##name##USAGE, _SCAN_LOCK, \
                     (pCaps)[NV##cl##_CORE_NOTIFIER_##n##_CAPABILITIES_##word]);

/* Take the max of MAX_PIXELS_t_TAP422 and MAX_PIXELS_t_TAP444 */
#define NV_EVO2_CAP_GET_HEAD_MAX_PIXELS(cl, n, pEvoCaps, i, x, t, pCaps) \
    (pEvoCaps)->head[(i)].scalerCaps.taps[NV_EVO_SCALER_##t##TAPS].maxPixelsVTaps = \
        NV_MAX(REF_VAL(NV##cl##_CORE_NOTIFIER_##n##_CAPABILITIES_CAP_HEAD##i##_##x##_MAX_PIXELS##t##TAP422, \
               (pCaps)[NV##cl##_CORE_NOTIFIER_##n##_CAPABILITIES_CAP_HEAD##i##_##x]), \
               REF_VAL(NV##cl##_CORE_NOTIFIER_##n##_CAPABILITIES_CAP_HEAD##i##_##x##_MAX_PIXELS##t##TAP444, \
               (pCaps)[NV##cl##_CORE_NOTIFIER_##n##_CAPABILITIES_CAP_HEAD##i##_##x]))

#define NV_EVO2_CAP_GET_HEAD(cl, n, pEvoCaps, i, x, y, z, a, pCaps) \
    (pEvoCaps)->head[(i)].usable = TRUE; \
    (pEvoCaps)->head[(i)].maxPClkKHz = \
        DRF_VAL(cl##_CORE_NOTIFIER_##n, _CAPABILITIES_CAP_HEAD##i##_##a, _PCLK_MAX, \
                (pCaps)[NV##cl##_CORE_NOTIFIER_##n##_CAPABILITIES_CAP_HEAD##i##_##a]) * 10000; \
    (pEvoCaps)->head[(i)].scalerCaps.present = TRUE; \
    NV_EVO2_CAP_GET_HEAD_MAX_PIXELS(cl, n, pEvoCaps, i, x, 5, pCaps); \
    NV_EVO2_CAP_GET_HEAD_MAX_PIXELS(cl, n, pEvoCaps, i, y, 3, pCaps); \
    NV_EVO2_CAP_GET_HEAD_MAX_PIXELS(cl, n, pEvoCaps, i, z, 2, pCaps); \
    (pEvoCaps)->head[(i)].scalerCaps.taps[NV_EVO_SCALER_8TAPS].maxHDownscaleFactor = NV_U16_MAX; \
    (pEvoCaps)->head[(i)].scalerCaps.taps[NV_EVO_SCALER_5TAPS].maxVDownscaleFactor = NV_U16_MAX; \
    (pEvoCaps)->head[(i)].scalerCaps.taps[NV_EVO_SCALER_3TAPS].maxVDownscaleFactor = NV_U16_MAX; \
    (pEvoCaps)->head[(i)].scalerCaps.taps[NV_EVO_SCALER_2TAPS].maxVDownscaleFactor = NV_U16_MAX; \
    (pEvoCaps)->head[(i)].scalerCaps.taps[NV_EVO_SCALER_2TAPS].maxHDownscaleFactor = NV_U16_MAX;

#define NV_EVO2_CAP_GET_SOR(cl, n, pEvoCaps, i, x, y, pCaps) \
    (pEvoCaps)->sor[(i)].dualTMDS = \
        FLD_TEST_DRF(cl##_CORE_NOTIFIER_##n, _CAPABILITIES_CAP_SOR##i##_##x, \
                     _DUAL_TMDS, _TRUE, \
                     (pCaps)[NV##cl##_CORE_NOTIFIER_##n##_CAPABILITIES_CAP_SOR##i##_##x]); \
    (pEvoCaps)->sor[(i)].maxTMDSClkKHz = \
        DRF_VAL(cl##_CORE_NOTIFIER_##n, _CAPABILITIES_CAP_SOR##i##_##y, _TMDS_LVDS_CLK_MAX, \
                (pCaps)[NV##cl##_CORE_NOTIFIER_##n##_CAPABILITIES_CAP_SOR##i##_##y]) * 10000;

static void EvoParseCapabilityNotifier3(NVEvoCapabilitiesPtr pEvoCaps,
                                        volatile const NvU32 *pCaps)
{
    // Lock pins
    // These magic numbers (5, 6, _A, etc.) are token-pasted into the
    // NV917D_CORE_NOTIFIER_3_* macros and can't be autogenerated by the
    // preprocessor.  Architecture appears to have no plans to ever fix this.
    NV_EVO2_CAP_GET_PIN(917D, 3, pEvoCaps, 5,  0, 0x0, pCaps);
    NV_EVO2_CAP_GET_PIN(917D, 3, pEvoCaps, 5,  1, 0x1, pCaps);
    NV_EVO2_CAP_GET_PIN(917D, 3, pEvoCaps, 5,  2, 0x2, pCaps);
    NV_EVO2_CAP_GET_PIN(917D, 3, pEvoCaps, 5,  3, 0x3, pCaps);
    NV_EVO2_CAP_GET_PIN(917D, 3, pEvoCaps, 5,  4, 0x4, pCaps);
    NV_EVO2_CAP_GET_PIN(917D, 3, pEvoCaps, 5,  5, 0x5, pCaps);
    NV_EVO2_CAP_GET_PIN(917D, 3, pEvoCaps, 5,  6, 0x6, pCaps);
    NV_EVO2_CAP_GET_PIN(917D, 3, pEvoCaps, 5,  7, 0x7, pCaps);
    NV_EVO2_CAP_GET_PIN(917D, 3, pEvoCaps, 6,  8, 0x8, pCaps);
    NV_EVO2_CAP_GET_PIN(917D, 3, pEvoCaps, 6,  9, 0x9, pCaps);
    NV_EVO2_CAP_GET_PIN(917D, 3, pEvoCaps, 6, _A, 0xa, pCaps);
    NV_EVO2_CAP_GET_PIN(917D, 3, pEvoCaps, 6, _B, 0xb, pCaps);
    NV_EVO2_CAP_GET_PIN(917D, 3, pEvoCaps, 6, _C, 0xc, pCaps);
    NV_EVO2_CAP_GET_PIN(917D, 3, pEvoCaps, 6, _D, 0xd, pCaps);
    NV_EVO2_CAP_GET_PIN(917D, 3, pEvoCaps, 6, _E, 0xe, pCaps);
    NV_EVO2_CAP_GET_PIN(917D, 3, pEvoCaps, 6, _F, 0xf, pCaps);

    // Miscellaneous capabilities
    pEvoCaps->misc.supportsInterlaced = TRUE;
    pEvoCaps->misc.supportsSemiPlanar = FALSE;
    pEvoCaps->misc.supportsPlanar = FALSE;
    pEvoCaps->misc.supportsDSI = FALSE;

    // Heads
    NV_EVO2_CAP_GET_HEAD(917D, 3, pEvoCaps, 0, 53, 54, 55, 56, pCaps);
    NV_EVO2_CAP_GET_HEAD(917D, 3, pEvoCaps, 1, 61, 62, 63, 64, pCaps);
    NV_EVO2_CAP_GET_HEAD(917D, 3, pEvoCaps, 2, 69, 70, 71, 72, pCaps);
    NV_EVO2_CAP_GET_HEAD(917D, 3, pEvoCaps, 3, 77, 78, 79, 80, pCaps);

    // SORs
    NV_EVO2_CAP_GET_SOR(917D, 3, pEvoCaps, 0, 20, 21, pCaps);
    NV_EVO2_CAP_GET_SOR(917D, 3, pEvoCaps, 1, 22, 23, pCaps);
    NV_EVO2_CAP_GET_SOR(917D, 3, pEvoCaps, 2, 24, 25, pCaps);
    NV_EVO2_CAP_GET_SOR(917D, 3, pEvoCaps, 3, 26, 27, pCaps);
    NV_EVO2_CAP_GET_SOR(917D, 3, pEvoCaps, 4, 28, 29, pCaps);
    NV_EVO2_CAP_GET_SOR(917D, 3, pEvoCaps, 5, 30, 31, pCaps);
    NV_EVO2_CAP_GET_SOR(917D, 3, pEvoCaps, 6, 32, 33, pCaps);
    NV_EVO2_CAP_GET_SOR(917D, 3, pEvoCaps, 7, 34, 35, pCaps);

    // Don't need any PIOR caps currently.
}

static NvBool EvoGetCapabilities90(NVDevEvoPtr pDevEvo)
{
    NVEvoChannelPtr pChannel = pDevEvo->core;
    NVDispEvoPtr pDispEvo;
    unsigned int i, sd;
    struct NvKmsRRParams rrParams = { NVKMS_ROTATION_0, FALSE, FALSE };
    NvU8 layer;

    nvAssert(nvPeekEvoSubDevMask(pDevEvo) == SUBDEVICE_MASK_ALL);

    /* Main layer position and size updates are not supported on EVO. */
    for (layer = 0;
         layer < ARRAY_LEN(pDevEvo->caps.layerCaps);
         layer++) {
        if (layer != NVKMS_MAIN_LAYER) {
            pDevEvo->caps.layerCaps[layer].supportsWindowMode = TRUE;
            pDevEvo->caps.legacyNotifierFormatSizeBytes[layer] =
                NV_DISP_NOTIFICATION_2_SIZEOF;
        } else {
            pDevEvo->caps.layerCaps[layer].supportsWindowMode = FALSE;
            pDevEvo->caps.legacyNotifierFormatSizeBytes[layer] =
               NV_DISP_BASE_NOTIFIER_1_SIZEOF;
        }
    }

    pDevEvo->caps.cursorCompositionCaps =
        (struct NvKmsCompositionCapabilities) {
            .supportedColorKeySelects =
                NVBIT(NVKMS_COMPOSITION_COLOR_KEY_SELECT_DISABLE),

            .colorKeySelect = {
                [NVKMS_COMPOSITION_COLOR_KEY_SELECT_DISABLE] = {
                    .supportedBlendModes = {
                        [1] = NV_EVO2_SUPPORTED_CURSOR_COMP_BLEND_MODES,
                    },
                },
            }
        };

    /* Base doesn't support any composition with underlying layers. */
    pDevEvo->caps.layerCaps[NVKMS_MAIN_LAYER].composition =
        (struct NvKmsCompositionCapabilities) {
            .supportedColorKeySelects =
                NVBIT(NVKMS_COMPOSITION_COLOR_KEY_SELECT_DISABLE) |
                NVBIT(NVKMS_COMPOSITION_COLOR_KEY_SELECT_SRC),

            .colorKeySelect = {
                [NVKMS_COMPOSITION_COLOR_KEY_SELECT_DISABLE] = {
                    .supportedBlendModes = {
                        [1] = NVBIT(NVKMS_COMPOSITION_BLENDING_MODE_OPAQUE),
                    },
                },

                [NVKMS_COMPOSITION_COLOR_KEY_SELECT_SRC] = {
                    .supportedBlendModes = {
                        [0] = NVBIT(NVKMS_COMPOSITION_BLENDING_MODE_OPAQUE),
                        [1] = NVBIT(NVKMS_COMPOSITION_BLENDING_MODE_OPAQUE),
                    },
                },
            },
        };

    pDevEvo->caps.layerCaps[NVKMS_OVERLAY_LAYER].composition =
        (struct NvKmsCompositionCapabilities) {
            .supportedColorKeySelects =
                NVBIT(NVKMS_COMPOSITION_COLOR_KEY_SELECT_DISABLE) |
                NVBIT(NVKMS_COMPOSITION_COLOR_KEY_SELECT_SRC) |
                NVBIT(NVKMS_COMPOSITION_COLOR_KEY_SELECT_DST),

            .colorKeySelect = {
                [NVKMS_COMPOSITION_COLOR_KEY_SELECT_DISABLE] = {
                    .supportedBlendModes = {
                        [1] = NVBIT(NVKMS_COMPOSITION_BLENDING_MODE_OPAQUE),
                    },
                },

                [NVKMS_COMPOSITION_COLOR_KEY_SELECT_SRC] = {
                    .supportedBlendModes = {
                        [0] = NVBIT(NVKMS_COMPOSITION_BLENDING_MODE_OPAQUE),
                        [1] = NVBIT(NVKMS_COMPOSITION_BLENDING_MODE_TRANSPARENT),
                    },
                },

                [NVKMS_COMPOSITION_COLOR_KEY_SELECT_DST] = {
                    .supportedBlendModes = {
                        [0] = NVBIT(NVKMS_COMPOSITION_BLENDING_MODE_TRANSPARENT),
                        [1] = NVBIT(NVKMS_COMPOSITION_BLENDING_MODE_OPAQUE),
                    },
                },
            },
        };

    pDevEvo->caps.validLayerRRTransforms |=
        NVBIT(NvKmsRRParamsToCapBit(&rrParams));

    for (i = NvKmsSurfaceMemoryFormatMin;
         i <= NvKmsSurfaceMemoryFormatMax;
         i++) {
        if (nvHwFormatFromKmsFormat90(i) != 0) {
            pDevEvo->caps.layerCaps[NVKMS_MAIN_LAYER].supportedSurfaceMemoryFormats |=
                NVBIT64(i);
        }

        if (EvoOverlayFormatFromKmsFormat91(i) != 0) {
            pDevEvo->caps.layerCaps[NVKMS_OVERLAY_LAYER].supportedSurfaceMemoryFormats |=
                NVBIT64(i);
        }
    }

    EvoSetNotifierMethods90(pDevEvo,
                            pChannel,
                            TRUE /* notify */,
                            TRUE /* awaken */,
                            0    /* notifier */);

    /* Initialize the capability notifiers. */
    FOR_ALL_EVO_DISPLAYS(pDispEvo, i, pDevEvo) {
        nvWriteEvoCoreNotifier(pDispEvo, NV917D_CORE_NOTIFIER_3_CAPABILITIES_4,
                DRF_DEF(917D_CORE_NOTIFIER_3, _CAPABILITIES_4, _DONE, _FALSE));
    }

    /* Tell the hardware to fill in the notifier. */
    nvDmaSetStartEvoMethod(pChannel, NV917D_GET_CAPABILITIES, 1);
    nvDmaSetEvoMethodData(pChannel, 0);
    nvDmaKickoffEvo(pChannel);

    FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {

        NVEvoSubDevPtr pEvoSubDev;
        volatile NvU32 *pCaps;

        nvEvoWaitForCoreNotifier(pDispEvo, NV917D_CORE_NOTIFIER_3_CAPABILITIES_4,
                                 DRF_BASE(NV917D_CORE_NOTIFIER_3_CAPABILITIES_4_DONE),
                                 DRF_EXTENT(NV917D_CORE_NOTIFIER_3_CAPABILITIES_4_DONE),
                                 NV917D_CORE_NOTIFIER_3_CAPABILITIES_4_DONE_TRUE);

        pEvoSubDev = &pDevEvo->gpus[sd];
        pCaps = pDevEvo->core->notifiersDma[sd].subDeviceAddress[sd];

        nvkms_memset(&pEvoSubDev->capabilities, 0,
                     sizeof(pEvoSubDev->capabilities));
        EvoParseCapabilityNotifier3(&pEvoSubDev->capabilities, pCaps);
    }

    /* Reset notifier state so it isn't on for future updates */
    EvoSetNotifierMethods90(pDevEvo,
                            pChannel,
                            FALSE /* notify */,
                            FALSE /* awaken */,
                            0     /* notifier */);
    nvDmaKickoffEvo(pChannel);

    return TRUE;
}

static void EvoSetViewportPointIn90(NVDevEvoPtr pDevEvo, const int head,
                                    NvU16 x, NvU16 y,
                                    NVEvoUpdateState *updateState)
{
    NVEvoChannelPtr pChannel = pDevEvo->core;

    /* These methods should only apply to a single pDpy */
    nvAssert(pDevEvo->subDevMaskStackDepth > 0);

    nvUpdateUpdateState(pDevEvo, updateState, pChannel);

    // Set the input viewport point
    nvDmaSetStartEvoMethod(pChannel, NV917D_HEAD_SET_VIEWPORT_POINT_IN(head), 1);
    nvDmaSetEvoMethodData(pChannel, DRF_NUM(917D, _HEAD_SET_VIEWPORT_POINT_IN, _X, x) |
                          DRF_NUM(917D, _HEAD_SET_VIEWPORT_POINT_IN, _Y, y));
}

static void EvoSetOutputScaler90(const NVDispEvoRec *pDispEvo, const NvU32 head,
                                 const NvU32 imageSharpeningValue,
                                 NVEvoUpdateState *updateState)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NVEvoChannelPtr pChannel = pDevEvo->core;
    const NVDispHeadStateEvoRec *pHeadState = &pDispEvo->headState[head];
    const NVHwModeViewPortEvo *pViewPort = &pHeadState->timings.viewPort;
    NvU32 setControlOutputScaler = 0;
    NvU32 vTapsHw = 0, hTapsHw = 0;

    /* These methods should only apply to a single pDpyEvo */
    nvAssert(pDevEvo->subDevMaskStackDepth > 0);

    nvUpdateUpdateState(pDevEvo, updateState, pChannel);
    switch (pViewPort->vTaps) {
    case NV_EVO_SCALER_5TAPS:
        vTapsHw = NV917D_HEAD_SET_CONTROL_OUTPUT_SCALER_VERTICAL_TAPS_TAPS_5;
        break;
    case NV_EVO_SCALER_3TAPS:
        // XXX TAPS_3_ADAPTIVE instead? --> I think only allowed with interlaced
        vTapsHw = NV917D_HEAD_SET_CONTROL_OUTPUT_SCALER_VERTICAL_TAPS_TAPS_3;
        break;
    case NV_EVO_SCALER_2TAPS:
        vTapsHw = NV917D_HEAD_SET_CONTROL_OUTPUT_SCALER_VERTICAL_TAPS_TAPS_2;
        break;
    case NV_EVO_SCALER_8TAPS:
        nvAssert(!"Unknown pHeadState->vTaps");
        // fall through
    case NV_EVO_SCALER_1TAP:
        vTapsHw = NV917D_HEAD_SET_CONTROL_OUTPUT_SCALER_VERTICAL_TAPS_TAPS_1;
        break;
    }
    switch (pViewPort->hTaps) {
    case NV_EVO_SCALER_8TAPS:
        hTapsHw = NV917D_HEAD_SET_CONTROL_OUTPUT_SCALER_HORIZONTAL_TAPS_TAPS_8;
        break;
    case NV_EVO_SCALER_2TAPS:
        hTapsHw = NV917D_HEAD_SET_CONTROL_OUTPUT_SCALER_HORIZONTAL_TAPS_TAPS_2;
        break;
    case NV_EVO_SCALER_5TAPS:
    case NV_EVO_SCALER_3TAPS:
        nvAssert(!"Unknown pHeadState->hTaps");
        // fall through
    case NV_EVO_SCALER_1TAP:
        hTapsHw = NV917D_HEAD_SET_CONTROL_OUTPUT_SCALER_HORIZONTAL_TAPS_TAPS_1;
        break;
    }
    setControlOutputScaler =
        DRF_NUM(917D, _HEAD_SET_CONTROL_OUTPUT_SCALER, _HORIZONTAL_TAPS,
                hTapsHw) |
        DRF_NUM(917D, _HEAD_SET_CONTROL_OUTPUT_SCALER, _VERTICAL_TAPS,
                vTapsHw);

    if (nvIsImageSharpeningAvailable(&pHeadState->timings.viewPort)) {
        setControlOutputScaler =
            FLD_SET_DRF_NUM(917D, _HEAD_SET_CONTROL_OUTPUT_SCALER,
                            _HRESPONSE_BIAS, imageSharpeningValue,
                            setControlOutputScaler);

        setControlOutputScaler =
            FLD_SET_DRF_NUM(917D, _HEAD_SET_CONTROL_OUTPUT_SCALER,
                            _VRESPONSE_BIAS, imageSharpeningValue,
                            setControlOutputScaler);
    }

    nvDmaSetStartEvoMethod(pChannel, NV917D_HEAD_SET_CONTROL_OUTPUT_SCALER(head), 1);
    nvDmaSetEvoMethodData(pChannel, setControlOutputScaler);
}

static void EvoSetViewportInOut90(NVDevEvoPtr pDevEvo, const int head,
                                  const NVHwModeViewPortEvo *pViewPortMin,
                                  const NVHwModeViewPortEvo *pViewPort,
                                  const NVHwModeViewPortEvo *pViewPortMax,
                                  NVEvoUpdateState *updateState)
{
    NVEvoChannelPtr pChannel = pDevEvo->core;

    /* These methods should only apply to a single pDpy */
    nvAssert(pDevEvo->subDevMaskStackDepth > 0);

    nvUpdateUpdateState(pDevEvo, updateState, pChannel);

    /* The input viewport shouldn't vary. */
    nvAssert(pViewPortMin->in.width == pViewPort->in.width);
    nvAssert(pViewPortMax->in.width == pViewPort->in.width);
    nvAssert(pViewPortMin->in.height == pViewPort->in.height);
    nvAssert(pViewPortMax->in.height == pViewPort->in.height);
    nvDmaSetStartEvoMethod(pChannel, NV917D_HEAD_SET_VIEWPORT_SIZE_IN(head), 1);
    nvDmaSetEvoMethodData(pChannel,
            DRF_NUM(917D, _HEAD_SET_VIEWPORT_SIZE_IN, _WIDTH, pViewPort->in.width) |
            DRF_NUM(917D, _HEAD_SET_VIEWPORT_SIZE_IN, _HEIGHT, pViewPort->in.height));

    nvDmaSetStartEvoMethod(pChannel, NV917D_HEAD_SET_VIEWPORT_POINT_OUT_ADJUST(head), 1);
    nvDmaSetEvoMethodData(pChannel,
            DRF_NUM(917D, _HEAD_SET_VIEWPORT_POINT_OUT, _ADJUST_X, pViewPort->out.xAdjust) |
            DRF_NUM(917D, _HEAD_SET_VIEWPORT_POINT_OUT, _ADJUST_Y, pViewPort->out.yAdjust));

    nvDmaSetStartEvoMethod(pChannel, NV917D_HEAD_SET_VIEWPORT_SIZE_OUT(head), 1);
    nvDmaSetEvoMethodData(pChannel,
            DRF_NUM(917D, _HEAD_SET_VIEWPORT_SIZE_OUT, _WIDTH, pViewPort->out.width) |
            DRF_NUM(917D, _HEAD_SET_VIEWPORT_SIZE_OUT, _HEIGHT, pViewPort->out.height));

    nvDmaSetStartEvoMethod(pChannel, NV917D_HEAD_SET_VIEWPORT_SIZE_OUT_MIN(head), 1);
    nvDmaSetEvoMethodData(pChannel,
            DRF_NUM(917D, _HEAD_SET_VIEWPORT_SIZE_OUT_MIN, _WIDTH, pViewPortMin->out.width) |
            DRF_NUM(917D, _HEAD_SET_VIEWPORT_SIZE_OUT_MIN, _HEIGHT, pViewPortMin->out.height));

    nvDmaSetStartEvoMethod(pChannel, NV917D_HEAD_SET_VIEWPORT_SIZE_OUT_MAX(head), 1);
    nvDmaSetEvoMethodData(pChannel,
            DRF_NUM(917D, _HEAD_SET_VIEWPORT_SIZE_OUT_MAX, _WIDTH, pViewPortMax->out.width) |
            DRF_NUM(917D, _HEAD_SET_VIEWPORT_SIZE_OUT_MAX, _HEIGHT, pViewPortMax->out.height));

}


/*!
 * Compute the 917D_HEAD_SET_CONTROL_CURSOR method value.
 *
 * This function also validates that the given NVSurfaceEvoRec can be
 * used as a cursor image.
 *
 * Pre-nvdisplay core channel classes have the same layout of the
 * *7D_HEAD_SET_CONTROL_CURSOR method value.

 *
 * \param[in]  pDevEvo      The device on which the cursor will be programmed.
 * \param[in]  pSurfaceEvo  The surface to be used as the cursor image.
 * \param[out] pValue       The 917D_HEAD_SET_CONTROL_CURSOR method value.

 * \return  If TRUE, the surface can be used as a cursor image, and
 *          pValue contains the method value.  If FALSE, the surface
 *          cannot be used as a cursor image.
 */
NvBool nvEvoGetHeadSetControlCursorValue90(const NVDevEvoRec *pDevEvo,
                                           const NVSurfaceEvoRec *pSurfaceEvo,
                                           NvU32 *pValue)
{
    NvU32 value = 0;

    if (pSurfaceEvo == NULL) {
        value |= DRF_DEF(927D, _HEAD_SET_CONTROL_CURSOR, _ENABLE, _DISABLE);
        value |= DRF_DEF(927D, _HEAD_SET_CONTROL_CURSOR, _FORMAT, _A8R8G8B8);
        goto done;
    } else {
        value |= DRF_DEF(927D, _HEAD_SET_CONTROL_CURSOR, _ENABLE, _ENABLE);
    }

    /* The cursor must always be pitch. */

    if (pSurfaceEvo->layout != NvKmsSurfaceMemoryLayoutPitch) {
        return FALSE;
    }

    /*
     * The only supported cursor image memory format is A8R8G8B8.
     */
    if (pSurfaceEvo->format == NvKmsSurfaceMemoryFormatA8R8G8B8) {
        value |= DRF_DEF(927D, _HEAD_SET_CONTROL_CURSOR, _FORMAT, _A8R8G8B8);
    } else {
        return FALSE;
    }

    /*
     * The cursor only supports a few image sizes.
     */
    if ((pSurfaceEvo->widthInPixels == 32) &&
        (pSurfaceEvo->heightInPixels == 32)) {
        value |= DRF_DEF(927D, _HEAD_SET_CONTROL_CURSOR, _SIZE, _W32_H32);
    } else if ((pSurfaceEvo->widthInPixels == 64) &&
               (pSurfaceEvo->heightInPixels == 64)) {
        value |= DRF_DEF(927D, _HEAD_SET_CONTROL_CURSOR, _SIZE, _W64_H64);
    } else if ((pDevEvo->cursorHal->caps.maxSize >= 128) &&
               (pSurfaceEvo->widthInPixels == 128) &&
               (pSurfaceEvo->heightInPixels == 128)) {
        value |= DRF_DEF(927D, _HEAD_SET_CONTROL_CURSOR, _SIZE, _W128_H128);
    } else if ((pDevEvo->cursorHal->caps.maxSize >= 256) &&
               (pSurfaceEvo->widthInPixels == 256) &&
               (pSurfaceEvo->heightInPixels == 256)) {
        value |= DRF_DEF(927D, _HEAD_SET_CONTROL_CURSOR, _SIZE, _W256_H256);
    } else {
        return FALSE;
    }

    /*
     * Hard code the cursor hotspot.
     */
    value |= DRF_NUM(927D, _HEAD_SET_CONTROL_CURSOR, _HOT_SPOT_Y, 0);
    value |= DRF_NUM(927D, _HEAD_SET_CONTROL_CURSOR, _HOT_SPOT_X, 0);

done:

    if (pValue != NULL) {
        *pValue = value;
    }

    return TRUE;
}

static void EvoSetCursorImage(NVDevEvoPtr pDevEvo, const int head,
                              const NVSurfaceEvoRec *pSurfaceEvo,
                              NVEvoUpdateState *updateState,
                              const struct NvKmsCompositionParams *pCursorCompParams)
{
    NVEvoChannelPtr pChannel = pDevEvo->core;
    const NvU32 ctxdma = pSurfaceEvo ? pSurfaceEvo->planes[0].surfaceDesc.ctxDmaHandle : 0;
    const NvU64 offset = pSurfaceEvo ? pSurfaceEvo->planes[0].offset : 0;
    NvU32 headSetControlCursorValue = 0;
    NvBool ret;

    nvUpdateUpdateState(pDevEvo, updateState, pChannel);

    /* These methods should only apply to a single pDpy */
    nvAssert(pDevEvo->subDevMaskStackDepth > 0);
    nvAssert(pCursorCompParams->colorKeySelect ==
                NVKMS_COMPOSITION_COLOR_KEY_SELECT_DISABLE);
    nvAssert(NVBIT(pCursorCompParams->blendingMode[1]) &
                NV_EVO2_SUPPORTED_CURSOR_COMP_BLEND_MODES);
    nvAssert(!pSurfaceEvo || ctxdma);

    ret = nvEvoGetHeadSetControlCursorValue90(pDevEvo, pSurfaceEvo,
                                              &headSetControlCursorValue);
    /*
     * The caller should have already validated the surface, so there
     * shouldn't be a failure.
     */
    if (!ret) {
        nvAssert(!"Could not construct HEAD_SET_CONTROL_CURSOR value");
    }

    nvDmaSetStartEvoMethod(pChannel, NV917D_HEAD_SET_PRESENT_CONTROL_CURSOR(head), 1);
    nvDmaSetEvoMethodData(pChannel,
            DRF_DEF(917D, _HEAD_SET_PRESENT_CONTROL_CURSOR, _MODE, _MONO));

    nvDmaSetStartEvoMethod(pChannel, NV917D_HEAD_SET_OFFSETS_CURSOR(head, 0), 4);
    // The cursor has its own context DMA.
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(917D, _HEAD_SET_OFFSETS_CURSOR, _ORIGIN,
                nvCtxDmaOffsetFromBytes(offset)));
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(917D, _HEAD_SET_OFFSETS_CURSOR, _ORIGIN,
                nvCtxDmaOffsetFromBytes(offset)));
    nvDmaSetEvoMethodData(pChannel,
            DRF_NUM(917D, _HEAD_SET_CONTEXT_DMAS_CURSOR, _HANDLE, ctxdma));
    // Always set the right cursor context DMA.
    // HW will just ignore this if it is not in stereo cursor mode.
    nvDmaSetEvoMethodData(pChannel,
            DRF_NUM(917D, _HEAD_SET_CONTEXT_DMAS_CURSOR, _HANDLE, ctxdma));

    switch (pCursorCompParams->blendingMode[1]) {
    case NVKMS_COMPOSITION_BLENDING_MODE_NON_PREMULT_ALPHA:
        headSetControlCursorValue |=
            DRF_DEF(917D, _HEAD_SET_CONTROL_CURSOR, _COMPOSITION, _ALPHA_BLEND);
        break;
    case NVKMS_COMPOSITION_BLENDING_MODE_PREMULT_ALPHA:
        headSetControlCursorValue |=
            DRF_DEF(917D, _HEAD_SET_CONTROL_CURSOR, _COMPOSITION, _PREMULT_ALPHA_BLEND);
        break;
    default:
        nvEvoLogDevDebug(pDevEvo, EVO_LOG_ERROR,
            "%s: composition mode %d not supported for cursor",
            __func__, pCursorCompParams->blendingMode[1]);
    }

    nvDmaSetStartEvoMethod(pChannel, NV917D_HEAD_SET_CONTROL_CURSOR(head), 1);
    nvDmaSetEvoMethodData(pChannel, headSetControlCursorValue);
}

static void EvoSetCursorImage91(NVDevEvoPtr pDevEvo, const int head,
                                const NVSurfaceEvoRec *pSurfaceEvo,
                                NVEvoUpdateState *updateState,
                                const struct NvKmsCompositionParams *pCursorCompParams)
{
    NvU32 sd;

    for (sd = 0;  sd < pDevEvo->numSubDevices; sd++) {
        if (!((nvPeekEvoSubDevMask(pDevEvo) & (1 << sd)))) {
            continue;
        }

        /*
         * Set up the cursor surface: a cursor surface is allowed only if
         * there's a non-NULL ISO ctxdma.
         */
        if (pDevEvo->pSubDevices[sd]->pCoreChannelSurface[head] == NULL &&
            pSurfaceEvo != NULL) {
            continue;
        }

        nvPushEvoSubDevMask(pDevEvo, NVBIT(sd));
        EvoSetCursorImage(pDevEvo,
                          head,
                          pSurfaceEvo,
                          updateState,
                          pCursorCompParams);
        nvPopEvoSubDevMask(pDevEvo);
    }
}

static NvBool EvoValidateCursorSurface90(const NVDevEvoRec *pDevEvo,
                                         const NVSurfaceEvoRec *pSurfaceEvo)
{
    return nvEvoGetHeadSetControlCursorValue90(pDevEvo, pSurfaceEvo, NULL);
}

/*
 * The 'sourceFetchRect' parameter is ignored by this function because there are
 * no format-dependent restrictions for the source fetch rectangle on EVO.
 */
static NvBool EvoValidateWindowFormat90(
    const enum NvKmsSurfaceMemoryFormat format,
    const struct NvKmsRect *sourceFetchRect,
    NvU32 *hwFormatOut)
{
    const NvU32 hwFormat = nvHwFormatFromKmsFormat90(format);

    if (hwFormat == 0) {
        return FALSE;
    }

    if (hwFormatOut != NULL) {
        *hwFormatOut = hwFormat;
    }

    return TRUE;
}

static void EvoInitCompNotifier3(const NVDispEvoRec *pDispEvo, int idx)
{
    nvWriteEvoCoreNotifier(pDispEvo, NV917D_CORE_NOTIFIER_3_COMPLETION_0 + idx,
                           DRF_DEF(917D_CORE_NOTIFIER_3, _COMPLETION_0, _DONE, _FALSE));
}

static NvBool EvoIsCompNotifierComplete3(NVDispEvoPtr pDispEvo, int idx) {
    return nvEvoIsCoreNotifierComplete(pDispEvo,
                                       NV917D_CORE_NOTIFIER_3_COMPLETION_0 + idx,
                                       DRF_BASE(NV917D_CORE_NOTIFIER_3_COMPLETION_0_DONE),
                                       DRF_EXTENT(NV917D_CORE_NOTIFIER_3_COMPLETION_0_DONE),
                                       NV917D_CORE_NOTIFIER_3_COMPLETION_0_DONE_TRUE);
}

static void EvoWaitForCompNotifier3(const NVDispEvoRec *pDispEvo, int idx)
{
    nvEvoWaitForCoreNotifier(pDispEvo, NV917D_CORE_NOTIFIER_3_COMPLETION_0 + idx,
                             DRF_BASE(NV917D_CORE_NOTIFIER_3_COMPLETION_0_DONE),
                             DRF_EXTENT(NV917D_CORE_NOTIFIER_3_COMPLETION_0_DONE),
                             NV917D_CORE_NOTIFIER_3_COMPLETION_0_DONE_TRUE);
}

static void EvoSetDither91(NVDispEvoPtr pDispEvo, const int head,
                           const NvBool enabled, const NvU32 type,
                           const NvU32 algo,
                           NVEvoUpdateState *updateState)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NVEvoChannelPtr pChannel = pDevEvo->core;
    NvU32 ditherControl;

    nvUpdateUpdateState(pDevEvo, updateState, pChannel);

    if (enabled) {
        ditherControl = DRF_DEF(917D, _HEAD_SET_DITHER_CONTROL, _ENABLE, _ENABLE);

        switch (type) {
        case NV0073_CTRL_SPECIFIC_OR_DITHER_TYPE_6_BITS:
            ditherControl |=
                DRF_DEF(917D, _HEAD_SET_DITHER_CONTROL, _BITS, _DITHER_TO_6_BITS);
            break;
        case NV0073_CTRL_SPECIFIC_OR_DITHER_TYPE_8_BITS:
            ditherControl |=
                DRF_DEF(917D, _HEAD_SET_DITHER_CONTROL, _BITS, _DITHER_TO_8_BITS);
            break;
        default:
            nvAssert(!"Unknown ditherType");
            // Fall through
        case NV0073_CTRL_SPECIFIC_OR_DITHER_TYPE_OFF:
            ditherControl = NV917D_HEAD_SET_DITHER_CONTROL_ENABLE_DISABLE;
            break;
        }

    } else {
        ditherControl = DRF_DEF(917D, _HEAD_SET_DITHER_CONTROL, _ENABLE, _DISABLE);
    }

    switch (algo) {
    case NV0073_CTRL_SPECIFIC_OR_DITHER_ALGO_STATIC_ERR_ACC:
        ditherControl |=
            DRF_DEF(917D, _HEAD_SET_DITHER_CONTROL, _MODE, _STATIC_ERR_ACC);
        break;
    case NV0073_CTRL_SPECIFIC_OR_DITHER_ALGO_DYNAMIC_2X2:
        ditherControl |=
            DRF_DEF(917D, _HEAD_SET_DITHER_CONTROL, _MODE, _DYNAMIC_2X2);
        break;
    case NV0073_CTRL_SPECIFIC_OR_DITHER_ALGO_STATIC_2X2:
        ditherControl |=
            DRF_DEF(917D, _HEAD_SET_DITHER_CONTROL, _MODE, _STATIC_2X2);
        break;
    case NV0073_CTRL_SPECIFIC_OR_DITHER_ALGO_TEMPORAL:
        ditherControl |=
            DRF_DEF(917D, _HEAD_SET_DITHER_CONTROL, _MODE, _TEMPORAL);
        break;
    default:
        nvAssert(!"Unknown DitherAlgo");
        // Fall through
    case NV0073_CTRL_SPECIFIC_OR_DITHER_ALGO_UNKNOWN:
    case NV0073_CTRL_SPECIFIC_OR_DITHER_ALGO_DYNAMIC_ERR_ACC:
        ditherControl |=
            DRF_DEF(917D, _HEAD_SET_DITHER_CONTROL, _MODE, _DYNAMIC_ERR_ACC);
        break;
    }

    nvDmaSetStartEvoMethod(pChannel, NV917D_HEAD_SET_DITHER_CONTROL(head), 1);
    nvDmaSetEvoMethodData(pChannel, ditherControl);
}

static void EvoSetStallLock94(NVDispEvoPtr pDispEvo, const int head,
                              NvBool enable, NVEvoUpdateState *updateState)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NVEvoChannelPtr pChannel = pDevEvo->core;

    nvUpdateUpdateState(pDevEvo, updateState, pChannel);

    if (enable) {
        nvDmaSetStartEvoMethod(pChannel, NV947D_HEAD_SET_STALL_LOCK(head), 1);
        nvDmaSetEvoMethodData(pChannel,
            DRF_DEF(947D, _HEAD_SET_STALL_LOCK, _ENABLE, _TRUE) |
            DRF_DEF(947D, _HEAD_SET_STALL_LOCK, _MODE, _ONE_SHOT) |
            DRF_DEF(947D, _HEAD_SET_STALL_LOCK, _LOCK_PIN, _UNSPECIFIED) |
            DRF_DEF(947D, _HEAD_SET_STALL_LOCK, _UNSTALL_MODE, _LINE_LOCK));
    } else {
        nvDmaSetStartEvoMethod(pChannel, NV947D_HEAD_SET_STALL_LOCK(head), 1);
        nvDmaSetEvoMethodData(pChannel,
            DRF_DEF(947D, _HEAD_SET_STALL_LOCK, _ENABLE, _FALSE));
    }
}

static NvBool ForceIdleBaseChannel(
    NVDevEvoPtr pDevEvo,
    NVEvoChannelPtr pChannel,
    NvU32 sd)
{
    NV5070_CTRL_CMD_STOP_BASE_PARAMS stopParams = { };
    NvNotification *pNotifyData = pChannel->notifiersDma[sd].subDeviceAddress[sd];
    NvU64 startTime = 0;
    const NvU32 timeout = 2000000; // 2 seconds
    NvU32 ret;

    nvAssert((pChannel->channelMask & NV_EVO_CHANNEL_MASK_BASE_ALL) != 0);

    pNotifyData->status = NV5070_NOTIFICATION_STATUS_IN_PROGRESS;

    stopParams.base.subdeviceIndex = sd;
    stopParams.channelInstance = pChannel->instance;
    stopParams.notifyMode = NV5070_CTRL_CMD_STOP_BASE_NOTIFY_MODE_WRITE;
    stopParams.hNotifierCtxDma = pChannel->notifiersDma[sd].surfaceDesc.ctxDmaHandle;
    stopParams.offset = 0;
    stopParams.hEvent = 0;

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->displayHandle,
                         NV5070_CTRL_CMD_STOP_BASE,
                         &stopParams, sizeof(stopParams));
    if (ret != NVOS_STATUS_SUCCESS) {
        nvAssert(!"STOP_BASE failed");
        return FALSE;
    }

    do {
        if (pNotifyData->status == NV5070_NOTIFICATION_STATUS_DONE_SUCCESS) {
            break;
        }

        if (nvExceedsTimeoutUSec(pDevEvo, &startTime, timeout)) {
            nvAssert(!"STOP_BASE timed out");
            return FALSE;
        }

        nvkms_yield();

    } while (TRUE);

    return TRUE;
}

static NvBool ForceIdleOverlayChannel(
    NVDevEvoPtr pDevEvo,
    NVEvoChannelPtr pChannel,
    NvU32 sd)
{
    NV5070_CTRL_CMD_STOP_OVERLAY_PARAMS stopParams = { };
    NvNotification *pNotifyData = pChannel->notifiersDma[sd].subDeviceAddress[sd];
    NvU64 startTime = 0;
    const NvU32 timeout = 2000000; // 2 seconds
    NvU32 ret;

    nvAssert((pChannel->channelMask & NV_EVO_CHANNEL_MASK_OVERLAY_ALL) != 0);

    pNotifyData->status = NV5070_NOTIFICATION_STATUS_IN_PROGRESS;

    stopParams.base.subdeviceIndex = sd;
    stopParams.channelInstance = pChannel->instance;
    stopParams.notifyMode = NV5070_CTRL_CMD_STOP_OVERLAY_NOTIFY_MODE_WRITE;
    stopParams.hNotifierCtxDma = pChannel->notifiersDma[sd].surfaceDesc.ctxDmaHandle;
    stopParams.offset = 0;
    stopParams.hEvent = 0;

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->displayHandle,
                         NV5070_CTRL_CMD_STOP_OVERLAY,
                         &stopParams, sizeof(stopParams));
    if (ret != NVOS_STATUS_SUCCESS) {
        nvAssert(!"STOP_OVERLAY failed");
        return FALSE;
    }

    do {
        if (pNotifyData->status == NV5070_NOTIFICATION_STATUS_DONE_SUCCESS) {
            break;
        }

        if (nvExceedsTimeoutUSec(pDevEvo, &startTime, timeout)) {
            nvAssert(!"STOP_OVERLAY timed out");
            return FALSE;
        }

        nvkms_yield();

    } while (TRUE);

    return TRUE;
}

static NvBool EvoForceIdleSatelliteChannel90(
    NVDevEvoPtr pDevEvo,
    const NVEvoIdleChannelState *idleChannelState)
{
    NvU32 head, sd;
    NvBool ret = TRUE;

    for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
        /*
         * Forcing a channel to be idle is currently only implemented for
         * base.
         */
        if ((idleChannelState->subdev[sd].channelMask &
             ~(NV_EVO_CHANNEL_MASK_BASE_ALL |
               NV_EVO_CHANNEL_MASK_OVERLAY_ALL)) != 0) {

            nvEvoLogDevDebug(pDevEvo, EVO_LOG_ERROR,
                "Forcing channel idle only implemented for base and overlay");
            return FALSE;
        }

        for (head = 0; head < pDevEvo->numHeads; head++) {
            const NVEvoChannelMask thisBaseMask =
                DRF_IDX_DEF64(_EVO, _CHANNEL_MASK, _BASE, head, _ENABLE);
            const NVEvoChannelMask thisOverlayMask =
                DRF_IDX_DEF64(_EVO, _CHANNEL_MASK, _OVERLAY, head, _ENABLE);

            if (idleChannelState->subdev[sd].channelMask &
                thisBaseMask) {

                NVEvoChannelPtr pBaseChannel = pDevEvo->base[head];

                if (!ForceIdleBaseChannel(pDevEvo, pBaseChannel, sd)) {
                    ret = FALSE;
                }
            }

            if (idleChannelState->subdev[sd].channelMask &
                thisOverlayMask) {

                NVEvoChannelPtr pOverlayChannel = pDevEvo->overlay[head];

                if (!ForceIdleOverlayChannel(pDevEvo, pOverlayChannel, sd)) {
                    ret = FALSE;
                }
            }
        }
    }

    return ret;
}

static NvBool EvoAllocRmCtrlObject90(NVDevEvoPtr pDevEvo)
{
    /* Nothing to do for pre-nvdisplay */
    return TRUE;
}

static void EvoFreeRmCtrlObject90(NVDevEvoPtr pDevEvo)
{
    /* Nothing to do for pre-nvdisplay */
}

static void EvoSetImmPointOut91(NVDevEvoPtr pDevEvo,
                                NVEvoChannelPtr pChannel,
                                NvU32 sd,
                                NVEvoUpdateState *updateState,
                                NvU16 x, NvU16 y)
{
    GK104DispOverlayImmControlPio *pOverlayImm =
        pChannel->imm.u.pio->control[sd];

    /* The only immediate channel we have is overlay. */
    nvAssert((pChannel->channelMask & NV_EVO_CHANNEL_MASK_OVERLAY_ALL) != 0);
    nvAssert(pChannel->imm.type == NV_EVO_IMM_CHANNEL_PIO);
    nvAssert(pOverlayImm != NULL);

    /* Left eye */
    pOverlayImm->SetPointsOut[0] =
        DRF_NUM(917B, _SET_POINTS_OUT, _X, x) |
        DRF_NUM(917B, _SET_POINTS_OUT, _Y, y);

    pOverlayImm->Update =
        DRF_DEF(917B, _UPDATE, _INTERLOCK_WITH_CORE, _DISABLE);
}

static void EvoStartHeadCRC32Capture90(NVDevEvoPtr pDevEvo,
                                       NVEvoDmaPtr pDma,
                                       NVConnectorEvoPtr pConnectorEvo,
                                       const enum nvKmsTimingsProtocol protocol,
                                       const NvU32 orIndex,
                                       NvU32 head,
                                       NvU32 sd,
                                       NVEvoUpdateState *updateState)
{
    NVEvoChannelPtr pChannel = pDevEvo->core;
    NvU32 dmaCtx = pDma->surfaceDesc.ctxDmaHandle;
    NvU32 orOutput = 0;

    /* These method should only apply to a single pDpy */
    nvAssert(pDevEvo->subDevMaskStackDepth > 0);

    nvUpdateUpdateState(pDevEvo, updateState, pChannel);

    switch (pConnectorEvo->or.type) {
    case NV0073_CTRL_SPECIFIC_OR_TYPE_DAC:
        orOutput =
            NV917D_HEAD_SET_CRC_CONTROL_PRIMARY_OUTPUT_DAC(orIndex);
        break;
    case NV0073_CTRL_SPECIFIC_OR_TYPE_SOR:
        if (protocol == NVKMS_PROTOCOL_SOR_DP_A ||
            protocol == NVKMS_PROTOCOL_SOR_DP_B) {
            orOutput =
                NV917D_HEAD_SET_CRC_CONTROL_PRIMARY_OUTPUT_SF(head);
        } else {
            orOutput =
                NV917D_HEAD_SET_CRC_CONTROL_PRIMARY_OUTPUT_SOR(orIndex);
        }
        break;
    case NV0073_CTRL_SPECIFIC_OR_TYPE_PIOR:
        orOutput =
            NV917D_HEAD_SET_CRC_CONTROL_PRIMARY_OUTPUT_PIOR(orIndex);
        break;
    default:
        nvAssert(!"Invalid pConnectorEvo->or.type");
        break;
    }

    nvDmaSetStartEvoMethod(pChannel, NV917D_HEAD_SET_CONTEXT_DMA_CRC(head), 1);
    nvDmaSetEvoMethodData(pChannel,
            DRF_NUM(917D, _HEAD_SET_CONTEXT_DMA_CRC, _HANDLE, dmaCtx));

    nvDmaSetStartEvoMethod(pChannel, NV917D_HEAD_SET_CRC_CONTROL(head), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(917D, _HEAD_SET_CRC_CONTROL, _PRIMARY_OUTPUT, orOutput) |
        DRF_DEF(917D, _HEAD_SET_CRC_CONTROL, _SECONDARY_OUTPUT, _NONE) |
        DRF_DEF(917D, _HEAD_SET_CRC_CONTROL, _CONTROLLING_CHANNEL, _CORE) |
        DRF_DEF(917D, _HEAD_SET_CRC_CONTROL, _EXPECT_BUFFER_COLLAPSE, _FALSE) |
        DRF_DEF(917D, _HEAD_SET_CRC_CONTROL, _TIMESTAMP_MODE, _FALSE) |
        DRF_DEF(917D, _HEAD_SET_CRC_CONTROL, _FLIPLOCK_MODE, _FALSE) |
        DRF_DEF(917D, _HEAD_SET_CRC_CONTROL, _CRC_DURING_SNOOZE, _DISABLE));

    /* Reset the CRC notifier */
    nvEvoResetCRC32Notifier(pDma->subDeviceAddress[sd],
                            NV917D_NOTIFIER_CRC_1_STATUS_0,
                            DRF_BASE(NV917D_NOTIFIER_CRC_1_STATUS_0_DONE),
                            NV917D_NOTIFIER_CRC_1_STATUS_0_DONE_FALSE);
}

static void EvoStopHeadCRC32Capture90(NVDevEvoPtr pDevEvo,
                                      NvU32 head,
                                      NVEvoUpdateState *updateState)
{
    NVEvoChannelPtr pChannel = pDevEvo->core;

    /* These method should only apply to a single pDpy */
    nvAssert(pDevEvo->subDevMaskStackDepth > 0);

    nvUpdateUpdateState(pDevEvo, updateState, pChannel);

    nvDmaSetStartEvoMethod(pChannel, NV917D_HEAD_SET_CONTEXT_DMA_CRC(head), 1);
    nvDmaSetEvoMethodData(pChannel, 0);

    nvDmaSetStartEvoMethod(pChannel, NV917D_HEAD_SET_CRC_CONTROL(head), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_DEF(917D, _HEAD_SET_CRC_CONTROL, _PRIMARY_OUTPUT, _NONE) |
        DRF_DEF(917D, _HEAD_SET_CRC_CONTROL, _SECONDARY_OUTPUT, _NONE) |
        DRF_DEF(917D, _HEAD_SET_CRC_CONTROL, _CONTROLLING_CHANNEL, _CORE) |
        DRF_DEF(917D, _HEAD_SET_CRC_CONTROL, _EXPECT_BUFFER_COLLAPSE, _FALSE) |
        DRF_DEF(917D, _HEAD_SET_CRC_CONTROL, _TIMESTAMP_MODE, _FALSE) |
        DRF_DEF(917D, _HEAD_SET_CRC_CONTROL, _FLIPLOCK_MODE, _FALSE) |
        DRF_DEF(917D, _HEAD_SET_CRC_CONTROL, _CRC_DURING_SNOOZE, _DISABLE));
}

/*!
 * Queries the current head's CRC Notifier and returns values if successful
 *
 * First waits for hardware to finish writing to the CRC32Notifier,
 * and performs a read of the Compositor and SF/OR CRCs in numCRC32 frames
 * Crc fields in input array crc32 should be calloc'd to 0s.
 *
 * \param[in]  pDevEvo          NVKMS device pointer
 * \param[in]  pDma             Pointer to DMA-mapped memory
 * \param[in]  sd               Subdevice index
 * \param[in]  entry_count      Number of independent frames to read CRCs from
 * \param[out] crc32            Contains pointers to CRC output arrays
 * \param[out] numCRC32         Number of CRC frames successfully read from DMA
 *
 * \return  Returns TRUE if was able to successfully read CRCs from DMA,
 *          otherwise FALSE
 */
static NvBool EvoQueryHeadCRC32_90(NVDevEvoPtr pDevEvo,
                                   NVEvoDmaPtr pDma,
                                   NvU32 sd,
                                   NvU32 entry_count,
                                   CRC32NotifierCrcOut *crc32,
                                   NvU32 *numCRC32)
{
    volatile NvU32 *pCRC32Notifier = pDma->subDeviceAddress[sd];
    const NvU32 entry_stride =
          NV917D_NOTIFIER_CRC_1_CRC_ENTRY1_8 - NV917D_NOTIFIER_CRC_1_CRC_ENTRY0_4;
    // Define how many/which variables to read from each CRCNotifierEntry struct
    const CRC32NotifierEntryRec field_info[NV_EVO2_NUM_CRC_FIELDS] = {
        {
            .field_offset = NV917D_NOTIFIER_CRC_1_CRC_ENTRY0_3,
            .field_base_bit =
             DRF_BASE(NV917D_NOTIFIER_CRC_1_CRC_ENTRY0_3_COMPOSITOR_CRC),
            .field_extent_bit =
             DRF_EXTENT(NV917D_NOTIFIER_CRC_1_CRC_ENTRY0_3_COMPOSITOR_CRC),
            .field_frame_values = crc32->compositorCrc32
        },
        {
            .field_offset = NV917D_NOTIFIER_CRC_1_CRC_ENTRY0_4,
            .field_base_bit =
             DRF_BASE(NV917D_NOTIFIER_CRC_1_CRC_ENTRY0_4_PRIMARY_OUTPUT_CRC),
            .field_extent_bit =
             DRF_EXTENT(NV917D_NOTIFIER_CRC_1_CRC_ENTRY0_4_PRIMARY_OUTPUT_CRC),
            .field_frame_values = crc32->outputCrc32
        }
    };
    const CRC32NotifierEntryFlags flag_info[NV_EVO2_NUM_CRC_FLAGS] = {
        {
            .flag_base_bit =
             DRF_BASE(NV917D_NOTIFIER_CRC_1_STATUS_0_COUNT),
            .flag_extent_bit =
             DRF_EXTENT(NV917D_NOTIFIER_CRC_1_STATUS_0_COUNT),
            .flag_type = NVEvoCrc32NotifierFlagCount
        },
        {
            .flag_base_bit =
             DRF_BASE(NV917D_NOTIFIER_CRC_1_STATUS_0_COMPOSITOR_OVERFLOW),
            .flag_extent_bit =
             DRF_EXTENT(NV917D_NOTIFIER_CRC_1_STATUS_0_COMPOSITOR_OVERFLOW),
            .flag_type = NVEvoCrc32NotifierFlagCrcOverflow
        },
        {
            .flag_base_bit =
             DRF_BASE(NV917D_NOTIFIER_CRC_1_STATUS_0_PRIMARY_OUTPUT_OVERFLOW),
            .flag_extent_bit =
             DRF_EXTENT(NV917D_NOTIFIER_CRC_1_STATUS_0_PRIMARY_OUTPUT_OVERFLOW),
            .flag_type = NVEvoCrc32NotifierFlagCrcOverflow
        }
    };

    if (!nvEvoWaitForCRC32Notifier(pDevEvo,
                                   pCRC32Notifier,
                                   NV917D_NOTIFIER_CRC_1_STATUS_0,
                                   DRF_BASE(NV917D_NOTIFIER_CRC_1_STATUS_0_DONE),
                                   DRF_EXTENT(NV917D_NOTIFIER_CRC_1_STATUS_0_DONE),
                                   NV917D_NOTIFIER_CRC_1_STATUS_0_DONE_TRUE)) {
        return FALSE;
    }

    *numCRC32 = nvEvoReadCRC32Notifier(pCRC32Notifier,
                                       entry_stride,
                                       entry_count,
                                       NV917D_NOTIFIER_CRC_1_STATUS_0, /* Status offset */
                                       NV_EVO2_NUM_CRC_FIELDS,
                                       NV_EVO2_NUM_CRC_FLAGS,
                                       field_info,
                                       flag_info);


    nvEvoResetCRC32Notifier(pCRC32Notifier,
                            NV917D_NOTIFIER_CRC_1_STATUS_0,
                            DRF_BASE(NV917D_NOTIFIER_CRC_1_STATUS_0_DONE),
                            NV917D_NOTIFIER_CRC_1_STATUS_0_DONE_FALSE);

    return TRUE;
}

static void EvoGetScanLine90(const NVDispEvoRec *pDispEvo,
                             const NvU32 head,
                             NvU16 *pScanLine,
                             NvBool *pInBlankingPeriod)
{
    const NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    const NvU32 sd = pDispEvo->displayOwner;
    const void *pDma = pDevEvo->base[head]->pb.control[sd];
    NvU32 scanLine = nvDmaLoadPioMethod(pDma, NV917C_GET_SCANLINE);

    /*
     * This method immediately returns the value of the scanline currently being
     * read by the DMI.  This method is a channel method so it operates
     * completely asynchronously from the processing of methods in the
     * pushbuffer. A negative value indicate that the DMI is in vertical
     * blanking. Note that this is a PIO method that executes immediately. The
     * coding of this value is as follows:
     * If Line[15] == 0 (positive value)
     *   then Line[14:0] is the post-aa resolved line currently being read by
     *   the DMI.
     * If Line[15] == 1 (negative value)
     *   then Line[14:0] is the number of microseconds remaining in the vertical
     *   blanking interval.
     * Examples:
     *   Line = 0x0192 - DMI is reading line 402 of the current buffer.
     *   Line = 0x8023 - DMI is 35 uS from the end of vertical blanking.
     */

    if ((scanLine & NVBIT(15)) == 0) {
        *pInBlankingPeriod = FALSE;
        *pScanLine = scanLine & DRF_MASK(14:0);
    } else {
        *pInBlankingPeriod = TRUE;
    }
}

static NvU32 EvoGetActiveViewportOffset94(NVDispEvoRec *pDispEvo, NvU32 head)
{
    NV5070_CTRL_CMD_GET_ACTIVE_VIEWPORT_BASE_PARAMS params = { };
    NvU32 ret;
    NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;

    params.base.subdeviceIndex = pDispEvo->displayOwner;
    params.head = head;

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->displayHandle,
                         NV5070_CTRL_CMD_GET_ACTIVE_VIEWPORT_BASE,
                         &params, sizeof(params));

    if (ret != NVOS_STATUS_SUCCESS) {
        nvEvoLogDevDebug(pDevEvo, EVO_LOG_ERROR,
                         "Failed to query active viewport offset");
    }

    return params.activeViewportBase;
}

static void
EvoClearSurfaceUsage91(NVDevEvoPtr pDevEvo, NVSurfaceEvoPtr pSurfaceEvo)
{
    NvU32 sd;
    NvBool kickOff = FALSE;
    NVEvoUpdateState updateState = { };

    for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
        NvU32 head;

        for (head = 0; head < pDevEvo->numHeads; head++) {
            const struct NvKmsCscMatrix zeroCscMatrix = { };
            const NVEvoSubDevHeadStateRec *pSdHeadState =
                &pDevEvo->gpus[sd].headState[head];

            /*
             * In background, if the given surface is used for the core surface
             * programming to satisfy the EVO hardware constraints then clear
             * that usage. Reuse the client specified base surface for the core
             * channel programming.
             */
            if (pSurfaceEvo !=
                pDevEvo->pSubDevices[sd]->pCoreChannelSurface[head]) {
                continue;
            }

            nvPushEvoSubDevMask(pDevEvo, NVBIT(sd));
            EvoPushSetCoreSurfaceMethodsForOneSd(pDevEvo, sd, head,
                pSdHeadState->layer[NVKMS_MAIN_LAYER].pSurfaceEvo[NVKMS_LEFT],
                &zeroCscMatrix, &updateState);
            nvPopEvoSubDevMask(pDevEvo);
            kickOff = TRUE;
        }
    }

    if (kickOff) {
        EvoUpdate91(pDevEvo, &updateState, TRUE /* releaseElv */);
    }
}

static NvBool EvoComputeWindowScalingTaps91(const NVDevEvoRec *pDevEvo,
                                            const NVEvoChannel *pChannel,
                                            NVFlipChannelEvoHwState *pHwState)
{
    /* Window scaling isn't supported on EVO. */
    if ((pHwState->sizeIn.width != pHwState->sizeOut.width) ||
        (pHwState->sizeIn.height != pHwState->sizeOut.height))
    {
        return FALSE;
    }

    pHwState->hTaps = NV_EVO_SCALER_1TAP;
    pHwState->vTaps = NV_EVO_SCALER_1TAP;

    return TRUE;
}

static NvU32 GetAccelerators(
    NVDevEvoPtr pDevEvo,
    NVEvoChannelPtr pChannel,
    NvU32 sd)
{
    NV5070_CTRL_GET_ACCL_PARAMS params = { };
    NvU32 ret;

    params.base.subdeviceIndex = sd;
    params.channelClass = pChannel->hwclass;
    nvAssert(pChannel->channelMask & NV_EVO_CHANNEL_MASK_BASE_ALL);
    params.channelInstance = pChannel->instance;

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->displayHandle,
                         NV5070_CTRL_CMD_GET_ACCL,
                         &params, sizeof(params));
    if (ret != NVOS_STATUS_SUCCESS) {
        nvEvoLogDevDebug(pDevEvo, EVO_LOG_ERROR,
                         "Failed to retrieve accelerators");
        return 0;
    }

    return params.accelerators;
}

static NvBool SetAccelerators(
    NVDevEvoPtr pDevEvo,
    NVEvoChannelPtr pChannel,
    NvU32 sd,
    NvU32 accelerators,
    NvU32 accelMask)
{
    NV5070_CTRL_SET_ACCL_PARAMS params = { };
    NvU32 ret;

    params.base.subdeviceIndex = sd;
    params.channelClass = pChannel->hwclass;
    nvAssert(pChannel->channelMask & NV_EVO_CHANNEL_MASK_BASE_ALL);
    params.channelInstance = pChannel->instance;
    params.accelerators = accelerators;
    params.accelMask = accelMask;

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->displayHandle,
                         NV5070_CTRL_CMD_SET_ACCL,
                         &params, sizeof(params));
    if (ret != NVOS_STATUS_SUCCESS) {
        nvEvoLogDevDebug(pDevEvo, EVO_LOG_ERROR,
                         "Failed to set accelerators");
        return FALSE;
    }

    return TRUE;
}

static void EvoAccelerateChannel91(NVDevEvoPtr pDevEvo,
                                   NVEvoChannelPtr pChannel,
                                   const NvU32 sd,
                                   const NvBool trashPendingMethods,
                                   const NvBool unblockMethodsInExecutation,
                                   NvU32 *pOldAccelerators)
{
    NvU32 accelMask = 0x0;

    if (trashPendingMethods) {
        accelMask |= NV5070_CTRL_ACCL_TRASH_ONLY;
    }

    /* Start with a conservative set of accelerators; may need to add more
     * later. */
    if (unblockMethodsInExecutation) {
        accelMask |= NV5070_CTRL_ACCL_IGNORE_PI |
                     NV5070_CTRL_ACCL_SKIP_SEMA |
                     NV5070_CTRL_ACCL_IGNORE_FLIPLOCK;
    }

    if (accelMask == 0x0) {
        return;
    }

    *pOldAccelerators = GetAccelerators(pDevEvo, pChannel, sd);

    /* Accelerate window channel. */
    if (!SetAccelerators(pDevEvo, pChannel, sd, accelMask, accelMask)) {
        nvAssert(!"Failed to set accelerators");
    }
}

static void EvoResetChannelAccelerators91(NVDevEvoPtr pDevEvo,
                                          NVEvoChannelPtr pChannel,
                                          const NvU32 sd,
                                          const NvBool trashPendingMethods,
                                          const NvBool unblockMethodsInExecutation,
                                          NvU32 oldAccelerators)
{
    NvU32 accelMask = 0x0;

    if (trashPendingMethods) {
        accelMask |= NV5070_CTRL_ACCL_TRASH_ONLY;
    }

    /* Start with a conservative set of accelerators; may need to add more
     * later. */
    if (unblockMethodsInExecutation) {
        accelMask |= NV5070_CTRL_ACCL_IGNORE_PI |
                     NV5070_CTRL_ACCL_SKIP_SEMA |
                     NV5070_CTRL_ACCL_IGNORE_FLIPLOCK;
    }

    if (accelMask == 0x0) {
        return;
    }

    /* Accelerate window channel. */
    if (!SetAccelerators(pDevEvo, pChannel, sd, oldAccelerators, accelMask)) {
        nvAssert(!"Failed to set accelerators");
    }
}

static NvU32 EvoAllocSurfaceDescriptor90(
    NVDevEvoPtr pDevEvo, NVSurfaceDescriptor *pSurfaceDesc,
    NvU32 memoryHandle, NvU32 localCtxDmaFlags,
    NvU64 limit)
{
    return nvCtxDmaAlloc(pDevEvo, &pSurfaceDesc->ctxDmaHandle,
                         memoryHandle,
                         localCtxDmaFlags, limit);
}

static void EvoFreeSurfaceDescriptor90(
    NVDevEvoPtr pDevEvo,
    NvU32 deviceHandle,
    NVSurfaceDescriptor *pSurfaceDesc)
{
    nvCtxDmaFree(pDevEvo, deviceHandle, &pSurfaceDesc->ctxDmaHandle);
}

static NvU32 EvoBindSurfaceDescriptor90(
    NVDevEvoPtr pDevEvo, NVEvoChannelPtr pChannel, NVSurfaceDescriptor *pSurfaceDesc)
{
    return nvCtxDmaBind(pDevEvo, pChannel, pSurfaceDesc->ctxDmaHandle);
}

NVEvoHAL nvEvo94 = {
    EvoSetRasterParams91,                         /* SetRasterParams */
    EvoSetProcAmp90,                              /* SetProcAmp */
    EvoSetHeadControl90,                          /* SetHeadControl */
    EvoSetHeadRefClk90,                           /* SetHeadRefClk */
    EvoHeadSetControlOR90,                        /* HeadSetControlOR */
    EvoORSetControl90,                            /* ORSetControl */
    EvoHeadSetDisplayId90,                        /* HeadSetDisplayId */
    EvoSetUsageBounds90,                          /* SetUsageBounds */
    EvoUpdate91,                                  /* Update */
    nvEvo1IsModePossible,                         /* IsModePossible */
    nvEvo1PrePostIMP,                             /* PrePostIMP */
    EvoSetNotifier90,                             /* SetNotifier */
    EvoGetCapabilities90,                         /* GetCapabilities */
    EvoFlip90,                                    /* Flip */
    EvoFlipTransitionWAR90,                       /* FlipTransitionWAR */
    EvoFillLUTSurface90,                          /* FillLUTSurface */
    EvoSetLUTContextDma90,                        /* SetLUTContextDma */
    EvoSetOutputScaler90,                         /* SetOutputScaler */
    EvoSetViewportPointIn90,                      /* SetViewportPointIn */
    EvoSetViewportInOut90,                        /* SetViewportInOut */
    EvoSetCursorImage91,                          /* SetCursorImage */
    EvoValidateCursorSurface90,                   /* ValidateCursorSurface */
    EvoValidateWindowFormat90,                    /* ValidateWindowFormat */
    EvoInitCompNotifier3,                         /* InitCompNotifier */
    EvoIsCompNotifierComplete3,                   /* IsCompNotifierComplete */
    EvoWaitForCompNotifier3,                      /* WaitForCompNotifier */
    EvoSetDither91,                               /* SetDither */
    EvoSetStallLock94,                            /* SetStallLock */
    NULL,                                         /* SetDisplayRate */
    EvoInitChannel90,                             /* InitChannel */
    NULL,                                         /* InitDefaultLut */
    EvoInitWindowMapping90,                       /* InitWindowMapping */
    nvEvo1IsChannelIdle,                          /* IsChannelIdle */
    nvEvo1IsChannelMethodPending,                 /* IsChannelMethodPending */
    EvoForceIdleSatelliteChannel90,               /* ForceIdleSatelliteChannel */
    EvoForceIdleSatelliteChannel90,               /* ForceIdleSatelliteChannelIgnoreLock */
    EvoAccelerateChannel91,                       /* AccelerateChannel */
    EvoResetChannelAccelerators91,                /* ResetChannelAccelerators */
    EvoAllocRmCtrlObject90,                       /* AllocRmCtrlObject */
    EvoFreeRmCtrlObject90,                        /* FreeRmCtrlObject */
    EvoSetImmPointOut91,                          /* SetImmPointOut */
    EvoStartHeadCRC32Capture90,                   /* StartCRC32Capture */
    EvoStopHeadCRC32Capture90,                    /* StopCRC32Capture */
    EvoQueryHeadCRC32_90,                         /* QueryCRC32 */
    EvoGetScanLine90,                             /* GetScanLine */
    NULL,                                         /* ConfigureVblankSyncObject */
    nvEvo1SetDscParams,                           /* SetDscParams */
    NULL,                                         /* EnableMidFrameAndDWCFWatermark */
    EvoGetActiveViewportOffset94,                 /* GetActiveViewportOffset */
    EvoClearSurfaceUsage91,                       /* ClearSurfaceUsage */
    EvoComputeWindowScalingTaps91,                /* ComputeWindowScalingTaps */
    NULL,                                         /* GetWindowScalingCaps */
    NULL,                                         /* SetMergeMode */
    EvoAllocSurfaceDescriptor90,                  /* AllocSurfaceDescriptor */
    EvoFreeSurfaceDescriptor90,                   /* FreeSurfaceDescriptor */
    EvoBindSurfaceDescriptor90,                   /* BindSurfaceDescriptor */
    NULL,                                         /* SetTmoLutSurfaceAddress */
    NULL,                                         /* SetILUTSurfaceAddress */
    NULL,                                         /* SetISOSurfaceAddress */
    NULL,                                         /* SetCoreNotifierSurfaceAddressAndControl */
    NULL,                                         /* SetWinNotifierSurfaceAddressAndControl */
    NULL,                                         /* SetSemaphoreSurfaceAddressAndControl */
    NULL,                                         /* SetAcqSemaphoreSurfaceAddressAndControl */
    {                                             /* caps */
        FALSE,                                    /* supportsNonInterlockedUsageBoundsUpdate */
        FALSE,                                    /* supportsDisplayRate */
        TRUE,                                     /* supportsFlipLockRGStatus */
        FALSE,                                    /* needDefaultLutSurface */
        FALSE,                                    /* hasUnorm10OLUT */
        TRUE,                                     /* supportsImageSharpening */
        FALSE,                                    /* supportsHDMIVRR */
        TRUE,                                     /* supportsCoreChannelSurface */
        FALSE,                                    /* supportsHDMIFRL */
        TRUE,                                     /* supportsSetStorageMemoryLayout */
        FALSE,                                    /* supportsIndependentAcqRelSemaphore */
        TRUE,                                     /* supportsCoreLut */
        FALSE,                                    /* supportsSynchronizedOverlayPositionUpdate */
        FALSE,                                    /* supportsVblankSyncObjects */
        TRUE,                                     /* requiresScalingTapsInBothDimensions */
        FALSE,                                    /* supportsMergeMode */
        FALSE,                                    /* supportsHDMI10BPC */
        FALSE,                                    /* supportsDPAudio192KHz */
        NV_EVO2_SUPPORTED_DITHERING_MODES,        /* supportedDitheringModes */
        sizeof(NV5070_CTRL_CMD_IS_MODE_POSSIBLE_PARAMS), /* impStructSize */
        NV_EVO_SCALER_1TAP,                       /* minScalerTaps */
        0,                                        /* xEmulatedSurfaceMemoryFormats */
    },
};
