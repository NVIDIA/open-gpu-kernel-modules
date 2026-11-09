
#ifndef _G_KERNEL_GRAPHICS_OBJECT_NVOC_H_
#define _G_KERNEL_GRAPHICS_OBJECT_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 1993-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_kernel_graphics_object_nvoc.h"

#ifndef _KERNEL_GRAPHICS_OBJECT_H_
#define _KERNEL_GRAPHICS_OBJECT_H_

#include "core/core.h"
#include "kernel/gpu/fifo/channel_descendant.h"
#include "ctrl/ctrl83de/ctrl83dedebug.h"
#include "mem_mgr/vaddr_list.h"
#include "utils/nv_enum.h"
#include "kernel/gpu/gr/kernel_graphics_context.h"

/*!
 * RM internal class representing 3D and compute graphics classes, e.g.: <arch>_A,
 * <arch>_COMPUTE_A, etc
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_KERNEL_GRAPHICS_OBJECT_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__KernelGraphicsObject;
struct NVOC_METADATA__ChannelDescendant;


struct KernelGraphicsObject {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__KernelGraphicsObject *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct ChannelDescendant __nvoc_base_ChannelDescendant;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct RsResource *__nvoc_pbase_RsResource;    // res super^4
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^4
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super^3
    struct GpuResource *__nvoc_pbase_GpuResource;    // gpures super^2
    struct INotifier *__nvoc_pbase_INotifier;    // inotify super^3
    struct Notifier *__nvoc_pbase_Notifier;    // notify super^2
    struct ChannelDescendant *__nvoc_pbase_ChannelDescendant;    // chandes super
    struct KernelGraphicsObject *__nvoc_pbase_KernelGraphicsObject;    // kgrobj

    // Vtable with 3 per-object function pointers
    void (*__kgrobjGetPromoteIds__)(struct OBJGPU *, struct KernelGraphicsObject * /*this*/, NvU32, NvU32 *, NvU32 *, NvBool *);  // halified (2 hals)
    NV_STATUS (*__kgrobjSetComputeMmio__)(struct OBJGPU *, struct KernelGraphicsObject * /*this*/);  // halified (2 hals) body
    void (*__kgrobjFreeComputeMmio__)(struct OBJGPU *, struct KernelGraphicsObject * /*this*/);  // halified (2 hals) body

    // Data members
    MEMORY_DESCRIPTOR *PRIVATE_FIELD(pMmioMemDesc);
    struct KernelGraphicsContext *PRIVATE_FIELD(pKernelGraphicsContext);
};


struct KernelGraphicsObject_PRIVATE {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__KernelGraphicsObject *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct ChannelDescendant __nvoc_base_ChannelDescendant;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct RsResource *__nvoc_pbase_RsResource;    // res super^4
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^4
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super^3
    struct GpuResource *__nvoc_pbase_GpuResource;    // gpures super^2
    struct INotifier *__nvoc_pbase_INotifier;    // inotify super^3
    struct Notifier *__nvoc_pbase_Notifier;    // notify super^2
    struct ChannelDescendant *__nvoc_pbase_ChannelDescendant;    // chandes super
    struct KernelGraphicsObject *__nvoc_pbase_KernelGraphicsObject;    // kgrobj

    // Vtable with 3 per-object function pointers
    void (*__kgrobjGetPromoteIds__)(struct OBJGPU *, struct KernelGraphicsObject * /*this*/, NvU32, NvU32 *, NvU32 *, NvBool *);  // halified (2 hals)
    NV_STATUS (*__kgrobjSetComputeMmio__)(struct OBJGPU *, struct KernelGraphicsObject * /*this*/);  // halified (2 hals) body
    void (*__kgrobjFreeComputeMmio__)(struct OBJGPU *, struct KernelGraphicsObject * /*this*/);  // halified (2 hals) body

    // Data members
    MEMORY_DESCRIPTOR *pMmioMemDesc;
    struct KernelGraphicsContext *pKernelGraphicsContext;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__KernelGraphicsObject {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__ChannelDescendant metadata__ChannelDescendant;
};

#ifndef __nvoc_class_id_KernelGraphicsObject
#define __nvoc_class_id_KernelGraphicsObject 0x097648u
typedef struct KernelGraphicsObject KernelGraphicsObject;
#endif /* __nvoc_class_id_KernelGraphicsObject */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelGraphicsObject;

#define __staticCast_KernelGraphicsObject(pThis) \
    ((pThis)->__nvoc_pbase_KernelGraphicsObject)

#ifdef __nvoc_kernel_graphics_object_h_disabled
#define __dynamicCast_KernelGraphicsObject(pThis) ((KernelGraphicsObject*) NULL)
#else //__nvoc_kernel_graphics_object_h_disabled
#define __dynamicCast_KernelGraphicsObject(pThis) \
    ((KernelGraphicsObject*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelGraphicsObject)))
