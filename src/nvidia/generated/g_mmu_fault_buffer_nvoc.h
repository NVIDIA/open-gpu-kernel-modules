
#ifndef _G_MMU_FAULT_BUFFER_NVOC_H_
#define _G_MMU_FAULT_BUFFER_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2010-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_mmu_fault_buffer_nvoc.h"

#ifndef MMU_FAULT_BUFFER_H
#define MMU_FAULT_BUFFER_H

#include "gpu/gpu_resource.h"
#include "rmapi/event_api.h"
#include "ctrl/ctrlb069.h"
#include "ctrl/ctrlc369.h"
#include "rmapi/control.h" // for macro RMCTRL_EXPORT etc.

/*!
 * RM internal class representing XXX_FAULT_BUFFER
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_MMU_FAULT_BUFFER_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__MmuFaultBuffer;
struct NVOC_METADATA__GpuResource;
struct NVOC_METADATA__Notifier;


struct MmuFaultBuffer {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__MmuFaultBuffer *__nvoc_metadata_ptr;
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
    struct MmuFaultBuffer *__nvoc_pbase_MmuFaultBuffer;    // faultbuf
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__MmuFaultBuffer {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResource metadata__GpuResource;
    const struct NVOC_METADATA__Notifier metadata__Notifier;
};

#ifndef __nvoc_class_id_MmuFaultBuffer
#define __nvoc_class_id_MmuFaultBuffer 0x7e1829u
typedef struct MmuFaultBuffer MmuFaultBuffer;
#endif /* __nvoc_class_id_MmuFaultBuffer */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_MmuFaultBuffer;

#define __staticCast_MmuFaultBuffer(pThis) \
    ((pThis)->__nvoc_pbase_MmuFaultBuffer)

#ifdef __nvoc_mmu_fault_buffer_h_disabled
#define __dynamicCast_MmuFaultBuffer(pThis) ((MmuFaultBuffer*) NULL)
#else //__nvoc_mmu_fault_buffer_h_disabled
#define __dynamicCast_MmuFaultBuffer(pThis) \
    ((MmuFaultBuffer*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(MmuFaultBuffer)))
#endif //__nvoc_mmu_fault_buffer_h_disabled

