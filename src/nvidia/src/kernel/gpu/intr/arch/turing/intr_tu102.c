/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "kernel/gpu/intr/intr.h"
#include "gpu/gpu.h"
#include "kernel/gpu/intr/engine_idx.h"
#include "gpu/bif/kernel_bif.h"
#include "gpu/timer/objtmr.h"
#include "gpu/uvm/uvm.h"
#include "os/os.h"
#include "vgpu/vgpu_events.h"
#include "vgpu/rpc.h"
#include "gpu/mmu/kern_gmmu.h"
#include "libraries/nvport/nvport.h"
#include "gpu/disp/kern_disp.h"

#include "published/turing/tu102/dev_ctrl.h"
#include "published/turing/tu102/dev_vm.h"
#include "published/turing/tu102/dev_vm_addendum.h"

//
// Static interface functions
//
static NvU32     _intrGetUvmLeafMask_TU102(OBJGPU *, Intr *);
static void      _intrEnableStall_TU102(OBJGPU *, Intr *, THREAD_STATE_NODE *pThreadState);
static void      _intrDisableStall_TU102(OBJGPU *, Intr *, THREAD_STATE_NODE *pThreadState);
static void      _intrClearLeafEnables_TU102(OBJGPU *pGpu, Intr *pIntr);

// Compile time asserts to make sure we don't write beyond the leaf register array

ct_assert(NV_CPU_INTR_STALL_SUBTREE_START   < NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF__SIZE_1);
ct_assert(NV_CPU_INTR_STALL_SUBTREE_LAST    < NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF__SIZE_1);
ct_assert(NV_CPU_INTR_STALL_SUBTREE_START   < NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF_EN_SET__SIZE_1);
ct_assert(NV_CPU_INTR_STALL_SUBTREE_LAST    < NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF_EN_SET__SIZE_1);
ct_assert(NV_CPU_INTR_STALL_SUBTREE_START   < NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF_EN_CLEAR__SIZE_1);
ct_assert(NV_CPU_INTR_STALL_SUBTREE_LAST    < NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF_EN_CLEAR__SIZE_1);

ct_assert(NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF__SIZE_1 == NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF_EN_SET__SIZE_1);
ct_assert(NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF__SIZE_1 == NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF_EN_CLEAR__SIZE_1);

//
// Few design issues and intentions stated upfront:
// Q: Why are interrupts being enabled/ disabled here instead of their respective HALs?
// A: The intent is to keep the "alternate tree" (nonstall tree) invisible from the rest of RM.
//
// Q: Then how does a HAL register its interrupts on this alternate tree?
// A: It does not. The alternate tree is an aberration of nature meant to service **non stall interrupts**
//    without using locking.
//
// Q: If the alternate tree does not respect locks taken by osAcquireRmSema then how do we prevent
//    race conditions?
// A: We dont!! The plan here is to *manually* inspect every piece of code that gets executed on the ISR/DPC
//    for this tree and make sure concurrent actions from elsewhere do not lead us in an inconsistent state.
//    In future before adding code to this tree, **carefully inspect it yourself**.
//
// A final note, if and when RM gets fine grained locks in the main interrupt tree, it might be worthwhile
// getting rid of this. More code is more complexity!!
//
NV_STATUS
intrStateLoad_TU102
(
    OBJGPU *pGpu,
    Intr   *pIntr,
    NvU32   flags
)
{
    NV_STATUS status = NV_OK;
    InterruptTable    *pIntrTable;
    InterruptTableIter iter;

    NV_ASSERT_OK_OR_RETURN(intrGetInterruptTable_HAL(pGpu, pIntr, &pIntrTable));

    //
    // Make sure all leaf nodes are disabled before we enable them.  Older drivers
    // and versions of mods leave them enabled.  Bug 3299004.
    //
    _intrClearLeafEnables_TU102(pGpu, pIntr);

    //
    // Enable interrupts either in legacy NV_PMC_INTR tree or new NV_CTRL tree
    // as per the MC interrupt vector table.
    //
    // We have to make an exception for the TMR engine though, since for now,
    // it reports into both PMC and dev_ctrl. We need the PTIMER alarm in
    // PMC, which is the only place where it reports, and we need it in
    // dev_ctrl for the countdown/callback timer, which we use in the PF
    // and all the VFs
    //
    pGpu->pmcRmOwnsIntrMask = INTERRUPT_MASK_DISABLED;
    for (iter = vectIterAll(pIntrTable); vectIterNext(&iter);)
    {
        INTR_TABLE_ENTRY *pEntry = iter.pValue;
        if (pEntry->pmcIntrMask != NV_PMC_INTR_INVALID_MASK)
        {
            pGpu->pmcRmOwnsIntrMask |= pEntry->pmcIntrMask;

            if (pEntry->mcEngine != MC_ENGINE_IDX_TMR)
                continue;
        }

        if (pEntry->intrVector != NV_INTR_VECTOR_INVALID)
        {
            intrEnableLeaf_HAL(pGpu, pIntr, pEntry->intrVector);
        }

        if ((pEntry->intrVectorNonStall != NV_INTR_VECTOR_INVALID)
            )
        {
            intrEnableLeaf_HAL(pGpu, pIntr, pEntry->intrVectorNonStall);
        }
    }

    status = intrCacheIntrFields_HAL(pGpu, pIntr);
    if (status != NV_OK)
    {
        goto exit;
    }

exit:
    if (pIntr->getProperty(pIntr, PDB_PROP_INTR_ENABLE_DETAILED_LOGS))
    {
        intrDumpState_HAL(pGpu, pIntr);
    }

    return status;
}

NV_STATUS
intrStateUnload_TU102
(
    OBJGPU  *pGpu,
    Intr *pIntr,
    NvU32    flags
)
{
    // Disable all interrupts since we're unloading

    intrWriteRegTopEnClear_HAL(pGpu, pIntr, 0, 0xFFFFFFFF, NULL);
    intrWriteRegTopEnClear_HAL(pGpu, pIntr, 1, 0xFFFFFFFF, NULL);

    _intrClearLeafEnables_TU102(pGpu, pIntr);

    return NV_OK;
}

/*!
 * @brief Cache few Intr fields for ease of use in interrupt or RPC context.
 *
 * @param[in]   pGpu    OBJGPU pointer
 * @param[in]   pIntr   Intr pointer
 */
