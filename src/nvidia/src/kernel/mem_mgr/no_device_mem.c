/*
 * SPDX-FileCopyrightText: Copyright (c) 2018 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "mem_mgr/no_device_mem.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "os/os.h"

NV_STATUS
nodevicememConstruct_IMPL
(
    NoDeviceMemory *pNoDeviceMemory,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    NV_STATUS                    status;
    MEMORY_DESCRIPTOR           *pMemDesc;
    NV_MEMORY_ALLOCATION_PARAMS *pAllocParams = pParams->pAllocParams;
    Memory                      *pMemory = staticCast(pNoDeviceMemory, Memory);
    NvU32                        cpuCacheAttrib;
    NvU32                        coherency;
    NvU32                        protection;
    NvU32                        attr = 0;
    NvU32                        attr2 = 0;

    coherency = DRF_VAL(OS32, _ATTR, _COHERENCY, pAllocParams->attr);
    attr = FLD_SET_DRF_NUM(OS32, _ATTR, _COHERENCY, coherency, attr);
    switch (coherency)
    {
        case NVOS32_ATTR_COHERENCY_CACHED:
        case NVOS32_ATTR_COHERENCY_WRITE_THROUGH:
        case NVOS32_ATTR_COHERENCY_WRITE_PROTECT:
        case NVOS32_ATTR_COHERENCY_WRITE_BACK:
            cpuCacheAttrib = NV_MEMORY_CACHED;
            break;
        case NVOS32_ATTR_COHERENCY_UNCACHED:
            cpuCacheAttrib = NV_MEMORY_UNCACHED;
            break;
        case NVOS32_ATTR_COHERENCY_WRITE_COMBINE:
            // Intentional fall-through
        default:
            cpuCacheAttrib = NV_MEMORY_WRITECOMBINED;
            break;
    }

    status = memdescCreate(&pMemory->pMemDesc, NULL, pAllocParams->size, 0, NV_MEMORY_CONTIGUOUS,
                           ADDR_SYSMEM, cpuCacheAttrib, MEMDESC_FLAGS_CPU_ONLY);
    if (status != NV_OK)
        return status;

    status = osAllocPages(pMemory->pMemDesc);
    if (status != NV_OK)
        goto cleanup;
    pMemDesc = pMemory->pMemDesc;
    pMemDesc->Allocated = 1;

    protection = DRF_VAL(OS32, _ATTR2, _PROTECTION_USER, pAllocParams->attr2);
    attr2 = FLD_SET_DRF_NUM(OS32, _ATTR2, _PROTECTION_USER, protection, attr2);
    if (protection == NVOS32_ATTR2_PROTECTION_USER_READ_ONLY)
        memdescSetFlag(pMemDesc, MEMDESC_FLAGS_USER_READ_ONLY, NV_TRUE);

    // initialize the memory description
    pMemory->categoryClassId = pCallContext->pResourceRef->externalClassId;
    pMemory->pMemDesc   = pMemDesc;
    pMemory->Length     = pMemDesc->Size;
    pMemory->RefCount   = 1;
    pMemory->HeapOwner  = 0;
    pMemory->pHeap      = NULL;
    pMemory->Attr       = attr;
    pMemory->Attr2      = attr2;
    pMemory->Flags      = pAllocParams->flags;
    pMemory->isMemDescOwner = NV_TRUE;

    // Initialize the circular list item for tracking dup/sharing of pMemDesc
    pMemory->dupListItem.pNext = pMemory->dupListItem.pPrev = pMemory;

    return NV_OK;

cleanup:
    nodevicememDestruct_IMPL(pNoDeviceMemory);
    return status;
}

void nodevicememDestruct_IMPL(NoDeviceMemory *pNoDeviceMemory)
{
    Memory *pMemory = staticCast(pNoDeviceMemory, Memory);

    if (pMemory->KernelVAddr != NvP64_NULL)
    {
        memdescUnmap(pMemory->pMemDesc, NV_TRUE, osGetCurrentProcess(),
                     pMemory->KernelVAddr, pMemory->KernelMapPriv);
        pMemory->KernelVAddr = NvP64_NULL;
        pMemory->KernelMapPriv = NvP64_NULL;
    }

    if (pMemory->pMemDesc)
    {
        memdescFree(pMemory->pMemDesc);
        memdescDestroy(pMemory->pMemDesc);
    }
}

NV_STATUS nodevicememGetMapAddrSpace_IMPL
(
    NoDeviceMemory *pNoDeviceMemory,
    CALL_CONTEXT *pCallContext,
    NvU32 mapFlags,
    NV_ADDRESS_SPACE *pAddrSpace
)
{
    *pAddrSpace = ADDR_SYSMEM;
    return NV_OK;
}
