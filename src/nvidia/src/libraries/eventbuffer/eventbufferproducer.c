/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "eventbufferproducer.h"
#include "nvport/nvport.h"
#include "utils/nvassert.h"

//
// This file contains generic event buffer producer implementation for adding variable length data
//
// Data format:
//
// Event Record buffer holds fixed size records
//
// |---------|---------|---------|---------|...|---------|
// | record1 | record2 | record3 | record4 |...| recordn |
// |---------|---------|---------|---------|...|---------|
//
// Variable length data buffer:
// The fixed event record can optionally contain a pointer to variable length data.
// This buffer stores the varlength data that doesn't fit in the fixed size records.
//
// |------------|--------|...|---------|
// | data2      | data4  |...| data n  |
// |------------|--------|...|---------|
//

static NV_EVENT_BUFFER_RECORD* _eventBufferGetFreeRecord(EVENT_BUFFER_PRODUCER_INFO *);
static void _eventBufferAddVardata(EVENT_BUFFER_PRODUCER_INFO*, NvP64, NvU32, NV_EVENT_BUFFER_RECORD_HEADER*);
static void _eventBufferUpdateRecordBufferCount(EVENT_BUFFER_PRODUCER_INFO*);
static void _eventBufferUpdateVarRemaingSize(EVENT_BUFFER_PRODUCER_INFO* info);

void
eventBufferInitRecordBuffer
(
    EVENT_BUFFER_PRODUCER_INFO *info,
    NV_EVENT_BUFFER_HEADER* pHeader,
    NvP64 recordBuffAddr,
    NvU32 recordSize,
    NvU32 recordCount,
    NvU32 bufferSize,
    NvU32 notificationThreshold
)
{
    RECORD_BUFFER_INFO* pRecordBuffer = &info->recordBuffer;
    pRecordBuffer->pHeader = pHeader;
    pRecordBuffer->recordBuffAddr = recordBuffAddr;
    pRecordBuffer->recordSize = recordSize;
    pRecordBuffer->totalRecordCount = recordCount;
    pRecordBuffer->bufferSize = bufferSize;
    pRecordBuffer->notificationThreshold = notificationThreshold;

    // Opt-in live-ring threshold gating.
    info->bMaintainRecordCount = NV_FALSE;
}

void
eventBufferInitVardataBuffer
(
    EVENT_BUFFER_PRODUCER_INFO *info,
    NvP64 vardataBuffAddr,
    NvU32 bufferSize,
    NvU32 notificationThreshold
)
{
    VARDATA_BUFFER_INFO* pVardataBuffer = &info->vardataBuffer;
    pVardataBuffer->vardataBuffAddr = vardataBuffAddr;
    pVardataBuffer->bufferSize = bufferSize;
    pVardataBuffer->notificationThreshold = notificationThreshold;
    pVardataBuffer->get = 0;
    pVardataBuffer->put = 0;
    pVardataBuffer->remainingSize = bufferSize;
}

void
eventBufferInitNotificationHandle(EVENT_BUFFER_PRODUCER_INFO *info, NvP64 notificationHandle)
{
    info->notificationHandle = notificationHandle;
}

void
eventBufferSetEnable(EVENT_BUFFER_PRODUCER_INFO *info, NvBool isEnabled)
{
    info->isEnabled = isEnabled;
}

void
eventBufferSetKeepNewest(EVENT_BUFFER_PRODUCER_INFO *info,NvBool isKeepNewest)
{
    info->isKeepNewest = isKeepNewest;
}

void
eventBufferSetMaintainRecordCount(EVENT_BUFFER_PRODUCER_INFO *info, NvBool bMaintain)
{
    info->bMaintainRecordCount = bMaintain;
}

void
eventBufferUpdateRecordBufferGet(EVENT_BUFFER_PRODUCER_INFO *info, NvU32 get)
{
    RECORD_BUFFER_INFO* pRecordBuffer = &info->recordBuffer;
    pRecordBuffer->pHeader->recordGet = get;

    // used for notification
    _eventBufferUpdateRecordBufferCount(info);

    // dropCounts get reset on every updateGet call
    pRecordBuffer->pHeader->recordDropcount = 0;
    pRecordBuffer->pHeader->vardataDropcount = 0;

}

