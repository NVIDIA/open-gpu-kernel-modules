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

/*
 * DIFR stands for Display Idle Frame Refresh which is a low-power feature
 * for display that allows scanning out frames from L2 cache. The actual GPU
 * memory can be gated off while the display outputs are served off the
 * cache.
 *
 * DIFR is defined in three operational layers 1, 2, and 3 and operates in
 * terms of entering and exiting these layers in order.
 *
 * Layer 1 has to deem it's possible to enter DIFR until layer 2 and 3 can
 * start considering. Any layer seeing conditions that prevent entering DIFR
 * mode can abort the attempt to enter. But, finally, if all layers agree
 * the hardware will switch to low-power mode, turn off GPU memory, and
 * start serving pixels off the cache.
 *
 * Managing some high-level state to help the hardware transition from one
 * layer to another is implemented in NVKMS and RM. Simplified, NVKMS
 * handles assistance for layer 1 and RM for layer 2.
 *
 * Much of the layer 1 or NVKMS DIFR specific code is collected into this
 * file, centered around an object called NVDIFRStateEvo.
 *
 * The role of NVKMS is to listen for DIFR prefetch events (which originate
 * from h/w and get dispatched by RM), prefetch framebuffer pixels into L2
 * cache, and report back to h/w (via RM). NVKMS will also disable DIFR each
 * time there's an explicitly known display update (such as a flip) and
 * re-enable it once enough idle time has passed.
 *
 * The rest of NVKMS will call entrypoints in this file to inform the DIFR
 * implementation here about changes in relevant state.
 *
 * For each DevEvo object nvkms-evo.c will call
 * nvDIFRAllocate()/nvDIFRFree() here to also create a corresponding DIFR
 * state object. The DIFR state will contain everything needed to implement
 * prefetching such as channel and copy engine allocation.
 *
 * If DIFR state was successfully allocated, nvkms-rm.c will create an event
 * listener for DIFR prefetch events which will call back to
 * nvDIFRPrefetchSurfaces() here in order to do prefetching. This means
 * going through each active head and issuing a special CE copy, for all
 * layers of the surface, to populate the L2 cache with framebuffer pixel
 * data.
 *
 * After all prefetches are complete, RM needs to know about the completion
 * status. This is implemented in nvDIFRSendPrefetchResponse(), again called
 * by nvkms-rm.c.
 *
 * NVKMS must also temporarily disable DIFR in hardware if it knows about
 * upcoming updates to the framebuffer and then re-enable DIFR when the
 * screen becomes idle again. For this, nvFlipEvoOneHead() will call us back
 * via nvDIFRNotifyFlip() when a new flip is happening. We will call RM to
 * disable DIFR, then set up a timer into the future and when it triggers we
 * will re-enable DIFR again. But if nvFlipEvoOneHead() notifies us about
 * another upcoming frame, we'll just replace the old timer with a new one.
 * Thus, the timer will eventually wake us after notifications of new frames
 * cease to come in.
 *
 * The DIFR hardware will automatically detect activity in graphics/copy
 * engines and will not try to enter the low-power mode if there is any. So
 * this is something NVKMS doesn't have to worry about.
 *
 * Userspace can also flag surfaces as non-cacheable which makes us abort
 * any prefetches if those surfaces are currently displayed on any active
 * heads. For now, CPU mapped surfaces are flagged as such because neither
 * NVKMS nor the hardware can observe CPU writes into a surface.
 */



#include "nvkms-difr.h"
#include "nvkms-push.h"
#include "nvkms-rm.h"
#include "nvkms-rmapi.h"
#include "nvkms-utils.h"
#include "nvkms-evo.h"

#include "nvidia-push-init.h"
#include "nvidia-push-methods.h"
#include "nvidia-push-types.h"
#include "nvidia-push-types.h"
#include "nvidia-push-utils.h"

#include <class/cl2080.h>
#include <class/cla06f.h>
#include <class/cla06fsubch.h>
#include <class/cla0b5.h>
#include <class/clb0b5sw.h>
#include <class/clc7b5.h>
#include "class/clcab5.h" // BLACKWELL_DMA_COPY_B
#include <ctrl/ctrl2080/ctrl2080ce.h>
#include <ctrl/ctrl2080/ctrl2080lpwr.h>

#define PREFETCH_DONE_VALUE             0x00000fed

/* How long to wait after last flip until re-enabling DIFR. */
#define DIFR_IDLE_WAIT_PERIOD_US        500000

/* How long to wait for prefetch dma completion. */
#define DIFR_PREFETCH_WAIT_PERIOD_US    10000 /* 10ms */

