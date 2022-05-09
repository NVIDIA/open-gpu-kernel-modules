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

#include "os/os.h"
#include "gpu/gpu.h"
#include "gpu/hwpm/profiler_v1.h"

NV_STATUS
profilerConstruct_IMPL
(
    Profiler *pProfiler,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    if (profilerIsProfilingPermitted_HAL(pProfiler))
    {
        return profilerConstructState_HAL(pProfiler, pCallContext, pParams);
    }

    return NV_ERR_INSUFFICIENT_PERMISSIONS;
}

NvBool
profilerIsProfilingPermitted_IMPL
(
    Profiler *pProfiler
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pProfiler);

    if (gpuIsRmProfilingPrivileged(pGpu) && !osIsAdministrator())
    {
        return NV_FALSE;
    }

    return NV_TRUE;
}

NV_STATUS profilerControl_IMPL
(
    Profiler *pProfiler,
    CALL_CONTEXT *pCallContext,
    RS_RES_CONTROL_PARAMS_INTERNAL *pParams
)
{
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, profilerControlHwpmSupported_HAL(pProfiler, pParams));

    return gpuresControl_IMPL(staticCast(pProfiler, GpuResource),
                              pCallContext, pParams);
}
