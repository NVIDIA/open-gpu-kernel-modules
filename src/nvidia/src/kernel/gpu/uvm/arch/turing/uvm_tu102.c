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
#include "gpu/uvm/access_cntr_buffer.h"
#include "kernel/gpu/intr/intr.h"

#include "class/clc365.h"
#include "ctrl/ctrlc365.h"
#include "published/turing/tu102/dev_access_counter.h"
#include "published/turing/tu102/dev_vm.h"

NvU32
uvmGetRegOffsetAccessCntrBufferPut_TU102
(
    OBJUVM *pUvm,
    NvU32   accessCounterIndex
)
{
    NV_ASSERT(accessCounterIndex == 0);

    return NV_VIRTUAL_FUNCTION_PRIV_ACCESS_COUNTER_NOTIFY_BUFFER_PUT;
}

NvU32
uvmGetRegOffsetAccessCntrBufferGet_TU102
(
    OBJUVM *pUvm,
    NvU32   accessCounterIndex
)
{
    NV_ASSERT(accessCounterIndex == 0);

    return NV_VIRTUAL_FUNCTION_PRIV_ACCESS_COUNTER_NOTIFY_BUFFER_GET;
}

NvU32
uvmGetRegOffsetAccessCntrBufferHi_TU102
(
    OBJUVM *pUvm,
    NvU32   accessCounterIndex
)
{
    NV_ASSERT(accessCounterIndex == 0);

    return NV_VIRTUAL_FUNCTION_PRIV_ACCESS_COUNTER_NOTIFY_BUFFER_HI;
}

NvU32
uvmGetRegOffsetAccessCntrBufferLo_TU102
(
    OBJUVM *pUvm,
    NvU32   accessCounterIndex
)
{
    NV_ASSERT(accessCounterIndex == 0);

    return NV_VIRTUAL_FUNCTION_PRIV_ACCESS_COUNTER_NOTIFY_BUFFER_LO;
}

NvU32
uvmGetRegOffsetAccessCntrBufferConfig_TU102
(
    OBJUVM *pUvm,
    NvU32   accessCounterIndex
)
{
    NV_ASSERT(accessCounterIndex == 0);

    return NV_VIRTUAL_FUNCTION_PRIV_ACCESS_COUNTER_CONFIG;
}

NvU32
uvmGetRegOffsetAccessCntrBufferInfo_TU102
(
    OBJUVM *pUvm,
    NvU32   accessCounterIndex
)
{
    NV_ASSERT(accessCounterIndex == 0);

    return NV_VIRTUAL_FUNCTION_PRIV_ACCESS_COUNTER_NOTIFY_BUFFER_INFO;
}

NvU32
uvmGetRegOffsetAccessCntrBufferSize_TU102
(
    OBJUVM *pUvm,
    NvU32   accessCounterIndex
)
{
    NV_ASSERT(accessCounterIndex == 0);

    return NV_VIRTUAL_FUNCTION_PRIV_ACCESS_COUNTER_NOTIFY_BUFFER_SIZE;
}

NV_STATUS
uvmReadAccessCntrBufferPutPtr_TU102
(
    OBJGPU *pGpu,
    OBJUVM *pUvm,
    NvU32   accessCounterIndex,
    NvU32  *pPutOffset
)
{
    *pPutOffset = GPU_VREG_RD32(pGpu, uvmGetRegOffsetAccessCntrBufferPut_HAL(pUvm, accessCounterIndex));

    return NV_OK;
}

NV_STATUS
uvmReadAccessCntrBufferGetPtr_TU102
(
    OBJGPU *pGpu,
    OBJUVM *pUvm,
    NvU32   accessCounterIndex,
    NvU32  *pGetOffset
)
{
    *pGetOffset = GPU_VREG_RD32(pGpu, uvmGetRegOffsetAccessCntrBufferGet_HAL(pUvm, accessCounterIndex));

    return NV_OK;
}

NV_STATUS
uvmWriteAccessCntrBufferGetPtr_TU102
(
    OBJGPU *pGpu,
    OBJUVM *pUvm,
    NvU32   accessCounterIndex,
    NvU32   getPtrValue
)
{
    GPU_VREG_WR32(pGpu, uvmGetRegOffsetAccessCntrBufferGet_HAL(pUvm, accessCounterIndex), getPtrValue);

    return NV_OK;
}

