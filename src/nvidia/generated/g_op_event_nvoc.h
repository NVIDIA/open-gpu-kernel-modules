
#ifndef _G_OP_EVENT_NVOC_H_
#define _G_OP_EVENT_NVOC_H_

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
#pragma once

#if defined(SRT_BUILD)
#define RMCFG_FEATURE_x 1
#else
#include "rmconfig.h"
#endif

#include "g_op_event_nvoc.h"

#ifndef _OP_EVENT_H_
#define _OP_EVENT_H_

#include "nvtypes.h"
#include "nvstatus.h"
#include "class/cl90d0.h"
#include "cper/gpu_cper.h"
#include "nvoc/event.h"
#include "events/event_defs.h"

typedef struct EventContextHeader EventContextHeader;

struct OBJGPU;

#ifndef __nvoc_class_id_OBJGPU
#define __nvoc_class_id_OBJGPU 0x7ef3cbu
typedef struct OBJGPU OBJGPU;
#endif /* __nvoc_class_id_OBJGPU */



typedef struct
{
    /* -------------------------------------- Serializers -------------------------------------- */
    NV_STATUS (*serializeToEventBufferOpEventCtx)(EventContextHeader *pContext,
                                                  NV_OPERATIONAL_EVENT_CONTEXT *pOutput,
                                                  NvU32 outputSize, NvU32 *pBytesWritten);

    NV_STATUS (*serializeToCperEventCtx)(EventContextHeader *pContext,
                                         NV_CPER_NV_EVENT_SECTION_STATE *pState);

    /* --------------------------------------- Emitters ---------------------------------------- */
    void (*emitToOsLogXid)(EventContextHeader *pContext, struct OBJGPU *pGpu);

    NV_STATUS (*emitToOsLogCper)(EventContextHeader *pContext,
                                 PORT_DEVICE *pDevice, PORT_LOG_LEVEL level, NvU32 seq,
                                 const char *pLogPrefix);
} EventContextOutputAdapters;

struct EventContextHeader
{
    EventContextHeader *pNext;
    const EventContextOutputAdapters *pOutputAdapters;

    // Size of retained context data following this header; serialized payloads may differ.
    NvU32 dataSize;

    // When set, context storage is heap-allocated separately from its OperationalEvent.
    NvBool bOwnedByEvent;
};


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_OP_EVENT_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__OperationalEvent;
struct NVOC_METADATA__Event;


struct OperationalEvent {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__OperationalEvent *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct Event __nvoc_base_Event;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct Event *__nvoc_pbase_Event;    // event super
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt

    // Data members
    NvU16 category;
    NvU16 eventCode;
    char moduleSignature[16];
    NvU64 instanceId;
    NvU64 timestamp;
    OPERATIONAL_EVENT_SEVERITY severity;
    OPERATIONAL_EVENT_ATTRIBUTES attributes;
    OPERATIONAL_EVENT_LOG_LEVEL logLevel;
    struct EventContextHeader *pContextList;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__OperationalEvent {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__Event metadata__Event;
};

#ifndef __nvoc_class_id_OperationalEvent
#define __nvoc_class_id_OperationalEvent 0x8eae09u
typedef struct OperationalEvent OperationalEvent;
#endif /* __nvoc_class_id_OperationalEvent */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OperationalEvent;

#define __staticCast_OperationalEvent(pThis) \
    ((pThis)->__nvoc_pbase_OperationalEvent)

#ifdef __nvoc_op_event_h_disabled
#define __dynamicCast_OperationalEvent(pThis) ((OperationalEvent*) NULL)
#else //__nvoc_op_event_h_disabled
#define __dynamicCast_OperationalEvent(pThis) \
    ((OperationalEvent*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(OperationalEvent)))
#endif //__nvoc_op_event_h_disabled

NV_STATUS __nvoc_objCreateDynamic_OperationalEvent(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_OperationalEvent(OperationalEvent**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU32 category, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 logLevel);
#define __objCreate_OperationalEvent(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, category, pModuleSignature, eventCode, severity, attributes, logLevel) \
    __nvoc_objCreate_OperationalEvent((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, category, pModuleSignature, eventCode, severity, attributes, logLevel)


// Wrapper macros for implementation functions
NV_STATUS opevtConstruct_IMPL(struct OperationalEvent *pEvent, struct EventBus *pEventBus, NvU32 category, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 logLevel);
#define __nvoc_opevtConstruct(pEvent, pEventBus, category, pModuleSignature, eventCode, severity, attributes, logLevel) opevtConstruct_IMPL(pEvent, pEventBus, category, pModuleSignature, eventCode, severity, attributes, logLevel)

void opevtDestruct_IMPL(struct OperationalEvent *pEvent);
#define __nvoc_opevtDestruct(pEvent) opevtDestruct_IMPL(pEvent)

void opevtAddContext_IMPL(struct OperationalEvent *pEvent, struct EventContextHeader *pContext);
#ifdef __nvoc_op_event_h_disabled
static inline void opevtAddContext(struct OperationalEvent *pEvent, struct EventContextHeader *pContext) {
    NV_ASSERT_FAILED_PRECOMP("OperationalEvent was disabled!");
}
#else // __nvoc_op_event_h_disabled
#define opevtAddContext(pEvent, pContext) opevtAddContext_IMPL(pEvent, pContext)
#endif // __nvoc_op_event_h_disabled


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


#endif // _OP_EVENT_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_OP_EVENT_NVOC_H_
