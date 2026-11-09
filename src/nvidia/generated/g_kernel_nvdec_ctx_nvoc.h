
#ifndef _G_KERNEL_NVDEC_CTX_NVOC_H_
#define _G_KERNEL_NVDEC_CTX_NVOC_H_

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
#include "g_kernel_nvdec_ctx_nvoc.h"

#ifndef KERNEL_NVDEC_CTX_H
#define KERNEL_NVDEC_CTX_H

#include "kernel/gpu/fifo/channel_descendant.h"

ENGDESCRIPTOR nvdecGetEngineDescFromAllocParams(OBJGPU *pGpu, NvU32 externalClassId, void *pAllocParams);

/*!
 * RM internal class representing NVXXXX_VIDEO_DECODER
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_KERNEL_NVDEC_CTX_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NvdecContext;
struct NVOC_METADATA__ChannelDescendant;


struct NvdecContext {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__NvdecContext *__nvoc_metadata_ptr;
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
    struct NvdecContext *__nvoc_pbase_NvdecContext;    // nvdecctx
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NvdecContext {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__ChannelDescendant metadata__ChannelDescendant;
};

#ifndef __nvoc_class_id_NvdecContext
#define __nvoc_class_id_NvdecContext 0x70d2beu
typedef struct NvdecContext NvdecContext;
#endif /* __nvoc_class_id_NvdecContext */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvdecContext;

#define __staticCast_NvdecContext(pThis) \
    ((pThis)->__nvoc_pbase_NvdecContext)

#ifdef __nvoc_kernel_nvdec_ctx_h_disabled
#define __dynamicCast_NvdecContext(pThis) ((NvdecContext*) NULL)
#else //__nvoc_kernel_nvdec_ctx_h_disabled
#define __dynamicCast_NvdecContext(pThis) \
    ((NvdecContext*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(NvdecContext)))
#endif //__nvoc_kernel_nvdec_ctx_h_disabled

