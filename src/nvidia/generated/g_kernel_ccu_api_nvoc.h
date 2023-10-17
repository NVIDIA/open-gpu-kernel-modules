#ifndef _G_KERNEL_CCU_API_NVOC_H_
#define _G_KERNEL_CCU_API_NVOC_H_
#include "nvoc/runtime.h"

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

#ifdef NVOC_KERNEL_CCU_API_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct KernelCcuApi {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct GpuResource __nvoc_base_GpuResource;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct GpuResource *__nvoc_pbase_GpuResource;
    struct KernelCcuApi *__nvoc_pbase_KernelCcuApi;
    NV_STATUS (*__kccuapiMap__)(struct KernelCcuApi *, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);
    NV_STATUS (*__kccuapiUnmap__)(struct KernelCcuApi *, struct CALL_CONTEXT *, struct RsCpuMapping *);
    NV_STATUS (*__kccuapiGetMapAddrSpace__)(struct KernelCcuApi *, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    NV_STATUS (*__kccuapiGetMemoryMappingDescriptor__)(struct KernelCcuApi *, struct MEMORY_DESCRIPTOR **);
    NV_STATUS (*__kccuapiCtrlCmdSubscribe__)(struct KernelCcuApi *, NV_COUNTER_COLLECTION_UNIT_SUBSCRIBE_PARAMS *);
    NV_STATUS (*__kccuapiCtrlCmdUnsubscribe__)(struct KernelCcuApi *);
    NV_STATUS (*__kccuapiCtrlCmdSetStreamState__)(struct KernelCcuApi *, NV_COUNTER_COLLECTION_UNIT_STREAM_STATE_PARAMS *);
    NV_STATUS (*__kccuapiCtrlCmdGetStreamState__)(struct KernelCcuApi *, NV_COUNTER_COLLECTION_UNIT_STREAM_STATE_PARAMS *);
    NvBool (*__kccuapiShareCallback__)(struct KernelCcuApi *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__kccuapiCheckMemInterUnmap__)(struct KernelCcuApi *, NvBool);
    NV_STATUS (*__kccuapiMapTo__)(struct KernelCcuApi *, RS_RES_MAP_TO_PARAMS *);
    NvU32 (*__kccuapiGetRefCount__)(struct KernelCcuApi *);
    void (*__kccuapiAddAdditionalDependants__)(struct RsClient *, struct KernelCcuApi *, RsResourceRef *);
    NV_STATUS (*__kccuapiControl_Prologue__)(struct KernelCcuApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__kccuapiGetRegBaseOffsetAndSize__)(struct KernelCcuApi *, struct OBJGPU *, NvU32 *, NvU32 *);
    NV_STATUS (*__kccuapiInternalControlForward__)(struct KernelCcuApi *, NvU32, void *, NvU32);
    NV_STATUS (*__kccuapiUnmapFrom__)(struct KernelCcuApi *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__kccuapiControl_Epilogue__)(struct KernelCcuApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__kccuapiControlLookup__)(struct KernelCcuApi *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NvHandle (*__kccuapiGetInternalObjectHandle__)(struct KernelCcuApi *);
    NV_STATUS (*__kccuapiControl__)(struct KernelCcuApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__kccuapiGetMemInterMapParams__)(struct KernelCcuApi *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__kccuapiControlFilter__)(struct KernelCcuApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__kccuapiControlSerialization_Prologue__)(struct KernelCcuApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__kccuapiCanCopy__)(struct KernelCcuApi *);
    void (*__kccuapiPreDestruct__)(struct KernelCcuApi *);
    NV_STATUS (*__kccuapiIsDuplicate__)(struct KernelCcuApi *, NvHandle, NvBool *);
    void (*__kccuapiControlSerialization_Epilogue__)(struct KernelCcuApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__kccuapiAccessCallback__)(struct KernelCcuApi *, struct RsClient *, void *, RsAccessRight);
};

#ifndef __NVOC_CLASS_KernelCcuApi_TYPEDEF__
#define __NVOC_CLASS_KernelCcuApi_TYPEDEF__
typedef struct KernelCcuApi KernelCcuApi;
#endif /* __NVOC_CLASS_KernelCcuApi_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelCcuApi
#define __nvoc_class_id_KernelCcuApi 0x3abed3
#endif /* __nvoc_class_id_KernelCcuApi */

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

#define kccuapiMap(pKernelCcuApi, pCallContext, pParams, pCpuMapping) kccuapiMap_DISPATCH(pKernelCcuApi, pCallContext, pParams, pCpuMapping)
#define kccuapiUnmap(pKernelCcuApi, pCallContext, pCpuMapping) kccuapiUnmap_DISPATCH(pKernelCcuApi, pCallContext, pCpuMapping)
#define kccuapiGetMapAddrSpace(pKernelCcuApi, pCallContext, mapFlags, pAddrSpace) kccuapiGetMapAddrSpace_DISPATCH(pKernelCcuApi, pCallContext, mapFlags, pAddrSpace)
#define kccuapiGetMemoryMappingDescriptor(pKernelCcuApi, ppMemDesc) kccuapiGetMemoryMappingDescriptor_DISPATCH(pKernelCcuApi, ppMemDesc)
#define kccuapiCtrlCmdSubscribe(pKernelCcuApi, pParams) kccuapiCtrlCmdSubscribe_DISPATCH(pKernelCcuApi, pParams)
#define kccuapiCtrlCmdUnsubscribe(pKernelCcuApi) kccuapiCtrlCmdUnsubscribe_DISPATCH(pKernelCcuApi)
#define kccuapiCtrlCmdSetStreamState(pKernelCcuApi, pParams) kccuapiCtrlCmdSetStreamState_DISPATCH(pKernelCcuApi, pParams)
#define kccuapiCtrlCmdGetStreamState(pKernelCcuApi, pParams) kccuapiCtrlCmdGetStreamState_DISPATCH(pKernelCcuApi, pParams)
#define kccuapiShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) kccuapiShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define kccuapiCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) kccuapiCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define kccuapiMapTo(pResource, pParams) kccuapiMapTo_DISPATCH(pResource, pParams)
#define kccuapiGetRefCount(pResource) kccuapiGetRefCount_DISPATCH(pResource)
#define kccuapiAddAdditionalDependants(pClient, pResource, pReference) kccuapiAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define kccuapiControl_Prologue(pResource, pCallContext, pParams) kccuapiControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define kccuapiGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) kccuapiGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define kccuapiInternalControlForward(pGpuResource, command, pParams, size) kccuapiInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define kccuapiUnmapFrom(pResource, pParams) kccuapiUnmapFrom_DISPATCH(pResource, pParams)
#define kccuapiControl_Epilogue(pResource, pCallContext, pParams) kccuapiControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define kccuapiControlLookup(pResource, pParams, ppEntry) kccuapiControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define kccuapiGetInternalObjectHandle(pGpuResource) kccuapiGetInternalObjectHandle_DISPATCH(pGpuResource)
#define kccuapiControl(pGpuResource, pCallContext, pParams) kccuapiControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define kccuapiGetMemInterMapParams(pRmResource, pParams) kccuapiGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define kccuapiControlFilter(pResource, pCallContext, pParams) kccuapiControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define kccuapiControlSerialization_Prologue(pResource, pCallContext, pParams) kccuapiControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define kccuapiCanCopy(pResource) kccuapiCanCopy_DISPATCH(pResource)
#define kccuapiPreDestruct(pResource) kccuapiPreDestruct_DISPATCH(pResource)
#define kccuapiIsDuplicate(pResource, hMemory, pDuplicate) kccuapiIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define kccuapiControlSerialization_Epilogue(pResource, pCallContext, pParams) kccuapiControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define kccuapiAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) kccuapiAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
NV_STATUS kccuapiMap_IMPL(struct KernelCcuApi *pKernelCcuApi, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);

static inline NV_STATUS kccuapiMap_DISPATCH(struct KernelCcuApi *pKernelCcuApi, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pKernelCcuApi->__kccuapiMap__(pKernelCcuApi, pCallContext, pParams, pCpuMapping);
}

NV_STATUS kccuapiUnmap_IMPL(struct KernelCcuApi *pKernelCcuApi, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);

static inline NV_STATUS kccuapiUnmap_DISPATCH(struct KernelCcuApi *pKernelCcuApi, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pKernelCcuApi->__kccuapiUnmap__(pKernelCcuApi, pCallContext, pCpuMapping);
}

NV_STATUS kccuapiGetMapAddrSpace_IMPL(struct KernelCcuApi *pKernelCcuApi, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);

static inline NV_STATUS kccuapiGetMapAddrSpace_DISPATCH(struct KernelCcuApi *pKernelCcuApi, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pKernelCcuApi->__kccuapiGetMapAddrSpace__(pKernelCcuApi, pCallContext, mapFlags, pAddrSpace);
}

NV_STATUS kccuapiGetMemoryMappingDescriptor_IMPL(struct KernelCcuApi *pKernelCcuApi, struct MEMORY_DESCRIPTOR **ppMemDesc);

static inline NV_STATUS kccuapiGetMemoryMappingDescriptor_DISPATCH(struct KernelCcuApi *pKernelCcuApi, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pKernelCcuApi->__kccuapiGetMemoryMappingDescriptor__(pKernelCcuApi, ppMemDesc);
}

NV_STATUS kccuapiCtrlCmdSubscribe_IMPL(struct KernelCcuApi *pKernelCcuApi, NV_COUNTER_COLLECTION_UNIT_SUBSCRIBE_PARAMS *pParams);

static inline NV_STATUS kccuapiCtrlCmdSubscribe_DISPATCH(struct KernelCcuApi *pKernelCcuApi, NV_COUNTER_COLLECTION_UNIT_SUBSCRIBE_PARAMS *pParams) {
    return pKernelCcuApi->__kccuapiCtrlCmdSubscribe__(pKernelCcuApi, pParams);
}

NV_STATUS kccuapiCtrlCmdUnsubscribe_IMPL(struct KernelCcuApi *pKernelCcuApi);

static inline NV_STATUS kccuapiCtrlCmdUnsubscribe_DISPATCH(struct KernelCcuApi *pKernelCcuApi) {
    return pKernelCcuApi->__kccuapiCtrlCmdUnsubscribe__(pKernelCcuApi);
}

NV_STATUS kccuapiCtrlCmdSetStreamState_IMPL(struct KernelCcuApi *pKernelCcuApi, NV_COUNTER_COLLECTION_UNIT_STREAM_STATE_PARAMS *pParams);

static inline NV_STATUS kccuapiCtrlCmdSetStreamState_DISPATCH(struct KernelCcuApi *pKernelCcuApi, NV_COUNTER_COLLECTION_UNIT_STREAM_STATE_PARAMS *pParams) {
    return pKernelCcuApi->__kccuapiCtrlCmdSetStreamState__(pKernelCcuApi, pParams);
}

NV_STATUS kccuapiCtrlCmdGetStreamState_IMPL(struct KernelCcuApi *pKernelCcuApi, NV_COUNTER_COLLECTION_UNIT_STREAM_STATE_PARAMS *pParams);

static inline NV_STATUS kccuapiCtrlCmdGetStreamState_DISPATCH(struct KernelCcuApi *pKernelCcuApi, NV_COUNTER_COLLECTION_UNIT_STREAM_STATE_PARAMS *pParams) {
    return pKernelCcuApi->__kccuapiCtrlCmdGetStreamState__(pKernelCcuApi, pParams);
}

static inline NvBool kccuapiShareCallback_DISPATCH(struct KernelCcuApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__kccuapiShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS kccuapiCheckMemInterUnmap_DISPATCH(struct KernelCcuApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__kccuapiCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS kccuapiMapTo_DISPATCH(struct KernelCcuApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__kccuapiMapTo__(pResource, pParams);
}

static inline NvU32 kccuapiGetRefCount_DISPATCH(struct KernelCcuApi *pResource) {
    return pResource->__kccuapiGetRefCount__(pResource);
}

static inline void kccuapiAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct KernelCcuApi *pResource, RsResourceRef *pReference) {
    pResource->__kccuapiAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS kccuapiControl_Prologue_DISPATCH(struct KernelCcuApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__kccuapiControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS kccuapiGetRegBaseOffsetAndSize_DISPATCH(struct KernelCcuApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__kccuapiGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS kccuapiInternalControlForward_DISPATCH(struct KernelCcuApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__kccuapiInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NV_STATUS kccuapiUnmapFrom_DISPATCH(struct KernelCcuApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__kccuapiUnmapFrom__(pResource, pParams);
}

static inline void kccuapiControl_Epilogue_DISPATCH(struct KernelCcuApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__kccuapiControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS kccuapiControlLookup_DISPATCH(struct KernelCcuApi *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__kccuapiControlLookup__(pResource, pParams, ppEntry);
}

static inline NvHandle kccuapiGetInternalObjectHandle_DISPATCH(struct KernelCcuApi *pGpuResource) {
    return pGpuResource->__kccuapiGetInternalObjectHandle__(pGpuResource);
}

static inline NV_STATUS kccuapiControl_DISPATCH(struct KernelCcuApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__kccuapiControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS kccuapiGetMemInterMapParams_DISPATCH(struct KernelCcuApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__kccuapiGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS kccuapiControlFilter_DISPATCH(struct KernelCcuApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__kccuapiControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS kccuapiControlSerialization_Prologue_DISPATCH(struct KernelCcuApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__kccuapiControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool kccuapiCanCopy_DISPATCH(struct KernelCcuApi *pResource) {
    return pResource->__kccuapiCanCopy__(pResource);
}

static inline void kccuapiPreDestruct_DISPATCH(struct KernelCcuApi *pResource) {
    pResource->__kccuapiPreDestruct__(pResource);
}

static inline NV_STATUS kccuapiIsDuplicate_DISPATCH(struct KernelCcuApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__kccuapiIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void kccuapiControlSerialization_Epilogue_DISPATCH(struct KernelCcuApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__kccuapiControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool kccuapiAccessCallback_DISPATCH(struct KernelCcuApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__kccuapiAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

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
