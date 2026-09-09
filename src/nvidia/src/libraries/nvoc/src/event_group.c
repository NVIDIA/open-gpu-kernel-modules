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

/*
 * This file is part of the NVOC runtime.
 */

#define NVOC_EVENT_GROUP_H_PRIVATE_ACCESS_ALLOWED

#include "nvport/nvport.h"
#include "nvoc/prelude.h"
#include "nvoc/event_group.h"
#include "nvoc/event.h"
#include "nvoc/event_bus.h"
#include "utils/nvassert.h"

#define EVENTGROUP_TRACE_ID_SEQ          55:0
#define EVENTGROUP_TRACE_ID_ORIGINATOR   63:56

static inline NvU64 _eventgroupNextTraceSeq(void)
{
    static PORT_ATOMIC NvU64 g_eventGroupTraceCounter;
    return portAtomicExIncrementU64(&g_eventGroupTraceCounter);
}

#if defined(PORT_MODULE_sync) && PORT_MODULE_sync
static NV_FORCEINLINE NV_STATUS
_eventgroupLockCreate(EventGroup *pEventGroup)
{
    pEventGroup->pLock = portSyncSpinlockCreate(portMemAllocatorGetGlobalNonPaged());
    return (pEventGroup->pLock != NULL) ? NV_OK : NV_ERR_NO_MEMORY;
}

static NV_FORCEINLINE void
_eventgroupLockDestroy(EventGroup *pEventGroup)
{
    if (pEventGroup->pLock != NULL)
    {
        portSyncSpinlockDestroy(pEventGroup->pLock);
        pEventGroup->pLock = NULL;
    }
}

static NV_FORCEINLINE void
_eventgroupLockAcquire(EventGroup *pEventGroup)
{
    portSyncSpinlockAcquire(pEventGroup->pLock);
}

static NV_FORCEINLINE void
_eventgroupLockRelease(EventGroup *pEventGroup)
{
    portSyncSpinlockRelease(pEventGroup->pLock);
}
#else
static NV_FORCEINLINE NV_STATUS
_eventgroupLockCreate(EventGroup *pEventGroup)
{
    PORT_UNREFERENCED_VARIABLE(pEventGroup);
    return NV_OK;
}

static NV_FORCEINLINE void
_eventgroupLockDestroy(EventGroup *pEventGroup)
{
    PORT_UNREFERENCED_VARIABLE(pEventGroup);
}

static NV_FORCEINLINE void
_eventgroupLockAcquire(EventGroup *pEventGroup)
{
    PORT_UNREFERENCED_VARIABLE(pEventGroup);
}

static NV_FORCEINLINE void
_eventgroupLockRelease(EventGroup *pEventGroup)
{
    PORT_UNREFERENCED_VARIABLE(pEventGroup);
}
#endif

NV_STATUS eventgroupConstruct_IMPL(EventGroup *pEventGroup, EventBus *pEventBus)
{
    NV_ASSERT_OR_RETURN(pEventGroup != NULL, NV_ERR_INVALID_POINTER);
    NV_ASSERT_OR_RETURN(pEventBus != NULL, NV_ERR_INVALID_POINTER);

    pEventGroup->pEventBus = pEventBus;
    listInitIntrusive(&pEventGroup->events);
    pEventGroup->bHasExportEvent = NV_FALSE;
    pEventGroup->eventCount = 0;
    pEventGroup->pausedEventCount = 0;
    pEventGroup->dispatchedEventCount = 0;
    pEventGroup->traceId = 0;
    pEventGroup->groupAttributes = 0;

    return _eventgroupLockCreate(pEventGroup);
}

void eventgroupDestruct_IMPL(EventGroup *pEventGroup)
{
    Event *pEvent;

    if (pEventGroup == NULL)
        return;

    //
    // Unlink and delete all events in this group.
    // The EventGroup is the owner of event lifetime once an event is added.
    //
    pEvent = listHead(&pEventGroup->events);
    while (pEvent != NULL)
    {
        Event *pNext = listNext(&pEventGroup->events, pEvent);
        listRemove(&pEventGroup->events, pEvent);
        pEvent->pEventGroup = NULL;
        objDelete(pEvent);
        pEvent = pNext;
    }

    pEventGroup->eventCount = 0;
    pEventGroup->pausedEventCount = 0;
    pEventGroup->bHasExportEvent = NV_FALSE;

    _eventgroupLockDestroy(pEventGroup);
}

