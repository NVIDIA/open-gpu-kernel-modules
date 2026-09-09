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

#include "diagnostics/op_event_log.h"
#include "nvport/memory.h"
#include "nvport/sync.h"
#include "utils/nvassert.h"

#include "gpu/gpu_op_event.h"
#include "diagnostics/op_event_log_entry.h"
#include "gpu/gpu.h"
#include "gpu_mgr/gpu_mgr.h"
#include "nvoc/event_group.h"
#include "diagnostics/import_event_group.h"
#include "rmapi/event_buffer.h"
#include "diagnostics/op_event_buffer_bind.h"
#include "events/gpu/gpu_event_defs.h"
#include "gpu/gpu_uuid.h"
#include "libraries/cper/gpu_cper.h"
#include "gpu/oob/kernel_oob.h"

//
// Wire-format groupSize / groupIndex fields are NvU8, so the per-group
// event count must fit in 8 bits; 256 would alias to 0.
//
#define OP_EVENT_LOG_PUSH_MAX_GROUP_EVENTS NV_U8_MAX

//
// Per-bind registry entry. One POD per attached OpEventBufferBind, owned
// by the log. Snapshots the push-path-relevant bits of the bind so the
// producer push doesn't dereference the OpEventBufferBind NVOC class
// (avoids a header cycle with the bind class header). pBind is a weak
// back-pointer used as a stable identity for the matching unregister.
//
typedef struct OpEventLogBindpoint
{
    ListNode node;
    struct OpEventBufferBind *pBind;       // weak back-pointer for match
    struct EventBuffer       *pTargetEventBuffer;  // weak; null after detach
    NvU8  recordFormat;        // NV_OPERATIONAL_EVENT_BUFFER_RECORD_FORMAT_*
    NvU8  bindId;              // EventBuffer-scoped 8-bit ID
    NvU8  scope;               // NV_OPERATIONAL_EVENT_BUFFER_SCOPE_*
    NvU8  minLogLevel;         // NV_GPU_OPERATIONAL_EVENT_LOG_LEVEL_*
    NvU8  minSeverity;         // NV_OPERATIONAL_EVENT_SEVERITY_*
    NvU8  resourceUuid[NV_UUID_LEN];
    //
    // pSpinlock-protected. First cperCursor this bind has not yet
    // attempted delivery of. Stamped at register time to
    // pLog->nextCperCursor + 1; advanced unconditionally per bind in
    // the push wave.
    //
    NvU64 nextEntryCursor;
} OpEventLogBindpoint;

MAKE_INTRUSIVE_LIST(OpEventLogBindList, OpEventLogBindpoint, node);

MAKE_INTRUSIVE_LIST(OpEventLogEntryList, OpEventLogEntry, node);

//
// Opaque to callers — full definition lives here so the public header
// stays free of private fields and intrusive-list machinery.
//
struct OpEventLog
{
    PORT_SPINLOCK *pSpinlock;
    OpEventLogEntryList entries;
    NvU32 entryCount;

    //
    // 1-based; preserves the legacy SYSTEM_READ_CPER RMAPI cursor model.
    //
    NvU64 nextCperCursor;

    //
    // pSpinlock-protected. Bumped each time an append is not retained because
    // no existing entry was both eligible (refcount == 0,
    // pendingBindCount == 0) and at or below the new entry's severity. Typed
    // entries may still be delivered to live binds via transient fan-out.
    //
    NvU64 overflowDropCount;

    //
    // pSpinlock-protected. Suppresses repeated retention-overflow warnings
    // until an entry is successfully retained again.
    //
    NvBool bRetentionOverflowWarned;

    //
    // pSpinlock-protected. Approximate count of records that failed to
    // make it out during best-effort push: per-record render and
    // EventBuffer publish failures count by 1; whole-entry drops
    // (scratch OOM, defensive paths) charge the entry's would-be
    // record count (GOE = eventCount, CPER = 1). Does NOT include
    // records silently dropped by the producer's own ring-full path —
    // those are visible via pHeader->recordDropcount /
    // vardataDropcount on the consumer side.
    //
    NvU64 droppedRecordCount;

    //
    // pSpinlock-protected. Bumped per whole-group delivery failure for
    // a bind. Counts: CPER render/EventBuffer publish failures (one record = one
    // group); GOE entries where every record failed; and any
    // whole-entry drop (scratch OOM, oversized/empty group,
    // unrecognized format).
    //
    NvU64 droppedGroupCount;

    OpEventLogBindList binds;

    //
    // Serializes _opevtlogPushToBindsLocked across GPUs and against
    // register / unregister / detach. The event-bus flush work item
    // runs under each GPU's own subdevice lock, so two GPUs can
    // otherwise concurrently publish records to the same
    // EventBuffer; the producer has no internal lock.
    //
    PORT_MUTEX *pPushMutex;
};

// Module singleton; lifecycle in opEventLogConstruct / opEventLogDestruct.
OpEventLog *opEventLog = NULL;

/* -------------------------------- File-local helpers -------------------------------- */

static void
_opevtlogFreeEntry(OpEventLogEntry *pEntry)
{
    if (pEntry == NULL)
        return;

    if (pEntry->pPrebuiltCperBytes != NULL)
        portMemFree(pEntry->pPrebuiltCperBytes);

    if (pEntry->pEvents != NULL)
    {
        NvU32 i;
        for (i = 0; i < pEntry->eventCount; i++)
        {
            EventContextHeader *pCtx = pEntry->pEvents[i].pCtxChainHead;
            while (pCtx != NULL)
            {
                EventContextHeader *pNext = pCtx->pNext;
                portMemFree(pCtx);
                pCtx = pNext;
            }
        }
        portMemFree(pEntry->pEvents);
    }

    portMemFree(pEntry);
}

static NvBool
_opevtlogEventMatchesBindScope
(
    const OpEventLogBindpoint *pBp,
    const OpEventLogEvent     *pEv
)
{
    switch (pBp->scope)
    {
        case NV_OPERATIONAL_EVENT_BUFFER_SCOPE_DEVICE:
            return portMemCmp(pEv->deviceUuid, pBp->resourceUuid, sizeof(pBp->resourceUuid)) == 0;
        default:
            return NV_FALSE;
    }
}

static NvBool
_opevtlogBuildBindVisibleSubset
(
    const OpEventLogBindpoint *pBp,
    const OpEventLogEntry     *pEntry,
    NvU8                      *pEventIndices,
    NvU32                     *pEventIndexCount
)
{
    NvU8 maxLogLevel = NV_GPU_OPERATIONAL_EVENT_LOG_LEVEL_ALL;
    NvU8 maxSeverity = NV_OPERATIONAL_EVENT_SEVERITY_ALL;
    NvU32 i;
    NvU32 count = 0;

    NV_ASSERT_OR_RETURN(pBp != NULL, NV_FALSE);
    NV_ASSERT_OR_RETURN(pEntry != NULL, NV_FALSE);
    NV_ASSERT_OR_RETURN(pEventIndices != NULL, NV_FALSE);
    NV_ASSERT_OR_RETURN(pEventIndexCount != NULL, NV_FALSE);
    NV_ASSERT_OR_RETURN(pEntry->eventCount <= OP_EVENT_LOG_PUSH_MAX_GROUP_EVENTS,
                        NV_FALSE);

    for (i = 0; i < pEntry->eventCount; i++)
    {
        const OpEventLogEvent *pEv = &pEntry->pEvents[i];

        if (!_opevtlogEventMatchesBindScope(pBp, pEv))
            continue;

        pEventIndices[count++] = (NvU8)i;
        if ((NvU8)pEv->logLevel > maxLogLevel)
            maxLogLevel = (NvU8)pEv->logLevel;
        if ((NvU8)pEv->severity > maxSeverity)
            maxSeverity = (NvU8)pEv->severity;
    }

    if (count == 0)
        return NV_FALSE;

    //
    // Floor filters apply at group level: when any scope-visible event reaches the requested floor,
    // deliver the full scope-visible subset so correlated context remains available.
    //
    if (pBp->minLogLevel != NV_GPU_OPERATIONAL_EVENT_LOG_LEVEL_ALL &&
        maxLogLevel < pBp->minLogLevel)
        return NV_FALSE;

    if (pBp->minSeverity != NV_OPERATIONAL_EVENT_SEVERITY_ALL &&
        maxSeverity < pBp->minSeverity)
        return NV_FALSE;

    *pEventIndexCount = count;
    return NV_TRUE;
}

