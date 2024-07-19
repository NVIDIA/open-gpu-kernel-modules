/*
 * SPDX-FileCopyrightText: Copyright (c) 2007-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "vgpu/rpc.h"
#include "os/os.h"
#include "rmapi/control.h"
#include "ctrl/ctrla0bc.h"
#include "gpu/nvenc/nvencsession.h"
#include "virtualization/hypervisor/hypervisor.h"

NV_STATUS
_nvencsessionCtrlCmdNvencSwSessionUpdateInfo
(
    OBJGPU  *pGpu,
    NvU32   hResolution,
    NvU32   vResolution,
    NvU32   averageEncodeLatency,
    NvU32   averageEncodeFps,
    NvU32   timeStampBufferSize,
    NVA0BC_CTRL_NVENC_TIMESTAMP *timeStampBuffer,
    NvencSession *pNvencSession
)
{
    NvU32 i, tempTimestampBufferSize;
    NvU64 timeToEncodeBuffer = 0;
    NV_STATUS status = NV_OK;

    NV_PRINTF(LEVEL_INFO, "\n");

    pNvencSession->nvencSessionEntry.hResolution = hResolution;
    pNvencSession->nvencSessionEntry.vResolution = vResolution;

    if (pNvencSession->version == NVA0BC_ALLOC_PARAMS_VER_0)
    {
        if (timeStampBufferSize != 0 && timeStampBuffer != NULL)
        {
            tempTimestampBufferSize = timeStampBufferSize;

            for (i = 0; i < tempTimestampBufferSize; i++)
            {
                if (timeStampBuffer[i].startTime > timeStampBuffer[i].endTime)
                {
                    timeToEncodeBuffer += ((0xFFFFFFFFFFFFFFFFULL - timeStampBuffer[i].startTime ) + timeStampBuffer[i].endTime);
                }
                else
                {
                    timeToEncodeBuffer += timeStampBuffer[i].endTime - timeStampBuffer[i].startTime;
                }
            }

            // Mask with 0xFFFFFFFF since it should not be greater than 32bit range.
            pNvencSession->nvencSessionEntry.averageEncodeLatency = (timeToEncodeBuffer / tempTimestampBufferSize) & 0xFFFFFFFF;

            if (timeStampBuffer[tempTimestampBufferSize - 1].endTime > timeStampBuffer[0].startTime)
            {
                timeToEncodeBuffer = timeStampBuffer[tempTimestampBufferSize - 1].endTime - timeStampBuffer[0].startTime;
            }
            else
            {
                timeToEncodeBuffer = (0xFFFFFFFFFFFFFFFFULL - timeStampBuffer[0].startTime) + timeStampBuffer[tempTimestampBufferSize - 1].endTime;
            }

            if (timeToEncodeBuffer)
            {
                pNvencSession->nvencSessionEntry.averageEncodeFps = ((tempTimestampBufferSize * 1000 * 1000) / timeToEncodeBuffer) & 0xFFFFFFFF;
            }
        }
        else
        {
            pNvencSession->nvencSessionEntry.averageEncodeLatency = averageEncodeLatency;
            pNvencSession->nvencSessionEntry.averageEncodeFps     = averageEncodeFps;
        }

        if (IS_VIRTUAL(pGpu) &&
            (pGpu->encSessionStatsReportingState == NV2080_CTRL_GPU_INFO_NVENC_STATS_REPORTING_STATE_ENABLED))
        {
            CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
            RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams;
            NVA0BC_CTRL_NVENC_SW_SESSION_UPDATE_INFO_PARAMS rpcParams = {0};

            rpcParams.hResolution           = hResolution;
            rpcParams.vResolution           = vResolution;
            rpcParams.averageEncodeLatency  = pNvencSession->nvencSessionEntry.averageEncodeLatency;
            rpcParams.averageEncodeFps      = pNvencSession->nvencSessionEntry.averageEncodeFps;
            rpcParams.timestampBufferSize   = 0;

            NV_RM_RPC_CONTROL(pGpu, pRmCtrlParams->hClient, pRmCtrlParams->hObject, pRmCtrlParams->cmd,
                            &rpcParams, pRmCtrlParams->paramsSize, status);
        }
    }
    else if (pNvencSession->version == NVA0BC_ALLOC_PARAMS_VER_1)
    {
        if (hypervisorIsVgxHyper())
        {
            pNvencSession->nvencSessionEntry.averageEncodeLatency = averageEncodeLatency;
            pNvencSession->nvencSessionEntry.averageEncodeFps     = averageEncodeFps;
        }
    }

    return status;
}

NV_STATUS
nvencsessionCtrlCmdNvencSwSessionUpdateInfo_IMPL
(
    NvencSession *pNvencSession,
    NVA0BC_CTRL_NVENC_SW_SESSION_UPDATE_INFO_PARAMS *pParams
)
{
    NVA0BC_CTRL_NVENC_TIMESTAMP *pTimeStampBuffer = NULL;
    OBJGPU *pGpu = GPU_RES_GET_GPU(pNvencSession);
    NV_STATUS status = NV_OK;

    NV_PRINTF(LEVEL_INFO, "\n");

    NV_ASSERT_OR_RETURN((pParams != NULL), NV_ERR_INVALID_POINTER);
    if (pParams->timestampBufferSize > NVA0BC_CTRL_CMD_NVENC_MAX_BUFFER_ENTRIES)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (pNvencSession->version == NVA0BC_ALLOC_PARAMS_VER_0)
    {
        if (pParams->timestampBufferSize != 0)
        {
            pTimeStampBuffer = NvP64_VALUE(pParams->timestampBuffer);
        }
    }

    status = _nvencsessionCtrlCmdNvencSwSessionUpdateInfo(pGpu, pParams->hResolution, pParams->vResolution, pParams->averageEncodeLatency, pParams->averageEncodeFps, pParams->timestampBufferSize, pTimeStampBuffer, pNvencSession);
    if (status != NV_OK)
        return status;

    return status;
}

NV_STATUS
nvencsessionCtrlCmdNvencSwSessionUpdateInfoV2_IMPL
(
    NvencSession *pNvencSession,
    NVA0BC_CTRL_NVENC_SW_SESSION_UPDATE_INFO_V2_PARAMS *pParams
)
{
    NVA0BC_CTRL_NVENC_TIMESTAMP *pTimeStampBuffer = NULL;
    OBJGPU *pGpu = GPU_RES_GET_GPU(pNvencSession);
    NV_STATUS status = NV_OK;

    NV_PRINTF(LEVEL_INFO, "\n");

    NV_ASSERT_OR_RETURN((pParams != NULL), NV_ERR_INVALID_POINTER);
    if (pParams->timestampBufferSize > NVA0BC_CTRL_CMD_NVENC_MAX_BUFFER_ENTRIES)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (pNvencSession->version == NVA0BC_ALLOC_PARAMS_VER_0)
    {
        if (pParams->timestampBufferSize != 0)
        {
            pTimeStampBuffer = pParams->timestampBuffer;
        }
    }

    status = _nvencsessionCtrlCmdNvencSwSessionUpdateInfo(pGpu, pParams->hResolution, pParams->vResolution, pParams->averageEncodeLatency, pParams->averageEncodeFps, pParams->timestampBufferSize, pTimeStampBuffer, pNvencSession);
    if (status != NV_OK)
        return status;

    return status;
}
