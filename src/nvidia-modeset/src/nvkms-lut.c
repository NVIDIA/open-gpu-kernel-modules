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

    nvRmEvoUnMapVideoMemory(pDevEvo, pSurfEvo->handle,
                            pSurfEvo->subDeviceAddress);

    /* Free display context dmas for the surface, if any */
    nvRmEvoFreeDispContextDMA(pDevEvo, &pSurfEvo->dispCtxDma);

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

    /* Allocate a display context dma */
    pSurfEvo->dispCtxDma =
        nvRmEvoAllocateAndBindDispContextDMA(pDevEvo,
                                             pSurfEvo->handle,
                                             NvKmsSurfaceMemoryLayoutPitch,
                                             pSurfEvo->size - 1);

    if (!pSurfEvo->dispCtxDma) {
        goto fail;
    }

    /* Map the surface for the CPU */
    if (!nvRmEvoMapVideoMemory(pSurfEvo->pDevEvo,
                               pSurfEvo->handle, pSurfEvo->size,
                               pSurfEvo->subDeviceAddress,
                               SUBDEVICE_MASK_ALL)) {
        goto fail;
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

    /* Free display context dmas for the surface, if any */
    nvRmEvoFreeDispContextDMA(pDevEvo, &pSurfEvo->dispCtxDma);

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

    /* Allocate and bind a display context dma */
    pSurfEvo->dispCtxDma =
        nvRmEvoAllocateAndBindDispContextDMA(pDevEvo,
                                             pSurfEvo->handle,
                                             NvKmsSurfaceMemoryLayoutPitch,
                                             pSurfEvo->size - 1);
    if (!pSurfEvo->dispCtxDma) {
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

NvBool nvAllocLutSurfacesEvo(NVDevEvoPtr pDevEvo)
{
    NVDispEvoPtr pDispEvo;
    NvU32 head, dispIndex, i;

    for (head = 0; head < pDevEvo->numHeads; head++) {
        for (i = 0; i < ARRAY_LEN(pDevEvo->lut.head[head].LUT); i++) {
            pDevEvo->lut.head[head].LUT[i] = AllocLutSurfaceEvo(pDevEvo);

            if (pDevEvo->lut.head[head].LUT[i] == NULL) {
                nvFreeLutSurfacesEvo(pDevEvo);
                return FALSE;
            }
        }

        FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
            // No palette has been loaded yet, so disable the LUT.
            pDevEvo->lut.head[head].disp[dispIndex].waitForPreviousUpdate = FALSE;
            pDevEvo->lut.head[head].disp[dispIndex].curBaseLutEnabled = FALSE;
            pDevEvo->lut.head[head].disp[dispIndex].curOutputLutEnabled = FALSE;
        }
    }

    if (pDevEvo->hal->caps.needDefaultLutSurface) {
        pDevEvo->lut.defaultLut = AllocLutSurfaceEvo(pDevEvo);
        if (pDevEvo->lut.defaultLut == NULL) {
            nvFreeLutSurfacesEvo(pDevEvo);
            return FALSE;
        }

        pDevEvo->hal->InitDefaultLut(pDevEvo);
    }

    return TRUE;
}

void nvFreeLutSurfacesEvo(NVDevEvoPtr pDevEvo)
{
    NvU32 head, i, dispIndex;
    NVDispEvoPtr pDispEvo;

    /* Cancel any queued LUT update timers */
    FOR_ALL_EVO_DISPLAYS(pDispEvo, dispIndex, pDevEvo) {
        for (head = 0; head < pDevEvo->numHeads; head++) {
            nvCancelLutUpdateEvo(pDispEvo, head);
        }
    }

    /* wait for any outstanding LUT updates before freeing the surface */
    if (pDevEvo->core) {
        nvRMSyncEvoChannel(pDevEvo, pDevEvo->core, __LINE__);
    }

    if (pDevEvo->lut.defaultLut != NULL) {
        FreeLutSurfaceEvo(pDevEvo->lut.defaultLut);
        pDevEvo->lut.defaultLut = NULL;
    }

    for (head = 0; head < pDevEvo->numHeads; head++) {
        for (i = 0; i < ARRAY_LEN(pDevEvo->lut.head[head].LUT); i++) {
            if (pDevEvo->lut.head[head].LUT[i] != NULL) {
                FreeLutSurfaceEvo(pDevEvo->lut.head[head].LUT[i]);
                pDevEvo->lut.head[head].LUT[i] = NULL;
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