/*
 * DIFR runtime state
 */
typedef struct _NVDIFRStateEvoRec {
    NVDevEvoPtr pDevEvo;
    NvU32 copyEngineClass;
    NvU32 copyEngineType;

    /*
     * This is kept in sync with whether DIFR is explicitly disabled in
     * hardware.
     */
    NvBool hwDisabled;
    NvU64 lastFlipTime;
    nvkms_timer_handle_t *idleTimer;

    /* Pushbuffer for DIFR prefetches. */
    NvPushChannelRec prefetchPushChannel;
    NvU32 pushChannelHandlePool[NV_PUSH_CHANNEL_HANDLE_POOL_NUM];

    /* Copy engine instance for DIFR prefetches. */
    NvU32 prefetchEngine;

    /* For tracking which surfaces have been prefetched already. */
    NvU32 prefetchPass;
} NVDIFRStateEvoRec;

/*
 * Prefetch parameters for DMA copy.
 */
typedef struct {
    NvUPtr surfGpuAddress;
    size_t surfSizeBytes;
    enum NvKmsSurfaceMemoryFormat surfFormat;
    NvU32 surfPitchBytes;
} NVDIFRPrefetchParams;

static NvBool AllocDIFRPushChannel(NVDIFRStateEvoPtr pDifr);
static void FreeDIFRPushChannel(NVDIFRStateEvoPtr pDifr);
static NvBool AllocDIFRCopyEngine(NVDIFRStateEvoPtr pDifr);
static void FreeDIFRCopyEngine(NVDIFRStateEvoPtr pDifr);

static NvU32 PrefetchSingleSurface(NVDIFRStateEvoPtr pDifr,
                                   NVDIFRPrefetchParams *pParams,
                                   size_t *remainingCache);
static NvBool PrefetchHelperSurfaceEvo(NVDIFRStateEvoPtr pDifr,
                                       size_t *cacheRemaining,
                                       NVSurfaceEvoPtr pSurfaceEvo,
                                       NvU32 *status);

static NvBool SetDisabledState(NVDIFRStateEvoPtr pDifr,
                               NvBool shouldDisable);
static NvBool IsCECompatibleWithDIFR(NVDevEvoPtr pDevEvo,
                                     NvU32 instance);
static void EnsureIdleTimer(NVDIFRStateEvoPtr pDifr);
static void IdleTimerProc(void *dataPtr, NvU32 dataU32);

/*
 * Public entry points.
 */

NVDIFRStateEvoPtr nvDIFRAllocate(NVDevEvoPtr pDevEvo)
{
    NV2080_CTRL_CMD_LPWR_DIFR_CTRL_PARAMS params = { 0 };
    NVDIFRStateEvoPtr pDifr;
    NvU32 ret;

    /* DIFR not supported/implemented on RM SLI */
    if (pDevEvo->numSubDevices > 1) {
        return NULL;
    }

    params.ctrlParamVal = NV2080_CTRL_LPWR_DIFR_CTRL_SUPPORT_STATUS;
    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->pSubDevices[0]->handle,
                         NV2080_CTRL_CMD_LPWR_DIFR_CTRL,
                         &params,
                         sizeof(params));

    if (ret == NV_ERR_NOT_SUPPORTED) {
        return NULL;
    }

    if (ret != NV_OK) {
        nvEvoLogDev(pDevEvo,
                    EVO_LOG_WARN,
                    "unable to query whether display caching is supported");
        return NULL;
    }

    if (params.ctrlParamVal != NV2080_CTRL_LPWR_DIFR_SUPPORTED) {
        return NULL;
    }

    pDifr = nvCalloc(sizeof(*pDifr), 1);
    if (!pDifr) {
        return NULL;
    }

    pDifr->pDevEvo = pDevEvo;

    if (!AllocDIFRPushChannel(pDifr) ||
        !AllocDIFRCopyEngine(pDifr)) {
        nvDIFRFree(pDifr);

        return NULL;
    }

    return pDifr;
}

void nvDIFRFree(NVDIFRStateEvoPtr pDifr)
{
    nvAssert(pDifr);

    /* Cancel pending idle timer. */
    nvkms_free_timer(pDifr->idleTimer);

    /* Leave DIFR enabled (default state). */
    SetDisabledState(pDifr, FALSE);

    /* Free resources. */
    FreeDIFRCopyEngine(pDifr);
    FreeDIFRPushChannel(pDifr);

    nvFree(pDifr);
}

/*
 * Notify of a new or upcoming flip. This will disable DIFR for a brief
 * period in anticipation of further flips.
 */
