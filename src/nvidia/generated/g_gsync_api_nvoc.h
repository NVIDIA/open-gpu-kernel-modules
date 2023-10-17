#ifndef _G_GSYNC_API_NVOC_H_
#define _G_GSYNC_API_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2006-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_gsync_api_nvoc.h"

#ifndef GSYNCAPI_H_
#define GSYNCAPI_H_

/* ------------------------ Includes --------------------------------------- */
#include "ctrl/ctrl30f1.h"
#include "resserv/resserv.h"
#include "rmapi/resource.h"
#include "core/core.h"
#include "rmapi/event.h"

/* ------------------------ Macros & Defines ------------------------------- */

#ifdef NVOC_GSYNC_API_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct GSyncApi {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct RmResource __nvoc_base_RmResource;
    struct Notifier __nvoc_base_Notifier;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct INotifier *__nvoc_pbase_INotifier;
    struct Notifier *__nvoc_pbase_Notifier;
    struct GSyncApi *__nvoc_pbase_GSyncApi;
    NV_STATUS (*__gsyncapiControl__)(struct GSyncApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__gsyncapiCtrlCmdGsyncGetVersion__)(struct GSyncApi *, NV30F1_CTRL_GSYNC_GET_VERSION_PARAMS *);
    NV_STATUS (*__gsyncapiCtrlCmdGetGsyncGpuTopology__)(struct GSyncApi *, NV30F1_CTRL_GET_GSYNC_GPU_TOPOLOGY_PARAMS *);
    NV_STATUS (*__gsyncapiCtrlCmdGsyncGetStatusSignals__)(struct GSyncApi *, NV30F1_CTRL_GSYNC_GET_STATUS_SIGNALS_PARAMS *);
    NV_STATUS (*__gsyncapiCtrlCmdGsyncGetControlParams__)(struct GSyncApi *, NV30F1_CTRL_GSYNC_GET_CONTROL_PARAMS_PARAMS *);
    NV_STATUS (*__gsyncapiCtrlCmdGsyncSetControlParams__)(struct GSyncApi *, NV30F1_CTRL_GSYNC_SET_CONTROL_PARAMS_PARAMS *);
    NV_STATUS (*__gsyncapiCtrlCmdGsyncGetControlSync__)(struct GSyncApi *, NV30F1_CTRL_GSYNC_GET_CONTROL_SYNC_PARAMS *);
    NV_STATUS (*__gsyncapiCtrlCmdGsyncSetControlSync__)(struct GSyncApi *, NV30F1_CTRL_GSYNC_SET_CONTROL_SYNC_PARAMS *);
    NV_STATUS (*__gsyncapiCtrlCmdGsyncSetControlUnsync__)(struct GSyncApi *, NV30F1_CTRL_GSYNC_SET_CONTROL_UNSYNC_PARAMS *);
    NV_STATUS (*__gsyncapiCtrlCmdGsyncGetStatusSync__)(struct GSyncApi *, NV30F1_CTRL_GSYNC_GET_STATUS_SYNC_PARAMS *);
    NV_STATUS (*__gsyncapiCtrlCmdGsyncGetStatus__)(struct GSyncApi *, NV30F1_CTRL_GSYNC_GET_STATUS_PARAMS *);
    NV_STATUS (*__gsyncapiCtrlCmdGsyncGetControlTesting__)(struct GSyncApi *, NV30F1_CTRL_GSYNC_GET_CONTROL_TESTING_PARAMS *);
    NV_STATUS (*__gsyncapiCtrlCmdGsyncSetControlTesting__)(struct GSyncApi *, NV30F1_CTRL_GSYNC_SET_CONTROL_TESTING_PARAMS *);
    NV_STATUS (*__gsyncapiCtrlCmdGsyncSetControlWatchdog__)(struct GSyncApi *, NV30F1_CTRL_GSYNC_SET_CONTROL_WATCHDOG_PARAMS *);
    NV_STATUS (*__gsyncapiCtrlCmdGsyncGetControlInterlaceMode__)(struct GSyncApi *, NV30F1_CTRL_GSYNC_GET_CONTROL_INTERLACE_MODE_PARAMS *);
    NV_STATUS (*__gsyncapiCtrlCmdGsyncSetControlInterlaceMode__)(struct GSyncApi *, NV30F1_CTRL_GSYNC_SET_CONTROL_INTERLACE_MODE_PARAMS *);
    NV_STATUS (*__gsyncapiCtrlCmdGsyncGetControlSwapBarrier__)(struct GSyncApi *, NV30F1_CTRL_GSYNC_GET_CONTROL_SWAP_BARRIER_PARAMS *);
    NV_STATUS (*__gsyncapiCtrlCmdGsyncSetControlSwapBarrier__)(struct GSyncApi *, NV30F1_CTRL_GSYNC_SET_CONTROL_SWAP_BARRIER_PARAMS *);
    NV_STATUS (*__gsyncapiCtrlCmdGsyncGetControlSwapLockWindow__)(struct GSyncApi *, NV30F1_CTRL_GSYNC_GET_CONTROL_SWAP_LOCK_WINDOW_PARAMS *);
    NV_STATUS (*__gsyncapiCtrlCmdGsyncGetCaps__)(struct GSyncApi *, NV30F1_CTRL_GSYNC_GET_CAPS_PARAMS *);
    NV_STATUS (*__gsyncapiCtrlCmdGsyncGetOptimizedTiming__)(struct GSyncApi *, NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_PARAMS *);
    NV_STATUS (*__gsyncapiCtrlCmdGsyncSetEventNotification__)(struct GSyncApi *, NV30F1_CTRL_GSYNC_SET_EVENT_NOTIFICATION_PARAMS *);
    NV_STATUS (*__gsyncapiCtrlCmdGsyncGetControlStereoLockMode__)(struct GSyncApi *, NV30F1_CTRL_CMD_GSYNC_GET_CONTROL_STEREO_LOCK_MODE_PARAMS *);
    NV_STATUS (*__gsyncapiCtrlCmdGsyncSetControlStereoLockMode__)(struct GSyncApi *, NV30F1_CTRL_CMD_GSYNC_SET_CONTROL_STEREO_LOCK_MODE_PARAMS *);
    NV_STATUS (*__gsyncapiCtrlCmdGsyncReadRegister__)(struct GSyncApi *, NV30F1_CTRL_GSYNC_READ_REGISTER_PARAMS *);
    NV_STATUS (*__gsyncapiCtrlCmdGsyncWriteRegister__)(struct GSyncApi *, NV30F1_CTRL_GSYNC_WRITE_REGISTER_PARAMS *);
    NV_STATUS (*__gsyncapiCtrlCmdGsyncSetLocalSync__)(struct GSyncApi *, NV30F1_CTRL_GSYNC_SET_LOCAL_SYNC_PARAMS *);
    NV_STATUS (*__gsyncapiCtrlCmdGsyncConfigFlash__)(struct GSyncApi *, NV30F1_CTRL_CMD_GSYNC_CONFIG_FLASH_PARAMS *);
    NV_STATUS (*__gsyncapiCtrlCmdGsyncGetHouseSyncMode__)(struct GSyncApi *, NV30F1_CTRL_GSYNC_HOUSE_SYNC_MODE_PARAMS *);
    NV_STATUS (*__gsyncapiCtrlCmdGsyncSetHouseSyncMode__)(struct GSyncApi *, NV30F1_CTRL_GSYNC_HOUSE_SYNC_MODE_PARAMS *);
    NvBool (*__gsyncapiShareCallback__)(struct GSyncApi *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__gsyncapiCheckMemInterUnmap__)(struct GSyncApi *, NvBool);
    NV_STATUS (*__gsyncapiMapTo__)(struct GSyncApi *, RS_RES_MAP_TO_PARAMS *);
    NvBool (*__gsyncapiAccessCallback__)(struct GSyncApi *, struct RsClient *, void *, RsAccessRight);
    void (*__gsyncapiSetNotificationShare__)(struct GSyncApi *, struct NotifShare *);
    NvU32 (*__gsyncapiGetRefCount__)(struct GSyncApi *);
    void (*__gsyncapiAddAdditionalDependants__)(struct RsClient *, struct GSyncApi *, RsResourceRef *);
    NV_STATUS (*__gsyncapiControl_Prologue__)(struct GSyncApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__gsyncapiUnmapFrom__)(struct GSyncApi *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__gsyncapiControl_Epilogue__)(struct GSyncApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__gsyncapiControlLookup__)(struct GSyncApi *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NV_STATUS (*__gsyncapiUnmap__)(struct GSyncApi *, struct CALL_CONTEXT *, RsCpuMapping *);
    NV_STATUS (*__gsyncapiGetMemInterMapParams__)(struct GSyncApi *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__gsyncapiGetMemoryMappingDescriptor__)(struct GSyncApi *, struct MEMORY_DESCRIPTOR **);
    NV_STATUS (*__gsyncapiControlFilter__)(struct GSyncApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__gsyncapiUnregisterEvent__)(struct GSyncApi *, NvHandle, NvHandle, NvHandle, NvHandle);
    NV_STATUS (*__gsyncapiControlSerialization_Prologue__)(struct GSyncApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__gsyncapiCanCopy__)(struct GSyncApi *);
    void (*__gsyncapiPreDestruct__)(struct GSyncApi *);
    NV_STATUS (*__gsyncapiIsDuplicate__)(struct GSyncApi *, NvHandle, NvBool *);
    void (*__gsyncapiControlSerialization_Epilogue__)(struct GSyncApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    PEVENTNOTIFICATION *(*__gsyncapiGetNotificationListPtr__)(struct GSyncApi *);
    struct NotifShare *(*__gsyncapiGetNotificationShare__)(struct GSyncApi *);
    NV_STATUS (*__gsyncapiMap__)(struct GSyncApi *, struct CALL_CONTEXT *, RS_CPU_MAP_PARAMS *, RsCpuMapping *);
    NV_STATUS (*__gsyncapiGetOrAllocNotifShare__)(struct GSyncApi *, NvHandle, NvHandle, struct NotifShare **);
    NvU32 instance;
    NvU32 classNum;
    NvU32 notifyAction;
    NvU32 lastEventNotified;
    PEVENTNOTIFICATION pEventByType[9];
    NvBool oldEventNotification;
};

#ifndef __NVOC_CLASS_GSyncApi_TYPEDEF__
#define __NVOC_CLASS_GSyncApi_TYPEDEF__
typedef struct GSyncApi GSyncApi;
#endif /* __NVOC_CLASS_GSyncApi_TYPEDEF__ */

#ifndef __nvoc_class_id_GSyncApi
#define __nvoc_class_id_GSyncApi 0x214628
#endif /* __nvoc_class_id_GSyncApi */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GSyncApi;

#define __staticCast_GSyncApi(pThis) \
    ((pThis)->__nvoc_pbase_GSyncApi)

#ifdef __nvoc_gsync_api_h_disabled
#define __dynamicCast_GSyncApi(pThis) ((GSyncApi*)NULL)
#else //__nvoc_gsync_api_h_disabled
#define __dynamicCast_GSyncApi(pThis) \
    ((GSyncApi*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(GSyncApi)))
#endif //__nvoc_gsync_api_h_disabled


NV_STATUS __nvoc_objCreateDynamic_GSyncApi(GSyncApi**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_GSyncApi(GSyncApi**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_GSyncApi(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_GSyncApi((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define gsyncapiControl(pGsyncApi, pCallContext, pParams) gsyncapiControl_DISPATCH(pGsyncApi, pCallContext, pParams)
#define gsyncapiCtrlCmdGsyncGetVersion(pGsyncApi, pGsyncGetVersionParams) gsyncapiCtrlCmdGsyncGetVersion_DISPATCH(pGsyncApi, pGsyncGetVersionParams)
#define gsyncapiCtrlCmdGetGsyncGpuTopology(pGsyncApi, pParams) gsyncapiCtrlCmdGetGsyncGpuTopology_DISPATCH(pGsyncApi, pParams)
#define gsyncapiCtrlCmdGsyncGetStatusSignals(pGsyncApi, pParams) gsyncapiCtrlCmdGsyncGetStatusSignals_DISPATCH(pGsyncApi, pParams)
#define gsyncapiCtrlCmdGsyncGetControlParams(pGsyncApi, pParams) gsyncapiCtrlCmdGsyncGetControlParams_DISPATCH(pGsyncApi, pParams)
#define gsyncapiCtrlCmdGsyncSetControlParams(pGsyncApi, pParams) gsyncapiCtrlCmdGsyncSetControlParams_DISPATCH(pGsyncApi, pParams)
#define gsyncapiCtrlCmdGsyncGetControlSync(pGsyncApi, pParams) gsyncapiCtrlCmdGsyncGetControlSync_DISPATCH(pGsyncApi, pParams)
#define gsyncapiCtrlCmdGsyncSetControlSync(pGsyncApi, pParams) gsyncapiCtrlCmdGsyncSetControlSync_DISPATCH(pGsyncApi, pParams)
#define gsyncapiCtrlCmdGsyncSetControlUnsync(pGsyncApi, pParams) gsyncapiCtrlCmdGsyncSetControlUnsync_DISPATCH(pGsyncApi, pParams)
#define gsyncapiCtrlCmdGsyncGetStatusSync(pGsyncApi, pParams) gsyncapiCtrlCmdGsyncGetStatusSync_DISPATCH(pGsyncApi, pParams)
#define gsyncapiCtrlCmdGsyncGetStatus(pGsyncApi, pParams) gsyncapiCtrlCmdGsyncGetStatus_DISPATCH(pGsyncApi, pParams)
#define gsyncapiCtrlCmdGsyncGetControlTesting(pGsyncApi, pParams) gsyncapiCtrlCmdGsyncGetControlTesting_DISPATCH(pGsyncApi, pParams)
#define gsyncapiCtrlCmdGsyncSetControlTesting(pGsyncApi, pParams) gsyncapiCtrlCmdGsyncSetControlTesting_DISPATCH(pGsyncApi, pParams)
#define gsyncapiCtrlCmdGsyncSetControlWatchdog(pGsyncApi, pParams) gsyncapiCtrlCmdGsyncSetControlWatchdog_DISPATCH(pGsyncApi, pParams)
#define gsyncapiCtrlCmdGsyncGetControlInterlaceMode(pGsyncApi, pParams) gsyncapiCtrlCmdGsyncGetControlInterlaceMode_DISPATCH(pGsyncApi, pParams)
#define gsyncapiCtrlCmdGsyncSetControlInterlaceMode(pGsyncApi, pParams) gsyncapiCtrlCmdGsyncSetControlInterlaceMode_DISPATCH(pGsyncApi, pParams)
#define gsyncapiCtrlCmdGsyncGetControlSwapBarrier(pGsyncApi, pParams) gsyncapiCtrlCmdGsyncGetControlSwapBarrier_DISPATCH(pGsyncApi, pParams)
#define gsyncapiCtrlCmdGsyncSetControlSwapBarrier(pGsyncApi, pParams) gsyncapiCtrlCmdGsyncSetControlSwapBarrier_DISPATCH(pGsyncApi, pParams)
#define gsyncapiCtrlCmdGsyncGetControlSwapLockWindow(pGsyncApi, pParams) gsyncapiCtrlCmdGsyncGetControlSwapLockWindow_DISPATCH(pGsyncApi, pParams)
#define gsyncapiCtrlCmdGsyncGetCaps(pGsyncApi, pParams) gsyncapiCtrlCmdGsyncGetCaps_DISPATCH(pGsyncApi, pParams)
#define gsyncapiCtrlCmdGsyncGetOptimizedTiming(pGsyncApi, pParams) gsyncapiCtrlCmdGsyncGetOptimizedTiming_DISPATCH(pGsyncApi, pParams)
#define gsyncapiCtrlCmdGsyncSetEventNotification(pGsyncApi, pSetEventParams) gsyncapiCtrlCmdGsyncSetEventNotification_DISPATCH(pGsyncApi, pSetEventParams)
#define gsyncapiCtrlCmdGsyncGetControlStereoLockMode(pGsyncApi, pParams) gsyncapiCtrlCmdGsyncGetControlStereoLockMode_DISPATCH(pGsyncApi, pParams)
#define gsyncapiCtrlCmdGsyncSetControlStereoLockMode(pGsyncApi, pParams) gsyncapiCtrlCmdGsyncSetControlStereoLockMode_DISPATCH(pGsyncApi, pParams)
#define gsyncapiCtrlCmdGsyncReadRegister(pGsyncApi, pParams) gsyncapiCtrlCmdGsyncReadRegister_DISPATCH(pGsyncApi, pParams)
#define gsyncapiCtrlCmdGsyncWriteRegister(pGsyncApi, pParams) gsyncapiCtrlCmdGsyncWriteRegister_DISPATCH(pGsyncApi, pParams)
#define gsyncapiCtrlCmdGsyncSetLocalSync(pGsyncApi, pParams) gsyncapiCtrlCmdGsyncSetLocalSync_DISPATCH(pGsyncApi, pParams)
#define gsyncapiCtrlCmdGsyncConfigFlash(pGsyncApi, pParams) gsyncapiCtrlCmdGsyncConfigFlash_DISPATCH(pGsyncApi, pParams)
#define gsyncapiCtrlCmdGsyncGetHouseSyncMode(pGsyncApi, pParams) gsyncapiCtrlCmdGsyncGetHouseSyncMode_DISPATCH(pGsyncApi, pParams)
#define gsyncapiCtrlCmdGsyncSetHouseSyncMode(pGsyncApi, pParams) gsyncapiCtrlCmdGsyncSetHouseSyncMode_DISPATCH(pGsyncApi, pParams)
#define gsyncapiShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) gsyncapiShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define gsyncapiCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) gsyncapiCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define gsyncapiMapTo(pResource, pParams) gsyncapiMapTo_DISPATCH(pResource, pParams)
#define gsyncapiAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) gsyncapiAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define gsyncapiSetNotificationShare(pNotifier, pNotifShare) gsyncapiSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define gsyncapiGetRefCount(pResource) gsyncapiGetRefCount_DISPATCH(pResource)
#define gsyncapiAddAdditionalDependants(pClient, pResource, pReference) gsyncapiAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define gsyncapiControl_Prologue(pResource, pCallContext, pParams) gsyncapiControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define gsyncapiUnmapFrom(pResource, pParams) gsyncapiUnmapFrom_DISPATCH(pResource, pParams)
#define gsyncapiControl_Epilogue(pResource, pCallContext, pParams) gsyncapiControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define gsyncapiControlLookup(pResource, pParams, ppEntry) gsyncapiControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define gsyncapiUnmap(pResource, pCallContext, pCpuMapping) gsyncapiUnmap_DISPATCH(pResource, pCallContext, pCpuMapping)
#define gsyncapiGetMemInterMapParams(pRmResource, pParams) gsyncapiGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define gsyncapiGetMemoryMappingDescriptor(pRmResource, ppMemDesc) gsyncapiGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define gsyncapiControlFilter(pResource, pCallContext, pParams) gsyncapiControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define gsyncapiUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) gsyncapiUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define gsyncapiControlSerialization_Prologue(pResource, pCallContext, pParams) gsyncapiControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define gsyncapiCanCopy(pResource) gsyncapiCanCopy_DISPATCH(pResource)
#define gsyncapiPreDestruct(pResource) gsyncapiPreDestruct_DISPATCH(pResource)
#define gsyncapiIsDuplicate(pResource, hMemory, pDuplicate) gsyncapiIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define gsyncapiControlSerialization_Epilogue(pResource, pCallContext, pParams) gsyncapiControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define gsyncapiGetNotificationListPtr(pNotifier) gsyncapiGetNotificationListPtr_DISPATCH(pNotifier)
#define gsyncapiGetNotificationShare(pNotifier) gsyncapiGetNotificationShare_DISPATCH(pNotifier)
#define gsyncapiMap(pResource, pCallContext, pParams, pCpuMapping) gsyncapiMap_DISPATCH(pResource, pCallContext, pParams, pCpuMapping)
#define gsyncapiGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) gsyncapiGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)
NV_STATUS gsyncapiControl_IMPL(struct GSyncApi *pGsyncApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);

static inline NV_STATUS gsyncapiControl_DISPATCH(struct GSyncApi *pGsyncApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGsyncApi->__gsyncapiControl__(pGsyncApi, pCallContext, pParams);
}

NV_STATUS gsyncapiCtrlCmdGsyncGetVersion_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_VERSION_PARAMS *pGsyncGetVersionParams);

static inline NV_STATUS gsyncapiCtrlCmdGsyncGetVersion_DISPATCH(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_VERSION_PARAMS *pGsyncGetVersionParams) {
    return pGsyncApi->__gsyncapiCtrlCmdGsyncGetVersion__(pGsyncApi, pGsyncGetVersionParams);
}

NV_STATUS gsyncapiCtrlCmdGetGsyncGpuTopology_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GET_GSYNC_GPU_TOPOLOGY_PARAMS *pParams);

static inline NV_STATUS gsyncapiCtrlCmdGetGsyncGpuTopology_DISPATCH(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GET_GSYNC_GPU_TOPOLOGY_PARAMS *pParams) {
    return pGsyncApi->__gsyncapiCtrlCmdGetGsyncGpuTopology__(pGsyncApi, pParams);
}

NV_STATUS gsyncapiCtrlCmdGsyncGetStatusSignals_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_STATUS_SIGNALS_PARAMS *pParams);

static inline NV_STATUS gsyncapiCtrlCmdGsyncGetStatusSignals_DISPATCH(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_STATUS_SIGNALS_PARAMS *pParams) {
    return pGsyncApi->__gsyncapiCtrlCmdGsyncGetStatusSignals__(pGsyncApi, pParams);
}

NV_STATUS gsyncapiCtrlCmdGsyncGetControlParams_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_CONTROL_PARAMS_PARAMS *pParams);

static inline NV_STATUS gsyncapiCtrlCmdGsyncGetControlParams_DISPATCH(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_CONTROL_PARAMS_PARAMS *pParams) {
    return pGsyncApi->__gsyncapiCtrlCmdGsyncGetControlParams__(pGsyncApi, pParams);
}

NV_STATUS gsyncapiCtrlCmdGsyncSetControlParams_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_SET_CONTROL_PARAMS_PARAMS *pParams);

static inline NV_STATUS gsyncapiCtrlCmdGsyncSetControlParams_DISPATCH(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_SET_CONTROL_PARAMS_PARAMS *pParams) {
    return pGsyncApi->__gsyncapiCtrlCmdGsyncSetControlParams__(pGsyncApi, pParams);
}

NV_STATUS gsyncapiCtrlCmdGsyncGetControlSync_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_CONTROL_SYNC_PARAMS *pParams);

