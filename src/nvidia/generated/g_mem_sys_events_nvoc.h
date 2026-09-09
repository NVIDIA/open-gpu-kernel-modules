
#ifndef _G_MEM_SYS_EVENTS_NVOC_H_
#define _G_MEM_SYS_EVENTS_NVOC_H_

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

#include "g_mem_sys_events_nvoc.h"

#ifndef _MEM_SYS_EVENTS_H_
#define _MEM_SYS_EVENTS_H_

#include "events/gpu/gpu_events.h"
#include "events/gpu/mem_sys/mem_sys_event_defs.h"

typedef struct MemSysTimeoutCtx
{
    EventContextHeader header;
    NvU32 maintOpError;
} MemSysTimeoutCtx;


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_MEM_SYS_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__MemSysTimeout;
struct NVOC_METADATA__GpuMemoryIntegrityError;


struct MemSysTimeout {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__MemSysTimeout *__nvoc_metadata_ptr;
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
    struct MemSysTimeout *__nvoc_pbase_MemSysTimeout;    // memsysto

    // Data members
    MemSysTimeoutCtx context;
    GpuXidEventCtx xid175;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__MemSysTimeout {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuMemoryIntegrityError metadata__GpuMemoryIntegrityError;
};

#ifndef __nvoc_class_id_MemSysTimeout
#define __nvoc_class_id_MemSysTimeout 0xa6fed2u
typedef struct MemSysTimeout MemSysTimeout;
#endif /* __nvoc_class_id_MemSysTimeout */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_MemSysTimeout;

#define __staticCast_MemSysTimeout(pThis) \
    ((pThis)->__nvoc_pbase_MemSysTimeout)

#ifdef __nvoc_mem_sys_events_h_disabled
#define __dynamicCast_MemSysTimeout(pThis) ((MemSysTimeout*) NULL)
#else //__nvoc_mem_sys_events_h_disabled
#define __dynamicCast_MemSysTimeout(pThis) \
    ((MemSysTimeout*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(MemSysTimeout)))
#endif //__nvoc_mem_sys_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_MemSysTimeout(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_MemSysTimeout(MemSysTimeout**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU32 severity, NvU32 maintOpError);
#define __objCreate_MemSysTimeout(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, severity, maintOpError) \
    __nvoc_objCreate_MemSysTimeout((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, severity, maintOpError)


// Wrapper macros for implementation functions
NV_STATUS memsystoConstruct_IMPL(struct MemSysTimeout *pEvent, struct EventBus *pEventBus, NvU32 severity, NvU32 maintOpError);
#define __nvoc_memsystoConstruct(pEvent, pEventBus, severity, maintOpError) memsystoConstruct_IMPL(pEvent, pEventBus, severity, maintOpError)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


#endif // _MEM_SYS_EVENTS_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_MEM_SYS_EVENTS_NVOC_H_
