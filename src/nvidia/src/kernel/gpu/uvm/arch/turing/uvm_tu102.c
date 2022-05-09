/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/gpu.h"
#include "gpu/mmu/kern_gmmu.h"
#include "gpu/uvm/uvm.h"
#include "kernel/gpu/intr/intr.h"

#include "class/clc365.h"
#include "ctrl/ctrlc365.h"
#include "published/turing/tu102/dev_access_counter.h"
#include "published/turing/tu102/dev_vm.h"
#include "published/turing/tu102/dev_fb.h"

NV_STATUS
uvmReadAccessCntrBufferPutPtr_TU102
(
    POBJGPU pGpu,
    POBJUVM pUvm,
    NvU32 *putOffset
)
{

    *putOffset = GPU_VREG_RD32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_ACCESS_COUNTER_NOTIFY_BUFFER_PUT);

    return NV_OK;
}

NV_STATUS
uvmReadAccessCntrBufferGetPtr_TU102
(
    POBJGPU pGpu,
    POBJUVM pUvm,
    NvU32 *getOffset
)
{
    *getOffset = GPU_VREG_RD32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_ACCESS_COUNTER_NOTIFY_BUFFER_GET);

    return NV_OK;
}

NV_STATUS
uvmWriteAccessCntrBufferGetPtr_TU102
(
    POBJGPU pGpu,
    POBJUVM pUvm,
    NvU32 getPtrValue
)
{
    GPU_VREG_WR32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_ACCESS_COUNTER_NOTIFY_BUFFER_GET, getPtrValue);
    return NV_OK;
}

NV_STATUS
uvmEnableAccessCntr_TU102
(
    POBJGPU pGpu,
    POBJUVM pUvm,
    NvBool  bIsErrorRecovery
)
{
    KernelGmmu *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
    NvBool bRmOwnsAccessCntr = !!(pKernelGmmu->uvmSharedIntrRmOwnsMask &
                                  RM_UVM_SHARED_INTR_MASK_HUB_ACCESS_COUNTER_NOTIFY);
    //
    // Do not touch interrupts if in error recovery path
    // Also disable interrupts if RM does not own the interrupt to prevent race
    // See bug 2094809 for more details
    //
    if (!bRmOwnsAccessCntr)
    {
        uvmDisableAccessCntrIntr_HAL(pGpu, pUvm);
    }
    else
    {
        if (!bIsErrorRecovery)
            uvmEnableAccessCntrIntr_HAL(pGpu, pUvm, intr_all);
    }

    GPU_VREG_FLD_WR_DRF_DEF(pGpu, _VIRTUAL_FUNCTION, _PRIV_ACCESS_COUNTER_NOTIFY_BUFFER_LO, _EN, _TRUE);

    return NV_OK;
}

NV_STATUS
uvmGetAccessCntrRegisterMappings_TU102
(
    POBJGPU pGpu,
    POBJUVM pUvm,
    NvP64 *pAccessCntrBufferGet,
    NvP64 *pAccessCntrBufferPut,
    NvP64 *pAccessCntrBufferFull,
    NvP64 *pHubIntr,
    NvP64 *pHubIntrEnSet,
    NvP64 *pHubIntrEnClear,
    NvU32 *accessCntrMask
)
{
    Intr *pIntr = GPU_GET_INTR(pGpu);
    DEVICE_MAPPING *pMapping = gpuGetDeviceMapping(pGpu, DEVICE_INDEX_GPU, 0);
    NvP64 bar0Mapping = NV_PTR_TO_NvP64(pMapping->gpuNvAddr);
    NvU32 intrVector = intrGetVectorFromEngineId(pGpu, pIntr, MC_ENGINE_IDX_ACCESS_CNTR, NV_FALSE);
    NvU32 leafReg, leafBit;

    leafReg = NV_CTRL_INTR_GPU_VECTOR_TO_LEAF_REG(intrVector);
    leafBit = NV_CTRL_INTR_GPU_VECTOR_TO_LEAF_BIT(intrVector);

    *pAccessCntrBufferGet = NvP64_PLUS_OFFSET(bar0Mapping, GPU_GET_VREG_OFFSET(pGpu, NV_VIRTUAL_FUNCTION_PRIV_ACCESS_COUNTER_NOTIFY_BUFFER_GET));
    *pAccessCntrBufferPut = NvP64_PLUS_OFFSET(bar0Mapping, GPU_GET_VREG_OFFSET(pGpu, NV_VIRTUAL_FUNCTION_PRIV_ACCESS_COUNTER_NOTIFY_BUFFER_PUT));
    *pAccessCntrBufferFull = NvP64_PLUS_OFFSET(bar0Mapping,GPU_GET_VREG_OFFSET(pGpu, NV_VIRTUAL_FUNCTION_PRIV_ACCESS_COUNTER_NOTIFY_BUFFER_INFO));
    *pHubIntr = NvP64_PLUS_OFFSET(bar0Mapping, GPU_GET_VREG_OFFSET(pGpu, NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF(leafReg)));
    *pHubIntrEnSet = NvP64_PLUS_OFFSET(bar0Mapping,  GPU_GET_VREG_OFFSET(pGpu, NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF_EN_SET(leafReg)));
    *pHubIntrEnClear = NvP64_PLUS_OFFSET(bar0Mapping,  GPU_GET_VREG_OFFSET(pGpu, NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF_EN_CLEAR(leafReg)));
    *accessCntrMask = NVBIT(leafBit);
    return NV_OK;
}

