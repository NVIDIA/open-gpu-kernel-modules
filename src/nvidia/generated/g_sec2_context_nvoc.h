
#ifndef _G_SEC2_CONTEXT_NVOC_H_
#define _G_SEC2_CONTEXT_NVOC_H_
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
 * SPDX-FileCopyrightText: Copyright (c) 2021-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_sec2_context_nvoc.h"

#ifndef SEC2_CONTEXT_H
#define SEC2_CONTEXT_H

#include "core/bin_data.h"
#include "core/core.h"
#include "kernel/gpu/fifo/channel_descendant.h"

/*!
 * RM internal class representing NVXXXX_TSEC. Class is used for channel work
 * submission.
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_SEC2_CONTEXT_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct Sec2Context {

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
    struct Sec2Context *__nvoc_pbase_Sec2Context;    // sec2ctx

    // Vtable with 32 per-object function pointers
    NV_STATUS (*__sec2ctxGetSwMethods__)(struct Sec2Context * /*this*/, const METHOD **, NvU32 *);  // virtual inherited (chandes) base (chandes)
    NvBool (*__sec2ctxIsSwMethodStalling__)(struct Sec2Context * /*this*/, NvU32);  // virtual inherited (chandes) base (chandes)
    NV_STATUS (*__sec2ctxCheckMemInterUnmap__)(struct Sec2Context * /*this*/, NvBool);  // virtual inherited (chandes) base (chandes)
    NV_STATUS (*__sec2ctxControl__)(struct Sec2Context * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (gpures) base (chandes)
    NV_STATUS (*__sec2ctxMap__)(struct Sec2Context * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual inherited (gpures) base (chandes)
    NV_STATUS (*__sec2ctxUnmap__)(struct Sec2Context * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual inherited (gpures) base (chandes)
    NvBool (*__sec2ctxShareCallback__)(struct Sec2Context * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (chandes)
    NV_STATUS (*__sec2ctxGetRegBaseOffsetAndSize__)(struct Sec2Context * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual inherited (gpures) base (chandes)
    NV_STATUS (*__sec2ctxGetMapAddrSpace__)(struct Sec2Context * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (gpures) base (chandes)
    NV_STATUS (*__sec2ctxInternalControlForward__)(struct Sec2Context * /*this*/, NvU32, void *, NvU32);  // virtual inherited (gpures) base (chandes)
    NvHandle (*__sec2ctxGetInternalObjectHandle__)(struct Sec2Context * /*this*/);  // virtual inherited (gpures) base (chandes)
    NvBool (*__sec2ctxAccessCallback__)(struct Sec2Context * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (chandes)
    NV_STATUS (*__sec2ctxGetMemInterMapParams__)(struct Sec2Context * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (chandes)
    NV_STATUS (*__sec2ctxGetMemoryMappingDescriptor__)(struct Sec2Context * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (chandes)
    NV_STATUS (*__sec2ctxControlSerialization_Prologue__)(struct Sec2Context * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (chandes)
    void (*__sec2ctxControlSerialization_Epilogue__)(struct Sec2Context * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (chandes)
    NV_STATUS (*__sec2ctxControl_Prologue__)(struct Sec2Context * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (chandes)
    void (*__sec2ctxControl_Epilogue__)(struct Sec2Context * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (chandes)
    NvBool (*__sec2ctxCanCopy__)(struct Sec2Context * /*this*/);  // virtual inherited (res) base (chandes)
    NV_STATUS (*__sec2ctxIsDuplicate__)(struct Sec2Context * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (chandes)
    void (*__sec2ctxPreDestruct__)(struct Sec2Context * /*this*/);  // virtual inherited (res) base (chandes)
    NV_STATUS (*__sec2ctxControlFilter__)(struct Sec2Context * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (chandes)
    NvBool (*__sec2ctxIsPartialUnmapSupported__)(struct Sec2Context * /*this*/);  // inline virtual inherited (res) base (chandes) body
    NV_STATUS (*__sec2ctxMapTo__)(struct Sec2Context * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (chandes)
    NV_STATUS (*__sec2ctxUnmapFrom__)(struct Sec2Context * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (chandes)
    NvU32 (*__sec2ctxGetRefCount__)(struct Sec2Context * /*this*/);  // virtual inherited (res) base (chandes)
    void (*__sec2ctxAddAdditionalDependants__)(struct RsClient *, struct Sec2Context * /*this*/, RsResourceRef *);  // virtual inherited (res) base (chandes)
    PEVENTNOTIFICATION * (*__sec2ctxGetNotificationListPtr__)(struct Sec2Context * /*this*/);  // virtual inherited (notify) base (chandes)
    struct NotifShare * (*__sec2ctxGetNotificationShare__)(struct Sec2Context * /*this*/);  // virtual inherited (notify) base (chandes)
    void (*__sec2ctxSetNotificationShare__)(struct Sec2Context * /*this*/, struct NotifShare *);  // virtual inherited (notify) base (chandes)
    NV_STATUS (*__sec2ctxUnregisterEvent__)(struct Sec2Context * /*this*/, NvHandle, NvHandle, NvHandle, NvHandle);  // virtual inherited (notify) base (chandes)
    NV_STATUS (*__sec2ctxGetOrAllocNotifShare__)(struct Sec2Context * /*this*/, NvHandle, NvHandle, struct NotifShare **);  // virtual inherited (notify) base (chandes)
};

#ifndef __NVOC_CLASS_Sec2Context_TYPEDEF__
#define __NVOC_CLASS_Sec2Context_TYPEDEF__
typedef struct Sec2Context Sec2Context;
#endif /* __NVOC_CLASS_Sec2Context_TYPEDEF__ */

#ifndef __nvoc_class_id_Sec2Context
#define __nvoc_class_id_Sec2Context 0x4c3439
#endif /* __nvoc_class_id_Sec2Context */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Sec2Context;

#define __staticCast_Sec2Context(pThis) \
    ((pThis)->__nvoc_pbase_Sec2Context)

#ifdef __nvoc_sec2_context_h_disabled
#define __dynamicCast_Sec2Context(pThis) ((Sec2Context*)NULL)
#else //__nvoc_sec2_context_h_disabled
#define __dynamicCast_Sec2Context(pThis) \
    ((Sec2Context*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(Sec2Context)))
#endif //__nvoc_sec2_context_h_disabled

NV_STATUS __nvoc_objCreateDynamic_Sec2Context(Sec2Context**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_Sec2Context(Sec2Context**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_Sec2Context(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_Sec2Context((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define sec2ctxGetSwMethods_FNPTR(pChannelDescendant) pChannelDescendant->__nvoc_base_ChannelDescendant.__chandesGetSwMethods__
#define sec2ctxGetSwMethods(pChannelDescendant, ppMethods, pNumMethods) sec2ctxGetSwMethods_DISPATCH(pChannelDescendant, ppMethods, pNumMethods)
#define sec2ctxIsSwMethodStalling_FNPTR(pChannelDescendant) pChannelDescendant->__nvoc_base_ChannelDescendant.__chandesIsSwMethodStalling__
#define sec2ctxIsSwMethodStalling(pChannelDescendant, hHandle) sec2ctxIsSwMethodStalling_DISPATCH(pChannelDescendant, hHandle)
#define sec2ctxCheckMemInterUnmap_FNPTR(pChannelDescendant) pChannelDescendant->__nvoc_base_ChannelDescendant.__chandesCheckMemInterUnmap__
#define sec2ctxCheckMemInterUnmap(pChannelDescendant, bSubdeviceHandleProvided) sec2ctxCheckMemInterUnmap_DISPATCH(pChannelDescendant, bSubdeviceHandleProvided)
#define sec2ctxControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__gpuresControl__
#define sec2ctxControl(pGpuResource, pCallContext, pParams) sec2ctxControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define sec2ctxMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__gpuresMap__
#define sec2ctxMap(pGpuResource, pCallContext, pParams, pCpuMapping) sec2ctxMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define sec2ctxUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__gpuresUnmap__
#define sec2ctxUnmap(pGpuResource, pCallContext, pCpuMapping) sec2ctxUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define sec2ctxShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__gpuresShareCallback__
#define sec2ctxShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) sec2ctxShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define sec2ctxGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__gpuresGetRegBaseOffsetAndSize__
#define sec2ctxGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) sec2ctxGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define sec2ctxGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__gpuresGetMapAddrSpace__
#define sec2ctxGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) sec2ctxGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define sec2ctxInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__gpuresInternalControlForward__
#define sec2ctxInternalControlForward(pGpuResource, command, pParams, size) sec2ctxInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define sec2ctxGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__gpuresGetInternalObjectHandle__
#define sec2ctxGetInternalObjectHandle(pGpuResource) sec2ctxGetInternalObjectHandle_DISPATCH(pGpuResource)
#define sec2ctxAccessCallback_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresAccessCallback__
#define sec2ctxAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) sec2ctxAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define sec2ctxGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemInterMapParams__
#define sec2ctxGetMemInterMapParams(pRmResource, pParams) sec2ctxGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define sec2ctxGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemoryMappingDescriptor__
#define sec2ctxGetMemoryMappingDescriptor(pRmResource, ppMemDesc) sec2ctxGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define sec2ctxControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Prologue__
#define sec2ctxControlSerialization_Prologue(pResource, pCallContext, pParams) sec2ctxControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define sec2ctxControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Epilogue__
#define sec2ctxControlSerialization_Epilogue(pResource, pCallContext, pParams) sec2ctxControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define sec2ctxControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Prologue__
#define sec2ctxControl_Prologue(pResource, pCallContext, pParams) sec2ctxControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define sec2ctxControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Epilogue__
#define sec2ctxControl_Epilogue(pResource, pCallContext, pParams) sec2ctxControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define sec2ctxCanCopy_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__
#define sec2ctxCanCopy(pResource) sec2ctxCanCopy_DISPATCH(pResource)
#define sec2ctxIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsDuplicate__
#define sec2ctxIsDuplicate(pResource, hMemory, pDuplicate) sec2ctxIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define sec2ctxPreDestruct_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resPreDestruct__
#define sec2ctxPreDestruct(pResource) sec2ctxPreDestruct_DISPATCH(pResource)
#define sec2ctxControlFilter_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resControlFilter__
#define sec2ctxControlFilter(pResource, pCallContext, pParams) sec2ctxControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define sec2ctxIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsPartialUnmapSupported__
#define sec2ctxIsPartialUnmapSupported(pResource) sec2ctxIsPartialUnmapSupported_DISPATCH(pResource)
#define sec2ctxMapTo_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resMapTo__
#define sec2ctxMapTo(pResource, pParams) sec2ctxMapTo_DISPATCH(pResource, pParams)
#define sec2ctxUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmapFrom__
#define sec2ctxUnmapFrom(pResource, pParams) sec2ctxUnmapFrom_DISPATCH(pResource, pParams)
#define sec2ctxGetRefCount_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resGetRefCount__
#define sec2ctxGetRefCount(pResource) sec2ctxGetRefCount_DISPATCH(pResource)
#define sec2ctxAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resAddAdditionalDependants__
#define sec2ctxAddAdditionalDependants(pClient, pResource, pReference) sec2ctxAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define sec2ctxGetNotificationListPtr_FNPTR(pNotifier) pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__notifyGetNotificationListPtr__
#define sec2ctxGetNotificationListPtr(pNotifier) sec2ctxGetNotificationListPtr_DISPATCH(pNotifier)
#define sec2ctxGetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__notifyGetNotificationShare__
#define sec2ctxGetNotificationShare(pNotifier) sec2ctxGetNotificationShare_DISPATCH(pNotifier)
#define sec2ctxSetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__notifySetNotificationShare__
#define sec2ctxSetNotificationShare(pNotifier, pNotifShare) sec2ctxSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define sec2ctxUnregisterEvent_FNPTR(pNotifier) pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__notifyUnregisterEvent__
#define sec2ctxUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) sec2ctxUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define sec2ctxGetOrAllocNotifShare_FNPTR(pNotifier) pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__notifyGetOrAllocNotifShare__
#define sec2ctxGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) sec2ctxGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)

// Dispatch functions
static inline NV_STATUS sec2ctxGetSwMethods_DISPATCH(struct Sec2Context *pChannelDescendant, const METHOD **ppMethods, NvU32 *pNumMethods) {
    return pChannelDescendant->__sec2ctxGetSwMethods__(pChannelDescendant, ppMethods, pNumMethods);
}

static inline NvBool sec2ctxIsSwMethodStalling_DISPATCH(struct Sec2Context *pChannelDescendant, NvU32 hHandle) {
    return pChannelDescendant->__sec2ctxIsSwMethodStalling__(pChannelDescendant, hHandle);
}

static inline NV_STATUS sec2ctxCheckMemInterUnmap_DISPATCH(struct Sec2Context *pChannelDescendant, NvBool bSubdeviceHandleProvided) {
    return pChannelDescendant->__sec2ctxCheckMemInterUnmap__(pChannelDescendant, bSubdeviceHandleProvided);
}

static inline NV_STATUS sec2ctxControl_DISPATCH(struct Sec2Context *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__sec2ctxControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS sec2ctxMap_DISPATCH(struct Sec2Context *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__sec2ctxMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS sec2ctxUnmap_DISPATCH(struct Sec2Context *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__sec2ctxUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NvBool sec2ctxShareCallback_DISPATCH(struct Sec2Context *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__sec2ctxShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS sec2ctxGetRegBaseOffsetAndSize_DISPATCH(struct Sec2Context *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__sec2ctxGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS sec2ctxGetMapAddrSpace_DISPATCH(struct Sec2Context *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__sec2ctxGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS sec2ctxInternalControlForward_DISPATCH(struct Sec2Context *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__sec2ctxInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NvHandle sec2ctxGetInternalObjectHandle_DISPATCH(struct Sec2Context *pGpuResource) {
    return pGpuResource->__sec2ctxGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool sec2ctxAccessCallback_DISPATCH(struct Sec2Context *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__sec2ctxAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS sec2ctxGetMemInterMapParams_DISPATCH(struct Sec2Context *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__sec2ctxGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS sec2ctxGetMemoryMappingDescriptor_DISPATCH(struct Sec2Context *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__sec2ctxGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS sec2ctxControlSerialization_Prologue_DISPATCH(struct Sec2Context *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__sec2ctxControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void sec2ctxControlSerialization_Epilogue_DISPATCH(struct Sec2Context *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__sec2ctxControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS sec2ctxControl_Prologue_DISPATCH(struct Sec2Context *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__sec2ctxControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void sec2ctxControl_Epilogue_DISPATCH(struct Sec2Context *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__sec2ctxControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool sec2ctxCanCopy_DISPATCH(struct Sec2Context *pResource) {
    return pResource->__sec2ctxCanCopy__(pResource);
}

static inline NV_STATUS sec2ctxIsDuplicate_DISPATCH(struct Sec2Context *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__sec2ctxIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void sec2ctxPreDestruct_DISPATCH(struct Sec2Context *pResource) {
    pResource->__sec2ctxPreDestruct__(pResource);
}

static inline NV_STATUS sec2ctxControlFilter_DISPATCH(struct Sec2Context *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__sec2ctxControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool sec2ctxIsPartialUnmapSupported_DISPATCH(struct Sec2Context *pResource) {
    return pResource->__sec2ctxIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS sec2ctxMapTo_DISPATCH(struct Sec2Context *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__sec2ctxMapTo__(pResource, pParams);
}

static inline NV_STATUS sec2ctxUnmapFrom_DISPATCH(struct Sec2Context *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__sec2ctxUnmapFrom__(pResource, pParams);
}

static inline NvU32 sec2ctxGetRefCount_DISPATCH(struct Sec2Context *pResource) {
    return pResource->__sec2ctxGetRefCount__(pResource);
}

static inline void sec2ctxAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct Sec2Context *pResource, RsResourceRef *pReference) {
    pResource->__sec2ctxAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline PEVENTNOTIFICATION * sec2ctxGetNotificationListPtr_DISPATCH(struct Sec2Context *pNotifier) {
    return pNotifier->__sec2ctxGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare * sec2ctxGetNotificationShare_DISPATCH(struct Sec2Context *pNotifier) {
    return pNotifier->__sec2ctxGetNotificationShare__(pNotifier);
}

static inline void sec2ctxSetNotificationShare_DISPATCH(struct Sec2Context *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__sec2ctxSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NV_STATUS sec2ctxUnregisterEvent_DISPATCH(struct Sec2Context *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__sec2ctxUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS sec2ctxGetOrAllocNotifShare_DISPATCH(struct Sec2Context *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__sec2ctxGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

NV_STATUS sec2ctxConstructHal_KERNEL(struct Sec2Context *pSec2Context, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);


#ifdef __nvoc_sec2_context_h_disabled
static inline NV_STATUS sec2ctxConstructHal(struct Sec2Context *pSec2Context, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Sec2Context was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_sec2_context_h_disabled
#define sec2ctxConstructHal(pSec2Context, pCallContext, pParams) sec2ctxConstructHal_KERNEL(pSec2Context, pCallContext, pParams)
#endif //__nvoc_sec2_context_h_disabled

#define sec2ctxConstructHal_HAL(pSec2Context, pCallContext, pParams) sec2ctxConstructHal(pSec2Context, pCallContext, pParams)

void sec2ctxDestructHal_KERNEL(struct Sec2Context *pSec2Context);


#ifdef __nvoc_sec2_context_h_disabled
static inline void sec2ctxDestructHal(struct Sec2Context *pSec2Context) {
    NV_ASSERT_FAILED_PRECOMP("Sec2Context was disabled!");
}
#else //__nvoc_sec2_context_h_disabled
#define sec2ctxDestructHal(pSec2Context) sec2ctxDestructHal_KERNEL(pSec2Context)
#endif //__nvoc_sec2_context_h_disabled

#define sec2ctxDestructHal_HAL(pSec2Context) sec2ctxDestructHal(pSec2Context)

static inline NV_STATUS __nvoc_sec2ctxConstruct(struct Sec2Context *arg_pSec2Context, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams) {
    return sec2ctxConstructHal(arg_pSec2Context, arg_pCallContext, arg_pParams);
}

static inline void __nvoc_sec2ctxDestruct(struct Sec2Context *pSec2Context) {
    sec2ctxDestructHal(pSec2Context);
}

#undef PRIVATE_FIELD


#endif  // SEC2_CONTEXT_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_SEC2_CONTEXT_NVOC_H_
