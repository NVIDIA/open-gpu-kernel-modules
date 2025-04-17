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

#include "nvkms-lut.h"
#include "nvkms-rm.h"
#include "nvkms-rmapi.h"
#include "nvkms-dma.h"
#include "nvkms-surface.h"
#include "nvkms-private.h"
#include "nvkms-utils.h"
#include "nvkms-headsurface.h"
#include "nvos.h"

#include <class/cl0040.h> /* NV01_MEMORY_LOCAL_USER */

static void FreeLutSurfaceEvo(NVDevEvoPtr pDevEvo, NVSurfaceEvoPtr pSurfEvo)
{
    if (pSurfEvo == NULL) {
        return;
    }

    nvAssert(pSurfEvo->rmRefCnt == 1);
    nvAssert(pSurfEvo->structRefCnt == 1);

    nvEvoUnregisterSurface(pDevEvo,
                           pDevEvo->pNvKmsOpenDev,
                           pSurfEvo->owner.surfaceHandle,
                           TRUE /* skipUpdate */,
                           FALSE /* skipSync */);
}

static NVSurfaceEvoPtr RegisterLutSurfaceEvo(NVDevEvoPtr pDevEvo, NvU32 memoryHandle)
{
    struct NvKmsRegisterSurfaceParams registerSurfaceParams = { };
    const NVEvoApiHandlesRec *pSurfaceHandles;
    NvU64 size = (sizeof(NVEvoLutDataRec) + 63) & ~63;
    NVSurfaceEvoPtr pSurfEvo = NULL;

    registerSurfaceParams.request.deviceHandle = pDevEvo->deviceHandle;
    registerSurfaceParams.request.useFd = FALSE;
    registerSurfaceParams.request.rmClient = nvEvoGlobal.clientHandle;

    registerSurfaceParams.request.planes[0].u.rmObject = memoryHandle;
    registerSurfaceParams.request.planes[0].offset = 0;
    registerSurfaceParams.request.planes[0].pitch = (size + 255) & ~255;
    registerSurfaceParams.request.planes[0].rmObjectSizeInBytes = size;

    registerSurfaceParams.request.widthInPixels = (size + 7) >> 3; /* TODO: Check on this */
    registerSurfaceParams.request.heightInPixels = 1;

    registerSurfaceParams.request.layout = NvKmsSurfaceMemoryLayoutPitch;
    registerSurfaceParams.request.format = NvKmsSurfaceMemoryFormatR16G16B16A16;

    registerSurfaceParams.request.noDisplayHardwareAccess = FALSE;
    registerSurfaceParams.request.noDisplayCaching = FALSE;

    registerSurfaceParams.request.isoType = NVKMS_MEMORY_ISO;
    registerSurfaceParams.request.log2GobsPerBlockY = 0;

    /*
     * Although the caller may like a GPU mapping, we pass
     * NvHsMapPermissionsNone so failing the mapping doesn't fail the surface
     * creation.
     */
    nvEvoRegisterSurface(pDevEvo,
                         pDevEvo->pNvKmsOpenDev,
                         &registerSurfaceParams,
                         NvHsMapPermissionsNone);

    if (registerSurfaceParams.reply.surfaceHandle == 0) {
        return NULL;
    }

    pSurfaceHandles = nvGetSurfaceHandlesFromOpenDevConst(pDevEvo->pNvKmsOpenDev);
    pSurfEvo =
        nvEvoGetSurfaceFromHandle(pDevEvo,
                                  pSurfaceHandles,
                                  registerSurfaceParams.reply.surfaceHandle,
                                  FALSE /* isUsedByCursorChannel */,
                                  TRUE /* isUsedByLayerChannel */);
    return pSurfEvo;
}

