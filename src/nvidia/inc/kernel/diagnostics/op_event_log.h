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
#pragma once

#ifndef _OP_EVENT_LOG_H_
#define _OP_EVENT_LOG_H_

#if defined(SRT_BUILD)
#define RMCFG_FEATURE_x 1
#else
#include "rmconfig.h"
#endif

#include "nvtypes.h"
#include "nvstatus.h"
#include "containers/list.h"
//
// NV_CPER_GUID is used by the always-on legacy CPER path
// (opevtlogFindNextCper + opevtlogAppendCperBytes), so the cper.h
// include must live outside the GPU_OPERATIONAL_EVENTS gate.
//
#include "libraries/cper/cper.h"
//
// OPERATIONAL_EVENT_SEVERITY is used by the always-built severity helper.
// The typedef depends only on nvtypes.h so this include is
// feature-gate-independent.
//
#include "events/event_defs.h"

#include "diagnostics/op_event.h"

//
// Per-group CPER descriptor (POD, GUIDs by value) — supplies the
// record-level state that is not derivable from the events in the group.
// Lives on every typed-POD entry and is fed to cperInit /
// cperAddNvidiaEventSection by opEventLogEntryRenderCper.
//
typedef struct GPU_CPER_GROUP_DESCRIPTOR
{
    NV_CPER_GUID    creatorId;
    NV_CPER_GUID    notifyType;
    NvU64           groupCursor;        // log-assigned; used as CPER recordId
    NvU16           sourcePartition;    // MIG GI; 0 if N/A
    NvU16           sourceSubPartition; // MIG CI; 0 if N/A
    NvU32           sectionFlags;       // NV_CPER_SECTION_FLAG_*
    NvBool          bTimestampPrecise;  // cperInit bTimestampPrecise
    NvBool          bDeviceUuidValid;   // NV_TRUE iff deviceUuid is a real UUID
} GPU_CPER_GROUP_DESCRIPTOR;

struct EventGroup;
struct EventBuffer;
struct OpEventBufferBind;

//
// Event POD — one per event in a group.
//
// On the feature-on path the log copies scalar identity out of the live
// GpuOperationalEvent and deep-copies the context chain (preserving the
// concrete-context memory layout so existing serializer thunks cast back
// without modification).
//
typedef struct OpEventLogEvent
{
    // OperationalEvent base
    NvU16 category;
    NvU16 eventCode;
    char  moduleSignature[OPERATIONAL_EVENT_MODULE_SIGNATURE_LEN];
    OPERATIONAL_EVENT_SEVERITY severity;
    OPERATIONAL_EVENT_ATTRIBUTES attributes;
    OPERATIONAL_EVENT_LOG_LEVEL logLevel;
    NvU64 instanceId;        // originator-scoped event sequence ID
    //
    // microseconds; matches OperationalEvent::timestamp and the
    // cperInit timestampUs arg.
    //
    NvU64 timestampUs;

    // GpuOperationalEvent extension
    NvU8  scope;             // GPU_OPERATIONAL_EVENT_SCOPE
    NvU8  originator;        // GPU_OPERATIONAL_EVENT_ORIGINATOR
    NvU8  reportingSource;   // GPU_OPERATIONAL_EVENT_REPORTING_SOURCE
    NvU8  migAttribution;
    NvU64 pdi;               // Per-Device Identifier for CPER
    NvU8  deviceUuid[16];
    NvU8  moduleInstance;
    NvU8  chipletId;

    //
    // Context chain — each context is a byte-identical copy of the live
    // concrete context struct (EventContextHeader at offset 0 +
    // payload); pOutputAdapters is preserved verbatim (it points into
    // .rodata).
    //
    EventContextHeader *pCtxChainHead;
} OpEventLogEvent;

