#ifndef _G_GPU_MGMT_API_NVOC_H_
#define _G_GPU_MGMT_API_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_gpu_mgmt_api_nvoc.h"

#ifndef GPU_MGMT_API_H
#define GPU_MGMT_API_H

#include "rmapi/resource.h"
#include "ctrl/ctrl0020.h"

// ****************************************************************************
//                          Type Definitions
// ****************************************************************************

//
// GpuManagementApi class information
//
// This is a global GPU class will help us to route IOCTLs to probed
// and persistent GPU state
//

#ifdef NVOC_GPU_MGMT_API_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct GpuManagementApi {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct RmResource __nvoc_base_RmResource;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct GpuManagementApi *__nvoc_pbase_GpuManagementApi;
    NV_STATUS (*__gpumgmtapiCtrlCmdSetShutdownState__)(struct GpuManagementApi *, NV0020_CTRL_GPU_MGMT_SET_SHUTDOWN_STATE_PARAMS *);
    NvBool (*__gpumgmtapiShareCallback__)(struct GpuManagementApi *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__gpumgmtapiCheckMemInterUnmap__)(struct GpuManagementApi *, NvBool);
    NV_STATUS (*__gpumgmtapiControl__)(struct GpuManagementApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__gpumgmtapiGetMemInterMapParams__)(struct GpuManagementApi *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__gpumgmtapiGetMemoryMappingDescriptor__)(struct GpuManagementApi *, struct MEMORY_DESCRIPTOR **);
    NvU32 (*__gpumgmtapiGetRefCount__)(struct GpuManagementApi *);
    NV_STATUS (*__gpumgmtapiControlFilter__)(struct GpuManagementApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    void (*__gpumgmtapiAddAdditionalDependants__)(struct RsClient *, struct GpuManagementApi *, RsResourceRef *);
    NV_STATUS (*__gpumgmtapiUnmap__)(struct GpuManagementApi *, struct CALL_CONTEXT *, RsCpuMapping *);
    NV_STATUS (*__gpumgmtapiControl_Prologue__)(struct GpuManagementApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__gpumgmtapiCanCopy__)(struct GpuManagementApi *);
    NV_STATUS (*__gpumgmtapiMapTo__)(struct GpuManagementApi *, RS_RES_MAP_TO_PARAMS *);
    void (*__gpumgmtapiPreDestruct__)(struct GpuManagementApi *);
    NV_STATUS (*__gpumgmtapiUnmapFrom__)(struct GpuManagementApi *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__gpumgmtapiControl_Epilogue__)(struct GpuManagementApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__gpumgmtapiControlLookup__)(struct GpuManagementApi *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NV_STATUS (*__gpumgmtapiMap__)(struct GpuManagementApi *, struct CALL_CONTEXT *, RS_CPU_MAP_PARAMS *, RsCpuMapping *);
    NvBool (*__gpumgmtapiAccessCallback__)(struct GpuManagementApi *, struct RsClient *, void *, RsAccessRight);
};

#ifndef __NVOC_CLASS_GpuManagementApi_TYPEDEF__
#define __NVOC_CLASS_GpuManagementApi_TYPEDEF__
typedef struct GpuManagementApi GpuManagementApi;
#endif /* __NVOC_CLASS_GpuManagementApi_TYPEDEF__ */

#ifndef __nvoc_class_id_GpuManagementApi
#define __nvoc_class_id_GpuManagementApi 0x376305
#endif /* __nvoc_class_id_GpuManagementApi */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuManagementApi;

#define __staticCast_GpuManagementApi(pThis) \
    ((pThis)->__nvoc_pbase_GpuManagementApi)

#ifdef __nvoc_gpu_mgmt_api_h_disabled
#define __dynamicCast_GpuManagementApi(pThis) ((GpuManagementApi*)NULL)
#else //__nvoc_gpu_mgmt_api_h_disabled
#define __dynamicCast_GpuManagementApi(pThis) \
    ((GpuManagementApi*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(GpuManagementApi)))
#endif //__nvoc_gpu_mgmt_api_h_disabled


NV_STATUS __nvoc_objCreateDynamic_GpuManagementApi(GpuManagementApi**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_GpuManagementApi(GpuManagementApi**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_GpuManagementApi(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_GpuManagementApi((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define gpumgmtapiCtrlCmdSetShutdownState(pGpuMgmt, pParams) gpumgmtapiCtrlCmdSetShutdownState_DISPATCH(pGpuMgmt, pParams)
#define gpumgmtapiShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) gpumgmtapiShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define gpumgmtapiCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) gpumgmtapiCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define gpumgmtapiControl(pResource, pCallContext, pParams) gpumgmtapiControl_DISPATCH(pResource, pCallContext, pParams)
#define gpumgmtapiGetMemInterMapParams(pRmResource, pParams) gpumgmtapiGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define gpumgmtapiGetMemoryMappingDescriptor(pRmResource, ppMemDesc) gpumgmtapiGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define gpumgmtapiGetRefCount(pResource) gpumgmtapiGetRefCount_DISPATCH(pResource)
#define gpumgmtapiControlFilter(pResource, pCallContext, pParams) gpumgmtapiControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define gpumgmtapiAddAdditionalDependants(pClient, pResource, pReference) gpumgmtapiAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define gpumgmtapiUnmap(pResource, pCallContext, pCpuMapping) gpumgmtapiUnmap_DISPATCH(pResource, pCallContext, pCpuMapping)
#define gpumgmtapiControl_Prologue(pResource, pCallContext, pParams) gpumgmtapiControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define gpumgmtapiCanCopy(pResource) gpumgmtapiCanCopy_DISPATCH(pResource)
#define gpumgmtapiMapTo(pResource, pParams) gpumgmtapiMapTo_DISPATCH(pResource, pParams)
#define gpumgmtapiPreDestruct(pResource) gpumgmtapiPreDestruct_DISPATCH(pResource)
#define gpumgmtapiUnmapFrom(pResource, pParams) gpumgmtapiUnmapFrom_DISPATCH(pResource, pParams)
#define gpumgmtapiControl_Epilogue(pResource, pCallContext, pParams) gpumgmtapiControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define gpumgmtapiControlLookup(pResource, pParams, ppEntry) gpumgmtapiControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define gpumgmtapiMap(pResource, pCallContext, pParams, pCpuMapping) gpumgmtapiMap_DISPATCH(pResource, pCallContext, pParams, pCpuMapping)
#define gpumgmtapiAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) gpumgmtapiAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
NV_STATUS gpumgmtapiCtrlCmdSetShutdownState_IMPL(struct GpuManagementApi *pGpuMgmt, NV0020_CTRL_GPU_MGMT_SET_SHUTDOWN_STATE_PARAMS *pParams);

static inline NV_STATUS gpumgmtapiCtrlCmdSetShutdownState_DISPATCH(struct GpuManagementApi *pGpuMgmt, NV0020_CTRL_GPU_MGMT_SET_SHUTDOWN_STATE_PARAMS *pParams) {
    return pGpuMgmt->__gpumgmtapiCtrlCmdSetShutdownState__(pGpuMgmt, pParams);
}

static inline NvBool gpumgmtapiShareCallback_DISPATCH(struct GpuManagementApi *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__gpumgmtapiShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS gpumgmtapiCheckMemInterUnmap_DISPATCH(struct GpuManagementApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__gpumgmtapiCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS gpumgmtapiControl_DISPATCH(struct GpuManagementApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__gpumgmtapiControl__(pResource, pCallContext, pParams);
}

static inline NV_STATUS gpumgmtapiGetMemInterMapParams_DISPATCH(struct GpuManagementApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__gpumgmtapiGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS gpumgmtapiGetMemoryMappingDescriptor_DISPATCH(struct GpuManagementApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__gpumgmtapiGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NvU32 gpumgmtapiGetRefCount_DISPATCH(struct GpuManagementApi *pResource) {
    return pResource->__gpumgmtapiGetRefCount__(pResource);
}

static inline NV_STATUS gpumgmtapiControlFilter_DISPATCH(struct GpuManagementApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__gpumgmtapiControlFilter__(pResource, pCallContext, pParams);
}

static inline void gpumgmtapiAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct GpuManagementApi *pResource, RsResourceRef *pReference) {
    pResource->__gpumgmtapiAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS gpumgmtapiUnmap_DISPATCH(struct GpuManagementApi *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pResource->__gpumgmtapiUnmap__(pResource, pCallContext, pCpuMapping);
}

static inline NV_STATUS gpumgmtapiControl_Prologue_DISPATCH(struct GpuManagementApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__gpumgmtapiControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool gpumgmtapiCanCopy_DISPATCH(struct GpuManagementApi *pResource) {
    return pResource->__gpumgmtapiCanCopy__(pResource);
}

static inline NV_STATUS gpumgmtapiMapTo_DISPATCH(struct GpuManagementApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__gpumgmtapiMapTo__(pResource, pParams);
}

static inline void gpumgmtapiPreDestruct_DISPATCH(struct GpuManagementApi *pResource) {
    pResource->__gpumgmtapiPreDestruct__(pResource);
}

static inline NV_STATUS gpumgmtapiUnmapFrom_DISPATCH(struct GpuManagementApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__gpumgmtapiUnmapFrom__(pResource, pParams);
}

static inline void gpumgmtapiControl_Epilogue_DISPATCH(struct GpuManagementApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__gpumgmtapiControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS gpumgmtapiControlLookup_DISPATCH(struct GpuManagementApi *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__gpumgmtapiControlLookup__(pResource, pParams, ppEntry);
}

static inline NV_STATUS gpumgmtapiMap_DISPATCH(struct GpuManagementApi *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pResource->__gpumgmtapiMap__(pResource, pCallContext, pParams, pCpuMapping);
}

static inline NvBool gpumgmtapiAccessCallback_DISPATCH(struct GpuManagementApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__gpumgmtapiAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

NV_STATUS gpumgmtapiConstruct_IMPL(struct GpuManagementApi *arg_pGpuMgmt, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __nvoc_gpumgmtapiConstruct(arg_pGpuMgmt, arg_pCallContext, arg_pParams) gpumgmtapiConstruct_IMPL(arg_pGpuMgmt, arg_pCallContext, arg_pParams)
void gpumgmtapiDestruct_IMPL(struct GpuManagementApi *pGpuMgmt);
#define __nvoc_gpumgmtapiDestruct(pGpuMgmt) gpumgmtapiDestruct_IMPL(pGpuMgmt)
#undef PRIVATE_FIELD


#endif // GPU_MGMT_API_H

#ifdef __cplusplus
} // extern "C"
#endif
#endif // _G_GPU_MGMT_API_NVOC_H_
