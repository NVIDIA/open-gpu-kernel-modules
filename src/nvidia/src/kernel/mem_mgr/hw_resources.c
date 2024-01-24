/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "class/cl0041.h" // NV04_MEMORY
#include "class/cl003e.h" // NV01_MEMORY_SYSTEM
#include "class/cl00b1.h" // NV01_MEMORY_HW_RESOURCES
#include "class/cl0040.h" // NV01_MEMORY_LOCAL_USER

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
    NvU32               retAttr, retAttr2;
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

    status = heapHwAlloc(pGpu, pHeap, hClient, hDevice, hMemory,
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
    OBJGPU            *pGpu = pMemory->pGpu;
    MEMORY_DESCRIPTOR *pMemDesc = pMemory->pMemDesc;
    Heap              *pHeap = GPU_GET_HEAP(pGpu);

    //
    // Must be done before memDestructCommon, as memDestructCommon will update BC state
    // (3/8/2019 - is this comment stale?)
    //
    heapHwFree(pGpu, pHeap, pMemory, NVOS32_DELETE_RESOURCES_ALL);

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
