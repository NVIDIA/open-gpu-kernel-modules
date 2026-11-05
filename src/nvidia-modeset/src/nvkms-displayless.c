/*
 * SPDX-FileCopyrightText: Copyright (c) 2010-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * This file contains implementations of the EVO HAL methods for the
 * GRID displayless class
 */

#include "nvkms-dma.h"
#include "nvkms-evo.h"
#include "nvkms-evo1.h"
#include "nvkms-evo3.h"
#include "nvkms-private.h"
#include "nvkms-surface.h"
#include "nvkms-sync.h"

#include "nvkms-rm.h"
#include "nvkms-rmapi.h"
#include <class/cla080.h> /* KEPLER_DEVICE_VGPU */
#include <ctrl/ctrla080.h> /* NVA080_CTRL_CMD_VGPU_DISPLAY_* */

#include <class/cla083.h>

static NvU32 DisplaylessAllocSurface(
    NVDevEvoPtr pDevEvo, NVSurfaceDescriptor *pSurfaceDesc,
    NvU32 memoryHandle, NvU32 localCtxDmaFlags,
    NvU64 limit, NvBool mapToDisplayRm)
{
    return 0;
}

static void DisplaylessFreeSurfaceDescriptor(
    NVDevEvoPtr pDevEvo,
    NvU32 deviceHandle,
    NVSurfaceDescriptor *pSurfaceDesc)
{
}

static NvU32 DisplaylessBindSurfaceDescriptor(
    NVDevEvoPtr pDevEvo, NVEvoChannelPtr pChannel,
    NVSurfaceDescriptor *pSurfaceDesc)
{
    return 0;
}

static void DisplaylessSetRasterParams(
    NVDevEvoPtr pDevEvo, int head,
    const NVHwModeTimingsEvo *pTimings,
    const NvU8 tilePosition,
    const NVDscInfoEvoRec *pDscInfo,
    const NVEvoColorRec *pOverscanColor,
    NVEvoUpdateState *updateState)
{
}

static void DisplaylessSetProcAmp(
    NVDispEvoPtr pDispEvo, const NvU32 head,
    NVEvoUpdateState *updateState)
{
}

static void DisplaylessSetHeadControl(
    NVDevEvoPtr pDevEvo, int sd, int head,
    NVEvoUpdateState *updateState)
{
}

static void DisplaylessSetHeadRefClk(
    NVDevEvoPtr pDevEvo, int head, NvBool external,
    NVEvoUpdateState *updateState)
{
}

static void DisplaylessHeadSetControlOR(
    NVDevEvoPtr pDevEvo,
    const int head,
    const NVHwModeTimingsEvo *pTimings,
    const enum nvKmsPixelDepth pixelDepth,
    const NvBool colorSpaceOverride,
    NVEvoUpdateState *updateState)
{
}

static void DisplaylessORSetControl(
    NVDevEvoPtr pDevEvo,
    const NVConnectorEvoRec *pConnectorEvo,
    const enum nvKmsTimingsProtocol protocol,
    const NvU32 orIndex,
    const NvU32 headMask,
    NVEvoUpdateState *updateState)
{
}

static void DisplaylessHeadSetDisplayId(
    NVDevEvoPtr pDevEvo,
    const NvU32 head, const NvU32 displayId,
    NVEvoUpdateState *updateState)
{
}

static NvBool DisplaylessSetUsageBounds(
    NVDevEvoPtr pDevEvo, NvU32 sd, NvU32 head,
    const struct NvKmsUsageBounds *pUsage,
    NVEvoUpdateState *updateState)
{
    return true;
}

static void DisplaylessUpdate(
    NVDevEvoPtr pDevEvo,
    const NVEvoUpdateState *updateState,
    NvBool releaseElv)
{
}

static void DisplaylessIsModePossible(
    NVDispEvoPtr pDispEvo,
    const NVEvoIsModePossibleDispInput *pInput,
    NVEvoIsModePossibleDispOutput *pOutput)
{
    if (pOutput)
        pOutput->possible = TRUE;
}

static void DisplaylessSetNotifier(
    NVDevEvoRec *pDevEvo,
    const NvBool notify,
    const NvBool awaken,
    const NvU32 notifier,
    NVEvoUpdateState *updateState)
{
}

static NvBool DisplaylessGetCapabilities(NVDevEvoPtr pDevEvo)
{
    NvU8 layer;

    pDevEvo->caps.cursorCompositionCaps =
        (struct NvKmsCompositionCapabilities) {
            .supportedColorKeySelects =
                NVBIT(NVKMS_COMPOSITION_COLOR_KEY_SELECT_DISABLE),

            .colorKeySelect = {
                [NVKMS_COMPOSITION_COLOR_KEY_SELECT_DISABLE] = {
                    .supportedBlendModes = {
                        [1] = (1 << NVKMS_COMPOSITION_BLENDING_MODE_PREMULT_ALPHA),
                    },
                },
            }
        };

    for (layer = 0;
         layer < ARRAY_LEN(pDevEvo->caps.layerCaps);
         layer++) {
        pDevEvo->caps.layerCaps[layer].supportedSurfaceMemoryFormats = ~0ULL;

        pDevEvo->caps.layerCaps[layer].composition =
            (struct NvKmsCompositionCapabilities) {
                .supportedColorKeySelects =
                    NVBIT(NVKMS_COMPOSITION_COLOR_KEY_SELECT_DISABLE),

                .colorKeySelect = {
                    [NVKMS_COMPOSITION_COLOR_KEY_SELECT_DISABLE] = {
                        .supportedBlendModes = {
                            [1] = (1 << NVKMS_COMPOSITION_BLENDING_MODE_OPAQUE),
                        },
                    },
                },
            };
    }

    pDevEvo->caps.validLayerRRTransforms = 0xff;

    pDevEvo->numWindows = pDevEvo->numHeads;
    return true;
}

