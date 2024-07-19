/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/timer/objtmr.h"
#include "os/os.h"

#include "published/ampere/ga100/dev_vm.h"
#include "published/ampere/ga100/dev_vm_addendum.h"

//
// intrga100.c - HAL routines for Ampere interrupt object
//


void
intrGetLocklessVectorsInRmSubtree_GA100
(
    OBJGPU *pGpu,
    Intr   *pIntr,
    NvU32 (*pInterruptVectors)[2]
)
{
    NvU32 i;
    for (i = 0; i < NV_ARRAY_ELEMENTS((*pInterruptVectors)); i++)
    {
        (*pInterruptVectors)[i] = NV_INTR_VECTOR_INVALID;
    }
    i = 0;

    //
    // The non-replayable fault interrupt is shared with the client, and in the
    // top half of the interrupt handler, as such, we only copy fault packets
    // from the HW buffer to the appropriate SW buffers.
    //
    //
    // On GSP we service non replayable faults in the bottom half, so we
    // shouldn't mask them
    //
    NV_ASSERT(i < NV_ARRAY_ELEMENTS((*pInterruptVectors)));

    if (!IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu))
    {
        (*pInterruptVectors)[i] = intrGetVectorFromEngineId(pGpu, pIntr,
            MC_ENGINE_IDX_NON_REPLAYABLE_FAULT,
            NV_FALSE);
        i++;
    }

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
