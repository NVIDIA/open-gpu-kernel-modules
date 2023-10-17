#ifndef _G_MEM_FABRIC_NVOC_H_
#define _G_MEM_FABRIC_NVOC_H_
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

/******************************************************************************
 *
 *   Description:
 *       This file contains the functions for managing fabric memory
 *
 *****************************************************************************/

#include "g_mem_fabric_nvoc.h"

#ifndef _MEMORYFABRIC_H_
#define _MEMORYFABRIC_H_

#include "core/core.h"
#include "mem_mgr/mem.h"
#include "rmapi/resource.h"
#include "gpu/mem_mgr/mem_desc.h"

#include "ctrl/ctrl00f8.h"

// ****************************************************************************
//                          Type Definitions
// ****************************************************************************

#ifdef NVOC_MEM_FABRIC_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct MemoryFabric {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct Memory __nvoc_base_Memory;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct Memory *__nvoc_pbase_Memory;
    struct MemoryFabric *__nvoc_pbase_MemoryFabric;
    NvBool (*__memoryfabricCanCopy__)(struct MemoryFabric *);
    NV_STATUS (*__memoryfabricCopyConstruct__)(struct MemoryFabric *, CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
    NV_STATUS (*__memoryfabricControl__)(struct MemoryFabric *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__memoryfabricCtrlGetInfo__)(struct MemoryFabric *, NV00F8_CTRL_GET_INFO_PARAMS *);
    NV_STATUS (*__memoryfabricCtrlCmdDescribe__)(struct MemoryFabric *, NV00F8_CTRL_DESCRIBE_PARAMS *);
    NV_STATUS (*__memoryfabricCtrlAttachMem__)(struct MemoryFabric *, NV00F8_CTRL_ATTACH_MEM_PARAMS *);
    NV_STATUS (*__memoryfabricCtrlDetachMem__)(struct MemoryFabric *, NV00F8_CTRL_DETACH_MEM_PARAMS *);
    NV_STATUS (*__memoryfabricCtrlGetNumAttachedMem__)(struct MemoryFabric *, NV00F8_CTRL_GET_NUM_ATTACHED_MEM_PARAMS *);
    NV_STATUS (*__memoryfabricCtrlGetAttachedMem__)(struct MemoryFabric *, NV00F8_CTRL_GET_ATTACHED_MEM_PARAMS *);
    NV_STATUS (*__memoryfabricCtrlGetPageLevelInfo__)(struct MemoryFabric *, NV00F8_CTRL_GET_PAGE_LEVEL_INFO_PARAMS *);
    NV_STATUS (*__memoryfabricCheckMemInterUnmap__)(struct MemoryFabric *, NvBool);
    NvBool (*__memoryfabricShareCallback__)(struct MemoryFabric *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__memoryfabricMapTo__)(struct MemoryFabric *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__memoryfabricGetMapAddrSpace__)(struct MemoryFabric *, CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    NvBool (*__memoryfabricIsExportAllowed__)(struct MemoryFabric *);
    NvU32 (*__memoryfabricGetRefCount__)(struct MemoryFabric *);
    void (*__memoryfabricAddAdditionalDependants__)(struct RsClient *, struct MemoryFabric *, RsResourceRef *);
    NV_STATUS (*__memoryfabricControl_Prologue__)(struct MemoryFabric *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__memoryfabricIsGpuMapAllowed__)(struct MemoryFabric *, struct OBJGPU *);
    NV_STATUS (*__memoryfabricUnmapFrom__)(struct MemoryFabric *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__memoryfabricControl_Epilogue__)(struct MemoryFabric *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__memoryfabricControlLookup__)(struct MemoryFabric *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NV_STATUS (*__memoryfabricUnmap__)(struct MemoryFabric *, CALL_CONTEXT *, RsCpuMapping *);
    NV_STATUS (*__memoryfabricGetMemInterMapParams__)(struct MemoryFabric *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__memoryfabricGetMemoryMappingDescriptor__)(struct MemoryFabric *, MEMORY_DESCRIPTOR **);
    NV_STATUS (*__memoryfabricControlFilter__)(struct MemoryFabric *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__memoryfabricControlSerialization_Prologue__)(struct MemoryFabric *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__memoryfabricIsReady__)(struct MemoryFabric *, NvBool);
    NV_STATUS (*__memoryfabricCheckCopyPermissions__)(struct MemoryFabric *, struct OBJGPU *, struct Device *);
    void (*__memoryfabricPreDestruct__)(struct MemoryFabric *);
    NV_STATUS (*__memoryfabricIsDuplicate__)(struct MemoryFabric *, NvHandle, NvBool *);
    void (*__memoryfabricControlSerialization_Epilogue__)(struct MemoryFabric *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__memoryfabricMap__)(struct MemoryFabric *, CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, RsCpuMapping *);
    NvBool (*__memoryfabricAccessCallback__)(struct MemoryFabric *, struct RsClient *, void *, RsAccessRight);
};

#ifndef __NVOC_CLASS_MemoryFabric_TYPEDEF__
#define __NVOC_CLASS_MemoryFabric_TYPEDEF__
typedef struct MemoryFabric MemoryFabric;
#endif /* __NVOC_CLASS_MemoryFabric_TYPEDEF__ */

#ifndef __nvoc_class_id_MemoryFabric
#define __nvoc_class_id_MemoryFabric 0x127499
#endif /* __nvoc_class_id_MemoryFabric */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_MemoryFabric;

#define __staticCast_MemoryFabric(pThis) \
    ((pThis)->__nvoc_pbase_MemoryFabric)

#ifdef __nvoc_mem_fabric_h_disabled
#define __dynamicCast_MemoryFabric(pThis) ((MemoryFabric*)NULL)
#else //__nvoc_mem_fabric_h_disabled
#define __dynamicCast_MemoryFabric(pThis) \
    ((MemoryFabric*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(MemoryFabric)))
#endif //__nvoc_mem_fabric_h_disabled


NV_STATUS __nvoc_objCreateDynamic_MemoryFabric(MemoryFabric**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_MemoryFabric(MemoryFabric**, Dynamic*, NvU32, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_MemoryFabric(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_MemoryFabric((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define memoryfabricCanCopy(pMemoryFabric) memoryfabricCanCopy_DISPATCH(pMemoryFabric)
#define memoryfabricCopyConstruct(pMemoryFabric, pCallContext, pParams) memoryfabricCopyConstruct_DISPATCH(pMemoryFabric, pCallContext, pParams)
#define memoryfabricControl(pMemoryFabric, pCallContext, pParams) memoryfabricControl_DISPATCH(pMemoryFabric, pCallContext, pParams)
#define memoryfabricCtrlGetInfo(pMemoryFabric, pParams) memoryfabricCtrlGetInfo_DISPATCH(pMemoryFabric, pParams)
#define memoryfabricCtrlCmdDescribe(pMemoryFabric, pParams) memoryfabricCtrlCmdDescribe_DISPATCH(pMemoryFabric, pParams)
#define memoryfabricCtrlAttachMem(pMemoryFabric, pParams) memoryfabricCtrlAttachMem_DISPATCH(pMemoryFabric, pParams)
#define memoryfabricCtrlDetachMem(pMemoryFabric, pParams) memoryfabricCtrlDetachMem_DISPATCH(pMemoryFabric, pParams)
#define memoryfabricCtrlGetNumAttachedMem(pMemoryFabric, pParams) memoryfabricCtrlGetNumAttachedMem_DISPATCH(pMemoryFabric, pParams)
#define memoryfabricCtrlGetAttachedMem(pMemoryFabric, pParams) memoryfabricCtrlGetAttachedMem_DISPATCH(pMemoryFabric, pParams)
#define memoryfabricCtrlGetPageLevelInfo(pMemoryFabric, pParams) memoryfabricCtrlGetPageLevelInfo_DISPATCH(pMemoryFabric, pParams)
#define memoryfabricCheckMemInterUnmap(pMemory, bSubdeviceHandleProvided) memoryfabricCheckMemInterUnmap_DISPATCH(pMemory, bSubdeviceHandleProvided)
#define memoryfabricShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) memoryfabricShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define memoryfabricMapTo(pResource, pParams) memoryfabricMapTo_DISPATCH(pResource, pParams)
#define memoryfabricGetMapAddrSpace(pMemory, pCallContext, mapFlags, pAddrSpace) memoryfabricGetMapAddrSpace_DISPATCH(pMemory, pCallContext, mapFlags, pAddrSpace)
#define memoryfabricIsExportAllowed(pMemory) memoryfabricIsExportAllowed_DISPATCH(pMemory)
#define memoryfabricGetRefCount(pResource) memoryfabricGetRefCount_DISPATCH(pResource)
#define memoryfabricAddAdditionalDependants(pClient, pResource, pReference) memoryfabricAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define memoryfabricControl_Prologue(pResource, pCallContext, pParams) memoryfabricControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define memoryfabricIsGpuMapAllowed(pMemory, pGpu) memoryfabricIsGpuMapAllowed_DISPATCH(pMemory, pGpu)
#define memoryfabricUnmapFrom(pResource, pParams) memoryfabricUnmapFrom_DISPATCH(pResource, pParams)
#define memoryfabricControl_Epilogue(pResource, pCallContext, pParams) memoryfabricControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define memoryfabricControlLookup(pResource, pParams, ppEntry) memoryfabricControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define memoryfabricUnmap(pMemory, pCallContext, pCpuMapping) memoryfabricUnmap_DISPATCH(pMemory, pCallContext, pCpuMapping)
#define memoryfabricGetMemInterMapParams(pMemory, pParams) memoryfabricGetMemInterMapParams_DISPATCH(pMemory, pParams)
#define memoryfabricGetMemoryMappingDescriptor(pMemory, ppMemDesc) memoryfabricGetMemoryMappingDescriptor_DISPATCH(pMemory, ppMemDesc)
#define memoryfabricControlFilter(pResource, pCallContext, pParams) memoryfabricControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define memoryfabricControlSerialization_Prologue(pResource, pCallContext, pParams) memoryfabricControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define memoryfabricIsReady(pMemory, bCopyConstructorContext) memoryfabricIsReady_DISPATCH(pMemory, bCopyConstructorContext)
#define memoryfabricCheckCopyPermissions(pMemory, pDstGpu, pDstDevice) memoryfabricCheckCopyPermissions_DISPATCH(pMemory, pDstGpu, pDstDevice)
#define memoryfabricPreDestruct(pResource) memoryfabricPreDestruct_DISPATCH(pResource)
#define memoryfabricIsDuplicate(pMemory, hMemory, pDuplicate) memoryfabricIsDuplicate_DISPATCH(pMemory, hMemory, pDuplicate)
#define memoryfabricControlSerialization_Epilogue(pResource, pCallContext, pParams) memoryfabricControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define memoryfabricMap(pMemory, pCallContext, pParams, pCpuMapping) memoryfabricMap_DISPATCH(pMemory, pCallContext, pParams, pCpuMapping)
#define memoryfabricAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) memoryfabricAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
NvBool memoryfabricCanCopy_IMPL(struct MemoryFabric *pMemoryFabric);

static inline NvBool memoryfabricCanCopy_DISPATCH(struct MemoryFabric *pMemoryFabric) {
    return pMemoryFabric->__memoryfabricCanCopy__(pMemoryFabric);
}

NV_STATUS memoryfabricCopyConstruct_IMPL(struct MemoryFabric *pMemoryFabric, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);

static inline NV_STATUS memoryfabricCopyConstruct_DISPATCH(struct MemoryFabric *pMemoryFabric, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {
    return pMemoryFabric->__memoryfabricCopyConstruct__(pMemoryFabric, pCallContext, pParams);
}

NV_STATUS memoryfabricControl_IMPL(struct MemoryFabric *pMemoryFabric, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);

static inline NV_STATUS memoryfabricControl_DISPATCH(struct MemoryFabric *pMemoryFabric, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pMemoryFabric->__memoryfabricControl__(pMemoryFabric, pCallContext, pParams);
}

NV_STATUS memoryfabricCtrlGetInfo_IMPL(struct MemoryFabric *pMemoryFabric, NV00F8_CTRL_GET_INFO_PARAMS *pParams);

static inline NV_STATUS memoryfabricCtrlGetInfo_DISPATCH(struct MemoryFabric *pMemoryFabric, NV00F8_CTRL_GET_INFO_PARAMS *pParams) {
    return pMemoryFabric->__memoryfabricCtrlGetInfo__(pMemoryFabric, pParams);
}

NV_STATUS memoryfabricCtrlCmdDescribe_IMPL(struct MemoryFabric *pMemoryFabric, NV00F8_CTRL_DESCRIBE_PARAMS *pParams);

static inline NV_STATUS memoryfabricCtrlCmdDescribe_DISPATCH(struct MemoryFabric *pMemoryFabric, NV00F8_CTRL_DESCRIBE_PARAMS *pParams) {
    return pMemoryFabric->__memoryfabricCtrlCmdDescribe__(pMemoryFabric, pParams);
}

NV_STATUS memoryfabricCtrlAttachMem_IMPL(struct MemoryFabric *pMemoryFabric, NV00F8_CTRL_ATTACH_MEM_PARAMS *pParams);

static inline NV_STATUS memoryfabricCtrlAttachMem_DISPATCH(struct MemoryFabric *pMemoryFabric, NV00F8_CTRL_ATTACH_MEM_PARAMS *pParams) {
    return pMemoryFabric->__memoryfabricCtrlAttachMem__(pMemoryFabric, pParams);
}

NV_STATUS memoryfabricCtrlDetachMem_IMPL(struct MemoryFabric *pMemoryFabric, NV00F8_CTRL_DETACH_MEM_PARAMS *pParams);

static inline NV_STATUS memoryfabricCtrlDetachMem_DISPATCH(struct MemoryFabric *pMemoryFabric, NV00F8_CTRL_DETACH_MEM_PARAMS *pParams) {
    return pMemoryFabric->__memoryfabricCtrlDetachMem__(pMemoryFabric, pParams);
}

NV_STATUS memoryfabricCtrlGetNumAttachedMem_IMPL(struct MemoryFabric *pMemoryFabric, NV00F8_CTRL_GET_NUM_ATTACHED_MEM_PARAMS *pParams);

static inline NV_STATUS memoryfabricCtrlGetNumAttachedMem_DISPATCH(struct MemoryFabric *pMemoryFabric, NV00F8_CTRL_GET_NUM_ATTACHED_MEM_PARAMS *pParams) {
    return pMemoryFabric->__memoryfabricCtrlGetNumAttachedMem__(pMemoryFabric, pParams);
}

NV_STATUS memoryfabricCtrlGetAttachedMem_IMPL(struct MemoryFabric *pMemoryFabric, NV00F8_CTRL_GET_ATTACHED_MEM_PARAMS *pParams);

static inline NV_STATUS memoryfabricCtrlGetAttachedMem_DISPATCH(struct MemoryFabric *pMemoryFabric, NV00F8_CTRL_GET_ATTACHED_MEM_PARAMS *pParams) {
    return pMemoryFabric->__memoryfabricCtrlGetAttachedMem__(pMemoryFabric, pParams);
}

NV_STATUS memoryfabricCtrlGetPageLevelInfo_IMPL(struct MemoryFabric *pMemoryFabric, NV00F8_CTRL_GET_PAGE_LEVEL_INFO_PARAMS *pParams);

static inline NV_STATUS memoryfabricCtrlGetPageLevelInfo_DISPATCH(struct MemoryFabric *pMemoryFabric, NV00F8_CTRL_GET_PAGE_LEVEL_INFO_PARAMS *pParams) {
    return pMemoryFabric->__memoryfabricCtrlGetPageLevelInfo__(pMemoryFabric, pParams);
}

static inline NV_STATUS memoryfabricCheckMemInterUnmap_DISPATCH(struct MemoryFabric *pMemory, NvBool bSubdeviceHandleProvided) {
    return pMemory->__memoryfabricCheckMemInterUnmap__(pMemory, bSubdeviceHandleProvided);
}

static inline NvBool memoryfabricShareCallback_DISPATCH(struct MemoryFabric *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__memoryfabricShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS memoryfabricMapTo_DISPATCH(struct MemoryFabric *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__memoryfabricMapTo__(pResource, pParams);
}

static inline NV_STATUS memoryfabricGetMapAddrSpace_DISPATCH(struct MemoryFabric *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pMemory->__memoryfabricGetMapAddrSpace__(pMemory, pCallContext, mapFlags, pAddrSpace);
}

static inline NvBool memoryfabricIsExportAllowed_DISPATCH(struct MemoryFabric *pMemory) {
    return pMemory->__memoryfabricIsExportAllowed__(pMemory);
}

static inline NvU32 memoryfabricGetRefCount_DISPATCH(struct MemoryFabric *pResource) {
    return pResource->__memoryfabricGetRefCount__(pResource);
}

static inline void memoryfabricAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct MemoryFabric *pResource, RsResourceRef *pReference) {
    pResource->__memoryfabricAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS memoryfabricControl_Prologue_DISPATCH(struct MemoryFabric *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__memoryfabricControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool memoryfabricIsGpuMapAllowed_DISPATCH(struct MemoryFabric *pMemory, struct OBJGPU *pGpu) {
    return pMemory->__memoryfabricIsGpuMapAllowed__(pMemory, pGpu);
}

static inline NV_STATUS memoryfabricUnmapFrom_DISPATCH(struct MemoryFabric *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__memoryfabricUnmapFrom__(pResource, pParams);
}

static inline void memoryfabricControl_Epilogue_DISPATCH(struct MemoryFabric *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__memoryfabricControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS memoryfabricControlLookup_DISPATCH(struct MemoryFabric *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__memoryfabricControlLookup__(pResource, pParams, ppEntry);
}

static inline NV_STATUS memoryfabricUnmap_DISPATCH(struct MemoryFabric *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pMemory->__memoryfabricUnmap__(pMemory, pCallContext, pCpuMapping);
}

static inline NV_STATUS memoryfabricGetMemInterMapParams_DISPATCH(struct MemoryFabric *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pMemory->__memoryfabricGetMemInterMapParams__(pMemory, pParams);
}

static inline NV_STATUS memoryfabricGetMemoryMappingDescriptor_DISPATCH(struct MemoryFabric *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return pMemory->__memoryfabricGetMemoryMappingDescriptor__(pMemory, ppMemDesc);
}

static inline NV_STATUS memoryfabricControlFilter_DISPATCH(struct MemoryFabric *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__memoryfabricControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS memoryfabricControlSerialization_Prologue_DISPATCH(struct MemoryFabric *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__memoryfabricControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS memoryfabricIsReady_DISPATCH(struct MemoryFabric *pMemory, NvBool bCopyConstructorContext) {
    return pMemory->__memoryfabricIsReady__(pMemory, bCopyConstructorContext);
}

static inline NV_STATUS memoryfabricCheckCopyPermissions_DISPATCH(struct MemoryFabric *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return pMemory->__memoryfabricCheckCopyPermissions__(pMemory, pDstGpu, pDstDevice);
}

static inline void memoryfabricPreDestruct_DISPATCH(struct MemoryFabric *pResource) {
    pResource->__memoryfabricPreDestruct__(pResource);
}

static inline NV_STATUS memoryfabricIsDuplicate_DISPATCH(struct MemoryFabric *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return pMemory->__memoryfabricIsDuplicate__(pMemory, hMemory, pDuplicate);
}

static inline void memoryfabricControlSerialization_Epilogue_DISPATCH(struct MemoryFabric *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__memoryfabricControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS memoryfabricMap_DISPATCH(struct MemoryFabric *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pMemory->__memoryfabricMap__(pMemory, pCallContext, pParams, pCpuMapping);
}

static inline NvBool memoryfabricAccessCallback_DISPATCH(struct MemoryFabric *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__memoryfabricAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

NV_STATUS memoryfabricConstruct_IMPL(struct MemoryFabric *arg_pMemoryFabric, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_memoryfabricConstruct(arg_pMemoryFabric, arg_pCallContext, arg_pParams) memoryfabricConstruct_IMPL(arg_pMemoryFabric, arg_pCallContext, arg_pParams)
void memoryfabricDestruct_IMPL(struct MemoryFabric *pMemoryFabric);

#define __nvoc_memoryfabricDestruct(pMemoryFabric) memoryfabricDestruct_IMPL(pMemoryFabric)
#undef PRIVATE_FIELD


typedef struct
{
    MEMORY_DESCRIPTOR *pPhysMemDesc;
    NvU64    physMapOffset;
    NvU64    physMapLength;
    NvHandle hDupedPhysMem;
    NODE     node;
} FABRIC_ATTCH_MEM_INFO_NODE;

typedef struct
{
    // Tracks memory attached using NV00F8_CTRL_CMD_ATTACH_MEM
    PNODE pAttachMemInfoTree;

    NvHandle hDupedPhysMem;

    NV_PHYSICAL_MEMORY_ATTRS physAttrs;

    NvU32 allocFlags;
} FABRIC_MEMDESC_DATA;

#endif /* _MEMORYFABRIC_H_ */

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_MEM_FABRIC_NVOC_H_
