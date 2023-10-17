#ifndef _G_KERNEL_NVENC_CTX_NVOC_H_
#define _G_KERNEL_NVENC_CTX_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2008-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_kernel_nvenc_ctx_nvoc.h"

#ifndef KERNEL_NVENC_CTX_H
#define KERNEL_NVENC_CTX_H

#include "kernel/gpu/fifo/channel_descendant.h"

ENGDESCRIPTOR msencGetEngineDescFromAllocParams(OBJGPU *pGpu, NvU32 externalClassId, void *pAllocParams);

/*!
 * RM internal class representing NVXXXX_VIDEO_ENCODER
 */
#ifdef NVOC_KERNEL_NVENC_CTX_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct MsencContext {
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
    struct MsencContext *__nvoc_pbase_MsencContext;
    NV_STATUS (*__msencctxCheckMemInterUnmap__)(struct MsencContext *, NvBool);
    NvBool (*__msencctxShareCallback__)(struct MsencContext *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NvBool (*__msencctxAccessCallback__)(struct MsencContext *, struct RsClient *, void *, RsAccessRight);
    NV_STATUS (*__msencctxMapTo__)(struct MsencContext *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__msencctxGetMapAddrSpace__)(struct MsencContext *, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    void (*__msencctxSetNotificationShare__)(struct MsencContext *, struct NotifShare *);
    NvU32 (*__msencctxGetRefCount__)(struct MsencContext *);
    void (*__msencctxAddAdditionalDependants__)(struct RsClient *, struct MsencContext *, RsResourceRef *);
    NV_STATUS (*__msencctxControl_Prologue__)(struct MsencContext *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__msencctxGetRegBaseOffsetAndSize__)(struct MsencContext *, struct OBJGPU *, NvU32 *, NvU32 *);
    NV_STATUS (*__msencctxInternalControlForward__)(struct MsencContext *, NvU32, void *, NvU32);
    NV_STATUS (*__msencctxUnmapFrom__)(struct MsencContext *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__msencctxControl_Epilogue__)(struct MsencContext *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__msencctxControlLookup__)(struct MsencContext *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NV_STATUS (*__msencctxGetSwMethods__)(struct MsencContext *, const METHOD **, NvU32 *);
    NvHandle (*__msencctxGetInternalObjectHandle__)(struct MsencContext *);
    NV_STATUS (*__msencctxControl__)(struct MsencContext *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__msencctxUnmap__)(struct MsencContext *, struct CALL_CONTEXT *, struct RsCpuMapping *);
    NV_STATUS (*__msencctxGetMemInterMapParams__)(struct MsencContext *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__msencctxGetMemoryMappingDescriptor__)(struct MsencContext *, struct MEMORY_DESCRIPTOR **);
    NvBool (*__msencctxIsSwMethodStalling__)(struct MsencContext *, NvU32);
    NV_STATUS (*__msencctxControlFilter__)(struct MsencContext *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__msencctxUnregisterEvent__)(struct MsencContext *, NvHandle, NvHandle, NvHandle, NvHandle);
    NV_STATUS (*__msencctxControlSerialization_Prologue__)(struct MsencContext *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__msencctxCanCopy__)(struct MsencContext *);
    void (*__msencctxPreDestruct__)(struct MsencContext *);
    NV_STATUS (*__msencctxIsDuplicate__)(struct MsencContext *, NvHandle, NvBool *);
    void (*__msencctxControlSerialization_Epilogue__)(struct MsencContext *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    PEVENTNOTIFICATION *(*__msencctxGetNotificationListPtr__)(struct MsencContext *);
    struct NotifShare *(*__msencctxGetNotificationShare__)(struct MsencContext *);
    NV_STATUS (*__msencctxMap__)(struct MsencContext *, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);
    NV_STATUS (*__msencctxGetOrAllocNotifShare__)(struct MsencContext *, NvHandle, NvHandle, struct NotifShare **);
};

#ifndef __NVOC_CLASS_MsencContext_TYPEDEF__
#define __NVOC_CLASS_MsencContext_TYPEDEF__
typedef struct MsencContext MsencContext;
#endif /* __NVOC_CLASS_MsencContext_TYPEDEF__ */

#ifndef __nvoc_class_id_MsencContext
#define __nvoc_class_id_MsencContext 0x88c92a
#endif /* __nvoc_class_id_MsencContext */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_MsencContext;

#define __staticCast_MsencContext(pThis) \
    ((pThis)->__nvoc_pbase_MsencContext)

#ifdef __nvoc_kernel_nvenc_ctx_h_disabled
#define __dynamicCast_MsencContext(pThis) ((MsencContext*)NULL)
#else //__nvoc_kernel_nvenc_ctx_h_disabled
#define __dynamicCast_MsencContext(pThis) \
    ((MsencContext*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(MsencContext)))
#endif //__nvoc_kernel_nvenc_ctx_h_disabled


NV_STATUS __nvoc_objCreateDynamic_MsencContext(MsencContext**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_MsencContext(MsencContext**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_MsencContext(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_MsencContext((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define msencctxCheckMemInterUnmap(pChannelDescendant, bSubdeviceHandleProvided) msencctxCheckMemInterUnmap_DISPATCH(pChannelDescendant, bSubdeviceHandleProvided)
#define msencctxShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) msencctxShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define msencctxAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) msencctxAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define msencctxMapTo(pResource, pParams) msencctxMapTo_DISPATCH(pResource, pParams)
#define msencctxGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) msencctxGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define msencctxSetNotificationShare(pNotifier, pNotifShare) msencctxSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define msencctxGetRefCount(pResource) msencctxGetRefCount_DISPATCH(pResource)
#define msencctxAddAdditionalDependants(pClient, pResource, pReference) msencctxAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define msencctxControl_Prologue(pResource, pCallContext, pParams) msencctxControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define msencctxGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) msencctxGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define msencctxInternalControlForward(pGpuResource, command, pParams, size) msencctxInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define msencctxUnmapFrom(pResource, pParams) msencctxUnmapFrom_DISPATCH(pResource, pParams)
#define msencctxControl_Epilogue(pResource, pCallContext, pParams) msencctxControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define msencctxControlLookup(pResource, pParams, ppEntry) msencctxControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define msencctxGetSwMethods(pChannelDescendant, ppMethods, pNumMethods) msencctxGetSwMethods_DISPATCH(pChannelDescendant, ppMethods, pNumMethods)
#define msencctxGetInternalObjectHandle(pGpuResource) msencctxGetInternalObjectHandle_DISPATCH(pGpuResource)
#define msencctxControl(pGpuResource, pCallContext, pParams) msencctxControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define msencctxUnmap(pGpuResource, pCallContext, pCpuMapping) msencctxUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define msencctxGetMemInterMapParams(pRmResource, pParams) msencctxGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define msencctxGetMemoryMappingDescriptor(pRmResource, ppMemDesc) msencctxGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define msencctxIsSwMethodStalling(pChannelDescendant, hHandle) msencctxIsSwMethodStalling_DISPATCH(pChannelDescendant, hHandle)
#define msencctxControlFilter(pResource, pCallContext, pParams) msencctxControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define msencctxUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) msencctxUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define msencctxControlSerialization_Prologue(pResource, pCallContext, pParams) msencctxControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define msencctxCanCopy(pResource) msencctxCanCopy_DISPATCH(pResource)
#define msencctxPreDestruct(pResource) msencctxPreDestruct_DISPATCH(pResource)
#define msencctxIsDuplicate(pResource, hMemory, pDuplicate) msencctxIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define msencctxControlSerialization_Epilogue(pResource, pCallContext, pParams) msencctxControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define msencctxGetNotificationListPtr(pNotifier) msencctxGetNotificationListPtr_DISPATCH(pNotifier)
#define msencctxGetNotificationShare(pNotifier) msencctxGetNotificationShare_DISPATCH(pNotifier)
#define msencctxMap(pGpuResource, pCallContext, pParams, pCpuMapping) msencctxMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define msencctxGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) msencctxGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)
NV_STATUS msencctxConstructHal_KERNEL(struct MsencContext *pMsencContext, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);


#ifdef __nvoc_kernel_nvenc_ctx_h_disabled
static inline NV_STATUS msencctxConstructHal(struct MsencContext *pMsencContext, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {
    NV_ASSERT_FAILED_PRECOMP("MsencContext was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvenc_ctx_h_disabled
#define msencctxConstructHal(pMsencContext, pCallContext, pParams) msencctxConstructHal_KERNEL(pMsencContext, pCallContext, pParams)
#endif //__nvoc_kernel_nvenc_ctx_h_disabled

#define msencctxConstructHal_HAL(pMsencContext, pCallContext, pParams) msencctxConstructHal(pMsencContext, pCallContext, pParams)

void msencctxDestructHal_KERNEL(struct MsencContext *pMsencContext);


#ifdef __nvoc_kernel_nvenc_ctx_h_disabled
static inline void msencctxDestructHal(struct MsencContext *pMsencContext) {
    NV_ASSERT_FAILED_PRECOMP("MsencContext was disabled!");
}
#else //__nvoc_kernel_nvenc_ctx_h_disabled
#define msencctxDestructHal(pMsencContext) msencctxDestructHal_KERNEL(pMsencContext)
#endif //__nvoc_kernel_nvenc_ctx_h_disabled

#define msencctxDestructHal_HAL(pMsencContext) msencctxDestructHal(pMsencContext)

static inline NV_STATUS msencctxCheckMemInterUnmap_DISPATCH(struct MsencContext *pChannelDescendant, NvBool bSubdeviceHandleProvided) {
    return pChannelDescendant->__msencctxCheckMemInterUnmap__(pChannelDescendant, bSubdeviceHandleProvided);
}

static inline NvBool msencctxShareCallback_DISPATCH(struct MsencContext *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__msencctxShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NvBool msencctxAccessCallback_DISPATCH(struct MsencContext *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__msencctxAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS msencctxMapTo_DISPATCH(struct MsencContext *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__msencctxMapTo__(pResource, pParams);
}

static inline NV_STATUS msencctxGetMapAddrSpace_DISPATCH(struct MsencContext *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__msencctxGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline void msencctxSetNotificationShare_DISPATCH(struct MsencContext *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__msencctxSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NvU32 msencctxGetRefCount_DISPATCH(struct MsencContext *pResource) {
    return pResource->__msencctxGetRefCount__(pResource);
}

static inline void msencctxAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct MsencContext *pResource, RsResourceRef *pReference) {
    pResource->__msencctxAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS msencctxControl_Prologue_DISPATCH(struct MsencContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__msencctxControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS msencctxGetRegBaseOffsetAndSize_DISPATCH(struct MsencContext *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__msencctxGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS msencctxInternalControlForward_DISPATCH(struct MsencContext *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__msencctxInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NV_STATUS msencctxUnmapFrom_DISPATCH(struct MsencContext *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__msencctxUnmapFrom__(pResource, pParams);
}

static inline void msencctxControl_Epilogue_DISPATCH(struct MsencContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__msencctxControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS msencctxControlLookup_DISPATCH(struct MsencContext *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__msencctxControlLookup__(pResource, pParams, ppEntry);
}

static inline NV_STATUS msencctxGetSwMethods_DISPATCH(struct MsencContext *pChannelDescendant, const METHOD **ppMethods, NvU32 *pNumMethods) {
    return pChannelDescendant->__msencctxGetSwMethods__(pChannelDescendant, ppMethods, pNumMethods);
}

static inline NvHandle msencctxGetInternalObjectHandle_DISPATCH(struct MsencContext *pGpuResource) {
    return pGpuResource->__msencctxGetInternalObjectHandle__(pGpuResource);
}

static inline NV_STATUS msencctxControl_DISPATCH(struct MsencContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__msencctxControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS msencctxUnmap_DISPATCH(struct MsencContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__msencctxUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NV_STATUS msencctxGetMemInterMapParams_DISPATCH(struct MsencContext *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__msencctxGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS msencctxGetMemoryMappingDescriptor_DISPATCH(struct MsencContext *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__msencctxGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NvBool msencctxIsSwMethodStalling_DISPATCH(struct MsencContext *pChannelDescendant, NvU32 hHandle) {
    return pChannelDescendant->__msencctxIsSwMethodStalling__(pChannelDescendant, hHandle);
}

static inline NV_STATUS msencctxControlFilter_DISPATCH(struct MsencContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__msencctxControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS msencctxUnregisterEvent_DISPATCH(struct MsencContext *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__msencctxUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS msencctxControlSerialization_Prologue_DISPATCH(struct MsencContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__msencctxControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool msencctxCanCopy_DISPATCH(struct MsencContext *pResource) {
    return pResource->__msencctxCanCopy__(pResource);
}

static inline void msencctxPreDestruct_DISPATCH(struct MsencContext *pResource) {
    pResource->__msencctxPreDestruct__(pResource);
}

static inline NV_STATUS msencctxIsDuplicate_DISPATCH(struct MsencContext *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__msencctxIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void msencctxControlSerialization_Epilogue_DISPATCH(struct MsencContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__msencctxControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline PEVENTNOTIFICATION *msencctxGetNotificationListPtr_DISPATCH(struct MsencContext *pNotifier) {
    return pNotifier->__msencctxGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare *msencctxGetNotificationShare_DISPATCH(struct MsencContext *pNotifier) {
    return pNotifier->__msencctxGetNotificationShare__(pNotifier);
}

static inline NV_STATUS msencctxMap_DISPATCH(struct MsencContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__msencctxMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS msencctxGetOrAllocNotifShare_DISPATCH(struct MsencContext *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__msencctxGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

static inline NV_STATUS __nvoc_msencctxConstruct(struct MsencContext *arg_pMsencContext, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams) {
    return msencctxConstructHal(arg_pMsencContext, arg_pCallContext, arg_pParams);
}

static inline void __nvoc_msencctxDestruct(struct MsencContext *pMsencContext) {
    msencctxDestructHal(pMsencContext);
}

#undef PRIVATE_FIELD


#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_NVENC_CTX_NVOC_H_
