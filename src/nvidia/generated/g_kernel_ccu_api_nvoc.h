
#ifndef _G_KERNEL_CCU_API_NVOC_H_
#define _G_KERNEL_CCU_API_NVOC_H_
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


struct KernelCcuApi {

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
    struct KernelCcuApi *__nvoc_pbase_KernelCcuApi;    // kccuapi

    // Vtable with 29 per-object function pointers
    NV_STATUS (*__kccuapiMap__)(struct KernelCcuApi * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual override (res) base (gpures)
    NV_STATUS (*__kccuapiUnmap__)(struct KernelCcuApi * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual override (res) base (gpures)
    NV_STATUS (*__kccuapiGetMapAddrSpace__)(struct KernelCcuApi * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual override (gpures) base (gpures)
    NV_STATUS (*__kccuapiGetMemoryMappingDescriptor__)(struct KernelCcuApi * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual override (rmres) base (gpures)
    NV_STATUS (*__kccuapiCtrlCmdSubscribe__)(struct KernelCcuApi * /*this*/, NV_COUNTER_COLLECTION_UNIT_SUBSCRIBE_PARAMS *);  // exported (id=0xcbca0101)
    NV_STATUS (*__kccuapiCtrlCmdUnsubscribe__)(struct KernelCcuApi * /*this*/);  // exported (id=0xcbca0102)
    NV_STATUS (*__kccuapiCtrlCmdSetStreamState__)(struct KernelCcuApi * /*this*/, NV_COUNTER_COLLECTION_UNIT_STREAM_STATE_PARAMS *);  // exported (id=0xcbca0103)
    NV_STATUS (*__kccuapiCtrlCmdGetStreamState__)(struct KernelCcuApi * /*this*/, NV_COUNTER_COLLECTION_UNIT_STREAM_STATE_PARAMS *);  // exported (id=0xcbca0104)
    NV_STATUS (*__kccuapiControl__)(struct KernelCcuApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (gpures) base (gpures)
    NvBool (*__kccuapiShareCallback__)(struct KernelCcuApi * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__kccuapiGetRegBaseOffsetAndSize__)(struct KernelCcuApi * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__kccuapiInternalControlForward__)(struct KernelCcuApi * /*this*/, NvU32, void *, NvU32);  // virtual inherited (gpures) base (gpures)
    NvHandle (*__kccuapiGetInternalObjectHandle__)(struct KernelCcuApi * /*this*/);  // virtual inherited (gpures) base (gpures)
    NvBool (*__kccuapiAccessCallback__)(struct KernelCcuApi * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__kccuapiGetMemInterMapParams__)(struct KernelCcuApi * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__kccuapiCheckMemInterUnmap__)(struct KernelCcuApi * /*this*/, NvBool);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__kccuapiControlSerialization_Prologue__)(struct KernelCcuApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__kccuapiControlSerialization_Epilogue__)(struct KernelCcuApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__kccuapiControl_Prologue__)(struct KernelCcuApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__kccuapiControl_Epilogue__)(struct KernelCcuApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NvBool (*__kccuapiCanCopy__)(struct KernelCcuApi * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__kccuapiIsDuplicate__)(struct KernelCcuApi * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (gpures)
    void (*__kccuapiPreDestruct__)(struct KernelCcuApi * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__kccuapiControlFilter__)(struct KernelCcuApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (gpures)
    NvBool (*__kccuapiIsPartialUnmapSupported__)(struct KernelCcuApi * /*this*/);  // inline virtual inherited (res) base (gpures) body
    NV_STATUS (*__kccuapiMapTo__)(struct KernelCcuApi * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__kccuapiUnmapFrom__)(struct KernelCcuApi * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (gpures)
    NvU32 (*__kccuapiGetRefCount__)(struct KernelCcuApi * /*this*/);  // virtual inherited (res) base (gpures)
    void (*__kccuapiAddAdditionalDependants__)(struct RsClient *, struct KernelCcuApi * /*this*/, RsResourceRef *);  // virtual inherited (res) base (gpures)
};

#ifndef __NVOC_CLASS_KernelCcuApi_TYPEDEF__
#define __NVOC_CLASS_KernelCcuApi_TYPEDEF__
typedef struct KernelCcuApi KernelCcuApi;
#endif /* __NVOC_CLASS_KernelCcuApi_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelCcuApi
#define __nvoc_class_id_KernelCcuApi 0x3abed3
#endif /* __nvoc_class_id_KernelCcuApi */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelCcuApi;

#define __staticCast_KernelCcuApi(pThis) \
    ((pThis)->__nvoc_pbase_KernelCcuApi)

#ifdef __nvoc_kernel_ccu_api_h_disabled
#define __dynamicCast_KernelCcuApi(pThis) ((KernelCcuApi*)NULL)
#else //__nvoc_kernel_ccu_api_h_disabled
#define __dynamicCast_KernelCcuApi(pThis) \
    ((KernelCcuApi*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelCcuApi)))
#endif //__nvoc_kernel_ccu_api_h_disabled

NV_STATUS __nvoc_objCreateDynamic_KernelCcuApi(KernelCcuApi**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelCcuApi(KernelCcuApi**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_KernelCcuApi(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_KernelCcuApi((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define kccuapiMap_FNPTR(pKernelCcuApi) pKernelCcuApi->__kccuapiMap__
#define kccuapiMap(pKernelCcuApi, pCallContext, pParams, pCpuMapping) kccuapiMap_DISPATCH(pKernelCcuApi, pCallContext, pParams, pCpuMapping)
#define kccuapiUnmap_FNPTR(pKernelCcuApi) pKernelCcuApi->__kccuapiUnmap__
#define kccuapiUnmap(pKernelCcuApi, pCallContext, pCpuMapping) kccuapiUnmap_DISPATCH(pKernelCcuApi, pCallContext, pCpuMapping)
#define kccuapiGetMapAddrSpace_FNPTR(pKernelCcuApi) pKernelCcuApi->__kccuapiGetMapAddrSpace__
#define kccuapiGetMapAddrSpace(pKernelCcuApi, pCallContext, mapFlags, pAddrSpace) kccuapiGetMapAddrSpace_DISPATCH(pKernelCcuApi, pCallContext, mapFlags, pAddrSpace)
#define kccuapiGetMemoryMappingDescriptor_FNPTR(pKernelCcuApi) pKernelCcuApi->__kccuapiGetMemoryMappingDescriptor__
#define kccuapiGetMemoryMappingDescriptor(pKernelCcuApi, ppMemDesc) kccuapiGetMemoryMappingDescriptor_DISPATCH(pKernelCcuApi, ppMemDesc)
#define kccuapiCtrlCmdSubscribe_FNPTR(pKernelCcuApi) pKernelCcuApi->__kccuapiCtrlCmdSubscribe__
#define kccuapiCtrlCmdSubscribe(pKernelCcuApi, pParams) kccuapiCtrlCmdSubscribe_DISPATCH(pKernelCcuApi, pParams)
#define kccuapiCtrlCmdUnsubscribe_FNPTR(pKernelCcuApi) pKernelCcuApi->__kccuapiCtrlCmdUnsubscribe__
#define kccuapiCtrlCmdUnsubscribe(pKernelCcuApi) kccuapiCtrlCmdUnsubscribe_DISPATCH(pKernelCcuApi)
#define kccuapiCtrlCmdSetStreamState_FNPTR(pKernelCcuApi) pKernelCcuApi->__kccuapiCtrlCmdSetStreamState__
#define kccuapiCtrlCmdSetStreamState(pKernelCcuApi, pParams) kccuapiCtrlCmdSetStreamState_DISPATCH(pKernelCcuApi, pParams)
#define kccuapiCtrlCmdGetStreamState_FNPTR(pKernelCcuApi) pKernelCcuApi->__kccuapiCtrlCmdGetStreamState__
#define kccuapiCtrlCmdGetStreamState(pKernelCcuApi, pParams) kccuapiCtrlCmdGetStreamState_DISPATCH(pKernelCcuApi, pParams)
#define kccuapiControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresControl__
#define kccuapiControl(pGpuResource, pCallContext, pParams) kccuapiControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define kccuapiShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresShareCallback__
#define kccuapiShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) kccuapiShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define kccuapiGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetRegBaseOffsetAndSize__
#define kccuapiGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) kccuapiGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define kccuapiInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresInternalControlForward__
#define kccuapiInternalControlForward(pGpuResource, command, pParams, size) kccuapiInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define kccuapiGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetInternalObjectHandle__
#define kccuapiGetInternalObjectHandle(pGpuResource) kccuapiGetInternalObjectHandle_DISPATCH(pGpuResource)
#define kccuapiAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresAccessCallback__
#define kccuapiAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) kccuapiAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define kccuapiGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemInterMapParams__
#define kccuapiGetMemInterMapParams(pRmResource, pParams) kccuapiGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define kccuapiCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresCheckMemInterUnmap__
#define kccuapiCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) kccuapiCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define kccuapiControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Prologue__
#define kccuapiControlSerialization_Prologue(pResource, pCallContext, pParams) kccuapiControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define kccuapiControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Epilogue__
#define kccuapiControlSerialization_Epilogue(pResource, pCallContext, pParams) kccuapiControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define kccuapiControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Prologue__
#define kccuapiControl_Prologue(pResource, pCallContext, pParams) kccuapiControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define kccuapiControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Epilogue__
#define kccuapiControl_Epilogue(pResource, pCallContext, pParams) kccuapiControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define kccuapiCanCopy_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__
#define kccuapiCanCopy(pResource) kccuapiCanCopy_DISPATCH(pResource)
#define kccuapiIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsDuplicate__
#define kccuapiIsDuplicate(pResource, hMemory, pDuplicate) kccuapiIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define kccuapiPreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resPreDestruct__
#define kccuapiPreDestruct(pResource) kccuapiPreDestruct_DISPATCH(pResource)
#define kccuapiControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resControlFilter__
#define kccuapiControlFilter(pResource, pCallContext, pParams) kccuapiControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define kccuapiIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsPartialUnmapSupported__
#define kccuapiIsPartialUnmapSupported(pResource) kccuapiIsPartialUnmapSupported_DISPATCH(pResource)
#define kccuapiMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resMapTo__
#define kccuapiMapTo(pResource, pParams) kccuapiMapTo_DISPATCH(pResource, pParams)
#define kccuapiUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmapFrom__
#define kccuapiUnmapFrom(pResource, pParams) kccuapiUnmapFrom_DISPATCH(pResource, pParams)
#define kccuapiGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resGetRefCount__
#define kccuapiGetRefCount(pResource) kccuapiGetRefCount_DISPATCH(pResource)
#define kccuapiAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resAddAdditionalDependants__
#define kccuapiAddAdditionalDependants(pClient, pResource, pReference) kccuapiAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NV_STATUS kccuapiMap_DISPATCH(struct KernelCcuApi *pKernelCcuApi, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pKernelCcuApi->__kccuapiMap__(pKernelCcuApi, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS kccuapiUnmap_DISPATCH(struct KernelCcuApi *pKernelCcuApi, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pKernelCcuApi->__kccuapiUnmap__(pKernelCcuApi, pCallContext, pCpuMapping);
}

static inline NV_STATUS kccuapiGetMapAddrSpace_DISPATCH(struct KernelCcuApi *pKernelCcuApi, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pKernelCcuApi->__kccuapiGetMapAddrSpace__(pKernelCcuApi, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS kccuapiGetMemoryMappingDescriptor_DISPATCH(struct KernelCcuApi *pKernelCcuApi, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pKernelCcuApi->__kccuapiGetMemoryMappingDescriptor__(pKernelCcuApi, ppMemDesc);
}

static inline NV_STATUS kccuapiCtrlCmdSubscribe_DISPATCH(struct KernelCcuApi *pKernelCcuApi, NV_COUNTER_COLLECTION_UNIT_SUBSCRIBE_PARAMS *pParams) {
    return pKernelCcuApi->__kccuapiCtrlCmdSubscribe__(pKernelCcuApi, pParams);
}

static inline NV_STATUS kccuapiCtrlCmdUnsubscribe_DISPATCH(struct KernelCcuApi *pKernelCcuApi) {
    return pKernelCcuApi->__kccuapiCtrlCmdUnsubscribe__(pKernelCcuApi);
}

static inline NV_STATUS kccuapiCtrlCmdSetStreamState_DISPATCH(struct KernelCcuApi *pKernelCcuApi, NV_COUNTER_COLLECTION_UNIT_STREAM_STATE_PARAMS *pParams) {
    return pKernelCcuApi->__kccuapiCtrlCmdSetStreamState__(pKernelCcuApi, pParams);
}

static inline NV_STATUS kccuapiCtrlCmdGetStreamState_DISPATCH(struct KernelCcuApi *pKernelCcuApi, NV_COUNTER_COLLECTION_UNIT_STREAM_STATE_PARAMS *pParams) {
    return pKernelCcuApi->__kccuapiCtrlCmdGetStreamState__(pKernelCcuApi, pParams);
}

static inline NV_STATUS kccuapiControl_DISPATCH(struct KernelCcuApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__kccuapiControl__(pGpuResource, pCallContext, pParams);
}

static inline NvBool kccuapiShareCallback_DISPATCH(struct KernelCcuApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__kccuapiShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS kccuapiGetRegBaseOffsetAndSize_DISPATCH(struct KernelCcuApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__kccuapiGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS kccuapiInternalControlForward_DISPATCH(struct KernelCcuApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__kccuapiInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NvHandle kccuapiGetInternalObjectHandle_DISPATCH(struct KernelCcuApi *pGpuResource) {
    return pGpuResource->__kccuapiGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool kccuapiAccessCallback_DISPATCH(struct KernelCcuApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__kccuapiAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS kccuapiGetMemInterMapParams_DISPATCH(struct KernelCcuApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__kccuapiGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS kccuapiCheckMemInterUnmap_DISPATCH(struct KernelCcuApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__kccuapiCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS kccuapiControlSerialization_Prologue_DISPATCH(struct KernelCcuApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__kccuapiControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void kccuapiControlSerialization_Epilogue_DISPATCH(struct KernelCcuApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__kccuapiControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS kccuapiControl_Prologue_DISPATCH(struct KernelCcuApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__kccuapiControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void kccuapiControl_Epilogue_DISPATCH(struct KernelCcuApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__kccuapiControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool kccuapiCanCopy_DISPATCH(struct KernelCcuApi *pResource) {
    return pResource->__kccuapiCanCopy__(pResource);
}

static inline NV_STATUS kccuapiIsDuplicate_DISPATCH(struct KernelCcuApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__kccuapiIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void kccuapiPreDestruct_DISPATCH(struct KernelCcuApi *pResource) {
    pResource->__kccuapiPreDestruct__(pResource);
}

static inline NV_STATUS kccuapiControlFilter_DISPATCH(struct KernelCcuApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__kccuapiControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool kccuapiIsPartialUnmapSupported_DISPATCH(struct KernelCcuApi *pResource) {
    return pResource->__kccuapiIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS kccuapiMapTo_DISPATCH(struct KernelCcuApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__kccuapiMapTo__(pResource, pParams);
}

static inline NV_STATUS kccuapiUnmapFrom_DISPATCH(struct KernelCcuApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__kccuapiUnmapFrom__(pResource, pParams);
}

static inline NvU32 kccuapiGetRefCount_DISPATCH(struct KernelCcuApi *pResource) {
    return pResource->__kccuapiGetRefCount__(pResource);
}

static inline void kccuapiAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct KernelCcuApi *pResource, RsResourceRef *pReference) {
    pResource->__kccuapiAddAdditionalDependants__(pClient, pResource, pReference);
}

NV_STATUS kccuapiMap_IMPL(struct KernelCcuApi *pKernelCcuApi, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);

NV_STATUS kccuapiUnmap_IMPL(struct KernelCcuApi *pKernelCcuApi, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);

NV_STATUS kccuapiGetMapAddrSpace_IMPL(struct KernelCcuApi *pKernelCcuApi, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);

NV_STATUS kccuapiGetMemoryMappingDescriptor_IMPL(struct KernelCcuApi *pKernelCcuApi, struct MEMORY_DESCRIPTOR **ppMemDesc);

NV_STATUS kccuapiCtrlCmdSubscribe_IMPL(struct KernelCcuApi *pKernelCcuApi, NV_COUNTER_COLLECTION_UNIT_SUBSCRIBE_PARAMS *pParams);

NV_STATUS kccuapiCtrlCmdUnsubscribe_IMPL(struct KernelCcuApi *pKernelCcuApi);

NV_STATUS kccuapiCtrlCmdSetStreamState_IMPL(struct KernelCcuApi *pKernelCcuApi, NV_COUNTER_COLLECTION_UNIT_STREAM_STATE_PARAMS *pParams);

NV_STATUS kccuapiCtrlCmdGetStreamState_IMPL(struct KernelCcuApi *pKernelCcuApi, NV_COUNTER_COLLECTION_UNIT_STREAM_STATE_PARAMS *pParams);

NV_STATUS kccuapiConstruct_IMPL(struct KernelCcuApi *arg_pKernelCcuApi, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_kccuapiConstruct(arg_pKernelCcuApi, arg_pCallContext, arg_pParams) kccuapiConstruct_IMPL(arg_pKernelCcuApi, arg_pCallContext, arg_pParams)
void kccuapiDestruct_IMPL(struct KernelCcuApi *pKernelCcuApi);

#define __nvoc_kccuapiDestruct(pKernelCcuApi) kccuapiDestruct_IMPL(pKernelCcuApi)
#undef PRIVATE_FIELD

#endif // KERNEL_CCU_API_H


#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_CCU_API_NVOC_H_
