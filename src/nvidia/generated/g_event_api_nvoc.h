
#ifndef _G_EVENT_API_NVOC_H_
#define _G_EVENT_API_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 1993-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_event_api_nvoc.h"

#ifndef _EVENT_API_H_
#define _EVENT_API_H_

#include "ctrl/ctrl0000/ctrl0000event.h" // NV0000_NOTIFIERS_MAXCOUNT

#include "resserv/resserv.h"
#include "nvoc/prelude.h"
#include "resserv/rs_server.h"
#include "rmapi/resource.h"
#include "kernel/gpu/gpu_engine_type.h"

// Opaque callback memory type for interfacing the scheduling API
typedef struct TMR_EVENT TMR_EVENT;

struct EVENTNOTIFICATION
{
    NvHandle            hEventClient;
    NvHandle            hEvent;
    NvU32               subdeviceInst;
    NvU32               NotifyIndex;              // NVnnnn_NOTIFIERS_xyz
    NvU32               NotifyType;               // Event class.  NV01_EVENT_OS_EVENT for example.
    NvBool              bUserOsEventHandle;       // Event was allocated from user app.
    NvBool              bBroadcastEvent;          // Wait for all subdevices before sending event.
    NvBool              bClientRM;                // Event was allocated from client RM.
    NvBool              bSubdeviceSpecificEvent;  // SubdeviceSpecificValue is valid.
    NvU32               SubdeviceSpecificValue;   // NV0005_NOTIFY_INDEX_SUBDEVICE
    NvBool              bEventDataRequired;       // nv_post_event allocates memory for Data.
    NvBool              bNonStallIntrEvent;
    NvU32               NotifyTriggerCount;       // Used with bBroadcastEvent.
    NvP64               Data;
    OBJGPU             *pGpu;                     // Store to free dynamic memory on teardown.
    TMR_EVENT          *pTmrEvent;                // Store to free dynamic memory on teardown.
    struct EVENTNOTIFICATION *Next;
};
typedef struct EVENTNOTIFICATION  EVENTNOTIFICATION, *PEVENTNOTIFICATION;

struct INotifier;

#ifndef __nvoc_class_id_INotifier
#define __nvoc_class_id_INotifier 0xf8f965u
typedef struct INotifier INotifier;
#endif /* __nvoc_class_id_INotifier */



MAKE_LIST(SystemEventQueueList, NV0000_CTRL_GET_SYSTEM_EVENT_DATA_PARAMS);

struct _def_client_system_event_info
{
    SystemEventQueueList eventQueue;
    NvU32                notifyActions[NV0000_NOTIFIERS_MAXCOUNT];
};

/**
 * This class represents data that is shared between one notifier and any
 * events that are registered with the notifier.
 *
 * Instances of this class are ref-counted and will be kept alive until
 * the notifier and all of its events have been freed.
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_EVENT_API_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NotifShare;
struct NVOC_METADATA__RsShared;


struct NotifShare {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__NotifShare *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct RsShared __nvoc_base_RsShared;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct RsShared *__nvoc_pbase_RsShared;    // shr super
    struct NotifShare *__nvoc_pbase_NotifShare;    // shrnotif

    // Data members
    struct INotifier *pNotifier;
    NvHandle hNotifierClient;
    NvHandle hNotifierResource;
    EVENTNOTIFICATION *pEventList;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NotifShare {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__RsShared metadata__RsShared;
};

#ifndef __nvoc_class_id_NotifShare
#define __nvoc_class_id_NotifShare 0xd5f150u
typedef struct NotifShare NotifShare;
#endif /* __nvoc_class_id_NotifShare */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_NotifShare;

#define __staticCast_NotifShare(pThis) \
    ((pThis)->__nvoc_pbase_NotifShare)

#ifdef __nvoc_event_api_h_disabled
#define __dynamicCast_NotifShare(pThis) ((NotifShare*) NULL)
#else //__nvoc_event_api_h_disabled
#define __dynamicCast_NotifShare(pThis) \
    ((NotifShare*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(NotifShare)))
#endif //__nvoc_event_api_h_disabled