void nvDIFRNotifyFlip(NVDIFRStateEvoPtr pDifr)
{
    pDifr->lastFlipTime = nvkms_get_usec();

    /* A flip is coming: signal RM to disable DIFR if we haven't already. */
    if (SetDisabledState(pDifr, TRUE)) {
        /* Check back after a while and re-enable if idle again. */
        EnsureIdleTimer(pDifr);
    }
}

NvU32 nvDIFRPrefetchSurfaces(NVDIFRStateEvoPtr pDifr, size_t l2CacheSize)
{
    NVDevEvoPtr pDevEvo = pDifr->pDevEvo;
    NVEvoSubDevPtr pSubDev;
    NVEvoSubDevHeadStatePtr pHeadState;
    size_t cacheRemaining = l2CacheSize;
    NvU32 layer;
    NvU32 head;
    NvU32 apiHead;
    NvU32 eye;
    NvU32 i;
    NvU32 status;

    /*
     * If the console is active then the scanout surfaces will get updated by
     * the OS console driver without any knowledge of NVKMS, DIFR should not be
     * enabled in that case.
     */
    if (nvEvoIsConsoleActive(pDevEvo)) {
        /*
         * NV2080_CTRL_LPWR_DIFR_PREFETCH_FAIL_INSUFFICIENT_L2_SIZE: Despite
         * what the name suggests this will actually tell RM (and further PMU)
         * to disable DIFR until the next modeset.
         */
        return NV2080_CTRL_LPWR_DIFR_PREFETCH_FAIL_INSUFFICIENT_L2_SIZE;
    }

    /*
     * If DIFR is disabled it's because we know we were or will be flipping.
     */
    if (pDifr->hwDisabled) {
        return NV2080_CTRL_LPWR_DIFR_PREFETCH_FAIL_OS_FLIPS_ENABLED;
    }

    status = NV2080_CTRL_LPWR_DIFR_PREFETCH_SUCCESS;

    pSubDev = &pDevEvo->gpus[0];

    /* Get new prefetch pass counter for this iteration. */
    pDifr->prefetchPass++;

    /*
     * Start by prefetching the cursor surface and image surfaces from
     * present layers.
     */
    for (head = 0; head < pDevEvo->numHeads; head++) {
        pHeadState = &pSubDev->headState[head];

        if (!PrefetchHelperSurfaceEvo(pDifr,
                                      &cacheRemaining,
                                      pHeadState->cursor.pSurfaceEvo,
                                      &status)) {
            goto out;
        }

        for (layer = 0; layer <= pDevEvo->head[head].numLayers; layer++) {
            for (eye = 0; eye < NVKMS_MAX_EYES; eye++) {

                if (!PrefetchHelperSurfaceEvo(pDifr,
                                              &cacheRemaining,
                                              pHeadState->layer[layer].pSurfaceEvo[eye],
                                              &status)) {
                    goto out;
                }
            }

            /*
             * Prefetch per-layer LUTs, if any, but skip null LUTs and
             * duplicates already prefetched.
             */
            if (!PrefetchHelperSurfaceEvo(pDifr,
                                          &cacheRemaining,
                                          pHeadState->layer[layer].inputLut.pLutSurfaceEvo,
                                          &status)) {
                goto out;
            }

            if (!PrefetchHelperSurfaceEvo(pDifr,
                                          &cacheRemaining,
                                          pHeadState->layer[layer].tmoLut.pLutSurfaceEvo,
                                          &status)) {
                goto out;
            }
        }
    }

    /*
     * Finally prefetch the known main LUTs.
     */
    if (!PrefetchHelperSurfaceEvo(pDifr,
                                  &cacheRemaining,
                                  pDevEvo->lut.defaultLut,
                                  &status)) {
        goto out;
    }

    for (apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
        for (i = 0; i < ARRAY_LEN(pDevEvo->lut.apiHead[apiHead].LUT); i++) {
            if (!PrefetchHelperSurfaceEvo(pDifr,
                                          &cacheRemaining,
                                          pDevEvo->lut.apiHead[apiHead].LUT[i],
                                          &status)) {
                goto out;
            }
        }
    }

out:
    return status;
}

NvBool nvDIFRSendPrefetchResponse(NVDIFRStateEvoPtr pDifr, NvU32 responseStatus)
{
    NVDevEvoPtr pDevEvo = pDifr->pDevEvo;
    NV2080_CTRL_CMD_LPWR_DIFR_PREFETCH_RESPONSE_PARAMS params = { 0 };

    params.responseVal = responseStatus;

    return (nvRmApiControl(nvEvoGlobal.clientHandle,
                           pDevEvo->pSubDevices[0]->handle,
                           NV2080_CTRL_CMD_LPWR_DIFR_PREFETCH_RESPONSE,
                           &params,
                           sizeof(params))
            == NV_OK);
}

