
#ifndef _G_GPU_OP_EVENT_IMPORT_NVOC_H_
#define _G_GPU_OP_EVENT_IMPORT_NVOC_H_

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

#include "rmconfig.h"

#include "g_gpu_op_event_import_nvoc.h"

#ifndef _GPU_OP_EVENT_IMPORT_H_
#define _GPU_OP_EVENT_IMPORT_H_

#include "nvtypes.h"
#include "nvstatus.h"
#include "nvos.h"                                // NVOS10_EVENT_KERNEL_CALLBACK_EX
#include "nvport/nvport.h"                       // PORT_MUTEX
#include "class/cl90cd.h"                        // NV_EVENT_BUFFER_HEADER
#include "nvoc/object.h"

struct OBJGPU;

#ifndef __nvoc_class_id_OBJGPU
#define __nvoc_class_id_OBJGPU 0x7ef3cbu
typedef struct OBJGPU OBJGPU;
#endif /* __nvoc_class_id_OBJGPU */


struct EventGroup;

#ifndef __nvoc_class_id_EventGroup
#define __nvoc_class_id_EventGroup 0x5e0b7cu
typedef struct EventGroup EventGroup;
#endif /* __nvoc_class_id_EventGroup */


struct ImportEventGroup;

#ifndef __nvoc_class_id_ImportEventGroup
#define __nvoc_class_id_ImportEventGroup 0x18e040u
typedef struct ImportEventGroup ImportEventGroup;
#endif /* __nvoc_class_id_ImportEventGroup */



//
// Worker-params held by the drain workitem instead of the importer itself, so the callback can hand
// off without dereferencing pImporter. Owned 1:1 by GpuOpEventImport: allocated in construct and
// freed in destruct -- which runs under the GPU lock, after the work-queue flush, so no queued
// workitem can still dispatch against it (the drain workitem sets bDropOnUnloadQueueFlush, so a
// still-queued one is dropped at unload). pLifetimeMutex guards the pImporter handoff: teardown
// nulls pImporter under it so a callback/workitem reading it gets NULL-or-valid, never torn. The
// GPU lock -- not this mutex -- protects the drain / RM state.
//
typedef struct GoeImportWorkerParams
{
    PORT_MUTEX              *pLifetimeMutex;
    struct GpuOpEventImport *pImporter;  // guarded by pLifetimeMutex
} GoeImportWorkerParams;

//
// Host-side importer of GSP-rendered operational-event wire records. One per GPU: constructed
// during GPU bring-up and destroyed on teardown. Setup is soft-fail by contract: errors latch
// bStreamingDisabled and init always continues; callback/workitem early-exit on the flag.
//

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_GPU_OP_EVENT_IMPORT_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__GpuOpEventImport;
struct NVOC_METADATA__Object;


struct GpuOpEventImport {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__GpuOpEventImport *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct Object __nvoc_base_Object;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super
    struct GpuOpEventImport *__nvoc_pbase_GpuOpEventImport;    // goeimport

    // Data members
    struct OBJGPU *pGpu;
    NvBool bStreamingDisabled;
    NvHandle hClient;
    NvHandle hDevice;
    NvHandle hSubdevice;
    NvHandle hHeaderMemory;
    NvHandle hRecordMemory;
    NvHandle hVardataMemory;
    NvHandle hPhysEventBuffer;
    NvHandle hPhysBind;
    NvBool bPhysBindAllocated;
    NV_EVENT_BUFFER_HEADER *pHeader;
    NvU8 *pRecords;
    NvU8 *pVardata;
    NVOS10_EVENT_KERNEL_CALLBACK_EX callback;
    NvU32 vardataGet;
    NvBool bFatalErrorRequested;
    struct ImportEventGroup *pSpanGroup;
    NvU64 spanCursor;
    NvU8 spanGroupSize;
    NvU8 spanObservedCount;
    NvS16 spanLastIndex;
    GoeImportWorkerParams *pWorkerParams;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__GpuOpEventImport {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__Object metadata__Object;
};

#ifndef __nvoc_class_id_GpuOpEventImport
#define __nvoc_class_id_GpuOpEventImport 0x1c9266u
typedef struct GpuOpEventImport GpuOpEventImport;
#endif /* __nvoc_class_id_GpuOpEventImport */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOpEventImport;

#define __staticCast_GpuOpEventImport(pThis) \
    ((pThis)->__nvoc_pbase_GpuOpEventImport)

#ifdef __nvoc_gpu_op_event_import_h_disabled
#define __dynamicCast_GpuOpEventImport(pThis) ((GpuOpEventImport*) NULL)
#else //__nvoc_gpu_op_event_import_h_disabled
#define __dynamicCast_GpuOpEventImport(pThis) \
    ((GpuOpEventImport*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(GpuOpEventImport)))
#endif //__nvoc_gpu_op_event_import_h_disabled

NV_STATUS __nvoc_objCreateDynamic_GpuOpEventImport(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_GpuOpEventImport(GpuOpEventImport**, Dynamic*, NvU32, struct OBJGPU *pGpu);
#define __objCreate_GpuOpEventImport(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pGpu) \
    __nvoc_objCreate_GpuOpEventImport((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pGpu)


// Wrapper macros for implementation functions
NV_STATUS goeimportConstruct_IMPL(struct GpuOpEventImport *pImporter, struct OBJGPU *pGpu);
#define __nvoc_goeimportConstruct(pImporter, pGpu) goeimportConstruct_IMPL(pImporter, pGpu)

void goeimportDestruct_IMPL(struct GpuOpEventImport *pImporter);
#define __nvoc_goeimportDestruct(pImporter) goeimportDestruct_IMPL(pImporter)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


#endif // _GPU_OP_EVENT_IMPORT_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_GPU_OP_EVENT_IMPORT_NVOC_H_
