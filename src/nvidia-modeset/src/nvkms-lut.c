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
#include "nvkms-utils.h"
#include "nvos.h"

#include <class/cl0040.h> /* NV01_MEMORY_LOCAL_USER */

static void FreeLutSurfaceEvoInVidmem(NVLutSurfaceEvoPtr pSurfEvo)
{
    NVDevEvoPtr pDevEvo;

    if (pSurfEvo == NULL) {
        return;
    }

    pDevEvo = pSurfEvo->pDevEvo;

    if (pSurfEvo->gpuAddress) {
        nvRmApiUnmapMemoryDma(nvEvoGlobal.clientHandle,
                              pDevEvo->deviceHandle,
                              pDevEvo->nvkmsGpuVASpace,
                              pSurfEvo->handle,
                              0,
                              (NvU64)pSurfEvo->gpuAddress);
    }

    nvRmEvoUnMapVideoMemory(pDevEvo, pSurfEvo->handle,
                            pSurfEvo->subDeviceAddress);

    /* Free surface descriptor */
    pDevEvo->hal->FreeSurfaceDescriptor(pDevEvo,
                                        nvEvoGlobal.clientHandle,
                                        &pSurfEvo->surfaceDesc);

    /* Free the surface */
    if (pSurfEvo->handle) {
        NvU32 result;

        result = nvRmApiFree(nvEvoGlobal.clientHandle,
                             pDevEvo->deviceHandle, pSurfEvo->handle);
        if (result != NVOS_STATUS_SUCCESS) {
            nvAssert(!"Freeing LUT surface failed");
        }

        nvFreeUnixRmHandle(&pDevEvo->handleAllocator,
                           pSurfEvo->handle);
        pSurfEvo->handle = 0;
    }

    nvFree(pSurfEvo);
}

static NVLutSurfaceEvoPtr AllocLutSurfaceEvoInVidmem(NVDevEvoPtr pDevEvo)
{
    NV_MEMORY_ALLOCATION_PARAMS memAllocParams = { };
    NvU32 ret = NVOS_STATUS_ERROR_GENERIC;
    NvU32 attr = 0, attr2 = 0;
    NvU32 allocFlags = NVOS32_ALLOC_FLAGS_FORCE_MEM_GROWS_DOWN |
                       NVOS32_ALLOC_FLAGS_ALIGNMENT_FORCE;
    NvU64 size = 0, alignment = 4096;

    NVLutSurfaceEvoPtr pSurfEvo;

    pSurfEvo = nvCalloc(1, sizeof(*pSurfEvo));
    if (pSurfEvo == NULL) {
        return NULL;
    }

    pSurfEvo->pDevEvo = pDevEvo;

    size = (sizeof(NVEvoLutDataRec) + 63) & ~63;

    pSurfEvo->size = size;

    pSurfEvo->handle = nvGenerateUnixRmHandle(&pDevEvo->handleAllocator);

    if (pSurfEvo->handle == 0) {
        goto fail;
    }

    attr = FLD_SET_DRF(OS32, _ATTR, _LOCATION, _VIDMEM, attr);
    attr2 = FLD_SET_DRF(OS32, _ATTR2, _GPU_CACHEABLE, _DEFAULT, attr2);

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
                       pSurfEvo->handle,
                       NV01_MEMORY_LOCAL_USER,
                       &memAllocParams);

    /* If we failed the allocation above, abort */
    if (ret != NVOS_STATUS_SUCCESS) {
        nvFreeUnixRmHandle(&pDevEvo->handleAllocator, pSurfEvo->handle);
        pSurfEvo->handle = 0;

        goto fail;
    }

    /* Allocate and bind surface descriptor */
    ret =
        nvRmAllocAndBindSurfaceDescriptor(
                pDevEvo,
                pSurfEvo->handle,
                NvKmsSurfaceMemoryLayoutPitch,
                pSurfEvo->size - 1,
                &pSurfEvo->surfaceDesc);
    if (ret != NVOS_STATUS_SUCCESS) {
        goto fail;
    }

    /* Map the surface for the CPU */
    if (!nvRmEvoMapVideoMemory(pSurfEvo->pDevEvo,
                               pSurfEvo->handle, pSurfEvo->size,
                               pSurfEvo->subDeviceAddress,
                               SUBDEVICE_MASK_ALL)) {
        goto fail;
    }

    /*
     * The GPU mapping is only needed for prefetching LUT surfaces for DIFR.
     * It isn't worth failing alone but we want to keep gpuAddress coherent.
     */
    ret = nvRmApiMapMemoryDma(nvEvoGlobal.clientHandle,
                              pDevEvo->deviceHandle,
                              pDevEvo->nvkmsGpuVASpace,
                              pSurfEvo->handle,
                              0,
                              pSurfEvo->size,
                              DRF_DEF(OS46, _FLAGS, _CACHE_SNOOP, _ENABLE) |
                              DRF_DEF(OS46, _FLAGS, _ACCESS, _READ_ONLY),
                              &pSurfEvo->gpuAddress);

    if (ret != NVOS_STATUS_SUCCESS) {
        pSurfEvo->gpuAddress = 0ULL;
    }

    return pSurfEvo;

 fail:
    /* An error occurred -- free the surface */
    FreeLutSurfaceEvoInVidmem(pSurfEvo);

    return NULL;

}

