#ifndef _G_EGM_MEM_NVOC_H_
#define _G_EGM_MEM_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_egm_mem_nvoc.h"

#ifndef _EGM_MEMORY_H_
#define _EGM_MEMORY_H_

#include "mem_mgr/video_mem.h"
#include "mem_mgr/system_mem.h"


/*!
 * Allocator for Extended GPU Memory (EGM)
 * EGM is CPU_MEM accessed like peer GPU_MEM
 */
#ifdef NVOC_EGM_MEM_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct ExtendedGpuMemory {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct StandardMemory __nvoc_base_StandardMemory;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct Memory *__nvoc_pbase_Memory;
    struct StandardMemory *__nvoc_pbase_StandardMemory;
    struct ExtendedGpuMemory *__nvoc_pbase_ExtendedGpuMemory;
    NV_STATUS (*__egmmemCheckMemInterUnmap__)(struct ExtendedGpuMemory *, NvBool);
    NvBool (*__egmmemShareCallback__)(struct ExtendedGpuMemory *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__egmmemMapTo__)(struct ExtendedGpuMemory *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__egmmemGetMapAddrSpace__)(struct ExtendedGpuMemory *, CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    NvBool (*__egmmemIsExportAllowed__)(struct ExtendedGpuMemory *);
    NvU32 (*__egmmemGetRefCount__)(struct ExtendedGpuMemory *);
    void (*__egmmemAddAdditionalDependants__)(struct RsClient *, struct ExtendedGpuMemory *, RsResourceRef *);
    NV_STATUS (*__egmmemControl_Prologue__)(struct ExtendedGpuMemory *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__egmmemIsGpuMapAllowed__)(struct ExtendedGpuMemory *, struct OBJGPU *);
    NV_STATUS (*__egmmemUnmapFrom__)(struct ExtendedGpuMemory *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__egmmemControl_Epilogue__)(struct ExtendedGpuMemory *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__egmmemControlLookup__)(struct ExtendedGpuMemory *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NV_STATUS (*__egmmemControl__)(struct ExtendedGpuMemory *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__egmmemUnmap__)(struct ExtendedGpuMemory *, CALL_CONTEXT *, RsCpuMapping *);
    NV_STATUS (*__egmmemGetMemInterMapParams__)(struct ExtendedGpuMemory *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__egmmemGetMemoryMappingDescriptor__)(struct ExtendedGpuMemory *, MEMORY_DESCRIPTOR **);
    NV_STATUS (*__egmmemControlFilter__)(struct ExtendedGpuMemory *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__egmmemControlSerialization_Prologue__)(struct ExtendedGpuMemory *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__egmmemCanCopy__)(struct ExtendedGpuMemory *);
    NV_STATUS (*__egmmemIsReady__)(struct ExtendedGpuMemory *, NvBool);
    NV_STATUS (*__egmmemCheckCopyPermissions__)(struct ExtendedGpuMemory *, struct OBJGPU *, struct Device *);
    void (*__egmmemPreDestruct__)(struct ExtendedGpuMemory *);
    NV_STATUS (*__egmmemIsDuplicate__)(struct ExtendedGpuMemory *, NvHandle, NvBool *);
    void (*__egmmemControlSerialization_Epilogue__)(struct ExtendedGpuMemory *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__egmmemMap__)(struct ExtendedGpuMemory *, CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, RsCpuMapping *);
    NvBool (*__egmmemAccessCallback__)(struct ExtendedGpuMemory *, struct RsClient *, void *, RsAccessRight);
};

#ifndef __NVOC_CLASS_ExtendedGpuMemory_TYPEDEF__
#define __NVOC_CLASS_ExtendedGpuMemory_TYPEDEF__
typedef struct ExtendedGpuMemory ExtendedGpuMemory;
#endif /* __NVOC_CLASS_ExtendedGpuMemory_TYPEDEF__ */

#ifndef __nvoc_class_id_ExtendedGpuMemory
#define __nvoc_class_id_ExtendedGpuMemory 0xeffa5c
#endif /* __nvoc_class_id_ExtendedGpuMemory */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_ExtendedGpuMemory;

#define __staticCast_ExtendedGpuMemory(pThis) \
    ((pThis)->__nvoc_pbase_ExtendedGpuMemory)

#ifdef __nvoc_egm_mem_h_disabled
#define __dynamicCast_ExtendedGpuMemory(pThis) ((ExtendedGpuMemory*)NULL)
#else //__nvoc_egm_mem_h_disabled
#define __dynamicCast_ExtendedGpuMemory(pThis) \
    ((ExtendedGpuMemory*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(ExtendedGpuMemory)))
#endif //__nvoc_egm_mem_h_disabled


NV_STATUS __nvoc_objCreateDynamic_ExtendedGpuMemory(ExtendedGpuMemory**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_ExtendedGpuMemory(ExtendedGpuMemory**, Dynamic*, NvU32, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_ExtendedGpuMemory(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_ExtendedGpuMemory((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define egmmemCheckMemInterUnmap(pMemory, bSubdeviceHandleProvided) egmmemCheckMemInterUnmap_DISPATCH(pMemory, bSubdeviceHandleProvided)
#define egmmemShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) egmmemShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define egmmemMapTo(pResource, pParams) egmmemMapTo_DISPATCH(pResource, pParams)
#define egmmemGetMapAddrSpace(pMemory, pCallContext, mapFlags, pAddrSpace) egmmemGetMapAddrSpace_DISPATCH(pMemory, pCallContext, mapFlags, pAddrSpace)
#define egmmemIsExportAllowed(pMemory) egmmemIsExportAllowed_DISPATCH(pMemory)
#define egmmemGetRefCount(pResource) egmmemGetRefCount_DISPATCH(pResource)
#define egmmemAddAdditionalDependants(pClient, pResource, pReference) egmmemAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define egmmemControl_Prologue(pResource, pCallContext, pParams) egmmemControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define egmmemIsGpuMapAllowed(pMemory, pGpu) egmmemIsGpuMapAllowed_DISPATCH(pMemory, pGpu)
#define egmmemUnmapFrom(pResource, pParams) egmmemUnmapFrom_DISPATCH(pResource, pParams)
#define egmmemControl_Epilogue(pResource, pCallContext, pParams) egmmemControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define egmmemControlLookup(pResource, pParams, ppEntry) egmmemControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define egmmemControl(pMemory, pCallContext, pParams) egmmemControl_DISPATCH(pMemory, pCallContext, pParams)
#define egmmemUnmap(pMemory, pCallContext, pCpuMapping) egmmemUnmap_DISPATCH(pMemory, pCallContext, pCpuMapping)
#define egmmemGetMemInterMapParams(pMemory, pParams) egmmemGetMemInterMapParams_DISPATCH(pMemory, pParams)
#define egmmemGetMemoryMappingDescriptor(pMemory, ppMemDesc) egmmemGetMemoryMappingDescriptor_DISPATCH(pMemory, ppMemDesc)
#define egmmemControlFilter(pResource, pCallContext, pParams) egmmemControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define egmmemControlSerialization_Prologue(pResource, pCallContext, pParams) egmmemControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define egmmemCanCopy(pStandardMemory) egmmemCanCopy_DISPATCH(pStandardMemory)
#define egmmemIsReady(pMemory, bCopyConstructorContext) egmmemIsReady_DISPATCH(pMemory, bCopyConstructorContext)
#define egmmemCheckCopyPermissions(pMemory, pDstGpu, pDstDevice) egmmemCheckCopyPermissions_DISPATCH(pMemory, pDstGpu, pDstDevice)
#define egmmemPreDestruct(pResource) egmmemPreDestruct_DISPATCH(pResource)
#define egmmemIsDuplicate(pMemory, hMemory, pDuplicate) egmmemIsDuplicate_DISPATCH(pMemory, hMemory, pDuplicate)
#define egmmemControlSerialization_Epilogue(pResource, pCallContext, pParams) egmmemControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define egmmemMap(pMemory, pCallContext, pParams, pCpuMapping) egmmemMap_DISPATCH(pMemory, pCallContext, pParams, pCpuMapping)
#define egmmemAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) egmmemAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
static inline NV_STATUS egmmemCheckMemInterUnmap_DISPATCH(struct ExtendedGpuMemory *pMemory, NvBool bSubdeviceHandleProvided) {
    return pMemory->__egmmemCheckMemInterUnmap__(pMemory, bSubdeviceHandleProvided);
}

static inline NvBool egmmemShareCallback_DISPATCH(struct ExtendedGpuMemory *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__egmmemShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS egmmemMapTo_DISPATCH(struct ExtendedGpuMemory *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__egmmemMapTo__(pResource, pParams);
}

static inline NV_STATUS egmmemGetMapAddrSpace_DISPATCH(struct ExtendedGpuMemory *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pMemory->__egmmemGetMapAddrSpace__(pMemory, pCallContext, mapFlags, pAddrSpace);
}

static inline NvBool egmmemIsExportAllowed_DISPATCH(struct ExtendedGpuMemory *pMemory) {
    return pMemory->__egmmemIsExportAllowed__(pMemory);
}

static inline NvU32 egmmemGetRefCount_DISPATCH(struct ExtendedGpuMemory *pResource) {
    return pResource->__egmmemGetRefCount__(pResource);
}

static inline void egmmemAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct ExtendedGpuMemory *pResource, RsResourceRef *pReference) {
    pResource->__egmmemAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS egmmemControl_Prologue_DISPATCH(struct ExtendedGpuMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__egmmemControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool egmmemIsGpuMapAllowed_DISPATCH(struct ExtendedGpuMemory *pMemory, struct OBJGPU *pGpu) {
    return pMemory->__egmmemIsGpuMapAllowed__(pMemory, pGpu);
}

static inline NV_STATUS egmmemUnmapFrom_DISPATCH(struct ExtendedGpuMemory *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__egmmemUnmapFrom__(pResource, pParams);
}

static inline void egmmemControl_Epilogue_DISPATCH(struct ExtendedGpuMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__egmmemControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS egmmemControlLookup_DISPATCH(struct ExtendedGpuMemory *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__egmmemControlLookup__(pResource, pParams, ppEntry);
}

static inline NV_STATUS egmmemControl_DISPATCH(struct ExtendedGpuMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pMemory->__egmmemControl__(pMemory, pCallContext, pParams);
}

static inline NV_STATUS egmmemUnmap_DISPATCH(struct ExtendedGpuMemory *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pMemory->__egmmemUnmap__(pMemory, pCallContext, pCpuMapping);
}

static inline NV_STATUS egmmemGetMemInterMapParams_DISPATCH(struct ExtendedGpuMemory *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pMemory->__egmmemGetMemInterMapParams__(pMemory, pParams);
}

static inline NV_STATUS egmmemGetMemoryMappingDescriptor_DISPATCH(struct ExtendedGpuMemory *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return pMemory->__egmmemGetMemoryMappingDescriptor__(pMemory, ppMemDesc);
}

static inline NV_STATUS egmmemControlFilter_DISPATCH(struct ExtendedGpuMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__egmmemControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS egmmemControlSerialization_Prologue_DISPATCH(struct ExtendedGpuMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__egmmemControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool egmmemCanCopy_DISPATCH(struct ExtendedGpuMemory *pStandardMemory) {
    return pStandardMemory->__egmmemCanCopy__(pStandardMemory);
}

static inline NV_STATUS egmmemIsReady_DISPATCH(struct ExtendedGpuMemory *pMemory, NvBool bCopyConstructorContext) {
    return pMemory->__egmmemIsReady__(pMemory, bCopyConstructorContext);
}

static inline NV_STATUS egmmemCheckCopyPermissions_DISPATCH(struct ExtendedGpuMemory *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return pMemory->__egmmemCheckCopyPermissions__(pMemory, pDstGpu, pDstDevice);
}

static inline void egmmemPreDestruct_DISPATCH(struct ExtendedGpuMemory *pResource) {
    pResource->__egmmemPreDestruct__(pResource);
}

static inline NV_STATUS egmmemIsDuplicate_DISPATCH(struct ExtendedGpuMemory *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return pMemory->__egmmemIsDuplicate__(pMemory, hMemory, pDuplicate);
}

static inline void egmmemControlSerialization_Epilogue_DISPATCH(struct ExtendedGpuMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__egmmemControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS egmmemMap_DISPATCH(struct ExtendedGpuMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pMemory->__egmmemMap__(pMemory, pCallContext, pParams, pCpuMapping);
}

static inline NvBool egmmemAccessCallback_DISPATCH(struct ExtendedGpuMemory *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__egmmemAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

NV_STATUS egmmemConstruct_IMPL(struct ExtendedGpuMemory *arg_pStandardMemory, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_egmmemConstruct(arg_pStandardMemory, arg_pCallContext, arg_pParams) egmmemConstruct_IMPL(arg_pStandardMemory, arg_pCallContext, arg_pParams)
NV_STATUS egmmemValidateParams_IMPL(struct OBJGPU *pGpu, NvHandle hClient, NV_MEMORY_ALLOCATION_PARAMS *pAllocData);

#define egmmemValidateParams(pGpu, hClient, pAllocData) egmmemValidateParams_IMPL(pGpu, hClient, pAllocData)
#undef PRIVATE_FIELD


NV_STATUS egmmemAllocResources(OBJGPU *pGpu, struct MemoryManager *pMemoryManager,
                               MEMORY_ALLOCATION_REQUEST *pAllocRequest, FB_ALLOC_INFO *pFbAllocInfo);

#endif // _EGM_MEMORY_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_EGM_MEM_NVOC_H_
