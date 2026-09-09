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

#define NVOC_EVENT_BUS_H_PRIVATE_ACCESS_ALLOWED

#include "nvport/nvport.h"
#include "nvoc/prelude.h"
#include "nvoc/rtti.h"
#include "nvoc/event_bus.h"
#include "nvoc/event.h"
#include "nvoc/event_group.h"
#include "utils/nvassert.h"

// Forward declarations for internal helpers
static NvBool _eventbusDispatchEventGroup(EventBus *pEventBus, EventGroup *pEventGroup);
static NV_STATUS _eventbusDispatchEvent(EventBus *pEventBus, Event *pEvent);
static void _eventbusProcessPendingGroups(EventBus *pEventBus);

#if defined(PORT_MODULE_sync) && PORT_MODULE_sync
static NV_FORCEINLINE NV_STATUS
_eventbusQueueLockCreate(EventBus *pEventBus)
{
    pEventBus->pQueueLock = portSyncSpinlockCreate(portMemAllocatorGetGlobalNonPaged());
    return (pEventBus->pQueueLock != NULL) ? NV_OK : NV_ERR_NO_MEMORY;
}

static NV_FORCEINLINE void
_eventbusQueueLockDestroy(EventBus *pEventBus)
{
    if (pEventBus->pQueueLock != NULL)
    {
        portSyncSpinlockDestroy(pEventBus->pQueueLock);
        pEventBus->pQueueLock = NULL;
    }
}

static NV_FORCEINLINE void
_eventbusLockQueue(EventBus *pEventBus)
{
    portSyncSpinlockAcquire(pEventBus->pQueueLock);
}

static NV_FORCEINLINE void
_eventbusUnlockQueue(EventBus *pEventBus)
{
    portSyncSpinlockRelease(pEventBus->pQueueLock);
}
#else
static NV_FORCEINLINE NV_STATUS
_eventbusQueueLockCreate(EventBus *pEventBus)
{
    PORT_UNREFERENCED_VARIABLE(pEventBus);
    return NV_OK;
}

static NV_FORCEINLINE void
_eventbusQueueLockDestroy(EventBus *pEventBus)
{
    PORT_UNREFERENCED_VARIABLE(pEventBus);
}

static NV_FORCEINLINE void
_eventbusLockQueue(EventBus *pEventBus)
{
    PORT_UNREFERENCED_VARIABLE(pEventBus);
}

static NV_FORCEINLINE void
_eventbusUnlockQueue(EventBus *pEventBus)
{
    PORT_UNREFERENCED_VARIABLE(pEventBus);
}
#endif

static NV_FORCEINLINE void
_eventbusLockScope(EventBus *pEventBus)
{
    if (pEventBus->callbacks.lockScope != NULL)
        pEventBus->callbacks.lockScope(pEventBus);
}

static NV_FORCEINLINE void
_eventbusUnlockScope(EventBus *pEventBus)
{
    if (pEventBus->callbacks.unlockScope != NULL)
        pEventBus->callbacks.unlockScope(pEventBus);
}

static NV_FORCEINLINE void
_eventbusReportRunnableEvent(EventBus *pEventBus, Event *pEvent)
{
    if (pEventBus->callbacks.reportRunnableEvent != NULL)
        pEventBus->callbacks.reportRunnableEvent(pEventBus, pEvent);
}

static NV_FORCEINLINE void
_eventbusInitEventGroup(EventBus *pEventBus, EventGroup *pEventGroup, Event *pEvent)
{
    if (pEventBus->callbacks.initEventGroup != NULL)
        pEventBus->callbacks.initEventGroup(pEventBus, pEventGroup, pEvent);
}

static void
_eventbusFlushOrReportRunnableEvent(EventBus *pEventBus, Event *pEvent)
{
    if (eventbusIsBlockingMode(pEventBus))
    {
        eventbusFlush(pEventBus);
        return;
    }

    _eventbusReportRunnableEvent(pEventBus, pEvent);
}

