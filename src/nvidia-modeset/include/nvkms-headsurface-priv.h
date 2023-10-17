/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __NVKMS_HEADSURFACE_PRIV_H__
#define __NVKMS_HEADSURFACE_PRIV_H__

#include "nvkms-types.h"
#include "nvkms-headsurface.h"
#include "nvkms-headsurface-config.h"
#include "nvkms-surface.h"
#include "nvkms-utils.h"

#include "nvidia-push-init.h"
#include "nvidia-3d.h"

#include "nv_list.h"

/*
 * This header file defines structures shared by the nvkms-headsurface*.c source
 * files.  To the rest of nvkms, these structures should be opaque.
 */

#define NVKMS_HEAD_SURFACE_MAX_NOTIFIERS_PER_HEAD 4
#define NVKMS_HEAD_SURFACE_MAX_NOTIFIER_SIZE 16
#define NVKMS_HEAD_SURFACE_NOTIFIER_BYTES_PER_HEAD \
    (NVKMS_HEAD_SURFACE_MAX_NOTIFIERS_PER_HEAD *   \
     NVKMS_HEAD_SURFACE_MAX_NOTIFIER_SIZE)

#define NVKMS_HEAD_SURFACE_MAX_FRAME_SEMAPHORES 2

#define NVKMS_HEAD_SURFACE_SEMAPHORE_BYTES_PER_HEAD \
    (sizeof(NvGpuSemaphore) * NVKMS_HEAD_SURFACE_MAX_FRAME_SEMAPHORES)

#define NVKMS_HEAD_SURFACE_FRAME_SEMAPHORE_DISPLAYABLE    0xFFFFFFFF
#define NVKMS_HEAD_SURFACE_FRAME_SEMAPHORE_RENDERABLE     0x11111111

/*
 * XXX NVKMS HEADSURFACE TODO: HeadSurface uses both notifiers and semaphores
 * for synchronization:
 *
 * - Notifiers to ensure the CPU waits until after the previous frame's flip
 *   completes before starting the next frame.
 *
 * - Semaphores to ensure the flip to the next frame is not started until the
 *   rendering for the next frame completes.
 *
 * We should simplify things by using semaphores for both.
 */
typedef struct _NVHsNotifiersOneSdRec {
    NvU8 notifier
        [NVKMS_MAX_HEADS_PER_DISP][NVKMS_HEAD_SURFACE_NOTIFIER_BYTES_PER_HEAD];
    NvU8 semaphore
        [NVKMS_MAX_HEADS_PER_DISP][NVKMS_HEAD_SURFACE_SEMAPHORE_BYTES_PER_HEAD];
} NVHsNotifiersOneSdRec;

#define NVKMS_HEAD_SURFACE_NOTIFIERS_SIZE_IN_BYTES 4096

ct_assert(NVKMS_HEAD_SURFACE_NOTIFIERS_SIZE_IN_BYTES >=
          sizeof(NVHsNotifiersOneSdRec));

typedef struct _NVHsNotifiersRec {

    NvU32 rmHandle;
    NvKmsSurfaceHandle nvKmsHandle;
    const NVSurfaceEvoRec *pSurfaceEvo;

    struct {
        NVHsNotifiersOneSdRec *ptr;
        struct {
            NvU8 nextSlot;
        } apiHead[NVKMS_MAX_HEADS_PER_DISP];
    } sd[NVKMS_MAX_SUBDEVICES];

    enum NvKmsNIsoFormat nIsoFormat;

} NVHsNotifiersRec;

typedef struct _NVHsSurfaceRec {

    NvKmsSurfaceHandle nvKmsHandle;
    NvU32 rmHandle;

    Nv3dBlockLinearLog2GobsPerBlock gobsPerBlock;

    const NVSurfaceEvoRec *pSurfaceEvo;

} NVHsSurfaceRec;

typedef struct _NVHsDeviceEvoRec {

    NVDevEvoRec *pDevEvo;

    NvU32 gpuVASpace;

    struct {
        Nv3dDeviceRec device;
    } nv3d;

    NVHsNotifiersRec notifiers;

} NVHsDeviceEvoRec;

