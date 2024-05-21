
#ifndef _G_GENERIC_ENGINE_NVOC_H_
#define _G_GENERIC_ENGINE_NVOC_H_
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
 * SPDX-FileCopyrightText: Copyright (c) 2016-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_generic_engine_nvoc.h"

#ifndef _GENERICENGINEAPI_H_
#define _GENERICENGINEAPI_H_

#include "gpu/gpu_resource.h"
#include "ctrl/ctrl90e6.h"
#include "ctrl/ctrl90e7.h"
#include "rmapi/resource.h" // for macro RMCTRL_EXPORT etc.

/*!
 * RM internal class providing a generic engine API to RM clients (e.g.:
 * GF100_SUBDEVICE_GRAPHICS and GF100_SUBDEVICE_FB). Classes are primarily used
 * for exposing BAR0 mappings and controls.
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_GENERIC_ENGINE_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct GenericEngineApi {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct GpuResource __nvoc_base_GpuResource;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^4
    struct RsResource *__nvoc_pbase_RsResource;    // res super^3
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^3
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super^2
    struct GpuResource *__nvoc_pbase_GpuResource;    // gpures super
    struct GenericEngineApi *__nvoc_pbase_GenericEngineApi;    // genapi

    // Vtable with 28 per-object function pointers
    NV_STATUS (*__genapiMap__)(struct GenericEngineApi * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual override (res) base (gpures)
    NV_STATUS (*__genapiGetMapAddrSpace__)(struct GenericEngineApi * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual override (gpures) base (gpures)
    NV_STATUS (*__genapiControl__)(struct GenericEngineApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual override (res) base (gpures)
    NV_STATUS (*__genapiCtrlCmdMasterGetErrorIntrOffsetMask__)(struct GenericEngineApi * /*this*/, NV90E6_CTRL_MASTER_GET_ERROR_INTR_OFFSET_MASK_PARAMS *);  // exported (id=0x90e60101)
    NV_STATUS (*__genapiCtrlCmdMasterGetVirtualFunctionErrorContIntrMask__)(struct GenericEngineApi * /*this*/, NV90E6_CTRL_MASTER_GET_VIRTUAL_FUNCTION_ERROR_CONT_INTR_MASK_PARAMS *);  // exported (id=0x90e60102)
    NV_STATUS (*__genapiCtrlCmdBBXGetLastFlushTime__)(struct GenericEngineApi * /*this*/, NV90E7_CTRL_BBX_GET_LAST_FLUSH_TIME_PARAMS *);  // exported (id=0x90e70113)
    NV_STATUS (*__genapiUnmap__)(struct GenericEngineApi * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NvBool (*__genapiShareCallback__)(struct GenericEngineApi * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__genapiGetRegBaseOffsetAndSize__)(struct GenericEngineApi * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__genapiInternalControlForward__)(struct GenericEngineApi * /*this*/, NvU32, void *, NvU32);  // virtual inherited (gpures) base (gpures)
    NvHandle (*__genapiGetInternalObjectHandle__)(struct GenericEngineApi * /*this*/);  // virtual inherited (gpures) base (gpures)
    NvBool (*__genapiAccessCallback__)(struct GenericEngineApi * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__genapiGetMemInterMapParams__)(struct GenericEngineApi * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__genapiCheckMemInterUnmap__)(struct GenericEngineApi * /*this*/, NvBool);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__genapiGetMemoryMappingDescriptor__)(struct GenericEngineApi * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__genapiControlSerialization_Prologue__)(struct GenericEngineApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__genapiControlSerialization_Epilogue__)(struct GenericEngineApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__genapiControl_Prologue__)(struct GenericEngineApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__genapiControl_Epilogue__)(struct GenericEngineApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NvBool (*__genapiCanCopy__)(struct GenericEngineApi * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__genapiIsDuplicate__)(struct GenericEngineApi * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (gpures)
    void (*__genapiPreDestruct__)(struct GenericEngineApi * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__genapiControlFilter__)(struct GenericEngineApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (gpures)
    NvBool (*__genapiIsPartialUnmapSupported__)(struct GenericEngineApi * /*this*/);  // inline virtual inherited (res) base (gpures) body
    NV_STATUS (*__genapiMapTo__)(struct GenericEngineApi * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__genapiUnmapFrom__)(struct GenericEngineApi * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (gpures)
    NvU32 (*__genapiGetRefCount__)(struct GenericEngineApi * /*this*/);  // virtual inherited (res) base (gpures)
    void (*__genapiAddAdditionalDependants__)(struct RsClient *, struct GenericEngineApi * /*this*/, RsResourceRef *);  // virtual inherited (res) base (gpures)
};

#ifndef __NVOC_CLASS_GenericEngineApi_TYPEDEF__
#define __NVOC_CLASS_GenericEngineApi_TYPEDEF__
typedef struct GenericEngineApi GenericEngineApi;
#endif /* __NVOC_CLASS_GenericEngineApi_TYPEDEF__ */

#ifndef __nvoc_class_id_GenericEngineApi
#define __nvoc_class_id_GenericEngineApi 0x4bc329
#endif /* __nvoc_class_id_GenericEngineApi */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GenericEngineApi;

#define __staticCast_GenericEngineApi(pThis) \
    ((pThis)->__nvoc_pbase_GenericEngineApi)

#ifdef __nvoc_generic_engine_h_disabled
#define __dynamicCast_GenericEngineApi(pThis) ((GenericEngineApi*)NULL)
#else //__nvoc_generic_engine_h_disabled
#define __dynamicCast_GenericEngineApi(pThis) \
    ((GenericEngineApi*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(GenericEngineApi)))
#endif //__nvoc_generic_engine_h_disabled

NV_STATUS __nvoc_objCreateDynamic_GenericEngineApi(GenericEngineApi**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_GenericEngineApi(GenericEngineApi**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_GenericEngineApi(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_GenericEngineApi((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define genapiMap_FNPTR(pGenericEngineApi) pGenericEngineApi->__genapiMap__
#define genapiMap(pGenericEngineApi, pCallContext, pParams, pCpuMapping) genapiMap_DISPATCH(pGenericEngineApi, pCallContext, pParams, pCpuMapping)
#define genapiGetMapAddrSpace_FNPTR(pGenericEngineApi) pGenericEngineApi->__genapiGetMapAddrSpace__
#define genapiGetMapAddrSpace(pGenericEngineApi, pCallContext, mapFlags, pAddrSpace) genapiGetMapAddrSpace_DISPATCH(pGenericEngineApi, pCallContext, mapFlags, pAddrSpace)
#define genapiControl_FNPTR(pGenericEngineApi) pGenericEngineApi->__genapiControl__
#define genapiControl(pGenericEngineApi, pCallContext, pParams) genapiControl_DISPATCH(pGenericEngineApi, pCallContext, pParams)
#define genapiCtrlCmdMasterGetErrorIntrOffsetMask_FNPTR(pGenericEngineApi) pGenericEngineApi->__genapiCtrlCmdMasterGetErrorIntrOffsetMask__
#define genapiCtrlCmdMasterGetErrorIntrOffsetMask(pGenericEngineApi, pParams) genapiCtrlCmdMasterGetErrorIntrOffsetMask_DISPATCH(pGenericEngineApi, pParams)
#define genapiCtrlCmdMasterGetVirtualFunctionErrorContIntrMask_FNPTR(pGenericEngineApi) pGenericEngineApi->__genapiCtrlCmdMasterGetVirtualFunctionErrorContIntrMask__
#define genapiCtrlCmdMasterGetVirtualFunctionErrorContIntrMask(pGenericEngineApi, pParams) genapiCtrlCmdMasterGetVirtualFunctionErrorContIntrMask_DISPATCH(pGenericEngineApi, pParams)
#define genapiCtrlCmdBBXGetLastFlushTime_FNPTR(pGenericEngineApi) pGenericEngineApi->__genapiCtrlCmdBBXGetLastFlushTime__
#define genapiCtrlCmdBBXGetLastFlushTime(pGenericEngineApi, pParams) genapiCtrlCmdBBXGetLastFlushTime_DISPATCH(pGenericEngineApi, pParams)
#define genapiUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresUnmap__
#define genapiUnmap(pGpuResource, pCallContext, pCpuMapping) genapiUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define genapiShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresShareCallback__
#define genapiShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) genapiShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define genapiGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetRegBaseOffsetAndSize__
#define genapiGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) genapiGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define genapiInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresInternalControlForward__
#define genapiInternalControlForward(pGpuResource, command, pParams, size) genapiInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define genapiGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetInternalObjectHandle__
#define genapiGetInternalObjectHandle(pGpuResource) genapiGetInternalObjectHandle_DISPATCH(pGpuResource)
#define genapiAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresAccessCallback__
#define genapiAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) genapiAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define genapiGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemInterMapParams__
#define genapiGetMemInterMapParams(pRmResource, pParams) genapiGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define genapiCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresCheckMemInterUnmap__
#define genapiCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) genapiCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define genapiGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemoryMappingDescriptor__
#define genapiGetMemoryMappingDescriptor(pRmResource, ppMemDesc) genapiGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define genapiControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Prologue__
#define genapiControlSerialization_Prologue(pResource, pCallContext, pParams) genapiControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define genapiControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Epilogue__
#define genapiControlSerialization_Epilogue(pResource, pCallContext, pParams) genapiControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define genapiControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Prologue__
#define genapiControl_Prologue(pResource, pCallContext, pParams) genapiControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define genapiControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Epilogue__
#define genapiControl_Epilogue(pResource, pCallContext, pParams) genapiControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define genapiCanCopy_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__
#define genapiCanCopy(pResource) genapiCanCopy_DISPATCH(pResource)
#define genapiIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsDuplicate__
#define genapiIsDuplicate(pResource, hMemory, pDuplicate) genapiIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define genapiPreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resPreDestruct__
#define genapiPreDestruct(pResource) genapiPreDestruct_DISPATCH(pResource)
#define genapiControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resControlFilter__
#define genapiControlFilter(pResource, pCallContext, pParams) genapiControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define genapiIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsPartialUnmapSupported__
#define genapiIsPartialUnmapSupported(pResource) genapiIsPartialUnmapSupported_DISPATCH(pResource)
#define genapiMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resMapTo__
#define genapiMapTo(pResource, pParams) genapiMapTo_DISPATCH(pResource, pParams)
#define genapiUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmapFrom__
#define genapiUnmapFrom(pResource, pParams) genapiUnmapFrom_DISPATCH(pResource, pParams)
#define genapiGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resGetRefCount__
#define genapiGetRefCount(pResource) genapiGetRefCount_DISPATCH(pResource)
#define genapiAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resAddAdditionalDependants__
#define genapiAddAdditionalDependants(pClient, pResource, pReference) genapiAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NV_STATUS genapiMap_DISPATCH(struct GenericEngineApi *pGenericEngineApi, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGenericEngineApi->__genapiMap__(pGenericEngineApi, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS genapiGetMapAddrSpace_DISPATCH(struct GenericEngineApi *pGenericEngineApi, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGenericEngineApi->__genapiGetMapAddrSpace__(pGenericEngineApi, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS genapiControl_DISPATCH(struct GenericEngineApi *pGenericEngineApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGenericEngineApi->__genapiControl__(pGenericEngineApi, pCallContext, pParams);
}

static inline NV_STATUS genapiCtrlCmdMasterGetErrorIntrOffsetMask_DISPATCH(struct GenericEngineApi *pGenericEngineApi, NV90E6_CTRL_MASTER_GET_ERROR_INTR_OFFSET_MASK_PARAMS *pParams) {
    return pGenericEngineApi->__genapiCtrlCmdMasterGetErrorIntrOffsetMask__(pGenericEngineApi, pParams);
}

static inline NV_STATUS genapiCtrlCmdMasterGetVirtualFunctionErrorContIntrMask_DISPATCH(struct GenericEngineApi *pGenericEngineApi, NV90E6_CTRL_MASTER_GET_VIRTUAL_FUNCTION_ERROR_CONT_INTR_MASK_PARAMS *pParams) {
    return pGenericEngineApi->__genapiCtrlCmdMasterGetVirtualFunctionErrorContIntrMask__(pGenericEngineApi, pParams);
}

static inline NV_STATUS genapiCtrlCmdBBXGetLastFlushTime_DISPATCH(struct GenericEngineApi *pGenericEngineApi, NV90E7_CTRL_BBX_GET_LAST_FLUSH_TIME_PARAMS *pParams) {
    return pGenericEngineApi->__genapiCtrlCmdBBXGetLastFlushTime__(pGenericEngineApi, pParams);
}

static inline NV_STATUS genapiUnmap_DISPATCH(struct GenericEngineApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__genapiUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NvBool genapiShareCallback_DISPATCH(struct GenericEngineApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__genapiShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS genapiGetRegBaseOffsetAndSize_DISPATCH(struct GenericEngineApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__genapiGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS genapiInternalControlForward_DISPATCH(struct GenericEngineApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__genapiInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NvHandle genapiGetInternalObjectHandle_DISPATCH(struct GenericEngineApi *pGpuResource) {
    return pGpuResource->__genapiGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool genapiAccessCallback_DISPATCH(struct GenericEngineApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__genapiAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS genapiGetMemInterMapParams_DISPATCH(struct GenericEngineApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__genapiGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS genapiCheckMemInterUnmap_DISPATCH(struct GenericEngineApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__genapiCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS genapiGetMemoryMappingDescriptor_DISPATCH(struct GenericEngineApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__genapiGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS genapiControlSerialization_Prologue_DISPATCH(struct GenericEngineApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__genapiControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void genapiControlSerialization_Epilogue_DISPATCH(struct GenericEngineApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__genapiControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS genapiControl_Prologue_DISPATCH(struct GenericEngineApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__genapiControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void genapiControl_Epilogue_DISPATCH(struct GenericEngineApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__genapiControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool genapiCanCopy_DISPATCH(struct GenericEngineApi *pResource) {
    return pResource->__genapiCanCopy__(pResource);
}

static inline NV_STATUS genapiIsDuplicate_DISPATCH(struct GenericEngineApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__genapiIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void genapiPreDestruct_DISPATCH(struct GenericEngineApi *pResource) {
    pResource->__genapiPreDestruct__(pResource);
}

static inline NV_STATUS genapiControlFilter_DISPATCH(struct GenericEngineApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__genapiControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool genapiIsPartialUnmapSupported_DISPATCH(struct GenericEngineApi *pResource) {
    return pResource->__genapiIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS genapiMapTo_DISPATCH(struct GenericEngineApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__genapiMapTo__(pResource, pParams);
}

static inline NV_STATUS genapiUnmapFrom_DISPATCH(struct GenericEngineApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__genapiUnmapFrom__(pResource, pParams);
}

static inline NvU32 genapiGetRefCount_DISPATCH(struct GenericEngineApi *pResource) {
    return pResource->__genapiGetRefCount__(pResource);
}

static inline void genapiAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct GenericEngineApi *pResource, RsResourceRef *pReference) {
    pResource->__genapiAddAdditionalDependants__(pClient, pResource, pReference);
}

NV_STATUS genapiMap_IMPL(struct GenericEngineApi *pGenericEngineApi, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);

NV_STATUS genapiGetMapAddrSpace_IMPL(struct GenericEngineApi *pGenericEngineApi, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);

NV_STATUS genapiControl_IMPL(struct GenericEngineApi *pGenericEngineApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);

NV_STATUS genapiCtrlCmdMasterGetErrorIntrOffsetMask_IMPL(struct GenericEngineApi *pGenericEngineApi, NV90E6_CTRL_MASTER_GET_ERROR_INTR_OFFSET_MASK_PARAMS *pParams);

NV_STATUS genapiCtrlCmdMasterGetVirtualFunctionErrorContIntrMask_IMPL(struct GenericEngineApi *pGenericEngineApi, NV90E6_CTRL_MASTER_GET_VIRTUAL_FUNCTION_ERROR_CONT_INTR_MASK_PARAMS *pParams);

NV_STATUS genapiCtrlCmdBBXGetLastFlushTime_IMPL(struct GenericEngineApi *pGenericEngineApi, NV90E7_CTRL_BBX_GET_LAST_FLUSH_TIME_PARAMS *pParams);

NV_STATUS genapiConstruct_IMPL(struct GenericEngineApi *arg_pGenericEngineApi, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_genapiConstruct(arg_pGenericEngineApi, arg_pCallContext, arg_pParams) genapiConstruct_IMPL(arg_pGenericEngineApi, arg_pCallContext, arg_pParams)
void genapiDestruct_IMPL(struct GenericEngineApi *pGenericEngineApi);

#define __nvoc_genapiDestruct(pGenericEngineApi) genapiDestruct_IMPL(pGenericEngineApi)
#undef PRIVATE_FIELD


#endif // _GENERICENGINEAPI_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_GENERIC_ENGINE_NVOC_H_
