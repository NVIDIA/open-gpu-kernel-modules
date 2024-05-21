
#ifndef _G_FLA_MEM_NVOC_H_
#define _G_FLA_MEM_NVOC_H_
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
#include "g_fla_mem_nvoc.h"

#ifndef _FLA_MEMORY_H_
#define _FLA_MEMORY_H_

#include "mem_mgr/mem.h"

/*!
 * This class is used by the FLA clients to allocate FLA memory handle
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_FLA_MEM_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct FlaMemory {

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
    struct FlaMemory *__nvoc_pbase_FlaMemory;    // flamem

    // Vtable with 27 per-object function pointers
    NvBool (*__flamemCanCopy__)(struct FlaMemory * /*this*/);  // virtual override (res) base (mem)
    NV_STATUS (*__flamemCopyConstruct__)(struct FlaMemory * /*this*/, CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);  // virtual override (mem) base (mem)
    NV_STATUS (*__flamemIsDuplicate__)(struct FlaMemory * /*this*/, NvHandle, NvBool *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__flamemGetMapAddrSpace__)(struct FlaMemory * /*this*/, CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__flamemControl__)(struct FlaMemory * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__flamemMap__)(struct FlaMemory * /*this*/, CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, RsCpuMapping *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__flamemUnmap__)(struct FlaMemory * /*this*/, CALL_CONTEXT *, RsCpuMapping *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__flamemGetMemInterMapParams__)(struct FlaMemory * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__flamemCheckMemInterUnmap__)(struct FlaMemory * /*this*/, NvBool);  // inline virtual inherited (mem) base (mem) body
    NV_STATUS (*__flamemGetMemoryMappingDescriptor__)(struct FlaMemory * /*this*/, MEMORY_DESCRIPTOR **);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__flamemCheckCopyPermissions__)(struct FlaMemory * /*this*/, struct OBJGPU *, struct Device *);  // inline virtual inherited (mem) base (mem) body
    NV_STATUS (*__flamemIsReady__)(struct FlaMemory * /*this*/, NvBool);  // virtual inherited (mem) base (mem)
    NvBool (*__flamemIsGpuMapAllowed__)(struct FlaMemory * /*this*/, struct OBJGPU *);  // inline virtual inherited (mem) base (mem) body
    NvBool (*__flamemIsExportAllowed__)(struct FlaMemory * /*this*/);  // inline virtual inherited (mem) base (mem) body
    NvBool (*__flamemAccessCallback__)(struct FlaMemory * /*this*/, RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (mem)
    NvBool (*__flamemShareCallback__)(struct FlaMemory * /*this*/, RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (rmres) base (mem)
    NV_STATUS (*__flamemControlSerialization_Prologue__)(struct FlaMemory * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (mem)
    void (*__flamemControlSerialization_Epilogue__)(struct FlaMemory * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (mem)
    NV_STATUS (*__flamemControl_Prologue__)(struct FlaMemory * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (mem)
    void (*__flamemControl_Epilogue__)(struct FlaMemory * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (mem)
    void (*__flamemPreDestruct__)(struct FlaMemory * /*this*/);  // virtual inherited (res) base (mem)
    NV_STATUS (*__flamemControlFilter__)(struct FlaMemory * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (mem)
    NvBool (*__flamemIsPartialUnmapSupported__)(struct FlaMemory * /*this*/);  // inline virtual inherited (res) base (mem) body
    NV_STATUS (*__flamemMapTo__)(struct FlaMemory * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (mem)
    NV_STATUS (*__flamemUnmapFrom__)(struct FlaMemory * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (mem)
    NvU32 (*__flamemGetRefCount__)(struct FlaMemory * /*this*/);  // virtual inherited (res) base (mem)
    void (*__flamemAddAdditionalDependants__)(struct RsClient *, struct FlaMemory * /*this*/, RsResourceRef *);  // virtual inherited (res) base (mem)

    // Data members
    NvU32 peerDeviceInst;
    NvU32 peerGpuInst;
    NvHandle hDupedExportMemory;
};

#ifndef __NVOC_CLASS_FlaMemory_TYPEDEF__
#define __NVOC_CLASS_FlaMemory_TYPEDEF__
typedef struct FlaMemory FlaMemory;
#endif /* __NVOC_CLASS_FlaMemory_TYPEDEF__ */

#ifndef __nvoc_class_id_FlaMemory
#define __nvoc_class_id_FlaMemory 0xe61ee1
#endif /* __nvoc_class_id_FlaMemory */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_FlaMemory;

#define __staticCast_FlaMemory(pThis) \
    ((pThis)->__nvoc_pbase_FlaMemory)

#ifdef __nvoc_fla_mem_h_disabled
#define __dynamicCast_FlaMemory(pThis) ((FlaMemory*)NULL)
#else //__nvoc_fla_mem_h_disabled
#define __dynamicCast_FlaMemory(pThis) \
    ((FlaMemory*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(FlaMemory)))
#endif //__nvoc_fla_mem_h_disabled

NV_STATUS __nvoc_objCreateDynamic_FlaMemory(FlaMemory**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_FlaMemory(FlaMemory**, Dynamic*, NvU32, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_FlaMemory(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_FlaMemory((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define flamemCanCopy_FNPTR(pFlaMemory) pFlaMemory->__flamemCanCopy__
#define flamemCanCopy(pFlaMemory) flamemCanCopy_DISPATCH(pFlaMemory)
#define flamemCopyConstruct_FNPTR(pFlaMemory) pFlaMemory->__flamemCopyConstruct__
#define flamemCopyConstruct(pFlaMemory, pCallContext, pParams) flamemCopyConstruct_DISPATCH(pFlaMemory, pCallContext, pParams)
#define flamemIsDuplicate_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memIsDuplicate__
#define flamemIsDuplicate(pMemory, hMemory, pDuplicate) flamemIsDuplicate_DISPATCH(pMemory, hMemory, pDuplicate)
#define flamemGetMapAddrSpace_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memGetMapAddrSpace__
#define flamemGetMapAddrSpace(pMemory, pCallContext, mapFlags, pAddrSpace) flamemGetMapAddrSpace_DISPATCH(pMemory, pCallContext, mapFlags, pAddrSpace)
#define flamemControl_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memControl__
#define flamemControl(pMemory, pCallContext, pParams) flamemControl_DISPATCH(pMemory, pCallContext, pParams)
#define flamemMap_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memMap__
#define flamemMap(pMemory, pCallContext, pParams, pCpuMapping) flamemMap_DISPATCH(pMemory, pCallContext, pParams, pCpuMapping)
#define flamemUnmap_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memUnmap__
#define flamemUnmap(pMemory, pCallContext, pCpuMapping) flamemUnmap_DISPATCH(pMemory, pCallContext, pCpuMapping)
#define flamemGetMemInterMapParams_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memGetMemInterMapParams__
#define flamemGetMemInterMapParams(pMemory, pParams) flamemGetMemInterMapParams_DISPATCH(pMemory, pParams)
#define flamemCheckMemInterUnmap_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memCheckMemInterUnmap__
#define flamemCheckMemInterUnmap(pMemory, bSubdeviceHandleProvided) flamemCheckMemInterUnmap_DISPATCH(pMemory, bSubdeviceHandleProvided)
#define flamemGetMemoryMappingDescriptor_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memGetMemoryMappingDescriptor__
#define flamemGetMemoryMappingDescriptor(pMemory, ppMemDesc) flamemGetMemoryMappingDescriptor_DISPATCH(pMemory, ppMemDesc)
#define flamemCheckCopyPermissions_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memCheckCopyPermissions__
#define flamemCheckCopyPermissions(pMemory, pDstGpu, pDstDevice) flamemCheckCopyPermissions_DISPATCH(pMemory, pDstGpu, pDstDevice)
#define flamemIsReady_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memIsReady__
#define flamemIsReady(pMemory, bCopyConstructorContext) flamemIsReady_DISPATCH(pMemory, bCopyConstructorContext)
#define flamemIsGpuMapAllowed_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memIsGpuMapAllowed__
#define flamemIsGpuMapAllowed(pMemory, pGpu) flamemIsGpuMapAllowed_DISPATCH(pMemory, pGpu)
#define flamemIsExportAllowed_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memIsExportAllowed__
#define flamemIsExportAllowed(pMemory) flamemIsExportAllowed_DISPATCH(pMemory)
#define flamemAccessCallback_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__rmresAccessCallback__
#define flamemAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) flamemAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define flamemShareCallback_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__rmresShareCallback__
#define flamemShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) flamemShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define flamemControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__rmresControlSerialization_Prologue__
#define flamemControlSerialization_Prologue(pResource, pCallContext, pParams) flamemControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define flamemControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__rmresControlSerialization_Epilogue__
#define flamemControlSerialization_Epilogue(pResource, pCallContext, pParams) flamemControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define flamemControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__rmresControl_Prologue__
#define flamemControl_Prologue(pResource, pCallContext, pParams) flamemControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define flamemControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__rmresControl_Epilogue__
#define flamemControl_Epilogue(pResource, pCallContext, pParams) flamemControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define flamemPreDestruct_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resPreDestruct__
#define flamemPreDestruct(pResource) flamemPreDestruct_DISPATCH(pResource)
#define flamemControlFilter_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resControlFilter__
#define flamemControlFilter(pResource, pCallContext, pParams) flamemControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define flamemIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsPartialUnmapSupported__
#define flamemIsPartialUnmapSupported(pResource) flamemIsPartialUnmapSupported_DISPATCH(pResource)
#define flamemMapTo_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resMapTo__
#define flamemMapTo(pResource, pParams) flamemMapTo_DISPATCH(pResource, pParams)
#define flamemUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmapFrom__
#define flamemUnmapFrom(pResource, pParams) flamemUnmapFrom_DISPATCH(pResource, pParams)
#define flamemGetRefCount_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resGetRefCount__
#define flamemGetRefCount(pResource) flamemGetRefCount_DISPATCH(pResource)
#define flamemAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resAddAdditionalDependants__
#define flamemAddAdditionalDependants(pClient, pResource, pReference) flamemAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NvBool flamemCanCopy_DISPATCH(struct FlaMemory *pFlaMemory) {
    return pFlaMemory->__flamemCanCopy__(pFlaMemory);
}

static inline NV_STATUS flamemCopyConstruct_DISPATCH(struct FlaMemory *pFlaMemory, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {
    return pFlaMemory->__flamemCopyConstruct__(pFlaMemory, pCallContext, pParams);
}

static inline NV_STATUS flamemIsDuplicate_DISPATCH(struct FlaMemory *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return pMemory->__flamemIsDuplicate__(pMemory, hMemory, pDuplicate);
}

static inline NV_STATUS flamemGetMapAddrSpace_DISPATCH(struct FlaMemory *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pMemory->__flamemGetMapAddrSpace__(pMemory, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS flamemControl_DISPATCH(struct FlaMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pMemory->__flamemControl__(pMemory, pCallContext, pParams);
}

static inline NV_STATUS flamemMap_DISPATCH(struct FlaMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pMemory->__flamemMap__(pMemory, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS flamemUnmap_DISPATCH(struct FlaMemory *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pMemory->__flamemUnmap__(pMemory, pCallContext, pCpuMapping);
}

static inline NV_STATUS flamemGetMemInterMapParams_DISPATCH(struct FlaMemory *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pMemory->__flamemGetMemInterMapParams__(pMemory, pParams);
}

static inline NV_STATUS flamemCheckMemInterUnmap_DISPATCH(struct FlaMemory *pMemory, NvBool bSubdeviceHandleProvided) {
    return pMemory->__flamemCheckMemInterUnmap__(pMemory, bSubdeviceHandleProvided);
}

static inline NV_STATUS flamemGetMemoryMappingDescriptor_DISPATCH(struct FlaMemory *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return pMemory->__flamemGetMemoryMappingDescriptor__(pMemory, ppMemDesc);
}

static inline NV_STATUS flamemCheckCopyPermissions_DISPATCH(struct FlaMemory *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return pMemory->__flamemCheckCopyPermissions__(pMemory, pDstGpu, pDstDevice);
}

static inline NV_STATUS flamemIsReady_DISPATCH(struct FlaMemory *pMemory, NvBool bCopyConstructorContext) {
    return pMemory->__flamemIsReady__(pMemory, bCopyConstructorContext);
}

static inline NvBool flamemIsGpuMapAllowed_DISPATCH(struct FlaMemory *pMemory, struct OBJGPU *pGpu) {
    return pMemory->__flamemIsGpuMapAllowed__(pMemory, pGpu);
}

static inline NvBool flamemIsExportAllowed_DISPATCH(struct FlaMemory *pMemory) {
    return pMemory->__flamemIsExportAllowed__(pMemory);
}

static inline NvBool flamemAccessCallback_DISPATCH(struct FlaMemory *pResource, RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__flamemAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool flamemShareCallback_DISPATCH(struct FlaMemory *pResource, RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__flamemShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS flamemControlSerialization_Prologue_DISPATCH(struct FlaMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__flamemControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void flamemControlSerialization_Epilogue_DISPATCH(struct FlaMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__flamemControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS flamemControl_Prologue_DISPATCH(struct FlaMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__flamemControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void flamemControl_Epilogue_DISPATCH(struct FlaMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__flamemControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline void flamemPreDestruct_DISPATCH(struct FlaMemory *pResource) {
    pResource->__flamemPreDestruct__(pResource);
}

static inline NV_STATUS flamemControlFilter_DISPATCH(struct FlaMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__flamemControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool flamemIsPartialUnmapSupported_DISPATCH(struct FlaMemory *pResource) {
    return pResource->__flamemIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS flamemMapTo_DISPATCH(struct FlaMemory *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__flamemMapTo__(pResource, pParams);
}

static inline NV_STATUS flamemUnmapFrom_DISPATCH(struct FlaMemory *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__flamemUnmapFrom__(pResource, pParams);
}

static inline NvU32 flamemGetRefCount_DISPATCH(struct FlaMemory *pResource) {
    return pResource->__flamemGetRefCount__(pResource);
}

static inline void flamemAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct FlaMemory *pResource, RsResourceRef *pReference) {
    pResource->__flamemAddAdditionalDependants__(pClient, pResource, pReference);
}

NvBool flamemCanCopy_IMPL(struct FlaMemory *pFlaMemory);

NV_STATUS flamemCopyConstruct_IMPL(struct FlaMemory *pFlaMemory, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);

NV_STATUS flamemConstruct_IMPL(struct FlaMemory *arg_pFlaMemory, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_flamemConstruct(arg_pFlaMemory, arg_pCallContext, arg_pParams) flamemConstruct_IMPL(arg_pFlaMemory, arg_pCallContext, arg_pParams)
void flamemDestruct_IMPL(struct FlaMemory *pFlaMemory);

#define __nvoc_flamemDestruct(pFlaMemory) flamemDestruct_IMPL(pFlaMemory)
#undef PRIVATE_FIELD


#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_FLA_MEM_NVOC_H_
