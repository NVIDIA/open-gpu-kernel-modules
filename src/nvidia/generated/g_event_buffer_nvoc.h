
#ifndef _G_EVENT_BUFFER_NVOC_H_
#define _G_EVENT_BUFFER_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2016-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "rmapi/event_api.h"
#include "rmapi/resource.h"
#include "ctrl/ctrl90cd.h"
#include "eventbufferproducer.h"

struct Memory;

#ifndef __nvoc_class_id_Memory
#define __nvoc_class_id_Memory 0x4789f2u
typedef struct Memory Memory;
#endif /* __nvoc_class_id_Memory */


struct OBJGPU;

#ifndef __nvoc_class_id_OBJGPU
#define __nvoc_class_id_OBJGPU 0x7ef3cbu
typedef struct OBJGPU OBJGPU;
#endif /* __nvoc_class_id_OBJGPU */



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

//
// Producer state.
//
// An EventBuffer may have one producer-owned state object. EventBuffer
// stores the embedded base pointer and invokes destroy before listener
// teardown and memory unmap so the producer can unlink any listener
// entries it owns.
//
struct EventBuffer;
struct EventBufferProducerState;

typedef void (*EventBufferProducerDestroyCb)(
                  struct EventBuffer *pBuf,
                  struct EventBufferProducerState *pState);

typedef struct EventBufferProducerState
{
    EventBufferProducerDestroyCb destroy;
} EventBufferProducerState;

// This class shares buffers between kernel and usermode

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_EVENT_BUFFER_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__EventBuffer;
struct NVOC_METADATA__RmResource;
struct NVOC_VTABLE__EventBuffer;


struct EventBuffer {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__EventBuffer *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct RmResource __nvoc_base_RmResource;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^3
    struct RsResource *__nvoc_pbase_RsResource;    // res super^2
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^2
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super
    struct EventBuffer *__nvoc_pbase_EventBuffer;    // eventbuffer

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
    EventBufferProducerState *pProducerState;
    NvHandle hInternalClient;
    NvHandle hInternalDevice;
    NvHandle hInternalSubdevice;
    NvHandle hInternalHeader;
    NvHandle hInternalBuffer;
};


