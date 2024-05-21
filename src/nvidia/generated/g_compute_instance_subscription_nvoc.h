
#ifndef _G_COMPUTE_INSTANCE_SUBSCRIPTION_NVOC_H_
#define _G_COMPUTE_INSTANCE_SUBSCRIPTION_NVOC_H_
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

#pragma once
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


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_COMPUTE_INSTANCE_SUBSCRIPTION_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct ComputeInstanceSubscription {

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
    struct ComputeInstanceSubscription *__nvoc_pbase_ComputeInstanceSubscription;    // cisubscription

    // Vtable with 26 per-object function pointers
    NvBool (*__cisubscriptionCanCopy__)(struct ComputeInstanceSubscription * /*this*/);  // virtual override (res) base (gpures)
    NV_STATUS (*__cisubscriptionCtrlCmdGetUuid__)(struct ComputeInstanceSubscription * /*this*/, NVC638_CTRL_GET_UUID_PARAMS *);  // exported (id=0xc6380101)
    NV_STATUS (*__cisubscriptionControl__)(struct ComputeInstanceSubscription * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__cisubscriptionMap__)(struct ComputeInstanceSubscription * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__cisubscriptionUnmap__)(struct ComputeInstanceSubscription * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NvBool (*__cisubscriptionShareCallback__)(struct ComputeInstanceSubscription * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__cisubscriptionGetRegBaseOffsetAndSize__)(struct ComputeInstanceSubscription * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__cisubscriptionGetMapAddrSpace__)(struct ComputeInstanceSubscription * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__cisubscriptionInternalControlForward__)(struct ComputeInstanceSubscription * /*this*/, NvU32, void *, NvU32);  // virtual inherited (gpures) base (gpures)
    NvHandle (*__cisubscriptionGetInternalObjectHandle__)(struct ComputeInstanceSubscription * /*this*/);  // virtual inherited (gpures) base (gpures)
    NvBool (*__cisubscriptionAccessCallback__)(struct ComputeInstanceSubscription * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__cisubscriptionGetMemInterMapParams__)(struct ComputeInstanceSubscription * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__cisubscriptionCheckMemInterUnmap__)(struct ComputeInstanceSubscription * /*this*/, NvBool);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__cisubscriptionGetMemoryMappingDescriptor__)(struct ComputeInstanceSubscription * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__cisubscriptionControlSerialization_Prologue__)(struct ComputeInstanceSubscription * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__cisubscriptionControlSerialization_Epilogue__)(struct ComputeInstanceSubscription * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__cisubscriptionControl_Prologue__)(struct ComputeInstanceSubscription * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__cisubscriptionControl_Epilogue__)(struct ComputeInstanceSubscription * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__cisubscriptionIsDuplicate__)(struct ComputeInstanceSubscription * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (gpures)
    void (*__cisubscriptionPreDestruct__)(struct ComputeInstanceSubscription * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__cisubscriptionControlFilter__)(struct ComputeInstanceSubscription * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (gpures)
    NvBool (*__cisubscriptionIsPartialUnmapSupported__)(struct ComputeInstanceSubscription * /*this*/);  // inline virtual inherited (res) base (gpures) body
    NV_STATUS (*__cisubscriptionMapTo__)(struct ComputeInstanceSubscription * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__cisubscriptionUnmapFrom__)(struct ComputeInstanceSubscription * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (gpures)
    NvU32 (*__cisubscriptionGetRefCount__)(struct ComputeInstanceSubscription * /*this*/);  // virtual inherited (res) base (gpures)
    void (*__cisubscriptionAddAdditionalDependants__)(struct RsClient *, struct ComputeInstanceSubscription * /*this*/, RsResourceRef *);  // virtual inherited (res) base (gpures)

    // Data members
    MIG_COMPUTE_INSTANCE *PRIVATE_FIELD(pMIGComputeInstance);
    NvU64 PRIVATE_FIELD(dupedCapDescriptor);
};


struct ComputeInstanceSubscription_PRIVATE {

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
    struct ComputeInstanceSubscription *__nvoc_pbase_ComputeInstanceSubscription;    // cisubscription

    // Vtable with 26 per-object function pointers
    NvBool (*__cisubscriptionCanCopy__)(struct ComputeInstanceSubscription * /*this*/);  // virtual override (res) base (gpures)
    NV_STATUS (*__cisubscriptionCtrlCmdGetUuid__)(struct ComputeInstanceSubscription * /*this*/, NVC638_CTRL_GET_UUID_PARAMS *);  // exported (id=0xc6380101)
    NV_STATUS (*__cisubscriptionControl__)(struct ComputeInstanceSubscription * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__cisubscriptionMap__)(struct ComputeInstanceSubscription * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__cisubscriptionUnmap__)(struct ComputeInstanceSubscription * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NvBool (*__cisubscriptionShareCallback__)(struct ComputeInstanceSubscription * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__cisubscriptionGetRegBaseOffsetAndSize__)(struct ComputeInstanceSubscription * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__cisubscriptionGetMapAddrSpace__)(struct ComputeInstanceSubscription * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__cisubscriptionInternalControlForward__)(struct ComputeInstanceSubscription * /*this*/, NvU32, void *, NvU32);  // virtual inherited (gpures) base (gpures)
    NvHandle (*__cisubscriptionGetInternalObjectHandle__)(struct ComputeInstanceSubscription * /*this*/);  // virtual inherited (gpures) base (gpures)
    NvBool (*__cisubscriptionAccessCallback__)(struct ComputeInstanceSubscription * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__cisubscriptionGetMemInterMapParams__)(struct ComputeInstanceSubscription * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__cisubscriptionCheckMemInterUnmap__)(struct ComputeInstanceSubscription * /*this*/, NvBool);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__cisubscriptionGetMemoryMappingDescriptor__)(struct ComputeInstanceSubscription * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__cisubscriptionControlSerialization_Prologue__)(struct ComputeInstanceSubscription * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__cisubscriptionControlSerialization_Epilogue__)(struct ComputeInstanceSubscription * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__cisubscriptionControl_Prologue__)(struct ComputeInstanceSubscription * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__cisubscriptionControl_Epilogue__)(struct ComputeInstanceSubscription * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__cisubscriptionIsDuplicate__)(struct ComputeInstanceSubscription * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (gpures)
    void (*__cisubscriptionPreDestruct__)(struct ComputeInstanceSubscription * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__cisubscriptionControlFilter__)(struct ComputeInstanceSubscription * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (gpures)
    NvBool (*__cisubscriptionIsPartialUnmapSupported__)(struct ComputeInstanceSubscription * /*this*/);  // inline virtual inherited (res) base (gpures) body
    NV_STATUS (*__cisubscriptionMapTo__)(struct ComputeInstanceSubscription * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__cisubscriptionUnmapFrom__)(struct ComputeInstanceSubscription * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (gpures)
    NvU32 (*__cisubscriptionGetRefCount__)(struct ComputeInstanceSubscription * /*this*/);  // virtual inherited (res) base (gpures)
    void (*__cisubscriptionAddAdditionalDependants__)(struct RsClient *, struct ComputeInstanceSubscription * /*this*/, RsResourceRef *);  // virtual inherited (res) base (gpures)

    // Data members
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

// Casting support
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


// Wrapper macros
#define cisubscriptionCanCopy_FNPTR(arg_this) arg_this->__cisubscriptionCanCopy__
#define cisubscriptionCanCopy(arg_this) cisubscriptionCanCopy_DISPATCH(arg_this)
#define cisubscriptionCtrlCmdGetUuid_FNPTR(arg_this) arg_this->__cisubscriptionCtrlCmdGetUuid__
#define cisubscriptionCtrlCmdGetUuid(arg_this, arg2) cisubscriptionCtrlCmdGetUuid_DISPATCH(arg_this, arg2)
#define cisubscriptionControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresControl__
#define cisubscriptionControl(pGpuResource, pCallContext, pParams) cisubscriptionControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define cisubscriptionMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresMap__
#define cisubscriptionMap(pGpuResource, pCallContext, pParams, pCpuMapping) cisubscriptionMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define cisubscriptionUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresUnmap__
#define cisubscriptionUnmap(pGpuResource, pCallContext, pCpuMapping) cisubscriptionUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define cisubscriptionShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresShareCallback__
#define cisubscriptionShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) cisubscriptionShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define cisubscriptionGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetRegBaseOffsetAndSize__
#define cisubscriptionGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) cisubscriptionGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define cisubscriptionGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetMapAddrSpace__
#define cisubscriptionGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) cisubscriptionGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define cisubscriptionInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresInternalControlForward__
#define cisubscriptionInternalControlForward(pGpuResource, command, pParams, size) cisubscriptionInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define cisubscriptionGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetInternalObjectHandle__
#define cisubscriptionGetInternalObjectHandle(pGpuResource) cisubscriptionGetInternalObjectHandle_DISPATCH(pGpuResource)
#define cisubscriptionAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresAccessCallback__
#define cisubscriptionAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) cisubscriptionAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define cisubscriptionGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemInterMapParams__
#define cisubscriptionGetMemInterMapParams(pRmResource, pParams) cisubscriptionGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define cisubscriptionCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresCheckMemInterUnmap__
#define cisubscriptionCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) cisubscriptionCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define cisubscriptionGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemoryMappingDescriptor__
#define cisubscriptionGetMemoryMappingDescriptor(pRmResource, ppMemDesc) cisubscriptionGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define cisubscriptionControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Prologue__
#define cisubscriptionControlSerialization_Prologue(pResource, pCallContext, pParams) cisubscriptionControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define cisubscriptionControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Epilogue__
#define cisubscriptionControlSerialization_Epilogue(pResource, pCallContext, pParams) cisubscriptionControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define cisubscriptionControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Prologue__
#define cisubscriptionControl_Prologue(pResource, pCallContext, pParams) cisubscriptionControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define cisubscriptionControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Epilogue__
#define cisubscriptionControl_Epilogue(pResource, pCallContext, pParams) cisubscriptionControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define cisubscriptionIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsDuplicate__
#define cisubscriptionIsDuplicate(pResource, hMemory, pDuplicate) cisubscriptionIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define cisubscriptionPreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resPreDestruct__
#define cisubscriptionPreDestruct(pResource) cisubscriptionPreDestruct_DISPATCH(pResource)
#define cisubscriptionControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resControlFilter__
#define cisubscriptionControlFilter(pResource, pCallContext, pParams) cisubscriptionControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define cisubscriptionIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsPartialUnmapSupported__
#define cisubscriptionIsPartialUnmapSupported(pResource) cisubscriptionIsPartialUnmapSupported_DISPATCH(pResource)
#define cisubscriptionMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resMapTo__
#define cisubscriptionMapTo(pResource, pParams) cisubscriptionMapTo_DISPATCH(pResource, pParams)
#define cisubscriptionUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmapFrom__
#define cisubscriptionUnmapFrom(pResource, pParams) cisubscriptionUnmapFrom_DISPATCH(pResource, pParams)
#define cisubscriptionGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resGetRefCount__
#define cisubscriptionGetRefCount(pResource) cisubscriptionGetRefCount_DISPATCH(pResource)
#define cisubscriptionAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resAddAdditionalDependants__
#define cisubscriptionAddAdditionalDependants(pClient, pResource, pReference) cisubscriptionAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NvBool cisubscriptionCanCopy_DISPATCH(struct ComputeInstanceSubscription *arg_this) {
    return arg_this->__cisubscriptionCanCopy__(arg_this);
}

static inline NV_STATUS cisubscriptionCtrlCmdGetUuid_DISPATCH(struct ComputeInstanceSubscription *arg_this, NVC638_CTRL_GET_UUID_PARAMS *arg2) {
    return arg_this->__cisubscriptionCtrlCmdGetUuid__(arg_this, arg2);
}

static inline NV_STATUS cisubscriptionControl_DISPATCH(struct ComputeInstanceSubscription *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__cisubscriptionControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS cisubscriptionMap_DISPATCH(struct ComputeInstanceSubscription *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__cisubscriptionMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS cisubscriptionUnmap_DISPATCH(struct ComputeInstanceSubscription *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__cisubscriptionUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NvBool cisubscriptionShareCallback_DISPATCH(struct ComputeInstanceSubscription *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__cisubscriptionShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS cisubscriptionGetRegBaseOffsetAndSize_DISPATCH(struct ComputeInstanceSubscription *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__cisubscriptionGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS cisubscriptionGetMapAddrSpace_DISPATCH(struct ComputeInstanceSubscription *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__cisubscriptionGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS cisubscriptionInternalControlForward_DISPATCH(struct ComputeInstanceSubscription *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__cisubscriptionInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NvHandle cisubscriptionGetInternalObjectHandle_DISPATCH(struct ComputeInstanceSubscription *pGpuResource) {
    return pGpuResource->__cisubscriptionGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool cisubscriptionAccessCallback_DISPATCH(struct ComputeInstanceSubscription *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__cisubscriptionAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS cisubscriptionGetMemInterMapParams_DISPATCH(struct ComputeInstanceSubscription *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__cisubscriptionGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS cisubscriptionCheckMemInterUnmap_DISPATCH(struct ComputeInstanceSubscription *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__cisubscriptionCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS cisubscriptionGetMemoryMappingDescriptor_DISPATCH(struct ComputeInstanceSubscription *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__cisubscriptionGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS cisubscriptionControlSerialization_Prologue_DISPATCH(struct ComputeInstanceSubscription *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__cisubscriptionControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void cisubscriptionControlSerialization_Epilogue_DISPATCH(struct ComputeInstanceSubscription *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__cisubscriptionControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS cisubscriptionControl_Prologue_DISPATCH(struct ComputeInstanceSubscription *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__cisubscriptionControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void cisubscriptionControl_Epilogue_DISPATCH(struct ComputeInstanceSubscription *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__cisubscriptionControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS cisubscriptionIsDuplicate_DISPATCH(struct ComputeInstanceSubscription *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__cisubscriptionIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void cisubscriptionPreDestruct_DISPATCH(struct ComputeInstanceSubscription *pResource) {
    pResource->__cisubscriptionPreDestruct__(pResource);
}

static inline NV_STATUS cisubscriptionControlFilter_DISPATCH(struct ComputeInstanceSubscription *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__cisubscriptionControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool cisubscriptionIsPartialUnmapSupported_DISPATCH(struct ComputeInstanceSubscription *pResource) {
    return pResource->__cisubscriptionIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS cisubscriptionMapTo_DISPATCH(struct ComputeInstanceSubscription *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__cisubscriptionMapTo__(pResource, pParams);
}

static inline NV_STATUS cisubscriptionUnmapFrom_DISPATCH(struct ComputeInstanceSubscription *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__cisubscriptionUnmapFrom__(pResource, pParams);
}

static inline NvU32 cisubscriptionGetRefCount_DISPATCH(struct ComputeInstanceSubscription *pResource) {
    return pResource->__cisubscriptionGetRefCount__(pResource);
}

static inline void cisubscriptionAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct ComputeInstanceSubscription *pResource, RsResourceRef *pReference) {
    pResource->__cisubscriptionAddAdditionalDependants__(pClient, pResource, pReference);
}

NvBool cisubscriptionCanCopy_IMPL(struct ComputeInstanceSubscription *arg1);

NV_STATUS cisubscriptionCtrlCmdGetUuid_IMPL(struct ComputeInstanceSubscription *arg1, NVC638_CTRL_GET_UUID_PARAMS *arg2);

static inline MIG_COMPUTE_INSTANCE *cisubscriptionGetMIGComputeInstance(struct ComputeInstanceSubscription *pComputeInstanceSubscription) {
    struct ComputeInstanceSubscription_PRIVATE *pComputeInstanceSubscription_PRIVATE = (struct ComputeInstanceSubscription_PRIVATE *)pComputeInstanceSubscription;
    return pComputeInstanceSubscription_PRIVATE->pMIGComputeInstance;
}

NV_STATUS cisubscriptionGetComputeInstanceSubscription_IMPL(struct RsClient *arg1, NvHandle arg2, struct ComputeInstanceSubscription **arg3);

#define cisubscriptionGetComputeInstanceSubscription(arg1, arg2, arg3) cisubscriptionGetComputeInstanceSubscription_IMPL(arg1, arg2, arg3)
NV_STATUS cisubscriptionConstruct_IMPL(struct ComputeInstanceSubscription *arg_pComputeInstanceSubscription, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_cisubscriptionConstruct(arg_pComputeInstanceSubscription, arg_pCallContext, arg_pParams) cisubscriptionConstruct_IMPL(arg_pComputeInstanceSubscription, arg_pCallContext, arg_pParams)
NV_STATUS cisubscriptionCopyConstruct_IMPL(struct ComputeInstanceSubscription *arg1, CALL_CONTEXT *arg2, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg3);

#ifdef __nvoc_compute_instance_subscription_h_disabled
static inline NV_STATUS cisubscriptionCopyConstruct(struct ComputeInstanceSubscription *arg1, CALL_CONTEXT *arg2, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg3) {
    NV_ASSERT_FAILED_PRECOMP("ComputeInstanceSubscription was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_compute_instance_subscription_h_disabled
#define cisubscriptionCopyConstruct(arg1, arg2, arg3) cisubscriptionCopyConstruct_IMPL(arg1, arg2, arg3)
#endif //__nvoc_compute_instance_subscription_h_disabled

void cisubscriptionDestruct_IMPL(struct ComputeInstanceSubscription *arg1);

#define __nvoc_cisubscriptionDestruct(arg1) cisubscriptionDestruct_IMPL(arg1)
#undef PRIVATE_FIELD


#endif // COMPUTE_INSTANCE_SUBSCRIPTION_H


#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_COMPUTE_INSTANCE_SUBSCRIPTION_NVOC_H_
