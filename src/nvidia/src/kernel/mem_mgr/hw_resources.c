/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "core/core.h"
#include "mem_mgr_internal.h"
#include "mem_mgr/hw_resources.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "vgpu/vgpu_util.h"
#include "mem_mgr/mem.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/mem_mgr/heap.h"
#include "gpu/mem_sys/kern_mem_sys.h"
#include "rmapi/client.h"
#include "mmu/gmmu_fmt.h"
#include "gpu/device/device.h"
#include "vgpu/rpc.h"

#include "class/cl0041.h" // NV04_MEMORY
#include "class/cl003e.h" // NV01_MEMORY_SYSTEM
#include "class/cl00b1.h" // NV01_MEMORY_HW_RESOURCES
#include "class/cl0040.h" // NV01_MEMORY_LOCAL_USER

static NV_STATUS
_hwresHwAlloc
(
    OBJGPU         *pGpu,
    NvHandle        hClient,
    NvHandle        hDevice,
    NvHandle        hMemory,
    MEMORY_HW_RESOURCES_ALLOCATION_REQUEST *pHwAlloc,
    NvU32           *pAttr,
    NvU32           *pAttr2
)
{
    MemoryManager          *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    NV_STATUS               status = NV_OK;
    FB_ALLOC_INFO          *pFbAllocInfo = NULL;
    FB_ALLOC_PAGE_FORMAT   *pFbAllocPageFormat = NULL;
    NvU64                   pageSize = 0;
    NV_MEMORY_HW_RESOURCES_ALLOCATION_PARAMS *pUserParams = pHwAlloc->pUserParams;

    // Ensure a valid allocation type was passed in
    if (pUserParams->type > NVOS32_NUM_MEM_TYPES - 1)
        return NV_ERR_GENERIC;

    pFbAllocInfo = portMemAllocNonPaged(sizeof(FB_ALLOC_INFO));
    if (pFbAllocInfo == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "No memory for Resource %p\n",
                  pHwAlloc->pHandle);
        status = NV_ERR_GENERIC;
        goto failed;
    }
    pFbAllocPageFormat = portMemAllocNonPaged(sizeof(FB_ALLOC_PAGE_FORMAT));
    if (pFbAllocPageFormat == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "No memory for Resource %p\n",
                  pHwAlloc->pHandle);
        status = NV_ERR_GENERIC;
        goto failed;
    }

    portMemSet(pFbAllocInfo, 0x0, sizeof(FB_ALLOC_INFO));
    portMemSet(pFbAllocPageFormat, 0x0, sizeof(FB_ALLOC_PAGE_FORMAT));
    pFbAllocInfo->pageFormat        = pFbAllocPageFormat;
    pFbAllocInfo->pageFormat->type  = pUserParams->type;
    pFbAllocInfo->hwResId       = 0;
    pFbAllocInfo->pad           = 0;
    pFbAllocInfo->height        = pUserParams->height;
    pFbAllocInfo->width         = pUserParams->width;
    pFbAllocInfo->pitch         = pUserParams->pitch;
    pFbAllocInfo->size          = pUserParams->size;
    pFbAllocInfo->origSize      = pUserParams->size;
    pFbAllocInfo->pageFormat->kind  = pUserParams->kind;
    pFbAllocInfo->offset        = memmgrGetInvalidOffset_HAL(pGpu, pMemoryManager);
    pFbAllocInfo->hClient       = hClient;
    pFbAllocInfo->hDevice       = hDevice;
    pFbAllocInfo->pageFormat->flags = pUserParams->flags;
    pFbAllocInfo->pageFormat->attr  = pUserParams->attr;
    pFbAllocInfo->pageFormat->attr2 = pUserParams->attr2;
    pFbAllocInfo->retAttr       = pUserParams->attr;
    pFbAllocInfo->retAttr2      = pUserParams->attr2;
    pFbAllocInfo->comprCovg     = pUserParams->comprCovg;
    pFbAllocInfo->zcullCovg     = 0;
    pFbAllocInfo->internalflags = 0;

    if ((pUserParams->flags & NVOS32_ALLOC_FLAGS_ALIGNMENT_HINT) ||
        (pUserParams->flags & NVOS32_ALLOC_FLAGS_ALIGNMENT_FORCE))
        pFbAllocInfo->align = pUserParams->alignment;
    else
        pFbAllocInfo->align = RM_PAGE_SIZE;

    // Fetch RM page size
    pageSize = memmgrDeterminePageSize(pMemoryManager, pFbAllocInfo->hClient, pFbAllocInfo->size,
                                       pFbAllocInfo->format, pFbAllocInfo->pageFormat->flags,
                                       &pFbAllocInfo->retAttr, &pFbAllocInfo->retAttr2);
    if (pageSize == 0)
    {
        status = NV_ERR_INVALID_STATE;
        NV_PRINTF(LEVEL_ERROR, "memmgrDeterminePageSize failed\n");
    }

    // Fetch memory alignment
    status = memmgrAllocDetermineAlignment_HAL(pGpu, pMemoryManager, &pFbAllocInfo->size, &pFbAllocInfo->align,
                                               pFbAllocInfo->alignPad, pFbAllocInfo->pageFormat->flags,
                                               pFbAllocInfo->retAttr, pFbAllocInfo->retAttr2, 0);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "memmgrAllocDetermineAlignment failed\n");
    }

    //
    // vGPU:
    //
    // Since vGPU does all real hardware management in the
    // host, if we are in guest OS (where IS_VIRTUAL(pGpu) is true),
    // do an RPC to the host to do the hardware update.
    //
    if ((status == NV_OK) && IS_VIRTUAL(pGpu))
    {
        if (vgpuIsGuestManagedHwAlloc(pGpu) &&
            (FLD_TEST_DRF(OS32, _ATTR, _COMPR, _NONE, pFbAllocInfo->pageFormat->attr)))
        {
            status = memmgrAllocHwResources(pGpu, pMemoryManager, pFbAllocInfo);
            pHwAlloc->hwResource.isVgpuHostAllocated = NV_FALSE;
            NV_ASSERT(status == NV_OK);
        }
        else
        {
            NV_RM_RPC_MANAGE_HW_RESOURCE_ALLOC(pGpu,
                                               hClient,
                                               hDevice,
                                               hMemory,
                                               pFbAllocInfo,
                                               status);
            pHwAlloc->hwResource.isVgpuHostAllocated = NV_TRUE;
        }

        pUserParams->uncompressedKind      = pFbAllocInfo->uncompressedKind;
        pUserParams->compPageShift         = pFbAllocInfo->compPageShift;
        pUserParams->compressedKind        = pFbAllocInfo->compressedKind;
        pUserParams->compTagLineMin        = pFbAllocInfo->compTagLineMin;
        pUserParams->compPageIndexLo       = pFbAllocInfo->compPageIndexLo;
        pUserParams->compPageIndexHi       = pFbAllocInfo->compPageIndexHi;
        pUserParams->compTagLineMultiplier = pFbAllocInfo->compTagLineMultiplier;
    }
    else
    {
        //
        // Call into HAL to reserve any hardware resources for
        // the specified memory type.
        // If the alignment was changed due to a HW limitation, and the
        // flag NVOS32_ALLOC_FLAGS_ALIGNMENT_FORCE is set, bad_argument
        // will be passed back from the HAL
        //
        status = memmgrAllocHwResources(pGpu, pMemoryManager, pFbAllocInfo);
    }

    // Is status bad or did we request attributes and they failed
    if ((status != NV_OK) || ((pUserParams->attr) && (0x0 == pFbAllocInfo->retAttr)))
    {
        //
        // probably means we passed in a bogus type or no tiling resources available
        // when tiled memory attribute was set to REQUIRED
        //
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "nvHalFbAlloc failure status = 0x%x Requested Attr 0x%x!\n",
                      status, pUserParams->attr);
        }
        else
        {
            NV_PRINTF(LEVEL_WARNING,
                      "nvHalFbAlloc Out of Resources Requested=%x Returned=%x !\n",
                      pUserParams->attr, pFbAllocInfo->retAttr);
        }
        goto failed;
    }

    //
    // Refresh search parameters.
    //
    pUserParams->pitch  = pFbAllocInfo->pitch;

    pUserParams->height = pFbAllocInfo->height;
    pHwAlloc->pad = NvU64_LO32(pFbAllocInfo->pad);
    pUserParams->kind = pFbAllocInfo->pageFormat->kind;
    pHwAlloc->hwResId = pFbAllocInfo->hwResId;

    pUserParams->size = pFbAllocInfo->size;           // returned to caller

    pHwAlloc->hwResource.attr = pFbAllocInfo->retAttr;
    pHwAlloc->hwResource.attr2 = pFbAllocInfo->retAttr2;
    pHwAlloc->hwResource.comprCovg = pFbAllocInfo->comprCovg;
    pHwAlloc->hwResource.ctagOffset = pFbAllocInfo->ctagOffset;
    pHwAlloc->hwResource.hwResId = pFbAllocInfo->hwResId;

    *pAttr  = pFbAllocInfo->retAttr;
    *pAttr2 = pFbAllocInfo->retAttr2;