static NVSurfaceEvoPtr AllocLutSurfaceEvoInVidmem(NVDevEvoPtr pDevEvo)
{
    NV_MEMORY_ALLOCATION_PARAMS memAllocParams = { };
    NvU32 ret = NVOS_STATUS_ERROR_GENERIC;
    NvU32 attr = 0, attr2 = 0;
    NvU32 allocFlags = NVOS32_ALLOC_FLAGS_FORCE_MEM_GROWS_DOWN |
                       NVOS32_ALLOC_FLAGS_ALIGNMENT_FORCE;
    NvU64 size = 0, alignment = 4096;
    NvU32 memoryHandle = 0;
    NVSurfaceEvoPtr pSurfEvo = NULL;

    memoryHandle = nvGenerateUnixRmHandle(&pDevEvo->handleAllocator);

    if (memoryHandle == 0) {
        goto fail;
    }

    size = (sizeof(NVEvoLutDataRec) + 63) & ~63;
    attr = FLD_SET_DRF(OS32, _ATTR, _LOCATION, _VIDMEM, attr);
    attr2 = FLD_SET_DRF(OS32, _ATTR2, _GPU_CACHEABLE, _DEFAULT, attr2);
    attr2 = FLD_SET_DRF(OS32, _ATTR2, _ISO, _YES, attr2);

    alignment = NV_MAX(alignment, NV_EVO_SURFACE_ALIGNMENT);
    if (alignment != 0) {
        allocFlags |= NVOS32_ALLOC_FLAGS_ALIGNMENT_FORCE;
    }

    memAllocParams.owner = NVKMS_RM_HEAP_ID;
    memAllocParams.type = NVOS32_TYPE_IMAGE;
    memAllocParams.size = size;
    memAllocParams.attr = attr;
    memAllocParams.attr2 = attr2;
    memAllocParams.flags = allocFlags;
    memAllocParams.alignment = alignment;

    ret = nvRmApiAlloc(nvEvoGlobal.clientHandle,
                       pDevEvo->deviceHandle,
                       memoryHandle,
                       NV01_MEMORY_LOCAL_USER,
                       &memAllocParams);

    /* If we failed the allocation above, abort */
    if (ret != NVOS_STATUS_SUCCESS) {
        nvFreeUnixRmHandle(&pDevEvo->handleAllocator, memoryHandle);

        goto fail;
    }

    pSurfEvo = RegisterLutSurfaceEvo(pDevEvo, memoryHandle);
    /*
     * nvEvoRegisterSurface dups the memory handle, so we free the one we
     * just created.
     */
    nvRmApiFree(nvEvoGlobal.clientHandle,
                pDevEvo->deviceHandle,
                memoryHandle);
    nvFreeUnixRmHandle(&pDevEvo->handleAllocator, memoryHandle);
    memoryHandle = 0;

    if (pSurfEvo == NULL) {
        goto fail;
    }

    /*
     * Map the surface for the CPU. This is only done by nvEvoRegisterSurface
     * for NISO surfaces, so it must be done manually here.
     */
    if (!nvRmEvoMapVideoMemory(pDevEvo,
                               pSurfEvo->planes[0].rmHandle,
                               size, pSurfEvo->cpuAddress,
                               SUBDEVICE_MASK_ALL)) {
        goto fail;
    }

    /*
     * The GPU mapping is only needed for prefetching LUT surfaces for DIFR.
     * It isn't worth failing alone but we want to keep gpuAddress coherent.
     */
    pSurfEvo->gpuAddress = nvHsMapSurfaceToDevice(pDevEvo,
                                                  pSurfEvo->planes[0].rmHandle,
                                                  size,
                                                  NvHsMapPermissionsReadOnly);

    if (pSurfEvo->gpuAddress == NV_HS_BAD_GPU_ADDRESS) {
        pSurfEvo->gpuAddress = 0ULL;
    }

    return pSurfEvo;

fail:
    /* An error occurred -- free the surface */
    FreeLutSurfaceEvo(pDevEvo, pSurfEvo);

    return NULL;
}

