
#ifndef _G_PROFILER_CONTEXT_EVENT_NVOC_H_
#define _G_PROFILER_CONTEXT_EVENT_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/******************************************************************************
 *
 *   Description:
 *      This file contains functions to grant profiler context event capability
 *
 *   Key attributes of ProfilerContextEvent class:
 *   - hClient is parent of ProfilerContextEvent.
 *   - ProfilerContextEvent allocation requires a privileged client only if
 *     the platform doesn't implement this capability
 *   - RmApi lock must be held.
 *****************************************************************************/

#pragma once
#include "g_profiler_context_event_nvoc.h"

#ifndef PROFILER_CONTEXT_EVENT_H
#define PROFILER_CONTEXT_EVENT_H


#include "rmapi/resource.h"

// ****************************************************************************
//                          Type Definitions
// ****************************************************************************


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_PROFILER_CONTEXT_EVENT_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__ProfilerContextEvent;
struct NVOC_METADATA__RmResource;


struct ProfilerContextEvent {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__ProfilerContextEvent *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct RmResource __nvoc_base_RmResource;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^3
    struct RsResource *__nvoc_pbase_RsResource;    // res super^2
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^2
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super
    struct ProfilerContextEvent *__nvoc_pbase_ProfilerContextEvent;    // profilerContextEvent

