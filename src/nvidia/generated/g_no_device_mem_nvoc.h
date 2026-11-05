
#ifndef _G_NO_DEVICE_MEM_NVOC_H_
#define _G_NO_DEVICE_MEM_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2018 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_no_device_mem_nvoc.h"

#ifndef _NO_DEVICE_MEMORY_H_
#define _NO_DEVICE_MEMORY_H_

#include "mem_mgr/mem.h"

/**
 * This class represents contiguous system memory that is associated with a
 * client instead of a device. This object can be used for memory allocations
 * that should survive device teardown.
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_NO_DEVICE_MEM_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NoDeviceMemory;
struct NVOC_METADATA__Memory;


struct NoDeviceMemory {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__NoDeviceMemory *__nvoc_metadata_ptr;
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
    struct NoDeviceMemory *__nvoc_pbase_NoDeviceMemory;    // nodevicemem
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NoDeviceMemory {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__Memory metadata__Memory;
};

#ifndef __nvoc_class_id_NoDeviceMemory
#define __nvoc_class_id_NoDeviceMemory 0x6c0832u
typedef struct NoDeviceMemory NoDeviceMemory;
#endif /* __nvoc_class_id_NoDeviceMemory */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_NoDeviceMemory;

#define __staticCast_NoDeviceMemory(pThis) \
    ((pThis)->__nvoc_pbase_NoDeviceMemory)

#ifdef __nvoc_no_device_mem_h_disabled
#define __dynamicCast_NoDeviceMemory(pThis) ((NoDeviceMemory*) NULL)
#else //__nvoc_no_device_mem_h_disabled
#define __dynamicCast_NoDeviceMemory(pThis) \
    ((NoDeviceMemory*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(NoDeviceMemory)))
#endif //__nvoc_no_device_mem_h_disabled

