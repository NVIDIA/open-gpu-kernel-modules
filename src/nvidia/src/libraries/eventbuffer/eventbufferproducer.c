/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

            pHeader->recordPut = putNext;
        }
    }
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
        // wrap-around; the effective vardataPut=0, vardataOffsetEnd=size
        vardataOffsetEnd = 0 + alignedSize;
        if ((!info->isKeepNewest) && (pVarInfo->get <= vardataOffsetEnd))
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

    if (((pRecInfo->totalRecordCount - pHeader->recordCount) <= pRecInfo->notificationThreshold) ||
        (pVarInfo->remainingSize <= pVarInfo->notificationThreshold))
    {
        return NV_TRUE;
    }
    return NV_FALSE;
}
