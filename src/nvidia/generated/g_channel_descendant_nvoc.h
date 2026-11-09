
#ifndef _G_CHANNEL_DESCENDANT_NVOC_H_
#define _G_CHANNEL_DESCENDANT_NVOC_H_

// Version of generated metadata structures
#ifdef NVOC_METADATA_VERSION
#undef NVOC_METADATA_VERSION
#endif
#define NVOC_METADATA_VERSION 2

#include "nvoc/runtime.h"
#include "nvoc/rtti.h"

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
#include "rmapi/event_api.h"

#include "containers/btree.h"
#include "resserv/rs_resource.h"
#include "gpu/gpu_resource.h"
#include "gpu/gpu_resource_desc.h"
#include "kernel/gpu/gpu_halspec.h"

struct ChannelDescendant;

#ifndef __nvoc_class_id_ChannelDescendant
#define __nvoc_class_id_ChannelDescendant 0x43d7c4u
typedef struct ChannelDescendant ChannelDescendant;
#endif /* __nvoc_class_id_ChannelDescendant */


struct ContextDma;

#ifndef __nvoc_class_id_ContextDma
#define __nvoc_class_id_ContextDma 0x88441bu
typedef struct ContextDma ContextDma;
#endif /* __nvoc_class_id_ContextDma */


struct KernelChannel;

#ifndef __nvoc_class_id_KernelChannel
#define __nvoc_class_id_KernelChannel 0x5d8d70u
typedef struct KernelChannel KernelChannel;
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


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__ChannelDescendant;
struct NVOC_METADATA__GpuResource;
struct NVOC_METADATA__Notifier;
struct NVOC_VTABLE__ChannelDescendant;


struct ChannelDescendant {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__ChannelDescendant *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
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

    // Data members
    struct KernelChannel *pKernelChannel;
    GPU_RESOURCE_DESC resourceDesc;
    NvU16 classID;
    NvU32 notifyAction;
    NvBool bNotifyTrigger;
};


