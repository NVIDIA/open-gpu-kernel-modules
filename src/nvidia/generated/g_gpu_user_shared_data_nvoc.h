
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


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__GpuUserSharedData;
struct NVOC_METADATA__Memory;


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

    // Data members
    NvU64 polledDataMask;
    NvU32 pollingIntervalMs;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__GpuUserSharedData {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__Memory metadata__Memory;
};

#ifndef __nvoc_class_id_GpuUserSharedData
#define __nvoc_class_id_GpuUserSharedData 0x5e7d1fu
typedef struct GpuUserSharedData GpuUserSharedData;
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

NV_STATUS __nvoc_objCreateDynamic_GpuUserSharedData(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_GpuUserSharedData(GpuUserSharedData**, Dynamic*, NvU32, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __objCreate_GpuUserSharedData(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams) \
    __nvoc_objCreate_GpuUserSharedData((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pCallContext, pParams)


// Wrapper macros for implementation functions
NV_STATUS gpushareddataConstruct_IMPL(struct GpuUserSharedData *pData, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __nvoc_gpushareddataConstruct(pData, pCallContext, pParams) gpushareddataConstruct_IMPL(pData, pCallContext, pParams)

void gpushareddataDestruct_IMPL(struct GpuUserSharedData *pData);
#define __nvoc_gpushareddataDestruct(pData) gpushareddataDestruct_IMPL(pData)

NV_STATUS gpushareddataCtrlCmdRequestDataPoll_IMPL(struct GpuUserSharedData *pData, NV00DE_CTRL_REQUEST_DATA_POLL_PARAMS *pParams);
#ifdef __nvoc_gpu_user_shared_data_h_disabled
static inline NV_STATUS gpushareddataCtrlCmdRequestDataPoll(struct GpuUserSharedData *pData, NV00DE_CTRL_REQUEST_DATA_POLL_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("GpuUserSharedData was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_gpu_user_shared_data_h_disabled
#define gpushareddataCtrlCmdRequestDataPoll(pData, pParams) gpushareddataCtrlCmdRequestDataPoll_IMPL(pData, pParams)
#endif // __nvoc_gpu_user_shared_data_h_disabled

NV_STATUS gpushareddataCtrlCmdRequestPollInterval_IMPL(struct GpuUserSharedData *pData, NV00DE_CTRL_REQUEST_POLL_INTERVAL_PARAM *pParams);
#ifdef __nvoc_gpu_user_shared_data_h_disabled
static inline NV_STATUS gpushareddataCtrlCmdRequestPollInterval(struct GpuUserSharedData *pData, NV00DE_CTRL_REQUEST_POLL_INTERVAL_PARAM *pParams) {
    NV_ASSERT_FAILED_PRECOMP("GpuUserSharedData was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_gpu_user_shared_data_h_disabled
#define gpushareddataCtrlCmdRequestPollInterval(pData, pParams) gpushareddataCtrlCmdRequestPollInterval_IMPL(pData, pParams)
#endif // __nvoc_gpu_user_shared_data_h_disabled


// Wrapper macros for halified functions
#define gpushareddataCanCopy_FNPTR(pData) pData->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define gpushareddataCanCopy(pData) gpushareddataCanCopy_DISPATCH(pData)
#define gpushareddataIsDuplicate_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define gpushareddataIsDuplicate(pMemory, hMemory, pDuplicate) gpushareddataIsDuplicate_DISPATCH(pMemory, hMemory, pDuplicate)
#define gpushareddataGetMapAddrSpace_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memGetMapAddrSpace__
#define gpushareddataGetMapAddrSpace(pMemory, pCallContext, mapFlags, pAddrSpace) gpushareddataGetMapAddrSpace_DISPATCH(pMemory, pCallContext, mapFlags, pAddrSpace)
#define gpushareddataControl_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__
#define gpushareddataControl(pMemory, pCallContext, pParams) gpushareddataControl_DISPATCH(pMemory, pCallContext, pParams)
#define gpushareddataMap_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__
#define gpushareddataMap(pMemory, pCallContext, pParams, pCpuMapping) gpushareddataMap_DISPATCH(pMemory, pCallContext, pParams, pCpuMapping)
#define gpushareddataUnmap_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__
#define gpushareddataUnmap(pMemory, pCallContext, pCpuMapping) gpushareddataUnmap_DISPATCH(pMemory, pCallContext, pCpuMapping)
#define gpushareddataGetMemInterMapParams_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define gpushareddataGetMemInterMapParams(pMemory, pParams) gpushareddataGetMemInterMapParams_DISPATCH(pMemory, pParams)
#define gpushareddataCheckMemInterUnmap_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define gpushareddataCheckMemInterUnmap(pMemory, bSubdeviceHandleProvided) gpushareddataCheckMemInterUnmap_DISPATCH(pMemory, bSubdeviceHandleProvided)
#define gpushareddataGetMemoryMappingDescriptor_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define gpushareddataGetMemoryMappingDescriptor(pMemory, ppMemDesc) gpushareddataGetMemoryMappingDescriptor_DISPATCH(pMemory, ppMemDesc)
#define gpushareddataCheckCopyPermissions_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memCheckCopyPermissions__
#define gpushareddataCheckCopyPermissions(pMemory, pDstGpu, pDstDevice) gpushareddataCheckCopyPermissions_DISPATCH(pMemory, pDstGpu, pDstDevice)
#define gpushareddataIsReady_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsReady__
#define gpushareddataIsReady(pMemory, bCopyConstructorContext) gpushareddataIsReady_DISPATCH(pMemory, bCopyConstructorContext)
#define gpushareddataIsGpuMapAllowed_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsGpuMapAllowed__
#define gpushareddataIsGpuMapAllowed(pMemory, pGpu) gpushareddataIsGpuMapAllowed_DISPATCH(pMemory, pGpu)
#define gpushareddataIsExportAllowed_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsExportAllowed__
#define gpushareddataIsExportAllowed(pMemory) gpushareddataIsExportAllowed_DISPATCH(pMemory)
#define gpushareddataAccessCallback_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__
#define gpushareddataAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) gpushareddataAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define gpushareddataShareCallback_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__
#define gpushareddataShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) gpushareddataShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define gpushareddataControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__
#define gpushareddataControlSerialization_Prologue(pResource, pCallContext, pParams) gpushareddataControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define gpushareddataControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__
#define gpushareddataControlSerialization_Epilogue(pResource, pCallContext, pParams) gpushareddataControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define gpushareddataControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__
#define gpushareddataControl_Prologue(pResource, pCallContext, pParams) gpushareddataControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define gpushareddataControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__
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
    return pData->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__(staticCast(pData, RsResource));
}

static inline NV_STATUS gpushareddataIsDuplicate_DISPATCH(struct GpuUserSharedData *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__(staticCast(pMemory, RsResource), hMemory, pDuplicate);
}

static inline NV_STATUS gpushareddataGetMapAddrSpace_DISPATCH(struct GpuUserSharedData *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memGetMapAddrSpace__(staticCast(pMemory, Memory), pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS gpushareddataControl_DISPATCH(struct GpuUserSharedData *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__(staticCast(pMemory, RsResource), pCallContext, pParams);
}

static inline NV_STATUS gpushareddataMap_DISPATCH(struct GpuUserSharedData *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__(staticCast(pMemory, RsResource), pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS gpushareddataUnmap_DISPATCH(struct GpuUserSharedData *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__(staticCast(pMemory, RsResource), pCallContext, pCpuMapping);
}

static inline NV_STATUS gpushareddataGetMemInterMapParams_DISPATCH(struct GpuUserSharedData *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__(staticCast(pMemory, RmResource), pParams);
}

static inline NV_STATUS gpushareddataCheckMemInterUnmap_DISPATCH(struct GpuUserSharedData *pMemory, NvBool bSubdeviceHandleProvided) {
    return pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__(staticCast(pMemory, RmResource), bSubdeviceHandleProvided);
}

static inline NV_STATUS gpushareddataGetMemoryMappingDescriptor_DISPATCH(struct GpuUserSharedData *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return pMemory->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__(staticCast(pMemory, RmResource), ppMemDesc);
}

static inline NV_STATUS gpushareddataCheckCopyPermissions_DISPATCH(struct GpuUserSharedData *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memCheckCopyPermissions__(staticCast(pMemory, Memory), pDstGpu, pDstDevice);
}

static inline NV_STATUS gpushareddataIsReady_DISPATCH(struct GpuUserSharedData *pMemory, NvBool bCopyConstructorContext) {
    return pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsReady__(staticCast(pMemory, Memory), bCopyConstructorContext);
}

static inline NvBool gpushareddataIsGpuMapAllowed_DISPATCH(struct GpuUserSharedData *pMemory, struct OBJGPU *pGpu) {
    return pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsGpuMapAllowed__(staticCast(pMemory, Memory), pGpu);
}

static inline NvBool gpushareddataIsExportAllowed_DISPATCH(struct GpuUserSharedData *pMemory) {
    return pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsExportAllowed__(staticCast(pMemory, Memory));
}

static inline NvBool gpushareddataAccessCallback_DISPATCH(struct GpuUserSharedData *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__(staticCast(pResource, RsResource), pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool gpushareddataShareCallback_DISPATCH(struct GpuUserSharedData *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__(staticCast(pResource, RsResource), pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS gpushareddataControlSerialization_Prologue_DISPATCH(struct GpuUserSharedData *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void gpushareddataControlSerialization_Epilogue_DISPATCH(struct GpuUserSharedData *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS gpushareddataControl_Prologue_DISPATCH(struct GpuUserSharedData *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void gpushareddataControl_Epilogue_DISPATCH(struct GpuUserSharedData *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void gpushareddataPreDestruct_DISPATCH(struct GpuUserSharedData *pResource) {
    pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__(staticCast(pResource, RsResource));
}

static inline NV_STATUS gpushareddataControlFilter_DISPATCH(struct GpuUserSharedData *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool gpushareddataIsPartialUnmapSupported_DISPATCH(struct GpuUserSharedData *pResource) {
    return pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__(staticCast(pResource, RsResource));
}

static inline NV_STATUS gpushareddataMapTo_DISPATCH(struct GpuUserSharedData *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__(staticCast(pResource, RsResource), pParams);
}

static inline NV_STATUS gpushareddataUnmapFrom_DISPATCH(struct GpuUserSharedData *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__(staticCast(pResource, RsResource), pParams);
}

static inline NvU32 gpushareddataGetRefCount_DISPATCH(struct GpuUserSharedData *pResource) {
    return pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__(staticCast(pResource, RsResource));
}

static inline void gpushareddataAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct GpuUserSharedData *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__(pClient, staticCast(pResource, RsResource), pReference);
}

// Virtual method declarations and/or inline definitions
NvBool gpushareddataCanCopy_IMPL(struct GpuUserSharedData *pData);

// Exported method declarations and/or inline definitions
NV_STATUS gpushareddataCtrlCmdRequestDataPoll_IMPL(struct GpuUserSharedData *pData, NV00DE_CTRL_REQUEST_DATA_POLL_PARAMS *pParams);

NV_STATUS gpushareddataCtrlCmdRequestPollInterval_IMPL(struct GpuUserSharedData *pData, NV00DE_CTRL_REQUEST_POLL_INTERVAL_PARAM *pParams);

// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


#endif // GPU_USER_SHARED_DATA_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_GPU_USER_SHARED_DATA_NVOC_H_
