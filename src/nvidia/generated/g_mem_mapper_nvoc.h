
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

#ifndef __NVOC_CLASS_Subdevice_TYPEDEF__
#define __NVOC_CLASS_Subdevice_TYPEDEF__
typedef struct Subdevice Subdevice;
#endif /* __NVOC_CLASS_Subdevice_TYPEDEF__ */

#ifndef __nvoc_class_id_Subdevice
#define __nvoc_class_id_Subdevice 0x4b01b3
#endif /* __nvoc_class_id_Subdevice */



struct Memory;

#ifndef __NVOC_CLASS_Memory_TYPEDEF__
#define __NVOC_CLASS_Memory_TYPEDEF__
typedef struct Memory Memory;
#endif /* __NVOC_CLASS_Memory_TYPEDEF__ */

#ifndef __nvoc_class_id_Memory
#define __nvoc_class_id_Memory 0x4789f2
#endif /* __nvoc_class_id_Memory */



struct OBJGPU;

#ifndef __NVOC_CLASS_OBJGPU_TYPEDEF__
#define __NVOC_CLASS_OBJGPU_TYPEDEF__
typedef struct OBJGPU OBJGPU;
#endif /* __NVOC_CLASS_OBJGPU_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJGPU
#define __nvoc_class_id_OBJGPU 0x7ef3cb
#endif /* __nvoc_class_id_OBJGPU */



struct SemaphoreSurface;

#ifndef __NVOC_CLASS_SemaphoreSurface_TYPEDEF__
#define __NVOC_CLASS_SemaphoreSurface_TYPEDEF__
typedef struct SemaphoreSurface SemaphoreSurface;
#endif /* __NVOC_CLASS_SemaphoreSurface_TYPEDEF__ */

#ifndef __nvoc_class_id_SemaphoreSurface
#define __nvoc_class_id_SemaphoreSurface 0xeabc69
#endif /* __nvoc_class_id_SemaphoreSurface */



struct MemoryMapper;

#ifndef __NVOC_CLASS_MemoryMapper_TYPEDEF__
#define __NVOC_CLASS_MemoryMapper_TYPEDEF__
typedef struct MemoryMapper MemoryMapper;
#endif /* __NVOC_CLASS_MemoryMapper_TYPEDEF__ */

#ifndef __nvoc_class_id_MemoryMapper
#define __nvoc_class_id_MemoryMapper 0xb8e4a2
#endif /* __nvoc_class_id_MemoryMapper */



