/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
* Module: fecs_event_list.c                                                  *
*   Description:                                                           *
*       This module contains an implementation of the Event Buffer         *
*        callback for FECS events                                          *
*                                                                          *
\***************************************************************************/

#include "kernel/gpu/gr/kernel_graphics.h"
#include "kernel/rmapi/event.h"
#include "kernel/rmapi/event_buffer.h"
#include "libraries/resserv/rs_server.h"
#include "kernel/core/locks.h"
#include "kernel/os/os.h"
#include "kernel/gpu/gr/fecs_event_list.h"
#include "kernel/gpu/mig_mgr/gpu_instance_subscription.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "kernel/gpu/bus/kern_bus.h"
#include "kernel/gpu/mem_mgr/mem_mgr.h"
#include "kernel/gpu/fifo/kernel_channel.h"
#include "kernel/gpu/subdevice/subdevice.h"
#include "kernel/gpu/timer/objtmr.h"
#include "kernel/virtualization/hypervisor/hypervisor.h"
#include "rmapi/client.h"
#include "vgpu/sdk-structures.h"

#include "class/cl90cdtypes.h"
#include "ctrl/ctrl90cd.h"

#define NV_FECS_TRACE_MAX_TIMESTAMPS 5
#define NV_FECS_TRACE_MAGIC_INVALIDATED 0xdededede         // magic number for entries that have been read
#define NV_FECS_TRACE_CALLBACK_TIME_NS 33333333            // Approximating 30Hz callback

typedef struct
{
    NvU32 magic_lo;
    NvU32 magic_hi;
    NvU32 context_id;
    NvU32 context_ptr;
    NvU32 new_context_id;
    NvU32 new_context_ptr;
    NvU64 ts[NV_FECS_TRACE_MAX_TIMESTAMPS];
    NvU32 reserved[13];
    NvU32 seqno;
} FECS_EVENT_RECORD;

/*! Opaque pointer to private data */
typedef struct VGPU_FECS_TRACE_STAGING_BUFFER VGPU_FECS_TRACE_STAGING_BUFFER;

/*! Private FECS event buffer data stored per-KGR */
struct KGRAPHICS_FECS_TRACE_INFO
{
    NvU8  *pFecsBufferMapping;
    NvU16  fecsCtxswLogRecordsPerIntr;
    NvU16  fecsTraceRdOffset;
    NvU16  fecsTraceCounter;
    NvU32  fecsCtxswLogIntrPending;
    NvU32  fecsLastSeqno;

#if PORT_IS_MODULE_SUPPORTED(crypto)
    PORT_CRYPTO_PRNG *pFecsLogPrng;
#endif

    //
    // GR Routing information for GPU instance to which this engine is assigned if MIG is enabled.
    // Will be 0/NULL for unassigned GR engines or if MIG is disabled
    //
    NvHandle hClient;
    NvHandle hSubdevice;
    NvU32    localGrEngineIdx;

    // vGPU FECS staging eventbuffer (guest only)
    VGPU_FECS_TRACE_STAGING_BUFFER *pVgpuStaging;
};

/*! Private FECS event buffer data stored for the GPU as a whole */
struct KGRMGR_FECS_GLOBAL_TRACE_INFO
{
    // map: { UserInfo* -> { pEventBuffer -> NV_EVENT_BUFFER_BIND_POINT* }}
    FecsEventBufferBindMultiMap fecsEventBufferBindingsUid;

    // Timer event to periodically the processing callback
    TMR_EVENT *pFecsTimerEvent;

    // Timer interval in nanoseconds
    NvU32 fecsTimerInterval;

    // Atomic for scheduling the fecs callback in timer mode
    NvU32 fecsCallbackScheduled;

    // Number of consumer clients
    NvS16 fecsCtxswLogConsumerCount;
};

/*!
 * @brief      Function to populate client/subdevice/grRouteInfo from cached
 *             information in order to make calls into the specific MIG GPU instance
 *             to which a GR engine is assigned. If MIG is not enabled, GPU
 *             internal client/subdevice handles will be used and grRouteInfo is
 *             cleared
 *
 * @param[in]   pGpu
 * @param[in]   pKernelGraphics
 * @param[out]  phClient            Client handle to populate
 * @param[out]  phSubdevice         Subdevice handle to populate
 * @param[out]  pGrRouteInfo        Internal GR Routing info to populate
 */
static NV_STATUS
_fecsLoadInternalRoutingInfo
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics,
    NvHandle *phClient,
    NvHandle *phSubdevice,
    NV2080_CTRL_GR_ROUTE_INFO *pGrRouteInfo
)
{
    KGRAPHICS_FECS_TRACE_INFO *pFecsTraceInfo = kgraphicsGetFecsTraceInfo(pGpu, pKernelGraphics);

    NV_ASSERT_OR_RETURN(pFecsTraceInfo != NULL, NV_ERR_INVALID_STATE);

    portMemSet(pGrRouteInfo, 0, sizeof(*pGrRouteInfo));

    if (!IS_MIG_IN_USE(pGpu))
    {
        *phClient = pGpu->hInternalClient;
        *phSubdevice = pGpu->hInternalSubdevice;
        return NV_OK;
    }

    // GR Engines not allocated to any GPU instance will have null handles
    NV_CHECK_OR_RETURN(LEVEL_INFO, pFecsTraceInfo->hClient != NV01_NULL_OBJECT, NV_ERR_INVALID_ARGUMENT);

    kgrmgrCtrlSetEngineID(pFecsTraceInfo->localGrEngineIdx, pGrRouteInfo);
    *phClient = pFecsTraceInfo->hClient;
    *phSubdevice = pFecsTraceInfo->hSubdevice;

    return NV_OK;
}

