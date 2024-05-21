
#ifndef _G_DISP_CHANNEL_NVOC_H_
#define _G_DISP_CHANNEL_NVOC_H_
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
 * SPDX-FileCopyrightText: Copyright (c) 1993-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
*       This file contains functions managing DispChannel and its derived classes.
*
******************************************************************************/

#pragma once
#include "g_disp_channel_nvoc.h"

#ifndef DISP_CHANNEL_H
#define DISP_CHANNEL_H

#include "gpu/gpu_resource.h"
#include "rmapi/event.h"


struct ContextDma;

#ifndef __NVOC_CLASS_ContextDma_TYPEDEF__
#define __NVOC_CLASS_ContextDma_TYPEDEF__
typedef struct ContextDma ContextDma;
#endif /* __NVOC_CLASS_ContextDma_TYPEDEF__ */

#ifndef __nvoc_class_id_ContextDma
#define __nvoc_class_id_ContextDma 0x88441b
#endif /* __nvoc_class_id_ContextDma */



struct DispObject;

#ifndef __NVOC_CLASS_DispObject_TYPEDEF__
#define __NVOC_CLASS_DispObject_TYPEDEF__
typedef struct DispObject DispObject;
#endif /* __NVOC_CLASS_DispObject_TYPEDEF__ */

#ifndef __nvoc_class_id_DispObject
#define __nvoc_class_id_DispObject 0x999839
#endif /* __nvoc_class_id_DispObject */



