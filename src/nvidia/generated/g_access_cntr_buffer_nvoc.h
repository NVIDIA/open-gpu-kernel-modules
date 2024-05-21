
#ifndef _G_ACCESS_CNTR_BUFFER_NVOC_H_
#define _G_ACCESS_CNTR_BUFFER_NVOC_H_
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
 * SPDX-FileCopyrightText: Copyright (c) 2015-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_access_cntr_buffer_nvoc.h"

#ifndef ACCESS_CNTR_BUFFER_H
#define ACCESS_CNTR_BUFFER_H

#include "gpu/gpu_resource.h"
#include "rmapi/event.h"

#include "ctrl/ctrlc365.h"

/*!
 * RM internal class representing ACCESS_COUNTER_NOTIFY_BUFFER
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_ACCESS_CNTR_BUFFER_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct AccessCounterBuffer {

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
    struct AccessCounterBuffer *__nvoc_pbase_AccessCounterBuffer;    // accesscntr

    // Vtable with 40 per-object function pointers
    NV_STATUS (*__accesscntrMap__)(struct AccessCounterBuffer * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual override (res) base (gpures)
    NV_STATUS (*__accesscntrUnmap__)(struct AccessCounterBuffer * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual override (res) base (gpures)
    NV_STATUS (*__accesscntrGetMapAddrSpace__)(struct AccessCounterBuffer * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual override (gpures) base (gpures)
    NV_STATUS (*__accesscntrCtrlCmdAccessCntrBufferReadGet__)(struct AccessCounterBuffer * /*this*/, NVC365_CTRL_ACCESS_CNTR_BUFFER_READ_GET_PARAMS *);  // exported (id=0xc3650101)
    NV_STATUS (*__accesscntrCtrlCmdAccessCntrBufferReadPut__)(struct AccessCounterBuffer * /*this*/, NVC365_CTRL_ACCESS_CNTR_BUFFER_READ_PUT_PARAMS *);  // exported (id=0xc3650103)
    NV_STATUS (*__accesscntrCtrlCmdAccessCntrBufferWriteGet__)(struct AccessCounterBuffer * /*this*/, NVC365_CTRL_ACCESS_CNTR_BUFFER_WRITE_GET_PARAMS *);  // exported (id=0xc3650102)
    NV_STATUS (*__accesscntrCtrlCmdAccessCntrBufferEnable__)(struct AccessCounterBuffer * /*this*/, NVC365_CTRL_ACCESS_CNTR_BUFFER_ENABLE_PARAMS *);  // exported (id=0xc3650104)
    NV_STATUS (*__accesscntrCtrlCmdAccessCntrBufferGetSize__)(struct AccessCounterBuffer * /*this*/, NVC365_CTRL_ACCESS_CNTR_BUFFER_GET_SIZE_PARAMS *);  // exported (id=0xc3650105)
    NV_STATUS (*__accesscntrCtrlCmdAccessCntrBufferGetRegisterMappings__)(struct AccessCounterBuffer * /*this*/, NVC365_CTRL_ACCESS_CNTR_BUFFER_GET_REGISTER_MAPPINGS_PARAMS *);  // exported (id=0xc3650106)
    NV_STATUS (*__accesscntrCtrlCmdAccessCntrBufferGetFullInfo__)(struct AccessCounterBuffer * /*this*/, NVC365_CTRL_ACCESS_CNTR_BUFFER_GET_FULL_INFO_PARAMS *);  // exported (id=0xc3650107)
    NV_STATUS (*__accesscntrCtrlCmdAccessCntrBufferResetCounters__)(struct AccessCounterBuffer * /*this*/, NVC365_CTRL_ACCESS_CNTR_BUFFER_RESET_COUNTERS_PARAMS *);  // exported (id=0xc3650108)
    NV_STATUS (*__accesscntrCtrlCmdAccessCntrSetConfig__)(struct AccessCounterBuffer * /*this*/, NVC365_CTRL_ACCESS_CNTR_SET_CONFIG_PARAMS *);  // exported (id=0xc3650109)
    NV_STATUS (*__accesscntrCtrlCmdAccessCntrBufferEnableIntr__)(struct AccessCounterBuffer * /*this*/, NVC365_CTRL_ACCESS_CNTR_BUFFER_ENABLE_INTR_PARAMS *);  // exported (id=0xc365010b)
    NV_STATUS (*__accesscntrControl__)(struct AccessCounterBuffer * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (gpures) base (gpures)
    NvBool (*__accesscntrShareCallback__)(struct AccessCounterBuffer * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__accesscntrGetRegBaseOffsetAndSize__)(struct AccessCounterBuffer * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__accesscntrInternalControlForward__)(struct AccessCounterBuffer * /*this*/, NvU32, void *, NvU32);  // virtual inherited (gpures) base (gpures)
    NvHandle (*__accesscntrGetInternalObjectHandle__)(struct AccessCounterBuffer * /*this*/);  // virtual inherited (gpures) base (gpures)
    NvBool (*__accesscntrAccessCallback__)(struct AccessCounterBuffer * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__accesscntrGetMemInterMapParams__)(struct AccessCounterBuffer * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__accesscntrCheckMemInterUnmap__)(struct AccessCounterBuffer * /*this*/, NvBool);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__accesscntrGetMemoryMappingDescriptor__)(struct AccessCounterBuffer * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__accesscntrControlSerialization_Prologue__)(struct AccessCounterBuffer * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__accesscntrControlSerialization_Epilogue__)(struct AccessCounterBuffer * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__accesscntrControl_Prologue__)(struct AccessCounterBuffer * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__accesscntrControl_Epilogue__)(struct AccessCounterBuffer * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NvBool (*__accesscntrCanCopy__)(struct AccessCounterBuffer * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__accesscntrIsDuplicate__)(struct AccessCounterBuffer * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (gpures)
    void (*__accesscntrPreDestruct__)(struct AccessCounterBuffer * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__accesscntrControlFilter__)(struct AccessCounterBuffer * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (gpures)
    NvBool (*__accesscntrIsPartialUnmapSupported__)(struct AccessCounterBuffer * /*this*/);  // inline virtual inherited (res) base (gpures) body
    NV_STATUS (*__accesscntrMapTo__)(struct AccessCounterBuffer * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__accesscntrUnmapFrom__)(struct AccessCounterBuffer * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (gpures)
    NvU32 (*__accesscntrGetRefCount__)(struct AccessCounterBuffer * /*this*/);  // virtual inherited (res) base (gpures)
    void (*__accesscntrAddAdditionalDependants__)(struct RsClient *, struct AccessCounterBuffer * /*this*/, RsResourceRef *);  // virtual inherited (res) base (gpures)
    PEVENTNOTIFICATION * (*__accesscntrGetNotificationListPtr__)(struct AccessCounterBuffer * /*this*/);  // virtual inherited (notify) base (notify)
    struct NotifShare * (*__accesscntrGetNotificationShare__)(struct AccessCounterBuffer * /*this*/);  // virtual inherited (notify) base (notify)
    void (*__accesscntrSetNotificationShare__)(struct AccessCounterBuffer * /*this*/, struct NotifShare *);  // virtual inherited (notify) base (notify)
    NV_STATUS (*__accesscntrUnregisterEvent__)(struct AccessCounterBuffer * /*this*/, NvHandle, NvHandle, NvHandle, NvHandle);  // virtual inherited (notify) base (notify)
    NV_STATUS (*__accesscntrGetOrAllocNotifShare__)(struct AccessCounterBuffer * /*this*/, NvHandle, NvHandle, struct NotifShare **);  // virtual inherited (notify) base (notify)

    // Data members
    struct MEMORY_DESCRIPTOR *pUvmAccessCntrAllocMemDesc;
    NvU32 accessCounterIndex;
};

#ifndef __NVOC_CLASS_AccessCounterBuffer_TYPEDEF__
#define __NVOC_CLASS_AccessCounterBuffer_TYPEDEF__
typedef struct AccessCounterBuffer AccessCounterBuffer;
#endif /* __NVOC_CLASS_AccessCounterBuffer_TYPEDEF__ */

#ifndef __nvoc_class_id_AccessCounterBuffer
#define __nvoc_class_id_AccessCounterBuffer 0x1f0074
#endif /* __nvoc_class_id_AccessCounterBuffer */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_AccessCounterBuffer;

#define __staticCast_AccessCounterBuffer(pThis) \
    ((pThis)->__nvoc_pbase_AccessCounterBuffer)

#ifdef __nvoc_access_cntr_buffer_h_disabled
#define __dynamicCast_AccessCounterBuffer(pThis) ((AccessCounterBuffer*)NULL)
#else //__nvoc_access_cntr_buffer_h_disabled
#define __dynamicCast_AccessCounterBuffer(pThis) \
    ((AccessCounterBuffer*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(AccessCounterBuffer)))
#endif //__nvoc_access_cntr_buffer_h_disabled

NV_STATUS __nvoc_objCreateDynamic_AccessCounterBuffer(AccessCounterBuffer**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_AccessCounterBuffer(AccessCounterBuffer**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_AccessCounterBuffer(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_AccessCounterBuffer((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define accesscntrMap_FNPTR(pAccessCounterBuffer) pAccessCounterBuffer->__accesscntrMap__
#define accesscntrMap(pAccessCounterBuffer, pCallContext, pParams, pCpuMapping) accesscntrMap_DISPATCH(pAccessCounterBuffer, pCallContext, pParams, pCpuMapping)
#define accesscntrUnmap_FNPTR(pAccessCounterBuffer) pAccessCounterBuffer->__accesscntrUnmap__
#define accesscntrUnmap(pAccessCounterBuffer, pCallContext, pCpuMapping) accesscntrUnmap_DISPATCH(pAccessCounterBuffer, pCallContext, pCpuMapping)
#define accesscntrGetMapAddrSpace_FNPTR(pAccessCounterBuffer) pAccessCounterBuffer->__accesscntrGetMapAddrSpace__
#define accesscntrGetMapAddrSpace(pAccessCounterBuffer, pCallContext, mapFlags, pAddrSpace) accesscntrGetMapAddrSpace_DISPATCH(pAccessCounterBuffer, pCallContext, mapFlags, pAddrSpace)
#define accesscntrCtrlCmdAccessCntrBufferReadGet_FNPTR(pAccessCounterBuffer) pAccessCounterBuffer->__accesscntrCtrlCmdAccessCntrBufferReadGet__
#define accesscntrCtrlCmdAccessCntrBufferReadGet(pAccessCounterBuffer, pGetParams) accesscntrCtrlCmdAccessCntrBufferReadGet_DISPATCH(pAccessCounterBuffer, pGetParams)
#define accesscntrCtrlCmdAccessCntrBufferReadPut_FNPTR(pAccessCounterBuffer) pAccessCounterBuffer->__accesscntrCtrlCmdAccessCntrBufferReadPut__
#define accesscntrCtrlCmdAccessCntrBufferReadPut(pAccessCounterBuffer, pParams) accesscntrCtrlCmdAccessCntrBufferReadPut_DISPATCH(pAccessCounterBuffer, pParams)
#define accesscntrCtrlCmdAccessCntrBufferWriteGet_FNPTR(pAccessCounterBuffer) pAccessCounterBuffer->__accesscntrCtrlCmdAccessCntrBufferWriteGet__
#define accesscntrCtrlCmdAccessCntrBufferWriteGet(pAccessCounterBuffer, pGetParams) accesscntrCtrlCmdAccessCntrBufferWriteGet_DISPATCH(pAccessCounterBuffer, pGetParams)
#define accesscntrCtrlCmdAccessCntrBufferEnable_FNPTR(pAccessCounterBuffer) pAccessCounterBuffer->__accesscntrCtrlCmdAccessCntrBufferEnable__
#define accesscntrCtrlCmdAccessCntrBufferEnable(pAccessCounterBuffer, pGetParams) accesscntrCtrlCmdAccessCntrBufferEnable_DISPATCH(pAccessCounterBuffer, pGetParams)
#define accesscntrCtrlCmdAccessCntrBufferGetSize_FNPTR(pAccessCounterBuffer) pAccessCounterBuffer->__accesscntrCtrlCmdAccessCntrBufferGetSize__
#define accesscntrCtrlCmdAccessCntrBufferGetSize(pAccessCounterBuffer, pGetParams) accesscntrCtrlCmdAccessCntrBufferGetSize_DISPATCH(pAccessCounterBuffer, pGetParams)
#define accesscntrCtrlCmdAccessCntrBufferGetRegisterMappings_FNPTR(pAccessCounterBuffer) pAccessCounterBuffer->__accesscntrCtrlCmdAccessCntrBufferGetRegisterMappings__
#define accesscntrCtrlCmdAccessCntrBufferGetRegisterMappings(pAccessCounterBuffer, pParams) accesscntrCtrlCmdAccessCntrBufferGetRegisterMappings_DISPATCH(pAccessCounterBuffer, pParams)
#define accesscntrCtrlCmdAccessCntrBufferGetFullInfo_FNPTR(pAccessCounterBuffer) pAccessCounterBuffer->__accesscntrCtrlCmdAccessCntrBufferGetFullInfo__
#define accesscntrCtrlCmdAccessCntrBufferGetFullInfo(pAccessCounterBuffer, pParams) accesscntrCtrlCmdAccessCntrBufferGetFullInfo_DISPATCH(pAccessCounterBuffer, pParams)
#define accesscntrCtrlCmdAccessCntrBufferResetCounters_FNPTR(pAccessCounterBuffer) pAccessCounterBuffer->__accesscntrCtrlCmdAccessCntrBufferResetCounters__
#define accesscntrCtrlCmdAccessCntrBufferResetCounters(pAccessCounterBuffer, pParams) accesscntrCtrlCmdAccessCntrBufferResetCounters_DISPATCH(pAccessCounterBuffer, pParams)
#define accesscntrCtrlCmdAccessCntrSetConfig_FNPTR(pAccessCounterBuffer) pAccessCounterBuffer->__accesscntrCtrlCmdAccessCntrSetConfig__
#define accesscntrCtrlCmdAccessCntrSetConfig(pAccessCounterBuffer, pParams) accesscntrCtrlCmdAccessCntrSetConfig_DISPATCH(pAccessCounterBuffer, pParams)
#define accesscntrCtrlCmdAccessCntrBufferEnableIntr_FNPTR(pAccessCounterBuffer) pAccessCounterBuffer->__accesscntrCtrlCmdAccessCntrBufferEnableIntr__
#define accesscntrCtrlCmdAccessCntrBufferEnableIntr(pAccessCounterBuffer, pGetParams) accesscntrCtrlCmdAccessCntrBufferEnableIntr_DISPATCH(pAccessCounterBuffer, pGetParams)
#define accesscntrControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresControl__
#define accesscntrControl(pGpuResource, pCallContext, pParams) accesscntrControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define accesscntrShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresShareCallback__
#define accesscntrShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) accesscntrShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define accesscntrGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetRegBaseOffsetAndSize__
#define accesscntrGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) accesscntrGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define accesscntrInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresInternalControlForward__
#define accesscntrInternalControlForward(pGpuResource, command, pParams, size) accesscntrInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define accesscntrGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetInternalObjectHandle__
#define accesscntrGetInternalObjectHandle(pGpuResource) accesscntrGetInternalObjectHandle_DISPATCH(pGpuResource)
#define accesscntrAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresAccessCallback__
#define accesscntrAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) accesscntrAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define accesscntrGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemInterMapParams__
#define accesscntrGetMemInterMapParams(pRmResource, pParams) accesscntrGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define accesscntrCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresCheckMemInterUnmap__
#define accesscntrCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) accesscntrCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define accesscntrGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemoryMappingDescriptor__
#define accesscntrGetMemoryMappingDescriptor(pRmResource, ppMemDesc) accesscntrGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define accesscntrControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Prologue__
#define accesscntrControlSerialization_Prologue(pResource, pCallContext, pParams) accesscntrControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define accesscntrControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Epilogue__
#define accesscntrControlSerialization_Epilogue(pResource, pCallContext, pParams) accesscntrControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define accesscntrControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Prologue__
#define accesscntrControl_Prologue(pResource, pCallContext, pParams) accesscntrControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define accesscntrControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Epilogue__
#define accesscntrControl_Epilogue(pResource, pCallContext, pParams) accesscntrControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define accesscntrCanCopy_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__
#define accesscntrCanCopy(pResource) accesscntrCanCopy_DISPATCH(pResource)
#define accesscntrIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsDuplicate__
#define accesscntrIsDuplicate(pResource, hMemory, pDuplicate) accesscntrIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define accesscntrPreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resPreDestruct__
#define accesscntrPreDestruct(pResource) accesscntrPreDestruct_DISPATCH(pResource)
#define accesscntrControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resControlFilter__
#define accesscntrControlFilter(pResource, pCallContext, pParams) accesscntrControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define accesscntrIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsPartialUnmapSupported__
#define accesscntrIsPartialUnmapSupported(pResource) accesscntrIsPartialUnmapSupported_DISPATCH(pResource)
#define accesscntrMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resMapTo__
#define accesscntrMapTo(pResource, pParams) accesscntrMapTo_DISPATCH(pResource, pParams)
#define accesscntrUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmapFrom__
#define accesscntrUnmapFrom(pResource, pParams) accesscntrUnmapFrom_DISPATCH(pResource, pParams)
#define accesscntrGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resGetRefCount__
#define accesscntrGetRefCount(pResource) accesscntrGetRefCount_DISPATCH(pResource)
#define accesscntrAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resAddAdditionalDependants__
#define accesscntrAddAdditionalDependants(pClient, pResource, pReference) accesscntrAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define accesscntrGetNotificationListPtr_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__notifyGetNotificationListPtr__
#define accesscntrGetNotificationListPtr(pNotifier) accesscntrGetNotificationListPtr_DISPATCH(pNotifier)
#define accesscntrGetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__notifyGetNotificationShare__
#define accesscntrGetNotificationShare(pNotifier) accesscntrGetNotificationShare_DISPATCH(pNotifier)
#define accesscntrSetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__notifySetNotificationShare__
#define accesscntrSetNotificationShare(pNotifier, pNotifShare) accesscntrSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define accesscntrUnregisterEvent_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__notifyUnregisterEvent__
#define accesscntrUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) accesscntrUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define accesscntrGetOrAllocNotifShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__notifyGetOrAllocNotifShare__
#define accesscntrGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) accesscntrGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)

// Dispatch functions
static inline NV_STATUS accesscntrMap_DISPATCH(struct AccessCounterBuffer *pAccessCounterBuffer, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pAccessCounterBuffer->__accesscntrMap__(pAccessCounterBuffer, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS accesscntrUnmap_DISPATCH(struct AccessCounterBuffer *pAccessCounterBuffer, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pAccessCounterBuffer->__accesscntrUnmap__(pAccessCounterBuffer, pCallContext, pCpuMapping);
}

static inline NV_STATUS accesscntrGetMapAddrSpace_DISPATCH(struct AccessCounterBuffer *pAccessCounterBuffer, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pAccessCounterBuffer->__accesscntrGetMapAddrSpace__(pAccessCounterBuffer, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS accesscntrCtrlCmdAccessCntrBufferReadGet_DISPATCH(struct AccessCounterBuffer *pAccessCounterBuffer, NVC365_CTRL_ACCESS_CNTR_BUFFER_READ_GET_PARAMS *pGetParams) {
    return pAccessCounterBuffer->__accesscntrCtrlCmdAccessCntrBufferReadGet__(pAccessCounterBuffer, pGetParams);
}

static inline NV_STATUS accesscntrCtrlCmdAccessCntrBufferReadPut_DISPATCH(struct AccessCounterBuffer *pAccessCounterBuffer, NVC365_CTRL_ACCESS_CNTR_BUFFER_READ_PUT_PARAMS *pParams) {
    return pAccessCounterBuffer->__accesscntrCtrlCmdAccessCntrBufferReadPut__(pAccessCounterBuffer, pParams);
}

static inline NV_STATUS accesscntrCtrlCmdAccessCntrBufferWriteGet_DISPATCH(struct AccessCounterBuffer *pAccessCounterBuffer, NVC365_CTRL_ACCESS_CNTR_BUFFER_WRITE_GET_PARAMS *pGetParams) {
    return pAccessCounterBuffer->__accesscntrCtrlCmdAccessCntrBufferWriteGet__(pAccessCounterBuffer, pGetParams);
}

static inline NV_STATUS accesscntrCtrlCmdAccessCntrBufferEnable_DISPATCH(struct AccessCounterBuffer *pAccessCounterBuffer, NVC365_CTRL_ACCESS_CNTR_BUFFER_ENABLE_PARAMS *pGetParams) {
    return pAccessCounterBuffer->__accesscntrCtrlCmdAccessCntrBufferEnable__(pAccessCounterBuffer, pGetParams);
}

static inline NV_STATUS accesscntrCtrlCmdAccessCntrBufferGetSize_DISPATCH(struct AccessCounterBuffer *pAccessCounterBuffer, NVC365_CTRL_ACCESS_CNTR_BUFFER_GET_SIZE_PARAMS *pGetParams) {
    return pAccessCounterBuffer->__accesscntrCtrlCmdAccessCntrBufferGetSize__(pAccessCounterBuffer, pGetParams);
}

static inline NV_STATUS accesscntrCtrlCmdAccessCntrBufferGetRegisterMappings_DISPATCH(struct AccessCounterBuffer *pAccessCounterBuffer, NVC365_CTRL_ACCESS_CNTR_BUFFER_GET_REGISTER_MAPPINGS_PARAMS *pParams) {
    return pAccessCounterBuffer->__accesscntrCtrlCmdAccessCntrBufferGetRegisterMappings__(pAccessCounterBuffer, pParams);
}

static inline NV_STATUS accesscntrCtrlCmdAccessCntrBufferGetFullInfo_DISPATCH(struct AccessCounterBuffer *pAccessCounterBuffer, NVC365_CTRL_ACCESS_CNTR_BUFFER_GET_FULL_INFO_PARAMS *pParams) {
    return pAccessCounterBuffer->__accesscntrCtrlCmdAccessCntrBufferGetFullInfo__(pAccessCounterBuffer, pParams);
}

static inline NV_STATUS accesscntrCtrlCmdAccessCntrBufferResetCounters_DISPATCH(struct AccessCounterBuffer *pAccessCounterBuffer, NVC365_CTRL_ACCESS_CNTR_BUFFER_RESET_COUNTERS_PARAMS *pParams) {
    return pAccessCounterBuffer->__accesscntrCtrlCmdAccessCntrBufferResetCounters__(pAccessCounterBuffer, pParams);
}

static inline NV_STATUS accesscntrCtrlCmdAccessCntrSetConfig_DISPATCH(struct AccessCounterBuffer *pAccessCounterBuffer, NVC365_CTRL_ACCESS_CNTR_SET_CONFIG_PARAMS *pParams) {
    return pAccessCounterBuffer->__accesscntrCtrlCmdAccessCntrSetConfig__(pAccessCounterBuffer, pParams);
}

static inline NV_STATUS accesscntrCtrlCmdAccessCntrBufferEnableIntr_DISPATCH(struct AccessCounterBuffer *pAccessCounterBuffer, NVC365_CTRL_ACCESS_CNTR_BUFFER_ENABLE_INTR_PARAMS *pGetParams) {
    return pAccessCounterBuffer->__accesscntrCtrlCmdAccessCntrBufferEnableIntr__(pAccessCounterBuffer, pGetParams);
}

static inline NV_STATUS accesscntrControl_DISPATCH(struct AccessCounterBuffer *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__accesscntrControl__(pGpuResource, pCallContext, pParams);
}

static inline NvBool accesscntrShareCallback_DISPATCH(struct AccessCounterBuffer *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__accesscntrShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS accesscntrGetRegBaseOffsetAndSize_DISPATCH(struct AccessCounterBuffer *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__accesscntrGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS accesscntrInternalControlForward_DISPATCH(struct AccessCounterBuffer *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__accesscntrInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NvHandle accesscntrGetInternalObjectHandle_DISPATCH(struct AccessCounterBuffer *pGpuResource) {
    return pGpuResource->__accesscntrGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool accesscntrAccessCallback_DISPATCH(struct AccessCounterBuffer *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__accesscntrAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS accesscntrGetMemInterMapParams_DISPATCH(struct AccessCounterBuffer *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__accesscntrGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS accesscntrCheckMemInterUnmap_DISPATCH(struct AccessCounterBuffer *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__accesscntrCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS accesscntrGetMemoryMappingDescriptor_DISPATCH(struct AccessCounterBuffer *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__accesscntrGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS accesscntrControlSerialization_Prologue_DISPATCH(struct AccessCounterBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__accesscntrControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void accesscntrControlSerialization_Epilogue_DISPATCH(struct AccessCounterBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__accesscntrControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS accesscntrControl_Prologue_DISPATCH(struct AccessCounterBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__accesscntrControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void accesscntrControl_Epilogue_DISPATCH(struct AccessCounterBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__accesscntrControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool accesscntrCanCopy_DISPATCH(struct AccessCounterBuffer *pResource) {
    return pResource->__accesscntrCanCopy__(pResource);
}

static inline NV_STATUS accesscntrIsDuplicate_DISPATCH(struct AccessCounterBuffer *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__accesscntrIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void accesscntrPreDestruct_DISPATCH(struct AccessCounterBuffer *pResource) {
    pResource->__accesscntrPreDestruct__(pResource);
}

static inline NV_STATUS accesscntrControlFilter_DISPATCH(struct AccessCounterBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__accesscntrControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool accesscntrIsPartialUnmapSupported_DISPATCH(struct AccessCounterBuffer *pResource) {
    return pResource->__accesscntrIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS accesscntrMapTo_DISPATCH(struct AccessCounterBuffer *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__accesscntrMapTo__(pResource, pParams);
}

static inline NV_STATUS accesscntrUnmapFrom_DISPATCH(struct AccessCounterBuffer *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__accesscntrUnmapFrom__(pResource, pParams);
}

static inline NvU32 accesscntrGetRefCount_DISPATCH(struct AccessCounterBuffer *pResource) {
    return pResource->__accesscntrGetRefCount__(pResource);
}

static inline void accesscntrAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct AccessCounterBuffer *pResource, RsResourceRef *pReference) {
    pResource->__accesscntrAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline PEVENTNOTIFICATION * accesscntrGetNotificationListPtr_DISPATCH(struct AccessCounterBuffer *pNotifier) {
    return pNotifier->__accesscntrGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare * accesscntrGetNotificationShare_DISPATCH(struct AccessCounterBuffer *pNotifier) {
    return pNotifier->__accesscntrGetNotificationShare__(pNotifier);
}

static inline void accesscntrSetNotificationShare_DISPATCH(struct AccessCounterBuffer *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__accesscntrSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NV_STATUS accesscntrUnregisterEvent_DISPATCH(struct AccessCounterBuffer *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__accesscntrUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS accesscntrGetOrAllocNotifShare_DISPATCH(struct AccessCounterBuffer *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__accesscntrGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

NV_STATUS accesscntrMap_IMPL(struct AccessCounterBuffer *pAccessCounterBuffer, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);

NV_STATUS accesscntrUnmap_IMPL(struct AccessCounterBuffer *pAccessCounterBuffer, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);

NV_STATUS accesscntrGetMapAddrSpace_IMPL(struct AccessCounterBuffer *pAccessCounterBuffer, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);

NV_STATUS accesscntrCtrlCmdAccessCntrBufferReadGet_IMPL(struct AccessCounterBuffer *pAccessCounterBuffer, NVC365_CTRL_ACCESS_CNTR_BUFFER_READ_GET_PARAMS *pGetParams);

NV_STATUS accesscntrCtrlCmdAccessCntrBufferReadPut_IMPL(struct AccessCounterBuffer *pAccessCounterBuffer, NVC365_CTRL_ACCESS_CNTR_BUFFER_READ_PUT_PARAMS *pParams);

NV_STATUS accesscntrCtrlCmdAccessCntrBufferWriteGet_IMPL(struct AccessCounterBuffer *pAccessCounterBuffer, NVC365_CTRL_ACCESS_CNTR_BUFFER_WRITE_GET_PARAMS *pGetParams);

NV_STATUS accesscntrCtrlCmdAccessCntrBufferEnable_IMPL(struct AccessCounterBuffer *pAccessCounterBuffer, NVC365_CTRL_ACCESS_CNTR_BUFFER_ENABLE_PARAMS *pGetParams);

NV_STATUS accesscntrCtrlCmdAccessCntrBufferGetSize_IMPL(struct AccessCounterBuffer *pAccessCounterBuffer, NVC365_CTRL_ACCESS_CNTR_BUFFER_GET_SIZE_PARAMS *pGetParams);

NV_STATUS accesscntrCtrlCmdAccessCntrBufferGetRegisterMappings_IMPL(struct AccessCounterBuffer *pAccessCounterBuffer, NVC365_CTRL_ACCESS_CNTR_BUFFER_GET_REGISTER_MAPPINGS_PARAMS *pParams);

NV_STATUS accesscntrCtrlCmdAccessCntrBufferGetFullInfo_IMPL(struct AccessCounterBuffer *pAccessCounterBuffer, NVC365_CTRL_ACCESS_CNTR_BUFFER_GET_FULL_INFO_PARAMS *pParams);

NV_STATUS accesscntrCtrlCmdAccessCntrBufferResetCounters_IMPL(struct AccessCounterBuffer *pAccessCounterBuffer, NVC365_CTRL_ACCESS_CNTR_BUFFER_RESET_COUNTERS_PARAMS *pParams);

NV_STATUS accesscntrCtrlCmdAccessCntrSetConfig_IMPL(struct AccessCounterBuffer *pAccessCounterBuffer, NVC365_CTRL_ACCESS_CNTR_SET_CONFIG_PARAMS *pParams);

NV_STATUS accesscntrCtrlCmdAccessCntrBufferEnableIntr_IMPL(struct AccessCounterBuffer *pAccessCounterBuffer, NVC365_CTRL_ACCESS_CNTR_BUFFER_ENABLE_INTR_PARAMS *pGetParams);

NV_STATUS accesscntrConstruct_IMPL(struct AccessCounterBuffer *arg_pAccessCounterBuffer, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_accesscntrConstruct(arg_pAccessCounterBuffer, arg_pCallContext, arg_pParams) accesscntrConstruct_IMPL(arg_pAccessCounterBuffer, arg_pCallContext, arg_pParams)
void accesscntrDestruct_IMPL(struct AccessCounterBuffer *pAccessCounterBuffer);

#define __nvoc_accesscntrDestruct(pAccessCounterBuffer) accesscntrDestruct_IMPL(pAccessCounterBuffer)
#undef PRIVATE_FIELD


#endif // ACCESS_CNTR_BUFFER_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_ACCESS_CNTR_BUFFER_NVOC_H_