NV_STATUS
intrCacheIntrFields_TU102
(
    OBJGPU            *pGpu,
    Intr              *pIntr
)
{
    NV_STATUS status = NV_OK;

    //
    // Cache the CPU_INTR_TOP_EN mask to clear when disabling stall
    // interrupts (other interrupts are either not disabled or disabled
    // selectively at leaf level)
    //
    pIntr->intrTopEnMask |= intrGetIntrTopLockedMask(pGpu, pIntr);

    // Cache client owned, shared interrupt, and display vectors for ease of use later
    pIntr->accessCntrIntrVector      = intrGetVectorFromEngineId(pGpu, pIntr, MC_ENGINE_IDX_ACCESS_CNTR, NV_FALSE);

    if (!gpuIsCCFeatureEnabled(pGpu) || !gpuIsGspOwnedFaultBuffersEnabled(pGpu))
    {
        pIntr->replayableFaultIntrVector = intrGetVectorFromEngineId(pGpu, pIntr, MC_ENGINE_IDX_REPLAYABLE_FAULT, NV_FALSE);
    }
    else
    {
        pIntr->replayableFaultIntrVector = intrGetVectorFromEngineId(pGpu, pIntr, MC_ENGINE_IDX_REPLAYABLE_FAULT_CPU, NV_FALSE);
    }

    intrCacheDispIntrVectors_HAL(pGpu, pIntr);

    //
    // Ensure that both UVM vectors are in the same leaf register (check right
    // now so we don't have to check later in latency critical paths where this
    // is assumed to be true)
    //
    if (pIntr->replayableFaultIntrVector != NV_INTR_VECTOR_INVALID && pIntr->accessCntrIntrVector != NV_INTR_VECTOR_INVALID)
    {
        if (NV_CTRL_INTR_GPU_VECTOR_TO_LEAF_REG(pIntr->replayableFaultIntrVector) != NV_CTRL_INTR_GPU_VECTOR_TO_LEAF_REG(pIntr->accessCntrIntrVector))
        {
            NV_PRINTF(LEVEL_ERROR, "UVM interrupt vectors for replayable fault 0x%x "
                "and access counter 0x%x are in different CPU_INTR_LEAF registers\n",
                pIntr->replayableFaultIntrVector, pIntr->accessCntrIntrVector);
            DBG_BREAKPOINT();
            status = NV_ERR_GENERIC;
            goto exit;
        }
    }

    {
        //
        // Now ensure that they're in the expected subtree (check right now so
        // we don't have to check later in latency critical paths where this is
        // assumed to be true)
        //
        NV2080_INTR_CATEGORY_SUBTREE_MAP uvmOwned;
        NvU32 accessCntrSubtree = NV_CTRL_INTR_GPU_VECTOR_TO_SUBTREE(
            pIntr->accessCntrIntrVector);
        NV_ASSERT_OK_OR_RETURN(
            intrGetSubtreeRange(pIntr,
                                NV2080_INTR_CATEGORY_UVM_OWNED,
                                &uvmOwned));
        if (!(uvmOwned.subtreeStart <= accessCntrSubtree &&
              accessCntrSubtree     <= uvmOwned.subtreeEnd))
        {
            NV_PRINTF(LEVEL_ERROR,
                "UVM owned interrupt vector for access counter is in an unexpected subtree\n"
                "Expected range = [0x%x, 0x%x], actual = 0x%x\n",
                uvmOwned.subtreeStart, uvmOwned.subtreeEnd, accessCntrSubtree);
            DBG_BREAKPOINT();
            status = NV_ERR_GENERIC;
            goto exit;
        }
    }

    {
        NV2080_INTR_CATEGORY_SUBTREE_MAP uvmShared;
        NV_ASSERT_OK_OR_RETURN(
            intrGetSubtreeRange(pIntr,
                                NV2080_INTR_CATEGORY_UVM_SHARED,
                                &uvmShared));
        //
        // Assert to make sure we have only one client shared subtree.
        // The below code assumes that.
        //
        NV_ASSERT_OR_RETURN(uvmShared.subtreeStart == uvmShared.subtreeEnd,
                            NV_ERR_INVALID_STATE);

        // Now cache the leaf enable mask for the subtree shared with the client
        NvU32 leafEnHi = intrReadRegLeafEnSet_HAL(pGpu, pIntr,
            NV_CTRL_INTR_SUBTREE_TO_LEAF_IDX_START(uvmShared.subtreeStart),
            NULL);
        NvU32 leafEnLo = intrReadRegLeafEnSet_HAL(pGpu, pIntr,
            NV_CTRL_INTR_SUBTREE_TO_LEAF_IDX_END(uvmShared.subtreeStart),
            NULL);

        pIntr->uvmSharedCpuLeafEn = ((NvU64)(leafEnHi) << 32) | leafEnLo;
        pIntr->uvmSharedCpuLeafEnDisableMask =
            intrGetUvmSharedLeafEnDisableMask(pGpu, pIntr);
    }

exit:

    return status;
}

void intrCacheDispIntrVectors_TU102(OBJGPU *pGpu, Intr *pIntr)
{
    KernelDisplay *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);

    if (pKernelDisplay != NULL && !RMCFG_FEATURE_PLATFORM_GSP)
    {
        pIntr->displayIntrVector           = intrGetVectorFromEngineId(pGpu, pIntr, MC_ENGINE_IDX_DISP, NV_FALSE);
    }
    else
    {
        pIntr->displayIntrVector           = NV_INTR_VECTOR_INVALID;
    }
    pIntr->displayLowLatencyIntrVector = NV_INTR_VECTOR_INVALID;
}

/*!
 * @brief Get the base interrupt vector to use when indexing engine stall
 *        interrupts
 *
 * @param[in]   pGpu    OBJGPU pointer
 * @param[in]   pIntr   Intr pointer
 *
 * @returns  the base interrupt vector for engine stall interrupts
 */
NvU32
intrGetStallBaseVector_TU102
(
    OBJGPU     *pGpu,
    Intr       *pIntr
)
{
    NvU32 base = GPU_REG_RD32(pGpu, NV_CTRL_LEGACY_ENGINE_STALL_INTR_BASE_VECTORID);
    return base;
}

/*!
 * @brief Enable a given interrupt vector in dev_ctrl at leaf level
 *
 * @param[in]   pGpu          OBJGPU pointer
 * @param[in]   pIntr         Intr pointer
 * @param[in]   intrVector    nonstall interrupt vector to enable
 */
