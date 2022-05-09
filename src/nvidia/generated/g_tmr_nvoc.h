#ifndef _G_TMR_NVOC_H_
#define _G_TMR_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_tmr_nvoc.h"

#ifndef _TMR_H_
#define _TMR_H_

/****************************** Timer Module *******************************\
*                                                                           *
* Module: TMR.H                                                             *
*       Timer functions.                                                    *
*                                                                           *
****************************************************************************/

#include "core/core.h"
#include "gpu/gpu_resource.h"
#include "rmapi/event.h"

#include "ctrl/ctrl0004.h"

typedef struct OBJTMR *POBJTMR;

#ifndef __NVOC_CLASS_OBJTMR_TYPEDEF__
#define __NVOC_CLASS_OBJTMR_TYPEDEF__
typedef struct OBJTMR OBJTMR;
#endif /* __NVOC_CLASS_OBJTMR_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJTMR
#define __nvoc_class_id_OBJTMR 0x9ddede
#endif /* __nvoc_class_id_OBJTMR */



//---------------------------------------------------------------------------
//
//  Time objects.
//
//---------------------------------------------------------------------------

#define TIMER_STATE_IDLE        0
#define TIMER_STATE_BUSY        1

// Opaque callback memory type for interfacing the scheduling API
typedef struct TMR_EVENT *PTMR_EVENT;
typedef struct TMR_EVENT TMR_EVENT;

typedef NV_STATUS (*TIMEPROC)(OBJGPU *, struct OBJTMR *, PTMR_EVENT);
typedef NV_STATUS (*TIMEPROC_OBSOLETE)(OBJGPU *, struct OBJTMR *, void *);
typedef NV_STATUS (*TIMEPROC_COUNTDOWN)(OBJGPU *, THREAD_STATE_NODE *);

/*!
 * RM internal class representing NV01_TIMER (child of SubDevice)
 */
