/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/ce/kernel_ce.h"
#include "kernel/gpu/gr/kernel_graphics.h"
#include "kernel/gpu/fifo/kernel_fifo.h"
#include "kernel/gpu/intr/engine_idx.h"
#include "os/os.h"
#include "rmapi/event.h"
#include "vgpu/rpc.h"
#include "vgpu/vgpu_events.h"

#include "published/turing/tu102/dev_vm.h"


/*! Get the current non-stall interrupt enable status. */
NvU32
intrGetNonStallEnable_TU102
(
    OBJGPU            *pGpu,
    Intr              *pIntr,
    THREAD_STATE_NODE *pThreadState
)
{
    NvU64 nonStallMask;
    NvU32 maskLo;
    NvU32 maskHi;

    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu))
    {
        return INTERRUPT_TYPE_DISABLED;
    }

    nonStallMask = intrGetIntrTopNonStallMask_HAL(pGpu, pIntr);
    maskLo = NvU64_LO32(nonStallMask);
    maskHi = NvU64_HI32(nonStallMask);

    //
    // We rely on boolean operator short-circuiting here to avoid reading a
    // TopEnSet register if not strictly necessary.
    //
    if ((maskLo != 0 &&
         (maskLo & intrReadRegTopEnSet_HAL(pGpu, pIntr, 0, pThreadState)) != 0)
        ||
        (maskHi != 0 &&
         (maskHi & intrReadRegTopEnSet_HAL(pGpu, pIntr, 1, pThreadState)) != 0))
        {
            //
        // If any top-level subtree corresponding to non-stall interrupts is
        // enabled, return that non-stall interrupts are enabled. We only
        // support enabling or disabling all non-stall interrupts at once, not a
        // subset.
        // Note that INTERRUPT_TYPE_MULTI basically means that all kinds of
        // non-stall interrupts are enabled. The legacy pre-Pascal code had
        // support to only enable software-triggerable interrupts or
        // hardware-triggerable interrupts or both. We're just continuing to the
        // same enum, but the naming of MULTI in the new interrupt tree warrants
        // some explanation, hence the detailed comment.
            //
            return INTERRUPT_TYPE_MULTI;
        }

    return INTERRUPT_TYPE_DISABLED;
}


/*! Enable all nonstall interrupts in dev_ctrl at top level. */
void
intrEnableTopNonstall_TU102
(
    OBJGPU            *pGpu,
    Intr              *pIntr,
    THREAD_STATE_NODE *pThreadState
)
{
    NvU64 nonStallMask;

    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu))
    {
        return;
    }

    nonStallMask = intrGetIntrTopNonStallMask_HAL(pGpu, pIntr);

    if (NvU64_LO32(nonStallMask) != 0)
    {
        intrWriteRegTopEnSet_HAL(pGpu, pIntr,
                                 0,
                                 NvU64_LO32(nonStallMask),
                                 pThreadState);
    }
    if (NvU64_HI32(nonStallMask) != 0)
    {
        intrWriteRegTopEnSet_HAL(pGpu, pIntr,
                                 1,
                                 NvU64_HI32(nonStallMask),
                                 pThreadState);
    }
}

/*! Disable all nonstall interrupts in dev_ctrl at top level. */
void
intrDisableTopNonstall_TU102
(
    OBJGPU            *pGpu,
    Intr              *pIntr,
    THREAD_STATE_NODE *pThreadState
)
{
    NvU64 nonStallMask;

    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu))
    {
        return;
    }

    nonStallMask = intrGetIntrTopNonStallMask_HAL(pGpu, pIntr);

    if (NvU64_LO32(nonStallMask) != 0)
    {
        intrWriteRegTopEnClear_HAL(pGpu, pIntr,
                                   0,
                                   NvU64_LO32(nonStallMask),
                                   pThreadState);
    }
    if (NvU64_HI32(nonStallMask) != 0)
    {
        intrWriteRegTopEnClear_HAL(pGpu, pIntr,
                                   1,
                                   NvU64_HI32(nonStallMask),
                                   pThreadState);
    }
}

