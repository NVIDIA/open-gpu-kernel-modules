
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
 * SPDX-FileCopyrightText: Copyright (c) 2025-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "gpu/gpu_halspec.h"
#include "gpu/gpu_resource.h"
#include "resserv/rs_server.h"
#include "resserv/rs_resource.h"
#include "kernel/gpu/rc/kernel_rc_watchdog.h"
#include "kernel/gpu/rc/kernel_rc_watchdog_private.h"

#include "class/cl008f.h" // KERNEL_WATCHDOG

struct Subdevice;

#ifndef __nvoc_class_id_Subdevice
#define __nvoc_class_id_Subdevice 0x4b01b3u
typedef struct Subdevice Subdevice;
#endif /* __nvoc_class_id_Subdevice */



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


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__KernelWatchdog;
struct NVOC_METADATA__GpuResource;


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

    // Vtable with 4 per-object function pointers
    NV_STATUS (*__kwdtWatchdogInit__)(struct OBJGPU *, struct KernelWatchdog * /*this*/);  // halified (2 hals) body
    void (*__kwdtWatchdogInitPushbuffer__)(struct OBJGPU *, struct KernelWatchdog * /*this*/);  // halified (2 hals) body
    void (*__kwdtWatchdog__)(struct OBJGPU *, struct KernelWatchdog * /*this*/);  // halified (2 hals) body
    void (*__kwdtWatchdogRecovery__)(struct OBJGPU *, struct KernelWatchdog * /*this*/);  // halified (2 hals) body

    // Data members
    NvHandle hClient;
    NvHandle hDevice;
    NvHandle hSubdevice;
    KernelWatchdogState watchdogState;
    KernelWatchdogPersistent watchdogPersistent;
    KernelWatchdogChannelInfo watchdogChannelInfo;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__KernelWatchdog {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResource metadata__GpuResource;
};

#ifndef __nvoc_class_id_KernelWatchdog
#define __nvoc_class_id_KernelWatchdog 0x7ace3du
typedef struct KernelWatchdog KernelWatchdog;
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