static NVSurfaceEvoPtr AllocLutSurfaceEvoInSysmem(NVDevEvoPtr pDevEvo)
{
    NvU32 memoryHandle = 0;
    void *pBase = NULL;
    NvU64 size = (sizeof(NVEvoLutDataRec) + 63) & ~63;
    NVSurfaceEvoPtr pSurfEvo = NULL;
    NvU32 ret = 0;

    memoryHandle = nvGenerateUnixRmHandle(&pDevEvo->handleAllocator);
    if (memoryHandle == 0) {
        goto fail;
    }

    /* Allocate the LUT memory from sysmem */
    if (!nvRmAllocSysmem(pDevEvo, memoryHandle, NULL, &pBase, size,
                         NVKMS_MEMORY_ISO)) {
        nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                    "Unable to allocate LUT memory from sysmem");
        nvFreeUnixRmHandle(&pDevEvo->handleAllocator, memoryHandle);

        goto fail;
    }

    pSurfEvo = RegisterLutSurfaceEvo(pDevEvo, memoryHandle);

    /*
     * nvEvoRegisterSurface dups the memory handle, so we can free the one we
     * just created.
     */
    nvRmApiUnmapMemory(nvEvoGlobal.clientHandle,
                       pDevEvo->deviceHandle,
                       memoryHandle,
                       pBase,
                       0 /* flags */);

    nvRmApiFree(nvEvoGlobal.clientHandle,
                pDevEvo->deviceHandle,
                memoryHandle);
    nvFreeUnixRmHandle(&pDevEvo->handleAllocator, memoryHandle);
    memoryHandle = 0;

    if (pSurfEvo == NULL) {
        goto fail;
    }

    ret = nvRmApiMapMemory(nvEvoGlobal.clientHandle,
                           pDevEvo->deviceHandle,
                           pSurfEvo->planes[0].rmHandle,
                           0, /* offset */
                           size,
                           &pBase,
                           0 /* flags */);

    if (ret != NVOS_STATUS_SUCCESS) {
        goto fail;
    }

    /*
     * AllocLutSurfaceEvoInSysmem() will only be called if
     * pDevEvo->requiresAllAllocationsInSysmem is TRUE. NVKMS will only set this
     * cap bit for SOC display devices, and these devices should only have one
     * subdevice.
     */
    nvAssert(pDevEvo->numSubDevices == 1);
    pSurfEvo->cpuAddress[0] = pBase;

    return pSurfEvo;

fail:
    /* An error occurred -- free the surface */
    FreeLutSurfaceEvo(pDevEvo, pSurfEvo);

    return NULL;
}

static NVSurfaceEvoPtr AllocLutSurfaceEvo(NVDevEvoPtr pDevEvo)
{
    if (pDevEvo->requiresAllAllocationsInSysmem) {
        return AllocLutSurfaceEvoInSysmem(pDevEvo);
    } else {
        return AllocLutSurfaceEvoInVidmem(pDevEvo);
    }
}

NvBool nvSetTmoLutSurfaceEvo(NVDevEvoPtr pDevEvo,
                             NVFlipChannelEvoHwState *pHwState)
{
    if (pHwState->hdrStaticMetadata.enabled) {
        if (!pHwState->tmoLut.pLutSurfaceEvo) {
            pHwState->tmoLut.pLutSurfaceEvo = AllocLutSurfaceEvo(pDevEvo);
            if (!pHwState->tmoLut.pLutSurfaceEvo) {
                return FALSE;
            }
        }
    } else {
        // Will be freed via nvEvoDecrementSurfaceRefCnts() and
        // nvFreeUnrefedTmoLutSurfacesEvo() on old state
        pHwState->tmoLut.pLutSurfaceEvo = NULL;
    }

    return TRUE;
}

/*
 * After the flip dereferences its TMO surfaces, or when it fails after TMO
 * surface allocation, any unused TMO surfaces are left with 1 refcount. If
 * these TMO surfaces are owned by the pDevEvo's pNvKmsOpenDev, then they have
 * been allocated within this file and need to be freed. If not, they have been
 * allocated by an NvKms client, which is in charge of freeing them.
 *
 * Only call FreeLutSurfaceEvo in the first case.
 */
void nvFreeUnrefedTmoLutSurfacesEvo(NVDevEvoPtr pDevEvo,
                                    NVFlipEvoHwState *pFlipState,
                                    NvU32 head)
{
    NvU32 layer;
    for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
        if (pFlipState->layer[layer].tmoLut.pLutSurfaceEvo != NULL &&
            pFlipState->layer[layer].tmoLut.pLutSurfaceEvo->structRefCnt <= 1) {

            if (pFlipState->layer[layer].tmoLut.pLutSurfaceEvo->owner.pOpenDev ==
                pDevEvo->pNvKmsOpenDev) {

                FreeLutSurfaceEvo(pDevEvo,
                    pFlipState->layer[layer].tmoLut.pLutSurfaceEvo);
            }
            pFlipState->layer[layer].tmoLut.pLutSurfaceEvo = NULL;
        }
    }
}

void nvInvalidateDefaultLut(NVDevEvoPtr pDevEvo)
{
    NvU32 sd;

    for (sd = 0; sd < NVKMS_MAX_SUBDEVICES; sd++) {
        pDevEvo->lut.defaultBaseLUTState[sd] =
        pDevEvo->lut.defaultOutputLUTState[sd] =
            NvKmsLUTStateUninitialized;
    }
}