//
// The export / class-id traversal helpers below are intended to be called
// from the exportEventGroup callback after the bus has finished dispatching
// the group, where no concurrent eventgroupAddEvent() can race. They still
// acquire the group lock around each scan so that a future caller using
// them outside that context (for example, a handler that wants to inspect
// sibling events while another thread emits an associated event) does not
// silently race with a list append. Holding the lock for the whole scan
// rather than per-step keeps the traversal atomic from the caller's point
// of view; the scans are bounded by the group's event count, which is small.
//
const Event *eventgroupFirstExportEvent_IMPL(EventGroup *pEventGroup)
{
    Event *pEvent;
    const Event *pResult = NULL;

    NV_ASSERT_OR_RETURN(pEventGroup != NULL, NULL);

    if (!eventgroupHasExportEvent(pEventGroup))
    {
        return NULL;
    }

    _eventgroupLockAcquire(pEventGroup);
    for (pEvent = listHead(&pEventGroup->events);
         pEvent != NULL;
         pEvent = listNext(&pEventGroup->events, pEvent))
    {
        if (objGetClassInfo(pEvent)->visibility == NVOC_EVENT_VISIBILITY_EXPORT)
        {
            pResult = pEvent;
            break;
        }
    }
    _eventgroupLockRelease(pEventGroup);

    return pResult;
}

const Event *eventgroupNextExportEvent_IMPL(EventGroup *pEventGroup,
                                            const Event *pEvent)
{
    Event *pNext;
    const Event *pResult = NULL;

    NV_ASSERT_OR_RETURN(pEventGroup != NULL, NULL);
    NV_ASSERT_OR_RETURN(pEvent != NULL, NULL);

    _eventgroupLockAcquire(pEventGroup);
    for (pNext = listNext(&pEventGroup->events, (Event *)pEvent);
         pNext != NULL;
         pNext = listNext(&pEventGroup->events, pNext))
    {
        if (objGetClassInfo(pNext)->visibility == NVOC_EVENT_VISIBILITY_EXPORT)
        {
            pResult = pNext;
            break;
        }
    }
    _eventgroupLockRelease(pEventGroup);

    return pResult;
}

const Event *eventgroupFirstEventOfType_IMPL(EventGroup *pEventGroup, NvU32 eventId)
{
    Event *pEvent;
    const Event *pResult = NULL;

    NV_ASSERT_OR_RETURN(pEventGroup != NULL, NULL);

    _eventgroupLockAcquire(pEventGroup);
    for (pEvent = listHead(&pEventGroup->events);
         pEvent != NULL;
         pEvent = listNext(&pEventGroup->events, pEvent))
    {
        if (objGetClassId(pEvent) == eventId)
        {
            pResult = pEvent;
            break;
        }
    }
    _eventgroupLockRelease(pEventGroup);

    return pResult;
}

const Event *eventgroupNextEventOfType_IMPL(EventGroup *pEventGroup,
                                             const Event *pEvent,
                                             NvU32 eventId)
{
    Event *pNext;
    const Event *pResult = NULL;

    NV_ASSERT_OR_RETURN(pEventGroup != NULL, NULL);
    NV_ASSERT_OR_RETURN(pEvent != NULL, NULL);

    _eventgroupLockAcquire(pEventGroup);
    for (pNext = listNext(&pEventGroup->events, (Event *)pEvent);
         pNext != NULL;
         pNext = listNext(&pEventGroup->events, pNext))
    {
        if (objGetClassId(pNext) == eventId)
        {
            pResult = pNext;
            break;
        }
    }
    _eventgroupLockRelease(pEventGroup);

    return pResult;
}

//
// The list-navigation methods take the group lock so a concurrent eventgroupAddEvent()
// (which appends a new event at the tail under the same lock) cannot race
// with the listHead/listNext reads here. Callers can therefore step through
// the list one element at a time, dropping the lock between elements, without
// needing to know about the group's internal locking.
//
Event *eventgroupFirstEvent_IMPL(EventGroup *pEventGroup)
{
    Event *pEvent;

    NV_ASSERT_OR_RETURN(pEventGroup != NULL, NULL);

    _eventgroupLockAcquire(pEventGroup);
    pEvent = listHead(&pEventGroup->events);
    _eventgroupLockRelease(pEventGroup);

    return pEvent;
}

Event *eventgroupNextEvent_IMPL(EventGroup *pEventGroup, Event *pEvent)
{
    Event *pNext;

    NV_ASSERT_OR_RETURN(pEventGroup != NULL, NULL);
    NV_ASSERT_OR_RETURN(pEvent != NULL, NULL);

    _eventgroupLockAcquire(pEventGroup);
    pNext = listNext(&pEventGroup->events, pEvent);
    _eventgroupLockRelease(pEventGroup);

    return pNext;
}

NvBool eventgroupHasExportEvent_IMPL(EventGroup *pEventGroup)
{
    NV_ASSERT_OR_RETURN(pEventGroup != NULL, NV_FALSE);
    return pEventGroup->bHasExportEvent;
}

void eventgroupEventPaused_IMPL(EventGroup *pEventGroup)
{
    NV_ASSERT_OR_RETURN_VOID(pEventGroup != NULL);

    _eventgroupLockAcquire(pEventGroup);
    pEventGroup->pausedEventCount++;
    _eventgroupLockRelease(pEventGroup);
}

