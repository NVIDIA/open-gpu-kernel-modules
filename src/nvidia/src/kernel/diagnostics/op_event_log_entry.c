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
#include "libraries/cper/cper.h"
#include "libraries/cper/gpu_cper.h"
#include "class/cl90cd.h"
#include "class/cl90d0.h"
#include "nvport/nvport.h"
#include "utils/nvassert.h"

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
            pCtx, pOutputCtx, outputSize, &ctxSize);
        if (status != NV_OK)
            return status;

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

