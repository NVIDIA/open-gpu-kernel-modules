/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*!
 * @file  kern_gmmu_tu102.c
 * @brief TURING specific HAL MMU routines reside in this file
 */

#define NVOC_KERN_GMMU_H_PRIVATE_ACCESS_ALLOWED

#include "gpu/mmu/kern_gmmu.h"
#include "gpu/gpu.h"
#include "kernel/gpu/intr/intr.h"

#include "published/turing/tu102/dev_fb.h"
#include "published/turing/tu102/dev_vm.h"


/*!
 * @brief Checks for any pending invalidations.
 *
 * @param pGpu
 * @param pKernelGmmu
 * @param pTimeOut      Timeout for the invalidate operation.
 * @param gfid          GFID
 */
NV_STATUS
kgmmuCheckPendingInvalidates_TU102
(
    OBJGPU     *pGpu,
    KernelGmmu *pKernelGmmu,
    RMTIMEOUT  *pTimeOut,
    NvU32       gfid
)
{
    NV_STATUS status = NV_OK;

    while (1)
    {
        NvU32   regVal;

        {
            regVal = GPU_VREG_RD32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE);
        }

        if (FLD_TEST_DRF(_VIRTUAL_FUNCTION_PRIV, _MMU_INVALIDATE, _TRIGGER, _FALSE, regVal))
            break;

        if (API_GPU_IN_RESET_SANITY_CHECK(pGpu))
            status = NV_ERR_GPU_IN_FULLCHIP_RESET;
        else if (!API_GPU_ATTACHED_SANITY_CHECK(pGpu))
            status = NV_ERR_GPU_IS_LOST;
        else
            status = gpuCheckTimeout(pGpu, pTimeOut);

        if (NV_OK != status)
            break;

        osSpinLoop();
    }
    return status;
}

/*!
 * @brief Commit the invalidate command to H/W.
 *
 * @param pGpu
 * @param pKernelGmmu
 * @param pParams      Pointer to TLB_INVALIDATE_PARAMS data.
 */
NV_STATUS
kgmmuCommitTlbInvalidate_TU102
(
    OBJGPU                *pGpu,
    KernelGmmu            *pKernelGmmu,
    TLB_INVALIDATE_PARAMS *pParams
)
{
    NV_STATUS status = NV_OK;

    if (!FLD_TEST_DRF(_VIRTUAL_FUNCTION_PRIV, _MMU_INVALIDATE, _ALL_PDB, _TRUE, pParams->regVal))
    {
        kgmmuSetPdbToInvalidate_HAL(pGpu, pKernelGmmu, pParams);
    }

    {
        if (IS_VIRTUAL(pGpu))
        {
            // Prevent VF from updating INVALIDATE_ALL_PDB, bug 3356599
            pParams->regVal = FLD_SET_DRF(_VIRTUAL_FUNCTION_PRIV, _MMU_INVALIDATE, _ALL_PDB, _FALSE, pParams->regVal);
        }

        GPU_VREG_WR32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE, pParams->regVal);
    }

    // Wait for the invalidate command to complete.
    status = kgmmuCheckPendingInvalidates_HAL(pGpu, pKernelGmmu, &pParams->timeout, pParams->gfid);

    return status;
}

/*!
 * @brief This function sets the PDB physical address for the VAS whose
 *        entries are to be invalidated.
 *
 * @param[in] pGpu
 * @param[in] pKernelGmmu
 * @param[in] pParams       Pointer to TLB_INVALIDATE_PARAMS data
 */
void
kgmmuSetPdbToInvalidate_TU102
(
    OBJGPU                *pGpu,
    KernelGmmu            *pKernelGmmu,
    TLB_INVALIDATE_PARAMS *pParams
)
{
    {
        GPU_VREG_WR32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_PDB,
                DRF_NUM(_VIRTUAL_FUNCTION_PRIV, _MMU_INVALIDATE_PDB, _ADDR,
                        NvU64_LO32(pParams->pdbAddress >>
                                NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_PDB_ADDR_ALIGNMENT)) |
                DRF_NUM(_VIRTUAL_FUNCTION_PRIV, _MMU_INVALIDATE_PDB, _APERTURE, pParams->pdbAperture));

        GPU_VREG_WR32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_UPPER_PDB,
                DRF_NUM(_VIRTUAL_FUNCTION_PRIV, _MMU_INVALIDATE_UPPER_PDB, _ADDR,
                        NvU64_LO32((pParams->pdbAddress >>
                                    NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_PDB_ADDR_ALIGNMENT) >>
                                    DRF_SIZE(NV_VIRTUAL_FUNCTION_PRIV_MMU_INVALIDATE_PDB_ADDR))));
    }
}

