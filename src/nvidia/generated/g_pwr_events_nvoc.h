
#ifndef _G_PWR_EVENTS_NVOC_H_
#define _G_PWR_EVENTS_NVOC_H_

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

#include "g_pwr_events_nvoc.h"

#ifndef _PWR_EVENTS_H_
#define _PWR_EVENTS_H_

#include "events/gpu/gpu_events.h"
#include "events/gpu/pwr/pwr_event_defs.h"

//
// PSHC power-smoothing controller events, emitted from the PMU->RM RPC handler.
// Each event carries an opaque context for EventBuffer/CPER plus a GpuXidEventCtx
// so the corresponding legacy Xid still reaches the OS log.
//

/* ------------------------------- PSHC Reengaged (Xid 162) ------------------------------- */
typedef struct PshcReengagedCtx
{
    EventContextHeader header;
} PshcReengagedCtx;


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_PWR_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__PshcReengaged;
struct NVOC_METADATA__GpuStateChanged;


struct PshcReengaged {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__PshcReengaged *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuStateChanged __nvoc_base_GpuStateChanged;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct Event *__nvoc_pbase_Event;    // event super^4
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^3
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super^2
    struct GpuStateChanged *__nvoc_pbase_GpuStateChanged;    // gpustatechanged super
    struct PshcReengaged *__nvoc_pbase_PshcReengaged;    // pshcreengaged

