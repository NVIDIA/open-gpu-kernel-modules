
#ifndef _G_CONTEXT_DMA_NVOC_H_
#define _G_CONTEXT_DMA_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 1993-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_context_dma_nvoc.h"

#ifndef CONTEXT_DMA_H
#define CONTEXT_DMA_H

#include "core/core.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "rmapi/resource.h"
#include "rmapi/event_api.h"
#include "ctrl/ctrl0002.h"
#include "rmapi/control.h" // for macro RMCTRL_EXPORT etc.
#include "nvlimits.h"

struct Device;

#ifndef __nvoc_class_id_Device
#define __nvoc_class_id_Device 0xe0ac20u
typedef struct Device Device;
#endif /* __nvoc_class_id_Device */


struct Memory;

#ifndef __nvoc_class_id_Memory
#define __nvoc_class_id_Memory 0x4789f2u
typedef struct Memory Memory;
#endif /* __nvoc_class_id_Memory */



/*!
 * RM internal class representing NV01_CONTEXT_DMA
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_CONTEXT_DMA_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__ContextDma;
struct NVOC_METADATA__RmResource;
struct NVOC_METADATA__Notifier;


struct ContextDma {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__ContextDma *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct RmResource __nvoc_base_RmResource;
    struct Notifier __nvoc_base_Notifier;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^3
    struct RsResource *__nvoc_pbase_RsResource;    // res super^2
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^2
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super
    struct INotifier *__nvoc_pbase_INotifier;    // inotify super^2
    struct Notifier *__nvoc_pbase_Notifier;    // notify super
    struct ContextDma *__nvoc_pbase_ContextDma;    // ctxdma

    // Data members
    NvU32 Class;
    NvU32 Flags;
    NvBool bReadOnly;
    NvU32 CacheSnoop;
    NvU32 Type;
    NvU64 Limit;
    NV_ADDRESS_SPACE AddressSpace;
    NvBool bUnicast;
    void *KernelVAddr;
    void *KernelPriv;
    NvU64 FbAperture;
    NvU64 FbApertureLen;
    struct Memory *pMemory;
    struct MEMORY_DESCRIPTOR *pMemDesc;
    NvU32 Instance;
    NvU32 InstRefCount;
    struct OBJGPU *pGpu;
    struct Device *pDevice;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__ContextDma {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__RmResource metadata__RmResource;
    const struct NVOC_METADATA__Notifier metadata__Notifier;
};

#ifndef __nvoc_class_id_ContextDma
#define __nvoc_class_id_ContextDma 0x88441bu
typedef struct ContextDma ContextDma;
#endif /* __nvoc_class_id_ContextDma */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_ContextDma;

#define __staticCast_ContextDma(pThis) \
    ((pThis)->__nvoc_pbase_ContextDma)

#ifdef __nvoc_context_dma_h_disabled
#define __dynamicCast_ContextDma(pThis) ((ContextDma*) NULL)
#else //__nvoc_context_dma_h_disabled
#define __dynamicCast_ContextDma(pThis) \
    ((ContextDma*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(ContextDma)))
#endif //__nvoc_context_dma_h_disabled