NV_STATUS __nvoc_objCreateDynamic_MmuFaultBuffer(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_MmuFaultBuffer(MmuFaultBuffer**, Dynamic*, NvU32, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __objCreate_MmuFaultBuffer(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams) \
    __nvoc_objCreate_MmuFaultBuffer((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pCallContext, pParams)


// Wrapper macros for implementation functions
NV_STATUS faultbufConstruct_IMPL(struct MmuFaultBuffer *pMmuFaultBuffer, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __nvoc_faultbufConstruct(pMmuFaultBuffer, pCallContext, pParams) faultbufConstruct_IMPL(pMmuFaultBuffer, pCallContext, pParams)

void faultbufDestruct_IMPL(struct MmuFaultBuffer *pMmuFaultBuffer);
#define __nvoc_faultbufDestruct(pMmuFaultBuffer) faultbufDestruct_IMPL(pMmuFaultBuffer)

NV_STATUS faultbufCtrlCmdFaultbufferGetSize_IMPL(struct MmuFaultBuffer *pMmuFaultBuffer, NVB069_CTRL_FAULTBUFFER_GET_SIZE_PARAMS *pGetParams);
#ifdef __nvoc_mmu_fault_buffer_h_disabled
static inline NV_STATUS faultbufCtrlCmdFaultbufferGetSize(struct MmuFaultBuffer *pMmuFaultBuffer, NVB069_CTRL_FAULTBUFFER_GET_SIZE_PARAMS *pGetParams) {
    NV_ASSERT_FAILED_PRECOMP("MmuFaultBuffer was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mmu_fault_buffer_h_disabled
#define faultbufCtrlCmdFaultbufferGetSize(pMmuFaultBuffer, pGetParams) faultbufCtrlCmdFaultbufferGetSize_IMPL(pMmuFaultBuffer, pGetParams)
#endif // __nvoc_mmu_fault_buffer_h_disabled

NV_STATUS faultbufCtrlCmdFaultbufferGetRegisterMappings_IMPL(struct MmuFaultBuffer *pMmuFaultBuffer, NVB069_CTRL_CMD_FAULTBUFFER_GET_REGISTER_MAPPINGS_PARAMS *pParams);
#ifdef __nvoc_mmu_fault_buffer_h_disabled
static inline NV_STATUS faultbufCtrlCmdFaultbufferGetRegisterMappings(struct MmuFaultBuffer *pMmuFaultBuffer, NVB069_CTRL_CMD_FAULTBUFFER_GET_REGISTER_MAPPINGS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("MmuFaultBuffer was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mmu_fault_buffer_h_disabled
#define faultbufCtrlCmdFaultbufferGetRegisterMappings(pMmuFaultBuffer, pParams) faultbufCtrlCmdFaultbufferGetRegisterMappings_IMPL(pMmuFaultBuffer, pParams)
#endif // __nvoc_mmu_fault_buffer_h_disabled

NV_STATUS faultbufCtrlCmdMmuFaultBufferRegisterNonReplayBuf_IMPL(struct MmuFaultBuffer *pMmuFaultBuffer, NVC369_CTRL_MMU_FAULT_BUFFER_REGISTER_NON_REPLAY_BUF_PARAMS *pParams);
#ifdef __nvoc_mmu_fault_buffer_h_disabled
static inline NV_STATUS faultbufCtrlCmdMmuFaultBufferRegisterNonReplayBuf(struct MmuFaultBuffer *pMmuFaultBuffer, NVC369_CTRL_MMU_FAULT_BUFFER_REGISTER_NON_REPLAY_BUF_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("MmuFaultBuffer was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mmu_fault_buffer_h_disabled
#define faultbufCtrlCmdMmuFaultBufferRegisterNonReplayBuf(pMmuFaultBuffer, pParams) faultbufCtrlCmdMmuFaultBufferRegisterNonReplayBuf_IMPL(pMmuFaultBuffer, pParams)
#endif // __nvoc_mmu_fault_buffer_h_disabled

NV_STATUS faultbufCtrlCmdMmuFaultBufferUnregisterNonReplayBuf_IMPL(struct MmuFaultBuffer *pMmuFaultBuffer, NVC369_CTRL_MMU_FAULT_BUFFER_UNREGISTER_NON_REPLAY_BUF_PARAMS *pParams);
#ifdef __nvoc_mmu_fault_buffer_h_disabled
static inline NV_STATUS faultbufCtrlCmdMmuFaultBufferUnregisterNonReplayBuf(struct MmuFaultBuffer *pMmuFaultBuffer, NVC369_CTRL_MMU_FAULT_BUFFER_UNREGISTER_NON_REPLAY_BUF_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("MmuFaultBuffer was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mmu_fault_buffer_h_disabled
#define faultbufCtrlCmdMmuFaultBufferUnregisterNonReplayBuf(pMmuFaultBuffer, pParams) faultbufCtrlCmdMmuFaultBufferUnregisterNonReplayBuf_IMPL(pMmuFaultBuffer, pParams)
#endif // __nvoc_mmu_fault_buffer_h_disabled

NV_STATUS faultbufCtrlCmdMmuFaultBufferRegisterReplayBuf_IMPL(struct MmuFaultBuffer *pMmuFaultBuffer, NVC369_CTRL_MMU_FAULT_BUFFER_REGISTER_REPLAY_BUF_PARAMS *pParams);
#ifdef __nvoc_mmu_fault_buffer_h_disabled
static inline NV_STATUS faultbufCtrlCmdMmuFaultBufferRegisterReplayBuf(struct MmuFaultBuffer *pMmuFaultBuffer, NVC369_CTRL_MMU_FAULT_BUFFER_REGISTER_REPLAY_BUF_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("MmuFaultBuffer was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mmu_fault_buffer_h_disabled
#define faultbufCtrlCmdMmuFaultBufferRegisterReplayBuf(pMmuFaultBuffer, pParams) faultbufCtrlCmdMmuFaultBufferRegisterReplayBuf_IMPL(pMmuFaultBuffer, pParams)
#endif // __nvoc_mmu_fault_buffer_h_disabled

NV_STATUS faultbufCtrlCmdMmuFaultBufferUnregisterReplayBuf_IMPL(struct MmuFaultBuffer *pMmuFaultBuffer, NVC369_CTRL_MMU_FAULT_BUFFER_UNREGISTER_REPLAY_BUF_PARAMS *pParams);
#ifdef __nvoc_mmu_fault_buffer_h_disabled
static inline NV_STATUS faultbufCtrlCmdMmuFaultBufferUnregisterReplayBuf(struct MmuFaultBuffer *pMmuFaultBuffer, NVC369_CTRL_MMU_FAULT_BUFFER_UNREGISTER_REPLAY_BUF_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("MmuFaultBuffer was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mmu_fault_buffer_h_disabled
#define faultbufCtrlCmdMmuFaultBufferUnregisterReplayBuf(pMmuFaultBuffer, pParams) faultbufCtrlCmdMmuFaultBufferUnregisterReplayBuf_IMPL(pMmuFaultBuffer, pParams)
#endif // __nvoc_mmu_fault_buffer_h_disabled


// Wrapper macros for halified functions
#define faultbufMap_FNPTR(pMmuFaultBuffer) pMmuFaultBuffer->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__
#define faultbufMap(pMmuFaultBuffer, pCallContext, pParams, pCpuMapping) faultbufMap_DISPATCH(pMmuFaultBuffer, pCallContext, pParams, pCpuMapping)
#define faultbufUnmap_FNPTR(pMmuFaultBuffer) pMmuFaultBuffer->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__
#define faultbufUnmap(pMmuFaultBuffer, pCallContext, pCpuMapping) faultbufUnmap_DISPATCH(pMmuFaultBuffer, pCallContext, pCpuMapping)
#define faultbufGetMapAddrSpace_FNPTR(pMmuFaultBuffer) pMmuFaultBuffer->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__
#define faultbufGetMapAddrSpace(pMmuFaultBuffer, pCallContext, mapFlags, pAddrSpace) faultbufGetMapAddrSpace_DISPATCH(pMmuFaultBuffer, pCallContext, mapFlags, pAddrSpace)
#define faultbufControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__
#define faultbufControl(pGpuResource, pCallContext, pParams) faultbufControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define faultbufShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__
#define faultbufShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) faultbufShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define faultbufGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__
#define faultbufGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) faultbufGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define faultbufInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__
#define faultbufInternalControlForward(pGpuResource, command, pParams, size) faultbufInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define faultbufGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__
#define faultbufGetInternalObjectHandle(pGpuResource) faultbufGetInternalObjectHandle_DISPATCH(pGpuResource)
#define faultbufAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__
#define faultbufAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) faultbufAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define faultbufGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define faultbufGetMemInterMapParams(pRmResource, pParams) faultbufGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define faultbufCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define faultbufCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) faultbufCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define faultbufGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define faultbufGetMemoryMappingDescriptor(pRmResource, ppMemDesc) faultbufGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define faultbufControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__
#define faultbufControlSerialization_Prologue(pResource, pCallContext, pParams) faultbufControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define faultbufControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__
#define faultbufControlSerialization_Epilogue(pResource, pCallContext, pParams) faultbufControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define faultbufControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__
#define faultbufControl_Prologue(pResource, pCallContext, pParams) faultbufControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define faultbufControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__
#define faultbufControl_Epilogue(pResource, pCallContext, pParams) faultbufControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define faultbufCanCopy_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define faultbufCanCopy(pResource) faultbufCanCopy_DISPATCH(pResource)
#define faultbufIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define faultbufIsDuplicate(pResource, hMemory, pDuplicate) faultbufIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define faultbufPreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define faultbufPreDestruct(pResource) faultbufPreDestruct_DISPATCH(pResource)
#define faultbufControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define faultbufControlFilter(pResource, pCallContext, pParams) faultbufControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define faultbufIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define faultbufIsPartialUnmapSupported(pResource) faultbufIsPartialUnmapSupported_DISPATCH(pResource)
#define faultbufMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define faultbufMapTo(pResource, pParams) faultbufMapTo_DISPATCH(pResource, pParams)
#define faultbufUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define faultbufUnmapFrom(pResource, pParams) faultbufUnmapFrom_DISPATCH(pResource, pParams)
#define faultbufGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define faultbufGetRefCount(pResource) faultbufGetRefCount_DISPATCH(pResource)
#define faultbufAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define faultbufAddAdditionalDependants(pClient, pResource, pReference) faultbufAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define faultbufGetNotificationListPtr_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetNotificationListPtr__
#define faultbufGetNotificationListPtr(pNotifier) faultbufGetNotificationListPtr_DISPATCH(pNotifier)
#define faultbufGetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetNotificationShare__
#define faultbufGetNotificationShare(pNotifier) faultbufGetNotificationShare_DISPATCH(pNotifier)
#define faultbufSetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifySetNotificationShare__
#define faultbufSetNotificationShare(pNotifier, pNotifShare) faultbufSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define faultbufUnregisterEvent_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyUnregisterEvent__
#define faultbufUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) faultbufUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define faultbufGetOrAllocNotifShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetOrAllocNotifShare__
#define faultbufGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) faultbufGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)

// Dispatch functions
static inline NV_STATUS faultbufMap_DISPATCH(struct MmuFaultBuffer *pMmuFaultBuffer, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pMmuFaultBuffer->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__(staticCast(pMmuFaultBuffer, RsResource), pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS faultbufUnmap_DISPATCH(struct MmuFaultBuffer *pMmuFaultBuffer, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pMmuFaultBuffer->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__(staticCast(pMmuFaultBuffer, RsResource), pCallContext, pCpuMapping);
}

static inline NV_STATUS faultbufGetMapAddrSpace_DISPATCH(struct MmuFaultBuffer *pMmuFaultBuffer, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pMmuFaultBuffer->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__(staticCast(pMmuFaultBuffer, GpuResource), pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS faultbufControl_DISPATCH(struct MmuFaultBuffer *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__(staticCast(pGpuResource, RsResource), pCallContext, pParams);
}

static inline NvBool faultbufShareCallback_DISPATCH(struct MmuFaultBuffer *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__(staticCast(pGpuResource, RsResource), pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS faultbufGetRegBaseOffsetAndSize_DISPATCH(struct MmuFaultBuffer *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__(staticCast(pGpuResource, GpuResource), pGpu, pOffset, pSize);
}

static inline NV_STATUS faultbufInternalControlForward_DISPATCH(struct MmuFaultBuffer *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__(staticCast(pGpuResource, GpuResource), command, pParams, size);
}

static inline NvHandle faultbufGetInternalObjectHandle_DISPATCH(struct MmuFaultBuffer *pGpuResource) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__(staticCast(pGpuResource, GpuResource));
}

static inline NvBool faultbufAccessCallback_DISPATCH(struct MmuFaultBuffer *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__(staticCast(pResource, RsResource), pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS faultbufGetMemInterMapParams_DISPATCH(struct MmuFaultBuffer *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__(staticCast(pRmResource, RmResource), pParams);
}

static inline NV_STATUS faultbufCheckMemInterUnmap_DISPATCH(struct MmuFaultBuffer *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__(staticCast(pRmResource, RmResource), bSubdeviceHandleProvided);
}

static inline NV_STATUS faultbufGetMemoryMappingDescriptor_DISPATCH(struct MmuFaultBuffer *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__(staticCast(pRmResource, RmResource), ppMemDesc);
}

static inline NV_STATUS faultbufControlSerialization_Prologue_DISPATCH(struct MmuFaultBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void faultbufControlSerialization_Epilogue_DISPATCH(struct MmuFaultBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS faultbufControl_Prologue_DISPATCH(struct MmuFaultBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void faultbufControl_Epilogue_DISPATCH(struct MmuFaultBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool faultbufCanCopy_DISPATCH(struct MmuFaultBuffer *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__(staticCast(pResource, RsResource));
}

static inline NV_STATUS faultbufIsDuplicate_DISPATCH(struct MmuFaultBuffer *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__(staticCast(pResource, RsResource), hMemory, pDuplicate);
}

static inline void faultbufPreDestruct_DISPATCH(struct MmuFaultBuffer *pResource) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__(staticCast(pResource, RsResource));
}

static inline NV_STATUS faultbufControlFilter_DISPATCH(struct MmuFaultBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool faultbufIsPartialUnmapSupported_DISPATCH(struct MmuFaultBuffer *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__(staticCast(pResource, RsResource));
}

static inline NV_STATUS faultbufMapTo_DISPATCH(struct MmuFaultBuffer *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__(staticCast(pResource, RsResource), pParams);
}

static inline NV_STATUS faultbufUnmapFrom_DISPATCH(struct MmuFaultBuffer *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__(staticCast(pResource, RsResource), pParams);
}

static inline NvU32 faultbufGetRefCount_DISPATCH(struct MmuFaultBuffer *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__(staticCast(pResource, RsResource));
}

static inline void faultbufAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct MmuFaultBuffer *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__(pClient, staticCast(pResource, RsResource), pReference);
}

static inline PEVENTNOTIFICATION * faultbufGetNotificationListPtr_DISPATCH(struct MmuFaultBuffer *pNotifier) {
    return pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetNotificationListPtr__(staticCast(pNotifier, INotifier));
}

static inline struct NotifShare * faultbufGetNotificationShare_DISPATCH(struct MmuFaultBuffer *pNotifier) {
    return pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetNotificationShare__(staticCast(pNotifier, INotifier));
}

static inline void faultbufSetNotificationShare_DISPATCH(struct MmuFaultBuffer *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifySetNotificationShare__(staticCast(pNotifier, INotifier), pNotifShare);
}

static inline NV_STATUS faultbufUnregisterEvent_DISPATCH(struct MmuFaultBuffer *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyUnregisterEvent__(staticCast(pNotifier, INotifier), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS faultbufGetOrAllocNotifShare_DISPATCH(struct MmuFaultBuffer *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetOrAllocNotifShare__(staticCast(pNotifier, INotifier), hNotifierClient, hNotifierResource, ppNotifShare);
}

// Virtual method declarations and/or inline definitions
NV_STATUS faultbufMap_IMPL(struct MmuFaultBuffer *pMmuFaultBuffer, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);

NV_STATUS faultbufUnmap_IMPL(struct MmuFaultBuffer *pMmuFaultBuffer, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);

NV_STATUS faultbufGetMapAddrSpace_IMPL(struct MmuFaultBuffer *pMmuFaultBuffer, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);

// Exported method declarations and/or inline definitions
NV_STATUS faultbufCtrlCmdFaultbufferGetSize_IMPL(struct MmuFaultBuffer *pMmuFaultBuffer, NVB069_CTRL_FAULTBUFFER_GET_SIZE_PARAMS *pGetParams);

NV_STATUS faultbufCtrlCmdFaultbufferGetRegisterMappings_IMPL(struct MmuFaultBuffer *pMmuFaultBuffer, NVB069_CTRL_CMD_FAULTBUFFER_GET_REGISTER_MAPPINGS_PARAMS *pParams);

NV_STATUS faultbufCtrlCmdMmuFaultBufferRegisterNonReplayBuf_IMPL(struct MmuFaultBuffer *pMmuFaultBuffer, NVC369_CTRL_MMU_FAULT_BUFFER_REGISTER_NON_REPLAY_BUF_PARAMS *pParams);

NV_STATUS faultbufCtrlCmdMmuFaultBufferUnregisterNonReplayBuf_IMPL(struct MmuFaultBuffer *pMmuFaultBuffer, NVC369_CTRL_MMU_FAULT_BUFFER_UNREGISTER_NON_REPLAY_BUF_PARAMS *pParams);

NV_STATUS faultbufCtrlCmdMmuFaultBufferRegisterReplayBuf_IMPL(struct MmuFaultBuffer *pMmuFaultBuffer, NVC369_CTRL_MMU_FAULT_BUFFER_REGISTER_REPLAY_BUF_PARAMS *pParams);

NV_STATUS faultbufCtrlCmdMmuFaultBufferUnregisterReplayBuf_IMPL(struct MmuFaultBuffer *pMmuFaultBuffer, NVC369_CTRL_MMU_FAULT_BUFFER_UNREGISTER_REPLAY_BUF_PARAMS *pParams);

// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


#endif // MMU_FAULT_BUFFER_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_MMU_FAULT_BUFFER_NVOC_H_