/*
 * Local helper functions.
 */
static NvBool AllocDIFRPushChannel(NVDIFRStateEvoPtr pDifr)
{
    NVDevEvoPtr pDevEvo = pDifr->pDevEvo;
    NvPushAllocChannelParams params = { 0 };
    NvU32 i;

    pDifr->copyEngineType = NV2080_ENGINE_TYPE_NULL;

    for (i = 0; i < NV2080_ENGINE_TYPE_COPY_SIZE; i++) {
        if (IsCECompatibleWithDIFR(pDevEvo, i)) {
            pDifr->copyEngineType = NV2080_ENGINE_TYPE_COPY(i);
            break;
        }
    }

    if (pDifr->copyEngineType == NV2080_ENGINE_TYPE_NULL) {
        return FALSE;
    }

    params.engineType = pDifr->copyEngineType;
    params.pDevice = &pDifr->pDevEvo->nvPush.device;
    params.difrPrefetch = TRUE;
    params.logNvDiss = FALSE;
    params.noTimeout = FALSE;
    params.ignoreChannelErrors = FALSE;
    params.numNotifiers = 1;
    params.pushBufferSizeInBytes = 1024;

    ct_assert(sizeof(params.handlePool) == sizeof(pDifr->pushChannelHandlePool));

    for (i = 0; i < ARRAY_LEN(pDifr->pushChannelHandlePool); i++) {
        pDifr->pushChannelHandlePool[i] =
            nvGenerateUnixRmHandle(&pDevEvo->handleAllocator);

        params.handlePool[i] = pDifr->pushChannelHandlePool[i];
    }

    if (!nvPushAllocChannel(&params, &pDifr->prefetchPushChannel)) {
        return FALSE;
    }

    return TRUE;
}

static void FreeDIFRPushChannel(NVDIFRStateEvoPtr pDifr)
{
    NVDevEvoPtr pDevEvo = pDifr->pDevEvo;
    NvU32 i;

    nvPushFreeChannel(&pDifr->prefetchPushChannel);

    for (i = 0; i < ARRAY_LEN(pDifr->pushChannelHandlePool); i++) {
        nvFreeUnixRmHandle(&pDevEvo->handleAllocator,
                           pDifr->pushChannelHandlePool[i]);
        pDifr->pushChannelHandlePool[i] = 0;
    }
}

static NvBool AllocDIFRCopyEngine(NVDIFRStateEvoPtr pDifr)
{
    NVB0B5_ALLOCATION_PARAMETERS allocParams = { 0 };
    NVDevEvoPtr pDevEvo = pDifr->pDevEvo;
    NvU32 ret;
    NvU32 ceClass = 0, i;

    static const NvU32 ceClasses[] = {
        BLACKWELL_DMA_COPY_B,
        AMPERE_DMA_COPY_B,
    };

    for (i = 0; i < ARRAY_LEN(ceClasses); i++) {
        if (nvRmEvoClassListCheck(pDevEvo, ceClasses[i])) {
            ceClass = ceClasses[i];
            break;
        }
    }

    if (ceClass == 0) {
        nvEvoLogDevDebug(pDevEvo, EVO_LOG_ERROR,
                         "Failed to find a supported DIFR CE class.");
        return NV_FALSE;
    }

    pDifr->prefetchEngine = nvGenerateUnixRmHandle(&pDevEvo->handleAllocator);
    if (pDifr->prefetchEngine == 0) {
        return NV_FALSE;
    }

    allocParams.version = NVB0B5_ALLOCATION_PARAMETERS_VERSION_1;
    allocParams.engineType = pDifr->copyEngineType;

    ret = nvRmApiAlloc(nvEvoGlobal.clientHandle,
                       pDifr->prefetchPushChannel.channelHandle[0],
                       pDifr->prefetchEngine,
                       ceClass,
                       &allocParams);
    if (ret != NVOS_STATUS_SUCCESS) {
        nvFreeUnixRmHandle(&pDifr->pDevEvo->handleAllocator,
                           pDifr->prefetchEngine);
        pDifr->prefetchEngine = 0;
        return NV_FALSE;
    }

    // For Ampere vs Blackwell+ differentiation later
    pDifr->copyEngineClass = ceClass;

    return NV_TRUE;
}

