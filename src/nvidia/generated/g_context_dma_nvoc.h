
#ifndef _G_CONTEXT_DMA_NVOC_H_
#define _G_CONTEXT_DMA_NVOC_H_
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
#include "rmapi/event.h"
#include "ctrl/ctrl0002.h"
#include "rmapi/control.h" // for macro RMCTRL_EXPORT etc.
#include "nvlimits.h"


struct Device;

#ifndef __NVOC_CLASS_Device_TYPEDEF__
#define __NVOC_CLASS_Device_TYPEDEF__
typedef struct Device Device;
#endif /* __NVOC_CLASS_Device_TYPEDEF__ */

#ifndef __nvoc_class_id_Device
#define __nvoc_class_id_Device 0xe0ac20
#endif /* __nvoc_class_id_Device */



struct Memory;

#ifndef __NVOC_CLASS_Memory_TYPEDEF__
#define __NVOC_CLASS_Memory_TYPEDEF__
typedef struct Memory Memory;
#endif /* __NVOC_CLASS_Memory_TYPEDEF__ */

#ifndef __nvoc_class_id_Memory
#define __nvoc_class_id_Memory 0x4789f2
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


struct ContextDma {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
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

    // Vtable with 29 per-object function pointers
    NV_STATUS (*__ctxdmaMapTo__)(struct ContextDma * /*this*/, struct RS_RES_MAP_TO_PARAMS *);  // virtual override (res) base (rmres)
    NV_STATUS (*__ctxdmaUnmapFrom__)(struct ContextDma * /*this*/, struct RS_RES_UNMAP_FROM_PARAMS *);  // virtual override (res) base (rmres)
    NV_STATUS (*__ctxdmaCtrlCmdUpdateContextdma__)(struct ContextDma * /*this*/, NV0002_CTRL_UPDATE_CONTEXTDMA_PARAMS *);  // exported (id=0x20101)
    NV_STATUS (*__ctxdmaCtrlCmdBindContextdma__)(struct ContextDma * /*this*/, NV0002_CTRL_BIND_CONTEXTDMA_PARAMS *);  // exported (id=0x20102)
    NV_STATUS (*__ctxdmaCtrlCmdUnbindContextdma__)(struct ContextDma * /*this*/, NV0002_CTRL_UNBIND_CONTEXTDMA_PARAMS *);  // exported (id=0x20103)
    NvBool (*__ctxdmaAccessCallback__)(struct ContextDma * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (rmres)
    NvBool (*__ctxdmaShareCallback__)(struct ContextDma * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__ctxdmaGetMemInterMapParams__)(struct ContextDma * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__ctxdmaCheckMemInterUnmap__)(struct ContextDma * /*this*/, NvBool);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__ctxdmaGetMemoryMappingDescriptor__)(struct ContextDma * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__ctxdmaControlSerialization_Prologue__)(struct ContextDma * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (rmres)
    void (*__ctxdmaControlSerialization_Epilogue__)(struct ContextDma * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__ctxdmaControl_Prologue__)(struct ContextDma * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (rmres)
    void (*__ctxdmaControl_Epilogue__)(struct ContextDma * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (rmres)
    NvBool (*__ctxdmaCanCopy__)(struct ContextDma * /*this*/);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__ctxdmaIsDuplicate__)(struct ContextDma * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (rmres)
    void (*__ctxdmaPreDestruct__)(struct ContextDma * /*this*/);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__ctxdmaControl__)(struct ContextDma * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__ctxdmaControlFilter__)(struct ContextDma * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__ctxdmaMap__)(struct ContextDma * /*this*/, struct CALL_CONTEXT *, RS_CPU_MAP_PARAMS *, RsCpuMapping *);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__ctxdmaUnmap__)(struct ContextDma * /*this*/, struct CALL_CONTEXT *, RsCpuMapping *);  // virtual inherited (res) base (rmres)
    NvBool (*__ctxdmaIsPartialUnmapSupported__)(struct ContextDma * /*this*/);  // inline virtual inherited (res) base (rmres) body
    NvU32 (*__ctxdmaGetRefCount__)(struct ContextDma * /*this*/);  // virtual inherited (res) base (rmres)
    void (*__ctxdmaAddAdditionalDependants__)(struct RsClient *, struct ContextDma * /*this*/, RsResourceRef *);  // virtual inherited (res) base (rmres)
    PEVENTNOTIFICATION * (*__ctxdmaGetNotificationListPtr__)(struct ContextDma * /*this*/);  // virtual inherited (notify) base (notify)
    struct NotifShare * (*__ctxdmaGetNotificationShare__)(struct ContextDma * /*this*/);  // virtual inherited (notify) base (notify)
    void (*__ctxdmaSetNotificationShare__)(struct ContextDma * /*this*/, struct NotifShare *);  // virtual inherited (notify) base (notify)
    NV_STATUS (*__ctxdmaUnregisterEvent__)(struct ContextDma * /*this*/, NvHandle, NvHandle, NvHandle, NvHandle);  // virtual inherited (notify) base (notify)
    NV_STATUS (*__ctxdmaGetOrAllocNotifShare__)(struct ContextDma * /*this*/, NvHandle, NvHandle, struct NotifShare **);  // virtual inherited (notify) base (notify)

    // Data members
    NvU32 Class;
    NvU32 Flags;
    NvBool bReadOnly;
    NvU32 CacheSnoop;
    NvU32 Type;
    NvU64 Limit;
    NV_ADDRESS_SPACE AddressSpace;
    NvBool bUnicast;
    void *KernelVAddr[8];
    void *KernelPriv;
    NvU64 FbAperture[8];
    NvU64 FbApertureLen[8];
    struct Memory *pMemory;
    struct MEMORY_DESCRIPTOR *pMemDesc;
    NvU32 Instance[8];
    NvU32 InstRefCount[8];
    struct OBJGPU *pGpu;
    struct Device *pDevice;
};

#ifndef __NVOC_CLASS_ContextDma_TYPEDEF__
#define __NVOC_CLASS_ContextDma_TYPEDEF__
typedef struct ContextDma ContextDma;
#endif /* __NVOC_CLASS_ContextDma_TYPEDEF__ */

#ifndef __nvoc_class_id_ContextDma
#define __nvoc_class_id_ContextDma 0x88441b
#endif /* __nvoc_class_id_ContextDma */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_ContextDma;

#define __staticCast_ContextDma(pThis) \
    ((pThis)->__nvoc_pbase_ContextDma)

#ifdef __nvoc_context_dma_h_disabled
#define __dynamicCast_ContextDma(pThis) ((ContextDma*)NULL)
#else //__nvoc_context_dma_h_disabled
#define __dynamicCast_ContextDma(pThis) \
    ((ContextDma*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(ContextDma)))
#endif //__nvoc_context_dma_h_disabled

NV_STATUS __nvoc_objCreateDynamic_ContextDma(ContextDma**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_ContextDma(ContextDma**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_ContextDma(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_ContextDma((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define ctxdmaMapTo_FNPTR(pContextDma) pContextDma->__ctxdmaMapTo__
#define ctxdmaMapTo(pContextDma, pParams) ctxdmaMapTo_DISPATCH(pContextDma, pParams)
#define ctxdmaUnmapFrom_FNPTR(pContextDma) pContextDma->__ctxdmaUnmapFrom__
#define ctxdmaUnmapFrom(pContextDma, pParams) ctxdmaUnmapFrom_DISPATCH(pContextDma, pParams)
#define ctxdmaCtrlCmdUpdateContextdma_FNPTR(pContextDma) pContextDma->__ctxdmaCtrlCmdUpdateContextdma__
#define ctxdmaCtrlCmdUpdateContextdma(pContextDma, pUpdateCtxtDmaParams) ctxdmaCtrlCmdUpdateContextdma_DISPATCH(pContextDma, pUpdateCtxtDmaParams)
#define ctxdmaCtrlCmdBindContextdma_FNPTR(pContextDma) pContextDma->__ctxdmaCtrlCmdBindContextdma__
#define ctxdmaCtrlCmdBindContextdma(pContextDma, pBindCtxtDmaParams) ctxdmaCtrlCmdBindContextdma_DISPATCH(pContextDma, pBindCtxtDmaParams)
#define ctxdmaCtrlCmdUnbindContextdma_FNPTR(pContextDma) pContextDma->__ctxdmaCtrlCmdUnbindContextdma__
#define ctxdmaCtrlCmdUnbindContextdma(pContextDma, pUnbindCtxtDmaParams) ctxdmaCtrlCmdUnbindContextdma_DISPATCH(pContextDma, pUnbindCtxtDmaParams)
#define ctxdmaAccessCallback_FNPTR(pResource) pResource->__nvoc_base_RmResource.__rmresAccessCallback__
#define ctxdmaAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) ctxdmaAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define ctxdmaShareCallback_FNPTR(pResource) pResource->__nvoc_base_RmResource.__rmresShareCallback__
#define ctxdmaShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) ctxdmaShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define ctxdmaGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__rmresGetMemInterMapParams__
#define ctxdmaGetMemInterMapParams(pRmResource, pParams) ctxdmaGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define ctxdmaCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__rmresCheckMemInterUnmap__
#define ctxdmaCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) ctxdmaCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define ctxdmaGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__rmresGetMemoryMappingDescriptor__
#define ctxdmaGetMemoryMappingDescriptor(pRmResource, ppMemDesc) ctxdmaGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define ctxdmaControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__rmresControlSerialization_Prologue__
#define ctxdmaControlSerialization_Prologue(pResource, pCallContext, pParams) ctxdmaControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define ctxdmaControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__rmresControlSerialization_Epilogue__
#define ctxdmaControlSerialization_Epilogue(pResource, pCallContext, pParams) ctxdmaControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define ctxdmaControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__rmresControl_Prologue__
#define ctxdmaControl_Prologue(pResource, pCallContext, pParams) ctxdmaControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define ctxdmaControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__rmresControl_Epilogue__
#define ctxdmaControl_Epilogue(pResource, pCallContext, pParams) ctxdmaControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define ctxdmaCanCopy_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__
#define ctxdmaCanCopy(pResource) ctxdmaCanCopy_DISPATCH(pResource)
#define ctxdmaIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsDuplicate__
#define ctxdmaIsDuplicate(pResource, hMemory, pDuplicate) ctxdmaIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define ctxdmaPreDestruct_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resPreDestruct__
#define ctxdmaPreDestruct(pResource) ctxdmaPreDestruct_DISPATCH(pResource)
#define ctxdmaControl_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resControl__
#define ctxdmaControl(pResource, pCallContext, pParams) ctxdmaControl_DISPATCH(pResource, pCallContext, pParams)
#define ctxdmaControlFilter_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resControlFilter__
#define ctxdmaControlFilter(pResource, pCallContext, pParams) ctxdmaControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define ctxdmaMap_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resMap__
#define ctxdmaMap(pResource, pCallContext, pParams, pCpuMapping) ctxdmaMap_DISPATCH(pResource, pCallContext, pParams, pCpuMapping)
#define ctxdmaUnmap_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmap__
#define ctxdmaUnmap(pResource, pCallContext, pCpuMapping) ctxdmaUnmap_DISPATCH(pResource, pCallContext, pCpuMapping)
#define ctxdmaIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsPartialUnmapSupported__
#define ctxdmaIsPartialUnmapSupported(pResource) ctxdmaIsPartialUnmapSupported_DISPATCH(pResource)
#define ctxdmaGetRefCount_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resGetRefCount__
#define ctxdmaGetRefCount(pResource) ctxdmaGetRefCount_DISPATCH(pResource)
#define ctxdmaAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__resAddAdditionalDependants__
#define ctxdmaAddAdditionalDependants(pClient, pResource, pReference) ctxdmaAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define ctxdmaGetNotificationListPtr_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__notifyGetNotificationListPtr__
#define ctxdmaGetNotificationListPtr(pNotifier) ctxdmaGetNotificationListPtr_DISPATCH(pNotifier)
#define ctxdmaGetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__notifyGetNotificationShare__
#define ctxdmaGetNotificationShare(pNotifier) ctxdmaGetNotificationShare_DISPATCH(pNotifier)
#define ctxdmaSetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__notifySetNotificationShare__
#define ctxdmaSetNotificationShare(pNotifier, pNotifShare) ctxdmaSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define ctxdmaUnregisterEvent_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__notifyUnregisterEvent__
#define ctxdmaUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) ctxdmaUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define ctxdmaGetOrAllocNotifShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__notifyGetOrAllocNotifShare__
#define ctxdmaGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) ctxdmaGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)

// Dispatch functions
static inline NV_STATUS ctxdmaMapTo_DISPATCH(struct ContextDma *pContextDma, struct RS_RES_MAP_TO_PARAMS *pParams) {
    return pContextDma->__ctxdmaMapTo__(pContextDma, pParams);
}

static inline NV_STATUS ctxdmaUnmapFrom_DISPATCH(struct ContextDma *pContextDma, struct RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pContextDma->__ctxdmaUnmapFrom__(pContextDma, pParams);
}

static inline NV_STATUS ctxdmaCtrlCmdUpdateContextdma_DISPATCH(struct ContextDma *pContextDma, NV0002_CTRL_UPDATE_CONTEXTDMA_PARAMS *pUpdateCtxtDmaParams) {
    return pContextDma->__ctxdmaCtrlCmdUpdateContextdma__(pContextDma, pUpdateCtxtDmaParams);
}

static inline NV_STATUS ctxdmaCtrlCmdBindContextdma_DISPATCH(struct ContextDma *pContextDma, NV0002_CTRL_BIND_CONTEXTDMA_PARAMS *pBindCtxtDmaParams) {
    return pContextDma->__ctxdmaCtrlCmdBindContextdma__(pContextDma, pBindCtxtDmaParams);
}

static inline NV_STATUS ctxdmaCtrlCmdUnbindContextdma_DISPATCH(struct ContextDma *pContextDma, NV0002_CTRL_UNBIND_CONTEXTDMA_PARAMS *pUnbindCtxtDmaParams) {
    return pContextDma->__ctxdmaCtrlCmdUnbindContextdma__(pContextDma, pUnbindCtxtDmaParams);
}

static inline NvBool ctxdmaAccessCallback_DISPATCH(struct ContextDma *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__ctxdmaAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool ctxdmaShareCallback_DISPATCH(struct ContextDma *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__ctxdmaShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS ctxdmaGetMemInterMapParams_DISPATCH(struct ContextDma *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__ctxdmaGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS ctxdmaCheckMemInterUnmap_DISPATCH(struct ContextDma *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__ctxdmaCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS ctxdmaGetMemoryMappingDescriptor_DISPATCH(struct ContextDma *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__ctxdmaGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS ctxdmaControlSerialization_Prologue_DISPATCH(struct ContextDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__ctxdmaControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void ctxdmaControlSerialization_Epilogue_DISPATCH(struct ContextDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__ctxdmaControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS ctxdmaControl_Prologue_DISPATCH(struct ContextDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__ctxdmaControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void ctxdmaControl_Epilogue_DISPATCH(struct ContextDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__ctxdmaControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool ctxdmaCanCopy_DISPATCH(struct ContextDma *pResource) {
    return pResource->__ctxdmaCanCopy__(pResource);
}

static inline NV_STATUS ctxdmaIsDuplicate_DISPATCH(struct ContextDma *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__ctxdmaIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void ctxdmaPreDestruct_DISPATCH(struct ContextDma *pResource) {
    pResource->__ctxdmaPreDestruct__(pResource);
}

static inline NV_STATUS ctxdmaControl_DISPATCH(struct ContextDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__ctxdmaControl__(pResource, pCallContext, pParams);
}

static inline NV_STATUS ctxdmaControlFilter_DISPATCH(struct ContextDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__ctxdmaControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS ctxdmaMap_DISPATCH(struct ContextDma *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pResource->__ctxdmaMap__(pResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS ctxdmaUnmap_DISPATCH(struct ContextDma *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pResource->__ctxdmaUnmap__(pResource, pCallContext, pCpuMapping);
}

static inline NvBool ctxdmaIsPartialUnmapSupported_DISPATCH(struct ContextDma *pResource) {
    return pResource->__ctxdmaIsPartialUnmapSupported__(pResource);
}

static inline NvU32 ctxdmaGetRefCount_DISPATCH(struct ContextDma *pResource) {
    return pResource->__ctxdmaGetRefCount__(pResource);
}

static inline void ctxdmaAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct ContextDma *pResource, RsResourceRef *pReference) {
    pResource->__ctxdmaAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline PEVENTNOTIFICATION * ctxdmaGetNotificationListPtr_DISPATCH(struct ContextDma *pNotifier) {
    return pNotifier->__ctxdmaGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare * ctxdmaGetNotificationShare_DISPATCH(struct ContextDma *pNotifier) {
    return pNotifier->__ctxdmaGetNotificationShare__(pNotifier);
}

static inline void ctxdmaSetNotificationShare_DISPATCH(struct ContextDma *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__ctxdmaSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NV_STATUS ctxdmaUnregisterEvent_DISPATCH(struct ContextDma *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__ctxdmaUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS ctxdmaGetOrAllocNotifShare_DISPATCH(struct ContextDma *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__ctxdmaGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

NV_STATUS ctxdmaMapTo_IMPL(struct ContextDma *pContextDma, struct RS_RES_MAP_TO_PARAMS *pParams);

NV_STATUS ctxdmaUnmapFrom_IMPL(struct ContextDma *pContextDma, struct RS_RES_UNMAP_FROM_PARAMS *pParams);

NV_STATUS ctxdmaCtrlCmdUpdateContextdma_IMPL(struct ContextDma *pContextDma, NV0002_CTRL_UPDATE_CONTEXTDMA_PARAMS *pUpdateCtxtDmaParams);

NV_STATUS ctxdmaCtrlCmdBindContextdma_IMPL(struct ContextDma *pContextDma, NV0002_CTRL_BIND_CONTEXTDMA_PARAMS *pBindCtxtDmaParams);

NV_STATUS ctxdmaCtrlCmdUnbindContextdma_IMPL(struct ContextDma *pContextDma, NV0002_CTRL_UNBIND_CONTEXTDMA_PARAMS *pUnbindCtxtDmaParams);

NV_STATUS ctxdmaConstruct_IMPL(struct ContextDma *arg_pCtxdma, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_ctxdmaConstruct(arg_pCtxdma, arg_pCallContext, arg_pParams) ctxdmaConstruct_IMPL(arg_pCtxdma, arg_pCallContext, arg_pParams)
void ctxdmaDestruct_IMPL(struct ContextDma *pCtxdma);

#define __nvoc_ctxdmaDestruct(pCtxdma) ctxdmaDestruct_IMPL(pCtxdma)
NV_STATUS ctxdmaValidate_IMPL(struct ContextDma *pContextDma, NvU64 start, NvU64 len);

#ifdef __nvoc_context_dma_h_disabled
static inline NV_STATUS ctxdmaValidate(struct ContextDma *pContextDma, NvU64 start, NvU64 len) {
    NV_ASSERT_FAILED_PRECOMP("ContextDma was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_context_dma_h_disabled
#define ctxdmaValidate(pContextDma, start, len) ctxdmaValidate_IMPL(pContextDma, start, len)
#endif //__nvoc_context_dma_h_disabled

NV_STATUS ctxdmaGetKernelVA_IMPL(struct ContextDma *pContextDma, NvU64 start, NvU64 len, void **arg4, NvU32 VA_idx);

#ifdef __nvoc_context_dma_h_disabled
static inline NV_STATUS ctxdmaGetKernelVA(struct ContextDma *pContextDma, NvU64 start, NvU64 len, void **arg4, NvU32 VA_idx) {
    NV_ASSERT_FAILED_PRECOMP("ContextDma was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_context_dma_h_disabled
#define ctxdmaGetKernelVA(pContextDma, start, len, arg4, VA_idx) ctxdmaGetKernelVA_IMPL(pContextDma, start, len, arg4, VA_idx)
#endif //__nvoc_context_dma_h_disabled

NvBool ctxdmaIsBound_IMPL(struct ContextDma *pContextDma);

#ifdef __nvoc_context_dma_h_disabled
static inline NvBool ctxdmaIsBound(struct ContextDma *pContextDma) {
    NV_ASSERT_FAILED_PRECOMP("ContextDma was disabled!");
    return NV_FALSE;
}
#else //__nvoc_context_dma_h_disabled
#define ctxdmaIsBound(pContextDma) ctxdmaIsBound_IMPL(pContextDma)
#endif //__nvoc_context_dma_h_disabled

NV_STATUS ctxdmaGetByHandle_IMPL(struct RsClient *pClient, NvHandle hContextDma, struct ContextDma **arg3);

#define ctxdmaGetByHandle(pClient, hContextDma, arg3) ctxdmaGetByHandle_IMPL(pClient, hContextDma, arg3)
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
