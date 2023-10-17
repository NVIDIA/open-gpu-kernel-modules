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

#include "nvkms-types.h"
#include "nvkms-headsurface.h"
#include "nvkms-headsurface-3d.h"
#include "nvkms-headsurface-priv.h"
#include "nvkms-headsurface-swapgroup.h"
#include "nvkms-utils.h"
#include "nvkms-rmapi.h"
#include "nvkms-surface.h"
#include "nvkms-sync.h"
#include "nvkms-flip.h"
#include "nvkms-private.h"
#include "nvkms-evo.h"
#include "nvkms-dma.h"
#include "nvkms-modeset.h"
#include "nvkms-rm.h"

#include <class/cl0040.h> /* NV01_MEMORY_LOCAL_USER */

static NvBool AllocNotifiers(NVHsDeviceEvoRec *pHsDevice);
static void FreeNotifiers(NVHsDeviceEvoRec *pHsDevice);
static void HsProcFsRecordFullscreenSgFrames(NVHsChannelEvoPtr pHsChannel,
                                             NvBool isFullscreen);

static NvU32 GetLog2GobsPerBlockY(NvU32 height)
{
    NvU32 log2GobsPerBlockY = 4; // 16 gobs/block

    const NvU64 heightAndOneHalf = (NvU64)height + ((NvU64)height/2ULL);
    const NvU64 nvFermiBlockLinearGobHeight = NVKMS_BLOCK_LINEAR_GOB_HEIGHT;

    // If we're wasting too much memory, cap the block height
    while ((log2GobsPerBlockY > 0U) &&
           (((nvFermiBlockLinearGobHeight * ((NvU64)1ULL << log2GobsPerBlockY))) >
            heightAndOneHalf)) {
        log2GobsPerBlockY--;
    }

    // If there is more than one gob per block,
    if (log2GobsPerBlockY > 0U) {

        // Proposed shrunk block size.
        // compute a new proposedBlockSize, based on a gob size that is half
        // of the current value (log2 - 1).  the "if(log2 > 0)" above keeps this
        // value always ">= 0".
        NvU32 proposedBlockSize =
            NVKMS_BLOCK_LINEAR_GOB_HEIGHT << (log2GobsPerBlockY - 1U);

        // While the proposedBlockSize is greater than the image size,
        while (proposedBlockSize >= height) {
            // It's safe to cut the gobs per block in half.
            --log2GobsPerBlockY;

            // If we've hit 1 gob per block, stop.
            if (log2GobsPerBlockY == 0U) {
                break;
            }
            // Otherwise, divide the proposed block dimension/size by two.
            proposedBlockSize /= 2U;
        }
    }

    return log2GobsPerBlockY;
}

static void GetLog2GobsPerBlock(
    NvU32 bytesPerPixel,
    NvU32 widthInPixels,
    NvU32 heightInPixels,
    NvU32 *pLog2GobsPerBlockY,
    NvU32 *pitchInBlocks,
    NvU64 *sizeInBytes)
{
    NvU32 xAlign, yAlign, pitchInBytes, lines;

    NvU32 log2GobsPerBlockY = GetLog2GobsPerBlockY(heightInPixels);

    xAlign = NVKMS_BLOCK_LINEAR_GOB_WIDTH - 1;
    yAlign = (NVKMS_BLOCK_LINEAR_GOB_HEIGHT << log2GobsPerBlockY) - 1;

    pitchInBytes = NV_ALIGN_UP(widthInPixels * bytesPerPixel, xAlign);
    lines = NV_ALIGN_UP(heightInPixels, yAlign);

    *pLog2GobsPerBlockY = log2GobsPerBlockY;
    *sizeInBytes = (NvU64)pitchInBytes * lines;
    *pitchInBlocks = pitchInBytes / NVKMS_BLOCK_LINEAR_GOB_WIDTH;
}

static NvU32 AllocSurfaceVidmem(
    const NVDevEvoRec *pDevEvo,
    NvU32 handle,
    NvU64 sizeInBytes)
{
    NV_MEMORY_ALLOCATION_PARAMS memAllocParams = { };

    memAllocParams.owner = NVKMS_RM_HEAP_ID;
    memAllocParams.size = sizeInBytes;
    memAllocParams.type = NVOS32_TYPE_IMAGE;

    memAllocParams.attr = DRF_DEF(OS32, _ATTR, _LOCATION, _VIDMEM) |
                          DRF_DEF(OS32, _ATTR, _PHYSICALITY, _CONTIGUOUS) |
                          DRF_DEF(OS32, _ATTR, _FORMAT, _BLOCK_LINEAR);

    memAllocParams.attr2 = DRF_DEF(OS32, _ATTR2, _GPU_CACHEABLE, _DEFAULT);

    memAllocParams.flags = NVOS32_ALLOC_FLAGS_FORCE_MEM_GROWS_DOWN |
                           NVOS32_ALLOC_FLAGS_ALIGNMENT_FORCE;

    memAllocParams.alignment = NV_EVO_SURFACE_ALIGNMENT;

    return nvRmApiAlloc(nvEvoGlobal.clientHandle,
                        pDevEvo->deviceHandle,
                        handle,
                        NV01_MEMORY_LOCAL_USER,
                        &memAllocParams);
}

NvU64 nvHsMapSurfaceToDevice(
    const NVDevEvoRec *pDevEvo,
    const NvU32 rmHandle,
    const NvU64 sizeInBytes,
    const enum NvHsMapPermissions hsMapPermissions)
{
    NvU32 ret;
    NvU32 flags = DRF_DEF(OS46, _FLAGS, _CACHE_SNOOP, _ENABLE);
    NvU64 gpuAddress = 0;

    /* pHsDevice could be NULL if we are in no3d mode. */

    if (pDevEvo->pHsDevice == NULL) {
        return gpuAddress;
    }

    switch (hsMapPermissions) {
    case NvHsMapPermissionsNone:
        return gpuAddress;
    case NvHsMapPermissionsReadWrite:
        flags |= DRF_DEF(OS46, _FLAGS, _ACCESS, _READ_WRITE);
        break;
    case NvHsMapPermissionsReadOnly:
        flags |= DRF_DEF(OS46, _FLAGS, _ACCESS, _READ_ONLY);
        break;
    }

    ret = nvRmApiMapMemoryDma(nvEvoGlobal.clientHandle,
                              pDevEvo->deviceHandle,
                              pDevEvo->nvkmsGpuVASpace,
                              rmHandle,
                              0, /* offset */
                              sizeInBytes,
                              flags,
                              &gpuAddress);

    if (ret == NVOS_STATUS_SUCCESS) {
        return gpuAddress;
    } else {
        return NV_HS_BAD_GPU_ADDRESS;
    }
}

void nvHsUnmapSurfaceFromDevice(
    const NVDevEvoRec *pDevEvo,
    const NvU32 rmHandle,
    const NvU64 gpuAddress)
{
    if ((gpuAddress == 0) || (gpuAddress == NV_HS_BAD_GPU_ADDRESS)) {
        return;
    }

    if (pDevEvo->pHsDevice == NULL) {
        return;
    }

    nvRmApiUnmapMemoryDma(nvEvoGlobal.clientHandle,
                          pDevEvo->deviceHandle,
                          pDevEvo->nvkmsGpuVASpace,
                          rmHandle,
                          0, /* flags */
                          gpuAddress);
}

/*!
 * Free an NVHsSurfaceRec, allocated by nvHsAllocSurface().
 *
 * \param[in]  pDevEvo     The device.
 * \param[in]  pHsSurface  The NVHsSurfaceRec to free.
 */
void nvHsFreeSurface(
    NVDevEvoRec *pDevEvo,
    NVHsSurfaceRec *pHsSurface)
{
    if (pHsSurface == NULL) {
        return;
    }

    if (pHsSurface->rmHandle != 0) {
        nvRmApiFree(nvEvoGlobal.clientHandle,
                    pDevEvo->deviceHandle,
                    pHsSurface->rmHandle);

        nvFreeUnixRmHandle(&pDevEvo->handleAllocator, pHsSurface->rmHandle);
        pHsSurface->rmHandle = 0;
    }

    if (pHsSurface->nvKmsHandle != 0) {
        nvEvoUnregisterSurface(pDevEvo,
                               pDevEvo->pNvKmsOpenDev,
                               pHsSurface->nvKmsHandle,
                               FALSE /* skipUpdate */);
    }

    nvFree(pHsSurface);
}

NVSurfaceEvoRec *nvHsGetNvKmsSurface(const NVDevEvoRec *pDevEvo,
                                     NvKmsSurfaceHandle surfaceHandle,
                                     const NvBool requireDisplayHardwareAccess)
{
    const NVEvoApiHandlesRec *pNvKmsOpenDevSurfaceHandles;
    NVSurfaceEvoRec *pKmsSurface;

    pNvKmsOpenDevSurfaceHandles =
        nvGetSurfaceHandlesFromOpenDevConst(pDevEvo->pNvKmsOpenDev);

    nvAssert(pNvKmsOpenDevSurfaceHandles != NULL);

    pKmsSurface =
        nvEvoGetSurfaceFromHandleNoDispHWAccessOk(pDevEvo,
                                                  pNvKmsOpenDevSurfaceHandles,
                                                  surfaceHandle);
    nvAssert(pKmsSurface != NULL);
    nvAssert(pKmsSurface->requireDisplayHardwareAccess == requireDisplayHardwareAccess);

    return pKmsSurface;
}

/*!
 * Allocate an NVHsSurfaceRec, for use with headSurface.
 *
 * Video memory is allocated, mapped into the device's GPU virtual address
 * space, and registered with NVKMS's pNvKmsOpenDev.
 *
 * Note the video memory is not cleared here, because the corresponding graphics
 * channel may not be allocated, yet.
 *
 * \param[in]  pDevEvo                      The device.
 * \param[in]  requireDisplayHardwareAccess Whether display hardware requires access.
 * \param[in]  format                       The format of the surface.
 * \param[in]  widthInPixels                The width of the surface, in pixels.
 * \param[in]  heightInPixels               The height of the surface, in pixels.
 *
 * \return  On success, an allocate NVHsSurfaceRec structure is returned.
 *          On failure, NULL is returned.
 */
NVHsSurfaceRec *nvHsAllocSurface(
    NVDevEvoRec *pDevEvo,
    const NvBool requireDisplayHardwareAccess,
    const enum NvKmsSurfaceMemoryFormat format,
    const NvU32 widthInPixels,
    const NvU32 heightInPixels)
{
    struct NvKmsRegisterSurfaceParams nvKmsParams = { };
    const NvKmsSurfaceMemoryFormatInfo *pFormatInfo =
        nvKmsGetSurfaceMemoryFormatInfo(format);
    NvU32 pitchInBlocks = 0;
    NvU64 sizeInBytes = 0;
    NvU32 log2GobsPerBlockY = 0;
    NvU32 ret = 0;
    NVHsSurfaceRec *pHsSurface = nvCalloc(1, sizeof(*pHsSurface));

    if (pHsSurface == NULL) {
        return NULL;
    }

    GetLog2GobsPerBlock(pFormatInfo->rgb.bytesPerPixel,
                        widthInPixels,
                        heightInPixels,
                        &log2GobsPerBlockY,
                        &pitchInBlocks,
                        &sizeInBytes);

    sizeInBytes = NV_ALIGN_UP(sizeInBytes, NV_EVO_SURFACE_ALIGNMENT);

    pHsSurface->rmHandle = nvGenerateUnixRmHandle(&pDevEvo->handleAllocator);

    if (pHsSurface->rmHandle == 0) {
        goto fail;
    }

    ret = AllocSurfaceVidmem(pDevEvo, pHsSurface->rmHandle, sizeInBytes);

    if (ret != NVOS_STATUS_SUCCESS) {
        nvFreeUnixRmHandle(&pDevEvo->handleAllocator, pHsSurface->rmHandle);
        pHsSurface->rmHandle = 0;

        goto fail;
    }

    pHsSurface->gobsPerBlock.y = log2GobsPerBlockY;

    /*
     * For blocklinear surfaces, the NVKMS pitch is in units of blocks, which
     * matches what GetLog2GobsPerBlock() returned to us.
     */
    nvKmsParams.request.useFd = FALSE;
    nvKmsParams.request.rmClient = nvEvoGlobal.clientHandle;
    nvKmsParams.request.widthInPixels = widthInPixels;
    nvKmsParams.request.heightInPixels = heightInPixels;
    nvKmsParams.request.layout = NvKmsSurfaceMemoryLayoutBlockLinear;
    nvKmsParams.request.format = format;
    nvKmsParams.request.noDisplayHardwareAccess = !requireDisplayHardwareAccess;
    nvKmsParams.request.log2GobsPerBlockY = log2GobsPerBlockY;

    nvKmsParams.request.planes[0].u.rmObject = pHsSurface->rmHandle;
    nvKmsParams.request.planes[0].pitch = pitchInBlocks;
    nvKmsParams.request.planes[0].rmObjectSizeInBytes = sizeInBytes;

    nvEvoRegisterSurface(pDevEvo, pDevEvo->pNvKmsOpenDev, &nvKmsParams,
                         NvHsMapPermissionsReadWrite);

    if (nvKmsParams.reply.surfaceHandle == 0) {
        goto fail;
    }

    pHsSurface->nvKmsHandle = nvKmsParams.reply.surfaceHandle;

    pHsSurface->pSurfaceEvo =
        nvHsGetNvKmsSurface(pDevEvo, pHsSurface->nvKmsHandle, requireDisplayHardwareAccess);

    if (pHsSurface->pSurfaceEvo == NULL) {
        goto fail;
    }

    return pHsSurface;

fail:
    nvHsFreeSurface(pDevEvo, pHsSurface);

    return NULL;
}

