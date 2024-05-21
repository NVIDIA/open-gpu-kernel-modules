
#ifndef _G_VIDEO_MEM_NVOC_H_
#define _G_VIDEO_MEM_NVOC_H_
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

#ifndef __NVOC_CLASS_Device_TYPEDEF__
#define __NVOC_CLASS_Device_TYPEDEF__
typedef struct Device Device;
#endif /* __NVOC_CLASS_Device_TYPEDEF__ */

#ifndef __nvoc_class_id_Device
#define __nvoc_class_id_Device 0xe0ac20
#endif /* __nvoc_class_id_Device */




// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_VIDEO_MEM_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct VideoMemory {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct StandardMemory __nvoc_base_StandardMemory;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct RsResource *__nvoc_pbase_RsResource;    // res super^4
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^4
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super^3
    struct Memory *__nvoc_pbase_Memory;    // mem super^2
    struct StandardMemory *__nvoc_pbase_StandardMemory;    // stdmem super
    struct VideoMemory *__nvoc_pbase_VideoMemory;    // vidmem

    // Vtable with 26 per-object function pointers
    NV_STATUS (*__vidmemCheckCopyPermissions__)(struct VideoMemory * /*this*/, struct OBJGPU *, struct Device *);  // virtual override (mem) base (stdmem)
    NvBool (*__vidmemCanCopy__)(struct VideoMemory * /*this*/);  // virtual inherited (stdmem) base (stdmem)
    NV_STATUS (*__vidmemIsDuplicate__)(struct VideoMemory * /*this*/, NvHandle, NvBool *);  // virtual inherited (mem) base (stdmem)
    NV_STATUS (*__vidmemGetMapAddrSpace__)(struct VideoMemory * /*this*/, CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (mem) base (stdmem)
    NV_STATUS (*__vidmemControl__)(struct VideoMemory * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (mem) base (stdmem)
    NV_STATUS (*__vidmemMap__)(struct VideoMemory * /*this*/, CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, RsCpuMapping *);  // virtual inherited (mem) base (stdmem)
    NV_STATUS (*__vidmemUnmap__)(struct VideoMemory * /*this*/, CALL_CONTEXT *, RsCpuMapping *);  // virtual inherited (mem) base (stdmem)
    NV_STATUS (*__vidmemGetMemInterMapParams__)(struct VideoMemory * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (mem) base (stdmem)
    NV_STATUS (*__vidmemCheckMemInterUnmap__)(struct VideoMemory * /*this*/, NvBool);  // inline virtual inherited (mem) base (stdmem) body
    NV_STATUS (*__vidmemGetMemoryMappingDescriptor__)(struct VideoMemory * /*this*/, MEMORY_DESCRIPTOR **);  // virtual inherited (mem) base (stdmem)
    NV_STATUS (*__vidmemIsReady__)(struct VideoMemory * /*this*/, NvBool);  // virtual inherited (mem) base (stdmem)
    NvBool (*__vidmemIsGpuMapAllowed__)(struct VideoMemory * /*this*/, struct OBJGPU *);  // inline virtual inherited (mem) base (stdmem) body
    NvBool (*__vidmemIsExportAllowed__)(struct VideoMemory * /*this*/);  // inline virtual inherited (mem) base (stdmem) body
    NvBool (*__vidmemAccessCallback__)(struct VideoMemory * /*this*/, RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (stdmem)
    NvBool (*__vidmemShareCallback__)(struct VideoMemory * /*this*/, RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (rmres) base (stdmem)
    NV_STATUS (*__vidmemControlSerialization_Prologue__)(struct VideoMemory * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (stdmem)
    void (*__vidmemControlSerialization_Epilogue__)(struct VideoMemory * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (stdmem)
    NV_STATUS (*__vidmemControl_Prologue__)(struct VideoMemory * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (stdmem)
    void (*__vidmemControl_Epilogue__)(struct VideoMemory * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (stdmem)
    void (*__vidmemPreDestruct__)(struct VideoMemory * /*this*/);  // virtual inherited (res) base (stdmem)
    NV_STATUS (*__vidmemControlFilter__)(struct VideoMemory * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (stdmem)
    NvBool (*__vidmemIsPartialUnmapSupported__)(struct VideoMemory * /*this*/);  // inline virtual inherited (res) base (stdmem) body
    NV_STATUS (*__vidmemMapTo__)(struct VideoMemory * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (stdmem)
    NV_STATUS (*__vidmemUnmapFrom__)(struct VideoMemory * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (stdmem)
    NvU32 (*__vidmemGetRefCount__)(struct VideoMemory * /*this*/);  // virtual inherited (res) base (stdmem)
    void (*__vidmemAddAdditionalDependants__)(struct RsClient *, struct VideoMemory * /*this*/, RsResourceRef *);  // virtual inherited (res) base (stdmem)
};

#ifndef __NVOC_CLASS_VideoMemory_TYPEDEF__
#define __NVOC_CLASS_VideoMemory_TYPEDEF__
typedef struct VideoMemory VideoMemory;
#endif /* __NVOC_CLASS_VideoMemory_TYPEDEF__ */

#ifndef __nvoc_class_id_VideoMemory
#define __nvoc_class_id_VideoMemory 0xed948f
#endif /* __nvoc_class_id_VideoMemory */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_VideoMemory;

#define __staticCast_VideoMemory(pThis) \
    ((pThis)->__nvoc_pbase_VideoMemory)

#ifdef __nvoc_video_mem_h_disabled
#define __dynamicCast_VideoMemory(pThis) ((VideoMemory*)NULL)
#else //__nvoc_video_mem_h_disabled
#define __dynamicCast_VideoMemory(pThis) \
    ((VideoMemory*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(VideoMemory)))
#endif //__nvoc_video_mem_h_disabled

NV_STATUS __nvoc_objCreateDynamic_VideoMemory(VideoMemory**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_VideoMemory(VideoMemory**, Dynamic*, NvU32, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_VideoMemory(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_VideoMemory((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define vidmemCheckCopyPermissions_FNPTR(pVideoMemory) pVideoMemory->__vidmemCheckCopyPermissions__
#define vidmemCheckCopyPermissions(pVideoMemory, pDstGpu, pDstDevice) vidmemCheckCopyPermissions_DISPATCH(pVideoMemory, pDstGpu, pDstDevice)
#define vidmemCanCopy_FNPTR(pStandardMemory) pStandardMemory->__nvoc_base_StandardMemory.__stdmemCanCopy__
#define vidmemCanCopy(pStandardMemory) vidmemCanCopy_DISPATCH(pStandardMemory)
#define vidmemIsDuplicate_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__memIsDuplicate__
#define vidmemIsDuplicate(pMemory, hMemory, pDuplicate) vidmemIsDuplicate_DISPATCH(pMemory, hMemory, pDuplicate)
#define vidmemGetMapAddrSpace_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__memGetMapAddrSpace__
#define vidmemGetMapAddrSpace(pMemory, pCallContext, mapFlags, pAddrSpace) vidmemGetMapAddrSpace_DISPATCH(pMemory, pCallContext, mapFlags, pAddrSpace)
#define vidmemControl_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__memControl__
#define vidmemControl(pMemory, pCallContext, pParams) vidmemControl_DISPATCH(pMemory, pCallContext, pParams)
#define vidmemMap_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__memMap__
#define vidmemMap(pMemory, pCallContext, pParams, pCpuMapping) vidmemMap_DISPATCH(pMemory, pCallContext, pParams, pCpuMapping)
#define vidmemUnmap_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__memUnmap__
#define vidmemUnmap(pMemory, pCallContext, pCpuMapping) vidmemUnmap_DISPATCH(pMemory, pCallContext, pCpuMapping)
#define vidmemGetMemInterMapParams_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__memGetMemInterMapParams__
#define vidmemGetMemInterMapParams(pMemory, pParams) vidmemGetMemInterMapParams_DISPATCH(pMemory, pParams)
#define vidmemCheckMemInterUnmap_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__memCheckMemInterUnmap__
#define vidmemCheckMemInterUnmap(pMemory, bSubdeviceHandleProvided) vidmemCheckMemInterUnmap_DISPATCH(pMemory, bSubdeviceHandleProvided)
#define vidmemGetMemoryMappingDescriptor_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__memGetMemoryMappingDescriptor__
#define vidmemGetMemoryMappingDescriptor(pMemory, ppMemDesc) vidmemGetMemoryMappingDescriptor_DISPATCH(pMemory, ppMemDesc)
#define vidmemIsReady_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__memIsReady__
#define vidmemIsReady(pMemory, bCopyConstructorContext) vidmemIsReady_DISPATCH(pMemory, bCopyConstructorContext)
#define vidmemIsGpuMapAllowed_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__memIsGpuMapAllowed__
#define vidmemIsGpuMapAllowed(pMemory, pGpu) vidmemIsGpuMapAllowed_DISPATCH(pMemory, pGpu)
#define vidmemIsExportAllowed_FNPTR(pMemory) pMemory->__nvoc_base_StandardMemory.__nvoc_base_Memory.__memIsExportAllowed__
#define vidmemIsExportAllowed(pMemory) vidmemIsExportAllowed_DISPATCH(pMemory)
#define vidmemAccessCallback_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__rmresAccessCallback__
#define vidmemAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) vidmemAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define vidmemShareCallback_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__rmresShareCallback__
#define vidmemShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) vidmemShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define vidmemControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__rmresControlSerialization_Prologue__
#define vidmemControlSerialization_Prologue(pResource, pCallContext, pParams) vidmemControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define vidmemControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__rmresControlSerialization_Epilogue__
#define vidmemControlSerialization_Epilogue(pResource, pCallContext, pParams) vidmemControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define vidmemControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__rmresControl_Prologue__
#define vidmemControl_Prologue(pResource, pCallContext, pParams) vidmemControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define vidmemControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__rmresControl_Epilogue__
#define vidmemControl_Epilogue(pResource, pCallContext, pParams) vidmemControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define vidmemPreDestruct_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resPreDestruct__
#define vidmemPreDestruct(pResource) vidmemPreDestruct_DISPATCH(pResource)
#define vidmemControlFilter_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resControlFilter__
#define vidmemControlFilter(pResource, pCallContext, pParams) vidmemControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define vidmemIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsPartialUnmapSupported__
#define vidmemIsPartialUnmapSupported(pResource) vidmemIsPartialUnmapSupported_DISPATCH(pResource)
#define vidmemMapTo_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resMapTo__
#define vidmemMapTo(pResource, pParams) vidmemMapTo_DISPATCH(pResource, pParams)
#define vidmemUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmapFrom__
#define vidmemUnmapFrom(pResource, pParams) vidmemUnmapFrom_DISPATCH(pResource, pParams)
#define vidmemGetRefCount_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resGetRefCount__
#define vidmemGetRefCount(pResource) vidmemGetRefCount_DISPATCH(pResource)
#define vidmemAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_StandardMemory.__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resAddAdditionalDependants__
#define vidmemAddAdditionalDependants(pClient, pResource, pReference) vidmemAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NV_STATUS vidmemCheckCopyPermissions_DISPATCH(struct VideoMemory *pVideoMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return pVideoMemory->__vidmemCheckCopyPermissions__(pVideoMemory, pDstGpu, pDstDevice);
}

static inline NvBool vidmemCanCopy_DISPATCH(struct VideoMemory *pStandardMemory) {
    return pStandardMemory->__vidmemCanCopy__(pStandardMemory);
}

static inline NV_STATUS vidmemIsDuplicate_DISPATCH(struct VideoMemory *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return pMemory->__vidmemIsDuplicate__(pMemory, hMemory, pDuplicate);
}

static inline NV_STATUS vidmemGetMapAddrSpace_DISPATCH(struct VideoMemory *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pMemory->__vidmemGetMapAddrSpace__(pMemory, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS vidmemControl_DISPATCH(struct VideoMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pMemory->__vidmemControl__(pMemory, pCallContext, pParams);
}

static inline NV_STATUS vidmemMap_DISPATCH(struct VideoMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pMemory->__vidmemMap__(pMemory, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS vidmemUnmap_DISPATCH(struct VideoMemory *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pMemory->__vidmemUnmap__(pMemory, pCallContext, pCpuMapping);
}

static inline NV_STATUS vidmemGetMemInterMapParams_DISPATCH(struct VideoMemory *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pMemory->__vidmemGetMemInterMapParams__(pMemory, pParams);
}

static inline NV_STATUS vidmemCheckMemInterUnmap_DISPATCH(struct VideoMemory *pMemory, NvBool bSubdeviceHandleProvided) {
    return pMemory->__vidmemCheckMemInterUnmap__(pMemory, bSubdeviceHandleProvided);
}

static inline NV_STATUS vidmemGetMemoryMappingDescriptor_DISPATCH(struct VideoMemory *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return pMemory->__vidmemGetMemoryMappingDescriptor__(pMemory, ppMemDesc);
}

static inline NV_STATUS vidmemIsReady_DISPATCH(struct VideoMemory *pMemory, NvBool bCopyConstructorContext) {
    return pMemory->__vidmemIsReady__(pMemory, bCopyConstructorContext);
}

static inline NvBool vidmemIsGpuMapAllowed_DISPATCH(struct VideoMemory *pMemory, struct OBJGPU *pGpu) {
    return pMemory->__vidmemIsGpuMapAllowed__(pMemory, pGpu);
}

static inline NvBool vidmemIsExportAllowed_DISPATCH(struct VideoMemory *pMemory) {
    return pMemory->__vidmemIsExportAllowed__(pMemory);
}

static inline NvBool vidmemAccessCallback_DISPATCH(struct VideoMemory *pResource, RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__vidmemAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool vidmemShareCallback_DISPATCH(struct VideoMemory *pResource, RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__vidmemShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS vidmemControlSerialization_Prologue_DISPATCH(struct VideoMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__vidmemControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void vidmemControlSerialization_Epilogue_DISPATCH(struct VideoMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__vidmemControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS vidmemControl_Prologue_DISPATCH(struct VideoMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__vidmemControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void vidmemControl_Epilogue_DISPATCH(struct VideoMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__vidmemControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline void vidmemPreDestruct_DISPATCH(struct VideoMemory *pResource) {
    pResource->__vidmemPreDestruct__(pResource);
}

static inline NV_STATUS vidmemControlFilter_DISPATCH(struct VideoMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__vidmemControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool vidmemIsPartialUnmapSupported_DISPATCH(struct VideoMemory *pResource) {
    return pResource->__vidmemIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS vidmemMapTo_DISPATCH(struct VideoMemory *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__vidmemMapTo__(pResource, pParams);
}

static inline NV_STATUS vidmemUnmapFrom_DISPATCH(struct VideoMemory *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__vidmemUnmapFrom__(pResource, pParams);
}

static inline NvU32 vidmemGetRefCount_DISPATCH(struct VideoMemory *pResource) {
    return pResource->__vidmemGetRefCount__(pResource);
}

static inline void vidmemAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct VideoMemory *pResource, RsResourceRef *pReference) {
    pResource->__vidmemAddAdditionalDependants__(pClient, pResource, pReference);
}

NV_STATUS vidmemCheckCopyPermissions_IMPL(struct VideoMemory *pVideoMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice);

NV_STATUS vidmemConstruct_IMPL(struct VideoMemory *arg_pVideoMemory, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_vidmemConstruct(arg_pVideoMemory, arg_pCallContext, arg_pParams) vidmemConstruct_IMPL(arg_pVideoMemory, arg_pCallContext, arg_pParams)
void vidmemDestruct_IMPL(struct VideoMemory *pVideoMemory);

#define __nvoc_vidmemDestruct(pVideoMemory) vidmemDestruct_IMPL(pVideoMemory)
struct Heap *vidmemGetHeap_IMPL(struct OBJGPU *pGpu, struct Device *pDevice, NvBool bSubheap, NvBool bForceGlobalHeap);

#define vidmemGetHeap(pGpu, pDevice, bSubheap, bForceGlobalHeap) vidmemGetHeap_IMPL(pGpu, pDevice, bSubheap, bForceGlobalHeap)
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