static NV_STATUS
fecsExtractTagAndTimestamp
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics,
    NvU64 rawTimestamp,
    NvU64 *pTimestampVal,
    NvU8 *pTag
)
{
    const KGRAPHICS_STATIC_INFO *pKernelGraphicsStaticInfo = kgraphicsGetStaticInfo(pGpu, pKernelGraphics);
    NV_ASSERT_OR_RETURN(pKernelGraphicsStaticInfo != NULL, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(pKernelGraphicsStaticInfo->pFecsTraceDefines != NULL, NV_ERR_INVALID_STATE);

    *pTag = ((NvU64_HI32(rawTimestamp)) >> pKernelGraphicsStaticInfo->pFecsTraceDefines->timestampHiTagShift) & pKernelGraphicsStaticInfo->pFecsTraceDefines->timestampHiTagMask;
    *pTimestampVal = rawTimestamp & pKernelGraphicsStaticInfo->pFecsTraceDefines->timestampVMask;

    // timestamp encoded as right shifted N bits, since they hold zeros. RM needs to reverse that here.
    *pTimestampVal <<= pKernelGraphicsStaticInfo->pFecsTraceDefines->numLowerBitsZeroShift;
    return NV_OK;
}

//
// The function formats the information from the FECS Buffer into a format
// suitable for EventBuffer, and then checks to see whether the subscriber
// needs to be notified. If so, the subscriber is notified.
//
// pGpu is used to retrieve data on the pid, and
// seqno provides the sequence number for the user to keep track of
//  whether any entry has been dropped.
// pRecord is the current FECS entry.
//
static void
formatAndNotifyFecsRecord
(
    OBJGPU             *pGpu,
    KernelGraphics     *pKernelGraphics,
    FECS_EVENT_RECORD  *pRecord
)
{
    FECS_EVENT_NOTIFICATION_DATA   notifRecord;
    KernelFifo                    *pKernelFifo       = GPU_GET_KERNEL_FIFO(pGpu);
    KernelChannel                 *pKernelChannel    = NULL;
    KernelChannel                 *pKernelChannelNew = NULL;
    KernelGraphicsManager         *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
    KGRMGR_FECS_GLOBAL_TRACE_INFO *pFecsGlobalTraceInfo = kgrmgrGetFecsGlobalTraceInfo(pGpu, pKernelGraphicsManager);
    MIG_INSTANCE_REF              *pChannelRef;
    MIG_INSTANCE_REF              *pNewChannelRef;
    INST_BLOCK_DESC                inst;
    NvU32                          timestampId;
    NvU64                          noisyTimestampStart = 0;
    NvU64                          noisyTimestampRange = 0;
    NvU32                          instSize;
    NvU32                          instShift;
    NV_STATUS                      status;

    if (pRecord == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid FECS record!\n");
        DBG_BREAKPOINT();
        return;
    }

    NV_ASSERT_OR_RETURN_VOID(pFecsGlobalTraceInfo != NULL);

    kfifoGetInstBlkSizeAlign_HAL(pKernelFifo, &instSize, &instShift);

    portMemSet(&notifRecord, 0, sizeof(notifRecord));

    inst.address = ((NvU64)pRecord->context_ptr) << instShift;
    inst.aperture = INST_BLOCK_APERTURE_VIDEO_MEMORY;
    inst.gfid = GPU_GFID_PF;
    if (pRecord->context_ptr &&
        (kfifoConvertInstToKernelChannel_HAL(pGpu, pKernelFifo, &inst, &pKernelChannel) != NV_OK))
    {
        NV_PRINTF(LEVEL_INFO, "Error getting channel!\n");
        pKernelChannel = NULL;
    }

    inst.address = ((NvU64)pRecord->new_context_ptr) << instShift;
    inst.aperture = INST_BLOCK_APERTURE_VIDEO_MEMORY;
    inst.gfid = GPU_GFID_PF;
    if (pRecord->new_context_ptr &&
        (kfifoConvertInstToKernelChannel_HAL(pGpu, pKernelFifo, &inst, &pKernelChannelNew) != NV_OK))
    {
        NV_PRINTF(LEVEL_INFO, "Error getting new channel!\n");
        pKernelChannelNew = NULL;
    }

    pChannelRef = (pKernelChannel != NULL) ? kchannelGetMIGReference(pKernelChannel) : NULL;
    pNewChannelRef = (pKernelChannelNew != NULL) ? kchannelGetMIGReference(pKernelChannelNew) : NULL;

    if (kgraphicsIsFecsRecordUcodeSeqnoSupported(pGpu, pKernelGraphics))
    {
        KGRAPHICS_FECS_TRACE_INFO *pFecsTraceInfo = kgraphicsGetFecsTraceInfo(pGpu, pKernelGraphics);

        // Dropped at least 1 event
        if ((pFecsTraceInfo->fecsLastSeqno + 1) != pRecord->seqno)
        {
            notifRecord.dropCount = pRecord->seqno - pFecsTraceInfo->fecsLastSeqno - 1;
        }

        pFecsTraceInfo->fecsLastSeqno = pRecord->seqno;
    }

    for (timestampId = 0; timestampId < NV_FECS_TRACE_MAX_TIMESTAMPS; timestampId++)
    {
        NV_ASSERT_OK_OR_ELSE(status,
            fecsExtractTagAndTimestamp(pGpu, pKernelGraphics,
                                       pRecord->ts[timestampId],
                                       &notifRecord.timestamp,
                                       &notifRecord.tag),
            return);

        //
        // determine a few more fields of the current record by subevent type,
        // before we notify the subscriber
        //
        switch (notifRecord.tag)
        {
            case NV_EVENT_BUFFER_FECS_CTXSWTAG_RESTORE_START:
            case NV_EVENT_BUFFER_FECS_CTXSWTAG_CONTEXT_START:
                if (pKernelChannelNew != NULL)
                {
                    notifRecord.pid = pKernelChannelNew->ProcessID;
                    notifRecord.subpid = pKernelChannelNew->SubProcessID;
                    notifRecord.userInfo = (NvU64)(NvUPtr)pKernelChannelNew->pUserInfo;
                    notifRecord.context_id = kchannelGetCid(pKernelChannelNew);

                    if (kmigmgrIsMIGReferenceValid(pNewChannelRef))
                    {
                        notifRecord.swizzId = pNewChannelRef->pKernelMIGGpuInstance->swizzId;
                        if (pNewChannelRef->pMIGComputeInstance)
                            notifRecord.computeInstanceId = pNewChannelRef->pMIGComputeInstance->id;
                        else
                            notifRecord.computeInstanceId = NV_EVENT_BUFFER_KERNEL_MIG_CI;
                    }

                    if (notifRecord.tag == NV_EVENT_BUFFER_FECS_CTXSWTAG_RESTORE_START)
                    {
                        noisyTimestampStart = notifRecord.timestamp;
                    }
                    else
                    {
                        noisyTimestampRange = notifRecord.timestamp - noisyTimestampStart;
                    }
                }
                break;

            case NV_EVENT_BUFFER_FECS_CTXSWTAG_FE_ACK_WFI:
            case NV_EVENT_BUFFER_FECS_CTXSWTAG_FE_ACK_GFXP:
            case NV_EVENT_BUFFER_FECS_CTXSWTAG_FE_ACK_CTAP:
            case NV_EVENT_BUFFER_FECS_CTXSWTAG_FE_ACK_CILP:
                if (pKernelChannel != NULL)
                {
                    notifRecord.pid = pKernelChannel->ProcessID;
                    notifRecord.subpid = pKernelChannel->SubProcessID;
                    notifRecord.userInfo = (NvU64)(NvUPtr)pKernelChannel->pUserInfo;
                    notifRecord.context_id = kchannelGetCid(pKernelChannel);

                    if (kmigmgrIsMIGReferenceValid(pChannelRef))
                    {
                        notifRecord.swizzId = pChannelRef->pKernelMIGGpuInstance->swizzId;
                        if (pChannelRef->pMIGComputeInstance)
                            notifRecord.computeInstanceId = pChannelRef->pMIGComputeInstance->id;
                        else
                            notifRecord.computeInstanceId = NV_EVENT_BUFFER_KERNEL_MIG_CI;
                    }
                }
                break;

            case NV_EVENT_BUFFER_FECS_CTXSWTAG_CTXSW_REQ_BY_HOST:
            case NV_EVENT_BUFFER_FECS_CTXSWTAG_SAVE_END:
                if (pKernelChannel != NULL)
                {
                    notifRecord.pid = pKernelChannel->ProcessID;
                    notifRecord.subpid = pKernelChannel->SubProcessID;
                    notifRecord.userInfo = (NvU64)(NvUPtr)pKernelChannel->pUserInfo;
                    notifRecord.context_id = kchannelGetCid(pKernelChannel);

                    if (kmigmgrIsMIGReferenceValid(pChannelRef))
                    {
                        notifRecord.swizzId = pChannelRef->pKernelMIGGpuInstance->swizzId;
                        if (pChannelRef->pMIGComputeInstance != NULL)
                            notifRecord.computeInstanceId = pChannelRef->pMIGComputeInstance->id;
                        else
                            notifRecord.computeInstanceId = NV_EVENT_BUFFER_KERNEL_MIG_CI;
                    }

                    if (notifRecord.tag == NV_EVENT_BUFFER_FECS_CTXSWTAG_CTXSW_REQ_BY_HOST)
                    {
                        noisyTimestampStart = notifRecord.timestamp;
                    }
                    else
                    {
                        noisyTimestampRange = notifRecord.timestamp - noisyTimestampStart;
                    }
                }
                break;

            default:
                continue;
        }

        if ((pKernelChannel != NULL) || (pKernelChannelNew != NULL))
        {
            FecsEventBufferBindMultiMapSubmap *pSubmap;
            KGRAPHICS_FECS_TRACE_INFO *pFecsTraceInfo = kgraphicsGetFecsTraceInfo(pGpu, pKernelGraphics);

            NV_ASSERT_OR_RETURN_VOID(pFecsTraceInfo != NULL);

            notifRecord.noisyTimestamp = 0;
            if ((noisyTimestampRange > 0) && (pFecsTraceInfo->pFecsLogPrng != NULL))
                notifRecord.noisyTimestamp = noisyTimestampStart + portCryptoPseudoRandomGeneratorGetU32(pFecsTraceInfo->pFecsLogPrng) % noisyTimestampRange;

            if (notifRecord.userInfo != 0)
            {
                // Notify event buffers listening for the current UID
                pSubmap = multimapFindSubmap(&pFecsGlobalTraceInfo->fecsEventBufferBindingsUid, notifRecord.userInfo);
                notifyEventBuffers(pGpu, pSubmap, &notifRecord);
            }

            // Notify event buffers listening for all UIDs
            pSubmap = multimapFindSubmap(&pFecsGlobalTraceInfo->fecsEventBufferBindingsUid, 0);
            notifyEventBuffers(pGpu, pSubmap, &notifRecord);

            // Clear so we don't report drops for every event in this record
            notifRecord.dropCount = 0;
        }
    }
}

static NV_STATUS
_fecsEventBufferAdd
(
    OBJGPU *pGpu,
    NV_EVENT_BUFFER_BIND_POINT_FECS *pBind,
    NvU8 tag,
    NvU32 pid,
    NvU8 swizzId,
    NvU8 computeInstanceId,
    NvU32 context_id,
    NvU64 timestamp
)
{
    NV_STATUS status;
    NvBool bNotify;
    NvP64 notificationHandle;
    EVENT_BUFFER_PRODUCER_DATA notifyEvent;
    NvU32 notifyIndex;

    switch (pBind->version)
    {
        case 2:
            notifyIndex = NV_EVENT_BUFFER_RECORD_TYPE_FECS_CTX_SWITCH_V2;
            break;
        case 1:
            notifyIndex = NV_EVENT_BUFFER_RECORD_TYPE_FECS_CTX_SWITCH;
            break;
        default:
            return NV_ERR_INVALID_ARGUMENT;
    }

    portMemSet(&notifyEvent, 0, sizeof(notifyEvent));
    notifyEvent.pVardata = NV_PTR_TO_NvP64(NULL);
    notifyEvent.vardataSize = 0;

    NV_EVENT_BUFFER_FECS_RECORD_V2 fecsRecord;
    portMemSet(&fecsRecord, 0, sizeof(fecsRecord));
    fecsRecord.tag = tag;
    fecsRecord.pid = pid;
    if (pBind->version >= 2)
    {
        fecsRecord.migGpuInstanceId = swizzId;
        fecsRecord.migComputeInstanceId = computeInstanceId;
    }
    fecsRecord.context_id = context_id;
    fecsRecord.timestamp = timestamp;
    fecsRecord.seqno = pBind->pEventBuffer->seqNo++;

    notifyEvent.pPayload = NV_PTR_TO_NvP64(&fecsRecord);
    notifyEvent.payloadSize = sizeof(fecsRecord);

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

void
notifyEventBuffers
(
    OBJGPU *pGpu,
    FecsEventBufferBindMultiMapSubmap *pSubmap,
    FECS_EVENT_NOTIFICATION_DATA const *pRecord
)
{
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
    KGRMGR_FECS_GLOBAL_TRACE_INFO *pFecsGlobalTraceInfo = kgrmgrGetFecsGlobalTraceInfo(pGpu, pKernelGraphicsManager);
    NvBool bMIGInUse = IS_MIG_IN_USE(pGpu);

    NV_ASSERT_OR_RETURN_VOID(pFecsGlobalTraceInfo != NULL);

    if (pSubmap != NULL)
    {
        FecsEventBufferBindMultiMapIter iter = multimapSubmapIterItems(&pFecsGlobalTraceInfo->fecsEventBufferBindingsUid, pSubmap);

        while (multimapItemIterNext(&iter))
        {
            NV_EVENT_BUFFER_BIND_POINT_FECS *pBind = iter.pValue;
            NvBool bSanitizeKernel = (!pBind->bKernel) && (pRecord->userInfo == 0);
            NvBool bSanitizeUser = (!pBind->bAdmin) && (pBind->pUserInfo != pRecord->userInfo);
            NvBool bSanitize = bSanitizeKernel || bSanitizeUser;
            NvU32 pid;
            NvU32 context_id;
            NvU64 timestamp;
            NvU32 tag = pRecord->tag;
            NvU8 swizzId = pRecord->swizzId;
            NvU8 computeInstanceId = pRecord->computeInstanceId;

            pBind->pEventBuffer->seqNo += pRecord->dropCount;

            if (bSanitize || !(NVBIT(pRecord->tag) & pBind->eventMask))
            {
                //
                // Re-map CONTEXT_START as SIMPLE_START and SAVE_END as SIMPLE_END if
                // the binding has simple level-of-detail or is being sanitized
                //
                if ((bSanitize || (pBind->eventMask & NV_EVENT_BUFFER_FECS_BITMASK_CTXSWTAG_SIMPLE_START)) &&
                    (tag == NV_EVENT_BUFFER_FECS_CTXSWTAG_CONTEXT_START))
                {
                    tag = NV_EVENT_BUFFER_FECS_CTXSWTAG_SIMPLE_START;
                }
                else if ((bSanitize || (pBind->eventMask & NV_EVENT_BUFFER_FECS_BITMASK_CTXSWTAG_SIMPLE_END)) &&
                         (tag == NV_EVENT_BUFFER_FECS_CTXSWTAG_SAVE_END))
                {
                    tag = NV_EVENT_BUFFER_FECS_CTXSWTAG_SIMPLE_END;
                }
                else if ((tag != NV_EVENT_BUFFER_FECS_CTXSWTAG_SIMPLE_START) &&
                         (tag != NV_EVENT_BUFFER_FECS_CTXSWTAG_SIMPLE_END))
                {
                    continue;
                }
            }

            //
            // While MIG is enabled, if the bindpoint is registered for a specific MIG GPU instance
            // then filter out records from other GPU instances
            //
            if (bMIGInUse &&
                ((pBind->swizzId != NV2080_CTRL_GPU_PARTITION_ID_INVALID) &&
                 (pRecord->swizzId != pBind->swizzId)))
                continue;

            // While MIG is enabled, pause tracing of V1 bindpoints
            if (bMIGInUse && (pBind->version < 2))
                continue;

            if (bSanitizeKernel)
            {
                timestamp = pRecord->noisyTimestamp;
                pid = NV_EVENT_BUFFER_KERNEL_PID;
                context_id = NV_EVENT_BUFFER_KERNEL_CONTEXT;
                swizzId = NV_EVENT_BUFFER_KERNEL_MIG_GI;
                computeInstanceId = NV_EVENT_BUFFER_KERNEL_MIG_CI;
            }
            else if (bSanitizeUser)
            {
                timestamp = pRecord->noisyTimestamp;
                pid = NV_EVENT_BUFFER_HIDDEN_PID;
                context_id = NV_EVENT_BUFFER_HIDDEN_CONTEXT;
                swizzId = NV_EVENT_BUFFER_HIDDEN_MIG_GI;
                computeInstanceId = NV_EVENT_BUFFER_HIDDEN_MIG_CI;
            }
            else
            {
                timestamp = pRecord->timestamp;
                pid = pRecord->pid;
                context_id = pRecord->context_id;
            }

            _fecsEventBufferAdd(pGpu, pBind, tag,
                                pid,
                                bMIGInUse ? swizzId : NV_EVENT_BUFFER_INVALID_MIG_GI,
                                bMIGInUse ? computeInstanceId : NV_EVENT_BUFFER_INVALID_MIG_CI,
                                context_id, timestamp);
        }
    }
}

static NV_STATUS
_getFecsMemDesc
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics,
    MEMORY_DESCRIPTOR **ppFecsMemDesc
)
{
    MEMORY_DESCRIPTOR *pMemDesc;
    GR_GLOBALCTX_BUFFERS *pGlobalCtxBuffers = kgraphicsGetGlobalCtxBuffers(pGpu, pKernelGraphics, GPU_GFID_PF);

    *ppFecsMemDesc = NULL;
    NV_CHECK_OR_RETURN(LEVEL_SILENT, pGlobalCtxBuffers != NULL, NV_ERR_INVALID_STATE);
    pMemDesc = pGlobalCtxBuffers->memDesc[GR_GLOBALCTX_BUFFER_FECS_EVENT];
    if (pMemDesc != NULL)
        pMemDesc = memdescGetMemDescFromGpu(pMemDesc, pGpu);

    *ppFecsMemDesc = pMemDesc;

    return NV_OK;
}

static NV_STATUS
_getFecsEventListParameters
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics,
    MEMORY_DESCRIPTOR **ppFecsMemDesc,
    NvU32 *pFecsRecordSize
)
{
    const KGRAPHICS_STATIC_INFO *pStaticInfo;
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        _getFecsMemDesc(pGpu, pKernelGraphics, ppFecsMemDesc));

    pStaticInfo = kgraphicsGetStaticInfo(pGpu, pKernelGraphics);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pStaticInfo != NULL, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(pStaticInfo->pFecsTraceDefines != NULL, NV_ERR_INVALID_STATE);
    *pFecsRecordSize = pStaticInfo->pFecsTraceDefines->fecsRecordSize;

    return NV_OK;
}

