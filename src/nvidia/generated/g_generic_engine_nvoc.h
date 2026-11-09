
#ifndef _G_GENERIC_ENGINE_NVOC_H_
#define _G_GENERIC_ENGINE_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2016-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__GenericEngineApi;
struct NVOC_METADATA__GpuResource;


struct GenericEngineApi {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__GenericEngineApi *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuResource __nvoc_base_GpuResource;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^4
    struct RsResource *__nvoc_pbase_RsResource;    // res super^3
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^3
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super^2
    struct GpuResource *__nvoc_pbase_GpuResource;    // gpures super
    struct GenericEngineApi *__nvoc_pbase_GenericEngineApi;    // genapi
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__GenericEngineApi {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResource metadata__GpuResource;
};

#ifndef __nvoc_class_id_GenericEngineApi
#define __nvoc_class_id_GenericEngineApi 0x4bc329u
typedef struct GenericEngineApi GenericEngineApi;
#endif /* __nvoc_class_id_GenericEngineApi */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GenericEngineApi;

#define __staticCast_GenericEngineApi(pThis) \
    ((pThis)->__nvoc_pbase_GenericEngineApi)

#ifdef __nvoc_generic_engine_h_disabled
#define __dynamicCast_GenericEngineApi(pThis) ((GenericEngineApi*) NULL)
#else //__nvoc_generic_engine_h_disabled
#define __dynamicCast_GenericEngineApi(pThis) \
    ((GenericEngineApi*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(GenericEngineApi)))
#endif //__nvoc_generic_engine_h_disabled

NV_STATUS __nvoc_objCreateDynamic_GenericEngineApi(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_GenericEngineApi(GenericEngineApi**, Dynamic*, NvU32, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __objCreate_GenericEngineApi(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams) \
    __nvoc_objCreate_GenericEngineApi((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pCallContext, pParams)


// Wrapper macros for implementation functions
NV_STATUS genapiConstruct_IMPL(struct GenericEngineApi *pGenericEngineApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __nvoc_genapiConstruct(pGenericEngineApi, pCallContext, pParams) genapiConstruct_IMPL(pGenericEngineApi, pCallContext, pParams)

void genapiDestruct_IMPL(struct GenericEngineApi *pGenericEngineApi);
#define __nvoc_genapiDestruct(pGenericEngineApi) genapiDestruct_IMPL(pGenericEngineApi)

NV_STATUS genapiCtrlCmdMasterGetErrorIntrOffsetMask_IMPL(struct GenericEngineApi *pGenericEngineApi, NV90E6_CTRL_MASTER_GET_ERROR_INTR_OFFSET_MASK_PARAMS *pParams);
#ifdef __nvoc_generic_engine_h_disabled
static inline NV_STATUS genapiCtrlCmdMasterGetErrorIntrOffsetMask(struct GenericEngineApi *pGenericEngineApi, NV90E6_CTRL_MASTER_GET_ERROR_INTR_OFFSET_MASK_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("GenericEngineApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_generic_engine_h_disabled
#define genapiCtrlCmdMasterGetErrorIntrOffsetMask(pGenericEngineApi, pParams) genapiCtrlCmdMasterGetErrorIntrOffsetMask_IMPL(pGenericEngineApi, pParams)
#endif // __nvoc_generic_engine_h_disabled

NV_STATUS genapiCtrlCmdMasterGetVirtualFunctionErrorContIntrMask_IMPL(struct GenericEngineApi *pGenericEngineApi, NV90E6_CTRL_MASTER_GET_VIRTUAL_FUNCTION_ERROR_CONT_INTR_MASK_PARAMS *pParams);
#ifdef __nvoc_generic_engine_h_disabled
static inline NV_STATUS genapiCtrlCmdMasterGetVirtualFunctionErrorContIntrMask(struct GenericEngineApi *pGenericEngineApi, NV90E6_CTRL_MASTER_GET_VIRTUAL_FUNCTION_ERROR_CONT_INTR_MASK_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("GenericEngineApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_generic_engine_h_disabled
#define genapiCtrlCmdMasterGetVirtualFunctionErrorContIntrMask(pGenericEngineApi, pParams) genapiCtrlCmdMasterGetVirtualFunctionErrorContIntrMask_IMPL(pGenericEngineApi, pParams)
#endif // __nvoc_generic_engine_h_disabled

NV_STATUS genapiCtrlCmdBBXGetLastFlushTime_IMPL(struct GenericEngineApi *pGenericEngineApi, NV90E7_CTRL_BBX_GET_LAST_FLUSH_TIME_PARAMS *pParams);
#ifdef __nvoc_generic_engine_h_disabled
static inline NV_STATUS genapiCtrlCmdBBXGetLastFlushTime(struct GenericEngineApi *pGenericEngineApi, NV90E7_CTRL_BBX_GET_LAST_FLUSH_TIME_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("GenericEngineApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_generic_engine_h_disabled
#define genapiCtrlCmdBBXGetLastFlushTime(pGenericEngineApi, pParams) genapiCtrlCmdBBXGetLastFlushTime_IMPL(pGenericEngineApi, pParams)
#endif // __nvoc_generic_engine_h_disabled

NV_STATUS genapiCtrlCmdBBXIsNVMFlushEnabled_IMPL(struct GenericEngineApi *pGenericEngineApi, NV90E7_CTRL_BBX_IS_NVM_FLUSH_ENABLED_PARAMS *pParams);
#ifdef __nvoc_generic_engine_h_disabled
static inline NV_STATUS genapiCtrlCmdBBXIsNVMFlushEnabled(struct GenericEngineApi *pGenericEngineApi, NV90E7_CTRL_BBX_IS_NVM_FLUSH_ENABLED_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("GenericEngineApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_generic_engine_h_disabled
#define genapiCtrlCmdBBXIsNVMFlushEnabled(pGenericEngineApi, pParams) genapiCtrlCmdBBXIsNVMFlushEnabled_IMPL(pGenericEngineApi, pParams)
#endif // __nvoc_generic_engine_h_disabled

NV_STATUS genapiCtrlCmdBBXGetTimeData_IMPL(struct GenericEngineApi *pGenericEngineApi, NV90E7_CTRL_BBX_GET_TIME_DATA_PARAMS *pParams);
#ifdef __nvoc_generic_engine_h_disabled
static inline NV_STATUS genapiCtrlCmdBBXGetTimeData(struct GenericEngineApi *pGenericEngineApi, NV90E7_CTRL_BBX_GET_TIME_DATA_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("GenericEngineApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_generic_engine_h_disabled
#define genapiCtrlCmdBBXGetTimeData(pGenericEngineApi, pParams) genapiCtrlCmdBBXGetTimeData_IMPL(pGenericEngineApi, pParams)
#endif // __nvoc_generic_engine_h_disabled


// Wrapper macros for halified functions
#define genapiMap_FNPTR(pGenericEngineApi) pGenericEngineApi->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__
#define genapiMap(pGenericEngineApi, pCallContext, pParams, pCpuMapping) genapiMap_DISPATCH(pGenericEngineApi, pCallContext, pParams, pCpuMapping)
#define genapiGetMapAddrSpace_FNPTR(pGenericEngineApi) pGenericEngineApi->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__
#define genapiGetMapAddrSpace(pGenericEngineApi, pCallContext, mapFlags, pAddrSpace) genapiGetMapAddrSpace_DISPATCH(pGenericEngineApi, pCallContext, mapFlags, pAddrSpace)
#define genapiControl_FNPTR(pGenericEngineApi) pGenericEngineApi->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__
#define genapiControl(pGenericEngineApi, pCallContext, pParams) genapiControl_DISPATCH(pGenericEngineApi, pCallContext, pParams)
#define genapiUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__
#define genapiUnmap(pGpuResource, pCallContext, pCpuMapping) genapiUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define genapiShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__
#define genapiShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) genapiShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define genapiGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__
#define genapiGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) genapiGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define genapiInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__
#define genapiInternalControlForward(pGpuResource, command, pParams, size) genapiInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define genapiGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__
#define genapiGetInternalObjectHandle(pGpuResource) genapiGetInternalObjectHandle_DISPATCH(pGpuResource)
#define genapiAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__
#define genapiAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) genapiAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define genapiGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define genapiGetMemInterMapParams(pRmResource, pParams) genapiGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define genapiCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define genapiCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) genapiCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define genapiGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define genapiGetMemoryMappingDescriptor(pRmResource, ppMemDesc) genapiGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define genapiControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__
#define genapiControlSerialization_Prologue(pResource, pCallContext, pParams) genapiControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define genapiControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__
#define genapiControlSerialization_Epilogue(pResource, pCallContext, pParams) genapiControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define genapiControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__
#define genapiControl_Prologue(pResource, pCallContext, pParams) genapiControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define genapiControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__
#define genapiControl_Epilogue(pResource, pCallContext, pParams) genapiControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define genapiCanCopy_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define genapiCanCopy(pResource) genapiCanCopy_DISPATCH(pResource)
#define genapiIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define genapiIsDuplicate(pResource, hMemory, pDuplicate) genapiIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define genapiPreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define genapiPreDestruct(pResource) genapiPreDestruct_DISPATCH(pResource)
#define genapiControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define genapiControlFilter(pResource, pCallContext, pParams) genapiControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define genapiIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define genapiIsPartialUnmapSupported(pResource) genapiIsPartialUnmapSupported_DISPATCH(pResource)
#define genapiMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define genapiMapTo(pResource, pParams) genapiMapTo_DISPATCH(pResource, pParams)
#define genapiUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define genapiUnmapFrom(pResource, pParams) genapiUnmapFrom_DISPATCH(pResource, pParams)
#define genapiGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define genapiGetRefCount(pResource) genapiGetRefCount_DISPATCH(pResource)
#define genapiAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define genapiAddAdditionalDependants(pClient, pResource, pReference) genapiAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NV_STATUS genapiMap_DISPATCH(struct GenericEngineApi *pGenericEngineApi, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGenericEngineApi->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__(staticCast(pGenericEngineApi, RsResource), pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS genapiGetMapAddrSpace_DISPATCH(struct GenericEngineApi *pGenericEngineApi, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGenericEngineApi->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__(staticCast(pGenericEngineApi, GpuResource), pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS genapiControl_DISPATCH(struct GenericEngineApi *pGenericEngineApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGenericEngineApi->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__(staticCast(pGenericEngineApi, RsResource), pCallContext, pParams);
}

static inline NV_STATUS genapiUnmap_DISPATCH(struct GenericEngineApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__(staticCast(pGpuResource, RsResource), pCallContext, pCpuMapping);
}

static inline NvBool genapiShareCallback_DISPATCH(struct GenericEngineApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__(staticCast(pGpuResource, RsResource), pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS genapiGetRegBaseOffsetAndSize_DISPATCH(struct GenericEngineApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__(staticCast(pGpuResource, GpuResource), pGpu, pOffset, pSize);
}

static inline NV_STATUS genapiInternalControlForward_DISPATCH(struct GenericEngineApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__(staticCast(pGpuResource, GpuResource), command, pParams, size);
}

static inline NvHandle genapiGetInternalObjectHandle_DISPATCH(struct GenericEngineApi *pGpuResource) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__(staticCast(pGpuResource, GpuResource));
}

static inline NvBool genapiAccessCallback_DISPATCH(struct GenericEngineApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__(staticCast(pResource, RsResource), pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS genapiGetMemInterMapParams_DISPATCH(struct GenericEngineApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__(staticCast(pRmResource, RmResource), pParams);
}

static inline NV_STATUS genapiCheckMemInterUnmap_DISPATCH(struct GenericEngineApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__(staticCast(pRmResource, RmResource), bSubdeviceHandleProvided);
}

static inline NV_STATUS genapiGetMemoryMappingDescriptor_DISPATCH(struct GenericEngineApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__(staticCast(pRmResource, RmResource), ppMemDesc);
}

static inline NV_STATUS genapiControlSerialization_Prologue_DISPATCH(struct GenericEngineApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void genapiControlSerialization_Epilogue_DISPATCH(struct GenericEngineApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS genapiControl_Prologue_DISPATCH(struct GenericEngineApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void genapiControl_Epilogue_DISPATCH(struct GenericEngineApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool genapiCanCopy_DISPATCH(struct GenericEngineApi *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__(staticCast(pResource, RsResource));
}

static inline NV_STATUS genapiIsDuplicate_DISPATCH(struct GenericEngineApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__(staticCast(pResource, RsResource), hMemory, pDuplicate);
}

static inline void genapiPreDestruct_DISPATCH(struct GenericEngineApi *pResource) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__(staticCast(pResource, RsResource));
}

static inline NV_STATUS genapiControlFilter_DISPATCH(struct GenericEngineApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool genapiIsPartialUnmapSupported_DISPATCH(struct GenericEngineApi *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__(staticCast(pResource, RsResource));
}

static inline NV_STATUS genapiMapTo_DISPATCH(struct GenericEngineApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__(staticCast(pResource, RsResource), pParams);
}

static inline NV_STATUS genapiUnmapFrom_DISPATCH(struct GenericEngineApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__(staticCast(pResource, RsResource), pParams);
}

static inline NvU32 genapiGetRefCount_DISPATCH(struct GenericEngineApi *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__(staticCast(pResource, RsResource));
}

static inline void genapiAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct GenericEngineApi *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__(pClient, staticCast(pResource, RsResource), pReference);
}

// Virtual method declarations and/or inline definitions
NV_STATUS genapiMap_IMPL(struct GenericEngineApi *pGenericEngineApi, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);

NV_STATUS genapiGetMapAddrSpace_IMPL(struct GenericEngineApi *pGenericEngineApi, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);

NV_STATUS genapiControl_IMPL(struct GenericEngineApi *pGenericEngineApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);

// Exported method declarations and/or inline definitions
NV_STATUS genapiCtrlCmdMasterGetErrorIntrOffsetMask_IMPL(struct GenericEngineApi *pGenericEngineApi, NV90E6_CTRL_MASTER_GET_ERROR_INTR_OFFSET_MASK_PARAMS *pParams);

NV_STATUS genapiCtrlCmdMasterGetVirtualFunctionErrorContIntrMask_IMPL(struct GenericEngineApi *pGenericEngineApi, NV90E6_CTRL_MASTER_GET_VIRTUAL_FUNCTION_ERROR_CONT_INTR_MASK_PARAMS *pParams);

NV_STATUS genapiCtrlCmdBBXGetLastFlushTime_IMPL(struct GenericEngineApi *pGenericEngineApi, NV90E7_CTRL_BBX_GET_LAST_FLUSH_TIME_PARAMS *pParams);

NV_STATUS genapiCtrlCmdBBXIsNVMFlushEnabled_IMPL(struct GenericEngineApi *pGenericEngineApi, NV90E7_CTRL_BBX_IS_NVM_FLUSH_ENABLED_PARAMS *pParams);

NV_STATUS genapiCtrlCmdBBXGetTimeData_IMPL(struct GenericEngineApi *pGenericEngineApi, NV90E7_CTRL_BBX_GET_TIME_DATA_PARAMS *pParams);

// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


#endif // _GENERICENGINEAPI_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_GENERIC_ENGINE_NVOC_H_
