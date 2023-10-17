#ifndef _G_SYNCGPUBOOST_NVOC_H_
#define _G_SYNCGPUBOOST_NVOC_H_
#include "nvoc/runtime.h"

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

#include "g_syncgpuboost_nvoc.h"

#ifndef _SYNCGPUBOOST_H_
#define _SYNCGPUBOOST_H_

#include "resserv/resserv.h"
#include "nvoc/prelude.h"
#include "rmapi/resource.h"
#include "class/cl0060.h"

#ifdef NVOC_SYNCGPUBOOST_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct SyncGpuBoost {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct RmResource __nvoc_base_RmResource;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct SyncGpuBoost *__nvoc_pbase_SyncGpuBoost;
    NvBool (*__syncgpuboostShareCallback__)(struct SyncGpuBoost *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__syncgpuboostCheckMemInterUnmap__)(struct SyncGpuBoost *, NvBool);
    NV_STATUS (*__syncgpuboostControl__)(struct SyncGpuBoost *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__syncgpuboostGetMemInterMapParams__)(struct SyncGpuBoost *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__syncgpuboostGetMemoryMappingDescriptor__)(struct SyncGpuBoost *, struct MEMORY_DESCRIPTOR **);
    NvU32 (*__syncgpuboostGetRefCount__)(struct SyncGpuBoost *);
    NV_STATUS (*__syncgpuboostControlFilter__)(struct SyncGpuBoost *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    void (*__syncgpuboostAddAdditionalDependants__)(struct RsClient *, struct SyncGpuBoost *, RsResourceRef *);
    NV_STATUS (*__syncgpuboostUnmapFrom__)(struct SyncGpuBoost *, RS_RES_UNMAP_FROM_PARAMS *);
    NV_STATUS (*__syncgpuboostControlSerialization_Prologue__)(struct SyncGpuBoost *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__syncgpuboostControl_Prologue__)(struct SyncGpuBoost *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__syncgpuboostCanCopy__)(struct SyncGpuBoost *);
    NV_STATUS (*__syncgpuboostUnmap__)(struct SyncGpuBoost *, struct CALL_CONTEXT *, RsCpuMapping *);
    void (*__syncgpuboostPreDestruct__)(struct SyncGpuBoost *);
    NV_STATUS (*__syncgpuboostMapTo__)(struct SyncGpuBoost *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__syncgpuboostIsDuplicate__)(struct SyncGpuBoost *, NvHandle, NvBool *);
    void (*__syncgpuboostControlSerialization_Epilogue__)(struct SyncGpuBoost *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    void (*__syncgpuboostControl_Epilogue__)(struct SyncGpuBoost *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__syncgpuboostControlLookup__)(struct SyncGpuBoost *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NV_STATUS (*__syncgpuboostMap__)(struct SyncGpuBoost *, struct CALL_CONTEXT *, RS_CPU_MAP_PARAMS *, RsCpuMapping *);
    NvBool (*__syncgpuboostAccessCallback__)(struct SyncGpuBoost *, struct RsClient *, void *, RsAccessRight);
    NvU32 gpuBoostGroupId;
};

#ifndef __NVOC_CLASS_SyncGpuBoost_TYPEDEF__
#define __NVOC_CLASS_SyncGpuBoost_TYPEDEF__
typedef struct SyncGpuBoost SyncGpuBoost;
#endif /* __NVOC_CLASS_SyncGpuBoost_TYPEDEF__ */

#ifndef __nvoc_class_id_SyncGpuBoost
#define __nvoc_class_id_SyncGpuBoost 0xc7e30b
#endif /* __nvoc_class_id_SyncGpuBoost */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_SyncGpuBoost;

#define __staticCast_SyncGpuBoost(pThis) \
    ((pThis)->__nvoc_pbase_SyncGpuBoost)

#ifdef __nvoc_syncgpuboost_h_disabled
#define __dynamicCast_SyncGpuBoost(pThis) ((SyncGpuBoost*)NULL)
#else //__nvoc_syncgpuboost_h_disabled
#define __dynamicCast_SyncGpuBoost(pThis) \
    ((SyncGpuBoost*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(SyncGpuBoost)))
#endif //__nvoc_syncgpuboost_h_disabled


NV_STATUS __nvoc_objCreateDynamic_SyncGpuBoost(SyncGpuBoost**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_SyncGpuBoost(SyncGpuBoost**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_SyncGpuBoost(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_SyncGpuBoost((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define syncgpuboostShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) syncgpuboostShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define syncgpuboostCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) syncgpuboostCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define syncgpuboostControl(pResource, pCallContext, pParams) syncgpuboostControl_DISPATCH(pResource, pCallContext, pParams)
#define syncgpuboostGetMemInterMapParams(pRmResource, pParams) syncgpuboostGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define syncgpuboostGetMemoryMappingDescriptor(pRmResource, ppMemDesc) syncgpuboostGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define syncgpuboostGetRefCount(pResource) syncgpuboostGetRefCount_DISPATCH(pResource)
#define syncgpuboostControlFilter(pResource, pCallContext, pParams) syncgpuboostControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define syncgpuboostAddAdditionalDependants(pClient, pResource, pReference) syncgpuboostAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define syncgpuboostUnmapFrom(pResource, pParams) syncgpuboostUnmapFrom_DISPATCH(pResource, pParams)
#define syncgpuboostControlSerialization_Prologue(pResource, pCallContext, pParams) syncgpuboostControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define syncgpuboostControl_Prologue(pResource, pCallContext, pParams) syncgpuboostControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define syncgpuboostCanCopy(pResource) syncgpuboostCanCopy_DISPATCH(pResource)
#define syncgpuboostUnmap(pResource, pCallContext, pCpuMapping) syncgpuboostUnmap_DISPATCH(pResource, pCallContext, pCpuMapping)
#define syncgpuboostPreDestruct(pResource) syncgpuboostPreDestruct_DISPATCH(pResource)
#define syncgpuboostMapTo(pResource, pParams) syncgpuboostMapTo_DISPATCH(pResource, pParams)
#define syncgpuboostIsDuplicate(pResource, hMemory, pDuplicate) syncgpuboostIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define syncgpuboostControlSerialization_Epilogue(pResource, pCallContext, pParams) syncgpuboostControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define syncgpuboostControl_Epilogue(pResource, pCallContext, pParams) syncgpuboostControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define syncgpuboostControlLookup(pResource, pParams, ppEntry) syncgpuboostControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define syncgpuboostMap(pResource, pCallContext, pParams, pCpuMapping) syncgpuboostMap_DISPATCH(pResource, pCallContext, pParams, pCpuMapping)
#define syncgpuboostAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) syncgpuboostAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
static inline NvBool syncgpuboostShareCallback_DISPATCH(struct SyncGpuBoost *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__syncgpuboostShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS syncgpuboostCheckMemInterUnmap_DISPATCH(struct SyncGpuBoost *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__syncgpuboostCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS syncgpuboostControl_DISPATCH(struct SyncGpuBoost *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__syncgpuboostControl__(pResource, pCallContext, pParams);
}

static inline NV_STATUS syncgpuboostGetMemInterMapParams_DISPATCH(struct SyncGpuBoost *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__syncgpuboostGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS syncgpuboostGetMemoryMappingDescriptor_DISPATCH(struct SyncGpuBoost *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__syncgpuboostGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NvU32 syncgpuboostGetRefCount_DISPATCH(struct SyncGpuBoost *pResource) {
    return pResource->__syncgpuboostGetRefCount__(pResource);
}

static inline NV_STATUS syncgpuboostControlFilter_DISPATCH(struct SyncGpuBoost *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__syncgpuboostControlFilter__(pResource, pCallContext, pParams);
}

static inline void syncgpuboostAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct SyncGpuBoost *pResource, RsResourceRef *pReference) {
    pResource->__syncgpuboostAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS syncgpuboostUnmapFrom_DISPATCH(struct SyncGpuBoost *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__syncgpuboostUnmapFrom__(pResource, pParams);
}

static inline NV_STATUS syncgpuboostControlSerialization_Prologue_DISPATCH(struct SyncGpuBoost *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__syncgpuboostControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS syncgpuboostControl_Prologue_DISPATCH(struct SyncGpuBoost *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__syncgpuboostControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool syncgpuboostCanCopy_DISPATCH(struct SyncGpuBoost *pResource) {
    return pResource->__syncgpuboostCanCopy__(pResource);
}

static inline NV_STATUS syncgpuboostUnmap_DISPATCH(struct SyncGpuBoost *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pResource->__syncgpuboostUnmap__(pResource, pCallContext, pCpuMapping);
}

static inline void syncgpuboostPreDestruct_DISPATCH(struct SyncGpuBoost *pResource) {
    pResource->__syncgpuboostPreDestruct__(pResource);
}

static inline NV_STATUS syncgpuboostMapTo_DISPATCH(struct SyncGpuBoost *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__syncgpuboostMapTo__(pResource, pParams);
}

static inline NV_STATUS syncgpuboostIsDuplicate_DISPATCH(struct SyncGpuBoost *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__syncgpuboostIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void syncgpuboostControlSerialization_Epilogue_DISPATCH(struct SyncGpuBoost *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__syncgpuboostControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline void syncgpuboostControl_Epilogue_DISPATCH(struct SyncGpuBoost *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__syncgpuboostControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS syncgpuboostControlLookup_DISPATCH(struct SyncGpuBoost *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__syncgpuboostControlLookup__(pResource, pParams, ppEntry);
}

static inline NV_STATUS syncgpuboostMap_DISPATCH(struct SyncGpuBoost *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pResource->__syncgpuboostMap__(pResource, pCallContext, pParams, pCpuMapping);
}

static inline NvBool syncgpuboostAccessCallback_DISPATCH(struct SyncGpuBoost *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__syncgpuboostAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
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
