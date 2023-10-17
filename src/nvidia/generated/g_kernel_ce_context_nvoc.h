#ifndef _G_KERNEL_CE_CONTEXT_NVOC_H_
#define _G_KERNEL_CE_CONTEXT_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_kernel_ce_context_nvoc.h"

#ifndef KERNEL_CE_CONTEXT_H
#define KERNEL_CE_CONTEXT_H

#include "core/core.h"
#include "gpu/eng_state.h"
#include "gpu/gpu_halspec.h"
#include "gpu/gpu.h"
#include "kernel/gpu/fifo/channel_descendant.h"

ENGDESCRIPTOR kceGetEngineDescFromAllocParams(struct OBJGPU *pGpu, NvU32 externalClassId, void *pAllocParams);

/*!
 * RM internal class representing XXX_DMA_COPY_A
 */
#ifdef NVOC_KERNEL_CE_CONTEXT_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct KernelCeContext {
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
    struct KernelCeContext *__nvoc_pbase_KernelCeContext;
    NV_STATUS (*__kcectxCheckMemInterUnmap__)(struct KernelCeContext *, NvBool);
    NvBool (*__kcectxShareCallback__)(struct KernelCeContext *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NvBool (*__kcectxAccessCallback__)(struct KernelCeContext *, struct RsClient *, void *, RsAccessRight);
    NV_STATUS (*__kcectxMapTo__)(struct KernelCeContext *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__kcectxGetMapAddrSpace__)(struct KernelCeContext *, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    void (*__kcectxSetNotificationShare__)(struct KernelCeContext *, struct NotifShare *);
    NvU32 (*__kcectxGetRefCount__)(struct KernelCeContext *);
    void (*__kcectxAddAdditionalDependants__)(struct RsClient *, struct KernelCeContext *, RsResourceRef *);
    NV_STATUS (*__kcectxControl_Prologue__)(struct KernelCeContext *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__kcectxGetRegBaseOffsetAndSize__)(struct KernelCeContext *, struct OBJGPU *, NvU32 *, NvU32 *);
    NV_STATUS (*__kcectxInternalControlForward__)(struct KernelCeContext *, NvU32, void *, NvU32);
    NV_STATUS (*__kcectxUnmapFrom__)(struct KernelCeContext *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__kcectxControl_Epilogue__)(struct KernelCeContext *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__kcectxControlLookup__)(struct KernelCeContext *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NV_STATUS (*__kcectxGetSwMethods__)(struct KernelCeContext *, const METHOD **, NvU32 *);
    NvHandle (*__kcectxGetInternalObjectHandle__)(struct KernelCeContext *);
    NV_STATUS (*__kcectxControl__)(struct KernelCeContext *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__kcectxUnmap__)(struct KernelCeContext *, struct CALL_CONTEXT *, struct RsCpuMapping *);
    NV_STATUS (*__kcectxGetMemInterMapParams__)(struct KernelCeContext *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__kcectxGetMemoryMappingDescriptor__)(struct KernelCeContext *, struct MEMORY_DESCRIPTOR **);
    NvBool (*__kcectxIsSwMethodStalling__)(struct KernelCeContext *, NvU32);
    NV_STATUS (*__kcectxControlFilter__)(struct KernelCeContext *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__kcectxUnregisterEvent__)(struct KernelCeContext *, NvHandle, NvHandle, NvHandle, NvHandle);
    NV_STATUS (*__kcectxControlSerialization_Prologue__)(struct KernelCeContext *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__kcectxCanCopy__)(struct KernelCeContext *);
    void (*__kcectxPreDestruct__)(struct KernelCeContext *);
    NV_STATUS (*__kcectxIsDuplicate__)(struct KernelCeContext *, NvHandle, NvBool *);
    void (*__kcectxControlSerialization_Epilogue__)(struct KernelCeContext *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    PEVENTNOTIFICATION *(*__kcectxGetNotificationListPtr__)(struct KernelCeContext *);
    struct NotifShare *(*__kcectxGetNotificationShare__)(struct KernelCeContext *);
    NV_STATUS (*__kcectxMap__)(struct KernelCeContext *, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);
    NV_STATUS (*__kcectxGetOrAllocNotifShare__)(struct KernelCeContext *, NvHandle, NvHandle, struct NotifShare **);
};

#ifndef __NVOC_CLASS_KernelCeContext_TYPEDEF__
#define __NVOC_CLASS_KernelCeContext_TYPEDEF__
typedef struct KernelCeContext KernelCeContext;
#endif /* __NVOC_CLASS_KernelCeContext_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelCeContext
#define __nvoc_class_id_KernelCeContext 0x2d0ee9
#endif /* __nvoc_class_id_KernelCeContext */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelCeContext;

#define __staticCast_KernelCeContext(pThis) \
    ((pThis)->__nvoc_pbase_KernelCeContext)

#ifdef __nvoc_kernel_ce_context_h_disabled
#define __dynamicCast_KernelCeContext(pThis) ((KernelCeContext*)NULL)
#else //__nvoc_kernel_ce_context_h_disabled
#define __dynamicCast_KernelCeContext(pThis) \
    ((KernelCeContext*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelCeContext)))
#endif //__nvoc_kernel_ce_context_h_disabled


NV_STATUS __nvoc_objCreateDynamic_KernelCeContext(KernelCeContext**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelCeContext(KernelCeContext**, Dynamic*, NvU32, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_KernelCeContext(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_KernelCeContext((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define kcectxCheckMemInterUnmap(pChannelDescendant, bSubdeviceHandleProvided) kcectxCheckMemInterUnmap_DISPATCH(pChannelDescendant, bSubdeviceHandleProvided)
#define kcectxShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) kcectxShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define kcectxAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) kcectxAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define kcectxMapTo(pResource, pParams) kcectxMapTo_DISPATCH(pResource, pParams)
#define kcectxGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) kcectxGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define kcectxSetNotificationShare(pNotifier, pNotifShare) kcectxSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define kcectxGetRefCount(pResource) kcectxGetRefCount_DISPATCH(pResource)
#define kcectxAddAdditionalDependants(pClient, pResource, pReference) kcectxAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define kcectxControl_Prologue(pResource, pCallContext, pParams) kcectxControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define kcectxGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) kcectxGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define kcectxInternalControlForward(pGpuResource, command, pParams, size) kcectxInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define kcectxUnmapFrom(pResource, pParams) kcectxUnmapFrom_DISPATCH(pResource, pParams)
#define kcectxControl_Epilogue(pResource, pCallContext, pParams) kcectxControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define kcectxControlLookup(pResource, pParams, ppEntry) kcectxControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define kcectxGetSwMethods(pChannelDescendant, ppMethods, pNumMethods) kcectxGetSwMethods_DISPATCH(pChannelDescendant, ppMethods, pNumMethods)
#define kcectxGetInternalObjectHandle(pGpuResource) kcectxGetInternalObjectHandle_DISPATCH(pGpuResource)
#define kcectxControl(pGpuResource, pCallContext, pParams) kcectxControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define kcectxUnmap(pGpuResource, pCallContext, pCpuMapping) kcectxUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define kcectxGetMemInterMapParams(pRmResource, pParams) kcectxGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define kcectxGetMemoryMappingDescriptor(pRmResource, ppMemDesc) kcectxGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define kcectxIsSwMethodStalling(pChannelDescendant, hHandle) kcectxIsSwMethodStalling_DISPATCH(pChannelDescendant, hHandle)
#define kcectxControlFilter(pResource, pCallContext, pParams) kcectxControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define kcectxUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) kcectxUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define kcectxControlSerialization_Prologue(pResource, pCallContext, pParams) kcectxControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define kcectxCanCopy(pResource) kcectxCanCopy_DISPATCH(pResource)
#define kcectxPreDestruct(pResource) kcectxPreDestruct_DISPATCH(pResource)
#define kcectxIsDuplicate(pResource, hMemory, pDuplicate) kcectxIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define kcectxControlSerialization_Epilogue(pResource, pCallContext, pParams) kcectxControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define kcectxGetNotificationListPtr(pNotifier) kcectxGetNotificationListPtr_DISPATCH(pNotifier)
#define kcectxGetNotificationShare(pNotifier) kcectxGetNotificationShare_DISPATCH(pNotifier)
#define kcectxMap(pGpuResource, pCallContext, pParams, pCpuMapping) kcectxMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define kcectxGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) kcectxGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)
static inline NV_STATUS kcectxCheckMemInterUnmap_DISPATCH(struct KernelCeContext *pChannelDescendant, NvBool bSubdeviceHandleProvided) {
    return pChannelDescendant->__kcectxCheckMemInterUnmap__(pChannelDescendant, bSubdeviceHandleProvided);
}

static inline NvBool kcectxShareCallback_DISPATCH(struct KernelCeContext *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__kcectxShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NvBool kcectxAccessCallback_DISPATCH(struct KernelCeContext *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__kcectxAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS kcectxMapTo_DISPATCH(struct KernelCeContext *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__kcectxMapTo__(pResource, pParams);
}

static inline NV_STATUS kcectxGetMapAddrSpace_DISPATCH(struct KernelCeContext *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__kcectxGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline void kcectxSetNotificationShare_DISPATCH(struct KernelCeContext *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__kcectxSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NvU32 kcectxGetRefCount_DISPATCH(struct KernelCeContext *pResource) {
    return pResource->__kcectxGetRefCount__(pResource);
}

static inline void kcectxAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct KernelCeContext *pResource, RsResourceRef *pReference) {
    pResource->__kcectxAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS kcectxControl_Prologue_DISPATCH(struct KernelCeContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__kcectxControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS kcectxGetRegBaseOffsetAndSize_DISPATCH(struct KernelCeContext *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__kcectxGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS kcectxInternalControlForward_DISPATCH(struct KernelCeContext *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__kcectxInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NV_STATUS kcectxUnmapFrom_DISPATCH(struct KernelCeContext *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__kcectxUnmapFrom__(pResource, pParams);
}

static inline void kcectxControl_Epilogue_DISPATCH(struct KernelCeContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__kcectxControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS kcectxControlLookup_DISPATCH(struct KernelCeContext *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__kcectxControlLookup__(pResource, pParams, ppEntry);
}

static inline NV_STATUS kcectxGetSwMethods_DISPATCH(struct KernelCeContext *pChannelDescendant, const METHOD **ppMethods, NvU32 *pNumMethods) {
    return pChannelDescendant->__kcectxGetSwMethods__(pChannelDescendant, ppMethods, pNumMethods);
}

static inline NvHandle kcectxGetInternalObjectHandle_DISPATCH(struct KernelCeContext *pGpuResource) {
    return pGpuResource->__kcectxGetInternalObjectHandle__(pGpuResource);
}

static inline NV_STATUS kcectxControl_DISPATCH(struct KernelCeContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__kcectxControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS kcectxUnmap_DISPATCH(struct KernelCeContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__kcectxUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NV_STATUS kcectxGetMemInterMapParams_DISPATCH(struct KernelCeContext *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__kcectxGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS kcectxGetMemoryMappingDescriptor_DISPATCH(struct KernelCeContext *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__kcectxGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NvBool kcectxIsSwMethodStalling_DISPATCH(struct KernelCeContext *pChannelDescendant, NvU32 hHandle) {
    return pChannelDescendant->__kcectxIsSwMethodStalling__(pChannelDescendant, hHandle);
}

static inline NV_STATUS kcectxControlFilter_DISPATCH(struct KernelCeContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__kcectxControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS kcectxUnregisterEvent_DISPATCH(struct KernelCeContext *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__kcectxUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS kcectxControlSerialization_Prologue_DISPATCH(struct KernelCeContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__kcectxControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool kcectxCanCopy_DISPATCH(struct KernelCeContext *pResource) {
    return pResource->__kcectxCanCopy__(pResource);
}

static inline void kcectxPreDestruct_DISPATCH(struct KernelCeContext *pResource) {
    pResource->__kcectxPreDestruct__(pResource);
}

static inline NV_STATUS kcectxIsDuplicate_DISPATCH(struct KernelCeContext *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__kcectxIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void kcectxControlSerialization_Epilogue_DISPATCH(struct KernelCeContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__kcectxControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline PEVENTNOTIFICATION *kcectxGetNotificationListPtr_DISPATCH(struct KernelCeContext *pNotifier) {
    return pNotifier->__kcectxGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare *kcectxGetNotificationShare_DISPATCH(struct KernelCeContext *pNotifier) {
    return pNotifier->__kcectxGetNotificationShare__(pNotifier);
}

static inline NV_STATUS kcectxMap_DISPATCH(struct KernelCeContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__kcectxMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS kcectxGetOrAllocNotifShare_DISPATCH(struct KernelCeContext *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__kcectxGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

NV_STATUS kcectxConstruct_IMPL(struct KernelCeContext *arg_pKCeContext, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_kcectxConstruct(arg_pKCeContext, arg_pCallContext, arg_pParams) kcectxConstruct_IMPL(arg_pKCeContext, arg_pCallContext, arg_pParams)
void kcectxDestruct_IMPL(struct KernelCeContext *pKCeContext);

#define __nvoc_kcectxDestruct(pKCeContext) kcectxDestruct_IMPL(pKCeContext)
#undef PRIVATE_FIELD


#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_CE_CONTEXT_NVOC_H_