static Event *
_eventbusFirstRunnableEvent(EventGroup *pEventGroup)
{
    Event *pEvent;

    for (pEvent = eventgroupFirstEvent(pEventGroup);
         pEvent != NULL;
         pEvent = eventgroupNextEvent(pEventGroup, pEvent))
    {
        if (!eventIsPaused(pEvent) && !eventIsDispatched(pEvent))
            return pEvent;
    }

    return NULL;
}

static NV_FORCEINLINE void
_eventbusReportDroppedEvent(EventBus *pEventBus, Event *pEvent, NV_STATUS status)
{
    if (pEventBus->callbacks.reportDroppedEvent != NULL)
        pEventBus->callbacks.reportDroppedEvent(pEventBus, pEvent, status);
}

NV_STATUS eventbusConstruct_IMPL(
    EventBus *pEventBus,
    EVENT_BUS_CONTEXT *pContext,
    EVENT_BUS_CALLBACKS *pCallbacks)
{
    NV_STATUS status;

    NV_ASSERT_OR_RETURN(pEventBus != NULL, NV_ERR_INVALID_POINTER);

    // Store the context and callbacks
    pEventBus->ctx = pContext;

    //
    // Always initialize the callbacks struct: copy from the caller when provided,
    // otherwise zero it out so the NULL function-pointer checks in the dispatch
    // path are valid (the NVOC allocator's zero-init guarantee is not relied on).
    //
    if (pCallbacks != NULL)
        pEventBus->callbacks = *pCallbacks;
    else
        portMemSet(&pEventBus->callbacks, 0, sizeof(pEventBus->callbacks));

    // Initialize the subscription multimap (priority -> eventId -> handlers)
    multimapInit(&pEventBus->subscriptions, portMemAllocatorGetGlobalNonPaged());

    status = _eventbusQueueLockCreate(pEventBus);
    if (status != NV_OK)
    {
        multimapDestroy(&pEventBus->subscriptions);
        return status;
    }

    // Initialize the pending event group queue
    listInitIntrusive(&pEventBus->pendingGroups);

    pEventBus->pActiveGroup = NULL;
    pEventBus->bDispatching = NV_FALSE;
    pEventBus->bBlockingMode = NV_FALSE;
    pEventBus->bIteratingSubscriptionList = NV_FALSE;

    return NV_OK;
}

void eventbusDestruct_IMPL(EventBus *pEventBus)
{
    EventSubscriptionMapIter it;
    EventGroup *pPendingGroup;

    if (pEventBus == NULL)
        return;

    //
    // Destroy any pending event groups still queued at teardown. The bus owns
    // the lifetime of groups it accepts via publish, so events that were never
    // flushed would otherwise leak (the group, in turn, owns its events).
    // The active group is similarly owned and destroyed if dispatch was
    // interrupted before completion.
    //
    pPendingGroup = listHead(&pEventBus->pendingGroups);
    while (pPendingGroup != NULL)
    {
        EventGroup *pNext = listNext(&pEventBus->pendingGroups, pPendingGroup);
        listRemove(&pEventBus->pendingGroups, pPendingGroup);
        objDelete(pPendingGroup);
        pPendingGroup = pNext;
    }

    if (pEventBus->pActiveGroup != NULL)
    {
        objDelete(pEventBus->pActiveGroup);
        pEventBus->pActiveGroup = NULL;
    }

    //
    // Free all subscription nodes we allocated, then destroy the multimap (which
    // frees the map/submap nodes).
    //
    for (it = multimapItemIterAll(&pEventBus->subscriptions);
         multimapItemIterNext(&it);)
    {
        EventSubscriptionList *pSubList = it.pValue;
        EventSubscription *pSub = pSubList->pHead;
        while (pSub != NULL)
        {
            EventSubscription *pNext = pSub->pNext;
            portMemFree(pSub);
            pSub = pNext;
        }
        pSubList->pHead = NULL;
    }

    multimapDestroy(&pEventBus->subscriptions);

    _eventbusQueueLockDestroy(pEventBus);

    pEventBus->bDispatching = NV_FALSE;
}

