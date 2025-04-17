
#ifndef _G_GPU_INSTANCE_SUBSCRIPTION_NVOC_H_
#define _G_GPU_INSTANCE_SUBSCRIPTION_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2018-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#pragma once
#include "g_gpu_instance_subscription_nvoc.h"

#ifndef GPU_INSTANCE_SUBSCRIPTION_H
#define GPU_INSTANCE_SUBSCRIPTION_H

#include "class/clc637.h"
#include "gpu/gpu_resource.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"

// ****************************************************************************
//                          Type Definitions
// ****************************************************************************


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_GPU_INSTANCE_SUBSCRIPTION_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__GPUInstanceSubscription;
struct NVOC_METADATA__GpuResource;
struct NVOC_VTABLE__GPUInstanceSubscription;


struct GPUInstanceSubscription {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__GPUInstanceSubscription *__nvoc_metadata_ptr;
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
    struct GPUInstanceSubscription *__nvoc_pbase_GPUInstanceSubscription;    // gisubscription

    // Vtable with 8 per-object function pointers
    NV_STATUS (*__gisubscriptionCtrlCmdExecPartitionsCreate__)(struct GPUInstanceSubscription * /*this*/, NVC637_CTRL_EXEC_PARTITIONS_CREATE_PARAMS *);  // exported (id=0xc6370101)
    NV_STATUS (*__gisubscriptionCtrlCmdExecPartitionsDelete__)(struct GPUInstanceSubscription * /*this*/, NVC637_CTRL_EXEC_PARTITIONS_DELETE_PARAMS *);  // exported (id=0xc6370102)
    NV_STATUS (*__gisubscriptionCtrlCmdExecPartitionsGet__)(struct GPUInstanceSubscription * /*this*/, NVC637_CTRL_EXEC_PARTITIONS_GET_PARAMS *);  // exported (id=0xc6370103)
    NV_STATUS (*__gisubscriptionCtrlCmdExecPartitionsGetProfileCapacity__)(struct GPUInstanceSubscription * /*this*/, NVC637_CTRL_EXEC_PARTITIONS_GET_PROFILE_CAPACITY_PARAMS *);  // exported (id=0xc63701a9)
    NV_STATUS (*__gisubscriptionCtrlCmdExecPartitionsGetActiveIds__)(struct GPUInstanceSubscription * /*this*/, NVC637_CTRL_EXEC_PARTITIONS_GET_ACTIVE_IDS_PARAMS *);  // exported (id=0xc6370104)
    NV_STATUS (*__gisubscriptionCtrlCmdExecPartitionsExport__)(struct GPUInstanceSubscription * /*this*/, NVC637_CTRL_EXEC_PARTITIONS_IMPORT_EXPORT_PARAMS *);  // exported (id=0xc6370105)
    NV_STATUS (*__gisubscriptionCtrlCmdExecPartitionsImport__)(struct GPUInstanceSubscription * /*this*/, NVC637_CTRL_EXEC_PARTITIONS_IMPORT_EXPORT_PARAMS *);  // exported (id=0xc6370106)
    NV_STATUS (*__gisubscriptionCtrlCmdGetUuid__)(struct GPUInstanceSubscription * /*this*/, NVC637_CTRL_GET_UUID_PARAMS *);  // exported (id=0xc63701aa)

    // Data members
    KERNEL_MIG_GPU_INSTANCE *PRIVATE_FIELD(pKernelMIGGpuInstance);
    NvBool PRIVATE_FIELD(bDeviceProfiling);
    NvBool PRIVATE_FIELD(bIsDuped);
    NvU64 PRIVATE_FIELD(dupedCapDescriptor);
};


struct GPUInstanceSubscription_PRIVATE {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__GPUInstanceSubscription *__nvoc_metadata_ptr;
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
    struct GPUInstanceSubscription *__nvoc_pbase_GPUInstanceSubscription;    // gisubscription

