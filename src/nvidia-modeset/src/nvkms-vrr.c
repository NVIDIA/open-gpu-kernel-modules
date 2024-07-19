/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "nvkms-dma.h"
#include "nvkms-evo.h"
#include "nvkms-rm.h"
#include "nvkms-rmapi.h"
#include "nvkms-vrr.h"
#include "dp/nvdp-connector-event-sink.h"
#include "nvkms-hdmi.h"
#include "nvkms-dpy.h"
#include "nvkms-sync.h"

#include <ctrl/ctrl0000/ctrl0000unix.h>
#include <class/cl0040.h> /* NV01_MEMORY_LOCAL_USER */
#include <class/cl2080.h> /* NV20_SUBDEVICE_0 */
#include <ctrl/ctrl2080/ctrl2080event.h> /* NV2080_CTRL_CMD_EVENT_SET_NOTIFICATION */

#define MAX_VRR_NOTIFIER_SLOTS_PER_HEAD 4
#define MAX_NOTIFIER_SIZE 0x10
#define NOTIFIER_BYTES_PER_HEAD \
    (MAX_VRR_NOTIFIER_SLOTS_PER_HEAD * MAX_NOTIFIER_SIZE)

#define MAX_VRR_FLIP_DELAY_TIME_RETRY_COUNT 5

typedef struct _vrrSurfaceNotifier
{
    NvU8 notifier[NV_MAX_HEADS][NOTIFIER_BYTES_PER_HEAD];
} vrrSurfaceNotifier, *vrrSurfaceNotifierPtr;

/*!
 * This file contains routines for handling Variable Refresh Rate (VRR) display
 * mode, also known as G-SYNC (not to be confused with the feature formerly
 * known as G-SYNC, which is now called Quadro Sync).
 *
 * VRR in NVKMS is handled in several phases:
 *
 *  1. During modeset, if NvKmsSetModeRequest::allowVrr is true and VRR-capable
 *     dpys are present, VRR is "enabled". This means that
 *
 *     a. VRR is disabled, if it was enabled before.
 *     b. The raster timings are adjusted by extending the back porch by 2
 *        lines. This signals to the monitor that it should enter G-SYNC mode.
 *     c. The mode is set.
 *     d. (EVO only) The RM VRR state machine is initialized, but left in
 *        "suspended" mode.
 *     e. Raster lock and frame lock are disabled.
 *
 *     pDevEvo->vrr.enabled indicates whether VRR was enabled successfully at
 *     modeset time.
 *
 *  2. At flip time, if NvKmsFlipRequest::allowVrr is true, VRR is "activated".
 *
 *     a. Stall lock is enabled.
 *     b. (NVDisplay only) The RG is switched from continuous mode to one-shot
 *        mode.
 *     c. (EVO only) RM's VRR state machine is enabled.
 *     d. (EVO only) The NVKMS client is told to release a special frame ready
 *        semaphore which tells RM to unstall the head.
 *     e. (NVDisplay only) The window channel flip is submitted with
 *        NVC37E_UPDATE_RELEASE_ELV_TRUE to trigger an unstall when the frame is
 *        ready.
 *
 *     pDevEvo->vrr.active (not to be confused with pDevEvo->vrr.enabled,
 *     described above) indicates whether VRR was activated successfully at flip
 *     time.
 *
 *  3. Also at flip time, if NvKmsFlipRequest::allowVrr is false, VRR is
 *     "deactivated".
 *
 *     a. Stall lock is disabled.
 *     b. (NVDisplay only) the RG is switched from one-shot mode to continuous
 *        mode.
 *     c. (EVO only) RM's VRR state machine is suspended.
 */

static NvBool SetVrrActivePriv(NVDevEvoPtr pDevEvo, NvBool active);
static void ConfigVrrPstateSwitch(NVDispEvoPtr pDispEvo,
                                  NvBool vrrEnabled,
                                  NvBool vrrState,
                                  NvBool vrrDirty,
                                  NvU32 head);


/*!
 * Allocate the VRR semaphore surface.
 *
 * Only one array of VRR semaphores is needed per "head group", which for our
 * purposes means a pDevEvo.  This array is allocated when the device is
 * initialized and kept around for the lifetime of the pDevEvo.
 */
void nvAllocVrrEvo(NVDevEvoPtr pDevEvo)
{
    NvU32 handle;
    NvU64 size = NVKMS_VRR_SEMAPHORE_SURFACE_SIZE;

    /* On GPUs that support the HEAD_SET_DISPLAY_RATE method (nvdisplay), we
     * don't need a VRR semaphore surface. */
    if (pDevEvo->hal->caps.supportsDisplayRate) {
        return;
    }

    handle = nvGenerateUnixRmHandle(&pDevEvo->handleAllocator);

    if (nvRmAllocSysmem(pDevEvo, handle, NULL, &pDevEvo->vrr.pSemaphores,
                        size, NVKMS_MEMORY_NISO)) {
        pDevEvo->vrr.semaphoreHandle = handle;
    } else {
        nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                    "Failed to allocate G-SYNC semaphore memory");
        nvFreeUnixRmHandle(&pDevEvo->handleAllocator, handle);
    }
}

void nvFreeVrrEvo(NVDevEvoPtr pDevEvo)
{
    if (pDevEvo->vrr.semaphoreHandle != 0) {
        if (pDevEvo->vrr.pSemaphores != NULL) {
            nvRmApiUnmapMemory(nvEvoGlobal.clientHandle,
                               pDevEvo->deviceHandle,
                               pDevEvo->vrr.semaphoreHandle,
                               pDevEvo->vrr.pSemaphores,
                               0);
            pDevEvo->vrr.pSemaphores = NULL;
        }
        nvRmApiFree(nvEvoGlobal.clientHandle, pDevEvo->deviceHandle,
                    pDevEvo->vrr.semaphoreHandle);
        nvFreeUnixRmHandle(&pDevEvo->handleAllocator,
                           pDevEvo->vrr.semaphoreHandle);
        pDevEvo->vrr.semaphoreHandle = 0;
    }
}

NvBool nvExportVrrSemaphoreSurface(const NVDevEvoRec *pDevEvo, int fd)
{
    // Export the memory as an FD.
    NV0000_CTRL_OS_UNIX_EXPORT_OBJECT_TO_FD_PARAMS exportParams = { };
    const NvU32 hMemory = pDevEvo->vrr.semaphoreHandle;
    NvU32 status;

    if (hMemory == 0) {
        return FALSE;
    }

    exportParams.fd = fd;
    exportParams.object.type = NV0000_CTRL_OS_UNIX_EXPORT_OBJECT_TYPE_RM;
    exportParams.object.data.rmObject.hDevice = pDevEvo->deviceHandle;
    exportParams.object.data.rmObject.hObject = hMemory;

    status = nvRmApiControl(nvEvoGlobal.clientHandle,
                           nvEvoGlobal.clientHandle,
                           NV0000_CTRL_CMD_OS_UNIX_EXPORT_OBJECT_TO_FD,
                           &exportParams, sizeof(exportParams));

    return status == NVOS_STATUS_SUCCESS;
}