static NvU32 GetSemaphoreIndex(
    const NVFlipNIsoSurfaceEvoHwState *pSemaSurface)
{
    const NvU32 offsetInBytes = pSemaSurface->offsetInWords * 4;
    const NvU32 sizeOfSemaphore = nvKmsSizeOfSemaphore();

    nvAssert(sizeOfSemaphore > 0);
    nvAssert((offsetInBytes % sizeOfSemaphore) == 0);

    return offsetInBytes / sizeOfSemaphore;
}

static inline NvBool QueueIsEmpty(NvU8 head, NvU8 tail)
{
    return head == tail;
}

static inline NvBool QueueIsFull(NvU8 head, NvU8 tail)
{
    return ((head + 1) % DISPLAYLESS_MAX_PENDING_FLIPS) == tail;
}

static void ReleaseLastSemaphore(NVDevEvoPtr pDevEvo, NvU32 apiHead)
{
    NVFlipNIsoSurfaceEvoHwState *pLastSemaSurface =
        &pDevEvo->displaylessWorker.head[apiHead].lastSemaSurface;
    NvU32 *pLastReleaseValue =
        &pDevEvo->displaylessWorker.head[apiHead].lastReleaseValue;

    if (pLastSemaSurface->pSurfaceEvo != NULL) {
        void *pCpuAddress = pLastSemaSurface->pSurfaceEvo->cpuAddress[0];
        const NvU32 semaIndex = GetSemaphoreIndex(pLastSemaSurface);

        nvKmsResetSemaphore(semaIndex, pCpuAddress, *pLastReleaseValue);
        nvEvoDecrementSurfaceRefCnts(pDevEvo, pLastSemaSurface->pSurfaceEvo);
    }
}

static void SetLastSemaphore(
    NVDevEvoPtr pDevEvo,
    NVFlipNIsoSurfaceEvoHwState *pSemaSurface,
    NvU32 releaseValue,
    NvU32 apiHead)
{
    pDevEvo->displaylessWorker.head[apiHead].lastSemaSurface = *pSemaSurface;
    pDevEvo->displaylessWorker.head[apiHead].lastReleaseValue = releaseValue;
}

static void ReleaseLastScanoutSurface(NVDevEvoPtr pDevEvo, NvU32 apiHead)
{
    NVSurfaceEvoPtr pScanoutSurface =
        pDevEvo->displaylessWorker.head[apiHead].lastScanoutSurface;

    if (pScanoutSurface != NULL) {
        nvEvoDecrementSurfaceRefCnts(pDevEvo, pScanoutSurface);
        pDevEvo->displaylessWorker.head[apiHead].lastScanoutSurface = NULL;
    }
}

static NvBool MapFormatToVgpuFormat(enum NvKmsSurfaceMemoryFormat format,
                                    NvU32 *pVgpuFormat)
{
    switch (format) {
        case NvKmsSurfaceMemoryFormatI8:
            *pVgpuFormat = NVA080_CTRL_VGPU_DISPLAY_SET_SURFACE_PROPERTIES_FORMAT_I8;
            return TRUE;
        case NvKmsSurfaceMemoryFormatA1R5G5B5:
        case NvKmsSurfaceMemoryFormatX1R5G5B5:
            *pVgpuFormat = NVA080_CTRL_VGPU_DISPLAY_SET_SURFACE_PROPERTIES_FORMAT_A1R5G5B5;
            return TRUE;
        case NvKmsSurfaceMemoryFormatR5G6B5:
            *pVgpuFormat = NVA080_CTRL_VGPU_DISPLAY_SET_SURFACE_PROPERTIES_FORMAT_R5G6B5;
            return TRUE;
        case NvKmsSurfaceMemoryFormatA8R8G8B8:
        case NvKmsSurfaceMemoryFormatX8R8G8B8:
            *pVgpuFormat = NVA080_CTRL_VGPU_DISPLAY_SET_SURFACE_PROPERTIES_FORMAT_A8R8G8B8;
            return TRUE;
        case NvKmsSurfaceMemoryFormatA8B8G8R8:
        case NvKmsSurfaceMemoryFormatX8B8G8R8:
            *pVgpuFormat = NVA080_CTRL_VGPU_DISPLAY_SET_SURFACE_PROPERTIES_FORMAT_A8B8G8R8;
            return TRUE;
        case NvKmsSurfaceMemoryFormatA2B10G10R10:
        case NvKmsSurfaceMemoryFormatX2B10G10R10:
            *pVgpuFormat = NVA080_CTRL_VGPU_DISPLAY_SET_SURFACE_PROPERTIES_FORMAT_A2B10G10R10;
            return TRUE;
        case NvKmsSurfaceMemoryFormatRF16GF16BF16AF16:
            *pVgpuFormat = NVA080_CTRL_VGPU_DISPLAY_SET_SURFACE_PROPERTIES_FORMAT_RF16_GF16_BF16_AF16;
            return TRUE;
        case NvKmsSurfaceMemoryFormatR16G16B16A16:
            *pVgpuFormat = NVA080_CTRL_VGPU_DISPLAY_SET_SURFACE_PROPERTIES_FORMAT_R16_G16_B16_A16;
            return TRUE;
        default:
            return FALSE;
    }
}

