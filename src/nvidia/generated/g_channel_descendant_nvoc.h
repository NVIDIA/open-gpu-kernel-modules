
#ifndef _G_CHANNEL_DESCENDANT_NVOC_H_
#define _G_CHANNEL_DESCENDANT_NVOC_H_
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

#pragma once
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

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_CHANNEL_DESCENDANT_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct ChannelDescendant {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct GpuResource __nvoc_base_GpuResource;
    struct Notifier __nvoc_base_Notifier;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^4
    struct RsResource *__nvoc_pbase_RsResource;    // res super^3
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^3
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super^2
    struct GpuResource *__nvoc_pbase_GpuResource;    // gpures super
    struct INotifier *__nvoc_pbase_INotifier;    // inotify super^2
    struct Notifier *__nvoc_pbase_Notifier;    // notify super
    struct ChannelDescendant *__nvoc_pbase_ChannelDescendant;    // chandes

    // Vtable with 32 per-object function pointers
    NV_STATUS (*__chandesGetSwMethods__)(struct ChannelDescendant * /*this*/, const METHOD **, NvU32 *);  // virtual
    NvBool (*__chandesIsSwMethodStalling__)(struct ChannelDescendant * /*this*/, NvU32);  // virtual
    NV_STATUS (*__chandesCheckMemInterUnmap__)(struct ChannelDescendant * /*this*/, NvBool);  // virtual override (rmres) base (gpures)
    NV_STATUS (*__chandesControl__)(struct ChannelDescendant * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__chandesMap__)(struct ChannelDescendant * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__chandesUnmap__)(struct ChannelDescendant * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NvBool (*__chandesShareCallback__)(struct ChannelDescendant * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__chandesGetRegBaseOffsetAndSize__)(struct ChannelDescendant * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__chandesGetMapAddrSpace__)(struct ChannelDescendant * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__chandesInternalControlForward__)(struct ChannelDescendant * /*this*/, NvU32, void *, NvU32);  // virtual inherited (gpures) base (gpures)
    NvHandle (*__chandesGetInternalObjectHandle__)(struct ChannelDescendant * /*this*/);  // virtual inherited (gpures) base (gpures)
    NvBool (*__chandesAccessCallback__)(struct ChannelDescendant * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__chandesGetMemInterMapParams__)(struct ChannelDescendant * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__chandesGetMemoryMappingDescriptor__)(struct ChannelDescendant * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__chandesControlSerialization_Prologue__)(struct ChannelDescendant * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__chandesControlSerialization_Epilogue__)(struct ChannelDescendant * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__chandesControl_Prologue__)(struct ChannelDescendant * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__chandesControl_Epilogue__)(struct ChannelDescendant * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NvBool (*__chandesCanCopy__)(struct ChannelDescendant * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__chandesIsDuplicate__)(struct ChannelDescendant * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (gpures)
    void (*__chandesPreDestruct__)(struct ChannelDescendant * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__chandesControlFilter__)(struct ChannelDescendant * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (gpures)
    NvBool (*__chandesIsPartialUnmapSupported__)(struct ChannelDescendant * /*this*/);  // inline virtual inherited (res) base (gpures) body
    NV_STATUS (*__chandesMapTo__)(struct ChannelDescendant * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__chandesUnmapFrom__)(struct ChannelDescendant * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (gpures)
    NvU32 (*__chandesGetRefCount__)(struct ChannelDescendant * /*this*/);  // virtual inherited (res) base (gpures)
    void (*__chandesAddAdditionalDependants__)(struct RsClient *, struct ChannelDescendant * /*this*/, RsResourceRef *);  // virtual inherited (res) base (gpures)
    PEVENTNOTIFICATION * (*__chandesGetNotificationListPtr__)(struct ChannelDescendant * /*this*/);  // virtual inherited (notify) base (notify)
    struct NotifShare * (*__chandesGetNotificationShare__)(struct ChannelDescendant * /*this*/);  // virtual inherited (notify) base (notify)
    void (*__chandesSetNotificationShare__)(struct ChannelDescendant * /*this*/, struct NotifShare *);  // virtual inherited (notify) base (notify)
    NV_STATUS (*__chandesUnregisterEvent__)(struct ChannelDescendant * /*this*/, NvHandle, NvHandle, NvHandle, NvHandle);  // virtual inherited (notify) base (notify)
    NV_STATUS (*__chandesGetOrAllocNotifShare__)(struct ChannelDescendant * /*this*/, NvHandle, NvHandle, struct NotifShare **);  // virtual inherited (notify) base (notify)

    // Data members
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

// Casting support
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


// Wrapper macros
#define chandesGetSwMethods_FNPTR(pChannelDescendant) pChannelDescendant->__chandesGetSwMethods__
#define chandesGetSwMethods(pChannelDescendant, ppMethods, pNumMethods) chandesGetSwMethods_DISPATCH(pChannelDescendant, ppMethods, pNumMethods)
#define chandesIsSwMethodStalling_FNPTR(pChannelDescendant) pChannelDescendant->__chandesIsSwMethodStalling__
#define chandesIsSwMethodStalling(pChannelDescendant, hHandle) chandesIsSwMethodStalling_DISPATCH(pChannelDescendant, hHandle)
#define chandesCheckMemInterUnmap_FNPTR(pChannelDescendant) pChannelDescendant->__chandesCheckMemInterUnmap__
#define chandesCheckMemInterUnmap(pChannelDescendant, bSubdeviceHandleProvided) chandesCheckMemInterUnmap_DISPATCH(pChannelDescendant, bSubdeviceHandleProvided)
#define chandesControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresControl__
#define chandesControl(pGpuResource, pCallContext, pParams) chandesControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define chandesMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresMap__
#define chandesMap(pGpuResource, pCallContext, pParams, pCpuMapping) chandesMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define chandesUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresUnmap__
#define chandesUnmap(pGpuResource, pCallContext, pCpuMapping) chandesUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define chandesShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresShareCallback__
#define chandesShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) chandesShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define chandesGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetRegBaseOffsetAndSize__
#define chandesGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) chandesGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define chandesGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetMapAddrSpace__
#define chandesGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) chandesGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define chandesInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresInternalControlForward__
#define chandesInternalControlForward(pGpuResource, command, pParams, size) chandesInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define chandesGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetInternalObjectHandle__
#define chandesGetInternalObjectHandle(pGpuResource) chandesGetInternalObjectHandle_DISPATCH(pGpuResource)
#define chandesAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresAccessCallback__
#define chandesAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) chandesAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define chandesGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemInterMapParams__
#define chandesGetMemInterMapParams(pRmResource, pParams) chandesGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define chandesGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemoryMappingDescriptor__
#define chandesGetMemoryMappingDescriptor(pRmResource, ppMemDesc) chandesGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define chandesControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Prologue__
#define chandesControlSerialization_Prologue(pResource, pCallContext, pParams) chandesControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define chandesControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Epilogue__
#define chandesControlSerialization_Epilogue(pResource, pCallContext, pParams) chandesControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define chandesControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Prologue__
#define chandesControl_Prologue(pResource, pCallContext, pParams) chandesControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define chandesControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Epilogue__
#define chandesControl_Epilogue(pResource, pCallContext, pParams) chandesControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define chandesCanCopy_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__
#define chandesCanCopy(pResource) chandesCanCopy_DISPATCH(pResource)
#define chandesIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsDuplicate__
#define chandesIsDuplicate(pResource, hMemory, pDuplicate) chandesIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define chandesPreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resPreDestruct__
#define chandesPreDestruct(pResource) chandesPreDestruct_DISPATCH(pResource)
#define chandesControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resControlFilter__
#define chandesControlFilter(pResource, pCallContext, pParams) chandesControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define chandesIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsPartialUnmapSupported__
#define chandesIsPartialUnmapSupported(pResource) chandesIsPartialUnmapSupported_DISPATCH(pResource)
#define chandesMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resMapTo__
#define chandesMapTo(pResource, pParams) chandesMapTo_DISPATCH(pResource, pParams)
#define chandesUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmapFrom__
#define chandesUnmapFrom(pResource, pParams) chandesUnmapFrom_DISPATCH(pResource, pParams)
#define chandesGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resGetRefCount__
#define chandesGetRefCount(pResource) chandesGetRefCount_DISPATCH(pResource)
#define chandesAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resAddAdditionalDependants__
#define chandesAddAdditionalDependants(pClient, pResource, pReference) chandesAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define chandesGetNotificationListPtr_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__notifyGetNotificationListPtr__
#define chandesGetNotificationListPtr(pNotifier) chandesGetNotificationListPtr_DISPATCH(pNotifier)
#define chandesGetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__notifyGetNotificationShare__
#define chandesGetNotificationShare(pNotifier) chandesGetNotificationShare_DISPATCH(pNotifier)
#define chandesSetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__notifySetNotificationShare__
#define chandesSetNotificationShare(pNotifier, pNotifShare) chandesSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define chandesUnregisterEvent_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__notifyUnregisterEvent__
#define chandesUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) chandesUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define chandesGetOrAllocNotifShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__notifyGetOrAllocNotifShare__
#define chandesGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) chandesGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)

// Dispatch functions
static inline NV_STATUS chandesGetSwMethods_DISPATCH(struct ChannelDescendant *pChannelDescendant, const METHOD **ppMethods, NvU32 *pNumMethods) {
    return pChannelDescendant->__chandesGetSwMethods__(pChannelDescendant, ppMethods, pNumMethods);
}

static inline NvBool chandesIsSwMethodStalling_DISPATCH(struct ChannelDescendant *pChannelDescendant, NvU32 hHandle) {
    return pChannelDescendant->__chandesIsSwMethodStalling__(pChannelDescendant, hHandle);
}

static inline NV_STATUS chandesCheckMemInterUnmap_DISPATCH(struct ChannelDescendant *pChannelDescendant, NvBool bSubdeviceHandleProvided) {
    return pChannelDescendant->__chandesCheckMemInterUnmap__(pChannelDescendant, bSubdeviceHandleProvided);
}

static inline NV_STATUS chandesControl_DISPATCH(struct ChannelDescendant *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__chandesControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS chandesMap_DISPATCH(struct ChannelDescendant *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__chandesMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS chandesUnmap_DISPATCH(struct ChannelDescendant *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__chandesUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NvBool chandesShareCallback_DISPATCH(struct ChannelDescendant *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__chandesShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS chandesGetRegBaseOffsetAndSize_DISPATCH(struct ChannelDescendant *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__chandesGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS chandesGetMapAddrSpace_DISPATCH(struct ChannelDescendant *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__chandesGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS chandesInternalControlForward_DISPATCH(struct ChannelDescendant *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__chandesInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NvHandle chandesGetInternalObjectHandle_DISPATCH(struct ChannelDescendant *pGpuResource) {
    return pGpuResource->__chandesGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool chandesAccessCallback_DISPATCH(struct ChannelDescendant *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__chandesAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS chandesGetMemInterMapParams_DISPATCH(struct ChannelDescendant *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__chandesGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS chandesGetMemoryMappingDescriptor_DISPATCH(struct ChannelDescendant *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__chandesGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS chandesControlSerialization_Prologue_DISPATCH(struct ChannelDescendant *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__chandesControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void chandesControlSerialization_Epilogue_DISPATCH(struct ChannelDescendant *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__chandesControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS chandesControl_Prologue_DISPATCH(struct ChannelDescendant *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__chandesControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void chandesControl_Epilogue_DISPATCH(struct ChannelDescendant *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__chandesControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool chandesCanCopy_DISPATCH(struct ChannelDescendant *pResource) {
    return pResource->__chandesCanCopy__(pResource);
}

static inline NV_STATUS chandesIsDuplicate_DISPATCH(struct ChannelDescendant *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__chandesIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void chandesPreDestruct_DISPATCH(struct ChannelDescendant *pResource) {
    pResource->__chandesPreDestruct__(pResource);
}

static inline NV_STATUS chandesControlFilter_DISPATCH(struct ChannelDescendant *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__chandesControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool chandesIsPartialUnmapSupported_DISPATCH(struct ChannelDescendant *pResource) {
    return pResource->__chandesIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS chandesMapTo_DISPATCH(struct ChannelDescendant *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__chandesMapTo__(pResource, pParams);
}

static inline NV_STATUS chandesUnmapFrom_DISPATCH(struct ChannelDescendant *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__chandesUnmapFrom__(pResource, pParams);
}

static inline NvU32 chandesGetRefCount_DISPATCH(struct ChannelDescendant *pResource) {
    return pResource->__chandesGetRefCount__(pResource);
}

static inline void chandesAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct ChannelDescendant *pResource, RsResourceRef *pReference) {
    pResource->__chandesAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline PEVENTNOTIFICATION * chandesGetNotificationListPtr_DISPATCH(struct ChannelDescendant *pNotifier) {
    return pNotifier->__chandesGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare * chandesGetNotificationShare_DISPATCH(struct ChannelDescendant *pNotifier) {
    return pNotifier->__chandesGetNotificationShare__(pNotifier);
}

static inline void chandesSetNotificationShare_DISPATCH(struct ChannelDescendant *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__chandesSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NV_STATUS chandesUnregisterEvent_DISPATCH(struct ChannelDescendant *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__chandesUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS chandesGetOrAllocNotifShare_DISPATCH(struct ChannelDescendant *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__chandesGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

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

NvBool chandesIsSwMethodStalling_IMPL(struct ChannelDescendant *pChannelDescendant, NvU32 hHandle);

NV_STATUS chandesCheckMemInterUnmap_IMPL(struct ChannelDescendant *pChannelDescendant, NvBool bSubdeviceHandleProvided);

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
