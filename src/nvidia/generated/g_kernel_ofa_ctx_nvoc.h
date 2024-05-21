
#ifndef _G_KERNEL_OFA_CTX_NVOC_H_
#define _G_KERNEL_OFA_CTX_NVOC_H_
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

#pragma once
#include "g_kernel_ofa_ctx_nvoc.h"

#ifndef KERNEL_OFA_CTX_H
#define KERNEL_OFA_CTX_H

#include "kernel/gpu/fifo/channel_descendant.h"

ENGDESCRIPTOR ofaGetEngineDescFromAllocParams(OBJGPU *pGpu, NvU32 externalClassId, void *pAllocParams);

/*!
 * RM internal class representing NVXXXX_VIDEO_OFA
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_KERNEL_OFA_CTX_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct OfaContext {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
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
    struct OfaContext *__nvoc_pbase_OfaContext;    // ofactx

    // Vtable with 32 per-object function pointers
    NV_STATUS (*__ofactxGetSwMethods__)(struct OfaContext * /*this*/, const METHOD **, NvU32 *);  // virtual inherited (chandes) base (chandes)
    NvBool (*__ofactxIsSwMethodStalling__)(struct OfaContext * /*this*/, NvU32);  // virtual inherited (chandes) base (chandes)
    NV_STATUS (*__ofactxCheckMemInterUnmap__)(struct OfaContext * /*this*/, NvBool);  // virtual inherited (chandes) base (chandes)
    NV_STATUS (*__ofactxControl__)(struct OfaContext * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (gpures) base (chandes)
    NV_STATUS (*__ofactxMap__)(struct OfaContext * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual inherited (gpures) base (chandes)
    NV_STATUS (*__ofactxUnmap__)(struct OfaContext * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual inherited (gpures) base (chandes)
    NvBool (*__ofactxShareCallback__)(struct OfaContext * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (chandes)
    NV_STATUS (*__ofactxGetRegBaseOffsetAndSize__)(struct OfaContext * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual inherited (gpures) base (chandes)
    NV_STATUS (*__ofactxGetMapAddrSpace__)(struct OfaContext * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (gpures) base (chandes)
    NV_STATUS (*__ofactxInternalControlForward__)(struct OfaContext * /*this*/, NvU32, void *, NvU32);  // virtual inherited (gpures) base (chandes)
    NvHandle (*__ofactxGetInternalObjectHandle__)(struct OfaContext * /*this*/);  // virtual inherited (gpures) base (chandes)
    NvBool (*__ofactxAccessCallback__)(struct OfaContext * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (chandes)
    NV_STATUS (*__ofactxGetMemInterMapParams__)(struct OfaContext * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (chandes)
    NV_STATUS (*__ofactxGetMemoryMappingDescriptor__)(struct OfaContext * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (chandes)
    NV_STATUS (*__ofactxControlSerialization_Prologue__)(struct OfaContext * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (chandes)
    void (*__ofactxControlSerialization_Epilogue__)(struct OfaContext * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (chandes)
    NV_STATUS (*__ofactxControl_Prologue__)(struct OfaContext * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (chandes)
    void (*__ofactxControl_Epilogue__)(struct OfaContext * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (chandes)
    NvBool (*__ofactxCanCopy__)(struct OfaContext * /*this*/);  // virtual inherited (res) base (chandes)
    NV_STATUS (*__ofactxIsDuplicate__)(struct OfaContext * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (chandes)
    void (*__ofactxPreDestruct__)(struct OfaContext * /*this*/);  // virtual inherited (res) base (chandes)
    NV_STATUS (*__ofactxControlFilter__)(struct OfaContext * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (chandes)
    NvBool (*__ofactxIsPartialUnmapSupported__)(struct OfaContext * /*this*/);  // inline virtual inherited (res) base (chandes) body
    NV_STATUS (*__ofactxMapTo__)(struct OfaContext * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (chandes)
    NV_STATUS (*__ofactxUnmapFrom__)(struct OfaContext * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (chandes)
    NvU32 (*__ofactxGetRefCount__)(struct OfaContext * /*this*/);  // virtual inherited (res) base (chandes)
    void (*__ofactxAddAdditionalDependants__)(struct RsClient *, struct OfaContext * /*this*/, RsResourceRef *);  // virtual inherited (res) base (chandes)
    PEVENTNOTIFICATION * (*__ofactxGetNotificationListPtr__)(struct OfaContext * /*this*/);  // virtual inherited (notify) base (chandes)
    struct NotifShare * (*__ofactxGetNotificationShare__)(struct OfaContext * /*this*/);  // virtual inherited (notify) base (chandes)
    void (*__ofactxSetNotificationShare__)(struct OfaContext * /*this*/, struct NotifShare *);  // virtual inherited (notify) base (chandes)
    NV_STATUS (*__ofactxUnregisterEvent__)(struct OfaContext * /*this*/, NvHandle, NvHandle, NvHandle, NvHandle);  // virtual inherited (notify) base (chandes)
    NV_STATUS (*__ofactxGetOrAllocNotifShare__)(struct OfaContext * /*this*/, NvHandle, NvHandle, struct NotifShare **);  // virtual inherited (notify) base (chandes)
};

#ifndef __NVOC_CLASS_OfaContext_TYPEDEF__
#define __NVOC_CLASS_OfaContext_TYPEDEF__
typedef struct OfaContext OfaContext;
#endif /* __NVOC_CLASS_OfaContext_TYPEDEF__ */

#ifndef __nvoc_class_id_OfaContext
#define __nvoc_class_id_OfaContext 0xf63d99
#endif /* __nvoc_class_id_OfaContext */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OfaContext;

#define __staticCast_OfaContext(pThis) \
    ((pThis)->__nvoc_pbase_OfaContext)

#ifdef __nvoc_kernel_ofa_ctx_h_disabled
#define __dynamicCast_OfaContext(pThis) ((OfaContext*)NULL)
#else //__nvoc_kernel_ofa_ctx_h_disabled
#define __dynamicCast_OfaContext(pThis) \
    ((OfaContext*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(OfaContext)))
#endif //__nvoc_kernel_ofa_ctx_h_disabled

NV_STATUS __nvoc_objCreateDynamic_OfaContext(OfaContext**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_OfaContext(OfaContext**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_OfaContext(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_OfaContext((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define ofactxGetSwMethods_FNPTR(pChannelDescendant) pChannelDescendant->__nvoc_base_ChannelDescendant.__chandesGetSwMethods__
#define ofactxGetSwMethods(pChannelDescendant, ppMethods, pNumMethods) ofactxGetSwMethods_DISPATCH(pChannelDescendant, ppMethods, pNumMethods)
#define ofactxIsSwMethodStalling_FNPTR(pChannelDescendant) pChannelDescendant->__nvoc_base_ChannelDescendant.__chandesIsSwMethodStalling__
#define ofactxIsSwMethodStalling(pChannelDescendant, hHandle) ofactxIsSwMethodStalling_DISPATCH(pChannelDescendant, hHandle)
#define ofactxCheckMemInterUnmap_FNPTR(pChannelDescendant) pChannelDescendant->__nvoc_base_ChannelDescendant.__chandesCheckMemInterUnmap__
#define ofactxCheckMemInterUnmap(pChannelDescendant, bSubdeviceHandleProvided) ofactxCheckMemInterUnmap_DISPATCH(pChannelDescendant, bSubdeviceHandleProvided)
#define ofactxControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__gpuresControl__
#define ofactxControl(pGpuResource, pCallContext, pParams) ofactxControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define ofactxMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__gpuresMap__
#define ofactxMap(pGpuResource, pCallContext, pParams, pCpuMapping) ofactxMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define ofactxUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__gpuresUnmap__
#define ofactxUnmap(pGpuResource, pCallContext, pCpuMapping) ofactxUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define ofactxShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__gpuresShareCallback__
#define ofactxShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) ofactxShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define ofactxGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__gpuresGetRegBaseOffsetAndSize__
#define ofactxGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) ofactxGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define ofactxGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__gpuresGetMapAddrSpace__
#define ofactxGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) ofactxGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define ofactxInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__gpuresInternalControlForward__
#define ofactxInternalControlForward(pGpuResource, command, pParams, size) ofactxInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define ofactxGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__gpuresGetInternalObjectHandle__
#define ofactxGetInternalObjectHandle(pGpuResource) ofactxGetInternalObjectHandle_DISPATCH(pGpuResource)
#define ofactxAccessCallback_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresAccessCallback__
#define ofactxAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) ofactxAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define ofactxGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemInterMapParams__
#define ofactxGetMemInterMapParams(pRmResource, pParams) ofactxGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define ofactxGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemoryMappingDescriptor__
#define ofactxGetMemoryMappingDescriptor(pRmResource, ppMemDesc) ofactxGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define ofactxControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Prologue__
#define ofactxControlSerialization_Prologue(pResource, pCallContext, pParams) ofactxControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define ofactxControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Epilogue__
#define ofactxControlSerialization_Epilogue(pResource, pCallContext, pParams) ofactxControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define ofactxControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Prologue__
#define ofactxControl_Prologue(pResource, pCallContext, pParams) ofactxControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define ofactxControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Epilogue__
#define ofactxControl_Epilogue(pResource, pCallContext, pParams) ofactxControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define ofactxCanCopy_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__
#define ofactxCanCopy(pResource) ofactxCanCopy_DISPATCH(pResource)
#define ofactxIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsDuplicate__
#define ofactxIsDuplicate(pResource, hMemory, pDuplicate) ofactxIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define ofactxPreDestruct_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resPreDestruct__
#define ofactxPreDestruct(pResource) ofactxPreDestruct_DISPATCH(pResource)
#define ofactxControlFilter_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resControlFilter__
#define ofactxControlFilter(pResource, pCallContext, pParams) ofactxControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define ofactxIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsPartialUnmapSupported__
#define ofactxIsPartialUnmapSupported(pResource) ofactxIsPartialUnmapSupported_DISPATCH(pResource)
#define ofactxMapTo_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resMapTo__
#define ofactxMapTo(pResource, pParams) ofactxMapTo_DISPATCH(pResource, pParams)
#define ofactxUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmapFrom__
#define ofactxUnmapFrom(pResource, pParams) ofactxUnmapFrom_DISPATCH(pResource, pParams)
#define ofactxGetRefCount_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resGetRefCount__
#define ofactxGetRefCount(pResource) ofactxGetRefCount_DISPATCH(pResource)
#define ofactxAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resAddAdditionalDependants__
#define ofactxAddAdditionalDependants(pClient, pResource, pReference) ofactxAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define ofactxGetNotificationListPtr_FNPTR(pNotifier) pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__notifyGetNotificationListPtr__
#define ofactxGetNotificationListPtr(pNotifier) ofactxGetNotificationListPtr_DISPATCH(pNotifier)
#define ofactxGetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__notifyGetNotificationShare__
#define ofactxGetNotificationShare(pNotifier) ofactxGetNotificationShare_DISPATCH(pNotifier)
#define ofactxSetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__notifySetNotificationShare__
#define ofactxSetNotificationShare(pNotifier, pNotifShare) ofactxSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define ofactxUnregisterEvent_FNPTR(pNotifier) pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__notifyUnregisterEvent__
#define ofactxUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) ofactxUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define ofactxGetOrAllocNotifShare_FNPTR(pNotifier) pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__notifyGetOrAllocNotifShare__
#define ofactxGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) ofactxGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)

// Dispatch functions
static inline NV_STATUS ofactxGetSwMethods_DISPATCH(struct OfaContext *pChannelDescendant, const METHOD **ppMethods, NvU32 *pNumMethods) {
    return pChannelDescendant->__ofactxGetSwMethods__(pChannelDescendant, ppMethods, pNumMethods);
}

static inline NvBool ofactxIsSwMethodStalling_DISPATCH(struct OfaContext *pChannelDescendant, NvU32 hHandle) {
    return pChannelDescendant->__ofactxIsSwMethodStalling__(pChannelDescendant, hHandle);
}

static inline NV_STATUS ofactxCheckMemInterUnmap_DISPATCH(struct OfaContext *pChannelDescendant, NvBool bSubdeviceHandleProvided) {
    return pChannelDescendant->__ofactxCheckMemInterUnmap__(pChannelDescendant, bSubdeviceHandleProvided);
}

static inline NV_STATUS ofactxControl_DISPATCH(struct OfaContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__ofactxControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS ofactxMap_DISPATCH(struct OfaContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__ofactxMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS ofactxUnmap_DISPATCH(struct OfaContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__ofactxUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NvBool ofactxShareCallback_DISPATCH(struct OfaContext *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__ofactxShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS ofactxGetRegBaseOffsetAndSize_DISPATCH(struct OfaContext *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__ofactxGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS ofactxGetMapAddrSpace_DISPATCH(struct OfaContext *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__ofactxGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS ofactxInternalControlForward_DISPATCH(struct OfaContext *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__ofactxInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NvHandle ofactxGetInternalObjectHandle_DISPATCH(struct OfaContext *pGpuResource) {
    return pGpuResource->__ofactxGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool ofactxAccessCallback_DISPATCH(struct OfaContext *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__ofactxAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS ofactxGetMemInterMapParams_DISPATCH(struct OfaContext *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__ofactxGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS ofactxGetMemoryMappingDescriptor_DISPATCH(struct OfaContext *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__ofactxGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS ofactxControlSerialization_Prologue_DISPATCH(struct OfaContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__ofactxControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void ofactxControlSerialization_Epilogue_DISPATCH(struct OfaContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__ofactxControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS ofactxControl_Prologue_DISPATCH(struct OfaContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__ofactxControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void ofactxControl_Epilogue_DISPATCH(struct OfaContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__ofactxControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool ofactxCanCopy_DISPATCH(struct OfaContext *pResource) {
    return pResource->__ofactxCanCopy__(pResource);
}

static inline NV_STATUS ofactxIsDuplicate_DISPATCH(struct OfaContext *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__ofactxIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void ofactxPreDestruct_DISPATCH(struct OfaContext *pResource) {
    pResource->__ofactxPreDestruct__(pResource);
}

static inline NV_STATUS ofactxControlFilter_DISPATCH(struct OfaContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__ofactxControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool ofactxIsPartialUnmapSupported_DISPATCH(struct OfaContext *pResource) {
    return pResource->__ofactxIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS ofactxMapTo_DISPATCH(struct OfaContext *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__ofactxMapTo__(pResource, pParams);
}

static inline NV_STATUS ofactxUnmapFrom_DISPATCH(struct OfaContext *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__ofactxUnmapFrom__(pResource, pParams);
}

static inline NvU32 ofactxGetRefCount_DISPATCH(struct OfaContext *pResource) {
    return pResource->__ofactxGetRefCount__(pResource);
}

static inline void ofactxAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct OfaContext *pResource, RsResourceRef *pReference) {
    pResource->__ofactxAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline PEVENTNOTIFICATION * ofactxGetNotificationListPtr_DISPATCH(struct OfaContext *pNotifier) {
    return pNotifier->__ofactxGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare * ofactxGetNotificationShare_DISPATCH(struct OfaContext *pNotifier) {
    return pNotifier->__ofactxGetNotificationShare__(pNotifier);
}

static inline void ofactxSetNotificationShare_DISPATCH(struct OfaContext *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__ofactxSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NV_STATUS ofactxUnregisterEvent_DISPATCH(struct OfaContext *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__ofactxUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS ofactxGetOrAllocNotifShare_DISPATCH(struct OfaContext *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__ofactxGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

NV_STATUS ofactxConstructHal_KERNEL(struct OfaContext *pOfaContext, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);


#ifdef __nvoc_kernel_ofa_ctx_h_disabled
static inline NV_STATUS ofactxConstructHal(struct OfaContext *pOfaContext, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {
    NV_ASSERT_FAILED_PRECOMP("OfaContext was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_ofa_ctx_h_disabled
#define ofactxConstructHal(pOfaContext, pCallContext, pParams) ofactxConstructHal_KERNEL(pOfaContext, pCallContext, pParams)
#endif //__nvoc_kernel_ofa_ctx_h_disabled

#define ofactxConstructHal_HAL(pOfaContext, pCallContext, pParams) ofactxConstructHal(pOfaContext, pCallContext, pParams)

void ofactxDestructHal_KERNEL(struct OfaContext *pOfaContext);


#ifdef __nvoc_kernel_ofa_ctx_h_disabled
static inline void ofactxDestructHal(struct OfaContext *pOfaContext) {
    NV_ASSERT_FAILED_PRECOMP("OfaContext was disabled!");
}
#else //__nvoc_kernel_ofa_ctx_h_disabled
#define ofactxDestructHal(pOfaContext) ofactxDestructHal_KERNEL(pOfaContext)
#endif //__nvoc_kernel_ofa_ctx_h_disabled

#define ofactxDestructHal_HAL(pOfaContext) ofactxDestructHal(pOfaContext)

static inline NV_STATUS __nvoc_ofactxConstruct(struct OfaContext *arg_pOfaContext, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams) {
    return ofactxConstructHal(arg_pOfaContext, arg_pCallContext, arg_pParams);
}

static inline void __nvoc_ofactxDestruct(struct OfaContext *pOfaContext) {
    ofactxDestructHal(pOfaContext);
}

#undef PRIVATE_FIELD


#endif // KERNEL_OFA_CTX_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_OFA_CTX_NVOC_H_