static inline NV_STATUS gsyncapiCtrlCmdGsyncGetControlSync_DISPATCH(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_CONTROL_SYNC_PARAMS *pParams) {
    return pGsyncApi->__gsyncapiCtrlCmdGsyncGetControlSync__(pGsyncApi, pParams);
}

NV_STATUS gsyncapiCtrlCmdGsyncSetControlSync_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_SET_CONTROL_SYNC_PARAMS *pParams);

static inline NV_STATUS gsyncapiCtrlCmdGsyncSetControlSync_DISPATCH(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_SET_CONTROL_SYNC_PARAMS *pParams) {
    return pGsyncApi->__gsyncapiCtrlCmdGsyncSetControlSync__(pGsyncApi, pParams);
}

NV_STATUS gsyncapiCtrlCmdGsyncSetControlUnsync_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_SET_CONTROL_UNSYNC_PARAMS *pParams);

static inline NV_STATUS gsyncapiCtrlCmdGsyncSetControlUnsync_DISPATCH(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_SET_CONTROL_UNSYNC_PARAMS *pParams) {
    return pGsyncApi->__gsyncapiCtrlCmdGsyncSetControlUnsync__(pGsyncApi, pParams);
}

NV_STATUS gsyncapiCtrlCmdGsyncGetStatusSync_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_STATUS_SYNC_PARAMS *pParams);

