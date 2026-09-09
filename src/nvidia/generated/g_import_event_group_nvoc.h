
#ifndef _G_IMPORT_EVENT_GROUP_NVOC_H_
#define _G_IMPORT_EVENT_GROUP_NVOC_H_

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

#include "g_import_event_group_nvoc.h"

#ifndef _IMPORT_EVENT_GROUP_H_
#define _IMPORT_EVENT_GROUP_H_

#include "nvtypes.h"
#include "nvoc/event_group.h"

//
// EventGroup specialization for events reconstructed from already-rendered
// wire records (host-side import of GSP-produced operational events).
//
// The only added state is the producer's groupCursor. opevtlogAppendEventGroup
// dynamicCasts to detect import groups and adopts the cursor into the log
// entry; group size is always eventCount (we re-serialize what we have).
//

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_IMPORT_EVENT_GROUP_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__ImportEventGroup;
struct NVOC_METADATA__EventGroup;


struct ImportEventGroup {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__ImportEventGroup *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct EventGroup __nvoc_base_EventGroup;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct EventGroup *__nvoc_pbase_EventGroup;    // eventgroup super
    struct ImportEventGroup *__nvoc_pbase_ImportEventGroup;    // impevtgrp

    // Data members
    NvU64 cursor;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__ImportEventGroup {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__EventGroup metadata__EventGroup;
};

#ifndef __nvoc_class_id_ImportEventGroup
#define __nvoc_class_id_ImportEventGroup 0x18e040u
typedef struct ImportEventGroup ImportEventGroup;
#endif /* __nvoc_class_id_ImportEventGroup */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_ImportEventGroup;

#define __staticCast_ImportEventGroup(pThis) \
    ((pThis)->__nvoc_pbase_ImportEventGroup)

#ifdef __nvoc_import_event_group_h_disabled
#define __dynamicCast_ImportEventGroup(pThis) ((ImportEventGroup*) NULL)
#else //__nvoc_import_event_group_h_disabled
#define __dynamicCast_ImportEventGroup(pThis) \
    ((ImportEventGroup*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(ImportEventGroup)))
#endif //__nvoc_import_event_group_h_disabled

NV_STATUS __nvoc_objCreateDynamic_ImportEventGroup(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_ImportEventGroup(ImportEventGroup**, Dynamic*, NvU32, struct EventBus *pEventBus);
#define __objCreate_ImportEventGroup(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus) \
    __nvoc_objCreate_ImportEventGroup((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus)


// Wrapper macros for implementation functions
NV_STATUS impevtgrpConstruct_IMPL(struct ImportEventGroup *pGroup, struct EventBus *pEventBus);
#define __nvoc_impevtgrpConstruct(pGroup, pEventBus) impevtgrpConstruct_IMPL(pGroup, pEventBus)

void impevtgrpDestruct_IMPL(struct ImportEventGroup *pGroup);
#define __nvoc_impevtgrpDestruct(pGroup) impevtgrpDestruct_IMPL(pGroup)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


#endif // _IMPORT_EVENT_GROUP_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_IMPORT_EVENT_GROUP_NVOC_H_
