#ifndef _G_STANDARD_MEM_NVOC_H_
#define _G_STANDARD_MEM_NVOC_H_
#include "nvoc/runtime.h"

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

#include "g_standard_mem_nvoc.h"

#ifndef _STANDARD_MEMORY_H_
#define _STANDARD_MEMORY_H_

#include "mem_mgr/mem.h"

#include "ctrl/ctrl003e.h"

typedef struct MEMORY_ALLOCATION_REQUEST MEMORY_ALLOCATION_REQUEST;

struct MemoryManager;

#ifndef __NVOC_CLASS_MemoryManager_TYPEDEF__
#define __NVOC_CLASS_MemoryManager_TYPEDEF__
typedef struct MemoryManager MemoryManager;
#endif /* __NVOC_CLASS_MemoryManager_TYPEDEF__ */

#ifndef __nvoc_class_id_MemoryManager
#define __nvoc_class_id_MemoryManager 0x22ad47
#endif /* __nvoc_class_id_MemoryManager */


/*!
 * Allocator for normal virtual, video and system memory
 */
#ifdef NVOC_STANDARD_MEM_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct StandardMemory {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct Memory __nvoc_base_Memory;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct Memory *__nvoc_pbase_Memory;
    struct StandardMemory *__nvoc_pbase_StandardMemory;
    NvBool (*__stdmemCanCopy__)(struct StandardMemory *);
    NV_STATUS (*__stdmemCheckMemInterUnmap__)(struct StandardMemory *, NvBool);
    NV_STATUS (*__stdmemControl__)(struct StandardMemory *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__stdmemUnmap__)(struct StandardMemory *, CALL_CONTEXT *, RsCpuMapping *);
    NV_STATUS (*__stdmemGetMemInterMapParams__)(struct StandardMemory *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__stdmemGetMemoryMappingDescriptor__)(struct StandardMemory *, MEMORY_DESCRIPTOR **);
    NV_STATUS (*__stdmemGetMapAddrSpace__)(struct StandardMemory *, CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    NvBool (*__stdmemShareCallback__)(struct StandardMemory *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__stdmemControlFilter__)(struct StandardMemory *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    void (*__stdmemAddAdditionalDependants__)(struct RsClient *, struct StandardMemory *, RsResourceRef *);
    NvU32 (*__stdmemGetRefCount__)(struct StandardMemory *);
    NV_STATUS (*__stdmemMapTo__)(struct StandardMemory *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__stdmemControl_Prologue__)(struct StandardMemory *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__stdmemIsReady__)(struct StandardMemory *);
    NV_STATUS (*__stdmemCheckCopyPermissions__)(struct StandardMemory *, struct OBJGPU *, NvHandle);
    void (*__stdmemPreDestruct__)(struct StandardMemory *);
    NV_STATUS (*__stdmemUnmapFrom__)(struct StandardMemory *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__stdmemControl_Epilogue__)(struct StandardMemory *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__stdmemControlLookup__)(struct StandardMemory *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NV_STATUS (*__stdmemMap__)(struct StandardMemory *, CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, RsCpuMapping *);
    NvBool (*__stdmemAccessCallback__)(struct StandardMemory *, struct RsClient *, void *, RsAccessRight);
};

#ifndef __NVOC_CLASS_StandardMemory_TYPEDEF__
#define __NVOC_CLASS_StandardMemory_TYPEDEF__
typedef struct StandardMemory StandardMemory;
#endif /* __NVOC_CLASS_StandardMemory_TYPEDEF__ */

#ifndef __nvoc_class_id_StandardMemory
#define __nvoc_class_id_StandardMemory 0x897bf7
#endif /* __nvoc_class_id_StandardMemory */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_StandardMemory;

#define __staticCast_StandardMemory(pThis) \
    ((pThis)->__nvoc_pbase_StandardMemory)

#ifdef __nvoc_standard_mem_h_disabled
#define __dynamicCast_StandardMemory(pThis) ((StandardMemory*)NULL)
#else //__nvoc_standard_mem_h_disabled
#define __dynamicCast_StandardMemory(pThis) \
    ((StandardMemory*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(StandardMemory)))
#endif //__nvoc_standard_mem_h_disabled


NV_STATUS __nvoc_objCreateDynamic_StandardMemory(StandardMemory**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_StandardMemory(StandardMemory**, Dynamic*, NvU32, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_StandardMemory(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_StandardMemory((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define stdmemCanCopy(pStandardMemory) stdmemCanCopy_DISPATCH(pStandardMemory)
#define stdmemCheckMemInterUnmap(pMemory, bSubdeviceHandleProvided) stdmemCheckMemInterUnmap_DISPATCH(pMemory, bSubdeviceHandleProvided)
#define stdmemControl(pMemory, pCallContext, pParams) stdmemControl_DISPATCH(pMemory, pCallContext, pParams)
#define stdmemUnmap(pMemory, pCallContext, pCpuMapping) stdmemUnmap_DISPATCH(pMemory, pCallContext, pCpuMapping)
#define stdmemGetMemInterMapParams(pMemory, pParams) stdmemGetMemInterMapParams_DISPATCH(pMemory, pParams)
#define stdmemGetMemoryMappingDescriptor(pMemory, ppMemDesc) stdmemGetMemoryMappingDescriptor_DISPATCH(pMemory, ppMemDesc)
#define stdmemGetMapAddrSpace(pMemory, pCallContext, mapFlags, pAddrSpace) stdmemGetMapAddrSpace_DISPATCH(pMemory, pCallContext, mapFlags, pAddrSpace)
#define stdmemShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) stdmemShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define stdmemControlFilter(pResource, pCallContext, pParams) stdmemControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define stdmemAddAdditionalDependants(pClient, pResource, pReference) stdmemAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define stdmemGetRefCount(pResource) stdmemGetRefCount_DISPATCH(pResource)
#define stdmemMapTo(pResource, pParams) stdmemMapTo_DISPATCH(pResource, pParams)
#define stdmemControl_Prologue(pResource, pCallContext, pParams) stdmemControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define stdmemIsReady(pMemory) stdmemIsReady_DISPATCH(pMemory)
#define stdmemCheckCopyPermissions(pMemory, pDstGpu, hDstClientNvBool) stdmemCheckCopyPermissions_DISPATCH(pMemory, pDstGpu, hDstClientNvBool)
#define stdmemPreDestruct(pResource) stdmemPreDestruct_DISPATCH(pResource)
#define stdmemUnmapFrom(pResource, pParams) stdmemUnmapFrom_DISPATCH(pResource, pParams)
#define stdmemControl_Epilogue(pResource, pCallContext, pParams) stdmemControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define stdmemControlLookup(pResource, pParams, ppEntry) stdmemControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define stdmemMap(pMemory, pCallContext, pParams, pCpuMapping) stdmemMap_DISPATCH(pMemory, pCallContext, pParams, pCpuMapping)
#define stdmemAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) stdmemAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
NvU32 stdmemGetSysmemPageSize_IMPL(struct OBJGPU *pGpu, struct StandardMemory *pMemory);

#ifdef __nvoc_standard_mem_h_disabled
static inline NvU32 stdmemGetSysmemPageSize(struct OBJGPU *pGpu, struct StandardMemory *pMemory) {
    NV_ASSERT_FAILED_PRECOMP("StandardMemory was disabled!");
    return 0;
}
#else //__nvoc_standard_mem_h_disabled
#define stdmemGetSysmemPageSize(pGpu, pMemory) stdmemGetSysmemPageSize_IMPL(pGpu, pMemory)
#endif //__nvoc_standard_mem_h_disabled

#define stdmemGetSysmemPageSize_HAL(pGpu, pMemory) stdmemGetSysmemPageSize(pGpu, pMemory)

NvBool stdmemCanCopy_IMPL(struct StandardMemory *pStandardMemory);

static inline NvBool stdmemCanCopy_DISPATCH(struct StandardMemory *pStandardMemory) {
    return pStandardMemory->__stdmemCanCopy__(pStandardMemory);
}

static inline NV_STATUS stdmemCheckMemInterUnmap_DISPATCH(struct StandardMemory *pMemory, NvBool bSubdeviceHandleProvided) {
    return pMemory->__stdmemCheckMemInterUnmap__(pMemory, bSubdeviceHandleProvided);
}

static inline NV_STATUS stdmemControl_DISPATCH(struct StandardMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pMemory->__stdmemControl__(pMemory, pCallContext, pParams);
}

static inline NV_STATUS stdmemUnmap_DISPATCH(struct StandardMemory *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pMemory->__stdmemUnmap__(pMemory, pCallContext, pCpuMapping);
}

static inline NV_STATUS stdmemGetMemInterMapParams_DISPATCH(struct StandardMemory *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pMemory->__stdmemGetMemInterMapParams__(pMemory, pParams);
}

static inline NV_STATUS stdmemGetMemoryMappingDescriptor_DISPATCH(struct StandardMemory *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return pMemory->__stdmemGetMemoryMappingDescriptor__(pMemory, ppMemDesc);
}

static inline NV_STATUS stdmemGetMapAddrSpace_DISPATCH(struct StandardMemory *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pMemory->__stdmemGetMapAddrSpace__(pMemory, pCallContext, mapFlags, pAddrSpace);
}

static inline NvBool stdmemShareCallback_DISPATCH(struct StandardMemory *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__stdmemShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS stdmemControlFilter_DISPATCH(struct StandardMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__stdmemControlFilter__(pResource, pCallContext, pParams);
}

static inline void stdmemAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct StandardMemory *pResource, RsResourceRef *pReference) {
    pResource->__stdmemAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NvU32 stdmemGetRefCount_DISPATCH(struct StandardMemory *pResource) {
    return pResource->__stdmemGetRefCount__(pResource);
}

static inline NV_STATUS stdmemMapTo_DISPATCH(struct StandardMemory *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__stdmemMapTo__(pResource, pParams);
}

static inline NV_STATUS stdmemControl_Prologue_DISPATCH(struct StandardMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__stdmemControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS stdmemIsReady_DISPATCH(struct StandardMemory *pMemory) {
    return pMemory->__stdmemIsReady__(pMemory);
}

static inline NV_STATUS stdmemCheckCopyPermissions_DISPATCH(struct StandardMemory *pMemory, struct OBJGPU *pDstGpu, NvHandle hDstClientNvBool) {
    return pMemory->__stdmemCheckCopyPermissions__(pMemory, pDstGpu, hDstClientNvBool);
}

static inline void stdmemPreDestruct_DISPATCH(struct StandardMemory *pResource) {
    pResource->__stdmemPreDestruct__(pResource);
}

static inline NV_STATUS stdmemUnmapFrom_DISPATCH(struct StandardMemory *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__stdmemUnmapFrom__(pResource, pParams);
}

static inline void stdmemControl_Epilogue_DISPATCH(struct StandardMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__stdmemControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS stdmemControlLookup_DISPATCH(struct StandardMemory *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__stdmemControlLookup__(pResource, pParams, ppEntry);
}

static inline NV_STATUS stdmemMap_DISPATCH(struct StandardMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pMemory->__stdmemMap__(pMemory, pCallContext, pParams, pCpuMapping);
}

static inline NvBool stdmemAccessCallback_DISPATCH(struct StandardMemory *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__stdmemAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

NV_STATUS stdmemConstruct_IMPL(struct StandardMemory *arg_pStandardMemory, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __nvoc_stdmemConstruct(arg_pStandardMemory, arg_pCallContext, arg_pParams) stdmemConstruct_IMPL(arg_pStandardMemory, arg_pCallContext, arg_pParams)
NV_STATUS stdmemValidateParams_IMPL(struct OBJGPU *pGpu, NvHandle hClient, NV_MEMORY_ALLOCATION_PARAMS *pAllocData);
#define stdmemValidateParams(pGpu, hClient, pAllocData) stdmemValidateParams_IMPL(pGpu, hClient, pAllocData)
void stdmemDumpInputAllocParams_IMPL(NV_MEMORY_ALLOCATION_PARAMS *pAllocData, CALL_CONTEXT *pCallContext);
#define stdmemDumpInputAllocParams(pAllocData, pCallContext) stdmemDumpInputAllocParams_IMPL(pAllocData, pCallContext)
void stdmemDumpOutputAllocParams_IMPL(NV_MEMORY_ALLOCATION_PARAMS *pAllocData);
#define stdmemDumpOutputAllocParams(pAllocData) stdmemDumpOutputAllocParams_IMPL(pAllocData)
NvU32 stdmemQueryPageSize_IMPL(struct MemoryManager *pMemoryManager, NvHandle hClient, NV_MEMORY_ALLOCATION_PARAMS *pAllocData);
#define stdmemQueryPageSize(pMemoryManager, hClient, pAllocData) stdmemQueryPageSize_IMPL(pMemoryManager, hClient, pAllocData)
#undef PRIVATE_FIELD


#endif

#ifdef __cplusplus
} // extern "C"
#endif
#endif // _G_STANDARD_MEM_NVOC_H_
