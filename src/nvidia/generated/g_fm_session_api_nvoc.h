
#ifndef _G_FM_SESSION_API_NVOC_H_
#define _G_FM_SESSION_API_NVOC_H_
#include "nvoc/runtime.h"

// Version of generated metadata structures
#ifdef NVOC_METADATA_VERSION
#undef NVOC_METADATA_VERSION
#endif
#define NVOC_METADATA_VERSION 0

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

#pragma once
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

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_FM_SESSION_API_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct FmSessionApi {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct RmResource __nvoc_base_RmResource;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^3
    struct RsResource *__nvoc_pbase_RsResource;    // res super^2
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^2
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super
    struct FmSessionApi *__nvoc_pbase_FmSessionApi;    // fmsessionapi

    // Vtable with 23 per-object function pointers
    NV_STATUS (*__fmsessionapiCtrlCmdSetFmState__)(struct FmSessionApi * /*this*/);  // exported (id=0xf0101)
    NV_STATUS (*__fmsessionapiCtrlCmdClearFmState__)(struct FmSessionApi * /*this*/);  // exported (id=0xf0102)
    NvBool (*__fmsessionapiAccessCallback__)(struct FmSessionApi * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (rmres)
    NvBool (*__fmsessionapiShareCallback__)(struct FmSessionApi * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__fmsessionapiGetMemInterMapParams__)(struct FmSessionApi * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__fmsessionapiCheckMemInterUnmap__)(struct FmSessionApi * /*this*/, NvBool);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__fmsessionapiGetMemoryMappingDescriptor__)(struct FmSessionApi * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__fmsessionapiControlSerialization_Prologue__)(struct FmSessionApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (rmres)
    void (*__fmsessionapiControlSerialization_Epilogue__)(struct FmSessionApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__fmsessionapiControl_Prologue__)(struct FmSessionApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (rmres)
    void (*__fmsessionapiControl_Epilogue__)(struct FmSessionApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (rmres)
    NvBool (*__fmsessionapiCanCopy__)(struct FmSessionApi * /*this*/);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__fmsessionapiIsDuplicate__)(struct FmSessionApi * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (rmres)
    void (*__fmsessionapiPreDestruct__)(struct FmSessionApi * /*this*/);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__fmsessionapiControl__)(struct FmSessionApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__fmsessionapiControlFilter__)(struct FmSessionApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__fmsessionapiMap__)(struct FmSessionApi * /*this*/, struct CALL_CONTEXT *, RS_CPU_MAP_PARAMS *, RsCpuMapping *);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__fmsessionapiUnmap__)(struct FmSessionApi * /*this*/, struct CALL_CONTEXT *, RsCpuMapping *);  // virtual inherited (res) base (rmres)
    NvBool (*__fmsessionapiIsPartialUnmapSupported__)(struct FmSessionApi * /*this*/);  // inline virtual inherited (res) base (rmres) body
    NV_STATUS (*__fmsessionapiMapTo__)(struct FmSessionApi * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__fmsessionapiUnmapFrom__)(struct FmSessionApi * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (rmres)
    NvU32 (*__fmsessionapiGetRefCount__)(struct FmSessionApi * /*this*/);  // virtual inherited (res) base (rmres)
    void (*__fmsessionapiAddAdditionalDependants__)(struct RsClient *, struct FmSessionApi * /*this*/, RsResourceRef *);  // virtual inherited (res) base (rmres)

    // Data members
    NvU64 dupedCapDescriptor;
};

#ifndef __NVOC_CLASS_FmSessionApi_TYPEDEF__
#define __NVOC_CLASS_FmSessionApi_TYPEDEF__
typedef struct FmSessionApi FmSessionApi;
#endif /* __NVOC_CLASS_FmSessionApi_TYPEDEF__ */

#ifndef __nvoc_class_id_FmSessionApi
#define __nvoc_class_id_FmSessionApi 0xdfbd08
#endif /* __nvoc_class_id_FmSessionApi */

// Casting support
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


// Wrapper macros
#define fmsessionapiCtrlCmdSetFmState_FNPTR(pFmSessionApi) pFmSessionApi->__fmsessionapiCtrlCmdSetFmState__
#define fmsessionapiCtrlCmdSetFmState(pFmSessionApi) fmsessionapiCtrlCmdSetFmState_DISPATCH(pFmSessionApi)
#define fmsessionapiCtrlCmdClearFmState_FNPTR(pFmSessionApi) pFmSessionApi->__fmsessionapiCtrlCmdClearFmState__
#define fmsessionapiCtrlCmdClearFmState(pFmSessionApi) fmsessionapiCtrlCmdClearFmState_DISPATCH(pFmSessionApi)
#define fmsessionapiAccessCallback_FNPTR(pResource) pResource->__nvoc_base_RmResource.__rmresAccessCallback__
#define fmsessionapiAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) fmsessionapiAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define fmsessionapiShareCallback_FNPTR(pResource) pResource->__nvoc_base_RmResource.__rmresShareCallback__
#define fmsessionapiShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) fmsessionapiShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define fmsessionapiGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__rmresGetMemInterMapParams__
#define fmsessionapiGetMemInterMapParams(pRmResource, pParams) fmsessionapiGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define fmsessionapiCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__rmresCheckMemInterUnmap__
#define fmsessionapiCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) fmsessionapiCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define fmsessionapiGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__rmresGetMemoryMappingDescriptor__
#define fmsessionapiGetMemoryMappingDescriptor(pRmResource, ppMemDesc) fmsessionapiGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define fmsessionapiControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__rmresControlSerialization_Prologue__
#define fmsessionapiControlSerialization_Prologue(pResource, pCallContext, pParams) fmsessionapiControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define fmsessionapiControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__rmresControlSerialization_Epilogue__
#define fmsessionapiControlSerialization_Epilogue(pResource, pCallContext, pParams) fmsessionapiControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define fmsessionapiControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__rmresControl_Prologue__
#define fmsessionapiControl_Prologue(pResource, pCallContext, pParams) fmsessionapiControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define fmsessionapiControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__rmresControl_Epilogue__
#define fmsessionapiControl_Epilogue(pResource, pCallContext, pParams) fmsessionapiControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define fmsessionapiCanCopy_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__
#define fmsessionapiCanCopy(pResource) fmsessionapiCanCopy_DISPATCH(pResource)
#define fmsessionapiIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsDuplicate__
#define fmsessionapiIsDuplicate(pResource, hMemory, pDuplicate) fmsessionapiIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define fmsessionapiPreDestruct_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resPreDestruct__
#define fmsessionapiPreDestruct(pResource) fmsessionapiPreDestruct_DISPATCH(pResource)
#define fmsessionapiControl_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resControl__
#define fmsessionapiControl(pResource, pCallContext, pParams) fmsessionapiControl_DISPATCH(pResource, pCallContext, pParams)
#define fmsessionapiControlFilter_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resControlFilter__
#define fmsessionapiControlFilter(pResource, pCallContext, pParams) fmsessionapiControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define fmsessionapiMap_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resMap__
#define fmsessionapiMap(pResource, pCallContext, pParams, pCpuMapping) fmsessionapiMap_DISPATCH(pResource, pCallContext, pParams, pCpuMapping)
#define fmsessionapiUnmap_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmap__
#define fmsessionapiUnmap(pResource, pCallContext, pCpuMapping) fmsessionapiUnmap_DISPATCH(pResource, pCallContext, pCpuMapping)
#define fmsessionapiIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsPartialUnmapSupported__
#define fmsessionapiIsPartialUnmapSupported(pResource) fmsessionapiIsPartialUnmapSupported_DISPATCH(pResource)
#define fmsessionapiMapTo_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resMapTo__
#define fmsessionapiMapTo(pResource, pParams) fmsessionapiMapTo_DISPATCH(pResource, pParams)
#define fmsessionapiUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmapFrom__
#define fmsessionapiUnmapFrom(pResource, pParams) fmsessionapiUnmapFrom_DISPATCH(pResource, pParams)
#define fmsessionapiGetRefCount_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resGetRefCount__
#define fmsessionapiGetRefCount(pResource) fmsessionapiGetRefCount_DISPATCH(pResource)
#define fmsessionapiAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resAddAdditionalDependants__
#define fmsessionapiAddAdditionalDependants(pClient, pResource, pReference) fmsessionapiAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NV_STATUS fmsessionapiCtrlCmdSetFmState_DISPATCH(struct FmSessionApi *pFmSessionApi) {
    return pFmSessionApi->__fmsessionapiCtrlCmdSetFmState__(pFmSessionApi);
}

static inline NV_STATUS fmsessionapiCtrlCmdClearFmState_DISPATCH(struct FmSessionApi *pFmSessionApi) {
    return pFmSessionApi->__fmsessionapiCtrlCmdClearFmState__(pFmSessionApi);
}

static inline NvBool fmsessionapiAccessCallback_DISPATCH(struct FmSessionApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__fmsessionapiAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool fmsessionapiShareCallback_DISPATCH(struct FmSessionApi *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__fmsessionapiShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS fmsessionapiGetMemInterMapParams_DISPATCH(struct FmSessionApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__fmsessionapiGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS fmsessionapiCheckMemInterUnmap_DISPATCH(struct FmSessionApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__fmsessionapiCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS fmsessionapiGetMemoryMappingDescriptor_DISPATCH(struct FmSessionApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__fmsessionapiGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS fmsessionapiControlSerialization_Prologue_DISPATCH(struct FmSessionApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__fmsessionapiControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void fmsessionapiControlSerialization_Epilogue_DISPATCH(struct FmSessionApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__fmsessionapiControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS fmsessionapiControl_Prologue_DISPATCH(struct FmSessionApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__fmsessionapiControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void fmsessionapiControl_Epilogue_DISPATCH(struct FmSessionApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__fmsessionapiControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool fmsessionapiCanCopy_DISPATCH(struct FmSessionApi *pResource) {
    return pResource->__fmsessionapiCanCopy__(pResource);
}

static inline NV_STATUS fmsessionapiIsDuplicate_DISPATCH(struct FmSessionApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__fmsessionapiIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void fmsessionapiPreDestruct_DISPATCH(struct FmSessionApi *pResource) {
    pResource->__fmsessionapiPreDestruct__(pResource);
}

static inline NV_STATUS fmsessionapiControl_DISPATCH(struct FmSessionApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__fmsessionapiControl__(pResource, pCallContext, pParams);
}

static inline NV_STATUS fmsessionapiControlFilter_DISPATCH(struct FmSessionApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__fmsessionapiControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS fmsessionapiMap_DISPATCH(struct FmSessionApi *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pResource->__fmsessionapiMap__(pResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS fmsessionapiUnmap_DISPATCH(struct FmSessionApi *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pResource->__fmsessionapiUnmap__(pResource, pCallContext, pCpuMapping);
}

static inline NvBool fmsessionapiIsPartialUnmapSupported_DISPATCH(struct FmSessionApi *pResource) {
    return pResource->__fmsessionapiIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS fmsessionapiMapTo_DISPATCH(struct FmSessionApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__fmsessionapiMapTo__(pResource, pParams);
}

static inline NV_STATUS fmsessionapiUnmapFrom_DISPATCH(struct FmSessionApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__fmsessionapiUnmapFrom__(pResource, pParams);
}

static inline NvU32 fmsessionapiGetRefCount_DISPATCH(struct FmSessionApi *pResource) {
    return pResource->__fmsessionapiGetRefCount__(pResource);
}

static inline void fmsessionapiAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct FmSessionApi *pResource, RsResourceRef *pReference) {
    pResource->__fmsessionapiAddAdditionalDependants__(pClient, pResource, pReference);
}

NV_STATUS fmsessionapiCtrlCmdSetFmState_IMPL(struct FmSessionApi *pFmSessionApi);

NV_STATUS fmsessionapiCtrlCmdClearFmState_IMPL(struct FmSessionApi *pFmSessionApi);

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
