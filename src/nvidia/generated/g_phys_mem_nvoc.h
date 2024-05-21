
#ifndef _G_PHYS_MEM_NVOC_H_
#define _G_PHYS_MEM_NVOC_H_
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
 * SPDX-FileCopyrightText: Copyright (c) 2018-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_phys_mem_nvoc.h"

#ifndef _PHYSICAL_MEMORY_H_
#define _PHYSICAL_MEMORY_H_

#include "mem_mgr/mem.h"

/*!
 * Allocator for NV01_MEMORY_LOCAL_PHYSICAL
 *
 * Linear view for all video memory (similar to /dev/mem).
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_PHYS_MEM_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct PhysicalMemory {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct Memory __nvoc_base_Memory;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^4
    struct RsResource *__nvoc_pbase_RsResource;    // res super^3
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^3
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super^2
    struct Memory *__nvoc_pbase_Memory;    // mem super
    struct PhysicalMemory *__nvoc_pbase_PhysicalMemory;    // physmem

    // Vtable with 26 per-object function pointers
    NvBool (*__physmemCanCopy__)(struct PhysicalMemory * /*this*/);  // virtual override (res) base (mem)
    NV_STATUS (*__physmemIsDuplicate__)(struct PhysicalMemory * /*this*/, NvHandle, NvBool *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__physmemGetMapAddrSpace__)(struct PhysicalMemory * /*this*/, CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__physmemControl__)(struct PhysicalMemory * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__physmemMap__)(struct PhysicalMemory * /*this*/, CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, RsCpuMapping *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__physmemUnmap__)(struct PhysicalMemory * /*this*/, CALL_CONTEXT *, RsCpuMapping *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__physmemGetMemInterMapParams__)(struct PhysicalMemory * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__physmemCheckMemInterUnmap__)(struct PhysicalMemory * /*this*/, NvBool);  // inline virtual inherited (mem) base (mem) body
    NV_STATUS (*__physmemGetMemoryMappingDescriptor__)(struct PhysicalMemory * /*this*/, MEMORY_DESCRIPTOR **);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__physmemCheckCopyPermissions__)(struct PhysicalMemory * /*this*/, struct OBJGPU *, struct Device *);  // inline virtual inherited (mem) base (mem) body
    NV_STATUS (*__physmemIsReady__)(struct PhysicalMemory * /*this*/, NvBool);  // virtual inherited (mem) base (mem)
    NvBool (*__physmemIsGpuMapAllowed__)(struct PhysicalMemory * /*this*/, struct OBJGPU *);  // inline virtual inherited (mem) base (mem) body
    NvBool (*__physmemIsExportAllowed__)(struct PhysicalMemory * /*this*/);  // inline virtual inherited (mem) base (mem) body
    NvBool (*__physmemAccessCallback__)(struct PhysicalMemory * /*this*/, RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (mem)
    NvBool (*__physmemShareCallback__)(struct PhysicalMemory * /*this*/, RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (rmres) base (mem)
    NV_STATUS (*__physmemControlSerialization_Prologue__)(struct PhysicalMemory * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (mem)
    void (*__physmemControlSerialization_Epilogue__)(struct PhysicalMemory * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (mem)
    NV_STATUS (*__physmemControl_Prologue__)(struct PhysicalMemory * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (mem)
    void (*__physmemControl_Epilogue__)(struct PhysicalMemory * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (mem)
    void (*__physmemPreDestruct__)(struct PhysicalMemory * /*this*/);  // virtual inherited (res) base (mem)
    NV_STATUS (*__physmemControlFilter__)(struct PhysicalMemory * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (mem)
    NvBool (*__physmemIsPartialUnmapSupported__)(struct PhysicalMemory * /*this*/);  // inline virtual inherited (res) base (mem) body
    NV_STATUS (*__physmemMapTo__)(struct PhysicalMemory * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (mem)
    NV_STATUS (*__physmemUnmapFrom__)(struct PhysicalMemory * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (mem)
    NvU32 (*__physmemGetRefCount__)(struct PhysicalMemory * /*this*/);  // virtual inherited (res) base (mem)
    void (*__physmemAddAdditionalDependants__)(struct RsClient *, struct PhysicalMemory * /*this*/, RsResourceRef *);  // virtual inherited (res) base (mem)
};

#ifndef __NVOC_CLASS_PhysicalMemory_TYPEDEF__
#define __NVOC_CLASS_PhysicalMemory_TYPEDEF__
typedef struct PhysicalMemory PhysicalMemory;
#endif /* __NVOC_CLASS_PhysicalMemory_TYPEDEF__ */

#ifndef __nvoc_class_id_PhysicalMemory
#define __nvoc_class_id_PhysicalMemory 0x5fccf2
#endif /* __nvoc_class_id_PhysicalMemory */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_PhysicalMemory;

#define __staticCast_PhysicalMemory(pThis) \
    ((pThis)->__nvoc_pbase_PhysicalMemory)

#ifdef __nvoc_phys_mem_h_disabled
#define __dynamicCast_PhysicalMemory(pThis) ((PhysicalMemory*)NULL)
#else //__nvoc_phys_mem_h_disabled
#define __dynamicCast_PhysicalMemory(pThis) \
    ((PhysicalMemory*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(PhysicalMemory)))
#endif //__nvoc_phys_mem_h_disabled

NV_STATUS __nvoc_objCreateDynamic_PhysicalMemory(PhysicalMemory**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_PhysicalMemory(PhysicalMemory**, Dynamic*, NvU32, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_PhysicalMemory(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_PhysicalMemory((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define physmemCanCopy_FNPTR(pPhysicalMemory) pPhysicalMemory->__physmemCanCopy__
#define physmemCanCopy(pPhysicalMemory) physmemCanCopy_DISPATCH(pPhysicalMemory)
#define physmemIsDuplicate_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memIsDuplicate__
#define physmemIsDuplicate(pMemory, hMemory, pDuplicate) physmemIsDuplicate_DISPATCH(pMemory, hMemory, pDuplicate)
#define physmemGetMapAddrSpace_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memGetMapAddrSpace__
#define physmemGetMapAddrSpace(pMemory, pCallContext, mapFlags, pAddrSpace) physmemGetMapAddrSpace_DISPATCH(pMemory, pCallContext, mapFlags, pAddrSpace)
#define physmemControl_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memControl__
#define physmemControl(pMemory, pCallContext, pParams) physmemControl_DISPATCH(pMemory, pCallContext, pParams)
#define physmemMap_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memMap__
#define physmemMap(pMemory, pCallContext, pParams, pCpuMapping) physmemMap_DISPATCH(pMemory, pCallContext, pParams, pCpuMapping)
#define physmemUnmap_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memUnmap__
#define physmemUnmap(pMemory, pCallContext, pCpuMapping) physmemUnmap_DISPATCH(pMemory, pCallContext, pCpuMapping)
#define physmemGetMemInterMapParams_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memGetMemInterMapParams__
#define physmemGetMemInterMapParams(pMemory, pParams) physmemGetMemInterMapParams_DISPATCH(pMemory, pParams)
#define physmemCheckMemInterUnmap_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memCheckMemInterUnmap__
#define physmemCheckMemInterUnmap(pMemory, bSubdeviceHandleProvided) physmemCheckMemInterUnmap_DISPATCH(pMemory, bSubdeviceHandleProvided)
#define physmemGetMemoryMappingDescriptor_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memGetMemoryMappingDescriptor__
#define physmemGetMemoryMappingDescriptor(pMemory, ppMemDesc) physmemGetMemoryMappingDescriptor_DISPATCH(pMemory, ppMemDesc)
#define physmemCheckCopyPermissions_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memCheckCopyPermissions__
#define physmemCheckCopyPermissions(pMemory, pDstGpu, pDstDevice) physmemCheckCopyPermissions_DISPATCH(pMemory, pDstGpu, pDstDevice)
#define physmemIsReady_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memIsReady__
#define physmemIsReady(pMemory, bCopyConstructorContext) physmemIsReady_DISPATCH(pMemory, bCopyConstructorContext)
#define physmemIsGpuMapAllowed_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memIsGpuMapAllowed__
#define physmemIsGpuMapAllowed(pMemory, pGpu) physmemIsGpuMapAllowed_DISPATCH(pMemory, pGpu)
#define physmemIsExportAllowed_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memIsExportAllowed__
#define physmemIsExportAllowed(pMemory) physmemIsExportAllowed_DISPATCH(pMemory)
#define physmemAccessCallback_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__rmresAccessCallback__
#define physmemAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) physmemAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define physmemShareCallback_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__rmresShareCallback__
#define physmemShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) physmemShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define physmemControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__rmresControlSerialization_Prologue__
#define physmemControlSerialization_Prologue(pResource, pCallContext, pParams) physmemControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define physmemControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__rmresControlSerialization_Epilogue__
#define physmemControlSerialization_Epilogue(pResource, pCallContext, pParams) physmemControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define physmemControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__rmresControl_Prologue__
#define physmemControl_Prologue(pResource, pCallContext, pParams) physmemControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define physmemControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__rmresControl_Epilogue__
#define physmemControl_Epilogue(pResource, pCallContext, pParams) physmemControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define physmemPreDestruct_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resPreDestruct__
#define physmemPreDestruct(pResource) physmemPreDestruct_DISPATCH(pResource)
#define physmemControlFilter_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resControlFilter__
#define physmemControlFilter(pResource, pCallContext, pParams) physmemControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define physmemIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsPartialUnmapSupported__
#define physmemIsPartialUnmapSupported(pResource) physmemIsPartialUnmapSupported_DISPATCH(pResource)
#define physmemMapTo_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resMapTo__
#define physmemMapTo(pResource, pParams) physmemMapTo_DISPATCH(pResource, pParams)
#define physmemUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmapFrom__
#define physmemUnmapFrom(pResource, pParams) physmemUnmapFrom_DISPATCH(pResource, pParams)
#define physmemGetRefCount_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resGetRefCount__
#define physmemGetRefCount(pResource) physmemGetRefCount_DISPATCH(pResource)
#define physmemAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resAddAdditionalDependants__
#define physmemAddAdditionalDependants(pClient, pResource, pReference) physmemAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NvBool physmemCanCopy_DISPATCH(struct PhysicalMemory *pPhysicalMemory) {
    return pPhysicalMemory->__physmemCanCopy__(pPhysicalMemory);
}

static inline NV_STATUS physmemIsDuplicate_DISPATCH(struct PhysicalMemory *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return pMemory->__physmemIsDuplicate__(pMemory, hMemory, pDuplicate);
}

static inline NV_STATUS physmemGetMapAddrSpace_DISPATCH(struct PhysicalMemory *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pMemory->__physmemGetMapAddrSpace__(pMemory, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS physmemControl_DISPATCH(struct PhysicalMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pMemory->__physmemControl__(pMemory, pCallContext, pParams);
}

static inline NV_STATUS physmemMap_DISPATCH(struct PhysicalMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pMemory->__physmemMap__(pMemory, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS physmemUnmap_DISPATCH(struct PhysicalMemory *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pMemory->__physmemUnmap__(pMemory, pCallContext, pCpuMapping);
}

static inline NV_STATUS physmemGetMemInterMapParams_DISPATCH(struct PhysicalMemory *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pMemory->__physmemGetMemInterMapParams__(pMemory, pParams);
}

static inline NV_STATUS physmemCheckMemInterUnmap_DISPATCH(struct PhysicalMemory *pMemory, NvBool bSubdeviceHandleProvided) {
    return pMemory->__physmemCheckMemInterUnmap__(pMemory, bSubdeviceHandleProvided);
}

static inline NV_STATUS physmemGetMemoryMappingDescriptor_DISPATCH(struct PhysicalMemory *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return pMemory->__physmemGetMemoryMappingDescriptor__(pMemory, ppMemDesc);
}

static inline NV_STATUS physmemCheckCopyPermissions_DISPATCH(struct PhysicalMemory *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return pMemory->__physmemCheckCopyPermissions__(pMemory, pDstGpu, pDstDevice);
}

static inline NV_STATUS physmemIsReady_DISPATCH(struct PhysicalMemory *pMemory, NvBool bCopyConstructorContext) {
    return pMemory->__physmemIsReady__(pMemory, bCopyConstructorContext);
}

static inline NvBool physmemIsGpuMapAllowed_DISPATCH(struct PhysicalMemory *pMemory, struct OBJGPU *pGpu) {
    return pMemory->__physmemIsGpuMapAllowed__(pMemory, pGpu);
}

static inline NvBool physmemIsExportAllowed_DISPATCH(struct PhysicalMemory *pMemory) {
    return pMemory->__physmemIsExportAllowed__(pMemory);
}

static inline NvBool physmemAccessCallback_DISPATCH(struct PhysicalMemory *pResource, RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__physmemAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool physmemShareCallback_DISPATCH(struct PhysicalMemory *pResource, RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__physmemShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS physmemControlSerialization_Prologue_DISPATCH(struct PhysicalMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__physmemControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void physmemControlSerialization_Epilogue_DISPATCH(struct PhysicalMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__physmemControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS physmemControl_Prologue_DISPATCH(struct PhysicalMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__physmemControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void physmemControl_Epilogue_DISPATCH(struct PhysicalMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__physmemControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline void physmemPreDestruct_DISPATCH(struct PhysicalMemory *pResource) {
    pResource->__physmemPreDestruct__(pResource);
}

static inline NV_STATUS physmemControlFilter_DISPATCH(struct PhysicalMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__physmemControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool physmemIsPartialUnmapSupported_DISPATCH(struct PhysicalMemory *pResource) {
    return pResource->__physmemIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS physmemMapTo_DISPATCH(struct PhysicalMemory *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__physmemMapTo__(pResource, pParams);
}

static inline NV_STATUS physmemUnmapFrom_DISPATCH(struct PhysicalMemory *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__physmemUnmapFrom__(pResource, pParams);
}

static inline NvU32 physmemGetRefCount_DISPATCH(struct PhysicalMemory *pResource) {
    return pResource->__physmemGetRefCount__(pResource);
}

static inline void physmemAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct PhysicalMemory *pResource, RsResourceRef *pReference) {
    pResource->__physmemAddAdditionalDependants__(pClient, pResource, pReference);
}

NvBool physmemCanCopy_IMPL(struct PhysicalMemory *pPhysicalMemory);

NV_STATUS physmemConstruct_IMPL(struct PhysicalMemory *arg_pPhysicalMemory, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_physmemConstruct(arg_pPhysicalMemory, arg_pCallContext, arg_pParams) physmemConstruct_IMPL(arg_pPhysicalMemory, arg_pCallContext, arg_pParams)
#undef PRIVATE_FIELD


#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_PHYS_MEM_NVOC_H_
