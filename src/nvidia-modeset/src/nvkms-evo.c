/*
 * SPDX-FileCopyrightText: Copyright (c) 2014 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "nvkms-types.h"

#include "nvkms-evo-states.h"
#include "dp/nvdp-connector.h"
#include "dp/nvdp-device.h"
#include "nvkms-console-restore.h"
#include "nvkms-rm.h"
#include "nvkms-dpy.h"
#include "nvkms-cursor.h"
#include "nvkms-hal.h"
#include "nvkms-hdmi.h"
#include "nvkms-modepool.h"
#include "nvkms-evo.h"
#include "nvkms-flip.h"
#include "nvkms-hw-flip.h"
#include "nvkms-dma.h"
#include "nvkms-framelock.h"
#include "nvkms-utils.h"
#include "nvkms-lut.h"
#include "nvkms-modeset.h"
#include "nvkms-prealloc.h"
#include "nvkms-rmapi.h"
#include "nvkms-surface.h"
#include "nvkms-headsurface.h"
#include "nvkms-difr.h"
#include "nvkms-vrr.h"
#include "nvkms-ioctl.h"

#include "nvctassert.h"

#include <ctrl/ctrl0073/ctrl0073dfp.h> // NV0073_CTRL_DFP_ASSIGN_SOR_PARAMS
#include <ctrl/ctrl0073/ctrl0073system.h> // NV0073_CTRL_CMD_SYSTEM_ALLOCATE_DISPLAY_BANDWIDTH
#include <ctrl/ctrl0080/ctrl0080gpu.h> // NV0080_CTRL_CMD_GPU_*
#include <ctrl/ctrl0080/ctrl0080unix.h> // NV0080_CTRL_OS_UNIX_VT_SWITCH_*
#include <ctrl/ctrl30f1.h> // NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_*
#include <ctrl/ctrl5070/ctrl5070rg.h> // NV5070_CTRL_CMD_GET_FRAMELOCK_HEADER_LOCKPINS
#include <ctrl/ctrl5070/ctrl5070system.h> // NV5070_CTRL_CMD_SYSTEM_GET_CAPS_V2
#include <ctrl/ctrl5070/ctrl5070or.h> // NV5070_CTRL_CMD_SET_SOR_FLUSH_MODE
#include <ctrl/ctrl0073/ctrl0073dp.h> // NV0073_CTRL_DP_CTRL

#include "nvkms.h"
#include "nvkms-private.h"
#include "nvos.h"

#include "displayport/dpcd.h"

#define EVO_RASTER_LOCK     1
#define EVO_FLIP_LOCK       2

#define NVUPDATE_LUT_TIMER_NVKMS_DATAU32_HEAD                     7:0
#define NVUPDATE_LUT_TIMER_NVKMS_DATAU32_BASE_LUT                 8:8
#define NVUPDATE_LUT_TIMER_NVKMS_DATAU32_BASE_LUT_DISABLE           0
#define NVUPDATE_LUT_TIMER_NVKMS_DATAU32_BASE_LUT_ENABLE            1
#define NVUPDATE_LUT_TIMER_NVKMS_DATAU32_OUTPUT_LUT               9:9
#define NVUPDATE_LUT_TIMER_NVKMS_DATAU32_OUTPUT_LUT_DISABLE         0
#define NVUPDATE_LUT_TIMER_NVKMS_DATAU32_OUTPUT_LUT_ENABLE          1

/*
 * This struct is used to describe a single set of GPUs to lock together by
 * GetRasterLockGroups().
 */
typedef struct _NVEvoRasterLockGroup {
    NvU32 numDisps;
    NVDispEvoPtr pDispEvoOrder[NVKMS_MAX_SUBDEVICES];
} RasterLockGroup;

/*
 * These are used hold additional state for each DispEvo during building of
 * RasterLockGroups.
 */
typedef struct
{
    NVDispEvoPtr pDispEvo;
    NvU32 gpuId;
    RasterLockGroup *pRasterLockGroup;
} DispEntry;

typedef struct
{
    /* Array of DispEvos and their assigned RasterLockGroups. */
    NvU32 numDisps;
    DispEntry disps[NVKMS_MAX_SUBDEVICES];
} DispEvoList;

struct _NVLockGroup {
    RasterLockGroup rasterLockGroup;
    NvBool flipLockEnabled;
};

static void EvoSetViewportPointIn(NVDispEvoPtr pDispEvo, NvU32 head,
                                  NvU16 x, NvU16 y,
                                  NVEvoUpdateState *updateState);
static void GetRasterLockPin(NVDispEvoPtr pDispEvo0, NvU32 head0,
                             NVDispEvoPtr pDispEvo1, NvU32 head1,
                             NVEvoLockPin *serverPin, NVEvoLockPin *clientPin);
static NvBool EvoWaitForLock(const NVDevEvoRec *pDevEvo, const NvU32 sd,
                             const NvU32 head, const NvU32 type,
                             NvU64 *pStartTime);
static void EvoUpdateHeadParams(const NVDispEvoRec *pDispEvo, NvU32 head,
                                NVEvoUpdateState *updateState);

static void SetRefClk(NVDevEvoPtr pDevEvo,
                      NvU32 sd, NvU32 head, NvBool external,
                      NVEvoUpdateState *updateState);
static NvBool ApplyLockActionIfPossible(NVDispEvoPtr pDispEvo,
                                        NVEvoSubDevPtr pEvoSubDev,
                                        NVEvoLockAction action);
static void FinishModesetOneDev(NVDevEvoRec *pDevEvo);
static void FinishModesetOneGroup(RasterLockGroup *pRasterLockGroup);
static void EnableFlipLockIfRequested(NVLockGroup *pLockGroup);

static void SyncEvoLockState(void);
static void UpdateEvoLockState(void);

static void ScheduleLutUpdate(NVDispEvoRec *pDispEvo,
                              const NvU32 apiHead, const NvU32 data,
                              const NvU64 usec);

NVEvoGlobal nvEvoGlobal = {
    .clientHandle = 0,
    .frameLockList = NV_LIST_INIT(&nvEvoGlobal.frameLockList),
    .devList = NV_LIST_INIT(&nvEvoGlobal.devList),
#if defined(DEBUG)
    .debugMemoryAllocationList =
        NV_LIST_INIT(&nvEvoGlobal.debugMemoryAllocationList),
#endif /* DEBUG */
};

static RasterLockGroup *globalRasterLockGroups = NULL;
static NvU32 numGlobalRasterLockGroups = 0;

/*
 * Keep track of groups of HW heads which the modeset owner has requested to be
 * fliplocked together.  All of the heads specified here are guaranteed to be
 * active.  A given head can only be in one group at a time.  Fliplock is not
 * guaranteed to be enabled in the hardware for these groups.
 */
typedef struct _FlipLockRequestedGroup {
    struct {
        NVDispEvoPtr pDispEvo;
        NvU32 flipLockHeads;
    } disp[NV_MAX_SUBDEVICES];

    NVListRec listEntry;
} FlipLockRequestedGroup;

static NVListRec requestedFlipLockGroups =
    NV_LIST_INIT(&requestedFlipLockGroups);

/*
 * The dummy infoString should be used in paths that take an
 * NVEvoInfoStringPtr where we don't need to log to a
 * string.  By setting the 's' field to NULL, nothing will be printed
 * to the infoString buffer.
 */
NVEvoInfoStringRec dummyInfoString = {
    .length = 0,
    .totalLength = 0,
    .s = NULL,
};

/*!
 * Return the NVDevEvoPtr, if any, that matches deviceId.
 */
NVDevEvoPtr nvFindDevEvoByDeviceId(NvU32 deviceId)
{
    NVDevEvoPtr pDevEvo;

    FOR_ALL_EVO_DEVS(pDevEvo) {
        if (pDevEvo->usesTegraDevice &&
            (deviceId == NVKMS_DEVICE_ID_TEGRA)) {
            return pDevEvo;
        } else if (pDevEvo->deviceId == deviceId) {
            return pDevEvo;
        }
    };

    return NULL;
}

/*!
 * Find the first unused gpuLogIndex.
 */
NvU8 nvGetGpuLogIndex(void)
{
    NVDevEvoPtr pDevEvo;
    NvU8 gpuLogIndex = 0;

 tryAgain:
    FOR_ALL_EVO_DEVS(pDevEvo) {
        NvU32 sd;
        for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
            if (pDevEvo->pSubDevices[sd] == NULL) {
                continue;
            }
            if (gpuLogIndex == pDevEvo->pSubDevices[sd]->gpuLogIndex) {
                gpuLogIndex++;
                if (gpuLogIndex == 0xFF) {
                    nvAssert(!"Too many GPUs");
                    return NV_INVALID_GPU_LOG_INDEX;
                }
                goto tryAgain;
            }
        }
    }

    return gpuLogIndex;
}

/*!
 * Return whether there are active heads on this pDispEvo.
 */
static NvBool HasActiveHeads(NVDispEvoPtr pDispEvo)
{
    return nvGetActiveHeadMask(pDispEvo) != 0;
}

static void BlankHeadEvo(NVDispEvoPtr pDispEvo, const NvU32 head,
                         NVEvoUpdateState *updateState)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    const NVDispHeadStateEvoRec *pHeadState = &pDispEvo->headState[head];
    struct NvKmsCompositionParams emptyCursorCompParams = 
        nvDefaultCursorCompositionParams(pDevEvo);
    
    /*
     * If core channel surface is supported, ->SetSurface()
     * disables Lut along with core channel surface. Otherwise need to disable
     * Lut explicitly.
     */
    if (!pDevEvo->hal->caps.supportsCoreChannelSurface) {
        pDevEvo->hal->SetLUTContextDma(pDispEvo,
                                       head,
                                       NULL /* pSurfEvo */,
                                       FALSE /* baseLutEnabled */,
                                       FALSE /* outputLutEnabled */,
                                       updateState,
                                       pHeadState->bypassComposition);
    }

    nvPushEvoSubDevMaskDisp(pDispEvo);

    pDevEvo->hal->SetCursorImage(pDevEvo,
                                 head,
                                 NULL /* pSurfaceEvo */,
                                 updateState,
                                 &emptyCursorCompParams);

    {
        NVFlipChannelEvoHwState hwState = { { 0 } };
        NvU32 layer;

        for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
            pDevEvo->hal->Flip(pDevEvo,
                               pDevEvo->head[head].layer[layer],
                               &hwState,
                               updateState,
                               FALSE /* bypassComposition */);
        }
    }

    nvPopEvoSubDevMask(pDevEvo);
}

void nvEvoDetachConnector(NVConnectorEvoRec *pConnectorEvo, const NvU32 head,
                          NVEvoModesetUpdateState *pModesetUpdateState)
{
    NVEvoUpdateState *updateState = &pModesetUpdateState->updateState;
    NVDispEvoPtr pDispEvo = pConnectorEvo->pDispEvo;
    const NVDispHeadStateEvoRec *pHeadState = &pDispEvo->headState[head];
    const NVHwModeTimingsEvo *pTimings = &pHeadState->timings;
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NvU32 orIndex;

    for (orIndex = 0;
            orIndex < ARRAY_LEN(pConnectorEvo->or.ownerHeadMask); orIndex++) {
        if ((pConnectorEvo->or.ownerHeadMask[orIndex] & NVBIT(head)) != 0x0) {
            break;
        }
    }

    if (orIndex >= ARRAY_LEN(pConnectorEvo->or.ownerHeadMask)) {
        nvAssert(!"Not found attached OR");
        return;
    }

    pConnectorEvo->or.ownerHeadMask[orIndex] &= ~NVBIT(head);

    /* Disable the palette, cursor, and ISO ctxDma on this head. */
    BlankHeadEvo(pDispEvo, head, updateState);

    // Only tear down the actual output for SLI primary.
    nvPushEvoSubDevMask(pDevEvo, 1 << pDispEvo->displayOwner);

    pDevEvo->hal->ORSetControl(pDevEvo,
                               pConnectorEvo,
                               pTimings->protocol,
                               orIndex,
                               pConnectorEvo->or.ownerHeadMask[orIndex],
                               updateState);

    /*
     * Tell RM that there is no DisplayID is associated with this head anymore.
     */
    pDevEvo->hal->HeadSetDisplayId(pDevEvo, head, 0x0, updateState);

    nvPopEvoSubDevMask(pDevEvo);

    pModesetUpdateState->connectorIds =
        nvAddDpyIdToDpyIdList(pHeadState->pConnectorEvo->displayId,
                              pModesetUpdateState->connectorIds);
}

static
NvU32 GetSorIndexToAttachConnector(const NVConnectorEvoRec *pConnectorEvo,
                                   const NvBool isPrimaryHead)
{
    NvU32 orIndex = NV_INVALID_OR;

    nvAssert(isPrimaryHead ||
                (pConnectorEvo->or.type == NV0073_CTRL_SPECIFIC_OR_TYPE_SOR));

    if (isPrimaryHead ||
            (pConnectorEvo->or.type != NV0073_CTRL_SPECIFIC_OR_TYPE_SOR)) {
        orIndex = pConnectorEvo->or.primary;
    } else {
        NvU32 i;

        FOR_EACH_INDEX_IN_MASK(32, i, pConnectorEvo->or.secondaryMask) {
            if (pConnectorEvo->or.ownerHeadMask[i] == 0x0) {
                orIndex = i;
                break;
            }
        } FOR_EACH_INDEX_IN_MASK_END;
    }

    return orIndex;
}

void nvEvoAttachConnector(NVConnectorEvoRec *pConnectorEvo,
                          const NvU32 head,
                          const NvU32 isPrimaryHead,
                          NVDPLibModesetStatePtr pDpLibModesetState,
                          NVEvoModesetUpdateState *pModesetUpdateState)
{
    NVEvoUpdateState *updateState = &pModesetUpdateState->updateState;
    NVDispEvoPtr pDispEvo = pConnectorEvo->pDispEvo;
    const NVDispHeadStateEvoRec *pHeadState = &pDispEvo->headState[head];
    const NVHwModeTimingsEvo *pTimings = &pHeadState->timings;
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NvU32 orIndex =
        GetSorIndexToAttachConnector(pConnectorEvo, isPrimaryHead);
    NvU32 i;

    nvAssert(orIndex != NV_INVALID_OR);
    nvAssert(!(pConnectorEvo->or.ownerHeadMask[orIndex] & NVBIT(head)));
    nvAssert(pHeadState->activeRmId != 0);

    FOR_EACH_INDEX_IN_MASK(32, i, pConnectorEvo->or.ownerHeadMask[orIndex]) {
        nvAssert(pTimings->protocol ==
                 pDispEvo->headState[i].timings.protocol);
    } FOR_EACH_INDEX_IN_MASK_END;

    pConnectorEvo->or.ownerHeadMask[orIndex] |= NVBIT(head);

    // Only set up the actual output for SLI primary.
    nvPushEvoSubDevMask(pDevEvo, 1 << pDispEvo->displayOwner);

    pDevEvo->hal->ORSetControl(pDevEvo,
                               pConnectorEvo,
                               pTimings->protocol,
                               orIndex,
                               pConnectorEvo->or.ownerHeadMask[orIndex],
                               updateState);


    /* Tell RM which DisplayID is associated with the head. */
    pDevEvo->hal->HeadSetDisplayId(pDevEvo,
                                   head, pHeadState->activeRmId,
                                   updateState);

    nvPopEvoSubDevMask(pDevEvo);

    pModesetUpdateState->connectorIds =
        nvAddDpyIdToDpyIdList(pConnectorEvo->displayId,
                              pModesetUpdateState->connectorIds);
    pModesetUpdateState->pDpLibModesetState[head] = pDpLibModesetState;
}

void nvSetViewPortPointInEvo(NVDispEvoPtr pDispEvo,
                             const NvU32 head,
                             const NvU16 x,
                             NvU16 y,
                             NVEvoUpdateState *updateState)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;

    NVEvoSubDevHeadStateRec *pSdHeadState =
        &pDevEvo->gpus[pDispEvo->displayOwner].headState[head];

    pSdHeadState->viewPortPointIn.x = x;
    pSdHeadState->viewPortPointIn.y = y;

    EvoSetViewportPointIn(pDispEvo, head, x, y, updateState);
}

//
// Sets the Update method which makes all the other methods in the PB to take effect.
//
static void EvoUpdateAndKickOffWithNotifier(
    const NVDispEvoRec *pDispEvo,
    NvBool notify,
    NvBool sync, int notifier,
    NVEvoUpdateState *updateState,
    NvBool releaseElv)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;

    // Calling code should reject operations that send updates while the console
    // is active.
    nvAssert(!pDevEvo->coreInitMethodsPending);

    // It doesn't make sense to request sync without requesting a notifier.
    nvAssert(!sync || notify);

    if (notify) {
        // Clear the completion notifier.
        pDevEvo->hal->InitCompNotifier(pDispEvo, notifier);
    }

    nvPushEvoSubDevMaskDisp(pDispEvo);
    pDevEvo->hal->SetNotifier(pDevEvo, notify, sync, notifier,
                              updateState);
    pDevEvo->hal->Update(pDevEvo, updateState, releaseElv);
    nvPopEvoSubDevMask(pDevEvo);

    // Wait for completion.
    if (sync) {
        pDevEvo->hal->WaitForCompNotifier(pDispEvo, notifier);
    }

    if (notify) {
        const NVDispEvoRec *pDispEvoTmp;
        NVEvoUpdateState coreUpdateState = { };
        NvU32 sd;

        // To work around HW bug 1945716 and to prevent subsequent core updates
        // from triggering unwanted notifier writes, set the core channel
        // completion notifier control and context DMA disables when
        // notification is not requested.

        nvPushEvoSubDevMaskDisp(pDispEvo);
        pDevEvo->hal->SetNotifier(pDevEvo,
                                  FALSE /* notify */,
                                  FALSE /* awaken */,
                                  0     /* notifier */,
                                  &coreUpdateState);
        nvPopEvoSubDevMask(pDevEvo);

        // SetCoreNotifier is only expected to push core channel methods.
        FOR_ALL_EVO_DISPLAYS(pDispEvoTmp, sd, pDevEvo) {
            if (pDispEvoTmp == pDispEvo) {
                nvAssert(coreUpdateState.subdev[sd].channelMask ==
                         DRF_DEF64(_EVO, _CHANNEL_MASK, _CORE, _ENABLE));
            } else {
                nvAssert(coreUpdateState.subdev[sd].channelMask == 0x0);
            }
        }

        // We don't really need to kick off here, but might as well to keep the
        // state cache up to date.  Note that we intentionally don't use
        // pDevEvo->hal->Update since we don't want another Update.
        nvDmaKickoffEvo(pDevEvo->core);
    }

    return;
}

void nvEvoUpdateAndKickOff(const NVDispEvoRec *pDispEvo, NvBool sync,
                           NVEvoUpdateState *updateState, NvBool releaseElv)
{
    EvoUpdateAndKickOffWithNotifier(pDispEvo, sync, sync, 0, updateState,
                                    releaseElv);
}

void nvDoIMPUpdateEvo(NVDispEvoPtr pDispEvo,
                      NVEvoUpdateState *updateState)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;

    // IMP pre-modeset
    pDevEvo->hal->PrePostIMP(pDispEvo, TRUE /* isPre */);

    // Do the update
    nvEvoUpdateAndKickOff(pDispEvo, TRUE, updateState, TRUE /* releaseElv */);

    // IMP post-modeset
    pDevEvo->hal->PrePostIMP(pDispEvo, FALSE /* isPre */);
}

void nvEvoFlipUpdate(NVDispEvoPtr pDispEvo,
                     NVEvoUpdateState *updateState)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    int notifier = -1;

    if (nvEvoLUTNotifiersNeedCommit(pDispEvo)) {
        notifier = nvEvoCommitLUTNotifiers(pDispEvo);
    }

    if (notifier >= 0) {
        EvoUpdateAndKickOffWithNotifier(pDispEvo,
                                        TRUE /* notify */,
                                        FALSE /* sync */,
                                        notifier,
                                        updateState,
                                        TRUE /* releaseElv */);
    } else {
        pDevEvo->hal->Update(pDevEvo, updateState, TRUE /* releaseElv */);
    }
}

/*!
 * Tell RM not to expect anything other than a stall lock change during the next
 * update.
 */
void nvEvoArmLightweightSupervisor(NVDispEvoPtr pDispEvo,
                                   const NvU32 head,
                                   NvBool isVrr,
                                   NvBool enable)
{
    const NVDispHeadStateEvoRec *pHeadState = &pDispEvo->headState[head];
    NV0073_CTRL_SYSTEM_ARM_LIGHTWEIGHT_SUPERVISOR_PARAMS params = { };
    const NVHwModeTimingsEvo *pTimings = &pHeadState->timings;

    if (!nvHeadIsActive(pDispEvo, head)) {
        return;
    }

    nvAssert(!pTimings->interlaced && !pTimings->doubleScan);

    params.subDeviceInstance = pDispEvo->displayOwner;
    params.displayId = pHeadState->activeRmId;
    params.bArmLWSV = enable;
    params.bVrrState = isVrr;
    params.vActive = nvEvoVisibleHeight(pTimings);
    params.vfp = pTimings->rasterSize.y -
                 pTimings->rasterBlankStart.y;

    if (nvRmApiControl(nvEvoGlobal.clientHandle,
                       pDispEvo->pDevEvo->displayCommonHandle,
                       NV0073_CTRL_CMD_SYSTEM_ARM_LIGHTWEIGHT_SUPERVISOR,
                       &params, sizeof(params))
            != NVOS_STATUS_SUCCESS) {
        nvAssert(!"ARM_LIGHTWEIGHT_SUPERVISOR failed");
    }
}

/*
 * Convert from NVHwModeTimingsEvoPtr to NvModeTimingsPtr.
 *
 * Note that converting from NvModeTimingsPtr to
 * NVHwModeTimingsEvoPtr (via
 * ConstructHwModeTimingsFromNvModeTimings()) and converting back from
 * NVHwModeTimingsEvoPtr to NvModeTimingsPtr (via
 * nvConstructNvModeTimingsFromHwModeTimings()) can lose precision in
 * the case of interlaced modes due to the division by 2.  This
 * function should only be used for reporting purposes.
 */

void
nvConstructNvModeTimingsFromHwModeTimings(const NVHwModeTimingsEvo *pTimings,
                                          NvModeTimingsPtr pModeTimings)
{
    NvU32 rasterBlankEndY, rasterSyncEndY;

    if (!pTimings || !pModeTimings) {
        nvAssert(!"Null params");
        return;
    }

    pModeTimings->pixelClockHz  = KHzToHz(pTimings->pixelClock);
    pModeTimings->hVisible      = nvEvoVisibleWidth(pTimings);
    pModeTimings->hSyncStart    = pTimings->rasterSize.x -
                                  pTimings->rasterBlankEnd.x - 1;
    pModeTimings->hSyncEnd      = pTimings->rasterSize.x -
                                  pTimings->rasterBlankEnd.x +
                                  pTimings->rasterSyncEnd.x;
    pModeTimings->hTotal        = pTimings->rasterSize.x;
    pModeTimings->vVisible      = nvEvoVisibleHeight(pTimings);
    rasterBlankEndY             = pTimings->rasterBlankEnd.y + 1;
    rasterSyncEndY              = pTimings->rasterSyncEnd.y + 1;

    if (pTimings->interlaced) {
        rasterBlankEndY *= 2;
        rasterSyncEndY *= 2;
    }

    /*
     * The real pixel clock and width values for modes using YUV 420 emulation
     * are half of the incoming values parsed from the EDID. This conversion is
     * performed here, so NvModeTimings will have the user-visible (full width)
     * values, and NVHwModeTimingsEvo will have the real (half width) values.
     */
    if (pTimings->yuv420Mode == NV_YUV420_MODE_SW) {
        pModeTimings->pixelClockHz *= 2;
        pModeTimings->hVisible *= 2;
        pModeTimings->hSyncStart *= 2;
        pModeTimings->hSyncEnd *= 2;
        pModeTimings->hTotal *= 2;
    }

    pModeTimings->vSyncStart    = pTimings->rasterSize.y - rasterBlankEndY;
    pModeTimings->vSyncEnd      = pTimings->rasterSize.y - rasterBlankEndY +
                                  rasterSyncEndY;
    pModeTimings->vTotal        = pTimings->rasterSize.y;
    pModeTimings->interlaced    = pTimings->interlaced;
    pModeTimings->doubleScan    = pTimings->doubleScan;
    pModeTimings->hSyncNeg      = pTimings->hSyncPol;
    pModeTimings->hSyncPos      = !pTimings->hSyncPol;
    pModeTimings->vSyncNeg      = pTimings->vSyncPol;
    pModeTimings->vSyncPos      = !pTimings->vSyncPol;
    pModeTimings->RRx1k         = (pModeTimings->pixelClockHz /
                                   (pModeTimings->hTotal *
                                    pModeTimings->vTotal));

    if (pModeTimings->doubleScan) {
        pModeTimings->vVisible /= 2;
        pModeTimings->vSyncStart /= 2;
        pModeTimings->vSyncEnd /= 2;
        pModeTimings->vTotal /= 2;
    }

    pModeTimings->hdmi3D = pTimings->hdmi3D;
    pModeTimings->yuv420Mode = pTimings->yuv420Mode;
}



/*
 * Tweak pTimings to be compatible with gsync.
 */

static void TweakTimingsForGsync(const NVDpyEvoRec *pDpyEvo,
                                 NVHwModeTimingsEvoPtr pTimings,
                                 NVEvoInfoStringPtr pInfoString,
                                 const enum NvKmsStereoMode stereo)
{
    NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_PARAMS gsyncOptTimingParams = { 0 };
    NVDispEvoPtr pDispEvo = pDpyEvo->pDispEvo;
    NvModeTimings modeTimings;
    NvU32 ret;

    /*
     * if 3D Vision Stereo is enabled, do not actually
     * tweak the modetimings; WAR for bug 692266
     */

    if (nvIs3DVisionStereoEvo(stereo)) {

        nvEvoLogInfoString(pInfoString,
                           "Not adjusting mode timings of %s for Quadro Sync "
                           "compatibility since 3D Vision Stereo is enabled.",
                           pDpyEvo->name);
        return;
    }

    gsyncOptTimingParams.gpuId = nvGpuIdOfDispEvo(pDispEvo);

    if (pDpyEvo->pConnectorEvo->legacyType ==
        NV0073_CTRL_SPECIFIC_DISPLAY_TYPE_DFP) {

        gsyncOptTimingParams.output =
            NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_PROTOCOL_OUTPUT_SOR;
        gsyncOptTimingParams.adjust =
            NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_ADJUST_DEFAULT_DFP;

    } else if (pDpyEvo->pConnectorEvo->legacyType ==
               NV0073_CTRL_SPECIFIC_DISPLAY_TYPE_CRT) {

        gsyncOptTimingParams.output =
            NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_PROTOCOL_OUTPUT_DAC;
        gsyncOptTimingParams.adjust =
            NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_ADJUST_DEFAULT_CRT;
    }

    gsyncOptTimingParams.pixelClockHz = KHzToHz(pTimings->pixelClock);

    if (pTimings->interlaced) {
        gsyncOptTimingParams.structure =
            NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_STRUCTURE_INTERLACED;
    } else {
        gsyncOptTimingParams.structure =
            NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_STRUCTURE_PROGRESSIVE;
    }

    gsyncOptTimingParams.hDeltaStep =
        NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_H_DELTA_STEP_USE_DEFAULTS;
    gsyncOptTimingParams.vDeltaStep =
        NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_V_DELTA_STEP_USE_DEFAULTS;
    gsyncOptTimingParams.hDeltaMax =
        NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_H_DELTA_MAX_USE_DEFAULTS;
    gsyncOptTimingParams.vDeltaMax =
        NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_V_DELTA_MAX_USE_DEFAULTS;

    gsyncOptTimingParams.hSyncEnd       = pTimings->rasterSyncEnd.x + 1;
    gsyncOptTimingParams.hBlankEnd      = pTimings->rasterBlankEnd.x + 1;
    gsyncOptTimingParams.hBlankStart    = pTimings->rasterBlankStart.x + 1;
    gsyncOptTimingParams.hTotal         = pTimings->rasterSize.x;

    gsyncOptTimingParams.vSyncEnd       = pTimings->rasterSyncEnd.y + 1;
    gsyncOptTimingParams.vBlankEnd      = pTimings->rasterBlankEnd.y + 1;
    gsyncOptTimingParams.vBlankStart    = pTimings->rasterBlankStart.y + 1;
    gsyncOptTimingParams.vTotal         = pTimings->rasterSize.y;

    gsyncOptTimingParams.vInterlacedBlankEnd = pTimings->rasterVertBlank2End;
    gsyncOptTimingParams.vInterlacedBlankStart =
        pTimings->rasterVertBlank2Start;

    switch (pTimings->protocol) {
        case NVKMS_PROTOCOL_DAC_RGB:
            gsyncOptTimingParams.protocol =
                NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_PROTOCOL_DAC_RGB_CRT;
            break;
        case NVKMS_PROTOCOL_PIOR_EXT_TMDS_ENC:
            nvAssert(!"GSYNC_GET_OPTIMIZED_TIMING doesn't handle external TMDS.");
            // fallthrough
        case NVKMS_PROTOCOL_SOR_SINGLE_TMDS_A:
            gsyncOptTimingParams.protocol =
                NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_PROTOCOL_SOR_SINGLE_TMDS_A;
            break;
        case NVKMS_PROTOCOL_SOR_SINGLE_TMDS_B:
            gsyncOptTimingParams.protocol =
                NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_PROTOCOL_SOR_SINGLE_TMDS_B;
            break;
        case NVKMS_PROTOCOL_SOR_DUAL_TMDS:
            gsyncOptTimingParams.protocol =
                NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_PROTOCOL_SOR_DUAL_TMDS;
            break;
        case NVKMS_PROTOCOL_SOR_DP_A:
            gsyncOptTimingParams.protocol =
                NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_PROTOCOL_SOR_DP_A;
            break;
        case NVKMS_PROTOCOL_SOR_DP_B:
            gsyncOptTimingParams.protocol =
                NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_PROTOCOL_SOR_DP_B;
            break;
        case NVKMS_PROTOCOL_SOR_LVDS_CUSTOM:
            gsyncOptTimingParams.protocol =
                NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_PROTOCOL_SOR_LVDS_CUSTOM;
            break;
        case NVKMS_PROTOCOL_SOR_HDMI_FRL:
            gsyncOptTimingParams.protocol =
                NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_PROTOCOL_SOR_HDMI_FRL;
            break;
        case NVKMS_PROTOCOL_DSI:
            nvAssert(!"GSYNC_GET_OPTIMIZED_TIMING doesn't handle DSI.");
            return;
    }

    nvEvoLogInfoString(pInfoString,
            "Adjusting Mode Timings for Quadro Sync Compatibility");

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDispEvo->pFrameLockEvo->device,
                         NV30F1_CTRL_CMD_GSYNC_GET_OPTIMIZED_TIMING,
                         &gsyncOptTimingParams,
                         sizeof(gsyncOptTimingParams));

    if (ret != NVOS_STATUS_SUCCESS) {
        nvAssert(!"Failed to convert to Quadro Sync safe timing");
        /* do not apply the timings returned by RM if the call failed */
        return;
    } else if (!gsyncOptTimingParams.bOptimized) {
        nvEvoLogInfoString(pInfoString, " Timings Unchanged.");
        return;
    }

    nvConstructNvModeTimingsFromHwModeTimings(pTimings, &modeTimings);

    nvEvoLogInfoString(pInfoString, " Old Timings:");
    nvEvoLogModeValidationModeTimings(pInfoString, &modeTimings);

    pTimings->rasterSyncEnd.x           = gsyncOptTimingParams.hSyncEnd - 1;
    pTimings->rasterSyncEnd.y           = gsyncOptTimingParams.vSyncEnd - 1;
    pTimings->rasterBlankEnd.x          = gsyncOptTimingParams.hBlankEnd - 1;
    pTimings->rasterBlankEnd.y          = gsyncOptTimingParams.vBlankEnd - 1;
    pTimings->rasterBlankStart.x        = gsyncOptTimingParams.hBlankStart - 1;
    pTimings->rasterBlankStart.y        = gsyncOptTimingParams.vBlankStart - 1;
    pTimings->rasterSize.x              = gsyncOptTimingParams.hTotal;
    pTimings->rasterSize.y              = gsyncOptTimingParams.vTotal;

    if (gsyncOptTimingParams.structure ==
        NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_STRUCTURE_INTERLACED) {
        pTimings->rasterVertBlank2Start =
            gsyncOptTimingParams.vInterlacedBlankStart;
        pTimings->rasterVertBlank2End =
            gsyncOptTimingParams.vInterlacedBlankEnd;
    }

    pTimings->pixelClock = HzToKHz(gsyncOptTimingParams.pixelClockHz); // Hz to KHz

    nvConstructNvModeTimingsFromHwModeTimings(pTimings, &modeTimings);

    nvEvoLogInfoString(pInfoString, " New Timings:");
    nvEvoLogModeValidationModeTimings(pInfoString, &modeTimings);
}

static NvBool HeadStateIsHdmiTmdsDeepColor(const NVDispHeadStateEvoRec *pHeadState)
{
    nvAssert(pHeadState->pConnectorEvo != NULL);

    // Check for HDMI TMDS.
    if (pHeadState->pConnectorEvo->isHdmiEnabled &&
        (pHeadState->timings.protocol != NVKMS_PROTOCOL_SOR_HDMI_FRL)) {
        // Check for pixelDepth >= 30.
        switch (pHeadState->pixelDepth) {
            case NVKMS_PIXEL_DEPTH_18_444:
            case NVKMS_PIXEL_DEPTH_24_444:
            case NVKMS_PIXEL_DEPTH_20_422:
            case NVKMS_PIXEL_DEPTH_16_422:
                return FALSE;
            case NVKMS_PIXEL_DEPTH_30_444:
                return TRUE;
        }
    }

    return FALSE;
}

/*!
 * Check whether rasterlock is possible between the two head states.
 * Note that we don't compare viewports, but I don't believe the viewport size
 * affects whether it is possible to rasterlock.
 */

static NvBool RasterLockPossible(const NVDispHeadStateEvoRec *pHeadState1,
                                 const NVDispHeadStateEvoRec *pHeadState2)
{
    const NVHwModeTimingsEvo *pTimings1 = &pHeadState1->timings;
    const NVHwModeTimingsEvo *pTimings2 = &pHeadState2->timings;

    /*
     * XXX Bug 4235728: With HDMI TMDS signaling >= 10 BPC, display requires a
     * higher VPLL clock multiplier varying by pixel depth, which can cause
     * rasterlock to fail between heads with differing multipliers. So, if a
     * head is using HDMI TMDS >= 10 BPC, it can only rasterlock with heads that
     * that are using HDMI TMDS with the same pixel depth.
     */

    // If either head is HDMI TMDS DeepColor (10+ BPC)...
    if (HeadStateIsHdmiTmdsDeepColor(pHeadState1) ||
        HeadStateIsHdmiTmdsDeepColor(pHeadState2)) {
        // The other head must also be HDMI TMDS DeepColor.
        if (!HeadStateIsHdmiTmdsDeepColor(pHeadState1) ||
            !HeadStateIsHdmiTmdsDeepColor(pHeadState2)) {
            return FALSE;
        }

        // Both heads must have identical pixel depth.
        if (pHeadState1->pixelDepth != pHeadState2->pixelDepth) {
            return FALSE;
        }
    }

    return ((pTimings1->rasterSize.x       == pTimings2->rasterSize.x) &&
            (pTimings1->rasterSize.y       == pTimings2->rasterSize.y) &&
            (pTimings1->rasterSyncEnd.x    == pTimings2->rasterSyncEnd.x) &&
            (pTimings1->rasterSyncEnd.y    == pTimings2->rasterSyncEnd.y) &&
            (pTimings1->rasterBlankEnd.x   == pTimings2->rasterBlankEnd.x) &&
            (pTimings1->rasterBlankEnd.y   == pTimings2->rasterBlankEnd.y) &&
            (pTimings1->rasterBlankStart.x == pTimings2->rasterBlankStart.x) &&
            (pTimings1->rasterBlankStart.y == pTimings2->rasterBlankStart.y) &&
            (pTimings1->rasterVertBlank2Start ==
             pTimings2->rasterVertBlank2Start) &&
            (pTimings1->rasterVertBlank2End ==
             pTimings2->rasterVertBlank2End) &&
            (pTimings1->pixelClock         == pTimings2->pixelClock) &&
            (pTimings1->hSyncPol           == pTimings2->hSyncPol) &&
            (pTimings1->vSyncPol           == pTimings2->vSyncPol) &&
            (pTimings1->interlaced         == pTimings2->interlaced) &&
            (pTimings1->doubleScan         == pTimings2->doubleScan));

}

/*!
 * Fill the overscan color struct to be passed to SetRasterParams based on
 * whether or not SW yuv420 is enabled.
 *
 * \param[out] pOverscanColor     The overscan color struct to be filled
 * \param[in] yuv420              Whether or not SW yuv420 is enabled
 */
static void SetOverscanColor(NVEvoColorPtr pOverscanColor, NvBool yuv420)
{
    // Black in RGB format.
    // If we're using an emulated YUV 4:2:0 mode, set the equivalent in
    // YUV ITU-R BT.709 (64/64/512).
    if (yuv420) {
        pOverscanColor->red = 64;
        pOverscanColor->green = 64;
        pOverscanColor->blue = 512;
    } else {
        pOverscanColor->red = 0;
        pOverscanColor->green = 0;
        pOverscanColor->blue = 0;
    }

#if defined(DEBUG)
    // Override the overscan color to red in debug builds.
    // XXX This will look different for YUV 4:2:0
    pOverscanColor->red = 1023;
    pOverscanColor->green = 0;
    pOverscanColor->blue = 0;
#endif
}

void nvEvoDisableHwYUV420Packer(const NVDispEvoRec *pDispEvo,
                                const NvU32 head,
                                NVEvoUpdateState *pUpdateState)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    pDevEvo->gpus[pDispEvo->displayOwner].headControl[head].hwYuv420 = FALSE;
    EvoUpdateHeadParams(pDispEvo, head, pUpdateState);
}

/*
 * Send the raster timings for the pDpyEvo to EVO.
 */
void nvEvoSetTimings(NVDispEvoPtr pDispEvo,
                     const NvU32 head,
                     NVEvoUpdateState *updateState)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    const NVDispHeadStateEvoRec *pHeadState = &pDispEvo->headState[head];
    const NVHwModeTimingsEvo *pTimings = &pHeadState->timings;
    const NVDscInfoEvoRec *pDscInfo = &pHeadState->dscInfo;
    const enum nvKmsPixelDepth pixelDepth = pHeadState->pixelDepth;
    NVEvoColorRec overscanColor;

    nvPushEvoSubDevMaskDisp(pDispEvo);
    SetOverscanColor(&overscanColor, (pTimings->yuv420Mode ==
                                      NV_YUV420_MODE_SW));

    pDevEvo->hal->SetRasterParams(pDevEvo, head,
                                  pTimings, pHeadState->tilePosition,
                                  pDscInfo, &overscanColor, updateState);

    // Set the head parameters
    pDevEvo->gpus[pDispEvo->displayOwner].headControl[head].interlaced =
        pTimings->interlaced;
    pDevEvo->gpus[pDispEvo->displayOwner].headControl[head].hdmi3D =
        pTimings->hdmi3D;

    /*
     * Current HW does not support the combination of HW YUV420 and DSC.
     * HW YUV420 is currently only supported with HDMI, so we should never see
     * the combination of DP DSC and HW YUV420.
     * The combination of HDMI FRL DSC and HW YUV420 should be disallowed by
     * the HDMI library.
     */
    nvAssert(!((pTimings->yuv420Mode == NV_YUV420_MODE_HW) &&
               (pDscInfo->type != NV_DSC_INFO_EVO_TYPE_DISABLED)));

    pDevEvo->gpus[pDispEvo->displayOwner].headControl[head].hwYuv420 =
        (pTimings->yuv420Mode == NV_YUV420_MODE_HW);

    EvoUpdateHeadParams(pDispEvo, head, updateState);

    pDevEvo->hal->SetDscParams(pDispEvo, head, pDscInfo, pixelDepth);

    nvPopEvoSubDevMask(pDevEvo);
}

/*
 * Increase the size of the provided raster lock group by 1.
 *
 * This involves incrementing *pNumRasterLockGroups, reallocating the
 * pRasterLockGroups array, and initializing the new entry.
 */
static RasterLockGroup *GrowRasterLockGroup(RasterLockGroup *pRasterLockGroups,
                                            unsigned int *pNumRasterLockGroups)
{
    RasterLockGroup *pNewRasterLockGroups, *pRasterLockGroup;
    unsigned int numRasterLockGroups;

    numRasterLockGroups = *pNumRasterLockGroups;

    numRasterLockGroups++;
    pNewRasterLockGroups =
        nvRealloc(pRasterLockGroups,
                  numRasterLockGroups * sizeof(RasterLockGroup));
    if (!pNewRasterLockGroups) {
        nvFree(pRasterLockGroups);
        *pNumRasterLockGroups = 0;
        return NULL;
    }

    pRasterLockGroup = &pNewRasterLockGroups[numRasterLockGroups - 1];
    pRasterLockGroup->numDisps = 0;

    *pNumRasterLockGroups = numRasterLockGroups;

    return pNewRasterLockGroups;
}

static RasterLockGroup *CopyAndAppendRasterLockGroup(
    RasterLockGroup *pRasterLockGroups,
    unsigned int *pNumRasterLockGroups,
    const RasterLockGroup *source)
{
    RasterLockGroup *dest;

    pRasterLockGroups = GrowRasterLockGroup(pRasterLockGroups,
                                            pNumRasterLockGroups);
    if (pRasterLockGroups) {
        dest = &pRasterLockGroups[*pNumRasterLockGroups - 1];
        nvkms_memcpy(dest, source, sizeof(RasterLockGroup));
    }

    return pRasterLockGroups;
}

static void AddDispEvoIntoRasterLockGroup(RasterLockGroup *pRasterLockGroup,
                                          NVDispEvoPtr pDispEvo)
{
    NvU32 i;

    /*
     * The extent of a RasterLockGroup is the largest number of GPUs that can
     * be linked together.
     */
    nvAssert(pRasterLockGroup->numDisps < NVKMS_MAX_SUBDEVICES);

    /* Caller should keep track of not adding duplicate entries. */
    for (i = 0; i < pRasterLockGroup->numDisps; i++) {
        nvAssert(pRasterLockGroup->pDispEvoOrder[i] != pDispEvo);
    }

    /* Add to the end of the array. */
    pRasterLockGroup->pDispEvoOrder[pRasterLockGroup->numDisps] = pDispEvo;
    pRasterLockGroup->numDisps++;
}

static const RasterLockGroup *FindRasterLockGroupForDispEvo(
    const RasterLockGroup *pRasterLockGroups,
    unsigned int numRasterLockGroups,
    const NVDispEvoPtr pDispEvo)
{
    const RasterLockGroup *pRasterLockGroup;
    NvU32 i;

    for (pRasterLockGroup = pRasterLockGroups;
         pRasterLockGroup < pRasterLockGroups + numRasterLockGroups;
         pRasterLockGroup++) {
        for (i = 0; i < pRasterLockGroup->numDisps; i++) {
            if (pRasterLockGroup->pDispEvoOrder[i] == pDispEvo) {
                return pRasterLockGroup;
            }
        }
    }

    return NULL;
}

static DispEntry *DispEvoListFindDispByGpuId (DispEvoList *list, NvU32 gpuId)
{
    NvU32 i;

    for (i = 0; i < list->numDisps; i++) {
        if (list->disps[i].gpuId == gpuId) {
            return &list->disps[i];
        }
    }

    return NULL;
}

static void DispEvoListInit(DispEvoList *list)
{
    list->numDisps = 0;
}

static void DispEvoListAppend(DispEvoList *list, NVDispEvoPtr pDispEvo)
{
    nvAssert(DispEvoListFindDispByGpuId(
                 list, nvGpuIdOfDispEvo(pDispEvo)) == NULL);

    nvAssert(list->numDisps < ARRAY_LEN(list->disps));
    list->disps[list->numDisps].pDispEvo = pDispEvo;
    list->disps[list->numDisps].gpuId = nvGpuIdOfDispEvo(pDispEvo);
    list->disps[list->numDisps].pRasterLockGroup = NULL;
    list->numDisps++;
}

/*
 * Helper function to look up, for a gpuId, the list of connected GPUs in
 * NV0000_CTRL_GPU_GET_VIDEO_LINKS_PARAMS.
 */
static NV0000_CTRL_GPU_VIDEO_LINKS *FindLinksForGpuId(
    NV0000_CTRL_GPU_GET_VIDEO_LINKS_PARAMS *vidLinksParams,
    NvU32 gpuId)
{
    NvU32 i;

    for (i = 0; i < NV0000_CTRL_GPU_MAX_ATTACHED_GPUS; i++) {
        if (vidLinksParams->links[i].gpuId == NV0000_CTRL_GPU_INVALID_ID) {
            break;
        }

        if (vidLinksParams->links[i].gpuId == gpuId) {
            return &vidLinksParams->links[i];
        }
    }

    return NULL;
}

static void BuildRasterLockGroupFromVideoLinks(
    DispEvoList *list,
    RasterLockGroup *pRasterLockGroup,
    NvU32 gpuId,
    NV0000_CTRL_GPU_GET_VIDEO_LINKS_PARAMS *vidLinksParams)
{
    DispEntry *dispEntry;
    NV0000_CTRL_GPU_VIDEO_LINKS *links;
    NvU32 i;

    /* Find the correct DispEntry for the gpuId. If we can't find one the
     * gpuId must be pointing to a DevEvo that was not listed in our
     * DevEvoList: ignore these links at this point. */
    dispEntry = DispEvoListFindDispByGpuId(list, gpuId);
    if (!dispEntry) {
        return;
    }

    /*
     * Unless we've seen this gpuId already add into current RasterLockGroup
     * and try to discover bridged GPUs.
     */
    if (!dispEntry->pRasterLockGroup) {
        /* Assign in the current RasterLockGroup. */
        AddDispEvoIntoRasterLockGroup(pRasterLockGroup, dispEntry->pDispEvo);
        dispEntry->pRasterLockGroup = pRasterLockGroup;

        /* First, get the links for this gpuId. */
        links = FindLinksForGpuId(vidLinksParams, gpuId);

        /* Recurse into connected GPUs. */
        if (links) {
            for (i = 0; i < NV0000_CTRL_GPU_MAX_VIDEO_LINKS; i++) {
                if (links->connectedGpuIds[i] == NV0000_CTRL_GPU_INVALID_ID) {
                    break;
                }

                BuildRasterLockGroupFromVideoLinks(list,
                                                   pRasterLockGroup,
                                                   links->connectedGpuIds[i],
                                                   vidLinksParams);
            }
        }
    }
}

/*
 * Stateless (RM SLI/client SLI agnostic) discovery of bridged GPUs: build
 * RasterLockGroups for all non-RM SLI devices based on the found GPU links.
 *
 * This function and BuildRasterLockGroupFromVideoLinks() implement a simple
 * algorithm that puts clusters of bridged GPUs into distinct RasterLockGroups.
 * Here's an outline of how we basically generate the final RasterLockGroups:
 *
 * 1. Create a DispEvoList array to hold RasterLockGroup state for all the
 *    DispEvo objects in the system.
 *
 * 2. Query RM for an array of video links for each GPU.
 *
 * 3. As long as the DispEvoList contains DispEvos of the given pDevEvo
 *    without a group, find the first occurrence of such, create a new
 *    group, and populate it by recursively adding the DispEvo and all
 *    its connected DispEvos into the new group.
 *
 * 4. Once all known DispEvos are assigned the result will be a list of
 *    global RasterLockGroups, each of which hosts <N> DispEvos that are
 *    connected together.
 *
 * The result of this function should be cached once and later used to
 * cheaply look up the appropriate, immutable RasterLockGroup for a DispEvo.
 *
 */
static RasterLockGroup *GetRasterLockGroupsStateless(
    unsigned int *pNumRasterLockGroups)
{
    RasterLockGroup *pRasterLockGroups = NULL;
    RasterLockGroup *pRasterLockGroup;
    DispEvoList evoList;
    NVDevEvoPtr pCurDev;
    NVDispEvoPtr pCurDisp;
    NV0000_CTRL_GPU_GET_VIDEO_LINKS_PARAMS *vidLinksParams;
    NvU32 sd;
    NvU32 i;

    DispEvoListInit(&evoList);

    /*
     * First create an array of DispEntries to hold some state for all the
     * DispEvos in the system.
     */
    FOR_ALL_EVO_DEVS(pCurDev) {
        /*
         * Only include non RM SLI devices so as to not clash with multi-GPU
         * RM SLI devices.
         */
        if (pCurDev->numSubDevices == 1) {
            FOR_ALL_EVO_DISPLAYS(pCurDisp, sd, pCurDev) {
                DispEvoListAppend(&evoList, pCurDisp);
            }
        }
    }

    /*
     * Ask RM about the currently known video links.
     */
    vidLinksParams = nvCalloc(1, sizeof(*vidLinksParams));
    if (!vidLinksParams) {
        return NULL;
    }

    if (nvRmApiControl(nvEvoGlobal.clientHandle,
                       nvEvoGlobal.clientHandle,
                       NV0000_CTRL_CMD_GPU_GET_VIDEO_LINKS,
                       vidLinksParams,
                       sizeof(*vidLinksParams)) == NVOS_STATUS_SUCCESS) {

        for (i = 0; i < evoList.numDisps; i++) {
            /*
             * Create a new group starting from the first DispEvo not yet
             * assigned into a RasterLockGroup, and all GPUs possibly reachable
             * from it through bridges.
             *
             * TODO: Consider if we should only ever start a new
             * RasterLockGroup with a GPU that has only one connection and not
             * two. Then the group's pDispEvoOrder would always start from a
             * "leaf" GPU of a linkage graph. But will the GPU links always be
             * linear and non-branching? NV0000_CTRL_GPU_GET_VIDEO_LINKS_PARAMS
             * makes it possible to represent GPUs with any number of links.
             * Either FinishModesetOneGroup() must be able to handle that
             * (in which case this is not a concern) or we must be able to
             * trust that only 0-2 links will be reported per GPU.
             */
            if (evoList.disps[i].pRasterLockGroup) {
                continue;
            }

            pRasterLockGroups = GrowRasterLockGroup(pRasterLockGroups,
                                                    pNumRasterLockGroups);
            if (!pRasterLockGroups) {
                nvFree(vidLinksParams);
                return NULL;
            }

            pRasterLockGroup = &pRasterLockGroups[*pNumRasterLockGroups - 1];

            BuildRasterLockGroupFromVideoLinks(&evoList,
                                               pRasterLockGroup,
                                               evoList.disps[i].gpuId,
                                               vidLinksParams);
        }

        nvFree(vidLinksParams);
        nvAssert(*pNumRasterLockGroups > 0);
        return pRasterLockGroups;
    }

    nvFree(vidLinksParams);
    nvFree(pRasterLockGroups);
    return NULL;
}

/*
 * GetRasterLockGroups() - Determine which GPUs to consider for locking (or
 * unlocking) displays.  This is one of the following:
 * 1. SLI video bridge order, if SLI is enabled;
 * 2. GPUs linked through rasterlock pins, no SLI (like in clientSLI);
 * 3. A single GPU,
 * in that order.
 *
 * Note that we still go through the same codepaths for the last degenerate
 * case, in order to potentially lock heads on the same GPU together.
 */
static RasterLockGroup *GetRasterLockGroups(
    NVDevEvoPtr pDevEvo,
    unsigned int *pNumRasterLockGroups)
{
    unsigned int i;
    RasterLockGroup *pRasterLockGroups = NULL;

    *pNumRasterLockGroups = 0;

    if (pDevEvo->numSubDevices > 1 && pDevEvo->sli.bridge.present) {
        NV0080_CTRL_GPU_GET_VIDLINK_ORDER_PARAMS params = { 0 };
        NvU32 ret;

        /* In SLI, with a video bridge.  Get the video bridge order from RM. */

        if ((ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                                  pDevEvo->deviceHandle,
                                  NV0080_CTRL_CMD_GPU_GET_VIDLINK_ORDER,
                                  &params, sizeof(params)))
                != NVOS_STATUS_SUCCESS) {
            nvEvoLogDevDebug(pDevEvo, EVO_LOG_ERROR,
                             "NvRmControl(GET_VIDLINK_ORDER) failed; "
                             "ret: %d\n", ret);
            return NULL;
        }

        if (params.ConnectionCount > 0) {
            RasterLockGroup *pRasterLockGroup;
            pRasterLockGroups = GrowRasterLockGroup(pRasterLockGroups,
                                                    pNumRasterLockGroups);

            if (!pRasterLockGroups) {
                return NULL;
            }

            pRasterLockGroup = &pRasterLockGroups[*pNumRasterLockGroups - 1];

            /*
             * For some reason this interface returns a mask instead of an
             * index, so we have to convert
             */
            for (i = 0; i < pDevEvo->numSubDevices; i++) {
                NvU32 subDeviceMask = params.Order[i];
                NvU32 sd = 0;

                nvAssert(nvPopCount32(subDeviceMask) == 1);

                if (!subDeviceMask) continue;

                while (!(subDeviceMask & (1 << sd))) sd++;

                nvAssert(sd < NVKMS_MAX_SUBDEVICES);
                nvAssert(pDevEvo->pDispEvo[sd] != NULL);

                /* SLI Mosaic. */
                AddDispEvoIntoRasterLockGroup(pRasterLockGroup,
                                              pDevEvo->pDispEvo[sd]);
            }
        }

        if (*pNumRasterLockGroups > 0) {
            return pRasterLockGroups;
        }
    }

    /*
     * Client SLI: Create a RasterLockGroup from pDevEvo's only DispEvo
     * and other DispEvos potentially bridged to that.
     */

    if (pDevEvo->numSubDevices == 1) {
        /* Get-or-create cached RasterLockGroup for this device. */
        if (!globalRasterLockGroups) {
            globalRasterLockGroups =
                GetRasterLockGroupsStateless(&numGlobalRasterLockGroups);
        }

        /* Look for a cached group containing this device's DispEvo. */
        if (globalRasterLockGroups && numGlobalRasterLockGroups > 0) {
            const RasterLockGroup *pRasterLockGroup =
                FindRasterLockGroupForDispEvo(globalRasterLockGroups,
                                              numGlobalRasterLockGroups,
                                              pDevEvo->pDispEvo[0]);

            /* Make a copy of it and add to 'pRasterLockGroups'. */
            if (pRasterLockGroup) {
                pRasterLockGroups =
                    CopyAndAppendRasterLockGroup(pRasterLockGroups,
                                                 pNumRasterLockGroups,
                                                 pRasterLockGroup);
            }
        }

        if (*pNumRasterLockGroups > 0) {
            return pRasterLockGroups;
        }
    }

    /*
     * Single GPU or bridgeless SLI. We create a group for each
     * individual DispEvo.
     */

    NVDispEvoPtr pDispEvo;
    unsigned int sd;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {
        RasterLockGroup *pRasterLockGroup;
        pRasterLockGroups = GrowRasterLockGroup(pRasterLockGroups,
                                                pNumRasterLockGroups);

        if (!pRasterLockGroups) {
            return NULL;
        }

        pRasterLockGroup = &pRasterLockGroups[*pNumRasterLockGroups - 1];

        AddDispEvoIntoRasterLockGroup(pRasterLockGroup, pDispEvo);
    }

    return pRasterLockGroups;
}

/*
 * ApplyLockActionIfPossible() - Check if the given action is a valid
 * transition for this pEvoSubDev's state, and apply it if so.
 * Return TRUE if any hardware state needs to be updated, FALSE o.w.
 */
static NvBool ApplyLockActionIfPossible(NVDispEvoPtr pDispEvo,
                                        NVEvoSubDevPtr pEvoSubDev,
                                        NVEvoLockAction action)
{
    if (!pEvoSubDev) {
        return FALSE;
    }

    if (pEvoSubDev->scanLockState(pDispEvo, pEvoSubDev,
                                  action, NULL)) {
        unsigned int i = 0;
        NvU32 pHeads[NVKMS_MAX_HEADS_PER_DISP + 1] = { NV_INVALID_HEAD, };
        NvU32 head;

        for (head = 0; head < NVKMS_MAX_HEADS_PER_DISP; head++) {
            if (nvHeadIsActive(pDispEvo, head)) {
                pHeads[i++] = head;
            }
        }
        nvAssert(i <= NVKMS_MAX_HEADS_PER_DISP);
        pHeads[i] = NV_INVALID_HEAD;

        pEvoSubDev->scanLockState(pDispEvo, pEvoSubDev, action, pHeads);

        return TRUE;
    }

    return FALSE;

} // ApplyLockActionIfPossible()

/*
 * Disable any intra-GPU lock state set up in FinishModesetOneDisp().
 * This assumes that any cross-GPU locking which may have been set up on this
 * GPU was already torn down.
 */
static void UnlockRasterLockOneDisp(NVDispEvoPtr pDispEvo)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NvU32 sd = pDispEvo->displayOwner;
    NVEvoSubDevPtr pEvoSubDev = &pDevEvo->gpus[sd];
    NvBool changed = FALSE;

    /* Initialize the assembly state */
    SyncEvoLockState();

    /* We want to evaluate all of these, so don't use || */
    changed |= ApplyLockActionIfPossible(pDispEvo, pEvoSubDev,
                                         NV_EVO_PROHIBIT_LOCK_DISABLE);
    changed |= ApplyLockActionIfPossible(pDispEvo, pEvoSubDev,
                                         NV_EVO_UNLOCK_HEADS);

    /* Update the hardware if anything has changed */
    if (changed) {
        UpdateEvoLockState();
    }

    pDispEvo->rasterLockPossible = FALSE;
}

/*
 * Call UnlockRasterLockOneDisp() for each disp on this device to tear down
 * intra-GPU locking on each.
 */
static void UnlockRasterLockOneDev(NVDevEvoPtr pDevEvo)
{
    NVDispEvoPtr pDispEvo;
    NvU32 sd;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {
        UnlockRasterLockOneDisp(pDispEvo);
    }
}

static void DisableLockGroupFlipLock(NVLockGroup *pLockGroup)
{

    const RasterLockGroup *pRasterLockGroup = &pLockGroup->rasterLockGroup;
    NvU32 i;

    if (!pLockGroup->flipLockEnabled) {
        return;
    }

    for (i = 0; i < pRasterLockGroup->numDisps; i++) {
        NVEvoUpdateState updateState = { };
        NVDispEvoPtr pDispEvo = pRasterLockGroup->pDispEvoOrder[i];
        NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
        NvU32 sd = pDispEvo->displayOwner;
        NVEvoSubDevPtr pEvoSubDev = &pDevEvo->gpus[sd];
        NvU32 head;
        NvBool changed = FALSE;

        for (head = 0; head < pDevEvo->numHeads; head++) {
            NvBool headChanged = FALSE;
            if (!nvHeadIsActive(pDispEvo, head)) {
                continue;
            }

            /*
             * scanLockState transitions (such as nvEvoLockHWStateLockHeads)
             * will update headControlAssy values for all heads, so we should
             * update flipLock and flipLockPin for all heads as well.
             */
            NVEvoHeadControlPtr pHC = &pEvoSubDev->headControlAssy[head];
            /*
             * Reset the fliplock pin, if it's not in use for framelock,
             * and unregister our use of the fliplock pin
             */
            if (!HEAD_MASK_QUERY(pEvoSubDev->flipLockPinSetForFrameLockHeadMask,
                                 head)) {
                if (pHC->flipLockPin != NV_EVO_LOCK_PIN_INTERNAL(0)) {
                    pHC->flipLockPin = NV_EVO_LOCK_PIN_INTERNAL(0);
                    headChanged = TRUE;
                }
            }
            pEvoSubDev->flipLockPinSetForSliHeadMask =
                HEAD_MASK_UNSET(pEvoSubDev->flipLockPinSetForSliHeadMask,
                                head);

            /*
             * Disable fliplock, if it's not in use for framelock, and
             * unregister our need for fliplock to be enabled
             */
            if (!HEAD_MASK_QUERY(pEvoSubDev->flipLockEnabledForFrameLockHeadMask,
                                 head)) {
                if (pHC->flipLock) {
                    pHC->flipLock = FALSE;
                    headChanged = TRUE;
                }
            }
            pEvoSubDev->flipLockEnabledForSliHeadMask =
                HEAD_MASK_UNSET(pEvoSubDev->flipLockEnabledForSliHeadMask,
                                head);
            if (headChanged) {
                EvoUpdateHeadParams(pDispEvo, head, &updateState);
            }
        }
        if (changed) {
            nvEvoUpdateAndKickOff(pDispEvo, TRUE, &updateState,
                                  TRUE /* releaseElv */);
        }
    }

    pLockGroup->flipLockEnabled = FALSE;
}

/*
 * Unlock cross-GPU locking in the given lock group.
 */
static void UnlockLockGroup(NVLockGroup *pLockGroup)
{
    RasterLockGroup *pRasterLockGroup;
    int i;

    if (pLockGroup == NULL) {
        return;
    }

    pRasterLockGroup = &pLockGroup->rasterLockGroup;

    DisableLockGroupFlipLock(pLockGroup);

    for (i = (int)pRasterLockGroup->numDisps - 1; i >= 0; i--) {
        NVDispEvoPtr pDispEvo = pRasterLockGroup->pDispEvoOrder[i];
        NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
        NvU32 sd = pDispEvo->displayOwner;
        NVEvoSubDevPtr pEvoSubDev = &pDevEvo->gpus[sd];

        /* Initialize the assembly state */
        SyncEvoLockState();

        if (ApplyLockActionIfPossible(pDispEvo, pEvoSubDev,
                                      NV_EVO_REM_SLI)) {
            /* Update the hardware if anything has changed */
            UpdateEvoLockState();
        }

        pEvoSubDev->flipLockProhibitedHeadMask = 0x0;

        nvAssert(pDispEvo->pLockGroup == pLockGroup);
        pDispEvo->pLockGroup = NULL;
    }

    /*
     * Disable any SLI video bridge features we may have enabled for locking.
     * This is a separate loop from the above in order to handle both cases:
     *
     * a) Multiple pDispEvos on the same pDevEvo (linked RM-SLI): all disps in
     *    the lock group will share the same pDevEvo.  In that case we should
     *    not call RM to disable the video bridge power across the entire
     *    device until we've disabled locking on all GPUs).  This loop will
     *    call nvEvoUpdateSliVideoBridge() redundantly for the same pDevEvo,
     *    but those calls will be filtered out.  (If we did this in the loop
     *    above, RM would broadcast the video bridge disable call to all pDisps
     *    on the first call, even before we've disabled locking on them.)
     *
     * b) Each pDispEvo on a separate pDevEvo (client-side SLI or no SLI, when
     *    a video bridge is present): in that case each pDispEvo has a separate
     *    pDevEvo, and we need to call nvEvoUpdateSliVideoBridge() on each.
     *    (It would be okay in this case to call nvEvoUpdateSliVideoBridge() in
     *    the loop above since it will only disable the video bridge power for
     *    one GPU at a time.)
     */
    for (i = (int)pRasterLockGroup->numDisps - 1; i >= 0; i--) {
        NVDispEvoPtr pDispEvo = pRasterLockGroup->pDispEvoOrder[i];
        NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;

        pDevEvo->sli.bridge.powerNeededForRasterLock = FALSE;
        nvEvoUpdateSliVideoBridge(pDevEvo);
    }

    nvFree(pLockGroup);
}

/*
 * Unlock all any cross-GPU locking in the rasterlock group(s) associated with
 * the given device.
 */
static void UnlockLockGroupsForDevice(NVDevEvoPtr pDevEvo)
{
    NVDispEvoPtr pDispEvo;
    NvU32 sd;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {
        UnlockLockGroup(pDispEvo->pLockGroup);
        nvAssert(pDispEvo->pLockGroup == NULL);
    }
}

void nvAssertAllDpysAreInactive(NVDevEvoPtr pDevEvo)
{
    NVDispEvoPtr pDispEvo;
    int i;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, i, pDevEvo) {
        NvU32 head;
        for (head = 0; head < NVKMS_MAX_HEADS_PER_DISP; head++) {
            nvAssert(!nvHeadIsActive(pDispEvo, head));
        }
    }
}

/*!
 * Disable locking-related state.
 */
static void DisableLockState(NVDevEvoPtr pDevEvo)
{
    NvU32 dispIndex;
    NVDispEvoPtr pDispEvo;

    /* Disable flip lock as requested by swap groups/framelock. */

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        nvToggleFlipLockPerDisp(pDispEvo,
                                nvGetActiveHeadMask(pDispEvo),
                                FALSE /* enable */);
    }

    /* Disable any locking across GPUs. */

    UnlockLockGroupsForDevice(pDevEvo);

    /* Disable intra-GPU rasterlock on this pDevEvo. */
    UnlockRasterLockOneDev(pDevEvo);

    /* Reset the EVO locking state machine. */

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        nvEvoStateAssertNoLock(&pDevEvo->gpus[pDispEvo->displayOwner]);
        nvEvoStateStartNoLock(&pDevEvo->gpus[pDispEvo->displayOwner]);
    }
}

void nvEvoLockStatePreModeset(NVDevEvoPtr pDevEvo)
{
    DisableLockState(pDevEvo);
}

/*!
 * Set up raster lock between GPUs, if applicable.
 */
void nvEvoLockStatePostModeset(NVDevEvoPtr pDevEvo, const NvBool doRasterLock)
{
    RasterLockGroup *pRasterLockGroups, *pRasterLockGroup;
    unsigned int numRasterLockGroups;

    if (!doRasterLock) {
        return;
    }

    FinishModesetOneDev(pDevEvo);

    pRasterLockGroups = GetRasterLockGroups(pDevEvo, &numRasterLockGroups);
    if (!pRasterLockGroups) {
        return;
    }

    for (pRasterLockGroup = pRasterLockGroups;
         pRasterLockGroup < pRasterLockGroups + numRasterLockGroups;
         pRasterLockGroup++) {
        FinishModesetOneGroup(pRasterLockGroup);
    }

    nvFree(pRasterLockGroups);
}

/*!
 * Updates the hardware based on software needs tracked in pDevEvo->sli.bridge.
 * Call this function after changing any of those needs variables.
 */
void nvEvoUpdateSliVideoBridge(NVDevEvoPtr pDevEvo)
{
    NV0080_CTRL_GPU_SET_VIDLINK_PARAMS params = { 0 };
    const NvBool enable = pDevEvo->sli.bridge.powerNeededForRasterLock;
    NvU32 status;

    if (pDevEvo->sli.bridge.powered == enable) {
        return;
    }

    if (enable) {
        /* SLI should be prohibited earlier if no bridge is present. */
        nvAssert(pDevEvo->sli.bridge.present);
    }

    params.enable = enable ?
        NV0080_CTRL_GPU_SET_VIDLINK_ENABLE_TRUE :
        NV0080_CTRL_GPU_SET_VIDLINK_ENABLE_FALSE;

    status = nvRmApiControl(nvEvoGlobal.clientHandle,
                            pDevEvo->deviceHandle,
                            NV0080_CTRL_CMD_GPU_SET_VIDLINK,
                            &params, sizeof(params));
    if (status != NV_OK) {
        nvAssert(!"NV0080_CTRL_CMD_GPU_SET_VIDLINK failed");
    }

    pDevEvo->sli.bridge.powered = enable;
}

/*
 * Check if VRR or MergeMode are enabled; if so, go into the special "prohibit
 * lock" mode which prevents other scanlock states from being reached.
 *
 * Return TRUE iff VRR or MergeMode is in use on this GPU.
 */
static NvBool ProhibitLockIfNecessary(NVDispEvoRec *pDispEvo)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NVEvoSubDevPtr pEvoSubDev = &pDevEvo->gpus[pDispEvo->displayOwner];
    NvU32 activeHeads[NVKMS_MAX_HEADS_PER_DISP + 1] = { NV_INVALID_HEAD, };
    NvBool prohibitLock = FALSE;
    NvU32 numActiveHeads = 0;
    NvU32 head;

    for (head = 0; head < NVKMS_MAX_HEADS_PER_DISP; head++) {
        if (nvHeadIsActive(pDispEvo, head)) {
            activeHeads[numActiveHeads++] = head;
            if ((pDispEvo->headState[head].timings.vrr.type !=
                 NVKMS_DPY_VRR_TYPE_NONE)) {
                prohibitLock = TRUE;
            }

            if (pDispEvo->headState[head].mergeMode !=
                    NV_EVO_MERGE_MODE_DISABLED) {
                prohibitLock = TRUE;
            }
        }
    }


    if (prohibitLock) {
        activeHeads[numActiveHeads] = NV_INVALID_HEAD;

        SyncEvoLockState();

        if (!pEvoSubDev->scanLockState(pDispEvo, pEvoSubDev,
                                       NV_EVO_PROHIBIT_LOCK,
                                       activeHeads)) {
            nvEvoLogDispDebug(pDispEvo, EVO_LOG_ERROR,
                              "Failed to prohibit lock");
            return FALSE;
        }

        UpdateEvoLockState();

        return TRUE;
    }
    return FALSE;
}


/*
 * Prohibit locking if necessary for the active configuration.
 *
 * Set up rasterlock between heads on a single GPU, if certain conditions are met:
 * - Locking is not prohibited due to the active configuration
 * - Opportunistic display sync is not disabled via kernel module parameter
 * - All active heads have identical mode timings
 *
 * Set pDispEvo->pRasterLockPossible to indicate whether rasterlock is possible
 * on this GPU, which will be used to determine if rasterlock is possible
 * between this GPU and other GPUs.
 * Note that this isn't the same as whether heads were locked: if fewer than
 * two heads were active, heads will not be locked but rasterlock with other
 * GPUs may still be possible.
 */
static void FinishModesetOneDisp(
    NVDispEvoRec *pDispEvo)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NVEvoSubDevPtr pEvoSubDev;
    const NVDispHeadStateEvoRec *pPrevHeadState = NULL;
    NvU32 head, usedHeads = 0;
    NvU32 headsToLock[NVKMS_MAX_HEADS_PER_DISP + 1] = { NV_INVALID_HEAD, };

    if (pDevEvo->gpus == NULL) {
        return;
    }

    pEvoSubDev = &pDevEvo->gpus[pDispEvo->displayOwner];

    pDispEvo->rasterLockPossible = FALSE;

    if (ProhibitLockIfNecessary(pDispEvo)) {
        /* If all locking is prohibited, do not attempt rasterlock. */
        return;
    }

    if (!nvkms_opportunistic_display_sync()) {
        /* If opportunistic display sync is disabled, do not attempt rasterlock. */
        return;
    }

    /*
     * Determine if rasterlock is possible: check each active display for
     * rasterlock compatibility with the previous one we looked at.  If any of
     * them aren't compatible, rasterlock is not possible.
     */
    pDispEvo->rasterLockPossible = TRUE;
    for (head = 0; head < NVKMS_MAX_HEADS_PER_DISP; head++) {
        const NVDispHeadStateEvoRec *pHeadState =
            &pDispEvo->headState[head];

        if (!nvHeadIsActive(pDispEvo, head)) {
            continue;
        }

        if (pPrevHeadState &&
            !RasterLockPossible(pHeadState, pPrevHeadState)) {
            pDispEvo->rasterLockPossible = FALSE;
            break;
        }

        pPrevHeadState = pHeadState;

        headsToLock[usedHeads] = head;
        usedHeads++;
    }

    if (!pDispEvo->rasterLockPossible) {
        return;
    }

    if (usedHeads > 1) {
        /* Terminate array */
        headsToLock[usedHeads] = NV_INVALID_HEAD;

        /* Initialize the assembly state */
        SyncEvoLockState();

        /* Set up rasterlock between heads on this disp. */
        nvAssert(headsToLock[0] != NV_INVALID_HEAD);
        if (!pEvoSubDev->scanLockState(pDispEvo, pEvoSubDev,
                                       NV_EVO_LOCK_HEADS,
                                       headsToLock)) {
            nvEvoLogDispDebug(pDispEvo, EVO_LOG_ERROR,
                              "Unable to lock heads");
            pDispEvo->rasterLockPossible = FALSE;
        }

        /* Update the hardware with the new state */
        UpdateEvoLockState();
    }
}

/* Call FinishModesetOneDisp() for each disp on this device to set up intra-GPU
 * locking on each. */
static void FinishModesetOneDev(
    NVDevEvoRec *pDevEvo)
{
    NVDispEvoPtr pDispEvo;
    NvU32 sd;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {
        FinishModesetOneDisp(pDispEvo);
    }
}

/*
 * Enable fliplock for the specified pLockGroup.
 * This assumes that rasterlock was already enabled.
 */
static void EnableLockGroupFlipLock(NVLockGroup *pLockGroup)
{
    const RasterLockGroup *pRasterLockGroup = &pLockGroup->rasterLockGroup;
    NvU32 i;

    if (pRasterLockGroup->numDisps < 2) {
        /* TODO: enable fliplock for single GPUs */
        return;
    }

    pLockGroup->flipLockEnabled = TRUE;

    for (i = 0; i < pRasterLockGroup->numDisps; i++) {
        NVEvoUpdateState updateState = { };
        NVDispEvoPtr pDispEvo = pRasterLockGroup->pDispEvoOrder[i];
        NvU32 sd = pDispEvo->displayOwner;
        NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
        NvU32 head;

        for (head = 0; head < pDevEvo->numHeads; head++) {
            NvU64 startTime = 0;

            if (!nvHeadIsActive(pDispEvo, head)) {
                continue;
            }

            NVEvoLockPin pin =
                nvEvoGetPinForSignal(pDispEvo, &pDevEvo->gpus[sd],
                                     NV_EVO_LOCK_SIGNAL_FLIP_LOCK);

            /* Wait for the raster lock to sync in.. */
            if (pin == NV_EVO_LOCK_PIN_ERROR ||
                !EvoWaitForLock(pDevEvo, sd, head, EVO_RASTER_LOCK,
                                &startTime)) {
                nvEvoLogDisp(pDispEvo, EVO_LOG_ERROR,
                    "Timed out waiting for rasterlock; not enabling fliplock.");
                goto fail;
            }

            /*
             * Enable fliplock, and register that we've enabled
             * fliplock for SLI to ensure it doesn't get disabled
             * later.
             */
            pDevEvo->gpus[sd].headControl[head].flipLockPin = pin;
            pDevEvo->gpus[sd].flipLockPinSetForSliHeadMask =
                HEAD_MASK_SET(pDevEvo->gpus[sd].flipLockPinSetForSliHeadMask, head);

            pDevEvo->gpus[sd].headControl[head].flipLock = TRUE;
            pDevEvo->gpus[sd].flipLockEnabledForSliHeadMask =
                HEAD_MASK_SET(pDevEvo->gpus[sd].flipLockEnabledForSliHeadMask, head);

            EvoUpdateHeadParams(pDispEvo, head, &updateState);
        }

         /*
         * This must be synchronous as EVO reports lock success if
         * locking isn't enabled, so we could race through the
         * WaitForLock check below otherwise.
         */
        nvEvoUpdateAndKickOff(pDispEvo, TRUE, &updateState,
                              TRUE /* releaseElv */);

        /*
         * Wait for flip lock sync.  I'm not sure this is really
         * necessary, but the docs say to do this before attempting any
         * flips in the base channel.
         */
        for (head = 0; head < pDevEvo->numHeads; head++) {
            NvU64 startTime = 0;

            if (!nvHeadIsActive(pDispEvo, head)) {
                continue;
            }

            if (!EvoWaitForLock(pDevEvo, sd, head, EVO_FLIP_LOCK,
                                &startTime)) {
                nvEvoLogDisp(pDispEvo, EVO_LOG_ERROR,
                    "Timed out waiting for fliplock.");
                goto fail;
            }
        }
    }

    return;
fail:
    DisableLockGroupFlipLock(pLockGroup);
}

/*
 * FinishModesetOneGroup() - Set up raster lock between GPUs, if applicable,
 * for one RasterLockGroup.  Called in a loop from nvFinishModesetEvo().
 */

static void FinishModesetOneGroup(RasterLockGroup *pRasterLockGroup)
{
    NVDispEvoPtr *pDispEvoOrder = pRasterLockGroup->pDispEvoOrder;
    NvU32 numUsedGpus = 0;
    const NVDispHeadStateEvoRec *pPrevHeadState = NULL;
    NvBool headInUse[NVKMS_MAX_SUBDEVICES][NVKMS_MAX_HEADS_PER_DISP];
    NvBool rasterLockPossible = TRUE, foundUnused = FALSE;
    unsigned int i, j;
    NVLockGroup *pLockGroup = NULL;

    /* Don't attempt locking across GPUs if, on any individual GPU, rasterlock
     * isn't possible. */
    for (i = 0; i < pRasterLockGroup->numDisps; i++) {
        NVDispEvoPtr pDispEvo = pDispEvoOrder[i];

        if (!pDispEvo->rasterLockPossible) {
            return;
        }
    }

    nvkms_memset(headInUse, 0, sizeof(headInUse));

    /*
     * Next, figure out if we can perform cross-GPU locking and which
     * GPUs/heads we can use.  Only attempt locking if all heads across GPUs
     * have compatible timings and are consecutive in the video bridge order.
     */
    for (i = 0; i < pRasterLockGroup->numDisps; i++) {
        NVDispEvoPtr pDispEvo = pDispEvoOrder[i];
        NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
        NvU32 head;

        /*
         * We can't lock if there is an unused GPU between two used GPUs on the
         * video bridge chain.
         * We much check if pDevEvo->gpus is NULL in case we haven't been
         * through AllocDeviceObject for this pDev (yet?).
         */
        if (!HasActiveHeads(pDispEvo) ||
            !pDevEvo->gpus) {
            foundUnused = TRUE;
            continue;
        } else {
            if (foundUnused) {
                rasterLockPossible = FALSE;
                break;
            }

            numUsedGpus++;
        }

        /*
         * Compare modetimings for each active display with the previous one we
         * looked at.  If any of them don't match, punt on locking.
         */
        for (head = 0; head < NVKMS_MAX_HEADS_PER_DISP; head++) {
            const NVDispHeadStateEvoRec *pHeadState =
                &pDispEvo->headState[head];

            if (!nvHeadIsActive(pDispEvo, head)) {
                continue;
            }

            if (pPrevHeadState &&
                !RasterLockPossible(pHeadState, pPrevHeadState)) {
                rasterLockPossible = FALSE;
                goto exitHeadLoop;
            }

            headInUse[i][head] = TRUE;

            pPrevHeadState = pHeadState;
        }

exitHeadLoop:
        if (!rasterLockPossible) {
            break;
        }
    }

    if (!rasterLockPossible || numUsedGpus == 0) {
        return;
    }

    /* Create a new lock group to store the current configuration */
    pLockGroup = nvCalloc(1, sizeof(*pLockGroup));

    if (pLockGroup == NULL) {
        return;
    }

    pLockGroup->rasterLockGroup = *pRasterLockGroup;

    /*
     * Finally, actually set up locking: go through the video bridge order
     * setting it up.
     */
    for (i = 0; i < pRasterLockGroup->numDisps; i++) {
        NVDispEvoPtr pDispEvo = pDispEvoOrder[i];
        NvU32 sd = pDispEvo->displayOwner;
        NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
        NvU32 head[NVKMS_MAX_HEADS_PER_DISP + 1] = { NV_INVALID_HEAD, };
        unsigned int usedHeads = 0;
        NvBool gpusLocked = FALSE;

        /* Remember that we've enabled this lock group on this GPU. */
        nvAssert(pDispEvo->pLockGroup == NULL);
        pDispEvo->pLockGroup = pLockGroup;

        /* If we're past the end of the chain, stop applying locking below, but
         * continue this loop to assign pDispEvo->pLockGroup above. */
        if (i >= numUsedGpus) {
            continue;
        }

        /* Initialize the assembly state */
        SyncEvoLockState();

        for (j = 0; j < NVKMS_MAX_HEADS_PER_DISP; j++) {
            if (headInUse[i][j]) {

                head[usedHeads] = j;

                usedHeads++;
            }
        }
        head[usedHeads] = NV_INVALID_HEAD;

        /* Then set up cross-GPU locking, if we have enough active GPUs */
        if (numUsedGpus > 1) {
            NVEvoLockAction action;
            NVEvoLockPin *pServerPin = &pDevEvo->gpus[sd].sliServerLockPin;
            NVEvoLockPin *pClientPin = &pDevEvo->gpus[sd].sliClientLockPin;

            *pServerPin = NV_EVO_LOCK_PIN_ERROR;
            *pClientPin = NV_EVO_LOCK_PIN_ERROR;

            if (i == 0) {
                action = NV_EVO_ADD_SLI_PRIMARY;
            } else {
                if (i == (numUsedGpus - 1)) {
                    action = NV_EVO_ADD_SLI_LAST_SECONDARY;
                } else {
                    action = NV_EVO_ADD_SLI_SECONDARY;
                }
            }

            if (action == NV_EVO_ADD_SLI_PRIMARY ||
                action == NV_EVO_ADD_SLI_SECONDARY) {
                /* Find pin for server to next */
                NVDispEvoPtr pDispEvoNext = pDispEvoOrder[i + 1];
                NvU32 headNext = 0;

                for (j = 0; j < NVKMS_MAX_HEADS_PER_DISP; j++) {
                    if (headInUse[i + 1][j]) {
                        headNext = j;
                        break;
                    }
                }

                GetRasterLockPin(pDispEvo, head[0],
                                 pDispEvoNext, headNext,
                                 pServerPin, NULL);
            }

            if (action == NV_EVO_ADD_SLI_SECONDARY ||
                action == NV_EVO_ADD_SLI_LAST_SECONDARY) {

                /* Find pin for client to prev */
                NVDispEvoPtr pDispEvoPrev = pDispEvoOrder[i - 1];
                NvU32 headPrev = 0;

                for (j = 0; j < NVKMS_MAX_HEADS_PER_DISP; j++) {
                    if (headInUse[i - 1][j]) {
                        headPrev = j;
                        break;
                    }
                }

                GetRasterLockPin(pDispEvo, head[0],
                                 pDispEvoPrev, headPrev,
                                 NULL, pClientPin);
            }

            if (!pDevEvo->gpus[sd].scanLockState(pDispEvo, &pDevEvo->gpus[sd],
                                                 action, head)) {
                nvEvoLogDispDebug(pDispEvo, EVO_LOG_ERROR,
                                  "Unable to set up SLI locking");
            } else {
                gpusLocked = TRUE;
            }
        }

        /*
         * On certain GPUs, we need to enable the video bridge (MIO pads) when
         * enabling rasterlock.  Note that we don't disable in this function,
         * so if gpusLocked is true for any iteration of these loops, this bit
         * will be on.
         */
        if (gpusLocked && NV0073_CTRL_SYSTEM_GET_CAP(pDevEvo->commonCapsBits,
                NV0073_CTRL_SYSTEM_CAPS_RASTER_LOCK_NEEDS_MIO_POWER)) {
            pDevEvo->sli.bridge.powerNeededForRasterLock = TRUE;
            nvEvoUpdateSliVideoBridge(pDevEvo);
        }

        /* If anything changed, update the hardware */
        if (gpusLocked) {
            UpdateEvoLockState();
        }
    }

    /* Enable fliplock, if we can */
    EnableFlipLockIfRequested(pLockGroup);
}

/*
 * Check if the given LockGroup matches the given FlipLockRequestedGroup.
 * This is true if the flip lock heads match the currently-active
 * heads on all pDispEvos.
 */
static NvBool CheckLockGroupMatchFlipLockRequestedGroup(
    const NVLockGroup *pLockGroup,
    const FlipLockRequestedGroup *pFLRG)
{
    const RasterLockGroup *pRasterLockGroup = &pLockGroup->rasterLockGroup;
    NvU32 disp, requestedDisp;

    /* Verify the number of disps is the same. */
    NvU32 numRequestedDisps = 0;
    for (requestedDisp = 0;
         requestedDisp < ARRAY_LEN(pFLRG->disp);
         requestedDisp++) {
        const NVDispEvoRec *pRequestedDispEvo =
            pFLRG->disp[requestedDisp].pDispEvo;
        if (pRequestedDispEvo == NULL) {
            break;
        }
        numRequestedDisps++;
    }
    if (numRequestedDisps != pRasterLockGroup->numDisps) {
        return FALSE;
    }

    /*
     * For each disp in the rasterlock group:
     * - If there is no matching disp in the pFLRG, no match
     * - If the disp's active head mask doesn't match the pFLRG's requested
     *   head mask for that disp, no match
     * If none of the conditions above failed, then we have a match.
     */
    for (disp = 0; disp < pRasterLockGroup->numDisps; disp++) {
        const NVDispEvoRec *pDispEvo = pRasterLockGroup->pDispEvoOrder[disp];
        NvBool found = FALSE;
        for (requestedDisp = 0;
             requestedDisp < ARRAY_LEN(pFLRG->disp);
             requestedDisp++) {
            const NVDispEvoRec *pRequestedDispEvo =
                pFLRG->disp[requestedDisp].pDispEvo;
            if (pRequestedDispEvo == NULL) {
                break;
            }
            if (pRequestedDispEvo == pDispEvo) {
                if (pFLRG->disp[requestedDisp].flipLockHeads !=
                    nvGetActiveHeadMask(pDispEvo)) {
                    return FALSE;
                }
                found = TRUE;
                break;
            }
        }
        if (!found) {
            return FALSE;
        }
    }

    return TRUE;
}

/*
 * Check if any requested fliplock groups match this lockgroup; if so, enable
 * fliplock on the lockgroup.
 */
static void EnableFlipLockIfRequested(NVLockGroup *pLockGroup)
{
    FlipLockRequestedGroup *pFLRG;
    nvListForEachEntry(pFLRG, &requestedFlipLockGroups, listEntry) {
        if (CheckLockGroupMatchFlipLockRequestedGroup(pLockGroup, pFLRG)) {
            EnableLockGroupFlipLock(pLockGroup);
            break;
        }
    }
}

/*
 * Check if there is an active NVLockGroup that matches the given
 * FlipLockRequestedGroup.
 * "Matches" means that the NVLockGroup extends to the exact same GPUs as the
 * FlipLockRequestedGroup, and that the *active* heads on those GPUs exactly
 * match the heads requested in the FlipLockRequestedGroup.
 */
static NVLockGroup *FindMatchingLockGroup(const FlipLockRequestedGroup *pFLRG)
{
    /* If there is an active lock group that matches this pFLRG, it must also
     * be active on the first disp, so we don't need to bother looping over
     * all disps. */
    NVLockGroup *pLockGroup = pFLRG->disp[0].pDispEvo->pLockGroup;

    if (pLockGroup != NULL &&
        CheckLockGroupMatchFlipLockRequestedGroup(pLockGroup, pFLRG)) {
        return pLockGroup;
    }
    return NULL;
}

/* Disable any currently-active lock groups that match the given pFLRG */
static void
DisableRequestedFlipLockGroup(const FlipLockRequestedGroup *pFLRG)
{
    NVLockGroup *pLockGroup = FindMatchingLockGroup(pFLRG);
    if (pLockGroup != NULL) {
        DisableLockGroupFlipLock(pLockGroup);

        nvAssert(!pLockGroup->flipLockEnabled);
    }
}

/*
 * Check if there is a currently-active rasterlock group that matches the
 * disps/heads of this FlipLockRequestedGroup.  If so, enable flip lock between
 * those heads.
 */
static void
EnableRequestedFlipLockGroup(const FlipLockRequestedGroup *pFLRG)
{
    NVLockGroup *pLockGroup = FindMatchingLockGroup(pFLRG);
    if (pLockGroup != NULL) {
        EnableLockGroupFlipLock(pLockGroup);
    }
}

/*
 * Convert the given API head mask to a HW head mask, using the
 * currently-active API head->HW head mapping.
 */
static NvU32 ApiHeadMaskToHwHeadMask(
    const NVDispEvoRec *pDispEvo,
    const NvU32 apiHeadMask)
{
    const NvU32 numHeads = pDispEvo->pDevEvo->numHeads;
    NvU32 apiHead;
    NvU32 hwHeadMask = 0;

    for (apiHead = 0; apiHead < numHeads; apiHead++) {
        if ((apiHeadMask & (1 << apiHead)) != 0) {
            const NVDispApiHeadStateEvoRec *pApiHeadState =
                &pDispEvo->apiHeadState[apiHead];
            if (nvApiHeadIsActive(pDispEvo, apiHead)) {
                hwHeadMask |= pApiHeadState->hwHeadsMask;
            }
        }
    }

    return hwHeadMask;
}

/*
 * Return true if all main channels are idle on the heads specified in the
 * FlipLockRequestedGroup.
 */
static NvBool CheckFlipLockGroupIdle(
    const FlipLockRequestedGroup *pFLRG)
{
    NvU32 i;

    for (i = 0; i < ARRAY_LEN(pFLRG->disp); i++) {
        NVDispEvoPtr pDispEvo = pFLRG->disp[i].pDispEvo;
        if (pDispEvo != NULL) {
            NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
            const NvU32 sd = pDispEvo->displayOwner;
            const NvU32 numHeads = pDevEvo->numHeads;
            NvU32 head;

            for (head = 0; head < numHeads; head++) {
                NvBool isMethodPending;
                if (!nvHeadIsActive(pDispEvo, head)) {
                    continue;
                }
                if (!pDevEvo->hal->IsChannelMethodPending(
                        pDevEvo,
                        pDevEvo->head[head].layer[NVKMS_MAIN_LAYER],
                        sd,
                        &isMethodPending) || isMethodPending) {
                    return FALSE;
                }
            }
        }
    }

    return TRUE;
}

/*
 * Return true if all main channels are idle on each head in overlapping flip
 * lock groups.
 */
static NvBool CheckOverlappingFlipLockRequestGroupsIdle(
    NVDevEvoRec *pDevEvo[NV_MAX_SUBDEVICES],
    const struct NvKmsSetFlipLockGroupRequest *pRequest)
{
    NvU32 dev;

    /* Loop over the GPUs specified in this FlipLockGroupRequest */
    for (dev = 0; dev < NV_MAX_SUBDEVICES && pDevEvo[dev] != NULL; dev++) {
        NVDispEvoPtr pDispEvo;
        NvU32 sd;

        FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo[dev]) {
            FlipLockRequestedGroup *pFLRG;

            if ((pRequest->dev[dev].requestedDispsBitMask & (1 << sd)) == 0) {
                continue;
            }

            /*
             * For each specified GPU, search through existing requested
             * fliplock groups and find any that overlap with heads in this
             * request.
             *
             * Return FALSE if any overlapping fliplock groups are not idle.
             */
            nvListForEachEntry(pFLRG, &requestedFlipLockGroups, listEntry) {
                NvU32 i;
                for (i = 0; i < ARRAY_LEN(pFLRG->disp); i++) {
                    if (pFLRG->disp[i].pDispEvo == NULL) {
                        break;
                    }
                    if (pFLRG->disp[i].pDispEvo == pDispEvo) {
                        /* API heads requested for this disp by the client */
                        const NvU32 requestedApiHeadMask =
                            pRequest->dev[dev].disp[sd].requestedHeadsBitMask;
                        const NvU32 requestedHwHeadMask =
                            ApiHeadMaskToHwHeadMask(pDispEvo, requestedApiHeadMask);

                        if ((requestedHwHeadMask &
                             pFLRG->disp[i].flipLockHeads) != 0) {
                            /* Match */
                            if (!CheckFlipLockGroupIdle(pFLRG)) {
                                return FALSE;
                            }
                        }
                        break;
                    }
                }
            }
        }
    }

    return TRUE;
}

/*
 * Disable and remove any FlipLockRequestGroups that contain any of the heads
 * in 'hwHeadsMask' on the given pDispEvo.
 */
static void
RemoveOverlappingFlipLockRequestGroupsOneDisp(
    NVDispEvoRec *pDispEvo,
    NvU32 hwHeadMask)
{
    FlipLockRequestedGroup *pFLRG, *tmp;

    /*
     * For each specified GPU, search through existing requested
     * fliplock groups and find any that overlap with heads in this
     * request.
     *
     * For any that are found, disable fliplock and remove the
     * requested flip lock group.
     */
    nvListForEachEntry_safe(pFLRG, tmp, &requestedFlipLockGroups, listEntry) {
        NvU32 i;

        for (i = 0; i < ARRAY_LEN(pFLRG->disp); i++) {
            if (pFLRG->disp[i].pDispEvo == NULL) {
                break;
            }
            if (pFLRG->disp[i].pDispEvo == pDispEvo) {

                if ((hwHeadMask &
                     pFLRG->disp[i].flipLockHeads) != 0) {
                    /* Match */
                    DisableRequestedFlipLockGroup(pFLRG);

                    /* Remove from global list */
                    nvListDel(&pFLRG->listEntry);
                    nvFree(pFLRG);
                }
                break;
            }
        }
    }
}

/*
 * Disable and remove any FlipLockRequestGroups that contain any of the heads
 * specified in 'pRequest'.
 */
static void
RemoveOverlappingFlipLockRequestGroups(
    NVDevEvoRec *pDevEvo[NV_MAX_SUBDEVICES],
    const struct NvKmsSetFlipLockGroupRequest *pRequest)
{
    NvU32 dev;

    /* Loop over the GPUs specified in this FlipLockGroupRequest */
    for (dev = 0; dev < NV_MAX_SUBDEVICES && pDevEvo[dev] != NULL; dev++) {
        NVDispEvoPtr pDispEvo;
        NvU32 sd;

        FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo[dev]) {
            NvU32 requestedApiHeadMask, requestedHwHeadMask;

            if ((pRequest->dev[dev].requestedDispsBitMask & (1 << sd)) == 0) {
                continue;
            }

            /* API heads requested for this disp by the client */
            requestedApiHeadMask =
                pRequest->dev[dev].disp[sd].requestedHeadsBitMask;
            requestedHwHeadMask =
                ApiHeadMaskToHwHeadMask(pDispEvo, requestedApiHeadMask);

            RemoveOverlappingFlipLockRequestGroupsOneDisp(pDispEvo,
                                                          requestedHwHeadMask);
        }
    }
}

/*
 * Disable and remove any FlipLockRequestGroups that contain any of the heads
 * specified in 'pRequest'.
 */
void nvEvoRemoveOverlappingFlipLockRequestGroupsForModeset(
    NVDevEvoPtr pDevEvo,
    const struct NvKmsSetModeRequest *pRequest)
{
    NVDispEvoPtr pDispEvo;
    NvU32 sd;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {
        NvU32 requestedApiHeadMask, requestedHwHeadMask;

        if ((pRequest->requestedDispsBitMask & (1 << sd)) == 0) {
            continue;
        }

        /* API heads requested for this disp by the client */
        requestedApiHeadMask =
            pRequest->disp[sd].requestedHeadsBitMask;
        requestedHwHeadMask =
            ApiHeadMaskToHwHeadMask(pDispEvo, requestedApiHeadMask);

        RemoveOverlappingFlipLockRequestGroupsOneDisp(pDispEvo,
                                                      requestedHwHeadMask);
    }
}

/*!
 * Handle a NVKMS_IOCTL_SET_FLIPLOCK_GROUP request.  This assumes that the
 * request was already validated by nvkms.c:SetFlipLockGroup().
 *
 * param[in]  pDevEvo  Array of NVDevEvoPtr pointers, in the same order as
 *                     the deviceHandle were specified in the request.
 * param[in]  pRequest The ioctl request.
 */
NvBool
nvSetFlipLockGroup(NVDevEvoRec *pDevEvo[NV_MAX_SUBDEVICES],
                   const struct NvKmsSetFlipLockGroupRequest *pRequest)
{
    FlipLockRequestedGroup *pFLRG = NULL;

    /* Construct the new RequestedFlipLockGroup first, so if it fails we can
     * return before removing overlapping groups. */
    if (pRequest->enable) {
        NvU32 dev, disp;

        pFLRG = nvCalloc(1, sizeof(*pFLRG));
        if (pFLRG == NULL) {
            goto fail;
        }

        disp = 0;
        for (dev = 0; dev < NV_MAX_SUBDEVICES && pDevEvo[dev] != NULL; dev++) {
            NVDispEvoPtr pDispEvo;
            NvU32 sd;

            FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo[dev]) {
                const NvU32 requestedApiHeads =
                    pRequest->dev[dev].disp[sd].requestedHeadsBitMask;

                if ((pRequest->dev[dev].requestedDispsBitMask & (1 << sd)) == 0) {
                    continue;
                }

                if (disp >= ARRAY_LEN(pFLRG->disp)) {
                    nvAssert(!"FlipLockRequestedGroup::disp too short?");
                    goto fail;
                }

                pFLRG->disp[disp].pDispEvo = pDispEvo;
                pFLRG->disp[disp].flipLockHeads =
                    ApiHeadMaskToHwHeadMask(pDispEvo, requestedApiHeads);
                disp++;
            }
        }

        if (!CheckFlipLockGroupIdle(pFLRG)) {
            nvEvoLogDebug(EVO_LOG_ERROR,
                          "Failed to request flip lock: group not idle");
            goto fail;
        }
    }

    if (!CheckOverlappingFlipLockRequestGroupsIdle(pDevEvo, pRequest)) {
        nvEvoLogDebug(EVO_LOG_ERROR,
                      "Failed to request flip lock: overlapping group(s) not idle");
        goto fail;
    }

    RemoveOverlappingFlipLockRequestGroups(pDevEvo, pRequest);

    if (pFLRG) {
        nvListAdd(&pFLRG->listEntry, &requestedFlipLockGroups);

        EnableRequestedFlipLockGroup(pFLRG);
    }

    return TRUE;

fail:
    nvFree(pFLRG);
    return FALSE;
}

NvBool nvSetUsageBoundsEvo(
    NVDevEvoPtr pDevEvo,
    const NvU32 sd,
    const NvU32 head,
    const struct NvKmsUsageBounds *pUsage,
    NVEvoUpdateState *updateState)
{
    NvBool needCoreUpdate;

    nvPushEvoSubDevMask(pDevEvo, NVBIT(sd));

    needCoreUpdate = pDevEvo->hal->SetUsageBounds(pDevEvo, sd, head, pUsage,
                                                  updateState);

    nvPopEvoSubDevMask(pDevEvo);

    pDevEvo->gpus[sd].headState[head].usage = *pUsage;

    return needCoreUpdate;
}

void nvEnableMidFrameAndDWCFWatermark(NVDevEvoPtr pDevEvo,
                                      NvU32 sd,
                                      NvU32 head,
                                      NvBool enable,
                                      NVEvoUpdateState *pUpdateState)
{
    pDevEvo->gpus[sd].headState[head].
        disableMidFrameAndDWCFWatermark = !enable;

    if (pDevEvo->hal->EnableMidFrameAndDWCFWatermark == NULL) {
        nvEvoLogDev(pDevEvo,
                    EVO_LOG_ERROR,
                    "EnableMidFrameAndDWCFWatermark() is not defined");
        return;
    }

    pDevEvo->hal->EnableMidFrameAndDWCFWatermark(pDevEvo,
                                                 sd,
                                                 head,
                                                 enable,
                                                 pUpdateState);
}

static NvBool GetDefaultColorSpace(
    const NvKmsDpyOutputColorFormatInfo *pColorFormatsInfo,
    enum NvKmsDpyAttributeCurrentColorSpaceValue *pColorSpace,
    enum NvKmsDpyAttributeColorBpcValue *pColorBpc)
{
    if (pColorFormatsInfo->rgb444.maxBpc !=
            NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_UNKNOWN) {
        *pColorSpace = NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_RGB;
        *pColorBpc = pColorFormatsInfo->rgb444.maxBpc;
        return TRUE;
    }

    if (pColorFormatsInfo->yuv444.maxBpc !=
            NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_UNKNOWN) {
        *pColorSpace = NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr444;
        *pColorBpc = pColorFormatsInfo->yuv444.maxBpc;
        return TRUE;
    }

    if (pColorFormatsInfo->yuv422.maxBpc !=
            NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_UNKNOWN) {
        *pColorSpace = NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr422;
        *pColorBpc = pColorFormatsInfo->yuv422.maxBpc;
        return TRUE;
    }

    return FALSE;
}

NvBool nvGetDefaultDpyColor(
    const NvKmsDpyOutputColorFormatInfo *pColorFormatsInfo,
    NVDpyAttributeColor *pDpyColor)
{
    nvkms_memset(pDpyColor, 0, sizeof(*pDpyColor));

    if (!GetDefaultColorSpace(pColorFormatsInfo, &pDpyColor->format,
                              &pDpyColor->bpc)) {
        return FALSE;
    }

    if (pDpyColor->format != NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_RGB) {
        pDpyColor->range = NV_KMS_DPY_ATTRIBUTE_COLOR_RANGE_LIMITED;
    } else {
        pDpyColor->range = NV_KMS_DPY_ATTRIBUTE_COLOR_RANGE_FULL;
    }

    pDpyColor->colorimetry = NVKMS_OUTPUT_COLORIMETRY_DEFAULT;

    return TRUE;
}

NvBool nvChooseColorRangeEvo(
    enum NvKmsOutputColorimetry colorimetry,
    const enum NvKmsDpyAttributeColorRangeValue requestedColorRange,
    const enum NvKmsDpyAttributeCurrentColorSpaceValue colorSpace,
    const enum NvKmsDpyAttributeColorBpcValue colorBpc,
    enum NvKmsDpyAttributeColorRangeValue *pColorRange)
{
    /* Hardware supports BPC_6 only for RGB */
    nvAssert((colorSpace == NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_RGB) ||
                (colorBpc != NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_6));

    if ((colorSpace == NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_RGB) &&
            (colorBpc == NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_6)) {
        /* At depth 18 only RGB and full range are allowed */
        if (colorimetry == NVKMS_OUTPUT_COLORIMETRY_BT2100) {
            /* BT2100 requires limited color range */
            return FALSE;
        }
        *pColorRange = NV_KMS_DPY_ATTRIBUTE_COLOR_RANGE_FULL;
    } else if ((colorSpace == NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr444) ||
               (colorSpace == NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr422) ||
               (colorSpace == NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr420) ||
               (colorimetry == NVKMS_OUTPUT_COLORIMETRY_BT2100)) {
        /* Both YUV and BT2100 colorimetry require limited color range. */
        *pColorRange = NV_KMS_DPY_ATTRIBUTE_COLOR_RANGE_LIMITED;
    } else {
        *pColorRange = requestedColorRange;
    }

    return TRUE;
}

static enum NvKmsDpyAttributeColorBpcValue ChooseColorBpc(
    const enum NvKmsDpyAttributeColorBpcValue requested,
    const enum NvKmsDpyAttributeColorBpcValue max,
    const enum NvKmsDpyAttributeColorBpcValue min)
{
    if ((requested == NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_UNKNOWN) ||
            (requested > max)) {
        return max;
    }

    if (requested < min) {
        return min;
    }

    return requested;
}

/*!
 * Choose current colorSpace and colorRange for the given dpy based on
 * the dpy's color format capailities, the given modeset parameters (YUV420
 * mode and output transfer function) and the requested color space and range.
 *
 * This needs to be called during a modeset as well as when the requested color
 * space or range have changed.
 *
 * If SW YUV420 mode is enabled, EVO HW is programmed with default (RGB color
 * space, FULL color range) values, and the real values are used in a
 * headSurface composite shader.
 */
NvBool nvChooseCurrentColorSpaceAndRangeEvo(
    const NVDpyEvoRec *pDpyEvo,
    const enum NvYuv420Mode yuv420Mode,
    enum NvKmsOutputColorimetry colorimetry,
    const enum NvKmsDpyAttributeRequestedColorSpaceValue requestedColorSpace,
    const enum NvKmsDpyAttributeColorBpcValue requestedColorBpc,
    const enum NvKmsDpyAttributeColorRangeValue requestedColorRange,
    enum NvKmsDpyAttributeCurrentColorSpaceValue *pCurrentColorSpace,
    enum NvKmsDpyAttributeColorBpcValue *pCurrentColorBpc,
    enum NvKmsDpyAttributeColorRangeValue *pCurrentColorRange)
{
    enum NvKmsDpyAttributeCurrentColorSpaceValue newColorSpace =
        NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_RGB;
    enum NvKmsDpyAttributeColorBpcValue newColorBpc =
        NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_10;
    enum NvKmsDpyAttributeColorRangeValue newColorRange =
        NV_KMS_DPY_ATTRIBUTE_COLOR_RANGE_FULL;
    const NvKmsDpyOutputColorFormatInfo colorFormatsInfo =
        nvDpyGetOutputColorFormatInfo(pDpyEvo);

    // XXX HDR TODO: Handle other colorimetries
    // XXX HDR TODO: Handle YUV
    if (colorimetry == NVKMS_OUTPUT_COLORIMETRY_BT2100) {
        /*
         * If the head currently has BT2100 colorimetry, we override the
         * requested color space with RGB.  We cannot support yuv420Mode in
         * that configuration, so fail in that case.
         */
        if (yuv420Mode != NV_YUV420_MODE_NONE) {
            return FALSE;
        }

        newColorSpace = NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_RGB;
        newColorBpc = colorFormatsInfo.rgb444.maxBpc;
    } else if (yuv420Mode != NV_YUV420_MODE_NONE) {
        /*
         * If the current mode timing requires YUV420 compression, we override the
         * requested color space with YUV420.
         */
        newColorSpace = NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr420;
        newColorBpc = NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_8;

        nvAssert(colorFormatsInfo.rgb444.maxBpc >=
                    NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_8);
    } else {
        /*
         * Note this is an assignment between different enum types. Checking the
         * value of requested colorSpace and then assigning the value to current
         * colorSpace, to avoid warnings about cross-enum assignment.
         */
        switch (requestedColorSpace) {
        case NV_KMS_DPY_ATTRIBUTE_REQUESTED_COLOR_SPACE_RGB:
            newColorSpace = NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_RGB;
            newColorBpc = ChooseColorBpc(requestedColorBpc,
                                         colorFormatsInfo.rgb444.maxBpc,
                                         colorFormatsInfo.rgb444.minBpc);
            break;
        case NV_KMS_DPY_ATTRIBUTE_REQUESTED_COLOR_SPACE_YCbCr422:
            newColorSpace = NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr422;
            newColorBpc = ChooseColorBpc(requestedColorBpc,
                                         colorFormatsInfo.yuv422.maxBpc,
                                         colorFormatsInfo.yuv422.minBpc);
            break;
        case NV_KMS_DPY_ATTRIBUTE_REQUESTED_COLOR_SPACE_YCbCr444:
            newColorSpace = NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr444;
            newColorBpc = ChooseColorBpc(requestedColorBpc,
                                         colorFormatsInfo.yuv444.maxBpc,
                                         colorFormatsInfo.yuv444.minBpc);
            break;
        default:
            nvAssert(!"Invalid Requested ColorSpace");
        }

        if ((newColorBpc ==
                NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_UNKNOWN) &&
            !GetDefaultColorSpace(&colorFormatsInfo, &newColorSpace,
                                  &newColorBpc)) {
            return FALSE;
        }
    }

    // 10 BPC required for HDR
    // XXX HDR TODO: Handle other colorimetries
    // XXX HDR TODO: Handle YUV
    if ((colorimetry == NVKMS_OUTPUT_COLORIMETRY_BT2100) &&
        (newColorBpc < NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_10)) {
        return FALSE;
    }

    if (!nvChooseColorRangeEvo(colorimetry, requestedColorRange, newColorSpace,
                               newColorBpc, &newColorRange)) {
    }

    *pCurrentColorSpace = newColorSpace;
    *pCurrentColorRange = newColorRange;
    *pCurrentColorBpc = newColorBpc;

    return TRUE;
}

void nvUpdateCurrentHardwareColorSpaceAndRangeEvo(
    NVDispEvoPtr pDispEvo,
    const NvU32 head,
    const NVDpyAttributeColor *pDpyColor,
    NVEvoUpdateState *pUpdateState)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NVDispHeadStateEvoRec *pHeadState = &pDispEvo->headState[head];
    const NVConnectorEvoRec *pConnectorEvo = pHeadState->pConnectorEvo;

    nvAssert(pConnectorEvo != NULL);

    // XXX HDR TODO: Support more output colorimetries
    if (pDpyColor->colorimetry == NVKMS_OUTPUT_COLORIMETRY_BT2100) {
        nvAssert(pHeadState->timings.yuv420Mode == NV_YUV420_MODE_NONE);
        nvAssert(pDpyColor->format == NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_RGB);
        nvAssert(pDpyColor->range == NV_KMS_DPY_ATTRIBUTE_COLOR_RANGE_LIMITED);

        pHeadState->procAmp.colorimetry =  NVT_COLORIMETRY_BT2020RGB;
        pHeadState->procAmp.colorRange = NVT_COLOR_RANGE_LIMITED;
        pHeadState->procAmp.colorFormat = NVT_COLOR_FORMAT_RGB;
    } else if ((pHeadState->timings.yuv420Mode == NV_YUV420_MODE_SW) &&
               (pDpyColor->format ==
                NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr420)) {
        /*
         * In SW YUV420 mode, HW is programmed with RGB color space and full
         * color range.  The color space conversion and color range compression
         * happen in a headSurface composite shader.
         */
        pHeadState->procAmp.colorimetry = NVT_COLORIMETRY_RGB;
        pHeadState->procAmp.colorRange = NVT_COLOR_RANGE_FULL;
        pHeadState->procAmp.colorFormat = NVT_COLOR_FORMAT_RGB;
    } else {

        // Set default colorimetry to RGB and default color range to full
        pHeadState->procAmp.colorimetry = NVT_COLORIMETRY_RGB;
        pHeadState->procAmp.colorRange = NVT_COLOR_RANGE_FULL;

        // Set color format
        switch (pDpyColor->format) {
        case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_RGB:
            pHeadState->procAmp.colorFormat = NVT_COLOR_FORMAT_RGB;
            break;
        case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr444:
            pHeadState->procAmp.colorFormat = NVT_COLOR_FORMAT_YCbCr444;
            break;
        case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr422:
            pHeadState->procAmp.colorFormat = NVT_COLOR_FORMAT_YCbCr422;
            break;
        case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr420:
            pHeadState->procAmp.colorFormat = NVT_COLOR_FORMAT_YCbCr420;
            break;
        default:
            nvAssert(!"unrecognized colorSpace");
        }

        switch (pConnectorEvo->legacyType) {
        case NV0073_CTRL_SPECIFIC_DISPLAY_TYPE_DFP:
            // program HW with RGB/YCbCr
            switch (pDpyColor->format) {
            case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_RGB:
                pHeadState->procAmp.colorimetry = NVT_COLORIMETRY_RGB;
                break;
            case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr444:
            case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr422:
            case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr420:
                if (nvEvoIsHDQualityVideoTimings(&pHeadState->timings)) {
                    pHeadState->procAmp.colorimetry = NVT_COLORIMETRY_YUV_709;
                } else {
                    pHeadState->procAmp.colorimetry = NVT_COLORIMETRY_YUV_601;
                }
                break;
            default:
                nvAssert(!"unrecognized colorSpace");
            }
            break;
        case NV0073_CTRL_SPECIFIC_DISPLAY_TYPE_CRT:
            // colorSpace isn't used for DEVICE_TYPE_CRT and
            // hence should be set to the "unchanged" value
            // (i.e. the default - RGB)
            nvAssert(pDpyColor->format ==
                        NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_RGB);

            // program HW with RGB only
            pHeadState->procAmp.colorimetry = NVT_COLORIMETRY_RGB;
            break;
        default:
            nvAssert(!"ERROR: invalid pDpyEvo->type");
        }

        /* YCbCr444 should be advertise only for DisplayPort and HDMI */
        nvAssert((pDpyColor->format !=
                    NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr444) ||
                    nvConnectorUsesDPLib(pConnectorEvo) ||
                    pConnectorEvo->isHdmiEnabled);

        /* YcbCr422 should be advertised only for HDMI and DP on supported GPUs */
        nvAssert((pDpyColor->format !=
                    NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr422) ||
                     (((pDevEvo->caps.hdmiYCbCr422MaxBpc != 0) &&
                       pConnectorEvo->isHdmiEnabled)) ||
                      ((pDevEvo->caps.dpYCbCr422MaxBpc != 0) &&
                       nvConnectorUsesDPLib(pConnectorEvo)));

        switch (pDpyColor->range) {
        case NV_KMS_DPY_ATTRIBUTE_COLOR_RANGE_FULL:
            pHeadState->procAmp.colorRange = NVT_COLOR_RANGE_FULL;
            break;
        case NV_KMS_DPY_ATTRIBUTE_COLOR_RANGE_LIMITED:
            pHeadState->procAmp.colorRange = NVT_COLOR_RANGE_LIMITED;
            break;
        default:
            nvAssert(!"Invalid colorRange");
            break;
        }
    }

    // In YUV colorimetry, only limited color range is allowed.
    nvAssert(!((pHeadState->procAmp.colorimetry != NVT_COLORIMETRY_RGB) &&
               (pHeadState->procAmp.colorRange != NVT_COLOR_RANGE_LIMITED)));

    // Limited color range is not allowed with 18bpp mode
    nvAssert(!((pHeadState->pixelDepth == NVKMS_PIXEL_DEPTH_18_444) &&
               (pHeadState->procAmp.colorRange == NVT_COLOR_RANGE_LIMITED)));

    nvPushEvoSubDevMaskDisp(pDispEvo);

    // Set the procamp head method
    pDevEvo->hal->SetProcAmp(pDispEvo, head, pUpdateState);

    // Clean up
    nvPopEvoSubDevMask(pDevEvo);
}

void nvEvoHeadSetControlOR(NVDispEvoPtr pDispEvo,
                           const NvU32 head, NVEvoUpdateState *pUpdateState)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    const NVDispHeadStateEvoPtr pHeadState = &pDispEvo->headState[head];
    const NVHwModeTimingsEvo *pTimings = &pHeadState->timings;
    const enum nvKmsPixelDepth pixelDepth = pHeadState->pixelDepth;
    NvBool colorSpaceOverride = FALSE;

    /*
     * Determine whether or not this dpy will need its color space
     * overridden.
     *
     * This is currently only used for DP 1.3 YUV420 mode, where the
     * HW's normal support for carrying color space information
     * together with the frame is insufficient.
     */
    if ((pTimings->yuv420Mode == NV_YUV420_MODE_SW) &&
        nvConnectorUsesDPLib(pHeadState->pConnectorEvo)) {

        nvAssert(pDispEvo->pDevEvo->caps.supportsDP13);
        colorSpaceOverride = TRUE;
    }

    // Only set up the actual output for SLI primary.
    nvPushEvoSubDevMask(pDevEvo, 1 << pDispEvo->displayOwner);

    pDevEvo->hal->HeadSetControlOR(pDevEvo, head, pTimings, pixelDepth,
                                   colorSpaceOverride,
                                   pUpdateState);

    nvPopEvoSubDevMask(pDevEvo);
}

static const struct {
    NvU32 algo;
    enum NvKmsDpyAttributeCurrentDitheringModeValue nvKmsDitherMode;
} ditherModeTable[] = {
    { NV0073_CTRL_SPECIFIC_OR_DITHER_ALGO_DYNAMIC_2X2,
      NV_KMS_DPY_ATTRIBUTE_CURRENT_DITHERING_MODE_DYNAMIC_2X2 },
    { NV0073_CTRL_SPECIFIC_OR_DITHER_ALGO_STATIC_2X2,
      NV_KMS_DPY_ATTRIBUTE_CURRENT_DITHERING_MODE_STATIC_2X2 },
    { NV0073_CTRL_SPECIFIC_OR_DITHER_ALGO_TEMPORAL,
      NV_KMS_DPY_ATTRIBUTE_CURRENT_DITHERING_MODE_TEMPORAL },
    { NV0073_CTRL_SPECIFIC_OR_DITHER_ALGO_UNKNOWN,
      NV_KMS_DPY_ATTRIBUTE_CURRENT_DITHERING_MODE_NONE }
};

static const struct {
    NvU32 type;
    enum NvKmsDpyAttributeCurrentDitheringDepthValue nvKmsDitherDepth;
} ditherDepthTable[] = {
    { NV0073_CTRL_SPECIFIC_OR_DITHER_TYPE_6_BITS,
      NV_KMS_DPY_ATTRIBUTE_CURRENT_DITHERING_DEPTH_6_BITS },
    { NV0073_CTRL_SPECIFIC_OR_DITHER_TYPE_8_BITS,
      NV_KMS_DPY_ATTRIBUTE_CURRENT_DITHERING_DEPTH_8_BITS },
    { NV0073_CTRL_SPECIFIC_OR_DITHER_TYPE_OFF,
      NV_KMS_DPY_ATTRIBUTE_CURRENT_DITHERING_DEPTH_NONE }
};

/*!
 * Choose dithering based on the requested dithering config
 * NVConnectorEvo::or::dither.
 */
void nvChooseDitheringEvo(
    const NVConnectorEvoRec *pConnectorEvo,
    enum NvKmsDpyAttributeColorBpcValue bpc,
    const NVDpyAttributeRequestedDitheringConfig *pReqDithering,
    NVDpyAttributeCurrentDitheringConfig *pCurrDithering)
{
    NvU32 i;
    NVDpyAttributeCurrentDitheringConfig currDithering = {
        .enabled = FALSE,
        .mode = NV_KMS_DPY_ATTRIBUTE_CURRENT_DITHERING_MODE_NONE,
        .depth = NV_KMS_DPY_ATTRIBUTE_CURRENT_DITHERING_DEPTH_NONE,
    };

    currDithering.enabled = (pConnectorEvo->or.ditherType !=
                                NV0073_CTRL_SPECIFIC_OR_DITHER_TYPE_OFF);

    for (i = 0; i < ARRAY_LEN(ditherDepthTable); i++) {
        if (ditherDepthTable[i].type == pConnectorEvo->or.ditherType) {
            currDithering.depth = ditherDepthTable[i].nvKmsDitherDepth;
            break;
        }
    }

    for (i = 0; i < ARRAY_LEN(ditherModeTable); i++) {
        if (ditherModeTable[i].algo == pConnectorEvo->or.ditherAlgo) {
            currDithering.mode = ditherModeTable[i].nvKmsDitherMode;
            break;
        }
    }

    switch (pReqDithering->state) {
    case NV_KMS_DPY_ATTRIBUTE_REQUESTED_DITHERING_ENABLED:
        currDithering.enabled = TRUE;
        break;
    case NV_KMS_DPY_ATTRIBUTE_REQUESTED_DITHERING_DISABLED:
        currDithering.enabled = FALSE;
        break;
    default:
        nvAssert(!"Unknown Dithering configuration");
        // Fall through
    case NV_KMS_DPY_ATTRIBUTE_REQUESTED_DITHERING_AUTO:
        /*
         * Left it initialized
         * based on value NVDpyEvoRec::or::dither::init::enabled.
         */
        break;
    }

    switch (pReqDithering->depth) {
    case NV_KMS_DPY_ATTRIBUTE_REQUESTED_DITHERING_DEPTH_6_BITS:
        currDithering.depth =
            NV_KMS_DPY_ATTRIBUTE_CURRENT_DITHERING_DEPTH_6_BITS;
        break;
    case NV_KMS_DPY_ATTRIBUTE_REQUESTED_DITHERING_DEPTH_8_BITS:
        currDithering.depth =
            NV_KMS_DPY_ATTRIBUTE_CURRENT_DITHERING_DEPTH_8_BITS;
        break;
    default:
        nvAssert(!"Unknown Dithering Depth");
        // Fall through
    case NV_KMS_DPY_ATTRIBUTE_REQUESTED_DITHERING_DEPTH_AUTO:
        /*
         * Left it initialized
         * based on value NVDpyEvoRec::or::dither::init::type.
         */
        break;
    }


    if (nvConnectorUsesDPLib(pConnectorEvo) &&
        (pReqDithering->state !=
            NV_KMS_DPY_ATTRIBUTE_REQUESTED_DITHERING_DISABLED)) {
        NvU32 lutBits = 11;

        /* If we are using DisplayPort panel with bandwidth constraints
         * which lowers the color depth, consider that while applying
         * dithering effects.
         */
        if (bpc == 0) {
            nvAssert(!"Unknown dpBits");
            bpc = 8;
        }

        /*
         * If fewer than 8 DP bits are available, dither.  Ideally we'd
         * dither from lutBits > 10 to 10 bpc, but EVO doesn't have an
         * option for that.
         *
         * XXX TODO: nvdisplay can dither to 10 bpc.
         */
        if ((bpc <= 8) && (lutBits > bpc)) {
            if (pReqDithering->state ==
                    NV_KMS_DPY_ATTRIBUTE_REQUESTED_DITHERING_AUTO) {
                currDithering.enabled = TRUE;
            }
        }

        if (pReqDithering->depth ==
                NV_KMS_DPY_ATTRIBUTE_REQUESTED_DITHERING_DEPTH_AUTO) {
            if (bpc <= 6) {
                currDithering.depth =
                    NV_KMS_DPY_ATTRIBUTE_CURRENT_DITHERING_DEPTH_6_BITS;
            } else if (bpc <= 8) {
                currDithering.depth =
                    NV_KMS_DPY_ATTRIBUTE_CURRENT_DITHERING_DEPTH_8_BITS;
            }
        }
    }

    if (currDithering.enabled) {
        switch (pReqDithering->mode) {
        case NV_KMS_DPY_ATTRIBUTE_REQUESTED_DITHERING_MODE_TEMPORAL:
            currDithering.mode =
                NV_KMS_DPY_ATTRIBUTE_CURRENT_DITHERING_MODE_TEMPORAL;
            break;
        case NV_KMS_DPY_ATTRIBUTE_REQUESTED_DITHERING_MODE_DYNAMIC_2X2:
            currDithering.mode =
                NV_KMS_DPY_ATTRIBUTE_CURRENT_DITHERING_MODE_DYNAMIC_2X2;
            break;
        case NV_KMS_DPY_ATTRIBUTE_REQUESTED_DITHERING_MODE_STATIC_2X2:
            currDithering.mode =
                NV_KMS_DPY_ATTRIBUTE_CURRENT_DITHERING_MODE_STATIC_2X2;
            break;
        default:
            nvAssert(!"Unknown Dithering Mode");
            // Fall through
        case NV_KMS_DPY_ATTRIBUTE_REQUESTED_DITHERING_MODE_AUTO:
            /*
             * Left it initialized
             * based on value NVDpyEvoRec::or::dither::init::algo.
             */
            break;
        }
    } else {
        currDithering.depth = NV_KMS_DPY_ATTRIBUTE_CURRENT_DITHERING_DEPTH_NONE;
        currDithering.mode = NV_KMS_DPY_ATTRIBUTE_CURRENT_DITHERING_MODE_NONE;
    }

    *pCurrDithering = currDithering;
}

void nvSetDitheringEvo(
    NVDispEvoPtr pDispEvo,
    const NvU32 head,
    const NVDpyAttributeCurrentDitheringConfig *pCurrDithering,
    NVEvoUpdateState *pUpdateState)
{
    NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    NvU32 i;
    NvU32 algo = NV0073_CTRL_SPECIFIC_OR_DITHER_ALGO_UNKNOWN;
    NvU32 type = NV0073_CTRL_SPECIFIC_OR_DITHER_TYPE_OFF;
    NvU32 enabled = pCurrDithering->enabled;

    for (i = 0; i < ARRAY_LEN(ditherModeTable); i++) {
        if (ditherModeTable[i].nvKmsDitherMode == pCurrDithering->mode) {
            algo = ditherModeTable[i].algo;
            break;
        }
    }
    nvAssert(i < ARRAY_LEN(ditherModeTable));

    for (i = 0; i < ARRAY_LEN(ditherDepthTable); i++) {
        if (ditherDepthTable[i].nvKmsDitherDepth == pCurrDithering->depth) {
            type = ditherDepthTable[i].type;
            break;
        }
    }
    nvAssert(i < ARRAY_LEN(ditherDepthTable));

    /*
     * Make sure algo is a recognizable value that we will be able to program
     * in hardware.
     */
    if (algo == NV0073_CTRL_SPECIFIC_OR_DITHER_ALGO_UNKNOWN) {
        algo = NV0073_CTRL_SPECIFIC_OR_DITHER_ALGO_DYNAMIC_2X2;
    }

    nvPushEvoSubDevMaskDisp(pDispEvo);
    pDevEvo->hal->SetDither(pDispEvo, head, enabled, type, algo,
                            pUpdateState);
    nvPopEvoSubDevMask(pDevEvo);
}

/*
 * HeadCanStereoLock() - Return whether or not this head can use stereo lock
 * mode.  This can only be called from UpdateEvoLockState, when the pending
 * interlaced/locked values are still in the head control assembly structure.
 */
static NvBool HeadCanStereoLock(NVDevEvoPtr pDevEvo, int sd, int head)
{
    NVEvoHeadControlPtr pHC = &pDevEvo->gpus[sd].headControlAssy[head];

    return (!pHC->interlaced && !pHC->mergeMode &&
            ((pHC->serverLock != NV_EVO_NO_LOCK) ||
             (pHC->clientLock != NV_EVO_NO_LOCK)));
}

/*
 * SetStereoLockMode() - For stereo lock mode, we need to notify
 * the gsync board that this GPU requires stereo lock mode.
 */
static NvBool SetStereoLockMode(NVDispEvoPtr pDispEvo, NvBool stereoLocked)
{
    NV30F1_CTRL_CMD_GSYNC_SET_CONTROL_STEREO_LOCK_MODE_PARAMS
        statusParams = { 0 };
    NVFrameLockEvoPtr pFrameLockEvo = pDispEvo->pFrameLockEvo;

    if (!pFrameLockEvo ||
        ((pFrameLockEvo->boardId != NV30F1_CTRL_GSYNC_GET_CAPS_BOARD_ID_P2060) &&
        (pFrameLockEvo->boardId != NV30F1_CTRL_GSYNC_GET_CAPS_BOARD_ID_P2061))) {
        return TRUE;
    }

    statusParams.gpuId = nvGpuIdOfDispEvo(pDispEvo);
    statusParams.enable = stereoLocked ? 1 : 0;

    if (nvRmApiControl(nvEvoGlobal.clientHandle,
                       pFrameLockEvo->device,
                       NV30F1_CTRL_CMD_GSYNC_SET_CONTROL_STEREO_LOCK_MODE,
                       &statusParams,
                       sizeof(statusParams)) != NVOS_STATUS_SUCCESS) {
        nvAssert(!"Failed to set stereo lock mode");
        return FALSE;
    }

    return TRUE;
}

/*
 * SyncEvoLockState()
 *
 * Set the Assembly state based on the current Armed state.  This should be
 * called before transitioning between states in the EVO state machine.
 */
static void SyncEvoLockState(void)
{
    NVDispEvoPtr pDispEvo;
    unsigned int sd;
    NVDevEvoPtr pDevEvo;

    FOR_ALL_EVO_DEVS(pDevEvo) {

        if (!pDevEvo->gpus) {
            continue;
        }

        if (pDevEvo->displayHandle == 0) {
            continue;
        }

        FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {
            NVEvoSubDevPtr pEvoSubDev = &pDevEvo->gpus[sd];
            NvU32 updateHeadMask = nvGetActiveHeadMask(pDispEvo);
            unsigned int head;

            /* Update the cached HEAD_SET_CONTROL EVO method state */
            FOR_ALL_HEADS(head, updateHeadMask) {
                pEvoSubDev->headControlAssy[head] =
                    pEvoSubDev->headControl[head];

                /*
                 * The following are probably not necessary, since no other
                 * code touches them (as opposed to headControl above which
                 * is updated beyond the scope of the state machine).  But
                 * update them here anyway to be consistent.
                 */
                pEvoSubDev->frameLockClientMaskAssy =
                    pEvoSubDev->frameLockClientMaskArmed;
                pEvoSubDev->frameLockServerMaskAssy =
                    pEvoSubDev->frameLockServerMaskArmed;
                pEvoSubDev->frameLockExtRefClkMaskAssy =
                    pEvoSubDev->frameLockExtRefClkMaskArmed;
            }
        }
    }
}

/*
 * Determine a unique index for the given (pDevEvo, sd) tuple.
 * This is used to index into an array of size NV_MAX_DEVICES.
 *
 * It would be more straightforward to use a two-dimensional array of
 * NV_MAX_DEVICES x NV_MAX_SUBDEVICES and index by (devIndex, sd), but
 * that makes the array too large to fit on the stack.  This is safe because
 * we should only ever have at most NV_MAX_DEVICES GPUs in the system
 * total, although at any given time they may be split into many single-GPU
 * device or a small number of many-GPU SLI devices.
 */
static NvU32 GpuIndex(const NVDevEvoRec *pDevEvo, NvU32 sd)
{
    const NVDevEvoRec *pDevEvoIter;
    NvU32 index = 0;

    nvAssert(sd < pDevEvo->numSubDevices);

    FOR_ALL_EVO_DEVS(pDevEvoIter) {
        if (pDevEvoIter == pDevEvo) {
            index += sd;
            nvAssert(index < NV_MAX_DEVICES);
            return index;
        }
        index += pDevEvo->numSubDevices;
    }

    nvAssert(!"Failed to look up GPU index");
    return 0;
}

NvU32 nvGetRefreshRate10kHz(const NVHwModeTimingsEvo *pTimings)
{
    const NvU32 totalPixels = pTimings->rasterSize.x * pTimings->rasterSize.y;

    /*
     * pTimings->pixelClock is in 1000/s
     * we want 0.0001/s
     * factor = 1000/0.0001 = 10000000.
     */
    NvU32 factor = 10000000;

    if (pTimings->doubleScan) factor /= 2;
    if (pTimings->interlaced) factor *= 2;

    if (totalPixels == 0) {
        return 0;
    }

    return axb_div_c(pTimings->pixelClock, factor, totalPixels);
}

/*!
 * Get the current refresh rate for the heads in headMask, in 0.0001 Hz units.
 * All heads in headMask are expected to have the same refresh rate.
 */
static NvU32 GetRefreshRateHeadMask10kHz(const NVDispEvoRec *pDispEvo,
                                         NvU32 headMask)
{
    const NVHwModeTimingsEvo *pTimings = NULL;
    NvU32 head;

    FOR_ALL_HEADS(head, headMask) {
        const NVDispHeadStateEvoRec *pHeadState =
            &pDispEvo->headState[head];

        if (head >= pDispEvo->pDevEvo->numHeads &&
                pHeadState->activeRmId == 0x0) {
            continue;
        }

        if (pTimings == NULL) {
            pTimings = &pHeadState->timings;
        } else {
            nvAssert(pTimings->rasterSize.x ==
                        pHeadState->timings.rasterSize.x);
            nvAssert(pTimings->rasterSize.y ==
                        pHeadState->timings.rasterSize.y);
            nvAssert(pTimings->doubleScan == pHeadState->timings.doubleScan);
            nvAssert(pTimings->interlaced == pHeadState->timings.interlaced);
            nvAssert(pTimings->pixelClock == pHeadState->timings.pixelClock);
        }
    }

    if (pTimings == NULL) {
        return 0;
    }

    return nvGetRefreshRate10kHz(pTimings);
}

/*!
 * Return a the mask of RmIds from the heads mask.
 */
static NvU32 HeadMaskToActiveRmIdMask(const NVDispEvoRec *pDispEvo,
                                      const NvU32 headMask)
{
    NvU32 head;
    NvU32 rmDisplayMask = 0;

    for (head = 0; head < NVKMS_MAX_HEADS_PER_DISP; head++) {
        if ((NVBIT(head) & headMask) != 0x0) {
            rmDisplayMask |=
                pDispEvo->headState[head].activeRmId;
        }
    }

    return rmDisplayMask;
}

static NvBool FramelockSetControlSync(NVDispEvoPtr pDispEvo, const NvU32 headMask,
                                  NvBool server)
{
    NVFrameLockEvoPtr pFrameLockEvo = pDispEvo->pFrameLockEvo;
    NV30F1_CTRL_GSYNC_SET_CONTROL_SYNC_PARAMS gsyncSetControlSyncParams = { 0 };
    NvU32 ret;

    /* There can only be one server. */

    nvAssert(!server || (nvPopCount32(headMask) == 1));

    gsyncSetControlSyncParams.gpuId = nvGpuIdOfDispEvo(pDispEvo);
    gsyncSetControlSyncParams.master = server;
    gsyncSetControlSyncParams.displays =
        HeadMaskToActiveRmIdMask(pDispEvo, headMask);

    if (gsyncSetControlSyncParams.displays == 0x0) {
        return FALSE;
    }

    gsyncSetControlSyncParams.refresh =
        GetRefreshRateHeadMask10kHz(pDispEvo, headMask);

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pFrameLockEvo->device,
                         NV30F1_CTRL_CMD_GSYNC_SET_CONTROL_SYNC,
                         &gsyncSetControlSyncParams,
                         sizeof(gsyncSetControlSyncParams));

    if (ret != NVOS_STATUS_SUCCESS) {
        return FALSE;
    }

    return TRUE;
}

NvBool nvFramelockSetControlUnsyncEvo(NVDispEvoPtr pDispEvo, const NvU32 headMask,
                                      NvBool server)
{
    NVFrameLockEvoPtr pFrameLockEvo = pDispEvo->pFrameLockEvo;
    NV30F1_CTRL_GSYNC_SET_CONTROL_UNSYNC_PARAMS
        gsyncSetControlUnsyncParams = { 0 };
    NvU32 ret;

    gsyncSetControlUnsyncParams.gpuId = nvGpuIdOfDispEvo(pDispEvo);
    gsyncSetControlUnsyncParams.master = server;
    gsyncSetControlUnsyncParams.displays =
        HeadMaskToActiveRmIdMask(pDispEvo, headMask);

    if (gsyncSetControlUnsyncParams.displays == 0x0) {
        return FALSE;
    }

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pFrameLockEvo->device,
                         NV30F1_CTRL_CMD_GSYNC_SET_CONTROL_UNSYNC,
                         &gsyncSetControlUnsyncParams,
                         sizeof(gsyncSetControlUnsyncParams));

    if (ret != NVOS_STATUS_SUCCESS) {
        return FALSE;
    }

    return TRUE;
}

/*
 * UpdateEvoLockState()
 *
 * Update the hardware based on the Assembly state, if it is different from the
 * current Armed state.  This should be called after transitioning through
 * states in the EVO state machine to propagate all of the necessary values to
 * HW.
 */
static void UpdateEvoLockState(void)
{
    NVDispEvoPtr pDispEvo;
    NVFrameLockEvoPtr pFrameLockEvo;
    unsigned int sd;
    NVDevEvoPtr pDevEvo;
    NvBool ret;
    enum {
        FIRST_ITERATION,
        DISABLE_UNNEEDED_CLIENTS = FIRST_ITERATION,
        DISABLE_UNNEEDED_SERVER,
        COMPUTE_HOUSE_SYNC,
        UPDATE_HOUSE_SYNC,
        ENABLE_SERVER,
        ENABLE_CLIENTS,
        LAST_ITERATION = ENABLE_CLIENTS,
    } iteration;
    struct {
        unsigned char disableServer:1;
        unsigned char disableClient:1;
        unsigned char enableServer:1;
        unsigned char enableClient:1;
    } cache[NV_MAX_DEVICES][NVKMS_MAX_HEADS_PER_DISP];

    nvkms_memset(cache, 0, sizeof(cache));

    /* XXX NVKMS TODO: idle base channel, first? */

    /*
     * Stereo lock mode is enabled if all heads are either raster locked or
     * frame locked, and if all heads are not using interlaced mode.
     */
    FOR_ALL_EVO_DEVS(pDevEvo) {
        if (!pDevEvo->gpus) {
            continue;
        }
        FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {
            NvBool gpuCanStereoLock = TRUE;
            NvBool testedOneHead = FALSE;

            /*
             * If at least one head is not locked or driving an interlaced
             * mode, then no heads on this GPU will use stereo lock mode.
             */
            NvU32 head;
            for (head = 0; head < NVKMS_MAX_HEADS_PER_DISP; head++) {
                NVEvoHeadControlPtr pHC = &pDevEvo->gpus[sd].headControlAssy[head];

                if (!nvHeadIsActive(pDispEvo, head) ||
                    ((pHC->serverLock == NV_EVO_NO_LOCK) &&
                     (pHC->clientLock  == NV_EVO_NO_LOCK))) {
                    /*
                     * If the heads aren't scan locked then we should skip
                     * them as if they aren't connected. NOTE this
                     * conservative approach means that we won't disable
                     * StereoLockMode when frameLock is turned off. This
                     * should be harmless.
                     */
                    continue;
                }
                testedOneHead = TRUE;
                if (!HeadCanStereoLock(pDevEvo, sd, head)) {
                    gpuCanStereoLock = FALSE;
                }
            }
            /*
             * Don't set StereoLockMode for screenless GPUs. As above we'll also
             * count heads that can't stereoLock as unconnected.
             */
            if (!testedOneHead) {
                continue;
            }

            /*
             * Notify the framelock board whether or not we we will use stereo
             * lock mode.  If it failed, then don't enable stereo lock mode on
             * the GPU.
             */
            if (!SetStereoLockMode(pDispEvo, gpuCanStereoLock)) {
                gpuCanStereoLock = FALSE;
            }

            /*
             * Cache whether or not we can use stereo lock mode, so we know
             * whether or not to enable stereo lock mode on the GPU during
             * SetHeadControl
             */
            for (head = 0; head < NVKMS_MAX_HEADS_PER_DISP; head++) {
                if (nvHeadIsActive(pDispEvo, head)) {
                    NVEvoSubDevPtr pEvoSubDev = &pDevEvo->gpus[sd];
                    pEvoSubDev->headControlAssy[head].stereoLocked =
                        gpuCanStereoLock;
                }
            }
        }
    }

    /*
     * Go through every GPU on the system, making its framelock state match the
     * assembly state that we've saved.
     *
     * We do this in six steps, in order to keep the overall system state sane
     * throughout:
     * 1. Disable any clients we no longer need
     * 2. Disable server we no longer need
     * 3. Compute which framelock devices need house sync
     * 4. Update framelock devices with new house sync info
     * 5. Enable new server
     * 6. Enable new clients
     */
    for (iteration = FIRST_ITERATION;
         iteration <= LAST_ITERATION;
         iteration++) {

        if (iteration == COMPUTE_HOUSE_SYNC) {
            /* First, clear assy state */
            FOR_ALL_EVO_FRAMELOCKS(pFrameLockEvo) {
                pFrameLockEvo->houseSyncAssy = FALSE;
            }
        }

        if (iteration == UPDATE_HOUSE_SYNC) {
            FOR_ALL_EVO_FRAMELOCKS(pFrameLockEvo) {
                /*
                 * Since nvFrameLockSetUseHouseSyncEvo sets house sync
                 * output mode in addition to house sync input mode and
                 * input polarity, this needs to be done unconditionally,
                 * even if a house sync state transition hasn't occurred.
                 */
                if (!nvFrameLockSetUseHouseSyncEvo(
                        pFrameLockEvo, pFrameLockEvo->houseSyncAssy)) {
                    nvAssert(!"Setting house sync failed");
                } else {
                    pFrameLockEvo->houseSyncArmed =
                        pFrameLockEvo->houseSyncAssy;
                }
            }

            continue;
        }

        FOR_ALL_EVO_DEVS(pDevEvo) {

            if (!pDevEvo->gpus) {
                continue;
            }

            if (pDevEvo->displayHandle == 0) {
                /*
                 * This may happen during init, when setting initial modes on
                 * one device while other devices have not yet been allocated.
                 * Skip these devices for now; we'll come back later when
                 * they've been brought up.
                 */
                continue;
            }

            FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {
                NVEvoSubDevPtr pEvoSubDev = &pDevEvo->gpus[sd];
                NvBool server = FALSE;
                NvU32 needsEnableMask = 0, needsDisableMask = 0;
                unsigned int head;

                switch (iteration) {
                case COMPUTE_HOUSE_SYNC:
                    /* Accumulate house sync across pDisps */
                    if (pEvoSubDev->frameLockHouseSync) {
                        pDispEvo->pFrameLockEvo->houseSyncAssy = TRUE;
                    }
                    break;
                case DISABLE_UNNEEDED_CLIENTS:
                    needsDisableMask = pEvoSubDev->frameLockClientMaskArmed &
                                       ~pEvoSubDev->frameLockClientMaskAssy;
                    server = FALSE;
                    break;
                case DISABLE_UNNEEDED_SERVER:
                    needsDisableMask = pEvoSubDev->frameLockServerMaskArmed &
                                       ~pEvoSubDev->frameLockServerMaskAssy;
                    server = TRUE;
                    break;
                case ENABLE_SERVER:
                    needsEnableMask = pEvoSubDev->frameLockServerMaskAssy &
                                      ~pEvoSubDev->frameLockServerMaskArmed;
                    server = TRUE;
                    break;
                case ENABLE_CLIENTS:
                    needsEnableMask = pEvoSubDev->frameLockClientMaskAssy &
                                      ~pEvoSubDev->frameLockClientMaskArmed;
                    server = FALSE;
                    break;
                case UPDATE_HOUSE_SYNC:
                    nvAssert(!"Shouldn't reach here");
                    break;
                }

                if (needsDisableMask) {
                    ret = nvFramelockSetControlUnsyncEvo(pDispEvo,
                                                         needsDisableMask,
                                                         server);
                    nvAssert(ret);

                    if (ret) {
                        if (server) {
                            pEvoSubDev->frameLockServerMaskArmed &=
                                ~needsDisableMask;

                            FOR_ALL_HEADS(head, needsDisableMask) {
                                cache[GpuIndex(pDevEvo, sd)][head].disableServer = TRUE;
                            }
                        } else {
                            pEvoSubDev->frameLockClientMaskArmed &=
                                ~needsDisableMask;

                            FOR_ALL_HEADS(head, needsDisableMask) {
                                cache[GpuIndex(pDevEvo, sd)][head].disableClient = TRUE;
                            }
                        }
                    }
                }
                if (needsEnableMask) {
                    ret = FramelockSetControlSync(pDispEvo,
                                                  needsEnableMask,
                                                  server);

                    nvAssert(ret);

                    if (ret) {
                        if (server) {
                            pEvoSubDev->frameLockServerMaskArmed |=
                                needsEnableMask;

                            FOR_ALL_HEADS(head, needsEnableMask) {
                                cache[GpuIndex(pDevEvo, sd)][head].enableServer = TRUE;
                            }
                        } else {
                            pEvoSubDev->frameLockClientMaskArmed |=
                                needsEnableMask;

                            FOR_ALL_HEADS(head, needsEnableMask) {
                                cache[GpuIndex(pDevEvo, sd)][head].enableClient = TRUE;
                            }
                        }
                    }
                }

                /* After the above process, we should have "promoted" assy
                 * to armed */
                if (iteration == LAST_ITERATION) {
                    nvAssert(pEvoSubDev->frameLockServerMaskArmed ==
                             pEvoSubDev->frameLockServerMaskAssy);
                    nvAssert(pEvoSubDev->frameLockClientMaskArmed ==
                             pEvoSubDev->frameLockClientMaskAssy);
                }
            }
        }
    }

    /*
     * Update the EVO HW state.  Make this a separate set of loops to not
     * confuse the one above
     */
    FOR_ALL_EVO_DEVS(pDevEvo) {

        if (!pDevEvo->gpus) {
            continue;
        }

        if (pDevEvo->displayHandle == 0) {
            continue;
        }

        FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {
            NvBool needUpdate = FALSE;
            NVEvoUpdateState updateState = { };
            NVEvoSubDevPtr pEvoSubDev = &pDevEvo->gpus[sd];
            NvU32 extRefClkMaskAssy, extRefClkUpdateMask;
            NvU32 possibleHeadMask;
            NvBool refClkChanged[NVKMS_MAX_HEADS_PER_DISP] = { FALSE };
            unsigned int head;

            extRefClkMaskAssy = pEvoSubDev->frameLockExtRefClkMaskAssy;

            /* Set the external reference clock, if different */
            extRefClkUpdateMask = extRefClkMaskAssy ^
                pEvoSubDev->frameLockExtRefClkMaskArmed;

            FOR_ALL_HEADS(head, extRefClkUpdateMask) {
                NvBool extRefClkNeeded =
                    !!(extRefClkMaskAssy & (1 << head));

                SetRefClk(pDevEvo, sd, head, extRefClkNeeded, &updateState);
                refClkChanged[head] = TRUE;

                /* Update armed state for this head */
                pEvoSubDev->frameLockExtRefClkMaskArmed =
                    (pEvoSubDev->frameLockExtRefClkMaskArmed &
                     (~(1 << head))) |
                    (extRefClkMaskAssy & (1 << head));
            }
            /* After the above process, the armed state should match
             * assembly state */
            nvAssert(extRefClkMaskAssy ==
                     pEvoSubDev->frameLockExtRefClkMaskArmed);

            /* Update the HEAD_SET_CONTROL EVO method state */

            possibleHeadMask = nvGetActiveHeadMask(pDispEvo);

            FOR_ALL_HEADS(head, possibleHeadMask) {
                if (nvkms_memcmp(&pEvoSubDev->headControl[head],
                                 &pEvoSubDev->headControlAssy[head],
                                 sizeof(NVEvoHeadControl))) {

                    nvPushEvoSubDevMask(pDevEvo, 1 << sd);

                    pEvoSubDev->headControl[head] =
                        pEvoSubDev->headControlAssy[head];
                    pDevEvo->hal->SetHeadControl(pDevEvo, sd, head,
                                                 &updateState);
                    needUpdate = TRUE;

                    nvPopEvoSubDevMask(pDevEvo);
                } else if (refClkChanged[head]) {
                    needUpdate = TRUE;
                }
            }

            if (needUpdate) {
                nvEvoUpdateAndKickOff(pDispEvo, TRUE, &updateState,
                                      TRUE /* releaseElv */);
            }
        }
    }

    /*
     * Inform GLS of framelock changes.  It uses this information to do things
     * like enable fake stereo to get stereo sync when stereo apps start
     * without flickering the displays.
     */
    for (iteration = FIRST_ITERATION;
         iteration <= LAST_ITERATION;
         iteration++) {

        FOR_ALL_EVO_DEVS(pDevEvo) {

            if (!pDevEvo->gpus) {
                continue;
            }

            if (pDevEvo->displayHandle == 0) {
                continue;
            }

            FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {
                NvU32 head;
                for (head = 0; head < NVKMS_MAX_HEADS_PER_DISP; head++) {
                    NvBool sendEvent = FALSE;
                    NvBool enable = FALSE, server = FALSE;

                    if (!nvHeadIsActive(pDispEvo, head)) {
                        continue;
                    }

                    switch (iteration) {
                    case DISABLE_UNNEEDED_CLIENTS:
                        if (cache[GpuIndex(pDevEvo, sd)][head].disableClient) {
                            enable = FALSE;
                            server = FALSE;
                            sendEvent = TRUE;
                        }
                        break;
                    case DISABLE_UNNEEDED_SERVER:
                        if (cache[GpuIndex(pDevEvo, sd)][head].disableServer) {
                            enable = FALSE;
                            server = TRUE;
                            sendEvent = TRUE;
                        }
                        break;
                    case ENABLE_SERVER:
                        if (cache[GpuIndex(pDevEvo, sd)][head].enableServer) {
                            enable = TRUE;
                            server = TRUE;
                            sendEvent = TRUE;
                        }
                        break;
                    case ENABLE_CLIENTS:
                        if (cache[GpuIndex(pDevEvo, sd)][head].enableClient) {
                            enable = TRUE;
                            server = FALSE;
                            sendEvent = TRUE;
                        }
                        break;
                    case UPDATE_HOUSE_SYNC:
                    case COMPUTE_HOUSE_SYNC:
                        sendEvent = FALSE;
                        break;
                    }

                    if (sendEvent) {
                        nvUpdateGLSFramelock(pDispEvo, head, enable, server);
                    }
                }
            }
        }
    }
}

/*
 * For every head in the headMask on pDispEvo, construct a prioritized
 * list of heads and call into the EVO locking state machine to
 * perform the given transition.
 *
 * Return the list of heads that actually succeeded.
 */
static NvU32 applyActionForHeads(NVDispEvoPtr pDispEvo,
                                 const NvU32 headMask,
                                 NVEvoLockAction action)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NVEvoSubDevPtr pEvoSubDev = &pDevEvo->gpus[pDispEvo->displayOwner];
    NvU32 appliedHeadMask = 0;
    NvU32 head;

    FOR_ALL_HEADS(head, headMask) {
        NvU32 pHeads[NVKMS_MAX_HEADS_PER_DISP + 1] = { NV_INVALID_HEAD, };
        unsigned int i = 0;
        NvU32 tmpHead, usedHeadMask = 0;

        /* Fill in the array starting with this head, then with the others in
         * the list, and finally any other active heads */
        pHeads[i++] = head;
        usedHeadMask |= (1 << head);

        FOR_ALL_HEADS(tmpHead, headMask) {
            if (usedHeadMask & (1 << tmpHead)) {
                continue;
            }
            pHeads[i++] = tmpHead;
            usedHeadMask |= (1 << tmpHead);
        }

        for (tmpHead = 0; tmpHead < NVKMS_MAX_HEADS_PER_DISP; tmpHead++) {
            if (!nvHeadIsActive(pDispEvo, tmpHead)) {
                continue;
            }
            if (usedHeadMask & (1 << tmpHead)) {
                continue;
            }
            pHeads[i++] = tmpHead;
            usedHeadMask |= (1 << tmpHead);
        }

        nvAssert(i <= NVKMS_MAX_HEADS_PER_DISP);
        pHeads[i] = NV_INVALID_HEAD;

        if (pEvoSubDev->scanLockState(pDispEvo, pEvoSubDev, action, pHeads)) {
            appliedHeadMask |= (1 << head);
        }
    }

    return appliedHeadMask;
}

//
// Set up raster lock and frame lock for external frame lock
//

NvBool nvEnableFrameLockEvo(NVDispEvoPtr pDispEvo)
{
    NVFrameLockEvoPtr pFrameLockEvo = pDispEvo->pFrameLockEvo;
    NvU32 serverHead = nvGetFramelockServerHead(pDispEvo);
    NvU32 clientHeadsMask = nvGetFramelockClientHeadsMask(pDispEvo);
    NvU32 appliedHeadMask;
    NvU32 activeClientHeadsMask;
    NvBool useHouseSync = FALSE;
    NvU32 head;

    nvAssert(pDispEvo->framelock.currentServerHead == NV_INVALID_HEAD);
    nvAssert(pDispEvo->framelock.currentClientHeadsMask == 0x0);

    if (serverHead != NV_INVALID_HEAD  &&
        (pFrameLockEvo->houseSyncMode ==
         NV_KMS_FRAMELOCK_ATTRIBUTE_HOUSE_SYNC_MODE_INPUT)) {

        NvS64 houseSync;

        /*
         * Only use house sync if present.
         * XXX what happens when house sync is unplugged?  why not enable it
         * now and let the FPGA decide?
         */
        if (!nvFrameLockGetStatusEvo(pFrameLockEvo,
                                     NV_KMS_FRAMELOCK_ATTRIBUTE_HOUSE_STATUS,
                                     &houseSync)) {
            return FALSE;
        }

        useHouseSync = (houseSync != 0);
    }

    /* Initialize the assembly state */
    SyncEvoLockState();

    /* Enable the server */
    if ((serverHead != NV_INVALID_HEAD) &&
            nvHeadIsActive(pDispEvo, serverHead)) {
        NvU32 serverHeadMask;

        serverHeadMask = (1 << serverHead);
        appliedHeadMask = applyActionForHeads(pDispEvo, serverHeadMask,
                                              NV_EVO_ADD_FRAME_LOCK_SERVER);

        nvAssert(appliedHeadMask == serverHeadMask);
        pDispEvo->framelock.currentServerHead = serverHead;

        /* Enable house sync, if requested */
        if (useHouseSync) {
            appliedHeadMask =
                applyActionForHeads(pDispEvo, serverHeadMask,
                                    NV_EVO_ADD_FRAME_LOCK_HOUSE_SYNC);

            if (appliedHeadMask == serverHeadMask) {
                pDispEvo->framelock.currentHouseSync = TRUE;
            }
        }
    }

    /* Enable the clients */
    activeClientHeadsMask = 0;
    FOR_ALL_HEADS(head, clientHeadsMask) {
        if (nvHeadIsActive(pDispEvo, head)) {
            activeClientHeadsMask |= (1 << head);
        }
    }
    appliedHeadMask = applyActionForHeads(pDispEvo, activeClientHeadsMask,
                                          NV_EVO_ADD_FRAME_LOCK_CLIENT);

    nvAssert(appliedHeadMask == activeClientHeadsMask);
    pDispEvo->framelock.currentClientHeadsMask = activeClientHeadsMask;

    /* Finally, update the hardware */
    UpdateEvoLockState();

    return TRUE;
}

//
// Disable raster lock and frame lock
//

NvBool nvDisableFrameLockEvo(NVDispEvoPtr pDispEvo)
{
    NvU32 serverHead = nvGetFramelockServerHead(pDispEvo);
    NvU32 clientHeadsMask = nvGetFramelockClientHeadsMask(pDispEvo);
    NvU32 activeClientHeadsMask;
    NvU32 appliedHeadMask;
    NvU32 head;

    /* Initialize the assembly state */
    SyncEvoLockState();

    /* Disable the clients */
    activeClientHeadsMask = 0;
    FOR_ALL_HEADS(head, clientHeadsMask) {
        if (nvHeadIsActive(pDispEvo, head)) {
            activeClientHeadsMask |= (1 << head);
        }
    }
    appliedHeadMask = applyActionForHeads(pDispEvo,
                                          activeClientHeadsMask,
                                          NV_EVO_REM_FRAME_LOCK_CLIENT);

    nvAssert(appliedHeadMask == activeClientHeadsMask);
    pDispEvo->framelock.currentClientHeadsMask &= ~activeClientHeadsMask;

    /* Disable house sync */
    if (serverHead != NV_INVALID_HEAD &&
            nvHeadIsActive(pDispEvo, serverHead)) {
        NvU32 serverHeadMask = (1 << serverHead);

        if (pDispEvo->framelock.currentHouseSync) {
            appliedHeadMask =
                applyActionForHeads(pDispEvo, serverHeadMask,
                                    NV_EVO_REM_FRAME_LOCK_HOUSE_SYNC);

            nvAssert(appliedHeadMask == serverHeadMask);
            pDispEvo->framelock.currentHouseSync = FALSE;
        }

        /* Disable the server */
        appliedHeadMask = applyActionForHeads(pDispEvo, serverHeadMask,
                                              NV_EVO_REM_FRAME_LOCK_SERVER);
        nvAssert(appliedHeadMask == serverHeadMask);
        if (appliedHeadMask == serverHeadMask) {
            pDispEvo->framelock.currentServerHead = NV_INVALID_HEAD;
        }
    }

    /* Finally, update the hardware */
    UpdateEvoLockState();

    return TRUE;
}

//
// Enable/Disable External Reference Clock Sync
//
// This function is used by frame lock to make the GPU sync to
// the external device's reference clock.
//
static void SetRefClk(NVDevEvoPtr pDevEvo,
                      NvU32 sd, NvU32 head, NvBool external,
                      NVEvoUpdateState *updateState)
{
    nvPushEvoSubDevMask(pDevEvo, 1 << sd);

    pDevEvo->hal->SetHeadRefClk(pDevEvo, head, external, updateState);

    nvPopEvoSubDevMask(pDevEvo);
}


//
// Query raster lock state
//

NvBool nvQueryRasterLockEvo(const NVDpyEvoRec *pDpyEvo, NvS64 *val)
{
    NVDispEvoPtr pDispEvo = pDpyEvo->pDispEvo;
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NVEvoSubDevPtr pEvoSubDev;
    const NvU32 apiHead = pDpyEvo->apiHead;
    const NvU32 head = nvGetPrimaryHwHead(pDispEvo, apiHead);
    NVEvoHeadControlPtr pHC;

    /*
     * XXX[2Heads1OR] The EVO lock state machine is not currently supported with
     * 2Heads1OR, the api head is expected to be mapped onto a single
     * hardware head (which is the primary hardware head) if 2Heads1OR is not
     * active and the EVO lock state machine is in use.
     */
    if ((apiHead == NV_INVALID_HEAD) ||
            (nvPopCount32(pDispEvo->apiHeadState[apiHead].hwHeadsMask) != 1)) {
        return FALSE;
    }

    if ((head == NV_INVALID_HEAD) || (pDevEvo->gpus == NULL)) {
        return FALSE;
    }

    pEvoSubDev = &pDevEvo->gpus[pDispEvo->displayOwner];
    pHC = &pEvoSubDev->headControl[head];

    *val = pHC->serverLock == NV_EVO_RASTER_LOCK ||
           pHC->clientLock == NV_EVO_RASTER_LOCK;

    return TRUE;
}

void nvInvalidateRasterLockGroupsEvo(void)
{
    if (globalRasterLockGroups) {
        nvFree(globalRasterLockGroups);

        globalRasterLockGroups = NULL;
        numGlobalRasterLockGroups = 0;
    }
}

/*
 * Return the surface format usage bounds that NVKMS will program for the
 * requested format.
 *
 * For an RGB XBPP format, this function will return a bitmask of all RGB YBPP
 * formats, where Y <= X.
 *
 * For a YUV format, this function will return a bitmask of all YUV formats
 * that:
 * - Have the same number of planes as the requested format
 * - Have the same chroma decimation factors as the requested format
 * - Have the same or lower effective fetch bpp as the requested format
 *
 * For example, if the requested format is YUV420 12-bit SP, this function will
 * include all YUV420 8/10/12-bit SP formats.
 */
NvU64 nvEvoGetFormatsWithEqualOrLowerUsageBound(
    const enum NvKmsSurfaceMemoryFormat format,
    NvU64 supportedFormatsCapMask)
{
    const NvKmsSurfaceMemoryFormatInfo *pFormatInfo =
        nvKmsGetSurfaceMemoryFormatInfo(format);
    NvU64 supportedFormatsUsageBound = 0;
    NvU8 formatIdx;

    FOR_EACH_INDEX_IN_MASK(64, formatIdx, supportedFormatsCapMask) {

        const NvKmsSurfaceMemoryFormatInfo *pOtherFormatInfo =
            nvKmsGetSurfaceMemoryFormatInfo(formatIdx);

        if ((pFormatInfo->isYUV != pOtherFormatInfo->isYUV) ||
            (pFormatInfo->numPlanes != pOtherFormatInfo->numPlanes)) {
            continue;
        }

        if (pFormatInfo->isYUV) {
            if ((pFormatInfo->yuv.horizChromaDecimationFactor !=
                 pOtherFormatInfo->yuv.horizChromaDecimationFactor) ||
                (pFormatInfo->yuv.vertChromaDecimationFactor !=
                 pOtherFormatInfo->yuv.vertChromaDecimationFactor) ||
                (pFormatInfo->yuv.depthPerComponent <
                 pOtherFormatInfo->yuv.depthPerComponent)) {
                continue;
            }
        } else {
            if (pFormatInfo->rgb.bitsPerPixel <
                pOtherFormatInfo->rgb.bitsPerPixel) {
                continue;
            }
        }

        supportedFormatsUsageBound |= NVBIT64(formatIdx);

    } FOR_EACH_INDEX_IN_MASK_END;

    return supportedFormatsUsageBound;
}

//
// Enable or disable flip lock (or query state)
//

NvBool nvUpdateFlipLockEvoOneHead(NVDispEvoPtr pDispEvo, const NvU32 head,
                                  NvU32 *val, NvBool set,
                                  NVEvoUpdateState *updateState)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NVEvoSubDevPtr pEvoSubDev = &pDevEvo->gpus[pDispEvo->displayOwner];
    NVEvoHeadControlPtr pHC = &pEvoSubDev->headControl[head];

    if (set) {
        // make sure we're dealing with a bool
        NvBool setVal = !!*val;

        if (setVal ^ pHC->flipLock) {
            NvBool isMethodPending;
            NvBool changed = FALSE;

            if (!pDevEvo->hal->
                    IsChannelMethodPending(pDevEvo,
                                           pDevEvo->head[head].layer[NVKMS_MAIN_LAYER],
                                           pDispEvo->displayOwner,
                                           &isMethodPending) ||
                isMethodPending) {
                nvAssert(!"Base channel not idle");
                return FALSE;
            }

            if (setVal) {
                /* make sure flip lock is not prohibited and raster lock is enabled
                 *
                 * XXX: [2Heads1OR] If head is locked in the merge mode then
                 * its flip-lock state can not be changed.
                 */
                if ((pHC->serverLock == NV_EVO_NO_LOCK &&
                     pHC->clientLock == NV_EVO_NO_LOCK) ||
                    HEAD_MASK_QUERY(pEvoSubDev->flipLockProhibitedHeadMask,
                                    head) ||
                    pHC->mergeMode) {
                    return FALSE;
                }
                pHC->flipLock = TRUE;
                changed = TRUE;
            } else {
                /* Only actually disable fliplock if it's not needed for SLI.
                 *
                 * XXX: [2Heads1OR] If head is locked in the merge mode then
                 * its flip-lock state can not be changed.
                 */
                if (!pHC->mergeMode &&
                    !HEAD_MASK_QUERY(pEvoSubDev->flipLockEnabledForSliHeadMask,
                                     head)) {
                    pHC->flipLock = FALSE;
                    changed = TRUE;
                }
            }

            if (changed) {
                EvoUpdateHeadParams(pDispEvo, head, updateState);
            }
        }

        /* Remember if we currently need fliplock enabled for framelock */
        pEvoSubDev->flipLockEnabledForFrameLockHeadMask =
            setVal ?
                HEAD_MASK_SET(pEvoSubDev->flipLockEnabledForFrameLockHeadMask, head) :
                HEAD_MASK_UNSET(pEvoSubDev->flipLockEnabledForFrameLockHeadMask, head);
    }

    /*
     * XXX should the query return the cached "enabled for framelock" state
     * instead?
     */
    *val = pHC->flipLock;


    return TRUE;
}


static NvBool UpdateFlipLock50(const NVDpyEvoRec *pDpyEvo,
                               NvU32 *val, NvBool set)
{
    NVDispEvoPtr pDispEvo = pDpyEvo->pDispEvo;
    const NvU32 apiHead = pDpyEvo->apiHead;
    const NvU32 head = nvGetPrimaryHwHead(pDispEvo, apiHead);
    NVEvoUpdateState updateState = { };
    NvBool ret;

    if (head == NV_INVALID_HEAD) {
        return FALSE;
    }

    ret = nvUpdateFlipLockEvoOneHead(pDispEvo, head, val, set,
                                     &updateState);

    if (set && ret) {
        nvEvoUpdateAndKickOff(pDispEvo, TRUE, &updateState,
                              TRUE /* releaseElv */);
    }

    return ret;
}

NvBool nvSetFlipLockEvo(NVDpyEvoPtr pDpyEvo, NvS64 value)
{
    NvU32 val32 = !!value;
    return UpdateFlipLock50(pDpyEvo, &val32, TRUE /* set */);
}

NvBool nvGetFlipLockEvo(const NVDpyEvoRec *pDpyEvo, NvS64 *pValue)
{
    NvBool ret;
    NvU32 val32 = 0;
    ret = UpdateFlipLock50(pDpyEvo, &val32, FALSE /* set */);

    if (ret) {
        *pValue = !!val32;
    }

    return ret;
}

static void ProhibitFlipLock50(NVDispEvoPtr pDispEvo)
{
    NvU32 head;
    NvBool needUpdate = FALSE;
    NVEvoUpdateState updateState = { };

    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NVEvoSubDevPtr pEvoSubDev = &pDevEvo->gpus[pDispEvo->displayOwner];

    for (head = 0; head < NVKMS_MAX_HEADS_PER_DISP; head++) {
        NVEvoHeadControlPtr pHC = &pEvoSubDev->headControl[head];
        /*
         * XXX: [2Heads1OR] If head is locked in the merge mode then its flip-lock
         * state can not be changed.
         */
        if (!nvHeadIsActive(pDispEvo, head) || pHC->mergeMode) {
            continue;
        }

        if (HEAD_MASK_QUERY(pEvoSubDev->flipLockEnabledForFrameLockHeadMask,
                            head)) {
            nvAssert(!"Can not prohibit flip lock "
                      "because it is already enabled for frame lock");
            continue;
        }

        if (pHC->flipLock) {
            needUpdate = TRUE;

            pHC->flipLock = FALSE;
            EvoUpdateHeadParams(pDispEvo, head, &updateState);
        }

        pEvoSubDev->flipLockProhibitedHeadMask =
            HEAD_MASK_SET(pEvoSubDev->flipLockProhibitedHeadMask, head);
    }

    if (needUpdate) {
        nvEvoUpdateAndKickOff(pDispEvo, TRUE, &updateState,
                              TRUE /* releaseElv */);
    }
}

static void AllowFlipLock50(NVDispEvoPtr pDispEvo)
{
    NvU32 head;
    NvBool needUpdate = FALSE;
    NVEvoUpdateState updateState = { };
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NVEvoSubDevPtr pEvoSubDev = &pDevEvo->gpus[pDispEvo->displayOwner];

    for (head = 0; head < NVKMS_MAX_HEADS_PER_DISP; head++) {
        NVEvoHeadControlPtr pHC = &pEvoSubDev->headControl[head];

        /*
         * XXX: [2Heads1OR] If head is locked in the merge mode then its flip-lock
         * state can not be changed.
         */
        if (!nvHeadIsActive(pDispEvo, head) || pHC->mergeMode) {
            continue;
        }

        if (!pHC->flipLock &&
            HEAD_MASK_QUERY(pEvoSubDev->flipLockEnabledForSliHeadMask,
                            head)) {
            needUpdate = TRUE;

            nvAssert(pHC->serverLock != NV_EVO_NO_LOCK ||
                     pHC->clientLock != NV_EVO_NO_LOCK);

            pHC->flipLock = TRUE;
            EvoUpdateHeadParams(pDispEvo, head, &updateState);
        }

        pEvoSubDev->flipLockProhibitedHeadMask =
            HEAD_MASK_UNSET(pEvoSubDev->flipLockProhibitedHeadMask, head);
    }

    if (needUpdate) {
        nvEvoUpdateAndKickOff(pDispEvo, TRUE, &updateState,
                              TRUE /* releaseElv */);
    }
}

NvBool nvAllowFlipLockEvo(NVDispEvoPtr pDispEvo, NvS64 value)
{
    if (value == 0) {
        ProhibitFlipLock50(pDispEvo);
    } else {
        AllowFlipLock50(pDispEvo);
    }
    return TRUE;
}

/*!
 * Enable or disable stereo.
 *
 * XXX SLI+Stereo For now, just set stereo on the display owner.
 */
NvBool nvSetStereoEvo(
    const NVDispEvoRec *pDispEvo,
    const NvU32 head,
    NvBool enable)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NVEvoSubDevPtr pEvoSubDev = &pDevEvo->gpus[pDispEvo->displayOwner];
    NVEvoHeadControlPtr pHC;
    NVEvoLockPin pin;

    nvAssert(head != NV_INVALID_HEAD);

    pHC = &pEvoSubDev->headControl[head];
    pin = NV_EVO_LOCK_PIN_INTERNAL(head);

    // make sure we're dealing with a bool
    NvBool stereo = !NV_EVO_LOCK_PIN_IS_INTERNAL(pHC->stereoPin);

    if (enable ^ stereo) {
        NVEvoUpdateState updateState = { };

        if (enable) {
            NvU32 otherHead;
            NvU32 signalPin;

            // If any other head is already driving stereo, fail
            for (otherHead = 0; otherHead < NVKMS_MAX_HEADS_PER_DISP;
                 otherHead++) {
                if (!nvHeadIsActive(pDispEvo, otherHead)) {
                    continue;
                }
                if (head == otherHead) {
                    continue;
                }

                const NVEvoHeadControl *pOtherHC =
                    &pEvoSubDev->headControl[otherHead];

                if (!NV_EVO_LOCK_PIN_IS_INTERNAL(pOtherHC->stereoPin)) {
                    return FALSE;
                }
            }

            signalPin = nvEvoGetPinForSignal(pDispEvo,
                                             pEvoSubDev,
                                             NV_EVO_LOCK_SIGNAL_STEREO);
            if (signalPin != NV_EVO_LOCK_PIN_ERROR) {
                pin = signalPin;
            }
        }

        pHC->stereoPin = pin;

        EvoUpdateHeadParams(pDispEvo, head, &updateState);

        // Make method take effect.
        nvEvoUpdateAndKickOff(pDispEvo, TRUE, &updateState,
                              TRUE /* releaseElv */);
    }

    return TRUE;
}

/*!
 * Query stereo state.
 *
 * XXX SLI+Stereo For now, just get stereo on the display owner.
 */
NvBool nvGetStereoEvo(const NVDispEvoRec *pDispEvo, const NvU32 head)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NVEvoSubDevPtr pEvoSubDev = &pDevEvo->gpus[pDispEvo->displayOwner];
    NVEvoHeadControlPtr pHC;

    nvAssert(head != NV_INVALID_HEAD);

    pHC = &pEvoSubDev->headControl[head];

    return !NV_EVO_LOCK_PIN_IS_INTERNAL(pHC->stereoPin);
}

void nvSetViewPortsEvo(NVDispEvoPtr pDispEvo,
                       const NvU32 head, NVEvoUpdateState *updateState)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NVDispHeadStateEvoRec *pHeadState = &pDispEvo->headState[head];
    const NVHwModeViewPortEvo *pViewPort = &pHeadState->timings.viewPort;

    nvPushEvoSubDevMaskDisp(pDispEvo);
    pDevEvo->hal->SetViewportInOut(pDevEvo, head,
                                   pViewPort, pViewPort, pViewPort,
                                   updateState);
    nvPopEvoSubDevMask(pDevEvo);

    /*
     * Specify safe default values of 0 for viewPortPointIn x and y; these
     * may be changed when panning out of band of a modeset.
     */
    EvoSetViewportPointIn(pDispEvo, head, 0 /* x */, 0 /* y */, updateState);
}



static void EvoSetViewportPointIn(NVDispEvoPtr pDispEvo, const NvU32 head,
                                  NvU16 x, NvU16 y,
                                  NVEvoUpdateState *updateState)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;

    nvPushEvoSubDevMaskDisp(pDispEvo);
    pDevEvo->hal->SetViewportPointIn(pDevEvo, head, x, y, updateState);
    nvPopEvoSubDevMask(pDevEvo);
}

void nvEvoSetLUTContextDma(NVDispEvoPtr pDispEvo,
                           const NvU32 head, NVEvoUpdateState *pUpdateState)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    const NVDispHeadStateEvoRec *pHeadState = &pDispEvo->headState[head];

    pDevEvo->hal->SetLUTContextDma(pDispEvo,
                                   head,
                                   pHeadState->lut.pCurrSurface,
                                   pHeadState->lut.baseLutEnabled,
                                   pHeadState->lut.outputLutEnabled,
                                   pUpdateState,
                                   pHeadState->bypassComposition);
}

static void EvoUpdateCurrentPalette(NVDispEvoPtr pDispEvo, const NvU32 apiHead)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NVDispApiHeadStateEvoRec *pApiHeadState =
                              &pDispEvo->apiHeadState[apiHead];
    const int dispIndex = pDispEvo->displayOwner;
    NvU32 head;
    NVEvoUpdateState updateState = { };

    FOR_EACH_EVO_HW_HEAD_IN_MASK(pApiHeadState->hwHeadsMask, head) {
        nvEvoSetLUTContextDma(pDispEvo, head, &updateState);
    }

    /*
     * EVO2 does not set LUT context DMA if the core channel
     * doesn't have a scanout surface set, in that case there is no update
     * state to kickoff.
     */
    if (!nvIsUpdateStateEmpty(pDevEvo, &updateState)) {
        int notifier;
        NvBool notify;

        nvEvoStageLUTNotifier(pDispEvo, apiHead);
        notifier = nvEvoCommitLUTNotifiers(pDispEvo);

        nvAssert(notifier >= 0);

        /*
         * XXX: The notifier index returned by nvEvoCommitLUTNotifiers here
         * shouldn't be < 0 because this function shouldn't have been called
         * while a previous LUT update is outstanding. If
         * nvEvoCommitLUTNotifiers ever returns -1 for one reason or another,
         * using notify and setting notifier to 0 in this manner to avoid
         * setting an invalid notifier in the following Update call prevents
         * potential kernel panics and Xids.
         */
        notify = notifier >= 0;
        if (!notify) {
            notifier = 0;
        }

        // Clear the completion notifier and kick off an update.  Wait for it
        // here if NV_CTRL_SYNCHRONOUS_PALETTE_UPDATES is enabled.  Otherwise,
        // don't wait for the notifier -- it'll be checked the next time a LUT
        // change request comes in.
        EvoUpdateAndKickOffWithNotifier(pDispEvo,
                                        notify, /* notify */
                                        FALSE, /* sync */
                                        notifier,
                                        &updateState,
                                        TRUE /* releaseElv */);
        pDevEvo->lut.apiHead[apiHead].disp[dispIndex].waitForPreviousUpdate |= notify;
    }
}

static void UpdateMaxPixelClock(NVDevEvoPtr pDevEvo)
{
    NVDispEvoPtr pDispEvo;
    NVDpyEvoPtr pDpyEvo;
    int i;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, i, pDevEvo) {
        FOR_ALL_EVO_DPYS(pDpyEvo, pDispEvo->validDisplays, pDispEvo) {
            nvDpyProbeMaxPixelClock(pDpyEvo);
        }
    }
}

static NvBool AllocEvoSubDevs(NVDevEvoPtr pDevEvo)
{
    NVDispEvoPtr pDispEvo;
    NvU32 sd;

    pDevEvo->gpus = nvCalloc(pDevEvo->numSubDevices, sizeof(NVEvoSubDevRec));

    if (pDevEvo->gpus == NULL) {
        return FALSE;
    }

    /* Assign the pDispEvo for each evoSubDevice */

    FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {
        pDevEvo->gpus[sd].pDispEvo = pDispEvo;
    }
    for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
        nvAssert(pDevEvo->gpus[sd].pDispEvo != NULL);
    }

    for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
        NVEvoSubDevPtr pEvoSubDev = &pDevEvo->gpus[sd];
        NvU32 head;

        pDevEvo->gpus[sd].subDeviceInstance = sd;
        // Initialize the lock state.
        nvEvoStateStartNoLock(pEvoSubDev);

        for (head = 0; head < pDevEvo->numHeads; head++) {
            NVEvoSubDevHeadStateRec *pSdHeadState =
                &pDevEvo->gpus[sd].headState[head];
            NvU32 i;

            for (i = 0; i < ARRAY_LEN(pSdHeadState->layer); i++) {
                pSdHeadState->layer[i].cscMatrix = NVKMS_IDENTITY_CSC_MATRIX;
            }

            pSdHeadState->cursor.cursorCompParams =
                nvDefaultCursorCompositionParams(pDevEvo);
        }
    }

    return TRUE;
}


// Replace default cursor composition params when zeroed-out values are unsupported.
struct NvKmsCompositionParams nvDefaultCursorCompositionParams(const NVDevEvoRec *pDevEvo)
{
    const struct NvKmsCompositionCapabilities *pCaps =
        &pDevEvo->caps.cursorCompositionCaps;
    const NvU32 supportedBlendMode =
        pCaps->colorKeySelect[NVKMS_COMPOSITION_COLOR_KEY_SELECT_DISABLE].supportedBlendModes[1];

    struct NvKmsCompositionParams params = { };

    if ((supportedBlendMode & NVBIT(NVKMS_COMPOSITION_BLENDING_MODE_OPAQUE)) != 0x0) {
        params.blendingMode[1] = NVKMS_COMPOSITION_BLENDING_MODE_OPAQUE;
    } else {
        params.blendingMode[1] = NVKMS_COMPOSITION_BLENDING_MODE_PREMULT_ALPHA;
    }

    return params;
}

static NvBool ValidateConnectorTypes(const NVDevEvoRec *pDevEvo)
{
    const NVDispEvoRec *pDispEvo;
    const NVConnectorEvoRec *pConnectorEvo;
    NvU32 dispIndex;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        const NVEvoSubDevRec *pEvoSubDev = &pDevEvo->gpus[pDispEvo->displayOwner];
        const NVEvoCapabilities *pEvoCaps = &pEvoSubDev->capabilities;
        const NVEvoMiscCaps *pMiscCaps = &pEvoCaps->misc;

        FOR_ALL_EVO_CONNECTORS(pConnectorEvo, pDispEvo) {
            if (!pMiscCaps->supportsDSI &&
                pConnectorEvo->signalFormat == NVKMS_CONNECTOR_SIGNAL_FORMAT_DSI) {
                nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                            "DSI connectors are unsupported!");
                return FALSE;
            }
        }
    }
    return TRUE;
}

static void UnregisterFlipOccurredEventOneHead(NVDispEvoRec *pDispEvo,
                                               const NvU32 head)
{
    NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    NvU32 layer;

    /* XXX NVKMS TODO: need disp-scope in event */
    if (pDispEvo->displayOwner != 0) {
        return;
    }

    for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
        NVEvoChannelPtr pChannel = pDevEvo->head[head].layer[layer];

        nvAssert((pChannel->completionNotifierEventHandle == 0) ||
                    (pChannel->completionNotifierEventRefPtr != NULL));

        if (pChannel->completionNotifierEventHandle != 0) {
            nvRmApiFree(nvEvoGlobal.clientHandle,
                        pChannel->pb.channel_handle,
                        pChannel->completionNotifierEventHandle);
            nvFreeUnixRmHandle(&pDevEvo->handleAllocator,
                               pChannel->completionNotifierEventHandle);
            pChannel->completionNotifierEventHandle = 0;
            pChannel->completionNotifierEventRefPtr = NULL;
        }
    }
}

static void ClearApiHeadStateOneDisp(NVDispEvoRec *pDispEvo)
{
    NvU32 apiHead;

    /*
     * Unregister all the flip-occurred event callbacks which are
     * registered with the (api-head, layer) pair event data,
     * before destroying the api-head states.
     */
    for (NvU32 head = 0; head < pDispEvo->pDevEvo->numHeads; head++) {
        UnregisterFlipOccurredEventOneHead(pDispEvo, head);
    }

    for (apiHead = 0; apiHead < ARRAY_LEN(pDispEvo->apiHeadState); apiHead++) {
        NvU32 layer;
        NVDispApiHeadStateEvoRec *pApiHeadState =
            &pDispEvo->apiHeadState[apiHead];
        nvAssert(nvListIsEmpty(&pApiHeadState->vblankCallbackList));
        for (layer = 0; layer < ARRAY_LEN(pApiHeadState->flipOccurredEvent); layer++) {
            if (pApiHeadState->flipOccurredEvent[layer].ref_ptr != NULL) {
                nvkms_free_ref_ptr(pApiHeadState->flipOccurredEvent[layer].ref_ptr);
                pApiHeadState->flipOccurredEvent[layer].ref_ptr = NULL;
            }
        }
    }

    nvkms_memset(pDispEvo->apiHeadState, 0, sizeof(pDispEvo->apiHeadState));
}

static void ClearApiHeadState(NVDevEvoRec *pDevEvo)
{
    NvU32 dispIndex;
    NVDispEvoRec *pDispEvo;

    nvRmFreeCoreRGSyncpts(pDevEvo);

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        ClearApiHeadStateOneDisp(pDispEvo);
    }

    nvkms_memset(pDevEvo->apiHead, 0, sizeof(pDevEvo->apiHead));
}

static NvBool InitApiHeadStateOneDisp(NVDispEvoRec *pDispEvo)
{
    NvU32 usedApiHeadsMask = 0x0;
    const NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;

    for (NvU32 apiHead = 0; apiHead < ARRAY_LEN(pDispEvo->apiHeadState); apiHead++) {
        NvU32 layer;
        NVDispApiHeadStateEvoRec *pApiHeadState =
            &pDispEvo->apiHeadState[apiHead];

        pApiHeadState->activeDpys = nvEmptyDpyIdList();
        pApiHeadState->attributes = NV_EVO_DEFAULT_ATTRIBUTES_SET;

        nvListInit(&pApiHeadState->vblankCallbackList);

        for (layer = 0; layer < ARRAY_LEN(pApiHeadState->flipOccurredEvent); layer++) {
            pApiHeadState->flipOccurredEvent[layer].ref_ptr =
                nvkms_alloc_ref_ptr(&pApiHeadState->flipOccurredEvent[layer].data);
            if (pApiHeadState->flipOccurredEvent[layer].ref_ptr == NULL) {
                goto failed;
            }

            pApiHeadState->flipOccurredEvent[layer].data =
                (NVDispFlipOccurredEventDataEvoRec) {
                .pDispEvo = pDispEvo,
                .apiHead = apiHead,
                .layer = layer,
            };
        }
    }

    for (NvU32 head = 0; head < pDevEvo->numHeads; head++) {
        if (pDispEvo->headState[head].pConnectorEvo != NULL) {
            NvU32 apiHead;
            const NVConnectorEvoRec *pConnectorEvo =
                pDispEvo->headState[head].pConnectorEvo;

            /* Find unused api-head which support the equal number of layers */
            for (apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
                if ((NVBIT(apiHead) & usedApiHeadsMask) != 0x0) {
                    continue;
                }

                if (pDevEvo->apiHead[apiHead].numLayers ==
                        pDevEvo->head[head].numLayers) {
                    usedApiHeadsMask |= NVBIT(apiHead);
                    break;
                }
            }
            nvAssert(apiHead < pDevEvo->numApiHeads);

            /*
             * Use the pDpyEvo for the connector, since we may not have one
             * for display id if it's a dynamic one.
             */
            NVDpyEvoRec *pDpyEvo = nvGetDpyEvoFromDispEvo(pDispEvo,
                pConnectorEvo->displayId);

            nvAssert(pDpyEvo->apiHead == NV_INVALID_HEAD);

            pDpyEvo->apiHead = apiHead;
            nvAssignHwHeadsMaskApiHeadState(
                &pDispEvo->apiHeadState[apiHead],
                NVBIT(head));
            pDispEvo->apiHeadState[apiHead].activeDpys =
                nvAddDpyIdToEmptyDpyIdList(pConnectorEvo->displayId);
        }
    }

    return TRUE;

failed:
    ClearApiHeadStateOneDisp(pDispEvo);

    return FALSE;
}

static void
CompletionNotifierEventDeferredWork(void *dataPtr, NvU32 dataU32)
{
    NVDispFlipOccurredEventDataEvoRec *pEventData = dataPtr;

    nvSendFlipOccurredEventEvo(pEventData->pDispEvo, pEventData->apiHead,
                               pEventData->layer);
}

static void CompletionNotifierEvent(void *arg, void *pEventDataVoid,
                                    NvU32 hEvent, NvU32 Data, NV_STATUS Status)
{
  (void) nvkms_alloc_timer_with_ref_ptr(
        CompletionNotifierEventDeferredWork, /* callback */
        arg, /* argument (this is a ref_ptr to NVDispFlipOccurredEventDataEvoRec) */
        0,   /* dataU32 */
        0);  /* timeout: schedule the work immediately */
}

void nvEvoPreModesetRegisterFlipOccurredEvent(NVDispEvoRec *pDispEvo,
                                              const NvU32 head,
                                              const NVEvoModesetUpdateState
                                                  *pModesetUpdate)
{
    NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    NvU32 layer;

    /* XXX NVKMS TODO: need disp-scope in event */
    if (pDispEvo->displayOwner != 0) {
        return;
    }

    for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
        NVEvoChannelPtr pChannel = pDevEvo->head[head].layer[layer];
        const struct _NVEvoModesetUpdateStateOneLayer *pLayer =
             &pModesetUpdate->flipOccurredEvent[head].layer[layer];

        if (!pLayer->changed ||
                (pLayer->ref_ptr == NULL) ||
                (pLayer->ref_ptr == pChannel->completionNotifierEventRefPtr)) {
            continue;
        }

        nvAssert((pChannel->completionNotifierEventHandle == 0) &&
                    (pChannel->completionNotifierEventRefPtr == NULL));

        pChannel->completionNotifierEventHandle =
            nvGenerateUnixRmHandle(&pDevEvo->handleAllocator);

        if (!nvRmRegisterCallback(pDevEvo,
                                  &pChannel->completionNotifierEventCallback,
                                  pLayer->ref_ptr,
                                  pChannel->pb.channel_handle,
                                  pChannel->completionNotifierEventHandle,
                                  CompletionNotifierEvent,
                                  0)) {
            nvFreeUnixRmHandle(&pDevEvo->handleAllocator,
                               pChannel->completionNotifierEventHandle);
            pChannel->completionNotifierEventHandle = 0;
        } else {
            pChannel->completionNotifierEventRefPtr = pLayer->ref_ptr;
        }
    }
}

void nvEvoPostModesetUnregisterFlipOccurredEvent(NVDispEvoRec *pDispEvo,
                                                   const NvU32 head,
                                                   const NVEvoModesetUpdateState
                                                       *pModesetUpdate)
{
    NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    NvU32 layer;

    /* XXX NVKMS TODO: need disp-scope in event */
    if (pDispEvo->displayOwner != 0) {
        return;
    }

    for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
        NVEvoChannelPtr pChannel = pDevEvo->head[head].layer[layer];
        const struct _NVEvoModesetUpdateStateOneLayer *pLayer =
             &pModesetUpdate->flipOccurredEvent[head].layer[layer];

        if (!pLayer->changed ||
                (pLayer->ref_ptr != NULL) ||
                (pChannel->completionNotifierEventHandle == 0)) {

            /*
             * If the flip occurred event of this layer is updated to get
             * enabled (pLayer->ref_ptr != NULL) then that update should have
             * been already processed by
             * nvEvoPreModesetRegisterFlipOccurredEvent() and
             * pChannel->completionNotifierEventRefPtr == pLayer->ref_ptr.
             */
            nvAssert(!pLayer->changed ||
                        (pChannel->completionNotifierEventHandle == 0) ||
                        (pChannel->completionNotifierEventRefPtr ==
                            pLayer->ref_ptr));
            continue;
        }

        nvRmApiFree(nvEvoGlobal.clientHandle,
                    pChannel->pb.channel_handle,
                    pChannel->completionNotifierEventHandle);
        nvFreeUnixRmHandle(&pDevEvo->handleAllocator,
                           pChannel->completionNotifierEventHandle);
        pChannel->completionNotifierEventHandle = 0;
        pChannel->completionNotifierEventRefPtr = NULL;
    }
}

static NvBool InitApiHeadState(NVDevEvoRec *pDevEvo)
{
    NVDispEvoRec *pDispEvo;
    NvU32 dispIndex;

    /*
     * For every hardware head, there should be at least one api-head
     * which supports the equal number of layer.
     */
    nvAssert(pDevEvo->numApiHeads == pDevEvo->numHeads);
    for (NvU32 head = 0; head < pDevEvo->numHeads; head++) {
        pDevEvo->apiHead[head].numLayers = pDevEvo->head[head].numLayers;
    }

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        if (!InitApiHeadStateOneDisp(pDispEvo)) {
            goto failed;
        }
    }

    nvRmAllocCoreRGSyncpts(pDevEvo);

    return TRUE;

failed:
    ClearApiHeadState(pDevEvo);

    return FALSE;
}

/*!
 * Allocate the EVO core channel.
 *
 * This function trivially succeeds if the core channel is already allocated.
 */
NvBool nvAllocCoreChannelEvo(NVDevEvoPtr pDevEvo)
{
    NV5070_CTRL_SYSTEM_GET_CAPS_V2_PARAMS capsParams = { };
    NvU32 ret;
    NvBool bRet;
    NVDispEvoRec *pDispEvo;
    NvU32 dispIndex;
    NvU32 head;

    /* Do nothing if the display was already allocated */
    if (pDevEvo->displayHandle != 0) {
        return TRUE;
    }

    if (!AllocEvoSubDevs(pDevEvo)) {
        goto failed;
    }

    // Disallow GC6 in anticipation of touching GPU/displays.
    if (!nvRmSetGc6Allowed(pDevEvo, FALSE)) {
        goto failed;
    }

    /* Query console FB info, and save the result into pDevEvo->vtFbInfo.
     * This is done at device allocation time.
     * nvRmImportFbConsoleMemory will import the surface for console restore by
     * nvEvoRestoreConsole if the surface format is compatible.
     * Else, console restore will cause core channel realloc, telling RM to
     * restore the console via nvRmVTSwitch.
     */
    if (!nvRmGetVTFBInfo(pDevEvo)) {
        goto failed;
    }

    if (!nvRmVTSwitch(pDevEvo,
                      NV0080_CTRL_OS_UNIX_VT_SWITCH_CMD_SAVE_VT_STATE)) {
        goto failed;
    }

    /* Evo object (parent of all other NV50 display stuff) */
    nvAssert(nvRmEvoClassListCheck(pDevEvo, pDevEvo->dispClass));
    pDevEvo->displayHandle = nvGenerateUnixRmHandle(&pDevEvo->handleAllocator);

    ret = nvRmApiAlloc(nvEvoGlobal.clientHandle,
                       pDevEvo->deviceHandle,
                       pDevEvo->displayHandle,
                       pDevEvo->dispClass,
                       NULL);
    if (ret != NVOS_STATUS_SUCCESS) {
        nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                    "Failed to initialize display engine: 0x%x (%s)",
                    ret, nvstatusToString(ret));
        goto failed;
    }

    /* Get the display caps bits */

    ct_assert(sizeof(pDevEvo->capsBits) == sizeof(capsParams.capsTbl));
    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->displayHandle,
                         NV5070_CTRL_CMD_SYSTEM_GET_CAPS_V2,
                         &capsParams, sizeof(capsParams));
    if (ret != NVOS_STATUS_SUCCESS) {
        nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                    "Failed to determine display capabilities");
        goto failed;
    }
    nvkms_memcpy(pDevEvo->capsBits, capsParams.capsTbl,
                 sizeof(pDevEvo->capsBits));

    // Evo core channel. Allocated once, shared per GPU
    if (!nvRMSetupEvoCoreChannel(pDevEvo)) {
        nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                    "Failed to allocate display engine core DMA push buffer");
        goto failed;
    }

    pDevEvo->coreInitMethodsPending = TRUE;

    bRet = pDevEvo->hal->GetCapabilities(pDevEvo);

    if (!bRet) {
        nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                    "Failed to query display engine capability bits.");
        goto failed;
    }

    /*
     * XXX NVKMS TODO: if the EVO core channel is allocated (and
     * capability notifier queried) before any nvDpyConnectEvo(), then
     * we won't need to update the pixelClock here.
     */
    UpdateMaxPixelClock(pDevEvo);

    if (pDevEvo->numWindows > 0) {
        int win;

        if (!nvRMAllocateWindowChannels(pDevEvo)) {
            nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                        "Failed to allocate display engine window channels");
            goto failed;
        }

        for (win = 0; win < pDevEvo->numWindows; win++) {
            const NvU32 head = pDevEvo->headForWindow[win];

            if (head == NV_INVALID_HEAD) {
                continue;
            }

            pDevEvo->head[head].layer[pDevEvo->head[head].numLayers]  =
                pDevEvo->window[win];
            pDevEvo->head[head].numLayers++;
        }
    } else {
        // Allocate the base channels
        if (!nvRMAllocateBaseChannels(pDevEvo)) {
            nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                        "Failed to allocate display engine base channels");
            goto failed;
        }

        // Allocate the overlay channels
        if (!nvRMAllocateOverlayChannels(pDevEvo)) {
            nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                        "Failed to allocate display engine overlay channels");
            goto failed;
        }

        /* Map base and overlay channels onto main and overlay layers. */
        for (head = 0; head < pDevEvo->numHeads; head++) {
            nvAssert(pDevEvo->base[head] != NULL && pDevEvo->overlay[head] != NULL);

            pDevEvo->head[head].layer[NVKMS_MAIN_LAYER] = pDevEvo->base[head];
            pDevEvo->head[head].layer[NVKMS_OVERLAY_LAYER] = pDevEvo->overlay[head];
            pDevEvo->head[head].numLayers = 2;
        }
    }

    // Allocate and map the cursor controls for all heads
    bRet = nvAllocCursorEvo(pDevEvo);
    if (!bRet) {
        goto failed;
    }

    if (!nvAllocLutSurfacesEvo(pDevEvo)) {
        nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
            "Failed to allocate memory for the display color lookup table.");
        goto failed;
    }

    // Resume the DisplayPort library's control of the device.
    if (!nvRmResumeDP(pDevEvo)) {
        nvEvoLogDev(
            pDevEvo,
            EVO_LOG_ERROR,
            "Failed to initialize DisplayPort sub-system.");
        goto failed;
    }

    if (!InitApiHeadState(pDevEvo)) {
        nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                    "Failed to initialize the api heads.");
        goto failed;
    }

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        nvRmRegisterBacklight(pDispEvo);
    }

    // Allow GC6 if no heads are active.
    if (nvAllHeadsInactive(pDevEvo)) {
        if (!nvRmSetGc6Allowed(pDevEvo, TRUE)) {
            nvEvoLogDev(pDevEvo, EVO_LOG_WARN,
                        "No head is active, but failed to allow GC6");
        }
    }

    return TRUE;

failed:
    nvFreeCoreChannelEvo(pDevEvo);

    return FALSE;
}

/*!
 * Clear the pConnectorEvo->or.primary and pConnectorEvo->or.secondaryMask
 * tracking.
 */
static void ClearSORAssignmentsOneDisp(const NVDispEvoRec *pDispEvo)
{
    NVConnectorEvoPtr pConnectorEvo;

    nvAssert(NV0073_CTRL_SYSTEM_GET_CAP(pDispEvo->pDevEvo->commonCapsBits,
                NV0073_CTRL_SYSTEM_CAPS_CROSS_BAR_SUPPORTED));

    FOR_ALL_EVO_CONNECTORS(pConnectorEvo, pDispEvo) {
        if (pConnectorEvo->or.type != NV0073_CTRL_SPECIFIC_OR_TYPE_SOR) {
            continue;
        }

        pConnectorEvo->or.primary = NV_INVALID_OR;
        pConnectorEvo->or.secondaryMask = 0x0;
    }
}

/*!
 * Update pConnectorEvo->or.primary and pConnectorEvo->or.secondaryMask from
 * the list given to us by RM.
 */
static void RefreshSORAssignments(const NVDispEvoRec *pDispEvo,
                                  const NV0073_CTRL_DFP_ASSIGN_SOR_PARAMS *pParams)
{
    NVConnectorEvoPtr pConnectorEvo;

    ClearSORAssignmentsOneDisp(pDispEvo);

    FOR_ALL_EVO_CONNECTORS(pConnectorEvo, pDispEvo) {
        const NvU32 displayId = nvDpyIdToNvU32(pConnectorEvo->displayId);
        NvU32 sorIndex;

        if (pConnectorEvo->or.type != NV0073_CTRL_SPECIFIC_OR_TYPE_SOR) {
            continue;
        }

        for (sorIndex = 0;
             sorIndex < ARRAY_LEN(pParams->sorAssignList) &&
             sorIndex < ARRAY_LEN(pConnectorEvo->or.ownerHeadMask);
             sorIndex++) {
            if ((pParams->sorAssignListWithTag[sorIndex].displayMask &
                    displayId) == displayId) {
                if ((pParams->sorAssignListWithTag[sorIndex].sorType ==
                        NV0073_CTRL_DFP_SOR_TYPE_SINGLE) ||
                        (pParams->sorAssignListWithTag[sorIndex].sorType ==
                         NV0073_CTRL_DFP_SOR_TYPE_2H1OR_PRIMARY)) {
                    pConnectorEvo->or.primary = sorIndex;
                } else {
                    nvAssert(pParams->sorAssignListWithTag[sorIndex].sorType ==
                                NV0073_CTRL_DFP_SOR_TYPE_2H1OR_SECONDARY);
                    pConnectorEvo->or.secondaryMask |= NVBIT(sorIndex);
                }
            }
        }

        nvAssert((pConnectorEvo->or.secondaryMask == 0) ||
                    (pConnectorEvo->or.primary != NV_INVALID_OR));
    }
}

/*
 * Ask RM to assign an SOR to given displayId.
 *
 * In 2Heads1OR MST case, this function gets called with the dynamic displayId.
 *
 * Note that this assignment may be temporary.  This function will always call
 * RM, and unless the connector is currently in use (i.e., being driven by a
 * head), a previously-assigned SOR may be reused.
 *
 * The RM will either:
 * a) return an SOR that's already assigned/attached
 *    to root port of this displayId, or
 * b) pick a new "unused" SOR, assign and attach it to this connector, and
 *    return that -- where "unused" means both not being actively driven by a
 *    head and not in the "exclude mask" argument.
 *    The "exclude mask" is useful if we need to assign multiple SORs up front
 *    before activating heads to drive them.
 *
 * For example, if head 0 is currently actively scanning out to SOR 0 and we
 * are doing a modeset to activate currently-inactive heads 1 and 2:
 * 1. nvkms calls RM for nvAssignSOREvo(pConnectorForHead1, 0);
 *    RM returns any SOR other than 0 (say 3)
 * 2. nvkms calls RM for nvAssignSOREvo(pConnectorForHead2, (1 << 3));
 *    RM returns any SOR other than 0 and 3 (say 1)
 * 3. At this point nvkms can push methods and UPDATE to enable heads 1 and 2
 *    to drive SORs 3 and 1.
 * In the example above, the sorExcludeMask == (1 << 3) at step 2 is important
 * to ensure that RM doesn't reuse the SOR 3 from step 1.  It won't reuse SOR 0
 * because it's in use by head 0.
 *
 * If an SOR is only needed temporarily (e.g., to do link training to "assess"
 * a DisplayPort or HDMI FRL link), then sorExcludeMask should be 0 -- any SOR
 * that's not actively used by a head can be used, and as soon as nvkms
 * finishes the "assessment", the SOR is again eligible for reuse.
 *
 * Because of the potential for SOR reuse, nvAssignSOREvo() will always call
 * RefreshSORAssignments() to update pConnectorEvo->or.primary and
 * pConnectorEvo->or.secondaryMask on *every* connector after calling
 * NV0073_CTRL_CMD_DFP_ASSIGN_SOR for *any* connector.
 */
NvBool nvAssignSOREvo(const NVConnectorEvoRec *pConnectorEvo,
                      const NvU32 targetDisplayId,
                      const NvBool b2Heads1Or,
                      const NvU32 sorExcludeMask)
{
    const NVDispEvoRec *pDispEvo = pConnectorEvo->pDispEvo;
    const NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    NV0073_CTRL_DFP_ASSIGN_SOR_PARAMS params = { 0 };
    NvU32 ret;

    if (!NV0073_CTRL_SYSTEM_GET_CAP(pDevEvo->commonCapsBits,
                NV0073_CTRL_SYSTEM_CAPS_CROSS_BAR_SUPPORTED)) {
        return TRUE;
    }

    params.subDeviceInstance = pDispEvo->displayOwner;
    params.displayId = targetDisplayId;
    params.bIs2Head1Or = b2Heads1Or;
    params.sorExcludeMask = sorExcludeMask;

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->displayCommonHandle,
                         NV0073_CTRL_CMD_DFP_ASSIGN_SOR,
                         &params,
                         sizeof(params));

    if (ret != NVOS_STATUS_SUCCESS) {
        return FALSE;
    }

    RefreshSORAssignments(pDispEvo, &params);

    return TRUE;
}

static void CacheSorAssignList(const NVDispEvoRec *pDispEvo,
    const NVConnectorEvoRec *sorAssignList[NV0073_CTRL_CMD_DFP_ASSIGN_SOR_MAX_SORS])
{
    const NVConnectorEvoRec *pConnectorEvo;

    FOR_ALL_EVO_CONNECTORS(pConnectorEvo, pDispEvo) {
        if ((pConnectorEvo->or.type != NV0073_CTRL_SPECIFIC_OR_TYPE_SOR) ||
                (pConnectorEvo->or.primary == NV_INVALID_OR)) {
            continue;
        }

        /*
         * RM populates same sor index into more than one connectors if
         * they are are DCC partners, this checks make sure SOR
         * assignment happens only for a single connector. The sor
         * assignment call before modeset/dp-link-training makes sure
         * assignment happens for the correct connector.
         */
        if (sorAssignList[pConnectorEvo->or.primary] != NULL) {
            continue;
        }
        sorAssignList[pConnectorEvo->or.primary] =
            pConnectorEvo;
    }
}

static void RestoreSorAssignList(NVDispEvoRec *pDispEvo,
    const NVConnectorEvoRec *sorAssignList[NV0073_CTRL_CMD_DFP_ASSIGN_SOR_MAX_SORS])
{
    NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    NvU32 sorIndex;

    for (sorIndex = 0;
         sorIndex < NV0073_CTRL_CMD_DFP_ASSIGN_SOR_MAX_SORS; sorIndex++) {

        if (sorAssignList[sorIndex] == NULL) {
            continue;
        }

        NV0073_CTRL_DFP_ASSIGN_SOR_PARAMS params = {
            .subDeviceInstance = pDispEvo->displayOwner,
            .displayId = nvDpyIdToNvU32(sorAssignList[sorIndex]->displayId),
            .sorExcludeMask = ~NVBIT(sorIndex),
        };
        NvU32 ret;

        ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                             pDevEvo->displayCommonHandle,
                             NV0073_CTRL_CMD_DFP_ASSIGN_SOR,
                             &params,
                             sizeof(params));

        if (ret != NVOS_STATUS_SUCCESS) {
            nvEvoLogDispDebug(pDispEvo,
                              EVO_LOG_ERROR,
                              "Failed to restore SOR-%u -> %s assignment.",
                              sorIndex, sorAssignList[sorIndex]->name);
        } else {
            RefreshSORAssignments(pDispEvo, &params);
        }
    }
}

NvBool nvResumeDevEvo(NVDevEvoRec *pDevEvo)
{
    struct {
        const NVConnectorEvoRec *
            sorAssignList[NV0073_CTRL_CMD_DFP_ASSIGN_SOR_MAX_SORS];
    } disp[NVKMS_MAX_SUBDEVICES] = { };
    NVDispEvoRec *pDispEvo;
    NvU32 dispIndex;

    if (NV0073_CTRL_SYSTEM_GET_CAP(pDevEvo->commonCapsBits,
                NV0073_CTRL_SYSTEM_CAPS_CROSS_BAR_SUPPORTED)) {
        FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
            CacheSorAssignList(pDispEvo, disp[dispIndex].sorAssignList);
        }
    }

    if (!nvAllocCoreChannelEvo(pDevEvo)) {
        return FALSE;
    }

    /*
     * During the hibernate-resume cycle vbios or GOP driver programs
     * the display engine to lit up the boot display. In
     * hibernate-resume path, doing NV0073_CTRL_CMD_DFP_ASSIGN_SOR
     * rm-control call before the core channel allocation causes display
     * channel hang because at that stage RM is not aware of the boot
     * display actived by vbios and it ends up unrouting active SOR
     * assignments. Therefore restore the SOR assignment only after the
     * core channel allocation.
     */

    if (NV0073_CTRL_SYSTEM_GET_CAP(pDevEvo->commonCapsBits,
                NV0073_CTRL_SYSTEM_CAPS_CROSS_BAR_SUPPORTED)) {

        /*
         * Shutdown all heads before restoring the SOR assignments because in
         * case of hibernate-resume the SOR, for which NVKMS is trying to
         * restore the assignment, might be in use by the boot display setup
         * by vbios/gop driver.
         */
        nvShutDownApiHeads(pDevEvo, pDevEvo->pNvKmsOpenDev,
                           NULL /* pTestFunc, shut down all heads */,
                           NULL /* pData */,
                           TRUE /* doRasterLock */);

        FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
            RestoreSorAssignList(pDispEvo, disp[dispIndex].sorAssignList);
        }
    }

    return TRUE;
}

void nvSuspendDevEvo(NVDevEvoRec *pDevEvo)
{
    nvFreeCoreChannelEvo(pDevEvo);
}

/*!
 * Free the EVO core channel.
 *
 * This function does nothing if the core channel was already free.
 */
void nvFreeCoreChannelEvo(NVDevEvoPtr pDevEvo)
{
    NVDispEvoPtr pDispEvo;
    NvU32 dispIndex;
    NvU32 head;

    ClearApiHeadState(pDevEvo);

    nvEvoCancelPostFlipIMPTimer(pDevEvo);
    nvCancelVrrFrameReleaseTimers(pDevEvo);

    nvCancelLowerDispBandwidthTimer(pDevEvo);

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        nvRmUnregisterBacklight(pDispEvo);

        nvAssert(pDevEvo->skipConsoleRestore ||
                 nvDpyIdListIsEmpty(nvActiveDpysOnDispEvo(pDispEvo)));
    }

    // Pause the DisplayPort library's control of the device.
    nvRmPauseDP(pDevEvo);

    nvFreeLutSurfacesEvo(pDevEvo);

    // Unmap and free the cursor controls for all heads
    nvFreeCursorEvo(pDevEvo);

    // TODO: Unregister all surfaces registered with this device.

    for (head = 0; head < pDevEvo->numHeads; head++) {
        NvU32 layer;

        for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
            nvRmEvoFreePreSyncpt(pDevEvo, pDevEvo->head[head].layer[layer]);
            pDevEvo->head[head].layer[layer] = NULL;
        }
        pDevEvo->head[head].numLayers = 0;
    }

    nvRMFreeWindowChannels(pDevEvo);
    nvRMFreeOverlayChannels(pDevEvo);
    nvRMFreeBaseChannels(pDevEvo);

    nvRMFreeEvoCoreChannel(pDevEvo);

    if (pDevEvo->displayHandle != 0) {
        if (nvRmApiFree(nvEvoGlobal.clientHandle,
                        pDevEvo->deviceHandle,
                        pDevEvo->displayHandle) != NVOS_STATUS_SUCCESS) {
            nvEvoLogDev(pDevEvo, EVO_LOG_ERROR, "Failed to tear down Disp");
        }
        nvFreeUnixRmHandle(&pDevEvo->handleAllocator, pDevEvo->displayHandle);
        pDevEvo->displayHandle = 0;

        if (!pDevEvo->skipConsoleRestore) {
            nvRmVTSwitch(pDevEvo,
                         NV0080_CTRL_OS_UNIX_VT_SWITCH_CMD_RESTORE_VT_STATE);
        } else {
            nvRmVTSwitch(pDevEvo,
                         NV0080_CTRL_OS_UNIX_VT_SWITCH_CMD_CONSOLE_RESTORED);
        }
    }

    // No longer possible that NVKMS is driving any displays, allow GC6.
    nvRmSetGc6Allowed(pDevEvo, TRUE);

    nvFree(pDevEvo->gpus);
    pDevEvo->gpus = NULL;
}


#define ASSIGN_PIN(_pPin, _pin)                         \
    do {                                                \
        ct_assert(NV_IS_UNSIGNED((_pin)));              \
        if ((_pPin)) {                                  \
            if ((_pin) >= NV_EVO_NUM_LOCK_PIN_CAPS) {   \
                return FALSE;                           \
            }                                           \
            *(_pPin) = (_pin);                          \
        }                                               \
    } while (0)

static NvBool QueryFrameLockHeaderPins(const NVDispEvoRec *pDispEvo,
                                       NVEvoSubDevPtr pEvoSubDev,
                                       NvU32 *pFrameLockPin,
                                       NvU32 *pRasterLockPin,
                                       NvU32 *pFlipLockPin)
{
    NV5070_CTRL_GET_FRAMELOCK_HEADER_LOCKPINS_PARAMS params = { };
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;

    params.base.subdeviceIndex = pEvoSubDev->subDeviceInstance;

    if (nvRmApiControl(nvEvoGlobal.clientHandle,
                       pDevEvo->displayHandle,
                       NV5070_CTRL_CMD_GET_FRAMELOCK_HEADER_LOCKPINS,
                       &params, sizeof(params)) != NVOS_STATUS_SUCCESS) {
        nvEvoLogDispDebug(pDispEvo, EVO_LOG_ERROR,
                          "Failed to query framelock header pins");
        return FALSE;
    }

    ASSIGN_PIN(pFrameLockPin, params.frameLockPin);
    ASSIGN_PIN(pRasterLockPin, params.rasterLockPin);
    ASSIGN_PIN(pFlipLockPin, params.flipLockPin);

    return TRUE;
}

// Gets the lock pin dedicated for a given signal and returns the corresponding method
NVEvoLockPin nvEvoGetPinForSignal(const NVDispEvoRec *pDispEvo,
                                  NVEvoSubDevPtr pEvoSubDev,
                                  NVEvoLockSignal signal)
{
    NVEvoLockPinCaps *caps = pEvoSubDev->capabilities.pin;
    NvU32 pin;

    switch (signal) {

        case NV_EVO_LOCK_SIGNAL_RASTER_LOCK:
            if (!QueryFrameLockHeaderPins(pDispEvo, pEvoSubDev,
                                          NULL, &pin, NULL)) {
                break;
            }

            if (!caps[pin].scanLock) break;

            return NV_EVO_LOCK_PIN_0 + pin;

        case NV_EVO_LOCK_SIGNAL_FRAME_LOCK:
            if (!QueryFrameLockHeaderPins(pDispEvo, pEvoSubDev,
                                          &pin, NULL, NULL)) {
                break;
            }

            if (!caps[pin].scanLock) break;

            return NV_EVO_LOCK_PIN_0 + pin;

        case NV_EVO_LOCK_SIGNAL_FLIP_LOCK:
            if (!QueryFrameLockHeaderPins(pDispEvo, pEvoSubDev,
                                          NULL, NULL, &pin) ||
                !caps[pin].flipLock) {
                // If the query from RM fails (or returns a bogus pin), fall
                // back to an alternate mechanism.  This may happen on boards
                // with no framelock header.  Look in the capabilities for the
                // pin that has the requested capability.
                for (pin = 0; pin < NV_EVO_NUM_LOCK_PIN_CAPS; pin++) {
                    if (caps[pin].flipLock)
                        break;
                }

                if (pin == NV_EVO_NUM_LOCK_PIN_CAPS) {
                    // Not found
                    break;
                }
            }

            if (!caps[pin].flipLock) {
                break;
            }

            return NV_EVO_LOCK_PIN_0 + pin;

        case NV_EVO_LOCK_SIGNAL_STEREO:
            // Look in the capabilities for the pin that has the requested capability
            for (pin = 0; pin < NV_EVO_NUM_LOCK_PIN_CAPS; pin++) {
                if (caps[pin].stereo)
                    break;
            }

            if (pin == NV_EVO_NUM_LOCK_PIN_CAPS) break;

            return NV_EVO_LOCK_PIN_0 + pin;

        default:
            nvAssert(!"Unknown signal type");
            break;
    }

    // Pin not found
    return NV_EVO_LOCK_PIN_ERROR;
}

void nvSetDVCEvo(NVDispEvoPtr pDispEvo,
                 const NvU32 head,
                 NvS32 dvc,
                 NVEvoUpdateState *updateState)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NVDispHeadStateEvoRec *pHeadState = &pDispEvo->headState[head];

    nvAssert(dvc >= NV_EVO_DVC_MIN);
    nvAssert(dvc <= NV_EVO_DVC_MAX);

    // HW range is from -2048 to + 2047
    // Negative values, are not used they distort the colors
    // Values from 1023 to 0 are greying the colors out.
    // We use 0 to 2047 with 1024 as default.
    dvc += 1024;
    nvAssert(dvc >= 0);
    pHeadState->procAmp.satCos = dvc;

    // In SW YUV420 mode, HW is programmed with default DVC. The DVC is handled
    // in a headSurface composite shader.
    if (pHeadState->timings.yuv420Mode == NV_YUV420_MODE_SW) {
        pHeadState->procAmp.satCos = 1024;
    }

    nvPushEvoSubDevMaskDisp(pDispEvo);
    pDevEvo->hal->SetProcAmp(pDispEvo, head, updateState);
    nvPopEvoSubDevMask(pDevEvo);
}

void nvSetImageSharpeningEvo(NVDispEvoRec *pDispEvo, const NvU32 head,
                             NvU32 value, NVEvoUpdateState *updateState)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;

    /*
     * Evo values are from -128 to 127, with a default of 0.
     * Negative values sharpen.
     * Control panel values from 0 (less sharp) to 255
     */
    value = 127 - value;

    nvPushEvoSubDevMaskDisp(pDispEvo);
    pDevEvo->hal->SetOutputScaler(pDispEvo, head, value, updateState);
    nvPopEvoSubDevMask(pDevEvo);
}

static void LayerSetPositionOneApiHead(NVDispEvoRec *pDispEvo,
                                       const NvU32 apiHead,
                                       const NvU32 layer,
                                       const NvS16 x,
                                       const NvS16 y,
                                       NVEvoUpdateState *pUpdateState)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    const NVDispApiHeadStateEvoRec *pApiHeadState =
        &pDispEvo->apiHeadState[apiHead];
    const NvU32 sd = pDispEvo->displayOwner;
    NvU32 head;

    nvPushEvoSubDevMaskDisp(pDispEvo);
    FOR_EACH_EVO_HW_HEAD_IN_MASK(pApiHeadState->hwHeadsMask, head) {
        NVEvoSubDevHeadStateRec *pSdHeadState =
            &pDevEvo->gpus[sd].headState[head];

        if ((pSdHeadState->layer[layer].outputPosition.x != x) ||
            (pSdHeadState->layer[layer].outputPosition.y != y)) {
            NVEvoChannelPtr pChannel =
                pDevEvo->head[head].layer[layer];

            pSdHeadState->layer[layer].outputPosition.x = x;
            pSdHeadState->layer[layer].outputPosition.y = y;

            pDevEvo->hal->SetImmPointOut(pDevEvo, pChannel, sd, pUpdateState,
                                         x, y);
        }
    }
    nvPopEvoSubDevMask(pDevEvo);
}

NvBool nvLayerSetPositionEvo(
    NVDevEvoPtr pDevEvo,
    const struct NvKmsSetLayerPositionRequest *pRequest)
{
    NVDispEvoPtr pDispEvo;
    NvU32 sd;

    /*
     * We need this call to not modify any state if it will fail, so we
     * first verify that all relevant layers support output positioning,
     * then go back through the layers to actually modify the relevant
     * state.
     */
    FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {
        NvU32 apiHead;

        if ((pRequest->requestedDispsBitMask & NVBIT(sd)) == 0) {
            continue;
        }

        for (apiHead = 0; apiHead < NVKMS_MAX_HEADS_PER_DISP; apiHead++) {
            NvU32 layer;

            if ((pRequest->disp[sd].requestedHeadsBitMask &
                 NVBIT(apiHead)) == 0) {
                continue;
            }

            if (!nvApiHeadIsActive(pDispEvo, apiHead)) {
                continue;
            }

            for (layer = 0; layer < pDevEvo->apiHead[apiHead].numLayers; layer++) {
                const NvS16 x = pRequest->disp[sd].head[apiHead].layerPosition[layer].x;
                const NvS16 y = pRequest->disp[sd].head[apiHead].layerPosition[layer].y;

                if ((pRequest->disp[sd].head[apiHead].requestedLayerBitMask &
                        NVBIT(layer)) == 0x0) {
                    continue;
                }

                /*
                 * Error out if a requested layer does not support position
                 * updates and the requested position is not (0, 0).
                 */
                if (!pDevEvo->caps.layerCaps[layer].supportsWindowMode &&
                    (x != 0 || y != 0)) {
                    nvEvoLogDebug(EVO_LOG_ERROR, "Layer %d does not support "
                                                 "position updates.", layer);
                    return FALSE;
                }
            }
        }
    }

    /* Checks in above block passed, so make the requested changes. */
    FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {
        NvU32 apiHead;

        if ((pRequest->requestedDispsBitMask & NVBIT(sd)) == 0) {
            continue;
        }

        for (apiHead = 0; apiHead < NVKMS_MAX_HEADS_PER_DISP; apiHead++) {
            NVEvoUpdateState updateState = { };
            NvU32 layer;

            if ((pRequest->disp[sd].requestedHeadsBitMask &
                 NVBIT(apiHead)) == 0) {
                continue;
            }

            if (!nvApiHeadIsActive(pDispEvo, apiHead)) {
                continue;
            }

            for (layer = 0; layer < pDevEvo->apiHead[apiHead].numLayers; layer++) {
                const NvS16 x = pRequest->disp[sd].head[apiHead].layerPosition[layer].x;
                const NvS16 y = pRequest->disp[sd].head[apiHead].layerPosition[layer].y;

                if ((pRequest->disp[sd].head[apiHead].requestedLayerBitMask &
                        NVBIT(layer)) == 0x0) {
                    continue;
                }

                LayerSetPositionOneApiHead(pDispEvo, apiHead, layer, x, y,
                                           &updateState);
            }

            pDevEvo->hal->Update(pDevEvo, &updateState, TRUE /* releaseElv */);
        }
    }

    return TRUE;
}

/*
 * nvConstructHwModeTimingsImpCheckEvo() - perform an IMP check on the
 * given raster timings and viewport during the
 * nvConstructHwModeTimingsEvo path.  If IMP fails, we try multiple
 * times, each time scaling back the usage bounds until we find a
 * configuration IMP will accept, or until we can't scale back any
 * further.  If this fails, mark the viewport as invalid.
 */

NvBool nvConstructHwModeTimingsImpCheckEvo(
    const NVConnectorEvoRec                *pConnectorEvo,
    const NVHwModeTimingsEvo               *pTimings,
    const NvBool                            enableDsc,
    const NvBool                            b2Heads1Or,
    const NVDpyAttributeColor              *pColor,
    const struct NvKmsModeValidationParams *pParams,
    NVHwModeTimingsEvo                      timings[NVKMS_MAX_HEADS_PER_DISP],
    NvU32                                  *pNumHeads,
    NVEvoInfoStringPtr                      pInfoString)
{
    NvU32 head;
    NvU32 activeRmId;
    const NvU32 numHeads = b2Heads1Or ? 2 : 1;
    NVValidateImpOneDispHeadParamsRec timingsParams[NVKMS_MAX_HEADS_PER_DISP];
    NvBool requireBootClocks = !!(pParams->overrides &
                                  NVKMS_MODE_VALIDATION_REQUIRE_BOOT_CLOCKS);
    NvU32 ret;

    activeRmId = nvRmAllocDisplayId(pConnectorEvo->pDispEvo,
                    nvAddDpyIdToEmptyDpyIdList(pConnectorEvo->displayId));
    if (activeRmId == 0x0) {
        return FALSE;
    }

    nvkms_memset(&timingsParams, 0, sizeof(timingsParams));

    for (head = 0; head < numHeads; head++) {
        timingsParams[head].pConnectorEvo = pConnectorEvo;
        timingsParams[head].activeRmId = activeRmId;
        timingsParams[head].pixelDepth = nvEvoDpyColorToPixelDepth(pColor);
        if (!nvEvoGetSingleTileHwModeTimings(pTimings, numHeads,
                                             &timings[head])) {
            ret = FALSE;
            goto done;
        }
        timingsParams[head].pTimings = &timings[head];
        timingsParams[head].enableDsc = enableDsc;
        timingsParams[head].b2Heads1Or = b2Heads1Or;
        timingsParams[head].pUsage = &timings[head].viewPort.guaranteedUsage;
    }

    /* bypass this checking if the user disabled IMP */
    if ((pParams->overrides &
         NVKMS_MODE_VALIDATION_NO_EXTENDED_GPU_CAPABILITIES_CHECK) != 0) {
        ret = TRUE;
    } else {
        ret = nvValidateImpOneDispDowngrade(pConnectorEvo->pDispEvo, timingsParams,
                                            requireBootClocks,
                                            NV_EVO_REALLOCATE_BANDWIDTH_MODE_NONE,
                                            /* downgradePossibleHeadsBitMask */
                                            (NVBIT(NVKMS_MAX_HEADS_PER_DISP) - 1UL));
    }

    if (ret) {
        *pNumHeads = numHeads;
    } else {
        nvEvoLogInfoString(pInfoString,
                           "ViewPort %dx%d exceeds hardware capabilities.",
                           pTimings->viewPort.out.width,
                           pTimings->viewPort.out.height);
    }

done:
    nvRmFreeDisplayId(pConnectorEvo->pDispEvo, activeRmId);

    return ret;
}

/*
 * Convert from NvModeTimings values to NVHwModeTimingsEvo.
 */

static void
ConstructHwModeTimingsFromNvModeTimings(const NvModeTimings *pModeTimings,
                                        NVHwModeTimingsEvoPtr pTimings)
{
    NvU32 hBlankStart;
    NvU32 vBlankStart;
    NvU32 hBlankEnd;
    NvU32 vBlankEnd;
    NvU32 hSyncWidth;
    NvU32 vSyncWidth;
    NvU32 vTotalAdjustment = 0;

    NvModeTimings modeTimings;

    modeTimings = *pModeTimings;

    if (modeTimings.doubleScan) {
        modeTimings.vVisible *= 2;
        modeTimings.vSyncStart *= 2;
        modeTimings.vSyncEnd *= 2;
        modeTimings.vTotal *= 2;
    }

    /*
     * The real pixel clock and width values for modes using YUV 420 emulation
     * are half of the incoming values parsed from the EDID. This conversion is
     * performed here, so NvModeTimings will have the user-visible (full width)
     * values, and NVHwModeTimingsEvo will have the real (half width) values.
     *
     * HW YUV 420 requires setting the full width mode timings, which are then
     * converted in HW.  RM will recognize YUV420 mode is in use and halve
     * these values for IMP.
     *
     * In either case, only modes with even width are allowed in YUV 420 mode.
     */
    if (modeTimings.yuv420Mode != NV_YUV420_MODE_NONE) {
        nvAssert(((modeTimings.pixelClockHz & 1) == 0) &&
                 ((modeTimings.hVisible & 1) == 0) &&
                 ((modeTimings.hSyncStart & 1) == 0) &&
                 ((modeTimings.hSyncEnd & 1) == 0) &&
                 ((modeTimings.hTotal & 1) == 0) &&
                 ((modeTimings.vVisible & 1) == 0));
        if (modeTimings.yuv420Mode == NV_YUV420_MODE_SW) {
            modeTimings.pixelClockHz /= 2;
            modeTimings.hVisible /= 2;
            modeTimings.hSyncStart /= 2;
            modeTimings.hSyncEnd /= 2;
            modeTimings.hTotal /= 2;
        }
    }

    pTimings->hSyncPol = modeTimings.hSyncNeg;
    pTimings->vSyncPol = modeTimings.vSyncNeg;
    pTimings->interlaced = modeTimings.interlaced;
    pTimings->doubleScan = modeTimings.doubleScan;

    /* pTimings->pixelClock are in KHz but modeTimings.pixelClock are in Hz */

    pTimings->pixelClock = HzToKHz(modeTimings.pixelClockHz);

    /*
     * assign total width, height; note that when the rastertimings
     * are interlaced, we need to make sure SetRasterSize.Height is
     * odd, per EVO's mfs file
     */

    if (pTimings->interlaced) vTotalAdjustment = 1;

    pTimings->rasterSize.x = modeTimings.hTotal;
    pTimings->rasterSize.y = modeTimings.vTotal | vTotalAdjustment;

    /*
     * A bit of EVO quirkiness: The hw increases the blank/sync values
     * by one. So we need to offset by subtracting one.
     *
     * In other words, the h/w inserts one extra sync line/pixel thus
     * incrementing the raster params by one. The number of blank
     * lines/pixels we get is true to what we ask for.  Note the hw
     * does not increase the TotalImageSize by one so we don't need to
     * adjust SetRasterSize.
     *
     * This is slightly unintuitive. Per Evo's specs, the blankEnd
     * comes before blankStart as defined below:  BlankStart: The last
     * pixel/line at the end of the h/v active area.  BlankEnd: The
     * last pixel/line at the end of the h/v blanking.
     *
     * Also: note that in the below computations, we divide by two for
     * interlaced modes *before* subtracting; see bug 263622.
     */

    hBlankStart = modeTimings.hVisible +
        (modeTimings.hTotal - modeTimings.hSyncStart);

    vBlankStart = modeTimings.vVisible +
        (modeTimings.vTotal - modeTimings.vSyncStart);

    hBlankEnd = (modeTimings.hTotal - modeTimings.hSyncStart);
    vBlankEnd = (modeTimings.vTotal - modeTimings.vSyncStart);

    hSyncWidth = (modeTimings.hSyncEnd - modeTimings.hSyncStart);
    vSyncWidth = (modeTimings.vSyncEnd - modeTimings.vSyncStart);

    if (pTimings->interlaced) {
        vBlankStart /= 2;
        vBlankEnd /= 2;
        vSyncWidth /= 2;
    }

    pTimings->rasterSyncEnd.x           = hSyncWidth - 1;
    pTimings->rasterSyncEnd.y           = vSyncWidth - 1;
    pTimings->rasterBlankStart.x        = hBlankStart - 1;
    pTimings->rasterBlankStart.y        = vBlankStart - 1;
    pTimings->rasterBlankEnd.x          = hBlankEnd - 1;
    pTimings->rasterBlankEnd.y          = vBlankEnd - 1;

    /* assign rasterVertBlank2 */

    if (pTimings->interlaced) {
        const NvU32 firstFieldHeight = modeTimings.vTotal / 2;

        pTimings->rasterVertBlank2Start = firstFieldHeight + vBlankStart - 1;
        pTimings->rasterVertBlank2End = firstFieldHeight + vBlankEnd - 1;
    } else {
        pTimings->rasterVertBlank2Start = 0;
        pTimings->rasterVertBlank2End = 0;
    }

    pTimings->hdmi3D = modeTimings.hdmi3D;
    pTimings->yuv420Mode = modeTimings.yuv420Mode;
}



/*
 * Adjust the HwModeTimings as necessary to meet dual link dvi
 * requirements; returns TRUE if the timings were successfully
 * modified; returns FALSE if the timings cannot be made valid for
 * dual link dvi.
 */
static NvBool ApplyDualLinkRequirements(const NVDpyEvoRec *pDpyEvo,
                                        const struct
                                        NvKmsModeValidationParams *pParams,
                                        NVHwModeTimingsEvoPtr pTimings,
                                        NVEvoInfoStringPtr pInfoString)
{
    int adjust;

    nvAssert(pDpyEvo->pConnectorEvo->legacyType ==
             NV0073_CTRL_SPECIFIC_DISPLAY_TYPE_DFP);

    if (pTimings->protocol != NVKMS_PROTOCOL_SOR_DUAL_TMDS) {
        return TRUE;
    }

    if ((pParams->overrides &
         NVKMS_MODE_VALIDATION_NO_DUAL_LINK_DVI_CHECK) != 0) {
        return TRUE;
    }

    /* extract the fields we will need below */

    /*
     * hTotal must be even for dual link dvi; we won't try to patch
     * the htotal size; just give up if it isn't even
     */

    if ((pTimings->rasterSize.x % 2) != 0) {
        nvEvoLogInfoString(pInfoString,
            "Horizontal Total (%d) must be even for dual link DVI mode timings.",
            pTimings->rasterSize.x);
        return FALSE;
    }

    /*
     * RASTER_BLANK_END_X must be odd, so that the active region
     * starts on the following (even) pixel; if it is odd, we are
     * already done
     */

    if ((pTimings->rasterBlankEnd.x % 2) == 1) return TRUE;

    /*
     * RASTER_BLANK_END_X is even, so we need to adjust both
     * RASTER_BLANK_END_X and RASTER_BLANK_START_X by one; we'll first
     * try to subtract one pixel from both
     */

    adjust = -1;

    /*
     * if RASTER_BLANK_END_X cannot be made smaller (would collide
     * with hSyncEnd), see if it would be safe to instead add one to
     * RASTER_BLANK_END_X and RASTER_BLANK_START_X
     */

    if (pTimings->rasterBlankEnd.x <= pTimings->rasterSyncEnd.x + 1) {
        if (pTimings->rasterBlankStart.x + 1 >= pTimings->rasterSize.x) {
            nvEvoLogInfoString(pInfoString,
                "Cannot adjust mode timings for dual link DVI requirements.");
            return FALSE;
        }
        adjust = 1;
    }

    pTimings->rasterBlankEnd.x += adjust;
    pTimings->rasterBlankStart.x += adjust;

    nvEvoLogInfoString(pInfoString,
        "Adjusted mode timings for dual link DVI requirements.");

    return TRUE;
}

void nvInitScalingUsageBounds(const NVDevEvoRec *pDevEvo,
                              struct NvKmsScalingUsageBounds *pScaling)
{
    pScaling->maxVDownscaleFactor = NV_EVO_SCALE_FACTOR_1X;
    pScaling->maxHDownscaleFactor = NV_EVO_SCALE_FACTOR_1X;
    pScaling->vTaps = pDevEvo->hal->caps.minScalerTaps;
    pScaling->vUpscalingAllowed = FALSE;
}

/*
 * Check if the provided number of vertical taps is possible based on the
 * capabilities: the lineStore (the smaller of inWidth and outWidth) must
 * not exceed the maximum pixels for the desired taps; see bug 241014
 */
static NvBool IsVTapsPossible(const NVEvoScalerCaps *pScalerCaps,
                              NvU32 inWidth, NvU32 outWidth,
                              NVEvoScalerTaps nTaps)
{
    const NvU32 lineStore = NV_MIN(inWidth, outWidth);
    NvU32 maxPixels = pScalerCaps->taps[nTaps].maxPixelsVTaps;

    return lineStore <= maxPixels;
}

/*!
 * Compute the scale factor and check against the maximum.
 *
 * param[in]    max     Max scale factor to check against (* 1024)
 * param[in]    in      Input width or height
 * param[in]    out     Output width or height
 * param[out]   pFactor Output scale factor (* 1024)
 */
static NvBool ComputeScalingFactor(NvU32 max,
                                   NvU16 in, NvU16 out,
                                   NvU16 *pFactor)
{
    /* Use a 32-bit temporary to prevent overflow */
    NvU32 tmp;

    /* Add (out - 1) to round up */
    tmp = ((in * 1024) + (out - 1)) / out;

    /* Check against scaling limits. */
    if (tmp > max) {
        return FALSE;
    }

    *pFactor = tmp;
    return TRUE;
}

/*!
 * Compute scaling factors based on in/out dimensions.
 * Used by IMP and when programming viewport and window parameters in HW.
 *
 * The 'maxScaleFactor' values are defined by nvdClass_01.mfs as:
 *      SizeIn/SizeOut * 1024
 */
NvBool nvComputeScalingUsageBounds(const NVEvoScalerCaps *pScalerCaps,
                                   const NvU32 inWidth, const NvU32 inHeight,
                                   const NvU32 outWidth, const NvU32 outHeight,
                                   NVEvoScalerTaps hTaps, NVEvoScalerTaps vTaps,
                                   struct NvKmsScalingUsageBounds *out)
{
    const NVEvoScalerTapsCaps *pTapsCaps = NULL;

    out->vTaps = vTaps;

    /* Start with default values (1.0) */
    out->maxVDownscaleFactor = NV_EVO_SCALE_FACTOR_1X;
    out->maxHDownscaleFactor = NV_EVO_SCALE_FACTOR_1X;

    if (outHeight > inHeight) {
        out->vUpscalingAllowed = TRUE;
    } else if (outHeight < inHeight) {
        out->vUpscalingAllowed = FALSE;

        pTapsCaps = &pScalerCaps->taps[vTaps];
        if (!ComputeScalingFactor(pTapsCaps->maxVDownscaleFactor,
                                  inHeight, outHeight,
                                  &out->maxVDownscaleFactor)) {
            return FALSE;
        }
    }

    if (outWidth < inWidth) {
        pTapsCaps = &pScalerCaps->taps[hTaps];
        if (!ComputeScalingFactor(pTapsCaps->maxHDownscaleFactor,
                                  inWidth, outWidth,
                                  &out->maxHDownscaleFactor)) {
            return FALSE;
        }
    }

    return TRUE;
}

NvBool nvAssignScalerTaps(const NVDevEvoRec *pDevEvo,
                          const NVEvoScalerCaps *pScalerCaps,
                          const NvU32 inWidth, const NvU32 inHeight,
                          const NvU32 outWidth, const NvU32 outHeight,
                          NvBool doubleScan,
                          NVEvoScalerTaps *hTapsOut, NVEvoScalerTaps *vTapsOut)
{
    NVEvoScalerTaps hTaps, vTaps;
    NvBool setHTaps = (outWidth != inWidth);
    NvBool setVTaps = (outHeight != inHeight);

    /*
     * Select the taps filtering; we select the highest taps allowed with our
     * scaling configuration.
     *
     * Note if requiresScalingTapsInBothDimensions is true and if we are
     * scaling in *either* dimension, then we need to program > 1 taps
     * in *both* dimensions.
     */
    if ((setHTaps || setVTaps) &&
        pDevEvo->hal->caps.requiresScalingTapsInBothDimensions) {
        setHTaps = TRUE;
        setVTaps = TRUE;
    }

    /*
     * Horizontal taps: if not scaling, then no filtering; otherwise, set the
     * maximum filtering, because htaps shouldn't have any constraints (unlike
     * vtaps... see below).
     */
    if (setHTaps) {
        /*
         * XXX dispClass_01.mfs says: "For text and desktop scaling, the 2 tap
         * bilinear frequently looks better than the 8 tap filter which is more
         * optimized for video type scaling." Once we determine how best to
         * expose configuration of taps, we should choose how to indicate that 8
         * or 5 taps is the maximum.
         *
         * For now, we'll start with 2 taps as the default, but may end up
         * picking a higher taps value if the required H downscaling factor
         * isn't possible with 2 taps.
         */
        NvBool hTapsFound = FALSE;

        for (hTaps = NV_EVO_SCALER_2TAPS;
             hTaps <= NV_EVO_SCALER_TAPS_MAX;
             hTaps++) {
            NvU16 hFactor;

            if (!ComputeScalingFactor(
                    pScalerCaps->taps[hTaps].maxHDownscaleFactor,
                    inWidth, outWidth,
                    &hFactor)) {
                continue;
            }

            hTapsFound = TRUE;
            break;
        }

        if (!hTapsFound) {
            return FALSE;
        }
    } else {
        hTaps = pDevEvo->hal->caps.minScalerTaps;
    }

    /*
     * Vertical taps: if scaling, set the maximum valid filtering, otherwise, no
     * filtering.
     */
    if (setVTaps) {
        /*
         * Select the maximum vertical taps based on the capabilities.
         *
         * For doublescan modes, limit to 2 taps to reduce blurriness. We really
         * want plain old line doubling, but EVO doesn't support that.
         */
        if (IsVTapsPossible(pScalerCaps, inWidth, outWidth, NV_EVO_SCALER_5TAPS) &&
            !doubleScan) {
            vTaps = NV_EVO_SCALER_5TAPS;
        } else if (IsVTapsPossible(pScalerCaps, inWidth, outWidth, NV_EVO_SCALER_3TAPS) &&
                   !doubleScan) {
            vTaps = NV_EVO_SCALER_3TAPS;
        } else if (IsVTapsPossible(pScalerCaps, inWidth, outWidth, NV_EVO_SCALER_2TAPS)) {
            vTaps = NV_EVO_SCALER_2TAPS;
        } else {
            return FALSE;
        }
    } else {
        vTaps = pDevEvo->hal->caps.minScalerTaps;
    }

    *hTapsOut = hTaps;
    *vTapsOut = vTaps;

    return TRUE;
}

/*
 * Check that ViewPortIn does not exceed hardware limits and compute vTaps and
 * hTaps based on configured ViewPortIn/Out scaling if possible given scaler
 * capabilities.
 */
NvBool nvValidateHwModeTimingsViewPort(const NVDevEvoRec *pDevEvo,
                                       const NVEvoScalerCaps *pScalerCaps,
                                       NVHwModeTimingsEvoPtr pTimings,
                                       NVEvoInfoStringPtr pInfoString)
{
    NVHwModeViewPortEvoPtr pViewPort = &pTimings->viewPort;
    const NvU32 inWidth   = pViewPort->in.width;
    const NvU32 inHeight  = pViewPort->in.height;
    const NvU32 outWidth  = pViewPort->out.width;
    const NvU32 outHeight = pViewPort->out.height;
    const NvBool scaling = (outWidth != inWidth) || (outHeight != inHeight);
    NVEvoScalerTaps hTaps, vTaps;

    /*
     * As per the MFS, there is a restriction for the width and height
     * of ViewPortIn and ViewPortOut
     */
    if (inWidth > 8192 || inHeight > 8192 ||
        outWidth > 8192 || outHeight > 8192) {
        nvEvoLogInfoString(pInfoString,
                           "Viewport dimensions exceed hardware capabilities");
        return FALSE;
    }

    if (!nvAssignScalerTaps(pDevEvo, pScalerCaps, inWidth, inHeight, outWidth, outHeight,
                            pTimings->doubleScan, &hTaps, &vTaps)) {
        nvEvoLogInfoString(pInfoString,
                           "Unable to configure scaling from %dx%d to %dx%d (exceeds filtering capabilities)",
                           inWidth, inHeight,
                           outWidth, outHeight);
        return FALSE;
    }

    /*
     * If this is an interlaced mode but we don't have scaling
     * configured, check that the width will fit in the 2-tap vertical
     * LineStoreSize; this is an EVO requirement for interlaced
     * rasters
     */
    if (pTimings->interlaced && !scaling) {
        /* !scaling means widths should be same */
        nvAssert(outWidth == inWidth);

        if (outWidth > pScalerCaps->taps[NV_EVO_SCALER_2TAPS].maxPixelsVTaps) {
            nvEvoLogInfoString(pInfoString,
                               "Interlaced mode requires filtering, but line width (%d) exceeds filtering capabilities",
                               outWidth);
            return FALSE;
        }

        /* hTaps and vTaps should have been set to minScalerTaps above */
        nvAssert(hTaps == pDevEvo->hal->caps.minScalerTaps);
        nvAssert(vTaps == pDevEvo->hal->caps.minScalerTaps);
    }

    pViewPort->hTaps = hTaps;
    pViewPort->vTaps = vTaps;
    return TRUE;
}

static void AssignGuaranteedSOCBounds(const NVDevEvoRec *pDevEvo,
                                      struct NvKmsUsageBounds *pGuaranteed)
{
    NvU32 layer;

    pGuaranteed->layer[NVKMS_MAIN_LAYER].usable = TRUE;
    pGuaranteed->layer[NVKMS_MAIN_LAYER].supportedSurfaceMemoryFormats =
        nvEvoGetFormatsWithEqualOrLowerUsageBound(
            NvKmsSurfaceMemoryFormatA8R8G8B8,
            pDevEvo->caps.layerCaps[NVKMS_MAIN_LAYER].supportedSurfaceMemoryFormats);
    nvInitScalingUsageBounds(pDevEvo, &pGuaranteed->layer[NVKMS_MAIN_LAYER].scaling);

    for (layer = 1; layer < ARRAY_LEN(pGuaranteed->layer); layer++) {
        pGuaranteed->layer[layer].usable = FALSE;
        nvInitScalingUsageBounds(pDevEvo, &pGuaranteed->layer[layer].scaling);
    }
}

/*
 * Initialize the given NvKmsUsageBounds. Ask for everything supported by the HW
 * by default.  Later, based on what IMP says, we will scale back as needed.
 */
void nvAssignDefaultUsageBounds(const NVDispEvoRec *pDispEvo,
                                NVHwModeViewPortEvo *pViewPort)
{
    const NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    struct NvKmsUsageBounds *pPossible = &pViewPort->possibleUsage;
    NvU32 i;

    for (i = 0; i < ARRAY_LEN(pPossible->layer); i++) {
        struct NvKmsScalingUsageBounds *pScaling = &pPossible->layer[i].scaling;

        pPossible->layer[i].supportedSurfaceMemoryFormats =
            pDevEvo->caps.layerCaps[i].supportedSurfaceMemoryFormats;
        pPossible->layer[i].usable =
            (pPossible->layer[i].supportedSurfaceMemoryFormats != 0);
        if (!pPossible->layer[i].usable) {
            continue;
        }

        nvInitScalingUsageBounds(pDevEvo, pScaling);

        if (pDevEvo->hal->GetWindowScalingCaps) {
            const NVEvoScalerCaps *pScalerCaps =
                pDevEvo->hal->GetWindowScalingCaps(pDevEvo);
            int j;

            for (j = NV_EVO_SCALER_TAPS_MAX; j >= NV_EVO_SCALER_TAPS_MIN; j--) {
                const NVEvoScalerTapsCaps *pTapsCaps = &pScalerCaps->taps[j];

                if ((pTapsCaps->maxVDownscaleFactor == 0) &&
                    (pTapsCaps->maxHDownscaleFactor == 0)) {
                    continue;
                }

                pScaling->maxVDownscaleFactor = pTapsCaps->maxVDownscaleFactor;
                pScaling->maxHDownscaleFactor = pTapsCaps->maxHDownscaleFactor;
                pScaling->vTaps = j;
                pScaling->vUpscalingAllowed = (pTapsCaps->maxPixelsVTaps > 0);
                break;
            }
        }
    }

    if (pDevEvo->isSOCDisplay) {
        AssignGuaranteedSOCBounds(pDevEvo, &pViewPort->guaranteedUsage);
    } else {
        pViewPort->guaranteedUsage = *pPossible;
    }
}

/*
 * ConstructHwModeTimingsViewPort() - determine the ViewPortOut size
 *
 * ViewPortIn (specified by inWidth, inHeight) selects the pixels to
 * extract from the scanout surface; ViewPortOut positions those
 * pixels within the raster timings.
 *
 * If the configuration is not possible, pViewPort->valid will be set
 * to false; otherwise, pViewPort->valid will be set to true.
 */

static NvBool
ConstructHwModeTimingsViewPort(const NVDispEvoRec *pDispEvo,
                               NVHwModeTimingsEvoPtr pTimings,
                               NVEvoInfoStringPtr pInfoString,
                               const struct NvKmsSize *pViewPortSizeIn,
                               const struct NvKmsRect *pViewPortOut)
{
    NVHwModeViewPortEvoPtr pViewPort = &pTimings->viewPort;
    NvU32 outWidth, outHeight;
    const NvU32 hVisible = nvEvoVisibleWidth(pTimings);
    const NvU32 vVisible = nvEvoVisibleHeight(pTimings);

    /* the ViewPortOut should default to the raster size */

    outWidth = hVisible;
    outHeight = vVisible;

    pViewPort->out.xAdjust = 0;
    pViewPort->out.yAdjust = 0;
    pViewPort->out.width = outWidth;
    pViewPort->out.height = outHeight;

    /*
     * If custom viewPortOut or viewPortIn were specified, do basic
     * validation and then assign them to pViewPort.  We'll do more
     * extensive checking of these values as part of IMP.  Note that
     * pViewPort->out.[xy]Adjust are relative to viewPortOut centered
     * within the raster timings, but pViewPortOut->[xy]1 are relative
     * to 0,0.
     */
    if (pViewPortOut) {
        NvS16 offset;
        struct NvKmsRect viewPortOut = *pViewPortOut;

        /*
         * When converting from user viewport out to hardware raster timings,
         * double in the vertical dimension
         */
        if (pTimings->doubleScan) {
            viewPortOut.y *= 2;
            viewPortOut.height *= 2;
        }

        /*
         * The client-specified viewPortOut is in "full" horizontal space for
         * SW YUV420 modes. Convert to "half" horizontal space (matching
         * NVHwModeTimingsEvo and viewPortIn).
         */
        if (pTimings->yuv420Mode == NV_YUV420_MODE_SW) {
            viewPortOut.x /= 2;
            viewPortOut.width /= 2;
        }

        if (A_plus_B_greater_than_C_U16(viewPortOut.x,
                                        viewPortOut.width,
                                        hVisible)) {
            return FALSE;
        }

        if (A_plus_B_greater_than_C_U16(viewPortOut.y,
                                        viewPortOut.height,
                                        vVisible)) {
            return FALSE;
        }

        offset = (hVisible - viewPortOut.width) / 2 * -1;
        pViewPort->out.xAdjust = offset + viewPortOut.x;

        offset = (vVisible - viewPortOut.height) / 2 * -1;
        pViewPort->out.yAdjust = offset + viewPortOut.y;

        pViewPort->out.width = viewPortOut.width;
        pViewPort->out.height = viewPortOut.height;
    }

    if (pViewPortSizeIn) {
        if (pViewPortSizeIn->width <= 0) {
            return FALSE;
        }
        if (pViewPortSizeIn->height <= 0) {
            return FALSE;
        }

        pViewPort->in.width = pViewPortSizeIn->width;
        pViewPort->in.height = pViewPortSizeIn->height;
    } else {
        pViewPort->in.width = pViewPort->out.width;
        pViewPort->in.height = pViewPort->out.height;

        /* When deriving viewportIn from viewportOut, halve the height for
         * doubleScan */
        if (pTimings->doubleScan) {
            pViewPort->in.height /= 2;
        }
    }

    nvAssignDefaultUsageBounds(pDispEvo, &pTimings->viewPort);

    return TRUE;
}


static NvBool GetDefaultFrlDpyColor(
    const NvKmsDpyOutputColorFormatInfo *pColorFormatsInfo,
    NVDpyAttributeColor *pDpyColor)
{
    nvkms_memset(pDpyColor, 0, sizeof(*pDpyColor));
    pDpyColor->colorimetry = NVKMS_OUTPUT_COLORIMETRY_DEFAULT;

    if (pColorFormatsInfo->rgb444.maxBpc >=
            NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_8) {
        pDpyColor->format = NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_RGB;
        pDpyColor->bpc = pColorFormatsInfo->rgb444.maxBpc;
        pDpyColor->range = NV_KMS_DPY_ATTRIBUTE_COLOR_RANGE_FULL;
        return TRUE;
    }

    if (pColorFormatsInfo->yuv444.maxBpc >=
            NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_8) {
        pDpyColor->format = NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr444;
        pDpyColor->bpc = pColorFormatsInfo->yuv444.maxBpc;
        pDpyColor->range = NV_KMS_DPY_ATTRIBUTE_COLOR_RANGE_LIMITED;
        return TRUE;
    }

    return FALSE;
}

static NvBool GetDfpHdmiProtocol(const NVDpyEvoRec *pDpyEvo,
                                 const NvU32 overrides,
                                 NVDpyAttributeColor *pDpyColor,
                                 NVHwModeTimingsEvoPtr pTimings,
                                 enum nvKmsTimingsProtocol *pTimingsProtocol)
{
    NVConnectorEvoPtr pConnectorEvo = pDpyEvo->pConnectorEvo;
    const NvU32 rmProtocol = pConnectorEvo->or.protocol;
    const NvKmsDpyOutputColorFormatInfo colorFormatsInfo =
        nvDpyGetOutputColorFormatInfo(pDpyEvo);
    const NvBool forceHdmiFrlIsSupported = FALSE;

    nvAssert(rmProtocol == NV0073_CTRL_SPECIFIC_OR_PROTOCOL_SOR_DUAL_TMDS ||
             rmProtocol == NV0073_CTRL_SPECIFIC_OR_PROTOCOL_SOR_SINGLE_TMDS_A ||
             rmProtocol == NV0073_CTRL_SPECIFIC_OR_PROTOCOL_SOR_SINGLE_TMDS_B);

    /* Override protocol if this mode requires HDMI FRL. */
    /* If we don't require boot clocks... */
    if (((overrides & NVKMS_MODE_VALIDATION_REQUIRE_BOOT_CLOCKS) == 0) &&
            ((nvHdmiGetEffectivePixelClockKHz(pDpyEvo, pTimings, pDpyColor) >
                pDpyEvo->maxSingleLinkPixelClockKHz) ||
             forceHdmiFrlIsSupported) &&
            /* If FRL is supported... */
            nvHdmiDpySupportsFrl(pDpyEvo)) {

        /* Hardware does not support HDMI FRL with YUV422 */
        if ((pDpyColor->format ==
                NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr422) &&
                !GetDefaultFrlDpyColor(&colorFormatsInfo, pDpyColor)) {
            return FALSE;
        }

        *pTimingsProtocol = NVKMS_PROTOCOL_SOR_HDMI_FRL;
        return TRUE;
    }

    do {
        if (nvHdmiGetEffectivePixelClockKHz(pDpyEvo, pTimings, pDpyColor) <=
               pDpyEvo->maxSingleLinkPixelClockKHz) {

            switch (rmProtocol) {
                case NV0073_CTRL_SPECIFIC_OR_PROTOCOL_SOR_DUAL_TMDS:
                    /*
                     * Force single link TMDS protocol. HDMI does not support
                     * physically support dual link TMDS.
                     *
                     * TMDS_A: "use A side of the link"
                     */
                    *pTimingsProtocol = NVKMS_PROTOCOL_SOR_SINGLE_TMDS_A;
                    break;
                case NV0073_CTRL_SPECIFIC_OR_PROTOCOL_SOR_SINGLE_TMDS_A:
                    *pTimingsProtocol = NVKMS_PROTOCOL_SOR_SINGLE_TMDS_A;
                    break;
                case NV0073_CTRL_SPECIFIC_OR_PROTOCOL_SOR_SINGLE_TMDS_B:
                    *pTimingsProtocol = NVKMS_PROTOCOL_SOR_SINGLE_TMDS_B;
                    break;
                default:
                    return FALSE;
            }
            return TRUE;
        }
    } while (nvDowngradeColorSpaceAndBpc(&colorFormatsInfo,
                                         pDpyColor));
    return FALSE;
}

/*
 * GetDfpProtocol()- determine the protocol to use on the given pDpy
 * with the given pTimings; assigns pTimings->protocol.
 */

static NvBool GetDfpProtocol(const NVDpyEvoRec *pDpyEvo,
                             const struct NvKmsModeValidationParams *pParams,
                             NVDpyAttributeColor *pDpyColor,
                             NVHwModeTimingsEvoPtr pTimings)
{
    NVConnectorEvoPtr pConnectorEvo = pDpyEvo->pConnectorEvo;
    const NvU32 rmProtocol = pConnectorEvo->or.protocol;
    const NvU32 overrides = pParams->overrides;
    enum nvKmsTimingsProtocol timingsProtocol;

    nvAssert(pConnectorEvo->legacyType ==
             NV0073_CTRL_SPECIFIC_DISPLAY_TYPE_DFP);

    if (pConnectorEvo->or.type == NV0073_CTRL_SPECIFIC_OR_TYPE_SOR) {
        if (nvDpyIsHdmiEvo(pDpyEvo)) {
            if (!GetDfpHdmiProtocol(pDpyEvo, overrides, pDpyColor, pTimings,
                                    &timingsProtocol)) {
                return FALSE;
            }

        } else {
            switch (rmProtocol) {
            default:
                nvAssert(!"unrecognized SOR RM protocol");
                return FALSE;
            case NV0073_CTRL_SPECIFIC_OR_PROTOCOL_SOR_SINGLE_TMDS_A:
                if (nvDpyRequiresDualLinkEvo(pDpyEvo, pTimings) &&
                    ((overrides & NVKMS_MODE_VALIDATION_NO_MAX_PCLK_CHECK) == 0)) {
                    return FALSE;
                }
                timingsProtocol = NVKMS_PROTOCOL_SOR_SINGLE_TMDS_A;
                break;
            case NV0073_CTRL_SPECIFIC_OR_PROTOCOL_SOR_SINGLE_TMDS_B:
                if (nvDpyRequiresDualLinkEvo(pDpyEvo, pTimings) &&
                    ((overrides & NVKMS_MODE_VALIDATION_NO_MAX_PCLK_CHECK) == 0)) {
                    return FALSE;
                }
                timingsProtocol = NVKMS_PROTOCOL_SOR_SINGLE_TMDS_B;
                break;
            case NV0073_CTRL_SPECIFIC_OR_PROTOCOL_SOR_DUAL_TMDS:
                /*
                 * Override dual/single link TMDS protocol if necessary.
                 * XXX might be nice to give a way for users to override the
                 * SingleLink/DualLink decision.
                 *
                 * TMDS_A: "use A side of the link"
                 * TMDS_B: "use B side of the link"
                 */
                if (nvDpyRequiresDualLinkEvo(pDpyEvo, pTimings)) {
                    timingsProtocol = NVKMS_PROTOCOL_SOR_DUAL_TMDS;
                } else {
                    timingsProtocol = NVKMS_PROTOCOL_SOR_SINGLE_TMDS_A;
                }
                break;
            case NV0073_CTRL_SPECIFIC_OR_PROTOCOL_SOR_DP_A:
                timingsProtocol = NVKMS_PROTOCOL_SOR_DP_A;
                break;
            case NV0073_CTRL_SPECIFIC_OR_PROTOCOL_SOR_DP_B:
                timingsProtocol = NVKMS_PROTOCOL_SOR_DP_B;
                break;
            case NV0073_CTRL_SPECIFIC_OR_PROTOCOL_SOR_LVDS_CUSTOM:
                timingsProtocol = NVKMS_PROTOCOL_SOR_LVDS_CUSTOM;
                break;
            }
        }
    } else if (pConnectorEvo->or.type == NV0073_CTRL_SPECIFIC_OR_TYPE_PIOR) {
        nvAssert(rmProtocol == NV0073_CTRL_SPECIFIC_OR_PROTOCOL_PIOR_EXT_TMDS_ENC);
        timingsProtocol = NVKMS_PROTOCOL_PIOR_EXT_TMDS_ENC;
    } else if (pConnectorEvo->or.type == NV0073_CTRL_SPECIFIC_OR_TYPE_DSI) {
        nvAssert(rmProtocol == NV0073_CTRL_SPECIFIC_OR_PROTOCOL_DSI);
        timingsProtocol = NVKMS_PROTOCOL_DSI;
    } else {
        nvAssert(!"Unknown OR type");
        return FALSE;
    }

    pTimings->protocol = timingsProtocol;

    return TRUE;

}



/*
 * ConstructHwModeTimingsEvoCrt() - construct EVO hardware timings to
 * drive a CRT, given the mode timings in pMt
 */

static NvBool
ConstructHwModeTimingsEvoCrt(const NVConnectorEvoRec *pConnectorEvo,
                             const NvModeTimings *pModeTimings,
                             const struct NvKmsSize *pViewPortSizeIn,
                             const struct NvKmsRect *pViewPortOut,
                             NVHwModeTimingsEvoPtr pTimings,
                             NVEvoInfoStringPtr pInfoString)
{
    ConstructHwModeTimingsFromNvModeTimings(pModeTimings, pTimings);

    /* assign the protocol; we expect DACs to have RGB protocol */

    nvAssert(pConnectorEvo->or.protocol ==
             NV0073_CTRL_SPECIFIC_OR_PROTOCOL_DAC_RGB_CRT);

    pTimings->protocol = NVKMS_PROTOCOL_DAC_RGB;

    /* assign scaling fields */

    return ConstructHwModeTimingsViewPort(pConnectorEvo->pDispEvo, pTimings,
                                          pInfoString, pViewPortSizeIn,
                                          pViewPortOut);
}


/*!
 * Construct EVO hardware timings to drive a digital protocol (TMDS,
 * DP, etc).
 *
 * \param[in]  pDpy          The display device for which to build timings.
 * \param[in]  pModeTimings  The hw-neutral description of the timings.
 * \param[out] pTimings      The EVO-specific modetimings.
 *
 * \return     TRUE if the EVO modetimings could be built; FALSE if failure.
 */
static NvBool ConstructHwModeTimingsEvoDfp(const NVDpyEvoRec *pDpyEvo,
                                           const NvModeTimings *pModeTimings,
                                           const struct NvKmsSize *pViewPortSizeIn,
                                           const struct NvKmsRect *pViewPortOut,
                                           NVDpyAttributeColor *pDpyColor,
                                           NVHwModeTimingsEvoPtr pTimings,
                                           const struct
                                           NvKmsModeValidationParams *pParams,
                                           NVEvoInfoStringPtr pInfoString)
{
    NvBool ret;

    ConstructHwModeTimingsFromNvModeTimings(pModeTimings, pTimings);

    ret = GetDfpProtocol(pDpyEvo, pParams, pDpyColor, pTimings);

    if (!ret) {
        return ret;
    }

    ret = ApplyDualLinkRequirements(pDpyEvo, pParams, pTimings, pInfoString);

    if (!ret) {
        return ret;
    }

    return ConstructHwModeTimingsViewPort(pDpyEvo->pDispEvo, pTimings,
                                          pInfoString, pViewPortSizeIn,
                                          pViewPortOut);
}

static NvBool IsColorBpcSupported(
    const NvKmsDpyOutputColorFormatInfo *pSupportedColorFormats,
    const enum NvKmsDpyAttributeCurrentColorSpaceValue format,
    const enum NvKmsDpyAttributeColorBpcValue bpc)
{
    switch (format) {
        case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_RGB:
            return (bpc <= pSupportedColorFormats->rgb444.maxBpc) &&
                        (bpc >= pSupportedColorFormats->rgb444.minBpc);
        case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr422:
            return (bpc <= pSupportedColorFormats->yuv422.maxBpc) &&
                        (bpc >= pSupportedColorFormats->yuv422.minBpc);
        case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr444:
            return (bpc <= pSupportedColorFormats->yuv444.maxBpc) &&
                        (bpc >= pSupportedColorFormats->yuv444.minBpc);
        case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr420:
            return (bpc == NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_8);
    }

    return FALSE;
}

NvBool nvDowngradeColorBpc(
    const NvKmsDpyOutputColorFormatInfo *pSupportedColorFormats,
    NVDpyAttributeColor *pDpyColor)
{
    switch (pDpyColor->bpc) {
        case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_10:
            if (pDpyColor->colorimetry == NVKMS_OUTPUT_COLORIMETRY_BT2100) {
                return FALSE;
            }

            if (!IsColorBpcSupported(pSupportedColorFormats,
                                     pDpyColor->format,
                                     NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_8)) {
                return FALSE;
            }

            pDpyColor->bpc = NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_8;
            break;
        case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_8:
            /* At depth 18 only RGB and full range are allowed */
            if (pDpyColor->format == NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_RGB) {
                if (!IsColorBpcSupported(pSupportedColorFormats,
                                         pDpyColor->format,
                                         NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_6)) {
                    return FALSE;
                }

                pDpyColor->bpc = NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_6;
                pDpyColor->range = NV_KMS_DPY_ATTRIBUTE_COLOR_RANGE_FULL;
            } else {
                return FALSE;
            }
            break;
        case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_UNKNOWN:
        case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_6:
            return FALSE;
    }

    return TRUE;
}

NvBool nvDowngradeColorSpaceAndBpc(
    const NvKmsDpyOutputColorFormatInfo *pSupportedColorFormats,
    NVDpyAttributeColor *pDpyColor)
{
    if (nvDowngradeColorBpc(pSupportedColorFormats, pDpyColor)) {
        return TRUE;
    }

    switch (pDpyColor->format) {
        case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_RGB:
            /* XXX Add support for downgrading to YUV with BT2100 */
            if (pDpyColor->colorimetry == NVKMS_OUTPUT_COLORIMETRY_BT2100) {
                return FALSE;
            }
            /* fallthrough */
        case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr444:
            if (pSupportedColorFormats->yuv422.maxBpc !=
                    NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_UNKNOWN) {
                pDpyColor->format = NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr422;
                pDpyColor->bpc = pSupportedColorFormats->yuv422.maxBpc;
                pDpyColor->range = NV_KMS_DPY_ATTRIBUTE_COLOR_RANGE_LIMITED;
                return TRUE;
            }
            break;
        case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr422: /* fallthrough */
        case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr420:
            break;
    }

    return FALSE;
}

/*
 * nvDPValidateModeEvo() - For DP devices handled by the DP lib, check DP
 * bandwidth and pick the best possible/supported pixel depth to use for
 * the given mode timings.
 */

NvBool nvDPValidateModeEvo(NVDpyEvoPtr pDpyEvo,
                           NVHwModeTimingsEvoPtr pTimings,
                           NVDpyAttributeColor *pDpyColor,
                           const NvBool b2Heads1Or,
                           NVDscInfoEvoRec *pDscInfo,
                           const struct NvKmsModeValidationParams *pParams)
{
    NVConnectorEvoPtr pConnectorEvo = pDpyEvo->pConnectorEvo;
    NVDpyAttributeColor dpyColor = *pDpyColor;
    const NvKmsDpyOutputColorFormatInfo supportedColorFormats =
        nvDpyGetOutputColorFormatInfo(pDpyEvo);

    /* Only do this for DP devices. */
    if (!nvConnectorUsesDPLib(pConnectorEvo)) {
        return TRUE;
    }

    if ((pParams->overrides &
         NVKMS_MODE_VALIDATION_NO_DISPLAYPORT_BANDWIDTH_CHECK) != 0) {
        return TRUE;
    }

    nvAssert(nvDpyUsesDPLib(pDpyEvo));
    nvAssert(pConnectorEvo->or.type == NV0073_CTRL_SPECIFIC_OR_TYPE_SOR);

 tryAgain:

    if (!nvDPValidateModeForDpyEvo(pDpyEvo, &dpyColor, pParams, pTimings,
                                   b2Heads1Or, pDscInfo)) {
        if (nvDowngradeColorSpaceAndBpc(&supportedColorFormats, &dpyColor)) {
             goto tryAgain;
        }
        /*
         * Cannot downgrade pixelDepth further --
         *     this mode is not possible on this DP link, so fail.
         */

        return FALSE;
    }

    *pDpyColor = dpyColor;
    return TRUE;
}

/*
 * Construct the hardware values to program EVO for the specified
 * NVModeTimings
 */

NvBool nvConstructHwModeTimingsEvo(const NVDpyEvoRec *pDpyEvo,
                                   const struct NvKmsMode *pKmsMode,
                                   const struct NvKmsSize *pViewPortSizeIn,
                                   const struct NvKmsRect *pViewPortOut,
                                   NVDpyAttributeColor *pDpyColor,
                                   NVHwModeTimingsEvoPtr pTimings,
                                   const struct NvKmsModeValidationParams
                                   *pParams,
                                   NVEvoInfoStringPtr pInfoString)
{
    const NVConnectorEvoRec *pConnectorEvo = pDpyEvo->pConnectorEvo;
    NvBool ret;

    /* assign the pTimings values */

    if (pConnectorEvo->legacyType ==
               NV0073_CTRL_SPECIFIC_DISPLAY_TYPE_DFP) {
        ret = ConstructHwModeTimingsEvoDfp(pDpyEvo,
                                           &pKmsMode->timings,
                                           pViewPortSizeIn, pViewPortOut,
                                           pDpyColor, pTimings, pParams,
                                           pInfoString);
    } else if (pConnectorEvo->legacyType ==
               NV0073_CTRL_SPECIFIC_DISPLAY_TYPE_CRT) {
        ret = ConstructHwModeTimingsEvoCrt(pConnectorEvo,
                                           &pKmsMode->timings,
                                           pViewPortSizeIn, pViewPortOut,
                                           pTimings, pInfoString);
    } else {
        nvAssert(!"Invalid pDpyEvo->type");
        return FALSE;
    }

    if (!ret) return FALSE;

    /* tweak the raster timings for gsync */

    if (pDpyEvo->pDispEvo->pFrameLockEvo) {
        // if this fails, the timing remains untweaked, which just means
        // that the mode may not work well with frame lock
        TweakTimingsForGsync(pDpyEvo, pTimings, pInfoString, pParams->stereoMode);
    }

    return TRUE;
}

static NvBool DowngradeViewPortTaps(const NVEvoHeadCaps *pHeadCaps,
                                    NVHwModeViewPortEvoPtr pViewPort,
                                    NVEvoScalerTaps srcTaps,
                                    NVEvoScalerTaps dstTaps,
                                    NvBool isVert,
                                    NVEvoScalerTaps *pTaps)
{
    const NVEvoScalerCaps *pScalerCaps = &pHeadCaps->scalerCaps;
    NvBool dstPossible;

    if (isVert) {
        dstPossible = IsVTapsPossible(pScalerCaps, pViewPort->in.width,
                                      pViewPort->out.width, dstTaps);
    } else {
        dstPossible = pScalerCaps->taps[dstTaps].maxHDownscaleFactor > 0;
    }

    if (*pTaps >= srcTaps && dstPossible) {
        *pTaps = dstTaps;
        return TRUE;
    }

    return FALSE;
}

/* Downgrade the htaps from 8 to 5 */
static NvBool DowngradeViewPortHTaps8(const NVDevEvoRec *pDevEvo,
                                      const NvU32 head,
                                      const NVEvoHeadCaps *pHeadCaps,
                                      NVHwModeViewPortEvoPtr pViewPort,
                                      NvU64 unused)
{
    return DowngradeViewPortTaps(pHeadCaps,
                                 pViewPort,
                                 NV_EVO_SCALER_8TAPS,
                                 NV_EVO_SCALER_5TAPS,
                                 FALSE /* isVert */,
                                 &pViewPort->hTaps);
}

/* Downgrade the htaps from 5 to 2 */
static NvBool DowngradeViewPortHTaps5(const NVDevEvoRec *pDevEvo,
                                      const NvU32 head,
                                      const NVEvoHeadCaps *pHeadCaps,
                                      NVHwModeViewPortEvoPtr pViewPort,
                                      NvU64 unused)
{
    return DowngradeViewPortTaps(pHeadCaps,
                                 pViewPort,
                                 NV_EVO_SCALER_5TAPS,
                                 NV_EVO_SCALER_2TAPS,
                                 FALSE /* isVert */,
                                 &pViewPort->hTaps);
}

/* Downgrade the vtaps from 5 to 3 */
static NvBool DowngradeViewPortVTaps5(const NVDevEvoRec *pDevEvo,
                                      const NvU32 head,
                                      const NVEvoHeadCaps *pHeadCaps,
                                      NVHwModeViewPortEvoPtr pViewPort,
                                      NvU64 unused)
{
    return DowngradeViewPortTaps(pHeadCaps,
                                 pViewPort,
                                 NV_EVO_SCALER_5TAPS,
                                 NV_EVO_SCALER_3TAPS,
                                 TRUE /* isVert */,
                                 &pViewPort->vTaps);
}

/* Downgrade the vtaps from 3 to 2 */
static NvBool DowngradeViewPortVTaps3(const NVDevEvoRec *pDevEvo,
                                      const NvU32 head,
                                      const NVEvoHeadCaps *pHeadCaps,
                                      NVHwModeViewPortEvoPtr pViewPort,
                                      NvU64 unused)
{
    return DowngradeViewPortTaps(pHeadCaps,
                                 pViewPort,
                                 NV_EVO_SCALER_3TAPS,
                                 NV_EVO_SCALER_2TAPS,
                                 TRUE /* isVert */,
                                 &pViewPort->vTaps);
}

static NvBool
DowngradeLayerDownscaleFactor(NVHwModeViewPortEvoPtr pViewPort,
                              const NvU32 layer,
                              NvU16 srcFactor,
                              NvU16 dstFactor,
                              NvU16 *pFactor)
{
    struct NvKmsUsageBounds *pUsage = &pViewPort->guaranteedUsage;

    if (!pUsage->layer[layer].usable) {
        return FALSE;
    }

    if (*pFactor == srcFactor) {
        *pFactor = dstFactor;
        return TRUE;
    }

    return FALSE;
}

static NvBool
DowngradeLayerVDownscaleFactor4X(const NVDevEvoRec *pDevEvo,
                                 const NvU32 head,
                                 const NVEvoHeadCaps *pHeadCaps,
                                 NVHwModeViewPortEvoPtr pViewPort,
                                 NvU64 unused)
{
    NvU32 layer;

    for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
        struct NvKmsScalingUsageBounds *pScaling =
            &pViewPort->guaranteedUsage.layer[layer].scaling;

        if (DowngradeLayerDownscaleFactor(pViewPort,
                                          layer,
                                          NV_EVO_SCALE_FACTOR_4X,
                                          NV_EVO_SCALE_FACTOR_3X,
                                          &pScaling->maxVDownscaleFactor)) {
            return TRUE;
        }
    }

    return FALSE;
}

static NvBool
DowngradeLayerVDownscaleFactor3X(const NVDevEvoRec *pDevEvo,
                                 const NvU32 head,
                                 const NVEvoHeadCaps *pHeadCaps,
                                 NVHwModeViewPortEvoPtr pViewPort,
                                 NvU64 unused)
{
    NvU32 layer;

    for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
        struct NvKmsScalingUsageBounds *pScaling =
            &pViewPort->guaranteedUsage.layer[layer].scaling;

        if (DowngradeLayerDownscaleFactor(pViewPort,
                                          layer,
                                          NV_EVO_SCALE_FACTOR_3X,
                                          NV_EVO_SCALE_FACTOR_2X,
                                          &pScaling->maxVDownscaleFactor)) {
            return TRUE;
        }
    }

    return FALSE;
}

static NvBool
DowngradeLayerVDownscaleFactor2X(const NVDevEvoRec *pDevEvo,
                                 const NvU32 head,
                                 const NVEvoHeadCaps *pHeadCaps,
                                 NVHwModeViewPortEvoPtr pViewPort,
                                 NvU64 unused)
{
    NvU32 layer;

    for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
        struct NvKmsScalingUsageBounds *pScaling =
            &pViewPort->guaranteedUsage.layer[layer].scaling;

        if (DowngradeLayerDownscaleFactor(pViewPort,
                                          layer,
                                          NV_EVO_SCALE_FACTOR_2X,
                                          NV_EVO_SCALE_FACTOR_1X,
                                          &pScaling->maxVDownscaleFactor)) {
            return TRUE;
        }
    }

    return FALSE;
}

static NvBool
DowngradeLayerHDownscaleFactor4X(const NVDevEvoRec *pDevEvo,
                                 const NvU32 head,
                                 const NVEvoHeadCaps *pHeadCaps,
                                 NVHwModeViewPortEvoPtr pViewPort,
                                 NvU64 unused)
{
    NvU32 layer;

    for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
        struct NvKmsScalingUsageBounds *pScaling =
            &pViewPort->guaranteedUsage.layer[layer].scaling;

        if (DowngradeLayerDownscaleFactor(pViewPort,
                                          layer,
                                          NV_EVO_SCALE_FACTOR_4X,
                                          NV_EVO_SCALE_FACTOR_3X,
                                          &pScaling->maxHDownscaleFactor)) {
            return TRUE;
        }
    }

    return FALSE;
}

static NvBool DowngradeLayerHDownscaleFactor3X(const NVDevEvoRec *pDevEvo,
                                               const NvU32 head,
                                               const NVEvoHeadCaps *pHeadCaps,
                                               NVHwModeViewPortEvoPtr pViewPort,
                                               NvU64 unused)
{
    NvU32 layer;

    for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
        struct NvKmsScalingUsageBounds *pScaling =
            &pViewPort->guaranteedUsage.layer[layer].scaling;

        if (DowngradeLayerDownscaleFactor(pViewPort,
                                     layer,
                                     NV_EVO_SCALE_FACTOR_3X,
                                     NV_EVO_SCALE_FACTOR_2X,
                                     &pScaling->maxHDownscaleFactor)) {
            return TRUE;
        }
    }

    return FALSE;
}

static NvBool DowngradeLayerHDownscaleFactor2X(const NVDevEvoRec *pDevEvo,
                                               const NvU32 head,
                                               const NVEvoHeadCaps *pHeadCaps,
                                               NVHwModeViewPortEvoPtr pViewPort,
                                               NvU64 unused)
{
    NvU32 layer;

    for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
        struct NvKmsScalingUsageBounds *pScaling =
            &pViewPort->guaranteedUsage.layer[layer].scaling;

        if (DowngradeLayerDownscaleFactor(pViewPort,
                                          layer,
                                          NV_EVO_SCALE_FACTOR_2X,
                                          NV_EVO_SCALE_FACTOR_1X,
                                          &pScaling->maxHDownscaleFactor)) {
            return TRUE;
        }
    }

    return FALSE;
}

/* Downgrade the vtaps from 5 to 2 */
static NvBool DowngradeLayerVTaps5(const NVDevEvoRec *pDevEvo,
                                   const NvU32 head,
                                   const NVEvoHeadCaps *pHeadCaps,
                                   NVHwModeViewPortEvoPtr pViewPort,
                                   NvU64 unused)
{
    struct NvKmsUsageBounds *pUsage = &pViewPort->guaranteedUsage;
    NvU32 layer;

    for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
        struct NvKmsScalingUsageBounds *pScaling =
            &pUsage->layer[layer].scaling;

        if (!pUsage->layer[layer].usable) {
            continue;
        }

        if (pScaling->vTaps == NV_EVO_SCALER_5TAPS) {
            pScaling->vTaps = NV_EVO_SCALER_2TAPS;
            return TRUE;
        }
    }

    return FALSE;
}

static NvBool DowngradeLayerVUpscaling(const NVDevEvoRec *pDevEvo,
                                       const NvU32 head,
                                       const NVEvoHeadCaps *pHeadCaps,
                                       NVHwModeViewPortEvoPtr pViewPort,
                                       NvU64 unused)
{
    struct NvKmsUsageBounds *pUsage = &pViewPort->guaranteedUsage;
    NvU32 layer;

    for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
        struct NvKmsScalingUsageBounds *pScaling =
            &pUsage->layer[layer].scaling;

        if (!pUsage->layer[layer].usable) {
            continue;
        }

        if (pScaling->vUpscalingAllowed) {
            pScaling->vUpscalingAllowed = FALSE;
            return TRUE;
        }
    }

    return FALSE;
}

static NvBool DowngradeViewPortOverlayFormats(
    const NVDevEvoRec *pDevEvo,
    const NvU32 head,
    const NVEvoHeadCaps *pHeadCaps,
    NVHwModeViewPortEvoPtr pViewPort,
    NvU64 removeSurfaceMemoryFormats)
{
    struct NvKmsUsageBounds *pUsage = &pViewPort->guaranteedUsage;
    NvU32 layer;

    for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
        if (layer == NVKMS_MAIN_LAYER || !pUsage->layer[layer].usable) {
            continue;
        }

        if (pUsage->layer[layer].supportedSurfaceMemoryFormats &
            removeSurfaceMemoryFormats) {
            pUsage->layer[layer].supportedSurfaceMemoryFormats &=
                ~removeSurfaceMemoryFormats;
            if (pUsage->layer[layer].supportedSurfaceMemoryFormats == 0) {
                pUsage->layer[layer].usable = FALSE;
            }

            return TRUE;
        }
    }

    return FALSE;
}

static NvBool DowngradeViewPortBaseFormats(
    const NVDevEvoRec *pDevEvo,
    const NvU32 head,
    const NVEvoHeadCaps *pHeadCaps,
    NVHwModeViewPortEvoPtr pViewPort,
    NvU64 removeSurfaceMemoryFormats)
{
    struct NvKmsUsageBounds *pUsage = &pViewPort->guaranteedUsage;

    if (!pUsage->layer[NVKMS_MAIN_LAYER].usable) {
        return FALSE;
    }

    if (pUsage->layer[NVKMS_MAIN_LAYER].supportedSurfaceMemoryFormats &
        removeSurfaceMemoryFormats) {
        pUsage->layer[NVKMS_MAIN_LAYER].supportedSurfaceMemoryFormats &=
            ~removeSurfaceMemoryFormats;
        if (pUsage->layer[NVKMS_MAIN_LAYER].supportedSurfaceMemoryFormats == 0) {
            pUsage->layer[NVKMS_MAIN_LAYER].usable = FALSE;
        }

        return TRUE;
    }

    return FALSE;
}

typedef NvBool (*DowngradeViewPortFuncPtr)(const NVDevEvoRec *pDevEvo,
                                           const NvU32 head,
                                           const NVEvoHeadCaps *pHeadCaps,
                                           NVHwModeViewPortEvoPtr pViewPort,
                                           NvU64 removeSurfaceMemoryFormats);

/*
 * Try to downgrade the usage bounds of the viewports, keeping the
 * viewports roughly equal in capability; we do this from
 * ValidateMetaMode50() when IMP rejects the mode.  Return TRUE if we
 * were able to downgrade something; return FALSE if there was nothing
 * left to downgrade.
 */

static NvBool DownGradeMetaModeUsageBounds(
    const NVDevEvoRec                      *pDevEvo,
    const NVValidateImpOneDispHeadParamsRec timingsParams[NVKMS_MAX_HEADS_PER_DISP],
    NvU32                                   downgradePossibleHeadsBitMask)
{
    static const struct {
        DowngradeViewPortFuncPtr downgradeFunc;
        NvU64 removeSurfaceMemoryFormats;
    } downgradeFuncs[] = {
        { DowngradeLayerVDownscaleFactor4X,
          0 },
        { DowngradeLayerHDownscaleFactor4X,
          0 },
        { DowngradeViewPortOverlayFormats,
          NVKMS_SURFACE_MEMORY_FORMATS_YUV_PLANAR444 },
        { DowngradeViewPortBaseFormats,
          NVKMS_SURFACE_MEMORY_FORMATS_YUV_PLANAR444 },
        { DowngradeViewPortOverlayFormats,
          NVKMS_SURFACE_MEMORY_FORMATS_YUV_PLANAR420 },
        { DowngradeViewPortBaseFormats,
          NVKMS_SURFACE_MEMORY_FORMATS_YUV_PLANAR420 },
        { DowngradeViewPortOverlayFormats,
          NVKMS_SURFACE_MEMORY_FORMATS_EXT_YUV_SP444 },
        { DowngradeViewPortBaseFormats,
          NVKMS_SURFACE_MEMORY_FORMATS_EXT_YUV_SP444 },
        { DowngradeViewPortOverlayFormats,
          NVKMS_SURFACE_MEMORY_FORMATS_EXT_YUV_SP422 },
        { DowngradeViewPortBaseFormats,
          NVKMS_SURFACE_MEMORY_FORMATS_EXT_YUV_SP422 },
        { DowngradeViewPortOverlayFormats,
          NVKMS_SURFACE_MEMORY_FORMATS_EXT_YUV_SP420 },
        { DowngradeViewPortBaseFormats,
          NVKMS_SURFACE_MEMORY_FORMATS_EXT_YUV_SP420 },
        { DowngradeViewPortOverlayFormats,
          NVKMS_SURFACE_MEMORY_FORMATS_YUV_SP444 },
        { DowngradeViewPortBaseFormats,
          NVKMS_SURFACE_MEMORY_FORMATS_YUV_SP444 },
        { DowngradeViewPortOverlayFormats,
          NVKMS_SURFACE_MEMORY_FORMATS_YUV_SP422 },
        { DowngradeViewPortBaseFormats,
          NVKMS_SURFACE_MEMORY_FORMATS_YUV_SP422 },
        { DowngradeViewPortOverlayFormats,
          NVKMS_SURFACE_MEMORY_FORMATS_YUV_SP420 },
        { DowngradeViewPortBaseFormats,
          NVKMS_SURFACE_MEMORY_FORMATS_YUV_SP420 },
        { DowngradeViewPortOverlayFormats,
          NVKMS_SURFACE_MEMORY_FORMATS_YUV_PACKED422 },
        { DowngradeViewPortBaseFormats,
          NVKMS_SURFACE_MEMORY_FORMATS_YUV_PACKED422 },
        { DowngradeViewPortOverlayFormats,
          NVKMS_SURFACE_MEMORY_FORMATS_RGB_PACKED8BPP },
        { DowngradeViewPortBaseFormats,
          NVKMS_SURFACE_MEMORY_FORMATS_RGB_PACKED8BPP },
        { DowngradeLayerVDownscaleFactor3X,
          0 },
        { DowngradeLayerHDownscaleFactor3X,
          0 },
        { DowngradeViewPortVTaps5,
          0 },
        { DowngradeViewPortVTaps3,
          0 },
        { DowngradeViewPortHTaps8,
          0 },
        { DowngradeViewPortHTaps5,
          0 },
        { DowngradeViewPortOverlayFormats,
          NVKMS_SURFACE_MEMORY_FORMATS_RGB_PACKED4BPP },
        { DowngradeLayerVTaps5,
          0 },
        { DowngradeViewPortOverlayFormats,
          NVKMS_SURFACE_MEMORY_FORMATS_RGB_PACKED2BPP },
        { DowngradeLayerVDownscaleFactor2X,
          0 },
        { DowngradeLayerHDownscaleFactor2X,
          0 },
        { DowngradeLayerVUpscaling,
          0 },
        { DowngradeViewPortOverlayFormats,
          NVKMS_SURFACE_MEMORY_FORMATS_RGB_PACKED1BPP },
        { DowngradeViewPortBaseFormats,
          NVKMS_SURFACE_MEMORY_FORMATS_RGB_PACKED4BPP },
        { DowngradeViewPortBaseFormats,
          NVKMS_SURFACE_MEMORY_FORMATS_RGB_PACKED2BPP },
        { DowngradeViewPortBaseFormats,
          NVKMS_SURFACE_MEMORY_FORMATS_RGB_PACKED1BPP },
    };
    int i;

    // XXX assume the heads have equal capabilities
    // XXX assume the gpus have equal capabilities

    const NVEvoHeadCaps *pHeadCaps =
        &pDevEvo->gpus[0].capabilities.head[0];


    for (i = 0; i < ARRAY_LEN(downgradeFuncs); i++) {
        int head;
        FOR_ALL_HEADS(head, downgradePossibleHeadsBitMask) {
            if (timingsParams[head].pTimings == NULL) {
                continue;
            }

            if (downgradeFuncs[i].downgradeFunc(
                    pDevEvo,
                    head,
                    pHeadCaps,
                    &timingsParams[head].pTimings->viewPort,
                    downgradeFuncs[i].removeSurfaceMemoryFormats)) {
                return TRUE;
            }
        }
    }

    /* Nothing else to downgrade */
    return FALSE;
}

NvBool nvAllocateDisplayBandwidth(
    NVDispEvoPtr pDispEvo,
    NvU32 newIsoBandwidthKBPS,
    NvU32 newDramFloorKBPS)
{
    NV0073_CTRL_SYSTEM_ALLOCATE_DISPLAY_BANDWIDTH_PARAMS params = { };
    const NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    NvU32 ret;

    if (!pDevEvo->isSOCDisplay) {
        return TRUE;
    }

    params.subDeviceInstance = 0;
    params.averageBandwidthKBPS = newIsoBandwidthKBPS;
    params.floorBandwidthKBPS = newDramFloorKBPS;

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->displayCommonHandle,
                         NV0073_CTRL_CMD_SYSTEM_ALLOCATE_DISPLAY_BANDWIDTH,
                         &params, sizeof(params));
    if (ret != NV_OK) {
        nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                    "Failed to allocate %u KBPS Iso and %u KBPS Dram",
                    newIsoBandwidthKBPS, newDramFloorKBPS);
        return FALSE;
    }

    pDispEvo->isoBandwidthKBPS = newIsoBandwidthKBPS;
    pDispEvo->dramFloorKBPS = newDramFloorKBPS;

    return TRUE;
}

static void AssignNVEvoIsModePossibleDispInput(
    NVDispEvoPtr                             pDispEvo,
    const NVValidateImpOneDispHeadParamsRec  timingsParams[NVKMS_MAX_HEADS_PER_DISP],
    NvBool                                   requireBootClocks,
    NVEvoReallocateBandwidthMode             reallocBandwidth,
    NVEvoIsModePossibleDispInput            *pImpInput)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NvU32 head;
    NvU32 nextSorIndex = 0;

    nvkms_memset(pImpInput, 0, sizeof(*pImpInput));

    pImpInput->requireBootClocks = requireBootClocks;
    pImpInput->reallocBandwidth = reallocBandwidth;

    for (head = 0; head < NVKMS_MAX_HEADS_PER_DISP; head++) {
        const NVConnectorEvoRec *pConnectorEvo =
                    timingsParams[head].pConnectorEvo;
        NvU32 otherHead = 0;

        nvAssert((timingsParams[head].pTimings == NULL) ==
                 (timingsParams[head].pConnectorEvo == NULL));

        pImpInput->head[head].orIndex = NV_INVALID_OR;

        if (timingsParams[head].pTimings == NULL) {
            continue;
        }

        pImpInput->head[head].pTimings = timingsParams[head].pTimings;
        pImpInput->head[head].enableDsc = timingsParams[head].enableDsc;
        pImpInput->head[head].b2Heads1Or = timingsParams[head].b2Heads1Or;
        pImpInput->head[head].pixelDepth = timingsParams[head].pixelDepth;
        pImpInput->head[head].displayId = timingsParams[head].activeRmId;
        pImpInput->head[head].orType = pConnectorEvo->or.type;
        pImpInput->head[head].pUsage = timingsParams[head].pUsage;

        if (!NV0073_CTRL_SYSTEM_GET_CAP(pDevEvo->commonCapsBits,
                NV0073_CTRL_SYSTEM_CAPS_CROSS_BAR_SUPPORTED) ||
             pConnectorEvo->or.type != NV0073_CTRL_SPECIFIC_OR_TYPE_SOR) {

            nvAssert(pConnectorEvo->or.primary != NV_INVALID_OR);

            pImpInput->head[head].orIndex = pConnectorEvo->or.primary;
            continue;
        }

        /*
         * If more than one head is attached to the same connector, then make
         * sure that all of them use the same SOR index.
         */
        for (otherHead = 0; otherHead < head; otherHead++) {
            if (timingsParams[otherHead].pConnectorEvo == pConnectorEvo) {
                pImpInput->head[head].orIndex = pImpInput->head[otherHead].orIndex;
                break;
            }
        }

        /*
         * On GPUs with a full crossbar, the SORs are equally capable, so just
         * use next unused SOR.
         *
         * We assume there are as many SORs as there are heads.
         */
        if (pImpInput->head[head].orIndex == NV_INVALID_OR) {
            pImpInput->head[head].orIndex = nextSorIndex;
            nextSorIndex++;
        }
    }
}

/*!
 * Validate the described disp configuration through IMP.

 * \param[in]      pDispEvo        The disp of the dpyIdList.
 *
 * \param[in.out]  timingsParams[] The proposed configuration to use on each head
 *                                 includes -
 *
 *                                   pConnectorEvo -
 *                                     The proposed connector to drive on each head.
 *
 *                                   activeRmId -
 *                                     The display ID that we use to talk to RM
 *                                     about the dpy(s) on each head.
 *
 *                                   pTimings -
 *                                     The proposed timings to use on each head;
 *                                     note the usage bounds within pTimings
 *                                     may be altered by this function.
 *
 *                                   depth -
 *                                     The depth of the buffer to be displayed on
 *                                     each head.
 * \param[in]      requireBootClocks
 *                                 Only validate modes that will work at P8
 *                                 clocks.
 *
 * \param[in]      reallocBandwidth
 *                                 Try to allocate the required display
 *                                 bandwidth if IMP passes.
 *
 * \param[out]     pMinIsoBandwidthKBPS
 *                                 The ISO bandwidth that's required for the
 *                                 proposed disp configuration only. This value
 *                                 doesn't take the current display state into
 *                                 account.
 *
 * \param[out]     pMinDramFloorKBPS
 *                                 The DRAM floor that's required for the
 *                                 proposed disp configuration only. This value
 *                                 doesn't take the current display state into
 *                                 account.
 *
 * \return         Return TRUE if the proposed disp configuration is
 *                 considered valid for IMP purposes.
 */
NvBool nvValidateImpOneDisp(
    NVDispEvoPtr                            pDispEvo,
    const NVValidateImpOneDispHeadParamsRec timingsParams[NVKMS_MAX_HEADS_PER_DISP],
    NvBool                                  requireBootClocks,
    NVEvoReallocateBandwidthMode            reallocBandwidth,
    NvU32                                   *pMinIsoBandwidthKBPS,
    NvU32                                   *pMinDramFloorKBPS)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NVEvoIsModePossibleDispInput impInput = { };
    NVEvoIsModePossibleDispOutput impOutput = { };
    NvU32 newIsoBandwidthKBPS, newDramFloorKBPS;
    NvBool needToRealloc = FALSE;

    AssignNVEvoIsModePossibleDispInput(pDispEvo,
                                       timingsParams, requireBootClocks,
                                       reallocBandwidth,
                                       &impInput);

    pDevEvo->hal->IsModePossible(pDispEvo, &impInput, &impOutput);
    if (!impOutput.possible) {
        return FALSE;
    }

    switch (reallocBandwidth) {
        case NV_EVO_REALLOCATE_BANDWIDTH_MODE_PRE:
            needToRealloc = (impOutput.minRequiredBandwidthKBPS > pDispEvo->isoBandwidthKBPS) ||
                            (impOutput.floorBandwidthKBPS > pDispEvo->dramFloorKBPS);
            newIsoBandwidthKBPS =
                NV_MAX(pDispEvo->isoBandwidthKBPS, impOutput.minRequiredBandwidthKBPS);
            newDramFloorKBPS =
                NV_MAX(pDispEvo->dramFloorKBPS, impOutput.floorBandwidthKBPS);

            break;
        case NV_EVO_REALLOCATE_BANDWIDTH_MODE_POST:
            needToRealloc = (impOutput.minRequiredBandwidthKBPS != pDispEvo->isoBandwidthKBPS) ||
                            (impOutput.floorBandwidthKBPS != pDispEvo->dramFloorKBPS);
            newIsoBandwidthKBPS = impOutput.minRequiredBandwidthKBPS;
            newDramFloorKBPS = impOutput.floorBandwidthKBPS;

            break;
        case NV_EVO_REALLOCATE_BANDWIDTH_MODE_NONE:
        default:
            break;
    }

    if (needToRealloc) {
        if (!nvAllocateDisplayBandwidth(pDispEvo,
                                        newIsoBandwidthKBPS,
                                        newDramFloorKBPS)) {
            return FALSE;
        }
    }

    if (pMinIsoBandwidthKBPS != NULL) {
        *pMinIsoBandwidthKBPS = impOutput.minRequiredBandwidthKBPS;
    }

    if (pMinDramFloorKBPS != NULL) {
        *pMinDramFloorKBPS = impOutput.floorBandwidthKBPS;
    }

    return TRUE;
}

NvBool nvValidateImpOneDispDowngrade(
    NVDispEvoPtr                            pDispEvo,
    const NVValidateImpOneDispHeadParamsRec timingsParams[NVKMS_MAX_HEADS_PER_DISP],
    NvBool                                  requireBootClocks,
    NVEvoReallocateBandwidthMode            reallocBandwidth,
    NvU32                                   downgradePossibleHeadsBitMask)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NvBool impPassed = FALSE;

    do {
        impPassed = nvValidateImpOneDisp(pDispEvo,
                                         timingsParams,
                                         requireBootClocks,
                                         reallocBandwidth,
                                         NULL /* pMinIsoBandwidthKBPS */,
                                         NULL /* pMinDramFloorKBPS */);
        if (impPassed) {
            break;
        }
    } while (DownGradeMetaModeUsageBounds(pDevEvo, timingsParams,
                                          downgradePossibleHeadsBitMask));

    if (impPassed && !pDevEvo->isSOCDisplay) {
        NvU32 head;

        for (head = 0; head < pDevEvo->numHeads; head++) {
            if (timingsParams[head].pTimings != NULL) {
                timingsParams[head].pTimings->viewPort.possibleUsage =
                    timingsParams[head].pTimings->viewPort.guaranteedUsage;
            }
        }
    }

    return impPassed;
}

/*
 * Return TRUE iff this display can be configured as a framelock
 * server given the current modetimings/framelock configuration, FALSE
 * o.w.
 */

NvBool nvFrameLockServerPossibleEvo(const NVDpyEvoRec *pDpyEvo)
{

    NVDispEvoPtr pDispEvo = pDpyEvo->pDispEvo;
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NVEvoSubDevPtr pEvoSubDev = &pDevEvo->gpus[pDispEvo->displayOwner];

    return pEvoSubDev->scanLockState(pDispEvo, pEvoSubDev,
                                     NV_EVO_ADD_FRAME_LOCK_SERVER,
                                     NULL);
}

/*
 * Return TRUE iff this display can be configured as a framelock client
 * given the current modetimings/framelock configuration, FALSE o.w.
 */

NvBool nvFrameLockClientPossibleEvo(const NVDpyEvoRec *pDpyEvo)
{
    NVDispEvoPtr pDispEvo = pDpyEvo->pDispEvo;
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NVEvoSubDevPtr pEvoSubDev = &pDevEvo->gpus[pDispEvo->displayOwner];

    return pEvoSubDev->scanLockState(pDispEvo, pEvoSubDev,
                                     NV_EVO_ADD_FRAME_LOCK_CLIENT,
                                     NULL);
}


/*
 * FrameLockSli() - Helper function for nvEvoRefFrameLockSli() and
 * nvEvoUnRefFrameLockSli(), which are hooked into the EVO locking state
 * machine via custom rules.  This function will find the GPU acting as the
 * given GPU's SLI primary and perform the NV_EVO_{ADD,REM}_FRAMELOCK_REF action
 * to increment or decrement the refcount on that GPU.
 * If queryOnly, it also figures out which displays to pass into the EVO state
 * machine; otherwise, it passes NULLs to perform a query without affecting
 * state.
 */

static NvBool FrameLockSli(NVDevEvoPtr pDevEvo,
                           NvU32 action,
                           NvBool queryOnly)
{
    RasterLockGroup *pRasterLockGroups;
    NVEvoSubDevPtr pEvoSubDev;
    NVDispEvoPtr pDispEvo;
    unsigned int numRasterLockGroups;

    pRasterLockGroups = GetRasterLockGroups(pDevEvo, &numRasterLockGroups);
    if (!pRasterLockGroups) {
        return FALSE;
    }

    nvAssert(numRasterLockGroups == 1);
    if (numRasterLockGroups != 1) {
        nvFree(pRasterLockGroups);
        return FALSE;
    }

    /* Want to be framelock server */
    pDispEvo = pRasterLockGroups[0].pDispEvoOrder[0];

    nvFree(pRasterLockGroups);

    if (!pDispEvo) {
        return FALSE;
    }

    nvAssert(pDevEvo == pDispEvo->pDevEvo);

    pEvoSubDev = &pDevEvo->gpus[pDispEvo->displayOwner];

    if (queryOnly) {
        return pEvoSubDev->scanLockState(pDispEvo, pEvoSubDev, action, NULL);
    } else {
        NvU32 pHeads[NVKMS_MAX_HEADS_PER_DISP + 1] = { NV_INVALID_HEAD, };
        NvU32 i = 0;
        NvU32 head;

        for (head = 0; head < NVKMS_MAX_HEADS_PER_DISP; head++) {
            if (nvHeadIsActive(pDispEvo, head)) {
                pHeads[i++] = head;
            }
        }
        nvAssert(i > 0 && i <= NVKMS_MAX_HEADS_PER_DISP);
        pHeads[i] = NV_INVALID_HEAD;

        return pEvoSubDev->scanLockState(pDispEvo, pEvoSubDev, action,
                                         pHeads);
    }
}


/*
 * nvEvoRefFrameLockSli() - Attempt to set up framelock on the GPU's SLI
 * primary.  Hooked into EVO state machine via custom rules.
 * If pHeads is NULL, only perform a query.
 */

NvBool nvEvoRefFrameLockSli(NVDispEvoPtr pDispEvo,
                            NVEvoSubDevPtr pEvoSubDev,
                            const NvU32 *pHeads)
{
    return FrameLockSli(pDispEvo->pDevEvo, NV_EVO_ADD_FRAME_LOCK_REF,
                        pHeads == NULL);

} /* nvEvoRefFrameLockSli */


/*
 * nvEvoUnRefFrameLockSli() - Attempt to clean up framelock on the GPU's SLI
 * primary.  Hooked into EVO state machine via custom rules.
 * If pHeads is NULL, only perform a query.
 */

NvBool nvEvoUnRefFrameLockSli(NVDispEvoPtr pDispEvo,
                              NVEvoSubDevPtr pEvoSubDev,
                              const NvU32 *pHeads)
{
    return FrameLockSli(pDispEvo->pDevEvo, NV_EVO_REM_FRAME_LOCK_REF,
                        pHeads == NULL);

} /* nvEvoUnRefFrameLockSli */


/*
 * GetRasterLockPin() - Ask RM which lockpin to use in order to configure GPU0
 * be a server or client of GPU1, where GPUn is represented by the duple
 * (pDispn, headn) (or NV_EVO_LOCK_PIN_ERROR if the two cannot be locked).
 */
static void GetRasterLockPin(NVDispEvoPtr pDispEvo0, NvU32 head0,
                             NVDispEvoPtr pDispEvo1, NvU32 head1,
                             NVEvoLockPin *serverPin, NVEvoLockPin *clientPin)
{
    NV5070_CTRL_GET_RG_CONNECTED_LOCKPIN_STATELESS_PARAMS params = { };
    NvU32 displayHandle0 = pDispEvo0->pDevEvo->displayHandle;
    NvU32 displayHandle1 = pDispEvo1->pDevEvo->displayHandle;
    NvU32 ret;

    params.base.subdeviceIndex = pDispEvo0->displayOwner;
    params.head = head0;

    params.peer.hDisplay = displayHandle1;
    params.peer.subdeviceIndex = pDispEvo1->displayOwner;
    params.peer.head = head1;

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         displayHandle0,
                         NV5070_CTRL_CMD_GET_RG_CONNECTED_LOCKPIN_STATELESS,
                         &params, sizeof(params));

    if (ret != NVOS_STATUS_SUCCESS) {
        nvEvoLogDispDebug(pDispEvo0, EVO_LOG_ERROR,
                          "stateless lockpin query failed; ret: 0x%x", ret);
        if (serverPin) *serverPin = NV_EVO_LOCK_PIN_ERROR;
        if (clientPin) *clientPin = NV_EVO_LOCK_PIN_ERROR;
        return;
    }

    if (serverPin) {
        if (FLD_TEST_DRF(5070, _CTRL_CMD_GET_RG_CONNECTED_LOCKPIN_STATELESS,
                               _MASTER_SCAN_LOCK_CONNECTED, _NO,
                               params.masterScanLock)) {
            *serverPin = NV_EVO_LOCK_PIN_ERROR;
        } else {
            int pin = DRF_VAL(5070, _CTRL_CMD_GET_RG_CONNECTED_LOCKPIN_STATELESS,
                              _MASTER_SCAN_LOCK_PIN,
                              params.masterScanLock);
            *serverPin = NV_EVO_LOCK_PIN_0 + pin;
        }
    }

    if (clientPin) {
        if (FLD_TEST_DRF(5070, _CTRL_CMD_GET_RG_CONNECTED_LOCKPIN_STATELESS,
                               _SLAVE_SCAN_LOCK_CONNECTED, _NO,
                               params.slaveScanLock)) {
            *clientPin = NV_EVO_LOCK_PIN_ERROR;
        } else {
            int pin = DRF_VAL(5070, _CTRL_CMD_GET_RG_CONNECTED_LOCKPIN_STATELESS,
                              _SLAVE_SCAN_LOCK_PIN,
                              params.slaveScanLock);
            *clientPin = NV_EVO_LOCK_PIN_0 + pin;
        }
    }
} /* GetRasterLockPin */

static void UpdateLUTNotifierTracking(
    NVDispEvoPtr pDispEvo)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    const int dispIndex = pDispEvo->displayOwner;
    NvU32 i;

    for (i = 0; i < ARRAY_LEN(pDevEvo->lut.notifierState.sd[dispIndex].notifiers); i++) {
        int notifier = pDevEvo->lut.notifierState.sd[dispIndex].notifiers[i].notifier;

        if (!pDevEvo->lut.notifierState.sd[dispIndex].notifiers[i].waiting) {
            continue;
        }

        if (!pDevEvo->hal->IsCompNotifierComplete(pDevEvo->pDispEvo[dispIndex],
                                                  notifier)) {
            continue;
        }

        pDevEvo->lut.notifierState.sd[dispIndex].waitingApiHeadMask &=
            ~pDevEvo->lut.notifierState.sd[dispIndex].notifiers[i].apiHeadMask;
        pDevEvo->lut.notifierState.sd[dispIndex].notifiers[i].waiting = FALSE;
    }
}

/*
 * Check whether there are any staged API head LUT notifiers that need to be
 * committed.
 */
NvBool nvEvoLUTNotifiersNeedCommit(
    NVDispEvoPtr pDispEvo)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    const int dispIndex = pDispEvo->displayOwner;
    NvU32 apiHeadMask = pDevEvo->lut.notifierState.sd[dispIndex].stagedApiHeadMask;

    return apiHeadMask != 0;
}

/*
 * Set up tracking for a LUT Notifier for the apiHeads in stagedApiHeadMask.
 *
 * The notifier returned by this function must be passed to a subsequent call to
 * EvoUpdateAndKickOffWithNotifier.
 *
 * Returns -1 if an error occurs or no apiHeads need a new LUT notifier. Passing
 * the -1 to EvoUpdateAndKickOffWithNotifier with its notify parameter set may
 * result in kernel panics.
 */
int nvEvoCommitLUTNotifiers(
    NVDispEvoPtr pDispEvo)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    const int dispIndex = pDispEvo->displayOwner;
    NvU32 apiHeadMask = pDevEvo->lut.notifierState.sd[dispIndex].stagedApiHeadMask;
    int i;

    pDevEvo->lut.notifierState.sd[dispIndex].stagedApiHeadMask = 0;

    UpdateLUTNotifierTracking(pDispEvo);

    if (apiHeadMask == 0) {
        return -1;
    }

    if (pDevEvo->lut.notifierState.sd[dispIndex].waitingApiHeadMask &
        apiHeadMask) {
        /*
         * an apiHead in the requested list is already waiting on a
         * notifier
         */
        nvEvoLogDisp(pDispEvo, EVO_LOG_WARN, "A requested API head is already waiting on a notifier");
        return -1;
    }

    for (i = 0; i < ARRAY_LEN(pDevEvo->lut.notifierState.sd[dispIndex].notifiers); i++) {
        int notifier = (dispIndex * NVKMS_MAX_HEADS_PER_DISP) + i + 1;

        if (pDevEvo->lut.notifierState.sd[dispIndex].notifiers[i].waiting) {
            continue;
        }

        /* use this notifier */
        pDevEvo->lut.notifierState.sd[dispIndex].notifiers[i].notifier = notifier;
        pDevEvo->lut.notifierState.sd[dispIndex].notifiers[i].waiting = TRUE;
        pDevEvo->lut.notifierState.sd[dispIndex].notifiers[i].apiHeadMask = apiHeadMask;

        pDevEvo->lut.notifierState.sd[dispIndex].waitingApiHeadMask |=
            apiHeadMask;

        return notifier;
    }

    /* slot not found */
    nvEvoLogDisp(pDispEvo, EVO_LOG_WARN, "No remaining LUT notifier slots");
    return -1;
}

/*
 * Unstage any staged API Heads' notifiers.
 */
void nvEvoClearStagedLUTNotifiers(
    NVDispEvoPtr pDispEvo)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    const int dispIndex = pDispEvo->displayOwner;

    pDevEvo->lut.notifierState.sd[dispIndex].stagedApiHeadMask = 0;
}

/*
 * Stage the API Head's notifier for tracking. In order to kickoff the staged
 * notifier, nvEvoCommitLUTNotifiers must be called and its return value
 * passed to EvoUpdateAndKickoffWithNotifier.
 *
 * This function and its siblings nvEvoIsLUTNotifierComplete and
 * nvEvoWaitForLUTNotifier can be used by callers of nvEvoSetLut to ensure the
 * triple-buffer for the color LUT is not overflowed even when nvEvoSetLut is
 * called with kickoff = FALSE.
 */
void nvEvoStageLUTNotifier(
    NVDispEvoPtr pDispEvo,
    NvU32 apiHead)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    const int dispIndex = pDispEvo->displayOwner;

    nvAssert((pDevEvo->lut.notifierState.sd[dispIndex].stagedApiHeadMask &
             NVBIT(apiHead)) == 0);

    pDevEvo->lut.notifierState.sd[dispIndex].stagedApiHeadMask |=
        NVBIT(apiHead);
}

/*
 * Check if the api head's LUT Notifier is complete.
 */

NvBool nvEvoIsLUTNotifierComplete(
    NVDispEvoPtr pDispEvo,
    NvU32 apiHead)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    const int dispIndex = pDispEvo->displayOwner;

    UpdateLUTNotifierTracking(pDispEvo);

    return (pDevEvo->lut.notifierState.sd[dispIndex].waitingApiHeadMask &
            NVBIT(apiHead)) == 0;
}

/*
 * Wait for the api head's LUT Notifier to complete.
 *
 * This function blocks while waiting for the notifier.
 */

void nvEvoWaitForLUTNotifier(
    const NVDispEvoPtr pDispEvo,
    NvU32 apiHead)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    const int dispIndex = pDispEvo->displayOwner;
    int i;

    if (nvEvoIsLUTNotifierComplete(pDispEvo, apiHead)) {
        return;
    }

    for (i = 0; i < ARRAY_LEN(pDevEvo->lut.notifierState.sd[dispIndex].notifiers); i++) {
        int notifier = pDevEvo->lut.notifierState.sd[dispIndex].notifiers[i].notifier;

        if (!pDevEvo->lut.notifierState.sd[dispIndex].notifiers[i].waiting) {
            continue;
        }

        if ((pDevEvo->lut.notifierState.sd[dispIndex].notifiers[i].apiHeadMask &
            NVBIT(apiHead)) == 0) {

            continue;
        }

        pDevEvo->hal->WaitForCompNotifier(pDispEvo, notifier);
        return;
    }
}

static void EvoIncrementCurrentLutIndex(NVDispEvoRec *pDispEvo,
                                        const NvU32 apiHead,
                                        const NvBool baseLutEnabled,
                                        const NvBool outputLutEnabled)
{
    NvU32 head;
    const int dispIndex = pDispEvo->displayOwner;
    NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    const int numLUTs = ARRAY_LEN(pDevEvo->lut.apiHead[apiHead].LUT);
    NVDispApiHeadStateEvoRec *pApiHeadState =
        &pDispEvo->apiHeadState[apiHead];

    pDevEvo->lut.apiHead[apiHead].disp[dispIndex].curLUTIndex++;
    pDevEvo->lut.apiHead[apiHead].disp[dispIndex].curLUTIndex %= numLUTs;
    pDevEvo->lut.apiHead[apiHead].disp[dispIndex].curBaseLutEnabled = baseLutEnabled;
    pDevEvo->lut.apiHead[apiHead].disp[dispIndex].curOutputLutEnabled = outputLutEnabled;

    FOR_EACH_EVO_HW_HEAD_IN_MASK(pApiHeadState->hwHeadsMask, head) {
        const NvU32 curLutIndex =
            pDevEvo->lut.apiHead[apiHead].disp[dispIndex].curLUTIndex;
        NVDispHeadStateEvoRec *pHeadState = &pDispEvo->headState[head];

        pHeadState->lut.outputLutEnabled =
            pDevEvo->lut.apiHead[apiHead].disp[dispIndex].curOutputLutEnabled;
        pHeadState->lut.baseLutEnabled =
            pDevEvo->lut.apiHead[apiHead].disp[dispIndex].curBaseLutEnabled;
        pHeadState->lut.pCurrSurface =
            pDevEvo->lut.apiHead[apiHead].LUT[curLutIndex];

    }
}

static NvU32 UpdateLUTTimer(NVDispEvoPtr pDispEvo,
                            const NvU32 apiHead,
                            const NvBool baseLutEnabled,
                            const NvBool outputLutEnabled)
{
    if (!nvEvoIsLUTNotifierComplete(pDispEvo, apiHead)) {
        // If the notifier is still pending, then the previous update is still
        // pending and further LUT changes should continue to go into the third
        // buffer.  Reschedule the timer for another 10 ms.
        return 10;
    }

    // Update the current LUT index and kick off an update.
    EvoIncrementCurrentLutIndex(pDispEvo, apiHead, baseLutEnabled,
                                outputLutEnabled);

    EvoUpdateCurrentPalette(pDispEvo, apiHead);

    // Return 0 to cancel the timer.
    return 0;
}

static void UpdateLUTTimerNVKMS(void *dataPtr, NvU32 dataU32)
{
    NVDispEvoPtr pDispEvo = dataPtr;
    const NvU32 apiHead = DRF_VAL(UPDATE_LUT_TIMER_NVKMS, _DATAU32, _HEAD,
                               dataU32);
    const NvBool baseLutEnabled = FLD_TEST_DRF(UPDATE_LUT_TIMER_NVKMS, _DATAU32,
                                               _BASE_LUT, _ENABLE, dataU32);
    const NvBool outputLutEnabled = FLD_TEST_DRF(UPDATE_LUT_TIMER_NVKMS, _DATAU32,
                                                 _OUTPUT_LUT, _ENABLE, dataU32);
    NvU32 ret = UpdateLUTTimer(pDispEvo, apiHead, baseLutEnabled,
                               outputLutEnabled);

    if (ret != 0) {
        ScheduleLutUpdate(pDispEvo, apiHead, dataU32, ret * 1000);
    }
}

static void ScheduleLutUpdate(NVDispEvoRec *pDispEvo,
                              const NvU32 apiHead, const NvU32 data,
                              const NvU64 usec)
{
    NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;

    /* Cancel previous update */
    nvCancelLutUpdateEvo(pDispEvo, apiHead);

    /* schedule a new timer */
    pDevEvo->lut.apiHead[apiHead].disp[pDispEvo->displayOwner].updateTimer =
        nvkms_alloc_timer(UpdateLUTTimerNVKMS,
                          pDispEvo, data,
                          usec);
}

/*
 * The gamma ramp, if specified, has a 16-bit range.  Convert it to EVO's 14-bit
 * shifted range and zero out the low 3 bits for bug 813188.
 */
static inline NvU16 GammaToEvo(NvU16 gamma)
{
    return ((gamma >> 2) & ~7) + 24576;
}

static NVEvoLutDataRec *GetNewLutBuffer(
    const NVDispEvoRec *pDispEvo,
    const struct NvKmsSetLutCommonParams *pParams)
{
    const NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    NVEvoLutDataRec *pLUTBuffer = NULL;

    // XXX NVKMS TODO: If only input or output are specified and the other one
    // is enabled in the hardware, this will zero out the one not specified. In
    // practice it isn't a problem today because the X driver always specifies
    // both, but we should fix this once we start always using the base channel,
    // where we have a separate base LUT ctxdma.
    //
    // This is also a problem if a partial update of the input LUT is attempted
    // (i.e. start != 0 or end != numberOfLutEntries-1).
    //
    // Filed bug: 2042919 to track removing this TODO.

    pLUTBuffer = nvCalloc(1, sizeof(*pLUTBuffer));

    if (pLUTBuffer == NULL) {
        goto done;
    }

    if (pParams->input.specified && pParams->input.end != 0) {
        const struct NvKmsLutRamps *pRamps =
            nvKmsNvU64ToPointer(pParams->input.pRamps);
        const NvU16 *red = pRamps->red;
        const NvU16 *green = pRamps->green;
        const NvU16 *blue = pRamps->blue;

        nvAssert(pRamps != NULL);

        // Update our shadow copy of the LUT.
        pDevEvo->hal->FillLUTSurface(pLUTBuffer->base,
                                     red, green, blue,
                                     pParams->input.end + 1,
                                     pParams->input.depth);
    }

    if (pParams->output.specified && pParams->output.enabled) {
        const struct NvKmsLutRamps *pRamps =
            nvKmsNvU64ToPointer(pParams->output.pRamps);
        int i;

        nvAssert(pRamps != NULL);

        if (pDevEvo->hal->caps.hasUnorm16OLUT) {
            for (i = 0; i < 1024; i++) {
                // Copy the client's 16-bit ramp directly to the LUT buffer.
                pLUTBuffer->output[NV_LUT_VSS_HEADER_SIZE + i].Red = pRamps->red[i];
                pLUTBuffer->output[NV_LUT_VSS_HEADER_SIZE + i].Green = pRamps->green[i];
                pLUTBuffer->output[NV_LUT_VSS_HEADER_SIZE + i].Blue = pRamps->blue[i];
            }

            pLUTBuffer->output[NV_LUT_VSS_HEADER_SIZE + 1024] =
                pLUTBuffer->output[NV_LUT_VSS_HEADER_SIZE + 1023];
        } else {
            for (i = 0; i < 1024; i++) {
                // Convert from the client's 16-bit range to the EVO 14-bit shifted
                // range.
                pLUTBuffer->output[i].Red = GammaToEvo(pRamps->red[i]);
                pLUTBuffer->output[i].Green = GammaToEvo(pRamps->green[i]);
                pLUTBuffer->output[i].Blue = GammaToEvo(pRamps->blue[i]);
            }

            pLUTBuffer->output[1024] = pLUTBuffer->output[1023];
        }
    }

    /* fall through */

done:
    return pLUTBuffer;
}


/*
 * Update the api head's LUT with the given colors.
 *
 * The color LUT is triple-buffered.
 *
 * curLUTIndex indicates the buffer currently being updated.  What the other
 * two buffers are used for depends on whether the previous update has
 * completed.  If not (case 1):
 *   curLUTIndex + 1 (mod 3): currently being displayed
 *   curLUTIndex + 2 (mod 3): will be displayed at next vblank
 * If so (case 2):
 *   curLUTIndex + 1 (mod 3): unused
 *   curLUTIndex + 2 (mod 3): currently being displayed
 *
 * In case 1, just update the current buffer and kick off a timer to submit the
 * update from i+2 to i.  If more LUT changes come in before the first update
 * happens, kill the timer and start a new one.
 *
 * In case 2, kill the timer if it still hasn't gone off, update buffer i, and
 * kick off an update.  No new timer needs to be scheduled.
 */

void nvEvoSetLut(NVDispEvoPtr pDispEvo, NvU32 apiHead, NvBool kickoff,
                 const struct NvKmsSetLutCommonParams *pParams)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    const int dispIndex = pDispEvo->displayOwner;
    const int curLUT = pDevEvo->lut.apiHead[apiHead].disp[dispIndex].curLUTIndex;
    const NvBool waitForPreviousUpdate =
        pDevEvo->lut.apiHead[apiHead].disp[dispIndex].waitForPreviousUpdate;
    const int numLUTs = ARRAY_LEN(pDevEvo->lut.apiHead[apiHead].LUT);
    const int lutToFill = (curLUT + 1) % numLUTs;
    NVLutSurfaceEvoPtr pSurfEvo = pDevEvo->lut.apiHead[apiHead].LUT[lutToFill];
    NvBool baseLutEnabled =
        pDevEvo->lut.apiHead[apiHead].disp[dispIndex].curBaseLutEnabled ;
    NvBool outputLutEnabled =
        pDevEvo->lut.apiHead[apiHead].disp[dispIndex].curOutputLutEnabled;

    if (!pParams->input.specified && !pParams->output.specified) {
        return;
    }

    if (pParams->input.specified) {
        baseLutEnabled = (pParams->input.end != 0);
    }

    if (pParams->output.specified) {
        outputLutEnabled = pParams->output.enabled;
    }

    nvAssert(pSurfEvo != NULL);

    if ((pParams->input.specified && pParams->input.end != 0) ||
        (pParams->output.specified && pParams->output.enabled)) {
        NVEvoLutDataRec *pLUTBuffer = GetNewLutBuffer(pDispEvo, pParams);

        if (pLUTBuffer == NULL) {
            nvEvoLogDev(pDevEvo, EVO_LOG_WARN,
                        "LUT Allocation failure; skipping LUT update");
            return;
        }

        // Fill in the new LUT buffer.
        nvUploadDataToLutSurfaceEvo(pSurfEvo, pLUTBuffer, pDispEvo);

        nvFree(pLUTBuffer);
    }

    /* Kill a pending timer */
    nvCancelLutUpdateEvo(pDispEvo, apiHead);

    if (!kickoff) {
        EvoIncrementCurrentLutIndex(pDispEvo, apiHead, baseLutEnabled,
                                    outputLutEnabled);
        return;
    }

    // See if we can just fill the next LUT buffer and kick off an update now.
    // We can do that if this is the very first update, or if the previous
    // update is complete, or if we need to guarantee that this update
    // is synchronous.
    NvBool previousUpdateComplete =
        nvEvoIsLUTNotifierComplete(pDispEvo, apiHead);
    if (!waitForPreviousUpdate || previousUpdateComplete ||
        pParams->synchronous) {

        if (!previousUpdateComplete) {
            nvEvoWaitForLUTNotifier(pDispEvo, apiHead);
        }

        // Kick off an update now.
        EvoIncrementCurrentLutIndex(pDispEvo, apiHead, baseLutEnabled,
                                    outputLutEnabled);
        EvoUpdateCurrentPalette(pDispEvo, apiHead);

        // If this LUT update is synchronous, then sync before returning.
        if (pParams->synchronous &&
            pDevEvo->lut.apiHead[apiHead].disp[dispIndex].waitForPreviousUpdate) {

            nvEvoWaitForLUTNotifier(pDispEvo, apiHead);
            pDevEvo->lut.apiHead[apiHead].disp[dispIndex].waitForPreviousUpdate =
                FALSE;
        }
    } else {
        // Schedule a timer to kick off an update later.
        // XXX 5 ms is a guess.  We could probably look at this pDpy's refresh
        // rate to come up with a more reasonable estimate.
        NvU32 dataU32 = DRF_NUM(UPDATE_LUT_TIMER_NVKMS, _DATAU32, _HEAD, apiHead);

        nvAssert((apiHead & ~0xff) == 0);

        if (baseLutEnabled) {
            dataU32 |= DRF_DEF(UPDATE_LUT_TIMER_NVKMS, _DATAU32, _BASE_LUT,
                               _ENABLE);
        }

        if (outputLutEnabled) {
            dataU32 |= DRF_DEF(UPDATE_LUT_TIMER_NVKMS, _DATAU32, _OUTPUT_LUT,
                               _ENABLE);
        }

        ScheduleLutUpdate(pDispEvo, apiHead, dataU32, 5 * 1000);
    }
}

NvBool nvValidateSetLutCommonParams(
    const NVDevEvoRec *pDevEvo,
    const struct NvKmsSetLutCommonParams *pParams)
{
    NvU32 maxSize = 0;

    if (pParams->output.specified && pParams->output.enabled) {
        if (pParams->output.pRamps == 0) {
            return FALSE;
        }
    }

    if (!pParams->input.specified || pParams->input.end == 0) {
        return TRUE;
    }

    if (pParams->input.pRamps == 0) {
        return FALSE;
    }

    switch (pParams->input.depth) {
        case 8:  maxSize = 256;  break;
        case 15: maxSize = 32;   break;
        case 16: maxSize = 64;   break;
        case 24: maxSize = 256;  break;
        case 30: maxSize = 1024; break;
        default: return FALSE;
    }

    nvAssert(maxSize <= NVKMS_LUT_ARRAY_SIZE);
    nvAssert(maxSize <= NVKMS_LUT_ARRAY_SIZE);
    nvAssert(maxSize <= NVKMS_LUT_ARRAY_SIZE);

    /* Currently, the implementation assumes start==0. */
    if (pParams->input.start != 0) {
        return FALSE;
    }

    if (pParams->input.end >= maxSize) {
        return FALSE;
    }

    return TRUE;
}

static NvU32 GetSwapLockoutWindowUs(NVDispEvoPtr pDispEvo)
{
    NV30F1_CTRL_GSYNC_GET_CONTROL_SWAP_LOCK_WINDOW_PARAMS params = { 0 };
    NvU32 ret;

    nvAssert(pDispEvo->pFrameLockEvo != NULL);

    ret = nvRmApiControl(
            nvEvoGlobal.clientHandle,
            pDispEvo->pFrameLockEvo->device,
            NV30F1_CTRL_CMD_GSYNC_GET_CONTROL_SWAP_LOCK_WINDOW,
            &params, sizeof(params));

    if (ret != NVOS_STATUS_SUCCESS) {
        nvAssert(!"NV30F1_CTRL_CMD_GSYNC_GET_CONTROL_SWAP_LOCK_WINDOW failed");
    }

    return params.tSwapRdyHi;
}

static NvU32 CalculateSwapLockoutStartP2060(NVDispEvoPtr pDispEvo,
                                            const NvU32 head,
                                            const NvU32 tSwapRdyHiUs)
{
    const NVHwModeTimingsEvo *pTimings;

    nvAssert(head != NV_INVALID_HEAD);
    nvAssert(nvHeadIsActive(pDispEvo, head));

    pTimings = &pDispEvo->headState[head].timings;

    /*
     *  SWAP_LOCKOUT_START = Vtotal * TswapRdyHi * Refresh_Rate
     *
     * = Vtotal * TswapRdyHi * (pclk / Refresh_Rate)
     * = Vtotal * TswapRdyHi * (pclk / (Votal * Htotal))
     * = Vtotal * TswapRdyHi * (pclk / (Votal * Htotal))
     * = TswapRdyHi * (pclk / Htotal)
     * = TswapRdyHiUs * 1e-6 * pclk / Htotal
     * = TswapRdyHiUs * pclk / (Htotal * 1000000)
     * = TswapRdyHiUs * (pclkKhz * 1000) / (Htotal * 1000000)
     * = TswapRdyHiUs * pclkKhz / (Htotal * 1000)
     *
     * Since SWAP_LOCKOUT_START must be higher than LSR_MIN_TIME, round this
     * result up to the nearest integer.
     */

    return NV_ROUNDUP_DIV(tSwapRdyHiUs * pTimings->pixelClock,
                          pTimings->rasterSize.x * 1000);
}

/**
 * Override the swap lockout start value on heads on this pDisp, or restore the
 * default value.
 *
 * This is called before (with isPre == TRUE) and after (with isPre == FALSE)
 * swap barriers are enabled on the G-Sync board.  In order to satisfy certain
 * timing criteria, we need to set a special value for SWAP_LOCKOUT_START for
 * the duration of swap barriers being enabled.
 */
void nvSetSwapBarrierNotifyEvo(NVDispEvoPtr pDispEvo,
                               NvBool enable, NvBool isPre)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NvU32 tSwapRdyHiUs = 0;
    NvU32 head;

    if ((isPre && !enable) || (!isPre && enable)) {
        return;
    }

    if (enable) {
        tSwapRdyHiUs = GetSwapLockoutWindowUs(pDispEvo);
    }

    for (head = 0; head < NVKMS_MAX_HEADS_PER_DISP; head++) {
        NV5070_CTRL_CMD_SET_RG_FLIPLOCK_PROP_PARAMS params = { };
        NvU32 ret;

        if (!nvHeadIsActive(pDispEvo, head)) {
            continue;
        }

        params.maxSwapLockoutSkew =
            NV5070_CTRL_CMD_SET_RG_FLIPLOCK_PROP_MAX_SWAP_LOCKOUT_SKEW_INIT;

        if (enable) {
            params.swapLockoutStart =
                CalculateSwapLockoutStartP2060(pDispEvo, head, tSwapRdyHiUs);
        } else {
            params.swapLockoutStart =
                NV5070_CTRL_CMD_SET_RG_FLIPLOCK_PROP_SWAP_LOCKOUT_START_INIT;
        }

        params.head = head;

        params.base.subdeviceIndex = pDispEvo->displayOwner;

        ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                             pDevEvo->displayHandle,
                             NV5070_CTRL_CMD_SET_RG_FLIPLOCK_PROP,
                             &params,
                             sizeof(params));

        if (ret != NVOS_STATUS_SUCCESS) {
            nvAssert(!"NV5070_CTRL_CMD_SET_RG_FLIPLOCK_PROP failed");
        }
    }
}

/*!
 * Release a reference to a pDevEvo
 *
 * If the refcount of the device drops to 0, this frees the device.
 *
 * \return TRUE if the device was freed, FALSE otherwise.
 */
NvBool nvFreeDevEvo(NVDevEvoPtr pDevEvo)
{
    if (pDevEvo == NULL) {
        return FALSE;
    }

    pDevEvo->allocRefCnt--;

    if (pDevEvo->allocRefCnt > 0) {
        return FALSE;
    }

    if (pDevEvo->pDifrState) {
        nvRmUnregisterDIFREventHandler(pDevEvo);
        nvDIFRFree(pDevEvo->pDifrState);
        pDevEvo->pDifrState = NULL;
    }

    if (pDevEvo->pNvKmsOpenDev != NULL) {
        /*
         * DP-MST allows to attach more than one heads/stream to single DP
         * connector, and there is no way to convey that DP-MST configuration to
         * next driver load; therefore disallow DP-MST.
         */
        nvEvoRestoreConsole(pDevEvo, FALSE /* allowMST */);

        nvEvoUnregisterSurface(pDevEvo, pDevEvo->pNvKmsOpenDev,
                               pDevEvo->fbConsoleSurfaceHandle,
                               TRUE /* skipUpdate */);
        pDevEvo->fbConsoleSurfaceHandle = 0;
    }

    nvFreeCoreChannelEvo(pDevEvo);

    nvTeardownHdmiLibrary(pDevEvo);

    nvHsFreeDevice(pDevEvo);

    nvFreePerOpenDev(nvEvoGlobal.nvKmsPerOpen, pDevEvo->pNvKmsOpenDev);

    nvFreeFrameLocksEvo(pDevEvo);

    if (pDevEvo->hal) {
        pDevEvo->hal->FreeRmCtrlObject(pDevEvo);
    }

    nvRmDestroyDisplays(pDevEvo);

    nvkms_free_timer(pDevEvo->consoleRestoreTimer);
    pDevEvo->consoleRestoreTimer = NULL;

    nvPreallocFree(pDevEvo);

    nvRmFreeDeviceEvo(pDevEvo);

    nvListDel(&pDevEvo->devListEntry);

    nvkms_free_ref_ptr(pDevEvo->ref_ptr);

    nvFree(pDevEvo);
    return TRUE;
}

static void AssignNumberOfApiHeads(NVDevEvoRec *pDevEvo)
{
    pDevEvo->numApiHeads = pDevEvo->numHeads;
}

NVDevEvoPtr nvAllocDevEvo(const struct NvKmsAllocDeviceRequest *pRequest,
                          enum NvKmsAllocDeviceStatus *pStatus)
{
    NVDevEvoPtr pDevEvo = NULL;
    enum NvKmsAllocDeviceStatus status =
        NVKMS_ALLOC_DEVICE_STATUS_NO_HARDWARE_AVAILABLE;
    NvU32 i;

    nvAssert(nvFindDevEvoByDeviceId(pRequest->deviceId) == NULL);

    pDevEvo = nvCalloc(1, sizeof(*pDevEvo));

    if (pDevEvo == NULL) {
        goto done;
    }

    pDevEvo->allocRefCnt = 1;

    pDevEvo->gpuLogIndex = NV_INVALID_GPU_LOG_INDEX;

    pDevEvo->gc6Allowed = TRUE;

    nvListAppend(&pDevEvo->devListEntry, &nvEvoGlobal.devList);

    pDevEvo->ref_ptr = nvkms_alloc_ref_ptr(pDevEvo);
    if (!pDevEvo->ref_ptr) {
        goto done;
    }

    for (i = 0; i < ARRAY_LEN(pDevEvo->openedGpuIds); i++) {
        pDevEvo->openedGpuIds[i] = NV0000_CTRL_GPU_INVALID_ID;
    }

    for (i = 0; i < ARRAY_LEN(pDevEvo->headForWindow); i++) {
        pDevEvo->headForWindow[i] = NV_INVALID_HEAD;
    }

    if (!nvRmAllocDeviceEvo(pDevEvo, pRequest)) {
        goto done;
    }

    status = nvAssignEvoCaps(pDevEvo);

    if (status != NVKMS_ALLOC_DEVICE_STATUS_SUCCESS) {
        goto done;
    }

    if (!nvPreallocAlloc(pDevEvo)) {
        status = NVKMS_ALLOC_DEVICE_STATUS_FATAL_ERROR;
        goto done;
    }

    /*
     * Copy the registry keys from the alloc device request to the device.
     *
     * This needs to be set before nvRmAllocDisplays, because nvRmAllocDisplays
     * will initialize DP lib which may read registry keys that we want to
     * allow clients to override.
     */
    ct_assert(ARRAY_LEN(pRequest->registryKeys) ==
              ARRAY_LEN(pDevEvo->registryKeys));
    ct_assert(ARRAY_LEN(pRequest->registryKeys[0].name) ==
              ARRAY_LEN(pDevEvo->registryKeys[0].name));

    for (i = 0; i < ARRAY_LEN(pRequest->registryKeys); i++) {
        const size_t nameLen = sizeof(pDevEvo->registryKeys[i].name);
        nvkms_memcpy(pDevEvo->registryKeys[i].name,
                     pRequest->registryKeys[i].name,
                     nameLen);
        pDevEvo->registryKeys[i].name[nameLen - 1] = '\0';
        pDevEvo->registryKeys[i].value = pRequest->registryKeys[i].value;
    }

    status = nvRmAllocDisplays(pDevEvo);

    if (status != NVKMS_ALLOC_DEVICE_STATUS_SUCCESS) {
        goto done;
    }

    nvAllocFrameLocksEvo(pDevEvo);

    if (!pDevEvo->hal->AllocRmCtrlObject(pDevEvo)) {
        status = NVKMS_ALLOC_DEVICE_STATUS_FATAL_ERROR;
        goto done;
    }

    AssignNumberOfApiHeads(pDevEvo);

    if (!nvAllocCoreChannelEvo(pDevEvo)) {
        status = NVKMS_ALLOC_DEVICE_STATUS_CORE_CHANNEL_ALLOC_FAILED;
        goto done;
    }

    pDevEvo->pNvKmsOpenDev = nvAllocPerOpenDev(nvEvoGlobal.nvKmsPerOpen,
                                               pDevEvo, TRUE /* isPrivileged */);
    if (!pDevEvo->pNvKmsOpenDev) {
        status = NVKMS_ALLOC_DEVICE_STATUS_FATAL_ERROR;
        goto done;
    }

    nvDPSetAllowMultiStreaming(pDevEvo, TRUE /* allowMST */);

    /*
     * Import the framebuffer console, if there is one,
     * as a surface we can flip to.
     */
    nvRmImportFbConsoleMemory(pDevEvo);

    /*
     * This check must be placed after nvAllocCoreChannelEvo() since it depends
     * on the HW capabilities that are read in that function.
     */
    if (!ValidateConnectorTypes(pDevEvo)) {
        status = NVKMS_ALLOC_DEVICE_STATUS_FATAL_ERROR;
        goto done;
    }

    if (!nvHsAllocDevice(pDevEvo, pRequest)) {
        status = NVKMS_ALLOC_DEVICE_STATUS_FATAL_ERROR;
        goto done;
    }

    if (!nvInitHdmiLibrary(pDevEvo)) {
        status = NVKMS_ALLOC_DEVICE_STATUS_FATAL_ERROR;
        goto done;
    }

    nvRmMuxInit(pDevEvo);

    status = NVKMS_ALLOC_DEVICE_STATUS_SUCCESS;

    /*
     * We can't allocate DIFR state if h/w doesn't support it. Only register
     * event handlers with DIFR state.
     */
    pDevEvo->pDifrState = nvDIFRAllocate(pDevEvo);
    if (pDevEvo->pDifrState) {
        if (!nvRmRegisterDIFREventHandler(pDevEvo)) {
            nvDIFRFree(pDevEvo->pDifrState);
            pDevEvo->pDifrState = NULL;
        }
    }

    /* fall through */

done:
    if (status != NVKMS_ALLOC_DEVICE_STATUS_SUCCESS) {
        nvFreeDevEvo(pDevEvo);
        pDevEvo = NULL;
    }

    *pStatus = status;

    return pDevEvo;
}


// How long before we time out waiting for lock?
// In microseconds.
#define LOCK_TIMEOUT 5000000

//
// EvoWaitForLock()
// Wait for raster or flip lock to complete
// Note that we use pDev and subdevice here instead of pDisp since this is used
// per-subdev in SLI (including the pDispEvo->numSubDevices > 1 case).
//
static NvBool EvoWaitForLock(const NVDevEvoRec *pDevEvo, const NvU32 sd,
                             const NvU32 head, const NvU32 type,
                             NvU64 *pStartTime)
{
    NV5070_CTRL_CMD_GET_RG_STATUS_PARAMS status = { };
    NvU32 ret;

    nvAssert(type == EVO_RASTER_LOCK || type == EVO_FLIP_LOCK);

    if ((type == EVO_FLIP_LOCK) &&
        !pDevEvo->hal->caps.supportsFlipLockRGStatus) {
        return TRUE;
    }

    status.head             = head;
    status.base.subdeviceIndex = sd;
    status.scanLocked       = NV5070_CTRL_CMD_GET_RG_STATUS_SCANLOCKED_NO;
    status.flipLocked       = NV5070_CTRL_CMD_GET_RG_STATUS_FLIPLOCKED_NO;

    // Just keep looping until we get what we want.
    do {
        ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                             pDevEvo->displayHandle,
                             NV5070_CTRL_CMD_GET_RG_STATUS,
                             &status,
                             sizeof(status));
        if (ret != NVOS_STATUS_SUCCESS) {
            nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                        "Unable to read SLI lock status");
            return FALSE;
        }

        if ((type == EVO_RASTER_LOCK) &&
            (status.scanLocked ==
                NV5070_CTRL_CMD_GET_RG_STATUS_SCANLOCKED_YES)) {
            break;
        }
        if ((type == EVO_FLIP_LOCK) &&
            (status.flipLocked ==
                NV5070_CTRL_CMD_GET_RG_STATUS_FLIPLOCKED_YES)) {
            break;
        }

        if (nvExceedsTimeoutUSec(pDevEvo, pStartTime, LOCK_TIMEOUT)) {
            nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                        "SLI lock timeout exceeded (type %d)", type);
            return FALSE;
        }

        nvkms_yield();

    } while (TRUE);

    // Once we've exited from the various loops above, we should be locked
    // as requested.
    return TRUE;
}

//
// EvoUpdateHeadParams()
// Send GPUs HeadParams updates; accounts for SLI.
//
static void EvoUpdateHeadParams(const NVDispEvoRec *pDispEvo, NvU32 head,
                                NVEvoUpdateState *updateState)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;

    nvPushEvoSubDevMaskDisp(pDispEvo);

    pDevEvo->hal->SetHeadControl(pDevEvo, pDispEvo->displayOwner, head, updateState);

    nvPopEvoSubDevMask(pDevEvo);
}

//
// nvReadCRC32Evo()
// Returns the last CRC32 value
NvBool nvReadCRC32Evo(NVDispEvoPtr pDispEvo, NvU32 head,
                      CRC32NotifierCrcOut *crcOut)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    const NVDispHeadStateEvoRec *pHeadState = &pDispEvo->headState[head];
    const NVHwModeTimingsEvo *pTimings = &pHeadState->timings;
    NVEvoDmaPtr dma = NULL;
    NVConnectorEvoPtr pConnectorEvo = NULL;
    NVEvoUpdateState updateState = { };
    NvU32 numCRC32 = 0;
    NvBool res = TRUE;
    NvBool found = FALSE;
    NvU32 ret;

    // Look up the head connector
    nvListForEachEntry(pConnectorEvo,
                       &pDispEvo->connectorList,
                       connectorListEntry) {
        NvU32 activeHeadMask =
            nvConnectorGetAttachedHeadMaskEvo(pConnectorEvo);
        if (activeHeadMask & NVBIT(head)) {
            found = TRUE;
            break;
        }
    }

    if (!found) {
        nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                    "Unable to find active connector for head %d", head);
        return FALSE;
    }

    // Allocate a temporary DMA notifier
    dma = nvCalloc(1, sizeof(NVEvoDma));
    if ((dma == NULL) ||
        !nvRmAllocEvoDma(pDevEvo,
                         dma,
                         NV_DMA_EVO_NOTIFIER_SIZE - 1,
                         DRF_DEF(OS03, _FLAGS, _TYPE, _NOTIFIER),
                         1 << pDispEvo->displayOwner)) {
        nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                    "CRC32 notifier DMA allocation failed");
        nvFree(dma);
        return FALSE;
    }

    // Bind the CRC32 notifier surface descriptor
    ret = pDevEvo->hal->BindSurfaceDescriptor(pDevEvo, pDevEvo->core, &dma->surfaceDesc);
    if (ret != NVOS_STATUS_SUCCESS) {
        nvEvoLogDevDebug(pDevEvo, EVO_LOG_ERROR,
                         "Failed to bind display engine CRC32 notify surface descriptor "
                         ": 0x%x (%s)", ret, nvstatusToString(ret));
        res = FALSE;
        goto done;
    }

    // Only set up the actual output for SLI primary.
    nvPushEvoSubDevMask(pDevEvo, 1 << pDispEvo->displayOwner);

    /* CRC notifiers are similar to completion notifiers, but work slightly
     * different:
     *
     *   1. In order to start CRC generation for a head, we need to:
     *
     *      - Point an EVO head at a block of memory with
     *        HEAD_SET_CONTEXT_DMA_CRC(head)
     *
     *      - Program the CRC control with HEAD_SET_CRC_CONTROL(head) to select
     *        what output we want to capture CRC values from, and kicking off a
     *        core channel update (this already generates a CRC value for the
     *        last scanout buffer)
     *
     *      ----> hal->StartCRC32Capture()
     *
     *   2. From 1) on, a new CRC value is generated per vblank and written to
     *      an incrementing entry in the CRC notifier. With pre-nvdisplay chips,
     *      a CRC notifier can hold up to 256 entries. Once filled up, new CRC
     *      values are discarded. Either case, we are only interested in the
     *      last CRC32 value.
     *
     *   3. In order to stop CRC generation, we need to perform the inverse
     *      operation of 1):
     *
     *      - Program the CRC control with HEAD_SET_CRC_CONTROL(head) to
     *        unselect all outputs we were capturing CRC values from.
     *
     *      - Unset the CRC context DMA with HEAD_SET_CONTEXT_DMA_CRC(head)
     *
     *      ----> hal->StopCRC32Capture()
     *
     *   4. From 3) on, it is safe to wait for the CRC notifier and query all
     *      entries.
     *
     *      ----> hal->QueryCRC32()
     */
    pDevEvo->hal->StartCRC32Capture(pDevEvo,
                                    dma,
                                    pConnectorEvo,
                                    pTimings->protocol,
                                    pConnectorEvo->or.primary,
                                    head,
                                    pDispEvo->displayOwner,
                                    &updateState);

    // This update should generate one CRC value.
    nvEvoUpdateAndKickOff(pDispEvo, TRUE, &updateState, TRUE /* releaseElv */);

    pDevEvo->hal->StopCRC32Capture(pDevEvo,
                                   head,
                                   &updateState);

    nvEvoUpdateAndKickOff(pDispEvo, TRUE, &updateState, TRUE /* releaseElv */);

    if (!pDevEvo->hal->QueryCRC32(pDevEvo,
                                  dma,
                                  pDispEvo->displayOwner,
                                  1,
                                  crcOut,
                                  &numCRC32) ||
        (numCRC32 == 0)) {
        nvEvoLogDev(pDevEvo, EVO_LOG_ERROR, "Failed to query all CRC32 values");
    }

    nvPopEvoSubDevMask(pDevEvo);

done:
    // Clean-up
    nvRmFreeEvoDma(pDevEvo, dma);
    nvFree(dma);

    return res;
}

NvU32 nvGetActiveSorMask(const NVDispEvoRec *pDispEvo)
{
    NvU32 activeSorMask = 0;
    NvU32 head;

    for (head = 0; head < pDispEvo->pDevEvo->numHeads; head++) {
        NVConnectorEvoPtr pConnectorEvo =
            pDispEvo->headState[head].pConnectorEvo;

        if (pConnectorEvo != NULL &&
            pConnectorEvo->or.type == NV0073_CTRL_SPECIFIC_OR_TYPE_SOR) {
            NvU32 orIndex;
            nvAssert(pConnectorEvo->or.primary != NV_INVALID_OR);
            FOR_EACH_INDEX_IN_MASK(32, orIndex, nvConnectorGetORMaskEvo(pConnectorEvo)) {
                if (pConnectorEvo->or.ownerHeadMask[orIndex] == 0x0) {
                    continue;
                }
                activeSorMask |= NVBIT(orIndex);
            } FOR_EACH_INDEX_IN_MASK_END;
        }
    }

    return activeSorMask;
}

NvBool nvEvoPollForNoMethodPending(NVDevEvoPtr pDevEvo,
                                   const NvU32 sd,
                                   NVEvoChannelPtr pChannel,
                                   NvU64 *pStartTime,
                                   const NvU32 timeout)
{
    do
    {
        NvBool isMethodPending = TRUE;

        if (pDevEvo->hal->IsChannelMethodPending(
                                    pDevEvo,
                                    pChannel,
                                    sd,
                                    &isMethodPending) && !isMethodPending) {
            break;
        }

        if (nvExceedsTimeoutUSec(pDevEvo, pStartTime, timeout)) {
            return FALSE;
        }

        nvkms_yield();
   } while (TRUE);

    return TRUE;
}

static NvU32 SetSORFlushMode(NVDevEvoPtr pDevEvo,
                             NvU32 sorNumber,
                             NvU32 headMask,
                             NvBool enable)
{
    NV5070_CTRL_SET_SOR_FLUSH_MODE_PARAMS params = { };

    params.base.subdeviceIndex = 0;
    params.sorNumber = sorNumber;
    params.headMask = headMask;
    params.bEnable = enable;

    return nvRmApiControl(nvEvoGlobal.clientHandle,
                          pDevEvo->displayHandle,
                          NV5070_CTRL_CMD_SET_SOR_FLUSH_MODE,
                          &params, sizeof(params));
}

static void DPSerializerLinkTrain(NVDispEvoPtr pDispEvo,
                                  NVConnectorEvoPtr pConnectorEvo,
                                  NvBool enableLink,
                                  NvBool reTrain)
{
    const NvU32 displayId = nvDpyIdToNvU32(pConnectorEvo->displayId);
    const NvU32 sorNumber = pConnectorEvo->or.primary;
    const NvU32 headMask = nvConnectorGetAttachedHeadMaskEvo(pConnectorEvo);
    NvBool force = NV_FALSE;
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;

    /*
     * The NV0073_CTRL_DP_DATA_SET_{LANE_COUNT, LINK_BW} defines are the same
     * as the actual DPCD values. As such, we can directly assign the
     * dpSerializerCaps here.
     */
    NvBool isMST = pConnectorEvo->dpSerializerCaps.supportsMST;
    NvU32 linkBW = pConnectorEvo->dpSerializerCaps.maxLinkBW;
    NvU32 laneCount = pConnectorEvo->dpSerializerCaps.maxLaneCount;

    nvAssert(nvConnectorIsDPSerializer(pConnectorEvo));

    if (sorNumber == NV_INVALID_OR) {
        return;
    }

    if (reTrain) {
        if (!pConnectorEvo->dpSerializerEnabled) {
            nvEvoLogDev(pDevEvo, EVO_LOG_INFO,
                        "Received expected HPD_IRQ during serializer shutdown");
            return;
        }
    } else if (enableLink) {
        pConnectorEvo->dpSerializerEnabled = NV_TRUE;
    } else {
        linkBW = 0;
        laneCount = NV0073_CTRL_DP_DATA_SET_LANE_COUNT_0;
        pConnectorEvo->dpSerializerEnabled = NV_FALSE;
    }

    if (isMST) {
        NvU32 dpcdData = 0;

        dpcdData = FLD_SET_DRF(_DPCD, _MSTM_CTRL, _EN, _YES, dpcdData);
        dpcdData =
            FLD_SET_DRF(_DPCD, _MSTM_CTRL, _UPSTREAM_IS_SRC, _YES, dpcdData);
        if (!nvWriteDPCDReg(pConnectorEvo, NV_DPCD_MSTM_CTRL, dpcdData)) {
            nvEvoLogDev(pDevEvo, EVO_LOG_ERROR, "Failed to enable MST DPCD");
            return;
        }
    }

    /*
     * We cannot perform link training while the OR has an attached head
     * since we would be changing the OR clocks and link frequency while
     * it's actively encoding pixels, and this could lead to FIFO overflow/
     * underflow issues. Instead, the recommended, safe sequence is to enter
     * flush mode first, re-train the link, and exit flush mode after.
     */
    if (reTrain) {
        if (SetSORFlushMode(pDevEvo, sorNumber, headMask, NV_TRUE) !=
            NVOS_STATUS_SUCCESS) {
            nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                        "Failed to enter flush mode");
            return;
        }
    }

    do {
        NvU32 dpCtrlData = 0;
        NvU32 dpCtrlCmd = 0;
        NV0073_CTRL_DP_CTRL_PARAMS dpCtrlParams = { };

        dpCtrlCmd = DRF_DEF(0073_CTRL, _DP_CMD, _SET_LANE_COUNT, _TRUE) |
                    DRF_DEF(0073_CTRL, _DP_CMD, _SET_LINK_BW, _TRUE) |
                    DRF_DEF(0073_CTRL, _DP_CMD, _SET_ENHANCED_FRAMING, _TRUE);

        if (isMST) {
            dpCtrlCmd |= DRF_DEF(0073_CTRL, _DP_CMD, _SET_FORMAT_MODE, _MULTI_STREAM);
        }

        if (force) {
            dpCtrlCmd |= DRF_DEF(0073_CTRL, _DP_CMD, _FAKE_LINK_TRAINING, _DONOT_TOGGLE_TRANSMISSION);
        }

        dpCtrlData = FLD_SET_DRF_NUM(0073_CTRL, _DP_DATA, _SET_LINK_BW,
                                     linkBW, dpCtrlData);
        dpCtrlData = FLD_SET_DRF_NUM(0073_CTRL, _DP_DATA, _SET_LANE_COUNT,
                                     laneCount, dpCtrlData);
        dpCtrlData = FLD_SET_DRF_NUM(0073_CTRL, _DP_DATA, _TARGET,
                                     NV0073_CTRL_DP_DATA_TARGET_SINK,
                                     dpCtrlData);

        dpCtrlParams.subDeviceInstance = pDispEvo->displayOwner;
        dpCtrlParams.displayId = displayId;
        dpCtrlParams.cmd = dpCtrlCmd;
        dpCtrlParams.data = dpCtrlData;

        if (nvRmApiControl(nvEvoGlobal.clientHandle,
                           pDevEvo->displayCommonHandle,
                           NV0073_CTRL_CMD_DP_CTRL,
                           &dpCtrlParams, sizeof(dpCtrlParams)) == NVOS_STATUS_SUCCESS) {
            break;
        }

        if (force) {
            nvEvoLogDev(pDevEvo, EVO_LOG_ERROR, "Fake link training failed");
            break;
        }

        nvEvoLogDev(pDevEvo, EVO_LOG_ERROR, "Link training failed");

        /*
         * XXX Force the link config on the GPU side to avoid hanging the display
         * pipe during modeset. Eventually, we need to figure out how to deal
         * with/report these kinds of LT failures.
         */
        force = NV_TRUE;

    } while (NV_TRUE);

    if (reTrain) {
        if (SetSORFlushMode(pDevEvo, sorNumber, headMask, NV_FALSE) !=
            NVOS_STATUS_SUCCESS) {
            nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                        "Failed to exit flush mode");
        }
    }
}

void nvDPSerializerHandleDPIRQ(NVDispEvoPtr pDispEvo,
                               NVConnectorEvoPtr pConnectorEvo)
{
    DPSerializerLinkTrain(pDispEvo, pConnectorEvo,
                          NV_TRUE /* enableLink */,
                          NV_TRUE /* reTrain */);
}

void nvDPSerializerPreSetMode(NVDispEvoPtr pDispEvo,
                              NVConnectorEvoPtr pConnectorEvo)
{
    const NvU32 headMask = nvConnectorGetAttachedHeadMaskEvo(pConnectorEvo);

    if (!pConnectorEvo->dpSerializerEnabled && (headMask != 0)) {
        DPSerializerLinkTrain(pDispEvo, pConnectorEvo,
                              NV_TRUE /* enableLink */,
                              NV_FALSE /* reTrain */);
    }
}

void nvDPSerializerPostSetMode(NVDispEvoPtr pDispEvo,
                               NVConnectorEvoPtr pConnectorEvo)
{
    const NvU32 headMask = nvConnectorGetAttachedHeadMaskEvo(pConnectorEvo);

    if (pConnectorEvo->dpSerializerEnabled && (headMask == 0)) {
        DPSerializerLinkTrain(pDispEvo, pConnectorEvo,
                              NV_FALSE /* enableLink */,
                              NV_FALSE /* reTrain */);
    }
}

NvU32 nvGetHDRSrcMaxLum(const NVFlipChannelEvoHwState *pHwState)
{
    if (!pHwState->hdrStaticMetadata.enabled) {
        return 0;
    }

    if (pHwState->hdrStaticMetadata.val.maxCLL > 0) {
        return pHwState->hdrStaticMetadata.val.maxCLL;
    }

    return pHwState->hdrStaticMetadata.val.maxDisplayMasteringLuminance;
}

NvBool nvNeedsTmoLut(NVDevEvoPtr pDevEvo,
                     NVEvoChannelPtr pChannel,
                     const NVFlipChannelEvoHwState *pHwState,
                     NvU32 srcMaxLum,
                     NvU32 targetMaxCLL)
{
    const NvU32 win = NV_EVO_CHANNEL_MASK_WINDOW_NUMBER(pChannel->channelMask);
    const NvU32 head = pDevEvo->headForWindow[win];
    const NvU32 sdMask = nvPeekEvoSubDevMask(pDevEvo);
    const NvU32 sd = (sdMask == 0) ? 0 : nv_ffs(sdMask) - 1;
    const NVDispHeadStateEvoRec *pHeadState =
        &pDevEvo->pDispEvo[sd]->headState[head];
    const NVEvoWindowCaps *pWinCaps =
        &pDevEvo->gpus[sd].capabilities.window[pChannel->instance];

    // Don't tone map if flipped to NULL.
    if (!pHwState->pSurfaceEvo[NVKMS_LEFT]) {
        return FALSE;
    }

    // Don't tone map if layer doesn't have static metadata.
    // XXX HDR TODO: Support tone mapping SDR surfaces to HDR
    if (!pHwState->hdrStaticMetadata.enabled) {
        return FALSE;
    }

    // Don't tone map if HDR infoframe isn't enabled
    // XXX HDR TODO: Support tone mapping HDR surfaces to SDR
    if (pHeadState->hdrInfoFrame.state != NVKMS_HDR_INFOFRAME_STATE_ENABLED) {
        return FALSE;
    }

    // Don't tone map if TMO not present
    if (!pWinCaps->tmoPresent) {
        return FALSE;
    }

    // Don't tone map if source or target max luminance is unspecified.
    if ((srcMaxLum == 0) || (targetMaxCLL == 0)) {
        return FALSE;
    }

    // Don't tone map unless source max luminance exceeds target by 10%.
    if (srcMaxLum <= ((targetMaxCLL * 110) / 100)) {
        return FALSE;
    }

    return TRUE;
}

NvBool nvIsCscMatrixIdentity(const struct NvKmsCscMatrix *matrix)
{
    const struct NvKmsCscMatrix identity = NVKMS_IDENTITY_CSC_MATRIX;

    int y;
    for (y = 0; y < 3; y++) {
        int x;

        for (x = 0; x < 4; x++) {
            if (matrix->m[y][x] != identity.m[y][x]) {
                return FALSE;
            }
        }
    }

    return TRUE;
}

enum nvKmsPixelDepth nvEvoDpyColorToPixelDepth(
    const NVDpyAttributeColor *pDpyColor)
{
    switch (pDpyColor->format) {
        case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_RGB:
        case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr444:
        case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr420:
            switch (pDpyColor->bpc) {
                case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_10:
                    return NVKMS_PIXEL_DEPTH_30_444;
                case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_8:
                    return NVKMS_PIXEL_DEPTH_24_444;
                case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_UNKNOWN: /* fallthrough */
                case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_6:
                    return NVKMS_PIXEL_DEPTH_18_444;
            }
            break;
        case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_SPACE_YCbCr422:
            nvAssert(pDpyColor->bpc != NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_6);
            switch (pDpyColor->bpc) {
                case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_10:
                    return NVKMS_PIXEL_DEPTH_20_422;
                case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_6: /* fallthrough */
                case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_UNKNOWN: /* fallthrough */
                case NV_KMS_DPY_ATTRIBUTE_CURRENT_COLOR_BPC_8:
                    return NVKMS_PIXEL_DEPTH_16_422;
            }
            break;
    }

    return NVKMS_PIXEL_DEPTH_18_444;
}

void nvEvoEnableMergeModePreModeset(NVDispEvoRec *pDispEvo,
                                    const NvU32 headsMask,
                                    NVEvoUpdateState *pUpdateState)
{
    const NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    const NvU32 sd = pDispEvo->displayOwner;
    const NvU32 primaryHead = nvGetPrimaryHwHeadFromMask(headsMask);
    NvU32 head;

    nvAssert(pDevEvo->hal->caps.supportsMergeMode);
    nvAssert((nvPopCount32(headsMask) > 1) &&
                (primaryHead != NV_INVALID_HEAD));

    FOR_EACH_EVO_HW_HEAD_IN_MASK(headsMask, head) {
        NVDispHeadStateEvoRec *pHeadState = &pDispEvo->headState[head];
        const NVHwModeTimingsEvo *pTimings = &pHeadState->timings;
        NVEvoHeadControl *pHC =
            &pDevEvo->gpus[sd].headControl[head];

        nvAssert(pHeadState->mergeMode ==
                    NV_EVO_MERGE_MODE_DISABLED);

        /*
         * Heads requires to be raster locked before they transition to
         * PRIMARY/SECONDARY merge mode.
         *
         * SETUP should be the intermediate state before head transition to
         * PRIMARY/SECONDARY  merge mode. During SETUP state, there is no pixel
         * transmission from secondary to primary head, RG fetches and drops
         * pixels, viewport gets filled by the special gray/black pixels.
         */
        pHeadState->mergeMode = NV_EVO_MERGE_MODE_SETUP;
        pDevEvo->hal->SetMergeMode(pDispEvo, head, pHeadState->mergeMode,
                                   pUpdateState);

        nvAssert((pHC->serverLock == NV_EVO_NO_LOCK) &&
                    (pHC->clientLock == NV_EVO_NO_LOCK));

        pHC->mergeMode = TRUE;
        if (head == primaryHead) {
            pHC->serverLock = NV_EVO_RASTER_LOCK;
            pHC->serverLockPin = NV_EVO_LOCK_PIN_INTERNAL(primaryHead);
            pHC->setLockOffsetX = TRUE;
            pHC->crashLockUnstallMode = FALSE;
        } else {
            pHC->clientLock = NV_EVO_RASTER_LOCK;
            pHC->clientLockPin = NV_EVO_LOCK_PIN_INTERNAL(primaryHead);
            if (pTimings->vrr.type != NVKMS_DPY_VRR_TYPE_NONE) {
                pHC->clientLockoutWindow = 4;
                pHC->useStallLockPin = TRUE;
                pHC->stallLockPin = NV_EVO_LOCK_PIN_INTERNAL(primaryHead);
            } else {
                pHC->clientLockoutWindow = 2;
            }
            pHC->crashLockUnstallMode =
                (pTimings->vrr.type != NVKMS_DPY_VRR_TYPE_NONE);
        }

        pHC->stereoLocked = FALSE;

        EvoUpdateHeadParams(pDispEvo, head, pUpdateState);
    }
}

void nvEvoEnableMergeModePostModeset(NVDispEvoRec *pDispEvo,
                                     const NvU32 headsMask,
                                     NVEvoUpdateState *pUpdateState)
{
    const NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    const NvU32 sd = pDispEvo->displayOwner;
    const NvU32 primaryHead = nvGetPrimaryHwHeadFromMask(headsMask);
    NvU64 startTime = 0;
    NvU32 head;

    nvAssert(pDevEvo->hal->caps.supportsMergeMode);
    nvAssert((nvPopCount32(headsMask) > 1) &&
                (primaryHead != NV_INVALID_HEAD));

    FOR_EACH_EVO_HW_HEAD_IN_MASK(headsMask, head) {
        nvAssert(pDispEvo->headState[head].mergeMode ==
                    NV_EVO_MERGE_MODE_SETUP);

        if (!EvoWaitForLock(pDevEvo, sd, head, EVO_RASTER_LOCK,
                            &startTime)) {
            nvEvoLogDispDebug(pDispEvo, EVO_LOG_ERROR, "Raster lock timeout");
            return;
        }
    }

    FOR_EACH_EVO_HW_HEAD_IN_MASK(headsMask, head) {
        NVDispHeadStateEvoRec *pHeadState = &pDispEvo->headState[head];
        NVEvoHeadControl *pHC = &pDevEvo->gpus[sd].headControl[head];

        pHC->flipLockPin = NV_EVO_LOCK_PIN_INTERNAL(primaryHead);
        pHC->flipLock = TRUE;

        EvoUpdateHeadParams(pDispEvo, head, pUpdateState);

        pHeadState->mergeMode = (head == primaryHead) ?
            NV_EVO_MERGE_MODE_PRIMARY : NV_EVO_MERGE_MODE_SECONDARY;
        pDevEvo->hal->SetMergeMode(pDispEvo, head, pHeadState->mergeMode,
                                   pUpdateState);
    }
}

void nvEvoDisableMergeMode(NVDispEvoRec *pDispEvo,
                           const NvU32 headsMask,
                           NVEvoUpdateState *pUpdateState)
{
    const NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    const NvU32 sd = pDispEvo->displayOwner;
    NvU32 head;

    nvAssert(pDevEvo->hal->caps.supportsMergeMode);
    nvAssert(nvPopCount32(headsMask) > 1);

    FOR_EACH_EVO_HW_HEAD_IN_MASK(headsMask, head) {
        NVDispHeadStateEvoRec *pHeadState = &pDispEvo->headState[head];
        NVEvoHeadControl *pHC =
            &pDevEvo->gpus[sd].headControl[head];

        pHeadState->mergeMode = NV_EVO_MERGE_MODE_DISABLED;
        pDevEvo->hal->SetMergeMode(pDispEvo, head, pHeadState->mergeMode,
                                   pUpdateState);

        pHC->mergeMode = FALSE;
        pHC->serverLock = NV_EVO_NO_LOCK;
        pHC->serverLockPin = NV_EVO_LOCK_PIN_INTERNAL(0);
        pHC->clientLock = NV_EVO_NO_LOCK;
        pHC->clientLockPin = NV_EVO_LOCK_PIN_INTERNAL(0);
        pHC->clientLockoutWindow = 0;
        pHC->setLockOffsetX = FALSE;
        pHC->flipLockPin = NV_EVO_LOCK_PIN_INTERNAL(0);
        pHC->flipLock = FALSE;
        pHC->useStallLockPin = FALSE;
        pHC->stallLockPin = NV_EVO_LOCK_PIN_INTERNAL(0);
        pHC->crashLockUnstallMode = FALSE;


        EvoUpdateHeadParams(pDispEvo, head, pUpdateState);
    }
}

NvBool nvEvoGetSingleTileHwModeTimings(const NVHwModeTimingsEvo *pSrc,
                                       const NvU32 numTiles,
                                       NVHwModeTimingsEvo *pDst)
{
    if (numTiles == 1) {
        *pDst = *pSrc;
        return TRUE;
    }

    if ((numTiles == 0) ||
            (pSrc->viewPort.out.xAdjust != 0) ||
            (pSrc->viewPort.out.width != nvEvoVisibleWidth(pSrc))) {
        return FALSE;
    }

    if (((pSrc->rasterSize.x % numTiles) != 0) ||
            (((pSrc->rasterSyncEnd.x + 1) % numTiles) != 0) ||
            (((pSrc->rasterBlankEnd.x + 1) % numTiles) != 0) ||
            (((pSrc->rasterBlankStart.x + 1) % numTiles) != 0) ||
            ((pSrc->pixelClock % numTiles) != 0) ||
            ((pSrc->viewPort.in.width % numTiles) != 0)) {
        return FALSE;
    }

    *pDst = *pSrc;

    pDst->rasterSize.x /= numTiles;
    pDst->rasterSyncEnd.x /= numTiles;
    pDst->rasterBlankEnd.x /= numTiles;
    pDst->rasterBlankStart.x /= numTiles;

    pDst->pixelClock /= numTiles;

    pDst->viewPort.out.width /= numTiles;
    pDst->viewPort.in.width /= numTiles;

    return TRUE;
}

NvBool nvEvoUse2Heads1OR(const NVDpyEvoRec *pDpyEvo,
                         const NVHwModeTimingsEvo *pTimings,
                         const struct NvKmsModeValidationParams *pParams)
{
    const NVDispEvoRec *pDispEvo = pDpyEvo->pDispEvo;
    const NvU32 sd = pDispEvo->displayOwner;
    const NVEvoHeadCaps *pHeadCaps =
        &pDispEvo->pDevEvo->gpus[sd].capabilities.head[0];

    /* The 2Heads1OR mode can not be used if GPU does not
     * support merge mode, or */
    if (!pDispEvo->pDevEvo->hal->caps.supportsMergeMode ||
            /* the 2Heads1OR mode is forced disabled by client, or */
            ((pParams->overrides &
              NVKMS_MODE_VALIDATION_MAX_ONE_HARDWARE_HEAD) != 0) ||
            /* the given dpy does not support the display stream compression
             * and the given mode timings are not using the hardware YUV420
             * packer, or */
            (!nvDPDpyIsDscPossible(pDpyEvo) && !nvHdmiDpySupportsDsc(pDpyEvo) &&
                (pTimings->yuv420Mode != NV_YUV420_MODE_HW)) ||
            /* the non-centered viewport out does not work with 2Heads1OR mode
             * an for simplicity disable all customized viewport out, or */
            (pTimings->viewPort.out.width != nvEvoVisibleWidth(pTimings)) ||
            (pTimings->viewPort.out.xAdjust != 0) ||
            /* either HVisible, HSyncWidth, HBackPorch, HForntPorch,
             * pixelClock, or viewPortIn width is odd and can not be split
             * equally across two heads, or */
            ((pTimings->rasterSize.x & 1 ) != 0) ||
            ((pTimings->rasterSyncEnd.x & 1) != 1) ||
            ((pTimings->rasterBlankEnd.x & 1) != 1) ||
            ((pTimings->rasterBlankStart.x & 1) != 1) ||
            ((pTimings->pixelClock & 1) != 0) ||
            ((pTimings->viewPort.in.width & 1) != 0)) {
        return FALSE;
    }

    /* Use 2Heads1OR mode only if the required pixel clock is greater than the
     * maximum pixel clock support by a head. */
    return (pTimings->pixelClock > pHeadCaps->maxPClkKHz);
}

NvBool nvIsLockGroupFlipLocked(const NVLockGroup *pLockGroup)
{
    return pLockGroup->flipLockEnabled;
}

NvBool nvEvoIsConsoleActive(const NVDevEvoRec *pDevEvo)
{
    /*
     * The actual console state can be known only after allocation of core
     * channel, if core channel is not allocated yet then assume that console
     * is active.
     */
    if (pDevEvo->core == NULL) {
        return TRUE;
    }

    /*
     * If (pDevEvo->modesetOwner == NULL) that means either the vbios
     * console or the NVKMS console might be active.
     *
     * If (pDevEvo->modesetOwner != NULL) but
     * pDevEvo->modesetOwnerChanged is TRUE, that means the modeset
     * ownership is grabbed by the external client but it hasn't
     * performed any modeset and the console is still active.
     */
    if ((pDevEvo->modesetOwner == NULL) || pDevEvo->modesetOwnerChanged) {
        NvU32 sd;
        const NVDispEvoRec *pDispEvo;
        FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {
            if (nvGetActiveHeadMask(pDispEvo) != 0x0) {
                return TRUE;
            }
        }
    }

    return FALSE;
}

