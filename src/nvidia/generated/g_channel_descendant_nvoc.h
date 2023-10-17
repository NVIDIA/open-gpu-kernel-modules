#ifndef _G_CHANNEL_DESCENDANT_NVOC_H_
#define _G_CHANNEL_DESCENDANT_NVOC_H_
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

#include "g_channel_descendant_nvoc.h"

#ifndef _CHANNEL_DESCENDANT_H_
#define _CHANNEL_DESCENDANT_H_

#include "core/core.h"
#include "rmapi/event.h"

#include "containers/btree.h"
#include "resserv/rs_resource.h"
#include "gpu/gpu_resource.h"
#include "gpu/gpu_resource_desc.h"
#include "kernel/gpu/gpu_halspec.h"

struct ChannelDescendant;

#ifndef __NVOC_CLASS_ChannelDescendant_TYPEDEF__
#define __NVOC_CLASS_ChannelDescendant_TYPEDEF__
typedef struct ChannelDescendant ChannelDescendant;
#endif /* __NVOC_CLASS_ChannelDescendant_TYPEDEF__ */

#ifndef __nvoc_class_id_ChannelDescendant
#define __nvoc_class_id_ChannelDescendant 0x43d7c4
#endif /* __nvoc_class_id_ChannelDescendant */


struct ContextDma;

#ifndef __NVOC_CLASS_ContextDma_TYPEDEF__
#define __NVOC_CLASS_ContextDma_TYPEDEF__
typedef struct ContextDma ContextDma;
#endif /* __NVOC_CLASS_ContextDma_TYPEDEF__ */

#ifndef __nvoc_class_id_ContextDma
#define __nvoc_class_id_ContextDma 0x88441b
#endif /* __nvoc_class_id_ContextDma */


struct KernelChannel;

#ifndef __NVOC_CLASS_KernelChannel_TYPEDEF__
#define __NVOC_CLASS_KernelChannel_TYPEDEF__
typedef struct KernelChannel KernelChannel;
#endif /* __NVOC_CLASS_KernelChannel_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelChannel
#define __nvoc_class_id_KernelChannel 0x5d8d70
#endif /* __nvoc_class_id_KernelChannel */



/*!
 * Definitions for SW methods (emulation of pushbuffer methods in SW)
 */
typedef struct _METHOD METHOD, *PMETHOD;

typedef NV_STATUS (*METHODPROC)(OBJGPU *, struct ChannelDescendant *, NvU32, NvV32);

struct _METHOD
{
    METHODPROC Proc;
    NvU32      Low;
    NvU32      High;
};

/*!
 * Determines id engine that should handle the resource allocation. Used when
 * there are multiple engines that support the same class id (e.g.: Copy
 * Engine).
 */
typedef ENGDESCRIPTOR PARAM_TO_ENGDESC_FUNCTION(OBJGPU *pGpu, NvU32 externalClassId,
                                                void *pAllocParams);

/*!
 * Abstract base class for descendants of XXX_CHANNEL_DMA (Channel)
 */