//
// Derive a per-entry severity for the eviction policy.
//
// Typed entries (pEvents != NULL): max over the group's event severities.
// OPERATIONAL_EVENT_SEVERITY is monotonic — a group containing one FATAL
// is FATAL.
//
// Legacy CPER-bytes entries (pPrebuiltCperBytes != NULL): map the CPER
// record header's errorSeverity through an explicit switch. The
// NV_CPER_SEVERITY enum is NOT monotonic (RECOVERABLE=0, FATAL=1,
// CORRECTED=2, INFORMATIONAL=3), so direct numeric comparison would be
// wrong. A malformed or too-small buffer falls back to RECOVERABLE
// (conservative — don't let a parse failure make severe legacy records
// the first eviction victim).
//
static OPERATIONAL_EVENT_SEVERITY
_opevtlogEntrySeverity(const OpEventLogEntry *pEntry)
{
    if (pEntry->pEvents != NULL)
    {
        OPERATIONAL_EVENT_SEVERITY maxSev = OPERATIONAL_EVENT_SEVERITY_INFORMATIONAL;
        NvU32 i;
        for (i = 0; i < pEntry->eventCount; i++)
        {
            if (pEntry->pEvents[i].severity > maxSev)
                maxSev = pEntry->pEvents[i].severity;
        }
        return maxSev;
    }

    if (pEntry->pPrebuiltCperBytes != NULL &&
        pEntry->prebuiltCperSize >= sizeof(NV_CPER_RECORD_HEADER))
    {
        const NV_CPER_RECORD_HEADER *pHdr =
            (const NV_CPER_RECORD_HEADER *)pEntry->pPrebuiltCperBytes;
        switch (pHdr->errorSeverity)
        {
            case NV_CPER_SEVERITY_FATAL:
                return OPERATIONAL_EVENT_SEVERITY_FATAL;
            case NV_CPER_SEVERITY_RECOVERABLE:
                return OPERATIONAL_EVENT_SEVERITY_RECOVERABLE;
            case NV_CPER_SEVERITY_CORRECTED:
                return OPERATIONAL_EVENT_SEVERITY_CORRECTED;
            case NV_CPER_SEVERITY_INFORMATIONAL:
                return OPERATIONAL_EVENT_SEVERITY_INFORMATIONAL;
            default:
                break;
        }
    }

    return OPERATIONAL_EVENT_SEVERITY_RECOVERABLE;
}

static void
_opevtlogWarnRetentionOverflowDrop
(
    const OpEventLogEntry        *pEntry,
    OPERATIONAL_EVENT_SEVERITY    newEntrySev,
    NvU32                         entryCount,
    NvBool                        bLiveFanout
)
{
    if ((pEntry->pEvents != NULL) && (pEntry->eventCount > 0))
    {
        const OpEventLogEvent *pEv = &pEntry->pEvents[0];

        NV_PRINTF(LEVEL_WARNING,
                  "OpEventLog: retention overflow; not retaining GOE group "
                  "(entries=%u/%u severity=%u liveFanout=%u first=%.*s/%u/%u attrs=0x%x)\n",
                  entryCount, OP_EVENT_LOG_MAX_ENTRIES, (NvU32)newEntrySev,
                  bLiveFanout,
                  (int)OPERATIONAL_EVENT_MODULE_SIGNATURE_LEN, pEv->moduleSignature,
                  pEv->category, pEv->eventCode, pEntry->groupAttrs);
        return;
    }

    NV_PRINTF(LEVEL_WARNING,
              "OpEventLog: retention overflow; dropping CPER entry "
              "(entries=%u/%u severity=%u)\n",
              entryCount, OP_EVENT_LOG_MAX_ENTRIES, (NvU32)newEntrySev);
}

//
// Pick the eviction target according to the design-doc policy. Caller
// holds pSpinlock.
//
// Scans entries head -> tail. An entry is eligible iff
// refcount == 0 (not pinned) AND pendingBindCount == 0 (no active client
// is still owed delivery). Among eligible entries, picks the one with
// the lowest severity; ties resolved by FIFO via strict-< during the
// scan (the first-seen entry at a given severity wins).
//
// The new entry participates in the severity decision via newEntrySev:
// the picked victim is only evicted if its severity is <= the new
// entry's severity. Otherwise (or if no eligible victim exists), the
// caller drops the new entry and bumps overflowDropCount. This prevents
// a stream of INFORMATIONAL entries from evicting an existing FATAL.
//
// On a successful pick: the victim is removed from the list and
// entryCount is decremented; the caller frees it outside the lock.
//
static void
_opevtlogPickEvictionTarget(OpEventLog *pLog,
                            OPERATIONAL_EVENT_SEVERITY newEntrySev,
                            OpEventLogEntry **ppVictim)
{
    OpEventLogEntry *pBest = NULL;
    OPERATIONAL_EVENT_SEVERITY bestSev = 0;

    OpEventLogEntryListIter it = listIterAll(&pLog->entries);
    while (listIterNext(&it))
    {
        OpEventLogEntry *pEntry = it.pValue;
        OPERATIONAL_EVENT_SEVERITY sev;

        if (pEntry->refcount != 0)
            continue;
        if (pEntry->pendingBindCount != 0)
            continue;

        sev = _opevtlogEntrySeverity(pEntry);

        //
        // Strict-< preserves FIFO at a given severity: the first eligible
        // entry seen at that severity stays the best across the rest of
        // the scan.
        //
        if (pBest == NULL || sev < bestSev)
        {
            pBest   = pEntry;
            bestSev = sev;
        }
    }

    if (pBest == NULL || bestSev > newEntrySev)
    {
        *ppVictim = NULL;
        return;
    }

    listRemove(&pLog->entries, pBest);
    pLog->entryCount--;
    *ppVictim = pBest;
}

//
// Insert pEntry into entries, or return it as a transient live-fanout entry when
// typed retention overflows. When *ppInsertedOut is NULL, this helper has freed
// pEntry; otherwise ownership stays with the retained list if *pbRetainedOut is
// NV_TRUE, or returns to the caller if *pbRetainedOut is NV_FALSE.
//
// initialRefcount is 0 (legacy CPER-bytes) or 1 (typed-POD, pinned for
// the upcoming push). bDeliverToBinds controls whether pendingBindCount
// is stamped to the current bind count (typed) or 0 (legacy). When
// bDeliverToBinds == NV_TRUE the caller must hold pPushMutex so
// listCount(pLog->binds) is stable across this call.
//
// On NV_OK: *ppInsertedOut is the retained or transient entry, OR NULL if the
// entry was fully dropped on overflow (already freed; overflowDropCount
// incremented).
//
static NV_STATUS
_opevtlogInsertWithEviction(OpEventLog *pLog,
                            OpEventLogEntry *pEntry,
                            NvU32 initialRefcount,
                            NvBool bDeliverToBinds,
                            OpEventLogEntry **ppInsertedOut,
                            NvBool *pbRetainedOut)
{
    OpEventLogEntry *pEvicted = NULL;
    NvBool bRetentionDropped = NV_FALSE;
    NvBool bTransientFanout = NV_FALSE;
    NvBool bWarnRetentionOverflow = NV_FALSE;
    OPERATIONAL_EVENT_SEVERITY droppedNewSev = 0;
    NvU32 droppedEntryCount = 0;
    NvU32 bindCount = 0;

    if (bDeliverToBinds)
        bindCount = (NvU32)listCount(&pLog->binds);

    *ppInsertedOut = NULL;
    if (pbRetainedOut != NULL)
        *pbRetainedOut = NV_FALSE;

    portSyncSpinlockAcquire(pLog->pSpinlock);

    if (pLog->entryCount >= OP_EVENT_LOG_MAX_ENTRIES)
    {
        OPERATIONAL_EVENT_SEVERITY newSev = _opevtlogEntrySeverity(pEntry);
        _opevtlogPickEvictionTarget(pLog, newSev, &pEvicted);
        if (pEvicted == NULL)
        {
            pLog->overflowDropCount++;
            bRetentionDropped = NV_TRUE;
            bTransientFanout = bDeliverToBinds;
            if (!pLog->bRetentionOverflowWarned)
            {
                pLog->bRetentionOverflowWarned = NV_TRUE;
                bWarnRetentionOverflow = NV_TRUE;
            }
            droppedNewSev = newSev;
            droppedEntryCount = pLog->entryCount;
        }
    }

    if (!bRetentionDropped || bTransientFanout)
    {
        pEntry->cperCursor       = ++pLog->nextCperCursor;
        pEntry->refcount         = initialRefcount;
        pEntry->pendingBindCount = bindCount;
        if (!bRetentionDropped)
        {
            listAppendExisting(&pLog->entries, pEntry);
            pLog->entryCount++;
            pLog->bRetentionOverflowWarned = NV_FALSE;
            if (pbRetainedOut != NULL)
                *pbRetainedOut = NV_TRUE;
        }
        *ppInsertedOut = pEntry;
    }

    portSyncSpinlockRelease(pLog->pSpinlock);

    if (bWarnRetentionOverflow)
        _opevtlogWarnRetentionOverflowDrop(pEntry, droppedNewSev,
                                           droppedEntryCount, bTransientFanout);
    if (pEvicted != NULL)
        _opevtlogFreeEntry(pEvicted);
    if (bRetentionDropped && !bTransientFanout)
        _opevtlogFreeEntry(pEntry);

    return NV_OK;
}

