/*
 * SPDX-FileCopyrightText: Copyright (c) 2012-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "virtualization/hypervisor/hypervisor.h"

#include "vgpu/rpc.h"
#include "gpu/nvenc/nvencsession.h"
#include "rmapi/rs_utils.h"
#include "core/locks.h"

static NvU32 sessionCounter = 0x01;

static void _gpuNvEncSessionDataProcessingCallback(OBJGPU *pGpu, void *data);

NV_STATUS
nvencsessionConstruct_IMPL
(
    NvencSession                 *pNvencSession,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    NVA0BC_ALLOC_PARAMETERS *pNvA0BCAllocParams = pParams->pAllocParams;
    NvHandle hNvencSessionHandle = pCallContext->pResourceRef->hResource;
    PNVENC_SESSION_LIST_ITEM pNvencSessionListItem = NULL;
    OBJGPU    *pGpu = GPU_RES_GET_GPU(pNvencSession);
    RsClient  *pRsClient = pCallContext->pClient;
    NvHandle  hClient = pRsClient->hClient;
    RmClient  *pClient = dynamicCast(pRsClient, RmClient);
    NV_STATUS status = NV_OK;

    if (listCount(&(pGpu->nvencSessionList)) >= NV2080_CTRL_GPU_NVENC_SESSION_INFO_MAX_COPYOUT_ENTRIES)
    {
        NV_PRINTF(LEVEL_WARNING,
                  "Creating NVENC session above max copyout limit.\n");
    }

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
                               pNvA0BCAllocParams,
                               sizeof(*pNvA0BCAllocParams),
                               status);

        if (status != NV_OK)
        {
            return status;
        }
    }

    if (pNvA0BCAllocParams->version == NVA0BC_ALLOC_PARAMS_VER_1)
    {
        pNvencSession->version = NVA0BC_ALLOC_PARAMS_VER_1;
    }
    else
    {
        pNvencSession->version = NVA0BC_ALLOC_PARAMS_VER_0;
    }

    if ((pNvencSession->version == NVA0BC_ALLOC_PARAMS_VER_1) &&
        (!hypervisorIsVgxHyper()))
    {
        status = memGetByHandle(pRsClient, pNvA0BCAllocParams->hMem, &(pNvencSession->pMemory));
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Unable to find mem corresponding to handle : 0x%0x.\n",
                      pNvA0BCAllocParams->hMem);
            pNvencSession->pMemory = NULL;
            return status;
        }

        status = memdescMap(pNvencSession->pMemory->pMemDesc,      // Descriptor
                            0,                                     // Offset
                            NVENC_SESSION_INFO_SIZE_V1,            // Length
                            NV_TRUE,                               // Kernel
                            NV_PROTECT_READABLE,                   // Protect
                            &(pNvencSession->pSessionStatsBuffer), // pAddress
                            &(pNvencSession->pPriv));              // pPriv
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Error mapping memory to CPU VA space, error : 0x%0x.\n",
                      status);
            pNvencSession->pMemory = NULL;
            return status;
        }
    }
    else
    {
        pNvencSession->pMemory              = NULL;
        pNvencSession->pSessionStatsBuffer  = NvP64_NULL;
        pNvencSession->pPriv                = NvP64_NULL;
    }

    pNvencSessionListItem = listAppendNew(&(pGpu->nvencSessionList));

    if (pNvencSessionListItem == NULL)
    {
        return NV_ERR_NO_MEMORY;
    }

    portMemSet(pNvencSessionListItem, 0, sizeof(NVENC_SESSION_LIST_ITEM));

    /*
     * When this class is allocated for NMOS or vGPU VM, subProcessId is 0x00.
     * However when allocated for host vGPU NVRM, processId represents plugin
     * and subProcessId represent NVENC session process Id within that VM.
     */

    pNvencSession->handle                                   = hNvencSessionHandle;
    pNvencSession->nvencSessionEntry.sessionId              = sessionCounter++;
    pNvencSession->nvencSessionEntry.processId              = pClient->ProcID;
    pNvencSession->nvencSessionEntry.subProcessId           = pClient->SubProcessID;
    pNvencSession->nvencSessionEntry.codecType              = pNvA0BCAllocParams->codecType;
    pNvencSession->nvencSessionEntry.hResolution            = pNvA0BCAllocParams->hResolution;
    pNvencSession->nvencSessionEntry.vResolution            = pNvA0BCAllocParams->vResolution;
    pNvencSession->nvencSessionEntry.averageEncodeFps       = 0;
    pNvencSession->nvencSessionEntry.averageEncodeLatency   = 0;

    pNvencSessionListItem->hClient    = hClient;
    pNvencSessionListItem->sessionPtr = pNvencSession;

    if ((pNvencSession->version  == NVA0BC_ALLOC_PARAMS_VER_1) &&
        (!hypervisorIsVgxHyper())                              &&
        (listCount(&(pGpu->nvencSessionList)) == 1))
    {
        // Register 1Hz timer callback for this GPU.
        pGpu->bNvEncSessionDataProcessingWorkItemPending = NV_FALSE;
        status = osSchedule1HzCallback(pGpu,
                                       _gpuNvEncSessionDataProcessingCallback,
                                       NULL,
                                       NV_OS_1HZ_REPEAT);
    }

    return NV_OK;
}

