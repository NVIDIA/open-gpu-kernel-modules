/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "mem_mgr/virt_mem_range.h"
#include "os/os.h"
#include "vgpu/rpc.h"
#include "gpu/mem_mgr/vaspace_api.h"
#include "gpu/mmu/kern_gmmu.h"
#include "class/cl0070.h"

NV_STATUS
vmrangeConstruct_IMPL
(
    VirtualMemoryRange           *pVmRange,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    NV_MEMORY_VIRTUAL_ALLOCATION_PARAMS *pAllocData;
    NV_STATUS           status         = NV_OK;
    RsResourceRef      *pResourceRef   = pCallContext->pResourceRef;
    RsResourceRef      *pVASpaceRef    = NULL;
    NvHandle            hClient        = pCallContext->pClient->hClient;
    NvHandle            hParent        = pCallContext->pResourceRef->pParentRef->hResource;
    NvHandle            hMemory        = pCallContext->pResourceRef->hResource;
    NvU32               class          = pResourceRef->externalClassId;
    VirtualMemory      *pVirtualMemory = staticCast(pVmRange, VirtualMemory);
    Memory             *pMemory        = staticCast(pVmRange, Memory);
    OBJGPU             *pGpu           = pMemory->pGpu;
    KernelGmmu         *pKernelGmmu    = GPU_GET_KERNEL_GMMU(pGpu);
    MEMORY_DESCRIPTOR  *pMemDesc       = NULL;
    NvU64               maxVA;

    // Copy checking is handled by VirtualMemory class
    if (RS_IS_COPY_CTOR(pParams))
        return NV_OK;

    pAllocData = pParams->pAllocParams;

    //
    // Handling of hVASpace
    //   0 to find the default hVASpace in this context. This is compatible with
    //      the old virtual ContextDma class.
    //   NV_MEMORY_VIRTUAL_SYSMEM_DYNAMIC_HVASPACE (0xffffffff)
    //      An invalid handle to disallow mappings with this DynamicObject.
    //      This is used to emulate the original behavior for NV01_MEMORY_SYSTEM_DYNAMIC
    //      class.
    //   or an actual hVASpace handle
    //
    if (pAllocData->hVASpace == 0)
    {
        NV_ASSERT_OR_RETURN(kgmmuGetMaxVASize(pKernelGmmu), NV_ERR_INVALID_STATE);
        maxVA = kgmmuGetMaxVASize(pKernelGmmu);
    }
    else if (pAllocData->hVASpace == NV_MEMORY_VIRTUAL_SYSMEM_DYNAMIC_HVASPACE)
    {
        NV_ASSERT_OR_RETURN(kgmmuGetMaxVASize(pKernelGmmu), NV_ERR_INVALID_STATE);
        maxVA = 1ULL << 40;
    }
    else
    {
        VaSpaceApi *pVAS;

        status = clientGetResourceRef(pCallContext->pClient, pAllocData->hVASpace, &pVASpaceRef);
        NV_CHECK_OR_RETURN(LEVEL_INFO, status == NV_OK, status);
        pVAS = dynamicCast(pVASpaceRef->pResource, VaSpaceApi);
        NV_CHECK_OR_RETURN(LEVEL_INFO, pVAS, NV_ERR_INVALID_OBJECT_HANDLE);
        NV_ASSERT_OR_RETURN(pVAS->pVASpace, NV_ERR_INVALID_OBJECT_HANDLE);

        maxVA = pVAS->pVASpace->vasLimit + 1;
    }
    if (pAllocData->limit != 0)
    {
        NV_CHECK_OR_RETURN(LEVEL_INFO, pAllocData->limit < maxVA, NV_ERR_INVALID_LIMIT);
        maxVA = pAllocData->limit + 1;
    }

    NV_CHECK_OR_RETURN(LEVEL_INFO, pAllocData->offset < maxVA, NV_ERR_INVALID_OFFSET);

    status = memCreateMemDesc(pGpu, &pMemDesc, ADDR_VIRTUAL,
                              pAllocData->offset,
                              maxVA - pAllocData->offset,
                              0, 0);
    NV_CHECK_OR_RETURN(LEVEL_INFO, status == NV_OK, status);

    status = memConstructCommon(pMemory, class, 0, pMemDesc, 0, NULL,
                                0, 0, 0, 0, NVOS32_MEM_TAG_NONE, (HWRESOURCE_INFO *)NULL);
    if (status != NV_OK)
    {
        memdescDestroy(pMemDesc);
        return status;
    }

    // Allow unicast mapping on NV01_MEMORY_VIRTUAL object.
    pVirtualMemory->bAllowUnicastMapping = NV_TRUE;

    // Construct sets hVASpace to invalid, update with our handle
    pVirtualMemory->hVASpace = pAllocData->hVASpace;

    //
    // VGPU: With GUEST_MANAGED_VA we should not have to forward this object,
    // but virtual ContextDma and the memory destructor depend on it.
    //
    if (IS_VIRTUAL(pGpu) || IS_GSP_CLIENT(pGpu))
    {
        NV_RM_RPC_ALLOC_OBJECT(pGpu, hClient, hParent, hMemory, class,
            pAllocData, sizeof(*pAllocData), status);
        if (status != NV_OK)
        {
            memdescDestroy(pMemDesc);
            return status;
        }
        pMemory->bRpcAlloc = NV_TRUE;
    }

    pAllocData->limit = maxVA - 1;          // Return limit to client

    if (pVASpaceRef)
        refAddDependant(pVASpaceRef, pResourceRef);

    return status;
}
