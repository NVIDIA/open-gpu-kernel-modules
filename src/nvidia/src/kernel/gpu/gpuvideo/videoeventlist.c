/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/***************************************************************************\
*                                                                          *
* Module: videoeventlist.c                                                 *
*   Description:                                                           *
*       This module contains an implementation of the Event Buffer         *
*        callback for video events                                         *
*                                                                          *
\***************************************************************************/

#include "rmapi/client.h"
#include "rmapi/event.h"
#include "rmapi/event_buffer.h"
#include "resserv/rs_server.h"
#include "gpu_mgr/gpu_mgr.h"
#include "core/locks.h"
#include "os/os.h"
#include "gpuvideo/video_event.h"
#include "gpuvideo/videoeventlist.h"
#include "kernel/gpu/timer/objtmr.h"
#include "kernel/gpu/video/kernel_video_engine.h"
#include "kernel/gpu/fifo/kernel_channel_group.h"
#include "kernel/gpu/bus/kern_bus.h"

#include "class/cl90cd.h"
#include "class/cl90cdtypes.h"
#include "class/cl90cdvideo.h"

#include "ctrl/ctrl2080/ctrl2080internal.h"

#define NV_VIDEO_TRACE_CALLBACK_TIME_NS 50000000            // Approximating 20Hz callback

/*!
 * This helper function initializes the context used for video event trace.
 */
NV_STATUS
videoEventTraceCtxInit
(
    OBJGPU *pGpu,
    KernelChannel *pKernelChannel,
    ENGDESCRIPTOR engDesc
)
{
    KernelVideoEngine *pKernelVideoEngine;
    MEMORY_DESCRIPTOR *pCtxMemDesc;
    VIDEO_ENGINE_EVENT__LOG_INFO logInfo;

    if (RMCFG_FEATURE_PLATFORM_GSP || !IS_VIDEO_ENGINE(engDesc))
        return NV_OK;

    pKernelVideoEngine = kvidengFromEngDesc(pGpu, engDesc);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pKernelVideoEngine != NULL, NV_OK);
    NV_CHECK_OR_RETURN(LEVEL_SILENT, pKernelVideoEngine->bVideoTraceEnabled, NV_OK);
    NV_CHECK_OR_RETURN(LEVEL_SILENT, pKernelVideoEngine->videoTraceInfo.pTraceBufferEngineMemDesc != NULL, NV_OK);

    // Fill some channel specific information for event logging
    logInfo.userInfo = (NvU64)(NvUPtr)pKernelChannel->pUserInfo;
    logInfo.pid = pKernelChannel->ProcessID;
    logInfo.context_id = kchannelGetCid(pKernelChannel);
    logInfo.engine_id = ENGDESC_FIELD(engDesc, _INST);
    logInfo.gfid = kchannelGetGfid(pKernelChannel);

    kchangrpGetEngineContextMemDesc(pGpu,
                                    pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup,
                                    &pCtxMemDesc);

    if (pCtxMemDesc != NULL)
    {
        NvU8 *pInstMem;
        NvU32 i;
        NvU32 *pLogInfo = (NvU32 *)&logInfo;

        // Is context allocation too small to hold the client info for event trace?
        NV_CHECK_OR_RETURN(LEVEL_INFO,
            memdescGetSize(pCtxMemDesc) >= (VIDEO_ENGINE_EVENT__LOG_INFO__OFFSET + VIDEO_ENGINE_EVENT__LOG_INFO__SIZE),
            NV_ERR_BUFFER_TOO_SMALL);

        pInstMem = kbusMapRmAperture_HAL(pGpu, pCtxMemDesc);
        NV_CHECK_OR_RETURN(LEVEL_ERROR, pInstMem != NULL, NV_ERR_INSUFFICIENT_RESOURCES);

        for (i = 0; i < sizeof(VIDEO_ENGINE_EVENT__LOG_INFO); i += 4)
        {
            // Initialize client information in context allocation.
            MEM_WR32(pInstMem + VIDEO_ENGINE_EVENT__LOG_INFO__OFFSET + i, pLogInfo[i / sizeof(NvU32)]);
        }

        // write kernel event buffer address
        NvU64 dmaAddr = memdescGetPhysAddr(pKernelVideoEngine->videoTraceInfo.pTraceBufferEngineMemDesc, AT_GPU, 0);
        MEM_WR32(pInstMem + VIDEO_ENGINE_EVENT__TRACE_ADDR__OFFSET_LO, NvU64_LO32(dmaAddr));
        MEM_WR32(pInstMem + VIDEO_ENGINE_EVENT__TRACE_ADDR__OFFSET_HI, NvU64_HI32(dmaAddr));

        kbusUnmapRmAperture_HAL(pGpu, pCtxMemDesc, &pInstMem, NV_TRUE);
    }

    return NV_OK;
}