/*!
 * @brief      Set cached routing info for this KernelGraphics engine to make RPC calls
 *             into the specific MIG GPU instance to which this engine is assigned
 *
 * @param[in]  pGpu
 * @param[in]  pKernelGraphics
 * @param[in]  hClient           Client handle to make calls into MIG GPU instance
 * @param[in]  hSubdevice        Subdevice handle to make calls into MIG GPU instance
 * @param[in]  localGrEngineIdx  Local GR index for this GR engine in MIG GPU instance
 */
void fecsSetRoutingInfo
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics,
    NvHandle hClient,
    NvHandle hSubdevice,
    NvU32 localGrEngineIdx
)
{
    KGRAPHICS_FECS_TRACE_INFO *pFecsTraceInfo = kgraphicsGetFecsTraceInfo(pGpu, pKernelGraphics);

    NV_ASSERT_OR_RETURN_VOID(pFecsTraceInfo != NULL);

    pFecsTraceInfo->hClient = hClient;
    pFecsTraceInfo->hSubdevice = hSubdevice;
    pFecsTraceInfo->localGrEngineIdx = localGrEngineIdx;
}

// Clear cached routing info used to make GR calls into specific MIG GPU instance
void fecsClearRoutingInfo
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics
)
{
    KGRAPHICS_FECS_TRACE_INFO *pFecsTraceInfo = kgraphicsGetFecsTraceInfo(pGpu, pKernelGraphics);

    NV_ASSERT_OR_RETURN_VOID(pFecsTraceInfo != NULL);

    pFecsTraceInfo->hClient = NV01_NULL_OBJECT;
    pFecsTraceInfo->hSubdevice = NV01_NULL_OBJECT;
    pFecsTraceInfo->localGrEngineIdx = 0;
}

NV_STATUS
fecsCtxswLoggingInit
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics,
    KGRAPHICS_FECS_TRACE_INFO **ppFecsTraceInfo
)
{
    NvU64 seed;
    KGRAPHICS_FECS_TRACE_INFO *pFecsTraceInfo;

    NV_ASSERT_OR_RETURN(ppFecsTraceInfo != NULL, NV_ERR_NOT_SUPPORTED);
    pFecsTraceInfo = portMemAllocNonPaged(sizeof(*pFecsTraceInfo));
    if (pFecsTraceInfo == NULL)
        return NV_ERR_NO_MEMORY;
    portMemSet(pFecsTraceInfo, 0, sizeof(*pFecsTraceInfo));

    osGetCurrentTick(&seed);
    pFecsTraceInfo->pFecsLogPrng = portCryptoPseudoRandomGeneratorCreate(seed);

    *ppFecsTraceInfo = pFecsTraceInfo;

    kgraphicsInitFecsRegistryOverrides_HAL(pGpu, pKernelGraphics);

    return NV_OK;
}

void
fecsCtxswLoggingTeardown
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics
)
{
    KGRAPHICS_FECS_TRACE_INFO *pFecsTraceInfo = kgraphicsGetFecsTraceInfo(pGpu, pKernelGraphics);

    NV_ASSERT_OR_RETURN_VOID(pFecsTraceInfo != NULL);

    portCryptoPseudoRandomGeneratorDestroy(pFecsTraceInfo->pFecsLogPrng);
    pFecsTraceInfo->pFecsLogPrng = NULL;
    portMemFree(pFecsTraceInfo);
}

