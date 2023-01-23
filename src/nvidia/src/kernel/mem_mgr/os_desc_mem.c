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
#include "mem_mgr/os_desc_mem.h"
#include "rmapi/client.h"
#include "rmapi/mapping_list.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "os/os.h"
#include "gpu/device/device.h"
#include "vgpu/rpc.h"
#include "mem_mgr/mem.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "deprecated/rmapi_deprecated.h"

#include "class/cl0071.h" // NV01_MEMORY_SYSTEM_OS_DESCRIPTOR

NV_STATUS
osdescConstruct_IMPL
(
    OsDescMemory                 *pOsDescMemory,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pRmAllocParams
)
{
    Memory            *pMemory = staticCast(pOsDescMemory, Memory);
    NV_OS_DESC_MEMORY_ALLOCATION_PARAMS *pUserParams;
    OBJGPU            *pGpu = pMemory->pGpu;
    MemoryManager     *pMemoryManager    = GPU_GET_MEMORY_MANAGER(pGpu);
    MEMORY_DESCRIPTOR *pMemDesc = NULL;
    NV_STATUS          status;
    NvU64              limit;
    NvU32              os02Flags;
    NvHandle           hClient = pCallContext->pClient->hClient;
    NvHandle           hParent = pCallContext->pResourceRef->pParentRef->hResource;
    NvHandle           hMemory = pCallContext->pResourceRef->hResource;

    // Copy-construction has already been done by the base Memory class
    if (RS_IS_COPY_CTOR(pRmAllocParams))
        return NV_OK;

    pUserParams = pRmAllocParams->pAllocParams;

    limit = pUserParams->limit;

    //
    // Bug 860684: osCreateMemFromOsDescriptor expects OS02 flags
    // from the old NvRmAllocMemory64() interface so we need to
    // translate the OS32_ATTR flags to OS02 flags.
    //
    status = RmDeprecatedConvertOs32ToOs02Flags(pUserParams->attr,
                                                pUserParams->attr2,
                                                pUserParams->flags,
                                                &os02Flags);

    if (status != NV_OK)
    {
        return status;
    }

    // Only kernel user is allowed to register physical address with RM
    if (pUserParams->descriptorType == NVOS32_DESCRIPTOR_TYPE_OS_PHYS_ADDR)
    {
        if (pCallContext->secInfo.privLevel < RS_PRIV_LEVEL_KERNEL)
        {
            return NV_ERR_NOT_SUPPORTED;
        }
    }

    if (pUserParams->descriptorType == NVOS32_DESCRIPTOR_TYPE_OS_IO_MEMORY)
    {
        //
        // We currently allow RmMapMemory on external IO resources which are
        // safe to share across processes. For example, NpuResource.
        //
        // Otherwise we would be affected by the security issues like Bug 1630288.
        //
        os02Flags = FLD_SET_DRF(OS02, _FLAGS, _MAPPING, _NEVER_MAP, os02Flags);

        //
        // Force peerMappingOverride check for IO memory registration through
        // RmVidHeapCtrl. See Bug 1630288 "[PeerSync] threat related to GPU.." for
        // more details.
        //
        os02Flags = FLD_SET_DRF(OS02, _FLAGS, _PEER_MAP_OVERRIDE, _REQUIRED, os02Flags);
    }

    //
    // Create and fill in the memory descriptor based on the current
    // state of the OS descriptor.
    //
    status = osCreateMemFromOsDescriptor(pGpu,
                                         pUserParams->descriptor,
                                         hClient,
                                         os02Flags,
                                         &limit,
                                         &pMemDesc,
                                         pUserParams->descriptorType,
                                         pRmAllocParams->pSecInfo->privLevel);

    if (status != NV_OK)
    {
        return status;
    }

    if (pMemoryManager->bAllowSysmemHugePages && pMemDesc->bForceHugePages)
    {
        pUserParams->attr = DRF_DEF(OS32, _ATTR, _PAGE_SIZE, _HUGE);
        pUserParams->attr2 = DRF_DEF(OS32, _ATTR2, _PAGE_SIZE_HUGE, _DEFAULT);
    }

    status = memConstructCommon(pMemory, NV01_MEMORY_SYSTEM_OS_DESCRIPTOR, pUserParams->flags,
                                pMemDesc, 0, NULL, pUserParams->attr, pUserParams->attr2, 0, 0,
                                pUserParams->tag, (HWRESOURCE_INFO *)NULL);

    if (status == NV_OK)
    {
        RsResourceRef *pResourceRef = RES_GET_REF(pMemory);
        RsCpuMapping   *pCpuMapping = NULL;
        NvU32 flags = 0;
        flags = FLD_SET_DRF(OS33, _FLAGS, _OS_DESCRIPTOR, _ENABLE, flags);
        RS_CPU_MAP_PARAMS dummyParams;
        portMemSet(&dummyParams, 0, sizeof(dummyParams));

        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            refAddMapping(pResourceRef, &dummyParams, pResourceRef->pParentRef, &pCpuMapping));

        NV_ASSERT_OK_OR_RETURN(CliUpdateMemoryMappingInfo(pCpuMapping,
                                                          pCallContext->secInfo.privLevel >= RS_PRIV_LEVEL_KERNEL,
                                                          pUserParams->descriptor, NvP64_NULL,
                                                          limit+1, flags));
        pCpuMapping->pPrivate->pGpu = pGpu;

        //
        // vGPU:
        //
        // Since vGPU does all real hardware management in the
        // host, if we are in guest OS (where IS_VIRTUAL(pGpu) is true),
        // do an RPC to the host to do the hardware update.
        //
        if (IS_VIRTUAL(pGpu))
        {
            NV_RM_RPC_ALLOC_MEMORY(pGpu,
                                   hClient,
                                   hParent,
                                   hMemory,
                                   NV01_MEMORY_SYSTEM_OS_DESCRIPTOR,
                                   os02Flags,
                                   pMemDesc,
                                   status);
            if (status == NV_OK)
                pMemory->bRpcAlloc = NV_TRUE;

        }
    }

    //
    // RM support for MODS PTE kind in external allocations
    // bug 1858656
    //

    // failure case
    if (status != NV_OK)
    {
        memdescFree(pMemDesc);
        memdescDestroy(pMemDesc);
    }

    return status;
}

NvBool
osdescCanCopy_IMPL
(
    OsDescMemory *pOsDescMemory
)
{
    return RMCFG_FEATURE_PLATFORM_UNIX;
}