NV_STATUS __nvoc_objCreateDynamic_ContextDma(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_ContextDma(ContextDma**, Dynamic*, NvU32, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __objCreate_ContextDma(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams) \
    __nvoc_objCreate_ContextDma((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pCallContext, pParams)


// Wrapper macros for implementation functions
NV_STATUS ctxdmaConstruct_IMPL(struct ContextDma *pCtxdma, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __nvoc_ctxdmaConstruct(pCtxdma, pCallContext, pParams) ctxdmaConstruct_IMPL(pCtxdma, pCallContext, pParams)

void ctxdmaDestruct_IMPL(struct ContextDma *pCtxdma);
#define __nvoc_ctxdmaDestruct(pCtxdma) ctxdmaDestruct_IMPL(pCtxdma)

NV_STATUS ctxdmaValidate_IMPL(struct ContextDma *pContextDma, NvU64 start, NvU64 len);
#ifdef __nvoc_context_dma_h_disabled
static inline NV_STATUS ctxdmaValidate(struct ContextDma *pContextDma, NvU64 start, NvU64 len) {
    NV_ASSERT_FAILED_PRECOMP("ContextDma was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_context_dma_h_disabled
#define ctxdmaValidate(pContextDma, start, len) ctxdmaValidate_IMPL(pContextDma, start, len)
#endif // __nvoc_context_dma_h_disabled

NV_STATUS ctxdmaGetKernelVA_IMPL(struct ContextDma *pContextDma, NvU64 start, NvU64 len, void **arg4);
#ifdef __nvoc_context_dma_h_disabled
static inline NV_STATUS ctxdmaGetKernelVA(struct ContextDma *pContextDma, NvU64 start, NvU64 len, void **arg4) {
    NV_ASSERT_FAILED_PRECOMP("ContextDma was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_context_dma_h_disabled
#define ctxdmaGetKernelVA(pContextDma, start, len, arg4) ctxdmaGetKernelVA_IMPL(pContextDma, start, len, arg4)
#endif // __nvoc_context_dma_h_disabled

NvBool ctxdmaIsBound_IMPL(struct ContextDma *pContextDma);
#ifdef __nvoc_context_dma_h_disabled
static inline NvBool ctxdmaIsBound(struct ContextDma *pContextDma) {
    NV_ASSERT_FAILED_PRECOMP("ContextDma was disabled!");
    return NV_FALSE;
}
#else // __nvoc_context_dma_h_disabled
#define ctxdmaIsBound(pContextDma) ctxdmaIsBound_IMPL(pContextDma)
#endif // __nvoc_context_dma_h_disabled

NV_STATUS ctxdmaGetByHandle_IMPL(struct RsClient *pClient, NvHandle hContextDma, struct ContextDma **arg3);
#define ctxdmaGetByHandle(pClient, hContextDma, arg3) ctxdmaGetByHandle_IMPL(pClient, hContextDma, arg3)

NV_STATUS ctxdmaCtrlCmdUpdateContextdma_IMPL(struct ContextDma *pContextDma, NV0002_CTRL_UPDATE_CONTEXTDMA_PARAMS *pUpdateCtxtDmaParams);
#ifdef __nvoc_context_dma_h_disabled
static inline NV_STATUS ctxdmaCtrlCmdUpdateContextdma(struct ContextDma *pContextDma, NV0002_CTRL_UPDATE_CONTEXTDMA_PARAMS *pUpdateCtxtDmaParams) {
    NV_ASSERT_FAILED_PRECOMP("ContextDma was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_context_dma_h_disabled
#define ctxdmaCtrlCmdUpdateContextdma(pContextDma, pUpdateCtxtDmaParams) ctxdmaCtrlCmdUpdateContextdma_IMPL(pContextDma, pUpdateCtxtDmaParams)
#endif // __nvoc_context_dma_h_disabled

NV_STATUS ctxdmaCtrlCmdBindContextdma_IMPL(struct ContextDma *pContextDma, NV0002_CTRL_BIND_CONTEXTDMA_PARAMS *pBindCtxtDmaParams);
#ifdef __nvoc_context_dma_h_disabled
static inline NV_STATUS ctxdmaCtrlCmdBindContextdma(struct ContextDma *pContextDma, NV0002_CTRL_BIND_CONTEXTDMA_PARAMS *pBindCtxtDmaParams) {
    NV_ASSERT_FAILED_PRECOMP("ContextDma was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_context_dma_h_disabled
#define ctxdmaCtrlCmdBindContextdma(pContextDma, pBindCtxtDmaParams) ctxdmaCtrlCmdBindContextdma_IMPL(pContextDma, pBindCtxtDmaParams)
#endif // __nvoc_context_dma_h_disabled

NV_STATUS ctxdmaCtrlCmdUnbindContextdma_IMPL(struct ContextDma *pContextDma, NV0002_CTRL_UNBIND_CONTEXTDMA_PARAMS *pUnbindCtxtDmaParams);
#ifdef __nvoc_context_dma_h_disabled
static inline NV_STATUS ctxdmaCtrlCmdUnbindContextdma(struct ContextDma *pContextDma, NV0002_CTRL_UNBIND_CONTEXTDMA_PARAMS *pUnbindCtxtDmaParams) {
    NV_ASSERT_FAILED_PRECOMP("ContextDma was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_context_dma_h_disabled
#define ctxdmaCtrlCmdUnbindContextdma(pContextDma, pUnbindCtxtDmaParams) ctxdmaCtrlCmdUnbindContextdma_IMPL(pContextDma, pUnbindCtxtDmaParams)
#endif // __nvoc_context_dma_h_disabled


// Wrapper macros for halified functions
#define ctxdmaMapTo_FNPTR(pContextDma) pContextDma->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define ctxdmaMapTo(pContextDma, pParams) ctxdmaMapTo_DISPATCH(pContextDma, pParams)
#define ctxdmaUnmapFrom_FNPTR(pContextDma) pContextDma->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define ctxdmaUnmapFrom(pContextDma, pParams) ctxdmaUnmapFrom_DISPATCH(pContextDma, pParams)
#define ctxdmaAccessCallback_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__
#define ctxdmaAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) ctxdmaAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define ctxdmaShareCallback_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__
#define ctxdmaShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) ctxdmaShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define ctxdmaGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define ctxdmaGetMemInterMapParams(pRmResource, pParams) ctxdmaGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define ctxdmaCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define ctxdmaCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) ctxdmaCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define ctxdmaGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define ctxdmaGetMemoryMappingDescriptor(pRmResource, ppMemDesc) ctxdmaGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define ctxdmaControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__
#define ctxdmaControlSerialization_Prologue(pResource, pCallContext, pParams) ctxdmaControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define ctxdmaControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__
#define ctxdmaControlSerialization_Epilogue(pResource, pCallContext, pParams) ctxdmaControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define ctxdmaControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__
#define ctxdmaControl_Prologue(pResource, pCallContext, pParams) ctxdmaControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define ctxdmaControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__
#define ctxdmaControl_Epilogue(pResource, pCallContext, pParams) ctxdmaControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define ctxdmaCanCopy_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define ctxdmaCanCopy(pResource) ctxdmaCanCopy_DISPATCH(pResource)
#define ctxdmaIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define ctxdmaIsDuplicate(pResource, hMemory, pDuplicate) ctxdmaIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define ctxdmaPreDestruct_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define ctxdmaPreDestruct(pResource) ctxdmaPreDestruct_DISPATCH(pResource)
#define ctxdmaControl_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__
#define ctxdmaControl(pResource, pCallContext, pParams) ctxdmaControl_DISPATCH(pResource, pCallContext, pParams)
#define ctxdmaControlFilter_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define ctxdmaControlFilter(pResource, pCallContext, pParams) ctxdmaControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define ctxdmaMap_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__
#define ctxdmaMap(pResource, pCallContext, pParams, pCpuMapping) ctxdmaMap_DISPATCH(pResource, pCallContext, pParams, pCpuMapping)
#define ctxdmaUnmap_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__
#define ctxdmaUnmap(pResource, pCallContext, pCpuMapping) ctxdmaUnmap_DISPATCH(pResource, pCallContext, pCpuMapping)
#define ctxdmaIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define ctxdmaIsPartialUnmapSupported(pResource) ctxdmaIsPartialUnmapSupported_DISPATCH(pResource)
#define ctxdmaGetRefCount_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define ctxdmaGetRefCount(pResource) ctxdmaGetRefCount_DISPATCH(pResource)
#define ctxdmaAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define ctxdmaAddAdditionalDependants(pClient, pResource, pReference) ctxdmaAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define ctxdmaGetNotificationListPtr_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetNotificationListPtr__
#define ctxdmaGetNotificationListPtr(pNotifier) ctxdmaGetNotificationListPtr_DISPATCH(pNotifier)
#define ctxdmaGetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetNotificationShare__
#define ctxdmaGetNotificationShare(pNotifier) ctxdmaGetNotificationShare_DISPATCH(pNotifier)
#define ctxdmaSetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifySetNotificationShare__
#define ctxdmaSetNotificationShare(pNotifier, pNotifShare) ctxdmaSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define ctxdmaUnregisterEvent_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyUnregisterEvent__
#define ctxdmaUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) ctxdmaUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define ctxdmaGetOrAllocNotifShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetOrAllocNotifShare__
#define ctxdmaGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) ctxdmaGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)

// Dispatch functions
static inline NV_STATUS ctxdmaMapTo_DISPATCH(struct ContextDma *pContextDma, struct RS_RES_MAP_TO_PARAMS *pParams) {
    return pContextDma->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__(staticCast(pContextDma, RsResource), pParams);
}

static inline NV_STATUS ctxdmaUnmapFrom_DISPATCH(struct ContextDma *pContextDma, struct RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pContextDma->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__(staticCast(pContextDma, RsResource), pParams);
}

static inline NvBool ctxdmaAccessCallback_DISPATCH(struct ContextDma *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__(staticCast(pResource, RsResource), pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool ctxdmaShareCallback_DISPATCH(struct ContextDma *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__(staticCast(pResource, RsResource), pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS ctxdmaGetMemInterMapParams_DISPATCH(struct ContextDma *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__(staticCast(pRmResource, RmResource), pParams);
}

static inline NV_STATUS ctxdmaCheckMemInterUnmap_DISPATCH(struct ContextDma *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__(staticCast(pRmResource, RmResource), bSubdeviceHandleProvided);
}

static inline NV_STATUS ctxdmaGetMemoryMappingDescriptor_DISPATCH(struct ContextDma *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__(staticCast(pRmResource, RmResource), ppMemDesc);
}

static inline NV_STATUS ctxdmaControlSerialization_Prologue_DISPATCH(struct ContextDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void ctxdmaControlSerialization_Epilogue_DISPATCH(struct ContextDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS ctxdmaControl_Prologue_DISPATCH(struct ContextDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void ctxdmaControl_Epilogue_DISPATCH(struct ContextDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool ctxdmaCanCopy_DISPATCH(struct ContextDma *pResource) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__(staticCast(pResource, RsResource));
}

static inline NV_STATUS ctxdmaIsDuplicate_DISPATCH(struct ContextDma *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__(staticCast(pResource, RsResource), hMemory, pDuplicate);
}

static inline void ctxdmaPreDestruct_DISPATCH(struct ContextDma *pResource) {
    pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__(staticCast(pResource, RsResource));
}

static inline NV_STATUS ctxdmaControl_DISPATCH(struct ContextDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS ctxdmaControlFilter_DISPATCH(struct ContextDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS ctxdmaMap_DISPATCH(struct ContextDma *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__(staticCast(pResource, RsResource), pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS ctxdmaUnmap_DISPATCH(struct ContextDma *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__(staticCast(pResource, RsResource), pCallContext, pCpuMapping);
}

static inline NvBool ctxdmaIsPartialUnmapSupported_DISPATCH(struct ContextDma *pResource) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__(staticCast(pResource, RsResource));
}

static inline NvU32 ctxdmaGetRefCount_DISPATCH(struct ContextDma *pResource) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__(staticCast(pResource, RsResource));
}

static inline void ctxdmaAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct ContextDma *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__(pClient, staticCast(pResource, RsResource), pReference);
}

static inline PEVENTNOTIFICATION * ctxdmaGetNotificationListPtr_DISPATCH(struct ContextDma *pNotifier) {
    return pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetNotificationListPtr__(staticCast(pNotifier, INotifier));
}

static inline struct NotifShare * ctxdmaGetNotificationShare_DISPATCH(struct ContextDma *pNotifier) {
    return pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetNotificationShare__(staticCast(pNotifier, INotifier));
}

static inline void ctxdmaSetNotificationShare_DISPATCH(struct ContextDma *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifySetNotificationShare__(staticCast(pNotifier, INotifier), pNotifShare);
}

static inline NV_STATUS ctxdmaUnregisterEvent_DISPATCH(struct ContextDma *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyUnregisterEvent__(staticCast(pNotifier, INotifier), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS ctxdmaGetOrAllocNotifShare_DISPATCH(struct ContextDma *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr->vtable.__inotifyGetOrAllocNotifShare__(staticCast(pNotifier, INotifier), hNotifierClient, hNotifierResource, ppNotifShare);
}

// Virtual method declarations and/or inline definitions
NV_STATUS ctxdmaMapTo_IMPL(struct ContextDma *pContextDma, struct RS_RES_MAP_TO_PARAMS *pParams);

NV_STATUS ctxdmaUnmapFrom_IMPL(struct ContextDma *pContextDma, struct RS_RES_UNMAP_FROM_PARAMS *pParams);

// Exported method declarations and/or inline definitions
NV_STATUS ctxdmaCtrlCmdUpdateContextdma_IMPL(struct ContextDma *pContextDma, NV0002_CTRL_UPDATE_CONTEXTDMA_PARAMS *pUpdateCtxtDmaParams);

NV_STATUS ctxdmaCtrlCmdBindContextdma_IMPL(struct ContextDma *pContextDma, NV0002_CTRL_BIND_CONTEXTDMA_PARAMS *pBindCtxtDmaParams);

NV_STATUS ctxdmaCtrlCmdUnbindContextdma_IMPL(struct ContextDma *pContextDma, NV0002_CTRL_UNBIND_CONTEXTDMA_PARAMS *pUnbindCtxtDmaParams);

// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


// ****************************************************************************
//                            Deprecated Definitions
// ****************************************************************************

#if RM_STRICT_CONFIG_EMIT_DEPRECATED_CONTEXT_DMA_DEFINITIONS == 1

/**
 * @warning This function is deprecated! Please use ctxdmaGetByHandle.
 */
NV_STATUS CliGetContextDma(NvHandle hClient, NvHandle hContextDma, struct ContextDma **);

#endif

#endif /* CONTEXT_DMA_H */

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_CONTEXT_DMA_NVOC_H_
