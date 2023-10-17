#ifndef _G_KERNEL_NVJPG_CTX_NVOC_H_
#define _G_KERNEL_NVJPG_CTX_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_kernel_nvjpg_ctx_nvoc.h"

#ifndef KERNEL_NVJPG_CTX_H
#define KERNEL_NVJPG_CTX_H

#include "kernel/gpu/fifo/channel_descendant.h"

ENGDESCRIPTOR nvjpgGetEngineDescFromAllocParams(OBJGPU *pGpu, NvU32 externalClassId, void *pAllocParams);

/*!
 * RM internal class representing NVXXXX_VIDEO_NVJPG
 */
#ifdef NVOC_KERNEL_NVJPG_CTX_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct NvjpgContext {
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
    struct NvjpgContext *__nvoc_pbase_NvjpgContext;
    NV_STATUS (*__nvjpgctxCheckMemInterUnmap__)(struct NvjpgContext *, NvBool);
    NvBool (*__nvjpgctxShareCallback__)(struct NvjpgContext *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NvBool (*__nvjpgctxAccessCallback__)(struct NvjpgContext *, struct RsClient *, void *, RsAccessRight);
    NV_STATUS (*__nvjpgctxMapTo__)(struct NvjpgContext *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__nvjpgctxGetMapAddrSpace__)(struct NvjpgContext *, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    void (*__nvjpgctxSetNotificationShare__)(struct NvjpgContext *, struct NotifShare *);
    NvU32 (*__nvjpgctxGetRefCount__)(struct NvjpgContext *);
    void (*__nvjpgctxAddAdditionalDependants__)(struct RsClient *, struct NvjpgContext *, RsResourceRef *);
    NV_STATUS (*__nvjpgctxControl_Prologue__)(struct NvjpgContext *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__nvjpgctxGetRegBaseOffsetAndSize__)(struct NvjpgContext *, struct OBJGPU *, NvU32 *, NvU32 *);
    NV_STATUS (*__nvjpgctxInternalControlForward__)(struct NvjpgContext *, NvU32, void *, NvU32);
    NV_STATUS (*__nvjpgctxUnmapFrom__)(struct NvjpgContext *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__nvjpgctxControl_Epilogue__)(struct NvjpgContext *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__nvjpgctxControlLookup__)(struct NvjpgContext *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NV_STATUS (*__nvjpgctxGetSwMethods__)(struct NvjpgContext *, const METHOD **, NvU32 *);
    NvHandle (*__nvjpgctxGetInternalObjectHandle__)(struct NvjpgContext *);
    NV_STATUS (*__nvjpgctxControl__)(struct NvjpgContext *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__nvjpgctxUnmap__)(struct NvjpgContext *, struct CALL_CONTEXT *, struct RsCpuMapping *);
    NV_STATUS (*__nvjpgctxGetMemInterMapParams__)(struct NvjpgContext *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__nvjpgctxGetMemoryMappingDescriptor__)(struct NvjpgContext *, struct MEMORY_DESCRIPTOR **);
    NvBool (*__nvjpgctxIsSwMethodStalling__)(struct NvjpgContext *, NvU32);
    NV_STATUS (*__nvjpgctxControlFilter__)(struct NvjpgContext *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__nvjpgctxUnregisterEvent__)(struct NvjpgContext *, NvHandle, NvHandle, NvHandle, NvHandle);
    NV_STATUS (*__nvjpgctxControlSerialization_Prologue__)(struct NvjpgContext *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__nvjpgctxCanCopy__)(struct NvjpgContext *);
    void (*__nvjpgctxPreDestruct__)(struct NvjpgContext *);
    NV_STATUS (*__nvjpgctxIsDuplicate__)(struct NvjpgContext *, NvHandle, NvBool *);
    void (*__nvjpgctxControlSerialization_Epilogue__)(struct NvjpgContext *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    PEVENTNOTIFICATION *(*__nvjpgctxGetNotificationListPtr__)(struct NvjpgContext *);
    struct NotifShare *(*__nvjpgctxGetNotificationShare__)(struct NvjpgContext *);
    NV_STATUS (*__nvjpgctxMap__)(struct NvjpgContext *, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);
    NV_STATUS (*__nvjpgctxGetOrAllocNotifShare__)(struct NvjpgContext *, NvHandle, NvHandle, struct NotifShare **);
};

#ifndef __NVOC_CLASS_NvjpgContext_TYPEDEF__
#define __NVOC_CLASS_NvjpgContext_TYPEDEF__
typedef struct NvjpgContext NvjpgContext;
#endif /* __NVOC_CLASS_NvjpgContext_TYPEDEF__ */

#ifndef __nvoc_class_id_NvjpgContext
#define __nvoc_class_id_NvjpgContext 0x08c1ce
#endif /* __nvoc_class_id_NvjpgContext */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvjpgContext;

#define __staticCast_NvjpgContext(pThis) \
    ((pThis)->__nvoc_pbase_NvjpgContext)

#ifdef __nvoc_kernel_nvjpg_ctx_h_disabled
#define __dynamicCast_NvjpgContext(pThis) ((NvjpgContext*)NULL)
#else //__nvoc_kernel_nvjpg_ctx_h_disabled
#define __dynamicCast_NvjpgContext(pThis) \
    ((NvjpgContext*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(NvjpgContext)))
#endif //__nvoc_kernel_nvjpg_ctx_h_disabled


NV_STATUS __nvoc_objCreateDynamic_NvjpgContext(NvjpgContext**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_NvjpgContext(NvjpgContext**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_NvjpgContext(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_NvjpgContext((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define nvjpgctxCheckMemInterUnmap(pChannelDescendant, bSubdeviceHandleProvided) nvjpgctxCheckMemInterUnmap_DISPATCH(pChannelDescendant, bSubdeviceHandleProvided)
#define nvjpgctxShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) nvjpgctxShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define nvjpgctxAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) nvjpgctxAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define nvjpgctxMapTo(pResource, pParams) nvjpgctxMapTo_DISPATCH(pResource, pParams)
#define nvjpgctxGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) nvjpgctxGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define nvjpgctxSetNotificationShare(pNotifier, pNotifShare) nvjpgctxSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define nvjpgctxGetRefCount(pResource) nvjpgctxGetRefCount_DISPATCH(pResource)
#define nvjpgctxAddAdditionalDependants(pClient, pResource, pReference) nvjpgctxAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define nvjpgctxControl_Prologue(pResource, pCallContext, pParams) nvjpgctxControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define nvjpgctxGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) nvjpgctxGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define nvjpgctxInternalControlForward(pGpuResource, command, pParams, size) nvjpgctxInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define nvjpgctxUnmapFrom(pResource, pParams) nvjpgctxUnmapFrom_DISPATCH(pResource, pParams)
#define nvjpgctxControl_Epilogue(pResource, pCallContext, pParams) nvjpgctxControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define nvjpgctxControlLookup(pResource, pParams, ppEntry) nvjpgctxControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define nvjpgctxGetSwMethods(pChannelDescendant, ppMethods, pNumMethods) nvjpgctxGetSwMethods_DISPATCH(pChannelDescendant, ppMethods, pNumMethods)
#define nvjpgctxGetInternalObjectHandle(pGpuResource) nvjpgctxGetInternalObjectHandle_DISPATCH(pGpuResource)
#define nvjpgctxControl(pGpuResource, pCallContext, pParams) nvjpgctxControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define nvjpgctxUnmap(pGpuResource, pCallContext, pCpuMapping) nvjpgctxUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define nvjpgctxGetMemInterMapParams(pRmResource, pParams) nvjpgctxGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define nvjpgctxGetMemoryMappingDescriptor(pRmResource, ppMemDesc) nvjpgctxGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define nvjpgctxIsSwMethodStalling(pChannelDescendant, hHandle) nvjpgctxIsSwMethodStalling_DISPATCH(pChannelDescendant, hHandle)
#define nvjpgctxControlFilter(pResource, pCallContext, pParams) nvjpgctxControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define nvjpgctxUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) nvjpgctxUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define nvjpgctxControlSerialization_Prologue(pResource, pCallContext, pParams) nvjpgctxControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define nvjpgctxCanCopy(pResource) nvjpgctxCanCopy_DISPATCH(pResource)
#define nvjpgctxPreDestruct(pResource) nvjpgctxPreDestruct_DISPATCH(pResource)
#define nvjpgctxIsDuplicate(pResource, hMemory, pDuplicate) nvjpgctxIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define nvjpgctxControlSerialization_Epilogue(pResource, pCallContext, pParams) nvjpgctxControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define nvjpgctxGetNotificationListPtr(pNotifier) nvjpgctxGetNotificationListPtr_DISPATCH(pNotifier)
#define nvjpgctxGetNotificationShare(pNotifier) nvjpgctxGetNotificationShare_DISPATCH(pNotifier)
#define nvjpgctxMap(pGpuResource, pCallContext, pParams, pCpuMapping) nvjpgctxMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define nvjpgctxGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) nvjpgctxGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)
NV_STATUS nvjpgctxConstructHal_KERNEL(struct NvjpgContext *pNvjpgContext, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);


#ifdef __nvoc_kernel_nvjpg_ctx_h_disabled
static inline NV_STATUS nvjpgctxConstructHal(struct NvjpgContext *pNvjpgContext, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {
    NV_ASSERT_FAILED_PRECOMP("NvjpgContext was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvjpg_ctx_h_disabled
#define nvjpgctxConstructHal(pNvjpgContext, pCallContext, pParams) nvjpgctxConstructHal_KERNEL(pNvjpgContext, pCallContext, pParams)
#endif //__nvoc_kernel_nvjpg_ctx_h_disabled

#define nvjpgctxConstructHal_HAL(pNvjpgContext, pCallContext, pParams) nvjpgctxConstructHal(pNvjpgContext, pCallContext, pParams)

void nvjpgctxDestructHal_KERNEL(struct NvjpgContext *pNvjpgContext);


#ifdef __nvoc_kernel_nvjpg_ctx_h_disabled
static inline void nvjpgctxDestructHal(struct NvjpgContext *pNvjpgContext) {
    NV_ASSERT_FAILED_PRECOMP("NvjpgContext was disabled!");
}
#else //__nvoc_kernel_nvjpg_ctx_h_disabled
#define nvjpgctxDestructHal(pNvjpgContext) nvjpgctxDestructHal_KERNEL(pNvjpgContext)
#endif //__nvoc_kernel_nvjpg_ctx_h_disabled

#define nvjpgctxDestructHal_HAL(pNvjpgContext) nvjpgctxDestructHal(pNvjpgContext)

static inline NV_STATUS nvjpgctxCheckMemInterUnmap_DISPATCH(struct NvjpgContext *pChannelDescendant, NvBool bSubdeviceHandleProvided) {
    return pChannelDescendant->__nvjpgctxCheckMemInterUnmap__(pChannelDescendant, bSubdeviceHandleProvided);
}

static inline NvBool nvjpgctxShareCallback_DISPATCH(struct NvjpgContext *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvjpgctxShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NvBool nvjpgctxAccessCallback_DISPATCH(struct NvjpgContext *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvjpgctxAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS nvjpgctxMapTo_DISPATCH(struct NvjpgContext *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvjpgctxMapTo__(pResource, pParams);
}

static inline NV_STATUS nvjpgctxGetMapAddrSpace_DISPATCH(struct NvjpgContext *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__nvjpgctxGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline void nvjpgctxSetNotificationShare_DISPATCH(struct NvjpgContext *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__nvjpgctxSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NvU32 nvjpgctxGetRefCount_DISPATCH(struct NvjpgContext *pResource) {
    return pResource->__nvjpgctxGetRefCount__(pResource);
}

static inline void nvjpgctxAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct NvjpgContext *pResource, RsResourceRef *pReference) {
    pResource->__nvjpgctxAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS nvjpgctxControl_Prologue_DISPATCH(struct NvjpgContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvjpgctxControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS nvjpgctxGetRegBaseOffsetAndSize_DISPATCH(struct NvjpgContext *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__nvjpgctxGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS nvjpgctxInternalControlForward_DISPATCH(struct NvjpgContext *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__nvjpgctxInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NV_STATUS nvjpgctxUnmapFrom_DISPATCH(struct NvjpgContext *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvjpgctxUnmapFrom__(pResource, pParams);
}

static inline void nvjpgctxControl_Epilogue_DISPATCH(struct NvjpgContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvjpgctxControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS nvjpgctxControlLookup_DISPATCH(struct NvjpgContext *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__nvjpgctxControlLookup__(pResource, pParams, ppEntry);
}

static inline NV_STATUS nvjpgctxGetSwMethods_DISPATCH(struct NvjpgContext *pChannelDescendant, const METHOD **ppMethods, NvU32 *pNumMethods) {
    return pChannelDescendant->__nvjpgctxGetSwMethods__(pChannelDescendant, ppMethods, pNumMethods);
}

static inline NvHandle nvjpgctxGetInternalObjectHandle_DISPATCH(struct NvjpgContext *pGpuResource) {
    return pGpuResource->__nvjpgctxGetInternalObjectHandle__(pGpuResource);
}

static inline NV_STATUS nvjpgctxControl_DISPATCH(struct NvjpgContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__nvjpgctxControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS nvjpgctxUnmap_DISPATCH(struct NvjpgContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvjpgctxUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NV_STATUS nvjpgctxGetMemInterMapParams_DISPATCH(struct NvjpgContext *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvjpgctxGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS nvjpgctxGetMemoryMappingDescriptor_DISPATCH(struct NvjpgContext *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvjpgctxGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NvBool nvjpgctxIsSwMethodStalling_DISPATCH(struct NvjpgContext *pChannelDescendant, NvU32 hHandle) {
    return pChannelDescendant->__nvjpgctxIsSwMethodStalling__(pChannelDescendant, hHandle);
}

static inline NV_STATUS nvjpgctxControlFilter_DISPATCH(struct NvjpgContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvjpgctxControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS nvjpgctxUnregisterEvent_DISPATCH(struct NvjpgContext *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__nvjpgctxUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS nvjpgctxControlSerialization_Prologue_DISPATCH(struct NvjpgContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvjpgctxControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool nvjpgctxCanCopy_DISPATCH(struct NvjpgContext *pResource) {
    return pResource->__nvjpgctxCanCopy__(pResource);
}

static inline void nvjpgctxPreDestruct_DISPATCH(struct NvjpgContext *pResource) {
    pResource->__nvjpgctxPreDestruct__(pResource);
}

static inline NV_STATUS nvjpgctxIsDuplicate_DISPATCH(struct NvjpgContext *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvjpgctxIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void nvjpgctxControlSerialization_Epilogue_DISPATCH(struct NvjpgContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvjpgctxControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline PEVENTNOTIFICATION *nvjpgctxGetNotificationListPtr_DISPATCH(struct NvjpgContext *pNotifier) {
    return pNotifier->__nvjpgctxGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare *nvjpgctxGetNotificationShare_DISPATCH(struct NvjpgContext *pNotifier) {
    return pNotifier->__nvjpgctxGetNotificationShare__(pNotifier);
}

static inline NV_STATUS nvjpgctxMap_DISPATCH(struct NvjpgContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvjpgctxMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS nvjpgctxGetOrAllocNotifShare_DISPATCH(struct NvjpgContext *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__nvjpgctxGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

static inline NV_STATUS __nvoc_nvjpgctxConstruct(struct NvjpgContext *arg_pNvjpgContext, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams) {
    return nvjpgctxConstructHal(arg_pNvjpgContext, arg_pCallContext, arg_pParams);
}

static inline void __nvoc_nvjpgctxDestruct(struct NvjpgContext *pNvjpgContext) {
    nvjpgctxDestructHal(pNvjpgContext);
}

#undef PRIVATE_FIELD


#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_NVJPG_CTX_NVOC_H_
