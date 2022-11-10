/*
 * SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "gpu/gpu_user_shared_data.h"
#include "gpu/gpu.h"
#include "gpu/subdevice/subdevice.h"
#include "os/os.h"
#include "rmapi/client.h"
#include "rmapi/rmapi.h"
#include "class/cl00de.h"

NV_STATUS
gpushareddataConstruct_IMPL
(
    GpuUserSharedData *pData,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    NV_STATUS            status   = NV_OK;
    OBJGPU              *pGpu     = GPU_RES_GET_GPU(pData);
    MEMORY_DESCRIPTOR   *pMemDesc = NULL;
    NV00DE_SHARED_DATA  *pSharedData;

    if (pGpu->userSharedData.pMemDesc == NULL)
    {
        // Create a kernel-side mapping for writing the data if one is not already present
        status = memdescCreate(&pMemDesc, pGpu, sizeof(NV00DE_SHARED_DATA), 0, NV_TRUE, ADDR_SYSMEM,
                               NV_MEMORY_WRITECOMBINED, MEMDESC_FLAGS_USER_READ_ONLY);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "memdescCreate failed - status=0x%08x\n", status);
            return NV_ERR_INSUFFICIENT_RESOURCES;
        }

        status = memdescAlloc(pMemDesc);
        if (status != NV_OK)
        {
            memdescDestroy(pMemDesc);
            NV_PRINTF(LEVEL_ERROR, "memdescAlloc failed - status=0x%08x\n", status);
            return status;
        }

        status = memdescMap(pMemDesc, 0, pMemDesc->Size,
                            NV_TRUE, NV_PROTECT_READABLE,
                            &pGpu->userSharedData.pMapBuffer,
                            &pGpu->userSharedData.pMapBufferPriv);
        if (status != NV_OK)
        {
            memdescFree(pMemDesc);
            memdescDestroy(pMemDesc);
            NV_PRINTF(LEVEL_ERROR, "memdescMap failed - status=0x%08x\n", status);
            return status;
        }

        pGpu->userSharedData.pMemDesc = pMemDesc;

        pSharedData = (NV00DE_SHARED_DATA*)(pGpu->userSharedData.pMapBuffer);
        portMemSet(pSharedData, 0, sizeof(*pSharedData));

        // Initial write from cached data
        gpushareddataWriteFinish(pGpu);
    }

    memdescAddRef(pGpu->userSharedData.pMemDesc);

    return NV_OK;
}

void
gpushareddataDestruct_IMPL
(
    GpuUserSharedData *pData
)
{
    OBJGPU             *pGpu = GPU_RES_GET_GPU(pData);
    MEMORY_DESCRIPTOR  *pMemDesc = pGpu->userSharedData.pMemDesc;

    memdescRemoveRef(pMemDesc);

    if (pMemDesc->RefCount == 1)
    {
        // Clean up kernel-side mapping if this is the last mapping for this GPU
        memdescUnmap(pMemDesc,
                     NV_TRUE,
                     pGpu->userSharedData.processId,
                     pGpu->userSharedData.pMapBuffer,
                     pGpu->userSharedData.pMapBufferPriv);

        memdescFree(pMemDesc);
        memdescDestroy(pMemDesc);
        pGpu->userSharedData.pMemDesc = NULL;
        pGpu->userSharedData.pMapBuffer = NULL;
        pGpu->userSharedData.pMapBufferPriv = NULL;
    }
}

NV_STATUS
gpushareddataMap_IMPL
(
    GpuUserSharedData *pData,
    CALL_CONTEXT *pCallContext,
    RS_CPU_MAP_PARAMS *pParams,
    RsCpuMapping *pCpuMapping
)
{
    NV_STATUS  status;
    NvBool     bKernel;
    RmClient  *pClient = dynamicCast(pCallContext->pClient, RmClient);
    OBJGPU    *pGpu = GPU_RES_GET_GPU(pData);

    status = rmapiValidateKernelMapping(rmclientGetCachedPrivilege(pClient),
                                        pCpuMapping->flags,
                                        &bKernel);
    if (status != NV_OK)
        return status;

    // Only support read-only, fail early if writeable
    if (pCpuMapping->pPrivate->protect & NV_PROTECT_WRITEABLE)
        return NV_ERR_INVALID_PARAMETER;

    pCpuMapping->processId = osGetCurrentProcess();

    // Map entire buffer (no offsets supported)
    status = memdescMap(pGpu->userSharedData.pMemDesc,
                        0,
                        pGpu->userSharedData.pMemDesc->Size,
                        NV_FALSE,
                        pCpuMapping->pPrivate->protect,
                        &pCpuMapping->pLinearAddress,
                        &pCpuMapping->pPrivate->pPriv);

    return status;
}

NV_STATUS
gpushareddataUnmap_IMPL
(
    GpuUserSharedData *pData,
    CALL_CONTEXT *pCallContext,
    RsCpuMapping *pCpuMapping
)
{
    NV_STATUS  status;
    NvBool     bKernel;
    RmClient  *pClient = dynamicCast(pCallContext->pClient, RmClient);
    OBJGPU    *pGpu = GPU_RES_GET_GPU(pData);

    if (pGpu->userSharedData.pMemDesc == NULL)
        return NV_ERR_INVALID_OBJECT;

    status = rmapiValidateKernelMapping(rmclientGetCachedPrivilege(pClient),
                                        pCpuMapping->flags,
                                        &bKernel);
    if (status != NV_OK)
        return status;

    memdescUnmap(pGpu->userSharedData.pMemDesc,
                 bKernel,
                 pCpuMapping->processId,
                 pCpuMapping->pLinearAddress,
                 pCpuMapping->pPrivate->pPriv);

    return NV_OK;
}

NV_STATUS
gpushareddataGetMapAddrSpace_IMPL
(
    GpuUserSharedData *pData,
    CALL_CONTEXT *pCallContext,
    NvU32 mapFlags,
    NV_ADDRESS_SPACE *pAddrSpace
)
{
    OBJGPU           *pGpu = GPU_RES_GET_GPU(pData);
    NV_ADDRESS_SPACE  addrSpace;

    if (pGpu->userSharedData.pMemDesc == NULL)
        return NV_ERR_INVALID_OBJECT;

    NV_ASSERT_OK_OR_RETURN(rmapiGetEffectiveAddrSpace(pGpu, pGpu->userSharedData.pMemDesc, mapFlags, &addrSpace));

    if (pAddrSpace != NULL)
        *pAddrSpace = addrSpace;

    return NV_OK;
}

NV_STATUS
gpushareddataGetMemoryMappingDescriptor_IMPL
(
    GpuUserSharedData *pData,
    MEMORY_DESCRIPTOR **ppMemDesc
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pData);

    *ppMemDesc = pGpu->userSharedData.pMemDesc;

    return NV_OK;
}

NV00DE_SHARED_DATA * gpushareddataWriteStart(OBJGPU *pGpu)
{
    return &pGpu->userSharedData.data;
}

void gpushareddataWriteFinish(OBJGPU *pGpu)
{
    NV00DE_SHARED_DATA *pSharedData = (NV00DE_SHARED_DATA*)(pGpu->userSharedData.pMapBuffer);
    const NvU32 data_offset = sizeof(pSharedData->seq);
    const NvU32 data_size = sizeof(NV00DE_SHARED_DATA) - data_offset;

    if (pSharedData == NULL)
        return;

    portAtomicIncrementU32(&pSharedData->seq);
    portAtomicMemoryFenceStore();

    // Push cached data to mapped buffer
    portMemCopy((NvU8*)pSharedData + data_offset, data_size,
                (NvU8*)&pGpu->userSharedData.data + data_offset, data_size);

    portAtomicMemoryFenceStore();
    portAtomicIncrementU32(&pSharedData->seq);
}
