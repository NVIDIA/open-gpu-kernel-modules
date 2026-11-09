
#ifndef _G_VIRTUAL_MEM_NVOC_H_
#define _G_VIRTUAL_MEM_NVOC_H_

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
#include "g_virtual_mem_nvoc.h"

#ifndef _VIRTUAL_MEMORY_H_
#define _VIRTUAL_MEMORY_H_

#include "mem_mgr/standard_mem.h"
#include "gpu/mem_mgr/heap_base.h"

/*!
 * Allocator for NV50_MEMORY_VIRTUAL class.
 *
 * Describes a range of typed virtual memory.  Used as
 * a target space for RmMapMemoryDma.
 *
 * The NV01_MEMORY_VIRTUAL subclass class is typeless.
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_VIRTUAL_MEM_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__VirtualMemory;
struct NVOC_METADATA__StandardMemory;


struct VirtualMemory {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__VirtualMemory *__nvoc_metadata_ptr;
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
    struct VirtualMemory *__nvoc_pbase_VirtualMemory;    // virtmem

    // Data members
    NvHandle hVASpace;
    NvBool bAllowUnicastMapping;
    NvBool bReserveVaOnAlloc;
    NvBool bRpcAlloc;
    NODE devMemoryTableNode;
    NODE *pDmaMappingList;
    NvBool bOptimizePageTableMempoolUsage;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__VirtualMemory {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__StandardMemory metadata__StandardMemory;
};

#ifndef __nvoc_class_id_VirtualMemory
#define __nvoc_class_id_VirtualMemory 0x2aea5cu
typedef struct VirtualMemory VirtualMemory;
#endif /* __nvoc_class_id_VirtualMemory */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_VirtualMemory;

#define __staticCast_VirtualMemory(pThis) \
    ((pThis)->__nvoc_pbase_VirtualMemory)

#ifdef __nvoc_virtual_mem_h_disabled
#define __dynamicCast_VirtualMemory(pThis) ((VirtualMemory*) NULL)
#else //__nvoc_virtual_mem_h_disabled
#define __dynamicCast_VirtualMemory(pThis) \
    ((VirtualMemory*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(VirtualMemory)))
#endif //__nvoc_virtual_mem_h_disabled