typedef struct OpEventLogEntry
{
    ListNode node;

    //
    // == CPER recordId (always present on the typed-POD path).
    // Generated via cperGenerateRecordId so consumers can run
    // cperRecordIdToSequence over it and get the same sequence the
    // on-wire CPER record carries. Zero on legacy CPER-bytes entries
    // (the bytes already carry their own recordId in the embedded
    // NV_CPER_RECORD_HEADER).
    //
    NvU64    groupCursor;

    //
    // 1-based RMAPI cursor, preserves the pre-existing
    // SYSTEM_READ_CPER semantics.
    //
    NvU64    cperCursor;

    //
    // 0 = unpinned; nonzero = pinned by an in-flight push or
    // SYSTEM_READ_CPER render. Pin only — not the same as "unconsumed".
    //
    NvU32    refcount;

    //
    // pSpinlock-protected. Number of registered OpEventBufferBinds that
    // have not yet ATTEMPTED delivery of this entry (not the same as
    // "successfully delivered" — see _opevtlogPushEntryToBind: delivery
    // is best-effort and the counter is decremented unconditionally
    // after each attempt). Stamped at insert time (typed path) to
    // listCount(binds); legacy CPER-bytes entries are stamped 0. Under
    // best-effort delivery this is normally 0 by the time any external
    // code observes it (the push wave runs synchronously inside
    // opevtlogAppendEventGroup under pPushMutex). Eviction predicate:
    // refcount == 0 && pendingBindCount == 0.
    //
    NvU32    pendingBindCount;

    // Legacy compat (CPER-bytes-only) path — always built.
    NvU32    prebuiltCperSize;
    NvU8    *pPrebuiltCperBytes;

    // Typed-POD path.
    NvU64    traceId;            // eventgroupGetTraceId() snapshot.
    NvU8     groupAttrs;         // RECOVERED / PREVERR / SIMULATED bits.
    GPU_CPER_GROUP_DESCRIPTOR desc;  // POD, GUIDs by value.
    NvU32    eventCount;
    OpEventLogEvent *pEvents;
} OpEventLogEntry;

//
// Module-wide singleton. Opaque to callers — the struct definition
// lives next to the implementation so the public header stays free of
// private fields and intrusive-list machinery.
//
typedef struct OpEventLog OpEventLog;
extern OpEventLog *opEventLog;

//
// Append CPER bytes as an entry (no GOE-typed source). Not pushed to
// bound EventBuffers; readable only via the RM API SYSTEM_READ_CPER
// path.
//
NV_STATUS opevtlogAppendCperBytes(OpEventLog *pLog,
                                  NvU8 *pCperBytes, NvU32 size);

//
// RM API control-call read path. Single-entry per call; matches the
// pre-existing SYSTEM_READ_CPER cursor / UUID semantics. Returns CPER
// bytes (legacy or rendered-on-demand from the typed POD).
//
NV_STATUS opevtlogFindNextCper(OpEventLog *pLog,
                                       NvU64 cperCursor,
                                       const NV_CPER_GUID *pUuidFilter,
                                       NvU8 *pBuffer, NvU32 bufferSize,
                                       NvU32 *pRecordSize,
                                       NvU64 *pNextCperCursor);

//
// Called from _gpuEventBusExportEventGroup under the GPU lock.
// Builds the typed POD entry from the live group, inserts with eviction,
// then drives the in-process producer over the bind registry.
//
NV_STATUS opevtlogAppendEventGroup(OpEventLog *pLog,
                                   struct EventGroup *pEventGroup);

//
// Bind registry. opevtbufbindConstruct / opevtbufbindDestruct call these
// under the GPU lock (RS_FLAGS_ACQUIRE_GPUS_LOCK).
//
NV_STATUS opevtlogRegisterBind(OpEventLog *pLog,
                               struct OpEventBufferBind *pBind);
void      opevtlogUnregisterBind(OpEventLog *pLog,
                                 struct OpEventBufferBind *pBind);

//
// Called from eventbufferDestruct when an EventBuffer is freed before its
// attached binds. Removes affected bindpoints from the registry so
// subsequent producer pushes skip them.
//
void      opevtlogDetachBindsForEventBuffer(OpEventLog *pLog,
                                            struct EventBuffer *pEventBuffer);

//
// Atomically check + mutate KEEP_NEWEST on pEventBuffer's producer state.
// When setting bKeepNewest == NV_TRUE: rejects with NV_ERR_INVALID_STATE
// if any registered bind targets pEventBuffer. When setting
// bKeepNewest == NV_FALSE: always succeeds (KEEP_OLDEST is safe for
// op-event binds). Both the bind-existence check and the
// eventBufferSetKeepNewest call happen under pPushMutex, so this is
// race-free against concurrent opevtlogRegisterBind (which also re-checks
// isKeepNewest under the same mutex). Called from
// eventbuffertBufferCtrlCmdEnableEvent_IMPL.
//
NV_STATUS opevtlogTrySetKeepNewest(OpEventLog *pLog,
                                   struct EventBuffer *pEventBuffer,
                                   NvBool bKeepNewest);

#define OP_EVENT_LOG_MAX_ENTRIES        64U

//
// Singleton lifecycle (called from core system init / teardown) and
// device-less CPER append used by callers that don't carry a pLog.
//
NV_STATUS opEventLogConstruct(void);
NV_STATUS opEventLogAppend(NvU8 *pCperBytes, NvU32 cperBufferSize);
void      opEventLogDestruct(void);

#endif // _OP_EVENT_LOG_H_