static void FreeLutSurfaceEvoInSysmem(NVLutSurfaceEvoPtr pSurfEvo)
{
    NVDevEvoPtr pDevEvo;

    if (pSurfEvo == NULL) {
        return;
    }

    pDevEvo = pSurfEvo->pDevEvo;

    /* Free surface descriptor */
    pDevEvo->hal->FreeSurfaceDescriptor(pDevEvo,
                                        nvEvoGlobal.clientHandle,
                                        &pSurfEvo->surfaceDesc);

    /* Free the surface */
    if (pSurfEvo->handle) {
        NvU32 result;

        if (pSurfEvo->subDeviceAddress[0] != NULL) {
            /*
             * SOC display devices should only have one subdevice
             * (and therefore it is safe to unmap only subDeviceAddress[0])
             * for reasons described in AllocLutSurfaceEvoInSysmem
             */
            nvAssert(pDevEvo->numSubDevices == 1);

            result = nvRmApiUnmapMemory(nvEvoGlobal.clientHandle,
                                        pDevEvo->deviceHandle,
                                        pSurfEvo->handle,
                                        pSurfEvo->subDeviceAddress[0],
                                        0);
            if (result != NVOS_STATUS_SUCCESS) {
                nvAssert(!"Unmapping LUT surface failed");
            }
            pSurfEvo->subDeviceAddress[0] = NULL;
        }

        result = nvRmApiFree(nvEvoGlobal.clientHandle,
                             pDevEvo->deviceHandle, pSurfEvo->handle);
        if (result != NVOS_STATUS_SUCCESS) {
            nvAssert(!"Freeing LUT surface failed");
        }

        nvFreeUnixRmHandle(&pDevEvo->handleAllocator, pSurfEvo->handle);
    }

    nvFree(pSurfEvo);
}

static NVLutSurfaceEvoPtr AllocLutSurfaceEvoInSysmem(NVDevEvoPtr pDevEvo)
{
    NvU32 memoryHandle = 0;
    void *pBase = NULL;
    NvU64 size = 0;
    NVLutSurfaceEvoPtr pSurfEvo;
    NvU32 ret;

    pSurfEvo = nvCalloc(1, sizeof(*pSurfEvo));
    if (pSurfEvo == NULL) {
        return NULL;
    }

    pSurfEvo->pDevEvo = pDevEvo;

    size = (sizeof(NVEvoLutDataRec) + 63) & ~63;

    pSurfEvo->size = size;

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

    pSurfEvo->handle = memoryHandle;

    /* Allocate and bind surface descriptor */
    ret =
        nvRmAllocAndBindSurfaceDescriptor(
                pDevEvo,
                pSurfEvo->handle,
                NvKmsSurfaceMemoryLayoutPitch,
                pSurfEvo->size - 1,
                &pSurfEvo->surfaceDesc);

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
    pSurfEvo->subDeviceAddress[0] = pBase;

    return pSurfEvo;

 fail:
    /* An error occurred -- free the surface */
    FreeLutSurfaceEvoInSysmem(pSurfEvo);

    return NULL;
}

static void FreeLutSurfaceEvo(NVLutSurfaceEvoPtr pSurfEvo)
{
    NVDevEvoPtr pDevEvo;

    if (pSurfEvo == NULL) {
        return;
    }

    pDevEvo = pSurfEvo->pDevEvo;

    if (pDevEvo->requiresAllAllocationsInSysmem) {
        FreeLutSurfaceEvoInSysmem(pSurfEvo);
    } else {
        FreeLutSurfaceEvoInVidmem(pSurfEvo);
    }
}

static NVLutSurfaceEvoPtr AllocLutSurfaceEvo(NVDevEvoPtr pDevEvo)
{
    if (pDevEvo->requiresAllAllocationsInSysmem) {
        return AllocLutSurfaceEvoInSysmem(pDevEvo);
    } else {
        return AllocLutSurfaceEvoInVidmem(pDevEvo);
    }
}

