/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "objtmr.h"
#include "os/os.h"

#include "published/ampere/ga100/dev_vm.h"
#include "published/ampere/ga100/dev_vm_addendum.h"

//
// intrga100.c - HAL routines for Ampere interrupt object
//

/*!
 * @brief Returns a 64 bit mask, where all the bits set to 0 are the ones we
 * intend to leave enabled in the client shared subtree even when we disable
 * interrupts (for example, when we take the GPU lock).
 *
 * The non-replayable fault interrupt is shared with the client, and in the
 * top half of the interrupt handler, as such, we only copy fault packets from
 * the HW buffer to the appropriate SW buffers.
 *
 * The timer interrupt also does not need to be blocked by the GPU lock
 * since SWRL granular locking requires the timer interrupt to be serviced
 * outside the GPU lock.
 * Note - While we keep the timer interrupt enabled during the GPU lock,
 * we don't enable it in the PTIMER level when SWRL granular locking is disabled.
 */
NvU64
intrGetUvmSharedLeafEnDisableMask_GA100
(
    OBJGPU  *pGpu,
    Intr *pIntr
)
{
    NvU32 intrVectorNonReplayableFault;
    NvU32 intrVectorTimerSwrl = NV_INTR_VECTOR_INVALID;
    NvU64 mask = 0;
    NV2080_INTR_CATEGORY_SUBTREE_MAP uvmShared;

    // GSP RM services both MMU non-replayable fault and FIFO interrupts with
    // vgpu plugin offload enabled. Otherwise, GSP RM only services FIFO interrupts
    if (IS_GSP_CLIENT(pGpu))
    {
        return ~mask;
    }

    intrVectorNonReplayableFault = intrGetVectorFromEngineId(pGpu, pIntr, MC_ENGINE_IDX_NON_REPLAYABLE_FAULT, NV_FALSE);

    if (!IS_VIRTUAL(pGpu))
    {
        intrVectorTimerSwrl = intrGetVectorFromEngineId(pGpu, pIntr, MC_ENGINE_IDX_TMR_SWRL, NV_FALSE);
    }

    if (intrVectorNonReplayableFault != NV_INTR_VECTOR_INVALID && intrVectorTimerSwrl != NV_INTR_VECTOR_INVALID)
    {
        // Ascertain that they're in the same subtree and same leaf
        NV_ASSERT(NV_CTRL_INTR_GPU_VECTOR_TO_SUBTREE(intrVectorNonReplayableFault) ==
                NV_CTRL_INTR_GPU_VECTOR_TO_SUBTREE(intrVectorTimerSwrl));
        NV_ASSERT(NV_CTRL_INTR_GPU_VECTOR_TO_LEAF_REG(intrVectorNonReplayableFault) ==
                NV_CTRL_INTR_GPU_VECTOR_TO_LEAF_REG(intrVectorTimerSwrl));
    }

    NV_ASSERT_OK(intrGetSubtreeRange(pIntr,
                                     NV2080_INTR_CATEGORY_UVM_SHARED,
                                     &uvmShared));
    //
    // Ascertain that we only have 1 client subtree (we assume this since we
    // cache only 64 bits).
    //
    NV_ASSERT(uvmShared.subtreeStart == uvmShared.subtreeEnd);

    //
    // Ascertain that we only have 2 subtrees as this is what we currently
    // support by only caching 64 bits
    //
    NV_ASSERT(
        (NV_CTRL_INTR_SUBTREE_TO_LEAF_IDX_END(uvmShared.subtreeEnd) - 1) ==
        NV_CTRL_INTR_SUBTREE_TO_LEAF_IDX_START(uvmShared.subtreeStart));

    // On GSP we service non replayable faults in the bottom half, so we shouldn't mask them
    if (intrVectorNonReplayableFault != NV_INTR_VECTOR_INVALID)
    {
        // Ascertain that it's in the first leaf
        NV_ASSERT(
            NV_CTRL_INTR_GPU_VECTOR_TO_LEAF_REG(intrVectorNonReplayableFault) ==
            NV_CTRL_INTR_SUBTREE_TO_LEAF_IDX_START(uvmShared.subtreeStart));

        mask |= NVBIT32(NV_CTRL_INTR_GPU_VECTOR_TO_LEAF_BIT(intrVectorNonReplayableFault));
    }

    if (intrVectorTimerSwrl != NV_INTR_VECTOR_INVALID)
    {
        // Ascertain that it's in the first leaf
        NV_ASSERT(
            NV_CTRL_INTR_GPU_VECTOR_TO_LEAF_REG(intrVectorTimerSwrl) ==
            NV_CTRL_INTR_SUBTREE_TO_LEAF_IDX_START(uvmShared.subtreeStart));

        mask |= NVBIT32(NV_CTRL_INTR_GPU_VECTOR_TO_LEAF_BIT(intrVectorTimerSwrl));
    }

    mask <<= 32;

    return ~mask;
}

/*!
 * @brief Sanity check that the given stall engine interrupt vector is in the right tree
 *
 * @param[in]   pGpu          OBJGPU pointer
 * @param[in]   pIntr         Intr pointer
 * @param[in]   vector        stall interrupt vector
 */
void
intrSanityCheckEngineIntrStallVector_GA100
(
    OBJGPU  *pGpu,
    Intr *pIntr,
    NvU32    vector,
    NvU16    mcEngine
)
{
    //
    // The leafIndex must be within the engine stall tree: leaf 6 on Ampere.
    // Try to catch this on pre-release drivers. Don't need this on release drivers since this
    // is only to catch issues during code development. Should never happen in practice.
    //
    if (NV_CTRL_INTR_GPU_VECTOR_TO_LEAF_REG(vector) != 6)
    {
        NV_PRINTF(LEVEL_ERROR, "MC_ENGINE_IDX %u has invalid stall intr vector %u\n", mcEngine, vector);
        DBG_BREAKPOINT();
    }
}

/*!
 * @brief Sanity check that the given notification engine interrupt vector is in the right tree
 *
 * @param[in]   pGpu          OBJGPU pointer
 * @param[in]   pIntr         Intr pointer
 * @param[in]   vector        stall interrupt vector
 */
void
intrSanityCheckEngineIntrNotificationVector_GA100
(
    OBJGPU  *pGpu,
    Intr *pIntr,
    NvU32    vector,
    NvU16    mcEngine
)
{
    //
    // The leafIndex must be within the engine notification tree: leaf 0 on Ampere.
    // Try to catch this on pre-release drivers. Don't need this on release drivers since this
    // is only to catch issues during code development. Should never happen in practice.
    //
    if(NV_CTRL_INTR_GPU_VECTOR_TO_LEAF_REG(vector) != 0)
    {
        NV_PRINTF(LEVEL_ERROR, "MC_ENGINE_IDX %u has invalid notification intr vector %u\n", mcEngine, vector);
        DBG_BREAKPOINT();
    }
}
