#ifndef _G_FM_SESSION_API_NVOC_H_
#define _G_FM_SESSION_API_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 *       This file contains the functions managing the FM session
 *
 *****************************************************************************/

#include "g_fm_session_api_nvoc.h"

#ifndef FM_SESSION_API_H
#define FM_SESSION_API_H

#include "rmapi/resource.h"
#include "rmapi/event.h"
#include "ctrl/ctrl000f.h"

// ****************************************************************************
//                          Type Definitions
// ****************************************************************************

//
// FM session information
//
// A client which owns FmSessionApi is identified as Fabric Manager in RM land.
//
// Key attributes of FmSessionApi class:
// - There can be only one instance of FmSessionApi system-wide. This ensures that
//   there is only one fabric manager daemon running in the system with exclusive
//   access to FmObject.
// - hClient is parent of FmSessionApi.
// - Only NV01_ROOT_USER should be allowed to allocate FmSessionApi.
// - FmSessionApi can be allocated only by privileged clients.
// - RmApi lock must be held.
//
#ifdef NVOC_FM_SESSION_API_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct FmSessionApi {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct RmResource __nvoc_base_RmResource;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct FmSessionApi *__nvoc_pbase_FmSessionApi;
    NV_STATUS (*__fmsessionapiCtrlCmdSetFmState__)(struct FmSessionApi *);
    NV_STATUS (*__fmsessionapiCtrlCmdClearFmState__)(struct FmSessionApi *);
    NvBool (*__fmsessionapiShareCallback__)(struct FmSessionApi *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__fmsessionapiCheckMemInterUnmap__)(struct FmSessionApi *, NvBool);
    NV_STATUS (*__fmsessionapiControl__)(struct FmSessionApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__fmsessionapiGetMemInterMapParams__)(struct FmSessionApi *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__fmsessionapiGetMemoryMappingDescriptor__)(struct FmSessionApi *, struct MEMORY_DESCRIPTOR **);
    NvU32 (*__fmsessionapiGetRefCount__)(struct FmSessionApi *);
    NV_STATUS (*__fmsessionapiControlFilter__)(struct FmSessionApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    void (*__fmsessionapiAddAdditionalDependants__)(struct RsClient *, struct FmSessionApi *, RsResourceRef *);
    NV_STATUS (*__fmsessionapiUnmapFrom__)(struct FmSessionApi *, RS_RES_UNMAP_FROM_PARAMS *);
    NV_STATUS (*__fmsessionapiControlSerialization_Prologue__)(struct FmSessionApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__fmsessionapiControl_Prologue__)(struct FmSessionApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__fmsessionapiCanCopy__)(struct FmSessionApi *);
    NV_STATUS (*__fmsessionapiUnmap__)(struct FmSessionApi *, struct CALL_CONTEXT *, RsCpuMapping *);
    void (*__fmsessionapiPreDestruct__)(struct FmSessionApi *);
    NV_STATUS (*__fmsessionapiMapTo__)(struct FmSessionApi *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__fmsessionapiIsDuplicate__)(struct FmSessionApi *, NvHandle, NvBool *);
    void (*__fmsessionapiControlSerialization_Epilogue__)(struct FmSessionApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    void (*__fmsessionapiControl_Epilogue__)(struct FmSessionApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__fmsessionapiControlLookup__)(struct FmSessionApi *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NV_STATUS (*__fmsessionapiMap__)(struct FmSessionApi *, struct CALL_CONTEXT *, RS_CPU_MAP_PARAMS *, RsCpuMapping *);
    NvBool (*__fmsessionapiAccessCallback__)(struct FmSessionApi *, struct RsClient *, void *, RsAccessRight);
    NvU64 dupedCapDescriptor;
};

#ifndef __NVOC_CLASS_FmSessionApi_TYPEDEF__
#define __NVOC_CLASS_FmSessionApi_TYPEDEF__
typedef struct FmSessionApi FmSessionApi;
#endif /* __NVOC_CLASS_FmSessionApi_TYPEDEF__ */

#ifndef __nvoc_class_id_FmSessionApi
#define __nvoc_class_id_FmSessionApi 0xdfbd08
#endif /* __nvoc_class_id_FmSessionApi */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_FmSessionApi;

#define __staticCast_FmSessionApi(pThis) \
    ((pThis)->__nvoc_pbase_FmSessionApi)

#ifdef __nvoc_fm_session_api_h_disabled
#define __dynamicCast_FmSessionApi(pThis) ((FmSessionApi*)NULL)
#else //__nvoc_fm_session_api_h_disabled
#define __dynamicCast_FmSessionApi(pThis) \
    ((FmSessionApi*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(FmSessionApi)))
#endif //__nvoc_fm_session_api_h_disabled


NV_STATUS __nvoc_objCreateDynamic_FmSessionApi(FmSessionApi**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_FmSessionApi(FmSessionApi**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_FmSessionApi(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_FmSessionApi((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define fmsessionapiCtrlCmdSetFmState(pFmSessionApi) fmsessionapiCtrlCmdSetFmState_DISPATCH(pFmSessionApi)
#define fmsessionapiCtrlCmdClearFmState(pFmSessionApi) fmsessionapiCtrlCmdClearFmState_DISPATCH(pFmSessionApi)
#define fmsessionapiShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) fmsessionapiShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define fmsessionapiCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) fmsessionapiCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define fmsessionapiControl(pResource, pCallContext, pParams) fmsessionapiControl_DISPATCH(pResource, pCallContext, pParams)
#define fmsessionapiGetMemInterMapParams(pRmResource, pParams) fmsessionapiGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define fmsessionapiGetMemoryMappingDescriptor(pRmResource, ppMemDesc) fmsessionapiGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define fmsessionapiGetRefCount(pResource) fmsessionapiGetRefCount_DISPATCH(pResource)
#define fmsessionapiControlFilter(pResource, pCallContext, pParams) fmsessionapiControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define fmsessionapiAddAdditionalDependants(pClient, pResource, pReference) fmsessionapiAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define fmsessionapiUnmapFrom(pResource, pParams) fmsessionapiUnmapFrom_DISPATCH(pResource, pParams)
#define fmsessionapiControlSerialization_Prologue(pResource, pCallContext, pParams) fmsessionapiControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define fmsessionapiControl_Prologue(pResource, pCallContext, pParams) fmsessionapiControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define fmsessionapiCanCopy(pResource) fmsessionapiCanCopy_DISPATCH(pResource)
#define fmsessionapiUnmap(pResource, pCallContext, pCpuMapping) fmsessionapiUnmap_DISPATCH(pResource, pCallContext, pCpuMapping)
#define fmsessionapiPreDestruct(pResource) fmsessionapiPreDestruct_DISPATCH(pResource)
#define fmsessionapiMapTo(pResource, pParams) fmsessionapiMapTo_DISPATCH(pResource, pParams)
#define fmsessionapiIsDuplicate(pResource, hMemory, pDuplicate) fmsessionapiIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define fmsessionapiControlSerialization_Epilogue(pResource, pCallContext, pParams) fmsessionapiControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define fmsessionapiControl_Epilogue(pResource, pCallContext, pParams) fmsessionapiControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define fmsessionapiControlLookup(pResource, pParams, ppEntry) fmsessionapiControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define fmsessionapiMap(pResource, pCallContext, pParams, pCpuMapping) fmsessionapiMap_DISPATCH(pResource, pCallContext, pParams, pCpuMapping)
#define fmsessionapiAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) fmsessionapiAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
NV_STATUS fmsessionapiCtrlCmdSetFmState_IMPL(struct FmSessionApi *pFmSessionApi);

static inline NV_STATUS fmsessionapiCtrlCmdSetFmState_DISPATCH(struct FmSessionApi *pFmSessionApi) {
    return pFmSessionApi->__fmsessionapiCtrlCmdSetFmState__(pFmSessionApi);
}

NV_STATUS fmsessionapiCtrlCmdClearFmState_IMPL(struct FmSessionApi *pFmSessionApi);

static inline NV_STATUS fmsessionapiCtrlCmdClearFmState_DISPATCH(struct FmSessionApi *pFmSessionApi) {
    return pFmSessionApi->__fmsessionapiCtrlCmdClearFmState__(pFmSessionApi);
}

static inline NvBool fmsessionapiShareCallback_DISPATCH(struct FmSessionApi *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__fmsessionapiShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS fmsessionapiCheckMemInterUnmap_DISPATCH(struct FmSessionApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__fmsessionapiCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS fmsessionapiControl_DISPATCH(struct FmSessionApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__fmsessionapiControl__(pResource, pCallContext, pParams);
}

static inline NV_STATUS fmsessionapiGetMemInterMapParams_DISPATCH(struct FmSessionApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__fmsessionapiGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS fmsessionapiGetMemoryMappingDescriptor_DISPATCH(struct FmSessionApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__fmsessionapiGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NvU32 fmsessionapiGetRefCount_DISPATCH(struct FmSessionApi *pResource) {
    return pResource->__fmsessionapiGetRefCount__(pResource);
}

static inline NV_STATUS fmsessionapiControlFilter_DISPATCH(struct FmSessionApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__fmsessionapiControlFilter__(pResource, pCallContext, pParams);
}

static inline void fmsessionapiAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct FmSessionApi *pResource, RsResourceRef *pReference) {
    pResource->__fmsessionapiAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS fmsessionapiUnmapFrom_DISPATCH(struct FmSessionApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__fmsessionapiUnmapFrom__(pResource, pParams);
}

static inline NV_STATUS fmsessionapiControlSerialization_Prologue_DISPATCH(struct FmSessionApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__fmsessionapiControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS fmsessionapiControl_Prologue_DISPATCH(struct FmSessionApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__fmsessionapiControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool fmsessionapiCanCopy_DISPATCH(struct FmSessionApi *pResource) {
    return pResource->__fmsessionapiCanCopy__(pResource);
}

static inline NV_STATUS fmsessionapiUnmap_DISPATCH(struct FmSessionApi *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pResource->__fmsessionapiUnmap__(pResource, pCallContext, pCpuMapping);
}

static inline void fmsessionapiPreDestruct_DISPATCH(struct FmSessionApi *pResource) {
    pResource->__fmsessionapiPreDestruct__(pResource);
}

static inline NV_STATUS fmsessionapiMapTo_DISPATCH(struct FmSessionApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__fmsessionapiMapTo__(pResource, pParams);
}

static inline NV_STATUS fmsessionapiIsDuplicate_DISPATCH(struct FmSessionApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__fmsessionapiIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void fmsessionapiControlSerialization_Epilogue_DISPATCH(struct FmSessionApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__fmsessionapiControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline void fmsessionapiControl_Epilogue_DISPATCH(struct FmSessionApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__fmsessionapiControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS fmsessionapiControlLookup_DISPATCH(struct FmSessionApi *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__fmsessionapiControlLookup__(pResource, pParams, ppEntry);
}

static inline NV_STATUS fmsessionapiMap_DISPATCH(struct FmSessionApi *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pResource->__fmsessionapiMap__(pResource, pCallContext, pParams, pCpuMapping);
}

static inline NvBool fmsessionapiAccessCallback_DISPATCH(struct FmSessionApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__fmsessionapiAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

NV_STATUS fmsessionapiConstruct_IMPL(struct FmSessionApi *arg_pFmSessionApi, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_fmsessionapiConstruct(arg_pFmSessionApi, arg_pCallContext, arg_pParams) fmsessionapiConstruct_IMPL(arg_pFmSessionApi, arg_pCallContext, arg_pParams)
void fmsessionapiDestruct_IMPL(struct FmSessionApi *pFmSessionApi);

#define __nvoc_fmsessionapiDestruct(pFmSessionApi) fmsessionapiDestruct_IMPL(pFmSessionApi)
#undef PRIVATE_FIELD


#endif // FM_SESSION_API_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_FM_SESSION_API_NVOC_H_