// MemoryMapper can't wait for workers to finish in destructor due to locking constraints
// Instead set pMemoryMapper to NULL and leave the params structure until all workers are done
typedef struct
{
    struct MemoryMapper *pMemoryMapper;
    NvU32         numRefs;
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


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__MemoryMapper;
struct NVOC_METADATA__GpuResource;
struct NVOC_VTABLE__MemoryMapper;


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

    // Vtable with 2 per-object function pointers
    NV_STATUS (*__memmapperCtrlCmdSubmitOperations__)(struct MemoryMapper * /*this*/, NV00FE_CTRL_SUBMIT_OPERATIONS_PARAMS *);  // exported (id=0xfe0101)
    NV_STATUS (*__memmapperCtrlCmdResizeQueue__)(struct MemoryMapper * /*this*/, NV00FE_CTRL_RESIZE_QUEUE_PARAMS *);  // exported (id=0xfe0102)

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


// Vtable with 25 per-class function pointers
struct NVOC_VTABLE__MemoryMapper {
    NV_STATUS (*__memmapperControl__)(struct MemoryMapper * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__memmapperMap__)(struct MemoryMapper * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__memmapperUnmap__)(struct MemoryMapper * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NvBool (*__memmapperShareCallback__)(struct MemoryMapper * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__memmapperGetRegBaseOffsetAndSize__)(struct MemoryMapper * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__memmapperGetMapAddrSpace__)(struct MemoryMapper * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__memmapperInternalControlForward__)(struct MemoryMapper * /*this*/, NvU32, void *, NvU32);  // virtual inherited (gpures) base (gpures)
    NvHandle (*__memmapperGetInternalObjectHandle__)(struct MemoryMapper * /*this*/);  // virtual inherited (gpures) base (gpures)
    NvBool (*__memmapperAccessCallback__)(struct MemoryMapper * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__memmapperGetMemInterMapParams__)(struct MemoryMapper * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__memmapperCheckMemInterUnmap__)(struct MemoryMapper * /*this*/, NvBool);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__memmapperGetMemoryMappingDescriptor__)(struct MemoryMapper * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__memmapperControlSerialization_Prologue__)(struct MemoryMapper * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__memmapperControlSerialization_Epilogue__)(struct MemoryMapper * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__memmapperControl_Prologue__)(struct MemoryMapper * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__memmapperControl_Epilogue__)(struct MemoryMapper * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NvBool (*__memmapperCanCopy__)(struct MemoryMapper * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__memmapperIsDuplicate__)(struct MemoryMapper * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (gpures)
    void (*__memmapperPreDestruct__)(struct MemoryMapper * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__memmapperControlFilter__)(struct MemoryMapper * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (gpures)
    NvBool (*__memmapperIsPartialUnmapSupported__)(struct MemoryMapper * /*this*/);  // inline virtual inherited (res) base (gpures) body
    NV_STATUS (*__memmapperMapTo__)(struct MemoryMapper * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__memmapperUnmapFrom__)(struct MemoryMapper * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (gpures)
    NvU32 (*__memmapperGetRefCount__)(struct MemoryMapper * /*this*/);  // virtual inherited (res) base (gpures)
    void (*__memmapperAddAdditionalDependants__)(struct RsClient *, struct MemoryMapper * /*this*/, RsResourceRef *);  // virtual inherited (res) base (gpures)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__MemoryMapper {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResource metadata__GpuResource;
    const struct NVOC_VTABLE__MemoryMapper vtable;
};

#ifndef __NVOC_CLASS_MemoryMapper_TYPEDEF__
#define __NVOC_CLASS_MemoryMapper_TYPEDEF__
typedef struct MemoryMapper MemoryMapper;
#endif /* __NVOC_CLASS_MemoryMapper_TYPEDEF__ */

#ifndef __nvoc_class_id_MemoryMapper
#define __nvoc_class_id_MemoryMapper 0xb8e4a2
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

NV_STATUS __nvoc_objCreateDynamic_MemoryMapper(MemoryMapper**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_MemoryMapper(MemoryMapper**, Dynamic*, NvU32, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __objCreate_MemoryMapper(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_MemoryMapper((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define memmapperCtrlCmdSubmitOperations_FNPTR(pMemoryMapper) pMemoryMapper->__memmapperCtrlCmdSubmitOperations__
#define memmapperCtrlCmdSubmitOperations(pMemoryMapper, pParams) memmapperCtrlCmdSubmitOperations_DISPATCH(pMemoryMapper, pParams)
#define memmapperCtrlCmdResizeQueue_FNPTR(pMemoryMapper) pMemoryMapper->__memmapperCtrlCmdResizeQueue__
#define memmapperCtrlCmdResizeQueue(pMemoryMapper, pParams) memmapperCtrlCmdResizeQueue_DISPATCH(pMemoryMapper, pParams)
#define memmapperControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresControl__
#define memmapperControl(pGpuResource, pCallContext, pParams) memmapperControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define memmapperMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresMap__
#define memmapperMap(pGpuResource, pCallContext, pParams, pCpuMapping) memmapperMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define memmapperUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresUnmap__
#define memmapperUnmap(pGpuResource, pCallContext, pCpuMapping) memmapperUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define memmapperShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresShareCallback__
#define memmapperShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) memmapperShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define memmapperGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__
#define memmapperGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) memmapperGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define memmapperGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__
#define memmapperGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) memmapperGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define memmapperInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__
#define memmapperInternalControlForward(pGpuResource, command, pParams, size) memmapperInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define memmapperGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__
#define memmapperGetInternalObjectHandle(pGpuResource) memmapperGetInternalObjectHandle_DISPATCH(pGpuResource)
#define memmapperAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresAccessCallback__
#define memmapperAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) memmapperAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define memmapperGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define memmapperGetMemInterMapParams(pRmResource, pParams) memmapperGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define memmapperCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define memmapperCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) memmapperCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define memmapperGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define memmapperGetMemoryMappingDescriptor(pRmResource, ppMemDesc) memmapperGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define memmapperControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Prologue__
#define memmapperControlSerialization_Prologue(pResource, pCallContext, pParams) memmapperControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define memmapperControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Epilogue__
#define memmapperControlSerialization_Epilogue(pResource, pCallContext, pParams) memmapperControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define memmapperControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Prologue__
#define memmapperControl_Prologue(pResource, pCallContext, pParams) memmapperControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define memmapperControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Epilogue__
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
static inline NV_STATUS memmapperCtrlCmdSubmitOperations_DISPATCH(struct MemoryMapper *pMemoryMapper, NV00FE_CTRL_SUBMIT_OPERATIONS_PARAMS *pParams) {
    return pMemoryMapper->__memmapperCtrlCmdSubmitOperations__(pMemoryMapper, pParams);
}

static inline NV_STATUS memmapperCtrlCmdResizeQueue_DISPATCH(struct MemoryMapper *pMemoryMapper, NV00FE_CTRL_RESIZE_QUEUE_PARAMS *pParams) {
    return pMemoryMapper->__memmapperCtrlCmdResizeQueue__(pMemoryMapper, pParams);
}

static inline NV_STATUS memmapperControl_DISPATCH(struct MemoryMapper *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__memmapperControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS memmapperMap_DISPATCH(struct MemoryMapper *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__memmapperMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS memmapperUnmap_DISPATCH(struct MemoryMapper *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__memmapperUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NvBool memmapperShareCallback_DISPATCH(struct MemoryMapper *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__memmapperShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS memmapperGetRegBaseOffsetAndSize_DISPATCH(struct MemoryMapper *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__memmapperGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS memmapperGetMapAddrSpace_DISPATCH(struct MemoryMapper *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__memmapperGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS memmapperInternalControlForward_DISPATCH(struct MemoryMapper *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__memmapperInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NvHandle memmapperGetInternalObjectHandle_DISPATCH(struct MemoryMapper *pGpuResource) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__memmapperGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool memmapperAccessCallback_DISPATCH(struct MemoryMapper *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_metadata_ptr->vtable.__memmapperAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS memmapperGetMemInterMapParams_DISPATCH(struct MemoryMapper *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__memmapperGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS memmapperCheckMemInterUnmap_DISPATCH(struct MemoryMapper *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__memmapperCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS memmapperGetMemoryMappingDescriptor_DISPATCH(struct MemoryMapper *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__memmapperGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS memmapperControlSerialization_Prologue_DISPATCH(struct MemoryMapper *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__memmapperControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void memmapperControlSerialization_Epilogue_DISPATCH(struct MemoryMapper *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__memmapperControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS memmapperControl_Prologue_DISPATCH(struct MemoryMapper *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__memmapperControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void memmapperControl_Epilogue_DISPATCH(struct MemoryMapper *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__memmapperControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool memmapperCanCopy_DISPATCH(struct MemoryMapper *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__memmapperCanCopy__(pResource);
}

static inline NV_STATUS memmapperIsDuplicate_DISPATCH(struct MemoryMapper *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_metadata_ptr->vtable.__memmapperIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void memmapperPreDestruct_DISPATCH(struct MemoryMapper *pResource) {
    pResource->__nvoc_metadata_ptr->vtable.__memmapperPreDestruct__(pResource);
}

static inline NV_STATUS memmapperControlFilter_DISPATCH(struct MemoryMapper *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__memmapperControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool memmapperIsPartialUnmapSupported_DISPATCH(struct MemoryMapper *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__memmapperIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS memmapperMapTo_DISPATCH(struct MemoryMapper *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__memmapperMapTo__(pResource, pParams);
}

static inline NV_STATUS memmapperUnmapFrom_DISPATCH(struct MemoryMapper *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__memmapperUnmapFrom__(pResource, pParams);
}

static inline NvU32 memmapperGetRefCount_DISPATCH(struct MemoryMapper *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__memmapperGetRefCount__(pResource);
}

static inline void memmapperAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct MemoryMapper *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_metadata_ptr->vtable.__memmapperAddAdditionalDependants__(pClient, pResource, pReference);
}

NV_STATUS memmapperCtrlCmdSubmitOperations_IMPL(struct MemoryMapper *pMemoryMapper, NV00FE_CTRL_SUBMIT_OPERATIONS_PARAMS *pParams);

NV_STATUS memmapperCtrlCmdResizeQueue_IMPL(struct MemoryMapper *pMemoryMapper, NV00FE_CTRL_RESIZE_QUEUE_PARAMS *pParams);

NV_STATUS memmapperConstruct_IMPL(struct MemoryMapper *arg_pMemoryMapper, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_memmapperConstruct(arg_pMemoryMapper, arg_pCallContext, arg_pParams) memmapperConstruct_IMPL(arg_pMemoryMapper, arg_pCallContext, arg_pParams)
void memmapperDestruct_IMPL(struct MemoryMapper *pMemoryMapper);

#define __nvoc_memmapperDestruct(pMemoryMapper) memmapperDestruct_IMPL(pMemoryMapper)
void memmapperQueueWork_IMPL(struct MemoryMapper *pMemoryMapper);

#ifdef __nvoc_mem_mapper_h_disabled
static inline void memmapperQueueWork(struct MemoryMapper *pMemoryMapper) {
    NV_ASSERT_FAILED_PRECOMP("MemoryMapper was disabled!");
}
#else //__nvoc_mem_mapper_h_disabled
#define memmapperQueueWork(pMemoryMapper) memmapperQueueWork_IMPL(pMemoryMapper)
#endif //__nvoc_mem_mapper_h_disabled

#undef PRIVATE_FIELD


#endif // MEMORY_MAPPER_H


#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_MEM_MAPPER_NVOC_H_
