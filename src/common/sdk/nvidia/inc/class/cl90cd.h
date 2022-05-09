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

#ifndef _cl90cd_h_
#define _cl90cd_h_

#ifdef __cplusplus
extern "C" {
#endif

/*
* NV_EVENT_BUFFER
*   An event buffer is shared between user (RO) and kernel(RW).
*   It holds debug/profile event data provided by the kernel.
*
*/
#define NV_EVENT_BUFFER                             (0x000090CD)

/*
* NV_EVENT_BUFFER_HEADER
*   This structure holds the get and put values used to index/consume event buffer.
*   Along with other RO data shared with the user.
*
*   recordGet/Put: These "pointers" work in the traditional sense:
*   -   when GET==PUT, the fifo is empty
*   -   when GET==PUT+1, the fifo is full
*   This implies a full fifo always has one "wasted" element.
*
*   recordCount: This is the total number of records added to the buffer by the kernel
*       This information is filled out when the buffer is setup to keep newest records.
*       recordCount = number of records currently in the buffer + overflow count.
*
*   recordDropcount: This is the number of event records that are dropped because the
*       buffer is full.
*       This information is filled out when event buffer is setup to keep oldest records.
*
*   vardataDropcount: Event buffer provides a dual stream of data, where the record can contain 
*       an optional offset to a variable length data buffer.
*       This is the number of variable data records that are dropped because the
*       buffer is full.
*       This information is filled out when event buffer is setup to keep oldest records.
*/
typedef struct
{
    NvU32   recordGet;
    NvU32   recordPut;
    NvU64   recordCount;
    NvU64   recordDropcount;
    NvU64   vardataDropcount;
} NV_EVENT_BUFFER_HEADER;

/*
* NV_EVENT_BUFFER_RECORD_HEADER
*   This is the header added to each event record.
*   This helps identify the event type and variable length data is associated with it.
*/
typedef struct
{
    NvU16   type;
    NvU16   subtype;
    NvU32   varData;            // [31: 5] = (varDataOffset >> 5);    0 < vardataOffset <= vardataBufferSize
                                // [ 4: 1] = reserved for future use
                                // [ 0: 0] = isVardataStartOffsetZero
} NV_EVENT_BUFFER_RECORD_HEADER;

/*
* NV_EVENT_BUFFER_RECORD
*   This structure defines a generic event record.
*   The size of this record is fixed for a given event buffer.
*   It is configured by the user during allocation.
*/
typedef struct
{
    NV_EVENT_BUFFER_RECORD_HEADER recordHeader;
    NvU64 inlinePayload[1] NV_ALIGN_BYTES(8); // 1st element of the payload/data
    // Do not add more elements here, inlinePayload can contain more than one elements
} NV_EVENT_BUFFER_RECORD;

#define NV_EVENT_VARDATA_GRANULARITY              32
#define NV_EVENT_VARDATA_OFFSET_MASK              (~(NV_EVENT_VARDATA_GRANULARITY - 1))
#define NV_EVENT_VARDATA_START_OFFSET_ZERO        0x01

/*
* NV_EVENT_BUFFER_ALLOC_PARAMETERS
*
*   bufferHeader [OUT]
*       This is the user VA offset pointing to the base of NV_EVENT_BUFFER_HEADER.
*
*   recordBuffer [OUT]
*       This is the user VA offset pointing to the base of the event record buffer.
*       This buffer will contain NV_EVENT_BUFFER_RECORDs added by the kernel.
*
*   recordSize [IN]
*       This is the size of NV_EVENT_BUFFER_RECORD used by this buffer
*
*   recordCount [IN]
*       This is the number of records that recordBuffer can hold.
*
*   vardataBuffer [OUT]
*       This is the user VA offset pointing to the base of the variable data buffer.
*
*   vardataBufferSize [IN]
*       Size of the variable data buffer in bytes.
*
*   recordsFreeThreshold [IN]
*       This is the notification threshold for the event record buffer.
*       This felid specifies the number of records that the buffer can
*       still hold before it gets full.
*
*   vardataFreeThreshold [IN]
*       This is the notification threshold for the vardata buffer.
*       This felid specifies the number of bytes that the buffer can
*       still hold before it gets full.
*
*   notificationHandle [IN]
*       When recordsFreeThreshold or vardataFreeThreshold is met, kernel will notify
*       user on this handle. If notificationHandle = NULL, event notification
*       is disabled. This is an OS specific notification handle.
*       It is a Windows event handle or a fd pointer on Linux.
*
*   hSubDevice [IN]
*       An event buffer can either hold sub-device related events or system events.
*       This handle specifies the sub-device to associate this buffer with.
*       If this parameter is NULL, then the buffer is tied to the client instead.
*
*   flags [IN]
*       Set to 0 by default.
*       This field can hold any future flags to configure the buffer if needed.
*
*   hBufferHeader [IN]
*       The backing memory object for the buffer header. Must be a NV01_MEMORY_DEVICELESS object.
*       On Windows platforms, a buffer will be internally generated if hBufferHeader is 0.
*
*   hRecordBuffer [IN]
*       The backing memory object for the record buffer. Must be a NV01_MEMORY_DEVICELESS object.
*       On Windows platforms, a buffer will be internally generated if hRecordBuffer is 0.
*
*   hVardataBuffer [IN]
*       The backing memory object for the vardata buffer. Must be a NV01_MEMORY_DEVICELESS object.
*       On Windows platforms, a buffer will be internally generated if hVardataBuffer is 0.
*
*/
typedef struct
{
    NvP64   bufferHeader NV_ALIGN_BYTES(8);
    NvP64   recordBuffer NV_ALIGN_BYTES(8);
    NvU32   recordSize;
    NvU32   recordCount;
    NvP64   vardataBuffer NV_ALIGN_BYTES(8);
    NvU32   vardataBufferSize;
    NvU32   recordsFreeThreshold;
    NvU64   notificationHandle NV_ALIGN_BYTES(8);
    NvU32   vardataFreeThreshold;
    NvHandle hSubDevice;
    NvU32   flags;

    NvHandle hBufferHeader;
    NvHandle hRecordBuffer;
    NvHandle hVardataBuffer;
} NV_EVENT_BUFFER_ALLOC_PARAMETERS;

/*
* NV_EVENT_BUFFER_BIND
*   This class is used to allocate an Event Type object bound to a given event buffer.
*   This allocation call associates an event type with an event buffer.
*   Multiple event types can be associated with the same buffer as long as they belong to
*   the same category i.e. either sub-device or system.
*   When event buffer is enabled, if an event bound to this buffer occurs,
*   some relevant data gets added to it.
*   cl2080.h has a list of sub-device events that can be associated with a buffer
*   cl0000.h has a list of system events that can be associated with a buffer
*   These defines are also used in class NV01_EVENT_OS_EVENT (0x79) to get event notification
*   and class NV01_EVENT_KERNEL_CALLBACK_EX (0x7E) to get kernel callbacks.
*   This class extends that support to additionally get relevant data in an event buffer
*
*/
#define NV_EVENT_BUFFER_BIND                       (0x0000007F)

/*
* NV_EVENT_BUFFER_BIND_PARAMETERS
*
*   bufferHandle [IN]
*       Event buffer handle used to bind the given event type
*
*   eventType [IN]
*       This is one of the eventTypeIDs from cl2080.h/cl000.h
*       e.g. NV2080_NOTIFIERS_PSTATE_CHANGE
*
*   eventSubtype [IN]
*       Event subtype for a given type of event.
*       This field is optional depending on if an eventtype has a subtype.
*
*   hClientTarget [IN]
*       Handle of the target client whose events are to be bound to the given buffer
*       e.g. context switch events can be tracked for a given client.
*       This field is optional depending on the event type.
*       e.g. pstate change events are per gpu but do not depend on a client.
*
*   hSrcResource [IN]
*       source resource handle for the event type
*       e.g. channel handle: RC/context switch can be tracked for a given channel
*       This field is optional depending on the event type.
*       e.g. pstate change events are per gpu and cannot be sub-categorized
*
*   KernelCallbackdata [IN]
*       This field is reserved for KERNEL ONLY clients.
*
*/
typedef struct
{
    NvHandle bufferHandle;
    NvU16 eventType;
    NvU16 eventSubtype;
    NvHandle hClientTarget;
    NvHandle hSrcResource;
    NvP64 KernelCallbackdata NV_ALIGN_BYTES(8);
} NV_EVENT_BUFFER_BIND_PARAMETERS;

#ifdef __cplusplus
};     /* extern "C" */
#endif

#endif // _cl90cd_h_