/*!
 * Return TRUE dpy support G-SYNC.
 */
static NvBool DpyIsGsync(const NVDpyEvoRec *pDpyEvo)
{
    return pDpyEvo->vrr.type == NVKMS_DPY_VRR_TYPE_GSYNC;
}

static NvBool AnyEnabledAdaptiveSyncDpys(const NVDevEvoRec *pDevEvo)
{
    NVDispEvoPtr pDispEvo;
    NvU32 dispIndex;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        NvU32 head;

        for (head = 0; head < pDevEvo->numHeads; head++) {
            const NVDispHeadStateEvoRec *pHeadState =
                &pDispEvo->headState[head];

            if (nvIsAdaptiveSyncDpyVrrType(pHeadState->timings.vrr.type)) {
                return TRUE;
            }
        }
    }

    return FALSE;
}

static NvBool DpyAllowsAdaptiveSync(
    const NVDpyEvoRec *pDpyEvo,
    const enum NvKmsAllowAdaptiveSync allowAdaptiveSync,
    const NvModeTimings *pTimings)
{
    /*
     * HDMI VRR and HDMI 3D both use the vendor specific infoframe in HW,
     * so disallow HDMI VRR when attempting to set an HDMI 3D mode.
     */

    if (pTimings->hdmi3D) {
        return FALSE;
    }

    return ((allowAdaptiveSync ==
             NVKMS_ALLOW_ADAPTIVE_SYNC_DEFAULTLISTED_ONLY) &&
            (pDpyEvo->vrr.type ==
             NVKMS_DPY_VRR_TYPE_ADAPTIVE_SYNC_DEFAULTLISTED)) ||
           ((allowAdaptiveSync == NVKMS_ALLOW_ADAPTIVE_SYNC_ALL) &&
            nvDpyIsAdaptiveSync(pDpyEvo));
}

NvBool nvDispSupportsVrr(const NVDispEvoRec *pDispEvo)
{
    // Don't allow VRR if a framelock device is present.
    // (In other words, don't allow G-SYNC with Quadro Sync).
    return !pDispEvo->pFrameLockEvo;
}

enum NvKmsDpyVRRType
nvGetAllowedDpyVrrType(const NVDpyEvoRec *pDpyEvo,
                       const NvModeTimings *pTimings,
                       enum NvKmsStereoMode stereoMode,
                       const NvBool allowGsync,
                       const enum NvKmsAllowAdaptiveSync allowAdaptiveSync)
{
    /*
     * Mark these mode timings as indicating a VRR mode, even if the timings
     * don't need to be adjusted; this is used to distinguish between VRR and
     * non-VRR heads elsewhere.
     */

    if ((stereoMode == NVKMS_STEREO_DISABLED) &&
        ((allowGsync && DpyIsGsync(pDpyEvo)) ||
         DpyAllowsAdaptiveSync(pDpyEvo, allowAdaptiveSync, pTimings))) {
        return pDpyEvo->vrr.type;
    }

    return NVKMS_DPY_VRR_TYPE_NONE;
}

static NvBool GetEdidTimeoutMicroseconds(
    const NVDpyEvoRec *pDpyEvo,
    const NVHwModeTimingsEvo *pTimings,
    NvU32 *pEdidTimeoutMicroseconds)
{
    const NvU32 rr10kHz = nvGetRefreshRate10kHz(pTimings);
    const NVParsedEdidEvoRec *pParsedEdid = &pDpyEvo->parsedEdid;
    const NVT_DISPLAYID_2_0_INFO *pDisplayIdInfo = NULL;
    const NvU32 nominalRefreshRateHz = rr10kHz / 10000; // XXX round?
    NVT_PROTOCOL sinkProtocol = NVT_PROTOCOL_UNKNOWN;
    NvU32 fmin;

    if (!pParsedEdid->valid) {
        return FALSE;
    }

    // XXX not sufficient; see what DD does in changelist 34157172
    if (nvDpyUsesDPLib(pDpyEvo)) {
        sinkProtocol = NVT_PROTOCOL_DP;
    } else if (nvDpyIsHdmiEvo(pDpyEvo)) {
        sinkProtocol = NVT_PROTOCOL_HDMI;
    }

    fmin = NvTiming_GetVrrFmin(&pParsedEdid->info,
                               pDisplayIdInfo,
                               nominalRefreshRateHz,
                               sinkProtocol);

    if (fmin == 0) {
        if (pDpyEvo->internal && pDpyEvo->pDispEvo->vrr.hasPlatformCookie) {

            /*
             * An internal notebook VRR panel must have a non-zero fmin.  The
             * recommendation from hardware is to use a default of fmin =
             * rr/2.4.  So, compute timeoutUsec as:
             *
             * timeoutUsec = 10^6 / fmin
             *             = 10^6 / (rr/2.4)
             *             = 10^6 * (2.4/rr)
             *             = 10^5 * 24 / rr
             */
            *pEdidTimeoutMicroseconds = 2400000 / nominalRefreshRateHz;
            return TRUE;
        }

        if (pDpyEvo->vrr.type == NVKMS_DPY_VRR_TYPE_GSYNC) {
            /* GSYNC can have fmin==0; i.e., the panel is self-refreshing. */
            *pEdidTimeoutMicroseconds = 0;
            return TRUE;
        }

        /* Otherwise, VRR is not possible. */
        return FALSE;
    }

    *pEdidTimeoutMicroseconds = 1000000 / fmin;

    return TRUE;
}

/*! Adjust mode timings as necessary for VRR. */
void nvAdjustHwModeTimingsForVrrEvo(const NVDpyEvoRec *pDpyEvo,
                                    const enum NvKmsDpyVRRType vrrType,
                                    const NvU32 vrrOverrideMinRefreshRate,
                                    const NvBool needsSwFramePacing,
                                    NVHwModeTimingsEvoPtr pTimings)
{
    NvU32 timeoutMicroseconds;
    NvU32 edidTimeoutMicroseconds;

    if (vrrType == NVKMS_DPY_VRR_TYPE_NONE) {
        return;
    }

    if (!GetEdidTimeoutMicroseconds(pDpyEvo,
                                    pTimings,
                                    &edidTimeoutMicroseconds)) {
        return;
    }

    // Allow overriding the EDID min refresh rate on Adaptive-Sync
    // displays.
    if (nvIsAdaptiveSyncDpyVrrType(vrrType) && vrrOverrideMinRefreshRate) {
        NvU32 minMinRefreshRate, maxMinRefreshRate;
        NvU32 clampedMinRefreshRate;

        nvGetDpyMinRefreshRateValidValues(pTimings,
                                          vrrType,
                                          edidTimeoutMicroseconds,
                                          &minMinRefreshRate,
                                          &maxMinRefreshRate);

        clampedMinRefreshRate =
            NV_MAX(vrrOverrideMinRefreshRate, minMinRefreshRate);

        clampedMinRefreshRate =
            NV_MIN(clampedMinRefreshRate, maxMinRefreshRate);

        timeoutMicroseconds = 1000000 / clampedMinRefreshRate;
    } else {
        timeoutMicroseconds = edidTimeoutMicroseconds;
    }

    // Disallow VRR if the refresh rate is less than 110% of the VRR minimum
    // refresh rate.
    if (nvGetRefreshRate10kHz(pTimings) <
        (((NvU64) 1000000 * 11000) / timeoutMicroseconds)) {
        return;
    }

    /*
     * On G-SYNC panels, the back porch extension is used to indicate to
     * the monitor that VRR is enabled.  It is not necessary on
     * Adaptive-Sync displays.
     */
    if (vrrType == NVKMS_DPY_VRR_TYPE_GSYNC) {
        pTimings->rasterSize.y += 2;
        pTimings->rasterBlankEnd.y += 2;
        pTimings->rasterBlankStart.y += 2;
    }

    pTimings->vrr.timeoutMicroseconds = timeoutMicroseconds;
    pTimings->vrr.needsSwFramePacing = needsSwFramePacing;
    pTimings->vrr.type = vrrType;
}

