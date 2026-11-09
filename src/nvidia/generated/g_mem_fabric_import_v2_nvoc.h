
#ifndef _G_MEM_FABRIC_IMPORT_V2_NVOC_H_
#define _G_MEM_FABRIC_IMPORT_V2_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2021-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/******************************************************************************
 *
 *   Description:
 *       This file contains the functions for import fabric memory
 *
 *****************************************************************************/

#pragma once
#include "g_mem_fabric_import_v2_nvoc.h"

#ifndef _MEMORYFABRICIMPORTV2_H_
#define _MEMORYFABRICIMPORTV2_H_

#include "core/core.h"
#include "rmapi/resource.h"
#include "mem_mgr/mem.h"
#include "containers/list.h"
#include "nvCpuUuid.h"

#include "ctrl/ctrl00f9.h"

// ****************************************************************************
//                          Type Definitions
// ****************************************************************************

typedef struct mem_fabric_import_descriptor MEM_FABRIC_IMPORT_DESCRIPTOR;


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_MEM_FABRIC_IMPORT_V2_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__MemoryFabricImportV2;
struct NVOC_METADATA__Memory;
struct NVOC_VTABLE__MemoryFabricImportV2;


struct MemoryFabricImportV2 {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__MemoryFabricImportV2 *__nvoc_metadata_ptr;
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
    struct MemoryFabricImportV2 *__nvoc_pbase_MemoryFabricImportV2;    // memoryfabricimportv2

    // Data members
    NvU16 expNodeId;
    MEM_FABRIC_IMPORT_DESCRIPTOR *PRIVATE_FIELD(pFabricImportDesc);
};


// Vtable with 1 per-class function pointer
struct NVOC_VTABLE__MemoryFabricImportV2 {
    NV_STATUS (*__memoryfabricimportv2CopyConstruct__)(struct MemoryFabricImportV2 * /*this*/, CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);  // virtual override (mem) base (mem)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__MemoryFabricImportV2 {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__Memory metadata__Memory;
    const struct NVOC_VTABLE__MemoryFabricImportV2 vtable;
};

#ifndef __nvoc_class_id_MemoryFabricImportV2
#define __nvoc_class_id_MemoryFabricImportV2 0xf96871u
typedef struct MemoryFabricImportV2 MemoryFabricImportV2;
#endif /* __nvoc_class_id_MemoryFabricImportV2 */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_MemoryFabricImportV2;

#define __staticCast_MemoryFabricImportV2(pThis) \
    ((pThis)->__nvoc_pbase_MemoryFabricImportV2)

#ifdef __nvoc_mem_fabric_import_v2_h_disabled
#define __dynamicCast_MemoryFabricImportV2(pThis) ((MemoryFabricImportV2*) NULL)
#else //__nvoc_mem_fabric_import_v2_h_disabled
#define __dynamicCast_MemoryFabricImportV2(pThis) \
    ((MemoryFabricImportV2*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(MemoryFabricImportV2)))
#endif //__nvoc_mem_fabric_import_v2_h_disabled

