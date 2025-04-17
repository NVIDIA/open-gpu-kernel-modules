
#ifndef _G_GPU_USER_SHARED_DATA_NVOC_H_
#define _G_GPU_USER_SHARED_DATA_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2022-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_gpu_user_shared_data_nvoc.h"

#ifndef GPU_USER_SHARED_DATA_H
#define GPU_USER_SHARED_DATA_H

#include "core/core.h"
#include "mem_mgr/mem.h"
#include "gpu/gpu.h"
#include "nvoc/utility.h"
#include "ctrl/ctrl00de.h"

// ****************************************************************************
//                          Type definitions
// ****************************************************************************

/*!
 * RM internal class representing RM_USER_SHARED_DATA
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_GPU_USER_SHARED_DATA_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__GpuUserSharedData;
struct NVOC_METADATA__Memory;
struct NVOC_VTABLE__GpuUserSharedData;


struct GpuUserSharedData {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__GpuUserSharedData *__nvoc_metadata_ptr;
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
    struct GpuUserSharedData *__nvoc_pbase_GpuUserSharedData;    // gpushareddata

    // Vtable with 1 per-object function pointer
    NV_STATUS (*__gpushareddataCtrlCmdRequestDataPoll__)(struct GpuUserSharedData * /*this*/, NV00DE_CTRL_REQUEST_DATA_POLL_PARAMS *);  // exported (id=0xde0001)

    // Data members
    NvU64 polledDataMask;
};


