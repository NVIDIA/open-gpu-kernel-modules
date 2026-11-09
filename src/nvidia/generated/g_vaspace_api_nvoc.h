
#ifndef _G_VASPACE_API_NVOC_H_
#define _G_VASPACE_API_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 1993-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_vaspace_api_nvoc.h"

#ifndef VASPACE_API_H
#define VASPACE_API_H

#include "core/core.h"
#include "mem_mgr/vaspace.h"
#include "rmapi/client.h"
#include "gpu/gpu_resource.h"
#include "ctrl/ctrl90f1.h"
#include "rmapi/control.h" // for macro RMCTRL_EXPORT etc.

#define RM_INVALID_VASPACE_HANDLE 0xFFFFFFFF

/*!
 * Actions to manage the Server RM page levels (only used by Split VAS mechanism)
 */
typedef enum
{
    VASPACEAPI_MANAGE_PAGE_LEVELS_RESERVE,
    VASPACEAPI_MANAGE_PAGE_LEVELS_RELEASE,
    VASPACEAPI_MANAGE_PAGE_LEVELS_TRIM,
} VASPACEAPI_MANAGE_PAGE_LEVELS_ACTION;


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_VASPACE_API_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__VaSpaceApi;
struct NVOC_METADATA__GpuResource;


struct VaSpaceApi {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__VaSpaceApi *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuResource __nvoc_base_GpuResource;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^4
    struct RsResource *__nvoc_pbase_RsResource;    // res super^3
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^3
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super^2
    struct GpuResource *__nvoc_pbase_GpuResource;    // gpures super
    struct VaSpaceApi *__nvoc_pbase_VaSpaceApi;    // vaspaceapi