//
// Compose the group cursor in the CPER recordId encoding (high 49 bits
// = microseconds since the current decade epoch; low 15 bits = atomic
// sequence counter). The cursor is later stamped into
// initParams.recordId by the CPER renderer, so the cursor and the
// recordId have to share construction — otherwise the on-wire CPER
// header would carry a value whose low bits cperRecordIdToSequence()
// can't decode as a sequence.
//
static NvU64
_opevtlogBuildGroupCursor(void)
{
    return cperGenerateRecordId();
}

//
// Deep-copy one context chain. Each copy is byte-identical to the live
// concrete struct (EventContextHeader at offset 0 + dataSize bytes of
// payload). The copy's pNext is rewritten to chain to the next copy;
// pOutputAdapters is preserved verbatim (it points into .rodata and
// survives the live event).
//
// Returns NV_OK and writes the chain head into *ppHeadOut, or an error
// status with any partial chain freed.
//
static NV_STATUS
_opevtlogCopyContextChain(EventContextHeader *pLive,
                          EventContextHeader **ppHeadOut)
{
    EventContextHeader  *pHead = NULL;
    EventContextHeader **ppSlot = &pHead;

    while (pLive != NULL)
    {
        NvU32 totalSize = (NvU32)sizeof(EventContextHeader) + pLive->dataSize;
        EventContextHeader *pCopy = portMemAllocNonPaged(totalSize);
        if (pCopy == NULL)
        {
            // Unwind any partial chain.
            while (pHead != NULL)
            {
                EventContextHeader *pNext = pHead->pNext;
                portMemFree(pHead);
                pHead = pNext;
            }
            return NV_ERR_NO_MEMORY;
        }

        portMemCopy(pCopy, totalSize, pLive, totalSize);
        pCopy->pNext = NULL;
        *ppSlot = pCopy;
        ppSlot = &pCopy->pNext;
        pLive = pLive->pNext;
    }

    *ppHeadOut = pHead;
    return NV_OK;
}

//
// Copy live OperationalEvent / GpuOperationalEvent scalars into a POD
// slot.
//
static void
_opevtlogCopyEventScalars(OpEventLogEvent *pDst,
                          const GpuOperationalEvent *pLive)
{
    const OperationalEvent *pOp = staticCast(pLive, OperationalEvent);

    pDst->category    = pOp->category;
    pDst->eventCode   = pOp->eventCode;
    portMemCopy(pDst->moduleSignature, sizeof(pDst->moduleSignature),
                pOp->moduleSignature, sizeof(pOp->moduleSignature));
    pDst->severity    = pOp->severity;
    pDst->attributes  = pOp->attributes;
    pDst->logLevel    = pOp->logLevel;
    pDst->instanceId  = pOp->instanceId;
    pDst->timestampUs = pOp->timestamp;

    pDst->scope           = (NvU8)pLive->scope;
    pDst->originator      = (NvU8)pLive->originator;
    pDst->reportingSource = (NvU8)pLive->reportingSource;
    pDst->migAttribution  = pLive->migAttribution;
    pDst->pdi             = pLive->pdi;
    pDst->moduleInstance  = pLive->moduleInstance;
    pDst->chipletId       = pLive->chipletId;
    portMemCopy(pDst->deviceUuid, sizeof(pDst->deviceUuid),
                pLive->deviceUuid, sizeof(pLive->deviceUuid));
}

//
// Populate the descriptor used by both EventBuffer CPER rendering and the
// RM API read-path CPER rendering. GUIDs are stored by value so the
// descriptor remains POD across the event-group teardown.
//
static void
_opevtlogBuildDescriptor(GPU_CPER_GROUP_DESCRIPTOR *pDesc,
                         const OpEventLogEntry *pEntry,
                         const OpEventLogEvent *pFirstEvent)
{
    static const NV_CPER_GUID creatorPfDrv = NV_CPER_CREATOR_NVIDIA_GPU_PF_DRIVER_GUID;
    static const NV_CPER_GUID notifyInt    = NV_CPER_NOTIFY_NVIDIA_GPU_INTERRUPT_GUID;
    static const NV_CPER_GUID notifyFw     = NV_CPER_NOTIFY_NVIDIA_GPU_FW_FAULT_GUID;
    static const NV_CPER_GUID notifyTo     = NV_CPER_NOTIFY_NVIDIA_GPU_TIMEOUT_GUID;
    static const NV_CPER_GUID notifySwChk  = NV_CPER_NOTIFY_NVIDIA_GPU_SW_CHECK_GUID;

    portMemSet(pDesc, 0, sizeof(*pDesc));

    pDesc->creatorId = creatorPfDrv;

    //
    // Notify-type from the first event's reporting source. Keeps CPER
    // notification GUID consistent with the underlying detection path.
    //
    switch (pFirstEvent->reportingSource)
    {
        case GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT:
            pDesc->notifyType = notifyInt;
            break;
        case GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_FW_FAULT:
            pDesc->notifyType = notifyFw;
            break;
        case GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_TIMEOUT:
            pDesc->notifyType = notifyTo;
            break;
        case GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_SW_CHECK:
        case GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_UNKNOWN:
        default:
            pDesc->notifyType = notifySwChk;
            break;
    }

    pDesc->groupCursor       = pEntry->groupCursor;
    pDesc->sourcePartition   = 0;
    pDesc->sourceSubPartition = 0;
    pDesc->sectionFlags      = 0;
    pDesc->bTimestampPrecise = NV_TRUE;

    //
    // gpuopevtConstruct zero-initializes deviceUuid and only overwrites
    // it when a real pGpu->gpuUuid is available. Treat a non-zero
    // deviceUuid as authoritative; otherwise the renderer suppresses the
    // CPER FRU ID so UUID-filtered SYSTEM_READ_CPER cannot match.
    //
    {
        NvU32 i;
        pDesc->bDeviceUuidValid = NV_FALSE;
        for (i = 0; i < sizeof(pFirstEvent->deviceUuid); i++)
        {
            if (pFirstEvent->deviceUuid[i] != 0)
            {
                pDesc->bDeviceUuidValid = NV_TRUE;
                break;
            }
        }
    }
}

