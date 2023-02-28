/*
 * SPDX-FileCopyrightText: Copyright (c) 2010-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#define NVOC_KERN_GMMU_H_PRIVATE_ACCESS_ALLOWED

#include "core/core.h"
#include "os/os.h"
#include "gpu/mmu/kern_gmmu.h"
#include "gpu/mmu/mmu_fault_buffer.h"
#include "gpu/device/device.h"
#include "rmapi/client.h"

NV_STATUS
faultbufConstruct_IMPL
(
    MmuFaultBuffer *pMmuFaultBuffer,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    NV_STATUS   status;
    OBJGPU     *pGpu = GPU_RES_GET_GPU(pMmuFaultBuffer);
    KernelGmmu *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
    NvHandle    hFaultBufferClient = pCallContext->pClient->hClient;
    NvHandle    hFaultBufferObject = pCallContext->pResourceRef->hResource;
    RmClient   *pRmClient = dynamicCast(pCallContext->pClient, RmClient);

    NV_ASSERT_OR_RETURN(pRmClient != NULL, NV_ERR_INVALID_CLIENT);

    if (!gpuIsClassSupported(pGpu, pParams->externalClassId))
    {
        NV_PRINTF(LEVEL_ERROR, "class %x not supported\n",
                  pParams->externalClassId);
        return NV_ERR_INVALID_CLASS;
    }

    NV_ASSERT_OR_RETURN(pKernelGmmu != NULL, NV_ERR_NOT_SUPPORTED);

    status = kgmmuFaultBufferReplayableAllocate(pGpu, pKernelGmmu,
                                                    hFaultBufferClient,
                                                    hFaultBufferObject);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to setup Replayable Fault buffer (status=0x%08x).\n",
                  status);
        return status;
    }

    return NV_OK;
}

void
faultbufDestruct_IMPL
(
    MmuFaultBuffer *pMmuFaultBuffer
)
{
    OBJGPU     *pGpu  = GPU_RES_GET_GPU(pMmuFaultBuffer);
    KernelGmmu *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);

    (void) kgmmuFaultBufferReplayableDestroy(pGpu, pKernelGmmu);
}

NV_STATUS
faultbufMap_IMPL
(
    MmuFaultBuffer *pMmuFaultBuffer,
    CALL_CONTEXT *pCallContext,
    RS_CPU_MAP_PARAMS *pParams,
    RsCpuMapping *pCpuMapping
)
{
    // This only maps the replayable fault buffer
    RmClient               *pClient = dynamicCast(pCallContext->pClient, RmClient);
    OBJGPU                 *pGpu;
    KernelGmmu             *pKernelGmmu;
    NV_STATUS               rmStatus = NV_OK;
    NvBool                  bBroadcast = NV_TRUE;
    NvBool                  bKernel;

    pGpu = CliGetGpuFromContext(pCpuMapping->pContextRef, &bBroadcast);
    NV_ASSERT_OR_RETURN(pGpu != NULL, NV_ERR_INVALID_ARGUMENT);
    gpuSetThreadBcState(pGpu, bBroadcast);

    pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);

    NV_ASSERT_OR_RETURN(pKernelGmmu != NULL, NV_ERR_INVALID_ARGUMENT);

    rmStatus = rmapiValidateKernelMapping(rmclientGetCachedPrivilege(pClient),
                                          pCpuMapping->flags,
                                          &bKernel);
    if (rmStatus != NV_OK)
        return rmStatus;

    pCpuMapping->processId = osGetCurrentProcess();

    // Map entire buffer (no offsets supported)
    rmStatus = memdescMap(pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].hwFaultBuffers[REPLAYABLE_FAULT_BUFFER].pFaultBufferMemDesc,
                          0,
                          pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].hwFaultBuffers[REPLAYABLE_FAULT_BUFFER].pFaultBufferMemDesc->Size,
                          bKernel,
                          pCpuMapping->pPrivate->protect,
                          &pCpuMapping->pLinearAddress,
                          &pCpuMapping->pPrivate->pPriv);
    pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].hwFaultBuffers[REPLAYABLE_FAULT_BUFFER].hCpuFaultBuffer = pCpuMapping->pPrivate->pPriv;

    return rmStatus;
}

NV_STATUS
faultbufUnmap_IMPL
(
    MmuFaultBuffer *pMmuFaultBuffer,
    CALL_CONTEXT *pCallContext,
    RsCpuMapping *pCpuMapping
)
{
    // This only unmaps the replayable fault buffer

    NV_STATUS               rmStatus;
    RmClient               *pClient = dynamicCast(pCallContext->pClient, RmClient);
    OBJGPU                 *pGpu;
    KernelGmmu             *pKernelGmmu;
    NvBool                  bBroadcast = NV_TRUE;
    NvBool                  bKernel;

    pGpu = CliGetGpuFromContext(pCpuMapping->pContextRef, &bBroadcast);
    NV_ASSERT_OR_RETURN(pGpu != NULL, NV_ERR_INVALID_ARGUMENT);
    gpuSetThreadBcState(pGpu, bBroadcast);

    pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
    NV_ASSERT_OR_RETURN(pKernelGmmu != NULL, NV_ERR_INVALID_ARGUMENT);

    rmStatus = rmapiValidateKernelMapping(rmclientGetCachedPrivilege(pClient),
                                          pCpuMapping->flags,
                                          &bKernel);
    if (rmStatus != NV_OK)
        return rmStatus;

    // Unmap it
    memdescUnmap(pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].hwFaultBuffers[REPLAYABLE_FAULT_BUFFER].pFaultBufferMemDesc,
                 bKernel,
                 pCpuMapping->processId,
                 pCpuMapping->pLinearAddress,
                 pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].hwFaultBuffers[REPLAYABLE_FAULT_BUFFER].hCpuFaultBuffer);

    return NV_OK;
}

NV_STATUS
faultbufGetMapAddrSpace_IMPL
(
    MmuFaultBuffer *pMmuFaultBuffer,
    CALL_CONTEXT *pCallContext,
    NvU32 mapFlags,
    NV_ADDRESS_SPACE *pAddrSpace
)
{
    OBJGPU  *pGpu = GPU_RES_GET_GPU(pMmuFaultBuffer);
    KernelGmmu *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
    NV_ADDRESS_SPACE addrSpace;
    PMEMORY_DESCRIPTOR pMemDesc;

    pMemDesc = pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].hwFaultBuffers[REPLAYABLE_FAULT_BUFFER].pFaultBufferMemDesc;
    if (pMemDesc == NULL)
        return NV_ERR_INVALID_OBJECT;

    NV_ASSERT_OK_OR_RETURN(rmapiGetEffectiveAddrSpace(pGpu, pMemDesc, mapFlags, &addrSpace));
    if (pAddrSpace)
        *pAddrSpace = addrSpace;

    return NV_OK;
}