NV_STATUS
uvmReadAccessCntrBufferFullPtr_TU102
(
    POBJGPU pGpu,
    POBJUVM pUvm,
    NvBool *fullFlag
)
{
    NvU32 fullPtrValue;

    fullPtrValue = GPU_VREG_RD32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_ACCESS_COUNTER_NOTIFY_BUFFER_INFO);
    if (fullPtrValue & NV_PFB_NISO_ACCESS_COUNTER_NOTIFY_BUFFER_INFO_FULL_TRUE)
    {
        *fullFlag = NV_TRUE;
    }
    else
    {
        *fullFlag = NV_FALSE;
    }
    return NV_OK;
}

NV_STATUS
uvmAccessCntrSetThreshold_TU102(POBJGPU pGpu, POBJUVM pUvm, NvU32 threshold)
{

    GPU_VREG_FLD_WR_DRF_NUM(pGpu, _VIRTUAL_FUNCTION_PRIV, _ACCESS_COUNTER_CONFIG, _THRESHOLD, threshold);
    return NV_OK;
}

// Note: This function returns zero for chips which do not support the access counter.
NvU32 uvmGetAccessCounterBufferSize_TU102(POBJGPU pGpu, POBJUVM pUvm)
{

    return GPU_VREG_RD32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_ACCESS_COUNTER_NOTIFY_BUFFER_SIZE) *
            NV_ACCESS_COUNTER_NOTIFY_BUF_SIZE;
}

NV_STATUS
uvmAccessCntrSetGranularity_TU102(POBJGPU pGpu, POBJUVM pUvm, ACCESS_CNTR_TYPE accessCntType, NvU32 granularity)
{

    if (accessCntType == MIMC)
    {
        switch(granularity)
        {
            case NVC365_CTRL_ACCESS_COUNTER_GRANULARITY_64K:
                GPU_VREG_FLD_WR_DRF_DEF(pGpu, _VIRTUAL_FUNCTION_PRIV, _ACCESS_COUNTER_CONFIG, _MIMC_GRANULARITY, _64K);
                break;
            case NVC365_CTRL_ACCESS_COUNTER_GRANULARITY_2M:
                GPU_VREG_FLD_WR_DRF_DEF(pGpu, _VIRTUAL_FUNCTION_PRIV, _ACCESS_COUNTER_CONFIG, _MIMC_GRANULARITY, _2M);
                break;
            case NVC365_CTRL_ACCESS_COUNTER_GRANULARITY_16M:
                GPU_VREG_FLD_WR_DRF_DEF(pGpu, _VIRTUAL_FUNCTION_PRIV, _ACCESS_COUNTER_CONFIG, _MIMC_GRANULARITY, _16M);
                break;
            case NVC365_CTRL_ACCESS_COUNTER_GRANULARITY_16G:
                GPU_VREG_FLD_WR_DRF_DEF(pGpu, _VIRTUAL_FUNCTION_PRIV, _ACCESS_COUNTER_CONFIG, _MIMC_GRANULARITY, _16G);
                break;
            default:
                return NV_ERR_INVALID_ARGUMENT;
        }
    }
    else if (accessCntType == MOMC)
    {
        switch(granularity)
        {
            case NVC365_CTRL_ACCESS_COUNTER_GRANULARITY_64K:
                GPU_VREG_FLD_WR_DRF_DEF(pGpu, _VIRTUAL_FUNCTION_PRIV, _ACCESS_COUNTER_CONFIG, _MOMC_GRANULARITY, _64K);
                break;
            case NVC365_CTRL_ACCESS_COUNTER_GRANULARITY_2M:
                GPU_VREG_FLD_WR_DRF_DEF(pGpu, _VIRTUAL_FUNCTION_PRIV, _ACCESS_COUNTER_CONFIG, _MOMC_GRANULARITY, _2M);
                break;
            case NVC365_CTRL_ACCESS_COUNTER_GRANULARITY_16M:
                GPU_VREG_FLD_WR_DRF_DEF(pGpu, _VIRTUAL_FUNCTION_PRIV, _ACCESS_COUNTER_CONFIG, _MOMC_GRANULARITY, _16M);
                break;
            case NVC365_CTRL_ACCESS_COUNTER_GRANULARITY_16G:
                GPU_VREG_FLD_WR_DRF_DEF(pGpu, _VIRTUAL_FUNCTION_PRIV, _ACCESS_COUNTER_CONFIG, _MOMC_GRANULARITY, _16G);
                break;
            default:
                return NV_ERR_INVALID_ARGUMENT;
        }
    }
    else
        return NV_ERR_INVALID_ARGUMENT;

    return NV_OK;
}

