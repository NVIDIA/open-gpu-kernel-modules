
#ifndef _G_PHYS_MEM_NVOC_H_
#define _G_PHYS_MEM_NVOC_H_

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


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__PhysicalMemory;
struct NVOC_METADATA__Memory;


struct PhysicalMemory {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__PhysicalMemory *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct Memory __nvoc_base_Memory;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^4
    struct RsResource *__nvoc_pbase_RsResource;    // res super^3
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^3
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super^2
    struct Memory *__nvoc_pbase_Memory;    // mem super
    struct PhysicalMemory *__nvoc_pbase_PhysicalMemory;    // physmem
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__PhysicalMemory {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__Memory metadata__Memory;
};

#ifndef __nvoc_class_id_PhysicalMemory
#define __nvoc_class_id_PhysicalMemory 0x5fccf2u
typedef struct PhysicalMemory PhysicalMemory;
#endif /* __nvoc_class_id_PhysicalMemory */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_PhysicalMemory;

#define __staticCast_PhysicalMemory(pThis) \
    ((pThis)->__nvoc_pbase_PhysicalMemory)

#ifdef __nvoc_phys_mem_h_disabled
#define __dynamicCast_PhysicalMemory(pThis) ((PhysicalMemory*) NULL)
#else //__nvoc_phys_mem_h_disabled
#define __dynamicCast_PhysicalMemory(pThis) \
    ((PhysicalMemory*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(PhysicalMemory)))
#endif //__nvoc_phys_mem_h_disabled

NV_STATUS __nvoc_objCreateDynamic_PhysicalMemory(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_PhysicalMemory(PhysicalMemory**, Dynamic*, NvU32, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __objCreate_PhysicalMemory(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams) \
    __nvoc_objCreate_PhysicalMemory((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pCallContext, pParams)


// Wrapper macros for implementation functions
NV_STATUS physmemConstruct_IMPL(struct PhysicalMemory *pPhysicalMemory, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __nvoc_physmemConstruct(pPhysicalMemory, pCallContext, pParams) physmemConstruct_IMPL(pPhysicalMemory, pCallContext, pParams)


// Wrapper macros for halified functions
#define physmemCanCopy_FNPTR(pPhysicalMemory) pPhysicalMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define physmemCanCopy(pPhysicalMemory) physmemCanCopy_DISPATCH(pPhysicalMemory)
#define physmemIsDuplicate_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define physmemIsDuplicate(pMemory, hMemory, pDuplicate) physmemIsDuplicate_DISPATCH(pMemory, hMemory, pDuplicate)
#define physmemGetMapAddrSpace_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memGetMapAddrSpace__
#define physmemGetMapAddrSpace(pMemory, pCallContext, mapFlags, pAddrSpace) physmemGetMapAddrSpace_DISPATCH(pMemory, pCallContext, mapFlags, pAddrSpace)
#define physmemControl_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__
#define physmemControl(pMemory, pCallContext, pParams) physmemControl_DISPATCH(pMemory, pCallContext, pParams)
#define physmemMap_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__
#define physmemMap(pMemory, pCallContext, pParams, pCpuMapping) physmemMap_DISPATCH(pMemory, pCallContext, pParams, pCpuMapping)
#define physmemUnmap_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__
#define physmemUnmap(pMemory, pCallContext, pCpuMapping) physmemUnmap_DISPATCH(pMemory, pCallContext, pCpuMapping)
#define physmemGetMemInterMapParams_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define physmemGetMemInterMapParams(pMemory, pParams) physmemGetMemInterMapParams_DISPATCH(pMemory, pParams)
#define physmemCheckMemInterUnmap_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define physmemCheckMemInterUnmap(pMemory, bSubdeviceHandleProvided) physmemCheckMemInterUnmap_DISPATCH(pMemory, bSubdeviceHandleProvided)
#define physmemGetMemoryMappingDescriptor_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define physmemGetMemoryMappingDescriptor(pMemory, ppMemDesc) physmemGetMemoryMappingDescriptor_DISPATCH(pMemory, ppMemDesc)
#define physmemCheckCopyPermissions_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memCheckCopyPermissions__
#define physmemCheckCopyPermissions(pMemory, pDstGpu, pDstDevice) physmemCheckCopyPermissions_DISPATCH(pMemory, pDstGpu, pDstDevice)
#define physmemIsReady_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsReady__
#define physmemIsReady(pMemory, bCopyConstructorContext) physmemIsReady_DISPATCH(pMemory, bCopyConstructorContext)
#define physmemIsGpuMapAllowed_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsGpuMapAllowed__
#define physmemIsGpuMapAllowed(pMemory, pGpu) physmemIsGpuMapAllowed_DISPATCH(pMemory, pGpu)
#define physmemIsExportAllowed_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsExportAllowed__
#define physmemIsExportAllowed(pMemory) physmemIsExportAllowed_DISPATCH(pMemory)
#define physmemAccessCallback_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__
#define physmemAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) physmemAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define physmemShareCallback_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__
#define physmemShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) physmemShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define physmemControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__
#define physmemControlSerialization_Prologue(pResource, pCallContext, pParams) physmemControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define physmemControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__
#define physmemControlSerialization_Epilogue(pResource, pCallContext, pParams) physmemControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define physmemControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__
#define physmemControl_Prologue(pResource, pCallContext, pParams) physmemControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define physmemControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__
#define physmemControl_Epilogue(pResource, pCallContext, pParams) physmemControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define physmemPreDestruct_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define physmemPreDestruct(pResource) physmemPreDestruct_DISPATCH(pResource)
#define physmemControlFilter_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define physmemControlFilter(pResource, pCallContext, pParams) physmemControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define physmemIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define physmemIsPartialUnmapSupported(pResource) physmemIsPartialUnmapSupported_DISPATCH(pResource)
#define physmemMapTo_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define physmemMapTo(pResource, pParams) physmemMapTo_DISPATCH(pResource, pParams)
#define physmemUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define physmemUnmapFrom(pResource, pParams) physmemUnmapFrom_DISPATCH(pResource, pParams)
#define physmemGetRefCount_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define physmemGetRefCount(pResource) physmemGetRefCount_DISPATCH(pResource)
#define physmemAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define physmemAddAdditionalDependants(pClient, pResource, pReference) physmemAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NvBool physmemCanCopy_DISPATCH(struct PhysicalMemory *pPhysicalMemory) {
    return pPhysicalMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__(staticCast(pPhysicalMemory, RsResource));
}

static inline NV_STATUS physmemIsDuplicate_DISPATCH(struct PhysicalMemory *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__(staticCast(pMemory, RsResource), hMemory, pDuplicate);
}

static inline NV_STATUS physmemGetMapAddrSpace_DISPATCH(struct PhysicalMemory *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memGetMapAddrSpace__(staticCast(pMemory, Memory), pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS physmemControl_DISPATCH(struct PhysicalMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__(staticCast(pMemory, RsResource), pCallContext, pParams);
}

static inline NV_STATUS physmemMap_DISPATCH(struct PhysicalMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__(staticCast(pMemory, RsResource), pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS physmemUnmap_DISPATCH(struct PhysicalMemory *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__(staticCast(pMemory, RsResource), pCallContext, pCpuMapping);
}

static inline NV_STATUS physmemGetMemInterMapParams_DISPATCH(struct PhysicalMemory *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__(staticCast(pMemory, RmResource), pParams);
}

static inline NV_STATUS physmemCheckMemInterUnmap_DISPATCH(struct PhysicalMemory *pMemory, NvBool bSubdeviceHandleProvided) {
    return pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__(staticCast(pMemory, RmResource), bSubdeviceHandleProvided);
}

static inline NV_STATUS physmemGetMemoryMappingDescriptor_DISPATCH(struct PhysicalMemory *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__(staticCast(pMemory, RmResource), ppMemDesc);
}

static inline NV_STATUS physmemCheckCopyPermissions_DISPATCH(struct PhysicalMemory *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memCheckCopyPermissions__(staticCast(pMemory, Memory), pDstGpu, pDstDevice);
}

static inline NV_STATUS physmemIsReady_DISPATCH(struct PhysicalMemory *pMemory, NvBool bCopyConstructorContext) {
    return pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsReady__(staticCast(pMemory, Memory), bCopyConstructorContext);
}

static inline NvBool physmemIsGpuMapAllowed_DISPATCH(struct PhysicalMemory *pMemory, struct OBJGPU *pGpu) {
    return pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsGpuMapAllowed__(staticCast(pMemory, Memory), pGpu);
}

static inline NvBool physmemIsExportAllowed_DISPATCH(struct PhysicalMemory *pMemory) {
    return pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsExportAllowed__(staticCast(pMemory, Memory));
}

static inline NvBool physmemAccessCallback_DISPATCH(struct PhysicalMemory *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__(staticCast(pResource, RsResource), pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool physmemShareCallback_DISPATCH(struct PhysicalMemory *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__(staticCast(pResource, RsResource), pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS physmemControlSerialization_Prologue_DISPATCH(struct PhysicalMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void physmemControlSerialization_Epilogue_DISPATCH(struct PhysicalMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS physmemControl_Prologue_DISPATCH(struct PhysicalMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void physmemControl_Epilogue_DISPATCH(struct PhysicalMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void physmemPreDestruct_DISPATCH(struct PhysicalMemory *pResource) {
    pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__(staticCast(pResource, RsResource));
}

static inline NV_STATUS physmemControlFilter_DISPATCH(struct PhysicalMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool physmemIsPartialUnmapSupported_DISPATCH(struct PhysicalMemory *pResource) {
    return pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__(staticCast(pResource, RsResource));
}

static inline NV_STATUS physmemMapTo_DISPATCH(struct PhysicalMemory *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__(staticCast(pResource, RsResource), pParams);
}

static inline NV_STATUS physmemUnmapFrom_DISPATCH(struct PhysicalMemory *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__(staticCast(pResource, RsResource), pParams);
}

static inline NvU32 physmemGetRefCount_DISPATCH(struct PhysicalMemory *pResource) {
    return pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__(staticCast(pResource, RsResource));
}

static inline void physmemAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct PhysicalMemory *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__(pClient, staticCast(pResource, RsResource), pReference);
}

// Virtual method declarations and/or inline definitions
NvBool physmemCanCopy_IMPL(struct PhysicalMemory *pPhysicalMemory);

// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_PHYS_MEM_NVOC_H_