EVENT_BUS_CONTEXT *eventbusGetContext_IMPL(EventBus *pEventBus)
{
    NV_ASSERT_OR_RETURN(pEventBus != NULL, NULL);
    return pEventBus->ctx;
}

void eventbusEnterBlockingMode_IMPL(EventBus *pEventBus)
{
    NV_ASSERT_OR_RETURN_VOID(pEventBus != NULL);

    _eventbusLockQueue(pEventBus);
    pEventBus->bBlockingMode = NV_TRUE;
    _eventbusUnlockQueue(pEventBus);
}

NvBool eventbusIsBlockingMode_IMPL(EventBus *pEventBus)
{
    NvBool bBlockingMode;

    NV_ASSERT_OR_RETURN(pEventBus != NULL, NV_FALSE);

    _eventbusLockQueue(pEventBus);
    bBlockingMode = pEventBus->bBlockingMode;
    _eventbusUnlockQueue(pEventBus);

    return bBlockingMode;
}

NV_STATUS eventbusSubscribe_IMPL(
    EventBus *pEventBus,
    NvU32 eventId,
    NvU32 priority,
    NvocEventHandler handlerFn,
    NvocEventFilter filterFn,
    Object *pHandler)
{
    NvU64 submapKey;
    EventSubscriptionList *pSubList;
    EventSubscription *pNewSub;
    NV_STATUS status = NV_OK;

    NV_ASSERT_OR_RETURN(pEventBus != NULL, NV_ERR_INVALID_POINTER);
    NV_ASSERT_OR_RETURN(handlerFn != NULL, NV_ERR_INVALID_POINTER);

    submapKey = eventbusPriorityToSubmapKey(priority);

    //
    // Take the bus's scope lock so the subscription map mutation here is
    // serialized against the dispatch loop in _eventbusDispatchEvent (which
    // also runs under scope lock) and against concurrent (un)subscribers.
    // A subscriber on a different thread may freely subscribe while another
    // thread is dispatching: the call simply blocks here until the in-flight
    // group has finished.
    //
    _eventbusLockScope(pEventBus);

    //
    // What is NOT supported is a handler running on the dispatch thread
    // calling eventbusSubscribe(): the dispatch loop is currently walking
    // the subscription map and an insertSubmap / insertItem here would race
    // with that iteration. Same-thread reentry is what bIteratingSubscriptionList
    // catches; cross-thread callers will already have blocked on scope lock
    // above and observe NV_FALSE here.
    //
    // The guard fires its assert in checked builds and bails out cleanly
    // (status + goto done so we still release the scope lock) in release
    // builds where NV_ASSERT may be a no-op, instead of falling through and
    // mutating the subscription map under the dispatcher's iterator.
    //
    NV_ASSERT_OR_ELSE(!pEventBus->bIteratingSubscriptionList,
    {
        status = NV_ERR_INVALID_STATE;
        goto done;
    });

    // Find or create the subscription list for this (priority, eventId) pair
    pSubList = multimapFindItem(&pEventBus->subscriptions, submapKey, eventId);
    if (pSubList == NULL)
    {
        //
        // multimapInsertItemNew requires the submap (priority bucket) to already exist.
        // Create it on-demand the first time we see a new priority level. Track
        // whether we created it so we can roll the submap back if the item
        // insertion below then fails: an empty bucket left in the multimap is
        // benign at runtime but gets uselessly traversed by every dispatch
        // until bus teardown.
        //
        NvBool bSubmapCreatedHere = NV_FALSE;
        if (multimapFindSubmap(&pEventBus->subscriptions, submapKey) == NULL)
        {
            if (multimapInsertSubmap(&pEventBus->subscriptions, submapKey) == NULL)
            {
                status = NV_ERR_NO_MEMORY;
                goto done;
            }
            bSubmapCreatedHere = NV_TRUE;
        }

        pSubList = multimapInsertItemNew(&pEventBus->subscriptions, submapKey, eventId);
        if (pSubList == NULL)
        {
            if (bSubmapCreatedHere)
            {
                EventSubscriptionMapSubmap *pSubmap =
                    multimapFindSubmap(&pEventBus->subscriptions, submapKey);
                if (pSubmap != NULL)
                    multimapRemoveSubmap(&pEventBus->subscriptions, pSubmap);
            }
            status = NV_ERR_NO_MEMORY;
            goto done;
        }
        pSubList->pHead = NULL;
    }

    // Allocate and initialize the new subscription
    pNewSub = portMemAllocNonPaged(sizeof(EventSubscription));
    if (pNewSub == NULL)
    {
        // Clean up empty list entry if we just created it
        if (pSubList->pHead == NULL)
            multimapRemoveItem(&pEventBus->subscriptions, pSubList);
        status = NV_ERR_NO_MEMORY;
        goto done;
    }

    pNewSub->handlerFn = handlerFn;
    pNewSub->filterFn = filterFn;
    pNewSub->pHandler = pHandler;

    // Insert at head of the subscription list
    pNewSub->pNext = pSubList->pHead;
    pSubList->pHead = pNewSub;

done:
    _eventbusUnlockScope(pEventBus);
    return status;
}