void
intrRestoreNonStall_TU102
(
    OBJGPU  *pGpu,
    Intr *pIntr,
    NvU32    intrEn1,
    THREAD_STATE_NODE *pThreadState
)
{
    if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_ALTERNATE_TREE_ENABLED))
    {
        return;
    }

    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu))
    {
        return;
    }

    //
    // We only support enabling all non-stall interrupts or disabling all
    // non-stall interrupts in one go. We don't have a usecase to enable/disable
    // some of them selectively.
    //
    if (intrEn1 == 0)
    {
        intrDisableTopNonstall_HAL(pGpu, pIntr, pThreadState);
    }
    else
    {
        intrEnableTopNonstall_HAL(pGpu, pIntr, pThreadState);
    }
}

/*!
 * @brief get a bitvector of engines with pending nonstall interrupts
 *
 * @param[in]   pGpu          OBJGPU pointer
 * @param[out]  pEngines      bitvector of engines that have pending interrupts
 * @param[in]   pThreadState  thread state node pointer
 */
NV_STATUS
intrGetPendingNonStall_TU102
(
    OBJGPU              *pGpu,
    Intr                *pIntr,
    PMC_ENGINE_BITVECTOR pEngines,
    THREAD_STATE_NODE   *pThreadState
)
{
    KernelFifo     *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
    InterruptTable *pIntrTable;
    NvU32           i, j;
    NvU32           pending;
    NvU32           intrVector;

    NV_ASSERT_OR_RETURN(pEngines != NULL, NV_ERR_INVALID_ARGUMENT);

    bitVectorClrAll(pEngines);

    if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_ALTERNATE_TREE_ENABLED))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    //
    // If the GPU is in GC6 (aka powered down or rail-gated state), return
    // early from this routine. We don't want to touch any GPU registers while
    // its powered off as it will hang the system
    //
    if (IS_GPU_GC6_STATE_ENTERED(pGpu))
    {
        return NV_ERR_GPU_NOT_FULL_POWER;
    }

    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    NV_ASSERT_OK_OR_RETURN(intrGetInterruptTable_HAL(pGpu, pIntr, &pIntrTable));

    FOR_EACH_INDEX_IN_MASK(64, i, intrGetIntrTopNonStallMask_HAL(pGpu, pIntr))
    {
        j = NV_CTRL_INTR_SUBTREE_TO_TOP_IDX(i);
        // TODO Bug 3823562: optimize this and don't read on every loop
        pending = intrReadRegTop_HAL(pGpu, pIntr, j, pThreadState);

        if ((pending & (NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_SUBTREE_INTR_PENDING << NV_CTRL_INTR_SUBTREE_TO_TOP_BIT(i))) == 0)
        {
            continue;
        }

        for (j = NV_CTRL_INTR_SUBTREE_TO_LEAF_IDX_START(i); j <= NV_CTRL_INTR_SUBTREE_TO_LEAF_IDX_END(i); j++)
        {
            InterruptTableIter iter;
            NvU32 intr = intrReadRegLeaf_HAL(pGpu, pIntr, j, pThreadState) &
                         intrReadRegLeafEnSet_HAL(pGpu, pIntr, j, pThreadState);
            if (intr == 0)
            {
                continue;
            }

            for (iter = vectIterAll(pIntrTable); vectIterNext(&iter);)
            {
                INTR_TABLE_ENTRY *pEntry = iter.pValue;
                NvU32 intrVector = pEntry->intrVectorNonStall;

                if (intrVector == NV_INTR_VECTOR_INVALID)
                {
                    // This engine does not have a valid nonstall interrupt vector
                    continue;
                }

                if (intrVector < NV_CTRL_INTR_LEAF_IDX_TO_GPU_VECTOR_START(j) ||
                    intrVector > NV_CTRL_INTR_LEAF_IDX_TO_GPU_VECTOR_END(j))
                {
                    // This engine's interrupt vector isn't part of this leaf anyway
                    continue;
                }
                if (intr & NVBIT(NV_CTRL_INTR_GPU_VECTOR_TO_LEAF_BIT(intrVector)))
                {
                    bitVectorSet(pEngines, pEntry->mcEngine);
                    //
                    // Don't break here. We can have multiple non stall
                    // interrupts on the same vector in case of MIG
                    // partitioning.
                    //
                    // Although most of them will be GFID filtered to VFs, the
                    // PF also uses engines assigned to a partition.
                    //
                }
            }
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

    // check for FIFO non stall interrupt
    if (!kfifoIsHostEngineExpansionSupported(pKernelFifo) &&
        !IS_GSP_CLIENT(pGpu) && !IS_VIRTUAL(pGpu))
    {
        intrVector = intrGetVectorFromEngineId(pGpu, pIntr, MC_ENGINE_IDX_FIFO, NV_TRUE);
        if (intrVector != NV_INTR_VECTOR_INVALID)
        {
            NvU32 intr;
            NvU32 leafIdx = NV_CTRL_INTR_GPU_VECTOR_TO_LEAF_REG(intrVector);

            intr = intrReadRegLeaf_HAL(pGpu, pIntr, leafIdx, pThreadState) &
                   intrReadRegLeafEnSet(pGpu, pIntr, leafIdx, pThreadState) &
                   NVBIT(NV_CTRL_INTR_GPU_VECTOR_TO_LEAF_BIT(intrVector));
            if (intr != 0)
            {
                bitVectorSet(pEngines, MC_ENGINE_IDX_FIFO);
            }
        }
    }

    return NV_OK;
}

/*!
 * @brief Given an interrupt status from a dev_ctrl LEAF register and the LEAF
 *        index, service the pending interrupts
 *
 * @param[in]   pGpu          OBJGPU pointer
 * @param[in]   pIntr         Intr pointer
 * @param[in]   intr          intr status from the intrLeafIdx'th leaf register
 * @param[in]   intrLeafIdx   index of the leaf register
 * @param[in]   pThreadState  thread state node pointer
 */
static NV_STATUS
_intrServiceNonStallLeaf_TU102
(
    OBJGPU              *pGpu,
    Intr                *pIntr,
    NvU32                intr,
    NvU32                intrLeafIdx,
    PMC_ENGINE_BITVECTOR pEngines,
    THREAD_STATE_NODE   *pThreadState
)
{
    InterruptTable    *pIntrTable;
    NV_STATUS          status = NV_OK;
    NV_STATUS          tmpStatus;
    InterruptTableIter iter;
    NvU16              mcEngineIdx;

    // Don't clear the bitvector pEngines since caller accumulates
    NV_ASSERT_OK_OR_RETURN(intrGetInterruptTable_HAL(pGpu, pIntr, &pIntrTable));

    for (iter = vectIterAll(pIntrTable); vectIterNext(&iter);)
    {
        INTR_TABLE_ENTRY *pEntry     = iter.pValue;
        NvU32             intrVector = pEntry->intrVectorNonStall;
        NvU32             intrPending;

        if (intrVector == NV_INTR_VECTOR_INVALID)
        {
            // This engine does not have a valid nonstall interrupt vector
            continue;
        }

        if (intrVector < NV_CTRL_INTR_LEAF_IDX_TO_GPU_VECTOR_START(intrLeafIdx) ||
            intrVector > NV_CTRL_INTR_LEAF_IDX_TO_GPU_VECTOR_END(intrLeafIdx))
        {
            // This engine's interrupt vector isn't part of this leaf anyway
            continue;
        }

        intrPending = intr & NVBIT(NV_CTRL_INTR_GPU_VECTOR_TO_LEAF_BIT(intrVector));
        if (!intrPending)
        {
            continue;
        }

        mcEngineIdx = pEntry->mcEngine;
        bitVectorSet(pEngines, mcEngineIdx);

        //
        // clear the latched state in dev_ctrl and then call the servicing
        // routine. If the servicing routine is unable to serve this interrupt,
        // then we'll throw a breakpoint but march on (interrupt would be
        // cleared so we wouldn't get ourselves into an interrupt storm unless
        // it fires again).
        //
        intrClearLeafVector_HAL(pGpu, pIntr, intrVector, pThreadState);
        if (NV_OK != (tmpStatus = intrServiceNotificationRecords(pGpu, pIntr,
                                                                 mcEngineIdx,
                                                                 pThreadState)))
        {
            NV_PRINTF(LEVEL_ERROR, "Could not service nonstall interrupt from "
                "mcEngineIdx %d. NV_STATUS = 0x%x\n", mcEngineIdx, tmpStatus);
            DBG_BREAKPOINT();
            status = tmpStatus;
        }
    }
    return status;
}

void
intrDisableNonStall_TU102
(
    OBJGPU            *pGpu,
    Intr              *pIntr,
    THREAD_STATE_NODE *pThreadState
)
{
    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_ALTERNATE_TREE_ENABLED))
    {
        intrDisableTopNonstall_HAL(pGpu, pIntr, pThreadState);
    }
}

