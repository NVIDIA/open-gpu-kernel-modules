
#ifndef _G_OS_DESC_MEM_NVOC_H_
#define _G_OS_DESC_MEM_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 1993-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_os_desc_mem_nvoc.h"

#ifndef _OS_DESC_MEMORY_H_
#define _OS_DESC_MEMORY_H_

#include "mem_mgr/mem.h"

/*!
 * Bind memory allocated through os descriptor
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_OS_DESC_MEM_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__OsDescMemory;
struct NVOC_METADATA__Memory;
struct NVOC_VTABLE__OsDescMemory;


struct OsDescMemory {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__OsDescMemory *__nvoc_metadata_ptr;
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
    struct OsDescMemory *__nvoc_pbase_OsDescMemory;    // osdesc
};


// Vtable with 26 per-class function pointers
struct NVOC_VTABLE__OsDescMemory {
    NvBool (*__osdescCanCopy__)(struct OsDescMemory * /*this*/);  // virtual override (res) base (mem)
    NV_STATUS (*__osdescIsDuplicate__)(struct OsDescMemory * /*this*/, NvHandle, NvBool *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__osdescGetMapAddrSpace__)(struct OsDescMemory * /*this*/, CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__osdescControl__)(struct OsDescMemory * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__osdescMap__)(struct OsDescMemory * /*this*/, CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, RsCpuMapping *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__osdescUnmap__)(struct OsDescMemory * /*this*/, CALL_CONTEXT *, RsCpuMapping *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__osdescGetMemInterMapParams__)(struct OsDescMemory * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__osdescCheckMemInterUnmap__)(struct OsDescMemory * /*this*/, NvBool);  // inline virtual inherited (mem) base (mem) body
    NV_STATUS (*__osdescGetMemoryMappingDescriptor__)(struct OsDescMemory * /*this*/, MEMORY_DESCRIPTOR **);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__osdescCheckCopyPermissions__)(struct OsDescMemory * /*this*/, struct OBJGPU *, struct Device *);  // inline virtual inherited (mem) base (mem) body
    NV_STATUS (*__osdescIsReady__)(struct OsDescMemory * /*this*/, NvBool);  // virtual inherited (mem) base (mem)
    NvBool (*__osdescIsGpuMapAllowed__)(struct OsDescMemory * /*this*/, struct OBJGPU *);  // inline virtual inherited (mem) base (mem) body
    NvBool (*__osdescIsExportAllowed__)(struct OsDescMemory * /*this*/);  // inline virtual inherited (mem) base (mem) body
    NvBool (*__osdescAccessCallback__)(struct OsDescMemory * /*this*/, RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (mem)
    NvBool (*__osdescShareCallback__)(struct OsDescMemory * /*this*/, RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (rmres) base (mem)
    NV_STATUS (*__osdescControlSerialization_Prologue__)(struct OsDescMemory * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (mem)
    void (*__osdescControlSerialization_Epilogue__)(struct OsDescMemory * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (mem)
    NV_STATUS (*__osdescControl_Prologue__)(struct OsDescMemory * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (mem)
    void (*__osdescControl_Epilogue__)(struct OsDescMemory * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (mem)
    void (*__osdescPreDestruct__)(struct OsDescMemory * /*this*/);  // virtual inherited (res) base (mem)
    NV_STATUS (*__osdescControlFilter__)(struct OsDescMemory * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (mem)
    NvBool (*__osdescIsPartialUnmapSupported__)(struct OsDescMemory * /*this*/);  // inline virtual inherited (res) base (mem) body
    NV_STATUS (*__osdescMapTo__)(struct OsDescMemory * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (mem)
    NV_STATUS (*__osdescUnmapFrom__)(struct OsDescMemory * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (mem)
    NvU32 (*__osdescGetRefCount__)(struct OsDescMemory * /*this*/);  // virtual inherited (res) base (mem)
    void (*__osdescAddAdditionalDependants__)(struct RsClient *, struct OsDescMemory * /*this*/, RsResourceRef *);  // virtual inherited (res) base (mem)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__OsDescMemory {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__Memory metadata__Memory;
    const struct NVOC_VTABLE__OsDescMemory vtable;
};

#ifndef __NVOC_CLASS_OsDescMemory_TYPEDEF__
#define __NVOC_CLASS_OsDescMemory_TYPEDEF__
typedef struct OsDescMemory OsDescMemory;
#endif /* __NVOC_CLASS_OsDescMemory_TYPEDEF__ */

#ifndef __nvoc_class_id_OsDescMemory
#define __nvoc_class_id_OsDescMemory 0xb3dacd
#endif /* __nvoc_class_id_OsDescMemory */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OsDescMemory;

#define __staticCast_OsDescMemory(pThis) \
    ((pThis)->__nvoc_pbase_OsDescMemory)

#ifdef __nvoc_os_desc_mem_h_disabled
#define __dynamicCast_OsDescMemory(pThis) ((OsDescMemory*) NULL)
#else //__nvoc_os_desc_mem_h_disabled
#define __dynamicCast_OsDescMemory(pThis) \
    ((OsDescMemory*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(OsDescMemory)))
#endif //__nvoc_os_desc_mem_h_disabled

NV_STATUS __nvoc_objCreateDynamic_OsDescMemory(OsDescMemory**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_OsDescMemory(OsDescMemory**, Dynamic*, NvU32, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __objCreate_OsDescMemory(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_OsDescMemory((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define osdescCanCopy_FNPTR(pOsDescMemory) pOsDescMemory->__nvoc_metadata_ptr->vtable.__osdescCanCopy__
#define osdescCanCopy(pOsDescMemory) osdescCanCopy_DISPATCH(pOsDescMemory)
#define osdescIsDuplicate_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsDuplicate__
#define osdescIsDuplicate(pMemory, hMemory, pDuplicate) osdescIsDuplicate_DISPATCH(pMemory, hMemory, pDuplicate)
#define osdescGetMapAddrSpace_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memGetMapAddrSpace__
#define osdescGetMapAddrSpace(pMemory, pCallContext, mapFlags, pAddrSpace) osdescGetMapAddrSpace_DISPATCH(pMemory, pCallContext, mapFlags, pAddrSpace)
#define osdescControl_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memControl__
#define osdescControl(pMemory, pCallContext, pParams) osdescControl_DISPATCH(pMemory, pCallContext, pParams)
#define osdescMap_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memMap__
#define osdescMap(pMemory, pCallContext, pParams, pCpuMapping) osdescMap_DISPATCH(pMemory, pCallContext, pParams, pCpuMapping)
#define osdescUnmap_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memUnmap__
#define osdescUnmap(pMemory, pCallContext, pCpuMapping) osdescUnmap_DISPATCH(pMemory, pCallContext, pCpuMapping)
#define osdescGetMemInterMapParams_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memGetMemInterMapParams__
#define osdescGetMemInterMapParams(pMemory, pParams) osdescGetMemInterMapParams_DISPATCH(pMemory, pParams)
#define osdescCheckMemInterUnmap_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memCheckMemInterUnmap__
#define osdescCheckMemInterUnmap(pMemory, bSubdeviceHandleProvided) osdescCheckMemInterUnmap_DISPATCH(pMemory, bSubdeviceHandleProvided)
#define osdescGetMemoryMappingDescriptor_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memGetMemoryMappingDescriptor__
#define osdescGetMemoryMappingDescriptor(pMemory, ppMemDesc) osdescGetMemoryMappingDescriptor_DISPATCH(pMemory, ppMemDesc)
#define osdescCheckCopyPermissions_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memCheckCopyPermissions__
#define osdescCheckCopyPermissions(pMemory, pDstGpu, pDstDevice) osdescCheckCopyPermissions_DISPATCH(pMemory, pDstGpu, pDstDevice)
#define osdescIsReady_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsReady__
#define osdescIsReady(pMemory, bCopyConstructorContext) osdescIsReady_DISPATCH(pMemory, bCopyConstructorContext)
#define osdescIsGpuMapAllowed_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsGpuMapAllowed__
#define osdescIsGpuMapAllowed(pMemory, pGpu) osdescIsGpuMapAllowed_DISPATCH(pMemory, pGpu)
#define osdescIsExportAllowed_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsExportAllowed__
#define osdescIsExportAllowed(pMemory) osdescIsExportAllowed_DISPATCH(pMemory)
#define osdescAccessCallback_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresAccessCallback__
#define osdescAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) osdescAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define osdescShareCallback_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresShareCallback__
#define osdescShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) osdescShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define osdescControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Prologue__
#define osdescControlSerialization_Prologue(pResource, pCallContext, pParams) osdescControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define osdescControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Epilogue__
#define osdescControlSerialization_Epilogue(pResource, pCallContext, pParams) osdescControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define osdescControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Prologue__
#define osdescControl_Prologue(pResource, pCallContext, pParams) osdescControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define osdescControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Epilogue__
#define osdescControl_Epilogue(pResource, pCallContext, pParams) osdescControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define osdescPreDestruct_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define osdescPreDestruct(pResource) osdescPreDestruct_DISPATCH(pResource)
#define osdescControlFilter_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define osdescControlFilter(pResource, pCallContext, pParams) osdescControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define osdescIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define osdescIsPartialUnmapSupported(pResource) osdescIsPartialUnmapSupported_DISPATCH(pResource)
#define osdescMapTo_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define osdescMapTo(pResource, pParams) osdescMapTo_DISPATCH(pResource, pParams)
#define osdescUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define osdescUnmapFrom(pResource, pParams) osdescUnmapFrom_DISPATCH(pResource, pParams)
#define osdescGetRefCount_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define osdescGetRefCount(pResource) osdescGetRefCount_DISPATCH(pResource)
#define osdescAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define osdescAddAdditionalDependants(pClient, pResource, pReference) osdescAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NvBool osdescCanCopy_DISPATCH(struct OsDescMemory *pOsDescMemory) {
    return pOsDescMemory->__nvoc_metadata_ptr->vtable.__osdescCanCopy__(pOsDescMemory);
}

static inline NV_STATUS osdescIsDuplicate_DISPATCH(struct OsDescMemory *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return pMemory->__nvoc_metadata_ptr->vtable.__osdescIsDuplicate__(pMemory, hMemory, pDuplicate);
}

static inline NV_STATUS osdescGetMapAddrSpace_DISPATCH(struct OsDescMemory *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pMemory->__nvoc_metadata_ptr->vtable.__osdescGetMapAddrSpace__(pMemory, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS osdescControl_DISPATCH(struct OsDescMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pMemory->__nvoc_metadata_ptr->vtable.__osdescControl__(pMemory, pCallContext, pParams);
}

static inline NV_STATUS osdescMap_DISPATCH(struct OsDescMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pMemory->__nvoc_metadata_ptr->vtable.__osdescMap__(pMemory, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS osdescUnmap_DISPATCH(struct OsDescMemory *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pMemory->__nvoc_metadata_ptr->vtable.__osdescUnmap__(pMemory, pCallContext, pCpuMapping);
}

static inline NV_STATUS osdescGetMemInterMapParams_DISPATCH(struct OsDescMemory *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pMemory->__nvoc_metadata_ptr->vtable.__osdescGetMemInterMapParams__(pMemory, pParams);
}

static inline NV_STATUS osdescCheckMemInterUnmap_DISPATCH(struct OsDescMemory *pMemory, NvBool bSubdeviceHandleProvided) {
    return pMemory->__nvoc_metadata_ptr->vtable.__osdescCheckMemInterUnmap__(pMemory, bSubdeviceHandleProvided);
}

static inline NV_STATUS osdescGetMemoryMappingDescriptor_DISPATCH(struct OsDescMemory *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return pMemory->__nvoc_metadata_ptr->vtable.__osdescGetMemoryMappingDescriptor__(pMemory, ppMemDesc);
}

static inline NV_STATUS osdescCheckCopyPermissions_DISPATCH(struct OsDescMemory *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return pMemory->__nvoc_metadata_ptr->vtable.__osdescCheckCopyPermissions__(pMemory, pDstGpu, pDstDevice);
}

static inline NV_STATUS osdescIsReady_DISPATCH(struct OsDescMemory *pMemory, NvBool bCopyConstructorContext) {
    return pMemory->__nvoc_metadata_ptr->vtable.__osdescIsReady__(pMemory, bCopyConstructorContext);
}

static inline NvBool osdescIsGpuMapAllowed_DISPATCH(struct OsDescMemory *pMemory, struct OBJGPU *pGpu) {
    return pMemory->__nvoc_metadata_ptr->vtable.__osdescIsGpuMapAllowed__(pMemory, pGpu);
}

static inline NvBool osdescIsExportAllowed_DISPATCH(struct OsDescMemory *pMemory) {
    return pMemory->__nvoc_metadata_ptr->vtable.__osdescIsExportAllowed__(pMemory);
}

static inline NvBool osdescAccessCallback_DISPATCH(struct OsDescMemory *pResource, RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_metadata_ptr->vtable.__osdescAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool osdescShareCallback_DISPATCH(struct OsDescMemory *pResource, RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__nvoc_metadata_ptr->vtable.__osdescShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS osdescControlSerialization_Prologue_DISPATCH(struct OsDescMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__osdescControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void osdescControlSerialization_Epilogue_DISPATCH(struct OsDescMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__osdescControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS osdescControl_Prologue_DISPATCH(struct OsDescMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__osdescControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void osdescControl_Epilogue_DISPATCH(struct OsDescMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__osdescControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline void osdescPreDestruct_DISPATCH(struct OsDescMemory *pResource) {
    pResource->__nvoc_metadata_ptr->vtable.__osdescPreDestruct__(pResource);
}

static inline NV_STATUS osdescControlFilter_DISPATCH(struct OsDescMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__osdescControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool osdescIsPartialUnmapSupported_DISPATCH(struct OsDescMemory *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__osdescIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS osdescMapTo_DISPATCH(struct OsDescMemory *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__osdescMapTo__(pResource, pParams);
}

static inline NV_STATUS osdescUnmapFrom_DISPATCH(struct OsDescMemory *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__osdescUnmapFrom__(pResource, pParams);
}

static inline NvU32 osdescGetRefCount_DISPATCH(struct OsDescMemory *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__osdescGetRefCount__(pResource);
}

static inline void osdescAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct OsDescMemory *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_metadata_ptr->vtable.__osdescAddAdditionalDependants__(pClient, pResource, pReference);
}

NvBool osdescCanCopy_IMPL(struct OsDescMemory *pOsDescMemory);

NV_STATUS osdescConstruct_IMPL(struct OsDescMemory *arg_pOsDescMemory, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_osdescConstruct(arg_pOsDescMemory, arg_pCallContext, arg_pParams) osdescConstruct_IMPL(arg_pOsDescMemory, arg_pCallContext, arg_pParams)
void osdescDestruct_IMPL(struct OsDescMemory *pOsDescMemory);

#define __nvoc_osdescDestruct(pOsDescMemory) osdescDestruct_IMPL(pOsDescMemory)
#undef PRIVATE_FIELD


#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_OS_DESC_MEM_NVOC_H_
