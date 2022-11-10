#ifndef _G_MEM_MULTICAST_FABRIC_NVOC_H_
#define _G_MEM_MULTICAST_FABRIC_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 *       This file contains the functions for managing multicast fabric memory
 *
 *****************************************************************************/

#include "g_mem_multicast_fabric_nvoc.h"

#ifndef _MEMORYMULTICASTFABRIC_H_
#define _MEMORYMULTICASTFABRIC_H_



#include "mem_mgr/mem.h"
#include "rmapi/resource.h"

#include "class/cl00fd.h"
#include "ctrl/ctrl00fd.h"
#include "ctrl/ctrl2080/ctrl2080nvlink.h"

// ****************************************************************************
//                          Type Definitions
// ****************************************************************************
typedef struct mem_multicast_fabric_attach_mem_info
{
    NvU64              offset;
    NvU64              mapOffset;
    NvU64              mapLength;
    NvU32              flags;
    OBJGPU            *pGpu;
    MEMORY_DESCRIPTOR *pPhysMemDesc;
    NvHandle           hDupedPhysMem;
    NvU64              gpuProbeHandle;
} MEM_MULTICAST_FABRIC_ATTACH_MEM_INFO;

typedef struct mem_multicast_fabric_client_info
{
    void   *pOsEvent;
    struct Memory *pMemory;
} MEM_MULTICAST_FABRIC_CLIENT_INFO;

typedef struct mem_multicast_fabric_gpu_os_info
{
    void *gpuOsInfo;
} MEM_MULTICAST_FABRIC_GPU_OS_INFO;

MAKE_LIST(MemMulticastFabricAttachMemInfoList, MEM_MULTICAST_FABRIC_ATTACH_MEM_INFO);

MAKE_LIST(MemMulticastFabricClientInfoList, MEM_MULTICAST_FABRIC_CLIENT_INFO);

MAKE_LIST(MemMulticastFabricGpuOsInfoList, MEM_MULTICAST_FABRIC_GPU_OS_INFO);

typedef enum
{
    MEM_MULTICAST_FABRIC_TEAM_SETUP_REQUEST = 0,
    MEM_MULTICAST_FABRIC_TEAM_RELEASE_REQUEST,
} MEM_MULTICAST_FABRIC_REQUEST_TYPE;

typedef struct mem_multicast_fabric_descriptor
{
    // Refcount to keep this descriptor alive
    NvU64 refCount;

    // List of clients waiting on this object to be ready
    MemMulticastFabricClientInfoList waitingClientsList;

    // List of attach mem info of GPUs already attached to the mutlicast object
    MemMulticastFabricAttachMemInfoList attachMemInfoList;

    // Mask representing the list of attached GPUs
    NvU32 attachedGpusMask;

    // List of GPU OS Info
    MemMulticastFabricGpuOsInfoList gpuOsInfoList;

    // Boolean to be set when pMemDesc is installed
    NvBool bMemdescInstalled;

    // Memory descriptor associated with the multicast object
    MEMORY_DESCRIPTOR *pMemDesc;

    // Unique handle assigned for the multicast team by FM
    NvU64 mcTeamHandle;

    // Status of the multicast team
    NV_STATUS mcTeamStatus;

    // Boolean to be set when an Inband request has been sent to FM and is currently in progress
    NvBool bInbandReqInProgress;

    // Request Id associated with the Inband request in progress when bInbandReqSent is set to true
    NvU64 inbandReqId;

    // Alignment for the multicast FLA allocation
    NvU64 alignment;

    // Multicast FLA allocation size
    NvU64 allocSize;

    // Page size for the multicast FLA
    NvU32 pageSize;

    // Multicast FLA allocation flags
    NvU32 allocFlags;

    // Max. number of unique GPUs associated with the multicast object
    NvU32 numMaxGpus;

    // No. of unique GPUs currently attached to the multicast object
    NvU32 numAttachedGpus;
} MEM_MULTICAST_FABRIC_DESCRIPTOR;

