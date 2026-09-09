
#ifndef _G_FSP_EVENTS_NVOC_H_
#define _G_FSP_EVENTS_NVOC_H_

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

#include "g_fsp_events_nvoc.h"

#ifndef _FSP_EVENTS_H_
#define _FSP_EVENTS_H_

#include "events/gpu/gpu_events.h"
#include "events/gpu/fsp/fsp_event_ctx_defs.h"
#include "events/gpu/fsp/fsp_event_defs.h"

typedef struct FspBootTimeoutCtx
{
    EventContextHeader header;
    FSP_BOOT_TIMEOUT_DATA data;
} FspBootTimeoutCtx;


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_FSP_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__FspBootTimeout;
struct NVOC_METADATA__GpuTimeout;


struct FspBootTimeout {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__FspBootTimeout *__nvoc_metadata_ptr;
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
    struct FspBootTimeout *__nvoc_pbase_FspBootTimeout;    // fspbootto

    // Data members
    FspBootTimeoutCtx context;
    GpuXidEventCtx xid143;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__FspBootTimeout {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuTimeout metadata__GpuTimeout;
};

#ifndef __nvoc_class_id_FspBootTimeout
#define __nvoc_class_id_FspBootTimeout 0x8a29efu
typedef struct FspBootTimeout FspBootTimeout;
#endif /* __nvoc_class_id_FspBootTimeout */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_FspBootTimeout;

#define __staticCast_FspBootTimeout(pThis) \
    ((pThis)->__nvoc_pbase_FspBootTimeout)

#ifdef __nvoc_fsp_events_h_disabled
#define __dynamicCast_FspBootTimeout(pThis) ((FspBootTimeout*) NULL)
#else //__nvoc_fsp_events_h_disabled
#define __dynamicCast_FspBootTimeout(pThis) \
    ((FspBootTimeout*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(FspBootTimeout)))
#endif //__nvoc_fsp_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_FspBootTimeout(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_FspBootTimeout(FspBootTimeout**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU64 timeoutNs, NvU64 elapsedNs, NV_STATUS errorStatus, NvU32 fspBootComplete, NvU32 scratch0, NvU32 scratch1, NvU32 scratch2, NvU32 scratch3);
#define __objCreate_FspBootTimeout(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, timeoutNs, elapsedNs, errorStatus, fspBootComplete, scratch0, scratch1, scratch2, scratch3) \
    __nvoc_objCreate_FspBootTimeout((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, timeoutNs, elapsedNs, errorStatus, fspBootComplete, scratch0, scratch1, scratch2, scratch3)


// Wrapper macros for implementation functions
NV_STATUS fspboottoConstruct_IMPL(struct FspBootTimeout *pEvent, struct EventBus *pEventBus, NvU64 timeoutNs, NvU64 elapsedNs, NV_STATUS errorStatus, NvU32 fspBootComplete, NvU32 scratch0, NvU32 scratch1, NvU32 scratch2, NvU32 scratch3);
#define __nvoc_fspboottoConstruct(pEvent, pEventBus, timeoutNs, elapsedNs, errorStatus, fspBootComplete, scratch0, scratch1, scratch2, scratch3) fspboottoConstruct_IMPL(pEvent, pEventBus, timeoutNs, elapsedNs, errorStatus, fspBootComplete, scratch0, scratch1, scratch2, scratch3)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


typedef struct FspFuseErrorCtx
{
    EventContextHeader header;
    FSP_FUSE_ERROR_DATA data;
} FspFuseErrorCtx;


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_FSP_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__FspFuseError;
struct NVOC_METADATA__GpuFirmwareFault;


struct FspFuseError {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__FspFuseError *__nvoc_metadata_ptr;
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
    struct FspFuseError *__nvoc_pbase_FspFuseError;    // fspfuseerr

    // Data members
    FspFuseErrorCtx context;
    GpuXidEventCtx xid143;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__FspFuseError {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuFirmwareFault metadata__GpuFirmwareFault;
};

#ifndef __nvoc_class_id_FspFuseError
#define __nvoc_class_id_FspFuseError 0x5afb18u
typedef struct FspFuseError FspFuseError;
#endif /* __nvoc_class_id_FspFuseError */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_FspFuseError;

#define __staticCast_FspFuseError(pThis) \
    ((pThis)->__nvoc_pbase_FspFuseError)

#ifdef __nvoc_fsp_events_h_disabled
#define __dynamicCast_FspFuseError(pThis) ((FspFuseError*) NULL)
#else //__nvoc_fsp_events_h_disabled
#define __dynamicCast_FspFuseError(pThis) \
    ((FspFuseError*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(FspFuseError)))
#endif //__nvoc_fsp_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_FspFuseError(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_FspFuseError(FspFuseError**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU32 fuseStatus);
#define __objCreate_FspFuseError(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, fuseStatus) \
    __nvoc_objCreate_FspFuseError((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, fuseStatus)


// Wrapper macros for implementation functions
NV_STATUS fspfuseerrConstruct_IMPL(struct FspFuseError *pEvent, struct EventBus *pEventBus, NvU32 fuseStatus);
#define __nvoc_fspfuseerrConstruct(pEvent, pEventBus, fuseStatus) fspfuseerrConstruct_IMPL(pEvent, pEventBus, fuseStatus)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


#endif // _FSP_EVENTS_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_FSP_EVENTS_NVOC_H_