NV_STATUS __nvoc_objCreateDynamic_KernelWatchdog(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelWatchdog(KernelWatchdog**, Dynamic*, NvU32, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __objCreate_KernelWatchdog(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams) \
    __nvoc_objCreate_KernelWatchdog((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pCallContext, pParams)


// Wrapper macros for implementation functions
NV_STATUS kwdtConstruct_IMPL(struct KernelWatchdog *pKernelWatchdog, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __nvoc_kwdtConstruct(pKernelWatchdog, pCallContext, pParams) kwdtConstruct_IMPL(pKernelWatchdog, pCallContext, pParams)

void kwdtDestruct_IMPL(struct KernelWatchdog *arg_this);
#define __nvoc_kwdtDestruct(arg_this) kwdtDestruct_IMPL(arg_this)

NV_STATUS kwdtWatchdogChangeState_IMPL(struct KernelWatchdog *pKernelWatchdog, struct Subdevice *pSubdevice, RC_CHANGE_WATCHDOG_STATE_OPERATION_TYPE operation);
#ifdef __nvoc_kernel_watchdog_h_disabled
static inline NV_STATUS kwdtWatchdogChangeState(struct KernelWatchdog *pKernelWatchdog, struct Subdevice *pSubdevice, RC_CHANGE_WATCHDOG_STATE_OPERATION_TYPE operation) {
    NV_ASSERT_FAILED_PRECOMP("KernelWatchdog was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_watchdog_h_disabled
#define kwdtWatchdogChangeState(pKernelWatchdog, pSubdevice, operation) kwdtWatchdogChangeState_IMPL(pKernelWatchdog, pSubdevice, operation)
#endif // __nvoc_kernel_watchdog_h_disabled

void kwdtWatchdogEnable_IMPL(struct KernelWatchdog *pKernelWatchdog, NvBool bOverRide);
#ifdef __nvoc_kernel_watchdog_h_disabled
static inline void kwdtWatchdogEnable(struct KernelWatchdog *pKernelWatchdog, NvBool bOverRide) {
    NV_ASSERT_FAILED_PRECOMP("KernelWatchdog was disabled!");
}
#else // __nvoc_kernel_watchdog_h_disabled
#define kwdtWatchdogEnable(pKernelWatchdog, bOverRide) kwdtWatchdogEnable_IMPL(pKernelWatchdog, bOverRide)
#endif // __nvoc_kernel_watchdog_h_disabled

void kwdtWatchdogDisable_IMPL(struct KernelWatchdog *pKernelWatchdog);
#ifdef __nvoc_kernel_watchdog_h_disabled
static inline void kwdtWatchdogDisable(struct KernelWatchdog *pKernelWatchdog) {
    NV_ASSERT_FAILED_PRECOMP("KernelWatchdog was disabled!");
}
#else // __nvoc_kernel_watchdog_h_disabled
#define kwdtWatchdogDisable(pKernelWatchdog) kwdtWatchdogDisable_IMPL(pKernelWatchdog)
#endif // __nvoc_kernel_watchdog_h_disabled

void kwdtWatchdogPause_IMPL(struct KernelWatchdog *pKernelWatchdog);
#ifdef __nvoc_kernel_watchdog_h_disabled
static inline void kwdtWatchdogPause(struct KernelWatchdog *pKernelWatchdog) {
    NV_ASSERT_FAILED_PRECOMP("KernelWatchdog was disabled!");
}
#else // __nvoc_kernel_watchdog_h_disabled
#define kwdtWatchdogPause(pKernelWatchdog) kwdtWatchdogPause_IMPL(pKernelWatchdog)
#endif // __nvoc_kernel_watchdog_h_disabled

void kwdtWatchdogResume_IMPL(struct KernelWatchdog *pKernelWatchdog);
#ifdef __nvoc_kernel_watchdog_h_disabled
static inline void kwdtWatchdogResume(struct KernelWatchdog *pKernelWatchdog) {
    NV_ASSERT_FAILED_PRECOMP("KernelWatchdog was disabled!");
}
#else // __nvoc_kernel_watchdog_h_disabled
#define kwdtWatchdogResume(pKernelWatchdog) kwdtWatchdogResume_IMPL(pKernelWatchdog)
#endif // __nvoc_kernel_watchdog_h_disabled

NV_STATUS kwdtWatchdogShutdown_IMPL(struct OBJGPU *pGpu, struct KernelWatchdog *pKernelWatchdog);
#ifdef __nvoc_kernel_watchdog_h_disabled
static inline NV_STATUS kwdtWatchdogShutdown(struct OBJGPU *pGpu, struct KernelWatchdog *pKernelWatchdog) {
    NV_ASSERT_FAILED_PRECOMP("KernelWatchdog was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_watchdog_h_disabled
#define kwdtWatchdogShutdown(pGpu, pKernelWatchdog) kwdtWatchdogShutdown_IMPL(pGpu, pKernelWatchdog)
#endif // __nvoc_kernel_watchdog_h_disabled

void kwdtWatchdogGetReservationCounts_IMPL(struct KernelWatchdog *pKernelWatchdog, NvS32 *pEnable, NvS32 *pDisable, NvS32 *pSoftDisable);
#ifdef __nvoc_kernel_watchdog_h_disabled
static inline void kwdtWatchdogGetReservationCounts(struct KernelWatchdog *pKernelWatchdog, NvS32 *pEnable, NvS32 *pDisable, NvS32 *pSoftDisable) {
    NV_ASSERT_FAILED_PRECOMP("KernelWatchdog was disabled!");
}
#else // __nvoc_kernel_watchdog_h_disabled
#define kwdtWatchdogGetReservationCounts(pKernelWatchdog, pEnable, pDisable, pSoftDisable) kwdtWatchdogGetReservationCounts_IMPL(pKernelWatchdog, pEnable, pDisable, pSoftDisable)
#endif // __nvoc_kernel_watchdog_h_disabled

NvBool kwdtTestAllowAlloc_IMPL(struct OBJGPU *pGpu, struct KernelWatchdog *pKernelWatchdog, NvU32 failMask);
#ifdef __nvoc_kernel_watchdog_h_disabled
static inline NvBool kwdtTestAllowAlloc(struct OBJGPU *pGpu, struct KernelWatchdog *pKernelWatchdog, NvU32 failMask) {
    NV_ASSERT_FAILED_PRECOMP("KernelWatchdog was disabled!");
    return NV_FALSE;
}
#else // __nvoc_kernel_watchdog_h_disabled
#define kwdtTestAllowAlloc(pGpu, pKernelWatchdog, failMask) kwdtTestAllowAlloc_IMPL(pGpu, pKernelWatchdog, failMask)
#endif // __nvoc_kernel_watchdog_h_disabled

void kwdtWatchdogWriteNotifierToGpfifo_IMPL(struct OBJGPU *pGpu, struct KernelWatchdog *pKernelWatchdog);
#ifdef __nvoc_kernel_watchdog_h_disabled
static inline void kwdtWatchdogWriteNotifierToGpfifo(struct OBJGPU *pGpu, struct KernelWatchdog *pKernelWatchdog) {
    NV_ASSERT_FAILED_PRECOMP("KernelWatchdog was disabled!");
}
#else // __nvoc_kernel_watchdog_h_disabled
#define kwdtWatchdogWriteNotifierToGpfifo(pGpu, pKernelWatchdog) kwdtWatchdogWriteNotifierToGpfifo_IMPL(pGpu, pKernelWatchdog)
#endif // __nvoc_kernel_watchdog_h_disabled

void kwdtWatchdogCallbackVblankRecovery_IMPL(struct OBJGPU *pGpu, struct KernelWatchdog *pKernelWatchdog);
#ifdef __nvoc_kernel_watchdog_h_disabled
static inline void kwdtWatchdogCallbackVblankRecovery(struct OBJGPU *pGpu, struct KernelWatchdog *pKernelWatchdog) {
    NV_ASSERT_FAILED_PRECOMP("KernelWatchdog was disabled!");
}
#else // __nvoc_kernel_watchdog_h_disabled
#define kwdtWatchdogCallbackVblankRecovery(pGpu, pKernelWatchdog) kwdtWatchdogCallbackVblankRecovery_IMPL(pGpu, pKernelWatchdog)
#endif // __nvoc_kernel_watchdog_h_disabled

#ifdef __nvoc_kernel_watchdog_h_disabled
static inline void kwdtWatchdogCallbackPerf(struct OBJGPU *pGpu, struct KernelWatchdog *pKernelWatchdog) {
    NV_ASSERT_FAILED_PRECOMP("KernelWatchdog was disabled!");
}
#else // __nvoc_kernel_watchdog_h_disabled
#define kwdtWatchdogCallbackPerf(pGpu, pKernelWatchdog) kwdtWatchdogCallbackPerf_d44104(pGpu, pKernelWatchdog)
#endif // __nvoc_kernel_watchdog_h_disabled


// Wrapper macros for halified functions
#define kwdtWatchdogInit_FNPTR(pKernelWatchdog) pKernelWatchdog->__kwdtWatchdogInit__
#define kwdtWatchdogInit(pGpu, pKernelWatchdog) kwdtWatchdogInit_DISPATCH(pGpu, pKernelWatchdog)
#define kwdtWatchdogInit_HAL(pGpu, pKernelWatchdog) kwdtWatchdogInit_DISPATCH(pGpu, pKernelWatchdog)
#define kwdtWatchdogInitPushbuffer_FNPTR(pKernelWatchdog) pKernelWatchdog->__kwdtWatchdogInitPushbuffer__
#define kwdtWatchdogInitPushbuffer(pGpu, pKernelWatchdog) kwdtWatchdogInitPushbuffer_DISPATCH(pGpu, pKernelWatchdog)
#define kwdtWatchdogInitPushbuffer_HAL(pGpu, pKernelWatchdog) kwdtWatchdogInitPushbuffer_DISPATCH(pGpu, pKernelWatchdog)
#define kwdtWatchdog_FNPTR(pKernelWatchdog) pKernelWatchdog->__kwdtWatchdog__
#define kwdtWatchdog(pGpu, pKernelWatchdog) kwdtWatchdog_DISPATCH(pGpu, pKernelWatchdog)
#define kwdtWatchdog_HAL(pGpu, pKernelWatchdog) kwdtWatchdog_DISPATCH(pGpu, pKernelWatchdog)
#define kwdtWatchdogRecovery_FNPTR(pKernelWatchdog) pKernelWatchdog->__kwdtWatchdogRecovery__
#define kwdtWatchdogRecovery(pGpu, pKernelWatchdog) kwdtWatchdogRecovery_DISPATCH(pGpu, pKernelWatchdog)
#define kwdtWatchdogRecovery_HAL(pGpu, pKernelWatchdog) kwdtWatchdogRecovery_DISPATCH(pGpu, pKernelWatchdog)
#define kwdtWatchdogCallbackPerf_HAL(pGpu, pKernelWatchdog) kwdtWatchdogCallbackPerf(pGpu, pKernelWatchdog)
#define kwdtControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__
#define kwdtControl(pGpuResource, pCallContext, pParams) kwdtControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define kwdtMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__
#define kwdtMap(pGpuResource, pCallContext, pParams, pCpuMapping) kwdtMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define kwdtUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__
#define kwdtUnmap(pGpuResource, pCallContext, pCpuMapping) kwdtUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define kwdtShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__
#define kwdtShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) kwdtShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define kwdtGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__
#define kwdtGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) kwdtGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define kwdtGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__
#define kwdtGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) kwdtGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define kwdtInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__
#define kwdtInternalControlForward(pGpuResource, command, pParams, size) kwdtInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define kwdtGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__
#define kwdtGetInternalObjectHandle(pGpuResource) kwdtGetInternalObjectHandle_DISPATCH(pGpuResource)
#define kwdtAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__
#define kwdtAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) kwdtAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define kwdtGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define kwdtGetMemInterMapParams(pRmResource, pParams) kwdtGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define kwdtCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define kwdtCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) kwdtCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define kwdtGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define kwdtGetMemoryMappingDescriptor(pRmResource, ppMemDesc) kwdtGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define kwdtControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__
#define kwdtControlSerialization_Prologue(pResource, pCallContext, pParams) kwdtControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define kwdtControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__
#define kwdtControlSerialization_Epilogue(pResource, pCallContext, pParams) kwdtControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define kwdtControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__
#define kwdtControl_Prologue(pResource, pCallContext, pParams) kwdtControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define kwdtControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__
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
static inline NV_STATUS kwdtWatchdogInit_DISPATCH(struct OBJGPU *pGpu, struct KernelWatchdog *pKernelWatchdog) {
    return pKernelWatchdog->__kwdtWatchdogInit__(pGpu, pKernelWatchdog);
}

static inline void kwdtWatchdogInitPushbuffer_DISPATCH(struct OBJGPU *pGpu, struct KernelWatchdog *pKernelWatchdog) {
    pKernelWatchdog->__kwdtWatchdogInitPushbuffer__(pGpu, pKernelWatchdog);
}

static inline void kwdtWatchdog_DISPATCH(struct OBJGPU *pGpu, struct KernelWatchdog *pKernelWatchdog) {
    pKernelWatchdog->__kwdtWatchdog__(pGpu, pKernelWatchdog);
}

static inline void kwdtWatchdogRecovery_DISPATCH(struct OBJGPU *pGpu, struct KernelWatchdog *pKernelWatchdog) {
    pKernelWatchdog->__kwdtWatchdogRecovery__(pGpu, pKernelWatchdog);
}

static inline NV_STATUS kwdtControl_DISPATCH(struct KernelWatchdog *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__(staticCast(pGpuResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS kwdtMap_DISPATCH(struct KernelWatchdog *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__(staticCast(pGpuResource, RsResource), pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS kwdtUnmap_DISPATCH(struct KernelWatchdog *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__(staticCast(pGpuResource, RsResource), pCallContext, pCpuMapping);
}

static inline NvBool kwdtShareCallback_DISPATCH(struct KernelWatchdog *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__(staticCast(pGpuResource, RsResource), pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS kwdtGetRegBaseOffsetAndSize_DISPATCH(struct KernelWatchdog *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__(staticCast(pGpuResource, GpuResource), pGpu, pOffset, pSize);
}

static inline NV_STATUS kwdtGetMapAddrSpace_DISPATCH(struct KernelWatchdog *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__(staticCast(pGpuResource, GpuResource), pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS kwdtInternalControlForward_DISPATCH(struct KernelWatchdog *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__(staticCast(pGpuResource, GpuResource), command, pParams, size);
}

static inline NvHandle kwdtGetInternalObjectHandle_DISPATCH(struct KernelWatchdog *pGpuResource) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__(staticCast(pGpuResource, GpuResource));
}

static inline NvBool kwdtAccessCallback_DISPATCH(struct KernelWatchdog *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__(staticCast(pResource, RsResource), pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS kwdtGetMemInterMapParams_DISPATCH(struct KernelWatchdog *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__(staticCast(pRmResource, RmResource), pParams);
}

static inline NV_STATUS kwdtCheckMemInterUnmap_DISPATCH(struct KernelWatchdog *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__(staticCast(pRmResource, RmResource), bSubdeviceHandleProvided);
}

static inline NV_STATUS kwdtGetMemoryMappingDescriptor_DISPATCH(struct KernelWatchdog *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__(staticCast(pRmResource, RmResource), ppMemDesc);
}

static inline NV_STATUS kwdtControlSerialization_Prologue_DISPATCH(struct KernelWatchdog *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void kwdtControlSerialization_Epilogue_DISPATCH(struct KernelWatchdog *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS kwdtControl_Prologue_DISPATCH(struct KernelWatchdog *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void kwdtControl_Epilogue_DISPATCH(struct KernelWatchdog *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool kwdtCanCopy_DISPATCH(struct KernelWatchdog *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__(staticCast(pResource, RsResource));
}

static inline NV_STATUS kwdtIsDuplicate_DISPATCH(struct KernelWatchdog *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__(staticCast(pResource, RsResource), hMemory, pDuplicate);
}

static inline void kwdtPreDestruct_DISPATCH(struct KernelWatchdog *pResource) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__(staticCast(pResource, RsResource));
}

static inline NV_STATUS kwdtControlFilter_DISPATCH(struct KernelWatchdog *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool kwdtIsPartialUnmapSupported_DISPATCH(struct KernelWatchdog *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__(staticCast(pResource, RsResource));
}

static inline NV_STATUS kwdtMapTo_DISPATCH(struct KernelWatchdog *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__(staticCast(pResource, RsResource), pParams);
}

static inline NV_STATUS kwdtUnmapFrom_DISPATCH(struct KernelWatchdog *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__(staticCast(pResource, RsResource), pParams);
}

static inline NvU32 kwdtGetRefCount_DISPATCH(struct KernelWatchdog *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__(staticCast(pResource, RsResource));
}

static inline void kwdtAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct KernelWatchdog *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__(pClient, staticCast(pResource, RsResource), pReference);
}

// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
NV_STATUS kwdtWatchdogInit_IMPL(struct OBJGPU *pGpu, struct KernelWatchdog *pKernelWatchdog);

void kwdtWatchdogInitPushbuffer_IMPL(struct OBJGPU *pGpu, struct KernelWatchdog *pKernelWatchdog);

void kwdtWatchdog_IMPL(struct OBJGPU *pGpu, struct KernelWatchdog *pKernelWatchdog);

void kwdtWatchdogRecovery_KERNEL(struct OBJGPU *pGpu, struct KernelWatchdog *pKernelWatchdog);

// Inline HAL method definitions
static inline NV_STATUS kwdtWatchdogInit_395e98(struct OBJGPU *pGpu, struct KernelWatchdog *pKernelWatchdog){
    return NV_ERR_NOT_SUPPORTED;
}

static inline void kwdtWatchdogInitPushbuffer_d44104(struct OBJGPU *pGpu, struct KernelWatchdog *pKernelWatchdog){
    return;
}

static inline void kwdtWatchdog_d44104(struct OBJGPU *pGpu, struct KernelWatchdog *pKernelWatchdog){
    return;
}

static inline void kwdtWatchdogRecovery_f2d351(struct OBJGPU *pGpu, struct KernelWatchdog *pKernelWatchdog){
    NV_ASSERT_PRECOMP(0);
}

static inline void kwdtWatchdogCallbackPerf_d44104(struct OBJGPU *pGpu, struct KernelWatchdog *pKernelWatchdog){
    return;
}

// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


/*! Watchdog timer function */
void kwdtWatchdogTimerProc(OBJGPU *pGpu, void *);

#endif // _KERNEL_WATCHDOG_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_WATCHDOG_NVOC_H_