//
// Build a typed POD entry from a live EventGroup. Holds no locks; all
// allocation happens outside the log spinlock. The caller supplies a
// pre-composed groupCursor in the CPER recordId encoding (see
// _opevtlogBuildGroupCursor).
//
static NV_STATUS
_opevtlogBuildTypedEntry(struct EventGroup *pEventGroup,
                        NvU64 groupCursor,
                        OpEventLogEntry **ppEntryOut)
{
    OpEventLogEntry *pEntry = NULL;
    const Event             *pLiveEvent;
    NvU32                    eventCount = 0;
    NvU32                    i;
    NV_STATUS                status = NV_OK;

    //
    // First scan: count export events AND validate that each is a
    // GpuOperationalEvent. Doing both in the same pass lets us fail
    // mixed-export groups cleanly with NV_ERR_NOT_SUPPORTED before any
    // allocation. Retaining only the GpuOperationalEvent subset would
    // silently change group semantics for the consumer, so we reject
    // the whole group instead.
    //
    pLiveEvent = eventgroupFirstExportEvent(pEventGroup);
    while (pLiveEvent != NULL)
    {
        if (dynamicCast((Event *)pLiveEvent, GpuOperationalEvent) == NULL)
            return NV_ERR_NOT_SUPPORTED;

        eventCount++;
        pLiveEvent = eventgroupNextExportEvent(pEventGroup, pLiveEvent);
    }

    if (eventCount == 0)
        return NV_ERR_INVALID_STATE;

    //
    // Reject before retention. Past this point pendingBindCount would
    // be stamped to listCount(binds) for every GOE bind, and any GOE
    // bind would then reject delivery permanently (the wire format's
    // NvU8 groupSize / groupIndex fields cannot encode > NV_U8_MAX
    // events). The entry would stay pending forever and eventually
    // force every new append to drop. Reject up front instead.
    //
    if (eventCount > OP_EVENT_LOG_PUSH_MAX_GROUP_EVENTS)
        return NV_ERR_NOT_SUPPORTED;

    pEntry = portMemAllocNonPaged(sizeof(*pEntry));
    if (pEntry == NULL)
        return NV_ERR_NO_MEMORY;
    portMemSet(pEntry, 0, sizeof(*pEntry));
    pEntry->groupCursor = groupCursor;
    pEntry->traceId     = eventgroupGetTraceId(pEventGroup);
    pEntry->groupAttrs  = eventgroupGetAttributes(pEventGroup);
    pEntry->eventCount  = eventCount;

    pEntry->pEvents = portMemAllocNonPaged(eventCount * sizeof(*pEntry->pEvents));
    if (pEntry->pEvents == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto fail;
    }
    portMemSet(pEntry->pEvents, 0, eventCount * sizeof(*pEntry->pEvents));

    //
    // Second pass: copy scalars and context chains. The first-pass scan
    // already verified the type of every event in the group; the
    // dynamicCast here is a defensive invariant check — the only way
    // it can fail is if the group mutated between the two walks, which
    // the caller's locking is supposed to prevent.
    //
    pLiveEvent = eventgroupFirstExportEvent(pEventGroup);
    for (i = 0; i < eventCount && pLiveEvent != NULL; i++)
    {
        GpuOperationalEvent *pGpuOp =
            dynamicCast((Event *)pLiveEvent, GpuOperationalEvent);
        NV_ASSERT_OR_ELSE(pGpuOp != NULL,
                          { status = NV_ERR_INVALID_STATE; goto fail; });

        _opevtlogCopyEventScalars(&pEntry->pEvents[i], pGpuOp);

        status = _opevtlogCopyContextChain(
            staticCast(pGpuOp, OperationalEvent)->pContextList,
            &pEntry->pEvents[i].pCtxChainHead);
        if (status != NV_OK)
            goto fail;

        pLiveEvent = eventgroupNextExportEvent(pEventGroup, pLiveEvent);
    }

    _opevtlogBuildDescriptor(&pEntry->desc, pEntry, &pEntry->pEvents[0]);

    *ppEntryOut = pEntry;
    return NV_OK;

fail:
    _opevtlogFreeEntry(pEntry);
    return status;
}

// Add one rendered record to the target EventBuffer and notify through
// the producer model for this RM domain.
static NV_STATUS
_opevtlogEventBufferAddNotify
(
    EventBuffer *pBuf,
    EVENT_BUFFER_PRODUCER_DATA *pData,
    NvU32 recordType,
    NvU32 recordSubtype
)
{
    return eventbufferAddNotify(pBuf, pData, recordType, recordSubtype,
                                0 /*notifyIndex*/,
                                NULL /*pGpu -- device-less*/);
}

