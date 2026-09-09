
#ifndef _G_EVENT_BUS_NVOC_H_
#define _G_EVENT_BUS_NVOC_H_

// Version of generated metadata structures
#ifdef NVOC_METADATA_VERSION
#undef NVOC_METADATA_VERSION
#endif
#define NVOC_METADATA_VERSION 2

#include "nvoc/runtime.h"
#include "nvoc/rtti.h"

#ifdef __cplusplus
extern "C" {
#endif

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

#pragma once

#include "g_event_bus_nvoc.h"

#ifndef _NVOC_EVENT_BUS_H_
#define _NVOC_EVENT_BUS_H_

#include "nvtypes.h"
#include "nvstatus.h"
#include "nvCpuUuid.h"
#include "nvport/nvport.h"
#include "containers/multimap.h"
#include "containers/list.h"
#include "nvoc/object.h"
#include "nvoc/event_group.h"

/*!
 * Opaque type that can be defined by the event bus owner to include any additional bus-wide
 * context required in callbacks.
 */
typedef struct EVENT_BUS_CONTEXT EVENT_BUS_CONTEXT;

struct Event;

#ifndef __nvoc_class_id_Event
#define __nvoc_class_id_Event 0xa4ecfcu
typedef struct Event Event;
#endif /* __nvoc_class_id_Event */


struct EventGroup;

#ifndef __nvoc_class_id_EventGroup
#define __nvoc_class_id_EventGroup 0x5e0b7cu
typedef struct EventGroup EventGroup;
#endif /* __nvoc_class_id_EventGroup */



/*!
 * Optional event bus owner callbacks for integrating bus telemetry and adapting to other
 * event sinks.
 */
typedef struct
{
    /*!
     * @brief Export function to call on each EventGroup that completes handling on the event bus.
     *
     * Only EventGroups that contain an event with 'export' visibility will reach this function.
     * This callback can be used to serialize the event structure for external event sinks.
     */
    void (*exportEventGroup)(struct EventBus *, struct EventGroup *);

    /*!
     * @brief Initialize a newly-created EventGroup after its first event is added.
     *
     * Invoked during publish before the group is reported runnable. Owners can
     * stamp generic group-level state such as a trace ID while the group is
     * observable by handlers through @ref Event::pEventGroup.
     *
     * If NULL, the runtime does no owner-specific group initialization.
     */
    void (*initEventGroup)(struct EventBus *, struct EventGroup *, struct Event *);

    /*!
     * @brief Acquire the scope lock before processing an event group.
     *
     * This callback is invoked before any event handlers are called for a group.
     * The owner should acquire whatever lock is needed to protect shared state
     * that event handlers may access.
     *
     * If NULL, no scope locking is performed (single-threaded mode).
     */
    void (*lockScope)(struct EventBus *);

    /*!
     * @brief Release the scope lock after processing an event group.
     *
     * This callback is invoked after all event handlers have been called for a group,
     * or when processing is suspended due to an event being paused.
     *
     * If NULL, no scope locking is performed (single-threaded mode).
     */
    void (*unlockScope)(struct EventBus *);

    /*!
     * @brief Report an event that is runnable on the bus.
     *
     * Invoked after the bus takes ownership of a newly published event, and
     * when a suspended group resumes with dispatchable work. Owners can use
     * this hook to wake an event thread or schedule deferred work that calls
     * eventbusFlush().
     *
     * If NULL, runnable work is processed only when a caller explicitly invokes
     * eventbusFlush().
     */
    void (*reportRunnableEvent)(struct EventBus *, struct Event *);

    /*!
     * @brief Report an event that the bus could not dispatch.
     *
     * Invoked by the bus when its inner dispatch routine returns an error
     * (e.g. NV_ERR_INVALID_STATE because the event's RTTI / CASTINFO was
     * unexpectedly NULL) before any subscribers ran. The event is still
     * marked dispatched after this callback returns so a broken event does
     * not cause the group to spin forever; this hook lets the bus owner
     * surface the failure through whatever logging or telemetry channel
     * fits its environment, instead of forcing the NVOC runtime to depend
     * on a specific logging API.
     *
     * NV_WARN_MORE_PROCESSING_REQUIRED (the normal handler-paused signal)
     * does NOT trigger this callback.
     *
     * If NULL, the dropped event is silently discarded.
     */
    void (*reportDroppedEvent)(struct EventBus *, struct Event *, NV_STATUS);
} EVENT_BUS_CALLBACKS;

typedef void (*NvocEventHandler)(struct Object *, struct Event *);
typedef NvBool (*NvocEventFilter)(struct Object *, struct Event *);

/*!
 * @brief A single subscription entry containing the handler and filter.
 */
typedef struct EventSubscription
{
    NvocEventHandler handlerFn;
    NvocEventFilter filterFn;
    struct Object *pHandler;
    struct EventSubscription *pNext;
} EventSubscription;

/*!
 * @brief Subscription list for a specific (priority, eventId) combination.
 *
 * These are stored in a multimap where:
 *   - Primary key (submap): NV_U32_MAX - priority (higher priorities have lower keys)
 *   - Secondary key (item): eventId/classId
 *
 * This ensures higher priorities come first when iterating submaps in ascending order.
 */
typedef struct EventSubscriptionList
{
    EventSubscription *pHead;
} EventSubscriptionList;

// Multimap: priority -> (eventId -> EventSubscriptionList)
// Primary key is (NV_U32_MAX - priority) so higher priorities iterate first
MAKE_MULTIMAP(EventSubscriptionMap, EventSubscriptionList);

/*!
 * @brief Convert priority to submap key.
 *
 * Higher priorities result in lower keys, so ascending iteration gives
 * highest priority first.
 */
static NV_FORCEINLINE NvU64
eventbusPriorityToSubmapKey(NvU32 priority)
{
    return (NvU64)(NV_U32_MAX - priority);
}

/*!
 * @brief Extract priority from a submap key.
 */
static NV_FORCEINLINE NvU32
eventbusSubmapKeyToPriority(NvU64 submapKey)
{
    return NV_U32_MAX - (NvU32)submapKey;
}

// Forward declare the intrusive list type for EventGroup
// The actual list node (queueNode) is embedded in EventGroup
MAKE_INTRUSIVE_LIST(EventGroupQueue, EventGroup, queueNode);


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_EVENT_BUS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__EventBus;
struct NVOC_METADATA__Object;


struct EventBus {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__EventBus *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct Object __nvoc_base_Object;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super
    struct EventBus *__nvoc_pbase_EventBus;    // eventbus

    // Data members
    EVENT_BUS_CONTEXT *PRIVATE_FIELD(ctx);
    EVENT_BUS_CALLBACKS PRIVATE_FIELD(callbacks);
    EventSubscriptionMap PRIVATE_FIELD(subscriptions);
    PORT_SPINLOCK *PRIVATE_FIELD(pQueueLock);
    EventGroupQueue PRIVATE_FIELD(pendingGroups);
    struct EventGroup *PRIVATE_FIELD(pActiveGroup);
    NvBool PRIVATE_FIELD(bDispatching);
    NvBool PRIVATE_FIELD(bBlockingMode);
    NvBool PRIVATE_FIELD(bIteratingSubscriptionList);
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__EventBus {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__Object metadata__Object;
};

#ifndef __nvoc_class_id_EventBus
#define __nvoc_class_id_EventBus 0x2e5918u
typedef struct EventBus EventBus;
#endif /* __nvoc_class_id_EventBus */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_EventBus;

#define __staticCast_EventBus(pThis) \
    ((pThis)->__nvoc_pbase_EventBus)

#ifdef __nvoc_event_bus_h_disabled
#define __dynamicCast_EventBus(pThis) ((EventBus*) NULL)
#else //__nvoc_event_bus_h_disabled
#define __dynamicCast_EventBus(pThis) \
    ((EventBus*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(EventBus)))
#endif //__nvoc_event_bus_h_disabled

NV_STATUS __nvoc_objCreateDynamic_EventBus(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_EventBus(EventBus**, Dynamic*, NvU32, EVENT_BUS_CONTEXT *pContext, EVENT_BUS_CALLBACKS *pCallbacks);
#define __objCreate_EventBus(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pContext, pCallbacks) \
    __nvoc_objCreate_EventBus((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pContext, pCallbacks)


// Wrapper macros for implementation functions
NV_STATUS eventbusConstruct_IMPL(struct EventBus *pEventBus, EVENT_BUS_CONTEXT *pContext, EVENT_BUS_CALLBACKS *pCallbacks);
#define __nvoc_eventbusConstruct(pEventBus, pContext, pCallbacks) eventbusConstruct_IMPL(pEventBus, pContext, pCallbacks)

void eventbusDestruct_IMPL(struct EventBus *pEventBus);
#define __nvoc_eventbusDestruct(pEventBus) eventbusDestruct_IMPL(pEventBus)

EVENT_BUS_CONTEXT * eventbusGetContext_IMPL(struct EventBus *pEventBus);
#ifdef __nvoc_event_bus_h_disabled
static inline EVENT_BUS_CONTEXT * eventbusGetContext(struct EventBus *pEventBus) {
    NV_ASSERT_FAILED_PRECOMP("EventBus was disabled!");
    return NULL;
}
#else // __nvoc_event_bus_h_disabled
#define eventbusGetContext(pEventBus) eventbusGetContext_IMPL(pEventBus)
#endif // __nvoc_event_bus_h_disabled

NV_STATUS eventbusSubscribe_IMPL(struct EventBus *pEventBus, NvU32 eventId, NvU32 priority, NvocEventHandler handlerFn, NvocEventFilter filterFn, struct Object *pHandler);
#ifdef __nvoc_event_bus_h_disabled
static inline NV_STATUS eventbusSubscribe(struct EventBus *pEventBus, NvU32 eventId, NvU32 priority, NvocEventHandler handlerFn, NvocEventFilter filterFn, struct Object *pHandler) {
    NV_ASSERT_FAILED_PRECOMP("EventBus was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_event_bus_h_disabled
#define eventbusSubscribe(pEventBus, eventId, priority, handlerFn, filterFn, pHandler) eventbusSubscribe_IMPL(pEventBus, eventId, priority, handlerFn, filterFn, pHandler)
#endif // __nvoc_event_bus_h_disabled

void eventbusUnsubscribe_IMPL(struct EventBus *pEventBus, NvU32 eventId, NvU32 priority, NvocEventHandler handlerFn, NvocEventFilter filterFn, struct Object *pHandler);
#ifdef __nvoc_event_bus_h_disabled
static inline void eventbusUnsubscribe(struct EventBus *pEventBus, NvU32 eventId, NvU32 priority, NvocEventHandler handlerFn, NvocEventFilter filterFn, struct Object *pHandler) {
    NV_ASSERT_FAILED_PRECOMP("EventBus was disabled!");
}
#else // __nvoc_event_bus_h_disabled
#define eventbusUnsubscribe(pEventBus, eventId, priority, handlerFn, filterFn, pHandler) eventbusUnsubscribe_IMPL(pEventBus, eventId, priority, handlerFn, filterFn, pHandler)
#endif // __nvoc_event_bus_h_disabled

NV_STATUS eventbusEventPublish_IMPL(struct EventBus *pEventBus, struct Event *pEvent);
#ifdef __nvoc_event_bus_h_disabled
static inline NV_STATUS eventbusEventPublish(struct EventBus *pEventBus, struct Event *pEvent) {
    NV_ASSERT_FAILED_PRECOMP("EventBus was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_event_bus_h_disabled
#define eventbusEventPublish(pEventBus, pEvent) eventbusEventPublish_IMPL(pEventBus, pEvent)
#endif // __nvoc_event_bus_h_disabled

NV_STATUS eventbusEventPublishAssociated_IMPL(struct EventBus *pEventBus, struct Event *pExistingEvent, struct Event *pEvent);
#ifdef __nvoc_event_bus_h_disabled
static inline NV_STATUS eventbusEventPublishAssociated(struct EventBus *pEventBus, struct Event *pExistingEvent, struct Event *pEvent) {
    NV_ASSERT_FAILED_PRECOMP("EventBus was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_event_bus_h_disabled
#define eventbusEventPublishAssociated(pEventBus, pExistingEvent, pEvent) eventbusEventPublishAssociated_IMPL(pEventBus, pExistingEvent, pEvent)
#endif // __nvoc_event_bus_h_disabled

void eventbusGroupResumed_IMPL(struct EventBus *pEventBus, struct EventGroup *pEventGroup);
#ifdef __nvoc_event_bus_h_disabled
static inline void eventbusGroupResumed(struct EventBus *pEventBus, struct EventGroup *pEventGroup) {
    NV_ASSERT_FAILED_PRECOMP("EventBus was disabled!");
}
#else // __nvoc_event_bus_h_disabled
#define eventbusGroupResumed(pEventBus, pEventGroup) eventbusGroupResumed_IMPL(pEventBus, pEventGroup)
#endif // __nvoc_event_bus_h_disabled

void eventbusEnterBlockingMode_IMPL(struct EventBus *pEventBus);
#ifdef __nvoc_event_bus_h_disabled
static inline void eventbusEnterBlockingMode(struct EventBus *pEventBus) {
    NV_ASSERT_FAILED_PRECOMP("EventBus was disabled!");
}
#else // __nvoc_event_bus_h_disabled
#define eventbusEnterBlockingMode(pEventBus) eventbusEnterBlockingMode_IMPL(pEventBus)
#endif // __nvoc_event_bus_h_disabled

NvBool eventbusIsBlockingMode_IMPL(struct EventBus *pEventBus);
#ifdef __nvoc_event_bus_h_disabled
static inline NvBool eventbusIsBlockingMode(struct EventBus *pEventBus) {
    NV_ASSERT_FAILED_PRECOMP("EventBus was disabled!");
    return NV_FALSE;
}
#else // __nvoc_event_bus_h_disabled
#define eventbusIsBlockingMode(pEventBus) eventbusIsBlockingMode_IMPL(pEventBus)
#endif // __nvoc_event_bus_h_disabled

void eventbusFlush_IMPL(struct EventBus *pEventBus);
#ifdef __nvoc_event_bus_h_disabled
static inline void eventbusFlush(struct EventBus *pEventBus) {
    NV_ASSERT_FAILED_PRECOMP("EventBus was disabled!");
}
#else // __nvoc_event_bus_h_disabled
#define eventbusFlush(pEventBus) eventbusFlush_IMPL(pEventBus)
#endif // __nvoc_event_bus_h_disabled


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


// Provide aliases for NVOC-generated code until nvh-trans can be fixed to call the functions
#define eventBusSubscribe(pEventBus, eventId, priority, handlerFn, filterFn, pHandler) \
    eventbusSubscribe(pEventBus, eventId, priority, handlerFn, filterFn, pHandler)
#define eventBusUnsubscribe(pEventBus, eventId, priority, handlerFn, filterFn, pHandler) \
    eventbusUnsubscribe(pEventBus, eventId, priority, handlerFn, filterFn, pHandler)

#endif // _NVOC_EVENT_BUS_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_EVENT_BUS_NVOC_H_