void eventgroupEventResumed_IMPL(EventGroup *pEventGroup)
{
    NvBool bReportResume = NV_FALSE;

    NV_ASSERT_OR_RETURN_VOID(pEventGroup != NULL);

    _eventgroupLockAcquire(pEventGroup);
    if (pEventGroup->pausedEventCount > 0)
    {
        bReportResume = (pEventGroup->eventCount > 0) &&
                        ((pEventGroup->pausedEventCount + pEventGroup->dispatchedEventCount) ==
                         pEventGroup->eventCount);
        pEventGroup->pausedEventCount--;
    }
    _eventgroupLockRelease(pEventGroup);

    if (bReportResume)
        eventbusGroupResumed(pEventGroup->pEventBus, pEventGroup);
}

void eventgroupEventDispatched_IMPL(EventGroup *pEventGroup)
{
    NV_ASSERT_OR_RETURN_VOID(pEventGroup != NULL);

    _eventgroupLockAcquire(pEventGroup);
    if (pEventGroup->dispatchedEventCount < pEventGroup->eventCount)
    {
        pEventGroup->dispatchedEventCount++;
    }
    _eventgroupLockRelease(pEventGroup);
}

NvBool eventgroupIsRunnable_IMPL(EventGroup *pEventGroup)
{
    NvBool bRunnable;

    NV_ASSERT_OR_RETURN(pEventGroup != NULL, NV_FALSE);

    _eventgroupLockAcquire(pEventGroup);
    //
    // Runnable when at least one event is neither paused nor already
    // dispatched. Groups whose remaining events are all waiting on resume
    // (or have no remaining work at all) report as not runnable so the bus
    // does not pick them up in a tight loop while paused events sit idle.
    //
    bRunnable = (pEventGroup->eventCount > 0) &&
                ((pEventGroup->pausedEventCount + pEventGroup->dispatchedEventCount)
                 < pEventGroup->eventCount);
    _eventgroupLockRelease(pEventGroup);

    return bRunnable;
}

void eventgroupSetTraceId_IMPL(EventGroup *pEventGroup, NvU64 traceId)
{
    NV_ASSERT_OR_RETURN_VOID(pEventGroup != NULL);

    _eventgroupLockAcquire(pEventGroup);
    pEventGroup->traceId = traceId;
    _eventgroupLockRelease(pEventGroup);
}

NvU64 eventgroupGetTraceId_IMPL(EventGroup *pEventGroup)
{
    NvU64 traceId;

    NV_ASSERT_OR_RETURN(pEventGroup != NULL, 0);

    _eventgroupLockAcquire(pEventGroup);
    traceId = pEventGroup->traceId;
    _eventgroupLockRelease(pEventGroup);

    return traceId;
}

NvU64 eventgroupInitTraceId(EventGroup *pEventGroup, NvU8 traceIdPrefix)
{
    NvU64 traceId;

    NV_ASSERT_OR_RETURN(pEventGroup != NULL, 0);

    _eventgroupLockAcquire(pEventGroup);
    traceId = pEventGroup->traceId;
    if (traceId == 0)
    {
        NV_ASSERT_OR_ELSE(traceIdPrefix != 0,
        {
            _eventgroupLockRelease(pEventGroup);
            return 0;
        });

        traceId = REF_NUM64(EVENTGROUP_TRACE_ID_SEQ, _eventgroupNextTraceSeq())
                | REF_NUM64(EVENTGROUP_TRACE_ID_ORIGINATOR, traceIdPrefix);
        pEventGroup->traceId = traceId;
    }
    _eventgroupLockRelease(pEventGroup);

    return traceId;
}

void eventgroupSetAttributes_IMPL(EventGroup *pEventGroup, NvU8 groupAttributes)
{
    NV_ASSERT_OR_RETURN_VOID(pEventGroup != NULL);

    _eventgroupLockAcquire(pEventGroup);
    pEventGroup->groupAttributes = groupAttributes;
    _eventgroupLockRelease(pEventGroup);
}

NvU8 eventgroupGetAttributes_IMPL(EventGroup *pEventGroup)
{
    NvU8 groupAttributes;

    NV_ASSERT_OR_RETURN(pEventGroup != NULL, 0);

    _eventgroupLockAcquire(pEventGroup);
    groupAttributes = pEventGroup->groupAttributes;
    _eventgroupLockRelease(pEventGroup);

    return groupAttributes;
}

void eventgroupAddEvent_IMPL(EventGroup *pEventGroup, Event *pEvent)
{
    NV_ASSERT_OR_RETURN_VOID(pEventGroup != NULL);
    NV_ASSERT_OR_RETURN_VOID(pEvent != NULL);

    _eventgroupLockAcquire(pEventGroup);

    // Link the event to this group
    pEvent->pEventGroup = pEventGroup;

    // Append to end of the event list (O(1) operation)
    listAppendExisting(&pEventGroup->events, pEvent);

    // Update event count
    pEventGroup->eventCount++;

    // Update export flag if needed
    if (objGetClassInfo(pEvent)->visibility == NVOC_EVENT_VISIBILITY_EXPORT)
    {
        pEventGroup->bHasExportEvent = NV_TRUE;
    }

    _eventgroupLockRelease(pEventGroup);
}