// Vtable with 21 per-class function pointers
struct NVOC_VTABLE__EventBuffer {
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
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__EventBuffer {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__RmResource metadata__RmResource;
    const struct NVOC_VTABLE__EventBuffer vtable;
};

#ifndef __nvoc_class_id_EventBuffer
#define __nvoc_class_id_EventBuffer 0x63502bu
typedef struct EventBuffer EventBuffer;
#endif /* __nvoc_class_id_EventBuffer */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_EventBuffer;

#define __staticCast_EventBuffer(pThis) \
    ((pThis)->__nvoc_pbase_EventBuffer)

#ifdef __nvoc_event_buffer_h_disabled
#define __dynamicCast_EventBuffer(pThis) ((EventBuffer*) NULL)
#else //__nvoc_event_buffer_h_disabled
#define __dynamicCast_EventBuffer(pThis) \
    ((EventBuffer*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(EventBuffer)))
#endif //__nvoc_event_buffer_h_disabled

NV_STATUS __nvoc_objCreateDynamic_EventBuffer(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_EventBuffer(EventBuffer**, Dynamic*, NvU32, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __objCreate_EventBuffer(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams) \
    __nvoc_objCreate_EventBuffer((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pCallContext, pParams)


// Wrapper macros for implementation functions
NV_STATUS eventbufferConstruct_IMPL(struct EventBuffer *pEventBuffer, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __nvoc_eventbufferConstruct(pEventBuffer, pCallContext, pParams) eventbufferConstruct_IMPL(pEventBuffer, pCallContext, pParams)

void eventbufferDestruct_IMPL(struct EventBuffer *pEventBuffer);
#define __nvoc_eventbufferDestruct(pEventBuffer) eventbufferDestruct_IMPL(pEventBuffer)

NV_STATUS eventbufferAddWithSubtype_IMPL(struct EventBuffer *pEventBuffer, EVENT_BUFFER_PRODUCER_DATA *pEventData, NvU32 recordType, NvU32 recordSubtype, NvBool *pBNotify, NvP64 *pHandle);
#ifdef __nvoc_event_buffer_h_disabled
static inline NV_STATUS eventbufferAddWithSubtype(struct EventBuffer *pEventBuffer, EVENT_BUFFER_PRODUCER_DATA *pEventData, NvU32 recordType, NvU32 recordSubtype, NvBool *pBNotify, NvP64 *pHandle) {
    NV_ASSERT_FAILED_PRECOMP("EventBuffer was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_event_buffer_h_disabled
#define eventbufferAddWithSubtype(pEventBuffer, pEventData, recordType, recordSubtype, pBNotify, pHandle) eventbufferAddWithSubtype_IMPL(pEventBuffer, pEventData, recordType, recordSubtype, pBNotify, pHandle)
#endif // __nvoc_event_buffer_h_disabled

NV_STATUS eventbufferAdd_IMPL(struct EventBuffer *pEventBuffer, EVENT_BUFFER_PRODUCER_DATA *pEventData, NvU32 recordType, NvBool *pBNotify, NvP64 *pHandle);
#ifdef __nvoc_event_buffer_h_disabled
static inline NV_STATUS eventbufferAdd(struct EventBuffer *pEventBuffer, EVENT_BUFFER_PRODUCER_DATA *pEventData, NvU32 recordType, NvBool *pBNotify, NvP64 *pHandle) {
    NV_ASSERT_FAILED_PRECOMP("EventBuffer was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_event_buffer_h_disabled
#define eventbufferAdd(pEventBuffer, pEventData, recordType, pBNotify, pHandle) eventbufferAdd_IMPL(pEventBuffer, pEventData, recordType, pBNotify, pHandle)
#endif // __nvoc_event_buffer_h_disabled

NV_STATUS eventbufferAddNotify_IMPL(struct EventBuffer *pEventBuffer, EVENT_BUFFER_PRODUCER_DATA *pEventData, NvU32 recordType, NvU32 recordSubtype, NvU32 notifyIndex, struct OBJGPU *pGpu);
#ifdef __nvoc_event_buffer_h_disabled
static inline NV_STATUS eventbufferAddNotify(struct EventBuffer *pEventBuffer, EVENT_BUFFER_PRODUCER_DATA *pEventData, NvU32 recordType, NvU32 recordSubtype, NvU32 notifyIndex, struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("EventBuffer was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_event_buffer_h_disabled
#define eventbufferAddNotify(pEventBuffer, pEventData, recordType, recordSubtype, notifyIndex, pGpu) eventbufferAddNotify_IMPL(pEventBuffer, pEventData, recordType, recordSubtype, notifyIndex, pGpu)
#endif // __nvoc_event_buffer_h_disabled

NV_STATUS eventbufferTryAddNotify_IMPL(struct EventBuffer *pEventBuffer, EVENT_BUFFER_PRODUCER_DATA *pEventData, NvU32 recordType, NvU32 recordSubtype, struct OBJGPU *pGpu);
#ifdef __nvoc_event_buffer_h_disabled
static inline NV_STATUS eventbufferTryAddNotify(struct EventBuffer *pEventBuffer, EVENT_BUFFER_PRODUCER_DATA *pEventData, NvU32 recordType, NvU32 recordSubtype, struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("EventBuffer was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_event_buffer_h_disabled
#define eventbufferTryAddNotify(pEventBuffer, pEventData, recordType, recordSubtype, pGpu) eventbufferTryAddNotify_IMPL(pEventBuffer, pEventData, recordType, recordSubtype, pGpu)
#endif // __nvoc_event_buffer_h_disabled

NvBool eventbufferIsEmpty_IMPL(struct EventBuffer *pEventBuffer);
#ifdef __nvoc_event_buffer_h_disabled
static inline NvBool eventbufferIsEmpty(struct EventBuffer *pEventBuffer) {
    NV_ASSERT_FAILED_PRECOMP("EventBuffer was disabled!");
    return NV_FALSE;
}
#else // __nvoc_event_buffer_h_disabled
#define eventbufferIsEmpty(pEventBuffer) eventbufferIsEmpty_IMPL(pEventBuffer)
#endif // __nvoc_event_buffer_h_disabled

NV_STATUS eventbufferRegisterProducerState_IMPL(struct EventBuffer *pEventBuffer, EventBufferProducerState *pState);
#ifdef __nvoc_event_buffer_h_disabled
static inline NV_STATUS eventbufferRegisterProducerState(struct EventBuffer *pEventBuffer, EventBufferProducerState *pState) {
    NV_ASSERT_FAILED_PRECOMP("EventBuffer was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_event_buffer_h_disabled
#define eventbufferRegisterProducerState(pEventBuffer, pState) eventbufferRegisterProducerState_IMPL(pEventBuffer, pState)
#endif // __nvoc_event_buffer_h_disabled

void eventbufferAddListener_IMPL(struct EventBuffer *pEventBuffer, EVENTNOTIFICATION *pEntry);
#ifdef __nvoc_event_buffer_h_disabled
static inline void eventbufferAddListener(struct EventBuffer *pEventBuffer, EVENTNOTIFICATION *pEntry) {
    NV_ASSERT_FAILED_PRECOMP("EventBuffer was disabled!");
}
#else // __nvoc_event_buffer_h_disabled
#define eventbufferAddListener(pEventBuffer, pEntry) eventbufferAddListener_IMPL(pEventBuffer, pEntry)
#endif // __nvoc_event_buffer_h_disabled

void eventbufferRemoveListener_IMPL(struct EventBuffer *pEventBuffer, EVENTNOTIFICATION *pEntry);
#ifdef __nvoc_event_buffer_h_disabled
static inline void eventbufferRemoveListener(struct EventBuffer *pEventBuffer, EVENTNOTIFICATION *pEntry) {
    NV_ASSERT_FAILED_PRECOMP("EventBuffer was disabled!");
}
#else // __nvoc_event_buffer_h_disabled
#define eventbufferRemoveListener(pEventBuffer, pEntry) eventbufferRemoveListener_IMPL(pEventBuffer, pEntry)
#endif // __nvoc_event_buffer_h_disabled

NV_STATUS eventbuffertBufferCtrlCmdEnableEvent_IMPL(struct EventBuffer *pEventBuffer, NV_EVENT_BUFFER_CTRL_CMD_ENABLE_EVENTS_PARAMS *pEnableParams);
#ifdef __nvoc_event_buffer_h_disabled
static inline NV_STATUS eventbuffertBufferCtrlCmdEnableEvent(struct EventBuffer *pEventBuffer, NV_EVENT_BUFFER_CTRL_CMD_ENABLE_EVENTS_PARAMS *pEnableParams) {
    NV_ASSERT_FAILED_PRECOMP("EventBuffer was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_event_buffer_h_disabled
#define eventbuffertBufferCtrlCmdEnableEvent(pEventBuffer, pEnableParams) eventbuffertBufferCtrlCmdEnableEvent_IMPL(pEventBuffer, pEnableParams)
#endif // __nvoc_event_buffer_h_disabled

NV_STATUS eventbuffertBufferCtrlCmdUpdateGet_IMPL(struct EventBuffer *pEventBuffer, NV_EVENT_BUFFER_CTRL_CMD_UPDATE_GET_PARAMS *pUpdateParams);
#ifdef __nvoc_event_buffer_h_disabled
static inline NV_STATUS eventbuffertBufferCtrlCmdUpdateGet(struct EventBuffer *pEventBuffer, NV_EVENT_BUFFER_CTRL_CMD_UPDATE_GET_PARAMS *pUpdateParams) {
    NV_ASSERT_FAILED_PRECOMP("EventBuffer was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_event_buffer_h_disabled
#define eventbuffertBufferCtrlCmdUpdateGet(pEventBuffer, pUpdateParams) eventbuffertBufferCtrlCmdUpdateGet_IMPL(pEventBuffer, pUpdateParams)
#endif // __nvoc_event_buffer_h_disabled

NV_STATUS eventbuffertBufferCtrlCmdFlush_IMPL(struct EventBuffer *pEventBuffer);
#ifdef __nvoc_event_buffer_h_disabled
static inline NV_STATUS eventbuffertBufferCtrlCmdFlush(struct EventBuffer *pEventBuffer) {
    NV_ASSERT_FAILED_PRECOMP("EventBuffer was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_event_buffer_h_disabled
#define eventbuffertBufferCtrlCmdFlush(pEventBuffer) eventbuffertBufferCtrlCmdFlush_IMPL(pEventBuffer)
#endif // __nvoc_event_buffer_h_disabled

NV_STATUS eventbuffertBufferCtrlCmdPostTelemetryEvent_IMPL(struct EventBuffer *pEventBuffer, NV_EVENT_BUFFER_CTRL_CMD_POST_TELEMETRY_EVENT_PARAMS *pPostTelemetryEvent);
#ifdef __nvoc_event_buffer_h_disabled
static inline NV_STATUS eventbuffertBufferCtrlCmdPostTelemetryEvent(struct EventBuffer *pEventBuffer, NV_EVENT_BUFFER_CTRL_CMD_POST_TELEMETRY_EVENT_PARAMS *pPostTelemetryEvent) {
    NV_ASSERT_FAILED_PRECOMP("EventBuffer was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_event_buffer_h_disabled
#define eventbuffertBufferCtrlCmdPostTelemetryEvent(pEventBuffer, pPostTelemetryEvent) eventbuffertBufferCtrlCmdPostTelemetryEvent_IMPL(pEventBuffer, pPostTelemetryEvent)
#endif // __nvoc_event_buffer_h_disabled


// Wrapper macros for halified functions
#define eventbufferAccessCallback_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresAccessCallback__
#define eventbufferAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) eventbufferAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define eventbufferShareCallback_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresShareCallback__
#define eventbufferShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) eventbufferShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define eventbufferGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define eventbufferGetMemInterMapParams(pRmResource, pParams) eventbufferGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define eventbufferCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define eventbufferCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) eventbufferCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define eventbufferGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define eventbufferGetMemoryMappingDescriptor(pRmResource, ppMemDesc) eventbufferGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define eventbufferControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Prologue__
#define eventbufferControlSerialization_Prologue(pResource, pCallContext, pParams) eventbufferControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define eventbufferControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Epilogue__
#define eventbufferControlSerialization_Epilogue(pResource, pCallContext, pParams) eventbufferControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define eventbufferControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Prologue__
#define eventbufferControl_Prologue(pResource, pCallContext, pParams) eventbufferControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define eventbufferControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Epilogue__
#define eventbufferControl_Epilogue(pResource, pCallContext, pParams) eventbufferControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define eventbufferCanCopy_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define eventbufferCanCopy(pResource) eventbufferCanCopy_DISPATCH(pResource)
#define eventbufferIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define eventbufferIsDuplicate(pResource, hMemory, pDuplicate) eventbufferIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define eventbufferPreDestruct_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define eventbufferPreDestruct(pResource) eventbufferPreDestruct_DISPATCH(pResource)
#define eventbufferControl_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__
#define eventbufferControl(pResource, pCallContext, pParams) eventbufferControl_DISPATCH(pResource, pCallContext, pParams)
#define eventbufferControlFilter_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define eventbufferControlFilter(pResource, pCallContext, pParams) eventbufferControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define eventbufferMap_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__
#define eventbufferMap(pResource, pCallContext, pParams, pCpuMapping) eventbufferMap_DISPATCH(pResource, pCallContext, pParams, pCpuMapping)
#define eventbufferUnmap_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__
#define eventbufferUnmap(pResource, pCallContext, pCpuMapping) eventbufferUnmap_DISPATCH(pResource, pCallContext, pCpuMapping)
#define eventbufferIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define eventbufferIsPartialUnmapSupported(pResource) eventbufferIsPartialUnmapSupported_DISPATCH(pResource)
#define eventbufferMapTo_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define eventbufferMapTo(pResource, pParams) eventbufferMapTo_DISPATCH(pResource, pParams)
#define eventbufferUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define eventbufferUnmapFrom(pResource, pParams) eventbufferUnmapFrom_DISPATCH(pResource, pParams)
#define eventbufferGetRefCount_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define eventbufferGetRefCount(pResource) eventbufferGetRefCount_DISPATCH(pResource)
#define eventbufferAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define eventbufferAddAdditionalDependants(pClient, pResource, pReference) eventbufferAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NvBool eventbufferAccessCallback_DISPATCH(struct EventBuffer *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_metadata_ptr->vtable.__eventbufferAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool eventbufferShareCallback_DISPATCH(struct EventBuffer *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__nvoc_metadata_ptr->vtable.__eventbufferShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS eventbufferGetMemInterMapParams_DISPATCH(struct EventBuffer *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__eventbufferGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS eventbufferCheckMemInterUnmap_DISPATCH(struct EventBuffer *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__eventbufferCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS eventbufferGetMemoryMappingDescriptor_DISPATCH(struct EventBuffer *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__eventbufferGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS eventbufferControlSerialization_Prologue_DISPATCH(struct EventBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__eventbufferControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void eventbufferControlSerialization_Epilogue_DISPATCH(struct EventBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__eventbufferControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS eventbufferControl_Prologue_DISPATCH(struct EventBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__eventbufferControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void eventbufferControl_Epilogue_DISPATCH(struct EventBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__eventbufferControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool eventbufferCanCopy_DISPATCH(struct EventBuffer *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__eventbufferCanCopy__(pResource);
}

static inline NV_STATUS eventbufferIsDuplicate_DISPATCH(struct EventBuffer *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_metadata_ptr->vtable.__eventbufferIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void eventbufferPreDestruct_DISPATCH(struct EventBuffer *pResource) {
    pResource->__nvoc_metadata_ptr->vtable.__eventbufferPreDestruct__(pResource);
}

static inline NV_STATUS eventbufferControl_DISPATCH(struct EventBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__eventbufferControl__(pResource, pCallContext, pParams);
}

static inline NV_STATUS eventbufferControlFilter_DISPATCH(struct EventBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__eventbufferControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS eventbufferMap_DISPATCH(struct EventBuffer *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pResource->__nvoc_metadata_ptr->vtable.__eventbufferMap__(pResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS eventbufferUnmap_DISPATCH(struct EventBuffer *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pResource->__nvoc_metadata_ptr->vtable.__eventbufferUnmap__(pResource, pCallContext, pCpuMapping);
}

static inline NvBool eventbufferIsPartialUnmapSupported_DISPATCH(struct EventBuffer *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__eventbufferIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS eventbufferMapTo_DISPATCH(struct EventBuffer *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__eventbufferMapTo__(pResource, pParams);
}

static inline NV_STATUS eventbufferUnmapFrom_DISPATCH(struct EventBuffer *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__eventbufferUnmapFrom__(pResource, pParams);
}

static inline NvU32 eventbufferGetRefCount_DISPATCH(struct EventBuffer *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__eventbufferGetRefCount__(pResource);
}

static inline void eventbufferAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct EventBuffer *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_metadata_ptr->vtable.__eventbufferAddAdditionalDependants__(pClient, pResource, pReference);
}

// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
NV_STATUS eventbuffertBufferCtrlCmdEnableEvent_IMPL(struct EventBuffer *pEventBuffer, NV_EVENT_BUFFER_CTRL_CMD_ENABLE_EVENTS_PARAMS *pEnableParams);

NV_STATUS eventbuffertBufferCtrlCmdUpdateGet_IMPL(struct EventBuffer *pEventBuffer, NV_EVENT_BUFFER_CTRL_CMD_UPDATE_GET_PARAMS *pUpdateParams);

NV_STATUS eventbuffertBufferCtrlCmdFlush_IMPL(struct EventBuffer *pEventBuffer);

NV_STATUS eventbuffertBufferCtrlCmdPostTelemetryEvent_IMPL(struct EventBuffer *pEventBuffer, NV_EVENT_BUFFER_CTRL_CMD_POST_TELEMETRY_EVENT_PARAMS *pPostTelemetryEvent);

// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


#define EVENT_BUFFER_PP_CALL(func, ...) NV_EXPAND(func NV_EXPAND() (__VA_ARGS__))

// Compatibility macros for functions that were moved into EventBuffer class
#define eventBufferAdd(...)            EVENT_BUFFER_PP_CALL(eventbufferAdd, __VA_ARGS__)
#define eventBufferIsEmpty(...)        EVENT_BUFFER_PP_CALL(eventbufferIsEmpty, __VA_ARGS__)

#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_EVENT_BUFFER_NVOC_H_