static void TellRMAboutVrrHead(NVDispEvoPtr pDispEvo,
                               NVDispHeadStateEvoRec *pHeadState,
                               NvBool vrrPossible)
{
    if (pHeadState->activeRmId != 0) {
        NV0073_CTRL_SYSTEM_VRR_DISPLAY_INFO_PARAMS params = { };
        NvU32 ret;

        params.subDeviceInstance = pDispEvo->displayOwner;
        params.displayId = pHeadState->activeRmId;
        params.bAddition = vrrPossible;

        ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                             pDispEvo->pDevEvo->displayCommonHandle,
                             NV0073_CTRL_CMD_SYSTEM_VRR_DISPLAY_INFO,
                             &params, sizeof(params));
        if (ret != NVOS_STATUS_SUCCESS) {
            nvEvoLogDispDebug(pDispEvo, EVO_LOG_WARN,
                "NV0073_CTRL_CMD_SYSTEM_VRR_DISPLAY_INFO failed");
        }
    }
}

static void RmDisableVrr(NVDevEvoPtr pDevEvo)
{
    NVDispEvoPtr pDispEvo;
    NvU32 head, dispIndex;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        for (head = 0; head < pDevEvo->numHeads; head++) {
            ConfigVrrPstateSwitch(pDispEvo, FALSE /* vrrEnabled */,
                                  FALSE /* vrrState */,
                                  TRUE /* vrrDirty */,
                                  head);
        }
    }
    nvAssert(pDevEvo->hal->caps.supportsDisplayRate);
}

NvU16 nvPrepareNextVrrNotifier(NVEvoChannelPtr pChannel, NvU32 sd, NvU32 head)
{
    enum NvKmsNIsoFormat nIsoFormat = NVKMS_NISO_FORMAT_FOUR_WORD_NVDISPLAY;

    vrrSurfaceNotifierPtr pNotifiers = pChannel->notifiersDma[sd].subDeviceAddress[sd];

    const NvU32 notifierSize =
        nvKmsSizeOfNotifier(nIsoFormat, FALSE /* overlay */);

    const NvU8 nextSlot =
            pChannel->notifiersDma[sd].vrrNotifierHead[head].vrrNotifierNextSlot;

    const NvU8 *headBase = pNotifiers->notifier[head];

    const NvU8 offsetInBytes =
        (headBase - ((const NvU8 *) pNotifiers)) + (notifierSize * nextSlot);

    nvAssert(notifierSize <= MAX_NOTIFIER_SIZE);

    nvKmsResetNotifier(nIsoFormat, FALSE /* overlay */,
                       nextSlot, pNotifiers->notifier[head]);

    pChannel->notifiersDma[sd].vrrNotifierHead[head].vrrNotifierNextSlot =
        (nextSlot + 1) % MAX_VRR_NOTIFIER_SLOTS_PER_HEAD;

    return offsetInBytes / 4;
}

static void SetTimeoutPerFrame(void *dataPtr, NvU32 dataU32)
{
    // Set the timeout after which the current frame will self-refresh.
    NVDispEvoPtr pDispEvo = dataPtr;
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NVEvoUpdateState updateState = { };
    NvU32 head;
    NvU32 inputHead = dataU32;
    NVDispHeadStateEvoRec *pInputHeadState = &pDispEvo->headState[inputHead];
    NvU32 displayRate = pInputHeadState->displayRate;
    struct NvKmsVrrFramePacingInfo *pInputVrrFramePacingInfo =
                            &(pInputHeadState->vrrFramePacingInfo);
    const NvU32 headsMask = pInputHeadState->mergeModeVrrSecondaryHeadMask |
        NVBIT(inputHead);
    volatile NV0073_CTRL_RM_VRR_SHARED_DATA *pData = pInputVrrFramePacingInfo->pData;

    /*
     * XXX[2Heads1OR] Implement per api-head frame pacing and remove this
     * mergeMode check and NVDispEvoRec::mergeModeVrrSecondaryHeadMask.
     */
    if ((pInputHeadState->mergeMode == NV_EVO_MERGE_MODE_SECONDARY) ||
            !pInputVrrFramePacingInfo->framePacingActive ||
            (displayRate == pData->timeout)) {
        return;
    }

    nvPushEvoSubDevMaskDisp(pDispEvo);
    FOR_EACH_EVO_HW_HEAD_IN_MASK(headsMask, head) {
        pDispEvo->headState[head].displayRate = pData->timeout;

        pDevEvo->hal->SetDisplayRate(pDispEvo, head,
                                     TRUE /* enable */,
                                     &updateState,
                                     pDispEvo->headState[head].displayRate / 1000);
    }

    /*
     * In order to change the one shot self refresh timeout mid-frame without
     * immediately triggering a new frame, skip setting RELEASE_ELV for this
     * update.
     */
    nvEvoUpdateAndKickOff(pDispEvo, FALSE, &updateState,
                          FALSE /* releaseElv */);
    nvPopEvoSubDevMask(pDevEvo);
}

static void SetTimeoutEvent(void *arg, void *pEventDataVoid, NvU32 hEvent,
                            NvU32 Data, NV_STATUS Status)
{
    Nv2080VrrSetTimeoutNotification  *pParams = pEventDataVoid;

    (void) nvkms_alloc_timer_with_ref_ptr(
        SetTimeoutPerFrame, /* callback */
        arg, /* argument (this is a ref_ptr to a pDispEvo) */
        pParams->head,   /* dataU32 */
        0);
}

