#ifndef _G_CONTEXT_DMA_NVOC_H_
#define _G_CONTEXT_DMA_NVOC_H_
#include "nvoc/runtime.h"

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
#ifdef NVOC_CONTEXT_DMA_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct ContextDma {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct RmResource __nvoc_base_RmResource;
    struct Notifier __nvoc_base_Notifier;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct INotifier *__nvoc_pbase_INotifier;
    struct Notifier *__nvoc_pbase_Notifier;
    struct ContextDma *__nvoc_pbase_ContextDma;
    NV_STATUS (*__ctxdmaValidate__)(struct ContextDma *, NvU64, NvU64);
    NV_STATUS (*__ctxdmaGetKernelVA__)(struct ContextDma *, NvU64, NvU64, void **, NvU32);
    NV_STATUS (*__ctxdmaMapTo__)(struct ContextDma *, struct RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__ctxdmaUnmapFrom__)(struct ContextDma *, struct RS_RES_UNMAP_FROM_PARAMS *);
    NV_STATUS (*__ctxdmaCtrlCmdUpdateContextdma__)(struct ContextDma *, NV0002_CTRL_UPDATE_CONTEXTDMA_PARAMS *);
    NV_STATUS (*__ctxdmaCtrlCmdBindContextdma__)(struct ContextDma *, NV0002_CTRL_BIND_CONTEXTDMA_PARAMS *);
    NV_STATUS (*__ctxdmaCtrlCmdUnbindContextdma__)(struct ContextDma *, NV0002_CTRL_UNBIND_CONTEXTDMA_PARAMS *);
    NvBool (*__ctxdmaShareCallback__)(struct ContextDma *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__ctxdmaCheckMemInterUnmap__)(struct ContextDma *, NvBool);
    NvBool (*__ctxdmaAccessCallback__)(struct ContextDma *, struct RsClient *, void *, RsAccessRight);
    NV_STATUS (*__ctxdmaGetMemInterMapParams__)(struct ContextDma *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__ctxdmaGetMemoryMappingDescriptor__)(struct ContextDma *, struct MEMORY_DESCRIPTOR **);
    void (*__ctxdmaSetNotificationShare__)(struct ContextDma *, struct NotifShare *);
    NV_STATUS (*__ctxdmaControl__)(struct ContextDma *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__ctxdmaControlFilter__)(struct ContextDma *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvU32 (*__ctxdmaGetRefCount__)(struct ContextDma *);
    NV_STATUS (*__ctxdmaUnregisterEvent__)(struct ContextDma *, NvHandle, NvHandle, NvHandle, NvHandle);
    NV_STATUS (*__ctxdmaUnmap__)(struct ContextDma *, struct CALL_CONTEXT *, RsCpuMapping *);
    NvBool (*__ctxdmaCanCopy__)(struct ContextDma *);
    NV_STATUS (*__ctxdmaControl_Prologue__)(struct ContextDma *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    void (*__ctxdmaAddAdditionalDependants__)(struct RsClient *, struct ContextDma *, RsResourceRef *);
    void (*__ctxdmaPreDestruct__)(struct ContextDma *);
    PEVENTNOTIFICATION *(*__ctxdmaGetNotificationListPtr__)(struct ContextDma *);
    void (*__ctxdmaControl_Epilogue__)(struct ContextDma *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    struct NotifShare *(*__ctxdmaGetNotificationShare__)(struct ContextDma *);
    NV_STATUS (*__ctxdmaControlLookup__)(struct ContextDma *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NV_STATUS (*__ctxdmaMap__)(struct ContextDma *, struct CALL_CONTEXT *, RS_CPU_MAP_PARAMS *, RsCpuMapping *);
    NV_STATUS (*__ctxdmaGetOrAllocNotifShare__)(struct ContextDma *, NvHandle, NvHandle, struct NotifShare **);
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

#define ctxdmaValidate(pContextDma, start, len) ctxdmaValidate_DISPATCH(pContextDma, start, len)
#define ctxdmaGetKernelVA(pContextDma, start, len, arg0, VA_idx) ctxdmaGetKernelVA_DISPATCH(pContextDma, start, len, arg0, VA_idx)
#define ctxdmaMapTo(pContextDma, pParams) ctxdmaMapTo_DISPATCH(pContextDma, pParams)
#define ctxdmaUnmapFrom(pContextDma, pParams) ctxdmaUnmapFrom_DISPATCH(pContextDma, pParams)
#define ctxdmaCtrlCmdUpdateContextdma(pContextDma, pUpdateCtxtDmaParams) ctxdmaCtrlCmdUpdateContextdma_DISPATCH(pContextDma, pUpdateCtxtDmaParams)
#define ctxdmaCtrlCmdBindContextdma(pContextDma, pBindCtxtDmaParams) ctxdmaCtrlCmdBindContextdma_DISPATCH(pContextDma, pBindCtxtDmaParams)
#define ctxdmaCtrlCmdUnbindContextdma(pContextDma, pUnbindCtxtDmaParams) ctxdmaCtrlCmdUnbindContextdma_DISPATCH(pContextDma, pUnbindCtxtDmaParams)
#define ctxdmaShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) ctxdmaShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define ctxdmaCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) ctxdmaCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define ctxdmaAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) ctxdmaAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define ctxdmaGetMemInterMapParams(pRmResource, pParams) ctxdmaGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define ctxdmaGetMemoryMappingDescriptor(pRmResource, ppMemDesc) ctxdmaGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define ctxdmaSetNotificationShare(pNotifier, pNotifShare) ctxdmaSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define ctxdmaControl(pResource, pCallContext, pParams) ctxdmaControl_DISPATCH(pResource, pCallContext, pParams)
#define ctxdmaControlFilter(pResource, pCallContext, pParams) ctxdmaControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define ctxdmaGetRefCount(pResource) ctxdmaGetRefCount_DISPATCH(pResource)
#define ctxdmaUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) ctxdmaUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define ctxdmaUnmap(pResource, pCallContext, pCpuMapping) ctxdmaUnmap_DISPATCH(pResource, pCallContext, pCpuMapping)
#define ctxdmaCanCopy(pResource) ctxdmaCanCopy_DISPATCH(pResource)
#define ctxdmaControl_Prologue(pResource, pCallContext, pParams) ctxdmaControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define ctxdmaAddAdditionalDependants(pClient, pResource, pReference) ctxdmaAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define ctxdmaPreDestruct(pResource) ctxdmaPreDestruct_DISPATCH(pResource)
#define ctxdmaGetNotificationListPtr(pNotifier) ctxdmaGetNotificationListPtr_DISPATCH(pNotifier)
#define ctxdmaControl_Epilogue(pResource, pCallContext, pParams) ctxdmaControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define ctxdmaGetNotificationShare(pNotifier) ctxdmaGetNotificationShare_DISPATCH(pNotifier)
#define ctxdmaControlLookup(pResource, pParams, ppEntry) ctxdmaControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define ctxdmaMap(pResource, pCallContext, pParams, pCpuMapping) ctxdmaMap_DISPATCH(pResource, pCallContext, pParams, pCpuMapping)
#define ctxdmaGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) ctxdmaGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)
NV_STATUS ctxdmaValidate_IMPL(struct ContextDma *pContextDma, NvU64 start, NvU64 len);

static inline NV_STATUS ctxdmaValidate_DISPATCH(struct ContextDma *pContextDma, NvU64 start, NvU64 len) {
    return pContextDma->__ctxdmaValidate__(pContextDma, start, len);
}

NV_STATUS ctxdmaGetKernelVA_IMPL(struct ContextDma *pContextDma, NvU64 start, NvU64 len, void **arg0, NvU32 VA_idx);

static inline NV_STATUS ctxdmaGetKernelVA_DISPATCH(struct ContextDma *pContextDma, NvU64 start, NvU64 len, void **arg0, NvU32 VA_idx) {
    return pContextDma->__ctxdmaGetKernelVA__(pContextDma, start, len, arg0, VA_idx);
}

NV_STATUS ctxdmaMapTo_IMPL(struct ContextDma *pContextDma, struct RS_RES_MAP_TO_PARAMS *pParams);

static inline NV_STATUS ctxdmaMapTo_DISPATCH(struct ContextDma *pContextDma, struct RS_RES_MAP_TO_PARAMS *pParams) {
    return pContextDma->__ctxdmaMapTo__(pContextDma, pParams);
}

NV_STATUS ctxdmaUnmapFrom_IMPL(struct ContextDma *pContextDma, struct RS_RES_UNMAP_FROM_PARAMS *pParams);

static inline NV_STATUS ctxdmaUnmapFrom_DISPATCH(struct ContextDma *pContextDma, struct RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pContextDma->__ctxdmaUnmapFrom__(pContextDma, pParams);
}

NV_STATUS ctxdmaCtrlCmdUpdateContextdma_IMPL(struct ContextDma *pContextDma, NV0002_CTRL_UPDATE_CONTEXTDMA_PARAMS *pUpdateCtxtDmaParams);

static inline NV_STATUS ctxdmaCtrlCmdUpdateContextdma_DISPATCH(struct ContextDma *pContextDma, NV0002_CTRL_UPDATE_CONTEXTDMA_PARAMS *pUpdateCtxtDmaParams) {
    return pContextDma->__ctxdmaCtrlCmdUpdateContextdma__(pContextDma, pUpdateCtxtDmaParams);
}

NV_STATUS ctxdmaCtrlCmdBindContextdma_IMPL(struct ContextDma *pContextDma, NV0002_CTRL_BIND_CONTEXTDMA_PARAMS *pBindCtxtDmaParams);

static inline NV_STATUS ctxdmaCtrlCmdBindContextdma_DISPATCH(struct ContextDma *pContextDma, NV0002_CTRL_BIND_CONTEXTDMA_PARAMS *pBindCtxtDmaParams) {
    return pContextDma->__ctxdmaCtrlCmdBindContextdma__(pContextDma, pBindCtxtDmaParams);
}

NV_STATUS ctxdmaCtrlCmdUnbindContextdma_IMPL(struct ContextDma *pContextDma, NV0002_CTRL_UNBIND_CONTEXTDMA_PARAMS *pUnbindCtxtDmaParams);

static inline NV_STATUS ctxdmaCtrlCmdUnbindContextdma_DISPATCH(struct ContextDma *pContextDma, NV0002_CTRL_UNBIND_CONTEXTDMA_PARAMS *pUnbindCtxtDmaParams) {
    return pContextDma->__ctxdmaCtrlCmdUnbindContextdma__(pContextDma, pUnbindCtxtDmaParams);
}

static inline NvBool ctxdmaShareCallback_DISPATCH(struct ContextDma *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__ctxdmaShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS ctxdmaCheckMemInterUnmap_DISPATCH(struct ContextDma *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__ctxdmaCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NvBool ctxdmaAccessCallback_DISPATCH(struct ContextDma *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__ctxdmaAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS ctxdmaGetMemInterMapParams_DISPATCH(struct ContextDma *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__ctxdmaGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS ctxdmaGetMemoryMappingDescriptor_DISPATCH(struct ContextDma *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__ctxdmaGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline void ctxdmaSetNotificationShare_DISPATCH(struct ContextDma *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__ctxdmaSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NV_STATUS ctxdmaControl_DISPATCH(struct ContextDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__ctxdmaControl__(pResource, pCallContext, pParams);
}

static inline NV_STATUS ctxdmaControlFilter_DISPATCH(struct ContextDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__ctxdmaControlFilter__(pResource, pCallContext, pParams);
}

static inline NvU32 ctxdmaGetRefCount_DISPATCH(struct ContextDma *pResource) {
    return pResource->__ctxdmaGetRefCount__(pResource);
}

static inline NV_STATUS ctxdmaUnregisterEvent_DISPATCH(struct ContextDma *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__ctxdmaUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS ctxdmaUnmap_DISPATCH(struct ContextDma *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pResource->__ctxdmaUnmap__(pResource, pCallContext, pCpuMapping);
}

static inline NvBool ctxdmaCanCopy_DISPATCH(struct ContextDma *pResource) {
    return pResource->__ctxdmaCanCopy__(pResource);
}

static inline NV_STATUS ctxdmaControl_Prologue_DISPATCH(struct ContextDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__ctxdmaControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void ctxdmaAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct ContextDma *pResource, RsResourceRef *pReference) {
    pResource->__ctxdmaAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline void ctxdmaPreDestruct_DISPATCH(struct ContextDma *pResource) {
    pResource->__ctxdmaPreDestruct__(pResource);
}

static inline PEVENTNOTIFICATION *ctxdmaGetNotificationListPtr_DISPATCH(struct ContextDma *pNotifier) {
    return pNotifier->__ctxdmaGetNotificationListPtr__(pNotifier);
}

static inline void ctxdmaControl_Epilogue_DISPATCH(struct ContextDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__ctxdmaControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline struct NotifShare *ctxdmaGetNotificationShare_DISPATCH(struct ContextDma *pNotifier) {
    return pNotifier->__ctxdmaGetNotificationShare__(pNotifier);
}

static inline NV_STATUS ctxdmaControlLookup_DISPATCH(struct ContextDma *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__ctxdmaControlLookup__(pResource, pParams, ppEntry);
}

static inline NV_STATUS ctxdmaMap_DISPATCH(struct ContextDma *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pResource->__ctxdmaMap__(pResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS ctxdmaGetOrAllocNotifShare_DISPATCH(struct ContextDma *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__ctxdmaGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

NV_STATUS ctxdmaConstruct_IMPL(struct ContextDma *arg_pCtxdma, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __nvoc_ctxdmaConstruct(arg_pCtxdma, arg_pCallContext, arg_pParams) ctxdmaConstruct_IMPL(arg_pCtxdma, arg_pCallContext, arg_pParams)
void ctxdmaDestruct_IMPL(struct ContextDma *pCtxdma);
#define __nvoc_ctxdmaDestruct(pCtxdma) ctxdmaDestruct_IMPL(pCtxdma)
NvBool ctxdmaIsBound_IMPL(struct ContextDma *pContextDma);
#ifdef __nvoc_context_dma_h_disabled
static inline NvBool ctxdmaIsBound(struct ContextDma *pContextDma) {
    NV_ASSERT_FAILED_PRECOMP("ContextDma was disabled!");
    return NV_FALSE;
}
#else //__nvoc_context_dma_h_disabled
#define ctxdmaIsBound(pContextDma) ctxdmaIsBound_IMPL(pContextDma)
#endif //__nvoc_context_dma_h_disabled

NV_STATUS ctxdmaGetByHandle_IMPL(struct RsClient *pClient, NvHandle hContextDma, struct ContextDma **arg0);
#define ctxdmaGetByHandle(pClient, hContextDma, arg0) ctxdmaGetByHandle_IMPL(pClient, hContextDma, arg0)
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
