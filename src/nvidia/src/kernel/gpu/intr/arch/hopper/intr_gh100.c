/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

ct_assert(NV_CPU_INTR_STALL_SUBTREE_START   < NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF__SIZE_1);
ct_assert(NV_CPU_INTR_STALL_SUBTREE_LAST    < NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF__SIZE_1);
ct_assert(NV_CPU_INTR_STALL_SUBTREE_START   < NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF_EN_SET__SIZE_1);
ct_assert(NV_CPU_INTR_STALL_SUBTREE_LAST    < NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF_EN_SET__SIZE_1);
ct_assert(NV_CPU_INTR_STALL_SUBTREE_START   < NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF_EN_CLEAR__SIZE_1);
ct_assert(NV_CPU_INTR_STALL_SUBTREE_LAST    < NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF_EN_CLEAR__SIZE_1);

ct_assert(NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF__SIZE_1 == NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF_EN_SET__SIZE_1);
ct_assert(NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF__SIZE_1 == NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF_EN_CLEAR__SIZE_1);

/*!
 * @brief Gets the stall subtree end index
 */
NvU32
intrGetStallSubtreeLast_GH100(OBJGPU *pGpu, Intr *pIntr)
{
    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_SWRL_GRANULAR_LOCKING))
    {
        return NV_CPU_INTR_STALL_SUBTREE_LAST_SWRL;
    }
    else
    {
        return NV_CPU_INTR_STALL_SUBTREE_LAST;
    }
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

/*!
 * @brief Gets the mask of INTR_TOP that covers nonstall interrupts
 */
NvU32
intrGetIntrTopNonStallMask_GH100(OBJGPU *pGpu, Intr *pIntr)
{
    NvU32 nonStallMask;

    // Compile-time assert against the highest set bit that will be returned
    #define NV_CPU_INTR_NOSTALL_SUBTREE_HIGHEST NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_SUBTREE(5)

    ct_assert(NV_CPU_INTR_NOSTALL_SUBTREE_HIGHEST < NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF__SIZE_1);
    ct_assert(NV_CPU_INTR_NOSTALL_SUBTREE_HIGHEST < NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF_EN_SET__SIZE_1);
    ct_assert(NV_CPU_INTR_NOSTALL_SUBTREE_HIGHEST < NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_LEAF_EN_CLEAR__SIZE_1);

    nonStallMask = NVBIT32(NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_SUBTREE(0));

    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_SWRL_GRANULAR_LOCKING))
    {
        nonStallMask |= NVBIT32(NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_SUBTREE(5));
    }

    return nonStallMask;
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