NvBool nvHsAllocDevice(
    NVDevEvoRec *pDevEvo,
    const struct NvKmsAllocDeviceRequest *pRequest)
{
    NVHsDeviceEvoRec *pHsDevice;

    nvAssert(pDevEvo->pHsDevice == NULL);

    if (!pDevEvo->isHeadSurfaceSupported) {
        return TRUE;
    }

    if (pRequest->no3d) {
        return TRUE;
    }

    pHsDevice = nvCalloc(1, sizeof(*pHsDevice));

    if (pHsDevice == NULL) {
        goto fail;
    }

    pDevEvo->pHsDevice = pHsDevice;
    pHsDevice->pDevEvo = pDevEvo;

    nvAssert(pDevEvo->nvkmsGpuVASpace);

    if (!nvHs3dAllocDevice(pHsDevice)) {
        goto fail;
    }

    if (!AllocNotifiers(pHsDevice)) {
        goto fail;
    }

    return TRUE;

fail:
    nvHsFreeDevice(pDevEvo);

    return FALSE;
}

void nvHsFreeDevice(NVDevEvoRec *pDevEvo)
{
    NVHsDeviceEvoRec *pHsDevice = pDevEvo->pHsDevice;

    if (pHsDevice == NULL) {
        return;
    }

    FreeNotifiers(pHsDevice);

    nvHs3dFreeDevice(pHsDevice);

    nvFree(pHsDevice);

    pDevEvo->pHsDevice = NULL;
}

NVHsChannelEvoPtr nvHsAllocChannel(NVDispEvoRec *pDispEvo, NvU32 apiHead)
{
    NVHsChannelEvoRec *pHsChannel = nvCalloc(1, sizeof(*pHsChannel));

    if (pHsChannel == NULL) {
        goto fail;
    }

    pHsChannel->pDispEvo = pDispEvo;
    pHsChannel->apiHead = apiHead;

    if (!nvHs3dAllocChannel(pHsChannel)) {
        goto fail;
    }

    return pHsChannel;

fail:
    nvHsFreeChannel(pHsChannel);

    return NULL;
}

void nvHsFreeChannel(NVHsChannelEvoPtr pHsChannel)
{
    if (pHsChannel == NULL) {
        return;
    }

    nvHs3dFreeChannel(pHsChannel);

    nvFree(pHsChannel);
}

static NvU32 HsGetSemaphoreIndex(
    const NVFlipNIsoSurfaceEvoHwState *pSemaSurface)
{
    const NvU32 offsetInBytes = pSemaSurface->offsetInWords * 4;
    const enum NvKmsNIsoFormat format = pSemaSurface->format;
    const NvU32 sizeOfSemaphore = nvKmsSizeOfSemaphore(format);

    /*
     * The semaphore size must be greater than zero.  Flip validation should
     * prevent us from getting here with an invalid NvKmsNIsoFormat.
     */
    nvAssert(sizeOfSemaphore > 0);

    /* The semaphore offset should be a multiple of the semaphore size. */
    nvAssert((offsetInBytes % sizeOfSemaphore) == 0);

    return offsetInBytes / sizeOfSemaphore;
}

/*!
 * Read the payload of the semaphore described in the pSemaSurface.
 */
static NvU32 HsFlipQueueReadSemaphore(
    const NVHsChannelEvoRec *pHsChannel,
    const NVFlipNIsoSurfaceEvoHwState *pSemaSurface)
{
    const enum NvKmsNIsoFormat format = pSemaSurface->format;
    const NvU32 semaphoreIndex = HsGetSemaphoreIndex(pSemaSurface);
    const NvU32 sd = pHsChannel->pDispEvo->displayOwner;
    const void *ptr;
    struct nvKmsParsedSemaphore parsedSemaphore = { };

    /* We should only get here if we have a valid semaphore surface. */
    nvAssert(pSemaSurface->pSurfaceEvo != NULL);

    ptr = pSemaSurface->pSurfaceEvo->cpuAddress[sd];

    if (ptr == NULL) {
        nvAssert(!"Semaphore surface without CPU mapping!");
        return 0;
    }

    nvKmsParseSemaphore(format, semaphoreIndex, ptr, &parsedSemaphore);

    return parsedSemaphore.payload;
}

/*!
 * Return whether the specified pFlipState is ready to flip.
 */
static NvBool HsFlipQueueEntryIsReady(
    const NVHsChannelEvoRec *pHsChannel,
    const NVHsLayerRequestedFlipState *pFlipState)
{
    const NVFlipNIsoSurfaceEvoHwState *pSemaSurface =
        &pFlipState->syncObject.u.semaphores.acquireSurface;

    if (pFlipState->syncObject.usingSyncpt) {
        return TRUE;
    }

    /*
     * If a semaphore surface was specified, check if the semaphore has reached
     * the specified acquire value.
     */
    if (pSemaSurface->pSurfaceEvo != NULL) {
        const NvU32 semaphoreValue =
            HsFlipQueueReadSemaphore(pHsChannel, pSemaSurface);

        if (pHsChannel->swapGroupFlipping) {
            // With swap group flipping, the client semaphore should be
            // written before the non-stall interrupt kicking off the flip.
            nvAssert(semaphoreValue == pFlipState->syncObject.u.semaphores.acquireValue);
        } else {
            if (semaphoreValue != pFlipState->syncObject.u.semaphores.acquireValue) {
                return FALSE;
            }
        }
    }

    /*
     * If a time stamp was specified for the flip, check if the time stamp has
     * been satisfied.
     *
     * XXX NVKMS HEADSURFACE TODO: Implement time stamp flip check.
     */

    return TRUE;
}

/*!
 * Update the reference count of all the surfaces described in the pFlipState.
 */
static void HsUpdateFlipQueueEntrySurfaceRefCount(
    NVDevEvoPtr pDevEvo,
    const NVHsLayerRequestedFlipState *pFlipState,
    NvBool increase)
{
    HsChangeSurfaceFlipRefCount(
        pDevEvo, pFlipState->pSurfaceEvo[NVKMS_LEFT], increase);

    HsChangeSurfaceFlipRefCount(
        pDevEvo, pFlipState->pSurfaceEvo[NVKMS_RIGHT], increase);

    if (!pFlipState->syncObject.usingSyncpt) {
        HsChangeSurfaceFlipRefCount(
            pDevEvo, pFlipState->syncObject.u.semaphores.acquireSurface.pSurfaceEvo, increase);

        HsChangeSurfaceFlipRefCount(
            pDevEvo, pFlipState->syncObject.u.semaphores.releaseSurface.pSurfaceEvo, increase);
    }
}

/*!
 * Update bookkeeping for "flipping away" from a pFlipState.
 */
static void HsReleaseFlipQueueEntry(
    NVDevEvoPtr pDevEvo,
    NVHsChannelEvoPtr pHsChannel,
    const NVHsLayerRequestedFlipState *pFlipState)
{
    /*
     * If a semaphore surface was specified, we can now write its release value.
     */
    if (!pFlipState->syncObject.usingSyncpt &&
        pFlipState->syncObject.u.semaphores.releaseSurface.pSurfaceEvo != NULL) {

        /*
         * XXX NVKMS HEADSURFACE TODO: write the timestamp in the EVO/NVDisplay
         * semaphore structure, based on NvKmsNIsoFormat.  The graphics channel
         * doesn't support all the NvKmsNIsoFormats, so we would need to use a
         * graphics channel semaphore release of STRUCTURE_SIZE = ONE_WORD with
         * the timestamp as payload.  It would be unfortunate to read ptimer
         * registers in order to compute the payload value.
         */

        nvHs3dReleaseSemaphore(pHsChannel,
                               pFlipState->syncObject.u.semaphores.releaseSurface.pSurfaceEvo,
                               pFlipState->syncObject.u.semaphores.releaseSurface.format,
                               pFlipState->syncObject.u.semaphores.releaseSurface.offsetInWords,
                               pFlipState->syncObject.u.semaphores.releaseValue,
                               TRUE /* allPreceedingReads */);
    }

    /*
     * HeadSurface no longer needs to read from the surfaces in pFlipState;
     * decrement their reference counts.
     */
    HsUpdateFlipQueueEntrySurfaceRefCount(pDevEvo, pFlipState, FALSE);
}

/*!
 * "Fast forward" through flip queue entries that are ready.
 *
 * \param[in,out]  pHsChannel               The headSurface channel.
 * \param[in]      layer                    The layer of the flip queue.
 * \param[in]      honorIsReadyCriteria     Honor the isReady check for
 *                                          flip queue entries.
 * \param[in]      honorMinPresentInterval  Honor the minPresentInterval in
 *                                          flip queue entries.
 */
static void HsFastForwardFlipQueue(
    NVHsChannelEvoPtr pHsChannel,
    const NvU8 layer,
    const NvBool honorIsReadyCriteria,
    const NvBool honorMinPresentInterval)
{
    NVDevEvoPtr pDevEvo = pHsChannel->pDispEvo->pDevEvo;
    NVListRec *pFlipQueue = &pHsChannel->flipQueue[layer].queue;

    /*
     * For swapgroup flips, every flip kicked off by the client needs to result
     * in a real flip in hardware, so we can't fast forward through flips here.
     */
    if (pHsChannel->config.neededForSwapGroup) {
        return;
    }

    while (!nvListIsEmpty(pFlipQueue)) {

        NVHsChannelFlipQueueEntry *pEntry =
            nvListFirstEntry(pFlipQueue,
                             NVHsChannelFlipQueueEntry,
                             flipQueueEntry);
        /*
         * Stop "fast forwarding" once we find a flip queue entry that is not
         * ready: we must not release semaphores out of order, otherwise we
         * could confuse client semaphore interlocking.
         */
        if (honorIsReadyCriteria &&
            !HsFlipQueueEntryIsReady(pHsChannel, &pEntry->hwState)) {
            break;
        }

        /*
         * Normally, we want to make sure that each MinPresentInterval > 0 flip
         * is displayed for one frame, so we shouldn't fast forward past them.
         */
        if (honorMinPresentInterval &&
            (pEntry->hwState.minPresentInterval != 0)) {
            break;
        }

        /*
         * We are "flipping away" from the flip queue entry in current.  Release
         * it, and replace it with the entry in pEntry.
         */

        HsReleaseFlipQueueEntry(pDevEvo, pHsChannel,
                                &pHsChannel->flipQueue[layer].current);

        pHsChannel->flipQueue[layer].current = pEntry->hwState;

        nvListDel(&pEntry->flipQueueEntry);
        nvFree(pEntry);
    }
}

/*!
 * Push a new entry to the end of the headSurface channel's flip queue.
 *
 * \param[in,out]  pHsChannel  The headSurface channel.
 * \param[in]      layer       The layer of the flip queue.
 * \param[in]      pFlipState    The hwState to be pushed on the flip queue.
 */
void nvHsPushFlipQueueEntry(
    NVHsChannelEvoPtr pHsChannel,
    const NvU8 layer,
    const NVHsLayerRequestedFlipState *pFlipState)
{
    NVDevEvoPtr pDevEvo = pHsChannel->pDispEvo->pDevEvo;
    NVListRec *pFlipQueue = &pHsChannel->flipQueue[layer].queue;
    NVHsChannelFlipQueueEntry *pEntry = nvCalloc(1, sizeof(*pEntry));

    if (pEntry == NULL) {
        /*
         * XXX NVKMS HEADSURFACE TODO: we cannot fail at this point in the call
         * chain (we've already committed to the flip).  Move the nvCalloc() call
         * earlier in the call chain to a point where we can fail.
         */
        return;
    }

    pEntry->hwState = *pFlipState;

    /* Increment the ref counts on the surfaces in the flip queue entry. */

    HsUpdateFlipQueueEntrySurfaceRefCount(pDevEvo, &pEntry->hwState, TRUE);

    /* "Fast forward" through existing flip queue entries that are ready. */

    HsFastForwardFlipQueue(pHsChannel, layer,
                           TRUE /* honorIsReadyCriteria */,
                           TRUE /* honorMinPresentInterval */);

    /* Append the new entry. */

    nvListAppend(&pEntry->flipQueueEntry, pFlipQueue);
}

/*!
 * Remove the first entry in the flip queue and return it.
 *
 * If the first entry in the flipQueue is ready to be consumed by headSurface,
 * remove it from the list and return it in the 'pFlipState' argument.
 *
 * If this function returns TRUE, it is the caller's responsibility to
 * eventually call
 *
 *    HsUpdateFlipQueueEntrySurfaceRefCount(pDevEvo, pFlipState, FALSE)
 *
 * for the returned pFlipState.
 *
 * \param[in,out]  pHsChannel  The headSurface channel.
 * \param[in]      layer       The layer of the flip queue.
 * \param[out]     pFlipState    The hwState that was popped off the flip queue.
 *
 * \return   Return TRUE if a flip queue entry was popped off the queue and
 *           copied into pFlipState.
 */
static NvBool HsPopFlipQueueEntry(
    NVHsChannelEvoPtr pHsChannel,
    const NvU8 layer,
    NVHsLayerRequestedFlipState *pFlipState)
{
    NVListRec *pFlipQueue = &pHsChannel->flipQueue[layer].queue;
    NVHsChannelFlipQueueEntry *pEntry;

    if (nvListIsEmpty(pFlipQueue)) {
        return FALSE;
    }

    pEntry = nvListFirstEntry(pFlipQueue,
                              NVHsChannelFlipQueueEntry,
                              flipQueueEntry);

    if (!HsFlipQueueEntryIsReady(pHsChannel, &pEntry->hwState)) {
        return FALSE;
    }

    *pFlipState = pEntry->hwState;

    nvListDel(&pEntry->flipQueueEntry);
    nvFree(pEntry);

    return TRUE;
}

/*!
 * Update the current flip queue entry for a new headSurface frame.
 *
 * To build a new frame of headSurface, we look at the flip queue of each layer.
 * If there is an entry available, we pop it off the queue and replace .current
 * with the entry.
 */
static void HsUpdateFlipQueueCurrent(
    NVHsChannelEvoPtr pHsChannel)
{
    NVDevEvoPtr pDevEvo = pHsChannel->pDispEvo->pDevEvo;
    NvU8 layer;

    for (layer = 0; layer < ARRAY_LEN(pHsChannel->flipQueue); layer++) {

        NVHsLayerRequestedFlipState newCurrent = { };

        /*
         * XXX NVKMS HEADSURFACE TODO: fast forward to the last ready flip queue
         * entry.  Share code with similar functionality in
         * nvHsPushFlipQueueEntry().
         */

        if (!HsPopFlipQueueEntry(pHsChannel, layer, &newCurrent)) {
            continue;
        }

        /*
         * We have a new flip queue entry to place in current.  Release the old
         * current flip queue entry, and replace it with the popped entry.
         */
        HsReleaseFlipQueueEntry(pDevEvo, pHsChannel,
                                &pHsChannel->flipQueue[layer].current);

        pHsChannel->flipQueue[layer].current = newCurrent;
    }
}

