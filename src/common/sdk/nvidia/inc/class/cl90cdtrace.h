/*
 * Copyright (c) 2023-2026, NVIDIA CORPORATION. All rights reserved.
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

typedef enum NV_RATS_TARGET_ID
{
    NV_RATS_TARGET_BUFFER_TASK_RM,
    NV_RATS_TARGET_BUFFER_TASK_INTERRUPT,
    NV_RATS_TARGET_BUFFER_TASK_VGPU,
    NV_RATS_TARGET_BUFFER_LIBOS,

    NV_RATS_TARGET_ID_SIZE
} NV_RATS_TARGET_ID;

typedef enum NV_RATS_RECORD_CLASS {
    NV_RATS_RECORD_CLASS_GSP_RM_TRACE,
    NV_RATS_RECORD_CLASS_LIBOS_TRACE,
    NV_RATS_RECORD_CLASS_GSP_RM_BACKTRACE,

    NV_RATS_RECORD_CLASS_SIZE
} NV_RATS_RECORD_CLASS;

#define NV_RATS_MAX_BACKTRACES_PER_RECORD 4

typedef struct NV_RATS_RECORD_V2 {
    NV_RATS_RECORD_CLASS recordClass;
    NvU16 seqNo;
    NvU16 gspSeqNo;
    union {
        struct {
            NvU8 traceType;
            NvU8 threadId;
            NvU8 partitionId;
            NvU8 reserved;
            NvU32 info32;
            NvU64 info64;
            NvU64 timeStamp;
        } gspRmTrace;
        struct {
            NvU8 traceType;
            NvU8 threadId;
            NvU8 partitionId;
            NvU8 reserved8;
            NvU16 libosSeqNo;
            NvU16 reserved16;
            NvU64 info;
            NvU64 timeStamp;
        } libosTrace;
        struct {
            NvU16 packedBacktrace[NV_RATS_MAX_BACKTRACES_PER_RECORD * 3]; // 12 NvU16 * 2 bytes = 4 address * 6 byte addresses = 24 bytes
        } gspRmBacktrace;
    };
} NV_RATS_RECORD_V2;

// 8 byte header + 24 byte union
#define NV_RATS_RECORD_SIZE_V2 32

#define NV_RATS_RECORD NV_RATS_RECORD_V2

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

    NV_RATS_RECORD *buffer;
} NV_RATS_VGPU_GSP_TRACING_BUFFER_V1;

typedef NV_RATS_VGPU_GSP_TRACING_BUFFER_V1 NV_RATS_VGPU_GSP_TRACING_BUFFER;

#endif
