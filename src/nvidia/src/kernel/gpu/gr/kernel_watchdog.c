/*
 * SPDX-FileCopyrightText: Copyright (c) 2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "kernel/gpu/gr/kernel_watchdog.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"

#include "nvrm_registry.h"

static void _kwdtInitRegistryOverrides(OBJGPU *pGpu, KernelWatchdog *pKernelWatchdog);

/*!
 * @brief Construct kernel RC watchdog
 */
NV_STATUS
kwdtConstruct_IMPL
(
    KernelWatchdog *pKernelWatchdog,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pKernelWatchdog);

    if (IS_MIG_ENABLED(pGpu))
    {
        // TODO: Disable GR watchdog for MIG until it's fully ready
        pKernelWatchdog->watchdogState.flags |= WATCHDOG_FLAGS_DISABLED;
    }

    _kwdtInitRegistryOverrides(pGpu, pKernelWatchdog);

    return NV_OK;
}

/*!
 * @brief Destruct kernel RC watchdog
 */
void
kwdtDestruct_IMPL
(
    KernelWatchdog *pKernelWatchdog
)
{
    return;
}

static void
_kwdtInitRegistryOverrides
(
    OBJGPU   *pGpu,
    KernelWatchdog *pKernelWatchdog
)
{
    NvU32 data32 = 0;

    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_ENABLE_GR_WATCHDOG, &data32) == NV_OK)
    {
        if (data32 == NV_REG_STR_RM_ENABLE_GR_WATCHDOG_DISABLED)
        {
            pKernelWatchdog->watchdogState.flags |= WATCHDOG_FLAGS_DISABLED;
        }
        else if (data32 == NV_REG_STR_RM_ENABLE_GR_WATCHDOG_ENABLED)
        {
            pKernelWatchdog->watchdogState.flags &= ~WATCHDOG_FLAGS_DISABLED;
        }
    }
    else if (IS_EMULATION(pGpu) || IS_SIMULATION(pGpu))
    {
        pKernelWatchdog->watchdogState.flags |= WATCHDOG_FLAGS_DISABLED;
    }
    else if (gpuIsCCFeatureEnabled(pGpu))
    {
        pKernelWatchdog->watchdogState.flags |= WATCHDOG_FLAGS_DISABLED;
    }
}