static
NV_STATUS
_videoEventBufferAdd
(
    OBJGPU *pGpu,
    NV_EVENT_BUFFER_BIND_POINT_VIDEO *pBind,
    NOTIFY_VIDEO_EVENT *pNotifyRecord,
    NvU32 *pLogData,
    NvBool bSanitizeUser,
    NvBool bSanitizeKernel
)
{
    NV_STATUS status;
    NvBool bNotify;
    NvP64 notificationHandle;
    EVENT_BUFFER_PRODUCER_DATA notifyEvent;
    NvU32 notifyIndex = NV_EVENT_BUFFER_RECORD_TYPE_VIDEO_TRACE;
    VIDEO_ENGINE_EVENT__RECORD const * pRecord;

    if (pNotifyRecord == NULL)
    {
        return NV_OK;
    }
    pRecord = (VIDEO_ENGINE_EVENT__RECORD const *)(pNotifyRecord->pEventData);

    portMemSet(&notifyEvent, 0, sizeof(notifyEvent));
    notifyEvent.pVardata = NV_PTR_TO_NvP64(NULL);
    notifyEvent.vardataSize = 0;

    NV_EVENT_BUFFER_VIDEO_RECORD_V1 videoRecord;
    portMemSet(&videoRecord, 0, sizeof(videoRecord));
    videoRecord.event_id = pRecord->event_id;
    videoRecord.timestamp = pRecord->ts;
    videoRecord.seqno = pRecord->seq_no;
    videoRecord.context_id = pRecord->context_id;
    videoRecord.pid = pRecord->pid;
    videoRecord.api_id = pRecord->api_id;

    if (bSanitizeKernel)
    {
        videoRecord.pid = NV_EVENT_BUFFER_VIDEO_KERNEL_PID;
        videoRecord.context_id = NV_EVENT_BUFFER_VIDEO_KERNEL_CONTEXT;
        videoRecord.api_id = NV_EVENT_BUFFER_VIDEO_KERNEL_CONTEXT;
    }
    else if (bSanitizeUser)
    {
        videoRecord.pid = NV_EVENT_BUFFER_VIDEO_HIDDEN_PID;
        videoRecord.context_id = NV_EVENT_BUFFER_VIDEO_HIDDEN_CONTEXT;
        videoRecord.api_id = NV_EVENT_BUFFER_VIDEO_HIDDEN_CONTEXT;
    }

#if PORT_IS_MODULE_SUPPORTED(crypto)
    // Randomized timestamp if sanitization is needed
    if (bSanitizeKernel || bSanitizeUser)
    {
        //
        // pNotifyRecord->noisyTimestampStart is copied from pKernelVideoEngine->videoTraceInfo->noisyTimestampStart which is initialized to 0
        // during engine initialization when trace surface is allocated before session starts. pKernelVideoEngine->videoTraceInfo->noisyTimestampStart
        // is recorded with the timestamp of a SESSION_START event. Engine will always log event in sequence and every SESSION_END event 
        // should have a SESSION_START event in front of it. Also, we are assuming NSight will discard a SESSION_END event 
        // without a SESSION_START event before it.
        //
        if ((pNotifyRecord->noisyTimestampStart !=  videoRecord.timestamp) && (pNotifyRecord->pVideoLogPrng != NULL))
        {
            NvU64 noisyTimestampRange;
            // The range is always non-zero since we had check
            //    (pNotifyRecord->noisyTimestampStart !=  videoRecord.timestamp)
            // above. 
            noisyTimestampRange = (videoRecord.timestamp >= pNotifyRecord->noisyTimestampStart) 
                                  ? (videoRecord.timestamp - pNotifyRecord->noisyTimestampStart) 
                                  : (((NvU64)(-1) - pNotifyRecord->noisyTimestampStart) + videoRecord.timestamp);
            videoRecord.timestamp = pNotifyRecord->noisyTimestampStart 
                                    + portCryptoPseudoRandomGeneratorGetU32(pNotifyRecord->pVideoLogPrng) % noisyTimestampRange;
        }
    }
#endif // PORT_IS_MODULE_SUPPORTED(crypto)

    switch (pRecord->event_id)
    {
        case VIDEO_ENGINE_EVENT_ID__SESSION_START:
            videoRecord.session.engine_type = pRecord->event_start.engine_type;
            videoRecord.session.engine_id = pRecord->event_start.engine_id;
            videoRecord.session.codec_id = pRecord->event_start.codec_id;
            break;
        case VIDEO_ENGINE_EVENT_ID__SESSION_END:
            videoRecord.session.engine_type = pRecord->event_start.engine_type;
            videoRecord.session.engine_id = pRecord->event_end.engine_id;
            videoRecord.session.codec_id = pRecord->event_end.codec_id;
            videoRecord.session.status = pRecord->event_end.status;
            break;
        case VIDEO_ENGINE_EVENT_ID__POWER_STATE_CHANGE:
            videoRecord.stateChange.to = pRecord->event_pstate_change.to;
            videoRecord.stateChange.from = pRecord->event_pstate_change.from;
            break;
        case VIDEO_ENGINE_EVENT_ID__LOG_DATA:
            videoRecord.logData.engine_type = pRecord->event_start.engine_type;
            videoRecord.logData.engine_id = pRecord->event_start.engine_id;
            videoRecord.logData.codec_id = pRecord->event_start.codec_id;
            videoRecord.logData.size = pRecord->event_log_data.size;
            notifyEvent.pVardata = NV_PTR_TO_NvP64(pLogData);
            notifyEvent.vardataSize = videoRecord.logData.size;
            break;
        default:
            videoRecord.event_data = pRecord->event_data;
    }

    notifyEvent.pPayload = NV_PTR_TO_NvP64(&videoRecord);
    notifyEvent.payloadSize = sizeof(videoRecord);

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

static void _notifyEventBuffers
(
    OBJGPU *pGpu,
    VideoEventBufferBindMultiMapSubmap *pSubmap,
    NOTIFY_VIDEO_EVENT *pNotifyRecord,
    NvU32 *pLogData
)
{
    VIDEO_ENGINE_EVENT__RECORD const * pRecord = (VIDEO_ENGINE_EVENT__RECORD const *)pNotifyRecord->pEventData;

    if (pSubmap != NULL)
    {
        VideoEventBufferBindMultiMapIter iter = multimapSubmapIterItems(&pGpu->videoEventBufferBindingsUid, pSubmap);

        while (multimapItemIterNext(&iter))
        {
            NV_EVENT_BUFFER_BIND_POINT_VIDEO* pBind = iter.pValue;
            NvBool bSanitizeKernel = (!pBind->bKernel) && (pRecord->userInfo == 0);
            NvBool bSanitizeUser = (!pBind->bAdmin) && (pBind->pUserInfo != pRecord->userInfo);

            if (!(NVBIT(pRecord->event_id) & pBind->eventMask))
                continue;

            _videoEventBufferAdd(pGpu, pBind, pNotifyRecord, pLogData, bSanitizeUser, bSanitizeKernel);
        }
    }
}

static void _videoGetTraceEvents
(
    OBJGPU  *pGpu,
    KernelVideoEngine *pKernelVideoEngine,
    VideoEventBufferBindMultiMapSubmap *pSubmapAll
)
{
    VideoEventBufferBindMultiMapSubmap *pSubmapUserOnly = NULL;
    NvU64 cachedUserInfo = 0;
    NvU32 magicHi = ENG_VIDEO_TRACE_EVENT_MAGIC_HI;
    NvU32 magicLo = ENG_VIDEO_TRACE_EVENT_MAGIC_LO;
    VIDEO_TRACE_RING_BUFFER *pRingbuffer;
    VIDEO_ENGINE_EVENT__RECORD videoRecord;
    NvU32 gotSize;

    NV_ASSERT_OR_RETURN_VOID(pKernelVideoEngine != NULL);
    NV_CHECK_OR_RETURN_VOID(LEVEL_INFO, pKernelVideoEngine->bVideoTraceEnabled);

    pRingbuffer = pKernelVideoEngine->videoTraceInfo.pTraceBufferEngine;

    if (pRingbuffer == NULL)
        return;

    while (kvidengRingbufferGetDataSize(pGpu, pRingbuffer) >= sizeof(VIDEO_ENGINE_EVENT__RECORD))
    {
        NOTIFY_VIDEO_EVENT notifyRecord;

        NvU32 oldReadPtr = pRingbuffer->readPtr;

        gotSize = kvidengEventbufferGetRecord(pGpu,
                                              pKernelVideoEngine,
                                              pRingbuffer,
                                              &videoRecord,
                                              magicHi,
                                              magicLo);

        // If the read pointer was not moved by us, this record may be invalid
        if ((oldReadPtr + sizeof(VIDEO_ENGINE_EVENT__RECORD)) != pRingbuffer->readPtr)
            continue;

        if (gotSize == 0)
            continue;

        if (videoRecord.event_id == VIDEO_ENGINE_EVENT_ID__SESSION_START)
        {
            pKernelVideoEngine->videoTraceInfo.noisyTimestampStart = videoRecord.ts;
        }
        notifyRecord.noisyTimestampStart = pKernelVideoEngine->videoTraceInfo.noisyTimestampStart;
        notifyRecord.pVideoLogPrng = pKernelVideoEngine->videoTraceInfo.pVideoLogPrng;
        notifyRecord.pEventData = (void *)(&videoRecord);

        if (videoRecord.userInfo != 0)
        {
            if (cachedUserInfo != videoRecord.userInfo)
            {
                pSubmapUserOnly = multimapFindSubmap(&pGpu->videoEventBufferBindingsUid, videoRecord.userInfo);
                cachedUserInfo = videoRecord.userInfo;
            }

            _notifyEventBuffers(pGpu,
                                pSubmapUserOnly,
                                &notifyRecord,
                                (NvU32 *)(pKernelVideoEngine->videoTraceInfo.pTraceBufferVariableData));
        }

        _notifyEventBuffers(pGpu,
                            pSubmapAll,
                            &notifyRecord,
                            (NvU32 *)(pKernelVideoEngine->videoTraceInfo.pTraceBufferVariableData));
    }
}

static NV_STATUS
_videoEventBufferSetFlag(OBJGPU *pGpu, NvU32 flag)
{
    NvU32 i;
    for (i = 0; i < pGpu->numKernelVideoEngines; i++)
    {
        KernelVideoEngine *pKernelVideoEngine = pGpu->kernelVideoEngines[i];
        if (pKernelVideoEngine != NULL)
        {
            VIDEO_TRACE_RING_BUFFER *pRingbuffer = pKernelVideoEngine->videoTraceInfo.pTraceBufferEngine;
            NvBool bAlwaysLogging = pKernelVideoEngine->videoTraceInfo.bAlwaysLogging;
            if (pRingbuffer != NULL && !bAlwaysLogging)
            {
                pRingbuffer->flags = flag;
            }
        }
    }

    return NV_OK;
}

static void
_videoOsWorkItem
(
    NvU32 gpuInstance,
    void *data
)
{
    OBJGPU *pGpu = gpumgrGetGpu(gpuInstance);

    nvEventBufferVideoCallback(pGpu, NULL);
}

static NV_STATUS
_videoTimerCallback
(
    OBJGPU *pGpu,
    OBJTMR *pTmr,
    TMR_EVENT *pTmrEvent
)
{
    NV_STATUS status;

    NV_CHECK_OK(status, LEVEL_ERROR, osQueueWorkItemWithFlags(pGpu, _videoOsWorkItem, NULL, OS_QUEUE_WORKITEM_FLAGS_LOCK_GPU_GROUP_DEVICE));

    // TMR_FLAG_RECUR does not work, so reschedule it here.
    NV_CHECK_OK_OR_CAPTURE_FIRST_ERROR(status, LEVEL_ERROR, tmrEventScheduleRel(pTmr, pTmrEvent, NV_VIDEO_TRACE_CALLBACK_TIME_NS));

    return status;
}

static NV_STATUS
_videoTimerCreate
(
    OBJGPU *pGpu
)
{
    OBJTMR *pTmr = GPU_GET_TIMER(pGpu);
    NvU32 timerFlags = TMR_FLAG_RECUR;
    // Unix needs to use the OS timer to avoid corrupting records, but Windows doesn't have an OS timer implementation
    timerFlags |= TMR_FLAG_USE_OS_TIMER;

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        tmrEventCreate(pTmr, &pGpu->pVideoTimerEvent, _videoTimerCallback, NULL, timerFlags));

    // This won't be a true 20Hz timer as the callbacks are scheduled from the time they're called
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        tmrEventScheduleRel(pTmr, pGpu->pVideoTimerEvent, NV_VIDEO_TRACE_CALLBACK_TIME_NS));

    return NV_OK;
}