    // Data members
    struct OBJVASPACE *pVASpace;
    void *sva_handle;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__VaSpaceApi {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResource metadata__GpuResource;
};

#ifndef __nvoc_class_id_VaSpaceApi
#define __nvoc_class_id_VaSpaceApi 0xcd048bu
typedef struct VaSpaceApi VaSpaceApi;
#endif /* __nvoc_class_id_VaSpaceApi */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_VaSpaceApi;

#define __staticCast_VaSpaceApi(pThis) \
    ((pThis)->__nvoc_pbase_VaSpaceApi)

#ifdef __nvoc_vaspace_api_h_disabled
#define __dynamicCast_VaSpaceApi(pThis) ((VaSpaceApi*) NULL)
#else //__nvoc_vaspace_api_h_disabled
#define __dynamicCast_VaSpaceApi(pThis) \
    ((VaSpaceApi*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(VaSpaceApi)))
#endif //__nvoc_vaspace_api_h_disabled

NV_STATUS __nvoc_objCreateDynamic_VaSpaceApi(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_VaSpaceApi(VaSpaceApi**, Dynamic*, NvU32, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __objCreate_VaSpaceApi(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams) \
    __nvoc_objCreate_VaSpaceApi((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pCallContext, pParams)


// Wrapper macros for implementation functions
NV_STATUS vaspaceapiConstruct_IMPL(struct VaSpaceApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __nvoc_vaspaceapiConstruct(pResource, pCallContext, pParams) vaspaceapiConstruct_IMPL(pResource, pCallContext, pParams)

NV_STATUS vaspaceapiCopyConstruct_IMPL(struct VaSpaceApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#ifdef __nvoc_vaspace_api_h_disabled
static inline NV_STATUS vaspaceapiCopyConstruct(struct VaSpaceApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {
    NV_ASSERT_FAILED_PRECOMP("VaSpaceApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_vaspace_api_h_disabled
#define vaspaceapiCopyConstruct(pResource, pCallContext, pParams) vaspaceapiCopyConstruct_IMPL(pResource, pCallContext, pParams)
#endif // __nvoc_vaspace_api_h_disabled

void vaspaceapiDestruct_IMPL(struct VaSpaceApi *pResource);
#define __nvoc_vaspaceapiDestruct(pResource) vaspaceapiDestruct_IMPL(pResource)

NV_STATUS vaspaceapiCtrlCmdVaspaceGetGmmuFormat_IMPL(struct VaSpaceApi *pVaspaceApi, NV90F1_CTRL_VASPACE_GET_GMMU_FORMAT_PARAMS *pGmmuFormatParams);
#ifdef __nvoc_vaspace_api_h_disabled
static inline NV_STATUS vaspaceapiCtrlCmdVaspaceGetGmmuFormat(struct VaSpaceApi *pVaspaceApi, NV90F1_CTRL_VASPACE_GET_GMMU_FORMAT_PARAMS *pGmmuFormatParams) {
    NV_ASSERT_FAILED_PRECOMP("VaSpaceApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_vaspace_api_h_disabled
#define vaspaceapiCtrlCmdVaspaceGetGmmuFormat(pVaspaceApi, pGmmuFormatParams) vaspaceapiCtrlCmdVaspaceGetGmmuFormat_IMPL(pVaspaceApi, pGmmuFormatParams)
#endif // __nvoc_vaspace_api_h_disabled

NV_STATUS vaspaceapiCtrlCmdVaspaceGetPageLevelInfo_IMPL(struct VaSpaceApi *pVaspaceApi, NV90F1_CTRL_VASPACE_GET_PAGE_LEVEL_INFO_PARAMS *pPageLevelInfoParams);
#ifdef __nvoc_vaspace_api_h_disabled
static inline NV_STATUS vaspaceapiCtrlCmdVaspaceGetPageLevelInfo(struct VaSpaceApi *pVaspaceApi, NV90F1_CTRL_VASPACE_GET_PAGE_LEVEL_INFO_PARAMS *pPageLevelInfoParams) {
    NV_ASSERT_FAILED_PRECOMP("VaSpaceApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_vaspace_api_h_disabled
#define vaspaceapiCtrlCmdVaspaceGetPageLevelInfo(pVaspaceApi, pPageLevelInfoParams) vaspaceapiCtrlCmdVaspaceGetPageLevelInfo_IMPL(pVaspaceApi, pPageLevelInfoParams)
#endif // __nvoc_vaspace_api_h_disabled

NV_STATUS vaspaceapiCtrlCmdVaspaceReserveEntries_IMPL(struct VaSpaceApi *pVaspaceApi, NV90F1_CTRL_VASPACE_RESERVE_ENTRIES_PARAMS *pReserveEntriesParams);
#ifdef __nvoc_vaspace_api_h_disabled
static inline NV_STATUS vaspaceapiCtrlCmdVaspaceReserveEntries(struct VaSpaceApi *pVaspaceApi, NV90F1_CTRL_VASPACE_RESERVE_ENTRIES_PARAMS *pReserveEntriesParams) {
    NV_ASSERT_FAILED_PRECOMP("VaSpaceApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_vaspace_api_h_disabled
#define vaspaceapiCtrlCmdVaspaceReserveEntries(pVaspaceApi, pReserveEntriesParams) vaspaceapiCtrlCmdVaspaceReserveEntries_IMPL(pVaspaceApi, pReserveEntriesParams)
#endif // __nvoc_vaspace_api_h_disabled

NV_STATUS vaspaceapiCtrlCmdVaspaceReleaseEntries_IMPL(struct VaSpaceApi *pVaspaceApi, NV90F1_CTRL_VASPACE_RELEASE_ENTRIES_PARAMS *pReleaseEntriesParams);
#ifdef __nvoc_vaspace_api_h_disabled
static inline NV_STATUS vaspaceapiCtrlCmdVaspaceReleaseEntries(struct VaSpaceApi *pVaspaceApi, NV90F1_CTRL_VASPACE_RELEASE_ENTRIES_PARAMS *pReleaseEntriesParams) {
    NV_ASSERT_FAILED_PRECOMP("VaSpaceApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_vaspace_api_h_disabled
#define vaspaceapiCtrlCmdVaspaceReleaseEntries(pVaspaceApi, pReleaseEntriesParams) vaspaceapiCtrlCmdVaspaceReleaseEntries_IMPL(pVaspaceApi, pReleaseEntriesParams)
#endif // __nvoc_vaspace_api_h_disabled

NV_STATUS vaspaceapiCtrlCmdVaspaceCopyServerReservedPdes_IMPL(struct VaSpaceApi *pVaspaceApi, NV90F1_CTRL_VASPACE_COPY_SERVER_RESERVED_PDES_PARAMS *pCopyServerReservedPdesParams);
#ifdef __nvoc_vaspace_api_h_disabled
static inline NV_STATUS vaspaceapiCtrlCmdVaspaceCopyServerReservedPdes(struct VaSpaceApi *pVaspaceApi, NV90F1_CTRL_VASPACE_COPY_SERVER_RESERVED_PDES_PARAMS *pCopyServerReservedPdesParams) {
    NV_ASSERT_FAILED_PRECOMP("VaSpaceApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_vaspace_api_h_disabled
#define vaspaceapiCtrlCmdVaspaceCopyServerReservedPdes(pVaspaceApi, pCopyServerReservedPdesParams) vaspaceapiCtrlCmdVaspaceCopyServerReservedPdes_IMPL(pVaspaceApi, pCopyServerReservedPdesParams)
#endif // __nvoc_vaspace_api_h_disabled

NV_STATUS vaspaceapiCtrlCmdVaspaceGetHostRmManagedSize_IMPL(struct VaSpaceApi *pVaspaceApi, NV90F1_CTRL_VASPACE_GET_HOST_RM_MANAGED_SIZE_PARAMS *pVaspaceGetHostRmManagedSizeParams);
#ifdef __nvoc_vaspace_api_h_disabled
static inline NV_STATUS vaspaceapiCtrlCmdVaspaceGetHostRmManagedSize(struct VaSpaceApi *pVaspaceApi, NV90F1_CTRL_VASPACE_GET_HOST_RM_MANAGED_SIZE_PARAMS *pVaspaceGetHostRmManagedSizeParams) {
    NV_ASSERT_FAILED_PRECOMP("VaSpaceApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_vaspace_api_h_disabled
#define vaspaceapiCtrlCmdVaspaceGetHostRmManagedSize(pVaspaceApi, pVaspaceGetHostRmManagedSizeParams) vaspaceapiCtrlCmdVaspaceGetHostRmManagedSize_IMPL(pVaspaceApi, pVaspaceGetHostRmManagedSizeParams)
#endif // __nvoc_vaspace_api_h_disabled


// Wrapper macros for halified functions
#define vaspaceapiCanCopy_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define vaspaceapiCanCopy(pResource) vaspaceapiCanCopy_DISPATCH(pResource)
#define vaspaceapiControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__
#define vaspaceapiControl(pGpuResource, pCallContext, pParams) vaspaceapiControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define vaspaceapiMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__
#define vaspaceapiMap(pGpuResource, pCallContext, pParams, pCpuMapping) vaspaceapiMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define vaspaceapiUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__
#define vaspaceapiUnmap(pGpuResource, pCallContext, pCpuMapping) vaspaceapiUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define vaspaceapiShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__
#define vaspaceapiShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) vaspaceapiShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define vaspaceapiGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__
#define vaspaceapiGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) vaspaceapiGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define vaspaceapiGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__
#define vaspaceapiGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) vaspaceapiGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define vaspaceapiInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__
#define vaspaceapiInternalControlForward(pGpuResource, command, pParams, size) vaspaceapiInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define vaspaceapiGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__
#define vaspaceapiGetInternalObjectHandle(pGpuResource) vaspaceapiGetInternalObjectHandle_DISPATCH(pGpuResource)
#define vaspaceapiAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__
#define vaspaceapiAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) vaspaceapiAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define vaspaceapiGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define vaspaceapiGetMemInterMapParams(pRmResource, pParams) vaspaceapiGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define vaspaceapiCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define vaspaceapiCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) vaspaceapiCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define vaspaceapiGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define vaspaceapiGetMemoryMappingDescriptor(pRmResource, ppMemDesc) vaspaceapiGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define vaspaceapiControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__
#define vaspaceapiControlSerialization_Prologue(pResource, pCallContext, pParams) vaspaceapiControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define vaspaceapiControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__
#define vaspaceapiControlSerialization_Epilogue(pResource, pCallContext, pParams) vaspaceapiControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define vaspaceapiControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__
#define vaspaceapiControl_Prologue(pResource, pCallContext, pParams) vaspaceapiControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define vaspaceapiControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__
#define vaspaceapiControl_Epilogue(pResource, pCallContext, pParams) vaspaceapiControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define vaspaceapiIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define vaspaceapiIsDuplicate(pResource, hMemory, pDuplicate) vaspaceapiIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define vaspaceapiPreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define vaspaceapiPreDestruct(pResource) vaspaceapiPreDestruct_DISPATCH(pResource)
#define vaspaceapiControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define vaspaceapiControlFilter(pResource, pCallContext, pParams) vaspaceapiControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define vaspaceapiIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define vaspaceapiIsPartialUnmapSupported(pResource) vaspaceapiIsPartialUnmapSupported_DISPATCH(pResource)
#define vaspaceapiMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define vaspaceapiMapTo(pResource, pParams) vaspaceapiMapTo_DISPATCH(pResource, pParams)
#define vaspaceapiUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define vaspaceapiUnmapFrom(pResource, pParams) vaspaceapiUnmapFrom_DISPATCH(pResource, pParams)
#define vaspaceapiGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define vaspaceapiGetRefCount(pResource) vaspaceapiGetRefCount_DISPATCH(pResource)
#define vaspaceapiAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define vaspaceapiAddAdditionalDependants(pClient, pResource, pReference) vaspaceapiAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NvBool vaspaceapiCanCopy_DISPATCH(struct VaSpaceApi *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__(staticCast(pResource, RsResource));
}

static inline NV_STATUS vaspaceapiControl_DISPATCH(struct VaSpaceApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__(staticCast(pGpuResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS vaspaceapiMap_DISPATCH(struct VaSpaceApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__(staticCast(pGpuResource, RsResource), pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS vaspaceapiUnmap_DISPATCH(struct VaSpaceApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__(staticCast(pGpuResource, RsResource), pCallContext, pCpuMapping);
}

static inline NvBool vaspaceapiShareCallback_DISPATCH(struct VaSpaceApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__(staticCast(pGpuResource, RsResource), pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS vaspaceapiGetRegBaseOffsetAndSize_DISPATCH(struct VaSpaceApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__(staticCast(pGpuResource, GpuResource), pGpu, pOffset, pSize);
}

static inline NV_STATUS vaspaceapiGetMapAddrSpace_DISPATCH(struct VaSpaceApi *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__(staticCast(pGpuResource, GpuResource), pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS vaspaceapiInternalControlForward_DISPATCH(struct VaSpaceApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__(staticCast(pGpuResource, GpuResource), command, pParams, size);
}

static inline NvHandle vaspaceapiGetInternalObjectHandle_DISPATCH(struct VaSpaceApi *pGpuResource) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__(staticCast(pGpuResource, GpuResource));
}

static inline NvBool vaspaceapiAccessCallback_DISPATCH(struct VaSpaceApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__(staticCast(pResource, RsResource), pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS vaspaceapiGetMemInterMapParams_DISPATCH(struct VaSpaceApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__(staticCast(pRmResource, RmResource), pParams);
}

static inline NV_STATUS vaspaceapiCheckMemInterUnmap_DISPATCH(struct VaSpaceApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__(staticCast(pRmResource, RmResource), bSubdeviceHandleProvided);
}

static inline NV_STATUS vaspaceapiGetMemoryMappingDescriptor_DISPATCH(struct VaSpaceApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__(staticCast(pRmResource, RmResource), ppMemDesc);
}

static inline NV_STATUS vaspaceapiControlSerialization_Prologue_DISPATCH(struct VaSpaceApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void vaspaceapiControlSerialization_Epilogue_DISPATCH(struct VaSpaceApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS vaspaceapiControl_Prologue_DISPATCH(struct VaSpaceApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void vaspaceapiControl_Epilogue_DISPATCH(struct VaSpaceApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS vaspaceapiIsDuplicate_DISPATCH(struct VaSpaceApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__(staticCast(pResource, RsResource), hMemory, pDuplicate);
}

static inline void vaspaceapiPreDestruct_DISPATCH(struct VaSpaceApi *pResource) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__(staticCast(pResource, RsResource));
}

static inline NV_STATUS vaspaceapiControlFilter_DISPATCH(struct VaSpaceApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool vaspaceapiIsPartialUnmapSupported_DISPATCH(struct VaSpaceApi *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__(staticCast(pResource, RsResource));
}

static inline NV_STATUS vaspaceapiMapTo_DISPATCH(struct VaSpaceApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__(staticCast(pResource, RsResource), pParams);
}

static inline NV_STATUS vaspaceapiUnmapFrom_DISPATCH(struct VaSpaceApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__(staticCast(pResource, RsResource), pParams);
}

static inline NvU32 vaspaceapiGetRefCount_DISPATCH(struct VaSpaceApi *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__(staticCast(pResource, RsResource));
}

static inline void vaspaceapiAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct VaSpaceApi *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__(pClient, staticCast(pResource, RsResource), pReference);
}

// Virtual method declarations and/or inline definitions
NvBool vaspaceapiCanCopy_IMPL(struct VaSpaceApi *pResource);

// Exported method declarations and/or inline definitions
NV_STATUS vaspaceapiCtrlCmdVaspaceGetGmmuFormat_IMPL(struct VaSpaceApi *pVaspaceApi, NV90F1_CTRL_VASPACE_GET_GMMU_FORMAT_PARAMS *pGmmuFormatParams);

NV_STATUS vaspaceapiCtrlCmdVaspaceGetPageLevelInfo_IMPL(struct VaSpaceApi *pVaspaceApi, NV90F1_CTRL_VASPACE_GET_PAGE_LEVEL_INFO_PARAMS *pPageLevelInfoParams);

NV_STATUS vaspaceapiCtrlCmdVaspaceReserveEntries_IMPL(struct VaSpaceApi *pVaspaceApi, NV90F1_CTRL_VASPACE_RESERVE_ENTRIES_PARAMS *pReserveEntriesParams);

NV_STATUS vaspaceapiCtrlCmdVaspaceReleaseEntries_IMPL(struct VaSpaceApi *pVaspaceApi, NV90F1_CTRL_VASPACE_RELEASE_ENTRIES_PARAMS *pReleaseEntriesParams);

NV_STATUS vaspaceapiCtrlCmdVaspaceCopyServerReservedPdes_IMPL(struct VaSpaceApi *pVaspaceApi, NV90F1_CTRL_VASPACE_COPY_SERVER_RESERVED_PDES_PARAMS *pCopyServerReservedPdesParams);

NV_STATUS vaspaceapiCtrlCmdVaspaceGetHostRmManagedSize_IMPL(struct VaSpaceApi *pVaspaceApi, NV90F1_CTRL_VASPACE_GET_HOST_RM_MANAGED_SIZE_PARAMS *pVaspaceGetHostRmManagedSizeParams);

// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


#endif // VASPACE_API_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_VASPACE_API_NVOC_H_