/*!
 * Drain the flip queue on each layer of pHsChannel.
 *
 * In preparation to disable headSurface, release the flip queue entry in
 * .current, as well as all entries in the queue.
 */
void nvHsDrainFlipQueue(
    NVHsChannelEvoPtr pHsChannel)
{
    NVDevEvoPtr pDevEvo = pHsChannel->pDispEvo->pDevEvo;
    NvU8 layer;

    for (layer = 0; layer < ARRAY_LEN(pHsChannel->flipQueue); layer++) {
        NVListRec *pFlipQueue = &pHsChannel->flipQueue[layer].queue;

        HsReleaseFlipQueueEntry(pDevEvo, pHsChannel,
                                &pHsChannel->flipQueue[layer].current);

        nvkms_memset(&pHsChannel->flipQueue[layer].current, 0,
                     sizeof(pHsChannel->flipQueue[layer].current));

        while (!nvListIsEmpty(pFlipQueue)) {

            NVHsChannelFlipQueueEntry *pEntry =
                nvListFirstEntry(pFlipQueue,
                                 NVHsChannelFlipQueueEntry,
                                 flipQueueEntry);

            HsReleaseFlipQueueEntry(pDevEvo, pHsChannel, &pEntry->hwState);

            nvListDel(&pEntry->flipQueueEntry);
            nvFree(pEntry);
        }
    }
}

/*!
 * Return whether all flip queues on this pHsChannel are idle.
 *
 * As a side effect, attempt to "fast forward" through flip queue entries, in an
 * effort to make the flip queues idle.  When fast forwarding, always ignore the
 * client-requested minPresentInterval.  Optionally (when force == TRUE), also
 * ignore the "IsReady" check.
 *
 * This is intended to be used in two scenarios:
 *
 * - First, call nvHsIdleFlipQueue(force=FALSE) in a loop with all other heads
 *   we are trying to idle.  This should allow semaphore interlocking to
 *   progress naturally.
 *
 * - If that loop times out, call nvHsIdleFlipQueue(force=TRUE), which will
 *   ignore the IsReady conditions and forcibly make the flip queues idle.
 */
NvBool nvHsIdleFlipQueue(
    NVHsChannelEvoPtr pHsChannel,
    NvBool force)
{
    const NvBool honorIsReadyCriteria = !force;
    NvBool ret = TRUE;
    NvU8 layer;

    for (layer = 0; layer < ARRAY_LEN(pHsChannel->flipQueue); layer++) {

        HsFastForwardFlipQueue(pHsChannel, layer,
                               honorIsReadyCriteria,
                               FALSE /* honorMinPresentInterval */);

        if (!nvListIsEmpty(&pHsChannel->flipQueue[layer].queue)) {
            /* force should always result in an empty flip queue */
            nvAssert(!force);
            ret = FALSE;
        }
    }

    return ret;
}

/*
 * We use notifiers to know when headSurface frames are presented, so that we
 * don't render to the visible buffer.
 */

static NvU32 AllocNotifierMemory(
    const NVDevEvoRec *pDevEvo,
    NvU32 handle)
{
    NV_MEMORY_ALLOCATION_PARAMS memAllocParams = { };

    memAllocParams.owner = NVKMS_RM_HEAP_ID;
    memAllocParams.size = NVKMS_HEAD_SURFACE_NOTIFIERS_SIZE_IN_BYTES;
    memAllocParams.type = NVOS32_TYPE_DMA;

    memAllocParams.attr = DRF_DEF(OS32, _ATTR, _LOCATION, _VIDMEM) |
                          DRF_DEF(OS32, _ATTR, _PHYSICALITY, _CONTIGUOUS) |
                          DRF_DEF(OS32, _ATTR, _PAGE_SIZE, _4KB) |
                          DRF_DEF(OS32, _ATTR, _COHERENCY, _UNCACHED);

    memAllocParams.flags = NVOS32_ALLOC_FLAGS_FORCE_MEM_GROWS_DOWN |
                           NVOS32_ALLOC_FLAGS_IGNORE_BANK_PLACEMENT |
                           NVOS32_ALLOC_FLAGS_FORCE_ALIGN_HOST_PAGE;

    memAllocParams.attr2 = DRF_DEF(OS32, _ATTR2, _ISO, _NO);

    return nvRmApiAlloc(nvEvoGlobal.clientHandle,
                       pDevEvo->deviceHandle,
                       handle,
                       NV01_MEMORY_LOCAL_USER,
                       &memAllocParams);
}

static NvBool MapNotifiers(NVHsDeviceEvoRec *pHsDevice)
{
    NVDevEvoRec *pDevEvo = pHsDevice->pDevEvo;
    NVHsNotifiersRec *pNotifiers = &pHsDevice->notifiers;
    const NvU64 size = NVKMS_HEAD_SURFACE_NOTIFIERS_SIZE_IN_BYTES;
    NvU32 sd, ret;

    for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
        ret = nvRmApiMapMemory(nvEvoGlobal.clientHandle,
                               pDevEvo->pSubDevices[sd]->handle,
                               pNotifiers->rmHandle,
                               0,
                               size,
                               (void **)&pNotifiers->sd[sd].ptr,
                               0);
        if (ret != NVOS_STATUS_SUCCESS) {
            return FALSE;
        }

        /*
         * Intentionally use NVMISC_MEMSET() rather than nvkms_memset(): some
         * CPU architectures, notably ARM, may fault if streaming stores like in
         * an optimized memset() implementation are used on a BAR1 mapping.
         * NVMISC_MEMSET() is conveniently not optimized.
         */
        NVMISC_MEMSET((void *)pNotifiers->sd[sd].ptr, 0, size);
    }

    return TRUE;
}

static void UnmapNotifiers(NVHsDeviceEvoRec *pHsDevice)
{
    NVDevEvoRec *pDevEvo = pHsDevice->pDevEvo;
    NVHsNotifiersRec *pNotifiers = &pHsDevice->notifiers;
    NvU32 sd;

    if (pNotifiers->rmHandle == 0) {
        return;
    }

    for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {

        if (pNotifiers->sd[sd].ptr == NULL) {
            continue;
        }

        nvRmApiUnmapMemory(nvEvoGlobal.clientHandle,
                           pDevEvo->pSubDevices[sd]->handle,
                           pNotifiers->rmHandle,
                           pNotifiers->sd[sd].ptr,
                           0);

        pNotifiers->sd[sd].ptr = NULL;
    }
}

static NvBool RegisterNotifiersWithNvKms(NVHsDeviceEvoRec *pHsDevice)
{
    struct NvKmsRegisterSurfaceParams params = { };
    NVHsNotifiersRec *pNotifiers = &pHsDevice->notifiers;
    NVDevEvoRec *pDevEvo = pHsDevice->pDevEvo;
    const NvBool requireDisplayHardwareAccess = TRUE;

    params.request.useFd       = FALSE;
    params.request.rmClient    = nvEvoGlobal.clientHandle;

    params.request.layout      = NvKmsSurfaceMemoryLayoutPitch;
    params.request.format      = NvKmsSurfaceMemoryFormatI8;

    params.request.isoType = NVKMS_MEMORY_NISO;

    params.request.planes[0].u.rmObject = pNotifiers->rmHandle;
    params.request.planes[0].pitch = NVKMS_HEAD_SURFACE_NOTIFIERS_SIZE_IN_BYTES;
    params.request.planes[0].rmObjectSizeInBytes =
        NVKMS_HEAD_SURFACE_NOTIFIERS_SIZE_IN_BYTES;

    nvEvoRegisterSurface(pDevEvo, pDevEvo->pNvKmsOpenDev, &params,
                         NvHsMapPermissionsReadWrite);

    pHsDevice->notifiers.nvKmsHandle = params.reply.surfaceHandle;

    if (pHsDevice->notifiers.nvKmsHandle == 0) {
        return FALSE;
    }

    pHsDevice->notifiers.pSurfaceEvo =
        nvHsGetNvKmsSurface(pDevEvo,
                            pHsDevice->notifiers.nvKmsHandle,
                            requireDisplayHardwareAccess);

    return (pHsDevice->notifiers.pSurfaceEvo != NULL);
}

static void AssignNIsoFormat(NVHsDeviceEvoRec *pHsDevice)
{
    const NVDevEvoRec *pDevEvo = pHsDevice->pDevEvo;

    if (pDevEvo->caps.validNIsoFormatMask &
        NVBIT(NVKMS_NISO_FORMAT_FOUR_WORD_NVDISPLAY)) {
        /* If available, use the "nvdisplay" format. */
        pHsDevice->notifiers.nIsoFormat = NVKMS_NISO_FORMAT_FOUR_WORD_NVDISPLAY;
    } else {
        /* Otherwise, use the "legacy" format. */
        nvAssert((pDevEvo->caps.validNIsoFormatMask &
                  NVBIT(NVKMS_NISO_FORMAT_LEGACY)) != 0);
        pHsDevice->notifiers.nIsoFormat = NVKMS_NISO_FORMAT_LEGACY;
    }
}

static NvBool AllocNotifiers(NVHsDeviceEvoRec *pHsDevice)
{
    NvU32 ret;
    NVDevEvoRec *pDevEvo;

    pDevEvo = pHsDevice->pDevEvo;

    pHsDevice->notifiers.rmHandle = nvGenerateUnixRmHandle(&pDevEvo->handleAllocator);

    if (pHsDevice->notifiers.rmHandle == 0) {
        goto fail;
    }

    ret = AllocNotifierMemory(pHsDevice->pDevEvo, pHsDevice->notifiers.rmHandle);

    if (ret != NVOS_STATUS_SUCCESS) {
        nvFreeUnixRmHandle(&pDevEvo->handleAllocator,
                           pHsDevice->notifiers.rmHandle);
        pHsDevice->notifiers.rmHandle = 0;

        goto fail;
    }

    if (!MapNotifiers(pHsDevice)) {
        goto fail;
    }

    if (!RegisterNotifiersWithNvKms(pHsDevice)) {
        goto fail;
    }

    AssignNIsoFormat(pHsDevice);

    return TRUE;

fail:
    FreeNotifiers(pHsDevice);

    return FALSE;
}

static void FreeNotifiers(NVHsDeviceEvoRec *pHsDevice)
{
    NVDevEvoRec *pDevEvo;
    NVHsNotifiersRec *pNotifiers;

    if (pHsDevice == NULL) {
        return;
    }

    pDevEvo = pHsDevice->pDevEvo;
    pNotifiers = &pHsDevice->notifiers;

    if (pNotifiers->nvKmsHandle != 0) {
        nvEvoUnregisterSurface(pDevEvo,
                               pDevEvo->pNvKmsOpenDev,
                               pNotifiers->nvKmsHandle,
                               FALSE /* skipUpdate */);
        pNotifiers->pSurfaceEvo = NULL;
    }

    UnmapNotifiers(pHsDevice);

    if (pHsDevice->notifiers.rmHandle != 0) {
        nvRmApiFree(nvEvoGlobal.clientHandle,
                    pDevEvo->deviceHandle,
                    pHsDevice->notifiers.rmHandle);

        nvFreeUnixRmHandle(&pDevEvo->handleAllocator,
                           pHsDevice->notifiers.rmHandle);
        pHsDevice->notifiers.rmHandle = 0;
    }
}

/*!
 * Reset headSurface notifiers for this channel to NOT_BEGUN.
 *
 * By the time the modeset completes to transition into a new headSurface
 * configuration, all headSurface flips from the previous completion should be
 * completed.  But, that would leave at least one notifier set to FINISHED.
 *
 * Initialize all notifiers for this channel to NOT_BEGUN, so that
 * HsVBlankCallbackDeferredWork() does not interpret notifier state from the
 * previous headSurface configuration as applying to the new headSurface
 * configuration.
 */
static void HsInitNotifiers(
    NVHsDeviceEvoRec *pHsDevice,
    NVHsChannelEvoRec *pHsChannel)
{
    const NvU32 apiHead = pHsChannel->apiHead;
    const NvU32 sd = pHsChannel->pDispEvo->displayOwner;
    NVHsNotifiersRec *pHsNotifiers = &pHsDevice->notifiers;
    NVHsNotifiersOneSdRec *pHsNotifiersOneSd = pHsNotifiers->sd[sd].ptr;
    NvU8 slot, buffer;

    for (slot = 0; slot < NVKMS_HEAD_SURFACE_MAX_NOTIFIERS_PER_HEAD; slot++) {
        nvKmsResetNotifier(pHsNotifiers->nIsoFormat,
                           FALSE /* overlay */,
                           slot,
                           pHsNotifiersOneSd->notifier[apiHead]);
    }

    for (buffer = 0; buffer < NVKMS_HEAD_SURFACE_MAX_BUFFERS; buffer++) {
        nvKmsResetSemaphore(pHsNotifiers->nIsoFormat,
                            buffer, pHsNotifiersOneSd->semaphore[apiHead],
                            NVKMS_HEAD_SURFACE_FRAME_SEMAPHORE_RENDERABLE);
    }
}

void nvHsInitNotifiers(
    NVHsDeviceEvoRec *pHsDevice,
    NVHsChannelEvoRec *pHsChannel)
{
    if (pHsChannel->config.neededForSwapGroup) {
        /*
         * XXX NVKMS HEADSURFACE TODO: initialize tracking for ViewPortIn
         * flips.
         */
    } else {
        HsInitNotifiers(pHsDevice, pHsChannel);
    }
}

/*!
 * For the given head and sd, prepare the next notifier:
 *
 * - Look up the next notifier to use.
 * - Clear that notifier to STATUS_NOT_BEGUN.
 * - Update the slot bookkeeping for the (head,sd) pair.
 * - Return the dword offset of the notifier.
 */
