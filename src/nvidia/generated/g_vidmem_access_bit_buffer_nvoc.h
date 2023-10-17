#ifndef _G_VIDMEM_ACCESS_BIT_BUFFER_NVOC_H_
#define _G_VIDMEM_ACCESS_BIT_BUFFER_NVOC_H_
#include "nvoc/runtime.h"

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
#ifdef NVOC_VIDMEM_ACCESS_BIT_BUFFER_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct VidmemAccessBitBuffer {
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
    struct VidmemAccessBitBuffer *__nvoc_pbase_VidmemAccessBitBuffer;
    NV_STATUS (*__vidmemAccessBitBufCtrlCmdVidmemAccessBitDump__)(struct VidmemAccessBitBuffer *, NVC763_CTRL_VIDMEM_ACCESS_BIT_BUFFER_DUMP_PARAMS *);
    NvBool (*__vidmemAccessBitBufShareCallback__)(struct VidmemAccessBitBuffer *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__vidmemAccessBitBufCheckMemInterUnmap__)(struct VidmemAccessBitBuffer *, NvBool);
    NV_STATUS (*__vidmemAccessBitBufGetOrAllocNotifShare__)(struct VidmemAccessBitBuffer *, NvHandle, NvHandle, struct NotifShare **);
    NV_STATUS (*__vidmemAccessBitBufMapTo__)(struct VidmemAccessBitBuffer *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__vidmemAccessBitBufGetMapAddrSpace__)(struct VidmemAccessBitBuffer *, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    void (*__vidmemAccessBitBufSetNotificationShare__)(struct VidmemAccessBitBuffer *, struct NotifShare *);
    NvU32 (*__vidmemAccessBitBufGetRefCount__)(struct VidmemAccessBitBuffer *);
    void (*__vidmemAccessBitBufAddAdditionalDependants__)(struct RsClient *, struct VidmemAccessBitBuffer *, RsResourceRef *);
    NV_STATUS (*__vidmemAccessBitBufControl_Prologue__)(struct VidmemAccessBitBuffer *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__vidmemAccessBitBufGetRegBaseOffsetAndSize__)(struct VidmemAccessBitBuffer *, struct OBJGPU *, NvU32 *, NvU32 *);
    NV_STATUS (*__vidmemAccessBitBufInternalControlForward__)(struct VidmemAccessBitBuffer *, NvU32, void *, NvU32);
    NV_STATUS (*__vidmemAccessBitBufUnmapFrom__)(struct VidmemAccessBitBuffer *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__vidmemAccessBitBufControl_Epilogue__)(struct VidmemAccessBitBuffer *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__vidmemAccessBitBufControlLookup__)(struct VidmemAccessBitBuffer *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NvHandle (*__vidmemAccessBitBufGetInternalObjectHandle__)(struct VidmemAccessBitBuffer *);
    NV_STATUS (*__vidmemAccessBitBufControl__)(struct VidmemAccessBitBuffer *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__vidmemAccessBitBufUnmap__)(struct VidmemAccessBitBuffer *, struct CALL_CONTEXT *, struct RsCpuMapping *);
    NV_STATUS (*__vidmemAccessBitBufGetMemInterMapParams__)(struct VidmemAccessBitBuffer *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__vidmemAccessBitBufGetMemoryMappingDescriptor__)(struct VidmemAccessBitBuffer *, struct MEMORY_DESCRIPTOR **);
    NV_STATUS (*__vidmemAccessBitBufControlFilter__)(struct VidmemAccessBitBuffer *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__vidmemAccessBitBufUnregisterEvent__)(struct VidmemAccessBitBuffer *, NvHandle, NvHandle, NvHandle, NvHandle);
    NV_STATUS (*__vidmemAccessBitBufControlSerialization_Prologue__)(struct VidmemAccessBitBuffer *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__vidmemAccessBitBufCanCopy__)(struct VidmemAccessBitBuffer *);
    void (*__vidmemAccessBitBufPreDestruct__)(struct VidmemAccessBitBuffer *);
    NV_STATUS (*__vidmemAccessBitBufIsDuplicate__)(struct VidmemAccessBitBuffer *, NvHandle, NvBool *);
    void (*__vidmemAccessBitBufControlSerialization_Epilogue__)(struct VidmemAccessBitBuffer *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    PEVENTNOTIFICATION *(*__vidmemAccessBitBufGetNotificationListPtr__)(struct VidmemAccessBitBuffer *);
    struct NotifShare *(*__vidmemAccessBitBufGetNotificationShare__)(struct VidmemAccessBitBuffer *);
    NV_STATUS (*__vidmemAccessBitBufMap__)(struct VidmemAccessBitBuffer *, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);
    NvBool (*__vidmemAccessBitBufAccessCallback__)(struct VidmemAccessBitBuffer *, struct RsClient *, void *, RsAccessRight);
};

#ifndef __NVOC_CLASS_VidmemAccessBitBuffer_TYPEDEF__
#define __NVOC_CLASS_VidmemAccessBitBuffer_TYPEDEF__
typedef struct VidmemAccessBitBuffer VidmemAccessBitBuffer;
#endif /* __NVOC_CLASS_VidmemAccessBitBuffer_TYPEDEF__ */

#ifndef __nvoc_class_id_VidmemAccessBitBuffer
#define __nvoc_class_id_VidmemAccessBitBuffer 0xebb6da
#endif /* __nvoc_class_id_VidmemAccessBitBuffer */

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

#define vidmemAccessBitBufCtrlCmdVidmemAccessBitDump(pVidmemAccessBitBuffer, pParams) vidmemAccessBitBufCtrlCmdVidmemAccessBitDump_DISPATCH(pVidmemAccessBitBuffer, pParams)
#define vidmemAccessBitBufShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) vidmemAccessBitBufShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define vidmemAccessBitBufCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) vidmemAccessBitBufCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define vidmemAccessBitBufGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) vidmemAccessBitBufGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)
#define vidmemAccessBitBufMapTo(pResource, pParams) vidmemAccessBitBufMapTo_DISPATCH(pResource, pParams)
#define vidmemAccessBitBufGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) vidmemAccessBitBufGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define vidmemAccessBitBufSetNotificationShare(pNotifier, pNotifShare) vidmemAccessBitBufSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define vidmemAccessBitBufGetRefCount(pResource) vidmemAccessBitBufGetRefCount_DISPATCH(pResource)
#define vidmemAccessBitBufAddAdditionalDependants(pClient, pResource, pReference) vidmemAccessBitBufAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define vidmemAccessBitBufControl_Prologue(pResource, pCallContext, pParams) vidmemAccessBitBufControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define vidmemAccessBitBufGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) vidmemAccessBitBufGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define vidmemAccessBitBufInternalControlForward(pGpuResource, command, pParams, size) vidmemAccessBitBufInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define vidmemAccessBitBufUnmapFrom(pResource, pParams) vidmemAccessBitBufUnmapFrom_DISPATCH(pResource, pParams)
#define vidmemAccessBitBufControl_Epilogue(pResource, pCallContext, pParams) vidmemAccessBitBufControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define vidmemAccessBitBufControlLookup(pResource, pParams, ppEntry) vidmemAccessBitBufControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define vidmemAccessBitBufGetInternalObjectHandle(pGpuResource) vidmemAccessBitBufGetInternalObjectHandle_DISPATCH(pGpuResource)
#define vidmemAccessBitBufControl(pGpuResource, pCallContext, pParams) vidmemAccessBitBufControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define vidmemAccessBitBufUnmap(pGpuResource, pCallContext, pCpuMapping) vidmemAccessBitBufUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define vidmemAccessBitBufGetMemInterMapParams(pRmResource, pParams) vidmemAccessBitBufGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define vidmemAccessBitBufGetMemoryMappingDescriptor(pRmResource, ppMemDesc) vidmemAccessBitBufGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define vidmemAccessBitBufControlFilter(pResource, pCallContext, pParams) vidmemAccessBitBufControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define vidmemAccessBitBufUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) vidmemAccessBitBufUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define vidmemAccessBitBufControlSerialization_Prologue(pResource, pCallContext, pParams) vidmemAccessBitBufControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define vidmemAccessBitBufCanCopy(pResource) vidmemAccessBitBufCanCopy_DISPATCH(pResource)
#define vidmemAccessBitBufPreDestruct(pResource) vidmemAccessBitBufPreDestruct_DISPATCH(pResource)
#define vidmemAccessBitBufIsDuplicate(pResource, hMemory, pDuplicate) vidmemAccessBitBufIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define vidmemAccessBitBufControlSerialization_Epilogue(pResource, pCallContext, pParams) vidmemAccessBitBufControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define vidmemAccessBitBufGetNotificationListPtr(pNotifier) vidmemAccessBitBufGetNotificationListPtr_DISPATCH(pNotifier)
#define vidmemAccessBitBufGetNotificationShare(pNotifier) vidmemAccessBitBufGetNotificationShare_DISPATCH(pNotifier)
#define vidmemAccessBitBufMap(pGpuResource, pCallContext, pParams, pCpuMapping) vidmemAccessBitBufMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define vidmemAccessBitBufAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) vidmemAccessBitBufAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
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

static inline NV_STATUS vidmemAccessBitBufCtrlCmdVidmemAccessBitDump_DISPATCH(struct VidmemAccessBitBuffer *pVidmemAccessBitBuffer, NVC763_CTRL_VIDMEM_ACCESS_BIT_BUFFER_DUMP_PARAMS *pParams) {
    return pVidmemAccessBitBuffer->__vidmemAccessBitBufCtrlCmdVidmemAccessBitDump__(pVidmemAccessBitBuffer, pParams);
}

static inline NvBool vidmemAccessBitBufShareCallback_DISPATCH(struct VidmemAccessBitBuffer *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__vidmemAccessBitBufShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS vidmemAccessBitBufCheckMemInterUnmap_DISPATCH(struct VidmemAccessBitBuffer *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__vidmemAccessBitBufCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS vidmemAccessBitBufGetOrAllocNotifShare_DISPATCH(struct VidmemAccessBitBuffer *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__vidmemAccessBitBufGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

static inline NV_STATUS vidmemAccessBitBufMapTo_DISPATCH(struct VidmemAccessBitBuffer *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__vidmemAccessBitBufMapTo__(pResource, pParams);
}

static inline NV_STATUS vidmemAccessBitBufGetMapAddrSpace_DISPATCH(struct VidmemAccessBitBuffer *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__vidmemAccessBitBufGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline void vidmemAccessBitBufSetNotificationShare_DISPATCH(struct VidmemAccessBitBuffer *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__vidmemAccessBitBufSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NvU32 vidmemAccessBitBufGetRefCount_DISPATCH(struct VidmemAccessBitBuffer *pResource) {
    return pResource->__vidmemAccessBitBufGetRefCount__(pResource);
}

static inline void vidmemAccessBitBufAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct VidmemAccessBitBuffer *pResource, RsResourceRef *pReference) {
    pResource->__vidmemAccessBitBufAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS vidmemAccessBitBufControl_Prologue_DISPATCH(struct VidmemAccessBitBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__vidmemAccessBitBufControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS vidmemAccessBitBufGetRegBaseOffsetAndSize_DISPATCH(struct VidmemAccessBitBuffer *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__vidmemAccessBitBufGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS vidmemAccessBitBufInternalControlForward_DISPATCH(struct VidmemAccessBitBuffer *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__vidmemAccessBitBufInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NV_STATUS vidmemAccessBitBufUnmapFrom_DISPATCH(struct VidmemAccessBitBuffer *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__vidmemAccessBitBufUnmapFrom__(pResource, pParams);
}

static inline void vidmemAccessBitBufControl_Epilogue_DISPATCH(struct VidmemAccessBitBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__vidmemAccessBitBufControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS vidmemAccessBitBufControlLookup_DISPATCH(struct VidmemAccessBitBuffer *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__vidmemAccessBitBufControlLookup__(pResource, pParams, ppEntry);
}

static inline NvHandle vidmemAccessBitBufGetInternalObjectHandle_DISPATCH(struct VidmemAccessBitBuffer *pGpuResource) {
    return pGpuResource->__vidmemAccessBitBufGetInternalObjectHandle__(pGpuResource);
}

static inline NV_STATUS vidmemAccessBitBufControl_DISPATCH(struct VidmemAccessBitBuffer *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__vidmemAccessBitBufControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS vidmemAccessBitBufUnmap_DISPATCH(struct VidmemAccessBitBuffer *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__vidmemAccessBitBufUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NV_STATUS vidmemAccessBitBufGetMemInterMapParams_DISPATCH(struct VidmemAccessBitBuffer *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__vidmemAccessBitBufGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS vidmemAccessBitBufGetMemoryMappingDescriptor_DISPATCH(struct VidmemAccessBitBuffer *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__vidmemAccessBitBufGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS vidmemAccessBitBufControlFilter_DISPATCH(struct VidmemAccessBitBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__vidmemAccessBitBufControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS vidmemAccessBitBufUnregisterEvent_DISPATCH(struct VidmemAccessBitBuffer *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__vidmemAccessBitBufUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS vidmemAccessBitBufControlSerialization_Prologue_DISPATCH(struct VidmemAccessBitBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__vidmemAccessBitBufControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool vidmemAccessBitBufCanCopy_DISPATCH(struct VidmemAccessBitBuffer *pResource) {
    return pResource->__vidmemAccessBitBufCanCopy__(pResource);
}

static inline void vidmemAccessBitBufPreDestruct_DISPATCH(struct VidmemAccessBitBuffer *pResource) {
    pResource->__vidmemAccessBitBufPreDestruct__(pResource);
}

static inline NV_STATUS vidmemAccessBitBufIsDuplicate_DISPATCH(struct VidmemAccessBitBuffer *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__vidmemAccessBitBufIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void vidmemAccessBitBufControlSerialization_Epilogue_DISPATCH(struct VidmemAccessBitBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__vidmemAccessBitBufControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline PEVENTNOTIFICATION *vidmemAccessBitBufGetNotificationListPtr_DISPATCH(struct VidmemAccessBitBuffer *pNotifier) {
    return pNotifier->__vidmemAccessBitBufGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare *vidmemAccessBitBufGetNotificationShare_DISPATCH(struct VidmemAccessBitBuffer *pNotifier) {
    return pNotifier->__vidmemAccessBitBufGetNotificationShare__(pNotifier);
}

static inline NV_STATUS vidmemAccessBitBufMap_DISPATCH(struct VidmemAccessBitBuffer *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__vidmemAccessBitBufMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NvBool vidmemAccessBitBufAccessCallback_DISPATCH(struct VidmemAccessBitBuffer *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__vidmemAccessBitBufAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS __nvoc_vidmemAccessBitBufConstruct(struct VidmemAccessBitBuffer *arg_pVidmemAccessBitBuffer, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams) {
    return vidmemAccessBitBufConstructHelper(arg_pVidmemAccessBitBuffer, arg_pCallContext, arg_pParams);
}

#undef PRIVATE_FIELD


#endif // VIDMEM_ACCESS_BIT_BUFFER_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_VIDMEM_ACCESS_BIT_BUFFER_NVOC_H_
