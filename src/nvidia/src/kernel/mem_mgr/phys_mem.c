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

#include "mem_mgr_internal.h"
#include "mem_mgr/phys_mem.h"
#include "os/os.h"
#include "vgpu/rpc.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "gpu/mmu/kern_gmmu.h"
#include "mem_mgr/mem.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/mem_mgr/heap.h"
#include "gpu/mem_sys/kern_mem_sys.h"
#include "gpu/bus/kern_bus.h"
#include "rmapi/client.h"
#include "virtualization/hypervisor/hypervisor.h"

#include "class/cl0040.h" // NV01_MEMORY_LOCAL_USER
#include "class/cl00c2.h" // NV01_MEMORY_LOCAL_PHYSICAL

NV_STATUS
physmemConstruct_IMPL
(
    PhysicalMemory               *pPhysicalMemory,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    NV_PHYSICAL_MEMORY_ALLOCATION_PARAMS *pAllocParams;
    NV_STATUS                             status         = NV_OK;
    NvHandle                              hClient        = pCallContext->pClient->hClient;
    NvHandle                              hParent        = pCallContext->pResourceRef->pParentRef->hResource;
    NvHandle                              hMemory        = pCallContext->pResourceRef->hResource;
    Memory                               *pMemory        = staticCast(pPhysicalMemory, Memory);
    OBJGPU                               *pGpu           = pMemory->pGpu;
    MemoryManager                        *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    KernelGmmu                           *pKernelGmmu    = GPU_GET_KERNEL_GMMU(pGpu);
    MEMORY_DESCRIPTOR                    *pMemDesc       = NULL;
    Heap                                 *pHeap          = MEMORY_MANAGER_GET_HEAP(pMemoryManager);
    HWRESOURCE_INFO                       hwResource     = {0};
    NvU64                                 heapBase;
    NvU64                                 trueLength;
    NvBool                                bCompressedKind;
    NvU32                                 attr           = DRF_DEF(OS32, _ATTR, _PHYSICALITY, _CONTIGUOUS) |
                                                           DRF_DEF(OS32, _ATTR, _LOCATION, _VIDMEM);
    NvU32                                 attr2          = 0;
    const MEMORY_SYSTEM_STATIC_CONFIG    *pMemorySystemConfig =
        kmemsysGetStaticConfig(pGpu, GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu));

    NV_ASSERT_OR_RETURN(RMCFG_FEATURE_KERNEL_RM, NV_ERR_NOT_SUPPORTED);

    // Copy-construction has already been done by the base Memory class
    if (RS_IS_COPY_CTOR(pParams))
        return NV_OK;

    //
    // This class does not allocate ANY framebuffer memory. This function
    // returns a dumb linear mapping to the entire framebuffer.
    //
    // The driver client is responsible for calling RmMapMemory() to actually
    // get mappings when needed.
    //
    //

    pAllocParams = pParams->pAllocParams;
    bCompressedKind = memmgrIsKind_HAL(pMemoryManager, FB_IS_KIND_COMPRESSIBLE, pAllocParams->format);

    heapGetBase(pHeap, &heapBase);
    heapGetSize(pHeap, &trueLength);
    pAllocParams->memSize = trueLength;

    switch (pAllocParams->pageSize)
    {
        case RM_PAGE_SIZE_512M:
            NV_ASSERT_OR_RETURN(kgmmuIsPageSize512mbSupported(pKernelGmmu), NV_ERR_INVALID_ARGUMENT);
            attr |= DRF_DEF(OS32, _ATTR, _PAGE_SIZE, _HUGE);
            attr2 |= DRF_DEF(OS32, _ATTR2, _PAGE_SIZE_HUGE, _512MB);
            break;
        case RM_PAGE_SIZE_HUGE:
            NV_ASSERT_OR_RETURN(kgmmuIsHugePageSupported(pKernelGmmu), NV_ERR_INVALID_ARGUMENT);
            attr |= DRF_DEF(OS32, _ATTR, _PAGE_SIZE, _HUGE);
            attr2 |= DRF_DEF(OS32, _ATTR2, _PAGE_SIZE_HUGE, _2MB);
            break;
        default:
            attr |= DRF_DEF(OS32, _ATTR, _PAGE_SIZE, _BIG);
            break;
    }

    if (bCompressedKind)
    {
        FB_ALLOC_INFO                      FbAllocInfo       = {0};
        FB_ALLOC_PAGE_FORMAT               fbAllocPageFormat = {0};

        NV_CHECK_OR_RETURN(LEVEL_ERROR,
            pMemorySystemConfig->bOneToOneComptagLineAllocation || pMemorySystemConfig->bUseRawModeComptaglineAllocation,
            NV_ERR_INVALID_ARGUMENT);

        attr |= DRF_DEF(OS32, _ATTR, _ZCULL, _NONE);
        attr2 |= DRF_DEF(OS32, _ATTR2, _ZBC, _PREFER_NO_ZBC);
        attr2 |= DRF_DEF(OS32, _ATTR2, _ZBC_SKIP_ZBCREFCOUNT, _YES);

        if (memmgrIsKind_HAL(pMemoryManager, FB_IS_KIND_DISALLOW_PLC, pAllocParams->format))
            attr |= DRF_DEF(OS32, _ATTR, _COMPR, _DISABLE_PLC_ANY);
        else
            attr |= DRF_DEF(OS32, _ATTR, _COMPR, _REQUIRED);

        FbAllocInfo.pageFormat          = &fbAllocPageFormat;
        FbAllocInfo.hClient             = hClient;
        FbAllocInfo.hDevice             = hParent;    /* device */
        FbAllocInfo.size                = trueLength;
        FbAllocInfo.origSize            = trueLength;
        FbAllocInfo.offset              = 0;
        FbAllocInfo.format              = pAllocParams->format;
        FbAllocInfo.retAttr             = attr;
        FbAllocInfo.retAttr2            = attr2;
        FbAllocInfo.pageFormat->kind    = FbAllocInfo.format;
        FbAllocInfo.pageFormat->attr    = FbAllocInfo.retAttr;
        FbAllocInfo.pageFormat->attr2   = FbAllocInfo.retAttr2;

        // Fetch RM page size
        NV_CHECK_OR_RETURN(LEVEL_ERROR,
            memmgrDeterminePageSize(pMemoryManager, FbAllocInfo.hClient, FbAllocInfo.size,
                                    FbAllocInfo.format, FbAllocInfo.pageFormat->flags,
                                   &FbAllocInfo.retAttr, &FbAllocInfo.retAttr2) != 0,
            NV_ERR_INVALID_ARGUMENT);

        // Fetch memory alignment
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            memmgrAllocDetermineAlignment_HAL(pGpu, pMemoryManager, &FbAllocInfo.size, &FbAllocInfo.align,
                                              FbAllocInfo.alignPad, FbAllocInfo.pageFormat->flags,
                                              FbAllocInfo.retAttr, FbAllocInfo.retAttr2, 0));

        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, memmgrAllocHwResources(pGpu, pMemoryManager, &FbAllocInfo));
        NV_ASSERT_OR_RETURN(FbAllocInfo.format == pAllocParams->format, NV_ERR_INVALID_ARGUMENT);

        attr = FbAllocInfo.retAttr;
        attr2 = FbAllocInfo.retAttr2;

        hwResource.attr       = FbAllocInfo.retAttr;
        hwResource.attr2      = FbAllocInfo.retAttr2;
        hwResource.comprCovg  = FbAllocInfo.comprCovg;
        hwResource.ctagOffset = FbAllocInfo.ctagOffset;
        hwResource.hwResId    = FbAllocInfo.hwResId;
    }

    status = memCreateMemDesc(pGpu, &pMemDesc, ADDR_FBMEM, 0, trueLength, attr, attr2);

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, status);

    memdescDescribe(pMemDesc, ADDR_FBMEM, heapBase, trueLength);

    memdescSetPteKind(pMemDesc, pAllocParams->format);
    if (bCompressedKind)
        memdescSetHwResId(pMemDesc, hwResource.hwResId);

    // Track internally as NV01_MEMORY_LOCAL_USER to share regular FB mem code paths
    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
                        memConstructCommon(pMemory, NV01_MEMORY_LOCAL_USER, 0, pMemDesc, 0,
                                NULL, attr, attr2, 0, 0, NVOS32_MEM_TAG_NONE,
                                bCompressedKind ? &hwResource : NULL),
                        cleanup_mem);

    if (!IS_GSP_CLIENT(pGpu))
    {
        //
        // vGPU:
        //
        // Since vGPU does all real hardware management in the
        // host, if we are in guest OS (where IS_VIRTUAL(pGpu) is true),
        // do an RPC to the host to do the hardware update.
        //
        NV_RM_RPC_ALLOC_LOCAL_USER(pGpu, hClient, hParent, hMemory, pMemDesc, trueLength,
                                   attr, attr2, pAllocParams->format, status);
        if (status != NV_OK)
        {
            // cleanup on an RPC failure
            goto cleanup_common;
        }

        pMemory->bRpcAlloc = NV_TRUE;
    }

    return NV_OK;

cleanup_common:
    memDestructCommon(pMemory);

cleanup_mem:
    memdescDestroy(pMemDesc);

    return status;
}

NvBool
physmemCanCopy_IMPL
(
    PhysicalMemory *pPhysicalMemory
)
{
    return NV_TRUE;
}
