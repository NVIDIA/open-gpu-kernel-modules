/*
 * SPDX-FileCopyrightText: Copyright (c) 2008-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "os/os.h"
#include "gpu_mgr/gpu_mgr.h"
#include "rmconfig.h"

#include "vgpu/rpc.h"
#include "disp/nvfbc_session.h"
#include "rmapi/rs_utils.h"
#include "gpu/gpu.h"
#include "ctrl/ctrla0bd.h"

#include "virtualization/hypervisor/hypervisor.h"

#define NVFBC_SESSION_INVALIDATE_DATA_TIMEOUT          2000000000    // nano second granularity, 2 seconds

static NvU32 sessionCounter = 0x01;

NvBool nvfbcIsSessionDataStale(NvU64 lastUpdateTimeStamp)
{
    NvU64 currTime = 0;

    osGetPerformanceCounter(&currTime);

    if ((currTime >= lastUpdateTimeStamp) &&
       ((currTime - lastUpdateTimeStamp) >= NVFBC_SESSION_INVALIDATE_DATA_TIMEOUT))
    {
        return NV_TRUE;
    }
    return NV_FALSE;
}

NV_STATUS
nvfbcsessionConstruct_IMPL
(
    NvfbcSession                 *pNvfbcSession,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    NVA0BD_ALLOC_PARAMETERS *pNvA0BDAllocParams = pParams->pAllocParams;
    PNVFBC_SESSION_LIST_ITEM pNvfbcSessionListItem = NULL;
    OBJGPU    *pGpu = GPU_RES_GET_GPU(pNvfbcSession);
    RsClient  *pRsClient = pCallContext->pClient;
    NvHandle  hClient = pRsClient->hClient;
    RmClient  *pClient;
    NV_STATUS status = NV_OK;

    if (listCount(&(pGpu->nvfbcSessionList)) >= NV2080_GPU_NVFBC_MAX_SESSION_COUNT)
    {
        NV_PRINTF(LEVEL_WARNING,
                  "Creating NVFBC session above max session limit.\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    pClient = serverutilGetClientUnderLock(hClient);
    if (pClient == NULL)
    {
        return NV_ERR_INVALID_CLIENT;
    }

    if (IS_VIRTUAL(pGpu))
    {
        NV_RM_RPC_ALLOC_OBJECT(pGpu,
            pParams->hClient,
            pParams->hParent,
            pParams->hResource,
            pParams->externalClassId,
            pNvA0BDAllocParams,
            sizeof(*pNvA0BDAllocParams),
            status);

        if (status != NV_OK)
        {
            goto done;
        }
    }

    pNvfbcSessionListItem = listAppendNew(&(pGpu->nvfbcSessionList));
    if (pNvfbcSessionListItem == NULL)
    {
        return NV_ERR_NO_MEMORY;
    }

    portMemSet(pNvfbcSessionListItem, 0, sizeof(NVFBC_SESSION_LIST_ITEM));

    /*
    * When this class is allocated for NMOS or vGPU VM, vgpuInstanceId will be 0x00.
    */

    if (hypervisorIsVgxHyper())
    {
        if (IS_GSP_CLIENT(pGpu)) {
            status = NV_ERR_NOT_SUPPORTED;
            goto done;
        }
    }
    else
    {
        pNvfbcSession->nvfbcSessionEntry.vgpuInstanceId = 0;
        pNvfbcSession->nvfbcSessionEntry.processId = pClient->ProcID;
    }

    pNvfbcSession->nvfbcSessionEntry.sessionId      = sessionCounter++;
    pNvfbcSession->nvfbcSessionEntry.displayOrdinal = pNvA0BDAllocParams->displayOrdinal;
    pNvfbcSession->nvfbcSessionEntry.sessionType    = pNvA0BDAllocParams->sessionType;

    pNvfbcSession->nvfbcSessionEntry.sessionFlags   =
        FLD_TEST_DRF(A0BD_, NVFBC_SESSION_FLAG_DIFFMAP, _ENABLED, _TRUE, pNvA0BDAllocParams->sessionFlags) ?
        NV2080_CTRL_NVFBC_SESSION_FLAG_DIFFMAP_ENABLED : 0;
    pNvfbcSession->nvfbcSessionEntry.sessionFlags  |=
        FLD_TEST_DRF(A0BD_, NVFBC_SESSION_FLAG_CLASSIFICATIONMAP, _ENABLED, _TRUE, pNvA0BDAllocParams->sessionFlags) ?
        NV2080_CTRL_NVFBC_SESSION_FLAG_CLASSIFICATIONMAP_ENABLED : 0;

    pNvfbcSession->nvfbcSessionEntry.hMaxResolution      = pNvA0BDAllocParams->hMaxResolution;
    pNvfbcSession->nvfbcSessionEntry.vMaxResolution      = pNvA0BDAllocParams->vMaxResolution;
    pNvfbcSession->nvfbcSessionEntry.averageFPS          = 0;
    pNvfbcSession->nvfbcSessionEntry.averageLatency      = 0;
    pNvfbcSession->nvfbcSessionEntry.lastUpdateTimeStamp = SESSION_UPDATE_TIMESTAMP_UNINITIALIZED;

    pNvfbcSessionListItem->hClient = hClient;
    pNvfbcSessionListItem->sessionPtr = pNvfbcSession;