    // Data members
    NvU64 PRIVATE_FIELD(dupedCapDescriptor);
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__ProfilerContextEvent {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__RmResource metadata__RmResource;
};

#ifndef __nvoc_class_id_ProfilerContextEvent
#define __nvoc_class_id_ProfilerContextEvent 0x98abfeu
typedef struct ProfilerContextEvent ProfilerContextEvent;
#endif /* __nvoc_class_id_ProfilerContextEvent */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_ProfilerContextEvent;

#define __staticCast_ProfilerContextEvent(pThis) \
    ((pThis)->__nvoc_pbase_ProfilerContextEvent)

#ifdef __nvoc_profiler_context_event_h_disabled
#define __dynamicCast_ProfilerContextEvent(pThis) ((ProfilerContextEvent*) NULL)
#else //__nvoc_profiler_context_event_h_disabled
#define __dynamicCast_ProfilerContextEvent(pThis) \
    ((ProfilerContextEvent*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(ProfilerContextEvent)))
#endif //__nvoc_profiler_context_event_h_disabled

NV_STATUS __nvoc_objCreateDynamic_ProfilerContextEvent(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_ProfilerContextEvent(ProfilerContextEvent**, Dynamic*, NvU32, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __objCreate_ProfilerContextEvent(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams) \
    __nvoc_objCreate_ProfilerContextEvent((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pCallContext, pParams)


// Wrapper macros for implementation functions
NV_STATUS profilerContextEventConstruct_IMPL(struct ProfilerContextEvent *profilerContextEvent, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __nvoc_profilerContextEventConstruct(profilerContextEvent, pCallContext, pParams) profilerContextEventConstruct_IMPL(profilerContextEvent, pCallContext, pParams)

void profilerContextEventDestruct_IMPL(struct ProfilerContextEvent *profilerContextEvent);
#define __nvoc_profilerContextEventDestruct(profilerContextEvent) profilerContextEventDestruct_IMPL(profilerContextEvent)


// Wrapper macros for halified functions
#define profilerContextEventCanCopy_FNPTR(profilerContextEvent) profilerContextEvent->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define profilerContextEventCanCopy(profilerContextEvent) profilerContextEventCanCopy_DISPATCH(profilerContextEvent)
#define profilerContextEventAccessCallback_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__
#define profilerContextEventAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) profilerContextEventAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define profilerContextEventShareCallback_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__
#define profilerContextEventShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) profilerContextEventShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define profilerContextEventGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define profilerContextEventGetMemInterMapParams(pRmResource, pParams) profilerContextEventGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define profilerContextEventCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define profilerContextEventCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) profilerContextEventCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define profilerContextEventGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define profilerContextEventGetMemoryMappingDescriptor(pRmResource, ppMemDesc) profilerContextEventGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define profilerContextEventControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__
#define profilerContextEventControlSerialization_Prologue(pResource, pCallContext, pParams) profilerContextEventControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define profilerContextEventControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__
#define profilerContextEventControlSerialization_Epilogue(pResource, pCallContext, pParams) profilerContextEventControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define profilerContextEventControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__
#define profilerContextEventControl_Prologue(pResource, pCallContext, pParams) profilerContextEventControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define profilerContextEventControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__
#define profilerContextEventControl_Epilogue(pResource, pCallContext, pParams) profilerContextEventControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define profilerContextEventIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define profilerContextEventIsDuplicate(pResource, hMemory, pDuplicate) profilerContextEventIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define profilerContextEventPreDestruct_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define profilerContextEventPreDestruct(pResource) profilerContextEventPreDestruct_DISPATCH(pResource)
#define profilerContextEventControl_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__
#define profilerContextEventControl(pResource, pCallContext, pParams) profilerContextEventControl_DISPATCH(pResource, pCallContext, pParams)
#define profilerContextEventControlFilter_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define profilerContextEventControlFilter(pResource, pCallContext, pParams) profilerContextEventControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define profilerContextEventMap_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__
#define profilerContextEventMap(pResource, pCallContext, pParams, pCpuMapping) profilerContextEventMap_DISPATCH(pResource, pCallContext, pParams, pCpuMapping)
#define profilerContextEventUnmap_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__
#define profilerContextEventUnmap(pResource, pCallContext, pCpuMapping) profilerContextEventUnmap_DISPATCH(pResource, pCallContext, pCpuMapping)
#define profilerContextEventIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define profilerContextEventIsPartialUnmapSupported(pResource) profilerContextEventIsPartialUnmapSupported_DISPATCH(pResource)
#define profilerContextEventMapTo_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define profilerContextEventMapTo(pResource, pParams) profilerContextEventMapTo_DISPATCH(pResource, pParams)
#define profilerContextEventUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define profilerContextEventUnmapFrom(pResource, pParams) profilerContextEventUnmapFrom_DISPATCH(pResource, pParams)
#define profilerContextEventGetRefCount_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define profilerContextEventGetRefCount(pResource) profilerContextEventGetRefCount_DISPATCH(pResource)
#define profilerContextEventAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define profilerContextEventAddAdditionalDependants(pClient, pResource, pReference) profilerContextEventAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NvBool profilerContextEventCanCopy_DISPATCH(struct ProfilerContextEvent *profilerContextEvent) {
    return profilerContextEvent->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__(staticCast(profilerContextEvent, RsResource));
}

static inline NvBool profilerContextEventAccessCallback_DISPATCH(struct ProfilerContextEvent *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__(staticCast(pResource, RsResource), pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool profilerContextEventShareCallback_DISPATCH(struct ProfilerContextEvent *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__(staticCast(pResource, RsResource), pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS profilerContextEventGetMemInterMapParams_DISPATCH(struct ProfilerContextEvent *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__(staticCast(pRmResource, RmResource), pParams);
}

static inline NV_STATUS profilerContextEventCheckMemInterUnmap_DISPATCH(struct ProfilerContextEvent *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__(staticCast(pRmResource, RmResource), bSubdeviceHandleProvided);
}

static inline NV_STATUS profilerContextEventGetMemoryMappingDescriptor_DISPATCH(struct ProfilerContextEvent *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__(staticCast(pRmResource, RmResource), ppMemDesc);
}

static inline NV_STATUS profilerContextEventControlSerialization_Prologue_DISPATCH(struct ProfilerContextEvent *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void profilerContextEventControlSerialization_Epilogue_DISPATCH(struct ProfilerContextEvent *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS profilerContextEventControl_Prologue_DISPATCH(struct ProfilerContextEvent *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void profilerContextEventControl_Epilogue_DISPATCH(struct ProfilerContextEvent *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS profilerContextEventIsDuplicate_DISPATCH(struct ProfilerContextEvent *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__(staticCast(pResource, RsResource), hMemory, pDuplicate);
}

static inline void profilerContextEventPreDestruct_DISPATCH(struct ProfilerContextEvent *pResource) {
    pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__(staticCast(pResource, RsResource));
}

static inline NV_STATUS profilerContextEventControl_DISPATCH(struct ProfilerContextEvent *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS profilerContextEventControlFilter_DISPATCH(struct ProfilerContextEvent *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS profilerContextEventMap_DISPATCH(struct ProfilerContextEvent *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__(staticCast(pResource, RsResource), pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS profilerContextEventUnmap_DISPATCH(struct ProfilerContextEvent *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__(staticCast(pResource, RsResource), pCallContext, pCpuMapping);
}

static inline NvBool profilerContextEventIsPartialUnmapSupported_DISPATCH(struct ProfilerContextEvent *pResource) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__(staticCast(pResource, RsResource));
}

static inline NV_STATUS profilerContextEventMapTo_DISPATCH(struct ProfilerContextEvent *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__(staticCast(pResource, RsResource), pParams);
}

static inline NV_STATUS profilerContextEventUnmapFrom_DISPATCH(struct ProfilerContextEvent *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__(staticCast(pResource, RsResource), pParams);
}

static inline NvU32 profilerContextEventGetRefCount_DISPATCH(struct ProfilerContextEvent *pResource) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__(staticCast(pResource, RsResource));
}

static inline void profilerContextEventAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct ProfilerContextEvent *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__(pClient, staticCast(pResource, RsResource), pReference);
}

// Virtual method declarations and/or inline definitions
NvBool profilerContextEventCanCopy_IMPL(struct ProfilerContextEvent *profilerContextEvent);

// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


#endif // PROFILER_CONTEXT_EVENT_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_PROFILER_CONTEXT_EVENT_NVOC_H_
