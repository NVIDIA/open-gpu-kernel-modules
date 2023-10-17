#ifndef _G_KERNEL_NVDEC_CTX_NVOC_H_
#define _G_KERNEL_NVDEC_CTX_NVOC_H_
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

#include "g_kernel_nvdec_ctx_nvoc.h"

#ifndef KERNEL_NVDEC_CTX_H
#define KERNEL_NVDEC_CTX_H

#include "kernel/gpu/fifo/channel_descendant.h"

ENGDESCRIPTOR nvdecGetEngineDescFromAllocParams(OBJGPU *pGpu, NvU32 externalClassId, void *pAllocParams);

/*!
 * RM internal class representing NVXXXX_VIDEO_DECODER
 */
#ifdef NVOC_KERNEL_NVDEC_CTX_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct NvdecContext {
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
    struct NvdecContext *__nvoc_pbase_NvdecContext;
    NV_STATUS (*__nvdecctxCheckMemInterUnmap__)(struct NvdecContext *, NvBool);
    NvBool (*__nvdecctxShareCallback__)(struct NvdecContext *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NvBool (*__nvdecctxAccessCallback__)(struct NvdecContext *, struct RsClient *, void *, RsAccessRight);
    NV_STATUS (*__nvdecctxMapTo__)(struct NvdecContext *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__nvdecctxGetMapAddrSpace__)(struct NvdecContext *, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    void (*__nvdecctxSetNotificationShare__)(struct NvdecContext *, struct NotifShare *);
    NvU32 (*__nvdecctxGetRefCount__)(struct NvdecContext *);
    void (*__nvdecctxAddAdditionalDependants__)(struct RsClient *, struct NvdecContext *, RsResourceRef *);
    NV_STATUS (*__nvdecctxControl_Prologue__)(struct NvdecContext *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__nvdecctxGetRegBaseOffsetAndSize__)(struct NvdecContext *, struct OBJGPU *, NvU32 *, NvU32 *);
    NV_STATUS (*__nvdecctxInternalControlForward__)(struct NvdecContext *, NvU32, void *, NvU32);
    NV_STATUS (*__nvdecctxUnmapFrom__)(struct NvdecContext *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__nvdecctxControl_Epilogue__)(struct NvdecContext *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__nvdecctxControlLookup__)(struct NvdecContext *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NV_STATUS (*__nvdecctxGetSwMethods__)(struct NvdecContext *, const METHOD **, NvU32 *);
    NvHandle (*__nvdecctxGetInternalObjectHandle__)(struct NvdecContext *);
    NV_STATUS (*__nvdecctxControl__)(struct NvdecContext *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__nvdecctxUnmap__)(struct NvdecContext *, struct CALL_CONTEXT *, struct RsCpuMapping *);
    NV_STATUS (*__nvdecctxGetMemInterMapParams__)(struct NvdecContext *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__nvdecctxGetMemoryMappingDescriptor__)(struct NvdecContext *, struct MEMORY_DESCRIPTOR **);
    NvBool (*__nvdecctxIsSwMethodStalling__)(struct NvdecContext *, NvU32);
    NV_STATUS (*__nvdecctxControlFilter__)(struct NvdecContext *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__nvdecctxUnregisterEvent__)(struct NvdecContext *, NvHandle, NvHandle, NvHandle, NvHandle);
    NV_STATUS (*__nvdecctxControlSerialization_Prologue__)(struct NvdecContext *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__nvdecctxCanCopy__)(struct NvdecContext *);
    void (*__nvdecctxPreDestruct__)(struct NvdecContext *);
    NV_STATUS (*__nvdecctxIsDuplicate__)(struct NvdecContext *, NvHandle, NvBool *);
    void (*__nvdecctxControlSerialization_Epilogue__)(struct NvdecContext *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    PEVENTNOTIFICATION *(*__nvdecctxGetNotificationListPtr__)(struct NvdecContext *);
    struct NotifShare *(*__nvdecctxGetNotificationShare__)(struct NvdecContext *);
    NV_STATUS (*__nvdecctxMap__)(struct NvdecContext *, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);
    NV_STATUS (*__nvdecctxGetOrAllocNotifShare__)(struct NvdecContext *, NvHandle, NvHandle, struct NotifShare **);
};

#ifndef __NVOC_CLASS_NvdecContext_TYPEDEF__
#define __NVOC_CLASS_NvdecContext_TYPEDEF__
typedef struct NvdecContext NvdecContext;
#endif /* __NVOC_CLASS_NvdecContext_TYPEDEF__ */

#ifndef __nvoc_class_id_NvdecContext
#define __nvoc_class_id_NvdecContext 0x70d2be
#endif /* __nvoc_class_id_NvdecContext */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvdecContext;

#define __staticCast_NvdecContext(pThis) \
    ((pThis)->__nvoc_pbase_NvdecContext)

#ifdef __nvoc_kernel_nvdec_ctx_h_disabled
#define __dynamicCast_NvdecContext(pThis) ((NvdecContext*)NULL)
#else //__nvoc_kernel_nvdec_ctx_h_disabled
#define __dynamicCast_NvdecContext(pThis) \
    ((NvdecContext*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(NvdecContext)))
#endif //__nvoc_kernel_nvdec_ctx_h_disabled


NV_STATUS __nvoc_objCreateDynamic_NvdecContext(NvdecContext**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_NvdecContext(NvdecContext**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_NvdecContext(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_NvdecContext((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define nvdecctxCheckMemInterUnmap(pChannelDescendant, bSubdeviceHandleProvided) nvdecctxCheckMemInterUnmap_DISPATCH(pChannelDescendant, bSubdeviceHandleProvided)
#define nvdecctxShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) nvdecctxShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define nvdecctxAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) nvdecctxAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define nvdecctxMapTo(pResource, pParams) nvdecctxMapTo_DISPATCH(pResource, pParams)
#define nvdecctxGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) nvdecctxGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define nvdecctxSetNotificationShare(pNotifier, pNotifShare) nvdecctxSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define nvdecctxGetRefCount(pResource) nvdecctxGetRefCount_DISPATCH(pResource)
#define nvdecctxAddAdditionalDependants(pClient, pResource, pReference) nvdecctxAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define nvdecctxControl_Prologue(pResource, pCallContext, pParams) nvdecctxControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define nvdecctxGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) nvdecctxGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define nvdecctxInternalControlForward(pGpuResource, command, pParams, size) nvdecctxInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define nvdecctxUnmapFrom(pResource, pParams) nvdecctxUnmapFrom_DISPATCH(pResource, pParams)
#define nvdecctxControl_Epilogue(pResource, pCallContext, pParams) nvdecctxControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define nvdecctxControlLookup(pResource, pParams, ppEntry) nvdecctxControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define nvdecctxGetSwMethods(pChannelDescendant, ppMethods, pNumMethods) nvdecctxGetSwMethods_DISPATCH(pChannelDescendant, ppMethods, pNumMethods)
#define nvdecctxGetInternalObjectHandle(pGpuResource) nvdecctxGetInternalObjectHandle_DISPATCH(pGpuResource)
#define nvdecctxControl(pGpuResource, pCallContext, pParams) nvdecctxControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define nvdecctxUnmap(pGpuResource, pCallContext, pCpuMapping) nvdecctxUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define nvdecctxGetMemInterMapParams(pRmResource, pParams) nvdecctxGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define nvdecctxGetMemoryMappingDescriptor(pRmResource, ppMemDesc) nvdecctxGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define nvdecctxIsSwMethodStalling(pChannelDescendant, hHandle) nvdecctxIsSwMethodStalling_DISPATCH(pChannelDescendant, hHandle)
#define nvdecctxControlFilter(pResource, pCallContext, pParams) nvdecctxControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define nvdecctxUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) nvdecctxUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define nvdecctxControlSerialization_Prologue(pResource, pCallContext, pParams) nvdecctxControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define nvdecctxCanCopy(pResource) nvdecctxCanCopy_DISPATCH(pResource)
#define nvdecctxPreDestruct(pResource) nvdecctxPreDestruct_DISPATCH(pResource)
#define nvdecctxIsDuplicate(pResource, hMemory, pDuplicate) nvdecctxIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define nvdecctxControlSerialization_Epilogue(pResource, pCallContext, pParams) nvdecctxControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define nvdecctxGetNotificationListPtr(pNotifier) nvdecctxGetNotificationListPtr_DISPATCH(pNotifier)
#define nvdecctxGetNotificationShare(pNotifier) nvdecctxGetNotificationShare_DISPATCH(pNotifier)
#define nvdecctxMap(pGpuResource, pCallContext, pParams, pCpuMapping) nvdecctxMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define nvdecctxGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) nvdecctxGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)
NV_STATUS nvdecctxConstructHal_KERNEL(struct NvdecContext *pNvdecContext, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);


#ifdef __nvoc_kernel_nvdec_ctx_h_disabled
static inline NV_STATUS nvdecctxConstructHal(struct NvdecContext *pNvdecContext, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {
    NV_ASSERT_FAILED_PRECOMP("NvdecContext was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_nvdec_ctx_h_disabled
#define nvdecctxConstructHal(pNvdecContext, pCallContext, pParams) nvdecctxConstructHal_KERNEL(pNvdecContext, pCallContext, pParams)
#endif //__nvoc_kernel_nvdec_ctx_h_disabled

#define nvdecctxConstructHal_HAL(pNvdecContext, pCallContext, pParams) nvdecctxConstructHal(pNvdecContext, pCallContext, pParams)

void nvdecctxDestructHal_KERNEL(struct NvdecContext *pNvdecContext);


#ifdef __nvoc_kernel_nvdec_ctx_h_disabled
static inline void nvdecctxDestructHal(struct NvdecContext *pNvdecContext) {
    NV_ASSERT_FAILED_PRECOMP("NvdecContext was disabled!");
}
#else //__nvoc_kernel_nvdec_ctx_h_disabled
#define nvdecctxDestructHal(pNvdecContext) nvdecctxDestructHal_KERNEL(pNvdecContext)
#endif //__nvoc_kernel_nvdec_ctx_h_disabled

#define nvdecctxDestructHal_HAL(pNvdecContext) nvdecctxDestructHal(pNvdecContext)

static inline NV_STATUS nvdecctxCheckMemInterUnmap_DISPATCH(struct NvdecContext *pChannelDescendant, NvBool bSubdeviceHandleProvided) {
    return pChannelDescendant->__nvdecctxCheckMemInterUnmap__(pChannelDescendant, bSubdeviceHandleProvided);
}

static inline NvBool nvdecctxShareCallback_DISPATCH(struct NvdecContext *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvdecctxShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NvBool nvdecctxAccessCallback_DISPATCH(struct NvdecContext *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvdecctxAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS nvdecctxMapTo_DISPATCH(struct NvdecContext *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvdecctxMapTo__(pResource, pParams);
}

static inline NV_STATUS nvdecctxGetMapAddrSpace_DISPATCH(struct NvdecContext *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__nvdecctxGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline void nvdecctxSetNotificationShare_DISPATCH(struct NvdecContext *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__nvdecctxSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NvU32 nvdecctxGetRefCount_DISPATCH(struct NvdecContext *pResource) {
    return pResource->__nvdecctxGetRefCount__(pResource);
}

static inline void nvdecctxAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct NvdecContext *pResource, RsResourceRef *pReference) {
    pResource->__nvdecctxAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS nvdecctxControl_Prologue_DISPATCH(struct NvdecContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvdecctxControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS nvdecctxGetRegBaseOffsetAndSize_DISPATCH(struct NvdecContext *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__nvdecctxGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS nvdecctxInternalControlForward_DISPATCH(struct NvdecContext *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__nvdecctxInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NV_STATUS nvdecctxUnmapFrom_DISPATCH(struct NvdecContext *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvdecctxUnmapFrom__(pResource, pParams);
}

static inline void nvdecctxControl_Epilogue_DISPATCH(struct NvdecContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvdecctxControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS nvdecctxControlLookup_DISPATCH(struct NvdecContext *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__nvdecctxControlLookup__(pResource, pParams, ppEntry);
}

static inline NV_STATUS nvdecctxGetSwMethods_DISPATCH(struct NvdecContext *pChannelDescendant, const METHOD **ppMethods, NvU32 *pNumMethods) {
    return pChannelDescendant->__nvdecctxGetSwMethods__(pChannelDescendant, ppMethods, pNumMethods);
}

static inline NvHandle nvdecctxGetInternalObjectHandle_DISPATCH(struct NvdecContext *pGpuResource) {
    return pGpuResource->__nvdecctxGetInternalObjectHandle__(pGpuResource);
}

static inline NV_STATUS nvdecctxControl_DISPATCH(struct NvdecContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__nvdecctxControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS nvdecctxUnmap_DISPATCH(struct NvdecContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvdecctxUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NV_STATUS nvdecctxGetMemInterMapParams_DISPATCH(struct NvdecContext *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvdecctxGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS nvdecctxGetMemoryMappingDescriptor_DISPATCH(struct NvdecContext *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvdecctxGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NvBool nvdecctxIsSwMethodStalling_DISPATCH(struct NvdecContext *pChannelDescendant, NvU32 hHandle) {
    return pChannelDescendant->__nvdecctxIsSwMethodStalling__(pChannelDescendant, hHandle);
}

static inline NV_STATUS nvdecctxControlFilter_DISPATCH(struct NvdecContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvdecctxControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS nvdecctxUnregisterEvent_DISPATCH(struct NvdecContext *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__nvdecctxUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS nvdecctxControlSerialization_Prologue_DISPATCH(struct NvdecContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvdecctxControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool nvdecctxCanCopy_DISPATCH(struct NvdecContext *pResource) {
    return pResource->__nvdecctxCanCopy__(pResource);
}

static inline void nvdecctxPreDestruct_DISPATCH(struct NvdecContext *pResource) {
    pResource->__nvdecctxPreDestruct__(pResource);
}

static inline NV_STATUS nvdecctxIsDuplicate_DISPATCH(struct NvdecContext *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvdecctxIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void nvdecctxControlSerialization_Epilogue_DISPATCH(struct NvdecContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvdecctxControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline PEVENTNOTIFICATION *nvdecctxGetNotificationListPtr_DISPATCH(struct NvdecContext *pNotifier) {
    return pNotifier->__nvdecctxGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare *nvdecctxGetNotificationShare_DISPATCH(struct NvdecContext *pNotifier) {
    return pNotifier->__nvdecctxGetNotificationShare__(pNotifier);
}

static inline NV_STATUS nvdecctxMap_DISPATCH(struct NvdecContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvdecctxMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS nvdecctxGetOrAllocNotifShare_DISPATCH(struct NvdecContext *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__nvdecctxGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

static inline NV_STATUS __nvoc_nvdecctxConstruct(struct NvdecContext *arg_pNvdecContext, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams) {
    return nvdecctxConstructHal(arg_pNvdecContext, arg_pCallContext, arg_pParams);
}

static inline void __nvoc_nvdecctxDestruct(struct NvdecContext *pNvdecContext) {
    nvdecctxDestructHal(pNvdecContext);
}

#undef PRIVATE_FIELD


#endif // KERNEL_NVDEC_CTX_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_NVDEC_CTX_NVOC_H_
