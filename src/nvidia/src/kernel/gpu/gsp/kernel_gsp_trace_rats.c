/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*!
 * @file
 * @brief Kernel Side RATS Functions
 */

#include "gpu/gsp/kernel_gsp_trace_rats.h"
#include "nvport/nvport.h"

#include "rmapi/client.h"
#include "rmapi/event_api.h"
#include "rmapi/event_buffer.h"
#include "core/locks.h"
#include "class/cl90cdtypes.h"
#include "class/cl90cdtrace.h"
#include "os/os.h"
#include "gpu/gpu.h"
#include "gpu_mgr/gpu_mgr.h"
#include "vgpu/rpc.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/timer/objtmr.h"

#define GSP_TRACE_BUFFER_ALLOC_SIZE_MAX (1 * 1024 * 1024)   // 1 MB

static
NV_STATUS
_ratsEventBufferAdd
(
    OBJGPU *pGpu,
    NV_EVENT_BUFFER_BIND_POINT_GSP_RATS *pBind,
    NV_RATS_RECORD *pRecord
)
{
    NV_STATUS status;
    NvBool bNotify;
    NvP64 notificationHandle;
    EVENT_BUFFER_PRODUCER_DATA notifyEvent;
    NvU32 notifyIndex = NV_EVENT_BUFFER_RECORD_TYPE_RATS_GSP_TRACE;
    OBJTMR *pTmr = GPU_GET_TIMER(pGpu);

    pRecord->seqNo = pBind->pEventBuffer->seqNo++;
    pRecord->gspRmTrace.timeStamp += pTmr->sysTimerOffsetNs;

    portMemSet(&notifyEvent, 0, sizeof(notifyEvent));
    notifyEvent.pVardata = NV_PTR_TO_NvP64(NULL);
    notifyEvent.vardataSize = 0;
    notifyEvent.pPayload = NV_PTR_TO_NvP64(pRecord);
    notifyEvent.payloadSize = sizeof(*pRecord);

    status = eventBufferAdd(pBind->pEventBuffer, &notifyEvent, notifyIndex, &bNotify, &notificationHandle);

    if ((status == NV_OK) && bNotify && notificationHandle)
    {
        osEventNotification(pGpu,
                pBind->pEventBuffer->pListeners,
                notifyIndex,
                &notifyEvent,
                0);             // Do not copy structure -- embedded pointers.
        pBind->pEventBuffer->bNotifyPending = NV_TRUE;
    }

    return status;
}

static void _ratsReadVgpuTracingBuffer(
    OBJGPU *pGpu,
    NV_RATS_VGPU_GSP_TRACING_BUFFER *pVgpuGspTracingBuffer
)
{
    // Allocated memory for buffer entries located immediately after header
    NV_RATS_RECORD *pRecords = (NV_RATS_RECORD*) (pVgpuGspTracingBuffer + 1);

    // Set static end point so we don't read forever
    NvU32 read_end = pVgpuGspTracingBuffer->write;

    while(pVgpuGspTracingBuffer->read != read_end)
    {
        gspRatsEventBufferLogRecord(pGpu, &pRecords[pVgpuGspTracingBuffer->read]);
        pVgpuGspTracingBuffer->read = (pVgpuGspTracingBuffer->read + 1) % pVgpuGspTracingBuffer->bufferSize;
    }

    pVgpuGspTracingBuffer->lastReadTimestamp = osGetTimestamp();
}