static NvBool BuildVncSetSurfaceParams(
    NVDevEvoPtr pDevEvo,
    const NVSurfaceEvoRec *pSurfaceEvo,
    NvU32 head,
    NVA080_CTRL_VGPU_DISPLAY_SET_SURFACE_PROPERTIES *pParams)
{
    const NvKmsSurfaceMemoryFormatInfo *pFormatInfo;
    const NVHwModeViewPortEvo *pViewPort;
    NvU32 vgpuFormat;
    NvU32 surfacePitch = pSurfaceEvo->planes[0].pitch;

    pFormatInfo = nvKmsGetSurfaceMemoryFormatInfo(pSurfaceEvo->format);
    if ((pFormatInfo == NULL) || (pFormatInfo->numPlanes != 1)) {
        return FALSE;
    }

    if (!MapFormatToVgpuFormat(pSurfaceEvo->format, &vgpuFormat)) {
        return FALSE;
    }

    if (pSurfaceEvo->layout != NvKmsSurfaceMemoryLayoutBlockLinear) {
        if ((surfacePitch & 0xffU) != 0) {
            return FALSE;
        }

        surfacePitch >>= 8;
    }

    pViewPort = &pDevEvo->pDispEvo[0]->headState[head].timings.viewPort;

    nvkms_memset(pParams, 0, sizeof(*pParams));
    pParams->headIndex          = 0;
    pParams->isPrimary          = 1;
    pParams->hMemory            = pSurfaceEvo->planes[0].rmHandle;
    pParams->offset             = (NvU32)pSurfaceEvo->planes[0].offset;
    pParams->surfaceType        =
        (pSurfaceEvo->layout == NvKmsSurfaceMemoryLayoutBlockLinear) ?
        NVA080_CTRL_VGPU_DISPLAY_SET_SURFACE_PROPERTIES_MEMORY_LAYOUT_BLOCKLINEAR :
        NVA080_CTRL_VGPU_DISPLAY_SET_SURFACE_PROPERTIES_MEMORY_LAYOUT_PITCH;
    pParams->surfaceBlockHeight = pSurfaceEvo->log2GobsPerBlockY;
    pParams->surfacePitch       = surfacePitch;
    pParams->surfaceFormat      = vgpuFormat;
    pParams->surfaceWidth       = pSurfaceEvo->widthInPixels;
    pParams->surfaceHeight      = pSurfaceEvo->heightInPixels;
    pParams->rectWidth          = pViewPort->in.width;
    pParams->rectHeight         = pViewPort->in.height;
    pParams->surfaceSize        = (NvU32)pSurfaceEvo->planes[0].rmObjectSizeInBytes;

    return TRUE;
}

static NvBool IssueVncSetSurfaceProperties(
    NVDevEvoPtr pDevEvo,
    NVA080_CTRL_VGPU_DISPLAY_SET_SURFACE_PROPERTIES *pParams)
{
    NvU32 ret;

    if (pDevEvo->displaylessWorker.hVgpuDevice == 0) {
        return FALSE;
    }

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->displaylessWorker.hVgpuDevice,
                         NVA080_CTRL_CMD_VGPU_DISPLAY_SET_SURFACE_PROPERTIES,
                         pParams, sizeof(*pParams));
    if (ret != NVOS_STATUS_SUCCESS) {
        nvEvoLogDev(pDevEvo, EVO_LOG_WARN,
            "NVA080_CTRL_CMD_VGPU_DISPLAY_SET_SURFACE_PROPERTIES "
            "failed on apiHead %u (0x%x)", pParams->headIndex, ret);
        return FALSE;
    }

    return TRUE;
}

static void ConsumeScanoutSurface(
    NVDevEvoPtr pDevEvo,
    NVDispEvoPtr pDispEvo,
    NVSurfaceEvoPtr pScanoutSurface)
{
    NVA080_CTRL_VGPU_DISPLAY_SET_SURFACE_PROPERTIES params;
    NvU32 head;

    if (pScanoutSurface == NULL) {
        return;
    }

    head = nvGetPrimaryHwHead(pDispEvo, 0);
    if ((head == NV_INVALID_HEAD) ||
        !BuildVncSetSurfaceParams(pDevEvo, pScanoutSurface, head, &params) ||
        !IssueVncSetSurfaceProperties(pDevEvo, &params)) {
        nvEvoDecrementSurfaceRefCnts(pDevEvo, pScanoutSurface);
        return;
    }

    /*
     * The vGPU plugin caches the surface we just told it about and keeps blitting
     * from it across compositor ticks until the next SET supersedes it. Hold a
     * reference until then (or until teardown) so the surface can't be freed
     * out from under the plugin.
     */
    ReleaseLastScanoutSurface(pDevEvo, 0);
    pDevEvo->displaylessWorker.head[0].lastScanoutSurface =
        pScanoutSurface;
}

static void CoalesceFlipScanoutSurface(
    NVDevEvoPtr pDevEvo,
    NVSurfaceEvoPtr *ppScanoutSurface,
    NVDisplaylessPendingFlipRec *pFlip)
{
    if (*ppScanoutSurface != NULL) {
        nvEvoDecrementSurfaceRefCnts(pDevEvo, *ppScanoutSurface);
    }

    *ppScanoutSurface = pFlip->pScanoutSurface;
    pFlip->pScanoutSurface = NULL;
}

static void SetPendingFlipScanoutSurface(
    NVDisplaylessPendingFlipRec *pFlip,
    NVSurfaceEvoPtr pScanoutSurface)
{
    pFlip->pScanoutSurface = pScanoutSurface;
    if (pScanoutSurface != NULL) {
        nvEvoIncrementSurfaceRefCnts(pScanoutSurface);
    }
}

static NVSurfaceEvoPtr GetConsoleVncScanoutSurface(
    NVDevEvoPtr pDevEvo,
    NvU32 apiHead,
    NvU32 head,
    NVEvoChannelPtr pChannel,
    const NVFlipChannelEvoHwState *pHwState)
{
    if ((apiHead == 0) &&
        (pDevEvo->displaylessWorker.hVgpuDevice != 0) &&
        (pChannel == pDevEvo->head[head].layer[NVKMS_MAIN_LAYER])) {
        return pHwState->pSurfaceEvo[NVKMS_LEFT];
    }

    return NULL;
}