void
_eventBufferUpdateRecordBufferCount(EVENT_BUFFER_PRODUCER_INFO *info)
{
    RECORD_BUFFER_INFO* pRecordBuffer = &info->recordBuffer;
    NV_EVENT_BUFFER_HEADER* pHeader = info->recordBuffer.pHeader;

    if (pHeader->recordGet <= pHeader->recordPut)
        pHeader->recordCount = (pHeader->recordPut - pHeader->recordGet);
    else
        pHeader->recordCount = pHeader->recordPut + (pRecordBuffer->totalRecordCount - pHeader->recordGet);
}

void
eventBufferUpdateVardataBufferGet(EVENT_BUFFER_PRODUCER_INFO *info, NvU32 get)
{
    VARDATA_BUFFER_INFO* pVardataBuffer = &info->vardataBuffer;
    pVardataBuffer->get = get;

    _eventBufferUpdateVarRemaingSize(info);
}

NvU32
eventBufferGetRecordBufferCount(EVENT_BUFFER_PRODUCER_INFO *info)
{
    return info->recordBuffer.totalRecordCount;
}

NvU32
eventBufferGetVardataBufferCount(EVENT_BUFFER_PRODUCER_INFO *info)
{
    return info->vardataBuffer.bufferSize;
}

//
// Free record slots under keep-oldest ring semantics. One slot is reserved
// to distinguish full from empty.
//
static NvU32
eventBufferUsableFreeSlots(EVENT_BUFFER_PRODUCER_INFO *info)
{
    RECORD_BUFFER_INFO     *pRecInfo = &info->recordBuffer;
    NV_EVENT_BUFFER_HEADER *pHeader  = pRecInfo->pHeader;
    NvU32                   total    = pRecInfo->totalRecordCount;
    NvU32                   used;

    if (total == 0)
        return 0;

    used = (pHeader->recordPut + total - pHeader->recordGet) % total;
    return (total - 1) - used;
}

//
// Preflight predicate for the no-drop publish path; mirrors
// _eventBufferAddVardata keep-oldest skip logic.
//
static NvBool
_eventBufferVardataWouldFit(EVENT_BUFFER_PRODUCER_INFO *info, NvU32 size)
{
    VARDATA_BUFFER_INFO *pVarInfo = &info->vardataBuffer;
    NvU32                alignedSize;
    NvU32                vardataOffsetEnd;

    if (size == 0)
        return NV_TRUE;

    alignedSize      = NV_ALIGN_UP(size, NV_EVENT_VARDATA_GRANULARITY);
    vardataOffsetEnd = pVarInfo->put + alignedSize;

    if (vardataOffsetEnd <= pVarInfo->bufferSize)
    {
        // No wrap.
        return (pVarInfo->remainingSize >= alignedSize);
    }
    else
    {
        // Treat the end sentinel as 0 for post-wrap overlap checks.
        NvU32 effectiveGet = (pVarInfo->get >= pVarInfo->bufferSize)
                             ? 0U
                             : pVarInfo->get;
        return (effectiveGet > alignedSize);
    }
}

//
// eventBufferProducerAddEvent
//
// Adds an event to an event buffer
// This function is called after acquiring correct locks (depending on which module includes it)
// and bound checks for input parameters
// eventType : for RM this would be either 2080 subdevice events or 0000 system events
// eventSubtype: optional
// payloadSize and vardataSize must be 64 bit aligned
//
void
eventBufferProducerAddEvent
(
    EVENT_BUFFER_PRODUCER_INFO *info,
    NvU16 eventType,
    NvU16 eventSubtype,
    EVENT_BUFFER_PRODUCER_DATA *pData
)
{
    NV_EVENT_BUFFER_RECORD *record;

    if (info->isEnabled)
    {
        record = _eventBufferGetFreeRecord(info);
        if (record)
        {
            RECORD_BUFFER_INFO *pRecInfo = &info->recordBuffer;
            NV_EVENT_BUFFER_HEADER *pHeader = pRecInfo->pHeader;
            NvU32 putNext = (pHeader->recordPut + 1) % pRecInfo->totalRecordCount;

            record->recordHeader.type = eventType;
            record->recordHeader.subtype = eventSubtype;

            if (pData->payloadSize)
                 portMemCopy(record->inlinePayload, pData->payloadSize,
                             NvP64_VALUE(pData->pPayload), pData->payloadSize);

            _eventBufferAddVardata(info, pData->pVardata, pData->vardataSize, &record->recordHeader);

            //
            // Release-fence the record payload + vardata stores before
            // publishing the recordPut advance, so a consumer that
            // observes the new recordPut via an acquire load is
            // guaranteed to see the full record contents.
            //
            portAtomicMemoryFenceStore();
            pHeader->recordPut = putNext;
        }
    }
}