NV_STATUS
fecsGlobalLoggingInit
(
    OBJGPU *pGpu,
    KernelGraphicsManager *pKernelGraphicsManager,
    KGRMGR_FECS_GLOBAL_TRACE_INFO **ppFecsGlobalTraceInfo
)
{
    KGRMGR_FECS_GLOBAL_TRACE_INFO *pFecsGlobalTraceInfo;

    pFecsGlobalTraceInfo = portMemAllocNonPaged(sizeof(*pFecsGlobalTraceInfo));
    NV_ASSERT_OR_RETURN(pFecsGlobalTraceInfo != NULL, NV_ERR_NO_MEMORY);
    portMemSet(pFecsGlobalTraceInfo, 0, sizeof(*pFecsGlobalTraceInfo));

    multimapInit(&pFecsGlobalTraceInfo->fecsEventBufferBindingsUid, portMemAllocatorGetGlobalNonPaged());

    *ppFecsGlobalTraceInfo = pFecsGlobalTraceInfo;

    pFecsGlobalTraceInfo->fecsTimerInterval = NV_FECS_TRACE_CALLBACK_TIME_NS;

    return NV_OK;
}

void
fecsGlobalLoggingTeardown
(
    OBJGPU *pGpu,
    KernelGraphicsManager *pKernelGraphicsManager
)
{
    KGRMGR_FECS_GLOBAL_TRACE_INFO *pFecsGlobalTraceInfo = kgrmgrGetFecsGlobalTraceInfo(pGpu, pKernelGraphicsManager);

    NV_ASSERT_OR_RETURN_VOID(pFecsGlobalTraceInfo != NULL);

    multimapDestroy(&pFecsGlobalTraceInfo->fecsEventBufferBindingsUid);

    portMemFree(pFecsGlobalTraceInfo);
}

/*! set num records to process per intr */
void fecsSetRecordsPerIntr
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics,
    NvU32 recordsPerIntr
)
{
    KGRAPHICS_FECS_TRACE_INFO *pFecsTraceInfo = kgraphicsGetFecsTraceInfo(pGpu, pKernelGraphics);

    NV_ASSERT_OR_RETURN_VOID(pFecsTraceInfo != NULL);
    pFecsTraceInfo->fecsCtxswLogRecordsPerIntr = recordsPerIntr;
}

NvBool
fecsBufferChanged
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics
)
{
    KGRAPHICS_FECS_TRACE_INFO *pFecsTraceInfo = kgraphicsGetFecsTraceInfo(pGpu, pKernelGraphics);
    NvU8 *pFecsBufferMapping;
    MEMORY_DESCRIPTOR *pFecsMemDesc = NULL;
    NvU32 fecsRecordSize;
    FECS_EVENT_RECORD *pPeekRecord;
    NV_STATUS status;

    NV_ASSERT_OR_RETURN(pFecsTraceInfo != NULL, NV_FALSE);
    pFecsBufferMapping = pFecsTraceInfo->pFecsBufferMapping;

    status = _getFecsEventListParameters(pGpu, pKernelGraphics, &pFecsMemDesc, &fecsRecordSize);
    if ((status != NV_OK) || (pFecsMemDesc == NULL) || (pFecsBufferMapping == NULL))
        return NV_FALSE;

    pPeekRecord = (FECS_EVENT_RECORD*)(pFecsBufferMapping +
                  (pFecsTraceInfo->fecsTraceRdOffset * fecsRecordSize));

    if (pPeekRecord->magic_lo != NV_FECS_TRACE_MAGIC_INVALIDATED)
    {
        return NV_TRUE;
    }

    return NV_FALSE;
}

static void
_fecsClearCallbackScheduled
(
    KGRMGR_FECS_GLOBAL_TRACE_INFO *pFecsGlobalTraceInfo
)
{
    NV_ASSERT_OR_RETURN_VOID(pFecsGlobalTraceInfo != NULL);
    portAtomicSetU32(&pFecsGlobalTraceInfo->fecsCallbackScheduled, 0);
}

/*!
 * @brief Atomically set fecs callback scheduled, return NV_TRUE if wasn't scheduled
 */
static NvBool
_fecsSignalCallbackScheduled
(
    KGRMGR_FECS_GLOBAL_TRACE_INFO *pFecsGlobalTraceInfo
)
{
    NV_ASSERT_OR_RETURN(pFecsGlobalTraceInfo != NULL, 0);
    return portAtomicCompareAndSwapU32(&pFecsGlobalTraceInfo->fecsCallbackScheduled, 1, 0);
}

static void
_fecsOsWorkItem
(
    NvU32 gpuInstance,
    void *data
)
{
    OBJGPU *pGpu = gpumgrGetGpu(gpuInstance);
    KernelGraphicsManager *pKernelGraphicsManager;
    KGRMGR_FECS_GLOBAL_TRACE_INFO *pFecsGlobalTraceInfo;

    NV_CHECK_OR_RETURN_VOID(LEVEL_ERROR, pGpu != NULL);
    pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
    pFecsGlobalTraceInfo = kgrmgrGetFecsGlobalTraceInfo(pGpu, pKernelGraphicsManager);

    NV_ASSERT_OR_RETURN_VOID(pFecsGlobalTraceInfo != NULL);

    nvEventBufferFecsCallback(pGpu, NULL);
    _fecsClearCallbackScheduled(pFecsGlobalTraceInfo);
}

static NV_STATUS
_fecsTimerCallback
(
    OBJGPU *pGpu,
    OBJTMR *pTmr,
    TMR_EVENT *pTmrEvent
)
{
    NV_STATUS status = NV_OK;
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
    KGRMGR_FECS_GLOBAL_TRACE_INFO *pFecsGlobalTraceInfo = kgrmgrGetFecsGlobalTraceInfo(pGpu, pKernelGraphicsManager);
    NvU32 i;
    NvU32 numIter = GPU_MAX_GRS;

    if (!IS_MIG_IN_USE(pGpu))
        numIter = 1;

    NV_ASSERT_OR_RETURN(pFecsGlobalTraceInfo != NULL, NV_ERR_INVALID_STATE);

    // If any Kgraphics have events, schedule work item
    for (i = 0; i < numIter; i++)
    {
        KernelGraphics *pKernelGraphics = GPU_GET_KERNEL_GRAPHICS(pGpu, i);

        if (pKernelGraphics == NULL)
            continue;

        if (fecsBufferChanged(pGpu, pKernelGraphics) && _fecsSignalCallbackScheduled(pFecsGlobalTraceInfo))
        {
            NV_CHECK_OK(status, LEVEL_ERROR, osQueueWorkItemWithFlags(pGpu, _fecsOsWorkItem, NULL, OS_QUEUE_WORKITEM_FLAGS_LOCK_GPU_GROUP_DEVICE));

            if (status != NV_OK)
                _fecsClearCallbackScheduled(pFecsGlobalTraceInfo);

            break;
        }
    }

    // TMR_FLAG_RECUR does not work, so reschedule it here.
    NV_CHECK_OK_OR_CAPTURE_FIRST_ERROR(status,
                                       LEVEL_ERROR,
                                       tmrEventScheduleRel(pTmr, pTmrEvent, pFecsGlobalTraceInfo->fecsTimerInterval));

    return status;
}

static NV_STATUS
_fecsTimerCreate
(
    OBJGPU *pGpu
)
{
    OBJTMR *pTmr = GPU_GET_TIMER(pGpu);
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
    KGRMGR_FECS_GLOBAL_TRACE_INFO *pFecsGlobalTraceInfo = kgrmgrGetFecsGlobalTraceInfo(pGpu, pKernelGraphicsManager);
    NvU32 timerFlags = TMR_FLAG_RECUR;
    // Unix needs to use the OS timer to avoid corrupting records, but Windows doesn't have an OS timer implementation
    timerFlags |= TMR_FLAG_USE_OS_TIMER;

    NV_ASSERT_OR_RETURN(pFecsGlobalTraceInfo != NULL, NV_ERR_INVALID_STATE);

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        tmrEventCreate(pTmr, &pFecsGlobalTraceInfo->pFecsTimerEvent, _fecsTimerCallback, NULL, timerFlags));

    // This won't be a true 30Hz timer as the callbacks are scheduled from the time they're called
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        tmrEventScheduleRel(pTmr, pFecsGlobalTraceInfo->pFecsTimerEvent, pFecsGlobalTraceInfo->fecsTimerInterval));

    return NV_OK;
}

static void
_fecsTimerDestroy
(
    OBJGPU *pGpu
)
{
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
    KGRMGR_FECS_GLOBAL_TRACE_INFO *pFecsGlobalTraceInfo = kgrmgrGetFecsGlobalTraceInfo(pGpu, pKernelGraphicsManager);

    NV_ASSERT_OR_RETURN_VOID(pFecsGlobalTraceInfo != NULL);

    if (pFecsGlobalTraceInfo->pFecsTimerEvent != NULL)
    {
        OBJTMR *pTmr = GPU_GET_TIMER(pGpu);

        tmrEventDestroy(pTmr, pFecsGlobalTraceInfo->pFecsTimerEvent);
        pFecsGlobalTraceInfo->pFecsTimerEvent = NULL;
    }
}