static inline NV_STATUS gsyncapiCtrlCmdGsyncGetStatusSync_DISPATCH(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_STATUS_SYNC_PARAMS *pParams) {
    return pGsyncApi->__gsyncapiCtrlCmdGsyncGetStatusSync__(pGsyncApi, pParams);
}

NV_STATUS gsyncapiCtrlCmdGsyncGetStatus_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_STATUS_PARAMS *pParams);

static inline NV_STATUS gsyncapiCtrlCmdGsyncGetStatus_DISPATCH(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_STATUS_PARAMS *pParams) {
    return pGsyncApi->__gsyncapiCtrlCmdGsyncGetStatus__(pGsyncApi, pParams);
}

NV_STATUS gsyncapiCtrlCmdGsyncGetControlTesting_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_CONTROL_TESTING_PARAMS *pParams);

static inline NV_STATUS gsyncapiCtrlCmdGsyncGetControlTesting_DISPATCH(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_CONTROL_TESTING_PARAMS *pParams) {
    return pGsyncApi->__gsyncapiCtrlCmdGsyncGetControlTesting__(pGsyncApi, pParams);
}

NV_STATUS gsyncapiCtrlCmdGsyncSetControlTesting_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_SET_CONTROL_TESTING_PARAMS *pParams);

static inline NV_STATUS gsyncapiCtrlCmdGsyncSetControlTesting_DISPATCH(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_SET_CONTROL_TESTING_PARAMS *pParams) {
    return pGsyncApi->__gsyncapiCtrlCmdGsyncSetControlTesting__(pGsyncApi, pParams);
}