static void DisableVrrSetTimeoutEvent(NVDispEvoRec *pDispEvo)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NvU32 sd = pDispEvo->displayOwner;
    NvU32 subdeviceHandle = pDevEvo->pSubDevices[sd]->handle;
    NV2080_CTRL_EVENT_SET_NOTIFICATION_PARAMS setEventParams = {0};
    NvU32 ret;

    nvAssert(pDispEvo->vrrSetTimeoutEventUsageCount != 0);

    pDispEvo->vrrSetTimeoutEventUsageCount--;
    if (pDispEvo->vrrSetTimeoutEventUsageCount != 0) {
        return;
    }

    nvAssert(pDispEvo->vrrSetTimeoutEventHandle != 0);

    setEventParams.event = NV2080_NOTIFIERS_VRR_SET_TIMEOUT;
    setEventParams.action = NV2080_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE;
    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         subdeviceHandle,
                         NV2080_CTRL_CMD_EVENT_SET_NOTIFICATION,
                         &setEventParams,
                         sizeof(setEventParams));
    if (ret != NVOS_STATUS_SUCCESS) {
        nvEvoLogDev(pDevEvo, EVO_LOG_WARN,
                "NV2080_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE failed for vrr %d", ret);
    }

    ret = nvRmApiFree(nvEvoGlobal.clientHandle,
                      subdeviceHandle,
                      pDispEvo->vrrSetTimeoutEventHandle);
    if (ret != NVOS_STATUS_SUCCESS) {
        nvEvoLogDev(pDevEvo, EVO_LOG_WARN,
                    "nvRmApiFree(notify) failed for vrr %d", ret);
    }

    nvFreeUnixRmHandle(&pDevEvo->handleAllocator,
                       pDispEvo->vrrSetTimeoutEventHandle);
    pDispEvo->vrrSetTimeoutEventHandle = 0;
}

static NvBool EnableVrrSetTimeoutEvent(NVDispEvoRec *pDispEvo)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NvU32 sd = pDispEvo->displayOwner;
    NvU32 subdeviceHandle = pDevEvo->pSubDevices[sd]->handle;
    NV2080_CTRL_EVENT_SET_NOTIFICATION_PARAMS setEventParams = { };
    NvU32 ret;

    if (pDispEvo->vrrSetTimeoutEventUsageCount != 0) {
        nvAssert(pDispEvo->vrrSetTimeoutEventHandle != 0);
        goto done;
    }

    nvAssert(pDispEvo->vrrSetTimeoutEventHandle == 0);

    pDispEvo->vrrSetTimeoutEventHandle =
        nvGenerateUnixRmHandle(&pDevEvo->handleAllocator);

    if (!nvRmRegisterCallback(pDevEvo,
                              &pDispEvo->vrrSetTimeoutCallback,
                              pDispEvo->ref_ptr,
                              subdeviceHandle,
                              pDispEvo->vrrSetTimeoutEventHandle,
                              SetTimeoutEvent,
                              NV2080_NOTIFIERS_VRR_SET_TIMEOUT)) {
        nvFreeUnixRmHandle(&pDevEvo->handleAllocator,
                           pDispEvo->vrrSetTimeoutEventHandle);
        nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                    "nvRmRegisterCallback failed for vrr");
        return FALSE;
    }

    // Enable VRR notifications from this subdevice.
    setEventParams.event = NV2080_NOTIFIERS_VRR_SET_TIMEOUT;
    setEventParams.action = NV2080_CTRL_EVENT_SET_NOTIFICATION_ACTION_REPEAT;
    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                              subdeviceHandle,
                              NV2080_CTRL_CMD_EVENT_SET_NOTIFICATION,
                              &setEventParams,
                              sizeof(setEventParams));
    if (ret != NVOS_STATUS_SUCCESS) {
        nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                    "NV2080_CTRL_EVENT_SET_NOTIFICATION_ACTION_REPEAT failed for vrr 0x%x", ret);
        nvRmApiFree(nvEvoGlobal.clientHandle,
                    subdeviceHandle, pDispEvo->vrrSetTimeoutEventHandle);
        nvFreeUnixRmHandle(&pDevEvo->handleAllocator,
                           pDispEvo->vrrSetTimeoutEventHandle);
        pDispEvo->vrrSetTimeoutEventHandle = 0;
        return FALSE;
    }

done:
    pDispEvo->vrrSetTimeoutEventUsageCount++;
    return TRUE;
}

static NvBool VrrRgLineActiveSessionOpen(NVDispEvoPtr pDispEvo,
                                        struct NvKmsVrrFramePacingInfo *pVrrFramePacingInfo)
{
    NvU32 ret = 0;
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NvU32 sd = pDispEvo->displayOwner;
    NvU32 subdeviceHandle = pDevEvo->pSubDevices[sd]->handle;
    NV_MEMORY_ALLOCATION_PARAMS memAllocParams = { };
    NvHandle memoryHandle;
    void *address = NULL;

    if (!EnableVrrSetTimeoutEvent(pDispEvo)) {
        return FALSE;
    }

    /* allocate memory from vidmem */
    memoryHandle = nvGenerateUnixRmHandle(&pDevEvo->handleAllocator);
    memAllocParams.owner = NVKMS_RM_HEAP_ID;
    memAllocParams.type = NVOS32_TYPE_DMA;
    memAllocParams.size = sizeof(NV0073_CTRL_RM_VRR_SHARED_DATA);
    memAllocParams.attr = DRF_DEF(OS32, _ATTR, _PAGE_SIZE, _4KB) |
                          DRF_DEF(OS32, _ATTR, _PHYSICALITY, _CONTIGUOUS) |
                          DRF_DEF(OS32, _ATTR, _COHERENCY, _UNCACHED) |
                          DRF_DEF(OS32, _ATTR, _LOCATION, _VIDMEM);

    memAllocParams.flags |= (NVOS32_ALLOC_FLAGS_ALLOCATE_KERNEL_PRIVILEGED |
                            NVOS32_ALLOC_FLAGS_PERSISTENT_VIDMEM);

    ret = nvRmApiAlloc(nvEvoGlobal.clientHandle,
                       pDevEvo->deviceHandle,
                       memoryHandle,
                       NV01_MEMORY_LOCAL_USER,
                       &memAllocParams);

    if (ret != NVOS_STATUS_SUCCESS) {
        nvEvoLogDev(pDevEvo, EVO_LOG_ERROR, "nvRmApiAlloc(memory) failed for vrr 0x%x", ret);
        goto free_memory_handle;
    }

    ret = nvRmApiMapMemory(nvEvoGlobal.clientHandle,
                           subdeviceHandle,
                           memoryHandle,
                           0,
                           sizeof(NV0073_CTRL_RM_VRR_SHARED_DATA),
                           &address,
                           0);
    if ((ret != NVOS_STATUS_SUCCESS) || (address == NULL)) {
        nvEvoLogDev(pDevEvo, EVO_LOG_ERROR, " nvRmApiMapMemory failed for vrr 0x%x addr: %p",
                ret, address);
        nvRmApiFree(nvEvoGlobal.clientHandle,
              subdeviceHandle, memoryHandle);
        goto free_memory_handle;
    }

    pVrrFramePacingInfo->pData = (NV0073_CTRL_RM_VRR_SHARED_DATA *)address;
    pVrrFramePacingInfo->memoryHandle = memoryHandle;

