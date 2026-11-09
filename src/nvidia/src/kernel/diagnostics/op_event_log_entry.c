/*
 * SPDX-FileCopyrightText: Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "diagnostics/op_event_log_entry.h"

#include "diagnostics/op_event.h"
#include "diagnostics/op_event_log.h"
#include "events/event_defs.h"
#include "libraries/cper/cper.h"
#include "libraries/cper/gpu_cper.h"
#include "nvport/memory.h"
#include "class/cl90cd.h"
#include "class/cl90d0.h"
#include "nvport/nvport.h"
#include "utils/nvassert.h"

#define OP_EVENT_LOG_GOE_PROTOBUF_SUPPORTED 0

#if OP_EVENT_LOG_GOE_PROTOBUF_SUPPORTED
#include "gpu/gpu.h"
#include "gpu/inforom/inforom.h"
#include "inforom/ifrgel.h"
#include "lib/protobuf/prb.h"
#include "lib/protobuf/prb_util.h"
#include "g_events_gpu_pb.h"

#define OP_EVENT_LOG_GOE_SCHEMA_VERSION                       1U
#define OP_EVENT_LOG_GOE_RECORD_BUFFER_SIZE                   INFOROM_GEL_OBJECT_V1_00_DATA_SIZE
// Covers record metadata and the maximum number of context wrappers.
#define OP_EVENT_LOG_GOE_RECORD_OVERHEAD_MAX_SIZE             1536U
#define OP_EVENT_LOG_GOE_CONTEXT_PAYLOAD_MAX_SIZE             4096U
#define OP_EVENT_LOG_GOE_CONTEXT_PAYLOAD_BUDGET               16384U
#define OP_EVENT_LOG_GOE_CONTEXT_MAX_COUNT                     32U
#endif

//
// Walk an event's context chain to render the per-context GOE vardata.
// Each context is padded to NV_EVENT_VARDATA_GRANULARITY to match the
// per-append alignment the EventBuffer producer applies; padding bytes
// are zeroed so they don't leak scratch contents to userspace.
//
// A (pVardata == NULL, vardataCap == 0) call is valid for a zero-vardata
// EventBuffer when the group has no contexts (or all contexts are
// skipped); the per-context capacity check below catches any actual
// write attempt.
//
static NV_STATUS
_walkGoeContextsVardata
(
    EventContextHeader *pChain,
    NvU8               *pVardata,
    NvU32               vardataCap,
    NvU32              *pBytesWritten,
    NvU32              *pContextsWritten
)
{
    NvU32 offset = 0;
    NvU32 written = 0;

    // Caller bug: claimed capacity but supplied no buffer.
    if (pVardata == NULL && vardataCap > 0)
        return NV_ERR_BUFFER_TOO_SMALL;

    for (EventContextHeader *pCtx = pChain; pCtx != NULL; pCtx = pCtx->pNext)
    {
        const EventContextOutputAdapters *pAdapters = pCtx->pOutputAdapters;
        NV_OPERATIONAL_EVENT_CONTEXT *pOutputCtx;
        EventBufferOpEventCtxInfo contextInfo = {0};
        NvU32 outputSize;
        NvU32 ctxSize = 0;
        NvU32 padded;
        NV_STATUS status;

        //
        // Skip contexts that don't have a GOE serializer (e.g.
        // init-metadata until its GOE wire format is defined). Skipped
        // contexts are not counted in pContextsWritten.
        //
        if (pAdapters == NULL || pAdapters->serializeToEventBufferOpEventCtx == NULL)
            continue;

        //
        // Select the slot explicitly. When pVardata == NULL (necessarily
        // vardataCap == 0) the serializer is called with NULL + 0 and
        // reports the size it would produce without touching memory.
        // Computing pVardata + offset when pVardata is NULL would be
        // undefined behavior even though it happens to evaluate to NULL
        // when offset == 0.
        //
        if (pVardata == NULL)
        {
            pOutputCtx = NULL;
            outputSize = 0;
        }
        else
        {
            pOutputCtx = (NV_OPERATIONAL_EVENT_CONTEXT *)(pVardata + offset);
            outputSize = vardataCap - offset;
        }

        status = pAdapters->serializeToEventBufferOpEventCtx(
            pCtx, pOutputCtx, outputSize, &contextInfo);
        if (status != NV_OK)
            return status;

        ctxSize = contextInfo.wireSize;
        padded = NV_ALIGN_UP(ctxSize, NV_EVENT_VARDATA_GRANULARITY);
        if (padded > vardataCap - offset)
            return NV_ERR_BUFFER_TOO_SMALL;

        //
        // Zero the alignment padding so uninitialized scratch bytes
        // don't appear in the consumer-visible vardata.
        //
        if (pVardata != NULL && padded > ctxSize)
            portMemSet(pVardata + offset + ctxSize, 0, padded - ctxSize);

        offset += padded;
        written++;
    }

    *pBytesWritten = offset;
    *pContextsWritten = written;
    return NV_OK;
}

//
// CPER severity is host/system scoped, so a FATAL GPU operational event
// is demoted to RECOVERABLE in the CPER record (a fatal GPU is not a
// fatal host).
//
static NV_CPER_SEVERITY
_gpuOpEventCperSeverity
(
    OPERATIONAL_EVENT_SEVERITY severity
)
{
    switch (severity)
    {
        case OPERATIONAL_EVENT_SEVERITY_FATAL:
            return NV_CPER_SEVERITY_RECOVERABLE;
        case OPERATIONAL_EVENT_SEVERITY_RECOVERABLE:
            return NV_CPER_SEVERITY_RECOVERABLE;
        case OPERATIONAL_EVENT_SEVERITY_CORRECTED:
            return NV_CPER_SEVERITY_CORRECTED;
        case OPERATIONAL_EVENT_SEVERITY_INFORMATIONAL:
        default:
            return NV_CPER_SEVERITY_INFORMATIONAL;
    }
}

static NvU32
_opEventGroupAttrsToCperRecordFlags
(
    NvU8 groupAttrs
)
{
    NvU32 recordFlags = 0;

    if (REF_VAL(OPERATIONAL_EVENT_GROUP_ATTR_RECOVERED, groupAttrs) ==
        OPERATIONAL_EVENT_GROUP_ATTR_RECOVERED_TRUE)
    {
        recordFlags |= NV_CPER_RECORD_FLAG_RECOVERED;
    }

    if (REF_VAL(OPERATIONAL_EVENT_GROUP_ATTR_PREVERR, groupAttrs) ==
        OPERATIONAL_EVENT_GROUP_ATTR_PREVERR_TRUE)
    {
        recordFlags |= NV_CPER_RECORD_FLAG_PREVERR;
    }

    if (REF_VAL(OPERATIONAL_EVENT_GROUP_ATTR_SIMULATED, groupAttrs) ==
        OPERATIONAL_EVENT_GROUP_ATTR_SIMULATED_TRUE)
    {
        recordFlags |= NV_CPER_RECORD_FLAG_SIMULATED;
    }

    return recordFlags;
}

static const OpEventLogEvent *
_opEventLogEntryGetIndexedEvent
(
    const struct OpEventLogEntry *pEntry,
    const NvU8                   *pEventIndices,
    NvU32                         eventIndexCount,
    NvU32                         subsetIndex
)
{
    NvU32 eventIndex;

    if (pEntry == NULL || pEntry->pEvents == NULL || subsetIndex >= eventIndexCount)
        return NULL;

    eventIndex = (pEventIndices != NULL) ? pEventIndices[subsetIndex] : subsetIndex;
    if (eventIndex >= pEntry->eventCount)
        return NULL;

    return &pEntry->pEvents[eventIndex];
}

#if OP_EVENT_LOG_GOE_PROTOBUF_SUPPORTED
static NV_STATUS
_opEventLogEntryRenderGoeContext
(
    PRB_ENCODER        *pPrbEnc,
    EventContextHeader *pCtx,
    NvU32              *pSkippedContexts,
    NvU32              *pPayloadBudget,
    NvBool             *pTruncated
)
{
    const EventContextOutputAdapters *pAdapters;
    NvU8                             *pPayload = NULL;
    NvU32                             wireSize = 0;
    NvU32                             contextType = 0;
    NvU32                             dataFormatVersion = 0;
    NvU32                             dataSize = 0;
    EventBufferOpEventCtxInfo         eventBufferInfo = {0};
    NvBool                            bUseProtobuf = NV_FALSE;
    NvBool                            bHaveContextMetadata = NV_FALSE;
    NvBool                            bPayloadTruncated = NV_FALSE;
    NvU8                              startingDepth;
    NV_STATUS                         status = NV_OK;

    NV_ASSERT_OR_RETURN(pPrbEnc != NULL, NV_ERR_INVALID_POINTER);
    NV_ASSERT_OR_RETURN(pCtx != NULL, NV_ERR_INVALID_POINTER);
    NV_ASSERT_OR_RETURN(pSkippedContexts != NULL, NV_ERR_INVALID_POINTER);
    NV_ASSERT_OR_RETURN(pPayloadBudget != NULL, NV_ERR_INVALID_POINTER);
    NV_ASSERT_OR_RETURN(pTruncated != NULL, NV_ERR_INVALID_POINTER);

    startingDepth = prbEncNestingLevel(pPrbEnc);
    pAdapters = pCtx->pOutputAdapters;
    if (pAdapters == NULL)
    {
        (*pSkippedContexts)++;
        *pTruncated = NV_TRUE;
        return NV_OK;
    }

    if (pAdapters->serializeToProtobuf != NULL)
    {
        PRB_ENCODER countEnc;
        void       *pEncoded;

        prbEncStartCount(&countEnc, GPUEVENTS_GPUOPERATIONALEVENTRECORD_CONTEXT,
                         OP_EVENT_LOG_GOE_CONTEXT_PAYLOAD_MAX_SIZE);
        status = pAdapters->serializeToProtobuf(
            pCtx, &countEnc, &contextType, &dataFormatVersion);
        wireSize = prbEncFinish(&countEnc, &pEncoded);
        if (status == NV_OK)
        {
            bUseProtobuf = NV_TRUE;
            bHaveContextMetadata = NV_TRUE;
            dataSize = pCtx->dataSize;
        }
        else
        {
            status = NV_OK;
            wireSize = 0;
        }
    }

    if (!bUseProtobuf)
    {
        if (pAdapters->serializeToEventBufferOpEventCtx == NULL)
        {
            (*pSkippedContexts)++;
            *pTruncated = NV_TRUE;
            return NV_OK;
        }

        status = pAdapters->serializeToEventBufferOpEventCtx(
            pCtx, NULL, 0, &eventBufferInfo);
        if (status != NV_OK)
        {
            (*pSkippedContexts)++;
            *pTruncated = NV_TRUE;
            return NV_OK;
        }

        wireSize = eventBufferInfo.wireSize;
        contextType = eventBufferInfo.contextType;
        dataFormatVersion = eventBufferInfo.dataFormatVersion;
        dataSize = eventBufferInfo.dataSize;
        bHaveContextMetadata = NV_TRUE;
    }

    if ((wireSize > OP_EVENT_LOG_GOE_CONTEXT_PAYLOAD_MAX_SIZE) ||
        (wireSize > *pPayloadBudget))
    {
        bPayloadTruncated = NV_TRUE;
        *pTruncated = NV_TRUE;
    }
    else if (!bUseProtobuf && wireSize != 0)
    {
        pPayload = portMemAllocStackOrHeap(wireSize);
        if (pPayload == NULL)
            return NV_ERR_NO_MEMORY;

        status = pAdapters->serializeToEventBufferOpEventCtx(
            pCtx, (NV_OPERATIONAL_EVENT_CONTEXT *)pPayload,
            wireSize, &eventBufferInfo);

        if ((status != NV_OK) || (eventBufferInfo.wireSize != wireSize))
        {
            (*pSkippedContexts)++;
            *pTruncated = NV_TRUE;
            status = NV_OK;
            goto cleanup;
        }

        *pPayloadBudget -= wireSize;
    }

    NV_CHECK_OK_OR_GOTO(status, LEVEL_NOTICE,
        prbEncNestedStart(pPrbEnc, GPUEVENTS_GPUOPERATIONALEVENTRECORD_EVENT_CONTEXT),
        cleanup);

    if (bUseProtobuf && !bPayloadTruncated)
    {
        NV_CHECK_OK_OR_GOTO(status, LEVEL_NOTICE,
            pAdapters->serializeToProtobuf(
                pCtx, pPrbEnc, &contextType, &dataFormatVersion),
            cleanup);
        *pPayloadBudget -= wireSize;
    }
    else if (!bUseProtobuf)
    {
        NV_CHECK_OK_OR_GOTO(status, LEVEL_NOTICE,
            prbEncNestedStart(
                pPrbEnc, GPUEVENTS_GPUOPERATIONALEVENTRECORD_CONTEXT_OPAQUE_EVENT_BUFFER),
            cleanup);
        NV_CHECK_OK_OR_GOTO(status, LEVEL_NOTICE,
            prbEncAddUInt32(
                pPrbEnc,
                GPUEVENTS_OPAQUEEVENTBUFFERCONTEXT_WIRE_SIZE,
                wireSize),
            cleanup);

        if (pPayload != NULL)
        {
            NV_CHECK_OK_OR_GOTO(status, LEVEL_NOTICE,
                prbEncAddBytes(
                    pPrbEnc,
                    GPUEVENTS_OPAQUEEVENTBUFFERCONTEXT_PAYLOAD,
                    (const NvU8 *)pPayload, wireSize),
                cleanup);
        }

        NV_CHECK_OK_OR_GOTO(status, LEVEL_NOTICE,
            prbEncNestedEnd(pPrbEnc), cleanup);
    }

    if (bHaveContextMetadata)
    {
        NV_CHECK_OK_OR_GOTO(status, LEVEL_NOTICE,
            prbEncAddUInt32(pPrbEnc,
                            GPUEVENTS_GPUOPERATIONALEVENTRECORD_CONTEXT_CONTEXT_TYPE,
                            contextType),
            cleanup);
        NV_CHECK_OK_OR_GOTO(status, LEVEL_NOTICE,
            prbEncAddUInt32(pPrbEnc,
                            GPUEVENTS_GPUOPERATIONALEVENTRECORD_CONTEXT_DATA_FORMAT_VERSION,
                            dataFormatVersion),
            cleanup);
        NV_CHECK_OK_OR_GOTO(status, LEVEL_NOTICE,
            prbEncAddUInt32(pPrbEnc,
                            GPUEVENTS_GPUOPERATIONALEVENTRECORD_CONTEXT_DATA_SIZE,
                            dataSize),
            cleanup);
    }

    NV_CHECK_OK_OR_GOTO(status, LEVEL_NOTICE,
        prbEncAddBool(pPrbEnc,
                      GPUEVENTS_GPUOPERATIONALEVENTRECORD_CONTEXT_PAYLOAD_TRUNCATED,
                      bPayloadTruncated),
        cleanup);

    NV_CHECK_OK_OR_GOTO(status, LEVEL_NOTICE, prbEncNestedEnd(pPrbEnc), cleanup);

cleanup:
    portMemFreeStackOrHeap(pPayload);
    NV_CHECK_OK_OR_CAPTURE_FIRST_ERROR(status, LEVEL_NOTICE,
        prbEncUnwindNesting(pPrbEnc, startingDepth));

    return status;
}

static NV_STATUS
_opEventLogEntryRenderGoeEvent
(
    PRB_ENCODER              *pPrbEnc,
    const OpEventLogEvent    *pEv,
    NvU32                     recordBufferCap,
    NvU32                    *pSkippedContexts,
    NvBool                   *pTruncated
)
{
    EventContextHeader *pCtx;
    NvU32               contextCount = 0;
    NvU32               payloadBudget =
        (recordBufferCap > OP_EVENT_LOG_GOE_RECORD_OVERHEAD_MAX_SIZE) ?
            NV_MIN(OP_EVENT_LOG_GOE_CONTEXT_PAYLOAD_BUDGET,
                   recordBufferCap -
                       OP_EVENT_LOG_GOE_RECORD_OVERHEAD_MAX_SIZE) :
            0;
    NvU8               startingDepth = prbEncNestingLevel(pPrbEnc);
    NV_STATUS          status = NV_OK;

    NV_ASSERT_OR_RETURN(pPrbEnc != NULL, NV_ERR_INVALID_POINTER);
    NV_ASSERT_OR_RETURN(pEv != NULL, NV_ERR_INVALID_POINTER);
    NV_ASSERT_OR_RETURN(pSkippedContexts != NULL, NV_ERR_INVALID_POINTER);
    NV_ASSERT_OR_RETURN(pTruncated != NULL, NV_ERR_INVALID_POINTER);

    NV_CHECK_OK_OR_GOTO(status, LEVEL_NOTICE,
        prbEncNestedStart(pPrbEnc, GPUEVENTS_GPUOPERATIONALEVENTRECORD_GROUP_EVENT),
        cleanup);

    NV_CHECK_OK_OR_GOTO(status, LEVEL_NOTICE,
        prbEncAddUInt32(pPrbEnc, GPUEVENTS_GPUOPERATIONALEVENTRECORD_EVENT_CATEGORY, pEv->category),
        cleanup);
    NV_CHECK_OK_OR_GOTO(status, LEVEL_NOTICE,
        prbEncAddUInt32(pPrbEnc, GPUEVENTS_GPUOPERATIONALEVENTRECORD_EVENT_EVENT_CODE, pEv->eventCode),
        cleanup);
    NV_CHECK_OK_OR_GOTO(status, LEVEL_NOTICE,
        prbEncAddBytes(pPrbEnc, GPUEVENTS_GPUOPERATIONALEVENTRECORD_EVENT_MODULE_SIGNATURE,
                       (const NvU8 *)pEv->moduleSignature,
                       sizeof(pEv->moduleSignature)),
        cleanup);
    NV_CHECK_OK_OR_GOTO(status, LEVEL_NOTICE,
        prbEncAddUInt32(pPrbEnc, GPUEVENTS_GPUOPERATIONALEVENTRECORD_EVENT_SEVERITY, pEv->severity),
        cleanup);
    NV_CHECK_OK_OR_GOTO(status, LEVEL_NOTICE,
        prbEncAddUInt32(pPrbEnc, GPUEVENTS_GPUOPERATIONALEVENTRECORD_EVENT_ATTRIBUTES, pEv->attributes),
        cleanup);
    NV_CHECK_OK_OR_GOTO(status, LEVEL_NOTICE,
        prbEncAddUInt32(pPrbEnc, GPUEVENTS_GPUOPERATIONALEVENTRECORD_EVENT_LOG_LEVEL, pEv->logLevel),
        cleanup);
    NV_CHECK_OK_OR_GOTO(status, LEVEL_NOTICE,
        prbEncAddUInt64(pPrbEnc, GPUEVENTS_GPUOPERATIONALEVENTRECORD_EVENT_INSTANCE_ID, pEv->instanceId),
        cleanup);
    NV_CHECK_OK_OR_GOTO(status, LEVEL_NOTICE,
        prbEncAddUInt64(pPrbEnc, GPUEVENTS_GPUOPERATIONALEVENTRECORD_EVENT_TIMESTAMP_US, pEv->timestampUs),
        cleanup);

    NV_CHECK_OK_OR_GOTO(status, LEVEL_NOTICE,
        prbEncAddUInt32(pPrbEnc, GPUEVENTS_GPUOPERATIONALEVENTRECORD_EVENT_SCOPE, pEv->scope),
        cleanup);
    NV_CHECK_OK_OR_GOTO(status, LEVEL_NOTICE,
        prbEncAddUInt32(pPrbEnc, GPUEVENTS_GPUOPERATIONALEVENTRECORD_EVENT_ORIGINATOR, pEv->originator),
        cleanup);
    NV_CHECK_OK_OR_GOTO(status, LEVEL_NOTICE,
        prbEncAddUInt32(pPrbEnc,
                        GPUEVENTS_GPUOPERATIONALEVENTRECORD_EVENT_REPORTING_SOURCE,
                        pEv->reportingSource),
        cleanup);
    NV_CHECK_OK_OR_GOTO(status, LEVEL_NOTICE,
        prbEncAddUInt64(pPrbEnc, GPUEVENTS_GPUOPERATIONALEVENTRECORD_EVENT_PDI, pEv->pdi),
        cleanup);
    NV_CHECK_OK_OR_GOTO(status, LEVEL_NOTICE,
        prbEncAddBytes(pPrbEnc, GPUEVENTS_GPUOPERATIONALEVENTRECORD_EVENT_DEVICE_UUID,
                       pEv->deviceUuid, sizeof(pEv->deviceUuid)),
        cleanup);
    NV_CHECK_OK_OR_GOTO(status, LEVEL_NOTICE,
        prbEncAddUInt32(pPrbEnc,
                        GPUEVENTS_GPUOPERATIONALEVENTRECORD_EVENT_MODULE_INSTANCE,
                        pEv->moduleInstance),
        cleanup);
    NV_CHECK_OK_OR_GOTO(status, LEVEL_NOTICE,
        prbEncAddUInt32(pPrbEnc, GPUEVENTS_GPUOPERATIONALEVENTRECORD_EVENT_CHIPLET_ID, pEv->chipletId),
        cleanup);
    NV_CHECK_OK_OR_GOTO(status, LEVEL_NOTICE,
        prbEncAddUInt32(pPrbEnc,
                        GPUEVENTS_GPUOPERATIONALEVENTRECORD_EVENT_MIG_ATTRIBUTION,
                        pEv->migAttribution),
        cleanup);
    NV_CHECK_OK_OR_GOTO(status, LEVEL_NOTICE,
        prbEncAddUInt32(pPrbEnc,
                        GPUEVENTS_GPUOPERATIONALEVENTRECORD_EVENT_PRODUCER_GROUP_INDEX,
                        pEv->producerGroupIndex),
        cleanup);

    for (pCtx = pEv->pCtxChainHead; pCtx != NULL; pCtx = pCtx->pNext)
    {
        if (contextCount++ >= OP_EVENT_LOG_GOE_CONTEXT_MAX_COUNT)
        {
            (*pSkippedContexts)++;
            *pTruncated = NV_TRUE;
            continue;
        }

        NV_CHECK_OK_OR_GOTO(status, LEVEL_NOTICE,
            _opEventLogEntryRenderGoeContext(pPrbEnc, pCtx, pSkippedContexts,
                                              &payloadBudget, pTruncated),
            cleanup);
    }

    NV_CHECK_OK_OR_GOTO(status, LEVEL_NOTICE, prbEncNestedEnd(pPrbEnc), cleanup);

cleanup:
    NV_CHECK_OK_OR_CAPTURE_FIRST_ERROR(status, LEVEL_NOTICE,
        prbEncUnwindNesting(pPrbEnc, startingDepth));

    return status;
}
#endif // OP_EVENT_LOG_GOE_PROTOBUF_SUPPORTED

static NV_STATUS
_opEventLogEntryRenderCperSubset
(
    const struct OpEventLogEntry *pEntry,
    const NvU8                   *pEventIndices,
    NvU32                         eventIndexCount,
    NvU8                         *pBuffer,
    NvU32                         bufferCap,
    NvU32                        *pRecordSize
)
{
    NV_CPER_INIT_PARAMS              initParams;
    NV_CPER_NV_EVENT_PARAMS          eventParams;
    NV_CPER_NV_EVENT_SECTION_STATE   eventState;
    const OpEventLogEvent           *pFirst;
    NvU32                            i;
    NV_STATUS                        status;
    EventContextHeader              *pCtx;

    NV_ASSERT_OR_RETURN(pEntry != NULL, NV_ERR_INVALID_POINTER);
    NV_ASSERT_OR_RETURN(pBuffer != NULL, NV_ERR_INVALID_POINTER);
    NV_ASSERT_OR_RETURN(pRecordSize != NULL, NV_ERR_INVALID_POINTER);
    NV_ASSERT_OR_RETURN(pEntry->pEvents != NULL && pEntry->eventCount > 0,
                        NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(eventIndexCount > 0, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(eventIndexCount <= NV_U16_MAX, NV_ERR_INVALID_ARGUMENT);

    pFirst = _opEventLogEntryGetIndexedEvent(pEntry, pEventIndices, eventIndexCount, 0);
    NV_ASSERT_OR_RETURN(pFirst != NULL, NV_ERR_INVALID_ARGUMENT);

    portMemSet(&initParams, 0, sizeof(initParams));
    initParams.pNotifyType       = &pEntry->desc.notifyType;
    initParams.pCreatorId        = &pEntry->desc.creatorId;
    initParams.pPlatformId       = NULL;
    initParams.pPartitionId      = NULL;
    initParams.timestampUs       = pFirst->timestampUs;
    initParams.recordId          = pEntry->groupCursor;
    initParams.recordFlags       = _opEventGroupAttrsToCperRecordFlags(pEntry->groupAttrs);
    initParams.bTimestampPrecise = pEntry->desc.bTimestampPrecise;
    initParams.sectionCount      = (NvU16)eventIndexCount;

    status = cperInit(pBuffer, bufferCap, &initParams);
    if (status != NV_OK)
        return status;

    //
    // One CPER section per event; the section's contexts come from that event's context chain.
    //
    for (i = 0; i < eventIndexCount; i++)
    {
        const OpEventLogEvent *pEv =
            _opEventLogEntryGetIndexedEvent(pEntry, pEventIndices, eventIndexCount, i);
        NV_CPER_GUID           fruIdGuid;
        const NV_CPER_GUID    *pFruId = NULL;

        NV_ASSERT_OR_RETURN(pEv != NULL, NV_ERR_INVALID_ARGUMENT);

        if (pEntry->desc.bDeviceUuidValid && cperGuidFromUuidBytes(pEv->deviceUuid, &fruIdGuid))
            pFruId = &fruIdGuid;

        portMemSet(&eventParams, 0, sizeof(eventParams));
        eventParams.severity           = _gpuOpEventCperSeverity(pEv->severity);
        eventParams.sectionFlags       = pEntry->desc.sectionFlags;
        eventParams.pFruId             = pFruId;
        eventParams.pFruText           = NULL;
        eventParams.eventType          = pEv->category;
        eventParams.eventSubType       = pEv->eventCode;
        eventParams.pModuleSignature   = pEv->moduleSignature;
        eventParams.traceId            = pEntry->traceId;
        eventParams.originator         = (NV_CPER_NV_GPU_EVENT_ORIGINATOR)pEv->originator;
        eventParams.pdi                = pEv->pdi;
        eventParams.sourcePartition    = pEntry->desc.sourcePartition;
        eventParams.sourceSubPartition = pEntry->desc.sourceSubPartition;

        status = cperAddNvidiaEventSection(pBuffer, bufferCap, &eventParams, &eventState);
        if (status != NV_OK)
            return status;

        //
        // Drive each context's CPER serializer; skip contexts that don't provide one
        // (e.g. init-metadata until its CPER thunk lands).
        //
        for (pCtx = pEv->pCtxChainHead; pCtx != NULL; pCtx = pCtx->pNext)
        {
            if (pCtx->pOutputAdapters != NULL &&
                pCtx->pOutputAdapters->serializeToCperEventCtx != NULL)
            {
                status = pCtx->pOutputAdapters->serializeToCperEventCtx(pCtx, &eventState);
                if (status != NV_OK)
                    return status;
            }
        }
    }

    return cperGetRecordSize(pBuffer, bufferCap, pRecordSize);
}

NV_STATUS
opEventLogEntryRenderGoeProtobufEvent
(
    const struct OpEventLogEntry *pEntry,
    NvU32                         eventIndex,
    NvU8                         *pBuffer,
    NvU32                         bufferCap,
    NvU32                        *pRecordSize,
    NvBool                       *pTruncated
)
{
#if OP_EVENT_LOG_GOE_PROTOBUF_SUPPORTED
    PRB_ENCODER prbEnc;
    NvU32       skippedContextCount = 0;
    NvU8        startingDepth;
    void       *pEncoded = NULL;
    NvU32       encodedSize;
    NV_STATUS   status = NV_OK;

    NV_ASSERT_OR_RETURN(pEntry != NULL, NV_ERR_INVALID_POINTER);
    NV_ASSERT_OR_RETURN(pBuffer != NULL, NV_ERR_INVALID_POINTER);
    NV_ASSERT_OR_RETURN(pRecordSize != NULL, NV_ERR_INVALID_POINTER);
    NV_ASSERT_OR_RETURN(pTruncated != NULL, NV_ERR_INVALID_POINTER);
    NV_ASSERT_OR_RETURN(pEntry->pEvents != NULL && pEntry->eventCount > 0,
                        NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(eventIndex < pEntry->eventCount, NV_ERR_INVALID_ARGUMENT);

    *pRecordSize = 0;
    *pTruncated = NV_FALSE;

    prbEncStart(&prbEnc, GPUEVENTS_GPUOPERATIONALEVENTRECORD, pBuffer, bufferCap, NULL);
    startingDepth = prbEncNestingLevel(&prbEnc);

    NV_CHECK_OK_OR_GOTO(status, LEVEL_NOTICE,
        prbEncAddUInt32(&prbEnc,
                        GPUEVENTS_GPUOPERATIONALEVENTRECORD_SCHEMA_VERSION,
                        OP_EVENT_LOG_GOE_SCHEMA_VERSION),
        cleanup);
    NV_CHECK_OK_OR_GOTO(status, LEVEL_NOTICE,
        prbEncNestedStart(&prbEnc, GPUEVENTS_GPUOPERATIONALEVENTRECORD_EVENT_GROUP),
        cleanup);

    NV_CHECK_OK_OR_GOTO(status, LEVEL_NOTICE,
        prbEncAddUInt64(&prbEnc,
                        GPUEVENTS_GPUOPERATIONALEVENTRECORD_GROUP_GROUP_CURSOR,
                        pEntry->groupCursor),
        cleanup);
    NV_CHECK_OK_OR_GOTO(status, LEVEL_NOTICE,
        prbEncAddUInt64(&prbEnc, GPUEVENTS_GPUOPERATIONALEVENTRECORD_GROUP_TRACE_ID, pEntry->traceId),
        cleanup);
    NV_CHECK_OK_OR_GOTO(status, LEVEL_NOTICE,
        prbEncAddUInt32(&prbEnc,
                        GPUEVENTS_GPUOPERATIONALEVENTRECORD_GROUP_GROUP_ATTRIBUTES,
                        pEntry->groupAttrs),
        cleanup);
    NV_CHECK_OK_OR_GOTO(status, LEVEL_NOTICE,
        prbEncAddUInt32(&prbEnc,
                        GPUEVENTS_GPUOPERATIONALEVENTRECORD_GROUP_EVENT_COUNT,
                        pEntry->eventCount),
        cleanup);
    NV_CHECK_OK_OR_GOTO(status, LEVEL_NOTICE,
        prbEncAddUInt32(&prbEnc,
                        GPUEVENTS_GPUOPERATIONALEVENTRECORD_GROUP_PRODUCER_GROUP_SIZE,
                        pEntry->producerGroupSize),
        cleanup);

    NV_CHECK_OK_OR_GOTO(status, LEVEL_NOTICE,
        _opEventLogEntryRenderGoeEvent(&prbEnc,
                                       &pEntry->pEvents[eventIndex],
                                       bufferCap,
                                       &skippedContextCount,
                                       pTruncated),
        cleanup);

    NV_CHECK_OK_OR_GOTO(status, LEVEL_NOTICE,
        prbEncAddUInt32(&prbEnc,
                        GPUEVENTS_GPUOPERATIONALEVENTRECORD_GROUP_SKIPPED_CONTEXT_COUNT,
                        skippedContextCount),
        cleanup);
    NV_CHECK_OK_OR_GOTO(status, LEVEL_NOTICE, prbEncNestedEnd(&prbEnc), cleanup);

cleanup:
    NV_CHECK_OK_OR_CAPTURE_FIRST_ERROR(status, LEVEL_NOTICE,
        prbEncUnwindNesting(&prbEnc, startingDepth));
    encodedSize = prbEncFinish(&prbEnc, &pEncoded);

    if (status != NV_OK)
        return status;

    if (encodedSize > bufferCap)
        return NV_ERR_BUFFER_TOO_SMALL;

    *pRecordSize = encodedSize;
    return NV_OK;
#else
    (void)pEntry;
    (void)eventIndex;
    (void)pBuffer;
    (void)bufferCap;
    (void)pRecordSize;
    (void)pTruncated;

    return NV_ERR_NOT_SUPPORTED;
#endif
}

#if OP_EVENT_LOG_GOE_PROTOBUF_SUPPORTED
typedef struct OP_EVENT_LOG_GOE_RENDER_CONTEXT
{
    const OpEventLogEntry *pEntry;
    NvU32                  eventIndex;
} OP_EVENT_LOG_GOE_RENDER_CONTEXT;

static NV_STATUS
_opEventLogEntryRenderGoeRecord
(
    void   *pContext,
    NvU8   *pBuffer,
    NvU32   bufferSize,
    NvU32  *pRecordSize,
    NvBool *pTruncated
)
{
    const OP_EVENT_LOG_GOE_RENDER_CONTEXT *pRenderContext = pContext;

    NV_ASSERT_OR_RETURN(pRenderContext != NULL, NV_ERR_INVALID_POINTER);

    return opEventLogEntryRenderGoeProtobufEvent(
        pRenderContext->pEntry,
        pRenderContext->eventIndex,
        pBuffer,
        bufferSize,
        pRecordSize,
        pTruncated);
}
#endif

void
opEventLogEntryPersistent
(
    struct OpEventLog *pLog,
    OBJGPU *pGpu,
    const struct OpEventLogEntry *pEntry
)
{
#if OP_EVENT_LOG_GOE_PROTOBUF_SUPPORTED
    OBJINFOROM *pInforom;
    NvU8       *pBuffer = NULL;
    NvU32       eventIndex;
    NvBool      bSimulated;

    NV_ASSERT_OR_RETURN_VOID(pLog != NULL);
    NV_ASSERT_OR_RETURN_VOID(pGpu != NULL);
    NV_ASSERT_OR_RETURN_VOID(pEntry != NULL);

    pInforom = GPU_GET_INFOROM(pGpu);
    if (pInforom == NULL)
        return;
    if (IS_GSP_CLIENT(pGpu))
        return;

    pBuffer = opevtlogGetRenderScratch(
        pLog, OP_EVENT_LOG_GOE_RECORD_BUFFER_SIZE);
    if (pBuffer == NULL)
        return;

    bSimulated =
        REF_VAL(OPERATIONAL_EVENT_GROUP_ATTR_SIMULATED, pEntry->groupAttrs) ==
        OPERATIONAL_EVENT_GROUP_ATTR_SIMULATED_TRUE;

    for (eventIndex = 0; eventIndex < pEntry->eventCount; eventIndex++)
    {
        const OpEventLogEvent *pEvent = &pEntry->pEvents[eventIndex];
        OP_EVENT_LOG_GOE_RENDER_CONTEXT renderContext =
        {
            .pEntry = pEntry,
            .eventIndex = eventIndex,
        };
        INFOROM_GEL_RECORD_REQUEST request =
        {
            .pRender = _opEventLogEntryRenderGoeRecord,
            .pRenderContext = &renderContext,
            .pRenderBuffer = pBuffer,
            .renderBufferSize = OP_EVENT_LOG_GOE_RECORD_BUFFER_SIZE,
            .timestampUs = pEvent->timestampUs,
            .goeSchemaVersion = OP_EVENT_LOG_GOE_SCHEMA_VERSION,
            .severity = pEvent->severity,
            .bSimulated = bSimulated,
            .bQueueFlush = eventIndex == (pEntry->eventCount - 1),
        };
        NV_STATUS status;

        status = inforomGelPersistRecord_HAL(
            pGpu, pInforom, &request);
        if (status == NV_ERR_NOT_SUPPORTED)
            break;

        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_NOTICE,
                      "GEL InfoROM persistence failed for event %u "
                      "(status=0x%x)\n",
                      eventIndex, status);
        }
    }
#else
    (void)pLog;
    (void)pGpu;
    (void)pEntry;
#endif
}

NV_STATUS
opEventLogEntryRenderCper
(
    const struct OpEventLogEntry *pEntry,
    NvU8                         *pBuffer,
    NvU32                         bufferCap,
    NvU32                        *pRecordSize
)
{
    NV_ASSERT_OR_RETURN(pEntry != NULL, NV_ERR_INVALID_POINTER);

    return _opEventLogEntryRenderCperSubset(pEntry, NULL, pEntry->eventCount,
                                            pBuffer, bufferCap, pRecordSize);
}

NV_STATUS
opEventLogEntryRenderToEventBufferGoe
(
    const struct OpEventLogEntry *pEntry,
    NvU32                         eventIndex,
    NvU8                          groupSize,
    NvU8                          groupIndex,
    NvU8                          bindId,
    NvU8                         *pInlinePayload,
    NvU32                         inlinePayloadCap,
    NvU8                         *pVardata,
    NvU32                         vardataCap,
    NvU32                        *pInlinePayloadWritten,
    NvU32                        *pVardataWritten
)
{
    NV_GPU_OPERATIONAL_EVENT_RECORD     gpuRec;
    const OpEventLogEvent              *pEv;
    NV_OPERATIONAL_EVENT_RECORD        *pInner = &gpuRec.event;
    NvU32                               vardataSize = 0;
    NvU32                               contextsWritten = 0;
    NvU32                               afterHeaderSize;
    NV_STATUS                           status;

    NV_ASSERT_OR_RETURN(pEntry != NULL, NV_ERR_INVALID_POINTER);
    NV_ASSERT_OR_RETURN(pInlinePayload != NULL, NV_ERR_INVALID_POINTER);
    NV_ASSERT_OR_RETURN(pInlinePayloadWritten != NULL, NV_ERR_INVALID_POINTER);
    NV_ASSERT_OR_RETURN(pVardataWritten != NULL, NV_ERR_INVALID_POINTER);
    NV_ASSERT_OR_RETURN(eventIndex < pEntry->eventCount, NV_ERR_INVALID_ARGUMENT);

    pEv = &pEntry->pEvents[eventIndex];

    //
    // Body size: everything after the leading
    // NV_EVENT_BUFFER_RECORD_HEADER — the EventBuffer producer writes
    // the header itself.
    //
    afterHeaderSize = (NvU32)(sizeof(gpuRec) - sizeof(NV_EVENT_BUFFER_RECORD_HEADER));
    if (inlinePayloadCap < afterHeaderSize)
        return NV_ERR_BUFFER_TOO_SMALL;

    portMemSet(&gpuRec, 0, sizeof(gpuRec));

    // Inner NV_OPERATIONAL_EVENT_RECORD.
    pInner->traceId          = pEntry->traceId;
    pInner->instanceId       = pEv->instanceId;
    pInner->timestamp        = pEv->timestampUs;
    portMemCopy(pInner->moduleSignature, sizeof(pInner->moduleSignature),
                pEv->moduleSignature, sizeof(pEv->moduleSignature));
    pInner->eventCategory    = pEv->category;
    pInner->moduleEventCode  = pEv->eventCode;
    pInner->severity         = (NvU8)pEv->severity;
    pInner->attributes       = (NvU8)pEv->attributes;
    pInner->groupAttributes  = pEntry->groupAttrs;
    pInner->bindId           = bindId;
    pInner->cursorValue      = pEntry->groupCursor;
    pInner->numContexts      = 0;   // populated below (serialized count only)
    pInner->totalContextSize = 0;   // populated below
    pInner->groupCperSize    = 0;   // GOE records don't carry CPER size
    pInner->groupSize        = groupSize;
    pInner->groupIndex       = groupIndex;

    // GPU extension.
    portMemCopy(gpuRec.deviceUuid, sizeof(gpuRec.deviceUuid),
                pEv->deviceUuid, sizeof(pEv->deviceUuid));
    gpuRec.scope            = pEv->scope;
    gpuRec.originator       = pEv->originator;
    gpuRec.moduleInstance   = pEv->moduleInstance;
    gpuRec.chipletId        = (NvU8)pEv->chipletId;
    gpuRec.migAttribution   = pEv->migAttribution;
    gpuRec.logLevel         = pEv->logLevel;
    gpuRec.reportingSource  = pEv->reportingSource;

    status = _walkGoeContextsVardata(pEv->pCtxChainHead,
                                     pVardata, vardataCap,
                                     &vardataSize, &contextsWritten);
    if (status != NV_OK)
        return status;

    pInner->numContexts      = contextsWritten;
    pInner->totalContextSize = vardataSize;

    portMemCopy(pInlinePayload, inlinePayloadCap,
                (const NvU8 *)&gpuRec + sizeof(NV_EVENT_BUFFER_RECORD_HEADER),
                afterHeaderSize);
    *pInlinePayloadWritten = afterHeaderSize;
    *pVardataWritten       = vardataSize;
    return NV_OK;
}

NV_STATUS
opEventLogEntryRenderToEventBufferCperSubset
(
    const struct OpEventLogEntry *pEntry,
    const NvU8                   *pEventIndices,
    NvU32                         eventIndexCount,
    NvU8                          bindId,
    NvU8                         *pInlinePayload,
    NvU32                         inlinePayloadCap,
    NvU8                         *pVardata,
    NvU32                         vardataCap,
    NvU32                        *pInlinePayloadWritten,
    NvU32                        *pVardataWritten
)
{
    NV_OPERATIONAL_EVENT_CPER_RECORD cperRec;
    NvU32                            afterHeaderSize;
    NvU32                            cperSize = 0;
    NV_STATUS                        status;

    NV_ASSERT_OR_RETURN(pEntry != NULL, NV_ERR_INVALID_POINTER);
    NV_ASSERT_OR_RETURN(pInlinePayload != NULL, NV_ERR_INVALID_POINTER);
    NV_ASSERT_OR_RETURN(pVardata != NULL, NV_ERR_INVALID_POINTER);
    NV_ASSERT_OR_RETURN(pInlinePayloadWritten != NULL, NV_ERR_INVALID_POINTER);
    NV_ASSERT_OR_RETURN(pVardataWritten != NULL, NV_ERR_INVALID_POINTER);

    afterHeaderSize = (NvU32)(sizeof(cperRec) - sizeof(NV_EVENT_BUFFER_RECORD_HEADER));
    if (inlinePayloadCap < afterHeaderSize)
        return NV_ERR_BUFFER_TOO_SMALL;

    status = _opEventLogEntryRenderCperSubset(pEntry, pEventIndices, eventIndexCount,
                                              pVardata, vardataCap, &cperSize);
    if (status != NV_OK)
        return status;

    portMemSet(&cperRec, 0, sizeof(cperRec));
    cperRec.cursorValue = pEntry->groupCursor;
    cperRec.cperSize    = cperSize;
    cperRec.bindId      = bindId;

    portMemCopy(pInlinePayload, inlinePayloadCap,
                (const NvU8 *)&cperRec + sizeof(NV_EVENT_BUFFER_RECORD_HEADER),
                afterHeaderSize);
    *pInlinePayloadWritten = afterHeaderSize;
    *pVardataWritten       = cperSize;
    return NV_OK;
}