enum NVHsChannelTexInfoEnum {
    NVKMS_HEADSURFACE_TEXINFO_SRC     = 0,
    /* XXX NVKMS HEADSURFACE TODO: enable all the below  */
    NVKMS_HEADSURFACE_TEXINFO_CURSOR  = 1,
    NVKMS_HEADSURFACE_TEXINFO_BLEND   = 2,
    NVKMS_HEADSURFACE_TEXINFO_OFFSET  = 3,
    NVKMS_HEADSURFACE_TEXINFO_OVERLAY = 4,
    /* NVKMS_HEADSURFACE_TEXINFO_LUT     = 5, */
    NVKMS_HEADSURFACE_TEXINFO_NUM,
};

typedef struct _NVHsChannelStatisticsOneEyeRec {
    /* Running total of the number of frames rendered by headSurface. */
    NvU64 nFrames;

    /* Running total of the GPU time spent rendering, in nanoseconds. */
    NvU64 gpuTimeSpent;

    /* We compute the FPS for 5 second periods. */
    struct {
        /*
         * Running total of the number of frames rendered by headSurface; reset
         * every 5 seconds.
         */
        NvU64 nFrames;
        /*
         * The time, in nanoseconds, when this FPS period started, so we know
         * when the 5 second period is done.
         */
        NvU64 startTime;
        /*
         * Most recently computed FPS for the last 5 second period.
         */
        NvU64 framesPerMs;
    } fps;
} NVHsChannelStatisticsOneEyeRec;

typedef struct _NVHsChannelFlipQueueEntry {
    NVListRec flipQueueEntry;
    NVHsLayerRequestedFlipState hwState;
} NVHsChannelFlipQueueEntry;