    return TRUE;

free_memory_handle:
    nvFreeUnixRmHandle(&pDevEvo->handleAllocator, memoryHandle);
    DisableVrrSetTimeoutEvent(pDispEvo);
    return FALSE;
}

static void VrrRgLineActiveSessionClose(NVDispEvoPtr pDispEvo,
                                        struct NvKmsVrrFramePacingInfo *pVrrFramePacingInfo)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NvU32 sd = pDispEvo->displayOwner;
    NvU32 subdeviceHandle = pDevEvo->pSubDevices[sd]->handle;
    NvU32 ret = 0;

    // clean up allocated memory
    ret = nvRmApiUnmapMemory(nvEvoGlobal.clientHandle,
                            subdeviceHandle,
                            pVrrFramePacingInfo->memoryHandle,
                            (void *)pVrrFramePacingInfo->pData,
                            0);
    if (ret != NVOS_STATUS_SUCCESS) {
        nvEvoLogDev(pDevEvo, EVO_LOG_WARN, "nvRmApiUnmapMemory failed for vrr %d", ret);
    }

    ret = nvRmApiFree(nvEvoGlobal.clientHandle,
                        subdeviceHandle,
                        pVrrFramePacingInfo->memoryHandle);
    if (ret != NVOS_STATUS_SUCCESS) {
        nvEvoLogDev(pDevEvo, EVO_LOG_WARN, "nvRmApiFree(memory) failed for vrr %d", ret);
    }

    nvFreeUnixRmHandle(&pDevEvo->handleAllocator, pVrrFramePacingInfo->memoryHandle);

    DisableVrrSetTimeoutEvent(pDispEvo);
}

/*!
 * Enable or disable SW Frame Pacing for one head.
 *
 * This will reset the NVDispHeadStateEvoRec::NvKmsVrrFramePacingInfo state used
 * to track Frame pacing per head and call RM to set an interrupt to be called
 * at every first RG scanline of every frame (whether initiated by a flip or
 * a self-refresh).
 */
static NvBool SetSwFramePacing(NVDispEvoPtr pDispEvo, NvU32 head, NvBool enable)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NVDispHeadStateEvoRec *pHeadState = &pDispEvo->headState[head];
    struct NvKmsVrrFramePacingInfo *pVrrFramePacingInfo =
        &pHeadState->vrrFramePacingInfo;
    const NVHwModeTimingsEvo *pTimings = &pHeadState->timings;
    const NvU32 timeout = pTimings->vrr.timeoutMicroseconds;
    struct NV0073_CTRL_CMD_SYSTEM_VRR_SET_RGLINE_ACTIVE_PARAMS params = { };
    NvU32 maxFrameTime = 0;
    NvU32 minFrameTime = 0;

    if (pVrrFramePacingInfo->framePacingActive == enable) {
        return TRUE;
    }

    if (enable) {
        maxFrameTime = timeout * 1000;
        minFrameTime = 1000 *
            axb_div_c(pTimings->rasterSize.y * 1000,
                      pTimings->rasterSize.x,
                      pTimings->pixelClock);

        /*
         * SW Frame pacing won't work with infinite self-refresh adaptive sync
         * or direct drive panels (none of which currently exist) or when
         * driving a mode below the panel's minimum refresh rate.
         */
        if ((maxFrameTime == 0) ||
            (minFrameTime == 0) ||
            (minFrameTime >= maxFrameTime)) {
            nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                        "Failed to set variable refresh rate with invalid "
                        "minimum frame time (%u ns) or maximum frame time "
                        "(%u ns)", minFrameTime, maxFrameTime);
            return FALSE;
        }

        if (!VrrRgLineActiveSessionOpen(pDispEvo, pVrrFramePacingInfo)) {
            nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                        "Failed to setup Rgline active session for vrr");
            return FALSE;
        }
    }

    params.head = head;
    params.height = 1;
    params.bEnable = enable;
    params.subDeviceInstance = pDispEvo->displayOwner;
    params.maxFrameTime = maxFrameTime;
    params.minFrameTime = minFrameTime;
    params.hMemory = pVrrFramePacingInfo->memoryHandle;
    if (nvRmApiControl(nvEvoGlobal.clientHandle,
                   pDispEvo->pDevEvo->displayCommonHandle,
                   NV0073_CTRL_CMD_SYSTEM_VRR_SET_RGLINE_ACTIVE,
                   &params, sizeof(params))
        != NVOS_STATUS_SUCCESS) {
        nvAssert(!"NV0073_CTRL_CMD_SYSTEM_VRR_SET_RGLINE_ACTIVE failed");
        VrrRgLineActiveSessionClose(pDispEvo, pVrrFramePacingInfo);
        return FALSE;
    }

    if (!enable) {
        VrrRgLineActiveSessionClose(pDispEvo, pVrrFramePacingInfo);

        // Reset the state used to track SW Frame pacing.
        nvkms_memset(pVrrFramePacingInfo, 0, sizeof(*pVrrFramePacingInfo));
    }

    pVrrFramePacingInfo->framePacingActive = enable;
    return TRUE;
}

void nvDisableVrr(NVDevEvoPtr pDevEvo)
{
    NVDispEvoPtr pDispEvo;
    NvU32 head, dispIndex;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        for (head = 0; head < pDevEvo->numHeads; head++) {
            NVDispHeadStateEvoRec *pHeadState = &pDispEvo->headState[head];

            TellRMAboutVrrHead(pDispEvo, pHeadState, FALSE);

            SetSwFramePacing(pDispEvo, head, FALSE);
        }
    }

    if (!pDevEvo->vrr.enabled) {
        return;
    }

    SetVrrActivePriv(pDevEvo, FALSE);
    RmDisableVrr(pDevEvo);

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        for (head = 0; head < pDevEvo->numHeads; head++) {
            NVDispHeadStateEvoRec *pHeadState = &pDispEvo->headState[head];

            if ((pHeadState->pConnectorEvo != NULL) &&
                    nvIsAdaptiveSyncDpyVrrType(pHeadState->timings.vrr.type)) {
                if (nvConnectorUsesDPLib(pHeadState->pConnectorEvo)) {
                    nvDPLibSetAdaptiveSync(pDispEvo, head, FALSE);
                } else {
                    nvHdmiSetVRR(pDispEvo, head, FALSE);
                }
            }
        }
    }

    pDevEvo->vrr.enabled = FALSE;
    nvAssert(!pDevEvo->vrr.active);
}

static NvBool AnyEnabledGsyncDpys(const NVDevEvoRec *pDevEvo)
{
    NVDispEvoPtr pDispEvo;
    NvU32 dispIndex;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        NvU32 head;

        for (head = 0; head < pDevEvo->numHeads; head++) {
            const NVDispHeadStateEvoRec *pHeadState = &pDispEvo->headState[head];

            if (pHeadState->timings.vrr.type == NVKMS_DPY_VRR_TYPE_GSYNC) {
                return TRUE;
            }
        }
    }

    return FALSE;
}

