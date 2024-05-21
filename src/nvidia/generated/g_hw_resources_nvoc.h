
#ifndef _G_HW_RESOURCES_NVOC_H_
#define _G_HW_RESOURCES_NVOC_H_
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
#include "g_hw_resources_nvoc.h"

#ifndef _HW_RESOURCES_H_
#define _HW_RESOURCES_H_

#include "mem_mgr/mem.h"

/*!
 * Allocator for normal virtual, video and system memory
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_HW_RESOURCES_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct MemoryHwResources {

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
    struct MemoryHwResources *__nvoc_pbase_MemoryHwResources;    // hwres

    // Vtable with 26 per-object function pointers
    NvBool (*__hwresCanCopy__)(struct MemoryHwResources * /*this*/);  // virtual override (res) base (mem)
    NV_STATUS (*__hwresIsDuplicate__)(struct MemoryHwResources * /*this*/, NvHandle, NvBool *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__hwresGetMapAddrSpace__)(struct MemoryHwResources * /*this*/, CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__hwresControl__)(struct MemoryHwResources * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__hwresMap__)(struct MemoryHwResources * /*this*/, CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, RsCpuMapping *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__hwresUnmap__)(struct MemoryHwResources * /*this*/, CALL_CONTEXT *, RsCpuMapping *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__hwresGetMemInterMapParams__)(struct MemoryHwResources * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__hwresCheckMemInterUnmap__)(struct MemoryHwResources * /*this*/, NvBool);  // inline virtual inherited (mem) base (mem) body
    NV_STATUS (*__hwresGetMemoryMappingDescriptor__)(struct MemoryHwResources * /*this*/, MEMORY_DESCRIPTOR **);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__hwresCheckCopyPermissions__)(struct MemoryHwResources * /*this*/, struct OBJGPU *, struct Device *);  // inline virtual inherited (mem) base (mem) body
    NV_STATUS (*__hwresIsReady__)(struct MemoryHwResources * /*this*/, NvBool);  // virtual inherited (mem) base (mem)
    NvBool (*__hwresIsGpuMapAllowed__)(struct MemoryHwResources * /*this*/, struct OBJGPU *);  // inline virtual inherited (mem) base (mem) body
    NvBool (*__hwresIsExportAllowed__)(struct MemoryHwResources * /*this*/);  // inline virtual inherited (mem) base (mem) body
    NvBool (*__hwresAccessCallback__)(struct MemoryHwResources * /*this*/, RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (mem)
    NvBool (*__hwresShareCallback__)(struct MemoryHwResources * /*this*/, RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (rmres) base (mem)
    NV_STATUS (*__hwresControlSerialization_Prologue__)(struct MemoryHwResources * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (mem)
    void (*__hwresControlSerialization_Epilogue__)(struct MemoryHwResources * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (mem)
    NV_STATUS (*__hwresControl_Prologue__)(struct MemoryHwResources * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (mem)
    void (*__hwresControl_Epilogue__)(struct MemoryHwResources * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (mem)
    void (*__hwresPreDestruct__)(struct MemoryHwResources * /*this*/);  // virtual inherited (res) base (mem)
    NV_STATUS (*__hwresControlFilter__)(struct MemoryHwResources * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (mem)
    NvBool (*__hwresIsPartialUnmapSupported__)(struct MemoryHwResources * /*this*/);  // inline virtual inherited (res) base (mem) body
    NV_STATUS (*__hwresMapTo__)(struct MemoryHwResources * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (mem)
    NV_STATUS (*__hwresUnmapFrom__)(struct MemoryHwResources * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (mem)
    NvU32 (*__hwresGetRefCount__)(struct MemoryHwResources * /*this*/);  // virtual inherited (res) base (mem)
    void (*__hwresAddAdditionalDependants__)(struct RsClient *, struct MemoryHwResources * /*this*/, RsResourceRef *);  // virtual inherited (res) base (mem)
};

#ifndef __NVOC_CLASS_MemoryHwResources_TYPEDEF__
#define __NVOC_CLASS_MemoryHwResources_TYPEDEF__
typedef struct MemoryHwResources MemoryHwResources;
#endif /* __NVOC_CLASS_MemoryHwResources_TYPEDEF__ */

#ifndef __nvoc_class_id_MemoryHwResources
#define __nvoc_class_id_MemoryHwResources 0x9a2a71
#endif /* __nvoc_class_id_MemoryHwResources */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_MemoryHwResources;

#define __staticCast_MemoryHwResources(pThis) \
    ((pThis)->__nvoc_pbase_MemoryHwResources)

#ifdef __nvoc_hw_resources_h_disabled
#define __dynamicCast_MemoryHwResources(pThis) ((MemoryHwResources*)NULL)
#else //__nvoc_hw_resources_h_disabled
#define __dynamicCast_MemoryHwResources(pThis) \
    ((MemoryHwResources*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(MemoryHwResources)))
#endif //__nvoc_hw_resources_h_disabled

NV_STATUS __nvoc_objCreateDynamic_MemoryHwResources(MemoryHwResources**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_MemoryHwResources(MemoryHwResources**, Dynamic*, NvU32, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_MemoryHwResources(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_MemoryHwResources((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define hwresCanCopy_FNPTR(pMemoryHwResources) pMemoryHwResources->__hwresCanCopy__
#define hwresCanCopy(pMemoryHwResources) hwresCanCopy_DISPATCH(pMemoryHwResources)
#define hwresIsDuplicate_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memIsDuplicate__
#define hwresIsDuplicate(pMemory, hMemory, pDuplicate) hwresIsDuplicate_DISPATCH(pMemory, hMemory, pDuplicate)
#define hwresGetMapAddrSpace_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memGetMapAddrSpace__
#define hwresGetMapAddrSpace(pMemory, pCallContext, mapFlags, pAddrSpace) hwresGetMapAddrSpace_DISPATCH(pMemory, pCallContext, mapFlags, pAddrSpace)
#define hwresControl_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memControl__
#define hwresControl(pMemory, pCallContext, pParams) hwresControl_DISPATCH(pMemory, pCallContext, pParams)
#define hwresMap_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memMap__
#define hwresMap(pMemory, pCallContext, pParams, pCpuMapping) hwresMap_DISPATCH(pMemory, pCallContext, pParams, pCpuMapping)
#define hwresUnmap_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memUnmap__
#define hwresUnmap(pMemory, pCallContext, pCpuMapping) hwresUnmap_DISPATCH(pMemory, pCallContext, pCpuMapping)
#define hwresGetMemInterMapParams_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memGetMemInterMapParams__
#define hwresGetMemInterMapParams(pMemory, pParams) hwresGetMemInterMapParams_DISPATCH(pMemory, pParams)
#define hwresCheckMemInterUnmap_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memCheckMemInterUnmap__
#define hwresCheckMemInterUnmap(pMemory, bSubdeviceHandleProvided) hwresCheckMemInterUnmap_DISPATCH(pMemory, bSubdeviceHandleProvided)
#define hwresGetMemoryMappingDescriptor_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memGetMemoryMappingDescriptor__
#define hwresGetMemoryMappingDescriptor(pMemory, ppMemDesc) hwresGetMemoryMappingDescriptor_DISPATCH(pMemory, ppMemDesc)
#define hwresCheckCopyPermissions_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memCheckCopyPermissions__
#define hwresCheckCopyPermissions(pMemory, pDstGpu, pDstDevice) hwresCheckCopyPermissions_DISPATCH(pMemory, pDstGpu, pDstDevice)
#define hwresIsReady_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memIsReady__
#define hwresIsReady(pMemory, bCopyConstructorContext) hwresIsReady_DISPATCH(pMemory, bCopyConstructorContext)
#define hwresIsGpuMapAllowed_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memIsGpuMapAllowed__
#define hwresIsGpuMapAllowed(pMemory, pGpu) hwresIsGpuMapAllowed_DISPATCH(pMemory, pGpu)
#define hwresIsExportAllowed_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memIsExportAllowed__
#define hwresIsExportAllowed(pMemory) hwresIsExportAllowed_DISPATCH(pMemory)
#define hwresAccessCallback_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__rmresAccessCallback__
#define hwresAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) hwresAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define hwresShareCallback_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__rmresShareCallback__
#define hwresShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) hwresShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define hwresControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__rmresControlSerialization_Prologue__
#define hwresControlSerialization_Prologue(pResource, pCallContext, pParams) hwresControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define hwresControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__rmresControlSerialization_Epilogue__
#define hwresControlSerialization_Epilogue(pResource, pCallContext, pParams) hwresControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define hwresControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__rmresControl_Prologue__
#define hwresControl_Prologue(pResource, pCallContext, pParams) hwresControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define hwresControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__rmresControl_Epilogue__
#define hwresControl_Epilogue(pResource, pCallContext, pParams) hwresControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define hwresPreDestruct_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resPreDestruct__
#define hwresPreDestruct(pResource) hwresPreDestruct_DISPATCH(pResource)
#define hwresControlFilter_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resControlFilter__
#define hwresControlFilter(pResource, pCallContext, pParams) hwresControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define hwresIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsPartialUnmapSupported__
#define hwresIsPartialUnmapSupported(pResource) hwresIsPartialUnmapSupported_DISPATCH(pResource)
#define hwresMapTo_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resMapTo__
#define hwresMapTo(pResource, pParams) hwresMapTo_DISPATCH(pResource, pParams)
#define hwresUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmapFrom__
#define hwresUnmapFrom(pResource, pParams) hwresUnmapFrom_DISPATCH(pResource, pParams)
#define hwresGetRefCount_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resGetRefCount__
#define hwresGetRefCount(pResource) hwresGetRefCount_DISPATCH(pResource)
#define hwresAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resAddAdditionalDependants__
#define hwresAddAdditionalDependants(pClient, pResource, pReference) hwresAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NvBool hwresCanCopy_DISPATCH(struct MemoryHwResources *pMemoryHwResources) {
    return pMemoryHwResources->__hwresCanCopy__(pMemoryHwResources);
}

static inline NV_STATUS hwresIsDuplicate_DISPATCH(struct MemoryHwResources *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return pMemory->__hwresIsDuplicate__(pMemory, hMemory, pDuplicate);
}

static inline NV_STATUS hwresGetMapAddrSpace_DISPATCH(struct MemoryHwResources *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pMemory->__hwresGetMapAddrSpace__(pMemory, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS hwresControl_DISPATCH(struct MemoryHwResources *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pMemory->__hwresControl__(pMemory, pCallContext, pParams);
}

static inline NV_STATUS hwresMap_DISPATCH(struct MemoryHwResources *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pMemory->__hwresMap__(pMemory, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS hwresUnmap_DISPATCH(struct MemoryHwResources *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pMemory->__hwresUnmap__(pMemory, pCallContext, pCpuMapping);
}

static inline NV_STATUS hwresGetMemInterMapParams_DISPATCH(struct MemoryHwResources *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pMemory->__hwresGetMemInterMapParams__(pMemory, pParams);
}

static inline NV_STATUS hwresCheckMemInterUnmap_DISPATCH(struct MemoryHwResources *pMemory, NvBool bSubdeviceHandleProvided) {
    return pMemory->__hwresCheckMemInterUnmap__(pMemory, bSubdeviceHandleProvided);
}

static inline NV_STATUS hwresGetMemoryMappingDescriptor_DISPATCH(struct MemoryHwResources *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return pMemory->__hwresGetMemoryMappingDescriptor__(pMemory, ppMemDesc);
}

static inline NV_STATUS hwresCheckCopyPermissions_DISPATCH(struct MemoryHwResources *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return pMemory->__hwresCheckCopyPermissions__(pMemory, pDstGpu, pDstDevice);
}

static inline NV_STATUS hwresIsReady_DISPATCH(struct MemoryHwResources *pMemory, NvBool bCopyConstructorContext) {
    return pMemory->__hwresIsReady__(pMemory, bCopyConstructorContext);
}

static inline NvBool hwresIsGpuMapAllowed_DISPATCH(struct MemoryHwResources *pMemory, struct OBJGPU *pGpu) {
    return pMemory->__hwresIsGpuMapAllowed__(pMemory, pGpu);
}

static inline NvBool hwresIsExportAllowed_DISPATCH(struct MemoryHwResources *pMemory) {
    return pMemory->__hwresIsExportAllowed__(pMemory);
}

static inline NvBool hwresAccessCallback_DISPATCH(struct MemoryHwResources *pResource, RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__hwresAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool hwresShareCallback_DISPATCH(struct MemoryHwResources *pResource, RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__hwresShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS hwresControlSerialization_Prologue_DISPATCH(struct MemoryHwResources *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__hwresControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void hwresControlSerialization_Epilogue_DISPATCH(struct MemoryHwResources *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__hwresControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS hwresControl_Prologue_DISPATCH(struct MemoryHwResources *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__hwresControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void hwresControl_Epilogue_DISPATCH(struct MemoryHwResources *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__hwresControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline void hwresPreDestruct_DISPATCH(struct MemoryHwResources *pResource) {
    pResource->__hwresPreDestruct__(pResource);
}

static inline NV_STATUS hwresControlFilter_DISPATCH(struct MemoryHwResources *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__hwresControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool hwresIsPartialUnmapSupported_DISPATCH(struct MemoryHwResources *pResource) {
    return pResource->__hwresIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS hwresMapTo_DISPATCH(struct MemoryHwResources *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__hwresMapTo__(pResource, pParams);
}

static inline NV_STATUS hwresUnmapFrom_DISPATCH(struct MemoryHwResources *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__hwresUnmapFrom__(pResource, pParams);
}

static inline NvU32 hwresGetRefCount_DISPATCH(struct MemoryHwResources *pResource) {
    return pResource->__hwresGetRefCount__(pResource);
}

static inline void hwresAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct MemoryHwResources *pResource, RsResourceRef *pReference) {
    pResource->__hwresAddAdditionalDependants__(pClient, pResource, pReference);
}

NvBool hwresCanCopy_IMPL(struct MemoryHwResources *pMemoryHwResources);

NV_STATUS hwresConstruct_IMPL(struct MemoryHwResources *arg_pMemoryHwResources, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_hwresConstruct(arg_pMemoryHwResources, arg_pCallContext, arg_pParams) hwresConstruct_IMPL(arg_pMemoryHwResources, arg_pCallContext, arg_pParams)
void hwresDestruct_IMPL(struct MemoryHwResources *pMemoryHwResources);

#define __nvoc_hwresDestruct(pMemoryHwResources) hwresDestruct_IMPL(pMemoryHwResources)
#undef PRIVATE_FIELD


#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_HW_RESOURCES_NVOC_H_
