#ifndef _G_FECS_EVENT_LIST_NVOC_H_
#define _G_FECS_EVENT_LIST_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2013-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_fecs_event_list_nvoc.h"

#ifndef _FECS_EVENT_LIST_H_
#define _FECS_EVENT_LIST_H_

/*!
 * @file   fecs_event_list.h
 * @brief  Provides definition for FECS callback on EventBuffer, as well as a list holding the subscribers to the event
 */

#include "core/core.h"
#include "class/cl90cd.h"
#include "class/cl90cdfecs.h"
#include "containers/multimap.h"
#include "resserv/resserv.h"
#include "rmapi/client.h"

#include "ctrl/ctrl2080/ctrl2080gr.h" // NV2080_CTRL_GR_FECS_BIND_EVTBUF_LOD

struct KernelGraphics;

#ifndef __NVOC_CLASS_KernelGraphics_TYPEDEF__
#define __NVOC_CLASS_KernelGraphics_TYPEDEF__
typedef struct KernelGraphics KernelGraphics;
#endif /* __NVOC_CLASS_KernelGraphics_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelGraphics
#define __nvoc_class_id_KernelGraphics 0xea3fa9
#endif /* __nvoc_class_id_KernelGraphics */


struct EventBuffer;

#ifndef __NVOC_CLASS_EventBuffer_TYPEDEF__
#define __NVOC_CLASS_EventBuffer_TYPEDEF__
typedef struct EventBuffer EventBuffer;
#endif /* __NVOC_CLASS_EventBuffer_TYPEDEF__ */

#ifndef __nvoc_class_id_EventBuffer
#define __nvoc_class_id_EventBuffer 0x63502b
#endif /* __nvoc_class_id_EventBuffer */



// TODO move to cl90cdfecs.h
#define NV_EVENT_BUFFER_INVALID_MIG_GI      0xFF
#define NV_EVENT_BUFFER_INVALID_MIG_CI      0xFF
#define NV_EVENT_BUFFER_HIDDEN_MIG_GI       0xFE
#define NV_EVENT_BUFFER_HIDDEN_MIG_CI       0xFE
#define NV_EVENT_BUFFER_KERNEL_MIG_GI       0xFD
#define NV_EVENT_BUFFER_KERNEL_MIG_CI       0xFD

typedef struct
{
    struct EventBuffer *pEventBuffer;
    NvHandle hClient;
    NvHandle hNotifier;
    NvHandle hEventBuffer;

    NvU64    pUserInfo;

    NvBool   bAdmin;
    NvBool   bKernel;
    NvU32    eventMask;

    NvU8     version;
    NvU32    swizzId;
    NvU32    computeInstanceId;

} NV_EVENT_BUFFER_BIND_POINT_FECS;
MAKE_MULTIMAP(FecsEventBufferBindMultiMap, NV_EVENT_BUFFER_BIND_POINT_FECS);
MAKE_MAP(EventBufferMap, EventBuffer*);

typedef struct
{
    NvU8    tag;
    NvU64   userInfo;
    NvU32   context_id;
    NvU32   pid;
    NvU32   subpid;
    NvU8    swizzId;
    NvU8    computeInstanceId;
    NvU16   dropCount;
    NvU64   timestamp;
    NvU64   noisyTimestamp;
} FECS_EVENT_NOTIFICATION_DATA;

typedef struct
{
    NV_EVENT_BUFFER_RECORD_HEADER header;
    NV_EVENT_BUFFER_FECS_RECORD_V2 record;
} FECS_EVENT_RECORD_OUTPUT;
ct_assert(NV_OFFSETOF(FECS_EVENT_RECORD_OUTPUT, record) == sizeof(NV_EVENT_BUFFER_RECORD_HEADER));
ct_assert(sizeof(FECS_EVENT_RECORD_OUTPUT) == sizeof(NV_EVENT_BUFFER_RECORD_HEADER) + sizeof(NV_EVENT_BUFFER_FECS_RECORD_V2));


NV_STATUS fecsAddBindpoint
(
    OBJGPU *pGpu,
    struct RmClient *pClient,
    RsResourceRef *pEventBufferRef,
    NvHandle hNotifier,
    NvBool bAllUsers,
    NV2080_CTRL_GR_FECS_BIND_EVTBUF_LOD levelOfDetail,
    NvU32 eventFilter,
    NvU8 version,
    NvU32 *pReasonCode
);

// Set cached internal routing handles and GR index for MIG
void fecsSetRoutingInfo
(
    OBJGPU *pGpu,
    struct KernelGraphics *pKernelGraphics,
    NvHandle hClient,
    NvHandle hSubdevice,
    NvU32 localGrEngineIdx
);