static NvBool RmEnableVrr(NVDevEvoPtr pDevEvo)
{
    NVDispEvoPtr pDispEvo;
    NvU32 dispIndex, head;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        for (head = 0; head < pDevEvo->numHeads; head++) {
            ConfigVrrPstateSwitch(pDispEvo, TRUE /* vrrEnabled */,
                                  FALSE /* vrrState */,
                                  TRUE/* vrrDirty */,
                                  head);
        }
    }
    nvAssert(pDevEvo->hal->caps.supportsDisplayRate);
    return TRUE;
}

void nvGetDpyMinRefreshRateValidValues(
    const NVHwModeTimingsEvo *pTimings,
    const enum NvKmsDpyVRRType vrrType,
    const NvU32 edidTimeoutMicroseconds,
    NvU32 *minMinRefreshRate,
    NvU32 *maxMinRefreshRate)
{
    NvU32 edidMinRefreshRate;

    if (nvIsAdaptiveSyncDpyVrrType(vrrType)) {
        /*
         * Adaptive-Sync monitors must always define a nonzero minimum refresh
         * rate in the EDID, and a modeset may override this within a range
         * of NVKMS_VRR_MIN_REFRESH_RATE_MAX_VARIANCE, as long as the minimum
         * is not below 1hz and the maximum does not exceed the current
         * refresh rate.
         */
        NvU32 minTimeoutMicroseconds =
            axb_div_c(pTimings->rasterSize.y * 1000,
                      pTimings->rasterSize.x, pTimings->pixelClock);
        NvU32 maxRefreshRate = 1000000 / minTimeoutMicroseconds;

        nvAssert(edidTimeoutMicroseconds != 0);

        edidMinRefreshRate =
            1000000 / edidTimeoutMicroseconds;

        if (edidMinRefreshRate <= NVKMS_VRR_MIN_REFRESH_RATE_MAX_VARIANCE) {
            *minMinRefreshRate = 1;
        } else {
            *minMinRefreshRate = edidMinRefreshRate -
                NVKMS_VRR_MIN_REFRESH_RATE_MAX_VARIANCE;
        }

        *maxMinRefreshRate = NV_MIN(maxRefreshRate,
            edidMinRefreshRate + NVKMS_VRR_MIN_REFRESH_RATE_MAX_VARIANCE);
    } else {
        /*
         * Non-Adaptive-Sync panels may not override the EDID-provided minimum
         * refresh rate, which will be 1hz for most G-SYNC panels or 0hz for
         * true self-refresh panels.
         */
        edidMinRefreshRate = edidTimeoutMicroseconds ?
            1000000 / edidTimeoutMicroseconds : 0;
        *minMinRefreshRate = *maxMinRefreshRate = edidMinRefreshRate;
    }
}

/*!
 * Modify the VRR state to enable (but not activate) VRR at modeset time.
 *
 * This prepares VRR displays for VRR (through a DP MSA override for
 * Adaptive-Sync and a backporch extension for G-SYNC) and sets up the RM
 * VRR state machine (for pre-nvdisplay) but does not actually start VRR
 * flipping until nvSetVrrActive() is called at flip time.
 *
 * \param[in]  pDevEvo            The device that is enabling VRR.
 */
void nvEnableVrr(NVDevEvoPtr pDevEvo)
{
    NVDispEvoPtr pDispEvo;
    NvU32 head, dispIndex;

    nvAssert(!pDevEvo->vrr.enabled);

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        for (head = 0; head < pDevEvo->numHeads; head++) {
            NVDispHeadStateEvoRec *pHeadState = &pDispEvo->headState[head];

            if ((pHeadState->pConnectorEvo != NULL) &&
                    nvIsAdaptiveSyncDpyVrrType(pHeadState->timings.vrr.type)) {
                if (nvConnectorUsesDPLib(pHeadState->pConnectorEvo)) {
                    nvDPLibSetAdaptiveSync(pDispEvo, head, TRUE);
                } else {
                    nvHdmiSetVRR(pDispEvo, head, TRUE);
                }
            }
        }
    }

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        for (head = 0; head < pDevEvo->numHeads; head++) {
            NVDispHeadStateEvoRec *pHeadState = &pDispEvo->headState[head];

            // To allow VRR-based mclk switching, RM needs to know which heads
            // are driving VRR displays capable of extending vblank.  This
            // includes all G-SYNC displays (regardless of whether the modeset
            // indicates that G-SYNC is allowed) but only Adaptive-Sync
            // displays which put the display into Adaptive-Sync mode by calling
            // nvDPLibSetAdaptiveSync above.
            TellRMAboutVrrHead(pDispEvo,
                               pHeadState,
                               (pHeadState->timings.vrr.type !=
                                NVKMS_DPY_VRR_TYPE_NONE));
        }
    }

    if (!(AnyEnabledGsyncDpys(pDevEvo) ||
          AnyEnabledAdaptiveSyncDpys(pDevEvo))) {
        return;
    }

    if (!RmEnableVrr(pDevEvo)) {
        return;
    }

    pDevEvo->vrr.enabled = TRUE;
}

static void ClearElvBlock(NVDispEvoPtr pDispEvo, NvU32 head)
{
    const NVDispHeadStateEvoRec *pHeadState = &pDispEvo->headState[head];
    NV0073_CTRL_SYSTEM_CLEAR_ELV_BLOCK_PARAMS params = { };

    params.subDeviceInstance = pDispEvo->displayOwner;
    params.displayId = pHeadState->activeRmId;

    if (nvRmApiControl(nvEvoGlobal.clientHandle,
                       pDispEvo->pDevEvo->displayCommonHandle,
                       NV0073_CTRL_CMD_SYSTEM_CLEAR_ELV_BLOCK,
                       &params, sizeof(params))
            != NVOS_STATUS_SUCCESS) {
        nvAssert(!"CLEAR_ELV_BLOCK failed");
    }
}

static void ConfigVrrPstateSwitch(NVDispEvoPtr pDispEvo, NvBool vrrEnabled,
                                  NvBool vrrState, NvBool vrrDirty, NvU32 head)
{
    NV0073_CTRL_SYSTEM_CONFIG_VRR_PSTATE_SWITCH_PARAMS params = { };
    NvU32 ret;
    const NVDispHeadStateEvoRec *pHeadState = &pDispEvo->headState[head];
    const NVHwModeTimingsEvo *pTimings = &pHeadState->timings;

    if (nvkms_disable_vrr_memclk_switch() ||
        (pTimings->vrr.type == NVKMS_DPY_VRR_TYPE_NONE)) {
        return;
    }

    /*
     * An inactive head should always have pTimings->vrr.type ==
     * NVKMS_DPY_VRR_TYPE_NONE and therefore return early above.
     */
    nvAssert(nvHeadIsActive(pDispEvo, head));

    params.displayId = pHeadState->activeRmId;
    params.bVrrEnabled = vrrEnabled;
    params.bVrrState = vrrState;
    params.bVrrDirty = vrrDirty;

    if (params.bVrrDirty) {
        NvU64 frameTimeUs = axb_div_c(pTimings->rasterSize.y * 1000ULL,
                                      pTimings->rasterSize.x, pTimings->pixelClock);
        NvU64 timePerLineNs = (frameTimeUs * 1000ULL) / pTimings->rasterSize.y;

        NvU64 maxFrameTimeUs = pTimings->vrr.timeoutMicroseconds;
        NvU64 maxVblankExtTimeNs = (maxFrameTimeUs - frameTimeUs) * 1000ULL;

        params.maxVblankExtension = maxVblankExtTimeNs / timePerLineNs;
    }

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDispEvo->pDevEvo->displayCommonHandle,
                         NV0073_CTRL_CMD_SYSTEM_CONFIG_VRR_PSTATE_SWITCH,
                         &params, sizeof(params));
    if (ret != NVOS_STATUS_SUCCESS) {
        nvEvoLogDispDebug(pDispEvo, EVO_LOG_WARN,
            "NV0073_CTRL_CMD_SYSTEM_CONFIG_VRR_PSTATE_SWITCH failed");
    }
}