NV_STATUS
fecsHandleFecsLoggingError
(
    OBJGPU *pGpu,
    NvU32 grIdx,
    FECS_ERROR_EVENT_TYPE errorType
)
{
    KernelGraphics *pKernelGraphics = GPU_GET_KERNEL_GRAPHICS(pGpu, grIdx);
    NV_STATUS status = NV_OK;

    switch (errorType)
    {
        case FECS_ERROR_EVENT_TYPE_BUFFER_RESET_REQUIRED:
        {
            fecsBufferDisableHw(pGpu, pKernelGraphics);
            kgraphicsSetCtxswLoggingEnabled(pGpu, pKernelGraphics, NV_FALSE);
            fecsBufferReset(pGpu, pKernelGraphics);
            break;
        }
        case FECS_ERROR_EVENT_TYPE_BUFFER_FULL:
        {
            nvEventBufferFecsCallback(pGpu, pKernelGraphics);
            break;
        }
        default:
        {
            status = NV_ERR_INVALID_ARGUMENT;
            break;
        }
    }

    return status;
}

/**
 * @brief The callback function that transfers FECS Buffer entries to an EventBuffer
 */
void
nvEventBufferFecsCallback
(
    OBJGPU  *pGpu,
    void    *pArgs
)
{
    KernelGraphics          *pKernelGraphics = (KernelGraphics*)pArgs;
    KernelGraphicsManager   *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
    KGRMGR_FECS_GLOBAL_TRACE_INFO *pFecsGlobalTraceInfo = kgrmgrGetFecsGlobalTraceInfo(pGpu, pKernelGraphicsManager);
    NvU32                    fecsReadOffset;
    NvU32                    fecsReadOffsetPrev;
    NvU64                    fecsBufferSize;
    NvU32                    fecsRecordSize;
    NvU32                    i, j;
    NvU8                    *pFecsBufferMapping;
    MEMORY_DESCRIPTOR       *pFecsMemDesc = NULL;
    FECS_EVENT_RECORD       *pPeekRecord;
    NvU16                    maxFecsRecordsPerIntr;
    NV_STATUS                status;
    RM_API                  *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NvU8                     numIterations = (pArgs == NULL)
                                             ? KGRMGR_MAX_GR
                                             : 1;

    if (!IS_MIG_IN_USE(pGpu))
        numIterations = 1;

    NV_ASSERT_OR_RETURN_VOID(rmDeviceGpuLockIsOwner(pGpu->gpuInstance));

    NV_ASSERT_OR_RETURN_VOID(pFecsGlobalTraceInfo != NULL);
    NV_ASSERT_OR_RETURN_VOID(pFecsGlobalTraceInfo->fecsCtxswLogConsumerCount >= 0);
    if (pFecsGlobalTraceInfo->fecsCtxswLogConsumerCount <= 0)
        return;

    for (j = 0; j < numIterations; j++)
    {
        KGRAPHICS_FECS_TRACE_INFO *pFecsTraceInfo;

        if (pArgs == NULL)
        {
            pKernelGraphics = GPU_GET_KERNEL_GRAPHICS(pGpu, j);
            if (pKernelGraphics == NULL)
                continue;
        }

        pFecsTraceInfo = kgraphicsGetFecsTraceInfo(pGpu, pKernelGraphics);
        NV_ASSERT_OR_RETURN_VOID(pFecsTraceInfo != NULL);

        pFecsBufferMapping = pFecsTraceInfo->pFecsBufferMapping;
        maxFecsRecordsPerIntr = pFecsTraceInfo->fecsCtxswLogRecordsPerIntr;

        if (pFecsBufferMapping == NULL)
            continue;

        status = _getFecsEventListParameters(pGpu, pKernelGraphics, &pFecsMemDesc, &fecsRecordSize);
        if ((status != NV_OK) || (pFecsMemDesc == NULL))
            continue;

        fecsBufferSize = memdescGetSize(pFecsMemDesc) / fecsRecordSize;
        NV_ASSERT_OR_RETURN_VOID(fecsBufferSize > 0);
        fecsReadOffset = pFecsTraceInfo->fecsTraceRdOffset;

        if (!osIsRaisedIRQL())
            maxFecsRecordsPerIntr = fecsBufferSize;

        // Bail out if the buffer has not changed
        pPeekRecord = (FECS_EVENT_RECORD*)(pFecsBufferMapping +
                      (fecsReadOffset * fecsRecordSize));

        if (pPeekRecord->magic_lo == NV_FECS_TRACE_MAGIC_INVALIDATED)
            continue;

        // Get the read offset from hw if the buffer wrapped around
        fecsReadOffsetPrev = (fecsReadOffset - 1) % fecsBufferSize;
        pPeekRecord = (FECS_EVENT_RECORD*)(pFecsBufferMapping +
                      (fecsReadOffsetPrev * fecsRecordSize));

        if (pPeekRecord->magic_lo != NV_FECS_TRACE_MAGIC_INVALIDATED)
        {
            NvHandle hClient;
            NvHandle hSubdevice;
            NV2080_CTRL_INTERNAL_GR_GET_FECS_TRACE_RD_OFFSET_PARAMS params;

            NV_PRINTF(LEVEL_ERROR, "FECS buffer overflow detected\n");

            NV_ASSERT_OK_OR_ELSE(
                status,
                _fecsLoadInternalRoutingInfo(pGpu,
                                             pKernelGraphics,
                                             &hClient,
                                             &hSubdevice,
                                             &params.grRouteInfo),
                return);

            NV_ASSERT_OK_OR_ELSE(
                status,
                pRmApi->Control(pRmApi,
                                hClient,
                                hSubdevice,
                                NV2080_CTRL_CMD_INTERNAL_GR_GET_FECS_TRACE_RD_OFFSET,
                                &params,
                                sizeof(params)),
                 return);
            fecsReadOffset = params.offset;
            pFecsTraceInfo->fecsTraceCounter = 0;
        }

        //
        // Over here we want to go through all EVENTNOTIFICATION nodes and
        // loop through them in lockstep with the FECS_EVENT_RECORD records
        //
        for (i = 0; i < maxFecsRecordsPerIntr; ++i)
        {
            FECS_EVENT_RECORD *pCurrRecord = (FECS_EVENT_RECORD *)(pFecsBufferMapping +
                                             (fecsReadOffset * fecsRecordSize));

            if (pCurrRecord->magic_lo == NV_FECS_TRACE_MAGIC_INVALIDATED)
                break;

            pCurrRecord->magic_lo = NV_FECS_TRACE_MAGIC_INVALIDATED;
            osFlushCpuWriteCombineBuffer();

            // Loop through all bound event buffers and copy filtered data to user buffers
            formatAndNotifyFecsRecord(pGpu, pKernelGraphics, pCurrRecord);

            // Update read reg
            pFecsTraceInfo->fecsTraceCounter++;
            fecsReadOffset++;
            if (fecsReadOffset >= fecsBufferSize)
            {
                fecsReadOffset = 0;
            }
        }

        if (pFecsTraceInfo->fecsTraceCounter > 0)
        {
            NvHandle hClient;
            NvHandle hSubdevice;
            NV2080_CTRL_INTERNAL_GR_SET_FECS_TRACE_RD_OFFSET_PARAMS params;

            params.offset = fecsReadOffset;
            NV_ASSERT_OK_OR_ELSE(
                status,
                _fecsLoadInternalRoutingInfo(pGpu,
                                             pKernelGraphics,
                                             &hClient,
                                             &hSubdevice,
                                             &params.grRouteInfo),
                return);

            NV_ASSERT_OK_OR_ELSE(
                status,
                pRmApi->Control(pRmApi,
                                hClient,
                                hSubdevice,
                                NV2080_CTRL_CMD_INTERNAL_GR_SET_FECS_TRACE_RD_OFFSET,
                                &params,
                                sizeof(params)),
                return);
            pFecsTraceInfo->fecsTraceCounter = 0;
        }
        pFecsTraceInfo->fecsTraceRdOffset = fecsReadOffset;

        // Re-arm interrupt if there may be more records
        if (i == maxFecsRecordsPerIntr)
            fecsSignalIntrPendingIfNotPending(pGpu, pKernelGraphics);
    }
}

