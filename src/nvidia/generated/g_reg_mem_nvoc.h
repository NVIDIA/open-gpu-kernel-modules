
#ifndef _G_REG_MEM_NVOC_H_
#define _G_REG_MEM_NVOC_H_
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
 * SPDX-FileCopyrightText: Copyright (c) 2018-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_reg_mem_nvoc.h"

#ifndef _REGISTER_MEMORY_H_
#define _REGISTER_MEMORY_H_

#include "mem_mgr/mem.h"

/*!
 * Memory allocation representing GPU register space
 *
 * For security and maintenance reasons we want to phase this class out.
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_REG_MEM_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct RegisterMemory {

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
    struct RegisterMemory *__nvoc_pbase_RegisterMemory;    // regmem

    // Vtable with 26 per-object function pointers
    NvBool (*__regmemCanCopy__)(struct RegisterMemory * /*this*/);  // virtual override (res) base (mem)
    NV_STATUS (*__regmemIsDuplicate__)(struct RegisterMemory * /*this*/, NvHandle, NvBool *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__regmemGetMapAddrSpace__)(struct RegisterMemory * /*this*/, CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__regmemControl__)(struct RegisterMemory * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__regmemMap__)(struct RegisterMemory * /*this*/, CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, RsCpuMapping *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__regmemUnmap__)(struct RegisterMemory * /*this*/, CALL_CONTEXT *, RsCpuMapping *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__regmemGetMemInterMapParams__)(struct RegisterMemory * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__regmemCheckMemInterUnmap__)(struct RegisterMemory * /*this*/, NvBool);  // inline virtual inherited (mem) base (mem) body
    NV_STATUS (*__regmemGetMemoryMappingDescriptor__)(struct RegisterMemory * /*this*/, MEMORY_DESCRIPTOR **);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__regmemCheckCopyPermissions__)(struct RegisterMemory * /*this*/, struct OBJGPU *, struct Device *);  // inline virtual inherited (mem) base (mem) body
    NV_STATUS (*__regmemIsReady__)(struct RegisterMemory * /*this*/, NvBool);  // virtual inherited (mem) base (mem)
    NvBool (*__regmemIsGpuMapAllowed__)(struct RegisterMemory * /*this*/, struct OBJGPU *);  // inline virtual inherited (mem) base (mem) body
    NvBool (*__regmemIsExportAllowed__)(struct RegisterMemory * /*this*/);  // inline virtual inherited (mem) base (mem) body
    NvBool (*__regmemAccessCallback__)(struct RegisterMemory * /*this*/, RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (mem)
    NvBool (*__regmemShareCallback__)(struct RegisterMemory * /*this*/, RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (rmres) base (mem)
    NV_STATUS (*__regmemControlSerialization_Prologue__)(struct RegisterMemory * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (mem)
    void (*__regmemControlSerialization_Epilogue__)(struct RegisterMemory * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (mem)
    NV_STATUS (*__regmemControl_Prologue__)(struct RegisterMemory * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (mem)
    void (*__regmemControl_Epilogue__)(struct RegisterMemory * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (mem)
    void (*__regmemPreDestruct__)(struct RegisterMemory * /*this*/);  // virtual inherited (res) base (mem)
    NV_STATUS (*__regmemControlFilter__)(struct RegisterMemory * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (mem)
    NvBool (*__regmemIsPartialUnmapSupported__)(struct RegisterMemory * /*this*/);  // inline virtual inherited (res) base (mem) body
    NV_STATUS (*__regmemMapTo__)(struct RegisterMemory * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (mem)
    NV_STATUS (*__regmemUnmapFrom__)(struct RegisterMemory * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (mem)
    NvU32 (*__regmemGetRefCount__)(struct RegisterMemory * /*this*/);  // virtual inherited (res) base (mem)
    void (*__regmemAddAdditionalDependants__)(struct RsClient *, struct RegisterMemory * /*this*/, RsResourceRef *);  // virtual inherited (res) base (mem)
};

#ifndef __NVOC_CLASS_RegisterMemory_TYPEDEF__
#define __NVOC_CLASS_RegisterMemory_TYPEDEF__
typedef struct RegisterMemory RegisterMemory;
#endif /* __NVOC_CLASS_RegisterMemory_TYPEDEF__ */

#ifndef __nvoc_class_id_RegisterMemory
#define __nvoc_class_id_RegisterMemory 0x40d457
#endif /* __nvoc_class_id_RegisterMemory */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RegisterMemory;

#define __staticCast_RegisterMemory(pThis) \
    ((pThis)->__nvoc_pbase_RegisterMemory)

#ifdef __nvoc_reg_mem_h_disabled
#define __dynamicCast_RegisterMemory(pThis) ((RegisterMemory*)NULL)
#else //__nvoc_reg_mem_h_disabled
#define __dynamicCast_RegisterMemory(pThis) \
    ((RegisterMemory*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(RegisterMemory)))
#endif //__nvoc_reg_mem_h_disabled

NV_STATUS __nvoc_objCreateDynamic_RegisterMemory(RegisterMemory**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_RegisterMemory(RegisterMemory**, Dynamic*, NvU32, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_RegisterMemory(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_RegisterMemory((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define regmemCanCopy_FNPTR(pRegisterMemory) pRegisterMemory->__regmemCanCopy__
#define regmemCanCopy(pRegisterMemory) regmemCanCopy_DISPATCH(pRegisterMemory)
#define regmemIsDuplicate_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memIsDuplicate__
#define regmemIsDuplicate(pMemory, hMemory, pDuplicate) regmemIsDuplicate_DISPATCH(pMemory, hMemory, pDuplicate)
#define regmemGetMapAddrSpace_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memGetMapAddrSpace__
#define regmemGetMapAddrSpace(pMemory, pCallContext, mapFlags, pAddrSpace) regmemGetMapAddrSpace_DISPATCH(pMemory, pCallContext, mapFlags, pAddrSpace)
#define regmemControl_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memControl__
#define regmemControl(pMemory, pCallContext, pParams) regmemControl_DISPATCH(pMemory, pCallContext, pParams)
#define regmemMap_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memMap__
#define regmemMap(pMemory, pCallContext, pParams, pCpuMapping) regmemMap_DISPATCH(pMemory, pCallContext, pParams, pCpuMapping)
#define regmemUnmap_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memUnmap__
#define regmemUnmap(pMemory, pCallContext, pCpuMapping) regmemUnmap_DISPATCH(pMemory, pCallContext, pCpuMapping)
#define regmemGetMemInterMapParams_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memGetMemInterMapParams__
#define regmemGetMemInterMapParams(pMemory, pParams) regmemGetMemInterMapParams_DISPATCH(pMemory, pParams)
#define regmemCheckMemInterUnmap_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memCheckMemInterUnmap__
#define regmemCheckMemInterUnmap(pMemory, bSubdeviceHandleProvided) regmemCheckMemInterUnmap_DISPATCH(pMemory, bSubdeviceHandleProvided)
#define regmemGetMemoryMappingDescriptor_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memGetMemoryMappingDescriptor__
#define regmemGetMemoryMappingDescriptor(pMemory, ppMemDesc) regmemGetMemoryMappingDescriptor_DISPATCH(pMemory, ppMemDesc)
#define regmemCheckCopyPermissions_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memCheckCopyPermissions__
#define regmemCheckCopyPermissions(pMemory, pDstGpu, pDstDevice) regmemCheckCopyPermissions_DISPATCH(pMemory, pDstGpu, pDstDevice)
#define regmemIsReady_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memIsReady__
#define regmemIsReady(pMemory, bCopyConstructorContext) regmemIsReady_DISPATCH(pMemory, bCopyConstructorContext)
#define regmemIsGpuMapAllowed_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memIsGpuMapAllowed__
#define regmemIsGpuMapAllowed(pMemory, pGpu) regmemIsGpuMapAllowed_DISPATCH(pMemory, pGpu)
#define regmemIsExportAllowed_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memIsExportAllowed__
#define regmemIsExportAllowed(pMemory) regmemIsExportAllowed_DISPATCH(pMemory)
#define regmemAccessCallback_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__rmresAccessCallback__
#define regmemAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) regmemAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define regmemShareCallback_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__rmresShareCallback__
#define regmemShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) regmemShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define regmemControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__rmresControlSerialization_Prologue__
#define regmemControlSerialization_Prologue(pResource, pCallContext, pParams) regmemControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define regmemControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__rmresControlSerialization_Epilogue__
#define regmemControlSerialization_Epilogue(pResource, pCallContext, pParams) regmemControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define regmemControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__rmresControl_Prologue__
#define regmemControl_Prologue(pResource, pCallContext, pParams) regmemControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define regmemControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__rmresControl_Epilogue__
#define regmemControl_Epilogue(pResource, pCallContext, pParams) regmemControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define regmemPreDestruct_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resPreDestruct__
#define regmemPreDestruct(pResource) regmemPreDestruct_DISPATCH(pResource)
#define regmemControlFilter_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resControlFilter__
#define regmemControlFilter(pResource, pCallContext, pParams) regmemControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define regmemIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsPartialUnmapSupported__
#define regmemIsPartialUnmapSupported(pResource) regmemIsPartialUnmapSupported_DISPATCH(pResource)
#define regmemMapTo_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resMapTo__
#define regmemMapTo(pResource, pParams) regmemMapTo_DISPATCH(pResource, pParams)
#define regmemUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmapFrom__
#define regmemUnmapFrom(pResource, pParams) regmemUnmapFrom_DISPATCH(pResource, pParams)
#define regmemGetRefCount_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resGetRefCount__
#define regmemGetRefCount(pResource) regmemGetRefCount_DISPATCH(pResource)
#define regmemAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resAddAdditionalDependants__
#define regmemAddAdditionalDependants(pClient, pResource, pReference) regmemAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NvBool regmemCanCopy_DISPATCH(struct RegisterMemory *pRegisterMemory) {
    return pRegisterMemory->__regmemCanCopy__(pRegisterMemory);
}

static inline NV_STATUS regmemIsDuplicate_DISPATCH(struct RegisterMemory *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return pMemory->__regmemIsDuplicate__(pMemory, hMemory, pDuplicate);
}

static inline NV_STATUS regmemGetMapAddrSpace_DISPATCH(struct RegisterMemory *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pMemory->__regmemGetMapAddrSpace__(pMemory, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS regmemControl_DISPATCH(struct RegisterMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pMemory->__regmemControl__(pMemory, pCallContext, pParams);
}

static inline NV_STATUS regmemMap_DISPATCH(struct RegisterMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pMemory->__regmemMap__(pMemory, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS regmemUnmap_DISPATCH(struct RegisterMemory *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pMemory->__regmemUnmap__(pMemory, pCallContext, pCpuMapping);
}

static inline NV_STATUS regmemGetMemInterMapParams_DISPATCH(struct RegisterMemory *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pMemory->__regmemGetMemInterMapParams__(pMemory, pParams);
}

static inline NV_STATUS regmemCheckMemInterUnmap_DISPATCH(struct RegisterMemory *pMemory, NvBool bSubdeviceHandleProvided) {
    return pMemory->__regmemCheckMemInterUnmap__(pMemory, bSubdeviceHandleProvided);
}

static inline NV_STATUS regmemGetMemoryMappingDescriptor_DISPATCH(struct RegisterMemory *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return pMemory->__regmemGetMemoryMappingDescriptor__(pMemory, ppMemDesc);
}

static inline NV_STATUS regmemCheckCopyPermissions_DISPATCH(struct RegisterMemory *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return pMemory->__regmemCheckCopyPermissions__(pMemory, pDstGpu, pDstDevice);
}

static inline NV_STATUS regmemIsReady_DISPATCH(struct RegisterMemory *pMemory, NvBool bCopyConstructorContext) {
    return pMemory->__regmemIsReady__(pMemory, bCopyConstructorContext);
}

static inline NvBool regmemIsGpuMapAllowed_DISPATCH(struct RegisterMemory *pMemory, struct OBJGPU *pGpu) {
    return pMemory->__regmemIsGpuMapAllowed__(pMemory, pGpu);
}

static inline NvBool regmemIsExportAllowed_DISPATCH(struct RegisterMemory *pMemory) {
    return pMemory->__regmemIsExportAllowed__(pMemory);
}

static inline NvBool regmemAccessCallback_DISPATCH(struct RegisterMemory *pResource, RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__regmemAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool regmemShareCallback_DISPATCH(struct RegisterMemory *pResource, RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__regmemShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS regmemControlSerialization_Prologue_DISPATCH(struct RegisterMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__regmemControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void regmemControlSerialization_Epilogue_DISPATCH(struct RegisterMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__regmemControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS regmemControl_Prologue_DISPATCH(struct RegisterMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__regmemControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void regmemControl_Epilogue_DISPATCH(struct RegisterMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__regmemControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline void regmemPreDestruct_DISPATCH(struct RegisterMemory *pResource) {
    pResource->__regmemPreDestruct__(pResource);
}

static inline NV_STATUS regmemControlFilter_DISPATCH(struct RegisterMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__regmemControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool regmemIsPartialUnmapSupported_DISPATCH(struct RegisterMemory *pResource) {
    return pResource->__regmemIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS regmemMapTo_DISPATCH(struct RegisterMemory *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__regmemMapTo__(pResource, pParams);
}

static inline NV_STATUS regmemUnmapFrom_DISPATCH(struct RegisterMemory *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__regmemUnmapFrom__(pResource, pParams);
}

static inline NvU32 regmemGetRefCount_DISPATCH(struct RegisterMemory *pResource) {
    return pResource->__regmemGetRefCount__(pResource);
}

static inline void regmemAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct RegisterMemory *pResource, RsResourceRef *pReference) {
    pResource->__regmemAddAdditionalDependants__(pClient, pResource, pReference);
}

NvBool regmemCanCopy_IMPL(struct RegisterMemory *pRegisterMemory);

NV_STATUS regmemConstruct_IMPL(struct RegisterMemory *arg_pRegisterMemory, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_regmemConstruct(arg_pRegisterMemory, arg_pCallContext, arg_pParams) regmemConstruct_IMPL(arg_pRegisterMemory, arg_pCallContext, arg_pParams)
#undef PRIVATE_FIELD


#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_REG_MEM_NVOC_H_