NvBool nvAllocLutSurfacesEvo(NVDevEvoPtr pDevEvo)
{
    NVDispEvoPtr pDispEvo;
    NvU32 apiHead, dispIndex, i;

    for (apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
        for (i = 0; i < ARRAY_LEN(pDevEvo->lut.apiHead[apiHead].LUT); i++) {
            pDevEvo->lut.apiHead[apiHead].LUT[i] = AllocLutSurfaceEvo(pDevEvo);

            if (pDevEvo->lut.apiHead[apiHead].LUT[i] == NULL) {
                nvFreeLutSurfacesEvo(pDevEvo);
                return FALSE;
            }
        }

        FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
            // No palette has been loaded yet, so disable the LUT.
            pDevEvo->lut.apiHead[apiHead].disp[dispIndex].waitForPreviousUpdate = FALSE;
            pDevEvo->lut.apiHead[apiHead].disp[dispIndex].curBaseLutEnabled = FALSE;
            pDevEvo->lut.apiHead[apiHead].disp[dispIndex].curOutputLutEnabled = FALSE;
        }
    }

    // Zero-initalize the LUT notifier state - ensure there's no stale data
    nvkms_memset(&pDevEvo->lut.notifierState, 0,
                 sizeof(pDevEvo->lut.notifierState));

    if (pDevEvo->hal->caps.needDefaultLutSurface) {
        pDevEvo->lut.defaultLut = AllocLutSurfaceEvo(pDevEvo);
        if (pDevEvo->lut.defaultLut == NULL) {
            nvFreeLutSurfacesEvo(pDevEvo);
            return FALSE;
        }

        nvInvalidateDefaultLut(pDevEvo);

        pDevEvo->hal->InitDefaultLut(pDevEvo);
    }

    return TRUE;
}

void nvFreeLutSurfacesEvo(NVDevEvoPtr pDevEvo)
{
    NvU32 head, i, dispIndex, apiHead;
    NVDispEvoPtr pDispEvo;

    /* Cancel any queued LUT update timers */
    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        for (apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
            nvCancelLutUpdateEvo(pDispEvo, apiHead);
        }
    }

    /* wait for any outstanding LUT updates before freeing the surface */
    if (pDevEvo->core) {
        nvRMSyncEvoChannel(pDevEvo, pDevEvo->core, __LINE__);
    }

    /* Clear the current lut surface stored in the hardware head state */
    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        for (head = 0; head < pDevEvo->numHeads; head++) {
            pDispEvo->headState[head].lut.pCurrSurface = NULL;
            pDispEvo->headState[head].lut.baseLutEnabled = FALSE;
            pDispEvo->headState[head].lut.outputLutEnabled = FALSE;
        }
    }

    if (pDevEvo->lut.defaultLut != NULL) {
        FreeLutSurfaceEvo(pDevEvo, pDevEvo->lut.defaultLut);
        pDevEvo->lut.defaultLut = NULL;
    }

    for (apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
        for (i = 0; i < ARRAY_LEN(pDevEvo->lut.apiHead[apiHead].LUT); i++) {
            if (pDevEvo->lut.apiHead[apiHead].LUT[i] != NULL) {
                FreeLutSurfaceEvo(pDevEvo, pDevEvo->lut.apiHead[apiHead].LUT[i]);
                pDevEvo->lut.apiHead[apiHead].LUT[i] = NULL;
            }
        }
    }
}

void nvUploadDataToLutSurfaceEvo(NVSurfaceEvoPtr pSurfEvo,
                                 const NVEvoLutDataRec *pLUTBuffer,
                                 NVDispEvoPtr pDispEvo)
{
    const NvU32* data = (const NvU32*)pLUTBuffer;
    size_t size = sizeof(*pLUTBuffer);
    const int sd = pDispEvo->displayOwner;
    NvU32 *dst;
    const NvU32 *src;
    int dword;

    if (pSurfEvo == NULL) {
        nvAssert(pSurfEvo);
        return;
    }

    nvAssert(pSurfEvo->cpuAddress[sd]);

    /* The size to copy should not be larger than the surface. */
    nvAssert(size <= pSurfEvo->planes[0].rmObjectSizeInBytes);

    /* The source, destination, and size should be 4-byte aligned. */
    nvAssert((((NvUPtr)data) & 0x3) == 0);
    nvAssert((((NvUPtr)pSurfEvo->cpuAddress[sd]) & 0x3) == 0);
    nvAssert((size % 4) == 0);

    src = data;
    dst = (NvU32*)pSurfEvo->cpuAddress[sd];

    for (dword = 0; dword < (size/4); dword++) {
        *(dst++) = *(src++);
    }
}
