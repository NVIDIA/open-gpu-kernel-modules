
#ifndef _G_MEM_MAPPER_NVOC_H_
#define _G_MEM_MAPPER_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2022-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_mem_mapper_nvoc.h"

#ifndef MEMORY_MAPPER_H
#define MEMORY_MAPPER_H

#include "core/core.h"
#include "rmapi/rmapi.h"
#include "rmapi/resource.h"
#include "gpu/gpu_resource.h"
#include "gpu/mem_mgr/mem_mgr.h"

#include "class/cl00fe.h"
#include "ctrl/ctrl00fe.h"
#include "ctrl/ctrl2080/ctrl2080fb.h"

struct Subdevice;

#ifndef __nvoc_class_id_Subdevice
#define __nvoc_class_id_Subdevice 0x4b01b3u
typedef struct Subdevice Subdevice;
#endif /* __nvoc_class_id_Subdevice */


struct Memory;

#ifndef __nvoc_class_id_Memory
#define __nvoc_class_id_Memory 0x4789f2u
typedef struct Memory Memory;
#endif /* __nvoc_class_id_Memory */


struct OBJGPU;

#ifndef __nvoc_class_id_OBJGPU
#define __nvoc_class_id_OBJGPU 0x7ef3cbu
typedef struct OBJGPU OBJGPU;
#endif /* __nvoc_class_id_OBJGPU */


struct SemaphoreSurface;

#ifndef __nvoc_class_id_SemaphoreSurface
#define __nvoc_class_id_SemaphoreSurface 0xeabc69u
typedef struct SemaphoreSurface SemaphoreSurface;
#endif /* __nvoc_class_id_SemaphoreSurface */


struct MemoryMapper;

#ifndef __nvoc_class_id_MemoryMapper
#define __nvoc_class_id_MemoryMapper 0xb8e4a2u
typedef struct MemoryMapper MemoryMapper;
#endif /* __nvoc_class_id_MemoryMapper */



// MemoryMapper can't wait for workers to finish in destructor due to locking constraints
// Instead set pMemoryMapper to NULL and leave the params structure until all workers are done
typedef struct
{
    struct MemoryMapper *pMemoryMapper;
    NvU32         numRefs;
    PORT_RWLOCK  *pLock; 
} MemoryMapperWorkerParams;

/*!
 * MemoryMapper provides paging operations channel interface to userspace clients.
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_MEM_MAPPER_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__MemoryMapper;
struct NVOC_METADATA__GpuResource;


struct MemoryMapper {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__MemoryMapper *__nvoc_metadata_ptr;
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
    struct MemoryMapper *__nvoc_pbase_MemoryMapper;    // memmapper

    // Data members
    API_SECURITY_INFO secInfo;
    struct Subdevice *pSubdevice;
    struct Memory *pNotificationMemory;
    TRANSFER_SURFACE notificationSurface;
    NV_MEMORY_MAPPER_NOTIFICATION *pNotification;
    NV00FE_CTRL_OPERATION *pOperationQueue;
    NvU32 operationQueuePut;
    NvU32 operationQueueGet;
    NvU32 operationQueueLen;
    NvHandle hInternalClient;
    NvHandle hInternalDevice;
    NvHandle hInternalSubdevice;
    NvHandle hInternalSemaphoreSurface;
    NVOS10_EVENT_KERNEL_CALLBACK_EX semaphoreCallback;
    MemoryMapperWorkerParams *pWorkerParams;
    struct SemaphoreSurface *pSemSurf;
    NvBool bError;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__MemoryMapper {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResource metadata__GpuResource;
};

#ifndef __nvoc_class_id_MemoryMapper
#define __nvoc_class_id_MemoryMapper 0xb8e4a2u
typedef struct MemoryMapper MemoryMapper;
#endif /* __nvoc_class_id_MemoryMapper */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_MemoryMapper;

#define __staticCast_MemoryMapper(pThis) \
    ((pThis)->__nvoc_pbase_MemoryMapper)

#ifdef __nvoc_mem_mapper_h_disabled
#define __dynamicCast_MemoryMapper(pThis) ((MemoryMapper*) NULL)
#else //__nvoc_mem_mapper_h_disabled
#define __dynamicCast_MemoryMapper(pThis) \
    ((MemoryMapper*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(MemoryMapper)))
#endif //__nvoc_mem_mapper_h_disabled

