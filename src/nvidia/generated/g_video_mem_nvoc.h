
#ifndef _G_VIDEO_MEM_NVOC_H_
#define _G_VIDEO_MEM_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2020-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_video_mem_nvoc.h"

#ifndef _VIDEO_MEMORY_H_
#define _VIDEO_MEMORY_H_

#include "mem_mgr/standard_mem.h"
#include "gpu/mem_mgr/heap_base.h"

typedef struct PMA_ALLOC_INFO PMA_ALLOC_INFO;
struct Device;

#ifndef __nvoc_class_id_Device
#define __nvoc_class_id_Device 0xe0ac20u
typedef struct Device Device;
#endif /* __nvoc_class_id_Device */




// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_VIDEO_MEM_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__VideoMemory;
struct NVOC_METADATA__StandardMemory;


struct VideoMemory {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__VideoMemory *__nvoc_metadata_ptr;
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
    struct VideoMemory *__nvoc_pbase_VideoMemory;    // vidmem
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__VideoMemory {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__StandardMemory metadata__StandardMemory;
};

#ifndef __nvoc_class_id_VideoMemory
#define __nvoc_class_id_VideoMemory 0xed948fu
typedef struct VideoMemory VideoMemory;
#endif /* __nvoc_class_id_VideoMemory */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_VideoMemory;

#define __staticCast_VideoMemory(pThis) \
    ((pThis)->__nvoc_pbase_VideoMemory)

#ifdef __nvoc_video_mem_h_disabled
#define __dynamicCast_VideoMemory(pThis) ((VideoMemory*) NULL)
#else //__nvoc_video_mem_h_disabled
#define __dynamicCast_VideoMemory(pThis) \
    ((VideoMemory*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(VideoMemory)))
#endif //__nvoc_video_mem_h_disabled

NV_STATUS __nvoc_objCreateDynamic_VideoMemory(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_VideoMemory(VideoMemory**, Dynamic*, NvU32, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __objCreate_VideoMemory(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams) \
    __nvoc_objCreate_VideoMemory((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pCallContext, pParams)


// Wrapper macros for implementation functions
NV_STATUS vidmemConstruct_IMPL(struct VideoMemory *pVideoMemory, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __nvoc_vidmemConstruct(pVideoMemory, pCallContext, pParams) vidmemConstruct_IMPL(pVideoMemory, pCallContext, pParams)

void vidmemDestruct_IMPL(struct VideoMemory *pVideoMemory);
#define __nvoc_vidmemDestruct(pVideoMemory) vidmemDestruct_IMPL(pVideoMemory)

struct Heap * vidmemGetHeap_IMPL(struct OBJGPU *pGpu, struct Device *pDevice, NvBool bSubheap, NvBool bForceGlobalHeap);
#define vidmemGetHeap(pGpu, pDevice, bSubheap, bForceGlobalHeap) vidmemGetHeap_IMPL(pGpu, pDevice, bSubheap, bForceGlobalHeap)


// Wrapper macros for halified functions
#define vidmemCheckCopyPermissions_FNPTR(pVideoMemory) pVideoMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memCheckCopyPermissions__
#define vidmemCheckCopyPermissions(pVideoMemory, pDstGpu, pDstDevice) vidmemCheckCopyPermissions_DISPATCH(pVideoMemory, pDstGpu, pDstDevice)
#define vidmemCanCopy_FNPTR(pStandardMemory) pStandardMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define vidmemCanCopy(pStandardMemory) vidmemCanCopy_DISPATCH(pStandardMemory)
#define vidmemIsDuplicate_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define vidmemIsDuplicate(pMemory, hMemory, pDuplicate) vidmemIsDuplicate_DISPATCH(pMemory, hMemory, pDuplicate)
#define vidmemGetMapAddrSpace_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memGetMapAddrSpace__
#define vidmemGetMapAddrSpace(pMemory, pCallContext, mapFlags, pAddrSpace) vidmemGetMapAddrSpace_DISPATCH(pMemory, pCallContext, mapFlags, pAddrSpace)
#define vidmemControl_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__
#define vidmemControl(pMemory, pCallContext, pParams) vidmemControl_DISPATCH(pMemory, pCallContext, pParams)
#define vidmemMap_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__
#define vidmemMap(pMemory, pCallContext, pParams, pCpuMapping) vidmemMap_DISPATCH(pMemory, pCallContext, pParams, pCpuMapping)
#define vidmemUnmap_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__
#define vidmemUnmap(pMemory, pCallContext, pCpuMapping) vidmemUnmap_DISPATCH(pMemory, pCallContext, pCpuMapping)
#define vidmemGetMemInterMapParams_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define vidmemGetMemInterMapParams(pMemory, pParams) vidmemGetMemInterMapParams_DISPATCH(pMemory, pParams)
#define vidmemCheckMemInterUnmap_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define vidmemCheckMemInterUnmap(pMemory, bSubdeviceHandleProvided) vidmemCheckMemInterUnmap_DISPATCH(pMemory, bSubdeviceHandleProvided)
#define vidmemGetMemoryMappingDescriptor_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define vidmemGetMemoryMappingDescriptor(pMemory, ppMemDesc) vidmemGetMemoryMappingDescriptor_DISPATCH(pMemory, ppMemDesc)
#define vidmemIsReady_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsReady__
#define vidmemIsReady(pMemory, bCopyConstructorContext) vidmemIsReady_DISPATCH(pMemory, bCopyConstructorContext)
#define vidmemIsGpuMapAllowed_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsGpuMapAllowed__
#define vidmemIsGpuMapAllowed(pMemory, pGpu) vidmemIsGpuMapAllowed_DISPATCH(pMemory, pGpu)
#define vidmemIsExportAllowed_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsExportAllowed__
#define vidmemIsExportAllowed(pMemory) vidmemIsExportAllowed_DISPATCH(pMemory)
#define vidmemAccessCallback_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__
#define vidmemAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) vidmemAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define vidmemShareCallback_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__
#define vidmemShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) vidmemShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define vidmemControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__
#define vidmemControlSerialization_Prologue(pResource, pCallContext, pParams) vidmemControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define vidmemControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__
#define vidmemControlSerialization_Epilogue(pResource, pCallContext, pParams) vidmemControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define vidmemControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__
#define vidmemControl_Prologue(pResource, pCallContext, pParams) vidmemControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define vidmemControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__
#define vidmemControl_Epilogue(pResource, pCallContext, pParams) vidmemControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define vidmemPreDestruct_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define vidmemPreDestruct(pResource) vidmemPreDestruct_DISPATCH(pResource)
#define vidmemControlFilter_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define vidmemControlFilter(pResource, pCallContext, pParams) vidmemControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define vidmemIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define vidmemIsPartialUnmapSupported(pResource) vidmemIsPartialUnmapSupported_DISPATCH(pResource)
#define vidmemMapTo_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define vidmemMapTo(pResource, pParams) vidmemMapTo_DISPATCH(pResource, pParams)
#define vidmemUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define vidmemUnmapFrom(pResource, pParams) vidmemUnmapFrom_DISPATCH(pResource, pParams)
#define vidmemGetRefCount_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define vidmemGetRefCount(pResource) vidmemGetRefCount_DISPATCH(pResource)
#define vidmemAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define vidmemAddAdditionalDependants(pClient, pResource, pReference) vidmemAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NV_STATUS vidmemCheckCopyPermissions_DISPATCH(struct VideoMemory *pVideoMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return pVideoMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memCheckCopyPermissions__(staticCast(pVideoMemory, Memory), pDstGpu, pDstDevice);
}

static inline NvBool vidmemCanCopy_DISPATCH(struct VideoMemory *pStandardMemory) {
    return pStandardMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__(staticCast(pStandardMemory, RsResource));
}

static inline NV_STATUS vidmemIsDuplicate_DISPATCH(struct VideoMemory *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__(staticCast(pMemory, RsResource), hMemory, pDuplicate);
}

static inline NV_STATUS vidmemGetMapAddrSpace_DISPATCH(struct VideoMemory *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memGetMapAddrSpace__(staticCast(pMemory, Memory), pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS vidmemControl_DISPATCH(struct VideoMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__(staticCast(pMemory, RsResource), pCallContext, pParams);
}

static inline NV_STATUS vidmemMap_DISPATCH(struct VideoMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__(staticCast(pMemory, RsResource), pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS vidmemUnmap_DISPATCH(struct VideoMemory *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__(staticCast(pMemory, RsResource), pCallContext, pCpuMapping);
}

static inline NV_STATUS vidmemGetMemInterMapParams_DISPATCH(struct VideoMemory *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__(staticCast(pMemory, RmResource), pParams);
}

static inline NV_STATUS vidmemCheckMemInterUnmap_DISPATCH(struct VideoMemory *pMemory, NvBool bSubdeviceHandleProvided) {
    return pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__(staticCast(pMemory, RmResource), bSubdeviceHandleProvided);
}

static inline NV_STATUS vidmemGetMemoryMappingDescriptor_DISPATCH(struct VideoMemory *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__(staticCast(pMemory, RmResource), ppMemDesc);
}

static inline NV_STATUS vidmemIsReady_DISPATCH(struct VideoMemory *pMemory, NvBool bCopyConstructorContext) {
    return pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsReady__(staticCast(pMemory, Memory), bCopyConstructorContext);
}

static inline NvBool vidmemIsGpuMapAllowed_DISPATCH(struct VideoMemory *pMemory, struct OBJGPU *pGpu) {
    return pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsGpuMapAllowed__(staticCast(pMemory, Memory), pGpu);
}

static inline NvBool vidmemIsExportAllowed_DISPATCH(struct VideoMemory *pMemory) {
    return pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsExportAllowed__(staticCast(pMemory, Memory));
}

static inline NvBool vidmemAccessCallback_DISPATCH(struct VideoMemory *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__(staticCast(pResource, RsResource), pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool vidmemShareCallback_DISPATCH(struct VideoMemory *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__(staticCast(pResource, RsResource), pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS vidmemControlSerialization_Prologue_DISPATCH(struct VideoMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void vidmemControlSerialization_Epilogue_DISPATCH(struct VideoMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS vidmemControl_Prologue_DISPATCH(struct VideoMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void vidmemControl_Epilogue_DISPATCH(struct VideoMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void vidmemPreDestruct_DISPATCH(struct VideoMemory *pResource) {
    pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__(staticCast(pResource, RsResource));
}

static inline NV_STATUS vidmemControlFilter_DISPATCH(struct VideoMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool vidmemIsPartialUnmapSupported_DISPATCH(struct VideoMemory *pResource) {
    return pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__(staticCast(pResource, RsResource));
}

static inline NV_STATUS vidmemMapTo_DISPATCH(struct VideoMemory *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__(staticCast(pResource, RsResource), pParams);
}

static inline NV_STATUS vidmemUnmapFrom_DISPATCH(struct VideoMemory *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__(staticCast(pResource, RsResource), pParams);
}

static inline NvU32 vidmemGetRefCount_DISPATCH(struct VideoMemory *pResource) {
    return pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__(staticCast(pResource, RsResource));
}

static inline void vidmemAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct VideoMemory *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__(pClient, staticCast(pResource, RsResource), pReference);
}

// Virtual method declarations and/or inline definitions
NV_STATUS vidmemCheckCopyPermissions_IMPL(struct VideoMemory *pVideoMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice);

// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


void vidmemPmaFree(OBJGPU *, struct Heap *, PMA_ALLOC_INFO *, NvU32 flags);

NV_STATUS vidmemAllocResources(OBJGPU *pGpu, struct MemoryManager *pMemoryManager,
                               MEMORY_ALLOCATION_REQUEST *pAllocRequest,
                               FB_ALLOC_INFO *pFbAllocInfo, struct Heap *pHeap);

#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_VIDEO_MEM_NVOC_H_
