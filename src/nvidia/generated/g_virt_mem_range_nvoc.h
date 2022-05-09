#ifndef _G_VIRT_MEM_RANGE_NVOC_H_
#define _G_VIRT_MEM_RANGE_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_virt_mem_range_nvoc.h"

#ifndef _VIRT_MEM_RANGE_H_
#define _VIRT_MEM_RANGE_H_

#include "mem_mgr/virtual_mem.h"

/*!
 * Allocator for NV01_MEMORY_VIRTUAL class.
 *
 * Describes a range of typeless virtual memory memory.  Used as
 * a target space for RmMapMemoryDma.
 */
#ifdef NVOC_VIRT_MEM_RANGE_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct VirtualMemoryRange {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct VirtualMemory __nvoc_base_VirtualMemory;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct Memory *__nvoc_pbase_Memory;
    struct StandardMemory *__nvoc_pbase_StandardMemory;
    struct VirtualMemory *__nvoc_pbase_VirtualMemory;
    struct VirtualMemoryRange *__nvoc_pbase_VirtualMemoryRange;
    NV_STATUS (*__vmrangeCheckMemInterUnmap__)(struct VirtualMemoryRange *, NvBool);
    NV_STATUS (*__vmrangeMapTo__)(struct VirtualMemoryRange *, struct RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__vmrangeControl__)(struct VirtualMemoryRange *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__vmrangeGetMemInterMapParams__)(struct VirtualMemoryRange *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__vmrangeUnmap__)(struct VirtualMemoryRange *, CALL_CONTEXT *, RsCpuMapping *);
    NV_STATUS (*__vmrangeGetMapAddrSpace__)(struct VirtualMemoryRange *, CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    NvBool (*__vmrangeShareCallback__)(struct VirtualMemoryRange *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__vmrangeGetMemoryMappingDescriptor__)(struct VirtualMemoryRange *, MEMORY_DESCRIPTOR **);
    NV_STATUS (*__vmrangeControlFilter__)(struct VirtualMemoryRange *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvU32 (*__vmrangeGetRefCount__)(struct VirtualMemoryRange *);
    void (*__vmrangeAddAdditionalDependants__)(struct RsClient *, struct VirtualMemoryRange *, RsResourceRef *);
    NvBool (*__vmrangeCanCopy__)(struct VirtualMemoryRange *);
    NV_STATUS (*__vmrangeControl_Prologue__)(struct VirtualMemoryRange *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__vmrangeIsReady__)(struct VirtualMemoryRange *);
    NV_STATUS (*__vmrangeUnmapFrom__)(struct VirtualMemoryRange *, struct RS_RES_UNMAP_FROM_PARAMS *);
    NV_STATUS (*__vmrangeCheckCopyPermissions__)(struct VirtualMemoryRange *, struct OBJGPU *, NvHandle);
    void (*__vmrangePreDestruct__)(struct VirtualMemoryRange *);
    void (*__vmrangeControl_Epilogue__)(struct VirtualMemoryRange *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__vmrangeControlLookup__)(struct VirtualMemoryRange *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NV_STATUS (*__vmrangeMap__)(struct VirtualMemoryRange *, CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, RsCpuMapping *);
    NvBool (*__vmrangeAccessCallback__)(struct VirtualMemoryRange *, struct RsClient *, void *, RsAccessRight);
};

#ifndef __NVOC_CLASS_VirtualMemoryRange_TYPEDEF__
#define __NVOC_CLASS_VirtualMemoryRange_TYPEDEF__
typedef struct VirtualMemoryRange VirtualMemoryRange;
#endif /* __NVOC_CLASS_VirtualMemoryRange_TYPEDEF__ */

#ifndef __nvoc_class_id_VirtualMemoryRange
#define __nvoc_class_id_VirtualMemoryRange 0x7032c6
#endif /* __nvoc_class_id_VirtualMemoryRange */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_VirtualMemoryRange;

#define __staticCast_VirtualMemoryRange(pThis) \
    ((pThis)->__nvoc_pbase_VirtualMemoryRange)

#ifdef __nvoc_virt_mem_range_h_disabled
#define __dynamicCast_VirtualMemoryRange(pThis) ((VirtualMemoryRange*)NULL)
#else //__nvoc_virt_mem_range_h_disabled
#define __dynamicCast_VirtualMemoryRange(pThis) \
    ((VirtualMemoryRange*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(VirtualMemoryRange)))
#endif //__nvoc_virt_mem_range_h_disabled


NV_STATUS __nvoc_objCreateDynamic_VirtualMemoryRange(VirtualMemoryRange**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_VirtualMemoryRange(VirtualMemoryRange**, Dynamic*, NvU32, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_VirtualMemoryRange(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_VirtualMemoryRange((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define vmrangeCheckMemInterUnmap(pMemory, bSubdeviceHandleProvided) vmrangeCheckMemInterUnmap_DISPATCH(pMemory, bSubdeviceHandleProvided)
#define vmrangeMapTo(pVirtualMemory, pParams) vmrangeMapTo_DISPATCH(pVirtualMemory, pParams)
#define vmrangeControl(pMemory, pCallContext, pParams) vmrangeControl_DISPATCH(pMemory, pCallContext, pParams)
#define vmrangeGetMemInterMapParams(pMemory, pParams) vmrangeGetMemInterMapParams_DISPATCH(pMemory, pParams)
#define vmrangeUnmap(pMemory, pCallContext, pCpuMapping) vmrangeUnmap_DISPATCH(pMemory, pCallContext, pCpuMapping)
#define vmrangeGetMapAddrSpace(pMemory, pCallContext, mapFlags, pAddrSpace) vmrangeGetMapAddrSpace_DISPATCH(pMemory, pCallContext, mapFlags, pAddrSpace)
#define vmrangeShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) vmrangeShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define vmrangeGetMemoryMappingDescriptor(pMemory, ppMemDesc) vmrangeGetMemoryMappingDescriptor_DISPATCH(pMemory, ppMemDesc)
#define vmrangeControlFilter(pResource, pCallContext, pParams) vmrangeControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define vmrangeGetRefCount(pResource) vmrangeGetRefCount_DISPATCH(pResource)
#define vmrangeAddAdditionalDependants(pClient, pResource, pReference) vmrangeAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define vmrangeCanCopy(pStandardMemory) vmrangeCanCopy_DISPATCH(pStandardMemory)
#define vmrangeControl_Prologue(pResource, pCallContext, pParams) vmrangeControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define vmrangeIsReady(pMemory) vmrangeIsReady_DISPATCH(pMemory)
#define vmrangeUnmapFrom(pVirtualMemory, pParams) vmrangeUnmapFrom_DISPATCH(pVirtualMemory, pParams)
#define vmrangeCheckCopyPermissions(pMemory, pDstGpu, hDstClientNvBool) vmrangeCheckCopyPermissions_DISPATCH(pMemory, pDstGpu, hDstClientNvBool)
#define vmrangePreDestruct(pResource) vmrangePreDestruct_DISPATCH(pResource)
#define vmrangeControl_Epilogue(pResource, pCallContext, pParams) vmrangeControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define vmrangeControlLookup(pResource, pParams, ppEntry) vmrangeControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define vmrangeMap(pMemory, pCallContext, pParams, pCpuMapping) vmrangeMap_DISPATCH(pMemory, pCallContext, pParams, pCpuMapping)
#define vmrangeAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) vmrangeAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
static inline NV_STATUS vmrangeCheckMemInterUnmap_DISPATCH(struct VirtualMemoryRange *pMemory, NvBool bSubdeviceHandleProvided) {
    return pMemory->__vmrangeCheckMemInterUnmap__(pMemory, bSubdeviceHandleProvided);
}

static inline NV_STATUS vmrangeMapTo_DISPATCH(struct VirtualMemoryRange *pVirtualMemory, struct RS_RES_MAP_TO_PARAMS *pParams) {
    return pVirtualMemory->__vmrangeMapTo__(pVirtualMemory, pParams);
}

static inline NV_STATUS vmrangeControl_DISPATCH(struct VirtualMemoryRange *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pMemory->__vmrangeControl__(pMemory, pCallContext, pParams);
}

static inline NV_STATUS vmrangeGetMemInterMapParams_DISPATCH(struct VirtualMemoryRange *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pMemory->__vmrangeGetMemInterMapParams__(pMemory, pParams);
}

static inline NV_STATUS vmrangeUnmap_DISPATCH(struct VirtualMemoryRange *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pMemory->__vmrangeUnmap__(pMemory, pCallContext, pCpuMapping);
}

static inline NV_STATUS vmrangeGetMapAddrSpace_DISPATCH(struct VirtualMemoryRange *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pMemory->__vmrangeGetMapAddrSpace__(pMemory, pCallContext, mapFlags, pAddrSpace);
}

static inline NvBool vmrangeShareCallback_DISPATCH(struct VirtualMemoryRange *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__vmrangeShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS vmrangeGetMemoryMappingDescriptor_DISPATCH(struct VirtualMemoryRange *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return pMemory->__vmrangeGetMemoryMappingDescriptor__(pMemory, ppMemDesc);
}

static inline NV_STATUS vmrangeControlFilter_DISPATCH(struct VirtualMemoryRange *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__vmrangeControlFilter__(pResource, pCallContext, pParams);
}

static inline NvU32 vmrangeGetRefCount_DISPATCH(struct VirtualMemoryRange *pResource) {
    return pResource->__vmrangeGetRefCount__(pResource);
}

static inline void vmrangeAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct VirtualMemoryRange *pResource, RsResourceRef *pReference) {
    pResource->__vmrangeAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NvBool vmrangeCanCopy_DISPATCH(struct VirtualMemoryRange *pStandardMemory) {
    return pStandardMemory->__vmrangeCanCopy__(pStandardMemory);
}

static inline NV_STATUS vmrangeControl_Prologue_DISPATCH(struct VirtualMemoryRange *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__vmrangeControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS vmrangeIsReady_DISPATCH(struct VirtualMemoryRange *pMemory) {
    return pMemory->__vmrangeIsReady__(pMemory);
}

static inline NV_STATUS vmrangeUnmapFrom_DISPATCH(struct VirtualMemoryRange *pVirtualMemory, struct RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pVirtualMemory->__vmrangeUnmapFrom__(pVirtualMemory, pParams);
}

static inline NV_STATUS vmrangeCheckCopyPermissions_DISPATCH(struct VirtualMemoryRange *pMemory, struct OBJGPU *pDstGpu, NvHandle hDstClientNvBool) {
    return pMemory->__vmrangeCheckCopyPermissions__(pMemory, pDstGpu, hDstClientNvBool);
}

static inline void vmrangePreDestruct_DISPATCH(struct VirtualMemoryRange *pResource) {
    pResource->__vmrangePreDestruct__(pResource);
}

static inline void vmrangeControl_Epilogue_DISPATCH(struct VirtualMemoryRange *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__vmrangeControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS vmrangeControlLookup_DISPATCH(struct VirtualMemoryRange *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__vmrangeControlLookup__(pResource, pParams, ppEntry);
}

static inline NV_STATUS vmrangeMap_DISPATCH(struct VirtualMemoryRange *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pMemory->__vmrangeMap__(pMemory, pCallContext, pParams, pCpuMapping);
}

static inline NvBool vmrangeAccessCallback_DISPATCH(struct VirtualMemoryRange *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__vmrangeAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

NV_STATUS vmrangeConstruct_IMPL(struct VirtualMemoryRange *arg_pVmRange, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __nvoc_vmrangeConstruct(arg_pVmRange, arg_pCallContext, arg_pParams) vmrangeConstruct_IMPL(arg_pVmRange, arg_pCallContext, arg_pParams)
#undef PRIVATE_FIELD


#endif

#ifdef __cplusplus
} // extern "C"
#endif
#endif // _G_VIRT_MEM_RANGE_NVOC_H_
