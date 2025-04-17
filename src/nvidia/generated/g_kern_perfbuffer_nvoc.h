
#ifndef _G_KERN_PERFBUFFER_NVOC_H_
#define _G_KERN_PERFBUFFER_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2021-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_kern_perfbuffer_nvoc.h"

#ifndef KERN_PERFBUFFER_H
#define KERN_PERFBUFFER_H

#include "rmapi/client.h"
#include "gpu/gpu_resource.h"
#include "gpu/gpu_halspec.h"

/*!
 * Definition of PerfBuffer resource class
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_KERN_PERFBUFFER_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__PerfBuffer;
struct NVOC_METADATA__GpuResource;
struct NVOC_VTABLE__PerfBuffer;


struct PerfBuffer {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__PerfBuffer *__nvoc_metadata_ptr;
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
    struct PerfBuffer *__nvoc_pbase_PerfBuffer;    // perfbuffer

    // Vtable with 1 per-object function pointer
    NV_STATUS (*__perfbufferConstructHal__)(struct PerfBuffer * /*this*/, struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);  // halified (2 hals) body

    // Data members
    void *pObject;
};


// Vtable with 25 per-class function pointers
struct NVOC_VTABLE__PerfBuffer {
    NV_STATUS (*__perfbufferControl__)(struct PerfBuffer * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__perfbufferMap__)(struct PerfBuffer * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__perfbufferUnmap__)(struct PerfBuffer * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NvBool (*__perfbufferShareCallback__)(struct PerfBuffer * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__perfbufferGetRegBaseOffsetAndSize__)(struct PerfBuffer * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__perfbufferGetMapAddrSpace__)(struct PerfBuffer * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__perfbufferInternalControlForward__)(struct PerfBuffer * /*this*/, NvU32, void *, NvU32);  // virtual inherited (gpures) base (gpures)
    NvHandle (*__perfbufferGetInternalObjectHandle__)(struct PerfBuffer * /*this*/);  // virtual inherited (gpures) base (gpures)
    NvBool (*__perfbufferAccessCallback__)(struct PerfBuffer * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__perfbufferGetMemInterMapParams__)(struct PerfBuffer * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__perfbufferCheckMemInterUnmap__)(struct PerfBuffer * /*this*/, NvBool);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__perfbufferGetMemoryMappingDescriptor__)(struct PerfBuffer * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__perfbufferControlSerialization_Prologue__)(struct PerfBuffer * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__perfbufferControlSerialization_Epilogue__)(struct PerfBuffer * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__perfbufferControl_Prologue__)(struct PerfBuffer * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__perfbufferControl_Epilogue__)(struct PerfBuffer * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NvBool (*__perfbufferCanCopy__)(struct PerfBuffer * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__perfbufferIsDuplicate__)(struct PerfBuffer * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (gpures)
    void (*__perfbufferPreDestruct__)(struct PerfBuffer * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__perfbufferControlFilter__)(struct PerfBuffer * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (gpures)
    NvBool (*__perfbufferIsPartialUnmapSupported__)(struct PerfBuffer * /*this*/);  // inline virtual inherited (res) base (gpures) body
    NV_STATUS (*__perfbufferMapTo__)(struct PerfBuffer * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__perfbufferUnmapFrom__)(struct PerfBuffer * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (gpures)
    NvU32 (*__perfbufferGetRefCount__)(struct PerfBuffer * /*this*/);  // virtual inherited (res) base (gpures)
    void (*__perfbufferAddAdditionalDependants__)(struct RsClient *, struct PerfBuffer * /*this*/, RsResourceRef *);  // virtual inherited (res) base (gpures)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__PerfBuffer {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResource metadata__GpuResource;
    const struct NVOC_VTABLE__PerfBuffer vtable;
};

#ifndef __NVOC_CLASS_PerfBuffer_TYPEDEF__
#define __NVOC_CLASS_PerfBuffer_TYPEDEF__
typedef struct PerfBuffer PerfBuffer;
#endif /* __NVOC_CLASS_PerfBuffer_TYPEDEF__ */

#ifndef __nvoc_class_id_PerfBuffer
#define __nvoc_class_id_PerfBuffer 0x4bc43b
#endif /* __nvoc_class_id_PerfBuffer */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_PerfBuffer;

#define __staticCast_PerfBuffer(pThis) \
    ((pThis)->__nvoc_pbase_PerfBuffer)

#ifdef __nvoc_kern_perfbuffer_h_disabled
#define __dynamicCast_PerfBuffer(pThis) ((PerfBuffer*) NULL)
#else //__nvoc_kern_perfbuffer_h_disabled
#define __dynamicCast_PerfBuffer(pThis) \
    ((PerfBuffer*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(PerfBuffer)))
#endif //__nvoc_kern_perfbuffer_h_disabled

NV_STATUS __nvoc_objCreateDynamic_PerfBuffer(PerfBuffer**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_PerfBuffer(PerfBuffer**, Dynamic*, NvU32, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __objCreate_PerfBuffer(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_PerfBuffer((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define perfbufferConstructHal_FNPTR(pResource) pResource->__perfbufferConstructHal__
#define perfbufferConstructHal(pResource, pCallContext, pParams) perfbufferConstructHal_DISPATCH(pResource, pCallContext, pParams)
#define perfbufferConstructHal_HAL(pResource, pCallContext, pParams) perfbufferConstructHal_DISPATCH(pResource, pCallContext, pParams)
#define perfbufferControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresControl__
#define perfbufferControl(pGpuResource, pCallContext, pParams) perfbufferControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define perfbufferMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresMap__
#define perfbufferMap(pGpuResource, pCallContext, pParams, pCpuMapping) perfbufferMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define perfbufferUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresUnmap__
#define perfbufferUnmap(pGpuResource, pCallContext, pCpuMapping) perfbufferUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define perfbufferShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresShareCallback__
#define perfbufferShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) perfbufferShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define perfbufferGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__
#define perfbufferGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) perfbufferGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define perfbufferGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__
#define perfbufferGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) perfbufferGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define perfbufferInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__
#define perfbufferInternalControlForward(pGpuResource, command, pParams, size) perfbufferInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define perfbufferGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__
#define perfbufferGetInternalObjectHandle(pGpuResource) perfbufferGetInternalObjectHandle_DISPATCH(pGpuResource)
#define perfbufferAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresAccessCallback__
#define perfbufferAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) perfbufferAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define perfbufferGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define perfbufferGetMemInterMapParams(pRmResource, pParams) perfbufferGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define perfbufferCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define perfbufferCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) perfbufferCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define perfbufferGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define perfbufferGetMemoryMappingDescriptor(pRmResource, ppMemDesc) perfbufferGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define perfbufferControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Prologue__
#define perfbufferControlSerialization_Prologue(pResource, pCallContext, pParams) perfbufferControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define perfbufferControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Epilogue__
#define perfbufferControlSerialization_Epilogue(pResource, pCallContext, pParams) perfbufferControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define perfbufferControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Prologue__
#define perfbufferControl_Prologue(pResource, pCallContext, pParams) perfbufferControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define perfbufferControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Epilogue__
#define perfbufferControl_Epilogue(pResource, pCallContext, pParams) perfbufferControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define perfbufferCanCopy_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define perfbufferCanCopy(pResource) perfbufferCanCopy_DISPATCH(pResource)
#define perfbufferIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define perfbufferIsDuplicate(pResource, hMemory, pDuplicate) perfbufferIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define perfbufferPreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define perfbufferPreDestruct(pResource) perfbufferPreDestruct_DISPATCH(pResource)
#define perfbufferControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define perfbufferControlFilter(pResource, pCallContext, pParams) perfbufferControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define perfbufferIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define perfbufferIsPartialUnmapSupported(pResource) perfbufferIsPartialUnmapSupported_DISPATCH(pResource)
#define perfbufferMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define perfbufferMapTo(pResource, pParams) perfbufferMapTo_DISPATCH(pResource, pParams)
#define perfbufferUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define perfbufferUnmapFrom(pResource, pParams) perfbufferUnmapFrom_DISPATCH(pResource, pParams)
#define perfbufferGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define perfbufferGetRefCount(pResource) perfbufferGetRefCount_DISPATCH(pResource)
#define perfbufferAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define perfbufferAddAdditionalDependants(pClient, pResource, pReference) perfbufferAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NV_STATUS perfbufferConstructHal_DISPATCH(struct PerfBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {
    return pResource->__perfbufferConstructHal__(pResource, pCallContext, pParams);
}

static inline NV_STATUS perfbufferControl_DISPATCH(struct PerfBuffer *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__perfbufferControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS perfbufferMap_DISPATCH(struct PerfBuffer *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__perfbufferMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS perfbufferUnmap_DISPATCH(struct PerfBuffer *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__perfbufferUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NvBool perfbufferShareCallback_DISPATCH(struct PerfBuffer *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__perfbufferShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS perfbufferGetRegBaseOffsetAndSize_DISPATCH(struct PerfBuffer *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__perfbufferGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS perfbufferGetMapAddrSpace_DISPATCH(struct PerfBuffer *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__perfbufferGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS perfbufferInternalControlForward_DISPATCH(struct PerfBuffer *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__perfbufferInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NvHandle perfbufferGetInternalObjectHandle_DISPATCH(struct PerfBuffer *pGpuResource) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__perfbufferGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool perfbufferAccessCallback_DISPATCH(struct PerfBuffer *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_metadata_ptr->vtable.__perfbufferAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS perfbufferGetMemInterMapParams_DISPATCH(struct PerfBuffer *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__perfbufferGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS perfbufferCheckMemInterUnmap_DISPATCH(struct PerfBuffer *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__perfbufferCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS perfbufferGetMemoryMappingDescriptor_DISPATCH(struct PerfBuffer *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__perfbufferGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS perfbufferControlSerialization_Prologue_DISPATCH(struct PerfBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__perfbufferControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void perfbufferControlSerialization_Epilogue_DISPATCH(struct PerfBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__perfbufferControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS perfbufferControl_Prologue_DISPATCH(struct PerfBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__perfbufferControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void perfbufferControl_Epilogue_DISPATCH(struct PerfBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__perfbufferControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool perfbufferCanCopy_DISPATCH(struct PerfBuffer *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__perfbufferCanCopy__(pResource);
}

static inline NV_STATUS perfbufferIsDuplicate_DISPATCH(struct PerfBuffer *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_metadata_ptr->vtable.__perfbufferIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void perfbufferPreDestruct_DISPATCH(struct PerfBuffer *pResource) {
    pResource->__nvoc_metadata_ptr->vtable.__perfbufferPreDestruct__(pResource);
}

static inline NV_STATUS perfbufferControlFilter_DISPATCH(struct PerfBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__perfbufferControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool perfbufferIsPartialUnmapSupported_DISPATCH(struct PerfBuffer *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__perfbufferIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS perfbufferMapTo_DISPATCH(struct PerfBuffer *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__perfbufferMapTo__(pResource, pParams);
}

static inline NV_STATUS perfbufferUnmapFrom_DISPATCH(struct PerfBuffer *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__perfbufferUnmapFrom__(pResource, pParams);
}

static inline NvU32 perfbufferGetRefCount_DISPATCH(struct PerfBuffer *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__perfbufferGetRefCount__(pResource);
}

static inline void perfbufferAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct PerfBuffer *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_metadata_ptr->vtable.__perfbufferAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline void perfbufferDestruct_b3696a(struct PerfBuffer *pResource) {
    return;
}


#define __nvoc_perfbufferDestruct(pResource) perfbufferDestruct_b3696a(pResource)
NV_STATUS perfbufferConstructHal_KERNEL(struct PerfBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);

static inline NV_STATUS perfbufferConstructHal_46f6a7(struct PerfBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS __nvoc_perfbufferConstruct(struct PerfBuffer *arg_pResource, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams) {
    return perfbufferConstructHal(arg_pResource, arg_pCallContext, arg_pParams);
}

NV_STATUS perfbufferPrivilegeCheck_IMPL(struct PerfBuffer *pPerfBuffer);

#ifdef __nvoc_kern_perfbuffer_h_disabled
static inline NV_STATUS perfbufferPrivilegeCheck(struct PerfBuffer *pPerfBuffer) {
    NV_ASSERT_FAILED_PRECOMP("PerfBuffer was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_perfbuffer_h_disabled
#define perfbufferPrivilegeCheck(pPerfBuffer) perfbufferPrivilegeCheck_IMPL(pPerfBuffer)
#endif //__nvoc_kern_perfbuffer_h_disabled

#undef PRIVATE_FIELD


#endif // KERN_PERFBUFFER_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERN_PERFBUFFER_NVOC_H_