NV_STATUS
uvmEnableAccessCntr_TU102
(
    OBJGPU *pGpu,
    OBJUVM *pUvm,
    NvU32   accessCounterIndex,
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

    uvmProgramAccessCntrBufferEnabled_HAL(pGpu, pUvm, accessCounterIndex, NV_TRUE);

    return NV_OK;
}

NV_STATUS
uvmGetAccessCntrRegisterMappings_TU102
(
    OBJGPU *pGpu,
    OBJUVM *pUvm,
    NvU32   accessCounterIndex,
    NvP64 *pAccessCntrBufferGet,
    NvP64 *pAccessCntrBufferPut,
    NvP64 *pAccessCntrBufferFull,
    NvP64 *pHubIntr,
    NvP64 *pHubIntrEnSet,
    NvP64 *pHubIntrEnClear,
    NvU32 *pAccessCntrMask
)
{
    Intr *pIntr = GPU_GET_INTR(pGpu);
    DEVICE_MAPPING *pMapping = gpuGetDeviceMapping(pGpu, DEVICE_INDEX_GPU, 0);
    NvP64 bar0Mapping = NV_PTR_TO_NvP64(pMapping->gpuNvAddr);
    NvU32 intrVector = intrGetVectorFromEngineId(pGpu, pIntr, MC_ENGINE_IDX_ACCESS_CNTR, NV_FALSE);
    NvU32 leafReg, leafBit;

    leafReg = NV_CTRL_INTR_GPU_VECTOR_TO_LEAF_REG(intrVector);
    leafBit = NV_CTRL_INTR_GPU_VECTOR_TO_LEAF_BIT(intrVector);

    *pAccessCntrBufferGet = NvP64_PLUS_OFFSET(bar0Mapping, GPU_GET_VREG_OFFSET(pGpu, uvmGetRegOffsetAccessCntrBufferGet_HAL(pUvm, accessCounterIndex)));
    *pAccessCntrBufferPut = NvP64_PLUS_OFFSET(bar0Mapping, GPU_GET_VREG_OFFSET(pGpu, uvmGetRegOffsetAccessCntrBufferPut_HAL(pUvm, accessCounterIndex)));
    *pAccessCntrBufferFull = NvP64_PLUS_OFFSET(bar0Mapping,GPU_GET_VREG_OFFSET(pGpu, uvmGetRegOffsetAccessCntrBufferInfo_HAL(pUvm, accessCounterIndex)));
    *pHubIntr = NvP64_PLUS_OFFSET(bar0Mapping, GPU_GET_VREG_OFFSET(pGpu, NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF(leafReg)));
    *pHubIntrEnSet = NvP64_PLUS_OFFSET(bar0Mapping,  GPU_GET_VREG_OFFSET(pGpu, NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF_EN_SET(leafReg)));
    *pHubIntrEnClear = NvP64_PLUS_OFFSET(bar0Mapping,  GPU_GET_VREG_OFFSET(pGpu, NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF_EN_CLEAR(leafReg)));
    *pAccessCntrMask = NVBIT(leafBit);

    return NV_OK;
}

NV_STATUS
uvmReadAccessCntrBufferFullPtr_TU102
(
    OBJGPU *pGpu,
    OBJUVM *pUvm,
    NvU32   accessCounterIndex,
    NvBool *pFullFlag
)
{
    NvU32 info = GPU_VREG_RD32(pGpu, uvmGetRegOffsetAccessCntrBufferInfo_HAL(pUvm, accessCounterIndex));

    *pFullFlag = FLD_TEST_DRF(_VIRTUAL_FUNCTION, _PRIV_ACCESS_COUNTER_NOTIFY_BUFFER_INFO, _FULL, _TRUE, info);

    return NV_OK;
}

