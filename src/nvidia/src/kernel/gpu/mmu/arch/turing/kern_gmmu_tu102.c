/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "nvtypes.h"
#define NVOC_KERN_GMMU_H_PRIVATE_ACCESS_ALLOWED

#include "gpu/mmu/kern_gmmu.h"
#include "gpu/gpu.h"
#include "kernel/gpu/intr/intr.h"
#include "gpu/mem_sys/kern_mem_sys.h"

#include "published/turing/tu102/dev_fault.h"
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
            NV_ASSERT(IS_GFID_PF(gfid));

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
        NV_ASSERT(IS_GFID_PF(pParams->gfid));

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
        NV_ASSERT(IS_GFID_PF(pParams->gfid));

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
        NV_ASSERT(IS_GFID_PF(gfid));

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
        NV_ASSERT(IS_GFID_PF(gfid));

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
kgmmuWriteFaultBufferGetPtr_TU102
(
    OBJGPU               *pGpu,
    KernelGmmu           *pKernelGmmu,
    NvU32                 index,
    NvU32                 getValue,
    THREAD_STATE_NODE    *pThreadState
)
{
    NV_ASSERT_OR_RETURN((index < NUM_FAULT_BUFFERS), NV_ERR_INVALID_ARGUMENT);

    if (index == REPLAYABLE_FAULT_BUFFER)
    {
        //
        // If HW has detected an overflow condition (PUT == GET - 1 and a fault has
        // arrived, which is dropped due to no more space in the fault buffer), it will
        // not deliver any more faults into the buffer until the overflow condition has
        // been cleared. The overflow condition is cleared by updating the GET index to
        // indicate space in the buffer and writing 1 to the OVERFLOW bit in GET.
        // Unfortunately, this can not be done in the same write because it can collide
        // with an arriving fault on the same cycle, resulting in the overflow condition
        // being instantly reasserted.
        // However, if the index is updated first and then the OVERFLOW bit is cleared
        // such a collision will not cause a reassertion of the overflow condition.
        //
        GPU_VREG_WR32_EX(pGpu, NV_VIRTUAL_FUNCTION_PRIV_MMU_FAULT_BUFFER_GET(index), getValue, pThreadState);
        getValue = FLD_SET_DRF(_VIRTUAL_FUNCTION_PRIV, _MMU_FAULT_BUFFER_GET, _OVERFLOW, _CLEAR, getValue);
        getValue = FLD_SET_DRF(_VIRTUAL_FUNCTION_PRIV, _MMU_FAULT_BUFFER_GET, _GETPTR_CORRUPTED, _CLEAR, getValue);
    }
    else
    {
        //
        // Mask off the _OVERFLOW and _GETPTR_CORRUPTED bits. Writing 0 should
        // have no effect, but they're write 1 to clear so make sure we're not
        // trying to clear an overflow or the corrupted bit.
        //
        getValue = FLD_SET_DRF(_VIRTUAL_FUNCTION_PRIV, _MMU_FAULT_BUFFER_GET, _OVERFLOW, _NO, getValue);
        getValue = FLD_SET_DRF(_VIRTUAL_FUNCTION_PRIV, _MMU_FAULT_BUFFER_GET, _GETPTR_CORRUPTED, _NO, getValue);
    }

    GPU_VREG_WR32_EX(pGpu, NV_VIRTUAL_FUNCTION_PRIV_MMU_FAULT_BUFFER_GET(index), getValue, pThreadState);

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

/**
 * @brief Print information about a MMU fault
 *
 * @param[in]  pGpu                  OBJGPU pointer
 * @param[in]  pKernelGmmu           KernelGmmu pointer
 * @param[in]  mmuFaultEngineId      Engine ID of the faulted engine
 * @param[in]  pMmuExceptionData     FIFO exception packet used
 *                                   for printing fault info
 * @returns
 */
void
kgmmuPrintFaultInfo_TU102
(
    OBJGPU                  *pGpu,
    KernelGmmu              *pKernelGmmu,
    NvU32                    mmuFaultEngineId,
    FIFO_MMU_EXCEPTION_DATA *pMmuExceptionData
)
{
    if (kmemsysCbcIsSafe_HAL(pGpu, GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu)) == NV_FALSE)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "MMU Fault: CBC Backingstore unsafe, this can be reported as "
                  "UNSUPPORTED_KIND error\n");
    }

    void kgmmuPrintFaultInfo_GV100(OBJGPU *pGpu, KernelGmmu *pKernelGmmu, NvU32, FIFO_MMU_EXCEPTION_DATA*);
    kgmmuPrintFaultInfo_GV100(pGpu, pKernelGmmu, mmuFaultEngineId, pMmuExceptionData);
}

/*!
 * @brief Check if the given engineID is BAR1
 *
 * @param[in] pKernelGmmu  KernelGmmu object
 * @param[in] engineID     Engine ID
 *
 * @return True if BAR1
 */
NvBool
kgmmuIsFaultEngineBar1_TU102
(
    KernelGmmu *pKernelGmmu,
    NvU32       engineID
)
{
    return (engineID == NV_PFAULT_MMU_ENG_ID_BAR1);
}

/*!
 * @brief Check if the given engineID is BAR2
 *
 * @param[in] pKernelGmmu  KernelGmmu object
 * @param[in] engineID     Engine ID
 *
 * @return True if BAR2
 */

