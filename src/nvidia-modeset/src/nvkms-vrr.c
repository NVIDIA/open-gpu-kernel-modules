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

#include <ctrl/ctrl0000/ctrl0000unix.h>


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
 *  2. At flip time, for each apiHead, if pHeadState[apiHead].flip.allowVrr is true,
 *     VRR is "activated".
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
 *     pApiHeadState->vrr.active indicates whether VRR was activated successfully 
 *     on a particular apiHead at flip time.
 *
 *  3. Also at flip time, for each apiHead, if pHeadState[apiHead].flip.allowVrr is 
 *     false, VRR is "deactivated".
 *
 *     a. Stall lock is disabled.
 *     b. (NVDisplay only) the RG is switched from one-shot mode to continuous
 *        mode.
 *     c. (EVO only) RM's VRR state machine is suspended.
 */

static NvBool SetVrrActivePriv(NVDevEvoPtr pDevEvo,
                               const NvU32 applyAllowVrrApiHeadMasks[NVKMS_MAX_SUBDEVICES],
                               const NvU32 vrrActiveApiHeadMasks[NVKMS_MAX_SUBDEVICES]);

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
    return nvIsGsyncDpyVrrType(pDpyEvo->vrr.type);
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

static NvBool AnyActiveVrrHeads(const NVDevEvoRec *pDevEvo)
{
    NvU32 sd, apiHead;
    NVDispEvoPtr pDispEvo;

    for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
        pDispEvo = pDevEvo->gpus[sd].pDispEvo;

        for (apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
            if (!nvApiHeadIsActive(pDispEvo, apiHead)) {
                continue;
            }
            
            const NVDispApiHeadStateEvoRec *pApiHeadState =
                &pDispEvo->apiHeadState[apiHead];

            if (!pApiHeadState->vrr.active) {
                continue;
            }
            
            return NV_TRUE;
        }
    }
    return NV_FALSE;
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
    
    /*
    At this point, we have verified that we have a valid display, so
    nominalRefreshRateHz should be != 0. Assert so Coverity doesn't complain
    about potential divide by 0 later in the function.
    */
    nvAssert(nominalRefreshRateHz != 0);

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

        if (nvIsGsyncDpyVrrType(pDpyEvo->vrr.type)) {
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
    if (timeoutMicroseconds > 0 &&
        nvGetRefreshRate10kHz(pTimings) <
        (((NvU64) 1000000 * 11000) / timeoutMicroseconds)) {
        return;
    }

    /*
     * On G-SYNC panels, the back porch extension is used to indicate to
     * the monitor that VRR is enabled.  It is not necessary on
     * Adaptive-Sync displays.
     */
    if (nvIsGsyncDpyVrrType(vrrType)) {
        pTimings->rasterSize.y += 2;
        pTimings->rasterBlankEnd.y += 2;
        pTimings->rasterBlankStart.y += 2;
    }

    pTimings->vrr.timeoutMicroseconds = timeoutMicroseconds;
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

void nvDisableVrr(NVDevEvoPtr pDevEvo)
{
    NVDispEvoPtr pDispEvo;
    NvU32 head, dispIndex;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        for (head = 0; head < pDevEvo->numHeads; head++) {
            NVDispHeadStateEvoRec *pHeadState = &pDispEvo->headState[head];

            TellRMAboutVrrHead(pDispEvo, pHeadState, FALSE);
        }
    }

    if (!pDevEvo->vrr.enabled) {
        return;
    }

    // set vrr on all apiHeads to inactive
    NvU32 fullApiHeadMasks[NVKMS_MAX_SUBDEVICES];
    NvU32 emptyApiHeadMasks[NVKMS_MAX_SUBDEVICES];
    nvkms_memset(fullApiHeadMasks, 0xFF, sizeof(fullApiHeadMasks));
    nvkms_memset(emptyApiHeadMasks, 0, sizeof(emptyApiHeadMasks));

    SetVrrActivePriv(pDevEvo, fullApiHeadMasks, emptyApiHeadMasks);
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
    nvAssert(!AnyActiveVrrHeads(pDevEvo));
}