NV_STATUS
uvmAccessCntrSetThreshold_TU102
(
    OBJGPU *pGpu,
    OBJUVM *pUvm,
    NvU32   accessCounterIndex,
    NvU32   threshold
)
{
    NvU32 regOffsetConfig = uvmGetRegOffsetAccessCntrBufferConfig_HAL(pUvm, accessCounterIndex);
    NvU32 config          = GPU_VREG_RD32(pGpu, regOffsetConfig);

    config = FLD_SET_DRF_NUM(_VIRTUAL_FUNCTION, _PRIV_ACCESS_COUNTER_CONFIG, _THRESHOLD, threshold, config);
    GPU_VREG_WR32(pGpu, regOffsetConfig, config);

    return NV_OK;
}

// Note: This function returns zero for chips which do not support the access counter.
NvU32
uvmGetAccessCounterBufferSize_TU102
(
    OBJGPU *pGpu,
    OBJUVM *pUvm,
    NvU32   accessCounterIndex
)
{
    return GPU_VREG_RD32(pGpu, uvmGetRegOffsetAccessCntrBufferSize_HAL(pUvm, accessCounterIndex)) *
            NV_ACCESS_COUNTER_NOTIFY_BUF_SIZE;
}

NV_STATUS
uvmAccessCntrSetGranularity_TU102
(
    OBJGPU *pGpu,
    OBJUVM *pUvm,
    NvU32   accessCounterIndex,
    ACCESS_CNTR_TYPE accessCntType,
    NvU32 granularity
)
{
    NvU32 regOffsetConfig = uvmGetRegOffsetAccessCntrBufferConfig_HAL(pUvm, accessCounterIndex);
    NvU32 config          = GPU_VREG_RD32(pGpu, regOffsetConfig);

    if (accessCntType == MIMC)
    {
        switch(granularity)
        {
            case NVC365_CTRL_ACCESS_COUNTER_GRANULARITY_64K:
                config = FLD_SET_DRF(_VIRTUAL_FUNCTION, _PRIV_ACCESS_COUNTER_CONFIG, _MIMC_GRANULARITY, _64K, config);
                break;
            case NVC365_CTRL_ACCESS_COUNTER_GRANULARITY_2M:
                config = FLD_SET_DRF(_VIRTUAL_FUNCTION, _PRIV_ACCESS_COUNTER_CONFIG, _MIMC_GRANULARITY, _2M, config);
                break;
            case NVC365_CTRL_ACCESS_COUNTER_GRANULARITY_16M:
                config = FLD_SET_DRF(_VIRTUAL_FUNCTION, _PRIV_ACCESS_COUNTER_CONFIG, _MIMC_GRANULARITY, _16M, config);
                break;
            case NVC365_CTRL_ACCESS_COUNTER_GRANULARITY_16G:
                config = FLD_SET_DRF(_VIRTUAL_FUNCTION, _PRIV_ACCESS_COUNTER_CONFIG, _MIMC_GRANULARITY, _16G, config);
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
                config = FLD_SET_DRF(_VIRTUAL_FUNCTION, _PRIV_ACCESS_COUNTER_CONFIG, _MOMC_GRANULARITY, _64K, config);
                break;
            case NVC365_CTRL_ACCESS_COUNTER_GRANULARITY_2M:
                config = FLD_SET_DRF(_VIRTUAL_FUNCTION, _PRIV_ACCESS_COUNTER_CONFIG, _MOMC_GRANULARITY, _2M, config);
                break;
            case NVC365_CTRL_ACCESS_COUNTER_GRANULARITY_16M:
                config = FLD_SET_DRF(_VIRTUAL_FUNCTION, _PRIV_ACCESS_COUNTER_CONFIG, _MOMC_GRANULARITY, _16M, config);
                break;
            case NVC365_CTRL_ACCESS_COUNTER_GRANULARITY_16G:
                config = FLD_SET_DRF(_VIRTUAL_FUNCTION, _PRIV_ACCESS_COUNTER_CONFIG, _MOMC_GRANULARITY, _16G, config);
                break;
            default:
                return NV_ERR_INVALID_ARGUMENT;
        }
    }
    else
        return NV_ERR_INVALID_ARGUMENT;

    GPU_VREG_WR32(pGpu, regOffsetConfig, config);

    return NV_OK;
}

