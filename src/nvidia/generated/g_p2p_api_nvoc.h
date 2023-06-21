#ifndef _G_P2P_API_NVOC_H_
#define _G_P2P_API_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2009-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_p2p_api_nvoc.h"


#ifndef _P2P_API_H_
#define _P2P_API_H_

#include "core/core.h"
#include "rmapi/client.h"

//
// Definitions for P2PApi.attributes.
// Need to keep NV_P2PAPI_ATTRIBUTES_CONNECTION_TYPE sync with P2P_CONNECTIVITY in p2p_caps.h
//
#define NV_P2PAPI_ATTRIBUTES_CONNECTION_TYPE                     3:0
#define NV_P2PAPI_ATTRIBUTES_CONNECTION_TYPE_UNKNOWN             0x0
#define NV_P2PAPI_ATTRIBUTES_CONNECTION_TYPE_PCIE                0x1
#define NV_P2PAPI_ATTRIBUTES_CONNECTION_TYPE_PCIE_BAR1           0x2
#define NV_P2PAPI_ATTRIBUTES_CONNECTION_TYPE_NVLINK              0x3
#define NV_P2PAPI_ATTRIBUTES_CONNECTION_TYPE_NVLINK_INDIRECT     0x4
#define NV_P2PAPI_ATTRIBUTES_CONNECTION_TYPE_C2C                 0x5
#define NV_P2PAPI_ATTRIBUTES_LINK_TYPE                           4:4
#define NV_P2PAPI_ATTRIBUTES_LINK_TYPE_GPA                       0x0
#define NV_P2PAPI_ATTRIBUTES_LINK_TYPE_SPA                       0x1
#define NV_P2PAPI_ATTRIBUTES_REMOTE_EGM                          5:5
#define NV_P2PAPI_ATTRIBUTES_REMOTE_EGM_NO                       0x0
#define NV_P2PAPI_ATTRIBUTES_REMOTE_EGM_YES                      0x1

