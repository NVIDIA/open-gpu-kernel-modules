
#ifndef _G_GSYNC_API_NVOC_H_
#define _G_GSYNC_API_NVOC_H_

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

#pragma once
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


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_GSYNC_API_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__GSyncApi;
struct NVOC_METADATA__RmResource;
struct NVOC_METADATA__Notifier;
struct NVOC_VTABLE__GSyncApi;


struct GSyncApi {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__GSyncApi *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct RmResource __nvoc_base_RmResource;
    struct Notifier __nvoc_base_Notifier;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^3
    struct RsResource *__nvoc_pbase_RsResource;    // res super^2
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^2
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super
    struct INotifier *__nvoc_pbase_INotifier;    // inotify super^2
    struct Notifier *__nvoc_pbase_Notifier;    // notify super
    struct GSyncApi *__nvoc_pbase_GSyncApi;    // gsyncapi

    // Data members
    NvU32 instance;
    NvU32 classNum;
    NvU32 notifyAction;
    NvU32 lastEventNotified;
    PEVENTNOTIFICATION pEventByType[9];
    NvBool oldEventNotification;
};


// Vtable with 26 per-class function pointers
struct NVOC_VTABLE__GSyncApi {
    NV_STATUS (*__gsyncapiControl__)(struct GSyncApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual override (res) base (rmres)
    NvBool (*__gsyncapiAccessCallback__)(struct GSyncApi * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (rmres)
    NvBool (*__gsyncapiShareCallback__)(struct GSyncApi * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__gsyncapiGetMemInterMapParams__)(struct GSyncApi * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__gsyncapiCheckMemInterUnmap__)(struct GSyncApi * /*this*/, NvBool);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__gsyncapiGetMemoryMappingDescriptor__)(struct GSyncApi * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__gsyncapiControlSerialization_Prologue__)(struct GSyncApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (rmres)
    void (*__gsyncapiControlSerialization_Epilogue__)(struct GSyncApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__gsyncapiControl_Prologue__)(struct GSyncApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (rmres)
    void (*__gsyncapiControl_Epilogue__)(struct GSyncApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (rmres)
    NvBool (*__gsyncapiCanCopy__)(struct GSyncApi * /*this*/);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__gsyncapiIsDuplicate__)(struct GSyncApi * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (rmres)
    void (*__gsyncapiPreDestruct__)(struct GSyncApi * /*this*/);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__gsyncapiControlFilter__)(struct GSyncApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__gsyncapiMap__)(struct GSyncApi * /*this*/, struct CALL_CONTEXT *, RS_CPU_MAP_PARAMS *, RsCpuMapping *);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__gsyncapiUnmap__)(struct GSyncApi * /*this*/, struct CALL_CONTEXT *, RsCpuMapping *);  // virtual inherited (res) base (rmres)
    NvBool (*__gsyncapiIsPartialUnmapSupported__)(struct GSyncApi * /*this*/);  // inline virtual inherited (res) base (rmres) body
    NV_STATUS (*__gsyncapiMapTo__)(struct GSyncApi * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__gsyncapiUnmapFrom__)(struct GSyncApi * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (rmres)
    NvU32 (*__gsyncapiGetRefCount__)(struct GSyncApi * /*this*/);  // virtual inherited (res) base (rmres)
    void (*__gsyncapiAddAdditionalDependants__)(struct RsClient *, struct GSyncApi * /*this*/, RsResourceRef *);  // virtual inherited (res) base (rmres)
    PEVENTNOTIFICATION * (*__gsyncapiGetNotificationListPtr__)(struct GSyncApi * /*this*/);  // virtual inherited (notify) base (notify)
    struct NotifShare * (*__gsyncapiGetNotificationShare__)(struct GSyncApi * /*this*/);  // virtual inherited (notify) base (notify)
    void (*__gsyncapiSetNotificationShare__)(struct GSyncApi * /*this*/, struct NotifShare *);  // virtual inherited (notify) base (notify)
    NV_STATUS (*__gsyncapiUnregisterEvent__)(struct GSyncApi * /*this*/, NvHandle, NvHandle, NvHandle, NvHandle);  // virtual inherited (notify) base (notify)
    NV_STATUS (*__gsyncapiGetOrAllocNotifShare__)(struct GSyncApi * /*this*/, NvHandle, NvHandle, struct NotifShare **);  // virtual inherited (notify) base (notify)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__GSyncApi {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__RmResource metadata__RmResource;
    const struct NVOC_METADATA__Notifier metadata__Notifier;
    const struct NVOC_VTABLE__GSyncApi vtable;
};

#ifndef __NVOC_CLASS_GSyncApi_TYPEDEF__
#define __NVOC_CLASS_GSyncApi_TYPEDEF__
typedef struct GSyncApi GSyncApi;
#endif /* __NVOC_CLASS_GSyncApi_TYPEDEF__ */

#ifndef __nvoc_class_id_GSyncApi
#define __nvoc_class_id_GSyncApi 0x214628
#endif /* __nvoc_class_id_GSyncApi */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GSyncApi;

#define __staticCast_GSyncApi(pThis) \
    ((pThis)->__nvoc_pbase_GSyncApi)

#ifdef __nvoc_gsync_api_h_disabled
#define __dynamicCast_GSyncApi(pThis) ((GSyncApi*) NULL)
#else //__nvoc_gsync_api_h_disabled
#define __dynamicCast_GSyncApi(pThis) \
    ((GSyncApi*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(GSyncApi)))
#endif //__nvoc_gsync_api_h_disabled

NV_STATUS __nvoc_objCreateDynamic_GSyncApi(GSyncApi**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_GSyncApi(GSyncApi**, Dynamic*, NvU32, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __objCreate_GSyncApi(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_GSyncApi((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros for implementation functions
NV_STATUS gsyncapiConstruct_IMPL(struct GSyncApi *arg_pGsyncApi, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __nvoc_gsyncapiConstruct(arg_pGsyncApi, arg_pCallContext, arg_pParams) gsyncapiConstruct_IMPL(arg_pGsyncApi, arg_pCallContext, arg_pParams)

NV_STATUS gsyncapiCtrlCmdGsyncGetVersion_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_VERSION_PARAMS *pGsyncGetVersionParams);
#ifdef __nvoc_gsync_api_h_disabled
static inline NV_STATUS gsyncapiCtrlCmdGsyncGetVersion(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_VERSION_PARAMS *pGsyncGetVersionParams) {
    NV_ASSERT_FAILED_PRECOMP("GSyncApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_gsync_api_h_disabled
#define gsyncapiCtrlCmdGsyncGetVersion(pGsyncApi, pGsyncGetVersionParams) gsyncapiCtrlCmdGsyncGetVersion_IMPL(pGsyncApi, pGsyncGetVersionParams)
#endif // __nvoc_gsync_api_h_disabled

NV_STATUS gsyncapiCtrlCmdGetGsyncGpuTopology_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GET_GSYNC_GPU_TOPOLOGY_PARAMS *pParams);
#ifdef __nvoc_gsync_api_h_disabled
static inline NV_STATUS gsyncapiCtrlCmdGetGsyncGpuTopology(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GET_GSYNC_GPU_TOPOLOGY_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("GSyncApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_gsync_api_h_disabled
#define gsyncapiCtrlCmdGetGsyncGpuTopology(pGsyncApi, pParams) gsyncapiCtrlCmdGetGsyncGpuTopology_IMPL(pGsyncApi, pParams)
#endif // __nvoc_gsync_api_h_disabled

NV_STATUS gsyncapiCtrlCmdGsyncGetStatusSignals_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_STATUS_SIGNALS_PARAMS *pParams);
#ifdef __nvoc_gsync_api_h_disabled
static inline NV_STATUS gsyncapiCtrlCmdGsyncGetStatusSignals(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_STATUS_SIGNALS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("GSyncApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_gsync_api_h_disabled
#define gsyncapiCtrlCmdGsyncGetStatusSignals(pGsyncApi, pParams) gsyncapiCtrlCmdGsyncGetStatusSignals_IMPL(pGsyncApi, pParams)
#endif // __nvoc_gsync_api_h_disabled

NV_STATUS gsyncapiCtrlCmdGsyncGetControlParams_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_CONTROL_PARAMS_PARAMS *pParams);
#ifdef __nvoc_gsync_api_h_disabled
static inline NV_STATUS gsyncapiCtrlCmdGsyncGetControlParams(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_CONTROL_PARAMS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("GSyncApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_gsync_api_h_disabled
#define gsyncapiCtrlCmdGsyncGetControlParams(pGsyncApi, pParams) gsyncapiCtrlCmdGsyncGetControlParams_IMPL(pGsyncApi, pParams)
#endif // __nvoc_gsync_api_h_disabled

NV_STATUS gsyncapiCtrlCmdGsyncSetControlParams_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_SET_CONTROL_PARAMS_PARAMS *pParams);
#ifdef __nvoc_gsync_api_h_disabled
static inline NV_STATUS gsyncapiCtrlCmdGsyncSetControlParams(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_SET_CONTROL_PARAMS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("GSyncApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_gsync_api_h_disabled
#define gsyncapiCtrlCmdGsyncSetControlParams(pGsyncApi, pParams) gsyncapiCtrlCmdGsyncSetControlParams_IMPL(pGsyncApi, pParams)
#endif // __nvoc_gsync_api_h_disabled

NV_STATUS gsyncapiCtrlCmdGsyncGetControlSync_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_CONTROL_SYNC_PARAMS *pParams);
#ifdef __nvoc_gsync_api_h_disabled
static inline NV_STATUS gsyncapiCtrlCmdGsyncGetControlSync(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_CONTROL_SYNC_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("GSyncApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_gsync_api_h_disabled
#define gsyncapiCtrlCmdGsyncGetControlSync(pGsyncApi, pParams) gsyncapiCtrlCmdGsyncGetControlSync_IMPL(pGsyncApi, pParams)
#endif // __nvoc_gsync_api_h_disabled

NV_STATUS gsyncapiCtrlCmdGsyncSetControlSync_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_SET_CONTROL_SYNC_PARAMS *pParams);
#ifdef __nvoc_gsync_api_h_disabled
static inline NV_STATUS gsyncapiCtrlCmdGsyncSetControlSync(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_SET_CONTROL_SYNC_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("GSyncApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_gsync_api_h_disabled
#define gsyncapiCtrlCmdGsyncSetControlSync(pGsyncApi, pParams) gsyncapiCtrlCmdGsyncSetControlSync_IMPL(pGsyncApi, pParams)
#endif // __nvoc_gsync_api_h_disabled

NV_STATUS gsyncapiCtrlCmdGsyncSetControlUnsync_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_SET_CONTROL_UNSYNC_PARAMS *pParams);
#ifdef __nvoc_gsync_api_h_disabled
static inline NV_STATUS gsyncapiCtrlCmdGsyncSetControlUnsync(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_SET_CONTROL_UNSYNC_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("GSyncApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_gsync_api_h_disabled
#define gsyncapiCtrlCmdGsyncSetControlUnsync(pGsyncApi, pParams) gsyncapiCtrlCmdGsyncSetControlUnsync_IMPL(pGsyncApi, pParams)
#endif // __nvoc_gsync_api_h_disabled

NV_STATUS gsyncapiCtrlCmdGsyncGetStatusSync_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_STATUS_SYNC_PARAMS *pParams);
#ifdef __nvoc_gsync_api_h_disabled
static inline NV_STATUS gsyncapiCtrlCmdGsyncGetStatusSync(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_STATUS_SYNC_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("GSyncApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_gsync_api_h_disabled
#define gsyncapiCtrlCmdGsyncGetStatusSync(pGsyncApi, pParams) gsyncapiCtrlCmdGsyncGetStatusSync_IMPL(pGsyncApi, pParams)
#endif // __nvoc_gsync_api_h_disabled

NV_STATUS gsyncapiCtrlCmdGsyncGetStatus_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_STATUS_PARAMS *pParams);
#ifdef __nvoc_gsync_api_h_disabled
static inline NV_STATUS gsyncapiCtrlCmdGsyncGetStatus(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_STATUS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("GSyncApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_gsync_api_h_disabled
#define gsyncapiCtrlCmdGsyncGetStatus(pGsyncApi, pParams) gsyncapiCtrlCmdGsyncGetStatus_IMPL(pGsyncApi, pParams)
#endif // __nvoc_gsync_api_h_disabled

NV_STATUS gsyncapiCtrlCmdGsyncGetControlTesting_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_CONTROL_TESTING_PARAMS *pParams);
#ifdef __nvoc_gsync_api_h_disabled
static inline NV_STATUS gsyncapiCtrlCmdGsyncGetControlTesting(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_CONTROL_TESTING_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("GSyncApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_gsync_api_h_disabled
#define gsyncapiCtrlCmdGsyncGetControlTesting(pGsyncApi, pParams) gsyncapiCtrlCmdGsyncGetControlTesting_IMPL(pGsyncApi, pParams)
#endif // __nvoc_gsync_api_h_disabled

NV_STATUS gsyncapiCtrlCmdGsyncSetControlTesting_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_SET_CONTROL_TESTING_PARAMS *pParams);
#ifdef __nvoc_gsync_api_h_disabled
static inline NV_STATUS gsyncapiCtrlCmdGsyncSetControlTesting(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_SET_CONTROL_TESTING_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("GSyncApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_gsync_api_h_disabled
#define gsyncapiCtrlCmdGsyncSetControlTesting(pGsyncApi, pParams) gsyncapiCtrlCmdGsyncSetControlTesting_IMPL(pGsyncApi, pParams)
#endif // __nvoc_gsync_api_h_disabled

NV_STATUS gsyncapiCtrlCmdGsyncSetControlWatchdog_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_SET_CONTROL_WATCHDOG_PARAMS *pParams);
#ifdef __nvoc_gsync_api_h_disabled
static inline NV_STATUS gsyncapiCtrlCmdGsyncSetControlWatchdog(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_SET_CONTROL_WATCHDOG_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("GSyncApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_gsync_api_h_disabled
#define gsyncapiCtrlCmdGsyncSetControlWatchdog(pGsyncApi, pParams) gsyncapiCtrlCmdGsyncSetControlWatchdog_IMPL(pGsyncApi, pParams)
#endif // __nvoc_gsync_api_h_disabled

NV_STATUS gsyncapiCtrlCmdGsyncGetControlInterlaceMode_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_CONTROL_INTERLACE_MODE_PARAMS *pParams);
#ifdef __nvoc_gsync_api_h_disabled
static inline NV_STATUS gsyncapiCtrlCmdGsyncGetControlInterlaceMode(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_CONTROL_INTERLACE_MODE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("GSyncApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_gsync_api_h_disabled
#define gsyncapiCtrlCmdGsyncGetControlInterlaceMode(pGsyncApi, pParams) gsyncapiCtrlCmdGsyncGetControlInterlaceMode_IMPL(pGsyncApi, pParams)
#endif // __nvoc_gsync_api_h_disabled

NV_STATUS gsyncapiCtrlCmdGsyncSetControlInterlaceMode_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_SET_CONTROL_INTERLACE_MODE_PARAMS *pParams);
#ifdef __nvoc_gsync_api_h_disabled
static inline NV_STATUS gsyncapiCtrlCmdGsyncSetControlInterlaceMode(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_SET_CONTROL_INTERLACE_MODE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("GSyncApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_gsync_api_h_disabled
#define gsyncapiCtrlCmdGsyncSetControlInterlaceMode(pGsyncApi, pParams) gsyncapiCtrlCmdGsyncSetControlInterlaceMode_IMPL(pGsyncApi, pParams)
#endif // __nvoc_gsync_api_h_disabled

NV_STATUS gsyncapiCtrlCmdGsyncGetControlSwapBarrier_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_CONTROL_SWAP_BARRIER_PARAMS *pParams);
#ifdef __nvoc_gsync_api_h_disabled
static inline NV_STATUS gsyncapiCtrlCmdGsyncGetControlSwapBarrier(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_CONTROL_SWAP_BARRIER_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("GSyncApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_gsync_api_h_disabled
#define gsyncapiCtrlCmdGsyncGetControlSwapBarrier(pGsyncApi, pParams) gsyncapiCtrlCmdGsyncGetControlSwapBarrier_IMPL(pGsyncApi, pParams)
#endif // __nvoc_gsync_api_h_disabled

NV_STATUS gsyncapiCtrlCmdGsyncSetControlSwapBarrier_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_SET_CONTROL_SWAP_BARRIER_PARAMS *pParams);
#ifdef __nvoc_gsync_api_h_disabled
static inline NV_STATUS gsyncapiCtrlCmdGsyncSetControlSwapBarrier(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_SET_CONTROL_SWAP_BARRIER_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("GSyncApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_gsync_api_h_disabled
#define gsyncapiCtrlCmdGsyncSetControlSwapBarrier(pGsyncApi, pParams) gsyncapiCtrlCmdGsyncSetControlSwapBarrier_IMPL(pGsyncApi, pParams)
#endif // __nvoc_gsync_api_h_disabled

NV_STATUS gsyncapiCtrlCmdGsyncGetControlSwapLockWindow_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_CONTROL_SWAP_LOCK_WINDOW_PARAMS *pParams);
#ifdef __nvoc_gsync_api_h_disabled
static inline NV_STATUS gsyncapiCtrlCmdGsyncGetControlSwapLockWindow(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_CONTROL_SWAP_LOCK_WINDOW_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("GSyncApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_gsync_api_h_disabled
#define gsyncapiCtrlCmdGsyncGetControlSwapLockWindow(pGsyncApi, pParams) gsyncapiCtrlCmdGsyncGetControlSwapLockWindow_IMPL(pGsyncApi, pParams)
#endif // __nvoc_gsync_api_h_disabled

NV_STATUS gsyncapiCtrlCmdGsyncGetCaps_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_CAPS_PARAMS *pParams);
#ifdef __nvoc_gsync_api_h_disabled
static inline NV_STATUS gsyncapiCtrlCmdGsyncGetCaps(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_CAPS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("GSyncApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_gsync_api_h_disabled
#define gsyncapiCtrlCmdGsyncGetCaps(pGsyncApi, pParams) gsyncapiCtrlCmdGsyncGetCaps_IMPL(pGsyncApi, pParams)
#endif // __nvoc_gsync_api_h_disabled

NV_STATUS gsyncapiCtrlCmdGsyncGetOptimizedTiming_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_PARAMS *pParams);
#ifdef __nvoc_gsync_api_h_disabled
static inline NV_STATUS gsyncapiCtrlCmdGsyncGetOptimizedTiming(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("GSyncApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_gsync_api_h_disabled
#define gsyncapiCtrlCmdGsyncGetOptimizedTiming(pGsyncApi, pParams) gsyncapiCtrlCmdGsyncGetOptimizedTiming_IMPL(pGsyncApi, pParams)
#endif // __nvoc_gsync_api_h_disabled

NV_STATUS gsyncapiCtrlCmdGsyncSetEventNotification_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_SET_EVENT_NOTIFICATION_PARAMS *pSetEventParams);
#ifdef __nvoc_gsync_api_h_disabled
static inline NV_STATUS gsyncapiCtrlCmdGsyncSetEventNotification(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_SET_EVENT_NOTIFICATION_PARAMS *pSetEventParams) {
    NV_ASSERT_FAILED_PRECOMP("GSyncApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_gsync_api_h_disabled
#define gsyncapiCtrlCmdGsyncSetEventNotification(pGsyncApi, pSetEventParams) gsyncapiCtrlCmdGsyncSetEventNotification_IMPL(pGsyncApi, pSetEventParams)
#endif // __nvoc_gsync_api_h_disabled

NV_STATUS gsyncapiCtrlCmdGsyncGetControlStereoLockMode_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_CMD_GSYNC_GET_CONTROL_STEREO_LOCK_MODE_PARAMS *pParams);
#ifdef __nvoc_gsync_api_h_disabled
static inline NV_STATUS gsyncapiCtrlCmdGsyncGetControlStereoLockMode(struct GSyncApi *pGsyncApi, NV30F1_CTRL_CMD_GSYNC_GET_CONTROL_STEREO_LOCK_MODE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("GSyncApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_gsync_api_h_disabled
#define gsyncapiCtrlCmdGsyncGetControlStereoLockMode(pGsyncApi, pParams) gsyncapiCtrlCmdGsyncGetControlStereoLockMode_IMPL(pGsyncApi, pParams)
#endif // __nvoc_gsync_api_h_disabled

NV_STATUS gsyncapiCtrlCmdGsyncSetControlStereoLockMode_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_CMD_GSYNC_SET_CONTROL_STEREO_LOCK_MODE_PARAMS *pParams);
#ifdef __nvoc_gsync_api_h_disabled
static inline NV_STATUS gsyncapiCtrlCmdGsyncSetControlStereoLockMode(struct GSyncApi *pGsyncApi, NV30F1_CTRL_CMD_GSYNC_SET_CONTROL_STEREO_LOCK_MODE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("GSyncApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_gsync_api_h_disabled
#define gsyncapiCtrlCmdGsyncSetControlStereoLockMode(pGsyncApi, pParams) gsyncapiCtrlCmdGsyncSetControlStereoLockMode_IMPL(pGsyncApi, pParams)
#endif // __nvoc_gsync_api_h_disabled

NV_STATUS gsyncapiCtrlCmdGsyncReadRegister_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_READ_REGISTER_PARAMS *pParams);
#ifdef __nvoc_gsync_api_h_disabled
static inline NV_STATUS gsyncapiCtrlCmdGsyncReadRegister(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_READ_REGISTER_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("GSyncApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_gsync_api_h_disabled
#define gsyncapiCtrlCmdGsyncReadRegister(pGsyncApi, pParams) gsyncapiCtrlCmdGsyncReadRegister_IMPL(pGsyncApi, pParams)
#endif // __nvoc_gsync_api_h_disabled

NV_STATUS gsyncapiCtrlCmdGsyncWriteRegister_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_WRITE_REGISTER_PARAMS *pParams);
#ifdef __nvoc_gsync_api_h_disabled
static inline NV_STATUS gsyncapiCtrlCmdGsyncWriteRegister(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_WRITE_REGISTER_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("GSyncApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_gsync_api_h_disabled
#define gsyncapiCtrlCmdGsyncWriteRegister(pGsyncApi, pParams) gsyncapiCtrlCmdGsyncWriteRegister_IMPL(pGsyncApi, pParams)
#endif // __nvoc_gsync_api_h_disabled

NV_STATUS gsyncapiCtrlCmdGsyncSetLocalSync_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_SET_LOCAL_SYNC_PARAMS *pParams);
#ifdef __nvoc_gsync_api_h_disabled
static inline NV_STATUS gsyncapiCtrlCmdGsyncSetLocalSync(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_SET_LOCAL_SYNC_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("GSyncApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_gsync_api_h_disabled
#define gsyncapiCtrlCmdGsyncSetLocalSync(pGsyncApi, pParams) gsyncapiCtrlCmdGsyncSetLocalSync_IMPL(pGsyncApi, pParams)
#endif // __nvoc_gsync_api_h_disabled

NV_STATUS gsyncapiCtrlCmdGsyncConfigFlash_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_CMD_GSYNC_CONFIG_FLASH_PARAMS *pParams);
#ifdef __nvoc_gsync_api_h_disabled
static inline NV_STATUS gsyncapiCtrlCmdGsyncConfigFlash(struct GSyncApi *pGsyncApi, NV30F1_CTRL_CMD_GSYNC_CONFIG_FLASH_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("GSyncApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_gsync_api_h_disabled
#define gsyncapiCtrlCmdGsyncConfigFlash(pGsyncApi, pParams) gsyncapiCtrlCmdGsyncConfigFlash_IMPL(pGsyncApi, pParams)
#endif // __nvoc_gsync_api_h_disabled

NV_STATUS gsyncapiCtrlCmdGsyncGetHouseSyncMode_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_HOUSE_SYNC_MODE_PARAMS *pParams);
#ifdef __nvoc_gsync_api_h_disabled
static inline NV_STATUS gsyncapiCtrlCmdGsyncGetHouseSyncMode(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_HOUSE_SYNC_MODE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("GSyncApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_gsync_api_h_disabled
#define gsyncapiCtrlCmdGsyncGetHouseSyncMode(pGsyncApi, pParams) gsyncapiCtrlCmdGsyncGetHouseSyncMode_IMPL(pGsyncApi, pParams)
#endif // __nvoc_gsync_api_h_disabled

NV_STATUS gsyncapiCtrlCmdGsyncSetHouseSyncMode_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_HOUSE_SYNC_MODE_PARAMS *pParams);
#ifdef __nvoc_gsync_api_h_disabled
static inline NV_STATUS gsyncapiCtrlCmdGsyncSetHouseSyncMode(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_HOUSE_SYNC_MODE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("GSyncApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_gsync_api_h_disabled
#define gsyncapiCtrlCmdGsyncSetHouseSyncMode(pGsyncApi, pParams) gsyncapiCtrlCmdGsyncSetHouseSyncMode_IMPL(pGsyncApi, pParams)
#endif // __nvoc_gsync_api_h_disabled


// Wrapper macros for halified functions
#define gsyncapiControl_FNPTR(pGsyncApi) pGsyncApi->__nvoc_metadata_ptr->vtable.__gsyncapiControl__
#define gsyncapiControl(pGsyncApi, pCallContext, pParams) gsyncapiControl_DISPATCH(pGsyncApi, pCallContext, pParams)
#define gsyncapiAccessCallback_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresAccessCallback__
#define gsyncapiAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) gsyncapiAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define gsyncapiShareCallback_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresShareCallback__
#define gsyncapiShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) gsyncapiShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define gsyncapiGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define gsyncapiGetMemInterMapParams(pRmResource, pParams) gsyncapiGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define gsyncapiCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define gsyncapiCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) gsyncapiCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define gsyncapiGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define gsyncapiGetMemoryMappingDescriptor(pRmResource, ppMemDesc) gsyncapiGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define gsyncapiControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Prologue__
#define gsyncapiControlSerialization_Prologue(pResource, pCallContext, pParams) gsyncapiControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define gsyncapiControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Epilogue__
#define gsyncapiControlSerialization_Epilogue(pResource, pCallContext, pParams) gsyncapiControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define gsyncapiControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Prologue__
#define gsyncapiControl_Prologue(pResource, pCallContext, pParams) gsyncapiControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define gsyncapiControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Epilogue__
#define gsyncapiControl_Epilogue(pResource, pCallContext, pParams) gsyncapiControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define gsyncapiCanCopy_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define gsyncapiCanCopy(pResource) gsyncapiCanCopy_DISPATCH(pResource)
#define gsyncapiIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define gsyncapiIsDuplicate(pResource, hMemory, pDuplicate) gsyncapiIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define gsyncapiPreDestruct_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define gsyncapiPreDestruct(pResource) gsyncapiPreDestruct_DISPATCH(pResource)
#define gsyncapiControlFilter_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define gsyncapiControlFilter(pResource, pCallContext, pParams) gsyncapiControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define gsyncapiMap_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__
#define gsyncapiMap(pResource, pCallContext, pParams, pCpuMapping) gsyncapiMap_DISPATCH(pResource, pCallContext, pParams, pCpuMapping)
#define gsyncapiUnmap_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__
#define gsyncapiUnmap(pResource, pCallContext, pCpuMapping) gsyncapiUnmap_DISPATCH(pResource, pCallContext, pCpuMapping)
#define gsyncapiIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define gsyncapiIsPartialUnmapSupported(pResource) gsyncapiIsPartialUnmapSupported_DISPATCH(pResource)
#define gsyncapiMapTo_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define gsyncapiMapTo(pResource, pParams) gsyncapiMapTo_DISPATCH(pResource, pParams)
#define gsyncapiUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define gsyncapiUnmapFrom(pResource, pParams) gsyncapiUnmapFrom_DISPATCH(pResource, pParams)
#define gsyncapiGetRefCount_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define gsyncapiGetRefCount(pResource) gsyncapiGetRefCount_DISPATCH(pResource)
#define gsyncapiAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define gsyncapiAddAdditionalDependants(pClient, pResource, pReference) gsyncapiAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define gsyncapiGetNotificationListPtr_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_metadata_ptr->vtable.__notifyGetNotificationListPtr__
#define gsyncapiGetNotificationListPtr(pNotifier) gsyncapiGetNotificationListPtr_DISPATCH(pNotifier)
#define gsyncapiGetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_metadata_ptr->vtable.__notifyGetNotificationShare__
#define gsyncapiGetNotificationShare(pNotifier) gsyncapiGetNotificationShare_DISPATCH(pNotifier)
#define gsyncapiSetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_metadata_ptr->vtable.__notifySetNotificationShare__
#define gsyncapiSetNotificationShare(pNotifier, pNotifShare) gsyncapiSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define gsyncapiUnregisterEvent_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_metadata_ptr->vtable.__notifyUnregisterEvent__
#define gsyncapiUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) gsyncapiUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define gsyncapiGetOrAllocNotifShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_metadata_ptr->vtable.__notifyGetOrAllocNotifShare__
#define gsyncapiGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) gsyncapiGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)

// Dispatch functions
static inline NV_STATUS gsyncapiControl_DISPATCH(struct GSyncApi *pGsyncApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGsyncApi->__nvoc_metadata_ptr->vtable.__gsyncapiControl__(pGsyncApi, pCallContext, pParams);
}

static inline NvBool gsyncapiAccessCallback_DISPATCH(struct GSyncApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_metadata_ptr->vtable.__gsyncapiAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool gsyncapiShareCallback_DISPATCH(struct GSyncApi *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__nvoc_metadata_ptr->vtable.__gsyncapiShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS gsyncapiGetMemInterMapParams_DISPATCH(struct GSyncApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__gsyncapiGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS gsyncapiCheckMemInterUnmap_DISPATCH(struct GSyncApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__gsyncapiCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS gsyncapiGetMemoryMappingDescriptor_DISPATCH(struct GSyncApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__gsyncapiGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS gsyncapiControlSerialization_Prologue_DISPATCH(struct GSyncApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__gsyncapiControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void gsyncapiControlSerialization_Epilogue_DISPATCH(struct GSyncApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__gsyncapiControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS gsyncapiControl_Prologue_DISPATCH(struct GSyncApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__gsyncapiControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void gsyncapiControl_Epilogue_DISPATCH(struct GSyncApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__gsyncapiControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool gsyncapiCanCopy_DISPATCH(struct GSyncApi *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__gsyncapiCanCopy__(pResource);
}

static inline NV_STATUS gsyncapiIsDuplicate_DISPATCH(struct GSyncApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_metadata_ptr->vtable.__gsyncapiIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void gsyncapiPreDestruct_DISPATCH(struct GSyncApi *pResource) {
    pResource->__nvoc_metadata_ptr->vtable.__gsyncapiPreDestruct__(pResource);
}

static inline NV_STATUS gsyncapiControlFilter_DISPATCH(struct GSyncApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__gsyncapiControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS gsyncapiMap_DISPATCH(struct GSyncApi *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pResource->__nvoc_metadata_ptr->vtable.__gsyncapiMap__(pResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS gsyncapiUnmap_DISPATCH(struct GSyncApi *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pResource->__nvoc_metadata_ptr->vtable.__gsyncapiUnmap__(pResource, pCallContext, pCpuMapping);
}

static inline NvBool gsyncapiIsPartialUnmapSupported_DISPATCH(struct GSyncApi *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__gsyncapiIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS gsyncapiMapTo_DISPATCH(struct GSyncApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__gsyncapiMapTo__(pResource, pParams);
}

static inline NV_STATUS gsyncapiUnmapFrom_DISPATCH(struct GSyncApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__gsyncapiUnmapFrom__(pResource, pParams);
}

static inline NvU32 gsyncapiGetRefCount_DISPATCH(struct GSyncApi *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__gsyncapiGetRefCount__(pResource);
}

static inline void gsyncapiAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct GSyncApi *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_metadata_ptr->vtable.__gsyncapiAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline PEVENTNOTIFICATION * gsyncapiGetNotificationListPtr_DISPATCH(struct GSyncApi *pNotifier) {
    return pNotifier->__nvoc_metadata_ptr->vtable.__gsyncapiGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare * gsyncapiGetNotificationShare_DISPATCH(struct GSyncApi *pNotifier) {
    return pNotifier->__nvoc_metadata_ptr->vtable.__gsyncapiGetNotificationShare__(pNotifier);
}

static inline void gsyncapiSetNotificationShare_DISPATCH(struct GSyncApi *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__nvoc_metadata_ptr->vtable.__gsyncapiSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NV_STATUS gsyncapiUnregisterEvent_DISPATCH(struct GSyncApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__nvoc_metadata_ptr->vtable.__gsyncapiUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS gsyncapiGetOrAllocNotifShare_DISPATCH(struct GSyncApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__nvoc_metadata_ptr->vtable.__gsyncapiGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

NV_STATUS gsyncapiControl_IMPL(struct GSyncApi *pGsyncApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);

NV_STATUS gsyncapiCtrlCmdGsyncGetVersion_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_VERSION_PARAMS *pGsyncGetVersionParams);

NV_STATUS gsyncapiCtrlCmdGetGsyncGpuTopology_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GET_GSYNC_GPU_TOPOLOGY_PARAMS *pParams);

NV_STATUS gsyncapiCtrlCmdGsyncGetStatusSignals_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_STATUS_SIGNALS_PARAMS *pParams);

NV_STATUS gsyncapiCtrlCmdGsyncGetControlParams_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_CONTROL_PARAMS_PARAMS *pParams);

NV_STATUS gsyncapiCtrlCmdGsyncSetControlParams_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_SET_CONTROL_PARAMS_PARAMS *pParams);

NV_STATUS gsyncapiCtrlCmdGsyncGetControlSync_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_CONTROL_SYNC_PARAMS *pParams);

NV_STATUS gsyncapiCtrlCmdGsyncSetControlSync_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_SET_CONTROL_SYNC_PARAMS *pParams);

NV_STATUS gsyncapiCtrlCmdGsyncSetControlUnsync_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_SET_CONTROL_UNSYNC_PARAMS *pParams);

NV_STATUS gsyncapiCtrlCmdGsyncGetStatusSync_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_STATUS_SYNC_PARAMS *pParams);

NV_STATUS gsyncapiCtrlCmdGsyncGetStatus_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_STATUS_PARAMS *pParams);

NV_STATUS gsyncapiCtrlCmdGsyncGetControlTesting_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_CONTROL_TESTING_PARAMS *pParams);

NV_STATUS gsyncapiCtrlCmdGsyncSetControlTesting_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_SET_CONTROL_TESTING_PARAMS *pParams);

NV_STATUS gsyncapiCtrlCmdGsyncSetControlWatchdog_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_SET_CONTROL_WATCHDOG_PARAMS *pParams);

NV_STATUS gsyncapiCtrlCmdGsyncGetControlInterlaceMode_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_CONTROL_INTERLACE_MODE_PARAMS *pParams);

NV_STATUS gsyncapiCtrlCmdGsyncSetControlInterlaceMode_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_SET_CONTROL_INTERLACE_MODE_PARAMS *pParams);

NV_STATUS gsyncapiCtrlCmdGsyncGetControlSwapBarrier_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_CONTROL_SWAP_BARRIER_PARAMS *pParams);

NV_STATUS gsyncapiCtrlCmdGsyncSetControlSwapBarrier_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_SET_CONTROL_SWAP_BARRIER_PARAMS *pParams);

NV_STATUS gsyncapiCtrlCmdGsyncGetControlSwapLockWindow_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_CONTROL_SWAP_LOCK_WINDOW_PARAMS *pParams);

NV_STATUS gsyncapiCtrlCmdGsyncGetCaps_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_CAPS_PARAMS *pParams);

NV_STATUS gsyncapiCtrlCmdGsyncGetOptimizedTiming_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_GET_OPTIMIZED_TIMING_PARAMS *pParams);

NV_STATUS gsyncapiCtrlCmdGsyncSetEventNotification_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_SET_EVENT_NOTIFICATION_PARAMS *pSetEventParams);

NV_STATUS gsyncapiCtrlCmdGsyncGetControlStereoLockMode_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_CMD_GSYNC_GET_CONTROL_STEREO_LOCK_MODE_PARAMS *pParams);

NV_STATUS gsyncapiCtrlCmdGsyncSetControlStereoLockMode_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_CMD_GSYNC_SET_CONTROL_STEREO_LOCK_MODE_PARAMS *pParams);

NV_STATUS gsyncapiCtrlCmdGsyncReadRegister_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_READ_REGISTER_PARAMS *pParams);

NV_STATUS gsyncapiCtrlCmdGsyncWriteRegister_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_WRITE_REGISTER_PARAMS *pParams);

NV_STATUS gsyncapiCtrlCmdGsyncSetLocalSync_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_SET_LOCAL_SYNC_PARAMS *pParams);

NV_STATUS gsyncapiCtrlCmdGsyncConfigFlash_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_CMD_GSYNC_CONFIG_FLASH_PARAMS *pParams);

NV_STATUS gsyncapiCtrlCmdGsyncGetHouseSyncMode_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_HOUSE_SYNC_MODE_PARAMS *pParams);

NV_STATUS gsyncapiCtrlCmdGsyncSetHouseSyncMode_IMPL(struct GSyncApi *pGsyncApi, NV30F1_CTRL_GSYNC_HOUSE_SYNC_MODE_PARAMS *pParams);

#undef PRIVATE_FIELD


void    CliNotifyGsyncEvent     (NvU32, NvU32);

#endif // GSYNCAPI_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_GSYNC_API_NVOC_H_
