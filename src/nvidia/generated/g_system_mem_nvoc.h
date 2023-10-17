#ifndef _G_SYSTEM_MEM_NVOC_H_
#define _G_SYSTEM_MEM_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_system_mem_nvoc.h"

#ifndef _SYSTEM_MEMORY_H_
#define _SYSTEM_MEMORY_H_

#include "mem_mgr/standard_mem.h"
#include "gpu/mem_mgr/heap_base.h"

#ifdef NVOC_SYSTEM_MEM_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct SystemMemory {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct StandardMemory __nvoc_base_StandardMemory;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct Memory *__nvoc_pbase_Memory;
    struct StandardMemory *__nvoc_pbase_StandardMemory;
    struct SystemMemory *__nvoc_pbase_SystemMemory;
    NV_STATUS (*__sysmemCtrlCmdGetSurfaceNumPhysPages__)(struct SystemMemory *, NV003E_CTRL_GET_SURFACE_NUM_PHYS_PAGES_PARAMS *);
    NV_STATUS (*__sysmemCtrlCmdGetSurfacePhysPages__)(struct SystemMemory *, NV003E_CTRL_GET_SURFACE_PHYS_PAGES_PARAMS *);
    NV_STATUS (*__sysmemCheckMemInterUnmap__)(struct SystemMemory *, NvBool);
    NvBool (*__sysmemShareCallback__)(struct SystemMemory *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__sysmemMapTo__)(struct SystemMemory *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__sysmemGetMapAddrSpace__)(struct SystemMemory *, CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    NvBool (*__sysmemIsExportAllowed__)(struct SystemMemory *);
    NvU32 (*__sysmemGetRefCount__)(struct SystemMemory *);
    void (*__sysmemAddAdditionalDependants__)(struct RsClient *, struct SystemMemory *, RsResourceRef *);
    NV_STATUS (*__sysmemControl_Prologue__)(struct SystemMemory *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__sysmemIsGpuMapAllowed__)(struct SystemMemory *, struct OBJGPU *);
    NV_STATUS (*__sysmemUnmapFrom__)(struct SystemMemory *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__sysmemControl_Epilogue__)(struct SystemMemory *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__sysmemControlLookup__)(struct SystemMemory *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NV_STATUS (*__sysmemControl__)(struct SystemMemory *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__sysmemUnmap__)(struct SystemMemory *, CALL_CONTEXT *, RsCpuMapping *);
    NV_STATUS (*__sysmemGetMemInterMapParams__)(struct SystemMemory *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__sysmemGetMemoryMappingDescriptor__)(struct SystemMemory *, MEMORY_DESCRIPTOR **);
    NV_STATUS (*__sysmemControlFilter__)(struct SystemMemory *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__sysmemControlSerialization_Prologue__)(struct SystemMemory *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__sysmemCanCopy__)(struct SystemMemory *);
    NV_STATUS (*__sysmemIsReady__)(struct SystemMemory *, NvBool);
    NV_STATUS (*__sysmemCheckCopyPermissions__)(struct SystemMemory *, struct OBJGPU *, struct Device *);
    void (*__sysmemPreDestruct__)(struct SystemMemory *);
    NV_STATUS (*__sysmemIsDuplicate__)(struct SystemMemory *, NvHandle, NvBool *);
    void (*__sysmemControlSerialization_Epilogue__)(struct SystemMemory *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__sysmemMap__)(struct SystemMemory *, CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, RsCpuMapping *);
    NvBool (*__sysmemAccessCallback__)(struct SystemMemory *, struct RsClient *, void *, RsAccessRight);
};

#ifndef __NVOC_CLASS_SystemMemory_TYPEDEF__
#define __NVOC_CLASS_SystemMemory_TYPEDEF__
typedef struct SystemMemory SystemMemory;
#endif /* __NVOC_CLASS_SystemMemory_TYPEDEF__ */

#ifndef __nvoc_class_id_SystemMemory
#define __nvoc_class_id_SystemMemory 0x007a98
#endif /* __nvoc_class_id_SystemMemory */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_SystemMemory;

#define __staticCast_SystemMemory(pThis) \
    ((pThis)->__nvoc_pbase_SystemMemory)

#ifdef __nvoc_system_mem_h_disabled
#define __dynamicCast_SystemMemory(pThis) ((SystemMemory*)NULL)
#else //__nvoc_system_mem_h_disabled
#define __dynamicCast_SystemMemory(pThis) \
    ((SystemMemory*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(SystemMemory)))
#endif //__nvoc_system_mem_h_disabled


NV_STATUS __nvoc_objCreateDynamic_SystemMemory(SystemMemory**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_SystemMemory(SystemMemory**, Dynamic*, NvU32, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_SystemMemory(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_SystemMemory((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define sysmemCtrlCmdGetSurfaceNumPhysPages(pStandardMemory, pParams) sysmemCtrlCmdGetSurfaceNumPhysPages_DISPATCH(pStandardMemory, pParams)
#define sysmemCtrlCmdGetSurfacePhysPages(pStandardMemory, pParams) sysmemCtrlCmdGetSurfacePhysPages_DISPATCH(pStandardMemory, pParams)
#define sysmemCheckMemInterUnmap(pMemory, bSubdeviceHandleProvided) sysmemCheckMemInterUnmap_DISPATCH(pMemory, bSubdeviceHandleProvided)
#define sysmemShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) sysmemShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define sysmemMapTo(pResource, pParams) sysmemMapTo_DISPATCH(pResource, pParams)
#define sysmemGetMapAddrSpace(pMemory, pCallContext, mapFlags, pAddrSpace) sysmemGetMapAddrSpace_DISPATCH(pMemory, pCallContext, mapFlags, pAddrSpace)
#define sysmemIsExportAllowed(pMemory) sysmemIsExportAllowed_DISPATCH(pMemory)
#define sysmemGetRefCount(pResource) sysmemGetRefCount_DISPATCH(pResource)
#define sysmemAddAdditionalDependants(pClient, pResource, pReference) sysmemAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define sysmemControl_Prologue(pResource, pCallContext, pParams) sysmemControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define sysmemIsGpuMapAllowed(pMemory, pGpu) sysmemIsGpuMapAllowed_DISPATCH(pMemory, pGpu)
#define sysmemUnmapFrom(pResource, pParams) sysmemUnmapFrom_DISPATCH(pResource, pParams)
#define sysmemControl_Epilogue(pResource, pCallContext, pParams) sysmemControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define sysmemControlLookup(pResource, pParams, ppEntry) sysmemControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define sysmemControl(pMemory, pCallContext, pParams) sysmemControl_DISPATCH(pMemory, pCallContext, pParams)
#define sysmemUnmap(pMemory, pCallContext, pCpuMapping) sysmemUnmap_DISPATCH(pMemory, pCallContext, pCpuMapping)
#define sysmemGetMemInterMapParams(pMemory, pParams) sysmemGetMemInterMapParams_DISPATCH(pMemory, pParams)
#define sysmemGetMemoryMappingDescriptor(pMemory, ppMemDesc) sysmemGetMemoryMappingDescriptor_DISPATCH(pMemory, ppMemDesc)
#define sysmemControlFilter(pResource, pCallContext, pParams) sysmemControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define sysmemControlSerialization_Prologue(pResource, pCallContext, pParams) sysmemControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define sysmemCanCopy(pStandardMemory) sysmemCanCopy_DISPATCH(pStandardMemory)
#define sysmemIsReady(pMemory, bCopyConstructorContext) sysmemIsReady_DISPATCH(pMemory, bCopyConstructorContext)
#define sysmemCheckCopyPermissions(pMemory, pDstGpu, pDstDevice) sysmemCheckCopyPermissions_DISPATCH(pMemory, pDstGpu, pDstDevice)
#define sysmemPreDestruct(pResource) sysmemPreDestruct_DISPATCH(pResource)
#define sysmemIsDuplicate(pMemory, hMemory, pDuplicate) sysmemIsDuplicate_DISPATCH(pMemory, hMemory, pDuplicate)
#define sysmemControlSerialization_Epilogue(pResource, pCallContext, pParams) sysmemControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define sysmemMap(pMemory, pCallContext, pParams, pCpuMapping) sysmemMap_DISPATCH(pMemory, pCallContext, pParams, pCpuMapping)
#define sysmemAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) sysmemAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
NV_STATUS sysmemInitAllocRequest_HMM(struct OBJGPU *pGpu, struct SystemMemory *pSystemMemory, MEMORY_ALLOCATION_REQUEST *pAllocRequest);


#ifdef __nvoc_system_mem_h_disabled
static inline NV_STATUS sysmemInitAllocRequest(struct OBJGPU *pGpu, struct SystemMemory *pSystemMemory, MEMORY_ALLOCATION_REQUEST *pAllocRequest) {
    NV_ASSERT_FAILED_PRECOMP("SystemMemory was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_system_mem_h_disabled
#define sysmemInitAllocRequest(pGpu, pSystemMemory, pAllocRequest) sysmemInitAllocRequest_HMM(pGpu, pSystemMemory, pAllocRequest)
#endif //__nvoc_system_mem_h_disabled

#define sysmemInitAllocRequest_HAL(pGpu, pSystemMemory, pAllocRequest) sysmemInitAllocRequest(pGpu, pSystemMemory, pAllocRequest)

NV_STATUS sysmemCtrlCmdGetSurfaceNumPhysPages_IMPL(struct SystemMemory *pStandardMemory, NV003E_CTRL_GET_SURFACE_NUM_PHYS_PAGES_PARAMS *pParams);

static inline NV_STATUS sysmemCtrlCmdGetSurfaceNumPhysPages_DISPATCH(struct SystemMemory *pStandardMemory, NV003E_CTRL_GET_SURFACE_NUM_PHYS_PAGES_PARAMS *pParams) {
    return pStandardMemory->__sysmemCtrlCmdGetSurfaceNumPhysPages__(pStandardMemory, pParams);
}

NV_STATUS sysmemCtrlCmdGetSurfacePhysPages_IMPL(struct SystemMemory *pStandardMemory, NV003E_CTRL_GET_SURFACE_PHYS_PAGES_PARAMS *pParams);

static inline NV_STATUS sysmemCtrlCmdGetSurfacePhysPages_DISPATCH(struct SystemMemory *pStandardMemory, NV003E_CTRL_GET_SURFACE_PHYS_PAGES_PARAMS *pParams) {
    return pStandardMemory->__sysmemCtrlCmdGetSurfacePhysPages__(pStandardMemory, pParams);
}

static inline NV_STATUS sysmemCheckMemInterUnmap_DISPATCH(struct SystemMemory *pMemory, NvBool bSubdeviceHandleProvided) {
    return pMemory->__sysmemCheckMemInterUnmap__(pMemory, bSubdeviceHandleProvided);
}

static inline NvBool sysmemShareCallback_DISPATCH(struct SystemMemory *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__sysmemShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS sysmemMapTo_DISPATCH(struct SystemMemory *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__sysmemMapTo__(pResource, pParams);
}

static inline NV_STATUS sysmemGetMapAddrSpace_DISPATCH(struct SystemMemory *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pMemory->__sysmemGetMapAddrSpace__(pMemory, pCallContext, mapFlags, pAddrSpace);
}

static inline NvBool sysmemIsExportAllowed_DISPATCH(struct SystemMemory *pMemory) {
    return pMemory->__sysmemIsExportAllowed__(pMemory);
}

static inline NvU32 sysmemGetRefCount_DISPATCH(struct SystemMemory *pResource) {
    return pResource->__sysmemGetRefCount__(pResource);
}

static inline void sysmemAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct SystemMemory *pResource, RsResourceRef *pReference) {
    pResource->__sysmemAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS sysmemControl_Prologue_DISPATCH(struct SystemMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__sysmemControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool sysmemIsGpuMapAllowed_DISPATCH(struct SystemMemory *pMemory, struct OBJGPU *pGpu) {
    return pMemory->__sysmemIsGpuMapAllowed__(pMemory, pGpu);
}

static inline NV_STATUS sysmemUnmapFrom_DISPATCH(struct SystemMemory *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__sysmemUnmapFrom__(pResource, pParams);
}

static inline void sysmemControl_Epilogue_DISPATCH(struct SystemMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__sysmemControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS sysmemControlLookup_DISPATCH(struct SystemMemory *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__sysmemControlLookup__(pResource, pParams, ppEntry);
}

static inline NV_STATUS sysmemControl_DISPATCH(struct SystemMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pMemory->__sysmemControl__(pMemory, pCallContext, pParams);
}

static inline NV_STATUS sysmemUnmap_DISPATCH(struct SystemMemory *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pMemory->__sysmemUnmap__(pMemory, pCallContext, pCpuMapping);
}

static inline NV_STATUS sysmemGetMemInterMapParams_DISPATCH(struct SystemMemory *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pMemory->__sysmemGetMemInterMapParams__(pMemory, pParams);
}

static inline NV_STATUS sysmemGetMemoryMappingDescriptor_DISPATCH(struct SystemMemory *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return pMemory->__sysmemGetMemoryMappingDescriptor__(pMemory, ppMemDesc);
}

static inline NV_STATUS sysmemControlFilter_DISPATCH(struct SystemMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__sysmemControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS sysmemControlSerialization_Prologue_DISPATCH(struct SystemMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__sysmemControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool sysmemCanCopy_DISPATCH(struct SystemMemory *pStandardMemory) {
    return pStandardMemory->__sysmemCanCopy__(pStandardMemory);
}

static inline NV_STATUS sysmemIsReady_DISPATCH(struct SystemMemory *pMemory, NvBool bCopyConstructorContext) {
    return pMemory->__sysmemIsReady__(pMemory, bCopyConstructorContext);
}

static inline NV_STATUS sysmemCheckCopyPermissions_DISPATCH(struct SystemMemory *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return pMemory->__sysmemCheckCopyPermissions__(pMemory, pDstGpu, pDstDevice);
}

static inline void sysmemPreDestruct_DISPATCH(struct SystemMemory *pResource) {
    pResource->__sysmemPreDestruct__(pResource);
}

static inline NV_STATUS sysmemIsDuplicate_DISPATCH(struct SystemMemory *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return pMemory->__sysmemIsDuplicate__(pMemory, hMemory, pDuplicate);
}

static inline void sysmemControlSerialization_Epilogue_DISPATCH(struct SystemMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__sysmemControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS sysmemMap_DISPATCH(struct SystemMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pMemory->__sysmemMap__(pMemory, pCallContext, pParams, pCpuMapping);
}

static inline NvBool sysmemAccessCallback_DISPATCH(struct SystemMemory *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__sysmemAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

NV_STATUS sysmemConstruct_IMPL(struct SystemMemory *arg_pStandardMemory, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_sysmemConstruct(arg_pStandardMemory, arg_pCallContext, arg_pParams) sysmemConstruct_IMPL(arg_pStandardMemory, arg_pCallContext, arg_pParams)
#undef PRIVATE_FIELD


NV_STATUS sysmemAllocResources(OBJGPU *pGpu, struct MemoryManager *pMemoryManager,
                               MEMORY_ALLOCATION_REQUEST *pAllocRequest, FB_ALLOC_INFO *pFbAllocInfo,
                               struct SystemMemory *pSystemMemory);

#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_SYSTEM_MEM_NVOC_H_