#ifdef NVOC_P2P_API_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct P2PApi {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct RmResource __nvoc_base_RmResource;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct P2PApi *__nvoc_pbase_P2PApi;
    NvBool (*__p2papiShareCallback__)(struct P2PApi *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__p2papiCheckMemInterUnmap__)(struct P2PApi *, NvBool);
    NV_STATUS (*__p2papiControl__)(struct P2PApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__p2papiGetMemInterMapParams__)(struct P2PApi *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__p2papiGetMemoryMappingDescriptor__)(struct P2PApi *, struct MEMORY_DESCRIPTOR **);
    NvU32 (*__p2papiGetRefCount__)(struct P2PApi *);
    NV_STATUS (*__p2papiControlFilter__)(struct P2PApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    void (*__p2papiAddAdditionalDependants__)(struct RsClient *, struct P2PApi *, RsResourceRef *);
    NV_STATUS (*__p2papiUnmapFrom__)(struct P2PApi *, RS_RES_UNMAP_FROM_PARAMS *);
    NV_STATUS (*__p2papiControlSerialization_Prologue__)(struct P2PApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__p2papiControl_Prologue__)(struct P2PApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__p2papiCanCopy__)(struct P2PApi *);
    NV_STATUS (*__p2papiUnmap__)(struct P2PApi *, struct CALL_CONTEXT *, RsCpuMapping *);
    void (*__p2papiPreDestruct__)(struct P2PApi *);
    NV_STATUS (*__p2papiMapTo__)(struct P2PApi *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__p2papiIsDuplicate__)(struct P2PApi *, NvHandle, NvBool *);
    void (*__p2papiControlSerialization_Epilogue__)(struct P2PApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    void (*__p2papiControl_Epilogue__)(struct P2PApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__p2papiControlLookup__)(struct P2PApi *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NV_STATUS (*__p2papiMap__)(struct P2PApi *, struct CALL_CONTEXT *, RS_CPU_MAP_PARAMS *, RsCpuMapping *);
    NvBool (*__p2papiAccessCallback__)(struct P2PApi *, struct RsClient *, void *, RsAccessRight);
    struct OBJGPU *peer1;
    struct OBJGPU *peer2;
    NvU32 localGfid;
    NvU32 remoteGfid;
    NvU32 peerId1;
    NvU32 peerId2;
    NvU32 egmPeerId1;
    NvU32 egmPeerId2;
    NvU32 attributes;
};

#ifndef __NVOC_CLASS_P2PApi_TYPEDEF__
#define __NVOC_CLASS_P2PApi_TYPEDEF__
typedef struct P2PApi P2PApi;
#endif /* __NVOC_CLASS_P2PApi_TYPEDEF__ */

#ifndef __nvoc_class_id_P2PApi
#define __nvoc_class_id_P2PApi 0x3982b7
#endif /* __nvoc_class_id_P2PApi */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_P2PApi;

#define __staticCast_P2PApi(pThis) \
    ((pThis)->__nvoc_pbase_P2PApi)

#ifdef __nvoc_p2p_api_h_disabled
#define __dynamicCast_P2PApi(pThis) ((P2PApi*)NULL)
#else //__nvoc_p2p_api_h_disabled
#define __dynamicCast_P2PApi(pThis) \
    ((P2PApi*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(P2PApi)))
#endif //__nvoc_p2p_api_h_disabled


NV_STATUS __nvoc_objCreateDynamic_P2PApi(P2PApi**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_P2PApi(P2PApi**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_P2PApi(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_P2PApi((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define p2papiShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) p2papiShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define p2papiCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) p2papiCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define p2papiControl(pResource, pCallContext, pParams) p2papiControl_DISPATCH(pResource, pCallContext, pParams)
#define p2papiGetMemInterMapParams(pRmResource, pParams) p2papiGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define p2papiGetMemoryMappingDescriptor(pRmResource, ppMemDesc) p2papiGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define p2papiGetRefCount(pResource) p2papiGetRefCount_DISPATCH(pResource)
#define p2papiControlFilter(pResource, pCallContext, pParams) p2papiControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define p2papiAddAdditionalDependants(pClient, pResource, pReference) p2papiAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define p2papiUnmapFrom(pResource, pParams) p2papiUnmapFrom_DISPATCH(pResource, pParams)
#define p2papiControlSerialization_Prologue(pResource, pCallContext, pParams) p2papiControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define p2papiControl_Prologue(pResource, pCallContext, pParams) p2papiControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define p2papiCanCopy(pResource) p2papiCanCopy_DISPATCH(pResource)
#define p2papiUnmap(pResource, pCallContext, pCpuMapping) p2papiUnmap_DISPATCH(pResource, pCallContext, pCpuMapping)
#define p2papiPreDestruct(pResource) p2papiPreDestruct_DISPATCH(pResource)
#define p2papiMapTo(pResource, pParams) p2papiMapTo_DISPATCH(pResource, pParams)
#define p2papiIsDuplicate(pResource, hMemory, pDuplicate) p2papiIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define p2papiControlSerialization_Epilogue(pResource, pCallContext, pParams) p2papiControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define p2papiControl_Epilogue(pResource, pCallContext, pParams) p2papiControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define p2papiControlLookup(pResource, pParams, ppEntry) p2papiControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define p2papiMap(pResource, pCallContext, pParams, pCpuMapping) p2papiMap_DISPATCH(pResource, pCallContext, pParams, pCpuMapping)
#define p2papiAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) p2papiAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
static inline NvBool p2papiShareCallback_DISPATCH(struct P2PApi *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__p2papiShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS p2papiCheckMemInterUnmap_DISPATCH(struct P2PApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__p2papiCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS p2papiControl_DISPATCH(struct P2PApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__p2papiControl__(pResource, pCallContext, pParams);
}

static inline NV_STATUS p2papiGetMemInterMapParams_DISPATCH(struct P2PApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__p2papiGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS p2papiGetMemoryMappingDescriptor_DISPATCH(struct P2PApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__p2papiGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NvU32 p2papiGetRefCount_DISPATCH(struct P2PApi *pResource) {
    return pResource->__p2papiGetRefCount__(pResource);
}

static inline NV_STATUS p2papiControlFilter_DISPATCH(struct P2PApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__p2papiControlFilter__(pResource, pCallContext, pParams);
}

static inline void p2papiAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct P2PApi *pResource, RsResourceRef *pReference) {
    pResource->__p2papiAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS p2papiUnmapFrom_DISPATCH(struct P2PApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__p2papiUnmapFrom__(pResource, pParams);
}

static inline NV_STATUS p2papiControlSerialization_Prologue_DISPATCH(struct P2PApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__p2papiControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS p2papiControl_Prologue_DISPATCH(struct P2PApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__p2papiControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool p2papiCanCopy_DISPATCH(struct P2PApi *pResource) {
    return pResource->__p2papiCanCopy__(pResource);
}

static inline NV_STATUS p2papiUnmap_DISPATCH(struct P2PApi *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pResource->__p2papiUnmap__(pResource, pCallContext, pCpuMapping);
}

static inline void p2papiPreDestruct_DISPATCH(struct P2PApi *pResource) {
    pResource->__p2papiPreDestruct__(pResource);
}

static inline NV_STATUS p2papiMapTo_DISPATCH(struct P2PApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__p2papiMapTo__(pResource, pParams);
}

static inline NV_STATUS p2papiIsDuplicate_DISPATCH(struct P2PApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__p2papiIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void p2papiControlSerialization_Epilogue_DISPATCH(struct P2PApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__p2papiControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline void p2papiControl_Epilogue_DISPATCH(struct P2PApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__p2papiControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS p2papiControlLookup_DISPATCH(struct P2PApi *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__p2papiControlLookup__(pResource, pParams, ppEntry);
}

static inline NV_STATUS p2papiMap_DISPATCH(struct P2PApi *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pResource->__p2papiMap__(pResource, pCallContext, pParams, pCpuMapping);
}

static inline NvBool p2papiAccessCallback_DISPATCH(struct P2PApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__p2papiAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

NV_STATUS p2papiConstruct_IMPL(struct P2PApi *arg_pResource, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_p2papiConstruct(arg_pResource, arg_pCallContext, arg_pParams) p2papiConstruct_IMPL(arg_pResource, arg_pCallContext, arg_pParams)
void p2papiDestruct_IMPL(struct P2PApi *pResource);

#define __nvoc_p2papiDestruct(pResource) p2papiDestruct_IMPL(pResource)
#undef PRIVATE_FIELD


#endif // _P2P_API_H_

#ifdef __cplusplus
} // extern "C"
#endif
#endif // _G_P2P_API_NVOC_H_