NV_STATUS __nvoc_objCreateDynamic_VirtualMemory(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_VirtualMemory(VirtualMemory**, Dynamic*, NvU32, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __objCreate_VirtualMemory(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams) \
    __nvoc_objCreate_VirtualMemory((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pCallContext, pParams)


// Wrapper macros for implementation functions
NV_STATUS virtmemConstruct_IMPL(struct VirtualMemory *pVirtualMemory, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __nvoc_virtmemConstruct(pVirtualMemory, pCallContext, pParams) virtmemConstruct_IMPL(pVirtualMemory, pCallContext, pParams)

void virtmemDestruct_IMPL(struct VirtualMemory *pVirtualMemory);
#define __nvoc_virtmemDestruct(pVirtualMemory) virtmemDestruct_IMPL(pVirtualMemory)

NV_STATUS virtmemReserveMempool_IMPL(struct VirtualMemory *pVirtualMemory, struct OBJGPU *arg2, struct Device *pDevice, NvU64 size, NvU64 pageSizeMask);
#ifdef __nvoc_virtual_mem_h_disabled
static inline NV_STATUS virtmemReserveMempool(struct VirtualMemory *pVirtualMemory, struct OBJGPU *arg2, struct Device *pDevice, NvU64 size, NvU64 pageSizeMask) {
    NV_ASSERT_FAILED_PRECOMP("VirtualMemory was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_virtual_mem_h_disabled
#define virtmemReserveMempool(pVirtualMemory, arg2, pDevice, size, pageSizeMask) virtmemReserveMempool_IMPL(pVirtualMemory, arg2, pDevice, size, pageSizeMask)
#endif // __nvoc_virtual_mem_h_disabled

NvBool virtmemMatchesVASpace_IMPL(struct VirtualMemory *pVirtualMemory, NvHandle hClient, NvHandle hVASpace);
#ifdef __nvoc_virtual_mem_h_disabled
static inline NvBool virtmemMatchesVASpace(struct VirtualMemory *pVirtualMemory, NvHandle hClient, NvHandle hVASpace) {
    NV_ASSERT_FAILED_PRECOMP("VirtualMemory was disabled!");
    return NV_FALSE;
}
#else // __nvoc_virtual_mem_h_disabled
#define virtmemMatchesVASpace(pVirtualMemory, hClient, hVASpace) virtmemMatchesVASpace_IMPL(pVirtualMemory, hClient, hVASpace)
#endif // __nvoc_virtual_mem_h_disabled

NV_STATUS virtmemGetByHandleAndDevice_IMPL(struct RsClient *pClient, NvHandle hMemory, NvHandle hDevice, struct VirtualMemory **ppVirtualMemory);
#define virtmemGetByHandleAndDevice(pClient, hMemory, hDevice, ppVirtualMemory) virtmemGetByHandleAndDevice_IMPL(pClient, hMemory, hDevice, ppVirtualMemory)

void virtmemGetAddressAndSize_IMPL(struct VirtualMemory *arg1, NvU64 *pVAddr, NvU64 *pSize);
#define virtmemGetAddressAndSize(arg1, pVAddr, pSize) virtmemGetAddressAndSize_IMPL(arg1, pVAddr, pSize)


// Wrapper macros for halified functions
#define virtmemMapTo_FNPTR(pVirtualMemory) pVirtualMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define virtmemMapTo(pVirtualMemory, pParams) virtmemMapTo_DISPATCH(pVirtualMemory, pParams)
#define virtmemUnmapFrom_FNPTR(pVirtualMemory) pVirtualMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define virtmemUnmapFrom(pVirtualMemory, pParams) virtmemUnmapFrom_DISPATCH(pVirtualMemory, pParams)
#define virtmemIsPartialUnmapSupported_FNPTR(pVirtualMemory) pVirtualMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define virtmemIsPartialUnmapSupported(pVirtualMemory) virtmemIsPartialUnmapSupported_DISPATCH(pVirtualMemory)
#define virtmemCanCopy_FNPTR(pStandardMemory) pStandardMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define virtmemCanCopy(pStandardMemory) virtmemCanCopy_DISPATCH(pStandardMemory)
#define virtmemIsDuplicate_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define virtmemIsDuplicate(pMemory, hMemory, pDuplicate) virtmemIsDuplicate_DISPATCH(pMemory, hMemory, pDuplicate)
#define virtmemGetMapAddrSpace_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memGetMapAddrSpace__
#define virtmemGetMapAddrSpace(pMemory, pCallContext, mapFlags, pAddrSpace) virtmemGetMapAddrSpace_DISPATCH(pMemory, pCallContext, mapFlags, pAddrSpace)
#define virtmemControl_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__
#define virtmemControl(pMemory, pCallContext, pParams) virtmemControl_DISPATCH(pMemory, pCallContext, pParams)
#define virtmemMap_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__
#define virtmemMap(pMemory, pCallContext, pParams, pCpuMapping) virtmemMap_DISPATCH(pMemory, pCallContext, pParams, pCpuMapping)
#define virtmemUnmap_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__
#define virtmemUnmap(pMemory, pCallContext, pCpuMapping) virtmemUnmap_DISPATCH(pMemory, pCallContext, pCpuMapping)
#define virtmemGetMemInterMapParams_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define virtmemGetMemInterMapParams(pMemory, pParams) virtmemGetMemInterMapParams_DISPATCH(pMemory, pParams)
#define virtmemCheckMemInterUnmap_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define virtmemCheckMemInterUnmap(pMemory, bSubdeviceHandleProvided) virtmemCheckMemInterUnmap_DISPATCH(pMemory, bSubdeviceHandleProvided)
#define virtmemGetMemoryMappingDescriptor_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define virtmemGetMemoryMappingDescriptor(pMemory, ppMemDesc) virtmemGetMemoryMappingDescriptor_DISPATCH(pMemory, ppMemDesc)
#define virtmemCheckCopyPermissions_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memCheckCopyPermissions__
#define virtmemCheckCopyPermissions(pMemory, pDstGpu, pDstDevice) virtmemCheckCopyPermissions_DISPATCH(pMemory, pDstGpu, pDstDevice)
#define virtmemIsReady_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsReady__
#define virtmemIsReady(pMemory, bCopyConstructorContext) virtmemIsReady_DISPATCH(pMemory, bCopyConstructorContext)
#define virtmemIsGpuMapAllowed_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsGpuMapAllowed__
#define virtmemIsGpuMapAllowed(pMemory, pGpu) virtmemIsGpuMapAllowed_DISPATCH(pMemory, pGpu)
#define virtmemIsExportAllowed_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsExportAllowed__
#define virtmemIsExportAllowed(pMemory) virtmemIsExportAllowed_DISPATCH(pMemory)
#define virtmemAccessCallback_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__
#define virtmemAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) virtmemAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define virtmemShareCallback_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__
#define virtmemShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) virtmemShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define virtmemControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__
#define virtmemControlSerialization_Prologue(pResource, pCallContext, pParams) virtmemControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define virtmemControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__
#define virtmemControlSerialization_Epilogue(pResource, pCallContext, pParams) virtmemControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define virtmemControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__
#define virtmemControl_Prologue(pResource, pCallContext, pParams) virtmemControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define virtmemControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__
#define virtmemControl_Epilogue(pResource, pCallContext, pParams) virtmemControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define virtmemPreDestruct_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define virtmemPreDestruct(pResource) virtmemPreDestruct_DISPATCH(pResource)
#define virtmemControlFilter_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define virtmemControlFilter(pResource, pCallContext, pParams) virtmemControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define virtmemGetRefCount_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define virtmemGetRefCount(pResource) virtmemGetRefCount_DISPATCH(pResource)
#define virtmemAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define virtmemAddAdditionalDependants(pClient, pResource, pReference) virtmemAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NV_STATUS virtmemMapTo_DISPATCH(struct VirtualMemory *pVirtualMemory, struct RS_RES_MAP_TO_PARAMS *pParams) {
    return pVirtualMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__(staticCast(pVirtualMemory, RsResource), pParams);
}

static inline NV_STATUS virtmemUnmapFrom_DISPATCH(struct VirtualMemory *pVirtualMemory, struct RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pVirtualMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__(staticCast(pVirtualMemory, RsResource), pParams);
}

static inline NvBool virtmemIsPartialUnmapSupported_DISPATCH(struct VirtualMemory *pVirtualMemory) {
    return pVirtualMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__(staticCast(pVirtualMemory, RsResource));
}

static inline NvBool virtmemCanCopy_DISPATCH(struct VirtualMemory *pStandardMemory) {
    return pStandardMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__(staticCast(pStandardMemory, RsResource));
}

static inline NV_STATUS virtmemIsDuplicate_DISPATCH(struct VirtualMemory *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__(staticCast(pMemory, RsResource), hMemory, pDuplicate);
}

static inline NV_STATUS virtmemGetMapAddrSpace_DISPATCH(struct VirtualMemory *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memGetMapAddrSpace__(staticCast(pMemory, Memory), pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS virtmemControl_DISPATCH(struct VirtualMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__(staticCast(pMemory, RsResource), pCallContext, pParams);
}

static inline NV_STATUS virtmemMap_DISPATCH(struct VirtualMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__(staticCast(pMemory, RsResource), pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS virtmemUnmap_DISPATCH(struct VirtualMemory *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__(staticCast(pMemory, RsResource), pCallContext, pCpuMapping);
}

static inline NV_STATUS virtmemGetMemInterMapParams_DISPATCH(struct VirtualMemory *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__(staticCast(pMemory, RmResource), pParams);
}

static inline NV_STATUS virtmemCheckMemInterUnmap_DISPATCH(struct VirtualMemory *pMemory, NvBool bSubdeviceHandleProvided) {
    return pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__(staticCast(pMemory, RmResource), bSubdeviceHandleProvided);
}

static inline NV_STATUS virtmemGetMemoryMappingDescriptor_DISPATCH(struct VirtualMemory *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__(staticCast(pMemory, RmResource), ppMemDesc);
}

static inline NV_STATUS virtmemCheckCopyPermissions_DISPATCH(struct VirtualMemory *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memCheckCopyPermissions__(staticCast(pMemory, Memory), pDstGpu, pDstDevice);
}

static inline NV_STATUS virtmemIsReady_DISPATCH(struct VirtualMemory *pMemory, NvBool bCopyConstructorContext) {
    return pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsReady__(staticCast(pMemory, Memory), bCopyConstructorContext);
}

static inline NvBool virtmemIsGpuMapAllowed_DISPATCH(struct VirtualMemory *pMemory, struct OBJGPU *pGpu) {
    return pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsGpuMapAllowed__(staticCast(pMemory, Memory), pGpu);
}

static inline NvBool virtmemIsExportAllowed_DISPATCH(struct VirtualMemory *pMemory) {
    return pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsExportAllowed__(staticCast(pMemory, Memory));
}

static inline NvBool virtmemAccessCallback_DISPATCH(struct VirtualMemory *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__(staticCast(pResource, RsResource), pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool virtmemShareCallback_DISPATCH(struct VirtualMemory *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__(staticCast(pResource, RsResource), pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS virtmemControlSerialization_Prologue_DISPATCH(struct VirtualMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void virtmemControlSerialization_Epilogue_DISPATCH(struct VirtualMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS virtmemControl_Prologue_DISPATCH(struct VirtualMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void virtmemControl_Epilogue_DISPATCH(struct VirtualMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void virtmemPreDestruct_DISPATCH(struct VirtualMemory *pResource) {
    pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__(staticCast(pResource, RsResource));
}

static inline NV_STATUS virtmemControlFilter_DISPATCH(struct VirtualMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvU32 virtmemGetRefCount_DISPATCH(struct VirtualMemory *pResource) {
    return pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__(staticCast(pResource, RsResource));
}

static inline void virtmemAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct VirtualMemory *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__(pClient, staticCast(pResource, RsResource), pReference);
}

// Virtual method declarations and/or inline definitions
NV_STATUS virtmemMapTo_IMPL(struct VirtualMemory *pVirtualMemory, struct RS_RES_MAP_TO_PARAMS *pParams);

NV_STATUS virtmemUnmapFrom_IMPL(struct VirtualMemory *pVirtualMemory, struct RS_RES_UNMAP_FROM_PARAMS *pParams);

static inline NvBool virtmemIsPartialUnmapSupported_e661f0(struct VirtualMemory *pVirtualMemory){
    return NV_TRUE;
}

// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


NV_STATUS virtmemAllocResources(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, MEMORY_ALLOCATION_REQUEST *pAllocRequest,
                                FB_ALLOC_INFO *pFbAllocInfo, struct RmClient *pFbAllocInfoClient);

#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_VIRTUAL_MEM_NVOC_H_
