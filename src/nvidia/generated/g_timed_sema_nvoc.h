#ifndef _G_TIMED_SEMA_NVOC_H_
#define _G_TIMED_SEMA_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_timed_sema_nvoc.h"

#ifndef _TIMED_SEMA_H_
#define _TIMED_SEMA_H_

#include "core/core.h"
#include "kernel/gpu/fifo/channel_descendant.h"
#include "ctrl/ctrl9074.h"

typedef struct
{
    NvU64       NotifierGPUVA;
    NvU64       SemaphoreGPUVA;
    NvU64       WaitTimestamp;
    NvU32       ReleaseValue;
    NvU32       NotifyAction;
} GF100_TIMED_SEM_ENTRY, *PGF100_TIMED_SEM_ENTRY;

MAKE_LIST(GF100_TIMED_SEM_ENTRY_LIST, GF100_TIMED_SEM_ENTRY);

/*!
 * RM internal class representing GF100_TIMED_SEMAPHORE_SW
 */
#ifdef NVOC_TIMED_SEMA_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct TimedSemaSwObject {
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
    struct TimedSemaSwObject *__nvoc_pbase_TimedSemaSwObject;
    NV_STATUS (*__tsemaGetSwMethods__)(struct TimedSemaSwObject *, const METHOD **, NvU32 *);
    NV_STATUS (*__tsemaCtrlCmdFlush__)(struct TimedSemaSwObject *, NV9074_CTRL_CMD_FLUSH_PARAMS *);
    NV_STATUS (*__tsemaCtrlCmdGetTime__)(struct TimedSemaSwObject *, NV9074_CTRL_CMD_GET_TIME_PARAMS *);
    NV_STATUS (*__tsemaCtrlCmdRelease__)(struct TimedSemaSwObject *, NV9074_CTRL_CMD_RELEASE_PARAMS *);
    NV_STATUS (*__tsemaCheckMemInterUnmap__)(struct TimedSemaSwObject *, NvBool);
    NvBool (*__tsemaShareCallback__)(struct TimedSemaSwObject *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NvBool (*__tsemaAccessCallback__)(struct TimedSemaSwObject *, struct RsClient *, void *, RsAccessRight);
    NV_STATUS (*__tsemaMapTo__)(struct TimedSemaSwObject *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__tsemaGetMapAddrSpace__)(struct TimedSemaSwObject *, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    void (*__tsemaSetNotificationShare__)(struct TimedSemaSwObject *, struct NotifShare *);
    NvU32 (*__tsemaGetRefCount__)(struct TimedSemaSwObject *);
    void (*__tsemaAddAdditionalDependants__)(struct RsClient *, struct TimedSemaSwObject *, RsResourceRef *);
    NV_STATUS (*__tsemaControl_Prologue__)(struct TimedSemaSwObject *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__tsemaGetRegBaseOffsetAndSize__)(struct TimedSemaSwObject *, struct OBJGPU *, NvU32 *, NvU32 *);
    NV_STATUS (*__tsemaInternalControlForward__)(struct TimedSemaSwObject *, NvU32, void *, NvU32);
    NV_STATUS (*__tsemaUnmapFrom__)(struct TimedSemaSwObject *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__tsemaControl_Epilogue__)(struct TimedSemaSwObject *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__tsemaControlLookup__)(struct TimedSemaSwObject *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NvHandle (*__tsemaGetInternalObjectHandle__)(struct TimedSemaSwObject *);
    NV_STATUS (*__tsemaControl__)(struct TimedSemaSwObject *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__tsemaUnmap__)(struct TimedSemaSwObject *, struct CALL_CONTEXT *, struct RsCpuMapping *);
    NV_STATUS (*__tsemaGetMemInterMapParams__)(struct TimedSemaSwObject *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__tsemaGetMemoryMappingDescriptor__)(struct TimedSemaSwObject *, struct MEMORY_DESCRIPTOR **);
    NvBool (*__tsemaIsSwMethodStalling__)(struct TimedSemaSwObject *, NvU32);
    NV_STATUS (*__tsemaControlFilter__)(struct TimedSemaSwObject *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__tsemaUnregisterEvent__)(struct TimedSemaSwObject *, NvHandle, NvHandle, NvHandle, NvHandle);
    NV_STATUS (*__tsemaControlSerialization_Prologue__)(struct TimedSemaSwObject *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__tsemaCanCopy__)(struct TimedSemaSwObject *);
    void (*__tsemaPreDestruct__)(struct TimedSemaSwObject *);
    NV_STATUS (*__tsemaIsDuplicate__)(struct TimedSemaSwObject *, NvHandle, NvBool *);
    void (*__tsemaControlSerialization_Epilogue__)(struct TimedSemaSwObject *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    PEVENTNOTIFICATION *(*__tsemaGetNotificationListPtr__)(struct TimedSemaSwObject *);
    struct NotifShare *(*__tsemaGetNotificationShare__)(struct TimedSemaSwObject *);
    NV_STATUS (*__tsemaMap__)(struct TimedSemaSwObject *, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);
    NV_STATUS (*__tsemaGetOrAllocNotifShare__)(struct TimedSemaSwObject *, NvHandle, NvHandle, struct NotifShare **);
    NvU32 NotifierHi;
    NvU32 NotifierLo;
    NvU32 SemaphoreHi;
    NvU32 SemaphoreLo;
    NvU32 WaitTimestampHi;
    NvU32 WaitTimestampLo;
    NvU32 ReleaseValue;
    NvU32 Flags;
    NvU64 NotifierGPUVA;
    NvU64 SemaphoreGPUVA;
    NvU64 WaitTimestamp;
    NvU64 FlushLimitTimestamp;
    GF100_TIMED_SEM_ENTRY_LIST entryList;
};

#ifndef __NVOC_CLASS_TimedSemaSwObject_TYPEDEF__
#define __NVOC_CLASS_TimedSemaSwObject_TYPEDEF__
typedef struct TimedSemaSwObject TimedSemaSwObject;
#endif /* __NVOC_CLASS_TimedSemaSwObject_TYPEDEF__ */

#ifndef __nvoc_class_id_TimedSemaSwObject
#define __nvoc_class_id_TimedSemaSwObject 0x335775
#endif /* __nvoc_class_id_TimedSemaSwObject */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_TimedSemaSwObject;

#define __staticCast_TimedSemaSwObject(pThis) \
    ((pThis)->__nvoc_pbase_TimedSemaSwObject)

#ifdef __nvoc_timed_sema_h_disabled
#define __dynamicCast_TimedSemaSwObject(pThis) ((TimedSemaSwObject*)NULL)
#else //__nvoc_timed_sema_h_disabled
#define __dynamicCast_TimedSemaSwObject(pThis) \
    ((TimedSemaSwObject*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(TimedSemaSwObject)))
#endif //__nvoc_timed_sema_h_disabled


NV_STATUS __nvoc_objCreateDynamic_TimedSemaSwObject(TimedSemaSwObject**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_TimedSemaSwObject(TimedSemaSwObject**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_TimedSemaSwObject(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_TimedSemaSwObject((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define tsemaGetSwMethods(pTimedSemSw, ppMethods, pNumMethods) tsemaGetSwMethods_DISPATCH(pTimedSemSw, ppMethods, pNumMethods)
#define tsemaCtrlCmdFlush(pTimedSemaSwObject, pFlushParams) tsemaCtrlCmdFlush_DISPATCH(pTimedSemaSwObject, pFlushParams)
#define tsemaCtrlCmdGetTime(pTimedSemaSwObject, pGetTimeParams) tsemaCtrlCmdGetTime_DISPATCH(pTimedSemaSwObject, pGetTimeParams)
#define tsemaCtrlCmdRelease(pTimedSemaSwObject, pReleaseParams) tsemaCtrlCmdRelease_DISPATCH(pTimedSemaSwObject, pReleaseParams)
#define tsemaCheckMemInterUnmap(pChannelDescendant, bSubdeviceHandleProvided) tsemaCheckMemInterUnmap_DISPATCH(pChannelDescendant, bSubdeviceHandleProvided)
#define tsemaShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) tsemaShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define tsemaAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) tsemaAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define tsemaMapTo(pResource, pParams) tsemaMapTo_DISPATCH(pResource, pParams)
#define tsemaGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) tsemaGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define tsemaSetNotificationShare(pNotifier, pNotifShare) tsemaSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define tsemaGetRefCount(pResource) tsemaGetRefCount_DISPATCH(pResource)
#define tsemaAddAdditionalDependants(pClient, pResource, pReference) tsemaAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define tsemaControl_Prologue(pResource, pCallContext, pParams) tsemaControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define tsemaGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) tsemaGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define tsemaInternalControlForward(pGpuResource, command, pParams, size) tsemaInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define tsemaUnmapFrom(pResource, pParams) tsemaUnmapFrom_DISPATCH(pResource, pParams)
#define tsemaControl_Epilogue(pResource, pCallContext, pParams) tsemaControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define tsemaControlLookup(pResource, pParams, ppEntry) tsemaControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define tsemaGetInternalObjectHandle(pGpuResource) tsemaGetInternalObjectHandle_DISPATCH(pGpuResource)
#define tsemaControl(pGpuResource, pCallContext, pParams) tsemaControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define tsemaUnmap(pGpuResource, pCallContext, pCpuMapping) tsemaUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define tsemaGetMemInterMapParams(pRmResource, pParams) tsemaGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define tsemaGetMemoryMappingDescriptor(pRmResource, ppMemDesc) tsemaGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define tsemaIsSwMethodStalling(pChannelDescendant, hHandle) tsemaIsSwMethodStalling_DISPATCH(pChannelDescendant, hHandle)
#define tsemaControlFilter(pResource, pCallContext, pParams) tsemaControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define tsemaUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) tsemaUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define tsemaControlSerialization_Prologue(pResource, pCallContext, pParams) tsemaControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define tsemaCanCopy(pResource) tsemaCanCopy_DISPATCH(pResource)
#define tsemaPreDestruct(pResource) tsemaPreDestruct_DISPATCH(pResource)
#define tsemaIsDuplicate(pResource, hMemory, pDuplicate) tsemaIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define tsemaControlSerialization_Epilogue(pResource, pCallContext, pParams) tsemaControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define tsemaGetNotificationListPtr(pNotifier) tsemaGetNotificationListPtr_DISPATCH(pNotifier)
#define tsemaGetNotificationShare(pNotifier) tsemaGetNotificationShare_DISPATCH(pNotifier)
#define tsemaMap(pGpuResource, pCallContext, pParams, pCpuMapping) tsemaMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define tsemaGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) tsemaGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)
NV_STATUS tsemaRelease_KERNEL(struct OBJGPU *pGpu, NvU64 semaphoreVA, NvU64 notifierVA, NvU32 hVASpace, NvU32 releasevalue, NvU32 completionStatus, struct Device *pDevice);


#define tsemaRelease(pGpu, semaphoreVA, notifierVA, hVASpace, releasevalue, completionStatus, pDevice) tsemaRelease_KERNEL(pGpu, semaphoreVA, notifierVA, hVASpace, releasevalue, completionStatus, pDevice)
#define tsemaRelease_HAL(pGpu, semaphoreVA, notifierVA, hVASpace, releasevalue, completionStatus, pDevice) tsemaRelease(pGpu, semaphoreVA, notifierVA, hVASpace, releasevalue, completionStatus, pDevice)

NV_STATUS tsemaGetSwMethods_IMPL(struct TimedSemaSwObject *pTimedSemSw, const METHOD **ppMethods, NvU32 *pNumMethods);

static inline NV_STATUS tsemaGetSwMethods_DISPATCH(struct TimedSemaSwObject *pTimedSemSw, const METHOD **ppMethods, NvU32 *pNumMethods) {
    return pTimedSemSw->__tsemaGetSwMethods__(pTimedSemSw, ppMethods, pNumMethods);
}

NV_STATUS tsemaCtrlCmdFlush_IMPL(struct TimedSemaSwObject *pTimedSemaSwObject, NV9074_CTRL_CMD_FLUSH_PARAMS *pFlushParams);

static inline NV_STATUS tsemaCtrlCmdFlush_DISPATCH(struct TimedSemaSwObject *pTimedSemaSwObject, NV9074_CTRL_CMD_FLUSH_PARAMS *pFlushParams) {
    return pTimedSemaSwObject->__tsemaCtrlCmdFlush__(pTimedSemaSwObject, pFlushParams);
}

NV_STATUS tsemaCtrlCmdGetTime_IMPL(struct TimedSemaSwObject *pTimedSemaSwObject, NV9074_CTRL_CMD_GET_TIME_PARAMS *pGetTimeParams);

static inline NV_STATUS tsemaCtrlCmdGetTime_DISPATCH(struct TimedSemaSwObject *pTimedSemaSwObject, NV9074_CTRL_CMD_GET_TIME_PARAMS *pGetTimeParams) {
    return pTimedSemaSwObject->__tsemaCtrlCmdGetTime__(pTimedSemaSwObject, pGetTimeParams);
}

NV_STATUS tsemaCtrlCmdRelease_IMPL(struct TimedSemaSwObject *pTimedSemaSwObject, NV9074_CTRL_CMD_RELEASE_PARAMS *pReleaseParams);

static inline NV_STATUS tsemaCtrlCmdRelease_DISPATCH(struct TimedSemaSwObject *pTimedSemaSwObject, NV9074_CTRL_CMD_RELEASE_PARAMS *pReleaseParams) {
    return pTimedSemaSwObject->__tsemaCtrlCmdRelease__(pTimedSemaSwObject, pReleaseParams);
}

static inline NV_STATUS tsemaCheckMemInterUnmap_DISPATCH(struct TimedSemaSwObject *pChannelDescendant, NvBool bSubdeviceHandleProvided) {
    return pChannelDescendant->__tsemaCheckMemInterUnmap__(pChannelDescendant, bSubdeviceHandleProvided);
}

static inline NvBool tsemaShareCallback_DISPATCH(struct TimedSemaSwObject *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__tsemaShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NvBool tsemaAccessCallback_DISPATCH(struct TimedSemaSwObject *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__tsemaAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS tsemaMapTo_DISPATCH(struct TimedSemaSwObject *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__tsemaMapTo__(pResource, pParams);
}

static inline NV_STATUS tsemaGetMapAddrSpace_DISPATCH(struct TimedSemaSwObject *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__tsemaGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline void tsemaSetNotificationShare_DISPATCH(struct TimedSemaSwObject *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__tsemaSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NvU32 tsemaGetRefCount_DISPATCH(struct TimedSemaSwObject *pResource) {
    return pResource->__tsemaGetRefCount__(pResource);
}

static inline void tsemaAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct TimedSemaSwObject *pResource, RsResourceRef *pReference) {
    pResource->__tsemaAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS tsemaControl_Prologue_DISPATCH(struct TimedSemaSwObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__tsemaControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS tsemaGetRegBaseOffsetAndSize_DISPATCH(struct TimedSemaSwObject *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__tsemaGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS tsemaInternalControlForward_DISPATCH(struct TimedSemaSwObject *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__tsemaInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NV_STATUS tsemaUnmapFrom_DISPATCH(struct TimedSemaSwObject *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__tsemaUnmapFrom__(pResource, pParams);
}

static inline void tsemaControl_Epilogue_DISPATCH(struct TimedSemaSwObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__tsemaControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS tsemaControlLookup_DISPATCH(struct TimedSemaSwObject *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__tsemaControlLookup__(pResource, pParams, ppEntry);
}

static inline NvHandle tsemaGetInternalObjectHandle_DISPATCH(struct TimedSemaSwObject *pGpuResource) {
    return pGpuResource->__tsemaGetInternalObjectHandle__(pGpuResource);
}

static inline NV_STATUS tsemaControl_DISPATCH(struct TimedSemaSwObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__tsemaControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS tsemaUnmap_DISPATCH(struct TimedSemaSwObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__tsemaUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NV_STATUS tsemaGetMemInterMapParams_DISPATCH(struct TimedSemaSwObject *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__tsemaGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS tsemaGetMemoryMappingDescriptor_DISPATCH(struct TimedSemaSwObject *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__tsemaGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NvBool tsemaIsSwMethodStalling_DISPATCH(struct TimedSemaSwObject *pChannelDescendant, NvU32 hHandle) {
    return pChannelDescendant->__tsemaIsSwMethodStalling__(pChannelDescendant, hHandle);
}

static inline NV_STATUS tsemaControlFilter_DISPATCH(struct TimedSemaSwObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__tsemaControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS tsemaUnregisterEvent_DISPATCH(struct TimedSemaSwObject *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__tsemaUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS tsemaControlSerialization_Prologue_DISPATCH(struct TimedSemaSwObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__tsemaControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool tsemaCanCopy_DISPATCH(struct TimedSemaSwObject *pResource) {
    return pResource->__tsemaCanCopy__(pResource);
}

static inline void tsemaPreDestruct_DISPATCH(struct TimedSemaSwObject *pResource) {
    pResource->__tsemaPreDestruct__(pResource);
}

static inline NV_STATUS tsemaIsDuplicate_DISPATCH(struct TimedSemaSwObject *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__tsemaIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void tsemaControlSerialization_Epilogue_DISPATCH(struct TimedSemaSwObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__tsemaControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline PEVENTNOTIFICATION *tsemaGetNotificationListPtr_DISPATCH(struct TimedSemaSwObject *pNotifier) {
    return pNotifier->__tsemaGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare *tsemaGetNotificationShare_DISPATCH(struct TimedSemaSwObject *pNotifier) {
    return pNotifier->__tsemaGetNotificationShare__(pNotifier);
}

static inline NV_STATUS tsemaMap_DISPATCH(struct TimedSemaSwObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__tsemaMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS tsemaGetOrAllocNotifShare_DISPATCH(struct TimedSemaSwObject *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__tsemaGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

NV_STATUS tsemaConstruct_IMPL(struct TimedSemaSwObject *arg_pTimedSemSw, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_tsemaConstruct(arg_pTimedSemSw, arg_pCallContext, arg_pParams) tsemaConstruct_IMPL(arg_pTimedSemSw, arg_pCallContext, arg_pParams)
void tsemaDestruct_IMPL(struct TimedSemaSwObject *pTimedSemSw);

#define __nvoc_tsemaDestruct(pTimedSemSw) tsemaDestruct_IMPL(pTimedSemSw)
#undef PRIVATE_FIELD


// RS-TODO: Delete. Keeping old typedef for transition.
typedef struct TimedSemaSwObject *PGF100_TIMED_SEM_SW_OBJECT;

#endif // _TIMED_SEMA_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_TIMED_SEMA_NVOC_H_
