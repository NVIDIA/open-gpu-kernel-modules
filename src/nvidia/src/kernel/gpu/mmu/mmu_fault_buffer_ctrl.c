/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "gpu/mmu/kern_gmmu.h"
#include "rmapi/control.h"

#include "ctrl/ctrlc369.h"
#include "gpu/mmu/mmu_fault_buffer.h"

NV_STATUS
faultbufCtrlCmdMmuFaultBufferRegisterNonReplayBuf_IMPL
(
    MmuFaultBuffer *pMmuFaultBuffer,
    NVC369_CTRL_MMU_FAULT_BUFFER_REGISTER_NON_REPLAY_BUF_PARAMS *pParams
)
{
    NV_STATUS status;
    OBJGPU   *pGpu    = GPU_RES_GET_GPU(pMmuFaultBuffer);
    KernelGmmu *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
    const NV2080_CTRL_INTERNAL_GMMU_GET_STATIC_INFO_PARAMS *pStaticInfo = kgmmuGetStaticInfo(pGpu, pKernelGmmu);
    GMMU_CLIENT_SHADOW_FAULT_BUFFER *pClientShadowFaultBuffer;

    pClientShadowFaultBuffer =
        pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].pClientShadowFaultBuffer[NON_REPLAYABLE_FAULT_BUFFER];

    // This call takes GpuLock, so shadowBuffer pointers can be accessed without grabbing lock protecting them
    if (pClientShadowFaultBuffer)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Client shadow fault buffer for non-replayable faults already allocated\n");
        NV_ASSERT(0);

        return NV_ERR_NOT_SUPPORTED;
    }

    status = kgmmuClientShadowFaultBufferAlloc_HAL(pGpu, pKernelGmmu, NON_REPLAYABLE_FAULT_BUFFER);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Error allocating client shadow fault buffer for non-replayable faults\n");

        return status;
    }

    pClientShadowFaultBuffer =
        pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].pClientShadowFaultBuffer[NON_REPLAYABLE_FAULT_BUFFER];

    if (gpuIsCCFeatureEnabled(pGpu) && gpuIsGspOwnedFaultBuffersEnabled(pGpu))
    {
        pParams->pShadowBuffer = pClientShadowFaultBuffer->pBufferAddress;
        pParams->pShadowBufferContext = NvP64_NULL;
        pParams->pShadowBufferMetadata = pClientShadowFaultBuffer->pFaultBufferMetadataAddress;
    }
    else
    {
        pParams->pShadowBuffer = pClientShadowFaultBuffer->pQueueAddress;
        pParams->pShadowBufferContext = (NvP64) &pClientShadowFaultBuffer->queueContext;
        pParams->pShadowBufferMetadata = NvP64_NULL;
    }
    pParams->bufferSize = pStaticInfo->nonReplayableFaultBufferSize;

    return NV_OK;
}

NV_STATUS
faultbufCtrlCmdMmuFaultBufferUnregisterNonReplayBuf_IMPL
(
    MmuFaultBuffer *pMmuFaultBuffer,
    NVC369_CTRL_MMU_FAULT_BUFFER_UNREGISTER_NON_REPLAY_BUF_PARAMS *pParams
)
{
    NV_STATUS status;
    OBJGPU   *pGpu  = GPU_RES_GET_GPU(pMmuFaultBuffer);
    KernelGmmu *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);

    // This call takes GpuLock, so shadowBuffer pointers can be accessed without grabbing lock protecting them
    if (!pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].pClientShadowFaultBuffer[NON_REPLAYABLE_FAULT_BUFFER])
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Client shadow fault buffer for non-replayable faults does not exist\n");
        return NV_OK;
    }

    if ((pParams->pShadowBuffer !=
        pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].pClientShadowFaultBuffer[NON_REPLAYABLE_FAULT_BUFFER]->pQueueAddress)
        && (!gpuIsCCFeatureEnabled(pGpu) || !gpuIsGspOwnedFaultBuffersEnabled(pGpu))
       )
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Given client shadow fault buffer for non-replayable faults does not "
                  "match with the actual\n");
    }
    else if (gpuIsCCFeatureEnabled(pGpu) && gpuIsGspOwnedFaultBuffersEnabled(pGpu) &&
            (pParams->pShadowBuffer !=
             pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].pClientShadowFaultBuffer[NON_REPLAYABLE_FAULT_BUFFER]->pBufferAddress))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Given client shadow fault buffer for non-replayable faults does not "
                  "match with the actual\n");
    }

    status = kgmmuClientShadowFaultBufferFree_HAL(pGpu, pKernelGmmu, NON_REPLAYABLE_FAULT_BUFFER);
    if (status != NV_OK) {
        NV_PRINTF(LEVEL_ERROR,
                  "Error freeing client shadow fault buffer for non-replayable faults\n");
    }

    return status;
}

