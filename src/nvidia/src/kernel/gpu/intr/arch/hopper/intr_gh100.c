/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "os/os.h"

#include "published/hopper/gh100/dev_vm.h"
#include "published/hopper/gh100/dev_vm_addendum.h"

// Compile time asserts to make sure we don't write beyond the leaf register array

ct_assert(NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF__SIZE_1 == NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF_EN_SET__SIZE_1);
ct_assert(NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF__SIZE_1 == NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF_EN_CLEAR__SIZE_1);


NV_STATUS
intrInitSubtreeMap_GH100
(
    OBJGPU *pGpu,
    Intr   *pIntr
)
{
    NV2080_INTR_CATEGORY_SUBTREE_MAP *pCategoryEngine =
        &pIntr->subtreeMap[NV2080_INTR_CATEGORY_ESCHED_DRIVEN_ENGINE];
    pCategoryEngine->subtreeStart = NV_CPU_INTR_STALL_SUBTREE_START;
    pCategoryEngine->subtreeEnd   = NV_CPU_INTR_STALL_SUBTREE_START;

    NV2080_INTR_CATEGORY_SUBTREE_MAP *pCategoryEngineNotification =
        &pIntr->subtreeMap[NV2080_INTR_CATEGORY_ESCHED_DRIVEN_ENGINE_NOTIFICATION];
    pCategoryEngineNotification->subtreeStart = NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_SUBTREE(0);
    pCategoryEngineNotification->subtreeEnd   = NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_SUBTREE(0);

    NV2080_INTR_CATEGORY_SUBTREE_MAP *pCategoryRunlistLocked =
        &pIntr->subtreeMap[NV2080_INTR_CATEGORY_RUNLIST];
    pCategoryRunlistLocked->subtreeStart = NV_CPU_INTR_STALL_SUBTREE_LAST_SWRL;
    pCategoryRunlistLocked->subtreeEnd   = NV_CPU_INTR_STALL_SUBTREE_LAST_SWRL;

    //
    // Don't reprogram NV_RUNLIST_INTR_TREE_LOCKLESS (runlist notification)
    // vectors to a different subtree than their init values in Hopper unlike
    // Ampere. INTR_TOP is covered with a "nonstall" mask corresponding to these
    // leaf ranges.
    // It is okay to reprogram the vectors for individual runlists within the
    // same subtree.
    //
    NV2080_INTR_CATEGORY_SUBTREE_MAP *pCategoryRunlistNotification =
        &pIntr->subtreeMap[NV2080_INTR_CATEGORY_RUNLIST_NOTIFICATION];
    pCategoryRunlistNotification->subtreeStart = NV_CPU_INTR_STALL_SUBTREE_LAST;
    pCategoryRunlistNotification->subtreeEnd   = NV_CPU_INTR_STALL_SUBTREE_LAST;

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
intrGetNumLeaves_GH100(OBJGPU *pGpu, Intr *pIntr)
{
    ct_assert((NV_CTRL_INTR_SUBTREE_TO_LEAF_IDX_END(NV_CPU_INTR_STALL_SUBTREE_LAST) + 1) <= NV_MAX_INTR_LEAVES);
    return (NV_CTRL_INTR_SUBTREE_TO_LEAF_IDX_END(NV_CPU_INTR_STALL_SUBTREE_LAST) + 1);
}

/*!
 * @brief Gets the value of VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF__SIZE_1
 */
NvU32
intrGetLeafSize_GH100(OBJGPU *pGpu, Intr *pIntr)
{
    return NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF__SIZE_1;
}


NvU64
intrGetIntrTopNonStallMask_GH100
(
    OBJGPU *pGpu,
    Intr   *pIntr
)
{
    // TODO Bug 3823562 Remove all these asserts
    // Compile-time assert against the highest set bit that will be returned
    #define NV_CPU_INTR_NOSTALL_SUBTREE_HIGHEST NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_SUBTREE(5)

    ct_assert(NV_CPU_INTR_NOSTALL_SUBTREE_HIGHEST < NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF__SIZE_1);
    ct_assert(NV_CPU_INTR_NOSTALL_SUBTREE_HIGHEST < NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF_EN_SET__SIZE_1);
    ct_assert(NV_CPU_INTR_NOSTALL_SUBTREE_HIGHEST < NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF_EN_CLEAR__SIZE_1);


    NvU64 mask = 0;
    mask |= intrGetIntrTopCategoryMask(pIntr,
        NV2080_INTR_CATEGORY_ESCHED_DRIVEN_ENGINE_NOTIFICATION);

    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_SWRL_GRANULAR_LOCKING))
    {
        mask |= intrGetIntrTopCategoryMask(pIntr,
            NV2080_INTR_CATEGORY_RUNLIST_NOTIFICATION);
    }

    // Sanity check that Intr.subtreeMap is initialized
    NV_ASSERT(mask != 0);
    return mask;
}


/*!
 * @brief Sanity check that the given stall engine interrupt vector is in the right tree
 *
 * @param[in]   pGpu          OBJGPU pointer
 * @param[in]   pIntr         Intr pointer
 * @param[in]   vector        stall interrupt vector
 */
void
intrSanityCheckEngineIntrStallVector_GH100
(
    OBJGPU  *pGpu,
    Intr *pIntr,
    NvU32    vector,
    NvU16    mcEngine
)
{
    //
    // The leafIndex must be within the engine stall tree: leaf 6 and 7 on Hopper.
    // Try to catch this on pre-release drivers. Don't need this on release drivers since this
    // is only to catch issues during code development. Should never happen in practice.
    //
    if ((NV_CTRL_INTR_GPU_VECTOR_TO_LEAF_REG(vector) != 6) &&
        (NV_CTRL_INTR_GPU_VECTOR_TO_LEAF_REG(vector) != 7))
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
intrSanityCheckEngineIntrNotificationVector_GH100
(
    OBJGPU  *pGpu,
    Intr    *pIntr,
    NvU32    vector,
    NvU16    mcEngine
)
{
    //
    // The leafIndex must be within the engine notification tree: leaf 0 and 1 on Hopper.
    // Try to catch this on pre-release drivers. Don't need this on release drivers since this
    // is only to catch issues during code development. Should never happen in practice.
    //
    if ((NV_CTRL_INTR_GPU_VECTOR_TO_LEAF_REG(vector) != 0) &&
        (NV_CTRL_INTR_GPU_VECTOR_TO_LEAF_REG(vector) != 1))
    {
        NV_PRINTF(LEVEL_ERROR, "MC_ENGINE_IDX %u has invalid notification intr vector %u\n", mcEngine, vector);
        DBG_BREAKPOINT();
    }
}
