/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "kernel/gpu/rc/kernel_rc.h"
#include "kernel/gpu/subdevice/subdevice.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"


NV_STATUS
subdeviceCtrlCmdRcGetWatchdogInfo_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_RC_GET_WATCHDOG_INFO_PARAMS *pWatchdogInfoParams
)
{
    OBJGPU   *pGpu      = GPU_RES_GET_GPU(pSubdevice);
    KernelRc *pKernelRc = GPU_GET_KERNEL_RC(pGpu);

    portMemSet(pWatchdogInfoParams, 0, sizeof *pWatchdogInfoParams);

    if (pKernelRc->watchdog.flags & WATCHDOG_FLAGS_INITIALIZED)
    {
        pWatchdogInfoParams->watchdogStatusFlags |=
            NV2080_CTRL_RC_GET_WATCHDOG_INFO_FLAGS_INITIALIZED;
    }

    if (pKernelRc->watchdog.flags & WATCHDOG_FLAGS_DISABLED)
    {
        pWatchdogInfoParams->watchdogStatusFlags |=
            NV2080_CTRL_RC_GET_WATCHDOG_INFO_FLAGS_DISABLED;
    }

    if ((pKernelRc->watchdog.flags & WATCHDOG_FLAGS_INITIALIZED) &&
        !(pKernelRc->watchdog.flags & WATCHDOG_FLAGS_DISABLED) &&
        gpuIsGpuFullPower(pGpu))
    {
        pWatchdogInfoParams->watchdogStatusFlags |=
            NV2080_CTRL_RC_GET_WATCHDOG_INFO_FLAGS_RUNNING;
    }

    return NV_OK;
}

NV_STATUS
subdeviceCtrlCmdRcDisableWatchdog_IMPL
(
    Subdevice *pSubdevice
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    // Watchdog not supported while SMC is active
    NV_CHECK_OR_RETURN(LEVEL_INFO, !IS_MIG_ENABLED(pGpu), NV_ERR_NOT_SUPPORTED);

    return krcWatchdogChangeState(GPU_GET_KERNEL_RC(pGpu),
                                  pSubdevice,
                                  RMAPI_DISABLE_REQUEST);
}

NV_STATUS
subdeviceCtrlCmdRcSoftDisableWatchdog_IMPL
(
    Subdevice *pSubdevice
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    // Watchdog not supported while SMC is active
    NV_CHECK_OR_RETURN(LEVEL_INFO, !IS_MIG_ENABLED(pGpu), NV_ERR_NOT_SUPPORTED);

    return krcWatchdogChangeState(GPU_GET_KERNEL_RC(pGpu),
                                  pSubdevice,
                                  RMAPI_SOFT_DISABLE_REQUEST);
}

NV_STATUS
subdeviceCtrlCmdRcEnableWatchdog_IMPL
(
    Subdevice *pSubdevice
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    // Watchdog not supported while SMC is active
    NV_CHECK_OR_RETURN(LEVEL_INFO, !IS_MIG_ENABLED(pGpu), NV_ERR_NOT_SUPPORTED);

    return krcWatchdogChangeState(GPU_GET_KERNEL_RC(pGpu),
                                  pSubdevice,
                                  RMAPI_ENABLE_REQUEST);
}

NV_STATUS
subdeviceCtrlCmdRcReleaseWatchdogRequests_IMPL
(
    Subdevice *pSubdevice
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    // Watchdog not supported while SMC is active
    NV_CHECK_OR_RETURN(LEVEL_INFO, !IS_MIG_ENABLED(pGpu), NV_ERR_NOT_SUPPORTED);

    return krcWatchdogChangeState(GPU_GET_KERNEL_RC(pGpu),
                                  pSubdevice,
                                  RMAPI_RELEASE_ALL_REQUESTS);
}