done:
    if (status != NV_OK && pNvfbcSessionListItem != NULL)
    {
        listRemove(&(pGpu->nvfbcSessionList), pNvfbcSessionListItem);
    }

    return status;
}

void
nvfbcsessionDestruct_IMPL
(
    NvfbcSession *pNvfbcSession
)
{
    RS_RES_FREE_PARAMS_INTERNAL *pParams;
    PNVFBC_SESSION_LIST_ITEM pNvfbcSessionListItem = NULL;
    PNVFBC_SESSION_LIST_ITEM pNvfbcSessionListItemNext = NULL;
    OBJGPU        *pGpu = GPU_RES_GET_GPU(pNvfbcSession);
    CALL_CONTEXT  *pCallContext;
    RsClient      *pRsClient;
    NvHandle        hClient;
    NvHandle        hParent;
    NvHandle        hNvfbcSessionHandle;
    NV_STATUS status = NV_OK;

    resGetFreeParams(staticCast(pNvfbcSession, RsResource), &pCallContext, &pParams);

    pRsClient = pCallContext->pClient;
    hClient = pRsClient->hClient;
    hParent = pCallContext->pResourceRef->pParentRef->hResource;
    hNvfbcSessionHandle = pCallContext->pResourceRef->hResource;

    if (IS_VIRTUAL(pGpu))
    {
        NV_RM_RPC_FREE(pGpu, hClient, hParent, hNvfbcSessionHandle, status);
        NV_ASSERT(NV_OK == status);
    }

    for (pNvfbcSessionListItem = listHead(&(pGpu->nvfbcSessionList));
         pNvfbcSessionListItem != NULL;
         pNvfbcSessionListItem = pNvfbcSessionListItemNext)
    {
        pNvfbcSessionListItemNext = listNext(&(pGpu->nvfbcSessionList), pNvfbcSessionListItem);
        if (pNvfbcSessionListItem != NULL && (pNvfbcSessionListItem->sessionPtr == pNvfbcSession))
        {
            listRemove(&(pGpu->nvfbcSessionList), pNvfbcSessionListItem);
        }
    }

    pParams->status = status;
}