NV_STATUS
fecsAddBindpoint
(
    OBJGPU *pGpu,
    RmClient *pClient,
    RsResourceRef *pEventBufferRef,
    Subdevice *pNotifier,
    NvBool bAllUsers,
    NV2080_CTRL_GR_FECS_BIND_EVTBUF_LOD levelOfDetail,
    NvU32 eventFilter,
    NvU8 version,
    NvU32 *pReasonCode
)
{
    NV_STATUS status;
    NvHandle hClient = staticCast(pClient, RsClient)->hClient;
    NvHandle hEventBuffer = pEventBufferRef->hResource;
    NvHandle hNotifier = RES_GET_HANDLE(pNotifier);
    EventBuffer *pEventBuffer;
    NvBool bAdmin = osIsAdministrator();
    NvU32 eventMask = 0;
    NvU64 targetUser;
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
    KGRMGR_FECS_GLOBAL_TRACE_INFO *pFecsGlobalTraceInfo = kgrmgrGetFecsGlobalTraceInfo(pGpu, pKernelGraphicsManager);
    NvS32 gpuConsumerCount;
    NvBool bFecsBindingActive;
    NvBool bIntrDriven = NV_FALSE;
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    NvBool bMIGInUse = IS_MIG_IN_USE(pGpu);
    NvU8 numIterations;
    NvU8 grIdx;
    NvBool bKernel;
    NvBool bSelectLOD;
    NV_EVENT_BUFFER_BIND_POINT_FECS *pBind;

    CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
    NV_ASSERT_OR_RETURN(pCallContext != NULL, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(pFecsGlobalTraceInfo != NULL, NV_ERR_INVALID_STATE);

    gpuConsumerCount = pFecsGlobalTraceInfo->fecsCtxswLogConsumerCount;
    bFecsBindingActive = (pFecsGlobalTraceInfo->fecsCtxswLogConsumerCount > 0);

    bKernel = pCallContext->secInfo.privLevel >= RS_PRIV_LEVEL_KERNEL;

    bSelectLOD = bKernel;

#if defined(DEBUG) || defined(DEVELOP) || defined(NV_VERIF_FEATURES)
    bSelectLOD = NV_TRUE;
#endif

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner() && rmDeviceGpuLockIsOwner(pGpu->gpuInstance));

    // Early bail-out if profiling capability is not enabled on vGPU
    if (IS_VIRTUAL(pGpu))
    {
        VGPU_STATIC_INFO *pVSI = GPU_GET_STATIC_INFO(pGpu);
        if ((pVSI == NULL) || !pVSI->vgpuStaticProperties.bProfilingTracingEnabled)
        {
            if (pReasonCode != NULL)
                *pReasonCode = NV2080_CTRL_GR_FECS_BIND_REASON_CODE_NEED_CAPABILITY;

            return NV_ERR_NOT_SUPPORTED;
        }
    }

    // On a hypervisor or VM: bail-out early if admin is required
    if (IS_VIRTUAL(pGpu) || hypervisorIsVgxHyper())
    {
        if (pGpu->bRmProfilingPrivileged && !bAdmin)
        {
            if (pReasonCode != NULL)
                *pReasonCode = NV2080_CTRL_GR_FECS_BIND_REASON_CODE_NEED_ADMIN;

            return NV_ERR_NOT_SUPPORTED;
        }
    }

    if (bSelectLOD)
    {
        switch (levelOfDetail)
        {
            case NV2080_CTRL_GR_FECS_BIND_EVTBUF_LOD_FULL:
                eventMask = ~0;
                break;
            case NV2080_CTRL_GR_FECS_BIND_EVTBUF_LOD_COMPAT:
                eventMask |= NV_EVENT_BUFFER_FECS_BITMASK_CTXSWTAG_RESTORE_START |
                             NV_EVENT_BUFFER_FECS_BITMASK_CTXSWTAG_CONTEXT_START |
                             NV_EVENT_BUFFER_FECS_BITMASK_CTXSWTAG_CTXSW_REQ_BY_HOST;
                break;
            case NV2080_CTRL_GR_FECS_BIND_EVTBUF_LOD_CUSTOM:
                eventMask = eventFilter;
                break;
            default:
                // Default to SIMPLIFIED level-of-detail
                eventMask |= NV_EVENT_BUFFER_FECS_BITMASK_CTXSWTAG_SIMPLE_START |
                             NV_EVENT_BUFFER_FECS_BITMASK_CTXSWTAG_SIMPLE_END;
        }
    }
    else
    {
        // Default to SIMPLIFIED level-of-detail
        eventMask |= NV_EVENT_BUFFER_FECS_BITMASK_CTXSWTAG_SIMPLE_START |
                     NV_EVENT_BUFFER_FECS_BITMASK_CTXSWTAG_SIMPLE_END;
    }

    if (eventMask & NV_EVENT_BUFFER_FECS_BITMASK_CTXSWTAG_FE_ACK)
    {
        eventMask |= NV_EVENT_BUFFER_FECS_CTXSWTAG_FE_ACK_WFI |
                     NV_EVENT_BUFFER_FECS_CTXSWTAG_FE_ACK_GFXP |
                     NV_EVENT_BUFFER_FECS_CTXSWTAG_FE_ACK_CTAP |
                     NV_EVENT_BUFFER_FECS_CTXSWTAG_FE_ACK_CILP;
    }

    if (bAllUsers)
    {
        targetUser = 0;
    }
    else
    {
        targetUser = (NvU64)(NvUPtr)pClient->pUserInfo;

        // Filtering UIDs is not yet implemented in legacy vGPU
        if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu))
        {
            if (pReasonCode != NULL)
                *pReasonCode = NV2080_CTRL_GR_FECS_BIND_REASON_CODE_NOT_ENABLED;

            return NV_ERR_NOT_SUPPORTED;
        }
    }

    pEventBuffer = dynamicCast(pEventBufferRef->pResource, EventBuffer);
    if (pEventBuffer == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    if (multimapFindSubmap(&pFecsGlobalTraceInfo->fecsEventBufferBindingsUid, targetUser) == NULL)
    {
        if (multimapInsertSubmap(&pFecsGlobalTraceInfo->fecsEventBufferBindingsUid, targetUser) == NULL)
        {
            NV_PRINTF(LEVEL_ERROR, "failed to add UID binding!\n");
            return NV_ERR_INSUFFICIENT_RESOURCES;
        }
    }

    // If the binding exists already, we're done
    if (multimapFindItem(&pFecsGlobalTraceInfo->fecsEventBufferBindingsUid, targetUser, (NvU64)(NvUPtr)pEventBuffer) != NULL)
        return NV_OK;

    pBind = multimapInsertItemNew(&pFecsGlobalTraceInfo->fecsEventBufferBindingsUid, targetUser, (NvU64)(NvUPtr)pEventBuffer);
    if (pBind == NULL)
        return NV_ERR_INVALID_ARGUMENT;
    ++pFecsGlobalTraceInfo->fecsCtxswLogConsumerCount;

    pBind->hClient = hClient;
    pBind->hNotifier = hNotifier;
    pBind->hEventBuffer = hEventBuffer;
    pBind->pEventBuffer = pEventBuffer;
    pBind->pUserInfo = (NvU64)(NvUPtr)pClient->pUserInfo;
    pBind->bAdmin = bAdmin;
    pBind->eventMask = eventMask;
    pBind->bKernel = bKernel;
    pBind->version = version;

    status = registerEventNotification(&pEventBuffer->pListeners,
                staticCast(pClient, RsClient),
                hNotifier,
                hEventBuffer,
                (version == 2 ?
                    NV_EVENT_BUFFER_RECORD_TYPE_FECS_CTX_SWITCH_V2 :
                    NV_EVENT_BUFFER_RECORD_TYPE_FECS_CTX_SWITCH) | NV01_EVENT_WITHOUT_EVENT_DATA,
                NV_EVENT_BUFFER_BIND,
                pEventBuffer->producerInfo.notificationHandle,
                NV_FALSE);
    if (status != NV_OK)
        goto done;

    if (bMIGInUse)
    {
        if (kmigmgrIsDeviceUsingDeviceProfiling(pGpu, pKernelMIGManager, GPU_RES_GET_DEVICE(pNotifier)))
        {
            pBind->swizzId = NV2080_CTRL_GPU_PARTITION_ID_INVALID;
        }
        else
        {
            GPUInstanceSubscription *pGPUInstanceSubscription;
            status = gisubscriptionGetGPUInstanceSubscription(
                    staticCast(pClient, RsClient), hNotifier, &pGPUInstanceSubscription);
            if (status != NV_OK)
                goto done;

            if (gisubscriptionGetMIGGPUInstance(pGPUInstanceSubscription) == NULL)
            {
                if (pReasonCode != NULL)
                    *pReasonCode = NV2080_CTRL_GR_FECS_BIND_REASON_CODE_NOT_ENABLED;

                status = NV_ERR_NOT_SUPPORTED;
                goto done;
            }

            pBind->swizzId = gisubscriptionGetMIGGPUInstance(pGPUInstanceSubscription)->swizzId;
        }
    }

    numIterations = bMIGInUse ? GPU_MAX_GRS: 1;
    for (grIdx = 0; grIdx < numIterations; grIdx++)
    {
        KernelGraphics *pKernelGraphics = GPU_GET_KERNEL_GRAPHICS(pGpu, grIdx);
        if (pKernelGraphics == NULL)
            continue;

        if (!kgraphicsIsCtxswLoggingSupported(pGpu, pKernelGraphics))
        {
            if (pReasonCode)
                *pReasonCode = NV2080_CTRL_GR_FECS_BIND_REASON_CODE_NOT_ENABLED_GPU;

            status = NV_ERR_NOT_SUPPORTED;
            goto done;
        }

        if (!bFecsBindingActive)
        {

            fecsBufferReset(pGpu, pKernelGraphics);
        }

        bIntrDriven |= kgraphicsIsIntrDrivenCtxswLoggingEnabled(pGpu, pKernelGraphics);
    }

    if (!bFecsBindingActive && !bIntrDriven)
    {
        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR, _fecsTimerCreate(pGpu), done);
    }

done:
    if (status != NV_OK)
    {
        if (gpuConsumerCount != pFecsGlobalTraceInfo->fecsCtxswLogConsumerCount)
            fecsRemoveBindpoint(pGpu, targetUser, pBind);

        _fecsTimerDestroy(pGpu);
    }

    return status;
}