void
nvencsessionDestruct_IMPL
(
    NvencSession *pNvencSession
)
{
    RS_RES_FREE_PARAMS_INTERNAL *pParams;
    PNVENC_SESSION_LIST_ITEM  pNvencSessionListItem;
    PNVENC_SESSION_LIST_ITEM  pNvencSessionListItemNext;
    OBJGPU        *pGpu = GPU_RES_GET_GPU(pNvencSession);
    CALL_CONTEXT  *pCallContext;
    RsClient      *pRsClient;
    NvHandle        hClient;
    NvHandle        hParent;
    NvHandle        hNvencSessionHandle;
    NV_STATUS status = NV_OK;

    resGetFreeParams(staticCast(pNvencSession, RsResource), &pCallContext, &pParams);

    pRsClient           = pCallContext->pClient;
    hClient             = pRsClient->hClient;
    hParent             = pCallContext->pResourceRef->pParentRef->hResource;
    hNvencSessionHandle = pCallContext->pResourceRef->hResource;

    if (pNvencSession->pMemory)
    {
        memdescUnmap(pNvencSession->pMemory->pMemDesc,
                     NV_TRUE,
                     osGetCurrentProcess(),
                     pNvencSession->pSessionStatsBuffer,
                     pNvencSession->pPriv);
    }

    if (IS_VIRTUAL(pGpu))
    {
        NV_RM_RPC_FREE(pGpu, hClient, hParent, hNvencSessionHandle, status);
        NV_ASSERT(NV_OK == status);
    }

    for (pNvencSessionListItem = listHead(&(pGpu->nvencSessionList));
         pNvencSessionListItem != NULL;
         pNvencSessionListItem = pNvencSessionListItemNext)
    {
        pNvencSessionListItemNext = listNext(&(pGpu->nvencSessionList), pNvencSessionListItem);

        if (pNvencSessionListItem != NULL && (pNvencSessionListItem->sessionPtr == pNvencSession))
        {
            listRemove(&(pGpu->nvencSessionList), pNvencSessionListItem);
        }
    }

    if ((pNvencSession->version  == NVA0BC_ALLOC_PARAMS_VER_1) &&
        (!hypervisorIsVgxHyper())                              &&
        (listCount(&(pGpu->nvencSessionList)) == 0))
    {
        // Stop 1Hz callback for this GPU.
        osRemove1HzCallback(pGpu, _gpuNvEncSessionDataProcessingCallback, NULL);
    }

    pParams->status = status;
}