/**
 * @brief     Initialize the GMMU format families.
 * @details   Turing supports PA based comptagline allocation policy
 *
 * @param[in] pGpu
 * @param[in] pKernelGmmu
 */
NV_STATUS
kgmmuFmtFamiliesInit_TU102(OBJGPU *pGpu, KernelGmmu *pKernelGmmu)
{
    extern NV_STATUS kgmmuFmtFamiliesInit_GV100(OBJGPU *pGpu, KernelGmmu *pKernelGmmu);

    NvU32            v;
    NV_STATUS        result;
    GMMU_FMT_FAMILY *pFam;

    for (v = 0; v < GMMU_FMT_MAX_VERSION_COUNT; ++v)
    {
        pFam = pKernelGmmu->pFmtFamilies[v];
        if (NULL != pFam)
        {
            if (kgmmuIsBug2720120WarEnabled(pKernelGmmu))
            {
                NV_ASSERT_OK_OR_RETURN(kgmmuSetupWarForBug2720120_HAL(pKernelGmmu, pFam));
            }
        }
    }

    // inherit former FmtFamilies setup procedure
    result = kgmmuFmtFamiliesInit_GV100(pGpu, pKernelGmmu);

    return result;
}

/*!
 * @brief Set membar parameters for tlb invalidation and return
 * any additional sysmembar flushes that is required after tlb invalidation
 * is committed.
 *
 * During PTE downgrades, tlb invalidates should be followed by
 * sysmembar. Since Turing doesnt have HSHUB we dont need any
 * additional flushes.
 *
 * Use invalidate units SYSMEMBAR and return flushCount 0.
 *
 * @param[in] pGpu
 * @param[in] pKernelGmmu
 * @param[in] pParams     Tlb invalidation parameter structure.
 *
 */
NvU32
kgmmuSetTlbInvalidateMembarWarParameters_TU102
(
    OBJGPU                *pGpu,
    KernelGmmu            *pKernelGmmu,
    TLB_INVALIDATE_PARAMS *pParams
)
{

    pParams->regVal = FLD_SET_DRF(_VIRTUAL_FUNCTION_PRIV, _MMU_INVALIDATE, _SYS_MEMBAR, _TRUE, pParams->regVal);
    pParams->regVal = FLD_SET_DRF(_VIRTUAL_FUNCTION_PRIV, _MMU_INVALIDATE, _ACK,    _GLOBALLY, pParams->regVal);

    return 0;
}