void eventbusUnsubscribe_IMPL(
    EventBus *pEventBus,
    NvU32 eventId,
    NvU32 priority,
    NvocEventHandler handlerFn,
    NvocEventFilter filterFn,
    Object *pHandler)
{
    NvU64 submapKey;
    EventSubscriptionList *pSubList;
    EventSubscription **ppIter;

    NV_ASSERT_OR_RETURN_VOID(pEventBus != NULL);

    submapKey = eventbusPriorityToSubmapKey(priority);

    //
    // Take the scope lock so we serialize against the dispatch loop and any
    // concurrent (un)subscriber. Cross-thread teardown of a subscriber while
    // dispatch is in flight is fully supported: this call just blocks here
    // until the active group's dispatch (including any in-flight handler
    // invocation) has returned. Once eventbusUnsubscribe() returns, the
    // subscription is gone and no future dispatch can call its handler, so
    // callers may safely free the subscriber object.
    //
    _eventbusLockScope(pEventBus);

    //
    // What is NOT supported is a handler running on the dispatch thread
    // calling eventbusUnsubscribe() to remove its own (or a peer's)
    // subscription: the dispatch loop is currently walking the subscription
    // list and any unlink/free here would dangle its in-flight pSub /
    // pSub->pNext pointers. Same-thread reentry is what
    // bIteratingSubscriptionList catches; cross-thread callers will already
    // have blocked on scope lock above and observe NV_FALSE here.
    //
    // The guard fires its assert in checked builds and bails out cleanly
    // (goto done so we still release the scope lock) in release builds
    // where NV_ASSERT may be a no-op, instead of falling through and
    // freeing a node the dispatcher is still iterating.
    //
    NV_ASSERT_OR_GOTO(!pEventBus->bIteratingSubscriptionList, done);

    pSubList = multimapFindItem(&pEventBus->subscriptions, submapKey, eventId);
    if (pSubList == NULL)
        goto done;

    ppIter = &pSubList->pHead;
    while (*ppIter != NULL)
    {
        EventSubscription *pSub = *ppIter;
        if (pSub->handlerFn == handlerFn &&
            pSub->filterFn == filterFn &&
            pSub->pHandler == pHandler)
        {
            *ppIter = pSub->pNext;
            portMemFree(pSub);

            // Remove the multimap entry if the list is now empty
            if (pSubList->pHead == NULL)
                multimapRemoveItem(&pEventBus->subscriptions, pSubList);
            break;
        }
        ppIter = &pSub->pNext;
    }

done:
    _eventbusUnlockScope(pEventBus);
}