// Vtable with 26 per-class function pointers
struct NVOC_VTABLE__GpuUserSharedData {
    NvBool (*__gpushareddataCanCopy__)(struct GpuUserSharedData * /*this*/);  // virtual override (res) base (mem)
    NV_STATUS (*__gpushareddataIsDuplicate__)(struct GpuUserSharedData * /*this*/, NvHandle, NvBool *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__gpushareddataGetMapAddrSpace__)(struct GpuUserSharedData * /*this*/, CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__gpushareddataControl__)(struct GpuUserSharedData * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__gpushareddataMap__)(struct GpuUserSharedData * /*this*/, CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, RsCpuMapping *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__gpushareddataUnmap__)(struct GpuUserSharedData * /*this*/, CALL_CONTEXT *, RsCpuMapping *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__gpushareddataGetMemInterMapParams__)(struct GpuUserSharedData * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__gpushareddataCheckMemInterUnmap__)(struct GpuUserSharedData * /*this*/, NvBool);  // inline virtual inherited (mem) base (mem) body
    NV_STATUS (*__gpushareddataGetMemoryMappingDescriptor__)(struct GpuUserSharedData * /*this*/, MEMORY_DESCRIPTOR **);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__gpushareddataCheckCopyPermissions__)(struct GpuUserSharedData * /*this*/, struct OBJGPU *, struct Device *);  // inline virtual inherited (mem) base (mem) body
    NV_STATUS (*__gpushareddataIsReady__)(struct GpuUserSharedData * /*this*/, NvBool);  // virtual inherited (mem) base (mem)
    NvBool (*__gpushareddataIsGpuMapAllowed__)(struct GpuUserSharedData * /*this*/, struct OBJGPU *);  // inline virtual inherited (mem) base (mem) body
    NvBool (*__gpushareddataIsExportAllowed__)(struct GpuUserSharedData * /*this*/);  // inline virtual inherited (mem) base (mem) body
    NvBool (*__gpushareddataAccessCallback__)(struct GpuUserSharedData * /*this*/, RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (mem)
    NvBool (*__gpushareddataShareCallback__)(struct GpuUserSharedData * /*this*/, RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (rmres) base (mem)
    NV_STATUS (*__gpushareddataControlSerialization_Prologue__)(struct GpuUserSharedData * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (mem)
    void (*__gpushareddataControlSerialization_Epilogue__)(struct GpuUserSharedData * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (mem)
    NV_STATUS (*__gpushareddataControl_Prologue__)(struct GpuUserSharedData * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (mem)
    void (*__gpushareddataControl_Epilogue__)(struct GpuUserSharedData * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (mem)
    void (*__gpushareddataPreDestruct__)(struct GpuUserSharedData * /*this*/);  // virtual inherited (res) base (mem)
    NV_STATUS (*__gpushareddataControlFilter__)(struct GpuUserSharedData * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (mem)
    NvBool (*__gpushareddataIsPartialUnmapSupported__)(struct GpuUserSharedData * /*this*/);  // inline virtual inherited (res) base (mem) body
    NV_STATUS (*__gpushareddataMapTo__)(struct GpuUserSharedData * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (mem)
    NV_STATUS (*__gpushareddataUnmapFrom__)(struct GpuUserSharedData * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (mem)
    NvU32 (*__gpushareddataGetRefCount__)(struct GpuUserSharedData * /*this*/);  // virtual inherited (res) base (mem)
    void (*__gpushareddataAddAdditionalDependants__)(struct RsClient *, struct GpuUserSharedData * /*this*/, RsResourceRef *);  // virtual inherited (res) base (mem)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__GpuUserSharedData {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__Memory metadata__Memory;
    const struct NVOC_VTABLE__GpuUserSharedData vtable;
};

#ifndef __NVOC_CLASS_GpuUserSharedData_TYPEDEF__
#define __NVOC_CLASS_GpuUserSharedData_TYPEDEF__
typedef struct GpuUserSharedData GpuUserSharedData;
#endif /* __NVOC_CLASS_GpuUserSharedData_TYPEDEF__ */

#ifndef __nvoc_class_id_GpuUserSharedData
#define __nvoc_class_id_GpuUserSharedData 0x5e7d1f
#endif /* __nvoc_class_id_GpuUserSharedData */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuUserSharedData;

#define __staticCast_GpuUserSharedData(pThis) \
    ((pThis)->__nvoc_pbase_GpuUserSharedData)

#ifdef __nvoc_gpu_user_shared_data_h_disabled
#define __dynamicCast_GpuUserSharedData(pThis) ((GpuUserSharedData*) NULL)
#else //__nvoc_gpu_user_shared_data_h_disabled
#define __dynamicCast_GpuUserSharedData(pThis) \
    ((GpuUserSharedData*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(GpuUserSharedData)))
#endif //__nvoc_gpu_user_shared_data_h_disabled

NV_STATUS __nvoc_objCreateDynamic_GpuUserSharedData(GpuUserSharedData**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_GpuUserSharedData(GpuUserSharedData**, Dynamic*, NvU32, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __objCreate_GpuUserSharedData(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_GpuUserSharedData((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define gpushareddataCanCopy_FNPTR(pData) pData->__nvoc_metadata_ptr->vtable.__gpushareddataCanCopy__
#define gpushareddataCanCopy(pData) gpushareddataCanCopy_DISPATCH(pData)
#define gpushareddataCtrlCmdRequestDataPoll_FNPTR(pData) pData->__gpushareddataCtrlCmdRequestDataPoll__
#define gpushareddataCtrlCmdRequestDataPoll(pData, pParams) gpushareddataCtrlCmdRequestDataPoll_DISPATCH(pData, pParams)
#define gpushareddataIsDuplicate_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsDuplicate__
#define gpushareddataIsDuplicate(pMemory, hMemory, pDuplicate) gpushareddataIsDuplicate_DISPATCH(pMemory, hMemory, pDuplicate)
#define gpushareddataGetMapAddrSpace_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memGetMapAddrSpace__
#define gpushareddataGetMapAddrSpace(pMemory, pCallContext, mapFlags, pAddrSpace) gpushareddataGetMapAddrSpace_DISPATCH(pMemory, pCallContext, mapFlags, pAddrSpace)
#define gpushareddataControl_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memControl__
#define gpushareddataControl(pMemory, pCallContext, pParams) gpushareddataControl_DISPATCH(pMemory, pCallContext, pParams)
#define gpushareddataMap_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memMap__
#define gpushareddataMap(pMemory, pCallContext, pParams, pCpuMapping) gpushareddataMap_DISPATCH(pMemory, pCallContext, pParams, pCpuMapping)
#define gpushareddataUnmap_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memUnmap__
#define gpushareddataUnmap(pMemory, pCallContext, pCpuMapping) gpushareddataUnmap_DISPATCH(pMemory, pCallContext, pCpuMapping)
#define gpushareddataGetMemInterMapParams_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memGetMemInterMapParams__
#define gpushareddataGetMemInterMapParams(pMemory, pParams) gpushareddataGetMemInterMapParams_DISPATCH(pMemory, pParams)
#define gpushareddataCheckMemInterUnmap_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memCheckMemInterUnmap__
#define gpushareddataCheckMemInterUnmap(pMemory, bSubdeviceHandleProvided) gpushareddataCheckMemInterUnmap_DISPATCH(pMemory, bSubdeviceHandleProvided)
#define gpushareddataGetMemoryMappingDescriptor_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memGetMemoryMappingDescriptor__
#define gpushareddataGetMemoryMappingDescriptor(pMemory, ppMemDesc) gpushareddataGetMemoryMappingDescriptor_DISPATCH(pMemory, ppMemDesc)
#define gpushareddataCheckCopyPermissions_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memCheckCopyPermissions__
#define gpushareddataCheckCopyPermissions(pMemory, pDstGpu, pDstDevice) gpushareddataCheckCopyPermissions_DISPATCH(pMemory, pDstGpu, pDstDevice)
#define gpushareddataIsReady_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsReady__
#define gpushareddataIsReady(pMemory, bCopyConstructorContext) gpushareddataIsReady_DISPATCH(pMemory, bCopyConstructorContext)
#define gpushareddataIsGpuMapAllowed_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsGpuMapAllowed__
#define gpushareddataIsGpuMapAllowed(pMemory, pGpu) gpushareddataIsGpuMapAllowed_DISPATCH(pMemory, pGpu)
#define gpushareddataIsExportAllowed_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsExportAllowed__
#define gpushareddataIsExportAllowed(pMemory) gpushareddataIsExportAllowed_DISPATCH(pMemory)
#define gpushareddataAccessCallback_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresAccessCallback__
#define gpushareddataAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) gpushareddataAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define gpushareddataShareCallback_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresShareCallback__
#define gpushareddataShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) gpushareddataShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define gpushareddataControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Prologue__
#define gpushareddataControlSerialization_Prologue(pResource, pCallContext, pParams) gpushareddataControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define gpushareddataControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Epilogue__
#define gpushareddataControlSerialization_Epilogue(pResource, pCallContext, pParams) gpushareddataControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define gpushareddataControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Prologue__
#define gpushareddataControl_Prologue(pResource, pCallContext, pParams) gpushareddataControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define gpushareddataControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Epilogue__
#define gpushareddataControl_Epilogue(pResource, pCallContext, pParams) gpushareddataControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define gpushareddataPreDestruct_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define gpushareddataPreDestruct(pResource) gpushareddataPreDestruct_DISPATCH(pResource)
#define gpushareddataControlFilter_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define gpushareddataControlFilter(pResource, pCallContext, pParams) gpushareddataControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define gpushareddataIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define gpushareddataIsPartialUnmapSupported(pResource) gpushareddataIsPartialUnmapSupported_DISPATCH(pResource)
#define gpushareddataMapTo_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define gpushareddataMapTo(pResource, pParams) gpushareddataMapTo_DISPATCH(pResource, pParams)
#define gpushareddataUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define gpushareddataUnmapFrom(pResource, pParams) gpushareddataUnmapFrom_DISPATCH(pResource, pParams)
#define gpushareddataGetRefCount_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define gpushareddataGetRefCount(pResource) gpushareddataGetRefCount_DISPATCH(pResource)
#define gpushareddataAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define gpushareddataAddAdditionalDependants(pClient, pResource, pReference) gpushareddataAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NvBool gpushareddataCanCopy_DISPATCH(struct GpuUserSharedData *pData) {
    return pData->__nvoc_metadata_ptr->vtable.__gpushareddataCanCopy__(pData);
}

static inline NV_STATUS gpushareddataCtrlCmdRequestDataPoll_DISPATCH(struct GpuUserSharedData *pData, NV00DE_CTRL_REQUEST_DATA_POLL_PARAMS *pParams) {
    return pData->__gpushareddataCtrlCmdRequestDataPoll__(pData, pParams);
}

static inline NV_STATUS gpushareddataIsDuplicate_DISPATCH(struct GpuUserSharedData *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return pMemory->__nvoc_metadata_ptr->vtable.__gpushareddataIsDuplicate__(pMemory, hMemory, pDuplicate);
}

static inline NV_STATUS gpushareddataGetMapAddrSpace_DISPATCH(struct GpuUserSharedData *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pMemory->__nvoc_metadata_ptr->vtable.__gpushareddataGetMapAddrSpace__(pMemory, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS gpushareddataControl_DISPATCH(struct GpuUserSharedData *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pMemory->__nvoc_metadata_ptr->vtable.__gpushareddataControl__(pMemory, pCallContext, pParams);
}

static inline NV_STATUS gpushareddataMap_DISPATCH(struct GpuUserSharedData *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pMemory->__nvoc_metadata_ptr->vtable.__gpushareddataMap__(pMemory, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS gpushareddataUnmap_DISPATCH(struct GpuUserSharedData *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pMemory->__nvoc_metadata_ptr->vtable.__gpushareddataUnmap__(pMemory, pCallContext, pCpuMapping);
}

static inline NV_STATUS gpushareddataGetMemInterMapParams_DISPATCH(struct GpuUserSharedData *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pMemory->__nvoc_metadata_ptr->vtable.__gpushareddataGetMemInterMapParams__(pMemory, pParams);
}

static inline NV_STATUS gpushareddataCheckMemInterUnmap_DISPATCH(struct GpuUserSharedData *pMemory, NvBool bSubdeviceHandleProvided) {
    return pMemory->__nvoc_metadata_ptr->vtable.__gpushareddataCheckMemInterUnmap__(pMemory, bSubdeviceHandleProvided);
}

static inline NV_STATUS gpushareddataGetMemoryMappingDescriptor_DISPATCH(struct GpuUserSharedData *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return pMemory->__nvoc_metadata_ptr->vtable.__gpushareddataGetMemoryMappingDescriptor__(pMemory, ppMemDesc);
}

static inline NV_STATUS gpushareddataCheckCopyPermissions_DISPATCH(struct GpuUserSharedData *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return pMemory->__nvoc_metadata_ptr->vtable.__gpushareddataCheckCopyPermissions__(pMemory, pDstGpu, pDstDevice);
}

static inline NV_STATUS gpushareddataIsReady_DISPATCH(struct GpuUserSharedData *pMemory, NvBool bCopyConstructorContext) {
    return pMemory->__nvoc_metadata_ptr->vtable.__gpushareddataIsReady__(pMemory, bCopyConstructorContext);
}

static inline NvBool gpushareddataIsGpuMapAllowed_DISPATCH(struct GpuUserSharedData *pMemory, struct OBJGPU *pGpu) {
    return pMemory->__nvoc_metadata_ptr->vtable.__gpushareddataIsGpuMapAllowed__(pMemory, pGpu);
}

static inline NvBool gpushareddataIsExportAllowed_DISPATCH(struct GpuUserSharedData *pMemory) {
    return pMemory->__nvoc_metadata_ptr->vtable.__gpushareddataIsExportAllowed__(pMemory);
}

static inline NvBool gpushareddataAccessCallback_DISPATCH(struct GpuUserSharedData *pResource, RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_metadata_ptr->vtable.__gpushareddataAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool gpushareddataShareCallback_DISPATCH(struct GpuUserSharedData *pResource, RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__nvoc_metadata_ptr->vtable.__gpushareddataShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS gpushareddataControlSerialization_Prologue_DISPATCH(struct GpuUserSharedData *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__gpushareddataControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void gpushareddataControlSerialization_Epilogue_DISPATCH(struct GpuUserSharedData *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__gpushareddataControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS gpushareddataControl_Prologue_DISPATCH(struct GpuUserSharedData *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__gpushareddataControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void gpushareddataControl_Epilogue_DISPATCH(struct GpuUserSharedData *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__gpushareddataControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline void gpushareddataPreDestruct_DISPATCH(struct GpuUserSharedData *pResource) {
    pResource->__nvoc_metadata_ptr->vtable.__gpushareddataPreDestruct__(pResource);
}

static inline NV_STATUS gpushareddataControlFilter_DISPATCH(struct GpuUserSharedData *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__gpushareddataControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool gpushareddataIsPartialUnmapSupported_DISPATCH(struct GpuUserSharedData *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__gpushareddataIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS gpushareddataMapTo_DISPATCH(struct GpuUserSharedData *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__gpushareddataMapTo__(pResource, pParams);
}

static inline NV_STATUS gpushareddataUnmapFrom_DISPATCH(struct GpuUserSharedData *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__gpushareddataUnmapFrom__(pResource, pParams);
}

static inline NvU32 gpushareddataGetRefCount_DISPATCH(struct GpuUserSharedData *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__gpushareddataGetRefCount__(pResource);
}

static inline void gpushareddataAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct GpuUserSharedData *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_metadata_ptr->vtable.__gpushareddataAddAdditionalDependants__(pClient, pResource, pReference);
}

NvBool gpushareddataCanCopy_IMPL(struct GpuUserSharedData *pData);

NV_STATUS gpushareddataCtrlCmdRequestDataPoll_IMPL(struct GpuUserSharedData *pData, NV00DE_CTRL_REQUEST_DATA_POLL_PARAMS *pParams);

NV_STATUS gpushareddataConstruct_IMPL(struct GpuUserSharedData *arg_pData, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_gpushareddataConstruct(arg_pData, arg_pCallContext, arg_pParams) gpushareddataConstruct_IMPL(arg_pData, arg_pCallContext, arg_pParams)
void gpushareddataDestruct_IMPL(struct GpuUserSharedData *pData);

#define __nvoc_gpushareddataDestruct(pData) gpushareddataDestruct_IMPL(pData)
#undef PRIVATE_FIELD


#endif // GPU_USER_SHARED_DATA_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_GPU_USER_SHARED_DATA_NVOC_H_
