
#ifndef _G_GPU_EVENTS_NVOC_H_
#define _G_GPU_EVENTS_NVOC_H_

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

#include "g_gpu_events_nvoc.h"

#ifndef _GPU_EVENTS_H_
#define _GPU_EVENTS_H_

#include "gpu/gpu_op_event.h"

/* ----------------------------- GPU Operational Event Categories ----------------------------- */

// GPU_OPERATIONAL_EVENT_CATEGORY_TIMEOUT
typedef struct GpuTimeoutEventCtx
{
    EventContextHeader header;
    GPU_OPERATIONAL_EVENT_CTX_GPU_TIMEOUT_DATA data;
} GpuTimeoutEventCtx;


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_GPU_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__GpuTimeout;
struct NVOC_METADATA__GpuOperationalEvent;


struct GpuTimeout {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__GpuTimeout *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuOperationalEvent __nvoc_base_GpuOperationalEvent;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^4
    struct Event *__nvoc_pbase_Event;    // event super^3
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^2
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super
    struct GpuTimeout *__nvoc_pbase_GpuTimeout;    // gputimeout

    // Data members
    GpuTimeoutEventCtx context;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__GpuTimeout {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuOperationalEvent metadata__GpuOperationalEvent;
};

#ifndef __nvoc_class_id_GpuTimeout
#define __nvoc_class_id_GpuTimeout 0x6b9776u
typedef struct GpuTimeout GpuTimeout;
#endif /* __nvoc_class_id_GpuTimeout */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuTimeout;

#define __staticCast_GpuTimeout(pThis) \
    ((pThis)->__nvoc_pbase_GpuTimeout)

#ifdef __nvoc_gpu_events_h_disabled
#define __dynamicCast_GpuTimeout(pThis) ((GpuTimeout*) NULL)
#else //__nvoc_gpu_events_h_disabled
#define __dynamicCast_GpuTimeout(pThis) \
    ((GpuTimeout*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(GpuTimeout)))
#endif //__nvoc_gpu_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_GpuTimeout(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_GpuTimeout(GpuTimeout**, Dynamic*, NvU32, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU64 timeoutNs, NvU64 elapsedNs, const char *pWaitTarget, NvU32 logLevel);
#define __objCreate_GpuTimeout(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, pModuleSignature, eventCode, severity, attributes, scope, timeoutNs, elapsedNs, pWaitTarget, logLevel) \
    __nvoc_objCreate_GpuTimeout((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, pModuleSignature, eventCode, severity, attributes, scope, timeoutNs, elapsedNs, pWaitTarget, logLevel)


// Wrapper macros for implementation functions
NV_STATUS gputimeoutConstruct_IMPL(struct GpuTimeout *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU64 timeoutNs, NvU64 elapsedNs, const char *pWaitTarget, NvU32 logLevel);
#define __nvoc_gputimeoutConstruct(pEvent, pEventBus, pModuleSignature, eventCode, severity, attributes, scope, timeoutNs, elapsedNs, pWaitTarget, logLevel) gputimeoutConstruct_IMPL(pEvent, pEventBus, pModuleSignature, eventCode, severity, attributes, scope, timeoutNs, elapsedNs, pWaitTarget, logLevel)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


// GPU_OPERATIONAL_EVENT_CATEGORY_MEMORY_INTEGRITY_ERROR

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_GPU_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__GpuMemoryIntegrityError;
struct NVOC_METADATA__GpuOperationalEvent;


struct GpuMemoryIntegrityError {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__GpuMemoryIntegrityError *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuOperationalEvent __nvoc_base_GpuOperationalEvent;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^4
    struct Event *__nvoc_pbase_Event;    // event super^3
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^2
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super
    struct GpuMemoryIntegrityError *__nvoc_pbase_GpuMemoryIntegrityError;    // gpumemintegrityerr
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__GpuMemoryIntegrityError {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuOperationalEvent metadata__GpuOperationalEvent;
};

#ifndef __nvoc_class_id_GpuMemoryIntegrityError
#define __nvoc_class_id_GpuMemoryIntegrityError 0xeb39adu
typedef struct GpuMemoryIntegrityError GpuMemoryIntegrityError;
#endif /* __nvoc_class_id_GpuMemoryIntegrityError */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuMemoryIntegrityError;

#define __staticCast_GpuMemoryIntegrityError(pThis) \
    ((pThis)->__nvoc_pbase_GpuMemoryIntegrityError)

#ifdef __nvoc_gpu_events_h_disabled
#define __dynamicCast_GpuMemoryIntegrityError(pThis) ((GpuMemoryIntegrityError*) NULL)
#else //__nvoc_gpu_events_h_disabled
#define __dynamicCast_GpuMemoryIntegrityError(pThis) \
    ((GpuMemoryIntegrityError*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(GpuMemoryIntegrityError)))
#endif //__nvoc_gpu_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_GpuMemoryIntegrityError(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_GpuMemoryIntegrityError(GpuMemoryIntegrityError**, Dynamic*, NvU32, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);
#define __objCreate_GpuMemoryIntegrityError(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, pModuleSignature, eventCode, severity, attributes, scope, reportingSource, logLevel) \
    __nvoc_objCreate_GpuMemoryIntegrityError((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, pModuleSignature, eventCode, severity, attributes, scope, reportingSource, logLevel)


// Wrapper macros for implementation functions
NV_STATUS gpumemintegrityerrConstruct_IMPL(struct GpuMemoryIntegrityError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);
#define __nvoc_gpumemintegrityerrConstruct(pEvent, pEventBus, pModuleSignature, eventCode, severity, attributes, scope, reportingSource, logLevel) gpumemintegrityerrConstruct_IMPL(pEvent, pEventBus, pModuleSignature, eventCode, severity, attributes, scope, reportingSource, logLevel)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


// GPU_OPERATIONAL_EVENT_CATEGORY_INTERCONNECT_ERROR

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_GPU_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__GpuInterconnectError;
struct NVOC_METADATA__GpuOperationalEvent;


struct GpuInterconnectError {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__GpuInterconnectError *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuOperationalEvent __nvoc_base_GpuOperationalEvent;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^4
    struct Event *__nvoc_pbase_Event;    // event super^3
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^2
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super
    struct GpuInterconnectError *__nvoc_pbase_GpuInterconnectError;    // gpuinterconnecterr
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__GpuInterconnectError {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuOperationalEvent metadata__GpuOperationalEvent;
};

#ifndef __nvoc_class_id_GpuInterconnectError
#define __nvoc_class_id_GpuInterconnectError 0x9050aau
typedef struct GpuInterconnectError GpuInterconnectError;
#endif /* __nvoc_class_id_GpuInterconnectError */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuInterconnectError;

#define __staticCast_GpuInterconnectError(pThis) \
    ((pThis)->__nvoc_pbase_GpuInterconnectError)

#ifdef __nvoc_gpu_events_h_disabled
#define __dynamicCast_GpuInterconnectError(pThis) ((GpuInterconnectError*) NULL)
#else //__nvoc_gpu_events_h_disabled
#define __dynamicCast_GpuInterconnectError(pThis) \
    ((GpuInterconnectError*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(GpuInterconnectError)))
#endif //__nvoc_gpu_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_GpuInterconnectError(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_GpuInterconnectError(GpuInterconnectError**, Dynamic*, NvU32, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);
#define __objCreate_GpuInterconnectError(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, pModuleSignature, eventCode, severity, attributes, scope, reportingSource, logLevel) \
    __nvoc_objCreate_GpuInterconnectError((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, pModuleSignature, eventCode, severity, attributes, scope, reportingSource, logLevel)


// Wrapper macros for implementation functions
NV_STATUS gpuinterconnecterrConstruct_IMPL(struct GpuInterconnectError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);
#define __nvoc_gpuinterconnecterrConstruct(pEvent, pEventBus, pModuleSignature, eventCode, severity, attributes, scope, reportingSource, logLevel) gpuinterconnecterrConstruct_IMPL(pEvent, pEventBus, pModuleSignature, eventCode, severity, attributes, scope, reportingSource, logLevel)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


// GPU_OPERATIONAL_EVENT_CATEGORY_FIRMWARE_FAULT

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_GPU_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__GpuFirmwareFault;
struct NVOC_METADATA__GpuOperationalEvent;


struct GpuFirmwareFault {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__GpuFirmwareFault *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuOperationalEvent __nvoc_base_GpuOperationalEvent;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^4
    struct Event *__nvoc_pbase_Event;    // event super^3
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^2
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super
    struct GpuFirmwareFault *__nvoc_pbase_GpuFirmwareFault;    // gpufwfault
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__GpuFirmwareFault {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuOperationalEvent metadata__GpuOperationalEvent;
};

#ifndef __nvoc_class_id_GpuFirmwareFault
#define __nvoc_class_id_GpuFirmwareFault 0x6f6239u
typedef struct GpuFirmwareFault GpuFirmwareFault;
#endif /* __nvoc_class_id_GpuFirmwareFault */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuFirmwareFault;

#define __staticCast_GpuFirmwareFault(pThis) \
    ((pThis)->__nvoc_pbase_GpuFirmwareFault)

#ifdef __nvoc_gpu_events_h_disabled
#define __dynamicCast_GpuFirmwareFault(pThis) ((GpuFirmwareFault*) NULL)
#else //__nvoc_gpu_events_h_disabled
#define __dynamicCast_GpuFirmwareFault(pThis) \
    ((GpuFirmwareFault*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(GpuFirmwareFault)))
#endif //__nvoc_gpu_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_GpuFirmwareFault(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_GpuFirmwareFault(GpuFirmwareFault**, Dynamic*, NvU32, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 logLevel);
#define __objCreate_GpuFirmwareFault(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, pModuleSignature, eventCode, severity, attributes, scope, logLevel) \
    __nvoc_objCreate_GpuFirmwareFault((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, pModuleSignature, eventCode, severity, attributes, scope, logLevel)


// Wrapper macros for implementation functions
NV_STATUS gpufwfaultConstruct_IMPL(struct GpuFirmwareFault *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 logLevel);
#define __nvoc_gpufwfaultConstruct(pEvent, pEventBus, pModuleSignature, eventCode, severity, attributes, scope, logLevel) gpufwfaultConstruct_IMPL(pEvent, pEventBus, pModuleSignature, eventCode, severity, attributes, scope, logLevel)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


// GPU_OPERATIONAL_EVENT_CATEGORY_RESOURCE_EXHAUSTED

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_GPU_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__GpuResourceExhausted;
struct NVOC_METADATA__GpuOperationalEvent;


struct GpuResourceExhausted {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__GpuResourceExhausted *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuOperationalEvent __nvoc_base_GpuOperationalEvent;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^4
    struct Event *__nvoc_pbase_Event;    // event super^3
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^2
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super
    struct GpuResourceExhausted *__nvoc_pbase_GpuResourceExhausted;    // gpuresexh
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__GpuResourceExhausted {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuOperationalEvent metadata__GpuOperationalEvent;
};

#ifndef __nvoc_class_id_GpuResourceExhausted
#define __nvoc_class_id_GpuResourceExhausted 0x07427au
typedef struct GpuResourceExhausted GpuResourceExhausted;
#endif /* __nvoc_class_id_GpuResourceExhausted */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResourceExhausted;

#define __staticCast_GpuResourceExhausted(pThis) \
    ((pThis)->__nvoc_pbase_GpuResourceExhausted)

#ifdef __nvoc_gpu_events_h_disabled
#define __dynamicCast_GpuResourceExhausted(pThis) ((GpuResourceExhausted*) NULL)
#else //__nvoc_gpu_events_h_disabled
#define __dynamicCast_GpuResourceExhausted(pThis) \
    ((GpuResourceExhausted*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(GpuResourceExhausted)))
#endif //__nvoc_gpu_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_GpuResourceExhausted(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_GpuResourceExhausted(GpuResourceExhausted**, Dynamic*, NvU32, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope);
#define __objCreate_GpuResourceExhausted(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, pModuleSignature, eventCode, severity, attributes, scope) \
    __nvoc_objCreate_GpuResourceExhausted((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, pModuleSignature, eventCode, severity, attributes, scope)


// Wrapper macros for implementation functions
NV_STATUS gpuresexhConstruct_IMPL(struct GpuResourceExhausted *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope);
#define __nvoc_gpuresexhConstruct(pEvent, pEventBus, pModuleSignature, eventCode, severity, attributes, scope) gpuresexhConstruct_IMPL(pEvent, pEventBus, pModuleSignature, eventCode, severity, attributes, scope)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


// GPU_OPERATIONAL_EVENT_CATEGORY_UNCLASSIFIED_ERROR

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_GPU_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__GpuUnclassifiedError;
struct NVOC_METADATA__GpuOperationalEvent;


struct GpuUnclassifiedError {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__GpuUnclassifiedError *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuOperationalEvent __nvoc_base_GpuOperationalEvent;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^4
    struct Event *__nvoc_pbase_Event;    // event super^3
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^2
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super
    struct GpuUnclassifiedError *__nvoc_pbase_GpuUnclassifiedError;    // gpuunclassifiederr
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__GpuUnclassifiedError {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuOperationalEvent metadata__GpuOperationalEvent;
};

#ifndef __nvoc_class_id_GpuUnclassifiedError
#define __nvoc_class_id_GpuUnclassifiedError 0x3555f8u
typedef struct GpuUnclassifiedError GpuUnclassifiedError;
#endif /* __nvoc_class_id_GpuUnclassifiedError */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuUnclassifiedError;

#define __staticCast_GpuUnclassifiedError(pThis) \
    ((pThis)->__nvoc_pbase_GpuUnclassifiedError)

#ifdef __nvoc_gpu_events_h_disabled
#define __dynamicCast_GpuUnclassifiedError(pThis) ((GpuUnclassifiedError*) NULL)
#else //__nvoc_gpu_events_h_disabled
#define __dynamicCast_GpuUnclassifiedError(pThis) \
    ((GpuUnclassifiedError*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(GpuUnclassifiedError)))
#endif //__nvoc_gpu_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_GpuUnclassifiedError(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_GpuUnclassifiedError(GpuUnclassifiedError**, Dynamic*, NvU32, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope);
#define __objCreate_GpuUnclassifiedError(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, pModuleSignature, eventCode, severity, attributes, scope) \
    __nvoc_objCreate_GpuUnclassifiedError((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, pModuleSignature, eventCode, severity, attributes, scope)


// Wrapper macros for implementation functions
NV_STATUS gpuunclassifiederrConstruct_IMPL(struct GpuUnclassifiedError *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope);
#define __nvoc_gpuunclassifiederrConstruct(pEvent, pEventBus, pModuleSignature, eventCode, severity, attributes, scope) gpuunclassifiederrConstruct_IMPL(pEvent, pEventBus, pModuleSignature, eventCode, severity, attributes, scope)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


// GPU_OPERATIONAL_EVENT_CATEGORY_INITIALIZATION

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_GPU_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__GpuInitializationEvent;
struct NVOC_METADATA__GpuOperationalEvent;


struct GpuInitializationEvent {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__GpuInitializationEvent *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuOperationalEvent __nvoc_base_GpuOperationalEvent;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^4
    struct Event *__nvoc_pbase_Event;    // event super^3
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^2
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super
    struct GpuInitializationEvent *__nvoc_pbase_GpuInitializationEvent;    // gpuinitevt
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__GpuInitializationEvent {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuOperationalEvent metadata__GpuOperationalEvent;
};

#ifndef __nvoc_class_id_GpuInitializationEvent
#define __nvoc_class_id_GpuInitializationEvent 0x1cdd99u
typedef struct GpuInitializationEvent GpuInitializationEvent;
#endif /* __nvoc_class_id_GpuInitializationEvent */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuInitializationEvent;

#define __staticCast_GpuInitializationEvent(pThis) \
    ((pThis)->__nvoc_pbase_GpuInitializationEvent)

#ifdef __nvoc_gpu_events_h_disabled
#define __dynamicCast_GpuInitializationEvent(pThis) ((GpuInitializationEvent*) NULL)
#else //__nvoc_gpu_events_h_disabled
#define __dynamicCast_GpuInitializationEvent(pThis) \
    ((GpuInitializationEvent*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(GpuInitializationEvent)))
#endif //__nvoc_gpu_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_GpuInitializationEvent(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_GpuInitializationEvent(GpuInitializationEvent**, Dynamic*, NvU32, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 logLevel);
#define __objCreate_GpuInitializationEvent(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, pModuleSignature, eventCode, severity, attributes, scope, logLevel) \
    __nvoc_objCreate_GpuInitializationEvent((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, pModuleSignature, eventCode, severity, attributes, scope, logLevel)


// Wrapper macros for implementation functions
NV_STATUS gpuinitevtConstruct_IMPL(struct GpuInitializationEvent *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 logLevel);
#define __nvoc_gpuinitevtConstruct(pEvent, pEventBus, pModuleSignature, eventCode, severity, attributes, scope, logLevel) gpuinitevtConstruct_IMPL(pEvent, pEventBus, pModuleSignature, eventCode, severity, attributes, scope, logLevel)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


// GPU_OPERATIONAL_EVENT_CATEGORY_RESOURCE_RETIREMENT

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_GPU_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__GpuResourceRetirement;
struct NVOC_METADATA__GpuOperationalEvent;


struct GpuResourceRetirement {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__GpuResourceRetirement *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuOperationalEvent __nvoc_base_GpuOperationalEvent;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^4
    struct Event *__nvoc_pbase_Event;    // event super^3
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^2
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super
    struct GpuResourceRetirement *__nvoc_pbase_GpuResourceRetirement;    // gpuresret
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__GpuResourceRetirement {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuOperationalEvent metadata__GpuOperationalEvent;
};

#ifndef __nvoc_class_id_GpuResourceRetirement
#define __nvoc_class_id_GpuResourceRetirement 0x8cf136u
typedef struct GpuResourceRetirement GpuResourceRetirement;
#endif /* __nvoc_class_id_GpuResourceRetirement */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResourceRetirement;

#define __staticCast_GpuResourceRetirement(pThis) \
    ((pThis)->__nvoc_pbase_GpuResourceRetirement)

#ifdef __nvoc_gpu_events_h_disabled
#define __dynamicCast_GpuResourceRetirement(pThis) ((GpuResourceRetirement*) NULL)
#else //__nvoc_gpu_events_h_disabled
#define __dynamicCast_GpuResourceRetirement(pThis) \
    ((GpuResourceRetirement*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(GpuResourceRetirement)))
#endif //__nvoc_gpu_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_GpuResourceRetirement(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_GpuResourceRetirement(GpuResourceRetirement**, Dynamic*, NvU32, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource);
#define __objCreate_GpuResourceRetirement(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, pModuleSignature, eventCode, severity, attributes, scope, reportingSource) \
    __nvoc_objCreate_GpuResourceRetirement((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, pModuleSignature, eventCode, severity, attributes, scope, reportingSource)


// Wrapper macros for implementation functions
NV_STATUS gpuresretConstruct_IMPL(struct GpuResourceRetirement *pEvent, struct EventBus *pEventBus, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource);
#define __nvoc_gpuresretConstruct(pEvent, pEventBus, pModuleSignature, eventCode, severity, attributes, scope, reportingSource) gpuresretConstruct_IMPL(pEvent, pEventBus, pModuleSignature, eventCode, severity, attributes, scope, reportingSource)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


/* ---------------------------------- GPU Operational Events ---------------------------------- */

typedef struct GpuDriverInitMetadataContext
{
    EventContextHeader header;
    GPU_OPERATIONAL_EVENT_CTX_GPU_INIT_METADATA data;
} GpuDriverInitMetadataContext;


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_GPU_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__GpuDriverInit;
struct NVOC_METADATA__GpuInitializationEvent;


struct GpuDriverInit {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__GpuDriverInit *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuInitializationEvent __nvoc_base_GpuInitializationEvent;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct Event *__nvoc_pbase_Event;    // event super^4
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^3
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super^2
    struct GpuInitializationEvent *__nvoc_pbase_GpuInitializationEvent;    // gpuinitevt super
    struct GpuDriverInit *__nvoc_pbase_GpuDriverInit;    // gpudrvinit

    // Data members
    GpuDriverInitMetadataContext context;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__GpuDriverInit {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuInitializationEvent metadata__GpuInitializationEvent;
};

#ifndef __nvoc_class_id_GpuDriverInit
#define __nvoc_class_id_GpuDriverInit 0x94f24eu
typedef struct GpuDriverInit GpuDriverInit;
#endif /* __nvoc_class_id_GpuDriverInit */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuDriverInit;

#define __staticCast_GpuDriverInit(pThis) \
    ((pThis)->__nvoc_pbase_GpuDriverInit)

#ifdef __nvoc_gpu_events_h_disabled
#define __dynamicCast_GpuDriverInit(pThis) ((GpuDriverInit*) NULL)
#else //__nvoc_gpu_events_h_disabled
#define __dynamicCast_GpuDriverInit(pThis) \
    ((GpuDriverInit*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(GpuDriverInit)))
#endif //__nvoc_gpu_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_GpuDriverInit(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_GpuDriverInit(GpuDriverInit**, Dynamic*, NvU32, struct EventBus *pEventBus);
#define __objCreate_GpuDriverInit(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus) \
    __nvoc_objCreate_GpuDriverInit((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus)


// Wrapper macros for implementation functions
NV_STATUS gpudrvinitConstruct_IMPL(struct GpuDriverInit *pEvent, struct EventBus *pEventBus);
#define __nvoc_gpudrvinitConstruct(pEvent, pEventBus) gpudrvinitConstruct_IMPL(pEvent, pEventBus)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


#endif // _GPU_EVENTS_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_GPU_EVENTS_NVOC_H_
