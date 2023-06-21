/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "kernel/gpu/intr/swintr.h"
#include "gpu/gpu.h"
#include "kernel/gpu/intr/engine_idx.h"
#include "kernel/gpu/intr/intr.h"

/**
 * @brief Provides an opportunity to register some IntrService during intrStateInit.
 */
void
swintrRegisterIntrService_IMPL
(
    OBJGPU *pGpu,
    SwIntr *pSwIntr,
    IntrServiceRecord pRecords[MC_ENGINE_IDX_MAX]
)
{
    NvU32 engineIdx = MC_ENGINE_IDX_CPU_DOORBELL;
    NV_ASSERT(pRecords[engineIdx].pInterruptService == NULL);
    pRecords[engineIdx].pInterruptService = staticCast(pSwIntr, IntrService);

    engineIdx = MC_ENGINE_IDX_PRIV_DOORBELL;
    NV_ASSERT(pRecords[engineIdx].pInterruptService == NULL);
    pRecords[engineIdx].pInterruptService = staticCast(pSwIntr, IntrService);
}

/**
 * @brief Service stall interrupts.
 *
 * @returns Zero, or any implementation-chosen nonzero value. If the same nonzero value is returned enough
 *          times the interrupt is considered stuck.
 */
NvU32
swintrServiceInterrupt_IMPL
(
    OBJGPU *pGpu,
    SwIntr *pSwIntr,
    IntrServiceServiceInterruptArguments *pParams
)
{
    NV_ASSERT_OR_RETURN(pParams != NULL, 0);

    switch (pParams->engineIdx)
    {
        case MC_ENGINE_IDX_CPU_DOORBELL:
        {
            MODS_ARCH_REPORT(NV_ARCH_EVENT_CPU_DOORBELL, "%s", "processing CPU doorbell interrupt\n");

            //
            // CPU_DOORBELL interrupt is used to notify a vGPU guest that there is an event pending.
            // The vGPU event itself will be picked up by vgpuGetPendingEvent()
            //
            return 0;
        }
        case MC_ENGINE_IDX_PRIV_DOORBELL:
        {
            MODS_ARCH_REPORT(NV_ARCH_EVENT_PRIV_DOORBELL, "%s", "processing PRIV doorbell interrupt\n");

            // Then service all virtual interrupts that may be pending
            intrServiceVirtual_HAL(pGpu, GPU_GET_INTR(pGpu));
            return 0;
        }
        default:
        {
            NV_ASSERT_FAILED("Invalid engineIdx");
            return 0;
        }
    }
}