/**
 * @brief Dispatch a single event to all matching handlers.
 *
 * This implements the inner dispatch loop:
 * For each registered priority level (highest first), for each relative class ID
 * in the event's CASTINFO (most-derived first), call matching handlers.
 *
 * Processing most-derived first allows specific event handlers to intercept
 * and pause the event before more general base class handlers run.
 *
 * Complexity: O(p * m * log n) where:
 *   p = number of distinct priority levels with subscriptions on this event bus
 *   m = number of classes in the event's inheritance hierarchy
 *   n = total number of subscriptions
 */
static NV_STATUS _eventbusDispatchEvent(EventBus *pEventBus, Event *pEvent)
{
    EventSubscriptionMapSupermapIter submapIt;
    Dynamic *pDerivedEvent;
    const struct NVOC_RTTI *pRtti;
    const struct NVOC_CLASS_DEF *pClassDef;
    const struct NVOC_CASTINFO *pCastInfo;
    NvS32 i;

    //
    // IMPORTANT: pEvent is typically a pointer to the Event base subobject.
    // We must resolve to the fully-derived object before using RTTI/CASTINFO,
    // otherwise we'll only see the base Event type and miss derived handlers.
    //
    pDerivedEvent = fullyDeriveWrapper(staticCast(pEvent, Dynamic));
    pRtti = (pDerivedEvent != NULL) ? pDerivedEvent->__nvoc_rtti : NULL;
    NV_ASSERT_OR_RETURN((pRtti != NULL) &&
                        (pRtti->pClassDef != NULL) &&
                        (pRtti->pClassDef->pCastInfo != NULL),
                        NV_ERR_INVALID_STATE);

    pClassDef = pRtti->pClassDef;
    pCastInfo = pClassDef->pCastInfo;

    // Iterate through all priority levels (submaps) in ascending key order.
    // Since key = NV_U32_MAX - priority, this gives us highest priority first.
    for (submapIt = multimapSubmapIterAll(&pEventBus->subscriptions);
         multimapSubmapIterNext(&submapIt);)
    {
        EventSubscriptionMapSubmap *pSubmap = submapIt.pValue;
        NvU64 submapKey = multimapSubmapKey(&pEventBus->subscriptions, pSubmap);

        //
        // For this priority level, iterate through the event's class hierarchy
        // from most-derived to least-derived (base classes).
        // CASTINFO relatives[0] is most-derived, relatives[numRelatives-1] is least-derived.
        // This allows derived event handlers to intercept/pause before base handlers run.
        //
        for (i = 0; i < (NvS32)pCastInfo->numRelatives; i++)
        {
            const struct NVOC_RTTI *pRelative = pCastInfo->relatives[i];
            NvU32 relativeClassId;
            EventSubscriptionList *pSubList;
            EventSubscription *pSub;

            if (pRelative == NULL || pRelative->pClassDef == NULL)
                continue;

            relativeClassId = pRelative->pClassDef->classInfo.classId;

            // Lookup subscriptions at this (priority, classId)
            pSubList = multimapFindItem(&pEventBus->subscriptions, submapKey, relativeClassId);
            if (pSubList == NULL)
                continue;

            //
            // Call all handlers in this subscription list.
            //
            // Set bIteratingSubscriptionList while we walk the list so
            // eventbusSubscribe / eventbusUnsubscribe can assert against
            // the unsupported case of a handler mutating the subscription
            // map mid-dispatch (which would leave our pSub / pSub->pNext
            // pointers dangling). Cleared at every exit from this loop,
            // including the paused-event early-return below.
            //
            pEventBus->bIteratingSubscriptionList = NV_TRUE;

            for (pSub = pSubList->pHead; pSub != NULL; pSub = pSub->pNext)
            {
                NvBool bInvoke = NV_TRUE;

                if (pSub->filterFn != NULL)
                    bInvoke = pSub->filterFn(pSub->pHandler, pEvent);

                if (bInvoke)
                    pSub->handlerFn(pSub->pHandler, pEvent);

                // Check if event was paused by this handler
                if (eventIsPaused(pEvent))
                {
                    pEventBus->bIteratingSubscriptionList = NV_FALSE;
                    return NV_WARN_MORE_PROCESSING_REQUIRED;
                }
            }

            pEventBus->bIteratingSubscriptionList = NV_FALSE;
        }
    }

    return NV_OK;
}

