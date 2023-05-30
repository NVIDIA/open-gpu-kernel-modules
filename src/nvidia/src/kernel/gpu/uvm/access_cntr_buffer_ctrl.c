/*
 * SPDX-FileCopyrightText: Copyright (c) 2013-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "kernel/gpu/subdevice/subdevice.h"
#include "gpu/mmu/kern_gmmu.h"
#include "gpu/uvm/uvm.h"
#include "rmapi/control.h"

#include "gpu/uvm/access_cntr_buffer.h"

NV_STATUS
accesscntrCtrlCmdAccessCntrBufferReadGet_IMPL
(
    AccessCounterBuffer *pAccessCounterBuffer,
    NVC365_CTRL_ACCESS_CNTR_BUFFER_READ_GET_PARAMS *pGetParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pAccessCounterBuffer);
    OBJUVM *pUvm = GPU_GET_UVM(pGpu);

    return uvmReadAccessCntrBufferGetPtr_HAL(pGpu, pUvm, pAccessCounterBuffer->accessCounterIndex, &pGetParams->accessCntrBufferGetOffset);
}

NV_STATUS
accesscntrCtrlCmdAccessCntrBufferWriteGet_IMPL
(
    AccessCounterBuffer *pAccessCounterBuffer,
    NVC365_CTRL_ACCESS_CNTR_BUFFER_WRITE_GET_PARAMS *pGetParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pAccessCounterBuffer);
    OBJUVM *pUvm = GPU_GET_UVM(pGpu);

    return uvmWriteAccessCntrBufferGetPtr_HAL(pGpu, pUvm, pAccessCounterBuffer->accessCounterIndex, pGetParams->accessCntrBufferGetValue);
}

NV_STATUS
accesscntrCtrlCmdAccessCntrBufferReadPut_IMPL
(
    AccessCounterBuffer *pAccessCounterBuffer,
    NVC365_CTRL_ACCESS_CNTR_BUFFER_READ_PUT_PARAMS *pGetParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pAccessCounterBuffer);
    OBJUVM *pUvm = GPU_GET_UVM(pGpu);

    return uvmReadAccessCntrBufferPutPtr_HAL(pGpu, pUvm, pAccessCounterBuffer->accessCounterIndex, &pGetParams->accessCntrBufferPutOffset);
}

NV_STATUS
accesscntrCtrlCmdAccessCntrBufferGetSize_IMPL
(
    AccessCounterBuffer *pAccessCounterBuffer,
    NVC365_CTRL_ACCESS_CNTR_BUFFER_GET_SIZE_PARAMS *pGetParams
)
{
    pGetParams->accessCntrBufferSize = pAccessCounterBuffer->pUvmAccessCntrAllocMemDesc->Size;

    return NV_OK;
}

NV_STATUS
accesscntrCtrlCmdAccessCntrBufferEnable_IMPL
(
    AccessCounterBuffer *pAccessCounterBuffer,
    NVC365_CTRL_ACCESS_CNTR_BUFFER_ENABLE_PARAMS *pGetParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pAccessCounterBuffer);
    OBJUVM *pUvm = GPU_GET_UVM(pGpu);
    KernelGmmu *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
    NvU32 intrOwnership = pGetParams->intrOwnership;

    //
    // Note: Changing the ownership mask to RM before disabling notifications could
    // cause starvation issues if not protected by the GPU lock.
    // For example, if interrupts need to be handled between changing of the ownership
    // and disabling of the interrupt, multiple access counter interrupts could come
    // to RM (since ownership changed from UVM to RM) and RM will starve the thread
    // that is trying to disable the interrupt.
    // See bug 2094809 for more details.
    //
    if (intrOwnership != NVC365_CTRL_ACCESS_COUNTER_INTERRUPT_OWNERSHIP_NO_CHANGE)
    {
        NV_CHECK_OR_RETURN(LEVEL_INFO, pKernelGmmu != NULL, NV_ERR_NOT_SUPPORTED);
        kgmmuAccessCntrChangeIntrOwnership(pGpu, pKernelGmmu,
            (intrOwnership == NVC365_CTRL_ACCESS_COUNTER_INTERRUPT_OWNERSHIP_RM));
    }

    if (pGetParams->enable)
    {
        return uvmEnableAccessCntr_HAL(pGpu, pUvm, pAccessCounterBuffer->accessCounterIndex, NV_FALSE);
    }
    else
    {
        return uvmDisableAccessCntr_HAL(pGpu, pUvm, pAccessCounterBuffer->accessCounterIndex, NV_FALSE);
    }
}

NV_STATUS
accesscntrCtrlCmdAccessCntrBufferEnableIntr_IMPL
(
    AccessCounterBuffer *pAccessCounterBuffer,
    NVC365_CTRL_ACCESS_CNTR_BUFFER_ENABLE_INTR_PARAMS *pGetParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pAccessCounterBuffer);
    OBJUVM *pUvm = GPU_GET_UVM(pGpu);
    NV_STATUS status = uvmEnableAccessCntrIntr_HAL(pGpu, pUvm, intr_notify);
    if (status == NV_OK)
    {
        pGetParams->enable = NV_TRUE;
    }
    else
    {
        pGetParams->enable = NV_FALSE;
    }

    return status;
}

NV_STATUS
accesscntrCtrlCmdAccessCntrBufferGetRegisterMappings_IMPL
(
    AccessCounterBuffer *pAccessCounterBuffer,
    NVC365_CTRL_ACCESS_CNTR_BUFFER_GET_REGISTER_MAPPINGS_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pAccessCounterBuffer);
    OBJUVM *pUvm = GPU_GET_UVM(pGpu);

    return uvmGetAccessCntrRegisterMappings_HAL(pGpu, pUvm,
                                                pAccessCounterBuffer->accessCounterIndex,
                                               &pParams->pAccessCntrBufferGet,
                                               &pParams->pAccessCntrBufferPut,
                                               &pParams->pAccessCntrBufferFull,
                                               &pParams->pHubIntr,
                                               &pParams->pHubIntrEnSet,
                                               &pParams->pHubIntrEnClear,
                                               &pParams->accessCntrMask);
}

NV_STATUS
accesscntrCtrlCmdAccessCntrBufferGetFullInfo_IMPL
(
    AccessCounterBuffer *pAccessCounterBuffer,
    NVC365_CTRL_ACCESS_CNTR_BUFFER_GET_FULL_INFO_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pAccessCounterBuffer);
    OBJUVM *pUvm = GPU_GET_UVM(pGpu);

    return uvmReadAccessCntrBufferFullPtr_HAL(pGpu, pUvm, pAccessCounterBuffer->accessCounterIndex, &pParams->fullFlag);
}

NV_STATUS
accesscntrCtrlCmdAccessCntrBufferResetCounters_IMPL
(
    AccessCounterBuffer *pAccessCounterBuffer,
    NVC365_CTRL_ACCESS_CNTR_BUFFER_RESET_COUNTERS_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pAccessCounterBuffer);
    OBJUVM *pUvm = GPU_GET_UVM(pGpu);

    NV_STATUS status = uvmResetAccessCntrBuffer_HAL(pGpu, pUvm, pAccessCounterBuffer->accessCounterIndex, pParams->counterType);
    if (status == NV_OK)
    {
        pParams->resetFlag = NV_TRUE;
    }
    else
    {
        pParams->resetFlag = NV_FALSE;
    }

    return status;
}

NV_STATUS
accesscntrCtrlCmdAccessCntrSetConfig_IMPL
(
    AccessCounterBuffer *pAccessCounterBuffer,
    NVC365_CTRL_ACCESS_CNTR_SET_CONFIG_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pAccessCounterBuffer);
    OBJUVM *pUvm = GPU_GET_UVM(pGpu);
    NvU32 cmd = pParams->cmd;

    if (cmd & NVC365_CTRL_ACCESS_COUNTER_SET_MIMC_GRANULARITY)
        uvmAccessCntrSetGranularity_HAL(pGpu, pUvm, pAccessCounterBuffer->accessCounterIndex,
            MIMC, pParams->mimcGranularity);
    if (cmd & NVC365_CTRL_ACCESS_COUNTER_SET_MOMC_GRANULARITY)
        uvmAccessCntrSetGranularity_HAL(pGpu, pUvm, pAccessCounterBuffer->accessCounterIndex,
            MOMC, pParams->momcGranularity);
    if (cmd & NVC365_CTRL_ACCESS_COUNTER_SET_MIMC_LIMIT)
        uvmAccessCntrSetCounterLimit_HAL(pGpu, pUvm, pAccessCounterBuffer->accessCounterIndex,
            NVC365_CTRL_ACCESS_COUNTER_MIMC_LIMIT, pParams->mimcLimit);
    if (cmd & NVC365_CTRL_ACCESS_COUNTER_SET_MOMC_LIMIT)
        uvmAccessCntrSetCounterLimit_HAL(pGpu, pUvm, pAccessCounterBuffer->accessCounterIndex,
            NVC365_CTRL_ACCESS_COUNTER_MOMC_LIMIT, pParams->momcLimit);
    if (cmd & NVC365_CTRL_ACCESS_COUNTER_SET_THRESHOLD)
        uvmAccessCntrSetThreshold_HAL(pGpu, pUvm, pAccessCounterBuffer->accessCounterIndex,
            pParams->threshold);

    return NV_OK;
}

NV_STATUS
subdeviceCtrlCmdInternalUvmRegisterAccessCntrBuffer_IMPL
(
    Subdevice                                                   *pSubdevice,
    NV2080_CTRL_INTERNAL_UVM_REGISTER_ACCESS_CNTR_BUFFER_PARAMS *pParams
)
{
    NV_STATUS status;
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    OBJUVM *pUvm = GPU_GET_UVM(pGpu);
    NvU32 numBufferPages = NV_ROUNDUP(pParams->bufferSize, RM_PAGE_SIZE) / RM_PAGE_SIZE;

    if (numBufferPages > NV_ARRAY_ELEMENTS(pParams->bufferPteArray) ||
        numBufferPages == 0)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    status = uvmAccessCntrBufferRegister(pGpu, pUvm,
                                         pParams->accessCounterIndex,
                                         pParams->bufferSize,
                                         pParams->bufferPteArray);
    return status;
}

NV_STATUS
subdeviceCtrlCmdInternalUvmUnregisterAccessCntrBuffer_IMPL
(
    Subdevice                                                     *pSubdevice,
    NV2080_CTRL_INTERNAL_UVM_UNREGISTER_ACCESS_CNTR_BUFFER_PARAMS *pParams
)
{
    NV_STATUS status;
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    OBJUVM *pUvm = GPU_GET_UVM(pGpu);

    status = uvmAccessCntrBufferUnregister(pGpu, pUvm, pParams->accessCounterIndex);

    return status;
}
