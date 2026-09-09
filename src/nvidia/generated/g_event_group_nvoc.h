
#ifndef _G_EVENT_GROUP_NVOC_H_
#define _G_EVENT_GROUP_NVOC_H_

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

#include "g_event_group_nvoc.h"

#ifndef _NVOC_EVENT_GROUP_H_
#define _NVOC_EVENT_GROUP_H_

#include "nvtypes.h"
#include "nvstatus.h"
#include "nvport/nvport.h"
#include "containers/list.h"
#include "nvoc/event.h"
#include "nvoc/object.h"

struct EventBus;

#ifndef __nvoc_class_id_EventBus
#define __nvoc_class_id_EventBus 0x2e5918u
typedef struct EventBus EventBus;
#endif /* __nvoc_class_id_EventBus */



// Intrusive list of events using the eventListNode field
MAKE_INTRUSIVE_LIST(EventList, Event, eventListNode);


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_EVENT_GROUP_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__EventGroup;
struct NVOC_METADATA__Object;


struct EventGroup {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__EventGroup *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct Object __nvoc_base_Object;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super
    struct EventGroup *__nvoc_pbase_EventGroup;    // eventgroup

    // Data members
    struct EventBus *PRIVATE_FIELD(pEventBus);
    PORT_SPINLOCK *PRIVATE_FIELD(pLock);
    EventList PRIVATE_FIELD(events);
    NvBool PRIVATE_FIELD(bHasExportEvent);
    NvU32 PRIVATE_FIELD(eventCount);
    NvU32 PRIVATE_FIELD(pausedEventCount);
    NvU32 PRIVATE_FIELD(dispatchedEventCount);
    NvU64 PRIVATE_FIELD(traceId);
    NvU8 PRIVATE_FIELD(groupAttributes);
    struct ListNode queueNode;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__EventGroup {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__Object metadata__Object;
};

#ifndef __nvoc_class_id_EventGroup
#define __nvoc_class_id_EventGroup 0x5e0b7cu
typedef struct EventGroup EventGroup;
#endif /* __nvoc_class_id_EventGroup */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_EventGroup;

#define __staticCast_EventGroup(pThis) \
    ((pThis)->__nvoc_pbase_EventGroup)

#ifdef __nvoc_event_group_h_disabled
#define __dynamicCast_EventGroup(pThis) ((EventGroup*) NULL)
#else //__nvoc_event_group_h_disabled
#define __dynamicCast_EventGroup(pThis) \
    ((EventGroup*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(EventGroup)))
#endif //__nvoc_event_group_h_disabled

NV_STATUS __nvoc_objCreateDynamic_EventGroup(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_EventGroup(EventGroup**, Dynamic*, NvU32, struct EventBus *pEventBus);
#define __objCreate_EventGroup(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus) \
    __nvoc_objCreate_EventGroup((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus)


// Wrapper macros for implementation functions
NV_STATUS eventgroupConstruct_IMPL(struct EventGroup *pEventGroup, struct EventBus *pEventBus);
#define __nvoc_eventgroupConstruct(pEventGroup, pEventBus) eventgroupConstruct_IMPL(pEventGroup, pEventBus)

void eventgroupDestruct_IMPL(struct EventGroup *pEventGroup);
#define __nvoc_eventgroupDestruct(pEventGroup) eventgroupDestruct_IMPL(pEventGroup)

const struct Event * eventgroupFirstExportEvent_IMPL(struct EventGroup *pEventGroup);
#ifdef __nvoc_event_group_h_disabled
static inline const struct Event * eventgroupFirstExportEvent(struct EventGroup *pEventGroup) {
    NV_ASSERT_FAILED_PRECOMP("EventGroup was disabled!");
    return NULL;
}
#else // __nvoc_event_group_h_disabled
#define eventgroupFirstExportEvent(pEventGroup) eventgroupFirstExportEvent_IMPL(pEventGroup)
#endif // __nvoc_event_group_h_disabled

const struct Event * eventgroupNextExportEvent_IMPL(struct EventGroup *pEventGroup, const struct Event *pEvent);
#ifdef __nvoc_event_group_h_disabled
static inline const struct Event * eventgroupNextExportEvent(struct EventGroup *pEventGroup, const struct Event *pEvent) {
    NV_ASSERT_FAILED_PRECOMP("EventGroup was disabled!");
    return NULL;
}
#else // __nvoc_event_group_h_disabled
#define eventgroupNextExportEvent(pEventGroup, pEvent) eventgroupNextExportEvent_IMPL(pEventGroup, pEvent)
#endif // __nvoc_event_group_h_disabled

const struct Event * eventgroupFirstEventOfType_IMPL(struct EventGroup *pEventGroup, NvU32 eventId);
#ifdef __nvoc_event_group_h_disabled
static inline const struct Event * eventgroupFirstEventOfType(struct EventGroup *pEventGroup, NvU32 eventId) {
    NV_ASSERT_FAILED_PRECOMP("EventGroup was disabled!");
    return NULL;
}
#else // __nvoc_event_group_h_disabled
#define eventgroupFirstEventOfType(pEventGroup, eventId) eventgroupFirstEventOfType_IMPL(pEventGroup, eventId)
#endif // __nvoc_event_group_h_disabled

const struct Event * eventgroupNextEventOfType_IMPL(struct EventGroup *pEventGroup, const struct Event *pEvent, NvU32 eventId);
#ifdef __nvoc_event_group_h_disabled
static inline const struct Event * eventgroupNextEventOfType(struct EventGroup *pEventGroup, const struct Event *pEvent, NvU32 eventId) {
    NV_ASSERT_FAILED_PRECOMP("EventGroup was disabled!");
    return NULL;
}
#else // __nvoc_event_group_h_disabled
#define eventgroupNextEventOfType(pEventGroup, pEvent, eventId) eventgroupNextEventOfType_IMPL(pEventGroup, pEvent, eventId)
#endif // __nvoc_event_group_h_disabled

struct Event * eventgroupFirstEvent_IMPL(struct EventGroup *pEventGroup);
#ifdef __nvoc_event_group_h_disabled
static inline struct Event * eventgroupFirstEvent(struct EventGroup *pEventGroup) {
    NV_ASSERT_FAILED_PRECOMP("EventGroup was disabled!");
    return NULL;
}
#else // __nvoc_event_group_h_disabled
#define eventgroupFirstEvent(pEventGroup) eventgroupFirstEvent_IMPL(pEventGroup)
#endif // __nvoc_event_group_h_disabled

struct Event * eventgroupNextEvent_IMPL(struct EventGroup *pEventGroup, struct Event *pEvent);
#ifdef __nvoc_event_group_h_disabled
static inline struct Event * eventgroupNextEvent(struct EventGroup *pEventGroup, struct Event *pEvent) {
    NV_ASSERT_FAILED_PRECOMP("EventGroup was disabled!");
    return NULL;
}
#else // __nvoc_event_group_h_disabled
#define eventgroupNextEvent(pEventGroup, pEvent) eventgroupNextEvent_IMPL(pEventGroup, pEvent)
#endif // __nvoc_event_group_h_disabled

NvBool eventgroupHasExportEvent_IMPL(struct EventGroup *pEventGroup);
#ifdef __nvoc_event_group_h_disabled
static inline NvBool eventgroupHasExportEvent(struct EventGroup *pEventGroup) {
    NV_ASSERT_FAILED_PRECOMP("EventGroup was disabled!");
    return NV_FALSE;
}
#else // __nvoc_event_group_h_disabled
#define eventgroupHasExportEvent(pEventGroup) eventgroupHasExportEvent_IMPL(pEventGroup)
#endif // __nvoc_event_group_h_disabled

void eventgroupEventPaused_IMPL(struct EventGroup *pEventGroup);
#ifdef __nvoc_event_group_h_disabled
static inline void eventgroupEventPaused(struct EventGroup *pEventGroup) {
    NV_ASSERT_FAILED_PRECOMP("EventGroup was disabled!");
}
#else // __nvoc_event_group_h_disabled
#define eventgroupEventPaused(pEventGroup) eventgroupEventPaused_IMPL(pEventGroup)
#endif // __nvoc_event_group_h_disabled

void eventgroupEventResumed_IMPL(struct EventGroup *pEventGroup);
#ifdef __nvoc_event_group_h_disabled
static inline void eventgroupEventResumed(struct EventGroup *pEventGroup) {
    NV_ASSERT_FAILED_PRECOMP("EventGroup was disabled!");
}
#else // __nvoc_event_group_h_disabled
#define eventgroupEventResumed(pEventGroup) eventgroupEventResumed_IMPL(pEventGroup)
#endif // __nvoc_event_group_h_disabled

void eventgroupEventDispatched_IMPL(struct EventGroup *pEventGroup);
#ifdef __nvoc_event_group_h_disabled
static inline void eventgroupEventDispatched(struct EventGroup *pEventGroup) {
    NV_ASSERT_FAILED_PRECOMP("EventGroup was disabled!");
}
#else // __nvoc_event_group_h_disabled
#define eventgroupEventDispatched(pEventGroup) eventgroupEventDispatched_IMPL(pEventGroup)
#endif // __nvoc_event_group_h_disabled

NvBool eventgroupIsRunnable_IMPL(struct EventGroup *pEventGroup);
#ifdef __nvoc_event_group_h_disabled
static inline NvBool eventgroupIsRunnable(struct EventGroup *pEventGroup) {
    NV_ASSERT_FAILED_PRECOMP("EventGroup was disabled!");
    return NV_FALSE;
}
#else // __nvoc_event_group_h_disabled
#define eventgroupIsRunnable(pEventGroup) eventgroupIsRunnable_IMPL(pEventGroup)
#endif // __nvoc_event_group_h_disabled

void eventgroupAddEvent_IMPL(struct EventGroup *pEventGroup, struct Event *pEvent);
#ifdef __nvoc_event_group_h_disabled
static inline void eventgroupAddEvent(struct EventGroup *pEventGroup, struct Event *pEvent) {
    NV_ASSERT_FAILED_PRECOMP("EventGroup was disabled!");
}
#else // __nvoc_event_group_h_disabled
#define eventgroupAddEvent(pEventGroup, pEvent) eventgroupAddEvent_IMPL(pEventGroup, pEvent)
#endif // __nvoc_event_group_h_disabled

void eventgroupSetTraceId_IMPL(struct EventGroup *pEventGroup, NvU64 traceId);
#ifdef __nvoc_event_group_h_disabled
static inline void eventgroupSetTraceId(struct EventGroup *pEventGroup, NvU64 traceId) {
    NV_ASSERT_FAILED_PRECOMP("EventGroup was disabled!");
}
#else // __nvoc_event_group_h_disabled
#define eventgroupSetTraceId(pEventGroup, traceId) eventgroupSetTraceId_IMPL(pEventGroup, traceId)
#endif // __nvoc_event_group_h_disabled

NvU64 eventgroupGetTraceId_IMPL(struct EventGroup *pEventGroup);
#ifdef __nvoc_event_group_h_disabled
static inline NvU64 eventgroupGetTraceId(struct EventGroup *pEventGroup) {
    NV_ASSERT_FAILED_PRECOMP("EventGroup was disabled!");
    return 0;
}
#else // __nvoc_event_group_h_disabled
#define eventgroupGetTraceId(pEventGroup) eventgroupGetTraceId_IMPL(pEventGroup)
#endif // __nvoc_event_group_h_disabled

void eventgroupSetAttributes_IMPL(struct EventGroup *pEventGroup, NvU8 groupAttributes);
#ifdef __nvoc_event_group_h_disabled
static inline void eventgroupSetAttributes(struct EventGroup *pEventGroup, NvU8 groupAttributes) {
    NV_ASSERT_FAILED_PRECOMP("EventGroup was disabled!");
}
#else // __nvoc_event_group_h_disabled
#define eventgroupSetAttributes(pEventGroup, groupAttributes) eventgroupSetAttributes_IMPL(pEventGroup, groupAttributes)
#endif // __nvoc_event_group_h_disabled

NvU8 eventgroupGetAttributes_IMPL(struct EventGroup *pEventGroup);
#ifdef __nvoc_event_group_h_disabled
static inline NvU8 eventgroupGetAttributes(struct EventGroup *pEventGroup) {
    NV_ASSERT_FAILED_PRECOMP("EventGroup was disabled!");
    return 0;
}
#else // __nvoc_event_group_h_disabled
#define eventgroupGetAttributes(pEventGroup) eventgroupGetAttributes_IMPL(pEventGroup)
#endif // __nvoc_event_group_h_disabled


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


/*!
 * @brief Initialize the group's 64-bit trace ID with the supplied originator prefix.
 *
 * If the group already has a trace ID, returns the existing value. Otherwise, allocates
 * the generic monotonic counter portion and stores `(traceIdPrefix << 56) | counter`.
 * Use @ref eventgroupSetTraceId when adopting a specific trace ID.
 *
 * @param pEventGroup   The event group
 * @param traceIdPrefix Non-zero upper 8-bit originator prefix
 *
 * @return Current or newly assigned trace ID; 0 if no trace ID could be assigned
 */
NvU64 eventgroupInitTraceId(struct EventGroup *pEventGroup, NvU8 traceIdPrefix);

#endif


#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_EVENT_GROUP_NVOC_H_