void gspRatsNotifyAllConsumers
(
    OBJGPU  *pGpu,
    void    *pArgs
)
{
    RatsEventBufferBindMultiMapSubmap *pSubmap = multimapFindSubmap(&pGpu->ratsEventBufferBindingsUid, 0);
    if (pSubmap != NULL)
    {
        RatsEventBufferBindMultiMapIter iter = multimapSubmapIterItems(&pGpu->ratsEventBufferBindingsUid, pSubmap);
        while (multimapItemIterNext(&iter))
        {
            NV_EVENT_BUFFER_BIND_POINT_GSP_RATS *pBind = iter.pValue;

            if (IS_VIRTUAL(pGpu))
            {
                NV_RATS_VGPU_GSP_TRACING_BUFFER *pVgpuGspTracingBuffer = (NV_RATS_VGPU_GSP_TRACING_BUFFER*)pBind->message_buffer;
                if (osGetTimestamp() - pVgpuGspTracingBuffer->lastReadTimestamp > osGetTimestampFreq())
                {
                    // vGPU plugin has no 1 second callback to empty buffer, so if we haven't read traces in over a second,
                    // read from GSP plugin to keep buffer up to date.
                    _ratsReadVgpuTracingBuffer(pGpu, pVgpuGspTracingBuffer);
                }
            }

            if (!eventBufferIsEmpty(pBind->pEventBuffer))
            {
                osEventNotification(pGpu,
                    pBind->pEventBuffer->pListeners,
                    NV_EVENT_BUFFER_RECORD_TYPE_RATS_GSP_TRACE,
                    NULL,
                    0);             // Do not copy structure -- embedded pointers.
            }
        }
    }
}

void
gspRatsEventBufferLogRecord
(
    OBJGPU *pGpu,
    NV_RATS_RECORD *pRecord
)
{
    RatsEventBufferBindMultiMapSubmap *pSubmap = multimapFindSubmap(&pGpu->ratsEventBufferBindingsUid, 0);
    if (pSubmap != NULL)
    {
        RatsEventBufferBindMultiMapIter iter = multimapSubmapIterItems(&pGpu->ratsEventBufferBindingsUid, pSubmap);
        while (multimapItemIterNext(&iter))
        {
            NV_EVENT_BUFFER_BIND_POINT_GSP_RATS *pBind = iter.pValue;
            _ratsEventBufferAdd(pGpu, pBind, pRecord);
        }
    }
}

void gspRatsServiceVgpuEventTracing(OBJGPU *pGpu)
{
    RatsEventBufferBindMultiMapSubmap *pSubmap = multimapFindSubmap(&pGpu->ratsEventBufferBindingsUid, 0);
    NV_EVENT_BUFFER_BIND_POINT_GSP_RATS *pBind = NULL;
    NV_RATS_VGPU_GSP_TRACING_BUFFER *pVgpuGspTracingBuffer;

    if (pSubmap != NULL)
    {
        RatsEventBufferBindMultiMapIter iter = multimapSubmapIterItems(&pGpu->ratsEventBufferBindingsUid, pSubmap);
        while (multimapItemIterNext(&iter))
        {
            pBind = iter.pValue;
            break; // only support one client right now
        }
    }

    if ((pBind == NULL) || (pBind->message_buffer == NULL))
    {
        return;
    }

    pVgpuGspTracingBuffer = (NV_RATS_VGPU_GSP_TRACING_BUFFER*)pBind->message_buffer;
    pVgpuGspTracingBuffer->bGuestNotifInProgress = NV_FALSE;
    _ratsReadVgpuTracingBuffer(pGpu, pVgpuGspTracingBuffer);
}