static NvU16 PrepareNextNotifier(
    NVHsNotifiersRec *pHsNotifiers,
    NvU32 sd,
    NvU32 apiHead)
{
    const NvU32 notifierSize =
        nvKmsSizeOfNotifier(pHsNotifiers->nIsoFormat, FALSE /* overlay */);

    const NvU8 nextSlot = pHsNotifiers->sd[sd].apiHead[apiHead].nextSlot;

    NVHsNotifiersOneSdRec *pHsNotifiersOneSd = pHsNotifiers->sd[sd].ptr;

    const NvU8 *headBase = pHsNotifiersOneSd->notifier[apiHead];

    const NvU8 offsetInBytes =
        (headBase - ((const NvU8 *) pHsNotifiersOneSd)) +
        (notifierSize * nextSlot);

    nvAssert(notifierSize <= NVKMS_HEAD_SURFACE_MAX_NOTIFIER_SIZE);

    nvKmsResetNotifier(pHsNotifiers->nIsoFormat, FALSE /* overlay */,
                       nextSlot, pHsNotifiersOneSd->notifier[apiHead]);

    pHsNotifiers->sd[sd].apiHead[apiHead].nextSlot =
        (nextSlot + 1) % NVKMS_HEAD_SURFACE_MAX_NOTIFIERS_PER_HEAD;

    return offsetInBytes / 4;
}

/*!
 * Helper function for nvHsFlip(); populate NvKmsFlipRequest and call
 * nvFlipEvo().
 *
 * \param[in,out]  pHsDevice         The headSurface device.
 * \param[in,out]  pHsChannel        The headSurface channel.
 * \param[in]      perEyeStereoFlip  Whether to flip per-eye.
 * \param[in]      surfaceHandles    The surfaces to flip to.
 * \param[in]      isFirstFlip       Whether this is the first flip after
 *                                   enabling headsurface.
 * \param[in]      allowFlipLock     Whether to allow fliplock for this flip.
 */
static void HsFlipHelper(
    NVHsDeviceEvoRec *pHsDevice,
    NVHsChannelEvoRec *pHsChannel,
    const NvBool perEyeStereoFlip,
    const NvKmsSurfaceHandle surfaceHandles[NVKMS_MAX_EYES],
    const NvBool isFirstFlip,
    const NvBool allowFlipLock)
{
    NVDevEvoRec *pDevEvo = pHsDevice->pDevEvo;
    struct NvKmsFlipCommonParams *pParamsOneHead;
    NVHsNotifiersRec *pHsNotifiers = &pHsDevice->notifiers;
    const NvU32 sd = pHsChannel->pDispEvo->displayOwner;
    const NvU32 apiHead = pHsChannel->apiHead;
    NvBool ret;

    /*
     * Use preallocated memory, so that we don't have to allocate
     * memory here (and deal with allocation failure).
     */
    struct NvKmsFlipRequestOneHead *pFlipHead = &pHsChannel->scratchParams;

    nvkms_memset(pFlipHead, 0, sizeof(*pFlipHead));

    pFlipHead->sd = sd;
    pFlipHead->head = apiHead;
    pParamsOneHead = &pFlipHead->flip;

    if (isFirstFlip) {
        /*
         * For the first flip after enabling headsurface
         * (NV_HS_NEXT_FRAME_REQUEST_TYPE_FIRST_FRAME), the old viewport
         * (saved in HsConfigInitSwapGroupOneHead or HsConfigInitModesetOneHead
         * and restored in HsConfigRestoreMainLayerSurface) which may specify an
         * offset within a multi-head surface needs to be overridden to the
         * origin for the per-head headsurface surfaces.
         */
        pParamsOneHead->viewPortIn.specified = TRUE;
        pParamsOneHead->viewPortIn.point.x = 0;
        pParamsOneHead->viewPortIn.point.y = 0;

        pParamsOneHead->cursor.imageSpecified = TRUE;

        pParamsOneHead->cursor.positionSpecified = TRUE;
    }

    pParamsOneHead->layer[NVKMS_MAIN_LAYER].surface.handle[NVKMS_LEFT] =
        surfaceHandles[NVKMS_LEFT];
    pParamsOneHead->layer[NVKMS_MAIN_LAYER].surface.handle[NVKMS_RIGHT] =
        surfaceHandles[NVKMS_RIGHT];
    pParamsOneHead->layer[NVKMS_MAIN_LAYER].surface.specified = TRUE;
    pParamsOneHead->layer[NVKMS_MAIN_LAYER].syncObjects.val.useSyncpt = FALSE;
    pParamsOneHead->layer[NVKMS_MAIN_LAYER].syncObjects.specified = TRUE;
    pParamsOneHead->layer[NVKMS_MAIN_LAYER].tearing = FALSE;
    pParamsOneHead->layer[NVKMS_MAIN_LAYER].perEyeStereoFlip = perEyeStereoFlip;
    pParamsOneHead->layer[NVKMS_MAIN_LAYER].minPresentInterval = 1;
    pParamsOneHead->layer[NVKMS_MAIN_LAYER].csc.specified = TRUE;
    pParamsOneHead->lut.input.specified = FALSE;
    pParamsOneHead->lut.output.specified = FALSE;

    /*
     * XXX NVKMS HEADSURFACE TODO: Work out in which cases we should use the
     * head's current CSC or LUT.
     */
    pParamsOneHead->layer[NVKMS_MAIN_LAYER].csc.matrix = NVKMS_IDENTITY_CSC_MATRIX;

    pParamsOneHead->layer[NVKMS_MAIN_LAYER].completionNotifier.specified = TRUE;

    if (surfaceHandles[NVKMS_LEFT] != 0) {
        NVEvoApiHandlesRec *pOpenDevSurfaceHandles =
            nvGetSurfaceHandlesFromOpenDev(pDevEvo->pNvKmsOpenDev);
        NVSurfaceEvoPtr pSurfaceEvo =
            nvEvoGetPointerFromApiHandle(pOpenDevSurfaceHandles, surfaceHandles[NVKMS_LEFT]);
        struct NvKmsSemaphore *pSema;

        pParamsOneHead->layer[NVKMS_MAIN_LAYER].completionNotifier.val.surface.surfaceHandle =
            pHsNotifiers->nvKmsHandle;
        pParamsOneHead->layer[NVKMS_MAIN_LAYER].completionNotifier.val.surface.format =
            pHsNotifiers->nIsoFormat;
        pParamsOneHead->layer[NVKMS_MAIN_LAYER].completionNotifier.val.surface.offsetInWords =
            PrepareNextNotifier(pHsNotifiers, sd, apiHead);

        pParamsOneHead->layer[NVKMS_MAIN_LAYER].syncObjects.val.useSyncpt = FALSE;
        pParamsOneHead->layer[NVKMS_MAIN_LAYER].syncObjects.specified = TRUE;

        pSema = &pParamsOneHead->layer[NVKMS_MAIN_LAYER].syncObjects.val.u.semaphores.acquire;
        pSema->surface.surfaceHandle = pHsNotifiers->nvKmsHandle;
        pSema->surface.format = pHsNotifiers->nIsoFormat;
        pSema->surface.offsetInWords =
            HsGetFrameSemaphoreOffsetInWords(pHsChannel);
        pSema->value = NVKMS_HEAD_SURFACE_FRAME_SEMAPHORE_DISPLAYABLE;

        pParamsOneHead->layer[NVKMS_MAIN_LAYER].syncObjects.val.u.semaphores.release =
            pParamsOneHead->layer[NVKMS_MAIN_LAYER].syncObjects.val.u.semaphores.acquire;

        pParamsOneHead->layer[NVKMS_MAIN_LAYER].syncObjects.val.u.semaphores.release.value =
            NVKMS_HEAD_SURFACE_FRAME_SEMAPHORE_RENDERABLE;

        pParamsOneHead->layer[NVKMS_MAIN_LAYER].sizeIn.specified = TRUE;
        pParamsOneHead->layer[NVKMS_MAIN_LAYER].sizeIn.val.width =
            pSurfaceEvo->widthInPixels;
        pParamsOneHead->layer[NVKMS_MAIN_LAYER].sizeIn.val.height =
            pSurfaceEvo->heightInPixels;

        pParamsOneHead->layer[NVKMS_MAIN_LAYER].sizeOut.specified = TRUE;
        pParamsOneHead->layer[NVKMS_MAIN_LAYER].sizeOut.val =
            pParamsOneHead->layer[NVKMS_MAIN_LAYER].sizeIn.val;
    }

    ret = nvFlipEvo(pDevEvo,
                    pDevEvo->pNvKmsOpenDev,
                    pFlipHead,
                    1     /* numFlipHeads */,
                    TRUE  /* commit */,
                    FALSE /* allowVrr */,
                    NULL  /* pReply */,
                    FALSE /* skipUpdate */,
                    allowFlipLock);

    if (!ret) {
        nvAssert(!"headSurface flip failed?");
    }
}

/*!
 * Flip to the headSurface buffer specified by index.
 *
 * If pHsOneHeadAllDisps == NULL, disable headSurface by flipping to NULL.
 *
 * \param[in,out]  pHsDevice           The headSurface device.
 * \param[in,out]  pHsChannel          The headSurface channel.
 * \param[in]      eyeMask             The mask of which eyes to flip.
 * \param[in]      perEyeStereoFlip    Whether to flip per-eye.
 * \param[in]      index               Which buffer to flip to.
 * \param[in]      pHsOneHeadAllDisps  The headSurface config.
 * \param[in]      isFirstFlip         Whether this is the first flip after
 *                                     enabling headsurface.
 * \param[in]      allowFlipLock       Whether to allow fliplock for this flip.
 */
void nvHsFlip(
    NVHsDeviceEvoRec *pHsDevice,
    NVHsChannelEvoRec *pHsChannel,
    const NvU8 eyeMask,
    const NvBool perEyeStereoFlip,
    const NvU8 index,
    const NVHsStateOneHeadAllDisps *pHsOneHeadAllDisps,
    const NvBool isFirstFlip,
    const NvBool allowFlipLock)
{
    NvKmsSurfaceHandle surfaceHandles[NVKMS_MAX_EYES] = { 0, 0 };
    const NvBool enable = (pHsOneHeadAllDisps != NULL);

    if (enable) {
        NvU8 eye;

        for (eye = NVKMS_LEFT; eye < NVKMS_MAX_EYES; eye++) {

            const NVHsSurfaceRec *pHsSurface =
                pHsOneHeadAllDisps->surfaces[eye][index].pSurface;

            if ((eyeMask & NVBIT(eye)) == 0) {
                continue;
            }

            nvAssert(pHsSurface != NULL);

            surfaceHandles[eye] = pHsSurface->nvKmsHandle;
            nvAssert(surfaceHandles[eye] != 0);
        }
    }

    HsFlipHelper(pHsDevice,
                 pHsChannel,
                 perEyeStereoFlip,
                 surfaceHandles,
                 isFirstFlip,
                 allowFlipLock);

    if (!enable) {
        /* XXX NVKMS HEADSURFACE TODO: disable stereo toggling, if necessary. */
    }
}

/*!
 * "Flip" using the core channel's ViewPortIn.
 */
static void HsFlipViewPortIn(NVHsChannelEvoPtr pHsChannel, NvU16 x, NvU16 y)
{
    const NVDispEvoRec *pDispEvo = pHsChannel->pDispEvo;

    /*
     * XXX NVKMS HEADSURFACE TODO: use the panning NVKMS API request, rather
     * than call the low-level SetViewportPointIn() HAL proc.  But, to do that,
     * we would need to make the pan request much lighter weight, so that it is
     * usable for our needs here.
     */
    nvApiHeadSetViewportPointIn(pDispEvo, pHsChannel->apiHead, x, y);

    /*
     * XXX NVKMS HEADSURFACE TODO: Add tracking so that IsPreviousFrameDone()
     * can know if this update latched.
     */
}

static void HsPickSrcEyeAndPixelShift(
    const NVHsChannelEvoRec *pHsChannel,
    const NvU8 dstEye,
    NvU8 *pSrcEye,
    enum NvKmsPixelShiftMode *pPixelShift)
{
    if (pHsChannel->config.pixelShift == NVKMS_PIXEL_SHIFT_8K) {

        if (dstEye == NVKMS_LEFT) {
            *pSrcEye = NVKMS_LEFT;
            *pPixelShift = NVKMS_PIXEL_SHIFT_4K_BOTTOM_RIGHT;
        }

        if (dstEye == NVKMS_RIGHT) {
            *pSrcEye = NVKMS_LEFT;
            *pPixelShift = NVKMS_PIXEL_SHIFT_4K_TOP_LEFT;
        }
    } else {
        *pSrcEye = dstEye;
        *pPixelShift = pHsChannel->config.pixelShift;
    }
}

/*!
 * Structure to drive the behavior of nvHsNextFrame().
 */
struct NvHsNextFrameWorkArea {

    /*
     * The range of surface indices to render to.  Indices here are used as the
     * 'index' in NVHsStateOneHeadAllDisps::surfaces[eye][index]::pSurface.
     */
    NvU8 dstBufferIndexStart;
    NvU8 dstBufferIndexEnd;

    /* Whether to flip to the surface indicated by pHsChannel->nextIndex. */
    NvBool doFlipToNextIndex;

    /* Whether to allow fliplock on the flip to the next surface. */
    NvBool allowFlipLock;

    /* Whether to flip to the destRect region of the surface.*/
    NvBool doFlipToDestRect;

    /* Whether to increment nextIndex and/or nextOffset. */
    NvBool doIncrementNextIndex;
    NvBool doIncrementNextOffset;

    /*
     * On which dstBuffer indices to honor the SwapGroup's exclusive
     * clip list.
     */
    NvU8 honorSwapGroupClipListBufferMask;

    /* The region within the surface to render into.  */
    struct NvKmsRect destRect;

    /*
     * If perEyeStereo::override == TRUE, use perEyeStereo::value to control the
     * headSurface flip.
     */
    struct {
        NvBool override;
        NvBool value;
    } perEyeStereo;
};

/*!
 * Assign an NvHsNextFrameWorkArea structure, to drive execution of
 * nvHsNextFrame().
 */
