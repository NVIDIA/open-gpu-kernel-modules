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

NvBool nvDispSupportsVrr(
    const NVDispEvoRec *pDispEvo)
{
    return FALSE;
}

void nvDisableVrr(NVDevEvoPtr pDevEvo)
{
    return;
}

void nvGetDpyMinRefreshRateValidValues(
    const NVHwModeTimingsEvo *pTimings,
    const enum NvKmsDpyVRRType vrrType,
    const NvU32 edidTimeoutMicroseconds,
    NvU32 *minMinRefreshRate,
    NvU32 *maxMinRefreshRate)
{
    return;
}

void nvEnableVrr(
    NVDevEvoPtr pDevEvo,
    const struct NvKmsSetModeRequest *pRequest)
{
    return;
}

void nvSetVrrActive(
    NVDevEvoPtr pDevEvo,
    NvBool active)
{
    return;
}

void nvApplyVrrBaseFlipOverrides(
    const NVDispEvoRec *pDispEvo,
    NvU32 head,
    const NVFlipChannelEvoHwState *pOld,
    NVFlipChannelEvoHwState *pNew)
{
    return;
}

void nvCancelVrrFrameReleaseTimers(
    NVDevEvoPtr pDevEvo)
{
    return;
}

void nvSetNextVrrFlipTypeAndIndex(
    NVDevEvoPtr pDevEvo,
    struct NvKmsFlipReply *reply)
{
    return;
}

void nvTriggerVrrUnstallMoveCursor(
    NVDispEvoPtr pDispEvo)
{
    return;
}

void nvTriggerVrrUnstallSetCursorImage(
    NVDispEvoPtr pDispEvo,
    NvBool ctxDmaChanged)
{
    return;
}