//
// Push one typed entry to one bind, BEST-EFFORT.
//
// Each record (per-event for GOE, per-group for CPER) is rendered and
// fed to the EventBuffer producer independently. Failures are counted but
// NEVER retried -- the caller advances the bind's cursor and decrements
// pendingBindCount unconditionally after this returns. Partial GOE
// groups are EXPECTED under back-pressure: consumers must tolerate
// missing groupIndex values within a groupSize span (the groupSize /
// groupIndex fields describe the bind-visible group, not a delivery
// guarantee). Do NOT add "retry on next append" semantics here -- that
// would re-introduce the stuck-bind retention pinning we deliberately
// removed when shifting from admission-based atomicity to best-effort
// per-record delivery.
//
// Caller holds pPushMutex and pEntry is pinned (refcount > 0) so the
// typed POD cannot be evicted underfoot.
//
static void
_opevtlogPushEntryToBind(OpEventLog *pLog,
                         OpEventLogBindpoint *pBp,
                         OpEventLogEntry *pEntry)
{
    EventBuffer *pBuf = pBp->pTargetEventBuffer;
    NvU8 *pPayloadScratch = NULL;
    NvU8 *pVardataScratch = NULL;
    NvU8 eventIndices[OP_EVENT_LOG_PUSH_MAX_GROUP_EVENTS];
    NvU32 visibleEventCount = 0;
    NvU32 payloadCap;
    //
    // Bind-time validation (opevtbufbindConstruct) guarantees
    // recordBuffer.recordSize >= NV_MAX(sizeof(GOE record),
    // sizeof(CPER record)), so payload bodies fit in compile-time
    // constants and we don't need a runtime accessor here. Vardata
    // scratch tracks the buffer's actual vardata ring size -- a fixed
    // cap would silently truncate CPER blobs that fit in a client's
    // generous buffer, and the renderer respects vardataCap as a
    // capacity ceiling so we never write past what we allocated.
    //
    const NvU32 vardataCap = pBuf->producerInfo.vardataBuffer.bufferSize;

    if (pEntry->eventCount == 0 ||
        pEntry->eventCount > OP_EVENT_LOG_PUSH_MAX_GROUP_EVENTS)
    {
        goto count_drop_whole_entry;
    }

    if (!_opevtlogBuildBindVisibleSubset(pBp, pEntry, eventIndices,
                                         &visibleEventCount))
    {
        return;
    }

    if (pBp->recordFormat == NV_OPERATIONAL_EVENT_BUFFER_RECORD_FORMAT_GOE)
    {
        payloadCap = (NvU32)(sizeof(NV_GPU_OPERATIONAL_EVENT_RECORD) -
                             sizeof(NV_EVENT_BUFFER_RECORD_HEADER));
    }
    else if (pBp->recordFormat == NV_OPERATIONAL_EVENT_BUFFER_RECORD_FORMAT_CPER)
    {
        payloadCap = (NvU32)(sizeof(NV_OPERATIONAL_EVENT_CPER_RECORD) -
                             sizeof(NV_EVENT_BUFFER_RECORD_HEADER));
    }
    else
    {
        // Unrecognized format — count as a whole-group drop and return.
        portSyncSpinlockAcquire(pLog->pSpinlock);
        pLog->droppedGroupCount++;
        portSyncSpinlockRelease(pLog->pSpinlock);
        return;
    }

    pPayloadScratch = portMemAllocNonPaged(payloadCap);
    if (pPayloadScratch == NULL)
        goto count_drop_whole_entry;

    //
    // vardataCap == 0 is a valid GOE-bind configuration (CPER binds with
    // zero vardata are rejected at bind time). Skip the alloc in that
    // case; the renderer accepts NULL+0 for zero-context paths and the
    // per-context capacity check inside _walkGoeContextsVardata fails
    // any actual write attempt.
    //
    if (vardataCap > 0)
    {
        pVardataScratch = portMemAllocNonPaged(vardataCap);
        if (pVardataScratch == NULL)
        {
            portMemFree(pPayloadScratch);
            goto count_drop_whole_entry;
        }
    }

    if (pBp->recordFormat == NV_OPERATIONAL_EVENT_BUFFER_RECORD_FORMAT_GOE)
    {
        NvU32 i;
        NvU32 perRecordDrops = 0;

        //
        // Best-effort per-record loop. Render failures (the renderer
        // signals NV_ERR_BUFFER_TOO_SMALL or similar when this record's
        // serialized form doesn't fit the buffer's vardataCap) are
        // counted as per-record drops and the loop continues to the
        // next record. EventBuffer publish failures are also counted
        // and skipped -- the producer's own ring-full path drops silently and bumps
        // pHeader->recordDropcount / vardataDropcount, which is the
        // expected back-pressure signal under this design.
        //
        for (i = 0; i < visibleEventCount; i++)
        {
            NvU32 payloadSize = 0, vardataSize = 0;
            NvU32 eventIndex = eventIndices[i];
            EVENT_BUFFER_PRODUCER_DATA data;
            NV_STATUS status;

            status = opEventLogEntryRenderToEventBufferGoe(
                pEntry, eventIndex, (NvU8)visibleEventCount, (NvU8)i,
                pBp->bindId,
                pPayloadScratch, payloadCap,
                pVardataScratch, vardataCap,
                &payloadSize, &vardataSize);
            if (status != NV_OK)
            {
                perRecordDrops++;
                continue;
            }

            portMemSet(&data, 0, sizeof(data));
            data.pPayload    = NV_PTR_TO_NvP64(pPayloadScratch);
            data.payloadSize = payloadSize;
            data.pVardata    = NV_PTR_TO_NvP64(pVardataScratch);
            data.vardataSize = vardataSize;

            status = _opevtlogEventBufferAddNotify(pBuf, &data,
                NV_EVENT_BUFFER_RECORD_TYPE_OPERATIONAL_EVENT_GOE,
                NV_OPERATIONAL_EVENT_RECORD_SUBTYPE_GOE_V1);
            if (status != NV_OK)
                perRecordDrops++;
        }

        portMemFree(pPayloadScratch);
        portMemFree(pVardataScratch);

        if (perRecordDrops > 0)
        {
            portSyncSpinlockAcquire(pLog->pSpinlock);
            pLog->droppedRecordCount += perRecordDrops;
            // Whole-group drop iff every record failed.
            if (perRecordDrops == visibleEventCount)
                pLog->droppedGroupCount++;
            portSyncSpinlockRelease(pLog->pSpinlock);
        }
        return;
    }

    // CPER: one record per group; any failure is a whole-group drop.
    {
        NvU32 payloadSize = 0, vardataSize = 0;
        EVENT_BUFFER_PRODUCER_DATA data;
        NV_STATUS status;

        status = opEventLogEntryRenderToEventBufferCperSubset(
            pEntry, eventIndices, visibleEventCount, pBp->bindId,
            pPayloadScratch, payloadCap,
            pVardataScratch, vardataCap,
            &payloadSize, &vardataSize);
        if (status != NV_OK)
        {
            portMemFree(pPayloadScratch);
            portMemFree(pVardataScratch);
            goto count_drop_whole_entry;
        }

        portMemSet(&data, 0, sizeof(data));
        data.pPayload    = NV_PTR_TO_NvP64(pPayloadScratch);
        data.payloadSize = payloadSize;
        data.pVardata    = NV_PTR_TO_NvP64(pVardataScratch);
        data.vardataSize = vardataSize;

        status = _opevtlogEventBufferAddNotify(pBuf, &data,
            NV_EVENT_BUFFER_RECORD_TYPE_OPERATIONAL_EVENT_CPER,
            NV_OPERATIONAL_EVENT_RECORD_SUBTYPE_CPER_V1);

        portMemFree(pPayloadScratch);
        portMemFree(pVardataScratch);

        if (status != NV_OK)
            goto count_drop_whole_entry;
    }
    return;

count_drop_whole_entry:
    //
    // Charge droppedRecordCount with the number of records this entry
    // would have produced — GOE = pEntry->eventCount (each event is a
    // record), CPER = 1. Without this, GOE scratch-OOM and similar
    // whole-entry drops would undercount: an N-record group would
    // register as a single record drop.
    //
    {
        NvU32 wouldBeRecords =
            (pBp->recordFormat == NV_OPERATIONAL_EVENT_BUFFER_RECORD_FORMAT_GOE)
            ? ((visibleEventCount != 0) ? visibleEventCount : pEntry->eventCount)
            : 1U;

        portSyncSpinlockAcquire(pLog->pSpinlock);
        pLog->droppedRecordCount += wouldBeRecords;
        pLog->droppedGroupCount++;
        portSyncSpinlockRelease(pLog->pSpinlock);
    }
}

//
// Deliver the just-inserted typed entry to every registered bind.
// Best-effort: each bind gets exactly one attempt; the bind's cursor
// is advanced past pEntry and pEntry->pendingBindCount is decremented
// unconditionally after the attempt. There is no backlog and no retry
// — under the best-effort design, every typed entry is "consumed for
// this bind" the moment we've tried to deliver it once.
//
// Caller holds pPushMutex; pEntry is pinned by the insert path so it
// cannot be evicted underfoot. pEntry must be a typed entry
// (pEvents != NULL); legacy CPER-bytes entries are never delivered to
// binds and the legacy append path doesn't invoke this helper.
// Iterates pLog->binds directly — pPushMutex serializes register /
// unregister / detach against this loop, so each bindpoint is stable
// for the duration of its attempt.
//
static void
_opevtlogPushToBindsLocked(OpEventLog *pLog, OpEventLogEntry *pEntry)
{
    OpEventLogBindListIter it;

    NV_ASSERT_OR_RETURN_VOID(pEntry->pEvents != NULL);

    it = listIterAll(&pLog->binds);
    while (listIterNext(&it))
    {
        OpEventLogBindpoint *pBp = it.pValue;

        _opevtlogPushEntryToBind(pLog, pBp, pEntry);

        portSyncSpinlockAcquire(pLog->pSpinlock);
        if (pBp->nextEntryCursor <= pEntry->cperCursor)
            pBp->nextEntryCursor = pEntry->cperCursor + 1;
        if (pEntry->pendingBindCount > 0)
            pEntry->pendingBindCount--;
        portSyncSpinlockRelease(pLog->pSpinlock);
    }
}

//
// Decrement pendingBindCount on every entry the dying bind was still
// owing (cperCursor >= pBp->nextEntryCursor). Called from unregister /
// detach paths after the bind has been removed from pLog->binds and
// before pPushMutex is released, so no concurrent push can reference
// pBp. Ensures stale binds don't pin retention forever.
//
static void
_opevtlogClearBindFromEntries(OpEventLog *pLog,
                               OpEventLogBindpoint *pBp)
{
    portSyncSpinlockAcquire(pLog->pSpinlock);
    {
        OpEventLogEntryListIter it = listIterAll(&pLog->entries);
        while (listIterNext(&it))
        {
            OpEventLogEntry *pEntry = it.pValue;
            if (pEntry->cperCursor < pBp->nextEntryCursor)
                continue;
            if (pEntry->pendingBindCount > 0)
                pEntry->pendingBindCount--;
        }
    }
    portSyncSpinlockRelease(pLog->pSpinlock);
}

/* ---------------------------------- OpEventLog API ---------------------------------- */