static NvBool ProcessPendingFlips(
    NVDevEvoPtr pDevEvo,
    NVDispEvoPtr pDispEvo,
    NvBool skipWaitingForSema)
{
    NvBool anyPending = FALSE;
    NvU32 apiHead;

    for (apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
        NvU8 *pHead = &pDevEvo->displaylessWorker.head[apiHead].head;
        NvU8 *pTail = &pDevEvo->displaylessWorker.head[apiHead].tail;
        /*
         * The vGPU plugin consumes SET asynchronously, so only expose the
         * newest scanout from this worker pass and release intermediate
         * candidates.
         */
        NVSurfaceEvoPtr pVncScanoutSurface = NULL;

        /* Consume flips from tail until empty or blocked */
        while (!QueueIsEmpty(*pHead, *pTail)) {
            NVDisplaylessPendingFlipRec *pFlip =
                &pDevEvo->displaylessWorker.head[apiHead].q[*pTail];

            if (!pFlip->usingSema) {
                CoalesceFlipScanoutSurface(pDevEvo, &pVncScanoutSurface, pFlip);
                nvSendFlipOccurredEventEvo(pDispEvo, apiHead, NVKMS_MAIN_LAYER);
            } else {
                void *pCpuAddress = pFlip->semaSurface.pSurfaceEvo->cpuAddress[0];
                const NvU32 semaIndex = GetSemaphoreIndex(&pFlip->semaSurface);
                struct nvKmsParsedSemaphore parsedSema = { };

                nvKmsParseSemaphore(semaIndex, pCpuAddress, &parsedSema);

                if (skipWaitingForSema || parsedSema.payload == pFlip->acquireValue) {
                    /*
                     * The flip acquire has been satisfied. To match
                     * display hardware behavior, we release the semaphore
                     * when we flip *away*. So, release the previously
                     * recorded semaphore, and record this semaphore so
                     * that it can be released on the next flip.
                     */
                    ReleaseLastSemaphore(pDevEvo, apiHead);
                    SetLastSemaphore(pDevEvo, &pFlip->semaSurface, pFlip->releaseValue, apiHead);
                    CoalesceFlipScanoutSurface(pDevEvo, &pVncScanoutSurface, pFlip);
                    nvSendFlipOccurredEventEvo(pDispEvo, apiHead, NVKMS_MAIN_LAYER);
                } else {
                    /* Blocked: stop processing this head */
                    anyPending = TRUE;
                    break;
                }
            }

            *pTail = (*pTail + 1) % DISPLAYLESS_MAX_PENDING_FLIPS;
        }

        ConsumeScanoutSurface(pDevEvo, pDispEvo, pVncScanoutSurface);

        /*
         * During teardown (skipWaitingForSema=true), the above loop
         * through the queue will have forced all flips and released all
         * semaphores except for the last semaphore. Release the last
         * semaphore now.
         */
        if (skipWaitingForSema) {
            ReleaseLastSemaphore(pDevEvo, apiHead);
        }
    }

    return anyPending;
}

#define DISPLAYLESS_POLL_INTERVAL_USEC 100

static void DisplaylessFlipWorker(void *dataPtr, NvU32 dataU32)
{
    NVDevEvoPtr pDevEvo = dataPtr;
    const NVDispEvoPtr pDispEvo = pDevEvo->pDispEvo[0];
    NvBool anyPending;

    anyPending = ProcessPendingFlips(pDevEvo, pDispEvo, FALSE /* skipWaitingForSema */);

    nvkms_free_timer(pDevEvo->displaylessWorker.timer);

    if (anyPending) {
        /* Re-arm the timer to poll on pending flips */
        pDevEvo->displaylessWorker.timer =
            nvkms_alloc_timer(
                DisplaylessFlipWorker,
                pDevEvo,
                0, /* dataU32 */
                DISPLAYLESS_POLL_INTERVAL_USEC); /* timeout */
    } else {
        pDevEvo->displaylessWorker.timer = NULL;
    }
}

static void DisplaylessEnqueueFlip(
    NVDevEvoPtr pDevEvo,
    NvU32 apiHead,
    NvBool usingSyncpt,
    const NVFlipNIsoSurfaceEvoHwState *pSemaSurface,
    NvU32 acquireValue,
    NvU32 releaseValue,
    NVSurfaceEvoPtr pScanoutSurface)
{
    NVDisplaylessPendingFlipRec *pFlip;
    NvU8 *pHead, *pTail;

    pHead = &pDevEvo->displaylessWorker.head[apiHead].head;
    pTail = &pDevEvo->displaylessWorker.head[apiHead].tail;

    if (QueueIsFull(*pHead, *pTail)) {
        nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
            "Displayless flip queue is full for apiHead %u", apiHead);
        return;
    }

    pFlip = &pDevEvo->displaylessWorker.head[apiHead].q[*pHead];
    nvkms_memset(pFlip, 0, sizeof(*pFlip));

    if (!usingSyncpt && pSemaSurface->pSurfaceEvo != NULL) {
        if (pSemaSurface->pSurfaceEvo->cpuAddress[0] == NULL) {
            nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                "Semaphore surface without CPU mapping!");
            return;
        }

        nvEvoIncrementSurfaceRefCnts(pSemaSurface->pSurfaceEvo);

        pFlip->usingSema = TRUE;
        pFlip->acquireValue = acquireValue;
        pFlip->releaseValue = releaseValue;
        pFlip->semaSurface.pSurfaceEvo = pSemaSurface->pSurfaceEvo;
        pFlip->semaSurface.format = pSemaSurface->format;
        pFlip->semaSurface.offsetInWords = pSemaSurface->offsetInWords;
    } else {
        pFlip->usingSema = FALSE;
        pFlip->semaSurface.pSurfaceEvo = NULL;
    }

    SetPendingFlipScanoutSurface(pFlip, pScanoutSurface);

    *pHead = (*pHead + 1) % DISPLAYLESS_MAX_PENDING_FLIPS;

    if (pDevEvo->displaylessWorker.timer == NULL) {
        pDevEvo->displaylessWorker.timer =
            nvkms_alloc_timer(
                DisplaylessFlipWorker,
                pDevEvo,
                0,   /* dataU32 */
                0);  /* timeout: schedule the work immediately */
    }
}

