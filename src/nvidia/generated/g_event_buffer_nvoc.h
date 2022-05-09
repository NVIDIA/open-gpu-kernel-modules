#ifndef _G_EVENT_BUFFER_NVOC_H_
#define _G_EVENT_BUFFER_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_event_buffer_nvoc.h"

#ifndef _EVENT_BUFFER_H_
#define _EVENT_BUFFER_H_

#include "resserv/resserv.h"
#include "nvoc/prelude.h"
#include "rmapi/event.h"
#include "rmapi/resource.h"
#include "ctrl/ctrl90cd.h"
#include "eventbufferproducer.h"

struct Memory;

#ifndef __NVOC_CLASS_Memory_TYPEDEF__
#define __NVOC_CLASS_Memory_TYPEDEF__
typedef struct Memory Memory;
#endif /* __NVOC_CLASS_Memory_TYPEDEF__ */

#ifndef __nvoc_class_id_Memory
#define __nvoc_class_id_Memory 0x4789f2
#endif /* __nvoc_class_id_Memory */



typedef struct
{
    //
    // Addr: user RO address
    // Priv: return cookie to be passed to unmap
    //
    NvP64                   headerAddr;
    NvP64                   headerPriv;
    NvP64                   recordBuffAddr;
    NvP64                   recordBuffPriv;
    NvP64                   vardataBuffAddr;
    NvP64                   vardataBuffPriv;
} EVENT_BUFFER_MAP_INFO;

