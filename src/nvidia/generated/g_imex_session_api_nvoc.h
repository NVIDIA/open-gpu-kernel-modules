
#ifndef _G_IMEX_SESSION_API_NVOC_H_
#define _G_IMEX_SESSION_API_NVOC_H_
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
 * SPDX-FileCopyrightText: Copyright (c) 2022-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 *       This file contains the functions managing the IMEX session
 *
 *****************************************************************************/

#pragma once
#include "g_imex_session_api_nvoc.h"

#ifndef IMEX_SESSION_API_H
#define IMEX_SESSION_API_H

#include "rmapi/resource.h"

#include "ctrl/ctrl00f1.h"

// ****************************************************************************
//                          Type Definitions
// ****************************************************************************

//
// IMEX session information
//
// A client which owns ImexSessionApi is identified as IMEX daemon in RM land.
// The privileged daemon manages (IMport/EXport) RM resources such as memory,
// across the nodes, in multinode systems.
//
// Key attributes of ImexSessionApi class:
// - There can be only one instance of ImexSessionApi system-wide. This ensures that
//   there is only one IMEX daemon running in the node exclusively.
// - hClient is parent of ImexSessionApi.
// - Only privileged user be allowed to allocate ImexSessionApi.
// - RmApi lock must be held. This class should not communicate with a GPU.
//

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_IMEX_SESSION_API_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct ImexSessionApi {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct RmResource __nvoc_base_RmResource;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^3
    struct RsResource *__nvoc_pbase_RsResource;    // res super^2
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^2
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super
    struct ImexSessionApi *__nvoc_pbase_ImexSessionApi;    // imexsessionapi

    // Vtable with 24 per-object function pointers
    NV_STATUS (*__imexsessionapiCtrlCmdGetFabricEvents__)(struct ImexSessionApi * /*this*/, NV00F1_CTRL_GET_FABRIC_EVENTS_PARAMS *);  // exported (id=0xf10001)
    NV_STATUS (*__imexsessionapiCtrlCmdFinishMemUnimport__)(struct ImexSessionApi * /*this*/, NV00F1_CTRL_FINISH_MEM_UNIMPORT_PARAMS *);  // exported (id=0xf10002)
    NV_STATUS (*__imexsessionapiCtrlCmdDisableImporters__)(struct ImexSessionApi * /*this*/, NV00F1_CTRL_DISABLE_IMPORTERS_PARAMS *);  // exported (id=0xf10003)
    NvBool (*__imexsessionapiAccessCallback__)(struct ImexSessionApi * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (rmres)
    NvBool (*__imexsessionapiShareCallback__)(struct ImexSessionApi * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__imexsessionapiGetMemInterMapParams__)(struct ImexSessionApi * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__imexsessionapiCheckMemInterUnmap__)(struct ImexSessionApi * /*this*/, NvBool);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__imexsessionapiGetMemoryMappingDescriptor__)(struct ImexSessionApi * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__imexsessionapiControlSerialization_Prologue__)(struct ImexSessionApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (rmres)
    void (*__imexsessionapiControlSerialization_Epilogue__)(struct ImexSessionApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__imexsessionapiControl_Prologue__)(struct ImexSessionApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (rmres)
    void (*__imexsessionapiControl_Epilogue__)(struct ImexSessionApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (rmres)
    NvBool (*__imexsessionapiCanCopy__)(struct ImexSessionApi * /*this*/);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__imexsessionapiIsDuplicate__)(struct ImexSessionApi * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (rmres)
    void (*__imexsessionapiPreDestruct__)(struct ImexSessionApi * /*this*/);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__imexsessionapiControl__)(struct ImexSessionApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__imexsessionapiControlFilter__)(struct ImexSessionApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__imexsessionapiMap__)(struct ImexSessionApi * /*this*/, struct CALL_CONTEXT *, RS_CPU_MAP_PARAMS *, RsCpuMapping *);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__imexsessionapiUnmap__)(struct ImexSessionApi * /*this*/, struct CALL_CONTEXT *, RsCpuMapping *);  // virtual inherited (res) base (rmres)
    NvBool (*__imexsessionapiIsPartialUnmapSupported__)(struct ImexSessionApi * /*this*/);  // inline virtual inherited (res) base (rmres) body
    NV_STATUS (*__imexsessionapiMapTo__)(struct ImexSessionApi * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__imexsessionapiUnmapFrom__)(struct ImexSessionApi * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (rmres)
    NvU32 (*__imexsessionapiGetRefCount__)(struct ImexSessionApi * /*this*/);  // virtual inherited (res) base (rmres)
    void (*__imexsessionapiAddAdditionalDependants__)(struct RsClient *, struct ImexSessionApi * /*this*/, RsResourceRef *);  // virtual inherited (res) base (rmres)

    // Data members
    NvU64 dupedCapDescriptor;
    NvU32 PRIVATE_FIELD(flags);
};

#ifndef __NVOC_CLASS_ImexSessionApi_TYPEDEF__
#define __NVOC_CLASS_ImexSessionApi_TYPEDEF__
typedef struct ImexSessionApi ImexSessionApi;
#endif /* __NVOC_CLASS_ImexSessionApi_TYPEDEF__ */

#ifndef __nvoc_class_id_ImexSessionApi
#define __nvoc_class_id_ImexSessionApi 0xb4748b
#endif /* __nvoc_class_id_ImexSessionApi */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_ImexSessionApi;

#define __staticCast_ImexSessionApi(pThis) \
    ((pThis)->__nvoc_pbase_ImexSessionApi)

#ifdef __nvoc_imex_session_api_h_disabled
#define __dynamicCast_ImexSessionApi(pThis) ((ImexSessionApi*)NULL)
#else //__nvoc_imex_session_api_h_disabled
#define __dynamicCast_ImexSessionApi(pThis) \
    ((ImexSessionApi*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(ImexSessionApi)))
#endif //__nvoc_imex_session_api_h_disabled

NV_STATUS __nvoc_objCreateDynamic_ImexSessionApi(ImexSessionApi**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_ImexSessionApi(ImexSessionApi**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_ImexSessionApi(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_ImexSessionApi((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define imexsessionapiCtrlCmdGetFabricEvents_FNPTR(pImexSessionApi) pImexSessionApi->__imexsessionapiCtrlCmdGetFabricEvents__
#define imexsessionapiCtrlCmdGetFabricEvents(pImexSessionApi, pParams) imexsessionapiCtrlCmdGetFabricEvents_DISPATCH(pImexSessionApi, pParams)
#define imexsessionapiCtrlCmdFinishMemUnimport_FNPTR(pImexSessionApi) pImexSessionApi->__imexsessionapiCtrlCmdFinishMemUnimport__
#define imexsessionapiCtrlCmdFinishMemUnimport(pImexSessionApi, pParams) imexsessionapiCtrlCmdFinishMemUnimport_DISPATCH(pImexSessionApi, pParams)
#define imexsessionapiCtrlCmdDisableImporters_FNPTR(pImexSessionApi) pImexSessionApi->__imexsessionapiCtrlCmdDisableImporters__
#define imexsessionapiCtrlCmdDisableImporters(pImexSessionApi, pParams) imexsessionapiCtrlCmdDisableImporters_DISPATCH(pImexSessionApi, pParams)
#define imexsessionapiAccessCallback_FNPTR(pResource) pResource->__nvoc_base_RmResource.__rmresAccessCallback__
#define imexsessionapiAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) imexsessionapiAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define imexsessionapiShareCallback_FNPTR(pResource) pResource->__nvoc_base_RmResource.__rmresShareCallback__
#define imexsessionapiShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) imexsessionapiShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define imexsessionapiGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__rmresGetMemInterMapParams__
#define imexsessionapiGetMemInterMapParams(pRmResource, pParams) imexsessionapiGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define imexsessionapiCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__rmresCheckMemInterUnmap__
#define imexsessionapiCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) imexsessionapiCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define imexsessionapiGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__rmresGetMemoryMappingDescriptor__
#define imexsessionapiGetMemoryMappingDescriptor(pRmResource, ppMemDesc) imexsessionapiGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define imexsessionapiControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__rmresControlSerialization_Prologue__
#define imexsessionapiControlSerialization_Prologue(pResource, pCallContext, pParams) imexsessionapiControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define imexsessionapiControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__rmresControlSerialization_Epilogue__
#define imexsessionapiControlSerialization_Epilogue(pResource, pCallContext, pParams) imexsessionapiControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define imexsessionapiControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__rmresControl_Prologue__
#define imexsessionapiControl_Prologue(pResource, pCallContext, pParams) imexsessionapiControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define imexsessionapiControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__rmresControl_Epilogue__
#define imexsessionapiControl_Epilogue(pResource, pCallContext, pParams) imexsessionapiControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define imexsessionapiCanCopy_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__
#define imexsessionapiCanCopy(pResource) imexsessionapiCanCopy_DISPATCH(pResource)
#define imexsessionapiIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsDuplicate__
#define imexsessionapiIsDuplicate(pResource, hMemory, pDuplicate) imexsessionapiIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define imexsessionapiPreDestruct_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resPreDestruct__
#define imexsessionapiPreDestruct(pResource) imexsessionapiPreDestruct_DISPATCH(pResource)
#define imexsessionapiControl_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resControl__
#define imexsessionapiControl(pResource, pCallContext, pParams) imexsessionapiControl_DISPATCH(pResource, pCallContext, pParams)
#define imexsessionapiControlFilter_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resControlFilter__
#define imexsessionapiControlFilter(pResource, pCallContext, pParams) imexsessionapiControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define imexsessionapiMap_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resMap__
#define imexsessionapiMap(pResource, pCallContext, pParams, pCpuMapping) imexsessionapiMap_DISPATCH(pResource, pCallContext, pParams, pCpuMapping)
#define imexsessionapiUnmap_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmap__
#define imexsessionapiUnmap(pResource, pCallContext, pCpuMapping) imexsessionapiUnmap_DISPATCH(pResource, pCallContext, pCpuMapping)
#define imexsessionapiIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsPartialUnmapSupported__
#define imexsessionapiIsPartialUnmapSupported(pResource) imexsessionapiIsPartialUnmapSupported_DISPATCH(pResource)
#define imexsessionapiMapTo_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resMapTo__
#define imexsessionapiMapTo(pResource, pParams) imexsessionapiMapTo_DISPATCH(pResource, pParams)
#define imexsessionapiUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmapFrom__
#define imexsessionapiUnmapFrom(pResource, pParams) imexsessionapiUnmapFrom_DISPATCH(pResource, pParams)
#define imexsessionapiGetRefCount_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resGetRefCount__
#define imexsessionapiGetRefCount(pResource) imexsessionapiGetRefCount_DISPATCH(pResource)
#define imexsessionapiAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resAddAdditionalDependants__
#define imexsessionapiAddAdditionalDependants(pClient, pResource, pReference) imexsessionapiAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NV_STATUS imexsessionapiCtrlCmdGetFabricEvents_DISPATCH(struct ImexSessionApi *pImexSessionApi, NV00F1_CTRL_GET_FABRIC_EVENTS_PARAMS *pParams) {
    return pImexSessionApi->__imexsessionapiCtrlCmdGetFabricEvents__(pImexSessionApi, pParams);
}

static inline NV_STATUS imexsessionapiCtrlCmdFinishMemUnimport_DISPATCH(struct ImexSessionApi *pImexSessionApi, NV00F1_CTRL_FINISH_MEM_UNIMPORT_PARAMS *pParams) {
    return pImexSessionApi->__imexsessionapiCtrlCmdFinishMemUnimport__(pImexSessionApi, pParams);
}

static inline NV_STATUS imexsessionapiCtrlCmdDisableImporters_DISPATCH(struct ImexSessionApi *pImexSessionApi, NV00F1_CTRL_DISABLE_IMPORTERS_PARAMS *pParams) {
    return pImexSessionApi->__imexsessionapiCtrlCmdDisableImporters__(pImexSessionApi, pParams);
}

static inline NvBool imexsessionapiAccessCallback_DISPATCH(struct ImexSessionApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__imexsessionapiAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool imexsessionapiShareCallback_DISPATCH(struct ImexSessionApi *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__imexsessionapiShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS imexsessionapiGetMemInterMapParams_DISPATCH(struct ImexSessionApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__imexsessionapiGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS imexsessionapiCheckMemInterUnmap_DISPATCH(struct ImexSessionApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__imexsessionapiCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS imexsessionapiGetMemoryMappingDescriptor_DISPATCH(struct ImexSessionApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__imexsessionapiGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS imexsessionapiControlSerialization_Prologue_DISPATCH(struct ImexSessionApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__imexsessionapiControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void imexsessionapiControlSerialization_Epilogue_DISPATCH(struct ImexSessionApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__imexsessionapiControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS imexsessionapiControl_Prologue_DISPATCH(struct ImexSessionApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__imexsessionapiControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void imexsessionapiControl_Epilogue_DISPATCH(struct ImexSessionApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__imexsessionapiControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool imexsessionapiCanCopy_DISPATCH(struct ImexSessionApi *pResource) {
    return pResource->__imexsessionapiCanCopy__(pResource);
}

static inline NV_STATUS imexsessionapiIsDuplicate_DISPATCH(struct ImexSessionApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__imexsessionapiIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void imexsessionapiPreDestruct_DISPATCH(struct ImexSessionApi *pResource) {
    pResource->__imexsessionapiPreDestruct__(pResource);
}

static inline NV_STATUS imexsessionapiControl_DISPATCH(struct ImexSessionApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__imexsessionapiControl__(pResource, pCallContext, pParams);
}

static inline NV_STATUS imexsessionapiControlFilter_DISPATCH(struct ImexSessionApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__imexsessionapiControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS imexsessionapiMap_DISPATCH(struct ImexSessionApi *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pResource->__imexsessionapiMap__(pResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS imexsessionapiUnmap_DISPATCH(struct ImexSessionApi *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pResource->__imexsessionapiUnmap__(pResource, pCallContext, pCpuMapping);
}

static inline NvBool imexsessionapiIsPartialUnmapSupported_DISPATCH(struct ImexSessionApi *pResource) {
    return pResource->__imexsessionapiIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS imexsessionapiMapTo_DISPATCH(struct ImexSessionApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__imexsessionapiMapTo__(pResource, pParams);
}

static inline NV_STATUS imexsessionapiUnmapFrom_DISPATCH(struct ImexSessionApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__imexsessionapiUnmapFrom__(pResource, pParams);
}

static inline NvU32 imexsessionapiGetRefCount_DISPATCH(struct ImexSessionApi *pResource) {
    return pResource->__imexsessionapiGetRefCount__(pResource);
}

static inline void imexsessionapiAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct ImexSessionApi *pResource, RsResourceRef *pReference) {
    pResource->__imexsessionapiAddAdditionalDependants__(pClient, pResource, pReference);
}

NV_STATUS imexsessionapiCtrlCmdGetFabricEvents_IMPL(struct ImexSessionApi *pImexSessionApi, NV00F1_CTRL_GET_FABRIC_EVENTS_PARAMS *pParams);

NV_STATUS imexsessionapiCtrlCmdFinishMemUnimport_IMPL(struct ImexSessionApi *pImexSessionApi, NV00F1_CTRL_FINISH_MEM_UNIMPORT_PARAMS *pParams);

NV_STATUS imexsessionapiCtrlCmdDisableImporters_IMPL(struct ImexSessionApi *pImexSessionApi, NV00F1_CTRL_DISABLE_IMPORTERS_PARAMS *pParams);

NV_STATUS imexsessionapiConstruct_IMPL(struct ImexSessionApi *arg_pImexSessionApi, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_imexsessionapiConstruct(arg_pImexSessionApi, arg_pCallContext, arg_pParams) imexsessionapiConstruct_IMPL(arg_pImexSessionApi, arg_pCallContext, arg_pParams)
void imexsessionapiDestruct_IMPL(struct ImexSessionApi *pImexSessionApi);

#define __nvoc_imexsessionapiDestruct(pImexSessionApi) imexsessionapiDestruct_IMPL(pImexSessionApi)
#undef PRIVATE_FIELD


void rcAndDisableOutstandingClientsWithImportedMemory(OBJGPU *pGpu, NvU16 nodeId);

#endif // IMEX_SESSION_API_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_IMEX_SESSION_API_NVOC_H_
