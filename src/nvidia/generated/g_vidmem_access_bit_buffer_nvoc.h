
#ifndef _G_VIDMEM_ACCESS_BIT_BUFFER_NVOC_H_
#define _G_VIDMEM_ACCESS_BIT_BUFFER_NVOC_H_
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
 * SPDX-FileCopyrightText: Copyright (c) 2021-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_vidmem_access_bit_buffer_nvoc.h"

#ifndef VIDMEM_ACCESS_BIT_BUFFER_H
#define VIDMEM_ACCESS_BIT_BUFFER_H

#include "gpu/gpu.h"
#include "gpu/gpu_resource.h"
#include "rmapi/event.h"
#include "ctrl/ctrlc763.h"

typedef struct
{
    NvU8   entrySize;
    NvU8   entryGranularity;
    NvU8   trackMode;
    NvU8   disableMode;
    NvBool bEnabled;
    NvBool bFloorswept;
}NV_VIDMEM_ACCESS_BIT_BUFFER_HW_ATTR;


#define NUM_MMU_TYPES 0x3

// This is used to make the conversion from the granularity enum to byte size more readable
#define NV_VAB_GRAN_SIZE(gran)      NVBIT64(16 + (NvU64)(gran))

#define NV_VAB_SIZE_TO_BIT_COUNT(vabSize)   ((vabSize)*8)
#define NV_VAB_SIZE_TO_NVU64_COUNT(vabSize) ((vabSize)/8)


/*
 * Maximum length of access bit mask and access bit buffer in NvU64s. Legacy define used in the old interface. To be deleted once MODS transitions to V2.
 */
#define AMPERE_VIDMEM_BIT_BUF_SIZE 512
#define NV_VAB_BUFFER_MAX_NVU64_COUNT NV_VAB_SIZE_TO_NVU64_COUNT(AMPERE_VIDMEM_BIT_BUF_SIZE)

/*
 * These values are returned from gmmuVidmemAccessBitBufferVersion_HAL to indicate
 * the type of VAB interface supported on the current platform.
 */
#define NV_VAB_VERSION_NONE           0
#define NV_VAB_VERSION_SINGLE_RANGE   1
#define NV_VAB_VERSION_MULTI_RANGE    2

