#ifndef _G_EVENT_NVOC_H_
#define _G_EVENT_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2019 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_event_nvoc.h"

#ifndef _EVENT_H_
#define _EVENT_H_

#include "class/cl0000.h" // NV0000_NOTIFIERS_MAXCOUNT

#include "resserv/resserv.h"
#include "nvoc/prelude.h"
#include "resserv/rs_server.h"
#include "rmapi/resource.h"

typedef struct _def_system_event_queue      SYSTEM_EVENTS_QUEUE;

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
    struct EVENTNOTIFICATION *Next;
};
typedef struct EVENTNOTIFICATION  EVENTNOTIFICATION, *PEVENTNOTIFICATION;

struct INotifier;

#ifndef __NVOC_CLASS_INotifier_TYPEDEF__
#define __NVOC_CLASS_INotifier_TYPEDEF__
typedef struct INotifier INotifier;
#endif /* __NVOC_CLASS_INotifier_TYPEDEF__ */

#ifndef __nvoc_class_id_INotifier
#define __nvoc_class_id_INotifier 0xf8f965
#endif /* __nvoc_class_id_INotifier */



#define NV_SYSTEM_EVENT_QUEUE_SIZE 16
struct _def_system_event_queue
{
    NvU32 Head;
    NvU32 Tail;
    struct event_queue
    {
        NvU32 event;
        NvU32 status;
    } EventQueue[NV_SYSTEM_EVENT_QUEUE_SIZE];
};

struct _def_client_system_event_info
{
    SYSTEM_EVENTS_QUEUE  systemEventsQueue;
    NvU32                notifyActions[NV0000_NOTIFIERS_MAXCOUNT];
};

/**
 * This class represents data that is shared between one notifier and any
 * events that are registered with the notifier.
 *
 * Instances of this class are ref-counted and will be kept alive until
 * the notifier and all of its events have been freed.
 */
#ifdef NVOC_EVENT_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct NotifShare {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct RsShared __nvoc_base_RsShared;
    struct Object *__nvoc_pbase_Object;
    struct RsShared *__nvoc_pbase_RsShared;
    struct NotifShare *__nvoc_pbase_NotifShare;
    struct INotifier *pNotifier;
    NvHandle hNotifierClient;
    NvHandle hNotifierResource;
    EVENTNOTIFICATION *pEventList;
};

#ifndef __NVOC_CLASS_NotifShare_TYPEDEF__
#define __NVOC_CLASS_NotifShare_TYPEDEF__
typedef struct NotifShare NotifShare;
#endif /* __NVOC_CLASS_NotifShare_TYPEDEF__ */

#ifndef __nvoc_class_id_NotifShare
#define __nvoc_class_id_NotifShare 0xd5f150
#endif /* __nvoc_class_id_NotifShare */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_NotifShare;

#define __staticCast_NotifShare(pThis) \
    ((pThis)->__nvoc_pbase_NotifShare)

#ifdef __nvoc_event_h_disabled
#define __dynamicCast_NotifShare(pThis) ((NotifShare*)NULL)
#else //__nvoc_event_h_disabled
#define __dynamicCast_NotifShare(pThis) \
    ((NotifShare*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(NotifShare)))
#endif //__nvoc_event_h_disabled


