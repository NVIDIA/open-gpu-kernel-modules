/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "published/maxwell/gm107/dev_boot.h"

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
    KernelDisplay *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
    NvU8 i;

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

    NV_ASSERT_OK_OR_RETURN(intrGetPendingStallEngines_HAL(pGpu, pIntr, pEngines, pThreadState));

    if (!API_GPU_ATTACHED_SANITY_CHECK(pGpu))
    {
        return NV_ERR_GPU_IS_LOST;
    }

    if (IS_VIRTUAL(pGpu) && vgpuGetPendingEvent(pGpu, pThreadState))
        bitVectorSet(pEngines, MC_ENGINE_IDX_VGPU);

    if (pKernelDisplay != NULL && kdispGetDeferredVblankHeadMask(pKernelDisplay))
    {
        // Deferred vblank is pending which we need to handle
        bitVectorSet(pEngines, MC_ENGINE_IDX_DISP);
    }

    if ((pKernelGraphicsManager != NULL) && (fecsGetCtxswLogConsumerCount(pGpu, pKernelGraphicsManager) > 0))
    {
        //
        // WARNING: This loop must not call any GR HALs or
        //          access any PGRAPH registers
        //
        for (i = 0; i < GPU_MAX_GRS; i++)
        {
            KernelGraphics *pKernelGraphics = GPU_GET_KERNEL_GRAPHICS(pGpu, i);
            if ((pKernelGraphics != NULL) &&
                kgraphicsIsIntrDrivenCtxswLoggingEnabled(pGpu, pKernelGraphics) &&
                fecsIsIntrPending(pGpu, pKernelGraphics))
            {
                bitVectorSet(pEngines, MC_ENGINE_IDX_GRn_FECS_LOG(i));
            }
        }
    }

    return NV_OK;
}

/*!
 * @brief Returns a bitfield with only MC_ENGINE_IDX_DISP set if it's pending
 *        Pre-Turing, there's only one stall interrupt register, so intrGetPendingStall_HAL
 *        will only read one register, and there's no perf concern in calling it
 *
 * @param[in]  pGpu
 * @param[in]  pMc
 * @param[out] pEngines     List of engines that have pending stall interrupts
 * @param[in]  pThreadState
 *
 * @return NV_OK if the list of engines that have pending stall interrupts was retrieved
 */
NV_STATUS
intrGetPendingDisplayIntr_GM107
(
    OBJGPU              *pGpu,
    Intr                *pIntr,
    PMC_ENGINE_BITVECTOR pEngines,
    THREAD_STATE_NODE   *pThreadState
)
{
    MC_ENGINE_BITVECTOR intr0Pending;

    bitVectorClrAll(pEngines);
    bitVectorClrAll(&intr0Pending);

    intrGetPendingStall_HAL(pGpu, pIntr, &intr0Pending, pThreadState);

    if (bitVectorTest(&intr0Pending, MC_ENGINE_IDX_DISP))
    {
        bitVectorSet(pEngines, MC_ENGINE_IDX_DISP);
    }

    return NV_OK;
}
