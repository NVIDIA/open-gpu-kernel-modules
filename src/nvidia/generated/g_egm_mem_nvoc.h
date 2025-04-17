
#ifndef _G_EGM_MEM_NVOC_H_
#define _G_EGM_MEM_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2021-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_egm_mem_nvoc.h"

#ifndef _EGM_MEMORY_H_
#define _EGM_MEMORY_H_

#include "mem_mgr/video_mem.h"
#include "mem_mgr/system_mem.h"


/*!
 * Allocator for Extended GPU Memory (EGM)
 * EGM is CPU_MEM accessed like peer GPU_MEM
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_EGM_MEM_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__ExtendedGpuMemory;
struct NVOC_METADATA__StandardMemory;
struct NVOC_VTABLE__ExtendedGpuMemory;


struct ExtendedGpuMemory {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__ExtendedGpuMemory *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct StandardMemory __nvoc_base_StandardMemory;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct RsResource *__nvoc_pbase_RsResource;    // res super^4
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^4
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super^3
    struct Memory *__nvoc_pbase_Memory;    // mem super^2
    struct StandardMemory *__nvoc_pbase_StandardMemory;    // stdmem super
    struct ExtendedGpuMemory *__nvoc_pbase_ExtendedGpuMemory;    // egmmem
};


// Vtable with 26 per-class function pointers
struct NVOC_VTABLE__ExtendedGpuMemory {
    NvBool (*__egmmemCanCopy__)(struct ExtendedGpuMemory * /*this*/);  // virtual inherited (stdmem) base (stdmem)
    NV_STATUS (*__egmmemIsDuplicate__)(struct ExtendedGpuMemory * /*this*/, NvHandle, NvBool *);  // virtual inherited (mem) base (stdmem)
    NV_STATUS (*__egmmemGetMapAddrSpace__)(struct ExtendedGpuMemory * /*this*/, CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (mem) base (stdmem)
    NV_STATUS (*__egmmemControl__)(struct ExtendedGpuMemory * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (mem) base (stdmem)
    NV_STATUS (*__egmmemMap__)(struct ExtendedGpuMemory * /*this*/, CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, RsCpuMapping *);  // virtual inherited (mem) base (stdmem)
    NV_STATUS (*__egmmemUnmap__)(struct ExtendedGpuMemory * /*this*/, CALL_CONTEXT *, RsCpuMapping *);  // virtual inherited (mem) base (stdmem)
    NV_STATUS (*__egmmemGetMemInterMapParams__)(struct ExtendedGpuMemory * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (mem) base (stdmem)
    NV_STATUS (*__egmmemCheckMemInterUnmap__)(struct ExtendedGpuMemory * /*this*/, NvBool);  // inline virtual inherited (mem) base (stdmem) body
    NV_STATUS (*__egmmemGetMemoryMappingDescriptor__)(struct ExtendedGpuMemory * /*this*/, MEMORY_DESCRIPTOR **);  // virtual inherited (mem) base (stdmem)
    NV_STATUS (*__egmmemCheckCopyPermissions__)(struct ExtendedGpuMemory * /*this*/, struct OBJGPU *, struct Device *);  // inline virtual inherited (mem) base (stdmem) body
    NV_STATUS (*__egmmemIsReady__)(struct ExtendedGpuMemory * /*this*/, NvBool);  // virtual inherited (mem) base (stdmem)
    NvBool (*__egmmemIsGpuMapAllowed__)(struct ExtendedGpuMemory * /*this*/, struct OBJGPU *);  // inline virtual inherited (mem) base (stdmem) body
    NvBool (*__egmmemIsExportAllowed__)(struct ExtendedGpuMemory * /*this*/);  // inline virtual inherited (mem) base (stdmem) body
    NvBool (*__egmmemAccessCallback__)(struct ExtendedGpuMemory * /*this*/, RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (stdmem)
    NvBool (*__egmmemShareCallback__)(struct ExtendedGpuMemory * /*this*/, RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (rmres) base (stdmem)
    NV_STATUS (*__egmmemControlSerialization_Prologue__)(struct ExtendedGpuMemory * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (stdmem)
    void (*__egmmemControlSerialization_Epilogue__)(struct ExtendedGpuMemory * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (stdmem)
    NV_STATUS (*__egmmemControl_Prologue__)(struct ExtendedGpuMemory * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (stdmem)
    void (*__egmmemControl_Epilogue__)(struct ExtendedGpuMemory * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (stdmem)
    void (*__egmmemPreDestruct__)(struct ExtendedGpuMemory * /*this*/);  // virtual inherited (res) base (stdmem)
    NV_STATUS (*__egmmemControlFilter__)(struct ExtendedGpuMemory * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (stdmem)
    NvBool (*__egmmemIsPartialUnmapSupported__)(struct ExtendedGpuMemory * /*this*/);  // inline virtual inherited (res) base (stdmem) body
    NV_STATUS (*__egmmemMapTo__)(struct ExtendedGpuMemory * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (stdmem)
    NV_STATUS (*__egmmemUnmapFrom__)(struct ExtendedGpuMemory * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (stdmem)
    NvU32 (*__egmmemGetRefCount__)(struct ExtendedGpuMemory * /*this*/);  // virtual inherited (res) base (stdmem)
    void (*__egmmemAddAdditionalDependants__)(struct RsClient *, struct ExtendedGpuMemory * /*this*/, RsResourceRef *);  // virtual inherited (res) base (stdmem)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__ExtendedGpuMemory {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__StandardMemory metadata__StandardMemory;
    const struct NVOC_VTABLE__ExtendedGpuMemory vtable;
};

#ifndef __NVOC_CLASS_ExtendedGpuMemory_TYPEDEF__
#define __NVOC_CLASS_ExtendedGpuMemory_TYPEDEF__
typedef struct ExtendedGpuMemory ExtendedGpuMemory;
#endif /* __NVOC_CLASS_ExtendedGpuMemory_TYPEDEF__ */

#ifndef __nvoc_class_id_ExtendedGpuMemory
#define __nvoc_class_id_ExtendedGpuMemory 0xeffa5c
#endif /* __nvoc_class_id_ExtendedGpuMemory */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_ExtendedGpuMemory;

#define __staticCast_ExtendedGpuMemory(pThis) \
    ((pThis)->__nvoc_pbase_ExtendedGpuMemory)

#ifdef __nvoc_egm_mem_h_disabled
#define __dynamicCast_ExtendedGpuMemory(pThis) ((ExtendedGpuMemory*) NULL)
#else //__nvoc_egm_mem_h_disabled
#define __dynamicCast_ExtendedGpuMemory(pThis) \
    ((ExtendedGpuMemory*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(ExtendedGpuMemory)))
#endif //__nvoc_egm_mem_h_disabled

NV_STATUS __nvoc_objCreateDynamic_ExtendedGpuMemory(ExtendedGpuMemory**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_ExtendedGpuMemory(ExtendedGpuMemory**, Dynamic*, NvU32, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __objCreate_ExtendedGpuMemory(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_ExtendedGpuMemory((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define egmmemCanCopy_FNPTR(pStandardMemory) pStandardMemory->__nvoc_base_StandardMemory.__nvoc_metadata_ptr->vtable.__stdmemCanCopy__
#define egmmemCanCopy(pStandardMemory) egmmemCanCopy_DISPATCH(pStandardMemory)
#define egmmemIsDuplicate_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsDuplicate__
#define egmmemIsDuplicate(pMemory, hMemory, pDuplicate) egmmemIsDuplicate_DISPATCH(pMemory, hMemory, pDuplicate)
#define egmmemGetMapAddrSpace_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memGetMapAddrSpace__
#define egmmemGetMapAddrSpace(pMemory, pCallContext, mapFlags, pAddrSpace) egmmemGetMapAddrSpace_DISPATCH(pMemory, pCallContext, mapFlags, pAddrSpace)
#define egmmemControl_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memControl__
#define egmmemControl(pMemory, pCallContext, pParams) egmmemControl_DISPATCH(pMemory, pCallContext, pParams)
#define egmmemMap_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memMap__
#define egmmemMap(pMemory, pCallContext, pParams, pCpuMapping) egmmemMap_DISPATCH(pMemory, pCallContext, pParams, pCpuMapping)
#define egmmemUnmap_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memUnmap__
#define egmmemUnmap(pMemory, pCallContext, pCpuMapping) egmmemUnmap_DISPATCH(pMemory, pCallContext, pCpuMapping)
#define egmmemGetMemInterMapParams_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memGetMemInterMapParams__
#define egmmemGetMemInterMapParams(pMemory, pParams) egmmemGetMemInterMapParams_DISPATCH(pMemory, pParams)
#define egmmemCheckMemInterUnmap_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memCheckMemInterUnmap__
#define egmmemCheckMemInterUnmap(pMemory, bSubdeviceHandleProvided) egmmemCheckMemInterUnmap_DISPATCH(pMemory, bSubdeviceHandleProvided)
#define egmmemGetMemoryMappingDescriptor_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memGetMemoryMappingDescriptor__
#define egmmemGetMemoryMappingDescriptor(pMemory, ppMemDesc) egmmemGetMemoryMappingDescriptor_DISPATCH(pMemory, ppMemDesc)
#define egmmemCheckCopyPermissions_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memCheckCopyPermissions__
#define egmmemCheckCopyPermissions(pMemory, pDstGpu, pDstDevice) egmmemCheckCopyPermissions_DISPATCH(pMemory, pDstGpu, pDstDevice)
#define egmmemIsReady_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsReady__
#define egmmemIsReady(pMemory, bCopyConstructorContext) egmmemIsReady_DISPATCH(pMemory, bCopyConstructorContext)
#define egmmemIsGpuMapAllowed_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsGpuMapAllowed__
#define egmmemIsGpuMapAllowed(pMemory, pGpu) egmmemIsGpuMapAllowed_DISPATCH(pMemory, pGpu)
#define egmmemIsExportAllowed_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsExportAllowed__
#define egmmemIsExportAllowed(pMemory) egmmemIsExportAllowed_DISPATCH(pMemory)
#define egmmemAccessCallback_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresAccessCallback__
#define egmmemAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) egmmemAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define egmmemShareCallback_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresShareCallback__
#define egmmemShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) egmmemShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define egmmemControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Prologue__
#define egmmemControlSerialization_Prologue(pResource, pCallContext, pParams) egmmemControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define egmmemControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Epilogue__
#define egmmemControlSerialization_Epilogue(pResource, pCallContext, pParams) egmmemControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define egmmemControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Prologue__
#define egmmemControl_Prologue(pResource, pCallContext, pParams) egmmemControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define egmmemControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Epilogue__
#define egmmemControl_Epilogue(pResource, pCallContext, pParams) egmmemControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define egmmemPreDestruct_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define egmmemPreDestruct(pResource) egmmemPreDestruct_DISPATCH(pResource)
#define egmmemControlFilter_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define egmmemControlFilter(pResource, pCallContext, pParams) egmmemControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define egmmemIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define egmmemIsPartialUnmapSupported(pResource) egmmemIsPartialUnmapSupported_DISPATCH(pResource)
#define egmmemMapTo_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define egmmemMapTo(pResource, pParams) egmmemMapTo_DISPATCH(pResource, pParams)
#define egmmemUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define egmmemUnmapFrom(pResource, pParams) egmmemUnmapFrom_DISPATCH(pResource, pParams)
#define egmmemGetRefCount_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define egmmemGetRefCount(pResource) egmmemGetRefCount_DISPATCH(pResource)
#define egmmemAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define egmmemAddAdditionalDependants(pClient, pResource, pReference) egmmemAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NvBool egmmemCanCopy_DISPATCH(struct ExtendedGpuMemory *pStandardMemory) {
    return pStandardMemory->__nvoc_metadata_ptr->vtable.__egmmemCanCopy__(pStandardMemory);
}

static inline NV_STATUS egmmemIsDuplicate_DISPATCH(struct ExtendedGpuMemory *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return pMemory->__nvoc_metadata_ptr->vtable.__egmmemIsDuplicate__(pMemory, hMemory, pDuplicate);
}

static inline NV_STATUS egmmemGetMapAddrSpace_DISPATCH(struct ExtendedGpuMemory *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pMemory->__nvoc_metadata_ptr->vtable.__egmmemGetMapAddrSpace__(pMemory, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS egmmemControl_DISPATCH(struct ExtendedGpuMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pMemory->__nvoc_metadata_ptr->vtable.__egmmemControl__(pMemory, pCallContext, pParams);
}

static inline NV_STATUS egmmemMap_DISPATCH(struct ExtendedGpuMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pMemory->__nvoc_metadata_ptr->vtable.__egmmemMap__(pMemory, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS egmmemUnmap_DISPATCH(struct ExtendedGpuMemory *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pMemory->__nvoc_metadata_ptr->vtable.__egmmemUnmap__(pMemory, pCallContext, pCpuMapping);
}

static inline NV_STATUS egmmemGetMemInterMapParams_DISPATCH(struct ExtendedGpuMemory *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pMemory->__nvoc_metadata_ptr->vtable.__egmmemGetMemInterMapParams__(pMemory, pParams);
}

static inline NV_STATUS egmmemCheckMemInterUnmap_DISPATCH(struct ExtendedGpuMemory *pMemory, NvBool bSubdeviceHandleProvided) {
    return pMemory->__nvoc_metadata_ptr->vtable.__egmmemCheckMemInterUnmap__(pMemory, bSubdeviceHandleProvided);
}

static inline NV_STATUS egmmemGetMemoryMappingDescriptor_DISPATCH(struct ExtendedGpuMemory *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return pMemory->__nvoc_metadata_ptr->vtable.__egmmemGetMemoryMappingDescriptor__(pMemory, ppMemDesc);
}

static inline NV_STATUS egmmemCheckCopyPermissions_DISPATCH(struct ExtendedGpuMemory *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return pMemory->__nvoc_metadata_ptr->vtable.__egmmemCheckCopyPermissions__(pMemory, pDstGpu, pDstDevice);
}

static inline NV_STATUS egmmemIsReady_DISPATCH(struct ExtendedGpuMemory *pMemory, NvBool bCopyConstructorContext) {
    return pMemory->__nvoc_metadata_ptr->vtable.__egmmemIsReady__(pMemory, bCopyConstructorContext);
}

static inline NvBool egmmemIsGpuMapAllowed_DISPATCH(struct ExtendedGpuMemory *pMemory, struct OBJGPU *pGpu) {
    return pMemory->__nvoc_metadata_ptr->vtable.__egmmemIsGpuMapAllowed__(pMemory, pGpu);
}

static inline NvBool egmmemIsExportAllowed_DISPATCH(struct ExtendedGpuMemory *pMemory) {
    return pMemory->__nvoc_metadata_ptr->vtable.__egmmemIsExportAllowed__(pMemory);
}

static inline NvBool egmmemAccessCallback_DISPATCH(struct ExtendedGpuMemory *pResource, RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_metadata_ptr->vtable.__egmmemAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool egmmemShareCallback_DISPATCH(struct ExtendedGpuMemory *pResource, RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__nvoc_metadata_ptr->vtable.__egmmemShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS egmmemControlSerialization_Prologue_DISPATCH(struct ExtendedGpuMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__egmmemControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void egmmemControlSerialization_Epilogue_DISPATCH(struct ExtendedGpuMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__egmmemControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS egmmemControl_Prologue_DISPATCH(struct ExtendedGpuMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__egmmemControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void egmmemControl_Epilogue_DISPATCH(struct ExtendedGpuMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__egmmemControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline void egmmemPreDestruct_DISPATCH(struct ExtendedGpuMemory *pResource) {
    pResource->__nvoc_metadata_ptr->vtable.__egmmemPreDestruct__(pResource);
}

static inline NV_STATUS egmmemControlFilter_DISPATCH(struct ExtendedGpuMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__egmmemControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool egmmemIsPartialUnmapSupported_DISPATCH(struct ExtendedGpuMemory *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__egmmemIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS egmmemMapTo_DISPATCH(struct ExtendedGpuMemory *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__egmmemMapTo__(pResource, pParams);
}

static inline NV_STATUS egmmemUnmapFrom_DISPATCH(struct ExtendedGpuMemory *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__egmmemUnmapFrom__(pResource, pParams);
}

static inline NvU32 egmmemGetRefCount_DISPATCH(struct ExtendedGpuMemory *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__egmmemGetRefCount__(pResource);
}

static inline void egmmemAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct ExtendedGpuMemory *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_metadata_ptr->vtable.__egmmemAddAdditionalDependants__(pClient, pResource, pReference);
}

NV_STATUS egmmemConstruct_IMPL(struct ExtendedGpuMemory *arg_pStandardMemory, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_egmmemConstruct(arg_pStandardMemory, arg_pCallContext, arg_pParams) egmmemConstruct_IMPL(arg_pStandardMemory, arg_pCallContext, arg_pParams)
NV_STATUS egmmemValidateParams_IMPL(struct OBJGPU *pGpu, struct RmClient *pRmClient, NV_MEMORY_ALLOCATION_PARAMS *pAllocData);

#define egmmemValidateParams(pGpu, pRmClient, pAllocData) egmmemValidateParams_IMPL(pGpu, pRmClient, pAllocData)
#undef PRIVATE_FIELD


NV_STATUS egmmemAllocResources(OBJGPU *pGpu, struct MemoryManager *pMemoryManager,
                               MEMORY_ALLOCATION_REQUEST *pAllocRequest, FB_ALLOC_INFO *pFbAllocInfo);

#endif // _EGM_MEMORY_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_EGM_MEM_NVOC_H_
