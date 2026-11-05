
#ifndef _G_SYNCPOINT_MEM_NVOC_H_
#define _G_SYNCPOINT_MEM_NVOC_H_

// Version of generated metadata structures
#ifdef NVOC_METADATA_VERSION
#undef NVOC_METADATA_VERSION
#endif
#define NVOC_METADATA_VERSION 2

#include "nvoc/runtime.h"
#include "nvoc/rtti.h"

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


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__SyncpointMemory;
struct NVOC_METADATA__Memory;


struct SyncpointMemory {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__SyncpointMemory *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct Memory __nvoc_base_Memory;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^4
    struct RsResource *__nvoc_pbase_RsResource;    // res super^3
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^3
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super^2
    struct Memory *__nvoc_pbase_Memory;    // mem super
    struct SyncpointMemory *__nvoc_pbase_SyncpointMemory;    // syncpoint
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__SyncpointMemory {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__Memory metadata__Memory;
};

#ifndef __nvoc_class_id_SyncpointMemory
#define __nvoc_class_id_SyncpointMemory 0x529defu
typedef struct SyncpointMemory SyncpointMemory;
#endif /* __nvoc_class_id_SyncpointMemory */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_SyncpointMemory;

#define __staticCast_SyncpointMemory(pThis) \
    ((pThis)->__nvoc_pbase_SyncpointMemory)

#ifdef __nvoc_syncpoint_mem_h_disabled
#define __dynamicCast_SyncpointMemory(pThis) ((SyncpointMemory*) NULL)
#else //__nvoc_syncpoint_mem_h_disabled
#define __dynamicCast_SyncpointMemory(pThis) \
    ((SyncpointMemory*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(SyncpointMemory)))
#endif //__nvoc_syncpoint_mem_h_disabled

NV_STATUS __nvoc_objCreateDynamic_SyncpointMemory(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_SyncpointMemory(SyncpointMemory**, Dynamic*, NvU32, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __objCreate_SyncpointMemory(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams) \
    __nvoc_objCreate_SyncpointMemory((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pCallContext, pParams)


// Wrapper macros for implementation functions
NV_STATUS syncpointConstruct_IMPL(struct SyncpointMemory *pSyncpointMemory, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __nvoc_syncpointConstruct(pSyncpointMemory, pCallContext, pParams) syncpointConstruct_IMPL(pSyncpointMemory, pCallContext, pParams)


// Wrapper macros for halified functions
#define syncpointCanCopy_FNPTR(pSyncpointMemory) pSyncpointMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define syncpointCanCopy(pSyncpointMemory) syncpointCanCopy_DISPATCH(pSyncpointMemory)
#define syncpointIsDuplicate_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define syncpointIsDuplicate(pMemory, hMemory, pDuplicate) syncpointIsDuplicate_DISPATCH(pMemory, hMemory, pDuplicate)
#define syncpointGetMapAddrSpace_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memGetMapAddrSpace__
#define syncpointGetMapAddrSpace(pMemory, pCallContext, mapFlags, pAddrSpace) syncpointGetMapAddrSpace_DISPATCH(pMemory, pCallContext, mapFlags, pAddrSpace)
#define syncpointControl_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__
#define syncpointControl(pMemory, pCallContext, pParams) syncpointControl_DISPATCH(pMemory, pCallContext, pParams)
#define syncpointMap_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__
#define syncpointMap(pMemory, pCallContext, pParams, pCpuMapping) syncpointMap_DISPATCH(pMemory, pCallContext, pParams, pCpuMapping)
#define syncpointUnmap_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__
#define syncpointUnmap(pMemory, pCallContext, pCpuMapping) syncpointUnmap_DISPATCH(pMemory, pCallContext, pCpuMapping)
#define syncpointGetMemInterMapParams_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define syncpointGetMemInterMapParams(pMemory, pParams) syncpointGetMemInterMapParams_DISPATCH(pMemory, pParams)
#define syncpointCheckMemInterUnmap_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define syncpointCheckMemInterUnmap(pMemory, bSubdeviceHandleProvided) syncpointCheckMemInterUnmap_DISPATCH(pMemory, bSubdeviceHandleProvided)
#define syncpointGetMemoryMappingDescriptor_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define syncpointGetMemoryMappingDescriptor(pMemory, ppMemDesc) syncpointGetMemoryMappingDescriptor_DISPATCH(pMemory, ppMemDesc)
#define syncpointCheckCopyPermissions_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memCheckCopyPermissions__
#define syncpointCheckCopyPermissions(pMemory, pDstGpu, pDstDevice) syncpointCheckCopyPermissions_DISPATCH(pMemory, pDstGpu, pDstDevice)
#define syncpointIsReady_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsReady__
#define syncpointIsReady(pMemory, bCopyConstructorContext) syncpointIsReady_DISPATCH(pMemory, bCopyConstructorContext)
#define syncpointIsGpuMapAllowed_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsGpuMapAllowed__
#define syncpointIsGpuMapAllowed(pMemory, pGpu) syncpointIsGpuMapAllowed_DISPATCH(pMemory, pGpu)
#define syncpointIsExportAllowed_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsExportAllowed__
#define syncpointIsExportAllowed(pMemory) syncpointIsExportAllowed_DISPATCH(pMemory)
#define syncpointAccessCallback_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__
#define syncpointAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) syncpointAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define syncpointShareCallback_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__
#define syncpointShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) syncpointShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define syncpointControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__
#define syncpointControlSerialization_Prologue(pResource, pCallContext, pParams) syncpointControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define syncpointControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__
#define syncpointControlSerialization_Epilogue(pResource, pCallContext, pParams) syncpointControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define syncpointControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__
#define syncpointControl_Prologue(pResource, pCallContext, pParams) syncpointControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define syncpointControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__
#define syncpointControl_Epilogue(pResource, pCallContext, pParams) syncpointControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define syncpointPreDestruct_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define syncpointPreDestruct(pResource) syncpointPreDestruct_DISPATCH(pResource)
#define syncpointControlFilter_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define syncpointControlFilter(pResource, pCallContext, pParams) syncpointControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define syncpointIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define syncpointIsPartialUnmapSupported(pResource) syncpointIsPartialUnmapSupported_DISPATCH(pResource)
#define syncpointMapTo_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define syncpointMapTo(pResource, pParams) syncpointMapTo_DISPATCH(pResource, pParams)
#define syncpointUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define syncpointUnmapFrom(pResource, pParams) syncpointUnmapFrom_DISPATCH(pResource, pParams)
#define syncpointGetRefCount_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define syncpointGetRefCount(pResource) syncpointGetRefCount_DISPATCH(pResource)
#define syncpointAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define syncpointAddAdditionalDependants(pClient, pResource, pReference) syncpointAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NvBool syncpointCanCopy_DISPATCH(struct SyncpointMemory *pSyncpointMemory) {
    return pSyncpointMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__(staticCast(pSyncpointMemory, RsResource));
}

static inline NV_STATUS syncpointIsDuplicate_DISPATCH(struct SyncpointMemory *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__(staticCast(pMemory, RsResource), hMemory, pDuplicate);
}

static inline NV_STATUS syncpointGetMapAddrSpace_DISPATCH(struct SyncpointMemory *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memGetMapAddrSpace__(staticCast(pMemory, Memory), pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS syncpointControl_DISPATCH(struct SyncpointMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__(staticCast(pMemory, RsResource), pCallContext, pParams);
}

static inline NV_STATUS syncpointMap_DISPATCH(struct SyncpointMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__(staticCast(pMemory, RsResource), pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS syncpointUnmap_DISPATCH(struct SyncpointMemory *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__(staticCast(pMemory, RsResource), pCallContext, pCpuMapping);
}

static inline NV_STATUS syncpointGetMemInterMapParams_DISPATCH(struct SyncpointMemory *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__(staticCast(pMemory, RmResource), pParams);
}

static inline NV_STATUS syncpointCheckMemInterUnmap_DISPATCH(struct SyncpointMemory *pMemory, NvBool bSubdeviceHandleProvided) {
    return pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__(staticCast(pMemory, RmResource), bSubdeviceHandleProvided);
}

static inline NV_STATUS syncpointGetMemoryMappingDescriptor_DISPATCH(struct SyncpointMemory *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__(staticCast(pMemory, RmResource), ppMemDesc);
}

static inline NV_STATUS syncpointCheckCopyPermissions_DISPATCH(struct SyncpointMemory *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memCheckCopyPermissions__(staticCast(pMemory, Memory), pDstGpu, pDstDevice);
}

static inline NV_STATUS syncpointIsReady_DISPATCH(struct SyncpointMemory *pMemory, NvBool bCopyConstructorContext) {
    return pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsReady__(staticCast(pMemory, Memory), bCopyConstructorContext);
}

static inline NvBool syncpointIsGpuMapAllowed_DISPATCH(struct SyncpointMemory *pMemory, struct OBJGPU *pGpu) {
    return pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsGpuMapAllowed__(staticCast(pMemory, Memory), pGpu);
}

static inline NvBool syncpointIsExportAllowed_DISPATCH(struct SyncpointMemory *pMemory) {
    return pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsExportAllowed__(staticCast(pMemory, Memory));
}

static inline NvBool syncpointAccessCallback_DISPATCH(struct SyncpointMemory *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__(staticCast(pResource, RsResource), pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool syncpointShareCallback_DISPATCH(struct SyncpointMemory *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__(staticCast(pResource, RsResource), pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS syncpointControlSerialization_Prologue_DISPATCH(struct SyncpointMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void syncpointControlSerialization_Epilogue_DISPATCH(struct SyncpointMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS syncpointControl_Prologue_DISPATCH(struct SyncpointMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void syncpointControl_Epilogue_DISPATCH(struct SyncpointMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void syncpointPreDestruct_DISPATCH(struct SyncpointMemory *pResource) {
    pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__(staticCast(pResource, RsResource));
}

static inline NV_STATUS syncpointControlFilter_DISPATCH(struct SyncpointMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool syncpointIsPartialUnmapSupported_DISPATCH(struct SyncpointMemory *pResource) {
    return pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__(staticCast(pResource, RsResource));
}

static inline NV_STATUS syncpointMapTo_DISPATCH(struct SyncpointMemory *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__(staticCast(pResource, RsResource), pParams);
}

static inline NV_STATUS syncpointUnmapFrom_DISPATCH(struct SyncpointMemory *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__(staticCast(pResource, RsResource), pParams);
}

static inline NvU32 syncpointGetRefCount_DISPATCH(struct SyncpointMemory *pResource) {
    return pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__(staticCast(pResource, RsResource));
}

static inline void syncpointAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct SyncpointMemory *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__(pClient, staticCast(pResource, RsResource), pReference);
}

// Virtual method declarations and/or inline definitions
NvBool syncpointCanCopy_IMPL(struct SyncpointMemory *pSyncpointMemory);

// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_SYNCPOINT_MEM_NVOC_H_
