
#ifndef _G_KERNEL_CCU_API_NVOC_H_
#define _G_KERNEL_CCU_API_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_kernel_ccu_api_nvoc.h"

#ifndef KERNEL_CCU_API_H
#define KERNEL_CCU_API_H

#include "gpu/gpu_resource.h"
#include "ctrl/ctrlcbca.h"

/****************************************************************************\
 *                                                                           *
 *      Kernel Ccu Api class header.                                         *
 *                                                                           *
 ****************************************************************************/


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_KERNEL_CCU_API_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__KernelCcuApi;
struct NVOC_METADATA__GpuResource;


struct KernelCcuApi {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__KernelCcuApi *__nvoc_metadata_ptr;
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
    struct KernelCcuApi *__nvoc_pbase_KernelCcuApi;    // kccuapi
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__KernelCcuApi {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResource metadata__GpuResource;
};

#ifndef __nvoc_class_id_KernelCcuApi
#define __nvoc_class_id_KernelCcuApi 0x3abed3u
typedef struct KernelCcuApi KernelCcuApi;
#endif /* __nvoc_class_id_KernelCcuApi */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelCcuApi;

#define __staticCast_KernelCcuApi(pThis) \
    ((pThis)->__nvoc_pbase_KernelCcuApi)

#ifdef __nvoc_kernel_ccu_api_h_disabled
#define __dynamicCast_KernelCcuApi(pThis) ((KernelCcuApi*) NULL)
#else //__nvoc_kernel_ccu_api_h_disabled
#define __dynamicCast_KernelCcuApi(pThis) \
    ((KernelCcuApi*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelCcuApi)))
#endif //__nvoc_kernel_ccu_api_h_disabled