NV_STATUS __nvoc_objCreateDynamic_NotifShare(NotifShare**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_NotifShare(NotifShare**, Dynamic*, NvU32);
#define __objCreate_NotifShare(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags) \
    __nvoc_objCreate_NotifShare((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags))


// Wrapper macros for implementation functions
NV_STATUS shrnotifConstruct_IMPL(struct NotifShare *pNotifShare);
#define __nvoc_shrnotifConstruct(pNotifShare) shrnotifConstruct_IMPL(pNotifShare)

void shrnotifDestruct_IMPL(struct NotifShare *pNotifShare);
#define __nvoc_shrnotifDestruct(pNotifShare) shrnotifDestruct_IMPL(pNotifShare)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


/**
 * This class represents event notification consumers
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_EVENT_API_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__EventApi;
struct NVOC_METADATA__RmResource;
struct NVOC_VTABLE__EventApi;


struct EventApi {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__EventApi *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct RmResource __nvoc_base_RmResource;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^3
    struct RsResource *__nvoc_pbase_RsResource;    // res super^2
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^2
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super
    struct EventApi *__nvoc_pbase_EventApi;    // eventapi

    // Data members
    struct NotifShare *pNotifierShare;
    NvHandle hNotifierClient;
    NvHandle hNotifierResource;
    NvHandle hEvent;
};


// Vtable with 21 per-class function pointers
struct NVOC_VTABLE__EventApi {
    NvBool (*__eventapiAccessCallback__)(struct EventApi * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (rmres)
    NvBool (*__eventapiShareCallback__)(struct EventApi * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__eventapiGetMemInterMapParams__)(struct EventApi * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__eventapiCheckMemInterUnmap__)(struct EventApi * /*this*/, NvBool);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__eventapiGetMemoryMappingDescriptor__)(struct EventApi * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__eventapiControlSerialization_Prologue__)(struct EventApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (rmres)
    void (*__eventapiControlSerialization_Epilogue__)(struct EventApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__eventapiControl_Prologue__)(struct EventApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (rmres)
    void (*__eventapiControl_Epilogue__)(struct EventApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (rmres)
    NvBool (*__eventapiCanCopy__)(struct EventApi * /*this*/);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__eventapiIsDuplicate__)(struct EventApi * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (rmres)
    void (*__eventapiPreDestruct__)(struct EventApi * /*this*/);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__eventapiControl__)(struct EventApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__eventapiControlFilter__)(struct EventApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__eventapiMap__)(struct EventApi * /*this*/, struct CALL_CONTEXT *, RS_CPU_MAP_PARAMS *, RsCpuMapping *);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__eventapiUnmap__)(struct EventApi * /*this*/, struct CALL_CONTEXT *, RsCpuMapping *);  // virtual inherited (res) base (rmres)
    NvBool (*__eventapiIsPartialUnmapSupported__)(struct EventApi * /*this*/);  // inline virtual inherited (res) base (rmres) body
    NV_STATUS (*__eventapiMapTo__)(struct EventApi * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__eventapiUnmapFrom__)(struct EventApi * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (rmres)
    NvU32 (*__eventapiGetRefCount__)(struct EventApi * /*this*/);  // virtual inherited (res) base (rmres)
    void (*__eventapiAddAdditionalDependants__)(struct RsClient *, struct EventApi * /*this*/, RsResourceRef *);  // virtual inherited (res) base (rmres)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__EventApi {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__RmResource metadata__RmResource;
    const struct NVOC_VTABLE__EventApi vtable;
};

#ifndef __nvoc_class_id_EventApi
#define __nvoc_class_id_EventApi 0x854293u
typedef struct EventApi EventApi;
#endif /* __nvoc_class_id_EventApi */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_EventApi;

#define __staticCast_EventApi(pThis) \
    ((pThis)->__nvoc_pbase_EventApi)

#ifdef __nvoc_event_api_h_disabled
#define __dynamicCast_EventApi(pThis) ((EventApi*) NULL)
#else //__nvoc_event_api_h_disabled
#define __dynamicCast_EventApi(pThis) \
    ((EventApi*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(EventApi)))
#endif //__nvoc_event_api_h_disabled

NV_STATUS __nvoc_objCreateDynamic_EventApi(EventApi**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_EventApi(EventApi**, Dynamic*, NvU32, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __objCreate_EventApi(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams) \
    __nvoc_objCreate_EventApi((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pCallContext, pParams)


// Wrapper macros for implementation functions
NV_STATUS eventapiConstruct_IMPL(struct EventApi *pEvent, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __nvoc_eventapiConstruct(pEvent, pCallContext, pParams) eventapiConstruct_IMPL(pEvent, pCallContext, pParams)

void eventapiDestruct_IMPL(struct EventApi *pEvent);
#define __nvoc_eventapiDestruct(pEvent) eventapiDestruct_IMPL(pEvent)

NV_STATUS eventapiInit_IMPL(struct EventApi *pEvent, struct CALL_CONTEXT *pCallContext, NvHandle hNotifierClient, NvHandle hNotifierResource, PEVENTNOTIFICATION **pppEventNotification);
#ifdef __nvoc_event_api_h_disabled
static inline NV_STATUS eventapiInit(struct EventApi *pEvent, struct CALL_CONTEXT *pCallContext, NvHandle hNotifierClient, NvHandle hNotifierResource, PEVENTNOTIFICATION **pppEventNotification) {
    NV_ASSERT_FAILED_PRECOMP("EventApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_event_api_h_disabled
#define eventapiInit(pEvent, pCallContext, hNotifierClient, hNotifierResource, pppEventNotification) eventapiInit_IMPL(pEvent, pCallContext, hNotifierClient, hNotifierResource, pppEventNotification)
#endif // __nvoc_event_api_h_disabled

NV_STATUS eventapiGetByHandle_IMPL(struct RsClient *pClient, NvHandle hEvent, NvU32 *pNotifyIndex);
#define eventapiGetByHandle(pClient, hEvent, pNotifyIndex) eventapiGetByHandle_IMPL(pClient, hEvent, pNotifyIndex)


// Wrapper macros for halified functions
#define eventapiAccessCallback_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresAccessCallback__
#define eventapiAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) eventapiAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define eventapiShareCallback_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresShareCallback__
#define eventapiShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) eventapiShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define eventapiGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define eventapiGetMemInterMapParams(pRmResource, pParams) eventapiGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define eventapiCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define eventapiCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) eventapiCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define eventapiGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define eventapiGetMemoryMappingDescriptor(pRmResource, ppMemDesc) eventapiGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define eventapiControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Prologue__
#define eventapiControlSerialization_Prologue(pResource, pCallContext, pParams) eventapiControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define eventapiControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Epilogue__
#define eventapiControlSerialization_Epilogue(pResource, pCallContext, pParams) eventapiControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define eventapiControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Prologue__
#define eventapiControl_Prologue(pResource, pCallContext, pParams) eventapiControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define eventapiControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Epilogue__
#define eventapiControl_Epilogue(pResource, pCallContext, pParams) eventapiControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define eventapiCanCopy_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define eventapiCanCopy(pResource) eventapiCanCopy_DISPATCH(pResource)
#define eventapiIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define eventapiIsDuplicate(pResource, hMemory, pDuplicate) eventapiIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define eventapiPreDestruct_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define eventapiPreDestruct(pResource) eventapiPreDestruct_DISPATCH(pResource)
#define eventapiControl_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__
#define eventapiControl(pResource, pCallContext, pParams) eventapiControl_DISPATCH(pResource, pCallContext, pParams)
#define eventapiControlFilter_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define eventapiControlFilter(pResource, pCallContext, pParams) eventapiControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define eventapiMap_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__
#define eventapiMap(pResource, pCallContext, pParams, pCpuMapping) eventapiMap_DISPATCH(pResource, pCallContext, pParams, pCpuMapping)
#define eventapiUnmap_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__
#define eventapiUnmap(pResource, pCallContext, pCpuMapping) eventapiUnmap_DISPATCH(pResource, pCallContext, pCpuMapping)
#define eventapiIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define eventapiIsPartialUnmapSupported(pResource) eventapiIsPartialUnmapSupported_DISPATCH(pResource)
#define eventapiMapTo_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define eventapiMapTo(pResource, pParams) eventapiMapTo_DISPATCH(pResource, pParams)
#define eventapiUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define eventapiUnmapFrom(pResource, pParams) eventapiUnmapFrom_DISPATCH(pResource, pParams)
#define eventapiGetRefCount_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define eventapiGetRefCount(pResource) eventapiGetRefCount_DISPATCH(pResource)
#define eventapiAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define eventapiAddAdditionalDependants(pClient, pResource, pReference) eventapiAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NvBool eventapiAccessCallback_DISPATCH(struct EventApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_metadata_ptr->vtable.__eventapiAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool eventapiShareCallback_DISPATCH(struct EventApi *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__nvoc_metadata_ptr->vtable.__eventapiShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS eventapiGetMemInterMapParams_DISPATCH(struct EventApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__eventapiGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS eventapiCheckMemInterUnmap_DISPATCH(struct EventApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__eventapiCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS eventapiGetMemoryMappingDescriptor_DISPATCH(struct EventApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__eventapiGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS eventapiControlSerialization_Prologue_DISPATCH(struct EventApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__eventapiControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void eventapiControlSerialization_Epilogue_DISPATCH(struct EventApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__eventapiControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS eventapiControl_Prologue_DISPATCH(struct EventApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__eventapiControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void eventapiControl_Epilogue_DISPATCH(struct EventApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__eventapiControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool eventapiCanCopy_DISPATCH(struct EventApi *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__eventapiCanCopy__(pResource);
}

static inline NV_STATUS eventapiIsDuplicate_DISPATCH(struct EventApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_metadata_ptr->vtable.__eventapiIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void eventapiPreDestruct_DISPATCH(struct EventApi *pResource) {
    pResource->__nvoc_metadata_ptr->vtable.__eventapiPreDestruct__(pResource);
}

static inline NV_STATUS eventapiControl_DISPATCH(struct EventApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__eventapiControl__(pResource, pCallContext, pParams);
}

static inline NV_STATUS eventapiControlFilter_DISPATCH(struct EventApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__eventapiControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS eventapiMap_DISPATCH(struct EventApi *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pResource->__nvoc_metadata_ptr->vtable.__eventapiMap__(pResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS eventapiUnmap_DISPATCH(struct EventApi *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pResource->__nvoc_metadata_ptr->vtable.__eventapiUnmap__(pResource, pCallContext, pCpuMapping);
}

static inline NvBool eventapiIsPartialUnmapSupported_DISPATCH(struct EventApi *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__eventapiIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS eventapiMapTo_DISPATCH(struct EventApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__eventapiMapTo__(pResource, pParams);
}

static inline NV_STATUS eventapiUnmapFrom_DISPATCH(struct EventApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__eventapiUnmapFrom__(pResource, pParams);
}

static inline NvU32 eventapiGetRefCount_DISPATCH(struct EventApi *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__eventapiGetRefCount__(pResource);
}

static inline void eventapiAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct EventApi *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_metadata_ptr->vtable.__eventapiAddAdditionalDependants__(pClient, pResource, pReference);
}

// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


/**
  * Mix-in interface for resources that send notifications to events
  */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_EVENT_API_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable
struct NVOC_METADATA__INotifier;
struct NVOC_VTABLE__INotifier;


struct INotifier {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__INotifier *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Ancestor object pointers for `staticCast` feature
    struct INotifier *__nvoc_pbase_INotifier;    // inotify
};


// Vtable with 5 per-class function pointers
struct NVOC_VTABLE__INotifier {
    PEVENTNOTIFICATION * (*__inotifyGetNotificationListPtr__)(struct INotifier * /*this*/);  // pure virtual
    void (*__inotifySetNotificationShare__)(struct INotifier * /*this*/, struct NotifShare *);  // pure virtual
    struct NotifShare * (*__inotifyGetNotificationShare__)(struct INotifier * /*this*/);  // pure virtual
    NV_STATUS (*__inotifyUnregisterEvent__)(struct INotifier * /*this*/, NvHandle, NvHandle, NvHandle, NvHandle);  // pure virtual
    NV_STATUS (*__inotifyGetOrAllocNotifShare__)(struct INotifier * /*this*/, NvHandle, NvHandle, struct NotifShare **);  // pure virtual
};

// Metadata with per-class RTTI and vtable
struct NVOC_METADATA__INotifier {
    const struct NVOC_RTTI rtti;
    const struct NVOC_VTABLE__INotifier vtable;
};

#ifndef __nvoc_class_id_INotifier
#define __nvoc_class_id_INotifier 0xf8f965u
typedef struct INotifier INotifier;
#endif /* __nvoc_class_id_INotifier */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;

#define __staticCast_INotifier(pThis) \
    ((pThis)->__nvoc_pbase_INotifier)

#ifdef __nvoc_event_api_h_disabled
#define __dynamicCast_INotifier(pThis) ((INotifier*) NULL)
#else //__nvoc_event_api_h_disabled
#define __dynamicCast_INotifier(pThis) \
    ((INotifier*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(INotifier)))
#endif //__nvoc_event_api_h_disabled

NV_STATUS __nvoc_objCreateDynamic_INotifier(INotifier**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_INotifier(INotifier**, Dynamic*, NvU32, struct CALL_CONTEXT *pCallContext);
#define __objCreate_INotifier(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pCallContext) \
    __nvoc_objCreate_INotifier((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pCallContext)


// Wrapper macros for implementation functions
NV_STATUS inotifyConstruct_IMPL(struct INotifier *pNotifier, struct CALL_CONTEXT *pCallContext);
#define __nvoc_inotifyConstruct(pNotifier, pCallContext) inotifyConstruct_IMPL(pNotifier, pCallContext)

void inotifyDestruct_IMPL(struct INotifier *pNotifier);
#define __nvoc_inotifyDestruct(pNotifier) inotifyDestruct_IMPL(pNotifier)

PEVENTNOTIFICATION inotifyGetNotificationList_IMPL(struct INotifier *pNotifier);
#ifdef __nvoc_event_api_h_disabled
static inline PEVENTNOTIFICATION inotifyGetNotificationList(struct INotifier *pNotifier) {
    NV_ASSERT_FAILED_PRECOMP("INotifier was disabled!");
    return NULL;
}
#else // __nvoc_event_api_h_disabled
#define inotifyGetNotificationList(pNotifier) inotifyGetNotificationList_IMPL(pNotifier)
#endif // __nvoc_event_api_h_disabled


// Wrapper macros for halified functions
#define inotifyGetNotificationListPtr_FNPTR(pNotifier) pNotifier->__nvoc_metadata_ptr->vtable.__inotifyGetNotificationListPtr__
#define inotifyGetNotificationListPtr(pNotifier) inotifyGetNotificationListPtr_DISPATCH(pNotifier)
#define inotifySetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_metadata_ptr->vtable.__inotifySetNotificationShare__
#define inotifySetNotificationShare(pNotifier, pNotifShare) inotifySetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define inotifyGetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_metadata_ptr->vtable.__inotifyGetNotificationShare__
#define inotifyGetNotificationShare(pNotifier) inotifyGetNotificationShare_DISPATCH(pNotifier)
#define inotifyUnregisterEvent_FNPTR(pNotifier) pNotifier->__nvoc_metadata_ptr->vtable.__inotifyUnregisterEvent__
#define inotifyUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) inotifyUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define inotifyGetOrAllocNotifShare_FNPTR(pNotifier) pNotifier->__nvoc_metadata_ptr->vtable.__inotifyGetOrAllocNotifShare__
#define inotifyGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) inotifyGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)

// Dispatch functions
static inline PEVENTNOTIFICATION * inotifyGetNotificationListPtr_DISPATCH(struct INotifier *pNotifier) {
    return pNotifier->__nvoc_metadata_ptr->vtable.__inotifyGetNotificationListPtr__(pNotifier);
}

static inline void inotifySetNotificationShare_DISPATCH(struct INotifier *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__nvoc_metadata_ptr->vtable.__inotifySetNotificationShare__(pNotifier, pNotifShare);
}

static inline struct NotifShare * inotifyGetNotificationShare_DISPATCH(struct INotifier *pNotifier) {
    return pNotifier->__nvoc_metadata_ptr->vtable.__inotifyGetNotificationShare__(pNotifier);
}

static inline NV_STATUS inotifyUnregisterEvent_DISPATCH(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__nvoc_metadata_ptr->vtable.__inotifyUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS inotifyGetOrAllocNotifShare_DISPATCH(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__nvoc_metadata_ptr->vtable.__inotifyGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


/**
  * Basic implementation for event notification mix-in
  */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_EVENT_API_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__Notifier;
struct NVOC_METADATA__INotifier;
struct NVOC_VTABLE__Notifier;


struct Notifier {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__Notifier *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct INotifier __nvoc_base_INotifier;

    // Ancestor object pointers for `staticCast` feature
    struct INotifier *__nvoc_pbase_INotifier;    // inotify super
    struct Notifier *__nvoc_pbase_Notifier;    // notify

    // Data members
    struct NotifShare *pNotifierShare;
};


// Vtable with 5 per-class function pointers
struct NVOC_VTABLE__Notifier {
    PEVENTNOTIFICATION * (*__notifyGetNotificationListPtr__)(struct Notifier * /*this*/);  // virtual override (inotify) base (inotify)
    struct NotifShare * (*__notifyGetNotificationShare__)(struct Notifier * /*this*/);  // virtual override (inotify) base (inotify)
    void (*__notifySetNotificationShare__)(struct Notifier * /*this*/, struct NotifShare *);  // virtual override (inotify) base (inotify)
    NV_STATUS (*__notifyUnregisterEvent__)(struct Notifier * /*this*/, NvHandle, NvHandle, NvHandle, NvHandle);  // virtual override (inotify) base (inotify)
    NV_STATUS (*__notifyGetOrAllocNotifShare__)(struct Notifier * /*this*/, NvHandle, NvHandle, struct NotifShare **);  // virtual override (inotify) base (inotify)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__Notifier {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__INotifier metadata__INotifier;
    const struct NVOC_VTABLE__Notifier vtable;
};

#ifndef __nvoc_class_id_Notifier
#define __nvoc_class_id_Notifier 0xa8683bu
typedef struct Notifier Notifier;
#endif /* __nvoc_class_id_Notifier */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;

#define __staticCast_Notifier(pThis) \
    ((pThis)->__nvoc_pbase_Notifier)

#ifdef __nvoc_event_api_h_disabled
#define __dynamicCast_Notifier(pThis) ((Notifier*) NULL)
#else //__nvoc_event_api_h_disabled
#define __dynamicCast_Notifier(pThis) \
    ((Notifier*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(Notifier)))
#endif //__nvoc_event_api_h_disabled

NV_STATUS __nvoc_objCreateDynamic_Notifier(Notifier**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_Notifier(Notifier**, Dynamic*, NvU32, struct CALL_CONTEXT *pCallContext);
#define __objCreate_Notifier(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pCallContext) \
    __nvoc_objCreate_Notifier((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pCallContext)


// Wrapper macros for implementation functions
NV_STATUS notifyConstruct_IMPL(struct Notifier *pNotifier, struct CALL_CONTEXT *pCallContext);
#define __nvoc_notifyConstruct(pNotifier, pCallContext) notifyConstruct_IMPL(pNotifier, pCallContext)

void notifyDestruct_IMPL(struct Notifier *pNotifier);
#define __nvoc_notifyDestruct(pNotifier) notifyDestruct_IMPL(pNotifier)


// Wrapper macros for halified functions
#define notifyGetNotificationListPtr_FNPTR(pNotifier) pNotifier->__nvoc_metadata_ptr->vtable.__notifyGetNotificationListPtr__
#define notifyGetNotificationListPtr(pNotifier) notifyGetNotificationListPtr_DISPATCH(pNotifier)
#define notifyGetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_metadata_ptr->vtable.__notifyGetNotificationShare__
#define notifyGetNotificationShare(pNotifier) notifyGetNotificationShare_DISPATCH(pNotifier)
#define notifySetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_metadata_ptr->vtable.__notifySetNotificationShare__
#define notifySetNotificationShare(pNotifier, pNotifShare) notifySetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define notifyUnregisterEvent_FNPTR(pNotifier) pNotifier->__nvoc_metadata_ptr->vtable.__notifyUnregisterEvent__
#define notifyUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) notifyUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define notifyGetOrAllocNotifShare_FNPTR(pNotifier) pNotifier->__nvoc_metadata_ptr->vtable.__notifyGetOrAllocNotifShare__
#define notifyGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) notifyGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)

// Dispatch functions
static inline PEVENTNOTIFICATION * notifyGetNotificationListPtr_DISPATCH(struct Notifier *pNotifier) {
    return pNotifier->__nvoc_metadata_ptr->vtable.__notifyGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare * notifyGetNotificationShare_DISPATCH(struct Notifier *pNotifier) {
    return pNotifier->__nvoc_metadata_ptr->vtable.__notifyGetNotificationShare__(pNotifier);
}

static inline void notifySetNotificationShare_DISPATCH(struct Notifier *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__nvoc_metadata_ptr->vtable.__notifySetNotificationShare__(pNotifier, pNotifShare);
}

static inline NV_STATUS notifyUnregisterEvent_DISPATCH(struct Notifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__nvoc_metadata_ptr->vtable.__notifyUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS notifyGetOrAllocNotifShare_DISPATCH(struct Notifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__nvoc_metadata_ptr->vtable.__notifyGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

// Virtual method declarations and/or inline definitions
PEVENTNOTIFICATION * notifyGetNotificationListPtr_IMPL(struct Notifier *pNotifier);

struct NotifShare * notifyGetNotificationShare_IMPL(struct Notifier *pNotifier);

void notifySetNotificationShare_IMPL(struct Notifier *pNotifier, struct NotifShare *pNotifShare);

NV_STATUS notifyUnregisterEvent_IMPL(struct Notifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);

NV_STATUS notifyGetOrAllocNotifShare_IMPL(struct Notifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);

// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


void CliAddSystemEvent(NvU32, void *, NvBool *);
void CliDelObjectEvents(RsResourceRef *pResourceRef);
NvBool CliGetEventInfo(NvHandle hClient, NvHandle hEvent, struct EventApi **ppEvent);
NV_STATUS CliGetEventNotificationList(NvHandle hClient, NvHandle hObject,
                                      struct INotifier **ppNotifier,
                                      PEVENTNOTIFICATION **pppEventNotification);

NV_STATUS registerEventNotification(PEVENTNOTIFICATION*, struct RsClient *, NvHandle, NvHandle, NvU32, NvU32, NvP64, NvBool);
NV_STATUS unregisterEventNotification(PEVENTNOTIFICATION*, NvHandle, NvHandle, NvHandle);
NV_STATUS unregisterEventNotificationWithData(PEVENTNOTIFICATION *, NvHandle, NvHandle, NvHandle, NvBool, NvP64);
NV_STATUS bindEventNotificationToSubdevice(PEVENTNOTIFICATION, NvHandle, NvU32);
NV_STATUS engineNonStallIntrNotify(OBJGPU *, RM_ENGINE_TYPE);
NV_STATUS notifyEvents(OBJGPU*, EVENTNOTIFICATION*, NvU32, NvU32, NvU32, NV_STATUS, NvU32);
NV_STATUS engineNonStallIntrNotifyEvent(OBJGPU *, RM_ENGINE_TYPE, NvHandle);

typedef struct GpuEngineEventNotificationList GpuEngineEventNotificationList;

NV_STATUS gpuEngineEventNotificationListCreate(OBJGPU *, GpuEngineEventNotificationList **);
void      gpuEngineEventNotificationListDestroy(OBJGPU *, GpuEngineEventNotificationList *);

// System Event Queue helpers
void eventSystemInitEventQueue(SystemEventQueueList *pQueue);
NV_STATUS eventSystemEnqueueEvent(SystemEventQueueList *pQueue, NvU32 event, void *pEventData);
NV_STATUS eventSystemDequeueEvent(SystemEventQueueList *pQueue, NV0000_CTRL_GET_SYSTEM_EVENT_DATA_PARAMS *pEvent);
void eventSystemClearEventQueue(SystemEventQueueList *pQueue);

#endif // _EVENT_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_EVENT_API_NVOC_H_
