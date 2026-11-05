
#ifndef _G_DISP_CHANNEL_NVOC_H_
#define _G_DISP_CHANNEL_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 1993-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/******************************************************************************
*
*   Description:
*       This file contains functions managing DispChannel and its derived classes.
*
******************************************************************************/

#pragma once
#include "g_disp_channel_nvoc.h"

#ifndef DISP_CHANNEL_H
#define DISP_CHANNEL_H

#include "gpu/gpu_resource.h"
#include "rmapi/event_api.h"

struct ContextDma;

#ifndef __nvoc_class_id_ContextDma
#define __nvoc_class_id_ContextDma 0x88441bu
typedef struct ContextDma ContextDma;
#endif /* __nvoc_class_id_ContextDma */


struct NvDispApi;

#ifndef __nvoc_class_id_NvDispApi
#define __nvoc_class_id_NvDispApi 0x36aa0bu
typedef struct NvDispApi NvDispApi;
#endif /* __nvoc_class_id_NvDispApi */



/*!
 * Base class for display channels
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_DISP_CHANNEL_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__DispChannel;
struct NVOC_METADATA__GpuResource;
struct NVOC_METADATA__Notifier;


struct DispChannel {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__DispChannel *__nvoc_metadata_ptr;
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
    struct DispChannel *__nvoc_pbase_DispChannel;    // dispchn

    // Data members
    struct NvDispApi *pNvDispApi;
    NvU32 DispClass;
    NvU32 InstanceNumber;
    NvP64 pControl;
    NvP64 pPriv;
    NvU32 ControlOffset;
    NvU32 ControlLength;
    NvBool bIsDma;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__DispChannel {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResource metadata__GpuResource;
    const struct NVOC_METADATA__Notifier metadata__Notifier;
};

#ifndef __nvoc_class_id_DispChannel
#define __nvoc_class_id_DispChannel 0xbd2ff3u
typedef struct DispChannel DispChannel;
#endif /* __nvoc_class_id_DispChannel */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_DispChannel;

#define __staticCast_DispChannel(pThis) \
    ((pThis)->__nvoc_pbase_DispChannel)

#ifdef __nvoc_disp_channel_h_disabled
#define __dynamicCast_DispChannel(pThis) ((DispChannel*) NULL)
#else //__nvoc_disp_channel_h_disabled
#define __dynamicCast_DispChannel(pThis) \
    ((DispChannel*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(DispChannel)))
#endif //__nvoc_disp_channel_h_disabled

