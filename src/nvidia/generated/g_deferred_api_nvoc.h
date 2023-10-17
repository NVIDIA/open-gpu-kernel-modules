#ifndef _G_DEFERRED_API_NVOC_H_
#define _G_DEFERRED_API_NVOC_H_
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
#include "g_deferred_api_nvoc.h"

#ifndef DEFERRED_API_H
#define DEFERRED_API_H 1

#include "core/core.h"
#include "kernel/gpu/fifo/channel_descendant.h"
#include "rmapi/control.h"
#include "ctrl/ctrl5080.h"
#include "nvsecurityinfo.h"

#define DEFERRED_API_INFO_FLAGS_HAS_EXECUTED            0x00000001
#define DEFERRED_API_INFO_FLAGS_HAS_TLB_FLUSHED         0x00000002
#define DEFERRED_API_INFO_FLAGS_HAS_PRIVATE_DATA_ALLOC  0x00000004

typedef struct _def_deferred_api_info
{
    NODE                Node;
    NvHandle            Handle;
    NvU32               Flags;                  // see DEFERRED_API_INFO_FLAGS_* defines
    RS_PRIV_LEVEL       privLevel;              // privilege level of the client that initiated deferred call.
    void *              pDeferredApiInfo;
    NvP64               pDeferredPrivateData;
} DEFERRED_API_INFO;


/*!
 * RM internal class representing NV50_DEFERRED_API_CLASS
 */