void
fecsRemoveBindpoint
(
    OBJGPU *pGpu,
    NvU64 uid,
    NV_EVENT_BUFFER_BIND_POINT_FECS *pBind
)
{
    EventBuffer *pEventBuffer = pBind->pEventBuffer;
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
    KGRMGR_FECS_GLOBAL_TRACE_INFO *pFecsGlobalTraceInfo = kgrmgrGetFecsGlobalTraceInfo(pGpu, pKernelGraphicsManager);

    NV_ASSERT_OR_RETURN_VOID(pFecsGlobalTraceInfo != NULL);

    --pFecsGlobalTraceInfo->fecsCtxswLogConsumerCount;

    unregisterEventNotificationWithData(&pEventBuffer->pListeners,
            pBind->hClient,
            pBind->hNotifier,
            pBind->hEventBuffer,
            NV_TRUE,
            pEventBuffer->producerInfo.notificationHandle);

    multimapRemoveItemByKey(&pFecsGlobalTraceInfo->fecsEventBufferBindingsUid,
            uid,
            (NvU64)(NvUPtr)pEventBuffer);

    if (pFecsGlobalTraceInfo->fecsCtxswLogConsumerCount == 0)
    {
        NvBool bMIGInUse = IS_MIG_IN_USE(pGpu);
        NvU8 grIdx;
        NvBool bIntrDriven = NV_FALSE;

        NvU8 numIterations = bMIGInUse ? GPU_MAX_GRS : 1;
        for (grIdx = 0; grIdx < numIterations; grIdx++)
        {
            KernelGraphics *pKernelGraphics = GPU_GET_KERNEL_GRAPHICS(pGpu, grIdx);
            if (pKernelGraphics == NULL)
                continue;

            //
            // Disable HW without unmapping buffer so that new event buffers still work properly
            // HW enable will happen on bindpoint creation.
            // Mapping only occurs on Graphics load/alloc, so unmapping should only occur when Graphics is destroyed.
            //
            fecsBufferDisableHw(pGpu, pKernelGraphics);
            bIntrDriven |= kgraphicsIsIntrDrivenCtxswLoggingEnabled(pGpu, pKernelGraphics);

        }

        if (!bIntrDriven)
        {
            _fecsTimerDestroy(pGpu);
        }
    }
}

void
fecsRemoveAllBindpointsForGpu
(
    OBJGPU *pGpu
)
{
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
    KGRMGR_FECS_GLOBAL_TRACE_INFO *pFecsGlobalTraceInfo = kgrmgrGetFecsGlobalTraceInfo(pGpu, pKernelGraphicsManager);
    FecsEventBufferBindMultiMapSupermapIter iter;

    NV_CHECK_OR_RETURN_VOID(LEVEL_SILENT, pFecsGlobalTraceInfo != NULL);

    iter = multimapSubmapIterAll(&pFecsGlobalTraceInfo->fecsEventBufferBindingsUid);
    while (multimapSubmapIterNext(&iter))
    {
        FecsEventBufferBindMultiMapSubmap *pSubmap = iter.pValue;
        FecsEventBufferBindMultiMapIter subIter = multimapSubmapIterItems(&pFecsGlobalTraceInfo->fecsEventBufferBindingsUid, pSubmap);
        NvU64 uid = mapKey_IMPL(iter.iter.pMap, pSubmap);

        while (multimapItemIterNext(&subIter))
        {
            NV_EVENT_BUFFER_BIND_POINT_FECS *pBind = subIter.pValue;
            fecsRemoveBindpoint(pGpu, uid, pBind);
        }
    }
}

void
fecsRemoveAllBindpoints
(
    EventBuffer *pEventBuffer
)
{
    OBJGPU *pGpu = NULL;
    NvU32 gpuMask = 0;
    NvU32 gpuIndex = 0;
    KernelGraphicsManager *pKernelGraphicsManager;
    FecsEventBufferBindMultiMapSupermapIter uidBindIter;

    eventBufferSetEnable(&pEventBuffer->producerInfo, NV_FALSE);

    gpumgrGetGpuAttachInfo(NULL, &gpuMask);
    while ((pGpu = gpumgrGetNextGpu(gpuMask, &gpuIndex)) != NULL)
    {
        pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
        KGRMGR_FECS_GLOBAL_TRACE_INFO *pFecsGlobalTraceInfo = kgrmgrGetFecsGlobalTraceInfo(pGpu, pKernelGraphicsManager);

        NV_CHECK_OR_ELSE(LEVEL_ERROR, pFecsGlobalTraceInfo != NULL, continue;);

        uidBindIter = multimapSubmapIterAll(&pFecsGlobalTraceInfo->fecsEventBufferBindingsUid);
        while (multimapSubmapIterNext(&uidBindIter))
        {
            FecsEventBufferBindMultiMapSubmap *pSubmap = uidBindIter.pValue;
            NV_EVENT_BUFFER_BIND_POINT_FECS *pBind = NULL;
            NvU64 uid = mapKey_IMPL(uidBindIter.iter.pMap, pSubmap);

            while ((pBind = multimapFindItem(&pFecsGlobalTraceInfo->fecsEventBufferBindingsUid,
                            uid,
                            (NvU64)(NvUPtr)pEventBuffer)) != NULL)
            {
                fecsRemoveBindpoint(pGpu, uid, pBind);
            }
        }

    }
}

void
fecsBufferReset
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics
)
{
    MEMORY_DESCRIPTOR *pFecsMemDesc = NULL;
    NV_STATUS status;
    KGRAPHICS_FECS_TRACE_INFO *pFecsTraceInfo = kgraphicsGetFecsTraceInfo(pGpu, pKernelGraphics);
    NV2080_CTRL_INTERNAL_GR_GET_FECS_TRACE_HW_ENABLE_PARAMS getHwEnableParams;
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NvHandle hClient;
    NvHandle hSubdevice;

    NV_ASSERT_OR_RETURN_VOID(pFecsTraceInfo != NULL);

    if (pFecsTraceInfo->pFecsBufferMapping == NULL)
        return;

    NV_ASSERT_OK_OR_ELSE(
        status,
        _fecsLoadInternalRoutingInfo(pGpu,
                                     pKernelGraphics,
                                     &hClient,
                                     &hSubdevice,
                                     &getHwEnableParams.grRouteInfo),
        return);

    NV_ASSERT_OK_OR_ELSE(
        status,
        pRmApi->Control(pRmApi,
                        hClient,
                        hSubdevice,
                        NV2080_CTRL_CMD_INTERNAL_GR_GET_FECS_TRACE_HW_ENABLE,
                        &getHwEnableParams,
                        sizeof(getHwEnableParams)),
        return);

    status = _getFecsMemDesc(pGpu, pKernelGraphics, &pFecsMemDesc);

    if ((status == NV_OK) && (pFecsMemDesc != NULL) && (getHwEnableParams.bEnable != NV_TRUE))
    {
        NV2080_CTRL_INTERNAL_GR_SET_FECS_TRACE_WR_OFFSET_PARAMS traceWrOffsetParams;
        NV2080_CTRL_INTERNAL_GR_SET_FECS_TRACE_RD_OFFSET_PARAMS traceRdOffsetParams;
        NV2080_CTRL_INTERNAL_GR_SET_FECS_TRACE_HW_ENABLE_PARAMS setHwEnableParams;

        portMemSet(pFecsTraceInfo->pFecsBufferMapping,
                   (NvU8)(NV_FECS_TRACE_MAGIC_INVALIDATED & 0xff),
                   memdescGetSize(pFecsMemDesc));

        pFecsTraceInfo->fecsLastSeqno = 0;

        // Routing info is the same for all future calls in this series
        traceWrOffsetParams.grRouteInfo = getHwEnableParams.grRouteInfo;
        traceRdOffsetParams.grRouteInfo = getHwEnableParams.grRouteInfo;
        setHwEnableParams.grRouteInfo   = getHwEnableParams.grRouteInfo;

        traceWrOffsetParams.offset = 0;
        NV_ASSERT_OK_OR_ELSE(
            status,
            pRmApi->Control(pRmApi,
                            hClient,
                            hSubdevice,
                            NV2080_CTRL_CMD_INTERNAL_GR_SET_FECS_TRACE_WR_OFFSET,
                            &traceWrOffsetParams,
                            sizeof(traceWrOffsetParams)),
            return);

        traceRdOffsetParams.offset = 0;
        NV_ASSERT_OK_OR_ELSE(
            status,
            pRmApi->Control(pRmApi,
                            hClient,
                            hSubdevice,
                            NV2080_CTRL_CMD_INTERNAL_GR_SET_FECS_TRACE_RD_OFFSET,
                            &traceRdOffsetParams,
                            sizeof(traceRdOffsetParams)),
             return);
        pFecsTraceInfo->fecsTraceRdOffset = 0;

        setHwEnableParams.bEnable = NV_TRUE;
        NV_ASSERT_OK_OR_ELSE(
            status,
            pRmApi->Control(pRmApi,
                            hClient,
                            hSubdevice,
                            NV2080_CTRL_CMD_INTERNAL_GR_SET_FECS_TRACE_HW_ENABLE,
                            &setHwEnableParams,
                            sizeof(setHwEnableParams)),
            return);
    }
}