NV_STATUS __nvoc_objCreateDynamic_NoDeviceMemory(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_NoDeviceMemory(NoDeviceMemory**, Dynamic*, NvU32, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __objCreate_NoDeviceMemory(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams) \
    __nvoc_objCreate_NoDeviceMemory((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pCallContext, pParams)


// Wrapper macros for implementation functions
NV_STATUS nodevicememConstruct_IMPL(struct NoDeviceMemory *pNoDeviceMemory, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __nvoc_nodevicememConstruct(pNoDeviceMemory, pCallContext, pParams) nodevicememConstruct_IMPL(pNoDeviceMemory, pCallContext, pParams)


// Wrapper macros for halified functions
#define nodevicememIsDuplicate_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define nodevicememIsDuplicate(pMemory, hMemory, pDuplicate) nodevicememIsDuplicate_DISPATCH(pMemory, hMemory, pDuplicate)
#define nodevicememGetMapAddrSpace_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memGetMapAddrSpace__
#define nodevicememGetMapAddrSpace(pMemory, pCallContext, mapFlags, pAddrSpace) nodevicememGetMapAddrSpace_DISPATCH(pMemory, pCallContext, mapFlags, pAddrSpace)
#define nodevicememControl_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__
#define nodevicememControl(pMemory, pCallContext, pParams) nodevicememControl_DISPATCH(pMemory, pCallContext, pParams)
#define nodevicememMap_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__
#define nodevicememMap(pMemory, pCallContext, pParams, pCpuMapping) nodevicememMap_DISPATCH(pMemory, pCallContext, pParams, pCpuMapping)
#define nodevicememUnmap_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__
#define nodevicememUnmap(pMemory, pCallContext, pCpuMapping) nodevicememUnmap_DISPATCH(pMemory, pCallContext, pCpuMapping)
#define nodevicememGetMemInterMapParams_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define nodevicememGetMemInterMapParams(pMemory, pParams) nodevicememGetMemInterMapParams_DISPATCH(pMemory, pParams)
#define nodevicememCheckMemInterUnmap_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define nodevicememCheckMemInterUnmap(pMemory, bSubdeviceHandleProvided) nodevicememCheckMemInterUnmap_DISPATCH(pMemory, bSubdeviceHandleProvided)
#define nodevicememGetMemoryMappingDescriptor_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define nodevicememGetMemoryMappingDescriptor(pMemory, ppMemDesc) nodevicememGetMemoryMappingDescriptor_DISPATCH(pMemory, ppMemDesc)
#define nodevicememCheckCopyPermissions_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memCheckCopyPermissions__
#define nodevicememCheckCopyPermissions(pMemory, pDstGpu, pDstDevice) nodevicememCheckCopyPermissions_DISPATCH(pMemory, pDstGpu, pDstDevice)
#define nodevicememIsReady_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsReady__
#define nodevicememIsReady(pMemory, bCopyConstructorContext) nodevicememIsReady_DISPATCH(pMemory, bCopyConstructorContext)
#define nodevicememIsGpuMapAllowed_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsGpuMapAllowed__
#define nodevicememIsGpuMapAllowed(pMemory, pGpu) nodevicememIsGpuMapAllowed_DISPATCH(pMemory, pGpu)
#define nodevicememIsExportAllowed_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsExportAllowed__
#define nodevicememIsExportAllowed(pMemory) nodevicememIsExportAllowed_DISPATCH(pMemory)
#define nodevicememAccessCallback_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__
#define nodevicememAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) nodevicememAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define nodevicememShareCallback_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__
#define nodevicememShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) nodevicememShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define nodevicememControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__
#define nodevicememControlSerialization_Prologue(pResource, pCallContext, pParams) nodevicememControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define nodevicememControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__
#define nodevicememControlSerialization_Epilogue(pResource, pCallContext, pParams) nodevicememControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define nodevicememControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__
#define nodevicememControl_Prologue(pResource, pCallContext, pParams) nodevicememControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define nodevicememControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__
#define nodevicememControl_Epilogue(pResource, pCallContext, pParams) nodevicememControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define nodevicememCanCopy_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define nodevicememCanCopy(pResource) nodevicememCanCopy_DISPATCH(pResource)
#define nodevicememPreDestruct_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define nodevicememPreDestruct(pResource) nodevicememPreDestruct_DISPATCH(pResource)
#define nodevicememControlFilter_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define nodevicememControlFilter(pResource, pCallContext, pParams) nodevicememControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define nodevicememIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define nodevicememIsPartialUnmapSupported(pResource) nodevicememIsPartialUnmapSupported_DISPATCH(pResource)
#define nodevicememMapTo_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define nodevicememMapTo(pResource, pParams) nodevicememMapTo_DISPATCH(pResource, pParams)
#define nodevicememUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define nodevicememUnmapFrom(pResource, pParams) nodevicememUnmapFrom_DISPATCH(pResource, pParams)
#define nodevicememGetRefCount_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define nodevicememGetRefCount(pResource) nodevicememGetRefCount_DISPATCH(pResource)
#define nodevicememAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define nodevicememAddAdditionalDependants(pClient, pResource, pReference) nodevicememAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NV_STATUS nodevicememIsDuplicate_DISPATCH(struct NoDeviceMemory *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__(staticCast(pMemory, RsResource), hMemory, pDuplicate);
}

static inline NV_STATUS nodevicememGetMapAddrSpace_DISPATCH(struct NoDeviceMemory *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memGetMapAddrSpace__(staticCast(pMemory, Memory), pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS nodevicememControl_DISPATCH(struct NoDeviceMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__(staticCast(pMemory, RsResource), pCallContext, pParams);
}

static inline NV_STATUS nodevicememMap_DISPATCH(struct NoDeviceMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__(staticCast(pMemory, RsResource), pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS nodevicememUnmap_DISPATCH(struct NoDeviceMemory *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__(staticCast(pMemory, RsResource), pCallContext, pCpuMapping);
}

static inline NV_STATUS nodevicememGetMemInterMapParams_DISPATCH(struct NoDeviceMemory *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__(staticCast(pMemory, RmResource), pParams);
}

static inline NV_STATUS nodevicememCheckMemInterUnmap_DISPATCH(struct NoDeviceMemory *pMemory, NvBool bSubdeviceHandleProvided) {
    return pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__(staticCast(pMemory, RmResource), bSubdeviceHandleProvided);
}

static inline NV_STATUS nodevicememGetMemoryMappingDescriptor_DISPATCH(struct NoDeviceMemory *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__(staticCast(pMemory, RmResource), ppMemDesc);
}

static inline NV_STATUS nodevicememCheckCopyPermissions_DISPATCH(struct NoDeviceMemory *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memCheckCopyPermissions__(staticCast(pMemory, Memory), pDstGpu, pDstDevice);
}

static inline NV_STATUS nodevicememIsReady_DISPATCH(struct NoDeviceMemory *pMemory, NvBool bCopyConstructorContext) {
    return pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsReady__(staticCast(pMemory, Memory), bCopyConstructorContext);
}

static inline NvBool nodevicememIsGpuMapAllowed_DISPATCH(struct NoDeviceMemory *pMemory, struct OBJGPU *pGpu) {
    return pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsGpuMapAllowed__(staticCast(pMemory, Memory), pGpu);
}

static inline NvBool nodevicememIsExportAllowed_DISPATCH(struct NoDeviceMemory *pMemory) {
    return pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsExportAllowed__(staticCast(pMemory, Memory));
}

static inline NvBool nodevicememAccessCallback_DISPATCH(struct NoDeviceMemory *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__(staticCast(pResource, RsResource), pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool nodevicememShareCallback_DISPATCH(struct NoDeviceMemory *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__(staticCast(pResource, RsResource), pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS nodevicememControlSerialization_Prologue_DISPATCH(struct NoDeviceMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void nodevicememControlSerialization_Epilogue_DISPATCH(struct NoDeviceMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS nodevicememControl_Prologue_DISPATCH(struct NoDeviceMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void nodevicememControl_Epilogue_DISPATCH(struct NoDeviceMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool nodevicememCanCopy_DISPATCH(struct NoDeviceMemory *pResource) {
    return pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__(staticCast(pResource, RsResource));
}

static inline void nodevicememPreDestruct_DISPATCH(struct NoDeviceMemory *pResource) {
    pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__(staticCast(pResource, RsResource));
}

static inline NV_STATUS nodevicememControlFilter_DISPATCH(struct NoDeviceMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool nodevicememIsPartialUnmapSupported_DISPATCH(struct NoDeviceMemory *pResource) {
    return pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__(staticCast(pResource, RsResource));
}

static inline NV_STATUS nodevicememMapTo_DISPATCH(struct NoDeviceMemory *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__(staticCast(pResource, RsResource), pParams);
}

static inline NV_STATUS nodevicememUnmapFrom_DISPATCH(struct NoDeviceMemory *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__(staticCast(pResource, RsResource), pParams);
}

static inline NvU32 nodevicememGetRefCount_DISPATCH(struct NoDeviceMemory *pResource) {
    return pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__(staticCast(pResource, RsResource));
}

static inline void nodevicememAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct NoDeviceMemory *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__(pClient, staticCast(pResource, RsResource), pReference);
}

// Virtual method declarations and/or inline definitions
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

#endif // _G_NO_DEVICE_MEM_NVOC_H_