NvBool nvSetTmoLutSurfacesEvo(NVDevEvoPtr pDevEvo,
                              NVFlipEvoHwState *pFlipState,
                              NvU32 head)
{
    NvU32 layer;
    for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
        if (pFlipState->layer[layer].hdrStaticMetadata.enabled) {
            if (!pFlipState->layer[layer].tmoLut.pLutSurfaceEvo) {
                pFlipState->layer[layer].tmoLut.pLutSurfaceEvo =
                    AllocLutSurfaceEvo(pDevEvo);
                if (!pFlipState->layer[layer].tmoLut.pLutSurfaceEvo) {
                    return FALSE;
                }

                // Will be referenced via nvRefTmoLutSurfacesEvo() on new state
                pFlipState->layer[layer].tmoLut.pLutSurfaceEvo->allocRefCnt = 0;
            }
        } else {
            // Will be freed via nvUnrefTmoLutSurfacesEvo() on old state
            pFlipState->layer[layer].tmoLut.pLutSurfaceEvo = NULL;
        }
    }

    return TRUE;
}

void nvRefTmoLutSurfacesEvo(NVDevEvoPtr pDevEvo,
                            NVFlipEvoHwState *pFlipState,
                            NvU32 head)
{
    // Reference new state layers that have hdrStaticMetadata enabled.
    NvU32 layer;
    for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
        if (pFlipState->layer[layer].hdrStaticMetadata.enabled) {
            nvAssert(pFlipState->layer[layer].tmoLut.pLutSurfaceEvo);
            pFlipState->layer[layer].tmoLut.pLutSurfaceEvo->allocRefCnt++;
        }
    }
}

void nvUnrefTmoLutSurfacesEvo(NVDevEvoPtr pDevEvo,
                              NVFlipEvoHwState *pFlipState,
                              NvU32 head)
{
    // Unref old state layers that had hdrStaticMetadata enabled.
    NvU32 layer;
    for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
        if (pFlipState->layer[layer].hdrStaticMetadata.enabled) {
            nvAssert(pFlipState->layer[layer].tmoLut.pLutSurfaceEvo);

            if (pFlipState->layer[layer].tmoLut.pLutSurfaceEvo->allocRefCnt <= 1) {
                // Wait for any outstanding LUT updates before freeing.
                if (pDevEvo->core) {
                    nvRMSyncEvoChannel(pDevEvo, pDevEvo->core, __LINE__);
                }

                FreeLutSurfaceEvo(
                    pFlipState->layer[layer].tmoLut.pLutSurfaceEvo);

                pFlipState->layer[layer].tmoLut.pLutSurfaceEvo = NULL;
            } else {
                pFlipState->layer[layer].tmoLut.pLutSurfaceEvo->allocRefCnt--;
            }
        }
    }
}

NvBool nvAllocLutSurfacesEvo(NVDevEvoPtr pDevEvo)
{
    NVDispEvoPtr pDispEvo;
    NvU32 apiHead, dispIndex, i, sd;

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

        for (sd = 0; sd < NVKMS_MAX_SUBDEVICES; sd++) {
            pDevEvo->lut.defaultBaseLUTState[sd] =
            pDevEvo->lut.defaultOutputLUTState[sd] =
                NvKmsLUTStateUninitialized;
        }

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
        FreeLutSurfaceEvo(pDevEvo->lut.defaultLut);
        pDevEvo->lut.defaultLut = NULL;
    }

    for (apiHead = 0; apiHead < pDevEvo->numApiHeads; apiHead++) {
        for (i = 0; i < ARRAY_LEN(pDevEvo->lut.apiHead[apiHead].LUT); i++) {
            if (pDevEvo->lut.apiHead[apiHead].LUT[i] != NULL) {
                FreeLutSurfaceEvo(pDevEvo->lut.apiHead[apiHead].LUT[i]);
                pDevEvo->lut.apiHead[apiHead].LUT[i] = NULL;
            }
        }
    }
}

void nvUploadDataToLutSurfaceEvo(NVLutSurfaceEvoPtr pSurfEvo,
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

    nvAssert(pSurfEvo->subDeviceAddress[sd]);

    /* The size to copy should not be larger than the surface. */
    nvAssert(size <= pSurfEvo->size);

    /* The source, destination, and size should be 4-byte aligned. */
    nvAssert((((NvUPtr)data) & 0x3) == 0);
    nvAssert((((NvUPtr)pSurfEvo->subDeviceAddress[sd]) & 0x3) == 0);
    nvAssert((size % 4) == 0);

    src = data;
    dst = (NvU32*)pSurfEvo->subDeviceAddress[sd];

    for (dword = 0; dword < (size/4); dword++) {
        *(dst++) = *(src++);
    }
}