void
intrEnableLeaf_TU102
(
    OBJGPU     *pGpu,
    Intr       *pIntr,
    NvU32       intrVector
)
{
    NvU32 reg = NV_CTRL_INTR_GPU_VECTOR_TO_LEAF_REG(intrVector);
    NvU32 leafBit = NV_CTRL_INTR_GPU_VECTOR_TO_LEAF_BIT(intrVector);
    NvU32 intrLeafEnSetSize = intrGetLeafSize_HAL(pGpu, pIntr);

    if (reg >= intrLeafEnSetSize)
    {
        NV_PRINTF(LEVEL_ERROR, "Exceeding the range of INTR leaf registers. "
            "intrVector = 0x%x, Reg = 0x%x\n", intrVector, reg);
        NV_ASSERT(0);
        return;
    }

    intrWriteRegLeafEnSet_HAL(pGpu, pIntr, reg, NVBIT(leafBit), NULL);
}

/*!
 * @brief Disable a given interrupt vector in dev_ctrl at leaf level
 *
 * @param[in]   pGpu          OBJGPU pointer
 * @param[in]   pIntr         Intr pointer
 * @param[in]   intrVector    nonstall interrupt vector to enable
 */
void
intrDisableLeaf_TU102
(
    OBJGPU     *pGpu,
    Intr       *pIntr,
    NvU32       intrVector
)
{
    NvU32 reg = NV_CTRL_INTR_GPU_VECTOR_TO_LEAF_REG(intrVector);
    NvU32 leafBit = NV_CTRL_INTR_GPU_VECTOR_TO_LEAF_BIT(intrVector);
    NvU32 intrLeafEnClearSize = intrGetLeafSize_HAL(pGpu, pIntr);

    if (reg >= intrLeafEnClearSize)
    {
        NV_PRINTF(LEVEL_ERROR, "Exceeding the range of INTR leaf registers. "
            "intrVector = 0x%x, Reg = 0x%x\n", intrVector, reg);
        NV_ASSERT(0);
        return;
    }

    intrWriteRegLeafEnClear_HAL(pGpu, pIntr, reg, NVBIT(leafBit), NULL);
}

/*!
 * @brief Disable/Enable stall interrupts in dev_ctrl
 *
 * @param[in]   pGpu          OBJGPU pointer
 * @param[in]   pIntr         Intr pointer
 * @param[in]   pThreadState  thread state node pointer
 */
void
intrSetStall_TU102
(
    OBJGPU            *pGpu,
    Intr              *pIntr,
    NvU32              intrType,
    THREAD_STATE_NODE *pThreadState
)
{
    // dev_ctrl tree is not used for legacy-vGPU
    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu))
    {
        return;
    }

    if (intrType == INTERRUPT_TYPE_DISABLED)
    {
        _intrDisableStall_TU102(pGpu, pIntr, pThreadState);
    }
    else
    {
        _intrEnableStall_TU102(pGpu, pIntr, pThreadState);
    }
}

/*!
 * @brief Clear all interrupt leaf nodes
 *
 * @param[in]   pGpu          OBJGPU pointer
 * @param[in]   pIntr         Intr pointer
 */
static void _intrClearLeafEnables_TU102
(
    OBJGPU  *pGpu,
    Intr *pIntr
)
{
    NvU32 i;
    NvU32 intrLeafSize = intrGetLeafSize_HAL(pGpu, pIntr);

    for (i = 0; i < intrLeafSize; i++)
    {
        intrWriteRegLeafEnClear_HAL(pGpu, pIntr, i, 0xFFFFFFFF, NULL);
    }
}

/*!
 * @brief Enable all stall interrupts in dev_ctrl
 *
 * @param[in]   pGpu          OBJGPU pointer
 * @param[in]   pIntr         Intr pointer
 * @param[in]   pThreadState  thread state node pointer
 */
static void
_intrEnableStall_TU102
(
    OBJGPU            *pGpu,
    Intr              *pIntr,
    THREAD_STATE_NODE *pThreadState
)
{
    NvU32 idx;

    //
    // 1. Enable the UVM interrupts that RM currently owns at INTR_LEAF
    // level.
    //
    NvU32 val = _intrGetUvmLeafMask_TU102(pGpu, pIntr);
    idx = NV_CTRL_INTR_GPU_VECTOR_TO_LEAF_REG(pIntr->accessCntrIntrVector);
    if (val != 0)
    {
        intrWriteRegLeafEnSet_HAL(pGpu, pIntr, idx, val, pThreadState);
    }

    //
    // 2. Enable all interrupts in the client shared subtree at INTR_LEAF
    // level, based on the cached value.
    //

    {
        NV2080_INTR_CATEGORY_SUBTREE_MAP uvmShared;
        NV_ASSERT_OK(intrGetSubtreeRange(pIntr,
                                         NV2080_INTR_CATEGORY_UVM_SHARED,
                                         &uvmShared));
        //
        // Assert to make sure we have only one client shared subtree.
        // The below code assumes that.
        //
        NV_ASSERT(uvmShared.subtreeStart == uvmShared.subtreeEnd);
        idx = uvmShared.subtreeStart;
    }

    if (NvU64_HI32(pIntr->uvmSharedCpuLeafEn) != 0)
    {
        intrWriteRegLeafEnSet_HAL(pGpu, pIntr,
                                  NV_CTRL_INTR_SUBTREE_TO_LEAF_IDX_START(idx),
                                  NvU64_HI32(pIntr->uvmSharedCpuLeafEn),
                                  pThreadState);
    }
    if (NvU64_LO32(pIntr->uvmSharedCpuLeafEn) != 0)
    {
        intrWriteRegLeafEnSet_HAL(pGpu, pIntr,
                                  NV_CTRL_INTR_SUBTREE_TO_LEAF_IDX_END(idx),
                                  NvU64_LO32(pIntr->uvmSharedCpuLeafEn),
                                  pThreadState);
    }

    // We use the assumption that 1 == ENABLE below
    ct_assert(NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_EN_SET_SUBTREE_ENABLE == 1);

    {
        //
        // 3. Enable all interrupt subtrees (except nonstall) at top level.
        // Nonstall enablement is handled by a different function.
        //
        NvU64 mask = NV_U64_MAX;

        mask &= ~intrGetIntrTopNonStallMask_HAL(pGpu, pIntr);

        if (NvU64_LO32(mask) != 0)
        {
            intrWriteRegTopEnSet_HAL(pGpu, pIntr,
                                     0,
                                     NvU64_LO32(mask),
                                     pThreadState);
        }
        if (NvU64_HI32(mask) != 0)
        {
            intrWriteRegTopEnSet_HAL(pGpu, pIntr,
                                     1,
                                     NvU64_HI32(mask),
                                     pThreadState);
        }
    }
}