//
// eventBufferProducerTryAddEvent
//
// No-drop variant of eventBufferProducerAddEvent. Preflights ring capacity
// and rejects KEEP_NEWEST because overwrite policy is incompatible with
// capacity checks.
//
NV_STATUS
eventBufferProducerTryAddEvent
(
    EVENT_BUFFER_PRODUCER_INFO *info,
    NvU16 eventType,
    NvU16 eventSubtype,
    EVENT_BUFFER_PRODUCER_DATA *pData
)
{
    RECORD_BUFFER_INFO     *pRecInfo;
    NV_EVENT_BUFFER_HEADER *pHeader;
    NV_EVENT_BUFFER_RECORD *record;
    NvU32                   recordOffset;
    NvU32                   putNext;

    NV_ASSERT_OR_RETURN(!info->isKeepNewest, NV_ERR_INVALID_STATE);

    if (!info->isEnabled)
        return NV_WARN_NOTHING_TO_DO;

    pRecInfo = &info->recordBuffer;
    pHeader  = pRecInfo->pHeader;

    // Preflight: record ring must have room (no overwrite).
    if (eventBufferUsableFreeSlots(info) == 0)
        return NV_ERR_INSUFFICIENT_RESOURCES;

    // Preflight: vardata ring must have room (no overwrite).
    if (!_eventBufferVardataWouldFit(info, pData->vardataSize))
        return NV_ERR_INSUFFICIENT_RESOURCES;

    // Commit after both rings pass preflight.
    recordOffset = pHeader->recordPut * pRecInfo->recordSize;
    record       = (NV_EVENT_BUFFER_RECORD *)((NvUPtr)pRecInfo->recordBuffAddr + recordOffset);
    putNext      = (pHeader->recordPut + 1) % pRecInfo->totalRecordCount;

    record->recordHeader.type    = eventType;
    record->recordHeader.subtype = eventSubtype;

    if (pData->payloadSize)
        portMemCopy(record->inlinePayload, pData->payloadSize,
                    NvP64_VALUE(pData->pPayload), pData->payloadSize);

    _eventBufferAddVardata(info, pData->pVardata, pData->vardataSize, &record->recordHeader);

    portAtomicMemoryFenceStore();
    pHeader->recordPut = putNext;

    if (info->bMaintainRecordCount)
        _eventBufferUpdateRecordBufferCount(info);

    return NV_OK;
}

NV_EVENT_BUFFER_RECORD *
_eventBufferGetFreeRecord(EVENT_BUFFER_PRODUCER_INFO *info)
{
    RECORD_BUFFER_INFO *pRecInfo = &info->recordBuffer;
    NV_EVENT_BUFFER_HEADER *pHeader = pRecInfo->pHeader;
    NvU32 recordOffset = 0;
    NV_EVENT_BUFFER_RECORD *pFreeRecord = NULL;

    NvU32 putNext = (pHeader->recordPut + 1) % pRecInfo->totalRecordCount;

    if ((!info->isKeepNewest) && (putNext == pHeader->recordGet))
    {
        pHeader->recordDropcount++;
    }
    else
    {
        recordOffset = pHeader->recordPut * pRecInfo->recordSize;
        pFreeRecord = (NV_EVENT_BUFFER_RECORD *)((NvUPtr)pRecInfo->recordBuffAddr + recordOffset);
    }
    return pFreeRecord;
}