NV_STATUS
kgmmuGetFaultRegisterMappings_TU102
(
    OBJGPU               *pGpu,
    KernelGmmu            *pKernelGmmu,
    NvU32                 index,
    NvP64                 *pFaultBufferGet,
    NvP64                 *pFaultBufferPut,
    NvP64                 *pFaultBufferInfo,
    NvP64                 *pHubIntr,
    NvP64                 *pHubIntrEnSet,
    NvP64                 *pHubIntrEnClear,
    NvU32                 *faultMask,
    NvP64                 *pPrefetchCtrl
)
{
    Intr               *pIntr       = GPU_GET_INTR(pGpu);
    DEVICE_MAPPING     *pMapping    = gpuGetDeviceMapping(pGpu, DEVICE_INDEX_GPU, 0);
    NvP64               bar0Mapping = NV_PTR_TO_NvP64(pMapping->gpuNvAddr);
    NvU32               intrVector  = intrGetVectorFromEngineId(pGpu, pIntr, MC_ENGINE_IDX_REPLAYABLE_FAULT, NV_FALSE);
    NvU32 leafReg, leafBit;

    leafReg = NV_CTRL_INTR_GPU_VECTOR_TO_LEAF_REG(intrVector);
    leafBit = NV_CTRL_INTR_GPU_VECTOR_TO_LEAF_BIT(intrVector);

    NV_ASSERT_OR_RETURN((index < NUM_FAULT_BUFFERS), NV_ERR_INVALID_ARGUMENT);

    *pFaultBufferGet = NvP64_PLUS_OFFSET(bar0Mapping,
                 GPU_GET_VREG_OFFSET(pGpu, NV_VIRTUAL_FUNCTION_PRIV_MMU_FAULT_BUFFER_GET(index)));
    *pFaultBufferPut = NvP64_PLUS_OFFSET(bar0Mapping,
                 GPU_GET_VREG_OFFSET(pGpu, NV_VIRTUAL_FUNCTION_PRIV_MMU_FAULT_BUFFER_PUT(index)));
    // Note: this variable is deprecated since buffer overflow is not a seperate register from Volta
    *pFaultBufferInfo = 0;
    *pHubIntr = NvP64_PLUS_OFFSET(bar0Mapping, GPU_GET_VREG_OFFSET(pGpu, NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF(leafReg)));
    *pHubIntrEnSet = NvP64_PLUS_OFFSET(bar0Mapping, GPU_GET_VREG_OFFSET(pGpu, NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF_EN_SET(leafReg)));
    *pHubIntrEnClear = NvP64_PLUS_OFFSET(bar0Mapping, GPU_GET_VREG_OFFSET(pGpu, NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF_EN_CLEAR(leafReg)));

    // Only REPLAYABLE fault buffer and ACCESS Counter interrupts can be owned by clients
    if (index == NV_VIRTUAL_FUNCTION_PRIV_MMU_REPLAY_FAULT_BUFFER)
    {
        *faultMask = NVBIT(leafBit);
        *pPrefetchCtrl = NvP64_PLUS_OFFSET(bar0Mapping, GPU_GET_VREG_OFFSET(pGpu, NV_VIRTUAL_FUNCTION_PRIV_MMU_PAGE_FAULT_CTRL));
    }

    return NV_OK;
}


/**
 * @brief Test the access counter write NAK.
 *
 * @param[in]  pGpu
 * @param[in]  pKernelGmmu
 * @retval     true                  Write was negatively acknowledged.
 */
NvBool kgmmuTestAccessCounterWriteNak_TU102
(
    OBJGPU     *pGpu,
    KernelGmmu *pKernelGmmu
)
{
    NvU32 accessCntrInfo = GPU_VREG_RD32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_ACCESS_COUNTER_NOTIFY_BUFFER_INFO);
    return FLD_TEST_DRF(_VIRTUAL_FUNCTION_PRIV, _ACCESS_COUNTER_NOTIFY_BUFFER_INFO,
                                      _WRITE_NACK, _TRUE, accessCntrInfo);
}

/*!
 * @brief Service replayable fault
 *
 * @param[in] pGpu
 * @param[in] pKernelGmmu
 *
 * @returns NV_STATUS
 */
NV_STATUS
kgmmuServiceReplayableFault_TU102
(
    OBJGPU     *pGpu,
    KernelGmmu *pKernelGmmu
)
{
    NV_STATUS rmStatus = NV_OK;
    PEVENTNOTIFICATION *ppEventNotification  = NULL;

    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu))
        return NV_OK;

    if (NV_OK == CliGetEventNotificationList(pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].hFaultBufferClient,
          pKernelGmmu->mmuFaultBuffer[GPU_GFID_PF].hFaultBufferObject, NULL, &ppEventNotification) && ppEventNotification)
    {
        rmStatus = notifyEvents(pGpu, *ppEventNotification, NVC369_NOTIFIER_MMU_FAULT_REPLAYABLE,
                                0, 0, NV_OK, NV_OS_WRITE_THEN_AWAKEN);
        NV_ASSERT(rmStatus == NV_OK);
    }
    return rmStatus;
}

NvU32
kgmmuReadMmuFaultStatus_TU102
(
    OBJGPU     *pGpu,
    KernelGmmu *pKernelGmmu,
    NvU32       gfid
)
{
    {
        return GPU_VREG_RD32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_MMU_FAULT_STATUS);
    }
}

void
kgmmuWriteMmuFaultStatus_TU102
(
    OBJGPU     *pGpu,
    KernelGmmu *pKernelGmmu,
    NvU32       value
)
{
    GPU_VREG_WR32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_MMU_FAULT_STATUS, value);
}

NvU32
kgmmuReadMmuFaultBufferSize_TU102
(
    OBJGPU     *pGpu,
    KernelGmmu *pKernelGmmu,
    NvU32       index,
    NvU32       gfid
)
{
    {
        return GPU_VREG_RD32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_MMU_FAULT_BUFFER_SIZE(index));
    }
}