/*!
 * @brief Disable all stall interrupts in dev_ctrl
 *
 * @param[in]   pGpu          OBJGPU pointer
 * @param[in]   pIntr         Intr pointer
 * @param[in]   pThreadState  thread state node pointer
 */
static void
_intrDisableStall_TU102
(
    OBJGPU            *pGpu,
    Intr              *pIntr,
    THREAD_STATE_NODE *pThreadState
)
{
    NvU32 idx;

    NvU32 val;

    // 1. Disable the UVM interrupts that RM currently owns at INTR_LEAF level
    idx = NV_CTRL_INTR_GPU_VECTOR_TO_LEAF_REG(pIntr->accessCntrIntrVector);
    val = _intrGetUvmLeafMask_TU102(pGpu, pIntr);
    if (val != 0)
    {
        intrWriteRegLeafEnClear_HAL(pGpu, pIntr, idx, val, pThreadState);
    }

    //
    // 2. Disable all interrupts in the client shared subtree at INTR_LEAF
    // level, except the ones that can be handled outside the GPU lock.
    //

    {
        NV2080_INTR_CATEGORY_SUBTREE_MAP uvmShared;
        NV_ASSERT_OK(intrGetSubtreeRange(pIntr,
                                         NV2080_INTR_CATEGORY_UVM_SHARED,
                                         &uvmShared));
        //
        // Assert to make sure we have only one client shared subtree.
        // The below code assumes that.
        //
        NV_ASSERT(uvmShared.subtreeStart == uvmShared.subtreeEnd);
        idx = uvmShared.subtreeStart;
    }

    if (!gpuIsStateLoaded(pGpu))
    {
        //
        // If GPU state load has not finished, there is nothing we can or want to
        // do here, since our cached state of interrupt vectors isn't valid yet
        // anyway.
        //
        intrWriteRegLeafEnClear_HAL(pGpu, pIntr,
                                    NV_CTRL_INTR_SUBTREE_TO_LEAF_IDX_START(idx),
                                    0xFFFFFFFF, pThreadState);
        intrWriteRegLeafEnClear_HAL(pGpu, pIntr,
                                    NV_CTRL_INTR_SUBTREE_TO_LEAF_IDX_END(idx),
                                    0xFFFFFFFF, pThreadState);
    }
    else
    {
        if ((NvU64_HI32(pIntr->uvmSharedCpuLeafEnDisableMask) != 0) &&
            (NvU64_HI32(pIntr->uvmSharedCpuLeafEn) != 0))
        {
            //
            // Only write to the register is both the enable mask and the
            // disable mask is non-zero. If there are no interrupts we're
            // interested in handling in one of the leafs, the enable mask will
            // be zero and the disable mask will be all 0xFs. There's no point
            // writing the register in that case since interrupts are already
            // not enabled. Using the cached value helps us avoid a register
            // read in latency critical paths.
            //
            intrWriteRegLeafEnClear_HAL(pGpu, pIntr,
                                        NV_CTRL_INTR_SUBTREE_TO_LEAF_IDX_START(idx),
                                        NvU64_HI32(pIntr->uvmSharedCpuLeafEnDisableMask),
                                        pThreadState);
        }
        if ((NvU64_LO32(pIntr->uvmSharedCpuLeafEnDisableMask) != 0) &&
            (NvU64_LO32(pIntr->uvmSharedCpuLeafEn) != 0))
        {
            //
            // Only write to the register is both the enable mask and the
            // disable mask is non-zero. If there are no interrupts we're
            // interested in handling in one of the leafs, the enable mask will
            // be zero and the disable mask will be all 0xFs. There's no point
            // writing the register in that case since interrupts are already
            // not enabled. Using the cached value helps us avoid a register
            // read in latency critical paths.
            //
            intrWriteRegLeafEnClear_HAL(pGpu, pIntr,
                                        NV_CTRL_INTR_SUBTREE_TO_LEAF_IDX_END(idx),
                                        NvU64_LO32(pIntr->uvmSharedCpuLeafEnDisableMask),
                                        pThreadState);
        }
    }

    //
    // 3. Disable some interrupt subtrees at top level (information about which
    // ones to disable is cached in pIntr->intrTopEnMask)
    //
    if (NvU64_LO32(pIntr->intrTopEnMask) != 0)
    {
        intrWriteRegTopEnClear_HAL(pGpu, pIntr,
                                   0,
                                   NvU64_LO32(pIntr->intrTopEnMask),
                                   pThreadState);
    }
    if (NvU64_HI32(pIntr->intrTopEnMask) != 0)
    {
        intrWriteRegTopEnClear_HAL(pGpu, pIntr,
                                   1,
                                   NvU64_HI32(pIntr->intrTopEnMask),
                                   pThreadState);
    }
}

/*!
 * @brief Clears a given interrupt vector at the dev_ctrl LEAF level
 *
 * @param[in]   pGpu          OBJGPU pointer
 * @param[in]   pIntr         Intr pointer
 * @param[in]   intrVector    interrupt vector to clear
 * @param[in]   pThreadState  thread state node pointer
 */
void
intrClearLeafVector_TU102
(
    OBJGPU            *pGpu,
    Intr              *pIntr,
    NvU32              intrVector,
    THREAD_STATE_NODE *pThreadState
)
{
    NvU32 reg = NV_CTRL_INTR_GPU_VECTOR_TO_LEAF_REG(intrVector);
    NvU32 bit = NV_CTRL_INTR_GPU_VECTOR_TO_LEAF_BIT(intrVector);

    intrWriteRegLeaf_HAL(pGpu, pIntr, reg, NVBIT(bit), pThreadState);
}

/*!
 * @brief Checks if the given interrupt vector is pending at the dev_ctrl LEAF level
 *
 * @param[in]   pGpu          OBJGPU pointer
 * @param[in]   pIntr         Intr pointer
 * @param[in]   intrVector    interrupt vector to check
 * @param[in]   pThreadState  thread state node pointer
 */
NvBool
intrIsVectorPending_TU102
(
    OBJGPU            *pGpu,
    Intr              *pIntr,
    NvU32              intrVector,
    THREAD_STATE_NODE *pThreadState
)
{
    NvU32 reg = NV_CTRL_INTR_GPU_VECTOR_TO_LEAF_REG(intrVector);
    NvU32 bit = NV_CTRL_INTR_GPU_VECTOR_TO_LEAF_BIT(intrVector);
    NvU32 val = intrReadRegLeaf_HAL(pGpu, pIntr, reg, pThreadState);

    if (val & NVBIT(bit))
        return NV_TRUE;
    return NV_FALSE;
}

