
#ifndef _G_SYNCPOINT_MEM_NVOC_H_
#define _G_SYNCPOINT_MEM_NVOC_H_
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

#pragma once
#include "g_syncpoint_mem_nvoc.h"

#ifndef _SYNCPOINT_MEMORY_H_
#define _SYNCPOINT_MEMORY_H_

#include "mem_mgr/mem.h"

/*!
 * Bind memory allocated through os descriptor
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_SYNCPOINT_MEM_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct SyncpointMemory {

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
    struct SyncpointMemory *__nvoc_pbase_SyncpointMemory;    // syncpoint

    // Vtable with 26 per-object function pointers
    NvBool (*__syncpointCanCopy__)(struct SyncpointMemory * /*this*/);  // virtual override (res) base (mem)
    NV_STATUS (*__syncpointIsDuplicate__)(struct SyncpointMemory * /*this*/, NvHandle, NvBool *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__syncpointGetMapAddrSpace__)(struct SyncpointMemory * /*this*/, CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__syncpointControl__)(struct SyncpointMemory * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__syncpointMap__)(struct SyncpointMemory * /*this*/, CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, RsCpuMapping *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__syncpointUnmap__)(struct SyncpointMemory * /*this*/, CALL_CONTEXT *, RsCpuMapping *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__syncpointGetMemInterMapParams__)(struct SyncpointMemory * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__syncpointCheckMemInterUnmap__)(struct SyncpointMemory * /*this*/, NvBool);  // inline virtual inherited (mem) base (mem) body
    NV_STATUS (*__syncpointGetMemoryMappingDescriptor__)(struct SyncpointMemory * /*this*/, MEMORY_DESCRIPTOR **);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__syncpointCheckCopyPermissions__)(struct SyncpointMemory * /*this*/, struct OBJGPU *, struct Device *);  // inline virtual inherited (mem) base (mem) body
    NV_STATUS (*__syncpointIsReady__)(struct SyncpointMemory * /*this*/, NvBool);  // virtual inherited (mem) base (mem)
    NvBool (*__syncpointIsGpuMapAllowed__)(struct SyncpointMemory * /*this*/, struct OBJGPU *);  // inline virtual inherited (mem) base (mem) body
    NvBool (*__syncpointIsExportAllowed__)(struct SyncpointMemory * /*this*/);  // inline virtual inherited (mem) base (mem) body
    NvBool (*__syncpointAccessCallback__)(struct SyncpointMemory * /*this*/, RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (mem)
    NvBool (*__syncpointShareCallback__)(struct SyncpointMemory * /*this*/, RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (rmres) base (mem)
    NV_STATUS (*__syncpointControlSerialization_Prologue__)(struct SyncpointMemory * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (mem)
    void (*__syncpointControlSerialization_Epilogue__)(struct SyncpointMemory * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (mem)
    NV_STATUS (*__syncpointControl_Prologue__)(struct SyncpointMemory * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (mem)
    void (*__syncpointControl_Epilogue__)(struct SyncpointMemory * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (mem)
    void (*__syncpointPreDestruct__)(struct SyncpointMemory * /*this*/);  // virtual inherited (res) base (mem)
    NV_STATUS (*__syncpointControlFilter__)(struct SyncpointMemory * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (mem)
    NvBool (*__syncpointIsPartialUnmapSupported__)(struct SyncpointMemory * /*this*/);  // inline virtual inherited (res) base (mem) body
    NV_STATUS (*__syncpointMapTo__)(struct SyncpointMemory * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (mem)
    NV_STATUS (*__syncpointUnmapFrom__)(struct SyncpointMemory * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (mem)
    NvU32 (*__syncpointGetRefCount__)(struct SyncpointMemory * /*this*/);  // virtual inherited (res) base (mem)
    void (*__syncpointAddAdditionalDependants__)(struct RsClient *, struct SyncpointMemory * /*this*/, RsResourceRef *);  // virtual inherited (res) base (mem)
};

#ifndef __NVOC_CLASS_SyncpointMemory_TYPEDEF__
#define __NVOC_CLASS_SyncpointMemory_TYPEDEF__
typedef struct SyncpointMemory SyncpointMemory;
#endif /* __NVOC_CLASS_SyncpointMemory_TYPEDEF__ */

#ifndef __nvoc_class_id_SyncpointMemory
#define __nvoc_class_id_SyncpointMemory 0x529def
#endif /* __nvoc_class_id_SyncpointMemory */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_SyncpointMemory;

#define __staticCast_SyncpointMemory(pThis) \
    ((pThis)->__nvoc_pbase_SyncpointMemory)

#ifdef __nvoc_syncpoint_mem_h_disabled
#define __dynamicCast_SyncpointMemory(pThis) ((SyncpointMemory*)NULL)
#else //__nvoc_syncpoint_mem_h_disabled
#define __dynamicCast_SyncpointMemory(pThis) \
    ((SyncpointMemory*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(SyncpointMemory)))
#endif //__nvoc_syncpoint_mem_h_disabled

NV_STATUS __nvoc_objCreateDynamic_SyncpointMemory(SyncpointMemory**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_SyncpointMemory(SyncpointMemory**, Dynamic*, NvU32, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_SyncpointMemory(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_SyncpointMemory((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define syncpointCanCopy_FNPTR(pSyncpointMemory) pSyncpointMemory->__syncpointCanCopy__
#define syncpointCanCopy(pSyncpointMemory) syncpointCanCopy_DISPATCH(pSyncpointMemory)
#define syncpointIsDuplicate_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memIsDuplicate__
#define syncpointIsDuplicate(pMemory, hMemory, pDuplicate) syncpointIsDuplicate_DISPATCH(pMemory, hMemory, pDuplicate)
#define syncpointGetMapAddrSpace_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memGetMapAddrSpace__
#define syncpointGetMapAddrSpace(pMemory, pCallContext, mapFlags, pAddrSpace) syncpointGetMapAddrSpace_DISPATCH(pMemory, pCallContext, mapFlags, pAddrSpace)
#define syncpointControl_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memControl__
#define syncpointControl(pMemory, pCallContext, pParams) syncpointControl_DISPATCH(pMemory, pCallContext, pParams)
#define syncpointMap_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memMap__
#define syncpointMap(pMemory, pCallContext, pParams, pCpuMapping) syncpointMap_DISPATCH(pMemory, pCallContext, pParams, pCpuMapping)
#define syncpointUnmap_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memUnmap__
#define syncpointUnmap(pMemory, pCallContext, pCpuMapping) syncpointUnmap_DISPATCH(pMemory, pCallContext, pCpuMapping)
#define syncpointGetMemInterMapParams_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memGetMemInterMapParams__
#define syncpointGetMemInterMapParams(pMemory, pParams) syncpointGetMemInterMapParams_DISPATCH(pMemory, pParams)
#define syncpointCheckMemInterUnmap_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memCheckMemInterUnmap__
#define syncpointCheckMemInterUnmap(pMemory, bSubdeviceHandleProvided) syncpointCheckMemInterUnmap_DISPATCH(pMemory, bSubdeviceHandleProvided)
#define syncpointGetMemoryMappingDescriptor_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memGetMemoryMappingDescriptor__
#define syncpointGetMemoryMappingDescriptor(pMemory, ppMemDesc) syncpointGetMemoryMappingDescriptor_DISPATCH(pMemory, ppMemDesc)
#define syncpointCheckCopyPermissions_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memCheckCopyPermissions__
#define syncpointCheckCopyPermissions(pMemory, pDstGpu, pDstDevice) syncpointCheckCopyPermissions_DISPATCH(pMemory, pDstGpu, pDstDevice)
#define syncpointIsReady_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memIsReady__
#define syncpointIsReady(pMemory, bCopyConstructorContext) syncpointIsReady_DISPATCH(pMemory, bCopyConstructorContext)
#define syncpointIsGpuMapAllowed_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memIsGpuMapAllowed__
#define syncpointIsGpuMapAllowed(pMemory, pGpu) syncpointIsGpuMapAllowed_DISPATCH(pMemory, pGpu)
#define syncpointIsExportAllowed_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__memIsExportAllowed__
#define syncpointIsExportAllowed(pMemory) syncpointIsExportAllowed_DISPATCH(pMemory)
#define syncpointAccessCallback_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__rmresAccessCallback__
#define syncpointAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) syncpointAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define syncpointShareCallback_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__rmresShareCallback__
#define syncpointShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) syncpointShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define syncpointControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__rmresControlSerialization_Prologue__
#define syncpointControlSerialization_Prologue(pResource, pCallContext, pParams) syncpointControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define syncpointControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__rmresControlSerialization_Epilogue__
#define syncpointControlSerialization_Epilogue(pResource, pCallContext, pParams) syncpointControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define syncpointControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__rmresControl_Prologue__
#define syncpointControl_Prologue(pResource, pCallContext, pParams) syncpointControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define syncpointControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__rmresControl_Epilogue__
#define syncpointControl_Epilogue(pResource, pCallContext, pParams) syncpointControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define syncpointPreDestruct_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resPreDestruct__
#define syncpointPreDestruct(pResource) syncpointPreDestruct_DISPATCH(pResource)
#define syncpointControlFilter_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resControlFilter__
#define syncpointControlFilter(pResource, pCallContext, pParams) syncpointControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define syncpointIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsPartialUnmapSupported__
#define syncpointIsPartialUnmapSupported(pResource) syncpointIsPartialUnmapSupported_DISPATCH(pResource)
#define syncpointMapTo_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resMapTo__
#define syncpointMapTo(pResource, pParams) syncpointMapTo_DISPATCH(pResource, pParams)
#define syncpointUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmapFrom__
#define syncpointUnmapFrom(pResource, pParams) syncpointUnmapFrom_DISPATCH(pResource, pParams)
#define syncpointGetRefCount_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resGetRefCount__
#define syncpointGetRefCount(pResource) syncpointGetRefCount_DISPATCH(pResource)
#define syncpointAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__resAddAdditionalDependants__
#define syncpointAddAdditionalDependants(pClient, pResource, pReference) syncpointAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NvBool syncpointCanCopy_DISPATCH(struct SyncpointMemory *pSyncpointMemory) {
    return pSyncpointMemory->__syncpointCanCopy__(pSyncpointMemory);
}

static inline NV_STATUS syncpointIsDuplicate_DISPATCH(struct SyncpointMemory *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return pMemory->__syncpointIsDuplicate__(pMemory, hMemory, pDuplicate);
}

static inline NV_STATUS syncpointGetMapAddrSpace_DISPATCH(struct SyncpointMemory *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pMemory->__syncpointGetMapAddrSpace__(pMemory, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS syncpointControl_DISPATCH(struct SyncpointMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pMemory->__syncpointControl__(pMemory, pCallContext, pParams);
}

static inline NV_STATUS syncpointMap_DISPATCH(struct SyncpointMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pMemory->__syncpointMap__(pMemory, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS syncpointUnmap_DISPATCH(struct SyncpointMemory *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pMemory->__syncpointUnmap__(pMemory, pCallContext, pCpuMapping);
}

static inline NV_STATUS syncpointGetMemInterMapParams_DISPATCH(struct SyncpointMemory *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pMemory->__syncpointGetMemInterMapParams__(pMemory, pParams);
}

static inline NV_STATUS syncpointCheckMemInterUnmap_DISPATCH(struct SyncpointMemory *pMemory, NvBool bSubdeviceHandleProvided) {
    return pMemory->__syncpointCheckMemInterUnmap__(pMemory, bSubdeviceHandleProvided);
}

static inline NV_STATUS syncpointGetMemoryMappingDescriptor_DISPATCH(struct SyncpointMemory *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return pMemory->__syncpointGetMemoryMappingDescriptor__(pMemory, ppMemDesc);
}

static inline NV_STATUS syncpointCheckCopyPermissions_DISPATCH(struct SyncpointMemory *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return pMemory->__syncpointCheckCopyPermissions__(pMemory, pDstGpu, pDstDevice);
}

static inline NV_STATUS syncpointIsReady_DISPATCH(struct SyncpointMemory *pMemory, NvBool bCopyConstructorContext) {
    return pMemory->__syncpointIsReady__(pMemory, bCopyConstructorContext);
}

static inline NvBool syncpointIsGpuMapAllowed_DISPATCH(struct SyncpointMemory *pMemory, struct OBJGPU *pGpu) {
    return pMemory->__syncpointIsGpuMapAllowed__(pMemory, pGpu);
}

static inline NvBool syncpointIsExportAllowed_DISPATCH(struct SyncpointMemory *pMemory) {
    return pMemory->__syncpointIsExportAllowed__(pMemory);
}

static inline NvBool syncpointAccessCallback_DISPATCH(struct SyncpointMemory *pResource, RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__syncpointAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool syncpointShareCallback_DISPATCH(struct SyncpointMemory *pResource, RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__syncpointShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS syncpointControlSerialization_Prologue_DISPATCH(struct SyncpointMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__syncpointControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void syncpointControlSerialization_Epilogue_DISPATCH(struct SyncpointMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__syncpointControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS syncpointControl_Prologue_DISPATCH(struct SyncpointMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__syncpointControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void syncpointControl_Epilogue_DISPATCH(struct SyncpointMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__syncpointControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline void syncpointPreDestruct_DISPATCH(struct SyncpointMemory *pResource) {
    pResource->__syncpointPreDestruct__(pResource);
}

static inline NV_STATUS syncpointControlFilter_DISPATCH(struct SyncpointMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__syncpointControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool syncpointIsPartialUnmapSupported_DISPATCH(struct SyncpointMemory *pResource) {
    return pResource->__syncpointIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS syncpointMapTo_DISPATCH(struct SyncpointMemory *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__syncpointMapTo__(pResource, pParams);
}

static inline NV_STATUS syncpointUnmapFrom_DISPATCH(struct SyncpointMemory *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__syncpointUnmapFrom__(pResource, pParams);
}

static inline NvU32 syncpointGetRefCount_DISPATCH(struct SyncpointMemory *pResource) {
    return pResource->__syncpointGetRefCount__(pResource);
}

static inline void syncpointAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct SyncpointMemory *pResource, RsResourceRef *pReference) {
    pResource->__syncpointAddAdditionalDependants__(pClient, pResource, pReference);
}

NvBool syncpointCanCopy_IMPL(struct SyncpointMemory *pSyncpointMemory);

NV_STATUS syncpointConstruct_IMPL(struct SyncpointMemory *arg_pSyncpointMemory, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_syncpointConstruct(arg_pSyncpointMemory, arg_pCallContext, arg_pParams) syncpointConstruct_IMPL(arg_pSyncpointMemory, arg_pCallContext, arg_pParams)
#undef PRIVATE_FIELD


#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_SYNCPOINT_MEM_NVOC_H_