static struct NvHsNextFrameWorkArea HsAssignNextFrameWorkArea(
    const NVHsChannelEvoRec *pHsChannel,
    const NvHsNextFrameRequestType requestType)
{
    struct NvHsNextFrameWorkArea workArea = { };
    NvU8 destOffset;

    if ((requestType == NV_HS_NEXT_FRAME_REQUEST_TYPE_FIRST_FRAME) ||
        (requestType == NV_HS_NEXT_FRAME_REQUEST_TYPE_VBLANK)) {

        /*
         * The swapgroup first frame renders and flips both core and base to
         * the back index double height headsurface swapgroup surface, just
         * like a non-swapgroup headsurface flip.
         */
        if (requestType == NV_HS_NEXT_FRAME_REQUEST_TYPE_FIRST_FRAME ||
            !pHsChannel->config.neededForSwapGroup) {

            /*
             * In the non-SwapGroup case, headSurface should:
             * - only render to the 'nextIndex' surface,
             * - flip to the nextIndex surface,
             * - increment nextIndex.
             */
            workArea.dstBufferIndexStart   = pHsChannel->nextIndex;
            workArea.dstBufferIndexEnd     = pHsChannel->nextIndex;

            workArea.doFlipToNextIndex     = TRUE;
            workArea.allowFlipLock         = FALSE;
            workArea.doFlipToDestRect      = FALSE;

            workArea.doIncrementNextIndex  = TRUE;
            workArea.doIncrementNextOffset = FALSE;

        } else {

            /*
             * In the SwapGroup case, headSurface should:
             * - render to both surfaces,
             * - flip to the nextOffset,
             * - increment nextOffset.
             */
            workArea.dstBufferIndexStart   = 0;
            workArea.dstBufferIndexEnd     = NVKMS_HEAD_SURFACE_MAX_BUFFERS - 1;

            workArea.doFlipToNextIndex     = FALSE;

            workArea.allowFlipLock         = FALSE;
            workArea.doFlipToDestRect      = TRUE;

            workArea.doIncrementNextIndex  = FALSE;
            workArea.doIncrementNextOffset = TRUE;

            /*
             * For VBLANK-initiated frames of SwapGroup headSurface, we want the
             * surface indicated by pHsChannel->nextIndex to contain the new
             * SwapGroup content, and the non-nextIndex surface to contain the
             * old SwapGroup content.
             *
             * Therefore, set the non-nextIndex bit(s) in
             * honorSwapGroupClipListBufferMask, so that we leave the old
             * SwapGroup content in that case.  In all other cases, we will get
             * the new SwapGroup content.
             */
            workArea.honorSwapGroupClipListBufferMask =
                ~NVBIT(pHsChannel->nextIndex);
        }

    } else {
        /*
         * SWAP_GROUP_READY-initiated headSurface frames are special: we render
         * a new frame to the nextIndex surface, using the previous destRect
         * (i.e., the location that ViewPortIn will use at the next vblank).
         * However, the flip may take indefinitely long to arrive: it will wait
         * for the rest of the SwapBarrier.  That is okay, because
         * nvHsNextFrame(VBLANK) calls between now and the flip actually
         * occurring will keep updating both surfaces, using ViewPortIn to
         * "flip" to the new content.
         *
         * Therefore, we do _not_ increment nextIndex here.  Instead, we update
         * nextIndex when we find that the flip completed.  Until then, we keep
         * nextIndex the same, so that nvHsNextFrame(VBLANK) frames know which
         * surface should receive the new SwapGroup content.
         */

        const NVSwapGroupRec *pSwapGroup =
            pHsChannel->pDispEvo->pSwapGroup[pHsChannel->apiHead];

        nvAssert(requestType == NV_HS_NEXT_FRAME_REQUEST_TYPE_SWAP_GROUP_READY);
        nvAssert(pHsChannel->config.neededForSwapGroup);

        workArea.dstBufferIndexStart   = pHsChannel->nextIndex;
        workArea.dstBufferIndexEnd     = pHsChannel->nextIndex;

        workArea.doFlipToNextIndex     = TRUE;
        workArea.allowFlipLock         = TRUE;
        workArea.doFlipToDestRect      = FALSE;

        workArea.doIncrementNextIndex  = FALSE;
        workArea.doIncrementNextOffset = FALSE;

        workArea.perEyeStereo.override = TRUE;
        workArea.perEyeStereo.value    =
            nvHsSwapGroupGetPerEyeStereo(pSwapGroup);
    }

    /*
     * Pick the rect within the destination surface that headSurface should
     * render into.
     *
     * For normal (!neededForSwapGroup) use, this should be simply:
     *   { 0, 0,                frameSize.width, frameSize.height }
     * When SwapGroups are enabled, the headSurface is allocated at
     * double height and we alternate between
     *   { 0, 0,                frameSize.width, frameSize.height }
     *   { 0, frameSize.height, frameSize.width, frameSize.height }
     * And use ViewPortIn to flip to the updated half.
     *
     * The 'nextOffset' field tracks which half headSurface should use for the
     * next frame.
     *
     * The exception to the above is SWAP_GROUP_READY: in that case, we will
     * flip between surfaces, but not change ViewPortIn, so we want to use the
     * _previous_ nextOffset value.
     */
    if (requestType == NV_HS_NEXT_FRAME_REQUEST_TYPE_SWAP_GROUP_READY) {
        destOffset = HsGetPreviousOffset(pHsChannel);
    } else {
        destOffset = pHsChannel->nextOffset;
    }

    workArea.destRect.x      = 0;
    workArea.destRect.y      = pHsChannel->config.frameSize.height *
                               destOffset;
    workArea.destRect.width  = pHsChannel->config.frameSize.width;
    workArea.destRect.height = pHsChannel->config.frameSize.height;

    return workArea;
}

/*!
 * Produce the next headSurface frame.
 *
 * Render the frame, flip to it, and update next{Index,Offset} bookkeeping
 * as necessary.
 *
 * \param[in,out]  pHsDevice   The device to render on.
 * \param[in,out]  pHsChannel  The channel to use for rendering.
 * \param[in]      requestType This indicates the type of frame behavior
 *                             desired by the caller: when FIRST_FRAME, we need
 *                             to populate the surface in the core channel on
 *                             pre-NVDisplay.
 */
void nvHsNextFrame(
    NVHsDeviceEvoPtr pHsDevice,
    NVHsChannelEvoPtr pHsChannel,
    const NvHsNextFrameRequestType requestType)
{
    const NVDevEvoRec *pDevEvo = pHsDevice->pDevEvo;
    const NVHsStateOneHeadAllDisps *pHsOneHeadAllDisps =
        &pDevEvo->apiHeadSurfaceAllDisps[pHsChannel->apiHead];
    NvBool perEyeStereoFlip = FALSE;
    NvU8 dstEye;
    NvU8 eyeMask = 0;

    struct NvHsNextFrameWorkArea workArea =
        HsAssignNextFrameWorkArea(pHsChannel, requestType);

    HsUpdateFlipQueueCurrent(pHsChannel);

    for (dstEye = NVKMS_LEFT; dstEye < NVKMS_MAX_EYES; dstEye++) {

        const NVSurfaceEvoRec *pSurfaceEvo[NVKMS_MAX_LAYERS_PER_HEAD];
        NvBool surfacesPresent = FALSE;
        NvU8 layer, srcEye = dstEye;
        NvU8 dstBufferIndex;
        enum NvKmsPixelShiftMode pixelShift = pHsChannel->config.pixelShift;
        NvBool ret;

        HsPickSrcEyeAndPixelShift(pHsChannel, dstEye, &srcEye, &pixelShift);

        for (layer = 0; layer < ARRAY_LEN(pHsChannel->flipQueue); layer++) {
            pSurfaceEvo[layer] =
                pHsChannel->flipQueue[layer].current.pSurfaceEvo[srcEye];

            surfacesPresent = surfacesPresent || (pSurfaceEvo[layer] != NULL);

            perEyeStereoFlip = perEyeStereoFlip ||
                pHsChannel->flipQueue[layer].current.perEyeStereoFlip;
        }

        /*
         * If there are no surfaces present for this srcEye, and the dstEye is
         * not LEFT, don't render it.
         *
         * This condition is limited to LEFT because:
         * - We need to perform _a_ flip even if no source surface is provided.
         * - We don't want to perform more rendering than absolutely
         *   unnecessarily.
         */
        if (!surfacesPresent && (dstEye != NVKMS_LEFT)) {
            continue;
        }

        for (dstBufferIndex = workArea.dstBufferIndexStart;
             dstBufferIndex <= workArea.dstBufferIndexEnd;
             dstBufferIndex++) {

            NvU8 thisEyeMask = 0;
            const NvBool honorSwapGroupClipList =
                !!(workArea.honorSwapGroupClipListBufferMask &
                   NVBIT(dstBufferIndex));

            ret = nvHs3dRenderFrame(pHsChannel,
                                    requestType,
                                    honorSwapGroupClipList,
                                    dstEye,
                                    dstBufferIndex,
                                    pixelShift,
                                    workArea.destRect,
                                    pSurfaceEvo);
            /*
             * Record which eyes we've rendered, so that we only flip those
             * eyes.
             *
             * In the case that we're looping over multiple buffer indices, we
             * should get the same result across buffers.
             */
            if (ret) {
                thisEyeMask = NVBIT(dstEye);
            }

            if (dstBufferIndex != workArea.dstBufferIndexStart) {
                nvAssert((eyeMask & NVBIT(dstEye)) ==
                         (thisEyeMask & NVBIT(dstEye)));
            }

            eyeMask |= thisEyeMask;
        }
    }

    if (workArea.doFlipToNextIndex) {

        if (workArea.perEyeStereo.override) {
            perEyeStereoFlip = workArea.perEyeStereo.value;
        }

        nvHs3dReleaseSemaphore(
            pHsChannel,
            pHsDevice->notifiers.pSurfaceEvo,
            pHsDevice->notifiers.nIsoFormat,
            HsGetFrameSemaphoreOffsetInWords(pHsChannel),
            NVKMS_HEAD_SURFACE_FRAME_SEMAPHORE_DISPLAYABLE,
            FALSE /* allPreceedingReads */);

        nvHsFlip(
            pHsDevice,
            pHsChannel,
            eyeMask,
            perEyeStereoFlip,
            pHsChannel->nextIndex,
            pHsOneHeadAllDisps,
            requestType == NV_HS_NEXT_FRAME_REQUEST_TYPE_FIRST_FRAME,
            workArea.allowFlipLock);
        HsIncrementFrameSemaphoreIndex(pHsChannel);

        // Record fullscreen/non-fullscreen swapgroup flip counts
        const NVSwapGroupRec *pSwapGroup =
            pHsChannel->pDispEvo->pSwapGroup[pHsChannel->apiHead];

        if (pSwapGroup) {
            HsProcFsRecordFullscreenSgFrames(pHsChannel,
                                             pSwapGroup->swapGroupIsFullscreen);
        }

        // Record the time of the last flip originating from client update
        if (requestType == NV_HS_NEXT_FRAME_REQUEST_TYPE_SWAP_GROUP_READY) {
            pHsChannel->lastHsClientFlipTimeUs = nvkms_get_usec();
        }
    }

    if (workArea.doFlipToDestRect) {
        // Viewport fake flips are only used in swapgroup configurations.
        nvAssert(pHsChannel->config.neededForSwapGroup);

        if (pHsChannel->usingRgIntrForSwapGroups) {
            nvHs3dPushPendingViewportFlip(pHsChannel);
        } else {
            HsFlipViewPortIn(pHsChannel,
                             workArea.destRect.x, workArea.destRect.y);
        }
    }

    if (workArea.doIncrementNextIndex) {
        HsIncrementNextIndex(pHsDevice, pHsChannel);
    }

    if (workArea.doIncrementNextOffset) {
        HsIncrementNextOffset(pHsDevice, pHsChannel);
    }
}

/*!
 * In response to a non-stall interrupt, check if a headsurface channel has
 * completed a frame of non-swapgroup headsurface rendering and kick off a
 * viewport flip to the offset that was used for that rendering.
 */
void nvHsProcessPendingViewportFlips(NVDevEvoPtr pDevEvo)
{
    NVDispEvoPtr pDispEvo;
    NvU32 dispIndex;

    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        NvU32 apiHead;
        for (apiHead = 0; apiHead < NVKMS_MAX_HEADS_PER_DISP; apiHead++) {
            NVHsChannelEvoPtr pHsChannel = pDispEvo->pHsChannel[apiHead];
            NvU32 lastRenderedOffset;

            if (pHsChannel == NULL) {
                continue;
            }

            lastRenderedOffset = nvHs3dLastRenderedOffset(pHsChannel);

            /*
             * If this channel is marked as having kicked off a frame of
             * rendering, and the semaphore write of the render offset to
             * NVKMS_HEADSURFACE_VIEWPORT_OFFSET_SEMAPHORE_INDEX has completed,
             * then this channel is ready to make a viewport flip to that
             * offset.
             */
            if (pHsChannel->viewportFlipPending &&
                (lastRenderedOffset == HsGetPreviousOffset(pHsChannel))) {

                HsFlipViewPortIn(pHsChannel, 0 /* x */,
                                 lastRenderedOffset *
                                 pHsChannel->config.frameSize.height);
                pHsChannel->viewportFlipPending = FALSE;
            }
        }
    }
}

/*!
 * Record the current scanline, for procfs statistics reporting.
 */
static void HsProcFsRecordScanline(
    const NVDispEvoRec *pDispEvo,
    const NvU32 apiHead)
{
#if NVKMS_PROCFS_ENABLE
    NVHsChannelEvoRec *pHsChannel = pDispEvo->pHsChannel[apiHead];
    NvU16 scanLine = 0;
    NvBool inBlankingPeriod = FALSE;

    if (pHsChannel->statistics.scanLine.pHistogram == NULL) {
        return;
    }

    nvApiHeadGetScanLine(pDispEvo, apiHead, &scanLine, &inBlankingPeriod);

    if (inBlankingPeriod) {
        pHsChannel->statistics.scanLine.nInBlankingPeriod++;
    } else {
        pHsChannel->statistics.scanLine.nNotInBlankingPeriod++;

        if (scanLine <= pHsChannel->statistics.scanLine.vVisible) {
            pHsChannel->statistics.scanLine.pHistogram[scanLine]++;
        } else {
            nvEvoLogDispDebug(pDispEvo, EVO_LOG_ERROR,
                "HsProcFsRecordScanline(): scanLine (%d) > vVisible (%d)",
                scanLine, pHsChannel->statistics.scanLine.vVisible);
        }
    }
#endif /* NVKMS_PROCFS_ENABLE */
}

