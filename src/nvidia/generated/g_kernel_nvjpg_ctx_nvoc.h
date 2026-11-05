
#ifndef _G_KERNEL_NVJPG_CTX_NVOC_H_
#define _G_KERNEL_NVJPG_CTX_NVOC_H_

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

#pragma once
#include "g_kernel_nvjpg_ctx_nvoc.h"

#ifndef KERNEL_NVJPG_CTX_H
#define KERNEL_NVJPG_CTX_H

#include "kernel/gpu/fifo/channel_descendant.h"

ENGDESCRIPTOR nvjpgGetEngineDescFromAllocParams(OBJGPU *pGpu, NvU32 externalClassId, void *pAllocParams);

/*!
 * RM internal class representing NVXXXX_VIDEO_NVJPG
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_KERNEL_NVJPG_CTX_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NvjpgContext;
struct NVOC_METADATA__ChannelDescendant;


struct NvjpgContext {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__NvjpgContext *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct ChannelDescendant __nvoc_base_ChannelDescendant;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct RsResource *__nvoc_pbase_RsResource;    // res super^4
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^4
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super^3
    struct GpuResource *__nvoc_pbase_GpuResource;    // gpures super^2
    struct INotifier *__nvoc_pbase_INotifier;    // inotify super^3
    struct Notifier *__nvoc_pbase_Notifier;    // notify super^2
    struct ChannelDescendant *__nvoc_pbase_ChannelDescendant;    // chandes super
    struct NvjpgContext *__nvoc_pbase_NvjpgContext;    // nvjpgctx
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NvjpgContext {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__ChannelDescendant metadata__ChannelDescendant;
};

#ifndef __nvoc_class_id_NvjpgContext
#define __nvoc_class_id_NvjpgContext 0x08c1ceu
typedef struct NvjpgContext NvjpgContext;
#endif /* __nvoc_class_id_NvjpgContext */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvjpgContext;

#define __staticCast_NvjpgContext(pThis) \
    ((pThis)->__nvoc_pbase_NvjpgContext)

#ifdef __nvoc_kernel_nvjpg_ctx_h_disabled
#define __dynamicCast_NvjpgContext(pThis) ((NvjpgContext*) NULL)
#else //__nvoc_kernel_nvjpg_ctx_h_disabled
#define __dynamicCast_NvjpgContext(pThis) \
    ((NvjpgContext*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(NvjpgContext)))
#endif //__nvoc_kernel_nvjpg_ctx_h_disabled