static void
_videoTimerDestroy
(
    OBJGPU *pGpu
)
{
    if (pGpu->pVideoTimerEvent != NULL)
    {
        OBJTMR *pTmr = GPU_GET_TIMER(pGpu);

        tmrEventDestroy(pTmr, pGpu->pVideoTimerEvent);
        pGpu->pVideoTimerEvent = NULL;
    }
}

void
nvEventBufferVideoCallback
(
    OBJGPU  *pGpu,
    void    *pArgs
)
{
    VideoEventBufferBindMultiMapSubmap *pSubmapAll = NULL;
    NvU8 i;

    if (!rmDeviceGpuLockIsOwner(pGpu->gpuInstance))
    {
        NV_ASSERT(0);
        return;
    }

    if (pGpu->videoCtxswLogConsumerCount <= 0)
    {
        NV_ASSERT(pGpu->videoCtxswLogConsumerCount >= 0);
        return;
    }

    pSubmapAll = multimapFindSubmap(&pGpu->videoEventBufferBindingsUid, 0);
    if (pSubmapAll == NULL)
        return;

    for (i = 0; i < pGpu->numKernelVideoEngines; i++)
    {
        KernelVideoEngine *pKernelVideoEngine = pGpu->kernelVideoEngines[i];
        _videoGetTraceEvents(pGpu, pKernelVideoEngine, pSubmapAll);
    }

}