void
fecsBufferDisableHw
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics
)
{
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NvHandle hClient;
    NvHandle hSubdevice;
    NV2080_CTRL_INTERNAL_GR_GET_FECS_TRACE_HW_ENABLE_PARAMS getHwEnableParams;
    NV2080_CTRL_INTERNAL_GR_SET_FECS_TRACE_HW_ENABLE_PARAMS setHwEnableParams;
    NV_STATUS status;

    // This function may be called with unused GR Engines
    NV_CHECK_OK_OR_ELSE(
        status,
        LEVEL_INFO,
        _fecsLoadInternalRoutingInfo(pGpu,
                                     pKernelGraphics,
                                     &hClient,
                                     &hSubdevice,
                                     &getHwEnableParams.grRouteInfo),
        return);

    NV_ASSERT_OK_OR_ELSE(
        status,
        pRmApi->Control(pRmApi,
                        hClient,
                        hSubdevice,
                        NV2080_CTRL_CMD_INTERNAL_GR_GET_FECS_TRACE_HW_ENABLE,
                        &getHwEnableParams,
                        sizeof(getHwEnableParams)),
        return);

    if (getHwEnableParams.bEnable)
    {
        // Copy previously loaded routing info
        setHwEnableParams.grRouteInfo = getHwEnableParams.grRouteInfo;
        setHwEnableParams.bEnable = NV_FALSE;

        NV_ASSERT_OK_OR_ELSE(
            status,
            pRmApi->Control(pRmApi,
                            hClient,
                            hSubdevice,
                            NV2080_CTRL_CMD_INTERNAL_GR_SET_FECS_TRACE_HW_ENABLE,
                            &setHwEnableParams,
                            sizeof(setHwEnableParams)),
            return);
    }
}

void
fecsBufferTeardown
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics
)
{
    fecsBufferDisableHw(pGpu, pKernelGraphics);
    fecsBufferUnmap(pGpu, pKernelGraphics);
}

/*! Is the FECS trace buffer mapped? */
NvBool
fecsBufferIsMapped
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics
)
{
    KGRAPHICS_FECS_TRACE_INFO *pFecsTraceInfo = kgraphicsGetFecsTraceInfo(pGpu, pKernelGraphics);

    NV_ASSERT_OR_RETURN(pFecsTraceInfo != NULL, NV_FALSE);
    return pFecsTraceInfo->pFecsBufferMapping != NULL;
}

NV_STATUS
fecsBufferMap
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics
)
{
    MEMORY_DESCRIPTOR *pFecsMemDesc = NULL;
    NvU8 *pFecsBufferMapping = NULL;
    NV_STATUS status;
    KGRAPHICS_FECS_TRACE_INFO *pFecsTraceInfo = kgraphicsGetFecsTraceInfo(pGpu, pKernelGraphics);
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);

    NV_ASSERT_OR_RETURN(pFecsTraceInfo != NULL, NV_ERR_INVALID_STATE);

    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu))
        return NV_OK;

    if (pFecsTraceInfo->pFecsBufferMapping != NULL)
        return NV_OK;

    status = _getFecsMemDesc(pGpu, pKernelGraphics, &pFecsMemDesc);
    if ((status != NV_OK) || (pFecsMemDesc == NULL))
        return NV_ERR_INVALID_STATE;

    pFecsBufferMapping = memmgrMemDescBeginTransfer(pMemoryManager, pFecsMemDesc,
                                                    TRANSFER_FLAGS_PREFER_PROCESSOR |
                                                    TRANSFER_FLAGS_PERSISTENT_CPU_MAPPING);
    if (pFecsBufferMapping == NULL)
        return NV_ERR_INSUFFICIENT_RESOURCES;

    pFecsTraceInfo->pFecsBufferMapping = pFecsBufferMapping;

    return NV_OK;
}

void
fecsBufferUnmap
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics
)
{
    MEMORY_DESCRIPTOR *pFecsMemDesc = NULL;
    NV_STATUS status;
    KGRAPHICS_FECS_TRACE_INFO *pFecsTraceInfo = kgraphicsGetFecsTraceInfo(pGpu, pKernelGraphics);
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);

    NV_ASSERT_OR_RETURN_VOID(pFecsTraceInfo != NULL);

    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu))
        return;

    status = _getFecsMemDesc(pGpu, pKernelGraphics, &pFecsMemDesc);
    if ((status != NV_OK) || (pFecsMemDesc == NULL))
        return;

    if (pFecsTraceInfo->pFecsBufferMapping != NULL)
    {
        memmgrMemDescEndTransfer(pMemoryManager, pFecsMemDesc,
                                 TRANSFER_FLAGS_PREFER_PROCESSOR |
                                 TRANSFER_FLAGS_PERSISTENT_CPU_MAPPING);

        pFecsTraceInfo->pFecsBufferMapping = NULL;
    }
}

/*! Atomically set intr callback pending, return NV_TRUE if wasn't pending prior */
NvBool
fecsSignalIntrPendingIfNotPending
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics
)
{
    KGRAPHICS_FECS_TRACE_INFO *pFecsTraceInfo = kgraphicsGetFecsTraceInfo(pGpu, pKernelGraphics);

    NV_ASSERT_OR_RETURN(pFecsTraceInfo != NULL, NV_FALSE);

    return portAtomicCompareAndSwapU32(&pFecsTraceInfo->fecsCtxswLogIntrPending, 1, 0);
}

/*! Atomically clear intr callback pending, return NV_TRUE if was pending */
NvBool
fecsClearIntrPendingIfPending
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics
)
{
    KGRAPHICS_FECS_TRACE_INFO *pFecsTraceInfo = kgraphicsGetFecsTraceInfo(pGpu, pKernelGraphics);

    NV_ASSERT_OR_RETURN(pFecsTraceInfo != NULL, NV_FALSE);

    return portAtomicCompareAndSwapU32(&pFecsTraceInfo->fecsCtxswLogIntrPending, 0, 1);
}

/*! Atomically check if intr callback pending */
NvBool fecsIsIntrPending
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics
)
{
    KGRAPHICS_FECS_TRACE_INFO *pFecsTraceInfo = kgraphicsGetFecsTraceInfo(pGpu, pKernelGraphics);

    NV_ASSERT_OR_RETURN(pFecsTraceInfo != NULL, NV_FALSE);

    return portAtomicOrU32(&pFecsTraceInfo->fecsCtxswLogIntrPending, 0) != 0;
}

NvS16
fecsGetCtxswLogConsumerCount
(
    OBJGPU *pGpu,
    KernelGraphicsManager *pKernelGraphicsManager
)
{
    KGRMGR_FECS_GLOBAL_TRACE_INFO *pFecsGlobalTraceInfo = kgrmgrGetFecsGlobalTraceInfo(pGpu, pKernelGraphicsManager);

    NV_CHECK_OR_RETURN(LEVEL_ERROR, pFecsGlobalTraceInfo != NULL, 0);

    return pFecsGlobalTraceInfo->fecsCtxswLogConsumerCount;
}

NV_STATUS
fecsIncrementCtxswLogConsumerCount
(
    OBJGPU *pGpu,
    KernelGraphicsManager *pKernelGraphicsManager
)
{
    KGRMGR_FECS_GLOBAL_TRACE_INFO *pFecsGlobalTraceInfo = kgrmgrGetFecsGlobalTraceInfo(pGpu, pKernelGraphicsManager);

    NV_ASSERT_OR_RETURN(pFecsGlobalTraceInfo != NULL, NV_ERR_INVALID_STATE);

    pFecsGlobalTraceInfo->fecsCtxswLogConsumerCount++;

    return NV_OK;
}

NV_STATUS
fecsDecrementCtxswLogConsumerCount
(
    OBJGPU *pGpu,
    KernelGraphicsManager *pKernelGraphicsManager
)
{
    KGRMGR_FECS_GLOBAL_TRACE_INFO *pFecsGlobalTraceInfo = kgrmgrGetFecsGlobalTraceInfo(pGpu, pKernelGraphicsManager);

    NV_ASSERT_OR_RETURN(pFecsGlobalTraceInfo != NULL, NV_ERR_INVALID_STATE);

    pFecsGlobalTraceInfo->fecsCtxswLogConsumerCount--;

    return NV_OK;
}

/*! Retrieve the current VGPU staging buffer */
VGPU_FECS_TRACE_STAGING_BUFFER *
fecsGetVgpuStagingBuffer
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics
)
{
    KGRAPHICS_FECS_TRACE_INFO *pFecsTraceInfo = kgraphicsGetFecsTraceInfo(pGpu, pKernelGraphics);

    NV_ASSERT_OR_RETURN(pFecsTraceInfo != NULL, NULL);

    return pFecsTraceInfo->pVgpuStaging;
}

/*! Store the given VGPU staging buffer */
void
fecsSetVgpuStagingBuffer
(
    OBJGPU *pGpu,
    KernelGraphics *pKernelGraphics,
    VGPU_FECS_TRACE_STAGING_BUFFER *pStagingBuffer
)
{
    KGRAPHICS_FECS_TRACE_INFO *pFecsTraceInfo = kgraphicsGetFecsTraceInfo(pGpu, pKernelGraphics);

    NV_ASSERT_OR_RETURN_VOID(pFecsTraceInfo != NULL);

    pFecsTraceInfo->pVgpuStaging = pStagingBuffer;
}

FecsEventBufferBindMultiMap *
fecsGetEventBufferBindMultiMap
(
    OBJGPU *pGpu,
    KernelGraphicsManager *pKernelGraphicsManager
)
{
    KGRMGR_FECS_GLOBAL_TRACE_INFO *pFecsGlobalTraceInfo = kgrmgrGetFecsGlobalTraceInfo(pGpu, pKernelGraphicsManager);

    NV_ASSERT_OR_RETURN(pFecsGlobalTraceInfo != NULL, NULL);

    return &pFecsGlobalTraceInfo->fecsEventBufferBindingsUid;
}