NV_STATUS
faultbufCtrlCmdMmuFaultBufferRegisterReplayBuf_IMPL
(
    MmuFaultBuffer *pMmuFaultBuffer,
    NVC369_CTRL_MMU_FAULT_BUFFER_REGISTER_REPLAY_BUF_PARAMS *pParams
)
{
    NV_STATUS status;
    OBJGPU *pGpu = GPU_RES_GET_GPU(pMmuFaultBuffer);
    KernelGmmu *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
    const NV2080_CTRL_INTERNAL_GMMU_GET_STATIC_INFO_PARAMS *pStaticInfo = kgmmuGetStaticInfo(pGpu, pKernelGmmu);
    GMMU_CLIENT_SHADOW_FAULT_BUFFER *pClientShadowFaultBuffer;

    NV_ASSERT_OR_RETURN(gpuIsCCFeatureEnabled(pGpu), NV_ERR_NOT_SUPPORTED);

    pClientShadowFaultBuffer =
        pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].pClientShadowFaultBuffer[REPLAYABLE_FAULT_BUFFER];

    // This call takes GpuLock, so shadowBuffer pointers can be accessed without grabbing lock protecting them
    if (pClientShadowFaultBuffer != NULL)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Client shadow fault buffer for replayable faults already allocated\n");
        NV_ASSERT(0);

        return NV_ERR_NOT_SUPPORTED;
    }

    status = kgmmuClientShadowFaultBufferAlloc_HAL(pGpu, pKernelGmmu, REPLAYABLE_FAULT_BUFFER);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Error allocating client shadow fault buffer for replayable faults\n");
        return status;
    }

    pClientShadowFaultBuffer =
        pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].pClientShadowFaultBuffer[REPLAYABLE_FAULT_BUFFER];

    pParams->pShadowBuffer = pClientShadowFaultBuffer->pBufferAddress;
    pParams->pShadowBufferMetadata = pClientShadowFaultBuffer->pFaultBufferMetadataAddress;
    pParams->bufferSize = pStaticInfo->replayableFaultBufferSize;

    return NV_OK;
}

NV_STATUS
faultbufCtrlCmdMmuFaultBufferUnregisterReplayBuf_IMPL
(
    MmuFaultBuffer *pMmuFaultBuffer,
    NVC369_CTRL_MMU_FAULT_BUFFER_UNREGISTER_REPLAY_BUF_PARAMS *pParams
)
{
    NV_STATUS status;
    OBJGPU *pGpu = GPU_RES_GET_GPU(pMmuFaultBuffer);
    KernelGmmu *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);

    NV_ASSERT_OR_RETURN(gpuIsCCFeatureEnabled(pGpu), NV_ERR_NOT_SUPPORTED);

    // This call takes GpuLock, so shadowBuffer pointers can be accessed without grabbing lock protecting them
    if (pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].pClientShadowFaultBuffer[REPLAYABLE_FAULT_BUFFER] == NULL)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Client shadow fault buffer for replayable faults does not exist\n");
        return NV_OK;
    }

    if (pParams->pShadowBuffer !=
        pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].pClientShadowFaultBuffer[REPLAYABLE_FAULT_BUFFER]->pBufferAddress)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Given client shadow fault buffer for replayable faults does not "
                  "match with the actual\n");
    }

    status = kgmmuClientShadowFaultBufferFree_HAL(pGpu, pKernelGmmu, REPLAYABLE_FAULT_BUFFER);
    if (status != NV_OK) {
        NV_PRINTF(LEVEL_ERROR,
                  "Error freeing client shadow fault buffer for replayable faults\n");
    }

    return status;
}