static NV_STATUS
_opevtlogInit(OpEventLog *pLog)
{
    pLog->pSpinlock = portSyncSpinlockCreate(portMemAllocatorGetGlobalNonPaged());
    if (pLog->pSpinlock == NULL)
        return NV_ERR_NO_MEMORY;

    listInitIntrusive(&pLog->entries);
    pLog->entryCount      = 0;
    pLog->nextCperCursor     = 0;
    pLog->overflowDropCount  = 0;
    pLog->bRetentionOverflowWarned = NV_FALSE;
    pLog->droppedRecordCount = 0;
    pLog->droppedGroupCount  = 0;

    listInitIntrusive(&pLog->binds);

    pLog->pPushMutex = portSyncMutexCreate(portMemAllocatorGetGlobalNonPaged());
    if (pLog->pPushMutex == NULL)
    {
        portSyncSpinlockDestroy(pLog->pSpinlock);
        pLog->pSpinlock = NULL;
        return NV_ERR_NO_MEMORY;
    }

    return NV_OK;
}

static void
_opevtlogTeardown(OpEventLog *pLog)
{
    if (pLog->pSpinlock == NULL)
        return;

    // Drain bindpoints first so no producer push can race the entry teardown.
    for (;;)
    {
        OpEventLogBindpoint *pBp;

        portSyncMutexAcquire(pLog->pPushMutex);
        pBp = listHead(&pLog->binds);
        if (pBp != NULL)
            listRemove(&pLog->binds, pBp);
        portSyncMutexRelease(pLog->pPushMutex);

        if (pBp == NULL)
            break;
        portMemFree(pBp);
    }
    listDestroy(&pLog->binds);

    if (pLog->pPushMutex != NULL)
    {
        portSyncMutexDestroy(pLog->pPushMutex);
        pLog->pPushMutex = NULL;
    }

    //
    // Drain entries (must be unpinned by this point; destruct is called
    // by the singleton owner with no in-flight readers).
    //
    for (;;)
    {
        OpEventLogEntry *pEntry;

        portSyncSpinlockAcquire(pLog->pSpinlock);
        pEntry = listHead(&pLog->entries);
        if (pEntry != NULL)
        {
            listRemove(&pLog->entries, pEntry);
            pLog->entryCount--;
        }
        portSyncSpinlockRelease(pLog->pSpinlock);

        if (pEntry == NULL)
            break;
        _opevtlogFreeEntry(pEntry);
    }
    listDestroy(&pLog->entries);

    portSyncSpinlockDestroy(pLog->pSpinlock);
    pLog->pSpinlock = NULL;
}

NV_STATUS
opevtlogAppendCperBytes(OpEventLog *pLog, NvU8 *pCperBytes, NvU32 size)
{
    OpEventLogEntry *pEntry = NULL;
    OpEventLogEntry *pInserted = NULL;

    NV_ASSERT_OR_RETURN(pLog != NULL, NV_ERR_INVALID_POINTER);

    if (pLog->pSpinlock == NULL)
        return NV_ERR_INVALID_STATE;

    if (pCperBytes == NULL || size == 0)
        return NV_ERR_INVALID_ARGUMENT;

    pEntry = portMemAllocNonPaged(sizeof(*pEntry));
    if (pEntry == NULL)
        return NV_ERR_NO_MEMORY;
    portMemSet(pEntry, 0, sizeof(*pEntry));

    //
    // Take ownership of the caller's buffer — the legacy path hands it
    // over for retention.
    //
    pEntry->pPrebuiltCperBytes = pCperBytes;
    pEntry->prebuiltCperSize   = size;
    pEntry->groupCursor        = 0;   // legacy entries have no composed cursor

    //
    // Legacy entries are not delivered to binds (pendingBindCount = 0).
    // No pPushMutex needed. Drop on overflow returns NV_OK; the caller's
    // pCperBytes buffer is freed by _opevtlogFreeEntry in either branch
    // (ownership transferred above).
    //
    (void)_opevtlogInsertWithEviction(pLog, pEntry, 0 /*refcount*/,
                                      NV_FALSE /*bDeliverToBinds*/, &pInserted,
                                      NULL /*pbRetainedOut*/);
    return NV_OK;
}

NV_STATUS
opevtlogFindNextCper
(
    OpEventLog *pLog,
    NvU64 cperCursor,
    const NV_CPER_GUID *pUuidFilter,
    NvU8 *pBuffer,
    NvU32 bufferSize,
    NvU32 *pRecordSize,
    NvU64 *pNextCperCursor
)
{
    OpEventLogEntry *pPinned = NULL;
    NV_STATUS status = NV_OK;

    NV_ASSERT_OR_RETURN(pLog != NULL, NV_ERR_INVALID_POINTER);
    NV_ASSERT_OR_RETURN(pRecordSize != NULL, NV_ERR_INVALID_POINTER);
    NV_ASSERT_OR_RETURN(pNextCperCursor != NULL, NV_ERR_INVALID_POINTER);

    *pRecordSize = 0;
    *pNextCperCursor = cperCursor;

    if (pLog->pSpinlock == NULL)
        return NV_ERR_INVALID_STATE;

    //
    // Pick the first entry whose cperCursor > input cursor (and matches
    // the UUID filter for typed-POD entries; legacy entries fall back
    // to cperGetFirstSectionFruId).
    //
    portSyncSpinlockAcquire(pLog->pSpinlock);
    {
        OpEventLogEntryListIter it = listIterAll(&pLog->entries);
        while (listIterNext(&it))
        {
            OpEventLogEntry *pEntry = it.pValue;
            if (pEntry->cperCursor <= cperCursor)
                continue;

            if (pUuidFilter != NULL)
            {
                if (pEntry->pEvents != NULL)
                {
                    NV_CPER_GUID entryFruId;

                    //
                    // The rendered CPER suppresses the FRU ID when the
                    // descriptor's bDeviceUuidValid is false (see
                    // opEventLogEntryRenderCper), so a filter can
                    // never match those records — skip them so the
                    // filter result is consistent with what the
                    // consumer will actually see.
                    //
                    if (!pEntry->desc.bDeviceUuidValid)
                        continue;
                    if (!cperGuidFromUuidBytes(pEntry->pEvents[0].deviceUuid,
                                               &entryFruId))
                        continue;
                    if (!cperGuidEqual(pUuidFilter, &entryFruId))
                        continue;
                }
                else
                if (pEntry->pPrebuiltCperBytes != NULL)
                {
                    NV_CPER_GUID entryFruId;

                    //
                    // The stored CPER bytes are what the consumer sees;
                    // extract the real FRU ID and skip on parse failure.
                    //
                    if (cperGetFirstSectionFruId(pEntry->pPrebuiltCperBytes,
                                                 pEntry->prebuiltCperSize,
                                                 &entryFruId) != NV_OK)
                        continue;
                    if (!cperGuidEqual(pUuidFilter, &entryFruId))
                        continue;
                }
            }

            pPinned = pEntry;
            pPinned->refcount++;
            break;
        }
    }
    portSyncSpinlockRelease(pLog->pSpinlock);

    if (pPinned == NULL)
        return NV_OK;   // empty-tail signal expected by SYSTEM_READ_CPER

    // Render outside the lock.
    if (pPinned->pPrebuiltCperBytes != NULL)
    {
        if (pPinned->prebuiltCperSize > bufferSize)
        {
            status = NV_ERR_BUFFER_TOO_SMALL;
            goto unpin;
        }
        portMemCopy(pBuffer, bufferSize,
                    pPinned->pPrebuiltCperBytes,
                    pPinned->prebuiltCperSize);
        *pRecordSize = pPinned->prebuiltCperSize;
    }
    else
    {
        status = opEventLogEntryRenderCper(pPinned, pBuffer,
                                                  bufferSize, pRecordSize);
        if (status != NV_OK)
            goto unpin;
    }

    *pNextCperCursor = pPinned->cperCursor;

unpin:
    portSyncSpinlockAcquire(pLog->pSpinlock);
    pPinned->refcount--;
    portSyncSpinlockRelease(pLog->pSpinlock);
    return status;
}

