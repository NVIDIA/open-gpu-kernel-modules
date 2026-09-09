
#ifndef _G_EVENT_NVOC_H_
#define _G_EVENT_NVOC_H_

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

#include "g_event_nvoc.h"

#ifndef _NVOC_EVENT_H_
#define _NVOC_EVENT_H_

#include "nvtypes.h"
#include "nvstatus.h"
#include "nvport/nvport.h"
#include "containers/list.h"
#include "nvoc/object.h"

struct EventBus;

#ifndef __nvoc_class_id_EventBus
#define __nvoc_class_id_EventBus 0x2e5918u
typedef struct EventBus EventBus;
#endif /* __nvoc_class_id_EventBus */


struct EventGroup;

#ifndef __nvoc_class_id_EventGroup
#define __nvoc_class_id_EventGroup 0x5e0b7cu
typedef struct EventGroup EventGroup;
#endif /* __nvoc_class_id_EventGroup */



//
// Event is the root of a parallel NVOC type hierarchy, distinct from Object
// (see the NVOC Event Support SADD). NVOC's frontend currently rejects
// `[[nvoc::event_visibility]]` classes that extend Object, so Event must
// stand alone here.
//
// Until NVOC emits its own `__nvoc_eventCreate_*` family for events, the
// runtime piggybacks on `objCreate` to instantiate event objects.
//

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_EVENT_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__Event;
struct NVOC_METADATA__Object;


struct Event {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__Event *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct Object __nvoc_base_Object;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super
    struct Event *__nvoc_pbase_Event;    // event

    // Data members
    struct EventBus *pEventBus;
    struct EventGroup *pEventGroup;
    struct ListNode eventListNode;
    NvBool PRIVATE_FIELD(bPaused);
    NvBool PRIVATE_FIELD(bDispatched);
};


struct Event_PRIVATE {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__Event *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct Object __nvoc_base_Object;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super
    struct Event *__nvoc_pbase_Event;    // event

