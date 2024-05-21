
#ifndef _G_KERNEL_NVENC_CTX_NVOC_H_
#define _G_KERNEL_NVENC_CTX_NVOC_H_
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
 * SPDX-FileCopyrightText: Copyright (c) 2008-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_kernel_nvenc_ctx_nvoc.h"

#ifndef KERNEL_NVENC_CTX_H
#define KERNEL_NVENC_CTX_H

#include "kernel/gpu/fifo/channel_descendant.h"

ENGDESCRIPTOR msencGetEngineDescFromAllocParams(OBJGPU *pGpu, NvU32 externalClassId, void *pAllocParams);

/*!
 * RM internal class representing NVXXXX_VIDEO_ENCODER
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_KERNEL_NVENC_CTX_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct MsencContext {

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
    struct MsencContext *__nvoc_pbase_MsencContext;    // msencctx

    // Vtable with 32 per-object function pointers
    NV_STATUS (*__msencctxGetSwMethods__)(struct MsencContext * /*this*/, const METHOD **, NvU32 *);  // virtual inherited (chandes) base (chandes)
    NvBool (*__msencctxIsSwMethodStalling__)(struct MsencContext * /*this*/, NvU32);  // virtual inherited (chandes) base (chandes)
    NV_STATUS (*__msencctxCheckMemInterUnmap__)(struct MsencContext * /*this*/, NvBool);  // virtual inherited (chandes) base (chandes)
    NV_STATUS (*__msencctxControl__)(struct MsencContext * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (gpures) base (chandes)
    NV_STATUS (*__msencctxMap__)(struct MsencContext * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual inherited (gpures) base (chandes)
    NV_STATUS (*__msencctxUnmap__)(struct MsencContext * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual inherited (gpures) base (chandes)
    NvBool (*__msencctxShareCallback__)(struct MsencContext * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (chandes)
    NV_STATUS (*__msencctxGetRegBaseOffsetAndSize__)(struct MsencContext * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual inherited (gpures) base (chandes)
    NV_STATUS (*__msencctxGetMapAddrSpace__)(struct MsencContext * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (gpures) base (chandes)
    NV_STATUS (*__msencctxInternalControlForward__)(struct MsencContext * /*this*/, NvU32, void *, NvU32);  // virtual inherited (gpures) base (chandes)
    NvHandle (*__msencctxGetInternalObjectHandle__)(struct MsencContext * /*this*/);  // virtual inherited (gpures) base (chandes)
    NvBool (*__msencctxAccessCallback__)(struct MsencContext * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (chandes)
    NV_STATUS (*__msencctxGetMemInterMapParams__)(struct MsencContext * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (chandes)
    NV_STATUS (*__msencctxGetMemoryMappingDescriptor__)(struct MsencContext * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (chandes)
    NV_STATUS (*__msencctxControlSerialization_Prologue__)(struct MsencContext * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (chandes)
    void (*__msencctxControlSerialization_Epilogue__)(struct MsencContext * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (chandes)
    NV_STATUS (*__msencctxControl_Prologue__)(struct MsencContext * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (chandes)
    void (*__msencctxControl_Epilogue__)(struct MsencContext * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (chandes)
    NvBool (*__msencctxCanCopy__)(struct MsencContext * /*this*/);  // virtual inherited (res) base (chandes)
    NV_STATUS (*__msencctxIsDuplicate__)(struct MsencContext * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (chandes)
    void (*__msencctxPreDestruct__)(struct MsencContext * /*this*/);  // virtual inherited (res) base (chandes)
    NV_STATUS (*__msencctxControlFilter__)(struct MsencContext * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (chandes)
    NvBool (*__msencctxIsPartialUnmapSupported__)(struct MsencContext * /*this*/);  // inline virtual inherited (res) base (chandes) body
    NV_STATUS (*__msencctxMapTo__)(struct MsencContext * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (chandes)
    NV_STATUS (*__msencctxUnmapFrom__)(struct MsencContext * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (chandes)
    NvU32 (*__msencctxGetRefCount__)(struct MsencContext * /*this*/);  // virtual inherited (res) base (chandes)
    void (*__msencctxAddAdditionalDependants__)(struct RsClient *, struct MsencContext * /*this*/, RsResourceRef *);  // virtual inherited (res) base (chandes)
    PEVENTNOTIFICATION * (*__msencctxGetNotificationListPtr__)(struct MsencContext * /*this*/);  // virtual inherited (notify) base (chandes)
    struct NotifShare * (*__msencctxGetNotificationShare__)(struct MsencContext * /*this*/);  // virtual inherited (notify) base (chandes)
    void (*__msencctxSetNotificationShare__)(struct MsencContext * /*this*/, struct NotifShare *);  // virtual inherited (notify) base (chandes)
    NV_STATUS (*__msencctxUnregisterEvent__)(struct MsencContext * /*this*/, NvHandle, NvHandle, NvHandle, NvHandle);  // virtual inherited (notify) base (chandes)
    NV_STATUS (*__msencctxGetOrAllocNotifShare__)(struct MsencContext * /*this*/, NvHandle, NvHandle, struct NotifShare **);  // virtual inherited (notify) base (chandes)
};

#ifndef __NVOC_CLASS_MsencContext_TYPEDEF__
#define __NVOC_CLASS_MsencContext_TYPEDEF__
typedef struct MsencContext MsencContext;
#endif /* __NVOC_CLASS_MsencContext_TYPEDEF__ */

#ifndef __nvoc_class_id_MsencContext
#define __nvoc_class_id_MsencContext 0x88c92a
#endif /* __nvoc_class_id_MsencContext */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_MsencContext;

#define __staticCast_MsencContext(pThis) \
    ((pThis)->__nvoc_pbase_MsencContext)

#ifdef __nvoc_kernel_nvenc_ctx_h_disabled
#define __dynamicCast_MsencContext(pThis) ((MsencContext*)NULL)
#else //__nvoc_kernel_nvenc_ctx_h_disabled
#define __dynamicCast_MsencContext(pThis) \
    ((MsencContext*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(MsencContext)))
#endif //__nvoc_kernel_nvenc_ctx_h_disabled

NV_STATUS __nvoc_objCreateDynamic_MsencContext(MsencContext**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_MsencContext(MsencContext**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_MsencContext(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_MsencContext((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define msencctxGetSwMethods_FNPTR(pChannelDescendant) pChannelDescendant->__nvoc_base_ChannelDescendant.__chandesGetSwMethods__
#define msencctxGetSwMethods(pChannelDescendant, ppMethods, pNumMethods) msencctxGetSwMethods_DISPATCH(pChannelDescendant, ppMethods, pNumMethods)
#define msencctxIsSwMethodStalling_FNPTR(pChannelDescendant) pChannelDescendant->__nvoc_base_ChannelDescendant.__chandesIsSwMethodStalling__
#define msencctxIsSwMethodStalling(pChannelDescendant, hHandle) msencctxIsSwMethodStalling_DISPATCH(pChannelDescendant, hHandle)
#define msencctxCheckMemInterUnmap_FNPTR(pChannelDescendant) pChannelDescendant->__nvoc_base_ChannelDescendant.__chandesCheckMemInterUnmap__
#define msencctxCheckMemInterUnmap(pChannelDescendant, bSubdeviceHandleProvided) msencctxCheckMemInterUnmap_DISPATCH(pChannelDescendant, bSubdeviceHandleProvided)
#define msencctxControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__gpuresControl__
#define msencctxControl(pGpuResource, pCallContext, pParams) msencctxControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define msencctxMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__gpuresMap__
#define msencctxMap(pGpuResource, pCallContext, pParams, pCpuMapping) msencctxMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define msencctxUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__gpuresUnmap__
#define msencctxUnmap(pGpuResource, pCallContext, pCpuMapping) msencctxUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define msencctxShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__gpuresShareCallback__
#define msencctxShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) msencctxShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define msencctxGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__gpuresGetRegBaseOffsetAndSize__
#define msencctxGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) msencctxGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define msencctxGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__gpuresGetMapAddrSpace__
#define msencctxGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) msencctxGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define msencctxInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__gpuresInternalControlForward__
#define msencctxInternalControlForward(pGpuResource, command, pParams, size) msencctxInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define msencctxGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__gpuresGetInternalObjectHandle__
#define msencctxGetInternalObjectHandle(pGpuResource) msencctxGetInternalObjectHandle_DISPATCH(pGpuResource)
#define msencctxAccessCallback_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresAccessCallback__
#define msencctxAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) msencctxAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define msencctxGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemInterMapParams__
#define msencctxGetMemInterMapParams(pRmResource, pParams) msencctxGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define msencctxGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemoryMappingDescriptor__
#define msencctxGetMemoryMappingDescriptor(pRmResource, ppMemDesc) msencctxGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define msencctxControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Prologue__
#define msencctxControlSerialization_Prologue(pResource, pCallContext, pParams) msencctxControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define msencctxControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Epilogue__
#define msencctxControlSerialization_Epilogue(pResource, pCallContext, pParams) msencctxControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define msencctxControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Prologue__
#define msencctxControl_Prologue(pResource, pCallContext, pParams) msencctxControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define msencctxControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Epilogue__
#define msencctxControl_Epilogue(pResource, pCallContext, pParams) msencctxControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define msencctxCanCopy_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__
#define msencctxCanCopy(pResource) msencctxCanCopy_DISPATCH(pResource)
#define msencctxIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsDuplicate__
#define msencctxIsDuplicate(pResource, hMemory, pDuplicate) msencctxIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define msencctxPreDestruct_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resPreDestruct__
#define msencctxPreDestruct(pResource) msencctxPreDestruct_DISPATCH(pResource)
#define msencctxControlFilter_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resControlFilter__
#define msencctxControlFilter(pResource, pCallContext, pParams) msencctxControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define msencctxIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsPartialUnmapSupported__
#define msencctxIsPartialUnmapSupported(pResource) msencctxIsPartialUnmapSupported_DISPATCH(pResource)
#define msencctxMapTo_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resMapTo__
#define msencctxMapTo(pResource, pParams) msencctxMapTo_DISPATCH(pResource, pParams)
#define msencctxUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmapFrom__
#define msencctxUnmapFrom(pResource, pParams) msencctxUnmapFrom_DISPATCH(pResource, pParams)
#define msencctxGetRefCount_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resGetRefCount__
#define msencctxGetRefCount(pResource) msencctxGetRefCount_DISPATCH(pResource)
#define msencctxAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resAddAdditionalDependants__
#define msencctxAddAdditionalDependants(pClient, pResource, pReference) msencctxAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define msencctxGetNotificationListPtr_FNPTR(pNotifier) pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__notifyGetNotificationListPtr__
#define msencctxGetNotificationListPtr(pNotifier) msencctxGetNotificationListPtr_DISPATCH(pNotifier)
#define msencctxGetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__notifyGetNotificationShare__
#define msencctxGetNotificationShare(pNotifier) msencctxGetNotificationShare_DISPATCH(pNotifier)
#define msencctxSetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__notifySetNotificationShare__
#define msencctxSetNotificationShare(pNotifier, pNotifShare) msencctxSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define msencctxUnregisterEvent_FNPTR(pNotifier) pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__notifyUnregisterEvent__
#define msencctxUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) msencctxUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define msencctxGetOrAllocNotifShare_FNPTR(pNotifier) pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__notifyGetOrAllocNotifShare__
#define msencctxGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) msencctxGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)

// Dispatch functions
static inline NV_STATUS msencctxGetSwMethods_DISPATCH(struct MsencContext *pChannelDescendant, const METHOD **ppMethods, NvU32 *pNumMethods) {
    return pChannelDescendant->__msencctxGetSwMethods__(pChannelDescendant, ppMethods, pNumMethods);
}

static inline NvBool msencctxIsSwMethodStalling_DISPATCH(struct MsencContext *pChannelDescendant, NvU32 hHandle) {
    return pChannelDescendant->__msencctxIsSwMethodStalling__(pChannelDescendant, hHandle);
}

static inline NV_STATUS msencctxCheckMemInterUnmap_DISPATCH(struct MsencContext *pChannelDescendant, NvBool bSubdeviceHandleProvided) {
    return pChannelDescendant->__msencctxCheckMemInterUnmap__(pChannelDescendant, bSubdeviceHandleProvided);
}

static inline NV_STATUS msencctxControl_DISPATCH(struct MsencContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__msencctxControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS msencctxMap_DISPATCH(struct MsencContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__msencctxMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS msencctxUnmap_DISPATCH(struct MsencContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__msencctxUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NvBool msencctxShareCallback_DISPATCH(struct MsencContext *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__msencctxShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS msencctxGetRegBaseOffsetAndSize_DISPATCH(struct MsencContext *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__msencctxGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS msencctxGetMapAddrSpace_DISPATCH(struct MsencContext *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__msencctxGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS msencctxInternalControlForward_DISPATCH(struct MsencContext *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__msencctxInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NvHandle msencctxGetInternalObjectHandle_DISPATCH(struct MsencContext *pGpuResource) {
    return pGpuResource->__msencctxGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool msencctxAccessCallback_DISPATCH(struct MsencContext *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__msencctxAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS msencctxGetMemInterMapParams_DISPATCH(struct MsencContext *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__msencctxGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS msencctxGetMemoryMappingDescriptor_DISPATCH(struct MsencContext *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__msencctxGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS msencctxControlSerialization_Prologue_DISPATCH(struct MsencContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__msencctxControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void msencctxControlSerialization_Epilogue_DISPATCH(struct MsencContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__msencctxControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS msencctxControl_Prologue_DISPATCH(struct MsencContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__msencctxControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void msencctxControl_Epilogue_DISPATCH(struct MsencContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__msencctxControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool msencctxCanCopy_DISPATCH(struct MsencContext *pResource) {
    return pResource->__msencctxCanCopy__(pResource);
}

static inline NV_STATUS msencctxIsDuplicate_DISPATCH(struct MsencContext *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__msencctxIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void msencctxPreDestruct_DISPATCH(struct MsencContext *pResource) {
    pResource->__msencctxPreDestruct__(pResource);
}

static inline NV_STATUS msencctxControlFilter_DISPATCH(struct MsencContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__msencctxControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool msencctxIsPartialUnmapSupported_DISPATCH(struct MsencContext *pResource) {
    return pResource->__msencctxIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS msencctxMapTo_DISPATCH(struct MsencContext *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__msencctxMapTo__(pResource, pParams);
}

static inline NV_STATUS msencctxUnmapFrom_DISPATCH(struct MsencContext *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__msencctxUnmapFrom__(pResource, pParams);
}

static inline NvU32 msencctxGetRefCount_DISPATCH(struct MsencContext *pResource) {
    return pResource->__msencctxGetRefCount__(pResource);
}

static inline void msencctxAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct MsencContext *pResource, RsResourceRef *pReference) {
    pResource->__msencctxAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline PEVENTNOTIFICATION * msencctxGetNotificationListPtr_DISPATCH(struct MsencContext *pNotifier) {
    return pNotifier->__msencctxGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare * msencctxGetNotificationShare_DISPATCH(struct MsencContext *pNotifier) {
    return pNotifier->__msencctxGetNotificationShare__(pNotifier);
}

static inline void msencctxSetNotificationShare_DISPATCH(struct MsencContext *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__msencctxSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NV_STATUS msencctxUnregisterEvent_DISPATCH(struct MsencContext *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__msencctxUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS msencctxGetOrAllocNotifShare_DISPATCH(struct MsencContext *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__msencctxGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

NV_STATUS msencctxConstructHal_KERNEL(struct MsencContext *pMsencContext, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);


#ifdef __nvoc_kernel_nvenc_ctx_h_disabled
static inline NV_STATUS msencctxConstructHal(struct MsencContext *pMsencContext, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {
    NV_ASSERT_FAILED_PRECOMP("MsencContext was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvenc_ctx_h_disabled
#define msencctxConstructHal(pMsencContext, pCallContext, pParams) msencctxConstructHal_KERNEL(pMsencContext, pCallContext, pParams)
#endif //__nvoc_kernel_nvenc_ctx_h_disabled

#define msencctxConstructHal_HAL(pMsencContext, pCallContext, pParams) msencctxConstructHal(pMsencContext, pCallContext, pParams)

void msencctxDestructHal_KERNEL(struct MsencContext *pMsencContext);


#ifdef __nvoc_kernel_nvenc_ctx_h_disabled
static inline void msencctxDestructHal(struct MsencContext *pMsencContext) {
    NV_ASSERT_FAILED_PRECOMP("MsencContext was disabled!");
}
#else //__nvoc_kernel_nvenc_ctx_h_disabled
#define msencctxDestructHal(pMsencContext) msencctxDestructHal_KERNEL(pMsencContext)
#endif //__nvoc_kernel_nvenc_ctx_h_disabled

#define msencctxDestructHal_HAL(pMsencContext) msencctxDestructHal(pMsencContext)

static inline NV_STATUS __nvoc_msencctxConstruct(struct MsencContext *arg_pMsencContext, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams) {
    return msencctxConstructHal(arg_pMsencContext, arg_pCallContext, arg_pParams);
}

static inline void __nvoc_msencctxDestruct(struct MsencContext *pMsencContext) {
    msencctxDestructHal(pMsencContext);
}

#undef PRIVATE_FIELD


#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_NVENC_CTX_NVOC_H_
