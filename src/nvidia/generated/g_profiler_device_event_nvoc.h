
#ifndef _G_PROFILER_DEVICE_EVENT_NVOC_H_
#define _G_PROFILER_DEVICE_EVENT_NVOC_H_

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
 *      This file contains functions to grant profiler device event capability
 *
 *   Key attributes of ProfilerDeviceEvent class:
 *   - hClient is parent of ProfilerDeviceEvent.
 *   - ProfilerDeviceEvent allocation requires a privileged client only if
 *     the platform doesn't implement this capability
 *   - RmApi lock must be held.
 *****************************************************************************/

#pragma once
#include "g_profiler_device_event_nvoc.h"

#ifndef PROFILING_DEVICE_EVENT
#define PROFILING_DEVICE_EVENT


#include "rmapi/resource.h"


// ****************************************************************************
//                          Type Definitions
// ****************************************************************************


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_PROFILER_DEVICE_EVENT_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__ProfilerDeviceEvent;
struct NVOC_METADATA__RmResource;


struct ProfilerDeviceEvent {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__ProfilerDeviceEvent *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct RmResource __nvoc_base_RmResource;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^3
    struct RsResource *__nvoc_pbase_RsResource;    // res super^2
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^2
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super
    struct ProfilerDeviceEvent *__nvoc_pbase_ProfilerDeviceEvent;    // profilerDeviceEvent