void
uvmWriteAccessCntrBufferHiReg_TU102
(
    POBJGPU pGpu,
    POBJUVM pUvm,
    NvU32   hiVal
)
{
   GPU_VREG_WR32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_ACCESS_COUNTER_NOTIFY_BUFFER_HI, hiVal);
}

void
uvmWriteAccessCntrBufferLoReg_TU102
(
    POBJGPU pGpu,
    POBJUVM pUvm,
    NvU32   loVal
)
{

   GPU_VREG_WR32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_ACCESS_COUNTER_NOTIFY_BUFFER_LO, loVal);
}

NvU32
uvmReadAccessCntrBufferLoReg_TU102
(
    POBJGPU pGpu,
    POBJUVM pUvm
)
{
    return GPU_VREG_RD32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_ACCESS_COUNTER_NOTIFY_BUFFER_LO);
}

NvU32
uvmReadAccessCntrBufferInfoReg_TU102
(
    POBJGPU pGpu,
    POBJUVM pUvm
)
{
    return GPU_VREG_RD32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_ACCESS_COUNTER_NOTIFY_BUFFER_INFO);
}

NV_STATUS
uvmEnableAccessCntrIntr_TU102
(
    POBJGPU pGpu,
    POBJUVM pUvm,
    NvU32   intrType
)
{
    Intr *pIntr = GPU_GET_INTR(pGpu);

    if (intrType == intr_all || intrType == intr_notify)
    {
        intrEnableLeaf_HAL(pGpu, pIntr,
            intrGetVectorFromEngineId(pGpu, pIntr, MC_ENGINE_IDX_ACCESS_CNTR, NV_FALSE));
    }

    return NV_OK;
}

NV_STATUS
uvmDisableAccessCntrIntr_TU102
(
    POBJGPU pGpu,
    POBJUVM pUvm
)
{
    Intr *pIntr = GPU_GET_INTR(pGpu);

    intrDisableLeaf_HAL(pGpu, pIntr,
        intrGetVectorFromEngineId(pGpu, pIntr, MC_ENGINE_IDX_ACCESS_CNTR, NV_FALSE));
    return NV_OK;
}

NV_STATUS
uvmAccessCntrService_TU102
(
    POBJGPU pGpu,
    POBJUVM pUvm
)
{
    NvU64       accessCntrAddress = 0;
    PEVENTNOTIFICATION *ppEventNotification  = NULL;

    if (NV_OK == CliGetEventNotificationList(pUvm->accessCntrBuffer.hAccessCntrBufferClient,
                    pUvm->accessCntrBuffer.hAccessCntrBufferObject, NULL, &ppEventNotification) && ppEventNotification)
    {
        NV_ASSERT_OK_OR_RETURN(notifyEvents(pGpu, *ppEventNotification, NVC365_NOTIFIERS_ACCESS_COUNTER,
        NvU64_HI32(accessCntrAddress), NvU64_LO32(accessCntrAddress), NV_OK, NV_OS_WRITE_THEN_AWAKEN));
    }
    return NV_OK;
}