/*!
 * @brief Dispatch all events in the active event group.
 *
 * Expected to be called with scope lock held.
 *
 * Each pass through the group's event list dispatches every event that is
 * neither paused nor already-dispatched on a prior pass. If any event is
 * still paused at the end of the pass, the group is re-queued so it will be
 * revisited once those events are resumed; on that next pass the
 * already-dispatched events are skipped via eventIsDispatched(), which
 * keeps handlers from firing twice and keeps paused events alive until they
 * are resumed and handled.
 *
 * @return NV_TRUE if the group completed (no paused events remain),
 *         NV_FALSE if the group was suspended and re-queued.
 */
static NvBool _eventbusDispatchEventGroup(EventBus *pEventBus, EventGroup *pEventGroup)
{
    Event *pEvent;
    NvBool bHasPausedEvents = NV_FALSE;

    NV_ASSERT_OR_RETURN(pEventGroup != NULL, NV_TRUE);

    //
    // Walk the group's intrusive event list one element at a time.
    // eventgroupFirstEvent/eventgroupNextEvent each take pEventGroup's
    // internal spinlock for just the navigation step, which is enough to
    // serialize against a concurrent emit-thread appending a new event at
    // the tail via eventgroupAddEvent_IMPL.
    //
    // No lock is held across _eventbusDispatchEvent() so handlers running on
    // this thread are free to re-enter the bus (publish, publishAssociated)
    // without recursive-acquiring the group's spinlock. pEventBus->bDispatching
    // guarantees that no other thread is also inside this loop, so the
    // lock-free eventIsDispatched()/eventMarkDispatched() pair is
    // single-writer; the spinlock is only needed for the list pointers.
    //
    pEvent = eventgroupFirstEvent(pEventGroup);

    while (pEvent != NULL)
    {
        Event *pNext;

        //
        // Skip already-dispatched events so a re-queued group does not
        // double-fire handlers on the next pass.
        //
        if (eventIsDispatched(pEvent))
        {
            // fall through to advance
        }
        //
        // Skip events that are currently paused. Track that the group still
        // has in-flight paused events so that the group is re-queued rather
        // than destroyed when the pass ends.
        //
        else if (eventIsPaused(pEvent))
        {
            bHasPausedEvents = NV_TRUE;
        }
        else
        {
            //
            // _eventbusDispatchEvent can fail (e.g. NV_ERR_INVALID_STATE
            // when the event's RTTI / CASTINFO is unexpectedly NULL) before
            // running any handlers. In that case eventIsPaused() will still
            // be false, so without surfacing the failure the bus would just
            // mark the event as dispatched and the error would silently
            // swallow the event in release builds. Hand the failure to the
            // bus owner via the reportDroppedEvent callback rather than
            // pulling a logging dependency into the runtime; the event is
            // still marked dispatched below so a broken event does not
            // cause the group to spin forever.
            //
            NV_STATUS dispatchStatus = _eventbusDispatchEvent(pEventBus, pEvent);
            if (dispatchStatus != NV_OK &&
                dispatchStatus != NV_WARN_MORE_PROCESSING_REQUIRED)
            {
                _eventbusReportDroppedEvent(pEventBus, pEvent, dispatchStatus);
            }

            //
            // If a handler paused the event, leave it in the group for a
            // later pass. Otherwise the event is done: mark it on both sides
            // so that (a) this loop skips it on a re-queued pass and (b) the
            // group's runnable check no longer counts it as outstanding work.
            //
            if (eventIsPaused(pEvent))
            {
                bHasPausedEvents = NV_TRUE;
            }
            else
            {
                eventMarkDispatched(pEvent);
                eventgroupEventDispatched(pEventGroup);
            }
        }

        pNext = eventgroupNextEvent(pEventGroup, pEvent);
        pEvent = pNext;
    }

    //
    // If any events in the group are still paused, re-queue the group so it
    // will be revisited once those events are resumed. Destroying the group
    // here would invalidate handler-owned references to the paused events.
    //
    if (bHasPausedEvents)
    {
        _eventbusLockQueue(pEventBus);
        listPrependExisting(&pEventBus->pendingGroups, pEventBus->pActiveGroup);
        pEventBus->pActiveGroup = NULL;
        _eventbusUnlockQueue(pEventBus);
        return NV_FALSE;
    }

    // All events handled: call the export callback if the group has export events.
    if (eventgroupHasExportEvent(pEventGroup) && pEventBus->callbacks.exportEventGroup != NULL)
    {
        pEventBus->callbacks.exportEventGroup(pEventBus, pEventGroup);
    }

    return NV_TRUE;
}

