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
#include "nvoc/prelude.h"
#include "nvstatuscodes.h"
#include "rmapi/rs_utils.h"
#include "gpu/hwpm/profiler_v2.h"
#include "ctrl/ctrlb0cc/ctrlb0ccinternal.h"
#include "ctrl/ctrlb0cc/ctrlb0ccprofiler.h"
#include "mem_mgr/mem.h"

NV_STATUS
profilerBaseCtrlCmdFreePmaStream_IMPL
(
    ProfilerBase *pProfiler,
    NVB0CC_CTRL_FREE_PMA_STREAM_PARAMS *pParams
)
{
    RM_API            *pRmApi     = GPU_GET_PHYSICAL_RMAPI(GPU_RES_GET_GPU(pProfiler));
    NVB0CC_CTRL_INTERNAL_FREE_PMA_STREAM_PARAMS internalParams;

    portMemSet(&internalParams, 0, sizeof(NVB0CC_CTRL_INTERNAL_FREE_PMA_STREAM_PARAMS));
    internalParams.pmaChannelIdx = pParams->pmaChannelIdx;

    return pRmApi->Control(pRmApi,
                           RES_GET_CLIENT_HANDLE(pProfiler),
                           RES_GET_HANDLE(pProfiler),
                           NVB0CC_CTRL_CMD_INTERNAL_FREE_PMA_STREAM,
                           &internalParams, sizeof(internalParams));
}

NV_STATUS
profilerBaseCtrlCmdBindPmResources_IMPL
(
    ProfilerBase *pProfiler
)
{
    OBJGPU        *pGpu                       = GPU_RES_GET_GPU(pProfiler);
    RM_API        *pRmApi                     = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NvHandle       hClient                    = RES_GET_CLIENT_HANDLE(pProfiler);
    NvHandle       hObject                    = RES_GET_HANDLE(pProfiler);
    NV_STATUS      status                     = NV_OK;

    status = pRmApi->Control(pRmApi, hClient, hObject,
                             NVB0CC_CTRL_CMD_INTERNAL_BIND_PM_RESOURCES,
                             NULL, 0);
    return status;
}

NV_STATUS
profilerBaseCtrlCmdUnbindPmResources_IMPL
(
    ProfilerBase *pProfiler
)
{
    OBJGPU   *pGpu                            = GPU_RES_GET_GPU(pProfiler);
    RM_API   *pRmApi                          = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NvHandle  hClient                         = RES_GET_CLIENT_HANDLE(pProfiler);
    NvHandle  hObject                         = RES_GET_HANDLE(pProfiler);

    return pRmApi->Control(pRmApi, hClient, hObject,
                           NVB0CC_CTRL_CMD_INTERNAL_UNBIND_PM_RESOURCES,
                           NULL, 0);
}

NV_STATUS
profilerBaseCtrlCmdReserveHwpmLegacy_IMPL
(
    ProfilerBase *pProfiler,
    NVB0CC_CTRL_RESERVE_HWPM_LEGACY_PARAMS *pParams
)
{
    OBJGPU   *pGpu                            = GPU_RES_GET_GPU(pProfiler);
    RM_API   *pRmApi                          = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NvHandle  hClient                         = RES_GET_CLIENT_HANDLE(pProfiler);
    NvHandle  hObject                         = RES_GET_HANDLE(pProfiler);

    return pRmApi->Control(pRmApi, hClient, hObject,
                           NVB0CC_CTRL_CMD_INTERNAL_RESERVE_HWPM_LEGACY,
                           pParams, sizeof(*pParams));
}

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
    NVB0CC_CTRL_INTERNAL_ALLOC_PMA_STREAM_PARAMS internalParams;
    //
    // REGISTER  MEMDESCs TO GSP
    // These are no-op with BareMetal/No GSP
    //
    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR, 
                        memdescRegisterToGSP(pGpu, hClient, hParent, pParams->hMemPmaBuffer),
                        fail);
    bMemPmaBufferRegistered = NV_TRUE;

    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR, 
                        memdescRegisterToGSP(pGpu, hClient, hParent, pParams->hMemPmaBytesAvailable),
                        fail);
    bMemPmaBytesAvailableRegistered = NV_TRUE;

    portMemSet(&internalParams, 0, sizeof(NVB0CC_CTRL_INTERNAL_ALLOC_PMA_STREAM_PARAMS));
    internalParams.hMemPmaBuffer = pParams->hMemPmaBuffer;
    internalParams.pmaBufferOffset = pParams->pmaBufferOffset;
    internalParams.pmaBufferSize = pParams->pmaBufferSize;
    internalParams.hMemPmaBytesAvailable = pParams->hMemPmaBytesAvailable;
    internalParams.pmaBytesAvailableOffset = pParams->pmaBytesAvailableOffset;
    internalParams.ctxsw = pParams->ctxsw;
    internalParams.pmaChannelIdx = pParams->pmaChannelIdx;
    internalParams.pmaBufferVA = pParams->pmaBufferVA;
    internalParams.bInputPmaChIdx = NV_FALSE;

     NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
        pRmApi->Control(pRmApi,
                        hClient,
                        hObject,
                        NVB0CC_CTRL_CMD_INTERNAL_ALLOC_PMA_STREAM,
                        &internalParams, sizeof(internalParams)), fail);

    pParams->pmaChannelIdx = internalParams.pmaChannelIdx;

    // Copy output params to external struct.
    pParams->pmaBufferVA = internalParams.pmaBufferVA;
    
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