    // Data members
    struct EventBus *pEventBus;
    struct EventGroup *pEventGroup;
    struct ListNode eventListNode;
    NvBool bPaused;
    NvBool bDispatched;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__Event {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__Object metadata__Object;
};

#ifndef __nvoc_class_id_Event
#define __nvoc_class_id_Event 0xa4ecfcu
typedef struct Event Event;
#endif /* __nvoc_class_id_Event */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;

#define __staticCast_Event(pThis) \
    ((pThis)->__nvoc_pbase_Event)

#ifdef __nvoc_event_h_disabled
#define __dynamicCast_Event(pThis) ((Event*) NULL)
#else //__nvoc_event_h_disabled
#define __dynamicCast_Event(pThis) \
    ((Event*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(Event)))
#endif //__nvoc_event_h_disabled

NV_STATUS __nvoc_objCreateDynamic_Event(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_Event(Event**, Dynamic*, NvU32, struct EventBus *pEventBus);
#define __objCreate_Event(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus) \
    __nvoc_objCreate_Event((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus)


// Wrapper macros for implementation functions
NV_STATUS eventConstruct_IMPL(struct Event *pEvent, struct EventBus *pEventBus);
#define __nvoc_eventConstruct(pEvent, pEventBus) eventConstruct_IMPL(pEvent, pEventBus)

void eventPause_IMPL(struct Event *pEvent);
#ifdef __nvoc_event_h_disabled
static inline void eventPause(struct Event *pEvent) {
    NV_ASSERT_FAILED_PRECOMP("Event was disabled!");
}
#else // __nvoc_event_h_disabled
#define eventPause(pEvent) eventPause_IMPL(pEvent)
#endif // __nvoc_event_h_disabled

void eventResume_IMPL(struct Event *pEvent);
#ifdef __nvoc_event_h_disabled
static inline void eventResume(struct Event *pEvent) {
    NV_ASSERT_FAILED_PRECOMP("Event was disabled!");
}
#else // __nvoc_event_h_disabled
#define eventResume(pEvent) eventResume_IMPL(pEvent)
#endif // __nvoc_event_h_disabled

NvBool eventIsPaused_IMPL(struct Event *pEvent);
#ifdef __nvoc_event_h_disabled
static inline NvBool eventIsPaused(struct Event *pEvent) {
    NV_ASSERT_FAILED_PRECOMP("Event was disabled!");
    return NV_FALSE;
}
#else // __nvoc_event_h_disabled
#define eventIsPaused(pEvent) eventIsPaused_IMPL(pEvent)
#endif // __nvoc_event_h_disabled


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
static inline NvBool eventIsDispatched(struct Event *pEvent){
    struct Event_PRIVATE *pEvent_PRIVATE = (struct Event_PRIVATE *)pEvent;
    return pEvent_PRIVATE->bDispatched;
}

static inline void eventMarkDispatched(struct Event *pEvent){
    struct Event_PRIVATE *pEvent_PRIVATE = (struct Event_PRIVATE *)pEvent;
    pEvent_PRIVATE->bDispatched = NV_TRUE;
}

#undef PRIVATE_FIELD


#define eventBus(pEvent) staticCast(pEvent, Event)->pEventBus
#define eventSource(pEvent) staticCast(pEvent, Object)->pParent

//
// eventEmit*: Create and publish an event without requiring the caller to retain
// the event pointer. This is analogous to objCreate(), but also enqueues the
// created event on the nearest event bus.
//
// _EVENT_PP_CALL is a local twin of NVOC_PP_CALL/NV_EXPAND, used to forward to
// generated NVOC helpers. objCreate() itself uses NVOC_PP_CALL/NV_EXPAND, so
// reusing those names here would trip the C preprocessor's "blue paint" rule
// (C11 §6.10.3.4): nested replacements may not re-expand the macro currently
// being replaced, so the inner NVOC_PP_CALL/NV_EXPAND tokens would be emitted
// verbatim. The local twin keeps a distinct identity for the outer hop so the
// inner expansions remain free.
//
// Routing the objCreate() call through this trampoline also gives MSVC's
// traditional preprocessor a fresh function-call re-scan at the eventEmit ->
// objCreate hop, mirroring the trick that NVOC_PP_CALL plays at the
// objCreate -> __objCreate_<NAME> hop. Without it, an empty variadic list at
// the eventEmit level leaves a stray empty argument that propagates through
// objCreate and overflows the fixed arity of the generated __objCreate_<NAME>
// macro, producing MSVC C4002.
//
#define _EVENT_PP_EXPAND(...) __VA_ARGS__
#define _EVENT_PP_CALL(func, ...) _EVENT_PP_EXPAND(func _EVENT_PP_EXPAND() (__VA_ARGS__))

//
// Common implementation behind eventEmit / eventEmitAssociated. Creates the
// new event via objCreate and forwards to either eventbusEventPublish (when
// pExistingEvent is NULL: new single-event group) or
// eventbusEventPublishAssociated (when non-NULL: add to the existing event's
// group). Both publish functions return NV_STATUS and validate their own
// inputs (NULL bus, mismatched bus on associated path, etc.); on any
// non-NV_OK return the macro destroys pEvent, so the macros stay free of
// validation logic.
//
#define _eventEmitImpl(eventType, pExistingEvent, pSource, flags, ...)         \
    do {                                                                       \
        eventType *pEvent;                                                     \
        NV_STATUS _eventEmitStatus;                                            \
        EventBus *pBus = objEventBus(pSource);                                 \
        NV_ASSERT_OK_OR_ELSE(_eventEmitStatus,                                 \
            _EVENT_PP_CALL(objCreate, &pEvent, pSource, eventType,             \
                           pBus, ##__VA_ARGS__),                               \
            break);                                                            \
        _eventEmitStatus = ((pExistingEvent) == NULL)                          \
            ? _EVENT_PP_CALL(eventbusEventPublish,                             \
                             pBus, staticCast(pEvent, Event))                  \
            : _EVENT_PP_CALL(eventbusEventPublishAssociated,                   \
                             pBus, (pExistingEvent),                           \
                             staticCast(pEvent, Event));                       \
        if (_eventEmitStatus != NV_OK)                                         \
            objDelete(pEvent);                                                 \
    } while (0)

#define eventEmitWithFlags(eventType, pSource, flags, ...) \
    _eventEmitImpl(eventType, NULL, pSource, flags, ##__VA_ARGS__)

#define eventEmit(eventType, pSource, ...) \
    eventEmitWithFlags(eventType, pSource, NVOC_OBJ_CREATE_FLAGS_NONE, ##__VA_ARGS__)

#define eventEmitAssociatedWithFlags(eventType, pExistingEvent, pSource, flags, ...) \
    _eventEmitImpl(eventType, pExistingEvent, pSource, flags, ##__VA_ARGS__)

#define eventEmitAssociated(eventType, pExistingEvent, pSource, ...)       \
    eventEmitAssociatedWithFlags(eventType, pExistingEvent, pSource,       \
                                 NVOC_OBJ_CREATE_FLAGS_NONE,               \
                                 ##__VA_ARGS__)

#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_EVENT_NVOC_H_
