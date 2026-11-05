
#ifndef _G_LOCK_STRESS_NVOC_H_
#define _G_LOCK_STRESS_NVOC_H_

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


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__LockStressObject;
struct NVOC_METADATA__GpuResource;


struct LockStressObject {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__LockStressObject *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuResource __nvoc_base_GpuResource;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^4
    struct RsResource *__nvoc_pbase_RsResource;    // res super^3
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^3
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super^2
    struct GpuResource *__nvoc_pbase_GpuResource;    // gpures super
    struct LockStressObject *__nvoc_pbase_LockStressObject;    // lockStressObj

    // Data members
    NvHandle PRIVATE_FIELD(hInternalClient);
    NvHandle PRIVATE_FIELD(hInternalDevice);
    NvHandle PRIVATE_FIELD(hInternalSubdevice);
    NvHandle PRIVATE_FIELD(hInternalLockStressObject);
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__LockStressObject {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResource metadata__GpuResource;
};

#ifndef __nvoc_class_id_LockStressObject
#define __nvoc_class_id_LockStressObject 0xecce10u
typedef struct LockStressObject LockStressObject;
#endif /* __nvoc_class_id_LockStressObject */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_LockStressObject;

#define __staticCast_LockStressObject(pThis) \
    ((pThis)->__nvoc_pbase_LockStressObject)

#ifdef __nvoc_lock_stress_h_disabled
#define __dynamicCast_LockStressObject(pThis) ((LockStressObject*) NULL)
#else //__nvoc_lock_stress_h_disabled
#define __dynamicCast_LockStressObject(pThis) \
    ((LockStressObject*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(LockStressObject)))
#endif //__nvoc_lock_stress_h_disabled

NV_STATUS __nvoc_objCreateDynamic_LockStressObject(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_LockStressObject(LockStressObject**, Dynamic*, NvU32, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __objCreate_LockStressObject(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams) \
    __nvoc_objCreate_LockStressObject((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pCallContext, pParams)


// Wrapper macros for implementation functions
NV_STATUS lockStressObjConstruct_IMPL(struct LockStressObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __nvoc_lockStressObjConstruct(pResource, pCallContext, pParams) lockStressObjConstruct_IMPL(pResource, pCallContext, pParams)

void lockStressObjDestruct_IMPL(struct LockStressObject *pResource);
#define __nvoc_lockStressObjDestruct(pResource) lockStressObjDestruct_IMPL(pResource)

NV_STATUS lockStressObjCtrlCmdResetLockStressState_IMPL(struct LockStressObject *pResource);
#ifdef __nvoc_lock_stress_h_disabled
static inline NV_STATUS lockStressObjCtrlCmdResetLockStressState(struct LockStressObject *pResource) {
    NV_ASSERT_FAILED_PRECOMP("LockStressObject was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_lock_stress_h_disabled
#define lockStressObjCtrlCmdResetLockStressState(pResource) lockStressObjCtrlCmdResetLockStressState_IMPL(pResource)
#endif // __nvoc_lock_stress_h_disabled

NV_STATUS lockStressObjCtrlCmdPerformLockStressAllRmLocks_IMPL(struct LockStressObject *pResource, NV0100_CTRL_PERFORM_LOCK_STRESS_ALL_RM_LOCKS_PARAMS *pParams);
#ifdef __nvoc_lock_stress_h_disabled
static inline NV_STATUS lockStressObjCtrlCmdPerformLockStressAllRmLocks(struct LockStressObject *pResource, NV0100_CTRL_PERFORM_LOCK_STRESS_ALL_RM_LOCKS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("LockStressObject was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_lock_stress_h_disabled
#define lockStressObjCtrlCmdPerformLockStressAllRmLocks(pResource, pParams) lockStressObjCtrlCmdPerformLockStressAllRmLocks_IMPL(pResource, pParams)
#endif // __nvoc_lock_stress_h_disabled

NV_STATUS lockStressObjCtrlCmdPerformLockStressNoGpusLock_IMPL(struct LockStressObject *pResource, NV0100_CTRL_PERFORM_LOCK_STRESS_NO_GPUS_LOCK_PARAMS *pParams);
#ifdef __nvoc_lock_stress_h_disabled
static inline NV_STATUS lockStressObjCtrlCmdPerformLockStressNoGpusLock(struct LockStressObject *pResource, NV0100_CTRL_PERFORM_LOCK_STRESS_NO_GPUS_LOCK_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("LockStressObject was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_lock_stress_h_disabled
#define lockStressObjCtrlCmdPerformLockStressNoGpusLock(pResource, pParams) lockStressObjCtrlCmdPerformLockStressNoGpusLock_IMPL(pResource, pParams)
#endif // __nvoc_lock_stress_h_disabled

NV_STATUS lockStressObjCtrlCmdPerformLockStressApiLockReadMode_IMPL(struct LockStressObject *pResource, NV0100_CTRL_PERFORM_LOCK_STRESS_API_LOCK_READ_MODE_PARAMS *pParams);
#ifdef __nvoc_lock_stress_h_disabled
static inline NV_STATUS lockStressObjCtrlCmdPerformLockStressApiLockReadMode(struct LockStressObject *pResource, NV0100_CTRL_PERFORM_LOCK_STRESS_API_LOCK_READ_MODE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("LockStressObject was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_lock_stress_h_disabled
#define lockStressObjCtrlCmdPerformLockStressApiLockReadMode(pResource, pParams) lockStressObjCtrlCmdPerformLockStressApiLockReadMode_IMPL(pResource, pParams)
#endif // __nvoc_lock_stress_h_disabled

NV_STATUS lockStressObjCtrlCmdPerformLockStressNoGpusLockApiLockReadMode_IMPL(struct LockStressObject *pResource, NV0100_CTRL_PERFORM_LOCK_STRESS_NO_GPUS_LOCK_API_LOCK_READ_MODE_PARAMS *pParams);
#ifdef __nvoc_lock_stress_h_disabled
static inline NV_STATUS lockStressObjCtrlCmdPerformLockStressNoGpusLockApiLockReadMode(struct LockStressObject *pResource, NV0100_CTRL_PERFORM_LOCK_STRESS_NO_GPUS_LOCK_API_LOCK_READ_MODE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("LockStressObject was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_lock_stress_h_disabled
#define lockStressObjCtrlCmdPerformLockStressNoGpusLockApiLockReadMode(pResource, pParams) lockStressObjCtrlCmdPerformLockStressNoGpusLockApiLockReadMode_IMPL(pResource, pParams)
#endif // __nvoc_lock_stress_h_disabled

NV_STATUS lockStressObjCtrlCmdPerformLockStressInternalAllRmLocks_IMPL(struct LockStressObject *pResource, NV0100_CTRL_PERFORM_LOCK_STRESS_INTERNAL_ALL_RM_LOCKS_PARAMS *pParams);
#ifdef __nvoc_lock_stress_h_disabled
static inline NV_STATUS lockStressObjCtrlCmdPerformLockStressInternalAllRmLocks(struct LockStressObject *pResource, NV0100_CTRL_PERFORM_LOCK_STRESS_INTERNAL_ALL_RM_LOCKS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("LockStressObject was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_lock_stress_h_disabled
#define lockStressObjCtrlCmdPerformLockStressInternalAllRmLocks(pResource, pParams) lockStressObjCtrlCmdPerformLockStressInternalAllRmLocks_IMPL(pResource, pParams)
#endif // __nvoc_lock_stress_h_disabled

NV_STATUS lockStressObjCtrlCmdPerformLockStressInternalNoGpusLock_IMPL(struct LockStressObject *pResource, NV0100_CTRL_PERFORM_LOCK_STRESS_INTERNAL_NO_GPUS_LOCK_PARAMS *pParams);
#ifdef __nvoc_lock_stress_h_disabled
static inline NV_STATUS lockStressObjCtrlCmdPerformLockStressInternalNoGpusLock(struct LockStressObject *pResource, NV0100_CTRL_PERFORM_LOCK_STRESS_INTERNAL_NO_GPUS_LOCK_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("LockStressObject was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_lock_stress_h_disabled
#define lockStressObjCtrlCmdPerformLockStressInternalNoGpusLock(pResource, pParams) lockStressObjCtrlCmdPerformLockStressInternalNoGpusLock_IMPL(pResource, pParams)
#endif // __nvoc_lock_stress_h_disabled

NV_STATUS lockStressObjCtrlCmdPerformLockStressInternalApiLockReadMode_IMPL(struct LockStressObject *pResource, NV0100_CTRL_PERFORM_LOCK_STRESS_INTERNAL_API_LOCK_READ_MODE_PARAMS *pParams);
#ifdef __nvoc_lock_stress_h_disabled
static inline NV_STATUS lockStressObjCtrlCmdPerformLockStressInternalApiLockReadMode(struct LockStressObject *pResource, NV0100_CTRL_PERFORM_LOCK_STRESS_INTERNAL_API_LOCK_READ_MODE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("LockStressObject was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_lock_stress_h_disabled
#define lockStressObjCtrlCmdPerformLockStressInternalApiLockReadMode(pResource, pParams) lockStressObjCtrlCmdPerformLockStressInternalApiLockReadMode_IMPL(pResource, pParams)
#endif // __nvoc_lock_stress_h_disabled

NV_STATUS lockStressObjCtrlCmdPerformLockStressInternalNoGpusLockApiLockReadMode_IMPL(struct LockStressObject *pResource, NV0100_CTRL_PERFORM_LOCK_STRESS_INTERNAL_NO_GPUS_LOCK_API_LOCK_READ_MODE_PARAMS *pParams);
#ifdef __nvoc_lock_stress_h_disabled
static inline NV_STATUS lockStressObjCtrlCmdPerformLockStressInternalNoGpusLockApiLockReadMode(struct LockStressObject *pResource, NV0100_CTRL_PERFORM_LOCK_STRESS_INTERNAL_NO_GPUS_LOCK_API_LOCK_READ_MODE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("LockStressObject was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_lock_stress_h_disabled
#define lockStressObjCtrlCmdPerformLockStressInternalNoGpusLockApiLockReadMode(pResource, pParams) lockStressObjCtrlCmdPerformLockStressInternalNoGpusLockApiLockReadMode_IMPL(pResource, pParams)
#endif // __nvoc_lock_stress_h_disabled

NV_STATUS lockStressObjCtrlCmdGetLockStressCounters_IMPL(struct LockStressObject *pResource, NV0100_CTRL_GET_LOCK_STRESS_COUNTERS_PARAMS *pParams);
#ifdef __nvoc_lock_stress_h_disabled
static inline NV_STATUS lockStressObjCtrlCmdGetLockStressCounters(struct LockStressObject *pResource, NV0100_CTRL_GET_LOCK_STRESS_COUNTERS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("LockStressObject was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_lock_stress_h_disabled
#define lockStressObjCtrlCmdGetLockStressCounters(pResource, pParams) lockStressObjCtrlCmdGetLockStressCounters_IMPL(pResource, pParams)
#endif // __nvoc_lock_stress_h_disabled

NV_STATUS lockStressObjCtrlCmdRecursiveGpuLockTestNoLock_IMPL(struct LockStressObject *pResource, NV0100_CTRL_CMD_RECURSIVE_GPU_LOCK_TEST_PARAM *pParams);
#ifdef __nvoc_lock_stress_h_disabled
static inline NV_STATUS lockStressObjCtrlCmdRecursiveGpuLockTestNoLock(struct LockStressObject *pResource, NV0100_CTRL_CMD_RECURSIVE_GPU_LOCK_TEST_PARAM *pParams) {
    NV_ASSERT_FAILED_PRECOMP("LockStressObject was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_lock_stress_h_disabled
#define lockStressObjCtrlCmdRecursiveGpuLockTestNoLock(pResource, pParams) lockStressObjCtrlCmdRecursiveGpuLockTestNoLock_IMPL(pResource, pParams)
#endif // __nvoc_lock_stress_h_disabled

NV_STATUS lockStressObjCtrlCmdRecursiveGpuLockTestAllLock_IMPL(struct LockStressObject *pResource, NV0100_CTRL_CMD_RECURSIVE_GPU_LOCK_TEST_PARAM *pParams);
#ifdef __nvoc_lock_stress_h_disabled
static inline NV_STATUS lockStressObjCtrlCmdRecursiveGpuLockTestAllLock(struct LockStressObject *pResource, NV0100_CTRL_CMD_RECURSIVE_GPU_LOCK_TEST_PARAM *pParams) {
    NV_ASSERT_FAILED_PRECOMP("LockStressObject was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_lock_stress_h_disabled
#define lockStressObjCtrlCmdRecursiveGpuLockTestAllLock(pResource, pParams) lockStressObjCtrlCmdRecursiveGpuLockTestAllLock_IMPL(pResource, pParams)
#endif // __nvoc_lock_stress_h_disabled

NV_STATUS lockStressObjCtrlCmdRecursiveGpuLockTestDeviceLock_IMPL(struct LockStressObject *pResource, NV0100_CTRL_CMD_RECURSIVE_GPU_LOCK_TEST_PARAM *pParams);
#ifdef __nvoc_lock_stress_h_disabled
static inline NV_STATUS lockStressObjCtrlCmdRecursiveGpuLockTestDeviceLock(struct LockStressObject *pResource, NV0100_CTRL_CMD_RECURSIVE_GPU_LOCK_TEST_PARAM *pParams) {
    NV_ASSERT_FAILED_PRECOMP("LockStressObject was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_lock_stress_h_disabled
#define lockStressObjCtrlCmdRecursiveGpuLockTestDeviceLock(pResource, pParams) lockStressObjCtrlCmdRecursiveGpuLockTestDeviceLock_IMPL(pResource, pParams)
#endif // __nvoc_lock_stress_h_disabled


// Wrapper macros for halified functions
#define lockStressObjControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__
#define lockStressObjControl(pGpuResource, pCallContext, pParams) lockStressObjControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define lockStressObjMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__
#define lockStressObjMap(pGpuResource, pCallContext, pParams, pCpuMapping) lockStressObjMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define lockStressObjUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__
#define lockStressObjUnmap(pGpuResource, pCallContext, pCpuMapping) lockStressObjUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define lockStressObjShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__
#define lockStressObjShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) lockStressObjShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define lockStressObjGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__
#define lockStressObjGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) lockStressObjGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define lockStressObjGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__
#define lockStressObjGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) lockStressObjGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define lockStressObjInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__
#define lockStressObjInternalControlForward(pGpuResource, command, pParams, size) lockStressObjInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define lockStressObjGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__
#define lockStressObjGetInternalObjectHandle(pGpuResource) lockStressObjGetInternalObjectHandle_DISPATCH(pGpuResource)
#define lockStressObjAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__
#define lockStressObjAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) lockStressObjAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define lockStressObjGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define lockStressObjGetMemInterMapParams(pRmResource, pParams) lockStressObjGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define lockStressObjCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define lockStressObjCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) lockStressObjCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define lockStressObjGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define lockStressObjGetMemoryMappingDescriptor(pRmResource, ppMemDesc) lockStressObjGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define lockStressObjControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__
#define lockStressObjControlSerialization_Prologue(pResource, pCallContext, pParams) lockStressObjControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define lockStressObjControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__
#define lockStressObjControlSerialization_Epilogue(pResource, pCallContext, pParams) lockStressObjControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define lockStressObjControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__
#define lockStressObjControl_Prologue(pResource, pCallContext, pParams) lockStressObjControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define lockStressObjControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__
#define lockStressObjControl_Epilogue(pResource, pCallContext, pParams) lockStressObjControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define lockStressObjCanCopy_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define lockStressObjCanCopy(pResource) lockStressObjCanCopy_DISPATCH(pResource)
#define lockStressObjIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define lockStressObjIsDuplicate(pResource, hMemory, pDuplicate) lockStressObjIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define lockStressObjPreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define lockStressObjPreDestruct(pResource) lockStressObjPreDestruct_DISPATCH(pResource)
#define lockStressObjControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define lockStressObjControlFilter(pResource, pCallContext, pParams) lockStressObjControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define lockStressObjIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define lockStressObjIsPartialUnmapSupported(pResource) lockStressObjIsPartialUnmapSupported_DISPATCH(pResource)
#define lockStressObjMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define lockStressObjMapTo(pResource, pParams) lockStressObjMapTo_DISPATCH(pResource, pParams)
#define lockStressObjUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define lockStressObjUnmapFrom(pResource, pParams) lockStressObjUnmapFrom_DISPATCH(pResource, pParams)
#define lockStressObjGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define lockStressObjGetRefCount(pResource) lockStressObjGetRefCount_DISPATCH(pResource)
#define lockStressObjAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define lockStressObjAddAdditionalDependants(pClient, pResource, pReference) lockStressObjAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NV_STATUS lockStressObjControl_DISPATCH(struct LockStressObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__(staticCast(pGpuResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS lockStressObjMap_DISPATCH(struct LockStressObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__(staticCast(pGpuResource, RsResource), pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS lockStressObjUnmap_DISPATCH(struct LockStressObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__(staticCast(pGpuResource, RsResource), pCallContext, pCpuMapping);
}

static inline NvBool lockStressObjShareCallback_DISPATCH(struct LockStressObject *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__(staticCast(pGpuResource, RsResource), pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS lockStressObjGetRegBaseOffsetAndSize_DISPATCH(struct LockStressObject *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__(staticCast(pGpuResource, GpuResource), pGpu, pOffset, pSize);
}

static inline NV_STATUS lockStressObjGetMapAddrSpace_DISPATCH(struct LockStressObject *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__(staticCast(pGpuResource, GpuResource), pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS lockStressObjInternalControlForward_DISPATCH(struct LockStressObject *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__(staticCast(pGpuResource, GpuResource), command, pParams, size);
}

static inline NvHandle lockStressObjGetInternalObjectHandle_DISPATCH(struct LockStressObject *pGpuResource) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__(staticCast(pGpuResource, GpuResource));
}

static inline NvBool lockStressObjAccessCallback_DISPATCH(struct LockStressObject *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__(staticCast(pResource, RsResource), pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS lockStressObjGetMemInterMapParams_DISPATCH(struct LockStressObject *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__(staticCast(pRmResource, RmResource), pParams);
}

static inline NV_STATUS lockStressObjCheckMemInterUnmap_DISPATCH(struct LockStressObject *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__(staticCast(pRmResource, RmResource), bSubdeviceHandleProvided);
}

static inline NV_STATUS lockStressObjGetMemoryMappingDescriptor_DISPATCH(struct LockStressObject *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__(staticCast(pRmResource, RmResource), ppMemDesc);
}

static inline NV_STATUS lockStressObjControlSerialization_Prologue_DISPATCH(struct LockStressObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void lockStressObjControlSerialization_Epilogue_DISPATCH(struct LockStressObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS lockStressObjControl_Prologue_DISPATCH(struct LockStressObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void lockStressObjControl_Epilogue_DISPATCH(struct LockStressObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool lockStressObjCanCopy_DISPATCH(struct LockStressObject *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__(staticCast(pResource, RsResource));
}

static inline NV_STATUS lockStressObjIsDuplicate_DISPATCH(struct LockStressObject *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__(staticCast(pResource, RsResource), hMemory, pDuplicate);
}

static inline void lockStressObjPreDestruct_DISPATCH(struct LockStressObject *pResource) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__(staticCast(pResource, RsResource));
}

static inline NV_STATUS lockStressObjControlFilter_DISPATCH(struct LockStressObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool lockStressObjIsPartialUnmapSupported_DISPATCH(struct LockStressObject *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__(staticCast(pResource, RsResource));
}

static inline NV_STATUS lockStressObjMapTo_DISPATCH(struct LockStressObject *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__(staticCast(pResource, RsResource), pParams);
}

static inline NV_STATUS lockStressObjUnmapFrom_DISPATCH(struct LockStressObject *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__(staticCast(pResource, RsResource), pParams);
}

static inline NvU32 lockStressObjGetRefCount_DISPATCH(struct LockStressObject *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__(staticCast(pResource, RsResource));
}

static inline void lockStressObjAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct LockStressObject *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__(pClient, staticCast(pResource, RsResource), pReference);
}

// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
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

NV_STATUS lockStressObjCtrlCmdRecursiveGpuLockTestNoLock_IMPL(struct LockStressObject *pResource, NV0100_CTRL_CMD_RECURSIVE_GPU_LOCK_TEST_PARAM *pParams);

NV_STATUS lockStressObjCtrlCmdRecursiveGpuLockTestAllLock_IMPL(struct LockStressObject *pResource, NV0100_CTRL_CMD_RECURSIVE_GPU_LOCK_TEST_PARAM *pParams);

NV_STATUS lockStressObjCtrlCmdRecursiveGpuLockTestDeviceLock_IMPL(struct LockStressObject *pResource, NV0100_CTRL_CMD_RECURSIVE_GPU_LOCK_TEST_PARAM *pParams);

// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


#endif // LOCK_STRESS_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_LOCK_STRESS_NVOC_H_
