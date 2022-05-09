/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2017 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#ifndef _NV_EVENT_BUFFER_PRODUCER_H_
#define _NV_EVENT_BUFFER_PRODUCER_H_
#include "nvtypes.h"
#include "class/cl90cd.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
* RECORD_BUFFER_INFO
*   This structure holds information for the record buffer, which is a circular
*   buffer with fixed sized records
*
*   pHeader
*       This is a shared header between the producer and consumer.
*       It holds the get put pointers and overflow counts.
*
*   recordBuffAddr
*       This is the producer mapping to the record buffer.
*
*   recordSize
*       Size of each record in bytes.
*
*   totalRecordCount
*       Total number of records that this buffer can hold.
*
*   bufferSize
*       Total size of record buffer in bytes.
*
*   notificationThreshold
*       This felid specifies the number of records that the buffer can
*       still hold before it gets full.
*       Consumer is notified when this threshold is met.
*
*/
typedef struct
{
    NV_EVENT_BUFFER_HEADER* pHeader;
    NvP64                   recordBuffAddr;
    NvU32                   recordSize;
    NvU32                   totalRecordCount;
    NvU32                   bufferSize;
    NvU32                   notificationThreshold;
} RECORD_BUFFER_INFO;

/*
*  VARDATA_BUFFER_INFO:
*   This structure holds information for the variable length data buffer,
*   which is a circular buffer with variable sized data records
*
*   vardataBuffAddr
*       This is the producer mapping to the vardata buffer.
*
*   bufferSize
*       Total size of vardata buffer in bytes.
*
*   notificationThreshold
*       This felid specifies the number of records that the buffer can
*       still hold before it gets full.
*       Consumer is notified when this threshold is met.
*
*   get\put
*       These are the get and put offsets for vardata buffer.
*       These are not shared with the consumer.
*
*   remainingSize
*       Size in bytes remaining in the vardata buffer.
*/
typedef struct
{
    NvP64                   vardataBuffAddr;
    NvU32                   bufferSize;
    NvU32                   notificationThreshold;
    NvU32                   get;
    NvU32                   put;
    NvU32                   remainingSize;
} VARDATA_BUFFER_INFO;

/*
*  EVENT_BUFFER_PRODUCER_INFO:
*
*   recordBuffer
*       Record buffer information
*
*   vardataBuffer
*       Vardata buffer information
*
*   notificationHandle
*       notification handle used to notify the consumer
*
*   isEnabled
*       Data is added to the event buffer only if this flag is set
*       Controlled by Consumer.
*
*   isKeepNewest
*       This flag is set if keepNewest mode is selected by the consumer.
*/
typedef struct
{
    RECORD_BUFFER_INFO      recordBuffer;
    VARDATA_BUFFER_INFO     vardataBuffer;
    NvP64                   notificationHandle;
    NvBool                  isEnabled;
    NvBool                  isKeepNewest;
} EVENT_BUFFER_PRODUCER_INFO;

/*
*  EVENT_BUFFER_PRODUCER_DATA:
*   This structure holds data info to add a record in a buffer
*
*   pPayload
*       Pointer to the payload that needs to be added in the record buffer
*
*   payloadSize
*       Size of payload in bytes.
*
*   pVardata
*       Pointer to data that needs to be added in the vardata buffer
*
*   vardataSize
*       Size of vardata in bytes.
*/
typedef struct
{
    NvP64                   pPayload;
    NvU32                   payloadSize;
    NvP64                   pVardata;
    NvU32                   vardataSize;
} EVENT_BUFFER_PRODUCER_DATA;

void eventBufferInitRecordBuffer(EVENT_BUFFER_PRODUCER_INFO *info, NV_EVENT_BUFFER_HEADER* pHeader,
    NvP64 recordBuffAddr, NvU32 recordSize, NvU32 recordCount, NvU32 bufferSize, NvU32 notificationThreshold);

void eventBufferInitVardataBuffer(EVENT_BUFFER_PRODUCER_INFO *info, NvP64 vardataBuffAddr,
    NvU32 bufferSize, NvU32 notificationThreshold);

void eventBufferInitNotificationHandle(EVENT_BUFFER_PRODUCER_INFO *info, NvP64 notificationHandle);
void eventBufferSetEnable(EVENT_BUFFER_PRODUCER_INFO *info, NvBool isEnabled);
void eventBufferSetKeepNewest(EVENT_BUFFER_PRODUCER_INFO *info, NvBool isKeepNewest);
void eventBufferUpdateRecordBufferGet(EVENT_BUFFER_PRODUCER_INFO *info, NvU32 get);
void eventBufferUpdateVardataBufferGet(EVENT_BUFFER_PRODUCER_INFO *info, NvU32 get);
NvU32 eventBufferGetRecordBufferCount(EVENT_BUFFER_PRODUCER_INFO *info);
NvU32 eventBufferGetVardataBufferCount(EVENT_BUFFER_PRODUCER_INFO *info);

void eventBufferProducerAddEvent(EVENT_BUFFER_PRODUCER_INFO* info, NvU16 eventType, NvU16 eventSubtype,
    EVENT_BUFFER_PRODUCER_DATA *pData);

NvBool eventBufferIsNotifyThresholdMet(EVENT_BUFFER_PRODUCER_INFO* info);

#ifdef __cplusplus
};     /* extern "C" */
#endif

#endif //_NV_EVENT_BUFFER_PRODUCER_H_