    // Data members
    PshcReengagedCtx context;
    GpuXidEventCtx xid162;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__PshcReengaged {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuStateChanged metadata__GpuStateChanged;
};

#ifndef __nvoc_class_id_PshcReengaged
#define __nvoc_class_id_PshcReengaged 0xf59a1fu
typedef struct PshcReengaged PshcReengaged;
#endif /* __nvoc_class_id_PshcReengaged */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_PshcReengaged;

#define __staticCast_PshcReengaged(pThis) \
    ((pThis)->__nvoc_pbase_PshcReengaged)

#ifdef __nvoc_pwr_events_h_disabled
#define __dynamicCast_PshcReengaged(pThis) ((PshcReengaged*) NULL)
#else //__nvoc_pwr_events_h_disabled
#define __dynamicCast_PshcReengaged(pThis) \
    ((PshcReengaged*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(PshcReengaged)))
#endif //__nvoc_pwr_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_PshcReengaged(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_PshcReengaged(PshcReengaged**, Dynamic*, NvU32, struct EventBus *pEventBus);
#define __objCreate_PshcReengaged(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus) \
    __nvoc_objCreate_PshcReengaged((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus)


// Wrapper macros for implementation functions
NV_STATUS pshcreengagedConstruct_IMPL(struct PshcReengaged *pEvent, struct EventBus *pEventBus);
#define __nvoc_pshcreengagedConstruct(pEvent, pEventBus) pshcreengagedConstruct_IMPL(pEvent, pEventBus)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


/* ------------------------------- PSHC Disengaged (Xid 163) ------------------------------ */
typedef struct PshcDisengagedCtx
{
    EventContextHeader header;
    NvU32 reason;        // NV2080_CTRL_PMGR_ISINK_CONTROLLERS_XID_ISINK_DISENGAGE_REASON_*
    NvU32 slowdownMask;  // valid for the SLOWDOWN reason only
} PshcDisengagedCtx;


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_PWR_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__PshcDisengaged;
struct NVOC_METADATA__GpuStateChanged;


struct PshcDisengaged {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__PshcDisengaged *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuStateChanged __nvoc_base_GpuStateChanged;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct Event *__nvoc_pbase_Event;    // event super^4
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^3
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super^2
    struct GpuStateChanged *__nvoc_pbase_GpuStateChanged;    // gpustatechanged super
    struct PshcDisengaged *__nvoc_pbase_PshcDisengaged;    // pshcdisengaged

    // Data members
    PshcDisengagedCtx context;
    GpuXidEventCtx xid163;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__PshcDisengaged {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuStateChanged metadata__GpuStateChanged;
};

#ifndef __nvoc_class_id_PshcDisengaged
#define __nvoc_class_id_PshcDisengaged 0xfa9cd6u
typedef struct PshcDisengaged PshcDisengaged;
#endif /* __nvoc_class_id_PshcDisengaged */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_PshcDisengaged;

#define __staticCast_PshcDisengaged(pThis) \
    ((pThis)->__nvoc_pbase_PshcDisengaged)

#ifdef __nvoc_pwr_events_h_disabled
#define __dynamicCast_PshcDisengaged(pThis) ((PshcDisengaged*) NULL)
#else //__nvoc_pwr_events_h_disabled
#define __dynamicCast_PshcDisengaged(pThis) \
    ((PshcDisengaged*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(PshcDisengaged)))
#endif //__nvoc_pwr_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_PshcDisengaged(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_PshcDisengaged(PshcDisengaged**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU32 reason, NvU32 slowdownMask);
#define __objCreate_PshcDisengaged(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, reason, slowdownMask) \
    __nvoc_objCreate_PshcDisengaged((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, reason, slowdownMask)


// Wrapper macros for implementation functions
NV_STATUS pshcdisengagedConstruct_IMPL(struct PshcDisengaged *pEvent, struct EventBus *pEventBus, NvU32 reason, NvU32 slowdownMask);
#define __nvoc_pshcdisengagedConstruct(pEvent, pEventBus, reason, slowdownMask) pshcdisengagedConstruct_IMPL(pEvent, pEventBus, reason, slowdownMask)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


/* ------------------------------ PSHC Low Lifetime (Xid 164) ----------------------------- */
typedef struct PshcLowLifetimeCtx
{
    EventContextHeader header;
    NvU32 thresholdPct;  // integer percent threshold that was crossed
} PshcLowLifetimeCtx;


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_PWR_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__PshcLowLifetime;
struct NVOC_METADATA__GpuResourceThreshold;


struct PshcLowLifetime {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__PshcLowLifetime *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuResourceThreshold __nvoc_base_GpuResourceThreshold;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct Event *__nvoc_pbase_Event;    // event super^4
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^3
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super^2
    struct GpuResourceThreshold *__nvoc_pbase_GpuResourceThreshold;    // gpuresthreshold super
    struct PshcLowLifetime *__nvoc_pbase_PshcLowLifetime;    // pshclowlifetime

    // Data members
    PshcLowLifetimeCtx context;
    GpuXidEventCtx xid164;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__PshcLowLifetime {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResourceThreshold metadata__GpuResourceThreshold;
};

#ifndef __nvoc_class_id_PshcLowLifetime
#define __nvoc_class_id_PshcLowLifetime 0xa3f2acu
typedef struct PshcLowLifetime PshcLowLifetime;
#endif /* __nvoc_class_id_PshcLowLifetime */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_PshcLowLifetime;

#define __staticCast_PshcLowLifetime(pThis) \
    ((pThis)->__nvoc_pbase_PshcLowLifetime)

#ifdef __nvoc_pwr_events_h_disabled
#define __dynamicCast_PshcLowLifetime(pThis) ((PshcLowLifetime*) NULL)
#else //__nvoc_pwr_events_h_disabled
#define __dynamicCast_PshcLowLifetime(pThis) \
    ((PshcLowLifetime*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(PshcLowLifetime)))
#endif //__nvoc_pwr_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_PshcLowLifetime(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_PshcLowLifetime(PshcLowLifetime**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU32 thresholdPct);
#define __objCreate_PshcLowLifetime(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, thresholdPct) \
    __nvoc_objCreate_PshcLowLifetime((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, thresholdPct)


// Wrapper macros for implementation functions
NV_STATUS pshclowlifetimeConstruct_IMPL(struct PshcLowLifetime *pEvent, struct EventBus *pEventBus, NvU32 thresholdPct);
#define __nvoc_pshclowlifetimeConstruct(pEvent, pEventBus, thresholdPct) pshclowlifetimeConstruct_IMPL(pEvent, pEventBus, thresholdPct)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


/* ------------------------------ PSHC Zero Lifetime (Xid 165) ---------------------------- */
typedef struct PshcZeroLifetimeCtx
{
    EventContextHeader header;
} PshcZeroLifetimeCtx;


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_PWR_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__PshcZeroLifetime;
struct NVOC_METADATA__GpuResourceThreshold;


struct PshcZeroLifetime {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__PshcZeroLifetime *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuResourceThreshold __nvoc_base_GpuResourceThreshold;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct Event *__nvoc_pbase_Event;    // event super^4
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^3
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super^2
    struct GpuResourceThreshold *__nvoc_pbase_GpuResourceThreshold;    // gpuresthreshold super
    struct PshcZeroLifetime *__nvoc_pbase_PshcZeroLifetime;    // pshczerolifetime

    // Data members
    PshcZeroLifetimeCtx context;
    GpuXidEventCtx xid165;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__PshcZeroLifetime {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResourceThreshold metadata__GpuResourceThreshold;
};

#ifndef __nvoc_class_id_PshcZeroLifetime
#define __nvoc_class_id_PshcZeroLifetime 0xda3388u
typedef struct PshcZeroLifetime PshcZeroLifetime;
#endif /* __nvoc_class_id_PshcZeroLifetime */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_PshcZeroLifetime;

#define __staticCast_PshcZeroLifetime(pThis) \
    ((pThis)->__nvoc_pbase_PshcZeroLifetime)

#ifdef __nvoc_pwr_events_h_disabled
#define __dynamicCast_PshcZeroLifetime(pThis) ((PshcZeroLifetime*) NULL)
#else //__nvoc_pwr_events_h_disabled
#define __dynamicCast_PshcZeroLifetime(pThis) \
    ((PshcZeroLifetime*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(PshcZeroLifetime)))
#endif //__nvoc_pwr_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_PshcZeroLifetime(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_PshcZeroLifetime(PshcZeroLifetime**, Dynamic*, NvU32, struct EventBus *pEventBus);
#define __objCreate_PshcZeroLifetime(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus) \
    __nvoc_objCreate_PshcZeroLifetime((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus)


// Wrapper macros for implementation functions
NV_STATUS pshczerolifetimeConstruct_IMPL(struct PshcZeroLifetime *pEvent, struct EventBus *pEventBus);
#define __nvoc_pshczerolifetimeConstruct(pEvent, pEventBus) pshczerolifetimeConstruct_IMPL(pEvent, pEventBus)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


#endif // _PWR_EVENTS_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_PWR_EVENTS_NVOC_H_