static NvBool AnyEnabledGsyncDpys(const NVDevEvoRec *pDevEvo)
{
    NVDispEvoPtr pDispEvo;
    NvU32 dispIndex;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        NvU32 head;

        for (head = 0; head < pDevEvo->numHeads; head++) {
            const NVDispHeadStateEvoRec *pHeadState = &pDispEvo->headState[head];

            if (nvIsGsyncDpyVrrType(pHeadState->timings.vrr.type)) {
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

static void SetStallLockOneDisp(NVDispEvoPtr pDispEvo, NvU32 applyAllowVrrApiHeadMask, 
                                NvU32 enableApiHeadMask)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NvBool enableVrrOnHead[NVKMS_MAX_HEADS_PER_DISP];
    NVEvoUpdateState updateState = { };
    NvU32 apiHead, head;

    for (apiHead = 0; apiHead < NVKMS_MAX_HEADS_PER_DISP; apiHead++) {
        if (!(applyAllowVrrApiHeadMask & (1 << apiHead))) {
            continue;
        }
        NvBool enable = enableApiHeadMask & (1 << apiHead);
        if (!enable) {
            continue;
        }
        FOR_EACH_EVO_HW_HEAD_IN_MASK(pDispEvo->apiHeadState[apiHead].hwHeadsMask, head) {
            // ignores inactive heads
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

    for (apiHead = 0; apiHead < NVKMS_MAX_HEADS_PER_DISP; apiHead++) {
        if (!(applyAllowVrrApiHeadMask & (1 << apiHead))) {
            continue;
        }

        NvBool enable = enableApiHeadMask & (1 << apiHead);

        FOR_EACH_EVO_HW_HEAD_IN_MASK(pDispEvo->apiHeadState[apiHead].hwHeadsMask, head) {
            const NVDispHeadStateEvoRec *pHeadState = &pDispEvo->headState[head];
            const NvU32 timeout = pHeadState->timings.vrr.timeoutMicroseconds;

            enableVrrOnHead[head] = ((pHeadState->timings.vrr.type !=
                                        NVKMS_DPY_VRR_TYPE_NONE) && enable);

            // ignores inactive heads
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
            }
        }
    }

    nvEvoUpdateAndKickOff(pDispEvo, TRUE, &updateState,
                          TRUE /* releaseElv */);


    for (apiHead = 0; apiHead < NVKMS_MAX_HEADS_PER_DISP; apiHead++) {
        if (!(applyAllowVrrApiHeadMask & (1 << apiHead))) {
            continue;
        }
        FOR_EACH_EVO_HW_HEAD_IN_MASK(pDispEvo->apiHeadState[apiHead].hwHeadsMask, head) {
            nvEvoArmLightweightSupervisor(pDispEvo, head,
                                          enableVrrOnHead[head], FALSE);
        }
    }

    nvPopEvoSubDevMask(pDevEvo);

    for (apiHead = 0; apiHead < NVKMS_MAX_HEADS_PER_DISP; apiHead++) {
        if (!(applyAllowVrrApiHeadMask & (1 << apiHead))) {
            continue;
        }
        NvBool enable = enableApiHeadMask & (1 << apiHead);
        if (enable) {
            continue;
        }
        FOR_EACH_EVO_HW_HEAD_IN_MASK(pDispEvo->apiHeadState[apiHead].hwHeadsMask, head) {
            ConfigVrrPstateSwitch(pDispEvo, TRUE /* vrrEnabled */,
                                    FALSE /* vrrState */,
                                    FALSE/* vrrDirty */,
                                    head);
        }
    }
}

static void SetStallLockOneDev(NVDevEvoPtr pDevEvo, 
                               const NvU32 applyAllowVrrApiHeadMasks[NVKMS_MAX_SUBDEVICES], 
                               const NvU32 enableApiHeadMasks[NVKMS_MAX_SUBDEVICES])
{
    NVDispEvoPtr pDispEvo;
    NvU32 dispIndex;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        SetStallLockOneDisp(pDispEvo, 
                            applyAllowVrrApiHeadMasks[dispIndex], enableApiHeadMasks[dispIndex]);
    }
}

/*!
 * Modify the VRR state to activate or deactivate VRR on the heads of a pDevEvo.
 */
static NvBool SetVrrActivePriv(NVDevEvoPtr pDevEvo,
                               const NvU32 applyAllowVrrApiHeadMasks[NVKMS_MAX_SUBDEVICES],
                               const NvU32 vrrActiveApiHeadMasks[NVKMS_MAX_SUBDEVICES])
{
    NvU32 sd, apiHead;
    NvU32 currVrrActiveApiHeadMasks[NVKMS_MAX_SUBDEVICES];
    NvBool isUpdate;
    NVDispEvoPtr pDispEvo;

    nvkms_memset(currVrrActiveApiHeadMasks, 0, sizeof(currVrrActiveApiHeadMasks));
    isUpdate = NV_FALSE;
    
    if (!pDevEvo->vrr.enabled) {
        return NV_TRUE;
    }

    for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
        pDispEvo = pDevEvo->pDispEvo[sd];
        for (apiHead = 0; apiHead < NVKMS_MAX_HEADS_PER_DISP; apiHead++) {
            if (pDispEvo->apiHeadState[apiHead].vrr.active) {
                currVrrActiveApiHeadMasks[sd] |= (1 << apiHead);
            }
        }
    }

    // check if we are asking to update the existing activeMasks
    for (sd = 0; sd < pDevEvo->numSubDevices; sd++){
        for (apiHead = 0; apiHead < NVKMS_MAX_HEADS_PER_DISP; apiHead++) {
            if (!(applyAllowVrrApiHeadMasks[sd] & (1 << apiHead))) {
                continue;
            }
            if ((vrrActiveApiHeadMasks[sd] & (1 << apiHead)) != 
                (currVrrActiveApiHeadMasks[sd] & (1 << apiHead))) {
                isUpdate = NV_TRUE;
                break;
            }
        }
    }

    if (!isUpdate) {
        return NV_TRUE;
    }

    SetStallLockOneDev(pDevEvo, applyAllowVrrApiHeadMasks, 
                       vrrActiveApiHeadMasks);

    for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
        pDispEvo = pDevEvo->gpus[sd].pDispEvo;
        for (apiHead = 0; apiHead < NVKMS_MAX_HEADS_PER_DISP; apiHead++) {
            if (!(applyAllowVrrApiHeadMasks[sd] & (1 << apiHead))) {
                continue;
            }
            pDispEvo->apiHeadState[apiHead].vrr.active = 
                (vrrActiveApiHeadMasks[sd] & (1 << apiHead)) > 0;
        }
    }
    pDevEvo->vrr.flipCounter = 0;
    return NV_TRUE;
}

