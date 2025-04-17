
#ifndef _G_SYSTEM_MEM_NVOC_H_
#define _G_SYSTEM_MEM_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2020-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_system_mem_nvoc.h"

#ifndef _SYSTEM_MEMORY_H_
#define _SYSTEM_MEMORY_H_

#include "mem_mgr/standard_mem.h"
#include "gpu/mem_mgr/heap_base.h"


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_SYSTEM_MEM_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__SystemMemory;
struct NVOC_METADATA__StandardMemory;
struct NVOC_VTABLE__SystemMemory;


struct SystemMemory {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__SystemMemory *__nvoc_metadata_ptr;
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
    struct SystemMemory *__nvoc_pbase_SystemMemory;    // sysmem

    // Vtable with 2 per-object function pointers
    NV_STATUS (*__sysmemCtrlCmdGetSurfaceNumPhysPages__)(struct SystemMemory * /*this*/, NV003E_CTRL_GET_SURFACE_NUM_PHYS_PAGES_PARAMS *);  // exported (id=0x3e0102)
    NV_STATUS (*__sysmemCtrlCmdGetSurfacePhysPages__)(struct SystemMemory * /*this*/, NV003E_CTRL_GET_SURFACE_PHYS_PAGES_PARAMS *);  // exported (id=0x3e0103)
};