NV_STATUS gsyncapiCtrlCmdGsyncSetControlWatchdog_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_SET_CONTROL_WATCHDOG_PARAMS *pParams);

static inline NV_STATUS gsyncapiCtrlCmdGsyncSetControlWatchdog_DISPATCH(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_SET_CONTROL_WATCHDOG_PARAMS *pParams) {
    return pGsyncApi->__gsyncapiCtrlCmdGsyncSetControlWatchdog__(pGsyncApi, pParams);
}

NV_STATUS gsyncapiCtrlCmdGsyncGetControlInterlaceMode_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_CONTROL_INTERLACE_MODE_PARAMS *pParams);

static inline NV_STATUS gsyncapiCtrlCmdGsyncGetControlInterlaceMode_DISPATCH(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_CONTROL_INTERLACE_MODE_PARAMS *pParams) {
    return pGsyncApi->__gsyncapiCtrlCmdGsyncGetControlInterlaceMode__(pGsyncApi, pParams);
}

NV_STATUS gsyncapiCtrlCmdGsyncSetControlInterlaceMode_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_SET_CONTROL_INTERLACE_MODE_PARAMS *pParams);

static inline NV_STATUS gsyncapiCtrlCmdGsyncSetControlInterlaceMode_DISPATCH(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_SET_CONTROL_INTERLACE_MODE_PARAMS *pParams) {
    return pGsyncApi->__gsyncapiCtrlCmdGsyncSetControlInterlaceMode__(pGsyncApi, pParams);
}

