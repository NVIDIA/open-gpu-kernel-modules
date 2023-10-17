#ifndef _G_SEM_SURF_NVOC_H_
#define _G_SEM_SURF_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
    PORT_SPINLOCK *pSpinlock;

    /* Internal interrupt handler callback data */
    NvHandle hClient;
    NvHandle hDevice;
    NvHandle hSubDevice;
    NvHandle hSemaphoreMem;
    NvHandle hMaxSubmittedMem;
    NvHandle hEvent;
    NVOS10_EVENT_KERNEL_CALLBACK_EX callback;

    /* The client-mappable semaphore memory region and associated data */
    NV2080_CTRL_FB_GET_SEMAPHORE_SURFACE_LAYOUT_PARAMS layout;
    struct Memory* pSemaphoreMem;
    struct Memory* pMaxSubmittedMem;
    NvP64 semKernAddr;
    NvP64 maxSubmittedKernAddr;
    NvU8 *pSem;
    NvU8 *pMaxSubmitted;

    /* Client active CPU waiters list XXX Should be per (offset,value) pair */
    SEM_INDEX_LISTENERS listenerMap;

    /* Number of semaphore surface objects sharing this data */
    NvU32 refCount;

    /* True if this semaphore surface supports 64-bit semaphores */
    NvBool bIs64Bit;

    /* True if the GPU supports conditional traps/monitored fence */
    NvBool bHasMonitoredFence;
} SEM_SHARED_DATA;