static void HsProcFsRecordPreviousFrameNotDone(
    NVHsChannelEvoPtr pHsChannel)
{
#if NVKMS_PROCFS_ENABLE
    pHsChannel->statistics.nPreviousFrameNotDone++;
#endif
}

static void HsProcFsRecordFullscreenSgFrames(
    NVHsChannelEvoPtr pHsChannel,
    NvBool isFullscreen)
{
#if NVKMS_PROCFS_ENABLE
    if (isFullscreen) {
        pHsChannel->statistics.nFullscreenSgFrames++;
    } else {
        pHsChannel->statistics.nNonFullscreenSgFrames++;
    }
#endif /* NVKMS_PROCFS_ENABLE */
}

static void HsProcFsRecordOmittedNonSgHsUpdate(
    NVHsChannelEvoPtr pHsChannel)
{
#if NVKMS_PROCFS_ENABLE
    pHsChannel->statistics.nOmittedNonSgHsUpdates++;
#endif
}

/*!
 * Determine if we've flipped to the previous frame.
 *
 * When we program the flip method, we reset the notifier to NOT_BEGUN, and when
 * EVO peforms the flip, it changes the notifier to BEGUN.
 *
 * Find the notifier slot for the previous frame, parse its notifier, and return
 * whether it is BEGUN.
 */
static NvBool IsPreviousFlipDone(NVHsChannelEvoPtr pHsChannel)
{
    const NVDispEvoRec *pDispEvo = pHsChannel->pDispEvo;
    const NvU32 apiHead = pHsChannel->apiHead;
    const NvU32 sd = pDispEvo->displayOwner;
    const NVHsDeviceEvoRec *pHsDevice = pDispEvo->pDevEvo->pHsDevice;
    const NVHsNotifiersRec *pHsNotifiers = &pHsDevice->notifiers;
    const NVHsNotifiersOneSdRec *pHsNotifiersOneSd = pHsNotifiers->sd[sd].ptr;
    const NvU8 nextSlot = pHsNotifiers->sd[sd].apiHead[apiHead].nextSlot;
    struct nvKmsParsedNotifier parsed = { };

    const NvU8 prevSlot =
        A_minus_b_with_wrap_U8(nextSlot, 1,
                               NVKMS_HEAD_SURFACE_MAX_NOTIFIERS_PER_HEAD);

    nvKmsParseNotifier(pHsNotifiers->nIsoFormat, FALSE /* overlay */,
                       prevSlot, pHsNotifiersOneSd->notifier[apiHead], &parsed);

    return parsed.status == NVKMS_NOTIFIER_STATUS_BEGUN;
}

/*!
 * Determine if we've flipped to the previous frame.
 */
static NvBool IsPreviousFrameDone(NVHsChannelEvoPtr pHsChannel)
{
    if (pHsChannel->config.neededForSwapGroup) {
        /*
         * XXX NVKMS HEADSURFACE TODO: Somehow determine if the previous
         * ViewPortIn update for this head was latched.
         */

        /*
         * XXX NVKMS HEADSURFACE TODO: In the absence of a mechanism to
         * determine if ViewPortIn was latched, we would normally rely on this
         * callback arriving once per vblank.  Unfortunately, bug 2086726 can
         * cause us to get called twice per vblank.  WAR this for now by
         * ignoring callbacks that arrive in a very small window of the previous
         * callback.
         *
         * Throttling is now implemented using the RG line 1 interrupt
         * headsurface rendering mechanism, so this limit can be lowered once
         * the old vblank-triggered viewport flipping mechanism is removed.
         */

        const NvU64 oldUSec = pHsChannel->lastCallbackUSec;
        const NvU64 newUSec = nvkms_get_usec();

        /*
         * This threshold is somewhat arbitrary.  In bug 2086726, we see the
         * callback get called from both the ISR and the bottom half, which are
         * usually within ~200 usec of each other on an idle system.  There
         * shouldn't be a danger of mistaking legitimate periodic callbacks with
         * this small threshold: 500 usec per refresh would require a 2000 Hz
         * mode.
         */
        const NvU64 thresholdUSec = 500;

        nvAssert(!pHsChannel->usingRgIntrForSwapGroups);

        if ((newUSec > oldUSec) &&
            (newUSec - oldUSec) < thresholdUSec) {
            return FALSE;
        }

        pHsChannel->lastCallbackUSec = newUSec;

        return TRUE;
    } else {
        return IsPreviousFlipDone(pHsChannel);
    }
}

/*!
 * If the client provided a notifier surface with a real flip
 * request while swap groups were enabled, write to that
 * notifier with the BEGUN status and the most recent
 * headsurface notifier timestamp to emulate what the client
 * would observe if their notifier was used in hardware.
 */
static void HsUpdateClientNotifier(NVHsChannelEvoPtr pHsChannel)
{
    const NVDispEvoRec *pDispEvo = pHsChannel->pDispEvo;
    const NvU32 apiHead = pHsChannel->apiHead;
    const NvU32 sd = pDispEvo->displayOwner;
    const NVHsDeviceEvoRec *pHsDevice = pDispEvo->pDevEvo->pHsDevice;
    const NVHsNotifiersRec *pHsNotifiers = &pHsDevice->notifiers;
    const NVHsNotifiersOneSdRec *pHsNotifiersOneSd = pHsNotifiers->sd[sd].ptr;
    const NvU8 nextSlot = pHsNotifiers->sd[sd].apiHead[apiHead].nextSlot;
    struct nvKmsParsedNotifier parsed = { };
    NVFlipNIsoSurfaceEvoHwState *pClientNotifier =
        &pHsChannel->flipQueue[NVKMS_MAIN_LAYER].current.completionNotifier.surface;

    if (pClientNotifier->pSurfaceEvo == NULL) {
        return;
    }

    const NvU8 prevSlot =
        A_minus_b_with_wrap_U8(nextSlot, 1,
                               NVKMS_HEAD_SURFACE_MAX_NOTIFIERS_PER_HEAD);

    nvKmsParseNotifier(pHsNotifiers->nIsoFormat, FALSE /* overlay */,
                       prevSlot, pHsNotifiersOneSd->notifier[apiHead], &parsed);

    nvAssert(parsed.status == NVKMS_NOTIFIER_STATUS_BEGUN);

    /*
     * XXX NVKMS HEADSURFACE TODO: Get valid timestamp through other means to
     * support this on platforms with legacy HW semaphores without valid
     * HW notifier timestamps in the main channel.
     */
    nvAssert(parsed.timeStampValid);

    nvKmsSetNotifier(pClientNotifier->format,
                     FALSE /* overlay */,
                     pClientNotifier->offsetInWords / 4,
                     pClientNotifier->pSurfaceEvo->cpuAddress[sd],
                     parsed.timeStamp);
}

/*!
 * Check if all flips completed for this SwapGroup.  If so, release the
 * SwapGroup.
 */
static void HsCheckSwapGroupFlipDone(
    NVDevEvoPtr pDevEvo,
    NVSwapGroupRec *pSwapGroup)
{
    const NVHsDeviceEvoRec *pHsDevice = pDevEvo->pHsDevice;
    NVDispEvoPtr pDispEvo;
    NvU32 dispIndex;

    nvAssert(pSwapGroup != NULL);

    if (!pSwapGroup->pendingFlip) {
        return;
    }

    /*
     * Check if all active heads in the SwapGroup have completed their flips.
     * If any haven't, return early.
     */
    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        NvU32 apiHead;
        for (apiHead = 0; apiHead < ARRAY_LEN(pDispEvo->pSwapGroup); apiHead++) {

            if (pDispEvo->pSwapGroup[apiHead] == pSwapGroup) {

                NVHsChannelEvoPtr pHsChannel = pDispEvo->pHsChannel[apiHead];

                if (pHsChannel == NULL) {
                    continue;
                }

                nvAssert(pHsChannel->config.neededForSwapGroup);

                if (!IsPreviousFlipDone(pHsChannel)) {
                    return;
                }
            }
        }
    }

    /*
     * The SwapGroup is ready: update client notifiers if necessary and
     * increment nextIndex for all active heads, so that subsequent frames of
     * headSurface render to the next buffer.
     */
    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        NvU32 apiHead;
        for (apiHead = 0; apiHead < ARRAY_LEN(pDispEvo->pSwapGroup); apiHead++) {

            if (pDispEvo->pSwapGroup[apiHead] == pSwapGroup) {

                NVHsChannelEvoPtr pHsChannel = pDispEvo->pHsChannel[apiHead];

                if (pHsChannel == NULL) {
                    continue;
                }

                nvAssert(pHsChannel->config.neededForSwapGroup);
                nvAssert(IsPreviousFlipDone(pHsChannel));

                HsUpdateClientNotifier(pHsChannel);
                HsIncrementNextIndex(pHsDevice, pHsChannel);
            }
        }
    }

    /*
     * The SwapGroup is ready: release all SwapGroup members so that they can
     * proceed.
     */
    nvHsSwapGroupRelease(pDevEvo, pSwapGroup);
}

/*
 * Called from RG line interrupt handler to determine whether rendering a
 * new frame could be skipped.
 */
static NvBool HsCanOmitNonSgHsUpdate(NVHsChannelEvoPtr pHsChannel)
{
    const NVSwapGroupRec *pHeadSwapGroup =
        pHsChannel->pDispEvo->pSwapGroup[pHsChannel->apiHead];

    /*
     * When fullscreen swapgroup flipping, updating
     * non-swapgroup content at vblank is unnecessary and
     * dangerous, since it results in releasing client
     * semaphores before their contents have actually been
     * displayed.
     */
    if (pHsChannel->swapGroupFlipping) {
        return NV_TRUE;
    }

    /*
     * In the case of a fullscreen swapgroup, we can generally omit updating
     * the headsurface entirely upon vblank as long as the client is
     * actively rendering. All the swapgroup content has already been
     * updated to the headsurface backbuffer at the client's swapbuffers
     * time and there's no need to update the backbuffer again on RG line 1
     * or vblank interrupt time.
     *
     * There is one exception to this. If the client isn't rendering
     * actively then updates to the cursor (and possibly overlays, head
     * config) still require rendering an updated frame to the backbuffer.
     * Thus, we will simply limit this optimization for frames that come
     * within one frame time after the last recorded flip.
     */
    if (pHeadSwapGroup &&
        pHeadSwapGroup->swapGroupIsFullscreen) {

        NvU64 nowUs = nvkms_get_usec();
        NvU64 frameTimeUs = nvEvoFrametimeUsFromTimings(
            &pHsChannel->pDispEvo->apiHeadState[pHsChannel->apiHead].timings);

        if (nowUs - pHsChannel->lastHsClientFlipTimeUs < frameTimeUs) {
            return NV_TRUE;
        }
    }

    return NV_FALSE;
}

/*!
 * Receive RG line 1 callback, in process context with nvkms_lock held.
 */
static void HsRgLine1CallbackProc(NVDispEvoRec *pDispEvo,
                                  const NvU32 head,
                                  NVRgLine1CallbackPtr pCallback)
{
    const NvU32 apiHead =
        (NvU32)(NvUPtr)pCallback->pUserData;
    NVHsChannelEvoPtr pHsChannel = pDispEvo->pHsChannel[apiHead];

    /*
     * The pHsChannel may have been torn down between when the callback was
     * generated and when this was called.  Ignore spurious callbacks.
     */
    if (pHsChannel == NULL) {
        return;
    }

    if (pHsChannel->config.neededForSwapGroup) {
        /*
         * Update the non-swapgroup content on the back half of both
         * headsurface surfaces, and the swapgroup content on the back half of
         * the back headsurface surface, and perform a viewportoffset flip to
         * the back offset.
         *
         * Synchronization is achieved by the following mechanism:
         *
         * - Before rendering a new frame, check that we aren't still scanning
         *   out from that half of the surface.
         * - After rendering a frame, push a semaphore write with the render
         *   offset and a non-stall interrupt.
         * - In response to the non-stall interrupt, perform the viewport
         *   flip to the render offset.
         */
        NvU32 activeViewportOffset =
            nvApiHeadGetActiveViewportOffset(pDispEvo, apiHead);

        nvAssert((activeViewportOffset == 0) ||
                 (activeViewportOffset == pHsChannel->config.frameSize.height));

        activeViewportOffset /= pHsChannel->config.frameSize.height;

        if (activeViewportOffset == HsGetPreviousOffset(pHsChannel)) {
            /*
             * The active viewport is the same as the last one we pushed, so
             * it's safe to start rendering to pHsChannel->nextOffset; check if
             * rendering from a previous interrupt hasn't completed yet.
             */
            if (pHsChannel->viewportFlipPending) {
                /*
                 * A non-stall interrupt hasn't been triggered since we kicked
                 * off the previous frame's rendering.
                 */
                HsProcFsRecordPreviousFrameNotDone(pHsChannel);
            } else {
                NVHsDeviceEvoRec *pHsDevice = pDispEvo->pDevEvo->pHsDevice;

                HsProcFsRecordScanline(pDispEvo, apiHead);

                if (HsCanOmitNonSgHsUpdate(pHsChannel)) {
                    HsProcFsRecordOmittedNonSgHsUpdate(pHsChannel);
                } else {
                    nvHsNextFrame(pHsDevice, pHsChannel, NV_HS_NEXT_FRAME_REQUEST_TYPE_VBLANK);
                }
            }
        } else {
            /*
             * The viewport flip we pushed after the previous frame's rendering
             * hasn't been applied in hardware yet.
             */
            HsProcFsRecordPreviousFrameNotDone(pHsChannel);
        }

        HsCheckSwapGroupFlipDone(pDispEvo->pDevEvo, pDispEvo->pSwapGroup[apiHead]);
    }
}