static void FreeDIFRCopyEngine(NVDIFRStateEvoPtr pDifr)
{
    if (pDifr->prefetchEngine != 0) {
        nvRmApiFree(nvEvoGlobal.clientHandle,
                    pDifr->pDevEvo->pSubDevices[0]->handle,
                    pDifr->prefetchEngine);
    }

    nvFreeUnixRmHandle(&pDifr->pDevEvo->handleAllocator,
                       pDifr->prefetchEngine);
    pDifr->prefetchEngine = 0;
}

static NvU32 PrefetchSingleSurface(NVDIFRStateEvoPtr pDifr,
                                   NVDIFRPrefetchParams *pParams,
                                   size_t *cacheRemaining)
{
    NvPushChannelPtr p = &pDifr->prefetchPushChannel;
    NvU64 semaphoreGPUAddress = nvPushGetNotifierGpuAddress(p, 0, 0);
    NvGpuSemaphore *semaphore = (NvGpuSemaphore *)
        nvPushGetNotifierCpuAddress(p, 0, 0);
    const NvKmsSurfaceMemoryFormatInfo *finfo =
        nvKmsGetSurfaceMemoryFormatInfo(pParams->surfFormat);
    NvU32 componentSizes;
    NvU32 dataTransferType;
    NvU32 line_length_in;
    NvU32 line_count;
    NvU64 starttime;
    NvU64 endtime;

    /*
     * Tell SET_REMAP_COMPONENTS the byte-size of a pixel in terms of color
     * component size and count. It doesn't matter which actual combinations we
     * choose as long as size*count will be equal to bytesPerPixel. This is
     * because we won't be doing any actual remapping per se: we will just
     * effectively tell the prefetch operation to fetch the correct amount of
     * bytes for each pixel.
     */
    switch (finfo->rgb.bytesPerPixel) {
#define COMPONENTS(size, num)                                           \
    (DRF_DEF(A0B5, _SET_REMAP_COMPONENTS, _COMPONENT_SIZE, size) |      \
     DRF_DEF(A0B5, _SET_REMAP_COMPONENTS, _NUM_SRC_COMPONENTS, num) |   \
     DRF_DEF(A0B5, _SET_REMAP_COMPONENTS, _NUM_DST_COMPONENTS, num))

    case 1: componentSizes = COMPONENTS(_ONE, _ONE); break;
    case 2: componentSizes = COMPONENTS(_ONE, _TWO); break;
    case 3: componentSizes = COMPONENTS(_ONE, _THREE); break;
    case 4: componentSizes = COMPONENTS(_ONE, _FOUR); break;
    case 6: componentSizes = COMPONENTS(_TWO, _THREE); break;
    case 8: componentSizes = COMPONENTS(_TWO, _FOUR); break;
    case 12: componentSizes = COMPONENTS(_FOUR, _THREE); break;
    case 16: componentSizes = COMPONENTS(_FOUR, _FOUR); break;
    default: componentSizes = 0; break;
#undef COMPONENTS
    }

    /*
     * TODO: For now, we don't prefetch multiplane surfaces. In order to do so
     * we'd need to loop over all valid planes of the pSurfaceEvo and issue a
     * prefetch for each plane.
     */
    if (finfo->numPlanes > 1) {
        /*
         * Regardless of its wording, this is the proper failure code to send
         * upstream. This lets the RM disable DIFR until the next modeset.
         */
        return NV2080_CTRL_LPWR_DIFR_PREFETCH_FAIL_INSUFFICIENT_L2_SIZE;
    }

    /*
     * Compute some dimensional values to obtain correct blob size for
     * prefetching. Use the given pitch and calculate the number of lines
     * needed to cover the whole memory region.
     */
    nvAssert(pParams->surfPitchBytes % finfo->rgb.bytesPerPixel == 0);
    line_length_in = pParams->surfPitchBytes / finfo->rgb.bytesPerPixel;

    nvAssert(pParams->surfSizeBytes % pParams->surfPitchBytes == 0);
    line_count = pParams->surfSizeBytes / pParams->surfPitchBytes;

    /*
     * Greedy strategy: assume all surfaces will fit in the supplied L2 size but
     * the first one that doesn't will cause the prefetch request to fail. If we
     * run out of cache then DIFR will disable itself until the next modeset.
     */
    if (*cacheRemaining < pParams->surfSizeBytes) {
        return NV2080_CTRL_LPWR_DIFR_PREFETCH_FAIL_INSUFFICIENT_L2_SIZE;
    }

    *cacheRemaining -= pParams->surfSizeBytes;

    /*
     * Push buffer DMA copy and semaphore programming.
     */
    nvPushSetObject(p, NVA06F_SUBCHANNEL_COPY_ENGINE, &pDifr->prefetchEngine);
    nvPushMethod(p, NVA06F_SUBCHANNEL_COPY_ENGINE,
                 NVA0B5_SET_REMAP_COMPONENTS, 1);
    nvPushSetMethodData(p,
                        componentSizes |
                        DRF_DEF(A0B5, _SET_REMAP_COMPONENTS, _DST_X, _CONST_A) |
                        DRF_DEF(A0B5, _SET_REMAP_COMPONENTS, _DST_Y, _CONST_A) |
                        DRF_DEF(A0B5, _SET_REMAP_COMPONENTS, _DST_Z, _CONST_A) |
                        DRF_DEF(A0B5, _SET_REMAP_COMPONENTS, _DST_W, _CONST_A));
    nvPushImmedVal(p, NVA06F_SUBCHANNEL_COPY_ENGINE,
                   NVA0B5_SET_REMAP_CONST_A, 0);
    nvPushMethod(p, NVA06F_SUBCHANNEL_COPY_ENGINE, NVA0B5_OFFSET_IN_UPPER, 2);
    nvPushSetMethodDataU64(p, pParams->surfGpuAddress);
    nvPushMethod(p, NVA06F_SUBCHANNEL_COPY_ENGINE, NVA0B5_OFFSET_OUT_UPPER, 2);
    nvPushSetMethodDataU64(p, pParams->surfGpuAddress);

    /*
     * We don't expect phenomally large pitches but the .mfs for DMA copy
     * defines PitchIn/PitchOut to be of signed 32-bit type for all
     * architectures so assert that the value will be what h/w understands.
     */
    nvAssert(pParams->surfPitchBytes <= NV_S32_MAX);

    nvPushMethod(p, NVA06F_SUBCHANNEL_COPY_ENGINE, NVA0B5_PITCH_IN, 1);
    nvPushSetMethodData(p, pParams->surfPitchBytes);
    nvPushMethod(p, NVA06F_SUBCHANNEL_COPY_ENGINE, NVA0B5_PITCH_OUT, 1);
    nvPushSetMethodData(p, pParams->surfPitchBytes);

    nvPushMethod(p, NVA06F_SUBCHANNEL_COPY_ENGINE, NVA0B5_LINE_LENGTH_IN, 1);
    nvPushSetMethodData(p, line_length_in);
    nvPushMethod(p, NVA06F_SUBCHANNEL_COPY_ENGINE, NVA0B5_LINE_COUNT, 1);
    nvPushSetMethodData(p, line_count);
    nvAssert(pParams->surfPitchBytes * line_count == pParams->surfSizeBytes);

    if (pDifr->copyEngineClass != AMPERE_DMA_COPY_B) {
        nvPushMethod(p, NVA06F_SUBCHANNEL_COPY_ENGINE, NVCAB5_REQ_ATTR, 1);
        nvPushSetMethodData
            (p, DRF_DEF(CAB5, _REQ_ATTR,   _PREFETCH_L2_CLASS, _EVICT_LAST));

        dataTransferType = DRF_DEF(CAB5, _LAUNCH_DMA, _DATA_TRANSFER_TYPE, _PREFETCH);
    } else
    {
        dataTransferType = DRF_DEF(A0B5, _LAUNCH_DMA, _DATA_TRANSFER_TYPE, _PIPELINED);
    }

    nvPushMethod(p, NVA06F_SUBCHANNEL_COPY_ENGINE, NVA0B5_LAUNCH_DMA, 1);
    nvPushSetMethodData
        (p,
         dataTransferType                                            |
         DRF_DEF(A0B5, _LAUNCH_DMA, _FLUSH_ENABLE,       _TRUE)      |
         DRF_DEF(A0B5, _LAUNCH_DMA, _SEMAPHORE_TYPE,     _NONE)      |
         DRF_DEF(A0B5, _LAUNCH_DMA, _INTERRUPT_TYPE,     _NONE)      |
         DRF_DEF(A0B5, _LAUNCH_DMA, _REMAP_ENABLE,       _TRUE)      |
         DRF_DEF(A0B5, _LAUNCH_DMA, _SRC_MEMORY_LAYOUT,  _PITCH)     |
         DRF_DEF(A0B5, _LAUNCH_DMA, _DST_MEMORY_LAYOUT,  _PITCH)     |
         DRF_DEF(A0B5, _LAUNCH_DMA, _MULTI_LINE_ENABLE,  _TRUE)      |
         DRF_DEF(A0B5, _LAUNCH_DMA, _SRC_TYPE,           _VIRTUAL)   |
         DRF_DEF(A0B5, _LAUNCH_DMA, _DST_TYPE,           _VIRTUAL));

    /*
     * Reset semaphore value. A memory barrier will be issued by nvidia-push so
     * we don't need one here.
     */
    semaphore->data[0] = 0;

    /* Program a semaphore release after prefetch DMA copy. */
    nvPushMethod(p, 0, NVA06F_SEMAPHOREA, 4);
    nvPushSetMethodDataU64(p, semaphoreGPUAddress);
    nvPushSetMethodData(p, PREFETCH_DONE_VALUE);
    nvPushSetMethodData(p,
                        DRF_DEF(A06F, _SEMAPHORED, _OPERATION, _RELEASE) |
                        DRF_DEF(A06F, _SEMAPHORED, _RELEASE_WFI, _EN) |
                        DRF_DEF(A06F, _SEMAPHORED, _RELEASE_SIZE, _4BYTE));
    nvPushKickoff(p);

    /*
     * Errors and prefetch faults are handled as follows. If prefetch
     * succeeds the semaphore release will trigger and we will exit upon
     * seeing PREFETCH_DONE_VALUE in the memory location. Upon failure we
     * will end up timing out, signal RM of the CE fault and DIFR will
     * remain disabled until next driver load.
     *
     * Currently the total launch-to-end effective (with scheduling)
     * prefetch rate on silicon seems to be around 15k pixels per
     * microsecond, empirically. Thus, the time will range from a couple of
     * hundred microseconds for a very small panel to slightly less than 2
     * milliseconds for a single 4k display. We'll wait for 100us at a time
     * and expect a realistic completion within few milliseconds at most.
     */
    starttime = nvkms_get_usec();
    do {
        endtime = nvkms_get_usec();

        if (semaphore->data[0] == PREFETCH_DONE_VALUE) {
            return NV2080_CTRL_LPWR_DIFR_PREFETCH_SUCCESS;
        }

        nvkms_usleep(100);
    } while (endtime - starttime < DIFR_PREFETCH_WAIT_PERIOD_US); /* 10ms */

    return NV2080_CTRL_LPWR_DIFR_PREFETCH_FAIL_CE_HW_ERROR;
}