// This class shares buffers between kernel and usermode
#ifdef NVOC_EVENT_BUFFER_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct EventBuffer {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct RmResource __nvoc_base_RmResource;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct EventBuffer *__nvoc_pbase_EventBuffer;
    NV_STATUS (*__eventbuffertBufferCtrlCmdEnableEvent__)(struct EventBuffer *, NV_EVENT_BUFFER_CTRL_CMD_ENABLE_EVENTS_PARAMS *);
    NV_STATUS (*__eventbuffertBufferCtrlCmdUpdateGet__)(struct EventBuffer *, NV_EVENT_BUFFER_CTRL_CMD_UPDATE_GET_PARAMS *);
    NV_STATUS (*__eventbuffertBufferCtrlCmdFlush__)(struct EventBuffer *);
    NV_STATUS (*__eventbuffertBufferCtrlCmdPostTelemetryEvent__)(struct EventBuffer *, NV_EVENT_BUFFER_CTRL_CMD_POST_TELEMETRY_EVENT_PARAMS *);
    NvBool (*__eventbufferShareCallback__)(struct EventBuffer *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__eventbufferCheckMemInterUnmap__)(struct EventBuffer *, NvBool);
    NV_STATUS (*__eventbufferControl__)(struct EventBuffer *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__eventbufferGetMemInterMapParams__)(struct EventBuffer *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__eventbufferGetMemoryMappingDescriptor__)(struct EventBuffer *, struct MEMORY_DESCRIPTOR **);
    NvU32 (*__eventbufferGetRefCount__)(struct EventBuffer *);
    NV_STATUS (*__eventbufferControlFilter__)(struct EventBuffer *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    void (*__eventbufferAddAdditionalDependants__)(struct RsClient *, struct EventBuffer *, RsResourceRef *);
    NV_STATUS (*__eventbufferUnmap__)(struct EventBuffer *, struct CALL_CONTEXT *, RsCpuMapping *);
    NV_STATUS (*__eventbufferControl_Prologue__)(struct EventBuffer *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__eventbufferCanCopy__)(struct EventBuffer *);
    NV_STATUS (*__eventbufferMapTo__)(struct EventBuffer *, RS_RES_MAP_TO_PARAMS *);
    void (*__eventbufferPreDestruct__)(struct EventBuffer *);
    NV_STATUS (*__eventbufferUnmapFrom__)(struct EventBuffer *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__eventbufferControl_Epilogue__)(struct EventBuffer *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__eventbufferControlLookup__)(struct EventBuffer *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NV_STATUS (*__eventbufferMap__)(struct EventBuffer *, struct CALL_CONTEXT *, RS_CPU_MAP_PARAMS *, RsCpuMapping *);
    NvBool (*__eventbufferAccessCallback__)(struct EventBuffer *, struct RsClient *, void *, RsAccessRight);
    struct MEMORY_DESCRIPTOR *pHeaderDesc;
    struct MEMORY_DESCRIPTOR *pRecordBufDesc;
    struct MEMORY_DESCRIPTOR *pVardataBufDesc;
    NvHandle hSubDevice;
    NvU32 subDeviceInst;
    EVENT_BUFFER_MAP_INFO kernelMapInfo;
    EVENT_BUFFER_MAP_INFO clientMapInfo;
    NvHandle hClient;
    NvU16 seqNo;
    NvBool bNotifyPending;
    PEVENTNOTIFICATION pListeners;
    EVENT_BUFFER_PRODUCER_INFO producerInfo;
    struct Memory *pHeader;
    struct Memory *pRecord;
    struct Memory *pVardata;
    NvHandle hInternalClient;
    NvHandle hInternalDevice;
    NvHandle hInternalSubdevice;
    NvHandle hInternalHeader;
    NvHandle hInternalBuffer;
};

#ifndef __NVOC_CLASS_EventBuffer_TYPEDEF__
#define __NVOC_CLASS_EventBuffer_TYPEDEF__
typedef struct EventBuffer EventBuffer;
#endif /* __NVOC_CLASS_EventBuffer_TYPEDEF__ */

#ifndef __nvoc_class_id_EventBuffer
#define __nvoc_class_id_EventBuffer 0x63502b
#endif /* __nvoc_class_id_EventBuffer */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_EventBuffer;

#define __staticCast_EventBuffer(pThis) \
    ((pThis)->__nvoc_pbase_EventBuffer)

#ifdef __nvoc_event_buffer_h_disabled
#define __dynamicCast_EventBuffer(pThis) ((EventBuffer*)NULL)
#else //__nvoc_event_buffer_h_disabled
#define __dynamicCast_EventBuffer(pThis) \
    ((EventBuffer*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(EventBuffer)))
#endif //__nvoc_event_buffer_h_disabled


NV_STATUS __nvoc_objCreateDynamic_EventBuffer(EventBuffer**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_EventBuffer(EventBuffer**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_EventBuffer(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_EventBuffer((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define eventbuffertBufferCtrlCmdEnableEvent(pEventBuffer, pEnableParams) eventbuffertBufferCtrlCmdEnableEvent_DISPATCH(pEventBuffer, pEnableParams)
#define eventbuffertBufferCtrlCmdUpdateGet(pEventBuffer, pUpdateParams) eventbuffertBufferCtrlCmdUpdateGet_DISPATCH(pEventBuffer, pUpdateParams)
#define eventbuffertBufferCtrlCmdFlush(pEventBuffer) eventbuffertBufferCtrlCmdFlush_DISPATCH(pEventBuffer)
#define eventbuffertBufferCtrlCmdPostTelemetryEvent(pEventBuffer, pPostTelemetryEvent) eventbuffertBufferCtrlCmdPostTelemetryEvent_DISPATCH(pEventBuffer, pPostTelemetryEvent)
#define eventbufferShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) eventbufferShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define eventbufferCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) eventbufferCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define eventbufferControl(pResource, pCallContext, pParams) eventbufferControl_DISPATCH(pResource, pCallContext, pParams)
#define eventbufferGetMemInterMapParams(pRmResource, pParams) eventbufferGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define eventbufferGetMemoryMappingDescriptor(pRmResource, ppMemDesc) eventbufferGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define eventbufferGetRefCount(pResource) eventbufferGetRefCount_DISPATCH(pResource)
#define eventbufferControlFilter(pResource, pCallContext, pParams) eventbufferControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define eventbufferAddAdditionalDependants(pClient, pResource, pReference) eventbufferAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define eventbufferUnmap(pResource, pCallContext, pCpuMapping) eventbufferUnmap_DISPATCH(pResource, pCallContext, pCpuMapping)
#define eventbufferControl_Prologue(pResource, pCallContext, pParams) eventbufferControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define eventbufferCanCopy(pResource) eventbufferCanCopy_DISPATCH(pResource)
#define eventbufferMapTo(pResource, pParams) eventbufferMapTo_DISPATCH(pResource, pParams)
#define eventbufferPreDestruct(pResource) eventbufferPreDestruct_DISPATCH(pResource)
#define eventbufferUnmapFrom(pResource, pParams) eventbufferUnmapFrom_DISPATCH(pResource, pParams)
#define eventbufferControl_Epilogue(pResource, pCallContext, pParams) eventbufferControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define eventbufferControlLookup(pResource, pParams, ppEntry) eventbufferControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define eventbufferMap(pResource, pCallContext, pParams, pCpuMapping) eventbufferMap_DISPATCH(pResource, pCallContext, pParams, pCpuMapping)
#define eventbufferAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) eventbufferAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
NV_STATUS eventbuffertBufferCtrlCmdEnableEvent_IMPL(struct EventBuffer *pEventBuffer, NV_EVENT_BUFFER_CTRL_CMD_ENABLE_EVENTS_PARAMS *pEnableParams);

static inline NV_STATUS eventbuffertBufferCtrlCmdEnableEvent_DISPATCH(struct EventBuffer *pEventBuffer, NV_EVENT_BUFFER_CTRL_CMD_ENABLE_EVENTS_PARAMS *pEnableParams) {
    return pEventBuffer->__eventbuffertBufferCtrlCmdEnableEvent__(pEventBuffer, pEnableParams);
}

NV_STATUS eventbuffertBufferCtrlCmdUpdateGet_IMPL(struct EventBuffer *pEventBuffer, NV_EVENT_BUFFER_CTRL_CMD_UPDATE_GET_PARAMS *pUpdateParams);

static inline NV_STATUS eventbuffertBufferCtrlCmdUpdateGet_DISPATCH(struct EventBuffer *pEventBuffer, NV_EVENT_BUFFER_CTRL_CMD_UPDATE_GET_PARAMS *pUpdateParams) {
    return pEventBuffer->__eventbuffertBufferCtrlCmdUpdateGet__(pEventBuffer, pUpdateParams);
}

NV_STATUS eventbuffertBufferCtrlCmdFlush_IMPL(struct EventBuffer *pEventBuffer);

static inline NV_STATUS eventbuffertBufferCtrlCmdFlush_DISPATCH(struct EventBuffer *pEventBuffer) {
    return pEventBuffer->__eventbuffertBufferCtrlCmdFlush__(pEventBuffer);
}

NV_STATUS eventbuffertBufferCtrlCmdPostTelemetryEvent_IMPL(struct EventBuffer *pEventBuffer, NV_EVENT_BUFFER_CTRL_CMD_POST_TELEMETRY_EVENT_PARAMS *pPostTelemetryEvent);

static inline NV_STATUS eventbuffertBufferCtrlCmdPostTelemetryEvent_DISPATCH(struct EventBuffer *pEventBuffer, NV_EVENT_BUFFER_CTRL_CMD_POST_TELEMETRY_EVENT_PARAMS *pPostTelemetryEvent) {
    return pEventBuffer->__eventbuffertBufferCtrlCmdPostTelemetryEvent__(pEventBuffer, pPostTelemetryEvent);
}

static inline NvBool eventbufferShareCallback_DISPATCH(struct EventBuffer *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__eventbufferShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS eventbufferCheckMemInterUnmap_DISPATCH(struct EventBuffer *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__eventbufferCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS eventbufferControl_DISPATCH(struct EventBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__eventbufferControl__(pResource, pCallContext, pParams);
}

static inline NV_STATUS eventbufferGetMemInterMapParams_DISPATCH(struct EventBuffer *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__eventbufferGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS eventbufferGetMemoryMappingDescriptor_DISPATCH(struct EventBuffer *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__eventbufferGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NvU32 eventbufferGetRefCount_DISPATCH(struct EventBuffer *pResource) {
    return pResource->__eventbufferGetRefCount__(pResource);
}

static inline NV_STATUS eventbufferControlFilter_DISPATCH(struct EventBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__eventbufferControlFilter__(pResource, pCallContext, pParams);
}

static inline void eventbufferAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct EventBuffer *pResource, RsResourceRef *pReference) {
    pResource->__eventbufferAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS eventbufferUnmap_DISPATCH(struct EventBuffer *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pResource->__eventbufferUnmap__(pResource, pCallContext, pCpuMapping);
}

static inline NV_STATUS eventbufferControl_Prologue_DISPATCH(struct EventBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__eventbufferControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool eventbufferCanCopy_DISPATCH(struct EventBuffer *pResource) {
    return pResource->__eventbufferCanCopy__(pResource);
}

static inline NV_STATUS eventbufferMapTo_DISPATCH(struct EventBuffer *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__eventbufferMapTo__(pResource, pParams);
}

static inline void eventbufferPreDestruct_DISPATCH(struct EventBuffer *pResource) {
    pResource->__eventbufferPreDestruct__(pResource);
}

static inline NV_STATUS eventbufferUnmapFrom_DISPATCH(struct EventBuffer *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__eventbufferUnmapFrom__(pResource, pParams);
}

static inline void eventbufferControl_Epilogue_DISPATCH(struct EventBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__eventbufferControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS eventbufferControlLookup_DISPATCH(struct EventBuffer *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__eventbufferControlLookup__(pResource, pParams, ppEntry);
}

static inline NV_STATUS eventbufferMap_DISPATCH(struct EventBuffer *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pResource->__eventbufferMap__(pResource, pCallContext, pParams, pCpuMapping);
}

static inline NvBool eventbufferAccessCallback_DISPATCH(struct EventBuffer *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__eventbufferAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

NV_STATUS eventbufferConstruct_IMPL(struct EventBuffer *arg_pEventBuffer, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __nvoc_eventbufferConstruct(arg_pEventBuffer, arg_pCallContext, arg_pParams) eventbufferConstruct_IMPL(arg_pEventBuffer, arg_pCallContext, arg_pParams)
void eventbufferDestruct_IMPL(struct EventBuffer *pEventBuffer);
#define __nvoc_eventbufferDestruct(pEventBuffer) eventbufferDestruct_IMPL(pEventBuffer)
#undef PRIVATE_FIELD


NV_STATUS eventBufferAdd(struct EventBuffer *pEventBuffer, void* pEventData, NvU32 recordType, NvBool* bNotify, NvP64 *pHandle);

#endif

#ifdef __cplusplus
} // extern "C"
#endif
#endif // _G_EVENT_BUFFER_NVOC_H_
