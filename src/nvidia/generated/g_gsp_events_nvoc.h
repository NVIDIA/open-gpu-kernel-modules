
#ifndef _G_GSP_EVENTS_NVOC_H_
#define _G_GSP_EVENTS_NVOC_H_

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

#include "g_gsp_events_nvoc.h"

#ifndef _GSP_EVENTS_H_
#define _GSP_EVENTS_H_

#include "events/gpu/gpu_events.h"
#include "events/gpu/gsp/gsp_event_ctx_defs.h"
#include "events/gpu/gsp/gsp_event_defs.h"

typedef struct GspRpcTimeoutCtx
{
    EventContextHeader header;
    GSP_RPC_TIMEOUT_DATA data;
} GspRpcTimeoutCtx;


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_GSP_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__GspRpcTimeout;
struct NVOC_METADATA__GpuTimeout;


struct GspRpcTimeout {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__GspRpcTimeout *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuTimeout __nvoc_base_GpuTimeout;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct Event *__nvoc_pbase_Event;    // event super^4
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^3
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super^2
    struct GpuTimeout *__nvoc_pbase_GpuTimeout;    // gputimeout super
    struct GspRpcTimeout *__nvoc_pbase_GspRpcTimeout;    // gsprpcto

    // Data members
    GspRpcTimeoutCtx context;
    GpuXidEventCtx xid119;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__GspRpcTimeout {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuTimeout metadata__GpuTimeout;
};

#ifndef __nvoc_class_id_GspRpcTimeout
#define __nvoc_class_id_GspRpcTimeout 0x3ef5d1u
typedef struct GspRpcTimeout GspRpcTimeout;
#endif /* __nvoc_class_id_GspRpcTimeout */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GspRpcTimeout;

#define __staticCast_GspRpcTimeout(pThis) \
    ((pThis)->__nvoc_pbase_GspRpcTimeout)

#ifdef __nvoc_gsp_events_h_disabled
#define __dynamicCast_GspRpcTimeout(pThis) ((GspRpcTimeout*) NULL)
#else //__nvoc_gsp_events_h_disabled
#define __dynamicCast_GspRpcTimeout(pThis) \
    ((GspRpcTimeout*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(GspRpcTimeout)))
#endif //__nvoc_gsp_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_GspRpcTimeout(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_GspRpcTimeout(GspRpcTimeout**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU32 severity, NvU64 timeoutNs, NvU64 elapsedNs, NvU32 gpuInstance, NvU32 expectedFunc, const char *pRpcName, NvU32 expectedSequence, NvU64 historyData0, NvU64 historyData1, NvU64 waitedSeconds);
#define __objCreate_GspRpcTimeout(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, severity, timeoutNs, elapsedNs, gpuInstance, expectedFunc, pRpcName, expectedSequence, historyData0, historyData1, waitedSeconds) \
    __nvoc_objCreate_GspRpcTimeout((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, severity, timeoutNs, elapsedNs, gpuInstance, expectedFunc, pRpcName, expectedSequence, historyData0, historyData1, waitedSeconds)


// Wrapper macros for implementation functions
NV_STATUS gsprpctoConstruct_IMPL(struct GspRpcTimeout *pEvent, struct EventBus *pEventBus, NvU32 severity, NvU64 timeoutNs, NvU64 elapsedNs, NvU32 gpuInstance, NvU32 expectedFunc, const char *pRpcName, NvU32 expectedSequence, NvU64 historyData0, NvU64 historyData1, NvU64 waitedSeconds);
#define __nvoc_gsprpctoConstruct(pEvent, pEventBus, severity, timeoutNs, elapsedNs, gpuInstance, expectedFunc, pRpcName, expectedSequence, historyData0, historyData1, waitedSeconds) gsprpctoConstruct_IMPL(pEvent, pEventBus, severity, timeoutNs, elapsedNs, gpuInstance, expectedFunc, pRpcName, expectedSequence, historyData0, historyData1, waitedSeconds)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


typedef struct GspHeartbeatTimeoutCtx
{
    EventContextHeader header;
    const char *pHeartbeatSource;
} GspHeartbeatTimeoutCtx;


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_GSP_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__GspHeartbeatTimeout;
struct NVOC_METADATA__GpuTimeout;


struct GspHeartbeatTimeout {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__GspHeartbeatTimeout *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuTimeout __nvoc_base_GpuTimeout;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct Event *__nvoc_pbase_Event;    // event super^4
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^3
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super^2
    struct GpuTimeout *__nvoc_pbase_GpuTimeout;    // gputimeout super
    struct GspHeartbeatTimeout *__nvoc_pbase_GspHeartbeatTimeout;    // gsphbto

    // Data members
    GspHeartbeatTimeoutCtx context;
    GpuXidEventCtx xid120;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__GspHeartbeatTimeout {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuTimeout metadata__GpuTimeout;
};

#ifndef __nvoc_class_id_GspHeartbeatTimeout
#define __nvoc_class_id_GspHeartbeatTimeout 0x447333u
typedef struct GspHeartbeatTimeout GspHeartbeatTimeout;
#endif /* __nvoc_class_id_GspHeartbeatTimeout */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GspHeartbeatTimeout;

#define __staticCast_GspHeartbeatTimeout(pThis) \
    ((pThis)->__nvoc_pbase_GspHeartbeatTimeout)

#ifdef __nvoc_gsp_events_h_disabled
#define __dynamicCast_GspHeartbeatTimeout(pThis) ((GspHeartbeatTimeout*) NULL)
#else //__nvoc_gsp_events_h_disabled
#define __dynamicCast_GspHeartbeatTimeout(pThis) \
    ((GspHeartbeatTimeout*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(GspHeartbeatTimeout)))
#endif //__nvoc_gsp_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_GspHeartbeatTimeout(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_GspHeartbeatTimeout(GspHeartbeatTimeout**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU32 eventCode, const char *pHeartbeatSource, NvU32 severity, NvU64 timeoutNs, NvU64 elapsedNs);
#define __objCreate_GspHeartbeatTimeout(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, eventCode, pHeartbeatSource, severity, timeoutNs, elapsedNs) \
    __nvoc_objCreate_GspHeartbeatTimeout((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, eventCode, pHeartbeatSource, severity, timeoutNs, elapsedNs)


// Wrapper macros for implementation functions
NV_STATUS gsphbtoConstruct_IMPL(struct GspHeartbeatTimeout *pEvent, struct EventBus *pEventBus, NvU32 eventCode, const char *pHeartbeatSource, NvU32 severity, NvU64 timeoutNs, NvU64 elapsedNs);
#define __nvoc_gsphbtoConstruct(pEvent, pEventBus, eventCode, pHeartbeatSource, severity, timeoutNs, elapsedNs) gsphbtoConstruct_IMPL(pEvent, pEventBus, eventCode, pHeartbeatSource, severity, timeoutNs, elapsedNs)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


typedef struct GspFirmwareFaultCtx
{
    EventContextHeader header;
    // No data, used to emit Xid message
} GspFirmwareFaultCtx;


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_GSP_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__GspFirmwareFault;
struct NVOC_METADATA__GpuFirmwareFault;


struct GspFirmwareFault {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__GspFirmwareFault *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuFirmwareFault __nvoc_base_GpuFirmwareFault;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct Event *__nvoc_pbase_Event;    // event super^4
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^3
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super^2
    struct GpuFirmwareFault *__nvoc_pbase_GpuFirmwareFault;    // gpufwfault super
    struct GspFirmwareFault *__nvoc_pbase_GspFirmwareFault;    // gspfwfault

    // Data members
    GspFirmwareFaultCtx context;
    GpuXidEventCtx xid120;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__GspFirmwareFault {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuFirmwareFault metadata__GpuFirmwareFault;
};

#ifndef __nvoc_class_id_GspFirmwareFault
#define __nvoc_class_id_GspFirmwareFault 0x5e0cdau
typedef struct GspFirmwareFault GspFirmwareFault;
#endif /* __nvoc_class_id_GspFirmwareFault */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GspFirmwareFault;

#define __staticCast_GspFirmwareFault(pThis) \
    ((pThis)->__nvoc_pbase_GspFirmwareFault)

#ifdef __nvoc_gsp_events_h_disabled
#define __dynamicCast_GspFirmwareFault(pThis) ((GspFirmwareFault*) NULL)
#else //__nvoc_gsp_events_h_disabled
#define __dynamicCast_GspFirmwareFault(pThis) \
    ((GspFirmwareFault*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(GspFirmwareFault)))
#endif //__nvoc_gsp_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_GspFirmwareFault(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_GspFirmwareFault(GspFirmwareFault**, Dynamic*, NvU32, struct EventBus *pEventBus);
#define __objCreate_GspFirmwareFault(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus) \
    __nvoc_objCreate_GspFirmwareFault((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus)


// Wrapper macros for implementation functions
NV_STATUS gspfwfaultConstruct_IMPL(struct GspFirmwareFault *pEvent, struct EventBus *pEventBus);
#define __nvoc_gspfwfaultConstruct(pEvent, pEventBus) gspfwfaultConstruct_IMPL(pEvent, pEventBus)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


typedef struct GspPoisonCtx
{
    EventContextHeader header;
} GspPoisonCtx;


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_GSP_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__GspPoison;
struct NVOC_METADATA__GpuMemoryIntegrityError;


struct GspPoison {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__GspPoison *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuMemoryIntegrityError __nvoc_base_GpuMemoryIntegrityError;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct Event *__nvoc_pbase_Event;    // event super^4
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^3
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super^2
    struct GpuMemoryIntegrityError *__nvoc_pbase_GpuMemoryIntegrityError;    // gpumemintegrityerr super
    struct GspPoison *__nvoc_pbase_GspPoison;    // gsppoison

    // Data members
    GspPoisonCtx context;
    GpuXidEventCtx xid140;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__GspPoison {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuMemoryIntegrityError metadata__GpuMemoryIntegrityError;
};

#ifndef __nvoc_class_id_GspPoison
#define __nvoc_class_id_GspPoison 0xa610b0u
typedef struct GspPoison GspPoison;
#endif /* __nvoc_class_id_GspPoison */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GspPoison;

#define __staticCast_GspPoison(pThis) \
    ((pThis)->__nvoc_pbase_GspPoison)

#ifdef __nvoc_gsp_events_h_disabled
#define __dynamicCast_GspPoison(pThis) ((GspPoison*) NULL)
#else //__nvoc_gsp_events_h_disabled
#define __dynamicCast_GspPoison(pThis) \
    ((GspPoison*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(GspPoison)))
#endif //__nvoc_gsp_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_GspPoison(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_GspPoison(GspPoison**, Dynamic*, NvU32, struct EventBus *pEventBus);
#define __objCreate_GspPoison(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus) \
    __nvoc_objCreate_GspPoison((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus)


// Wrapper macros for implementation functions
NV_STATUS gsppoisonConstruct_IMPL(struct GspPoison *pEvent, struct EventBus *pEventBus);
#define __nvoc_gsppoisonConstruct(pEvent, pEventBus) gsppoisonConstruct_IMPL(pEvent, pEventBus)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


#endif // _GSP_EVENTS_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_GSP_EVENTS_NVOC_H_
