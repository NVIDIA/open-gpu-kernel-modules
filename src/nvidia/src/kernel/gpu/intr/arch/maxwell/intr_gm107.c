/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "core/locks.h"
#include "kernel/gpu/gr/kernel_graphics.h"
#include "kernel/gpu/intr/engine_idx.h"
#include "gpu/gsp/kernel_gsp.h"
#include "resserv/rs_server.h"
#include "vgpu/vgpu_events.h"
#include "gpu/disp/kern_disp.h"

/**
 * @brief Returns a bitfield with the MC_ENGINES that have pending interrupts
 */
NV_STATUS
intrGetPendingStall_GM107
(
    OBJGPU              *pGpu,
    Intr                *pIntr,
    MC_ENGINE_BITVECTOR *pEngines,
    THREAD_STATE_NODE   *pThreadState
)
{
    NV_ASSERT_OR_RETURN(pEngines != NULL, NV_ERR_INVALID_ARGUMENT);

    bitVectorClrAll(pEngines);

    //
    // If the GPU is in GC6 (aka powered down or rail-gated state), return
    // early from this routine. We don't want to touch any GPU registers while
    // its powered off as it will hang the system.
    //
    // If the GPU has fallen off the bus, there are obviously no interrupts
    // pending that we can do anything about, but attempting to read a status
    // register might indicate otherwise (returning 0xffffffff). Its better to do a GPU
    // sanity check after we read the status register and bail out if necessary.
    //
    if (IS_GPU_GC6_STATE_ENTERED(pGpu))
    {
        return NV_ERR_GPU_NOT_FULL_POWER;
    }

    NV_ASSERT_OK_OR_RETURN(intrGetPendingStallEngines_HAL(pGpu, pIntr, NV_FALSE, 0, pEngines, pThreadState));

    if (!API_GPU_ATTACHED_SANITY_CHECK(pGpu))
    {
        return NV_ERR_GPU_IS_LOST;
    }

    intrGetAuxiliaryPendingStall_HAL(pGpu, pIntr, pEngines, NV_TRUE, MC_ENGINE_IDX_NULL, pThreadState);

    return NV_OK;
}

/*!
 *
 * @param[in]  pGpu
 * @param[in]  pIntr
 * @param[out] pStaticRegVal unused on pre-Turing
 * @param[out] pEngines      List of engines that have pending stall interrupts
 * @param[in]  pThreadState
 *
 * @return NV_OK if the list of engines that have pending stall interrupts was retrieved
 */
NV_STATUS
intrGetPendingDispLowLatencyAndStaticReg_GM107
(
    OBJGPU              *pGpu,
    Intr                *pIntr,
    NvU32               *pStaticRegVal,
    MC_ENGINE_BITVECTOR *pEngines,
    THREAD_STATE_NODE   *pThreadState
)
{
    // Just get all the interrupts, there's not much to optimize for pre-Turing
    return intrGetPendingStall_HAL(pGpu, pIntr, pEngines, pThreadState);
}