    // Vtable with 8 per-object function pointers
    NV_STATUS (*__gisubscriptionCtrlCmdExecPartitionsCreate__)(struct GPUInstanceSubscription * /*this*/, NVC637_CTRL_EXEC_PARTITIONS_CREATE_PARAMS *);  // exported (id=0xc6370101)
    NV_STATUS (*__gisubscriptionCtrlCmdExecPartitionsDelete__)(struct GPUInstanceSubscription * /*this*/, NVC637_CTRL_EXEC_PARTITIONS_DELETE_PARAMS *);  // exported (id=0xc6370102)
    NV_STATUS (*__gisubscriptionCtrlCmdExecPartitionsGet__)(struct GPUInstanceSubscription * /*this*/, NVC637_CTRL_EXEC_PARTITIONS_GET_PARAMS *);  // exported (id=0xc6370103)
    NV_STATUS (*__gisubscriptionCtrlCmdExecPartitionsGetProfileCapacity__)(struct GPUInstanceSubscription * /*this*/, NVC637_CTRL_EXEC_PARTITIONS_GET_PROFILE_CAPACITY_PARAMS *);  // exported (id=0xc63701a9)
    NV_STATUS (*__gisubscriptionCtrlCmdExecPartitionsGetActiveIds__)(struct GPUInstanceSubscription * /*this*/, NVC637_CTRL_EXEC_PARTITIONS_GET_ACTIVE_IDS_PARAMS *);  // exported (id=0xc6370104)
    NV_STATUS (*__gisubscriptionCtrlCmdExecPartitionsExport__)(struct GPUInstanceSubscription * /*this*/, NVC637_CTRL_EXEC_PARTITIONS_IMPORT_EXPORT_PARAMS *);  // exported (id=0xc6370105)
    NV_STATUS (*__gisubscriptionCtrlCmdExecPartitionsImport__)(struct GPUInstanceSubscription * /*this*/, NVC637_CTRL_EXEC_PARTITIONS_IMPORT_EXPORT_PARAMS *);  // exported (id=0xc6370106)
    NV_STATUS (*__gisubscriptionCtrlCmdGetUuid__)(struct GPUInstanceSubscription * /*this*/, NVC637_CTRL_GET_UUID_PARAMS *);  // exported (id=0xc63701aa)

    // Data members
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance;
    NvBool bDeviceProfiling;
    NvBool bIsDuped;
    NvU64 dupedCapDescriptor;
};


