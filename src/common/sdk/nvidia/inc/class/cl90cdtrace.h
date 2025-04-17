/*
 * Copyright (c) 2023, NVIDIA CORPORATION. All rights reserved.
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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef _cl90cdtrace_h_
#define _cl90cdtrace_h_

#include <nvtypes.h>

/* This file defines parameters for RATS/RM tracing */

typedef struct NV_RATS_GSP_TRACE_RECORD_V1
{
    NvU16 seqNo;
    NvU16 gspSeqNo;
    NvU32 threadId;
    NvU64 info;
    NvU64 timeStamp;
    NvU64 recordType;
} NV_RATS_GSP_TRACE_RECORD_V1;

typedef NV_RATS_GSP_TRACE_RECORD_V1 NV_RATS_GSP_TRACE_RECORD;

#define VGPU_TRACING_BUFFER_KEEP_OLDEST 0
#define VGPU_TRACING_BUFFER_KEEP_NEWEST 1

typedef struct NV_RATS_VGPU_GSP_TRACING_BUFFER_V1{
    NvU8 policy;

    NvBool bGuestNotifInProgress;

    NvU16 seqNo;
    NvU32 bufferSize;
    NvU32 bufferWatermark;
    NvU32 recordCount;

    NvU64 tracepointMask;
    NvU32 read;
    NvU32 write;

    NvU64 lastReadTimestamp;

    NV_RATS_GSP_TRACE_RECORD *buffer;
} NV_RATS_VGPU_GSP_TRACING_BUFFER_V1;

typedef NV_RATS_VGPU_GSP_TRACING_BUFFER_V1 NV_RATS_VGPU_GSP_TRACING_BUFFER;

#endif
