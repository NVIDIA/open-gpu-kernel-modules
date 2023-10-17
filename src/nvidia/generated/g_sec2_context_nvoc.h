#ifndef _G_SEC2_CONTEXT_NVOC_H_
#define _G_SEC2_CONTEXT_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_sec2_context_nvoc.h"

#ifndef SEC2_CONTEXT_H
#define SEC2_CONTEXT_H

#include "core/bin_data.h"
#include "core/core.h"
#include "kernel/gpu/fifo/channel_descendant.h"

/*!
 * RM internal class representing NVXXXX_TSEC. Class is used for channel work
 * submission.
 */
#ifdef NVOC_SEC2_CONTEXT_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct Sec2Context {
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
    struct Sec2Context *__nvoc_pbase_Sec2Context;
    NV_STATUS (*__sec2ctxCheckMemInterUnmap__)(struct Sec2Context *, NvBool);
    NvBool (*__sec2ctxShareCallback__)(struct Sec2Context *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NvBool (*__sec2ctxAccessCallback__)(struct Sec2Context *, struct RsClient *, void *, RsAccessRight);
    NV_STATUS (*__sec2ctxMapTo__)(struct Sec2Context *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__sec2ctxGetMapAddrSpace__)(struct Sec2Context *, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    void (*__sec2ctxSetNotificationShare__)(struct Sec2Context *, struct NotifShare *);
    NvU32 (*__sec2ctxGetRefCount__)(struct Sec2Context *);
    void (*__sec2ctxAddAdditionalDependants__)(struct RsClient *, struct Sec2Context *, RsResourceRef *);
    NV_STATUS (*__sec2ctxControl_Prologue__)(struct Sec2Context *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__sec2ctxGetRegBaseOffsetAndSize__)(struct Sec2Context *, struct OBJGPU *, NvU32 *, NvU32 *);
    NV_STATUS (*__sec2ctxInternalControlForward__)(struct Sec2Context *, NvU32, void *, NvU32);
    NV_STATUS (*__sec2ctxUnmapFrom__)(struct Sec2Context *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__sec2ctxControl_Epilogue__)(struct Sec2Context *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__sec2ctxControlLookup__)(struct Sec2Context *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NV_STATUS (*__sec2ctxGetSwMethods__)(struct Sec2Context *, const METHOD **, NvU32 *);
    NvHandle (*__sec2ctxGetInternalObjectHandle__)(struct Sec2Context *);
    NV_STATUS (*__sec2ctxControl__)(struct Sec2Context *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__sec2ctxUnmap__)(struct Sec2Context *, struct CALL_CONTEXT *, struct RsCpuMapping *);
    NV_STATUS (*__sec2ctxGetMemInterMapParams__)(struct Sec2Context *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__sec2ctxGetMemoryMappingDescriptor__)(struct Sec2Context *, struct MEMORY_DESCRIPTOR **);
    NvBool (*__sec2ctxIsSwMethodStalling__)(struct Sec2Context *, NvU32);
    NV_STATUS (*__sec2ctxControlFilter__)(struct Sec2Context *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__sec2ctxUnregisterEvent__)(struct Sec2Context *, NvHandle, NvHandle, NvHandle, NvHandle);
    NV_STATUS (*__sec2ctxControlSerialization_Prologue__)(struct Sec2Context *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__sec2ctxCanCopy__)(struct Sec2Context *);
    void (*__sec2ctxPreDestruct__)(struct Sec2Context *);
    NV_STATUS (*__sec2ctxIsDuplicate__)(struct Sec2Context *, NvHandle, NvBool *);
    void (*__sec2ctxControlSerialization_Epilogue__)(struct Sec2Context *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    PEVENTNOTIFICATION *(*__sec2ctxGetNotificationListPtr__)(struct Sec2Context *);
    struct NotifShare *(*__sec2ctxGetNotificationShare__)(struct Sec2Context *);
    NV_STATUS (*__sec2ctxMap__)(struct Sec2Context *, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);
    NV_STATUS (*__sec2ctxGetOrAllocNotifShare__)(struct Sec2Context *, NvHandle, NvHandle, struct NotifShare **);
};

#ifndef __NVOC_CLASS_Sec2Context_TYPEDEF__
#define __NVOC_CLASS_Sec2Context_TYPEDEF__
typedef struct Sec2Context Sec2Context;
#endif /* __NVOC_CLASS_Sec2Context_TYPEDEF__ */

#ifndef __nvoc_class_id_Sec2Context
#define __nvoc_class_id_Sec2Context 0x4c3439
#endif /* __nvoc_class_id_Sec2Context */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Sec2Context;

#define __staticCast_Sec2Context(pThis) \
    ((pThis)->__nvoc_pbase_Sec2Context)

#ifdef __nvoc_sec2_context_h_disabled
#define __dynamicCast_Sec2Context(pThis) ((Sec2Context*)NULL)
#else //__nvoc_sec2_context_h_disabled
#define __dynamicCast_Sec2Context(pThis) \
    ((Sec2Context*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(Sec2Context)))
#endif //__nvoc_sec2_context_h_disabled


NV_STATUS __nvoc_objCreateDynamic_Sec2Context(Sec2Context**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_Sec2Context(Sec2Context**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_Sec2Context(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_Sec2Context((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define sec2ctxCheckMemInterUnmap(pChannelDescendant, bSubdeviceHandleProvided) sec2ctxCheckMemInterUnmap_DISPATCH(pChannelDescendant, bSubdeviceHandleProvided)
#define sec2ctxShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) sec2ctxShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define sec2ctxAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) sec2ctxAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define sec2ctxMapTo(pResource, pParams) sec2ctxMapTo_DISPATCH(pResource, pParams)
#define sec2ctxGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) sec2ctxGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define sec2ctxSetNotificationShare(pNotifier, pNotifShare) sec2ctxSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define sec2ctxGetRefCount(pResource) sec2ctxGetRefCount_DISPATCH(pResource)
#define sec2ctxAddAdditionalDependants(pClient, pResource, pReference) sec2ctxAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define sec2ctxControl_Prologue(pResource, pCallContext, pParams) sec2ctxControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define sec2ctxGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) sec2ctxGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define sec2ctxInternalControlForward(pGpuResource, command, pParams, size) sec2ctxInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define sec2ctxUnmapFrom(pResource, pParams) sec2ctxUnmapFrom_DISPATCH(pResource, pParams)
#define sec2ctxControl_Epilogue(pResource, pCallContext, pParams) sec2ctxControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define sec2ctxControlLookup(pResource, pParams, ppEntry) sec2ctxControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define sec2ctxGetSwMethods(pChannelDescendant, ppMethods, pNumMethods) sec2ctxGetSwMethods_DISPATCH(pChannelDescendant, ppMethods, pNumMethods)
#define sec2ctxGetInternalObjectHandle(pGpuResource) sec2ctxGetInternalObjectHandle_DISPATCH(pGpuResource)
#define sec2ctxControl(pGpuResource, pCallContext, pParams) sec2ctxControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define sec2ctxUnmap(pGpuResource, pCallContext, pCpuMapping) sec2ctxUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define sec2ctxGetMemInterMapParams(pRmResource, pParams) sec2ctxGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define sec2ctxGetMemoryMappingDescriptor(pRmResource, ppMemDesc) sec2ctxGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define sec2ctxIsSwMethodStalling(pChannelDescendant, hHandle) sec2ctxIsSwMethodStalling_DISPATCH(pChannelDescendant, hHandle)
#define sec2ctxControlFilter(pResource, pCallContext, pParams) sec2ctxControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define sec2ctxUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) sec2ctxUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define sec2ctxControlSerialization_Prologue(pResource, pCallContext, pParams) sec2ctxControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define sec2ctxCanCopy(pResource) sec2ctxCanCopy_DISPATCH(pResource)
#define sec2ctxPreDestruct(pResource) sec2ctxPreDestruct_DISPATCH(pResource)
#define sec2ctxIsDuplicate(pResource, hMemory, pDuplicate) sec2ctxIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define sec2ctxControlSerialization_Epilogue(pResource, pCallContext, pParams) sec2ctxControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define sec2ctxGetNotificationListPtr(pNotifier) sec2ctxGetNotificationListPtr_DISPATCH(pNotifier)
#define sec2ctxGetNotificationShare(pNotifier) sec2ctxGetNotificationShare_DISPATCH(pNotifier)
#define sec2ctxMap(pGpuResource, pCallContext, pParams, pCpuMapping) sec2ctxMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define sec2ctxGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) sec2ctxGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)
NV_STATUS sec2ctxConstructHal_KERNEL(struct Sec2Context *pSec2Context, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);


#ifdef __nvoc_sec2_context_h_disabled
static inline NV_STATUS sec2ctxConstructHal(struct Sec2Context *pSec2Context, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Sec2Context was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_sec2_context_h_disabled
#define sec2ctxConstructHal(pSec2Context, pCallContext, pParams) sec2ctxConstructHal_KERNEL(pSec2Context, pCallContext, pParams)
#endif //__nvoc_sec2_context_h_disabled

#define sec2ctxConstructHal_HAL(pSec2Context, pCallContext, pParams) sec2ctxConstructHal(pSec2Context, pCallContext, pParams)

void sec2ctxDestructHal_KERNEL(struct Sec2Context *pSec2Context);


#ifdef __nvoc_sec2_context_h_disabled
static inline void sec2ctxDestructHal(struct Sec2Context *pSec2Context) {
    NV_ASSERT_FAILED_PRECOMP("Sec2Context was disabled!");
}
#else //__nvoc_sec2_context_h_disabled
#define sec2ctxDestructHal(pSec2Context) sec2ctxDestructHal_KERNEL(pSec2Context)
#endif //__nvoc_sec2_context_h_disabled

#define sec2ctxDestructHal_HAL(pSec2Context) sec2ctxDestructHal(pSec2Context)

static inline NV_STATUS sec2ctxCheckMemInterUnmap_DISPATCH(struct Sec2Context *pChannelDescendant, NvBool bSubdeviceHandleProvided) {
    return pChannelDescendant->__sec2ctxCheckMemInterUnmap__(pChannelDescendant, bSubdeviceHandleProvided);
}

static inline NvBool sec2ctxShareCallback_DISPATCH(struct Sec2Context *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__sec2ctxShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NvBool sec2ctxAccessCallback_DISPATCH(struct Sec2Context *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__sec2ctxAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS sec2ctxMapTo_DISPATCH(struct Sec2Context *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__sec2ctxMapTo__(pResource, pParams);
}

static inline NV_STATUS sec2ctxGetMapAddrSpace_DISPATCH(struct Sec2Context *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__sec2ctxGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline void sec2ctxSetNotificationShare_DISPATCH(struct Sec2Context *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__sec2ctxSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NvU32 sec2ctxGetRefCount_DISPATCH(struct Sec2Context *pResource) {
    return pResource->__sec2ctxGetRefCount__(pResource);
}

static inline void sec2ctxAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct Sec2Context *pResource, RsResourceRef *pReference) {
    pResource->__sec2ctxAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS sec2ctxControl_Prologue_DISPATCH(struct Sec2Context *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__sec2ctxControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS sec2ctxGetRegBaseOffsetAndSize_DISPATCH(struct Sec2Context *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__sec2ctxGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS sec2ctxInternalControlForward_DISPATCH(struct Sec2Context *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__sec2ctxInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NV_STATUS sec2ctxUnmapFrom_DISPATCH(struct Sec2Context *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__sec2ctxUnmapFrom__(pResource, pParams);
}

static inline void sec2ctxControl_Epilogue_DISPATCH(struct Sec2Context *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__sec2ctxControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS sec2ctxControlLookup_DISPATCH(struct Sec2Context *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__sec2ctxControlLookup__(pResource, pParams, ppEntry);
}

static inline NV_STATUS sec2ctxGetSwMethods_DISPATCH(struct Sec2Context *pChannelDescendant, const METHOD **ppMethods, NvU32 *pNumMethods) {
    return pChannelDescendant->__sec2ctxGetSwMethods__(pChannelDescendant, ppMethods, pNumMethods);
}

static inline NvHandle sec2ctxGetInternalObjectHandle_DISPATCH(struct Sec2Context *pGpuResource) {
    return pGpuResource->__sec2ctxGetInternalObjectHandle__(pGpuResource);
}

static inline NV_STATUS sec2ctxControl_DISPATCH(struct Sec2Context *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__sec2ctxControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS sec2ctxUnmap_DISPATCH(struct Sec2Context *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__sec2ctxUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NV_STATUS sec2ctxGetMemInterMapParams_DISPATCH(struct Sec2Context *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__sec2ctxGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS sec2ctxGetMemoryMappingDescriptor_DISPATCH(struct Sec2Context *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__sec2ctxGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NvBool sec2ctxIsSwMethodStalling_DISPATCH(struct Sec2Context *pChannelDescendant, NvU32 hHandle) {
    return pChannelDescendant->__sec2ctxIsSwMethodStalling__(pChannelDescendant, hHandle);
}

static inline NV_STATUS sec2ctxControlFilter_DISPATCH(struct Sec2Context *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__sec2ctxControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS sec2ctxUnregisterEvent_DISPATCH(struct Sec2Context *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__sec2ctxUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS sec2ctxControlSerialization_Prologue_DISPATCH(struct Sec2Context *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__sec2ctxControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool sec2ctxCanCopy_DISPATCH(struct Sec2Context *pResource) {
    return pResource->__sec2ctxCanCopy__(pResource);
}

static inline void sec2ctxPreDestruct_DISPATCH(struct Sec2Context *pResource) {
    pResource->__sec2ctxPreDestruct__(pResource);
}

static inline NV_STATUS sec2ctxIsDuplicate_DISPATCH(struct Sec2Context *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__sec2ctxIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void sec2ctxControlSerialization_Epilogue_DISPATCH(struct Sec2Context *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__sec2ctxControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline PEVENTNOTIFICATION *sec2ctxGetNotificationListPtr_DISPATCH(struct Sec2Context *pNotifier) {
    return pNotifier->__sec2ctxGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare *sec2ctxGetNotificationShare_DISPATCH(struct Sec2Context *pNotifier) {
    return pNotifier->__sec2ctxGetNotificationShare__(pNotifier);
}

static inline NV_STATUS sec2ctxMap_DISPATCH(struct Sec2Context *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__sec2ctxMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS sec2ctxGetOrAllocNotifShare_DISPATCH(struct Sec2Context *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__sec2ctxGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

static inline NV_STATUS __nvoc_sec2ctxConstruct(struct Sec2Context *arg_pSec2Context, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams) {
    return sec2ctxConstructHal(arg_pSec2Context, arg_pCallContext, arg_pParams);
}

static inline void __nvoc_sec2ctxDestruct(struct Sec2Context *pSec2Context) {
    sec2ctxDestructHal(pSec2Context);
}

#undef PRIVATE_FIELD


#endif  // SEC2_CONTEXT_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_SEC2_CONTEXT_NVOC_H_
