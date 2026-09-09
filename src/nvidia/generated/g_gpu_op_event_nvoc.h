
#ifndef _G_GPU_OP_EVENT_NVOC_H_
#define _G_GPU_OP_EVENT_NVOC_H_

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

#include "g_gpu_op_event_nvoc.h"

#ifndef _GPU_OP_EVENT_H_
#define _GPU_OP_EVENT_H_

#include "nvtypes.h"
#include "nvstatus.h"
#include "events/gpu/gpu_event_defs.h"
#include "diagnostics/op_event.h"

#define GPU_OPERATIONAL_EVENT_DEVICE_UUID_LEN 16U

typedef struct
{
    NvU8 moduleInstance;
    NvU8 chipletId;
    NvU8 migAttribution;
} GpuOperationalEventParams;


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_GPU_OP_EVENT_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__GpuOperationalEvent;
struct NVOC_METADATA__OperationalEvent;


struct GpuOperationalEvent {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__GpuOperationalEvent *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct OperationalEvent __nvoc_base_OperationalEvent;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^3
    struct Event *__nvoc_pbase_Event;    // event super^2
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt

    // Data members
    GPU_OPERATIONAL_EVENT_SCOPE scope;
    GPU_OPERATIONAL_EVENT_ORIGINATOR originator;
    GPU_OPERATIONAL_EVENT_REPORTING_SOURCE reportingSource;
    NvU8 deviceUuid[16];
    NvU64 pdi;
    NvU8 moduleInstance;
    NvU8 chipletId;
    NvU8 migAttribution;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__GpuOperationalEvent {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__OperationalEvent metadata__OperationalEvent;
};

#ifndef __nvoc_class_id_GpuOperationalEvent
#define __nvoc_class_id_GpuOperationalEvent 0x553cbau
typedef struct GpuOperationalEvent GpuOperationalEvent;
#endif /* __nvoc_class_id_GpuOperationalEvent */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuOperationalEvent;

#define __staticCast_GpuOperationalEvent(pThis) \
    ((pThis)->__nvoc_pbase_GpuOperationalEvent)

#ifdef __nvoc_gpu_op_event_h_disabled
#define __dynamicCast_GpuOperationalEvent(pThis) ((GpuOperationalEvent*) NULL)
#else //__nvoc_gpu_op_event_h_disabled
#define __dynamicCast_GpuOperationalEvent(pThis) \
    ((GpuOperationalEvent*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(GpuOperationalEvent)))
#endif //__nvoc_gpu_op_event_h_disabled

NV_STATUS __nvoc_objCreateDynamic_GpuOperationalEvent(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_GpuOperationalEvent(GpuOperationalEvent**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU32 category, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);
#define __objCreate_GpuOperationalEvent(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, category, pModuleSignature, eventCode, severity, attributes, scope, reportingSource, logLevel) \
    __nvoc_objCreate_GpuOperationalEvent((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, category, pModuleSignature, eventCode, severity, attributes, scope, reportingSource, logLevel)


// Wrapper macros for implementation functions
NV_STATUS gpuopevtConstruct_IMPL(struct GpuOperationalEvent *pEvent, struct EventBus *pEventBus, NvU32 category, const char *pModuleSignature, NvU32 eventCode, NvU32 severity, NvU32 attributes, NvU32 scope, NvU32 reportingSource, NvU32 logLevel);
#define __nvoc_gpuopevtConstruct(pEvent, pEventBus, category, pModuleSignature, eventCode, severity, attributes, scope, reportingSource, logLevel) gpuopevtConstruct_IMPL(pEvent, pEventBus, category, pModuleSignature, eventCode, severity, attributes, scope, reportingSource, logLevel)

void gpuopevtInit_IMPL(struct GpuOperationalEvent *pEvent, const GpuOperationalEventParams *pParams);
#ifdef __nvoc_gpu_op_event_h_disabled
static inline void gpuopevtInit(struct GpuOperationalEvent *pEvent, const GpuOperationalEventParams *pParams) {
    NV_ASSERT_FAILED_PRECOMP("GpuOperationalEvent was disabled!");
}
#else // __nvoc_gpu_op_event_h_disabled
#define gpuopevtInit(pEvent, pParams) gpuopevtInit_IMPL(pEvent, pParams)
#endif // __nvoc_gpu_op_event_h_disabled


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


/* ----------------------------------- Generic Event Context ----------------------------------- */

NV_STATUS gpuEventCtxSerializeToEventBufferGoeOpaqueCtx(EventContextHeader *pCtxHeader,
                                                        NV_GPU_OPERATIONAL_EVENT_CONTEXT *pOutputCtx,
                                                        NvU32 outputSize, NvU32 *pBytesWritten);

NV_STATUS gpuEventCtxSerializeToEventBufferGoeCtxType(EventContextHeader *pCtxHeader,
                                                      NV_GPU_OPERATIONAL_EVENT_CONTEXT *pOutputCtx,
                                                      NvU32 outputSize, NvU32 *pBytesWritten,
                                                      GPU_OPERATIONAL_EVENT_CTX_TYPE ctxType,
                                                      NvU16 dataFormatVersion);

NV_STATUS gpuEventCtxSerializeToCperOpaqueCtx(EventContextHeader *pCtxHeader,
                                              NV_CPER_NV_EVENT_SECTION_STATE *pState);

/* ------------------------------------- Xid Event Context ------------------------------------- */
typedef struct
{
    EventContextHeader header;
    GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID data;
} GpuXidEventCtx;

// GpuXidEventCtx implementation of serializeToEventBufferOpEventCtx 
NV_STATUS gpuXidEventCtxSerializeToEventBufferGoeCtx(EventContextHeader *pCtxHeader,
                                                     NV_GPU_OPERATIONAL_EVENT_CONTEXT *pOutput,
                                                     NvU32 outputSize, NvU32 *pBytesWritten);

// GpuXidEventCtx implementation of serializeToCperEventCtx
NV_STATUS gpuXidEventCtxSerializeToCperCtx(EventContextHeader *pCtxHeader,
                                           NV_CPER_NV_EVENT_SECTION_STATE *pState);

#endif // _GPU_OP_EVENT_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_GPU_OP_EVENT_NVOC_H_