static NvBool PrefetchHelperSurfaceEvo(NVDIFRStateEvoPtr pDifr,
                                       size_t *cacheRemaining,
                                       NVSurfaceEvoPtr pSurfaceEvo,
                                       NvU32 *status)
{
    NVDIFRPrefetchParams params;

    nvAssert(*status == NV2080_CTRL_LPWR_DIFR_PREFETCH_SUCCESS);

    if (!pSurfaceEvo) {
        return TRUE;
    }

    if (pSurfaceEvo->noDisplayCaching) {
        *status = NV2080_CTRL_LPWR_DIFR_PREFETCH_FAIL_OS_FLIPS_ENABLED;
        return FALSE;
    }

    /*
     * If we see the same SurfaceEvo twice (UBB, multi-head X screens, etc)
     * we only ever want to prefetch it once within a single
     * nvDIFRPrefetchSurfaces() call.
     */
    if (pSurfaceEvo->difrLastPrefetchPass == pDifr->prefetchPass) {
        return TRUE;
    }

    /*
     * Update pass counter even if we fail later: we want to try each
     * surface only once.
     */
    pSurfaceEvo->difrLastPrefetchPass = pDifr->prefetchPass;

    /* Collect copy parameters and do the prefetch. */
    params.surfGpuAddress = pSurfaceEvo->gpuAddress;
    params.surfSizeBytes = pSurfaceEvo->planes[0].rmObjectSizeInBytes;
    params.surfPitchBytes = pSurfaceEvo->planes[0].pitch;
    params.surfFormat = pSurfaceEvo->format;

    if (pSurfaceEvo->layout == NvKmsSurfaceMemoryLayoutBlockLinear) {
        params.surfPitchBytes *= NVKMS_BLOCK_LINEAR_GOB_WIDTH;
    }

    *status = PrefetchSingleSurface(pDifr, &params, cacheRemaining);

    return *status == NV2080_CTRL_LPWR_DIFR_PREFETCH_SUCCESS;
}