// Clear cached internal routing handles and GR index for MIG
void fecsClearRoutingInfo(OBJGPU *, struct KernelGraphics *);

/*! Opaque FECS event buffer private data */
typedef struct KGRAPHICS_FECS_TRACE_INFO KGRAPHICS_FECS_TRACE_INFO;

NV_STATUS fecsCtxswLoggingInit
(
    OBJGPU *pGpu,
    struct KernelGraphics *pKernelGraphics,
    KGRAPHICS_FECS_TRACE_INFO **ppFecsTraceInfo
);

void fecsCtxswLoggingTeardown(OBJGPU *pGpu, struct KernelGraphics *pKernelGraphics);

/*! set num records to process per intr */
void fecsSetRecordsPerIntr
(
    OBJGPU *pGpu,
    struct KernelGraphics *pKernelGraphics,
    NvU32 recordsPerIntr
);

/**
 * Returns true if a GR's FECS trace buffer has a record ready for processing
 */
NvBool fecsBufferChanged(OBJGPU *pGpu, struct KernelGraphics *pKernelGraphics);

/**
 * Scrub the FECS tracing buffer and enable tracing (if a buffer has been mapped).
 *
 * The ctx logging state in GR will be set to enabled if the operation succeeded.
 */
void fecsBufferReset(OBJGPU *pGpu, struct KernelGraphics *pKernelGraphics);

/*! Is the FECS trace buffer mapped? */
NvBool fecsBufferIsMapped(OBJGPU *pGpu, struct KernelGraphics *pKernelGraphics);

/**
 * Map the FECS trace buffer
 */
NV_STATUS fecsBufferMap(OBJGPU *pGpu, struct KernelGraphics *pKernelGraphics);

/**
 * Unmap the FECS trace buffer
 */
void fecsBufferUnmap(OBJGPU *pGpu, struct KernelGraphics *pKernelGraphics);

/**
 * Disable FECS trace logging, unmap the trace log buffer, and remove any
 * registered FECS trace logging callbacks.
 *
 * The ctx logging state in GR will be set to disabled after this operation.
 */
void fecsBufferTeardown(OBJGPU *pGpu, struct KernelGraphics *pKernelGraphics);

/**
 * Disable FECS logging at hardware level
 */
void fecsBufferDisableHw(OBJGPU *pGpu, struct KernelGraphics *pKernelGraphics);

void fecsRemoveAllBindpoints(struct EventBuffer *pEventBuffer);
void fecsRemoveBindpoint(OBJGPU *pGpu, NvU64 uid, NV_EVENT_BUFFER_BIND_POINT_FECS* pBind);

/* The callback function that transfers FECS Buffer entries to an EventBuffer */
void nvEventBufferFecsCallback(OBJGPU *pGpu, void *pArgs);

void notifyEventBuffers
(
    OBJGPU *pGpu,
    FecsEventBufferBindMultiMapSubmap *pSubmap,
    FECS_EVENT_NOTIFICATION_DATA const *pRecord
);

/*! Atomically set intr callback pending, return NV_TRUE if wasn't pending prior */
NvBool fecsSignalIntrPendingIfNotPending(OBJGPU *pGpu, struct KernelGraphics *pKernelGraphics);

/*! Atomically clear intr callback pending, return NV_TRUE if was pending */
NvBool fecsClearIntrPendingIfPending(OBJGPU *pGpu, struct KernelGraphics *pKernelGraphics);

/*! Atomically check is intr callback pending */
NvBool fecsIsIntrPending(OBJGPU *pGpu, struct KernelGraphics *pKernelGraphics);

/*! Opaque VGPU fecs event buffer private data */
typedef struct VGPU_FECS_TRACE_STAGING_BUFFER VGPU_FECS_TRACE_STAGING_BUFFER;

/*! Retrieve the current VGPU staging buffer */
VGPU_FECS_TRACE_STAGING_BUFFER *fecsGetVgpuStagingBuffer(OBJGPU *pGpu, struct KernelGraphics *pKernelGraphics);

/*! Store the given VGPU staging buffer */
void fecsSetVgpuStagingBuffer
(
    OBJGPU *pGpu,
    struct KernelGraphics *pKernelGraphics,
    VGPU_FECS_TRACE_STAGING_BUFFER *pStagingBuffer
);

#endif // _FECS_EVENT_LIST_H_

#ifdef __cplusplus
} // extern "C"
#endif
#endif // _G_FECS_EVENT_LIST_NVOC_H_