void
_eventBufferAddVardata
(
    EVENT_BUFFER_PRODUCER_INFO *info,
    NvP64 data,
    NvU32 size,
    NV_EVENT_BUFFER_RECORD_HEADER* recordHeader
)
{
    VARDATA_BUFFER_INFO *pVarInfo = &info->vardataBuffer;
    NV_EVENT_BUFFER_HEADER* pHeader = info->recordBuffer.pHeader;
    NvU32 pVardataOffset;
    NvU32 alignedSize = NV_ALIGN_UP(size, NV_EVENT_VARDATA_GRANULARITY);
    NvU32 vardataOffsetEnd = pVarInfo->put + alignedSize;

    if (vardataOffsetEnd <= pVarInfo->bufferSize)
    {
        if ((!info->isKeepNewest) && (pVarInfo->remainingSize < alignedSize))
            goto skip;

        pVardataOffset = pVarInfo->put;
        recordHeader->varData = vardataOffsetEnd;
    }
    else
    {
        // Treat the end sentinel as 0 for post-wrap overlap checks.
        NvU32 effectiveGet;
        vardataOffsetEnd = 0 + alignedSize;
        effectiveGet     = (pVarInfo->get >= pVarInfo->bufferSize)
                           ? 0U
                           : pVarInfo->get;
        if ((!info->isKeepNewest) && (effectiveGet <= vardataOffsetEnd))
            goto skip;

        recordHeader->varData = vardataOffsetEnd | NV_EVENT_VARDATA_START_OFFSET_ZERO;
        pVardataOffset = 0;
    }

    if(size)
    {
         portMemCopy((void*)((NvUPtr)pVarInfo->vardataBuffAddr + pVardataOffset), size, NvP64_VALUE(data), size);

        if (alignedSize != size)
        {
            pVardataOffset += size;
            portMemSet((void*)((NvUPtr)pVarInfo->vardataBuffAddr + pVardataOffset), 0, (alignedSize - size));
        }
    }

    pVarInfo->put = vardataOffsetEnd;
    _eventBufferUpdateVarRemaingSize(info);
    return;

skip:
    recordHeader->varData = pVarInfo->put;
    pHeader->vardataDropcount += 1;
}

void
_eventBufferUpdateVarRemaingSize(EVENT_BUFFER_PRODUCER_INFO* info)
{
    VARDATA_BUFFER_INFO *pVarInfo = &info->vardataBuffer;

    if (!info->isKeepNewest)
    {
        if (pVarInfo->get <= pVarInfo->put)
            pVarInfo->remainingSize = pVarInfo->get + (pVarInfo->bufferSize - pVarInfo->put);
        else
            pVarInfo->remainingSize = pVarInfo->get - pVarInfo->put;
    }
}

NvBool
eventBufferIsNotifyThresholdMet(EVENT_BUFFER_PRODUCER_INFO* info)
{
    VARDATA_BUFFER_INFO *pVarInfo = &info->vardataBuffer;
    RECORD_BUFFER_INFO* pRecInfo = &info->recordBuffer;
    NV_EVENT_BUFFER_HEADER* pHeader = pRecInfo->pHeader;
    NvBool recordsThresholdMet;

    if (info->bMaintainRecordCount)
    {
        // Live ring math for callers that notify between consumer drains.
        recordsThresholdMet =
            (eventBufferUsableFreeSlots(info) <= pRecInfo->notificationThreshold);
    }
    else
    {
        //
        // Existing semantics: relies on recordCount being maintained
        // out-of-band by the caller (or zero-by-design for callers that
        // never publish through eventBufferProducerAddEvent). Preserved
        // for FECS / video / NOCAT / RATS / OpEventLog / vGPU FECS
        // staging.
        //
        recordsThresholdMet =
            ((pRecInfo->totalRecordCount - pHeader->recordCount) <= pRecInfo->notificationThreshold);
    }

    if (recordsThresholdMet || (pVarInfo->remainingSize <= pVarInfo->notificationThreshold))
    {
        return NV_TRUE;
    }
    return NV_FALSE;
}