/*!
 * Base class for display channels
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_DISP_CHANNEL_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct DispChannel {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct GpuResource __nvoc_base_GpuResource;
    struct Notifier __nvoc_base_Notifier;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^4
    struct RsResource *__nvoc_pbase_RsResource;    // res super^3
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^3
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super^2
    struct GpuResource *__nvoc_pbase_GpuResource;    // gpures super
    struct INotifier *__nvoc_pbase_INotifier;    // inotify super^2
    struct Notifier *__nvoc_pbase_Notifier;    // notify super
    struct DispChannel *__nvoc_pbase_DispChannel;    // dispchn

    // Vtable with 30 per-object function pointers
    NV_STATUS (*__dispchnGetRegBaseOffsetAndSize__)(struct DispChannel * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual override (gpures) base (gpures)
    NV_STATUS (*__dispchnControl__)(struct DispChannel * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__dispchnMap__)(struct DispChannel * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__dispchnUnmap__)(struct DispChannel * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NvBool (*__dispchnShareCallback__)(struct DispChannel * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__dispchnGetMapAddrSpace__)(struct DispChannel * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__dispchnInternalControlForward__)(struct DispChannel * /*this*/, NvU32, void *, NvU32);  // virtual inherited (gpures) base (gpures)
    NvHandle (*__dispchnGetInternalObjectHandle__)(struct DispChannel * /*this*/);  // virtual inherited (gpures) base (gpures)
    NvBool (*__dispchnAccessCallback__)(struct DispChannel * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__dispchnGetMemInterMapParams__)(struct DispChannel * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__dispchnCheckMemInterUnmap__)(struct DispChannel * /*this*/, NvBool);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__dispchnGetMemoryMappingDescriptor__)(struct DispChannel * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__dispchnControlSerialization_Prologue__)(struct DispChannel * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__dispchnControlSerialization_Epilogue__)(struct DispChannel * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__dispchnControl_Prologue__)(struct DispChannel * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__dispchnControl_Epilogue__)(struct DispChannel * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NvBool (*__dispchnCanCopy__)(struct DispChannel * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__dispchnIsDuplicate__)(struct DispChannel * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (gpures)
    void (*__dispchnPreDestruct__)(struct DispChannel * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__dispchnControlFilter__)(struct DispChannel * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (gpures)
    NvBool (*__dispchnIsPartialUnmapSupported__)(struct DispChannel * /*this*/);  // inline virtual inherited (res) base (gpures) body
    NV_STATUS (*__dispchnMapTo__)(struct DispChannel * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__dispchnUnmapFrom__)(struct DispChannel * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (gpures)
    NvU32 (*__dispchnGetRefCount__)(struct DispChannel * /*this*/);  // virtual inherited (res) base (gpures)
    void (*__dispchnAddAdditionalDependants__)(struct RsClient *, struct DispChannel * /*this*/, RsResourceRef *);  // virtual inherited (res) base (gpures)
    PEVENTNOTIFICATION * (*__dispchnGetNotificationListPtr__)(struct DispChannel * /*this*/);  // virtual inherited (notify) base (notify)
    struct NotifShare * (*__dispchnGetNotificationShare__)(struct DispChannel * /*this*/);  // virtual inherited (notify) base (notify)
    void (*__dispchnSetNotificationShare__)(struct DispChannel * /*this*/, struct NotifShare *);  // virtual inherited (notify) base (notify)
    NV_STATUS (*__dispchnUnregisterEvent__)(struct DispChannel * /*this*/, NvHandle, NvHandle, NvHandle, NvHandle);  // virtual inherited (notify) base (notify)
    NV_STATUS (*__dispchnGetOrAllocNotifShare__)(struct DispChannel * /*this*/, NvHandle, NvHandle, struct NotifShare **);  // virtual inherited (notify) base (notify)

    // Data members
    struct DispObject *pDispObject;
    NvU32 DispClass;
    NvU32 InstanceNumber;
    NvP64 pControl;
    NvP64 pPriv;
    NvU32 ControlOffset;
    NvU32 ControlLength;
    NvBool bIsDma;
};

#ifndef __NVOC_CLASS_DispChannel_TYPEDEF__
#define __NVOC_CLASS_DispChannel_TYPEDEF__
typedef struct DispChannel DispChannel;
#endif /* __NVOC_CLASS_DispChannel_TYPEDEF__ */

#ifndef __nvoc_class_id_DispChannel
#define __nvoc_class_id_DispChannel 0xbd2ff3
#endif /* __nvoc_class_id_DispChannel */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_DispChannel;

#define __staticCast_DispChannel(pThis) \
    ((pThis)->__nvoc_pbase_DispChannel)

#ifdef __nvoc_disp_channel_h_disabled
#define __dynamicCast_DispChannel(pThis) ((DispChannel*)NULL)
#else //__nvoc_disp_channel_h_disabled
#define __dynamicCast_DispChannel(pThis) \
    ((DispChannel*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(DispChannel)))
#endif //__nvoc_disp_channel_h_disabled

NV_STATUS __nvoc_objCreateDynamic_DispChannel(DispChannel**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_DispChannel(DispChannel**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams, NvU32 arg_isDma);
#define __objCreate_DispChannel(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams, arg_isDma) \
    __nvoc_objCreate_DispChannel((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams, arg_isDma)


// Wrapper macros
#define dispchnGetRegBaseOffsetAndSize_FNPTR(pDispChannel) pDispChannel->__dispchnGetRegBaseOffsetAndSize__
#define dispchnGetRegBaseOffsetAndSize(pDispChannel, pGpu, pOffset, pSize) dispchnGetRegBaseOffsetAndSize_DISPATCH(pDispChannel, pGpu, pOffset, pSize)
#define dispchnControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresControl__
#define dispchnControl(pGpuResource, pCallContext, pParams) dispchnControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define dispchnMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresMap__
#define dispchnMap(pGpuResource, pCallContext, pParams, pCpuMapping) dispchnMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define dispchnUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresUnmap__
#define dispchnUnmap(pGpuResource, pCallContext, pCpuMapping) dispchnUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define dispchnShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresShareCallback__
#define dispchnShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) dispchnShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define dispchnGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetMapAddrSpace__
#define dispchnGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) dispchnGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define dispchnInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresInternalControlForward__
#define dispchnInternalControlForward(pGpuResource, command, pParams, size) dispchnInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define dispchnGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetInternalObjectHandle__
#define dispchnGetInternalObjectHandle(pGpuResource) dispchnGetInternalObjectHandle_DISPATCH(pGpuResource)
#define dispchnAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresAccessCallback__
#define dispchnAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) dispchnAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define dispchnGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemInterMapParams__
#define dispchnGetMemInterMapParams(pRmResource, pParams) dispchnGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define dispchnCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresCheckMemInterUnmap__
#define dispchnCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) dispchnCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define dispchnGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemoryMappingDescriptor__
#define dispchnGetMemoryMappingDescriptor(pRmResource, ppMemDesc) dispchnGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define dispchnControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Prologue__
#define dispchnControlSerialization_Prologue(pResource, pCallContext, pParams) dispchnControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define dispchnControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Epilogue__
#define dispchnControlSerialization_Epilogue(pResource, pCallContext, pParams) dispchnControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define dispchnControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Prologue__
#define dispchnControl_Prologue(pResource, pCallContext, pParams) dispchnControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define dispchnControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Epilogue__
#define dispchnControl_Epilogue(pResource, pCallContext, pParams) dispchnControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define dispchnCanCopy_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__
#define dispchnCanCopy(pResource) dispchnCanCopy_DISPATCH(pResource)
#define dispchnIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsDuplicate__
#define dispchnIsDuplicate(pResource, hMemory, pDuplicate) dispchnIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define dispchnPreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resPreDestruct__
#define dispchnPreDestruct(pResource) dispchnPreDestruct_DISPATCH(pResource)
#define dispchnControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resControlFilter__
#define dispchnControlFilter(pResource, pCallContext, pParams) dispchnControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define dispchnIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsPartialUnmapSupported__
#define dispchnIsPartialUnmapSupported(pResource) dispchnIsPartialUnmapSupported_DISPATCH(pResource)
#define dispchnMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resMapTo__
#define dispchnMapTo(pResource, pParams) dispchnMapTo_DISPATCH(pResource, pParams)
#define dispchnUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmapFrom__
#define dispchnUnmapFrom(pResource, pParams) dispchnUnmapFrom_DISPATCH(pResource, pParams)
#define dispchnGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resGetRefCount__
#define dispchnGetRefCount(pResource) dispchnGetRefCount_DISPATCH(pResource)
#define dispchnAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resAddAdditionalDependants__
#define dispchnAddAdditionalDependants(pClient, pResource, pReference) dispchnAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define dispchnGetNotificationListPtr_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__notifyGetNotificationListPtr__
#define dispchnGetNotificationListPtr(pNotifier) dispchnGetNotificationListPtr_DISPATCH(pNotifier)
#define dispchnGetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__notifyGetNotificationShare__
#define dispchnGetNotificationShare(pNotifier) dispchnGetNotificationShare_DISPATCH(pNotifier)
#define dispchnSetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__notifySetNotificationShare__
#define dispchnSetNotificationShare(pNotifier, pNotifShare) dispchnSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define dispchnUnregisterEvent_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__notifyUnregisterEvent__
#define dispchnUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) dispchnUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define dispchnGetOrAllocNotifShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__notifyGetOrAllocNotifShare__
#define dispchnGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) dispchnGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)

// Dispatch functions
static inline NV_STATUS dispchnGetRegBaseOffsetAndSize_DISPATCH(struct DispChannel *pDispChannel, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pDispChannel->__dispchnGetRegBaseOffsetAndSize__(pDispChannel, pGpu, pOffset, pSize);
}

static inline NV_STATUS dispchnControl_DISPATCH(struct DispChannel *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__dispchnControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS dispchnMap_DISPATCH(struct DispChannel *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__dispchnMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS dispchnUnmap_DISPATCH(struct DispChannel *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__dispchnUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NvBool dispchnShareCallback_DISPATCH(struct DispChannel *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__dispchnShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS dispchnGetMapAddrSpace_DISPATCH(struct DispChannel *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__dispchnGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS dispchnInternalControlForward_DISPATCH(struct DispChannel *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__dispchnInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NvHandle dispchnGetInternalObjectHandle_DISPATCH(struct DispChannel *pGpuResource) {
    return pGpuResource->__dispchnGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool dispchnAccessCallback_DISPATCH(struct DispChannel *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__dispchnAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS dispchnGetMemInterMapParams_DISPATCH(struct DispChannel *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__dispchnGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS dispchnCheckMemInterUnmap_DISPATCH(struct DispChannel *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__dispchnCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS dispchnGetMemoryMappingDescriptor_DISPATCH(struct DispChannel *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__dispchnGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS dispchnControlSerialization_Prologue_DISPATCH(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__dispchnControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void dispchnControlSerialization_Epilogue_DISPATCH(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__dispchnControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS dispchnControl_Prologue_DISPATCH(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__dispchnControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void dispchnControl_Epilogue_DISPATCH(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__dispchnControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool dispchnCanCopy_DISPATCH(struct DispChannel *pResource) {
    return pResource->__dispchnCanCopy__(pResource);
}

static inline NV_STATUS dispchnIsDuplicate_DISPATCH(struct DispChannel *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__dispchnIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void dispchnPreDestruct_DISPATCH(struct DispChannel *pResource) {
    pResource->__dispchnPreDestruct__(pResource);
}

static inline NV_STATUS dispchnControlFilter_DISPATCH(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__dispchnControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool dispchnIsPartialUnmapSupported_DISPATCH(struct DispChannel *pResource) {
    return pResource->__dispchnIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS dispchnMapTo_DISPATCH(struct DispChannel *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__dispchnMapTo__(pResource, pParams);
}

static inline NV_STATUS dispchnUnmapFrom_DISPATCH(struct DispChannel *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__dispchnUnmapFrom__(pResource, pParams);
}

static inline NvU32 dispchnGetRefCount_DISPATCH(struct DispChannel *pResource) {
    return pResource->__dispchnGetRefCount__(pResource);
}

static inline void dispchnAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct DispChannel *pResource, RsResourceRef *pReference) {
    pResource->__dispchnAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline PEVENTNOTIFICATION * dispchnGetNotificationListPtr_DISPATCH(struct DispChannel *pNotifier) {
    return pNotifier->__dispchnGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare * dispchnGetNotificationShare_DISPATCH(struct DispChannel *pNotifier) {
    return pNotifier->__dispchnGetNotificationShare__(pNotifier);
}

static inline void dispchnSetNotificationShare_DISPATCH(struct DispChannel *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__dispchnSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NV_STATUS dispchnUnregisterEvent_DISPATCH(struct DispChannel *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__dispchnUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS dispchnGetOrAllocNotifShare_DISPATCH(struct DispChannel *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__dispchnGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

NV_STATUS dispchnGetRegBaseOffsetAndSize_IMPL(struct DispChannel *pDispChannel, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize);

NV_STATUS dispchnConstruct_IMPL(struct DispChannel *arg_pDispChannel, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams, NvU32 arg_isDma);

#define __nvoc_dispchnConstruct(arg_pDispChannel, arg_pCallContext, arg_pParams, arg_isDma) dispchnConstruct_IMPL(arg_pDispChannel, arg_pCallContext, arg_pParams, arg_isDma)
void dispchnDestruct_IMPL(struct DispChannel *pDispChannel);

#define __nvoc_dispchnDestruct(pDispChannel) dispchnDestruct_IMPL(pDispChannel)
void dispchnSetRegBaseOffsetAndSize_IMPL(struct DispChannel *pDispChannel, struct OBJGPU *pGpu);

#ifdef __nvoc_disp_channel_h_disabled
static inline void dispchnSetRegBaseOffsetAndSize(struct DispChannel *pDispChannel, struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("DispChannel was disabled!");
}
#else //__nvoc_disp_channel_h_disabled
#define dispchnSetRegBaseOffsetAndSize(pDispChannel, pGpu) dispchnSetRegBaseOffsetAndSize_IMPL(pDispChannel, pGpu)
#endif //__nvoc_disp_channel_h_disabled

NV_STATUS dispchnGrabChannel_IMPL(struct DispChannel *pDispChannel, NvHandle hClient, NvHandle hParent, NvHandle hChannel, NvU32 hClass, void *pAllocParms);

#ifdef __nvoc_disp_channel_h_disabled
static inline NV_STATUS dispchnGrabChannel(struct DispChannel *pDispChannel, NvHandle hClient, NvHandle hParent, NvHandle hChannel, NvU32 hClass, void *pAllocParms) {
    NV_ASSERT_FAILED_PRECOMP("DispChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_disp_channel_h_disabled
#define dispchnGrabChannel(pDispChannel, hClient, hParent, hChannel, hClass, pAllocParms) dispchnGrabChannel_IMPL(pDispChannel, hClient, hParent, hChannel, hClass, pAllocParms)
#endif //__nvoc_disp_channel_h_disabled

NV_STATUS dispchnBindCtx_IMPL(struct OBJGPU *pGpu, struct ContextDma *pContextDma, NvHandle hDispChannel);

#define dispchnBindCtx(pGpu, pContextDma, hDispChannel) dispchnBindCtx_IMPL(pGpu, pContextDma, hDispChannel)
NV_STATUS dispchnUnbindCtx_IMPL(struct OBJGPU *pGpu, struct ContextDma *pContextDma, NvHandle hDispChannel);

#define dispchnUnbindCtx(pGpu, pContextDma, hDispChannel) dispchnUnbindCtx_IMPL(pGpu, pContextDma, hDispChannel)
void dispchnUnbindCtxFromAllChannels_IMPL(struct OBJGPU *pGpu, struct ContextDma *pContextDma);

#define dispchnUnbindCtxFromAllChannels(pGpu, pContextDma) dispchnUnbindCtxFromAllChannels_IMPL(pGpu, pContextDma)
void dispchnUnbindAllCtx_IMPL(struct OBJGPU *pGpu, struct DispChannel *pDispChannel);

#ifdef __nvoc_disp_channel_h_disabled
static inline void dispchnUnbindAllCtx(struct OBJGPU *pGpu, struct DispChannel *pDispChannel) {
    NV_ASSERT_FAILED_PRECOMP("DispChannel was disabled!");
}
#else //__nvoc_disp_channel_h_disabled
#define dispchnUnbindAllCtx(pGpu, pDispChannel) dispchnUnbindAllCtx_IMPL(pGpu, pDispChannel)
#endif //__nvoc_disp_channel_h_disabled

NV_STATUS dispchnGetByHandle_IMPL(struct RsClient *pClient, NvHandle hDisplayChannel, struct DispChannel **ppDispChannel);

#define dispchnGetByHandle(pClient, hDisplayChannel, ppDispChannel) dispchnGetByHandle_IMPL(pClient, hDisplayChannel, ppDispChannel)
#undef PRIVATE_FIELD


/*!
 * RM internal class representing XXX_XXX_CHANNEL_PIO
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_DISP_CHANNEL_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct DispChannelPio {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct DispChannel __nvoc_base_DispChannel;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct RsResource *__nvoc_pbase_RsResource;    // res super^4
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^4
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super^3
    struct GpuResource *__nvoc_pbase_GpuResource;    // gpures super^2
    struct INotifier *__nvoc_pbase_INotifier;    // inotify super^3
    struct Notifier *__nvoc_pbase_Notifier;    // notify super^2
    struct DispChannel *__nvoc_pbase_DispChannel;    // dispchn super
    struct DispChannelPio *__nvoc_pbase_DispChannelPio;    // dispchnpio

    // Vtable with 30 per-object function pointers
    NV_STATUS (*__dispchnpioGetRegBaseOffsetAndSize__)(struct DispChannelPio * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual inherited (dispchn) base (dispchn)
    NV_STATUS (*__dispchnpioControl__)(struct DispChannelPio * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (gpures) base (dispchn)
    NV_STATUS (*__dispchnpioMap__)(struct DispChannelPio * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual inherited (gpures) base (dispchn)
    NV_STATUS (*__dispchnpioUnmap__)(struct DispChannelPio * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual inherited (gpures) base (dispchn)
    NvBool (*__dispchnpioShareCallback__)(struct DispChannelPio * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (dispchn)
    NV_STATUS (*__dispchnpioGetMapAddrSpace__)(struct DispChannelPio * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (gpures) base (dispchn)
    NV_STATUS (*__dispchnpioInternalControlForward__)(struct DispChannelPio * /*this*/, NvU32, void *, NvU32);  // virtual inherited (gpures) base (dispchn)
    NvHandle (*__dispchnpioGetInternalObjectHandle__)(struct DispChannelPio * /*this*/);  // virtual inherited (gpures) base (dispchn)
    NvBool (*__dispchnpioAccessCallback__)(struct DispChannelPio * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (dispchn)
    NV_STATUS (*__dispchnpioGetMemInterMapParams__)(struct DispChannelPio * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (dispchn)
    NV_STATUS (*__dispchnpioCheckMemInterUnmap__)(struct DispChannelPio * /*this*/, NvBool);  // virtual inherited (rmres) base (dispchn)
    NV_STATUS (*__dispchnpioGetMemoryMappingDescriptor__)(struct DispChannelPio * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (dispchn)
    NV_STATUS (*__dispchnpioControlSerialization_Prologue__)(struct DispChannelPio * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (dispchn)
    void (*__dispchnpioControlSerialization_Epilogue__)(struct DispChannelPio * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (dispchn)
    NV_STATUS (*__dispchnpioControl_Prologue__)(struct DispChannelPio * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (dispchn)
    void (*__dispchnpioControl_Epilogue__)(struct DispChannelPio * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (dispchn)
    NvBool (*__dispchnpioCanCopy__)(struct DispChannelPio * /*this*/);  // virtual inherited (res) base (dispchn)
    NV_STATUS (*__dispchnpioIsDuplicate__)(struct DispChannelPio * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (dispchn)
    void (*__dispchnpioPreDestruct__)(struct DispChannelPio * /*this*/);  // virtual inherited (res) base (dispchn)
    NV_STATUS (*__dispchnpioControlFilter__)(struct DispChannelPio * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (dispchn)
    NvBool (*__dispchnpioIsPartialUnmapSupported__)(struct DispChannelPio * /*this*/);  // inline virtual inherited (res) base (dispchn) body
    NV_STATUS (*__dispchnpioMapTo__)(struct DispChannelPio * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (dispchn)
    NV_STATUS (*__dispchnpioUnmapFrom__)(struct DispChannelPio * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (dispchn)
    NvU32 (*__dispchnpioGetRefCount__)(struct DispChannelPio * /*this*/);  // virtual inherited (res) base (dispchn)
    void (*__dispchnpioAddAdditionalDependants__)(struct RsClient *, struct DispChannelPio * /*this*/, RsResourceRef *);  // virtual inherited (res) base (dispchn)
    PEVENTNOTIFICATION * (*__dispchnpioGetNotificationListPtr__)(struct DispChannelPio * /*this*/);  // virtual inherited (notify) base (dispchn)
    struct NotifShare * (*__dispchnpioGetNotificationShare__)(struct DispChannelPio * /*this*/);  // virtual inherited (notify) base (dispchn)
    void (*__dispchnpioSetNotificationShare__)(struct DispChannelPio * /*this*/, struct NotifShare *);  // virtual inherited (notify) base (dispchn)
    NV_STATUS (*__dispchnpioUnregisterEvent__)(struct DispChannelPio * /*this*/, NvHandle, NvHandle, NvHandle, NvHandle);  // virtual inherited (notify) base (dispchn)
    NV_STATUS (*__dispchnpioGetOrAllocNotifShare__)(struct DispChannelPio * /*this*/, NvHandle, NvHandle, struct NotifShare **);  // virtual inherited (notify) base (dispchn)
};

#ifndef __NVOC_CLASS_DispChannelPio_TYPEDEF__
#define __NVOC_CLASS_DispChannelPio_TYPEDEF__
typedef struct DispChannelPio DispChannelPio;
#endif /* __NVOC_CLASS_DispChannelPio_TYPEDEF__ */

#ifndef __nvoc_class_id_DispChannelPio
#define __nvoc_class_id_DispChannelPio 0x10dec3
#endif /* __nvoc_class_id_DispChannelPio */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_DispChannelPio;

#define __staticCast_DispChannelPio(pThis) \
    ((pThis)->__nvoc_pbase_DispChannelPio)

#ifdef __nvoc_disp_channel_h_disabled
#define __dynamicCast_DispChannelPio(pThis) ((DispChannelPio*)NULL)
#else //__nvoc_disp_channel_h_disabled
#define __dynamicCast_DispChannelPio(pThis) \
    ((DispChannelPio*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(DispChannelPio)))
#endif //__nvoc_disp_channel_h_disabled

NV_STATUS __nvoc_objCreateDynamic_DispChannelPio(DispChannelPio**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_DispChannelPio(DispChannelPio**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_DispChannelPio(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_DispChannelPio((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define dispchnpioGetRegBaseOffsetAndSize_FNPTR(pDispChannel) pDispChannel->__nvoc_base_DispChannel.__dispchnGetRegBaseOffsetAndSize__
#define dispchnpioGetRegBaseOffsetAndSize(pDispChannel, pGpu, pOffset, pSize) dispchnpioGetRegBaseOffsetAndSize_DISPATCH(pDispChannel, pGpu, pOffset, pSize)
#define dispchnpioControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__gpuresControl__
#define dispchnpioControl(pGpuResource, pCallContext, pParams) dispchnpioControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define dispchnpioMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__gpuresMap__
#define dispchnpioMap(pGpuResource, pCallContext, pParams, pCpuMapping) dispchnpioMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define dispchnpioUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__gpuresUnmap__
#define dispchnpioUnmap(pGpuResource, pCallContext, pCpuMapping) dispchnpioUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define dispchnpioShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__gpuresShareCallback__
#define dispchnpioShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) dispchnpioShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define dispchnpioGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__gpuresGetMapAddrSpace__
#define dispchnpioGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) dispchnpioGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define dispchnpioInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__gpuresInternalControlForward__
#define dispchnpioInternalControlForward(pGpuResource, command, pParams, size) dispchnpioInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define dispchnpioGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__gpuresGetInternalObjectHandle__
#define dispchnpioGetInternalObjectHandle(pGpuResource) dispchnpioGetInternalObjectHandle_DISPATCH(pGpuResource)
#define dispchnpioAccessCallback_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresAccessCallback__
#define dispchnpioAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) dispchnpioAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define dispchnpioGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemInterMapParams__
#define dispchnpioGetMemInterMapParams(pRmResource, pParams) dispchnpioGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define dispchnpioCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresCheckMemInterUnmap__
#define dispchnpioCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) dispchnpioCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define dispchnpioGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemoryMappingDescriptor__
#define dispchnpioGetMemoryMappingDescriptor(pRmResource, ppMemDesc) dispchnpioGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define dispchnpioControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Prologue__
#define dispchnpioControlSerialization_Prologue(pResource, pCallContext, pParams) dispchnpioControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define dispchnpioControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Epilogue__
#define dispchnpioControlSerialization_Epilogue(pResource, pCallContext, pParams) dispchnpioControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define dispchnpioControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Prologue__
#define dispchnpioControl_Prologue(pResource, pCallContext, pParams) dispchnpioControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define dispchnpioControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Epilogue__
#define dispchnpioControl_Epilogue(pResource, pCallContext, pParams) dispchnpioControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define dispchnpioCanCopy_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__
#define dispchnpioCanCopy(pResource) dispchnpioCanCopy_DISPATCH(pResource)
#define dispchnpioIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsDuplicate__
#define dispchnpioIsDuplicate(pResource, hMemory, pDuplicate) dispchnpioIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define dispchnpioPreDestruct_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resPreDestruct__
#define dispchnpioPreDestruct(pResource) dispchnpioPreDestruct_DISPATCH(pResource)
#define dispchnpioControlFilter_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resControlFilter__
#define dispchnpioControlFilter(pResource, pCallContext, pParams) dispchnpioControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define dispchnpioIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsPartialUnmapSupported__
#define dispchnpioIsPartialUnmapSupported(pResource) dispchnpioIsPartialUnmapSupported_DISPATCH(pResource)
#define dispchnpioMapTo_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resMapTo__
#define dispchnpioMapTo(pResource, pParams) dispchnpioMapTo_DISPATCH(pResource, pParams)
#define dispchnpioUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmapFrom__
#define dispchnpioUnmapFrom(pResource, pParams) dispchnpioUnmapFrom_DISPATCH(pResource, pParams)
#define dispchnpioGetRefCount_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resGetRefCount__
#define dispchnpioGetRefCount(pResource) dispchnpioGetRefCount_DISPATCH(pResource)
#define dispchnpioAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resAddAdditionalDependants__
#define dispchnpioAddAdditionalDependants(pClient, pResource, pReference) dispchnpioAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define dispchnpioGetNotificationListPtr_FNPTR(pNotifier) pNotifier->__nvoc_base_DispChannel.__nvoc_base_Notifier.__notifyGetNotificationListPtr__
#define dispchnpioGetNotificationListPtr(pNotifier) dispchnpioGetNotificationListPtr_DISPATCH(pNotifier)
#define dispchnpioGetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_DispChannel.__nvoc_base_Notifier.__notifyGetNotificationShare__
#define dispchnpioGetNotificationShare(pNotifier) dispchnpioGetNotificationShare_DISPATCH(pNotifier)
#define dispchnpioSetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_DispChannel.__nvoc_base_Notifier.__notifySetNotificationShare__
#define dispchnpioSetNotificationShare(pNotifier, pNotifShare) dispchnpioSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define dispchnpioUnregisterEvent_FNPTR(pNotifier) pNotifier->__nvoc_base_DispChannel.__nvoc_base_Notifier.__notifyUnregisterEvent__
#define dispchnpioUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) dispchnpioUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define dispchnpioGetOrAllocNotifShare_FNPTR(pNotifier) pNotifier->__nvoc_base_DispChannel.__nvoc_base_Notifier.__notifyGetOrAllocNotifShare__
#define dispchnpioGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) dispchnpioGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)

// Dispatch functions
static inline NV_STATUS dispchnpioGetRegBaseOffsetAndSize_DISPATCH(struct DispChannelPio *pDispChannel, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pDispChannel->__dispchnpioGetRegBaseOffsetAndSize__(pDispChannel, pGpu, pOffset, pSize);
}

static inline NV_STATUS dispchnpioControl_DISPATCH(struct DispChannelPio *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__dispchnpioControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS dispchnpioMap_DISPATCH(struct DispChannelPio *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__dispchnpioMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS dispchnpioUnmap_DISPATCH(struct DispChannelPio *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__dispchnpioUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NvBool dispchnpioShareCallback_DISPATCH(struct DispChannelPio *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__dispchnpioShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS dispchnpioGetMapAddrSpace_DISPATCH(struct DispChannelPio *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__dispchnpioGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS dispchnpioInternalControlForward_DISPATCH(struct DispChannelPio *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__dispchnpioInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NvHandle dispchnpioGetInternalObjectHandle_DISPATCH(struct DispChannelPio *pGpuResource) {
    return pGpuResource->__dispchnpioGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool dispchnpioAccessCallback_DISPATCH(struct DispChannelPio *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__dispchnpioAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS dispchnpioGetMemInterMapParams_DISPATCH(struct DispChannelPio *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__dispchnpioGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS dispchnpioCheckMemInterUnmap_DISPATCH(struct DispChannelPio *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__dispchnpioCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS dispchnpioGetMemoryMappingDescriptor_DISPATCH(struct DispChannelPio *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__dispchnpioGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS dispchnpioControlSerialization_Prologue_DISPATCH(struct DispChannelPio *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__dispchnpioControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void dispchnpioControlSerialization_Epilogue_DISPATCH(struct DispChannelPio *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__dispchnpioControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS dispchnpioControl_Prologue_DISPATCH(struct DispChannelPio *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__dispchnpioControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void dispchnpioControl_Epilogue_DISPATCH(struct DispChannelPio *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__dispchnpioControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool dispchnpioCanCopy_DISPATCH(struct DispChannelPio *pResource) {
    return pResource->__dispchnpioCanCopy__(pResource);
}

static inline NV_STATUS dispchnpioIsDuplicate_DISPATCH(struct DispChannelPio *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__dispchnpioIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void dispchnpioPreDestruct_DISPATCH(struct DispChannelPio *pResource) {
    pResource->__dispchnpioPreDestruct__(pResource);
}

static inline NV_STATUS dispchnpioControlFilter_DISPATCH(struct DispChannelPio *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__dispchnpioControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool dispchnpioIsPartialUnmapSupported_DISPATCH(struct DispChannelPio *pResource) {
    return pResource->__dispchnpioIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS dispchnpioMapTo_DISPATCH(struct DispChannelPio *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__dispchnpioMapTo__(pResource, pParams);
}

static inline NV_STATUS dispchnpioUnmapFrom_DISPATCH(struct DispChannelPio *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__dispchnpioUnmapFrom__(pResource, pParams);
}

static inline NvU32 dispchnpioGetRefCount_DISPATCH(struct DispChannelPio *pResource) {
    return pResource->__dispchnpioGetRefCount__(pResource);
}

static inline void dispchnpioAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct DispChannelPio *pResource, RsResourceRef *pReference) {
    pResource->__dispchnpioAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline PEVENTNOTIFICATION * dispchnpioGetNotificationListPtr_DISPATCH(struct DispChannelPio *pNotifier) {
    return pNotifier->__dispchnpioGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare * dispchnpioGetNotificationShare_DISPATCH(struct DispChannelPio *pNotifier) {
    return pNotifier->__dispchnpioGetNotificationShare__(pNotifier);
}

static inline void dispchnpioSetNotificationShare_DISPATCH(struct DispChannelPio *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__dispchnpioSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NV_STATUS dispchnpioUnregisterEvent_DISPATCH(struct DispChannelPio *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__dispchnpioUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS dispchnpioGetOrAllocNotifShare_DISPATCH(struct DispChannelPio *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__dispchnpioGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

NV_STATUS dispchnpioConstruct_IMPL(struct DispChannelPio *arg_pDispChannelPio, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_dispchnpioConstruct(arg_pDispChannelPio, arg_pCallContext, arg_pParams) dispchnpioConstruct_IMPL(arg_pDispChannelPio, arg_pCallContext, arg_pParams)
#undef PRIVATE_FIELD


/*!
 * RM internal class representing XXX_XXX_CHANNEL_DMA
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_DISP_CHANNEL_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct DispChannelDma {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct DispChannel __nvoc_base_DispChannel;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct RsResource *__nvoc_pbase_RsResource;    // res super^4
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^4
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super^3
    struct GpuResource *__nvoc_pbase_GpuResource;    // gpures super^2
    struct INotifier *__nvoc_pbase_INotifier;    // inotify super^3
    struct Notifier *__nvoc_pbase_Notifier;    // notify super^2
    struct DispChannel *__nvoc_pbase_DispChannel;    // dispchn super
    struct DispChannelDma *__nvoc_pbase_DispChannelDma;    // dispchndma

    // Vtable with 30 per-object function pointers
    NV_STATUS (*__dispchndmaGetRegBaseOffsetAndSize__)(struct DispChannelDma * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual inherited (dispchn) base (dispchn)
    NV_STATUS (*__dispchndmaControl__)(struct DispChannelDma * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (gpures) base (dispchn)
    NV_STATUS (*__dispchndmaMap__)(struct DispChannelDma * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual inherited (gpures) base (dispchn)
    NV_STATUS (*__dispchndmaUnmap__)(struct DispChannelDma * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual inherited (gpures) base (dispchn)
    NvBool (*__dispchndmaShareCallback__)(struct DispChannelDma * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (dispchn)
    NV_STATUS (*__dispchndmaGetMapAddrSpace__)(struct DispChannelDma * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (gpures) base (dispchn)
    NV_STATUS (*__dispchndmaInternalControlForward__)(struct DispChannelDma * /*this*/, NvU32, void *, NvU32);  // virtual inherited (gpures) base (dispchn)
    NvHandle (*__dispchndmaGetInternalObjectHandle__)(struct DispChannelDma * /*this*/);  // virtual inherited (gpures) base (dispchn)
    NvBool (*__dispchndmaAccessCallback__)(struct DispChannelDma * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (dispchn)
    NV_STATUS (*__dispchndmaGetMemInterMapParams__)(struct DispChannelDma * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (dispchn)
    NV_STATUS (*__dispchndmaCheckMemInterUnmap__)(struct DispChannelDma * /*this*/, NvBool);  // virtual inherited (rmres) base (dispchn)
    NV_STATUS (*__dispchndmaGetMemoryMappingDescriptor__)(struct DispChannelDma * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (dispchn)
    NV_STATUS (*__dispchndmaControlSerialization_Prologue__)(struct DispChannelDma * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (dispchn)
    void (*__dispchndmaControlSerialization_Epilogue__)(struct DispChannelDma * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (dispchn)
    NV_STATUS (*__dispchndmaControl_Prologue__)(struct DispChannelDma * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (dispchn)
    void (*__dispchndmaControl_Epilogue__)(struct DispChannelDma * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (dispchn)
    NvBool (*__dispchndmaCanCopy__)(struct DispChannelDma * /*this*/);  // virtual inherited (res) base (dispchn)
    NV_STATUS (*__dispchndmaIsDuplicate__)(struct DispChannelDma * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (dispchn)
    void (*__dispchndmaPreDestruct__)(struct DispChannelDma * /*this*/);  // virtual inherited (res) base (dispchn)
    NV_STATUS (*__dispchndmaControlFilter__)(struct DispChannelDma * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (dispchn)
    NvBool (*__dispchndmaIsPartialUnmapSupported__)(struct DispChannelDma * /*this*/);  // inline virtual inherited (res) base (dispchn) body
    NV_STATUS (*__dispchndmaMapTo__)(struct DispChannelDma * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (dispchn)
    NV_STATUS (*__dispchndmaUnmapFrom__)(struct DispChannelDma * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (dispchn)
    NvU32 (*__dispchndmaGetRefCount__)(struct DispChannelDma * /*this*/);  // virtual inherited (res) base (dispchn)
    void (*__dispchndmaAddAdditionalDependants__)(struct RsClient *, struct DispChannelDma * /*this*/, RsResourceRef *);  // virtual inherited (res) base (dispchn)
    PEVENTNOTIFICATION * (*__dispchndmaGetNotificationListPtr__)(struct DispChannelDma * /*this*/);  // virtual inherited (notify) base (dispchn)
    struct NotifShare * (*__dispchndmaGetNotificationShare__)(struct DispChannelDma * /*this*/);  // virtual inherited (notify) base (dispchn)
    void (*__dispchndmaSetNotificationShare__)(struct DispChannelDma * /*this*/, struct NotifShare *);  // virtual inherited (notify) base (dispchn)
    NV_STATUS (*__dispchndmaUnregisterEvent__)(struct DispChannelDma * /*this*/, NvHandle, NvHandle, NvHandle, NvHandle);  // virtual inherited (notify) base (dispchn)
    NV_STATUS (*__dispchndmaGetOrAllocNotifShare__)(struct DispChannelDma * /*this*/, NvHandle, NvHandle, struct NotifShare **);  // virtual inherited (notify) base (dispchn)
};

#ifndef __NVOC_CLASS_DispChannelDma_TYPEDEF__
#define __NVOC_CLASS_DispChannelDma_TYPEDEF__
typedef struct DispChannelDma DispChannelDma;
#endif /* __NVOC_CLASS_DispChannelDma_TYPEDEF__ */

#ifndef __nvoc_class_id_DispChannelDma
#define __nvoc_class_id_DispChannelDma 0xfe3d2e
#endif /* __nvoc_class_id_DispChannelDma */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_DispChannelDma;

#define __staticCast_DispChannelDma(pThis) \
    ((pThis)->__nvoc_pbase_DispChannelDma)

#ifdef __nvoc_disp_channel_h_disabled
#define __dynamicCast_DispChannelDma(pThis) ((DispChannelDma*)NULL)
#else //__nvoc_disp_channel_h_disabled
#define __dynamicCast_DispChannelDma(pThis) \
    ((DispChannelDma*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(DispChannelDma)))
#endif //__nvoc_disp_channel_h_disabled

NV_STATUS __nvoc_objCreateDynamic_DispChannelDma(DispChannelDma**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_DispChannelDma(DispChannelDma**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_DispChannelDma(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_DispChannelDma((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define dispchndmaGetRegBaseOffsetAndSize_FNPTR(pDispChannel) pDispChannel->__nvoc_base_DispChannel.__dispchnGetRegBaseOffsetAndSize__
#define dispchndmaGetRegBaseOffsetAndSize(pDispChannel, pGpu, pOffset, pSize) dispchndmaGetRegBaseOffsetAndSize_DISPATCH(pDispChannel, pGpu, pOffset, pSize)
#define dispchndmaControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__gpuresControl__
#define dispchndmaControl(pGpuResource, pCallContext, pParams) dispchndmaControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define dispchndmaMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__gpuresMap__
#define dispchndmaMap(pGpuResource, pCallContext, pParams, pCpuMapping) dispchndmaMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define dispchndmaUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__gpuresUnmap__
#define dispchndmaUnmap(pGpuResource, pCallContext, pCpuMapping) dispchndmaUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define dispchndmaShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__gpuresShareCallback__
#define dispchndmaShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) dispchndmaShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define dispchndmaGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__gpuresGetMapAddrSpace__
#define dispchndmaGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) dispchndmaGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define dispchndmaInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__gpuresInternalControlForward__
#define dispchndmaInternalControlForward(pGpuResource, command, pParams, size) dispchndmaInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define dispchndmaGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__gpuresGetInternalObjectHandle__
#define dispchndmaGetInternalObjectHandle(pGpuResource) dispchndmaGetInternalObjectHandle_DISPATCH(pGpuResource)
#define dispchndmaAccessCallback_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresAccessCallback__
#define dispchndmaAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) dispchndmaAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define dispchndmaGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemInterMapParams__
#define dispchndmaGetMemInterMapParams(pRmResource, pParams) dispchndmaGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define dispchndmaCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresCheckMemInterUnmap__
#define dispchndmaCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) dispchndmaCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define dispchndmaGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemoryMappingDescriptor__
#define dispchndmaGetMemoryMappingDescriptor(pRmResource, ppMemDesc) dispchndmaGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define dispchndmaControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Prologue__
#define dispchndmaControlSerialization_Prologue(pResource, pCallContext, pParams) dispchndmaControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define dispchndmaControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Epilogue__
#define dispchndmaControlSerialization_Epilogue(pResource, pCallContext, pParams) dispchndmaControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define dispchndmaControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Prologue__
#define dispchndmaControl_Prologue(pResource, pCallContext, pParams) dispchndmaControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define dispchndmaControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Epilogue__
#define dispchndmaControl_Epilogue(pResource, pCallContext, pParams) dispchndmaControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define dispchndmaCanCopy_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__
#define dispchndmaCanCopy(pResource) dispchndmaCanCopy_DISPATCH(pResource)
#define dispchndmaIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsDuplicate__
#define dispchndmaIsDuplicate(pResource, hMemory, pDuplicate) dispchndmaIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define dispchndmaPreDestruct_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resPreDestruct__
#define dispchndmaPreDestruct(pResource) dispchndmaPreDestruct_DISPATCH(pResource)
#define dispchndmaControlFilter_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resControlFilter__
#define dispchndmaControlFilter(pResource, pCallContext, pParams) dispchndmaControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define dispchndmaIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsPartialUnmapSupported__
#define dispchndmaIsPartialUnmapSupported(pResource) dispchndmaIsPartialUnmapSupported_DISPATCH(pResource)
#define dispchndmaMapTo_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resMapTo__
#define dispchndmaMapTo(pResource, pParams) dispchndmaMapTo_DISPATCH(pResource, pParams)
#define dispchndmaUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmapFrom__
#define dispchndmaUnmapFrom(pResource, pParams) dispchndmaUnmapFrom_DISPATCH(pResource, pParams)
#define dispchndmaGetRefCount_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resGetRefCount__
#define dispchndmaGetRefCount(pResource) dispchndmaGetRefCount_DISPATCH(pResource)
#define dispchndmaAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resAddAdditionalDependants__
#define dispchndmaAddAdditionalDependants(pClient, pResource, pReference) dispchndmaAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define dispchndmaGetNotificationListPtr_FNPTR(pNotifier) pNotifier->__nvoc_base_DispChannel.__nvoc_base_Notifier.__notifyGetNotificationListPtr__
#define dispchndmaGetNotificationListPtr(pNotifier) dispchndmaGetNotificationListPtr_DISPATCH(pNotifier)
#define dispchndmaGetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_DispChannel.__nvoc_base_Notifier.__notifyGetNotificationShare__
#define dispchndmaGetNotificationShare(pNotifier) dispchndmaGetNotificationShare_DISPATCH(pNotifier)
#define dispchndmaSetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_DispChannel.__nvoc_base_Notifier.__notifySetNotificationShare__
#define dispchndmaSetNotificationShare(pNotifier, pNotifShare) dispchndmaSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define dispchndmaUnregisterEvent_FNPTR(pNotifier) pNotifier->__nvoc_base_DispChannel.__nvoc_base_Notifier.__notifyUnregisterEvent__
#define dispchndmaUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) dispchndmaUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define dispchndmaGetOrAllocNotifShare_FNPTR(pNotifier) pNotifier->__nvoc_base_DispChannel.__nvoc_base_Notifier.__notifyGetOrAllocNotifShare__
#define dispchndmaGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) dispchndmaGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)

// Dispatch functions
static inline NV_STATUS dispchndmaGetRegBaseOffsetAndSize_DISPATCH(struct DispChannelDma *pDispChannel, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pDispChannel->__dispchndmaGetRegBaseOffsetAndSize__(pDispChannel, pGpu, pOffset, pSize);
}

static inline NV_STATUS dispchndmaControl_DISPATCH(struct DispChannelDma *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__dispchndmaControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS dispchndmaMap_DISPATCH(struct DispChannelDma *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__dispchndmaMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS dispchndmaUnmap_DISPATCH(struct DispChannelDma *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__dispchndmaUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NvBool dispchndmaShareCallback_DISPATCH(struct DispChannelDma *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__dispchndmaShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS dispchndmaGetMapAddrSpace_DISPATCH(struct DispChannelDma *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__dispchndmaGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS dispchndmaInternalControlForward_DISPATCH(struct DispChannelDma *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__dispchndmaInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NvHandle dispchndmaGetInternalObjectHandle_DISPATCH(struct DispChannelDma *pGpuResource) {
    return pGpuResource->__dispchndmaGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool dispchndmaAccessCallback_DISPATCH(struct DispChannelDma *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__dispchndmaAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS dispchndmaGetMemInterMapParams_DISPATCH(struct DispChannelDma *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__dispchndmaGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS dispchndmaCheckMemInterUnmap_DISPATCH(struct DispChannelDma *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__dispchndmaCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS dispchndmaGetMemoryMappingDescriptor_DISPATCH(struct DispChannelDma *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__dispchndmaGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS dispchndmaControlSerialization_Prologue_DISPATCH(struct DispChannelDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__dispchndmaControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void dispchndmaControlSerialization_Epilogue_DISPATCH(struct DispChannelDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__dispchndmaControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS dispchndmaControl_Prologue_DISPATCH(struct DispChannelDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__dispchndmaControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void dispchndmaControl_Epilogue_DISPATCH(struct DispChannelDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__dispchndmaControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool dispchndmaCanCopy_DISPATCH(struct DispChannelDma *pResource) {
    return pResource->__dispchndmaCanCopy__(pResource);
}

static inline NV_STATUS dispchndmaIsDuplicate_DISPATCH(struct DispChannelDma *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__dispchndmaIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void dispchndmaPreDestruct_DISPATCH(struct DispChannelDma *pResource) {
    pResource->__dispchndmaPreDestruct__(pResource);
}

static inline NV_STATUS dispchndmaControlFilter_DISPATCH(struct DispChannelDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__dispchndmaControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool dispchndmaIsPartialUnmapSupported_DISPATCH(struct DispChannelDma *pResource) {
    return pResource->__dispchndmaIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS dispchndmaMapTo_DISPATCH(struct DispChannelDma *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__dispchndmaMapTo__(pResource, pParams);
}

static inline NV_STATUS dispchndmaUnmapFrom_DISPATCH(struct DispChannelDma *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__dispchndmaUnmapFrom__(pResource, pParams);
}

static inline NvU32 dispchndmaGetRefCount_DISPATCH(struct DispChannelDma *pResource) {
    return pResource->__dispchndmaGetRefCount__(pResource);
}

static inline void dispchndmaAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct DispChannelDma *pResource, RsResourceRef *pReference) {
    pResource->__dispchndmaAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline PEVENTNOTIFICATION * dispchndmaGetNotificationListPtr_DISPATCH(struct DispChannelDma *pNotifier) {
    return pNotifier->__dispchndmaGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare * dispchndmaGetNotificationShare_DISPATCH(struct DispChannelDma *pNotifier) {
    return pNotifier->__dispchndmaGetNotificationShare__(pNotifier);
}

static inline void dispchndmaSetNotificationShare_DISPATCH(struct DispChannelDma *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__dispchndmaSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NV_STATUS dispchndmaUnregisterEvent_DISPATCH(struct DispChannelDma *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__dispchndmaUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS dispchndmaGetOrAllocNotifShare_DISPATCH(struct DispChannelDma *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__dispchndmaGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

NV_STATUS dispchndmaConstruct_IMPL(struct DispChannelDma *arg_pDispChannelDma, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_dispchndmaConstruct(arg_pDispChannelDma, arg_pCallContext, arg_pParams) dispchndmaConstruct_IMPL(arg_pDispChannelDma, arg_pCallContext, arg_pParams)
#undef PRIVATE_FIELD


#endif // DISP_CHANNEL_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_DISP_CHANNEL_NVOC_H_
