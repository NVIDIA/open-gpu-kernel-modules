
#ifndef _G_TMR_NVOC_H_
#define _G_TMR_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 1993-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "rmapi/event_api.h"

#include "ctrl/ctrl0004.h"

typedef struct OBJTMR *POBJTMR;

#ifndef __nvoc_class_id_OBJTMR
#define __nvoc_class_id_OBJTMR 0x9ddedeu
typedef struct OBJTMR OBJTMR;
#endif /* __nvoc_class_id_OBJTMR */



//---------------------------------------------------------------------------
//
//  Time objects.
//
//---------------------------------------------------------------------------

#define TIMER_STATE_IDLE        0
#define TIMER_STATE_BUSY        1

typedef NV_STATUS (*TIMEPROC)(OBJGPU *, struct OBJTMR *, TMR_EVENT *);
typedef NV_STATUS (*TIMEPROC_OBSOLETE)(OBJGPU *, struct OBJTMR *, void *);
typedef NV_STATUS (*TIMEPROC_COUNTDOWN)(OBJGPU *, THREAD_STATE_NODE *);

/*!
 * RM internal class representing NV01_TIMER (child of SubDevice)
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_TMR_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__TimerApi;
struct NVOC_METADATA__GpuResource;
struct NVOC_METADATA__Notifier;


struct TimerApi {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__TimerApi *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
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
    struct TimerApi *__nvoc_pbase_TimerApi;    // tmrapi
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__TimerApi {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResource metadata__GpuResource;
    const struct NVOC_METADATA__Notifier metadata__Notifier;
};

#ifndef __nvoc_class_id_TimerApi
#define __nvoc_class_id_TimerApi 0xb13ac4u
typedef struct TimerApi TimerApi;
#endif /* __nvoc_class_id_TimerApi */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_TimerApi;

#define __staticCast_TimerApi(pThis) \
    ((pThis)->__nvoc_pbase_TimerApi)

#ifdef __nvoc_tmr_h_disabled
#define __dynamicCast_TimerApi(pThis) ((TimerApi*) NULL)
#else //__nvoc_tmr_h_disabled
#define __dynamicCast_TimerApi(pThis) \
    ((TimerApi*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(TimerApi)))
#endif //__nvoc_tmr_h_disabled

