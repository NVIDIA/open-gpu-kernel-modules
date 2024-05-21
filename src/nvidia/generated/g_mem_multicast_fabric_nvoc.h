
#ifndef _G_MEM_MULTICAST_FABRIC_NVOC_H_
#define _G_MEM_MULTICAST_FABRIC_NVOC_H_
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
 * SPDX-FileCopyrightText: Copyright (c) 2022-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#pragma once
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

typedef struct mem_multicast_fabric_descriptor MEM_MULTICAST_FABRIC_DESCRIPTOR;


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_MEM_MULTICAST_FABRIC_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct MemoryMulticastFabric {

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
    struct MemoryMulticastFabric *__nvoc_pbase_MemoryMulticastFabric;    // memorymulticastfabric

    // Vtable with 34 per-object function pointers
    NvBool (*__memorymulticastfabricCanCopy__)(struct MemoryMulticastFabric * /*this*/);  // virtual override (res) base (mem)
    NV_STATUS (*__memorymulticastfabricCopyConstruct__)(struct MemoryMulticastFabric * /*this*/, CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);  // virtual override (mem) base (mem)
    NV_STATUS (*__memorymulticastfabricIsReady__)(struct MemoryMulticastFabric * /*this*/, NvBool);  // virtual override (mem) base (mem)
    NV_STATUS (*__memorymulticastfabricControl__)(struct MemoryMulticastFabric * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual override (res) base (mem)
    NvBool (*__memorymulticastfabricIsGpuMapAllowed__)(struct MemoryMulticastFabric * /*this*/, struct OBJGPU *);  // virtual override (mem) base (mem)
    NvBool (*__memorymulticastfabricIsExportAllowed__)(struct MemoryMulticastFabric * /*this*/);  // virtual override (mem) base (mem)
    NV_STATUS (*__memorymulticastfabricGetMapAddrSpace__)(struct MemoryMulticastFabric * /*this*/, CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual override (mem) base (mem)
    NV_STATUS (*__memorymulticastfabricCtrlGetInfo__)(struct MemoryMulticastFabric * /*this*/, NV00FD_CTRL_GET_INFO_PARAMS *);  // exported (id=0xfd0101)
    NV_STATUS (*__memorymulticastfabricCtrlAttachMem__)(struct MemoryMulticastFabric * /*this*/, NV00FD_CTRL_ATTACH_MEM_PARAMS *);  // exported (id=0xfd0102)
    NV_STATUS (*__memorymulticastfabricCtrlDetachMem__)(struct MemoryMulticastFabric * /*this*/, NV00FD_CTRL_DETACH_MEM_PARAMS *);  // exported (id=0xfd0105)
    NV_STATUS (*__memorymulticastfabricCtrlAttachGpu__)(struct MemoryMulticastFabric * /*this*/, NV00FD_CTRL_ATTACH_GPU_PARAMS *);  // exported (id=0xfd0104)
    NV_STATUS (*__memorymulticastfabricCtrlRegisterEvent__)(struct MemoryMulticastFabric * /*this*/, NV00FD_CTRL_REGISTER_EVENT_PARAMS *);  // exported (id=0xfd0103)
    NV_STATUS (*__memorymulticastfabricCtrlAttachRemoteGpu__)(struct MemoryMulticastFabric * /*this*/, NV00FD_CTRL_ATTACH_REMOTE_GPU_PARAMS *);  // exported (id=0xfd0106)
    NV_STATUS (*__memorymulticastfabricCtrlSetFailure__)(struct MemoryMulticastFabric * /*this*/, NV00FD_CTRL_SET_FAILURE_PARAMS *);  // exported (id=0xfd0107)
    NV_STATUS (*__memorymulticastfabricIsDuplicate__)(struct MemoryMulticastFabric * /*this*/, NvHandle, NvBool *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__memorymulticastfabricMap__)(struct MemoryMulticastFabric * /*this*/, CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, RsCpuMapping *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__memorymulticastfabricUnmap__)(struct MemoryMulticastFabric * /*this*/, CALL_CONTEXT *, RsCpuMapping *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__memorymulticastfabricGetMemInterMapParams__)(struct MemoryMulticastFabric * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__memorymulticastfabricCheckMemInterUnmap__)(struct MemoryMulticastFabric * /*this*/, NvBool);  // inline virtual inherited (mem) base (mem) body
    NV_STATUS (*__memorymulticastfabricGetMemoryMappingDescriptor__)(struct MemoryMulticastFabric * /*this*/, MEMORY_DESCRIPTOR **);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__memorymulticastfabricCheckCopyPermissions__)(struct MemoryMulticastFabric * /*this*/, struct OBJGPU *, struct Device *);  // inline virtual inherited (mem) base (mem) body
    NvBool (*__memorymulticastfabricAccessCallback__)(struct MemoryMulticastFabric * /*this*/, RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (mem)
    NvBool (*__memorymulticastfabricShareCallback__)(struct MemoryMulticastFabric * /*this*/, RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (rmres) base (mem)
    NV_STATUS (*__memorymulticastfabricControlSerialization_Prologue__)(struct MemoryMulticastFabric * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (mem)
    void (*__memorymulticastfabricControlSerialization_Epilogue__)(struct MemoryMulticastFabric * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (mem)
    NV_STATUS (*__memorymulticastfabricControl_Prologue__)(struct MemoryMulticastFabric * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (mem)
    void (*__memorymulticastfabricControl_Epilogue__)(struct MemoryMulticastFabric * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (mem)
    void (*__memorymulticastfabricPreDestruct__)(struct MemoryMulticastFabric * /*this*/);  // virtual inherited (res) base (mem)
    NV_STATUS (*__memorymulticastfabricControlFilter__)(struct MemoryMulticastFabric * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (mem)
    NvBool (*__memorymulticastfabricIsPartialUnmapSupported__)(struct MemoryMulticastFabric * /*this*/);  // inline virtual inherited (res) base (mem) body
    NV_STATUS (*__memorymulticastfabricMapTo__)(struct MemoryMulticastFabric * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (mem)
    NV_STATUS (*__memorymulticastfabricUnmapFrom__)(struct MemoryMulticastFabric * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (mem)
    NvU32 (*__memorymulticastfabricGetRefCount__)(struct MemoryMulticastFabric * /*this*/);  // virtual inherited (res) base (mem)
    void (*__memorymulticastfabricAddAdditionalDependants__)(struct RsClient *, struct MemoryMulticastFabric * /*this*/, RsResourceRef *);  // virtual inherited (res) base (mem)

    // Data members
    NvU16 expNodeId;
    NvBool bImported;
    MEM_MULTICAST_FABRIC_DESCRIPTOR *PRIVATE_FIELD(pMulticastFabricDesc);
};

#ifndef __NVOC_CLASS_MemoryMulticastFabric_TYPEDEF__
#define __NVOC_CLASS_MemoryMulticastFabric_TYPEDEF__
typedef struct MemoryMulticastFabric MemoryMulticastFabric;
#endif /* __NVOC_CLASS_MemoryMulticastFabric_TYPEDEF__ */

#ifndef __nvoc_class_id_MemoryMulticastFabric
#define __nvoc_class_id_MemoryMulticastFabric 0x130210
#endif /* __nvoc_class_id_MemoryMulticastFabric */

// Casting support
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


// Wrapper macros
#define memorymulticastfabricCanCopy_FNPTR(pMemoryMulticastFabric) pMemoryMulticastFabric->__memorymulticastfabricCanCopy__
#define memorymulticastfabricCanCopy(pMemoryMulticastFabric) memorymulticastfabricCanCopy_DISPATCH(pMemoryMulticastFabric)
#define memorymulticastfabricCopyConstruct_FNPTR(pMemoryMulticastFabric) pMemoryMulticastFabric->__memorymulticastfabricCopyConstruct__
#define memorymulticastfabricCopyConstruct(pMemoryMulticastFabric, pCallContext, pParams) memorymulticastfabricCopyConstruct_DISPATCH(pMemoryMulticastFabric, pCallContext, pParams)
#define memorymulticastfabricIsReady_FNPTR(pMemoryMulticastFabric) pMemoryMulticastFabric->__memorymulticastfabricIsReady__
#define memorymulticastfabricIsReady(pMemoryMulticastFabric, bCopyConstructorContext) memorymulticastfabricIsReady_DISPATCH(pMemoryMulticastFabric, bCopyConstructorContext)
#define memorymulticastfabricControl_FNPTR(pMemoryMulticastFabric) pMemoryMulticastFabric->__memorymulticastfabricControl__
#define memorymulticastfabricControl(pMemoryMulticastFabric, pCallContext, pParams) memorymulticastfabricControl_DISPATCH(pMemoryMulticastFabric, pCallContext, pParams)
#define memorymulticastfabricIsGpuMapAllowed_FNPTR(pMemoryMulticastFabric) pMemoryMulticastFabric->__memorymulticastfabricIsGpuMapAllowed__
#define memorymulticastfabricIsGpuMapAllowed(pMemoryMulticastFabric, pGpu) memorymulticastfabricIsGpuMapAllowed_DISPATCH(pMemoryMulticastFabric, pGpu)
#define memorymulticastfabricIsExportAllowed_FNPTR(pMemoryMulticastFabric) pMemoryMulticastFabric->__memorymulticastfabricIsExportAllowed__
#define memorymulticastfabricIsExportAllowed(pMemoryMulticastFabric) memorymulticastfabricIsExportAllowed_DISPATCH(pMemoryMulticastFabric)
#define memorymulticastfabricGetMapAddrSpace_FNPTR(pMemoryMulticastFabric) pMemoryMulticastFabric->__memorymulticastfabricGetMapAddrSpace__
#define memorymulticastfabricGetMapAddrSpace(pMemoryMulticastFabric, pCallContext, mapFlags, pAddrSpace) memorymulticastfabricGetMapAddrSpace_DISPATCH(pMemoryMulticastFabric, pCallContext, mapFlags, pAddrSpace)
#define memorymulticastfabricCtrlGetInfo_FNPTR(pMemoryMulticastFabric) pMemoryMulticastFabric->__memorymulticastfabricCtrlGetInfo__
#define memorymulticastfabricCtrlGetInfo(pMemoryMulticastFabric, pParams) memorymulticastfabricCtrlGetInfo_DISPATCH(pMemoryMulticastFabric, pParams)
#define memorymulticastfabricCtrlAttachMem_FNPTR(pMemoryMulticastFabric) pMemoryMulticastFabric->__memorymulticastfabricCtrlAttachMem__
#define memorymulticastfabricCtrlAttachMem(pMemoryMulticastFabric, pParams) memorymulticastfabricCtrlAttachMem_DISPATCH(pMemoryMulticastFabric, pParams)
#define memorymulticastfabricCtrlDetachMem_FNPTR(pMemoryMulticastFabric) pMemoryMulticastFabric->__memorymulticastfabricCtrlDetachMem__
#define memorymulticastfabricCtrlDetachMem(pMemoryMulticastFabric, pParams) memorymulticastfabricCtrlDetachMem_DISPATCH(pMemoryMulticastFabric, pParams)
#define memorymulticastfabricCtrlAttachGpu_FNPTR(pMemoryMulticastFabric) pMemoryMulticastFabric->__memorymulticastfabricCtrlAttachGpu__
#define memorymulticastfabricCtrlAttachGpu(pMemoryMulticastFabric, pParams) memorymulticastfabricCtrlAttachGpu_DISPATCH(pMemoryMulticastFabric, pParams)
#define memorymulticastfabricCtrlRegisterEvent_FNPTR(pMemoryMulticastFabric) pMemoryMulticastFabric->__memorymulticastfabricCtrlRegisterEvent__
#define memorymulticastfabricCtrlRegisterEvent(pMemoryMulticastFabric, pParams) memorymulticastfabricCtrlRegisterEvent_DISPATCH(pMemoryMulticastFabric, pParams)
#define memorymulticastfabricCtrlAttachRemoteGpu_FNPTR(pMemoryMulticastFabric) pMemoryMulticastFabric->__memorymulticastfabricCtrlAttachRemoteGpu__
#define memorymulticastfabricCtrlAttachRemoteGpu(pMemoryMulticastFabric, pParams) memorymulticastfabricCtrlAttachRemoteGpu_DISPATCH(pMemoryMulticastFabric, pParams)
#define memorymulticastfabricCtrlSetFailure_FNPTR(pMemoryMulticastFabric) pMemoryMulticastFabric->__memorymulticastfabricCtrlSetFailure__
#define memorymulticastfabricCtrlSetFailure(pMemoryMulticastFabric, pParams) memorymulticastfabricCtrlSetFailure_DISPATCH(pMemoryMulticastFabric, pParams)
#define memorymulticastfabricIsDuplicate_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memIsDuplicate__
#define memorymulticastfabricIsDuplicate(pMemory, hMemory, pDuplicate) memorymulticastfabricIsDuplicate_DISPATCH(pMemory, hMemory, pDuplicate)
#define memorymulticastfabricMap_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memMap__
#define memorymulticastfabricMap(pMemory, pCallContext, pParams, pCpuMapping) memorymulticastfabricMap_DISPATCH(pMemory, pCallContext, pParams, pCpuMapping)
#define memorymulticastfabricUnmap_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memUnmap__
#define memorymulticastfabricUnmap(pMemory, pCallContext, pCpuMapping) memorymulticastfabricUnmap_DISPATCH(pMemory, pCallContext, pCpuMapping)
#define memorymulticastfabricGetMemInterMapParams_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memGetMemInterMapParams__
#define memorymulticastfabricGetMemInterMapParams(pMemory, pParams) memorymulticastfabricGetMemInterMapParams_DISPATCH(pMemory, pParams)
#define memorymulticastfabricCheckMemInterUnmap_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memCheckMemInterUnmap__
#define memorymulticastfabricCheckMemInterUnmap(pMemory, bSubdeviceHandleProvided) memorymulticastfabricCheckMemInterUnmap_DISPATCH(pMemory, bSubdeviceHandleProvided)
#define memorymulticastfabricGetMemoryMappingDescriptor_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memGetMemoryMappingDescriptor__
#define memorymulticastfabricGetMemoryMappingDescriptor(pMemory, ppMemDesc) memorymulticastfabricGetMemoryMappingDescriptor_DISPATCH(pMemory, ppMemDesc)
#define memorymulticastfabricCheckCopyPermissions_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memCheckCopyPermissions__
#define memorymulticastfabricCheckCopyPermissions(pMemory, pDstGpu, pDstDevice) memorymulticastfabricCheckCopyPermissions_DISPATCH(pMemory, pDstGpu, pDstDevice)
#define memorymulticastfabricAccessCallback_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__rmresAccessCallback__
#define memorymulticastfabricAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) memorymulticastfabricAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define memorymulticastfabricShareCallback_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__rmresShareCallback__
#define memorymulticastfabricShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) memorymulticastfabricShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define memorymulticastfabricControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__rmresControlSerialization_Prologue__
#define memorymulticastfabricControlSerialization_Prologue(pResource, pCallContext, pParams) memorymulticastfabricControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define memorymulticastfabricControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__rmresControlSerialization_Epilogue__
#define memorymulticastfabricControlSerialization_Epilogue(pResource, pCallContext, pParams) memorymulticastfabricControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define memorymulticastfabricControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__rmresControl_Prologue__
#define memorymulticastfabricControl_Prologue(pResource, pCallContext, pParams) memorymulticastfabricControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define memorymulticastfabricControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__rmresControl_Epilogue__
#define memorymulticastfabricControl_Epilogue(pResource, pCallContext, pParams) memorymulticastfabricControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define memorymulticastfabricPreDestruct_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resPreDestruct__
#define memorymulticastfabricPreDestruct(pResource) memorymulticastfabricPreDestruct_DISPATCH(pResource)
#define memorymulticastfabricControlFilter_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resControlFilter__
#define memorymulticastfabricControlFilter(pResource, pCallContext, pParams) memorymulticastfabricControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define memorymulticastfabricIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsPartialUnmapSupported__
#define memorymulticastfabricIsPartialUnmapSupported(pResource) memorymulticastfabricIsPartialUnmapSupported_DISPATCH(pResource)
#define memorymulticastfabricMapTo_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resMapTo__
#define memorymulticastfabricMapTo(pResource, pParams) memorymulticastfabricMapTo_DISPATCH(pResource, pParams)
#define memorymulticastfabricUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmapFrom__
#define memorymulticastfabricUnmapFrom(pResource, pParams) memorymulticastfabricUnmapFrom_DISPATCH(pResource, pParams)
#define memorymulticastfabricGetRefCount_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resGetRefCount__
#define memorymulticastfabricGetRefCount(pResource) memorymulticastfabricGetRefCount_DISPATCH(pResource)
#define memorymulticastfabricAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resAddAdditionalDependants__
#define memorymulticastfabricAddAdditionalDependants(pClient, pResource, pReference) memorymulticastfabricAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NvBool memorymulticastfabricCanCopy_DISPATCH(struct MemoryMulticastFabric *pMemoryMulticastFabric) {
    return pMemoryMulticastFabric->__memorymulticastfabricCanCopy__(pMemoryMulticastFabric);
}

static inline NV_STATUS memorymulticastfabricCopyConstruct_DISPATCH(struct MemoryMulticastFabric *pMemoryMulticastFabric, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {
    return pMemoryMulticastFabric->__memorymulticastfabricCopyConstruct__(pMemoryMulticastFabric, pCallContext, pParams);
}

static inline NV_STATUS memorymulticastfabricIsReady_DISPATCH(struct MemoryMulticastFabric *pMemoryMulticastFabric, NvBool bCopyConstructorContext) {
    return pMemoryMulticastFabric->__memorymulticastfabricIsReady__(pMemoryMulticastFabric, bCopyConstructorContext);
}

static inline NV_STATUS memorymulticastfabricControl_DISPATCH(struct MemoryMulticastFabric *pMemoryMulticastFabric, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pMemoryMulticastFabric->__memorymulticastfabricControl__(pMemoryMulticastFabric, pCallContext, pParams);
}

static inline NvBool memorymulticastfabricIsGpuMapAllowed_DISPATCH(struct MemoryMulticastFabric *pMemoryMulticastFabric, struct OBJGPU *pGpu) {
    return pMemoryMulticastFabric->__memorymulticastfabricIsGpuMapAllowed__(pMemoryMulticastFabric, pGpu);
}

static inline NvBool memorymulticastfabricIsExportAllowed_DISPATCH(struct MemoryMulticastFabric *pMemoryMulticastFabric) {
    return pMemoryMulticastFabric->__memorymulticastfabricIsExportAllowed__(pMemoryMulticastFabric);
}

static inline NV_STATUS memorymulticastfabricGetMapAddrSpace_DISPATCH(struct MemoryMulticastFabric *pMemoryMulticastFabric, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pMemoryMulticastFabric->__memorymulticastfabricGetMapAddrSpace__(pMemoryMulticastFabric, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS memorymulticastfabricCtrlGetInfo_DISPATCH(struct MemoryMulticastFabric *pMemoryMulticastFabric, NV00FD_CTRL_GET_INFO_PARAMS *pParams) {
    return pMemoryMulticastFabric->__memorymulticastfabricCtrlGetInfo__(pMemoryMulticastFabric, pParams);
}

static inline NV_STATUS memorymulticastfabricCtrlAttachMem_DISPATCH(struct MemoryMulticastFabric *pMemoryMulticastFabric, NV00FD_CTRL_ATTACH_MEM_PARAMS *pParams) {
    return pMemoryMulticastFabric->__memorymulticastfabricCtrlAttachMem__(pMemoryMulticastFabric, pParams);
}

static inline NV_STATUS memorymulticastfabricCtrlDetachMem_DISPATCH(struct MemoryMulticastFabric *pMemoryMulticastFabric, NV00FD_CTRL_DETACH_MEM_PARAMS *pParams) {
    return pMemoryMulticastFabric->__memorymulticastfabricCtrlDetachMem__(pMemoryMulticastFabric, pParams);
}

static inline NV_STATUS memorymulticastfabricCtrlAttachGpu_DISPATCH(struct MemoryMulticastFabric *pMemoryMulticastFabric, NV00FD_CTRL_ATTACH_GPU_PARAMS *pParams) {
    return pMemoryMulticastFabric->__memorymulticastfabricCtrlAttachGpu__(pMemoryMulticastFabric, pParams);
}

static inline NV_STATUS memorymulticastfabricCtrlRegisterEvent_DISPATCH(struct MemoryMulticastFabric *pMemoryMulticastFabric, NV00FD_CTRL_REGISTER_EVENT_PARAMS *pParams) {
    return pMemoryMulticastFabric->__memorymulticastfabricCtrlRegisterEvent__(pMemoryMulticastFabric, pParams);
}

static inline NV_STATUS memorymulticastfabricCtrlAttachRemoteGpu_DISPATCH(struct MemoryMulticastFabric *pMemoryMulticastFabric, NV00FD_CTRL_ATTACH_REMOTE_GPU_PARAMS *pParams) {
    return pMemoryMulticastFabric->__memorymulticastfabricCtrlAttachRemoteGpu__(pMemoryMulticastFabric, pParams);
}

static inline NV_STATUS memorymulticastfabricCtrlSetFailure_DISPATCH(struct MemoryMulticastFabric *pMemoryMulticastFabric, NV00FD_CTRL_SET_FAILURE_PARAMS *pParams) {
    return pMemoryMulticastFabric->__memorymulticastfabricCtrlSetFailure__(pMemoryMulticastFabric, pParams);
}

static inline NV_STATUS memorymulticastfabricIsDuplicate_DISPATCH(struct MemoryMulticastFabric *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return pMemory->__memorymulticastfabricIsDuplicate__(pMemory, hMemory, pDuplicate);
}

static inline NV_STATUS memorymulticastfabricMap_DISPATCH(struct MemoryMulticastFabric *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pMemory->__memorymulticastfabricMap__(pMemory, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS memorymulticastfabricUnmap_DISPATCH(struct MemoryMulticastFabric *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pMemory->__memorymulticastfabricUnmap__(pMemory, pCallContext, pCpuMapping);
}

static inline NV_STATUS memorymulticastfabricGetMemInterMapParams_DISPATCH(struct MemoryMulticastFabric *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pMemory->__memorymulticastfabricGetMemInterMapParams__(pMemory, pParams);
}

static inline NV_STATUS memorymulticastfabricCheckMemInterUnmap_DISPATCH(struct MemoryMulticastFabric *pMemory, NvBool bSubdeviceHandleProvided) {
    return pMemory->__memorymulticastfabricCheckMemInterUnmap__(pMemory, bSubdeviceHandleProvided);
}

static inline NV_STATUS memorymulticastfabricGetMemoryMappingDescriptor_DISPATCH(struct MemoryMulticastFabric *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return pMemory->__memorymulticastfabricGetMemoryMappingDescriptor__(pMemory, ppMemDesc);
}

static inline NV_STATUS memorymulticastfabricCheckCopyPermissions_DISPATCH(struct MemoryMulticastFabric *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return pMemory->__memorymulticastfabricCheckCopyPermissions__(pMemory, pDstGpu, pDstDevice);
}

static inline NvBool memorymulticastfabricAccessCallback_DISPATCH(struct MemoryMulticastFabric *pResource, RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__memorymulticastfabricAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool memorymulticastfabricShareCallback_DISPATCH(struct MemoryMulticastFabric *pResource, RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__memorymulticastfabricShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS memorymulticastfabricControlSerialization_Prologue_DISPATCH(struct MemoryMulticastFabric *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__memorymulticastfabricControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void memorymulticastfabricControlSerialization_Epilogue_DISPATCH(struct MemoryMulticastFabric *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__memorymulticastfabricControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS memorymulticastfabricControl_Prologue_DISPATCH(struct MemoryMulticastFabric *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__memorymulticastfabricControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void memorymulticastfabricControl_Epilogue_DISPATCH(struct MemoryMulticastFabric *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__memorymulticastfabricControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline void memorymulticastfabricPreDestruct_DISPATCH(struct MemoryMulticastFabric *pResource) {
    pResource->__memorymulticastfabricPreDestruct__(pResource);
}

static inline NV_STATUS memorymulticastfabricControlFilter_DISPATCH(struct MemoryMulticastFabric *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__memorymulticastfabricControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool memorymulticastfabricIsPartialUnmapSupported_DISPATCH(struct MemoryMulticastFabric *pResource) {
    return pResource->__memorymulticastfabricIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS memorymulticastfabricMapTo_DISPATCH(struct MemoryMulticastFabric *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__memorymulticastfabricMapTo__(pResource, pParams);
}

static inline NV_STATUS memorymulticastfabricUnmapFrom_DISPATCH(struct MemoryMulticastFabric *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__memorymulticastfabricUnmapFrom__(pResource, pParams);
}

static inline NvU32 memorymulticastfabricGetRefCount_DISPATCH(struct MemoryMulticastFabric *pResource) {
    return pResource->__memorymulticastfabricGetRefCount__(pResource);
}

static inline void memorymulticastfabricAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct MemoryMulticastFabric *pResource, RsResourceRef *pReference) {
    pResource->__memorymulticastfabricAddAdditionalDependants__(pClient, pResource, pReference);
}

NvBool memorymulticastfabricCanCopy_IMPL(struct MemoryMulticastFabric *pMemoryMulticastFabric);

NV_STATUS memorymulticastfabricCopyConstruct_IMPL(struct MemoryMulticastFabric *pMemoryMulticastFabric, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);

NV_STATUS memorymulticastfabricIsReady_IMPL(struct MemoryMulticastFabric *pMemoryMulticastFabric, NvBool bCopyConstructorContext);

NV_STATUS memorymulticastfabricControl_IMPL(struct MemoryMulticastFabric *pMemoryMulticastFabric, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);

NvBool memorymulticastfabricIsGpuMapAllowed_IMPL(struct MemoryMulticastFabric *pMemoryMulticastFabric, struct OBJGPU *pGpu);

NvBool memorymulticastfabricIsExportAllowed_IMPL(struct MemoryMulticastFabric *pMemoryMulticastFabric);

NV_STATUS memorymulticastfabricGetMapAddrSpace_IMPL(struct MemoryMulticastFabric *pMemoryMulticastFabric, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);

NV_STATUS memorymulticastfabricCtrlGetInfo_IMPL(struct MemoryMulticastFabric *pMemoryMulticastFabric, NV00FD_CTRL_GET_INFO_PARAMS *pParams);

NV_STATUS memorymulticastfabricCtrlAttachMem_IMPL(struct MemoryMulticastFabric *pMemoryMulticastFabric, NV00FD_CTRL_ATTACH_MEM_PARAMS *pParams);

NV_STATUS memorymulticastfabricCtrlDetachMem_IMPL(struct MemoryMulticastFabric *pMemoryMulticastFabric, NV00FD_CTRL_DETACH_MEM_PARAMS *pParams);

NV_STATUS memorymulticastfabricCtrlAttachGpu_IMPL(struct MemoryMulticastFabric *pMemoryMulticastFabric, NV00FD_CTRL_ATTACH_GPU_PARAMS *pParams);

NV_STATUS memorymulticastfabricCtrlRegisterEvent_IMPL(struct MemoryMulticastFabric *pMemoryMulticastFabric, NV00FD_CTRL_REGISTER_EVENT_PARAMS *pParams);

NV_STATUS memorymulticastfabricCtrlAttachRemoteGpu_IMPL(struct MemoryMulticastFabric *pMemoryMulticastFabric, NV00FD_CTRL_ATTACH_REMOTE_GPU_PARAMS *pParams);

NV_STATUS memorymulticastfabricCtrlSetFailure_IMPL(struct MemoryMulticastFabric *pMemoryMulticastFabric, NV00FD_CTRL_SET_FAILURE_PARAMS *pParams);

NV_STATUS memorymulticastfabricConstruct_IMPL(struct MemoryMulticastFabric *arg_pMemoryMulticastFabric, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_memorymulticastfabricConstruct(arg_pMemoryMulticastFabric, arg_pCallContext, arg_pParams) memorymulticastfabricConstruct_IMPL(arg_pMemoryMulticastFabric, arg_pCallContext, arg_pParams)
void memorymulticastfabricDestruct_IMPL(struct MemoryMulticastFabric *pMemoryMulticastFabric);

#define __nvoc_memorymulticastfabricDestruct(pMemoryMulticastFabric) memorymulticastfabricDestruct_IMPL(pMemoryMulticastFabric)
void memorymulticastfabricRemoveFromCache_IMPL(struct MemoryMulticastFabric *pMemoryMulticastFabric);

#ifdef __nvoc_mem_multicast_fabric_h_disabled
static inline void memorymulticastfabricRemoveFromCache(struct MemoryMulticastFabric *pMemoryMulticastFabric) {
    NV_ASSERT_FAILED_PRECOMP("MemoryMulticastFabric was disabled!");
}
#else //__nvoc_mem_multicast_fabric_h_disabled
#define memorymulticastfabricRemoveFromCache(pMemoryMulticastFabric) memorymulticastfabricRemoveFromCache_IMPL(pMemoryMulticastFabric)
#endif //__nvoc_mem_multicast_fabric_h_disabled

#undef PRIVATE_FIELD


NV_STATUS memorymulticastfabricTeamSetupResponseCallback(NvU32 gpuInstance, NvU64 *pNotifyGfIdMask,
                                        NV2080_CTRL_NVLINK_INBAND_RECEIVED_DATA_PARAMS *pMessage);

#endif // _MEMORYMULTICASTFABRIC_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_MEM_MULTICAST_FABRIC_NVOC_H_