/*!
 * Receive vblank callback, in process context with nvkms_lock held.
 *
 */
static void HsVBlankCallback(NVDispEvoRec *pDispEvo,
                             NVVBlankCallbackPtr pCallbackData)
{
    const NvU32 apiHead = pCallbackData->apiHead;
    NVHsChannelEvoPtr pHsChannel = pDispEvo->pHsChannel[apiHead];
    NVHsDeviceEvoRec *pHsDevice = pDispEvo->pDevEvo->pHsDevice;

    /*
     * The pHsChannel may have been torn down between when the vblank was
     * generated and when this was called.  Ignore spurious callbacks.
     */
    if (pHsChannel == NULL) {
        return;
    }

    if (!pHsChannel->usingRgIntrForSwapGroups &&
        pHsChannel->config.neededForSwapGroup) {
        HsCheckSwapGroupFlipDone(pDispEvo->pDevEvo, pDispEvo->pSwapGroup[apiHead]);
    }

    if (pHsChannel->usingRgIntrForSwapGroups &&
        pHsChannel->config.neededForSwapGroup) {
        // The next frame will be rendered during the RG line 1 interrupt.
        return;
    }

    /*
     * If we have not flipped to the previous buffer, yet, we should not render
     * to the next buffer.  Wait until the next vblank callback.
     */
    if (!IsPreviousFrameDone(pHsChannel)) {
        HsProcFsRecordPreviousFrameNotDone(pHsChannel);
        return;
    }

    HsProcFsRecordScanline(pDispEvo, apiHead);

    /*
     * XXX NVKMS HEADSURFACE TODO: evaluate whether there has been
     * damage to the source buffer since the last headSurface frame.
     * Only if so, perform the headSurface transformation and flip to
     * the resulting headSurface buffer.
     *
     * For headSurface bringup purposes, just always flip to the next
     * headSurface buffer.
     */

    /*
     * When fullscreen swapgroup flipping, updating
     * non-swapgroup content at vblank is unnecessary and
     * dangerous, since it results in releasing client
     * semaphores before their contents have actually been
     * displayed.
     */
    if (!pHsChannel->swapGroupFlipping) {
        nvHsNextFrame(pHsDevice, pHsChannel,
                      NV_HS_NEXT_FRAME_REQUEST_TYPE_VBLANK);
    }
}

/*!
 * Schedule vblank callbacks from resman on a specific head and subdevice.
 */
void nvHsAddVBlankCallback(NVHsChannelEvoPtr pHsChannel)
{
    NVDispEvoRec *pDispEvo = pHsChannel->pDispEvo;

    pHsChannel->vBlankCallback =
        nvApiHeadRegisterVBlankCallback(pDispEvo,
                                        pHsChannel->apiHead,
                                        HsVBlankCallback,
                                        NULL);
}

/*!
 * Add an RG line 1 callback to check the swapgroup flip notifier and release
 * its associated deferred request fifo.
 *
 * This is done in an RG line 1 callback instead of the vblank callback to WAR
 * an issue where certain mode timings cause the vblank callback to fire
 * slightly before LOADV causes the notifier to transition from NOT_BEGUN
 * to BEGUN, causing an extra frame of delay before the next vblank occurs and
 * the deferred request fifo can be released.
 */
void nvHsAddRgLine1Callback(NVHsChannelEvoPtr pHsChannel)
{
    NVDispEvoRec *pDispEvo = pHsChannel->pDispEvo;
    NvBool found;
    NvU32 val;

    /*
     * Use the RG line 1 interrupt to check swapgroup completion by default,
     * but allow setting NVKMS_DELAY_SWAPGROUP_CHECK=0 by regkey to revert to
     * the old method of checking during vblank for debugging purposes.
     */
    found = nvGetRegkeyValue(pDispEvo->pDevEvo, "NVKMS_DELAY_SWAPGROUP_CHECK",
                             &val);

    if (found && (val == 0)) {
        return;
    }

    pHsChannel->pRgIntrCallback =
        nvApiHeadAddRgLine1Callback(pDispEvo,
                                    pHsChannel->apiHead,
                                    HsRgLine1CallbackProc,
                                    (void*)(NvUPtr)pHsChannel->apiHead);

    if (pHsChannel->pRgIntrCallback == NULL) {
        nvAssert(!"Failed to register headSurface RG line 1 interrupt");
    } else {
        pHsChannel->usingRgIntrForSwapGroups = TRUE;
    }
}

/*!
 * Cancel RG line 1 callbacks from resman on the specified head and subdevice.
 *
 * The same limitations regarding leftover vblank callbacks after vblank
 * callbacks are disabled in nvHsRemoveVblankCallback apply to RG callbacks.
 */
void nvHsRemoveRgLine1Callback(NVHsChannelEvoPtr pHsChannel)
{
    const NVDispEvoRec *pDispEvo = pHsChannel->pDispEvo;

    if (pHsChannel->usingRgIntrForSwapGroups) {
        nvRmRemoveRgLine1Callback(pDispEvo,
                                  pHsChannel->pRgIntrCallback);
        pHsChannel->pRgIntrCallback = NULL;
    }
}

/*!
 * Cancel vblank callbacks from resman on the specified head and subdevice.
 *
 * Note that there could currently be callbacks in flight.  We should be
 * prepared to handle a spurious callback after cancelling the callbacks here.
 *
 * XXX NVKMS HEADSURFACE TODO: It would be better to:
 *
 * (a) Remove the vblank callback before the modeset that disables headSurface.
 * (b) Drain/cancel any in flight callbacks while holding the nvkms_lock.
 *
 * A mechanism like that should avoid spurious callbacks.
 */
void nvHsRemoveVBlankCallback(NVHsChannelEvoPtr pHsChannel)
{
    NVDispEvoRec *pDispEvo = pHsChannel->pDispEvo;

    nvApiHeadUnregisterVBlankCallback(pDispEvo,
                                      pHsChannel->vBlankCallback);
    pHsChannel->vBlankCallback = NULL;
}

void nvHsAllocStatistics(
    NVHsChannelEvoRec *pHsChannel)
{
#if NVKMS_PROCFS_ENABLE
    const NVDispEvoRec *pDispEvo = pHsChannel->pDispEvo;
    const NvU32 apiHead = pHsChannel->apiHead;
    const NVHwModeTimingsEvo *pTimings =
        &pDispEvo->apiHeadState[apiHead].timings;
    NvU32 n;

    nvkms_memset(&pHsChannel->statistics, 0, sizeof(pHsChannel->statistics));

    pHsChannel->statistics.scanLine.vVisible = nvEvoVisibleHeight(pTimings);

    n = pHsChannel->statistics.scanLine.vVisible + 1;

    pHsChannel->statistics.scanLine.pHistogram = nvCalloc(1, sizeof(NvU64) * n);
#endif /* NVKMS_PROCFS_ENABLE */
}

void nvHsFreeStatistics(
    NVHsChannelEvoRec *pHsChannel)
{
#if NVKMS_PROCFS_ENABLE
    nvFree(pHsChannel->statistics.scanLine.pHistogram);
    nvkms_memset(&pHsChannel->statistics, 0, sizeof(pHsChannel->statistics));
#endif /* NVKMS_PROCFS_ENABLE */
}

#if NVKMS_PROCFS_ENABLE

static const struct {
    const char *before;
    const char *after;
} HsProcFsIndentTable[] = {
    [0] = { .before = "", .after = "    " },
    [1] = { .before = " ", .after = "   " },
    [2] = { .before = "  ", .after = "  " },
    [3] = { .before = "   ", .after = " " },
    [5] = { .before = "    ", .after = "" },
};

static const char *HsProcFsIndentBefore(NvU8 indent)
{
    nvAssert(indent < ARRAY_LEN(HsProcFsIndentTable));

    return HsProcFsIndentTable[indent].before;
}

static const char *HsProcFsIndentAfter(NvU8 indent)
{
    nvAssert(indent < ARRAY_LEN(HsProcFsIndentTable));

    return HsProcFsIndentTable[indent].after;
}

static void HsProcFsGpuTime(
    NVEvoInfoStringRec *pInfoString,
    const NvU64 nFrames,
    const NvU64 gpuTimeSpent,
    const NvU8 indent)
{
    /*
     * Use nFrames - 1 to compute averageGpuTimeNs: the nvHs3dRenderFrame() path
     * increments nFrames at the end of rendering a frame, but it only updates
     * gpuTimeSpent at the start of rendering the _next_ frame.  I.e.,
     * gpuTimeSpent has time for nFrames - 1 frames.
     */
    const NvU64 averageGpuTimeNs =
        (nFrames <= 1) ? 0 : (gpuTimeSpent / (nFrames - 1));
    const NvU64 averageGpuTimeUs = (averageGpuTimeNs + 500) / 1000;
    const NvU64 nFramesToReport = (nFrames <= 1) ? 0 : nFrames - 1;

    nvEvoLogInfoString(
        pInfoString, "   %savg GPU time / frame%s   : "
        "%" NvU64_fmtu ".%03" NvU64_fmtu " msec "
        "(%" NvU64_fmtu " nsec / %" NvU64_fmtu " frames)",
        HsProcFsIndentBefore(indent),
        HsProcFsIndentAfter(indent),
        averageGpuTimeUs / 1000,
        averageGpuTimeUs % 1000,
        gpuTimeSpent,
        nFramesToReport);
}

static void HsProcFsFrameStatisticsOneEye(
    NVEvoInfoStringRec *pInfoString,
    const NVHsChannelEvoRec *pHsChannel,
    const NvU8 eye,
    const NvU8 slot,
    const NvU8 indent)
{
    const NVHsChannelStatisticsOneEyeRec *pPerEye =
        &pHsChannel->statistics.perEye[eye][slot];

    const NvU64 framesPerMs = pPerEye->fps.framesPerMs;

    nvEvoLogInfoString(
        pInfoString,
        "   %snFrames%s                : %" NvU64_fmtu,
        HsProcFsIndentBefore(indent),
        HsProcFsIndentAfter(indent),
        pPerEye->nFrames);

    nvEvoLogInfoString(
        pInfoString, "   %sFPS (computed every 5s)%s: "
        "%" NvU64_fmtu ".%03" NvU64_fmtu,
        HsProcFsIndentBefore(indent),
        HsProcFsIndentAfter(indent),
        framesPerMs / 1000,
        framesPerMs % 1000);

    HsProcFsGpuTime(
        pInfoString,
        pPerEye->nFrames,
        pPerEye->gpuTimeSpent,
        indent);
}

static void HsProcFsFrameStatisticsOneSlot(
    NVEvoInfoStringRec *pInfoString,
    const NVHsChannelEvoRec *pHsChannel,
    const NvU8 slot,
    const NvU8 indent)
{
    const char *eyeLabel[] = {
        [NVKMS_LEFT]  = "Left Eye ",
        [NVKMS_RIGHT] = "Right Eye",
    };

    const NvBool needEyeLabel =
        pHsChannel->statistics.perEye[NVKMS_RIGHT][slot].nFrames != 0;
    NvU8 eye;

    for (eye = NVKMS_LEFT; eye < NVKMS_MAX_EYES; eye++) {

        NvU8 localIndent = 0;

        if (pHsChannel->statistics.perEye[eye][slot].nFrames == 0) {
            continue;
        }

        if (needEyeLabel) {
            nvEvoLogInfoString(
                pInfoString, "   %s%s%s              :",
                HsProcFsIndentBefore(indent),
                eyeLabel[eye],
                HsProcFsIndentAfter(indent));
            localIndent++;
        }

        HsProcFsFrameStatisticsOneEye(
            pInfoString,
            pHsChannel,
            eye,
            slot,
            indent + localIndent);
    }
}

static void HsProcFsFrameStatistics(
    NVEvoInfoStringRec *pInfoString,
    const NVHsChannelEvoRec *pHsChannel)
{
    NvU8 slot;

    if (pHsChannel->config.neededForSwapGroup) {
        nvEvoLogInfoString(pInfoString,
                           "   VBLANK frames              :");

        nvEvoLogInfoString(pInfoString,
                           "    Old swapGroup content     :");

        slot = Hs3dStatisticsGetSlot(
                    pHsChannel,
                    NV_HS_NEXT_FRAME_REQUEST_TYPE_VBLANK, 0,
                    TRUE /* honorSwapGroupClipList */);

        HsProcFsFrameStatisticsOneSlot(pInfoString, pHsChannel, slot, 2);

        nvEvoLogInfoString(pInfoString,
                           "    New swapGroup content     :");

        slot = Hs3dStatisticsGetSlot(
                    pHsChannel,
                    NV_HS_NEXT_FRAME_REQUEST_TYPE_VBLANK, 0,
                    FALSE /* honorSwapGroupClipList */);

        HsProcFsFrameStatisticsOneSlot(pInfoString, pHsChannel, slot, 2);

        nvEvoLogInfoString(pInfoString,
                           "   SWAP_GROUP_READY frames    :");

        slot = Hs3dStatisticsGetSlot(
                    pHsChannel,
                    NV_HS_NEXT_FRAME_REQUEST_TYPE_SWAP_GROUP_READY, 0,
                    FALSE /* honorSwapGroupClipList */);

        HsProcFsFrameStatisticsOneSlot(pInfoString, pHsChannel, slot, 1);

    } else {
        const NvU8 indent = 0; /* start with no indentation */

        slot = Hs3dStatisticsGetSlot(
                    pHsChannel,
                    NV_HS_NEXT_FRAME_REQUEST_TYPE_VBLANK, 0,
                    FALSE);

        HsProcFsFrameStatisticsOneSlot(pInfoString, pHsChannel, slot, indent);
    }
}