/*
 * Set DIFR disabled state in H/W. Return true if state was changed and it
 * was successfully signalled downstream.
 */
static NvBool SetDisabledState(NVDIFRStateEvoPtr pDifr,
                               NvBool shouldDisable)
{
    NVDevEvoPtr pDevEvo = pDifr->pDevEvo;
    NV2080_CTRL_CMD_LPWR_DIFR_CTRL_PARAMS params = { 0 };
    NvU32 ret;

    if (shouldDisable == pDifr->hwDisabled) {
        return TRUE;
    }

    params.ctrlParamVal = shouldDisable
        ? NV2080_CTRL_LPWR_DIFR_CTRL_DISABLE
        : NV2080_CTRL_LPWR_DIFR_CTRL_ENABLE;

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->pSubDevices[0]->handle,
                         NV2080_CTRL_CMD_LPWR_DIFR_CTRL,
                         &params,
                         sizeof(params));

    if (ret != NV_OK) {
        return FALSE;
    }

    pDifr->hwDisabled = shouldDisable;

    return TRUE;
}

static NvBool IsCECompatibleWithDIFR(NVDevEvoPtr pDevEvo, NvU32 instance)
{
    NV2080_CTRL_CE_GET_CAPS_V2_PARAMS params;
    NvU32 ret;

    nvkms_memset(&params, 0, sizeof(params));
    params.ceEngineType = NV2080_ENGINE_TYPE_COPY(instance);

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->pSubDevices[0]->handle,
                         NV2080_CTRL_CMD_CE_GET_CAPS_V2,
                         &params,
                         sizeof(params));

    if (ret != NVOS_STATUS_SUCCESS) {
        return FALSE;
    }

    ct_assert(sizeof(params.capsTbl) == NV2080_CTRL_CE_CAPS_TBL_SIZE);

    /* Current criteria: DIFR prefetches can't use graphics CEs. */
    if (NV2080_CTRL_CE_GET_CAP(params.capsTbl, NV2080_CTRL_CE_CAPS_CE_GRCE)) {
        return FALSE;
    }

    return TRUE;
}

