
#ifndef _G_LOCK_STRESS_NVOC_H_
#define _G_LOCK_STRESS_NVOC_H_
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

#pragma once
#include "g_lock_stress_nvoc.h"

#ifndef LOCK_STRESS_H
#define LOCK_STRESS_H

#include "gpu/gpu_resource.h"
#include "nvoc/prelude.h"
#include "nvstatus.h"
#include "resserv/resserv.h"

#include "ctrl/ctrl0100.h"


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_LOCK_STRESS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct LockStressObject {

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
    struct LockStressObject *__nvoc_pbase_LockStressObject;    // lockStressObj

    // Vtable with 35 per-object function pointers
    NV_STATUS (*__lockStressObjCtrlCmdResetLockStressState__)(struct LockStressObject * /*this*/);  // exported (id=0x1000101)
    NV_STATUS (*__lockStressObjCtrlCmdPerformLockStressAllRmLocks__)(struct LockStressObject * /*this*/, NV0100_CTRL_PERFORM_LOCK_STRESS_ALL_RM_LOCKS_PARAMS *);  // exported (id=0x1000102)
    NV_STATUS (*__lockStressObjCtrlCmdPerformLockStressNoGpusLock__)(struct LockStressObject * /*this*/, NV0100_CTRL_PERFORM_LOCK_STRESS_NO_GPUS_LOCK_PARAMS *);  // exported (id=0x1000103)
    NV_STATUS (*__lockStressObjCtrlCmdPerformLockStressApiLockReadMode__)(struct LockStressObject * /*this*/, NV0100_CTRL_PERFORM_LOCK_STRESS_API_LOCK_READ_MODE_PARAMS *);  // exported (id=0x1000104)
    NV_STATUS (*__lockStressObjCtrlCmdPerformLockStressNoGpusLockApiLockReadMode__)(struct LockStressObject * /*this*/, NV0100_CTRL_PERFORM_LOCK_STRESS_NO_GPUS_LOCK_API_LOCK_READ_MODE_PARAMS *);  // exported (id=0x1000105)
    NV_STATUS (*__lockStressObjCtrlCmdPerformLockStressInternalAllRmLocks__)(struct LockStressObject * /*this*/, NV0100_CTRL_PERFORM_LOCK_STRESS_INTERNAL_ALL_RM_LOCKS_PARAMS *);  // exported (id=0x1000106)
    NV_STATUS (*__lockStressObjCtrlCmdPerformLockStressInternalNoGpusLock__)(struct LockStressObject * /*this*/, NV0100_CTRL_PERFORM_LOCK_STRESS_INTERNAL_NO_GPUS_LOCK_PARAMS *);  // exported (id=0x1000107)
    NV_STATUS (*__lockStressObjCtrlCmdPerformLockStressInternalApiLockReadMode__)(struct LockStressObject * /*this*/, NV0100_CTRL_PERFORM_LOCK_STRESS_INTERNAL_API_LOCK_READ_MODE_PARAMS *);  // exported (id=0x1000108)
    NV_STATUS (*__lockStressObjCtrlCmdPerformLockStressInternalNoGpusLockApiLockReadMode__)(struct LockStressObject * /*this*/, NV0100_CTRL_PERFORM_LOCK_STRESS_INTERNAL_NO_GPUS_LOCK_API_LOCK_READ_MODE_PARAMS *);  // exported (id=0x1000109)
    NV_STATUS (*__lockStressObjCtrlCmdGetLockStressCounters__)(struct LockStressObject * /*this*/, NV0100_CTRL_GET_LOCK_STRESS_COUNTERS_PARAMS *);  // exported (id=0x100010a)
    NV_STATUS (*__lockStressObjControl__)(struct LockStressObject * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__lockStressObjMap__)(struct LockStressObject * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__lockStressObjUnmap__)(struct LockStressObject * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NvBool (*__lockStressObjShareCallback__)(struct LockStressObject * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__lockStressObjGetRegBaseOffsetAndSize__)(struct LockStressObject * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__lockStressObjGetMapAddrSpace__)(struct LockStressObject * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__lockStressObjInternalControlForward__)(struct LockStressObject * /*this*/, NvU32, void *, NvU32);  // virtual inherited (gpures) base (gpures)
    NvHandle (*__lockStressObjGetInternalObjectHandle__)(struct LockStressObject * /*this*/);  // virtual inherited (gpures) base (gpures)
    NvBool (*__lockStressObjAccessCallback__)(struct LockStressObject * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__lockStressObjGetMemInterMapParams__)(struct LockStressObject * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__lockStressObjCheckMemInterUnmap__)(struct LockStressObject * /*this*/, NvBool);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__lockStressObjGetMemoryMappingDescriptor__)(struct LockStressObject * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__lockStressObjControlSerialization_Prologue__)(struct LockStressObject * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__lockStressObjControlSerialization_Epilogue__)(struct LockStressObject * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__lockStressObjControl_Prologue__)(struct LockStressObject * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__lockStressObjControl_Epilogue__)(struct LockStressObject * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NvBool (*__lockStressObjCanCopy__)(struct LockStressObject * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__lockStressObjIsDuplicate__)(struct LockStressObject * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (gpures)
    void (*__lockStressObjPreDestruct__)(struct LockStressObject * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__lockStressObjControlFilter__)(struct LockStressObject * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (gpures)
    NvBool (*__lockStressObjIsPartialUnmapSupported__)(struct LockStressObject * /*this*/);  // inline virtual inherited (res) base (gpures) body
    NV_STATUS (*__lockStressObjMapTo__)(struct LockStressObject * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__lockStressObjUnmapFrom__)(struct LockStressObject * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (gpures)
    NvU32 (*__lockStressObjGetRefCount__)(struct LockStressObject * /*this*/);  // virtual inherited (res) base (gpures)
    void (*__lockStressObjAddAdditionalDependants__)(struct RsClient *, struct LockStressObject * /*this*/, RsResourceRef *);  // virtual inherited (res) base (gpures)

    // Data members
    NvHandle PRIVATE_FIELD(hInternalClient);
    NvHandle PRIVATE_FIELD(hInternalDevice);
    NvHandle PRIVATE_FIELD(hInternalSubdevice);
    NvHandle PRIVATE_FIELD(hInternalLockStressObject);
};

#ifndef __NVOC_CLASS_LockStressObject_TYPEDEF__
#define __NVOC_CLASS_LockStressObject_TYPEDEF__
typedef struct LockStressObject LockStressObject;
#endif /* __NVOC_CLASS_LockStressObject_TYPEDEF__ */

#ifndef __nvoc_class_id_LockStressObject
#define __nvoc_class_id_LockStressObject 0xecce10
#endif /* __nvoc_class_id_LockStressObject */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_LockStressObject;

#define __staticCast_LockStressObject(pThis) \
    ((pThis)->__nvoc_pbase_LockStressObject)

#ifdef __nvoc_lock_stress_h_disabled
#define __dynamicCast_LockStressObject(pThis) ((LockStressObject*)NULL)
#else //__nvoc_lock_stress_h_disabled
#define __dynamicCast_LockStressObject(pThis) \
    ((LockStressObject*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(LockStressObject)))
#endif //__nvoc_lock_stress_h_disabled

NV_STATUS __nvoc_objCreateDynamic_LockStressObject(LockStressObject**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_LockStressObject(LockStressObject**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_LockStressObject(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_LockStressObject((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define lockStressObjCtrlCmdResetLockStressState_FNPTR(pResource) pResource->__lockStressObjCtrlCmdResetLockStressState__
#define lockStressObjCtrlCmdResetLockStressState(pResource) lockStressObjCtrlCmdResetLockStressState_DISPATCH(pResource)
#define lockStressObjCtrlCmdPerformLockStressAllRmLocks_FNPTR(pResource) pResource->__lockStressObjCtrlCmdPerformLockStressAllRmLocks__
#define lockStressObjCtrlCmdPerformLockStressAllRmLocks(pResource, pParams) lockStressObjCtrlCmdPerformLockStressAllRmLocks_DISPATCH(pResource, pParams)
#define lockStressObjCtrlCmdPerformLockStressNoGpusLock_FNPTR(pResource) pResource->__lockStressObjCtrlCmdPerformLockStressNoGpusLock__
#define lockStressObjCtrlCmdPerformLockStressNoGpusLock(pResource, pParams) lockStressObjCtrlCmdPerformLockStressNoGpusLock_DISPATCH(pResource, pParams)
#define lockStressObjCtrlCmdPerformLockStressApiLockReadMode_FNPTR(pResource) pResource->__lockStressObjCtrlCmdPerformLockStressApiLockReadMode__
#define lockStressObjCtrlCmdPerformLockStressApiLockReadMode(pResource, pParams) lockStressObjCtrlCmdPerformLockStressApiLockReadMode_DISPATCH(pResource, pParams)
#define lockStressObjCtrlCmdPerformLockStressNoGpusLockApiLockReadMode_FNPTR(pResource) pResource->__lockStressObjCtrlCmdPerformLockStressNoGpusLockApiLockReadMode__
#define lockStressObjCtrlCmdPerformLockStressNoGpusLockApiLockReadMode(pResource, pParams) lockStressObjCtrlCmdPerformLockStressNoGpusLockApiLockReadMode_DISPATCH(pResource, pParams)
#define lockStressObjCtrlCmdPerformLockStressInternalAllRmLocks_FNPTR(pResource) pResource->__lockStressObjCtrlCmdPerformLockStressInternalAllRmLocks__
#define lockStressObjCtrlCmdPerformLockStressInternalAllRmLocks(pResource, pParams) lockStressObjCtrlCmdPerformLockStressInternalAllRmLocks_DISPATCH(pResource, pParams)
#define lockStressObjCtrlCmdPerformLockStressInternalNoGpusLock_FNPTR(pResource) pResource->__lockStressObjCtrlCmdPerformLockStressInternalNoGpusLock__
#define lockStressObjCtrlCmdPerformLockStressInternalNoGpusLock(pResource, pParams) lockStressObjCtrlCmdPerformLockStressInternalNoGpusLock_DISPATCH(pResource, pParams)
#define lockStressObjCtrlCmdPerformLockStressInternalApiLockReadMode_FNPTR(pResource) pResource->__lockStressObjCtrlCmdPerformLockStressInternalApiLockReadMode__
#define lockStressObjCtrlCmdPerformLockStressInternalApiLockReadMode(pResource, pParams) lockStressObjCtrlCmdPerformLockStressInternalApiLockReadMode_DISPATCH(pResource, pParams)
#define lockStressObjCtrlCmdPerformLockStressInternalNoGpusLockApiLockReadMode_FNPTR(pResource) pResource->__lockStressObjCtrlCmdPerformLockStressInternalNoGpusLockApiLockReadMode__
#define lockStressObjCtrlCmdPerformLockStressInternalNoGpusLockApiLockReadMode(pResource, pParams) lockStressObjCtrlCmdPerformLockStressInternalNoGpusLockApiLockReadMode_DISPATCH(pResource, pParams)
#define lockStressObjCtrlCmdGetLockStressCounters_FNPTR(pResource) pResource->__lockStressObjCtrlCmdGetLockStressCounters__
#define lockStressObjCtrlCmdGetLockStressCounters(pResource, pParams) lockStressObjCtrlCmdGetLockStressCounters_DISPATCH(pResource, pParams)
#define lockStressObjControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresControl__
#define lockStressObjControl(pGpuResource, pCallContext, pParams) lockStressObjControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define lockStressObjMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresMap__
#define lockStressObjMap(pGpuResource, pCallContext, pParams, pCpuMapping) lockStressObjMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define lockStressObjUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresUnmap__
#define lockStressObjUnmap(pGpuResource, pCallContext, pCpuMapping) lockStressObjUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define lockStressObjShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresShareCallback__
#define lockStressObjShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) lockStressObjShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define lockStressObjGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetRegBaseOffsetAndSize__
#define lockStressObjGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) lockStressObjGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define lockStressObjGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetMapAddrSpace__
#define lockStressObjGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) lockStressObjGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define lockStressObjInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresInternalControlForward__
#define lockStressObjInternalControlForward(pGpuResource, command, pParams, size) lockStressObjInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define lockStressObjGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetInternalObjectHandle__
#define lockStressObjGetInternalObjectHandle(pGpuResource) lockStressObjGetInternalObjectHandle_DISPATCH(pGpuResource)
#define lockStressObjAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresAccessCallback__
#define lockStressObjAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) lockStressObjAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define lockStressObjGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemInterMapParams__
#define lockStressObjGetMemInterMapParams(pRmResource, pParams) lockStressObjGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define lockStressObjCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresCheckMemInterUnmap__
#define lockStressObjCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) lockStressObjCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define lockStressObjGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemoryMappingDescriptor__
#define lockStressObjGetMemoryMappingDescriptor(pRmResource, ppMemDesc) lockStressObjGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define lockStressObjControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Prologue__
#define lockStressObjControlSerialization_Prologue(pResource, pCallContext, pParams) lockStressObjControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define lockStressObjControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Epilogue__
#define lockStressObjControlSerialization_Epilogue(pResource, pCallContext, pParams) lockStressObjControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define lockStressObjControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Prologue__
#define lockStressObjControl_Prologue(pResource, pCallContext, pParams) lockStressObjControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define lockStressObjControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Epilogue__
#define lockStressObjControl_Epilogue(pResource, pCallContext, pParams) lockStressObjControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define lockStressObjCanCopy_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__
#define lockStressObjCanCopy(pResource) lockStressObjCanCopy_DISPATCH(pResource)
#define lockStressObjIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsDuplicate__
#define lockStressObjIsDuplicate(pResource, hMemory, pDuplicate) lockStressObjIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define lockStressObjPreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resPreDestruct__
#define lockStressObjPreDestruct(pResource) lockStressObjPreDestruct_DISPATCH(pResource)
#define lockStressObjControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resControlFilter__
#define lockStressObjControlFilter(pResource, pCallContext, pParams) lockStressObjControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define lockStressObjIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsPartialUnmapSupported__
#define lockStressObjIsPartialUnmapSupported(pResource) lockStressObjIsPartialUnmapSupported_DISPATCH(pResource)
#define lockStressObjMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resMapTo__
#define lockStressObjMapTo(pResource, pParams) lockStressObjMapTo_DISPATCH(pResource, pParams)
#define lockStressObjUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmapFrom__
#define lockStressObjUnmapFrom(pResource, pParams) lockStressObjUnmapFrom_DISPATCH(pResource, pParams)
#define lockStressObjGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resGetRefCount__
#define lockStressObjGetRefCount(pResource) lockStressObjGetRefCount_DISPATCH(pResource)
#define lockStressObjAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resAddAdditionalDependants__
#define lockStressObjAddAdditionalDependants(pClient, pResource, pReference) lockStressObjAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NV_STATUS lockStressObjCtrlCmdResetLockStressState_DISPATCH(struct LockStressObject *pResource) {
    return pResource->__lockStressObjCtrlCmdResetLockStressState__(pResource);
}

static inline NV_STATUS lockStressObjCtrlCmdPerformLockStressAllRmLocks_DISPATCH(struct LockStressObject *pResource, NV0100_CTRL_PERFORM_LOCK_STRESS_ALL_RM_LOCKS_PARAMS *pParams) {
    return pResource->__lockStressObjCtrlCmdPerformLockStressAllRmLocks__(pResource, pParams);
}

static inline NV_STATUS lockStressObjCtrlCmdPerformLockStressNoGpusLock_DISPATCH(struct LockStressObject *pResource, NV0100_CTRL_PERFORM_LOCK_STRESS_NO_GPUS_LOCK_PARAMS *pParams) {
    return pResource->__lockStressObjCtrlCmdPerformLockStressNoGpusLock__(pResource, pParams);
}

static inline NV_STATUS lockStressObjCtrlCmdPerformLockStressApiLockReadMode_DISPATCH(struct LockStressObject *pResource, NV0100_CTRL_PERFORM_LOCK_STRESS_API_LOCK_READ_MODE_PARAMS *pParams) {
    return pResource->__lockStressObjCtrlCmdPerformLockStressApiLockReadMode__(pResource, pParams);
}

static inline NV_STATUS lockStressObjCtrlCmdPerformLockStressNoGpusLockApiLockReadMode_DISPATCH(struct LockStressObject *pResource, NV0100_CTRL_PERFORM_LOCK_STRESS_NO_GPUS_LOCK_API_LOCK_READ_MODE_PARAMS *pParams) {
    return pResource->__lockStressObjCtrlCmdPerformLockStressNoGpusLockApiLockReadMode__(pResource, pParams);
}

static inline NV_STATUS lockStressObjCtrlCmdPerformLockStressInternalAllRmLocks_DISPATCH(struct LockStressObject *pResource, NV0100_CTRL_PERFORM_LOCK_STRESS_INTERNAL_ALL_RM_LOCKS_PARAMS *pParams) {
    return pResource->__lockStressObjCtrlCmdPerformLockStressInternalAllRmLocks__(pResource, pParams);
}

static inline NV_STATUS lockStressObjCtrlCmdPerformLockStressInternalNoGpusLock_DISPATCH(struct LockStressObject *pResource, NV0100_CTRL_PERFORM_LOCK_STRESS_INTERNAL_NO_GPUS_LOCK_PARAMS *pParams) {
    return pResource->__lockStressObjCtrlCmdPerformLockStressInternalNoGpusLock__(pResource, pParams);
}

static inline NV_STATUS lockStressObjCtrlCmdPerformLockStressInternalApiLockReadMode_DISPATCH(struct LockStressObject *pResource, NV0100_CTRL_PERFORM_LOCK_STRESS_INTERNAL_API_LOCK_READ_MODE_PARAMS *pParams) {
    return pResource->__lockStressObjCtrlCmdPerformLockStressInternalApiLockReadMode__(pResource, pParams);
}

static inline NV_STATUS lockStressObjCtrlCmdPerformLockStressInternalNoGpusLockApiLockReadMode_DISPATCH(struct LockStressObject *pResource, NV0100_CTRL_PERFORM_LOCK_STRESS_INTERNAL_NO_GPUS_LOCK_API_LOCK_READ_MODE_PARAMS *pParams) {
    return pResource->__lockStressObjCtrlCmdPerformLockStressInternalNoGpusLockApiLockReadMode__(pResource, pParams);
}

static inline NV_STATUS lockStressObjCtrlCmdGetLockStressCounters_DISPATCH(struct LockStressObject *pResource, NV0100_CTRL_GET_LOCK_STRESS_COUNTERS_PARAMS *pParams) {
    return pResource->__lockStressObjCtrlCmdGetLockStressCounters__(pResource, pParams);
}

static inline NV_STATUS lockStressObjControl_DISPATCH(struct LockStressObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__lockStressObjControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS lockStressObjMap_DISPATCH(struct LockStressObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__lockStressObjMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS lockStressObjUnmap_DISPATCH(struct LockStressObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__lockStressObjUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NvBool lockStressObjShareCallback_DISPATCH(struct LockStressObject *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__lockStressObjShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS lockStressObjGetRegBaseOffsetAndSize_DISPATCH(struct LockStressObject *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__lockStressObjGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS lockStressObjGetMapAddrSpace_DISPATCH(struct LockStressObject *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__lockStressObjGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS lockStressObjInternalControlForward_DISPATCH(struct LockStressObject *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__lockStressObjInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NvHandle lockStressObjGetInternalObjectHandle_DISPATCH(struct LockStressObject *pGpuResource) {
    return pGpuResource->__lockStressObjGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool lockStressObjAccessCallback_DISPATCH(struct LockStressObject *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__lockStressObjAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS lockStressObjGetMemInterMapParams_DISPATCH(struct LockStressObject *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__lockStressObjGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS lockStressObjCheckMemInterUnmap_DISPATCH(struct LockStressObject *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__lockStressObjCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS lockStressObjGetMemoryMappingDescriptor_DISPATCH(struct LockStressObject *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__lockStressObjGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS lockStressObjControlSerialization_Prologue_DISPATCH(struct LockStressObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__lockStressObjControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void lockStressObjControlSerialization_Epilogue_DISPATCH(struct LockStressObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__lockStressObjControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS lockStressObjControl_Prologue_DISPATCH(struct LockStressObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__lockStressObjControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void lockStressObjControl_Epilogue_DISPATCH(struct LockStressObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__lockStressObjControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool lockStressObjCanCopy_DISPATCH(struct LockStressObject *pResource) {
    return pResource->__lockStressObjCanCopy__(pResource);
}

static inline NV_STATUS lockStressObjIsDuplicate_DISPATCH(struct LockStressObject *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__lockStressObjIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void lockStressObjPreDestruct_DISPATCH(struct LockStressObject *pResource) {
    pResource->__lockStressObjPreDestruct__(pResource);
}

static inline NV_STATUS lockStressObjControlFilter_DISPATCH(struct LockStressObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__lockStressObjControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool lockStressObjIsPartialUnmapSupported_DISPATCH(struct LockStressObject *pResource) {
    return pResource->__lockStressObjIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS lockStressObjMapTo_DISPATCH(struct LockStressObject *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__lockStressObjMapTo__(pResource, pParams);
}

static inline NV_STATUS lockStressObjUnmapFrom_DISPATCH(struct LockStressObject *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__lockStressObjUnmapFrom__(pResource, pParams);
}

static inline NvU32 lockStressObjGetRefCount_DISPATCH(struct LockStressObject *pResource) {
    return pResource->__lockStressObjGetRefCount__(pResource);
}

static inline void lockStressObjAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct LockStressObject *pResource, RsResourceRef *pReference) {
    pResource->__lockStressObjAddAdditionalDependants__(pClient, pResource, pReference);
}

NV_STATUS lockStressObjCtrlCmdResetLockStressState_IMPL(struct LockStressObject *pResource);

NV_STATUS lockStressObjCtrlCmdPerformLockStressAllRmLocks_IMPL(struct LockStressObject *pResource, NV0100_CTRL_PERFORM_LOCK_STRESS_ALL_RM_LOCKS_PARAMS *pParams);

NV_STATUS lockStressObjCtrlCmdPerformLockStressNoGpusLock_IMPL(struct LockStressObject *pResource, NV0100_CTRL_PERFORM_LOCK_STRESS_NO_GPUS_LOCK_PARAMS *pParams);

NV_STATUS lockStressObjCtrlCmdPerformLockStressApiLockReadMode_IMPL(struct LockStressObject *pResource, NV0100_CTRL_PERFORM_LOCK_STRESS_API_LOCK_READ_MODE_PARAMS *pParams);

NV_STATUS lockStressObjCtrlCmdPerformLockStressNoGpusLockApiLockReadMode_IMPL(struct LockStressObject *pResource, NV0100_CTRL_PERFORM_LOCK_STRESS_NO_GPUS_LOCK_API_LOCK_READ_MODE_PARAMS *pParams);

NV_STATUS lockStressObjCtrlCmdPerformLockStressInternalAllRmLocks_IMPL(struct LockStressObject *pResource, NV0100_CTRL_PERFORM_LOCK_STRESS_INTERNAL_ALL_RM_LOCKS_PARAMS *pParams);

NV_STATUS lockStressObjCtrlCmdPerformLockStressInternalNoGpusLock_IMPL(struct LockStressObject *pResource, NV0100_CTRL_PERFORM_LOCK_STRESS_INTERNAL_NO_GPUS_LOCK_PARAMS *pParams);

NV_STATUS lockStressObjCtrlCmdPerformLockStressInternalApiLockReadMode_IMPL(struct LockStressObject *pResource, NV0100_CTRL_PERFORM_LOCK_STRESS_INTERNAL_API_LOCK_READ_MODE_PARAMS *pParams);

NV_STATUS lockStressObjCtrlCmdPerformLockStressInternalNoGpusLockApiLockReadMode_IMPL(struct LockStressObject *pResource, NV0100_CTRL_PERFORM_LOCK_STRESS_INTERNAL_NO_GPUS_LOCK_API_LOCK_READ_MODE_PARAMS *pParams);

NV_STATUS lockStressObjCtrlCmdGetLockStressCounters_IMPL(struct LockStressObject *pResource, NV0100_CTRL_GET_LOCK_STRESS_COUNTERS_PARAMS *pParams);

NV_STATUS lockStressObjConstruct_IMPL(struct LockStressObject *arg_pResource, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_lockStressObjConstruct(arg_pResource, arg_pCallContext, arg_pParams) lockStressObjConstruct_IMPL(arg_pResource, arg_pCallContext, arg_pParams)
void lockStressObjDestruct_IMPL(struct LockStressObject *pResource);

#define __nvoc_lockStressObjDestruct(pResource) lockStressObjDestruct_IMPL(pResource)
#undef PRIVATE_FIELD


#endif // LOCK_STRESS_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_LOCK_STRESS_NVOC_H_