NV_STATUS __nvoc_objCreateDynamic_DispChannel(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_DispChannel(DispChannel**, Dynamic*, NvU32, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams, NvU32 isDma);
#define __objCreate_DispChannel(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams, isDma) \
    __nvoc_objCreate_DispChannel((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pCallContext, pParams, isDma)


// Wrapper macros for implementation functions
NV_STATUS dispchnConstruct_IMPL(struct DispChannel *pDispChannel, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams, NvU32 isDma);
#define __nvoc_dispchnConstruct(pDispChannel, pCallContext, pParams, isDma) dispchnConstruct_IMPL(pDispChannel, pCallContext, pParams, isDma)

void dispchnDestruct_IMPL(struct DispChannel *pDispChannel);
#define __nvoc_dispchnDestruct(pDispChannel) dispchnDestruct_IMPL(pDispChannel)

void dispchnSetRegBaseOffsetAndSize_IMPL(struct DispChannel *pDispChannel, struct OBJGPU *pGpu);
#ifdef __nvoc_disp_channel_h_disabled
static inline void dispchnSetRegBaseOffsetAndSize(struct DispChannel *pDispChannel, struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("DispChannel was disabled!");
}
#else // __nvoc_disp_channel_h_disabled
#define dispchnSetRegBaseOffsetAndSize(pDispChannel, pGpu) dispchnSetRegBaseOffsetAndSize_IMPL(pDispChannel, pGpu)
#endif // __nvoc_disp_channel_h_disabled

NV_STATUS dispchnGrabChannel_IMPL(struct DispChannel *pDispChannel, NvHandle hClient, NvHandle hParent, NvHandle hChannel, NvU32 hClass, void *pAllocParms);
#ifdef __nvoc_disp_channel_h_disabled
static inline NV_STATUS dispchnGrabChannel(struct DispChannel *pDispChannel, NvHandle hClient, NvHandle hParent, NvHandle hChannel, NvU32 hClass, void *pAllocParms) {
    NV_ASSERT_FAILED_PRECOMP("DispChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_disp_channel_h_disabled
#define dispchnGrabChannel(pDispChannel, hClient, hParent, hChannel, hClass, pAllocParms) dispchnGrabChannel_IMPL(pDispChannel, hClient, hParent, hChannel, hClass, pAllocParms)
#endif // __nvoc_disp_channel_h_disabled

NV_STATUS dispchnBindCtx_IMPL(struct OBJGPU *pGpu, struct ContextDma *pContextDma, NvHandle hDispChannel);
#define dispchnBindCtx(pGpu, pContextDma, hDispChannel) dispchnBindCtx_IMPL(pGpu, pContextDma, hDispChannel)

NV_STATUS dispchnUnbindCtx_IMPL(struct OBJGPU *pGpu, struct ContextDma *pContextDma, NvHandle hDispChannel);
#define dispchnUnbindCtx(pGpu, pContextDma, hDispChannel) dispchnUnbindCtx_IMPL(pGpu, pContextDma, hDispChannel)

void dispchnUnbindCtxFromAllChannels_IMPL(struct OBJGPU *pGpu, struct ContextDma *pContextDma);
#define dispchnUnbindCtxFromAllChannels(pGpu, pContextDma) dispchnUnbindCtxFromAllChannels_IMPL(pGpu, pContextDma)

void dispchnUnbindAllCtx_IMPL(struct OBJGPU *pGpu, struct DispChannel *pDispChannel);
#ifdef __nvoc_disp_channel_h_disabled
static inline void dispchnUnbindAllCtx(struct OBJGPU *pGpu, struct DispChannel *pDispChannel) {
    NV_ASSERT_FAILED_PRECOMP("DispChannel was disabled!");
}
#else // __nvoc_disp_channel_h_disabled
#define dispchnUnbindAllCtx(pGpu, pDispChannel) dispchnUnbindAllCtx_IMPL(pGpu, pDispChannel)
#endif // __nvoc_disp_channel_h_disabled

NV_STATUS dispchnGetByHandle_IMPL(struct RsClient *pClient, NvHandle hDisplayChannel, struct DispChannel **ppDispChannel);
#define dispchnGetByHandle(pClient, hDisplayChannel, ppDispChannel) dispchnGetByHandle_IMPL(pClient, hDisplayChannel, ppDispChannel)


// Wrapper macros for halified functions
#define dispchnGetRegBaseOffsetAndSize_FNPTR(pDispChannel) pDispChannel->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__
#define dispchnGetRegBaseOffsetAndSize(pDispChannel, pGpu, pOffset, pSize) dispchnGetRegBaseOffsetAndSize_DISPATCH(pDispChannel, pGpu, pOffset, pSize)
#define dispchnControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__
#define dispchnControl(pGpuResource, pCallContext, pParams) dispchnControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define dispchnMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__
#define dispchnMap(pGpuResource, pCallContext, pParams, pCpuMapping) dispchnMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define dispchnUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__
#define dispchnUnmap(pGpuResource, pCallContext, pCpuMapping) dispchnUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define dispchnShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__
#define dispchnShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) dispchnShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define dispchnGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__
#define dispchnGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) dispchnGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define dispchnInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__
#define dispchnInternalControlForward(pGpuResource, command, pParams, size) dispchnInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define dispchnGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__
#define dispchnGetInternalObjectHandle(pGpuResource) dispchnGetInternalObjectHandle_DISPATCH(pGpuResource)
#define dispchnAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__
#define dispchnAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) dispchnAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define dispchnGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define dispchnGetMemInterMapParams(pRmResource, pParams) dispchnGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define dispchnCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define dispchnCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) dispchnCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define dispchnGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define dispchnGetMemoryMappingDescriptor(pRmResource, ppMemDesc) dispchnGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define dispchnControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__
#define dispchnControlSerialization_Prologue(pResource, pCallContext, pParams) dispchnControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define dispchnControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__
#define dispchnControlSerialization_Epilogue(pResource, pCallContext, pParams) dispchnControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define dispchnControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__
#define dispchnControl_Prologue(pResource, pCallContext, pParams) dispchnControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define dispchnControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__
#define dispchnControl_Epilogue(pResource, pCallContext, pParams) dispchnControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define dispchnCanCopy_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define dispchnCanCopy(pResource) dispchnCanCopy_DISPATCH(pResource)
#define dispchnIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define dispchnIsDuplicate(pResource, hMemory, pDuplicate) dispchnIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define dispchnPreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define dispchnPreDestruct(pResource) dispchnPreDestruct_DISPATCH(pResource)
#define dispchnControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define dispchnControlFilter(pResource, pCallContext, pParams) dispchnControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define dispchnIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define dispchnIsPartialUnmapSupported(pResource) dispchnIsPartialUnmapSupported_DISPATCH(pResource)
#define dispchnMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define dispchnMapTo(pResource, pParams) dispchnMapTo_DISPATCH(pResource, pParams)
#define dispchnUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define dispchnUnmapFrom(pResource, pParams) dispchnUnmapFrom_DISPATCH(pResource, pParams)
#define dispchnGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define dispchnGetRefCount(pResource) dispchnGetRefCount_DISPATCH(pResource)
#define dispchnAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define dispchnAddAdditionalDependants(pClient, pResource, pReference) dispchnAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define dispchnGetNotificationListPtr_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetNotificationListPtr__
#define dispchnGetNotificationListPtr(pNotifier) dispchnGetNotificationListPtr_DISPATCH(pNotifier)
#define dispchnGetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetNotificationShare__
#define dispchnGetNotificationShare(pNotifier) dispchnGetNotificationShare_DISPATCH(pNotifier)
#define dispchnSetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifySetNotificationShare__
#define dispchnSetNotificationShare(pNotifier, pNotifShare) dispchnSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define dispchnUnregisterEvent_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyUnregisterEvent__
#define dispchnUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) dispchnUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define dispchnGetOrAllocNotifShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetOrAllocNotifShare__
#define dispchnGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) dispchnGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)

// Dispatch functions
static inline NV_STATUS dispchnGetRegBaseOffsetAndSize_DISPATCH(struct DispChannel *pDispChannel, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pDispChannel->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__(staticCast(pDispChannel, GpuResource), pGpu, pOffset, pSize);
}

static inline NV_STATUS dispchnControl_DISPATCH(struct DispChannel *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__(staticCast(pGpuResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS dispchnMap_DISPATCH(struct DispChannel *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__(staticCast(pGpuResource, RsResource), pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS dispchnUnmap_DISPATCH(struct DispChannel *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__(staticCast(pGpuResource, RsResource), pCallContext, pCpuMapping);
}

static inline NvBool dispchnShareCallback_DISPATCH(struct DispChannel *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__(staticCast(pGpuResource, RsResource), pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS dispchnGetMapAddrSpace_DISPATCH(struct DispChannel *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__(staticCast(pGpuResource, GpuResource), pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS dispchnInternalControlForward_DISPATCH(struct DispChannel *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__(staticCast(pGpuResource, GpuResource), command, pParams, size);
}

static inline NvHandle dispchnGetInternalObjectHandle_DISPATCH(struct DispChannel *pGpuResource) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__(staticCast(pGpuResource, GpuResource));
}

static inline NvBool dispchnAccessCallback_DISPATCH(struct DispChannel *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__(staticCast(pResource, RsResource), pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS dispchnGetMemInterMapParams_DISPATCH(struct DispChannel *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__(staticCast(pRmResource, RmResource), pParams);
}

static inline NV_STATUS dispchnCheckMemInterUnmap_DISPATCH(struct DispChannel *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__(staticCast(pRmResource, RmResource), bSubdeviceHandleProvided);
}

static inline NV_STATUS dispchnGetMemoryMappingDescriptor_DISPATCH(struct DispChannel *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__(staticCast(pRmResource, RmResource), ppMemDesc);
}

static inline NV_STATUS dispchnControlSerialization_Prologue_DISPATCH(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void dispchnControlSerialization_Epilogue_DISPATCH(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS dispchnControl_Prologue_DISPATCH(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void dispchnControl_Epilogue_DISPATCH(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool dispchnCanCopy_DISPATCH(struct DispChannel *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__(staticCast(pResource, RsResource));
}

static inline NV_STATUS dispchnIsDuplicate_DISPATCH(struct DispChannel *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__(staticCast(pResource, RsResource), hMemory, pDuplicate);
}

static inline void dispchnPreDestruct_DISPATCH(struct DispChannel *pResource) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__(staticCast(pResource, RsResource));
}

static inline NV_STATUS dispchnControlFilter_DISPATCH(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool dispchnIsPartialUnmapSupported_DISPATCH(struct DispChannel *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__(staticCast(pResource, RsResource));
}

static inline NV_STATUS dispchnMapTo_DISPATCH(struct DispChannel *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__(staticCast(pResource, RsResource), pParams);
}

static inline NV_STATUS dispchnUnmapFrom_DISPATCH(struct DispChannel *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__(staticCast(pResource, RsResource), pParams);
}

static inline NvU32 dispchnGetRefCount_DISPATCH(struct DispChannel *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__(staticCast(pResource, RsResource));
}

static inline void dispchnAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct DispChannel *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__(pClient, staticCast(pResource, RsResource), pReference);
}

static inline PEVENTNOTIFICATION * dispchnGetNotificationListPtr_DISPATCH(struct DispChannel *pNotifier) {
    return pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetNotificationListPtr__(staticCast(pNotifier, INotifier));
}

static inline struct NotifShare * dispchnGetNotificationShare_DISPATCH(struct DispChannel *pNotifier) {
    return pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetNotificationShare__(staticCast(pNotifier, INotifier));
}

static inline void dispchnSetNotificationShare_DISPATCH(struct DispChannel *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifySetNotificationShare__(staticCast(pNotifier, INotifier), pNotifShare);
}

static inline NV_STATUS dispchnUnregisterEvent_DISPATCH(struct DispChannel *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyUnregisterEvent__(staticCast(pNotifier, INotifier), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS dispchnGetOrAllocNotifShare_DISPATCH(struct DispChannel *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetOrAllocNotifShare__(staticCast(pNotifier, INotifier), hNotifierClient, hNotifierResource, ppNotifShare);
}

// Virtual method declarations and/or inline definitions
NV_STATUS dispchnGetRegBaseOffsetAndSize_IMPL(struct DispChannel *pDispChannel, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize);

// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


/*!
 * RM internal class representing XXX_XXX_CHANNEL_PIO
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_DISP_CHANNEL_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__DispChannelPio;
struct NVOC_METADATA__DispChannel;


struct DispChannelPio {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__DispChannelPio *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct DispChannel __nvoc_base_DispChannel;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct RsResource *__nvoc_pbase_RsResource;    // res super^4
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^4
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super^3
    struct GpuResource *__nvoc_pbase_GpuResource;    // gpures super^2
    struct INotifier *__nvoc_pbase_INotifier;    // inotify super^3
    struct Notifier *__nvoc_pbase_Notifier;    // notify super^2
    struct DispChannel *__nvoc_pbase_DispChannel;    // dispchn super
    struct DispChannelPio *__nvoc_pbase_DispChannelPio;    // dispchnpio
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__DispChannelPio {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__DispChannel metadata__DispChannel;
};

#ifndef __nvoc_class_id_DispChannelPio
#define __nvoc_class_id_DispChannelPio 0x10dec3u
typedef struct DispChannelPio DispChannelPio;
#endif /* __nvoc_class_id_DispChannelPio */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_DispChannelPio;

#define __staticCast_DispChannelPio(pThis) \
    ((pThis)->__nvoc_pbase_DispChannelPio)

#ifdef __nvoc_disp_channel_h_disabled
#define __dynamicCast_DispChannelPio(pThis) ((DispChannelPio*) NULL)
#else //__nvoc_disp_channel_h_disabled
#define __dynamicCast_DispChannelPio(pThis) \
    ((DispChannelPio*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(DispChannelPio)))
#endif //__nvoc_disp_channel_h_disabled

NV_STATUS __nvoc_objCreateDynamic_DispChannelPio(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_DispChannelPio(DispChannelPio**, Dynamic*, NvU32, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __objCreate_DispChannelPio(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams) \
    __nvoc_objCreate_DispChannelPio((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pCallContext, pParams)


// Wrapper macros for implementation functions
NV_STATUS dispchnpioConstruct_IMPL(struct DispChannelPio *pDispChannelPio, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __nvoc_dispchnpioConstruct(pDispChannelPio, pCallContext, pParams) dispchnpioConstruct_IMPL(pDispChannelPio, pCallContext, pParams)


// Wrapper macros for halified functions
#define dispchnpioGetRegBaseOffsetAndSize_FNPTR(pDispChannel) pDispChannel->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__
#define dispchnpioGetRegBaseOffsetAndSize(pDispChannel, pGpu, pOffset, pSize) dispchnpioGetRegBaseOffsetAndSize_DISPATCH(pDispChannel, pGpu, pOffset, pSize)
#define dispchnpioControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__
#define dispchnpioControl(pGpuResource, pCallContext, pParams) dispchnpioControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define dispchnpioMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__
#define dispchnpioMap(pGpuResource, pCallContext, pParams, pCpuMapping) dispchnpioMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define dispchnpioUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__
#define dispchnpioUnmap(pGpuResource, pCallContext, pCpuMapping) dispchnpioUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define dispchnpioShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__
#define dispchnpioShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) dispchnpioShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define dispchnpioGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__
#define dispchnpioGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) dispchnpioGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define dispchnpioInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__
#define dispchnpioInternalControlForward(pGpuResource, command, pParams, size) dispchnpioInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define dispchnpioGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__
#define dispchnpioGetInternalObjectHandle(pGpuResource) dispchnpioGetInternalObjectHandle_DISPATCH(pGpuResource)
#define dispchnpioAccessCallback_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__
#define dispchnpioAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) dispchnpioAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define dispchnpioGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define dispchnpioGetMemInterMapParams(pRmResource, pParams) dispchnpioGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define dispchnpioCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define dispchnpioCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) dispchnpioCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define dispchnpioGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define dispchnpioGetMemoryMappingDescriptor(pRmResource, ppMemDesc) dispchnpioGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define dispchnpioControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__
#define dispchnpioControlSerialization_Prologue(pResource, pCallContext, pParams) dispchnpioControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define dispchnpioControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__
#define dispchnpioControlSerialization_Epilogue(pResource, pCallContext, pParams) dispchnpioControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define dispchnpioControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__
#define dispchnpioControl_Prologue(pResource, pCallContext, pParams) dispchnpioControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define dispchnpioControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__
#define dispchnpioControl_Epilogue(pResource, pCallContext, pParams) dispchnpioControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define dispchnpioCanCopy_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define dispchnpioCanCopy(pResource) dispchnpioCanCopy_DISPATCH(pResource)
#define dispchnpioIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define dispchnpioIsDuplicate(pResource, hMemory, pDuplicate) dispchnpioIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define dispchnpioPreDestruct_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define dispchnpioPreDestruct(pResource) dispchnpioPreDestruct_DISPATCH(pResource)
#define dispchnpioControlFilter_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define dispchnpioControlFilter(pResource, pCallContext, pParams) dispchnpioControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define dispchnpioIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define dispchnpioIsPartialUnmapSupported(pResource) dispchnpioIsPartialUnmapSupported_DISPATCH(pResource)
#define dispchnpioMapTo_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define dispchnpioMapTo(pResource, pParams) dispchnpioMapTo_DISPATCH(pResource, pParams)
#define dispchnpioUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define dispchnpioUnmapFrom(pResource, pParams) dispchnpioUnmapFrom_DISPATCH(pResource, pParams)
#define dispchnpioGetRefCount_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define dispchnpioGetRefCount(pResource) dispchnpioGetRefCount_DISPATCH(pResource)
#define dispchnpioAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define dispchnpioAddAdditionalDependants(pClient, pResource, pReference) dispchnpioAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define dispchnpioGetNotificationListPtr_FNPTR(pNotifier) pNotifier->__nvoc_base_DispChannel.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetNotificationListPtr__
#define dispchnpioGetNotificationListPtr(pNotifier) dispchnpioGetNotificationListPtr_DISPATCH(pNotifier)
#define dispchnpioGetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_DispChannel.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetNotificationShare__
#define dispchnpioGetNotificationShare(pNotifier) dispchnpioGetNotificationShare_DISPATCH(pNotifier)
#define dispchnpioSetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_DispChannel.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifySetNotificationShare__
#define dispchnpioSetNotificationShare(pNotifier, pNotifShare) dispchnpioSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define dispchnpioUnregisterEvent_FNPTR(pNotifier) pNotifier->__nvoc_base_DispChannel.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyUnregisterEvent__
#define dispchnpioUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) dispchnpioUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define dispchnpioGetOrAllocNotifShare_FNPTR(pNotifier) pNotifier->__nvoc_base_DispChannel.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetOrAllocNotifShare__
#define dispchnpioGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) dispchnpioGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)

// Dispatch functions
static inline NV_STATUS dispchnpioGetRegBaseOffsetAndSize_DISPATCH(struct DispChannelPio *pDispChannel, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pDispChannel->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__(staticCast(pDispChannel, GpuResource), pGpu, pOffset, pSize);
}

static inline NV_STATUS dispchnpioControl_DISPATCH(struct DispChannelPio *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__(staticCast(pGpuResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS dispchnpioMap_DISPATCH(struct DispChannelPio *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__(staticCast(pGpuResource, RsResource), pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS dispchnpioUnmap_DISPATCH(struct DispChannelPio *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__(staticCast(pGpuResource, RsResource), pCallContext, pCpuMapping);
}

static inline NvBool dispchnpioShareCallback_DISPATCH(struct DispChannelPio *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__(staticCast(pGpuResource, RsResource), pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS dispchnpioGetMapAddrSpace_DISPATCH(struct DispChannelPio *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__(staticCast(pGpuResource, GpuResource), pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS dispchnpioInternalControlForward_DISPATCH(struct DispChannelPio *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__(staticCast(pGpuResource, GpuResource), command, pParams, size);
}

static inline NvHandle dispchnpioGetInternalObjectHandle_DISPATCH(struct DispChannelPio *pGpuResource) {
    return pGpuResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__(staticCast(pGpuResource, GpuResource));
}

static inline NvBool dispchnpioAccessCallback_DISPATCH(struct DispChannelPio *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__(staticCast(pResource, RsResource), pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS dispchnpioGetMemInterMapParams_DISPATCH(struct DispChannelPio *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__(staticCast(pRmResource, RmResource), pParams);
}

static inline NV_STATUS dispchnpioCheckMemInterUnmap_DISPATCH(struct DispChannelPio *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__(staticCast(pRmResource, RmResource), bSubdeviceHandleProvided);
}

static inline NV_STATUS dispchnpioGetMemoryMappingDescriptor_DISPATCH(struct DispChannelPio *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__(staticCast(pRmResource, RmResource), ppMemDesc);
}

static inline NV_STATUS dispchnpioControlSerialization_Prologue_DISPATCH(struct DispChannelPio *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void dispchnpioControlSerialization_Epilogue_DISPATCH(struct DispChannelPio *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS dispchnpioControl_Prologue_DISPATCH(struct DispChannelPio *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void dispchnpioControl_Epilogue_DISPATCH(struct DispChannelPio *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool dispchnpioCanCopy_DISPATCH(struct DispChannelPio *pResource) {
    return pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__(staticCast(pResource, RsResource));
}

static inline NV_STATUS dispchnpioIsDuplicate_DISPATCH(struct DispChannelPio *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__(staticCast(pResource, RsResource), hMemory, pDuplicate);
}

static inline void dispchnpioPreDestruct_DISPATCH(struct DispChannelPio *pResource) {
    pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__(staticCast(pResource, RsResource));
}

static inline NV_STATUS dispchnpioControlFilter_DISPATCH(struct DispChannelPio *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool dispchnpioIsPartialUnmapSupported_DISPATCH(struct DispChannelPio *pResource) {
    return pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__(staticCast(pResource, RsResource));
}

static inline NV_STATUS dispchnpioMapTo_DISPATCH(struct DispChannelPio *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__(staticCast(pResource, RsResource), pParams);
}

static inline NV_STATUS dispchnpioUnmapFrom_DISPATCH(struct DispChannelPio *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__(staticCast(pResource, RsResource), pParams);
}

static inline NvU32 dispchnpioGetRefCount_DISPATCH(struct DispChannelPio *pResource) {
    return pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__(staticCast(pResource, RsResource));
}

static inline void dispchnpioAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct DispChannelPio *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__(pClient, staticCast(pResource, RsResource), pReference);
}

static inline PEVENTNOTIFICATION * dispchnpioGetNotificationListPtr_DISPATCH(struct DispChannelPio *pNotifier) {
    return pNotifier->__nvoc_base_DispChannel.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetNotificationListPtr__(staticCast(pNotifier, INotifier));
}

static inline struct NotifShare * dispchnpioGetNotificationShare_DISPATCH(struct DispChannelPio *pNotifier) {
    return pNotifier->__nvoc_base_DispChannel.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetNotificationShare__(staticCast(pNotifier, INotifier));
}

static inline void dispchnpioSetNotificationShare_DISPATCH(struct DispChannelPio *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__nvoc_base_DispChannel.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifySetNotificationShare__(staticCast(pNotifier, INotifier), pNotifShare);
}

static inline NV_STATUS dispchnpioUnregisterEvent_DISPATCH(struct DispChannelPio *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__nvoc_base_DispChannel.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyUnregisterEvent__(staticCast(pNotifier, INotifier), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS dispchnpioGetOrAllocNotifShare_DISPATCH(struct DispChannelPio *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__nvoc_base_DispChannel.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetOrAllocNotifShare__(staticCast(pNotifier, INotifier), hNotifierClient, hNotifierResource, ppNotifShare);
}

// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


/*!
 * RM internal class representing XXX_XXX_CHANNEL_DMA
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_DISP_CHANNEL_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__DispChannelDma;
struct NVOC_METADATA__DispChannel;


struct DispChannelDma {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__DispChannelDma *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct DispChannel __nvoc_base_DispChannel;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct RsResource *__nvoc_pbase_RsResource;    // res super^4
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^4
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super^3
    struct GpuResource *__nvoc_pbase_GpuResource;    // gpures super^2
    struct INotifier *__nvoc_pbase_INotifier;    // inotify super^3
    struct Notifier *__nvoc_pbase_Notifier;    // notify super^2
    struct DispChannel *__nvoc_pbase_DispChannel;    // dispchn super
    struct DispChannelDma *__nvoc_pbase_DispChannelDma;    // dispchndma
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__DispChannelDma {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__DispChannel metadata__DispChannel;
};

#ifndef __nvoc_class_id_DispChannelDma
#define __nvoc_class_id_DispChannelDma 0xfe3d2eu
typedef struct DispChannelDma DispChannelDma;
#endif /* __nvoc_class_id_DispChannelDma */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_DispChannelDma;

#define __staticCast_DispChannelDma(pThis) \
    ((pThis)->__nvoc_pbase_DispChannelDma)

#ifdef __nvoc_disp_channel_h_disabled
#define __dynamicCast_DispChannelDma(pThis) ((DispChannelDma*) NULL)
#else //__nvoc_disp_channel_h_disabled
#define __dynamicCast_DispChannelDma(pThis) \
    ((DispChannelDma*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(DispChannelDma)))
#endif //__nvoc_disp_channel_h_disabled

NV_STATUS __nvoc_objCreateDynamic_DispChannelDma(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_DispChannelDma(DispChannelDma**, Dynamic*, NvU32, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __objCreate_DispChannelDma(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams) \
    __nvoc_objCreate_DispChannelDma((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pCallContext, pParams)


// Wrapper macros for implementation functions
NV_STATUS dispchndmaConstruct_IMPL(struct DispChannelDma *pDispChannelDma, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __nvoc_dispchndmaConstruct(pDispChannelDma, pCallContext, pParams) dispchndmaConstruct_IMPL(pDispChannelDma, pCallContext, pParams)


// Wrapper macros for halified functions
#define dispchndmaGetRegBaseOffsetAndSize_FNPTR(pDispChannel) pDispChannel->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__
#define dispchndmaGetRegBaseOffsetAndSize(pDispChannel, pGpu, pOffset, pSize) dispchndmaGetRegBaseOffsetAndSize_DISPATCH(pDispChannel, pGpu, pOffset, pSize)
#define dispchndmaControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__
#define dispchndmaControl(pGpuResource, pCallContext, pParams) dispchndmaControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define dispchndmaMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__
#define dispchndmaMap(pGpuResource, pCallContext, pParams, pCpuMapping) dispchndmaMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define dispchndmaUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__
#define dispchndmaUnmap(pGpuResource, pCallContext, pCpuMapping) dispchndmaUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define dispchndmaShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__
#define dispchndmaShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) dispchndmaShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define dispchndmaGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__
#define dispchndmaGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) dispchndmaGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define dispchndmaInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__
#define dispchndmaInternalControlForward(pGpuResource, command, pParams, size) dispchndmaInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define dispchndmaGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__
#define dispchndmaGetInternalObjectHandle(pGpuResource) dispchndmaGetInternalObjectHandle_DISPATCH(pGpuResource)
#define dispchndmaAccessCallback_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__
#define dispchndmaAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) dispchndmaAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define dispchndmaGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define dispchndmaGetMemInterMapParams(pRmResource, pParams) dispchndmaGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define dispchndmaCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define dispchndmaCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) dispchndmaCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define dispchndmaGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define dispchndmaGetMemoryMappingDescriptor(pRmResource, ppMemDesc) dispchndmaGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define dispchndmaControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__
#define dispchndmaControlSerialization_Prologue(pResource, pCallContext, pParams) dispchndmaControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define dispchndmaControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__
#define dispchndmaControlSerialization_Epilogue(pResource, pCallContext, pParams) dispchndmaControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define dispchndmaControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__
#define dispchndmaControl_Prologue(pResource, pCallContext, pParams) dispchndmaControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define dispchndmaControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__
#define dispchndmaControl_Epilogue(pResource, pCallContext, pParams) dispchndmaControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define dispchndmaCanCopy_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define dispchndmaCanCopy(pResource) dispchndmaCanCopy_DISPATCH(pResource)
#define dispchndmaIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define dispchndmaIsDuplicate(pResource, hMemory, pDuplicate) dispchndmaIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define dispchndmaPreDestruct_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define dispchndmaPreDestruct(pResource) dispchndmaPreDestruct_DISPATCH(pResource)
#define dispchndmaControlFilter_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define dispchndmaControlFilter(pResource, pCallContext, pParams) dispchndmaControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define dispchndmaIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define dispchndmaIsPartialUnmapSupported(pResource) dispchndmaIsPartialUnmapSupported_DISPATCH(pResource)
#define dispchndmaMapTo_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define dispchndmaMapTo(pResource, pParams) dispchndmaMapTo_DISPATCH(pResource, pParams)
#define dispchndmaUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define dispchndmaUnmapFrom(pResource, pParams) dispchndmaUnmapFrom_DISPATCH(pResource, pParams)
#define dispchndmaGetRefCount_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define dispchndmaGetRefCount(pResource) dispchndmaGetRefCount_DISPATCH(pResource)
#define dispchndmaAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define dispchndmaAddAdditionalDependants(pClient, pResource, pReference) dispchndmaAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define dispchndmaGetNotificationListPtr_FNPTR(pNotifier) pNotifier->__nvoc_base_DispChannel.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetNotificationListPtr__
#define dispchndmaGetNotificationListPtr(pNotifier) dispchndmaGetNotificationListPtr_DISPATCH(pNotifier)
#define dispchndmaGetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_DispChannel.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetNotificationShare__
#define dispchndmaGetNotificationShare(pNotifier) dispchndmaGetNotificationShare_DISPATCH(pNotifier)
#define dispchndmaSetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_DispChannel.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifySetNotificationShare__
#define dispchndmaSetNotificationShare(pNotifier, pNotifShare) dispchndmaSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define dispchndmaUnregisterEvent_FNPTR(pNotifier) pNotifier->__nvoc_base_DispChannel.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyUnregisterEvent__
#define dispchndmaUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) dispchndmaUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define dispchndmaGetOrAllocNotifShare_FNPTR(pNotifier) pNotifier->__nvoc_base_DispChannel.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetOrAllocNotifShare__
#define dispchndmaGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) dispchndmaGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)

// Dispatch functions
static inline NV_STATUS dispchndmaGetRegBaseOffsetAndSize_DISPATCH(struct DispChannelDma *pDispChannel, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pDispChannel->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__(staticCast(pDispChannel, GpuResource), pGpu, pOffset, pSize);
}

static inline NV_STATUS dispchndmaControl_DISPATCH(struct DispChannelDma *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__(staticCast(pGpuResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS dispchndmaMap_DISPATCH(struct DispChannelDma *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__(staticCast(pGpuResource, RsResource), pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS dispchndmaUnmap_DISPATCH(struct DispChannelDma *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__(staticCast(pGpuResource, RsResource), pCallContext, pCpuMapping);
}

static inline NvBool dispchndmaShareCallback_DISPATCH(struct DispChannelDma *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__(staticCast(pGpuResource, RsResource), pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS dispchndmaGetMapAddrSpace_DISPATCH(struct DispChannelDma *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__(staticCast(pGpuResource, GpuResource), pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS dispchndmaInternalControlForward_DISPATCH(struct DispChannelDma *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__(staticCast(pGpuResource, GpuResource), command, pParams, size);
}

static inline NvHandle dispchndmaGetInternalObjectHandle_DISPATCH(struct DispChannelDma *pGpuResource) {
    return pGpuResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__(staticCast(pGpuResource, GpuResource));
}

static inline NvBool dispchndmaAccessCallback_DISPATCH(struct DispChannelDma *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__(staticCast(pResource, RsResource), pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS dispchndmaGetMemInterMapParams_DISPATCH(struct DispChannelDma *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__(staticCast(pRmResource, RmResource), pParams);
}

static inline NV_STATUS dispchndmaCheckMemInterUnmap_DISPATCH(struct DispChannelDma *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__(staticCast(pRmResource, RmResource), bSubdeviceHandleProvided);
}

static inline NV_STATUS dispchndmaGetMemoryMappingDescriptor_DISPATCH(struct DispChannelDma *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__(staticCast(pRmResource, RmResource), ppMemDesc);
}

static inline NV_STATUS dispchndmaControlSerialization_Prologue_DISPATCH(struct DispChannelDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void dispchndmaControlSerialization_Epilogue_DISPATCH(struct DispChannelDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS dispchndmaControl_Prologue_DISPATCH(struct DispChannelDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void dispchndmaControl_Epilogue_DISPATCH(struct DispChannelDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool dispchndmaCanCopy_DISPATCH(struct DispChannelDma *pResource) {
    return pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__(staticCast(pResource, RsResource));
}

static inline NV_STATUS dispchndmaIsDuplicate_DISPATCH(struct DispChannelDma *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__(staticCast(pResource, RsResource), hMemory, pDuplicate);
}

static inline void dispchndmaPreDestruct_DISPATCH(struct DispChannelDma *pResource) {
    pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__(staticCast(pResource, RsResource));
}

static inline NV_STATUS dispchndmaControlFilter_DISPATCH(struct DispChannelDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool dispchndmaIsPartialUnmapSupported_DISPATCH(struct DispChannelDma *pResource) {
    return pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__(staticCast(pResource, RsResource));
}

static inline NV_STATUS dispchndmaMapTo_DISPATCH(struct DispChannelDma *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__(staticCast(pResource, RsResource), pParams);
}

static inline NV_STATUS dispchndmaUnmapFrom_DISPATCH(struct DispChannelDma *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__(staticCast(pResource, RsResource), pParams);
}

static inline NvU32 dispchndmaGetRefCount_DISPATCH(struct DispChannelDma *pResource) {
    return pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__(staticCast(pResource, RsResource));
}

static inline void dispchndmaAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct DispChannelDma *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__(pClient, staticCast(pResource, RsResource), pReference);
}

static inline PEVENTNOTIFICATION * dispchndmaGetNotificationListPtr_DISPATCH(struct DispChannelDma *pNotifier) {
    return pNotifier->__nvoc_base_DispChannel.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetNotificationListPtr__(staticCast(pNotifier, INotifier));
}

static inline struct NotifShare * dispchndmaGetNotificationShare_DISPATCH(struct DispChannelDma *pNotifier) {
    return pNotifier->__nvoc_base_DispChannel.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetNotificationShare__(staticCast(pNotifier, INotifier));
}

static inline void dispchndmaSetNotificationShare_DISPATCH(struct DispChannelDma *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__nvoc_base_DispChannel.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifySetNotificationShare__(staticCast(pNotifier, INotifier), pNotifShare);
}

static inline NV_STATUS dispchndmaUnregisterEvent_DISPATCH(struct DispChannelDma *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__nvoc_base_DispChannel.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyUnregisterEvent__(staticCast(pNotifier, INotifier), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS dispchndmaGetOrAllocNotifShare_DISPATCH(struct DispChannelDma *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__nvoc_base_DispChannel.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetOrAllocNotifShare__(staticCast(pNotifier, INotifier), hNotifierClient, hNotifierResource, ppNotifShare);
}

// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


#endif // DISP_CHANNEL_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_DISP_CHANNEL_NVOC_H_