static void SetStallLockOneDisp(NVDispEvoPtr pDispEvo, NvBool enable)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NvBool enableVrrOnHead[NVKMS_MAX_HEADS_PER_DISP];
    NVEvoUpdateState updateState = { };
    NvU32 head;

    if (enable) {
        for (head = 0; head < pDevEvo->numHeads; head++) {
            ConfigVrrPstateSwitch(pDispEvo, TRUE /* vrrEnabled */,
                                  TRUE /* vrrState */,
                                  FALSE/* vrrDirty */,
                                  head);
        }
    }

    nvPushEvoSubDevMaskDisp(pDispEvo);

    // Make sure any pending updates that we didn't wait for previously have
    // completed.
    nvRMSyncEvoChannel(pDevEvo, pDevEvo->core, __LINE__);

    for (head = 0; head < pDevEvo->numHeads; head++) {
        const NVDispHeadStateEvoRec *pHeadState = &pDispEvo->headState[head];
        const NvU32 timeout = pHeadState->timings.vrr.timeoutMicroseconds;

        enableVrrOnHead[head] = ((pHeadState->timings.vrr.type !=
                                    NVKMS_DPY_VRR_TYPE_NONE) && enable);

        nvEvoArmLightweightSupervisor(pDispEvo, head,
                                      enableVrrOnHead[head], TRUE);
        if (!enableVrrOnHead[head]) {
            ClearElvBlock(pDispEvo, head);
        }
        pDevEvo->hal->SetStallLock(pDispEvo, head,
                                   enableVrrOnHead[head],
                                   &updateState);

        if (pDevEvo->hal->caps.supportsDisplayRate) {
            pDevEvo->hal->SetDisplayRate(pDispEvo, head,
                                         enableVrrOnHead[head],
                                         &updateState,
                                         timeout);

            if ((pHeadState->timings.vrr.type !=
                        NVKMS_DPY_VRR_TYPE_NONE) &&
                    pHeadState->timings.vrr.needsSwFramePacing) {
                SetSwFramePacing(pDispEvo, head,
                                 enableVrrOnHead[head]);
            }
        }
    }

    nvEvoUpdateAndKickOff(pDispEvo, TRUE, &updateState,
                          TRUE /* releaseElv */);

    for (head = 0; head < pDevEvo->numHeads; head++) {
        nvEvoArmLightweightSupervisor(pDispEvo, head,
                                      enableVrrOnHead[head], FALSE);
    }

    nvPopEvoSubDevMask(pDevEvo);

    if (!enable) {
        for (head = 0; head < pDevEvo->numHeads; head++) {
            ConfigVrrPstateSwitch(pDispEvo, TRUE /* vrrEnabled */,
                                  FALSE /* vrrState */,
                                  FALSE /* vrrDirty */,
                                  head);
        }
    }
}

static void SetStallLockOneDev(NVDevEvoPtr pDevEvo, NvBool enable)
{
    NVDispEvoPtr pDispEvo;
    NvU32 dispIndex;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        SetStallLockOneDisp(pDispEvo, enable);
    }
}

/*!
 * Modify the VRR state to activate or deactivate VRR on the heads of a pDevEvo.
 */
static NvBool SetVrrActivePriv(NVDevEvoPtr pDevEvo, NvBool active)
{
    if (!pDevEvo->vrr.enabled ||
        pDevEvo->vrr.active == active) {
        return NV_TRUE;
    }

    // TODO: Drain the base channel first?
    SetStallLockOneDev(pDevEvo, active);

    pDevEvo->vrr.active = active;
    pDevEvo->vrr.flipCounter = 0;
    return NV_TRUE;
}

void nvSetVrrActive(NVDevEvoPtr pDevEvo, NvBool active)
{
    if (!SetVrrActivePriv(pDevEvo, active)) {
        nvDisableVrr(pDevEvo);
    }
}

/*!
 * Track the minimum and average time between flips over the last 16 flips, and
 * add a timestamp to delay the next flip to adjust Frame pacing if necessary.
 */
void nvTrackAndDelayFlipForVrrSwFramePacing(NVDispEvoPtr pDispEvo,
    const struct NvKmsVrrFramePacingInfo *pVrrFramePacingInfo,
    NVFlipChannelEvoHwState *pFlip)
{
    volatile NV0073_CTRL_RM_VRR_SHARED_DATA *pData = pVrrFramePacingInfo->pData;
    NvU32 retryCount = MAX_VRR_FLIP_DELAY_TIME_RETRY_COUNT;
    NvU64 flipTimeStamp = 0;
    NvU64 dataTimeStamp1 = 0, dataTimeStamp2 = 0;
    NvU32 expectedFrameNum = 0;
    NvBool bFlipTimeAdjustment = NV_FALSE;
    NvBool bCheckFlipTime = NV_FALSE;

    // If the RG interrupt isn't active, then SW Frame pacing isn't in use.
    if (!pVrrFramePacingInfo->framePacingActive) {
        return;
    }

    do {
        // read the data timestamp first
        dataTimeStamp1 = pData->dataTimeStamp;

        // now read the actual data required
        expectedFrameNum = pData->expectedFrameNum;
        bFlipTimeAdjustment = pData->bFlipTimeAdjustment;
        bCheckFlipTime = pData->bCheckFlipTime;
        flipTimeStamp = pData->flipTimeStamp;

        // read the data timestamp again to check if values were updated
        // by RM in between while nvkms was reading them.
        dataTimeStamp2 = pData->dataTimeStamp;
    } while ((dataTimeStamp1 != dataTimeStamp2) && --retryCount);

    if (retryCount == 0) {
        nvEvoLogDisp(pDispEvo, EVO_LOG_ERROR,
                    "Failed to sync with RM to get flipTimeStamp related data");
        return;
    }

    if (expectedFrameNum > 1) {
        pFlip->tearing = FALSE;
    }

    if (bFlipTimeAdjustment && !(pFlip->tearing && bCheckFlipTime)) {
        pFlip->timeStamp = flipTimeStamp;
    }
}

/*!
 * Override flip parameters for a head based on VRR state.
 */