NV_STATUS gsyncapiCtrlCmdGsyncGetControlSwapBarrier_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_CONTROL_SWAP_BARRIER_PARAMS *pParams);

static inline NV_STATUS gsyncapiCtrlCmdGsyncGetControlSwapBarrier_DISPATCH(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_CONTROL_SWAP_BARRIER_PARAMS *pParams) {
    return pGsyncApi->__gsyncapiCtrlCmdGsyncGetControlSwapBarrier__(pGsyncApi, pParams);
}

NV_STATUS gsyncapiCtrlCmdGsyncSetControlSwapBarrier_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_SET_CONTROL_SWAP_BARRIER_PARAMS *pParams);

static inline NV_STATUS gsyncapiCtrlCmdGsyncSetControlSwapBarrier_DISPATCH(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_SET_CONTROL_SWAP_BARRIER_PARAMS *pParams) {
    return pGsyncApi->__gsyncapiCtrlCmdGsyncSetControlSwapBarrier__(pGsyncApi, pParams);
}

NV_STATUS gsyncapiCtrlCmdGsyncGetControlSwapLockWindow_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_CONTROL_SWAP_LOCK_WINDOW_PARAMS *pParams);

static inline NV_STATUS gsyncapiCtrlCmdGsyncGetControlSwapLockWindow_DISPATCH(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_CONTROL_SWAP_LOCK_WINDOW_PARAMS *pParams) {
    return pGsyncApi->__gsyncapiCtrlCmdGsyncGetControlSwapLockWindow__(pGsyncApi, pParams);
}

