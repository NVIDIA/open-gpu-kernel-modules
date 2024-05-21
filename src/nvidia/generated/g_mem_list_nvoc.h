
#ifndef _G_MEM_LIST_NVOC_H_
#define _G_MEM_LIST_NVOC_H_
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
 * SPDX-FileCopyrightText: Copyright (c) 1993-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_mem_list_nvoc.h"

#ifndef _MEMORY_LIST_H_
#define _MEMORY_LIST_H_

#include "mem_mgr/mem.h"

/*!
 * These classes are used by the vGPU support to create memory objects for memory
 * assigned to a guest VM.
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_MEM_LIST_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct MemoryList {

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
    struct MemoryList *__nvoc_pbase_MemoryList;    // memlist

    // Vtable with 26 per-object function pointers
    NvBool (*__memlistCanCopy__)(struct MemoryList * /*this*/);  // virtual override (res) base (mem)
    NV_STATUS (*__memlistIsDuplicate__)(struct MemoryList * /*this*/, NvHandle, NvBool *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__memlistGetMapAddrSpace__)(struct MemoryList * /*this*/, CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__memlistControl__)(struct MemoryList * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__memlistMap__)(struct MemoryList * /*this*/, CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, RsCpuMapping *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__memlistUnmap__)(struct MemoryList * /*this*/, CALL_CONTEXT *, RsCpuMapping *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__memlistGetMemInterMapParams__)(struct MemoryList * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__memlistCheckMemInterUnmap__)(struct MemoryList * /*this*/, NvBool);  // inline virtual inherited (mem) base (mem) body
    NV_STATUS (*__memlistGetMemoryMappingDescriptor__)(struct MemoryList * /*this*/, MEMORY_DESCRIPTOR **);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__memlistCheckCopyPermissions__)(struct MemoryList * /*this*/, struct OBJGPU *, struct Device *);  // inline virtual inherited (mem) base (mem) body
    NV_STATUS (*__memlistIsReady__)(struct MemoryList * /*this*/, NvBool);  // virtual inherited (mem) base (mem)
    NvBool (*__memlistIsGpuMapAllowed__)(struct MemoryList * /*this*/, struct OBJGPU *);  // inline virtual inherited (mem) base (mem) body
    NvBool (*__memlistIsExportAllowed__)(struct MemoryList * /*this*/);  // inline virtual inherited (mem) base (mem) body
    NvBool (*__memlistAccessCallback__)(struct MemoryList * /*this*/, RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (mem)
    NvBool (*__memlistShareCallback__)(struct MemoryList * /*this*/, RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (rmres) base (mem)
    NV_STATUS (*__memlistControlSerialization_Prologue__)(struct MemoryList * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (mem)
    void (*__memlistControlSerialization_Epilogue__)(struct MemoryList * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (mem)
    NV_STATUS (*__memlistControl_Prologue__)(struct MemoryList * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (mem)
    void (*__memlistControl_Epilogue__)(struct MemoryList * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (mem)
    void (*__memlistPreDestruct__)(struct MemoryList * /*this*/);  // virtual inherited (res) base (mem)
    NV_STATUS (*__memlistControlFilter__)(struct MemoryList * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (mem)
    NvBool (*__memlistIsPartialUnmapSupported__)(struct MemoryList * /*this*/);  // inline virtual inherited (res) base (mem) body
    NV_STATUS (*__memlistMapTo__)(struct MemoryList * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (mem)
    NV_STATUS (*__memlistUnmapFrom__)(struct MemoryList * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (mem)
    NvU32 (*__memlistGetRefCount__)(struct MemoryList * /*this*/);  // virtual inherited (res) base (mem)
    void (*__memlistAddAdditionalDependants__)(struct RsClient *, struct MemoryList * /*this*/, RsResourceRef *);  // virtual inherited (res) base (mem)
};

#ifndef __NVOC_CLASS_MemoryList_TYPEDEF__
#define __NVOC_CLASS_MemoryList_TYPEDEF__
typedef struct MemoryList MemoryList;
#endif /* __NVOC_CLASS_MemoryList_TYPEDEF__ */

#ifndef __nvoc_class_id_MemoryList
#define __nvoc_class_id_MemoryList 0x298f78
#endif /* __nvoc_class_id_MemoryList */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_MemoryList;

#define __staticCast_MemoryList(pThis) \
    ((pThis)->__nvoc_pbase_MemoryList)

#ifdef __nvoc_mem_list_h_disabled
#define __dynamicCast_MemoryList(pThis) ((MemoryList*)NULL)
#else //__nvoc_mem_list_h_disabled
#define __dynamicCast_MemoryList(pThis) \
    ((MemoryList*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(MemoryList)))
#endif //__nvoc_mem_list_h_disabled

NV_STATUS __nvoc_objCreateDynamic_MemoryList(MemoryList**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_MemoryList(MemoryList**, Dynamic*, NvU32, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_MemoryList(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_MemoryList((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define memlistCanCopy_FNPTR(pMemoryList) pMemoryList->__memlistCanCopy__
#define memlistCanCopy(pMemoryList) memlistCanCopy_DISPATCH(pMemoryList)
#define memlistIsDuplicate_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memIsDuplicate__
#define memlistIsDuplicate(pMemory, hMemory, pDuplicate) memlistIsDuplicate_DISPATCH(pMemory, hMemory, pDuplicate)
#define memlistGetMapAddrSpace_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memGetMapAddrSpace__
#define memlistGetMapAddrSpace(pMemory, pCallContext, mapFlags, pAddrSpace) memlistGetMapAddrSpace_DISPATCH(pMemory, pCallContext, mapFlags, pAddrSpace)
#define memlistControl_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memControl__
#define memlistControl(pMemory, pCallContext, pParams) memlistControl_DISPATCH(pMemory, pCallContext, pParams)
#define memlistMap_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memMap__
#define memlistMap(pMemory, pCallContext, pParams, pCpuMapping) memlistMap_DISPATCH(pMemory, pCallContext, pParams, pCpuMapping)
#define memlistUnmap_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memUnmap__
#define memlistUnmap(pMemory, pCallContext, pCpuMapping) memlistUnmap_DISPATCH(pMemory, pCallContext, pCpuMapping)
#define memlistGetMemInterMapParams_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memGetMemInterMapParams__
#define memlistGetMemInterMapParams(pMemory, pParams) memlistGetMemInterMapParams_DISPATCH(pMemory, pParams)
#define memlistCheckMemInterUnmap_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memCheckMemInterUnmap__
#define memlistCheckMemInterUnmap(pMemory, bSubdeviceHandleProvided) memlistCheckMemInterUnmap_DISPATCH(pMemory, bSubdeviceHandleProvided)
#define memlistGetMemoryMappingDescriptor_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memGetMemoryMappingDescriptor__
#define memlistGetMemoryMappingDescriptor(pMemory, ppMemDesc) memlistGetMemoryMappingDescriptor_DISPATCH(pMemory, ppMemDesc)
#define memlistCheckCopyPermissions_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memCheckCopyPermissions__
#define memlistCheckCopyPermissions(pMemory, pDstGpu, pDstDevice) memlistCheckCopyPermissions_DISPATCH(pMemory, pDstGpu, pDstDevice)
#define memlistIsReady_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memIsReady__
#define memlistIsReady(pMemory, bCopyConstructorContext) memlistIsReady_DISPATCH(pMemory, bCopyConstructorContext)
#define memlistIsGpuMapAllowed_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memIsGpuMapAllowed__
#define memlistIsGpuMapAllowed(pMemory, pGpu) memlistIsGpuMapAllowed_DISPATCH(pMemory, pGpu)
#define memlistIsExportAllowed_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memIsExportAllowed__
#define memlistIsExportAllowed(pMemory) memlistIsExportAllowed_DISPATCH(pMemory)
#define memlistAccessCallback_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__rmresAccessCallback__
#define memlistAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) memlistAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define memlistShareCallback_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__rmresShareCallback__
#define memlistShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) memlistShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define memlistControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__rmresControlSerialization_Prologue__
#define memlistControlSerialization_Prologue(pResource, pCallContext, pParams) memlistControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define memlistControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__rmresControlSerialization_Epilogue__
#define memlistControlSerialization_Epilogue(pResource, pCallContext, pParams) memlistControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define memlistControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__rmresControl_Prologue__
#define memlistControl_Prologue(pResource, pCallContext, pParams) memlistControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define memlistControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__rmresControl_Epilogue__
#define memlistControl_Epilogue(pResource, pCallContext, pParams) memlistControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define memlistPreDestruct_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resPreDestruct__
#define memlistPreDestruct(pResource) memlistPreDestruct_DISPATCH(pResource)
#define memlistControlFilter_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resControlFilter__
#define memlistControlFilter(pResource, pCallContext, pParams) memlistControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define memlistIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsPartialUnmapSupported__
#define memlistIsPartialUnmapSupported(pResource) memlistIsPartialUnmapSupported_DISPATCH(pResource)
#define memlistMapTo_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resMapTo__
#define memlistMapTo(pResource, pParams) memlistMapTo_DISPATCH(pResource, pParams)
#define memlistUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmapFrom__
#define memlistUnmapFrom(pResource, pParams) memlistUnmapFrom_DISPATCH(pResource, pParams)
#define memlistGetRefCount_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resGetRefCount__
#define memlistGetRefCount(pResource) memlistGetRefCount_DISPATCH(pResource)
#define memlistAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resAddAdditionalDependants__
#define memlistAddAdditionalDependants(pClient, pResource, pReference) memlistAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NvBool memlistCanCopy_DISPATCH(struct MemoryList *pMemoryList) {
    return pMemoryList->__memlistCanCopy__(pMemoryList);
}

static inline NV_STATUS memlistIsDuplicate_DISPATCH(struct MemoryList *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return pMemory->__memlistIsDuplicate__(pMemory, hMemory, pDuplicate);
}

static inline NV_STATUS memlistGetMapAddrSpace_DISPATCH(struct MemoryList *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pMemory->__memlistGetMapAddrSpace__(pMemory, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS memlistControl_DISPATCH(struct MemoryList *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pMemory->__memlistControl__(pMemory, pCallContext, pParams);
}

static inline NV_STATUS memlistMap_DISPATCH(struct MemoryList *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pMemory->__memlistMap__(pMemory, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS memlistUnmap_DISPATCH(struct MemoryList *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pMemory->__memlistUnmap__(pMemory, pCallContext, pCpuMapping);
}

static inline NV_STATUS memlistGetMemInterMapParams_DISPATCH(struct MemoryList *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pMemory->__memlistGetMemInterMapParams__(pMemory, pParams);
}

static inline NV_STATUS memlistCheckMemInterUnmap_DISPATCH(struct MemoryList *pMemory, NvBool bSubdeviceHandleProvided) {
    return pMemory->__memlistCheckMemInterUnmap__(pMemory, bSubdeviceHandleProvided);
}

static inline NV_STATUS memlistGetMemoryMappingDescriptor_DISPATCH(struct MemoryList *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return pMemory->__memlistGetMemoryMappingDescriptor__(pMemory, ppMemDesc);
}

static inline NV_STATUS memlistCheckCopyPermissions_DISPATCH(struct MemoryList *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return pMemory->__memlistCheckCopyPermissions__(pMemory, pDstGpu, pDstDevice);
}

static inline NV_STATUS memlistIsReady_DISPATCH(struct MemoryList *pMemory, NvBool bCopyConstructorContext) {
    return pMemory->__memlistIsReady__(pMemory, bCopyConstructorContext);
}

static inline NvBool memlistIsGpuMapAllowed_DISPATCH(struct MemoryList *pMemory, struct OBJGPU *pGpu) {
    return pMemory->__memlistIsGpuMapAllowed__(pMemory, pGpu);
}

static inline NvBool memlistIsExportAllowed_DISPATCH(struct MemoryList *pMemory) {
    return pMemory->__memlistIsExportAllowed__(pMemory);
}

static inline NvBool memlistAccessCallback_DISPATCH(struct MemoryList *pResource, RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__memlistAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool memlistShareCallback_DISPATCH(struct MemoryList *pResource, RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__memlistShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS memlistControlSerialization_Prologue_DISPATCH(struct MemoryList *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__memlistControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void memlistControlSerialization_Epilogue_DISPATCH(struct MemoryList *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__memlistControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS memlistControl_Prologue_DISPATCH(struct MemoryList *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__memlistControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void memlistControl_Epilogue_DISPATCH(struct MemoryList *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__memlistControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline void memlistPreDestruct_DISPATCH(struct MemoryList *pResource) {
    pResource->__memlistPreDestruct__(pResource);
}

static inline NV_STATUS memlistControlFilter_DISPATCH(struct MemoryList *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__memlistControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool memlistIsPartialUnmapSupported_DISPATCH(struct MemoryList *pResource) {
    return pResource->__memlistIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS memlistMapTo_DISPATCH(struct MemoryList *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__memlistMapTo__(pResource, pParams);
}

static inline NV_STATUS memlistUnmapFrom_DISPATCH(struct MemoryList *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__memlistUnmapFrom__(pResource, pParams);
}

static inline NvU32 memlistGetRefCount_DISPATCH(struct MemoryList *pResource) {
    return pResource->__memlistGetRefCount__(pResource);
}

static inline void memlistAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct MemoryList *pResource, RsResourceRef *pReference) {
    pResource->__memlistAddAdditionalDependants__(pClient, pResource, pReference);
}

NvBool memlistCanCopy_IMPL(struct MemoryList *pMemoryList);

NV_STATUS memlistConstruct_IMPL(struct MemoryList *arg_pMemoryList, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_memlistConstruct(arg_pMemoryList, arg_pCallContext, arg_pParams) memlistConstruct_IMPL(arg_pMemoryList, arg_pCallContext, arg_pParams)
#undef PRIVATE_FIELD


#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_MEM_LIST_NVOC_H_
