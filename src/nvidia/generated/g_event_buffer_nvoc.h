
#ifndef _G_EVENT_BUFFER_NVOC_H_
#define _G_EVENT_BUFFER_NVOC_H_
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
#pragma once
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

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_EVENT_BUFFER_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct EventBuffer {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct RmResource __nvoc_base_RmResource;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^3
    struct RsResource *__nvoc_pbase_RsResource;    // res super^2
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^2
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super
    struct EventBuffer *__nvoc_pbase_EventBuffer;    // eventbuffer

    // Vtable with 25 per-object function pointers
    NV_STATUS (*__eventbuffertBufferCtrlCmdEnableEvent__)(struct EventBuffer * /*this*/, NV_EVENT_BUFFER_CTRL_CMD_ENABLE_EVENTS_PARAMS *);  // exported (id=0x90cd0101)
    NV_STATUS (*__eventbuffertBufferCtrlCmdUpdateGet__)(struct EventBuffer * /*this*/, NV_EVENT_BUFFER_CTRL_CMD_UPDATE_GET_PARAMS *);  // exported (id=0x90cd0102)
    NV_STATUS (*__eventbuffertBufferCtrlCmdFlush__)(struct EventBuffer * /*this*/);  // exported (id=0x90cd0104)
    NV_STATUS (*__eventbuffertBufferCtrlCmdPostTelemetryEvent__)(struct EventBuffer * /*this*/, NV_EVENT_BUFFER_CTRL_CMD_POST_TELEMETRY_EVENT_PARAMS *);  // exported (id=0x90cd0105)
    NvBool (*__eventbufferAccessCallback__)(struct EventBuffer * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (rmres)
    NvBool (*__eventbufferShareCallback__)(struct EventBuffer * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__eventbufferGetMemInterMapParams__)(struct EventBuffer * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__eventbufferCheckMemInterUnmap__)(struct EventBuffer * /*this*/, NvBool);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__eventbufferGetMemoryMappingDescriptor__)(struct EventBuffer * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__eventbufferControlSerialization_Prologue__)(struct EventBuffer * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (rmres)
    void (*__eventbufferControlSerialization_Epilogue__)(struct EventBuffer * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__eventbufferControl_Prologue__)(struct EventBuffer * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (rmres)
    void (*__eventbufferControl_Epilogue__)(struct EventBuffer * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (rmres)
    NvBool (*__eventbufferCanCopy__)(struct EventBuffer * /*this*/);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__eventbufferIsDuplicate__)(struct EventBuffer * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (rmres)
    void (*__eventbufferPreDestruct__)(struct EventBuffer * /*this*/);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__eventbufferControl__)(struct EventBuffer * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__eventbufferControlFilter__)(struct EventBuffer * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__eventbufferMap__)(struct EventBuffer * /*this*/, struct CALL_CONTEXT *, RS_CPU_MAP_PARAMS *, RsCpuMapping *);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__eventbufferUnmap__)(struct EventBuffer * /*this*/, struct CALL_CONTEXT *, RsCpuMapping *);  // virtual inherited (res) base (rmres)
    NvBool (*__eventbufferIsPartialUnmapSupported__)(struct EventBuffer * /*this*/);  // inline virtual inherited (res) base (rmres) body
    NV_STATUS (*__eventbufferMapTo__)(struct EventBuffer * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__eventbufferUnmapFrom__)(struct EventBuffer * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (rmres)
    NvU32 (*__eventbufferGetRefCount__)(struct EventBuffer * /*this*/);  // virtual inherited (res) base (rmres)
    void (*__eventbufferAddAdditionalDependants__)(struct RsClient *, struct EventBuffer * /*this*/, RsResourceRef *);  // virtual inherited (res) base (rmres)

    // Data members
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

// Casting support
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


// Wrapper macros
#define eventbuffertBufferCtrlCmdEnableEvent_FNPTR(pEventBuffer) pEventBuffer->__eventbuffertBufferCtrlCmdEnableEvent__
#define eventbuffertBufferCtrlCmdEnableEvent(pEventBuffer, pEnableParams) eventbuffertBufferCtrlCmdEnableEvent_DISPATCH(pEventBuffer, pEnableParams)
#define eventbuffertBufferCtrlCmdUpdateGet_FNPTR(pEventBuffer) pEventBuffer->__eventbuffertBufferCtrlCmdUpdateGet__
#define eventbuffertBufferCtrlCmdUpdateGet(pEventBuffer, pUpdateParams) eventbuffertBufferCtrlCmdUpdateGet_DISPATCH(pEventBuffer, pUpdateParams)
#define eventbuffertBufferCtrlCmdFlush_FNPTR(pEventBuffer) pEventBuffer->__eventbuffertBufferCtrlCmdFlush__
#define eventbuffertBufferCtrlCmdFlush(pEventBuffer) eventbuffertBufferCtrlCmdFlush_DISPATCH(pEventBuffer)
#define eventbuffertBufferCtrlCmdPostTelemetryEvent_FNPTR(pEventBuffer) pEventBuffer->__eventbuffertBufferCtrlCmdPostTelemetryEvent__
#define eventbuffertBufferCtrlCmdPostTelemetryEvent(pEventBuffer, pPostTelemetryEvent) eventbuffertBufferCtrlCmdPostTelemetryEvent_DISPATCH(pEventBuffer, pPostTelemetryEvent)
#define eventbufferAccessCallback_FNPTR(pResource) pResource->__nvoc_base_RmResource.__rmresAccessCallback__
#define eventbufferAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) eventbufferAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define eventbufferShareCallback_FNPTR(pResource) pResource->__nvoc_base_RmResource.__rmresShareCallback__
#define eventbufferShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) eventbufferShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define eventbufferGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__rmresGetMemInterMapParams__
#define eventbufferGetMemInterMapParams(pRmResource, pParams) eventbufferGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define eventbufferCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__rmresCheckMemInterUnmap__
#define eventbufferCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) eventbufferCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define eventbufferGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__rmresGetMemoryMappingDescriptor__
#define eventbufferGetMemoryMappingDescriptor(pRmResource, ppMemDesc) eventbufferGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define eventbufferControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__rmresControlSerialization_Prologue__
#define eventbufferControlSerialization_Prologue(pResource, pCallContext, pParams) eventbufferControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define eventbufferControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__rmresControlSerialization_Epilogue__
#define eventbufferControlSerialization_Epilogue(pResource, pCallContext, pParams) eventbufferControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define eventbufferControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__rmresControl_Prologue__
#define eventbufferControl_Prologue(pResource, pCallContext, pParams) eventbufferControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define eventbufferControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__rmresControl_Epilogue__
#define eventbufferControl_Epilogue(pResource, pCallContext, pParams) eventbufferControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define eventbufferCanCopy_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__
#define eventbufferCanCopy(pResource) eventbufferCanCopy_DISPATCH(pResource)
#define eventbufferIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsDuplicate__
#define eventbufferIsDuplicate(pResource, hMemory, pDuplicate) eventbufferIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define eventbufferPreDestruct_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resPreDestruct__
#define eventbufferPreDestruct(pResource) eventbufferPreDestruct_DISPATCH(pResource)
#define eventbufferControl_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resControl__
#define eventbufferControl(pResource, pCallContext, pParams) eventbufferControl_DISPATCH(pResource, pCallContext, pParams)
#define eventbufferControlFilter_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resControlFilter__
#define eventbufferControlFilter(pResource, pCallContext, pParams) eventbufferControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define eventbufferMap_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resMap__
#define eventbufferMap(pResource, pCallContext, pParams, pCpuMapping) eventbufferMap_DISPATCH(pResource, pCallContext, pParams, pCpuMapping)
#define eventbufferUnmap_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmap__
#define eventbufferUnmap(pResource, pCallContext, pCpuMapping) eventbufferUnmap_DISPATCH(pResource, pCallContext, pCpuMapping)
#define eventbufferIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsPartialUnmapSupported__
#define eventbufferIsPartialUnmapSupported(pResource) eventbufferIsPartialUnmapSupported_DISPATCH(pResource)
#define eventbufferMapTo_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resMapTo__
#define eventbufferMapTo(pResource, pParams) eventbufferMapTo_DISPATCH(pResource, pParams)
#define eventbufferUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmapFrom__
#define eventbufferUnmapFrom(pResource, pParams) eventbufferUnmapFrom_DISPATCH(pResource, pParams)
#define eventbufferGetRefCount_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resGetRefCount__
#define eventbufferGetRefCount(pResource) eventbufferGetRefCount_DISPATCH(pResource)
#define eventbufferAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resAddAdditionalDependants__
#define eventbufferAddAdditionalDependants(pClient, pResource, pReference) eventbufferAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NV_STATUS eventbuffertBufferCtrlCmdEnableEvent_DISPATCH(struct EventBuffer *pEventBuffer, NV_EVENT_BUFFER_CTRL_CMD_ENABLE_EVENTS_PARAMS *pEnableParams) {
    return pEventBuffer->__eventbuffertBufferCtrlCmdEnableEvent__(pEventBuffer, pEnableParams);
}

static inline NV_STATUS eventbuffertBufferCtrlCmdUpdateGet_DISPATCH(struct EventBuffer *pEventBuffer, NV_EVENT_BUFFER_CTRL_CMD_UPDATE_GET_PARAMS *pUpdateParams) {
    return pEventBuffer->__eventbuffertBufferCtrlCmdUpdateGet__(pEventBuffer, pUpdateParams);
}

static inline NV_STATUS eventbuffertBufferCtrlCmdFlush_DISPATCH(struct EventBuffer *pEventBuffer) {
    return pEventBuffer->__eventbuffertBufferCtrlCmdFlush__(pEventBuffer);
}

static inline NV_STATUS eventbuffertBufferCtrlCmdPostTelemetryEvent_DISPATCH(struct EventBuffer *pEventBuffer, NV_EVENT_BUFFER_CTRL_CMD_POST_TELEMETRY_EVENT_PARAMS *pPostTelemetryEvent) {
    return pEventBuffer->__eventbuffertBufferCtrlCmdPostTelemetryEvent__(pEventBuffer, pPostTelemetryEvent);
}

static inline NvBool eventbufferAccessCallback_DISPATCH(struct EventBuffer *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__eventbufferAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool eventbufferShareCallback_DISPATCH(struct EventBuffer *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__eventbufferShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS eventbufferGetMemInterMapParams_DISPATCH(struct EventBuffer *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__eventbufferGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS eventbufferCheckMemInterUnmap_DISPATCH(struct EventBuffer *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__eventbufferCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS eventbufferGetMemoryMappingDescriptor_DISPATCH(struct EventBuffer *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__eventbufferGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS eventbufferControlSerialization_Prologue_DISPATCH(struct EventBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__eventbufferControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void eventbufferControlSerialization_Epilogue_DISPATCH(struct EventBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__eventbufferControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS eventbufferControl_Prologue_DISPATCH(struct EventBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__eventbufferControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void eventbufferControl_Epilogue_DISPATCH(struct EventBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__eventbufferControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool eventbufferCanCopy_DISPATCH(struct EventBuffer *pResource) {
    return pResource->__eventbufferCanCopy__(pResource);
}

static inline NV_STATUS eventbufferIsDuplicate_DISPATCH(struct EventBuffer *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__eventbufferIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void eventbufferPreDestruct_DISPATCH(struct EventBuffer *pResource) {
    pResource->__eventbufferPreDestruct__(pResource);
}

static inline NV_STATUS eventbufferControl_DISPATCH(struct EventBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__eventbufferControl__(pResource, pCallContext, pParams);
}

static inline NV_STATUS eventbufferControlFilter_DISPATCH(struct EventBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__eventbufferControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS eventbufferMap_DISPATCH(struct EventBuffer *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pResource->__eventbufferMap__(pResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS eventbufferUnmap_DISPATCH(struct EventBuffer *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pResource->__eventbufferUnmap__(pResource, pCallContext, pCpuMapping);
}

static inline NvBool eventbufferIsPartialUnmapSupported_DISPATCH(struct EventBuffer *pResource) {
    return pResource->__eventbufferIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS eventbufferMapTo_DISPATCH(struct EventBuffer *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__eventbufferMapTo__(pResource, pParams);
}

static inline NV_STATUS eventbufferUnmapFrom_DISPATCH(struct EventBuffer *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__eventbufferUnmapFrom__(pResource, pParams);
}

static inline NvU32 eventbufferGetRefCount_DISPATCH(struct EventBuffer *pResource) {
    return pResource->__eventbufferGetRefCount__(pResource);
}

static inline void eventbufferAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct EventBuffer *pResource, RsResourceRef *pReference) {
    pResource->__eventbufferAddAdditionalDependants__(pClient, pResource, pReference);
}

NV_STATUS eventbuffertBufferCtrlCmdEnableEvent_IMPL(struct EventBuffer *pEventBuffer, NV_EVENT_BUFFER_CTRL_CMD_ENABLE_EVENTS_PARAMS *pEnableParams);

NV_STATUS eventbuffertBufferCtrlCmdUpdateGet_IMPL(struct EventBuffer *pEventBuffer, NV_EVENT_BUFFER_CTRL_CMD_UPDATE_GET_PARAMS *pUpdateParams);

NV_STATUS eventbuffertBufferCtrlCmdFlush_IMPL(struct EventBuffer *pEventBuffer);

NV_STATUS eventbuffertBufferCtrlCmdPostTelemetryEvent_IMPL(struct EventBuffer *pEventBuffer, NV_EVENT_BUFFER_CTRL_CMD_POST_TELEMETRY_EVENT_PARAMS *pPostTelemetryEvent);

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