// Maps CPER record severity to PORT_LOG_LEVEL + CPER_LOG_LEVEL_FW_* prefix.
static void
_cperSeverityToLogLevelPrefix
(
    NV_CPER_SEVERITY    severity,
    PORT_LOG_LEVEL     *pLevel,
    const char        **ppLogPrefix
)
{
    switch (severity)
    {
        case NV_CPER_SEVERITY_FATAL:
            *pLevel       = PORT_LOG_LEVEL_ERROR;
            *ppLogPrefix  = CPER_LOG_LEVEL_FW_BUG;
            break;
        case NV_CPER_SEVERITY_RECOVERABLE:
            *pLevel       = PORT_LOG_LEVEL_WARNING;
            *ppLogPrefix  = CPER_LOG_LEVEL_FW_WARN;
            break;
        case NV_CPER_SEVERITY_CORRECTED:
        case NV_CPER_SEVERITY_INFORMATIONAL:
        default:
            *pLevel       = PORT_LOG_LEVEL_INFO;
            *ppLogPrefix  = CPER_LOG_LEVEL_FW_INFO;
            break;
    }
}

//
// Best-effort OS log + OOBHUB fan-out for an inserted entry. Drives any
// per-context emitToOsLogXid adapters, then builds a single CPER record
// for the entry and routes the bytes to OS log and/or OOBHUB. UUID-to-pGpu
// is resolved at emit time since entries persist past GPU detach.
//
#define GOE_CPER_BUFFER_SIZE 2048U

static void
_opEventLogEntryEmitToSystemSinks
(
    const OpEventLogEntry *pEntry
)
{
    NvU32          eventIdx;
    OBJGPU        *pGpu;
    NvBool         bOsLogCperDumpEnabled;
    NvBool         bKoobEnabled = NV_FALSE;
    NvU8          *pCperBuffer = NULL;
    NvU32          recordSize  = 0;
    PORT_LOG_LEVEL level;
    const char    *pLogPrefix;
    NV_STATUS      status;
    KernelOob *pKernelOob = NULL;

    NV_ASSERT_OR_RETURN_VOID(pEntry != NULL);
    if (pEntry->eventCount == 0)
        return;

    // Entries currently group events from a single device; pick that device
    // from the first event's UUID.
    pGpu = gpumgrGetGpuFromUuid(pEntry->pEvents[0].deviceUuid,
                                DRF_DEF(2080_GPU_CMD, _GPU_GET_GID_FLAGS, _TYPE, _SHA1) |
                                DRF_DEF(2080_GPU_CMD, _GPU_GET_GID_FLAGS, _FORMAT, _BINARY));
    if (pGpu == NULL)
        return;

    if (pGpu->pOsGpuInfo == NULL)
        return;

    // Always emit entry Xids via contexts that implement emitToOsLogXid.
    for (eventIdx = 0; eventIdx < pEntry->eventCount; eventIdx++)
    {
        const OpEventLogEvent *pEv = &pEntry->pEvents[eventIdx];
        EventContextHeader    *pCtx;

        for (pCtx = pEv->pCtxChainHead; pCtx != NULL; pCtx = pCtx->pNext)
        {
            if (pCtx->pOutputAdapters == NULL)
                continue;
            if (pCtx->pOutputAdapters->emitToOsLogXid != NULL)
                pCtx->pOutputAdapters->emitToOsLogXid(pCtx, pGpu);
        }
    }

    //
    // GSP-RM has no OS_GPU_INFO, but its osErrorLog path forwards Xids to
    // Kernel-RM by RPC. Local CPER dump/OOB sinks still require OS state.
    //
    if (pGpu->pOsGpuInfo == NULL)
        return;

    bOsLogCperDumpEnabled = pGpu->bCperDumpEnabled;
    pKernelOob   = GPU_GET_KERNEL_OOB(pGpu);
    bKoobEnabled = (pKernelOob != NULL);

    if (!bOsLogCperDumpEnabled && !bKoobEnabled)
        return;

    // Build CPER record once for the entry, then route to enabled sinks.
    pCperBuffer = portMemAllocStackOrHeap(GOE_CPER_BUFFER_SIZE);
    if (pCperBuffer == NULL)
        return;

    status = opEventLogEntryRenderCper(pEntry,
                                       pCperBuffer,
                                       GOE_CPER_BUFFER_SIZE,
                                       &recordSize);
    if ((status != NV_OK) || (recordSize < sizeof(NV_CPER_RECORD_HEADER)))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "CPER render failed (status=0x%x size=%u cap=%u); "
                  "dropping OS log/OOBHUB emit\n",
                  status, recordSize, GOE_CPER_BUFFER_SIZE);
        portMemFreeStackOrHeap(pCperBuffer);
        return;
    }

    // The CPER library has already merged section severities into the
    // record header's errorSeverity ("worst of all sections").
    _cperSeverityToLogLevelPrefix(
        ((const NV_CPER_RECORD_HEADER *)pCperBuffer)->errorSeverity,
        &level, &pLogPrefix);

    if (bOsLogCperDumpEnabled)
    {
        cperDumpRecord((PORT_DEVICE *)pGpu->pOsGpuInfo,
                       level,
                       pCperBuffer,
                       recordSize,
                       pLogPrefix);
    }

    if (bKoobEnabled)
    {
        koobCperSendCommon(pGpu, pKernelOob, pCperBuffer, recordSize);
    }

    portMemFreeStackOrHeap(pCperBuffer);
}

NV_STATUS
opevtlogAppendEventGroup
(
    OpEventLog *pLog,
    struct EventGroup *pEventGroup
)
{
    OpEventLogEntry *pEntry = NULL;
    OpEventLogEntry *pInserted = NULL;
    NvBool bRetained = NV_FALSE;
    NvU64 groupCursor;
    NV_STATUS status;

    NV_ASSERT_OR_RETURN(pLog != NULL, NV_ERR_INVALID_POINTER);
    NV_ASSERT_OR_RETURN(pEventGroup != NULL, NV_ERR_INVALID_POINTER);

    //
    // pPushMutex serializes the insert + push across GPUs and against
    // register / unregister / detach. Held across the build / insert /
    // push window so listCount(pLog->binds) is stable for
    // pendingBindCount stamping and so the per-bind push attempt sees
    // a consistent bind set.
    //
    portSyncMutexAcquire(pLog->pPushMutex);

    //
    // groupCursor: adopt the producer's cursor for ImportEventGroup (host import of GSP-rendered
    // records -- preserves the producer's cl90d0 cursor end-to-end so consumer-side records carry
    // the original identity). Otherwise mint via the CPER recordId encoding so the renderer can
    // stamp the value directly into the CPER record header.
    //
    {
        ImportEventGroup *pImport = dynamicCast(pEventGroup, ImportEventGroup);
        groupCursor = (pImport != NULL) ? pImport->cursor
                                        : _opevtlogBuildGroupCursor();
    }

    //
    // Build the typed POD entry outside the spinlock (allocs run here;
    // pPushMutex is still held, which serializes the build against
    // bind register / unregister / detach but does not block readers
    // that take the spinlock alone). _opevtlogBuildTypedEntry handles
    // empty groups (NV_ERR_INVALID_STATE) and non-GpuOperationalEvent
    // export events (NV_ERR_NOT_SUPPORTED) in its first scan.
    //
    status = _opevtlogBuildTypedEntry(pEventGroup, groupCursor, &pEntry);
    if (status != NV_OK)
    {
        portSyncMutexRelease(pLog->pPushMutex);
        return status;
    }

    //
    // Insert pinned (refcount=1) so the upcoming push cannot race
    // eviction. Stamps pendingBindCount = listCount(pLog->binds). If the
    // retained log is full and the new group loses the severity eviction
    // decision, the helper returns a transient entry for live fan-out but
    // leaves it out of the retained list.
    //
    (void)_opevtlogInsertWithEviction(pLog, pEntry, 1 /*refcount*/,
                                      NV_TRUE /*bDeliverToBinds*/,
                                      &pInserted, &bRetained);
    if (pInserted == NULL)
    {
        //
        // Defensive: typed live-fanout entries should come back retained or
        // transient. If the helper ever returns NULL here, it has freed the
        // entry and bumped overflowDropCount. Drop is non-fatal.
        //
        portSyncMutexRelease(pLog->pPushMutex);
        return NV_OK;
    }

    // Best-effort fan-out to OS log + OOBHUB sinks.
    _opEventLogEntryEmitToSystemSinks(pInserted);

    //
    // Best-effort delivery to every bind. No retry on failure; the
    // bind's cursor and pendingBindCount are advanced unconditionally
    // inside this call, and any per-record / whole-group drops bump
    // pLog->droppedRecordCount / droppedGroupCount.
    //
    _opevtlogPushToBindsLocked(pLog, pInserted);

    if (bRetained)
    {
        //
        // Drop the insert pin BEFORE releasing pPushMutex. If we released
        // the mutex first, a concurrent appender entering the eviction
        // decision in this window would see refcount == 1 and treat the
        // just-delivered entry as ineligible — forcing a spurious overflow
        // drop even though the entry has been fully consumed (in the
        // best-effort sense — attempted to all binds).
        //
        portSyncSpinlockAcquire(pLog->pSpinlock);
        pInserted->refcount--;
        portSyncSpinlockRelease(pLog->pSpinlock);
    }
    else
    {
        _opevtlogFreeEntry(pInserted);
    }

    portSyncMutexRelease(pLog->pPushMutex);

    return NV_OK;
}

