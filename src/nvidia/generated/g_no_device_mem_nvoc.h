
#ifndef _G_NO_DEVICE_MEM_NVOC_H_
#define _G_NO_DEVICE_MEM_NVOC_H_
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


struct NoDeviceMemory {

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
    struct NoDeviceMemory *__nvoc_pbase_NoDeviceMemory;    // nodevicemem

    // Vtable with 26 per-object function pointers
    NV_STATUS (*__nodevicememGetMapAddrSpace__)(struct NoDeviceMemory * /*this*/, CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual override (mem) base (mem)
    NV_STATUS (*__nodevicememIsDuplicate__)(struct NoDeviceMemory * /*this*/, NvHandle, NvBool *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__nodevicememControl__)(struct NoDeviceMemory * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__nodevicememMap__)(struct NoDeviceMemory * /*this*/, CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, RsCpuMapping *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__nodevicememUnmap__)(struct NoDeviceMemory * /*this*/, CALL_CONTEXT *, RsCpuMapping *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__nodevicememGetMemInterMapParams__)(struct NoDeviceMemory * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__nodevicememCheckMemInterUnmap__)(struct NoDeviceMemory * /*this*/, NvBool);  // inline virtual inherited (mem) base (mem) body
    NV_STATUS (*__nodevicememGetMemoryMappingDescriptor__)(struct NoDeviceMemory * /*this*/, MEMORY_DESCRIPTOR **);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__nodevicememCheckCopyPermissions__)(struct NoDeviceMemory * /*this*/, struct OBJGPU *, struct Device *);  // inline virtual inherited (mem) base (mem) body
    NV_STATUS (*__nodevicememIsReady__)(struct NoDeviceMemory * /*this*/, NvBool);  // virtual inherited (mem) base (mem)
    NvBool (*__nodevicememIsGpuMapAllowed__)(struct NoDeviceMemory * /*this*/, struct OBJGPU *);  // inline virtual inherited (mem) base (mem) body
    NvBool (*__nodevicememIsExportAllowed__)(struct NoDeviceMemory * /*this*/);  // inline virtual inherited (mem) base (mem) body
    NvBool (*__nodevicememAccessCallback__)(struct NoDeviceMemory * /*this*/, RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (mem)
    NvBool (*__nodevicememShareCallback__)(struct NoDeviceMemory * /*this*/, RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (rmres) base (mem)
    NV_STATUS (*__nodevicememControlSerialization_Prologue__)(struct NoDeviceMemory * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (mem)
    void (*__nodevicememControlSerialization_Epilogue__)(struct NoDeviceMemory * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (mem)
    NV_STATUS (*__nodevicememControl_Prologue__)(struct NoDeviceMemory * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (mem)
    void (*__nodevicememControl_Epilogue__)(struct NoDeviceMemory * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (mem)
    NvBool (*__nodevicememCanCopy__)(struct NoDeviceMemory * /*this*/);  // virtual inherited (res) base (mem)
    void (*__nodevicememPreDestruct__)(struct NoDeviceMemory * /*this*/);  // virtual inherited (res) base (mem)
    NV_STATUS (*__nodevicememControlFilter__)(struct NoDeviceMemory * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (mem)
    NvBool (*__nodevicememIsPartialUnmapSupported__)(struct NoDeviceMemory * /*this*/);  // inline virtual inherited (res) base (mem) body
    NV_STATUS (*__nodevicememMapTo__)(struct NoDeviceMemory * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (mem)
    NV_STATUS (*__nodevicememUnmapFrom__)(struct NoDeviceMemory * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (mem)
    NvU32 (*__nodevicememGetRefCount__)(struct NoDeviceMemory * /*this*/);  // virtual inherited (res) base (mem)
    void (*__nodevicememAddAdditionalDependants__)(struct RsClient *, struct NoDeviceMemory * /*this*/, RsResourceRef *);  // virtual inherited (res) base (mem)
};

#ifndef __NVOC_CLASS_NoDeviceMemory_TYPEDEF__
#define __NVOC_CLASS_NoDeviceMemory_TYPEDEF__
typedef struct NoDeviceMemory NoDeviceMemory;
#endif /* __NVOC_CLASS_NoDeviceMemory_TYPEDEF__ */

#ifndef __nvoc_class_id_NoDeviceMemory
#define __nvoc_class_id_NoDeviceMemory 0x6c0832
#endif /* __nvoc_class_id_NoDeviceMemory */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_NoDeviceMemory;

#define __staticCast_NoDeviceMemory(pThis) \
    ((pThis)->__nvoc_pbase_NoDeviceMemory)

#ifdef __nvoc_no_device_mem_h_disabled
#define __dynamicCast_NoDeviceMemory(pThis) ((NoDeviceMemory*)NULL)
#else //__nvoc_no_device_mem_h_disabled
#define __dynamicCast_NoDeviceMemory(pThis) \
    ((NoDeviceMemory*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(NoDeviceMemory)))
#endif //__nvoc_no_device_mem_h_disabled

NV_STATUS __nvoc_objCreateDynamic_NoDeviceMemory(NoDeviceMemory**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_NoDeviceMemory(NoDeviceMemory**, Dynamic*, NvU32, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_NoDeviceMemory(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_NoDeviceMemory((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define nodevicememGetMapAddrSpace_FNPTR(pNoDeviceMemory) pNoDeviceMemory->__nodevicememGetMapAddrSpace__
#define nodevicememGetMapAddrSpace(pNoDeviceMemory, pCallContext, mapFlags, pAddrSpace) nodevicememGetMapAddrSpace_DISPATCH(pNoDeviceMemory, pCallContext, mapFlags, pAddrSpace)
#define nodevicememIsDuplicate_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memIsDuplicate__
#define nodevicememIsDuplicate(pMemory, hMemory, pDuplicate) nodevicememIsDuplicate_DISPATCH(pMemory, hMemory, pDuplicate)
#define nodevicememControl_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memControl__
#define nodevicememControl(pMemory, pCallContext, pParams) nodevicememControl_DISPATCH(pMemory, pCallContext, pParams)
#define nodevicememMap_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memMap__
#define nodevicememMap(pMemory, pCallContext, pParams, pCpuMapping) nodevicememMap_DISPATCH(pMemory, pCallContext, pParams, pCpuMapping)
#define nodevicememUnmap_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memUnmap__
#define nodevicememUnmap(pMemory, pCallContext, pCpuMapping) nodevicememUnmap_DISPATCH(pMemory, pCallContext, pCpuMapping)
#define nodevicememGetMemInterMapParams_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memGetMemInterMapParams__
#define nodevicememGetMemInterMapParams(pMemory, pParams) nodevicememGetMemInterMapParams_DISPATCH(pMemory, pParams)
#define nodevicememCheckMemInterUnmap_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memCheckMemInterUnmap__
#define nodevicememCheckMemInterUnmap(pMemory, bSubdeviceHandleProvided) nodevicememCheckMemInterUnmap_DISPATCH(pMemory, bSubdeviceHandleProvided)
#define nodevicememGetMemoryMappingDescriptor_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memGetMemoryMappingDescriptor__
#define nodevicememGetMemoryMappingDescriptor(pMemory, ppMemDesc) nodevicememGetMemoryMappingDescriptor_DISPATCH(pMemory, ppMemDesc)
#define nodevicememCheckCopyPermissions_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memCheckCopyPermissions__
#define nodevicememCheckCopyPermissions(pMemory, pDstGpu, pDstDevice) nodevicememCheckCopyPermissions_DISPATCH(pMemory, pDstGpu, pDstDevice)
#define nodevicememIsReady_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memIsReady__
#define nodevicememIsReady(pMemory, bCopyConstructorContext) nodevicememIsReady_DISPATCH(pMemory, bCopyConstructorContext)
#define nodevicememIsGpuMapAllowed_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memIsGpuMapAllowed__
#define nodevicememIsGpuMapAllowed(pMemory, pGpu) nodevicememIsGpuMapAllowed_DISPATCH(pMemory, pGpu)
#define nodevicememIsExportAllowed_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memIsExportAllowed__
#define nodevicememIsExportAllowed(pMemory) nodevicememIsExportAllowed_DISPATCH(pMemory)
#define nodevicememAccessCallback_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__rmresAccessCallback__
#define nodevicememAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) nodevicememAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define nodevicememShareCallback_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__rmresShareCallback__
#define nodevicememShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) nodevicememShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define nodevicememControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__rmresControlSerialization_Prologue__
#define nodevicememControlSerialization_Prologue(pResource, pCallContext, pParams) nodevicememControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define nodevicememControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__rmresControlSerialization_Epilogue__
#define nodevicememControlSerialization_Epilogue(pResource, pCallContext, pParams) nodevicememControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define nodevicememControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__rmresControl_Prologue__
#define nodevicememControl_Prologue(pResource, pCallContext, pParams) nodevicememControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define nodevicememControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__rmresControl_Epilogue__
#define nodevicememControl_Epilogue(pResource, pCallContext, pParams) nodevicememControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define nodevicememCanCopy_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__
#define nodevicememCanCopy(pResource) nodevicememCanCopy_DISPATCH(pResource)
#define nodevicememPreDestruct_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resPreDestruct__
#define nodevicememPreDestruct(pResource) nodevicememPreDestruct_DISPATCH(pResource)
#define nodevicememControlFilter_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resControlFilter__
#define nodevicememControlFilter(pResource, pCallContext, pParams) nodevicememControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define nodevicememIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsPartialUnmapSupported__
#define nodevicememIsPartialUnmapSupported(pResource) nodevicememIsPartialUnmapSupported_DISPATCH(pResource)
#define nodevicememMapTo_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resMapTo__
#define nodevicememMapTo(pResource, pParams) nodevicememMapTo_DISPATCH(pResource, pParams)
#define nodevicememUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmapFrom__
#define nodevicememUnmapFrom(pResource, pParams) nodevicememUnmapFrom_DISPATCH(pResource, pParams)
#define nodevicememGetRefCount_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resGetRefCount__
#define nodevicememGetRefCount(pResource) nodevicememGetRefCount_DISPATCH(pResource)
#define nodevicememAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resAddAdditionalDependants__
#define nodevicememAddAdditionalDependants(pClient, pResource, pReference) nodevicememAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NV_STATUS nodevicememGetMapAddrSpace_DISPATCH(struct NoDeviceMemory *pNoDeviceMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pNoDeviceMemory->__nodevicememGetMapAddrSpace__(pNoDeviceMemory, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS nodevicememIsDuplicate_DISPATCH(struct NoDeviceMemory *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return pMemory->__nodevicememIsDuplicate__(pMemory, hMemory, pDuplicate);
}

static inline NV_STATUS nodevicememControl_DISPATCH(struct NoDeviceMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pMemory->__nodevicememControl__(pMemory, pCallContext, pParams);
}

static inline NV_STATUS nodevicememMap_DISPATCH(struct NoDeviceMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pMemory->__nodevicememMap__(pMemory, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS nodevicememUnmap_DISPATCH(struct NoDeviceMemory *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pMemory->__nodevicememUnmap__(pMemory, pCallContext, pCpuMapping);
}

static inline NV_STATUS nodevicememGetMemInterMapParams_DISPATCH(struct NoDeviceMemory *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pMemory->__nodevicememGetMemInterMapParams__(pMemory, pParams);
}

static inline NV_STATUS nodevicememCheckMemInterUnmap_DISPATCH(struct NoDeviceMemory *pMemory, NvBool bSubdeviceHandleProvided) {
    return pMemory->__nodevicememCheckMemInterUnmap__(pMemory, bSubdeviceHandleProvided);
}

static inline NV_STATUS nodevicememGetMemoryMappingDescriptor_DISPATCH(struct NoDeviceMemory *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return pMemory->__nodevicememGetMemoryMappingDescriptor__(pMemory, ppMemDesc);
}

static inline NV_STATUS nodevicememCheckCopyPermissions_DISPATCH(struct NoDeviceMemory *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return pMemory->__nodevicememCheckCopyPermissions__(pMemory, pDstGpu, pDstDevice);
}

static inline NV_STATUS nodevicememIsReady_DISPATCH(struct NoDeviceMemory *pMemory, NvBool bCopyConstructorContext) {
    return pMemory->__nodevicememIsReady__(pMemory, bCopyConstructorContext);
}

static inline NvBool nodevicememIsGpuMapAllowed_DISPATCH(struct NoDeviceMemory *pMemory, struct OBJGPU *pGpu) {
    return pMemory->__nodevicememIsGpuMapAllowed__(pMemory, pGpu);
}

static inline NvBool nodevicememIsExportAllowed_DISPATCH(struct NoDeviceMemory *pMemory) {
    return pMemory->__nodevicememIsExportAllowed__(pMemory);
}

static inline NvBool nodevicememAccessCallback_DISPATCH(struct NoDeviceMemory *pResource, RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nodevicememAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool nodevicememShareCallback_DISPATCH(struct NoDeviceMemory *pResource, RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__nodevicememShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS nodevicememControlSerialization_Prologue_DISPATCH(struct NoDeviceMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nodevicememControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void nodevicememControlSerialization_Epilogue_DISPATCH(struct NoDeviceMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nodevicememControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS nodevicememControl_Prologue_DISPATCH(struct NoDeviceMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nodevicememControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void nodevicememControl_Epilogue_DISPATCH(struct NoDeviceMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nodevicememControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool nodevicememCanCopy_DISPATCH(struct NoDeviceMemory *pResource) {
    return pResource->__nodevicememCanCopy__(pResource);
}

static inline void nodevicememPreDestruct_DISPATCH(struct NoDeviceMemory *pResource) {
    pResource->__nodevicememPreDestruct__(pResource);
}

static inline NV_STATUS nodevicememControlFilter_DISPATCH(struct NoDeviceMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nodevicememControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool nodevicememIsPartialUnmapSupported_DISPATCH(struct NoDeviceMemory *pResource) {
    return pResource->__nodevicememIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS nodevicememMapTo_DISPATCH(struct NoDeviceMemory *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nodevicememMapTo__(pResource, pParams);
}

static inline NV_STATUS nodevicememUnmapFrom_DISPATCH(struct NoDeviceMemory *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nodevicememUnmapFrom__(pResource, pParams);
}

static inline NvU32 nodevicememGetRefCount_DISPATCH(struct NoDeviceMemory *pResource) {
    return pResource->__nodevicememGetRefCount__(pResource);
}

static inline void nodevicememAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct NoDeviceMemory *pResource, RsResourceRef *pReference) {
    pResource->__nodevicememAddAdditionalDependants__(pClient, pResource, pReference);
}

NV_STATUS nodevicememGetMapAddrSpace_IMPL(struct NoDeviceMemory *pNoDeviceMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);

NV_STATUS nodevicememConstruct_IMPL(struct NoDeviceMemory *arg_pNoDeviceMemory, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_nodevicememConstruct(arg_pNoDeviceMemory, arg_pCallContext, arg_pParams) nodevicememConstruct_IMPL(arg_pNoDeviceMemory, arg_pCallContext, arg_pParams)
void nodevicememDestruct_IMPL(struct NoDeviceMemory *pNoDeviceMemory);

#define __nvoc_nodevicememDestruct(pNoDeviceMemory) nodevicememDestruct_IMPL(pNoDeviceMemory)
#undef PRIVATE_FIELD


#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_NO_DEVICE_MEM_NVOC_H_