typedef struct _NVHsChannelEvoRec {

    NVDispEvoRec *pDispEvo;

    NvU32 apiHead;

    struct {
        NvPushChannelRec channel;
        NvU32 handlePool[NV_PUSH_CHANNEL_HANDLE_POOL_NUM];
    } nvPush;

    struct {
        NvU32 handle;
        Nv3dChannelRec channel;
        Nv3dRenderTexInfo texInfo[NVKMS_HEADSURFACE_TEXINFO_NUM];
    } nv3d;

    struct {
        NvU32 handle;
    } nv2d;

    /*
     * Flip request parameters are too large to declare on the stack.  We
     * preallocate them here so that we don't have to allocate and free them on
     * every headSurface flip.
     */
    struct NvKmsFlipRequestOneHead scratchParams;

    /*
     * The index into NVDevEvoRec::apiHeadSurfaceAllDisps[apiHead]::surface[] to use
     * for the next frame of headSurface.
     */
    NvU8 nextIndex;

    /*
     * When neededForSwapGroup is true, frames of headSurface are rendered to
     * alternating offsets within double-sized headSurface surfaces.  nextOffset
     * is either 0 or 1, to select the offset of the next headSurface frame.
     */
    NvU8 nextOffset;

    /*
     * HeadSurface flips are semaphore interlocked with headSurface rendering.
     * We need to use a different semaphore offset for subsequent flips.
     * frameSemaphoreIndex is used to alternate between
     * NVKMS_HEAD_SURFACE_MAX_FRAME_SEMAPHORES offsets.
     */
    NvU8 frameSemaphoreIndex;

    NVHsChannelConfig config;

    NVVBlankCallbackPtr vBlankCallback;

    /*
     * NVHsChannelEvoRec keeps a list of flip queue entries, and the "current"
     * entry.  NVHsChannelFlipQueueEntry is a single entry in the flip queue.
     *
     * Each entry describes a validated flip request.  When NVKMS is called to
     * build the next frame of headSurface, it inspects if the next entry in the
     * queue is ready to flip (e.g., any semaphore acquires have been
     * satisfied).  If the next flip queue entry is ready, we use it to replace
     * the current entry.  Otherwise, we continue to use the existing current
     * entry.
     *
     * Surfaces within an NVHsChannelFlipQueueEntry have their reference counts:
     *
     * - incremented when the NVHsChannelFlipQueueEntry is added to the flip
     *   queue.
     *
     * - decremented when the NVHsChannelFlipQueueEntry is removed from current
     *   (i.e., when we do the equivalent of "flip away").
     *
     * To simulate EVO/NVDisplay semaphore behavior, if an
     * NVHsChannelFlipQueueEntry specifies a semaphore:
     *
     * - We wait for the semaphore's acquire value to be reached before
     *   promoting the entry from the flip queue to current.
     *
     * - We write the semaphore's release value when the
     *   NVHsChannelFlipQueueEntry is removed from current (i.e., when we do the
     *   equivalent of "flip away").
     */

    struct {
        NVHsLayerRequestedFlipState current;
        NVListRec queue;
    } flipQueue[NVKMS_MAX_LAYERS_PER_HEAD];

    /*
     * This cached main layer surface needed when the main layer transitioning
     * out of headSurface due to exiting a swapgroup. I.e. in this path:
     *     nvHsConfigStop() => HsConfigRestoreMainLayerSurface()
     */
    struct {
        NVSurfaceEvoPtr pSurfaceEvo[NVKMS_MAX_EYES];
    } flipQueueMainLayerState;

    NvU64 lastCallbackUSec;

    /*
     * For NVKMS headsurface swap groups, at some point after the flip has been
     * issued, NVKMS needs to check the notifier associated with that flip to
     * see if the flip has been completed and release the deferred request
     * fifo entry associated with that flip.  This bool reflects whether that
     * check is done during the headsurface vblank interrupt callback or later
     * during the RG line 1 interrupt callback.
     */
    NvBool usingRgIntrForSwapGroups;

    /*
     * Pointer to the RG line interrupt callback object. This is needed to
     * enabled and disable the RG interrupt callback.
     */
    NVRgLine1CallbackPtr pRgIntrCallback;

#if NVKMS_PROCFS_ENABLE

    /*
     * We track statistics differently for SwapGroup and non-SwapGroup
     * headSurface; abstract the grouping into "slots".  For non-SwapGroup there
     * is only one rendered frame (one "slot").  For SwapGroup, there are three
     * different rendered frames (so three "slots").
     */
#define NVKMS_HEADSURFACE_STATS_MAX_SLOTS 3

#define NVKMS_HEADSURFACE_STATS_SEMAPHORE_BEFORE 0
#define NVKMS_HEADSURFACE_STATS_SEMAPHORE_AFTER  1

    /*
     * One semaphore before the frame, and one semaphore after the frame.
     */
#define NVKMS_HEAD_SURFACE_STATS_SEMAPHORE_STAGE_COUNT 2

    /*
     * We need semaphores for each stereo eye for each "slot".
     */
#define NVKMS_HEADSURFACE_STATS_MAX_SEMAPHORES        \
    (NVKMS_HEAD_SURFACE_STATS_SEMAPHORE_STAGE_COUNT * \
     NVKMS_MAX_EYES *                                 \
     NVKMS_HEADSURFACE_STATS_MAX_SLOTS)

    struct {

        NVHsChannelStatisticsOneEyeRec
            perEye[NVKMS_MAX_EYES][NVKMS_HEADSURFACE_STATS_MAX_SLOTS];

        /* How often we were called back before the previous frame was done. */
        NvU64 nPreviousFrameNotDone;

        /* How often we did not update HS backbuffer with non-sg content. */
        NvU64 nOmittedNonSgHsUpdates;

        /* How often did we have fullscreen swapgroup, and didn't. */
        NvU64 nFullscreenSgFrames;
        NvU64 nNonFullscreenSgFrames;

        /*
         * Statistics on which Display Memory Interface (DMI) scanline we are on
         * when headSurface is called.
         *
         * pHistogram is a dynamically allocated array of counts.  The array has
         * vVisible + 1 elements (the +1 is because the hardware-reported
         * scanline values are in the inclusive range [0,vVisible]).  Each
         * element contains how many times we've been called back while on that
         * scanline.
         *
         * When in the blanking region, there isn't a DMI scanline.  We
         * increment n{,Not}InBlankingPeriod to keep track of how often we are
         * called back while in the blanking region.
         */
        struct {
            NvU64 *pHistogram; /* array with vVisible elements */
            NvU16 vVisible;
            NvU64 nInBlankingPeriod;
            NvU64 nNotInBlankingPeriod;
        } scanLine;

    } statistics;
#else
#define NVKMS_HEADSURFACE_STATS_MAX_SEMAPHORES 0
#endif /* NVKMS_PROCFS_ENABLE */

    /*
     * We need one semaphore for the non-stall interrupt following rendering to
     * the next viewport offset with swapgroups enabled.
     */
#define NVKMS_HEADSURFACE_VIEWPORT_OFFSET_SEMAPHORE_INDEX \
    NVKMS_HEADSURFACE_STATS_MAX_SEMAPHORES

#define NVKMS_HEADSURFACE_MAX_SEMAPHORES \
    (NVKMS_HEADSURFACE_VIEWPORT_OFFSET_SEMAPHORE_INDEX + 1)

    /*
     * Whether this channel has kicked off rendering to a new viewport offset
     * for non-swapgroup content updates, but hasn't yet kicked off the
     * viewport flip to the new offset.  Used to prevent rendering a new
     * frame if rendering the previous frame took longer than a full frame of
     * scanout.
     */
    NvBool viewportFlipPending;

    /*
     * Recorded timestamp of the last headsurface flip. Used for deciding if
     * certain blits to the headsurface can be omitted.
     */
    NvU64 lastHsClientFlipTimeUs;

    /*
     * If this channel has kicked off a real flip while swapgroups were active,
     * mark this channel as using real flips instead of blits for swapgroups,
     * don't fast forward through headsurface flips (since every flip needs to
     * be kicked off with every swapgroup ready event), and skip the part of
     * the RG interrupt that would update non-swapgroup content.
     */
    NvBool swapGroupFlipping;

} NVHsChannelEvoRec;