NV_STATUS
opevtlogRegisterBind
(
    OpEventLog *pLog,
    struct OpEventBufferBind *pBind
)
{
    OpEventLogBindpoint *pBp = NULL;

    NV_ASSERT_OR_RETURN(pBind != NULL, NV_ERR_INVALID_POINTER);
    NV_ASSERT_OR_RETURN(pBind->pTargetEventBuffer != NULL,
                        NV_ERR_INVALID_STATE);

    pBp = portMemAllocNonPaged(sizeof(*pBp));
    if (pBp == NULL)
        return NV_ERR_NO_MEMORY;
    portMemSet(pBp, 0, sizeof(*pBp));

    pBp->pBind              = pBind;
    pBp->pTargetEventBuffer = pBind->pTargetEventBuffer;
    pBp->recordFormat       = pBind->recordFormat;
    pBp->bindId             = pBind->bindId;
    pBp->scope              = pBind->scope;
    pBp->minLogLevel        = pBind->minLogLevel;
    pBp->minSeverity        = pBind->minSeverity;
    portMemCopy(pBp->resourceUuid, sizeof(pBp->resourceUuid),
                pBind->resourceUuid, sizeof(pBp->resourceUuid));

    //
    // pPushMutex serializes the bind list AND the nextCperCursor snapshot
    // against concurrent appends.
    //
    portSyncMutexAcquire(pLog->pPushMutex);

    //
    // Re-check isKeepNewest under pPushMutex so a concurrent
    // opevtlogTrySetKeepNewest call cannot slip between the bind
    // constructor's early-rejection check and our list append. The
    // constructor's check is an optimization; this is the authoritative
    // one.
    //
    if (pBind->pTargetEventBuffer->producerInfo.isKeepNewest)
    {
        portSyncMutexRelease(pLog->pPushMutex);
        portMemFree(pBp);
        return NV_ERR_INVALID_STATE;
    }

    //
    // Stamp nextEntryCursor = nextCperCursor + 1 so the bind owes only
    // future entries; the +1 accounts for the cperCursor that the next
    // insert will produce.
    //
    portSyncSpinlockAcquire(pLog->pSpinlock);
    pBp->nextEntryCursor = pLog->nextCperCursor + 1;
    portSyncSpinlockRelease(pLog->pSpinlock);
    listAppendExisting(&pLog->binds, pBp);
    portSyncMutexRelease(pLog->pPushMutex);

    return NV_OK;
}

void
opevtlogUnregisterBind
(
    OpEventLog *pLog,
    struct OpEventBufferBind *pBind
)
{
    OpEventLogBindpoint *pBpToFree = NULL;

    portSyncMutexAcquire(pLog->pPushMutex);
    {
        OpEventLogBindListIter it = listIterAll(&pLog->binds);
        while (listIterNext(&it))
        {
            if (it.pValue->pBind == pBind)
            {
                pBpToFree = it.pValue;
                listRemove(&pLog->binds, pBpToFree);
                break;
            }
        }

        //
        // Clear pendingBindCount on entries this bind was still owing
        // — must happen while pPushMutex is held so no concurrent push
        // can reference pBpToFree's state.
        //
        if (pBpToFree != NULL)
            _opevtlogClearBindFromEntries(pLog, pBpToFree);
    }
    portSyncMutexRelease(pLog->pPushMutex);

    if (pBpToFree != NULL)
        portMemFree(pBpToFree);
}

NV_STATUS
opevtlogTrySetKeepNewest
(
    OpEventLog *pLog,
    struct EventBuffer *pEventBuffer,
    NvBool bKeepNewest
)
{
    NV_STATUS status = NV_OK;

    if (pLog == NULL || pEventBuffer == NULL)
        return NV_ERR_INVALID_POINTER;
    if (pLog->pPushMutex == NULL)
        return NV_ERR_INVALID_STATE;

    portSyncMutexAcquire(pLog->pPushMutex);

    if (bKeepNewest)
    {
        OpEventLogBindListIter it = listIterAll(&pLog->binds);
        while (listIterNext(&it))
        {
            if (it.pValue->pTargetEventBuffer == pEventBuffer)
            {
                status = NV_ERR_INVALID_STATE;
                break;
            }
        }
    }

    if (status == NV_OK)
        eventBufferSetKeepNewest(&pEventBuffer->producerInfo, bKeepNewest);

    portSyncMutexRelease(pLog->pPushMutex);

    return status;
}

void
opevtlogDetachBindsForEventBuffer
(
    OpEventLog *pLog,
    struct EventBuffer *pEventBuffer
)
{
    if (pLog == NULL || pEventBuffer == NULL)
        return;

    //
    // pPushMutex serializes detach against _opevtlogPushToBindsLocked
    // so the bindpoint memory removed here can't be accessed by a
    // concurrent push iteration. _opevtlogClearBindFromEntries also
    // needs to run under pPushMutex so no push concurrently reads
    // pendingBindCount for an entry we're decrementing.
    //
    portSyncMutexAcquire(pLog->pPushMutex);
    {
        OpEventLogBindpoint *pCur = listHead(&pLog->binds);
        while (pCur != NULL)
        {
            OpEventLogBindpoint *pNext = listNext(&pLog->binds, pCur);

            if (pCur->pTargetEventBuffer == pEventBuffer)
            {
                //
                // Null the live bind's target so its destructor skips
                // the eventbufferFreeBindId call on a dying buffer.
                //
                if (pCur->pBind != NULL)
                    pCur->pBind->pTargetEventBuffer = NULL;

                listRemove(&pLog->binds, pCur);
                _opevtlogClearBindFromEntries(pLog, pCur);
                portMemFree(pCur);
            }

            pCur = pNext;
        }
    }
    portSyncMutexRelease(pLog->pPushMutex);
}

/* -------------------------------- Singleton lifecycle -------------------------------- */

NV_STATUS
opEventLogConstruct(void)
{
    OpEventLog *pLog;
    NV_STATUS   status;

    if (opEventLog != NULL)
        return NV_OK;

    pLog = portMemAllocNonPaged(sizeof(*pLog));
    if (pLog == NULL)
        return NV_ERR_NO_MEMORY;
    portMemSet(pLog, 0, sizeof(*pLog));

    status = _opevtlogInit(pLog);
    if (status != NV_OK)
    {
        portMemFree(pLog);
        return status;
    }

    opEventLog = pLog;
    return NV_OK;
}

NV_STATUS
opEventLogAppend(NvU8 *pCperBytes, NvU32 cperBufferSize)
{
    if (opEventLog == NULL)
        return NV_ERR_INVALID_STATE;
    return opevtlogAppendCperBytes(opEventLog, pCperBytes, cperBufferSize);
}

void
opEventLogDestruct(void)
{
    if (opEventLog == NULL)
        return;
    _opevtlogTeardown(opEventLog);
    portMemFree(opEventLog);
    opEventLog = NULL;
}
