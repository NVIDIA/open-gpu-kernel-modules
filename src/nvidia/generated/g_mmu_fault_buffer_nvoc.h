#ifndef _G_MMU_FAULT_BUFFER_NVOC_H_
#define _G_MMU_FAULT_BUFFER_NVOC_H_
#include "nvoc/runtime.h"

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
#ifdef NVOC_MMU_FAULT_BUFFER_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct MmuFaultBuffer {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct GpuResource __nvoc_base_GpuResource;
    struct Notifier __nvoc_base_Notifier;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct GpuResource *__nvoc_pbase_GpuResource;
    struct INotifier *__nvoc_pbase_INotifier;
    struct Notifier *__nvoc_pbase_Notifier;
    struct MmuFaultBuffer *__nvoc_pbase_MmuFaultBuffer;
    NV_STATUS (*__faultbufMap__)(struct MmuFaultBuffer *, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);
    NV_STATUS (*__faultbufUnmap__)(struct MmuFaultBuffer *, struct CALL_CONTEXT *, struct RsCpuMapping *);
    NV_STATUS (*__faultbufGetMapAddrSpace__)(struct MmuFaultBuffer *, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    NV_STATUS (*__faultbufCtrlCmdFaultbufferGetSize__)(struct MmuFaultBuffer *, NVB069_CTRL_FAULTBUFFER_GET_SIZE_PARAMS *);
    NV_STATUS (*__faultbufCtrlCmdFaultbufferGetRegisterMappings__)(struct MmuFaultBuffer *, NVB069_CTRL_CMD_FAULTBUFFER_GET_REGISTER_MAPPINGS_PARAMS *);
    NV_STATUS (*__faultbufCtrlCmdMmuFaultBufferRegisterNonReplayBuf__)(struct MmuFaultBuffer *, NVC369_CTRL_MMU_FAULT_BUFFER_REGISTER_NON_REPLAY_BUF_PARAMS *);
    NV_STATUS (*__faultbufCtrlCmdMmuFaultBufferUnregisterNonReplayBuf__)(struct MmuFaultBuffer *, NVC369_CTRL_MMU_FAULT_BUFFER_UNREGISTER_NON_REPLAY_BUF_PARAMS *);
    NV_STATUS (*__faultbufCtrlCmdMmuFaultBufferRegisterReplayBuf__)(struct MmuFaultBuffer *, NVC369_CTRL_MMU_FAULT_BUFFER_REGISTER_REPLAY_BUF_PARAMS *);
    NV_STATUS (*__faultbufCtrlCmdMmuFaultBufferUnregisterReplayBuf__)(struct MmuFaultBuffer *, NVC369_CTRL_MMU_FAULT_BUFFER_UNREGISTER_REPLAY_BUF_PARAMS *);
    NvBool (*__faultbufShareCallback__)(struct MmuFaultBuffer *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__faultbufCheckMemInterUnmap__)(struct MmuFaultBuffer *, NvBool);
    NV_STATUS (*__faultbufGetOrAllocNotifShare__)(struct MmuFaultBuffer *, NvHandle, NvHandle, struct NotifShare **);
    NV_STATUS (*__faultbufMapTo__)(struct MmuFaultBuffer *, RS_RES_MAP_TO_PARAMS *);
    void (*__faultbufSetNotificationShare__)(struct MmuFaultBuffer *, struct NotifShare *);
    NvU32 (*__faultbufGetRefCount__)(struct MmuFaultBuffer *);
    void (*__faultbufAddAdditionalDependants__)(struct RsClient *, struct MmuFaultBuffer *, RsResourceRef *);
    NV_STATUS (*__faultbufControl_Prologue__)(struct MmuFaultBuffer *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__faultbufGetRegBaseOffsetAndSize__)(struct MmuFaultBuffer *, struct OBJGPU *, NvU32 *, NvU32 *);
    NV_STATUS (*__faultbufInternalControlForward__)(struct MmuFaultBuffer *, NvU32, void *, NvU32);
    NV_STATUS (*__faultbufUnmapFrom__)(struct MmuFaultBuffer *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__faultbufControl_Epilogue__)(struct MmuFaultBuffer *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__faultbufControlLookup__)(struct MmuFaultBuffer *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NvHandle (*__faultbufGetInternalObjectHandle__)(struct MmuFaultBuffer *);
    NV_STATUS (*__faultbufControl__)(struct MmuFaultBuffer *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__faultbufGetMemInterMapParams__)(struct MmuFaultBuffer *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__faultbufGetMemoryMappingDescriptor__)(struct MmuFaultBuffer *, struct MEMORY_DESCRIPTOR **);
    NV_STATUS (*__faultbufControlFilter__)(struct MmuFaultBuffer *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__faultbufUnregisterEvent__)(struct MmuFaultBuffer *, NvHandle, NvHandle, NvHandle, NvHandle);
    NV_STATUS (*__faultbufControlSerialization_Prologue__)(struct MmuFaultBuffer *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__faultbufCanCopy__)(struct MmuFaultBuffer *);
    void (*__faultbufPreDestruct__)(struct MmuFaultBuffer *);
    NV_STATUS (*__faultbufIsDuplicate__)(struct MmuFaultBuffer *, NvHandle, NvBool *);
    void (*__faultbufControlSerialization_Epilogue__)(struct MmuFaultBuffer *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    PEVENTNOTIFICATION *(*__faultbufGetNotificationListPtr__)(struct MmuFaultBuffer *);
    struct NotifShare *(*__faultbufGetNotificationShare__)(struct MmuFaultBuffer *);
    NvBool (*__faultbufAccessCallback__)(struct MmuFaultBuffer *, struct RsClient *, void *, RsAccessRight);
};

#ifndef __NVOC_CLASS_MmuFaultBuffer_TYPEDEF__
#define __NVOC_CLASS_MmuFaultBuffer_TYPEDEF__
typedef struct MmuFaultBuffer MmuFaultBuffer;
#endif /* __NVOC_CLASS_MmuFaultBuffer_TYPEDEF__ */

#ifndef __nvoc_class_id_MmuFaultBuffer
#define __nvoc_class_id_MmuFaultBuffer 0x7e1829
#endif /* __nvoc_class_id_MmuFaultBuffer */

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

#define faultbufMap(pMmuFaultBuffer, pCallContext, pParams, pCpuMapping) faultbufMap_DISPATCH(pMmuFaultBuffer, pCallContext, pParams, pCpuMapping)
#define faultbufUnmap(pMmuFaultBuffer, pCallContext, pCpuMapping) faultbufUnmap_DISPATCH(pMmuFaultBuffer, pCallContext, pCpuMapping)
#define faultbufGetMapAddrSpace(pMmuFaultBuffer, pCallContext, mapFlags, pAddrSpace) faultbufGetMapAddrSpace_DISPATCH(pMmuFaultBuffer, pCallContext, mapFlags, pAddrSpace)
#define faultbufCtrlCmdFaultbufferGetSize(pMmuFaultBuffer, pGetParams) faultbufCtrlCmdFaultbufferGetSize_DISPATCH(pMmuFaultBuffer, pGetParams)
#define faultbufCtrlCmdFaultbufferGetRegisterMappings(pMmuFaultBuffer, pParams) faultbufCtrlCmdFaultbufferGetRegisterMappings_DISPATCH(pMmuFaultBuffer, pParams)
#define faultbufCtrlCmdMmuFaultBufferRegisterNonReplayBuf(pMmuFaultBuffer, pParams) faultbufCtrlCmdMmuFaultBufferRegisterNonReplayBuf_DISPATCH(pMmuFaultBuffer, pParams)
#define faultbufCtrlCmdMmuFaultBufferUnregisterNonReplayBuf(pMmuFaultBuffer, pParams) faultbufCtrlCmdMmuFaultBufferUnregisterNonReplayBuf_DISPATCH(pMmuFaultBuffer, pParams)
#define faultbufCtrlCmdMmuFaultBufferRegisterReplayBuf(pMmuFaultBuffer, pParams) faultbufCtrlCmdMmuFaultBufferRegisterReplayBuf_DISPATCH(pMmuFaultBuffer, pParams)
#define faultbufCtrlCmdMmuFaultBufferUnregisterReplayBuf(pMmuFaultBuffer, pParams) faultbufCtrlCmdMmuFaultBufferUnregisterReplayBuf_DISPATCH(pMmuFaultBuffer, pParams)
#define faultbufShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) faultbufShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define faultbufCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) faultbufCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define faultbufGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) faultbufGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)
#define faultbufMapTo(pResource, pParams) faultbufMapTo_DISPATCH(pResource, pParams)
#define faultbufSetNotificationShare(pNotifier, pNotifShare) faultbufSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define faultbufGetRefCount(pResource) faultbufGetRefCount_DISPATCH(pResource)
#define faultbufAddAdditionalDependants(pClient, pResource, pReference) faultbufAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define faultbufControl_Prologue(pResource, pCallContext, pParams) faultbufControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define faultbufGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) faultbufGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define faultbufInternalControlForward(pGpuResource, command, pParams, size) faultbufInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define faultbufUnmapFrom(pResource, pParams) faultbufUnmapFrom_DISPATCH(pResource, pParams)
#define faultbufControl_Epilogue(pResource, pCallContext, pParams) faultbufControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define faultbufControlLookup(pResource, pParams, ppEntry) faultbufControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define faultbufGetInternalObjectHandle(pGpuResource) faultbufGetInternalObjectHandle_DISPATCH(pGpuResource)
#define faultbufControl(pGpuResource, pCallContext, pParams) faultbufControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define faultbufGetMemInterMapParams(pRmResource, pParams) faultbufGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define faultbufGetMemoryMappingDescriptor(pRmResource, ppMemDesc) faultbufGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define faultbufControlFilter(pResource, pCallContext, pParams) faultbufControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define faultbufUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) faultbufUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define faultbufControlSerialization_Prologue(pResource, pCallContext, pParams) faultbufControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define faultbufCanCopy(pResource) faultbufCanCopy_DISPATCH(pResource)
#define faultbufPreDestruct(pResource) faultbufPreDestruct_DISPATCH(pResource)
#define faultbufIsDuplicate(pResource, hMemory, pDuplicate) faultbufIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define faultbufControlSerialization_Epilogue(pResource, pCallContext, pParams) faultbufControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define faultbufGetNotificationListPtr(pNotifier) faultbufGetNotificationListPtr_DISPATCH(pNotifier)
#define faultbufGetNotificationShare(pNotifier) faultbufGetNotificationShare_DISPATCH(pNotifier)
#define faultbufAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) faultbufAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
NV_STATUS faultbufMap_IMPL(struct MmuFaultBuffer *pMmuFaultBuffer, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);

static inline NV_STATUS faultbufMap_DISPATCH(struct MmuFaultBuffer *pMmuFaultBuffer, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pMmuFaultBuffer->__faultbufMap__(pMmuFaultBuffer, pCallContext, pParams, pCpuMapping);
}

NV_STATUS faultbufUnmap_IMPL(struct MmuFaultBuffer *pMmuFaultBuffer, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);

static inline NV_STATUS faultbufUnmap_DISPATCH(struct MmuFaultBuffer *pMmuFaultBuffer, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pMmuFaultBuffer->__faultbufUnmap__(pMmuFaultBuffer, pCallContext, pCpuMapping);
}

NV_STATUS faultbufGetMapAddrSpace_IMPL(struct MmuFaultBuffer *pMmuFaultBuffer, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);

static inline NV_STATUS faultbufGetMapAddrSpace_DISPATCH(struct MmuFaultBuffer *pMmuFaultBuffer, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pMmuFaultBuffer->__faultbufGetMapAddrSpace__(pMmuFaultBuffer, pCallContext, mapFlags, pAddrSpace);
}

NV_STATUS faultbufCtrlCmdFaultbufferGetSize_IMPL(struct MmuFaultBuffer *pMmuFaultBuffer, NVB069_CTRL_FAULTBUFFER_GET_SIZE_PARAMS *pGetParams);

static inline NV_STATUS faultbufCtrlCmdFaultbufferGetSize_DISPATCH(struct MmuFaultBuffer *pMmuFaultBuffer, NVB069_CTRL_FAULTBUFFER_GET_SIZE_PARAMS *pGetParams) {
    return pMmuFaultBuffer->__faultbufCtrlCmdFaultbufferGetSize__(pMmuFaultBuffer, pGetParams);
}

NV_STATUS faultbufCtrlCmdFaultbufferGetRegisterMappings_IMPL(struct MmuFaultBuffer *pMmuFaultBuffer, NVB069_CTRL_CMD_FAULTBUFFER_GET_REGISTER_MAPPINGS_PARAMS *pParams);

static inline NV_STATUS faultbufCtrlCmdFaultbufferGetRegisterMappings_DISPATCH(struct MmuFaultBuffer *pMmuFaultBuffer, NVB069_CTRL_CMD_FAULTBUFFER_GET_REGISTER_MAPPINGS_PARAMS *pParams) {
    return pMmuFaultBuffer->__faultbufCtrlCmdFaultbufferGetRegisterMappings__(pMmuFaultBuffer, pParams);
}

NV_STATUS faultbufCtrlCmdMmuFaultBufferRegisterNonReplayBuf_IMPL(struct MmuFaultBuffer *pMmuFaultBuffer, NVC369_CTRL_MMU_FAULT_BUFFER_REGISTER_NON_REPLAY_BUF_PARAMS *pParams);

static inline NV_STATUS faultbufCtrlCmdMmuFaultBufferRegisterNonReplayBuf_DISPATCH(struct MmuFaultBuffer *pMmuFaultBuffer, NVC369_CTRL_MMU_FAULT_BUFFER_REGISTER_NON_REPLAY_BUF_PARAMS *pParams) {
    return pMmuFaultBuffer->__faultbufCtrlCmdMmuFaultBufferRegisterNonReplayBuf__(pMmuFaultBuffer, pParams);
}

NV_STATUS faultbufCtrlCmdMmuFaultBufferUnregisterNonReplayBuf_IMPL(struct MmuFaultBuffer *pMmuFaultBuffer, NVC369_CTRL_MMU_FAULT_BUFFER_UNREGISTER_NON_REPLAY_BUF_PARAMS *pParams);

static inline NV_STATUS faultbufCtrlCmdMmuFaultBufferUnregisterNonReplayBuf_DISPATCH(struct MmuFaultBuffer *pMmuFaultBuffer, NVC369_CTRL_MMU_FAULT_BUFFER_UNREGISTER_NON_REPLAY_BUF_PARAMS *pParams) {
    return pMmuFaultBuffer->__faultbufCtrlCmdMmuFaultBufferUnregisterNonReplayBuf__(pMmuFaultBuffer, pParams);
}

NV_STATUS faultbufCtrlCmdMmuFaultBufferRegisterReplayBuf_IMPL(struct MmuFaultBuffer *pMmuFaultBuffer, NVC369_CTRL_MMU_FAULT_BUFFER_REGISTER_REPLAY_BUF_PARAMS *pParams);

static inline NV_STATUS faultbufCtrlCmdMmuFaultBufferRegisterReplayBuf_DISPATCH(struct MmuFaultBuffer *pMmuFaultBuffer, NVC369_CTRL_MMU_FAULT_BUFFER_REGISTER_REPLAY_BUF_PARAMS *pParams) {
    return pMmuFaultBuffer->__faultbufCtrlCmdMmuFaultBufferRegisterReplayBuf__(pMmuFaultBuffer, pParams);
}

NV_STATUS faultbufCtrlCmdMmuFaultBufferUnregisterReplayBuf_IMPL(struct MmuFaultBuffer *pMmuFaultBuffer, NVC369_CTRL_MMU_FAULT_BUFFER_UNREGISTER_REPLAY_BUF_PARAMS *pParams);

static inline NV_STATUS faultbufCtrlCmdMmuFaultBufferUnregisterReplayBuf_DISPATCH(struct MmuFaultBuffer *pMmuFaultBuffer, NVC369_CTRL_MMU_FAULT_BUFFER_UNREGISTER_REPLAY_BUF_PARAMS *pParams) {
    return pMmuFaultBuffer->__faultbufCtrlCmdMmuFaultBufferUnregisterReplayBuf__(pMmuFaultBuffer, pParams);
}

static inline NvBool faultbufShareCallback_DISPATCH(struct MmuFaultBuffer *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__faultbufShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS faultbufCheckMemInterUnmap_DISPATCH(struct MmuFaultBuffer *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__faultbufCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS faultbufGetOrAllocNotifShare_DISPATCH(struct MmuFaultBuffer *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__faultbufGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

static inline NV_STATUS faultbufMapTo_DISPATCH(struct MmuFaultBuffer *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__faultbufMapTo__(pResource, pParams);
}

static inline void faultbufSetNotificationShare_DISPATCH(struct MmuFaultBuffer *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__faultbufSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NvU32 faultbufGetRefCount_DISPATCH(struct MmuFaultBuffer *pResource) {
    return pResource->__faultbufGetRefCount__(pResource);
}

static inline void faultbufAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct MmuFaultBuffer *pResource, RsResourceRef *pReference) {
    pResource->__faultbufAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS faultbufControl_Prologue_DISPATCH(struct MmuFaultBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__faultbufControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS faultbufGetRegBaseOffsetAndSize_DISPATCH(struct MmuFaultBuffer *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__faultbufGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS faultbufInternalControlForward_DISPATCH(struct MmuFaultBuffer *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__faultbufInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NV_STATUS faultbufUnmapFrom_DISPATCH(struct MmuFaultBuffer *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__faultbufUnmapFrom__(pResource, pParams);
}

static inline void faultbufControl_Epilogue_DISPATCH(struct MmuFaultBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__faultbufControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS faultbufControlLookup_DISPATCH(struct MmuFaultBuffer *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__faultbufControlLookup__(pResource, pParams, ppEntry);
}

static inline NvHandle faultbufGetInternalObjectHandle_DISPATCH(struct MmuFaultBuffer *pGpuResource) {
    return pGpuResource->__faultbufGetInternalObjectHandle__(pGpuResource);
}

static inline NV_STATUS faultbufControl_DISPATCH(struct MmuFaultBuffer *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__faultbufControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS faultbufGetMemInterMapParams_DISPATCH(struct MmuFaultBuffer *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__faultbufGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS faultbufGetMemoryMappingDescriptor_DISPATCH(struct MmuFaultBuffer *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__faultbufGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS faultbufControlFilter_DISPATCH(struct MmuFaultBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__faultbufControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS faultbufUnregisterEvent_DISPATCH(struct MmuFaultBuffer *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__faultbufUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS faultbufControlSerialization_Prologue_DISPATCH(struct MmuFaultBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__faultbufControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool faultbufCanCopy_DISPATCH(struct MmuFaultBuffer *pResource) {
    return pResource->__faultbufCanCopy__(pResource);
}

static inline void faultbufPreDestruct_DISPATCH(struct MmuFaultBuffer *pResource) {
    pResource->__faultbufPreDestruct__(pResource);
}

static inline NV_STATUS faultbufIsDuplicate_DISPATCH(struct MmuFaultBuffer *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__faultbufIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void faultbufControlSerialization_Epilogue_DISPATCH(struct MmuFaultBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__faultbufControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline PEVENTNOTIFICATION *faultbufGetNotificationListPtr_DISPATCH(struct MmuFaultBuffer *pNotifier) {
    return pNotifier->__faultbufGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare *faultbufGetNotificationShare_DISPATCH(struct MmuFaultBuffer *pNotifier) {
    return pNotifier->__faultbufGetNotificationShare__(pNotifier);
}

static inline NvBool faultbufAccessCallback_DISPATCH(struct MmuFaultBuffer *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__faultbufAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

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