/*!
* @brief Returns the INTR_LEAF mask for RM owned client interrupts.
*
* NOTE: Must be called after @intrStateLoad_TU102. This code assumes that the
* client owned interrupts are in the same leaf register. We would have checked
* whether that assumption is true in @intrStateLoad_TU102 and if it was
* violated, we'd have failed state load.
*/
static NvU32
_intrGetUvmLeafMask_TU102
(
    OBJGPU *pGpu,
    Intr *pIntr
)
{
    NvU32 val = 0;
    KernelGmmu *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
    if (pKernelGmmu != NULL)
    {
        NvBool bRmOwnsReplayableFault = !!(pKernelGmmu->uvmSharedIntrRmOwnsMask & RM_UVM_SHARED_INTR_MASK_MMU_REPLAYABLE_FAULT_NOTIFY);
        NvBool bRmOwnsAccessCntr      = !!(pKernelGmmu->uvmSharedIntrRmOwnsMask & RM_UVM_SHARED_INTR_MASK_HUB_ACCESS_COUNTER_NOTIFY);

        if (bRmOwnsReplayableFault)
        {
            val |= NVBIT(NV_CTRL_INTR_GPU_VECTOR_TO_LEAF_BIT(pIntr->replayableFaultIntrVector));
        }
        if (bRmOwnsAccessCntr)
        {
            val |= NVBIT(NV_CTRL_INTR_GPU_VECTOR_TO_LEAF_BIT(pIntr->accessCntrIntrVector));
        }
    }
    return val;
}

void
intrGetLocklessVectorsInRmSubtree_TU102
(
    OBJGPU *pGpu,
    Intr   *pIntr,
    NvU32  (*pInterruptVectors)[2]
)
{
    NvU32 i;
    for (i = 0; i < NV_ARRAY_ELEMENTS((*pInterruptVectors)); i++)
    {
        (*pInterruptVectors)[i] = NV_INTR_VECTOR_INVALID;
    }
    i = 0;

    NV_ASSERT(i < NV_ARRAY_ELEMENTS((*pInterruptVectors)));
    //
    // The non-replayable fault interrupt is shared with the client, and in the
    // top half of the interrupt handler, as such, we only copy fault packets
    // from the HW buffer to the appropriate SW buffers.
    //
    (*pInterruptVectors)[i] = intrGetVectorFromEngineId(pGpu, pIntr,
        MC_ENGINE_IDX_NON_REPLAYABLE_FAULT,
        NV_FALSE);
    i++;

    if (!IS_VIRTUAL(pGpu))
    {
        NV_ASSERT(i < NV_ARRAY_ELEMENTS((*pInterruptVectors)));
        //
        // The fifo non-stall interrupt is used for runlist events, which also
        // does not need to be blocked by the GPU lock (existing codepaths
        // already ascertain that this is safe, so we're maintaining that
        // behavior in NV_CTRL).
        //
        (*pInterruptVectors)[i] = intrGetVectorFromEngineId(pGpu, pIntr,
                                                            MC_ENGINE_IDX_FIFO,
                                                            NV_TRUE);
        i++;
    }
}


/*!
 * @brief Gets list of engines with pending stalling interrupts as per the interrupt trees
 *
 * @param[in]  pGpu
 * @param[in]  pIntr
 * @param[out] pEngines     List of engines that have pending stall interrupts
 * @param[in]  pThreadState
 *
 * @return NV_OK if the list of engines that have pending stall interrupts was retrieved
 */
NV_STATUS
intrGetPendingStallEngines_TU102
(
    OBJGPU              *pGpu,
    Intr                *pIntr,
    MC_ENGINE_BITVECTOR *pEngines,
    THREAD_STATE_NODE   *pThreadState
)
{
    KernelGmmu *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
    InterruptTable    *pIntrTable;
    InterruptTableIter iter;
    NvU64 sanityCheckSubtreeMask = 0;
    NvU32 numIntrLeaves = intrGetNumLeaves_HAL(pGpu, pIntr);
    NV_ASSERT(numIntrLeaves <= NV_MAX_INTR_LEAVES);
    NvU32 intrLeafValues[NV_MAX_INTR_LEAVES];

    sanityCheckSubtreeMask = intrGetIntrTopLegacyStallMask(pIntr);

    portMemSet(intrLeafValues, 0, numIntrLeaves * sizeof(NvU32));
    bitVectorClrAll(pEngines);

    // dev_ctrl tree is not used for legacy-vGPU
    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu))
    {
        return NV_OK;
    }

    NV_ASSERT_OK_OR_RETURN(intrGetLeafStatus_HAL(pGpu, pIntr, intrLeafValues, pThreadState));
    NV_ASSERT_OK_OR_RETURN(intrGetInterruptTable_HAL(pGpu, pIntr, &pIntrTable));

    for (iter = vectIterAll(pIntrTable); vectIterNext(&iter);)
    {
        INTR_TABLE_ENTRY *pEntry     = iter.pValue;
        NvU32             intrVector = pEntry->intrVector;
        NvU32             leaf;
        NvU32             leafIndex;
        NvU32             leafBit;

        // Check if this engine has a valid stalling interrupt vector in the new tree
        if (intrVector == NV_INTR_VECTOR_INVALID)
        {
            continue;
        }

        leafIndex = NV_CTRL_INTR_GPU_VECTOR_TO_LEAF_REG(intrVector);
        leafBit = NV_CTRL_INTR_GPU_VECTOR_TO_LEAF_BIT(intrVector);

        //
        // The leafIndex must be within the stall tree. Try to catch this on
        // pre-release drivers. Don't need this on release drivers since this
        // is only to catch issues during code development. Should never happen
        // in practice
        //

        if ((sanityCheckSubtreeMask &
             NVBIT64(NV_CTRL_INTR_LEAF_IDX_TO_SUBTREE(leafIndex))) == 0)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "MC_ENGINE_IDX %u has invalid stall intr vector %u\n",
                      pEntry->mcEngine,
                      intrVector);
            DBG_BREAKPOINT();
            continue;
        }
        //
        // Check if interrupt is pending. We skip checking if it is enabled in
        // the leaf register since we mess around with the leaf enables in
        // the interrupt disable path and will need special casing to handle it
        // In the transition period from NV_PMC to NV_CTRL, the interrupt vector
        // for engines that haven't yet switched would be INVALID, so we'd never
        // get here anyway.
        //
        leaf = intrLeafValues[leafIndex] & NVBIT(leafBit);

        if (leaf == 0)
        {
            continue;
        }

        // Add engine to bitvector
        bitVectorSet(pEngines, pEntry->mcEngine);
    }

    if (pKernelGmmu != NULL)
    {
        NvBool bRmOwnsReplayableFault = !!(pKernelGmmu->uvmSharedIntrRmOwnsMask & RM_UVM_SHARED_INTR_MASK_MMU_REPLAYABLE_FAULT_NOTIFY);
        NvBool bRmOwnsAccessCntr      = !!(pKernelGmmu->uvmSharedIntrRmOwnsMask & RM_UVM_SHARED_INTR_MASK_HUB_ACCESS_COUNTER_NOTIFY);

        //
        // Add non replayable fault engine if there is something in the shadow buffer,
        // as the interrupt itself is cleared earlier.
        //
        if (portAtomicOrS32(kgmmuGetFatalFaultIntrPendingState(pKernelGmmu, GPU_GFID_PF), 0))
        {
            bitVectorSet(pEngines, MC_ENGINE_IDX_NON_REPLAYABLE_FAULT);
        }

         // If AM doesn't own either UVM interrupt, remove it from the pending mask
        if (!bRmOwnsReplayableFault)
        {
            bitVectorClr(pEngines, MC_ENGINE_IDX_REPLAYABLE_FAULT);
        }

        if (!bRmOwnsAccessCntr)
        {
            bitVectorClr(pEngines, MC_ENGINE_IDX_ACCESS_CNTR);
        }
    }

    return NV_OK;
}

