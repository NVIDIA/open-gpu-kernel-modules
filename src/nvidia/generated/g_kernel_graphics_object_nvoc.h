
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


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__KernelGraphicsObject;
struct NVOC_METADATA__ChannelDescendant;
struct NVOC_VTABLE__KernelGraphicsObject;


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

    // Vtable with 1 per-object function pointer
    void (*__kgrobjGetPromoteIds__)(struct OBJGPU *, struct KernelGraphicsObject * /*this*/, NvU32, NvU32 *, NvU32 *, NvBool *);  // halified (2 hals)

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

    // Vtable with 1 per-object function pointer
    void (*__kgrobjGetPromoteIds__)(struct OBJGPU *, struct KernelGraphicsObject * /*this*/, NvU32, NvU32 *, NvU32 *, NvBool *);  // halified (2 hals)

    // Data members
    MEMORY_DESCRIPTOR *pMmioMemDesc;
    struct KernelGraphicsContext *pKernelGraphicsContext;
};


// Vtable with 32 per-class function pointers
struct NVOC_VTABLE__KernelGraphicsObject {
    NV_STATUS (*__kgrobjGetMemInterMapParams__)(struct KernelGraphicsObject * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual override (rmres) base (chandes)
    NV_STATUS (*__kgrobjGetSwMethods__)(struct KernelGraphicsObject * /*this*/, const METHOD **, NvU32 *);  // virtual inherited (chandes) base (chandes)
    NvBool (*__kgrobjIsSwMethodStalling__)(struct KernelGraphicsObject * /*this*/, NvU32);  // virtual inherited (chandes) base (chandes)
    NV_STATUS (*__kgrobjCheckMemInterUnmap__)(struct KernelGraphicsObject * /*this*/, NvBool);  // virtual inherited (chandes) base (chandes)
    NV_STATUS (*__kgrobjControl__)(struct KernelGraphicsObject * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (gpures) base (chandes)
    NV_STATUS (*__kgrobjMap__)(struct KernelGraphicsObject * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual inherited (gpures) base (chandes)
    NV_STATUS (*__kgrobjUnmap__)(struct KernelGraphicsObject * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual inherited (gpures) base (chandes)
    NvBool (*__kgrobjShareCallback__)(struct KernelGraphicsObject * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (chandes)
    NV_STATUS (*__kgrobjGetRegBaseOffsetAndSize__)(struct KernelGraphicsObject * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual inherited (gpures) base (chandes)
    NV_STATUS (*__kgrobjGetMapAddrSpace__)(struct KernelGraphicsObject * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (gpures) base (chandes)
    NV_STATUS (*__kgrobjInternalControlForward__)(struct KernelGraphicsObject * /*this*/, NvU32, void *, NvU32);  // virtual inherited (gpures) base (chandes)
    NvHandle (*__kgrobjGetInternalObjectHandle__)(struct KernelGraphicsObject * /*this*/);  // virtual inherited (gpures) base (chandes)
    NvBool (*__kgrobjAccessCallback__)(struct KernelGraphicsObject * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (chandes)
    NV_STATUS (*__kgrobjGetMemoryMappingDescriptor__)(struct KernelGraphicsObject * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (chandes)
    NV_STATUS (*__kgrobjControlSerialization_Prologue__)(struct KernelGraphicsObject * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (chandes)
    void (*__kgrobjControlSerialization_Epilogue__)(struct KernelGraphicsObject * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (chandes)
    NV_STATUS (*__kgrobjControl_Prologue__)(struct KernelGraphicsObject * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (chandes)
    void (*__kgrobjControl_Epilogue__)(struct KernelGraphicsObject * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (chandes)
    NvBool (*__kgrobjCanCopy__)(struct KernelGraphicsObject * /*this*/);  // virtual inherited (res) base (chandes)
    NV_STATUS (*__kgrobjIsDuplicate__)(struct KernelGraphicsObject * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (chandes)
    void (*__kgrobjPreDestruct__)(struct KernelGraphicsObject * /*this*/);  // virtual inherited (res) base (chandes)
    NV_STATUS (*__kgrobjControlFilter__)(struct KernelGraphicsObject * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (chandes)
    NvBool (*__kgrobjIsPartialUnmapSupported__)(struct KernelGraphicsObject * /*this*/);  // inline virtual inherited (res) base (chandes) body
    NV_STATUS (*__kgrobjMapTo__)(struct KernelGraphicsObject * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (chandes)
    NV_STATUS (*__kgrobjUnmapFrom__)(struct KernelGraphicsObject * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (chandes)
    NvU32 (*__kgrobjGetRefCount__)(struct KernelGraphicsObject * /*this*/);  // virtual inherited (res) base (chandes)
    void (*__kgrobjAddAdditionalDependants__)(struct RsClient *, struct KernelGraphicsObject * /*this*/, RsResourceRef *);  // virtual inherited (res) base (chandes)
    PEVENTNOTIFICATION * (*__kgrobjGetNotificationListPtr__)(struct KernelGraphicsObject * /*this*/);  // virtual inherited (notify) base (chandes)
    struct NotifShare * (*__kgrobjGetNotificationShare__)(struct KernelGraphicsObject * /*this*/);  // virtual inherited (notify) base (chandes)
    void (*__kgrobjSetNotificationShare__)(struct KernelGraphicsObject * /*this*/, struct NotifShare *);  // virtual inherited (notify) base (chandes)
    NV_STATUS (*__kgrobjUnregisterEvent__)(struct KernelGraphicsObject * /*this*/, NvHandle, NvHandle, NvHandle, NvHandle);  // virtual inherited (notify) base (chandes)
    NV_STATUS (*__kgrobjGetOrAllocNotifShare__)(struct KernelGraphicsObject * /*this*/, NvHandle, NvHandle, struct NotifShare **);  // virtual inherited (notify) base (chandes)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__KernelGraphicsObject {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__ChannelDescendant metadata__ChannelDescendant;
    const struct NVOC_VTABLE__KernelGraphicsObject vtable;
};

#ifndef __NVOC_CLASS_KernelGraphicsObject_TYPEDEF__
#define __NVOC_CLASS_KernelGraphicsObject_TYPEDEF__
typedef struct KernelGraphicsObject KernelGraphicsObject;
#endif /* __NVOC_CLASS_KernelGraphicsObject_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelGraphicsObject
#define __nvoc_class_id_KernelGraphicsObject 0x097648
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

NV_STATUS __nvoc_objCreateDynamic_KernelGraphicsObject(KernelGraphicsObject**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelGraphicsObject(KernelGraphicsObject**, Dynamic*, NvU32, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __objCreate_KernelGraphicsObject(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_KernelGraphicsObject((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define kgrobjGetMemInterMapParams_FNPTR(arg_this) arg_this->__nvoc_metadata_ptr->vtable.__kgrobjGetMemInterMapParams__
#define kgrobjGetMemInterMapParams(arg_this, arg2) kgrobjGetMemInterMapParams_DISPATCH(arg_this, arg2)
#define kgrobjGetPromoteIds_FNPTR(arg_this) arg_this->__kgrobjGetPromoteIds__
#define kgrobjGetPromoteIds(arg1, arg_this, maxPromoteIds, pPromoteIds, pNumEntries, pbPromote) kgrobjGetPromoteIds_DISPATCH(arg1, arg_this, maxPromoteIds, pPromoteIds, pNumEntries, pbPromote)
#define kgrobjGetPromoteIds_HAL(arg1, arg_this, maxPromoteIds, pPromoteIds, pNumEntries, pbPromote) kgrobjGetPromoteIds_DISPATCH(arg1, arg_this, maxPromoteIds, pPromoteIds, pNumEntries, pbPromote)
#define kgrobjGetSwMethods_FNPTR(pChannelDescendant) pChannelDescendant->__nvoc_base_ChannelDescendant.__nvoc_metadata_ptr->vtable.__chandesGetSwMethods__
#define kgrobjGetSwMethods(pChannelDescendant, ppMethods, pNumMethods) kgrobjGetSwMethods_DISPATCH(pChannelDescendant, ppMethods, pNumMethods)
#define kgrobjIsSwMethodStalling_FNPTR(pChannelDescendant) pChannelDescendant->__nvoc_base_ChannelDescendant.__nvoc_metadata_ptr->vtable.__chandesIsSwMethodStalling__
#define kgrobjIsSwMethodStalling(pChannelDescendant, hHandle) kgrobjIsSwMethodStalling_DISPATCH(pChannelDescendant, hHandle)
#define kgrobjCheckMemInterUnmap_FNPTR(pChannelDescendant) pChannelDescendant->__nvoc_base_ChannelDescendant.__nvoc_metadata_ptr->vtable.__chandesCheckMemInterUnmap__
#define kgrobjCheckMemInterUnmap(pChannelDescendant, bSubdeviceHandleProvided) kgrobjCheckMemInterUnmap_DISPATCH(pChannelDescendant, bSubdeviceHandleProvided)
#define kgrobjControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresControl__
#define kgrobjControl(pGpuResource, pCallContext, pParams) kgrobjControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define kgrobjMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresMap__
#define kgrobjMap(pGpuResource, pCallContext, pParams, pCpuMapping) kgrobjMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define kgrobjUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresUnmap__
#define kgrobjUnmap(pGpuResource, pCallContext, pCpuMapping) kgrobjUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define kgrobjShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresShareCallback__
#define kgrobjShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) kgrobjShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define kgrobjGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__
#define kgrobjGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) kgrobjGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define kgrobjGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__
#define kgrobjGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) kgrobjGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define kgrobjInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__
#define kgrobjInternalControlForward(pGpuResource, command, pParams, size) kgrobjInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define kgrobjGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__
#define kgrobjGetInternalObjectHandle(pGpuResource) kgrobjGetInternalObjectHandle_DISPATCH(pGpuResource)
#define kgrobjAccessCallback_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresAccessCallback__
#define kgrobjAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) kgrobjAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define kgrobjGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define kgrobjGetMemoryMappingDescriptor(pRmResource, ppMemDesc) kgrobjGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define kgrobjControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Prologue__
#define kgrobjControlSerialization_Prologue(pResource, pCallContext, pParams) kgrobjControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define kgrobjControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Epilogue__
#define kgrobjControlSerialization_Epilogue(pResource, pCallContext, pParams) kgrobjControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define kgrobjControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Prologue__
#define kgrobjControl_Prologue(pResource, pCallContext, pParams) kgrobjControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define kgrobjControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Epilogue__
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
#define kgrobjGetNotificationListPtr_FNPTR(pNotifier) pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_metadata_ptr->vtable.__notifyGetNotificationListPtr__
#define kgrobjGetNotificationListPtr(pNotifier) kgrobjGetNotificationListPtr_DISPATCH(pNotifier)
#define kgrobjGetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_metadata_ptr->vtable.__notifyGetNotificationShare__
#define kgrobjGetNotificationShare(pNotifier) kgrobjGetNotificationShare_DISPATCH(pNotifier)
#define kgrobjSetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_metadata_ptr->vtable.__notifySetNotificationShare__
#define kgrobjSetNotificationShare(pNotifier, pNotifShare) kgrobjSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define kgrobjUnregisterEvent_FNPTR(pNotifier) pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_metadata_ptr->vtable.__notifyUnregisterEvent__
#define kgrobjUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) kgrobjUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define kgrobjGetOrAllocNotifShare_FNPTR(pNotifier) pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_metadata_ptr->vtable.__notifyGetOrAllocNotifShare__
#define kgrobjGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) kgrobjGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)

// Dispatch functions
static inline NV_STATUS kgrobjGetMemInterMapParams_DISPATCH(struct KernelGraphicsObject *arg_this, RMRES_MEM_INTER_MAP_PARAMS *arg2) {
    return arg_this->__nvoc_metadata_ptr->vtable.__kgrobjGetMemInterMapParams__(arg_this, arg2);
}

static inline void kgrobjGetPromoteIds_DISPATCH(struct OBJGPU *arg1, struct KernelGraphicsObject *arg_this, NvU32 maxPromoteIds, NvU32 *pPromoteIds, NvU32 *pNumEntries, NvBool *pbPromote) {
    arg_this->__kgrobjGetPromoteIds__(arg1, arg_this, maxPromoteIds, pPromoteIds, pNumEntries, pbPromote);
}

static inline NV_STATUS kgrobjGetSwMethods_DISPATCH(struct KernelGraphicsObject *pChannelDescendant, const METHOD **ppMethods, NvU32 *pNumMethods) {
    return pChannelDescendant->__nvoc_metadata_ptr->vtable.__kgrobjGetSwMethods__(pChannelDescendant, ppMethods, pNumMethods);
}

static inline NvBool kgrobjIsSwMethodStalling_DISPATCH(struct KernelGraphicsObject *pChannelDescendant, NvU32 hHandle) {
    return pChannelDescendant->__nvoc_metadata_ptr->vtable.__kgrobjIsSwMethodStalling__(pChannelDescendant, hHandle);
}

static inline NV_STATUS kgrobjCheckMemInterUnmap_DISPATCH(struct KernelGraphicsObject *pChannelDescendant, NvBool bSubdeviceHandleProvided) {
    return pChannelDescendant->__nvoc_metadata_ptr->vtable.__kgrobjCheckMemInterUnmap__(pChannelDescendant, bSubdeviceHandleProvided);
}

static inline NV_STATUS kgrobjControl_DISPATCH(struct KernelGraphicsObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__kgrobjControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS kgrobjMap_DISPATCH(struct KernelGraphicsObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__kgrobjMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS kgrobjUnmap_DISPATCH(struct KernelGraphicsObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__kgrobjUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NvBool kgrobjShareCallback_DISPATCH(struct KernelGraphicsObject *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__kgrobjShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS kgrobjGetRegBaseOffsetAndSize_DISPATCH(struct KernelGraphicsObject *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__kgrobjGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS kgrobjGetMapAddrSpace_DISPATCH(struct KernelGraphicsObject *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__kgrobjGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS kgrobjInternalControlForward_DISPATCH(struct KernelGraphicsObject *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__kgrobjInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NvHandle kgrobjGetInternalObjectHandle_DISPATCH(struct KernelGraphicsObject *pGpuResource) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__kgrobjGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool kgrobjAccessCallback_DISPATCH(struct KernelGraphicsObject *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_metadata_ptr->vtable.__kgrobjAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS kgrobjGetMemoryMappingDescriptor_DISPATCH(struct KernelGraphicsObject *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__kgrobjGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS kgrobjControlSerialization_Prologue_DISPATCH(struct KernelGraphicsObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__kgrobjControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void kgrobjControlSerialization_Epilogue_DISPATCH(struct KernelGraphicsObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__kgrobjControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS kgrobjControl_Prologue_DISPATCH(struct KernelGraphicsObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__kgrobjControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void kgrobjControl_Epilogue_DISPATCH(struct KernelGraphicsObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__kgrobjControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool kgrobjCanCopy_DISPATCH(struct KernelGraphicsObject *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__kgrobjCanCopy__(pResource);
}

static inline NV_STATUS kgrobjIsDuplicate_DISPATCH(struct KernelGraphicsObject *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_metadata_ptr->vtable.__kgrobjIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void kgrobjPreDestruct_DISPATCH(struct KernelGraphicsObject *pResource) {
    pResource->__nvoc_metadata_ptr->vtable.__kgrobjPreDestruct__(pResource);
}

static inline NV_STATUS kgrobjControlFilter_DISPATCH(struct KernelGraphicsObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__kgrobjControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool kgrobjIsPartialUnmapSupported_DISPATCH(struct KernelGraphicsObject *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__kgrobjIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS kgrobjMapTo_DISPATCH(struct KernelGraphicsObject *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__kgrobjMapTo__(pResource, pParams);
}

static inline NV_STATUS kgrobjUnmapFrom_DISPATCH(struct KernelGraphicsObject *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__kgrobjUnmapFrom__(pResource, pParams);
}

static inline NvU32 kgrobjGetRefCount_DISPATCH(struct KernelGraphicsObject *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__kgrobjGetRefCount__(pResource);
}

static inline void kgrobjAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct KernelGraphicsObject *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_metadata_ptr->vtable.__kgrobjAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline PEVENTNOTIFICATION * kgrobjGetNotificationListPtr_DISPATCH(struct KernelGraphicsObject *pNotifier) {
    return pNotifier->__nvoc_metadata_ptr->vtable.__kgrobjGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare * kgrobjGetNotificationShare_DISPATCH(struct KernelGraphicsObject *pNotifier) {
    return pNotifier->__nvoc_metadata_ptr->vtable.__kgrobjGetNotificationShare__(pNotifier);
}

static inline void kgrobjSetNotificationShare_DISPATCH(struct KernelGraphicsObject *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__nvoc_metadata_ptr->vtable.__kgrobjSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NV_STATUS kgrobjUnregisterEvent_DISPATCH(struct KernelGraphicsObject *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__nvoc_metadata_ptr->vtable.__kgrobjUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS kgrobjGetOrAllocNotifShare_DISPATCH(struct KernelGraphicsObject *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__nvoc_metadata_ptr->vtable.__kgrobjGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

NvBool kgrobjShouldCleanup_KERNEL(struct OBJGPU *arg1, struct KernelGraphicsObject *arg2);


#ifdef __nvoc_kernel_graphics_object_h_disabled
static inline NvBool kgrobjShouldCleanup(struct OBJGPU *arg1, struct KernelGraphicsObject *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsObject was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_graphics_object_h_disabled
#define kgrobjShouldCleanup(arg1, arg2) kgrobjShouldCleanup_KERNEL(arg1, arg2)
#endif //__nvoc_kernel_graphics_object_h_disabled

#define kgrobjShouldCleanup_HAL(arg1, arg2) kgrobjShouldCleanup(arg1, arg2)

NV_STATUS kgrobjSetComputeMmio_IMPL(struct OBJGPU *arg1, struct KernelGraphicsObject *arg2);


#ifdef __nvoc_kernel_graphics_object_h_disabled
static inline NV_STATUS kgrobjSetComputeMmio(struct OBJGPU *arg1, struct KernelGraphicsObject *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsObject was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_object_h_disabled
#define kgrobjSetComputeMmio(arg1, arg2) kgrobjSetComputeMmio_IMPL(arg1, arg2)
#endif //__nvoc_kernel_graphics_object_h_disabled

#define kgrobjSetComputeMmio_HAL(arg1, arg2) kgrobjSetComputeMmio(arg1, arg2)

void kgrobjFreeComputeMmio_IMPL(struct OBJGPU *arg1, struct KernelGraphicsObject *arg2);


#ifdef __nvoc_kernel_graphics_object_h_disabled
static inline void kgrobjFreeComputeMmio(struct OBJGPU *arg1, struct KernelGraphicsObject *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsObject was disabled!");
}
#else //__nvoc_kernel_graphics_object_h_disabled
#define kgrobjFreeComputeMmio(arg1, arg2) kgrobjFreeComputeMmio_IMPL(arg1, arg2)
#endif //__nvoc_kernel_graphics_object_h_disabled

#define kgrobjFreeComputeMmio_HAL(arg1, arg2) kgrobjFreeComputeMmio(arg1, arg2)

NV_STATUS kgrobjGetMemInterMapParams_IMPL(struct KernelGraphicsObject *arg1, RMRES_MEM_INTER_MAP_PARAMS *arg2);

void kgrobjGetPromoteIds_VF(struct OBJGPU *arg1, struct KernelGraphicsObject *arg2, NvU32 maxPromoteIds, NvU32 *pPromoteIds, NvU32 *pNumEntries, NvBool *pbPromote);

void kgrobjGetPromoteIds_FWCLIENT(struct OBJGPU *arg1, struct KernelGraphicsObject *arg2, NvU32 maxPromoteIds, NvU32 *pPromoteIds, NvU32 *pNumEntries, NvBool *pbPromote);

static inline struct KernelGraphicsContext *kgrobjGetKernelGraphicsContext(struct OBJGPU *pGpu, struct KernelGraphicsObject *pKernelGraphicsObject) {
    struct KernelGraphicsObject_PRIVATE *pKernelGraphicsObject_PRIVATE = (struct KernelGraphicsObject_PRIVATE *)pKernelGraphicsObject;
    return pKernelGraphicsObject_PRIVATE->pKernelGraphicsContext;
}

NV_STATUS kgrobjConstruct_IMPL(struct KernelGraphicsObject *arg_pKernelGraphicsObject, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_kgrobjConstruct(arg_pKernelGraphicsObject, arg_pCallContext, arg_pParams) kgrobjConstruct_IMPL(arg_pKernelGraphicsObject, arg_pCallContext, arg_pParams)
void kgrobjDestruct_IMPL(struct KernelGraphicsObject *pKernelGraphicsObject);

#define __nvoc_kgrobjDestruct(pKernelGraphicsObject) kgrobjDestruct_IMPL(pKernelGraphicsObject)
NV_STATUS kgrobjPromoteContext_IMPL(struct OBJGPU *arg1, struct KernelGraphicsObject *arg2, struct KernelGraphics *arg3);

#ifdef __nvoc_kernel_graphics_object_h_disabled
static inline NV_STATUS kgrobjPromoteContext(struct OBJGPU *arg1, struct KernelGraphicsObject *arg2, struct KernelGraphics *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsObject was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_object_h_disabled
#define kgrobjPromoteContext(arg1, arg2, arg3) kgrobjPromoteContext_IMPL(arg1, arg2, arg3)
#endif //__nvoc_kernel_graphics_object_h_disabled

#undef PRIVATE_FIELD


#endif // _KERNEL_GRAPHICS_OBJECT_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_GRAPHICS_OBJECT_NVOC_H_