static inline NvU8 Hs3dStatisticsGetSlot(
    const NVHsChannelEvoRec *pHsChannel,
    const NvHsNextFrameRequestType requestType,
    const NvU8 dstBufferIndex,
    const NvBool honorSwapGroupClipList)
{
    if (pHsChannel->config.neededForSwapGroup) {
        switch (requestType) {
        case NV_HS_NEXT_FRAME_REQUEST_TYPE_FIRST_FRAME:
            /*
             * SwapGroup FIRST_FRAME will render to pHsChannel->nextIndex with
             * honorSwapGroupClipList==false.
             */
            nvAssert(dstBufferIndex < 2);
            return dstBufferIndex;
        case NV_HS_NEXT_FRAME_REQUEST_TYPE_VBLANK:
            /*
             * SwapGroup VBLANK fully populates the nextIndex buffer
             * (honorSwapGroupClipList==false), and only populates the
             * non-swapgroup regions of the current index.
             */
            return honorSwapGroupClipList ? 0 : 1;
        case NV_HS_NEXT_FRAME_REQUEST_TYPE_SWAP_GROUP_READY:
            return 2;
        }
    }

    return 0; /* non-SwapGroup always only uses slot 0 */
}

/*!
 * Get the offset, in words, of the frame semaphore within NVHsNotifiersOneSdRec
 * that corresponds to (head, frameSemaphoreIndex).
 */
