#ifndef _G_GPU_INSTANCE_SUBSCRIPTION_NVOC_H_
#define _G_GPU_INSTANCE_SUBSCRIPTION_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 *       This file contains the functions managing GPU instance subscriptions
 *
 *****************************************************************************/

#include "g_gpu_instance_subscription_nvoc.h"

#ifndef GPU_INSTANCE_SUBSCRIPTION_H
#define GPU_INSTANCE_SUBSCRIPTION_H

#include "class/clc637.h"
#include "gpu/gpu_resource.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"

// ****************************************************************************
//                          Type Definitions
// ****************************************************************************

#ifdef NVOC_GPU_INSTANCE_SUBSCRIPTION_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct GPUInstanceSubscription {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct GpuResource __nvoc_base_GpuResource;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct GpuResource *__nvoc_pbase_GpuResource;
    struct GPUInstanceSubscription *__nvoc_pbase_GPUInstanceSubscription;
    NvBool (*__gisubscriptionCanCopy__)(struct GPUInstanceSubscription *);
    NV_STATUS (*__gisubscriptionCtrlCmdExecPartitionsCreate__)(struct GPUInstanceSubscription *, NVC637_CTRL_EXEC_PARTITIONS_CREATE_PARAMS *);
    NV_STATUS (*__gisubscriptionCtrlCmdExecPartitionsDelete__)(struct GPUInstanceSubscription *, NVC637_CTRL_EXEC_PARTITIONS_DELETE_PARAMS *);
    NV_STATUS (*__gisubscriptionCtrlCmdExecPartitionsGet__)(struct GPUInstanceSubscription *, NVC637_CTRL_EXEC_PARTITIONS_GET_PARAMS *);
    NV_STATUS (*__gisubscriptionCtrlCmdExecPartitionsGetProfileCapacity__)(struct GPUInstanceSubscription *, NVC637_CTRL_EXEC_PARTITIONS_GET_PROFILE_CAPACITY_PARAMS *);
    NV_STATUS (*__gisubscriptionCtrlCmdExecPartitionsGetActiveIds__)(struct GPUInstanceSubscription *, NVC637_CTRL_EXEC_PARTITIONS_GET_ACTIVE_IDS_PARAMS *);
    NV_STATUS (*__gisubscriptionCtrlCmdExecPartitionsExport__)(struct GPUInstanceSubscription *, NVC637_CTRL_EXEC_PARTITIONS_IMPORT_EXPORT_PARAMS *);
    NV_STATUS (*__gisubscriptionCtrlCmdExecPartitionsImport__)(struct GPUInstanceSubscription *, NVC637_CTRL_EXEC_PARTITIONS_IMPORT_EXPORT_PARAMS *);
    NvBool (*__gisubscriptionShareCallback__)(struct GPUInstanceSubscription *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__gisubscriptionCheckMemInterUnmap__)(struct GPUInstanceSubscription *, NvBool);
    NV_STATUS (*__gisubscriptionMapTo__)(struct GPUInstanceSubscription *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__gisubscriptionGetMapAddrSpace__)(struct GPUInstanceSubscription *, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    NvU32 (*__gisubscriptionGetRefCount__)(struct GPUInstanceSubscription *);
    void (*__gisubscriptionAddAdditionalDependants__)(struct RsClient *, struct GPUInstanceSubscription *, RsResourceRef *);
    NV_STATUS (*__gisubscriptionControl_Prologue__)(struct GPUInstanceSubscription *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__gisubscriptionGetRegBaseOffsetAndSize__)(struct GPUInstanceSubscription *, struct OBJGPU *, NvU32 *, NvU32 *);
    NV_STATUS (*__gisubscriptionInternalControlForward__)(struct GPUInstanceSubscription *, NvU32, void *, NvU32);
    NV_STATUS (*__gisubscriptionUnmapFrom__)(struct GPUInstanceSubscription *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__gisubscriptionControl_Epilogue__)(struct GPUInstanceSubscription *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__gisubscriptionControlLookup__)(struct GPUInstanceSubscription *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NvHandle (*__gisubscriptionGetInternalObjectHandle__)(struct GPUInstanceSubscription *);
    NV_STATUS (*__gisubscriptionControl__)(struct GPUInstanceSubscription *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__gisubscriptionUnmap__)(struct GPUInstanceSubscription *, struct CALL_CONTEXT *, struct RsCpuMapping *);
    NV_STATUS (*__gisubscriptionGetMemInterMapParams__)(struct GPUInstanceSubscription *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__gisubscriptionGetMemoryMappingDescriptor__)(struct GPUInstanceSubscription *, struct MEMORY_DESCRIPTOR **);
    NV_STATUS (*__gisubscriptionControlFilter__)(struct GPUInstanceSubscription *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__gisubscriptionControlSerialization_Prologue__)(struct GPUInstanceSubscription *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    void (*__gisubscriptionPreDestruct__)(struct GPUInstanceSubscription *);
    NV_STATUS (*__gisubscriptionIsDuplicate__)(struct GPUInstanceSubscription *, NvHandle, NvBool *);
    void (*__gisubscriptionControlSerialization_Epilogue__)(struct GPUInstanceSubscription *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__gisubscriptionMap__)(struct GPUInstanceSubscription *, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);
    NvBool (*__gisubscriptionAccessCallback__)(struct GPUInstanceSubscription *, struct RsClient *, void *, RsAccessRight);
    KERNEL_MIG_GPU_INSTANCE *PRIVATE_FIELD(pKernelMIGGpuInstance);
    NvBool PRIVATE_FIELD(bDeviceProfiling);
    NvBool PRIVATE_FIELD(bIsDuped);
    NvU64 PRIVATE_FIELD(dupedCapDescriptor);
};

struct GPUInstanceSubscription_PRIVATE {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct GpuResource __nvoc_base_GpuResource;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct GpuResource *__nvoc_pbase_GpuResource;
    struct GPUInstanceSubscription *__nvoc_pbase_GPUInstanceSubscription;
    NvBool (*__gisubscriptionCanCopy__)(struct GPUInstanceSubscription *);
    NV_STATUS (*__gisubscriptionCtrlCmdExecPartitionsCreate__)(struct GPUInstanceSubscription *, NVC637_CTRL_EXEC_PARTITIONS_CREATE_PARAMS *);
    NV_STATUS (*__gisubscriptionCtrlCmdExecPartitionsDelete__)(struct GPUInstanceSubscription *, NVC637_CTRL_EXEC_PARTITIONS_DELETE_PARAMS *);
    NV_STATUS (*__gisubscriptionCtrlCmdExecPartitionsGet__)(struct GPUInstanceSubscription *, NVC637_CTRL_EXEC_PARTITIONS_GET_PARAMS *);
    NV_STATUS (*__gisubscriptionCtrlCmdExecPartitionsGetProfileCapacity__)(struct GPUInstanceSubscription *, NVC637_CTRL_EXEC_PARTITIONS_GET_PROFILE_CAPACITY_PARAMS *);
    NV_STATUS (*__gisubscriptionCtrlCmdExecPartitionsGetActiveIds__)(struct GPUInstanceSubscription *, NVC637_CTRL_EXEC_PARTITIONS_GET_ACTIVE_IDS_PARAMS *);
    NV_STATUS (*__gisubscriptionCtrlCmdExecPartitionsExport__)(struct GPUInstanceSubscription *, NVC637_CTRL_EXEC_PARTITIONS_IMPORT_EXPORT_PARAMS *);
    NV_STATUS (*__gisubscriptionCtrlCmdExecPartitionsImport__)(struct GPUInstanceSubscription *, NVC637_CTRL_EXEC_PARTITIONS_IMPORT_EXPORT_PARAMS *);
    NvBool (*__gisubscriptionShareCallback__)(struct GPUInstanceSubscription *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__gisubscriptionCheckMemInterUnmap__)(struct GPUInstanceSubscription *, NvBool);
    NV_STATUS (*__gisubscriptionMapTo__)(struct GPUInstanceSubscription *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__gisubscriptionGetMapAddrSpace__)(struct GPUInstanceSubscription *, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    NvU32 (*__gisubscriptionGetRefCount__)(struct GPUInstanceSubscription *);
    void (*__gisubscriptionAddAdditionalDependants__)(struct RsClient *, struct GPUInstanceSubscription *, RsResourceRef *);
    NV_STATUS (*__gisubscriptionControl_Prologue__)(struct GPUInstanceSubscription *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__gisubscriptionGetRegBaseOffsetAndSize__)(struct GPUInstanceSubscription *, struct OBJGPU *, NvU32 *, NvU32 *);
    NV_STATUS (*__gisubscriptionInternalControlForward__)(struct GPUInstanceSubscription *, NvU32, void *, NvU32);
    NV_STATUS (*__gisubscriptionUnmapFrom__)(struct GPUInstanceSubscription *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__gisubscriptionControl_Epilogue__)(struct GPUInstanceSubscription *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__gisubscriptionControlLookup__)(struct GPUInstanceSubscription *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NvHandle (*__gisubscriptionGetInternalObjectHandle__)(struct GPUInstanceSubscription *);
    NV_STATUS (*__gisubscriptionControl__)(struct GPUInstanceSubscription *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__gisubscriptionUnmap__)(struct GPUInstanceSubscription *, struct CALL_CONTEXT *, struct RsCpuMapping *);
    NV_STATUS (*__gisubscriptionGetMemInterMapParams__)(struct GPUInstanceSubscription *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__gisubscriptionGetMemoryMappingDescriptor__)(struct GPUInstanceSubscription *, struct MEMORY_DESCRIPTOR **);
    NV_STATUS (*__gisubscriptionControlFilter__)(struct GPUInstanceSubscription *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__gisubscriptionControlSerialization_Prologue__)(struct GPUInstanceSubscription *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    void (*__gisubscriptionPreDestruct__)(struct GPUInstanceSubscription *);
    NV_STATUS (*__gisubscriptionIsDuplicate__)(struct GPUInstanceSubscription *, NvHandle, NvBool *);
    void (*__gisubscriptionControlSerialization_Epilogue__)(struct GPUInstanceSubscription *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__gisubscriptionMap__)(struct GPUInstanceSubscription *, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);
    NvBool (*__gisubscriptionAccessCallback__)(struct GPUInstanceSubscription *, struct RsClient *, void *, RsAccessRight);
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance;
    NvBool bDeviceProfiling;
    NvBool bIsDuped;
    NvU64 dupedCapDescriptor;
};

#ifndef __NVOC_CLASS_GPUInstanceSubscription_TYPEDEF__
#define __NVOC_CLASS_GPUInstanceSubscription_TYPEDEF__
typedef struct GPUInstanceSubscription GPUInstanceSubscription;
#endif /* __NVOC_CLASS_GPUInstanceSubscription_TYPEDEF__ */

#ifndef __nvoc_class_id_GPUInstanceSubscription
#define __nvoc_class_id_GPUInstanceSubscription 0x91fde7
#endif /* __nvoc_class_id_GPUInstanceSubscription */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GPUInstanceSubscription;

#define __staticCast_GPUInstanceSubscription(pThis) \
    ((pThis)->__nvoc_pbase_GPUInstanceSubscription)

#ifdef __nvoc_gpu_instance_subscription_h_disabled
#define __dynamicCast_GPUInstanceSubscription(pThis) ((GPUInstanceSubscription*)NULL)
#else //__nvoc_gpu_instance_subscription_h_disabled
#define __dynamicCast_GPUInstanceSubscription(pThis) \
    ((GPUInstanceSubscription*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(GPUInstanceSubscription)))
#endif //__nvoc_gpu_instance_subscription_h_disabled


NV_STATUS __nvoc_objCreateDynamic_GPUInstanceSubscription(GPUInstanceSubscription**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_GPUInstanceSubscription(GPUInstanceSubscription**, Dynamic*, NvU32, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_GPUInstanceSubscription(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_GPUInstanceSubscription((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define gisubscriptionCanCopy(arg0) gisubscriptionCanCopy_DISPATCH(arg0)
#define gisubscriptionCtrlCmdExecPartitionsCreate(arg0, arg1) gisubscriptionCtrlCmdExecPartitionsCreate_DISPATCH(arg0, arg1)
#define gisubscriptionCtrlCmdExecPartitionsDelete(arg0, arg1) gisubscriptionCtrlCmdExecPartitionsDelete_DISPATCH(arg0, arg1)
#define gisubscriptionCtrlCmdExecPartitionsGet(arg0, arg1) gisubscriptionCtrlCmdExecPartitionsGet_DISPATCH(arg0, arg1)
#define gisubscriptionCtrlCmdExecPartitionsGetProfileCapacity(arg0, arg1) gisubscriptionCtrlCmdExecPartitionsGetProfileCapacity_DISPATCH(arg0, arg1)
#define gisubscriptionCtrlCmdExecPartitionsGetActiveIds(arg0, arg1) gisubscriptionCtrlCmdExecPartitionsGetActiveIds_DISPATCH(arg0, arg1)
#define gisubscriptionCtrlCmdExecPartitionsExport(arg0, arg1) gisubscriptionCtrlCmdExecPartitionsExport_DISPATCH(arg0, arg1)
#define gisubscriptionCtrlCmdExecPartitionsImport(arg0, arg1) gisubscriptionCtrlCmdExecPartitionsImport_DISPATCH(arg0, arg1)
#define gisubscriptionShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) gisubscriptionShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define gisubscriptionCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) gisubscriptionCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define gisubscriptionMapTo(pResource, pParams) gisubscriptionMapTo_DISPATCH(pResource, pParams)
#define gisubscriptionGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) gisubscriptionGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define gisubscriptionGetRefCount(pResource) gisubscriptionGetRefCount_DISPATCH(pResource)
#define gisubscriptionAddAdditionalDependants(pClient, pResource, pReference) gisubscriptionAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define gisubscriptionControl_Prologue(pResource, pCallContext, pParams) gisubscriptionControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define gisubscriptionGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) gisubscriptionGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define gisubscriptionInternalControlForward(pGpuResource, command, pParams, size) gisubscriptionInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define gisubscriptionUnmapFrom(pResource, pParams) gisubscriptionUnmapFrom_DISPATCH(pResource, pParams)
#define gisubscriptionControl_Epilogue(pResource, pCallContext, pParams) gisubscriptionControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define gisubscriptionControlLookup(pResource, pParams, ppEntry) gisubscriptionControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define gisubscriptionGetInternalObjectHandle(pGpuResource) gisubscriptionGetInternalObjectHandle_DISPATCH(pGpuResource)
#define gisubscriptionControl(pGpuResource, pCallContext, pParams) gisubscriptionControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define gisubscriptionUnmap(pGpuResource, pCallContext, pCpuMapping) gisubscriptionUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define gisubscriptionGetMemInterMapParams(pRmResource, pParams) gisubscriptionGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define gisubscriptionGetMemoryMappingDescriptor(pRmResource, ppMemDesc) gisubscriptionGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define gisubscriptionControlFilter(pResource, pCallContext, pParams) gisubscriptionControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define gisubscriptionControlSerialization_Prologue(pResource, pCallContext, pParams) gisubscriptionControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define gisubscriptionPreDestruct(pResource) gisubscriptionPreDestruct_DISPATCH(pResource)
#define gisubscriptionIsDuplicate(pResource, hMemory, pDuplicate) gisubscriptionIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define gisubscriptionControlSerialization_Epilogue(pResource, pCallContext, pParams) gisubscriptionControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define gisubscriptionMap(pGpuResource, pCallContext, pParams, pCpuMapping) gisubscriptionMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define gisubscriptionAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) gisubscriptionAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
NvBool gisubscriptionCanCopy_IMPL(struct GPUInstanceSubscription *arg0);

static inline NvBool gisubscriptionCanCopy_DISPATCH(struct GPUInstanceSubscription *arg0) {
    return arg0->__gisubscriptionCanCopy__(arg0);
}

NV_STATUS gisubscriptionCtrlCmdExecPartitionsCreate_IMPL(struct GPUInstanceSubscription *arg0, NVC637_CTRL_EXEC_PARTITIONS_CREATE_PARAMS *arg1);

static inline NV_STATUS gisubscriptionCtrlCmdExecPartitionsCreate_DISPATCH(struct GPUInstanceSubscription *arg0, NVC637_CTRL_EXEC_PARTITIONS_CREATE_PARAMS *arg1) {
    return arg0->__gisubscriptionCtrlCmdExecPartitionsCreate__(arg0, arg1);
}

NV_STATUS gisubscriptionCtrlCmdExecPartitionsDelete_IMPL(struct GPUInstanceSubscription *arg0, NVC637_CTRL_EXEC_PARTITIONS_DELETE_PARAMS *arg1);

static inline NV_STATUS gisubscriptionCtrlCmdExecPartitionsDelete_DISPATCH(struct GPUInstanceSubscription *arg0, NVC637_CTRL_EXEC_PARTITIONS_DELETE_PARAMS *arg1) {
    return arg0->__gisubscriptionCtrlCmdExecPartitionsDelete__(arg0, arg1);
}

NV_STATUS gisubscriptionCtrlCmdExecPartitionsGet_IMPL(struct GPUInstanceSubscription *arg0, NVC637_CTRL_EXEC_PARTITIONS_GET_PARAMS *arg1);

static inline NV_STATUS gisubscriptionCtrlCmdExecPartitionsGet_DISPATCH(struct GPUInstanceSubscription *arg0, NVC637_CTRL_EXEC_PARTITIONS_GET_PARAMS *arg1) {
    return arg0->__gisubscriptionCtrlCmdExecPartitionsGet__(arg0, arg1);
}

NV_STATUS gisubscriptionCtrlCmdExecPartitionsGetProfileCapacity_IMPL(struct GPUInstanceSubscription *arg0, NVC637_CTRL_EXEC_PARTITIONS_GET_PROFILE_CAPACITY_PARAMS *arg1);

static inline NV_STATUS gisubscriptionCtrlCmdExecPartitionsGetProfileCapacity_DISPATCH(struct GPUInstanceSubscription *arg0, NVC637_CTRL_EXEC_PARTITIONS_GET_PROFILE_CAPACITY_PARAMS *arg1) {
    return arg0->__gisubscriptionCtrlCmdExecPartitionsGetProfileCapacity__(arg0, arg1);
}

NV_STATUS gisubscriptionCtrlCmdExecPartitionsGetActiveIds_IMPL(struct GPUInstanceSubscription *arg0, NVC637_CTRL_EXEC_PARTITIONS_GET_ACTIVE_IDS_PARAMS *arg1);

static inline NV_STATUS gisubscriptionCtrlCmdExecPartitionsGetActiveIds_DISPATCH(struct GPUInstanceSubscription *arg0, NVC637_CTRL_EXEC_PARTITIONS_GET_ACTIVE_IDS_PARAMS *arg1) {
    return arg0->__gisubscriptionCtrlCmdExecPartitionsGetActiveIds__(arg0, arg1);
}

NV_STATUS gisubscriptionCtrlCmdExecPartitionsExport_IMPL(struct GPUInstanceSubscription *arg0, NVC637_CTRL_EXEC_PARTITIONS_IMPORT_EXPORT_PARAMS *arg1);

static inline NV_STATUS gisubscriptionCtrlCmdExecPartitionsExport_DISPATCH(struct GPUInstanceSubscription *arg0, NVC637_CTRL_EXEC_PARTITIONS_IMPORT_EXPORT_PARAMS *arg1) {
    return arg0->__gisubscriptionCtrlCmdExecPartitionsExport__(arg0, arg1);
}

NV_STATUS gisubscriptionCtrlCmdExecPartitionsImport_IMPL(struct GPUInstanceSubscription *arg0, NVC637_CTRL_EXEC_PARTITIONS_IMPORT_EXPORT_PARAMS *arg1);

static inline NV_STATUS gisubscriptionCtrlCmdExecPartitionsImport_DISPATCH(struct GPUInstanceSubscription *arg0, NVC637_CTRL_EXEC_PARTITIONS_IMPORT_EXPORT_PARAMS *arg1) {
    return arg0->__gisubscriptionCtrlCmdExecPartitionsImport__(arg0, arg1);
}

static inline NvBool gisubscriptionShareCallback_DISPATCH(struct GPUInstanceSubscription *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__gisubscriptionShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS gisubscriptionCheckMemInterUnmap_DISPATCH(struct GPUInstanceSubscription *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__gisubscriptionCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS gisubscriptionMapTo_DISPATCH(struct GPUInstanceSubscription *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__gisubscriptionMapTo__(pResource, pParams);
}

static inline NV_STATUS gisubscriptionGetMapAddrSpace_DISPATCH(struct GPUInstanceSubscription *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__gisubscriptionGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NvU32 gisubscriptionGetRefCount_DISPATCH(struct GPUInstanceSubscription *pResource) {
    return pResource->__gisubscriptionGetRefCount__(pResource);
}

static inline void gisubscriptionAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct GPUInstanceSubscription *pResource, RsResourceRef *pReference) {
    pResource->__gisubscriptionAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS gisubscriptionControl_Prologue_DISPATCH(struct GPUInstanceSubscription *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__gisubscriptionControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS gisubscriptionGetRegBaseOffsetAndSize_DISPATCH(struct GPUInstanceSubscription *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__gisubscriptionGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS gisubscriptionInternalControlForward_DISPATCH(struct GPUInstanceSubscription *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__gisubscriptionInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NV_STATUS gisubscriptionUnmapFrom_DISPATCH(struct GPUInstanceSubscription *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__gisubscriptionUnmapFrom__(pResource, pParams);
}

static inline void gisubscriptionControl_Epilogue_DISPATCH(struct GPUInstanceSubscription *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__gisubscriptionControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS gisubscriptionControlLookup_DISPATCH(struct GPUInstanceSubscription *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__gisubscriptionControlLookup__(pResource, pParams, ppEntry);
}

static inline NvHandle gisubscriptionGetInternalObjectHandle_DISPATCH(struct GPUInstanceSubscription *pGpuResource) {
    return pGpuResource->__gisubscriptionGetInternalObjectHandle__(pGpuResource);
}

static inline NV_STATUS gisubscriptionControl_DISPATCH(struct GPUInstanceSubscription *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__gisubscriptionControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS gisubscriptionUnmap_DISPATCH(struct GPUInstanceSubscription *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__gisubscriptionUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NV_STATUS gisubscriptionGetMemInterMapParams_DISPATCH(struct GPUInstanceSubscription *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__gisubscriptionGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS gisubscriptionGetMemoryMappingDescriptor_DISPATCH(struct GPUInstanceSubscription *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__gisubscriptionGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS gisubscriptionControlFilter_DISPATCH(struct GPUInstanceSubscription *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__gisubscriptionControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS gisubscriptionControlSerialization_Prologue_DISPATCH(struct GPUInstanceSubscription *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__gisubscriptionControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void gisubscriptionPreDestruct_DISPATCH(struct GPUInstanceSubscription *pResource) {
    pResource->__gisubscriptionPreDestruct__(pResource);
}

static inline NV_STATUS gisubscriptionIsDuplicate_DISPATCH(struct GPUInstanceSubscription *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__gisubscriptionIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void gisubscriptionControlSerialization_Epilogue_DISPATCH(struct GPUInstanceSubscription *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__gisubscriptionControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS gisubscriptionMap_DISPATCH(struct GPUInstanceSubscription *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__gisubscriptionMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NvBool gisubscriptionAccessCallback_DISPATCH(struct GPUInstanceSubscription *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__gisubscriptionAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool gisubscriptionIsDeviceProfiling(struct GPUInstanceSubscription *pGPUInstanceSubscription) {
    struct GPUInstanceSubscription_PRIVATE *pGPUInstanceSubscription_PRIVATE = (struct GPUInstanceSubscription_PRIVATE *)pGPUInstanceSubscription;
    return pGPUInstanceSubscription_PRIVATE->bDeviceProfiling;
}

static inline KERNEL_MIG_GPU_INSTANCE *gisubscriptionGetMIGGPUInstance(struct GPUInstanceSubscription *pGPUInstanceSubscription) {
    struct GPUInstanceSubscription_PRIVATE *pGPUInstanceSubscription_PRIVATE = (struct GPUInstanceSubscription_PRIVATE *)pGPUInstanceSubscription;
    return pGPUInstanceSubscription_PRIVATE->pKernelMIGGpuInstance;
}

NV_STATUS gisubscriptionGetGPUInstanceSubscription_IMPL(struct RsClient *arg0, NvHandle arg1, struct GPUInstanceSubscription **arg2);

#define gisubscriptionGetGPUInstanceSubscription(arg0, arg1, arg2) gisubscriptionGetGPUInstanceSubscription_IMPL(arg0, arg1, arg2)
NvBool gisubscriptionShouldClassBeFreedOnUnsubscribe_IMPL(NvU32 internalClassId);

#define gisubscriptionShouldClassBeFreedOnUnsubscribe(internalClassId) gisubscriptionShouldClassBeFreedOnUnsubscribe_IMPL(internalClassId)
void gisubscriptionCleanupOnUnsubscribe_IMPL(CALL_CONTEXT *arg0);

#define gisubscriptionCleanupOnUnsubscribe(arg0) gisubscriptionCleanupOnUnsubscribe_IMPL(arg0)
NV_STATUS gisubscriptionConstruct_IMPL(struct GPUInstanceSubscription *arg_pGPUInstanceSubscription, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_gisubscriptionConstruct(arg_pGPUInstanceSubscription, arg_pCallContext, arg_pParams) gisubscriptionConstruct_IMPL(arg_pGPUInstanceSubscription, arg_pCallContext, arg_pParams)
NV_STATUS gisubscriptionCopyConstruct_IMPL(struct GPUInstanceSubscription *arg0, CALL_CONTEXT *arg1, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg2);

#ifdef __nvoc_gpu_instance_subscription_h_disabled
static inline NV_STATUS gisubscriptionCopyConstruct(struct GPUInstanceSubscription *arg0, CALL_CONTEXT *arg1, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg2) {
    NV_ASSERT_FAILED_PRECOMP("GPUInstanceSubscription was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_instance_subscription_h_disabled
#define gisubscriptionCopyConstruct(arg0, arg1, arg2) gisubscriptionCopyConstruct_IMPL(arg0, arg1, arg2)
#endif //__nvoc_gpu_instance_subscription_h_disabled

void gisubscriptionDestruct_IMPL(struct GPUInstanceSubscription *arg0);

#define __nvoc_gisubscriptionDestruct(arg0) gisubscriptionDestruct_IMPL(arg0)
NvBool gisubscriptionIsDuped_IMPL(struct GPUInstanceSubscription *arg0);

#ifdef __nvoc_gpu_instance_subscription_h_disabled
static inline NvBool gisubscriptionIsDuped(struct GPUInstanceSubscription *arg0) {
    NV_ASSERT_FAILED_PRECOMP("GPUInstanceSubscription was disabled!");
    return NV_FALSE;
}
#else //__nvoc_gpu_instance_subscription_h_disabled
#define gisubscriptionIsDuped(arg0) gisubscriptionIsDuped_IMPL(arg0)
#endif //__nvoc_gpu_instance_subscription_h_disabled

#undef PRIVATE_FIELD


#endif // GPU_INSTANCE_SUBSCRIPTION_H


#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_GPU_INSTANCE_SUBSCRIPTION_NVOC_H_
