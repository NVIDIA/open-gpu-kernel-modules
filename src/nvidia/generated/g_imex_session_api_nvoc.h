
#ifndef _G_IMEX_SESSION_API_NVOC_H_
#define _G_IMEX_SESSION_API_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2022-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__ImexSessionApi;
struct NVOC_METADATA__RmResource;


struct ImexSessionApi {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__ImexSessionApi *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct RmResource __nvoc_base_RmResource;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^3
    struct RsResource *__nvoc_pbase_RsResource;    // res super^2
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^2
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super
    struct ImexSessionApi *__nvoc_pbase_ImexSessionApi;    // imexsessionapi

    // Data members
    NvU64 dupedCapDescriptor;
    NvU32 PRIVATE_FIELD(flags);
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__ImexSessionApi {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__RmResource metadata__RmResource;
};

#ifndef __nvoc_class_id_ImexSessionApi
#define __nvoc_class_id_ImexSessionApi 0xb4748bu
typedef struct ImexSessionApi ImexSessionApi;
#endif /* __nvoc_class_id_ImexSessionApi */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_ImexSessionApi;

#define __staticCast_ImexSessionApi(pThis) \
    ((pThis)->__nvoc_pbase_ImexSessionApi)

#ifdef __nvoc_imex_session_api_h_disabled
#define __dynamicCast_ImexSessionApi(pThis) ((ImexSessionApi*) NULL)
#else //__nvoc_imex_session_api_h_disabled
#define __dynamicCast_ImexSessionApi(pThis) \
    ((ImexSessionApi*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(ImexSessionApi)))
#endif //__nvoc_imex_session_api_h_disabled

NV_STATUS __nvoc_objCreateDynamic_ImexSessionApi(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_ImexSessionApi(ImexSessionApi**, Dynamic*, NvU32, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __objCreate_ImexSessionApi(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams) \
    __nvoc_objCreate_ImexSessionApi((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pCallContext, pParams)


// Wrapper macros for implementation functions
NV_STATUS imexsessionapiConstruct_IMPL(struct ImexSessionApi *pImexSessionApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __nvoc_imexsessionapiConstruct(pImexSessionApi, pCallContext, pParams) imexsessionapiConstruct_IMPL(pImexSessionApi, pCallContext, pParams)

void imexsessionapiDestruct_IMPL(struct ImexSessionApi *pImexSessionApi);
#define __nvoc_imexsessionapiDestruct(pImexSessionApi) imexsessionapiDestruct_IMPL(pImexSessionApi)

NV_STATUS imexsessionapiCtrlCmdGetFabricEvents_IMPL(struct ImexSessionApi *pImexSessionApi, NV00F1_CTRL_GET_FABRIC_EVENTS_PARAMS *pParams);
#ifdef __nvoc_imex_session_api_h_disabled
static inline NV_STATUS imexsessionapiCtrlCmdGetFabricEvents(struct ImexSessionApi *pImexSessionApi, NV00F1_CTRL_GET_FABRIC_EVENTS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("ImexSessionApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_imex_session_api_h_disabled
#define imexsessionapiCtrlCmdGetFabricEvents(pImexSessionApi, pParams) imexsessionapiCtrlCmdGetFabricEvents_IMPL(pImexSessionApi, pParams)
#endif // __nvoc_imex_session_api_h_disabled

NV_STATUS imexsessionapiCtrlCmdFinishMemUnimport_IMPL(struct ImexSessionApi *pImexSessionApi, NV00F1_CTRL_FINISH_MEM_UNIMPORT_PARAMS *pParams);
#ifdef __nvoc_imex_session_api_h_disabled
static inline NV_STATUS imexsessionapiCtrlCmdFinishMemUnimport(struct ImexSessionApi *pImexSessionApi, NV00F1_CTRL_FINISH_MEM_UNIMPORT_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("ImexSessionApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_imex_session_api_h_disabled
#define imexsessionapiCtrlCmdFinishMemUnimport(pImexSessionApi, pParams) imexsessionapiCtrlCmdFinishMemUnimport_IMPL(pImexSessionApi, pParams)
#endif // __nvoc_imex_session_api_h_disabled

NV_STATUS imexsessionapiCtrlCmdDisableImporters_IMPL(struct ImexSessionApi *pImexSessionApi, NV00F1_CTRL_DISABLE_IMPORTERS_PARAMS *pParams);
#ifdef __nvoc_imex_session_api_h_disabled
static inline NV_STATUS imexsessionapiCtrlCmdDisableImporters(struct ImexSessionApi *pImexSessionApi, NV00F1_CTRL_DISABLE_IMPORTERS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("ImexSessionApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_imex_session_api_h_disabled
#define imexsessionapiCtrlCmdDisableImporters(pImexSessionApi, pParams) imexsessionapiCtrlCmdDisableImporters_IMPL(pImexSessionApi, pParams)
#endif // __nvoc_imex_session_api_h_disabled


// Wrapper macros for halified functions
#define imexsessionapiAccessCallback_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__
#define imexsessionapiAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) imexsessionapiAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define imexsessionapiShareCallback_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__
#define imexsessionapiShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) imexsessionapiShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define imexsessionapiGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define imexsessionapiGetMemInterMapParams(pRmResource, pParams) imexsessionapiGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define imexsessionapiCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define imexsessionapiCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) imexsessionapiCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define imexsessionapiGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define imexsessionapiGetMemoryMappingDescriptor(pRmResource, ppMemDesc) imexsessionapiGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define imexsessionapiControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__
#define imexsessionapiControlSerialization_Prologue(pResource, pCallContext, pParams) imexsessionapiControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define imexsessionapiControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__
#define imexsessionapiControlSerialization_Epilogue(pResource, pCallContext, pParams) imexsessionapiControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define imexsessionapiControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__
#define imexsessionapiControl_Prologue(pResource, pCallContext, pParams) imexsessionapiControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define imexsessionapiControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__
#define imexsessionapiControl_Epilogue(pResource, pCallContext, pParams) imexsessionapiControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define imexsessionapiCanCopy_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define imexsessionapiCanCopy(pResource) imexsessionapiCanCopy_DISPATCH(pResource)
#define imexsessionapiIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define imexsessionapiIsDuplicate(pResource, hMemory, pDuplicate) imexsessionapiIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define imexsessionapiPreDestruct_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define imexsessionapiPreDestruct(pResource) imexsessionapiPreDestruct_DISPATCH(pResource)
#define imexsessionapiControl_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__
#define imexsessionapiControl(pResource, pCallContext, pParams) imexsessionapiControl_DISPATCH(pResource, pCallContext, pParams)
#define imexsessionapiControlFilter_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define imexsessionapiControlFilter(pResource, pCallContext, pParams) imexsessionapiControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define imexsessionapiMap_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__
#define imexsessionapiMap(pResource, pCallContext, pParams, pCpuMapping) imexsessionapiMap_DISPATCH(pResource, pCallContext, pParams, pCpuMapping)
#define imexsessionapiUnmap_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__
#define imexsessionapiUnmap(pResource, pCallContext, pCpuMapping) imexsessionapiUnmap_DISPATCH(pResource, pCallContext, pCpuMapping)
#define imexsessionapiIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define imexsessionapiIsPartialUnmapSupported(pResource) imexsessionapiIsPartialUnmapSupported_DISPATCH(pResource)
#define imexsessionapiMapTo_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define imexsessionapiMapTo(pResource, pParams) imexsessionapiMapTo_DISPATCH(pResource, pParams)
#define imexsessionapiUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define imexsessionapiUnmapFrom(pResource, pParams) imexsessionapiUnmapFrom_DISPATCH(pResource, pParams)
#define imexsessionapiGetRefCount_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define imexsessionapiGetRefCount(pResource) imexsessionapiGetRefCount_DISPATCH(pResource)
#define imexsessionapiAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define imexsessionapiAddAdditionalDependants(pClient, pResource, pReference) imexsessionapiAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NvBool imexsessionapiAccessCallback_DISPATCH(struct ImexSessionApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__(staticCast(pResource, RsResource), pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool imexsessionapiShareCallback_DISPATCH(struct ImexSessionApi *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__(staticCast(pResource, RsResource), pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS imexsessionapiGetMemInterMapParams_DISPATCH(struct ImexSessionApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__(staticCast(pRmResource, RmResource), pParams);
}

static inline NV_STATUS imexsessionapiCheckMemInterUnmap_DISPATCH(struct ImexSessionApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__(staticCast(pRmResource, RmResource), bSubdeviceHandleProvided);
}

static inline NV_STATUS imexsessionapiGetMemoryMappingDescriptor_DISPATCH(struct ImexSessionApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__(staticCast(pRmResource, RmResource), ppMemDesc);
}

static inline NV_STATUS imexsessionapiControlSerialization_Prologue_DISPATCH(struct ImexSessionApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void imexsessionapiControlSerialization_Epilogue_DISPATCH(struct ImexSessionApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS imexsessionapiControl_Prologue_DISPATCH(struct ImexSessionApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void imexsessionapiControl_Epilogue_DISPATCH(struct ImexSessionApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool imexsessionapiCanCopy_DISPATCH(struct ImexSessionApi *pResource) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__(staticCast(pResource, RsResource));
}

static inline NV_STATUS imexsessionapiIsDuplicate_DISPATCH(struct ImexSessionApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__(staticCast(pResource, RsResource), hMemory, pDuplicate);
}

static inline void imexsessionapiPreDestruct_DISPATCH(struct ImexSessionApi *pResource) {
    pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__(staticCast(pResource, RsResource));
}

static inline NV_STATUS imexsessionapiControl_DISPATCH(struct ImexSessionApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS imexsessionapiControlFilter_DISPATCH(struct ImexSessionApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS imexsessionapiMap_DISPATCH(struct ImexSessionApi *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__(staticCast(pResource, RsResource), pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS imexsessionapiUnmap_DISPATCH(struct ImexSessionApi *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__(staticCast(pResource, RsResource), pCallContext, pCpuMapping);
}

static inline NvBool imexsessionapiIsPartialUnmapSupported_DISPATCH(struct ImexSessionApi *pResource) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__(staticCast(pResource, RsResource));
}

static inline NV_STATUS imexsessionapiMapTo_DISPATCH(struct ImexSessionApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__(staticCast(pResource, RsResource), pParams);
}

static inline NV_STATUS imexsessionapiUnmapFrom_DISPATCH(struct ImexSessionApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__(staticCast(pResource, RsResource), pParams);
}

static inline NvU32 imexsessionapiGetRefCount_DISPATCH(struct ImexSessionApi *pResource) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__(staticCast(pResource, RsResource));
}

static inline void imexsessionapiAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct ImexSessionApi *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__(pClient, staticCast(pResource, RsResource), pReference);
}

// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
NV_STATUS imexsessionapiCtrlCmdGetFabricEvents_IMPL(struct ImexSessionApi *pImexSessionApi, NV00F1_CTRL_GET_FABRIC_EVENTS_PARAMS *pParams);

NV_STATUS imexsessionapiCtrlCmdFinishMemUnimport_IMPL(struct ImexSessionApi *pImexSessionApi, NV00F1_CTRL_FINISH_MEM_UNIMPORT_PARAMS *pParams);

NV_STATUS imexsessionapiCtrlCmdDisableImporters_IMPL(struct ImexSessionApi *pImexSessionApi, NV00F1_CTRL_DISABLE_IMPORTERS_PARAMS *pParams);

// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


// Returns true if at least one channel is RC'ed
NvBool rcAndDisableOutstandingClientsWithImportedMemory(OBJGPU *pGpu, NvU16 nodeId);

#endif // IMEX_SESSION_API_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_IMEX_SESSION_API_NVOC_H_