NV_STATUS
gspRatsAddBindpoint
(
    OBJGPU *pGpu,
    RsClient *pClient,
    RsResourceRef *pEventBufferRef,
    NvHandle hNotifier,
    NvU64 tracepointMask,
    NvU32 gspTracingBufferSize,
    NvU32 gspTracingBufferWatermark,
    NvU16 targetTask
)
{

    NvHandle hClient            = pClient->hClient;
    NvU32 hInternalClient       = pGpu->hInternalClient;
    NvU32 hInternalSubdevice    = pGpu->hInternalSubdevice;
    NvHandle hEventBuffer       = pEventBufferRef->hResource;
    RmClient *pRmClient         = dynamicCast(pClient, RmClient);
    RM_API *pRmApi              = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NvU64 targetUser            = 0; // one root user app, for now...
    NvBool bBindingActive       = pGpu->ratsConsumerCount > 0;
    NvBool bScheduled           = NV_FALSE;

    NV_STATUS status;
    EventBuffer *pEventBuffer;
    NV_EVENT_BUFFER_BIND_POINT_GSP_RATS *pBind;

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmDeviceGpuLockIsOwner(pGpu->gpuInstance),
        NV_ERR_INVALID_LOCK_STATE);

    if (IS_VIRTUAL(pGpu) && bBindingActive)
    {
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    pEventBuffer = dynamicCast(pEventBufferRef->pResource, EventBuffer);
    if (pEventBuffer == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    if (NULL == multimapFindSubmap(&pGpu->ratsEventBufferBindingsUid, targetUser))
    {
        if (NULL == multimapInsertSubmap(&pGpu->ratsEventBufferBindingsUid, targetUser))
        {
            return NV_ERR_NO_MEMORY;
        }
    }

    if (bBindingActive)
    {
        pBind = multimapFindItem(&pGpu->ratsEventBufferBindingsUid, 0, (NvU64)(NvUPtr)pEventBuffer);
        NV_ASSERT_OR_RETURN(pBind != NULL, NV_ERR_INVALID_STATE);
    }
    else
    {
        pBind = multimapInsertItemNew(&pGpu->ratsEventBufferBindingsUid, 0, (NvU64)(NvUPtr)pEventBuffer);
        if (pBind == NULL)
        {
            return NV_ERR_INVALID_ARGUMENT;
        }
        pBind->hClient = hClient;
        pBind->hNotifier = hNotifier;
        pBind->hEventBuffer = hEventBuffer;
        pBind->pEventBuffer = pEventBuffer;
        pBind->pUserInfo = (NvU64)(NvUPtr)pRmClient->pUserInfo;

        status = registerEventNotification(&pEventBuffer->pListeners,
                                           pClient,
                                           hNotifier,
                                           hEventBuffer,
                                           NV_EVENT_BUFFER_RECORD_TYPE_RATS_GSP_TRACE | NV01_EVENT_WITHOUT_EVENT_DATA,
                                           NV_EVENT_BUFFER_BIND,
                                           pEventBuffer->producerInfo.notificationHandle,
                                           NV_FALSE);

        if (status != NV_OK)
            goto done;
    }

    NV2080_CTRL_CMD_INTERNAL_CONTROL_GSP_TRACE_PARAMS ctrlParams = {0};
    ctrlParams.tracepointMask = tracepointMask;
    ctrlParams.bufferSize = gspTracingBufferSize;
    ctrlParams.bufferWatermark = gspTracingBufferWatermark;
    ctrlParams.targetTask = targetTask;
    ctrlParams.flag = NV2080_CTRL_CMD_INTERNAL_CONTROL_GSP_TRACE_FLAG_START_KEEP_OLDEST;

    if (IS_VIRTUAL(pGpu))
    {
        NvU32 allocBufferSize;
        NvU32 mulResult;

        NV_CHECK_OR_RETURN(LEVEL_ERROR,
                           portSafeMulU32(gspTracingBufferSize, (NvU32)sizeof(NV_RATS_RECORD), &mulResult),
                           NV_ERR_INVALID_ARGUMENT);
        NV_CHECK_OR_RETURN(LEVEL_ERROR,
                           portSafeAddU32(mulResult, (NvU32)sizeof(NV_RATS_VGPU_GSP_TRACING_BUFFER), &allocBufferSize),
                           NV_ERR_INVALID_ARGUMENT);
        NV_CHECK_OR_RETURN(LEVEL_ERROR, allocBufferSize <= GSP_TRACE_BUFFER_ALLOC_SIZE_MAX,
                           NV_ERR_INVALID_ARGUMENT);
        NV_ASSERT_OK_OR_GOTO(status,
            memdescCreate(&(pBind->pMemDesc),
                          pGpu,
                          allocBufferSize,
                          0, //alignment
                          NV_TRUE, ADDR_FBMEM, NV_MEMORY_UNCACHED,
                          MEMDESC_FLAGS_NONE),
            done);
        memdescAlloc(pBind->pMemDesc);

        pBind->message_buffer = memdescMapInternal(pGpu, pBind->pMemDesc, TRANSFER_FLAGS_NONE);
        ctrlParams.bufferAddr = (NvU64) pBind->pMemDesc->_pteArray[0];

        portMemSet(pBind->message_buffer, 0, allocBufferSize);
    }

    status = pRmApi->Control(pRmApi, hInternalClient, hInternalSubdevice,
                             NV2080_CTRL_CMD_INTERNAL_CONTROL_GSP_TRACE,
                             &ctrlParams, sizeof(ctrlParams));

    if (status != NV_OK)
        goto done;

    pGpu->ratsLoggingBufferActive = NV_TRUE;
    if (!bBindingActive)
    {
        status = osSchedule1HzCallback(pGpu,
                                       gspRatsNotifyAllConsumers,
                                       NULL,
                                       NV_OS_1HZ_REPEAT);

        if (status != NV_OK)
        {
            status = NV_ERR_INSUFFICIENT_RESOURCES;
        }
        
        bScheduled = NV_TRUE;
    }

done:
    if (status != NV_OK)
    {
        gspRatsRemoveBindpoint(pGpu, 0, pBind);
        if (bScheduled)
        {
            osRemove1HzCallback(pGpu, gspRatsNotifyAllConsumers, NULL);
        }
    }
    else if (!bBindingActive)
    {
        ++pGpu->ratsConsumerCount;
    }
    return status;
}

void
gspRatsRemoveBindpoint
(
    OBJGPU *pGpu,
    NvU64 uid,
    NV_EVENT_BUFFER_BIND_POINT_GSP_RATS *pBind
)
{
    RM_API *pRmApi              = GPU_GET_PHYSICAL_RMAPI(pGpu);
    EventBuffer *pEventBuffer   = pBind->pEventBuffer;
    NvU32 hInternalClient       = pGpu->hInternalClient;
    NvU32 hInternalSubdevice    = pGpu->hInternalSubdevice;

    if (pGpu->ratsConsumerCount == 0)
        return;

    --pGpu->ratsConsumerCount;

    unregisterEventNotificationWithData(&pEventBuffer->pListeners,
            pBind->hClient,
            pBind->hNotifier,
            pBind->hEventBuffer,
            NV_TRUE,
            pEventBuffer->producerInfo.notificationHandle);

    multimapRemoveItemByKey(&pGpu->ratsEventBufferBindingsUid,
            uid,
            (NvU64)(NvUPtr)pEventBuffer);

    
    osRemove1HzCallback(pGpu, gspRatsNotifyAllConsumers, NULL);

    if (pGpu->ratsLoggingBufferActive)
    {
        NV2080_CTRL_CMD_INTERNAL_CONTROL_GSP_TRACE_PARAMS params = {0};
        params.flag = NV2080_CTRL_CMD_INTERNAL_CONTROL_GSP_TRACE_FLAG_STOP;

        pRmApi->Control(pRmApi, hInternalClient, hInternalSubdevice,
                        NV2080_CTRL_CMD_INTERNAL_CONTROL_GSP_TRACE,
                        &params, sizeof(params));
        if (IS_VIRTUAL(pGpu))
        {
            memdescUnmapInternal(pGpu, pBind->pMemDesc, TRANSFER_FLAGS_NONE);
            memdescFree(pBind->pMemDesc);
            memdescDestroy(pBind->pMemDesc);
            pBind->pMemDesc = NULL;
            pBind->message_buffer = NULL;
        }
        pGpu->ratsLoggingBufferActive = NV_FALSE;
    }
}

void
gspRatsRemoveAllBindpoints
(
    EventBuffer *pEventBuffer
)
{
    OBJGPU *pGpu = NULL;
    NvU32 gpuMask = 0;
    NvU32 gpuIndex = 0;
    RatsEventBufferBindMultiMapSupermapIter iter;

    gpumgrGetGpuAttachInfo(NULL, &gpuMask);
    while ((pGpu = gpumgrGetNextGpu(gpuMask, &gpuIndex)) != NULL)
    {
        iter = multimapSubmapIterAll(&pGpu->ratsEventBufferBindingsUid);
        while (multimapSubmapIterNext(&iter))
        {
            RatsEventBufferBindMultiMapSubmap *pSubmap = iter.pValue;
            NV_EVENT_BUFFER_BIND_POINT_GSP_RATS *pBind = NULL;
            NvU64 uid = mapKey_IMPL(iter.iter.pMap, pSubmap);

            while ((pBind = multimapFindItem(&pGpu->ratsEventBufferBindingsUid,
                            uid,
                            (NvU64)(NvUPtr)pEventBuffer)) != NULL)
            {
                gspRatsRemoveBindpoint(pGpu, uid, pBind);
            }
        }
    }
}
