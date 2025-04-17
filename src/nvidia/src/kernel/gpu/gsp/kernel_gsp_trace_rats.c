/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * @brief Kernel Side GSP Tracing Functions
 */

#include "gpu/gsp/kernel_gsp_trace_rats.h"

#include "rmapi/client.h"
#include "rmapi/event.h"
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

static
NV_STATUS
_gspTraceEventBufferAdd
(
    OBJGPU *pGpu,
    NV_EVENT_BUFFER_BIND_POINT_GSP_TRACE *pBind,
    NV_RATS_GSP_TRACE_RECORD *pRecord
)
{
    NV_STATUS status;
    NvBool bNotify;
    NvP64 notificationHandle;
    EVENT_BUFFER_PRODUCER_DATA notifyEvent;
    NvU32 notifyIndex = NV_EVENT_BUFFER_RECORD_TYPE_RATS_GSP_TRACE;
    OBJTMR *pTmr = GPU_GET_TIMER(pGpu);

    pRecord->seqNo = pBind->pEventBuffer->seqNo++;
    pRecord->timeStamp += pTmr->sysTimerOffsetNs;

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

static void _gspTraceReadVgpuTracingBuffer(OBJGPU *pGpu, NV_RATS_VGPU_GSP_TRACING_BUFFER *pVgpuGspTracingBuffer)
{
    // Allocated memory for buffer entries located immediately after header
    NV_RATS_GSP_TRACE_RECORD *pRecords = (NV_RATS_GSP_TRACE_RECORD*) (pVgpuGspTracingBuffer + 1);

    // Set static end point so we don't read forever
    NvU32 read_end = pVgpuGspTracingBuffer->write;

    while(pVgpuGspTracingBuffer->read != read_end)
    {
        gspTraceEventBufferLogRecord(pGpu, &pRecords[pVgpuGspTracingBuffer->read]);
        pVgpuGspTracingBuffer->read = (pVgpuGspTracingBuffer->read + 1) % pVgpuGspTracingBuffer->bufferSize;
    }

    pVgpuGspTracingBuffer->lastReadTimestamp = osGetTimestamp();
}

void gspTraceNotifyAllConsumers
(
    OBJGPU  *pGpu,
    void    *pArgs
)
{
    GspTraceEventBufferBindMultiMapSubmap *pSubmap = multimapFindSubmap(&pGpu->gspTraceEventBufferBindingsUid, 0);
    if (pSubmap != NULL)
    {
        GspTraceEventBufferBindMultiMapIter iter = multimapSubmapIterItems(&pGpu->gspTraceEventBufferBindingsUid, pSubmap);
        while (multimapItemIterNext(&iter))
        {
            NV_EVENT_BUFFER_BIND_POINT_GSP_TRACE *pBind = iter.pValue;

            if (IS_VIRTUAL(pGpu))
            {
                NV_RATS_VGPU_GSP_TRACING_BUFFER *pVgpuGspTracingBuffer = (NV_RATS_VGPU_GSP_TRACING_BUFFER*)pBind->message_buffer;
                if (osGetTimestamp() - pVgpuGspTracingBuffer->lastReadTimestamp > osGetTimestampFreq())
                {
                    // vGPU plugin has no 1 second callback to empty buffer, so if we haven't read traces in over a second,
                    // read from GSP plugin to keep buffer up to date.
                    _gspTraceReadVgpuTracingBuffer(pGpu, pVgpuGspTracingBuffer);
                }
            }

            osEventNotification(pGpu,
                pBind->pEventBuffer->pListeners,
                NV_EVENT_BUFFER_RECORD_TYPE_RATS_GSP_TRACE,
                NULL,
                0);             // Do not copy structure -- embedded pointers.
        }
    }
}

void
gspTraceEventBufferLogRecord
(
    OBJGPU *pGpu,
    NV_RATS_GSP_TRACE_RECORD *pRecord
)
{
    GspTraceEventBufferBindMultiMapSubmap *pSubmap = multimapFindSubmap(&pGpu->gspTraceEventBufferBindingsUid, 0);
    if (pSubmap != NULL)
    {
        GspTraceEventBufferBindMultiMapIter iter = multimapSubmapIterItems(&pGpu->gspTraceEventBufferBindingsUid, pSubmap);
        while (multimapItemIterNext(&iter))
        {
            NV_EVENT_BUFFER_BIND_POINT_GSP_TRACE *pBind = iter.pValue;
            _gspTraceEventBufferAdd(pGpu, pBind, pRecord);
        }
    }
}

void gspTraceServiceVgpuEventTracing(OBJGPU *pGpu)
{
    GspTraceEventBufferBindMultiMapSubmap *pSubmap = multimapFindSubmap(&pGpu->gspTraceEventBufferBindingsUid, 0);
    NV_EVENT_BUFFER_BIND_POINT_GSP_TRACE *pBind = NULL;
    NV_RATS_VGPU_GSP_TRACING_BUFFER *pVgpuGspTracingBuffer;

    if (pSubmap != NULL)
    {
        GspTraceEventBufferBindMultiMapIter iter = multimapSubmapIterItems(&pGpu->gspTraceEventBufferBindingsUid, pSubmap);
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
    _gspTraceReadVgpuTracingBuffer(pGpu, pVgpuGspTracingBuffer);
}

NV_STATUS
gspTraceAddBindpoint
(
    OBJGPU *pGpu,
    RsClient *pClient,
    RsResourceRef *pEventBufferRef,
    NvHandle hNotifier,
    NvU64 tracepointMask,
    NvU32 gspTracingBufferSize,
    NvU32 gspTracingBufferWatermark
)
{

    NvHandle hClient            = pClient->hClient;
    NvU32 hInternalClient       = pGpu->hInternalClient;
    NvU32 hInternalSubdevice    = pGpu->hInternalSubdevice;
    NvHandle hEventBuffer       = pEventBufferRef->hResource;
    RmClient *pRmClient         = dynamicCast(pClient, RmClient);
    RM_API *pRmApi              = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NvU64 targetUser            = 0; // one root user app, for now...
    NvBool bBindingActive       = pGpu->gspTraceConsumerCount > 0;
    NvBool bScheduled           = NV_FALSE;

    NV_STATUS status;
    EventBuffer *pEventBuffer;

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmDeviceGpuLockIsOwner(pGpu->gpuInstance),
        NV_ERR_INVALID_LOCK_STATE);

    if (bBindingActive)
        return NV_ERR_INSUFFICIENT_RESOURCES;

    pEventBuffer = dynamicCast(pEventBufferRef->pResource, EventBuffer);
    if (pEventBuffer == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    if (NULL == multimapFindSubmap(&pGpu->gspTraceEventBufferBindingsUid, targetUser))
    {
        if (NULL == multimapInsertSubmap(&pGpu->gspTraceEventBufferBindingsUid, targetUser))
        {
            return NV_ERR_NO_MEMORY;
        }
    }

    NV_EVENT_BUFFER_BIND_POINT_GSP_TRACE *pBind = multimapInsertItemNew(&pGpu->gspTraceEventBufferBindingsUid, 0, (NvU64)(NvUPtr)pEventBuffer);
    if (pBind == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    pBind->hClient = hClient;
    pBind->hNotifier = hNotifier;
    pBind->hEventBuffer = hEventBuffer;
    pBind->pEventBuffer = pEventBuffer;
    pBind->pUserInfo = (NvU64)(NvUPtr)pRmClient->pUserInfo;

    ++pGpu->gspTraceConsumerCount;
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


    NV2080_CTRL_CMD_INTERNAL_CONTROL_GSP_TRACE_PARAMS ctrlParams = {0};
    ctrlParams.tracepointMask = tracepointMask;
    ctrlParams.bufferSize = gspTracingBufferSize;
    ctrlParams.bufferWatermark = gspTracingBufferWatermark;
    ctrlParams.flag = NV2080_CTRL_CMD_INTERNAL_CONTROL_GSP_TRACE_FLAG_START_KEEP_OLDEST;

    if (IS_VIRTUAL(pGpu))
    {
        NvU32 allocBufferSize = gspTracingBufferSize * sizeof(NV_RATS_GSP_TRACE_RECORD) + sizeof(NV_RATS_VGPU_GSP_TRACING_BUFFER);
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

    pGpu->gspTraceLoggingBufferActive = NV_TRUE;

    status = osSchedule1HzCallback(pGpu,
                                   gspTraceNotifyAllConsumers,
                                   NULL,
                                   NV_OS_1HZ_REPEAT);

    if (status != NV_OK)
        status = NV_ERR_INSUFFICIENT_RESOURCES;

    bScheduled = NV_TRUE;

done:
    if (status != NV_OK)
    {
        gspTraceRemoveBindpoint(pGpu, 0, pBind);
        if (bScheduled)
        {
            osRemove1HzCallback(pGpu, gspTraceNotifyAllConsumers, NULL);
        }
    }
    return status;
}

void
gspTraceRemoveBindpoint
(
    OBJGPU *pGpu,
    NvU64 uid,
    NV_EVENT_BUFFER_BIND_POINT_GSP_TRACE *pBind
)
{
    RM_API *pRmApi              = GPU_GET_PHYSICAL_RMAPI(pGpu);
    EventBuffer *pEventBuffer   = pBind->pEventBuffer;
    NvU32 hInternalClient       = pGpu->hInternalClient;
    NvU32 hInternalSubdevice    = pGpu->hInternalSubdevice;

    if (pGpu->gspTraceConsumerCount == 0)
        return;

    --pGpu->gspTraceConsumerCount;

    unregisterEventNotificationWithData(&pEventBuffer->pListeners,
            pBind->hClient,
            pBind->hNotifier,
            pBind->hEventBuffer,
            NV_TRUE,
            pEventBuffer->producerInfo.notificationHandle);

    multimapRemoveItemByKey(&pGpu->gspTraceEventBufferBindingsUid,
            uid,
            (NvU64)(NvUPtr)pEventBuffer);

    if (pGpu->gspTraceConsumerCount == 0)
    {
        osRemove1HzCallback(pGpu, gspTraceNotifyAllConsumers, NULL);

        if (pGpu->gspTraceLoggingBufferActive)
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

            pGpu->gspTraceLoggingBufferActive = NV_FALSE;
        }
    }
}

void
gspTraceRemoveAllBindpoints
(
    EventBuffer *pEventBuffer
)
{
    OBJGPU *pGpu = NULL;
    NvU32 gpuMask = 0;
    NvU32 gpuIndex = 0;
    GspTraceEventBufferBindMultiMapSupermapIter iter;

    gpumgrGetGpuAttachInfo(NULL, &gpuMask);
    while ((pGpu = gpumgrGetNextGpu(gpuMask, &gpuIndex)) != NULL)
    {
        iter = multimapSubmapIterAll(&pGpu->gspTraceEventBufferBindingsUid);
        while (multimapSubmapIterNext(&iter))
        {
            GspTraceEventBufferBindMultiMapSubmap *pSubmap = iter.pValue;
            NV_EVENT_BUFFER_BIND_POINT_GSP_TRACE *pBind = NULL;
            NvU64 uid = mapKey_IMPL(iter.iter.pMap, pSubmap);

            while ((pBind = multimapFindItem(&pGpu->gspTraceEventBufferBindingsUid,
                            uid,
                            (NvU64)(NvUPtr)pEventBuffer)) != NULL)
            {
                gspTraceRemoveBindpoint(pGpu, uid, pBind);
            }
        }
    }
}