NV_STATUS
intrServiceNonStall_TU102
(
    OBJGPU              *pGpu,
    Intr                *pIntr,
    PMC_ENGINE_BITVECTOR pEngines,
    THREAD_STATE_NODE   *pThreadState
)
{
    KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
    NV_STATUS   status      = NV_OK;
    NV_STATUS   tmpStatus   = NV_OK;
    NvU32       i, pending;
    NvU32       intr, intrVector;

    bitVectorClrAll(pEngines);

    if (!API_GPU_ATTACHED_SANITY_CHECK(pGpu))
    {
        return NV_ERR_GPU_IS_LOST;
    }

    if (API_GPU_IN_RESET_SANITY_CHECK(pGpu))
    {
        return NV_ERR_GPU_IN_FULLCHIP_RESET;
    }

    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu))
    {
        return NV_ERR_NOT_SUPPORTED;
    }
    FOR_EACH_INDEX_IN_MASK(64, i, intrGetIntrTopNonStallMask_HAL(pGpu, pIntr))
    {
        NvU32 j = NV_CTRL_INTR_SUBTREE_TO_TOP_IDX(i);
        // TODO Bug 3823562: optimize this and don't read on every loop
        pending = intrReadRegTop_HAL(pGpu, pIntr, j, pThreadState);

        if ((pending & (NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_SUBTREE_INTR_PENDING << NV_CTRL_INTR_SUBTREE_TO_TOP_BIT(i))) == 0)
        {
            continue;
        }

        for (j = NV_CTRL_INTR_SUBTREE_TO_LEAF_IDX_START(i); j <= NV_CTRL_INTR_SUBTREE_TO_LEAF_IDX_END(i); j++)
        {
            intr = intrReadRegLeaf_HAL(pGpu, pIntr, j, pThreadState) &
                   intrReadRegLeafEnSet_HAL(pGpu, pIntr, j, pThreadState);
            if (intr == 0)
            {
                continue;
            }

            if ((tmpStatus = _intrServiceNonStallLeaf_TU102(pGpu, pIntr, intr, j, pEngines, pThreadState)) != NV_OK)
            {
                NV_ASSERT_OK_FAILED("Could not service nonstall interrupt leafs", tmpStatus);
                status = tmpStatus;
            }
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

    //
    // Special handling for the FIFO "non-stall" interrupt that does not report
    // into the non-stall subtree (unfortunately, by HW design) ane hence, is
    // not handled above.
    //
    // This is a physical-only interrupt
    //
    if (!kfifoIsHostEngineExpansionSupported(pKernelFifo) &&
        !IS_GSP_CLIENT(pGpu) && !IS_VIRTUAL(pGpu))
    {
        intrVector = intrGetVectorFromEngineId(pGpu, pIntr, MC_ENGINE_IDX_FIFO, NV_TRUE);
        if (intrVector != NV_INTR_VECTOR_INVALID)
        {
            NvU32 leafIdx = NV_CTRL_INTR_GPU_VECTOR_TO_LEAF_REG(intrVector);

            intr = intrReadRegLeaf_HAL(pGpu, pIntr, leafIdx, pThreadState) &
                   intrReadRegLeafEnSet_HAL(pGpu, pIntr, leafIdx, pThreadState) &
                   NVBIT(NV_CTRL_INTR_GPU_VECTOR_TO_LEAF_BIT(intrVector));
            if (intr != 0)
            {
                if ((tmpStatus = _intrServiceNonStallLeaf_TU102(pGpu, pIntr, intr, leafIdx, pEngines, pThreadState)) != NV_OK)
                {
                    NV_ASSERT_OK_FAILED("Could not service FIFO 'non-stall' intr", tmpStatus);
                    status = tmpStatus;
                }
            }
        }
    }

    //
    if (status != NV_OK)
    {
        DBG_BREAKPOINT();
    }
    return status;
}
