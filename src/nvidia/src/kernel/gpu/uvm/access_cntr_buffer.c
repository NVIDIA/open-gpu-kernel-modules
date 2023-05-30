/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "os/os.h"

#include "gpu/uvm/uvm.h"
#include "gpu/uvm/access_cntr_buffer.h"
#include "gpu/device/device.h"
#include "kernel/rmapi/client.h"

#include "alloc/alloc_access_counter_buffer.h"

NV_STATUS
accesscntrConstruct_IMPL
(
    AccessCounterBuffer          *pAccessCounterBuffer,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    NV_ACCESS_COUNTER_NOTIFY_BUFFER_ALLOC_PARAMS *pAllocParams = pParams->pAllocParams;
    OBJGPU *pGpu = GPU_RES_GET_GPU(pAccessCounterBuffer);
    OBJUVM *pUvm = GPU_GET_UVM(pGpu);

    // Not supported on legacy guests and in case of sr-iov heavy
    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu) ||
        (IS_VIRTUAL(pGpu) && gpuIsWarBug200577889SriovHeavyEnabled(pGpu)))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    if (!gpuIsClassSupported(pGpu, pParams->externalClassId))
    {
        NV_PRINTF(LEVEL_ERROR, "class %x not supported\n",
                  pParams->externalClassId);
        return NV_ERR_INVALID_CLASS;
    }

    NV_ASSERT_OR_RETURN(pUvm != NULL, NV_ERR_NOT_SUPPORTED);

    pAccessCounterBuffer->accessCounterIndex = (pAllocParams != NULL) ? pAllocParams->accessCounterIndex : 0;

    NV_CHECK_OR_RETURN(LEVEL_ERROR,
        pAccessCounterBuffer->accessCounterIndex < pUvm->accessCounterBufferCount,
        NV_ERR_INVALID_ARGUMENT);
    NV_CHECK_OR_RETURN(LEVEL_ERROR,
        pUvm->pAccessCounterBuffers[pAccessCounterBuffer->accessCounterIndex].pAccessCounterBuffer == NULL,
        NV_ERR_INVALID_STATE);

    NV_ASSERT_OK_OR_RETURN(uvmInitializeAccessCntrBuffer(pGpu, pUvm, pAccessCounterBuffer));
    pUvm->pAccessCounterBuffers[pAccessCounterBuffer->accessCounterIndex].pAccessCounterBuffer = pAccessCounterBuffer;

    return NV_OK;
}

void
accesscntrDestruct_IMPL
(
    AccessCounterBuffer *pAccessCounterBuffer
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pAccessCounterBuffer);
    OBJUVM *pUvm = GPU_GET_UVM(pGpu);

    NV_ASSERT(pUvm->pAccessCounterBuffers[pAccessCounterBuffer->accessCounterIndex].pAccessCounterBuffer == pAccessCounterBuffer);

    NV_ASSERT_OK(uvmTerminateAccessCntrBuffer(pGpu, pUvm, pAccessCounterBuffer));

    pUvm->pAccessCounterBuffers[pAccessCounterBuffer->accessCounterIndex].pAccessCounterBuffer = NULL;
}

NV_STATUS
accesscntrMap_IMPL
(
    AccessCounterBuffer *pAccessCounterBuffer,
    CALL_CONTEXT *pCallContext,
    RS_CPU_MAP_PARAMS *pParams,
    RsCpuMapping *pCpuMapping
)
{
    RmClient               *pClient = dynamicCast(pCallContext->pClient, RmClient);
    OBJGPU                 *pGpu;
    NV_STATUS               rmStatus = NV_OK;
    NvBool                  bBroadcast = NV_TRUE;
    NvBool                  bKernel;

    pGpu = CliGetGpuFromContext(pCpuMapping->pContextRef, &bBroadcast);
    NV_ASSERT_OR_RETURN(pGpu != NULL, NV_ERR_INVALID_ARGUMENT);
    gpuSetThreadBcState(pGpu, bBroadcast);

    rmStatus = rmapiValidateKernelMapping(rmclientGetCachedPrivilege(pClient),
                                          pCpuMapping->flags,
                                          &bKernel);
    if (rmStatus != NV_OK)
        return rmStatus;

    pCpuMapping->processId = osGetCurrentProcess();

    rmStatus = memdescMap(pAccessCounterBuffer->pUvmAccessCntrAllocMemDesc,
                          0,
                          pAccessCounterBuffer->pUvmAccessCntrAllocMemDesc->Size,
                          bKernel,
                          pCpuMapping->pPrivate->protect,
                          &pCpuMapping->pLinearAddress,
                          &pCpuMapping->pPrivate->pPriv);

    return rmStatus;
}

NV_STATUS
accesscntrUnmap_IMPL
(
    AccessCounterBuffer *pAccessCounterBuffer,
    CALL_CONTEXT *pCallContext,
    RsCpuMapping *pCpuMapping
)
{
    NV_STATUS               rmStatus;
    RmClient               *pClient = dynamicCast(pCallContext->pClient, RmClient);
    OBJGPU                 *pGpu;
    NvBool                  bBroadcast = NV_TRUE;
    NvBool                  bKernel;

    pGpu = CliGetGpuFromContext(pCpuMapping->pContextRef, &bBroadcast);
    NV_ASSERT_OR_RETURN(pGpu != NULL, NV_ERR_INVALID_ARGUMENT);
    gpuSetThreadBcState(pGpu, bBroadcast);

    rmStatus = rmapiValidateKernelMapping(rmclientGetCachedPrivilege(pClient),
                                          pCpuMapping->flags,
                                          &bKernel);
    if (rmStatus != NV_OK)
        return rmStatus;

    memdescUnmap(pAccessCounterBuffer->pUvmAccessCntrAllocMemDesc,
                 bKernel,
                 pCpuMapping->processId,
                 pCpuMapping->pLinearAddress,
                 pCpuMapping->pPrivate->pPriv);

    return NV_OK;
}

NV_STATUS
accesscntrGetMapAddrSpace_IMPL
(
    AccessCounterBuffer *pAccessCounterBuffer,
    CALL_CONTEXT *pCallContext,
    NvU32 mapFlags,
    NV_ADDRESS_SPACE *pAddrSpace
)
{
    NV_ADDRESS_SPACE addrSpace;
    OBJGPU *pGpu = GPU_RES_GET_GPU(pAccessCounterBuffer);
    PMEMORY_DESCRIPTOR pMemDesc = pAccessCounterBuffer->pUvmAccessCntrAllocMemDesc;

    if (pMemDesc == NULL)
        return NV_ERR_INVALID_OBJECT;

    NV_ASSERT_OK_OR_RETURN(rmapiGetEffectiveAddrSpace(pGpu, pMemDesc, mapFlags, &addrSpace));

    if (pAddrSpace)
        *pAddrSpace = addrSpace;

    return NV_OK;
}