#ifdef NVOC_MEM_MULTICAST_FABRIC_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct MemoryMulticastFabric {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct Memory __nvoc_base_Memory;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct Memory *__nvoc_pbase_Memory;
    struct MemoryMulticastFabric *__nvoc_pbase_MemoryMulticastFabric;
    NvBool (*__memorymulticastfabricCanCopy__)(struct MemoryMulticastFabric *);
    NV_STATUS (*__memorymulticastfabricCopyConstruct__)(struct MemoryMulticastFabric *, CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
    NV_STATUS (*__memorymulticastfabricIsReady__)(struct MemoryMulticastFabric *, NvBool);
    NV_STATUS (*__memorymulticastfabricControl__)(struct MemoryMulticastFabric *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__memorymulticastfabricControl_Prologue__)(struct MemoryMulticastFabric *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__memorymulticastfabricIsGpuMapAllowed__)(struct MemoryMulticastFabric *, struct OBJGPU *);
    NV_STATUS (*__memorymulticastfabricGetMapAddrSpace__)(struct MemoryMulticastFabric *, CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    NV_STATUS (*__memorymulticastfabricCtrlGetInfo__)(struct MemoryMulticastFabric *, NV00FD_CTRL_GET_INFO_PARAMS *);
    NV_STATUS (*__memorymulticastfabricCtrlAttachMem__)(struct MemoryMulticastFabric *, NV00FD_CTRL_ATTACH_MEM_PARAMS *);
    NV_STATUS (*__memorymulticastfabricCtrlRegisterEvent__)(struct MemoryMulticastFabric *, NV00FD_CTRL_REGISTER_EVENT_PARAMS *);
    NV_STATUS (*__memorymulticastfabricCheckMemInterUnmap__)(struct MemoryMulticastFabric *, NvBool);
    NV_STATUS (*__memorymulticastfabricUnmap__)(struct MemoryMulticastFabric *, CALL_CONTEXT *, RsCpuMapping *);
    NV_STATUS (*__memorymulticastfabricGetMemInterMapParams__)(struct MemoryMulticastFabric *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__memorymulticastfabricGetMemoryMappingDescriptor__)(struct MemoryMulticastFabric *, MEMORY_DESCRIPTOR **);
    NvBool (*__memorymulticastfabricShareCallback__)(struct MemoryMulticastFabric *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__memorymulticastfabricControlFilter__)(struct MemoryMulticastFabric *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    void (*__memorymulticastfabricAddAdditionalDependants__)(struct RsClient *, struct MemoryMulticastFabric *, RsResourceRef *);
    NvU32 (*__memorymulticastfabricGetRefCount__)(struct MemoryMulticastFabric *);
    NV_STATUS (*__memorymulticastfabricMapTo__)(struct MemoryMulticastFabric *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__memorymulticastfabricCheckCopyPermissions__)(struct MemoryMulticastFabric *, struct OBJGPU *, NvHandle);
    void (*__memorymulticastfabricPreDestruct__)(struct MemoryMulticastFabric *);
    NV_STATUS (*__memorymulticastfabricIsDuplicate__)(struct MemoryMulticastFabric *, NvHandle, NvBool *);
    NV_STATUS (*__memorymulticastfabricUnmapFrom__)(struct MemoryMulticastFabric *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__memorymulticastfabricControl_Epilogue__)(struct MemoryMulticastFabric *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__memorymulticastfabricControlLookup__)(struct MemoryMulticastFabric *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NV_STATUS (*__memorymulticastfabricMap__)(struct MemoryMulticastFabric *, CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, RsCpuMapping *);
    NvBool (*__memorymulticastfabricAccessCallback__)(struct MemoryMulticastFabric *, struct RsClient *, void *, RsAccessRight);
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc;
};

#ifndef __NVOC_CLASS_MemoryMulticastFabric_TYPEDEF__
#define __NVOC_CLASS_MemoryMulticastFabric_TYPEDEF__
typedef struct MemoryMulticastFabric MemoryMulticastFabric;
#endif /* __NVOC_CLASS_MemoryMulticastFabric_TYPEDEF__ */

#ifndef __nvoc_class_id_MemoryMulticastFabric
#define __nvoc_class_id_MemoryMulticastFabric 0x130210
#endif /* __nvoc_class_id_MemoryMulticastFabric */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_MemoryMulticastFabric;

#define __staticCast_MemoryMulticastFabric(pThis) \
    ((pThis)->__nvoc_pbase_MemoryMulticastFabric)

#ifdef __nvoc_mem_multicast_fabric_h_disabled
#define __dynamicCast_MemoryMulticastFabric(pThis) ((MemoryMulticastFabric*)NULL)
#else //__nvoc_mem_multicast_fabric_h_disabled
#define __dynamicCast_MemoryMulticastFabric(pThis) \
    ((MemoryMulticastFabric*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(MemoryMulticastFabric)))
#endif //__nvoc_mem_multicast_fabric_h_disabled


NV_STATUS __nvoc_objCreateDynamic_MemoryMulticastFabric(MemoryMulticastFabric**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_MemoryMulticastFabric(MemoryMulticastFabric**, Dynamic*, NvU32, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_MemoryMulticastFabric(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_MemoryMulticastFabric((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define memorymulticastfabricCanCopy(pMemoryMulticastFabric) memorymulticastfabricCanCopy_DISPATCH(pMemoryMulticastFabric)
#define memorymulticastfabricCopyConstruct(pMemoryMulticastFabric, pCallContext, pParams) memorymulticastfabricCopyConstruct_DISPATCH(pMemoryMulticastFabric, pCallContext, pParams)
#define memorymulticastfabricIsReady(pMemoryMulticastFabric, bCopyConstructorContext) memorymulticastfabricIsReady_DISPATCH(pMemoryMulticastFabric, bCopyConstructorContext)
#define memorymulticastfabricControl(pMemoryMulticastFabric, pCallContext, pParams) memorymulticastfabricControl_DISPATCH(pMemoryMulticastFabric, pCallContext, pParams)
#define memorymulticastfabricControl_Prologue(pMemoryMulticastFabric, pCallContext, pParams) memorymulticastfabricControl_Prologue_DISPATCH(pMemoryMulticastFabric, pCallContext, pParams)
#define memorymulticastfabricIsGpuMapAllowed(pMemoryMulticastFabric, pGpu) memorymulticastfabricIsGpuMapAllowed_DISPATCH(pMemoryMulticastFabric, pGpu)
#define memorymulticastfabricGetMapAddrSpace(pMemoryMulticastFabric, pCallContext, mapFlags, pAddrSpace) memorymulticastfabricGetMapAddrSpace_DISPATCH(pMemoryMulticastFabric, pCallContext, mapFlags, pAddrSpace)
#define memorymulticastfabricCtrlGetInfo(pMemoryMulticastFabric, pParams) memorymulticastfabricCtrlGetInfo_DISPATCH(pMemoryMulticastFabric, pParams)
#define memorymulticastfabricCtrlAttachMem(pMemoryMulticastFabric, pParams) memorymulticastfabricCtrlAttachMem_DISPATCH(pMemoryMulticastFabric, pParams)
#define memorymulticastfabricCtrlRegisterEvent(pMemoryMulticastFabric, pParams) memorymulticastfabricCtrlRegisterEvent_DISPATCH(pMemoryMulticastFabric, pParams)
#define memorymulticastfabricCheckMemInterUnmap(pMemory, bSubdeviceHandleProvided) memorymulticastfabricCheckMemInterUnmap_DISPATCH(pMemory, bSubdeviceHandleProvided)
#define memorymulticastfabricUnmap(pMemory, pCallContext, pCpuMapping) memorymulticastfabricUnmap_DISPATCH(pMemory, pCallContext, pCpuMapping)
#define memorymulticastfabricGetMemInterMapParams(pMemory, pParams) memorymulticastfabricGetMemInterMapParams_DISPATCH(pMemory, pParams)
#define memorymulticastfabricGetMemoryMappingDescriptor(pMemory, ppMemDesc) memorymulticastfabricGetMemoryMappingDescriptor_DISPATCH(pMemory, ppMemDesc)
#define memorymulticastfabricShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) memorymulticastfabricShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define memorymulticastfabricControlFilter(pResource, pCallContext, pParams) memorymulticastfabricControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define memorymulticastfabricAddAdditionalDependants(pClient, pResource, pReference) memorymulticastfabricAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define memorymulticastfabricGetRefCount(pResource) memorymulticastfabricGetRefCount_DISPATCH(pResource)
#define memorymulticastfabricMapTo(pResource, pParams) memorymulticastfabricMapTo_DISPATCH(pResource, pParams)
#define memorymulticastfabricCheckCopyPermissions(pMemory, pDstGpu, hDstClientNvBool) memorymulticastfabricCheckCopyPermissions_DISPATCH(pMemory, pDstGpu, hDstClientNvBool)
#define memorymulticastfabricPreDestruct(pResource) memorymulticastfabricPreDestruct_DISPATCH(pResource)
#define memorymulticastfabricIsDuplicate(pMemory, hMemory, pDuplicate) memorymulticastfabricIsDuplicate_DISPATCH(pMemory, hMemory, pDuplicate)
#define memorymulticastfabricUnmapFrom(pResource, pParams) memorymulticastfabricUnmapFrom_DISPATCH(pResource, pParams)
#define memorymulticastfabricControl_Epilogue(pResource, pCallContext, pParams) memorymulticastfabricControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define memorymulticastfabricControlLookup(pResource, pParams, ppEntry) memorymulticastfabricControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define memorymulticastfabricMap(pMemory, pCallContext, pParams, pCpuMapping) memorymulticastfabricMap_DISPATCH(pMemory, pCallContext, pParams, pCpuMapping)
#define memorymulticastfabricAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) memorymulticastfabricAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
NvBool memorymulticastfabricCanCopy_IMPL(struct MemoryMulticastFabric *pMemoryMulticastFabric);

static inline NvBool memorymulticastfabricCanCopy_DISPATCH(struct MemoryMulticastFabric *pMemoryMulticastFabric) {
    return pMemoryMulticastFabric->__memorymulticastfabricCanCopy__(pMemoryMulticastFabric);
}

NV_STATUS memorymulticastfabricCopyConstruct_IMPL(struct MemoryMulticastFabric *pMemoryMulticastFabric, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);

static inline NV_STATUS memorymulticastfabricCopyConstruct_DISPATCH(struct MemoryMulticastFabric *pMemoryMulticastFabric, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {
    return pMemoryMulticastFabric->__memorymulticastfabricCopyConstruct__(pMemoryMulticastFabric, pCallContext, pParams);
}

NV_STATUS memorymulticastfabricIsReady_IMPL(struct MemoryMulticastFabric *pMemoryMulticastFabric, NvBool bCopyConstructorContext);

static inline NV_STATUS memorymulticastfabricIsReady_DISPATCH(struct MemoryMulticastFabric *pMemoryMulticastFabric, NvBool bCopyConstructorContext) {
    return pMemoryMulticastFabric->__memorymulticastfabricIsReady__(pMemoryMulticastFabric, bCopyConstructorContext);
}

NV_STATUS memorymulticastfabricControl_IMPL(struct MemoryMulticastFabric *pMemoryMulticastFabric, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);

static inline NV_STATUS memorymulticastfabricControl_DISPATCH(struct MemoryMulticastFabric *pMemoryMulticastFabric, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pMemoryMulticastFabric->__memorymulticastfabricControl__(pMemoryMulticastFabric, pCallContext, pParams);
}

NV_STATUS memorymulticastfabricControl_Prologue_IMPL(struct MemoryMulticastFabric *pMemoryMulticastFabric, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);

static inline NV_STATUS memorymulticastfabricControl_Prologue_DISPATCH(struct MemoryMulticastFabric *pMemoryMulticastFabric, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pMemoryMulticastFabric->__memorymulticastfabricControl_Prologue__(pMemoryMulticastFabric, pCallContext, pParams);
}

NvBool memorymulticastfabricIsGpuMapAllowed_IMPL(struct MemoryMulticastFabric *pMemoryMulticastFabric, struct OBJGPU *pGpu);

static inline NvBool memorymulticastfabricIsGpuMapAllowed_DISPATCH(struct MemoryMulticastFabric *pMemoryMulticastFabric, struct OBJGPU *pGpu) {
    return pMemoryMulticastFabric->__memorymulticastfabricIsGpuMapAllowed__(pMemoryMulticastFabric, pGpu);
}

NV_STATUS memorymulticastfabricGetMapAddrSpace_IMPL(struct MemoryMulticastFabric *pMemoryMulticastFabric, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);

static inline NV_STATUS memorymulticastfabricGetMapAddrSpace_DISPATCH(struct MemoryMulticastFabric *pMemoryMulticastFabric, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pMemoryMulticastFabric->__memorymulticastfabricGetMapAddrSpace__(pMemoryMulticastFabric, pCallContext, mapFlags, pAddrSpace);
}

NV_STATUS memorymulticastfabricCtrlGetInfo_IMPL(struct MemoryMulticastFabric *pMemoryMulticastFabric, NV00FD_CTRL_GET_INFO_PARAMS *pParams);

static inline NV_STATUS memorymulticastfabricCtrlGetInfo_DISPATCH(struct MemoryMulticastFabric *pMemoryMulticastFabric, NV00FD_CTRL_GET_INFO_PARAMS *pParams) {
    return pMemoryMulticastFabric->__memorymulticastfabricCtrlGetInfo__(pMemoryMulticastFabric, pParams);
}

NV_STATUS memorymulticastfabricCtrlAttachMem_IMPL(struct MemoryMulticastFabric *pMemoryMulticastFabric, NV00FD_CTRL_ATTACH_MEM_PARAMS *pParams);

static inline NV_STATUS memorymulticastfabricCtrlAttachMem_DISPATCH(struct MemoryMulticastFabric *pMemoryMulticastFabric, NV00FD_CTRL_ATTACH_MEM_PARAMS *pParams) {
    return pMemoryMulticastFabric->__memorymulticastfabricCtrlAttachMem__(pMemoryMulticastFabric, pParams);
}

NV_STATUS memorymulticastfabricCtrlRegisterEvent_IMPL(struct MemoryMulticastFabric *pMemoryMulticastFabric, NV00FD_CTRL_REGISTER_EVENT_PARAMS *pParams);

static inline NV_STATUS memorymulticastfabricCtrlRegisterEvent_DISPATCH(struct MemoryMulticastFabric *pMemoryMulticastFabric, NV00FD_CTRL_REGISTER_EVENT_PARAMS *pParams) {
    return pMemoryMulticastFabric->__memorymulticastfabricCtrlRegisterEvent__(pMemoryMulticastFabric, pParams);
}

static inline NV_STATUS memorymulticastfabricCheckMemInterUnmap_DISPATCH(struct MemoryMulticastFabric *pMemory, NvBool bSubdeviceHandleProvided) {
    return pMemory->__memorymulticastfabricCheckMemInterUnmap__(pMemory, bSubdeviceHandleProvided);
}

static inline NV_STATUS memorymulticastfabricUnmap_DISPATCH(struct MemoryMulticastFabric *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pMemory->__memorymulticastfabricUnmap__(pMemory, pCallContext, pCpuMapping);
}

static inline NV_STATUS memorymulticastfabricGetMemInterMapParams_DISPATCH(struct MemoryMulticastFabric *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pMemory->__memorymulticastfabricGetMemInterMapParams__(pMemory, pParams);
}

static inline NV_STATUS memorymulticastfabricGetMemoryMappingDescriptor_DISPATCH(struct MemoryMulticastFabric *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return pMemory->__memorymulticastfabricGetMemoryMappingDescriptor__(pMemory, ppMemDesc);
}

static inline NvBool memorymulticastfabricShareCallback_DISPATCH(struct MemoryMulticastFabric *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__memorymulticastfabricShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS memorymulticastfabricControlFilter_DISPATCH(struct MemoryMulticastFabric *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__memorymulticastfabricControlFilter__(pResource, pCallContext, pParams);
}

static inline void memorymulticastfabricAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct MemoryMulticastFabric *pResource, RsResourceRef *pReference) {
    pResource->__memorymulticastfabricAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NvU32 memorymulticastfabricGetRefCount_DISPATCH(struct MemoryMulticastFabric *pResource) {
    return pResource->__memorymulticastfabricGetRefCount__(pResource);
}

static inline NV_STATUS memorymulticastfabricMapTo_DISPATCH(struct MemoryMulticastFabric *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__memorymulticastfabricMapTo__(pResource, pParams);
}

static inline NV_STATUS memorymulticastfabricCheckCopyPermissions_DISPATCH(struct MemoryMulticastFabric *pMemory, struct OBJGPU *pDstGpu, NvHandle hDstClientNvBool) {
    return pMemory->__memorymulticastfabricCheckCopyPermissions__(pMemory, pDstGpu, hDstClientNvBool);
}

static inline void memorymulticastfabricPreDestruct_DISPATCH(struct MemoryMulticastFabric *pResource) {
    pResource->__memorymulticastfabricPreDestruct__(pResource);
}

static inline NV_STATUS memorymulticastfabricIsDuplicate_DISPATCH(struct MemoryMulticastFabric *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return pMemory->__memorymulticastfabricIsDuplicate__(pMemory, hMemory, pDuplicate);
}

static inline NV_STATUS memorymulticastfabricUnmapFrom_DISPATCH(struct MemoryMulticastFabric *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__memorymulticastfabricUnmapFrom__(pResource, pParams);
}

static inline void memorymulticastfabricControl_Epilogue_DISPATCH(struct MemoryMulticastFabric *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__memorymulticastfabricControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS memorymulticastfabricControlLookup_DISPATCH(struct MemoryMulticastFabric *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__memorymulticastfabricControlLookup__(pResource, pParams, ppEntry);
}

static inline NV_STATUS memorymulticastfabricMap_DISPATCH(struct MemoryMulticastFabric *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pMemory->__memorymulticastfabricMap__(pMemory, pCallContext, pParams, pCpuMapping);
}

static inline NvBool memorymulticastfabricAccessCallback_DISPATCH(struct MemoryMulticastFabric *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__memorymulticastfabricAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

NV_STATUS memorymulticastfabricConstruct_IMPL(struct MemoryMulticastFabric *arg_pMemoryMulticastFabric, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_memorymulticastfabricConstruct(arg_pMemoryMulticastFabric, arg_pCallContext, arg_pParams) memorymulticastfabricConstruct_IMPL(arg_pMemoryMulticastFabric, arg_pCallContext, arg_pParams)
void memorymulticastfabricDestruct_IMPL(struct MemoryMulticastFabric *pMemoryMulticastFabric);

#define __nvoc_memorymulticastfabricDestruct(pMemoryMulticastFabric) memorymulticastfabricDestruct_IMPL(pMemoryMulticastFabric)
#undef PRIVATE_FIELD


void memorymulticastfabricTeamSetupResponseCallback(OBJGPU *pGpu,
                                                    NV2080_CTRL_NVLINK_INBAND_RECEIVED_DATA_PARAMS *pMessage);

#endif // _MEMORYMULTICASTFABRIC_H_

#ifdef __cplusplus
} // extern "C"
#endif
#endif // _G_MEM_MULTICAST_FABRIC_NVOC_H_
