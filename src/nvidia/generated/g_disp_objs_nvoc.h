
#ifndef _G_DISP_OBJS_NVOC_H_
#define _G_DISP_OBJS_NVOC_H_
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
 * SPDX-FileCopyrightText: Copyright (c) 1993-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/******************************************************************************
*
*   Description:
*       This file contains functions managing the display - both Disp and DispCommon
*       entries with their insides (DispChannelList and DispDmaControlList)
*
******************************************************************************/

#pragma once
#include "g_disp_objs_nvoc.h"

#ifndef DISP_OBJS_H
#define DISP_OBJS_H

#include "rmapi/event.h"
#include "rmapi/resource.h"

#include "gpu/gpu_halspec.h"

#include "ctrl/ctrl0073.h"
#include "ctrl/ctrl5070/ctrl5070event.h"
#include "ctrl/ctrl5070/ctrl5070or.h"
#include "ctrl/ctrl5070/ctrl5070system.h"
#include "ctrl/ctrlc370/ctrlc370chnc.h"
#include "ctrl/ctrlc370/ctrlc370event.h"
#include "ctrl/ctrlc370/ctrlc370rg.h"
#include "ctrl/ctrlc370/ctrlc370or.h"
#include "ctrl/ctrlc370/ctrlc370verif.h"
#include "ctrl/ctrlc372/ctrlc372base.h"
#include "ctrl/ctrlc372/ctrlc372chnc.h"

// ****************************************************************************
//                          Type definitions
// ****************************************************************************

struct OBJGPU;
struct Device;
struct Memory;
struct RsResource;
struct RmResource;

struct DispChannel;

#ifndef __NVOC_CLASS_DispChannel_TYPEDEF__
#define __NVOC_CLASS_DispChannel_TYPEDEF__
typedef struct DispChannel DispChannel;
#endif /* __NVOC_CLASS_DispChannel_TYPEDEF__ */

#ifndef __nvoc_class_id_DispChannel
#define __nvoc_class_id_DispChannel 0xbd2ff3
#endif /* __nvoc_class_id_DispChannel */



#define DISPAPI_GET_GPU(pDispRes) staticCast(pDispRes, DisplayApi)->pGpuInRmctrl

#define DISPAPI_GET_GPUGRP(pDispRes) staticCast(pDispRes, DisplayApi)->pGpuGrp

/*!
 * Base class for many of display's RsResource subclasses
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_DISP_OBJS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct DisplayApi {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct RmResource __nvoc_base_RmResource;
    struct Notifier __nvoc_base_Notifier;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^3
    struct RsResource *__nvoc_pbase_RsResource;    // res super^2
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^2
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super
    struct INotifier *__nvoc_pbase_INotifier;    // inotify super^2
    struct Notifier *__nvoc_pbase_Notifier;    // notify super
    struct DisplayApi *__nvoc_pbase_DisplayApi;    // dispapi

    // Vtable with 26 per-object function pointers
    NV_STATUS (*__dispapiControl__)(struct DisplayApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual override (res) base (rmres)
    NV_STATUS (*__dispapiControl_Prologue__)(struct DisplayApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual override (res) base (rmres)
    void (*__dispapiControl_Epilogue__)(struct DisplayApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual override (res) base (rmres)
    NvBool (*__dispapiAccessCallback__)(struct DisplayApi * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (rmres)
    NvBool (*__dispapiShareCallback__)(struct DisplayApi * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__dispapiGetMemInterMapParams__)(struct DisplayApi * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__dispapiCheckMemInterUnmap__)(struct DisplayApi * /*this*/, NvBool);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__dispapiGetMemoryMappingDescriptor__)(struct DisplayApi * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__dispapiControlSerialization_Prologue__)(struct DisplayApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (rmres)
    void (*__dispapiControlSerialization_Epilogue__)(struct DisplayApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (rmres)
    NvBool (*__dispapiCanCopy__)(struct DisplayApi * /*this*/);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__dispapiIsDuplicate__)(struct DisplayApi * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (rmres)
    void (*__dispapiPreDestruct__)(struct DisplayApi * /*this*/);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__dispapiControlFilter__)(struct DisplayApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__dispapiMap__)(struct DisplayApi * /*this*/, struct CALL_CONTEXT *, RS_CPU_MAP_PARAMS *, RsCpuMapping *);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__dispapiUnmap__)(struct DisplayApi * /*this*/, struct CALL_CONTEXT *, RsCpuMapping *);  // virtual inherited (res) base (rmres)
    NvBool (*__dispapiIsPartialUnmapSupported__)(struct DisplayApi * /*this*/);  // inline virtual inherited (res) base (rmres) body
    NV_STATUS (*__dispapiMapTo__)(struct DisplayApi * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__dispapiUnmapFrom__)(struct DisplayApi * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (rmres)
    NvU32 (*__dispapiGetRefCount__)(struct DisplayApi * /*this*/);  // virtual inherited (res) base (rmres)
    void (*__dispapiAddAdditionalDependants__)(struct RsClient *, struct DisplayApi * /*this*/, RsResourceRef *);  // virtual inherited (res) base (rmres)
    PEVENTNOTIFICATION * (*__dispapiGetNotificationListPtr__)(struct DisplayApi * /*this*/);  // virtual inherited (notify) base (notify)
    struct NotifShare * (*__dispapiGetNotificationShare__)(struct DisplayApi * /*this*/);  // virtual inherited (notify) base (notify)
    void (*__dispapiSetNotificationShare__)(struct DisplayApi * /*this*/, struct NotifShare *);  // virtual inherited (notify) base (notify)
    NV_STATUS (*__dispapiUnregisterEvent__)(struct DisplayApi * /*this*/, NvHandle, NvHandle, NvHandle, NvHandle);  // virtual inherited (notify) base (notify)
    NV_STATUS (*__dispapiGetOrAllocNotifShare__)(struct DisplayApi * /*this*/, NvHandle, NvHandle, struct NotifShare **);  // virtual inherited (notify) base (notify)

    // Data members
    struct OBJGPU *pGpuInRmctrl;
    struct OBJGPUGRP *pGpuGrp;
    NvBool bBcResource;
    NvU32 *pNotifyActions[8];
    NvU32 numNotifiers;
    NvHandle hNotifierMemory;
    struct Memory *pNotifierMemory;
};

#ifndef __NVOC_CLASS_DisplayApi_TYPEDEF__
#define __NVOC_CLASS_DisplayApi_TYPEDEF__
typedef struct DisplayApi DisplayApi;
#endif /* __NVOC_CLASS_DisplayApi_TYPEDEF__ */

#ifndef __nvoc_class_id_DisplayApi
#define __nvoc_class_id_DisplayApi 0xe9980c
#endif /* __nvoc_class_id_DisplayApi */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_DisplayApi;

#define __staticCast_DisplayApi(pThis) \
    ((pThis)->__nvoc_pbase_DisplayApi)

#ifdef __nvoc_disp_objs_h_disabled
#define __dynamicCast_DisplayApi(pThis) ((DisplayApi*)NULL)
#else //__nvoc_disp_objs_h_disabled
#define __dynamicCast_DisplayApi(pThis) \
    ((DisplayApi*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(DisplayApi)))
#endif //__nvoc_disp_objs_h_disabled

