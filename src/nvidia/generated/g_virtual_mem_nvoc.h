#ifndef _G_VIRTUAL_MEM_NVOC_H_
#define _G_VIRTUAL_MEM_NVOC_H_
#include "nvoc/runtime.h"

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

#include "g_virtual_mem_nvoc.h"

#ifndef _VIRTUAL_MEMORY_H_
#define _VIRTUAL_MEMORY_H_

#include "mem_mgr/standard_mem.h"
#include "gpu/mem_mgr/heap_base.h"

/*!
 * Allocator for NV50_MEMORY_VIRTUAL class.
 *
 * Describes a range of typed virtual memory.  Used as
 * a target space for RmMapMemoryDma.
 *
 * The NV01_MEMORY_VIRTUAL subclass class is typeless.
 */
#ifdef NVOC_VIRTUAL_MEM_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct VirtualMemory {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct StandardMemory __nvoc_base_StandardMemory;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct Memory *__nvoc_pbase_Memory;
    struct StandardMemory *__nvoc_pbase_StandardMemory;
    struct VirtualMemory *__nvoc_pbase_VirtualMemory;
    NV_STATUS (*__virtmemMapTo__)(struct VirtualMemory *, struct RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__virtmemUnmapFrom__)(struct VirtualMemory *, struct RS_RES_UNMAP_FROM_PARAMS *);
    NV_STATUS (*__virtmemCheckMemInterUnmap__)(struct VirtualMemory *, NvBool);
    NvBool (*__virtmemShareCallback__)(struct VirtualMemory *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__virtmemGetMapAddrSpace__)(struct VirtualMemory *, CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    NvBool (*__virtmemIsExportAllowed__)(struct VirtualMemory *);
    NvU32 (*__virtmemGetRefCount__)(struct VirtualMemory *);
    void (*__virtmemAddAdditionalDependants__)(struct RsClient *, struct VirtualMemory *, RsResourceRef *);
    NV_STATUS (*__virtmemControl_Prologue__)(struct VirtualMemory *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__virtmemIsGpuMapAllowed__)(struct VirtualMemory *, struct OBJGPU *);
    void (*__virtmemControl_Epilogue__)(struct VirtualMemory *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__virtmemControlLookup__)(struct VirtualMemory *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NV_STATUS (*__virtmemControl__)(struct VirtualMemory *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__virtmemUnmap__)(struct VirtualMemory *, CALL_CONTEXT *, RsCpuMapping *);
    NV_STATUS (*__virtmemGetMemInterMapParams__)(struct VirtualMemory *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__virtmemGetMemoryMappingDescriptor__)(struct VirtualMemory *, MEMORY_DESCRIPTOR **);
    NV_STATUS (*__virtmemControlFilter__)(struct VirtualMemory *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__virtmemControlSerialization_Prologue__)(struct VirtualMemory *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__virtmemCanCopy__)(struct VirtualMemory *);
    NV_STATUS (*__virtmemIsReady__)(struct VirtualMemory *, NvBool);
    NV_STATUS (*__virtmemCheckCopyPermissions__)(struct VirtualMemory *, struct OBJGPU *, struct Device *);
    void (*__virtmemPreDestruct__)(struct VirtualMemory *);
    NV_STATUS (*__virtmemIsDuplicate__)(struct VirtualMemory *, NvHandle, NvBool *);
    void (*__virtmemControlSerialization_Epilogue__)(struct VirtualMemory *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__virtmemMap__)(struct VirtualMemory *, CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, RsCpuMapping *);
    NvBool (*__virtmemAccessCallback__)(struct VirtualMemory *, struct RsClient *, void *, RsAccessRight);
    NvHandle hVASpace;
    NvBool bAllowUnicastMapping;
    NvBool bReserveVaOnAlloc;
    NvBool bFlaVAS;
    NvBool bRpcAlloc;
    NODE *pDmaMappingList;
    NvBool bOptimizePageTableMempoolUsage;
};

#ifndef __NVOC_CLASS_VirtualMemory_TYPEDEF__
#define __NVOC_CLASS_VirtualMemory_TYPEDEF__
typedef struct VirtualMemory VirtualMemory;
#endif /* __NVOC_CLASS_VirtualMemory_TYPEDEF__ */

#ifndef __nvoc_class_id_VirtualMemory
#define __nvoc_class_id_VirtualMemory 0x2aea5c
#endif /* __nvoc_class_id_VirtualMemory */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_VirtualMemory;

#define __staticCast_VirtualMemory(pThis) \
    ((pThis)->__nvoc_pbase_VirtualMemory)

#ifdef __nvoc_virtual_mem_h_disabled
#define __dynamicCast_VirtualMemory(pThis) ((VirtualMemory*)NULL)
#else //__nvoc_virtual_mem_h_disabled
#define __dynamicCast_VirtualMemory(pThis) \
    ((VirtualMemory*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(VirtualMemory)))
#endif //__nvoc_virtual_mem_h_disabled


NV_STATUS __nvoc_objCreateDynamic_VirtualMemory(VirtualMemory**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_VirtualMemory(VirtualMemory**, Dynamic*, NvU32, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_VirtualMemory(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_VirtualMemory((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define virtmemMapTo(pVirtualMemory, pParams) virtmemMapTo_DISPATCH(pVirtualMemory, pParams)
#define virtmemUnmapFrom(pVirtualMemory, pParams) virtmemUnmapFrom_DISPATCH(pVirtualMemory, pParams)
#define virtmemCheckMemInterUnmap(pMemory, bSubdeviceHandleProvided) virtmemCheckMemInterUnmap_DISPATCH(pMemory, bSubdeviceHandleProvided)
#define virtmemShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) virtmemShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define virtmemGetMapAddrSpace(pMemory, pCallContext, mapFlags, pAddrSpace) virtmemGetMapAddrSpace_DISPATCH(pMemory, pCallContext, mapFlags, pAddrSpace)
#define virtmemIsExportAllowed(pMemory) virtmemIsExportAllowed_DISPATCH(pMemory)
#define virtmemGetRefCount(pResource) virtmemGetRefCount_DISPATCH(pResource)
#define virtmemAddAdditionalDependants(pClient, pResource, pReference) virtmemAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define virtmemControl_Prologue(pResource, pCallContext, pParams) virtmemControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define virtmemIsGpuMapAllowed(pMemory, pGpu) virtmemIsGpuMapAllowed_DISPATCH(pMemory, pGpu)
#define virtmemControl_Epilogue(pResource, pCallContext, pParams) virtmemControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define virtmemControlLookup(pResource, pParams, ppEntry) virtmemControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define virtmemControl(pMemory, pCallContext, pParams) virtmemControl_DISPATCH(pMemory, pCallContext, pParams)
#define virtmemUnmap(pMemory, pCallContext, pCpuMapping) virtmemUnmap_DISPATCH(pMemory, pCallContext, pCpuMapping)
#define virtmemGetMemInterMapParams(pMemory, pParams) virtmemGetMemInterMapParams_DISPATCH(pMemory, pParams)
#define virtmemGetMemoryMappingDescriptor(pMemory, ppMemDesc) virtmemGetMemoryMappingDescriptor_DISPATCH(pMemory, ppMemDesc)
#define virtmemControlFilter(pResource, pCallContext, pParams) virtmemControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define virtmemControlSerialization_Prologue(pResource, pCallContext, pParams) virtmemControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define virtmemCanCopy(pStandardMemory) virtmemCanCopy_DISPATCH(pStandardMemory)
#define virtmemIsReady(pMemory, bCopyConstructorContext) virtmemIsReady_DISPATCH(pMemory, bCopyConstructorContext)
#define virtmemCheckCopyPermissions(pMemory, pDstGpu, pDstDevice) virtmemCheckCopyPermissions_DISPATCH(pMemory, pDstGpu, pDstDevice)
#define virtmemPreDestruct(pResource) virtmemPreDestruct_DISPATCH(pResource)
#define virtmemIsDuplicate(pMemory, hMemory, pDuplicate) virtmemIsDuplicate_DISPATCH(pMemory, hMemory, pDuplicate)
#define virtmemControlSerialization_Epilogue(pResource, pCallContext, pParams) virtmemControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define virtmemMap(pMemory, pCallContext, pParams, pCpuMapping) virtmemMap_DISPATCH(pMemory, pCallContext, pParams, pCpuMapping)
#define virtmemAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) virtmemAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
NV_STATUS virtmemMapTo_IMPL(struct VirtualMemory *pVirtualMemory, struct RS_RES_MAP_TO_PARAMS *pParams);

static inline NV_STATUS virtmemMapTo_DISPATCH(struct VirtualMemory *pVirtualMemory, struct RS_RES_MAP_TO_PARAMS *pParams) {
    return pVirtualMemory->__virtmemMapTo__(pVirtualMemory, pParams);
}

NV_STATUS virtmemUnmapFrom_IMPL(struct VirtualMemory *pVirtualMemory, struct RS_RES_UNMAP_FROM_PARAMS *pParams);

static inline NV_STATUS virtmemUnmapFrom_DISPATCH(struct VirtualMemory *pVirtualMemory, struct RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pVirtualMemory->__virtmemUnmapFrom__(pVirtualMemory, pParams);
}

static inline NV_STATUS virtmemCheckMemInterUnmap_DISPATCH(struct VirtualMemory *pMemory, NvBool bSubdeviceHandleProvided) {
    return pMemory->__virtmemCheckMemInterUnmap__(pMemory, bSubdeviceHandleProvided);
}

static inline NvBool virtmemShareCallback_DISPATCH(struct VirtualMemory *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__virtmemShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS virtmemGetMapAddrSpace_DISPATCH(struct VirtualMemory *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pMemory->__virtmemGetMapAddrSpace__(pMemory, pCallContext, mapFlags, pAddrSpace);
}

static inline NvBool virtmemIsExportAllowed_DISPATCH(struct VirtualMemory *pMemory) {
    return pMemory->__virtmemIsExportAllowed__(pMemory);
}

static inline NvU32 virtmemGetRefCount_DISPATCH(struct VirtualMemory *pResource) {
    return pResource->__virtmemGetRefCount__(pResource);
}

static inline void virtmemAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct VirtualMemory *pResource, RsResourceRef *pReference) {
    pResource->__virtmemAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS virtmemControl_Prologue_DISPATCH(struct VirtualMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__virtmemControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool virtmemIsGpuMapAllowed_DISPATCH(struct VirtualMemory *pMemory, struct OBJGPU *pGpu) {
    return pMemory->__virtmemIsGpuMapAllowed__(pMemory, pGpu);
}

static inline void virtmemControl_Epilogue_DISPATCH(struct VirtualMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__virtmemControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS virtmemControlLookup_DISPATCH(struct VirtualMemory *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__virtmemControlLookup__(pResource, pParams, ppEntry);
}

static inline NV_STATUS virtmemControl_DISPATCH(struct VirtualMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pMemory->__virtmemControl__(pMemory, pCallContext, pParams);
}

static inline NV_STATUS virtmemUnmap_DISPATCH(struct VirtualMemory *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pMemory->__virtmemUnmap__(pMemory, pCallContext, pCpuMapping);
}

static inline NV_STATUS virtmemGetMemInterMapParams_DISPATCH(struct VirtualMemory *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pMemory->__virtmemGetMemInterMapParams__(pMemory, pParams);
}

static inline NV_STATUS virtmemGetMemoryMappingDescriptor_DISPATCH(struct VirtualMemory *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return pMemory->__virtmemGetMemoryMappingDescriptor__(pMemory, ppMemDesc);
}

static inline NV_STATUS virtmemControlFilter_DISPATCH(struct VirtualMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__virtmemControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS virtmemControlSerialization_Prologue_DISPATCH(struct VirtualMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__virtmemControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool virtmemCanCopy_DISPATCH(struct VirtualMemory *pStandardMemory) {
    return pStandardMemory->__virtmemCanCopy__(pStandardMemory);
}

static inline NV_STATUS virtmemIsReady_DISPATCH(struct VirtualMemory *pMemory, NvBool bCopyConstructorContext) {
    return pMemory->__virtmemIsReady__(pMemory, bCopyConstructorContext);
}

static inline NV_STATUS virtmemCheckCopyPermissions_DISPATCH(struct VirtualMemory *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return pMemory->__virtmemCheckCopyPermissions__(pMemory, pDstGpu, pDstDevice);
}

static inline void virtmemPreDestruct_DISPATCH(struct VirtualMemory *pResource) {
    pResource->__virtmemPreDestruct__(pResource);
}

static inline NV_STATUS virtmemIsDuplicate_DISPATCH(struct VirtualMemory *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return pMemory->__virtmemIsDuplicate__(pMemory, hMemory, pDuplicate);
}

static inline void virtmemControlSerialization_Epilogue_DISPATCH(struct VirtualMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__virtmemControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS virtmemMap_DISPATCH(struct VirtualMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pMemory->__virtmemMap__(pMemory, pCallContext, pParams, pCpuMapping);
}

static inline NvBool virtmemAccessCallback_DISPATCH(struct VirtualMemory *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__virtmemAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

NV_STATUS virtmemConstruct_IMPL(struct VirtualMemory *arg_pVirtualMemory, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_virtmemConstruct(arg_pVirtualMemory, arg_pCallContext, arg_pParams) virtmemConstruct_IMPL(arg_pVirtualMemory, arg_pCallContext, arg_pParams)
void virtmemDestruct_IMPL(struct VirtualMemory *pVirtualMemory);

#define __nvoc_virtmemDestruct(pVirtualMemory) virtmemDestruct_IMPL(pVirtualMemory)
NV_STATUS virtmemReserveMempool_IMPL(struct VirtualMemory *pVirtualMemory, struct OBJGPU *arg0, struct Device *pDevice, NvU64 size, NvU64 pageSizeMask);

#ifdef __nvoc_virtual_mem_h_disabled
static inline NV_STATUS virtmemReserveMempool(struct VirtualMemory *pVirtualMemory, struct OBJGPU *arg0, struct Device *pDevice, NvU64 size, NvU64 pageSizeMask) {
    NV_ASSERT_FAILED_PRECOMP("VirtualMemory was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_virtual_mem_h_disabled
#define virtmemReserveMempool(pVirtualMemory, arg0, pDevice, size, pageSizeMask) virtmemReserveMempool_IMPL(pVirtualMemory, arg0, pDevice, size, pageSizeMask)
#endif //__nvoc_virtual_mem_h_disabled

NvBool virtmemMatchesVASpace_IMPL(struct VirtualMemory *pVirtualMemory, NvHandle hClient, NvHandle hVASpace);

#ifdef __nvoc_virtual_mem_h_disabled
static inline NvBool virtmemMatchesVASpace(struct VirtualMemory *pVirtualMemory, NvHandle hClient, NvHandle hVASpace) {
    NV_ASSERT_FAILED_PRECOMP("VirtualMemory was disabled!");
    return NV_FALSE;
}
#else //__nvoc_virtual_mem_h_disabled
#define virtmemMatchesVASpace(pVirtualMemory, hClient, hVASpace) virtmemMatchesVASpace_IMPL(pVirtualMemory, hClient, hVASpace)
#endif //__nvoc_virtual_mem_h_disabled

NV_STATUS virtmemGetByHandleAndDevice_IMPL(struct RsClient *pClient, NvHandle hMemory, NvHandle hDevice, struct VirtualMemory **ppVirtualMemory);

#define virtmemGetByHandleAndDevice(pClient, hMemory, hDevice, ppVirtualMemory) virtmemGetByHandleAndDevice_IMPL(pClient, hMemory, hDevice, ppVirtualMemory)
void virtmemGetAddressAndSize_IMPL(struct VirtualMemory *arg0, NvU64 *pVAddr, NvU64 *pSize);

#define virtmemGetAddressAndSize(arg0, pVAddr, pSize) virtmemGetAddressAndSize_IMPL(arg0, pVAddr, pSize)
#undef PRIVATE_FIELD


NV_STATUS virtmemAllocResources(OBJGPU *pGpu, struct MemoryManager *pMemoryManager, MEMORY_ALLOCATION_REQUEST *pAllocRequest,
                                FB_ALLOC_INFO *pFbAllocInfo);

#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_VIRTUAL_MEM_NVOC_H_