NV_STATUS __nvoc_objCreateDynamic_MemoryMapper(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_MemoryMapper(MemoryMapper**, Dynamic*, NvU32, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __objCreate_MemoryMapper(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams) \
    __nvoc_objCreate_MemoryMapper((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pCallContext, pParams)


// Wrapper macros for implementation functions
NV_STATUS memmapperConstruct_IMPL(struct MemoryMapper *pMemoryMapper, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __nvoc_memmapperConstruct(pMemoryMapper, pCallContext, pParams) memmapperConstruct_IMPL(pMemoryMapper, pCallContext, pParams)

void memmapperDestruct_IMPL(struct MemoryMapper *pMemoryMapper);
#define __nvoc_memmapperDestruct(pMemoryMapper) memmapperDestruct_IMPL(pMemoryMapper)

void memmapperQueueWork_IMPL(struct MemoryMapper *pMemoryMapper);
#ifdef __nvoc_mem_mapper_h_disabled
static inline void memmapperQueueWork(struct MemoryMapper *pMemoryMapper) {
    NV_ASSERT_FAILED_PRECOMP("MemoryMapper was disabled!");
}
#else // __nvoc_mem_mapper_h_disabled
#define memmapperQueueWork(pMemoryMapper) memmapperQueueWork_IMPL(pMemoryMapper)
#endif // __nvoc_mem_mapper_h_disabled

NV_STATUS memmapperCtrlCmdSubmitOperations_IMPL(struct MemoryMapper *pMemoryMapper, NV00FE_CTRL_SUBMIT_OPERATIONS_PARAMS *pParams);
#ifdef __nvoc_mem_mapper_h_disabled
static inline NV_STATUS memmapperCtrlCmdSubmitOperations(struct MemoryMapper *pMemoryMapper, NV00FE_CTRL_SUBMIT_OPERATIONS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("MemoryMapper was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mapper_h_disabled
#define memmapperCtrlCmdSubmitOperations(pMemoryMapper, pParams) memmapperCtrlCmdSubmitOperations_IMPL(pMemoryMapper, pParams)
#endif // __nvoc_mem_mapper_h_disabled

NV_STATUS memmapperCtrlCmdResizeQueue_IMPL(struct MemoryMapper *pMemoryMapper, NV00FE_CTRL_RESIZE_QUEUE_PARAMS *pParams);
#ifdef __nvoc_mem_mapper_h_disabled
static inline NV_STATUS memmapperCtrlCmdResizeQueue(struct MemoryMapper *pMemoryMapper, NV00FE_CTRL_RESIZE_QUEUE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("MemoryMapper was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_mapper_h_disabled
#define memmapperCtrlCmdResizeQueue(pMemoryMapper, pParams) memmapperCtrlCmdResizeQueue_IMPL(pMemoryMapper, pParams)
#endif // __nvoc_mem_mapper_h_disabled


// Wrapper macros for halified functions
#define memmapperControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__
#define memmapperControl(pGpuResource, pCallContext, pParams) memmapperControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define memmapperMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__
#define memmapperMap(pGpuResource, pCallContext, pParams, pCpuMapping) memmapperMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define memmapperUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__
#define memmapperUnmap(pGpuResource, pCallContext, pCpuMapping) memmapperUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define memmapperShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__
#define memmapperShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) memmapperShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define memmapperGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__
#define memmapperGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) memmapperGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define memmapperGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__
#define memmapperGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) memmapperGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define memmapperInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__
#define memmapperInternalControlForward(pGpuResource, command, pParams, size) memmapperInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define memmapperGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__
#define memmapperGetInternalObjectHandle(pGpuResource) memmapperGetInternalObjectHandle_DISPATCH(pGpuResource)
#define memmapperAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__
#define memmapperAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) memmapperAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define memmapperGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define memmapperGetMemInterMapParams(pRmResource, pParams) memmapperGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define memmapperCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define memmapperCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) memmapperCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define memmapperGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define memmapperGetMemoryMappingDescriptor(pRmResource, ppMemDesc) memmapperGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define memmapperControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__
#define memmapperControlSerialization_Prologue(pResource, pCallContext, pParams) memmapperControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define memmapperControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__
#define memmapperControlSerialization_Epilogue(pResource, pCallContext, pParams) memmapperControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define memmapperControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__
#define memmapperControl_Prologue(pResource, pCallContext, pParams) memmapperControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define memmapperControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__
#define memmapperControl_Epilogue(pResource, pCallContext, pParams) memmapperControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define memmapperCanCopy_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define memmapperCanCopy(pResource) memmapperCanCopy_DISPATCH(pResource)
#define memmapperIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define memmapperIsDuplicate(pResource, hMemory, pDuplicate) memmapperIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define memmapperPreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define memmapperPreDestruct(pResource) memmapperPreDestruct_DISPATCH(pResource)
#define memmapperControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define memmapperControlFilter(pResource, pCallContext, pParams) memmapperControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define memmapperIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define memmapperIsPartialUnmapSupported(pResource) memmapperIsPartialUnmapSupported_DISPATCH(pResource)
#define memmapperMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define memmapperMapTo(pResource, pParams) memmapperMapTo_DISPATCH(pResource, pParams)
#define memmapperUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define memmapperUnmapFrom(pResource, pParams) memmapperUnmapFrom_DISPATCH(pResource, pParams)
#define memmapperGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define memmapperGetRefCount(pResource) memmapperGetRefCount_DISPATCH(pResource)
#define memmapperAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define memmapperAddAdditionalDependants(pClient, pResource, pReference) memmapperAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NV_STATUS memmapperControl_DISPATCH(struct MemoryMapper *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__(staticCast(pGpuResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS memmapperMap_DISPATCH(struct MemoryMapper *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__(staticCast(pGpuResource, RsResource), pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS memmapperUnmap_DISPATCH(struct MemoryMapper *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__(staticCast(pGpuResource, RsResource), pCallContext, pCpuMapping);
}

static inline NvBool memmapperShareCallback_DISPATCH(struct MemoryMapper *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__(staticCast(pGpuResource, RsResource), pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS memmapperGetRegBaseOffsetAndSize_DISPATCH(struct MemoryMapper *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__(staticCast(pGpuResource, GpuResource), pGpu, pOffset, pSize);
}

static inline NV_STATUS memmapperGetMapAddrSpace_DISPATCH(struct MemoryMapper *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__(staticCast(pGpuResource, GpuResource), pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS memmapperInternalControlForward_DISPATCH(struct MemoryMapper *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__(staticCast(pGpuResource, GpuResource), command, pParams, size);
}

static inline NvHandle memmapperGetInternalObjectHandle_DISPATCH(struct MemoryMapper *pGpuResource) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__(staticCast(pGpuResource, GpuResource));
}

static inline NvBool memmapperAccessCallback_DISPATCH(struct MemoryMapper *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__(staticCast(pResource, RsResource), pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS memmapperGetMemInterMapParams_DISPATCH(struct MemoryMapper *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__(staticCast(pRmResource, RmResource), pParams);
}

static inline NV_STATUS memmapperCheckMemInterUnmap_DISPATCH(struct MemoryMapper *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__(staticCast(pRmResource, RmResource), bSubdeviceHandleProvided);
}

static inline NV_STATUS memmapperGetMemoryMappingDescriptor_DISPATCH(struct MemoryMapper *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__(staticCast(pRmResource, RmResource), ppMemDesc);
}

static inline NV_STATUS memmapperControlSerialization_Prologue_DISPATCH(struct MemoryMapper *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void memmapperControlSerialization_Epilogue_DISPATCH(struct MemoryMapper *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS memmapperControl_Prologue_DISPATCH(struct MemoryMapper *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void memmapperControl_Epilogue_DISPATCH(struct MemoryMapper *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool memmapperCanCopy_DISPATCH(struct MemoryMapper *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__(staticCast(pResource, RsResource));
}

static inline NV_STATUS memmapperIsDuplicate_DISPATCH(struct MemoryMapper *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__(staticCast(pResource, RsResource), hMemory, pDuplicate);
}

static inline void memmapperPreDestruct_DISPATCH(struct MemoryMapper *pResource) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__(staticCast(pResource, RsResource));
}

static inline NV_STATUS memmapperControlFilter_DISPATCH(struct MemoryMapper *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool memmapperIsPartialUnmapSupported_DISPATCH(struct MemoryMapper *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__(staticCast(pResource, RsResource));
}

static inline NV_STATUS memmapperMapTo_DISPATCH(struct MemoryMapper *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__(staticCast(pResource, RsResource), pParams);
}

static inline NV_STATUS memmapperUnmapFrom_DISPATCH(struct MemoryMapper *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__(staticCast(pResource, RsResource), pParams);
}

static inline NvU32 memmapperGetRefCount_DISPATCH(struct MemoryMapper *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__(staticCast(pResource, RsResource));
}

static inline void memmapperAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct MemoryMapper *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__(pClient, staticCast(pResource, RsResource), pReference);
}

// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
NV_STATUS memmapperCtrlCmdSubmitOperations_IMPL(struct MemoryMapper *pMemoryMapper, NV00FE_CTRL_SUBMIT_OPERATIONS_PARAMS *pParams);

NV_STATUS memmapperCtrlCmdResizeQueue_IMPL(struct MemoryMapper *pMemoryMapper, NV00FE_CTRL_RESIZE_QUEUE_PARAMS *pParams);

// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


#endif // MEMORY_MAPPER_H


#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_MEM_MAPPER_NVOC_H_