#ifdef NVOC_SEM_SURF_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct SemaphoreSurface {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct GpuResource __nvoc_base_GpuResource;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct GpuResource *__nvoc_pbase_GpuResource;
    struct SemaphoreSurface *__nvoc_pbase_SemaphoreSurface;
    NvBool (*__semsurfCanCopy__)(struct SemaphoreSurface *);
    NV_STATUS (*__semsurfCtrlCmdRefMemory__)(struct SemaphoreSurface *, NV_SEMAPHORE_SURFACE_CTRL_REF_MEMORY_PARAMS *);
    NV_STATUS (*__semsurfCtrlCmdBindChannel__)(struct SemaphoreSurface *, NV_SEMAPHORE_SURFACE_CTRL_BIND_CHANNEL_PARAMS *);
    NV_STATUS (*__semsurfCtrlCmdRegisterWaiter__)(struct SemaphoreSurface *, NV_SEMAPHORE_SURFACE_CTRL_REGISTER_WAITER_PARAMS *);
    NV_STATUS (*__semsurfCtrlCmdSetValue__)(struct SemaphoreSurface *, NV_SEMAPHORE_SURFACE_CTRL_SET_VALUE_PARAMS *);
    NV_STATUS (*__semsurfCtrlCmdUnregisterWaiter__)(struct SemaphoreSurface *, NV_SEMAPHORE_SURFACE_CTRL_UNREGISTER_WAITER_PARAMS *);
    NvBool (*__semsurfShareCallback__)(struct SemaphoreSurface *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__semsurfCheckMemInterUnmap__)(struct SemaphoreSurface *, NvBool);
    NV_STATUS (*__semsurfMapTo__)(struct SemaphoreSurface *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__semsurfGetMapAddrSpace__)(struct SemaphoreSurface *, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    NvU32 (*__semsurfGetRefCount__)(struct SemaphoreSurface *);
    void (*__semsurfAddAdditionalDependants__)(struct RsClient *, struct SemaphoreSurface *, RsResourceRef *);
    NV_STATUS (*__semsurfControl_Prologue__)(struct SemaphoreSurface *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__semsurfGetRegBaseOffsetAndSize__)(struct SemaphoreSurface *, struct OBJGPU *, NvU32 *, NvU32 *);
    NV_STATUS (*__semsurfInternalControlForward__)(struct SemaphoreSurface *, NvU32, void *, NvU32);
    NV_STATUS (*__semsurfUnmapFrom__)(struct SemaphoreSurface *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__semsurfControl_Epilogue__)(struct SemaphoreSurface *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__semsurfControlLookup__)(struct SemaphoreSurface *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NvHandle (*__semsurfGetInternalObjectHandle__)(struct SemaphoreSurface *);
    NV_STATUS (*__semsurfControl__)(struct SemaphoreSurface *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__semsurfUnmap__)(struct SemaphoreSurface *, struct CALL_CONTEXT *, struct RsCpuMapping *);
    NV_STATUS (*__semsurfGetMemInterMapParams__)(struct SemaphoreSurface *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__semsurfGetMemoryMappingDescriptor__)(struct SemaphoreSurface *, struct MEMORY_DESCRIPTOR **);
    NV_STATUS (*__semsurfControlFilter__)(struct SemaphoreSurface *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__semsurfControlSerialization_Prologue__)(struct SemaphoreSurface *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    void (*__semsurfPreDestruct__)(struct SemaphoreSurface *);
    NV_STATUS (*__semsurfIsDuplicate__)(struct SemaphoreSurface *, NvHandle, NvBool *);
    void (*__semsurfControlSerialization_Epilogue__)(struct SemaphoreSurface *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__semsurfMap__)(struct SemaphoreSurface *, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);
    NvBool (*__semsurfAccessCallback__)(struct SemaphoreSurface *, struct RsClient *, void *, RsAccessRight);
    SEM_SHARED_DATA *pShared;
};

#ifndef __NVOC_CLASS_SemaphoreSurface_TYPEDEF__
#define __NVOC_CLASS_SemaphoreSurface_TYPEDEF__
typedef struct SemaphoreSurface SemaphoreSurface;
#endif /* __NVOC_CLASS_SemaphoreSurface_TYPEDEF__ */

#ifndef __nvoc_class_id_SemaphoreSurface
#define __nvoc_class_id_SemaphoreSurface 0xeabc69
#endif /* __nvoc_class_id_SemaphoreSurface */

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

#define semsurfCanCopy(pSemSurf) semsurfCanCopy_DISPATCH(pSemSurf)
#define semsurfCtrlCmdRefMemory(pSemaphoreSurf, pParams) semsurfCtrlCmdRefMemory_DISPATCH(pSemaphoreSurf, pParams)
#define semsurfCtrlCmdBindChannel(pSemaphoreSurf, pParams) semsurfCtrlCmdBindChannel_DISPATCH(pSemaphoreSurf, pParams)
#define semsurfCtrlCmdRegisterWaiter(pSemaphoreSurf, pParams) semsurfCtrlCmdRegisterWaiter_DISPATCH(pSemaphoreSurf, pParams)
#define semsurfCtrlCmdSetValue(pSemaphoreSurf, pParams) semsurfCtrlCmdSetValue_DISPATCH(pSemaphoreSurf, pParams)
#define semsurfCtrlCmdUnregisterWaiter(pSemaphoreSurf, pParams) semsurfCtrlCmdUnregisterWaiter_DISPATCH(pSemaphoreSurf, pParams)
#define semsurfShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) semsurfShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define semsurfCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) semsurfCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define semsurfMapTo(pResource, pParams) semsurfMapTo_DISPATCH(pResource, pParams)
#define semsurfGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) semsurfGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define semsurfGetRefCount(pResource) semsurfGetRefCount_DISPATCH(pResource)
#define semsurfAddAdditionalDependants(pClient, pResource, pReference) semsurfAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define semsurfControl_Prologue(pResource, pCallContext, pParams) semsurfControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define semsurfGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) semsurfGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define semsurfInternalControlForward(pGpuResource, command, pParams, size) semsurfInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define semsurfUnmapFrom(pResource, pParams) semsurfUnmapFrom_DISPATCH(pResource, pParams)
#define semsurfControl_Epilogue(pResource, pCallContext, pParams) semsurfControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define semsurfControlLookup(pResource, pParams, ppEntry) semsurfControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define semsurfGetInternalObjectHandle(pGpuResource) semsurfGetInternalObjectHandle_DISPATCH(pGpuResource)
#define semsurfControl(pGpuResource, pCallContext, pParams) semsurfControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define semsurfUnmap(pGpuResource, pCallContext, pCpuMapping) semsurfUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define semsurfGetMemInterMapParams(pRmResource, pParams) semsurfGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define semsurfGetMemoryMappingDescriptor(pRmResource, ppMemDesc) semsurfGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define semsurfControlFilter(pResource, pCallContext, pParams) semsurfControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define semsurfControlSerialization_Prologue(pResource, pCallContext, pParams) semsurfControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define semsurfPreDestruct(pResource) semsurfPreDestruct_DISPATCH(pResource)
#define semsurfIsDuplicate(pResource, hMemory, pDuplicate) semsurfIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define semsurfControlSerialization_Epilogue(pResource, pCallContext, pParams) semsurfControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define semsurfMap(pGpuResource, pCallContext, pParams, pCpuMapping) semsurfMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define semsurfAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) semsurfAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
static inline NvBool semsurfCanCopy_0c883b(struct SemaphoreSurface *pSemSurf) {
    return ((NvBool)(0 == 0));
}

static inline NvBool semsurfCanCopy_DISPATCH(struct SemaphoreSurface *pSemSurf) {
    return pSemSurf->__semsurfCanCopy__(pSemSurf);
}

NV_STATUS semsurfCtrlCmdRefMemory_IMPL(struct SemaphoreSurface *pSemaphoreSurf, NV_SEMAPHORE_SURFACE_CTRL_REF_MEMORY_PARAMS *pParams);

static inline NV_STATUS semsurfCtrlCmdRefMemory_DISPATCH(struct SemaphoreSurface *pSemaphoreSurf, NV_SEMAPHORE_SURFACE_CTRL_REF_MEMORY_PARAMS *pParams) {
    return pSemaphoreSurf->__semsurfCtrlCmdRefMemory__(pSemaphoreSurf, pParams);
}

NV_STATUS semsurfCtrlCmdBindChannel_IMPL(struct SemaphoreSurface *pSemaphoreSurf, NV_SEMAPHORE_SURFACE_CTRL_BIND_CHANNEL_PARAMS *pParams);

static inline NV_STATUS semsurfCtrlCmdBindChannel_DISPATCH(struct SemaphoreSurface *pSemaphoreSurf, NV_SEMAPHORE_SURFACE_CTRL_BIND_CHANNEL_PARAMS *pParams) {
    return pSemaphoreSurf->__semsurfCtrlCmdBindChannel__(pSemaphoreSurf, pParams);
}

NV_STATUS semsurfCtrlCmdRegisterWaiter_IMPL(struct SemaphoreSurface *pSemaphoreSurf, NV_SEMAPHORE_SURFACE_CTRL_REGISTER_WAITER_PARAMS *pParams);

static inline NV_STATUS semsurfCtrlCmdRegisterWaiter_DISPATCH(struct SemaphoreSurface *pSemaphoreSurf, NV_SEMAPHORE_SURFACE_CTRL_REGISTER_WAITER_PARAMS *pParams) {
    return pSemaphoreSurf->__semsurfCtrlCmdRegisterWaiter__(pSemaphoreSurf, pParams);
}

NV_STATUS semsurfCtrlCmdSetValue_IMPL(struct SemaphoreSurface *pSemaphoreSurf, NV_SEMAPHORE_SURFACE_CTRL_SET_VALUE_PARAMS *pParams);

static inline NV_STATUS semsurfCtrlCmdSetValue_DISPATCH(struct SemaphoreSurface *pSemaphoreSurf, NV_SEMAPHORE_SURFACE_CTRL_SET_VALUE_PARAMS *pParams) {
    return pSemaphoreSurf->__semsurfCtrlCmdSetValue__(pSemaphoreSurf, pParams);
}

NV_STATUS semsurfCtrlCmdUnregisterWaiter_IMPL(struct SemaphoreSurface *pSemaphoreSurf, NV_SEMAPHORE_SURFACE_CTRL_UNREGISTER_WAITER_PARAMS *pParams);

static inline NV_STATUS semsurfCtrlCmdUnregisterWaiter_DISPATCH(struct SemaphoreSurface *pSemaphoreSurf, NV_SEMAPHORE_SURFACE_CTRL_UNREGISTER_WAITER_PARAMS *pParams) {
    return pSemaphoreSurf->__semsurfCtrlCmdUnregisterWaiter__(pSemaphoreSurf, pParams);
}

static inline NvBool semsurfShareCallback_DISPATCH(struct SemaphoreSurface *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__semsurfShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS semsurfCheckMemInterUnmap_DISPATCH(struct SemaphoreSurface *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__semsurfCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS semsurfMapTo_DISPATCH(struct SemaphoreSurface *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__semsurfMapTo__(pResource, pParams);
}

static inline NV_STATUS semsurfGetMapAddrSpace_DISPATCH(struct SemaphoreSurface *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__semsurfGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NvU32 semsurfGetRefCount_DISPATCH(struct SemaphoreSurface *pResource) {
    return pResource->__semsurfGetRefCount__(pResource);
}

static inline void semsurfAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct SemaphoreSurface *pResource, RsResourceRef *pReference) {
    pResource->__semsurfAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS semsurfControl_Prologue_DISPATCH(struct SemaphoreSurface *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__semsurfControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS semsurfGetRegBaseOffsetAndSize_DISPATCH(struct SemaphoreSurface *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__semsurfGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS semsurfInternalControlForward_DISPATCH(struct SemaphoreSurface *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__semsurfInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NV_STATUS semsurfUnmapFrom_DISPATCH(struct SemaphoreSurface *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__semsurfUnmapFrom__(pResource, pParams);
}

static inline void semsurfControl_Epilogue_DISPATCH(struct SemaphoreSurface *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__semsurfControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS semsurfControlLookup_DISPATCH(struct SemaphoreSurface *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__semsurfControlLookup__(pResource, pParams, ppEntry);
}

static inline NvHandle semsurfGetInternalObjectHandle_DISPATCH(struct SemaphoreSurface *pGpuResource) {
    return pGpuResource->__semsurfGetInternalObjectHandle__(pGpuResource);
}

static inline NV_STATUS semsurfControl_DISPATCH(struct SemaphoreSurface *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__semsurfControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS semsurfUnmap_DISPATCH(struct SemaphoreSurface *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__semsurfUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NV_STATUS semsurfGetMemInterMapParams_DISPATCH(struct SemaphoreSurface *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__semsurfGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS semsurfGetMemoryMappingDescriptor_DISPATCH(struct SemaphoreSurface *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__semsurfGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS semsurfControlFilter_DISPATCH(struct SemaphoreSurface *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__semsurfControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS semsurfControlSerialization_Prologue_DISPATCH(struct SemaphoreSurface *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__semsurfControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void semsurfPreDestruct_DISPATCH(struct SemaphoreSurface *pResource) {
    pResource->__semsurfPreDestruct__(pResource);
}

static inline NV_STATUS semsurfIsDuplicate_DISPATCH(struct SemaphoreSurface *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__semsurfIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void semsurfControlSerialization_Epilogue_DISPATCH(struct SemaphoreSurface *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__semsurfControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS semsurfMap_DISPATCH(struct SemaphoreSurface *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__semsurfMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NvBool semsurfAccessCallback_DISPATCH(struct SemaphoreSurface *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__semsurfAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

NV_STATUS semsurfConstruct_IMPL(struct SemaphoreSurface *arg_semaphoreSurf, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_semsurfConstruct(arg_semaphoreSurf, arg_pCallContext, arg_pParams) semsurfConstruct_IMPL(arg_semaphoreSurf, arg_pCallContext, arg_pParams)
void semsurfDestruct_IMPL(struct SemaphoreSurface *pSemSurf);

#define __nvoc_semsurfDestruct(pSemSurf) semsurfDestruct_IMPL(pSemSurf)
#undef PRIVATE_FIELD


#endif // SEM_SURF_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_SEM_SURF_NVOC_H_
