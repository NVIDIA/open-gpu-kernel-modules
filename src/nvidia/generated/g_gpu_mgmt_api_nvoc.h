
#ifndef _G_GPU_MGMT_API_NVOC_H_
#define _G_GPU_MGMT_API_NVOC_H_

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
#pragma once
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


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_GPU_MGMT_API_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__GpuManagementApi;
struct NVOC_METADATA__RmResource;
struct NVOC_VTABLE__GpuManagementApi;


struct GpuManagementApi {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__GpuManagementApi *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct RmResource __nvoc_base_RmResource;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^3
    struct RsResource *__nvoc_pbase_RsResource;    // res super^2
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^2
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super
    struct GpuManagementApi *__nvoc_pbase_GpuManagementApi;    // gpumgmtapi

    // Vtable with 1 per-object function pointer
    NV_STATUS (*__gpumgmtapiCtrlCmdSetShutdownState__)(struct GpuManagementApi * /*this*/, NV0020_CTRL_GPU_MGMT_SET_SHUTDOWN_STATE_PARAMS *);  // exported (id=0x200101)
};


// Vtable with 21 per-class function pointers
struct NVOC_VTABLE__GpuManagementApi {
    NvBool (*__gpumgmtapiAccessCallback__)(struct GpuManagementApi * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (rmres)
    NvBool (*__gpumgmtapiShareCallback__)(struct GpuManagementApi * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__gpumgmtapiGetMemInterMapParams__)(struct GpuManagementApi * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__gpumgmtapiCheckMemInterUnmap__)(struct GpuManagementApi * /*this*/, NvBool);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__gpumgmtapiGetMemoryMappingDescriptor__)(struct GpuManagementApi * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__gpumgmtapiControlSerialization_Prologue__)(struct GpuManagementApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (rmres)
    void (*__gpumgmtapiControlSerialization_Epilogue__)(struct GpuManagementApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__gpumgmtapiControl_Prologue__)(struct GpuManagementApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (rmres)
    void (*__gpumgmtapiControl_Epilogue__)(struct GpuManagementApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (rmres)
    NvBool (*__gpumgmtapiCanCopy__)(struct GpuManagementApi * /*this*/);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__gpumgmtapiIsDuplicate__)(struct GpuManagementApi * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (rmres)
    void (*__gpumgmtapiPreDestruct__)(struct GpuManagementApi * /*this*/);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__gpumgmtapiControl__)(struct GpuManagementApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__gpumgmtapiControlFilter__)(struct GpuManagementApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__gpumgmtapiMap__)(struct GpuManagementApi * /*this*/, struct CALL_CONTEXT *, RS_CPU_MAP_PARAMS *, RsCpuMapping *);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__gpumgmtapiUnmap__)(struct GpuManagementApi * /*this*/, struct CALL_CONTEXT *, RsCpuMapping *);  // virtual inherited (res) base (rmres)
    NvBool (*__gpumgmtapiIsPartialUnmapSupported__)(struct GpuManagementApi * /*this*/);  // inline virtual inherited (res) base (rmres) body
    NV_STATUS (*__gpumgmtapiMapTo__)(struct GpuManagementApi * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__gpumgmtapiUnmapFrom__)(struct GpuManagementApi * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (rmres)
    NvU32 (*__gpumgmtapiGetRefCount__)(struct GpuManagementApi * /*this*/);  // virtual inherited (res) base (rmres)
    void (*__gpumgmtapiAddAdditionalDependants__)(struct RsClient *, struct GpuManagementApi * /*this*/, RsResourceRef *);  // virtual inherited (res) base (rmres)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__GpuManagementApi {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__RmResource metadata__RmResource;
    const struct NVOC_VTABLE__GpuManagementApi vtable;
};

#ifndef __NVOC_CLASS_GpuManagementApi_TYPEDEF__
#define __NVOC_CLASS_GpuManagementApi_TYPEDEF__
typedef struct GpuManagementApi GpuManagementApi;
#endif /* __NVOC_CLASS_GpuManagementApi_TYPEDEF__ */

#ifndef __nvoc_class_id_GpuManagementApi
#define __nvoc_class_id_GpuManagementApi 0x376305
#endif /* __nvoc_class_id_GpuManagementApi */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuManagementApi;

#define __staticCast_GpuManagementApi(pThis) \
    ((pThis)->__nvoc_pbase_GpuManagementApi)

#ifdef __nvoc_gpu_mgmt_api_h_disabled
#define __dynamicCast_GpuManagementApi(pThis) ((GpuManagementApi*) NULL)
#else //__nvoc_gpu_mgmt_api_h_disabled
#define __dynamicCast_GpuManagementApi(pThis) \
    ((GpuManagementApi*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(GpuManagementApi)))
#endif //__nvoc_gpu_mgmt_api_h_disabled

NV_STATUS __nvoc_objCreateDynamic_GpuManagementApi(GpuManagementApi**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_GpuManagementApi(GpuManagementApi**, Dynamic*, NvU32, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __objCreate_GpuManagementApi(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_GpuManagementApi((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define gpumgmtapiCtrlCmdSetShutdownState_FNPTR(pGpuMgmt) pGpuMgmt->__gpumgmtapiCtrlCmdSetShutdownState__
#define gpumgmtapiCtrlCmdSetShutdownState(pGpuMgmt, pParams) gpumgmtapiCtrlCmdSetShutdownState_DISPATCH(pGpuMgmt, pParams)
#define gpumgmtapiAccessCallback_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresAccessCallback__
#define gpumgmtapiAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) gpumgmtapiAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define gpumgmtapiShareCallback_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresShareCallback__
#define gpumgmtapiShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) gpumgmtapiShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define gpumgmtapiGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define gpumgmtapiGetMemInterMapParams(pRmResource, pParams) gpumgmtapiGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define gpumgmtapiCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define gpumgmtapiCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) gpumgmtapiCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define gpumgmtapiGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define gpumgmtapiGetMemoryMappingDescriptor(pRmResource, ppMemDesc) gpumgmtapiGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define gpumgmtapiControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Prologue__
#define gpumgmtapiControlSerialization_Prologue(pResource, pCallContext, pParams) gpumgmtapiControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define gpumgmtapiControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Epilogue__
#define gpumgmtapiControlSerialization_Epilogue(pResource, pCallContext, pParams) gpumgmtapiControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define gpumgmtapiControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Prologue__
#define gpumgmtapiControl_Prologue(pResource, pCallContext, pParams) gpumgmtapiControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define gpumgmtapiControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Epilogue__
#define gpumgmtapiControl_Epilogue(pResource, pCallContext, pParams) gpumgmtapiControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define gpumgmtapiCanCopy_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define gpumgmtapiCanCopy(pResource) gpumgmtapiCanCopy_DISPATCH(pResource)
#define gpumgmtapiIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define gpumgmtapiIsDuplicate(pResource, hMemory, pDuplicate) gpumgmtapiIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define gpumgmtapiPreDestruct_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define gpumgmtapiPreDestruct(pResource) gpumgmtapiPreDestruct_DISPATCH(pResource)
#define gpumgmtapiControl_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__
#define gpumgmtapiControl(pResource, pCallContext, pParams) gpumgmtapiControl_DISPATCH(pResource, pCallContext, pParams)
#define gpumgmtapiControlFilter_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define gpumgmtapiControlFilter(pResource, pCallContext, pParams) gpumgmtapiControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define gpumgmtapiMap_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__
#define gpumgmtapiMap(pResource, pCallContext, pParams, pCpuMapping) gpumgmtapiMap_DISPATCH(pResource, pCallContext, pParams, pCpuMapping)
#define gpumgmtapiUnmap_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__
#define gpumgmtapiUnmap(pResource, pCallContext, pCpuMapping) gpumgmtapiUnmap_DISPATCH(pResource, pCallContext, pCpuMapping)
#define gpumgmtapiIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define gpumgmtapiIsPartialUnmapSupported(pResource) gpumgmtapiIsPartialUnmapSupported_DISPATCH(pResource)
#define gpumgmtapiMapTo_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define gpumgmtapiMapTo(pResource, pParams) gpumgmtapiMapTo_DISPATCH(pResource, pParams)
#define gpumgmtapiUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define gpumgmtapiUnmapFrom(pResource, pParams) gpumgmtapiUnmapFrom_DISPATCH(pResource, pParams)
#define gpumgmtapiGetRefCount_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define gpumgmtapiGetRefCount(pResource) gpumgmtapiGetRefCount_DISPATCH(pResource)
#define gpumgmtapiAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define gpumgmtapiAddAdditionalDependants(pClient, pResource, pReference) gpumgmtapiAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NV_STATUS gpumgmtapiCtrlCmdSetShutdownState_DISPATCH(struct GpuManagementApi *pGpuMgmt, NV0020_CTRL_GPU_MGMT_SET_SHUTDOWN_STATE_PARAMS *pParams) {
    return pGpuMgmt->__gpumgmtapiCtrlCmdSetShutdownState__(pGpuMgmt, pParams);
}

static inline NvBool gpumgmtapiAccessCallback_DISPATCH(struct GpuManagementApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_metadata_ptr->vtable.__gpumgmtapiAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool gpumgmtapiShareCallback_DISPATCH(struct GpuManagementApi *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__nvoc_metadata_ptr->vtable.__gpumgmtapiShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS gpumgmtapiGetMemInterMapParams_DISPATCH(struct GpuManagementApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__gpumgmtapiGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS gpumgmtapiCheckMemInterUnmap_DISPATCH(struct GpuManagementApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__gpumgmtapiCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS gpumgmtapiGetMemoryMappingDescriptor_DISPATCH(struct GpuManagementApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__gpumgmtapiGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS gpumgmtapiControlSerialization_Prologue_DISPATCH(struct GpuManagementApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__gpumgmtapiControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void gpumgmtapiControlSerialization_Epilogue_DISPATCH(struct GpuManagementApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__gpumgmtapiControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS gpumgmtapiControl_Prologue_DISPATCH(struct GpuManagementApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__gpumgmtapiControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void gpumgmtapiControl_Epilogue_DISPATCH(struct GpuManagementApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__gpumgmtapiControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool gpumgmtapiCanCopy_DISPATCH(struct GpuManagementApi *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__gpumgmtapiCanCopy__(pResource);
}

static inline NV_STATUS gpumgmtapiIsDuplicate_DISPATCH(struct GpuManagementApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_metadata_ptr->vtable.__gpumgmtapiIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void gpumgmtapiPreDestruct_DISPATCH(struct GpuManagementApi *pResource) {
    pResource->__nvoc_metadata_ptr->vtable.__gpumgmtapiPreDestruct__(pResource);
}

static inline NV_STATUS gpumgmtapiControl_DISPATCH(struct GpuManagementApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__gpumgmtapiControl__(pResource, pCallContext, pParams);
}

static inline NV_STATUS gpumgmtapiControlFilter_DISPATCH(struct GpuManagementApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__gpumgmtapiControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS gpumgmtapiMap_DISPATCH(struct GpuManagementApi *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pResource->__nvoc_metadata_ptr->vtable.__gpumgmtapiMap__(pResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS gpumgmtapiUnmap_DISPATCH(struct GpuManagementApi *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pResource->__nvoc_metadata_ptr->vtable.__gpumgmtapiUnmap__(pResource, pCallContext, pCpuMapping);
}

static inline NvBool gpumgmtapiIsPartialUnmapSupported_DISPATCH(struct GpuManagementApi *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__gpumgmtapiIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS gpumgmtapiMapTo_DISPATCH(struct GpuManagementApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__gpumgmtapiMapTo__(pResource, pParams);
}

static inline NV_STATUS gpumgmtapiUnmapFrom_DISPATCH(struct GpuManagementApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__gpumgmtapiUnmapFrom__(pResource, pParams);
}

static inline NvU32 gpumgmtapiGetRefCount_DISPATCH(struct GpuManagementApi *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__gpumgmtapiGetRefCount__(pResource);
}

static inline void gpumgmtapiAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct GpuManagementApi *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_metadata_ptr->vtable.__gpumgmtapiAddAdditionalDependants__(pClient, pResource, pReference);
}

NV_STATUS gpumgmtapiCtrlCmdSetShutdownState_IMPL(struct GpuManagementApi *pGpuMgmt, NV0020_CTRL_GPU_MGMT_SET_SHUTDOWN_STATE_PARAMS *pParams);

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