NV_STATUS __nvoc_objCreateDynamic_NotifShare(NotifShare**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_NotifShare(NotifShare**, Dynamic*, NvU32);
#define __objCreate_NotifShare(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_NotifShare((ppNewObj), staticCast((pParent), Dynamic), (createFlags))

NV_STATUS shrnotifConstruct_IMPL(struct NotifShare *arg_pNotifShare);
#define __nvoc_shrnotifConstruct(arg_pNotifShare) shrnotifConstruct_IMPL(arg_pNotifShare)
void shrnotifDestruct_IMPL(struct NotifShare *pNotifShare);
#define __nvoc_shrnotifDestruct(pNotifShare) shrnotifDestruct_IMPL(pNotifShare)
#undef PRIVATE_FIELD


/**
 * This class represents event notification consumers
 */
#ifdef NVOC_EVENT_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct Event {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct RmResource __nvoc_base_RmResource;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct Event *__nvoc_pbase_Event;
    NvBool (*__eventShareCallback__)(struct Event *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__eventCheckMemInterUnmap__)(struct Event *, NvBool);
    NV_STATUS (*__eventControl__)(struct Event *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__eventGetMemInterMapParams__)(struct Event *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__eventGetMemoryMappingDescriptor__)(struct Event *, struct MEMORY_DESCRIPTOR **);
    NvU32 (*__eventGetRefCount__)(struct Event *);
    NV_STATUS (*__eventControlFilter__)(struct Event *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    void (*__eventAddAdditionalDependants__)(struct RsClient *, struct Event *, RsResourceRef *);
    NV_STATUS (*__eventUnmap__)(struct Event *, struct CALL_CONTEXT *, RsCpuMapping *);
    NV_STATUS (*__eventControl_Prologue__)(struct Event *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__eventCanCopy__)(struct Event *);
    NV_STATUS (*__eventMapTo__)(struct Event *, RS_RES_MAP_TO_PARAMS *);
    void (*__eventPreDestruct__)(struct Event *);
    NV_STATUS (*__eventUnmapFrom__)(struct Event *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__eventControl_Epilogue__)(struct Event *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__eventControlLookup__)(struct Event *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NV_STATUS (*__eventMap__)(struct Event *, struct CALL_CONTEXT *, RS_CPU_MAP_PARAMS *, RsCpuMapping *);
    NvBool (*__eventAccessCallback__)(struct Event *, struct RsClient *, void *, RsAccessRight);
    struct NotifShare *pNotifierShare;
    NvHandle hNotifierClient;
    NvHandle hNotifierResource;
    NvHandle hEvent;
};

#ifndef __NVOC_CLASS_Event_TYPEDEF__
#define __NVOC_CLASS_Event_TYPEDEF__
typedef struct Event Event;
#endif /* __NVOC_CLASS_Event_TYPEDEF__ */

#ifndef __nvoc_class_id_Event
#define __nvoc_class_id_Event 0xa4ecfc
#endif /* __nvoc_class_id_Event */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;

#define __staticCast_Event(pThis) \
    ((pThis)->__nvoc_pbase_Event)

#ifdef __nvoc_event_h_disabled
#define __dynamicCast_Event(pThis) ((Event*)NULL)
#else //__nvoc_event_h_disabled
#define __dynamicCast_Event(pThis) \
    ((Event*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(Event)))
#endif //__nvoc_event_h_disabled


NV_STATUS __nvoc_objCreateDynamic_Event(Event**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_Event(Event**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_Event(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_Event((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define eventShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) eventShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define eventCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) eventCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define eventControl(pResource, pCallContext, pParams) eventControl_DISPATCH(pResource, pCallContext, pParams)
#define eventGetMemInterMapParams(pRmResource, pParams) eventGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define eventGetMemoryMappingDescriptor(pRmResource, ppMemDesc) eventGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define eventGetRefCount(pResource) eventGetRefCount_DISPATCH(pResource)
#define eventControlFilter(pResource, pCallContext, pParams) eventControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define eventAddAdditionalDependants(pClient, pResource, pReference) eventAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define eventUnmap(pResource, pCallContext, pCpuMapping) eventUnmap_DISPATCH(pResource, pCallContext, pCpuMapping)
#define eventControl_Prologue(pResource, pCallContext, pParams) eventControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define eventCanCopy(pResource) eventCanCopy_DISPATCH(pResource)
#define eventMapTo(pResource, pParams) eventMapTo_DISPATCH(pResource, pParams)
#define eventPreDestruct(pResource) eventPreDestruct_DISPATCH(pResource)
#define eventUnmapFrom(pResource, pParams) eventUnmapFrom_DISPATCH(pResource, pParams)
#define eventControl_Epilogue(pResource, pCallContext, pParams) eventControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define eventControlLookup(pResource, pParams, ppEntry) eventControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define eventMap(pResource, pCallContext, pParams, pCpuMapping) eventMap_DISPATCH(pResource, pCallContext, pParams, pCpuMapping)
#define eventAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) eventAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
static inline NvBool eventShareCallback_DISPATCH(struct Event *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__eventShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS eventCheckMemInterUnmap_DISPATCH(struct Event *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__eventCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS eventControl_DISPATCH(struct Event *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__eventControl__(pResource, pCallContext, pParams);
}

static inline NV_STATUS eventGetMemInterMapParams_DISPATCH(struct Event *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__eventGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS eventGetMemoryMappingDescriptor_DISPATCH(struct Event *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__eventGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NvU32 eventGetRefCount_DISPATCH(struct Event *pResource) {
    return pResource->__eventGetRefCount__(pResource);
}

static inline NV_STATUS eventControlFilter_DISPATCH(struct Event *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__eventControlFilter__(pResource, pCallContext, pParams);
}

static inline void eventAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct Event *pResource, RsResourceRef *pReference) {
    pResource->__eventAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS eventUnmap_DISPATCH(struct Event *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pResource->__eventUnmap__(pResource, pCallContext, pCpuMapping);
}

static inline NV_STATUS eventControl_Prologue_DISPATCH(struct Event *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__eventControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool eventCanCopy_DISPATCH(struct Event *pResource) {
    return pResource->__eventCanCopy__(pResource);
}

static inline NV_STATUS eventMapTo_DISPATCH(struct Event *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__eventMapTo__(pResource, pParams);
}

static inline void eventPreDestruct_DISPATCH(struct Event *pResource) {
    pResource->__eventPreDestruct__(pResource);
}

static inline NV_STATUS eventUnmapFrom_DISPATCH(struct Event *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__eventUnmapFrom__(pResource, pParams);
}

static inline void eventControl_Epilogue_DISPATCH(struct Event *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__eventControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS eventControlLookup_DISPATCH(struct Event *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__eventControlLookup__(pResource, pParams, ppEntry);
}

static inline NV_STATUS eventMap_DISPATCH(struct Event *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pResource->__eventMap__(pResource, pCallContext, pParams, pCpuMapping);
}

static inline NvBool eventAccessCallback_DISPATCH(struct Event *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__eventAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

NV_STATUS eventConstruct_IMPL(struct Event *arg_pEvent, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __nvoc_eventConstruct(arg_pEvent, arg_pCallContext, arg_pParams) eventConstruct_IMPL(arg_pEvent, arg_pCallContext, arg_pParams)
void eventDestruct_IMPL(struct Event *pEvent);
#define __nvoc_eventDestruct(pEvent) eventDestruct_IMPL(pEvent)
NV_STATUS eventInit_IMPL(struct Event *pEvent, struct CALL_CONTEXT *pCallContext, NvHandle hNotifierClient, NvHandle hNotifierResource, PEVENTNOTIFICATION **pppEventNotification);
#ifdef __nvoc_event_h_disabled
static inline NV_STATUS eventInit(struct Event *pEvent, struct CALL_CONTEXT *pCallContext, NvHandle hNotifierClient, NvHandle hNotifierResource, PEVENTNOTIFICATION **pppEventNotification) {
    NV_ASSERT_FAILED_PRECOMP("Event was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_event_h_disabled
#define eventInit(pEvent, pCallContext, hNotifierClient, hNotifierResource, pppEventNotification) eventInit_IMPL(pEvent, pCallContext, hNotifierClient, hNotifierResource, pppEventNotification)
#endif //__nvoc_event_h_disabled

#undef PRIVATE_FIELD


/**
  * Mix-in interface for resources that send notifications to events
  */
#ifdef NVOC_EVENT_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct INotifier {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct INotifier *__nvoc_pbase_INotifier;
    PEVENTNOTIFICATION *(*__inotifyGetNotificationListPtr__)(struct INotifier *);
    void (*__inotifySetNotificationShare__)(struct INotifier *, struct NotifShare *);
    struct NotifShare *(*__inotifyGetNotificationShare__)(struct INotifier *);
    NV_STATUS (*__inotifyUnregisterEvent__)(struct INotifier *, NvHandle, NvHandle, NvHandle, NvHandle);
    NV_STATUS (*__inotifyGetOrAllocNotifShare__)(struct INotifier *, NvHandle, NvHandle, struct NotifShare **);
};

#ifndef __NVOC_CLASS_INotifier_TYPEDEF__
#define __NVOC_CLASS_INotifier_TYPEDEF__
typedef struct INotifier INotifier;
#endif /* __NVOC_CLASS_INotifier_TYPEDEF__ */

#ifndef __nvoc_class_id_INotifier
#define __nvoc_class_id_INotifier 0xf8f965
#endif /* __nvoc_class_id_INotifier */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;

#define __staticCast_INotifier(pThis) \
    ((pThis)->__nvoc_pbase_INotifier)

#ifdef __nvoc_event_h_disabled
#define __dynamicCast_INotifier(pThis) ((INotifier*)NULL)
#else //__nvoc_event_h_disabled
#define __dynamicCast_INotifier(pThis) \
    ((INotifier*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(INotifier)))
#endif //__nvoc_event_h_disabled


NV_STATUS __nvoc_objCreateDynamic_INotifier(INotifier**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_INotifier(INotifier**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext);
#define __objCreate_INotifier(ppNewObj, pParent, createFlags, arg_pCallContext) \
    __nvoc_objCreate_INotifier((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext)

#define inotifyGetNotificationListPtr(pNotifier) inotifyGetNotificationListPtr_DISPATCH(pNotifier)
#define inotifySetNotificationShare(pNotifier, pNotifShare) inotifySetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define inotifyGetNotificationShare(pNotifier) inotifyGetNotificationShare_DISPATCH(pNotifier)
#define inotifyUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) inotifyUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define inotifyGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) inotifyGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)
static inline PEVENTNOTIFICATION *inotifyGetNotificationListPtr_DISPATCH(struct INotifier *pNotifier) {
    return pNotifier->__inotifyGetNotificationListPtr__(pNotifier);
}

static inline void inotifySetNotificationShare_DISPATCH(struct INotifier *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__inotifySetNotificationShare__(pNotifier, pNotifShare);
}

static inline struct NotifShare *inotifyGetNotificationShare_DISPATCH(struct INotifier *pNotifier) {
    return pNotifier->__inotifyGetNotificationShare__(pNotifier);
}

static inline NV_STATUS inotifyUnregisterEvent_DISPATCH(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__inotifyUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS inotifyGetOrAllocNotifShare_DISPATCH(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__inotifyGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

NV_STATUS inotifyConstruct_IMPL(struct INotifier *arg_pNotifier, struct CALL_CONTEXT *arg_pCallContext);
#define __nvoc_inotifyConstruct(arg_pNotifier, arg_pCallContext) inotifyConstruct_IMPL(arg_pNotifier, arg_pCallContext)
void inotifyDestruct_IMPL(struct INotifier *pNotifier);
#define __nvoc_inotifyDestruct(pNotifier) inotifyDestruct_IMPL(pNotifier)
PEVENTNOTIFICATION inotifyGetNotificationList_IMPL(struct INotifier *pNotifier);
#ifdef __nvoc_event_h_disabled
static inline PEVENTNOTIFICATION inotifyGetNotificationList(struct INotifier *pNotifier) {
    NV_ASSERT_FAILED_PRECOMP("INotifier was disabled!");
    return NULL;
}
#else //__nvoc_event_h_disabled
#define inotifyGetNotificationList(pNotifier) inotifyGetNotificationList_IMPL(pNotifier)
#endif //__nvoc_event_h_disabled

#undef PRIVATE_FIELD


/**
  * Basic implementation for event notification mix-in
  */
#ifdef NVOC_EVENT_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct Notifier {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct INotifier __nvoc_base_INotifier;
    struct INotifier *__nvoc_pbase_INotifier;
    struct Notifier *__nvoc_pbase_Notifier;
    PEVENTNOTIFICATION *(*__notifyGetNotificationListPtr__)(struct Notifier *);
    struct NotifShare *(*__notifyGetNotificationShare__)(struct Notifier *);
    void (*__notifySetNotificationShare__)(struct Notifier *, struct NotifShare *);
    NV_STATUS (*__notifyUnregisterEvent__)(struct Notifier *, NvHandle, NvHandle, NvHandle, NvHandle);
    NV_STATUS (*__notifyGetOrAllocNotifShare__)(struct Notifier *, NvHandle, NvHandle, struct NotifShare **);
    struct NotifShare *pNotifierShare;
};

#ifndef __NVOC_CLASS_Notifier_TYPEDEF__
#define __NVOC_CLASS_Notifier_TYPEDEF__
typedef struct Notifier Notifier;
#endif /* __NVOC_CLASS_Notifier_TYPEDEF__ */

#ifndef __nvoc_class_id_Notifier
#define __nvoc_class_id_Notifier 0xa8683b
#endif /* __nvoc_class_id_Notifier */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;

#define __staticCast_Notifier(pThis) \
    ((pThis)->__nvoc_pbase_Notifier)

#ifdef __nvoc_event_h_disabled
#define __dynamicCast_Notifier(pThis) ((Notifier*)NULL)
#else //__nvoc_event_h_disabled
#define __dynamicCast_Notifier(pThis) \
    ((Notifier*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(Notifier)))
#endif //__nvoc_event_h_disabled


NV_STATUS __nvoc_objCreateDynamic_Notifier(Notifier**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_Notifier(Notifier**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext);
#define __objCreate_Notifier(ppNewObj, pParent, createFlags, arg_pCallContext) \
    __nvoc_objCreate_Notifier((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext)

#define notifyGetNotificationListPtr(pNotifier) notifyGetNotificationListPtr_DISPATCH(pNotifier)
#define notifyGetNotificationShare(pNotifier) notifyGetNotificationShare_DISPATCH(pNotifier)
#define notifySetNotificationShare(pNotifier, pNotifShare) notifySetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define notifyUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) notifyUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define notifyGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) notifyGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)
PEVENTNOTIFICATION *notifyGetNotificationListPtr_IMPL(struct Notifier *pNotifier);

static inline PEVENTNOTIFICATION *notifyGetNotificationListPtr_DISPATCH(struct Notifier *pNotifier) {
    return pNotifier->__notifyGetNotificationListPtr__(pNotifier);
}

struct NotifShare *notifyGetNotificationShare_IMPL(struct Notifier *pNotifier);

static inline struct NotifShare *notifyGetNotificationShare_DISPATCH(struct Notifier *pNotifier) {
    return pNotifier->__notifyGetNotificationShare__(pNotifier);
}

void notifySetNotificationShare_IMPL(struct Notifier *pNotifier, struct NotifShare *pNotifShare);

static inline void notifySetNotificationShare_DISPATCH(struct Notifier *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__notifySetNotificationShare__(pNotifier, pNotifShare);
}

NV_STATUS notifyUnregisterEvent_IMPL(struct Notifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);

static inline NV_STATUS notifyUnregisterEvent_DISPATCH(struct Notifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__notifyUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

NV_STATUS notifyGetOrAllocNotifShare_IMPL(struct Notifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);

static inline NV_STATUS notifyGetOrAllocNotifShare_DISPATCH(struct Notifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__notifyGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

NV_STATUS notifyConstruct_IMPL(struct Notifier *arg_pNotifier, struct CALL_CONTEXT *arg_pCallContext);
#define __nvoc_notifyConstruct(arg_pNotifier, arg_pCallContext) notifyConstruct_IMPL(arg_pNotifier, arg_pCallContext)
void notifyDestruct_IMPL(struct Notifier *pNotifier);
#define __nvoc_notifyDestruct(pNotifier) notifyDestruct_IMPL(pNotifier)
#undef PRIVATE_FIELD


void CliAddSystemEvent(NvU32, NvU32);
NvBool CliDelObjectEvents(NvHandle hClient, NvHandle hObject);
NvBool CliGetEventInfo(NvHandle hClient, NvHandle hEvent, struct Event **ppEvent);
NV_STATUS CliGetEventNotificationList(NvHandle hClient, NvHandle hObject,
                                      struct INotifier **ppNotifier,
                                      PEVENTNOTIFICATION **pppEventNotification);

NV_STATUS registerEventNotification(PEVENTNOTIFICATION*, NvHandle, NvHandle, NvHandle, NvU32, NvU32, NvP64, NvBool);
NV_STATUS unregisterEventNotification(PEVENTNOTIFICATION*, NvHandle, NvHandle, NvHandle);
NV_STATUS unregisterEventNotificationWithData(PEVENTNOTIFICATION *, NvHandle, NvHandle, NvHandle, NvBool, NvP64);
NV_STATUS bindEventNotificationToSubdevice(PEVENTNOTIFICATION, NvHandle, NvU32);
NV_STATUS engineNonStallIntrNotify(OBJGPU *, NvU32);
NV_STATUS notifyEvents(OBJGPU*, EVENTNOTIFICATION*, NvU32, NvU32, NvU32, NV_STATUS, NvU32);
NV_STATUS engineNonStallIntrNotifyEvent(OBJGPU *, NvU32, NvHandle);

#endif // _EVENT_H_

#ifdef __cplusplus
} // extern "C"
#endif
#endif // _G_EVENT_NVOC_H_