/*!
 * @brief Process all pending event groups in the queue.
 *
 * Skips groups that are not runnable (all events paused).
 * Acquires/releases scope lock for each group processed.
 */
static void _eventbusProcessPendingGroups(EventBus *pEventBus)
{
    EventGroup *pEventGroup;

    for (;;)
    {
        pEventGroup = NULL;

        // Find next runnable group under queue lock
        _eventbusLockQueue(pEventBus);

        for (pEventGroup = listHead(&pEventBus->pendingGroups);
             pEventGroup != NULL;
             pEventGroup = listNext(&pEventBus->pendingGroups, pEventGroup))
        {
            if (eventgroupIsRunnable(pEventGroup))
            {
                listRemove(&pEventBus->pendingGroups, pEventGroup);
                pEventBus->pActiveGroup = pEventGroup;
                break;
            }
        }

        _eventbusUnlockQueue(pEventBus);

        // Exit the loop if there are no runnable groups
        if (pEventGroup == NULL)
            return;

        // Acquire scope lock and dispatch
        _eventbusLockScope(pEventBus);

        if (_eventbusDispatchEventGroup(pEventBus, pEventGroup))
        {
            // Group completed
            _eventbusLockQueue(pEventBus);
            pEventBus->pActiveGroup = NULL;
            _eventbusUnlockQueue(pEventBus);

            //
            // The bus owns the lifetime of groups it processes. Destroying the group
            // also destroys all events in the group.
            //
            _eventbusUnlockScope(pEventBus);
            objDelete(pEventGroup);
            continue;
        }
        // else: group was suspended, re-queued to pending list

        _eventbusUnlockScope(pEventBus);

        // Loop to check for more runnable groups
    }
}

NV_STATUS eventbusEventPublish_IMPL(EventBus *pEventBus, Event *pEvent)
{
    EventGroup *pEventGroup;
    NV_STATUS status;

    NV_ASSERT_OR_RETURN(pEventBus != NULL, NV_ERR_INVALID_POINTER);
    NV_ASSERT_OR_RETURN(pEvent != NULL, NV_ERR_INVALID_POINTER);

    //
    // An event must be published at most once. pEvent->pEventGroup is set
    // only by this function (when we wrap the event in a fresh group below)
    // and by eventbusEventPublishAssociated() (when the event is added to
    // an existing group), and is cleared only when the owning group is
    // destroyed. A non-NULL group on entry therefore means the caller is
    // either re-publishing an already-queued event or publishing an event
    // that already belongs to some other group via PublishAssociated; both
    // would result in the same group being enqueued twice on pendingGroups
    // and dispatched twice with no error path to detect it.
    //
    NV_ASSERT_OR_RETURN(pEvent->pEventGroup == NULL, NV_ERR_INVALID_STATE);

    //
    // Create a single-event group to wrap pEvent. On failure, return the
    // status to the caller so it can destroy the event; this function never
    // touches pEvent's lifetime on the failure path. (See the contract on
    // eventbusEventPublish() in event_bus.h.)
    //
    status = objCreate(&pEventGroup, eventSource(pEvent), EventGroup, pEventBus);
    NV_ASSERT_OK_OR_RETURN(status);

    eventgroupAddEvent(pEventGroup, pEvent);
    _eventbusInitEventGroup(pEventBus, pEventGroup, pEvent);

    // Queue the group for later processing by eventbusFlush()
    _eventbusLockQueue(pEventBus);
    listAppendExisting(&pEventBus->pendingGroups, pEventGroup);
    _eventbusUnlockQueue(pEventBus);

    _eventbusFlushOrReportRunnableEvent(pEventBus, pEvent);

    return NV_OK;
}