void
videoRemoveBindpoint
(
    OBJGPU *pGpu,
    NvU64 uid,
    NV_EVENT_BUFFER_BIND_POINT_VIDEO* pBind
)
{
    EventBuffer *pEventBuffer = pBind->pEventBuffer;

    --pGpu->videoCtxswLogConsumerCount;
    if (pGpu->videoCtxswLogConsumerCount == 0)
    {
        // When last client is unbound, disable engine event logging.
        _videoEventBufferSetFlag(pGpu, 0);
    }

    unregisterEventNotificationWithData(&pEventBuffer->pListeners,
            pBind->hClient,
            pBind->hNotifier,
            pBind->hEventBuffer,
            NV_TRUE,
            pEventBuffer->producerInfo.notificationHandle);

    multimapRemoveItemByKey(&pGpu->videoEventBufferBindingsUid,
            uid,
            (NvU64)(NvUPtr)pEventBuffer);
}

NV_STATUS
videoAddBindpoint
(
    OBJGPU *pGpu,
    RsClient *pClient,
    RsResourceRef *pEventBufferRef,
    NvHandle hNotifier,
    NvBool bAllUsers,
    NV2080_CTRL_EVENT_VIDEO_BIND_EVTBUF_LOD levelOfDetail,
    NvU32 eventFilter
)
{
    NV_STATUS status;
    NvHandle hClient = pClient->hClient;
    RmClient *pRmClient = dynamicCast(pClient, RmClient);
    NvHandle hEventBuffer = pEventBufferRef->hResource;
    EventBuffer *pEventBuffer;
    NvBool bVideoBindingActive = (pGpu->videoCtxswLogConsumerCount > 0);
    NvU64 targetUser;

    NvBool bAdmin = osIsAdministrator();
    NvBool bKernel;
    NvU32 eventMask = 0;
    NvBool bSelectLOD;

    CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
    NV_ASSERT_OR_RETURN(pCallContext != NULL, NV_ERR_INVALID_STATE);
    bKernel = pCallContext->secInfo.privLevel >= RS_PRIV_LEVEL_KERNEL;
    bSelectLOD = bKernel;

#if defined(DEBUG) || defined(DEVELOP) || defined(NV_VERIF_FEATURES)
    bSelectLOD = NV_TRUE;
#endif

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner() && rmDeviceGpuLockIsOwner(pGpu->gpuInstance));

    if (!kvidengIsVideoTraceLogSupported(pGpu))
        return NV_ERR_NOT_SUPPORTED;

    if (bSelectLOD)
    {
        switch(levelOfDetail)
        {
            case NV2080_CTRL_EVENT_VIDEO_BIND_EVTBUF_LOD_FULL:
                eventMask = ~0;
                break;
            case NV2080_CTRL_EVENT_VIDEO_BIND_EVTBUF_LOD_CUSTOM:
                eventMask = eventFilter;
                break;
            case NV2080_CTRL_EVENT_VIDEO_BIND_EVTBUF_LOD_SIMPLE:
            default:
                // Default to SIMPLIFIED level-of-detail
                eventMask |= NV_EVENT_BUFFER_VIDEO_BITMASK_TAG_ENGINE_START |
                             NV_EVENT_BUFFER_VIDEO_BITMASK_TAG_ENGINE_END;
        }
    }
    else
    {
        // Default to SIMPLIFIED level-of-detail
        eventMask |= NV_EVENT_BUFFER_VIDEO_BITMASK_TAG_ENGINE_START |
                     NV_EVENT_BUFFER_VIDEO_BITMASK_TAG_ENGINE_END;
    }

    if (bAllUsers)
    {
        targetUser = 0;
    }
    else
    {
        // Clients requesting only their own events will not work
        NV_ASSERT_OR_RETURN(bAllUsers, NV_ERR_INVALID_ARGUMENT);
    }

    pEventBuffer = dynamicCast(pEventBufferRef->pResource, EventBuffer);
    if (NULL == pEventBuffer)
        return NV_ERR_INVALID_ARGUMENT;

    if (NULL == multimapFindSubmap(&pGpu->videoEventBufferBindingsUid, targetUser))
    {
        if (NULL == multimapInsertSubmap(&pGpu->videoEventBufferBindingsUid, targetUser))
        {
            NV_PRINTF(LEVEL_ERROR, "failed to add UID binding!\n");
            return NV_ERR_INSUFFICIENT_RESOURCES;
        }
    }

    // If the binding exists already, we're done
    if (NULL != multimapFindItem(&pGpu->videoEventBufferBindingsUid, targetUser, (NvU64)(NvUPtr)pEventBuffer))
        return NV_OK;

    NV_EVENT_BUFFER_BIND_POINT_VIDEO* pBind = multimapInsertItemNew(&pGpu->videoEventBufferBindingsUid, 0, (NvU64)(NvUPtr)pEventBuffer);
    if (pBind == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    pBind->hClient = hClient;
    pBind->hNotifier = hNotifier;
    pBind->hEventBuffer = hEventBuffer;
    pBind->pEventBuffer = pEventBuffer;
    pBind->pUserInfo = (NvU64)(NvUPtr)pRmClient->pUserInfo;
    pBind->bAdmin = bAdmin;
    pBind->eventMask = eventMask;
    pBind->bKernel = bKernel;

    ++pGpu->videoCtxswLogConsumerCount;
    if (pGpu->videoCtxswLogConsumerCount == 1)
    {
        // When first client is bound, enable engine event logging.
        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
            _videoEventBufferSetFlag(pGpu, VIDEO_TRACE_FLAG__LOGGING_ENABLED),
            done);
    }

    status = registerEventNotification(&pEventBuffer->pListeners,
                pClient,
                hNotifier,
                hEventBuffer,
                NV_EVENT_BUFFER_RECORD_TYPE_VIDEO_TRACE | NV01_EVENT_WITHOUT_EVENT_DATA,
                NV_EVENT_BUFFER_BIND,
                pEventBuffer->producerInfo.notificationHandle,
                NV_FALSE);
    if (status != NV_OK)
        goto done;

    if (!bVideoBindingActive)
    {
        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR, _videoTimerCreate(pGpu), done);
    }