NV_STATUS
kgmmuReadFaultBufferGetPtr_TU102
(
    OBJGPU               *pGpu,
    KernelGmmu           *pKernelGmmu,
    NvU32                 index,
    NvU32                *pGetOffset,
    THREAD_STATE_NODE    *pThreadState
)
{
    NvU32 val;
    NV_ASSERT_OR_RETURN((index < NUM_FAULT_BUFFERS), NV_ERR_INVALID_ARGUMENT);

    val = GPU_VREG_RD32_EX(pGpu, NV_VIRTUAL_FUNCTION_PRIV_MMU_FAULT_BUFFER_GET(index), pThreadState);
    *pGetOffset = DRF_VAL(_VIRTUAL_FUNCTION_PRIV, _MMU_FAULT_BUFFER_GET, _PTR, val);

    return NV_OK;
}

NV_STATUS
kgmmuReadFaultBufferPutPtr_TU102
(
    OBJGPU               *pGpu,
    KernelGmmu           *pKernelGmmu,
    NvU32                 index,
    NvU32                *pPutOffset,
    THREAD_STATE_NODE    *pThreadState
)
{
    NvU32 val;
    NV_ASSERT_OR_RETURN((index < NUM_FAULT_BUFFERS), NV_ERR_INVALID_ARGUMENT);

    val = GPU_VREG_RD32_EX(pGpu, NV_VIRTUAL_FUNCTION_PRIV_MMU_FAULT_BUFFER_PUT(index), pThreadState);
    *pPutOffset = DRF_VAL(_VIRTUAL_FUNCTION_PRIV, _MMU_FAULT_BUFFER_PUT, _PTR, val);

    return NV_OK;
}

/*!
 * @brief Test if an MMU non-replayable fault is pending
 *
 * @param[in] pGpu
 * @param[in] pKernelGmmu
 *
 * @returns NV_TRUE if an MMU non-replayable fault is pending, else NV_FALSE
 */
NvBool
kgmmuIsNonReplayableFaultPending_TU102
(
    OBJGPU     *pGpu,
    KernelGmmu *pKernelGmmu,
    THREAD_STATE_NODE *pThreadState
)
{
    NvU32 reg = NV_CTRL_INTR_GPU_VECTOR_TO_LEAF_REG(NV_PFB_PRI_MMU_INT_VECTOR_FAULT_NOTIFY_NON_REPLAYABLE);
    NvU32 bit = NV_CTRL_INTR_GPU_VECTOR_TO_LEAF_BIT(NV_PFB_PRI_MMU_INT_VECTOR_FAULT_NOTIFY_NON_REPLAYABLE);
    NvU32 pending = GPU_VREG_RD32_EX(pGpu, NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF(reg), pThreadState);
    return pending & NVBIT(bit);
}

/*!
 * @brief Clear non replayable fault interrupt.
 *
 * @param[in] pGpu              OBJGPU pointer
 * @param[in] pKernelGmmu       KernelGmmu pointer
 * @param[in] pThreadState      THREAD_STATE_NODE pointer
 */
void
kgmmuClearNonReplayableFaultIntr_TU102
(
    OBJGPU            *pGpu,
    KernelGmmu        *pKernelGmmu,
    THREAD_STATE_NODE *pThreadState
)
{
    Intr *pIntr = GPU_GET_INTR(pGpu);
    intrClearLeafVector_HAL(pGpu, pIntr, NV_PFB_PRI_MMU_INT_VECTOR_FAULT_NOTIFY_NON_REPLAYABLE, pThreadState);
}

/*!
 * @brief Clear replayable fault interrupt.
 *
 * @param[in] pGpu              OBJGPU pointer
 * @param[in] pKernelGmmu       KernelGmmu pointer
 * @param[in] pThreadState      THREAD_STATE_NODE pointer
 */
void
kgmmuClearReplayableFaultIntr_TU102
(
    OBJGPU            *pGpu,
    KernelGmmu        *pKernelGmmu,
    THREAD_STATE_NODE *pThreadState
)
{
    Intr *pIntr = GPU_GET_INTR(pGpu);
    intrClearLeafVector_HAL(pGpu, pIntr, NV_PFB_PRI_MMU_INT_VECTOR_FAULT_NOTIFY_REPLAYABLE, pThreadState);
}