#endif //__nvoc_kernel_graphics_object_h_disabled

NV_STATUS __nvoc_objCreateDynamic_KernelGraphicsObject(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelGraphicsObject(KernelGraphicsObject**, Dynamic*, NvU32, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __objCreate_KernelGraphicsObject(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams) \
    __nvoc_objCreate_KernelGraphicsObject((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pCallContext, pParams)


// Wrapper macros for implementation functions
NV_STATUS kgrobjConstruct_IMPL(struct KernelGraphicsObject *pKernelGraphicsObject, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __nvoc_kgrobjConstruct(pKernelGraphicsObject, pCallContext, pParams) kgrobjConstruct_IMPL(pKernelGraphicsObject, pCallContext, pParams)

void kgrobjDestruct_IMPL(struct KernelGraphicsObject *pKernelGraphicsObject);
#define __nvoc_kgrobjDestruct(pKernelGraphicsObject) kgrobjDestruct_IMPL(pKernelGraphicsObject)

NV_STATUS kgrobjPromoteContext_IMPL(struct OBJGPU *arg1, struct KernelGraphicsObject *arg_this, struct KernelGraphics *arg3);
#ifdef __nvoc_kernel_graphics_object_h_disabled
static inline NV_STATUS kgrobjPromoteContext(struct OBJGPU *arg1, struct KernelGraphicsObject *arg_this, struct KernelGraphics *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsObject was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_graphics_object_h_disabled
#define kgrobjPromoteContext(arg1, arg_this, arg3) kgrobjPromoteContext_IMPL(arg1, arg_this, arg3)
#endif // __nvoc_kernel_graphics_object_h_disabled

#ifdef __nvoc_kernel_graphics_object_h_disabled
static inline NvBool kgrobjShouldCleanup(struct OBJGPU *arg1, struct KernelGraphicsObject *arg_this) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsObject was disabled!");
    return NV_FALSE;
}
#else // __nvoc_kernel_graphics_object_h_disabled
#define kgrobjShouldCleanup(arg1, arg_this) kgrobjShouldCleanup_KERNEL(arg1, arg_this)
#endif // __nvoc_kernel_graphics_object_h_disabled


// Wrapper macros for halified functions
#define kgrobjGetMemInterMapParams_FNPTR(arg_this) arg_this->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define kgrobjGetMemInterMapParams(arg_this, arg2) kgrobjGetMemInterMapParams_DISPATCH(arg_this, arg2)
#define kgrobjGetPromoteIds_FNPTR(arg_this) arg_this->__kgrobjGetPromoteIds__
#define kgrobjGetPromoteIds(arg1, arg_this, maxPromoteIds, pPromoteIds, pNumEntries, pbPromote) kgrobjGetPromoteIds_DISPATCH(arg1, arg_this, maxPromoteIds, pPromoteIds, pNumEntries, pbPromote)
#define kgrobjGetPromoteIds_HAL(arg1, arg_this, maxPromoteIds, pPromoteIds, pNumEntries, pbPromote) kgrobjGetPromoteIds_DISPATCH(arg1, arg_this, maxPromoteIds, pPromoteIds, pNumEntries, pbPromote)
#define kgrobjShouldCleanup_HAL(arg1, arg_this) kgrobjShouldCleanup(arg1, arg_this)
#define kgrobjSetComputeMmio_FNPTR(arg_this) arg_this->__kgrobjSetComputeMmio__
#define kgrobjSetComputeMmio(arg1, arg_this) kgrobjSetComputeMmio_DISPATCH(arg1, arg_this)
#define kgrobjSetComputeMmio_HAL(arg1, arg_this) kgrobjSetComputeMmio_DISPATCH(arg1, arg_this)
#define kgrobjFreeComputeMmio_FNPTR(arg_this) arg_this->__kgrobjFreeComputeMmio__
#define kgrobjFreeComputeMmio(arg1, arg_this) kgrobjFreeComputeMmio_DISPATCH(arg1, arg_this)
#define kgrobjFreeComputeMmio_HAL(arg1, arg_this) kgrobjFreeComputeMmio_DISPATCH(arg1, arg_this)
#define kgrobjGetSwMethods_FNPTR(pChannelDescendant) pChannelDescendant->__nvoc_base_ChannelDescendant.__nvoc_metadata_ptr->vtable.__chandesGetSwMethods__
#define kgrobjGetSwMethods(pChannelDescendant, ppMethods, pNumMethods) kgrobjGetSwMethods_DISPATCH(pChannelDescendant, ppMethods, pNumMethods)
#define kgrobjIsSwMethodStalling_FNPTR(pChannelDescendant) pChannelDescendant->__nvoc_base_ChannelDescendant.__nvoc_metadata_ptr->vtable.__chandesIsSwMethodStalling__
#define kgrobjIsSwMethodStalling(pChannelDescendant, hHandle) kgrobjIsSwMethodStalling_DISPATCH(pChannelDescendant, hHandle)
#define kgrobjCheckMemInterUnmap_FNPTR(pChannelDescendant) pChannelDescendant->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define kgrobjCheckMemInterUnmap(pChannelDescendant, bSubdeviceHandleProvided) kgrobjCheckMemInterUnmap_DISPATCH(pChannelDescendant, bSubdeviceHandleProvided)
#define kgrobjControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__
#define kgrobjControl(pGpuResource, pCallContext, pParams) kgrobjControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define kgrobjMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__
#define kgrobjMap(pGpuResource, pCallContext, pParams, pCpuMapping) kgrobjMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define kgrobjUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__
#define kgrobjUnmap(pGpuResource, pCallContext, pCpuMapping) kgrobjUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define kgrobjShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__
#define kgrobjShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) kgrobjShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define kgrobjGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__
#define kgrobjGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) kgrobjGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define kgrobjGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__
#define kgrobjGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) kgrobjGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define kgrobjInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__
#define kgrobjInternalControlForward(pGpuResource, command, pParams, size) kgrobjInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define kgrobjGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__
#define kgrobjGetInternalObjectHandle(pGpuResource) kgrobjGetInternalObjectHandle_DISPATCH(pGpuResource)
#define kgrobjAccessCallback_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__
#define kgrobjAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) kgrobjAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define kgrobjGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define kgrobjGetMemoryMappingDescriptor(pRmResource, ppMemDesc) kgrobjGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define kgrobjControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__
#define kgrobjControlSerialization_Prologue(pResource, pCallContext, pParams) kgrobjControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define kgrobjControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__
#define kgrobjControlSerialization_Epilogue(pResource, pCallContext, pParams) kgrobjControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define kgrobjControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__
#define kgrobjControl_Prologue(pResource, pCallContext, pParams) kgrobjControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define kgrobjControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__
#define kgrobjControl_Epilogue(pResource, pCallContext, pParams) kgrobjControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define kgrobjCanCopy_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define kgrobjCanCopy(pResource) kgrobjCanCopy_DISPATCH(pResource)
#define kgrobjIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define kgrobjIsDuplicate(pResource, hMemory, pDuplicate) kgrobjIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define kgrobjPreDestruct_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define kgrobjPreDestruct(pResource) kgrobjPreDestruct_DISPATCH(pResource)
#define kgrobjControlFilter_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define kgrobjControlFilter(pResource, pCallContext, pParams) kgrobjControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define kgrobjIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define kgrobjIsPartialUnmapSupported(pResource) kgrobjIsPartialUnmapSupported_DISPATCH(pResource)
#define kgrobjMapTo_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define kgrobjMapTo(pResource, pParams) kgrobjMapTo_DISPATCH(pResource, pParams)
#define kgrobjUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define kgrobjUnmapFrom(pResource, pParams) kgrobjUnmapFrom_DISPATCH(pResource, pParams)
#define kgrobjGetRefCount_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define kgrobjGetRefCount(pResource) kgrobjGetRefCount_DISPATCH(pResource)
#define kgrobjAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define kgrobjAddAdditionalDependants(pClient, pResource, pReference) kgrobjAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define kgrobjGetNotificationListPtr_FNPTR(pNotifier) pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetNotificationListPtr__
#define kgrobjGetNotificationListPtr(pNotifier) kgrobjGetNotificationListPtr_DISPATCH(pNotifier)
#define kgrobjGetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetNotificationShare__
#define kgrobjGetNotificationShare(pNotifier) kgrobjGetNotificationShare_DISPATCH(pNotifier)
#define kgrobjSetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifySetNotificationShare__
#define kgrobjSetNotificationShare(pNotifier, pNotifShare) kgrobjSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define kgrobjUnregisterEvent_FNPTR(pNotifier) pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyUnregisterEvent__
#define kgrobjUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) kgrobjUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define kgrobjGetOrAllocNotifShare_FNPTR(pNotifier) pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetOrAllocNotifShare__
#define kgrobjGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) kgrobjGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)

// Dispatch functions
static inline NV_STATUS kgrobjGetMemInterMapParams_DISPATCH(struct KernelGraphicsObject *arg_this, RMRES_MEM_INTER_MAP_PARAMS *arg2) {
    return arg_this->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__(staticCast(arg_this, RmResource), arg2);
}

static inline void kgrobjGetPromoteIds_DISPATCH(struct OBJGPU *arg1, struct KernelGraphicsObject *arg_this, NvU32 maxPromoteIds, NvU32 *pPromoteIds, NvU32 *pNumEntries, NvBool *pbPromote) {
    arg_this->__kgrobjGetPromoteIds__(arg1, arg_this, maxPromoteIds, pPromoteIds, pNumEntries, pbPromote);
}

static inline NV_STATUS kgrobjSetComputeMmio_DISPATCH(struct OBJGPU *arg1, struct KernelGraphicsObject *arg_this) {
    return arg_this->__kgrobjSetComputeMmio__(arg1, arg_this);
}

static inline void kgrobjFreeComputeMmio_DISPATCH(struct OBJGPU *arg1, struct KernelGraphicsObject *arg_this) {
    arg_this->__kgrobjFreeComputeMmio__(arg1, arg_this);
}

static inline NV_STATUS kgrobjGetSwMethods_DISPATCH(struct KernelGraphicsObject *pChannelDescendant, const METHOD **ppMethods, NvU32 *pNumMethods) {
    return pChannelDescendant->__nvoc_base_ChannelDescendant.__nvoc_metadata_ptr->vtable.__chandesGetSwMethods__(staticCast(pChannelDescendant, ChannelDescendant), ppMethods, pNumMethods);
}

static inline NvBool kgrobjIsSwMethodStalling_DISPATCH(struct KernelGraphicsObject *pChannelDescendant, NvU32 hHandle) {
    return pChannelDescendant->__nvoc_base_ChannelDescendant.__nvoc_metadata_ptr->vtable.__chandesIsSwMethodStalling__(staticCast(pChannelDescendant, ChannelDescendant), hHandle);
}

static inline NV_STATUS kgrobjCheckMemInterUnmap_DISPATCH(struct KernelGraphicsObject *pChannelDescendant, NvBool bSubdeviceHandleProvided) {
    return pChannelDescendant->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__(staticCast(pChannelDescendant, RmResource), bSubdeviceHandleProvided);
}

static inline NV_STATUS kgrobjControl_DISPATCH(struct KernelGraphicsObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__(staticCast(pGpuResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS kgrobjMap_DISPATCH(struct KernelGraphicsObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__(staticCast(pGpuResource, RsResource), pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS kgrobjUnmap_DISPATCH(struct KernelGraphicsObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__(staticCast(pGpuResource, RsResource), pCallContext, pCpuMapping);
}

static inline NvBool kgrobjShareCallback_DISPATCH(struct KernelGraphicsObject *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__(staticCast(pGpuResource, RsResource), pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS kgrobjGetRegBaseOffsetAndSize_DISPATCH(struct KernelGraphicsObject *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__(staticCast(pGpuResource, GpuResource), pGpu, pOffset, pSize);
}

static inline NV_STATUS kgrobjGetMapAddrSpace_DISPATCH(struct KernelGraphicsObject *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__(staticCast(pGpuResource, GpuResource), pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS kgrobjInternalControlForward_DISPATCH(struct KernelGraphicsObject *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__(staticCast(pGpuResource, GpuResource), command, pParams, size);
}

static inline NvHandle kgrobjGetInternalObjectHandle_DISPATCH(struct KernelGraphicsObject *pGpuResource) {
    return pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__(staticCast(pGpuResource, GpuResource));
}

static inline NvBool kgrobjAccessCallback_DISPATCH(struct KernelGraphicsObject *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__(staticCast(pResource, RsResource), pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS kgrobjGetMemoryMappingDescriptor_DISPATCH(struct KernelGraphicsObject *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__(staticCast(pRmResource, RmResource), ppMemDesc);
}

static inline NV_STATUS kgrobjControlSerialization_Prologue_DISPATCH(struct KernelGraphicsObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void kgrobjControlSerialization_Epilogue_DISPATCH(struct KernelGraphicsObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS kgrobjControl_Prologue_DISPATCH(struct KernelGraphicsObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void kgrobjControl_Epilogue_DISPATCH(struct KernelGraphicsObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool kgrobjCanCopy_DISPATCH(struct KernelGraphicsObject *pResource) {
    return pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__(staticCast(pResource, RsResource));
}

static inline NV_STATUS kgrobjIsDuplicate_DISPATCH(struct KernelGraphicsObject *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__(staticCast(pResource, RsResource), hMemory, pDuplicate);
}

static inline void kgrobjPreDestruct_DISPATCH(struct KernelGraphicsObject *pResource) {
    pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__(staticCast(pResource, RsResource));
}

static inline NV_STATUS kgrobjControlFilter_DISPATCH(struct KernelGraphicsObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool kgrobjIsPartialUnmapSupported_DISPATCH(struct KernelGraphicsObject *pResource) {
    return pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__(staticCast(pResource, RsResource));
}

static inline NV_STATUS kgrobjMapTo_DISPATCH(struct KernelGraphicsObject *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__(staticCast(pResource, RsResource), pParams);
}

static inline NV_STATUS kgrobjUnmapFrom_DISPATCH(struct KernelGraphicsObject *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__(staticCast(pResource, RsResource), pParams);
}

static inline NvU32 kgrobjGetRefCount_DISPATCH(struct KernelGraphicsObject *pResource) {
    return pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__(staticCast(pResource, RsResource));
}

static inline void kgrobjAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct KernelGraphicsObject *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__(pClient, staticCast(pResource, RsResource), pReference);
}

static inline PEVENTNOTIFICATION * kgrobjGetNotificationListPtr_DISPATCH(struct KernelGraphicsObject *pNotifier) {
    return pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetNotificationListPtr__(staticCast(pNotifier, INotifier));
}

static inline struct NotifShare * kgrobjGetNotificationShare_DISPATCH(struct KernelGraphicsObject *pNotifier) {
    return pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetNotificationShare__(staticCast(pNotifier, INotifier));
}

static inline void kgrobjSetNotificationShare_DISPATCH(struct KernelGraphicsObject *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifySetNotificationShare__(staticCast(pNotifier, INotifier), pNotifShare);
}

static inline NV_STATUS kgrobjUnregisterEvent_DISPATCH(struct KernelGraphicsObject *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyUnregisterEvent__(staticCast(pNotifier, INotifier), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS kgrobjGetOrAllocNotifShare_DISPATCH(struct KernelGraphicsObject *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetOrAllocNotifShare__(staticCast(pNotifier, INotifier), hNotifierClient, hNotifierResource, ppNotifShare);
}

// Virtual method declarations and/or inline definitions
NV_STATUS kgrobjGetMemInterMapParams_IMPL(struct KernelGraphicsObject *arg_this, RMRES_MEM_INTER_MAP_PARAMS *arg2);

// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
void kgrobjGetPromoteIds_VF(struct OBJGPU *arg1, struct KernelGraphicsObject *arg_this, NvU32 maxPromoteIds, NvU32 *pPromoteIds, NvU32 *pNumEntries, NvBool *pbPromote);

void kgrobjGetPromoteIds_FWCLIENT(struct OBJGPU *arg1, struct KernelGraphicsObject *arg_this, NvU32 maxPromoteIds, NvU32 *pPromoteIds, NvU32 *pNumEntries, NvBool *pbPromote);

NvBool kgrobjShouldCleanup_KERNEL(struct OBJGPU *arg1, struct KernelGraphicsObject *arg_this);

NV_STATUS kgrobjSetComputeMmio_IMPL(struct OBJGPU *arg1, struct KernelGraphicsObject *arg_this);

void kgrobjFreeComputeMmio_IMPL(struct OBJGPU *arg1, struct KernelGraphicsObject *arg_this);

// Inline HAL method definitions
static inline NV_STATUS kgrobjSetComputeMmio_ac1694(struct OBJGPU *arg1, struct KernelGraphicsObject *arg_this){
    return NV_OK;
}

static inline void kgrobjFreeComputeMmio_d44104(struct OBJGPU *arg1, struct KernelGraphicsObject *arg_this){
    return;
}

// Static dispatch method declarations
// Static inline method definitions
static inline struct KernelGraphicsContext * kgrobjGetKernelGraphicsContext(struct OBJGPU *pGpu, struct KernelGraphicsObject *pKernelGraphicsObject){
    struct KernelGraphicsObject_PRIVATE *pKernelGraphicsObject_PRIVATE = (struct KernelGraphicsObject_PRIVATE *)pKernelGraphicsObject;
    return pKernelGraphicsObject_PRIVATE->pKernelGraphicsContext;
}

#undef PRIVATE_FIELD


#endif // _KERNEL_GRAPHICS_OBJECT_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_GRAPHICS_OBJECT_NVOC_H_
