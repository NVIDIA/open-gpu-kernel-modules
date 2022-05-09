/*
 * SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "gpu/gpu.h"
#include "rmapi/rs_utils.h"
#include "gpu/hwpm/profiler_v2.h"
#include "ctrl/ctrlb0cc/ctrlb0ccinternal.h"

NV_STATUS
profilerBaseCtrlCmdAllocPmaStream_IMPL
(
    ProfilerBase *pProfiler,
    NVB0CC_CTRL_ALLOC_PMA_STREAM_PARAMS *pParams
)
{
    NV_STATUS status                          = NV_OK;
    OBJGPU   *pGpu                            = GPU_RES_GET_GPU(pProfiler);
    RM_API   *pRmApi                          = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NvHandle  hClient                         = RES_GET_CLIENT_HANDLE(pProfiler);
    NvHandle  hParent                         = RES_GET_PARENT_HANDLE(pProfiler);
    NvHandle  hObject                         = RES_GET_HANDLE(pProfiler);
    NvBool    bMemPmaBufferRegistered         = NV_FALSE;
    NvBool    bMemPmaBytesAvailableRegistered = NV_FALSE;

    //
    // REGISTER  MEMDESCs TO GSP
    // These are no-op with BareMetal/No GSP
    //
    status = memdescRegisterToGSP(pGpu, hClient, hParent, pParams->hMemPmaBuffer);
    if (status != NV_OK)
    {
        goto fail;
    }
    bMemPmaBufferRegistered = NV_TRUE;

    status = memdescRegisterToGSP(pGpu, hClient, hParent, pParams->hMemPmaBytesAvailable);
    if (status != NV_OK)
    {
        goto fail;
    }
    bMemPmaBytesAvailableRegistered = NV_TRUE;

    //
    // With BareMetal/No GSP: this control is a direct call to
    // profilerBaseCtrlCmdInternalReleaseHwpmLegacy_IMPL
    //
    status = pRmApi->Control(pRmApi,
                             hClient,
                             hObject,
                             NVB0CC_CTRL_CMD_INTERNAL_ALLOC_PMA_STREAM,
                             pParams, sizeof(*pParams));
    if (status != NV_OK)
    {
        goto fail;
    }

    return status;

fail:
    if (bMemPmaBufferRegistered)
    {
        // These are no-op with BareMetal/No GSP
        NV_ASSERT_OK(memdescDeregisterFromGSP(pGpu, hClient, hParent, pParams->hMemPmaBuffer));
    }

    if (bMemPmaBytesAvailableRegistered)
    {
        // These are no-op with BareMetal/No GSP
        NV_ASSERT_OK(memdescDeregisterFromGSP(pGpu, hClient, hParent, pParams->hMemPmaBytesAvailable));
    }

    return status;
}
