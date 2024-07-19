
#ifndef _G_SEM_SURF_NVOC_H_
#define _G_SEM_SURF_NVOC_H_
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
 * SPDX-FileCopyrightText: Copyright (c) 2023-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_sem_surf_nvoc.h"

#ifndef SEM_SURF_H
#define SEM_SURF_H

#include "gpu/gpu_resource.h" // GpuResource
#include "mem_mgr/mem.h"
#include "rmapi/event.h"     // EVENTNOTIFICATION

#include "class/cl00da.h"    // NV_SEMAPHORE_SURFACE
#include "ctrl/ctrl00da.h"   // NV_SEMAPHORE_SURFACE_CTRL_*
#include "ctrl/ctrl2080/ctrl2080fb.h"   // NV2080_CTRL_FB_GET_SEMAPHORE_SURFACE_LAYOUT_PARAMS

typedef struct {
    /* Value being waited for by pListeners list */
    NvU64 value;

    /* Value to set the semaphore to after wait value is reached */
    NvU64 newValue;

    /* Index of this value listener in the semaphore surface */
    NvU64 index;

    /* List of listeners waiting for the value */
    EVENTNOTIFICATION *pListeners;

    ListNode valueListEntry;
    ListNode pendingNotificationsEntry;
} SEM_VALUE_LISTENERS_NODE;

MAKE_INTRUSIVE_LIST(SEM_VALUE_LISTENERS, SEM_VALUE_LISTENERS_NODE, valueListEntry);
MAKE_INTRUSIVE_LIST(SEM_PENDING_NOTIFICATIONS, SEM_VALUE_LISTENERS_NODE, pendingNotificationsEntry);

typedef struct {
    /* List of values being listened for at this index, ordered by value. */
    SEM_VALUE_LISTENERS listeners;

    MapNode node;
} SEM_INDEX_LISTENERS_NODE;

MAKE_INTRUSIVE_MAP(SEM_INDEX_LISTENERS,
                   SEM_INDEX_LISTENERS_NODE,
                   node);

typedef struct {
    NvHandle hEvent;
    NvU32 nUsers;

    MapNode node;
} SEM_NOTIFIER_NODE;

MAKE_INTRUSIVE_MAP(SEM_NOTIFIER,
                   SEM_NOTIFIER_NODE,
                   node);

typedef struct {
    PORT_SPINLOCK *pSpinlock;

    /* Internal interrupt handler callback data */
    NvHandle hClient;
    NvHandle hDevice[NV_MAX_DEVICES];
    NvHandle hSubDevice[NV_MAX_DEVICES];
    NvHandle hSemaphoreMem;
    NvHandle hMaxSubmittedMem;
    NvHandle *phEvents;
    NVOS10_EVENT_KERNEL_CALLBACK_EX callback;
    NvU32 numEvents;

    /* The client-mappable semaphore memory region and associated data */
    NV2080_CTRL_FB_GET_SEMAPHORE_SURFACE_LAYOUT_PARAMS layout;
    struct Memory* pSemaphoreMem;
    struct Memory* pMaxSubmittedMem;
    NvP64 semKernAddr;
    NvP64 maxSubmittedKernAddr;
    NvU8 *pSem;
    NvU8 *pMaxSubmitted;
    NvU64 slotCount;

    /* Client active CPU waiters list XXX Should be per (offset,value) pair */
    SEM_INDEX_LISTENERS listenerMap;

    /* Map of registered interrupt callbacks by notifier index XXX Should be per (device instance, notifier index) */
    SEM_NOTIFIER notifierMap;

    /* Number of semaphore surface objects sharing this data */
    NvU32 refCount;

    /*
     * GPU instance used to instantiate this shared data. This can be used to
     * find the hDevice/hSubDevice handles corresponding to the memory objects
     * duplicated into the shared data's private RM client.
     */
    NvU32 memGpuIdx;

    /* True if this semaphore surface supports 64-bit semaphores */
    NvBool bIs64Bit;

    /* True if the GPU supports conditional traps/monitored fence */
    NvBool bHasMonitoredFence;
} SEM_SHARED_DATA;

