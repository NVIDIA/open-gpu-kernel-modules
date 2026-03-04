
#ifndef _G_KERNEL_WATCHDOG_NVOC_H_
#define _G_KERNEL_WATCHDOG_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_kernel_watchdog_nvoc.h"

#ifndef _KERNEL_WATCHDOG_H_
#define _KERNEL_WATCHDOG_H_

#include "gpu/gpu_resource.h"
#include "resserv/rs_server.h"
#include "resserv/rs_resource.h"
#include "kernel/gpu/rc/kernel_rc_watchdog_private.h"

#include "class/cl008f.h" // KERNEL_WATCHDOG
//
// Watchdog object ids
//
#define KERNEL_WATCHDOG_OBJECT_ID             (0x31425900)

/*!
 * KernelWatchdog provides kernel side services for managing RC Watchdog channels.
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_KERNEL_WATCHDOG_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__KernelWatchdog;
struct NVOC_METADATA__GpuResource;
struct NVOC_VTABLE__KernelWatchdog;


struct KernelWatchdog {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__KernelWatchdog *__nvoc_metadata_ptr;
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
    struct KernelWatchdog *__nvoc_pbase_KernelWatchdog;    // kwdt

    // Data members
    NvHandle hClient;
    NvHandle hDevice;
    NvHandle hSubdevice;
    KernelWatchdogState watchdogState;
    KernelWatchdogPersistent watchdogPersistent;
    KernelWatchdogChannelInfo watchdogChannelInfo;
};


// Vtable with 25 per-class function pointers
struct NVOC_VTABLE__KernelWatchdog {
    NV_STATUS (*__kwdtControl__)(struct KernelWatchdog * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__kwdtMap__)(struct KernelWatchdog * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__kwdtUnmap__)(struct KernelWatchdog * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NvBool (*__kwdtShareCallback__)(struct KernelWatchdog * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__kwdtGetRegBaseOffsetAndSize__)(struct KernelWatchdog * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__kwdtGetMapAddrSpace__)(struct KernelWatchdog * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__kwdtInternalControlForward__)(struct KernelWatchdog * /*this*/, NvU32, void *, NvU32);  // virtual inherited (gpures) base (gpures)
    NvHandle (*__kwdtGetInternalObjectHandle__)(struct KernelWatchdog * /*this*/);  // virtual inherited (gpures) base (gpures)
    NvBool (*__kwdtAccessCallback__)(struct KernelWatchdog * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__kwdtGetMemInterMapParams__)(struct KernelWatchdog * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__kwdtCheckMemInterUnmap__)(struct KernelWatchdog * /*this*/, NvBool);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__kwdtGetMemoryMappingDescriptor__)(struct KernelWatchdog * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__kwdtControlSerialization_Prologue__)(struct KernelWatchdog * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__kwdtControlSerialization_Epilogue__)(struct KernelWatchdog * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__kwdtControl_Prologue__)(struct KernelWatchdog * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__kwdtControl_Epilogue__)(struct KernelWatchdog * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NvBool (*__kwdtCanCopy__)(struct KernelWatchdog * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__kwdtIsDuplicate__)(struct KernelWatchdog * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (gpures)
    void (*__kwdtPreDestruct__)(struct KernelWatchdog * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__kwdtControlFilter__)(struct KernelWatchdog * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (gpures)
    NvBool (*__kwdtIsPartialUnmapSupported__)(struct KernelWatchdog * /*this*/);  // inline virtual inherited (res) base (gpures) body
    NV_STATUS (*__kwdtMapTo__)(struct KernelWatchdog * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__kwdtUnmapFrom__)(struct KernelWatchdog * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (gpures)
    NvU32 (*__kwdtGetRefCount__)(struct KernelWatchdog * /*this*/);  // virtual inherited (res) base (gpures)
    void (*__kwdtAddAdditionalDependants__)(struct RsClient *, struct KernelWatchdog * /*this*/, RsResourceRef *);  // virtual inherited (res) base (gpures)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__KernelWatchdog {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResource metadata__GpuResource;
    const struct NVOC_VTABLE__KernelWatchdog vtable;
};

#ifndef __NVOC_CLASS_KernelWatchdog_TYPEDEF__
#define __NVOC_CLASS_KernelWatchdog_TYPEDEF__
typedef struct KernelWatchdog KernelWatchdog;
#endif /* __NVOC_CLASS_KernelWatchdog_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelWatchdog
#define __nvoc_class_id_KernelWatchdog 0x7ace3d
#endif /* __nvoc_class_id_KernelWatchdog */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelWatchdog;

#define __staticCast_KernelWatchdog(pThis) \
    ((pThis)->__nvoc_pbase_KernelWatchdog)

#ifdef __nvoc_kernel_watchdog_h_disabled
#define __dynamicCast_KernelWatchdog(pThis) ((KernelWatchdog*) NULL)
#else //__nvoc_kernel_watchdog_h_disabled
#define __dynamicCast_KernelWatchdog(pThis) \
    ((KernelWatchdog*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelWatchdog)))
#endif //__nvoc_kernel_watchdog_h_disabled

NV_STATUS __nvoc_objCreateDynamic_KernelWatchdog(KernelWatchdog**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelWatchdog(KernelWatchdog**, Dynamic*, NvU32, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __objCreate_KernelWatchdog(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_KernelWatchdog((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros for implementation functions
NV_STATUS kwdtConstruct_IMPL(struct KernelWatchdog *arg_pKernelWatchdog, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __nvoc_kwdtConstruct(arg_pKernelWatchdog, arg_pCallContext, arg_pParams) kwdtConstruct_IMPL(arg_pKernelWatchdog, arg_pCallContext, arg_pParams)

void kwdtDestruct_IMPL(struct KernelWatchdog *arg_this);
#define __nvoc_kwdtDestruct(arg_this) kwdtDestruct_IMPL(arg_this)


// Wrapper macros for halified functions
#define kwdtControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresControl__
#define kwdtControl(pGpuResource, pCallContext, pParams) kwdtControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define kwdtMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresMap__
#define kwdtMap(pGpuResource, pCallContext, pParams, pCpuMapping) kwdtMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define kwdtUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresUnmap__
#define kwdtUnmap(pGpuResource, pCallContext, pCpuMapping) kwdtUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define kwdtShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresShareCallback__
#define kwdtShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) kwdtShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define kwdtGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__
#define kwdtGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) kwdtGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define kwdtGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__
#define kwdtGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) kwdtGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define kwdtInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__
#define kwdtInternalControlForward(pGpuResource, command, pParams, size) kwdtInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define kwdtGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__
#define kwdtGetInternalObjectHandle(pGpuResource) kwdtGetInternalObjectHandle_DISPATCH(pGpuResource)
#define kwdtAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresAccessCallback__
#define kwdtAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) kwdtAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define kwdtGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define kwdtGetMemInterMapParams(pRmResource, pParams) kwdtGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define kwdtCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define kwdtCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) kwdtCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define kwdtGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define kwdtGetMemoryMappingDescriptor(pRmResource, ppMemDesc) kwdtGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define kwdtControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Prologue__
#define kwdtControlSerialization_Prologue(pResource, pCallContext, pParams) kwdtControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define kwdtControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Epilogue__
#define kwdtControlSerialization_Epilogue(pResource, pCallContext, pParams) kwdtControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define kwdtControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Prologue__
#define kwdtControl_Prologue(pResource, pCallContext, pParams) kwdtControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define kwdtControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Epilogue__
#define kwdtControl_Epilogue(pResource, pCallContext, pParams) kwdtControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define kwdtCanCopy_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define kwdtCanCopy(pResource) kwdtCanCopy_DISPATCH(pResource)
#define kwdtIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define kwdtIsDuplicate(pResource, hMemory, pDuplicate) kwdtIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define kwdtPreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define kwdtPreDestruct(pResource) kwdtPreDestruct_DISPATCH(pResource)
#define kwdtControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define kwdtControlFilter(pResource, pCallContext, pParams) kwdtControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define kwdtIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define kwdtIsPartialUnmapSupported(pResource) kwdtIsPartialUnmapSupported_DISPATCH(pResource)
#define kwdtMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define kwdtMapTo(pResource, pParams) kwdtMapTo_DISPATCH(pResource, pParams)
#define kwdtUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define kwdtUnmapFrom(pResource, pParams) kwdtUnmapFrom_DISPATCH(pResource, pParams)
#define kwdtGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define kwdtGetRefCount(pResource) kwdtGetRefCount_DISPATCH(pResource)
#define kwdtAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define kwdtAddAdditionalDependants(pClient, pResource, pReference) kwdtAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NV_STATUS kwdtControl_DISPATCH(struct KernelWatchdog *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__kwdtControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS kwdtMap_DISPATCH(struct KernelWatchdog *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__kwdtMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS kwdtUnmap_DISPATCH(struct KernelWatchdog *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__kwdtUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NvBool kwdtShareCallback_DISPATCH(struct KernelWatchdog *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__kwdtShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS kwdtGetRegBaseOffsetAndSize_DISPATCH(struct KernelWatchdog *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__kwdtGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS kwdtGetMapAddrSpace_DISPATCH(struct KernelWatchdog *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__kwdtGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS kwdtInternalControlForward_DISPATCH(struct KernelWatchdog *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__kwdtInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NvHandle kwdtGetInternalObjectHandle_DISPATCH(struct KernelWatchdog *pGpuResource) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__kwdtGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool kwdtAccessCallback_DISPATCH(struct KernelWatchdog *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_metadata_ptr->vtable.__kwdtAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS kwdtGetMemInterMapParams_DISPATCH(struct KernelWatchdog *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__kwdtGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS kwdtCheckMemInterUnmap_DISPATCH(struct KernelWatchdog *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__kwdtCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS kwdtGetMemoryMappingDescriptor_DISPATCH(struct KernelWatchdog *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__kwdtGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS kwdtControlSerialization_Prologue_DISPATCH(struct KernelWatchdog *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__kwdtControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void kwdtControlSerialization_Epilogue_DISPATCH(struct KernelWatchdog *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__kwdtControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS kwdtControl_Prologue_DISPATCH(struct KernelWatchdog *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__kwdtControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void kwdtControl_Epilogue_DISPATCH(struct KernelWatchdog *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__kwdtControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool kwdtCanCopy_DISPATCH(struct KernelWatchdog *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__kwdtCanCopy__(pResource);
}

static inline NV_STATUS kwdtIsDuplicate_DISPATCH(struct KernelWatchdog *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_metadata_ptr->vtable.__kwdtIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void kwdtPreDestruct_DISPATCH(struct KernelWatchdog *pResource) {
    pResource->__nvoc_metadata_ptr->vtable.__kwdtPreDestruct__(pResource);
}

static inline NV_STATUS kwdtControlFilter_DISPATCH(struct KernelWatchdog *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__kwdtControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool kwdtIsPartialUnmapSupported_DISPATCH(struct KernelWatchdog *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__kwdtIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS kwdtMapTo_DISPATCH(struct KernelWatchdog *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__kwdtMapTo__(pResource, pParams);
}

static inline NV_STATUS kwdtUnmapFrom_DISPATCH(struct KernelWatchdog *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__kwdtUnmapFrom__(pResource, pParams);
}

static inline NvU32 kwdtGetRefCount_DISPATCH(struct KernelWatchdog *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__kwdtGetRefCount__(pResource);
}

static inline void kwdtAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct KernelWatchdog *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_metadata_ptr->vtable.__kwdtAddAdditionalDependants__(pClient, pResource, pReference);
}

#undef PRIVATE_FIELD

#endif // _KERNEL_WATCHDOG_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_WATCHDOG_NVOC_H_