#ifdef NVOC_CHANNEL_DESCENDANT_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct ChannelDescendant {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct GpuResource __nvoc_base_GpuResource;
    struct Notifier __nvoc_base_Notifier;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct GpuResource *__nvoc_pbase_GpuResource;
    struct INotifier *__nvoc_pbase_INotifier;
    struct Notifier *__nvoc_pbase_Notifier;
    struct ChannelDescendant *__nvoc_pbase_ChannelDescendant;
    NV_STATUS (*__chandesGetSwMethods__)(struct ChannelDescendant *, const METHOD **, NvU32 *);
    NvBool (*__chandesIsSwMethodStalling__)(struct ChannelDescendant *, NvU32);
    NV_STATUS (*__chandesCheckMemInterUnmap__)(struct ChannelDescendant *, NvBool);
    NvBool (*__chandesShareCallback__)(struct ChannelDescendant *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__chandesGetOrAllocNotifShare__)(struct ChannelDescendant *, NvHandle, NvHandle, struct NotifShare **);
    NV_STATUS (*__chandesMapTo__)(struct ChannelDescendant *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__chandesGetMapAddrSpace__)(struct ChannelDescendant *, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    void (*__chandesSetNotificationShare__)(struct ChannelDescendant *, struct NotifShare *);
    NvU32 (*__chandesGetRefCount__)(struct ChannelDescendant *);
    void (*__chandesAddAdditionalDependants__)(struct RsClient *, struct ChannelDescendant *, RsResourceRef *);
    NV_STATUS (*__chandesControl_Prologue__)(struct ChannelDescendant *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__chandesGetRegBaseOffsetAndSize__)(struct ChannelDescendant *, struct OBJGPU *, NvU32 *, NvU32 *);
    NV_STATUS (*__chandesInternalControlForward__)(struct ChannelDescendant *, NvU32, void *, NvU32);
    NV_STATUS (*__chandesUnmapFrom__)(struct ChannelDescendant *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__chandesControl_Epilogue__)(struct ChannelDescendant *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__chandesControlLookup__)(struct ChannelDescendant *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NvHandle (*__chandesGetInternalObjectHandle__)(struct ChannelDescendant *);
    NV_STATUS (*__chandesControl__)(struct ChannelDescendant *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__chandesUnmap__)(struct ChannelDescendant *, struct CALL_CONTEXT *, struct RsCpuMapping *);
    NV_STATUS (*__chandesGetMemInterMapParams__)(struct ChannelDescendant *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__chandesGetMemoryMappingDescriptor__)(struct ChannelDescendant *, struct MEMORY_DESCRIPTOR **);
    NV_STATUS (*__chandesControlFilter__)(struct ChannelDescendant *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__chandesUnregisterEvent__)(struct ChannelDescendant *, NvHandle, NvHandle, NvHandle, NvHandle);
    NV_STATUS (*__chandesControlSerialization_Prologue__)(struct ChannelDescendant *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__chandesCanCopy__)(struct ChannelDescendant *);
    void (*__chandesPreDestruct__)(struct ChannelDescendant *);
    NV_STATUS (*__chandesIsDuplicate__)(struct ChannelDescendant *, NvHandle, NvBool *);
    void (*__chandesControlSerialization_Epilogue__)(struct ChannelDescendant *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    PEVENTNOTIFICATION *(*__chandesGetNotificationListPtr__)(struct ChannelDescendant *);
    struct NotifShare *(*__chandesGetNotificationShare__)(struct ChannelDescendant *);
    NV_STATUS (*__chandesMap__)(struct ChannelDescendant *, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);
    NvBool (*__chandesAccessCallback__)(struct ChannelDescendant *, struct RsClient *, void *, RsAccessRight);
    struct KernelChannel *pKernelChannel;
    GPU_RESOURCE_DESC resourceDesc;
    NvU16 classID;
    NvU32 notifyAction;
    NvBool bNotifyTrigger;
};

#ifndef __NVOC_CLASS_ChannelDescendant_TYPEDEF__
#define __NVOC_CLASS_ChannelDescendant_TYPEDEF__
typedef struct ChannelDescendant ChannelDescendant;
#endif /* __NVOC_CLASS_ChannelDescendant_TYPEDEF__ */

#ifndef __nvoc_class_id_ChannelDescendant
#define __nvoc_class_id_ChannelDescendant 0x43d7c4
#endif /* __nvoc_class_id_ChannelDescendant */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_ChannelDescendant;

#define __staticCast_ChannelDescendant(pThis) \
    ((pThis)->__nvoc_pbase_ChannelDescendant)

#ifdef __nvoc_channel_descendant_h_disabled
#define __dynamicCast_ChannelDescendant(pThis) ((ChannelDescendant*)NULL)
#else //__nvoc_channel_descendant_h_disabled
#define __dynamicCast_ChannelDescendant(pThis) \
    ((ChannelDescendant*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(ChannelDescendant)))
#endif //__nvoc_channel_descendant_h_disabled


NV_STATUS __nvoc_objCreateDynamic_ChannelDescendant(ChannelDescendant**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_ChannelDescendant(ChannelDescendant**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams, PARAM_TO_ENGDESC_FUNCTION * arg_pParamToEngDescFn);
#define __objCreate_ChannelDescendant(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams, arg_pParamToEngDescFn) \
    __nvoc_objCreate_ChannelDescendant((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams, arg_pParamToEngDescFn)

#define chandesGetSwMethods(pChannelDescendant, ppMethods, pNumMethods) chandesGetSwMethods_DISPATCH(pChannelDescendant, ppMethods, pNumMethods)
#define chandesIsSwMethodStalling(pChannelDescendant, hHandle) chandesIsSwMethodStalling_DISPATCH(pChannelDescendant, hHandle)
#define chandesCheckMemInterUnmap(pChannelDescendant, bSubdeviceHandleProvided) chandesCheckMemInterUnmap_DISPATCH(pChannelDescendant, bSubdeviceHandleProvided)
#define chandesShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) chandesShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define chandesGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) chandesGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)
#define chandesMapTo(pResource, pParams) chandesMapTo_DISPATCH(pResource, pParams)
#define chandesGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) chandesGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define chandesSetNotificationShare(pNotifier, pNotifShare) chandesSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define chandesGetRefCount(pResource) chandesGetRefCount_DISPATCH(pResource)
#define chandesAddAdditionalDependants(pClient, pResource, pReference) chandesAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define chandesControl_Prologue(pResource, pCallContext, pParams) chandesControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define chandesGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) chandesGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define chandesInternalControlForward(pGpuResource, command, pParams, size) chandesInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define chandesUnmapFrom(pResource, pParams) chandesUnmapFrom_DISPATCH(pResource, pParams)
#define chandesControl_Epilogue(pResource, pCallContext, pParams) chandesControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define chandesControlLookup(pResource, pParams, ppEntry) chandesControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define chandesGetInternalObjectHandle(pGpuResource) chandesGetInternalObjectHandle_DISPATCH(pGpuResource)
#define chandesControl(pGpuResource, pCallContext, pParams) chandesControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define chandesUnmap(pGpuResource, pCallContext, pCpuMapping) chandesUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define chandesGetMemInterMapParams(pRmResource, pParams) chandesGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define chandesGetMemoryMappingDescriptor(pRmResource, ppMemDesc) chandesGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define chandesControlFilter(pResource, pCallContext, pParams) chandesControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define chandesUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) chandesUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define chandesControlSerialization_Prologue(pResource, pCallContext, pParams) chandesControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define chandesCanCopy(pResource) chandesCanCopy_DISPATCH(pResource)
#define chandesPreDestruct(pResource) chandesPreDestruct_DISPATCH(pResource)
#define chandesIsDuplicate(pResource, hMemory, pDuplicate) chandesIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define chandesControlSerialization_Epilogue(pResource, pCallContext, pParams) chandesControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define chandesGetNotificationListPtr(pNotifier) chandesGetNotificationListPtr_DISPATCH(pNotifier)
#define chandesGetNotificationShare(pNotifier) chandesGetNotificationShare_DISPATCH(pNotifier)
#define chandesMap(pGpuResource, pCallContext, pParams, pCpuMapping) chandesMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define chandesAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) chandesAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
static inline void chandesIsolateOnDestruct_b3696a(struct ChannelDescendant *pChannelDescendant) {
    return;
}


#ifdef __nvoc_channel_descendant_h_disabled
static inline void chandesIsolateOnDestruct(struct ChannelDescendant *pChannelDescendant) {
    NV_ASSERT_FAILED_PRECOMP("ChannelDescendant was disabled!");
}
#else //__nvoc_channel_descendant_h_disabled
#define chandesIsolateOnDestruct(pChannelDescendant) chandesIsolateOnDestruct_b3696a(pChannelDescendant)
#endif //__nvoc_channel_descendant_h_disabled

#define chandesIsolateOnDestruct_HAL(pChannelDescendant) chandesIsolateOnDestruct(pChannelDescendant)

static inline void chandesDestroy_b3696a(struct ChannelDescendant *pChannelDescendant) {
    return;
}


#ifdef __nvoc_channel_descendant_h_disabled
static inline void chandesDestroy(struct ChannelDescendant *pChannelDescendant) {
    NV_ASSERT_FAILED_PRECOMP("ChannelDescendant was disabled!");
}
#else //__nvoc_channel_descendant_h_disabled
#define chandesDestroy(pChannelDescendant) chandesDestroy_b3696a(pChannelDescendant)
#endif //__nvoc_channel_descendant_h_disabled

#define chandesDestroy_HAL(pChannelDescendant) chandesDestroy(pChannelDescendant)

NV_STATUS chandesGetSwMethods_IMPL(struct ChannelDescendant *pChannelDescendant, const METHOD **ppMethods, NvU32 *pNumMethods);

static inline NV_STATUS chandesGetSwMethods_DISPATCH(struct ChannelDescendant *pChannelDescendant, const METHOD **ppMethods, NvU32 *pNumMethods) {
    return pChannelDescendant->__chandesGetSwMethods__(pChannelDescendant, ppMethods, pNumMethods);
}

NvBool chandesIsSwMethodStalling_IMPL(struct ChannelDescendant *pChannelDescendant, NvU32 hHandle);

static inline NvBool chandesIsSwMethodStalling_DISPATCH(struct ChannelDescendant *pChannelDescendant, NvU32 hHandle) {
    return pChannelDescendant->__chandesIsSwMethodStalling__(pChannelDescendant, hHandle);
}

NV_STATUS chandesCheckMemInterUnmap_IMPL(struct ChannelDescendant *pChannelDescendant, NvBool bSubdeviceHandleProvided);

static inline NV_STATUS chandesCheckMemInterUnmap_DISPATCH(struct ChannelDescendant *pChannelDescendant, NvBool bSubdeviceHandleProvided) {
    return pChannelDescendant->__chandesCheckMemInterUnmap__(pChannelDescendant, bSubdeviceHandleProvided);
}

static inline NvBool chandesShareCallback_DISPATCH(struct ChannelDescendant *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__chandesShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS chandesGetOrAllocNotifShare_DISPATCH(struct ChannelDescendant *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__chandesGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

static inline NV_STATUS chandesMapTo_DISPATCH(struct ChannelDescendant *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__chandesMapTo__(pResource, pParams);
}

static inline NV_STATUS chandesGetMapAddrSpace_DISPATCH(struct ChannelDescendant *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__chandesGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline void chandesSetNotificationShare_DISPATCH(struct ChannelDescendant *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__chandesSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NvU32 chandesGetRefCount_DISPATCH(struct ChannelDescendant *pResource) {
    return pResource->__chandesGetRefCount__(pResource);
}

static inline void chandesAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct ChannelDescendant *pResource, RsResourceRef *pReference) {
    pResource->__chandesAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS chandesControl_Prologue_DISPATCH(struct ChannelDescendant *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__chandesControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS chandesGetRegBaseOffsetAndSize_DISPATCH(struct ChannelDescendant *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__chandesGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS chandesInternalControlForward_DISPATCH(struct ChannelDescendant *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__chandesInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NV_STATUS chandesUnmapFrom_DISPATCH(struct ChannelDescendant *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__chandesUnmapFrom__(pResource, pParams);
}

static inline void chandesControl_Epilogue_DISPATCH(struct ChannelDescendant *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__chandesControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS chandesControlLookup_DISPATCH(struct ChannelDescendant *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__chandesControlLookup__(pResource, pParams, ppEntry);
}

static inline NvHandle chandesGetInternalObjectHandle_DISPATCH(struct ChannelDescendant *pGpuResource) {
    return pGpuResource->__chandesGetInternalObjectHandle__(pGpuResource);
}

static inline NV_STATUS chandesControl_DISPATCH(struct ChannelDescendant *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__chandesControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS chandesUnmap_DISPATCH(struct ChannelDescendant *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__chandesUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NV_STATUS chandesGetMemInterMapParams_DISPATCH(struct ChannelDescendant *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__chandesGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS chandesGetMemoryMappingDescriptor_DISPATCH(struct ChannelDescendant *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__chandesGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS chandesControlFilter_DISPATCH(struct ChannelDescendant *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__chandesControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS chandesUnregisterEvent_DISPATCH(struct ChannelDescendant *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__chandesUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS chandesControlSerialization_Prologue_DISPATCH(struct ChannelDescendant *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__chandesControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool chandesCanCopy_DISPATCH(struct ChannelDescendant *pResource) {
    return pResource->__chandesCanCopy__(pResource);
}

static inline void chandesPreDestruct_DISPATCH(struct ChannelDescendant *pResource) {
    pResource->__chandesPreDestruct__(pResource);
}

static inline NV_STATUS chandesIsDuplicate_DISPATCH(struct ChannelDescendant *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__chandesIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void chandesControlSerialization_Epilogue_DISPATCH(struct ChannelDescendant *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__chandesControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline PEVENTNOTIFICATION *chandesGetNotificationListPtr_DISPATCH(struct ChannelDescendant *pNotifier) {
    return pNotifier->__chandesGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare *chandesGetNotificationShare_DISPATCH(struct ChannelDescendant *pNotifier) {
    return pNotifier->__chandesGetNotificationShare__(pNotifier);
}

static inline NV_STATUS chandesMap_DISPATCH(struct ChannelDescendant *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__chandesMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NvBool chandesAccessCallback_DISPATCH(struct ChannelDescendant *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__chandesAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

NV_STATUS chandesConstruct_IMPL(struct ChannelDescendant *arg_pChannelDescendant, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams, PARAM_TO_ENGDESC_FUNCTION *arg_pParamToEngDescFn);

#define __nvoc_chandesConstruct(arg_pChannelDescendant, arg_pCallContext, arg_pParams, arg_pParamToEngDescFn) chandesConstruct_IMPL(arg_pChannelDescendant, arg_pCallContext, arg_pParams, arg_pParamToEngDescFn)
void chandesDestruct_IMPL(struct ChannelDescendant *pChannelDescendant);

#define __nvoc_chandesDestruct(pChannelDescendant) chandesDestruct_IMPL(pChannelDescendant)
#undef PRIVATE_FIELD


//---------------------------------------------------------------------------
//
//  Method prototypes.
//
//---------------------------------------------------------------------------

NV_STATUS mthdNoOperation(OBJGPU *, struct ChannelDescendant *, NvU32, NvU32);

#endif // _CHANNEL_DESCENDANT_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_CHANNEL_DESCENDANT_NVOC_H_