#ifdef NVOC_DEFERRED_API_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct DeferredApiObject {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct ChannelDescendant __nvoc_base_ChannelDescendant;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct GpuResource *__nvoc_pbase_GpuResource;
    struct INotifier *__nvoc_pbase_INotifier;
    struct Notifier *__nvoc_pbase_Notifier;
    struct ChannelDescendant *__nvoc_pbase_ChannelDescendant;
    struct DeferredApiObject *__nvoc_pbase_DeferredApiObject;
    NV_STATUS (*__defapiGetSwMethods__)(struct DeferredApiObject *, const METHOD **, NvU32 *);
    NvBool (*__defapiIsSwMethodStalling__)(struct DeferredApiObject *, NvU32);
    NV_STATUS (*__defapiCtrlCmdDeferredApi__)(struct DeferredApiObject *, NV5080_CTRL_DEFERRED_API_PARAMS *);
    NV_STATUS (*__defapiCtrlCmdDeferredApiV2__)(struct DeferredApiObject *, NV5080_CTRL_DEFERRED_API_V2_PARAMS *);
    NV_STATUS (*__defapiCtrlCmdRemoveApi__)(struct DeferredApiObject *, NV5080_CTRL_REMOVE_API_PARAMS *);
    NV_STATUS (*__defapiCheckMemInterUnmap__)(struct DeferredApiObject *, NvBool);
    NvBool (*__defapiShareCallback__)(struct DeferredApiObject *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NvBool (*__defapiAccessCallback__)(struct DeferredApiObject *, struct RsClient *, void *, RsAccessRight);
    NV_STATUS (*__defapiMapTo__)(struct DeferredApiObject *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__defapiGetMapAddrSpace__)(struct DeferredApiObject *, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    void (*__defapiSetNotificationShare__)(struct DeferredApiObject *, struct NotifShare *);
    NvU32 (*__defapiGetRefCount__)(struct DeferredApiObject *);
    void (*__defapiAddAdditionalDependants__)(struct RsClient *, struct DeferredApiObject *, RsResourceRef *);
    NV_STATUS (*__defapiControl_Prologue__)(struct DeferredApiObject *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__defapiGetRegBaseOffsetAndSize__)(struct DeferredApiObject *, struct OBJGPU *, NvU32 *, NvU32 *);
    NV_STATUS (*__defapiInternalControlForward__)(struct DeferredApiObject *, NvU32, void *, NvU32);
    NV_STATUS (*__defapiUnmapFrom__)(struct DeferredApiObject *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__defapiControl_Epilogue__)(struct DeferredApiObject *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__defapiControlLookup__)(struct DeferredApiObject *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NvHandle (*__defapiGetInternalObjectHandle__)(struct DeferredApiObject *);
    NV_STATUS (*__defapiControl__)(struct DeferredApiObject *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__defapiUnmap__)(struct DeferredApiObject *, struct CALL_CONTEXT *, struct RsCpuMapping *);
    NV_STATUS (*__defapiGetMemInterMapParams__)(struct DeferredApiObject *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__defapiGetMemoryMappingDescriptor__)(struct DeferredApiObject *, struct MEMORY_DESCRIPTOR **);
    NV_STATUS (*__defapiControlFilter__)(struct DeferredApiObject *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__defapiUnregisterEvent__)(struct DeferredApiObject *, NvHandle, NvHandle, NvHandle, NvHandle);
    NV_STATUS (*__defapiControlSerialization_Prologue__)(struct DeferredApiObject *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__defapiCanCopy__)(struct DeferredApiObject *);
    void (*__defapiPreDestruct__)(struct DeferredApiObject *);
    NV_STATUS (*__defapiIsDuplicate__)(struct DeferredApiObject *, NvHandle, NvBool *);
    void (*__defapiControlSerialization_Epilogue__)(struct DeferredApiObject *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    PEVENTNOTIFICATION *(*__defapiGetNotificationListPtr__)(struct DeferredApiObject *);
    struct NotifShare *(*__defapiGetNotificationShare__)(struct DeferredApiObject *);
    NV_STATUS (*__defapiMap__)(struct DeferredApiObject *, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);
    NV_STATUS (*__defapiGetOrAllocNotifShare__)(struct DeferredApiObject *, NvHandle, NvHandle, struct NotifShare **);
    PNODE DeferredApiList;
    NvU32 NumWaitingOnTLBFlush;
};

#ifndef __NVOC_CLASS_DeferredApiObject_TYPEDEF__
#define __NVOC_CLASS_DeferredApiObject_TYPEDEF__
typedef struct DeferredApiObject DeferredApiObject;
#endif /* __NVOC_CLASS_DeferredApiObject_TYPEDEF__ */

#ifndef __nvoc_class_id_DeferredApiObject
#define __nvoc_class_id_DeferredApiObject 0x8ea933
#endif /* __nvoc_class_id_DeferredApiObject */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_DeferredApiObject;

#define __staticCast_DeferredApiObject(pThis) \
    ((pThis)->__nvoc_pbase_DeferredApiObject)

#ifdef __nvoc_deferred_api_h_disabled
#define __dynamicCast_DeferredApiObject(pThis) ((DeferredApiObject*)NULL)
#else //__nvoc_deferred_api_h_disabled
#define __dynamicCast_DeferredApiObject(pThis) \
    ((DeferredApiObject*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(DeferredApiObject)))
#endif //__nvoc_deferred_api_h_disabled


NV_STATUS __nvoc_objCreateDynamic_DeferredApiObject(DeferredApiObject**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_DeferredApiObject(DeferredApiObject**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_DeferredApiObject(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_DeferredApiObject((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define defapiGetSwMethods(pDeferredApi, ppMethods, pNumMethods) defapiGetSwMethods_DISPATCH(pDeferredApi, ppMethods, pNumMethods)
#define defapiIsSwMethodStalling(pDeferredApi, hDeferredApi) defapiIsSwMethodStalling_DISPATCH(pDeferredApi, hDeferredApi)
#define defapiCtrlCmdDeferredApi(pDeferredApiObj, pDeferredApi) defapiCtrlCmdDeferredApi_DISPATCH(pDeferredApiObj, pDeferredApi)
#define defapiCtrlCmdDeferredApiV2(pDeferredApiObj, pDeferredApi) defapiCtrlCmdDeferredApiV2_DISPATCH(pDeferredApiObj, pDeferredApi)
#define defapiCtrlCmdRemoveApi(pDeferredApiObj, pRemoveApi) defapiCtrlCmdRemoveApi_DISPATCH(pDeferredApiObj, pRemoveApi)
#define defapiCheckMemInterUnmap(pChannelDescendant, bSubdeviceHandleProvided) defapiCheckMemInterUnmap_DISPATCH(pChannelDescendant, bSubdeviceHandleProvided)
#define defapiShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) defapiShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define defapiAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) defapiAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define defapiMapTo(pResource, pParams) defapiMapTo_DISPATCH(pResource, pParams)
#define defapiGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) defapiGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define defapiSetNotificationShare(pNotifier, pNotifShare) defapiSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define defapiGetRefCount(pResource) defapiGetRefCount_DISPATCH(pResource)
#define defapiAddAdditionalDependants(pClient, pResource, pReference) defapiAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define defapiControl_Prologue(pResource, pCallContext, pParams) defapiControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define defapiGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) defapiGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define defapiInternalControlForward(pGpuResource, command, pParams, size) defapiInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define defapiUnmapFrom(pResource, pParams) defapiUnmapFrom_DISPATCH(pResource, pParams)
#define defapiControl_Epilogue(pResource, pCallContext, pParams) defapiControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define defapiControlLookup(pResource, pParams, ppEntry) defapiControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define defapiGetInternalObjectHandle(pGpuResource) defapiGetInternalObjectHandle_DISPATCH(pGpuResource)
#define defapiControl(pGpuResource, pCallContext, pParams) defapiControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define defapiUnmap(pGpuResource, pCallContext, pCpuMapping) defapiUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define defapiGetMemInterMapParams(pRmResource, pParams) defapiGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define defapiGetMemoryMappingDescriptor(pRmResource, ppMemDesc) defapiGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define defapiControlFilter(pResource, pCallContext, pParams) defapiControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define defapiUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) defapiUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define defapiControlSerialization_Prologue(pResource, pCallContext, pParams) defapiControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define defapiCanCopy(pResource) defapiCanCopy_DISPATCH(pResource)
#define defapiPreDestruct(pResource) defapiPreDestruct_DISPATCH(pResource)
#define defapiIsDuplicate(pResource, hMemory, pDuplicate) defapiIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define defapiControlSerialization_Epilogue(pResource, pCallContext, pParams) defapiControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define defapiGetNotificationListPtr(pNotifier) defapiGetNotificationListPtr_DISPATCH(pNotifier)
#define defapiGetNotificationShare(pNotifier) defapiGetNotificationShare_DISPATCH(pNotifier)
#define defapiMap(pGpuResource, pCallContext, pParams, pCpuMapping) defapiMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define defapiGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) defapiGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)
NV_STATUS defapiGetSwMethods_IMPL(struct DeferredApiObject *pDeferredApi, const METHOD **ppMethods, NvU32 *pNumMethods);

static inline NV_STATUS defapiGetSwMethods_DISPATCH(struct DeferredApiObject *pDeferredApi, const METHOD **ppMethods, NvU32 *pNumMethods) {
    return pDeferredApi->__defapiGetSwMethods__(pDeferredApi, ppMethods, pNumMethods);
}

NvBool defapiIsSwMethodStalling_IMPL(struct DeferredApiObject *pDeferredApi, NvU32 hDeferredApi);

static inline NvBool defapiIsSwMethodStalling_DISPATCH(struct DeferredApiObject *pDeferredApi, NvU32 hDeferredApi) {
    return pDeferredApi->__defapiIsSwMethodStalling__(pDeferredApi, hDeferredApi);
}

NV_STATUS defapiCtrlCmdDeferredApi_IMPL(struct DeferredApiObject *pDeferredApiObj, NV5080_CTRL_DEFERRED_API_PARAMS *pDeferredApi);

static inline NV_STATUS defapiCtrlCmdDeferredApi_DISPATCH(struct DeferredApiObject *pDeferredApiObj, NV5080_CTRL_DEFERRED_API_PARAMS *pDeferredApi) {
    return pDeferredApiObj->__defapiCtrlCmdDeferredApi__(pDeferredApiObj, pDeferredApi);
}

NV_STATUS defapiCtrlCmdDeferredApiV2_IMPL(struct DeferredApiObject *pDeferredApiObj, NV5080_CTRL_DEFERRED_API_V2_PARAMS *pDeferredApi);

static inline NV_STATUS defapiCtrlCmdDeferredApiV2_DISPATCH(struct DeferredApiObject *pDeferredApiObj, NV5080_CTRL_DEFERRED_API_V2_PARAMS *pDeferredApi) {
    return pDeferredApiObj->__defapiCtrlCmdDeferredApiV2__(pDeferredApiObj, pDeferredApi);
}

NV_STATUS defapiCtrlCmdRemoveApi_IMPL(struct DeferredApiObject *pDeferredApiObj, NV5080_CTRL_REMOVE_API_PARAMS *pRemoveApi);

static inline NV_STATUS defapiCtrlCmdRemoveApi_DISPATCH(struct DeferredApiObject *pDeferredApiObj, NV5080_CTRL_REMOVE_API_PARAMS *pRemoveApi) {
    return pDeferredApiObj->__defapiCtrlCmdRemoveApi__(pDeferredApiObj, pRemoveApi);
}

static inline NV_STATUS defapiCheckMemInterUnmap_DISPATCH(struct DeferredApiObject *pChannelDescendant, NvBool bSubdeviceHandleProvided) {
    return pChannelDescendant->__defapiCheckMemInterUnmap__(pChannelDescendant, bSubdeviceHandleProvided);
}

static inline NvBool defapiShareCallback_DISPATCH(struct DeferredApiObject *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__defapiShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NvBool defapiAccessCallback_DISPATCH(struct DeferredApiObject *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__defapiAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS defapiMapTo_DISPATCH(struct DeferredApiObject *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__defapiMapTo__(pResource, pParams);
}

static inline NV_STATUS defapiGetMapAddrSpace_DISPATCH(struct DeferredApiObject *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__defapiGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline void defapiSetNotificationShare_DISPATCH(struct DeferredApiObject *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__defapiSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NvU32 defapiGetRefCount_DISPATCH(struct DeferredApiObject *pResource) {
    return pResource->__defapiGetRefCount__(pResource);
}

static inline void defapiAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct DeferredApiObject *pResource, RsResourceRef *pReference) {
    pResource->__defapiAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS defapiControl_Prologue_DISPATCH(struct DeferredApiObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__defapiControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS defapiGetRegBaseOffsetAndSize_DISPATCH(struct DeferredApiObject *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__defapiGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS defapiInternalControlForward_DISPATCH(struct DeferredApiObject *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__defapiInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NV_STATUS defapiUnmapFrom_DISPATCH(struct DeferredApiObject *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__defapiUnmapFrom__(pResource, pParams);
}

static inline void defapiControl_Epilogue_DISPATCH(struct DeferredApiObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__defapiControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS defapiControlLookup_DISPATCH(struct DeferredApiObject *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__defapiControlLookup__(pResource, pParams, ppEntry);
}

static inline NvHandle defapiGetInternalObjectHandle_DISPATCH(struct DeferredApiObject *pGpuResource) {
    return pGpuResource->__defapiGetInternalObjectHandle__(pGpuResource);
}

static inline NV_STATUS defapiControl_DISPATCH(struct DeferredApiObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__defapiControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS defapiUnmap_DISPATCH(struct DeferredApiObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__defapiUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NV_STATUS defapiGetMemInterMapParams_DISPATCH(struct DeferredApiObject *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__defapiGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS defapiGetMemoryMappingDescriptor_DISPATCH(struct DeferredApiObject *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__defapiGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS defapiControlFilter_DISPATCH(struct DeferredApiObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__defapiControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS defapiUnregisterEvent_DISPATCH(struct DeferredApiObject *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__defapiUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS defapiControlSerialization_Prologue_DISPATCH(struct DeferredApiObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__defapiControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool defapiCanCopy_DISPATCH(struct DeferredApiObject *pResource) {
    return pResource->__defapiCanCopy__(pResource);
}

static inline void defapiPreDestruct_DISPATCH(struct DeferredApiObject *pResource) {
    pResource->__defapiPreDestruct__(pResource);
}

static inline NV_STATUS defapiIsDuplicate_DISPATCH(struct DeferredApiObject *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__defapiIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void defapiControlSerialization_Epilogue_DISPATCH(struct DeferredApiObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__defapiControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline PEVENTNOTIFICATION *defapiGetNotificationListPtr_DISPATCH(struct DeferredApiObject *pNotifier) {
    return pNotifier->__defapiGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare *defapiGetNotificationShare_DISPATCH(struct DeferredApiObject *pNotifier) {
    return pNotifier->__defapiGetNotificationShare__(pNotifier);
}

static inline NV_STATUS defapiMap_DISPATCH(struct DeferredApiObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__defapiMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS defapiGetOrAllocNotifShare_DISPATCH(struct DeferredApiObject *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__defapiGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

NV_STATUS defapiConstruct_IMPL(struct DeferredApiObject *arg_pDeferredApi, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_defapiConstruct(arg_pDeferredApi, arg_pCallContext, arg_pParams) defapiConstruct_IMPL(arg_pDeferredApi, arg_pCallContext, arg_pParams)
void defapiDestruct_IMPL(struct DeferredApiObject *pDeferredApi);

#define __nvoc_defapiDestruct(pDeferredApi) defapiDestruct_IMPL(pDeferredApi)
#undef PRIVATE_FIELD


#endif // DEFERRED_API_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_DEFERRED_API_NVOC_H_