#ifdef NVOC_TMR_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct TimerApi {
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
    struct TimerApi *__nvoc_pbase_TimerApi;
    NV_STATUS (*__tmrapiGetRegBaseOffsetAndSize__)(struct TimerApi *, struct OBJGPU *, NvU32 *, NvU32 *);
    NV_STATUS (*__tmrapiCtrlCmdTmrSetAlarmNotify__)(struct TimerApi *, NV0004_CTRL_TMR_SET_ALARM_NOTIFY_PARAMS *);
    NvBool (*__tmrapiShareCallback__)(struct TimerApi *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__tmrapiMapTo__)(struct TimerApi *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__tmrapiGetOrAllocNotifShare__)(struct TimerApi *, NvHandle, NvHandle, struct NotifShare **);
    NV_STATUS (*__tmrapiCheckMemInterUnmap__)(struct TimerApi *, NvBool);
    NV_STATUS (*__tmrapiGetMapAddrSpace__)(struct TimerApi *, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    void (*__tmrapiSetNotificationShare__)(struct TimerApi *, struct NotifShare *);
    NvU32 (*__tmrapiGetRefCount__)(struct TimerApi *);
    void (*__tmrapiAddAdditionalDependants__)(struct RsClient *, struct TimerApi *, RsResourceRef *);
    NV_STATUS (*__tmrapiControl_Prologue__)(struct TimerApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__tmrapiInternalControlForward__)(struct TimerApi *, NvU32, void *, NvU32);
    NV_STATUS (*__tmrapiUnmapFrom__)(struct TimerApi *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__tmrapiControl_Epilogue__)(struct TimerApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__tmrapiControlLookup__)(struct TimerApi *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NvHandle (*__tmrapiGetInternalObjectHandle__)(struct TimerApi *);
    NV_STATUS (*__tmrapiControl__)(struct TimerApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__tmrapiUnmap__)(struct TimerApi *, struct CALL_CONTEXT *, struct RsCpuMapping *);
    NV_STATUS (*__tmrapiGetMemInterMapParams__)(struct TimerApi *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__tmrapiGetMemoryMappingDescriptor__)(struct TimerApi *, struct MEMORY_DESCRIPTOR **);
    NV_STATUS (*__tmrapiControlFilter__)(struct TimerApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__tmrapiUnregisterEvent__)(struct TimerApi *, NvHandle, NvHandle, NvHandle, NvHandle);
    NvBool (*__tmrapiCanCopy__)(struct TimerApi *);
    void (*__tmrapiPreDestruct__)(struct TimerApi *);
    PEVENTNOTIFICATION *(*__tmrapiGetNotificationListPtr__)(struct TimerApi *);
    struct NotifShare *(*__tmrapiGetNotificationShare__)(struct TimerApi *);
    NV_STATUS (*__tmrapiMap__)(struct TimerApi *, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);
    NvBool (*__tmrapiAccessCallback__)(struct TimerApi *, struct RsClient *, void *, RsAccessRight);
};

#ifndef __NVOC_CLASS_TimerApi_TYPEDEF__
#define __NVOC_CLASS_TimerApi_TYPEDEF__
typedef struct TimerApi TimerApi;
#endif /* __NVOC_CLASS_TimerApi_TYPEDEF__ */

#ifndef __nvoc_class_id_TimerApi
#define __nvoc_class_id_TimerApi 0xb13ac4
#endif /* __nvoc_class_id_TimerApi */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_TimerApi;

#define __staticCast_TimerApi(pThis) \
    ((pThis)->__nvoc_pbase_TimerApi)

#ifdef __nvoc_tmr_h_disabled
#define __dynamicCast_TimerApi(pThis) ((TimerApi*)NULL)
#else //__nvoc_tmr_h_disabled
#define __dynamicCast_TimerApi(pThis) \
    ((TimerApi*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(TimerApi)))
#endif //__nvoc_tmr_h_disabled


NV_STATUS __nvoc_objCreateDynamic_TimerApi(TimerApi**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_TimerApi(TimerApi**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_TimerApi(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_TimerApi((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define tmrapiGetRegBaseOffsetAndSize(pTimerApi, pGpu, pOffset, pSize) tmrapiGetRegBaseOffsetAndSize_DISPATCH(pTimerApi, pGpu, pOffset, pSize)
#define tmrapiCtrlCmdTmrSetAlarmNotify(pTimerApi, pParams) tmrapiCtrlCmdTmrSetAlarmNotify_DISPATCH(pTimerApi, pParams)
#define tmrapiShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) tmrapiShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define tmrapiMapTo(pResource, pParams) tmrapiMapTo_DISPATCH(pResource, pParams)
#define tmrapiGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) tmrapiGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)
#define tmrapiCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) tmrapiCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define tmrapiGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) tmrapiGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define tmrapiSetNotificationShare(pNotifier, pNotifShare) tmrapiSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define tmrapiGetRefCount(pResource) tmrapiGetRefCount_DISPATCH(pResource)
#define tmrapiAddAdditionalDependants(pClient, pResource, pReference) tmrapiAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define tmrapiControl_Prologue(pResource, pCallContext, pParams) tmrapiControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define tmrapiInternalControlForward(pGpuResource, command, pParams, size) tmrapiInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define tmrapiUnmapFrom(pResource, pParams) tmrapiUnmapFrom_DISPATCH(pResource, pParams)
#define tmrapiControl_Epilogue(pResource, pCallContext, pParams) tmrapiControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define tmrapiControlLookup(pResource, pParams, ppEntry) tmrapiControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define tmrapiGetInternalObjectHandle(pGpuResource) tmrapiGetInternalObjectHandle_DISPATCH(pGpuResource)
#define tmrapiControl(pGpuResource, pCallContext, pParams) tmrapiControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define tmrapiUnmap(pGpuResource, pCallContext, pCpuMapping) tmrapiUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define tmrapiGetMemInterMapParams(pRmResource, pParams) tmrapiGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define tmrapiGetMemoryMappingDescriptor(pRmResource, ppMemDesc) tmrapiGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define tmrapiControlFilter(pResource, pCallContext, pParams) tmrapiControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define tmrapiUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) tmrapiUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define tmrapiCanCopy(pResource) tmrapiCanCopy_DISPATCH(pResource)
#define tmrapiPreDestruct(pResource) tmrapiPreDestruct_DISPATCH(pResource)
#define tmrapiGetNotificationListPtr(pNotifier) tmrapiGetNotificationListPtr_DISPATCH(pNotifier)
#define tmrapiGetNotificationShare(pNotifier) tmrapiGetNotificationShare_DISPATCH(pNotifier)
#define tmrapiMap(pGpuResource, pCallContext, pParams, pCpuMapping) tmrapiMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define tmrapiAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) tmrapiAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
NV_STATUS tmrapiGetRegBaseOffsetAndSize_IMPL(struct TimerApi *pTimerApi, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize);

static inline NV_STATUS tmrapiGetRegBaseOffsetAndSize_DISPATCH(struct TimerApi *pTimerApi, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pTimerApi->__tmrapiGetRegBaseOffsetAndSize__(pTimerApi, pGpu, pOffset, pSize);
}

NV_STATUS tmrapiCtrlCmdTmrSetAlarmNotify_IMPL(struct TimerApi *pTimerApi, NV0004_CTRL_TMR_SET_ALARM_NOTIFY_PARAMS *pParams);

static inline NV_STATUS tmrapiCtrlCmdTmrSetAlarmNotify_DISPATCH(struct TimerApi *pTimerApi, NV0004_CTRL_TMR_SET_ALARM_NOTIFY_PARAMS *pParams) {
    return pTimerApi->__tmrapiCtrlCmdTmrSetAlarmNotify__(pTimerApi, pParams);
}

static inline NvBool tmrapiShareCallback_DISPATCH(struct TimerApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__tmrapiShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS tmrapiMapTo_DISPATCH(struct TimerApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__tmrapiMapTo__(pResource, pParams);
}

static inline NV_STATUS tmrapiGetOrAllocNotifShare_DISPATCH(struct TimerApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__tmrapiGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

static inline NV_STATUS tmrapiCheckMemInterUnmap_DISPATCH(struct TimerApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__tmrapiCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS tmrapiGetMapAddrSpace_DISPATCH(struct TimerApi *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__tmrapiGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline void tmrapiSetNotificationShare_DISPATCH(struct TimerApi *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__tmrapiSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NvU32 tmrapiGetRefCount_DISPATCH(struct TimerApi *pResource) {
    return pResource->__tmrapiGetRefCount__(pResource);
}

static inline void tmrapiAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct TimerApi *pResource, RsResourceRef *pReference) {
    pResource->__tmrapiAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS tmrapiControl_Prologue_DISPATCH(struct TimerApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__tmrapiControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS tmrapiInternalControlForward_DISPATCH(struct TimerApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__tmrapiInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NV_STATUS tmrapiUnmapFrom_DISPATCH(struct TimerApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__tmrapiUnmapFrom__(pResource, pParams);
}

static inline void tmrapiControl_Epilogue_DISPATCH(struct TimerApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__tmrapiControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS tmrapiControlLookup_DISPATCH(struct TimerApi *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__tmrapiControlLookup__(pResource, pParams, ppEntry);
}

static inline NvHandle tmrapiGetInternalObjectHandle_DISPATCH(struct TimerApi *pGpuResource) {
    return pGpuResource->__tmrapiGetInternalObjectHandle__(pGpuResource);
}

static inline NV_STATUS tmrapiControl_DISPATCH(struct TimerApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__tmrapiControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS tmrapiUnmap_DISPATCH(struct TimerApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__tmrapiUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NV_STATUS tmrapiGetMemInterMapParams_DISPATCH(struct TimerApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__tmrapiGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS tmrapiGetMemoryMappingDescriptor_DISPATCH(struct TimerApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__tmrapiGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS tmrapiControlFilter_DISPATCH(struct TimerApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__tmrapiControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS tmrapiUnregisterEvent_DISPATCH(struct TimerApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__tmrapiUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NvBool tmrapiCanCopy_DISPATCH(struct TimerApi *pResource) {
    return pResource->__tmrapiCanCopy__(pResource);
}

static inline void tmrapiPreDestruct_DISPATCH(struct TimerApi *pResource) {
    pResource->__tmrapiPreDestruct__(pResource);
}

static inline PEVENTNOTIFICATION *tmrapiGetNotificationListPtr_DISPATCH(struct TimerApi *pNotifier) {
    return pNotifier->__tmrapiGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare *tmrapiGetNotificationShare_DISPATCH(struct TimerApi *pNotifier) {
    return pNotifier->__tmrapiGetNotificationShare__(pNotifier);
}

static inline NV_STATUS tmrapiMap_DISPATCH(struct TimerApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__tmrapiMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NvBool tmrapiAccessCallback_DISPATCH(struct TimerApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__tmrapiAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

NV_STATUS tmrapiConstruct_IMPL(struct TimerApi *arg_pTimerApi, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __nvoc_tmrapiConstruct(arg_pTimerApi, arg_pCallContext, arg_pParams) tmrapiConstruct_IMPL(arg_pTimerApi, arg_pCallContext, arg_pParams)
void tmrapiDestruct_IMPL(struct TimerApi *pTimerApi);
#define __nvoc_tmrapiDestruct(pTimerApi) tmrapiDestruct_IMPL(pTimerApi)
void tmrapiDeregisterEvents_IMPL(struct TimerApi *pTimerApi);
#ifdef __nvoc_tmr_h_disabled
static inline void tmrapiDeregisterEvents(struct TimerApi *pTimerApi) {
    NV_ASSERT_FAILED_PRECOMP("TimerApi was disabled!");
}
#else //__nvoc_tmr_h_disabled
#define tmrapiDeregisterEvents(pTimerApi) tmrapiDeregisterEvents_IMPL(pTimerApi)
#endif //__nvoc_tmr_h_disabled

#undef PRIVATE_FIELD



//---------------------------------------------------------------------------
//
//  Function prototypes.
//
//---------------------------------------------------------------------------


#endif // _TMR_H_


#ifdef __cplusplus
} // extern "C"
#endif
#endif // _G_TMR_NVOC_H_
