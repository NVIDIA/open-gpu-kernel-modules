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

#ifndef _GSP_OOM_REPORT_H_
#define _GSP_OOM_REPORT_H_

#include "nvtypes.h"
#include "nvmisc.h"

//
// Host-allocated sysmem buffer used by one GSP-RM instance to publish its first
// OOM report.  The writer claims state, fills the payload, fences, then marks
// it valid.  The host snapshots the buffer at NvDump time without invoking GSP.
//

#define GSP_OOM_REPORT_BUFFER_SIZE    (16U * 1024U)  // 16 KB
#define GSP_OOM_REPORT_VERSION        1U
#define GSP_OOM_REPORT_MAGIC          0xC0FFEE0000007903ULL
#define GSP_OOM_REPORT_INIT_ARG_ID    "OOMRPT"

#define GSP_OOM_REPORT_STATE_EMPTY    0U
#define GSP_OOM_REPORT_STATE_WRITING  1U
#define GSP_OOM_REPORT_STATE_VALID    2U

#define GSP_OOM_REPORT_MAX_LIBOS_POOLS   32U
#define GSP_OOM_REPORT_MAX_CLASS_ENTRIES 128U
#define GSP_OOM_REPORT_MAX_PID_ENTRIES   64U

typedef struct GSP_OOM_REPORT_ALLOCATOR_STATS
{
    NvU32  numAllocations;
    NvU32  reserved;
    NvU64  allocatedSize;
    NvU64  usefulSize;
    NvU64  metaSize;
} GSP_OOM_REPORT_ALLOCATOR_STATS;

typedef struct GSP_OOM_REPORT_MALLOC_HEAP_STATS
{
    NvU64                          heapSize;
    NvU64                          freeSize;
    NvU64                          largestFreeChunk;
    NvU32                          fragPct;
    NvU32                          reserved;
    GSP_OOM_REPORT_ALLOCATOR_STATS active;
    GSP_OOM_REPORT_ALLOCATOR_STATS total;
    GSP_OOM_REPORT_ALLOCATOR_STATS peak;
} GSP_OOM_REPORT_MALLOC_HEAP_STATS;

typedef struct GSP_OOM_REPORT_LIBOS_POOL
{
    NvU32  allocations;
    NvU32  peakAllocations;
    NvU64  objectSize;
} GSP_OOM_REPORT_LIBOS_POOL;

typedef struct GSP_OOM_REPORT_LIBOS_HEAP_STATS
{
    NvU64                      totalHeapSize;
    NvU32                      poolCount;
    NvU32                      reserved;
    GSP_OOM_REPORT_LIBOS_POOL  pools[GSP_OOM_REPORT_MAX_LIBOS_POOLS];
} GSP_OOM_REPORT_LIBOS_HEAP_STATS;

typedef struct GSP_OOM_REPORT_CLASS_ENTRY
{
    NvU32  classId;
    NvU32  count;
} GSP_OOM_REPORT_CLASS_ENTRY;

typedef struct GSP_OOM_REPORT_PID_ENTRY
{
    NvU32  pid;
    NvU32  clients;
    NvU32  resources;
    NvU32  reserved;
} GSP_OOM_REPORT_PID_ENTRY;

typedef struct GSP_OOM_REPORT_RESSERV_STATS
{
    NvU32                       numClients;
    NvU32                       classCount;
    NvU64                       numResources;
    NvU32                       pidCount;
    NvU32                       reserved;
    GSP_OOM_REPORT_CLASS_ENTRY  classHist[GSP_OOM_REPORT_MAX_CLASS_ENTRIES];
    GSP_OOM_REPORT_PID_ENTRY    pidSummary[GSP_OOM_REPORT_MAX_PID_ENTRIES];
} GSP_OOM_REPORT_RESSERV_STATS;

//
// The payload is from this GSP-RM instance's first OOM.
//
typedef struct GSP_OOM_REPORT_SHARED
{
    NvU64  magic;
    NvU32  version;
    NvU32  oomCount;
    NvU32  state;
    NvU32  reserved0;

    NvU64  requestedSize;
    GSP_OOM_REPORT_MALLOC_HEAP_STATS  mallocHeap;
    GSP_OOM_REPORT_LIBOS_HEAP_STATS   libosHeap;
    GSP_OOM_REPORT_RESSERV_STATS      resservStats;
} GSP_OOM_REPORT_SHARED;

ct_assert(sizeof(GSP_OOM_REPORT_SHARED) <= GSP_OOM_REPORT_BUFFER_SIZE);

#endif // _GSP_OOM_REPORT_H_
