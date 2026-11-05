
#ifndef _G_FM_SESSION_API_NVOC_H_
#define _G_FM_SESSION_API_NVOC_H_

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
#include "rmapi/event_api.h"
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


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__FmSessionApi;
struct NVOC_METADATA__RmResource;


struct FmSessionApi {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__FmSessionApi *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct RmResource __nvoc_base_RmResource;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^3
    struct RsResource *__nvoc_pbase_RsResource;    // res super^2
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^2
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super
    struct FmSessionApi *__nvoc_pbase_FmSessionApi;    // fmsessionapi

    // Data members
    NvU64 dupedCapDescriptor;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__FmSessionApi {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__RmResource metadata__RmResource;
};

#ifndef __nvoc_class_id_FmSessionApi
#define __nvoc_class_id_FmSessionApi 0xdfbd08u
typedef struct FmSessionApi FmSessionApi;
#endif /* __nvoc_class_id_FmSessionApi */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_FmSessionApi;

#define __staticCast_FmSessionApi(pThis) \
    ((pThis)->__nvoc_pbase_FmSessionApi)

#ifdef __nvoc_fm_session_api_h_disabled
#define __dynamicCast_FmSessionApi(pThis) ((FmSessionApi*) NULL)
#else //__nvoc_fm_session_api_h_disabled
#define __dynamicCast_FmSessionApi(pThis) \
    ((FmSessionApi*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(FmSessionApi)))
#endif //__nvoc_fm_session_api_h_disabled

NV_STATUS __nvoc_objCreateDynamic_FmSessionApi(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_FmSessionApi(FmSessionApi**, Dynamic*, NvU32, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __objCreate_FmSessionApi(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams) \
    __nvoc_objCreate_FmSessionApi((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pCallContext, pParams)


// Wrapper macros for implementation functions
NV_STATUS fmsessionapiConstruct_IMPL(struct FmSessionApi *pFmSessionApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __nvoc_fmsessionapiConstruct(pFmSessionApi, pCallContext, pParams) fmsessionapiConstruct_IMPL(pFmSessionApi, pCallContext, pParams)

void fmsessionapiDestruct_IMPL(struct FmSessionApi *pFmSessionApi);
#define __nvoc_fmsessionapiDestruct(pFmSessionApi) fmsessionapiDestruct_IMPL(pFmSessionApi)

NV_STATUS fmsessionapiCtrlCmdSetFmState_IMPL(struct FmSessionApi *pFmSessionApi);
#ifdef __nvoc_fm_session_api_h_disabled
static inline NV_STATUS fmsessionapiCtrlCmdSetFmState(struct FmSessionApi *pFmSessionApi) {
    NV_ASSERT_FAILED_PRECOMP("FmSessionApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_fm_session_api_h_disabled
#define fmsessionapiCtrlCmdSetFmState(pFmSessionApi) fmsessionapiCtrlCmdSetFmState_IMPL(pFmSessionApi)
#endif // __nvoc_fm_session_api_h_disabled

NV_STATUS fmsessionapiCtrlCmdClearFmState_IMPL(struct FmSessionApi *pFmSessionApi);
#ifdef __nvoc_fm_session_api_h_disabled
static inline NV_STATUS fmsessionapiCtrlCmdClearFmState(struct FmSessionApi *pFmSessionApi) {
    NV_ASSERT_FAILED_PRECOMP("FmSessionApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_fm_session_api_h_disabled
#define fmsessionapiCtrlCmdClearFmState(pFmSessionApi) fmsessionapiCtrlCmdClearFmState_IMPL(pFmSessionApi)
#endif // __nvoc_fm_session_api_h_disabled


// Wrapper macros for halified functions
#define fmsessionapiAccessCallback_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__
#define fmsessionapiAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) fmsessionapiAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define fmsessionapiShareCallback_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__
#define fmsessionapiShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) fmsessionapiShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define fmsessionapiGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define fmsessionapiGetMemInterMapParams(pRmResource, pParams) fmsessionapiGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define fmsessionapiCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define fmsessionapiCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) fmsessionapiCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define fmsessionapiGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define fmsessionapiGetMemoryMappingDescriptor(pRmResource, ppMemDesc) fmsessionapiGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define fmsessionapiControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__
#define fmsessionapiControlSerialization_Prologue(pResource, pCallContext, pParams) fmsessionapiControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define fmsessionapiControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__
#define fmsessionapiControlSerialization_Epilogue(pResource, pCallContext, pParams) fmsessionapiControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define fmsessionapiControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__
#define fmsessionapiControl_Prologue(pResource, pCallContext, pParams) fmsessionapiControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define fmsessionapiControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__
#define fmsessionapiControl_Epilogue(pResource, pCallContext, pParams) fmsessionapiControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define fmsessionapiCanCopy_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define fmsessionapiCanCopy(pResource) fmsessionapiCanCopy_DISPATCH(pResource)
#define fmsessionapiIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define fmsessionapiIsDuplicate(pResource, hMemory, pDuplicate) fmsessionapiIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define fmsessionapiPreDestruct_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define fmsessionapiPreDestruct(pResource) fmsessionapiPreDestruct_DISPATCH(pResource)
#define fmsessionapiControl_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__
#define fmsessionapiControl(pResource, pCallContext, pParams) fmsessionapiControl_DISPATCH(pResource, pCallContext, pParams)
#define fmsessionapiControlFilter_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define fmsessionapiControlFilter(pResource, pCallContext, pParams) fmsessionapiControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define fmsessionapiMap_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__
#define fmsessionapiMap(pResource, pCallContext, pParams, pCpuMapping) fmsessionapiMap_DISPATCH(pResource, pCallContext, pParams, pCpuMapping)
#define fmsessionapiUnmap_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__
#define fmsessionapiUnmap(pResource, pCallContext, pCpuMapping) fmsessionapiUnmap_DISPATCH(pResource, pCallContext, pCpuMapping)
#define fmsessionapiIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define fmsessionapiIsPartialUnmapSupported(pResource) fmsessionapiIsPartialUnmapSupported_DISPATCH(pResource)
#define fmsessionapiMapTo_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define fmsessionapiMapTo(pResource, pParams) fmsessionapiMapTo_DISPATCH(pResource, pParams)
#define fmsessionapiUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define fmsessionapiUnmapFrom(pResource, pParams) fmsessionapiUnmapFrom_DISPATCH(pResource, pParams)
#define fmsessionapiGetRefCount_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define fmsessionapiGetRefCount(pResource) fmsessionapiGetRefCount_DISPATCH(pResource)
#define fmsessionapiAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define fmsessionapiAddAdditionalDependants(pClient, pResource, pReference) fmsessionapiAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NvBool fmsessionapiAccessCallback_DISPATCH(struct FmSessionApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__(staticCast(pResource, RsResource), pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool fmsessionapiShareCallback_DISPATCH(struct FmSessionApi *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__(staticCast(pResource, RsResource), pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS fmsessionapiGetMemInterMapParams_DISPATCH(struct FmSessionApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__(staticCast(pRmResource, RmResource), pParams);
}

static inline NV_STATUS fmsessionapiCheckMemInterUnmap_DISPATCH(struct FmSessionApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__(staticCast(pRmResource, RmResource), bSubdeviceHandleProvided);
}

static inline NV_STATUS fmsessionapiGetMemoryMappingDescriptor_DISPATCH(struct FmSessionApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__(staticCast(pRmResource, RmResource), ppMemDesc);
}

static inline NV_STATUS fmsessionapiControlSerialization_Prologue_DISPATCH(struct FmSessionApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void fmsessionapiControlSerialization_Epilogue_DISPATCH(struct FmSessionApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS fmsessionapiControl_Prologue_DISPATCH(struct FmSessionApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void fmsessionapiControl_Epilogue_DISPATCH(struct FmSessionApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool fmsessionapiCanCopy_DISPATCH(struct FmSessionApi *pResource) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__(staticCast(pResource, RsResource));
}

static inline NV_STATUS fmsessionapiIsDuplicate_DISPATCH(struct FmSessionApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__(staticCast(pResource, RsResource), hMemory, pDuplicate);
}

static inline void fmsessionapiPreDestruct_DISPATCH(struct FmSessionApi *pResource) {
    pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__(staticCast(pResource, RsResource));
}

static inline NV_STATUS fmsessionapiControl_DISPATCH(struct FmSessionApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS fmsessionapiControlFilter_DISPATCH(struct FmSessionApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS fmsessionapiMap_DISPATCH(struct FmSessionApi *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__(staticCast(pResource, RsResource), pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS fmsessionapiUnmap_DISPATCH(struct FmSessionApi *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__(staticCast(pResource, RsResource), pCallContext, pCpuMapping);
}

static inline NvBool fmsessionapiIsPartialUnmapSupported_DISPATCH(struct FmSessionApi *pResource) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__(staticCast(pResource, RsResource));
}

static inline NV_STATUS fmsessionapiMapTo_DISPATCH(struct FmSessionApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__(staticCast(pResource, RsResource), pParams);
}

static inline NV_STATUS fmsessionapiUnmapFrom_DISPATCH(struct FmSessionApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__(staticCast(pResource, RsResource), pParams);
}

static inline NvU32 fmsessionapiGetRefCount_DISPATCH(struct FmSessionApi *pResource) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__(staticCast(pResource, RsResource));
}

static inline void fmsessionapiAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct FmSessionApi *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__(pClient, staticCast(pResource, RsResource), pReference);
}

// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
NV_STATUS fmsessionapiCtrlCmdSetFmState_IMPL(struct FmSessionApi *pFmSessionApi);

NV_STATUS fmsessionapiCtrlCmdClearFmState_IMPL(struct FmSessionApi *pFmSessionApi);

// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


#endif // FM_SESSION_API_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_FM_SESSION_API_NVOC_H_