static void HsProcFsScanLine(
    NVEvoInfoStringRec *pInfoString,
    const NVHsChannelEvoRec *pHsChannel)
{
    NvU16 i;

    nvEvoLogInfoString(pInfoString,
                       "   scanLine information       :");

    nvEvoLogInfoString(pInfoString,
                       "    nInBlankingPeriod         : %" NvU64_fmtu,
                       pHsChannel->statistics.scanLine.nInBlankingPeriod);
    nvEvoLogInfoString(pInfoString,
                       "    nNotInBlankingPeriod      : %" NvU64_fmtu,
                       pHsChannel->statistics.scanLine.nNotInBlankingPeriod);
    nvEvoLogInfoString(pInfoString,
                       "    vVisible                  : %d",
                       pHsChannel->statistics.scanLine.vVisible);

    if (pHsChannel->statistics.scanLine.pHistogram == NULL) {

        nvEvoLogInfoString(pInfoString,
                           "    scanline histogram        : failed allocation");
    } else {

        nvEvoLogInfoString(pInfoString,
                           "    scanline histogram        :");

        for (i = 0; i <= pHsChannel->statistics.scanLine.vVisible; i++) {

            if (pHsChannel->statistics.scanLine.pHistogram[i] != 0) {
                nvEvoLogInfoString(pInfoString,
                    "     scanLine[%04d]           : %" NvU64_fmtu,
                    i, pHsChannel->statistics.scanLine.pHistogram[i]);
            }
        }
    }
}

static void HsProcFsFlipQueueOneEntry(
    NVEvoInfoStringRec *pInfoString,
    const NVHsLayerRequestedFlipState *pFlipState)
{
    /*
     * Print the pointers by casting to NvUPtr and formatting with NvUPtr_fmtx,
     * so that NULL is printed as "0x0", rather than "(null)".
     */

    nvEvoLogInfoString(pInfoString,
        "        pSurfaceEvo(L,R)      : 0x%" NvUPtr_fmtx ", 0x%" NvUPtr_fmtx,
        (NvUPtr)pFlipState->pSurfaceEvo[NVKMS_LEFT],
        (NvUPtr)pFlipState->pSurfaceEvo[NVKMS_RIGHT]);

    if (!pFlipState->syncObject.usingSyncpt) {
        nvEvoLogInfoString(pInfoString,
            "        semaphore             : "
            "acquire pSurfaceEvo: 0x%" NvUPtr_fmtx ", "
            "release pSurfaceEvo: 0x%" NvUPtr_fmtx ", "
            "acquire value: 0x%08x, "
            "release value: 0x%08x",
            (NvUPtr)pFlipState->syncObject.u.semaphores.acquireSurface.pSurfaceEvo,
            (NvUPtr)pFlipState->syncObject.u.semaphores.releaseSurface.pSurfaceEvo,
            pFlipState->syncObject.u.semaphores.acquireValue,
            pFlipState->syncObject.u.semaphores.releaseValue);
    }
}

static void HsProcFsFlipQueue(
    NVEvoInfoStringRec *pInfoString,
    const NVHsChannelEvoRec *pHsChannel)
{
    const NVHsChannelFlipQueueEntry *pEntry;
    NvU8 layer;

    for (layer = 0; layer < ARRAY_LEN(pHsChannel->flipQueue); layer++) {

        const char *layerString[NVKMS_MAX_LAYERS_PER_HEAD] = {
            [NVKMS_MAIN_LAYER]    = "(main)   ",
            [NVKMS_OVERLAY_LAYER] = "(overlay)",
        };

        nvEvoLogInfoString(pInfoString,
            "   flipQueue%s         :", layerString[layer]);

        nvEvoLogInfoString(pInfoString,
            "     current                  :");

        HsProcFsFlipQueueOneEntry(pInfoString,
                                  &pHsChannel->flipQueue[layer].current);

        nvListForEachEntry(pEntry,
                           &pHsChannel->flipQueue[layer].queue,
                           flipQueueEntry) {

            nvEvoLogInfoString(pInfoString,
                "     pending                  :");

            HsProcFsFlipQueueOneEntry(pInfoString, &pEntry->hwState);
        }
    }
}

static const char *HsGetEyeMaskString(const NvU8 eyeMask)
{
    if (eyeMask == NVBIT(NVKMS_LEFT)) {
        return "L";
    } else {
        nvAssert(eyeMask == (NVBIT(NVKMS_LEFT) | NVBIT(NVKMS_RIGHT)));
        return "L|R";
    }
}

static const char *HsGetPixelShiftString(
    const enum NvKmsPixelShiftMode pixelShift)
{
    switch (pixelShift) {
    case NVKMS_PIXEL_SHIFT_NONE:            return "none";
    case NVKMS_PIXEL_SHIFT_4K_TOP_LEFT:     return "4kTopLeft";
    case NVKMS_PIXEL_SHIFT_4K_BOTTOM_RIGHT: return "4kBottomRight";
    case NVKMS_PIXEL_SHIFT_8K:              return "8k";
    }

    return "unknown";
}

static void HsProcFsTransform(
    NVEvoInfoStringRec *pInfoString,
    const NVHsChannelEvoRec *pHsChannel)
{
    nvEvoLogInfoString(pInfoString,
                       "   transform matrix           : "
                       "{ { 0x%08x, 0x%08x, 0x%08x },",
                       F32viewAsNvU32(pHsChannel->config.transform.m[0][0]),
                       F32viewAsNvU32(pHsChannel->config.transform.m[0][1]),
                       F32viewAsNvU32(pHsChannel->config.transform.m[0][2]));

    nvEvoLogInfoString(pInfoString,
                       "                              : "
                       "  { 0x%08x, 0x%08x, 0x%08x },",
                       F32viewAsNvU32(pHsChannel->config.transform.m[1][0]),
                       F32viewAsNvU32(pHsChannel->config.transform.m[1][1]),
                       F32viewAsNvU32(pHsChannel->config.transform.m[1][2]));

    nvEvoLogInfoString(pInfoString,
                       "                              : "
                       "  { 0x%08x, 0x%08x, 0x%08x } }",
                       F32viewAsNvU32(pHsChannel->config.transform.m[2][0]),
                       F32viewAsNvU32(pHsChannel->config.transform.m[2][1]),
                       F32viewAsNvU32(pHsChannel->config.transform.m[2][2]));
}

static void HsProcFsStaticWarpMesh(
    NVEvoInfoStringRec *pInfoString,
    const NVHsChannelEvoRec *pHsChannel)
{
    nvEvoLogInfoString(pInfoString,
                       "   staticWarpMesh             : "
                       "{ { 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x },",
                       pHsChannel->config.staticWarpMesh.vertex[0].x,
                       pHsChannel->config.staticWarpMesh.vertex[0].y,
                       pHsChannel->config.staticWarpMesh.vertex[0].u,
                       pHsChannel->config.staticWarpMesh.vertex[0].v,
                       pHsChannel->config.staticWarpMesh.vertex[0].r,
                       pHsChannel->config.staticWarpMesh.vertex[0].q);

    nvEvoLogInfoString(pInfoString,
                       "                              : "
                       "  { 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x },",
                       pHsChannel->config.staticWarpMesh.vertex[1].x,
                       pHsChannel->config.staticWarpMesh.vertex[1].y,
                       pHsChannel->config.staticWarpMesh.vertex[1].u,
                       pHsChannel->config.staticWarpMesh.vertex[1].v,
                       pHsChannel->config.staticWarpMesh.vertex[1].r,
                       pHsChannel->config.staticWarpMesh.vertex[1].q);

    nvEvoLogInfoString(pInfoString,
                       "                              : "
                       "  { 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x },",
                       pHsChannel->config.staticWarpMesh.vertex[2].x,
                       pHsChannel->config.staticWarpMesh.vertex[2].y,
                       pHsChannel->config.staticWarpMesh.vertex[2].u,
                       pHsChannel->config.staticWarpMesh.vertex[2].v,
                       pHsChannel->config.staticWarpMesh.vertex[2].r,
                       pHsChannel->config.staticWarpMesh.vertex[2].q);

    nvEvoLogInfoString(pInfoString,
                       "                              : "
                       "  { 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x } }",
                       pHsChannel->config.staticWarpMesh.vertex[3].x,
                       pHsChannel->config.staticWarpMesh.vertex[3].y,
                       pHsChannel->config.staticWarpMesh.vertex[3].u,
                       pHsChannel->config.staticWarpMesh.vertex[3].v,
                       pHsChannel->config.staticWarpMesh.vertex[3].r,
                       pHsChannel->config.staticWarpMesh.vertex[3].q);
}

static const char *HsProcFsGetNeededForString(
    const NVHsChannelEvoRec *pHsChannel)
{
    if (pHsChannel->config.neededForModeset &&
        pHsChannel->config.neededForSwapGroup) {
        return "modeset, swapgroup";
    }

    if (pHsChannel->config.neededForModeset &&
        !pHsChannel->config.neededForSwapGroup) {
        return "modeset";
    }

    if (!pHsChannel->config.neededForModeset &&
        pHsChannel->config.neededForSwapGroup) {
        return "swapgroup";
    }

    return "unknown";
}

static void HsProcFsFrameSemaphores(
    NVEvoInfoStringRec *pInfoString,
    const NVHsChannelEvoRec *pHsChannel)
{
    const NVDispEvoRec *pDispEvo = pHsChannel->pDispEvo;
    const NVHsDeviceEvoRec *pHsDevice = pDispEvo->pDevEvo->pHsDevice;
    const NvU32 sd = pDispEvo->displayOwner;
    const NVHsNotifiersOneSdRec *p = pHsDevice->notifiers.sd[sd].ptr;
    const NvGpuSemaphore *pSema =
        (const NvGpuSemaphore *)p->semaphore[pHsChannel->apiHead];

    NvU8 buffer;

    for (buffer = 0; buffer < NVKMS_HEAD_SURFACE_MAX_BUFFERS; buffer++) {
        nvEvoLogInfoString(pInfoString,
                           "   frameSemaphore[%d]          : 0x%0x",
                           buffer,
                           pSema[buffer].data[0]);
    }
}

void nvHsProcFs(
    NVEvoInfoStringRec *pInfoString,
    NVDevEvoRec *pDevEvo,
    NvU32 dispIndex,
    NvU32 apiHead)
{
    NVDispEvoPtr pDispEvo = pDevEvo->pDispEvo[dispIndex];
    const NVHsChannelEvoRec *pHsChannel = pDispEvo->pHsChannel[apiHead];
    const NVHsStateOneHeadAllDisps *pHsOneHeadAllDisps =
        &pDevEvo->apiHeadSurfaceAllDisps[apiHead];

    if (pHsChannel == NULL) {
        nvEvoLogInfoString(pInfoString,
                           "  headSurface[head:%02d]        : disabled", apiHead);
        return;
    }

    nvEvoLogInfoString(pInfoString,
                       "  headSurface[head:%02d]        : "
                       "enabled (needed for: %s)",
                       apiHead, HsProcFsGetNeededForString(pHsChannel));

    HsProcFsFrameStatistics(pInfoString, pHsChannel);

    nvEvoLogInfoString(pInfoString,
                       "   nextIndex                  : %d",
                       pHsChannel->nextIndex);

    nvEvoLogInfoString(pInfoString,
                       "   nextOffset                 : %d",
                       pHsChannel->nextOffset);

    nvEvoLogInfoString(pInfoString,
                       "   nPreviousFrameNotDone      : %" NvU64_fmtu,
                       pHsChannel->statistics.nPreviousFrameNotDone);

    nvEvoLogInfoString(pInfoString,
                       "   nOmittedNonSgHsUpdates     : %" NvU64_fmtu,
                       pHsChannel->statistics.nOmittedNonSgHsUpdates);

    nvEvoLogInfoString(pInfoString,
                       "   nFullscreenSgFrames        : %" NvU64_fmtu,
                       pHsChannel->statistics.nFullscreenSgFrames);

    nvEvoLogInfoString(pInfoString,
                       "   nNonFullscreenSgFrames     : %" NvU64_fmtu,
                       pHsChannel->statistics.nNonFullscreenSgFrames);

    nvEvoLogInfoString(pInfoString,
                       "   viewPortIn                 : %d x %d +%d +%d",
                       pHsChannel->config.viewPortIn.width,
                       pHsChannel->config.viewPortIn.height,
                       pHsChannel->config.viewPortIn.x,
                       pHsChannel->config.viewPortIn.y);

    nvEvoLogInfoString(pInfoString,
                       "   viewPortOut                : %d x %d +%d +%d",
                       pHsChannel->config.viewPortOut.width,
                       pHsChannel->config.viewPortOut.height,
                       pHsChannel->config.viewPortOut.x,
                       pHsChannel->config.viewPortOut.y);

    nvEvoLogInfoString(pInfoString,
                       "   frameSize                  : %d x %d",
                       pHsChannel->config.frameSize.width,
                       pHsChannel->config.frameSize.height);

    nvEvoLogInfoString(pInfoString,
                       "   surfaceSize                : %d x %d",
                       pHsChannel->config.surfaceSize.width,
                       pHsChannel->config.surfaceSize.height);

    nvEvoLogInfoString(pInfoString,
                       "   stagingSurfaceSize         : %d x %d",
                       pHsChannel->config.stagingSurfaceSize.width,
                       pHsChannel->config.stagingSurfaceSize.height);

    nvEvoLogInfoString(pInfoString,
                       "   allDispsSurfaceSize        : %d x %d",
                       pHsOneHeadAllDisps->size.width,
                       pHsOneHeadAllDisps->size.height);

    nvEvoLogInfoString(pInfoString,
                       "   allDispsStagingSize        : %d x %d",
                       pHsOneHeadAllDisps->stagingSize.width,
                       pHsOneHeadAllDisps->stagingSize.height);

    nvEvoLogInfoString(pInfoString,
                       "   allDispsSurfaceCount       : %d",
                       pHsOneHeadAllDisps->surfaceCount);

    nvEvoLogInfoString(pInfoString,
                       "   eyeMask                    : %s",
                       HsGetEyeMaskString(pHsChannel->config.eyeMask));

    nvEvoLogInfoString(pInfoString,
                       "   pixelShift                 : %s",
                       HsGetPixelShiftString(pHsChannel->config.pixelShift));

    HsProcFsTransform(pInfoString, pHsChannel);

    HsProcFsStaticWarpMesh(pInfoString, pHsChannel);

    HsProcFsFlipQueue(pInfoString, pHsChannel);

    HsProcFsFrameSemaphores(pInfoString, pHsChannel);

    HsProcFsScanLine(pInfoString, pHsChannel);
}
#endif /* NVKMS_PROCFS_ENABLE */