/*!
 * RM internal class representing VIDMEM_ACCESS_BIT_BUFFER
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_VIDMEM_ACCESS_BIT_BUFFER_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct VidmemAccessBitBuffer {

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
    struct VidmemAccessBitBuffer *__nvoc_pbase_VidmemAccessBitBuffer;    // vidmemAccessBitBuf

    // Vtable with 31 per-object function pointers
    NV_STATUS (*__vidmemAccessBitBufCtrlCmdVidmemAccessBitDump__)(struct VidmemAccessBitBuffer * /*this*/, NVC763_CTRL_VIDMEM_ACCESS_BIT_BUFFER_DUMP_PARAMS *);  // exported (id=0xc7630103)
    NV_STATUS (*__vidmemAccessBitBufControl__)(struct VidmemAccessBitBuffer * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__vidmemAccessBitBufMap__)(struct VidmemAccessBitBuffer * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__vidmemAccessBitBufUnmap__)(struct VidmemAccessBitBuffer * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NvBool (*__vidmemAccessBitBufShareCallback__)(struct VidmemAccessBitBuffer * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__vidmemAccessBitBufGetRegBaseOffsetAndSize__)(struct VidmemAccessBitBuffer * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__vidmemAccessBitBufGetMapAddrSpace__)(struct VidmemAccessBitBuffer * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__vidmemAccessBitBufInternalControlForward__)(struct VidmemAccessBitBuffer * /*this*/, NvU32, void *, NvU32);  // virtual inherited (gpures) base (gpures)
    NvHandle (*__vidmemAccessBitBufGetInternalObjectHandle__)(struct VidmemAccessBitBuffer * /*this*/);  // virtual inherited (gpures) base (gpures)
    NvBool (*__vidmemAccessBitBufAccessCallback__)(struct VidmemAccessBitBuffer * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__vidmemAccessBitBufGetMemInterMapParams__)(struct VidmemAccessBitBuffer * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__vidmemAccessBitBufCheckMemInterUnmap__)(struct VidmemAccessBitBuffer * /*this*/, NvBool);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__vidmemAccessBitBufGetMemoryMappingDescriptor__)(struct VidmemAccessBitBuffer * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__vidmemAccessBitBufControlSerialization_Prologue__)(struct VidmemAccessBitBuffer * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__vidmemAccessBitBufControlSerialization_Epilogue__)(struct VidmemAccessBitBuffer * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__vidmemAccessBitBufControl_Prologue__)(struct VidmemAccessBitBuffer * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__vidmemAccessBitBufControl_Epilogue__)(struct VidmemAccessBitBuffer * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NvBool (*__vidmemAccessBitBufCanCopy__)(struct VidmemAccessBitBuffer * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__vidmemAccessBitBufIsDuplicate__)(struct VidmemAccessBitBuffer * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (gpures)
    void (*__vidmemAccessBitBufPreDestruct__)(struct VidmemAccessBitBuffer * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__vidmemAccessBitBufControlFilter__)(struct VidmemAccessBitBuffer * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (gpures)
    NvBool (*__vidmemAccessBitBufIsPartialUnmapSupported__)(struct VidmemAccessBitBuffer * /*this*/);  // inline virtual inherited (res) base (gpures) body
    NV_STATUS (*__vidmemAccessBitBufMapTo__)(struct VidmemAccessBitBuffer * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__vidmemAccessBitBufUnmapFrom__)(struct VidmemAccessBitBuffer * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (gpures)
    NvU32 (*__vidmemAccessBitBufGetRefCount__)(struct VidmemAccessBitBuffer * /*this*/);  // virtual inherited (res) base (gpures)
    void (*__vidmemAccessBitBufAddAdditionalDependants__)(struct RsClient *, struct VidmemAccessBitBuffer * /*this*/, RsResourceRef *);  // virtual inherited (res) base (gpures)
    PEVENTNOTIFICATION * (*__vidmemAccessBitBufGetNotificationListPtr__)(struct VidmemAccessBitBuffer * /*this*/);  // virtual inherited (notify) base (notify)
    struct NotifShare * (*__vidmemAccessBitBufGetNotificationShare__)(struct VidmemAccessBitBuffer * /*this*/);  // virtual inherited (notify) base (notify)
    void (*__vidmemAccessBitBufSetNotificationShare__)(struct VidmemAccessBitBuffer * /*this*/, struct NotifShare *);  // virtual inherited (notify) base (notify)
    NV_STATUS (*__vidmemAccessBitBufUnregisterEvent__)(struct VidmemAccessBitBuffer * /*this*/, NvHandle, NvHandle, NvHandle, NvHandle);  // virtual inherited (notify) base (notify)
    NV_STATUS (*__vidmemAccessBitBufGetOrAllocNotifShare__)(struct VidmemAccessBitBuffer * /*this*/, NvHandle, NvHandle, struct NotifShare **);  // virtual inherited (notify) base (notify)
};

#ifndef __NVOC_CLASS_VidmemAccessBitBuffer_TYPEDEF__
#define __NVOC_CLASS_VidmemAccessBitBuffer_TYPEDEF__
typedef struct VidmemAccessBitBuffer VidmemAccessBitBuffer;
#endif /* __NVOC_CLASS_VidmemAccessBitBuffer_TYPEDEF__ */

#ifndef __nvoc_class_id_VidmemAccessBitBuffer
#define __nvoc_class_id_VidmemAccessBitBuffer 0xebb6da
#endif /* __nvoc_class_id_VidmemAccessBitBuffer */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_VidmemAccessBitBuffer;

#define __staticCast_VidmemAccessBitBuffer(pThis) \
    ((pThis)->__nvoc_pbase_VidmemAccessBitBuffer)

#ifdef __nvoc_vidmem_access_bit_buffer_h_disabled
#define __dynamicCast_VidmemAccessBitBuffer(pThis) ((VidmemAccessBitBuffer*)NULL)
#else //__nvoc_vidmem_access_bit_buffer_h_disabled
#define __dynamicCast_VidmemAccessBitBuffer(pThis) \
    ((VidmemAccessBitBuffer*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(VidmemAccessBitBuffer)))
#endif //__nvoc_vidmem_access_bit_buffer_h_disabled

NV_STATUS __nvoc_objCreateDynamic_VidmemAccessBitBuffer(VidmemAccessBitBuffer**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_VidmemAccessBitBuffer(VidmemAccessBitBuffer**, Dynamic*, NvU32, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_VidmemAccessBitBuffer(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_VidmemAccessBitBuffer((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define vidmemAccessBitBufCtrlCmdVidmemAccessBitDump_FNPTR(pVidmemAccessBitBuffer) pVidmemAccessBitBuffer->__vidmemAccessBitBufCtrlCmdVidmemAccessBitDump__
#define vidmemAccessBitBufCtrlCmdVidmemAccessBitDump(pVidmemAccessBitBuffer, pParams) vidmemAccessBitBufCtrlCmdVidmemAccessBitDump_DISPATCH(pVidmemAccessBitBuffer, pParams)
#define vidmemAccessBitBufControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresControl__
#define vidmemAccessBitBufControl(pGpuResource, pCallContext, pParams) vidmemAccessBitBufControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define vidmemAccessBitBufMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresMap__
#define vidmemAccessBitBufMap(pGpuResource, pCallContext, pParams, pCpuMapping) vidmemAccessBitBufMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define vidmemAccessBitBufUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresUnmap__
#define vidmemAccessBitBufUnmap(pGpuResource, pCallContext, pCpuMapping) vidmemAccessBitBufUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define vidmemAccessBitBufShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresShareCallback__
#define vidmemAccessBitBufShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) vidmemAccessBitBufShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define vidmemAccessBitBufGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetRegBaseOffsetAndSize__
#define vidmemAccessBitBufGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) vidmemAccessBitBufGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define vidmemAccessBitBufGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetMapAddrSpace__
#define vidmemAccessBitBufGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) vidmemAccessBitBufGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define vidmemAccessBitBufInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresInternalControlForward__
#define vidmemAccessBitBufInternalControlForward(pGpuResource, command, pParams, size) vidmemAccessBitBufInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define vidmemAccessBitBufGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetInternalObjectHandle__
#define vidmemAccessBitBufGetInternalObjectHandle(pGpuResource) vidmemAccessBitBufGetInternalObjectHandle_DISPATCH(pGpuResource)
#define vidmemAccessBitBufAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresAccessCallback__
#define vidmemAccessBitBufAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) vidmemAccessBitBufAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define vidmemAccessBitBufGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemInterMapParams__
#define vidmemAccessBitBufGetMemInterMapParams(pRmResource, pParams) vidmemAccessBitBufGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define vidmemAccessBitBufCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresCheckMemInterUnmap__
#define vidmemAccessBitBufCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) vidmemAccessBitBufCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define vidmemAccessBitBufGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemoryMappingDescriptor__
#define vidmemAccessBitBufGetMemoryMappingDescriptor(pRmResource, ppMemDesc) vidmemAccessBitBufGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define vidmemAccessBitBufControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Prologue__
#define vidmemAccessBitBufControlSerialization_Prologue(pResource, pCallContext, pParams) vidmemAccessBitBufControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define vidmemAccessBitBufControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Epilogue__
#define vidmemAccessBitBufControlSerialization_Epilogue(pResource, pCallContext, pParams) vidmemAccessBitBufControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define vidmemAccessBitBufControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Prologue__
#define vidmemAccessBitBufControl_Prologue(pResource, pCallContext, pParams) vidmemAccessBitBufControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define vidmemAccessBitBufControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Epilogue__
#define vidmemAccessBitBufControl_Epilogue(pResource, pCallContext, pParams) vidmemAccessBitBufControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define vidmemAccessBitBufCanCopy_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__
#define vidmemAccessBitBufCanCopy(pResource) vidmemAccessBitBufCanCopy_DISPATCH(pResource)
#define vidmemAccessBitBufIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsDuplicate__
#define vidmemAccessBitBufIsDuplicate(pResource, hMemory, pDuplicate) vidmemAccessBitBufIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define vidmemAccessBitBufPreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resPreDestruct__
#define vidmemAccessBitBufPreDestruct(pResource) vidmemAccessBitBufPreDestruct_DISPATCH(pResource)
#define vidmemAccessBitBufControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resControlFilter__
#define vidmemAccessBitBufControlFilter(pResource, pCallContext, pParams) vidmemAccessBitBufControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define vidmemAccessBitBufIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsPartialUnmapSupported__
#define vidmemAccessBitBufIsPartialUnmapSupported(pResource) vidmemAccessBitBufIsPartialUnmapSupported_DISPATCH(pResource)
#define vidmemAccessBitBufMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resMapTo__
#define vidmemAccessBitBufMapTo(pResource, pParams) vidmemAccessBitBufMapTo_DISPATCH(pResource, pParams)
#define vidmemAccessBitBufUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmapFrom__
#define vidmemAccessBitBufUnmapFrom(pResource, pParams) vidmemAccessBitBufUnmapFrom_DISPATCH(pResource, pParams)
#define vidmemAccessBitBufGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resGetRefCount__
#define vidmemAccessBitBufGetRefCount(pResource) vidmemAccessBitBufGetRefCount_DISPATCH(pResource)
#define vidmemAccessBitBufAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resAddAdditionalDependants__
#define vidmemAccessBitBufAddAdditionalDependants(pClient, pResource, pReference) vidmemAccessBitBufAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define vidmemAccessBitBufGetNotificationListPtr_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__notifyGetNotificationListPtr__
#define vidmemAccessBitBufGetNotificationListPtr(pNotifier) vidmemAccessBitBufGetNotificationListPtr_DISPATCH(pNotifier)
#define vidmemAccessBitBufGetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__notifyGetNotificationShare__
#define vidmemAccessBitBufGetNotificationShare(pNotifier) vidmemAccessBitBufGetNotificationShare_DISPATCH(pNotifier)
#define vidmemAccessBitBufSetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__notifySetNotificationShare__
#define vidmemAccessBitBufSetNotificationShare(pNotifier, pNotifShare) vidmemAccessBitBufSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define vidmemAccessBitBufUnregisterEvent_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__notifyUnregisterEvent__
#define vidmemAccessBitBufUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) vidmemAccessBitBufUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define vidmemAccessBitBufGetOrAllocNotifShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__notifyGetOrAllocNotifShare__
#define vidmemAccessBitBufGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) vidmemAccessBitBufGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)

// Dispatch functions
static inline NV_STATUS vidmemAccessBitBufCtrlCmdVidmemAccessBitDump_DISPATCH(struct VidmemAccessBitBuffer *pVidmemAccessBitBuffer, NVC763_CTRL_VIDMEM_ACCESS_BIT_BUFFER_DUMP_PARAMS *pParams) {
    return pVidmemAccessBitBuffer->__vidmemAccessBitBufCtrlCmdVidmemAccessBitDump__(pVidmemAccessBitBuffer, pParams);
}

static inline NV_STATUS vidmemAccessBitBufControl_DISPATCH(struct VidmemAccessBitBuffer *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__vidmemAccessBitBufControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS vidmemAccessBitBufMap_DISPATCH(struct VidmemAccessBitBuffer *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__vidmemAccessBitBufMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS vidmemAccessBitBufUnmap_DISPATCH(struct VidmemAccessBitBuffer *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__vidmemAccessBitBufUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NvBool vidmemAccessBitBufShareCallback_DISPATCH(struct VidmemAccessBitBuffer *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__vidmemAccessBitBufShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS vidmemAccessBitBufGetRegBaseOffsetAndSize_DISPATCH(struct VidmemAccessBitBuffer *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__vidmemAccessBitBufGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS vidmemAccessBitBufGetMapAddrSpace_DISPATCH(struct VidmemAccessBitBuffer *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__vidmemAccessBitBufGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS vidmemAccessBitBufInternalControlForward_DISPATCH(struct VidmemAccessBitBuffer *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__vidmemAccessBitBufInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NvHandle vidmemAccessBitBufGetInternalObjectHandle_DISPATCH(struct VidmemAccessBitBuffer *pGpuResource) {
    return pGpuResource->__vidmemAccessBitBufGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool vidmemAccessBitBufAccessCallback_DISPATCH(struct VidmemAccessBitBuffer *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__vidmemAccessBitBufAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS vidmemAccessBitBufGetMemInterMapParams_DISPATCH(struct VidmemAccessBitBuffer *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__vidmemAccessBitBufGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS vidmemAccessBitBufCheckMemInterUnmap_DISPATCH(struct VidmemAccessBitBuffer *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__vidmemAccessBitBufCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS vidmemAccessBitBufGetMemoryMappingDescriptor_DISPATCH(struct VidmemAccessBitBuffer *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__vidmemAccessBitBufGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS vidmemAccessBitBufControlSerialization_Prologue_DISPATCH(struct VidmemAccessBitBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__vidmemAccessBitBufControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void vidmemAccessBitBufControlSerialization_Epilogue_DISPATCH(struct VidmemAccessBitBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__vidmemAccessBitBufControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS vidmemAccessBitBufControl_Prologue_DISPATCH(struct VidmemAccessBitBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__vidmemAccessBitBufControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void vidmemAccessBitBufControl_Epilogue_DISPATCH(struct VidmemAccessBitBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__vidmemAccessBitBufControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool vidmemAccessBitBufCanCopy_DISPATCH(struct VidmemAccessBitBuffer *pResource) {
    return pResource->__vidmemAccessBitBufCanCopy__(pResource);
}

static inline NV_STATUS vidmemAccessBitBufIsDuplicate_DISPATCH(struct VidmemAccessBitBuffer *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__vidmemAccessBitBufIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void vidmemAccessBitBufPreDestruct_DISPATCH(struct VidmemAccessBitBuffer *pResource) {
    pResource->__vidmemAccessBitBufPreDestruct__(pResource);
}

static inline NV_STATUS vidmemAccessBitBufControlFilter_DISPATCH(struct VidmemAccessBitBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__vidmemAccessBitBufControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool vidmemAccessBitBufIsPartialUnmapSupported_DISPATCH(struct VidmemAccessBitBuffer *pResource) {
    return pResource->__vidmemAccessBitBufIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS vidmemAccessBitBufMapTo_DISPATCH(struct VidmemAccessBitBuffer *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__vidmemAccessBitBufMapTo__(pResource, pParams);
}

static inline NV_STATUS vidmemAccessBitBufUnmapFrom_DISPATCH(struct VidmemAccessBitBuffer *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__vidmemAccessBitBufUnmapFrom__(pResource, pParams);
}

static inline NvU32 vidmemAccessBitBufGetRefCount_DISPATCH(struct VidmemAccessBitBuffer *pResource) {
    return pResource->__vidmemAccessBitBufGetRefCount__(pResource);
}

static inline void vidmemAccessBitBufAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct VidmemAccessBitBuffer *pResource, RsResourceRef *pReference) {
    pResource->__vidmemAccessBitBufAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline PEVENTNOTIFICATION * vidmemAccessBitBufGetNotificationListPtr_DISPATCH(struct VidmemAccessBitBuffer *pNotifier) {
    return pNotifier->__vidmemAccessBitBufGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare * vidmemAccessBitBufGetNotificationShare_DISPATCH(struct VidmemAccessBitBuffer *pNotifier) {
    return pNotifier->__vidmemAccessBitBufGetNotificationShare__(pNotifier);
}

static inline void vidmemAccessBitBufSetNotificationShare_DISPATCH(struct VidmemAccessBitBuffer *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__vidmemAccessBitBufSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NV_STATUS vidmemAccessBitBufUnregisterEvent_DISPATCH(struct VidmemAccessBitBuffer *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__vidmemAccessBitBufUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS vidmemAccessBitBufGetOrAllocNotifShare_DISPATCH(struct VidmemAccessBitBuffer *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__vidmemAccessBitBufGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

NV_STATUS vidmemAccessBitBufConstructHelper_KERNEL(struct VidmemAccessBitBuffer *pVidmem, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);


#ifdef __nvoc_vidmem_access_bit_buffer_h_disabled
static inline NV_STATUS vidmemAccessBitBufConstructHelper(struct VidmemAccessBitBuffer *pVidmem, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {
    NV_ASSERT_FAILED_PRECOMP("VidmemAccessBitBuffer was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_vidmem_access_bit_buffer_h_disabled
#define vidmemAccessBitBufConstructHelper(pVidmem, pCallContext, pParams) vidmemAccessBitBufConstructHelper_KERNEL(pVidmem, pCallContext, pParams)
#endif //__nvoc_vidmem_access_bit_buffer_h_disabled

#define vidmemAccessBitBufConstructHelper_HAL(pVidmem, pCallContext, pParams) vidmemAccessBitBufConstructHelper(pVidmem, pCallContext, pParams)

static inline void vidmemAccessBitBufDestruct_b3696a(struct VidmemAccessBitBuffer *pVidmemAccessBitBuffer) {
    return;
}


#define __nvoc_vidmemAccessBitBufDestruct(pVidmemAccessBitBuffer) vidmemAccessBitBufDestruct_b3696a(pVidmemAccessBitBuffer)
NV_STATUS vidmemAccessBitBufCtrlCmdVidmemAccessBitDump_IMPL(struct VidmemAccessBitBuffer *pVidmemAccessBitBuffer, NVC763_CTRL_VIDMEM_ACCESS_BIT_BUFFER_DUMP_PARAMS *pParams);

static inline NV_STATUS __nvoc_vidmemAccessBitBufConstruct(struct VidmemAccessBitBuffer *arg_pVidmemAccessBitBuffer, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams) {
    return vidmemAccessBitBufConstructHelper(arg_pVidmemAccessBitBuffer, arg_pCallContext, arg_pParams);
}

#undef PRIVATE_FIELD


#endif // VIDMEM_ACCESS_BIT_BUFFER_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_VIDMEM_ACCESS_BIT_BUFFER_NVOC_H_