// Vtable with 25 per-class function pointers
struct NVOC_VTABLE__GPUInstanceSubscription {
    NvBool (*__gisubscriptionCanCopy__)(struct GPUInstanceSubscription * /*this*/);  // virtual override (res) base (gpures)
    NV_STATUS (*__gisubscriptionControl__)(struct GPUInstanceSubscription * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__gisubscriptionMap__)(struct GPUInstanceSubscription * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__gisubscriptionUnmap__)(struct GPUInstanceSubscription * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NvBool (*__gisubscriptionShareCallback__)(struct GPUInstanceSubscription * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__gisubscriptionGetRegBaseOffsetAndSize__)(struct GPUInstanceSubscription * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__gisubscriptionGetMapAddrSpace__)(struct GPUInstanceSubscription * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__gisubscriptionInternalControlForward__)(struct GPUInstanceSubscription * /*this*/, NvU32, void *, NvU32);  // virtual inherited (gpures) base (gpures)
    NvHandle (*__gisubscriptionGetInternalObjectHandle__)(struct GPUInstanceSubscription * /*this*/);  // virtual inherited (gpures) base (gpures)
    NvBool (*__gisubscriptionAccessCallback__)(struct GPUInstanceSubscription * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__gisubscriptionGetMemInterMapParams__)(struct GPUInstanceSubscription * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__gisubscriptionCheckMemInterUnmap__)(struct GPUInstanceSubscription * /*this*/, NvBool);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__gisubscriptionGetMemoryMappingDescriptor__)(struct GPUInstanceSubscription * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__gisubscriptionControlSerialization_Prologue__)(struct GPUInstanceSubscription * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__gisubscriptionControlSerialization_Epilogue__)(struct GPUInstanceSubscription * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__gisubscriptionControl_Prologue__)(struct GPUInstanceSubscription * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__gisubscriptionControl_Epilogue__)(struct GPUInstanceSubscription * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__gisubscriptionIsDuplicate__)(struct GPUInstanceSubscription * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (gpures)
    void (*__gisubscriptionPreDestruct__)(struct GPUInstanceSubscription * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__gisubscriptionControlFilter__)(struct GPUInstanceSubscription * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (gpures)
    NvBool (*__gisubscriptionIsPartialUnmapSupported__)(struct GPUInstanceSubscription * /*this*/);  // inline virtual inherited (res) base (gpures) body
    NV_STATUS (*__gisubscriptionMapTo__)(struct GPUInstanceSubscription * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__gisubscriptionUnmapFrom__)(struct GPUInstanceSubscription * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (gpures)
    NvU32 (*__gisubscriptionGetRefCount__)(struct GPUInstanceSubscription * /*this*/);  // virtual inherited (res) base (gpures)
    void (*__gisubscriptionAddAdditionalDependants__)(struct RsClient *, struct GPUInstanceSubscription * /*this*/, RsResourceRef *);  // virtual inherited (res) base (gpures)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__GPUInstanceSubscription {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResource metadata__GpuResource;
    const struct NVOC_VTABLE__GPUInstanceSubscription vtable;
};

#ifndef __NVOC_CLASS_GPUInstanceSubscription_TYPEDEF__
#define __NVOC_CLASS_GPUInstanceSubscription_TYPEDEF__
typedef struct GPUInstanceSubscription GPUInstanceSubscription;
#endif /* __NVOC_CLASS_GPUInstanceSubscription_TYPEDEF__ */

#ifndef __nvoc_class_id_GPUInstanceSubscription
#define __nvoc_class_id_GPUInstanceSubscription 0x91fde7
#endif /* __nvoc_class_id_GPUInstanceSubscription */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GPUInstanceSubscription;

#define __staticCast_GPUInstanceSubscription(pThis) \
    ((pThis)->__nvoc_pbase_GPUInstanceSubscription)

#ifdef __nvoc_gpu_instance_subscription_h_disabled
#define __dynamicCast_GPUInstanceSubscription(pThis) ((GPUInstanceSubscription*) NULL)
#else //__nvoc_gpu_instance_subscription_h_disabled
#define __dynamicCast_GPUInstanceSubscription(pThis) \
    ((GPUInstanceSubscription*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(GPUInstanceSubscription)))
#endif //__nvoc_gpu_instance_subscription_h_disabled

NV_STATUS __nvoc_objCreateDynamic_GPUInstanceSubscription(GPUInstanceSubscription**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_GPUInstanceSubscription(GPUInstanceSubscription**, Dynamic*, NvU32, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __objCreate_GPUInstanceSubscription(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_GPUInstanceSubscription((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define gisubscriptionCanCopy_FNPTR(arg_this) arg_this->__nvoc_metadata_ptr->vtable.__gisubscriptionCanCopy__
#define gisubscriptionCanCopy(arg_this) gisubscriptionCanCopy_DISPATCH(arg_this)
#define gisubscriptionCtrlCmdExecPartitionsCreate_FNPTR(arg_this) arg_this->__gisubscriptionCtrlCmdExecPartitionsCreate__
#define gisubscriptionCtrlCmdExecPartitionsCreate(arg_this, arg2) gisubscriptionCtrlCmdExecPartitionsCreate_DISPATCH(arg_this, arg2)
#define gisubscriptionCtrlCmdExecPartitionsDelete_FNPTR(arg_this) arg_this->__gisubscriptionCtrlCmdExecPartitionsDelete__
#define gisubscriptionCtrlCmdExecPartitionsDelete(arg_this, arg2) gisubscriptionCtrlCmdExecPartitionsDelete_DISPATCH(arg_this, arg2)
#define gisubscriptionCtrlCmdExecPartitionsGet_FNPTR(arg_this) arg_this->__gisubscriptionCtrlCmdExecPartitionsGet__
#define gisubscriptionCtrlCmdExecPartitionsGet(arg_this, arg2) gisubscriptionCtrlCmdExecPartitionsGet_DISPATCH(arg_this, arg2)
#define gisubscriptionCtrlCmdExecPartitionsGetProfileCapacity_FNPTR(arg_this) arg_this->__gisubscriptionCtrlCmdExecPartitionsGetProfileCapacity__
#define gisubscriptionCtrlCmdExecPartitionsGetProfileCapacity(arg_this, arg2) gisubscriptionCtrlCmdExecPartitionsGetProfileCapacity_DISPATCH(arg_this, arg2)
#define gisubscriptionCtrlCmdExecPartitionsGetActiveIds_FNPTR(arg_this) arg_this->__gisubscriptionCtrlCmdExecPartitionsGetActiveIds__
#define gisubscriptionCtrlCmdExecPartitionsGetActiveIds(arg_this, arg2) gisubscriptionCtrlCmdExecPartitionsGetActiveIds_DISPATCH(arg_this, arg2)
#define gisubscriptionCtrlCmdExecPartitionsExport_FNPTR(arg_this) arg_this->__gisubscriptionCtrlCmdExecPartitionsExport__
#define gisubscriptionCtrlCmdExecPartitionsExport(arg_this, arg2) gisubscriptionCtrlCmdExecPartitionsExport_DISPATCH(arg_this, arg2)
#define gisubscriptionCtrlCmdExecPartitionsImport_FNPTR(arg_this) arg_this->__gisubscriptionCtrlCmdExecPartitionsImport__
#define gisubscriptionCtrlCmdExecPartitionsImport(arg_this, arg2) gisubscriptionCtrlCmdExecPartitionsImport_DISPATCH(arg_this, arg2)
#define gisubscriptionCtrlCmdGetUuid_FNPTR(arg_this) arg_this->__gisubscriptionCtrlCmdGetUuid__
#define gisubscriptionCtrlCmdGetUuid(arg_this, arg2) gisubscriptionCtrlCmdGetUuid_DISPATCH(arg_this, arg2)
#define gisubscriptionControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresControl__
#define gisubscriptionControl(pGpuResource, pCallContext, pParams) gisubscriptionControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define gisubscriptionMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresMap__
#define gisubscriptionMap(pGpuResource, pCallContext, pParams, pCpuMapping) gisubscriptionMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define gisubscriptionUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresUnmap__
#define gisubscriptionUnmap(pGpuResource, pCallContext, pCpuMapping) gisubscriptionUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define gisubscriptionShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresShareCallback__
#define gisubscriptionShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) gisubscriptionShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define gisubscriptionGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__
#define gisubscriptionGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) gisubscriptionGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define gisubscriptionGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__
#define gisubscriptionGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) gisubscriptionGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define gisubscriptionInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__
#define gisubscriptionInternalControlForward(pGpuResource, command, pParams, size) gisubscriptionInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define gisubscriptionGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__
#define gisubscriptionGetInternalObjectHandle(pGpuResource) gisubscriptionGetInternalObjectHandle_DISPATCH(pGpuResource)
#define gisubscriptionAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresAccessCallback__
#define gisubscriptionAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) gisubscriptionAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define gisubscriptionGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define gisubscriptionGetMemInterMapParams(pRmResource, pParams) gisubscriptionGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define gisubscriptionCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define gisubscriptionCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) gisubscriptionCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define gisubscriptionGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define gisubscriptionGetMemoryMappingDescriptor(pRmResource, ppMemDesc) gisubscriptionGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define gisubscriptionControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Prologue__
#define gisubscriptionControlSerialization_Prologue(pResource, pCallContext, pParams) gisubscriptionControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define gisubscriptionControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Epilogue__
#define gisubscriptionControlSerialization_Epilogue(pResource, pCallContext, pParams) gisubscriptionControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define gisubscriptionControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Prologue__
#define gisubscriptionControl_Prologue(pResource, pCallContext, pParams) gisubscriptionControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define gisubscriptionControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Epilogue__
#define gisubscriptionControl_Epilogue(pResource, pCallContext, pParams) gisubscriptionControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define gisubscriptionIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define gisubscriptionIsDuplicate(pResource, hMemory, pDuplicate) gisubscriptionIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define gisubscriptionPreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define gisubscriptionPreDestruct(pResource) gisubscriptionPreDestruct_DISPATCH(pResource)
#define gisubscriptionControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define gisubscriptionControlFilter(pResource, pCallContext, pParams) gisubscriptionControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define gisubscriptionIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define gisubscriptionIsPartialUnmapSupported(pResource) gisubscriptionIsPartialUnmapSupported_DISPATCH(pResource)
#define gisubscriptionMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define gisubscriptionMapTo(pResource, pParams) gisubscriptionMapTo_DISPATCH(pResource, pParams)
#define gisubscriptionUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define gisubscriptionUnmapFrom(pResource, pParams) gisubscriptionUnmapFrom_DISPATCH(pResource, pParams)
#define gisubscriptionGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define gisubscriptionGetRefCount(pResource) gisubscriptionGetRefCount_DISPATCH(pResource)
#define gisubscriptionAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define gisubscriptionAddAdditionalDependants(pClient, pResource, pReference) gisubscriptionAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NvBool gisubscriptionCanCopy_DISPATCH(struct GPUInstanceSubscription *arg_this) {
    return arg_this->__nvoc_metadata_ptr->vtable.__gisubscriptionCanCopy__(arg_this);
}

static inline NV_STATUS gisubscriptionCtrlCmdExecPartitionsCreate_DISPATCH(struct GPUInstanceSubscription *arg_this, NVC637_CTRL_EXEC_PARTITIONS_CREATE_PARAMS *arg2) {
    return arg_this->__gisubscriptionCtrlCmdExecPartitionsCreate__(arg_this, arg2);
}

static inline NV_STATUS gisubscriptionCtrlCmdExecPartitionsDelete_DISPATCH(struct GPUInstanceSubscription *arg_this, NVC637_CTRL_EXEC_PARTITIONS_DELETE_PARAMS *arg2) {
    return arg_this->__gisubscriptionCtrlCmdExecPartitionsDelete__(arg_this, arg2);
}

static inline NV_STATUS gisubscriptionCtrlCmdExecPartitionsGet_DISPATCH(struct GPUInstanceSubscription *arg_this, NVC637_CTRL_EXEC_PARTITIONS_GET_PARAMS *arg2) {
    return arg_this->__gisubscriptionCtrlCmdExecPartitionsGet__(arg_this, arg2);
}

static inline NV_STATUS gisubscriptionCtrlCmdExecPartitionsGetProfileCapacity_DISPATCH(struct GPUInstanceSubscription *arg_this, NVC637_CTRL_EXEC_PARTITIONS_GET_PROFILE_CAPACITY_PARAMS *arg2) {
    return arg_this->__gisubscriptionCtrlCmdExecPartitionsGetProfileCapacity__(arg_this, arg2);
}

static inline NV_STATUS gisubscriptionCtrlCmdExecPartitionsGetActiveIds_DISPATCH(struct GPUInstanceSubscription *arg_this, NVC637_CTRL_EXEC_PARTITIONS_GET_ACTIVE_IDS_PARAMS *arg2) {
    return arg_this->__gisubscriptionCtrlCmdExecPartitionsGetActiveIds__(arg_this, arg2);
}

static inline NV_STATUS gisubscriptionCtrlCmdExecPartitionsExport_DISPATCH(struct GPUInstanceSubscription *arg_this, NVC637_CTRL_EXEC_PARTITIONS_IMPORT_EXPORT_PARAMS *arg2) {
    return arg_this->__gisubscriptionCtrlCmdExecPartitionsExport__(arg_this, arg2);
}

static inline NV_STATUS gisubscriptionCtrlCmdExecPartitionsImport_DISPATCH(struct GPUInstanceSubscription *arg_this, NVC637_CTRL_EXEC_PARTITIONS_IMPORT_EXPORT_PARAMS *arg2) {
    return arg_this->__gisubscriptionCtrlCmdExecPartitionsImport__(arg_this, arg2);
}

static inline NV_STATUS gisubscriptionCtrlCmdGetUuid_DISPATCH(struct GPUInstanceSubscription *arg_this, NVC637_CTRL_GET_UUID_PARAMS *arg2) {
    return arg_this->__gisubscriptionCtrlCmdGetUuid__(arg_this, arg2);
}

static inline NV_STATUS gisubscriptionControl_DISPATCH(struct GPUInstanceSubscription *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__gisubscriptionControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS gisubscriptionMap_DISPATCH(struct GPUInstanceSubscription *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__gisubscriptionMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS gisubscriptionUnmap_DISPATCH(struct GPUInstanceSubscription *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__gisubscriptionUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NvBool gisubscriptionShareCallback_DISPATCH(struct GPUInstanceSubscription *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__gisubscriptionShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS gisubscriptionGetRegBaseOffsetAndSize_DISPATCH(struct GPUInstanceSubscription *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__gisubscriptionGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS gisubscriptionGetMapAddrSpace_DISPATCH(struct GPUInstanceSubscription *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__gisubscriptionGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS gisubscriptionInternalControlForward_DISPATCH(struct GPUInstanceSubscription *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__gisubscriptionInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NvHandle gisubscriptionGetInternalObjectHandle_DISPATCH(struct GPUInstanceSubscription *pGpuResource) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__gisubscriptionGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool gisubscriptionAccessCallback_DISPATCH(struct GPUInstanceSubscription *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_metadata_ptr->vtable.__gisubscriptionAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS gisubscriptionGetMemInterMapParams_DISPATCH(struct GPUInstanceSubscription *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__gisubscriptionGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS gisubscriptionCheckMemInterUnmap_DISPATCH(struct GPUInstanceSubscription *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__gisubscriptionCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS gisubscriptionGetMemoryMappingDescriptor_DISPATCH(struct GPUInstanceSubscription *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__gisubscriptionGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS gisubscriptionControlSerialization_Prologue_DISPATCH(struct GPUInstanceSubscription *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__gisubscriptionControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void gisubscriptionControlSerialization_Epilogue_DISPATCH(struct GPUInstanceSubscription *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__gisubscriptionControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS gisubscriptionControl_Prologue_DISPATCH(struct GPUInstanceSubscription *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__gisubscriptionControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void gisubscriptionControl_Epilogue_DISPATCH(struct GPUInstanceSubscription *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__gisubscriptionControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS gisubscriptionIsDuplicate_DISPATCH(struct GPUInstanceSubscription *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_metadata_ptr->vtable.__gisubscriptionIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void gisubscriptionPreDestruct_DISPATCH(struct GPUInstanceSubscription *pResource) {
    pResource->__nvoc_metadata_ptr->vtable.__gisubscriptionPreDestruct__(pResource);
}

static inline NV_STATUS gisubscriptionControlFilter_DISPATCH(struct GPUInstanceSubscription *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__gisubscriptionControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool gisubscriptionIsPartialUnmapSupported_DISPATCH(struct GPUInstanceSubscription *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__gisubscriptionIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS gisubscriptionMapTo_DISPATCH(struct GPUInstanceSubscription *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__gisubscriptionMapTo__(pResource, pParams);
}

static inline NV_STATUS gisubscriptionUnmapFrom_DISPATCH(struct GPUInstanceSubscription *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__gisubscriptionUnmapFrom__(pResource, pParams);
}

static inline NvU32 gisubscriptionGetRefCount_DISPATCH(struct GPUInstanceSubscription *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__gisubscriptionGetRefCount__(pResource);
}

static inline void gisubscriptionAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct GPUInstanceSubscription *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_metadata_ptr->vtable.__gisubscriptionAddAdditionalDependants__(pClient, pResource, pReference);
}

NvBool gisubscriptionCanCopy_IMPL(struct GPUInstanceSubscription *arg1);

NV_STATUS gisubscriptionCtrlCmdExecPartitionsCreate_IMPL(struct GPUInstanceSubscription *arg1, NVC637_CTRL_EXEC_PARTITIONS_CREATE_PARAMS *arg2);

NV_STATUS gisubscriptionCtrlCmdExecPartitionsDelete_IMPL(struct GPUInstanceSubscription *arg1, NVC637_CTRL_EXEC_PARTITIONS_DELETE_PARAMS *arg2);

NV_STATUS gisubscriptionCtrlCmdExecPartitionsGet_IMPL(struct GPUInstanceSubscription *arg1, NVC637_CTRL_EXEC_PARTITIONS_GET_PARAMS *arg2);

NV_STATUS gisubscriptionCtrlCmdExecPartitionsGetProfileCapacity_IMPL(struct GPUInstanceSubscription *arg1, NVC637_CTRL_EXEC_PARTITIONS_GET_PROFILE_CAPACITY_PARAMS *arg2);

NV_STATUS gisubscriptionCtrlCmdExecPartitionsGetActiveIds_IMPL(struct GPUInstanceSubscription *arg1, NVC637_CTRL_EXEC_PARTITIONS_GET_ACTIVE_IDS_PARAMS *arg2);

NV_STATUS gisubscriptionCtrlCmdExecPartitionsExport_IMPL(struct GPUInstanceSubscription *arg1, NVC637_CTRL_EXEC_PARTITIONS_IMPORT_EXPORT_PARAMS *arg2);

NV_STATUS gisubscriptionCtrlCmdExecPartitionsImport_IMPL(struct GPUInstanceSubscription *arg1, NVC637_CTRL_EXEC_PARTITIONS_IMPORT_EXPORT_PARAMS *arg2);

NV_STATUS gisubscriptionCtrlCmdGetUuid_IMPL(struct GPUInstanceSubscription *arg1, NVC637_CTRL_GET_UUID_PARAMS *arg2);

static inline NvBool gisubscriptionIsDeviceProfiling(struct GPUInstanceSubscription *pGPUInstanceSubscription) {
    struct GPUInstanceSubscription_PRIVATE *pGPUInstanceSubscription_PRIVATE = (struct GPUInstanceSubscription_PRIVATE *)pGPUInstanceSubscription;
    return pGPUInstanceSubscription_PRIVATE->bDeviceProfiling;
}

static inline KERNEL_MIG_GPU_INSTANCE *gisubscriptionGetMIGGPUInstance(struct GPUInstanceSubscription *pGPUInstanceSubscription) {
    struct GPUInstanceSubscription_PRIVATE *pGPUInstanceSubscription_PRIVATE = (struct GPUInstanceSubscription_PRIVATE *)pGPUInstanceSubscription;
    return pGPUInstanceSubscription_PRIVATE->pKernelMIGGpuInstance;
}

NV_STATUS gisubscriptionGetGPUInstanceSubscription_IMPL(struct RsClient *arg1, NvHandle arg2, struct GPUInstanceSubscription **arg3);

#define gisubscriptionGetGPUInstanceSubscription(arg1, arg2, arg3) gisubscriptionGetGPUInstanceSubscription_IMPL(arg1, arg2, arg3)
NvBool gisubscriptionShouldClassBeFreedOnUnsubscribe_IMPL(NvU32 internalClassId);

#define gisubscriptionShouldClassBeFreedOnUnsubscribe(internalClassId) gisubscriptionShouldClassBeFreedOnUnsubscribe_IMPL(internalClassId)
void gisubscriptionCleanupOnUnsubscribe_IMPL(CALL_CONTEXT *arg1);

#define gisubscriptionCleanupOnUnsubscribe(arg1) gisubscriptionCleanupOnUnsubscribe_IMPL(arg1)
NV_STATUS gisubscriptionConstruct_IMPL(struct GPUInstanceSubscription *arg_pGPUInstanceSubscription, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_gisubscriptionConstruct(arg_pGPUInstanceSubscription, arg_pCallContext, arg_pParams) gisubscriptionConstruct_IMPL(arg_pGPUInstanceSubscription, arg_pCallContext, arg_pParams)
NV_STATUS gisubscriptionCopyConstruct_IMPL(struct GPUInstanceSubscription *arg1, CALL_CONTEXT *arg2, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg3);

#ifdef __nvoc_gpu_instance_subscription_h_disabled
static inline NV_STATUS gisubscriptionCopyConstruct(struct GPUInstanceSubscription *arg1, CALL_CONTEXT *arg2, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg3) {
    NV_ASSERT_FAILED_PRECOMP("GPUInstanceSubscription was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_instance_subscription_h_disabled
#define gisubscriptionCopyConstruct(arg1, arg2, arg3) gisubscriptionCopyConstruct_IMPL(arg1, arg2, arg3)
#endif //__nvoc_gpu_instance_subscription_h_disabled

void gisubscriptionDestruct_IMPL(struct GPUInstanceSubscription *arg1);

#define __nvoc_gisubscriptionDestruct(arg1) gisubscriptionDestruct_IMPL(arg1)
NvBool gisubscriptionIsDuped_IMPL(struct GPUInstanceSubscription *arg1);

#ifdef __nvoc_gpu_instance_subscription_h_disabled
static inline NvBool gisubscriptionIsDuped(struct GPUInstanceSubscription *arg1) {
    NV_ASSERT_FAILED_PRECOMP("GPUInstanceSubscription was disabled!");
    return NV_FALSE;
}
#else //__nvoc_gpu_instance_subscription_h_disabled
#define gisubscriptionIsDuped(arg1) gisubscriptionIsDuped_IMPL(arg1)
#endif //__nvoc_gpu_instance_subscription_h_disabled

#undef PRIVATE_FIELD


#endif // GPU_INSTANCE_SUBSCRIPTION_H


#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_GPU_INSTANCE_SUBSCRIPTION_NVOC_H_