static void
_gpuNvEncSessionProcessBuffer(OBJGPU *pGpu, NvencSession *pNvencSession)
{
    NvU32 frameCount;
    NvU32 currIndex;
    NvU32 latestFrameIndex;
    NvU32 latestFrameId;
    NvU64 latestFrameEndTS;
    NvU64 processedFrameCount;
    NvU64 timeTakenToEncodeNs;
    NvS64 timeDiffFrameTS;
    NVENC_SESSION_INFO_V1 *pSessionInfoBuffer;
    NVENC_SESSION_INFO_V1 *pLocalSessionInfoBuffer;
    NVENC_SESSION_INFO_ENTRY_V1 *pSubmissionTSEntry;
    NVENC_SESSION_INFO_ENTRY_V1 *pStartTSEntry;
    NVENC_SESSION_INFO_ENTRY_V1 *pEndTSEntry;
    NVENC_SESSION_INFO_REGION_1_ENTRY_V1 *pRegion1;

    pSessionInfoBuffer = (NVENC_SESSION_INFO_V1 *)NvP64_VALUE(pNvencSession->pSessionStatsBuffer);
    if (pSessionInfoBuffer == NULL)
    {
        NV_PRINTF(LEVEL_INFO, "GPU : 0x%0x, NvEnc session stats buffer pointer is null.\n", pGpu->gpuId);
        return;
    }

    pLocalSessionInfoBuffer = portMemAllocNonPaged(sizeof(NVENC_SESSION_INFO_V1));
    if (pLocalSessionInfoBuffer == NULL)
    {
        NV_PRINTF(LEVEL_INFO, "GPU : 0x%0x, Failed to allocate memory for local stats buffer.\n", pGpu->gpuId);
        return;
    }

    // Reset local buffer.
    portMemSet(pLocalSessionInfoBuffer, 0, sizeof(NVENC_SESSION_INFO_V1));

    // Copy all the frame data from memory allocated by UMD to local buffer.
    portMemCopy(pLocalSessionInfoBuffer, sizeof(NVENC_SESSION_INFO_V1), pSessionInfoBuffer, sizeof(NVENC_SESSION_INFO_V1));

    pRegion1 = pLocalSessionInfoBuffer->region1.frameInfo;

    // Detect if UMD has not written all NVENC_SESSION_INFO_REGION_1_MAX_ENTRIES_COUNT_V1 entries.
    // If yes, then pick the index next to last processed index from previous callback.
    // If not, then we need to find lowest frame no from where we should start reading the timestamp data.
    if (pRegion1[pNvencSession->lastProcessedIndex].submissionTSEntry.frameId == pNvencSession->lastProcessedFrameId)
    {
        currIndex = (pNvencSession->lastProcessedIndex + 1) % NVENC_SESSION_INFO_REGION_1_MAX_ENTRIES_COUNT_V1;
    }
    else
    {
        NvU32 oldestIndex = NVENC_SESSION_INFO_REGION_1_MAX_ENTRIES_COUNT_V1 - 1;
        NvU32 minFrameId  = pRegion1[oldestIndex].submissionTSEntry.frameId;

        while (oldestIndex > 0)
        {
            oldestIndex--;
            if (minFrameId > pRegion1[oldestIndex].submissionTSEntry.frameId)
            {
                minFrameId = pRegion1[oldestIndex].submissionTSEntry.frameId;
            }
            else
            {
                oldestIndex++;
                break;
            }
        }
        currIndex = oldestIndex;
    }

    timeTakenToEncodeNs  = 0;
    processedFrameCount  = 0;
    latestFrameIndex     = currIndex;
    if (pNvencSession->lastProcessedFrameTS == 0)
    {
        pNvencSession->lastProcessedFrameTS = pLocalSessionInfoBuffer->region2.frameInfo[currIndex].startTSEntry.timestamp;
    }

    // Loop through all entries, find correct ones and use them for calculating average FPS and latency.
    for (frameCount = 0;
         frameCount < NVENC_SESSION_INFO_REGION_1_MAX_ENTRIES_COUNT_V1;
         frameCount++, (currIndex = (currIndex + 1) % NVENC_SESSION_INFO_REGION_1_MAX_ENTRIES_COUNT_V1))
    {
        pSubmissionTSEntry = &(pLocalSessionInfoBuffer->region1.frameInfo[currIndex].submissionTSEntry);
        pStartTSEntry      = &(pLocalSessionInfoBuffer->region2.frameInfo[currIndex].startTSEntry);
        pEndTSEntry        = &(pLocalSessionInfoBuffer->region2.frameInfo[currIndex].endTSEntry);

        // Validation : Check if last processed frame id is less than current frame id.
        if (pRegion1[latestFrameIndex].submissionTSEntry.frameId > pSubmissionTSEntry->frameId)
        {
            break;
        }

        // Validation : Check if submission-start-end frame ids match.
        if ((pSubmissionTSEntry->frameId != pStartTSEntry->frameId) || (pStartTSEntry->frameId != pEndTSEntry->frameId))
        {
            continue;
        }
        // Validation : Check if submission-start-end timestamps are in incrementing order.
        if ((pSubmissionTSEntry->timestamp > pStartTSEntry->timestamp) || (pStartTSEntry->timestamp > pEndTSEntry->timestamp))
        {
            continue;
        }

        // Update latest processed frame index.
        latestFrameIndex = currIndex;

        // Add the difference of end timestamp and submission timestamp to total time taken.
        timeTakenToEncodeNs += (pEndTSEntry->timestamp - pSubmissionTSEntry->timestamp);
        processedFrameCount++;
    }

    if (processedFrameCount > 0)
    {
        latestFrameId    = pLocalSessionInfoBuffer->region1.frameInfo[latestFrameIndex].submissionTSEntry.frameId;
        latestFrameEndTS = pLocalSessionInfoBuffer->region2.frameInfo[latestFrameIndex].endTSEntry.timestamp;

        // Calculate average latency.
        timeTakenToEncodeNs /= processedFrameCount;
        // averageEncodeLatency is in micro second.
        pNvencSession->nvencSessionEntry.averageEncodeLatency =
            ((timeTakenToEncodeNs / 1000) < 0xFFFFFFFF) ? (timeTakenToEncodeNs / 1000) : 0xFFFFFFFF;

        // Calculate average FPS.
        // Find time difference between latest processed frame end TS and last processed frame end TS in last callback.
        // Same is done for findng processed frame count.
        // This would provide a better average FPS value.
        timeDiffFrameTS = latestFrameEndTS - pNvencSession->lastProcessedFrameTS;
        if (timeDiffFrameTS > 0)
        {
            processedFrameCount = latestFrameId - pNvencSession->lastProcessedFrameId;
            pNvencSession->nvencSessionEntry.averageEncodeFps = ((processedFrameCount * 1000 * 1000 * 1000) / timeDiffFrameTS);
        }
        else
        {
            pNvencSession->nvencSessionEntry.averageEncodeLatency = 0;
            pNvencSession->nvencSessionEntry.averageEncodeFps     = 0;
        }

        pNvencSession->lastProcessedIndex   = latestFrameIndex;
        pNvencSession->lastProcessedFrameId = latestFrameId;
        pNvencSession->lastProcessedFrameTS = latestFrameEndTS;
    }
    else
    {
        pNvencSession->nvencSessionEntry.averageEncodeLatency = 0;
        pNvencSession->nvencSessionEntry.averageEncodeFps     = 0;
    }

    portMemFree(pLocalSessionInfoBuffer);
}