NV_STATUS __nvoc_objCreateDynamic_DisplayApi(DisplayApi**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_DisplayApi(DisplayApi**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_DisplayApi(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_DisplayApi((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define dispapiControl_FNPTR(pDisplayApi) pDisplayApi->__dispapiControl__
#define dispapiControl(pDisplayApi, pCallContext, pParams) dispapiControl_DISPATCH(pDisplayApi, pCallContext, pParams)
#define dispapiControl_Prologue_FNPTR(pDisplayApi) pDisplayApi->__dispapiControl_Prologue__
#define dispapiControl_Prologue(pDisplayApi, pCallContext, pRsParams) dispapiControl_Prologue_DISPATCH(pDisplayApi, pCallContext, pRsParams)
#define dispapiControl_Epilogue_FNPTR(pDisplayApi) pDisplayApi->__dispapiControl_Epilogue__
#define dispapiControl_Epilogue(pDisplayApi, pCallContext, pRsParams) dispapiControl_Epilogue_DISPATCH(pDisplayApi, pCallContext, pRsParams)
#define dispapiAccessCallback_FNPTR(pResource) pResource->__nvoc_base_RmResource.__rmresAccessCallback__
#define dispapiAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) dispapiAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define dispapiShareCallback_FNPTR(pResource) pResource->__nvoc_base_RmResource.__rmresShareCallback__
#define dispapiShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) dispapiShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define dispapiGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__rmresGetMemInterMapParams__
#define dispapiGetMemInterMapParams(pRmResource, pParams) dispapiGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define dispapiCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__rmresCheckMemInterUnmap__
#define dispapiCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) dispapiCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define dispapiGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__rmresGetMemoryMappingDescriptor__
#define dispapiGetMemoryMappingDescriptor(pRmResource, ppMemDesc) dispapiGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define dispapiControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__rmresControlSerialization_Prologue__
#define dispapiControlSerialization_Prologue(pResource, pCallContext, pParams) dispapiControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define dispapiControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__rmresControlSerialization_Epilogue__
#define dispapiControlSerialization_Epilogue(pResource, pCallContext, pParams) dispapiControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define dispapiCanCopy_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__
#define dispapiCanCopy(pResource) dispapiCanCopy_DISPATCH(pResource)
#define dispapiIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsDuplicate__
#define dispapiIsDuplicate(pResource, hMemory, pDuplicate) dispapiIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define dispapiPreDestruct_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resPreDestruct__
#define dispapiPreDestruct(pResource) dispapiPreDestruct_DISPATCH(pResource)
#define dispapiControlFilter_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resControlFilter__
#define dispapiControlFilter(pResource, pCallContext, pParams) dispapiControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define dispapiMap_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resMap__
#define dispapiMap(pResource, pCallContext, pParams, pCpuMapping) dispapiMap_DISPATCH(pResource, pCallContext, pParams, pCpuMapping)
#define dispapiUnmap_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmap__
#define dispapiUnmap(pResource, pCallContext, pCpuMapping) dispapiUnmap_DISPATCH(pResource, pCallContext, pCpuMapping)
#define dispapiIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsPartialUnmapSupported__
#define dispapiIsPartialUnmapSupported(pResource) dispapiIsPartialUnmapSupported_DISPATCH(pResource)
#define dispapiMapTo_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resMapTo__
#define dispapiMapTo(pResource, pParams) dispapiMapTo_DISPATCH(pResource, pParams)
#define dispapiUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmapFrom__
#define dispapiUnmapFrom(pResource, pParams) dispapiUnmapFrom_DISPATCH(pResource, pParams)
#define dispapiGetRefCount_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resGetRefCount__
#define dispapiGetRefCount(pResource) dispapiGetRefCount_DISPATCH(pResource)
#define dispapiAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resAddAdditionalDependants__
#define dispapiAddAdditionalDependants(pClient, pResource, pReference) dispapiAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define dispapiGetNotificationListPtr_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__notifyGetNotificationListPtr__
#define dispapiGetNotificationListPtr(pNotifier) dispapiGetNotificationListPtr_DISPATCH(pNotifier)
#define dispapiGetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__notifyGetNotificationShare__
#define dispapiGetNotificationShare(pNotifier) dispapiGetNotificationShare_DISPATCH(pNotifier)
#define dispapiSetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__notifySetNotificationShare__
#define dispapiSetNotificationShare(pNotifier, pNotifShare) dispapiSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define dispapiUnregisterEvent_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__notifyUnregisterEvent__
#define dispapiUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) dispapiUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define dispapiGetOrAllocNotifShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__notifyGetOrAllocNotifShare__
#define dispapiGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) dispapiGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)

// Dispatch functions
static inline NV_STATUS dispapiControl_DISPATCH(struct DisplayApi *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pDisplayApi->__dispapiControl__(pDisplayApi, pCallContext, pParams);
}

static inline NV_STATUS dispapiControl_Prologue_DISPATCH(struct DisplayApi *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams) {
    return pDisplayApi->__dispapiControl_Prologue__(pDisplayApi, pCallContext, pRsParams);
}

static inline void dispapiControl_Epilogue_DISPATCH(struct DisplayApi *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams) {
    pDisplayApi->__dispapiControl_Epilogue__(pDisplayApi, pCallContext, pRsParams);
}

static inline NvBool dispapiAccessCallback_DISPATCH(struct DisplayApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__dispapiAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool dispapiShareCallback_DISPATCH(struct DisplayApi *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__dispapiShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS dispapiGetMemInterMapParams_DISPATCH(struct DisplayApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__dispapiGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS dispapiCheckMemInterUnmap_DISPATCH(struct DisplayApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__dispapiCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS dispapiGetMemoryMappingDescriptor_DISPATCH(struct DisplayApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__dispapiGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS dispapiControlSerialization_Prologue_DISPATCH(struct DisplayApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__dispapiControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void dispapiControlSerialization_Epilogue_DISPATCH(struct DisplayApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__dispapiControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool dispapiCanCopy_DISPATCH(struct DisplayApi *pResource) {
    return pResource->__dispapiCanCopy__(pResource);
}

static inline NV_STATUS dispapiIsDuplicate_DISPATCH(struct DisplayApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__dispapiIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void dispapiPreDestruct_DISPATCH(struct DisplayApi *pResource) {
    pResource->__dispapiPreDestruct__(pResource);
}

static inline NV_STATUS dispapiControlFilter_DISPATCH(struct DisplayApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__dispapiControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS dispapiMap_DISPATCH(struct DisplayApi *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pResource->__dispapiMap__(pResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS dispapiUnmap_DISPATCH(struct DisplayApi *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pResource->__dispapiUnmap__(pResource, pCallContext, pCpuMapping);
}

static inline NvBool dispapiIsPartialUnmapSupported_DISPATCH(struct DisplayApi *pResource) {
    return pResource->__dispapiIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS dispapiMapTo_DISPATCH(struct DisplayApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__dispapiMapTo__(pResource, pParams);
}

static inline NV_STATUS dispapiUnmapFrom_DISPATCH(struct DisplayApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__dispapiUnmapFrom__(pResource, pParams);
}

static inline NvU32 dispapiGetRefCount_DISPATCH(struct DisplayApi *pResource) {
    return pResource->__dispapiGetRefCount__(pResource);
}

static inline void dispapiAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct DisplayApi *pResource, RsResourceRef *pReference) {
    pResource->__dispapiAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline PEVENTNOTIFICATION * dispapiGetNotificationListPtr_DISPATCH(struct DisplayApi *pNotifier) {
    return pNotifier->__dispapiGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare * dispapiGetNotificationShare_DISPATCH(struct DisplayApi *pNotifier) {
    return pNotifier->__dispapiGetNotificationShare__(pNotifier);
}

static inline void dispapiSetNotificationShare_DISPATCH(struct DisplayApi *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__dispapiSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NV_STATUS dispapiUnregisterEvent_DISPATCH(struct DisplayApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__dispapiUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS dispapiGetOrAllocNotifShare_DISPATCH(struct DisplayApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__dispapiGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

NV_STATUS dispapiSetUnicastAndSynchronize_KERNEL(struct DisplayApi *pDisplayApi, struct OBJGPUGRP *pGpuGroup, struct OBJGPU **ppGpu, OBJDISP **ppDisp, NvU32 subDeviceInstance);


#ifdef __nvoc_disp_objs_h_disabled
static inline NV_STATUS dispapiSetUnicastAndSynchronize(struct DisplayApi *pDisplayApi, struct OBJGPUGRP *pGpuGroup, struct OBJGPU **ppGpu, OBJDISP **ppDisp, NvU32 subDeviceInstance) {
    NV_ASSERT_FAILED_PRECOMP("DisplayApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_disp_objs_h_disabled
#define dispapiSetUnicastAndSynchronize(pDisplayApi, pGpuGroup, ppGpu, ppDisp, subDeviceInstance) dispapiSetUnicastAndSynchronize_KERNEL(pDisplayApi, pGpuGroup, ppGpu, ppDisp, subDeviceInstance)
#endif //__nvoc_disp_objs_h_disabled

#define dispapiSetUnicastAndSynchronize_HAL(pDisplayApi, pGpuGroup, ppGpu, ppDisp, subDeviceInstance) dispapiSetUnicastAndSynchronize(pDisplayApi, pGpuGroup, ppGpu, ppDisp, subDeviceInstance)

NV_STATUS dispapiControl_IMPL(struct DisplayApi *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);

NV_STATUS dispapiControl_Prologue_IMPL(struct DisplayApi *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams);

void dispapiControl_Epilogue_IMPL(struct DisplayApi *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams);

NV_STATUS dispapiConstruct_IMPL(struct DisplayApi *arg_pDisplayApi, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_dispapiConstruct(arg_pDisplayApi, arg_pCallContext, arg_pParams) dispapiConstruct_IMPL(arg_pDisplayApi, arg_pCallContext, arg_pParams)
void dispapiDestruct_IMPL(struct DisplayApi *pDisplayApi);

#define __nvoc_dispapiDestruct(pDisplayApi) dispapiDestruct_IMPL(pDisplayApi)
NV_STATUS dispapiCtrlCmdEventSetNotification_IMPL(struct DisplayApi *pDisplayApi, NV5070_CTRL_EVENT_SET_NOTIFICATION_PARAMS *pSetEventParams);

#ifdef __nvoc_disp_objs_h_disabled
static inline NV_STATUS dispapiCtrlCmdEventSetNotification(struct DisplayApi *pDisplayApi, NV5070_CTRL_EVENT_SET_NOTIFICATION_PARAMS *pSetEventParams) {
    NV_ASSERT_FAILED_PRECOMP("DisplayApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_disp_objs_h_disabled
#define dispapiCtrlCmdEventSetNotification(pDisplayApi, pSetEventParams) dispapiCtrlCmdEventSetNotification_IMPL(pDisplayApi, pSetEventParams)
#endif //__nvoc_disp_objs_h_disabled

#undef PRIVATE_FIELD


/*!
 * RM internal class representing XXX_DISPLAY. Parent for all other display
 * resources (channels, etc). Allocated under a device or subdevice.
 *
 * Only one instance of this class is allowed per-GPU. Multi-instance restrictions
 * are enforced by resource_list.h
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_DISP_OBJS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct DispObject {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct DisplayApi __nvoc_base_DisplayApi;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^4
    struct RsResource *__nvoc_pbase_RsResource;    // res super^3
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^3
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super^2
    struct INotifier *__nvoc_pbase_INotifier;    // inotify super^3
    struct Notifier *__nvoc_pbase_Notifier;    // notify super^2
    struct DisplayApi *__nvoc_pbase_DisplayApi;    // dispapi super
    struct DispObject *__nvoc_pbase_DispObject;    // dispobj

    // Vtable with 46 per-object function pointers
    NV_STATUS (*__dispobjCtrlCmdGetPinsetCount__)(struct DispObject * /*this*/, NV5070_CTRL_GET_PINSET_COUNT_PARAMS *);  // exported (id=0x50700115)
    NV_STATUS (*__dispobjCtrlCmdGetPinsetPeer__)(struct DispObject * /*this*/, NV5070_CTRL_GET_PINSET_PEER_PARAMS *);  // exported (id=0x50700116)
    NV_STATUS (*__dispobjCtrlCmdSetMempoolWARForBlitTearing__)(struct DispObject * /*this*/, NV5070_CTRL_SET_MEMPOOL_WAR_FOR_BLIT_TEARING_PARAMS *);  // exported (id=0x50700119)
    NV_STATUS (*__dispobjCtrlCmdGetPinsetLockpins__)(struct DispObject * /*this*/, NV5070_CTRL_GET_PINSET_LOCKPINS_PARAMS *);  // exported (id=0x5070020b)
    NV_STATUS (*__dispobjCtrlCmdGetFrameLockHeaderLockPins__)(struct DispObject * /*this*/, NV5070_CTRL_GET_FRAMELOCK_HEADER_LOCKPINS_PARAMS *);  // exported (id=0x5070020d)
    NV_STATUS (*__dispobjCtrlCmdEventSetMemoryNotifies__)(struct DispObject * /*this*/, NV5070_CTRL_EVENT_SET_MEMORY_NOTIFIES_PARAMS *);  // exported (id=0x50700903)
    NV_STATUS (*__dispobjCtrlCmdSetRmFreeFlags__)(struct DispObject * /*this*/, NV5070_CTRL_SET_RMFREE_FLAGS_PARAMS *);  // exported (id=0x50700117)
    NV_STATUS (*__dispobjCtrlCmdIMPSetGetParameter__)(struct DispObject * /*this*/, NV5070_CTRL_IMP_SET_GET_PARAMETER_PARAMS *);  // exported (id=0x50700118)
    NV_STATUS (*__dispobjCtrlCmdGetRgStatus__)(struct DispObject * /*this*/, NV5070_CTRL_CMD_GET_RG_STATUS_PARAMS *);  // exported (id=0x50700202)
    NV_STATUS (*__dispobjCtrlCmdGetRgUnderflowProp__)(struct DispObject * /*this*/, NV5070_CTRL_CMD_GET_RG_UNDERFLOW_PROP_PARAMS *);  // exported (id=0x50700203)
    NV_STATUS (*__dispobjCtrlCmdSetRgUnderflowProp__)(struct DispObject * /*this*/, NV5070_CTRL_CMD_SET_RG_UNDERFLOW_PROP_PARAMS *);  // exported (id=0x50700204)
    NV_STATUS (*__dispobjCtrlCmdGetRgFliplockProp__)(struct DispObject * /*this*/, NV5070_CTRL_CMD_GET_RG_FLIPLOCK_PROP_PARAMS *);  // exported (id=0x50700205)
    NV_STATUS (*__dispobjCtrlCmdSetRgFliplockProp__)(struct DispObject * /*this*/, NV5070_CTRL_CMD_SET_RG_FLIPLOCK_PROP_PARAMS *);  // exported (id=0x50700206)
    NV_STATUS (*__dispobjCtrlCmdGetRgConnectedLockpinStateless__)(struct DispObject * /*this*/, NV5070_CTRL_GET_RG_CONNECTED_LOCKPIN_STATELESS_PARAMS *);  // exported (id=0x5070020a)
    NV_STATUS (*__dispobjCtrlCmdGetRgScanLine__)(struct DispObject * /*this*/, NV5070_CTRL_CMD_GET_RG_SCAN_LINE_PARAMS *);  // exported (id=0x5070020c)
    NV_STATUS (*__dispobjCtrlCmdGetSorOpMode__)(struct DispObject * /*this*/, NV5070_CTRL_CMD_GET_SOR_OP_MODE_PARAMS *);  // exported (id=0x50700422)
    NV_STATUS (*__dispobjCtrlCmdSetSorOpMode__)(struct DispObject * /*this*/, NV5070_CTRL_CMD_SET_SOR_OP_MODE_PARAMS *);  // exported (id=0x50700423)
    NV_STATUS (*__dispobjCtrlCmdSetSorFlushMode__)(struct DispObject * /*this*/, NV5070_CTRL_SET_SOR_FLUSH_MODE_PARAMS *);  // exported (id=0x50700457)
    NV_STATUS (*__dispobjCtrlCmdSystemGetCapsV2__)(struct DispObject * /*this*/, NV5070_CTRL_SYSTEM_GET_CAPS_V2_PARAMS *);  // exported (id=0x50700709)
    NV_STATUS (*__dispobjCtrlCmdEventSetTrigger__)(struct DispObject * /*this*/);  // exported (id=0x50700902)
    NV_STATUS (*__dispobjControl__)(struct DispObject * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (dispapi) base (dispapi)
    NV_STATUS (*__dispobjControl_Prologue__)(struct DispObject * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (dispapi) base (dispapi)
    void (*__dispobjControl_Epilogue__)(struct DispObject * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (dispapi) base (dispapi)
    NvBool (*__dispobjAccessCallback__)(struct DispObject * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (dispapi)
    NvBool (*__dispobjShareCallback__)(struct DispObject * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (rmres) base (dispapi)
    NV_STATUS (*__dispobjGetMemInterMapParams__)(struct DispObject * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (dispapi)
    NV_STATUS (*__dispobjCheckMemInterUnmap__)(struct DispObject * /*this*/, NvBool);  // virtual inherited (rmres) base (dispapi)
    NV_STATUS (*__dispobjGetMemoryMappingDescriptor__)(struct DispObject * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (dispapi)
    NV_STATUS (*__dispobjControlSerialization_Prologue__)(struct DispObject * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (dispapi)
    void (*__dispobjControlSerialization_Epilogue__)(struct DispObject * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (dispapi)
    NvBool (*__dispobjCanCopy__)(struct DispObject * /*this*/);  // virtual inherited (res) base (dispapi)
    NV_STATUS (*__dispobjIsDuplicate__)(struct DispObject * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (dispapi)
    void (*__dispobjPreDestruct__)(struct DispObject * /*this*/);  // virtual inherited (res) base (dispapi)
    NV_STATUS (*__dispobjControlFilter__)(struct DispObject * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (dispapi)
    NV_STATUS (*__dispobjMap__)(struct DispObject * /*this*/, struct CALL_CONTEXT *, RS_CPU_MAP_PARAMS *, RsCpuMapping *);  // virtual inherited (res) base (dispapi)
    NV_STATUS (*__dispobjUnmap__)(struct DispObject * /*this*/, struct CALL_CONTEXT *, RsCpuMapping *);  // virtual inherited (res) base (dispapi)
    NvBool (*__dispobjIsPartialUnmapSupported__)(struct DispObject * /*this*/);  // inline virtual inherited (res) base (dispapi) body
    NV_STATUS (*__dispobjMapTo__)(struct DispObject * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (dispapi)
    NV_STATUS (*__dispobjUnmapFrom__)(struct DispObject * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (dispapi)
    NvU32 (*__dispobjGetRefCount__)(struct DispObject * /*this*/);  // virtual inherited (res) base (dispapi)
    void (*__dispobjAddAdditionalDependants__)(struct RsClient *, struct DispObject * /*this*/, RsResourceRef *);  // virtual inherited (res) base (dispapi)
    PEVENTNOTIFICATION * (*__dispobjGetNotificationListPtr__)(struct DispObject * /*this*/);  // virtual inherited (notify) base (dispapi)
    struct NotifShare * (*__dispobjGetNotificationShare__)(struct DispObject * /*this*/);  // virtual inherited (notify) base (dispapi)
    void (*__dispobjSetNotificationShare__)(struct DispObject * /*this*/, struct NotifShare *);  // virtual inherited (notify) base (dispapi)
    NV_STATUS (*__dispobjUnregisterEvent__)(struct DispObject * /*this*/, NvHandle, NvHandle, NvHandle, NvHandle);  // virtual inherited (notify) base (dispapi)
    NV_STATUS (*__dispobjGetOrAllocNotifShare__)(struct DispObject * /*this*/, NvHandle, NvHandle, struct NotifShare **);  // virtual inherited (notify) base (dispapi)

    // Data members
    NvU32 rmFreeFlags;
};

#ifndef __NVOC_CLASS_DispObject_TYPEDEF__
#define __NVOC_CLASS_DispObject_TYPEDEF__
typedef struct DispObject DispObject;
#endif /* __NVOC_CLASS_DispObject_TYPEDEF__ */

#ifndef __nvoc_class_id_DispObject
#define __nvoc_class_id_DispObject 0x999839
#endif /* __nvoc_class_id_DispObject */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_DispObject;

#define __staticCast_DispObject(pThis) \
    ((pThis)->__nvoc_pbase_DispObject)

#ifdef __nvoc_disp_objs_h_disabled
#define __dynamicCast_DispObject(pThis) ((DispObject*)NULL)
#else //__nvoc_disp_objs_h_disabled
#define __dynamicCast_DispObject(pThis) \
    ((DispObject*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(DispObject)))
#endif //__nvoc_disp_objs_h_disabled

NV_STATUS __nvoc_objCreateDynamic_DispObject(DispObject**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_DispObject(DispObject**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_DispObject(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_DispObject((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define dispobjCtrlCmdGetPinsetCount_FNPTR(pDispObject) pDispObject->__dispobjCtrlCmdGetPinsetCount__
#define dispobjCtrlCmdGetPinsetCount(pDispObject, pParams) dispobjCtrlCmdGetPinsetCount_DISPATCH(pDispObject, pParams)
#define dispobjCtrlCmdGetPinsetPeer_FNPTR(pDispObject) pDispObject->__dispobjCtrlCmdGetPinsetPeer__
#define dispobjCtrlCmdGetPinsetPeer(pDispObject, pParams) dispobjCtrlCmdGetPinsetPeer_DISPATCH(pDispObject, pParams)
#define dispobjCtrlCmdSetMempoolWARForBlitTearing_FNPTR(pDispObject) pDispObject->__dispobjCtrlCmdSetMempoolWARForBlitTearing__
#define dispobjCtrlCmdSetMempoolWARForBlitTearing(pDispObject, pParams) dispobjCtrlCmdSetMempoolWARForBlitTearing_DISPATCH(pDispObject, pParams)
#define dispobjCtrlCmdGetPinsetLockpins_FNPTR(pDispObject) pDispObject->__dispobjCtrlCmdGetPinsetLockpins__
#define dispobjCtrlCmdGetPinsetLockpins(pDispObject, pParams) dispobjCtrlCmdGetPinsetLockpins_DISPATCH(pDispObject, pParams)
#define dispobjCtrlCmdGetFrameLockHeaderLockPins_FNPTR(pDispObject) pDispObject->__dispobjCtrlCmdGetFrameLockHeaderLockPins__
#define dispobjCtrlCmdGetFrameLockHeaderLockPins(pDispObject, pParams) dispobjCtrlCmdGetFrameLockHeaderLockPins_DISPATCH(pDispObject, pParams)
#define dispobjCtrlCmdEventSetMemoryNotifies_FNPTR(pDispObject) pDispObject->__dispobjCtrlCmdEventSetMemoryNotifies__
#define dispobjCtrlCmdEventSetMemoryNotifies(pDispObject, pSetMemoryNotifiesParams) dispobjCtrlCmdEventSetMemoryNotifies_DISPATCH(pDispObject, pSetMemoryNotifiesParams)
#define dispobjCtrlCmdSetRmFreeFlags_FNPTR(pDispObject) pDispObject->__dispobjCtrlCmdSetRmFreeFlags__
#define dispobjCtrlCmdSetRmFreeFlags(pDispObject, pParams) dispobjCtrlCmdSetRmFreeFlags_DISPATCH(pDispObject, pParams)
#define dispobjCtrlCmdIMPSetGetParameter_FNPTR(pDispObject) pDispObject->__dispobjCtrlCmdIMPSetGetParameter__
#define dispobjCtrlCmdIMPSetGetParameter(pDispObject, pImpSetGetParams) dispobjCtrlCmdIMPSetGetParameter_DISPATCH(pDispObject, pImpSetGetParams)
#define dispobjCtrlCmdGetRgStatus_FNPTR(pDispObject) pDispObject->__dispobjCtrlCmdGetRgStatus__
#define dispobjCtrlCmdGetRgStatus(pDispObject, pParams) dispobjCtrlCmdGetRgStatus_DISPATCH(pDispObject, pParams)
#define dispobjCtrlCmdGetRgUnderflowProp_FNPTR(pDispObject) pDispObject->__dispobjCtrlCmdGetRgUnderflowProp__
#define dispobjCtrlCmdGetRgUnderflowProp(pDispObject, pParams) dispobjCtrlCmdGetRgUnderflowProp_DISPATCH(pDispObject, pParams)
#define dispobjCtrlCmdSetRgUnderflowProp_FNPTR(pDispObject) pDispObject->__dispobjCtrlCmdSetRgUnderflowProp__
#define dispobjCtrlCmdSetRgUnderflowProp(pDispObject, pParams) dispobjCtrlCmdSetRgUnderflowProp_DISPATCH(pDispObject, pParams)
#define dispobjCtrlCmdGetRgFliplockProp_FNPTR(pDispObject) pDispObject->__dispobjCtrlCmdGetRgFliplockProp__
#define dispobjCtrlCmdGetRgFliplockProp(pDispObject, pParams) dispobjCtrlCmdGetRgFliplockProp_DISPATCH(pDispObject, pParams)
#define dispobjCtrlCmdSetRgFliplockProp_FNPTR(pDispObject) pDispObject->__dispobjCtrlCmdSetRgFliplockProp__
#define dispobjCtrlCmdSetRgFliplockProp(pDispObject, pParams) dispobjCtrlCmdSetRgFliplockProp_DISPATCH(pDispObject, pParams)
#define dispobjCtrlCmdGetRgConnectedLockpinStateless_FNPTR(pDispObject) pDispObject->__dispobjCtrlCmdGetRgConnectedLockpinStateless__
#define dispobjCtrlCmdGetRgConnectedLockpinStateless(pDispObject, pParams) dispobjCtrlCmdGetRgConnectedLockpinStateless_DISPATCH(pDispObject, pParams)
#define dispobjCtrlCmdGetRgScanLine_FNPTR(pDispObject) pDispObject->__dispobjCtrlCmdGetRgScanLine__
#define dispobjCtrlCmdGetRgScanLine(pDispObject, pParams) dispobjCtrlCmdGetRgScanLine_DISPATCH(pDispObject, pParams)
#define dispobjCtrlCmdGetSorOpMode_FNPTR(pDispObject) pDispObject->__dispobjCtrlCmdGetSorOpMode__
#define dispobjCtrlCmdGetSorOpMode(pDispObject, pParams) dispobjCtrlCmdGetSorOpMode_DISPATCH(pDispObject, pParams)
#define dispobjCtrlCmdSetSorOpMode_FNPTR(pDispObject) pDispObject->__dispobjCtrlCmdSetSorOpMode__
#define dispobjCtrlCmdSetSorOpMode(pDispObject, pParams) dispobjCtrlCmdSetSorOpMode_DISPATCH(pDispObject, pParams)
#define dispobjCtrlCmdSetSorFlushMode_FNPTR(pDispObject) pDispObject->__dispobjCtrlCmdSetSorFlushMode__
#define dispobjCtrlCmdSetSorFlushMode(pDispObject, pParams) dispobjCtrlCmdSetSorFlushMode_DISPATCH(pDispObject, pParams)
#define dispobjCtrlCmdSystemGetCapsV2_FNPTR(pDispObject) pDispObject->__dispobjCtrlCmdSystemGetCapsV2__
#define dispobjCtrlCmdSystemGetCapsV2(pDispObject, pCapsParams) dispobjCtrlCmdSystemGetCapsV2_DISPATCH(pDispObject, pCapsParams)
#define dispobjCtrlCmdEventSetTrigger_FNPTR(pDispObject) pDispObject->__dispobjCtrlCmdEventSetTrigger__
#define dispobjCtrlCmdEventSetTrigger(pDispObject) dispobjCtrlCmdEventSetTrigger_DISPATCH(pDispObject)
#define dispobjControl_FNPTR(pDisplayApi) pDisplayApi->__nvoc_base_DisplayApi.__dispapiControl__
#define dispobjControl(pDisplayApi, pCallContext, pParams) dispobjControl_DISPATCH(pDisplayApi, pCallContext, pParams)
#define dispobjControl_Prologue_FNPTR(pDisplayApi) pDisplayApi->__nvoc_base_DisplayApi.__dispapiControl_Prologue__
#define dispobjControl_Prologue(pDisplayApi, pCallContext, pRsParams) dispobjControl_Prologue_DISPATCH(pDisplayApi, pCallContext, pRsParams)
#define dispobjControl_Epilogue_FNPTR(pDisplayApi) pDisplayApi->__nvoc_base_DisplayApi.__dispapiControl_Epilogue__
#define dispobjControl_Epilogue(pDisplayApi, pCallContext, pRsParams) dispobjControl_Epilogue_DISPATCH(pDisplayApi, pCallContext, pRsParams)
#define dispobjAccessCallback_FNPTR(pResource) pResource->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__rmresAccessCallback__
#define dispobjAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) dispobjAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define dispobjShareCallback_FNPTR(pResource) pResource->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__rmresShareCallback__
#define dispobjShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) dispobjShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define dispobjGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__rmresGetMemInterMapParams__
#define dispobjGetMemInterMapParams(pRmResource, pParams) dispobjGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define dispobjCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__rmresCheckMemInterUnmap__
#define dispobjCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) dispobjCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define dispobjGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__rmresGetMemoryMappingDescriptor__
#define dispobjGetMemoryMappingDescriptor(pRmResource, ppMemDesc) dispobjGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define dispobjControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__rmresControlSerialization_Prologue__
#define dispobjControlSerialization_Prologue(pResource, pCallContext, pParams) dispobjControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define dispobjControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__rmresControlSerialization_Epilogue__
#define dispobjControlSerialization_Epilogue(pResource, pCallContext, pParams) dispobjControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define dispobjCanCopy_FNPTR(pResource) pResource->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__
#define dispobjCanCopy(pResource) dispobjCanCopy_DISPATCH(pResource)
#define dispobjIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsDuplicate__
#define dispobjIsDuplicate(pResource, hMemory, pDuplicate) dispobjIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define dispobjPreDestruct_FNPTR(pResource) pResource->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__resPreDestruct__
#define dispobjPreDestruct(pResource) dispobjPreDestruct_DISPATCH(pResource)
#define dispobjControlFilter_FNPTR(pResource) pResource->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__resControlFilter__
#define dispobjControlFilter(pResource, pCallContext, pParams) dispobjControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define dispobjMap_FNPTR(pResource) pResource->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__resMap__
#define dispobjMap(pResource, pCallContext, pParams, pCpuMapping) dispobjMap_DISPATCH(pResource, pCallContext, pParams, pCpuMapping)
#define dispobjUnmap_FNPTR(pResource) pResource->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmap__
#define dispobjUnmap(pResource, pCallContext, pCpuMapping) dispobjUnmap_DISPATCH(pResource, pCallContext, pCpuMapping)
#define dispobjIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsPartialUnmapSupported__
#define dispobjIsPartialUnmapSupported(pResource) dispobjIsPartialUnmapSupported_DISPATCH(pResource)
#define dispobjMapTo_FNPTR(pResource) pResource->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__resMapTo__
#define dispobjMapTo(pResource, pParams) dispobjMapTo_DISPATCH(pResource, pParams)
#define dispobjUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmapFrom__
#define dispobjUnmapFrom(pResource, pParams) dispobjUnmapFrom_DISPATCH(pResource, pParams)
#define dispobjGetRefCount_FNPTR(pResource) pResource->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__resGetRefCount__
#define dispobjGetRefCount(pResource) dispobjGetRefCount_DISPATCH(pResource)
#define dispobjAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__resAddAdditionalDependants__
#define dispobjAddAdditionalDependants(pClient, pResource, pReference) dispobjAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define dispobjGetNotificationListPtr_FNPTR(pNotifier) pNotifier->__nvoc_base_DisplayApi.__nvoc_base_Notifier.__notifyGetNotificationListPtr__
#define dispobjGetNotificationListPtr(pNotifier) dispobjGetNotificationListPtr_DISPATCH(pNotifier)
#define dispobjGetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_DisplayApi.__nvoc_base_Notifier.__notifyGetNotificationShare__
#define dispobjGetNotificationShare(pNotifier) dispobjGetNotificationShare_DISPATCH(pNotifier)
#define dispobjSetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_DisplayApi.__nvoc_base_Notifier.__notifySetNotificationShare__
#define dispobjSetNotificationShare(pNotifier, pNotifShare) dispobjSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define dispobjUnregisterEvent_FNPTR(pNotifier) pNotifier->__nvoc_base_DisplayApi.__nvoc_base_Notifier.__notifyUnregisterEvent__
#define dispobjUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) dispobjUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define dispobjGetOrAllocNotifShare_FNPTR(pNotifier) pNotifier->__nvoc_base_DisplayApi.__nvoc_base_Notifier.__notifyGetOrAllocNotifShare__
#define dispobjGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) dispobjGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)

// Dispatch functions
static inline NV_STATUS dispobjCtrlCmdGetPinsetCount_DISPATCH(struct DispObject *pDispObject, NV5070_CTRL_GET_PINSET_COUNT_PARAMS *pParams) {
    return pDispObject->__dispobjCtrlCmdGetPinsetCount__(pDispObject, pParams);
}

static inline NV_STATUS dispobjCtrlCmdGetPinsetPeer_DISPATCH(struct DispObject *pDispObject, NV5070_CTRL_GET_PINSET_PEER_PARAMS *pParams) {
    return pDispObject->__dispobjCtrlCmdGetPinsetPeer__(pDispObject, pParams);
}

static inline NV_STATUS dispobjCtrlCmdSetMempoolWARForBlitTearing_DISPATCH(struct DispObject *pDispObject, NV5070_CTRL_SET_MEMPOOL_WAR_FOR_BLIT_TEARING_PARAMS *pParams) {
    return pDispObject->__dispobjCtrlCmdSetMempoolWARForBlitTearing__(pDispObject, pParams);
}

static inline NV_STATUS dispobjCtrlCmdGetPinsetLockpins_DISPATCH(struct DispObject *pDispObject, NV5070_CTRL_GET_PINSET_LOCKPINS_PARAMS *pParams) {
    return pDispObject->__dispobjCtrlCmdGetPinsetLockpins__(pDispObject, pParams);
}

static inline NV_STATUS dispobjCtrlCmdGetFrameLockHeaderLockPins_DISPATCH(struct DispObject *pDispObject, NV5070_CTRL_GET_FRAMELOCK_HEADER_LOCKPINS_PARAMS *pParams) {
    return pDispObject->__dispobjCtrlCmdGetFrameLockHeaderLockPins__(pDispObject, pParams);
}

static inline NV_STATUS dispobjCtrlCmdEventSetMemoryNotifies_DISPATCH(struct DispObject *pDispObject, NV5070_CTRL_EVENT_SET_MEMORY_NOTIFIES_PARAMS *pSetMemoryNotifiesParams) {
    return pDispObject->__dispobjCtrlCmdEventSetMemoryNotifies__(pDispObject, pSetMemoryNotifiesParams);
}

static inline NV_STATUS dispobjCtrlCmdSetRmFreeFlags_DISPATCH(struct DispObject *pDispObject, NV5070_CTRL_SET_RMFREE_FLAGS_PARAMS *pParams) {
    return pDispObject->__dispobjCtrlCmdSetRmFreeFlags__(pDispObject, pParams);
}

static inline NV_STATUS dispobjCtrlCmdIMPSetGetParameter_DISPATCH(struct DispObject *pDispObject, NV5070_CTRL_IMP_SET_GET_PARAMETER_PARAMS *pImpSetGetParams) {
    return pDispObject->__dispobjCtrlCmdIMPSetGetParameter__(pDispObject, pImpSetGetParams);
}

static inline NV_STATUS dispobjCtrlCmdGetRgStatus_DISPATCH(struct DispObject *pDispObject, NV5070_CTRL_CMD_GET_RG_STATUS_PARAMS *pParams) {
    return pDispObject->__dispobjCtrlCmdGetRgStatus__(pDispObject, pParams);
}

static inline NV_STATUS dispobjCtrlCmdGetRgUnderflowProp_DISPATCH(struct DispObject *pDispObject, NV5070_CTRL_CMD_GET_RG_UNDERFLOW_PROP_PARAMS *pParams) {
    return pDispObject->__dispobjCtrlCmdGetRgUnderflowProp__(pDispObject, pParams);
}

static inline NV_STATUS dispobjCtrlCmdSetRgUnderflowProp_DISPATCH(struct DispObject *pDispObject, NV5070_CTRL_CMD_SET_RG_UNDERFLOW_PROP_PARAMS *pParams) {
    return pDispObject->__dispobjCtrlCmdSetRgUnderflowProp__(pDispObject, pParams);
}

static inline NV_STATUS dispobjCtrlCmdGetRgFliplockProp_DISPATCH(struct DispObject *pDispObject, NV5070_CTRL_CMD_GET_RG_FLIPLOCK_PROP_PARAMS *pParams) {
    return pDispObject->__dispobjCtrlCmdGetRgFliplockProp__(pDispObject, pParams);
}

static inline NV_STATUS dispobjCtrlCmdSetRgFliplockProp_DISPATCH(struct DispObject *pDispObject, NV5070_CTRL_CMD_SET_RG_FLIPLOCK_PROP_PARAMS *pParams) {
    return pDispObject->__dispobjCtrlCmdSetRgFliplockProp__(pDispObject, pParams);
}

static inline NV_STATUS dispobjCtrlCmdGetRgConnectedLockpinStateless_DISPATCH(struct DispObject *pDispObject, NV5070_CTRL_GET_RG_CONNECTED_LOCKPIN_STATELESS_PARAMS *pParams) {
    return pDispObject->__dispobjCtrlCmdGetRgConnectedLockpinStateless__(pDispObject, pParams);
}

static inline NV_STATUS dispobjCtrlCmdGetRgScanLine_DISPATCH(struct DispObject *pDispObject, NV5070_CTRL_CMD_GET_RG_SCAN_LINE_PARAMS *pParams) {
    return pDispObject->__dispobjCtrlCmdGetRgScanLine__(pDispObject, pParams);
}

static inline NV_STATUS dispobjCtrlCmdGetSorOpMode_DISPATCH(struct DispObject *pDispObject, NV5070_CTRL_CMD_GET_SOR_OP_MODE_PARAMS *pParams) {
    return pDispObject->__dispobjCtrlCmdGetSorOpMode__(pDispObject, pParams);
}

static inline NV_STATUS dispobjCtrlCmdSetSorOpMode_DISPATCH(struct DispObject *pDispObject, NV5070_CTRL_CMD_SET_SOR_OP_MODE_PARAMS *pParams) {
    return pDispObject->__dispobjCtrlCmdSetSorOpMode__(pDispObject, pParams);
}

static inline NV_STATUS dispobjCtrlCmdSetSorFlushMode_DISPATCH(struct DispObject *pDispObject, NV5070_CTRL_SET_SOR_FLUSH_MODE_PARAMS *pParams) {
    return pDispObject->__dispobjCtrlCmdSetSorFlushMode__(pDispObject, pParams);
}

static inline NV_STATUS dispobjCtrlCmdSystemGetCapsV2_DISPATCH(struct DispObject *pDispObject, NV5070_CTRL_SYSTEM_GET_CAPS_V2_PARAMS *pCapsParams) {
    return pDispObject->__dispobjCtrlCmdSystemGetCapsV2__(pDispObject, pCapsParams);
}

static inline NV_STATUS dispobjCtrlCmdEventSetTrigger_DISPATCH(struct DispObject *pDispObject) {
    return pDispObject->__dispobjCtrlCmdEventSetTrigger__(pDispObject);
}

static inline NV_STATUS dispobjControl_DISPATCH(struct DispObject *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pDisplayApi->__dispobjControl__(pDisplayApi, pCallContext, pParams);
}

static inline NV_STATUS dispobjControl_Prologue_DISPATCH(struct DispObject *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams) {
    return pDisplayApi->__dispobjControl_Prologue__(pDisplayApi, pCallContext, pRsParams);
}

static inline void dispobjControl_Epilogue_DISPATCH(struct DispObject *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams) {
    pDisplayApi->__dispobjControl_Epilogue__(pDisplayApi, pCallContext, pRsParams);
}

static inline NvBool dispobjAccessCallback_DISPATCH(struct DispObject *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__dispobjAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool dispobjShareCallback_DISPATCH(struct DispObject *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__dispobjShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS dispobjGetMemInterMapParams_DISPATCH(struct DispObject *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__dispobjGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS dispobjCheckMemInterUnmap_DISPATCH(struct DispObject *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__dispobjCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS dispobjGetMemoryMappingDescriptor_DISPATCH(struct DispObject *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__dispobjGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS dispobjControlSerialization_Prologue_DISPATCH(struct DispObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__dispobjControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void dispobjControlSerialization_Epilogue_DISPATCH(struct DispObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__dispobjControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool dispobjCanCopy_DISPATCH(struct DispObject *pResource) {
    return pResource->__dispobjCanCopy__(pResource);
}

static inline NV_STATUS dispobjIsDuplicate_DISPATCH(struct DispObject *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__dispobjIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void dispobjPreDestruct_DISPATCH(struct DispObject *pResource) {
    pResource->__dispobjPreDestruct__(pResource);
}

static inline NV_STATUS dispobjControlFilter_DISPATCH(struct DispObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__dispobjControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS dispobjMap_DISPATCH(struct DispObject *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pResource->__dispobjMap__(pResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS dispobjUnmap_DISPATCH(struct DispObject *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pResource->__dispobjUnmap__(pResource, pCallContext, pCpuMapping);
}

static inline NvBool dispobjIsPartialUnmapSupported_DISPATCH(struct DispObject *pResource) {
    return pResource->__dispobjIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS dispobjMapTo_DISPATCH(struct DispObject *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__dispobjMapTo__(pResource, pParams);
}

static inline NV_STATUS dispobjUnmapFrom_DISPATCH(struct DispObject *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__dispobjUnmapFrom__(pResource, pParams);
}

static inline NvU32 dispobjGetRefCount_DISPATCH(struct DispObject *pResource) {
    return pResource->__dispobjGetRefCount__(pResource);
}

static inline void dispobjAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct DispObject *pResource, RsResourceRef *pReference) {
    pResource->__dispobjAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline PEVENTNOTIFICATION * dispobjGetNotificationListPtr_DISPATCH(struct DispObject *pNotifier) {
    return pNotifier->__dispobjGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare * dispobjGetNotificationShare_DISPATCH(struct DispObject *pNotifier) {
    return pNotifier->__dispobjGetNotificationShare__(pNotifier);
}

static inline void dispobjSetNotificationShare_DISPATCH(struct DispObject *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__dispobjSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NV_STATUS dispobjUnregisterEvent_DISPATCH(struct DispObject *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__dispobjUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS dispobjGetOrAllocNotifShare_DISPATCH(struct DispObject *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__dispobjGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

NV_STATUS dispobjConstructHal_IMPL(struct DispObject *pDispObject, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);


#ifdef __nvoc_disp_objs_h_disabled
static inline NV_STATUS dispobjConstructHal(struct DispObject *pDispObject, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {
    NV_ASSERT_FAILED_PRECOMP("DispObject was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_disp_objs_h_disabled
#define dispobjConstructHal(pDispObject, pCallContext, pParams) dispobjConstructHal_IMPL(pDispObject, pCallContext, pParams)
#endif //__nvoc_disp_objs_h_disabled

#define dispobjConstructHal_HAL(pDispObject, pCallContext, pParams) dispobjConstructHal(pDispObject, pCallContext, pParams)

NV_STATUS dispobjCtrlCmdGetPinsetCount_IMPL(struct DispObject *pDispObject, NV5070_CTRL_GET_PINSET_COUNT_PARAMS *pParams);

NV_STATUS dispobjCtrlCmdGetPinsetPeer_IMPL(struct DispObject *pDispObject, NV5070_CTRL_GET_PINSET_PEER_PARAMS *pParams);

NV_STATUS dispobjCtrlCmdSetMempoolWARForBlitTearing_IMPL(struct DispObject *pDispObject, NV5070_CTRL_SET_MEMPOOL_WAR_FOR_BLIT_TEARING_PARAMS *pParams);

NV_STATUS dispobjCtrlCmdGetPinsetLockpins_IMPL(struct DispObject *pDispObject, NV5070_CTRL_GET_PINSET_LOCKPINS_PARAMS *pParams);

NV_STATUS dispobjCtrlCmdGetFrameLockHeaderLockPins_IMPL(struct DispObject *pDispObject, NV5070_CTRL_GET_FRAMELOCK_HEADER_LOCKPINS_PARAMS *pParams);

NV_STATUS dispobjCtrlCmdEventSetMemoryNotifies_IMPL(struct DispObject *pDispObject, NV5070_CTRL_EVENT_SET_MEMORY_NOTIFIES_PARAMS *pSetMemoryNotifiesParams);

NV_STATUS dispobjCtrlCmdSetRmFreeFlags_IMPL(struct DispObject *pDispObject, NV5070_CTRL_SET_RMFREE_FLAGS_PARAMS *pParams);

NV_STATUS dispobjCtrlCmdIMPSetGetParameter_IMPL(struct DispObject *pDispObject, NV5070_CTRL_IMP_SET_GET_PARAMETER_PARAMS *pImpSetGetParams);

NV_STATUS dispobjCtrlCmdGetRgStatus_IMPL(struct DispObject *pDispObject, NV5070_CTRL_CMD_GET_RG_STATUS_PARAMS *pParams);

NV_STATUS dispobjCtrlCmdGetRgUnderflowProp_IMPL(struct DispObject *pDispObject, NV5070_CTRL_CMD_GET_RG_UNDERFLOW_PROP_PARAMS *pParams);

NV_STATUS dispobjCtrlCmdSetRgUnderflowProp_IMPL(struct DispObject *pDispObject, NV5070_CTRL_CMD_SET_RG_UNDERFLOW_PROP_PARAMS *pParams);

NV_STATUS dispobjCtrlCmdGetRgFliplockProp_IMPL(struct DispObject *pDispObject, NV5070_CTRL_CMD_GET_RG_FLIPLOCK_PROP_PARAMS *pParams);

NV_STATUS dispobjCtrlCmdSetRgFliplockProp_IMPL(struct DispObject *pDispObject, NV5070_CTRL_CMD_SET_RG_FLIPLOCK_PROP_PARAMS *pParams);

NV_STATUS dispobjCtrlCmdGetRgConnectedLockpinStateless_IMPL(struct DispObject *pDispObject, NV5070_CTRL_GET_RG_CONNECTED_LOCKPIN_STATELESS_PARAMS *pParams);

NV_STATUS dispobjCtrlCmdGetRgScanLine_IMPL(struct DispObject *pDispObject, NV5070_CTRL_CMD_GET_RG_SCAN_LINE_PARAMS *pParams);

NV_STATUS dispobjCtrlCmdGetSorOpMode_IMPL(struct DispObject *pDispObject, NV5070_CTRL_CMD_GET_SOR_OP_MODE_PARAMS *pParams);

NV_STATUS dispobjCtrlCmdSetSorOpMode_IMPL(struct DispObject *pDispObject, NV5070_CTRL_CMD_SET_SOR_OP_MODE_PARAMS *pParams);

NV_STATUS dispobjCtrlCmdSetSorFlushMode_IMPL(struct DispObject *pDispObject, NV5070_CTRL_SET_SOR_FLUSH_MODE_PARAMS *pParams);

NV_STATUS dispobjCtrlCmdSystemGetCapsV2_IMPL(struct DispObject *pDispObject, NV5070_CTRL_SYSTEM_GET_CAPS_V2_PARAMS *pCapsParams);

NV_STATUS dispobjCtrlCmdEventSetTrigger_IMPL(struct DispObject *pDispObject);

NV_STATUS dispobjConstruct_IMPL(struct DispObject *arg_pDispObject, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_dispobjConstruct(arg_pDispObject, arg_pCallContext, arg_pParams) dispobjConstruct_IMPL(arg_pDispObject, arg_pCallContext, arg_pParams)
NV_STATUS dispobjGetByHandle_IMPL(struct RsClient *pClient, NvHandle hDispObject, struct DispObject **ppDispObject);

#define dispobjGetByHandle(pClient, hDispObject, ppDispObject) dispobjGetByHandle_IMPL(pClient, hDispObject, ppDispObject)
NV_STATUS dispobjGetByDevice_IMPL(struct RsClient *pClient, struct Device *pDevice, struct DispObject **ppDispObject);

#define dispobjGetByDevice(pClient, pDevice, ppDispObject) dispobjGetByDevice_IMPL(pClient, pDevice, ppDispObject)
void dispobjClearRmFreeFlags_IMPL(struct DispObject *pDispObject);

#ifdef __nvoc_disp_objs_h_disabled
static inline void dispobjClearRmFreeFlags(struct DispObject *pDispObject) {
    NV_ASSERT_FAILED_PRECOMP("DispObject was disabled!");
}
#else //__nvoc_disp_objs_h_disabled
#define dispobjClearRmFreeFlags(pDispObject) dispobjClearRmFreeFlags_IMPL(pDispObject)
#endif //__nvoc_disp_objs_h_disabled

NvBool dispobjGetRmFreeFlags_IMPL(struct DispObject *pDispObject);

#ifdef __nvoc_disp_objs_h_disabled
static inline NvBool dispobjGetRmFreeFlags(struct DispObject *pDispObject) {
    NV_ASSERT_FAILED_PRECOMP("DispObject was disabled!");
    return NV_FALSE;
}
#else //__nvoc_disp_objs_h_disabled
#define dispobjGetRmFreeFlags(pDispObject) dispobjGetRmFreeFlags_IMPL(pDispObject)
#endif //__nvoc_disp_objs_h_disabled

#undef PRIVATE_FIELD


/*!
 * RM internal class representing NvDisp's XXX_DISPLAY (C370, C570...etc). Parent for
 * all other display resources (channels, etc). Allocated under a device or subdevice.
 *
 * Only one instance of this class is allowed per-GPU. Multi-instance restrictions
 * are enforced by resource_list.h
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_DISP_OBJS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct NvDispApi {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct DispObject __nvoc_base_DispObject;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct RsResource *__nvoc_pbase_RsResource;    // res super^4
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^4
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super^3
    struct INotifier *__nvoc_pbase_INotifier;    // inotify super^4
    struct Notifier *__nvoc_pbase_Notifier;    // notify super^3
    struct DisplayApi *__nvoc_pbase_DisplayApi;    // dispapi super^2
    struct DispObject *__nvoc_pbase_DispObject;    // dispobj super
    struct NvDispApi *__nvoc_pbase_NvDispApi;    // nvdispapi

    // Vtable with 33 per-object function pointers
    NV_STATUS (*__nvdispapiCtrlCmdIdleChannel__)(struct NvDispApi * /*this*/, NVC370_CTRL_IDLE_CHANNEL_PARAMS *);  // exported (id=0xc3700101)
    NV_STATUS (*__nvdispapiCtrlCmdSetAccl__)(struct NvDispApi * /*this*/, NVC370_CTRL_SET_ACCL_PARAMS *);  // exported (id=0xc3700102)
    NV_STATUS (*__nvdispapiCtrlCmdGetAccl__)(struct NvDispApi * /*this*/, NVC370_CTRL_GET_ACCL_PARAMS *);  // exported (id=0xc3700103)
    NV_STATUS (*__nvdispapiCtrlCmdGetChannelInfo__)(struct NvDispApi * /*this*/, NVC370_CTRL_CMD_GET_CHANNEL_INFO_PARAMS *);  // exported (id=0xc3700104)
    NV_STATUS (*__nvdispapiCtrlCmdSetSwaprdyGpioWar__)(struct NvDispApi * /*this*/, NVC370_CTRL_SET_SWAPRDY_GPIO_WAR_PARAMS *);  // exported (id=0xc3700202)
    NV_STATUS (*__nvdispapiCtrlCmdGetLockpinsCaps__)(struct NvDispApi * /*this*/, NVC370_CTRL_GET_LOCKPINS_CAPS_PARAMS *);  // exported (id=0xc3700201)
    NV_STATUS (*__nvdispapiCtrlCmdSetForceModeswitchFlagsOverrides__)(struct NvDispApi * /*this*/, NVC370_CTRL_CMD_SET_FORCE_MODESWITCH_FLAGS_OVERRIDES_PARAMS *);  // exported (id=0xc3700602)
    NV_STATUS (*__nvdispapiControl__)(struct NvDispApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (dispapi) base (dispobj)
    NV_STATUS (*__nvdispapiControl_Prologue__)(struct NvDispApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (dispapi) base (dispobj)
    void (*__nvdispapiControl_Epilogue__)(struct NvDispApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (dispapi) base (dispobj)
    NvBool (*__nvdispapiAccessCallback__)(struct NvDispApi * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (dispobj)
    NvBool (*__nvdispapiShareCallback__)(struct NvDispApi * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (rmres) base (dispobj)
    NV_STATUS (*__nvdispapiGetMemInterMapParams__)(struct NvDispApi * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (dispobj)
    NV_STATUS (*__nvdispapiCheckMemInterUnmap__)(struct NvDispApi * /*this*/, NvBool);  // virtual inherited (rmres) base (dispobj)
    NV_STATUS (*__nvdispapiGetMemoryMappingDescriptor__)(struct NvDispApi * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (dispobj)
    NV_STATUS (*__nvdispapiControlSerialization_Prologue__)(struct NvDispApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (dispobj)
    void (*__nvdispapiControlSerialization_Epilogue__)(struct NvDispApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (dispobj)
    NvBool (*__nvdispapiCanCopy__)(struct NvDispApi * /*this*/);  // virtual inherited (res) base (dispobj)
    NV_STATUS (*__nvdispapiIsDuplicate__)(struct NvDispApi * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (dispobj)
    void (*__nvdispapiPreDestruct__)(struct NvDispApi * /*this*/);  // virtual inherited (res) base (dispobj)
    NV_STATUS (*__nvdispapiControlFilter__)(struct NvDispApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (dispobj)
    NV_STATUS (*__nvdispapiMap__)(struct NvDispApi * /*this*/, struct CALL_CONTEXT *, RS_CPU_MAP_PARAMS *, RsCpuMapping *);  // virtual inherited (res) base (dispobj)
    NV_STATUS (*__nvdispapiUnmap__)(struct NvDispApi * /*this*/, struct CALL_CONTEXT *, RsCpuMapping *);  // virtual inherited (res) base (dispobj)
    NvBool (*__nvdispapiIsPartialUnmapSupported__)(struct NvDispApi * /*this*/);  // inline virtual inherited (res) base (dispobj) body
    NV_STATUS (*__nvdispapiMapTo__)(struct NvDispApi * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (dispobj)
    NV_STATUS (*__nvdispapiUnmapFrom__)(struct NvDispApi * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (dispobj)
    NvU32 (*__nvdispapiGetRefCount__)(struct NvDispApi * /*this*/);  // virtual inherited (res) base (dispobj)
    void (*__nvdispapiAddAdditionalDependants__)(struct RsClient *, struct NvDispApi * /*this*/, RsResourceRef *);  // virtual inherited (res) base (dispobj)
    PEVENTNOTIFICATION * (*__nvdispapiGetNotificationListPtr__)(struct NvDispApi * /*this*/);  // virtual inherited (notify) base (dispobj)
    struct NotifShare * (*__nvdispapiGetNotificationShare__)(struct NvDispApi * /*this*/);  // virtual inherited (notify) base (dispobj)
    void (*__nvdispapiSetNotificationShare__)(struct NvDispApi * /*this*/, struct NotifShare *);  // virtual inherited (notify) base (dispobj)
    NV_STATUS (*__nvdispapiUnregisterEvent__)(struct NvDispApi * /*this*/, NvHandle, NvHandle, NvHandle, NvHandle);  // virtual inherited (notify) base (dispobj)
    NV_STATUS (*__nvdispapiGetOrAllocNotifShare__)(struct NvDispApi * /*this*/, NvHandle, NvHandle, struct NotifShare **);  // virtual inherited (notify) base (dispobj)
};

#ifndef __NVOC_CLASS_NvDispApi_TYPEDEF__
#define __NVOC_CLASS_NvDispApi_TYPEDEF__
typedef struct NvDispApi NvDispApi;
#endif /* __NVOC_CLASS_NvDispApi_TYPEDEF__ */

#ifndef __nvoc_class_id_NvDispApi
#define __nvoc_class_id_NvDispApi 0x36aa0b
#endif /* __nvoc_class_id_NvDispApi */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvDispApi;

#define __staticCast_NvDispApi(pThis) \
    ((pThis)->__nvoc_pbase_NvDispApi)

#ifdef __nvoc_disp_objs_h_disabled
#define __dynamicCast_NvDispApi(pThis) ((NvDispApi*)NULL)
#else //__nvoc_disp_objs_h_disabled
#define __dynamicCast_NvDispApi(pThis) \
    ((NvDispApi*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(NvDispApi)))
#endif //__nvoc_disp_objs_h_disabled

NV_STATUS __nvoc_objCreateDynamic_NvDispApi(NvDispApi**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_NvDispApi(NvDispApi**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_NvDispApi(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_NvDispApi((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define nvdispapiCtrlCmdIdleChannel_FNPTR(pNvDispApi) pNvDispApi->__nvdispapiCtrlCmdIdleChannel__
#define nvdispapiCtrlCmdIdleChannel(pNvDispApi, pParams) nvdispapiCtrlCmdIdleChannel_DISPATCH(pNvDispApi, pParams)
#define nvdispapiCtrlCmdSetAccl_FNPTR(pNvDispApi) pNvDispApi->__nvdispapiCtrlCmdSetAccl__
#define nvdispapiCtrlCmdSetAccl(pNvDispApi, pParams) nvdispapiCtrlCmdSetAccl_DISPATCH(pNvDispApi, pParams)
#define nvdispapiCtrlCmdGetAccl_FNPTR(pNvDispApi) pNvDispApi->__nvdispapiCtrlCmdGetAccl__
#define nvdispapiCtrlCmdGetAccl(pNvDispApi, pParams) nvdispapiCtrlCmdGetAccl_DISPATCH(pNvDispApi, pParams)
#define nvdispapiCtrlCmdGetChannelInfo_FNPTR(pNvDispApi) pNvDispApi->__nvdispapiCtrlCmdGetChannelInfo__
#define nvdispapiCtrlCmdGetChannelInfo(pNvDispApi, pParams) nvdispapiCtrlCmdGetChannelInfo_DISPATCH(pNvDispApi, pParams)
#define nvdispapiCtrlCmdSetSwaprdyGpioWar_FNPTR(pNvDispApi) pNvDispApi->__nvdispapiCtrlCmdSetSwaprdyGpioWar__
#define nvdispapiCtrlCmdSetSwaprdyGpioWar(pNvDispApi, pParams) nvdispapiCtrlCmdSetSwaprdyGpioWar_DISPATCH(pNvDispApi, pParams)
#define nvdispapiCtrlCmdGetLockpinsCaps_FNPTR(pNvDispApi) pNvDispApi->__nvdispapiCtrlCmdGetLockpinsCaps__
#define nvdispapiCtrlCmdGetLockpinsCaps(pNvDispApi, pParams) nvdispapiCtrlCmdGetLockpinsCaps_DISPATCH(pNvDispApi, pParams)
#define nvdispapiCtrlCmdSetForceModeswitchFlagsOverrides_FNPTR(pNvDispApi) pNvDispApi->__nvdispapiCtrlCmdSetForceModeswitchFlagsOverrides__
#define nvdispapiCtrlCmdSetForceModeswitchFlagsOverrides(pNvDispApi, pParams) nvdispapiCtrlCmdSetForceModeswitchFlagsOverrides_DISPATCH(pNvDispApi, pParams)
#define nvdispapiControl_FNPTR(pDisplayApi) pDisplayApi->__nvoc_base_DispObject.__nvoc_base_DisplayApi.__dispapiControl__
#define nvdispapiControl(pDisplayApi, pCallContext, pParams) nvdispapiControl_DISPATCH(pDisplayApi, pCallContext, pParams)
#define nvdispapiControl_Prologue_FNPTR(pDisplayApi) pDisplayApi->__nvoc_base_DispObject.__nvoc_base_DisplayApi.__dispapiControl_Prologue__
#define nvdispapiControl_Prologue(pDisplayApi, pCallContext, pRsParams) nvdispapiControl_Prologue_DISPATCH(pDisplayApi, pCallContext, pRsParams)
#define nvdispapiControl_Epilogue_FNPTR(pDisplayApi) pDisplayApi->__nvoc_base_DispObject.__nvoc_base_DisplayApi.__dispapiControl_Epilogue__
#define nvdispapiControl_Epilogue(pDisplayApi, pCallContext, pRsParams) nvdispapiControl_Epilogue_DISPATCH(pDisplayApi, pCallContext, pRsParams)
#define nvdispapiAccessCallback_FNPTR(pResource) pResource->__nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource.__rmresAccessCallback__
#define nvdispapiAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) nvdispapiAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define nvdispapiShareCallback_FNPTR(pResource) pResource->__nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource.__rmresShareCallback__
#define nvdispapiShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) nvdispapiShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define nvdispapiGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource.__rmresGetMemInterMapParams__
#define nvdispapiGetMemInterMapParams(pRmResource, pParams) nvdispapiGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define nvdispapiCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource.__rmresCheckMemInterUnmap__
#define nvdispapiCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) nvdispapiCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define nvdispapiGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource.__rmresGetMemoryMappingDescriptor__
#define nvdispapiGetMemoryMappingDescriptor(pRmResource, ppMemDesc) nvdispapiGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define nvdispapiControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource.__rmresControlSerialization_Prologue__
#define nvdispapiControlSerialization_Prologue(pResource, pCallContext, pParams) nvdispapiControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define nvdispapiControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource.__rmresControlSerialization_Epilogue__
#define nvdispapiControlSerialization_Epilogue(pResource, pCallContext, pParams) nvdispapiControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define nvdispapiCanCopy_FNPTR(pResource) pResource->__nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__
#define nvdispapiCanCopy(pResource) nvdispapiCanCopy_DISPATCH(pResource)
#define nvdispapiIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsDuplicate__
#define nvdispapiIsDuplicate(pResource, hMemory, pDuplicate) nvdispapiIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define nvdispapiPreDestruct_FNPTR(pResource) pResource->__nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__resPreDestruct__
#define nvdispapiPreDestruct(pResource) nvdispapiPreDestruct_DISPATCH(pResource)
#define nvdispapiControlFilter_FNPTR(pResource) pResource->__nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__resControlFilter__
#define nvdispapiControlFilter(pResource, pCallContext, pParams) nvdispapiControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define nvdispapiMap_FNPTR(pResource) pResource->__nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__resMap__
#define nvdispapiMap(pResource, pCallContext, pParams, pCpuMapping) nvdispapiMap_DISPATCH(pResource, pCallContext, pParams, pCpuMapping)
#define nvdispapiUnmap_FNPTR(pResource) pResource->__nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmap__
#define nvdispapiUnmap(pResource, pCallContext, pCpuMapping) nvdispapiUnmap_DISPATCH(pResource, pCallContext, pCpuMapping)
#define nvdispapiIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsPartialUnmapSupported__
#define nvdispapiIsPartialUnmapSupported(pResource) nvdispapiIsPartialUnmapSupported_DISPATCH(pResource)
#define nvdispapiMapTo_FNPTR(pResource) pResource->__nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__resMapTo__
#define nvdispapiMapTo(pResource, pParams) nvdispapiMapTo_DISPATCH(pResource, pParams)
#define nvdispapiUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmapFrom__
#define nvdispapiUnmapFrom(pResource, pParams) nvdispapiUnmapFrom_DISPATCH(pResource, pParams)
#define nvdispapiGetRefCount_FNPTR(pResource) pResource->__nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__resGetRefCount__
#define nvdispapiGetRefCount(pResource) nvdispapiGetRefCount_DISPATCH(pResource)
#define nvdispapiAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__resAddAdditionalDependants__
#define nvdispapiAddAdditionalDependants(pClient, pResource, pReference) nvdispapiAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define nvdispapiGetNotificationListPtr_FNPTR(pNotifier) pNotifier->__nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_Notifier.__notifyGetNotificationListPtr__
#define nvdispapiGetNotificationListPtr(pNotifier) nvdispapiGetNotificationListPtr_DISPATCH(pNotifier)
#define nvdispapiGetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_Notifier.__notifyGetNotificationShare__
#define nvdispapiGetNotificationShare(pNotifier) nvdispapiGetNotificationShare_DISPATCH(pNotifier)
#define nvdispapiSetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_Notifier.__notifySetNotificationShare__
#define nvdispapiSetNotificationShare(pNotifier, pNotifShare) nvdispapiSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define nvdispapiUnregisterEvent_FNPTR(pNotifier) pNotifier->__nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_Notifier.__notifyUnregisterEvent__
#define nvdispapiUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) nvdispapiUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define nvdispapiGetOrAllocNotifShare_FNPTR(pNotifier) pNotifier->__nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_Notifier.__notifyGetOrAllocNotifShare__
#define nvdispapiGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) nvdispapiGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)

// Dispatch functions
static inline NV_STATUS nvdispapiCtrlCmdIdleChannel_DISPATCH(struct NvDispApi *pNvDispApi, NVC370_CTRL_IDLE_CHANNEL_PARAMS *pParams) {
    return pNvDispApi->__nvdispapiCtrlCmdIdleChannel__(pNvDispApi, pParams);
}

static inline NV_STATUS nvdispapiCtrlCmdSetAccl_DISPATCH(struct NvDispApi *pNvDispApi, NVC370_CTRL_SET_ACCL_PARAMS *pParams) {
    return pNvDispApi->__nvdispapiCtrlCmdSetAccl__(pNvDispApi, pParams);
}

static inline NV_STATUS nvdispapiCtrlCmdGetAccl_DISPATCH(struct NvDispApi *pNvDispApi, NVC370_CTRL_GET_ACCL_PARAMS *pParams) {
    return pNvDispApi->__nvdispapiCtrlCmdGetAccl__(pNvDispApi, pParams);
}

static inline NV_STATUS nvdispapiCtrlCmdGetChannelInfo_DISPATCH(struct NvDispApi *pNvDispApi, NVC370_CTRL_CMD_GET_CHANNEL_INFO_PARAMS *pParams) {
    return pNvDispApi->__nvdispapiCtrlCmdGetChannelInfo__(pNvDispApi, pParams);
}

static inline NV_STATUS nvdispapiCtrlCmdSetSwaprdyGpioWar_DISPATCH(struct NvDispApi *pNvDispApi, NVC370_CTRL_SET_SWAPRDY_GPIO_WAR_PARAMS *pParams) {
    return pNvDispApi->__nvdispapiCtrlCmdSetSwaprdyGpioWar__(pNvDispApi, pParams);
}

static inline NV_STATUS nvdispapiCtrlCmdGetLockpinsCaps_DISPATCH(struct NvDispApi *pNvDispApi, NVC370_CTRL_GET_LOCKPINS_CAPS_PARAMS *pParams) {
    return pNvDispApi->__nvdispapiCtrlCmdGetLockpinsCaps__(pNvDispApi, pParams);
}

static inline NV_STATUS nvdispapiCtrlCmdSetForceModeswitchFlagsOverrides_DISPATCH(struct NvDispApi *pNvDispApi, NVC370_CTRL_CMD_SET_FORCE_MODESWITCH_FLAGS_OVERRIDES_PARAMS *pParams) {
    return pNvDispApi->__nvdispapiCtrlCmdSetForceModeswitchFlagsOverrides__(pNvDispApi, pParams);
}

static inline NV_STATUS nvdispapiControl_DISPATCH(struct NvDispApi *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pDisplayApi->__nvdispapiControl__(pDisplayApi, pCallContext, pParams);
}

static inline NV_STATUS nvdispapiControl_Prologue_DISPATCH(struct NvDispApi *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams) {
    return pDisplayApi->__nvdispapiControl_Prologue__(pDisplayApi, pCallContext, pRsParams);
}

static inline void nvdispapiControl_Epilogue_DISPATCH(struct NvDispApi *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams) {
    pDisplayApi->__nvdispapiControl_Epilogue__(pDisplayApi, pCallContext, pRsParams);
}

static inline NvBool nvdispapiAccessCallback_DISPATCH(struct NvDispApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvdispapiAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool nvdispapiShareCallback_DISPATCH(struct NvDispApi *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__nvdispapiShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS nvdispapiGetMemInterMapParams_DISPATCH(struct NvDispApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvdispapiGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS nvdispapiCheckMemInterUnmap_DISPATCH(struct NvDispApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvdispapiCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS nvdispapiGetMemoryMappingDescriptor_DISPATCH(struct NvDispApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvdispapiGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS nvdispapiControlSerialization_Prologue_DISPATCH(struct NvDispApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvdispapiControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void nvdispapiControlSerialization_Epilogue_DISPATCH(struct NvDispApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvdispapiControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool nvdispapiCanCopy_DISPATCH(struct NvDispApi *pResource) {
    return pResource->__nvdispapiCanCopy__(pResource);
}

static inline NV_STATUS nvdispapiIsDuplicate_DISPATCH(struct NvDispApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvdispapiIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void nvdispapiPreDestruct_DISPATCH(struct NvDispApi *pResource) {
    pResource->__nvdispapiPreDestruct__(pResource);
}

static inline NV_STATUS nvdispapiControlFilter_DISPATCH(struct NvDispApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvdispapiControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS nvdispapiMap_DISPATCH(struct NvDispApi *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pResource->__nvdispapiMap__(pResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS nvdispapiUnmap_DISPATCH(struct NvDispApi *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pResource->__nvdispapiUnmap__(pResource, pCallContext, pCpuMapping);
}

static inline NvBool nvdispapiIsPartialUnmapSupported_DISPATCH(struct NvDispApi *pResource) {
    return pResource->__nvdispapiIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS nvdispapiMapTo_DISPATCH(struct NvDispApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvdispapiMapTo__(pResource, pParams);
}

static inline NV_STATUS nvdispapiUnmapFrom_DISPATCH(struct NvDispApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvdispapiUnmapFrom__(pResource, pParams);
}

static inline NvU32 nvdispapiGetRefCount_DISPATCH(struct NvDispApi *pResource) {
    return pResource->__nvdispapiGetRefCount__(pResource);
}

static inline void nvdispapiAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct NvDispApi *pResource, RsResourceRef *pReference) {
    pResource->__nvdispapiAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline PEVENTNOTIFICATION * nvdispapiGetNotificationListPtr_DISPATCH(struct NvDispApi *pNotifier) {
    return pNotifier->__nvdispapiGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare * nvdispapiGetNotificationShare_DISPATCH(struct NvDispApi *pNotifier) {
    return pNotifier->__nvdispapiGetNotificationShare__(pNotifier);
}

static inline void nvdispapiSetNotificationShare_DISPATCH(struct NvDispApi *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__nvdispapiSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NV_STATUS nvdispapiUnregisterEvent_DISPATCH(struct NvDispApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__nvdispapiUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS nvdispapiGetOrAllocNotifShare_DISPATCH(struct NvDispApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__nvdispapiGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

NV_STATUS nvdispapiCtrlCmdIdleChannel_IMPL(struct NvDispApi *pNvDispApi, NVC370_CTRL_IDLE_CHANNEL_PARAMS *pParams);

NV_STATUS nvdispapiCtrlCmdSetAccl_IMPL(struct NvDispApi *pNvDispApi, NVC370_CTRL_SET_ACCL_PARAMS *pParams);

NV_STATUS nvdispapiCtrlCmdGetAccl_IMPL(struct NvDispApi *pNvDispApi, NVC370_CTRL_GET_ACCL_PARAMS *pParams);

NV_STATUS nvdispapiCtrlCmdGetChannelInfo_IMPL(struct NvDispApi *pNvDispApi, NVC370_CTRL_CMD_GET_CHANNEL_INFO_PARAMS *pParams);

NV_STATUS nvdispapiCtrlCmdSetSwaprdyGpioWar_IMPL(struct NvDispApi *pNvDispApi, NVC370_CTRL_SET_SWAPRDY_GPIO_WAR_PARAMS *pParams);

NV_STATUS nvdispapiCtrlCmdGetLockpinsCaps_IMPL(struct NvDispApi *pNvDispApi, NVC370_CTRL_GET_LOCKPINS_CAPS_PARAMS *pParams);

NV_STATUS nvdispapiCtrlCmdSetForceModeswitchFlagsOverrides_IMPL(struct NvDispApi *pNvDispApi, NVC370_CTRL_CMD_SET_FORCE_MODESWITCH_FLAGS_OVERRIDES_PARAMS *pParams);

NV_STATUS nvdispapiConstruct_IMPL(struct NvDispApi *arg_pNvdispApi, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_nvdispapiConstruct(arg_pNvdispApi, arg_pCallContext, arg_pParams) nvdispapiConstruct_IMPL(arg_pNvdispApi, arg_pCallContext, arg_pParams)
#undef PRIVATE_FIELD


/*!
 * RM internal class representing XXX_DISPLAY_SW
 *
 * With NvDisplay, we have divided classes into HW & SW classes. HW class provides
 * interface for register/methods. SW class provides rmctrls. Clients can use
 * multiple SW classes on a chip, but only one HW class. NVC372_DISPLAY_SW is SW
 * class of NvDisplay family chips.
 *
 * Multi-instance restrictions are enforced by resource_list.h
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_DISP_OBJS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct DispSwObj {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct DisplayApi __nvoc_base_DisplayApi;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^4
    struct RsResource *__nvoc_pbase_RsResource;    // res super^3
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^3
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super^2
    struct INotifier *__nvoc_pbase_INotifier;    // inotify super^3
    struct Notifier *__nvoc_pbase_Notifier;    // notify super^2
    struct DisplayApi *__nvoc_pbase_DisplayApi;    // dispapi super
    struct DispSwObj *__nvoc_pbase_DispSwObj;    // dispswobj

    // Vtable with 30 per-object function pointers
    NV_STATUS (*__dispswobjCtrlCmdIsModePossible__)(struct DispSwObj * /*this*/, NVC372_CTRL_IS_MODE_POSSIBLE_PARAMS *);  // exported (id=0xc3720101)
    NV_STATUS (*__dispswobjCtrlCmdIsModePossibleOrSettings__)(struct DispSwObj * /*this*/, NVC372_CTRL_IS_MODE_POSSIBLE_OR_SETTINGS_PARAMS *);  // exported (id=0xc3720102)
    NV_STATUS (*__dispswobjCtrlCmdVideoAdaptiveRefreshRate__)(struct DispSwObj * /*this*/, NVC372_CTRL_CMD_VIDEO_ADAPTIVE_REFRESH_RATE_PARAMS *);  // exported (id=0xc3720103)
    NV_STATUS (*__dispswobjCtrlCmdGetActiveViewportPointIn__)(struct DispSwObj * /*this*/, NVC372_CTRL_CMD_GET_ACTIVE_VIEWPORT_POINT_IN_PARAMS *);  // exported (id=0xc3720104)
    NV_STATUS (*__dispswobjControl__)(struct DispSwObj * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (dispapi) base (dispapi)
    NV_STATUS (*__dispswobjControl_Prologue__)(struct DispSwObj * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (dispapi) base (dispapi)
    void (*__dispswobjControl_Epilogue__)(struct DispSwObj * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (dispapi) base (dispapi)
    NvBool (*__dispswobjAccessCallback__)(struct DispSwObj * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (dispapi)
    NvBool (*__dispswobjShareCallback__)(struct DispSwObj * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (rmres) base (dispapi)
    NV_STATUS (*__dispswobjGetMemInterMapParams__)(struct DispSwObj * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (dispapi)
    NV_STATUS (*__dispswobjCheckMemInterUnmap__)(struct DispSwObj * /*this*/, NvBool);  // virtual inherited (rmres) base (dispapi)
    NV_STATUS (*__dispswobjGetMemoryMappingDescriptor__)(struct DispSwObj * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (dispapi)
    NV_STATUS (*__dispswobjControlSerialization_Prologue__)(struct DispSwObj * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (dispapi)
    void (*__dispswobjControlSerialization_Epilogue__)(struct DispSwObj * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (dispapi)
    NvBool (*__dispswobjCanCopy__)(struct DispSwObj * /*this*/);  // virtual inherited (res) base (dispapi)
    NV_STATUS (*__dispswobjIsDuplicate__)(struct DispSwObj * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (dispapi)
    void (*__dispswobjPreDestruct__)(struct DispSwObj * /*this*/);  // virtual inherited (res) base (dispapi)
    NV_STATUS (*__dispswobjControlFilter__)(struct DispSwObj * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (dispapi)
    NV_STATUS (*__dispswobjMap__)(struct DispSwObj * /*this*/, struct CALL_CONTEXT *, RS_CPU_MAP_PARAMS *, RsCpuMapping *);  // virtual inherited (res) base (dispapi)
    NV_STATUS (*__dispswobjUnmap__)(struct DispSwObj * /*this*/, struct CALL_CONTEXT *, RsCpuMapping *);  // virtual inherited (res) base (dispapi)
    NvBool (*__dispswobjIsPartialUnmapSupported__)(struct DispSwObj * /*this*/);  // inline virtual inherited (res) base (dispapi) body
    NV_STATUS (*__dispswobjMapTo__)(struct DispSwObj * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (dispapi)
    NV_STATUS (*__dispswobjUnmapFrom__)(struct DispSwObj * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (dispapi)
    NvU32 (*__dispswobjGetRefCount__)(struct DispSwObj * /*this*/);  // virtual inherited (res) base (dispapi)
    void (*__dispswobjAddAdditionalDependants__)(struct RsClient *, struct DispSwObj * /*this*/, RsResourceRef *);  // virtual inherited (res) base (dispapi)
    PEVENTNOTIFICATION * (*__dispswobjGetNotificationListPtr__)(struct DispSwObj * /*this*/);  // virtual inherited (notify) base (dispapi)
    struct NotifShare * (*__dispswobjGetNotificationShare__)(struct DispSwObj * /*this*/);  // virtual inherited (notify) base (dispapi)
    void (*__dispswobjSetNotificationShare__)(struct DispSwObj * /*this*/, struct NotifShare *);  // virtual inherited (notify) base (dispapi)
    NV_STATUS (*__dispswobjUnregisterEvent__)(struct DispSwObj * /*this*/, NvHandle, NvHandle, NvHandle, NvHandle);  // virtual inherited (notify) base (dispapi)
    NV_STATUS (*__dispswobjGetOrAllocNotifShare__)(struct DispSwObj * /*this*/, NvHandle, NvHandle, struct NotifShare **);  // virtual inherited (notify) base (dispapi)
};

#ifndef __NVOC_CLASS_DispSwObj_TYPEDEF__
#define __NVOC_CLASS_DispSwObj_TYPEDEF__
typedef struct DispSwObj DispSwObj;
#endif /* __NVOC_CLASS_DispSwObj_TYPEDEF__ */

#ifndef __nvoc_class_id_DispSwObj
#define __nvoc_class_id_DispSwObj 0x6aa5e2
#endif /* __nvoc_class_id_DispSwObj */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_DispSwObj;

#define __staticCast_DispSwObj(pThis) \
    ((pThis)->__nvoc_pbase_DispSwObj)

#ifdef __nvoc_disp_objs_h_disabled
#define __dynamicCast_DispSwObj(pThis) ((DispSwObj*)NULL)
#else //__nvoc_disp_objs_h_disabled
#define __dynamicCast_DispSwObj(pThis) \
    ((DispSwObj*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(DispSwObj)))
#endif //__nvoc_disp_objs_h_disabled

NV_STATUS __nvoc_objCreateDynamic_DispSwObj(DispSwObj**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_DispSwObj(DispSwObj**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_DispSwObj(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_DispSwObj((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define dispswobjCtrlCmdIsModePossible_FNPTR(pDispSwObj) pDispSwObj->__dispswobjCtrlCmdIsModePossible__
#define dispswobjCtrlCmdIsModePossible(pDispSwObj, pParams) dispswobjCtrlCmdIsModePossible_DISPATCH(pDispSwObj, pParams)
#define dispswobjCtrlCmdIsModePossibleOrSettings_FNPTR(pDispSwObj) pDispSwObj->__dispswobjCtrlCmdIsModePossibleOrSettings__
#define dispswobjCtrlCmdIsModePossibleOrSettings(pDispSwObj, pParams) dispswobjCtrlCmdIsModePossibleOrSettings_DISPATCH(pDispSwObj, pParams)
#define dispswobjCtrlCmdVideoAdaptiveRefreshRate_FNPTR(pDispSwObj) pDispSwObj->__dispswobjCtrlCmdVideoAdaptiveRefreshRate__
#define dispswobjCtrlCmdVideoAdaptiveRefreshRate(pDispSwObj, pParams) dispswobjCtrlCmdVideoAdaptiveRefreshRate_DISPATCH(pDispSwObj, pParams)
#define dispswobjCtrlCmdGetActiveViewportPointIn_FNPTR(pDispSwObj) pDispSwObj->__dispswobjCtrlCmdGetActiveViewportPointIn__
#define dispswobjCtrlCmdGetActiveViewportPointIn(pDispSwObj, pParams) dispswobjCtrlCmdGetActiveViewportPointIn_DISPATCH(pDispSwObj, pParams)
#define dispswobjControl_FNPTR(pDisplayApi) pDisplayApi->__nvoc_base_DisplayApi.__dispapiControl__
#define dispswobjControl(pDisplayApi, pCallContext, pParams) dispswobjControl_DISPATCH(pDisplayApi, pCallContext, pParams)
#define dispswobjControl_Prologue_FNPTR(pDisplayApi) pDisplayApi->__nvoc_base_DisplayApi.__dispapiControl_Prologue__
#define dispswobjControl_Prologue(pDisplayApi, pCallContext, pRsParams) dispswobjControl_Prologue_DISPATCH(pDisplayApi, pCallContext, pRsParams)
#define dispswobjControl_Epilogue_FNPTR(pDisplayApi) pDisplayApi->__nvoc_base_DisplayApi.__dispapiControl_Epilogue__
#define dispswobjControl_Epilogue(pDisplayApi, pCallContext, pRsParams) dispswobjControl_Epilogue_DISPATCH(pDisplayApi, pCallContext, pRsParams)
#define dispswobjAccessCallback_FNPTR(pResource) pResource->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__rmresAccessCallback__
#define dispswobjAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) dispswobjAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define dispswobjShareCallback_FNPTR(pResource) pResource->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__rmresShareCallback__
#define dispswobjShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) dispswobjShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define dispswobjGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__rmresGetMemInterMapParams__
#define dispswobjGetMemInterMapParams(pRmResource, pParams) dispswobjGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define dispswobjCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__rmresCheckMemInterUnmap__
#define dispswobjCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) dispswobjCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define dispswobjGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__rmresGetMemoryMappingDescriptor__
#define dispswobjGetMemoryMappingDescriptor(pRmResource, ppMemDesc) dispswobjGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define dispswobjControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__rmresControlSerialization_Prologue__
#define dispswobjControlSerialization_Prologue(pResource, pCallContext, pParams) dispswobjControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define dispswobjControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__rmresControlSerialization_Epilogue__
#define dispswobjControlSerialization_Epilogue(pResource, pCallContext, pParams) dispswobjControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define dispswobjCanCopy_FNPTR(pResource) pResource->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__
#define dispswobjCanCopy(pResource) dispswobjCanCopy_DISPATCH(pResource)
#define dispswobjIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsDuplicate__
#define dispswobjIsDuplicate(pResource, hMemory, pDuplicate) dispswobjIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define dispswobjPreDestruct_FNPTR(pResource) pResource->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__resPreDestruct__
#define dispswobjPreDestruct(pResource) dispswobjPreDestruct_DISPATCH(pResource)
#define dispswobjControlFilter_FNPTR(pResource) pResource->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__resControlFilter__
#define dispswobjControlFilter(pResource, pCallContext, pParams) dispswobjControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define dispswobjMap_FNPTR(pResource) pResource->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__resMap__
#define dispswobjMap(pResource, pCallContext, pParams, pCpuMapping) dispswobjMap_DISPATCH(pResource, pCallContext, pParams, pCpuMapping)
#define dispswobjUnmap_FNPTR(pResource) pResource->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmap__
#define dispswobjUnmap(pResource, pCallContext, pCpuMapping) dispswobjUnmap_DISPATCH(pResource, pCallContext, pCpuMapping)
#define dispswobjIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsPartialUnmapSupported__
#define dispswobjIsPartialUnmapSupported(pResource) dispswobjIsPartialUnmapSupported_DISPATCH(pResource)
#define dispswobjMapTo_FNPTR(pResource) pResource->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__resMapTo__
#define dispswobjMapTo(pResource, pParams) dispswobjMapTo_DISPATCH(pResource, pParams)
#define dispswobjUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmapFrom__
#define dispswobjUnmapFrom(pResource, pParams) dispswobjUnmapFrom_DISPATCH(pResource, pParams)
#define dispswobjGetRefCount_FNPTR(pResource) pResource->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__resGetRefCount__
#define dispswobjGetRefCount(pResource) dispswobjGetRefCount_DISPATCH(pResource)
#define dispswobjAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__resAddAdditionalDependants__
#define dispswobjAddAdditionalDependants(pClient, pResource, pReference) dispswobjAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define dispswobjGetNotificationListPtr_FNPTR(pNotifier) pNotifier->__nvoc_base_DisplayApi.__nvoc_base_Notifier.__notifyGetNotificationListPtr__
#define dispswobjGetNotificationListPtr(pNotifier) dispswobjGetNotificationListPtr_DISPATCH(pNotifier)
#define dispswobjGetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_DisplayApi.__nvoc_base_Notifier.__notifyGetNotificationShare__
#define dispswobjGetNotificationShare(pNotifier) dispswobjGetNotificationShare_DISPATCH(pNotifier)
#define dispswobjSetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_DisplayApi.__nvoc_base_Notifier.__notifySetNotificationShare__
#define dispswobjSetNotificationShare(pNotifier, pNotifShare) dispswobjSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define dispswobjUnregisterEvent_FNPTR(pNotifier) pNotifier->__nvoc_base_DisplayApi.__nvoc_base_Notifier.__notifyUnregisterEvent__
#define dispswobjUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) dispswobjUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define dispswobjGetOrAllocNotifShare_FNPTR(pNotifier) pNotifier->__nvoc_base_DisplayApi.__nvoc_base_Notifier.__notifyGetOrAllocNotifShare__
#define dispswobjGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) dispswobjGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)

// Dispatch functions
static inline NV_STATUS dispswobjCtrlCmdIsModePossible_DISPATCH(struct DispSwObj *pDispSwObj, NVC372_CTRL_IS_MODE_POSSIBLE_PARAMS *pParams) {
    return pDispSwObj->__dispswobjCtrlCmdIsModePossible__(pDispSwObj, pParams);
}

static inline NV_STATUS dispswobjCtrlCmdIsModePossibleOrSettings_DISPATCH(struct DispSwObj *pDispSwObj, NVC372_CTRL_IS_MODE_POSSIBLE_OR_SETTINGS_PARAMS *pParams) {
    return pDispSwObj->__dispswobjCtrlCmdIsModePossibleOrSettings__(pDispSwObj, pParams);
}

static inline NV_STATUS dispswobjCtrlCmdVideoAdaptiveRefreshRate_DISPATCH(struct DispSwObj *pDispSwObj, NVC372_CTRL_CMD_VIDEO_ADAPTIVE_REFRESH_RATE_PARAMS *pParams) {
    return pDispSwObj->__dispswobjCtrlCmdVideoAdaptiveRefreshRate__(pDispSwObj, pParams);
}

static inline NV_STATUS dispswobjCtrlCmdGetActiveViewportPointIn_DISPATCH(struct DispSwObj *pDispSwObj, NVC372_CTRL_CMD_GET_ACTIVE_VIEWPORT_POINT_IN_PARAMS *pParams) {
    return pDispSwObj->__dispswobjCtrlCmdGetActiveViewportPointIn__(pDispSwObj, pParams);
}

static inline NV_STATUS dispswobjControl_DISPATCH(struct DispSwObj *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pDisplayApi->__dispswobjControl__(pDisplayApi, pCallContext, pParams);
}

static inline NV_STATUS dispswobjControl_Prologue_DISPATCH(struct DispSwObj *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams) {
    return pDisplayApi->__dispswobjControl_Prologue__(pDisplayApi, pCallContext, pRsParams);
}

static inline void dispswobjControl_Epilogue_DISPATCH(struct DispSwObj *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams) {
    pDisplayApi->__dispswobjControl_Epilogue__(pDisplayApi, pCallContext, pRsParams);
}

static inline NvBool dispswobjAccessCallback_DISPATCH(struct DispSwObj *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__dispswobjAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool dispswobjShareCallback_DISPATCH(struct DispSwObj *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__dispswobjShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS dispswobjGetMemInterMapParams_DISPATCH(struct DispSwObj *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__dispswobjGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS dispswobjCheckMemInterUnmap_DISPATCH(struct DispSwObj *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__dispswobjCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS dispswobjGetMemoryMappingDescriptor_DISPATCH(struct DispSwObj *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__dispswobjGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS dispswobjControlSerialization_Prologue_DISPATCH(struct DispSwObj *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__dispswobjControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void dispswobjControlSerialization_Epilogue_DISPATCH(struct DispSwObj *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__dispswobjControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool dispswobjCanCopy_DISPATCH(struct DispSwObj *pResource) {
    return pResource->__dispswobjCanCopy__(pResource);
}

static inline NV_STATUS dispswobjIsDuplicate_DISPATCH(struct DispSwObj *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__dispswobjIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void dispswobjPreDestruct_DISPATCH(struct DispSwObj *pResource) {
    pResource->__dispswobjPreDestruct__(pResource);
}

static inline NV_STATUS dispswobjControlFilter_DISPATCH(struct DispSwObj *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__dispswobjControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS dispswobjMap_DISPATCH(struct DispSwObj *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pResource->__dispswobjMap__(pResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS dispswobjUnmap_DISPATCH(struct DispSwObj *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pResource->__dispswobjUnmap__(pResource, pCallContext, pCpuMapping);
}

static inline NvBool dispswobjIsPartialUnmapSupported_DISPATCH(struct DispSwObj *pResource) {
    return pResource->__dispswobjIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS dispswobjMapTo_DISPATCH(struct DispSwObj *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__dispswobjMapTo__(pResource, pParams);
}

static inline NV_STATUS dispswobjUnmapFrom_DISPATCH(struct DispSwObj *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__dispswobjUnmapFrom__(pResource, pParams);
}

static inline NvU32 dispswobjGetRefCount_DISPATCH(struct DispSwObj *pResource) {
    return pResource->__dispswobjGetRefCount__(pResource);
}

static inline void dispswobjAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct DispSwObj *pResource, RsResourceRef *pReference) {
    pResource->__dispswobjAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline PEVENTNOTIFICATION * dispswobjGetNotificationListPtr_DISPATCH(struct DispSwObj *pNotifier) {
    return pNotifier->__dispswobjGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare * dispswobjGetNotificationShare_DISPATCH(struct DispSwObj *pNotifier) {
    return pNotifier->__dispswobjGetNotificationShare__(pNotifier);
}

static inline void dispswobjSetNotificationShare_DISPATCH(struct DispSwObj *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__dispswobjSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NV_STATUS dispswobjUnregisterEvent_DISPATCH(struct DispSwObj *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__dispswobjUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS dispswobjGetOrAllocNotifShare_DISPATCH(struct DispSwObj *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__dispswobjGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

NV_STATUS dispswobjCtrlCmdIsModePossible_IMPL(struct DispSwObj *pDispSwObj, NVC372_CTRL_IS_MODE_POSSIBLE_PARAMS *pParams);

NV_STATUS dispswobjCtrlCmdIsModePossibleOrSettings_IMPL(struct DispSwObj *pDispSwObj, NVC372_CTRL_IS_MODE_POSSIBLE_OR_SETTINGS_PARAMS *pParams);

NV_STATUS dispswobjCtrlCmdVideoAdaptiveRefreshRate_IMPL(struct DispSwObj *pDispSwObj, NVC372_CTRL_CMD_VIDEO_ADAPTIVE_REFRESH_RATE_PARAMS *pParams);

NV_STATUS dispswobjCtrlCmdGetActiveViewportPointIn_IMPL(struct DispSwObj *pDispSwObj, NVC372_CTRL_CMD_GET_ACTIVE_VIEWPORT_POINT_IN_PARAMS *pParams);

NV_STATUS dispswobjConstruct_IMPL(struct DispSwObj *arg_pDispSwObj, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_dispswobjConstruct(arg_pDispSwObj, arg_pCallContext, arg_pParams) dispswobjConstruct_IMPL(arg_pDispSwObj, arg_pCallContext, arg_pParams)
#undef PRIVATE_FIELD


/*!
 * RM internal class representing XXX_DISPLAY_COMMON (class id: 0x0073)
 *
 * Only one instance of this class is allowed per-GPU. Multi-instance restrictions
 * are enforced by resource_list.h
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_DISP_OBJS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct DispCommon {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct DisplayApi __nvoc_base_DisplayApi;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^4
    struct RsResource *__nvoc_pbase_RsResource;    // res super^3
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^3
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super^2
    struct INotifier *__nvoc_pbase_INotifier;    // inotify super^3
    struct Notifier *__nvoc_pbase_Notifier;    // notify super^2
    struct DisplayApi *__nvoc_pbase_DisplayApi;    // dispapi super
    struct DispCommon *__nvoc_pbase_DispCommon;    // dispcmn

    // Vtable with 156 per-object function pointers
    NV_STATUS (*__dispcmnCtrlCmdSystemGetVblankCounter__)(struct DispCommon * /*this*/, NV0073_CTRL_SYSTEM_GET_VBLANK_COUNTER_PARAMS *);  // exported (id=0x730105)
    NV_STATUS (*__dispcmnCtrlCmdSystemGetVblankEnable__)(struct DispCommon * /*this*/, NV0073_CTRL_SYSTEM_GET_VBLANK_ENABLE_PARAMS *);  // exported (id=0x730106)
    NV_STATUS (*__dispcmnCtrlCmdSystemCheckSidebandSrSupport__)(struct DispCommon * /*this*/, NV0073_CTRL_CMD_SYSTEM_CHECK_SIDEBAND_SR_SUPPORT_PARAMS *);  // exported (id=0x73014c)
    NV_STATUS (*__dispcmnCtrlCmdSystemGetHotplugState__)(struct DispCommon * /*this*/, NV0073_CTRL_SYSTEM_GET_HOTPLUG_STATE_PARAMS *);  // exported (id=0x73010a)
    NV_STATUS (*__dispcmnCtrlCmdSystemGetInternalDisplays__)(struct DispCommon * /*this*/, NV0073_CTRL_SYSTEM_GET_INTERNAL_DISPLAYS_PARAMS *);  // exported (id=0x730116)
    NV_STATUS (*__dispcmnCtrlCmdSystemGetConnectorTable__)(struct DispCommon * /*this*/, NV0073_CTRL_SYSTEM_GET_CONNECTOR_TABLE_PARAMS *);  // exported (id=0x73011d)
    NV_STATUS (*__dispcmnCtrlCmdSystemVrrDisplayInfo__)(struct DispCommon * /*this*/, NV0073_CTRL_SYSTEM_VRR_DISPLAY_INFO_PARAMS *);  // exported (id=0x73012c)
    NV_STATUS (*__dispcmnCtrlCmdVRRSetRgLineActive__)(struct DispCommon * /*this*/, NV0073_CTRL_CMD_SYSTEM_VRR_SET_RGLINE_ACTIVE_PARAMS *);  // exported (id=0x73014d)
    NV_STATUS (*__dispcmnCtrlCmdInternalVRRSetRgLineActive__)(struct DispCommon * /*this*/, NV0073_CTRL_CMD_SYSTEM_VRR_SET_RGLINE_ACTIVE_PARAMS *);  // exported (id=0x730402)
    NV_STATUS (*__dispcmnCtrlCmdVblankSemControlEnable__)(struct DispCommon * /*this*/, NV0073_CTRL_CMD_SYSTEM_VBLANK_SEM_CONTROL_ENABLE_PARAMS *);  // exported (id=0x73014e)
    NV_STATUS (*__dispcmnCtrlCmdInternalVblankSemControlEnable__)(struct DispCommon * /*this*/, NV0073_CTRL_CMD_INTERNAL_VBLANK_SEM_CONTROL_ENABLE_PARAMS *);  // exported (id=0x730405)
    NV_STATUS (*__dispcmnCtrlCmdVblankSemControlDisable__)(struct DispCommon * /*this*/, NV0073_CTRL_CMD_SYSTEM_VBLANK_SEM_CONTROL_DISABLE_PARAMS *);  // exported (id=0x73014f)
    NV_STATUS (*__dispcmnCtrlCmdAccelVblankSemControl__)(struct DispCommon * /*this*/, NV0073_CTRL_CMD_SYSTEM_ACCEL_VBLANK_SEM_CONTROLS_PARAMS *);  // exported (id=0x730150)
    NV_STATUS (*__dispcmnCtrlCmdDpEnableVrr__)(struct DispCommon * /*this*/, NV0073_CTRL_CMD_DP_ENABLE_VRR_PARAMS *);  // exported (id=0x73137d)
    NV_STATUS (*__dispcmnCtrlCmdClearELVBlock__)(struct DispCommon * /*this*/, NV0073_CTRL_SYSTEM_CLEAR_ELV_BLOCK_PARAMS *);  // exported (id=0x73012e)
    NV_STATUS (*__dispcmnCtrlCmdSpecificDisplayChange__)(struct DispCommon * /*this*/, NV0073_CTRL_SPECIFIC_DISPLAY_CHANGE_PARAMS *);  // exported (id=0x7302a4)
    NV_STATUS (*__dispcmnCtrlCmdDfpGetSpreadSpectrum__)(struct DispCommon * /*this*/, NV0073_CTRL_DFP_GET_SPREAD_SPECTRUM_PARAMS *);  // exported (id=0x73114c)
    NV_STATUS (*__dispcmnCtrlCmdDfpGetLcdGpioPinNum__)(struct DispCommon * /*this*/, NV0073_CTRL_DFP_GET_LCD_GPIO_PIN_NUM_PARAMS *);  // exported (id=0x731154)
    NV_STATUS (*__dispcmnCtrlCmdDpAuxchI2cTransferCtrl__)(struct DispCommon * /*this*/, NV0073_CTRL_DP_AUXCH_I2C_TRANSFER_CTRL_PARAMS *);  // exported (id=0x73137c)
    NV_STATUS (*__dispcmnCtrlCmdDpASSRCtrl__)(struct DispCommon * /*this*/, NV0073_CTRL_DP_ASSR_CTRL_PARAMS *);  // exported (id=0x73135a)
    NV_STATUS (*__dispcmnCtrlCmdDpSetEcf__)(struct DispCommon * /*this*/, NV0073_CTRL_CMD_DP_SET_ECF_PARAMS *);  // exported (id=0x731366)
    NV_STATUS (*__dispcmnCtrlCmdDfpRecordChannelRegisters__)(struct DispCommon * /*this*/, NV0073_CTRL_CMD_SYSTEM_RECORD_CHANNEL_REGS_PARAMS *);  // exported (id=0x73014a)
    NV_STATUS (*__dispcmnCtrlCmdSpecificGetBacklightBrightness__)(struct DispCommon * /*this*/, NV0073_CTRL_SPECIFIC_BACKLIGHT_BRIGHTNESS_PARAMS *);  // exported (id=0x730291)
    NV_STATUS (*__dispcmnCtrlCmdSpecificSetBacklightBrightness__)(struct DispCommon * /*this*/, NV0073_CTRL_SPECIFIC_BACKLIGHT_BRIGHTNESS_PARAMS *);  // exported (id=0x730292)
    NV_STATUS (*__dispcmnCtrlCmdPsrGetSrPanelInfo__)(struct DispCommon * /*this*/, NV0073_CTRL_PSR_GET_SR_PANEL_INFO_PARAMS *);  // exported (id=0x731602)
    NV_STATUS (*__dispcmnCtrlCmdDfpSwitchDispMux__)(struct DispCommon * /*this*/, NV0073_CTRL_CMD_DFP_SWITCH_DISP_MUX_PARAMS *);  // exported (id=0x731160)
    NV_STATUS (*__dispcmnCtrlCmdInternalDfpSwitchDispMux__)(struct DispCommon * /*this*/, NV0073_CTRL_CMD_DFP_SWITCH_DISP_MUX_PARAMS *);  // exported (id=0x730460)
    NV_STATUS (*__dispcmnCtrlCmdDfpRunPreDispMuxOperations__)(struct DispCommon * /*this*/, NV0073_CTRL_CMD_DFP_RUN_PRE_DISP_MUX_OPERATIONS_PARAMS *);  // exported (id=0x731161)
    NV_STATUS (*__dispcmnCtrlCmdDfpRunPostDispMuxOperations__)(struct DispCommon * /*this*/, NV0073_CTRL_CMD_DFP_RUN_POST_DISP_MUX_OPERATIONS_PARAMS *);  // exported (id=0x731162)
    NV_STATUS (*__dispcmnCtrlCmdDfpGetDispMuxStatus__)(struct DispCommon * /*this*/, NV0073_CTRL_CMD_DFP_GET_DISP_MUX_STATUS_PARAMS *);  // exported (id=0x731163)
    NV_STATUS (*__dispcmnCtrlCmdInternalDfpGetDispMuxStatus__)(struct DispCommon * /*this*/, NV0073_CTRL_CMD_DFP_GET_DISP_MUX_STATUS_PARAMS *);  // exported (id=0x730404)
    NV_STATUS (*__dispcmnCtrlCmdDfpInternalLcdOverdrive__)(struct DispCommon * /*this*/, NV0073_CTRL_CMD_DP_AUXCH_OD_CTRL_PARAMS *);  // exported (id=0x731380)
    NV_STATUS (*__dispcmnCtrlCmdDfpVariableBacklightCtrl__)(struct DispCommon * /*this*/, NV0073_CTRL_CMD_DP_AUXCH_VBL_CTRL_PARAMS *);  // exported (id=0x731386)
    NV_STATUS (*__dispcmnCtrlCmdSystemExecuteAcpiMethod__)(struct DispCommon * /*this*/, NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_PARAMS *);  // exported (id=0x730120)
    NV_STATUS (*__dispcmnCtrlCmdSystemGetAcpiIdMap__)(struct DispCommon * /*this*/, NV0073_CTRL_SYSTEM_GET_ACPI_ID_MAP_PARAMS *);  // exported (id=0x730115)
    NV_STATUS (*__dispcmnCtrlCmdSystemAcpiSubsystemActivated__)(struct DispCommon * /*this*/, NV0073_CTRL_SYSTEM_ACPI_SUBSYSTEM_ACTIVATED_PARAMS *);  // exported (id=0x730117)
    NV_STATUS (*__dispcmnCtrlCmdSpecificSetAcpiIdMapping__)(struct DispCommon * /*this*/, NV0073_CTRL_SPECIFIC_SET_ACPI_ID_MAPPING_PARAMS *);  // exported (id=0x730284)
    NV_STATUS (*__dispcmnCtrlCmdSpecificGetAcpiDodDisplayPortAttachment__)(struct DispCommon * /*this*/, NV0073_CTRL_GET_ACPI_DOD_DISPLAY_PORT_ATTACHMENT_PARAMS *);  // exported (id=0x730285)
    NV_STATUS (*__dispcmnCtrlCmdSystemGetCapsV2__)(struct DispCommon * /*this*/, NV0073_CTRL_SYSTEM_GET_CAPS_V2_PARAMS *);  // exported (id=0x730101)
    NV_STATUS (*__dispcmnCtrlCmdSystemGetNumHeads__)(struct DispCommon * /*this*/, NV0073_CTRL_SYSTEM_GET_NUM_HEADS_PARAMS *);  // exported (id=0x730102)
    NV_STATUS (*__dispcmnCtrlCmdSystemGetScanline__)(struct DispCommon * /*this*/, NV0073_CTRL_SYSTEM_GET_SCANLINE_PARAMS *);  // exported (id=0x730104)
    NV_STATUS (*__dispcmnCtrlCmdSystemGetSuppported__)(struct DispCommon * /*this*/, NV0073_CTRL_SYSTEM_GET_SUPPORTED_PARAMS *);  // exported (id=0x730107)
    NV_STATUS (*__dispcmnCtrlCmdSystemGetConnectState__)(struct DispCommon * /*this*/, NV0073_CTRL_SYSTEM_GET_CONNECT_STATE_PARAMS *);  // exported (id=0x730108)
    NV_STATUS (*__dispcmnCtrlCmdSystemGetHotplugUnplugState__)(struct DispCommon * /*this*/, NV0073_CTRL_SYSTEM_GET_HOTPLUG_UNPLUG_STATE_PARAMS *);  // exported (id=0x73012d)
    NV_STATUS (*__dispcmnCtrlCmdInternalGetHotplugUnplugState__)(struct DispCommon * /*this*/, NV0073_CTRL_SYSTEM_GET_HOTPLUG_UNPLUG_STATE_PARAMS *);  // exported (id=0x730401)
    NV_STATUS (*__dispcmnCtrlCmdSystemGetHeadRoutingMap__)(struct DispCommon * /*this*/, NV0073_CTRL_SYSTEM_GET_HEAD_ROUTING_MAP_PARAMS *);  // exported (id=0x73010b)
    NV_STATUS (*__dispcmnCtrlCmdSystemGetActive__)(struct DispCommon * /*this*/, NV0073_CTRL_SYSTEM_GET_ACTIVE_PARAMS *);  // exported (id=0x73010c)
    NV_STATUS (*__dispcmnCtrlCmdSystemGetBootDisplays__)(struct DispCommon * /*this*/, NV0073_CTRL_SYSTEM_GET_BOOT_DISPLAYS_PARAMS *);  // exported (id=0x73011e)
    NV_STATUS (*__dispcmnCtrlCmdSystemQueryDisplayIdsWithMux__)(struct DispCommon * /*this*/, NV0073_CTRL_CMD_SYSTEM_QUERY_DISPLAY_IDS_WITH_MUX_PARAMS *);  // exported (id=0x73013d)
    NV_STATUS (*__dispcmnCtrlCmdSystemCheckSidebandI2cSupport__)(struct DispCommon * /*this*/, NV0073_CTRL_CMD_SYSTEM_CHECK_SIDEBAND_I2C_SUPPORT_PARAMS *);  // exported (id=0x73014b)
    NV_STATUS (*__dispcmnCtrlCmdSystemAllocateDisplayBandwidth__)(struct DispCommon * /*this*/, NV0073_CTRL_SYSTEM_ALLOCATE_DISPLAY_BANDWIDTH_PARAMS *);  // exported (id=0x730143)
    NV_STATUS (*__dispcmnCtrlCmdSystemGetHotplugConfig__)(struct DispCommon * /*this*/, NV0073_CTRL_SYSTEM_GET_SET_HOTPLUG_CONFIG_PARAMS *);  // exported (id=0x730109)
    NV_STATUS (*__dispcmnCtrlCmdSystemGetHotplugEventConfig__)(struct DispCommon * /*this*/, NV0073_CTRL_SYSTEM_HOTPLUG_EVENT_CONFIG_PARAMS *);  // exported (id=0x730144)
    NV_STATUS (*__dispcmnCtrlCmdSystemSetHotplugEventConfig__)(struct DispCommon * /*this*/, NV0073_CTRL_SYSTEM_HOTPLUG_EVENT_CONFIG_PARAMS *);  // exported (id=0x730145)
    NV_STATUS (*__dispcmnCtrlCmdSystemArmLightweightSupervisor__)(struct DispCommon * /*this*/, NV0073_CTRL_SYSTEM_ARM_LIGHTWEIGHT_SUPERVISOR_PARAMS *);  // exported (id=0x73012f)
    NV_STATUS (*__dispcmnCtrlCmdSystemConfigVrrPstateSwitch__)(struct DispCommon * /*this*/, NV0073_CTRL_SYSTEM_CONFIG_VRR_PSTATE_SWITCH_PARAMS *);  // exported (id=0x730134)
    NV_STATUS (*__dispcmnCtrlCmdSpecificGetType__)(struct DispCommon * /*this*/, NV0073_CTRL_SPECIFIC_GET_TYPE_PARAMS *);  // exported (id=0x730240)
    NV_STATUS (*__dispcmnCtrlCmdSpecificGetEdidV2__)(struct DispCommon * /*this*/, NV0073_CTRL_SPECIFIC_GET_EDID_V2_PARAMS *);  // exported (id=0x730245)
    NV_STATUS (*__dispcmnCtrlCmdSpecificSetEdidV2__)(struct DispCommon * /*this*/, NV0073_CTRL_SPECIFIC_SET_EDID_V2_PARAMS *);  // exported (id=0x730246)
    NV_STATUS (*__dispcmnCtrlCmdSpecificFakeDevice__)(struct DispCommon * /*this*/, NV0073_CTRL_CMD_SPECIFIC_FAKE_DEVICE_PARAMS *);  // exported (id=0x730243)
    NV_STATUS (*__dispcmnCtrlCmdSpecificGetConnectorData__)(struct DispCommon * /*this*/, NV0073_CTRL_SPECIFIC_GET_CONNECTOR_DATA_PARAMS *);  // exported (id=0x730250)
    NV_STATUS (*__dispcmnCtrlCmdSpecificSetHdmiEnable__)(struct DispCommon * /*this*/, NV0073_CTRL_SPECIFIC_SET_HDMI_ENABLE_PARAMS *);  // exported (id=0x730273)
    NV_STATUS (*__dispcmnCtrlCmdSpecificCtrlHdmi__)(struct DispCommon * /*this*/, NV0073_CTRL_SPECIFIC_CTRL_HDMI_PARAMS *);  // exported (id=0x730274)
    NV_STATUS (*__dispcmnCtrlCmdSpecificGetAllHeadMask__)(struct DispCommon * /*this*/, NV0073_CTRL_SPECIFIC_GET_ALL_HEAD_MASK_PARAMS *);  // exported (id=0x730287)
    NV_STATUS (*__dispcmnCtrlCmdSpecificSetOdPacket__)(struct DispCommon * /*this*/, NV0073_CTRL_SPECIFIC_SET_OD_PACKET_PARAMS *);  // exported (id=0x730288)
    NV_STATUS (*__dispcmnCtrlCmdSpecificAcquireSharedGenericPacket__)(struct DispCommon * /*this*/, NV0073_CTRL_SPECIFIC_ACQUIRE_SHARED_GENERIC_PACKET_PARAMS *);  // exported (id=0x7302aa)
    NV_STATUS (*__dispcmnCtrlCmdSpecificSetSharedGenericPacket__)(struct DispCommon * /*this*/, NV0073_CTRL_SPECIFIC_SET_SHARED_GENERIC_PACKET_PARAMS *);  // exported (id=0x7302a9)
    NV_STATUS (*__dispcmnCtrlCmdSpecificReleaseSharedGenericPacket__)(struct DispCommon * /*this*/, NV0073_CTRL_SPECIFIC_RELEASE_SHARED_GENERIC_PACKET_PARAMS *);  // exported (id=0x7302ab)
    NV_STATUS (*__dispcmnCtrlCmdSpecificSetOdPacketCtrl__)(struct DispCommon * /*this*/, NV0073_CTRL_SPECIFIC_SET_OD_PACKET_CTRL_PARAMS *);  // exported (id=0x730289)
    NV_STATUS (*__dispcmnCtrlCmdSpecificOrGetInfo__)(struct DispCommon * /*this*/, NV0073_CTRL_SPECIFIC_OR_GET_INFO_PARAMS *);  // exported (id=0x73028b)
    NV_STATUS (*__dispcmnCtrlCmdSpecificGetPclkLimit__)(struct DispCommon * /*this*/, NV0073_CTRL_SPECIFIC_GET_PCLK_LIMIT_PARAMS *);  // exported (id=0x73028a)
    NV_STATUS (*__dispcmnCtrlCmdSpecificSetHdmiSinkCaps__)(struct DispCommon * /*this*/, NV0073_CTRL_SPECIFIC_SET_HDMI_SINK_CAPS_PARAMS *);  // exported (id=0x730293)
    NV_STATUS (*__dispcmnCtrlCmdSpecificSetMonitorPower__)(struct DispCommon * /*this*/, NV0073_CTRL_SPECIFIC_SET_MONITOR_POWER_PARAMS *);  // exported (id=0x730295)
    NV_STATUS (*__dispcmnCtrlCmdSpecificSetHdmiFrlLinkConfig__)(struct DispCommon * /*this*/, NV0073_CTRL_SPECIFIC_SET_HDMI_FRL_LINK_CONFIG_PARAMS *);  // exported (id=0x73029a)
    NV_STATUS (*__dispcmnCtrlCmdSpecificApplyEdidOverrideV2__)(struct DispCommon * /*this*/, NV0073_CTRL_SPECIFIC_APPLY_EDID_OVERRIDE_V2_PARAMS *);  // exported (id=0x7302a1)
    NV_STATUS (*__dispcmnCtrlCmdSpecificGetI2cPortid__)(struct DispCommon * /*this*/, NV0073_CTRL_SPECIFIC_GET_I2C_PORTID_PARAMS *);  // exported (id=0x730211)
    NV_STATUS (*__dispcmnCtrlCmdSpecificGetHdmiGpuCaps__)(struct DispCommon * /*this*/, NV0073_CTRL_SPECIFIC_GET_HDMI_GPU_CAPS_PARAMS *);  // exported (id=0x7302a2)
    NV_STATUS (*__dispcmnCtrlCmdSpecificGetHdmiScdcData__)(struct DispCommon * /*this*/, NV0073_CTRL_SPECIFIC_GET_HDMI_SCDC_DATA_PARAMS *);  // exported (id=0x7302a6)
    NV_STATUS (*__dispcmnCtrlCmdSpecificIsDirectmodeDisplay__)(struct DispCommon * /*this*/, NV0073_CTRL_SPECIFIC_IS_DIRECTMODE_DISPLAY_PARAMS *);  // exported (id=0x7302a7)
    NV_STATUS (*__dispcmnCtrlCmdSpecificDefaultAdaptivesyncDisplay__)(struct DispCommon * /*this*/, NV0073_CTRL_SPECIFIC_DEFAULT_ADAPTIVESYNC_DISPLAY_PARAMS *);  // exported (id=0x7302ae)
    NV_STATUS (*__dispcmnCtrlCmdSpecificSetHdmiFrlCapacityComputation__)(struct DispCommon * /*this*/, NV0073_CTRL_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION_PARAMS *);  // exported (id=0x7302a8)
    NV_STATUS (*__dispcmnCtrlCmdSpecificDispI2cReadWrite__)(struct DispCommon * /*this*/, NV0073_CTRL_SPECIFIC_DISP_I2C_READ_WRITE_PARAMS *);  // exported (id=0x7302ac)
    NV_STATUS (*__dispcmnCtrlCmdSpecificGetValidHeadWindowAssignment__)(struct DispCommon * /*this*/, NV0073_CTRL_SPECIFIC_GET_VALID_HEAD_WINDOW_ASSIGNMENT_PARAMS *);  // exported (id=0x7302ad)
    NV_STATUS (*__dispcmnCtrlCmdSpecificSetHdmiAudioMutestream__)(struct DispCommon * /*this*/, NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_AUDIO_MUTESTREAM_PARAMS *);  // exported (id=0x730275)
    NV_STATUS (*__dispcmnCtrlCmdDfpEdpDriverUnload__)(struct DispCommon * /*this*/, NV0073_CTRL_DFP_EDP_DRIVER_UNLOAD_PARAMS *);  // exported (id=0x731176)
    NV_STATUS (*__dispcmnCtrlCmdDfpGetInfo__)(struct DispCommon * /*this*/, NV0073_CTRL_DFP_GET_INFO_PARAMS *);  // exported (id=0x731140)
    NV_STATUS (*__dispcmnCtrlCmdDfpGetDisplayportDongleInfo__)(struct DispCommon * /*this*/, NV0073_CTRL_DFP_GET_DISPLAYPORT_DONGLE_INFO_PARAMS *);  // exported (id=0x731142)
    NV_STATUS (*__dispcmnCtrlCmdDfpSetEldAudioCaps__)(struct DispCommon * /*this*/, NV0073_CTRL_DFP_SET_ELD_AUDIO_CAP_PARAMS *);  // exported (id=0x731144)
    NV_STATUS (*__dispcmnCtrlCmdDfpSetAudioEnable__)(struct DispCommon * /*this*/, NV0073_CTRL_DFP_SET_AUDIO_ENABLE_PARAMS *);  // exported (id=0x731150)
    NV_STATUS (*__dispcmnCtrlCmdDfpUpdateDynamicDfpCache__)(struct DispCommon * /*this*/, NV0073_CTRL_DFP_UPDATE_DYNAMIC_DFP_CACHE_PARAMS *);  // exported (id=0x73114e)
    NV_STATUS (*__dispcmnCtrlCmdDfpAssignSor__)(struct DispCommon * /*this*/, NV0073_CTRL_DFP_ASSIGN_SOR_PARAMS *);  // exported (id=0x731152)
    NV_STATUS (*__dispcmnCtrlCmdDfpDscCrcControl__)(struct DispCommon * /*this*/, NV0073_CTRL_DFP_DSC_CRC_CONTROL_PARAMS *);  // exported (id=0x731157)
    NV_STATUS (*__dispcmnCtrlCmdDfpInitMuxData__)(struct DispCommon * /*this*/, NV0073_CTRL_CMD_DFP_INIT_MUX_DATA_PARAMS *);  // exported (id=0x731158)
    NV_STATUS (*__dispcmnCtrlCmdDfpGetDsiModeTiming__)(struct DispCommon * /*this*/, NV0073_CTRL_CMD_DFP_GET_DSI_MODE_TIMING_PARAMS *);  // exported (id=0x731166)
    NV_STATUS (*__dispcmnCtrlCmdDfpConfigTwoHeadOneOr__)(struct DispCommon * /*this*/, NV0073_CTRL_DFP_CONFIG_TWO_HEAD_ONE_OR_PARAMS *);  // exported (id=0x731156)
    NV_STATUS (*__dispcmnCtrlCmdDfpGetPadlinkMask__)(struct DispCommon * /*this*/, NV0073_CTRL_DFP_GET_PADLINK_MASK_PARAMS *);  // exported (id=0x731153)
    NV_STATUS (*__dispcmnCtrlCmdDfpGetFixedModeTiming__)(struct DispCommon * /*this*/, NV0073_CTRL_DFP_GET_FIXED_MODE_TIMING_PARAMS *);  // exported (id=0x731172)
    NV_STATUS (*__dispcmnCtrlCmdDpAuxchCtrl__)(struct DispCommon * /*this*/, NV0073_CTRL_DP_AUXCH_CTRL_PARAMS *);  // exported (id=0x731341)
    NV_STATUS (*__dispcmnCtrlCmdDpAuxchSetSema__)(struct DispCommon * /*this*/, NV0073_CTRL_DP_AUXCH_SET_SEMA_PARAMS *);  // exported (id=0x731342)
    NV_STATUS (*__dispcmnCtrlCmdDpCtrl__)(struct DispCommon * /*this*/, NV0073_CTRL_DP_CTRL_PARAMS *);  // exported (id=0x731343)
    NV_STATUS (*__dispcmnCtrlCmdDpGetLaneData__)(struct DispCommon * /*this*/, NV0073_CTRL_DP_LANE_DATA_PARAMS *);  // exported (id=0x731345)
    NV_STATUS (*__dispcmnCtrlCmdDpSetLaneData__)(struct DispCommon * /*this*/, NV0073_CTRL_DP_LANE_DATA_PARAMS *);  // exported (id=0x731346)
    NV_STATUS (*__dispcmnCtrlCmdDpSetTestpattern__)(struct DispCommon * /*this*/, NV0073_CTRL_DP_SET_TESTPATTERN_PARAMS *);  // exported (id=0x731347)
    NV_STATUS (*__dispcmnCtrlCmdDpMainLinkCtrl__)(struct DispCommon * /*this*/, NV0073_CTRL_DP_MAIN_LINK_CTRL_PARAMS *);  // exported (id=0x731356)
    NV_STATUS (*__dispcmnCtrlCmdDpSetAudioMuteStream__)(struct DispCommon * /*this*/, NV0073_CTRL_DP_SET_AUDIO_MUTESTREAM_PARAMS *);  // exported (id=0x731359)
    NV_STATUS (*__dispcmnCtrlCmdDpGetLinkConfig__)(struct DispCommon * /*this*/, NV0073_CTRL_DP_GET_LINK_CONFIG_PARAMS *);  // exported (id=0x731360)
    NV_STATUS (*__dispcmnCtrlCmdDpGetEDPData__)(struct DispCommon * /*this*/, NV0073_CTRL_DP_GET_EDP_DATA_PARAMS *);  // exported (id=0x731361)
    NV_STATUS (*__dispcmnCtrlCmdDpTopologyAllocateDisplayId__)(struct DispCommon * /*this*/, NV0073_CTRL_CMD_DP_TOPOLOGY_ALLOCATE_DISPLAYID_PARAMS *);  // exported (id=0x73135b)
    NV_STATUS (*__dispcmnCtrlCmdDpTopologyFreeDisplayId__)(struct DispCommon * /*this*/, NV0073_CTRL_CMD_DP_TOPOLOGY_FREE_DISPLAYID_PARAMS *);  // exported (id=0x73135c)
    NV_STATUS (*__dispcmnCtrlCmdDpConfigStream__)(struct DispCommon * /*this*/, NV0073_CTRL_CMD_DP_CONFIG_STREAM_PARAMS *);  // exported (id=0x731362)
    NV_STATUS (*__dispcmnCtrlCmdDpConfigSingleHeadMultiStream__)(struct DispCommon * /*this*/, NV0073_CTRL_CMD_DP_CONFIG_SINGLE_HEAD_MULTI_STREAM_PARAMS *);  // exported (id=0x73136e)
    NV_STATUS (*__dispcmnCtrlCmdDpSetRateGov__)(struct DispCommon * /*this*/, NV0073_CTRL_CMD_DP_SET_RATE_GOV_PARAMS *);  // exported (id=0x731363)
    NV_STATUS (*__dispcmnCtrlCmdDpSendACT__)(struct DispCommon * /*this*/, NV0073_CTRL_CMD_DP_SEND_ACT_PARAMS *);  // exported (id=0x731367)
    NV_STATUS (*__dispcmnCtrlCmdDpSetManualDisplayPort__)(struct DispCommon * /*this*/, NV0073_CTRL_CMD_DP_SET_MANUAL_DISPLAYPORT_PARAMS *);  // exported (id=0x731365)
    NV_STATUS (*__dispcmnCtrlCmdDpGetCaps__)(struct DispCommon * /*this*/, NV0073_CTRL_CMD_DP_GET_CAPS_PARAMS *);  // exported (id=0x731369)
    NV_STATUS (*__dispcmnCtrlCmdDpSetMSAPropertiesv2__)(struct DispCommon * /*this*/, NV0073_CTRL_CMD_DP_SET_MSA_PROPERTIES_V2_PARAMS *);  // exported (id=0x731381)
    NV_STATUS (*__dispcmnCtrlCmdDpSetStereoMSAProperties__)(struct DispCommon * /*this*/, NV0073_CTRL_CMD_DP_SET_STEREO_MSA_PROPERTIES_PARAMS *);  // exported (id=0x731378)
    NV_STATUS (*__dispcmnCtrlCmdDpGenerateFakeInterrupt__)(struct DispCommon * /*this*/, NV0073_CTRL_CMD_DP_GENERATE_FAKE_INTERRUPT_PARAMS *);  // exported (id=0x73136b)
    NV_STATUS (*__dispcmnCtrlCmdDpConfigRadScratchReg__)(struct DispCommon * /*this*/, NV0073_CTRL_CMD_DP_CONFIG_RAD_SCRATCH_REG_PARAMS *);  // exported (id=0x73136c)
    NV_STATUS (*__dispcmnCtrlCmdDpSetTriggerSelect__)(struct DispCommon * /*this*/, NV0073_CTRL_CMD_DP_SET_TRIGGER_SELECT_PARAMS *);  // exported (id=0x73136f)
    NV_STATUS (*__dispcmnCtrlCmdDpSetTriggerAll__)(struct DispCommon * /*this*/, NV0073_CTRL_CMD_DP_SET_TRIGGER_ALL_PARAMS *);  // exported (id=0x731370)
    NV_STATUS (*__dispcmnCtrlCmdDpGetAuxLogData__)(struct DispCommon * /*this*/, NV0073_CTRL_CMD_DP_GET_AUXLOGGER_BUFFER_DATA_PARAMS *);  // exported (id=0x731373)
    NV_STATUS (*__dispcmnCtrlCmdDpConfigIndexedLinkRates__)(struct DispCommon * /*this*/, NV0073_CTRL_CMD_DP_CONFIG_INDEXED_LINK_RATES_PARAMS *);  // exported (id=0x731377)
    NV_STATUS (*__dispcmnCtrlCmdDpConfigureFec__)(struct DispCommon * /*this*/, NV0073_CTRL_CMD_DP_CONFIGURE_FEC_PARAMS *);  // exported (id=0x73137a)
    NV_STATUS (*__dispcmnCtrlCmdDpGetGenericInfoframe__)(struct DispCommon * /*this*/, NV0073_CTRL_DP_GET_GENERIC_INFOFRAME_PARAMS *);  // exported (id=0x73137e)
    NV_STATUS (*__dispcmnCtrlCmdDpGetMsaAttributes__)(struct DispCommon * /*this*/, NV0073_CTRL_DP_GET_MSA_ATTRIBUTES_PARAMS *);  // exported (id=0x73137f)
    NV_STATUS (*__dispcmnCtrlCmdFrlConfigMacroPad__)(struct DispCommon * /*this*/, NV0073_CTRL_CMD_FRL_CONFIG_MACRO_PAD_PARAMS *);  // exported (id=0x730502)
    NV_STATUS (*__dispcmnCtrlCmdDpConfigMacroPad__)(struct DispCommon * /*this*/, NV0073_CTRL_CMD_DP_CONFIG_MACRO_PAD_PARAMS *);  // exported (id=0x73137b)
    NV_STATUS (*__dispcmnCtrlCmdDpSetPreemphasisDrivecurrentPostcursor2Data__)(struct DispCommon * /*this*/, NV0073_CTRL_DP_SET_PREEMPHASIS_DRIVECURRENT_POSTCURSOR2_DATA_PARAMS *);  // exported (id=0x731351)
    NV_STATUS (*__dispcmnCtrlCmdDpGetPreemphasisDrivecurrentPostcursor2Data__)(struct DispCommon * /*this*/, NV0073_CTRL_DP_GET_PREEMPHASIS_DRIVECURRENT_POSTCURSOR2_DATA_PARAMS *);  // exported (id=0x731352)
    NV_STATUS (*__dispcmnControl__)(struct DispCommon * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (dispapi) base (dispapi)
    NV_STATUS (*__dispcmnControl_Prologue__)(struct DispCommon * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (dispapi) base (dispapi)
    void (*__dispcmnControl_Epilogue__)(struct DispCommon * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (dispapi) base (dispapi)
    NvBool (*__dispcmnAccessCallback__)(struct DispCommon * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (dispapi)
    NvBool (*__dispcmnShareCallback__)(struct DispCommon * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (rmres) base (dispapi)
    NV_STATUS (*__dispcmnGetMemInterMapParams__)(struct DispCommon * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (dispapi)
    NV_STATUS (*__dispcmnCheckMemInterUnmap__)(struct DispCommon * /*this*/, NvBool);  // virtual inherited (rmres) base (dispapi)
    NV_STATUS (*__dispcmnGetMemoryMappingDescriptor__)(struct DispCommon * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (dispapi)
    NV_STATUS (*__dispcmnControlSerialization_Prologue__)(struct DispCommon * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (dispapi)
    void (*__dispcmnControlSerialization_Epilogue__)(struct DispCommon * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (dispapi)
    NvBool (*__dispcmnCanCopy__)(struct DispCommon * /*this*/);  // virtual inherited (res) base (dispapi)
    NV_STATUS (*__dispcmnIsDuplicate__)(struct DispCommon * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (dispapi)
    void (*__dispcmnPreDestruct__)(struct DispCommon * /*this*/);  // virtual inherited (res) base (dispapi)
    NV_STATUS (*__dispcmnControlFilter__)(struct DispCommon * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (dispapi)
    NV_STATUS (*__dispcmnMap__)(struct DispCommon * /*this*/, struct CALL_CONTEXT *, RS_CPU_MAP_PARAMS *, RsCpuMapping *);  // virtual inherited (res) base (dispapi)
    NV_STATUS (*__dispcmnUnmap__)(struct DispCommon * /*this*/, struct CALL_CONTEXT *, RsCpuMapping *);  // virtual inherited (res) base (dispapi)
    NvBool (*__dispcmnIsPartialUnmapSupported__)(struct DispCommon * /*this*/);  // inline virtual inherited (res) base (dispapi) body
    NV_STATUS (*__dispcmnMapTo__)(struct DispCommon * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (dispapi)
    NV_STATUS (*__dispcmnUnmapFrom__)(struct DispCommon * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (dispapi)
    NvU32 (*__dispcmnGetRefCount__)(struct DispCommon * /*this*/);  // virtual inherited (res) base (dispapi)
    void (*__dispcmnAddAdditionalDependants__)(struct RsClient *, struct DispCommon * /*this*/, RsResourceRef *);  // virtual inherited (res) base (dispapi)
    PEVENTNOTIFICATION * (*__dispcmnGetNotificationListPtr__)(struct DispCommon * /*this*/);  // virtual inherited (notify) base (dispapi)
    struct NotifShare * (*__dispcmnGetNotificationShare__)(struct DispCommon * /*this*/);  // virtual inherited (notify) base (dispapi)
    void (*__dispcmnSetNotificationShare__)(struct DispCommon * /*this*/, struct NotifShare *);  // virtual inherited (notify) base (dispapi)
    NV_STATUS (*__dispcmnUnregisterEvent__)(struct DispCommon * /*this*/, NvHandle, NvHandle, NvHandle, NvHandle);  // virtual inherited (notify) base (dispapi)
    NV_STATUS (*__dispcmnGetOrAllocNotifShare__)(struct DispCommon * /*this*/, NvHandle, NvHandle, struct NotifShare **);  // virtual inherited (notify) base (dispapi)

    // Data members
    NvU32 hotPlugMaskToBeReported;
    NvU32 hotUnplugMaskToBeReported;
};

#ifndef __NVOC_CLASS_DispCommon_TYPEDEF__
#define __NVOC_CLASS_DispCommon_TYPEDEF__
typedef struct DispCommon DispCommon;
#endif /* __NVOC_CLASS_DispCommon_TYPEDEF__ */

#ifndef __nvoc_class_id_DispCommon
#define __nvoc_class_id_DispCommon 0x41f4f2
#endif /* __nvoc_class_id_DispCommon */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_DispCommon;

#define __staticCast_DispCommon(pThis) \
    ((pThis)->__nvoc_pbase_DispCommon)

#ifdef __nvoc_disp_objs_h_disabled
#define __dynamicCast_DispCommon(pThis) ((DispCommon*)NULL)
#else //__nvoc_disp_objs_h_disabled
#define __dynamicCast_DispCommon(pThis) \
    ((DispCommon*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(DispCommon)))
#endif //__nvoc_disp_objs_h_disabled

NV_STATUS __nvoc_objCreateDynamic_DispCommon(DispCommon**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_DispCommon(DispCommon**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_DispCommon(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_DispCommon((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define dispcmnCtrlCmdSystemGetVblankCounter_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSystemGetVblankCounter__
#define dispcmnCtrlCmdSystemGetVblankCounter(pDispCommon, pVBCounterParams) dispcmnCtrlCmdSystemGetVblankCounter_DISPATCH(pDispCommon, pVBCounterParams)
#define dispcmnCtrlCmdSystemGetVblankEnable_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSystemGetVblankEnable__
#define dispcmnCtrlCmdSystemGetVblankEnable(pDispCommon, pVBEnableParams) dispcmnCtrlCmdSystemGetVblankEnable_DISPATCH(pDispCommon, pVBEnableParams)
#define dispcmnCtrlCmdSystemCheckSidebandSrSupport_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSystemCheckSidebandSrSupport__
#define dispcmnCtrlCmdSystemCheckSidebandSrSupport(pDispCommon, pParams) dispcmnCtrlCmdSystemCheckSidebandSrSupport_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSystemGetHotplugState_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSystemGetHotplugState__
#define dispcmnCtrlCmdSystemGetHotplugState(pDispCommon, pHotplugParams) dispcmnCtrlCmdSystemGetHotplugState_DISPATCH(pDispCommon, pHotplugParams)
#define dispcmnCtrlCmdSystemGetInternalDisplays_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSystemGetInternalDisplays__
#define dispcmnCtrlCmdSystemGetInternalDisplays(pDispCommon, pInternalDisplaysParams) dispcmnCtrlCmdSystemGetInternalDisplays_DISPATCH(pDispCommon, pInternalDisplaysParams)
#define dispcmnCtrlCmdSystemGetConnectorTable_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSystemGetConnectorTable__
#define dispcmnCtrlCmdSystemGetConnectorTable(pDispCommon, pParams) dispcmnCtrlCmdSystemGetConnectorTable_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSystemVrrDisplayInfo_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSystemVrrDisplayInfo__
#define dispcmnCtrlCmdSystemVrrDisplayInfo(pDispCommon, pParams) dispcmnCtrlCmdSystemVrrDisplayInfo_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdVRRSetRgLineActive_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdVRRSetRgLineActive__
#define dispcmnCtrlCmdVRRSetRgLineActive(pDispCommon, pParams) dispcmnCtrlCmdVRRSetRgLineActive_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdInternalVRRSetRgLineActive_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdInternalVRRSetRgLineActive__
#define dispcmnCtrlCmdInternalVRRSetRgLineActive(pDispCommon, pParams) dispcmnCtrlCmdInternalVRRSetRgLineActive_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdVblankSemControlEnable_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdVblankSemControlEnable__
#define dispcmnCtrlCmdVblankSemControlEnable(pDispCommon, pParams) dispcmnCtrlCmdVblankSemControlEnable_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdInternalVblankSemControlEnable_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdInternalVblankSemControlEnable__
#define dispcmnCtrlCmdInternalVblankSemControlEnable(pDispCommon, pParams) dispcmnCtrlCmdInternalVblankSemControlEnable_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdVblankSemControlDisable_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdVblankSemControlDisable__
#define dispcmnCtrlCmdVblankSemControlDisable(pDispCommon, pParams) dispcmnCtrlCmdVblankSemControlDisable_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdAccelVblankSemControl_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdAccelVblankSemControl__
#define dispcmnCtrlCmdAccelVblankSemControl(pDispCommon, pParams) dispcmnCtrlCmdAccelVblankSemControl_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpEnableVrr_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDpEnableVrr__
#define dispcmnCtrlCmdDpEnableVrr(pDispCommon, pParams) dispcmnCtrlCmdDpEnableVrr_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdClearELVBlock_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdClearELVBlock__
#define dispcmnCtrlCmdClearELVBlock(pDispCommon, pParams) dispcmnCtrlCmdClearELVBlock_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSpecificDisplayChange_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSpecificDisplayChange__
#define dispcmnCtrlCmdSpecificDisplayChange(pDispCommon, pParams) dispcmnCtrlCmdSpecificDisplayChange_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDfpGetSpreadSpectrum_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDfpGetSpreadSpectrum__
#define dispcmnCtrlCmdDfpGetSpreadSpectrum(pDispCommon, pParams) dispcmnCtrlCmdDfpGetSpreadSpectrum_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDfpGetLcdGpioPinNum_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDfpGetLcdGpioPinNum__
#define dispcmnCtrlCmdDfpGetLcdGpioPinNum(pDispCommon, pParams) dispcmnCtrlCmdDfpGetLcdGpioPinNum_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpAuxchI2cTransferCtrl_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDpAuxchI2cTransferCtrl__
#define dispcmnCtrlCmdDpAuxchI2cTransferCtrl(pDispCommon, pParams) dispcmnCtrlCmdDpAuxchI2cTransferCtrl_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpASSRCtrl_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDpASSRCtrl__
#define dispcmnCtrlCmdDpASSRCtrl(pDispCommon, pParams) dispcmnCtrlCmdDpASSRCtrl_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpSetEcf_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDpSetEcf__
#define dispcmnCtrlCmdDpSetEcf(pDispCommon, pCtrlEcfParams) dispcmnCtrlCmdDpSetEcf_DISPATCH(pDispCommon, pCtrlEcfParams)
#define dispcmnCtrlCmdDfpRecordChannelRegisters_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDfpRecordChannelRegisters__
#define dispcmnCtrlCmdDfpRecordChannelRegisters(pDispCommon, pParams) dispcmnCtrlCmdDfpRecordChannelRegisters_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSpecificGetBacklightBrightness_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSpecificGetBacklightBrightness__
#define dispcmnCtrlCmdSpecificGetBacklightBrightness(pDispCommon, pAllHeadMaskParams) dispcmnCtrlCmdSpecificGetBacklightBrightness_DISPATCH(pDispCommon, pAllHeadMaskParams)
#define dispcmnCtrlCmdSpecificSetBacklightBrightness_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSpecificSetBacklightBrightness__
#define dispcmnCtrlCmdSpecificSetBacklightBrightness(pDispCommon, pParams) dispcmnCtrlCmdSpecificSetBacklightBrightness_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdPsrGetSrPanelInfo_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdPsrGetSrPanelInfo__
#define dispcmnCtrlCmdPsrGetSrPanelInfo(pDispCommon, pParams) dispcmnCtrlCmdPsrGetSrPanelInfo_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDfpSwitchDispMux_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDfpSwitchDispMux__
#define dispcmnCtrlCmdDfpSwitchDispMux(pDispCommon, pParams) dispcmnCtrlCmdDfpSwitchDispMux_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdInternalDfpSwitchDispMux_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdInternalDfpSwitchDispMux__
#define dispcmnCtrlCmdInternalDfpSwitchDispMux(pDispCommon, pParams) dispcmnCtrlCmdInternalDfpSwitchDispMux_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDfpRunPreDispMuxOperations_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDfpRunPreDispMuxOperations__
#define dispcmnCtrlCmdDfpRunPreDispMuxOperations(pDispCommon, pParams) dispcmnCtrlCmdDfpRunPreDispMuxOperations_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDfpRunPostDispMuxOperations_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDfpRunPostDispMuxOperations__
#define dispcmnCtrlCmdDfpRunPostDispMuxOperations(pDispCommon, pParams) dispcmnCtrlCmdDfpRunPostDispMuxOperations_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDfpGetDispMuxStatus_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDfpGetDispMuxStatus__
#define dispcmnCtrlCmdDfpGetDispMuxStatus(pDispCommon, pParams) dispcmnCtrlCmdDfpGetDispMuxStatus_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdInternalDfpGetDispMuxStatus_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdInternalDfpGetDispMuxStatus__
#define dispcmnCtrlCmdInternalDfpGetDispMuxStatus(pDispCommon, pParams) dispcmnCtrlCmdInternalDfpGetDispMuxStatus_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDfpInternalLcdOverdrive_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDfpInternalLcdOverdrive__
#define dispcmnCtrlCmdDfpInternalLcdOverdrive(pDispCommon, pParams) dispcmnCtrlCmdDfpInternalLcdOverdrive_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDfpVariableBacklightCtrl_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDfpVariableBacklightCtrl__
#define dispcmnCtrlCmdDfpVariableBacklightCtrl(pDispCommon, pParams) dispcmnCtrlCmdDfpVariableBacklightCtrl_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSystemExecuteAcpiMethod_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSystemExecuteAcpiMethod__
#define dispcmnCtrlCmdSystemExecuteAcpiMethod(pDispCommon, pAcpiMethodParams) dispcmnCtrlCmdSystemExecuteAcpiMethod_DISPATCH(pDispCommon, pAcpiMethodParams)
#define dispcmnCtrlCmdSystemGetAcpiIdMap_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSystemGetAcpiIdMap__
#define dispcmnCtrlCmdSystemGetAcpiIdMap(pDispCommon, pAcpiIdMapParams) dispcmnCtrlCmdSystemGetAcpiIdMap_DISPATCH(pDispCommon, pAcpiIdMapParams)
#define dispcmnCtrlCmdSystemAcpiSubsystemActivated_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSystemAcpiSubsystemActivated__
#define dispcmnCtrlCmdSystemAcpiSubsystemActivated(pDispCommon, pParams) dispcmnCtrlCmdSystemAcpiSubsystemActivated_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSpecificSetAcpiIdMapping_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSpecificSetAcpiIdMapping__
#define dispcmnCtrlCmdSpecificSetAcpiIdMapping(pDispCommon, pParams) dispcmnCtrlCmdSpecificSetAcpiIdMapping_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSpecificGetAcpiDodDisplayPortAttachment_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSpecificGetAcpiDodDisplayPortAttachment__
#define dispcmnCtrlCmdSpecificGetAcpiDodDisplayPortAttachment(pDispCommon, pParams) dispcmnCtrlCmdSpecificGetAcpiDodDisplayPortAttachment_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSystemGetCapsV2_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSystemGetCapsV2__
#define dispcmnCtrlCmdSystemGetCapsV2(pDispCommon, pCapsParams) dispcmnCtrlCmdSystemGetCapsV2_DISPATCH(pDispCommon, pCapsParams)
#define dispcmnCtrlCmdSystemGetNumHeads_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSystemGetNumHeads__
#define dispcmnCtrlCmdSystemGetNumHeads(pDispCommon, pNumHeadsParams) dispcmnCtrlCmdSystemGetNumHeads_DISPATCH(pDispCommon, pNumHeadsParams)
#define dispcmnCtrlCmdSystemGetScanline_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSystemGetScanline__
#define dispcmnCtrlCmdSystemGetScanline(pDispCommon, pScanlineParams) dispcmnCtrlCmdSystemGetScanline_DISPATCH(pDispCommon, pScanlineParams)
#define dispcmnCtrlCmdSystemGetSuppported_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSystemGetSuppported__
#define dispcmnCtrlCmdSystemGetSuppported(pDispCommon, pSupportedParams) dispcmnCtrlCmdSystemGetSuppported_DISPATCH(pDispCommon, pSupportedParams)
#define dispcmnCtrlCmdSystemGetConnectState_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSystemGetConnectState__
#define dispcmnCtrlCmdSystemGetConnectState(pDispCommon, pConnectParams) dispcmnCtrlCmdSystemGetConnectState_DISPATCH(pDispCommon, pConnectParams)
#define dispcmnCtrlCmdSystemGetHotplugUnplugState_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSystemGetHotplugUnplugState__
#define dispcmnCtrlCmdSystemGetHotplugUnplugState(pDispCommon, pHotplugParams) dispcmnCtrlCmdSystemGetHotplugUnplugState_DISPATCH(pDispCommon, pHotplugParams)
#define dispcmnCtrlCmdInternalGetHotplugUnplugState_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdInternalGetHotplugUnplugState__
#define dispcmnCtrlCmdInternalGetHotplugUnplugState(pDispCommon, pHotplugParams) dispcmnCtrlCmdInternalGetHotplugUnplugState_DISPATCH(pDispCommon, pHotplugParams)
#define dispcmnCtrlCmdSystemGetHeadRoutingMap_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSystemGetHeadRoutingMap__
#define dispcmnCtrlCmdSystemGetHeadRoutingMap(pDispCommon, pMapParams) dispcmnCtrlCmdSystemGetHeadRoutingMap_DISPATCH(pDispCommon, pMapParams)
#define dispcmnCtrlCmdSystemGetActive_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSystemGetActive__
#define dispcmnCtrlCmdSystemGetActive(pDispCommon, pActiveParams) dispcmnCtrlCmdSystemGetActive_DISPATCH(pDispCommon, pActiveParams)
#define dispcmnCtrlCmdSystemGetBootDisplays_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSystemGetBootDisplays__
#define dispcmnCtrlCmdSystemGetBootDisplays(pDispCommon, pParams) dispcmnCtrlCmdSystemGetBootDisplays_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSystemQueryDisplayIdsWithMux_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSystemQueryDisplayIdsWithMux__
#define dispcmnCtrlCmdSystemQueryDisplayIdsWithMux(pDispCommon, pParams) dispcmnCtrlCmdSystemQueryDisplayIdsWithMux_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSystemCheckSidebandI2cSupport_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSystemCheckSidebandI2cSupport__
#define dispcmnCtrlCmdSystemCheckSidebandI2cSupport(pDispCommon, pParams) dispcmnCtrlCmdSystemCheckSidebandI2cSupport_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSystemAllocateDisplayBandwidth_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSystemAllocateDisplayBandwidth__
#define dispcmnCtrlCmdSystemAllocateDisplayBandwidth(pDispCommon, pParams) dispcmnCtrlCmdSystemAllocateDisplayBandwidth_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSystemGetHotplugConfig_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSystemGetHotplugConfig__
#define dispcmnCtrlCmdSystemGetHotplugConfig(pDispCommon, pHotplugParams) dispcmnCtrlCmdSystemGetHotplugConfig_DISPATCH(pDispCommon, pHotplugParams)
#define dispcmnCtrlCmdSystemGetHotplugEventConfig_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSystemGetHotplugEventConfig__
#define dispcmnCtrlCmdSystemGetHotplugEventConfig(pDispCommon, pParams) dispcmnCtrlCmdSystemGetHotplugEventConfig_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSystemSetHotplugEventConfig_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSystemSetHotplugEventConfig__
#define dispcmnCtrlCmdSystemSetHotplugEventConfig(pDispCommon, pParams) dispcmnCtrlCmdSystemSetHotplugEventConfig_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSystemArmLightweightSupervisor_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSystemArmLightweightSupervisor__
#define dispcmnCtrlCmdSystemArmLightweightSupervisor(pDispCommon, pParams) dispcmnCtrlCmdSystemArmLightweightSupervisor_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSystemConfigVrrPstateSwitch_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSystemConfigVrrPstateSwitch__
#define dispcmnCtrlCmdSystemConfigVrrPstateSwitch(pDispCommon, pParams) dispcmnCtrlCmdSystemConfigVrrPstateSwitch_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSpecificGetType_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSpecificGetType__
#define dispcmnCtrlCmdSpecificGetType(pDispCommon, pDisplayTypeParams) dispcmnCtrlCmdSpecificGetType_DISPATCH(pDispCommon, pDisplayTypeParams)
#define dispcmnCtrlCmdSpecificGetEdidV2_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSpecificGetEdidV2__
#define dispcmnCtrlCmdSpecificGetEdidV2(pDispCommon, pEdidParams) dispcmnCtrlCmdSpecificGetEdidV2_DISPATCH(pDispCommon, pEdidParams)
#define dispcmnCtrlCmdSpecificSetEdidV2_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSpecificSetEdidV2__
#define dispcmnCtrlCmdSpecificSetEdidV2(pDispCommon, pEdidParams) dispcmnCtrlCmdSpecificSetEdidV2_DISPATCH(pDispCommon, pEdidParams)
#define dispcmnCtrlCmdSpecificFakeDevice_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSpecificFakeDevice__
#define dispcmnCtrlCmdSpecificFakeDevice(pDispCommon, pTestParams) dispcmnCtrlCmdSpecificFakeDevice_DISPATCH(pDispCommon, pTestParams)
#define dispcmnCtrlCmdSpecificGetConnectorData_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSpecificGetConnectorData__
#define dispcmnCtrlCmdSpecificGetConnectorData(pDispCommon, pConnectorParams) dispcmnCtrlCmdSpecificGetConnectorData_DISPATCH(pDispCommon, pConnectorParams)
#define dispcmnCtrlCmdSpecificSetHdmiEnable_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSpecificSetHdmiEnable__
#define dispcmnCtrlCmdSpecificSetHdmiEnable(pDispCommon, pParams) dispcmnCtrlCmdSpecificSetHdmiEnable_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSpecificCtrlHdmi_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSpecificCtrlHdmi__
#define dispcmnCtrlCmdSpecificCtrlHdmi(pDispCommon, pParams) dispcmnCtrlCmdSpecificCtrlHdmi_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSpecificGetAllHeadMask_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSpecificGetAllHeadMask__
#define dispcmnCtrlCmdSpecificGetAllHeadMask(pDispCommon, pAllHeadMaskParams) dispcmnCtrlCmdSpecificGetAllHeadMask_DISPATCH(pDispCommon, pAllHeadMaskParams)
#define dispcmnCtrlCmdSpecificSetOdPacket_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSpecificSetOdPacket__
#define dispcmnCtrlCmdSpecificSetOdPacket(pDispCommon, pParams) dispcmnCtrlCmdSpecificSetOdPacket_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSpecificAcquireSharedGenericPacket_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSpecificAcquireSharedGenericPacket__
#define dispcmnCtrlCmdSpecificAcquireSharedGenericPacket(pDispCommon, pParams) dispcmnCtrlCmdSpecificAcquireSharedGenericPacket_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSpecificSetSharedGenericPacket_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSpecificSetSharedGenericPacket__
#define dispcmnCtrlCmdSpecificSetSharedGenericPacket(pDispCommon, pParams) dispcmnCtrlCmdSpecificSetSharedGenericPacket_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSpecificReleaseSharedGenericPacket_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSpecificReleaseSharedGenericPacket__
#define dispcmnCtrlCmdSpecificReleaseSharedGenericPacket(pDispCommon, pParams) dispcmnCtrlCmdSpecificReleaseSharedGenericPacket_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSpecificSetOdPacketCtrl_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSpecificSetOdPacketCtrl__
#define dispcmnCtrlCmdSpecificSetOdPacketCtrl(pDispCommon, pParams) dispcmnCtrlCmdSpecificSetOdPacketCtrl_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSpecificOrGetInfo_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSpecificOrGetInfo__
#define dispcmnCtrlCmdSpecificOrGetInfo(pDispCommon, pParams) dispcmnCtrlCmdSpecificOrGetInfo_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSpecificGetPclkLimit_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSpecificGetPclkLimit__
#define dispcmnCtrlCmdSpecificGetPclkLimit(pDispCommon, pParams) dispcmnCtrlCmdSpecificGetPclkLimit_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSpecificSetHdmiSinkCaps_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSpecificSetHdmiSinkCaps__
#define dispcmnCtrlCmdSpecificSetHdmiSinkCaps(pDispCommon, pParams) dispcmnCtrlCmdSpecificSetHdmiSinkCaps_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSpecificSetMonitorPower_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSpecificSetMonitorPower__
#define dispcmnCtrlCmdSpecificSetMonitorPower(pDispCommon, setMonitorPowerParams) dispcmnCtrlCmdSpecificSetMonitorPower_DISPATCH(pDispCommon, setMonitorPowerParams)
#define dispcmnCtrlCmdSpecificSetHdmiFrlLinkConfig_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSpecificSetHdmiFrlLinkConfig__
#define dispcmnCtrlCmdSpecificSetHdmiFrlLinkConfig(pDispCommon, pParams) dispcmnCtrlCmdSpecificSetHdmiFrlLinkConfig_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSpecificApplyEdidOverrideV2_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSpecificApplyEdidOverrideV2__
#define dispcmnCtrlCmdSpecificApplyEdidOverrideV2(pDispCommon, pEdidOverrideParams) dispcmnCtrlCmdSpecificApplyEdidOverrideV2_DISPATCH(pDispCommon, pEdidOverrideParams)
#define dispcmnCtrlCmdSpecificGetI2cPortid_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSpecificGetI2cPortid__
#define dispcmnCtrlCmdSpecificGetI2cPortid(pDispCommon, pParams) dispcmnCtrlCmdSpecificGetI2cPortid_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSpecificGetHdmiGpuCaps_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSpecificGetHdmiGpuCaps__
#define dispcmnCtrlCmdSpecificGetHdmiGpuCaps(pDispCommon, pParams) dispcmnCtrlCmdSpecificGetHdmiGpuCaps_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSpecificGetHdmiScdcData_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSpecificGetHdmiScdcData__
#define dispcmnCtrlCmdSpecificGetHdmiScdcData(pDispCommon, pParams) dispcmnCtrlCmdSpecificGetHdmiScdcData_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSpecificIsDirectmodeDisplay_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSpecificIsDirectmodeDisplay__
#define dispcmnCtrlCmdSpecificIsDirectmodeDisplay(pDispCommon, pParams) dispcmnCtrlCmdSpecificIsDirectmodeDisplay_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSpecificDefaultAdaptivesyncDisplay_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSpecificDefaultAdaptivesyncDisplay__
#define dispcmnCtrlCmdSpecificDefaultAdaptivesyncDisplay(pDispCommon, pParams) dispcmnCtrlCmdSpecificDefaultAdaptivesyncDisplay_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSpecificSetHdmiFrlCapacityComputation_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSpecificSetHdmiFrlCapacityComputation__
#define dispcmnCtrlCmdSpecificSetHdmiFrlCapacityComputation(pDispCommon, pParams) dispcmnCtrlCmdSpecificSetHdmiFrlCapacityComputation_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSpecificDispI2cReadWrite_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSpecificDispI2cReadWrite__
#define dispcmnCtrlCmdSpecificDispI2cReadWrite(pDispCommon, pParams) dispcmnCtrlCmdSpecificDispI2cReadWrite_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSpecificGetValidHeadWindowAssignment_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSpecificGetValidHeadWindowAssignment__
#define dispcmnCtrlCmdSpecificGetValidHeadWindowAssignment(pDispCommon, pParams) dispcmnCtrlCmdSpecificGetValidHeadWindowAssignment_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSpecificSetHdmiAudioMutestream_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdSpecificSetHdmiAudioMutestream__
#define dispcmnCtrlCmdSpecificSetHdmiAudioMutestream(pDispCommon, pParams) dispcmnCtrlCmdSpecificSetHdmiAudioMutestream_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDfpEdpDriverUnload_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDfpEdpDriverUnload__
#define dispcmnCtrlCmdDfpEdpDriverUnload(pDispCommon, pParams) dispcmnCtrlCmdDfpEdpDriverUnload_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDfpGetInfo_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDfpGetInfo__
#define dispcmnCtrlCmdDfpGetInfo(pDispCommon, pParams) dispcmnCtrlCmdDfpGetInfo_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDfpGetDisplayportDongleInfo_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDfpGetDisplayportDongleInfo__
#define dispcmnCtrlCmdDfpGetDisplayportDongleInfo(pDispCommon, pParams) dispcmnCtrlCmdDfpGetDisplayportDongleInfo_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDfpSetEldAudioCaps_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDfpSetEldAudioCaps__
#define dispcmnCtrlCmdDfpSetEldAudioCaps(pDispCommon, pEldAudioCapsParams) dispcmnCtrlCmdDfpSetEldAudioCaps_DISPATCH(pDispCommon, pEldAudioCapsParams)
#define dispcmnCtrlCmdDfpSetAudioEnable_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDfpSetAudioEnable__
#define dispcmnCtrlCmdDfpSetAudioEnable(pDispCommon, pParams) dispcmnCtrlCmdDfpSetAudioEnable_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDfpUpdateDynamicDfpCache_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDfpUpdateDynamicDfpCache__
#define dispcmnCtrlCmdDfpUpdateDynamicDfpCache(pDispCommon, pParams) dispcmnCtrlCmdDfpUpdateDynamicDfpCache_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDfpAssignSor_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDfpAssignSor__
#define dispcmnCtrlCmdDfpAssignSor(pDispCommon, pParams) dispcmnCtrlCmdDfpAssignSor_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDfpDscCrcControl_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDfpDscCrcControl__
#define dispcmnCtrlCmdDfpDscCrcControl(pDispCommon, pParams) dispcmnCtrlCmdDfpDscCrcControl_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDfpInitMuxData_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDfpInitMuxData__
#define dispcmnCtrlCmdDfpInitMuxData(pDispCommon, pParams) dispcmnCtrlCmdDfpInitMuxData_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDfpGetDsiModeTiming_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDfpGetDsiModeTiming__
#define dispcmnCtrlCmdDfpGetDsiModeTiming(pDispCommon, pParams) dispcmnCtrlCmdDfpGetDsiModeTiming_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDfpConfigTwoHeadOneOr_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDfpConfigTwoHeadOneOr__
#define dispcmnCtrlCmdDfpConfigTwoHeadOneOr(pDispCommon, pParams) dispcmnCtrlCmdDfpConfigTwoHeadOneOr_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDfpGetPadlinkMask_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDfpGetPadlinkMask__
#define dispcmnCtrlCmdDfpGetPadlinkMask(pDispCommon, pParams) dispcmnCtrlCmdDfpGetPadlinkMask_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDfpGetFixedModeTiming_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDfpGetFixedModeTiming__
#define dispcmnCtrlCmdDfpGetFixedModeTiming(pDispCommon, pParams) dispcmnCtrlCmdDfpGetFixedModeTiming_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpAuxchCtrl_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDpAuxchCtrl__
#define dispcmnCtrlCmdDpAuxchCtrl(pDispCommon, pAuxchCtrlParams) dispcmnCtrlCmdDpAuxchCtrl_DISPATCH(pDispCommon, pAuxchCtrlParams)
#define dispcmnCtrlCmdDpAuxchSetSema_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDpAuxchSetSema__
#define dispcmnCtrlCmdDpAuxchSetSema(pDispCommon, pSemaParams) dispcmnCtrlCmdDpAuxchSetSema_DISPATCH(pDispCommon, pSemaParams)
#define dispcmnCtrlCmdDpCtrl_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDpCtrl__
#define dispcmnCtrlCmdDpCtrl(pDispCommon, pParams) dispcmnCtrlCmdDpCtrl_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpGetLaneData_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDpGetLaneData__
#define dispcmnCtrlCmdDpGetLaneData(pDispCommon, pParams) dispcmnCtrlCmdDpGetLaneData_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpSetLaneData_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDpSetLaneData__
#define dispcmnCtrlCmdDpSetLaneData(pDispCommon, pParams) dispcmnCtrlCmdDpSetLaneData_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpSetTestpattern_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDpSetTestpattern__
#define dispcmnCtrlCmdDpSetTestpattern(pDispCommon, pParams) dispcmnCtrlCmdDpSetTestpattern_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpMainLinkCtrl_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDpMainLinkCtrl__
#define dispcmnCtrlCmdDpMainLinkCtrl(pDispCommon, pParams) dispcmnCtrlCmdDpMainLinkCtrl_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpSetAudioMuteStream_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDpSetAudioMuteStream__
#define dispcmnCtrlCmdDpSetAudioMuteStream(pDispCommon, pParams) dispcmnCtrlCmdDpSetAudioMuteStream_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpGetLinkConfig_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDpGetLinkConfig__
#define dispcmnCtrlCmdDpGetLinkConfig(pDispCommon, pParams) dispcmnCtrlCmdDpGetLinkConfig_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpGetEDPData_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDpGetEDPData__
#define dispcmnCtrlCmdDpGetEDPData(pDispCommon, pParams) dispcmnCtrlCmdDpGetEDPData_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpTopologyAllocateDisplayId_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDpTopologyAllocateDisplayId__
#define dispcmnCtrlCmdDpTopologyAllocateDisplayId(pDispCommon, pParams) dispcmnCtrlCmdDpTopologyAllocateDisplayId_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpTopologyFreeDisplayId_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDpTopologyFreeDisplayId__
#define dispcmnCtrlCmdDpTopologyFreeDisplayId(pDispCommon, pParams) dispcmnCtrlCmdDpTopologyFreeDisplayId_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpConfigStream_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDpConfigStream__
#define dispcmnCtrlCmdDpConfigStream(pDispCommon, pParams) dispcmnCtrlCmdDpConfigStream_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpConfigSingleHeadMultiStream_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDpConfigSingleHeadMultiStream__
#define dispcmnCtrlCmdDpConfigSingleHeadMultiStream(pDispCommon, pParams) dispcmnCtrlCmdDpConfigSingleHeadMultiStream_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpSetRateGov_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDpSetRateGov__
#define dispcmnCtrlCmdDpSetRateGov(pDispCommon, pParams) dispcmnCtrlCmdDpSetRateGov_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpSendACT_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDpSendACT__
#define dispcmnCtrlCmdDpSendACT(pDispCommon, pParams) dispcmnCtrlCmdDpSendACT_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpSetManualDisplayPort_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDpSetManualDisplayPort__
#define dispcmnCtrlCmdDpSetManualDisplayPort(pDispCommon, pParams) dispcmnCtrlCmdDpSetManualDisplayPort_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpGetCaps_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDpGetCaps__
#define dispcmnCtrlCmdDpGetCaps(pDispCommon, pParams) dispcmnCtrlCmdDpGetCaps_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpSetMSAPropertiesv2_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDpSetMSAPropertiesv2__
#define dispcmnCtrlCmdDpSetMSAPropertiesv2(pDispCommon, pParams) dispcmnCtrlCmdDpSetMSAPropertiesv2_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpSetStereoMSAProperties_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDpSetStereoMSAProperties__
#define dispcmnCtrlCmdDpSetStereoMSAProperties(pDispCommon, pParams) dispcmnCtrlCmdDpSetStereoMSAProperties_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpGenerateFakeInterrupt_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDpGenerateFakeInterrupt__
#define dispcmnCtrlCmdDpGenerateFakeInterrupt(pDispCommon, pParams) dispcmnCtrlCmdDpGenerateFakeInterrupt_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpConfigRadScratchReg_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDpConfigRadScratchReg__
#define dispcmnCtrlCmdDpConfigRadScratchReg(pDispCommon, pParams) dispcmnCtrlCmdDpConfigRadScratchReg_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpSetTriggerSelect_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDpSetTriggerSelect__
#define dispcmnCtrlCmdDpSetTriggerSelect(pDispCommon, pTriggerSelectParams) dispcmnCtrlCmdDpSetTriggerSelect_DISPATCH(pDispCommon, pTriggerSelectParams)
#define dispcmnCtrlCmdDpSetTriggerAll_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDpSetTriggerAll__
#define dispcmnCtrlCmdDpSetTriggerAll(pDispCommon, pTriggerAllParams) dispcmnCtrlCmdDpSetTriggerAll_DISPATCH(pDispCommon, pTriggerAllParams)
#define dispcmnCtrlCmdDpGetAuxLogData_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDpGetAuxLogData__
#define dispcmnCtrlCmdDpGetAuxLogData(pDispCommon, pDpAuxBufferWrapper) dispcmnCtrlCmdDpGetAuxLogData_DISPATCH(pDispCommon, pDpAuxBufferWrapper)
#define dispcmnCtrlCmdDpConfigIndexedLinkRates_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDpConfigIndexedLinkRates__
#define dispcmnCtrlCmdDpConfigIndexedLinkRates(pDispCommon, pParams) dispcmnCtrlCmdDpConfigIndexedLinkRates_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpConfigureFec_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDpConfigureFec__
#define dispcmnCtrlCmdDpConfigureFec(pDispCommon, pParams) dispcmnCtrlCmdDpConfigureFec_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpGetGenericInfoframe_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDpGetGenericInfoframe__
#define dispcmnCtrlCmdDpGetGenericInfoframe(pDispCommon, pParams) dispcmnCtrlCmdDpGetGenericInfoframe_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpGetMsaAttributes_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDpGetMsaAttributes__
#define dispcmnCtrlCmdDpGetMsaAttributes(pDispCommon, pParams) dispcmnCtrlCmdDpGetMsaAttributes_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdFrlConfigMacroPad_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdFrlConfigMacroPad__
#define dispcmnCtrlCmdFrlConfigMacroPad(pDispCommon, pParams) dispcmnCtrlCmdFrlConfigMacroPad_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpConfigMacroPad_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDpConfigMacroPad__
#define dispcmnCtrlCmdDpConfigMacroPad(pDispCommon, pParams) dispcmnCtrlCmdDpConfigMacroPad_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpSetPreemphasisDrivecurrentPostcursor2Data_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDpSetPreemphasisDrivecurrentPostcursor2Data__
#define dispcmnCtrlCmdDpSetPreemphasisDrivecurrentPostcursor2Data(pDispCommon, pParams) dispcmnCtrlCmdDpSetPreemphasisDrivecurrentPostcursor2Data_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpGetPreemphasisDrivecurrentPostcursor2Data_FNPTR(pDispCommon) pDispCommon->__dispcmnCtrlCmdDpGetPreemphasisDrivecurrentPostcursor2Data__
#define dispcmnCtrlCmdDpGetPreemphasisDrivecurrentPostcursor2Data(pDispCommon, pParams) dispcmnCtrlCmdDpGetPreemphasisDrivecurrentPostcursor2Data_DISPATCH(pDispCommon, pParams)
#define dispcmnControl_FNPTR(pDisplayApi) pDisplayApi->__nvoc_base_DisplayApi.__dispapiControl__
#define dispcmnControl(pDisplayApi, pCallContext, pParams) dispcmnControl_DISPATCH(pDisplayApi, pCallContext, pParams)
#define dispcmnControl_Prologue_FNPTR(pDisplayApi) pDisplayApi->__nvoc_base_DisplayApi.__dispapiControl_Prologue__
#define dispcmnControl_Prologue(pDisplayApi, pCallContext, pRsParams) dispcmnControl_Prologue_DISPATCH(pDisplayApi, pCallContext, pRsParams)
#define dispcmnControl_Epilogue_FNPTR(pDisplayApi) pDisplayApi->__nvoc_base_DisplayApi.__dispapiControl_Epilogue__
#define dispcmnControl_Epilogue(pDisplayApi, pCallContext, pRsParams) dispcmnControl_Epilogue_DISPATCH(pDisplayApi, pCallContext, pRsParams)
#define dispcmnAccessCallback_FNPTR(pResource) pResource->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__rmresAccessCallback__
#define dispcmnAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) dispcmnAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define dispcmnShareCallback_FNPTR(pResource) pResource->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__rmresShareCallback__
#define dispcmnShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) dispcmnShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define dispcmnGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__rmresGetMemInterMapParams__
#define dispcmnGetMemInterMapParams(pRmResource, pParams) dispcmnGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define dispcmnCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__rmresCheckMemInterUnmap__
#define dispcmnCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) dispcmnCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define dispcmnGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__rmresGetMemoryMappingDescriptor__
#define dispcmnGetMemoryMappingDescriptor(pRmResource, ppMemDesc) dispcmnGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define dispcmnControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__rmresControlSerialization_Prologue__
#define dispcmnControlSerialization_Prologue(pResource, pCallContext, pParams) dispcmnControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define dispcmnControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__rmresControlSerialization_Epilogue__
#define dispcmnControlSerialization_Epilogue(pResource, pCallContext, pParams) dispcmnControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define dispcmnCanCopy_FNPTR(pResource) pResource->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__
#define dispcmnCanCopy(pResource) dispcmnCanCopy_DISPATCH(pResource)
#define dispcmnIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsDuplicate__
#define dispcmnIsDuplicate(pResource, hMemory, pDuplicate) dispcmnIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define dispcmnPreDestruct_FNPTR(pResource) pResource->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__resPreDestruct__
#define dispcmnPreDestruct(pResource) dispcmnPreDestruct_DISPATCH(pResource)
#define dispcmnControlFilter_FNPTR(pResource) pResource->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__resControlFilter__
#define dispcmnControlFilter(pResource, pCallContext, pParams) dispcmnControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define dispcmnMap_FNPTR(pResource) pResource->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__resMap__
#define dispcmnMap(pResource, pCallContext, pParams, pCpuMapping) dispcmnMap_DISPATCH(pResource, pCallContext, pParams, pCpuMapping)
#define dispcmnUnmap_FNPTR(pResource) pResource->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmap__
#define dispcmnUnmap(pResource, pCallContext, pCpuMapping) dispcmnUnmap_DISPATCH(pResource, pCallContext, pCpuMapping)
#define dispcmnIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsPartialUnmapSupported__
#define dispcmnIsPartialUnmapSupported(pResource) dispcmnIsPartialUnmapSupported_DISPATCH(pResource)
#define dispcmnMapTo_FNPTR(pResource) pResource->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__resMapTo__
#define dispcmnMapTo(pResource, pParams) dispcmnMapTo_DISPATCH(pResource, pParams)
#define dispcmnUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmapFrom__
#define dispcmnUnmapFrom(pResource, pParams) dispcmnUnmapFrom_DISPATCH(pResource, pParams)
#define dispcmnGetRefCount_FNPTR(pResource) pResource->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__resGetRefCount__
#define dispcmnGetRefCount(pResource) dispcmnGetRefCount_DISPATCH(pResource)
#define dispcmnAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__resAddAdditionalDependants__
#define dispcmnAddAdditionalDependants(pClient, pResource, pReference) dispcmnAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define dispcmnGetNotificationListPtr_FNPTR(pNotifier) pNotifier->__nvoc_base_DisplayApi.__nvoc_base_Notifier.__notifyGetNotificationListPtr__
#define dispcmnGetNotificationListPtr(pNotifier) dispcmnGetNotificationListPtr_DISPATCH(pNotifier)
#define dispcmnGetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_DisplayApi.__nvoc_base_Notifier.__notifyGetNotificationShare__
#define dispcmnGetNotificationShare(pNotifier) dispcmnGetNotificationShare_DISPATCH(pNotifier)
#define dispcmnSetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_DisplayApi.__nvoc_base_Notifier.__notifySetNotificationShare__
#define dispcmnSetNotificationShare(pNotifier, pNotifShare) dispcmnSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define dispcmnUnregisterEvent_FNPTR(pNotifier) pNotifier->__nvoc_base_DisplayApi.__nvoc_base_Notifier.__notifyUnregisterEvent__
#define dispcmnUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) dispcmnUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define dispcmnGetOrAllocNotifShare_FNPTR(pNotifier) pNotifier->__nvoc_base_DisplayApi.__nvoc_base_Notifier.__notifyGetOrAllocNotifShare__
#define dispcmnGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) dispcmnGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)

// Dispatch functions
static inline NV_STATUS dispcmnCtrlCmdSystemGetVblankCounter_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_VBLANK_COUNTER_PARAMS *pVBCounterParams) {
    return pDispCommon->__dispcmnCtrlCmdSystemGetVblankCounter__(pDispCommon, pVBCounterParams);
}

static inline NV_STATUS dispcmnCtrlCmdSystemGetVblankEnable_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_VBLANK_ENABLE_PARAMS *pVBEnableParams) {
    return pDispCommon->__dispcmnCtrlCmdSystemGetVblankEnable__(pDispCommon, pVBEnableParams);
}

static inline NV_STATUS dispcmnCtrlCmdSystemCheckSidebandSrSupport_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_SYSTEM_CHECK_SIDEBAND_SR_SUPPORT_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSystemCheckSidebandSrSupport__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdSystemGetHotplugState_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_HOTPLUG_STATE_PARAMS *pHotplugParams) {
    return pDispCommon->__dispcmnCtrlCmdSystemGetHotplugState__(pDispCommon, pHotplugParams);
}

static inline NV_STATUS dispcmnCtrlCmdSystemGetInternalDisplays_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_INTERNAL_DISPLAYS_PARAMS *pInternalDisplaysParams) {
    return pDispCommon->__dispcmnCtrlCmdSystemGetInternalDisplays__(pDispCommon, pInternalDisplaysParams);
}

static inline NV_STATUS dispcmnCtrlCmdSystemGetConnectorTable_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_CONNECTOR_TABLE_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSystemGetConnectorTable__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdSystemVrrDisplayInfo_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_VRR_DISPLAY_INFO_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSystemVrrDisplayInfo__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdVRRSetRgLineActive_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_SYSTEM_VRR_SET_RGLINE_ACTIVE_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdVRRSetRgLineActive__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdInternalVRRSetRgLineActive_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_SYSTEM_VRR_SET_RGLINE_ACTIVE_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdInternalVRRSetRgLineActive__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdVblankSemControlEnable_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_SYSTEM_VBLANK_SEM_CONTROL_ENABLE_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdVblankSemControlEnable__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdInternalVblankSemControlEnable_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_INTERNAL_VBLANK_SEM_CONTROL_ENABLE_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdInternalVblankSemControlEnable__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdVblankSemControlDisable_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_SYSTEM_VBLANK_SEM_CONTROL_DISABLE_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdVblankSemControlDisable__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdAccelVblankSemControl_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_SYSTEM_ACCEL_VBLANK_SEM_CONTROLS_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdAccelVblankSemControl__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdDpEnableVrr_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_ENABLE_VRR_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpEnableVrr__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdClearELVBlock_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_CLEAR_ELV_BLOCK_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdClearELVBlock__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdSpecificDisplayChange_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_DISPLAY_CHANGE_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificDisplayChange__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdDfpGetSpreadSpectrum_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DFP_GET_SPREAD_SPECTRUM_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDfpGetSpreadSpectrum__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdDfpGetLcdGpioPinNum_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DFP_GET_LCD_GPIO_PIN_NUM_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDfpGetLcdGpioPinNum__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdDpAuxchI2cTransferCtrl_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DP_AUXCH_I2C_TRANSFER_CTRL_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpAuxchI2cTransferCtrl__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdDpASSRCtrl_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DP_ASSR_CTRL_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpASSRCtrl__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdDpSetEcf_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_SET_ECF_PARAMS *pCtrlEcfParams) {
    return pDispCommon->__dispcmnCtrlCmdDpSetEcf__(pDispCommon, pCtrlEcfParams);
}

static inline NV_STATUS dispcmnCtrlCmdDfpRecordChannelRegisters_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_SYSTEM_RECORD_CHANNEL_REGS_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDfpRecordChannelRegisters__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdSpecificGetBacklightBrightness_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_BACKLIGHT_BRIGHTNESS_PARAMS *pAllHeadMaskParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificGetBacklightBrightness__(pDispCommon, pAllHeadMaskParams);
}

static inline NV_STATUS dispcmnCtrlCmdSpecificSetBacklightBrightness_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_BACKLIGHT_BRIGHTNESS_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificSetBacklightBrightness__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdPsrGetSrPanelInfo_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_PSR_GET_SR_PANEL_INFO_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdPsrGetSrPanelInfo__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdDfpSwitchDispMux_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DFP_SWITCH_DISP_MUX_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDfpSwitchDispMux__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdInternalDfpSwitchDispMux_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DFP_SWITCH_DISP_MUX_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdInternalDfpSwitchDispMux__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdDfpRunPreDispMuxOperations_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DFP_RUN_PRE_DISP_MUX_OPERATIONS_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDfpRunPreDispMuxOperations__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdDfpRunPostDispMuxOperations_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DFP_RUN_POST_DISP_MUX_OPERATIONS_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDfpRunPostDispMuxOperations__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdDfpGetDispMuxStatus_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DFP_GET_DISP_MUX_STATUS_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDfpGetDispMuxStatus__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdInternalDfpGetDispMuxStatus_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DFP_GET_DISP_MUX_STATUS_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdInternalDfpGetDispMuxStatus__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdDfpInternalLcdOverdrive_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_AUXCH_OD_CTRL_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDfpInternalLcdOverdrive__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdDfpVariableBacklightCtrl_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_AUXCH_VBL_CTRL_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDfpVariableBacklightCtrl__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdSystemExecuteAcpiMethod_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_PARAMS *pAcpiMethodParams) {
    return pDispCommon->__dispcmnCtrlCmdSystemExecuteAcpiMethod__(pDispCommon, pAcpiMethodParams);
}

static inline NV_STATUS dispcmnCtrlCmdSystemGetAcpiIdMap_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_ACPI_ID_MAP_PARAMS *pAcpiIdMapParams) {
    return pDispCommon->__dispcmnCtrlCmdSystemGetAcpiIdMap__(pDispCommon, pAcpiIdMapParams);
}

static inline NV_STATUS dispcmnCtrlCmdSystemAcpiSubsystemActivated_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_ACPI_SUBSYSTEM_ACTIVATED_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSystemAcpiSubsystemActivated__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdSpecificSetAcpiIdMapping_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_SET_ACPI_ID_MAPPING_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificSetAcpiIdMapping__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdSpecificGetAcpiDodDisplayPortAttachment_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_GET_ACPI_DOD_DISPLAY_PORT_ATTACHMENT_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificGetAcpiDodDisplayPortAttachment__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdSystemGetCapsV2_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_CAPS_V2_PARAMS *pCapsParams) {
    return pDispCommon->__dispcmnCtrlCmdSystemGetCapsV2__(pDispCommon, pCapsParams);
}

static inline NV_STATUS dispcmnCtrlCmdSystemGetNumHeads_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_NUM_HEADS_PARAMS *pNumHeadsParams) {
    return pDispCommon->__dispcmnCtrlCmdSystemGetNumHeads__(pDispCommon, pNumHeadsParams);
}

static inline NV_STATUS dispcmnCtrlCmdSystemGetScanline_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_SCANLINE_PARAMS *pScanlineParams) {
    return pDispCommon->__dispcmnCtrlCmdSystemGetScanline__(pDispCommon, pScanlineParams);
}

static inline NV_STATUS dispcmnCtrlCmdSystemGetSuppported_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_SUPPORTED_PARAMS *pSupportedParams) {
    return pDispCommon->__dispcmnCtrlCmdSystemGetSuppported__(pDispCommon, pSupportedParams);
}

static inline NV_STATUS dispcmnCtrlCmdSystemGetConnectState_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_CONNECT_STATE_PARAMS *pConnectParams) {
    return pDispCommon->__dispcmnCtrlCmdSystemGetConnectState__(pDispCommon, pConnectParams);
}

static inline NV_STATUS dispcmnCtrlCmdSystemGetHotplugUnplugState_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_HOTPLUG_UNPLUG_STATE_PARAMS *pHotplugParams) {
    return pDispCommon->__dispcmnCtrlCmdSystemGetHotplugUnplugState__(pDispCommon, pHotplugParams);
}

static inline NV_STATUS dispcmnCtrlCmdInternalGetHotplugUnplugState_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_HOTPLUG_UNPLUG_STATE_PARAMS *pHotplugParams) {
    return pDispCommon->__dispcmnCtrlCmdInternalGetHotplugUnplugState__(pDispCommon, pHotplugParams);
}

static inline NV_STATUS dispcmnCtrlCmdSystemGetHeadRoutingMap_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_HEAD_ROUTING_MAP_PARAMS *pMapParams) {
    return pDispCommon->__dispcmnCtrlCmdSystemGetHeadRoutingMap__(pDispCommon, pMapParams);
}

static inline NV_STATUS dispcmnCtrlCmdSystemGetActive_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_ACTIVE_PARAMS *pActiveParams) {
    return pDispCommon->__dispcmnCtrlCmdSystemGetActive__(pDispCommon, pActiveParams);
}

static inline NV_STATUS dispcmnCtrlCmdSystemGetBootDisplays_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_BOOT_DISPLAYS_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSystemGetBootDisplays__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdSystemQueryDisplayIdsWithMux_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_SYSTEM_QUERY_DISPLAY_IDS_WITH_MUX_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSystemQueryDisplayIdsWithMux__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdSystemCheckSidebandI2cSupport_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_SYSTEM_CHECK_SIDEBAND_I2C_SUPPORT_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSystemCheckSidebandI2cSupport__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdSystemAllocateDisplayBandwidth_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_ALLOCATE_DISPLAY_BANDWIDTH_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSystemAllocateDisplayBandwidth__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdSystemGetHotplugConfig_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_SET_HOTPLUG_CONFIG_PARAMS *pHotplugParams) {
    return pDispCommon->__dispcmnCtrlCmdSystemGetHotplugConfig__(pDispCommon, pHotplugParams);
}

static inline NV_STATUS dispcmnCtrlCmdSystemGetHotplugEventConfig_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_HOTPLUG_EVENT_CONFIG_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSystemGetHotplugEventConfig__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdSystemSetHotplugEventConfig_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_HOTPLUG_EVENT_CONFIG_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSystemSetHotplugEventConfig__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdSystemArmLightweightSupervisor_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_ARM_LIGHTWEIGHT_SUPERVISOR_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSystemArmLightweightSupervisor__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdSystemConfigVrrPstateSwitch_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_CONFIG_VRR_PSTATE_SWITCH_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSystemConfigVrrPstateSwitch__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdSpecificGetType_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_GET_TYPE_PARAMS *pDisplayTypeParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificGetType__(pDispCommon, pDisplayTypeParams);
}

static inline NV_STATUS dispcmnCtrlCmdSpecificGetEdidV2_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_GET_EDID_V2_PARAMS *pEdidParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificGetEdidV2__(pDispCommon, pEdidParams);
}

static inline NV_STATUS dispcmnCtrlCmdSpecificSetEdidV2_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_SET_EDID_V2_PARAMS *pEdidParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificSetEdidV2__(pDispCommon, pEdidParams);
}

static inline NV_STATUS dispcmnCtrlCmdSpecificFakeDevice_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_SPECIFIC_FAKE_DEVICE_PARAMS *pTestParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificFakeDevice__(pDispCommon, pTestParams);
}

static inline NV_STATUS dispcmnCtrlCmdSpecificGetConnectorData_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_GET_CONNECTOR_DATA_PARAMS *pConnectorParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificGetConnectorData__(pDispCommon, pConnectorParams);
}

static inline NV_STATUS dispcmnCtrlCmdSpecificSetHdmiEnable_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_SET_HDMI_ENABLE_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificSetHdmiEnable__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdSpecificCtrlHdmi_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_CTRL_HDMI_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificCtrlHdmi__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdSpecificGetAllHeadMask_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_GET_ALL_HEAD_MASK_PARAMS *pAllHeadMaskParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificGetAllHeadMask__(pDispCommon, pAllHeadMaskParams);
}

static inline NV_STATUS dispcmnCtrlCmdSpecificSetOdPacket_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_SET_OD_PACKET_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificSetOdPacket__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdSpecificAcquireSharedGenericPacket_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_ACQUIRE_SHARED_GENERIC_PACKET_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificAcquireSharedGenericPacket__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdSpecificSetSharedGenericPacket_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_SET_SHARED_GENERIC_PACKET_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificSetSharedGenericPacket__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdSpecificReleaseSharedGenericPacket_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_RELEASE_SHARED_GENERIC_PACKET_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificReleaseSharedGenericPacket__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdSpecificSetOdPacketCtrl_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_SET_OD_PACKET_CTRL_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificSetOdPacketCtrl__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdSpecificOrGetInfo_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_OR_GET_INFO_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificOrGetInfo__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdSpecificGetPclkLimit_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_GET_PCLK_LIMIT_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificGetPclkLimit__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdSpecificSetHdmiSinkCaps_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_SET_HDMI_SINK_CAPS_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificSetHdmiSinkCaps__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdSpecificSetMonitorPower_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_SET_MONITOR_POWER_PARAMS *setMonitorPowerParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificSetMonitorPower__(pDispCommon, setMonitorPowerParams);
}

static inline NV_STATUS dispcmnCtrlCmdSpecificSetHdmiFrlLinkConfig_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_SET_HDMI_FRL_LINK_CONFIG_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificSetHdmiFrlLinkConfig__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdSpecificApplyEdidOverrideV2_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_APPLY_EDID_OVERRIDE_V2_PARAMS *pEdidOverrideParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificApplyEdidOverrideV2__(pDispCommon, pEdidOverrideParams);
}

static inline NV_STATUS dispcmnCtrlCmdSpecificGetI2cPortid_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_GET_I2C_PORTID_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificGetI2cPortid__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdSpecificGetHdmiGpuCaps_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_GET_HDMI_GPU_CAPS_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificGetHdmiGpuCaps__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdSpecificGetHdmiScdcData_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_GET_HDMI_SCDC_DATA_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificGetHdmiScdcData__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdSpecificIsDirectmodeDisplay_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_IS_DIRECTMODE_DISPLAY_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificIsDirectmodeDisplay__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdSpecificDefaultAdaptivesyncDisplay_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_DEFAULT_ADAPTIVESYNC_DISPLAY_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificDefaultAdaptivesyncDisplay__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdSpecificSetHdmiFrlCapacityComputation_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificSetHdmiFrlCapacityComputation__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdSpecificDispI2cReadWrite_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_DISP_I2C_READ_WRITE_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificDispI2cReadWrite__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdSpecificGetValidHeadWindowAssignment_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_GET_VALID_HEAD_WINDOW_ASSIGNMENT_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificGetValidHeadWindowAssignment__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdSpecificSetHdmiAudioMutestream_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_AUDIO_MUTESTREAM_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificSetHdmiAudioMutestream__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdDfpEdpDriverUnload_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DFP_EDP_DRIVER_UNLOAD_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDfpEdpDriverUnload__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdDfpGetInfo_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DFP_GET_INFO_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDfpGetInfo__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdDfpGetDisplayportDongleInfo_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DFP_GET_DISPLAYPORT_DONGLE_INFO_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDfpGetDisplayportDongleInfo__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdDfpSetEldAudioCaps_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DFP_SET_ELD_AUDIO_CAP_PARAMS *pEldAudioCapsParams) {
    return pDispCommon->__dispcmnCtrlCmdDfpSetEldAudioCaps__(pDispCommon, pEldAudioCapsParams);
}

static inline NV_STATUS dispcmnCtrlCmdDfpSetAudioEnable_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DFP_SET_AUDIO_ENABLE_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDfpSetAudioEnable__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdDfpUpdateDynamicDfpCache_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DFP_UPDATE_DYNAMIC_DFP_CACHE_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDfpUpdateDynamicDfpCache__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdDfpAssignSor_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DFP_ASSIGN_SOR_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDfpAssignSor__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdDfpDscCrcControl_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DFP_DSC_CRC_CONTROL_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDfpDscCrcControl__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdDfpInitMuxData_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DFP_INIT_MUX_DATA_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDfpInitMuxData__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdDfpGetDsiModeTiming_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DFP_GET_DSI_MODE_TIMING_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDfpGetDsiModeTiming__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdDfpConfigTwoHeadOneOr_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DFP_CONFIG_TWO_HEAD_ONE_OR_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDfpConfigTwoHeadOneOr__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdDfpGetPadlinkMask_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DFP_GET_PADLINK_MASK_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDfpGetPadlinkMask__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdDfpGetFixedModeTiming_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DFP_GET_FIXED_MODE_TIMING_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDfpGetFixedModeTiming__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdDpAuxchCtrl_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DP_AUXCH_CTRL_PARAMS *pAuxchCtrlParams) {
    return pDispCommon->__dispcmnCtrlCmdDpAuxchCtrl__(pDispCommon, pAuxchCtrlParams);
}

static inline NV_STATUS dispcmnCtrlCmdDpAuxchSetSema_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DP_AUXCH_SET_SEMA_PARAMS *pSemaParams) {
    return pDispCommon->__dispcmnCtrlCmdDpAuxchSetSema__(pDispCommon, pSemaParams);
}

static inline NV_STATUS dispcmnCtrlCmdDpCtrl_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DP_CTRL_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpCtrl__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdDpGetLaneData_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DP_LANE_DATA_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpGetLaneData__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdDpSetLaneData_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DP_LANE_DATA_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpSetLaneData__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdDpSetTestpattern_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DP_SET_TESTPATTERN_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpSetTestpattern__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdDpMainLinkCtrl_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DP_MAIN_LINK_CTRL_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpMainLinkCtrl__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdDpSetAudioMuteStream_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DP_SET_AUDIO_MUTESTREAM_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpSetAudioMuteStream__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdDpGetLinkConfig_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DP_GET_LINK_CONFIG_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpGetLinkConfig__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdDpGetEDPData_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DP_GET_EDP_DATA_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpGetEDPData__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdDpTopologyAllocateDisplayId_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_TOPOLOGY_ALLOCATE_DISPLAYID_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpTopologyAllocateDisplayId__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdDpTopologyFreeDisplayId_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_TOPOLOGY_FREE_DISPLAYID_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpTopologyFreeDisplayId__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdDpConfigStream_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_CONFIG_STREAM_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpConfigStream__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdDpConfigSingleHeadMultiStream_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_CONFIG_SINGLE_HEAD_MULTI_STREAM_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpConfigSingleHeadMultiStream__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdDpSetRateGov_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_SET_RATE_GOV_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpSetRateGov__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdDpSendACT_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_SEND_ACT_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpSendACT__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdDpSetManualDisplayPort_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_SET_MANUAL_DISPLAYPORT_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpSetManualDisplayPort__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdDpGetCaps_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_GET_CAPS_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpGetCaps__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdDpSetMSAPropertiesv2_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_SET_MSA_PROPERTIES_V2_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpSetMSAPropertiesv2__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdDpSetStereoMSAProperties_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_SET_STEREO_MSA_PROPERTIES_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpSetStereoMSAProperties__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdDpGenerateFakeInterrupt_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_GENERATE_FAKE_INTERRUPT_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpGenerateFakeInterrupt__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdDpConfigRadScratchReg_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_CONFIG_RAD_SCRATCH_REG_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpConfigRadScratchReg__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdDpSetTriggerSelect_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_SET_TRIGGER_SELECT_PARAMS *pTriggerSelectParams) {
    return pDispCommon->__dispcmnCtrlCmdDpSetTriggerSelect__(pDispCommon, pTriggerSelectParams);
}

static inline NV_STATUS dispcmnCtrlCmdDpSetTriggerAll_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_SET_TRIGGER_ALL_PARAMS *pTriggerAllParams) {
    return pDispCommon->__dispcmnCtrlCmdDpSetTriggerAll__(pDispCommon, pTriggerAllParams);
}

static inline NV_STATUS dispcmnCtrlCmdDpGetAuxLogData_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_GET_AUXLOGGER_BUFFER_DATA_PARAMS *pDpAuxBufferWrapper) {
    return pDispCommon->__dispcmnCtrlCmdDpGetAuxLogData__(pDispCommon, pDpAuxBufferWrapper);
}

static inline NV_STATUS dispcmnCtrlCmdDpConfigIndexedLinkRates_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_CONFIG_INDEXED_LINK_RATES_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpConfigIndexedLinkRates__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdDpConfigureFec_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_CONFIGURE_FEC_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpConfigureFec__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdDpGetGenericInfoframe_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DP_GET_GENERIC_INFOFRAME_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpGetGenericInfoframe__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdDpGetMsaAttributes_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DP_GET_MSA_ATTRIBUTES_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpGetMsaAttributes__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdFrlConfigMacroPad_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_FRL_CONFIG_MACRO_PAD_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdFrlConfigMacroPad__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdDpConfigMacroPad_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_CONFIG_MACRO_PAD_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpConfigMacroPad__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdDpSetPreemphasisDrivecurrentPostcursor2Data_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DP_SET_PREEMPHASIS_DRIVECURRENT_POSTCURSOR2_DATA_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpSetPreemphasisDrivecurrentPostcursor2Data__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnCtrlCmdDpGetPreemphasisDrivecurrentPostcursor2Data_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DP_GET_PREEMPHASIS_DRIVECURRENT_POSTCURSOR2_DATA_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpGetPreemphasisDrivecurrentPostcursor2Data__(pDispCommon, pParams);
}

static inline NV_STATUS dispcmnControl_DISPATCH(struct DispCommon *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pDisplayApi->__dispcmnControl__(pDisplayApi, pCallContext, pParams);
}

static inline NV_STATUS dispcmnControl_Prologue_DISPATCH(struct DispCommon *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams) {
    return pDisplayApi->__dispcmnControl_Prologue__(pDisplayApi, pCallContext, pRsParams);
}

static inline void dispcmnControl_Epilogue_DISPATCH(struct DispCommon *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams) {
    pDisplayApi->__dispcmnControl_Epilogue__(pDisplayApi, pCallContext, pRsParams);
}

static inline NvBool dispcmnAccessCallback_DISPATCH(struct DispCommon *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__dispcmnAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool dispcmnShareCallback_DISPATCH(struct DispCommon *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__dispcmnShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS dispcmnGetMemInterMapParams_DISPATCH(struct DispCommon *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__dispcmnGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS dispcmnCheckMemInterUnmap_DISPATCH(struct DispCommon *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__dispcmnCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS dispcmnGetMemoryMappingDescriptor_DISPATCH(struct DispCommon *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__dispcmnGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS dispcmnControlSerialization_Prologue_DISPATCH(struct DispCommon *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__dispcmnControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void dispcmnControlSerialization_Epilogue_DISPATCH(struct DispCommon *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__dispcmnControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool dispcmnCanCopy_DISPATCH(struct DispCommon *pResource) {
    return pResource->__dispcmnCanCopy__(pResource);
}

static inline NV_STATUS dispcmnIsDuplicate_DISPATCH(struct DispCommon *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__dispcmnIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void dispcmnPreDestruct_DISPATCH(struct DispCommon *pResource) {
    pResource->__dispcmnPreDestruct__(pResource);
}

static inline NV_STATUS dispcmnControlFilter_DISPATCH(struct DispCommon *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__dispcmnControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS dispcmnMap_DISPATCH(struct DispCommon *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pResource->__dispcmnMap__(pResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS dispcmnUnmap_DISPATCH(struct DispCommon *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pResource->__dispcmnUnmap__(pResource, pCallContext, pCpuMapping);
}

static inline NvBool dispcmnIsPartialUnmapSupported_DISPATCH(struct DispCommon *pResource) {
    return pResource->__dispcmnIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS dispcmnMapTo_DISPATCH(struct DispCommon *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__dispcmnMapTo__(pResource, pParams);
}

static inline NV_STATUS dispcmnUnmapFrom_DISPATCH(struct DispCommon *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__dispcmnUnmapFrom__(pResource, pParams);
}

static inline NvU32 dispcmnGetRefCount_DISPATCH(struct DispCommon *pResource) {
    return pResource->__dispcmnGetRefCount__(pResource);
}

static inline void dispcmnAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct DispCommon *pResource, RsResourceRef *pReference) {
    pResource->__dispcmnAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline PEVENTNOTIFICATION * dispcmnGetNotificationListPtr_DISPATCH(struct DispCommon *pNotifier) {
    return pNotifier->__dispcmnGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare * dispcmnGetNotificationShare_DISPATCH(struct DispCommon *pNotifier) {
    return pNotifier->__dispcmnGetNotificationShare__(pNotifier);
}

static inline void dispcmnSetNotificationShare_DISPATCH(struct DispCommon *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__dispcmnSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NV_STATUS dispcmnUnregisterEvent_DISPATCH(struct DispCommon *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__dispcmnUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS dispcmnGetOrAllocNotifShare_DISPATCH(struct DispCommon *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__dispcmnGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

NV_STATUS dispcmnCtrlCmdSystemGetVblankCounter_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_VBLANK_COUNTER_PARAMS *pVBCounterParams);

NV_STATUS dispcmnCtrlCmdSystemGetVblankEnable_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_VBLANK_ENABLE_PARAMS *pVBEnableParams);

NV_STATUS dispcmnCtrlCmdSystemCheckSidebandSrSupport_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_SYSTEM_CHECK_SIDEBAND_SR_SUPPORT_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdSystemGetHotplugState_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_HOTPLUG_STATE_PARAMS *pHotplugParams);

NV_STATUS dispcmnCtrlCmdSystemGetInternalDisplays_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_INTERNAL_DISPLAYS_PARAMS *pInternalDisplaysParams);

NV_STATUS dispcmnCtrlCmdSystemGetConnectorTable_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_CONNECTOR_TABLE_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdSystemVrrDisplayInfo_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_VRR_DISPLAY_INFO_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdVRRSetRgLineActive_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_SYSTEM_VRR_SET_RGLINE_ACTIVE_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdInternalVRRSetRgLineActive_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_SYSTEM_VRR_SET_RGLINE_ACTIVE_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdVblankSemControlEnable_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_SYSTEM_VBLANK_SEM_CONTROL_ENABLE_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdInternalVblankSemControlEnable_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_INTERNAL_VBLANK_SEM_CONTROL_ENABLE_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdVblankSemControlDisable_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_SYSTEM_VBLANK_SEM_CONTROL_DISABLE_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdAccelVblankSemControl_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_SYSTEM_ACCEL_VBLANK_SEM_CONTROLS_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdDpEnableVrr_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_ENABLE_VRR_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdClearELVBlock_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_CLEAR_ELV_BLOCK_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdSpecificDisplayChange_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_DISPLAY_CHANGE_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdDfpGetSpreadSpectrum_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DFP_GET_SPREAD_SPECTRUM_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdDfpGetLcdGpioPinNum_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DFP_GET_LCD_GPIO_PIN_NUM_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdDpAuxchI2cTransferCtrl_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DP_AUXCH_I2C_TRANSFER_CTRL_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdDpASSRCtrl_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DP_ASSR_CTRL_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdDpSetEcf_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_SET_ECF_PARAMS *pCtrlEcfParams);

NV_STATUS dispcmnCtrlCmdDfpRecordChannelRegisters_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_SYSTEM_RECORD_CHANNEL_REGS_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdSpecificGetBacklightBrightness_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_BACKLIGHT_BRIGHTNESS_PARAMS *pAllHeadMaskParams);

NV_STATUS dispcmnCtrlCmdSpecificSetBacklightBrightness_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_BACKLIGHT_BRIGHTNESS_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdPsrGetSrPanelInfo_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_PSR_GET_SR_PANEL_INFO_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdDfpSwitchDispMux_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DFP_SWITCH_DISP_MUX_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdInternalDfpSwitchDispMux_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DFP_SWITCH_DISP_MUX_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdDfpRunPreDispMuxOperations_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DFP_RUN_PRE_DISP_MUX_OPERATIONS_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdDfpRunPostDispMuxOperations_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DFP_RUN_POST_DISP_MUX_OPERATIONS_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdDfpGetDispMuxStatus_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DFP_GET_DISP_MUX_STATUS_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdInternalDfpGetDispMuxStatus_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DFP_GET_DISP_MUX_STATUS_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdDfpInternalLcdOverdrive_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_AUXCH_OD_CTRL_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdDfpVariableBacklightCtrl_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_AUXCH_VBL_CTRL_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdSystemExecuteAcpiMethod_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_PARAMS *pAcpiMethodParams);

NV_STATUS dispcmnCtrlCmdSystemGetAcpiIdMap_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_ACPI_ID_MAP_PARAMS *pAcpiIdMapParams);

NV_STATUS dispcmnCtrlCmdSystemAcpiSubsystemActivated_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_ACPI_SUBSYSTEM_ACTIVATED_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdSpecificSetAcpiIdMapping_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_SET_ACPI_ID_MAPPING_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdSpecificGetAcpiDodDisplayPortAttachment_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_GET_ACPI_DOD_DISPLAY_PORT_ATTACHMENT_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdSystemGetCapsV2_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_CAPS_V2_PARAMS *pCapsParams);

NV_STATUS dispcmnCtrlCmdSystemGetNumHeads_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_NUM_HEADS_PARAMS *pNumHeadsParams);

NV_STATUS dispcmnCtrlCmdSystemGetScanline_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_SCANLINE_PARAMS *pScanlineParams);

NV_STATUS dispcmnCtrlCmdSystemGetSuppported_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_SUPPORTED_PARAMS *pSupportedParams);

NV_STATUS dispcmnCtrlCmdSystemGetConnectState_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_CONNECT_STATE_PARAMS *pConnectParams);

NV_STATUS dispcmnCtrlCmdSystemGetHotplugUnplugState_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_HOTPLUG_UNPLUG_STATE_PARAMS *pHotplugParams);

NV_STATUS dispcmnCtrlCmdInternalGetHotplugUnplugState_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_HOTPLUG_UNPLUG_STATE_PARAMS *pHotplugParams);

NV_STATUS dispcmnCtrlCmdSystemGetHeadRoutingMap_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_HEAD_ROUTING_MAP_PARAMS *pMapParams);

NV_STATUS dispcmnCtrlCmdSystemGetActive_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_ACTIVE_PARAMS *pActiveParams);

NV_STATUS dispcmnCtrlCmdSystemGetBootDisplays_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_BOOT_DISPLAYS_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdSystemQueryDisplayIdsWithMux_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_SYSTEM_QUERY_DISPLAY_IDS_WITH_MUX_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdSystemCheckSidebandI2cSupport_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_SYSTEM_CHECK_SIDEBAND_I2C_SUPPORT_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdSystemAllocateDisplayBandwidth_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_ALLOCATE_DISPLAY_BANDWIDTH_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdSystemGetHotplugConfig_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_SET_HOTPLUG_CONFIG_PARAMS *pHotplugParams);

NV_STATUS dispcmnCtrlCmdSystemGetHotplugEventConfig_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_HOTPLUG_EVENT_CONFIG_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdSystemSetHotplugEventConfig_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_HOTPLUG_EVENT_CONFIG_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdSystemArmLightweightSupervisor_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_ARM_LIGHTWEIGHT_SUPERVISOR_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdSystemConfigVrrPstateSwitch_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_CONFIG_VRR_PSTATE_SWITCH_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdSpecificGetType_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_GET_TYPE_PARAMS *pDisplayTypeParams);

NV_STATUS dispcmnCtrlCmdSpecificGetEdidV2_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_GET_EDID_V2_PARAMS *pEdidParams);

NV_STATUS dispcmnCtrlCmdSpecificSetEdidV2_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_SET_EDID_V2_PARAMS *pEdidParams);

NV_STATUS dispcmnCtrlCmdSpecificFakeDevice_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_SPECIFIC_FAKE_DEVICE_PARAMS *pTestParams);

NV_STATUS dispcmnCtrlCmdSpecificGetConnectorData_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_GET_CONNECTOR_DATA_PARAMS *pConnectorParams);

NV_STATUS dispcmnCtrlCmdSpecificSetHdmiEnable_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_SET_HDMI_ENABLE_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdSpecificCtrlHdmi_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_CTRL_HDMI_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdSpecificGetAllHeadMask_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_GET_ALL_HEAD_MASK_PARAMS *pAllHeadMaskParams);

NV_STATUS dispcmnCtrlCmdSpecificSetOdPacket_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_SET_OD_PACKET_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdSpecificAcquireSharedGenericPacket_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_ACQUIRE_SHARED_GENERIC_PACKET_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdSpecificSetSharedGenericPacket_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_SET_SHARED_GENERIC_PACKET_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdSpecificReleaseSharedGenericPacket_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_RELEASE_SHARED_GENERIC_PACKET_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdSpecificSetOdPacketCtrl_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_SET_OD_PACKET_CTRL_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdSpecificOrGetInfo_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_OR_GET_INFO_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdSpecificGetPclkLimit_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_GET_PCLK_LIMIT_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdSpecificSetHdmiSinkCaps_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_SET_HDMI_SINK_CAPS_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdSpecificSetMonitorPower_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_SET_MONITOR_POWER_PARAMS *setMonitorPowerParams);

NV_STATUS dispcmnCtrlCmdSpecificSetHdmiFrlLinkConfig_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_SET_HDMI_FRL_LINK_CONFIG_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdSpecificApplyEdidOverrideV2_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_APPLY_EDID_OVERRIDE_V2_PARAMS *pEdidOverrideParams);

NV_STATUS dispcmnCtrlCmdSpecificGetI2cPortid_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_GET_I2C_PORTID_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdSpecificGetHdmiGpuCaps_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_GET_HDMI_GPU_CAPS_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdSpecificGetHdmiScdcData_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_GET_HDMI_SCDC_DATA_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdSpecificIsDirectmodeDisplay_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_IS_DIRECTMODE_DISPLAY_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdSpecificDefaultAdaptivesyncDisplay_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_DEFAULT_ADAPTIVESYNC_DISPLAY_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdSpecificSetHdmiFrlCapacityComputation_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdSpecificDispI2cReadWrite_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_DISP_I2C_READ_WRITE_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdSpecificGetValidHeadWindowAssignment_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_GET_VALID_HEAD_WINDOW_ASSIGNMENT_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdSpecificSetHdmiAudioMutestream_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_AUDIO_MUTESTREAM_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdDfpEdpDriverUnload_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DFP_EDP_DRIVER_UNLOAD_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdDfpGetInfo_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DFP_GET_INFO_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdDfpGetDisplayportDongleInfo_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DFP_GET_DISPLAYPORT_DONGLE_INFO_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdDfpSetEldAudioCaps_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DFP_SET_ELD_AUDIO_CAP_PARAMS *pEldAudioCapsParams);

NV_STATUS dispcmnCtrlCmdDfpSetAudioEnable_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DFP_SET_AUDIO_ENABLE_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdDfpUpdateDynamicDfpCache_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DFP_UPDATE_DYNAMIC_DFP_CACHE_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdDfpAssignSor_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DFP_ASSIGN_SOR_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdDfpDscCrcControl_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DFP_DSC_CRC_CONTROL_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdDfpInitMuxData_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DFP_INIT_MUX_DATA_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdDfpGetDsiModeTiming_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DFP_GET_DSI_MODE_TIMING_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdDfpConfigTwoHeadOneOr_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DFP_CONFIG_TWO_HEAD_ONE_OR_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdDfpGetPadlinkMask_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DFP_GET_PADLINK_MASK_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdDfpGetFixedModeTiming_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DFP_GET_FIXED_MODE_TIMING_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdDpAuxchCtrl_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DP_AUXCH_CTRL_PARAMS *pAuxchCtrlParams);

NV_STATUS dispcmnCtrlCmdDpAuxchSetSema_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DP_AUXCH_SET_SEMA_PARAMS *pSemaParams);

NV_STATUS dispcmnCtrlCmdDpCtrl_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DP_CTRL_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdDpGetLaneData_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DP_LANE_DATA_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdDpSetLaneData_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DP_LANE_DATA_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdDpSetTestpattern_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DP_SET_TESTPATTERN_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdDpMainLinkCtrl_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DP_MAIN_LINK_CTRL_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdDpSetAudioMuteStream_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DP_SET_AUDIO_MUTESTREAM_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdDpGetLinkConfig_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DP_GET_LINK_CONFIG_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdDpGetEDPData_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DP_GET_EDP_DATA_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdDpTopologyAllocateDisplayId_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_TOPOLOGY_ALLOCATE_DISPLAYID_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdDpTopologyFreeDisplayId_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_TOPOLOGY_FREE_DISPLAYID_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdDpConfigStream_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_CONFIG_STREAM_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdDpConfigSingleHeadMultiStream_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_CONFIG_SINGLE_HEAD_MULTI_STREAM_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdDpSetRateGov_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_SET_RATE_GOV_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdDpSendACT_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_SEND_ACT_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdDpSetManualDisplayPort_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_SET_MANUAL_DISPLAYPORT_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdDpGetCaps_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_GET_CAPS_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdDpSetMSAPropertiesv2_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_SET_MSA_PROPERTIES_V2_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdDpSetStereoMSAProperties_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_SET_STEREO_MSA_PROPERTIES_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdDpGenerateFakeInterrupt_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_GENERATE_FAKE_INTERRUPT_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdDpConfigRadScratchReg_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_CONFIG_RAD_SCRATCH_REG_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdDpSetTriggerSelect_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_SET_TRIGGER_SELECT_PARAMS *pTriggerSelectParams);

NV_STATUS dispcmnCtrlCmdDpSetTriggerAll_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_SET_TRIGGER_ALL_PARAMS *pTriggerAllParams);

NV_STATUS dispcmnCtrlCmdDpGetAuxLogData_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_GET_AUXLOGGER_BUFFER_DATA_PARAMS *pDpAuxBufferWrapper);

NV_STATUS dispcmnCtrlCmdDpConfigIndexedLinkRates_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_CONFIG_INDEXED_LINK_RATES_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdDpConfigureFec_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_CONFIGURE_FEC_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdDpGetGenericInfoframe_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DP_GET_GENERIC_INFOFRAME_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdDpGetMsaAttributes_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DP_GET_MSA_ATTRIBUTES_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdFrlConfigMacroPad_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_FRL_CONFIG_MACRO_PAD_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdDpConfigMacroPad_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_CONFIG_MACRO_PAD_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdDpSetPreemphasisDrivecurrentPostcursor2Data_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DP_SET_PREEMPHASIS_DRIVECURRENT_POSTCURSOR2_DATA_PARAMS *pParams);

NV_STATUS dispcmnCtrlCmdDpGetPreemphasisDrivecurrentPostcursor2Data_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DP_GET_PREEMPHASIS_DRIVECURRENT_POSTCURSOR2_DATA_PARAMS *pParams);

NV_STATUS dispcmnConstruct_IMPL(struct DispCommon *arg_pDispCommon, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_dispcmnConstruct(arg_pDispCommon, arg_pCallContext, arg_pParams) dispcmnConstruct_IMPL(arg_pDispCommon, arg_pCallContext, arg_pParams)
NV_STATUS dispcmnGetByHandle_IMPL(struct RsClient *pClient, NvHandle hDispCommon, struct DispCommon **ppDispCommon);

#define dispcmnGetByHandle(pClient, hDispCommon, ppDispCommon) dispcmnGetByHandle_IMPL(pClient, hDispCommon, ppDispCommon)
void dispcmnGetByDevice_IMPL(struct RsClient *pClient, NvHandle hDevice, struct DispCommon **ppDispCommon);

#define dispcmnGetByDevice(pClient, hDevice, ppDispCommon) dispcmnGetByDevice_IMPL(pClient, hDevice, ppDispCommon)
#undef PRIVATE_FIELD


// ****************************************************************************
//                            Deprecated Definitions
// ****************************************************************************

/**
 * @warning This function is deprecated! Please use dispchnGetByHandle.
 */
NV_STATUS CliFindDispChannelInfo(NvHandle, NvHandle, struct DispChannel **ppDispChannel, NvHandle*);

/**
 * @warning This function is deprecated! Please use dispcmnGetByHandle.
 */
NvBool CliGetDispCommonInfo(NvHandle, NvHandle, struct DisplayApi **);

/**
 * @warning This function is deprecated! Please use dispobjGetByHandle.
 */
NvBool CliGetDispInfo(NvHandle, NvHandle, struct DisplayApi **);

/**
 * @warning This function is deprecated! Please use dispobjGetByHandle.
 */
struct DisplayApi *CliGetDispFromDispHandle(NvHandle hClient, NvHandle hDisp);

#endif // DISP_OBJS_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_DISP_OBJS_NVOC_H_