/*!
 * @brief Checks and services MMU non=replayable fault interrupts that may not
 * have been queued as DPC if we didn't get the GPU lock in the top half.
 *
 * If the MMU non-replayable fault interrupt was the only interrupt pending and
 * we were unable to get the GPU lock in the top half, a DPC would not have
 * been scheduled, but the non-replayable fault interrupt packet(s) would have
 * been copied into the SW buffers. Try to do the bottom-half servicing of
 * interrupts that could have been cleared in the top half.
 *
 * @param[in]   pGpu          OBJGPU pointer
 * @param[in]   pIntr         Intr pointer
 * @param[in]   pThreadState  THREAD_STATE_NODE pointer
 */
NV_STATUS
intrCheckAndServiceNonReplayableFault_TU102
(
    OBJGPU            *pGpu,
    Intr              *pIntr,
    THREAD_STATE_NODE *pThreadState
)
{
    NV_STATUS status = NV_OK;
    MC_ENGINE_BITVECTOR intrPending;

    intrGetPendingStall_HAL(pGpu, pIntr, &intrPending, pThreadState);

    if (bitVectorTest(&intrPending, MC_ENGINE_IDX_NON_REPLAYABLE_FAULT))
    {
        KernelGmmu *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);

        //
        // This interrupt was already cleared in the top half and "serviced" in
        // the top half since copy from HW fault buffer always happens in the
        // top half. This servicing is merely copying from the SW fault buffer,
        // so doesn't need interrupt clearing. Also, we will only copy from the
        // SW fault buffer if the fatalFaultIntrPending cache tells us that
        // there is something to copy. Else, we'll just return early and rely
        // on another interrupt to fire that will eventually update this state.
        // In the top half, we will unconditionally write GET back, which will
        // force HW to send us a new pulse as long as GET != PUT and we'd be
        // eventually guaranteed to copy something into the SW fault buffer.
        //
        if (portAtomicCompareAndSwapS32(kgmmuGetFatalFaultIntrPendingState(pKernelGmmu, GPU_GFID_PF), 0, 1))
        {
            status = kgmmuServiceNonReplayableFault_HAL(pGpu, pKernelGmmu);
            if (status != NV_OK)
            {
                NV_ASSERT_OK_FAILED("Failed to service MMU non-replayable fault", status);
            }
        }
    }
    return status;
}

/*!
 * @brief Retrigger interrupts by toggling enables of those subtrees not
 * toggled at top level in GPU lock acquire/release. Subtrees that are toggled
 * at top level will be implicitly re-armed when the GPU lock is released.
 *
 * @param[in]   pGpu        GPU Object
 * @param[in]   pIntr       Intr Object
 */
void
intrRetriggerTopLevel_TU102
(
    OBJGPU  *pGpu,
    Intr *pIntr
)
{
    NvU64 mask = 0;

    // We use the assumption that 1 == DISABLE below
    ct_assert(NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_EN_CLEAR_SUBTREE_DISABLE == 1);

    //
    // Toggle the top level interrupt enables for all interrupts whose top
    // level enables are not toggled during RM lock acquire/release.
    //
    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_ALTERNATE_TREE_HANDLE_LOCKLESS) &&
        pGpu->getProperty(pGpu, PDB_PROP_GPU_ALTERNATE_TREE_ENABLED))
    {
        //
        // 1. If the alternate tree (nonstall tree) is handled "lockless", it
        // is not disabled during RM lock acquire, so needs re-arming.
        //
        mask |= intrGetIntrTopNonStallMask_HAL(pGpu, pIntr);
    }

    // 2. UVM-owned interrupt tree (never disabled at top level)
    mask |= intrGetIntrTopCategoryMask(pIntr, NV2080_INTR_CATEGORY_UVM_OWNED);

    // 3. UVM/RM shared interrupt tree (never disabled at top level)
    mask |= intrGetIntrTopCategoryMask(pIntr, NV2080_INTR_CATEGORY_UVM_SHARED);

    //
    // Bypass GPU_REG_WR32 that requires the GPU lock to be held (for some
    // register filters) by using the OS interface directly.
    //
    // Clear all first, then set
    //
    if (NvU64_LO32(mask) != 0 &&
        0 < NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_EN_CLEAR__SIZE_1)
    {
        osGpuWriteReg032(pGpu,
            GPU_GET_VREG_OFFSET(pGpu,
                NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_EN_CLEAR(0)),
            NvU64_LO32(mask));
    }
    if (NvU64_HI32(mask) != 0 &&
        1 < NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_EN_CLEAR__SIZE_1)
    {
        osGpuWriteReg032(pGpu,
            GPU_GET_VREG_OFFSET(pGpu,
                NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_EN_CLEAR(1)),
            NvU64_HI32(mask));
    }
    if (NvU64_LO32(mask) != 0 &&
        0 < NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_EN_SET__SIZE_1)
    {
        osGpuWriteReg032(pGpu,
            GPU_GET_VREG_OFFSET(pGpu,
                NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_EN_SET(0)),
            NvU64_LO32(mask));
    }
    if (NvU64_HI32(mask) != 0 &&
        1 < NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_EN_SET__SIZE_1)
    {
        osGpuWriteReg032(pGpu,
            GPU_GET_VREG_OFFSET(pGpu,
                NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_EN_SET(1)),
            NvU64_HI32(mask));
    }
}