NV_STATUS eventbusEventPublishAssociated_IMPL(
    EventBus *pEventBus,
    Event *pExistingEvent,
    Event *pEvent)
{
    NV_ASSERT_OR_RETURN(pEventBus != NULL, NV_ERR_INVALID_POINTER);
    NV_ASSERT_OR_RETURN(pExistingEvent != NULL, NV_ERR_INVALID_POINTER);
    NV_ASSERT_OR_RETURN(pEvent != NULL, NV_ERR_INVALID_POINTER);

    //
    // pExistingEvent must already belong to the same bus as the one being
    // published on, and must already belong to a group (i.e. it must have
    // been passed through eventbusEventPublish() first - the bus only
    // assigns pEventGroup on publish). pEvent must NOT already belong to
    // a group: this check is the symmetric guard that eventbusEventPublish
    // performs, since eventgroupAddEvent() would otherwise overwrite
    // pEvent->pEventGroup and re-link its intrusive list node into a
    // second group's event list, corrupting both lists.
    //
    NV_ASSERT_OR_RETURN(pExistingEvent->pEventBus == pEventBus, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(pExistingEvent->pEventGroup != NULL, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(pEvent->pEventGroup == NULL, NV_ERR_INVALID_STATE);

    //
    // Append the new event to the existing event's group. If the group is the
    // currently-dispatching active group, _eventbusDispatchEventGroup()'s
    // forward iteration will reach pEvent at the new tail of the event list
    // and dispatch it exactly once. Otherwise, pEvent will be dispatched when
    // the group is processed by a subsequent eventbusFlush(). Dispatching
    // pEvent inline here in addition would cause a second dispatch from the
    // outer loop.
    //
    eventgroupAddEvent(pExistingEvent->pEventGroup, pEvent);

    _eventbusFlushOrReportRunnableEvent(pEventBus, pEvent);

    return NV_OK;
}

void eventbusGroupResumed_IMPL(EventBus *pEventBus, EventGroup *pEventGroup)
{
    Event *pEvent;

    NV_ASSERT_OR_RETURN_VOID(pEventBus != NULL);
    NV_ASSERT_OR_RETURN_VOID(pEventGroup != NULL);

    pEvent = _eventbusFirstRunnableEvent(pEventGroup);
    if (pEvent != NULL)
        _eventbusFlushOrReportRunnableEvent(pEventBus, pEvent);
}

void eventbusFlush_IMPL(EventBus *pEventBus)
{
    NV_ASSERT_OR_RETURN_VOID(pEventBus != NULL);

    _eventbusLockQueue(pEventBus);

    // For simplicity, only one dispatcher thread is supported
    if (pEventBus->bDispatching)
    {
        _eventbusUnlockQueue(pEventBus);
        return;
    }

    pEventBus->bDispatching = NV_TRUE;
    _eventbusUnlockQueue(pEventBus);

    _eventbusProcessPendingGroups(pEventBus);

    _eventbusLockQueue(pEventBus);
    pEventBus->bDispatching = NV_FALSE;
    if (pEventBus->bBlockingMode)
    {
        NV_ASSERT(listHead(&pEventBus->pendingGroups) == NULL);
        NV_ASSERT(pEventBus->pActiveGroup == NULL);
    }
    _eventbusUnlockQueue(pEventBus);
}
