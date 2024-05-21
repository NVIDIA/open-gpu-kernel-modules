/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef KERNEL_RATS_GSP_TRACE_H
#define KERNEL_RATS_GSP_TRACE_H

#include "core/core.h"
#include "containers/multimap.h"
#include "class/cl90cdtrace.h"
#include "rmapi/event_buffer.h"

typedef struct
{
    EventBuffer *pEventBuffer;
    NvHandle     hClient;
    NvHandle     hNotifier;
    NvHandle     hEventBuffer;
    NvU64        pUserInfo;
} NV_EVENT_BUFFER_BIND_POINT_GSP_TRACE;

MAKE_MULTIMAP(GspTraceEventBufferBindMultiMap, NV_EVENT_BUFFER_BIND_POINT_GSP_TRACE);

void gspTraceNotifyAllConsumers(OBJGPU *pGpu, void *pArgs);

void gspTraceEventBufferLogRecord(OBJGPU *pGpu, NV_RATS_GSP_TRACE_RECORD *intrTraceRecord);

NV_STATUS gspTraceAddBindpoint(OBJGPU *pGpu,
                               RsClient *pClient,
                               RsResourceRef *pEventBufferRef,
                               NvHandle hNotifier,
                               NvU64 tracepointMask,
                               NvU32 gspLoggingBufferSize,
                               NvU32 gspLoggingBufferWatermark);

void gspTraceRemoveBindpoint(OBJGPU *pGpu, NvU64 uid, NV_EVENT_BUFFER_BIND_POINT_GSP_TRACE* pBind);

void gspTraceRemoveAllBindpoints(EventBuffer *pEventBuffer);

#endif