NV_STATUS gsyncapiCtrlCmdGsyncGetCaps_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_CAPS_PARAMS *pParams);

static inline NV_STATUS gsyncapiCtrlCmdGsyncGetCaps_DISPATCH(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_CAPS_PARAMS *pParams) {
    return pGsyncApi->__gsyncapiCtrlCmdGsyncGetCaps__(pGsyncApi, pParams);
}

NV_STATUS gsyncapiCtrlCmdGsyncGetOptimizedTiming_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_PARAMS *pParams);

static inline NV_STATUS gsyncapiCtrlCmdGsyncGetOptimizedTiming_DISPATCH(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_PARAMS *pParams) {
    return pGsyncApi->__gsyncapiCtrlCmdGsyncGetOptimizedTiming__(pGsyncApi, pParams);
}

NV_STATUS gsyncapiCtrlCmdGsyncSetEventNotification_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_SET_EVENT_NOTIFICATION_PARAMS *pSetEventParams);

static inline NV_STATUS gsyncapiCtrlCmdGsyncSetEventNotification_DISPATCH(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_SET_EVENT_NOTIFICATION_PARAMS *pSetEventParams) {
    return pGsyncApi->__gsyncapiCtrlCmdGsyncSetEventNotification__(pGsyncApi, pSetEventParams);
}

