
#ifndef _G_MPS_API_NVOC_H_
#define _G_MPS_API_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2017 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 *   File:  mpsApi.h
 *
 *   Description:
 *       This file contains the functions managing the MpsApi object
 *
 *****************************************************************************/

#pragma once
#include "g_mps_api_nvoc.h"

#ifndef _MPSAPI_H_
#define _MPSAPI_H_

#include "gpu/gpu.h"
#include "rmapi/resource.h"


// ****************************************************************************
//                          Type Definitions
// ****************************************************************************

//
// MpsApi information
//
// A client which has allocated MpsApi object is identified as MPS process.
//

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_MPS_API_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__MpsApi;
struct NVOC_METADATA__RmResource;
struct NVOC_VTABLE__MpsApi;


struct MpsApi {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__MpsApi *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct RmResource __nvoc_base_RmResource;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^3
    struct RsResource *__nvoc_pbase_RsResource;    // res super^2
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^2
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super
    struct MpsApi *__nvoc_pbase_MpsApi;    // mpsApi
};


// Vtable with 21 per-class function pointers
struct NVOC_VTABLE__MpsApi {
    NvBool (*__mpsApiAccessCallback__)(struct MpsApi * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (rmres)
    NvBool (*__mpsApiShareCallback__)(struct MpsApi * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__mpsApiGetMemInterMapParams__)(struct MpsApi * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__mpsApiCheckMemInterUnmap__)(struct MpsApi * /*this*/, NvBool);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__mpsApiGetMemoryMappingDescriptor__)(struct MpsApi * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__mpsApiControlSerialization_Prologue__)(struct MpsApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (rmres)
    void (*__mpsApiControlSerialization_Epilogue__)(struct MpsApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__mpsApiControl_Prologue__)(struct MpsApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (rmres)
    void (*__mpsApiControl_Epilogue__)(struct MpsApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (rmres)
    NvBool (*__mpsApiCanCopy__)(struct MpsApi * /*this*/);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__mpsApiIsDuplicate__)(struct MpsApi * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (rmres)
    void (*__mpsApiPreDestruct__)(struct MpsApi * /*this*/);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__mpsApiControl__)(struct MpsApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__mpsApiControlFilter__)(struct MpsApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__mpsApiMap__)(struct MpsApi * /*this*/, struct CALL_CONTEXT *, RS_CPU_MAP_PARAMS *, RsCpuMapping *);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__mpsApiUnmap__)(struct MpsApi * /*this*/, struct CALL_CONTEXT *, RsCpuMapping *);  // virtual inherited (res) base (rmres)
    NvBool (*__mpsApiIsPartialUnmapSupported__)(struct MpsApi * /*this*/);  // inline virtual inherited (res) base (rmres) body
    NV_STATUS (*__mpsApiMapTo__)(struct MpsApi * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__mpsApiUnmapFrom__)(struct MpsApi * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (rmres)
    NvU32 (*__mpsApiGetRefCount__)(struct MpsApi * /*this*/);  // virtual inherited (res) base (rmres)
    void (*__mpsApiAddAdditionalDependants__)(struct RsClient *, struct MpsApi * /*this*/, RsResourceRef *);  // virtual inherited (res) base (rmres)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__MpsApi {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__RmResource metadata__RmResource;
    const struct NVOC_VTABLE__MpsApi vtable;
};

#ifndef __NVOC_CLASS_MpsApi_TYPEDEF__
#define __NVOC_CLASS_MpsApi_TYPEDEF__
typedef struct MpsApi MpsApi;
#endif /* __NVOC_CLASS_MpsApi_TYPEDEF__ */

#ifndef __nvoc_class_id_MpsApi
#define __nvoc_class_id_MpsApi 0x22ce42
#endif /* __nvoc_class_id_MpsApi */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_MpsApi;

#define __staticCast_MpsApi(pThis) \
    ((pThis)->__nvoc_pbase_MpsApi)

#ifdef __nvoc_mps_api_h_disabled
#define __dynamicCast_MpsApi(pThis) ((MpsApi*) NULL)
#else //__nvoc_mps_api_h_disabled
#define __dynamicCast_MpsApi(pThis) \
    ((MpsApi*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(MpsApi)))
#endif //__nvoc_mps_api_h_disabled

NV_STATUS __nvoc_objCreateDynamic_MpsApi(MpsApi**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_MpsApi(MpsApi**, Dynamic*, NvU32, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __objCreate_MpsApi(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_MpsApi((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define mpsApiAccessCallback_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresAccessCallback__
#define mpsApiAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) mpsApiAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define mpsApiShareCallback_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresShareCallback__
#define mpsApiShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) mpsApiShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define mpsApiGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define mpsApiGetMemInterMapParams(pRmResource, pParams) mpsApiGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define mpsApiCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define mpsApiCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) mpsApiCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define mpsApiGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define mpsApiGetMemoryMappingDescriptor(pRmResource, ppMemDesc) mpsApiGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define mpsApiControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Prologue__
#define mpsApiControlSerialization_Prologue(pResource, pCallContext, pParams) mpsApiControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define mpsApiControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Epilogue__
#define mpsApiControlSerialization_Epilogue(pResource, pCallContext, pParams) mpsApiControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define mpsApiControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Prologue__
#define mpsApiControl_Prologue(pResource, pCallContext, pParams) mpsApiControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define mpsApiControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Epilogue__
#define mpsApiControl_Epilogue(pResource, pCallContext, pParams) mpsApiControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define mpsApiCanCopy_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define mpsApiCanCopy(pResource) mpsApiCanCopy_DISPATCH(pResource)
#define mpsApiIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define mpsApiIsDuplicate(pResource, hMemory, pDuplicate) mpsApiIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define mpsApiPreDestruct_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define mpsApiPreDestruct(pResource) mpsApiPreDestruct_DISPATCH(pResource)
#define mpsApiControl_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__
#define mpsApiControl(pResource, pCallContext, pParams) mpsApiControl_DISPATCH(pResource, pCallContext, pParams)
#define mpsApiControlFilter_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define mpsApiControlFilter(pResource, pCallContext, pParams) mpsApiControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define mpsApiMap_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__
#define mpsApiMap(pResource, pCallContext, pParams, pCpuMapping) mpsApiMap_DISPATCH(pResource, pCallContext, pParams, pCpuMapping)
#define mpsApiUnmap_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__
#define mpsApiUnmap(pResource, pCallContext, pCpuMapping) mpsApiUnmap_DISPATCH(pResource, pCallContext, pCpuMapping)
#define mpsApiIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define mpsApiIsPartialUnmapSupported(pResource) mpsApiIsPartialUnmapSupported_DISPATCH(pResource)
#define mpsApiMapTo_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define mpsApiMapTo(pResource, pParams) mpsApiMapTo_DISPATCH(pResource, pParams)
#define mpsApiUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define mpsApiUnmapFrom(pResource, pParams) mpsApiUnmapFrom_DISPATCH(pResource, pParams)
#define mpsApiGetRefCount_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define mpsApiGetRefCount(pResource) mpsApiGetRefCount_DISPATCH(pResource)
#define mpsApiAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define mpsApiAddAdditionalDependants(pClient, pResource, pReference) mpsApiAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NvBool mpsApiAccessCallback_DISPATCH(struct MpsApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_metadata_ptr->vtable.__mpsApiAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool mpsApiShareCallback_DISPATCH(struct MpsApi *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__nvoc_metadata_ptr->vtable.__mpsApiShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS mpsApiGetMemInterMapParams_DISPATCH(struct MpsApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__mpsApiGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS mpsApiCheckMemInterUnmap_DISPATCH(struct MpsApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__mpsApiCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS mpsApiGetMemoryMappingDescriptor_DISPATCH(struct MpsApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__mpsApiGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS mpsApiControlSerialization_Prologue_DISPATCH(struct MpsApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__mpsApiControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void mpsApiControlSerialization_Epilogue_DISPATCH(struct MpsApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__mpsApiControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS mpsApiControl_Prologue_DISPATCH(struct MpsApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__mpsApiControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void mpsApiControl_Epilogue_DISPATCH(struct MpsApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__mpsApiControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool mpsApiCanCopy_DISPATCH(struct MpsApi *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__mpsApiCanCopy__(pResource);
}

static inline NV_STATUS mpsApiIsDuplicate_DISPATCH(struct MpsApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_metadata_ptr->vtable.__mpsApiIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void mpsApiPreDestruct_DISPATCH(struct MpsApi *pResource) {
    pResource->__nvoc_metadata_ptr->vtable.__mpsApiPreDestruct__(pResource);
}

static inline NV_STATUS mpsApiControl_DISPATCH(struct MpsApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__mpsApiControl__(pResource, pCallContext, pParams);
}

static inline NV_STATUS mpsApiControlFilter_DISPATCH(struct MpsApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__mpsApiControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS mpsApiMap_DISPATCH(struct MpsApi *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pResource->__nvoc_metadata_ptr->vtable.__mpsApiMap__(pResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS mpsApiUnmap_DISPATCH(struct MpsApi *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pResource->__nvoc_metadata_ptr->vtable.__mpsApiUnmap__(pResource, pCallContext, pCpuMapping);
}

static inline NvBool mpsApiIsPartialUnmapSupported_DISPATCH(struct MpsApi *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__mpsApiIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS mpsApiMapTo_DISPATCH(struct MpsApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__mpsApiMapTo__(pResource, pParams);
}

static inline NV_STATUS mpsApiUnmapFrom_DISPATCH(struct MpsApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__mpsApiUnmapFrom__(pResource, pParams);
}

static inline NvU32 mpsApiGetRefCount_DISPATCH(struct MpsApi *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__mpsApiGetRefCount__(pResource);
}

static inline void mpsApiAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct MpsApi *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_metadata_ptr->vtable.__mpsApiAddAdditionalDependants__(pClient, pResource, pReference);
}

NV_STATUS mpsApiConstruct_IMPL(struct MpsApi *arg_pMpsApi, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_mpsApiConstruct(arg_pMpsApi, arg_pCallContext, arg_pParams) mpsApiConstruct_IMPL(arg_pMpsApi, arg_pCallContext, arg_pParams)
void mpsApiDestruct_IMPL(struct MpsApi *pMpsApi);

#define __nvoc_mpsApiDestruct(pMpsApi) mpsApiDestruct_IMPL(pMpsApi)
#undef PRIVATE_FIELD


#endif // _MPSAPI_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_MPS_API_NVOC_H_