static void DisplaylessCleanupConsoleVnc(NVDevEvoPtr pDevEvo, NvU32 apiHead)
{
    NVA080_CTRL_VGPU_DISPLAY_CLEANUP_SURFACE_PARAMS params = { 0 };
    NvU32 ret;

    if (pDevEvo->displaylessWorker.hVgpuDevice == 0) {
        return;
    }

    params.headIndex = apiHead;
    params.blankingEnabled = 0;

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->displaylessWorker.hVgpuDevice,
                         NVA080_CTRL_CMD_VGPU_DISPLAY_CLEANUP_SURFACE,
                         &params, sizeof(params));
    if (ret != NVOS_STATUS_SUCCESS) {
        nvEvoLogDev(pDevEvo, EVO_LOG_WARN,
            "NVA080_CTRL_CMD_VGPU_DISPLAY_CLEANUP_SURFACE "
            "failed on apiHead %u (0x%x)", apiHead, ret);
    }
}

static void DisplaylessAllocConsoleVnc(NVDevEvoPtr pDevEvo)
{
    NvU32 hVgpuDevice = nvGenerateUnixRmHandle(&pDevEvo->handleAllocator);
    NvU32 ret;

    ret = nvRmApiAlloc(nvEvoGlobal.clientHandle,
                       pDevEvo->deviceHandle,
                       hVgpuDevice,
                       KEPLER_DEVICE_VGPU, NULL);
    if (ret != NVOS_STATUS_SUCCESS) {
        nvEvoLogDev(pDevEvo, EVO_LOG_WARN,
            "Failed to allocate KEPLER_DEVICE_VGPU; console VNC mirroring "
            "will be disabled (0x%x)", ret);
        nvFreeUnixRmHandle(&pDevEvo->handleAllocator, hVgpuDevice);
    } else {
        pDevEvo->displaylessWorker.hVgpuDevice = hVgpuDevice;
    }
}

static void DisplaylessDestroyConsoleVnc(NVDevEvoPtr pDevEvo)
{
    NvU32 ret;

    if (pDevEvo->displaylessWorker.hVgpuDevice == 0) {
        return;
    }

    DisplaylessCleanupConsoleVnc(pDevEvo, 0);

    /*
     * The plugin has been told to drop its cached surface, so it is now
     * safe to release the reference we held on its behalf.
     */
    ReleaseLastScanoutSurface(pDevEvo, 0);

    ret = nvRmApiFree(nvEvoGlobal.clientHandle,
                      pDevEvo->deviceHandle,
                      pDevEvo->displaylessWorker.hVgpuDevice);
    if (ret != NVOS_STATUS_SUCCESS) {
        nvAssert(!"Free(hVgpuDevice) failed");
    }
    nvFreeUnixRmHandle(&pDevEvo->handleAllocator,
                       pDevEvo->displaylessWorker.hVgpuDevice);
}

static void DisplaylessFlip(
    NVDevEvoPtr pDevEvo,
    NVEvoChannelPtr pChannel,
    const NVFlipChannelEvoHwState *pHwState,
    NVEvoUpdateState *updateState,
    NvBool bypassComposition)
{
    const NVDispEvoPtr pDispEvo = pDevEvo->pDispEvo[0];
    NvU32 win = NV_EVO_CHANNEL_MASK_WINDOW_NUMBER(pChannel->channelMask);
    NvU32 head = pDevEvo->headForWindow[win];
    NvU32 apiHead, h;

    for (apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
        FOR_EACH_EVO_HW_HEAD(pDispEvo, apiHead, h) {
            if (h == head) {
                goto api_head_found;
            }
        }
    }
    nvAssert(!"Api head not found!");
    return;

api_head_found:
    {
        NVSurfaceEvoPtr pScanoutSurface = NULL;

        pScanoutSurface = GetConsoleVncScanoutSurface(pDevEvo, apiHead, head,
                                                      pChannel, pHwState);

        DisplaylessEnqueueFlip(
            pDevEvo, apiHead,
            pHwState->syncObject.usingSyncpt,
            &pHwState->syncObject.u.semaphores.acquireSurface,
            pHwState->syncObject.u.semaphores.acquireValue,
            pHwState->syncObject.u.semaphores.releaseValue,
            pScanoutSurface);
    }
}

static void DisplaylessFlipTransitionWAR(
    NVDevEvoPtr pDevEvo, NvU32 sd, NvU32 head,
    const NVEvoSubDevHeadStateRec *pSdHeadState,
    const NVFlipEvoHwState *pFlipState,
    NVEvoUpdateState *updateState)
{
}

static void
DisplaylessFillLUTSurface(
    NVEvoLutEntryRec *pLUTBuffer,
    const NvU16 *red,
    const NvU16 *green,
    const NvU16 *blue,
    int nColorMapEntries, int depth)
{
}

static void DisplaylessSetOutputLut(
    NVDevEvoPtr pDevEvo,
    NvU32 sd,
    NvU32 head,
    const NVFlipLutHwState *pOutputLut,
    NvU32 fpNormScale,
    NVEvoUpdateState *updateState,
    NvBool bypassComposition)
{
}

static void DisplaylessSetOutputScaler(
    const NVDispEvoRec *pDispEvo, const NvU32 head,
    NVEvoUpdateState *updateState)
{
}

static void DisplaylessSetViewportPointIn(
    NVDevEvoPtr pDevEvo, const int head,
    NvU16 x, NvU16 y,
    NVEvoUpdateState *updateState)
{
}