void nvSetVrrActive(NVDevEvoPtr pDevEvo,
                    const NvU32 applyAllowVrrApiHeadMasks[NVKMS_MAX_SUBDEVICES],
                    const NvU32 vrrActiveApiHeadMasks[NVKMS_MAX_SUBDEVICES])
{
    if (!SetVrrActivePriv(pDevEvo, applyAllowVrrApiHeadMasks, 
                          vrrActiveApiHeadMasks)) {
        nvDisableVrr(pDevEvo);
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

void nvCancelVrrFrameReleaseTimers(NVDevEvoPtr pDevEvo,
                                   const NvU32 applyAllowVrrApiHeadMasks[NVKMS_MAX_SUBDEVICES])
{
    NVDispEvoPtr pDispEvo;
    NvU32 dispIndex, apiHead;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        NvBool pendingCursorMotionUnflipped = NV_FALSE;
        NvU32 applyAllowVrrApiHeadMask = applyAllowVrrApiHeadMasks[dispIndex];

        for (apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
            if (!(applyAllowVrrApiHeadMask & (1 << apiHead))) {
                // This apiHead is not currently flipping:
                // Check if this apiHead is pending cursor motion;
                // if so, we don't want to cancel the unstall timer yet, 
                // as the cursor on this apiHead could then freeze up
                pendingCursorMotionUnflipped = (pendingCursorMotionUnflipped) || 
                                               (pDispEvo->apiHeadState[apiHead].vrr.pendingCursorMotion);
            } else {
                // clear pendingCursorMotion for all flipped apiHeads
                pDispEvo->apiHeadState[apiHead].vrr.pendingCursorMotion = NV_FALSE;
            }
        }
        
        if (!pendingCursorMotionUnflipped) {
            nvkms_free_timer(pDispEvo->vrr.unstallTimer);
            pDispEvo->vrr.unstallTimer = NULL;
        }
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
    if (AnyActiveVrrHeads(pDevEvo)) {
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
NvS32 nvIncVrrSemaphoreIndex(NVDevEvoPtr pDevEvo, 
                             const NvU32 applyAllowVrrApiHeadMasks[NVKMS_MAX_SUBDEVICES])
{
    NvS32 vrrSemaphoreIndex = -1;

    // If there are pending unstall timers (e.g. triggered by cursor motion),
    // cancel them now. The flip that was just requested will trigger an
    // unstall.
    // NOTE: This call will not cancel the frame release timer in
    // the case where there is a vrr active head that is pending cursor motion
    // and not currently flipping, since we need to wait for the timer for that head
    nvCancelVrrFrameReleaseTimers(pDevEvo, applyAllowVrrApiHeadMasks);

    if (AnyActiveVrrHeads(pDevEvo) && !pDevEvo->hal->caps.supportsDisplayRate) {
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
    NvU32 apiHead, head;

    nvAssert(pDevEvo->hal->caps.supportsDisplayRate);

    for (apiHead = 0; apiHead < NVKMS_MAX_HEADS_PER_DISP; apiHead++) {
        if (!pDispEvo->apiHeadState[apiHead].vrr.pendingCursorMotion) {
            continue;
        }
        FOR_EACH_EVO_HW_HEAD_IN_MASK (pDispEvo->apiHeadState[apiHead].hwHeadsMask, head) {
            if (!nvHeadIsActive(pDispEvo, head)) {
                continue;
            }
            pDevEvo->cursorHal->ReleaseElv(pDevEvo, pDispEvo->displayOwner, head);
        }
        pDispEvo->apiHeadState[apiHead].vrr.pendingCursorMotion = NV_FALSE;
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

    NvU32 apiHead;

    if (!AnyActiveVrrHeads(pDevEvo)) {
        return;
    }

    {
        if (!pDispEvo->vrr.unstallTimer) {
            for (apiHead = 0; apiHead < NVKMS_MAX_HEADS_PER_DISP; apiHead++) {
                if (pDispEvo->apiHeadState[apiHead].vrr.active) {
                    pDispEvo->apiHeadState[apiHead].vrr.pendingCursorMotion = NV_TRUE;
                }
            }
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

    if (AnyActiveVrrHeads(pDevEvo)) {
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