    // Data members
    NvU64 PRIVATE_FIELD(dupedCapDescriptor);
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__ProfilerDeviceEvent {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__RmResource metadata__RmResource;
};

#ifndef __nvoc_class_id_ProfilerDeviceEvent
#define __nvoc_class_id_ProfilerDeviceEvent 0xf121bfu
typedef struct ProfilerDeviceEvent ProfilerDeviceEvent;
#endif /* __nvoc_class_id_ProfilerDeviceEvent */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_ProfilerDeviceEvent;

#define __staticCast_ProfilerDeviceEvent(pThis) \
    ((pThis)->__nvoc_pbase_ProfilerDeviceEvent)

#ifdef __nvoc_profiler_device_event_h_disabled
#define __dynamicCast_ProfilerDeviceEvent(pThis) ((ProfilerDeviceEvent*) NULL)
#else //__nvoc_profiler_device_event_h_disabled
#define __dynamicCast_ProfilerDeviceEvent(pThis) \
    ((ProfilerDeviceEvent*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(ProfilerDeviceEvent)))
#endif //__nvoc_profiler_device_event_h_disabled

NV_STATUS __nvoc_objCreateDynamic_ProfilerDeviceEvent(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_ProfilerDeviceEvent(ProfilerDeviceEvent**, Dynamic*, NvU32, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __objCreate_ProfilerDeviceEvent(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams) \
    __nvoc_objCreate_ProfilerDeviceEvent((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pCallContext, pParams)


// Wrapper macros for implementation functions
NV_STATUS profilerDeviceEventConstruct_IMPL(struct ProfilerDeviceEvent *profilerDeviceEvent, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __nvoc_profilerDeviceEventConstruct(profilerDeviceEvent, pCallContext, pParams) profilerDeviceEventConstruct_IMPL(profilerDeviceEvent, pCallContext, pParams)

void profilerDeviceEventDestruct_IMPL(struct ProfilerDeviceEvent *profilerDeviceEvent);
#define __nvoc_profilerDeviceEventDestruct(profilerDeviceEvent) profilerDeviceEventDestruct_IMPL(profilerDeviceEvent)


// Wrapper macros for halified functions
#define profilerDeviceEventCanCopy_FNPTR(profilerDeviceEvent) profilerDeviceEvent->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define profilerDeviceEventCanCopy(profilerDeviceEvent) profilerDeviceEventCanCopy_DISPATCH(profilerDeviceEvent)
#define profilerDeviceEventAccessCallback_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__
#define profilerDeviceEventAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) profilerDeviceEventAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define profilerDeviceEventShareCallback_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__
#define profilerDeviceEventShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) profilerDeviceEventShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define profilerDeviceEventGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define profilerDeviceEventGetMemInterMapParams(pRmResource, pParams) profilerDeviceEventGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define profilerDeviceEventCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define profilerDeviceEventCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) profilerDeviceEventCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define profilerDeviceEventGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define profilerDeviceEventGetMemoryMappingDescriptor(pRmResource, ppMemDesc) profilerDeviceEventGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define profilerDeviceEventControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__
#define profilerDeviceEventControlSerialization_Prologue(pResource, pCallContext, pParams) profilerDeviceEventControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define profilerDeviceEventControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__
#define profilerDeviceEventControlSerialization_Epilogue(pResource, pCallContext, pParams) profilerDeviceEventControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define profilerDeviceEventControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__
#define profilerDeviceEventControl_Prologue(pResource, pCallContext, pParams) profilerDeviceEventControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define profilerDeviceEventControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__
#define profilerDeviceEventControl_Epilogue(pResource, pCallContext, pParams) profilerDeviceEventControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define profilerDeviceEventIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define profilerDeviceEventIsDuplicate(pResource, hMemory, pDuplicate) profilerDeviceEventIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define profilerDeviceEventPreDestruct_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define profilerDeviceEventPreDestruct(pResource) profilerDeviceEventPreDestruct_DISPATCH(pResource)
#define profilerDeviceEventControl_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__
#define profilerDeviceEventControl(pResource, pCallContext, pParams) profilerDeviceEventControl_DISPATCH(pResource, pCallContext, pParams)
#define profilerDeviceEventControlFilter_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define profilerDeviceEventControlFilter(pResource, pCallContext, pParams) profilerDeviceEventControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define profilerDeviceEventMap_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__
#define profilerDeviceEventMap(pResource, pCallContext, pParams, pCpuMapping) profilerDeviceEventMap_DISPATCH(pResource, pCallContext, pParams, pCpuMapping)
#define profilerDeviceEventUnmap_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__
#define profilerDeviceEventUnmap(pResource, pCallContext, pCpuMapping) profilerDeviceEventUnmap_DISPATCH(pResource, pCallContext, pCpuMapping)
#define profilerDeviceEventIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define profilerDeviceEventIsPartialUnmapSupported(pResource) profilerDeviceEventIsPartialUnmapSupported_DISPATCH(pResource)
#define profilerDeviceEventMapTo_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define profilerDeviceEventMapTo(pResource, pParams) profilerDeviceEventMapTo_DISPATCH(pResource, pParams)
#define profilerDeviceEventUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define profilerDeviceEventUnmapFrom(pResource, pParams) profilerDeviceEventUnmapFrom_DISPATCH(pResource, pParams)
#define profilerDeviceEventGetRefCount_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define profilerDeviceEventGetRefCount(pResource) profilerDeviceEventGetRefCount_DISPATCH(pResource)
#define profilerDeviceEventAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define profilerDeviceEventAddAdditionalDependants(pClient, pResource, pReference) profilerDeviceEventAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NvBool profilerDeviceEventCanCopy_DISPATCH(struct ProfilerDeviceEvent *profilerDeviceEvent) {
    return profilerDeviceEvent->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__(staticCast(profilerDeviceEvent, RsResource));
}

static inline NvBool profilerDeviceEventAccessCallback_DISPATCH(struct ProfilerDeviceEvent *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__(staticCast(pResource, RsResource), pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool profilerDeviceEventShareCallback_DISPATCH(struct ProfilerDeviceEvent *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__(staticCast(pResource, RsResource), pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS profilerDeviceEventGetMemInterMapParams_DISPATCH(struct ProfilerDeviceEvent *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__(staticCast(pRmResource, RmResource), pParams);
}

static inline NV_STATUS profilerDeviceEventCheckMemInterUnmap_DISPATCH(struct ProfilerDeviceEvent *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__(staticCast(pRmResource, RmResource), bSubdeviceHandleProvided);
}

static inline NV_STATUS profilerDeviceEventGetMemoryMappingDescriptor_DISPATCH(struct ProfilerDeviceEvent *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__(staticCast(pRmResource, RmResource), ppMemDesc);
}

static inline NV_STATUS profilerDeviceEventControlSerialization_Prologue_DISPATCH(struct ProfilerDeviceEvent *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void profilerDeviceEventControlSerialization_Epilogue_DISPATCH(struct ProfilerDeviceEvent *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS profilerDeviceEventControl_Prologue_DISPATCH(struct ProfilerDeviceEvent *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void profilerDeviceEventControl_Epilogue_DISPATCH(struct ProfilerDeviceEvent *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS profilerDeviceEventIsDuplicate_DISPATCH(struct ProfilerDeviceEvent *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__(staticCast(pResource, RsResource), hMemory, pDuplicate);
}

static inline void profilerDeviceEventPreDestruct_DISPATCH(struct ProfilerDeviceEvent *pResource) {
    pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__(staticCast(pResource, RsResource));
}

static inline NV_STATUS profilerDeviceEventControl_DISPATCH(struct ProfilerDeviceEvent *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS profilerDeviceEventControlFilter_DISPATCH(struct ProfilerDeviceEvent *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS profilerDeviceEventMap_DISPATCH(struct ProfilerDeviceEvent *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__(staticCast(pResource, RsResource), pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS profilerDeviceEventUnmap_DISPATCH(struct ProfilerDeviceEvent *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__(staticCast(pResource, RsResource), pCallContext, pCpuMapping);
}

static inline NvBool profilerDeviceEventIsPartialUnmapSupported_DISPATCH(struct ProfilerDeviceEvent *pResource) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__(staticCast(pResource, RsResource));
}

static inline NV_STATUS profilerDeviceEventMapTo_DISPATCH(struct ProfilerDeviceEvent *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__(staticCast(pResource, RsResource), pParams);
}

static inline NV_STATUS profilerDeviceEventUnmapFrom_DISPATCH(struct ProfilerDeviceEvent *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__(staticCast(pResource, RsResource), pParams);
}

static inline NvU32 profilerDeviceEventGetRefCount_DISPATCH(struct ProfilerDeviceEvent *pResource) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__(staticCast(pResource, RsResource));
}

static inline void profilerDeviceEventAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct ProfilerDeviceEvent *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__(pClient, staticCast(pResource, RsResource), pReference);
}

// Virtual method declarations and/or inline definitions
NvBool profilerDeviceEventCanCopy_IMPL(struct ProfilerDeviceEvent *profilerDeviceEvent);

// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD



#endif // PROFILING_DEVICE_EVENT

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_PROFILER_DEVICE_EVENT_NVOC_H_
