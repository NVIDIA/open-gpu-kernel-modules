
#ifndef _G_UVM_SW_NVOC_H_
#define _G_UVM_SW_NVOC_H_
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
#include "g_uvm_sw_nvoc.h"

#ifndef UVM_SW_H
#define UVM_SW_H

#include "core/core.h"
#include "kernel/gpu/fifo/channel_descendant.h"

/*!
 * RM internal class representing GP100_UVM_SW
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_UVM_SW_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct UvmSwObject {

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
    struct UvmSwObject *__nvoc_pbase_UvmSwObject;    // uvmsw

    // Vtable with 32 per-object function pointers
    NV_STATUS (*__uvmswGetSwMethods__)(struct UvmSwObject * /*this*/, const METHOD **, NvU32 *);  // virtual halified (singleton optimized) override (chandes) base (chandes) body
    NvBool (*__uvmswIsSwMethodStalling__)(struct UvmSwObject * /*this*/, NvU32);  // virtual inherited (chandes) base (chandes)
    NV_STATUS (*__uvmswCheckMemInterUnmap__)(struct UvmSwObject * /*this*/, NvBool);  // virtual inherited (chandes) base (chandes)
    NV_STATUS (*__uvmswControl__)(struct UvmSwObject * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (gpures) base (chandes)
    NV_STATUS (*__uvmswMap__)(struct UvmSwObject * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual inherited (gpures) base (chandes)
    NV_STATUS (*__uvmswUnmap__)(struct UvmSwObject * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual inherited (gpures) base (chandes)
    NvBool (*__uvmswShareCallback__)(struct UvmSwObject * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (chandes)
    NV_STATUS (*__uvmswGetRegBaseOffsetAndSize__)(struct UvmSwObject * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual inherited (gpures) base (chandes)
    NV_STATUS (*__uvmswGetMapAddrSpace__)(struct UvmSwObject * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (gpures) base (chandes)
    NV_STATUS (*__uvmswInternalControlForward__)(struct UvmSwObject * /*this*/, NvU32, void *, NvU32);  // virtual inherited (gpures) base (chandes)
    NvHandle (*__uvmswGetInternalObjectHandle__)(struct UvmSwObject * /*this*/);  // virtual inherited (gpures) base (chandes)
    NvBool (*__uvmswAccessCallback__)(struct UvmSwObject * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (chandes)
    NV_STATUS (*__uvmswGetMemInterMapParams__)(struct UvmSwObject * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (chandes)
    NV_STATUS (*__uvmswGetMemoryMappingDescriptor__)(struct UvmSwObject * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (chandes)
    NV_STATUS (*__uvmswControlSerialization_Prologue__)(struct UvmSwObject * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (chandes)
    void (*__uvmswControlSerialization_Epilogue__)(struct UvmSwObject * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (chandes)
    NV_STATUS (*__uvmswControl_Prologue__)(struct UvmSwObject * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (chandes)
    void (*__uvmswControl_Epilogue__)(struct UvmSwObject * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (chandes)
    NvBool (*__uvmswCanCopy__)(struct UvmSwObject * /*this*/);  // virtual inherited (res) base (chandes)
    NV_STATUS (*__uvmswIsDuplicate__)(struct UvmSwObject * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (chandes)
    void (*__uvmswPreDestruct__)(struct UvmSwObject * /*this*/);  // virtual inherited (res) base (chandes)
    NV_STATUS (*__uvmswControlFilter__)(struct UvmSwObject * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (chandes)
    NvBool (*__uvmswIsPartialUnmapSupported__)(struct UvmSwObject * /*this*/);  // inline virtual inherited (res) base (chandes) body
    NV_STATUS (*__uvmswMapTo__)(struct UvmSwObject * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (chandes)
    NV_STATUS (*__uvmswUnmapFrom__)(struct UvmSwObject * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (chandes)
    NvU32 (*__uvmswGetRefCount__)(struct UvmSwObject * /*this*/);  // virtual inherited (res) base (chandes)
    void (*__uvmswAddAdditionalDependants__)(struct RsClient *, struct UvmSwObject * /*this*/, RsResourceRef *);  // virtual inherited (res) base (chandes)
    PEVENTNOTIFICATION * (*__uvmswGetNotificationListPtr__)(struct UvmSwObject * /*this*/);  // virtual inherited (notify) base (chandes)
    struct NotifShare * (*__uvmswGetNotificationShare__)(struct UvmSwObject * /*this*/);  // virtual inherited (notify) base (chandes)
    void (*__uvmswSetNotificationShare__)(struct UvmSwObject * /*this*/, struct NotifShare *);  // virtual inherited (notify) base (chandes)
    NV_STATUS (*__uvmswUnregisterEvent__)(struct UvmSwObject * /*this*/, NvHandle, NvHandle, NvHandle, NvHandle);  // virtual inherited (notify) base (chandes)
    NV_STATUS (*__uvmswGetOrAllocNotifShare__)(struct UvmSwObject * /*this*/, NvHandle, NvHandle, struct NotifShare **);  // virtual inherited (notify) base (chandes)

    // Data members
    NvU32 methodA;
    NvU32 methodB;
    NvBool bCancelMethodASet;
    NvBool bCancelMethodBSet;
    NvBool bClearMethodASet;
};

#ifndef __NVOC_CLASS_UvmSwObject_TYPEDEF__
#define __NVOC_CLASS_UvmSwObject_TYPEDEF__
typedef struct UvmSwObject UvmSwObject;
#endif /* __NVOC_CLASS_UvmSwObject_TYPEDEF__ */

#ifndef __nvoc_class_id_UvmSwObject
#define __nvoc_class_id_UvmSwObject 0xc35503
#endif /* __nvoc_class_id_UvmSwObject */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_UvmSwObject;

#define __staticCast_UvmSwObject(pThis) \
    ((pThis)->__nvoc_pbase_UvmSwObject)

#ifdef __nvoc_uvm_sw_h_disabled
#define __dynamicCast_UvmSwObject(pThis) ((UvmSwObject*)NULL)
#else //__nvoc_uvm_sw_h_disabled
#define __dynamicCast_UvmSwObject(pThis) \
    ((UvmSwObject*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(UvmSwObject)))
#endif //__nvoc_uvm_sw_h_disabled

NV_STATUS __nvoc_objCreateDynamic_UvmSwObject(UvmSwObject**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_UvmSwObject(UvmSwObject**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_UvmSwObject(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_UvmSwObject((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define uvmswGetSwMethods_FNPTR(pUvmSw) pUvmSw->__uvmswGetSwMethods__
#define uvmswGetSwMethods(pUvmSw, ppMethods, pNumMethods) uvmswGetSwMethods_DISPATCH(pUvmSw, ppMethods, pNumMethods)
#define uvmswGetSwMethods_HAL(pUvmSw, ppMethods, pNumMethods) uvmswGetSwMethods_DISPATCH(pUvmSw, ppMethods, pNumMethods)
#define uvmswIsSwMethodStalling_FNPTR(pChannelDescendant) pChannelDescendant->__nvoc_base_ChannelDescendant.__chandesIsSwMethodStalling__
#define uvmswIsSwMethodStalling(pChannelDescendant, hHandle) uvmswIsSwMethodStalling_DISPATCH(pChannelDescendant, hHandle)
#define uvmswCheckMemInterUnmap_FNPTR(pChannelDescendant) pChannelDescendant->__nvoc_base_ChannelDescendant.__chandesCheckMemInterUnmap__
#define uvmswCheckMemInterUnmap(pChannelDescendant, bSubdeviceHandleProvided) uvmswCheckMemInterUnmap_DISPATCH(pChannelDescendant, bSubdeviceHandleProvided)
#define uvmswControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__gpuresControl__
#define uvmswControl(pGpuResource, pCallContext, pParams) uvmswControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define uvmswMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__gpuresMap__
#define uvmswMap(pGpuResource, pCallContext, pParams, pCpuMapping) uvmswMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define uvmswUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__gpuresUnmap__
#define uvmswUnmap(pGpuResource, pCallContext, pCpuMapping) uvmswUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define uvmswShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__gpuresShareCallback__
#define uvmswShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) uvmswShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define uvmswGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__gpuresGetRegBaseOffsetAndSize__
#define uvmswGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) uvmswGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define uvmswGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__gpuresGetMapAddrSpace__
#define uvmswGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) uvmswGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define uvmswInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__gpuresInternalControlForward__
#define uvmswInternalControlForward(pGpuResource, command, pParams, size) uvmswInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define uvmswGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__gpuresGetInternalObjectHandle__
#define uvmswGetInternalObjectHandle(pGpuResource) uvmswGetInternalObjectHandle_DISPATCH(pGpuResource)
#define uvmswAccessCallback_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresAccessCallback__
#define uvmswAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) uvmswAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define uvmswGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemInterMapParams__
#define uvmswGetMemInterMapParams(pRmResource, pParams) uvmswGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define uvmswGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemoryMappingDescriptor__
#define uvmswGetMemoryMappingDescriptor(pRmResource, ppMemDesc) uvmswGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define uvmswControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Prologue__
#define uvmswControlSerialization_Prologue(pResource, pCallContext, pParams) uvmswControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define uvmswControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Epilogue__
#define uvmswControlSerialization_Epilogue(pResource, pCallContext, pParams) uvmswControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define uvmswControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Prologue__
#define uvmswControl_Prologue(pResource, pCallContext, pParams) uvmswControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define uvmswControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Epilogue__
#define uvmswControl_Epilogue(pResource, pCallContext, pParams) uvmswControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define uvmswCanCopy_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__
#define uvmswCanCopy(pResource) uvmswCanCopy_DISPATCH(pResource)
#define uvmswIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsDuplicate__
#define uvmswIsDuplicate(pResource, hMemory, pDuplicate) uvmswIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define uvmswPreDestruct_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resPreDestruct__
#define uvmswPreDestruct(pResource) uvmswPreDestruct_DISPATCH(pResource)
#define uvmswControlFilter_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resControlFilter__
#define uvmswControlFilter(pResource, pCallContext, pParams) uvmswControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define uvmswIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsPartialUnmapSupported__
#define uvmswIsPartialUnmapSupported(pResource) uvmswIsPartialUnmapSupported_DISPATCH(pResource)
#define uvmswMapTo_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resMapTo__
#define uvmswMapTo(pResource, pParams) uvmswMapTo_DISPATCH(pResource, pParams)
#define uvmswUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmapFrom__
#define uvmswUnmapFrom(pResource, pParams) uvmswUnmapFrom_DISPATCH(pResource, pParams)
#define uvmswGetRefCount_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resGetRefCount__
#define uvmswGetRefCount(pResource) uvmswGetRefCount_DISPATCH(pResource)
#define uvmswAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resAddAdditionalDependants__
#define uvmswAddAdditionalDependants(pClient, pResource, pReference) uvmswAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define uvmswGetNotificationListPtr_FNPTR(pNotifier) pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__notifyGetNotificationListPtr__
#define uvmswGetNotificationListPtr(pNotifier) uvmswGetNotificationListPtr_DISPATCH(pNotifier)
#define uvmswGetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__notifyGetNotificationShare__
#define uvmswGetNotificationShare(pNotifier) uvmswGetNotificationShare_DISPATCH(pNotifier)
#define uvmswSetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__notifySetNotificationShare__
#define uvmswSetNotificationShare(pNotifier, pNotifShare) uvmswSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define uvmswUnregisterEvent_FNPTR(pNotifier) pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__notifyUnregisterEvent__
#define uvmswUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) uvmswUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define uvmswGetOrAllocNotifShare_FNPTR(pNotifier) pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__notifyGetOrAllocNotifShare__
#define uvmswGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) uvmswGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)

// Dispatch functions
static inline NV_STATUS uvmswGetSwMethods_DISPATCH(struct UvmSwObject *pUvmSw, const METHOD **ppMethods, NvU32 *pNumMethods) {
    return pUvmSw->__uvmswGetSwMethods__(pUvmSw, ppMethods, pNumMethods);
}

static inline NvBool uvmswIsSwMethodStalling_DISPATCH(struct UvmSwObject *pChannelDescendant, NvU32 hHandle) {
    return pChannelDescendant->__uvmswIsSwMethodStalling__(pChannelDescendant, hHandle);
}

static inline NV_STATUS uvmswCheckMemInterUnmap_DISPATCH(struct UvmSwObject *pChannelDescendant, NvBool bSubdeviceHandleProvided) {
    return pChannelDescendant->__uvmswCheckMemInterUnmap__(pChannelDescendant, bSubdeviceHandleProvided);
}

static inline NV_STATUS uvmswControl_DISPATCH(struct UvmSwObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__uvmswControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS uvmswMap_DISPATCH(struct UvmSwObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__uvmswMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS uvmswUnmap_DISPATCH(struct UvmSwObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__uvmswUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NvBool uvmswShareCallback_DISPATCH(struct UvmSwObject *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__uvmswShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS uvmswGetRegBaseOffsetAndSize_DISPATCH(struct UvmSwObject *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__uvmswGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS uvmswGetMapAddrSpace_DISPATCH(struct UvmSwObject *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__uvmswGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS uvmswInternalControlForward_DISPATCH(struct UvmSwObject *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__uvmswInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NvHandle uvmswGetInternalObjectHandle_DISPATCH(struct UvmSwObject *pGpuResource) {
    return pGpuResource->__uvmswGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool uvmswAccessCallback_DISPATCH(struct UvmSwObject *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__uvmswAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS uvmswGetMemInterMapParams_DISPATCH(struct UvmSwObject *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__uvmswGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS uvmswGetMemoryMappingDescriptor_DISPATCH(struct UvmSwObject *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__uvmswGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS uvmswControlSerialization_Prologue_DISPATCH(struct UvmSwObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__uvmswControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void uvmswControlSerialization_Epilogue_DISPATCH(struct UvmSwObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__uvmswControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS uvmswControl_Prologue_DISPATCH(struct UvmSwObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__uvmswControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void uvmswControl_Epilogue_DISPATCH(struct UvmSwObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__uvmswControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool uvmswCanCopy_DISPATCH(struct UvmSwObject *pResource) {
    return pResource->__uvmswCanCopy__(pResource);
}

static inline NV_STATUS uvmswIsDuplicate_DISPATCH(struct UvmSwObject *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__uvmswIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void uvmswPreDestruct_DISPATCH(struct UvmSwObject *pResource) {
    pResource->__uvmswPreDestruct__(pResource);
}

static inline NV_STATUS uvmswControlFilter_DISPATCH(struct UvmSwObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__uvmswControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool uvmswIsPartialUnmapSupported_DISPATCH(struct UvmSwObject *pResource) {
    return pResource->__uvmswIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS uvmswMapTo_DISPATCH(struct UvmSwObject *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__uvmswMapTo__(pResource, pParams);
}

static inline NV_STATUS uvmswUnmapFrom_DISPATCH(struct UvmSwObject *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__uvmswUnmapFrom__(pResource, pParams);
}

static inline NvU32 uvmswGetRefCount_DISPATCH(struct UvmSwObject *pResource) {
    return pResource->__uvmswGetRefCount__(pResource);
}

static inline void uvmswAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct UvmSwObject *pResource, RsResourceRef *pReference) {
    pResource->__uvmswAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline PEVENTNOTIFICATION * uvmswGetNotificationListPtr_DISPATCH(struct UvmSwObject *pNotifier) {
    return pNotifier->__uvmswGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare * uvmswGetNotificationShare_DISPATCH(struct UvmSwObject *pNotifier) {
    return pNotifier->__uvmswGetNotificationShare__(pNotifier);
}

static inline void uvmswSetNotificationShare_DISPATCH(struct UvmSwObject *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__uvmswSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NV_STATUS uvmswUnregisterEvent_DISPATCH(struct UvmSwObject *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__uvmswUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS uvmswGetOrAllocNotifShare_DISPATCH(struct UvmSwObject *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__uvmswGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

static inline NV_STATUS uvmswGetSwMethods_56cd7a(struct UvmSwObject *pUvmSw, const METHOD **ppMethods, NvU32 *pNumMethods) {
    return NV_OK;
}

NV_STATUS uvmswConstruct_IMPL(struct UvmSwObject *arg_pUvmSw, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_uvmswConstruct(arg_pUvmSw, arg_pCallContext, arg_pParams) uvmswConstruct_IMPL(arg_pUvmSw, arg_pCallContext, arg_pParams)
void uvmswDestruct_IMPL(struct UvmSwObject *pUvmSw);

#define __nvoc_uvmswDestruct(pUvmSw) uvmswDestruct_IMPL(pUvmSw)
void uvmswInitSwMethodState_IMPL(struct UvmSwObject *pUvmSw);

#ifdef __nvoc_uvm_sw_h_disabled
static inline void uvmswInitSwMethodState(struct UvmSwObject *pUvmSw) {
    NV_ASSERT_FAILED_PRECOMP("UvmSwObject was disabled!");
}
#else //__nvoc_uvm_sw_h_disabled
#define uvmswInitSwMethodState(pUvmSw) uvmswInitSwMethodState_IMPL(pUvmSw)
#endif //__nvoc_uvm_sw_h_disabled

#undef PRIVATE_FIELD


#endif // UVM_SW_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_UVM_SW_NVOC_H_
