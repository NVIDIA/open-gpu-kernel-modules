
#ifndef _G_MMU_FAULT_BUFFER_NVOC_H_
#define _G_MMU_FAULT_BUFFER_NVOC_H_
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
 * SPDX-FileCopyrightText: Copyright (c) 2010-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "rmapi/event.h"
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


struct MmuFaultBuffer {

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
    struct MmuFaultBuffer *__nvoc_pbase_MmuFaultBuffer;    // faultbuf

    // Vtable with 36 per-object function pointers
    NV_STATUS (*__faultbufMap__)(struct MmuFaultBuffer * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual override (res) base (gpures)
    NV_STATUS (*__faultbufUnmap__)(struct MmuFaultBuffer * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual override (res) base (gpures)
    NV_STATUS (*__faultbufGetMapAddrSpace__)(struct MmuFaultBuffer * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual override (gpures) base (gpures)
    NV_STATUS (*__faultbufCtrlCmdFaultbufferGetSize__)(struct MmuFaultBuffer * /*this*/, NVB069_CTRL_FAULTBUFFER_GET_SIZE_PARAMS *);  // exported (id=0xb0690105)
    NV_STATUS (*__faultbufCtrlCmdFaultbufferGetRegisterMappings__)(struct MmuFaultBuffer * /*this*/, NVB069_CTRL_CMD_FAULTBUFFER_GET_REGISTER_MAPPINGS_PARAMS *);  // exported (id=0xb0690106)
    NV_STATUS (*__faultbufCtrlCmdMmuFaultBufferRegisterNonReplayBuf__)(struct MmuFaultBuffer * /*this*/, NVC369_CTRL_MMU_FAULT_BUFFER_REGISTER_NON_REPLAY_BUF_PARAMS *);  // exported (id=0xc3690101)
    NV_STATUS (*__faultbufCtrlCmdMmuFaultBufferUnregisterNonReplayBuf__)(struct MmuFaultBuffer * /*this*/, NVC369_CTRL_MMU_FAULT_BUFFER_UNREGISTER_NON_REPLAY_BUF_PARAMS *);  // exported (id=0xc3690102)
    NV_STATUS (*__faultbufCtrlCmdMmuFaultBufferRegisterReplayBuf__)(struct MmuFaultBuffer * /*this*/, NVC369_CTRL_MMU_FAULT_BUFFER_REGISTER_REPLAY_BUF_PARAMS *);  // exported (id=0xc3690103)
    NV_STATUS (*__faultbufCtrlCmdMmuFaultBufferUnregisterReplayBuf__)(struct MmuFaultBuffer * /*this*/, NVC369_CTRL_MMU_FAULT_BUFFER_UNREGISTER_REPLAY_BUF_PARAMS *);  // exported (id=0xc3690104)
    NV_STATUS (*__faultbufControl__)(struct MmuFaultBuffer * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (gpures) base (gpures)
    NvBool (*__faultbufShareCallback__)(struct MmuFaultBuffer * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__faultbufGetRegBaseOffsetAndSize__)(struct MmuFaultBuffer * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__faultbufInternalControlForward__)(struct MmuFaultBuffer * /*this*/, NvU32, void *, NvU32);  // virtual inherited (gpures) base (gpures)
    NvHandle (*__faultbufGetInternalObjectHandle__)(struct MmuFaultBuffer * /*this*/);  // virtual inherited (gpures) base (gpures)
    NvBool (*__faultbufAccessCallback__)(struct MmuFaultBuffer * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__faultbufGetMemInterMapParams__)(struct MmuFaultBuffer * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__faultbufCheckMemInterUnmap__)(struct MmuFaultBuffer * /*this*/, NvBool);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__faultbufGetMemoryMappingDescriptor__)(struct MmuFaultBuffer * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__faultbufControlSerialization_Prologue__)(struct MmuFaultBuffer * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__faultbufControlSerialization_Epilogue__)(struct MmuFaultBuffer * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__faultbufControl_Prologue__)(struct MmuFaultBuffer * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__faultbufControl_Epilogue__)(struct MmuFaultBuffer * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NvBool (*__faultbufCanCopy__)(struct MmuFaultBuffer * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__faultbufIsDuplicate__)(struct MmuFaultBuffer * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (gpures)
    void (*__faultbufPreDestruct__)(struct MmuFaultBuffer * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__faultbufControlFilter__)(struct MmuFaultBuffer * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (gpures)
    NvBool (*__faultbufIsPartialUnmapSupported__)(struct MmuFaultBuffer * /*this*/);  // inline virtual inherited (res) base (gpures) body
    NV_STATUS (*__faultbufMapTo__)(struct MmuFaultBuffer * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__faultbufUnmapFrom__)(struct MmuFaultBuffer * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (gpures)
    NvU32 (*__faultbufGetRefCount__)(struct MmuFaultBuffer * /*this*/);  // virtual inherited (res) base (gpures)
    void (*__faultbufAddAdditionalDependants__)(struct RsClient *, struct MmuFaultBuffer * /*this*/, RsResourceRef *);  // virtual inherited (res) base (gpures)
    PEVENTNOTIFICATION * (*__faultbufGetNotificationListPtr__)(struct MmuFaultBuffer * /*this*/);  // virtual inherited (notify) base (notify)
    struct NotifShare * (*__faultbufGetNotificationShare__)(struct MmuFaultBuffer * /*this*/);  // virtual inherited (notify) base (notify)
    void (*__faultbufSetNotificationShare__)(struct MmuFaultBuffer * /*this*/, struct NotifShare *);  // virtual inherited (notify) base (notify)
    NV_STATUS (*__faultbufUnregisterEvent__)(struct MmuFaultBuffer * /*this*/, NvHandle, NvHandle, NvHandle, NvHandle);  // virtual inherited (notify) base (notify)
    NV_STATUS (*__faultbufGetOrAllocNotifShare__)(struct MmuFaultBuffer * /*this*/, NvHandle, NvHandle, struct NotifShare **);  // virtual inherited (notify) base (notify)
};

#ifndef __NVOC_CLASS_MmuFaultBuffer_TYPEDEF__
#define __NVOC_CLASS_MmuFaultBuffer_TYPEDEF__
typedef struct MmuFaultBuffer MmuFaultBuffer;
#endif /* __NVOC_CLASS_MmuFaultBuffer_TYPEDEF__ */

#ifndef __nvoc_class_id_MmuFaultBuffer
#define __nvoc_class_id_MmuFaultBuffer 0x7e1829
#endif /* __nvoc_class_id_MmuFaultBuffer */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_MmuFaultBuffer;

#define __staticCast_MmuFaultBuffer(pThis) \
    ((pThis)->__nvoc_pbase_MmuFaultBuffer)

#ifdef __nvoc_mmu_fault_buffer_h_disabled
#define __dynamicCast_MmuFaultBuffer(pThis) ((MmuFaultBuffer*)NULL)
#else //__nvoc_mmu_fault_buffer_h_disabled
#define __dynamicCast_MmuFaultBuffer(pThis) \
    ((MmuFaultBuffer*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(MmuFaultBuffer)))
#endif //__nvoc_mmu_fault_buffer_h_disabled

NV_STATUS __nvoc_objCreateDynamic_MmuFaultBuffer(MmuFaultBuffer**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_MmuFaultBuffer(MmuFaultBuffer**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_MmuFaultBuffer(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_MmuFaultBuffer((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define faultbufMap_FNPTR(pMmuFaultBuffer) pMmuFaultBuffer->__faultbufMap__
#define faultbufMap(pMmuFaultBuffer, pCallContext, pParams, pCpuMapping) faultbufMap_DISPATCH(pMmuFaultBuffer, pCallContext, pParams, pCpuMapping)
#define faultbufUnmap_FNPTR(pMmuFaultBuffer) pMmuFaultBuffer->__faultbufUnmap__
#define faultbufUnmap(pMmuFaultBuffer, pCallContext, pCpuMapping) faultbufUnmap_DISPATCH(pMmuFaultBuffer, pCallContext, pCpuMapping)
#define faultbufGetMapAddrSpace_FNPTR(pMmuFaultBuffer) pMmuFaultBuffer->__faultbufGetMapAddrSpace__
#define faultbufGetMapAddrSpace(pMmuFaultBuffer, pCallContext, mapFlags, pAddrSpace) faultbufGetMapAddrSpace_DISPATCH(pMmuFaultBuffer, pCallContext, mapFlags, pAddrSpace)
#define faultbufCtrlCmdFaultbufferGetSize_FNPTR(pMmuFaultBuffer) pMmuFaultBuffer->__faultbufCtrlCmdFaultbufferGetSize__
#define faultbufCtrlCmdFaultbufferGetSize(pMmuFaultBuffer, pGetParams) faultbufCtrlCmdFaultbufferGetSize_DISPATCH(pMmuFaultBuffer, pGetParams)
#define faultbufCtrlCmdFaultbufferGetRegisterMappings_FNPTR(pMmuFaultBuffer) pMmuFaultBuffer->__faultbufCtrlCmdFaultbufferGetRegisterMappings__
#define faultbufCtrlCmdFaultbufferGetRegisterMappings(pMmuFaultBuffer, pParams) faultbufCtrlCmdFaultbufferGetRegisterMappings_DISPATCH(pMmuFaultBuffer, pParams)
#define faultbufCtrlCmdMmuFaultBufferRegisterNonReplayBuf_FNPTR(pMmuFaultBuffer) pMmuFaultBuffer->__faultbufCtrlCmdMmuFaultBufferRegisterNonReplayBuf__
#define faultbufCtrlCmdMmuFaultBufferRegisterNonReplayBuf(pMmuFaultBuffer, pParams) faultbufCtrlCmdMmuFaultBufferRegisterNonReplayBuf_DISPATCH(pMmuFaultBuffer, pParams)
#define faultbufCtrlCmdMmuFaultBufferUnregisterNonReplayBuf_FNPTR(pMmuFaultBuffer) pMmuFaultBuffer->__faultbufCtrlCmdMmuFaultBufferUnregisterNonReplayBuf__
#define faultbufCtrlCmdMmuFaultBufferUnregisterNonReplayBuf(pMmuFaultBuffer, pParams) faultbufCtrlCmdMmuFaultBufferUnregisterNonReplayBuf_DISPATCH(pMmuFaultBuffer, pParams)
#define faultbufCtrlCmdMmuFaultBufferRegisterReplayBuf_FNPTR(pMmuFaultBuffer) pMmuFaultBuffer->__faultbufCtrlCmdMmuFaultBufferRegisterReplayBuf__
#define faultbufCtrlCmdMmuFaultBufferRegisterReplayBuf(pMmuFaultBuffer, pParams) faultbufCtrlCmdMmuFaultBufferRegisterReplayBuf_DISPATCH(pMmuFaultBuffer, pParams)
#define faultbufCtrlCmdMmuFaultBufferUnregisterReplayBuf_FNPTR(pMmuFaultBuffer) pMmuFaultBuffer->__faultbufCtrlCmdMmuFaultBufferUnregisterReplayBuf__
#define faultbufCtrlCmdMmuFaultBufferUnregisterReplayBuf(pMmuFaultBuffer, pParams) faultbufCtrlCmdMmuFaultBufferUnregisterReplayBuf_DISPATCH(pMmuFaultBuffer, pParams)
#define faultbufControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresControl__
#define faultbufControl(pGpuResource, pCallContext, pParams) faultbufControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define faultbufShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresShareCallback__
#define faultbufShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) faultbufShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define faultbufGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetRegBaseOffsetAndSize__
#define faultbufGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) faultbufGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define faultbufInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresInternalControlForward__
#define faultbufInternalControlForward(pGpuResource, command, pParams, size) faultbufInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define faultbufGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetInternalObjectHandle__
#define faultbufGetInternalObjectHandle(pGpuResource) faultbufGetInternalObjectHandle_DISPATCH(pGpuResource)
#define faultbufAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresAccessCallback__
#define faultbufAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) faultbufAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define faultbufGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemInterMapParams__
#define faultbufGetMemInterMapParams(pRmResource, pParams) faultbufGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define faultbufCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresCheckMemInterUnmap__
#define faultbufCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) faultbufCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define faultbufGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemoryMappingDescriptor__
#define faultbufGetMemoryMappingDescriptor(pRmResource, ppMemDesc) faultbufGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define faultbufControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Prologue__
#define faultbufControlSerialization_Prologue(pResource, pCallContext, pParams) faultbufControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define faultbufControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Epilogue__
#define faultbufControlSerialization_Epilogue(pResource, pCallContext, pParams) faultbufControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define faultbufControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Prologue__
#define faultbufControl_Prologue(pResource, pCallContext, pParams) faultbufControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define faultbufControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Epilogue__
#define faultbufControl_Epilogue(pResource, pCallContext, pParams) faultbufControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define faultbufCanCopy_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__
#define faultbufCanCopy(pResource) faultbufCanCopy_DISPATCH(pResource)
#define faultbufIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsDuplicate__
#define faultbufIsDuplicate(pResource, hMemory, pDuplicate) faultbufIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define faultbufPreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resPreDestruct__
#define faultbufPreDestruct(pResource) faultbufPreDestruct_DISPATCH(pResource)
#define faultbufControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resControlFilter__
#define faultbufControlFilter(pResource, pCallContext, pParams) faultbufControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define faultbufIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsPartialUnmapSupported__
#define faultbufIsPartialUnmapSupported(pResource) faultbufIsPartialUnmapSupported_DISPATCH(pResource)
#define faultbufMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resMapTo__
#define faultbufMapTo(pResource, pParams) faultbufMapTo_DISPATCH(pResource, pParams)
#define faultbufUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmapFrom__
#define faultbufUnmapFrom(pResource, pParams) faultbufUnmapFrom_DISPATCH(pResource, pParams)
#define faultbufGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resGetRefCount__
#define faultbufGetRefCount(pResource) faultbufGetRefCount_DISPATCH(pResource)
#define faultbufAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resAddAdditionalDependants__
#define faultbufAddAdditionalDependants(pClient, pResource, pReference) faultbufAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define faultbufGetNotificationListPtr_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__notifyGetNotificationListPtr__
#define faultbufGetNotificationListPtr(pNotifier) faultbufGetNotificationListPtr_DISPATCH(pNotifier)
#define faultbufGetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__notifyGetNotificationShare__
#define faultbufGetNotificationShare(pNotifier) faultbufGetNotificationShare_DISPATCH(pNotifier)
#define faultbufSetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__notifySetNotificationShare__
#define faultbufSetNotificationShare(pNotifier, pNotifShare) faultbufSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define faultbufUnregisterEvent_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__notifyUnregisterEvent__
#define faultbufUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) faultbufUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define faultbufGetOrAllocNotifShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__notifyGetOrAllocNotifShare__
#define faultbufGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) faultbufGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)

// Dispatch functions
static inline NV_STATUS faultbufMap_DISPATCH(struct MmuFaultBuffer *pMmuFaultBuffer, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pMmuFaultBuffer->__faultbufMap__(pMmuFaultBuffer, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS faultbufUnmap_DISPATCH(struct MmuFaultBuffer *pMmuFaultBuffer, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pMmuFaultBuffer->__faultbufUnmap__(pMmuFaultBuffer, pCallContext, pCpuMapping);
}

static inline NV_STATUS faultbufGetMapAddrSpace_DISPATCH(struct MmuFaultBuffer *pMmuFaultBuffer, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pMmuFaultBuffer->__faultbufGetMapAddrSpace__(pMmuFaultBuffer, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS faultbufCtrlCmdFaultbufferGetSize_DISPATCH(struct MmuFaultBuffer *pMmuFaultBuffer, NVB069_CTRL_FAULTBUFFER_GET_SIZE_PARAMS *pGetParams) {
    return pMmuFaultBuffer->__faultbufCtrlCmdFaultbufferGetSize__(pMmuFaultBuffer, pGetParams);
}

static inline NV_STATUS faultbufCtrlCmdFaultbufferGetRegisterMappings_DISPATCH(struct MmuFaultBuffer *pMmuFaultBuffer, NVB069_CTRL_CMD_FAULTBUFFER_GET_REGISTER_MAPPINGS_PARAMS *pParams) {
    return pMmuFaultBuffer->__faultbufCtrlCmdFaultbufferGetRegisterMappings__(pMmuFaultBuffer, pParams);
}

static inline NV_STATUS faultbufCtrlCmdMmuFaultBufferRegisterNonReplayBuf_DISPATCH(struct MmuFaultBuffer *pMmuFaultBuffer, NVC369_CTRL_MMU_FAULT_BUFFER_REGISTER_NON_REPLAY_BUF_PARAMS *pParams) {
    return pMmuFaultBuffer->__faultbufCtrlCmdMmuFaultBufferRegisterNonReplayBuf__(pMmuFaultBuffer, pParams);
}

static inline NV_STATUS faultbufCtrlCmdMmuFaultBufferUnregisterNonReplayBuf_DISPATCH(struct MmuFaultBuffer *pMmuFaultBuffer, NVC369_CTRL_MMU_FAULT_BUFFER_UNREGISTER_NON_REPLAY_BUF_PARAMS *pParams) {
    return pMmuFaultBuffer->__faultbufCtrlCmdMmuFaultBufferUnregisterNonReplayBuf__(pMmuFaultBuffer, pParams);
}

static inline NV_STATUS faultbufCtrlCmdMmuFaultBufferRegisterReplayBuf_DISPATCH(struct MmuFaultBuffer *pMmuFaultBuffer, NVC369_CTRL_MMU_FAULT_BUFFER_REGISTER_REPLAY_BUF_PARAMS *pParams) {
    return pMmuFaultBuffer->__faultbufCtrlCmdMmuFaultBufferRegisterReplayBuf__(pMmuFaultBuffer, pParams);
}

static inline NV_STATUS faultbufCtrlCmdMmuFaultBufferUnregisterReplayBuf_DISPATCH(struct MmuFaultBuffer *pMmuFaultBuffer, NVC369_CTRL_MMU_FAULT_BUFFER_UNREGISTER_REPLAY_BUF_PARAMS *pParams) {
    return pMmuFaultBuffer->__faultbufCtrlCmdMmuFaultBufferUnregisterReplayBuf__(pMmuFaultBuffer, pParams);
}

static inline NV_STATUS faultbufControl_DISPATCH(struct MmuFaultBuffer *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__faultbufControl__(pGpuResource, pCallContext, pParams);
}

static inline NvBool faultbufShareCallback_DISPATCH(struct MmuFaultBuffer *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__faultbufShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS faultbufGetRegBaseOffsetAndSize_DISPATCH(struct MmuFaultBuffer *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__faultbufGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS faultbufInternalControlForward_DISPATCH(struct MmuFaultBuffer *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__faultbufInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NvHandle faultbufGetInternalObjectHandle_DISPATCH(struct MmuFaultBuffer *pGpuResource) {
    return pGpuResource->__faultbufGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool faultbufAccessCallback_DISPATCH(struct MmuFaultBuffer *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__faultbufAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS faultbufGetMemInterMapParams_DISPATCH(struct MmuFaultBuffer *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__faultbufGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS faultbufCheckMemInterUnmap_DISPATCH(struct MmuFaultBuffer *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__faultbufCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS faultbufGetMemoryMappingDescriptor_DISPATCH(struct MmuFaultBuffer *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__faultbufGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS faultbufControlSerialization_Prologue_DISPATCH(struct MmuFaultBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__faultbufControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void faultbufControlSerialization_Epilogue_DISPATCH(struct MmuFaultBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__faultbufControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS faultbufControl_Prologue_DISPATCH(struct MmuFaultBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__faultbufControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void faultbufControl_Epilogue_DISPATCH(struct MmuFaultBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__faultbufControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool faultbufCanCopy_DISPATCH(struct MmuFaultBuffer *pResource) {
    return pResource->__faultbufCanCopy__(pResource);
}

static inline NV_STATUS faultbufIsDuplicate_DISPATCH(struct MmuFaultBuffer *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__faultbufIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void faultbufPreDestruct_DISPATCH(struct MmuFaultBuffer *pResource) {
    pResource->__faultbufPreDestruct__(pResource);
}

static inline NV_STATUS faultbufControlFilter_DISPATCH(struct MmuFaultBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__faultbufControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool faultbufIsPartialUnmapSupported_DISPATCH(struct MmuFaultBuffer *pResource) {
    return pResource->__faultbufIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS faultbufMapTo_DISPATCH(struct MmuFaultBuffer *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__faultbufMapTo__(pResource, pParams);
}

static inline NV_STATUS faultbufUnmapFrom_DISPATCH(struct MmuFaultBuffer *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__faultbufUnmapFrom__(pResource, pParams);
}

static inline NvU32 faultbufGetRefCount_DISPATCH(struct MmuFaultBuffer *pResource) {
    return pResource->__faultbufGetRefCount__(pResource);
}

static inline void faultbufAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct MmuFaultBuffer *pResource, RsResourceRef *pReference) {
    pResource->__faultbufAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline PEVENTNOTIFICATION * faultbufGetNotificationListPtr_DISPATCH(struct MmuFaultBuffer *pNotifier) {
    return pNotifier->__faultbufGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare * faultbufGetNotificationShare_DISPATCH(struct MmuFaultBuffer *pNotifier) {
    return pNotifier->__faultbufGetNotificationShare__(pNotifier);
}

static inline void faultbufSetNotificationShare_DISPATCH(struct MmuFaultBuffer *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__faultbufSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NV_STATUS faultbufUnregisterEvent_DISPATCH(struct MmuFaultBuffer *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__faultbufUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS faultbufGetOrAllocNotifShare_DISPATCH(struct MmuFaultBuffer *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__faultbufGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

NV_STATUS faultbufMap_IMPL(struct MmuFaultBuffer *pMmuFaultBuffer, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);

NV_STATUS faultbufUnmap_IMPL(struct MmuFaultBuffer *pMmuFaultBuffer, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);

NV_STATUS faultbufGetMapAddrSpace_IMPL(struct MmuFaultBuffer *pMmuFaultBuffer, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);

NV_STATUS faultbufCtrlCmdFaultbufferGetSize_IMPL(struct MmuFaultBuffer *pMmuFaultBuffer, NVB069_CTRL_FAULTBUFFER_GET_SIZE_PARAMS *pGetParams);

NV_STATUS faultbufCtrlCmdFaultbufferGetRegisterMappings_IMPL(struct MmuFaultBuffer *pMmuFaultBuffer, NVB069_CTRL_CMD_FAULTBUFFER_GET_REGISTER_MAPPINGS_PARAMS *pParams);

NV_STATUS faultbufCtrlCmdMmuFaultBufferRegisterNonReplayBuf_IMPL(struct MmuFaultBuffer *pMmuFaultBuffer, NVC369_CTRL_MMU_FAULT_BUFFER_REGISTER_NON_REPLAY_BUF_PARAMS *pParams);

NV_STATUS faultbufCtrlCmdMmuFaultBufferUnregisterNonReplayBuf_IMPL(struct MmuFaultBuffer *pMmuFaultBuffer, NVC369_CTRL_MMU_FAULT_BUFFER_UNREGISTER_NON_REPLAY_BUF_PARAMS *pParams);

NV_STATUS faultbufCtrlCmdMmuFaultBufferRegisterReplayBuf_IMPL(struct MmuFaultBuffer *pMmuFaultBuffer, NVC369_CTRL_MMU_FAULT_BUFFER_REGISTER_REPLAY_BUF_PARAMS *pParams);

NV_STATUS faultbufCtrlCmdMmuFaultBufferUnregisterReplayBuf_IMPL(struct MmuFaultBuffer *pMmuFaultBuffer, NVC369_CTRL_MMU_FAULT_BUFFER_UNREGISTER_REPLAY_BUF_PARAMS *pParams);

NV_STATUS faultbufConstruct_IMPL(struct MmuFaultBuffer *arg_pMmuFaultBuffer, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_faultbufConstruct(arg_pMmuFaultBuffer, arg_pCallContext, arg_pParams) faultbufConstruct_IMPL(arg_pMmuFaultBuffer, arg_pCallContext, arg_pParams)
void faultbufDestruct_IMPL(struct MmuFaultBuffer *pMmuFaultBuffer);

#define __nvoc_faultbufDestruct(pMmuFaultBuffer) faultbufDestruct_IMPL(pMmuFaultBuffer)
#undef PRIVATE_FIELD


#endif // MMU_FAULT_BUFFER_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_MMU_FAULT_BUFFER_NVOC_H_