void nvApplyVrrBaseFlipOverrides(const NVDispEvoRec *pDispEvo, NvU32 head,
                                 const NVFlipChannelEvoHwState *pOld,
                                 NVFlipChannelEvoHwState *pNew)
{
    const NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    const NVDispHeadStateEvoRec *pHeadState = NULL;

    if (!pDevEvo->vrr.enabled) {
        return;
    }

    if (head != NV_INVALID_HEAD) {
        pHeadState = &pDispEvo->headState[head];
        nvAssert(pDevEvo->head[head].layer[NVKMS_MAIN_LAYER]->caps.vrrTearingFlips);
    }

    // Tell RM the real requested tearing mode so that it can honor
    // __GL_SYNC_TO_VBLANK.
    pNew->vrrTearing = pNew->tearing;

    // If this head is driving non-VRR displays, force swap interval to be 0.
    // RM will block the flips as necessary using the pre-update trap methods
    // based on the vrrTearing flag above.
    if (pHeadState != NULL &&
            (pHeadState->timings.vrr.type == NVKMS_DPY_VRR_TYPE_NONE)) {
        pNew->tearing = TRUE;
        pNew->minPresentInterval = 0;
    }

    // If oneshot mode is in use, and the previous flip was non-tearing with
    // nonzero MIN_PRESENT_INTERVAL, and the new flip will be tearing with
    // MIN_PRESENT_INTERVAL 0, then force this first new flip to be
    // non-tearing to WAR bug 2406398 which causes these transitional flips
    // to stall for up to the display's minimum refresh rate.
    if ((pHeadState != NULL) &&
        (pHeadState->timings.vrr.type != NVKMS_DPY_VRR_TYPE_NONE) &&
        !pOld->tearing &&
        (pOld->minPresentInterval != 0) &&
        pNew->tearing &&
        (pNew->minPresentInterval == 0)) {
        pNew->tearing = FALSE;
    }
}

void nvCancelVrrFrameReleaseTimers(NVDevEvoPtr pDevEvo)
{
    NVDispEvoPtr pDispEvo;
    NvU32 dispIndex;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        nvkms_free_timer(pDispEvo->vrr.unstallTimer);
        pDispEvo->vrr.unstallTimer = NULL;
    }
}

/* Get active vrr type used by the flips. */
enum NvKmsVrrFlipType nvGetActiveVrrType(const NVDevEvoRec *pDevEvo)
{
    /*
     * If VRR is active, and any connected display is G-SYNC, then report that
     * this flip was a G-SYNC flip, otherwise report it as an Adaptive-Sync
     * flip.
     *
     * XXX NVKMS TODO: We could be smarter about reporting whether this flip
     * exclusively changed surfaces on Adaptive-Sync or G-SYNC heads.
     */
    if (pDevEvo->vrr.active) {
        if (AnyEnabledGsyncDpys(pDevEvo)) {
            return NV_KMS_VRR_FLIP_GSYNC;
        } else {
            return NV_KMS_VRR_FLIP_ADAPTIVE_SYNC;
        }
    }

    return NV_KMS_VRR_FLIP_NON_VRR;
}

/*!
 * Get the next VRR semaphore index to be released
 * by the client, increments the counter and handles wrapping.
 */
NvS32 nvIncVrrSemaphoreIndex(NVDevEvoPtr pDevEvo)
{
    NvS32 vrrSemaphoreIndex = -1;

    // If there are pending unstall timers (e.g. triggered by cursor motion),
    // cancel them now. The flip that was just requested will trigger an
    // unstall.
    nvCancelVrrFrameReleaseTimers(pDevEvo);

    if (pDevEvo->vrr.active && !pDevEvo->hal->caps.supportsDisplayRate) {
        vrrSemaphoreIndex = pDevEvo->vrr.flipCounter++;
        if (pDevEvo->vrr.flipCounter >= NVKMS_VRR_SEMAPHORE_SURFACE_COUNT) {
            pDevEvo->vrr.flipCounter = 0;
        }
    }

    return vrrSemaphoreIndex;
}

static void
VrrUnstallNow(NVDispEvoPtr pDispEvo)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NvU32 head;

    nvAssert(pDevEvo->hal->caps.supportsDisplayRate);

    for (head = 0; head < pDevEvo->numHeads; head++) {
        if (!nvHeadIsActive(pDispEvo, head)) {
            continue;
        }

        pDevEvo->cursorHal->ReleaseElv(pDevEvo, pDispEvo->displayOwner, head);
    }
}

static void
VrrUnstallTimer(void *dataPtr, NvU32 dataU32)
{
    NVDispEvoPtr pDispEvo = dataPtr;

    VrrUnstallNow(pDispEvo);
    pDispEvo->vrr.unstallTimer = NULL;
}

/*!
 * Schedule a timer to trigger a VRR unstall if no flip occurs soon.
 *
 * When VRR is active and something other than a flip (i.e. cursor motion)
 * changes the screen, RM needs to be notified so that it can trigger a VRR
 * unstall to present the new frame.  However, if it does that immediately, then
 * applications that flip in response to cursor motion will end up always
 * flipping during the unstall, causing stutter.  So instead, schedule a timeout
 * for some time in the future in order to give the application some time to
 * respond, but force a minimum refresh rate if it doesn't.
 *
 * On nvdisplay, this schedules an nvkms timer and uses a method to trigger an
 * unstall. On EVO, it calls into RM to do something equivalent.
 */
void nvTriggerVrrUnstallMoveCursor(NVDispEvoPtr pDispEvo)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    const NvU32 timeoutMs = 33; // 30 fps

    if (!pDevEvo->vrr.active) {
        return;
    }

    {
        if (!pDispEvo->vrr.unstallTimer) {
            pDispEvo->vrr.unstallTimer =
                nvkms_alloc_timer(VrrUnstallTimer, pDispEvo, 0, timeoutMs * 1000);
        }
    }
}

/*!
 * Trigger a VRR unstall in response to a cursor image change.
 */
void nvTriggerVrrUnstallSetCursorImage(NVDispEvoPtr pDispEvo,
                                       NvBool elvReleased)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;

    if (pDevEvo->vrr.active) {
        if (!elvReleased) {
            // On nvdisplay, no unstall is necessary if the cursor image update
            // path did a releaseElv=true Update.
            //
            // But, if elv was not released, then force an immediate unstall
            // now.
            VrrUnstallNow(pDispEvo);
        }
    }
}

void nvVrrSignalSemaphore(NVDevEvoPtr pDevEvo, NvS32 vrrSemaphoreIndex)
{
    NvU32* pVrrSemaphores = (NvU32*)pDevEvo->vrr.pSemaphores;

    if (!pDevEvo->vrr.pSemaphores) {
        return;
    }

    if (vrrSemaphoreIndex < 0) {
        return;
    }

    if (vrrSemaphoreIndex >= NVKMS_VRR_SEMAPHORE_SURFACE_COUNT) {
        return;
    }

    pVrrSemaphores[vrrSemaphoreIndex] = 1;
}
