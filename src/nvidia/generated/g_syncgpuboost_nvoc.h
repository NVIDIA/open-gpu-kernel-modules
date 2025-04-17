
#ifndef _G_SYNCGPUBOOST_NVOC_H_
#define _G_SYNCGPUBOOST_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2016-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_syncgpuboost_nvoc.h"

#ifndef _SYNCGPUBOOST_H_
#define _SYNCGPUBOOST_H_

#include "resserv/resserv.h"
#include "nvoc/prelude.h"
#include "rmapi/resource.h"
#include "class/cl0060.h"


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_SYNCGPUBOOST_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__SyncGpuBoost;
struct NVOC_METADATA__RmResource;
struct NVOC_VTABLE__SyncGpuBoost;


struct SyncGpuBoost {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__SyncGpuBoost *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct RmResource __nvoc_base_RmResource;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^3
    struct RsResource *__nvoc_pbase_RsResource;    // res super^2
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^2
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super
    struct SyncGpuBoost *__nvoc_pbase_SyncGpuBoost;    // syncgpuboost

    // Data members
    NvU32 gpuBoostGroupId;
};


// Vtable with 21 per-class function pointers
struct NVOC_VTABLE__SyncGpuBoost {
    NvBool (*__syncgpuboostAccessCallback__)(struct SyncGpuBoost * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (rmres)
    NvBool (*__syncgpuboostShareCallback__)(struct SyncGpuBoost * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__syncgpuboostGetMemInterMapParams__)(struct SyncGpuBoost * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__syncgpuboostCheckMemInterUnmap__)(struct SyncGpuBoost * /*this*/, NvBool);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__syncgpuboostGetMemoryMappingDescriptor__)(struct SyncGpuBoost * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__syncgpuboostControlSerialization_Prologue__)(struct SyncGpuBoost * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (rmres)
    void (*__syncgpuboostControlSerialization_Epilogue__)(struct SyncGpuBoost * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__syncgpuboostControl_Prologue__)(struct SyncGpuBoost * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (rmres)
    void (*__syncgpuboostControl_Epilogue__)(struct SyncGpuBoost * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (rmres)
    NvBool (*__syncgpuboostCanCopy__)(struct SyncGpuBoost * /*this*/);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__syncgpuboostIsDuplicate__)(struct SyncGpuBoost * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (rmres)
    void (*__syncgpuboostPreDestruct__)(struct SyncGpuBoost * /*this*/);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__syncgpuboostControl__)(struct SyncGpuBoost * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__syncgpuboostControlFilter__)(struct SyncGpuBoost * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__syncgpuboostMap__)(struct SyncGpuBoost * /*this*/, struct CALL_CONTEXT *, RS_CPU_MAP_PARAMS *, RsCpuMapping *);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__syncgpuboostUnmap__)(struct SyncGpuBoost * /*this*/, struct CALL_CONTEXT *, RsCpuMapping *);  // virtual inherited (res) base (rmres)
    NvBool (*__syncgpuboostIsPartialUnmapSupported__)(struct SyncGpuBoost * /*this*/);  // inline virtual inherited (res) base (rmres) body
    NV_STATUS (*__syncgpuboostMapTo__)(struct SyncGpuBoost * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__syncgpuboostUnmapFrom__)(struct SyncGpuBoost * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (rmres)
    NvU32 (*__syncgpuboostGetRefCount__)(struct SyncGpuBoost * /*this*/);  // virtual inherited (res) base (rmres)
    void (*__syncgpuboostAddAdditionalDependants__)(struct RsClient *, struct SyncGpuBoost * /*this*/, RsResourceRef *);  // virtual inherited (res) base (rmres)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__SyncGpuBoost {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__RmResource metadata__RmResource;
    const struct NVOC_VTABLE__SyncGpuBoost vtable;
};

#ifndef __NVOC_CLASS_SyncGpuBoost_TYPEDEF__
#define __NVOC_CLASS_SyncGpuBoost_TYPEDEF__
typedef struct SyncGpuBoost SyncGpuBoost;
#endif /* __NVOC_CLASS_SyncGpuBoost_TYPEDEF__ */

#ifndef __nvoc_class_id_SyncGpuBoost
#define __nvoc_class_id_SyncGpuBoost 0xc7e30b
#endif /* __nvoc_class_id_SyncGpuBoost */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_SyncGpuBoost;

#define __staticCast_SyncGpuBoost(pThis) \
    ((pThis)->__nvoc_pbase_SyncGpuBoost)

#ifdef __nvoc_syncgpuboost_h_disabled
#define __dynamicCast_SyncGpuBoost(pThis) ((SyncGpuBoost*) NULL)
#else //__nvoc_syncgpuboost_h_disabled
#define __dynamicCast_SyncGpuBoost(pThis) \
    ((SyncGpuBoost*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(SyncGpuBoost)))
#endif //__nvoc_syncgpuboost_h_disabled

NV_STATUS __nvoc_objCreateDynamic_SyncGpuBoost(SyncGpuBoost**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_SyncGpuBoost(SyncGpuBoost**, Dynamic*, NvU32, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __objCreate_SyncGpuBoost(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_SyncGpuBoost((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define syncgpuboostAccessCallback_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresAccessCallback__
#define syncgpuboostAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) syncgpuboostAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define syncgpuboostShareCallback_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresShareCallback__
#define syncgpuboostShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) syncgpuboostShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define syncgpuboostGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define syncgpuboostGetMemInterMapParams(pRmResource, pParams) syncgpuboostGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define syncgpuboostCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define syncgpuboostCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) syncgpuboostCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define syncgpuboostGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define syncgpuboostGetMemoryMappingDescriptor(pRmResource, ppMemDesc) syncgpuboostGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define syncgpuboostControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Prologue__
#define syncgpuboostControlSerialization_Prologue(pResource, pCallContext, pParams) syncgpuboostControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define syncgpuboostControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Epilogue__
#define syncgpuboostControlSerialization_Epilogue(pResource, pCallContext, pParams) syncgpuboostControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define syncgpuboostControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Prologue__
#define syncgpuboostControl_Prologue(pResource, pCallContext, pParams) syncgpuboostControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define syncgpuboostControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Epilogue__
#define syncgpuboostControl_Epilogue(pResource, pCallContext, pParams) syncgpuboostControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define syncgpuboostCanCopy_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define syncgpuboostCanCopy(pResource) syncgpuboostCanCopy_DISPATCH(pResource)
#define syncgpuboostIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define syncgpuboostIsDuplicate(pResource, hMemory, pDuplicate) syncgpuboostIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define syncgpuboostPreDestruct_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define syncgpuboostPreDestruct(pResource) syncgpuboostPreDestruct_DISPATCH(pResource)
#define syncgpuboostControl_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__
#define syncgpuboostControl(pResource, pCallContext, pParams) syncgpuboostControl_DISPATCH(pResource, pCallContext, pParams)
#define syncgpuboostControlFilter_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define syncgpuboostControlFilter(pResource, pCallContext, pParams) syncgpuboostControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define syncgpuboostMap_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__
#define syncgpuboostMap(pResource, pCallContext, pParams, pCpuMapping) syncgpuboostMap_DISPATCH(pResource, pCallContext, pParams, pCpuMapping)
#define syncgpuboostUnmap_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__
#define syncgpuboostUnmap(pResource, pCallContext, pCpuMapping) syncgpuboostUnmap_DISPATCH(pResource, pCallContext, pCpuMapping)
#define syncgpuboostIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define syncgpuboostIsPartialUnmapSupported(pResource) syncgpuboostIsPartialUnmapSupported_DISPATCH(pResource)
#define syncgpuboostMapTo_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define syncgpuboostMapTo(pResource, pParams) syncgpuboostMapTo_DISPATCH(pResource, pParams)
#define syncgpuboostUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define syncgpuboostUnmapFrom(pResource, pParams) syncgpuboostUnmapFrom_DISPATCH(pResource, pParams)
#define syncgpuboostGetRefCount_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define syncgpuboostGetRefCount(pResource) syncgpuboostGetRefCount_DISPATCH(pResource)
#define syncgpuboostAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define syncgpuboostAddAdditionalDependants(pClient, pResource, pReference) syncgpuboostAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NvBool syncgpuboostAccessCallback_DISPATCH(struct SyncGpuBoost *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_metadata_ptr->vtable.__syncgpuboostAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool syncgpuboostShareCallback_DISPATCH(struct SyncGpuBoost *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__nvoc_metadata_ptr->vtable.__syncgpuboostShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS syncgpuboostGetMemInterMapParams_DISPATCH(struct SyncGpuBoost *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__syncgpuboostGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS syncgpuboostCheckMemInterUnmap_DISPATCH(struct SyncGpuBoost *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__syncgpuboostCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS syncgpuboostGetMemoryMappingDescriptor_DISPATCH(struct SyncGpuBoost *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__syncgpuboostGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS syncgpuboostControlSerialization_Prologue_DISPATCH(struct SyncGpuBoost *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__syncgpuboostControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void syncgpuboostControlSerialization_Epilogue_DISPATCH(struct SyncGpuBoost *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__syncgpuboostControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS syncgpuboostControl_Prologue_DISPATCH(struct SyncGpuBoost *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__syncgpuboostControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void syncgpuboostControl_Epilogue_DISPATCH(struct SyncGpuBoost *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__syncgpuboostControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool syncgpuboostCanCopy_DISPATCH(struct SyncGpuBoost *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__syncgpuboostCanCopy__(pResource);
}

static inline NV_STATUS syncgpuboostIsDuplicate_DISPATCH(struct SyncGpuBoost *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_metadata_ptr->vtable.__syncgpuboostIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void syncgpuboostPreDestruct_DISPATCH(struct SyncGpuBoost *pResource) {
    pResource->__nvoc_metadata_ptr->vtable.__syncgpuboostPreDestruct__(pResource);
}

static inline NV_STATUS syncgpuboostControl_DISPATCH(struct SyncGpuBoost *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__syncgpuboostControl__(pResource, pCallContext, pParams);
}

static inline NV_STATUS syncgpuboostControlFilter_DISPATCH(struct SyncGpuBoost *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__syncgpuboostControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS syncgpuboostMap_DISPATCH(struct SyncGpuBoost *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pResource->__nvoc_metadata_ptr->vtable.__syncgpuboostMap__(pResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS syncgpuboostUnmap_DISPATCH(struct SyncGpuBoost *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pResource->__nvoc_metadata_ptr->vtable.__syncgpuboostUnmap__(pResource, pCallContext, pCpuMapping);
}

static inline NvBool syncgpuboostIsPartialUnmapSupported_DISPATCH(struct SyncGpuBoost *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__syncgpuboostIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS syncgpuboostMapTo_DISPATCH(struct SyncGpuBoost *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__syncgpuboostMapTo__(pResource, pParams);
}

static inline NV_STATUS syncgpuboostUnmapFrom_DISPATCH(struct SyncGpuBoost *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__syncgpuboostUnmapFrom__(pResource, pParams);
}

static inline NvU32 syncgpuboostGetRefCount_DISPATCH(struct SyncGpuBoost *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__syncgpuboostGetRefCount__(pResource);
}

static inline void syncgpuboostAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct SyncGpuBoost *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_metadata_ptr->vtable.__syncgpuboostAddAdditionalDependants__(pClient, pResource, pReference);
}

NV_STATUS syncgpuboostConstruct_IMPL(struct SyncGpuBoost *arg_pResource, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_syncgpuboostConstruct(arg_pResource, arg_pCallContext, arg_pParams) syncgpuboostConstruct_IMPL(arg_pResource, arg_pCallContext, arg_pParams)
void syncgpuboostDestruct_IMPL(struct SyncGpuBoost *pResource);

#define __nvoc_syncgpuboostDestruct(pResource) syncgpuboostDestruct_IMPL(pResource)
#undef PRIVATE_FIELD


#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_SYNCGPUBOOST_NVOC_H_
