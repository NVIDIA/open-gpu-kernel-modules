#ifndef _G_DISP_OBJS_NVOC_H_
#define _G_DISP_OBJS_NVOC_H_
#include "nvoc/runtime.h"

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

#include "g_disp_objs_nvoc.h"

#ifndef DISP_OBJS_H
#define DISP_OBJS_H

#include "rmapi/event.h"
#include "rmapi/resource.h"

#include "gpu/gpu_halspec.h"

#include "ctrl/ctrl0073.h"
#include "ctrl/ctrl5070/ctrl5070event.h"
#include "ctrl/ctrl5070/ctrl5070or.h"
#include "ctrl/ctrl5070/ctrl5070seq.h"
#include "ctrl/ctrl5070/ctrl5070system.h"
#include "ctrl/ctrlc370/ctrlc370chnc.h"
#include "ctrl/ctrlc370/ctrlc370event.h"
#include "ctrl/ctrlc370/ctrlc370rg.h"
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
#ifdef NVOC_DISP_OBJS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct DisplayApi {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct RmResource __nvoc_base_RmResource;
    struct Notifier __nvoc_base_Notifier;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct INotifier *__nvoc_pbase_INotifier;
    struct Notifier *__nvoc_pbase_Notifier;
    struct DisplayApi *__nvoc_pbase_DisplayApi;
    NV_STATUS (*__dispapiControl__)(struct DisplayApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__dispapiControl_Prologue__)(struct DisplayApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    void (*__dispapiControl_Epilogue__)(struct DisplayApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__dispapiShareCallback__)(struct DisplayApi *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__dispapiCheckMemInterUnmap__)(struct DisplayApi *, NvBool);
    NV_STATUS (*__dispapiMapTo__)(struct DisplayApi *, RS_RES_MAP_TO_PARAMS *);
    NvBool (*__dispapiAccessCallback__)(struct DisplayApi *, struct RsClient *, void *, RsAccessRight);
    void (*__dispapiSetNotificationShare__)(struct DisplayApi *, struct NotifShare *);
    NvU32 (*__dispapiGetRefCount__)(struct DisplayApi *);
    void (*__dispapiAddAdditionalDependants__)(struct RsClient *, struct DisplayApi *, RsResourceRef *);
    NV_STATUS (*__dispapiUnmapFrom__)(struct DisplayApi *, RS_RES_UNMAP_FROM_PARAMS *);
    NV_STATUS (*__dispapiControlLookup__)(struct DisplayApi *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NV_STATUS (*__dispapiUnmap__)(struct DisplayApi *, struct CALL_CONTEXT *, RsCpuMapping *);
    NV_STATUS (*__dispapiGetMemInterMapParams__)(struct DisplayApi *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__dispapiGetMemoryMappingDescriptor__)(struct DisplayApi *, struct MEMORY_DESCRIPTOR **);
    NV_STATUS (*__dispapiControlFilter__)(struct DisplayApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__dispapiUnregisterEvent__)(struct DisplayApi *, NvHandle, NvHandle, NvHandle, NvHandle);
    NV_STATUS (*__dispapiControlSerialization_Prologue__)(struct DisplayApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__dispapiCanCopy__)(struct DisplayApi *);
    void (*__dispapiPreDestruct__)(struct DisplayApi *);
    NV_STATUS (*__dispapiIsDuplicate__)(struct DisplayApi *, NvHandle, NvBool *);
    void (*__dispapiControlSerialization_Epilogue__)(struct DisplayApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    PEVENTNOTIFICATION *(*__dispapiGetNotificationListPtr__)(struct DisplayApi *);
    struct NotifShare *(*__dispapiGetNotificationShare__)(struct DisplayApi *);
    NV_STATUS (*__dispapiMap__)(struct DisplayApi *, struct CALL_CONTEXT *, RS_CPU_MAP_PARAMS *, RsCpuMapping *);
    NV_STATUS (*__dispapiGetOrAllocNotifShare__)(struct DisplayApi *, NvHandle, NvHandle, struct NotifShare **);
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

#define dispapiControl(pDisplayApi, pCallContext, pParams) dispapiControl_DISPATCH(pDisplayApi, pCallContext, pParams)
#define dispapiControl_Prologue(pDisplayApi, pCallContext, pRsParams) dispapiControl_Prologue_DISPATCH(pDisplayApi, pCallContext, pRsParams)
#define dispapiControl_Epilogue(pDisplayApi, pCallContext, pRsParams) dispapiControl_Epilogue_DISPATCH(pDisplayApi, pCallContext, pRsParams)
#define dispapiShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) dispapiShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define dispapiCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) dispapiCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define dispapiMapTo(pResource, pParams) dispapiMapTo_DISPATCH(pResource, pParams)
#define dispapiAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) dispapiAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define dispapiSetNotificationShare(pNotifier, pNotifShare) dispapiSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define dispapiGetRefCount(pResource) dispapiGetRefCount_DISPATCH(pResource)
#define dispapiAddAdditionalDependants(pClient, pResource, pReference) dispapiAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define dispapiUnmapFrom(pResource, pParams) dispapiUnmapFrom_DISPATCH(pResource, pParams)
#define dispapiControlLookup(pResource, pParams, ppEntry) dispapiControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define dispapiUnmap(pResource, pCallContext, pCpuMapping) dispapiUnmap_DISPATCH(pResource, pCallContext, pCpuMapping)
#define dispapiGetMemInterMapParams(pRmResource, pParams) dispapiGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define dispapiGetMemoryMappingDescriptor(pRmResource, ppMemDesc) dispapiGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define dispapiControlFilter(pResource, pCallContext, pParams) dispapiControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define dispapiUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) dispapiUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define dispapiControlSerialization_Prologue(pResource, pCallContext, pParams) dispapiControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define dispapiCanCopy(pResource) dispapiCanCopy_DISPATCH(pResource)
#define dispapiPreDestruct(pResource) dispapiPreDestruct_DISPATCH(pResource)
#define dispapiIsDuplicate(pResource, hMemory, pDuplicate) dispapiIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define dispapiControlSerialization_Epilogue(pResource, pCallContext, pParams) dispapiControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define dispapiGetNotificationListPtr(pNotifier) dispapiGetNotificationListPtr_DISPATCH(pNotifier)
#define dispapiGetNotificationShare(pNotifier) dispapiGetNotificationShare_DISPATCH(pNotifier)
#define dispapiMap(pResource, pCallContext, pParams, pCpuMapping) dispapiMap_DISPATCH(pResource, pCallContext, pParams, pCpuMapping)
#define dispapiGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) dispapiGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)
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

static inline NV_STATUS dispapiControl_DISPATCH(struct DisplayApi *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pDisplayApi->__dispapiControl__(pDisplayApi, pCallContext, pParams);
}

NV_STATUS dispapiControl_Prologue_IMPL(struct DisplayApi *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams);

static inline NV_STATUS dispapiControl_Prologue_DISPATCH(struct DisplayApi *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams) {
    return pDisplayApi->__dispapiControl_Prologue__(pDisplayApi, pCallContext, pRsParams);
}

void dispapiControl_Epilogue_IMPL(struct DisplayApi *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams);

static inline void dispapiControl_Epilogue_DISPATCH(struct DisplayApi *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams) {
    pDisplayApi->__dispapiControl_Epilogue__(pDisplayApi, pCallContext, pRsParams);
}

static inline NvBool dispapiShareCallback_DISPATCH(struct DisplayApi *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__dispapiShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS dispapiCheckMemInterUnmap_DISPATCH(struct DisplayApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__dispapiCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS dispapiMapTo_DISPATCH(struct DisplayApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__dispapiMapTo__(pResource, pParams);
}

static inline NvBool dispapiAccessCallback_DISPATCH(struct DisplayApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__dispapiAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline void dispapiSetNotificationShare_DISPATCH(struct DisplayApi *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__dispapiSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NvU32 dispapiGetRefCount_DISPATCH(struct DisplayApi *pResource) {
    return pResource->__dispapiGetRefCount__(pResource);
}

static inline void dispapiAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct DisplayApi *pResource, RsResourceRef *pReference) {
    pResource->__dispapiAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS dispapiUnmapFrom_DISPATCH(struct DisplayApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__dispapiUnmapFrom__(pResource, pParams);
}

static inline NV_STATUS dispapiControlLookup_DISPATCH(struct DisplayApi *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__dispapiControlLookup__(pResource, pParams, ppEntry);
}

static inline NV_STATUS dispapiUnmap_DISPATCH(struct DisplayApi *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pResource->__dispapiUnmap__(pResource, pCallContext, pCpuMapping);
}

static inline NV_STATUS dispapiGetMemInterMapParams_DISPATCH(struct DisplayApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__dispapiGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS dispapiGetMemoryMappingDescriptor_DISPATCH(struct DisplayApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__dispapiGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS dispapiControlFilter_DISPATCH(struct DisplayApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__dispapiControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS dispapiUnregisterEvent_DISPATCH(struct DisplayApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__dispapiUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS dispapiControlSerialization_Prologue_DISPATCH(struct DisplayApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__dispapiControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool dispapiCanCopy_DISPATCH(struct DisplayApi *pResource) {
    return pResource->__dispapiCanCopy__(pResource);
}

static inline void dispapiPreDestruct_DISPATCH(struct DisplayApi *pResource) {
    pResource->__dispapiPreDestruct__(pResource);
}

static inline NV_STATUS dispapiIsDuplicate_DISPATCH(struct DisplayApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__dispapiIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void dispapiControlSerialization_Epilogue_DISPATCH(struct DisplayApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__dispapiControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline PEVENTNOTIFICATION *dispapiGetNotificationListPtr_DISPATCH(struct DisplayApi *pNotifier) {
    return pNotifier->__dispapiGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare *dispapiGetNotificationShare_DISPATCH(struct DisplayApi *pNotifier) {
    return pNotifier->__dispapiGetNotificationShare__(pNotifier);
}

static inline NV_STATUS dispapiMap_DISPATCH(struct DisplayApi *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pResource->__dispapiMap__(pResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS dispapiGetOrAllocNotifShare_DISPATCH(struct DisplayApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__dispapiGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

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
#ifdef NVOC_DISP_OBJS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct DispObject {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct DisplayApi __nvoc_base_DisplayApi;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct INotifier *__nvoc_pbase_INotifier;
    struct Notifier *__nvoc_pbase_Notifier;
    struct DisplayApi *__nvoc_pbase_DisplayApi;
    struct DispObject *__nvoc_pbase_DispObject;
    NV_STATUS (*__dispobjCtrlCmdGetPinsetCount__)(struct DispObject *, NV5070_CTRL_GET_PINSET_COUNT_PARAMS *);
    NV_STATUS (*__dispobjCtrlCmdGetPinsetPeer__)(struct DispObject *, NV5070_CTRL_GET_PINSET_PEER_PARAMS *);
    NV_STATUS (*__dispobjCtrlCmdSetMempoolWARForBlitTearing__)(struct DispObject *, NV5070_CTRL_SET_MEMPOOL_WAR_FOR_BLIT_TEARING_PARAMS *);
    NV_STATUS (*__dispobjCtrlCmdGetPinsetLockpins__)(struct DispObject *, NV5070_CTRL_GET_PINSET_LOCKPINS_PARAMS *);
    NV_STATUS (*__dispobjCtrlCmdGetFrameLockHeaderLockPins__)(struct DispObject *, NV5070_CTRL_GET_FRAMELOCK_HEADER_LOCKPINS_PARAMS *);
    NV_STATUS (*__dispobjCtrlCmdGetPiorSeqCtl__)(struct DispObject *, NV5070_CTRL_CMD_GET_PIOR_SEQ_CTL_PARAMS *);
    NV_STATUS (*__dispobjCtrlCmdSetPiorSeqCtl__)(struct DispObject *, NV5070_CTRL_CMD_SET_PIOR_SEQ_CTL_PARAMS *);
    NV_STATUS (*__dispobjCtrlCmdGetPiorOpMode__)(struct DispObject *, NV5070_CTRL_CMD_GET_PIOR_OP_MODE_PARAMS *);
    NV_STATUS (*__dispobjCtrlCmdSetPiorOpMode__)(struct DispObject *, NV5070_CTRL_CMD_SET_PIOR_OP_MODE_PARAMS *);
    NV_STATUS (*__dispobjCtrlCmdEventSetMemoryNotifies__)(struct DispObject *, NV5070_CTRL_EVENT_SET_MEMORY_NOTIFIES_PARAMS *);
    NV_STATUS (*__dispobjCtrlCmdSetRmFreeFlags__)(struct DispObject *, NV5070_CTRL_SET_RMFREE_FLAGS_PARAMS *);
    NV_STATUS (*__dispobjCtrlCmdIMPSetGetParameter__)(struct DispObject *, NV5070_CTRL_IMP_SET_GET_PARAMETER_PARAMS *);
    NV_STATUS (*__dispobjCtrlCmdGetRgStatus__)(struct DispObject *, NV5070_CTRL_CMD_GET_RG_STATUS_PARAMS *);
    NV_STATUS (*__dispobjCtrlCmdGetRgUnderflowProp__)(struct DispObject *, NV5070_CTRL_CMD_GET_RG_UNDERFLOW_PROP_PARAMS *);
    NV_STATUS (*__dispobjCtrlCmdSetRgUnderflowProp__)(struct DispObject *, NV5070_CTRL_CMD_SET_RG_UNDERFLOW_PROP_PARAMS *);
    NV_STATUS (*__dispobjCtrlCmdGetRgFliplockProp__)(struct DispObject *, NV5070_CTRL_CMD_GET_RG_FLIPLOCK_PROP_PARAMS *);
    NV_STATUS (*__dispobjCtrlCmdSetRgFliplockProp__)(struct DispObject *, NV5070_CTRL_CMD_SET_RG_FLIPLOCK_PROP_PARAMS *);
    NV_STATUS (*__dispobjCtrlCmdGetRgConnectedLockpinStateless__)(struct DispObject *, NV5070_CTRL_GET_RG_CONNECTED_LOCKPIN_STATELESS_PARAMS *);
    NV_STATUS (*__dispobjCtrlCmdGetRgScanLine__)(struct DispObject *, NV5070_CTRL_CMD_GET_RG_SCAN_LINE_PARAMS *);
    NV_STATUS (*__dispobjCtrlCmdGetSorSeqCtl__)(struct DispObject *, NV5070_CTRL_CMD_GET_SOR_SEQ_CTL_PARAMS *);
    NV_STATUS (*__dispobjCtrlCmdSetSorSeqCtl__)(struct DispObject *, NV5070_CTRL_CMD_SET_SOR_SEQ_CTL_PARAMS *);
    NV_STATUS (*__dispobjCtrlCmdSeqProgSpeed__)(struct DispObject *, NV5070_CTRL_SEQ_PROG_SPEED_PARAMS *);
    NV_STATUS (*__dispobjCtrlCmdGetSorPwm__)(struct DispObject *, NV5070_CTRL_CMD_GET_SOR_PWM_PARAMS *);
    NV_STATUS (*__dispobjCtrlCmdSetSorPwm__)(struct DispObject *, NV5070_CTRL_CMD_SET_SOR_PWM_PARAMS *);
    NV_STATUS (*__dispobjCtrlCmdGetSorOpMode__)(struct DispObject *, NV5070_CTRL_CMD_GET_SOR_OP_MODE_PARAMS *);
    NV_STATUS (*__dispobjCtrlCmdSetSorOpMode__)(struct DispObject *, NV5070_CTRL_CMD_SET_SOR_OP_MODE_PARAMS *);
    NV_STATUS (*__dispobjCtrlCmdSetSorFlushMode__)(struct DispObject *, NV5070_CTRL_SET_SOR_FLUSH_MODE_PARAMS *);
    NV_STATUS (*__dispobjCtrlCmdSystemGetCapsV2__)(struct DispObject *, NV5070_CTRL_SYSTEM_GET_CAPS_V2_PARAMS *);
    NV_STATUS (*__dispobjCtrlCmdEventSetTrigger__)(struct DispObject *);
    NvBool (*__dispobjShareCallback__)(struct DispObject *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__dispobjCheckMemInterUnmap__)(struct DispObject *, NvBool);
    NV_STATUS (*__dispobjMapTo__)(struct DispObject *, RS_RES_MAP_TO_PARAMS *);
    NvBool (*__dispobjAccessCallback__)(struct DispObject *, struct RsClient *, void *, RsAccessRight);
    void (*__dispobjSetNotificationShare__)(struct DispObject *, struct NotifShare *);
    NvU32 (*__dispobjGetRefCount__)(struct DispObject *);
    void (*__dispobjAddAdditionalDependants__)(struct RsClient *, struct DispObject *, RsResourceRef *);
    NV_STATUS (*__dispobjControl_Prologue__)(struct DispObject *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__dispobjUnmapFrom__)(struct DispObject *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__dispobjControl_Epilogue__)(struct DispObject *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__dispobjControlLookup__)(struct DispObject *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NV_STATUS (*__dispobjControl__)(struct DispObject *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__dispobjUnmap__)(struct DispObject *, struct CALL_CONTEXT *, RsCpuMapping *);
    NV_STATUS (*__dispobjGetMemInterMapParams__)(struct DispObject *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__dispobjGetMemoryMappingDescriptor__)(struct DispObject *, struct MEMORY_DESCRIPTOR **);
    NV_STATUS (*__dispobjControlFilter__)(struct DispObject *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__dispobjUnregisterEvent__)(struct DispObject *, NvHandle, NvHandle, NvHandle, NvHandle);
    NV_STATUS (*__dispobjControlSerialization_Prologue__)(struct DispObject *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__dispobjCanCopy__)(struct DispObject *);
    void (*__dispobjPreDestruct__)(struct DispObject *);
    NV_STATUS (*__dispobjIsDuplicate__)(struct DispObject *, NvHandle, NvBool *);
    void (*__dispobjControlSerialization_Epilogue__)(struct DispObject *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    PEVENTNOTIFICATION *(*__dispobjGetNotificationListPtr__)(struct DispObject *);
    struct NotifShare *(*__dispobjGetNotificationShare__)(struct DispObject *);
    NV_STATUS (*__dispobjMap__)(struct DispObject *, struct CALL_CONTEXT *, RS_CPU_MAP_PARAMS *, RsCpuMapping *);
    NV_STATUS (*__dispobjGetOrAllocNotifShare__)(struct DispObject *, NvHandle, NvHandle, struct NotifShare **);
    NvU32 rmFreeFlags;
};

#ifndef __NVOC_CLASS_DispObject_TYPEDEF__
#define __NVOC_CLASS_DispObject_TYPEDEF__
typedef struct DispObject DispObject;
#endif /* __NVOC_CLASS_DispObject_TYPEDEF__ */

#ifndef __nvoc_class_id_DispObject
#define __nvoc_class_id_DispObject 0x999839
#endif /* __nvoc_class_id_DispObject */

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

#define dispobjCtrlCmdGetPinsetCount(pDispObject, pParams) dispobjCtrlCmdGetPinsetCount_DISPATCH(pDispObject, pParams)
#define dispobjCtrlCmdGetPinsetPeer(pDispObject, pParams) dispobjCtrlCmdGetPinsetPeer_DISPATCH(pDispObject, pParams)
#define dispobjCtrlCmdSetMempoolWARForBlitTearing(pDispObject, pParams) dispobjCtrlCmdSetMempoolWARForBlitTearing_DISPATCH(pDispObject, pParams)
#define dispobjCtrlCmdGetPinsetLockpins(pDispObject, pParams) dispobjCtrlCmdGetPinsetLockpins_DISPATCH(pDispObject, pParams)
#define dispobjCtrlCmdGetFrameLockHeaderLockPins(pDispObject, pParams) dispobjCtrlCmdGetFrameLockHeaderLockPins_DISPATCH(pDispObject, pParams)
#define dispobjCtrlCmdGetPiorSeqCtl(pDispObject, pParams) dispobjCtrlCmdGetPiorSeqCtl_DISPATCH(pDispObject, pParams)
#define dispobjCtrlCmdSetPiorSeqCtl(pDispObject, pParams) dispobjCtrlCmdSetPiorSeqCtl_DISPATCH(pDispObject, pParams)
#define dispobjCtrlCmdGetPiorOpMode(pDispObject, pParams) dispobjCtrlCmdGetPiorOpMode_DISPATCH(pDispObject, pParams)
#define dispobjCtrlCmdSetPiorOpMode(pDispObject, pParams) dispobjCtrlCmdSetPiorOpMode_DISPATCH(pDispObject, pParams)
#define dispobjCtrlCmdEventSetMemoryNotifies(pDispObject, pSetMemoryNotifiesParams) dispobjCtrlCmdEventSetMemoryNotifies_DISPATCH(pDispObject, pSetMemoryNotifiesParams)
#define dispobjCtrlCmdSetRmFreeFlags(pDispObject, pParams) dispobjCtrlCmdSetRmFreeFlags_DISPATCH(pDispObject, pParams)
#define dispobjCtrlCmdIMPSetGetParameter(pDispObject, pImpSetGetParams) dispobjCtrlCmdIMPSetGetParameter_DISPATCH(pDispObject, pImpSetGetParams)
#define dispobjCtrlCmdGetRgStatus(pDispObject, pParams) dispobjCtrlCmdGetRgStatus_DISPATCH(pDispObject, pParams)
#define dispobjCtrlCmdGetRgUnderflowProp(pDispObject, pParams) dispobjCtrlCmdGetRgUnderflowProp_DISPATCH(pDispObject, pParams)
#define dispobjCtrlCmdSetRgUnderflowProp(pDispObject, pParams) dispobjCtrlCmdSetRgUnderflowProp_DISPATCH(pDispObject, pParams)
#define dispobjCtrlCmdGetRgFliplockProp(pDispObject, pParams) dispobjCtrlCmdGetRgFliplockProp_DISPATCH(pDispObject, pParams)
#define dispobjCtrlCmdSetRgFliplockProp(pDispObject, pParams) dispobjCtrlCmdSetRgFliplockProp_DISPATCH(pDispObject, pParams)
#define dispobjCtrlCmdGetRgConnectedLockpinStateless(pDispObject, pParams) dispobjCtrlCmdGetRgConnectedLockpinStateless_DISPATCH(pDispObject, pParams)
#define dispobjCtrlCmdGetRgScanLine(pDispObject, pParams) dispobjCtrlCmdGetRgScanLine_DISPATCH(pDispObject, pParams)
#define dispobjCtrlCmdGetSorSeqCtl(pDispObject, pParams) dispobjCtrlCmdGetSorSeqCtl_DISPATCH(pDispObject, pParams)
#define dispobjCtrlCmdSetSorSeqCtl(pDispObject, pParams) dispobjCtrlCmdSetSorSeqCtl_DISPATCH(pDispObject, pParams)
#define dispobjCtrlCmdSeqProgSpeed(pDispObject, pParams) dispobjCtrlCmdSeqProgSpeed_DISPATCH(pDispObject, pParams)
#define dispobjCtrlCmdGetSorPwm(pDispObject, pParams) dispobjCtrlCmdGetSorPwm_DISPATCH(pDispObject, pParams)
#define dispobjCtrlCmdSetSorPwm(pDispObject, pParams) dispobjCtrlCmdSetSorPwm_DISPATCH(pDispObject, pParams)
#define dispobjCtrlCmdGetSorOpMode(pDispObject, pParams) dispobjCtrlCmdGetSorOpMode_DISPATCH(pDispObject, pParams)
#define dispobjCtrlCmdSetSorOpMode(pDispObject, pParams) dispobjCtrlCmdSetSorOpMode_DISPATCH(pDispObject, pParams)
#define dispobjCtrlCmdSetSorFlushMode(pDispObject, pParams) dispobjCtrlCmdSetSorFlushMode_DISPATCH(pDispObject, pParams)
#define dispobjCtrlCmdSystemGetCapsV2(pDispObject, pCapsParams) dispobjCtrlCmdSystemGetCapsV2_DISPATCH(pDispObject, pCapsParams)
#define dispobjCtrlCmdEventSetTrigger(pDispObject) dispobjCtrlCmdEventSetTrigger_DISPATCH(pDispObject)
#define dispobjShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) dispobjShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define dispobjCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) dispobjCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define dispobjMapTo(pResource, pParams) dispobjMapTo_DISPATCH(pResource, pParams)
#define dispobjAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) dispobjAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define dispobjSetNotificationShare(pNotifier, pNotifShare) dispobjSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define dispobjGetRefCount(pResource) dispobjGetRefCount_DISPATCH(pResource)
#define dispobjAddAdditionalDependants(pClient, pResource, pReference) dispobjAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define dispobjControl_Prologue(pDisplayApi, pCallContext, pRsParams) dispobjControl_Prologue_DISPATCH(pDisplayApi, pCallContext, pRsParams)
#define dispobjUnmapFrom(pResource, pParams) dispobjUnmapFrom_DISPATCH(pResource, pParams)
#define dispobjControl_Epilogue(pDisplayApi, pCallContext, pRsParams) dispobjControl_Epilogue_DISPATCH(pDisplayApi, pCallContext, pRsParams)
#define dispobjControlLookup(pResource, pParams, ppEntry) dispobjControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define dispobjControl(pDisplayApi, pCallContext, pParams) dispobjControl_DISPATCH(pDisplayApi, pCallContext, pParams)
#define dispobjUnmap(pResource, pCallContext, pCpuMapping) dispobjUnmap_DISPATCH(pResource, pCallContext, pCpuMapping)
#define dispobjGetMemInterMapParams(pRmResource, pParams) dispobjGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define dispobjGetMemoryMappingDescriptor(pRmResource, ppMemDesc) dispobjGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define dispobjControlFilter(pResource, pCallContext, pParams) dispobjControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define dispobjUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) dispobjUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define dispobjControlSerialization_Prologue(pResource, pCallContext, pParams) dispobjControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define dispobjCanCopy(pResource) dispobjCanCopy_DISPATCH(pResource)
#define dispobjPreDestruct(pResource) dispobjPreDestruct_DISPATCH(pResource)
#define dispobjIsDuplicate(pResource, hMemory, pDuplicate) dispobjIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define dispobjControlSerialization_Epilogue(pResource, pCallContext, pParams) dispobjControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define dispobjGetNotificationListPtr(pNotifier) dispobjGetNotificationListPtr_DISPATCH(pNotifier)
#define dispobjGetNotificationShare(pNotifier) dispobjGetNotificationShare_DISPATCH(pNotifier)
#define dispobjMap(pResource, pCallContext, pParams, pCpuMapping) dispobjMap_DISPATCH(pResource, pCallContext, pParams, pCpuMapping)
#define dispobjGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) dispobjGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)
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

static inline NV_STATUS dispobjCtrlCmdGetPinsetCount_DISPATCH(struct DispObject *pDispObject, NV5070_CTRL_GET_PINSET_COUNT_PARAMS *pParams) {
    return pDispObject->__dispobjCtrlCmdGetPinsetCount__(pDispObject, pParams);
}

NV_STATUS dispobjCtrlCmdGetPinsetPeer_IMPL(struct DispObject *pDispObject, NV5070_CTRL_GET_PINSET_PEER_PARAMS *pParams);

static inline NV_STATUS dispobjCtrlCmdGetPinsetPeer_DISPATCH(struct DispObject *pDispObject, NV5070_CTRL_GET_PINSET_PEER_PARAMS *pParams) {
    return pDispObject->__dispobjCtrlCmdGetPinsetPeer__(pDispObject, pParams);
}

NV_STATUS dispobjCtrlCmdSetMempoolWARForBlitTearing_IMPL(struct DispObject *pDispObject, NV5070_CTRL_SET_MEMPOOL_WAR_FOR_BLIT_TEARING_PARAMS *pParams);

static inline NV_STATUS dispobjCtrlCmdSetMempoolWARForBlitTearing_DISPATCH(struct DispObject *pDispObject, NV5070_CTRL_SET_MEMPOOL_WAR_FOR_BLIT_TEARING_PARAMS *pParams) {
    return pDispObject->__dispobjCtrlCmdSetMempoolWARForBlitTearing__(pDispObject, pParams);
}

NV_STATUS dispobjCtrlCmdGetPinsetLockpins_IMPL(struct DispObject *pDispObject, NV5070_CTRL_GET_PINSET_LOCKPINS_PARAMS *pParams);

static inline NV_STATUS dispobjCtrlCmdGetPinsetLockpins_DISPATCH(struct DispObject *pDispObject, NV5070_CTRL_GET_PINSET_LOCKPINS_PARAMS *pParams) {
    return pDispObject->__dispobjCtrlCmdGetPinsetLockpins__(pDispObject, pParams);
}

NV_STATUS dispobjCtrlCmdGetFrameLockHeaderLockPins_IMPL(struct DispObject *pDispObject, NV5070_CTRL_GET_FRAMELOCK_HEADER_LOCKPINS_PARAMS *pParams);

static inline NV_STATUS dispobjCtrlCmdGetFrameLockHeaderLockPins_DISPATCH(struct DispObject *pDispObject, NV5070_CTRL_GET_FRAMELOCK_HEADER_LOCKPINS_PARAMS *pParams) {
    return pDispObject->__dispobjCtrlCmdGetFrameLockHeaderLockPins__(pDispObject, pParams);
}

NV_STATUS dispobjCtrlCmdGetPiorSeqCtl_IMPL(struct DispObject *pDispObject, NV5070_CTRL_CMD_GET_PIOR_SEQ_CTL_PARAMS *pParams);

static inline NV_STATUS dispobjCtrlCmdGetPiorSeqCtl_DISPATCH(struct DispObject *pDispObject, NV5070_CTRL_CMD_GET_PIOR_SEQ_CTL_PARAMS *pParams) {
    return pDispObject->__dispobjCtrlCmdGetPiorSeqCtl__(pDispObject, pParams);
}

NV_STATUS dispobjCtrlCmdSetPiorSeqCtl_IMPL(struct DispObject *pDispObject, NV5070_CTRL_CMD_SET_PIOR_SEQ_CTL_PARAMS *pParams);

static inline NV_STATUS dispobjCtrlCmdSetPiorSeqCtl_DISPATCH(struct DispObject *pDispObject, NV5070_CTRL_CMD_SET_PIOR_SEQ_CTL_PARAMS *pParams) {
    return pDispObject->__dispobjCtrlCmdSetPiorSeqCtl__(pDispObject, pParams);
}

NV_STATUS dispobjCtrlCmdGetPiorOpMode_IMPL(struct DispObject *pDispObject, NV5070_CTRL_CMD_GET_PIOR_OP_MODE_PARAMS *pParams);

static inline NV_STATUS dispobjCtrlCmdGetPiorOpMode_DISPATCH(struct DispObject *pDispObject, NV5070_CTRL_CMD_GET_PIOR_OP_MODE_PARAMS *pParams) {
    return pDispObject->__dispobjCtrlCmdGetPiorOpMode__(pDispObject, pParams);
}

NV_STATUS dispobjCtrlCmdSetPiorOpMode_IMPL(struct DispObject *pDispObject, NV5070_CTRL_CMD_SET_PIOR_OP_MODE_PARAMS *pParams);

static inline NV_STATUS dispobjCtrlCmdSetPiorOpMode_DISPATCH(struct DispObject *pDispObject, NV5070_CTRL_CMD_SET_PIOR_OP_MODE_PARAMS *pParams) {
    return pDispObject->__dispobjCtrlCmdSetPiorOpMode__(pDispObject, pParams);
}

NV_STATUS dispobjCtrlCmdEventSetMemoryNotifies_IMPL(struct DispObject *pDispObject, NV5070_CTRL_EVENT_SET_MEMORY_NOTIFIES_PARAMS *pSetMemoryNotifiesParams);

static inline NV_STATUS dispobjCtrlCmdEventSetMemoryNotifies_DISPATCH(struct DispObject *pDispObject, NV5070_CTRL_EVENT_SET_MEMORY_NOTIFIES_PARAMS *pSetMemoryNotifiesParams) {
    return pDispObject->__dispobjCtrlCmdEventSetMemoryNotifies__(pDispObject, pSetMemoryNotifiesParams);
}

NV_STATUS dispobjCtrlCmdSetRmFreeFlags_IMPL(struct DispObject *pDispObject, NV5070_CTRL_SET_RMFREE_FLAGS_PARAMS *pParams);

static inline NV_STATUS dispobjCtrlCmdSetRmFreeFlags_DISPATCH(struct DispObject *pDispObject, NV5070_CTRL_SET_RMFREE_FLAGS_PARAMS *pParams) {
    return pDispObject->__dispobjCtrlCmdSetRmFreeFlags__(pDispObject, pParams);
}

NV_STATUS dispobjCtrlCmdIMPSetGetParameter_IMPL(struct DispObject *pDispObject, NV5070_CTRL_IMP_SET_GET_PARAMETER_PARAMS *pImpSetGetParams);

static inline NV_STATUS dispobjCtrlCmdIMPSetGetParameter_DISPATCH(struct DispObject *pDispObject, NV5070_CTRL_IMP_SET_GET_PARAMETER_PARAMS *pImpSetGetParams) {
    return pDispObject->__dispobjCtrlCmdIMPSetGetParameter__(pDispObject, pImpSetGetParams);
}

NV_STATUS dispobjCtrlCmdGetRgStatus_IMPL(struct DispObject *pDispObject, NV5070_CTRL_CMD_GET_RG_STATUS_PARAMS *pParams);

static inline NV_STATUS dispobjCtrlCmdGetRgStatus_DISPATCH(struct DispObject *pDispObject, NV5070_CTRL_CMD_GET_RG_STATUS_PARAMS *pParams) {
    return pDispObject->__dispobjCtrlCmdGetRgStatus__(pDispObject, pParams);
}

NV_STATUS dispobjCtrlCmdGetRgUnderflowProp_IMPL(struct DispObject *pDispObject, NV5070_CTRL_CMD_GET_RG_UNDERFLOW_PROP_PARAMS *pParams);

static inline NV_STATUS dispobjCtrlCmdGetRgUnderflowProp_DISPATCH(struct DispObject *pDispObject, NV5070_CTRL_CMD_GET_RG_UNDERFLOW_PROP_PARAMS *pParams) {
    return pDispObject->__dispobjCtrlCmdGetRgUnderflowProp__(pDispObject, pParams);
}

NV_STATUS dispobjCtrlCmdSetRgUnderflowProp_IMPL(struct DispObject *pDispObject, NV5070_CTRL_CMD_SET_RG_UNDERFLOW_PROP_PARAMS *pParams);

static inline NV_STATUS dispobjCtrlCmdSetRgUnderflowProp_DISPATCH(struct DispObject *pDispObject, NV5070_CTRL_CMD_SET_RG_UNDERFLOW_PROP_PARAMS *pParams) {
    return pDispObject->__dispobjCtrlCmdSetRgUnderflowProp__(pDispObject, pParams);
}

NV_STATUS dispobjCtrlCmdGetRgFliplockProp_IMPL(struct DispObject *pDispObject, NV5070_CTRL_CMD_GET_RG_FLIPLOCK_PROP_PARAMS *pParams);

static inline NV_STATUS dispobjCtrlCmdGetRgFliplockProp_DISPATCH(struct DispObject *pDispObject, NV5070_CTRL_CMD_GET_RG_FLIPLOCK_PROP_PARAMS *pParams) {
    return pDispObject->__dispobjCtrlCmdGetRgFliplockProp__(pDispObject, pParams);
}

NV_STATUS dispobjCtrlCmdSetRgFliplockProp_IMPL(struct DispObject *pDispObject, NV5070_CTRL_CMD_SET_RG_FLIPLOCK_PROP_PARAMS *pParams);

static inline NV_STATUS dispobjCtrlCmdSetRgFliplockProp_DISPATCH(struct DispObject *pDispObject, NV5070_CTRL_CMD_SET_RG_FLIPLOCK_PROP_PARAMS *pParams) {
    return pDispObject->__dispobjCtrlCmdSetRgFliplockProp__(pDispObject, pParams);
}

NV_STATUS dispobjCtrlCmdGetRgConnectedLockpinStateless_IMPL(struct DispObject *pDispObject, NV5070_CTRL_GET_RG_CONNECTED_LOCKPIN_STATELESS_PARAMS *pParams);

static inline NV_STATUS dispobjCtrlCmdGetRgConnectedLockpinStateless_DISPATCH(struct DispObject *pDispObject, NV5070_CTRL_GET_RG_CONNECTED_LOCKPIN_STATELESS_PARAMS *pParams) {
    return pDispObject->__dispobjCtrlCmdGetRgConnectedLockpinStateless__(pDispObject, pParams);
}

NV_STATUS dispobjCtrlCmdGetRgScanLine_IMPL(struct DispObject *pDispObject, NV5070_CTRL_CMD_GET_RG_SCAN_LINE_PARAMS *pParams);

static inline NV_STATUS dispobjCtrlCmdGetRgScanLine_DISPATCH(struct DispObject *pDispObject, NV5070_CTRL_CMD_GET_RG_SCAN_LINE_PARAMS *pParams) {
    return pDispObject->__dispobjCtrlCmdGetRgScanLine__(pDispObject, pParams);
}

NV_STATUS dispobjCtrlCmdGetSorSeqCtl_IMPL(struct DispObject *pDispObject, NV5070_CTRL_CMD_GET_SOR_SEQ_CTL_PARAMS *pParams);

static inline NV_STATUS dispobjCtrlCmdGetSorSeqCtl_DISPATCH(struct DispObject *pDispObject, NV5070_CTRL_CMD_GET_SOR_SEQ_CTL_PARAMS *pParams) {
    return pDispObject->__dispobjCtrlCmdGetSorSeqCtl__(pDispObject, pParams);
}

NV_STATUS dispobjCtrlCmdSetSorSeqCtl_IMPL(struct DispObject *pDispObject, NV5070_CTRL_CMD_SET_SOR_SEQ_CTL_PARAMS *pParams);

static inline NV_STATUS dispobjCtrlCmdSetSorSeqCtl_DISPATCH(struct DispObject *pDispObject, NV5070_CTRL_CMD_SET_SOR_SEQ_CTL_PARAMS *pParams) {
    return pDispObject->__dispobjCtrlCmdSetSorSeqCtl__(pDispObject, pParams);
}

NV_STATUS dispobjCtrlCmdSeqProgSpeed_IMPL(struct DispObject *pDispObject, NV5070_CTRL_SEQ_PROG_SPEED_PARAMS *pParams);

static inline NV_STATUS dispobjCtrlCmdSeqProgSpeed_DISPATCH(struct DispObject *pDispObject, NV5070_CTRL_SEQ_PROG_SPEED_PARAMS *pParams) {
    return pDispObject->__dispobjCtrlCmdSeqProgSpeed__(pDispObject, pParams);
}

NV_STATUS dispobjCtrlCmdGetSorPwm_IMPL(struct DispObject *pDispObject, NV5070_CTRL_CMD_GET_SOR_PWM_PARAMS *pParams);

static inline NV_STATUS dispobjCtrlCmdGetSorPwm_DISPATCH(struct DispObject *pDispObject, NV5070_CTRL_CMD_GET_SOR_PWM_PARAMS *pParams) {
    return pDispObject->__dispobjCtrlCmdGetSorPwm__(pDispObject, pParams);
}

NV_STATUS dispobjCtrlCmdSetSorPwm_IMPL(struct DispObject *pDispObject, NV5070_CTRL_CMD_SET_SOR_PWM_PARAMS *pParams);

static inline NV_STATUS dispobjCtrlCmdSetSorPwm_DISPATCH(struct DispObject *pDispObject, NV5070_CTRL_CMD_SET_SOR_PWM_PARAMS *pParams) {
    return pDispObject->__dispobjCtrlCmdSetSorPwm__(pDispObject, pParams);
}

NV_STATUS dispobjCtrlCmdGetSorOpMode_IMPL(struct DispObject *pDispObject, NV5070_CTRL_CMD_GET_SOR_OP_MODE_PARAMS *pParams);

static inline NV_STATUS dispobjCtrlCmdGetSorOpMode_DISPATCH(struct DispObject *pDispObject, NV5070_CTRL_CMD_GET_SOR_OP_MODE_PARAMS *pParams) {
    return pDispObject->__dispobjCtrlCmdGetSorOpMode__(pDispObject, pParams);
}

NV_STATUS dispobjCtrlCmdSetSorOpMode_IMPL(struct DispObject *pDispObject, NV5070_CTRL_CMD_SET_SOR_OP_MODE_PARAMS *pParams);

static inline NV_STATUS dispobjCtrlCmdSetSorOpMode_DISPATCH(struct DispObject *pDispObject, NV5070_CTRL_CMD_SET_SOR_OP_MODE_PARAMS *pParams) {
    return pDispObject->__dispobjCtrlCmdSetSorOpMode__(pDispObject, pParams);
}

NV_STATUS dispobjCtrlCmdSetSorFlushMode_IMPL(struct DispObject *pDispObject, NV5070_CTRL_SET_SOR_FLUSH_MODE_PARAMS *pParams);

static inline NV_STATUS dispobjCtrlCmdSetSorFlushMode_DISPATCH(struct DispObject *pDispObject, NV5070_CTRL_SET_SOR_FLUSH_MODE_PARAMS *pParams) {
    return pDispObject->__dispobjCtrlCmdSetSorFlushMode__(pDispObject, pParams);
}

NV_STATUS dispobjCtrlCmdSystemGetCapsV2_IMPL(struct DispObject *pDispObject, NV5070_CTRL_SYSTEM_GET_CAPS_V2_PARAMS *pCapsParams);

static inline NV_STATUS dispobjCtrlCmdSystemGetCapsV2_DISPATCH(struct DispObject *pDispObject, NV5070_CTRL_SYSTEM_GET_CAPS_V2_PARAMS *pCapsParams) {
    return pDispObject->__dispobjCtrlCmdSystemGetCapsV2__(pDispObject, pCapsParams);
}

NV_STATUS dispobjCtrlCmdEventSetTrigger_IMPL(struct DispObject *pDispObject);

static inline NV_STATUS dispobjCtrlCmdEventSetTrigger_DISPATCH(struct DispObject *pDispObject) {
    return pDispObject->__dispobjCtrlCmdEventSetTrigger__(pDispObject);
}

static inline NvBool dispobjShareCallback_DISPATCH(struct DispObject *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__dispobjShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS dispobjCheckMemInterUnmap_DISPATCH(struct DispObject *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__dispobjCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS dispobjMapTo_DISPATCH(struct DispObject *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__dispobjMapTo__(pResource, pParams);
}

static inline NvBool dispobjAccessCallback_DISPATCH(struct DispObject *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__dispobjAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline void dispobjSetNotificationShare_DISPATCH(struct DispObject *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__dispobjSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NvU32 dispobjGetRefCount_DISPATCH(struct DispObject *pResource) {
    return pResource->__dispobjGetRefCount__(pResource);
}

static inline void dispobjAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct DispObject *pResource, RsResourceRef *pReference) {
    pResource->__dispobjAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS dispobjControl_Prologue_DISPATCH(struct DispObject *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams) {
    return pDisplayApi->__dispobjControl_Prologue__(pDisplayApi, pCallContext, pRsParams);
}

static inline NV_STATUS dispobjUnmapFrom_DISPATCH(struct DispObject *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__dispobjUnmapFrom__(pResource, pParams);
}

static inline void dispobjControl_Epilogue_DISPATCH(struct DispObject *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams) {
    pDisplayApi->__dispobjControl_Epilogue__(pDisplayApi, pCallContext, pRsParams);
}

static inline NV_STATUS dispobjControlLookup_DISPATCH(struct DispObject *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__dispobjControlLookup__(pResource, pParams, ppEntry);
}

static inline NV_STATUS dispobjControl_DISPATCH(struct DispObject *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pDisplayApi->__dispobjControl__(pDisplayApi, pCallContext, pParams);
}

static inline NV_STATUS dispobjUnmap_DISPATCH(struct DispObject *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pResource->__dispobjUnmap__(pResource, pCallContext, pCpuMapping);
}

static inline NV_STATUS dispobjGetMemInterMapParams_DISPATCH(struct DispObject *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__dispobjGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS dispobjGetMemoryMappingDescriptor_DISPATCH(struct DispObject *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__dispobjGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS dispobjControlFilter_DISPATCH(struct DispObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__dispobjControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS dispobjUnregisterEvent_DISPATCH(struct DispObject *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__dispobjUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS dispobjControlSerialization_Prologue_DISPATCH(struct DispObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__dispobjControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool dispobjCanCopy_DISPATCH(struct DispObject *pResource) {
    return pResource->__dispobjCanCopy__(pResource);
}

static inline void dispobjPreDestruct_DISPATCH(struct DispObject *pResource) {
    pResource->__dispobjPreDestruct__(pResource);
}

static inline NV_STATUS dispobjIsDuplicate_DISPATCH(struct DispObject *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__dispobjIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void dispobjControlSerialization_Epilogue_DISPATCH(struct DispObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__dispobjControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline PEVENTNOTIFICATION *dispobjGetNotificationListPtr_DISPATCH(struct DispObject *pNotifier) {
    return pNotifier->__dispobjGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare *dispobjGetNotificationShare_DISPATCH(struct DispObject *pNotifier) {
    return pNotifier->__dispobjGetNotificationShare__(pNotifier);
}

static inline NV_STATUS dispobjMap_DISPATCH(struct DispObject *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pResource->__dispobjMap__(pResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS dispobjGetOrAllocNotifShare_DISPATCH(struct DispObject *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__dispobjGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

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
#ifdef NVOC_DISP_OBJS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct NvDispApi {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct DispObject __nvoc_base_DispObject;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct INotifier *__nvoc_pbase_INotifier;
    struct Notifier *__nvoc_pbase_Notifier;
    struct DisplayApi *__nvoc_pbase_DisplayApi;
    struct DispObject *__nvoc_pbase_DispObject;
    struct NvDispApi *__nvoc_pbase_NvDispApi;
    NV_STATUS (*__nvdispapiCtrlCmdIdleChannel__)(struct NvDispApi *, NVC370_CTRL_IDLE_CHANNEL_PARAMS *);
    NV_STATUS (*__nvdispapiCtrlCmdSetAccl__)(struct NvDispApi *, NVC370_CTRL_SET_ACCL_PARAMS *);
    NV_STATUS (*__nvdispapiCtrlCmdGetAccl__)(struct NvDispApi *, NVC370_CTRL_GET_ACCL_PARAMS *);
    NV_STATUS (*__nvdispapiCtrlCmdGetChannelInfo__)(struct NvDispApi *, NVC370_CTRL_CMD_GET_CHANNEL_INFO_PARAMS *);
    NV_STATUS (*__nvdispapiCtrlCmdSetSwaprdyGpioWar__)(struct NvDispApi *, NVC370_CTRL_SET_SWAPRDY_GPIO_WAR_PARAMS *);
    NV_STATUS (*__nvdispapiCtrlCmdGetLockpinsCaps__)(struct NvDispApi *, NVC370_CTRL_GET_LOCKPINS_CAPS_PARAMS *);
    NV_STATUS (*__nvdispapiCtrlCmdSetForceModeswitchFlagsOverrides__)(struct NvDispApi *, NVC370_CTRL_CMD_SET_FORCE_MODESWITCH_FLAGS_OVERRIDES_PARAMS *);
    NvBool (*__nvdispapiShareCallback__)(struct NvDispApi *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__nvdispapiCheckMemInterUnmap__)(struct NvDispApi *, NvBool);
    NV_STATUS (*__nvdispapiMapTo__)(struct NvDispApi *, RS_RES_MAP_TO_PARAMS *);
    NvBool (*__nvdispapiAccessCallback__)(struct NvDispApi *, struct RsClient *, void *, RsAccessRight);
    void (*__nvdispapiSetNotificationShare__)(struct NvDispApi *, struct NotifShare *);
    NvU32 (*__nvdispapiGetRefCount__)(struct NvDispApi *);
    void (*__nvdispapiAddAdditionalDependants__)(struct RsClient *, struct NvDispApi *, RsResourceRef *);
    NV_STATUS (*__nvdispapiControl_Prologue__)(struct NvDispApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__nvdispapiUnmapFrom__)(struct NvDispApi *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__nvdispapiControl_Epilogue__)(struct NvDispApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__nvdispapiControlLookup__)(struct NvDispApi *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NV_STATUS (*__nvdispapiControl__)(struct NvDispApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__nvdispapiUnmap__)(struct NvDispApi *, struct CALL_CONTEXT *, RsCpuMapping *);
    NV_STATUS (*__nvdispapiGetMemInterMapParams__)(struct NvDispApi *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__nvdispapiGetMemoryMappingDescriptor__)(struct NvDispApi *, struct MEMORY_DESCRIPTOR **);
    NV_STATUS (*__nvdispapiControlFilter__)(struct NvDispApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__nvdispapiUnregisterEvent__)(struct NvDispApi *, NvHandle, NvHandle, NvHandle, NvHandle);
    NV_STATUS (*__nvdispapiControlSerialization_Prologue__)(struct NvDispApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__nvdispapiCanCopy__)(struct NvDispApi *);
    void (*__nvdispapiPreDestruct__)(struct NvDispApi *);
    NV_STATUS (*__nvdispapiIsDuplicate__)(struct NvDispApi *, NvHandle, NvBool *);
    void (*__nvdispapiControlSerialization_Epilogue__)(struct NvDispApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    PEVENTNOTIFICATION *(*__nvdispapiGetNotificationListPtr__)(struct NvDispApi *);
    struct NotifShare *(*__nvdispapiGetNotificationShare__)(struct NvDispApi *);
    NV_STATUS (*__nvdispapiMap__)(struct NvDispApi *, struct CALL_CONTEXT *, RS_CPU_MAP_PARAMS *, RsCpuMapping *);
    NV_STATUS (*__nvdispapiGetOrAllocNotifShare__)(struct NvDispApi *, NvHandle, NvHandle, struct NotifShare **);
};

#ifndef __NVOC_CLASS_NvDispApi_TYPEDEF__
#define __NVOC_CLASS_NvDispApi_TYPEDEF__
typedef struct NvDispApi NvDispApi;
#endif /* __NVOC_CLASS_NvDispApi_TYPEDEF__ */

#ifndef __nvoc_class_id_NvDispApi
#define __nvoc_class_id_NvDispApi 0x36aa0b
#endif /* __nvoc_class_id_NvDispApi */

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

#define nvdispapiCtrlCmdIdleChannel(pNvDispApi, pParams) nvdispapiCtrlCmdIdleChannel_DISPATCH(pNvDispApi, pParams)
#define nvdispapiCtrlCmdSetAccl(pNvDispApi, pParams) nvdispapiCtrlCmdSetAccl_DISPATCH(pNvDispApi, pParams)
#define nvdispapiCtrlCmdGetAccl(pNvDispApi, pParams) nvdispapiCtrlCmdGetAccl_DISPATCH(pNvDispApi, pParams)
#define nvdispapiCtrlCmdGetChannelInfo(pNvDispApi, pParams) nvdispapiCtrlCmdGetChannelInfo_DISPATCH(pNvDispApi, pParams)
#define nvdispapiCtrlCmdSetSwaprdyGpioWar(pNvDispApi, pParams) nvdispapiCtrlCmdSetSwaprdyGpioWar_DISPATCH(pNvDispApi, pParams)
#define nvdispapiCtrlCmdGetLockpinsCaps(pNvDispApi, pParams) nvdispapiCtrlCmdGetLockpinsCaps_DISPATCH(pNvDispApi, pParams)
#define nvdispapiCtrlCmdSetForceModeswitchFlagsOverrides(pNvDispApi, pParams) nvdispapiCtrlCmdSetForceModeswitchFlagsOverrides_DISPATCH(pNvDispApi, pParams)
#define nvdispapiShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) nvdispapiShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define nvdispapiCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) nvdispapiCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define nvdispapiMapTo(pResource, pParams) nvdispapiMapTo_DISPATCH(pResource, pParams)
#define nvdispapiAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) nvdispapiAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define nvdispapiSetNotificationShare(pNotifier, pNotifShare) nvdispapiSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define nvdispapiGetRefCount(pResource) nvdispapiGetRefCount_DISPATCH(pResource)
#define nvdispapiAddAdditionalDependants(pClient, pResource, pReference) nvdispapiAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define nvdispapiControl_Prologue(pDisplayApi, pCallContext, pRsParams) nvdispapiControl_Prologue_DISPATCH(pDisplayApi, pCallContext, pRsParams)
#define nvdispapiUnmapFrom(pResource, pParams) nvdispapiUnmapFrom_DISPATCH(pResource, pParams)
#define nvdispapiControl_Epilogue(pDisplayApi, pCallContext, pRsParams) nvdispapiControl_Epilogue_DISPATCH(pDisplayApi, pCallContext, pRsParams)
#define nvdispapiControlLookup(pResource, pParams, ppEntry) nvdispapiControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define nvdispapiControl(pDisplayApi, pCallContext, pParams) nvdispapiControl_DISPATCH(pDisplayApi, pCallContext, pParams)
#define nvdispapiUnmap(pResource, pCallContext, pCpuMapping) nvdispapiUnmap_DISPATCH(pResource, pCallContext, pCpuMapping)
#define nvdispapiGetMemInterMapParams(pRmResource, pParams) nvdispapiGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define nvdispapiGetMemoryMappingDescriptor(pRmResource, ppMemDesc) nvdispapiGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define nvdispapiControlFilter(pResource, pCallContext, pParams) nvdispapiControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define nvdispapiUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) nvdispapiUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define nvdispapiControlSerialization_Prologue(pResource, pCallContext, pParams) nvdispapiControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define nvdispapiCanCopy(pResource) nvdispapiCanCopy_DISPATCH(pResource)
#define nvdispapiPreDestruct(pResource) nvdispapiPreDestruct_DISPATCH(pResource)
#define nvdispapiIsDuplicate(pResource, hMemory, pDuplicate) nvdispapiIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define nvdispapiControlSerialization_Epilogue(pResource, pCallContext, pParams) nvdispapiControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define nvdispapiGetNotificationListPtr(pNotifier) nvdispapiGetNotificationListPtr_DISPATCH(pNotifier)
#define nvdispapiGetNotificationShare(pNotifier) nvdispapiGetNotificationShare_DISPATCH(pNotifier)
#define nvdispapiMap(pResource, pCallContext, pParams, pCpuMapping) nvdispapiMap_DISPATCH(pResource, pCallContext, pParams, pCpuMapping)
#define nvdispapiGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) nvdispapiGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)
NV_STATUS nvdispapiCtrlCmdIdleChannel_IMPL(struct NvDispApi *pNvDispApi, NVC370_CTRL_IDLE_CHANNEL_PARAMS *pParams);

static inline NV_STATUS nvdispapiCtrlCmdIdleChannel_DISPATCH(struct NvDispApi *pNvDispApi, NVC370_CTRL_IDLE_CHANNEL_PARAMS *pParams) {
    return pNvDispApi->__nvdispapiCtrlCmdIdleChannel__(pNvDispApi, pParams);
}

NV_STATUS nvdispapiCtrlCmdSetAccl_IMPL(struct NvDispApi *pNvDispApi, NVC370_CTRL_SET_ACCL_PARAMS *pParams);

static inline NV_STATUS nvdispapiCtrlCmdSetAccl_DISPATCH(struct NvDispApi *pNvDispApi, NVC370_CTRL_SET_ACCL_PARAMS *pParams) {
    return pNvDispApi->__nvdispapiCtrlCmdSetAccl__(pNvDispApi, pParams);
}

NV_STATUS nvdispapiCtrlCmdGetAccl_IMPL(struct NvDispApi *pNvDispApi, NVC370_CTRL_GET_ACCL_PARAMS *pParams);

static inline NV_STATUS nvdispapiCtrlCmdGetAccl_DISPATCH(struct NvDispApi *pNvDispApi, NVC370_CTRL_GET_ACCL_PARAMS *pParams) {
    return pNvDispApi->__nvdispapiCtrlCmdGetAccl__(pNvDispApi, pParams);
}

NV_STATUS nvdispapiCtrlCmdGetChannelInfo_IMPL(struct NvDispApi *pNvDispApi, NVC370_CTRL_CMD_GET_CHANNEL_INFO_PARAMS *pParams);

static inline NV_STATUS nvdispapiCtrlCmdGetChannelInfo_DISPATCH(struct NvDispApi *pNvDispApi, NVC370_CTRL_CMD_GET_CHANNEL_INFO_PARAMS *pParams) {
    return pNvDispApi->__nvdispapiCtrlCmdGetChannelInfo__(pNvDispApi, pParams);
}

NV_STATUS nvdispapiCtrlCmdSetSwaprdyGpioWar_IMPL(struct NvDispApi *pNvDispApi, NVC370_CTRL_SET_SWAPRDY_GPIO_WAR_PARAMS *pParams);

static inline NV_STATUS nvdispapiCtrlCmdSetSwaprdyGpioWar_DISPATCH(struct NvDispApi *pNvDispApi, NVC370_CTRL_SET_SWAPRDY_GPIO_WAR_PARAMS *pParams) {
    return pNvDispApi->__nvdispapiCtrlCmdSetSwaprdyGpioWar__(pNvDispApi, pParams);
}

NV_STATUS nvdispapiCtrlCmdGetLockpinsCaps_IMPL(struct NvDispApi *pNvDispApi, NVC370_CTRL_GET_LOCKPINS_CAPS_PARAMS *pParams);

static inline NV_STATUS nvdispapiCtrlCmdGetLockpinsCaps_DISPATCH(struct NvDispApi *pNvDispApi, NVC370_CTRL_GET_LOCKPINS_CAPS_PARAMS *pParams) {
    return pNvDispApi->__nvdispapiCtrlCmdGetLockpinsCaps__(pNvDispApi, pParams);
}

NV_STATUS nvdispapiCtrlCmdSetForceModeswitchFlagsOverrides_IMPL(struct NvDispApi *pNvDispApi, NVC370_CTRL_CMD_SET_FORCE_MODESWITCH_FLAGS_OVERRIDES_PARAMS *pParams);

static inline NV_STATUS nvdispapiCtrlCmdSetForceModeswitchFlagsOverrides_DISPATCH(struct NvDispApi *pNvDispApi, NVC370_CTRL_CMD_SET_FORCE_MODESWITCH_FLAGS_OVERRIDES_PARAMS *pParams) {
    return pNvDispApi->__nvdispapiCtrlCmdSetForceModeswitchFlagsOverrides__(pNvDispApi, pParams);
}

static inline NvBool nvdispapiShareCallback_DISPATCH(struct NvDispApi *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__nvdispapiShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS nvdispapiCheckMemInterUnmap_DISPATCH(struct NvDispApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvdispapiCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS nvdispapiMapTo_DISPATCH(struct NvDispApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvdispapiMapTo__(pResource, pParams);
}

static inline NvBool nvdispapiAccessCallback_DISPATCH(struct NvDispApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvdispapiAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline void nvdispapiSetNotificationShare_DISPATCH(struct NvDispApi *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__nvdispapiSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NvU32 nvdispapiGetRefCount_DISPATCH(struct NvDispApi *pResource) {
    return pResource->__nvdispapiGetRefCount__(pResource);
}

static inline void nvdispapiAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct NvDispApi *pResource, RsResourceRef *pReference) {
    pResource->__nvdispapiAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS nvdispapiControl_Prologue_DISPATCH(struct NvDispApi *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams) {
    return pDisplayApi->__nvdispapiControl_Prologue__(pDisplayApi, pCallContext, pRsParams);
}

static inline NV_STATUS nvdispapiUnmapFrom_DISPATCH(struct NvDispApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvdispapiUnmapFrom__(pResource, pParams);
}

static inline void nvdispapiControl_Epilogue_DISPATCH(struct NvDispApi *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams) {
    pDisplayApi->__nvdispapiControl_Epilogue__(pDisplayApi, pCallContext, pRsParams);
}

static inline NV_STATUS nvdispapiControlLookup_DISPATCH(struct NvDispApi *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__nvdispapiControlLookup__(pResource, pParams, ppEntry);
}

static inline NV_STATUS nvdispapiControl_DISPATCH(struct NvDispApi *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pDisplayApi->__nvdispapiControl__(pDisplayApi, pCallContext, pParams);
}

static inline NV_STATUS nvdispapiUnmap_DISPATCH(struct NvDispApi *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pResource->__nvdispapiUnmap__(pResource, pCallContext, pCpuMapping);
}

static inline NV_STATUS nvdispapiGetMemInterMapParams_DISPATCH(struct NvDispApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvdispapiGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS nvdispapiGetMemoryMappingDescriptor_DISPATCH(struct NvDispApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvdispapiGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS nvdispapiControlFilter_DISPATCH(struct NvDispApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvdispapiControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS nvdispapiUnregisterEvent_DISPATCH(struct NvDispApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__nvdispapiUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS nvdispapiControlSerialization_Prologue_DISPATCH(struct NvDispApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvdispapiControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool nvdispapiCanCopy_DISPATCH(struct NvDispApi *pResource) {
    return pResource->__nvdispapiCanCopy__(pResource);
}

static inline void nvdispapiPreDestruct_DISPATCH(struct NvDispApi *pResource) {
    pResource->__nvdispapiPreDestruct__(pResource);
}

static inline NV_STATUS nvdispapiIsDuplicate_DISPATCH(struct NvDispApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvdispapiIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void nvdispapiControlSerialization_Epilogue_DISPATCH(struct NvDispApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvdispapiControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline PEVENTNOTIFICATION *nvdispapiGetNotificationListPtr_DISPATCH(struct NvDispApi *pNotifier) {
    return pNotifier->__nvdispapiGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare *nvdispapiGetNotificationShare_DISPATCH(struct NvDispApi *pNotifier) {
    return pNotifier->__nvdispapiGetNotificationShare__(pNotifier);
}

static inline NV_STATUS nvdispapiMap_DISPATCH(struct NvDispApi *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pResource->__nvdispapiMap__(pResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS nvdispapiGetOrAllocNotifShare_DISPATCH(struct NvDispApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__nvdispapiGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

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
#ifdef NVOC_DISP_OBJS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct DispSwObj {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct DisplayApi __nvoc_base_DisplayApi;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct INotifier *__nvoc_pbase_INotifier;
    struct Notifier *__nvoc_pbase_Notifier;
    struct DisplayApi *__nvoc_pbase_DisplayApi;
    struct DispSwObj *__nvoc_pbase_DispSwObj;
    NV_STATUS (*__dispswobjCtrlCmdIsModePossible__)(struct DispSwObj *, NVC372_CTRL_IS_MODE_POSSIBLE_PARAMS *);
    NV_STATUS (*__dispswobjCtrlCmdIsModePossibleOrSettings__)(struct DispSwObj *, NVC372_CTRL_IS_MODE_POSSIBLE_OR_SETTINGS_PARAMS *);
    NV_STATUS (*__dispswobjCtrlCmdVideoAdaptiveRefreshRate__)(struct DispSwObj *, NVC372_CTRL_CMD_VIDEO_ADAPTIVE_REFRESH_RATE_PARAMS *);
    NV_STATUS (*__dispswobjCtrlCmdGetActiveViewportPointIn__)(struct DispSwObj *, NVC372_CTRL_CMD_GET_ACTIVE_VIEWPORT_POINT_IN_PARAMS *);
    NvBool (*__dispswobjShareCallback__)(struct DispSwObj *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__dispswobjCheckMemInterUnmap__)(struct DispSwObj *, NvBool);
    NV_STATUS (*__dispswobjMapTo__)(struct DispSwObj *, RS_RES_MAP_TO_PARAMS *);
    NvBool (*__dispswobjAccessCallback__)(struct DispSwObj *, struct RsClient *, void *, RsAccessRight);
    void (*__dispswobjSetNotificationShare__)(struct DispSwObj *, struct NotifShare *);
    NvU32 (*__dispswobjGetRefCount__)(struct DispSwObj *);
    void (*__dispswobjAddAdditionalDependants__)(struct RsClient *, struct DispSwObj *, RsResourceRef *);
    NV_STATUS (*__dispswobjControl_Prologue__)(struct DispSwObj *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__dispswobjUnmapFrom__)(struct DispSwObj *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__dispswobjControl_Epilogue__)(struct DispSwObj *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__dispswobjControlLookup__)(struct DispSwObj *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NV_STATUS (*__dispswobjControl__)(struct DispSwObj *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__dispswobjUnmap__)(struct DispSwObj *, struct CALL_CONTEXT *, RsCpuMapping *);
    NV_STATUS (*__dispswobjGetMemInterMapParams__)(struct DispSwObj *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__dispswobjGetMemoryMappingDescriptor__)(struct DispSwObj *, struct MEMORY_DESCRIPTOR **);
    NV_STATUS (*__dispswobjControlFilter__)(struct DispSwObj *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__dispswobjUnregisterEvent__)(struct DispSwObj *, NvHandle, NvHandle, NvHandle, NvHandle);
    NV_STATUS (*__dispswobjControlSerialization_Prologue__)(struct DispSwObj *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__dispswobjCanCopy__)(struct DispSwObj *);
    void (*__dispswobjPreDestruct__)(struct DispSwObj *);
    NV_STATUS (*__dispswobjIsDuplicate__)(struct DispSwObj *, NvHandle, NvBool *);
    void (*__dispswobjControlSerialization_Epilogue__)(struct DispSwObj *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    PEVENTNOTIFICATION *(*__dispswobjGetNotificationListPtr__)(struct DispSwObj *);
    struct NotifShare *(*__dispswobjGetNotificationShare__)(struct DispSwObj *);
    NV_STATUS (*__dispswobjMap__)(struct DispSwObj *, struct CALL_CONTEXT *, RS_CPU_MAP_PARAMS *, RsCpuMapping *);
    NV_STATUS (*__dispswobjGetOrAllocNotifShare__)(struct DispSwObj *, NvHandle, NvHandle, struct NotifShare **);
};

#ifndef __NVOC_CLASS_DispSwObj_TYPEDEF__
#define __NVOC_CLASS_DispSwObj_TYPEDEF__
typedef struct DispSwObj DispSwObj;
#endif /* __NVOC_CLASS_DispSwObj_TYPEDEF__ */

#ifndef __nvoc_class_id_DispSwObj
#define __nvoc_class_id_DispSwObj 0x6aa5e2
#endif /* __nvoc_class_id_DispSwObj */

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

#define dispswobjCtrlCmdIsModePossible(pDispSwObj, pParams) dispswobjCtrlCmdIsModePossible_DISPATCH(pDispSwObj, pParams)
#define dispswobjCtrlCmdIsModePossibleOrSettings(pDispSwObj, pParams) dispswobjCtrlCmdIsModePossibleOrSettings_DISPATCH(pDispSwObj, pParams)
#define dispswobjCtrlCmdVideoAdaptiveRefreshRate(pDispSwObj, pParams) dispswobjCtrlCmdVideoAdaptiveRefreshRate_DISPATCH(pDispSwObj, pParams)
#define dispswobjCtrlCmdGetActiveViewportPointIn(pDispSwObj, pParams) dispswobjCtrlCmdGetActiveViewportPointIn_DISPATCH(pDispSwObj, pParams)
#define dispswobjShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) dispswobjShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define dispswobjCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) dispswobjCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define dispswobjMapTo(pResource, pParams) dispswobjMapTo_DISPATCH(pResource, pParams)
#define dispswobjAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) dispswobjAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define dispswobjSetNotificationShare(pNotifier, pNotifShare) dispswobjSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define dispswobjGetRefCount(pResource) dispswobjGetRefCount_DISPATCH(pResource)
#define dispswobjAddAdditionalDependants(pClient, pResource, pReference) dispswobjAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define dispswobjControl_Prologue(pDisplayApi, pCallContext, pRsParams) dispswobjControl_Prologue_DISPATCH(pDisplayApi, pCallContext, pRsParams)
#define dispswobjUnmapFrom(pResource, pParams) dispswobjUnmapFrom_DISPATCH(pResource, pParams)
#define dispswobjControl_Epilogue(pDisplayApi, pCallContext, pRsParams) dispswobjControl_Epilogue_DISPATCH(pDisplayApi, pCallContext, pRsParams)
#define dispswobjControlLookup(pResource, pParams, ppEntry) dispswobjControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define dispswobjControl(pDisplayApi, pCallContext, pParams) dispswobjControl_DISPATCH(pDisplayApi, pCallContext, pParams)
#define dispswobjUnmap(pResource, pCallContext, pCpuMapping) dispswobjUnmap_DISPATCH(pResource, pCallContext, pCpuMapping)
#define dispswobjGetMemInterMapParams(pRmResource, pParams) dispswobjGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define dispswobjGetMemoryMappingDescriptor(pRmResource, ppMemDesc) dispswobjGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define dispswobjControlFilter(pResource, pCallContext, pParams) dispswobjControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define dispswobjUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) dispswobjUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define dispswobjControlSerialization_Prologue(pResource, pCallContext, pParams) dispswobjControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define dispswobjCanCopy(pResource) dispswobjCanCopy_DISPATCH(pResource)
#define dispswobjPreDestruct(pResource) dispswobjPreDestruct_DISPATCH(pResource)
#define dispswobjIsDuplicate(pResource, hMemory, pDuplicate) dispswobjIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define dispswobjControlSerialization_Epilogue(pResource, pCallContext, pParams) dispswobjControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define dispswobjGetNotificationListPtr(pNotifier) dispswobjGetNotificationListPtr_DISPATCH(pNotifier)
#define dispswobjGetNotificationShare(pNotifier) dispswobjGetNotificationShare_DISPATCH(pNotifier)
#define dispswobjMap(pResource, pCallContext, pParams, pCpuMapping) dispswobjMap_DISPATCH(pResource, pCallContext, pParams, pCpuMapping)
#define dispswobjGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) dispswobjGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)
NV_STATUS dispswobjCtrlCmdIsModePossible_IMPL(struct DispSwObj *pDispSwObj, NVC372_CTRL_IS_MODE_POSSIBLE_PARAMS *pParams);

static inline NV_STATUS dispswobjCtrlCmdIsModePossible_DISPATCH(struct DispSwObj *pDispSwObj, NVC372_CTRL_IS_MODE_POSSIBLE_PARAMS *pParams) {
    return pDispSwObj->__dispswobjCtrlCmdIsModePossible__(pDispSwObj, pParams);
}

NV_STATUS dispswobjCtrlCmdIsModePossibleOrSettings_IMPL(struct DispSwObj *pDispSwObj, NVC372_CTRL_IS_MODE_POSSIBLE_OR_SETTINGS_PARAMS *pParams);

static inline NV_STATUS dispswobjCtrlCmdIsModePossibleOrSettings_DISPATCH(struct DispSwObj *pDispSwObj, NVC372_CTRL_IS_MODE_POSSIBLE_OR_SETTINGS_PARAMS *pParams) {
    return pDispSwObj->__dispswobjCtrlCmdIsModePossibleOrSettings__(pDispSwObj, pParams);
}

NV_STATUS dispswobjCtrlCmdVideoAdaptiveRefreshRate_IMPL(struct DispSwObj *pDispSwObj, NVC372_CTRL_CMD_VIDEO_ADAPTIVE_REFRESH_RATE_PARAMS *pParams);

static inline NV_STATUS dispswobjCtrlCmdVideoAdaptiveRefreshRate_DISPATCH(struct DispSwObj *pDispSwObj, NVC372_CTRL_CMD_VIDEO_ADAPTIVE_REFRESH_RATE_PARAMS *pParams) {
    return pDispSwObj->__dispswobjCtrlCmdVideoAdaptiveRefreshRate__(pDispSwObj, pParams);
}

NV_STATUS dispswobjCtrlCmdGetActiveViewportPointIn_IMPL(struct DispSwObj *pDispSwObj, NVC372_CTRL_CMD_GET_ACTIVE_VIEWPORT_POINT_IN_PARAMS *pParams);

static inline NV_STATUS dispswobjCtrlCmdGetActiveViewportPointIn_DISPATCH(struct DispSwObj *pDispSwObj, NVC372_CTRL_CMD_GET_ACTIVE_VIEWPORT_POINT_IN_PARAMS *pParams) {
    return pDispSwObj->__dispswobjCtrlCmdGetActiveViewportPointIn__(pDispSwObj, pParams);
}

static inline NvBool dispswobjShareCallback_DISPATCH(struct DispSwObj *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__dispswobjShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS dispswobjCheckMemInterUnmap_DISPATCH(struct DispSwObj *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__dispswobjCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS dispswobjMapTo_DISPATCH(struct DispSwObj *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__dispswobjMapTo__(pResource, pParams);
}

static inline NvBool dispswobjAccessCallback_DISPATCH(struct DispSwObj *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__dispswobjAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline void dispswobjSetNotificationShare_DISPATCH(struct DispSwObj *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__dispswobjSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NvU32 dispswobjGetRefCount_DISPATCH(struct DispSwObj *pResource) {
    return pResource->__dispswobjGetRefCount__(pResource);
}

static inline void dispswobjAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct DispSwObj *pResource, RsResourceRef *pReference) {
    pResource->__dispswobjAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS dispswobjControl_Prologue_DISPATCH(struct DispSwObj *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams) {
    return pDisplayApi->__dispswobjControl_Prologue__(pDisplayApi, pCallContext, pRsParams);
}

static inline NV_STATUS dispswobjUnmapFrom_DISPATCH(struct DispSwObj *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__dispswobjUnmapFrom__(pResource, pParams);
}

static inline void dispswobjControl_Epilogue_DISPATCH(struct DispSwObj *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams) {
    pDisplayApi->__dispswobjControl_Epilogue__(pDisplayApi, pCallContext, pRsParams);
}

static inline NV_STATUS dispswobjControlLookup_DISPATCH(struct DispSwObj *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__dispswobjControlLookup__(pResource, pParams, ppEntry);
}

static inline NV_STATUS dispswobjControl_DISPATCH(struct DispSwObj *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pDisplayApi->__dispswobjControl__(pDisplayApi, pCallContext, pParams);
}

static inline NV_STATUS dispswobjUnmap_DISPATCH(struct DispSwObj *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pResource->__dispswobjUnmap__(pResource, pCallContext, pCpuMapping);
}

static inline NV_STATUS dispswobjGetMemInterMapParams_DISPATCH(struct DispSwObj *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__dispswobjGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS dispswobjGetMemoryMappingDescriptor_DISPATCH(struct DispSwObj *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__dispswobjGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS dispswobjControlFilter_DISPATCH(struct DispSwObj *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__dispswobjControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS dispswobjUnregisterEvent_DISPATCH(struct DispSwObj *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__dispswobjUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS dispswobjControlSerialization_Prologue_DISPATCH(struct DispSwObj *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__dispswobjControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool dispswobjCanCopy_DISPATCH(struct DispSwObj *pResource) {
    return pResource->__dispswobjCanCopy__(pResource);
}

static inline void dispswobjPreDestruct_DISPATCH(struct DispSwObj *pResource) {
    pResource->__dispswobjPreDestruct__(pResource);
}

static inline NV_STATUS dispswobjIsDuplicate_DISPATCH(struct DispSwObj *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__dispswobjIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void dispswobjControlSerialization_Epilogue_DISPATCH(struct DispSwObj *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__dispswobjControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline PEVENTNOTIFICATION *dispswobjGetNotificationListPtr_DISPATCH(struct DispSwObj *pNotifier) {
    return pNotifier->__dispswobjGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare *dispswobjGetNotificationShare_DISPATCH(struct DispSwObj *pNotifier) {
    return pNotifier->__dispswobjGetNotificationShare__(pNotifier);
}

static inline NV_STATUS dispswobjMap_DISPATCH(struct DispSwObj *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pResource->__dispswobjMap__(pResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS dispswobjGetOrAllocNotifShare_DISPATCH(struct DispSwObj *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__dispswobjGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

NV_STATUS dispswobjConstruct_IMPL(struct DispSwObj *arg_pDispSwObj, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_dispswobjConstruct(arg_pDispSwObj, arg_pCallContext, arg_pParams) dispswobjConstruct_IMPL(arg_pDispSwObj, arg_pCallContext, arg_pParams)
#undef PRIVATE_FIELD


/*!
 * RM internal class representing XXX_DISPLAY_COMMON (class id: 0x0073)
 *
 * Only one instance of this class is allowed per-GPU. Multi-instance restrictions
 * are enforced by resource_list.h
 */
#ifdef NVOC_DISP_OBJS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct DispCommon {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct DisplayApi __nvoc_base_DisplayApi;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct INotifier *__nvoc_pbase_INotifier;
    struct Notifier *__nvoc_pbase_Notifier;
    struct DisplayApi *__nvoc_pbase_DisplayApi;
    struct DispCommon *__nvoc_pbase_DispCommon;
    NV_STATUS (*__dispcmnCtrlCmdSystemGetVblankCounter__)(struct DispCommon *, NV0073_CTRL_SYSTEM_GET_VBLANK_COUNTER_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSystemGetVblankEnable__)(struct DispCommon *, NV0073_CTRL_SYSTEM_GET_VBLANK_ENABLE_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSystemCheckSidebandSrSupport__)(struct DispCommon *, NV0073_CTRL_CMD_SYSTEM_CHECK_SIDEBAND_SR_SUPPORT_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSystemGetHotplugState__)(struct DispCommon *, NV0073_CTRL_SYSTEM_GET_HOTPLUG_STATE_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSystemGetInternalDisplays__)(struct DispCommon *, NV0073_CTRL_SYSTEM_GET_INTERNAL_DISPLAYS_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSystemGetConnectorTable__)(struct DispCommon *, NV0073_CTRL_SYSTEM_GET_CONNECTOR_TABLE_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSystemVrrDisplayInfo__)(struct DispCommon *, NV0073_CTRL_SYSTEM_VRR_DISPLAY_INFO_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdVRRSetRgLineActive__)(struct DispCommon *, NV0073_CTRL_CMD_SYSTEM_VRR_SET_RGLINE_ACTIVE_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdInternalVRRSetRgLineActive__)(struct DispCommon *, NV0073_CTRL_CMD_SYSTEM_VRR_SET_RGLINE_ACTIVE_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDpEnableVrr__)(struct DispCommon *, NV0073_CTRL_CMD_DP_ENABLE_VRR_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdClearELVBlock__)(struct DispCommon *, NV0073_CTRL_SYSTEM_CLEAR_ELV_BLOCK_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSpecificDisplayChange__)(struct DispCommon *, NV0073_CTRL_SPECIFIC_DISPLAY_CHANGE_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDfpGetSpreadSpectrum__)(struct DispCommon *, NV0073_CTRL_DFP_GET_SPREAD_SPECTRUM_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDfpGetLcdGpioPinNum__)(struct DispCommon *, NV0073_CTRL_DFP_GET_LCD_GPIO_PIN_NUM_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDpGetAudioMuteStream__)(struct DispCommon *, NV0073_CTRL_DP_GET_AUDIO_MUTESTREAM_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDpAuxchI2cTransferCtrl__)(struct DispCommon *, NV0073_CTRL_DP_AUXCH_I2C_TRANSFER_CTRL_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDpASSRCtrl__)(struct DispCommon *, NV0073_CTRL_DP_ASSR_CTRL_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDpSetEcf__)(struct DispCommon *, NV0073_CTRL_CMD_DP_SET_ECF_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdStereoDongleSupported__)(struct DispCommon *, NV0073_CTRL_STEREO_DONGLE_SUPPORTED_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDfpRecordChannelRegisters__)(struct DispCommon *, NV0073_CTRL_CMD_SYSTEM_RECORD_CHANNEL_REGS_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSpecificGetBacklightBrightness__)(struct DispCommon *, NV0073_CTRL_SPECIFIC_BACKLIGHT_BRIGHTNESS_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSpecificSetBacklightBrightness__)(struct DispCommon *, NV0073_CTRL_SPECIFIC_BACKLIGHT_BRIGHTNESS_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdPsrGetSrPanelInfo__)(struct DispCommon *, NV0073_CTRL_PSR_GET_SR_PANEL_INFO_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDfpSwitchDispMux__)(struct DispCommon *, NV0073_CTRL_CMD_DFP_SWITCH_DISP_MUX_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDfpRunPreDispMuxOperations__)(struct DispCommon *, NV0073_CTRL_CMD_DFP_RUN_PRE_DISP_MUX_OPERATIONS_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDfpRunPostDispMuxOperations__)(struct DispCommon *, NV0073_CTRL_CMD_DFP_RUN_POST_DISP_MUX_OPERATIONS_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDfpGetDispMuxStatus__)(struct DispCommon *, NV0073_CTRL_CMD_DFP_GET_DISP_MUX_STATUS_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDfpInternalLcdOverdrive__)(struct DispCommon *, NV0073_CTRL_CMD_DP_AUXCH_OD_CTRL_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDfpExecuteInternalLcdOverDrivePolicy__)(struct DispCommon *, NV0073_CTRL_DP_EXECUTE_OVERDRIVE_POLICY_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSystemExecuteAcpiMethod__)(struct DispCommon *, NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSystemGetAcpiIdMap__)(struct DispCommon *, NV0073_CTRL_SYSTEM_GET_ACPI_ID_MAP_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSystemAcpiSubsystemActivated__)(struct DispCommon *, NV0073_CTRL_SYSTEM_ACPI_SUBSYSTEM_ACTIVATED_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSpecificSetAcpiIdMapping__)(struct DispCommon *, NV0073_CTRL_SPECIFIC_SET_ACPI_ID_MAPPING_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSpecificGetAcpiDodDisplayPortAttachment__)(struct DispCommon *, NV0073_CTRL_GET_ACPI_DOD_DISPLAY_PORT_ATTACHMENT_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSystemGetCapsV2__)(struct DispCommon *, NV0073_CTRL_SYSTEM_GET_CAPS_V2_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSystemGetNumHeads__)(struct DispCommon *, NV0073_CTRL_SYSTEM_GET_NUM_HEADS_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSystemGetScanline__)(struct DispCommon *, NV0073_CTRL_SYSTEM_GET_SCANLINE_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSystemGetSuppported__)(struct DispCommon *, NV0073_CTRL_SYSTEM_GET_SUPPORTED_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSystemGetConnectState__)(struct DispCommon *, NV0073_CTRL_SYSTEM_GET_CONNECT_STATE_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSystemGetHotplugUnplugState__)(struct DispCommon *, NV0073_CTRL_SYSTEM_GET_HOTPLUG_UNPLUG_STATE_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdInternalGetHotplugUnplugState__)(struct DispCommon *, NV0073_CTRL_SYSTEM_GET_HOTPLUG_UNPLUG_STATE_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSystemGetHeadRoutingMap__)(struct DispCommon *, NV0073_CTRL_SYSTEM_GET_HEAD_ROUTING_MAP_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSystemGetActive__)(struct DispCommon *, NV0073_CTRL_SYSTEM_GET_ACTIVE_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSystemGetBootDisplays__)(struct DispCommon *, NV0073_CTRL_SYSTEM_GET_BOOT_DISPLAYS_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSystemQueryDisplayIdsWithMux__)(struct DispCommon *, NV0073_CTRL_CMD_SYSTEM_QUERY_DISPLAY_IDS_WITH_MUX_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSystemCheckSidebandI2cSupport__)(struct DispCommon *, NV0073_CTRL_CMD_SYSTEM_CHECK_SIDEBAND_I2C_SUPPORT_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSystemAllocateDisplayBandwidth__)(struct DispCommon *, NV0073_CTRL_SYSTEM_ALLOCATE_DISPLAY_BANDWIDTH_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSystemGetHotplugConfig__)(struct DispCommon *, NV0073_CTRL_SYSTEM_GET_SET_HOTPLUG_CONFIG_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSystemGetHotplugEventConfig__)(struct DispCommon *, NV0073_CTRL_SYSTEM_HOTPLUG_EVENT_CONFIG_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSystemSetHotplugEventConfig__)(struct DispCommon *, NV0073_CTRL_SYSTEM_HOTPLUG_EVENT_CONFIG_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSystemArmLightweightSupervisor__)(struct DispCommon *, NV0073_CTRL_SYSTEM_ARM_LIGHTWEIGHT_SUPERVISOR_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSystemConfigVrrPstateSwitch__)(struct DispCommon *, NV0073_CTRL_SYSTEM_CONFIG_VRR_PSTATE_SWITCH_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSpecificGetType__)(struct DispCommon *, NV0073_CTRL_SPECIFIC_GET_TYPE_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSpecificGetEdidV2__)(struct DispCommon *, NV0073_CTRL_SPECIFIC_GET_EDID_V2_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSpecificSetEdidV2__)(struct DispCommon *, NV0073_CTRL_SPECIFIC_SET_EDID_V2_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSpecificFakeDevice__)(struct DispCommon *, NV0073_CTRL_CMD_SPECIFIC_FAKE_DEVICE_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSpecificGetConnectorData__)(struct DispCommon *, NV0073_CTRL_SPECIFIC_GET_CONNECTOR_DATA_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSpecificSetHdmiEnable__)(struct DispCommon *, NV0073_CTRL_SPECIFIC_SET_HDMI_ENABLE_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSpecificCtrlHdmi__)(struct DispCommon *, NV0073_CTRL_SPECIFIC_CTRL_HDMI_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSpecificGetAllHeadMask__)(struct DispCommon *, NV0073_CTRL_SPECIFIC_GET_ALL_HEAD_MASK_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSpecificSetOdPacket__)(struct DispCommon *, NV0073_CTRL_SPECIFIC_SET_OD_PACKET_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSpecificAcquireSharedGenericPacket__)(struct DispCommon *, NV0073_CTRL_SPECIFIC_ACQUIRE_SHARED_GENERIC_PACKET_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSpecificSetSharedGenericPacket__)(struct DispCommon *, NV0073_CTRL_SPECIFIC_SET_SHARED_GENERIC_PACKET_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSpecificReleaseSharedGenericPacket__)(struct DispCommon *, NV0073_CTRL_SPECIFIC_RELEASE_SHARED_GENERIC_PACKET_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSpecificSetOdPacketCtrl__)(struct DispCommon *, NV0073_CTRL_SPECIFIC_SET_OD_PACKET_CTRL_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSpecificOrGetInfo__)(struct DispCommon *, NV0073_CTRL_SPECIFIC_OR_GET_INFO_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSpecificGetPclkLimit__)(struct DispCommon *, NV0073_CTRL_SPECIFIC_GET_PCLK_LIMIT_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSpecificSetHdmiSinkCaps__)(struct DispCommon *, NV0073_CTRL_SPECIFIC_SET_HDMI_SINK_CAPS_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSpecificSetMonitorPower__)(struct DispCommon *, NV0073_CTRL_SPECIFIC_SET_MONITOR_POWER_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSpecificSetHdmiFrlLinkConfig__)(struct DispCommon *, NV0073_CTRL_SPECIFIC_SET_HDMI_FRL_LINK_CONFIG_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSpecificApplyEdidOverrideV2__)(struct DispCommon *, NV0073_CTRL_SPECIFIC_APPLY_EDID_OVERRIDE_V2_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSpecificGetI2cPortid__)(struct DispCommon *, NV0073_CTRL_SPECIFIC_GET_I2C_PORTID_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSpecificGetHdmiGpuCaps__)(struct DispCommon *, NV0073_CTRL_SPECIFIC_GET_HDMI_GPU_CAPS_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSpecificGetHdmiScdcData__)(struct DispCommon *, NV0073_CTRL_SPECIFIC_GET_HDMI_SCDC_DATA_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSpecificIsDirectmodeDisplay__)(struct DispCommon *, NV0073_CTRL_SPECIFIC_IS_DIRECTMODE_DISPLAY_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSpecificDefaultAdaptivesyncDisplay__)(struct DispCommon *, NV0073_CTRL_SPECIFIC_DEFAULT_ADAPTIVESYNC_DISPLAY_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSpecificSetHdmiFrlCapacityComputation__)(struct DispCommon *, NV0073_CTRL_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSpecificDispI2cReadWrite__)(struct DispCommon *, NV0073_CTRL_SPECIFIC_DISP_I2C_READ_WRITE_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSpecificGetValidHeadWindowAssignment__)(struct DispCommon *, NV0073_CTRL_SPECIFIC_GET_VALID_HEAD_WINDOW_ASSIGNMENT_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdSpecificSetHdmiAudioMutestream__)(struct DispCommon *, NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_AUDIO_MUTESTREAM_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDfpGetInfo__)(struct DispCommon *, NV0073_CTRL_DFP_GET_INFO_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDfpGetDisplayportDongleInfo__)(struct DispCommon *, NV0073_CTRL_DFP_GET_DISPLAYPORT_DONGLE_INFO_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDfpSetEldAudioCaps__)(struct DispCommon *, NV0073_CTRL_DFP_SET_ELD_AUDIO_CAP_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDfpSetAudioEnable__)(struct DispCommon *, NV0073_CTRL_DFP_SET_AUDIO_ENABLE_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDfpUpdateDynamicDfpCache__)(struct DispCommon *, NV0073_CTRL_DFP_UPDATE_DYNAMIC_DFP_CACHE_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDfpAssignSor__)(struct DispCommon *, NV0073_CTRL_DFP_ASSIGN_SOR_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDfpDscCrcControl__)(struct DispCommon *, NV0073_CTRL_DFP_DSC_CRC_CONTROL_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDfpInitMuxData__)(struct DispCommon *, NV0073_CTRL_CMD_DFP_INIT_MUX_DATA_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDfpGetDsiModeTiming__)(struct DispCommon *, NV0073_CTRL_CMD_DFP_GET_DSI_MODE_TIMING_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDfpConfigTwoHeadOneOr__)(struct DispCommon *, NV0073_CTRL_DFP_CONFIG_TWO_HEAD_ONE_OR_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDfpGetPadlinkMask__)(struct DispCommon *, NV0073_CTRL_DFP_GET_PADLINK_MASK_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDfpGetFixedModeTiming__)(struct DispCommon *, NV0073_CTRL_DFP_GET_FIXED_MODE_TIMING_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDpAuxchCtrl__)(struct DispCommon *, NV0073_CTRL_DP_AUXCH_CTRL_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDpAuxchSetSema__)(struct DispCommon *, NV0073_CTRL_DP_AUXCH_SET_SEMA_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDpCtrl__)(struct DispCommon *, NV0073_CTRL_DP_CTRL_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDpGetLaneData__)(struct DispCommon *, NV0073_CTRL_DP_LANE_DATA_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDpSetLaneData__)(struct DispCommon *, NV0073_CTRL_DP_LANE_DATA_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDpSetTestpattern__)(struct DispCommon *, NV0073_CTRL_DP_SET_TESTPATTERN_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDpMainLinkCtrl__)(struct DispCommon *, NV0073_CTRL_DP_MAIN_LINK_CTRL_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDpSetAudioMuteStream__)(struct DispCommon *, NV0073_CTRL_DP_SET_AUDIO_MUTESTREAM_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDpGetLinkConfig__)(struct DispCommon *, NV0073_CTRL_DP_GET_LINK_CONFIG_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDpGetEDPData__)(struct DispCommon *, NV0073_CTRL_DP_GET_EDP_DATA_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDpTopologyAllocateDisplayId__)(struct DispCommon *, NV0073_CTRL_CMD_DP_TOPOLOGY_ALLOCATE_DISPLAYID_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDpTopologyFreeDisplayId__)(struct DispCommon *, NV0073_CTRL_CMD_DP_TOPOLOGY_FREE_DISPLAYID_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDpConfigStream__)(struct DispCommon *, NV0073_CTRL_CMD_DP_CONFIG_STREAM_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDpConfigSingleHeadMultiStream__)(struct DispCommon *, NV0073_CTRL_CMD_DP_CONFIG_SINGLE_HEAD_MULTI_STREAM_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDpSetRateGov__)(struct DispCommon *, NV0073_CTRL_CMD_DP_SET_RATE_GOV_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDpSendACT__)(struct DispCommon *, NV0073_CTRL_CMD_DP_SEND_ACT_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDpSetManualDisplayPort__)(struct DispCommon *, NV0073_CTRL_CMD_DP_SET_MANUAL_DISPLAYPORT_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDpGetCaps__)(struct DispCommon *, NV0073_CTRL_CMD_DP_GET_CAPS_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDpSetMSAPropertiesv2__)(struct DispCommon *, NV0073_CTRL_CMD_DP_SET_MSA_PROPERTIES_V2_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDpSetStereoMSAProperties__)(struct DispCommon *, NV0073_CTRL_CMD_DP_SET_STEREO_MSA_PROPERTIES_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDpGenerateFakeInterrupt__)(struct DispCommon *, NV0073_CTRL_CMD_DP_GENERATE_FAKE_INTERRUPT_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDpConfigRadScratchReg__)(struct DispCommon *, NV0073_CTRL_CMD_DP_CONFIG_RAD_SCRATCH_REG_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDpSetTriggerSelect__)(struct DispCommon *, NV0073_CTRL_CMD_DP_SET_TRIGGER_SELECT_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDpSetTriggerAll__)(struct DispCommon *, NV0073_CTRL_CMD_DP_SET_TRIGGER_ALL_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDpGetAuxLogData__)(struct DispCommon *, NV0073_CTRL_CMD_DP_GET_AUXLOGGER_BUFFER_DATA_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDpConfigIndexedLinkRates__)(struct DispCommon *, NV0073_CTRL_CMD_DP_CONFIG_INDEXED_LINK_RATES_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDpConfigureFec__)(struct DispCommon *, NV0073_CTRL_CMD_DP_CONFIGURE_FEC_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDpGetGenericInfoframe__)(struct DispCommon *, NV0073_CTRL_DP_GET_GENERIC_INFOFRAME_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDpGetMsaAttributes__)(struct DispCommon *, NV0073_CTRL_DP_GET_MSA_ATTRIBUTES_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDpConfigMacroPad__)(struct DispCommon *, NV0073_CTRL_CMD_DP_CONFIG_MACRO_PAD_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDpSetPreemphasisDrivecurrentPostcursor2Data__)(struct DispCommon *, NV0073_CTRL_DP_SET_PREEMPHASIS_DRIVECURRENT_POSTCURSOR2_DATA_PARAMS *);
    NV_STATUS (*__dispcmnCtrlCmdDpGetPreemphasisDrivecurrentPostcursor2Data__)(struct DispCommon *, NV0073_CTRL_DP_GET_PREEMPHASIS_DRIVECURRENT_POSTCURSOR2_DATA_PARAMS *);
    NvBool (*__dispcmnShareCallback__)(struct DispCommon *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__dispcmnCheckMemInterUnmap__)(struct DispCommon *, NvBool);
    NV_STATUS (*__dispcmnMapTo__)(struct DispCommon *, RS_RES_MAP_TO_PARAMS *);
    NvBool (*__dispcmnAccessCallback__)(struct DispCommon *, struct RsClient *, void *, RsAccessRight);
    void (*__dispcmnSetNotificationShare__)(struct DispCommon *, struct NotifShare *);
    NvU32 (*__dispcmnGetRefCount__)(struct DispCommon *);
    void (*__dispcmnAddAdditionalDependants__)(struct RsClient *, struct DispCommon *, RsResourceRef *);
    NV_STATUS (*__dispcmnControl_Prologue__)(struct DispCommon *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__dispcmnUnmapFrom__)(struct DispCommon *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__dispcmnControl_Epilogue__)(struct DispCommon *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__dispcmnControlLookup__)(struct DispCommon *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NV_STATUS (*__dispcmnControl__)(struct DispCommon *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__dispcmnUnmap__)(struct DispCommon *, struct CALL_CONTEXT *, RsCpuMapping *);
    NV_STATUS (*__dispcmnGetMemInterMapParams__)(struct DispCommon *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__dispcmnGetMemoryMappingDescriptor__)(struct DispCommon *, struct MEMORY_DESCRIPTOR **);
    NV_STATUS (*__dispcmnControlFilter__)(struct DispCommon *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__dispcmnUnregisterEvent__)(struct DispCommon *, NvHandle, NvHandle, NvHandle, NvHandle);
    NV_STATUS (*__dispcmnControlSerialization_Prologue__)(struct DispCommon *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__dispcmnCanCopy__)(struct DispCommon *);
    void (*__dispcmnPreDestruct__)(struct DispCommon *);
    NV_STATUS (*__dispcmnIsDuplicate__)(struct DispCommon *, NvHandle, NvBool *);
    void (*__dispcmnControlSerialization_Epilogue__)(struct DispCommon *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    PEVENTNOTIFICATION *(*__dispcmnGetNotificationListPtr__)(struct DispCommon *);
    struct NotifShare *(*__dispcmnGetNotificationShare__)(struct DispCommon *);
    NV_STATUS (*__dispcmnMap__)(struct DispCommon *, struct CALL_CONTEXT *, RS_CPU_MAP_PARAMS *, RsCpuMapping *);
    NV_STATUS (*__dispcmnGetOrAllocNotifShare__)(struct DispCommon *, NvHandle, NvHandle, struct NotifShare **);
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

#define dispcmnCtrlCmdSystemGetVblankCounter(pDispCommon, pVBCounterParams) dispcmnCtrlCmdSystemGetVblankCounter_DISPATCH(pDispCommon, pVBCounterParams)
#define dispcmnCtrlCmdSystemGetVblankEnable(pDispCommon, pVBEnableParams) dispcmnCtrlCmdSystemGetVblankEnable_DISPATCH(pDispCommon, pVBEnableParams)
#define dispcmnCtrlCmdSystemCheckSidebandSrSupport(pDispCommon, pParams) dispcmnCtrlCmdSystemCheckSidebandSrSupport_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSystemGetHotplugState(pDispCommon, pHotplugParams) dispcmnCtrlCmdSystemGetHotplugState_DISPATCH(pDispCommon, pHotplugParams)
#define dispcmnCtrlCmdSystemGetInternalDisplays(pDispCommon, pInternalDisplaysParams) dispcmnCtrlCmdSystemGetInternalDisplays_DISPATCH(pDispCommon, pInternalDisplaysParams)
#define dispcmnCtrlCmdSystemGetConnectorTable(pDispCommon, pParams) dispcmnCtrlCmdSystemGetConnectorTable_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSystemVrrDisplayInfo(pDispCommon, pParams) dispcmnCtrlCmdSystemVrrDisplayInfo_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdVRRSetRgLineActive(pDispCommon, pParams) dispcmnCtrlCmdVRRSetRgLineActive_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdInternalVRRSetRgLineActive(pDispCommon, pParams) dispcmnCtrlCmdInternalVRRSetRgLineActive_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpEnableVrr(pDispCommon, pParams) dispcmnCtrlCmdDpEnableVrr_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdClearELVBlock(pDispCommon, pParams) dispcmnCtrlCmdClearELVBlock_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSpecificDisplayChange(pDispCommon, pParams) dispcmnCtrlCmdSpecificDisplayChange_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDfpGetSpreadSpectrum(pDispCommon, pParams) dispcmnCtrlCmdDfpGetSpreadSpectrum_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDfpGetLcdGpioPinNum(pDispCommon, pParams) dispcmnCtrlCmdDfpGetLcdGpioPinNum_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpGetAudioMuteStream(pDispCommon, pParams) dispcmnCtrlCmdDpGetAudioMuteStream_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpAuxchI2cTransferCtrl(pDispCommon, pParams) dispcmnCtrlCmdDpAuxchI2cTransferCtrl_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpASSRCtrl(pDispCommon, pParams) dispcmnCtrlCmdDpASSRCtrl_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpSetEcf(pDispCommon, pCtrlEcfParams) dispcmnCtrlCmdDpSetEcf_DISPATCH(pDispCommon, pCtrlEcfParams)
#define dispcmnCtrlCmdStereoDongleSupported(pDispCommon, pCtrlStereoParams) dispcmnCtrlCmdStereoDongleSupported_DISPATCH(pDispCommon, pCtrlStereoParams)
#define dispcmnCtrlCmdDfpRecordChannelRegisters(pDispCommon, pParams) dispcmnCtrlCmdDfpRecordChannelRegisters_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSpecificGetBacklightBrightness(pDispCommon, pAllHeadMaskParams) dispcmnCtrlCmdSpecificGetBacklightBrightness_DISPATCH(pDispCommon, pAllHeadMaskParams)
#define dispcmnCtrlCmdSpecificSetBacklightBrightness(pDispCommon, pParams) dispcmnCtrlCmdSpecificSetBacklightBrightness_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdPsrGetSrPanelInfo(pDispCommon, pParams) dispcmnCtrlCmdPsrGetSrPanelInfo_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDfpSwitchDispMux(pDispCommon, pParams) dispcmnCtrlCmdDfpSwitchDispMux_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDfpRunPreDispMuxOperations(pDispCommon, pParams) dispcmnCtrlCmdDfpRunPreDispMuxOperations_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDfpRunPostDispMuxOperations(pDispCommon, pParams) dispcmnCtrlCmdDfpRunPostDispMuxOperations_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDfpGetDispMuxStatus(pDispCommon, pParams) dispcmnCtrlCmdDfpGetDispMuxStatus_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDfpInternalLcdOverdrive(pDispCommon, pParams) dispcmnCtrlCmdDfpInternalLcdOverdrive_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDfpExecuteInternalLcdOverDrivePolicy(pDispCommon, pParams) dispcmnCtrlCmdDfpExecuteInternalLcdOverDrivePolicy_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSystemExecuteAcpiMethod(pDispCommon, pAcpiMethodParams) dispcmnCtrlCmdSystemExecuteAcpiMethod_DISPATCH(pDispCommon, pAcpiMethodParams)
#define dispcmnCtrlCmdSystemGetAcpiIdMap(pDispCommon, pAcpiIdMapParams) dispcmnCtrlCmdSystemGetAcpiIdMap_DISPATCH(pDispCommon, pAcpiIdMapParams)
#define dispcmnCtrlCmdSystemAcpiSubsystemActivated(pDispCommon, pParams) dispcmnCtrlCmdSystemAcpiSubsystemActivated_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSpecificSetAcpiIdMapping(pDispCommon, pParams) dispcmnCtrlCmdSpecificSetAcpiIdMapping_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSpecificGetAcpiDodDisplayPortAttachment(pDispCommon, pParams) dispcmnCtrlCmdSpecificGetAcpiDodDisplayPortAttachment_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSystemGetCapsV2(pDispCommon, pCapsParams) dispcmnCtrlCmdSystemGetCapsV2_DISPATCH(pDispCommon, pCapsParams)
#define dispcmnCtrlCmdSystemGetNumHeads(pDispCommon, pNumHeadsParams) dispcmnCtrlCmdSystemGetNumHeads_DISPATCH(pDispCommon, pNumHeadsParams)
#define dispcmnCtrlCmdSystemGetScanline(pDispCommon, pScanlineParams) dispcmnCtrlCmdSystemGetScanline_DISPATCH(pDispCommon, pScanlineParams)
#define dispcmnCtrlCmdSystemGetSuppported(pDispCommon, pSupportedParams) dispcmnCtrlCmdSystemGetSuppported_DISPATCH(pDispCommon, pSupportedParams)
#define dispcmnCtrlCmdSystemGetConnectState(pDispCommon, pConnectParams) dispcmnCtrlCmdSystemGetConnectState_DISPATCH(pDispCommon, pConnectParams)
#define dispcmnCtrlCmdSystemGetHotplugUnplugState(pDispCommon, pHotplugParams) dispcmnCtrlCmdSystemGetHotplugUnplugState_DISPATCH(pDispCommon, pHotplugParams)
#define dispcmnCtrlCmdInternalGetHotplugUnplugState(pDispCommon, pHotplugParams) dispcmnCtrlCmdInternalGetHotplugUnplugState_DISPATCH(pDispCommon, pHotplugParams)
#define dispcmnCtrlCmdSystemGetHeadRoutingMap(pDispCommon, pMapParams) dispcmnCtrlCmdSystemGetHeadRoutingMap_DISPATCH(pDispCommon, pMapParams)
#define dispcmnCtrlCmdSystemGetActive(pDispCommon, pActiveParams) dispcmnCtrlCmdSystemGetActive_DISPATCH(pDispCommon, pActiveParams)
#define dispcmnCtrlCmdSystemGetBootDisplays(pDispCommon, pParams) dispcmnCtrlCmdSystemGetBootDisplays_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSystemQueryDisplayIdsWithMux(pDispCommon, pParams) dispcmnCtrlCmdSystemQueryDisplayIdsWithMux_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSystemCheckSidebandI2cSupport(pDispCommon, pParams) dispcmnCtrlCmdSystemCheckSidebandI2cSupport_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSystemAllocateDisplayBandwidth(pDispCommon, pParams) dispcmnCtrlCmdSystemAllocateDisplayBandwidth_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSystemGetHotplugConfig(pDispCommon, pHotplugParams) dispcmnCtrlCmdSystemGetHotplugConfig_DISPATCH(pDispCommon, pHotplugParams)
#define dispcmnCtrlCmdSystemGetHotplugEventConfig(pDispCommon, pParams) dispcmnCtrlCmdSystemGetHotplugEventConfig_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSystemSetHotplugEventConfig(pDispCommon, pParams) dispcmnCtrlCmdSystemSetHotplugEventConfig_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSystemArmLightweightSupervisor(pDispCommon, pParams) dispcmnCtrlCmdSystemArmLightweightSupervisor_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSystemConfigVrrPstateSwitch(pDispCommon, pParams) dispcmnCtrlCmdSystemConfigVrrPstateSwitch_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSpecificGetType(pDispCommon, pDisplayTypeParams) dispcmnCtrlCmdSpecificGetType_DISPATCH(pDispCommon, pDisplayTypeParams)
#define dispcmnCtrlCmdSpecificGetEdidV2(pDispCommon, pEdidParams) dispcmnCtrlCmdSpecificGetEdidV2_DISPATCH(pDispCommon, pEdidParams)
#define dispcmnCtrlCmdSpecificSetEdidV2(pDispCommon, pEdidParams) dispcmnCtrlCmdSpecificSetEdidV2_DISPATCH(pDispCommon, pEdidParams)
#define dispcmnCtrlCmdSpecificFakeDevice(pDispCommon, pTestParams) dispcmnCtrlCmdSpecificFakeDevice_DISPATCH(pDispCommon, pTestParams)
#define dispcmnCtrlCmdSpecificGetConnectorData(pDispCommon, pConnectorParams) dispcmnCtrlCmdSpecificGetConnectorData_DISPATCH(pDispCommon, pConnectorParams)
#define dispcmnCtrlCmdSpecificSetHdmiEnable(pDispCommon, pParams) dispcmnCtrlCmdSpecificSetHdmiEnable_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSpecificCtrlHdmi(pDispCommon, pParams) dispcmnCtrlCmdSpecificCtrlHdmi_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSpecificGetAllHeadMask(pDispCommon, pAllHeadMaskParams) dispcmnCtrlCmdSpecificGetAllHeadMask_DISPATCH(pDispCommon, pAllHeadMaskParams)
#define dispcmnCtrlCmdSpecificSetOdPacket(pDispCommon, pParams) dispcmnCtrlCmdSpecificSetOdPacket_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSpecificAcquireSharedGenericPacket(pDispCommon, pParams) dispcmnCtrlCmdSpecificAcquireSharedGenericPacket_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSpecificSetSharedGenericPacket(pDispCommon, pParams) dispcmnCtrlCmdSpecificSetSharedGenericPacket_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSpecificReleaseSharedGenericPacket(pDispCommon, pParams) dispcmnCtrlCmdSpecificReleaseSharedGenericPacket_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSpecificSetOdPacketCtrl(pDispCommon, pParams) dispcmnCtrlCmdSpecificSetOdPacketCtrl_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSpecificOrGetInfo(pDispCommon, pParams) dispcmnCtrlCmdSpecificOrGetInfo_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSpecificGetPclkLimit(pDispCommon, pParams) dispcmnCtrlCmdSpecificGetPclkLimit_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSpecificSetHdmiSinkCaps(pDispCommon, pParams) dispcmnCtrlCmdSpecificSetHdmiSinkCaps_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSpecificSetMonitorPower(pDispCommon, setMonitorPowerParams) dispcmnCtrlCmdSpecificSetMonitorPower_DISPATCH(pDispCommon, setMonitorPowerParams)
#define dispcmnCtrlCmdSpecificSetHdmiFrlLinkConfig(pDispCommon, pParams) dispcmnCtrlCmdSpecificSetHdmiFrlLinkConfig_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSpecificApplyEdidOverrideV2(pDispCommon, pEdidOverrideParams) dispcmnCtrlCmdSpecificApplyEdidOverrideV2_DISPATCH(pDispCommon, pEdidOverrideParams)
#define dispcmnCtrlCmdSpecificGetI2cPortid(pDispCommon, pParams) dispcmnCtrlCmdSpecificGetI2cPortid_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSpecificGetHdmiGpuCaps(pDispCommon, pParams) dispcmnCtrlCmdSpecificGetHdmiGpuCaps_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSpecificGetHdmiScdcData(pDispCommon, pParams) dispcmnCtrlCmdSpecificGetHdmiScdcData_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSpecificIsDirectmodeDisplay(pDispCommon, pParams) dispcmnCtrlCmdSpecificIsDirectmodeDisplay_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSpecificDefaultAdaptivesyncDisplay(pDispCommon, pParams) dispcmnCtrlCmdSpecificDefaultAdaptivesyncDisplay_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSpecificSetHdmiFrlCapacityComputation(pDispCommon, pParams) dispcmnCtrlCmdSpecificSetHdmiFrlCapacityComputation_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSpecificDispI2cReadWrite(pDispCommon, pParams) dispcmnCtrlCmdSpecificDispI2cReadWrite_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSpecificGetValidHeadWindowAssignment(pDispCommon, pParams) dispcmnCtrlCmdSpecificGetValidHeadWindowAssignment_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdSpecificSetHdmiAudioMutestream(pDispCommon, pParams) dispcmnCtrlCmdSpecificSetHdmiAudioMutestream_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDfpGetInfo(pDispCommon, pParams) dispcmnCtrlCmdDfpGetInfo_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDfpGetDisplayportDongleInfo(pDispCommon, pParams) dispcmnCtrlCmdDfpGetDisplayportDongleInfo_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDfpSetEldAudioCaps(pDispCommon, pEldAudioCapsParams) dispcmnCtrlCmdDfpSetEldAudioCaps_DISPATCH(pDispCommon, pEldAudioCapsParams)
#define dispcmnCtrlCmdDfpSetAudioEnable(pDispCommon, pParams) dispcmnCtrlCmdDfpSetAudioEnable_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDfpUpdateDynamicDfpCache(pDispCommon, pParams) dispcmnCtrlCmdDfpUpdateDynamicDfpCache_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDfpAssignSor(pDispCommon, pParams) dispcmnCtrlCmdDfpAssignSor_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDfpDscCrcControl(pDispCommon, pParams) dispcmnCtrlCmdDfpDscCrcControl_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDfpInitMuxData(pDispCommon, pParams) dispcmnCtrlCmdDfpInitMuxData_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDfpGetDsiModeTiming(pDispCommon, pParams) dispcmnCtrlCmdDfpGetDsiModeTiming_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDfpConfigTwoHeadOneOr(pDispCommon, pParams) dispcmnCtrlCmdDfpConfigTwoHeadOneOr_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDfpGetPadlinkMask(pDispCommon, pParams) dispcmnCtrlCmdDfpGetPadlinkMask_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDfpGetFixedModeTiming(pDispCommon, pParams) dispcmnCtrlCmdDfpGetFixedModeTiming_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpAuxchCtrl(pDispCommon, pAuxchCtrlParams) dispcmnCtrlCmdDpAuxchCtrl_DISPATCH(pDispCommon, pAuxchCtrlParams)
#define dispcmnCtrlCmdDpAuxchSetSema(pDispCommon, pSemaParams) dispcmnCtrlCmdDpAuxchSetSema_DISPATCH(pDispCommon, pSemaParams)
#define dispcmnCtrlCmdDpCtrl(pDispCommon, pParams) dispcmnCtrlCmdDpCtrl_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpGetLaneData(pDispCommon, pParams) dispcmnCtrlCmdDpGetLaneData_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpSetLaneData(pDispCommon, pParams) dispcmnCtrlCmdDpSetLaneData_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpSetTestpattern(pDispCommon, pParams) dispcmnCtrlCmdDpSetTestpattern_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpMainLinkCtrl(pDispCommon, pParams) dispcmnCtrlCmdDpMainLinkCtrl_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpSetAudioMuteStream(pDispCommon, pParams) dispcmnCtrlCmdDpSetAudioMuteStream_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpGetLinkConfig(pDispCommon, pParams) dispcmnCtrlCmdDpGetLinkConfig_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpGetEDPData(pDispCommon, pParams) dispcmnCtrlCmdDpGetEDPData_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpTopologyAllocateDisplayId(pDispCommon, pParams) dispcmnCtrlCmdDpTopologyAllocateDisplayId_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpTopologyFreeDisplayId(pDispCommon, pParams) dispcmnCtrlCmdDpTopologyFreeDisplayId_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpConfigStream(pDispCommon, pParams) dispcmnCtrlCmdDpConfigStream_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpConfigSingleHeadMultiStream(pDispCommon, pParams) dispcmnCtrlCmdDpConfigSingleHeadMultiStream_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpSetRateGov(pDispCommon, pParams) dispcmnCtrlCmdDpSetRateGov_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpSendACT(pDispCommon, pParams) dispcmnCtrlCmdDpSendACT_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpSetManualDisplayPort(pDispCommon, pParams) dispcmnCtrlCmdDpSetManualDisplayPort_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpGetCaps(pDispCommon, pParams) dispcmnCtrlCmdDpGetCaps_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpSetMSAPropertiesv2(pDispCommon, pParams) dispcmnCtrlCmdDpSetMSAPropertiesv2_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpSetStereoMSAProperties(pDispCommon, pParams) dispcmnCtrlCmdDpSetStereoMSAProperties_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpGenerateFakeInterrupt(pDispCommon, pParams) dispcmnCtrlCmdDpGenerateFakeInterrupt_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpConfigRadScratchReg(pDispCommon, pParams) dispcmnCtrlCmdDpConfigRadScratchReg_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpSetTriggerSelect(pDispCommon, pTriggerSelectParams) dispcmnCtrlCmdDpSetTriggerSelect_DISPATCH(pDispCommon, pTriggerSelectParams)
#define dispcmnCtrlCmdDpSetTriggerAll(pDispCommon, pTriggerAllParams) dispcmnCtrlCmdDpSetTriggerAll_DISPATCH(pDispCommon, pTriggerAllParams)
#define dispcmnCtrlCmdDpGetAuxLogData(pDispCommon, pDpAuxBufferWrapper) dispcmnCtrlCmdDpGetAuxLogData_DISPATCH(pDispCommon, pDpAuxBufferWrapper)
#define dispcmnCtrlCmdDpConfigIndexedLinkRates(pDispCommon, pParams) dispcmnCtrlCmdDpConfigIndexedLinkRates_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpConfigureFec(pDispCommon, pParams) dispcmnCtrlCmdDpConfigureFec_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpGetGenericInfoframe(pDispCommon, pParams) dispcmnCtrlCmdDpGetGenericInfoframe_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpGetMsaAttributes(pDispCommon, pParams) dispcmnCtrlCmdDpGetMsaAttributes_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpConfigMacroPad(pDispCommon, pParams) dispcmnCtrlCmdDpConfigMacroPad_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpSetPreemphasisDrivecurrentPostcursor2Data(pDispCommon, pParams) dispcmnCtrlCmdDpSetPreemphasisDrivecurrentPostcursor2Data_DISPATCH(pDispCommon, pParams)
#define dispcmnCtrlCmdDpGetPreemphasisDrivecurrentPostcursor2Data(pDispCommon, pParams) dispcmnCtrlCmdDpGetPreemphasisDrivecurrentPostcursor2Data_DISPATCH(pDispCommon, pParams)
#define dispcmnShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) dispcmnShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define dispcmnCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) dispcmnCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define dispcmnMapTo(pResource, pParams) dispcmnMapTo_DISPATCH(pResource, pParams)
#define dispcmnAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) dispcmnAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define dispcmnSetNotificationShare(pNotifier, pNotifShare) dispcmnSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define dispcmnGetRefCount(pResource) dispcmnGetRefCount_DISPATCH(pResource)
#define dispcmnAddAdditionalDependants(pClient, pResource, pReference) dispcmnAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define dispcmnControl_Prologue(pDisplayApi, pCallContext, pRsParams) dispcmnControl_Prologue_DISPATCH(pDisplayApi, pCallContext, pRsParams)
#define dispcmnUnmapFrom(pResource, pParams) dispcmnUnmapFrom_DISPATCH(pResource, pParams)
#define dispcmnControl_Epilogue(pDisplayApi, pCallContext, pRsParams) dispcmnControl_Epilogue_DISPATCH(pDisplayApi, pCallContext, pRsParams)
#define dispcmnControlLookup(pResource, pParams, ppEntry) dispcmnControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define dispcmnControl(pDisplayApi, pCallContext, pParams) dispcmnControl_DISPATCH(pDisplayApi, pCallContext, pParams)
#define dispcmnUnmap(pResource, pCallContext, pCpuMapping) dispcmnUnmap_DISPATCH(pResource, pCallContext, pCpuMapping)
#define dispcmnGetMemInterMapParams(pRmResource, pParams) dispcmnGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define dispcmnGetMemoryMappingDescriptor(pRmResource, ppMemDesc) dispcmnGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define dispcmnControlFilter(pResource, pCallContext, pParams) dispcmnControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define dispcmnUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) dispcmnUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define dispcmnControlSerialization_Prologue(pResource, pCallContext, pParams) dispcmnControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define dispcmnCanCopy(pResource) dispcmnCanCopy_DISPATCH(pResource)
#define dispcmnPreDestruct(pResource) dispcmnPreDestruct_DISPATCH(pResource)
#define dispcmnIsDuplicate(pResource, hMemory, pDuplicate) dispcmnIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define dispcmnControlSerialization_Epilogue(pResource, pCallContext, pParams) dispcmnControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define dispcmnGetNotificationListPtr(pNotifier) dispcmnGetNotificationListPtr_DISPATCH(pNotifier)
#define dispcmnGetNotificationShare(pNotifier) dispcmnGetNotificationShare_DISPATCH(pNotifier)
#define dispcmnMap(pResource, pCallContext, pParams, pCpuMapping) dispcmnMap_DISPATCH(pResource, pCallContext, pParams, pCpuMapping)
#define dispcmnGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) dispcmnGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)
NV_STATUS dispcmnCtrlCmdSystemGetVblankCounter_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_VBLANK_COUNTER_PARAMS *pVBCounterParams);

static inline NV_STATUS dispcmnCtrlCmdSystemGetVblankCounter_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_VBLANK_COUNTER_PARAMS *pVBCounterParams) {
    return pDispCommon->__dispcmnCtrlCmdSystemGetVblankCounter__(pDispCommon, pVBCounterParams);
}

NV_STATUS dispcmnCtrlCmdSystemGetVblankEnable_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_VBLANK_ENABLE_PARAMS *pVBEnableParams);

static inline NV_STATUS dispcmnCtrlCmdSystemGetVblankEnable_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_VBLANK_ENABLE_PARAMS *pVBEnableParams) {
    return pDispCommon->__dispcmnCtrlCmdSystemGetVblankEnable__(pDispCommon, pVBEnableParams);
}

NV_STATUS dispcmnCtrlCmdSystemCheckSidebandSrSupport_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_SYSTEM_CHECK_SIDEBAND_SR_SUPPORT_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdSystemCheckSidebandSrSupport_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_SYSTEM_CHECK_SIDEBAND_SR_SUPPORT_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSystemCheckSidebandSrSupport__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdSystemGetHotplugState_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_HOTPLUG_STATE_PARAMS *pHotplugParams);

static inline NV_STATUS dispcmnCtrlCmdSystemGetHotplugState_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_HOTPLUG_STATE_PARAMS *pHotplugParams) {
    return pDispCommon->__dispcmnCtrlCmdSystemGetHotplugState__(pDispCommon, pHotplugParams);
}

NV_STATUS dispcmnCtrlCmdSystemGetInternalDisplays_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_INTERNAL_DISPLAYS_PARAMS *pInternalDisplaysParams);

static inline NV_STATUS dispcmnCtrlCmdSystemGetInternalDisplays_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_INTERNAL_DISPLAYS_PARAMS *pInternalDisplaysParams) {
    return pDispCommon->__dispcmnCtrlCmdSystemGetInternalDisplays__(pDispCommon, pInternalDisplaysParams);
}

NV_STATUS dispcmnCtrlCmdSystemGetConnectorTable_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_CONNECTOR_TABLE_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdSystemGetConnectorTable_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_CONNECTOR_TABLE_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSystemGetConnectorTable__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdSystemVrrDisplayInfo_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_VRR_DISPLAY_INFO_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdSystemVrrDisplayInfo_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_VRR_DISPLAY_INFO_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSystemVrrDisplayInfo__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdVRRSetRgLineActive_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_SYSTEM_VRR_SET_RGLINE_ACTIVE_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdVRRSetRgLineActive_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_SYSTEM_VRR_SET_RGLINE_ACTIVE_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdVRRSetRgLineActive__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdInternalVRRSetRgLineActive_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_SYSTEM_VRR_SET_RGLINE_ACTIVE_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdInternalVRRSetRgLineActive_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_SYSTEM_VRR_SET_RGLINE_ACTIVE_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdInternalVRRSetRgLineActive__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdDpEnableVrr_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_ENABLE_VRR_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdDpEnableVrr_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_ENABLE_VRR_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpEnableVrr__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdClearELVBlock_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_CLEAR_ELV_BLOCK_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdClearELVBlock_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_CLEAR_ELV_BLOCK_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdClearELVBlock__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdSpecificDisplayChange_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_DISPLAY_CHANGE_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdSpecificDisplayChange_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_DISPLAY_CHANGE_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificDisplayChange__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdDfpGetSpreadSpectrum_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DFP_GET_SPREAD_SPECTRUM_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdDfpGetSpreadSpectrum_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DFP_GET_SPREAD_SPECTRUM_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDfpGetSpreadSpectrum__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdDfpGetLcdGpioPinNum_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DFP_GET_LCD_GPIO_PIN_NUM_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdDfpGetLcdGpioPinNum_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DFP_GET_LCD_GPIO_PIN_NUM_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDfpGetLcdGpioPinNum__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdDpGetAudioMuteStream_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DP_GET_AUDIO_MUTESTREAM_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdDpGetAudioMuteStream_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DP_GET_AUDIO_MUTESTREAM_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpGetAudioMuteStream__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdDpAuxchI2cTransferCtrl_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DP_AUXCH_I2C_TRANSFER_CTRL_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdDpAuxchI2cTransferCtrl_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DP_AUXCH_I2C_TRANSFER_CTRL_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpAuxchI2cTransferCtrl__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdDpASSRCtrl_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DP_ASSR_CTRL_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdDpASSRCtrl_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DP_ASSR_CTRL_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpASSRCtrl__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdDpSetEcf_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_SET_ECF_PARAMS *pCtrlEcfParams);

static inline NV_STATUS dispcmnCtrlCmdDpSetEcf_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_SET_ECF_PARAMS *pCtrlEcfParams) {
    return pDispCommon->__dispcmnCtrlCmdDpSetEcf__(pDispCommon, pCtrlEcfParams);
}

NV_STATUS dispcmnCtrlCmdStereoDongleSupported_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_STEREO_DONGLE_SUPPORTED_PARAMS *pCtrlStereoParams);

static inline NV_STATUS dispcmnCtrlCmdStereoDongleSupported_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_STEREO_DONGLE_SUPPORTED_PARAMS *pCtrlStereoParams) {
    return pDispCommon->__dispcmnCtrlCmdStereoDongleSupported__(pDispCommon, pCtrlStereoParams);
}

NV_STATUS dispcmnCtrlCmdDfpRecordChannelRegisters_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_SYSTEM_RECORD_CHANNEL_REGS_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdDfpRecordChannelRegisters_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_SYSTEM_RECORD_CHANNEL_REGS_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDfpRecordChannelRegisters__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdSpecificGetBacklightBrightness_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_BACKLIGHT_BRIGHTNESS_PARAMS *pAllHeadMaskParams);

static inline NV_STATUS dispcmnCtrlCmdSpecificGetBacklightBrightness_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_BACKLIGHT_BRIGHTNESS_PARAMS *pAllHeadMaskParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificGetBacklightBrightness__(pDispCommon, pAllHeadMaskParams);
}

NV_STATUS dispcmnCtrlCmdSpecificSetBacklightBrightness_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_BACKLIGHT_BRIGHTNESS_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdSpecificSetBacklightBrightness_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_BACKLIGHT_BRIGHTNESS_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificSetBacklightBrightness__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdPsrGetSrPanelInfo_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_PSR_GET_SR_PANEL_INFO_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdPsrGetSrPanelInfo_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_PSR_GET_SR_PANEL_INFO_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdPsrGetSrPanelInfo__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdDfpSwitchDispMux_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DFP_SWITCH_DISP_MUX_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdDfpSwitchDispMux_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DFP_SWITCH_DISP_MUX_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDfpSwitchDispMux__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdDfpRunPreDispMuxOperations_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DFP_RUN_PRE_DISP_MUX_OPERATIONS_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdDfpRunPreDispMuxOperations_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DFP_RUN_PRE_DISP_MUX_OPERATIONS_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDfpRunPreDispMuxOperations__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdDfpRunPostDispMuxOperations_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DFP_RUN_POST_DISP_MUX_OPERATIONS_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdDfpRunPostDispMuxOperations_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DFP_RUN_POST_DISP_MUX_OPERATIONS_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDfpRunPostDispMuxOperations__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdDfpGetDispMuxStatus_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DFP_GET_DISP_MUX_STATUS_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdDfpGetDispMuxStatus_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DFP_GET_DISP_MUX_STATUS_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDfpGetDispMuxStatus__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdDfpInternalLcdOverdrive_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_AUXCH_OD_CTRL_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdDfpInternalLcdOverdrive_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_AUXCH_OD_CTRL_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDfpInternalLcdOverdrive__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdDfpExecuteInternalLcdOverDrivePolicy_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DP_EXECUTE_OVERDRIVE_POLICY_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdDfpExecuteInternalLcdOverDrivePolicy_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DP_EXECUTE_OVERDRIVE_POLICY_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDfpExecuteInternalLcdOverDrivePolicy__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdSystemExecuteAcpiMethod_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_PARAMS *pAcpiMethodParams);

static inline NV_STATUS dispcmnCtrlCmdSystemExecuteAcpiMethod_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_PARAMS *pAcpiMethodParams) {
    return pDispCommon->__dispcmnCtrlCmdSystemExecuteAcpiMethod__(pDispCommon, pAcpiMethodParams);
}

NV_STATUS dispcmnCtrlCmdSystemGetAcpiIdMap_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_ACPI_ID_MAP_PARAMS *pAcpiIdMapParams);

static inline NV_STATUS dispcmnCtrlCmdSystemGetAcpiIdMap_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_ACPI_ID_MAP_PARAMS *pAcpiIdMapParams) {
    return pDispCommon->__dispcmnCtrlCmdSystemGetAcpiIdMap__(pDispCommon, pAcpiIdMapParams);
}

NV_STATUS dispcmnCtrlCmdSystemAcpiSubsystemActivated_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_ACPI_SUBSYSTEM_ACTIVATED_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdSystemAcpiSubsystemActivated_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_ACPI_SUBSYSTEM_ACTIVATED_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSystemAcpiSubsystemActivated__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdSpecificSetAcpiIdMapping_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_SET_ACPI_ID_MAPPING_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdSpecificSetAcpiIdMapping_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_SET_ACPI_ID_MAPPING_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificSetAcpiIdMapping__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdSpecificGetAcpiDodDisplayPortAttachment_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_GET_ACPI_DOD_DISPLAY_PORT_ATTACHMENT_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdSpecificGetAcpiDodDisplayPortAttachment_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_GET_ACPI_DOD_DISPLAY_PORT_ATTACHMENT_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificGetAcpiDodDisplayPortAttachment__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdSystemGetCapsV2_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_CAPS_V2_PARAMS *pCapsParams);

static inline NV_STATUS dispcmnCtrlCmdSystemGetCapsV2_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_CAPS_V2_PARAMS *pCapsParams) {
    return pDispCommon->__dispcmnCtrlCmdSystemGetCapsV2__(pDispCommon, pCapsParams);
}

NV_STATUS dispcmnCtrlCmdSystemGetNumHeads_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_NUM_HEADS_PARAMS *pNumHeadsParams);

static inline NV_STATUS dispcmnCtrlCmdSystemGetNumHeads_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_NUM_HEADS_PARAMS *pNumHeadsParams) {
    return pDispCommon->__dispcmnCtrlCmdSystemGetNumHeads__(pDispCommon, pNumHeadsParams);
}

NV_STATUS dispcmnCtrlCmdSystemGetScanline_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_SCANLINE_PARAMS *pScanlineParams);

static inline NV_STATUS dispcmnCtrlCmdSystemGetScanline_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_SCANLINE_PARAMS *pScanlineParams) {
    return pDispCommon->__dispcmnCtrlCmdSystemGetScanline__(pDispCommon, pScanlineParams);
}

NV_STATUS dispcmnCtrlCmdSystemGetSuppported_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_SUPPORTED_PARAMS *pSupportedParams);

static inline NV_STATUS dispcmnCtrlCmdSystemGetSuppported_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_SUPPORTED_PARAMS *pSupportedParams) {
    return pDispCommon->__dispcmnCtrlCmdSystemGetSuppported__(pDispCommon, pSupportedParams);
}

NV_STATUS dispcmnCtrlCmdSystemGetConnectState_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_CONNECT_STATE_PARAMS *pConnectParams);

static inline NV_STATUS dispcmnCtrlCmdSystemGetConnectState_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_CONNECT_STATE_PARAMS *pConnectParams) {
    return pDispCommon->__dispcmnCtrlCmdSystemGetConnectState__(pDispCommon, pConnectParams);
}

NV_STATUS dispcmnCtrlCmdSystemGetHotplugUnplugState_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_HOTPLUG_UNPLUG_STATE_PARAMS *pHotplugParams);

static inline NV_STATUS dispcmnCtrlCmdSystemGetHotplugUnplugState_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_HOTPLUG_UNPLUG_STATE_PARAMS *pHotplugParams) {
    return pDispCommon->__dispcmnCtrlCmdSystemGetHotplugUnplugState__(pDispCommon, pHotplugParams);
}

NV_STATUS dispcmnCtrlCmdInternalGetHotplugUnplugState_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_HOTPLUG_UNPLUG_STATE_PARAMS *pHotplugParams);

static inline NV_STATUS dispcmnCtrlCmdInternalGetHotplugUnplugState_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_HOTPLUG_UNPLUG_STATE_PARAMS *pHotplugParams) {
    return pDispCommon->__dispcmnCtrlCmdInternalGetHotplugUnplugState__(pDispCommon, pHotplugParams);
}

NV_STATUS dispcmnCtrlCmdSystemGetHeadRoutingMap_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_HEAD_ROUTING_MAP_PARAMS *pMapParams);

static inline NV_STATUS dispcmnCtrlCmdSystemGetHeadRoutingMap_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_HEAD_ROUTING_MAP_PARAMS *pMapParams) {
    return pDispCommon->__dispcmnCtrlCmdSystemGetHeadRoutingMap__(pDispCommon, pMapParams);
}

NV_STATUS dispcmnCtrlCmdSystemGetActive_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_ACTIVE_PARAMS *pActiveParams);

static inline NV_STATUS dispcmnCtrlCmdSystemGetActive_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_ACTIVE_PARAMS *pActiveParams) {
    return pDispCommon->__dispcmnCtrlCmdSystemGetActive__(pDispCommon, pActiveParams);
}

NV_STATUS dispcmnCtrlCmdSystemGetBootDisplays_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_BOOT_DISPLAYS_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdSystemGetBootDisplays_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_BOOT_DISPLAYS_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSystemGetBootDisplays__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdSystemQueryDisplayIdsWithMux_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_SYSTEM_QUERY_DISPLAY_IDS_WITH_MUX_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdSystemQueryDisplayIdsWithMux_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_SYSTEM_QUERY_DISPLAY_IDS_WITH_MUX_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSystemQueryDisplayIdsWithMux__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdSystemCheckSidebandI2cSupport_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_SYSTEM_CHECK_SIDEBAND_I2C_SUPPORT_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdSystemCheckSidebandI2cSupport_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_SYSTEM_CHECK_SIDEBAND_I2C_SUPPORT_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSystemCheckSidebandI2cSupport__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdSystemAllocateDisplayBandwidth_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_ALLOCATE_DISPLAY_BANDWIDTH_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdSystemAllocateDisplayBandwidth_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_ALLOCATE_DISPLAY_BANDWIDTH_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSystemAllocateDisplayBandwidth__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdSystemGetHotplugConfig_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_SET_HOTPLUG_CONFIG_PARAMS *pHotplugParams);

static inline NV_STATUS dispcmnCtrlCmdSystemGetHotplugConfig_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_GET_SET_HOTPLUG_CONFIG_PARAMS *pHotplugParams) {
    return pDispCommon->__dispcmnCtrlCmdSystemGetHotplugConfig__(pDispCommon, pHotplugParams);
}

NV_STATUS dispcmnCtrlCmdSystemGetHotplugEventConfig_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_HOTPLUG_EVENT_CONFIG_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdSystemGetHotplugEventConfig_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_HOTPLUG_EVENT_CONFIG_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSystemGetHotplugEventConfig__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdSystemSetHotplugEventConfig_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_HOTPLUG_EVENT_CONFIG_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdSystemSetHotplugEventConfig_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_HOTPLUG_EVENT_CONFIG_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSystemSetHotplugEventConfig__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdSystemArmLightweightSupervisor_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_ARM_LIGHTWEIGHT_SUPERVISOR_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdSystemArmLightweightSupervisor_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_ARM_LIGHTWEIGHT_SUPERVISOR_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSystemArmLightweightSupervisor__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdSystemConfigVrrPstateSwitch_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_CONFIG_VRR_PSTATE_SWITCH_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdSystemConfigVrrPstateSwitch_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SYSTEM_CONFIG_VRR_PSTATE_SWITCH_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSystemConfigVrrPstateSwitch__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdSpecificGetType_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_GET_TYPE_PARAMS *pDisplayTypeParams);

static inline NV_STATUS dispcmnCtrlCmdSpecificGetType_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_GET_TYPE_PARAMS *pDisplayTypeParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificGetType__(pDispCommon, pDisplayTypeParams);
}

NV_STATUS dispcmnCtrlCmdSpecificGetEdidV2_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_GET_EDID_V2_PARAMS *pEdidParams);

static inline NV_STATUS dispcmnCtrlCmdSpecificGetEdidV2_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_GET_EDID_V2_PARAMS *pEdidParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificGetEdidV2__(pDispCommon, pEdidParams);
}

NV_STATUS dispcmnCtrlCmdSpecificSetEdidV2_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_SET_EDID_V2_PARAMS *pEdidParams);

static inline NV_STATUS dispcmnCtrlCmdSpecificSetEdidV2_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_SET_EDID_V2_PARAMS *pEdidParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificSetEdidV2__(pDispCommon, pEdidParams);
}

NV_STATUS dispcmnCtrlCmdSpecificFakeDevice_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_SPECIFIC_FAKE_DEVICE_PARAMS *pTestParams);

static inline NV_STATUS dispcmnCtrlCmdSpecificFakeDevice_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_SPECIFIC_FAKE_DEVICE_PARAMS *pTestParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificFakeDevice__(pDispCommon, pTestParams);
}

NV_STATUS dispcmnCtrlCmdSpecificGetConnectorData_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_GET_CONNECTOR_DATA_PARAMS *pConnectorParams);

static inline NV_STATUS dispcmnCtrlCmdSpecificGetConnectorData_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_GET_CONNECTOR_DATA_PARAMS *pConnectorParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificGetConnectorData__(pDispCommon, pConnectorParams);
}

NV_STATUS dispcmnCtrlCmdSpecificSetHdmiEnable_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_SET_HDMI_ENABLE_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdSpecificSetHdmiEnable_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_SET_HDMI_ENABLE_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificSetHdmiEnable__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdSpecificCtrlHdmi_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_CTRL_HDMI_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdSpecificCtrlHdmi_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_CTRL_HDMI_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificCtrlHdmi__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdSpecificGetAllHeadMask_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_GET_ALL_HEAD_MASK_PARAMS *pAllHeadMaskParams);

static inline NV_STATUS dispcmnCtrlCmdSpecificGetAllHeadMask_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_GET_ALL_HEAD_MASK_PARAMS *pAllHeadMaskParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificGetAllHeadMask__(pDispCommon, pAllHeadMaskParams);
}

NV_STATUS dispcmnCtrlCmdSpecificSetOdPacket_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_SET_OD_PACKET_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdSpecificSetOdPacket_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_SET_OD_PACKET_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificSetOdPacket__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdSpecificAcquireSharedGenericPacket_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_ACQUIRE_SHARED_GENERIC_PACKET_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdSpecificAcquireSharedGenericPacket_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_ACQUIRE_SHARED_GENERIC_PACKET_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificAcquireSharedGenericPacket__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdSpecificSetSharedGenericPacket_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_SET_SHARED_GENERIC_PACKET_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdSpecificSetSharedGenericPacket_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_SET_SHARED_GENERIC_PACKET_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificSetSharedGenericPacket__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdSpecificReleaseSharedGenericPacket_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_RELEASE_SHARED_GENERIC_PACKET_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdSpecificReleaseSharedGenericPacket_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_RELEASE_SHARED_GENERIC_PACKET_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificReleaseSharedGenericPacket__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdSpecificSetOdPacketCtrl_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_SET_OD_PACKET_CTRL_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdSpecificSetOdPacketCtrl_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_SET_OD_PACKET_CTRL_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificSetOdPacketCtrl__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdSpecificOrGetInfo_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_OR_GET_INFO_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdSpecificOrGetInfo_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_OR_GET_INFO_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificOrGetInfo__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdSpecificGetPclkLimit_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_GET_PCLK_LIMIT_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdSpecificGetPclkLimit_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_GET_PCLK_LIMIT_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificGetPclkLimit__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdSpecificSetHdmiSinkCaps_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_SET_HDMI_SINK_CAPS_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdSpecificSetHdmiSinkCaps_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_SET_HDMI_SINK_CAPS_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificSetHdmiSinkCaps__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdSpecificSetMonitorPower_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_SET_MONITOR_POWER_PARAMS *setMonitorPowerParams);

static inline NV_STATUS dispcmnCtrlCmdSpecificSetMonitorPower_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_SET_MONITOR_POWER_PARAMS *setMonitorPowerParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificSetMonitorPower__(pDispCommon, setMonitorPowerParams);
}

NV_STATUS dispcmnCtrlCmdSpecificSetHdmiFrlLinkConfig_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_SET_HDMI_FRL_LINK_CONFIG_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdSpecificSetHdmiFrlLinkConfig_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_SET_HDMI_FRL_LINK_CONFIG_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificSetHdmiFrlLinkConfig__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdSpecificApplyEdidOverrideV2_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_APPLY_EDID_OVERRIDE_V2_PARAMS *pEdidOverrideParams);

static inline NV_STATUS dispcmnCtrlCmdSpecificApplyEdidOverrideV2_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_APPLY_EDID_OVERRIDE_V2_PARAMS *pEdidOverrideParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificApplyEdidOverrideV2__(pDispCommon, pEdidOverrideParams);
}

NV_STATUS dispcmnCtrlCmdSpecificGetI2cPortid_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_GET_I2C_PORTID_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdSpecificGetI2cPortid_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_GET_I2C_PORTID_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificGetI2cPortid__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdSpecificGetHdmiGpuCaps_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_GET_HDMI_GPU_CAPS_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdSpecificGetHdmiGpuCaps_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_GET_HDMI_GPU_CAPS_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificGetHdmiGpuCaps__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdSpecificGetHdmiScdcData_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_GET_HDMI_SCDC_DATA_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdSpecificGetHdmiScdcData_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_GET_HDMI_SCDC_DATA_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificGetHdmiScdcData__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdSpecificIsDirectmodeDisplay_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_IS_DIRECTMODE_DISPLAY_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdSpecificIsDirectmodeDisplay_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_IS_DIRECTMODE_DISPLAY_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificIsDirectmodeDisplay__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdSpecificDefaultAdaptivesyncDisplay_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_DEFAULT_ADAPTIVESYNC_DISPLAY_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdSpecificDefaultAdaptivesyncDisplay_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_DEFAULT_ADAPTIVESYNC_DISPLAY_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificDefaultAdaptivesyncDisplay__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdSpecificSetHdmiFrlCapacityComputation_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdSpecificSetHdmiFrlCapacityComputation_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificSetHdmiFrlCapacityComputation__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdSpecificDispI2cReadWrite_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_DISP_I2C_READ_WRITE_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdSpecificDispI2cReadWrite_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_DISP_I2C_READ_WRITE_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificDispI2cReadWrite__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdSpecificGetValidHeadWindowAssignment_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_GET_VALID_HEAD_WINDOW_ASSIGNMENT_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdSpecificGetValidHeadWindowAssignment_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_SPECIFIC_GET_VALID_HEAD_WINDOW_ASSIGNMENT_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificGetValidHeadWindowAssignment__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdSpecificSetHdmiAudioMutestream_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_AUDIO_MUTESTREAM_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdSpecificSetHdmiAudioMutestream_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_AUDIO_MUTESTREAM_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdSpecificSetHdmiAudioMutestream__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdDfpGetInfo_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DFP_GET_INFO_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdDfpGetInfo_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DFP_GET_INFO_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDfpGetInfo__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdDfpGetDisplayportDongleInfo_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DFP_GET_DISPLAYPORT_DONGLE_INFO_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdDfpGetDisplayportDongleInfo_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DFP_GET_DISPLAYPORT_DONGLE_INFO_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDfpGetDisplayportDongleInfo__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdDfpSetEldAudioCaps_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DFP_SET_ELD_AUDIO_CAP_PARAMS *pEldAudioCapsParams);

static inline NV_STATUS dispcmnCtrlCmdDfpSetEldAudioCaps_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DFP_SET_ELD_AUDIO_CAP_PARAMS *pEldAudioCapsParams) {
    return pDispCommon->__dispcmnCtrlCmdDfpSetEldAudioCaps__(pDispCommon, pEldAudioCapsParams);
}

NV_STATUS dispcmnCtrlCmdDfpSetAudioEnable_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DFP_SET_AUDIO_ENABLE_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdDfpSetAudioEnable_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DFP_SET_AUDIO_ENABLE_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDfpSetAudioEnable__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdDfpUpdateDynamicDfpCache_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DFP_UPDATE_DYNAMIC_DFP_CACHE_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdDfpUpdateDynamicDfpCache_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DFP_UPDATE_DYNAMIC_DFP_CACHE_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDfpUpdateDynamicDfpCache__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdDfpAssignSor_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DFP_ASSIGN_SOR_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdDfpAssignSor_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DFP_ASSIGN_SOR_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDfpAssignSor__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdDfpDscCrcControl_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DFP_DSC_CRC_CONTROL_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdDfpDscCrcControl_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DFP_DSC_CRC_CONTROL_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDfpDscCrcControl__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdDfpInitMuxData_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DFP_INIT_MUX_DATA_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdDfpInitMuxData_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DFP_INIT_MUX_DATA_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDfpInitMuxData__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdDfpGetDsiModeTiming_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DFP_GET_DSI_MODE_TIMING_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdDfpGetDsiModeTiming_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DFP_GET_DSI_MODE_TIMING_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDfpGetDsiModeTiming__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdDfpConfigTwoHeadOneOr_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DFP_CONFIG_TWO_HEAD_ONE_OR_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdDfpConfigTwoHeadOneOr_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DFP_CONFIG_TWO_HEAD_ONE_OR_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDfpConfigTwoHeadOneOr__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdDfpGetPadlinkMask_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DFP_GET_PADLINK_MASK_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdDfpGetPadlinkMask_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DFP_GET_PADLINK_MASK_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDfpGetPadlinkMask__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdDfpGetFixedModeTiming_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DFP_GET_FIXED_MODE_TIMING_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdDfpGetFixedModeTiming_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DFP_GET_FIXED_MODE_TIMING_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDfpGetFixedModeTiming__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdDpAuxchCtrl_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DP_AUXCH_CTRL_PARAMS *pAuxchCtrlParams);

static inline NV_STATUS dispcmnCtrlCmdDpAuxchCtrl_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DP_AUXCH_CTRL_PARAMS *pAuxchCtrlParams) {
    return pDispCommon->__dispcmnCtrlCmdDpAuxchCtrl__(pDispCommon, pAuxchCtrlParams);
}

NV_STATUS dispcmnCtrlCmdDpAuxchSetSema_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DP_AUXCH_SET_SEMA_PARAMS *pSemaParams);

static inline NV_STATUS dispcmnCtrlCmdDpAuxchSetSema_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DP_AUXCH_SET_SEMA_PARAMS *pSemaParams) {
    return pDispCommon->__dispcmnCtrlCmdDpAuxchSetSema__(pDispCommon, pSemaParams);
}

NV_STATUS dispcmnCtrlCmdDpCtrl_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DP_CTRL_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdDpCtrl_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DP_CTRL_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpCtrl__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdDpGetLaneData_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DP_LANE_DATA_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdDpGetLaneData_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DP_LANE_DATA_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpGetLaneData__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdDpSetLaneData_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DP_LANE_DATA_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdDpSetLaneData_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DP_LANE_DATA_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpSetLaneData__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdDpSetTestpattern_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DP_SET_TESTPATTERN_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdDpSetTestpattern_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DP_SET_TESTPATTERN_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpSetTestpattern__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdDpMainLinkCtrl_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DP_MAIN_LINK_CTRL_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdDpMainLinkCtrl_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DP_MAIN_LINK_CTRL_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpMainLinkCtrl__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdDpSetAudioMuteStream_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DP_SET_AUDIO_MUTESTREAM_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdDpSetAudioMuteStream_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DP_SET_AUDIO_MUTESTREAM_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpSetAudioMuteStream__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdDpGetLinkConfig_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DP_GET_LINK_CONFIG_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdDpGetLinkConfig_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DP_GET_LINK_CONFIG_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpGetLinkConfig__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdDpGetEDPData_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DP_GET_EDP_DATA_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdDpGetEDPData_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DP_GET_EDP_DATA_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpGetEDPData__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdDpTopologyAllocateDisplayId_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_TOPOLOGY_ALLOCATE_DISPLAYID_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdDpTopologyAllocateDisplayId_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_TOPOLOGY_ALLOCATE_DISPLAYID_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpTopologyAllocateDisplayId__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdDpTopologyFreeDisplayId_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_TOPOLOGY_FREE_DISPLAYID_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdDpTopologyFreeDisplayId_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_TOPOLOGY_FREE_DISPLAYID_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpTopologyFreeDisplayId__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdDpConfigStream_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_CONFIG_STREAM_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdDpConfigStream_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_CONFIG_STREAM_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpConfigStream__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdDpConfigSingleHeadMultiStream_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_CONFIG_SINGLE_HEAD_MULTI_STREAM_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdDpConfigSingleHeadMultiStream_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_CONFIG_SINGLE_HEAD_MULTI_STREAM_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpConfigSingleHeadMultiStream__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdDpSetRateGov_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_SET_RATE_GOV_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdDpSetRateGov_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_SET_RATE_GOV_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpSetRateGov__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdDpSendACT_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_SEND_ACT_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdDpSendACT_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_SEND_ACT_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpSendACT__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdDpSetManualDisplayPort_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_SET_MANUAL_DISPLAYPORT_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdDpSetManualDisplayPort_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_SET_MANUAL_DISPLAYPORT_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpSetManualDisplayPort__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdDpGetCaps_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_GET_CAPS_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdDpGetCaps_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_GET_CAPS_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpGetCaps__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdDpSetMSAPropertiesv2_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_SET_MSA_PROPERTIES_V2_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdDpSetMSAPropertiesv2_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_SET_MSA_PROPERTIES_V2_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpSetMSAPropertiesv2__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdDpSetStereoMSAProperties_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_SET_STEREO_MSA_PROPERTIES_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdDpSetStereoMSAProperties_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_SET_STEREO_MSA_PROPERTIES_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpSetStereoMSAProperties__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdDpGenerateFakeInterrupt_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_GENERATE_FAKE_INTERRUPT_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdDpGenerateFakeInterrupt_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_GENERATE_FAKE_INTERRUPT_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpGenerateFakeInterrupt__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdDpConfigRadScratchReg_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_CONFIG_RAD_SCRATCH_REG_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdDpConfigRadScratchReg_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_CONFIG_RAD_SCRATCH_REG_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpConfigRadScratchReg__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdDpSetTriggerSelect_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_SET_TRIGGER_SELECT_PARAMS *pTriggerSelectParams);

static inline NV_STATUS dispcmnCtrlCmdDpSetTriggerSelect_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_SET_TRIGGER_SELECT_PARAMS *pTriggerSelectParams) {
    return pDispCommon->__dispcmnCtrlCmdDpSetTriggerSelect__(pDispCommon, pTriggerSelectParams);
}

NV_STATUS dispcmnCtrlCmdDpSetTriggerAll_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_SET_TRIGGER_ALL_PARAMS *pTriggerAllParams);

static inline NV_STATUS dispcmnCtrlCmdDpSetTriggerAll_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_SET_TRIGGER_ALL_PARAMS *pTriggerAllParams) {
    return pDispCommon->__dispcmnCtrlCmdDpSetTriggerAll__(pDispCommon, pTriggerAllParams);
}

NV_STATUS dispcmnCtrlCmdDpGetAuxLogData_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_GET_AUXLOGGER_BUFFER_DATA_PARAMS *pDpAuxBufferWrapper);

static inline NV_STATUS dispcmnCtrlCmdDpGetAuxLogData_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_GET_AUXLOGGER_BUFFER_DATA_PARAMS *pDpAuxBufferWrapper) {
    return pDispCommon->__dispcmnCtrlCmdDpGetAuxLogData__(pDispCommon, pDpAuxBufferWrapper);
}

NV_STATUS dispcmnCtrlCmdDpConfigIndexedLinkRates_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_CONFIG_INDEXED_LINK_RATES_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdDpConfigIndexedLinkRates_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_CONFIG_INDEXED_LINK_RATES_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpConfigIndexedLinkRates__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdDpConfigureFec_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_CONFIGURE_FEC_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdDpConfigureFec_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_CONFIGURE_FEC_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpConfigureFec__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdDpGetGenericInfoframe_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DP_GET_GENERIC_INFOFRAME_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdDpGetGenericInfoframe_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DP_GET_GENERIC_INFOFRAME_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpGetGenericInfoframe__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdDpGetMsaAttributes_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DP_GET_MSA_ATTRIBUTES_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdDpGetMsaAttributes_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DP_GET_MSA_ATTRIBUTES_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpGetMsaAttributes__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdDpConfigMacroPad_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_CONFIG_MACRO_PAD_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdDpConfigMacroPad_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_CMD_DP_CONFIG_MACRO_PAD_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpConfigMacroPad__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdDpSetPreemphasisDrivecurrentPostcursor2Data_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DP_SET_PREEMPHASIS_DRIVECURRENT_POSTCURSOR2_DATA_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdDpSetPreemphasisDrivecurrentPostcursor2Data_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DP_SET_PREEMPHASIS_DRIVECURRENT_POSTCURSOR2_DATA_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpSetPreemphasisDrivecurrentPostcursor2Data__(pDispCommon, pParams);
}

NV_STATUS dispcmnCtrlCmdDpGetPreemphasisDrivecurrentPostcursor2Data_IMPL(struct DispCommon *pDispCommon, NV0073_CTRL_DP_GET_PREEMPHASIS_DRIVECURRENT_POSTCURSOR2_DATA_PARAMS *pParams);

static inline NV_STATUS dispcmnCtrlCmdDpGetPreemphasisDrivecurrentPostcursor2Data_DISPATCH(struct DispCommon *pDispCommon, NV0073_CTRL_DP_GET_PREEMPHASIS_DRIVECURRENT_POSTCURSOR2_DATA_PARAMS *pParams) {
    return pDispCommon->__dispcmnCtrlCmdDpGetPreemphasisDrivecurrentPostcursor2Data__(pDispCommon, pParams);
}

static inline NvBool dispcmnShareCallback_DISPATCH(struct DispCommon *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__dispcmnShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS dispcmnCheckMemInterUnmap_DISPATCH(struct DispCommon *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__dispcmnCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS dispcmnMapTo_DISPATCH(struct DispCommon *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__dispcmnMapTo__(pResource, pParams);
}

static inline NvBool dispcmnAccessCallback_DISPATCH(struct DispCommon *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__dispcmnAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline void dispcmnSetNotificationShare_DISPATCH(struct DispCommon *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__dispcmnSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NvU32 dispcmnGetRefCount_DISPATCH(struct DispCommon *pResource) {
    return pResource->__dispcmnGetRefCount__(pResource);
}

static inline void dispcmnAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct DispCommon *pResource, RsResourceRef *pReference) {
    pResource->__dispcmnAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS dispcmnControl_Prologue_DISPATCH(struct DispCommon *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams) {
    return pDisplayApi->__dispcmnControl_Prologue__(pDisplayApi, pCallContext, pRsParams);
}

static inline NV_STATUS dispcmnUnmapFrom_DISPATCH(struct DispCommon *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__dispcmnUnmapFrom__(pResource, pParams);
}

static inline void dispcmnControl_Epilogue_DISPATCH(struct DispCommon *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams) {
    pDisplayApi->__dispcmnControl_Epilogue__(pDisplayApi, pCallContext, pRsParams);
}

static inline NV_STATUS dispcmnControlLookup_DISPATCH(struct DispCommon *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__dispcmnControlLookup__(pResource, pParams, ppEntry);
}

static inline NV_STATUS dispcmnControl_DISPATCH(struct DispCommon *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pDisplayApi->__dispcmnControl__(pDisplayApi, pCallContext, pParams);
}

static inline NV_STATUS dispcmnUnmap_DISPATCH(struct DispCommon *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pResource->__dispcmnUnmap__(pResource, pCallContext, pCpuMapping);
}

static inline NV_STATUS dispcmnGetMemInterMapParams_DISPATCH(struct DispCommon *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__dispcmnGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS dispcmnGetMemoryMappingDescriptor_DISPATCH(struct DispCommon *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__dispcmnGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS dispcmnControlFilter_DISPATCH(struct DispCommon *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__dispcmnControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS dispcmnUnregisterEvent_DISPATCH(struct DispCommon *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__dispcmnUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS dispcmnControlSerialization_Prologue_DISPATCH(struct DispCommon *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__dispcmnControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool dispcmnCanCopy_DISPATCH(struct DispCommon *pResource) {
    return pResource->__dispcmnCanCopy__(pResource);
}

static inline void dispcmnPreDestruct_DISPATCH(struct DispCommon *pResource) {
    pResource->__dispcmnPreDestruct__(pResource);
}

static inline NV_STATUS dispcmnIsDuplicate_DISPATCH(struct DispCommon *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__dispcmnIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void dispcmnControlSerialization_Epilogue_DISPATCH(struct DispCommon *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__dispcmnControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline PEVENTNOTIFICATION *dispcmnGetNotificationListPtr_DISPATCH(struct DispCommon *pNotifier) {
    return pNotifier->__dispcmnGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare *dispcmnGetNotificationShare_DISPATCH(struct DispCommon *pNotifier) {
    return pNotifier->__dispcmnGetNotificationShare__(pNotifier);
}

static inline NV_STATUS dispcmnMap_DISPATCH(struct DispCommon *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pResource->__dispcmnMap__(pResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS dispcmnGetOrAllocNotifShare_DISPATCH(struct DispCommon *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__dispcmnGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

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