/*
 * Make sure we have a pending idle timer to check back on idleness.
 */
static void EnsureIdleTimer(NVDIFRStateEvoPtr pDifr)
{
    if (!pDifr->idleTimer) {
        /* Wait 100x longer in emulation. */
        NvU64 idlePeriod =
            DIFR_IDLE_WAIT_PERIOD_US *
            (nvIsEmulationEvo(pDifr->pDevEvo) ? 100 : 1);

        pDifr->idleTimer =
            nvkms_alloc_timer(IdleTimerProc, pDifr, 0, idlePeriod);
    }
}

/*
 * An idle timer should always remain pending after a flip until further
 * flips cease and DIFR can be re-enabled.
 *
 * Currently we'll try to re-enable DIFR after a constant period of idleness
 * since the last flip but this could resonate badly with a client that's
 * rendering at the same pace.
 *
 * To avoid churn we could track the time DIFR actually did remain enabled.
 * If the enabled-period is relatively short against the disabled-period, we
 * should bump the timeout to re-enable so that we won't be retrying all the
 * time. Conversely, we should reset the bumped timeout after we actually
 * managed to sleep long enough with DIFR enabled.
 *
 * Note: There's the question of whether we should apply slight hysteresis
 * within NVKMS regarding enabling/disabling DIFR. The hardware itself does
 * some churn-limiting and practical observations show that it seems to work
 * sufficiently and I've not observed rapid, repeating prefetch requests.
 * Keeping this note here in case this matter needs to be revisited later.
 */
static void IdleTimerProc(void *dataPtr, NvU32 dataU32)
{
    NVDIFRStateEvoPtr pDifr = (NVDIFRStateEvoPtr)dataPtr;
    NvU64 now = nvkms_get_usec();
    NvU64 idlePeriod =
        DIFR_IDLE_WAIT_PERIOD_US *
        (nvIsEmulationEvo(pDifr->pDevEvo) ? 100 : 1);

    /* First free the timer that triggered us. */
    nvkms_free_timer(pDifr->idleTimer);
    pDifr->idleTimer = NULL;

    if (now - pDifr->lastFlipTime >= idlePeriod) {
        /*
         * Enough time has passed with no new flips, enable DIFR if the console
         * is not active. If the console is active then the scanout surfaces
         * will get updated by the OS console driver without any knowledge of
         * NVKMS, DIFR can not be enabled in that case; the idle timer will get
         * scheduled by nvDIFRNotifyFlip() on next modeset/flip, till then DIFR
         * will remain disabled.
         */
        if (!nvEvoIsConsoleActive(pDifr->pDevEvo)) {
            SetDisabledState(pDifr, FALSE);
        }
    } else {
        /* New flips have happened since the original, reset idle timer. */
        EnsureIdleTimer(pDifr);
    }
}
