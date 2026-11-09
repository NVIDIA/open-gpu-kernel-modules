
#ifndef _G_ACCESS_CNTR_BUFFER_NVOC_H_
#define _G_ACCESS_CNTR_BUFFER_NVOC_H_

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
#include "rmapi/event_api.h"

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


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__AccessCounterBuffer;
struct NVOC_METADATA__GpuResource;
struct NVOC_METADATA__Notifier;


struct AccessCounterBuffer {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__AccessCounterBuffer *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
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

    // Data members
    struct MEMORY_DESCRIPTOR *pUvmAccessCntrAllocMemDesc;
    NvU32 accessCounterIndex;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__AccessCounterBuffer {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResource metadata__GpuResource;
    const struct NVOC_METADATA__Notifier metadata__Notifier;
};

#ifndef __nvoc_class_id_AccessCounterBuffer
#define __nvoc_class_id_AccessCounterBuffer 0x1f0074u
typedef struct AccessCounterBuffer AccessCounterBuffer;
#endif /* __nvoc_class_id_AccessCounterBuffer */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_AccessCounterBuffer;

#define __staticCast_AccessCounterBuffer(pThis) \
    ((pThis)->__nvoc_pbase_AccessCounterBuffer)

#ifdef __nvoc_access_cntr_buffer_h_disabled
#define __dynamicCast_AccessCounterBuffer(pThis) ((AccessCounterBuffer*) NULL)
#else //__nvoc_access_cntr_buffer_h_disabled
#define __dynamicCast_AccessCounterBuffer(pThis) \
    ((AccessCounterBuffer*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(AccessCounterBuffer)))
#endif //__nvoc_access_cntr_buffer_h_disabled

NV_STATUS __nvoc_objCreateDynamic_AccessCounterBuffer(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_AccessCounterBuffer(AccessCounterBuffer**, Dynamic*, NvU32, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __objCreate_AccessCounterBuffer(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams) \
    __nvoc_objCreate_AccessCounterBuffer((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pCallContext, pParams)


// Wrapper macros for implementation functions
NV_STATUS accesscntrConstruct_IMPL(struct AccessCounterBuffer *pAccessCounterBuffer, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __nvoc_accesscntrConstruct(pAccessCounterBuffer, pCallContext, pParams) accesscntrConstruct_IMPL(pAccessCounterBuffer, pCallContext, pParams)

void accesscntrDestruct_IMPL(struct AccessCounterBuffer *pAccessCounterBuffer);
#define __nvoc_accesscntrDestruct(pAccessCounterBuffer) accesscntrDestruct_IMPL(pAccessCounterBuffer)

NV_STATUS accesscntrCtrlCmdAccessCntrBufferReadGet_IMPL(struct AccessCounterBuffer *pAccessCounterBuffer, NVC365_CTRL_ACCESS_CNTR_BUFFER_READ_GET_PARAMS *pGetParams);
#ifdef __nvoc_access_cntr_buffer_h_disabled
static inline NV_STATUS accesscntrCtrlCmdAccessCntrBufferReadGet(struct AccessCounterBuffer *pAccessCounterBuffer, NVC365_CTRL_ACCESS_CNTR_BUFFER_READ_GET_PARAMS *pGetParams) {
    NV_ASSERT_FAILED_PRECOMP("AccessCounterBuffer was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_access_cntr_buffer_h_disabled
#define accesscntrCtrlCmdAccessCntrBufferReadGet(pAccessCounterBuffer, pGetParams) accesscntrCtrlCmdAccessCntrBufferReadGet_IMPL(pAccessCounterBuffer, pGetParams)
#endif // __nvoc_access_cntr_buffer_h_disabled

NV_STATUS accesscntrCtrlCmdAccessCntrBufferReadPut_IMPL(struct AccessCounterBuffer *pAccessCounterBuffer, NVC365_CTRL_ACCESS_CNTR_BUFFER_READ_PUT_PARAMS *pParams);
#ifdef __nvoc_access_cntr_buffer_h_disabled
static inline NV_STATUS accesscntrCtrlCmdAccessCntrBufferReadPut(struct AccessCounterBuffer *pAccessCounterBuffer, NVC365_CTRL_ACCESS_CNTR_BUFFER_READ_PUT_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("AccessCounterBuffer was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_access_cntr_buffer_h_disabled
#define accesscntrCtrlCmdAccessCntrBufferReadPut(pAccessCounterBuffer, pParams) accesscntrCtrlCmdAccessCntrBufferReadPut_IMPL(pAccessCounterBuffer, pParams)
#endif // __nvoc_access_cntr_buffer_h_disabled

NV_STATUS accesscntrCtrlCmdAccessCntrBufferWriteGet_IMPL(struct AccessCounterBuffer *pAccessCounterBuffer, NVC365_CTRL_ACCESS_CNTR_BUFFER_WRITE_GET_PARAMS *pGetParams);
#ifdef __nvoc_access_cntr_buffer_h_disabled
static inline NV_STATUS accesscntrCtrlCmdAccessCntrBufferWriteGet(struct AccessCounterBuffer *pAccessCounterBuffer, NVC365_CTRL_ACCESS_CNTR_BUFFER_WRITE_GET_PARAMS *pGetParams) {
    NV_ASSERT_FAILED_PRECOMP("AccessCounterBuffer was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_access_cntr_buffer_h_disabled
#define accesscntrCtrlCmdAccessCntrBufferWriteGet(pAccessCounterBuffer, pGetParams) accesscntrCtrlCmdAccessCntrBufferWriteGet_IMPL(pAccessCounterBuffer, pGetParams)
#endif // __nvoc_access_cntr_buffer_h_disabled

NV_STATUS accesscntrCtrlCmdAccessCntrBufferEnable_IMPL(struct AccessCounterBuffer *pAccessCounterBuffer, NVC365_CTRL_ACCESS_CNTR_BUFFER_ENABLE_PARAMS *pGetParams);
#ifdef __nvoc_access_cntr_buffer_h_disabled
static inline NV_STATUS accesscntrCtrlCmdAccessCntrBufferEnable(struct AccessCounterBuffer *pAccessCounterBuffer, NVC365_CTRL_ACCESS_CNTR_BUFFER_ENABLE_PARAMS *pGetParams) {
    NV_ASSERT_FAILED_PRECOMP("AccessCounterBuffer was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_access_cntr_buffer_h_disabled
#define accesscntrCtrlCmdAccessCntrBufferEnable(pAccessCounterBuffer, pGetParams) accesscntrCtrlCmdAccessCntrBufferEnable_IMPL(pAccessCounterBuffer, pGetParams)
#endif // __nvoc_access_cntr_buffer_h_disabled

NV_STATUS accesscntrCtrlCmdAccessCntrBufferGetSize_IMPL(struct AccessCounterBuffer *pAccessCounterBuffer, NVC365_CTRL_ACCESS_CNTR_BUFFER_GET_SIZE_PARAMS *pGetParams);
#ifdef __nvoc_access_cntr_buffer_h_disabled
static inline NV_STATUS accesscntrCtrlCmdAccessCntrBufferGetSize(struct AccessCounterBuffer *pAccessCounterBuffer, NVC365_CTRL_ACCESS_CNTR_BUFFER_GET_SIZE_PARAMS *pGetParams) {
    NV_ASSERT_FAILED_PRECOMP("AccessCounterBuffer was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_access_cntr_buffer_h_disabled
#define accesscntrCtrlCmdAccessCntrBufferGetSize(pAccessCounterBuffer, pGetParams) accesscntrCtrlCmdAccessCntrBufferGetSize_IMPL(pAccessCounterBuffer, pGetParams)
#endif // __nvoc_access_cntr_buffer_h_disabled

NV_STATUS accesscntrCtrlCmdAccessCntrBufferGetRegisterMappings_IMPL(struct AccessCounterBuffer *pAccessCounterBuffer, NVC365_CTRL_ACCESS_CNTR_BUFFER_GET_REGISTER_MAPPINGS_PARAMS *pParams);
#ifdef __nvoc_access_cntr_buffer_h_disabled
static inline NV_STATUS accesscntrCtrlCmdAccessCntrBufferGetRegisterMappings(struct AccessCounterBuffer *pAccessCounterBuffer, NVC365_CTRL_ACCESS_CNTR_BUFFER_GET_REGISTER_MAPPINGS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("AccessCounterBuffer was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_access_cntr_buffer_h_disabled
#define accesscntrCtrlCmdAccessCntrBufferGetRegisterMappings(pAccessCounterBuffer, pParams) accesscntrCtrlCmdAccessCntrBufferGetRegisterMappings_IMPL(pAccessCounterBuffer, pParams)
#endif // __nvoc_access_cntr_buffer_h_disabled

NV_STATUS accesscntrCtrlCmdAccessCntrBufferGetFullInfo_IMPL(struct AccessCounterBuffer *pAccessCounterBuffer, NVC365_CTRL_ACCESS_CNTR_BUFFER_GET_FULL_INFO_PARAMS *pParams);
#ifdef __nvoc_access_cntr_buffer_h_disabled
static inline NV_STATUS accesscntrCtrlCmdAccessCntrBufferGetFullInfo(struct AccessCounterBuffer *pAccessCounterBuffer, NVC365_CTRL_ACCESS_CNTR_BUFFER_GET_FULL_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("AccessCounterBuffer was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_access_cntr_buffer_h_disabled
#define accesscntrCtrlCmdAccessCntrBufferGetFullInfo(pAccessCounterBuffer, pParams) accesscntrCtrlCmdAccessCntrBufferGetFullInfo_IMPL(pAccessCounterBuffer, pParams)
#endif // __nvoc_access_cntr_buffer_h_disabled

NV_STATUS accesscntrCtrlCmdAccessCntrBufferResetCounters_IMPL(struct AccessCounterBuffer *pAccessCounterBuffer, NVC365_CTRL_ACCESS_CNTR_BUFFER_RESET_COUNTERS_PARAMS *pParams);
#ifdef __nvoc_access_cntr_buffer_h_disabled
static inline NV_STATUS accesscntrCtrlCmdAccessCntrBufferResetCounters(struct AccessCounterBuffer *pAccessCounterBuffer, NVC365_CTRL_ACCESS_CNTR_BUFFER_RESET_COUNTERS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("AccessCounterBuffer was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_access_cntr_buffer_h_disabled
#define accesscntrCtrlCmdAccessCntrBufferResetCounters(pAccessCounterBuffer, pParams) accesscntrCtrlCmdAccessCntrBufferResetCounters_IMPL(pAccessCounterBuffer, pParams)
#endif // __nvoc_access_cntr_buffer_h_disabled

NV_STATUS accesscntrCtrlCmdAccessCntrSetConfig_IMPL(struct AccessCounterBuffer *pAccessCounterBuffer, NVC365_CTRL_ACCESS_CNTR_SET_CONFIG_PARAMS *pParams);
#ifdef __nvoc_access_cntr_buffer_h_disabled
static inline NV_STATUS accesscntrCtrlCmdAccessCntrSetConfig(struct AccessCounterBuffer *pAccessCounterBuffer, NVC365_CTRL_ACCESS_CNTR_SET_CONFIG_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("AccessCounterBuffer was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_access_cntr_buffer_h_disabled
#define accesscntrCtrlCmdAccessCntrSetConfig(pAccessCounterBuffer, pParams) accesscntrCtrlCmdAccessCntrSetConfig_IMPL(pAccessCounterBuffer, pParams)
#endif // __nvoc_access_cntr_buffer_h_disabled

NV_STATUS accesscntrCtrlCmdAccessCntrBufferEnableIntr_IMPL(struct AccessCounterBuffer *pAccessCounterBuffer, NVC365_CTRL_ACCESS_CNTR_BUFFER_ENABLE_INTR_PARAMS *pGetParams);
#ifdef __nvoc_access_cntr_buffer_h_disabled
static inline NV_STATUS accesscntrCtrlCmdAccessCntrBufferEnableIntr(struct AccessCounterBuffer *pAccessCounterBuffer, NVC365_CTRL_ACCESS_CNTR_BUFFER_ENABLE_INTR_PARAMS *pGetParams) {
    NV_ASSERT_FAILED_PRECOMP("AccessCounterBuffer was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_access_cntr_buffer_h_disabled
#define accesscntrCtrlCmdAccessCntrBufferEnableIntr(pAccessCounterBuffer, pGetParams) accesscntrCtrlCmdAccessCntrBufferEnableIntr_IMPL(pAccessCounterBuffer, pGetParams)
#endif // __nvoc_access_cntr_buffer_h_disabled


// Wrapper macros for halified functions
#define accesscntrMap_FNPTR(pAccessCounterBuffer) pAccessCounterBuffer->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__
#define accesscntrMap(pAccessCounterBuffer, pCallContext, pParams, pCpuMapping) accesscntrMap_DISPATCH(pAccessCounterBuffer, pCallContext, pParams, pCpuMapping)
#define accesscntrUnmap_FNPTR(pAccessCounterBuffer) pAccessCounterBuffer->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__
#define accesscntrUnmap(pAccessCounterBuffer, pCallContext, pCpuMapping) accesscntrUnmap_DISPATCH(pAccessCounterBuffer, pCallContext, pCpuMapping)
#define accesscntrGetMapAddrSpace_FNPTR(pAccessCounterBuffer) pAccessCounterBuffer->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__
#define accesscntrGetMapAddrSpace(pAccessCounterBuffer, pCallContext, mapFlags, pAddrSpace) accesscntrGetMapAddrSpace_DISPATCH(pAccessCounterBuffer, pCallContext, mapFlags, pAddrSpace)
#define accesscntrControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__
#define accesscntrControl(pGpuResource, pCallContext, pParams) accesscntrControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define accesscntrShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__
#define accesscntrShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) accesscntrShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define accesscntrGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__
#define accesscntrGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) accesscntrGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define accesscntrInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__
#define accesscntrInternalControlForward(pGpuResource, command, pParams, size) accesscntrInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define accesscntrGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__
#define accesscntrGetInternalObjectHandle(pGpuResource) accesscntrGetInternalObjectHandle_DISPATCH(pGpuResource)
#define accesscntrAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__
#define accesscntrAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) accesscntrAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define accesscntrGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define accesscntrGetMemInterMapParams(pRmResource, pParams) accesscntrGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define accesscntrCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define accesscntrCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) accesscntrCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define accesscntrGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define accesscntrGetMemoryMappingDescriptor(pRmResource, ppMemDesc) accesscntrGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define accesscntrControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__
#define accesscntrControlSerialization_Prologue(pResource, pCallContext, pParams) accesscntrControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define accesscntrControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__
#define accesscntrControlSerialization_Epilogue(pResource, pCallContext, pParams) accesscntrControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define accesscntrControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__
#define accesscntrControl_Prologue(pResource, pCallContext, pParams) accesscntrControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define accesscntrControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__
#define accesscntrControl_Epilogue(pResource, pCallContext, pParams) accesscntrControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define accesscntrCanCopy_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define accesscntrCanCopy(pResource) accesscntrCanCopy_DISPATCH(pResource)
#define accesscntrIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define accesscntrIsDuplicate(pResource, hMemory, pDuplicate) accesscntrIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define accesscntrPreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define accesscntrPreDestruct(pResource) accesscntrPreDestruct_DISPATCH(pResource)
#define accesscntrControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define accesscntrControlFilter(pResource, pCallContext, pParams) accesscntrControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define accesscntrIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define accesscntrIsPartialUnmapSupported(pResource) accesscntrIsPartialUnmapSupported_DISPATCH(pResource)
#define accesscntrMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define accesscntrMapTo(pResource, pParams) accesscntrMapTo_DISPATCH(pResource, pParams)
#define accesscntrUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define accesscntrUnmapFrom(pResource, pParams) accesscntrUnmapFrom_DISPATCH(pResource, pParams)
#define accesscntrGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define accesscntrGetRefCount(pResource) accesscntrGetRefCount_DISPATCH(pResource)
#define accesscntrAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define accesscntrAddAdditionalDependants(pClient, pResource, pReference) accesscntrAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define accesscntrGetNotificationListPtr_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetNotificationListPtr__
#define accesscntrGetNotificationListPtr(pNotifier) accesscntrGetNotificationListPtr_DISPATCH(pNotifier)
#define accesscntrGetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetNotificationShare__
#define accesscntrGetNotificationShare(pNotifier) accesscntrGetNotificationShare_DISPATCH(pNotifier)
#define accesscntrSetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifySetNotificationShare__
#define accesscntrSetNotificationShare(pNotifier, pNotifShare) accesscntrSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define accesscntrUnregisterEvent_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyUnregisterEvent__
#define accesscntrUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) accesscntrUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define accesscntrGetOrAllocNotifShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetOrAllocNotifShare__
#define accesscntrGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) accesscntrGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)

// Dispatch functions
static inline NV_STATUS accesscntrMap_DISPATCH(struct AccessCounterBuffer *pAccessCounterBuffer, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pAccessCounterBuffer->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__(staticCast(pAccessCounterBuffer, RsResource), pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS accesscntrUnmap_DISPATCH(struct AccessCounterBuffer *pAccessCounterBuffer, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pAccessCounterBuffer->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__(staticCast(pAccessCounterBuffer, RsResource), pCallContext, pCpuMapping);
}

static inline NV_STATUS accesscntrGetMapAddrSpace_DISPATCH(struct AccessCounterBuffer *pAccessCounterBuffer, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pAccessCounterBuffer->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__(staticCast(pAccessCounterBuffer, GpuResource), pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS accesscntrControl_DISPATCH(struct AccessCounterBuffer *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__(staticCast(pGpuResource, RsResource), pCallContext, pParams);
}

static inline NvBool accesscntrShareCallback_DISPATCH(struct AccessCounterBuffer *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__(staticCast(pGpuResource, RsResource), pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS accesscntrGetRegBaseOffsetAndSize_DISPATCH(struct AccessCounterBuffer *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__(staticCast(pGpuResource, GpuResource), pGpu, pOffset, pSize);
}

static inline NV_STATUS accesscntrInternalControlForward_DISPATCH(struct AccessCounterBuffer *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__(staticCast(pGpuResource, GpuResource), command, pParams, size);
}

static inline NvHandle accesscntrGetInternalObjectHandle_DISPATCH(struct AccessCounterBuffer *pGpuResource) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__(staticCast(pGpuResource, GpuResource));
}

static inline NvBool accesscntrAccessCallback_DISPATCH(struct AccessCounterBuffer *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__(staticCast(pResource, RsResource), pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS accesscntrGetMemInterMapParams_DISPATCH(struct AccessCounterBuffer *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__(staticCast(pRmResource, RmResource), pParams);
}

static inline NV_STATUS accesscntrCheckMemInterUnmap_DISPATCH(struct AccessCounterBuffer *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__(staticCast(pRmResource, RmResource), bSubdeviceHandleProvided);
}

static inline NV_STATUS accesscntrGetMemoryMappingDescriptor_DISPATCH(struct AccessCounterBuffer *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__(staticCast(pRmResource, RmResource), ppMemDesc);
}

static inline NV_STATUS accesscntrControlSerialization_Prologue_DISPATCH(struct AccessCounterBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void accesscntrControlSerialization_Epilogue_DISPATCH(struct AccessCounterBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS accesscntrControl_Prologue_DISPATCH(struct AccessCounterBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void accesscntrControl_Epilogue_DISPATCH(struct AccessCounterBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool accesscntrCanCopy_DISPATCH(struct AccessCounterBuffer *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__(staticCast(pResource, RsResource));
}

static inline NV_STATUS accesscntrIsDuplicate_DISPATCH(struct AccessCounterBuffer *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__(staticCast(pResource, RsResource), hMemory, pDuplicate);
}

static inline void accesscntrPreDestruct_DISPATCH(struct AccessCounterBuffer *pResource) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__(staticCast(pResource, RsResource));
}

static inline NV_STATUS accesscntrControlFilter_DISPATCH(struct AccessCounterBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool accesscntrIsPartialUnmapSupported_DISPATCH(struct AccessCounterBuffer *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__(staticCast(pResource, RsResource));
}

static inline NV_STATUS accesscntrMapTo_DISPATCH(struct AccessCounterBuffer *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__(staticCast(pResource, RsResource), pParams);
}

static inline NV_STATUS accesscntrUnmapFrom_DISPATCH(struct AccessCounterBuffer *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__(staticCast(pResource, RsResource), pParams);
}

static inline NvU32 accesscntrGetRefCount_DISPATCH(struct AccessCounterBuffer *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__(staticCast(pResource, RsResource));
}

static inline void accesscntrAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct AccessCounterBuffer *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__(pClient, staticCast(pResource, RsResource), pReference);
}

static inline PEVENTNOTIFICATION * accesscntrGetNotificationListPtr_DISPATCH(struct AccessCounterBuffer *pNotifier) {
    return pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetNotificationListPtr__(staticCast(pNotifier, INotifier));
}

static inline struct NotifShare * accesscntrGetNotificationShare_DISPATCH(struct AccessCounterBuffer *pNotifier) {
    return pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetNotificationShare__(staticCast(pNotifier, INotifier));
}

static inline void accesscntrSetNotificationShare_DISPATCH(struct AccessCounterBuffer *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifySetNotificationShare__(staticCast(pNotifier, INotifier), pNotifShare);
}

static inline NV_STATUS accesscntrUnregisterEvent_DISPATCH(struct AccessCounterBuffer *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyUnregisterEvent__(staticCast(pNotifier, INotifier), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS accesscntrGetOrAllocNotifShare_DISPATCH(struct AccessCounterBuffer *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetOrAllocNotifShare__(staticCast(pNotifier, INotifier), hNotifierClient, hNotifierResource, ppNotifShare);
}

// Virtual method declarations and/or inline definitions
NV_STATUS accesscntrMap_IMPL(struct AccessCounterBuffer *pAccessCounterBuffer, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);

NV_STATUS accesscntrUnmap_IMPL(struct AccessCounterBuffer *pAccessCounterBuffer, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);

NV_STATUS accesscntrGetMapAddrSpace_IMPL(struct AccessCounterBuffer *pAccessCounterBuffer, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);

// Exported method declarations and/or inline definitions
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

// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


#endif // ACCESS_CNTR_BUFFER_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_ACCESS_CNTR_BUFFER_NVOC_H_