// Vtable with 26 per-class function pointers
struct NVOC_VTABLE__SystemMemory {
    NvBool (*__sysmemCanCopy__)(struct SystemMemory * /*this*/);  // virtual inherited (stdmem) base (stdmem)
    NV_STATUS (*__sysmemIsDuplicate__)(struct SystemMemory * /*this*/, NvHandle, NvBool *);  // virtual inherited (mem) base (stdmem)
    NV_STATUS (*__sysmemGetMapAddrSpace__)(struct SystemMemory * /*this*/, CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (mem) base (stdmem)
    NV_STATUS (*__sysmemControl__)(struct SystemMemory * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (mem) base (stdmem)
    NV_STATUS (*__sysmemMap__)(struct SystemMemory * /*this*/, CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, RsCpuMapping *);  // virtual inherited (mem) base (stdmem)
    NV_STATUS (*__sysmemUnmap__)(struct SystemMemory * /*this*/, CALL_CONTEXT *, RsCpuMapping *);  // virtual inherited (mem) base (stdmem)
    NV_STATUS (*__sysmemGetMemInterMapParams__)(struct SystemMemory * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (mem) base (stdmem)
    NV_STATUS (*__sysmemCheckMemInterUnmap__)(struct SystemMemory * /*this*/, NvBool);  // inline virtual inherited (mem) base (stdmem) body
    NV_STATUS (*__sysmemGetMemoryMappingDescriptor__)(struct SystemMemory * /*this*/, MEMORY_DESCRIPTOR **);  // virtual inherited (mem) base (stdmem)
    NV_STATUS (*__sysmemCheckCopyPermissions__)(struct SystemMemory * /*this*/, struct OBJGPU *, struct Device *);  // inline virtual inherited (mem) base (stdmem) body
    NV_STATUS (*__sysmemIsReady__)(struct SystemMemory * /*this*/, NvBool);  // virtual inherited (mem) base (stdmem)
    NvBool (*__sysmemIsGpuMapAllowed__)(struct SystemMemory * /*this*/, struct OBJGPU *);  // inline virtual inherited (mem) base (stdmem) body
    NvBool (*__sysmemIsExportAllowed__)(struct SystemMemory * /*this*/);  // inline virtual inherited (mem) base (stdmem) body
    NvBool (*__sysmemAccessCallback__)(struct SystemMemory * /*this*/, RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (stdmem)
    NvBool (*__sysmemShareCallback__)(struct SystemMemory * /*this*/, RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (rmres) base (stdmem)
    NV_STATUS (*__sysmemControlSerialization_Prologue__)(struct SystemMemory * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (stdmem)
    void (*__sysmemControlSerialization_Epilogue__)(struct SystemMemory * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (stdmem)
    NV_STATUS (*__sysmemControl_Prologue__)(struct SystemMemory * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (stdmem)
    void (*__sysmemControl_Epilogue__)(struct SystemMemory * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (stdmem)
    void (*__sysmemPreDestruct__)(struct SystemMemory * /*this*/);  // virtual inherited (res) base (stdmem)
    NV_STATUS (*__sysmemControlFilter__)(struct SystemMemory * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (stdmem)
    NvBool (*__sysmemIsPartialUnmapSupported__)(struct SystemMemory * /*this*/);  // inline virtual inherited (res) base (stdmem) body
    NV_STATUS (*__sysmemMapTo__)(struct SystemMemory * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (stdmem)
    NV_STATUS (*__sysmemUnmapFrom__)(struct SystemMemory * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (stdmem)
    NvU32 (*__sysmemGetRefCount__)(struct SystemMemory * /*this*/);  // virtual inherited (res) base (stdmem)
    void (*__sysmemAddAdditionalDependants__)(struct RsClient *, struct SystemMemory * /*this*/, RsResourceRef *);  // virtual inherited (res) base (stdmem)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__SystemMemory {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__StandardMemory metadata__StandardMemory;
    const struct NVOC_VTABLE__SystemMemory vtable;
};

#ifndef __NVOC_CLASS_SystemMemory_TYPEDEF__
#define __NVOC_CLASS_SystemMemory_TYPEDEF__
typedef struct SystemMemory SystemMemory;
#endif /* __NVOC_CLASS_SystemMemory_TYPEDEF__ */

#ifndef __nvoc_class_id_SystemMemory
#define __nvoc_class_id_SystemMemory 0x007a98
#endif /* __nvoc_class_id_SystemMemory */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_SystemMemory;

#define __staticCast_SystemMemory(pThis) \
    ((pThis)->__nvoc_pbase_SystemMemory)

#ifdef __nvoc_system_mem_h_disabled
#define __dynamicCast_SystemMemory(pThis) ((SystemMemory*) NULL)
#else //__nvoc_system_mem_h_disabled
#define __dynamicCast_SystemMemory(pThis) \
    ((SystemMemory*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(SystemMemory)))
#endif //__nvoc_system_mem_h_disabled

NV_STATUS __nvoc_objCreateDynamic_SystemMemory(SystemMemory**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_SystemMemory(SystemMemory**, Dynamic*, NvU32, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __objCreate_SystemMemory(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_SystemMemory((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define sysmemCtrlCmdGetSurfaceNumPhysPages_FNPTR(pStandardMemory) pStandardMemory->__sysmemCtrlCmdGetSurfaceNumPhysPages__
#define sysmemCtrlCmdGetSurfaceNumPhysPages(pStandardMemory, pParams) sysmemCtrlCmdGetSurfaceNumPhysPages_DISPATCH(pStandardMemory, pParams)
#define sysmemCtrlCmdGetSurfacePhysPages_FNPTR(pStandardMemory) pStandardMemory->__sysmemCtrlCmdGetSurfacePhysPages__
#define sysmemCtrlCmdGetSurfacePhysPages(pStandardMemory, pParams) sysmemCtrlCmdGetSurfacePhysPages_DISPATCH(pStandardMemory, pParams)
#define sysmemCanCopy_FNPTR(pStandardMemory) pStandardMemory->__nvoc_base_StandardMemory.__nvoc_metadata_ptr->vtable.__stdmemCanCopy__
#define sysmemCanCopy(pStandardMemory) sysmemCanCopy_DISPATCH(pStandardMemory)
#define sysmemIsDuplicate_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsDuplicate__
#define sysmemIsDuplicate(pMemory, hMemory, pDuplicate) sysmemIsDuplicate_DISPATCH(pMemory, hMemory, pDuplicate)
#define sysmemGetMapAddrSpace_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memGetMapAddrSpace__
#define sysmemGetMapAddrSpace(pMemory, pCallContext, mapFlags, pAddrSpace) sysmemGetMapAddrSpace_DISPATCH(pMemory, pCallContext, mapFlags, pAddrSpace)
#define sysmemControl_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memControl__
#define sysmemControl(pMemory, pCallContext, pParams) sysmemControl_DISPATCH(pMemory, pCallContext, pParams)
#define sysmemMap_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memMap__
#define sysmemMap(pMemory, pCallContext, pParams, pCpuMapping) sysmemMap_DISPATCH(pMemory, pCallContext, pParams, pCpuMapping)
#define sysmemUnmap_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memUnmap__
#define sysmemUnmap(pMemory, pCallContext, pCpuMapping) sysmemUnmap_DISPATCH(pMemory, pCallContext, pCpuMapping)
#define sysmemGetMemInterMapParams_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memGetMemInterMapParams__
#define sysmemGetMemInterMapParams(pMemory, pParams) sysmemGetMemInterMapParams_DISPATCH(pMemory, pParams)
#define sysmemCheckMemInterUnmap_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memCheckMemInterUnmap__
#define sysmemCheckMemInterUnmap(pMemory, bSubdeviceHandleProvided) sysmemCheckMemInterUnmap_DISPATCH(pMemory, bSubdeviceHandleProvided)
#define sysmemGetMemoryMappingDescriptor_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memGetMemoryMappingDescriptor__
#define sysmemGetMemoryMappingDescriptor(pMemory, ppMemDesc) sysmemGetMemoryMappingDescriptor_DISPATCH(pMemory, ppMemDesc)
#define sysmemCheckCopyPermissions_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memCheckCopyPermissions__
#define sysmemCheckCopyPermissions(pMemory, pDstGpu, pDstDevice) sysmemCheckCopyPermissions_DISPATCH(pMemory, pDstGpu, pDstDevice)
#define sysmemIsReady_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsReady__
#define sysmemIsReady(pMemory, bCopyConstructorContext) sysmemIsReady_DISPATCH(pMemory, bCopyConstructorContext)
#define sysmemIsGpuMapAllowed_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsGpuMapAllowed__
#define sysmemIsGpuMapAllowed(pMemory, pGpu) sysmemIsGpuMapAllowed_DISPATCH(pMemory, pGpu)
#define sysmemIsExportAllowed_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsExportAllowed__
#define sysmemIsExportAllowed(pMemory) sysmemIsExportAllowed_DISPATCH(pMemory)
#define sysmemAccessCallback_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresAccessCallback__
#define sysmemAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) sysmemAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define sysmemShareCallback_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresShareCallback__
#define sysmemShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) sysmemShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define sysmemControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Prologue__
#define sysmemControlSerialization_Prologue(pResource, pCallContext, pParams) sysmemControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define sysmemControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Epilogue__
#define sysmemControlSerialization_Epilogue(pResource, pCallContext, pParams) sysmemControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define sysmemControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Prologue__
#define sysmemControl_Prologue(pResource, pCallContext, pParams) sysmemControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define sysmemControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Epilogue__
#define sysmemControl_Epilogue(pResource, pCallContext, pParams) sysmemControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define sysmemPreDestruct_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define sysmemPreDestruct(pResource) sysmemPreDestruct_DISPATCH(pResource)
#define sysmemControlFilter_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define sysmemControlFilter(pResource, pCallContext, pParams) sysmemControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define sysmemIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define sysmemIsPartialUnmapSupported(pResource) sysmemIsPartialUnmapSupported_DISPATCH(pResource)
#define sysmemMapTo_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define sysmemMapTo(pResource, pParams) sysmemMapTo_DISPATCH(pResource, pParams)
#define sysmemUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define sysmemUnmapFrom(pResource, pParams) sysmemUnmapFrom_DISPATCH(pResource, pParams)
#define sysmemGetRefCount_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define sysmemGetRefCount(pResource) sysmemGetRefCount_DISPATCH(pResource)
#define sysmemAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define sysmemAddAdditionalDependants(pClient, pResource, pReference) sysmemAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NV_STATUS sysmemCtrlCmdGetSurfaceNumPhysPages_DISPATCH(struct SystemMemory *pStandardMemory, NV003E_CTRL_GET_SURFACE_NUM_PHYS_PAGES_PARAMS *pParams) {
    return pStandardMemory->__sysmemCtrlCmdGetSurfaceNumPhysPages__(pStandardMemory, pParams);
}

static inline NV_STATUS sysmemCtrlCmdGetSurfacePhysPages_DISPATCH(struct SystemMemory *pStandardMemory, NV003E_CTRL_GET_SURFACE_PHYS_PAGES_PARAMS *pParams) {
    return pStandardMemory->__sysmemCtrlCmdGetSurfacePhysPages__(pStandardMemory, pParams);
}

static inline NvBool sysmemCanCopy_DISPATCH(struct SystemMemory *pStandardMemory) {
    return pStandardMemory->__nvoc_metadata_ptr->vtable.__sysmemCanCopy__(pStandardMemory);
}

static inline NV_STATUS sysmemIsDuplicate_DISPATCH(struct SystemMemory *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return pMemory->__nvoc_metadata_ptr->vtable.__sysmemIsDuplicate__(pMemory, hMemory, pDuplicate);
}

static inline NV_STATUS sysmemGetMapAddrSpace_DISPATCH(struct SystemMemory *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pMemory->__nvoc_metadata_ptr->vtable.__sysmemGetMapAddrSpace__(pMemory, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS sysmemControl_DISPATCH(struct SystemMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pMemory->__nvoc_metadata_ptr->vtable.__sysmemControl__(pMemory, pCallContext, pParams);
}

static inline NV_STATUS sysmemMap_DISPATCH(struct SystemMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pMemory->__nvoc_metadata_ptr->vtable.__sysmemMap__(pMemory, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS sysmemUnmap_DISPATCH(struct SystemMemory *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pMemory->__nvoc_metadata_ptr->vtable.__sysmemUnmap__(pMemory, pCallContext, pCpuMapping);
}

static inline NV_STATUS sysmemGetMemInterMapParams_DISPATCH(struct SystemMemory *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pMemory->__nvoc_metadata_ptr->vtable.__sysmemGetMemInterMapParams__(pMemory, pParams);
}

static inline NV_STATUS sysmemCheckMemInterUnmap_DISPATCH(struct SystemMemory *pMemory, NvBool bSubdeviceHandleProvided) {
    return pMemory->__nvoc_metadata_ptr->vtable.__sysmemCheckMemInterUnmap__(pMemory, bSubdeviceHandleProvided);
}

static inline NV_STATUS sysmemGetMemoryMappingDescriptor_DISPATCH(struct SystemMemory *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return pMemory->__nvoc_metadata_ptr->vtable.__sysmemGetMemoryMappingDescriptor__(pMemory, ppMemDesc);
}

static inline NV_STATUS sysmemCheckCopyPermissions_DISPATCH(struct SystemMemory *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return pMemory->__nvoc_metadata_ptr->vtable.__sysmemCheckCopyPermissions__(pMemory, pDstGpu, pDstDevice);
}

static inline NV_STATUS sysmemIsReady_DISPATCH(struct SystemMemory *pMemory, NvBool bCopyConstructorContext) {
    return pMemory->__nvoc_metadata_ptr->vtable.__sysmemIsReady__(pMemory, bCopyConstructorContext);
}

static inline NvBool sysmemIsGpuMapAllowed_DISPATCH(struct SystemMemory *pMemory, struct OBJGPU *pGpu) {
    return pMemory->__nvoc_metadata_ptr->vtable.__sysmemIsGpuMapAllowed__(pMemory, pGpu);
}

static inline NvBool sysmemIsExportAllowed_DISPATCH(struct SystemMemory *pMemory) {
    return pMemory->__nvoc_metadata_ptr->vtable.__sysmemIsExportAllowed__(pMemory);
}

static inline NvBool sysmemAccessCallback_DISPATCH(struct SystemMemory *pResource, RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_metadata_ptr->vtable.__sysmemAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool sysmemShareCallback_DISPATCH(struct SystemMemory *pResource, RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__nvoc_metadata_ptr->vtable.__sysmemShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS sysmemControlSerialization_Prologue_DISPATCH(struct SystemMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__sysmemControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void sysmemControlSerialization_Epilogue_DISPATCH(struct SystemMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__sysmemControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS sysmemControl_Prologue_DISPATCH(struct SystemMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__sysmemControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void sysmemControl_Epilogue_DISPATCH(struct SystemMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__sysmemControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline void sysmemPreDestruct_DISPATCH(struct SystemMemory *pResource) {
    pResource->__nvoc_metadata_ptr->vtable.__sysmemPreDestruct__(pResource);
}

static inline NV_STATUS sysmemControlFilter_DISPATCH(struct SystemMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__sysmemControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool sysmemIsPartialUnmapSupported_DISPATCH(struct SystemMemory *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__sysmemIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS sysmemMapTo_DISPATCH(struct SystemMemory *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__sysmemMapTo__(pResource, pParams);
}

static inline NV_STATUS sysmemUnmapFrom_DISPATCH(struct SystemMemory *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__sysmemUnmapFrom__(pResource, pParams);
}

static inline NvU32 sysmemGetRefCount_DISPATCH(struct SystemMemory *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__sysmemGetRefCount__(pResource);
}

static inline void sysmemAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct SystemMemory *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_metadata_ptr->vtable.__sysmemAddAdditionalDependants__(pClient, pResource, pReference);
}

NV_STATUS sysmemInitAllocRequest_HMM(struct OBJGPU *pGpu, struct SystemMemory *pSystemMemory, MEMORY_ALLOCATION_REQUEST *pAllocRequest);


#ifdef __nvoc_system_mem_h_disabled
static inline NV_STATUS sysmemInitAllocRequest(struct OBJGPU *pGpu, struct SystemMemory *pSystemMemory, MEMORY_ALLOCATION_REQUEST *pAllocRequest) {
    NV_ASSERT_FAILED_PRECOMP("SystemMemory was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_system_mem_h_disabled
#define sysmemInitAllocRequest(pGpu, pSystemMemory, pAllocRequest) sysmemInitAllocRequest_HMM(pGpu, pSystemMemory, pAllocRequest)
#endif //__nvoc_system_mem_h_disabled

#define sysmemInitAllocRequest_HAL(pGpu, pSystemMemory, pAllocRequest) sysmemInitAllocRequest(pGpu, pSystemMemory, pAllocRequest)

NV_STATUS sysmemCtrlCmdGetSurfaceNumPhysPages_IMPL(struct SystemMemory *pStandardMemory, NV003E_CTRL_GET_SURFACE_NUM_PHYS_PAGES_PARAMS *pParams);

NV_STATUS sysmemCtrlCmdGetSurfacePhysPages_IMPL(struct SystemMemory *pStandardMemory, NV003E_CTRL_GET_SURFACE_PHYS_PAGES_PARAMS *pParams);

NV_STATUS sysmemConstruct_IMPL(struct SystemMemory *arg_pStandardMemory, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_sysmemConstruct(arg_pStandardMemory, arg_pCallContext, arg_pParams) sysmemConstruct_IMPL(arg_pStandardMemory, arg_pCallContext, arg_pParams)
#undef PRIVATE_FIELD


NV_STATUS sysmemAllocResources(OBJGPU *pGpu, struct MemoryManager *pMemoryManager,
                               MEMORY_ALLOCATION_REQUEST *pAllocRequest, FB_ALLOC_INFO *pFbAllocInfo,
                               struct SystemMemory *pSystemMemory);
#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_SYSTEM_MEM_NVOC_H_