/*!
 * @brief read all leaf interrupt registers into an array
 *
 * @param[in]   pGpu       OBJGPU pointer
 * @param[in]   pIntr      Intr pointer
 * @param[out]  pLeafVals  array that leaf values will be returned in.
 *                         assumes that it is sufficiently large
 */
NV_STATUS
intrGetLeafStatus_TU102
(
    OBJGPU             *pGpu,
    Intr               *pIntr,
    NvU32              *pLeafVals,
    THREAD_STATE_NODE  *pThreadState
)
{
    NvU32 subtreeIndex;
    NvU32 leafIndex;

    FOR_EACH_INDEX_IN_MASK(64, subtreeIndex,
                           intrGetIntrTopLegacyStallMask(pIntr))
    {
        leafIndex = NV_CTRL_INTR_SUBTREE_TO_LEAF_IDX_START(subtreeIndex);
        if (pIntr->getProperty(pIntr, PDB_PROP_INTR_READ_ONLY_EVEN_NUMBERED_INTR_LEAF_REGS))
        {
            //
            // Since we know that on Turing, only one leaf per subtree has valid
            // interrupts, optimize to only read those leaf registers.
            //
            pLeafVals[leafIndex] = intrReadRegLeaf_HAL(pGpu, pIntr, leafIndex, pThreadState);
        }
        else
        {
            for (; leafIndex <= NV_CTRL_INTR_SUBTREE_TO_LEAF_IDX_END(subtreeIndex); leafIndex++)
            {
                pLeafVals[leafIndex] = intrReadRegLeaf_HAL(pGpu, pIntr, leafIndex, pThreadState);
            }
        }
    } FOR_EACH_INDEX_IN_MASK_END

    return NV_OK;
}

/*!
 * @brief Returns a bitfield with only MC_ENGINE_IDX_DISP set if it's pending in hardware
 *        On Turing+, there are multiple stall interrupt registers, and reading them
 *        all in the top half would be expensive. To saitsfy bug 3220319, only find out
 *        if display interrupt is pending. Fix this in bug 3279300.
 *        The MC_ENGINE_IDX_DISP that this function reports conflates both the low latency display
 *        interrupts and other display interrupts in architectures supported by this HAL.
 *
 * @param[in]  pGpu
 * @param[in]  pMc
 * @param[out] pEngines     List of engines that have pending stall interrupts
 * @param[in]  pThreadState
 *
 * @return NV_OK if the list of engines that have pending stall interrupts was retrieved
 */
NV_STATUS
intrGetPendingLowLatencyHwDisplayIntr_TU102
(
    OBJGPU              *pGpu,
    Intr                *pIntr,
    PMC_ENGINE_BITVECTOR pEngines,
    THREAD_STATE_NODE   *pThreadState
)
{
    bitVectorClrAll(pEngines);

    if (IS_GPU_GC6_STATE_ENTERED(pGpu))
    {
        return NV_ERR_GPU_NOT_FULL_POWER;
    }

    if (!API_GPU_ATTACHED_SANITY_CHECK(pGpu))
    {
        return NV_ERR_GPU_IS_LOST;
    }

    if (pIntr->displayIntrVector == NV_INTR_VECTOR_INVALID)
    {
        return NV_OK;
    }

    if (intrIsVectorPending_HAL(pGpu, pIntr, pIntr->displayIntrVector, pThreadState))
    {
        bitVectorSet(pEngines, MC_ENGINE_IDX_DISP);
    }

    return NV_OK;
}


/**
 * @brief Enable or disable the display interrupt.
 * This implements the missing functionality of PDB_PROP_INTR_USE_INTR_MASK_FOR_LOCKING
 * for Turing+: The ability to leave display interrrupts unmasked while the GPU lock is held
 * The PMC_INTR_MASK HW registers were deprecated in Pascal, but the Pascal-Volta interrupt
 * code still emulates them in SW. The Turing+ code did not implement any of the masking code,
 * but as seen in bug 3152190, the ability to leave the display interupt unmasked is still
 * needed. The ability to unmask the interrupts to enable them to show up in interrupt registers
 * is not needed, so this call is not needed at callsites that just do that
 * (_intrEnterCriticalSection / _intrExitCriticalSection)
 * This whole interrupts code mess needs refactored - bug 3279300
 *
 * @param[in] pGpu
 * @param[in] pIntr
 * @param[in] bEnable
 * @param[in] pThreadState - Needed for bypassing register filters in unlocked top half
 *
 */
void
intrSetDisplayInterruptEnable_TU102
(
    OBJGPU            *pGpu,
    Intr              *pIntr,
    NvBool             bEnable,
    THREAD_STATE_NODE *pThreadState
)
{
    if (pIntr->displayIntrVector == NV_INTR_VECTOR_INVALID)
    {
        return;
    }

    NvU32 reg = NV_CTRL_INTR_GPU_VECTOR_TO_LEAF_REG(pIntr->displayIntrVector);
    NvU32 leafBit = NV_CTRL_INTR_GPU_VECTOR_TO_LEAF_BIT(pIntr->displayIntrVector);

    if (bEnable)
    {
        intrWriteRegLeafEnSet_HAL(pGpu, pIntr, reg, NVBIT(leafBit), pThreadState);
    }
    else
    {
        intrWriteRegLeafEnClear_HAL(pGpu, pIntr, reg, NVBIT(leafBit), pThreadState);
    }
}

/*!
 * @brief Dumps interrupt state (registers, vector table) for debugging purpose.
 *
 * @param[in] pGpu   OBJGPU pointer
 * @param[in] pIntr  Intr pointer
 */