NvBool
kgmmuIsFaultEngineBar2_TU102
(
    KernelGmmu *pKernelGmmu,
    NvU32       engineID
)
{
    return (engineID == NV_PFAULT_MMU_ENG_ID_BAR2);
}

/*!
 * @brief Returns the HW default size of MMU fault buffers (replayable
 *        or non replayable buffers).
 *
 * @param[in] pGpu         OBJGPU  pointer
 * @param[in] pKernelGmmu  KernelGmmu pointer
 * @param[in] index        Fault buffer index
 *
 * @returns size of MMU fault buffers
 */
NvU32
kgmmuSetAndGetDefaultFaultBufferSize_TU102
(
    OBJGPU           *pGpu,
    KernelGmmu       *pKernelGmmu,
    FAULT_BUFFER_TYPE index,
    NvU32             gfid
)
{
    NvU32 faultBufferSize;
    {
        {
            NV_ASSERT(IS_GFID_PF(gfid));

            GPU_VREG_FLD_IDX_WR_DRF_DEF(pGpu, _VIRTUAL_FUNCTION_PRIV, _MMU_FAULT_BUFFER_SIZE, index, _SET_DEFAULT, _YES);
            faultBufferSize = GPU_VREG_IDX_RD_DRF(pGpu, _VIRTUAL_FUNCTION_PRIV, _MMU_FAULT_BUFFER_SIZE, index, _VAL) * NVC369_BUF_SIZE;
        }
    }

    return faultBufferSize;
}

void
kgmmuWriteMmuFaultBufferSize_TU102
(
    OBJGPU     *pGpu,
    KernelGmmu *pKernelGmmu,
    NvU32       index,
    NvU32       value,
    NvU32       gfid
)
{
    {
        NV_ASSERT(IS_GFID_PF(gfid));

        GPU_VREG_WR32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_MMU_FAULT_BUFFER_SIZE(index), value);
    }

    // Align the HW/SW _GET_PTR if _FAULT_BUFFER_SIZE_ENABLE is _FALSE which will be 0
    if (FLD_TEST_DRF(_PFB_PRI, _MMU_FAULT_BUFFER_SIZE, _ENABLE, _FALSE, value))
    {
        pKernelGmmu->mmuFaultBuffer[gfid].hwFaultBuffers[index].cachedGetIndex = 0;
    }
}

void
kgmmuWriteMmuFaultBufferHiLo_TU102
(
    OBJGPU     *pGpu,
    KernelGmmu *pKernelGmmu,
    NvU32       index,
    NvU32       loValue,
    NvU32       hiValue,
    NvU32       gfid
)
{
    {
        NV_ASSERT(IS_GFID_PF(gfid));

        GPU_VREG_WR32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_MMU_FAULT_BUFFER_HI(index), hiValue);
        GPU_VREG_WR32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_MMU_FAULT_BUFFER_LO(index), loValue);
    }
}

void
kgmmuReadMmuFaultInstHiLo_TU102
(
    OBJGPU     *pGpu,
    KernelGmmu *pKernelGmmu,
    NvU32      *pHiVal,
    NvU32      *pLoVal
)
{
    NV_ASSERT(pHiVal != NULL);
    NV_ASSERT(pLoVal != NULL);
    *pLoVal = GPU_VREG_RD32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_MMU_FAULT_INST_LO);
    *pHiVal = GPU_VREG_RD32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_MMU_FAULT_INST_HI);
}

void
kgmmuReadMmuFaultAddrHiLo_TU102
(
    OBJGPU     *pGpu,
    KernelGmmu *pKernelGmmu,
    NvU32      *pHiVal,
    NvU32      *pLoVal
)
{
    NV_ASSERT(pHiVal != NULL);
    NV_ASSERT(pLoVal != NULL);
    *pLoVal = GPU_VREG_RD32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_MMU_FAULT_ADDR_LO);
    *pHiVal = GPU_VREG_RD32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_MMU_FAULT_ADDR_HI);
}

NvU32
kgmmuReadMmuFaultInfo_TU102
(
    OBJGPU     *pGpu,
    KernelGmmu *pKernelGmmu
)
{
    return GPU_VREG_RD32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_MMU_FAULT_INFO);
}

NvU32
kgmmuGetEccCounts_TU102
(
    OBJGPU     *pGpu,
    KernelGmmu *pKernelGmmu
)
{
    NvU32 mmuCount = 0;
    NvU32 regVal;

    // L2TLB
    regVal = GPU_REG_RD32(pGpu, NV_PFB_PRI_MMU_L2TLB_ECC_UNCORRECTED_ERR_COUNT);
    mmuCount += DRF_VAL(_PFB_PRI_MMU, _L2TLB_ECC, _UNCORRECTED_ERR_COUNT_UNIQUE, regVal);

    // HUBTLB
    regVal = GPU_REG_RD32(pGpu, NV_PFB_PRI_MMU_HUBTLB_ECC_UNCORRECTED_ERR_COUNT);
    mmuCount += DRF_VAL(_PFB_PRI_MMU, _HUBTLB_ECC, _UNCORRECTED_ERR_COUNT_UNIQUE, regVal);

    // FILLUNIT
    regVal = GPU_REG_RD32(pGpu, NV_PFB_PRI_MMU_FILLUNIT_ECC_UNCORRECTED_ERR_COUNT);
    mmuCount += DRF_VAL(_PFB_PRI_MMU, _FILLUNIT_ECC, _UNCORRECTED_ERR_COUNT_UNIQUE, regVal);

    return mmuCount;
}