static inline NvU16 HsGetFrameSemaphoreOffsetInWords(
    const NVHsChannelEvoRec *pHsChannel)
{
    const NvU16 semBase =
        offsetof(NVHsNotifiersOneSdRec, semaphore[pHsChannel->apiHead]);
    const NvU16 semOffset = sizeof(NvGpuSemaphore) *
        pHsChannel->frameSemaphoreIndex;

    const NvU16 offsetInBytes = semBase + semOffset;

    /*
     * NVHsNotifiersOneSdRec::semaphore should be word-aligned, and
     * sizeof(NvGpuSemaphore) is a multiple of words, so the offset to any
     * NvGpuSemaphore within the array should be word-aligned.
     */
    nvAssert((offsetInBytes % 4) == 0);

    return offsetInBytes / 4;
}

static inline void HsIncrementFrameSemaphoreIndex(
    NVHsChannelEvoRec *pHsChannel)
{
    pHsChannel->frameSemaphoreIndex++;
    pHsChannel->frameSemaphoreIndex %= NVKMS_HEAD_SURFACE_MAX_FRAME_SEMAPHORES;
}

static inline NvU8 HsGetPreviousOffset(
    const NVHsChannelEvoRec *pHsChannel)
{
    nvAssert(pHsChannel->config.neededForSwapGroup);

    nvAssert(pHsChannel->config.surfaceSize.height ==
             (pHsChannel->config.frameSize.height * 2));

    return A_minus_b_with_wrap_U8(pHsChannel->nextOffset, 1,
                                  NVKMS_HEAD_SURFACE_MAX_BUFFERS);
}

static inline void HsIncrementNextOffset(
    const NVHsDeviceEvoRec *pHsDevice,
    NVHsChannelEvoRec *pHsChannel)
{
    nvAssert(pHsChannel->config.neededForSwapGroup);

    nvAssert(pHsChannel->config.surfaceSize.height ==
             (pHsChannel->config.frameSize.height * 2));

    pHsChannel->nextOffset++;
    pHsChannel->nextOffset %= 2;
}

static inline void HsIncrementNextIndex(
    const NVHsDeviceEvoRec *pHsDevice,
    NVHsChannelEvoRec *pHsChannel)
{
    const NVDevEvoRec *pDevEvo = pHsDevice->pDevEvo;
    const NvU32 surfaceCount =
        pDevEvo->apiHeadSurfaceAllDisps[pHsChannel->apiHead].surfaceCount;

    nvAssert(surfaceCount > 0);

    pHsChannel->nextIndex++;
    pHsChannel->nextIndex %= surfaceCount;
}

static inline void HsChangeSurfaceFlipRefCount(
    NVDevEvoPtr pDevEvo,
    NVSurfaceEvoPtr pSurfaceEvo,
    NvBool increase)
{
    if (pSurfaceEvo != NULL) {
        if (increase) {
            nvEvoIncrementSurfaceRefCnts(pSurfaceEvo);
        } else {
            nvEvoDecrementSurfaceRefCnts(pDevEvo, pSurfaceEvo);
        }
    }
}

/*!
 * Get the last NVHsLayerRequestedFlipState entry in the pHsChannel's flip queue for
 * the specified layer.
 *
 * If the flip queue is empty, return the 'current' entry.  Otherwise, return
 * the most recently queued entry.
 *
 * This function cannot fail.
 */
static inline const NVHsLayerRequestedFlipState *HsGetLastFlipQueueEntry(
    const NVHsChannelEvoRec *pHsChannel,
    const NvU8 layer)
{
    const NVListRec *pFlipQueue = &pHsChannel->flipQueue[layer].queue;
    const NVHsChannelFlipQueueEntry *pEntry;

    /*
     * XXX NVKMS HEADSURFACE TODO: use nvListIsEmpty() once bugfix_main is
     * updated to make nvListIsEmpty()'s argument const; see changelist
     * 23614050.
     *
     * if (nvListIsEmpty(pFlipQueue)) {
     */
    if (pFlipQueue->next == pFlipQueue) {
        return &pHsChannel->flipQueue[layer].current;
    }

    pEntry = nvListLastEntry(pFlipQueue,
                             NVHsChannelFlipQueueEntry,
                             flipQueueEntry);

    return &pEntry->hwState;
}

#endif /* __NVKMS_HEADSURFACE_PRIV_H__ */