done:
    if (status != NV_OK)
    {
        videoRemoveBindpoint(pGpu, 0, pBind);

        _videoTimerDestroy(pGpu);
    }

    return status;
}

void
videoRemoveAllBindpointsForGpu
(
    OBJGPU *pGpu
)
{
    NvS16 prevConsumerCount = pGpu->videoCtxswLogConsumerCount;
    VideoEventBufferBindMultiMapSupermapIter iter;

    if (pGpu->videoEventBufferBindingsUid.real.base.map.pAllocator == NULL)
        return;

    iter = multimapSubmapIterAll(&pGpu->videoEventBufferBindingsUid);
    while (multimapSubmapIterNext(&iter))
    {
        VideoEventBufferBindMultiMapSubmap *pSubmap = iter.pValue;
        VideoEventBufferBindMultiMapIter subIter = multimapSubmapIterItems(&pGpu->videoEventBufferBindingsUid, pSubmap);
        NvU64 uid = mapKey_IMPL(iter.iter.pMap, pSubmap);

        while (multimapItemIterNext(&subIter))
        {
            NV_EVENT_BUFFER_BIND_POINT_VIDEO* pBind = subIter.pValue;
            videoRemoveBindpoint(pGpu, uid, pBind);
            subIter = multimapSubmapIterItems(&pGpu->videoEventBufferBindingsUid, pSubmap);
        }
    }

    if ((prevConsumerCount != 0) && (pGpu->videoCtxswLogConsumerCount == 0))
        videoBufferTeardown(pGpu);
}