void
intrDumpState_TU102
(
    OBJGPU  *pGpu,
    Intr *pIntr
)
{
    InterruptTable    *pIntrTable;
    InterruptTableIter iter;
    NvU32              i;
    NvU32              intrLeafSize = intrGetLeafSize_HAL(pGpu, pIntr);

    NV_PRINTF(LEVEL_INFO, "Interrupt registers:\n");
    for (i = 0; i < NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP__SIZE_1; i++)
    {
        NV_PRINTF(LEVEL_INFO, "INTR_TOP_EN_SET(%u)=0x%x\n", i,
                  intrReadRegTopEnSet_HAL(pGpu, pIntr, i, NULL));
    }

    for (i = 0; i < intrLeafSize; i++)
    {
        NV_PRINTF(LEVEL_INFO, "INTR_LEAF_EN_SET(%u)=0x%x\n", i,
                  intrReadRegLeafEnSet_HAL(pGpu, pIntr, i, NULL));
    }

    NV_PRINTF(LEVEL_INFO, "MC Interrupt table:\n");
    intrGetInterruptTable_HAL(pGpu, pIntr, &pIntrTable);

    for (i = 0, iter = vectIterAll(pIntrTable); vectIterNext(&iter); i++)
    {
        INTR_TABLE_ENTRY *pEntry = iter.pValue;
        PORT_UNREFERENCED_VARIABLE(pEntry);

        NV_PRINTF(LEVEL_INFO,
            "%2u: mcEngineIdx=%-4u intrVector=%-10u intrVectorNonStall=%-10u\n",
            i,
            pEntry->mcEngine,
            pEntry->intrVector,
            pEntry->intrVectorNonStall);
    }
}


NV_STATUS
intrInitSubtreeMap_TU102
(
    OBJGPU *pGpu,
    Intr   *pIntr
)
{
    NV2080_INTR_CATEGORY_SUBTREE_MAP *pCategoryEngine =
        &pIntr->subtreeMap[NV2080_INTR_CATEGORY_ESCHED_DRIVEN_ENGINE];
    pCategoryEngine->subtreeStart = NV_CPU_INTR_STALL_SUBTREE_START;
    pCategoryEngine->subtreeEnd   = NV_CPU_INTR_STALL_SUBTREE_LAST;

    NV2080_INTR_CATEGORY_SUBTREE_MAP *pCategoryEngineNotification =
        &pIntr->subtreeMap[NV2080_INTR_CATEGORY_ESCHED_DRIVEN_ENGINE_NOTIFICATION];
    pCategoryEngineNotification->subtreeStart = NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_SUBTREE(0);
    pCategoryEngineNotification->subtreeEnd   = NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_SUBTREE(0);

    NV2080_INTR_CATEGORY_SUBTREE_MAP *pCategoryUvmOwned =
        &pIntr->subtreeMap[NV2080_INTR_CATEGORY_UVM_OWNED];
    pCategoryUvmOwned->subtreeStart = NV_CPU_INTR_UVM_SUBTREE_START;
    pCategoryUvmOwned->subtreeEnd   = NV_CPU_INTR_UVM_SUBTREE_LAST;

    NV2080_INTR_CATEGORY_SUBTREE_MAP *pCategoryUvmShared =
        &pIntr->subtreeMap[NV2080_INTR_CATEGORY_UVM_SHARED];
    pCategoryUvmShared->subtreeStart = NV_CPU_INTR_UVM_SHARED_SUBTREE_START;
    pCategoryUvmShared->subtreeEnd   = NV_CPU_INTR_UVM_SHARED_SUBTREE_LAST;

    return NV_OK;
}


/*!
 * @brief Gets the number of leaf registers used
 */
NvU32
intrGetNumLeaves_TU102(OBJGPU *pGpu, Intr *pIntr)
{
    ct_assert((NV_CTRL_INTR_SUBTREE_TO_LEAF_IDX_END(NV_CPU_INTR_STALL_SUBTREE_LAST) + 1) <= NV_MAX_INTR_LEAVES);
    return (NV_CTRL_INTR_SUBTREE_TO_LEAF_IDX_END(NV_CPU_INTR_STALL_SUBTREE_LAST) + 1);
}

/*!
 * @brief Gets the value of VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF__SIZE_1
 */
NvU32
intrGetLeafSize_TU102(OBJGPU *pGpu, Intr *pIntr)
{
    return NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF__SIZE_1;
}


NvU64
intrGetIntrTopNonStallMask_TU102
(
    OBJGPU *pGpu,
    Intr   *pIntr
)
{
    // TODO Bug 3823562 Remove these asserts
    // Compile-time assert against the highest set bit that will be returned
    #define NV_CPU_INTR_NOSTALL_SUBTREE_HIGHEST NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_SUBTREE(0)

    ct_assert(NV_CPU_INTR_NOSTALL_SUBTREE_HIGHEST < NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF__SIZE_1);
    ct_assert(NV_CPU_INTR_NOSTALL_SUBTREE_HIGHEST < NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF_EN_SET__SIZE_1);
    ct_assert(NV_CPU_INTR_NOSTALL_SUBTREE_HIGHEST < NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF_EN_CLEAR__SIZE_1);

    NvU64 mask = intrGetIntrTopCategoryMask(pIntr,
        NV2080_INTR_CATEGORY_ESCHED_DRIVEN_ENGINE_NOTIFICATION);

    //
    // On all Ampere+ that use this TU102 HAL, Esched notification interrupts
    // are also included in this if PDB_PROP_GPU_SWRL_GRANULAR_LOCKING is set.
    //

    // Sanity check that Intr.subtreeMap is initialized
    NV_ASSERT(mask != 0);
    return mask;
}


/*!
 * @brief Decode the interrupt mode for SW to use
 *
 * @param[in]   pIntr       Intr Object
 * @param[in]   intrEn      the enable value to decode
 *
 * @returns the value of the decoded interrupt
 *
 */
NvU32
intrDecodeStallIntrEn_TU102
(
    OBJGPU  *pGpu,
    Intr *pIntr,
    NvU32    intrEn
)
{
    // mask with interrupts that RM owns
    if (pGpu->pmcRmOwnsIntrMask != 0)
    {
        intrEn &= pGpu->pmcRmOwnsIntrMask;
    }

    switch (intrEn)
    {
        case INTERRUPT_MASK_DISABLED:
            return INTERRUPT_TYPE_DISABLED;
        case INTERRUPT_MASK_HARDWARE:
            return INTERRUPT_TYPE_HARDWARE;
        case INTERRUPT_MASK_SOFTWARE:
            return INTERRUPT_TYPE_SOFTWARE;
        default:
            return INTERRUPT_TYPE_MULTI;
    }
}