static void _gpuNvEncSessionDataProcessing(OBJGPU *pGpu)
{
    PNVENC_SESSION_LIST_ITEM  pNvencSessionListItem;
    PNVENC_SESSION_LIST_ITEM  pNvencSessionListItemNext;
    NV_STATUS status = NV_OK;
    NVA0BC_CTRL_NVENC_SW_SESSION_UPDATE_INFO_PARAMS rpcParams = {0};

    // Loop through all sessions.
    for (pNvencSessionListItem = listHead(&(pGpu->nvencSessionList));
         pNvencSessionListItem != NULL;
         pNvencSessionListItem = pNvencSessionListItemNext)
    {
        pNvencSessionListItemNext = listNext(&(pGpu->nvencSessionList), pNvencSessionListItem);
        if (pNvencSessionListItem != NULL && pNvencSessionListItem->sessionPtr)
        {
            _gpuNvEncSessionProcessBuffer(pGpu, pNvencSessionListItem->sessionPtr);

            if (IS_VIRTUAL(pGpu) && (pGpu->encSessionStatsReportingState == NV2080_CTRL_GPU_INFO_NVENC_STATS_REPORTING_STATE_ENABLED))
            {
                rpcParams.hResolution           = pNvencSessionListItem->sessionPtr->nvencSessionEntry.hResolution;
                rpcParams.vResolution           = pNvencSessionListItem->sessionPtr->nvencSessionEntry.vResolution;
                rpcParams.averageEncodeLatency  = pNvencSessionListItem->sessionPtr->nvencSessionEntry.averageEncodeLatency;
                rpcParams.averageEncodeFps      = pNvencSessionListItem->sessionPtr->nvencSessionEntry.averageEncodeFps;
                rpcParams.timestampBufferSize   = 0;

                NV_RM_RPC_CONTROL(pGpu,
                                  pNvencSessionListItem->hClient,
                                  pNvencSessionListItem->sessionPtr->handle,
                                  NVA0BC_CTRL_CMD_NVENC_SW_SESSION_UPDATE_INFO,
                                  &rpcParams,
                                  sizeof(NVA0BC_CTRL_NVENC_SW_SESSION_UPDATE_INFO_PARAMS),
                                  status);
            }
        }
    }
}

static void _gpuNvEncSessionDataProcessingWorkItem(NvU32 gpuInstance, void *pArgs)
{
    OBJGPU *pGpu;

    pGpu = gpumgrGetGpu(gpuInstance);
    if (pGpu == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "NVENC Sessions GPU instance is invalid\n");
        return;
    }

    _gpuNvEncSessionDataProcessing(pGpu);
    pGpu->bNvEncSessionDataProcessingWorkItemPending = NV_FALSE;
}

static void
_gpuNvEncSessionDataProcessingCallback(OBJGPU *pGpu, void *data)
{
    NV_STATUS   status;

    if (!pGpu->bNvEncSessionDataProcessingWorkItemPending)
    {
        status = osQueueWorkItemWithFlags(pGpu,
                                          _gpuNvEncSessionDataProcessingWorkItem,
                                          NULL,
                                          OS_QUEUE_WORKITEM_FLAGS_LOCK_SEMA
                                          | OS_QUEUE_WORKITEM_FLAGS_LOCK_GPU_GROUP_DEVICE);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "NVENC session queuing async callback failed, status=%x\n",
                      status);

            // Call directly to do NVENC session data processing
            _gpuNvEncSessionDataProcessing(pGpu);
        }
        else
        {
            pGpu->bNvEncSessionDataProcessingWorkItemPending = NV_TRUE;
        }
    }
}