// Vtable with 2 per-class function pointers
struct NVOC_VTABLE__ChannelDescendant {
    NV_STATUS (*__chandesGetSwMethods__)(struct ChannelDescendant * /*this*/, const METHOD **, NvU32 *);  // virtual
    NvBool (*__chandesIsSwMethodStalling__)(struct ChannelDescendant * /*this*/, NvU32);  // virtual
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__ChannelDescendant {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResource metadata__GpuResource;
    const struct NVOC_METADATA__Notifier metadata__Notifier;
    const struct NVOC_VTABLE__ChannelDescendant vtable;
};

#ifndef __nvoc_class_id_ChannelDescendant
#define __nvoc_class_id_ChannelDescendant 0x43d7c4u
typedef struct ChannelDescendant ChannelDescendant;
#endif /* __nvoc_class_id_ChannelDescendant */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_ChannelDescendant;

#define __staticCast_ChannelDescendant(pThis) \
    ((pThis)->__nvoc_pbase_ChannelDescendant)

#ifdef __nvoc_channel_descendant_h_disabled
#define __dynamicCast_ChannelDescendant(pThis) ((ChannelDescendant*) NULL)
#else //__nvoc_channel_descendant_h_disabled
#define __dynamicCast_ChannelDescendant(pThis) \
    ((ChannelDescendant*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(ChannelDescendant)))
#endif //__nvoc_channel_descendant_h_disabled

NV_STATUS __nvoc_objCreateDynamic_ChannelDescendant(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_ChannelDescendant(ChannelDescendant**, Dynamic*, NvU32, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams, PARAM_TO_ENGDESC_FUNCTION *pParamToEngDescFn);
#define __objCreate_ChannelDescendant(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams, pParamToEngDescFn) \
    __nvoc_objCreate_ChannelDescendant((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pCallContext, pParams, pParamToEngDescFn)


// Wrapper macros for implementation functions
NV_STATUS chandesConstruct_IMPL(struct ChannelDescendant *pChannelDescendant, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams, PARAM_TO_ENGDESC_FUNCTION *pParamToEngDescFn);
#define __nvoc_chandesConstruct(pChannelDescendant, pCallContext, pParams, pParamToEngDescFn) chandesConstruct_IMPL(pChannelDescendant, pCallContext, pParams, pParamToEngDescFn)

void chandesDestruct_IMPL(struct ChannelDescendant *pChannelDescendant);
#define __nvoc_chandesDestruct(pChannelDescendant) chandesDestruct_IMPL(pChannelDescendant)

#ifdef __nvoc_channel_descendant_h_disabled
static inline void chandesIsolateOnDestruct(struct ChannelDescendant *pChannelDescendant) {
    NV_ASSERT_FAILED_PRECOMP("ChannelDescendant was disabled!");
}
#else // __nvoc_channel_descendant_h_disabled
#define chandesIsolateOnDestruct(pChannelDescendant) chandesIsolateOnDestruct_d44104(pChannelDescendant)
#endif // __nvoc_channel_descendant_h_disabled

#ifdef __nvoc_channel_descendant_h_disabled
static inline void chandesDestroy(struct ChannelDescendant *pChannelDescendant) {
    NV_ASSERT_FAILED_PRECOMP("ChannelDescendant was disabled!");
}
#else // __nvoc_channel_descendant_h_disabled
#define chandesDestroy(pChannelDescendant) chandesDestroy_d44104(pChannelDescendant)
#endif // __nvoc_channel_descendant_h_disabled


// Wrapper macros for halified functions
#define chandesIsolateOnDestruct_HAL(pChannelDescendant) chandesIsolateOnDestruct(pChannelDescendant)
#define chandesDestroy_HAL(pChannelDescendant) chandesDestroy(pChannelDescendant)
#define chandesGetSwMethods_FNPTR(pChannelDescendant) pChannelDescendant->__nvoc_metadata_ptr->vtable.__chandesGetSwMethods__
#define chandesGetSwMethods(pChannelDescendant, ppMethods, pNumMethods) chandesGetSwMethods_DISPATCH(pChannelDescendant, ppMethods, pNumMethods)
#define chandesIsSwMethodStalling_FNPTR(pChannelDescendant) pChannelDescendant->__nvoc_metadata_ptr->vtable.__chandesIsSwMethodStalling__
#define chandesIsSwMethodStalling(pChannelDescendant, hHandle) chandesIsSwMethodStalling_DISPATCH(pChannelDescendant, hHandle)
#define chandesCheckMemInterUnmap_FNPTR(pChannelDescendant) pChannelDescendant->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define chandesCheckMemInterUnmap(pChannelDescendant, bSubdeviceHandleProvided) chandesCheckMemInterUnmap_DISPATCH(pChannelDescendant, bSubdeviceHandleProvided)
#define chandesControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__
#define chandesControl(pGpuResource, pCallContext, pParams) chandesControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define chandesMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__
#define chandesMap(pGpuResource, pCallContext, pParams, pCpuMapping) chandesMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define chandesUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__
#define chandesUnmap(pGpuResource, pCallContext, pCpuMapping) chandesUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define chandesShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__
#define chandesShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) chandesShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define chandesGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__
#define chandesGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) chandesGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define chandesGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__
#define chandesGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) chandesGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define chandesInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__
#define chandesInternalControlForward(pGpuResource, command, pParams, size) chandesInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define chandesGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__
#define chandesGetInternalObjectHandle(pGpuResource) chandesGetInternalObjectHandle_DISPATCH(pGpuResource)
#define chandesAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__
#define chandesAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) chandesAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define chandesGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define chandesGetMemInterMapParams(pRmResource, pParams) chandesGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define chandesGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define chandesGetMemoryMappingDescriptor(pRmResource, ppMemDesc) chandesGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define chandesControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__
#define chandesControlSerialization_Prologue(pResource, pCallContext, pParams) chandesControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define chandesControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__
#define chandesControlSerialization_Epilogue(pResource, pCallContext, pParams) chandesControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define chandesControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__
#define chandesControl_Prologue(pResource, pCallContext, pParams) chandesControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define chandesControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__
#define chandesControl_Epilogue(pResource, pCallContext, pParams) chandesControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define chandesCanCopy_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define chandesCanCopy(pResource) chandesCanCopy_DISPATCH(pResource)
#define chandesIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define chandesIsDuplicate(pResource, hMemory, pDuplicate) chandesIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define chandesPreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define chandesPreDestruct(pResource) chandesPreDestruct_DISPATCH(pResource)
#define chandesControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define chandesControlFilter(pResource, pCallContext, pParams) chandesControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define chandesIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define chandesIsPartialUnmapSupported(pResource) chandesIsPartialUnmapSupported_DISPATCH(pResource)
#define chandesMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define chandesMapTo(pResource, pParams) chandesMapTo_DISPATCH(pResource, pParams)
#define chandesUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define chandesUnmapFrom(pResource, pParams) chandesUnmapFrom_DISPATCH(pResource, pParams)
#define chandesGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define chandesGetRefCount(pResource) chandesGetRefCount_DISPATCH(pResource)
#define chandesAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define chandesAddAdditionalDependants(pClient, pResource, pReference) chandesAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define chandesGetNotificationListPtr_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetNotificationListPtr__
#define chandesGetNotificationListPtr(pNotifier) chandesGetNotificationListPtr_DISPATCH(pNotifier)
#define chandesGetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetNotificationShare__
#define chandesGetNotificationShare(pNotifier) chandesGetNotificationShare_DISPATCH(pNotifier)
#define chandesSetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifySetNotificationShare__
#define chandesSetNotificationShare(pNotifier, pNotifShare) chandesSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define chandesUnregisterEvent_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyUnregisterEvent__
#define chandesUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) chandesUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define chandesGetOrAllocNotifShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetOrAllocNotifShare__
#define chandesGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) chandesGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)

// Dispatch functions
static inline NV_STATUS chandesGetSwMethods_DISPATCH(struct ChannelDescendant *pChannelDescendant, const METHOD **ppMethods, NvU32 *pNumMethods) {
    return pChannelDescendant->__nvoc_metadata_ptr->vtable.__chandesGetSwMethods__(pChannelDescendant, ppMethods, pNumMethods);
}

static inline NvBool chandesIsSwMethodStalling_DISPATCH(struct ChannelDescendant *pChannelDescendant, NvU32 hHandle) {
    return pChannelDescendant->__nvoc_metadata_ptr->vtable.__chandesIsSwMethodStalling__(pChannelDescendant, hHandle);
}

static inline NV_STATUS chandesCheckMemInterUnmap_DISPATCH(struct ChannelDescendant *pChannelDescendant, NvBool bSubdeviceHandleProvided) {
    return pChannelDescendant->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__(staticCast(pChannelDescendant, RmResource), bSubdeviceHandleProvided);
}

static inline NV_STATUS chandesControl_DISPATCH(struct ChannelDescendant *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__(staticCast(pGpuResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS chandesMap_DISPATCH(struct ChannelDescendant *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__(staticCast(pGpuResource, RsResource), pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS chandesUnmap_DISPATCH(struct ChannelDescendant *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__(staticCast(pGpuResource, RsResource), pCallContext, pCpuMapping);
}

static inline NvBool chandesShareCallback_DISPATCH(struct ChannelDescendant *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__(staticCast(pGpuResource, RsResource), pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS chandesGetRegBaseOffsetAndSize_DISPATCH(struct ChannelDescendant *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__(staticCast(pGpuResource, GpuResource), pGpu, pOffset, pSize);
}

static inline NV_STATUS chandesGetMapAddrSpace_DISPATCH(struct ChannelDescendant *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__(staticCast(pGpuResource, GpuResource), pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS chandesInternalControlForward_DISPATCH(struct ChannelDescendant *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__(staticCast(pGpuResource, GpuResource), command, pParams, size);
}

static inline NvHandle chandesGetInternalObjectHandle_DISPATCH(struct ChannelDescendant *pGpuResource) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__(staticCast(pGpuResource, GpuResource));
}

static inline NvBool chandesAccessCallback_DISPATCH(struct ChannelDescendant *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__(staticCast(pResource, RsResource), pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS chandesGetMemInterMapParams_DISPATCH(struct ChannelDescendant *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__(staticCast(pRmResource, RmResource), pParams);
}

static inline NV_STATUS chandesGetMemoryMappingDescriptor_DISPATCH(struct ChannelDescendant *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__(staticCast(pRmResource, RmResource), ppMemDesc);
}

static inline NV_STATUS chandesControlSerialization_Prologue_DISPATCH(struct ChannelDescendant *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void chandesControlSerialization_Epilogue_DISPATCH(struct ChannelDescendant *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS chandesControl_Prologue_DISPATCH(struct ChannelDescendant *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void chandesControl_Epilogue_DISPATCH(struct ChannelDescendant *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool chandesCanCopy_DISPATCH(struct ChannelDescendant *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__(staticCast(pResource, RsResource));
}

static inline NV_STATUS chandesIsDuplicate_DISPATCH(struct ChannelDescendant *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__(staticCast(pResource, RsResource), hMemory, pDuplicate);
}

static inline void chandesPreDestruct_DISPATCH(struct ChannelDescendant *pResource) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__(staticCast(pResource, RsResource));
}

static inline NV_STATUS chandesControlFilter_DISPATCH(struct ChannelDescendant *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool chandesIsPartialUnmapSupported_DISPATCH(struct ChannelDescendant *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__(staticCast(pResource, RsResource));
}

static inline NV_STATUS chandesMapTo_DISPATCH(struct ChannelDescendant *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__(staticCast(pResource, RsResource), pParams);
}

static inline NV_STATUS chandesUnmapFrom_DISPATCH(struct ChannelDescendant *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__(staticCast(pResource, RsResource), pParams);
}

static inline NvU32 chandesGetRefCount_DISPATCH(struct ChannelDescendant *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__(staticCast(pResource, RsResource));
}

static inline void chandesAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct ChannelDescendant *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__(pClient, staticCast(pResource, RsResource), pReference);
}

static inline PEVENTNOTIFICATION * chandesGetNotificationListPtr_DISPATCH(struct ChannelDescendant *pNotifier) {
    return pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetNotificationListPtr__(staticCast(pNotifier, INotifier));
}

static inline struct NotifShare * chandesGetNotificationShare_DISPATCH(struct ChannelDescendant *pNotifier) {
    return pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetNotificationShare__(staticCast(pNotifier, INotifier));
}

static inline void chandesSetNotificationShare_DISPATCH(struct ChannelDescendant *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifySetNotificationShare__(staticCast(pNotifier, INotifier), pNotifShare);
}

static inline NV_STATUS chandesUnregisterEvent_DISPATCH(struct ChannelDescendant *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyUnregisterEvent__(staticCast(pNotifier, INotifier), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS chandesGetOrAllocNotifShare_DISPATCH(struct ChannelDescendant *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetOrAllocNotifShare__(staticCast(pNotifier, INotifier), hNotifierClient, hNotifierResource, ppNotifShare);
}

// Virtual method declarations and/or inline definitions
NV_STATUS chandesGetSwMethods_IMPL(struct ChannelDescendant *pChannelDescendant, const METHOD **ppMethods, NvU32 *pNumMethods);

NvBool chandesIsSwMethodStalling_IMPL(struct ChannelDescendant *pChannelDescendant, NvU32 hHandle);

NV_STATUS chandesCheckMemInterUnmap_IMPL(struct ChannelDescendant *pChannelDescendant, NvBool bSubdeviceHandleProvided);

// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
static inline void chandesIsolateOnDestruct_d44104(struct ChannelDescendant *pChannelDescendant){
    return;
}

static inline void chandesDestroy_d44104(struct ChannelDescendant *pChannelDescendant){
    return;
}

// Static dispatch method declarations
// Static inline method definitions
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