NV_STATUS gsyncapiCtrlCmdGsyncGetControlStereoLockMode_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_CMD_GSYNC_GET_CONTROL_STEREO_LOCK_MODE_PARAMS *pParams);

static inline NV_STATUS gsyncapiCtrlCmdGsyncGetControlStereoLockMode_DISPATCH(struct GSyncApi *pGsyncApi, NV30F1_CTRL_CMD_GSYNC_GET_CONTROL_STEREO_LOCK_MODE_PARAMS *pParams) {
    return pGsyncApi->__gsyncapiCtrlCmdGsyncGetControlStereoLockMode__(pGsyncApi, pParams);
}

NV_STATUS gsyncapiCtrlCmdGsyncSetControlStereoLockMode_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_CMD_GSYNC_SET_CONTROL_STEREO_LOCK_MODE_PARAMS *pParams);

static inline NV_STATUS gsyncapiCtrlCmdGsyncSetControlStereoLockMode_DISPATCH(struct GSyncApi *pGsyncApi, NV30F1_CTRL_CMD_GSYNC_SET_CONTROL_STEREO_LOCK_MODE_PARAMS *pParams) {
    return pGsyncApi->__gsyncapiCtrlCmdGsyncSetControlStereoLockMode__(pGsyncApi, pParams);
}

NV_STATUS gsyncapiCtrlCmdGsyncReadRegister_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_READ_REGISTER_PARAMS *pParams);

static inline NV_STATUS gsyncapiCtrlCmdGsyncReadRegister_DISPATCH(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_READ_REGISTER_PARAMS *pParams) {
    return pGsyncApi->__gsyncapiCtrlCmdGsyncReadRegister__(pGsyncApi, pParams);
}

NV_STATUS gsyncapiCtrlCmdGsyncWriteRegister_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_WRITE_REGISTER_PARAMS *pParams);

static inline NV_STATUS gsyncapiCtrlCmdGsyncWriteRegister_DISPATCH(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_WRITE_REGISTER_PARAMS *pParams) {
    return pGsyncApi->__gsyncapiCtrlCmdGsyncWriteRegister__(pGsyncApi, pParams);
}

NV_STATUS gsyncapiCtrlCmdGsyncSetLocalSync_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_SET_LOCAL_SYNC_PARAMS *pParams);

static inline NV_STATUS gsyncapiCtrlCmdGsyncSetLocalSync_DISPATCH(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_SET_LOCAL_SYNC_PARAMS *pParams) {
    return pGsyncApi->__gsyncapiCtrlCmdGsyncSetLocalSync__(pGsyncApi, pParams);
}

NV_STATUS gsyncapiCtrlCmdGsyncConfigFlash_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_CMD_GSYNC_CONFIG_FLASH_PARAMS *pParams);

static inline NV_STATUS gsyncapiCtrlCmdGsyncConfigFlash_DISPATCH(struct GSyncApi *pGsyncApi, NV30F1_CTRL_CMD_GSYNC_CONFIG_FLASH_PARAMS *pParams) {
    return pGsyncApi->__gsyncapiCtrlCmdGsyncConfigFlash__(pGsyncApi, pParams);
}