NV_STATUS __nvoc_objCreateDynamic_TimerApi(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_TimerApi(TimerApi**, Dynamic*, NvU32, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __objCreate_TimerApi(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams) \
    __nvoc_objCreate_TimerApi((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pCallContext, pParams)


// Wrapper macros for implementation functions
NV_STATUS tmrapiConstruct_IMPL(struct TimerApi *pTimerApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __nvoc_tmrapiConstruct(pTimerApi, pCallContext, pParams) tmrapiConstruct_IMPL(pTimerApi, pCallContext, pParams)

void tmrapiDestruct_IMPL(struct TimerApi *pTimerApi);
#define __nvoc_tmrapiDestruct(pTimerApi) tmrapiDestruct_IMPL(pTimerApi)

void tmrapiDeregisterEvents_IMPL(struct TimerApi *pTimerApi);
#ifdef __nvoc_tmr_h_disabled
static inline void tmrapiDeregisterEvents(struct TimerApi *pTimerApi) {
    NV_ASSERT_FAILED_PRECOMP("TimerApi was disabled!");
}
#else // __nvoc_tmr_h_disabled
#define tmrapiDeregisterEvents(pTimerApi) tmrapiDeregisterEvents_IMPL(pTimerApi)
#endif // __nvoc_tmr_h_disabled

NV_STATUS tmrapiCtrlCmdTmrSetAlarmNotify_IMPL(struct TimerApi *pTimerApi, NV0004_CTRL_TMR_SET_ALARM_NOTIFY_PARAMS *pParams);
#ifdef __nvoc_tmr_h_disabled
static inline NV_STATUS tmrapiCtrlCmdTmrSetAlarmNotify(struct TimerApi *pTimerApi, NV0004_CTRL_TMR_SET_ALARM_NOTIFY_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("TimerApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_tmr_h_disabled
#define tmrapiCtrlCmdTmrSetAlarmNotify(pTimerApi, pParams) tmrapiCtrlCmdTmrSetAlarmNotify_IMPL(pTimerApi, pParams)
#endif // __nvoc_tmr_h_disabled


// Wrapper macros for halified functions
#define tmrapiGetRegBaseOffsetAndSize_FNPTR(pTimerApi) pTimerApi->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__
#define tmrapiGetRegBaseOffsetAndSize(pTimerApi, pGpu, pOffset, pSize) tmrapiGetRegBaseOffsetAndSize_DISPATCH(pTimerApi, pGpu, pOffset, pSize)
#define tmrapiControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__
#define tmrapiControl(pGpuResource, pCallContext, pParams) tmrapiControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define tmrapiMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__
#define tmrapiMap(pGpuResource, pCallContext, pParams, pCpuMapping) tmrapiMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define tmrapiUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__
#define tmrapiUnmap(pGpuResource, pCallContext, pCpuMapping) tmrapiUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define tmrapiShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__
#define tmrapiShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) tmrapiShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define tmrapiGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__
#define tmrapiGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) tmrapiGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define tmrapiInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__
#define tmrapiInternalControlForward(pGpuResource, command, pParams, size) tmrapiInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define tmrapiGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__
#define tmrapiGetInternalObjectHandle(pGpuResource) tmrapiGetInternalObjectHandle_DISPATCH(pGpuResource)
#define tmrapiAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__
#define tmrapiAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) tmrapiAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define tmrapiGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define tmrapiGetMemInterMapParams(pRmResource, pParams) tmrapiGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define tmrapiCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define tmrapiCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) tmrapiCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define tmrapiGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define tmrapiGetMemoryMappingDescriptor(pRmResource, ppMemDesc) tmrapiGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define tmrapiControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__
#define tmrapiControlSerialization_Prologue(pResource, pCallContext, pParams) tmrapiControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define tmrapiControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__
#define tmrapiControlSerialization_Epilogue(pResource, pCallContext, pParams) tmrapiControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define tmrapiControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__
#define tmrapiControl_Prologue(pResource, pCallContext, pParams) tmrapiControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define tmrapiControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__
#define tmrapiControl_Epilogue(pResource, pCallContext, pParams) tmrapiControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define tmrapiCanCopy_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define tmrapiCanCopy(pResource) tmrapiCanCopy_DISPATCH(pResource)
#define tmrapiIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define tmrapiIsDuplicate(pResource, hMemory, pDuplicate) tmrapiIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define tmrapiPreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define tmrapiPreDestruct(pResource) tmrapiPreDestruct_DISPATCH(pResource)
#define tmrapiControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define tmrapiControlFilter(pResource, pCallContext, pParams) tmrapiControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define tmrapiIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define tmrapiIsPartialUnmapSupported(pResource) tmrapiIsPartialUnmapSupported_DISPATCH(pResource)
#define tmrapiMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define tmrapiMapTo(pResource, pParams) tmrapiMapTo_DISPATCH(pResource, pParams)
#define tmrapiUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define tmrapiUnmapFrom(pResource, pParams) tmrapiUnmapFrom_DISPATCH(pResource, pParams)
#define tmrapiGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define tmrapiGetRefCount(pResource) tmrapiGetRefCount_DISPATCH(pResource)
#define tmrapiAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define tmrapiAddAdditionalDependants(pClient, pResource, pReference) tmrapiAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define tmrapiGetNotificationListPtr_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetNotificationListPtr__
#define tmrapiGetNotificationListPtr(pNotifier) tmrapiGetNotificationListPtr_DISPATCH(pNotifier)
#define tmrapiGetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetNotificationShare__
#define tmrapiGetNotificationShare(pNotifier) tmrapiGetNotificationShare_DISPATCH(pNotifier)
#define tmrapiSetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifySetNotificationShare__
#define tmrapiSetNotificationShare(pNotifier, pNotifShare) tmrapiSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define tmrapiUnregisterEvent_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyUnregisterEvent__
#define tmrapiUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) tmrapiUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define tmrapiGetOrAllocNotifShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetOrAllocNotifShare__
#define tmrapiGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) tmrapiGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)

// Dispatch functions
static inline NV_STATUS tmrapiGetRegBaseOffsetAndSize_DISPATCH(struct TimerApi *pTimerApi, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pTimerApi->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__(staticCast(pTimerApi, GpuResource), pGpu, pOffset, pSize);
}

static inline NV_STATUS tmrapiControl_DISPATCH(struct TimerApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__(staticCast(pGpuResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS tmrapiMap_DISPATCH(struct TimerApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__(staticCast(pGpuResource, RsResource), pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS tmrapiUnmap_DISPATCH(struct TimerApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__(staticCast(pGpuResource, RsResource), pCallContext, pCpuMapping);
}

static inline NvBool tmrapiShareCallback_DISPATCH(struct TimerApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__(staticCast(pGpuResource, RsResource), pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS tmrapiGetMapAddrSpace_DISPATCH(struct TimerApi *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__(staticCast(pGpuResource, GpuResource), pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS tmrapiInternalControlForward_DISPATCH(struct TimerApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__(staticCast(pGpuResource, GpuResource), command, pParams, size);
}

static inline NvHandle tmrapiGetInternalObjectHandle_DISPATCH(struct TimerApi *pGpuResource) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__(staticCast(pGpuResource, GpuResource));
}

static inline NvBool tmrapiAccessCallback_DISPATCH(struct TimerApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__(staticCast(pResource, RsResource), pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS tmrapiGetMemInterMapParams_DISPATCH(struct TimerApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__(staticCast(pRmResource, RmResource), pParams);
}

static inline NV_STATUS tmrapiCheckMemInterUnmap_DISPATCH(struct TimerApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__(staticCast(pRmResource, RmResource), bSubdeviceHandleProvided);
}

static inline NV_STATUS tmrapiGetMemoryMappingDescriptor_DISPATCH(struct TimerApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__(staticCast(pRmResource, RmResource), ppMemDesc);
}

static inline NV_STATUS tmrapiControlSerialization_Prologue_DISPATCH(struct TimerApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void tmrapiControlSerialization_Epilogue_DISPATCH(struct TimerApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS tmrapiControl_Prologue_DISPATCH(struct TimerApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void tmrapiControl_Epilogue_DISPATCH(struct TimerApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool tmrapiCanCopy_DISPATCH(struct TimerApi *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__(staticCast(pResource, RsResource));
}

static inline NV_STATUS tmrapiIsDuplicate_DISPATCH(struct TimerApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__(staticCast(pResource, RsResource), hMemory, pDuplicate);
}

static inline void tmrapiPreDestruct_DISPATCH(struct TimerApi *pResource) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__(staticCast(pResource, RsResource));
}

static inline NV_STATUS tmrapiControlFilter_DISPATCH(struct TimerApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool tmrapiIsPartialUnmapSupported_DISPATCH(struct TimerApi *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__(staticCast(pResource, RsResource));
}

static inline NV_STATUS tmrapiMapTo_DISPATCH(struct TimerApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__(staticCast(pResource, RsResource), pParams);
}

static inline NV_STATUS tmrapiUnmapFrom_DISPATCH(struct TimerApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__(staticCast(pResource, RsResource), pParams);
}

static inline NvU32 tmrapiGetRefCount_DISPATCH(struct TimerApi *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__(staticCast(pResource, RsResource));
}

static inline void tmrapiAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct TimerApi *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__(pClient, staticCast(pResource, RsResource), pReference);
}

static inline PEVENTNOTIFICATION * tmrapiGetNotificationListPtr_DISPATCH(struct TimerApi *pNotifier) {
    return pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetNotificationListPtr__(staticCast(pNotifier, INotifier));
}

static inline struct NotifShare * tmrapiGetNotificationShare_DISPATCH(struct TimerApi *pNotifier) {
    return pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetNotificationShare__(staticCast(pNotifier, INotifier));
}

static inline void tmrapiSetNotificationShare_DISPATCH(struct TimerApi *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifySetNotificationShare__(staticCast(pNotifier, INotifier), pNotifShare);
}

static inline NV_STATUS tmrapiUnregisterEvent_DISPATCH(struct TimerApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyUnregisterEvent__(staticCast(pNotifier, INotifier), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS tmrapiGetOrAllocNotifShare_DISPATCH(struct TimerApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetOrAllocNotifShare__(staticCast(pNotifier, INotifier), hNotifierClient, hNotifierResource, ppNotifShare);
}

// Virtual method declarations and/or inline definitions
NV_STATUS tmrapiGetRegBaseOffsetAndSize_IMPL(struct TimerApi *pTimerApi, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize);

// Exported method declarations and/or inline definitions
NV_STATUS tmrapiCtrlCmdTmrSetAlarmNotify_IMPL(struct TimerApi *pTimerApi, NV0004_CTRL_TMR_SET_ALARM_NOTIFY_PARAMS *pParams);

// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
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