void
uvmProgramWriteAccessCntrBufferAddress_TU102
(
    OBJGPU *pGpu,
    OBJUVM *pUvm,
    NvU32   accessCounterIndex,
    NvU64   addr
)
{
    GPU_VREG_WR32(pGpu, uvmGetRegOffsetAccessCntrBufferHi_HAL(pUvm, accessCounterIndex), NvU64_HI32(addr));
    GPU_VREG_WR32(pGpu, uvmGetRegOffsetAccessCntrBufferLo_HAL(pUvm, accessCounterIndex), NvU64_LO32(addr));
}

void
uvmProgramAccessCntrBufferEnabled_TU102
(
    OBJGPU *pGpu,
    OBJUVM *pUvm,
    NvU32   accessCounterIndex,
    NvBool  bEn
)
{
    NvU32 regOffsetLo = uvmGetRegOffsetAccessCntrBufferLo_HAL(pUvm, accessCounterIndex);
    NvU32 lo          = GPU_VREG_RD32(pGpu, regOffsetLo);

    lo = FLD_SET_DRF_NUM(_VIRTUAL_FUNCTION, _PRIV_ACCESS_COUNTER_NOTIFY_BUFFER_LO, _EN, bEn, lo);
    GPU_VREG_WR32(pGpu, regOffsetLo, lo);
}

NvBool
uvmIsAccessCntrBufferEnabled_TU102
(
    OBJGPU *pGpu,
    OBJUVM *pUvm,
    NvU32   accessCounterIndex
)
{
    NvU32 lo = GPU_VREG_RD32(pGpu, uvmGetRegOffsetAccessCntrBufferLo_HAL(pUvm, accessCounterIndex));

    return FLD_TEST_DRF(_VIRTUAL_FUNCTION, _PRIV_ACCESS_COUNTER_NOTIFY_BUFFER_LO, _EN, _TRUE, lo);
}

NvBool
uvmIsAccessCntrBufferPushed_TU102
(
    OBJGPU *pGpu,
    OBJUVM *pUvm,
    NvU32   accessCounterIndex
)
{
    NvU32 info = GPU_VREG_RD32(pGpu, uvmGetRegOffsetAccessCntrBufferInfo_HAL(pUvm, accessCounterIndex));

    return FLD_TEST_DRF(_VIRTUAL_FUNCTION, _PRIV_ACCESS_COUNTER_NOTIFY_BUFFER_INFO, _PUSHED, _TRUE, info);
}

NV_STATUS
uvmEnableAccessCntrIntr_TU102
(
    OBJGPU *pGpu,
    OBJUVM *pUvm,
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
    OBJGPU *pGpu,
    OBJUVM *pUvm
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
    OBJGPU *pGpu,
    OBJUVM *pUvm
)
{
    NvU32 i;

    for (i = 0; i < pUvm->accessCounterBufferCount; i++)
    {
        AccessCounterBuffer *pAccessCounterBuffer = pUvm->pAccessCounterBuffers[i].pAccessCounterBuffer;
        EVENTNOTIFICATION **ppEventNotification;

        if (pAccessCounterBuffer == NULL)
            continue;

        ppEventNotification = inotifyGetNotificationListPtr(staticCast(pAccessCounterBuffer, INotifier));

        if (ppEventNotification == NULL)
            continue;

        if (pUvm->accessCounterBufferCount > 1)
        {
            NvU32 get, put;

            NV_ASSERT_OK_OR_RETURN(uvmReadAccessCntrBufferGetPtr(pGpu, pUvm, pAccessCounterBuffer->accessCounterIndex, &get));
            NV_ASSERT_OK_OR_RETURN(uvmReadAccessCntrBufferPutPtr(pGpu, pUvm, pAccessCounterBuffer->accessCounterIndex, &put));

            //
            // We can't know which access counter buffer has the new entries,
            // so we send out notifications for each non-empty buffer.
            // This can generate extra notifications when interrupts for different counters
            // arrive in rapid succession, so client doesn't update get pointer in time.
            //
            // We could remember the last known put pointer and compare it, but
            // theoretically the buffer can wrap around while the interrupt is being handled by client
            //
            if (get == put)
                continue;
        }

        NV_ASSERT_OK(notifyEvents(pGpu, *ppEventNotification, NVC365_NOTIFIERS_ACCESS_COUNTER,
                                  0, 0, NV_OK, NV_OS_WRITE_THEN_AWAKEN));
    }

    return NV_OK;
}
