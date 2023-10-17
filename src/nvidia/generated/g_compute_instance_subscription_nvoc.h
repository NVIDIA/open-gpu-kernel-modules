#ifndef _G_COMPUTE_INSTANCE_SUBSCRIPTION_NVOC_H_
#define _G_COMPUTE_INSTANCE_SUBSCRIPTION_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 *       This file contains the functions managing MIG compute instance subscriptions
 *
 *****************************************************************************/

#include "g_compute_instance_subscription_nvoc.h"

#ifndef COMPUTE_INSTANCE_SUBSCRIPTION_H
#define COMPUTE_INSTANCE_SUBSCRIPTION_H

#include "class/clc638.h"
#include "ctrl/ctrlc638.h"
#include "gpu/gpu_resource.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"

// ****************************************************************************
//                          Type Definitions
// ****************************************************************************

#ifdef NVOC_COMPUTE_INSTANCE_SUBSCRIPTION_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct ComputeInstanceSubscription {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct GpuResource __nvoc_base_GpuResource;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct GpuResource *__nvoc_pbase_GpuResource;
    struct ComputeInstanceSubscription *__nvoc_pbase_ComputeInstanceSubscription;
    NvBool (*__cisubscriptionCanCopy__)(struct ComputeInstanceSubscription *);
    NV_STATUS (*__cisubscriptionCtrlCmdGetUuid__)(struct ComputeInstanceSubscription *, NVC638_CTRL_GET_UUID_PARAMS *);
    NvBool (*__cisubscriptionShareCallback__)(struct ComputeInstanceSubscription *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__cisubscriptionCheckMemInterUnmap__)(struct ComputeInstanceSubscription *, NvBool);
    NV_STATUS (*__cisubscriptionMapTo__)(struct ComputeInstanceSubscription *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__cisubscriptionGetMapAddrSpace__)(struct ComputeInstanceSubscription *, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    NvU32 (*__cisubscriptionGetRefCount__)(struct ComputeInstanceSubscription *);
    void (*__cisubscriptionAddAdditionalDependants__)(struct RsClient *, struct ComputeInstanceSubscription *, RsResourceRef *);
    NV_STATUS (*__cisubscriptionControl_Prologue__)(struct ComputeInstanceSubscription *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__cisubscriptionGetRegBaseOffsetAndSize__)(struct ComputeInstanceSubscription *, struct OBJGPU *, NvU32 *, NvU32 *);
    NV_STATUS (*__cisubscriptionInternalControlForward__)(struct ComputeInstanceSubscription *, NvU32, void *, NvU32);
    NV_STATUS (*__cisubscriptionUnmapFrom__)(struct ComputeInstanceSubscription *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__cisubscriptionControl_Epilogue__)(struct ComputeInstanceSubscription *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__cisubscriptionControlLookup__)(struct ComputeInstanceSubscription *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NvHandle (*__cisubscriptionGetInternalObjectHandle__)(struct ComputeInstanceSubscription *);
    NV_STATUS (*__cisubscriptionControl__)(struct ComputeInstanceSubscription *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__cisubscriptionUnmap__)(struct ComputeInstanceSubscription *, struct CALL_CONTEXT *, struct RsCpuMapping *);
    NV_STATUS (*__cisubscriptionGetMemInterMapParams__)(struct ComputeInstanceSubscription *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__cisubscriptionGetMemoryMappingDescriptor__)(struct ComputeInstanceSubscription *, struct MEMORY_DESCRIPTOR **);
    NV_STATUS (*__cisubscriptionControlFilter__)(struct ComputeInstanceSubscription *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__cisubscriptionControlSerialization_Prologue__)(struct ComputeInstanceSubscription *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    void (*__cisubscriptionPreDestruct__)(struct ComputeInstanceSubscription *);
    NV_STATUS (*__cisubscriptionIsDuplicate__)(struct ComputeInstanceSubscription *, NvHandle, NvBool *);
    void (*__cisubscriptionControlSerialization_Epilogue__)(struct ComputeInstanceSubscription *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__cisubscriptionMap__)(struct ComputeInstanceSubscription *, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);
    NvBool (*__cisubscriptionAccessCallback__)(struct ComputeInstanceSubscription *, struct RsClient *, void *, RsAccessRight);
    MIG_COMPUTE_INSTANCE *PRIVATE_FIELD(pMIGComputeInstance);
    NvU64 PRIVATE_FIELD(dupedCapDescriptor);
};

struct ComputeInstanceSubscription_PRIVATE {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct GpuResource __nvoc_base_GpuResource;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct GpuResource *__nvoc_pbase_GpuResource;
    struct ComputeInstanceSubscription *__nvoc_pbase_ComputeInstanceSubscription;
    NvBool (*__cisubscriptionCanCopy__)(struct ComputeInstanceSubscription *);
    NV_STATUS (*__cisubscriptionCtrlCmdGetUuid__)(struct ComputeInstanceSubscription *, NVC638_CTRL_GET_UUID_PARAMS *);
    NvBool (*__cisubscriptionShareCallback__)(struct ComputeInstanceSubscription *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__cisubscriptionCheckMemInterUnmap__)(struct ComputeInstanceSubscription *, NvBool);
    NV_STATUS (*__cisubscriptionMapTo__)(struct ComputeInstanceSubscription *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__cisubscriptionGetMapAddrSpace__)(struct ComputeInstanceSubscription *, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    NvU32 (*__cisubscriptionGetRefCount__)(struct ComputeInstanceSubscription *);
    void (*__cisubscriptionAddAdditionalDependants__)(struct RsClient *, struct ComputeInstanceSubscription *, RsResourceRef *);
    NV_STATUS (*__cisubscriptionControl_Prologue__)(struct ComputeInstanceSubscription *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__cisubscriptionGetRegBaseOffsetAndSize__)(struct ComputeInstanceSubscription *, struct OBJGPU *, NvU32 *, NvU32 *);
    NV_STATUS (*__cisubscriptionInternalControlForward__)(struct ComputeInstanceSubscription *, NvU32, void *, NvU32);
    NV_STATUS (*__cisubscriptionUnmapFrom__)(struct ComputeInstanceSubscription *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__cisubscriptionControl_Epilogue__)(struct ComputeInstanceSubscription *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__cisubscriptionControlLookup__)(struct ComputeInstanceSubscription *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NvHandle (*__cisubscriptionGetInternalObjectHandle__)(struct ComputeInstanceSubscription *);
    NV_STATUS (*__cisubscriptionControl__)(struct ComputeInstanceSubscription *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__cisubscriptionUnmap__)(struct ComputeInstanceSubscription *, struct CALL_CONTEXT *, struct RsCpuMapping *);
    NV_STATUS (*__cisubscriptionGetMemInterMapParams__)(struct ComputeInstanceSubscription *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__cisubscriptionGetMemoryMappingDescriptor__)(struct ComputeInstanceSubscription *, struct MEMORY_DESCRIPTOR **);
    NV_STATUS (*__cisubscriptionControlFilter__)(struct ComputeInstanceSubscription *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__cisubscriptionControlSerialization_Prologue__)(struct ComputeInstanceSubscription *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    void (*__cisubscriptionPreDestruct__)(struct ComputeInstanceSubscription *);
    NV_STATUS (*__cisubscriptionIsDuplicate__)(struct ComputeInstanceSubscription *, NvHandle, NvBool *);
    void (*__cisubscriptionControlSerialization_Epilogue__)(struct ComputeInstanceSubscription *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__cisubscriptionMap__)(struct ComputeInstanceSubscription *, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);
    NvBool (*__cisubscriptionAccessCallback__)(struct ComputeInstanceSubscription *, struct RsClient *, void *, RsAccessRight);
    MIG_COMPUTE_INSTANCE *pMIGComputeInstance;
    NvU64 dupedCapDescriptor;
};

#ifndef __NVOC_CLASS_ComputeInstanceSubscription_TYPEDEF__
#define __NVOC_CLASS_ComputeInstanceSubscription_TYPEDEF__
typedef struct ComputeInstanceSubscription ComputeInstanceSubscription;
#endif /* __NVOC_CLASS_ComputeInstanceSubscription_TYPEDEF__ */

#ifndef __nvoc_class_id_ComputeInstanceSubscription
#define __nvoc_class_id_ComputeInstanceSubscription 0xd1f238
#endif /* __nvoc_class_id_ComputeInstanceSubscription */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_ComputeInstanceSubscription;

#define __staticCast_ComputeInstanceSubscription(pThis) \
    ((pThis)->__nvoc_pbase_ComputeInstanceSubscription)

#ifdef __nvoc_compute_instance_subscription_h_disabled
#define __dynamicCast_ComputeInstanceSubscription(pThis) ((ComputeInstanceSubscription*)NULL)
#else //__nvoc_compute_instance_subscription_h_disabled
#define __dynamicCast_ComputeInstanceSubscription(pThis) \
    ((ComputeInstanceSubscription*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(ComputeInstanceSubscription)))
#endif //__nvoc_compute_instance_subscription_h_disabled


NV_STATUS __nvoc_objCreateDynamic_ComputeInstanceSubscription(ComputeInstanceSubscription**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_ComputeInstanceSubscription(ComputeInstanceSubscription**, Dynamic*, NvU32, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_ComputeInstanceSubscription(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_ComputeInstanceSubscription((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define cisubscriptionCanCopy(arg0) cisubscriptionCanCopy_DISPATCH(arg0)
#define cisubscriptionCtrlCmdGetUuid(arg0, arg1) cisubscriptionCtrlCmdGetUuid_DISPATCH(arg0, arg1)
#define cisubscriptionShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) cisubscriptionShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define cisubscriptionCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) cisubscriptionCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define cisubscriptionMapTo(pResource, pParams) cisubscriptionMapTo_DISPATCH(pResource, pParams)
#define cisubscriptionGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) cisubscriptionGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define cisubscriptionGetRefCount(pResource) cisubscriptionGetRefCount_DISPATCH(pResource)
#define cisubscriptionAddAdditionalDependants(pClient, pResource, pReference) cisubscriptionAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define cisubscriptionControl_Prologue(pResource, pCallContext, pParams) cisubscriptionControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define cisubscriptionGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) cisubscriptionGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define cisubscriptionInternalControlForward(pGpuResource, command, pParams, size) cisubscriptionInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define cisubscriptionUnmapFrom(pResource, pParams) cisubscriptionUnmapFrom_DISPATCH(pResource, pParams)
#define cisubscriptionControl_Epilogue(pResource, pCallContext, pParams) cisubscriptionControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define cisubscriptionControlLookup(pResource, pParams, ppEntry) cisubscriptionControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define cisubscriptionGetInternalObjectHandle(pGpuResource) cisubscriptionGetInternalObjectHandle_DISPATCH(pGpuResource)
#define cisubscriptionControl(pGpuResource, pCallContext, pParams) cisubscriptionControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define cisubscriptionUnmap(pGpuResource, pCallContext, pCpuMapping) cisubscriptionUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define cisubscriptionGetMemInterMapParams(pRmResource, pParams) cisubscriptionGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define cisubscriptionGetMemoryMappingDescriptor(pRmResource, ppMemDesc) cisubscriptionGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define cisubscriptionControlFilter(pResource, pCallContext, pParams) cisubscriptionControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define cisubscriptionControlSerialization_Prologue(pResource, pCallContext, pParams) cisubscriptionControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define cisubscriptionPreDestruct(pResource) cisubscriptionPreDestruct_DISPATCH(pResource)
#define cisubscriptionIsDuplicate(pResource, hMemory, pDuplicate) cisubscriptionIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define cisubscriptionControlSerialization_Epilogue(pResource, pCallContext, pParams) cisubscriptionControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define cisubscriptionMap(pGpuResource, pCallContext, pParams, pCpuMapping) cisubscriptionMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define cisubscriptionAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) cisubscriptionAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
NvBool cisubscriptionCanCopy_IMPL(struct ComputeInstanceSubscription *arg0);

static inline NvBool cisubscriptionCanCopy_DISPATCH(struct ComputeInstanceSubscription *arg0) {
    return arg0->__cisubscriptionCanCopy__(arg0);
}

NV_STATUS cisubscriptionCtrlCmdGetUuid_IMPL(struct ComputeInstanceSubscription *arg0, NVC638_CTRL_GET_UUID_PARAMS *arg1);

static inline NV_STATUS cisubscriptionCtrlCmdGetUuid_DISPATCH(struct ComputeInstanceSubscription *arg0, NVC638_CTRL_GET_UUID_PARAMS *arg1) {
    return arg0->__cisubscriptionCtrlCmdGetUuid__(arg0, arg1);
}

static inline NvBool cisubscriptionShareCallback_DISPATCH(struct ComputeInstanceSubscription *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__cisubscriptionShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS cisubscriptionCheckMemInterUnmap_DISPATCH(struct ComputeInstanceSubscription *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__cisubscriptionCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS cisubscriptionMapTo_DISPATCH(struct ComputeInstanceSubscription *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__cisubscriptionMapTo__(pResource, pParams);
}

static inline NV_STATUS cisubscriptionGetMapAddrSpace_DISPATCH(struct ComputeInstanceSubscription *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__cisubscriptionGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NvU32 cisubscriptionGetRefCount_DISPATCH(struct ComputeInstanceSubscription *pResource) {
    return pResource->__cisubscriptionGetRefCount__(pResource);
}

static inline void cisubscriptionAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct ComputeInstanceSubscription *pResource, RsResourceRef *pReference) {
    pResource->__cisubscriptionAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS cisubscriptionControl_Prologue_DISPATCH(struct ComputeInstanceSubscription *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__cisubscriptionControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS cisubscriptionGetRegBaseOffsetAndSize_DISPATCH(struct ComputeInstanceSubscription *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__cisubscriptionGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS cisubscriptionInternalControlForward_DISPATCH(struct ComputeInstanceSubscription *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__cisubscriptionInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NV_STATUS cisubscriptionUnmapFrom_DISPATCH(struct ComputeInstanceSubscription *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__cisubscriptionUnmapFrom__(pResource, pParams);
}

static inline void cisubscriptionControl_Epilogue_DISPATCH(struct ComputeInstanceSubscription *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__cisubscriptionControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS cisubscriptionControlLookup_DISPATCH(struct ComputeInstanceSubscription *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__cisubscriptionControlLookup__(pResource, pParams, ppEntry);
}

static inline NvHandle cisubscriptionGetInternalObjectHandle_DISPATCH(struct ComputeInstanceSubscription *pGpuResource) {
    return pGpuResource->__cisubscriptionGetInternalObjectHandle__(pGpuResource);
}

static inline NV_STATUS cisubscriptionControl_DISPATCH(struct ComputeInstanceSubscription *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__cisubscriptionControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS cisubscriptionUnmap_DISPATCH(struct ComputeInstanceSubscription *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__cisubscriptionUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NV_STATUS cisubscriptionGetMemInterMapParams_DISPATCH(struct ComputeInstanceSubscription *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__cisubscriptionGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS cisubscriptionGetMemoryMappingDescriptor_DISPATCH(struct ComputeInstanceSubscription *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__cisubscriptionGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS cisubscriptionControlFilter_DISPATCH(struct ComputeInstanceSubscription *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__cisubscriptionControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS cisubscriptionControlSerialization_Prologue_DISPATCH(struct ComputeInstanceSubscription *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__cisubscriptionControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void cisubscriptionPreDestruct_DISPATCH(struct ComputeInstanceSubscription *pResource) {
    pResource->__cisubscriptionPreDestruct__(pResource);
}

static inline NV_STATUS cisubscriptionIsDuplicate_DISPATCH(struct ComputeInstanceSubscription *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__cisubscriptionIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void cisubscriptionControlSerialization_Epilogue_DISPATCH(struct ComputeInstanceSubscription *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__cisubscriptionControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS cisubscriptionMap_DISPATCH(struct ComputeInstanceSubscription *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__cisubscriptionMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NvBool cisubscriptionAccessCallback_DISPATCH(struct ComputeInstanceSubscription *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__cisubscriptionAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline MIG_COMPUTE_INSTANCE *cisubscriptionGetMIGComputeInstance(struct ComputeInstanceSubscription *pComputeInstanceSubscription) {
    struct ComputeInstanceSubscription_PRIVATE *pComputeInstanceSubscription_PRIVATE = (struct ComputeInstanceSubscription_PRIVATE *)pComputeInstanceSubscription;
    return pComputeInstanceSubscription_PRIVATE->pMIGComputeInstance;
}

NV_STATUS cisubscriptionGetComputeInstanceSubscription_IMPL(struct RsClient *arg0, NvHandle arg1, struct ComputeInstanceSubscription **arg2);

#define cisubscriptionGetComputeInstanceSubscription(arg0, arg1, arg2) cisubscriptionGetComputeInstanceSubscription_IMPL(arg0, arg1, arg2)
NV_STATUS cisubscriptionConstruct_IMPL(struct ComputeInstanceSubscription *arg_pComputeInstanceSubscription, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_cisubscriptionConstruct(arg_pComputeInstanceSubscription, arg_pCallContext, arg_pParams) cisubscriptionConstruct_IMPL(arg_pComputeInstanceSubscription, arg_pCallContext, arg_pParams)
NV_STATUS cisubscriptionCopyConstruct_IMPL(struct ComputeInstanceSubscription *arg0, CALL_CONTEXT *arg1, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg2);

#ifdef __nvoc_compute_instance_subscription_h_disabled
static inline NV_STATUS cisubscriptionCopyConstruct(struct ComputeInstanceSubscription *arg0, CALL_CONTEXT *arg1, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg2) {
    NV_ASSERT_FAILED_PRECOMP("ComputeInstanceSubscription was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_compute_instance_subscription_h_disabled
#define cisubscriptionCopyConstruct(arg0, arg1, arg2) cisubscriptionCopyConstruct_IMPL(arg0, arg1, arg2)
#endif //__nvoc_compute_instance_subscription_h_disabled

void cisubscriptionDestruct_IMPL(struct ComputeInstanceSubscription *arg0);

#define __nvoc_cisubscriptionDestruct(arg0) cisubscriptionDestruct_IMPL(arg0)
#undef PRIVATE_FIELD


#endif // COMPUTE_INSTANCE_SUBSCRIPTION_H


#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_COMPUTE_INSTANCE_SUBSCRIPTION_NVOC_H_