void
videoRemoveAllBindpoints
(
    EventBuffer *pEventBuffer
)
{
    OBJGPU *pGpu = NULL;
    NvU32 gpuMask = 0;
    NvU32 gpuIndex = 0;
    VideoEventBufferBindMultiMapSupermapIter iter;

    gpumgrGetGpuAttachInfo(NULL, &gpuMask);
    while ((pGpu = gpumgrGetNextGpu(gpuMask, &gpuIndex)) != NULL)
    {
        iter = multimapSubmapIterAll(&pGpu->videoEventBufferBindingsUid);
        while (multimapSubmapIterNext(&iter))
        {
            VideoEventBufferBindMultiMapSubmap *pSubmap = iter.pValue;
            NV_EVENT_BUFFER_BIND_POINT_VIDEO* pBind = NULL;
            NvU64 uid = mapKey_IMPL(iter.iter.pMap, pSubmap);

            while ((pBind = multimapFindItem(&pGpu->videoEventBufferBindingsUid,
                            uid,
                            (NvU64)(NvUPtr)pEventBuffer)) != NULL)
            {
                videoRemoveBindpoint(pGpu, uid, pBind);
            }
        }

        if (pGpu->videoCtxswLogConsumerCount == 0)
            videoBufferTeardown(pGpu);
    }
}

void
videoBufferTeardown
(
    OBJGPU *pGpu
)
{
    _videoTimerDestroy(pGpu);
}