failed:
    portMemFree(pFbAllocPageFormat);
    portMemFree(pFbAllocInfo);

    return status;
}

NV_STATUS
hwresConstruct_IMPL
(
    MemoryHwResources            *pMemoryHwResources,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    NV_MEMORY_HW_RESOURCES_ALLOCATION_PARAMS *pAllocData;
    MEMORY_HW_RESOURCES_ALLOCATION_REQUEST    allocRequest  = {0};
    MEMORY_HW_RESOURCES_ALLOCATION_REQUEST   *pAllocRequest = &allocRequest;
    Memory             *pMemory        = staticCast(pMemoryHwResources, Memory);
    OBJGPU             *pGpu           = pMemory->pGpu;
    MemoryManager      *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    Heap               *pHeap          = MEMORY_MANAGER_GET_HEAP(pMemoryManager);
    NV_STATUS           status         = NV_OK;
    NvHandle            hDevice        = RES_GET_HANDLE(pMemory->pDevice);
    NvHandle            hMemory        = pCallContext->pResourceRef->hResource;
    NvHandle            hClient        = pCallContext->pClient->hClient;
    NvU32               retAttr        = 0;
    NvU32               retAttr2       = 0;
    NvBool              bVidmem;
    const MEMORY_SYSTEM_STATIC_CONFIG *pMemorySystemConfig =
        kmemsysGetStaticConfig(pGpu, GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu));

    // Copy-construction has already been done by the base Memory class
    if (RS_IS_COPY_CTOR(pParams))
        return NV_OK;

    pAllocData = pParams->pAllocParams;

    if (gpuIsDebuggerActive_HAL(pGpu))
    {
        // Bug 643431 - WAR for GR WFI timeouts when debugger is active
        return NV_ERR_BUSY_RETRY;
    }

    // Init alloc request
    pAllocRequest->pUserParams = pAllocData;
    pAllocRequest->bindResultFunc = NvP64_VALUE(pAllocData->bindResultFunc);
    pAllocRequest->pHandle = NvP64_VALUE(pAllocData->pHandle);
    pAllocRequest->hwResId = 0;

    bVidmem = FLD_TEST_DRF(OS32, _ATTR, _LOCATION, _VIDMEM, pAllocData->attr);

    status = _hwresHwAlloc(pGpu, hClient, hDevice, hMemory,
                           pAllocRequest, &retAttr, &retAttr2);

    pAllocData->attr = retAttr;
    pAllocData->attr2 = retAttr2;

    // If Allocation succeeded then allocate a handle
    if (status == NV_OK)
    {
        MEMORY_DESCRIPTOR *pMemDesc;

        //
        // Default to not GPU-cachable. This doesn't matter for the HW resources
        // class but the lower level code expects this field to be updated.
        //
        if (FLD_TEST_DRF(OS32, _ATTR2, _GPU_CACHEABLE, _DEFAULT, pAllocData->attr2))
        {
            pAllocData->attr2 = FLD_SET_DRF(OS32, _ATTR2, _GPU_CACHEABLE, _NO,
                                            pAllocData->attr2);
        }

        // comtags lines are allocated contiguously.
        //pAllocData->attr = FLD_SET_DRF(OS32, _ATTR, _PHYSICALITY, _CONTIGUOUS,
        //pUserParams->attr);

        status = memCreateMemDesc(pGpu,
                                  &pMemDesc,
                                  bVidmem ? ADDR_FBMEM : ADDR_SYSMEM,
                                  // Offset - do not know this yet. Must be page aligned.
                                  memmgrGetInvalidOffset_HAL(pGpu, pMemoryManager),
                                  pAllocData->size + pAllocRequest->pad,
                                  pAllocData->attr,
                                  pAllocData->attr2);

        if (status == NV_OK)
        {
            status = memConstructCommon(pMemory,
                                        bVidmem ? NV01_MEMORY_LOCAL_USER : NV01_MEMORY_SYSTEM,
                                        pAllocData->flags, pMemDesc,
                                        pAllocData->owner, pHeap, pAllocData->attr,
                                        pAllocData->attr2, 0, pAllocData->type,
                                        NVOS32_MEM_TAG_NONE, &pAllocRequest->hwResource);

            if (status == NV_OK)
            {
                NV_ASSERT(pMemory->pMemDesc);
                memdescSetPteKind(pMemory->pMemDesc, pAllocData->kind);
                memdescSetHwResId(pMemory->pMemDesc, pAllocRequest->hwResId);

                pMemory->osDeviceHandle = pAllocData->osDeviceHandle;

                if (IS_VIRTUAL(pGpu) && vgpuIsGuestManagedHwAlloc(pGpu))
                {
                    pAllocData->compPageShift   = pMemorySystemConfig->comprPageShift;
                    pAllocData->compressedKind  = pAllocData->kind;

                    if (memmgrIsKind_HAL(pMemoryManager, FB_IS_KIND_COMPRESSIBLE, pAllocData->kind))
                    {
                        pAllocData->uncompressedKind = memmgrGetUncompressedKind_HAL(pGpu, pMemoryManager,
                                                                                     pAllocData->kind, NV_FALSE);
                    }
                    else
                    {
                        pAllocData->uncompressedKind = pAllocData->kind;
                    }
                }

                if (!IS_VIRTUAL(pGpu) && !IS_GSP_CLIENT(pGpu))
                {
                    NvU32 compressedKind;
                    COMPR_INFO comprInfo;

                    status = memmgrGetKindComprForGpu_HAL(pMemoryManager,
                                                          pMemory->pMemDesc,
                                                          pMemory->pMemDesc->pGpu,
                                                          0,
                                                          &compressedKind,
                                                          &comprInfo);

                    if (status == NV_OK)
                    {
                        pAllocData->compPageShift   = pMemorySystemConfig->comprPageShift;
                        pAllocData->compressedKind  = comprInfo.kind;
                        pAllocData->compTagLineMin  = comprInfo.compTagLineMin;
                        pAllocData->compPageIndexLo = comprInfo.compPageIndexLo;
                        pAllocData->compPageIndexHi = comprInfo.compPageIndexHi;
                        pAllocData->compTagLineMultiplier = comprInfo.compTagLineMultiplier;

                        if (memmgrIsKind_HAL(pMemoryManager, FB_IS_KIND_COMPRESSIBLE, compressedKind))
                        {
                            pAllocData->uncompressedKind = memmgrGetUncompressedKind_HAL(pGpu, pMemoryManager,
                                                                                         compressedKind, NV_FALSE);
                        }
                        else
                        {
                            pAllocData->uncompressedKind = compressedKind;
                        }
                    }
                }
            }

            if (status != NV_OK)
            {
                memdescDestroy(pMemDesc);
            }
        }
    }

    return status;
}

void
hwresDestruct_IMPL
(
    MemoryHwResources *pMemoryHwResources
)
{
    Memory            *pMemory = staticCast(pMemoryHwResources, Memory);
    MEMORY_DESCRIPTOR *pMemDesc = pMemory->pMemDesc;

    memDestructCommon(pMemory);

    NV_ASSERT(pMemDesc->Allocated == 0);
    memdescFree(pMemDesc);
    memdescDestroy(pMemDesc);
}

NvBool
hwresCanCopy_IMPL
(
    MemoryHwResources *pMemoryHwResources
)
{
    return NV_TRUE;
}