NV_STATUS __nvoc_objCreateDynamic_NvjpgContext(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_NvjpgContext(NvjpgContext**, Dynamic*, NvU32, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __objCreate_NvjpgContext(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams) \
    __nvoc_objCreate_NvjpgContext((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pCallContext, pParams)


// Wrapper macros for implementation functions
#ifdef __nvoc_kernel_nvjpg_ctx_h_disabled
static inline NV_STATUS nvjpgctxConstructHal(struct NvjpgContext *pNvjpgContext, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {
    NV_ASSERT_FAILED_PRECOMP("NvjpgContext was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_nvjpg_ctx_h_disabled
#define nvjpgctxConstructHal(pNvjpgContext, pCallContext, pParams) nvjpgctxConstructHal_KERNEL(pNvjpgContext, pCallContext, pParams)
#endif // __nvoc_kernel_nvjpg_ctx_h_disabled

#ifdef __nvoc_kernel_nvjpg_ctx_h_disabled
static inline void nvjpgctxDestructHal(struct NvjpgContext *pNvjpgContext) {
    NV_ASSERT_FAILED_PRECOMP("NvjpgContext was disabled!");
}
#else // __nvoc_kernel_nvjpg_ctx_h_disabled
#define nvjpgctxDestructHal(pNvjpgContext) nvjpgctxDestructHal_KERNEL(pNvjpgContext)
#endif // __nvoc_kernel_nvjpg_ctx_h_disabled


// Wrapper macros for halified functions
#define nvjpgctxConstructHal_HAL(pNvjpgContext, pCallContext, pParams) nvjpgctxConstructHal(pNvjpgContext, pCallContext, pParams)
#define nvjpgctxDestructHal_HAL(pNvjpgContext) nvjpgctxDestructHal(pNvjpgContext)
#define nvjpgctxGetSwMethods_FNPTR(pChannelDescendant) pChannelDescendant->__nvoc_base_ChannelDescendant.__nvoc_metadata_ptr->vtable.__chandesGetSwMethods__
#define nvjpgctxGetSwMethods(pChannelDescendant, ppMethods, pNumMethods) nvjpgctxGetSwMethods_DISPATCH(pChannelDescendant, ppMethods, pNumMethods)
#define nvjpgctxIsSwMethodStalling_FNPTR(pChannelDescendant) pChannelDescendant->__nvoc_base_ChannelDescendant.__nvoc_metadata_ptr->vtable.__chandesIsSwMethodStalling__
#define nvjpgctxIsSwMethodStalling(pChannelDescendant, hHandle) nvjpgctxIsSwMethodStalling_DISPATCH(pChannelDescendant, hHandle)
#define nvjpgctxCheckMemInterUnmap_FNPTR(pChannelDescendant) pChannelDescendant->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define nvjpgctxCheckMemInterUnmap(pChannelDescendant, bSubdeviceHandleProvided) nvjpgctxCheckMemInterUnmap_DISPATCH(pChannelDescendant, bSubdeviceHandleProvided)
#define nvjpgctxControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__
#define nvjpgctxControl(pGpuResource, pCallContext, pParams) nvjpgctxControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define nvjpgctxMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__
#define nvjpgctxMap(pGpuResource, pCallContext, pParams, pCpuMapping) nvjpgctxMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define nvjpgctxUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__
#define nvjpgctxUnmap(pGpuResource, pCallContext, pCpuMapping) nvjpgctxUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define nvjpgctxShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__
#define nvjpgctxShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) nvjpgctxShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define nvjpgctxGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__
#define nvjpgctxGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) nvjpgctxGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define nvjpgctxGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__
#define nvjpgctxGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) nvjpgctxGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define nvjpgctxInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__
#define nvjpgctxInternalControlForward(pGpuResource, command, pParams, size) nvjpgctxInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define nvjpgctxGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__
#define nvjpgctxGetInternalObjectHandle(pGpuResource) nvjpgctxGetInternalObjectHandle_DISPATCH(pGpuResource)
#define nvjpgctxAccessCallback_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__
#define nvjpgctxAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) nvjpgctxAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define nvjpgctxGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define nvjpgctxGetMemInterMapParams(pRmResource, pParams) nvjpgctxGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define nvjpgctxGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define nvjpgctxGetMemoryMappingDescriptor(pRmResource, ppMemDesc) nvjpgctxGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define nvjpgctxControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__
#define nvjpgctxControlSerialization_Prologue(pResource, pCallContext, pParams) nvjpgctxControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define nvjpgctxControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__
#define nvjpgctxControlSerialization_Epilogue(pResource, pCallContext, pParams) nvjpgctxControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define nvjpgctxControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__
#define nvjpgctxControl_Prologue(pResource, pCallContext, pParams) nvjpgctxControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define nvjpgctxControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__
#define nvjpgctxControl_Epilogue(pResource, pCallContext, pParams) nvjpgctxControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define nvjpgctxCanCopy_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define nvjpgctxCanCopy(pResource) nvjpgctxCanCopy_DISPATCH(pResource)
#define nvjpgctxIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define nvjpgctxIsDuplicate(pResource, hMemory, pDuplicate) nvjpgctxIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define nvjpgctxPreDestruct_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define nvjpgctxPreDestruct(pResource) nvjpgctxPreDestruct_DISPATCH(pResource)
#define nvjpgctxControlFilter_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define nvjpgctxControlFilter(pResource, pCallContext, pParams) nvjpgctxControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define nvjpgctxIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define nvjpgctxIsPartialUnmapSupported(pResource) nvjpgctxIsPartialUnmapSupported_DISPATCH(pResource)
#define nvjpgctxMapTo_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define nvjpgctxMapTo(pResource, pParams) nvjpgctxMapTo_DISPATCH(pResource, pParams)
#define nvjpgctxUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define nvjpgctxUnmapFrom(pResource, pParams) nvjpgctxUnmapFrom_DISPATCH(pResource, pParams)
#define nvjpgctxGetRefCount_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define nvjpgctxGetRefCount(pResource) nvjpgctxGetRefCount_DISPATCH(pResource)
#define nvjpgctxAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define nvjpgctxAddAdditionalDependants(pClient, pResource, pReference) nvjpgctxAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define nvjpgctxGetNotificationListPtr_FNPTR(pNotifier) pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetNotificationListPtr__
#define nvjpgctxGetNotificationListPtr(pNotifier) nvjpgctxGetNotificationListPtr_DISPATCH(pNotifier)
#define nvjpgctxGetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetNotificationShare__
#define nvjpgctxGetNotificationShare(pNotifier) nvjpgctxGetNotificationShare_DISPATCH(pNotifier)
#define nvjpgctxSetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifySetNotificationShare__
#define nvjpgctxSetNotificationShare(pNotifier, pNotifShare) nvjpgctxSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define nvjpgctxUnregisterEvent_FNPTR(pNotifier) pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyUnregisterEvent__
#define nvjpgctxUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) nvjpgctxUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define nvjpgctxGetOrAllocNotifShare_FNPTR(pNotifier) pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetOrAllocNotifShare__
#define nvjpgctxGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) nvjpgctxGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)

// Dispatch functions
static inline NV_STATUS nvjpgctxGetSwMethods_DISPATCH(struct NvjpgContext *pChannelDescendant, const METHOD **ppMethods, NvU32 *pNumMethods) {
    return pChannelDescendant->__nvoc_base_ChannelDescendant.__nvoc_metadata_ptr->vtable.__chandesGetSwMethods__(staticCast(pChannelDescendant, ChannelDescendant), ppMethods, pNumMethods);
}

static inline NvBool nvjpgctxIsSwMethodStalling_DISPATCH(struct NvjpgContext *pChannelDescendant, NvU32 hHandle) {
    return pChannelDescendant->__nvoc_base_ChannelDescendant.__nvoc_metadata_ptr->vtable.__chandesIsSwMethodStalling__(staticCast(pChannelDescendant, ChannelDescendant), hHandle);
}

static inline NV_STATUS nvjpgctxCheckMemInterUnmap_DISPATCH(struct NvjpgContext *pChannelDescendant, NvBool bSubdeviceHandleProvided) {
    return pChannelDescendant->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__(staticCast(pChannelDescendant, RmResource), bSubdeviceHandleProvided);
}

static inline NV_STATUS nvjpgctxControl_DISPATCH(struct NvjpgContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__(staticCast(pGpuResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS nvjpgctxMap_DISPATCH(struct NvjpgContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__(staticCast(pGpuResource, RsResource), pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS nvjpgctxUnmap_DISPATCH(struct NvjpgContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__(staticCast(pGpuResource, RsResource), pCallContext, pCpuMapping);
}

static inline NvBool nvjpgctxShareCallback_DISPATCH(struct NvjpgContext *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__(staticCast(pGpuResource, RsResource), pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS nvjpgctxGetRegBaseOffsetAndSize_DISPATCH(struct NvjpgContext *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__(staticCast(pGpuResource, GpuResource), pGpu, pOffset, pSize);
}

static inline NV_STATUS nvjpgctxGetMapAddrSpace_DISPATCH(struct NvjpgContext *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__(staticCast(pGpuResource, GpuResource), pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS nvjpgctxInternalControlForward_DISPATCH(struct NvjpgContext *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__(staticCast(pGpuResource, GpuResource), command, pParams, size);
}

static inline NvHandle nvjpgctxGetInternalObjectHandle_DISPATCH(struct NvjpgContext *pGpuResource) {
    return pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__(staticCast(pGpuResource, GpuResource));
}

static inline NvBool nvjpgctxAccessCallback_DISPATCH(struct NvjpgContext *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__(staticCast(pResource, RsResource), pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS nvjpgctxGetMemInterMapParams_DISPATCH(struct NvjpgContext *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__(staticCast(pRmResource, RmResource), pParams);
}

static inline NV_STATUS nvjpgctxGetMemoryMappingDescriptor_DISPATCH(struct NvjpgContext *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__(staticCast(pRmResource, RmResource), ppMemDesc);
}

static inline NV_STATUS nvjpgctxControlSerialization_Prologue_DISPATCH(struct NvjpgContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void nvjpgctxControlSerialization_Epilogue_DISPATCH(struct NvjpgContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS nvjpgctxControl_Prologue_DISPATCH(struct NvjpgContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void nvjpgctxControl_Epilogue_DISPATCH(struct NvjpgContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool nvjpgctxCanCopy_DISPATCH(struct NvjpgContext *pResource) {
    return pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__(staticCast(pResource, RsResource));
}

static inline NV_STATUS nvjpgctxIsDuplicate_DISPATCH(struct NvjpgContext *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__(staticCast(pResource, RsResource), hMemory, pDuplicate);
}

static inline void nvjpgctxPreDestruct_DISPATCH(struct NvjpgContext *pResource) {
    pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__(staticCast(pResource, RsResource));
}

static inline NV_STATUS nvjpgctxControlFilter_DISPATCH(struct NvjpgContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool nvjpgctxIsPartialUnmapSupported_DISPATCH(struct NvjpgContext *pResource) {
    return pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__(staticCast(pResource, RsResource));
}

static inline NV_STATUS nvjpgctxMapTo_DISPATCH(struct NvjpgContext *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__(staticCast(pResource, RsResource), pParams);
}

static inline NV_STATUS nvjpgctxUnmapFrom_DISPATCH(struct NvjpgContext *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__(staticCast(pResource, RsResource), pParams);
}

static inline NvU32 nvjpgctxGetRefCount_DISPATCH(struct NvjpgContext *pResource) {
    return pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__(staticCast(pResource, RsResource));
}

static inline void nvjpgctxAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct NvjpgContext *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__(pClient, staticCast(pResource, RsResource), pReference);
}

static inline PEVENTNOTIFICATION * nvjpgctxGetNotificationListPtr_DISPATCH(struct NvjpgContext *pNotifier) {
    return pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetNotificationListPtr__(staticCast(pNotifier, INotifier));
}

static inline struct NotifShare * nvjpgctxGetNotificationShare_DISPATCH(struct NvjpgContext *pNotifier) {
    return pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetNotificationShare__(staticCast(pNotifier, INotifier));
}

static inline void nvjpgctxSetNotificationShare_DISPATCH(struct NvjpgContext *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifySetNotificationShare__(staticCast(pNotifier, INotifier), pNotifShare);
}

static inline NV_STATUS nvjpgctxUnregisterEvent_DISPATCH(struct NvjpgContext *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyUnregisterEvent__(staticCast(pNotifier, INotifier), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS nvjpgctxGetOrAllocNotifShare_DISPATCH(struct NvjpgContext *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetOrAllocNotifShare__(staticCast(pNotifier, INotifier), hNotifierClient, hNotifierResource, ppNotifShare);
}

// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
NV_STATUS nvjpgctxConstructHal_KERNEL(struct NvjpgContext *pNvjpgContext, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);

void nvjpgctxDestructHal_KERNEL(struct NvjpgContext *pNvjpgContext);

// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
static inline NV_STATUS __nvoc_nvjpgctxConstruct(struct NvjpgContext *pNvjpgContext, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams){
    return nvjpgctxConstructHal(pNvjpgContext, pCallContext, pParams);
}

static inline void __nvoc_nvjpgctxDestruct(struct NvjpgContext *pNvjpgContext){
    nvjpgctxDestructHal(pNvjpgContext);
}

#undef PRIVATE_FIELD


#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_NVJPG_CTX_NVOC_H_