NV_STATUS __nvoc_objCreateDynamic_MemoryFabricImportV2(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_MemoryFabricImportV2(MemoryFabricImportV2**, Dynamic*, NvU32, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __objCreate_MemoryFabricImportV2(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams) \
    __nvoc_objCreate_MemoryFabricImportV2((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pCallContext, pParams)


// Wrapper macros for implementation functions
NV_STATUS memoryfabricimportv2Construct_IMPL(struct MemoryFabricImportV2 *pMemoryFabricImportV2, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __nvoc_memoryfabricimportv2Construct(pMemoryFabricImportV2, pCallContext, pParams) memoryfabricimportv2Construct_IMPL(pMemoryFabricImportV2, pCallContext, pParams)

void memoryfabricimportv2Destruct_IMPL(struct MemoryFabricImportV2 *pMemoryFabricImportV2);
#define __nvoc_memoryfabricimportv2Destruct(pMemoryFabricImportV2) memoryfabricimportv2Destruct_IMPL(pMemoryFabricImportV2)

void memoryfabricimportv2RemoveFromCache_IMPL(struct MemoryFabricImportV2 *pMemoryFabricImportV2);
#ifdef __nvoc_mem_fabric_import_v2_h_disabled
static inline void memoryfabricimportv2RemoveFromCache(struct MemoryFabricImportV2 *pMemoryFabricImportV2) {
    NV_ASSERT_FAILED_PRECOMP("MemoryFabricImportV2 was disabled!");
}
#else // __nvoc_mem_fabric_import_v2_h_disabled
#define memoryfabricimportv2RemoveFromCache(pMemoryFabricImportV2) memoryfabricimportv2RemoveFromCache_IMPL(pMemoryFabricImportV2)
#endif // __nvoc_mem_fabric_import_v2_h_disabled

NV_STATUS memoryfabricimportv2CtrlGetInfo_IMPL(struct MemoryFabricImportV2 *pMemoryFabricImportV2, NV00F9_CTRL_GET_INFO_PARAMS *pParams);
#ifdef __nvoc_mem_fabric_import_v2_h_disabled
static inline NV_STATUS memoryfabricimportv2CtrlGetInfo(struct MemoryFabricImportV2 *pMemoryFabricImportV2, NV00F9_CTRL_GET_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("MemoryFabricImportV2 was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_fabric_import_v2_h_disabled
#define memoryfabricimportv2CtrlGetInfo(pMemoryFabricImportV2, pParams) memoryfabricimportv2CtrlGetInfo_IMPL(pMemoryFabricImportV2, pParams)
#endif // __nvoc_mem_fabric_import_v2_h_disabled


// Wrapper macros for halified functions
#define memoryfabricimportv2CanCopy_FNPTR(pMemoryFabricImportV2) pMemoryFabricImportV2->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define memoryfabricimportv2CanCopy(pMemoryFabricImportV2) memoryfabricimportv2CanCopy_DISPATCH(pMemoryFabricImportV2)
#define memoryfabricimportv2IsReady_FNPTR(pMemoryFabricImportV2) pMemoryFabricImportV2->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsReady__
#define memoryfabricimportv2IsReady(pMemoryFabricImportV2, bCopyConstructorContext) memoryfabricimportv2IsReady_DISPATCH(pMemoryFabricImportV2, bCopyConstructorContext)
#define memoryfabricimportv2CopyConstruct_FNPTR(pMemoryFabricImportV2) pMemoryFabricImportV2->__nvoc_metadata_ptr->vtable.__memoryfabricimportv2CopyConstruct__
#define memoryfabricimportv2CopyConstruct(pMemoryFabricImportV2, pCallContext, pParams) memoryfabricimportv2CopyConstruct_DISPATCH(pMemoryFabricImportV2, pCallContext, pParams)
#define memoryfabricimportv2Control_FNPTR(pMemoryFabricImportV2) pMemoryFabricImportV2->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__
#define memoryfabricimportv2Control(pMemoryFabricImportV2, pCallContext, pParams) memoryfabricimportv2Control_DISPATCH(pMemoryFabricImportV2, pCallContext, pParams)
#define memoryfabricimportv2GetMapAddrSpace_FNPTR(pMemoryFabricImportV2) pMemoryFabricImportV2->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memGetMapAddrSpace__
#define memoryfabricimportv2GetMapAddrSpace(pMemoryFabricImportV2, pCallContext, mapFlags, pAddrSpace) memoryfabricimportv2GetMapAddrSpace_DISPATCH(pMemoryFabricImportV2, pCallContext, mapFlags, pAddrSpace)
#define memoryfabricimportv2IsGpuMapAllowed_FNPTR(pMemoryFabricImportV2) pMemoryFabricImportV2->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsGpuMapAllowed__
#define memoryfabricimportv2IsGpuMapAllowed(pMemoryFabricImportV2, pGpu) memoryfabricimportv2IsGpuMapAllowed_DISPATCH(pMemoryFabricImportV2, pGpu)
#define memoryfabricimportv2IsExportAllowed_FNPTR(pMemoryFabricImportV2) pMemoryFabricImportV2->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsExportAllowed__
#define memoryfabricimportv2IsExportAllowed(pMemoryFabricImportV2) memoryfabricimportv2IsExportAllowed_DISPATCH(pMemoryFabricImportV2)
#define memoryfabricimportv2IsDuplicate_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define memoryfabricimportv2IsDuplicate(pMemory, hMemory, pDuplicate) memoryfabricimportv2IsDuplicate_DISPATCH(pMemory, hMemory, pDuplicate)
#define memoryfabricimportv2Map_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__
#define memoryfabricimportv2Map(pMemory, pCallContext, pParams, pCpuMapping) memoryfabricimportv2Map_DISPATCH(pMemory, pCallContext, pParams, pCpuMapping)
#define memoryfabricimportv2Unmap_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__
#define memoryfabricimportv2Unmap(pMemory, pCallContext, pCpuMapping) memoryfabricimportv2Unmap_DISPATCH(pMemory, pCallContext, pCpuMapping)
#define memoryfabricimportv2GetMemInterMapParams_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define memoryfabricimportv2GetMemInterMapParams(pMemory, pParams) memoryfabricimportv2GetMemInterMapParams_DISPATCH(pMemory, pParams)
#define memoryfabricimportv2CheckMemInterUnmap_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define memoryfabricimportv2CheckMemInterUnmap(pMemory, bSubdeviceHandleProvided) memoryfabricimportv2CheckMemInterUnmap_DISPATCH(pMemory, bSubdeviceHandleProvided)
#define memoryfabricimportv2GetMemoryMappingDescriptor_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define memoryfabricimportv2GetMemoryMappingDescriptor(pMemory, ppMemDesc) memoryfabricimportv2GetMemoryMappingDescriptor_DISPATCH(pMemory, ppMemDesc)
#define memoryfabricimportv2CheckCopyPermissions_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memCheckCopyPermissions__
#define memoryfabricimportv2CheckCopyPermissions(pMemory, pDstGpu, pDstDevice) memoryfabricimportv2CheckCopyPermissions_DISPATCH(pMemory, pDstGpu, pDstDevice)
#define memoryfabricimportv2AccessCallback_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__
#define memoryfabricimportv2AccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) memoryfabricimportv2AccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define memoryfabricimportv2ShareCallback_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__
#define memoryfabricimportv2ShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) memoryfabricimportv2ShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define memoryfabricimportv2ControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__
#define memoryfabricimportv2ControlSerialization_Prologue(pResource, pCallContext, pParams) memoryfabricimportv2ControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define memoryfabricimportv2ControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__
#define memoryfabricimportv2ControlSerialization_Epilogue(pResource, pCallContext, pParams) memoryfabricimportv2ControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define memoryfabricimportv2Control_Prologue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__
#define memoryfabricimportv2Control_Prologue(pResource, pCallContext, pParams) memoryfabricimportv2Control_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define memoryfabricimportv2Control_Epilogue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__
#define memoryfabricimportv2Control_Epilogue(pResource, pCallContext, pParams) memoryfabricimportv2Control_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define memoryfabricimportv2PreDestruct_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define memoryfabricimportv2PreDestruct(pResource) memoryfabricimportv2PreDestruct_DISPATCH(pResource)
#define memoryfabricimportv2ControlFilter_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define memoryfabricimportv2ControlFilter(pResource, pCallContext, pParams) memoryfabricimportv2ControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define memoryfabricimportv2IsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define memoryfabricimportv2IsPartialUnmapSupported(pResource) memoryfabricimportv2IsPartialUnmapSupported_DISPATCH(pResource)
#define memoryfabricimportv2MapTo_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define memoryfabricimportv2MapTo(pResource, pParams) memoryfabricimportv2MapTo_DISPATCH(pResource, pParams)
#define memoryfabricimportv2UnmapFrom_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define memoryfabricimportv2UnmapFrom(pResource, pParams) memoryfabricimportv2UnmapFrom_DISPATCH(pResource, pParams)
#define memoryfabricimportv2GetRefCount_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define memoryfabricimportv2GetRefCount(pResource) memoryfabricimportv2GetRefCount_DISPATCH(pResource)
#define memoryfabricimportv2AddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define memoryfabricimportv2AddAdditionalDependants(pClient, pResource, pReference) memoryfabricimportv2AddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NvBool memoryfabricimportv2CanCopy_DISPATCH(struct MemoryFabricImportV2 *pMemoryFabricImportV2) {
    return pMemoryFabricImportV2->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__(staticCast(pMemoryFabricImportV2, RsResource));
}

static inline NV_STATUS memoryfabricimportv2IsReady_DISPATCH(struct MemoryFabricImportV2 *pMemoryFabricImportV2, NvBool bCopyConstructorContext) {
    return pMemoryFabricImportV2->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsReady__(staticCast(pMemoryFabricImportV2, Memory), bCopyConstructorContext);
}

static inline NV_STATUS memoryfabricimportv2CopyConstruct_DISPATCH(struct MemoryFabricImportV2 *pMemoryFabricImportV2, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {
    return pMemoryFabricImportV2->__nvoc_metadata_ptr->vtable.__memoryfabricimportv2CopyConstruct__(pMemoryFabricImportV2, pCallContext, pParams);
}

static inline NV_STATUS memoryfabricimportv2Control_DISPATCH(struct MemoryFabricImportV2 *pMemoryFabricImportV2, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pMemoryFabricImportV2->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__(staticCast(pMemoryFabricImportV2, RsResource), pCallContext, pParams);
}

static inline NV_STATUS memoryfabricimportv2GetMapAddrSpace_DISPATCH(struct MemoryFabricImportV2 *pMemoryFabricImportV2, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pMemoryFabricImportV2->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memGetMapAddrSpace__(staticCast(pMemoryFabricImportV2, Memory), pCallContext, mapFlags, pAddrSpace);
}

static inline NvBool memoryfabricimportv2IsGpuMapAllowed_DISPATCH(struct MemoryFabricImportV2 *pMemoryFabricImportV2, struct OBJGPU *pGpu) {
    return pMemoryFabricImportV2->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsGpuMapAllowed__(staticCast(pMemoryFabricImportV2, Memory), pGpu);
}

static inline NvBool memoryfabricimportv2IsExportAllowed_DISPATCH(struct MemoryFabricImportV2 *pMemoryFabricImportV2) {
    return pMemoryFabricImportV2->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsExportAllowed__(staticCast(pMemoryFabricImportV2, Memory));
}

static inline NV_STATUS memoryfabricimportv2IsDuplicate_DISPATCH(struct MemoryFabricImportV2 *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__(staticCast(pMemory, RsResource), hMemory, pDuplicate);
}

static inline NV_STATUS memoryfabricimportv2Map_DISPATCH(struct MemoryFabricImportV2 *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__(staticCast(pMemory, RsResource), pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS memoryfabricimportv2Unmap_DISPATCH(struct MemoryFabricImportV2 *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__(staticCast(pMemory, RsResource), pCallContext, pCpuMapping);
}

static inline NV_STATUS memoryfabricimportv2GetMemInterMapParams_DISPATCH(struct MemoryFabricImportV2 *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__(staticCast(pMemory, RmResource), pParams);
}

static inline NV_STATUS memoryfabricimportv2CheckMemInterUnmap_DISPATCH(struct MemoryFabricImportV2 *pMemory, NvBool bSubdeviceHandleProvided) {
    return pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__(staticCast(pMemory, RmResource), bSubdeviceHandleProvided);
}

static inline NV_STATUS memoryfabricimportv2GetMemoryMappingDescriptor_DISPATCH(struct MemoryFabricImportV2 *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__(staticCast(pMemory, RmResource), ppMemDesc);
}

static inline NV_STATUS memoryfabricimportv2CheckCopyPermissions_DISPATCH(struct MemoryFabricImportV2 *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memCheckCopyPermissions__(staticCast(pMemory, Memory), pDstGpu, pDstDevice);
}

static inline NvBool memoryfabricimportv2AccessCallback_DISPATCH(struct MemoryFabricImportV2 *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__(staticCast(pResource, RsResource), pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool memoryfabricimportv2ShareCallback_DISPATCH(struct MemoryFabricImportV2 *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__(staticCast(pResource, RsResource), pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS memoryfabricimportv2ControlSerialization_Prologue_DISPATCH(struct MemoryFabricImportV2 *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void memoryfabricimportv2ControlSerialization_Epilogue_DISPATCH(struct MemoryFabricImportV2 *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS memoryfabricimportv2Control_Prologue_DISPATCH(struct MemoryFabricImportV2 *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void memoryfabricimportv2Control_Epilogue_DISPATCH(struct MemoryFabricImportV2 *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void memoryfabricimportv2PreDestruct_DISPATCH(struct MemoryFabricImportV2 *pResource) {
    pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__(staticCast(pResource, RsResource));
}

static inline NV_STATUS memoryfabricimportv2ControlFilter_DISPATCH(struct MemoryFabricImportV2 *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool memoryfabricimportv2IsPartialUnmapSupported_DISPATCH(struct MemoryFabricImportV2 *pResource) {
    return pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__(staticCast(pResource, RsResource));
}

static inline NV_STATUS memoryfabricimportv2MapTo_DISPATCH(struct MemoryFabricImportV2 *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__(staticCast(pResource, RsResource), pParams);
}

static inline NV_STATUS memoryfabricimportv2UnmapFrom_DISPATCH(struct MemoryFabricImportV2 *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__(staticCast(pResource, RsResource), pParams);
}

static inline NvU32 memoryfabricimportv2GetRefCount_DISPATCH(struct MemoryFabricImportV2 *pResource) {
    return pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__(staticCast(pResource, RsResource));
}

static inline void memoryfabricimportv2AddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct MemoryFabricImportV2 *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__(pClient, staticCast(pResource, RsResource), pReference);
}

// Virtual method declarations and/or inline definitions
NvBool memoryfabricimportv2CanCopy_IMPL(struct MemoryFabricImportV2 *pMemoryFabricImportV2);

NV_STATUS memoryfabricimportv2IsReady_IMPL(struct MemoryFabricImportV2 *pMemoryFabricImportV2, NvBool bCopyConstructorContext);

NV_STATUS memoryfabricimportv2CopyConstruct_IMPL(struct MemoryFabricImportV2 *pMemoryFabricImportV2, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);

NV_STATUS memoryfabricimportv2Control_IMPL(struct MemoryFabricImportV2 *pMemoryFabricImportV2, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);

NV_STATUS memoryfabricimportv2GetMapAddrSpace_IMPL(struct MemoryFabricImportV2 *pMemoryFabricImportV2, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);

NvBool memoryfabricimportv2IsGpuMapAllowed_IMPL(struct MemoryFabricImportV2 *pMemoryFabricImportV2, struct OBJGPU *pGpu);

NvBool memoryfabricimportv2IsExportAllowed_IMPL(struct MemoryFabricImportV2 *pMemoryFabricImportV2);

// Exported method declarations and/or inline definitions
NV_STATUS memoryfabricimportv2CtrlGetInfo_IMPL(struct MemoryFabricImportV2 *pMemoryFabricImportV2, NV00F9_CTRL_GET_INFO_PARAMS *pParams);

// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


typedef struct
{
    NV_PHYSICAL_MEMORY_ATTRS physAttrs;
    NvU32 memFlags;
    NvU64 clique;
    NvU64 bwModeEpoch;
    NvU16 bwMode;
} FABRIC_IMPORT_MEMDESC_DATA;

//
// Gets (ref-counts) an import descriptor on which importDescriptorAttachMemDesc()
// is never called.
//
// importDescriptorPutNonBlocking() must be called to drop the ref-count.
//
MEM_FABRIC_IMPORT_DESCRIPTOR* importDescriptorGetUnused(const NvUuid *pExportUuid,
                                                        NvU16 index, NvU64 id);

//
// importDescriptorPutNonBlocking() doesn't generate an unimport event, which
// further blocks the thread on the fabric manager to complete the unimport request.
//
// This function must be used by the code paths executed by the fabric manager, so
// that we don't block the fabric manager thread.
//
void importDescriptorPutNonBlocking(MEM_FABRIC_IMPORT_DESCRIPTOR *pFabricImportDesc);

// Installs a memdesc to an import descriptor. A NULL memdesc is acceptable.
NV_STATUS importDescriptorInstallMemDesc(MEM_FABRIC_IMPORT_DESCRIPTOR *pFabricImportDesc,
                                         MEMORY_DESCRIPTOR *pMemDesc);

#endif /* _MEMORYFABRICIMPORTV2_H_ */

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_MEM_FABRIC_IMPORT_V2_NVOC_H_
