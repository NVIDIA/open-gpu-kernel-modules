
#ifndef _G_KERNEL_CE_CONTEXT_NVOC_H_
#define _G_KERNEL_CE_CONTEXT_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 1993-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_kernel_ce_context_nvoc.h"

#ifndef KERNEL_CE_CONTEXT_H
#define KERNEL_CE_CONTEXT_H

#include "core/core.h"
#include "gpu/eng_state.h"
#include "gpu/gpu_halspec.h"
#include "gpu/gpu.h"
#include "kernel/gpu/fifo/channel_descendant.h"

ENGDESCRIPTOR kceGetEngineDescFromAllocParams(struct OBJGPU *pGpu, NvU32 externalClassId, void *pAllocParams);

/*!
 * RM internal class representing XXX_DMA_COPY_A
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_KERNEL_CE_CONTEXT_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__KernelCeContext;
struct NVOC_METADATA__ChannelDescendant;
struct NVOC_VTABLE__KernelCeContext;


struct KernelCeContext {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__KernelCeContext *__nvoc_metadata_ptr;
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
    struct KernelCeContext *__nvoc_pbase_KernelCeContext;    // kcectx
};


// Vtable with 32 per-class function pointers
struct NVOC_VTABLE__KernelCeContext {
    NV_STATUS (*__kcectxGetSwMethods__)(struct KernelCeContext * /*this*/, const METHOD **, NvU32 *);  // virtual inherited (chandes) base (chandes)
    NvBool (*__kcectxIsSwMethodStalling__)(struct KernelCeContext * /*this*/, NvU32);  // virtual inherited (chandes) base (chandes)
    NV_STATUS (*__kcectxCheckMemInterUnmap__)(struct KernelCeContext * /*this*/, NvBool);  // virtual inherited (chandes) base (chandes)
    NV_STATUS (*__kcectxControl__)(struct KernelCeContext * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (gpures) base (chandes)
    NV_STATUS (*__kcectxMap__)(struct KernelCeContext * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual inherited (gpures) base (chandes)
    NV_STATUS (*__kcectxUnmap__)(struct KernelCeContext * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual inherited (gpures) base (chandes)
    NvBool (*__kcectxShareCallback__)(struct KernelCeContext * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (chandes)
    NV_STATUS (*__kcectxGetRegBaseOffsetAndSize__)(struct KernelCeContext * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual inherited (gpures) base (chandes)
    NV_STATUS (*__kcectxGetMapAddrSpace__)(struct KernelCeContext * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (gpures) base (chandes)
    NV_STATUS (*__kcectxInternalControlForward__)(struct KernelCeContext * /*this*/, NvU32, void *, NvU32);  // virtual inherited (gpures) base (chandes)
    NvHandle (*__kcectxGetInternalObjectHandle__)(struct KernelCeContext * /*this*/);  // virtual inherited (gpures) base (chandes)
    NvBool (*__kcectxAccessCallback__)(struct KernelCeContext * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (chandes)
    NV_STATUS (*__kcectxGetMemInterMapParams__)(struct KernelCeContext * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (chandes)
    NV_STATUS (*__kcectxGetMemoryMappingDescriptor__)(struct KernelCeContext * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (chandes)
    NV_STATUS (*__kcectxControlSerialization_Prologue__)(struct KernelCeContext * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (chandes)
    void (*__kcectxControlSerialization_Epilogue__)(struct KernelCeContext * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (chandes)
    NV_STATUS (*__kcectxControl_Prologue__)(struct KernelCeContext * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (chandes)
    void (*__kcectxControl_Epilogue__)(struct KernelCeContext * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (chandes)
    NvBool (*__kcectxCanCopy__)(struct KernelCeContext * /*this*/);  // virtual inherited (res) base (chandes)
    NV_STATUS (*__kcectxIsDuplicate__)(struct KernelCeContext * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (chandes)
    void (*__kcectxPreDestruct__)(struct KernelCeContext * /*this*/);  // virtual inherited (res) base (chandes)
    NV_STATUS (*__kcectxControlFilter__)(struct KernelCeContext * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (chandes)
    NvBool (*__kcectxIsPartialUnmapSupported__)(struct KernelCeContext * /*this*/);  // inline virtual inherited (res) base (chandes) body
    NV_STATUS (*__kcectxMapTo__)(struct KernelCeContext * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (chandes)
    NV_STATUS (*__kcectxUnmapFrom__)(struct KernelCeContext * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (chandes)
    NvU32 (*__kcectxGetRefCount__)(struct KernelCeContext * /*this*/);  // virtual inherited (res) base (chandes)
    void (*__kcectxAddAdditionalDependants__)(struct RsClient *, struct KernelCeContext * /*this*/, RsResourceRef *);  // virtual inherited (res) base (chandes)
    PEVENTNOTIFICATION * (*__kcectxGetNotificationListPtr__)(struct KernelCeContext * /*this*/);  // virtual inherited (notify) base (chandes)
    struct NotifShare * (*__kcectxGetNotificationShare__)(struct KernelCeContext * /*this*/);  // virtual inherited (notify) base (chandes)
    void (*__kcectxSetNotificationShare__)(struct KernelCeContext * /*this*/, struct NotifShare *);  // virtual inherited (notify) base (chandes)
    NV_STATUS (*__kcectxUnregisterEvent__)(struct KernelCeContext * /*this*/, NvHandle, NvHandle, NvHandle, NvHandle);  // virtual inherited (notify) base (chandes)
    NV_STATUS (*__kcectxGetOrAllocNotifShare__)(struct KernelCeContext * /*this*/, NvHandle, NvHandle, struct NotifShare **);  // virtual inherited (notify) base (chandes)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__KernelCeContext {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__ChannelDescendant metadata__ChannelDescendant;
    const struct NVOC_VTABLE__KernelCeContext vtable;
};

#ifndef __NVOC_CLASS_KernelCeContext_TYPEDEF__
#define __NVOC_CLASS_KernelCeContext_TYPEDEF__
typedef struct KernelCeContext KernelCeContext;
#endif /* __NVOC_CLASS_KernelCeContext_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelCeContext
#define __nvoc_class_id_KernelCeContext 0x2d0ee9
#endif /* __nvoc_class_id_KernelCeContext */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelCeContext;

#define __staticCast_KernelCeContext(pThis) \
    ((pThis)->__nvoc_pbase_KernelCeContext)

#ifdef __nvoc_kernel_ce_context_h_disabled
#define __dynamicCast_KernelCeContext(pThis) ((KernelCeContext*) NULL)
#else //__nvoc_kernel_ce_context_h_disabled
#define __dynamicCast_KernelCeContext(pThis) \
    ((KernelCeContext*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelCeContext)))
#endif //__nvoc_kernel_ce_context_h_disabled

NV_STATUS __nvoc_objCreateDynamic_KernelCeContext(KernelCeContext**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelCeContext(KernelCeContext**, Dynamic*, NvU32, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __objCreate_KernelCeContext(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_KernelCeContext((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define kcectxGetSwMethods_FNPTR(pChannelDescendant) pChannelDescendant->__nvoc_base_ChannelDescendant.__nvoc_metadata_ptr->vtable.__chandesGetSwMethods__
#define kcectxGetSwMethods(pChannelDescendant, ppMethods, pNumMethods) kcectxGetSwMethods_DISPATCH(pChannelDescendant, ppMethods, pNumMethods)
#define kcectxIsSwMethodStalling_FNPTR(pChannelDescendant) pChannelDescendant->__nvoc_base_ChannelDescendant.__nvoc_metadata_ptr->vtable.__chandesIsSwMethodStalling__
#define kcectxIsSwMethodStalling(pChannelDescendant, hHandle) kcectxIsSwMethodStalling_DISPATCH(pChannelDescendant, hHandle)
#define kcectxCheckMemInterUnmap_FNPTR(pChannelDescendant) pChannelDescendant->__nvoc_base_ChannelDescendant.__nvoc_metadata_ptr->vtable.__chandesCheckMemInterUnmap__
#define kcectxCheckMemInterUnmap(pChannelDescendant, bSubdeviceHandleProvided) kcectxCheckMemInterUnmap_DISPATCH(pChannelDescendant, bSubdeviceHandleProvided)
#define kcectxControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresControl__
#define kcectxControl(pGpuResource, pCallContext, pParams) kcectxControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define kcectxMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresMap__
#define kcectxMap(pGpuResource, pCallContext, pParams, pCpuMapping) kcectxMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define kcectxUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresUnmap__
#define kcectxUnmap(pGpuResource, pCallContext, pCpuMapping) kcectxUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define kcectxShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresShareCallback__
#define kcectxShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) kcectxShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define kcectxGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__
#define kcectxGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) kcectxGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define kcectxGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__
#define kcectxGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) kcectxGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define kcectxInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__
#define kcectxInternalControlForward(pGpuResource, command, pParams, size) kcectxInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define kcectxGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__
#define kcectxGetInternalObjectHandle(pGpuResource) kcectxGetInternalObjectHandle_DISPATCH(pGpuResource)
#define kcectxAccessCallback_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresAccessCallback__
#define kcectxAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) kcectxAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define kcectxGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define kcectxGetMemInterMapParams(pRmResource, pParams) kcectxGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define kcectxGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define kcectxGetMemoryMappingDescriptor(pRmResource, ppMemDesc) kcectxGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define kcectxControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Prologue__
#define kcectxControlSerialization_Prologue(pResource, pCallContext, pParams) kcectxControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define kcectxControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Epilogue__
#define kcectxControlSerialization_Epilogue(pResource, pCallContext, pParams) kcectxControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define kcectxControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Prologue__
#define kcectxControl_Prologue(pResource, pCallContext, pParams) kcectxControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define kcectxControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Epilogue__
#define kcectxControl_Epilogue(pResource, pCallContext, pParams) kcectxControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define kcectxCanCopy_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define kcectxCanCopy(pResource) kcectxCanCopy_DISPATCH(pResource)
#define kcectxIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define kcectxIsDuplicate(pResource, hMemory, pDuplicate) kcectxIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define kcectxPreDestruct_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define kcectxPreDestruct(pResource) kcectxPreDestruct_DISPATCH(pResource)
#define kcectxControlFilter_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define kcectxControlFilter(pResource, pCallContext, pParams) kcectxControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define kcectxIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define kcectxIsPartialUnmapSupported(pResource) kcectxIsPartialUnmapSupported_DISPATCH(pResource)
#define kcectxMapTo_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define kcectxMapTo(pResource, pParams) kcectxMapTo_DISPATCH(pResource, pParams)
#define kcectxUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define kcectxUnmapFrom(pResource, pParams) kcectxUnmapFrom_DISPATCH(pResource, pParams)
#define kcectxGetRefCount_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define kcectxGetRefCount(pResource) kcectxGetRefCount_DISPATCH(pResource)
#define kcectxAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define kcectxAddAdditionalDependants(pClient, pResource, pReference) kcectxAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define kcectxGetNotificationListPtr_FNPTR(pNotifier) pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_metadata_ptr->vtable.__notifyGetNotificationListPtr__
#define kcectxGetNotificationListPtr(pNotifier) kcectxGetNotificationListPtr_DISPATCH(pNotifier)
#define kcectxGetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_metadata_ptr->vtable.__notifyGetNotificationShare__
#define kcectxGetNotificationShare(pNotifier) kcectxGetNotificationShare_DISPATCH(pNotifier)
#define kcectxSetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_metadata_ptr->vtable.__notifySetNotificationShare__
#define kcectxSetNotificationShare(pNotifier, pNotifShare) kcectxSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define kcectxUnregisterEvent_FNPTR(pNotifier) pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_metadata_ptr->vtable.__notifyUnregisterEvent__
#define kcectxUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) kcectxUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define kcectxGetOrAllocNotifShare_FNPTR(pNotifier) pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_metadata_ptr->vtable.__notifyGetOrAllocNotifShare__
#define kcectxGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) kcectxGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)

// Dispatch functions
static inline NV_STATUS kcectxGetSwMethods_DISPATCH(struct KernelCeContext *pChannelDescendant, const METHOD **ppMethods, NvU32 *pNumMethods) {
    return pChannelDescendant->__nvoc_metadata_ptr->vtable.__kcectxGetSwMethods__(pChannelDescendant, ppMethods, pNumMethods);
}

static inline NvBool kcectxIsSwMethodStalling_DISPATCH(struct KernelCeContext *pChannelDescendant, NvU32 hHandle) {
    return pChannelDescendant->__nvoc_metadata_ptr->vtable.__kcectxIsSwMethodStalling__(pChannelDescendant, hHandle);
}

static inline NV_STATUS kcectxCheckMemInterUnmap_DISPATCH(struct KernelCeContext *pChannelDescendant, NvBool bSubdeviceHandleProvided) {
    return pChannelDescendant->__nvoc_metadata_ptr->vtable.__kcectxCheckMemInterUnmap__(pChannelDescendant, bSubdeviceHandleProvided);
}

static inline NV_STATUS kcectxControl_DISPATCH(struct KernelCeContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__kcectxControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS kcectxMap_DISPATCH(struct KernelCeContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__kcectxMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS kcectxUnmap_DISPATCH(struct KernelCeContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__kcectxUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NvBool kcectxShareCallback_DISPATCH(struct KernelCeContext *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__kcectxShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS kcectxGetRegBaseOffsetAndSize_DISPATCH(struct KernelCeContext *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__kcectxGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS kcectxGetMapAddrSpace_DISPATCH(struct KernelCeContext *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__kcectxGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS kcectxInternalControlForward_DISPATCH(struct KernelCeContext *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__kcectxInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NvHandle kcectxGetInternalObjectHandle_DISPATCH(struct KernelCeContext *pGpuResource) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__kcectxGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool kcectxAccessCallback_DISPATCH(struct KernelCeContext *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_metadata_ptr->vtable.__kcectxAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS kcectxGetMemInterMapParams_DISPATCH(struct KernelCeContext *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__kcectxGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS kcectxGetMemoryMappingDescriptor_DISPATCH(struct KernelCeContext *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__kcectxGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS kcectxControlSerialization_Prologue_DISPATCH(struct KernelCeContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__kcectxControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void kcectxControlSerialization_Epilogue_DISPATCH(struct KernelCeContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__kcectxControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS kcectxControl_Prologue_DISPATCH(struct KernelCeContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__kcectxControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void kcectxControl_Epilogue_DISPATCH(struct KernelCeContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__kcectxControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool kcectxCanCopy_DISPATCH(struct KernelCeContext *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__kcectxCanCopy__(pResource);
}

static inline NV_STATUS kcectxIsDuplicate_DISPATCH(struct KernelCeContext *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_metadata_ptr->vtable.__kcectxIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void kcectxPreDestruct_DISPATCH(struct KernelCeContext *pResource) {
    pResource->__nvoc_metadata_ptr->vtable.__kcectxPreDestruct__(pResource);
}

static inline NV_STATUS kcectxControlFilter_DISPATCH(struct KernelCeContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__kcectxControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool kcectxIsPartialUnmapSupported_DISPATCH(struct KernelCeContext *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__kcectxIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS kcectxMapTo_DISPATCH(struct KernelCeContext *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__kcectxMapTo__(pResource, pParams);
}

static inline NV_STATUS kcectxUnmapFrom_DISPATCH(struct KernelCeContext *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__kcectxUnmapFrom__(pResource, pParams);
}

static inline NvU32 kcectxGetRefCount_DISPATCH(struct KernelCeContext *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__kcectxGetRefCount__(pResource);
}

static inline void kcectxAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct KernelCeContext *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_metadata_ptr->vtable.__kcectxAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline PEVENTNOTIFICATION * kcectxGetNotificationListPtr_DISPATCH(struct KernelCeContext *pNotifier) {
    return pNotifier->__nvoc_metadata_ptr->vtable.__kcectxGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare * kcectxGetNotificationShare_DISPATCH(struct KernelCeContext *pNotifier) {
    return pNotifier->__nvoc_metadata_ptr->vtable.__kcectxGetNotificationShare__(pNotifier);
}

static inline void kcectxSetNotificationShare_DISPATCH(struct KernelCeContext *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__nvoc_metadata_ptr->vtable.__kcectxSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NV_STATUS kcectxUnregisterEvent_DISPATCH(struct KernelCeContext *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__nvoc_metadata_ptr->vtable.__kcectxUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS kcectxGetOrAllocNotifShare_DISPATCH(struct KernelCeContext *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__nvoc_metadata_ptr->vtable.__kcectxGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

NV_STATUS kcectxConstruct_IMPL(struct KernelCeContext *arg_pKCeContext, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_kcectxConstruct(arg_pKCeContext, arg_pCallContext, arg_pParams) kcectxConstruct_IMPL(arg_pKCeContext, arg_pCallContext, arg_pParams)
void kcectxDestruct_IMPL(struct KernelCeContext *pKCeContext);

#define __nvoc_kcectxDestruct(pKCeContext) kcectxDestruct_IMPL(pKCeContext)
#undef PRIVATE_FIELD


#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_CE_CONTEXT_NVOC_H_
