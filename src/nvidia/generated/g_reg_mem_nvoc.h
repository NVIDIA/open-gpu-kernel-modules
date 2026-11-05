
#ifndef _G_REG_MEM_NVOC_H_
#define _G_REG_MEM_NVOC_H_

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


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__RegisterMemory;
struct NVOC_METADATA__Memory;


struct RegisterMemory {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__RegisterMemory *__nvoc_metadata_ptr;
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
    struct RegisterMemory *__nvoc_pbase_RegisterMemory;    // regmem
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__RegisterMemory {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__Memory metadata__Memory;
};

#ifndef __nvoc_class_id_RegisterMemory
#define __nvoc_class_id_RegisterMemory 0x40d457u
typedef struct RegisterMemory RegisterMemory;
#endif /* __nvoc_class_id_RegisterMemory */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RegisterMemory;

#define __staticCast_RegisterMemory(pThis) \
    ((pThis)->__nvoc_pbase_RegisterMemory)

#ifdef __nvoc_reg_mem_h_disabled
#define __dynamicCast_RegisterMemory(pThis) ((RegisterMemory*) NULL)
#else //__nvoc_reg_mem_h_disabled
#define __dynamicCast_RegisterMemory(pThis) \
    ((RegisterMemory*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(RegisterMemory)))
#endif //__nvoc_reg_mem_h_disabled

NV_STATUS __nvoc_objCreateDynamic_RegisterMemory(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_RegisterMemory(RegisterMemory**, Dynamic*, NvU32, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __objCreate_RegisterMemory(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams) \
    __nvoc_objCreate_RegisterMemory((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pCallContext, pParams)


// Wrapper macros for implementation functions
NV_STATUS regmemConstruct_IMPL(struct RegisterMemory *pRegisterMemory, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __nvoc_regmemConstruct(pRegisterMemory, pCallContext, pParams) regmemConstruct_IMPL(pRegisterMemory, pCallContext, pParams)


// Wrapper macros for halified functions
#define regmemCanCopy_FNPTR(pRegisterMemory) pRegisterMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define regmemCanCopy(pRegisterMemory) regmemCanCopy_DISPATCH(pRegisterMemory)
#define regmemIsDuplicate_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define regmemIsDuplicate(pMemory, hMemory, pDuplicate) regmemIsDuplicate_DISPATCH(pMemory, hMemory, pDuplicate)
#define regmemGetMapAddrSpace_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memGetMapAddrSpace__
#define regmemGetMapAddrSpace(pMemory, pCallContext, mapFlags, pAddrSpace) regmemGetMapAddrSpace_DISPATCH(pMemory, pCallContext, mapFlags, pAddrSpace)
#define regmemControl_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__
#define regmemControl(pMemory, pCallContext, pParams) regmemControl_DISPATCH(pMemory, pCallContext, pParams)
#define regmemMap_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__
#define regmemMap(pMemory, pCallContext, pParams, pCpuMapping) regmemMap_DISPATCH(pMemory, pCallContext, pParams, pCpuMapping)
#define regmemUnmap_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__
#define regmemUnmap(pMemory, pCallContext, pCpuMapping) regmemUnmap_DISPATCH(pMemory, pCallContext, pCpuMapping)
#define regmemGetMemInterMapParams_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define regmemGetMemInterMapParams(pMemory, pParams) regmemGetMemInterMapParams_DISPATCH(pMemory, pParams)
#define regmemCheckMemInterUnmap_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define regmemCheckMemInterUnmap(pMemory, bSubdeviceHandleProvided) regmemCheckMemInterUnmap_DISPATCH(pMemory, bSubdeviceHandleProvided)
#define regmemGetMemoryMappingDescriptor_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define regmemGetMemoryMappingDescriptor(pMemory, ppMemDesc) regmemGetMemoryMappingDescriptor_DISPATCH(pMemory, ppMemDesc)
#define regmemCheckCopyPermissions_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memCheckCopyPermissions__
#define regmemCheckCopyPermissions(pMemory, pDstGpu, pDstDevice) regmemCheckCopyPermissions_DISPATCH(pMemory, pDstGpu, pDstDevice)
#define regmemIsReady_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsReady__
#define regmemIsReady(pMemory, bCopyConstructorContext) regmemIsReady_DISPATCH(pMemory, bCopyConstructorContext)
#define regmemIsGpuMapAllowed_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsGpuMapAllowed__
#define regmemIsGpuMapAllowed(pMemory, pGpu) regmemIsGpuMapAllowed_DISPATCH(pMemory, pGpu)
#define regmemIsExportAllowed_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsExportAllowed__
#define regmemIsExportAllowed(pMemory) regmemIsExportAllowed_DISPATCH(pMemory)
#define regmemAccessCallback_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__
#define regmemAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) regmemAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define regmemShareCallback_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__
#define regmemShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) regmemShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define regmemControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__
#define regmemControlSerialization_Prologue(pResource, pCallContext, pParams) regmemControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define regmemControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__
#define regmemControlSerialization_Epilogue(pResource, pCallContext, pParams) regmemControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define regmemControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__
#define regmemControl_Prologue(pResource, pCallContext, pParams) regmemControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define regmemControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__
#define regmemControl_Epilogue(pResource, pCallContext, pParams) regmemControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define regmemPreDestruct_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define regmemPreDestruct(pResource) regmemPreDestruct_DISPATCH(pResource)
#define regmemControlFilter_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define regmemControlFilter(pResource, pCallContext, pParams) regmemControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define regmemIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define regmemIsPartialUnmapSupported(pResource) regmemIsPartialUnmapSupported_DISPATCH(pResource)
#define regmemMapTo_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define regmemMapTo(pResource, pParams) regmemMapTo_DISPATCH(pResource, pParams)
#define regmemUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define regmemUnmapFrom(pResource, pParams) regmemUnmapFrom_DISPATCH(pResource, pParams)
#define regmemGetRefCount_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define regmemGetRefCount(pResource) regmemGetRefCount_DISPATCH(pResource)
#define regmemAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define regmemAddAdditionalDependants(pClient, pResource, pReference) regmemAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NvBool regmemCanCopy_DISPATCH(struct RegisterMemory *pRegisterMemory) {
    return pRegisterMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__(staticCast(pRegisterMemory, RsResource));
}

static inline NV_STATUS regmemIsDuplicate_DISPATCH(struct RegisterMemory *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__(staticCast(pMemory, RsResource), hMemory, pDuplicate);
}

static inline NV_STATUS regmemGetMapAddrSpace_DISPATCH(struct RegisterMemory *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memGetMapAddrSpace__(staticCast(pMemory, Memory), pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS regmemControl_DISPATCH(struct RegisterMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__(staticCast(pMemory, RsResource), pCallContext, pParams);
}

static inline NV_STATUS regmemMap_DISPATCH(struct RegisterMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__(staticCast(pMemory, RsResource), pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS regmemUnmap_DISPATCH(struct RegisterMemory *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__(staticCast(pMemory, RsResource), pCallContext, pCpuMapping);
}

static inline NV_STATUS regmemGetMemInterMapParams_DISPATCH(struct RegisterMemory *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__(staticCast(pMemory, RmResource), pParams);
}

static inline NV_STATUS regmemCheckMemInterUnmap_DISPATCH(struct RegisterMemory *pMemory, NvBool bSubdeviceHandleProvided) {
    return pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__(staticCast(pMemory, RmResource), bSubdeviceHandleProvided);
}

static inline NV_STATUS regmemGetMemoryMappingDescriptor_DISPATCH(struct RegisterMemory *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__(staticCast(pMemory, RmResource), ppMemDesc);
}

static inline NV_STATUS regmemCheckCopyPermissions_DISPATCH(struct RegisterMemory *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memCheckCopyPermissions__(staticCast(pMemory, Memory), pDstGpu, pDstDevice);
}

static inline NV_STATUS regmemIsReady_DISPATCH(struct RegisterMemory *pMemory, NvBool bCopyConstructorContext) {
    return pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsReady__(staticCast(pMemory, Memory), bCopyConstructorContext);
}

static inline NvBool regmemIsGpuMapAllowed_DISPATCH(struct RegisterMemory *pMemory, struct OBJGPU *pGpu) {
    return pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsGpuMapAllowed__(staticCast(pMemory, Memory), pGpu);
}

static inline NvBool regmemIsExportAllowed_DISPATCH(struct RegisterMemory *pMemory) {
    return pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsExportAllowed__(staticCast(pMemory, Memory));
}

static inline NvBool regmemAccessCallback_DISPATCH(struct RegisterMemory *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__(staticCast(pResource, RsResource), pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool regmemShareCallback_DISPATCH(struct RegisterMemory *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__(staticCast(pResource, RsResource), pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS regmemControlSerialization_Prologue_DISPATCH(struct RegisterMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void regmemControlSerialization_Epilogue_DISPATCH(struct RegisterMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS regmemControl_Prologue_DISPATCH(struct RegisterMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void regmemControl_Epilogue_DISPATCH(struct RegisterMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void regmemPreDestruct_DISPATCH(struct RegisterMemory *pResource) {
    pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__(staticCast(pResource, RsResource));
}

static inline NV_STATUS regmemControlFilter_DISPATCH(struct RegisterMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool regmemIsPartialUnmapSupported_DISPATCH(struct RegisterMemory *pResource) {
    return pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__(staticCast(pResource, RsResource));
}

static inline NV_STATUS regmemMapTo_DISPATCH(struct RegisterMemory *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__(staticCast(pResource, RsResource), pParams);
}

static inline NV_STATUS regmemUnmapFrom_DISPATCH(struct RegisterMemory *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__(staticCast(pResource, RsResource), pParams);
}

static inline NvU32 regmemGetRefCount_DISPATCH(struct RegisterMemory *pResource) {
    return pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__(staticCast(pResource, RsResource));
}

static inline void regmemAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct RegisterMemory *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__(pClient, staticCast(pResource, RsResource), pReference);
}

// Virtual method declarations and/or inline definitions
NvBool regmemCanCopy_IMPL(struct RegisterMemory *pRegisterMemory);

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

#endif // _G_REG_MEM_NVOC_H_