NV_STATUS gsyncapiCtrlCmdGsyncGetHouseSyncMode_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_HOUSE_SYNC_MODE_PARAMS *pParams);

static inline NV_STATUS gsyncapiCtrlCmdGsyncGetHouseSyncMode_DISPATCH(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_HOUSE_SYNC_MODE_PARAMS *pParams) {
    return pGsyncApi->__gsyncapiCtrlCmdGsyncGetHouseSyncMode__(pGsyncApi, pParams);
}

NV_STATUS gsyncapiCtrlCmdGsyncSetHouseSyncMode_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_HOUSE_SYNC_MODE_PARAMS *pParams);

static inline NV_STATUS gsyncapiCtrlCmdGsyncSetHouseSyncMode_DISPATCH(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_HOUSE_SYNC_MODE_PARAMS *pParams) {
    return pGsyncApi->__gsyncapiCtrlCmdGsyncSetHouseSyncMode__(pGsyncApi, pParams);
}

static inline NvBool gsyncapiShareCallback_DISPATCH(struct GSyncApi *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__gsyncapiShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS gsyncapiCheckMemInterUnmap_DISPATCH(struct GSyncApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__gsyncapiCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS gsyncapiMapTo_DISPATCH(struct GSyncApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__gsyncapiMapTo__(pResource, pParams);
}

static inline NvBool gsyncapiAccessCallback_DISPATCH(struct GSyncApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__gsyncapiAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline void gsyncapiSetNotificationShare_DISPATCH(struct GSyncApi *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__gsyncapiSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NvU32 gsyncapiGetRefCount_DISPATCH(struct GSyncApi *pResource) {
    return pResource->__gsyncapiGetRefCount__(pResource);
}

static inline void gsyncapiAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct GSyncApi *pResource, RsResourceRef *pReference) {
    pResource->__gsyncapiAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS gsyncapiControl_Prologue_DISPATCH(struct GSyncApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__gsyncapiControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS gsyncapiUnmapFrom_DISPATCH(struct GSyncApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__gsyncapiUnmapFrom__(pResource, pParams);
}

static inline void gsyncapiControl_Epilogue_DISPATCH(struct GSyncApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__gsyncapiControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS gsyncapiControlLookup_DISPATCH(struct GSyncApi *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__gsyncapiControlLookup__(pResource, pParams, ppEntry);
}

static inline NV_STATUS gsyncapiUnmap_DISPATCH(struct GSyncApi *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pResource->__gsyncapiUnmap__(pResource, pCallContext, pCpuMapping);
}

static inline NV_STATUS gsyncapiGetMemInterMapParams_DISPATCH(struct GSyncApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__gsyncapiGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS gsyncapiGetMemoryMappingDescriptor_DISPATCH(struct GSyncApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__gsyncapiGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS gsyncapiControlFilter_DISPATCH(struct GSyncApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__gsyncapiControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS gsyncapiUnregisterEvent_DISPATCH(struct GSyncApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__gsyncapiUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS gsyncapiControlSerialization_Prologue_DISPATCH(struct GSyncApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__gsyncapiControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool gsyncapiCanCopy_DISPATCH(struct GSyncApi *pResource) {
    return pResource->__gsyncapiCanCopy__(pResource);
}

static inline void gsyncapiPreDestruct_DISPATCH(struct GSyncApi *pResource) {
    pResource->__gsyncapiPreDestruct__(pResource);
}

static inline NV_STATUS gsyncapiIsDuplicate_DISPATCH(struct GSyncApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__gsyncapiIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void gsyncapiControlSerialization_Epilogue_DISPATCH(struct GSyncApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__gsyncapiControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline PEVENTNOTIFICATION *gsyncapiGetNotificationListPtr_DISPATCH(struct GSyncApi *pNotifier) {
    return pNotifier->__gsyncapiGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare *gsyncapiGetNotificationShare_DISPATCH(struct GSyncApi *pNotifier) {
    return pNotifier->__gsyncapiGetNotificationShare__(pNotifier);
}

static inline NV_STATUS gsyncapiMap_DISPATCH(struct GSyncApi *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pResource->__gsyncapiMap__(pResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS gsyncapiGetOrAllocNotifShare_DISPATCH(struct GSyncApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__gsyncapiGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

NV_STATUS gsyncapiConstruct_IMPL(struct GSyncApi *arg_pGsyncApi, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_gsyncapiConstruct(arg_pGsyncApi, arg_pCallContext, arg_pParams) gsyncapiConstruct_IMPL(arg_pGsyncApi, arg_pCallContext, arg_pParams)
#undef PRIVATE_FIELD


void    CliNotifyGsyncEvent     (NvU32, NvU32);

#endif // GSYNCAPI_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_GSYNC_API_NVOC_H_
