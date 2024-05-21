
#ifndef _G_VIRT_MEM_RANGE_NVOC_H_
#define _G_VIRT_MEM_RANGE_NVOC_H_
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
#include "g_virt_mem_range_nvoc.h"

#ifndef _VIRT_MEM_RANGE_H_
#define _VIRT_MEM_RANGE_H_

#include "mem_mgr/virtual_mem.h"

/*!
 * Allocator for NV01_MEMORY_VIRTUAL class.
 *
 * Describes a range of typeless virtual memory memory.  Used as
 * a target space for RmMapMemoryDma.
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_VIRT_MEM_RANGE_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct VirtualMemoryRange {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct VirtualMemory __nvoc_base_VirtualMemory;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^6
    struct RsResource *__nvoc_pbase_RsResource;    // res super^5
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^5
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super^4
    struct Memory *__nvoc_pbase_Memory;    // mem super^3
    struct StandardMemory *__nvoc_pbase_StandardMemory;    // stdmem super^2
    struct VirtualMemory *__nvoc_pbase_VirtualMemory;    // virtmem super
    struct VirtualMemoryRange *__nvoc_pbase_VirtualMemoryRange;    // vmrange

    // Vtable with 26 per-object function pointers
    NV_STATUS (*__vmrangeMapTo__)(struct VirtualMemoryRange * /*this*/, struct RS_RES_MAP_TO_PARAMS *);  // virtual inherited (virtmem) base (virtmem)
    NV_STATUS (*__vmrangeUnmapFrom__)(struct VirtualMemoryRange * /*this*/, struct RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (virtmem) base (virtmem)
    NvBool (*__vmrangeIsPartialUnmapSupported__)(struct VirtualMemoryRange * /*this*/);  // inline virtual inherited (virtmem) base (virtmem) body
    NvBool (*__vmrangeCanCopy__)(struct VirtualMemoryRange * /*this*/);  // virtual inherited (stdmem) base (virtmem)
    NV_STATUS (*__vmrangeIsDuplicate__)(struct VirtualMemoryRange * /*this*/, NvHandle, NvBool *);  // virtual inherited (mem) base (virtmem)
    NV_STATUS (*__vmrangeGetMapAddrSpace__)(struct VirtualMemoryRange * /*this*/, CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (mem) base (virtmem)
    NV_STATUS (*__vmrangeControl__)(struct VirtualMemoryRange * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (mem) base (virtmem)
    NV_STATUS (*__vmrangeMap__)(struct VirtualMemoryRange * /*this*/, CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, RsCpuMapping *);  // virtual inherited (mem) base (virtmem)
    NV_STATUS (*__vmrangeUnmap__)(struct VirtualMemoryRange * /*this*/, CALL_CONTEXT *, RsCpuMapping *);  // virtual inherited (mem) base (virtmem)
    NV_STATUS (*__vmrangeGetMemInterMapParams__)(struct VirtualMemoryRange * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (mem) base (virtmem)
    NV_STATUS (*__vmrangeCheckMemInterUnmap__)(struct VirtualMemoryRange * /*this*/, NvBool);  // inline virtual inherited (mem) base (virtmem) body
    NV_STATUS (*__vmrangeGetMemoryMappingDescriptor__)(struct VirtualMemoryRange * /*this*/, MEMORY_DESCRIPTOR **);  // virtual inherited (mem) base (virtmem)
    NV_STATUS (*__vmrangeCheckCopyPermissions__)(struct VirtualMemoryRange * /*this*/, struct OBJGPU *, struct Device *);  // inline virtual inherited (mem) base (virtmem) body
    NV_STATUS (*__vmrangeIsReady__)(struct VirtualMemoryRange * /*this*/, NvBool);  // virtual inherited (mem) base (virtmem)
    NvBool (*__vmrangeIsGpuMapAllowed__)(struct VirtualMemoryRange * /*this*/, struct OBJGPU *);  // inline virtual inherited (mem) base (virtmem) body
    NvBool (*__vmrangeIsExportAllowed__)(struct VirtualMemoryRange * /*this*/);  // inline virtual inherited (mem) base (virtmem) body
    NvBool (*__vmrangeAccessCallback__)(struct VirtualMemoryRange * /*this*/, RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (virtmem)
    NvBool (*__vmrangeShareCallback__)(struct VirtualMemoryRange * /*this*/, RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (rmres) base (virtmem)
    NV_STATUS (*__vmrangeControlSerialization_Prologue__)(struct VirtualMemoryRange * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (virtmem)
    void (*__vmrangeControlSerialization_Epilogue__)(struct VirtualMemoryRange * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (virtmem)
    NV_STATUS (*__vmrangeControl_Prologue__)(struct VirtualMemoryRange * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (virtmem)
    void (*__vmrangeControl_Epilogue__)(struct VirtualMemoryRange * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (virtmem)
    void (*__vmrangePreDestruct__)(struct VirtualMemoryRange * /*this*/);  // virtual inherited (res) base (virtmem)
    NV_STATUS (*__vmrangeControlFilter__)(struct VirtualMemoryRange * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (virtmem)
    NvU32 (*__vmrangeGetRefCount__)(struct VirtualMemoryRange * /*this*/);  // virtual inherited (res) base (virtmem)
    void (*__vmrangeAddAdditionalDependants__)(struct RsClient *, struct VirtualMemoryRange * /*this*/, RsResourceRef *);  // virtual inherited (res) base (virtmem)
};

#ifndef __NVOC_CLASS_VirtualMemoryRange_TYPEDEF__
#define __NVOC_CLASS_VirtualMemoryRange_TYPEDEF__
typedef struct VirtualMemoryRange VirtualMemoryRange;
#endif /* __NVOC_CLASS_VirtualMemoryRange_TYPEDEF__ */

#ifndef __nvoc_class_id_VirtualMemoryRange
#define __nvoc_class_id_VirtualMemoryRange 0x7032c6
#endif /* __nvoc_class_id_VirtualMemoryRange */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_VirtualMemoryRange;

#define __staticCast_VirtualMemoryRange(pThis) \
    ((pThis)->__nvoc_pbase_VirtualMemoryRange)

#ifdef __nvoc_virt_mem_range_h_disabled
#define __dynamicCast_VirtualMemoryRange(pThis) ((VirtualMemoryRange*)NULL)
#else //__nvoc_virt_mem_range_h_disabled
#define __dynamicCast_VirtualMemoryRange(pThis) \
    ((VirtualMemoryRange*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(VirtualMemoryRange)))
#endif //__nvoc_virt_mem_range_h_disabled

NV_STATUS __nvoc_objCreateDynamic_VirtualMemoryRange(VirtualMemoryRange**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_VirtualMemoryRange(VirtualMemoryRange**, Dynamic*, NvU32, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_VirtualMemoryRange(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_VirtualMemoryRange((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define vmrangeMapTo_FNPTR(pVirtualMemory) pVirtualMemory->__nvoc_base_VirtualMemory.__virtmemMapTo__
#define vmrangeMapTo(pVirtualMemory, pParams) vmrangeMapTo_DISPATCH(pVirtualMemory, pParams)
#define vmrangeUnmapFrom_FNPTR(pVirtualMemory) pVirtualMemory->__nvoc_base_VirtualMemory.__virtmemUnmapFrom__
#define vmrangeUnmapFrom(pVirtualMemory, pParams) vmrangeUnmapFrom_DISPATCH(pVirtualMemory, pParams)
#define vmrangeIsPartialUnmapSupported_FNPTR(pVirtualMemory) pVirtualMemory->__nvoc_base_VirtualMemory.__virtmemIsPartialUnmapSupported__
#define vmrangeIsPartialUnmapSupported(pVirtualMemory) vmrangeIsPartialUnmapSupported_DISPATCH(pVirtualMemory)
#define vmrangeCanCopy_FNPTR(pStandardMemory) pStandardMemory->__nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__stdmemCanCopy__
#define vmrangeCanCopy(pStandardMemory) vmrangeCanCopy_DISPATCH(pStandardMemory)
#define vmrangeIsDuplicate_FNPTR(pMemory) pMemory->__nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__memIsDuplicate__
#define vmrangeIsDuplicate(pMemory, hMemory, pDuplicate) vmrangeIsDuplicate_DISPATCH(pMemory, hMemory, pDuplicate)
#define vmrangeGetMapAddrSpace_FNPTR(pMemory) pMemory->__nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__memGetMapAddrSpace__
#define vmrangeGetMapAddrSpace(pMemory, pCallContext, mapFlags, pAddrSpace) vmrangeGetMapAddrSpace_DISPATCH(pMemory, pCallContext, mapFlags, pAddrSpace)
#define vmrangeControl_FNPTR(pMemory) pMemory->__nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__memControl__
#define vmrangeControl(pMemory, pCallContext, pParams) vmrangeControl_DISPATCH(pMemory, pCallContext, pParams)
#define vmrangeMap_FNPTR(pMemory) pMemory->__nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__memMap__
#define vmrangeMap(pMemory, pCallContext, pParams, pCpuMapping) vmrangeMap_DISPATCH(pMemory, pCallContext, pParams, pCpuMapping)
#define vmrangeUnmap_FNPTR(pMemory) pMemory->__nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__memUnmap__
#define vmrangeUnmap(pMemory, pCallContext, pCpuMapping) vmrangeUnmap_DISPATCH(pMemory, pCallContext, pCpuMapping)
#define vmrangeGetMemInterMapParams_FNPTR(pMemory) pMemory->__nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__memGetMemInterMapParams__
#define vmrangeGetMemInterMapParams(pMemory, pParams) vmrangeGetMemInterMapParams_DISPATCH(pMemory, pParams)
#define vmrangeCheckMemInterUnmap_FNPTR(pMemory) pMemory->__nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__memCheckMemInterUnmap__
#define vmrangeCheckMemInterUnmap(pMemory, bSubdeviceHandleProvided) vmrangeCheckMemInterUnmap_DISPATCH(pMemory, bSubdeviceHandleProvided)
#define vmrangeGetMemoryMappingDescriptor_FNPTR(pMemory) pMemory->__nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__memGetMemoryMappingDescriptor__
#define vmrangeGetMemoryMappingDescriptor(pMemory, ppMemDesc) vmrangeGetMemoryMappingDescriptor_DISPATCH(pMemory, ppMemDesc)
#define vmrangeCheckCopyPermissions_FNPTR(pMemory) pMemory->__nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__memCheckCopyPermissions__
#define vmrangeCheckCopyPermissions(pMemory, pDstGpu, pDstDevice) vmrangeCheckCopyPermissions_DISPATCH(pMemory, pDstGpu, pDstDevice)
#define vmrangeIsReady_FNPTR(pMemory) pMemory->__nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__memIsReady__
#define vmrangeIsReady(pMemory, bCopyConstructorContext) vmrangeIsReady_DISPATCH(pMemory, bCopyConstructorContext)
#define vmrangeIsGpuMapAllowed_FNPTR(pMemory) pMemory->__nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__memIsGpuMapAllowed__
#define vmrangeIsGpuMapAllowed(pMemory, pGpu) vmrangeIsGpuMapAllowed_DISPATCH(pMemory, pGpu)
#define vmrangeIsExportAllowed_FNPTR(pMemory) pMemory->__nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__memIsExportAllowed__
#define vmrangeIsExportAllowed(pMemory) vmrangeIsExportAllowed_DISPATCH(pMemory)
#define vmrangeAccessCallback_FNPTR(pResource) pResource->__nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__rmresAccessCallback__
#define vmrangeAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) vmrangeAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define vmrangeShareCallback_FNPTR(pResource) pResource->__nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__rmresShareCallback__
#define vmrangeShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) vmrangeShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define vmrangeControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__rmresControlSerialization_Prologue__
#define vmrangeControlSerialization_Prologue(pResource, pCallContext, pParams) vmrangeControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define vmrangeControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__rmresControlSerialization_Epilogue__
#define vmrangeControlSerialization_Epilogue(pResource, pCallContext, pParams) vmrangeControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define vmrangeControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__rmresControl_Prologue__
#define vmrangeControl_Prologue(pResource, pCallContext, pParams) vmrangeControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define vmrangeControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__rmresControl_Epilogue__
#define vmrangeControl_Epilogue(pResource, pCallContext, pParams) vmrangeControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define vmrangePreDestruct_FNPTR(pResource) pResource->__nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resPreDestruct__
#define vmrangePreDestruct(pResource) vmrangePreDestruct_DISPATCH(pResource)
#define vmrangeControlFilter_FNPTR(pResource) pResource->__nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resControlFilter__
#define vmrangeControlFilter(pResource, pCallContext, pParams) vmrangeControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define vmrangeGetRefCount_FNPTR(pResource) pResource->__nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resGetRefCount__
#define vmrangeGetRefCount(pResource) vmrangeGetRefCount_DISPATCH(pResource)
#define vmrangeAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_VirtualMemory.__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resAddAdditionalDependants__
#define vmrangeAddAdditionalDependants(pClient, pResource, pReference) vmrangeAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NV_STATUS vmrangeMapTo_DISPATCH(struct VirtualMemoryRange *pVirtualMemory, struct RS_RES_MAP_TO_PARAMS *pParams) {
    return pVirtualMemory->__vmrangeMapTo__(pVirtualMemory, pParams);
}

static inline NV_STATUS vmrangeUnmapFrom_DISPATCH(struct VirtualMemoryRange *pVirtualMemory, struct RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pVirtualMemory->__vmrangeUnmapFrom__(pVirtualMemory, pParams);
}

static inline NvBool vmrangeIsPartialUnmapSupported_DISPATCH(struct VirtualMemoryRange *pVirtualMemory) {
    return pVirtualMemory->__vmrangeIsPartialUnmapSupported__(pVirtualMemory);
}

static inline NvBool vmrangeCanCopy_DISPATCH(struct VirtualMemoryRange *pStandardMemory) {
    return pStandardMemory->__vmrangeCanCopy__(pStandardMemory);
}

static inline NV_STATUS vmrangeIsDuplicate_DISPATCH(struct VirtualMemoryRange *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return pMemory->__vmrangeIsDuplicate__(pMemory, hMemory, pDuplicate);
}

static inline NV_STATUS vmrangeGetMapAddrSpace_DISPATCH(struct VirtualMemoryRange *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pMemory->__vmrangeGetMapAddrSpace__(pMemory, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS vmrangeControl_DISPATCH(struct VirtualMemoryRange *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pMemory->__vmrangeControl__(pMemory, pCallContext, pParams);
}

static inline NV_STATUS vmrangeMap_DISPATCH(struct VirtualMemoryRange *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pMemory->__vmrangeMap__(pMemory, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS vmrangeUnmap_DISPATCH(struct VirtualMemoryRange *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pMemory->__vmrangeUnmap__(pMemory, pCallContext, pCpuMapping);
}

static inline NV_STATUS vmrangeGetMemInterMapParams_DISPATCH(struct VirtualMemoryRange *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pMemory->__vmrangeGetMemInterMapParams__(pMemory, pParams);
}

static inline NV_STATUS vmrangeCheckMemInterUnmap_DISPATCH(struct VirtualMemoryRange *pMemory, NvBool bSubdeviceHandleProvided) {
    return pMemory->__vmrangeCheckMemInterUnmap__(pMemory, bSubdeviceHandleProvided);
}

static inline NV_STATUS vmrangeGetMemoryMappingDescriptor_DISPATCH(struct VirtualMemoryRange *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return pMemory->__vmrangeGetMemoryMappingDescriptor__(pMemory, ppMemDesc);
}

static inline NV_STATUS vmrangeCheckCopyPermissions_DISPATCH(struct VirtualMemoryRange *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return pMemory->__vmrangeCheckCopyPermissions__(pMemory, pDstGpu, pDstDevice);
}

static inline NV_STATUS vmrangeIsReady_DISPATCH(struct VirtualMemoryRange *pMemory, NvBool bCopyConstructorContext) {
    return pMemory->__vmrangeIsReady__(pMemory, bCopyConstructorContext);
}

static inline NvBool vmrangeIsGpuMapAllowed_DISPATCH(struct VirtualMemoryRange *pMemory, struct OBJGPU *pGpu) {
    return pMemory->__vmrangeIsGpuMapAllowed__(pMemory, pGpu);
}

static inline NvBool vmrangeIsExportAllowed_DISPATCH(struct VirtualMemoryRange *pMemory) {
    return pMemory->__vmrangeIsExportAllowed__(pMemory);
}

static inline NvBool vmrangeAccessCallback_DISPATCH(struct VirtualMemoryRange *pResource, RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__vmrangeAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool vmrangeShareCallback_DISPATCH(struct VirtualMemoryRange *pResource, RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__vmrangeShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS vmrangeControlSerialization_Prologue_DISPATCH(struct VirtualMemoryRange *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__vmrangeControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void vmrangeControlSerialization_Epilogue_DISPATCH(struct VirtualMemoryRange *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__vmrangeControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS vmrangeControl_Prologue_DISPATCH(struct VirtualMemoryRange *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__vmrangeControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void vmrangeControl_Epilogue_DISPATCH(struct VirtualMemoryRange *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__vmrangeControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline void vmrangePreDestruct_DISPATCH(struct VirtualMemoryRange *pResource) {
    pResource->__vmrangePreDestruct__(pResource);
}

static inline NV_STATUS vmrangeControlFilter_DISPATCH(struct VirtualMemoryRange *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__vmrangeControlFilter__(pResource, pCallContext, pParams);
}

static inline NvU32 vmrangeGetRefCount_DISPATCH(struct VirtualMemoryRange *pResource) {
    return pResource->__vmrangeGetRefCount__(pResource);
}

static inline void vmrangeAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct VirtualMemoryRange *pResource, RsResourceRef *pReference) {
    pResource->__vmrangeAddAdditionalDependants__(pClient, pResource, pReference);
}

NV_STATUS vmrangeConstruct_IMPL(struct VirtualMemoryRange *arg_pVmRange, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_vmrangeConstruct(arg_pVmRange, arg_pCallContext, arg_pParams) vmrangeConstruct_IMPL(arg_pVmRange, arg_pCallContext, arg_pParams)
#undef PRIVATE_FIELD


#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_VIRT_MEM_RANGE_NVOC_H_
