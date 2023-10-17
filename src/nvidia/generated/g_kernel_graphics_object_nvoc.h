#ifndef _G_KERNEL_GRAPHICS_OBJECT_NVOC_H_
#define _G_KERNEL_GRAPHICS_OBJECT_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_kernel_graphics_object_nvoc.h"

#ifndef _KERNEL_GRAPHICS_OBJECT_H_
#define _KERNEL_GRAPHICS_OBJECT_H_

#include "core/core.h"
#include "kernel/gpu/fifo/channel_descendant.h"
#include "ctrl/ctrl83de/ctrl83dedebug.h"
#include "mem_mgr/vaddr_list.h"
#include "utils/nv_enum.h"
#include "kernel/gpu/gr/kernel_graphics_context.h"

/*!
 * RM internal class representing 3D and compute graphics classes, e.g.: <arch>_A,
 * <arch>_COMPUTE_A, etc
 */
#ifdef NVOC_KERNEL_GRAPHICS_OBJECT_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct KernelGraphicsObject {
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
    struct KernelGraphicsObject *__nvoc_pbase_KernelGraphicsObject;
    NV_STATUS (*__kgrobjGetMemInterMapParams__)(struct KernelGraphicsObject *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__kgrobjCheckMemInterUnmap__)(struct KernelGraphicsObject *, NvBool);
    NvBool (*__kgrobjShareCallback__)(struct KernelGraphicsObject *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NvBool (*__kgrobjAccessCallback__)(struct KernelGraphicsObject *, struct RsClient *, void *, RsAccessRight);
    NV_STATUS (*__kgrobjMapTo__)(struct KernelGraphicsObject *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__kgrobjGetMapAddrSpace__)(struct KernelGraphicsObject *, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    void (*__kgrobjSetNotificationShare__)(struct KernelGraphicsObject *, struct NotifShare *);
    NvU32 (*__kgrobjGetRefCount__)(struct KernelGraphicsObject *);
    void (*__kgrobjAddAdditionalDependants__)(struct RsClient *, struct KernelGraphicsObject *, RsResourceRef *);
    NV_STATUS (*__kgrobjControl_Prologue__)(struct KernelGraphicsObject *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__kgrobjGetRegBaseOffsetAndSize__)(struct KernelGraphicsObject *, struct OBJGPU *, NvU32 *, NvU32 *);
    NV_STATUS (*__kgrobjInternalControlForward__)(struct KernelGraphicsObject *, NvU32, void *, NvU32);
    NV_STATUS (*__kgrobjUnmapFrom__)(struct KernelGraphicsObject *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__kgrobjControl_Epilogue__)(struct KernelGraphicsObject *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__kgrobjControlLookup__)(struct KernelGraphicsObject *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NV_STATUS (*__kgrobjGetSwMethods__)(struct KernelGraphicsObject *, const METHOD **, NvU32 *);
    NvHandle (*__kgrobjGetInternalObjectHandle__)(struct KernelGraphicsObject *);
    NV_STATUS (*__kgrobjControl__)(struct KernelGraphicsObject *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__kgrobjUnmap__)(struct KernelGraphicsObject *, struct CALL_CONTEXT *, struct RsCpuMapping *);
    NV_STATUS (*__kgrobjGetMemoryMappingDescriptor__)(struct KernelGraphicsObject *, struct MEMORY_DESCRIPTOR **);
    NvBool (*__kgrobjIsSwMethodStalling__)(struct KernelGraphicsObject *, NvU32);
    NV_STATUS (*__kgrobjControlFilter__)(struct KernelGraphicsObject *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__kgrobjUnregisterEvent__)(struct KernelGraphicsObject *, NvHandle, NvHandle, NvHandle, NvHandle);
    NV_STATUS (*__kgrobjControlSerialization_Prologue__)(struct KernelGraphicsObject *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__kgrobjCanCopy__)(struct KernelGraphicsObject *);
    void (*__kgrobjPreDestruct__)(struct KernelGraphicsObject *);
    NV_STATUS (*__kgrobjIsDuplicate__)(struct KernelGraphicsObject *, NvHandle, NvBool *);
    void (*__kgrobjControlSerialization_Epilogue__)(struct KernelGraphicsObject *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    PEVENTNOTIFICATION *(*__kgrobjGetNotificationListPtr__)(struct KernelGraphicsObject *);
    struct NotifShare *(*__kgrobjGetNotificationShare__)(struct KernelGraphicsObject *);
    NV_STATUS (*__kgrobjMap__)(struct KernelGraphicsObject *, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);
    NV_STATUS (*__kgrobjGetOrAllocNotifShare__)(struct KernelGraphicsObject *, NvHandle, NvHandle, struct NotifShare **);
    MEMORY_DESCRIPTOR *PRIVATE_FIELD(pMmioMemDesc);
    struct KernelGraphicsContext *PRIVATE_FIELD(pKernelGraphicsContext);
};

struct KernelGraphicsObject_PRIVATE {
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
    struct KernelGraphicsObject *__nvoc_pbase_KernelGraphicsObject;
    NV_STATUS (*__kgrobjGetMemInterMapParams__)(struct KernelGraphicsObject *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__kgrobjCheckMemInterUnmap__)(struct KernelGraphicsObject *, NvBool);
    NvBool (*__kgrobjShareCallback__)(struct KernelGraphicsObject *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NvBool (*__kgrobjAccessCallback__)(struct KernelGraphicsObject *, struct RsClient *, void *, RsAccessRight);
    NV_STATUS (*__kgrobjMapTo__)(struct KernelGraphicsObject *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__kgrobjGetMapAddrSpace__)(struct KernelGraphicsObject *, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    void (*__kgrobjSetNotificationShare__)(struct KernelGraphicsObject *, struct NotifShare *);
    NvU32 (*__kgrobjGetRefCount__)(struct KernelGraphicsObject *);
    void (*__kgrobjAddAdditionalDependants__)(struct RsClient *, struct KernelGraphicsObject *, RsResourceRef *);
    NV_STATUS (*__kgrobjControl_Prologue__)(struct KernelGraphicsObject *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__kgrobjGetRegBaseOffsetAndSize__)(struct KernelGraphicsObject *, struct OBJGPU *, NvU32 *, NvU32 *);
    NV_STATUS (*__kgrobjInternalControlForward__)(struct KernelGraphicsObject *, NvU32, void *, NvU32);
    NV_STATUS (*__kgrobjUnmapFrom__)(struct KernelGraphicsObject *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__kgrobjControl_Epilogue__)(struct KernelGraphicsObject *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__kgrobjControlLookup__)(struct KernelGraphicsObject *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NV_STATUS (*__kgrobjGetSwMethods__)(struct KernelGraphicsObject *, const METHOD **, NvU32 *);
    NvHandle (*__kgrobjGetInternalObjectHandle__)(struct KernelGraphicsObject *);
    NV_STATUS (*__kgrobjControl__)(struct KernelGraphicsObject *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__kgrobjUnmap__)(struct KernelGraphicsObject *, struct CALL_CONTEXT *, struct RsCpuMapping *);
    NV_STATUS (*__kgrobjGetMemoryMappingDescriptor__)(struct KernelGraphicsObject *, struct MEMORY_DESCRIPTOR **);
    NvBool (*__kgrobjIsSwMethodStalling__)(struct KernelGraphicsObject *, NvU32);
    NV_STATUS (*__kgrobjControlFilter__)(struct KernelGraphicsObject *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__kgrobjUnregisterEvent__)(struct KernelGraphicsObject *, NvHandle, NvHandle, NvHandle, NvHandle);
    NV_STATUS (*__kgrobjControlSerialization_Prologue__)(struct KernelGraphicsObject *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__kgrobjCanCopy__)(struct KernelGraphicsObject *);
    void (*__kgrobjPreDestruct__)(struct KernelGraphicsObject *);
    NV_STATUS (*__kgrobjIsDuplicate__)(struct KernelGraphicsObject *, NvHandle, NvBool *);
    void (*__kgrobjControlSerialization_Epilogue__)(struct KernelGraphicsObject *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    PEVENTNOTIFICATION *(*__kgrobjGetNotificationListPtr__)(struct KernelGraphicsObject *);
    struct NotifShare *(*__kgrobjGetNotificationShare__)(struct KernelGraphicsObject *);
    NV_STATUS (*__kgrobjMap__)(struct KernelGraphicsObject *, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);
    NV_STATUS (*__kgrobjGetOrAllocNotifShare__)(struct KernelGraphicsObject *, NvHandle, NvHandle, struct NotifShare **);
    MEMORY_DESCRIPTOR *pMmioMemDesc;
    struct KernelGraphicsContext *pKernelGraphicsContext;
};

#ifndef __NVOC_CLASS_KernelGraphicsObject_TYPEDEF__
#define __NVOC_CLASS_KernelGraphicsObject_TYPEDEF__
typedef struct KernelGraphicsObject KernelGraphicsObject;
#endif /* __NVOC_CLASS_KernelGraphicsObject_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelGraphicsObject
#define __nvoc_class_id_KernelGraphicsObject 0x097648
#endif /* __nvoc_class_id_KernelGraphicsObject */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelGraphicsObject;

#define __staticCast_KernelGraphicsObject(pThis) \
    ((pThis)->__nvoc_pbase_KernelGraphicsObject)

#ifdef __nvoc_kernel_graphics_object_h_disabled
#define __dynamicCast_KernelGraphicsObject(pThis) ((KernelGraphicsObject*)NULL)
#else //__nvoc_kernel_graphics_object_h_disabled
#define __dynamicCast_KernelGraphicsObject(pThis) \
    ((KernelGraphicsObject*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelGraphicsObject)))
#endif //__nvoc_kernel_graphics_object_h_disabled


NV_STATUS __nvoc_objCreateDynamic_KernelGraphicsObject(KernelGraphicsObject**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelGraphicsObject(KernelGraphicsObject**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_KernelGraphicsObject(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_KernelGraphicsObject((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define kgrobjGetMemInterMapParams(arg0, arg1) kgrobjGetMemInterMapParams_DISPATCH(arg0, arg1)
#define kgrobjCheckMemInterUnmap(pChannelDescendant, bSubdeviceHandleProvided) kgrobjCheckMemInterUnmap_DISPATCH(pChannelDescendant, bSubdeviceHandleProvided)
#define kgrobjShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) kgrobjShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define kgrobjAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) kgrobjAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define kgrobjMapTo(pResource, pParams) kgrobjMapTo_DISPATCH(pResource, pParams)
#define kgrobjGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) kgrobjGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define kgrobjSetNotificationShare(pNotifier, pNotifShare) kgrobjSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define kgrobjGetRefCount(pResource) kgrobjGetRefCount_DISPATCH(pResource)
#define kgrobjAddAdditionalDependants(pClient, pResource, pReference) kgrobjAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define kgrobjControl_Prologue(pResource, pCallContext, pParams) kgrobjControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define kgrobjGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) kgrobjGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define kgrobjInternalControlForward(pGpuResource, command, pParams, size) kgrobjInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define kgrobjUnmapFrom(pResource, pParams) kgrobjUnmapFrom_DISPATCH(pResource, pParams)
#define kgrobjControl_Epilogue(pResource, pCallContext, pParams) kgrobjControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define kgrobjControlLookup(pResource, pParams, ppEntry) kgrobjControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define kgrobjGetSwMethods(pChannelDescendant, ppMethods, pNumMethods) kgrobjGetSwMethods_DISPATCH(pChannelDescendant, ppMethods, pNumMethods)
#define kgrobjGetInternalObjectHandle(pGpuResource) kgrobjGetInternalObjectHandle_DISPATCH(pGpuResource)
#define kgrobjControl(pGpuResource, pCallContext, pParams) kgrobjControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define kgrobjUnmap(pGpuResource, pCallContext, pCpuMapping) kgrobjUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define kgrobjGetMemoryMappingDescriptor(pRmResource, ppMemDesc) kgrobjGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define kgrobjIsSwMethodStalling(pChannelDescendant, hHandle) kgrobjIsSwMethodStalling_DISPATCH(pChannelDescendant, hHandle)
#define kgrobjControlFilter(pResource, pCallContext, pParams) kgrobjControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define kgrobjUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) kgrobjUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define kgrobjControlSerialization_Prologue(pResource, pCallContext, pParams) kgrobjControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define kgrobjCanCopy(pResource) kgrobjCanCopy_DISPATCH(pResource)
#define kgrobjPreDestruct(pResource) kgrobjPreDestruct_DISPATCH(pResource)
#define kgrobjIsDuplicate(pResource, hMemory, pDuplicate) kgrobjIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define kgrobjControlSerialization_Epilogue(pResource, pCallContext, pParams) kgrobjControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define kgrobjGetNotificationListPtr(pNotifier) kgrobjGetNotificationListPtr_DISPATCH(pNotifier)
#define kgrobjGetNotificationShare(pNotifier) kgrobjGetNotificationShare_DISPATCH(pNotifier)
#define kgrobjMap(pGpuResource, pCallContext, pParams, pCpuMapping) kgrobjMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define kgrobjGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) kgrobjGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)
void kgrobjGetPromoteIds_FWCLIENT(struct OBJGPU *arg0, struct KernelGraphicsObject *arg1, NvU32 maxPromoteIds, NvU32 *pPromoteIds, NvU32 *pNumEntries, NvBool *pbPromote);


#ifdef __nvoc_kernel_graphics_object_h_disabled
static inline void kgrobjGetPromoteIds(struct OBJGPU *arg0, struct KernelGraphicsObject *arg1, NvU32 maxPromoteIds, NvU32 *pPromoteIds, NvU32 *pNumEntries, NvBool *pbPromote) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsObject was disabled!");
}
#else //__nvoc_kernel_graphics_object_h_disabled
#define kgrobjGetPromoteIds(arg0, arg1, maxPromoteIds, pPromoteIds, pNumEntries, pbPromote) kgrobjGetPromoteIds_FWCLIENT(arg0, arg1, maxPromoteIds, pPromoteIds, pNumEntries, pbPromote)
#endif //__nvoc_kernel_graphics_object_h_disabled

#define kgrobjGetPromoteIds_HAL(arg0, arg1, maxPromoteIds, pPromoteIds, pNumEntries, pbPromote) kgrobjGetPromoteIds(arg0, arg1, maxPromoteIds, pPromoteIds, pNumEntries, pbPromote)

NvBool kgrobjShouldCleanup_KERNEL(struct OBJGPU *arg0, struct KernelGraphicsObject *arg1);


#ifdef __nvoc_kernel_graphics_object_h_disabled
static inline NvBool kgrobjShouldCleanup(struct OBJGPU *arg0, struct KernelGraphicsObject *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsObject was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_graphics_object_h_disabled
#define kgrobjShouldCleanup(arg0, arg1) kgrobjShouldCleanup_KERNEL(arg0, arg1)
#endif //__nvoc_kernel_graphics_object_h_disabled

#define kgrobjShouldCleanup_HAL(arg0, arg1) kgrobjShouldCleanup(arg0, arg1)

NV_STATUS kgrobjSetComputeMmio_IMPL(struct OBJGPU *arg0, struct KernelGraphicsObject *arg1);


#ifdef __nvoc_kernel_graphics_object_h_disabled
static inline NV_STATUS kgrobjSetComputeMmio(struct OBJGPU *arg0, struct KernelGraphicsObject *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsObject was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_object_h_disabled
#define kgrobjSetComputeMmio(arg0, arg1) kgrobjSetComputeMmio_IMPL(arg0, arg1)
#endif //__nvoc_kernel_graphics_object_h_disabled

#define kgrobjSetComputeMmio_HAL(arg0, arg1) kgrobjSetComputeMmio(arg0, arg1)

void kgrobjFreeComputeMmio_IMPL(struct OBJGPU *arg0, struct KernelGraphicsObject *arg1);


#ifdef __nvoc_kernel_graphics_object_h_disabled
static inline void kgrobjFreeComputeMmio(struct OBJGPU *arg0, struct KernelGraphicsObject *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsObject was disabled!");
}
#else //__nvoc_kernel_graphics_object_h_disabled
#define kgrobjFreeComputeMmio(arg0, arg1) kgrobjFreeComputeMmio_IMPL(arg0, arg1)
#endif //__nvoc_kernel_graphics_object_h_disabled

#define kgrobjFreeComputeMmio_HAL(arg0, arg1) kgrobjFreeComputeMmio(arg0, arg1)

NV_STATUS kgrobjGetMemInterMapParams_IMPL(struct KernelGraphicsObject *arg0, RMRES_MEM_INTER_MAP_PARAMS *arg1);

static inline NV_STATUS kgrobjGetMemInterMapParams_DISPATCH(struct KernelGraphicsObject *arg0, RMRES_MEM_INTER_MAP_PARAMS *arg1) {
    return arg0->__kgrobjGetMemInterMapParams__(arg0, arg1);
}

static inline NV_STATUS kgrobjCheckMemInterUnmap_DISPATCH(struct KernelGraphicsObject *pChannelDescendant, NvBool bSubdeviceHandleProvided) {
    return pChannelDescendant->__kgrobjCheckMemInterUnmap__(pChannelDescendant, bSubdeviceHandleProvided);
}

static inline NvBool kgrobjShareCallback_DISPATCH(struct KernelGraphicsObject *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__kgrobjShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NvBool kgrobjAccessCallback_DISPATCH(struct KernelGraphicsObject *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__kgrobjAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS kgrobjMapTo_DISPATCH(struct KernelGraphicsObject *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__kgrobjMapTo__(pResource, pParams);
}

static inline NV_STATUS kgrobjGetMapAddrSpace_DISPATCH(struct KernelGraphicsObject *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__kgrobjGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline void kgrobjSetNotificationShare_DISPATCH(struct KernelGraphicsObject *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__kgrobjSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NvU32 kgrobjGetRefCount_DISPATCH(struct KernelGraphicsObject *pResource) {
    return pResource->__kgrobjGetRefCount__(pResource);
}

static inline void kgrobjAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct KernelGraphicsObject *pResource, RsResourceRef *pReference) {
    pResource->__kgrobjAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS kgrobjControl_Prologue_DISPATCH(struct KernelGraphicsObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__kgrobjControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS kgrobjGetRegBaseOffsetAndSize_DISPATCH(struct KernelGraphicsObject *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__kgrobjGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS kgrobjInternalControlForward_DISPATCH(struct KernelGraphicsObject *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__kgrobjInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NV_STATUS kgrobjUnmapFrom_DISPATCH(struct KernelGraphicsObject *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__kgrobjUnmapFrom__(pResource, pParams);
}

static inline void kgrobjControl_Epilogue_DISPATCH(struct KernelGraphicsObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__kgrobjControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS kgrobjControlLookup_DISPATCH(struct KernelGraphicsObject *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__kgrobjControlLookup__(pResource, pParams, ppEntry);
}

static inline NV_STATUS kgrobjGetSwMethods_DISPATCH(struct KernelGraphicsObject *pChannelDescendant, const METHOD **ppMethods, NvU32 *pNumMethods) {
    return pChannelDescendant->__kgrobjGetSwMethods__(pChannelDescendant, ppMethods, pNumMethods);
}

static inline NvHandle kgrobjGetInternalObjectHandle_DISPATCH(struct KernelGraphicsObject *pGpuResource) {
    return pGpuResource->__kgrobjGetInternalObjectHandle__(pGpuResource);
}

static inline NV_STATUS kgrobjControl_DISPATCH(struct KernelGraphicsObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__kgrobjControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS kgrobjUnmap_DISPATCH(struct KernelGraphicsObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__kgrobjUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NV_STATUS kgrobjGetMemoryMappingDescriptor_DISPATCH(struct KernelGraphicsObject *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__kgrobjGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NvBool kgrobjIsSwMethodStalling_DISPATCH(struct KernelGraphicsObject *pChannelDescendant, NvU32 hHandle) {
    return pChannelDescendant->__kgrobjIsSwMethodStalling__(pChannelDescendant, hHandle);
}

static inline NV_STATUS kgrobjControlFilter_DISPATCH(struct KernelGraphicsObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__kgrobjControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS kgrobjUnregisterEvent_DISPATCH(struct KernelGraphicsObject *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__kgrobjUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS kgrobjControlSerialization_Prologue_DISPATCH(struct KernelGraphicsObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__kgrobjControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool kgrobjCanCopy_DISPATCH(struct KernelGraphicsObject *pResource) {
    return pResource->__kgrobjCanCopy__(pResource);
}

static inline void kgrobjPreDestruct_DISPATCH(struct KernelGraphicsObject *pResource) {
    pResource->__kgrobjPreDestruct__(pResource);
}

static inline NV_STATUS kgrobjIsDuplicate_DISPATCH(struct KernelGraphicsObject *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__kgrobjIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void kgrobjControlSerialization_Epilogue_DISPATCH(struct KernelGraphicsObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__kgrobjControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline PEVENTNOTIFICATION *kgrobjGetNotificationListPtr_DISPATCH(struct KernelGraphicsObject *pNotifier) {
    return pNotifier->__kgrobjGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare *kgrobjGetNotificationShare_DISPATCH(struct KernelGraphicsObject *pNotifier) {
    return pNotifier->__kgrobjGetNotificationShare__(pNotifier);
}

static inline NV_STATUS kgrobjMap_DISPATCH(struct KernelGraphicsObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__kgrobjMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS kgrobjGetOrAllocNotifShare_DISPATCH(struct KernelGraphicsObject *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__kgrobjGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

static inline struct KernelGraphicsContext *kgrobjGetKernelGraphicsContext(struct OBJGPU *pGpu, struct KernelGraphicsObject *pKernelGraphicsObject) {
    struct KernelGraphicsObject_PRIVATE *pKernelGraphicsObject_PRIVATE = (struct KernelGraphicsObject_PRIVATE *)pKernelGraphicsObject;
    return pKernelGraphicsObject_PRIVATE->pKernelGraphicsContext;
}

NV_STATUS kgrobjConstruct_IMPL(struct KernelGraphicsObject *arg_pKernelGraphicsObject, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_kgrobjConstruct(arg_pKernelGraphicsObject, arg_pCallContext, arg_pParams) kgrobjConstruct_IMPL(arg_pKernelGraphicsObject, arg_pCallContext, arg_pParams)
void kgrobjDestruct_IMPL(struct KernelGraphicsObject *pKernelGraphicsObject);

#define __nvoc_kgrobjDestruct(pKernelGraphicsObject) kgrobjDestruct_IMPL(pKernelGraphicsObject)
NV_STATUS kgrobjPromoteContext_IMPL(struct OBJGPU *arg0, struct KernelGraphicsObject *arg1, struct KernelGraphics *arg2);

#ifdef __nvoc_kernel_graphics_object_h_disabled
static inline NV_STATUS kgrobjPromoteContext(struct OBJGPU *arg0, struct KernelGraphicsObject *arg1, struct KernelGraphics *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsObject was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_object_h_disabled
#define kgrobjPromoteContext(arg0, arg1, arg2) kgrobjPromoteContext_IMPL(arg0, arg1, arg2)
#endif //__nvoc_kernel_graphics_object_h_disabled

#undef PRIVATE_FIELD


#endif // _KERNEL_GRAPHICS_OBJECT_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_GRAPHICS_OBJECT_NVOC_H_
