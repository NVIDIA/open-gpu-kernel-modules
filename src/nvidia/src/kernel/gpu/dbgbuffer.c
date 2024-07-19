/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "os/os.h"
#include "gpu/dbgbuffer.h"
#include "rmapi/client.h"

NV_STATUS
dbgbufConstruct_IMPL
(
    DebugBufferApi *pDebugBufferApi,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    NV_STATUS                     status       = NV_OK;
    OBJGPU                       *pGpu         = GPU_RES_GET_GPU(pDebugBufferApi);
    NvDebugDump                  *pNvd         = GPU_GET_NVD(pGpu);
    MEMORY_DESCRIPTOR            *pMemDesc     = NULL;
    NV00DB_ALLOCATION_PARAMETERS *pUserParams = pParams->pAllocParams;

    // Allocate a memory descriptor and backing memory for this historical buffer
    status = nvdAllocDebugBuffer(pGpu, pNvd, pUserParams->tag, &pUserParams->size, &pMemDesc);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "DebugBuffer object could not be allocated.\n");
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }
    pDebugBufferApi->pMemDesc = pMemDesc;

    return status;
}

void
dbgbufDestruct_IMPL
(
    DebugBufferApi *pDebugBufferApi
)
{
    NV_STATUS                    status;
    CALL_CONTEXT                *pCallContext;
    RS_RES_FREE_PARAMS_INTERNAL *pParams;
    OBJGPU                      *pGpu = GPU_RES_GET_GPU(pDebugBufferApi);
    NvDebugDump                 *pNvd = GPU_GET_NVD(pGpu);

    resGetFreeParams(staticCast(pDebugBufferApi, RsResource), &pCallContext, &pParams);

    // Unlink and free historical buffer
    status = nvdFreeDebugBuffer(pGpu, pNvd, pDebugBufferApi->pMemDesc);
    NV_ASSERT(status == NV_OK);

    pParams->status = status;
}

NV_STATUS
dbgbufMap_IMPL
(
    DebugBufferApi *pDebugBufferApi,
    CALL_CONTEXT *pCallContext,
    RS_CPU_MAP_PARAMS *pParams,
    RsCpuMapping *pCpuMapping
)
{
    NV_STATUS status;
    NvBool bKernel;
    RmClient *pClient = dynamicCast(pCallContext->pClient, RmClient);

    status = rmapiValidateKernelMapping(rmclientGetCachedPrivilege(pClient),
                                        pCpuMapping->flags,
                                        &bKernel);
    if (status != NV_OK)
        return status;

    pCpuMapping->processId = osGetCurrentProcess();

    // Map entire buffer (no offsets supported)
    return memdescMap(pDebugBufferApi->pMemDesc,
                      0,
                      pDebugBufferApi->pMemDesc->Size,
                      bKernel,
                      pCpuMapping->pPrivate->protect,
                      &pCpuMapping->pLinearAddress,
                      &pCpuMapping->pPrivate->pPriv);
}

NV_STATUS
dbgbufUnmap_IMPL
(
    DebugBufferApi *pDebugBufferApi,
    CALL_CONTEXT *pCallContext,
    RsCpuMapping *pCpuMapping
)
{
    NV_STATUS status;
    NvBool bKernel;
    RmClient *pClient = dynamicCast(pCallContext->pClient, RmClient);

    status = rmapiValidateKernelMapping(rmclientGetCachedPrivilege(pClient),
                                        pCpuMapping->flags,
                                        &bKernel);
    if (status != NV_OK)
        return status;

    memdescUnmap(pDebugBufferApi->pMemDesc,
                 bKernel,
                 pCpuMapping->processId,
                 pCpuMapping->pLinearAddress,
                 pCpuMapping->pPrivate->pPriv);

    return NV_OK;
}

NV_STATUS
dbgbufGetMapAddrSpace_IMPL
(
    DebugBufferApi *pDebugBufferApi,
    CALL_CONTEXT *pCallContext,
    NvU32 mapFlags,
    NV_ADDRESS_SPACE *pAddrSpace
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pDebugBufferApi);
    NV_ADDRESS_SPACE addrSpace;
    PMEMORY_DESCRIPTOR pMemDesc = pDebugBufferApi->pMemDesc;

    if (pMemDesc == NULL)
        return NV_ERR_INVALID_OBJECT;

    NV_ASSERT_OK_OR_RETURN(rmapiGetEffectiveAddrSpace(pGpu, pMemDesc, mapFlags, &addrSpace));

    if (pAddrSpace)
        *pAddrSpace = addrSpace;

    return NV_OK;
}

NV_STATUS
dbgbufGetMemoryMappingDescriptor_IMPL
(
    DebugBufferApi *pDebugBufferApi,
    MEMORY_DESCRIPTOR **ppMemDesc
)
{
    *ppMemDesc = pDebugBufferApi->pMemDesc;
    return NV_OK;
}