static void DisplaylessSetViewportInOut(
    NVDevEvoPtr pDevEvo, const int head,
    const NVHwModeViewPortEvo *pViewPortMin,
    const NVHwModeViewPortEvo *pViewPort,
    const NVHwModeViewPortEvo *pViewPortMax,
    NVEvoUpdateState *updateState)
{
}

static void DisplaylessSetCursorImage(
    NVDevEvoPtr pDevEvo, const int head,
    const NVSurfaceEvoRec *pSurfaceEvo,
    NVEvoUpdateState *updateState,
    const struct NvKmsCompositionParams *pCursorCompParams)
{
}

static NvBool DisplaylessValidateCursorSurface(
    const NVDevEvoRec *pDevEvo,
    const NVSurfaceEvoRec *pSurfaceEvo)
{
    return true;
}

static NvBool DisplaylessValidateWindowFormat(
    const enum NvKmsSurfaceMemoryFormat format,
    const struct NvKmsRect *sourceFetchRect,
    NvU32 *hwFormatOut)
{
    return true;
}

static void DisplaylessInitCompNotifier(const NVDispEvoRec *pDispEvo, int idx)
{
}

static NvBool DisplaylessIsCompNotifierComplete(NVDispEvoPtr pDispEvo, int idx)
{
    return true;
}

static void DisplaylessWaitForCompNotifier(
    const NVDispEvoRec *pDispEvo, int idx)
{
}

static void DisplaylessSetDither(
    NVDispEvoPtr pDispEvo, const int head,
    const NvBool enabled, const NvU32 type,
    const NvU32 algo,
    NVEvoUpdateState *updateState)
{
}

static void DisplaylessSetStallLock(
    NVDispEvoPtr pDispEvo, const int head,
    NvBool enable, NVEvoUpdateState *updateState)
{
}

static void DisplaylessSetDisplayRate(
    NVDispEvoPtr pDispEvo, const int head,
    NvBool enable,
    NVEvoUpdateState *updateState,
    NvU32 timeoutMicroseconds)
{
}

static void DisplaylessInitChannel(
    NVDevEvoPtr pDevEvo, NVEvoChannelPtr pChannel)
{
}

static void DisplaylessInitDefaultLut(NVDevEvoPtr pDevEvo)
{
}

static void DisplaylessInitWindowMapping(
    const NVDispEvoRec *pDispEvo,
    NVEvoModesetUpdateState *pModesetUpdateState)
{
}

static NvBool DisplaylessIsChannelIdle(
    NVDevEvoPtr pDevEvo,
    NVEvoChannelPtr pChan,
    NvU32 sd,
    NvBool *result)
{
    return true;
}

static NvBool DisplaylessIsChannelMethodPending(
    NVDevEvoPtr pDevEvo,
    NVEvoChannelPtr pChan,
    NvU32 sd,
    NvBool *result)
{
    return false;
}

static NvBool DisplaylessForceIdleSatelliteChannel(
    NVDevEvoPtr pDevEvo,
    const NVEvoIdleChannelState *idleChannelState)
{
    return true;
}

static NvBool DisplaylessForceIdleSatelliteChannelIgnoreLock(
    NVDevEvoPtr pDevEvo,
    const NVEvoIdleChannelState *idleChannelState)
{
    return true;
}

static void DisplaylessAccelerateChannel(
    NVDevEvoPtr pDevEvo,
    NVEvoChannelPtr pChannel,
    const NvU32 sd,
    const NvBool trashPendingMethods,
    const NvBool unblockMethodsInExecutation,
    NvU32 *pOldAccelerators)
{
}

static void DisplaylessResetChannelAccelerators(
    NVDevEvoPtr pDevEvo,
    NVEvoChannelPtr pChannel,
    const NvU32 sd,
    const NvBool trashPendingMethods,
    const NvBool unblockMethodsInExecutation,
    NvU32 oldAccelerators)
{
}

static void DisplaylessInitFlipWorker(NVDevEvoPtr pDevEvo)
{
    nvkms_memset(&pDevEvo->displaylessWorker, 0, sizeof(pDevEvo->displaylessWorker));
}

static NvBool DisplaylessAllocRmCtrlObject(NVDevEvoPtr pDevEvo)
{
    DisplaylessInitFlipWorker(pDevEvo);

    DisplaylessAllocConsoleVnc(pDevEvo);

    return true;
}

static void DisplaylessDestroyFlipWorker(NVDevEvoPtr pDevEvo)
{
    const NVDispEvoPtr pDispEvo = pDevEvo->pDispEvo[0];

    nvkms_free_timer(pDevEvo->displaylessWorker.timer);
    ProcessPendingFlips(pDevEvo, pDispEvo, TRUE /* skipWaitingForSema */);

    DisplaylessDestroyConsoleVnc(pDevEvo);

    nvkms_memset(&pDevEvo->displaylessWorker, 0, sizeof(pDevEvo->displaylessWorker));
}

static void DisplaylessFreeRmCtrlObject(NVDevEvoPtr pDevEvo)
{
    DisplaylessDestroyFlipWorker(pDevEvo);
}

static void DisplaylessSetImmPointOut(
    NVDevEvoPtr pDevEvo,
    NVEvoChannelPtr pChannel,
    NvU32 sd,
    NVEvoUpdateState *updateState,
    NvU16 x, NvU16 y)
{
}

static void DisplaylessStartCRC32Capture(
    NVDevEvoPtr pDevEvo,
    NVEvoDmaPtr pDma,
    NVConnectorEvoPtr pConnectorEvo,
    const enum nvKmsTimingsProtocol protocol,
    const NvU32 orIndex,
    NvU32 head,
    NVEvoUpdateState *updateState)
{
}

static void DisplaylessStopCRC32Capture(
    NVDevEvoPtr pDevEvo,
    NvU32 head,
    NVEvoUpdateState *updateState)
{
}

