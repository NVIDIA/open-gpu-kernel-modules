#ifndef _G_MPS_API_NVOC_H_
#define _G_MPS_API_NVOC_H_
#include "nvoc/runtime.h"

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
#ifdef NVOC_MPS_API_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct MpsApi {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct RmResource __nvoc_base_RmResource;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct MpsApi *__nvoc_pbase_MpsApi;
    NvBool (*__mpsApiShareCallback__)(struct MpsApi *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__mpsApiCheckMemInterUnmap__)(struct MpsApi *, NvBool);
    NV_STATUS (*__mpsApiControl__)(struct MpsApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__mpsApiGetMemInterMapParams__)(struct MpsApi *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__mpsApiGetMemoryMappingDescriptor__)(struct MpsApi *, struct MEMORY_DESCRIPTOR **);
    NvU32 (*__mpsApiGetRefCount__)(struct MpsApi *);
    NV_STATUS (*__mpsApiControlFilter__)(struct MpsApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    void (*__mpsApiAddAdditionalDependants__)(struct RsClient *, struct MpsApi *, RsResourceRef *);
    NV_STATUS (*__mpsApiUnmapFrom__)(struct MpsApi *, RS_RES_UNMAP_FROM_PARAMS *);
    NV_STATUS (*__mpsApiControlSerialization_Prologue__)(struct MpsApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__mpsApiControl_Prologue__)(struct MpsApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__mpsApiCanCopy__)(struct MpsApi *);
    NV_STATUS (*__mpsApiUnmap__)(struct MpsApi *, struct CALL_CONTEXT *, RsCpuMapping *);
    void (*__mpsApiPreDestruct__)(struct MpsApi *);
    NV_STATUS (*__mpsApiMapTo__)(struct MpsApi *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__mpsApiIsDuplicate__)(struct MpsApi *, NvHandle, NvBool *);
    void (*__mpsApiControlSerialization_Epilogue__)(struct MpsApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    void (*__mpsApiControl_Epilogue__)(struct MpsApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__mpsApiControlLookup__)(struct MpsApi *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NV_STATUS (*__mpsApiMap__)(struct MpsApi *, struct CALL_CONTEXT *, RS_CPU_MAP_PARAMS *, RsCpuMapping *);
    NvBool (*__mpsApiAccessCallback__)(struct MpsApi *, struct RsClient *, void *, RsAccessRight);
};

#ifndef __NVOC_CLASS_MpsApi_TYPEDEF__
#define __NVOC_CLASS_MpsApi_TYPEDEF__
typedef struct MpsApi MpsApi;
#endif /* __NVOC_CLASS_MpsApi_TYPEDEF__ */

#ifndef __nvoc_class_id_MpsApi
#define __nvoc_class_id_MpsApi 0x22ce42
#endif /* __nvoc_class_id_MpsApi */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_MpsApi;

#define __staticCast_MpsApi(pThis) \
    ((pThis)->__nvoc_pbase_MpsApi)

#ifdef __nvoc_mps_api_h_disabled
#define __dynamicCast_MpsApi(pThis) ((MpsApi*)NULL)
#else //__nvoc_mps_api_h_disabled
#define __dynamicCast_MpsApi(pThis) \
    ((MpsApi*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(MpsApi)))
#endif //__nvoc_mps_api_h_disabled


NV_STATUS __nvoc_objCreateDynamic_MpsApi(MpsApi**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_MpsApi(MpsApi**, Dynamic*, NvU32, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_MpsApi(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_MpsApi((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define mpsApiShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) mpsApiShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define mpsApiCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) mpsApiCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define mpsApiControl(pResource, pCallContext, pParams) mpsApiControl_DISPATCH(pResource, pCallContext, pParams)
#define mpsApiGetMemInterMapParams(pRmResource, pParams) mpsApiGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define mpsApiGetMemoryMappingDescriptor(pRmResource, ppMemDesc) mpsApiGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define mpsApiGetRefCount(pResource) mpsApiGetRefCount_DISPATCH(pResource)
#define mpsApiControlFilter(pResource, pCallContext, pParams) mpsApiControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define mpsApiAddAdditionalDependants(pClient, pResource, pReference) mpsApiAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define mpsApiUnmapFrom(pResource, pParams) mpsApiUnmapFrom_DISPATCH(pResource, pParams)
#define mpsApiControlSerialization_Prologue(pResource, pCallContext, pParams) mpsApiControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define mpsApiControl_Prologue(pResource, pCallContext, pParams) mpsApiControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define mpsApiCanCopy(pResource) mpsApiCanCopy_DISPATCH(pResource)
#define mpsApiUnmap(pResource, pCallContext, pCpuMapping) mpsApiUnmap_DISPATCH(pResource, pCallContext, pCpuMapping)
#define mpsApiPreDestruct(pResource) mpsApiPreDestruct_DISPATCH(pResource)
#define mpsApiMapTo(pResource, pParams) mpsApiMapTo_DISPATCH(pResource, pParams)
#define mpsApiIsDuplicate(pResource, hMemory, pDuplicate) mpsApiIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define mpsApiControlSerialization_Epilogue(pResource, pCallContext, pParams) mpsApiControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define mpsApiControl_Epilogue(pResource, pCallContext, pParams) mpsApiControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define mpsApiControlLookup(pResource, pParams, ppEntry) mpsApiControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define mpsApiMap(pResource, pCallContext, pParams, pCpuMapping) mpsApiMap_DISPATCH(pResource, pCallContext, pParams, pCpuMapping)
#define mpsApiAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) mpsApiAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
static inline NvBool mpsApiShareCallback_DISPATCH(struct MpsApi *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__mpsApiShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS mpsApiCheckMemInterUnmap_DISPATCH(struct MpsApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__mpsApiCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS mpsApiControl_DISPATCH(struct MpsApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__mpsApiControl__(pResource, pCallContext, pParams);
}

static inline NV_STATUS mpsApiGetMemInterMapParams_DISPATCH(struct MpsApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__mpsApiGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS mpsApiGetMemoryMappingDescriptor_DISPATCH(struct MpsApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__mpsApiGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NvU32 mpsApiGetRefCount_DISPATCH(struct MpsApi *pResource) {
    return pResource->__mpsApiGetRefCount__(pResource);
}

static inline NV_STATUS mpsApiControlFilter_DISPATCH(struct MpsApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__mpsApiControlFilter__(pResource, pCallContext, pParams);
}

static inline void mpsApiAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct MpsApi *pResource, RsResourceRef *pReference) {
    pResource->__mpsApiAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS mpsApiUnmapFrom_DISPATCH(struct MpsApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__mpsApiUnmapFrom__(pResource, pParams);
}

static inline NV_STATUS mpsApiControlSerialization_Prologue_DISPATCH(struct MpsApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__mpsApiControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS mpsApiControl_Prologue_DISPATCH(struct MpsApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__mpsApiControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool mpsApiCanCopy_DISPATCH(struct MpsApi *pResource) {
    return pResource->__mpsApiCanCopy__(pResource);
}

static inline NV_STATUS mpsApiUnmap_DISPATCH(struct MpsApi *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pResource->__mpsApiUnmap__(pResource, pCallContext, pCpuMapping);
}

static inline void mpsApiPreDestruct_DISPATCH(struct MpsApi *pResource) {
    pResource->__mpsApiPreDestruct__(pResource);
}

static inline NV_STATUS mpsApiMapTo_DISPATCH(struct MpsApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__mpsApiMapTo__(pResource, pParams);
}

static inline NV_STATUS mpsApiIsDuplicate_DISPATCH(struct MpsApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__mpsApiIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void mpsApiControlSerialization_Epilogue_DISPATCH(struct MpsApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__mpsApiControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline void mpsApiControl_Epilogue_DISPATCH(struct MpsApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__mpsApiControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS mpsApiControlLookup_DISPATCH(struct MpsApi *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__mpsApiControlLookup__(pResource, pParams, ppEntry);
}

static inline NV_STATUS mpsApiMap_DISPATCH(struct MpsApi *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pResource->__mpsApiMap__(pResource, pCallContext, pParams, pCpuMapping);
}

static inline NvBool mpsApiAccessCallback_DISPATCH(struct MpsApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__mpsApiAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
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