NV_STATUS __nvoc_objCreateDynamic_KernelCcuApi(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelCcuApi(KernelCcuApi**, Dynamic*, NvU32, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __objCreate_KernelCcuApi(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams) \
    __nvoc_objCreate_KernelCcuApi((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pCallContext, pParams)


// Wrapper macros for implementation functions
NV_STATUS kccuapiConstruct_IMPL(struct KernelCcuApi *pKernelCcuApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __nvoc_kccuapiConstruct(pKernelCcuApi, pCallContext, pParams) kccuapiConstruct_IMPL(pKernelCcuApi, pCallContext, pParams)

void kccuapiDestruct_IMPL(struct KernelCcuApi *pKernelCcuApi);
#define __nvoc_kccuapiDestruct(pKernelCcuApi) kccuapiDestruct_IMPL(pKernelCcuApi)

NV_STATUS kccuapiCtrlCmdSubscribe_IMPL(struct KernelCcuApi *pKernelCcuApi, NV_COUNTER_COLLECTION_UNIT_SUBSCRIBE_PARAMS *pParams);
#ifdef __nvoc_kernel_ccu_api_h_disabled
static inline NV_STATUS kccuapiCtrlCmdSubscribe(struct KernelCcuApi *pKernelCcuApi, NV_COUNTER_COLLECTION_UNIT_SUBSCRIBE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelCcuApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_ccu_api_h_disabled
#define kccuapiCtrlCmdSubscribe(pKernelCcuApi, pParams) kccuapiCtrlCmdSubscribe_IMPL(pKernelCcuApi, pParams)
#endif // __nvoc_kernel_ccu_api_h_disabled

NV_STATUS kccuapiCtrlCmdUnsubscribe_IMPL(struct KernelCcuApi *pKernelCcuApi);
#ifdef __nvoc_kernel_ccu_api_h_disabled
static inline NV_STATUS kccuapiCtrlCmdUnsubscribe(struct KernelCcuApi *pKernelCcuApi) {
    NV_ASSERT_FAILED_PRECOMP("KernelCcuApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_ccu_api_h_disabled
#define kccuapiCtrlCmdUnsubscribe(pKernelCcuApi) kccuapiCtrlCmdUnsubscribe_IMPL(pKernelCcuApi)
#endif // __nvoc_kernel_ccu_api_h_disabled

NV_STATUS kccuapiCtrlCmdSetStreamState_IMPL(struct KernelCcuApi *pKernelCcuApi, NV_COUNTER_COLLECTION_UNIT_STREAM_STATE_PARAMS *pParams);
#ifdef __nvoc_kernel_ccu_api_h_disabled
static inline NV_STATUS kccuapiCtrlCmdSetStreamState(struct KernelCcuApi *pKernelCcuApi, NV_COUNTER_COLLECTION_UNIT_STREAM_STATE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelCcuApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_ccu_api_h_disabled
#define kccuapiCtrlCmdSetStreamState(pKernelCcuApi, pParams) kccuapiCtrlCmdSetStreamState_IMPL(pKernelCcuApi, pParams)
#endif // __nvoc_kernel_ccu_api_h_disabled

NV_STATUS kccuapiCtrlCmdGetStreamState_IMPL(struct KernelCcuApi *pKernelCcuApi, NV_COUNTER_COLLECTION_UNIT_STREAM_STATE_PARAMS *pParams);
#ifdef __nvoc_kernel_ccu_api_h_disabled
static inline NV_STATUS kccuapiCtrlCmdGetStreamState(struct KernelCcuApi *pKernelCcuApi, NV_COUNTER_COLLECTION_UNIT_STREAM_STATE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelCcuApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_ccu_api_h_disabled
#define kccuapiCtrlCmdGetStreamState(pKernelCcuApi, pParams) kccuapiCtrlCmdGetStreamState_IMPL(pKernelCcuApi, pParams)
#endif // __nvoc_kernel_ccu_api_h_disabled


// Wrapper macros for halified functions
#define kccuapiMap_FNPTR(pKernelCcuApi) pKernelCcuApi->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__
#define kccuapiMap(pKernelCcuApi, pCallContext, pParams, pCpuMapping) kccuapiMap_DISPATCH(pKernelCcuApi, pCallContext, pParams, pCpuMapping)
#define kccuapiUnmap_FNPTR(pKernelCcuApi) pKernelCcuApi->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__
#define kccuapiUnmap(pKernelCcuApi, pCallContext, pCpuMapping) kccuapiUnmap_DISPATCH(pKernelCcuApi, pCallContext, pCpuMapping)
#define kccuapiGetMapAddrSpace_FNPTR(pKernelCcuApi) pKernelCcuApi->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__
#define kccuapiGetMapAddrSpace(pKernelCcuApi, pCallContext, mapFlags, pAddrSpace) kccuapiGetMapAddrSpace_DISPATCH(pKernelCcuApi, pCallContext, mapFlags, pAddrSpace)
#define kccuapiGetMemoryMappingDescriptor_FNPTR(pKernelCcuApi) pKernelCcuApi->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define kccuapiGetMemoryMappingDescriptor(pKernelCcuApi, ppMemDesc) kccuapiGetMemoryMappingDescriptor_DISPATCH(pKernelCcuApi, ppMemDesc)
#define kccuapiControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__
#define kccuapiControl(pGpuResource, pCallContext, pParams) kccuapiControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define kccuapiShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__
#define kccuapiShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) kccuapiShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define kccuapiGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__
#define kccuapiGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) kccuapiGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define kccuapiInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__
#define kccuapiInternalControlForward(pGpuResource, command, pParams, size) kccuapiInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define kccuapiGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__
#define kccuapiGetInternalObjectHandle(pGpuResource) kccuapiGetInternalObjectHandle_DISPATCH(pGpuResource)
#define kccuapiAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__
#define kccuapiAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) kccuapiAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define kccuapiGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define kccuapiGetMemInterMapParams(pRmResource, pParams) kccuapiGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define kccuapiCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define kccuapiCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) kccuapiCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define kccuapiControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__
#define kccuapiControlSerialization_Prologue(pResource, pCallContext, pParams) kccuapiControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define kccuapiControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__
#define kccuapiControlSerialization_Epilogue(pResource, pCallContext, pParams) kccuapiControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define kccuapiControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__
#define kccuapiControl_Prologue(pResource, pCallContext, pParams) kccuapiControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define kccuapiControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__
#define kccuapiControl_Epilogue(pResource, pCallContext, pParams) kccuapiControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define kccuapiCanCopy_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define kccuapiCanCopy(pResource) kccuapiCanCopy_DISPATCH(pResource)
#define kccuapiIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define kccuapiIsDuplicate(pResource, hMemory, pDuplicate) kccuapiIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define kccuapiPreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define kccuapiPreDestruct(pResource) kccuapiPreDestruct_DISPATCH(pResource)
#define kccuapiControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define kccuapiControlFilter(pResource, pCallContext, pParams) kccuapiControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define kccuapiIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define kccuapiIsPartialUnmapSupported(pResource) kccuapiIsPartialUnmapSupported_DISPATCH(pResource)
#define kccuapiMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define kccuapiMapTo(pResource, pParams) kccuapiMapTo_DISPATCH(pResource, pParams)
#define kccuapiUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define kccuapiUnmapFrom(pResource, pParams) kccuapiUnmapFrom_DISPATCH(pResource, pParams)
#define kccuapiGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define kccuapiGetRefCount(pResource) kccuapiGetRefCount_DISPATCH(pResource)
#define kccuapiAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define kccuapiAddAdditionalDependants(pClient, pResource, pReference) kccuapiAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NV_STATUS kccuapiMap_DISPATCH(struct KernelCcuApi *pKernelCcuApi, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pKernelCcuApi->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__(staticCast(pKernelCcuApi, RsResource), pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS kccuapiUnmap_DISPATCH(struct KernelCcuApi *pKernelCcuApi, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pKernelCcuApi->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__(staticCast(pKernelCcuApi, RsResource), pCallContext, pCpuMapping);
}

static inline NV_STATUS kccuapiGetMapAddrSpace_DISPATCH(struct KernelCcuApi *pKernelCcuApi, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pKernelCcuApi->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__(staticCast(pKernelCcuApi, GpuResource), pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS kccuapiGetMemoryMappingDescriptor_DISPATCH(struct KernelCcuApi *pKernelCcuApi, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pKernelCcuApi->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__(staticCast(pKernelCcuApi, RmResource), ppMemDesc);
}

static inline NV_STATUS kccuapiControl_DISPATCH(struct KernelCcuApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__(staticCast(pGpuResource, RsResource), pCallContext, pParams);
}

static inline NvBool kccuapiShareCallback_DISPATCH(struct KernelCcuApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__(staticCast(pGpuResource, RsResource), pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS kccuapiGetRegBaseOffsetAndSize_DISPATCH(struct KernelCcuApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__(staticCast(pGpuResource, GpuResource), pGpu, pOffset, pSize);
}

static inline NV_STATUS kccuapiInternalControlForward_DISPATCH(struct KernelCcuApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__(staticCast(pGpuResource, GpuResource), command, pParams, size);
}

static inline NvHandle kccuapiGetInternalObjectHandle_DISPATCH(struct KernelCcuApi *pGpuResource) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__(staticCast(pGpuResource, GpuResource));
}

static inline NvBool kccuapiAccessCallback_DISPATCH(struct KernelCcuApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__(staticCast(pResource, RsResource), pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS kccuapiGetMemInterMapParams_DISPATCH(struct KernelCcuApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__(staticCast(pRmResource, RmResource), pParams);
}

static inline NV_STATUS kccuapiCheckMemInterUnmap_DISPATCH(struct KernelCcuApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__(staticCast(pRmResource, RmResource), bSubdeviceHandleProvided);
}

static inline NV_STATUS kccuapiControlSerialization_Prologue_DISPATCH(struct KernelCcuApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void kccuapiControlSerialization_Epilogue_DISPATCH(struct KernelCcuApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS kccuapiControl_Prologue_DISPATCH(struct KernelCcuApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void kccuapiControl_Epilogue_DISPATCH(struct KernelCcuApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool kccuapiCanCopy_DISPATCH(struct KernelCcuApi *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__(staticCast(pResource, RsResource));
}

static inline NV_STATUS kccuapiIsDuplicate_DISPATCH(struct KernelCcuApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__(staticCast(pResource, RsResource), hMemory, pDuplicate);
}

static inline void kccuapiPreDestruct_DISPATCH(struct KernelCcuApi *pResource) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__(staticCast(pResource, RsResource));
}

static inline NV_STATUS kccuapiControlFilter_DISPATCH(struct KernelCcuApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool kccuapiIsPartialUnmapSupported_DISPATCH(struct KernelCcuApi *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__(staticCast(pResource, RsResource));
}

static inline NV_STATUS kccuapiMapTo_DISPATCH(struct KernelCcuApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__(staticCast(pResource, RsResource), pParams);
}

static inline NV_STATUS kccuapiUnmapFrom_DISPATCH(struct KernelCcuApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__(staticCast(pResource, RsResource), pParams);
}

static inline NvU32 kccuapiGetRefCount_DISPATCH(struct KernelCcuApi *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__(staticCast(pResource, RsResource));
}

static inline void kccuapiAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct KernelCcuApi *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__(pClient, staticCast(pResource, RsResource), pReference);
}

// Virtual method declarations and/or inline definitions
NV_STATUS kccuapiMap_IMPL(struct KernelCcuApi *pKernelCcuApi, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);

NV_STATUS kccuapiUnmap_IMPL(struct KernelCcuApi *pKernelCcuApi, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);

NV_STATUS kccuapiGetMapAddrSpace_IMPL(struct KernelCcuApi *pKernelCcuApi, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);

NV_STATUS kccuapiGetMemoryMappingDescriptor_IMPL(struct KernelCcuApi *pKernelCcuApi, struct MEMORY_DESCRIPTOR **ppMemDesc);

// Exported method declarations and/or inline definitions
NV_STATUS kccuapiCtrlCmdSubscribe_IMPL(struct KernelCcuApi *pKernelCcuApi, NV_COUNTER_COLLECTION_UNIT_SUBSCRIBE_PARAMS *pParams);

NV_STATUS kccuapiCtrlCmdUnsubscribe_IMPL(struct KernelCcuApi *pKernelCcuApi);

NV_STATUS kccuapiCtrlCmdSetStreamState_IMPL(struct KernelCcuApi *pKernelCcuApi, NV_COUNTER_COLLECTION_UNIT_STREAM_STATE_PARAMS *pParams);

NV_STATUS kccuapiCtrlCmdGetStreamState_IMPL(struct KernelCcuApi *pKernelCcuApi, NV_COUNTER_COLLECTION_UNIT_STREAM_STATE_PARAMS *pParams);

// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD

#endif // KERNEL_CCU_API_H


#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_CCU_API_NVOC_H_