static NvBool DisplaylessQueryCRC32(
    NVDevEvoPtr pDevEvo,
    NVEvoDmaPtr pDma,
    NvU32 entry_count,
    CRC32NotifierCrcOut *crc32,
    NvU32 *numCRC32)
{
    return true;
}

static void DisplaylessGetScanLine(
    const NVDispEvoRec *pDispEvo,
    const NvU32 head,
    NvU16 *pScanLine,
    NvBool *pInBlankingPeriod)
{
}

static void DisplaylessConfigureVblankSyncObject(
    const NVDevEvoPtr pDevEvo,
    const NvU16 rasterLine,
    const NvU32 head,
    const NvU32 semaphoreIndex,
    const NVSurfaceDescriptor *pSurfaceDesc,
    NVEvoUpdateState* pUpdateState)
{
}

static void DisplaylessSetDscParams(
    const NVDispEvoRec *pDispEvo,
    const NvU32 head,
    const NVDscInfoEvoRec *pDscInfo,
    const enum nvKmsPixelDepth pixelDepth)
{
}

static NvU32 DisplaylessGetActiveViewportOffset(
    NVDispEvoRec *pDispEvo, NvU32 head)
{
    return 0;
}

static NvBool DisplaylessComputeWindowScalingTaps(
    const NVDevEvoRec *pDevEvo,
    const NVEvoChannel *pChannel,
    NVFlipChannelEvoHwState *pHwState)
{
    return true;
}

static void DisplaylessSendHdmiInfoFrame(
    const NVDispEvoRec *pDispEvo,
    const NvU32 head,
    const NvEvoInfoFrameTransmitControl *transmitCtrl,
    const NVT_INFOFRAME_HEADER *pInfoFrameHeader,
    const NvU32 infoframeSize,
    NvBool needChecksum)
{
}

static void DisplaylessDisableHdmiInfoFrame(
    const NVDispEvoRec *pDispEvo,
    const NvU32 head,
    const NvEvoInfoFrameTransmitControl *transmitCtrl,
    const NvU8 nvtInfoFrameType)
{
}

static void DisplaylessSendDpInfoFrameSdp(
    const NVDispEvoRec *pDispEvo,
    const NvU32 head,
    const NvEvoInfoFrameTransmitControl *transmitCtrl,
    const DPSDP_DESCRIPTOR *sdp)
{
}

static void DisplaylessSetTmoLutSurfaceAddress(
     const NVDevEvoRec *pDevEvo,
     NVEvoChannelPtr pChannel,
     const NVSurfaceDescriptor *pSurfaceDesc,
     NvU32 offset)
{
}

static void DisplaylessSetILUTSurfaceAddress(
     const NVDevEvoRec *pDevEvo,
     NVEvoChannelPtr pChannel,
     const NVSurfaceDescriptor *pSurfaceDesc,
     NvU32 offset)
{
}

static void DisplaylessSetISOSurfaceAddress(
     const NVDevEvoRec *pDevEvo,
     NVEvoChannelPtr pChannel,
     const NVSurfaceDescriptor *pSurfaceDesc,
     NvU32 offset,
     NvU32 ctxDmaIdx,
     NvBool isBlocklinear)
{
}

static void DisplaylessSetCoreNotifierSurfaceAddressAndControl(
    const NVDevEvoRec *pDevEvo,
    NVEvoChannelPtr pChannel,
    const NVSurfaceDescriptor *pSurfaceDesc,
    NvU32 notifierOffset,
    NvU32 ctrlVal)
{
}

static void DisplaylessSetWinNotifierSurfaceAddressAndControl(
    const NVDevEvoRec *pDevEvo,
    NVEvoChannelPtr pChannel,
    const NVSurfaceDescriptor *pSurfaceDesc,
    NvU32 notifierOffset,
    NvU32 ctrlVal)
{
}

static void DisplaylessSetSemaphoreSurfaceAddressAndControl(
    const NVDevEvoRec *pDevEvo,
    NVEvoChannelPtr pChannel,
    const NVSurfaceDescriptor *pSurfaceDesc,
    NvU32 semaphoreOffset,
    NvU32 ctrlVal)
{
}

static void DisplaylessSetAcqSemaphoreSurfaceAddressAndControl(
    const NVDevEvoRec *pDevEvo,
    NVEvoChannelPtr pChannel,
    const NVSurfaceDescriptor *pSurfaceDesc,
    NvU32 semaphoreOffset,
    NvU32 ctrlVal)
{
}