NV_STATUS
nvfbcsessionCtrlCmdNvFBCSwSessionUpdateInfo_IMPL
(
    NvfbcSession *pNvfbcSession,
    NVA0BD_CTRL_NVFBC_SW_SESSION_UPDATE_INFO_PARAMS *pParams
)
{
    NVA0BD_CTRL_NVFBC_TIMESTAMP *pTimeStampBuffer = NULL;
    OBJGPU *pGpu = GPU_RES_GET_GPU(pNvfbcSession);
    NvU32 i;
    NvU64 startTime;
    NvU64 endTime;
    NvU32 totalEntries;
    NvU64 localAverageLatency;
    NvU64 localAverageFPS;
    NvU64 timeToCapture = 0;
    NV_STATUS status = NV_OK;

    NV_PRINTF(LEVEL_INFO, "Enter function\n");

    NV_ASSERT_OR_RETURN((pParams != NULL), NV_ERR_INVALID_POINTER);

    pNvfbcSession->nvfbcSessionEntry.hResolution    = pParams->hResolution;
    pNvfbcSession->nvfbcSessionEntry.vResolution    = pParams->vResolution;
    pNvfbcSession->nvfbcSessionEntry.totalGrabCalls = pParams->totalGrabCalls;

    // Clear out previous NV2080_CTRL_NVFBC_SESSION_FLAG_CAPTURE_WITH_WAIT_xxx flag.
    pNvfbcSession->nvfbcSessionEntry.sessionFlags  &= ~(NV2080_CTRL_NVFBC_SESSION_FLAG_CAPTURE_WITH_WAIT_NO_WAIT  |
                                                            NV2080_CTRL_NVFBC_SESSION_FLAG_CAPTURE_WITH_WAIT_INFINITE |
                                                            NV2080_CTRL_NVFBC_SESSION_FLAG_CAPTURE_WITH_WAIT_TIMEOUT);
    // Find and assign NV2080_CTRL_NVFBC_SESSION_FLAG_CAPTURE_WITH_WAIT_xxx flag.
    switch (pParams->captureCallFlags)
    {
        case NVA0BD_NVFBC_WITH_WAIT_FALSE:
            pNvfbcSession->nvfbcSessionEntry.sessionFlags |= NV2080_CTRL_NVFBC_SESSION_FLAG_CAPTURE_WITH_WAIT_NO_WAIT;
            break;
        case NVA0BD_NVFBC_WITH_WAIT_INFINITE:
            pNvfbcSession->nvfbcSessionEntry.sessionFlags |= NV2080_CTRL_NVFBC_SESSION_FLAG_CAPTURE_WITH_WAIT_INFINITE;
            break;
        case NVA0BD_NVFBC_WITH_WAIT_TIMEOUT:
            pNvfbcSession->nvfbcSessionEntry.sessionFlags |= NV2080_CTRL_NVFBC_SESSION_FLAG_CAPTURE_WITH_WAIT_TIMEOUT;
            break;
    }

    NV_ASSERT(pParams->timestampEntryCount <= NVA0BD_CTRL_CMD_NVFBC_MAX_TIMESTAMP_ENTRIES);
    if (pParams->timestampEntryCount > NVA0BD_CTRL_CMD_NVFBC_MAX_TIMESTAMP_ENTRIES)
    {
        pParams->timestampEntryCount = NVA0BD_CTRL_CMD_NVFBC_MAX_TIMESTAMP_ENTRIES;
    }

    if (pParams->timestampEntryCount != 0)
    {
        pTimeStampBuffer = pParams->timestampEntry;

        // Calculate Average Latency.
        for (i = 0; i < pParams->timestampEntryCount; i++)
        {
            if (pTimeStampBuffer[i].startTime > pTimeStampBuffer[i].endTime)
            {
                timeToCapture += ((0xFFFFFFFFFFFFFFFFULL - pTimeStampBuffer[i].startTime) + pTimeStampBuffer[i].endTime);
            }
            else
            {
                timeToCapture += pTimeStampBuffer[i].endTime - pTimeStampBuffer[i].startTime;
            }
        }

        localAverageLatency = timeToCapture / pParams->timestampEntryCount;
        // Mask with 0xFFFFFFFF since it should not be greater than 32bit range.
        NV_ASSERT(localAverageLatency < 0xFFFFFFFF);
        pNvfbcSession->nvfbcSessionEntry.averageLatency = localAverageLatency < 0xFFFFFFFF   ?
                                                              (localAverageLatency & 0xFFFFFFFF) :
                                                              0xFFFFFFFF;

        // Calculate Average FPS.
        // Formula used to calculate FPS : n-1/(lastFrameTimestamp.startTime - firstFrameTimestamp.startTime)
        // n is the total no of entries.
        // If there is only one entry, we need to use firstFrameTimestamp.endtime as endTime.
        startTime = pTimeStampBuffer[0].startTime;
        if (pParams->timestampEntryCount > 1)
        {
            endTime      = pTimeStampBuffer[pParams->timestampEntryCount - 1].startTime;
            totalEntries = pParams->timestampEntryCount - 1;
        }
        else
        {
            endTime      = pTimeStampBuffer[0].endTime;
            totalEntries = pParams->timestampEntryCount;
        }

        if (endTime >= startTime)
        {
            timeToCapture = endTime - startTime;
        }
        else
        {
            timeToCapture = (0xFFFFFFFFFFFFFFFFULL - startTime) + endTime;
        }

        if (timeToCapture)
        {
            localAverageFPS = ((totalEntries * 1000 * 1000) / timeToCapture);
            // Mask with 0xFFFFFFFF since it should not be greater than 32bit range.
            NV_ASSERT(localAverageFPS < 0xFFFFFFFF);
            pNvfbcSession->nvfbcSessionEntry.averageFPS = localAverageFPS < 0xFFFFFFFF ?
                                                              (localAverageFPS & 0xFFFFFFFF) : 0xFFFFFFFF;
        }
        else
        {
            pNvfbcSession->nvfbcSessionEntry.averageFPS = 0;
        }
    }
    else
    {
        pNvfbcSession->nvfbcSessionEntry.averageLatency = pParams->averageLatency;
        pNvfbcSession->nvfbcSessionEntry.averageFPS     = pParams->averageFPS;
    }
    osGetPerformanceCounter(&pNvfbcSession->nvfbcSessionEntry.lastUpdateTimeStamp);

    if (IS_VIRTUAL(pGpu))
    {
        CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
        RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams;
        NVA0BD_CTRL_NVFBC_SW_SESSION_UPDATE_INFO_PARAMS rpcParams = { 0 };

        rpcParams.hResolution         = pParams->hResolution;
        rpcParams.vResolution         = pParams->vResolution;
        rpcParams.captureCallFlags    = pParams->captureCallFlags;
        rpcParams.totalGrabCalls      = pParams->totalGrabCalls;
        rpcParams.averageLatency      = pNvfbcSession->nvfbcSessionEntry.averageLatency;
        rpcParams.averageFPS          = pNvfbcSession->nvfbcSessionEntry.averageFPS;
        rpcParams.timestampEntryCount = 0;

        NV_RM_RPC_CONTROL(pGpu, pRmCtrlParams->hClient, pRmCtrlParams->hObject, pRmCtrlParams->cmd,
            &rpcParams, pRmCtrlParams->paramsSize, status);
    }

    return status;
}