NV_STATUS __nvoc_objCreateDynamic_NvdecContext(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_NvdecContext(NvdecContext**, Dynamic*, NvU32, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __objCreate_NvdecContext(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams) \
    __nvoc_objCreate_NvdecContext((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pCallContext, pParams)


// Wrapper macros for implementation functions
#ifdef __nvoc_kernel_nvdec_ctx_h_disabled
static inline NV_STATUS nvdecctxConstructHal(struct NvdecContext *pNvdecContext, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {
    NV_ASSERT_FAILED_PRECOMP("NvdecContext was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_nvdec_ctx_h_disabled
#define nvdecctxConstructHal(pNvdecContext, pCallContext, pParams) nvdecctxConstructHal_KERNEL(pNvdecContext, pCallContext, pParams)
#endif // __nvoc_kernel_nvdec_ctx_h_disabled

#ifdef __nvoc_kernel_nvdec_ctx_h_disabled
static inline void nvdecctxDestructHal(struct NvdecContext *pNvdecContext) {
    NV_ASSERT_FAILED_PRECOMP("NvdecContext was disabled!");
}
#else // __nvoc_kernel_nvdec_ctx_h_disabled
#define nvdecctxDestructHal(pNvdecContext) nvdecctxDestructHal_KERNEL(pNvdecContext)
#endif // __nvoc_kernel_nvdec_ctx_h_disabled


// Wrapper macros for halified functions
#define nvdecctxConstructHal_HAL(pNvdecContext, pCallContext, pParams) nvdecctxConstructHal(pNvdecContext, pCallContext, pParams)
#define nvdecctxDestructHal_HAL(pNvdecContext) nvdecctxDestructHal(pNvdecContext)
#define nvdecctxGetSwMethods_FNPTR(pChannelDescendant) pChannelDescendant->__nvoc_base_ChannelDescendant.__nvoc_metadata_ptr->vtable.__chandesGetSwMethods__
#define nvdecctxGetSwMethods(pChannelDescendant, ppMethods, pNumMethods) nvdecctxGetSwMethods_DISPATCH(pChannelDescendant, ppMethods, pNumMethods)
#define nvdecctxIsSwMethodStalling_FNPTR(pChannelDescendant) pChannelDescendant->__nvoc_base_ChannelDescendant.__nvoc_metadata_ptr->vtable.__chandesIsSwMethodStalling__
#define nvdecctxIsSwMethodStalling(pChannelDescendant, hHandle) nvdecctxIsSwMethodStalling_DISPATCH(pChannelDescendant, hHandle)
#define nvdecctxCheckMemInterUnmap_FNPTR(pChannelDescendant) pChannelDescendant->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define nvdecctxCheckMemInterUnmap(pChannelDescendant, bSubdeviceHandleProvided) nvdecctxCheckMemInterUnmap_DISPATCH(pChannelDescendant, bSubdeviceHandleProvided)
#define nvdecctxControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__
#define nvdecctxControl(pGpuResource, pCallContext, pParams) nvdecctxControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define nvdecctxMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__
#define nvdecctxMap(pGpuResource, pCallContext, pParams, pCpuMapping) nvdecctxMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define nvdecctxUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__
#define nvdecctxUnmap(pGpuResource, pCallContext, pCpuMapping) nvdecctxUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define nvdecctxShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__
#define nvdecctxShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) nvdecctxShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define nvdecctxGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__
#define nvdecctxGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) nvdecctxGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define nvdecctxGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__
#define nvdecctxGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) nvdecctxGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define nvdecctxInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__
#define nvdecctxInternalControlForward(pGpuResource, command, pParams, size) nvdecctxInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define nvdecctxGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__
#define nvdecctxGetInternalObjectHandle(pGpuResource) nvdecctxGetInternalObjectHandle_DISPATCH(pGpuResource)
#define nvdecctxAccessCallback_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__
#define nvdecctxAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) nvdecctxAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define nvdecctxGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define nvdecctxGetMemInterMapParams(pRmResource, pParams) nvdecctxGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define nvdecctxGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define nvdecctxGetMemoryMappingDescriptor(pRmResource, ppMemDesc) nvdecctxGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define nvdecctxControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__
#define nvdecctxControlSerialization_Prologue(pResource, pCallContext, pParams) nvdecctxControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define nvdecctxControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__
#define nvdecctxControlSerialization_Epilogue(pResource, pCallContext, pParams) nvdecctxControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define nvdecctxControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__
#define nvdecctxControl_Prologue(pResource, pCallContext, pParams) nvdecctxControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define nvdecctxControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__
#define nvdecctxControl_Epilogue(pResource, pCallContext, pParams) nvdecctxControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define nvdecctxCanCopy_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define nvdecctxCanCopy(pResource) nvdecctxCanCopy_DISPATCH(pResource)
#define nvdecctxIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define nvdecctxIsDuplicate(pResource, hMemory, pDuplicate) nvdecctxIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define nvdecctxPreDestruct_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define nvdecctxPreDestruct(pResource) nvdecctxPreDestruct_DISPATCH(pResource)
#define nvdecctxControlFilter_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define nvdecctxControlFilter(pResource, pCallContext, pParams) nvdecctxControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define nvdecctxIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define nvdecctxIsPartialUnmapSupported(pResource) nvdecctxIsPartialUnmapSupported_DISPATCH(pResource)
#define nvdecctxMapTo_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define nvdecctxMapTo(pResource, pParams) nvdecctxMapTo_DISPATCH(pResource, pParams)
#define nvdecctxUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define nvdecctxUnmapFrom(pResource, pParams) nvdecctxUnmapFrom_DISPATCH(pResource, pParams)
#define nvdecctxGetRefCount_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define nvdecctxGetRefCount(pResource) nvdecctxGetRefCount_DISPATCH(pResource)
#define nvdecctxAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define nvdecctxAddAdditionalDependants(pClient, pResource, pReference) nvdecctxAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define nvdecctxGetNotificationListPtr_FNPTR(pNotifier) pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetNotificationListPtr__
#define nvdecctxGetNotificationListPtr(pNotifier) nvdecctxGetNotificationListPtr_DISPATCH(pNotifier)
#define nvdecctxGetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetNotificationShare__
#define nvdecctxGetNotificationShare(pNotifier) nvdecctxGetNotificationShare_DISPATCH(pNotifier)
#define nvdecctxSetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifySetNotificationShare__
#define nvdecctxSetNotificationShare(pNotifier, pNotifShare) nvdecctxSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define nvdecctxUnregisterEvent_FNPTR(pNotifier) pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyUnregisterEvent__
#define nvdecctxUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) nvdecctxUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define nvdecctxGetOrAllocNotifShare_FNPTR(pNotifier) pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetOrAllocNotifShare__
#define nvdecctxGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) nvdecctxGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)

// Dispatch functions
static inline NV_STATUS nvdecctxGetSwMethods_DISPATCH(struct NvdecContext *pChannelDescendant, const METHOD **ppMethods, NvU32 *pNumMethods) {
    return pChannelDescendant->__nvoc_base_ChannelDescendant.__nvoc_metadata_ptr->vtable.__chandesGetSwMethods__(staticCast(pChannelDescendant, ChannelDescendant), ppMethods, pNumMethods);
}

static inline NvBool nvdecctxIsSwMethodStalling_DISPATCH(struct NvdecContext *pChannelDescendant, NvU32 hHandle) {
    return pChannelDescendant->__nvoc_base_ChannelDescendant.__nvoc_metadata_ptr->vtable.__chandesIsSwMethodStalling__(staticCast(pChannelDescendant, ChannelDescendant), hHandle);
}

static inline NV_STATUS nvdecctxCheckMemInterUnmap_DISPATCH(struct NvdecContext *pChannelDescendant, NvBool bSubdeviceHandleProvided) {
    return pChannelDescendant->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__(staticCast(pChannelDescendant, RmResource), bSubdeviceHandleProvided);
}

static inline NV_STATUS nvdecctxControl_DISPATCH(struct NvdecContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__(staticCast(pGpuResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS nvdecctxMap_DISPATCH(struct NvdecContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__(staticCast(pGpuResource, RsResource), pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS nvdecctxUnmap_DISPATCH(struct NvdecContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__(staticCast(pGpuResource, RsResource), pCallContext, pCpuMapping);
}

static inline NvBool nvdecctxShareCallback_DISPATCH(struct NvdecContext *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__(staticCast(pGpuResource, RsResource), pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS nvdecctxGetRegBaseOffsetAndSize_DISPATCH(struct NvdecContext *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__(staticCast(pGpuResource, GpuResource), pGpu, pOffset, pSize);
}

static inline NV_STATUS nvdecctxGetMapAddrSpace_DISPATCH(struct NvdecContext *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__(staticCast(pGpuResource, GpuResource), pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS nvdecctxInternalControlForward_DISPATCH(struct NvdecContext *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__(staticCast(pGpuResource, GpuResource), command, pParams, size);
}

static inline NvHandle nvdecctxGetInternalObjectHandle_DISPATCH(struct NvdecContext *pGpuResource) {
    return pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__(staticCast(pGpuResource, GpuResource));
}

static inline NvBool nvdecctxAccessCallback_DISPATCH(struct NvdecContext *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__(staticCast(pResource, RsResource), pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS nvdecctxGetMemInterMapParams_DISPATCH(struct NvdecContext *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__(staticCast(pRmResource, RmResource), pParams);
}

static inline NV_STATUS nvdecctxGetMemoryMappingDescriptor_DISPATCH(struct NvdecContext *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__(staticCast(pRmResource, RmResource), ppMemDesc);
}

static inline NV_STATUS nvdecctxControlSerialization_Prologue_DISPATCH(struct NvdecContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void nvdecctxControlSerialization_Epilogue_DISPATCH(struct NvdecContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS nvdecctxControl_Prologue_DISPATCH(struct NvdecContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void nvdecctxControl_Epilogue_DISPATCH(struct NvdecContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool nvdecctxCanCopy_DISPATCH(struct NvdecContext *pResource) {
    return pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__(staticCast(pResource, RsResource));
}

static inline NV_STATUS nvdecctxIsDuplicate_DISPATCH(struct NvdecContext *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__(staticCast(pResource, RsResource), hMemory, pDuplicate);
}

static inline void nvdecctxPreDestruct_DISPATCH(struct NvdecContext *pResource) {
    pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__(staticCast(pResource, RsResource));
}

static inline NV_STATUS nvdecctxControlFilter_DISPATCH(struct NvdecContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool nvdecctxIsPartialUnmapSupported_DISPATCH(struct NvdecContext *pResource) {
    return pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__(staticCast(pResource, RsResource));
}

static inline NV_STATUS nvdecctxMapTo_DISPATCH(struct NvdecContext *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__(staticCast(pResource, RsResource), pParams);
}

static inline NV_STATUS nvdecctxUnmapFrom_DISPATCH(struct NvdecContext *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__(staticCast(pResource, RsResource), pParams);
}

static inline NvU32 nvdecctxGetRefCount_DISPATCH(struct NvdecContext *pResource) {
    return pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__(staticCast(pResource, RsResource));
}

static inline void nvdecctxAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct NvdecContext *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__(pClient, staticCast(pResource, RsResource), pReference);
}

static inline PEVENTNOTIFICATION * nvdecctxGetNotificationListPtr_DISPATCH(struct NvdecContext *pNotifier) {
    return pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetNotificationListPtr__(staticCast(pNotifier, INotifier));
}

static inline struct NotifShare * nvdecctxGetNotificationShare_DISPATCH(struct NvdecContext *pNotifier) {
    return pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetNotificationShare__(staticCast(pNotifier, INotifier));
}

static inline void nvdecctxSetNotificationShare_DISPATCH(struct NvdecContext *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifySetNotificationShare__(staticCast(pNotifier, INotifier), pNotifShare);
}

static inline NV_STATUS nvdecctxUnregisterEvent_DISPATCH(struct NvdecContext *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyUnregisterEvent__(staticCast(pNotifier, INotifier), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS nvdecctxGetOrAllocNotifShare_DISPATCH(struct NvdecContext *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetOrAllocNotifShare__(staticCast(pNotifier, INotifier), hNotifierClient, hNotifierResource, ppNotifShare);
}

// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
NV_STATUS nvdecctxConstructHal_KERNEL(struct NvdecContext *pNvdecContext, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);

void nvdecctxDestructHal_KERNEL(struct NvdecContext *pNvdecContext);

// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
static inline NV_STATUS __nvoc_nvdecctxConstruct(struct NvdecContext *pNvdecContext, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams){
    return nvdecctxConstructHal(pNvdecContext, pCallContext, pParams);
}

static inline void __nvoc_nvdecctxDestruct(struct NvdecContext *pNvdecContext){
    nvdecctxDestructHal(pNvdecContext);
}

#undef PRIVATE_FIELD


#endif // KERNEL_NVDEC_CTX_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_NVDEC_CTX_NVOC_H_