NVEvoHAL nvDisplayless = {
    DisplaylessSetRasterParams,                         /* SetRasterParams */
    DisplaylessSetProcAmp,                              /* SetProcAmp */
    DisplaylessSetHeadControl,                          /* SetHeadControl */
    DisplaylessSetHeadRefClk,                           /* SetHeadRefClk */
    DisplaylessHeadSetControlOR,                        /* HeadSetControlOR */
    DisplaylessORSetControl,                            /* ORSetControl */
    DisplaylessHeadSetDisplayId,                        /* HeadSetDisplayId */
    DisplaylessSetUsageBounds,                          /* SetUsageBounds */
    DisplaylessUpdate,                                  /* Update */
    DisplaylessIsModePossible,                          /* IsModePossible */
    DisplaylessSetNotifier,                             /* SetNotifier */
    DisplaylessGetCapabilities,                         /* GetCapabilities */
    DisplaylessFlip,                                    /* Flip */
    DisplaylessFlipTransitionWAR,                       /* FlipTransitionWAR */
    DisplaylessFillLUTSurface,                          /* FillLUTSurface */
    DisplaylessSetOutputLut,                            /* SetOutputLut */
    DisplaylessSetOutputScaler,                         /* SetOutputScaler */
    DisplaylessSetViewportPointIn,                      /* SetViewportPointIn */
    DisplaylessSetViewportInOut,                        /* SetViewportInOut */
    DisplaylessSetCursorImage,                          /* SetCursorImage */
    DisplaylessValidateCursorSurface,                   /* ValidateCursorSurface */
    DisplaylessValidateWindowFormat,                    /* ValidateWindowFormat */
    DisplaylessInitCompNotifier,                        /* InitCompNotifier */
    DisplaylessIsCompNotifierComplete,                  /* IsCompNotifierComplete */
    DisplaylessWaitForCompNotifier,                     /* WaitForCompNotifier */
    DisplaylessSetDither,                               /* SetDither */
    DisplaylessSetStallLock,                            /* SetStallLock */
    DisplaylessSetDisplayRate,                          /* SetDisplayRate */
    DisplaylessInitChannel,                             /* InitChannel */
    DisplaylessInitDefaultLut,                          /* InitDefaultLut */
    DisplaylessInitWindowMapping,                       /* InitWindowMapping */
    DisplaylessIsChannelIdle,                           /* IsChannelIdle */
    DisplaylessIsChannelMethodPending,                  /* IsChannelMethodPending */
    DisplaylessForceIdleSatelliteChannel,               /* ForceIdleSatelliteChannel */
    DisplaylessForceIdleSatelliteChannelIgnoreLock,     /* ForceIdleSatelliteChannelIgnoreLock */
    DisplaylessAccelerateChannel,                       /* AccelerateChannel */
    DisplaylessResetChannelAccelerators,                /* ResetChannelAccelerators */
    DisplaylessAllocRmCtrlObject,                       /* AllocRmCtrlObject */
    DisplaylessFreeRmCtrlObject,                        /* FreeRmCtrlObject */
    DisplaylessSetImmPointOut,                          /* SetImmPointOut */
    DisplaylessStartCRC32Capture,                       /* StartCRC32Capture */
    DisplaylessStopCRC32Capture,                        /* StopCRC32Capture */
    DisplaylessQueryCRC32,                              /* QueryCRC32 */
    DisplaylessGetScanLine,                             /* GetScanLine */
    DisplaylessConfigureVblankSyncObject,               /* ConfigureVblankSyncObject */
    DisplaylessSetDscParams,                            /* SetDscParams */
    NULL,                                               /* EnableMidFrameAndDWCFWatermark */
    DisplaylessGetActiveViewportOffset,                 /* GetActiveViewportOffset */
    DisplaylessComputeWindowScalingTaps,                /* ComputeWindowScalingTaps */
    NULL,                                               /* GetWindowScalingCaps */
    NULL,                                               /* SetMergeMode */
    NULL,                                               /* SendHdmiVrrVtem */
    DisplaylessSendHdmiInfoFrame,                       /* SendHdmiInfoFrame */
    DisplaylessDisableHdmiInfoFrame,                    /* DisableHdmiInfoFrame */
    DisplaylessSendDpInfoFrameSdp,                      /* SendDpInfoFrameSdp */
    NULL,                                               /* DisableAdaptiveSyncSdp */
    NULL,                                               /* SetDpVscSdp */
    NULL,                                               /* InitHwHeadMultiTileConfig */
    NULL,                                               /* SetMultiTileConfig */
    DisplaylessAllocSurface,                            /* AllocSurfaceDescriptor */
    DisplaylessFreeSurfaceDescriptor,                   /* FreeSurfaceDescriptor */
    DisplaylessBindSurfaceDescriptor,                   /* BindSurfaceDescriptor */
    DisplaylessSetTmoLutSurfaceAddress,                 /* SetTmoLutSurfaceAddress */
    DisplaylessSetILUTSurfaceAddress,                   /* SetILUTSurfaceAddress */
    DisplaylessSetISOSurfaceAddress,                    /* SetISOSurfaceAddress */
    DisplaylessSetCoreNotifierSurfaceAddressAndControl, /* SetCoreNotifierSurfaceAddressAndControl */
    DisplaylessSetWinNotifierSurfaceAddressAndControl,  /* SetWinNotifierSurfaceAddressAndControl */
    DisplaylessSetSemaphoreSurfaceAddressAndControl,    /* SetSemaphoreSurfaceAddressAndControl */
    DisplaylessSetAcqSemaphoreSurfaceAddressAndControl, /* SetAcqSemaphoreSurfaceAddressAndControl */
    NULL,                                               /* SetCoreFid */
    NULL,                                               /* SetupVBlankRgSemaphoreInterrupt */
    NULL,                                               /* SetVTotalAndFraction */
    {                                                   /* caps */
        FALSE,                                          /* supportsHDMIFRL */
        FALSE,                                          /* supportsSetStorageMemoryLayout */
        FALSE,                                          /* supportsIndependentAcqRelSemaphore */
        FALSE,                                          /* supportsVblankSyncObjects */
        FALSE,                                          /* supportsMergeMode */
        FALSE,                                          /* supportsHDMI10BPC */
        FALSE,                                          /* supportsDPAudio192KHz */
        FALSE,                                          /* supportsYCbCr422OverHDMIFRL */
        FALSE,                                          /* supportsColorPassthrough */
        NV_EVO3_X_EMULATED_SURFACE_MEMORY_FORMATS_C6,   /* xEmulatedSurfaceMemoryFormats */
    },
};

static void DisplaylessMoveCursor(NVDevEvoPtr pDevEvo, NvU32 head,
                                  NvS16 x, NvS16 y)
{
}

static void DisplaylessReleaseElv(NVDevEvoPtr pDevEvo, NvU32 head)
{
}

NVEvoCursorHAL nvDisplaylessCursor = {
    NVA083_GRID_DISPLAYLESS,            /* klass */
    DisplaylessMoveCursor,              /* MoveCursor */
    DisplaylessReleaseElv,              /* ReleaseElv */
    {                                   /* caps */
       256,                             /* maxSize */
    },
};