typedef struct {
    MapNode node;

    NvU32 gpuIdx;
    NvU32 numNotifyIndices;
    NvU32 notifyIndices[NV_SEMAPHORE_SURFACE_CTRL_CMD_BIND_CHANNEL_MAX_INDICES];
} SEM_CHANNEL_NODE;

MAKE_INTRUSIVE_MAP(SEM_CHANNEL_BINDING,
                   SEM_CHANNEL_NODE,
                   node);


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_SEM_SURF_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct SemaphoreSurface {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct GpuResource __nvoc_base_GpuResource;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^4
    struct RsResource *__nvoc_pbase_RsResource;    // res super^3
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^3
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super^2
    struct GpuResource *__nvoc_pbase_GpuResource;    // gpures super
    struct SemaphoreSurface *__nvoc_pbase_SemaphoreSurface;    // semsurf

    // Vtable with 31 per-object function pointers
    NvBool (*__semsurfCanCopy__)(struct SemaphoreSurface * /*this*/);  // inline virtual override (res) base (gpures) body
    NV_STATUS (*__semsurfCtrlCmdRefMemory__)(struct SemaphoreSurface * /*this*/, NV_SEMAPHORE_SURFACE_CTRL_REF_MEMORY_PARAMS *);  // exported (id=0xda0001)
    NV_STATUS (*__semsurfCtrlCmdBindChannel__)(struct SemaphoreSurface * /*this*/, NV_SEMAPHORE_SURFACE_CTRL_BIND_CHANNEL_PARAMS *);  // exported (id=0xda0002)
    NV_STATUS (*__semsurfCtrlCmdUnbindChannel__)(struct SemaphoreSurface * /*this*/, NV_SEMAPHORE_SURFACE_CTRL_UNBIND_CHANNEL_PARAMS *);  // exported (id=0xda0006)
    NV_STATUS (*__semsurfCtrlCmdRegisterWaiter__)(struct SemaphoreSurface * /*this*/, NV_SEMAPHORE_SURFACE_CTRL_REGISTER_WAITER_PARAMS *);  // exported (id=0xda0003)
    NV_STATUS (*__semsurfCtrlCmdSetValue__)(struct SemaphoreSurface * /*this*/, NV_SEMAPHORE_SURFACE_CTRL_SET_VALUE_PARAMS *);  // exported (id=0xda0004)
    NV_STATUS (*__semsurfCtrlCmdUnregisterWaiter__)(struct SemaphoreSurface * /*this*/, NV_SEMAPHORE_SURFACE_CTRL_UNREGISTER_WAITER_PARAMS *);  // exported (id=0xda0005)
    NV_STATUS (*__semsurfControl__)(struct SemaphoreSurface * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__semsurfMap__)(struct SemaphoreSurface * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__semsurfUnmap__)(struct SemaphoreSurface * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NvBool (*__semsurfShareCallback__)(struct SemaphoreSurface * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__semsurfGetRegBaseOffsetAndSize__)(struct SemaphoreSurface * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__semsurfGetMapAddrSpace__)(struct SemaphoreSurface * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__semsurfInternalControlForward__)(struct SemaphoreSurface * /*this*/, NvU32, void *, NvU32);  // virtual inherited (gpures) base (gpures)
    NvHandle (*__semsurfGetInternalObjectHandle__)(struct SemaphoreSurface * /*this*/);  // virtual inherited (gpures) base (gpures)
    NvBool (*__semsurfAccessCallback__)(struct SemaphoreSurface * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__semsurfGetMemInterMapParams__)(struct SemaphoreSurface * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__semsurfCheckMemInterUnmap__)(struct SemaphoreSurface * /*this*/, NvBool);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__semsurfGetMemoryMappingDescriptor__)(struct SemaphoreSurface * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__semsurfControlSerialization_Prologue__)(struct SemaphoreSurface * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__semsurfControlSerialization_Epilogue__)(struct SemaphoreSurface * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__semsurfControl_Prologue__)(struct SemaphoreSurface * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__semsurfControl_Epilogue__)(struct SemaphoreSurface * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__semsurfIsDuplicate__)(struct SemaphoreSurface * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (gpures)
    void (*__semsurfPreDestruct__)(struct SemaphoreSurface * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__semsurfControlFilter__)(struct SemaphoreSurface * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (gpures)
    NvBool (*__semsurfIsPartialUnmapSupported__)(struct SemaphoreSurface * /*this*/);  // inline virtual inherited (res) base (gpures) body
    NV_STATUS (*__semsurfMapTo__)(struct SemaphoreSurface * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__semsurfUnmapFrom__)(struct SemaphoreSurface * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (gpures)
    NvU32 (*__semsurfGetRefCount__)(struct SemaphoreSurface * /*this*/);  // virtual inherited (res) base (gpures)
    void (*__semsurfAddAdditionalDependants__)(struct RsClient *, struct SemaphoreSurface * /*this*/, RsResourceRef *);  // virtual inherited (res) base (gpures)

    // Data members
    SEM_SHARED_DATA *pShared;
    SEM_CHANNEL_BINDING boundChannelMap;
};

#ifndef __NVOC_CLASS_SemaphoreSurface_TYPEDEF__
#define __NVOC_CLASS_SemaphoreSurface_TYPEDEF__
typedef struct SemaphoreSurface SemaphoreSurface;
#endif /* __NVOC_CLASS_SemaphoreSurface_TYPEDEF__ */

#ifndef __nvoc_class_id_SemaphoreSurface
#define __nvoc_class_id_SemaphoreSurface 0xeabc69
#endif /* __nvoc_class_id_SemaphoreSurface */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_SemaphoreSurface;

#define __staticCast_SemaphoreSurface(pThis) \
    ((pThis)->__nvoc_pbase_SemaphoreSurface)

#ifdef __nvoc_sem_surf_h_disabled
#define __dynamicCast_SemaphoreSurface(pThis) ((SemaphoreSurface*)NULL)
#else //__nvoc_sem_surf_h_disabled
#define __dynamicCast_SemaphoreSurface(pThis) \
    ((SemaphoreSurface*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(SemaphoreSurface)))
#endif //__nvoc_sem_surf_h_disabled

NV_STATUS __nvoc_objCreateDynamic_SemaphoreSurface(SemaphoreSurface**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_SemaphoreSurface(SemaphoreSurface**, Dynamic*, NvU32, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_SemaphoreSurface(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_SemaphoreSurface((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define semsurfCanCopy_FNPTR(pSemSurf) pSemSurf->__semsurfCanCopy__
#define semsurfCanCopy(pSemSurf) semsurfCanCopy_DISPATCH(pSemSurf)
#define semsurfCtrlCmdRefMemory_FNPTR(pSemaphoreSurf) pSemaphoreSurf->__semsurfCtrlCmdRefMemory__
#define semsurfCtrlCmdRefMemory(pSemaphoreSurf, pParams) semsurfCtrlCmdRefMemory_DISPATCH(pSemaphoreSurf, pParams)
#define semsurfCtrlCmdBindChannel_FNPTR(pSemaphoreSurf) pSemaphoreSurf->__semsurfCtrlCmdBindChannel__
#define semsurfCtrlCmdBindChannel(pSemaphoreSurf, pParams) semsurfCtrlCmdBindChannel_DISPATCH(pSemaphoreSurf, pParams)
#define semsurfCtrlCmdUnbindChannel_FNPTR(pSemaphoreSurf) pSemaphoreSurf->__semsurfCtrlCmdUnbindChannel__
#define semsurfCtrlCmdUnbindChannel(pSemaphoreSurf, pParams) semsurfCtrlCmdUnbindChannel_DISPATCH(pSemaphoreSurf, pParams)
#define semsurfCtrlCmdRegisterWaiter_FNPTR(pSemaphoreSurf) pSemaphoreSurf->__semsurfCtrlCmdRegisterWaiter__
#define semsurfCtrlCmdRegisterWaiter(pSemaphoreSurf, pParams) semsurfCtrlCmdRegisterWaiter_DISPATCH(pSemaphoreSurf, pParams)
#define semsurfCtrlCmdSetValue_FNPTR(pSemaphoreSurf) pSemaphoreSurf->__semsurfCtrlCmdSetValue__
#define semsurfCtrlCmdSetValue(pSemaphoreSurf, pParams) semsurfCtrlCmdSetValue_DISPATCH(pSemaphoreSurf, pParams)
#define semsurfCtrlCmdUnregisterWaiter_FNPTR(pSemaphoreSurf) pSemaphoreSurf->__semsurfCtrlCmdUnregisterWaiter__
#define semsurfCtrlCmdUnregisterWaiter(pSemaphoreSurf, pParams) semsurfCtrlCmdUnregisterWaiter_DISPATCH(pSemaphoreSurf, pParams)
#define semsurfControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresControl__
#define semsurfControl(pGpuResource, pCallContext, pParams) semsurfControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define semsurfMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresMap__
#define semsurfMap(pGpuResource, pCallContext, pParams, pCpuMapping) semsurfMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define semsurfUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresUnmap__
#define semsurfUnmap(pGpuResource, pCallContext, pCpuMapping) semsurfUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define semsurfShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresShareCallback__
#define semsurfShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) semsurfShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define semsurfGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetRegBaseOffsetAndSize__
#define semsurfGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) semsurfGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define semsurfGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetMapAddrSpace__
#define semsurfGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) semsurfGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define semsurfInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresInternalControlForward__
#define semsurfInternalControlForward(pGpuResource, command, pParams, size) semsurfInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define semsurfGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetInternalObjectHandle__
#define semsurfGetInternalObjectHandle(pGpuResource) semsurfGetInternalObjectHandle_DISPATCH(pGpuResource)
#define semsurfAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresAccessCallback__
#define semsurfAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) semsurfAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define semsurfGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemInterMapParams__
#define semsurfGetMemInterMapParams(pRmResource, pParams) semsurfGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define semsurfCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresCheckMemInterUnmap__
#define semsurfCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) semsurfCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define semsurfGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemoryMappingDescriptor__
#define semsurfGetMemoryMappingDescriptor(pRmResource, ppMemDesc) semsurfGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define semsurfControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Prologue__
#define semsurfControlSerialization_Prologue(pResource, pCallContext, pParams) semsurfControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define semsurfControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Epilogue__
#define semsurfControlSerialization_Epilogue(pResource, pCallContext, pParams) semsurfControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define semsurfControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Prologue__
#define semsurfControl_Prologue(pResource, pCallContext, pParams) semsurfControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define semsurfControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Epilogue__
#define semsurfControl_Epilogue(pResource, pCallContext, pParams) semsurfControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define semsurfIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsDuplicate__
#define semsurfIsDuplicate(pResource, hMemory, pDuplicate) semsurfIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define semsurfPreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resPreDestruct__
#define semsurfPreDestruct(pResource) semsurfPreDestruct_DISPATCH(pResource)
#define semsurfControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resControlFilter__
#define semsurfControlFilter(pResource, pCallContext, pParams) semsurfControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define semsurfIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsPartialUnmapSupported__
#define semsurfIsPartialUnmapSupported(pResource) semsurfIsPartialUnmapSupported_DISPATCH(pResource)
#define semsurfMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resMapTo__
#define semsurfMapTo(pResource, pParams) semsurfMapTo_DISPATCH(pResource, pParams)
#define semsurfUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmapFrom__
#define semsurfUnmapFrom(pResource, pParams) semsurfUnmapFrom_DISPATCH(pResource, pParams)
#define semsurfGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resGetRefCount__
#define semsurfGetRefCount(pResource) semsurfGetRefCount_DISPATCH(pResource)
#define semsurfAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resAddAdditionalDependants__
#define semsurfAddAdditionalDependants(pClient, pResource, pReference) semsurfAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NvBool semsurfCanCopy_DISPATCH(struct SemaphoreSurface *pSemSurf) {
    return pSemSurf->__semsurfCanCopy__(pSemSurf);
}

static inline NV_STATUS semsurfCtrlCmdRefMemory_DISPATCH(struct SemaphoreSurface *pSemaphoreSurf, NV_SEMAPHORE_SURFACE_CTRL_REF_MEMORY_PARAMS *pParams) {
    return pSemaphoreSurf->__semsurfCtrlCmdRefMemory__(pSemaphoreSurf, pParams);
}

static inline NV_STATUS semsurfCtrlCmdBindChannel_DISPATCH(struct SemaphoreSurface *pSemaphoreSurf, NV_SEMAPHORE_SURFACE_CTRL_BIND_CHANNEL_PARAMS *pParams) {
    return pSemaphoreSurf->__semsurfCtrlCmdBindChannel__(pSemaphoreSurf, pParams);
}

static inline NV_STATUS semsurfCtrlCmdUnbindChannel_DISPATCH(struct SemaphoreSurface *pSemaphoreSurf, NV_SEMAPHORE_SURFACE_CTRL_UNBIND_CHANNEL_PARAMS *pParams) {
    return pSemaphoreSurf->__semsurfCtrlCmdUnbindChannel__(pSemaphoreSurf, pParams);
}

static inline NV_STATUS semsurfCtrlCmdRegisterWaiter_DISPATCH(struct SemaphoreSurface *pSemaphoreSurf, NV_SEMAPHORE_SURFACE_CTRL_REGISTER_WAITER_PARAMS *pParams) {
    return pSemaphoreSurf->__semsurfCtrlCmdRegisterWaiter__(pSemaphoreSurf, pParams);
}

static inline NV_STATUS semsurfCtrlCmdSetValue_DISPATCH(struct SemaphoreSurface *pSemaphoreSurf, NV_SEMAPHORE_SURFACE_CTRL_SET_VALUE_PARAMS *pParams) {
    return pSemaphoreSurf->__semsurfCtrlCmdSetValue__(pSemaphoreSurf, pParams);
}

static inline NV_STATUS semsurfCtrlCmdUnregisterWaiter_DISPATCH(struct SemaphoreSurface *pSemaphoreSurf, NV_SEMAPHORE_SURFACE_CTRL_UNREGISTER_WAITER_PARAMS *pParams) {
    return pSemaphoreSurf->__semsurfCtrlCmdUnregisterWaiter__(pSemaphoreSurf, pParams);
}

static inline NV_STATUS semsurfControl_DISPATCH(struct SemaphoreSurface *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__semsurfControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS semsurfMap_DISPATCH(struct SemaphoreSurface *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__semsurfMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS semsurfUnmap_DISPATCH(struct SemaphoreSurface *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__semsurfUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NvBool semsurfShareCallback_DISPATCH(struct SemaphoreSurface *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__semsurfShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS semsurfGetRegBaseOffsetAndSize_DISPATCH(struct SemaphoreSurface *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__semsurfGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS semsurfGetMapAddrSpace_DISPATCH(struct SemaphoreSurface *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__semsurfGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS semsurfInternalControlForward_DISPATCH(struct SemaphoreSurface *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__semsurfInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NvHandle semsurfGetInternalObjectHandle_DISPATCH(struct SemaphoreSurface *pGpuResource) {
    return pGpuResource->__semsurfGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool semsurfAccessCallback_DISPATCH(struct SemaphoreSurface *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__semsurfAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS semsurfGetMemInterMapParams_DISPATCH(struct SemaphoreSurface *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__semsurfGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS semsurfCheckMemInterUnmap_DISPATCH(struct SemaphoreSurface *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__semsurfCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS semsurfGetMemoryMappingDescriptor_DISPATCH(struct SemaphoreSurface *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__semsurfGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS semsurfControlSerialization_Prologue_DISPATCH(struct SemaphoreSurface *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__semsurfControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void semsurfControlSerialization_Epilogue_DISPATCH(struct SemaphoreSurface *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__semsurfControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS semsurfControl_Prologue_DISPATCH(struct SemaphoreSurface *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__semsurfControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void semsurfControl_Epilogue_DISPATCH(struct SemaphoreSurface *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__semsurfControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS semsurfIsDuplicate_DISPATCH(struct SemaphoreSurface *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__semsurfIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void semsurfPreDestruct_DISPATCH(struct SemaphoreSurface *pResource) {
    pResource->__semsurfPreDestruct__(pResource);
}

static inline NV_STATUS semsurfControlFilter_DISPATCH(struct SemaphoreSurface *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__semsurfControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool semsurfIsPartialUnmapSupported_DISPATCH(struct SemaphoreSurface *pResource) {
    return pResource->__semsurfIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS semsurfMapTo_DISPATCH(struct SemaphoreSurface *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__semsurfMapTo__(pResource, pParams);
}

static inline NV_STATUS semsurfUnmapFrom_DISPATCH(struct SemaphoreSurface *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__semsurfUnmapFrom__(pResource, pParams);
}

static inline NvU32 semsurfGetRefCount_DISPATCH(struct SemaphoreSurface *pResource) {
    return pResource->__semsurfGetRefCount__(pResource);
}

static inline void semsurfAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct SemaphoreSurface *pResource, RsResourceRef *pReference) {
    pResource->__semsurfAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NvBool semsurfCanCopy_0c883b(struct SemaphoreSurface *pSemSurf) {
    return ((NvBool)(0 == 0));
}

NV_STATUS semsurfCtrlCmdRefMemory_IMPL(struct SemaphoreSurface *pSemaphoreSurf, NV_SEMAPHORE_SURFACE_CTRL_REF_MEMORY_PARAMS *pParams);

NV_STATUS semsurfCtrlCmdBindChannel_IMPL(struct SemaphoreSurface *pSemaphoreSurf, NV_SEMAPHORE_SURFACE_CTRL_BIND_CHANNEL_PARAMS *pParams);

NV_STATUS semsurfCtrlCmdUnbindChannel_IMPL(struct SemaphoreSurface *pSemaphoreSurf, NV_SEMAPHORE_SURFACE_CTRL_UNBIND_CHANNEL_PARAMS *pParams);

NV_STATUS semsurfCtrlCmdRegisterWaiter_IMPL(struct SemaphoreSurface *pSemaphoreSurf, NV_SEMAPHORE_SURFACE_CTRL_REGISTER_WAITER_PARAMS *pParams);

NV_STATUS semsurfCtrlCmdSetValue_IMPL(struct SemaphoreSurface *pSemaphoreSurf, NV_SEMAPHORE_SURFACE_CTRL_SET_VALUE_PARAMS *pParams);

NV_STATUS semsurfCtrlCmdUnregisterWaiter_IMPL(struct SemaphoreSurface *pSemaphoreSurf, NV_SEMAPHORE_SURFACE_CTRL_UNREGISTER_WAITER_PARAMS *pParams);

NV_STATUS semsurfConstruct_IMPL(struct SemaphoreSurface *arg_semaphoreSurf, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_semsurfConstruct(arg_semaphoreSurf, arg_pCallContext, arg_pParams) semsurfConstruct_IMPL(arg_semaphoreSurf, arg_pCallContext, arg_pParams)
void semsurfDestruct_IMPL(struct SemaphoreSurface *pSemSurf);

#define __nvoc_semsurfDestruct(pSemSurf) semsurfDestruct_IMPL(pSemSurf)
NvU64 semsurfGetValue_IMPL(struct SemaphoreSurface *pSemSurf, NvU64 index);

#ifdef __nvoc_sem_surf_h_disabled
static inline NvU64 semsurfGetValue(struct SemaphoreSurface *pSemSurf, NvU64 index) {
    NV_ASSERT_FAILED_PRECOMP("SemaphoreSurface was disabled!");
    return 0;
}
#else //__nvoc_sem_surf_h_disabled
#define semsurfGetValue(pSemSurf, index) semsurfGetValue_IMPL(pSemSurf, index)
#endif //__nvoc_sem_surf_h_disabled

NvBool semsurfValidateIndex_IMPL(struct SemaphoreSurface *pSemSurf, NvU64 index);

#ifdef __nvoc_sem_surf_h_disabled
static inline NvBool semsurfValidateIndex(struct SemaphoreSurface *pSemSurf, NvU64 index) {
    NV_ASSERT_FAILED_PRECOMP("SemaphoreSurface was disabled!");
    return NV_FALSE;
}
#else //__nvoc_sem_surf_h_disabled
#define semsurfValidateIndex(pSemSurf, index) semsurfValidateIndex_IMPL(pSemSurf, index)
#endif //__nvoc_sem_surf_h_disabled

#undef PRIVATE_FIELD


#endif // SEM_SURF_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_SEM_SURF_NVOC_H_
