
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
 * SPDX-FileCopyrightText: Copyright (c) 1993-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__VaSpaceApi;
struct NVOC_METADATA__GpuResource;
struct NVOC_VTABLE__VaSpaceApi;


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

    // Vtable with 6 per-object function pointers
    NV_STATUS (*__vaspaceapiCtrlCmdVaspaceGetGmmuFormat__)(struct VaSpaceApi * /*this*/, NV90F1_CTRL_VASPACE_GET_GMMU_FORMAT_PARAMS *);  // exported (id=0x90f10101)
    NV_STATUS (*__vaspaceapiCtrlCmdVaspaceGetPageLevelInfo__)(struct VaSpaceApi * /*this*/, NV90F1_CTRL_VASPACE_GET_PAGE_LEVEL_INFO_PARAMS *);  // exported (id=0x90f10102)
    NV_STATUS (*__vaspaceapiCtrlCmdVaspaceReserveEntries__)(struct VaSpaceApi * /*this*/, NV90F1_CTRL_VASPACE_RESERVE_ENTRIES_PARAMS *);  // exported (id=0x90f10103)
    NV_STATUS (*__vaspaceapiCtrlCmdVaspaceReleaseEntries__)(struct VaSpaceApi * /*this*/, NV90F1_CTRL_VASPACE_RELEASE_ENTRIES_PARAMS *);  // exported (id=0x90f10104)
    NV_STATUS (*__vaspaceapiCtrlCmdVaspaceCopyServerReservedPdes__)(struct VaSpaceApi * /*this*/, NV90F1_CTRL_VASPACE_COPY_SERVER_RESERVED_PDES_PARAMS *);  // exported (id=0x90f10106)
    NV_STATUS (*__vaspaceapiCtrlCmdVaspaceGetHostRmManagedSize__)(struct VaSpaceApi * /*this*/, NV90F1_CTRL_VASPACE_GET_HOST_RM_MANAGED_SIZE_PARAMS *);  // exported (id=0x90f10107)

    // Data members
    struct OBJVASPACE *pVASpace;
};


// Vtable with 25 per-class function pointers
struct NVOC_VTABLE__VaSpaceApi {
    NvBool (*__vaspaceapiCanCopy__)(struct VaSpaceApi * /*this*/);  // virtual override (res) base (gpures)
    NV_STATUS (*__vaspaceapiControl__)(struct VaSpaceApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__vaspaceapiMap__)(struct VaSpaceApi * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__vaspaceapiUnmap__)(struct VaSpaceApi * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NvBool (*__vaspaceapiShareCallback__)(struct VaSpaceApi * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__vaspaceapiGetRegBaseOffsetAndSize__)(struct VaSpaceApi * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__vaspaceapiGetMapAddrSpace__)(struct VaSpaceApi * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__vaspaceapiInternalControlForward__)(struct VaSpaceApi * /*this*/, NvU32, void *, NvU32);  // virtual inherited (gpures) base (gpures)
    NvHandle (*__vaspaceapiGetInternalObjectHandle__)(struct VaSpaceApi * /*this*/);  // virtual inherited (gpures) base (gpures)
    NvBool (*__vaspaceapiAccessCallback__)(struct VaSpaceApi * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__vaspaceapiGetMemInterMapParams__)(struct VaSpaceApi * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__vaspaceapiCheckMemInterUnmap__)(struct VaSpaceApi * /*this*/, NvBool);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__vaspaceapiGetMemoryMappingDescriptor__)(struct VaSpaceApi * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__vaspaceapiControlSerialization_Prologue__)(struct VaSpaceApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__vaspaceapiControlSerialization_Epilogue__)(struct VaSpaceApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__vaspaceapiControl_Prologue__)(struct VaSpaceApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__vaspaceapiControl_Epilogue__)(struct VaSpaceApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__vaspaceapiIsDuplicate__)(struct VaSpaceApi * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (gpures)
    void (*__vaspaceapiPreDestruct__)(struct VaSpaceApi * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__vaspaceapiControlFilter__)(struct VaSpaceApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (gpures)
    NvBool (*__vaspaceapiIsPartialUnmapSupported__)(struct VaSpaceApi * /*this*/);  // inline virtual inherited (res) base (gpures) body
    NV_STATUS (*__vaspaceapiMapTo__)(struct VaSpaceApi * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__vaspaceapiUnmapFrom__)(struct VaSpaceApi * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (gpures)
    NvU32 (*__vaspaceapiGetRefCount__)(struct VaSpaceApi * /*this*/);  // virtual inherited (res) base (gpures)
    void (*__vaspaceapiAddAdditionalDependants__)(struct RsClient *, struct VaSpaceApi * /*this*/, RsResourceRef *);  // virtual inherited (res) base (gpures)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__VaSpaceApi {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResource metadata__GpuResource;
    const struct NVOC_VTABLE__VaSpaceApi vtable;
};

#ifndef __NVOC_CLASS_VaSpaceApi_TYPEDEF__
#define __NVOC_CLASS_VaSpaceApi_TYPEDEF__
typedef struct VaSpaceApi VaSpaceApi;
#endif /* __NVOC_CLASS_VaSpaceApi_TYPEDEF__ */

#ifndef __nvoc_class_id_VaSpaceApi
#define __nvoc_class_id_VaSpaceApi 0xcd048b
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

NV_STATUS __nvoc_objCreateDynamic_VaSpaceApi(VaSpaceApi**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_VaSpaceApi(VaSpaceApi**, Dynamic*, NvU32, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __objCreate_VaSpaceApi(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_VaSpaceApi((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define vaspaceapiCanCopy_FNPTR(pResource) pResource->__nvoc_metadata_ptr->vtable.__vaspaceapiCanCopy__
#define vaspaceapiCanCopy(pResource) vaspaceapiCanCopy_DISPATCH(pResource)
#define vaspaceapiCtrlCmdVaspaceGetGmmuFormat_FNPTR(pVaspaceApi) pVaspaceApi->__vaspaceapiCtrlCmdVaspaceGetGmmuFormat__
#define vaspaceapiCtrlCmdVaspaceGetGmmuFormat(pVaspaceApi, pGmmuFormatParams) vaspaceapiCtrlCmdVaspaceGetGmmuFormat_DISPATCH(pVaspaceApi, pGmmuFormatParams)
#define vaspaceapiCtrlCmdVaspaceGetPageLevelInfo_FNPTR(pVaspaceApi) pVaspaceApi->__vaspaceapiCtrlCmdVaspaceGetPageLevelInfo__
#define vaspaceapiCtrlCmdVaspaceGetPageLevelInfo(pVaspaceApi, pPageLevelInfoParams) vaspaceapiCtrlCmdVaspaceGetPageLevelInfo_DISPATCH(pVaspaceApi, pPageLevelInfoParams)
#define vaspaceapiCtrlCmdVaspaceReserveEntries_FNPTR(pVaspaceApi) pVaspaceApi->__vaspaceapiCtrlCmdVaspaceReserveEntries__
#define vaspaceapiCtrlCmdVaspaceReserveEntries(pVaspaceApi, pReserveEntriesParams) vaspaceapiCtrlCmdVaspaceReserveEntries_DISPATCH(pVaspaceApi, pReserveEntriesParams)
#define vaspaceapiCtrlCmdVaspaceReleaseEntries_FNPTR(pVaspaceApi) pVaspaceApi->__vaspaceapiCtrlCmdVaspaceReleaseEntries__
#define vaspaceapiCtrlCmdVaspaceReleaseEntries(pVaspaceApi, pReleaseEntriesParams) vaspaceapiCtrlCmdVaspaceReleaseEntries_DISPATCH(pVaspaceApi, pReleaseEntriesParams)
#define vaspaceapiCtrlCmdVaspaceCopyServerReservedPdes_FNPTR(pVaspaceApi) pVaspaceApi->__vaspaceapiCtrlCmdVaspaceCopyServerReservedPdes__
#define vaspaceapiCtrlCmdVaspaceCopyServerReservedPdes(pVaspaceApi, pCopyServerReservedPdesParams) vaspaceapiCtrlCmdVaspaceCopyServerReservedPdes_DISPATCH(pVaspaceApi, pCopyServerReservedPdesParams)
#define vaspaceapiCtrlCmdVaspaceGetHostRmManagedSize_FNPTR(pVaspaceApi) pVaspaceApi->__vaspaceapiCtrlCmdVaspaceGetHostRmManagedSize__
#define vaspaceapiCtrlCmdVaspaceGetHostRmManagedSize(pVaspaceApi, pVaspaceGetHostRmManagedSizeParams) vaspaceapiCtrlCmdVaspaceGetHostRmManagedSize_DISPATCH(pVaspaceApi, pVaspaceGetHostRmManagedSizeParams)
#define vaspaceapiControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresControl__
#define vaspaceapiControl(pGpuResource, pCallContext, pParams) vaspaceapiControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define vaspaceapiMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresMap__
#define vaspaceapiMap(pGpuResource, pCallContext, pParams, pCpuMapping) vaspaceapiMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define vaspaceapiUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresUnmap__
#define vaspaceapiUnmap(pGpuResource, pCallContext, pCpuMapping) vaspaceapiUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define vaspaceapiShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresShareCallback__
#define vaspaceapiShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) vaspaceapiShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define vaspaceapiGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__
#define vaspaceapiGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) vaspaceapiGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define vaspaceapiGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__
#define vaspaceapiGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) vaspaceapiGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define vaspaceapiInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__
#define vaspaceapiInternalControlForward(pGpuResource, command, pParams, size) vaspaceapiInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define vaspaceapiGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__
#define vaspaceapiGetInternalObjectHandle(pGpuResource) vaspaceapiGetInternalObjectHandle_DISPATCH(pGpuResource)
#define vaspaceapiAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresAccessCallback__
#define vaspaceapiAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) vaspaceapiAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define vaspaceapiGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define vaspaceapiGetMemInterMapParams(pRmResource, pParams) vaspaceapiGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define vaspaceapiCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define vaspaceapiCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) vaspaceapiCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define vaspaceapiGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define vaspaceapiGetMemoryMappingDescriptor(pRmResource, ppMemDesc) vaspaceapiGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define vaspaceapiControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Prologue__
#define vaspaceapiControlSerialization_Prologue(pResource, pCallContext, pParams) vaspaceapiControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define vaspaceapiControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Epilogue__
#define vaspaceapiControlSerialization_Epilogue(pResource, pCallContext, pParams) vaspaceapiControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define vaspaceapiControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Prologue__
#define vaspaceapiControl_Prologue(pResource, pCallContext, pParams) vaspaceapiControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define vaspaceapiControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Epilogue__
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
    return pResource->__nvoc_metadata_ptr->vtable.__vaspaceapiCanCopy__(pResource);
}

static inline NV_STATUS vaspaceapiCtrlCmdVaspaceGetGmmuFormat_DISPATCH(struct VaSpaceApi *pVaspaceApi, NV90F1_CTRL_VASPACE_GET_GMMU_FORMAT_PARAMS *pGmmuFormatParams) {
    return pVaspaceApi->__vaspaceapiCtrlCmdVaspaceGetGmmuFormat__(pVaspaceApi, pGmmuFormatParams);
}

static inline NV_STATUS vaspaceapiCtrlCmdVaspaceGetPageLevelInfo_DISPATCH(struct VaSpaceApi *pVaspaceApi, NV90F1_CTRL_VASPACE_GET_PAGE_LEVEL_INFO_PARAMS *pPageLevelInfoParams) {
    return pVaspaceApi->__vaspaceapiCtrlCmdVaspaceGetPageLevelInfo__(pVaspaceApi, pPageLevelInfoParams);
}

static inline NV_STATUS vaspaceapiCtrlCmdVaspaceReserveEntries_DISPATCH(struct VaSpaceApi *pVaspaceApi, NV90F1_CTRL_VASPACE_RESERVE_ENTRIES_PARAMS *pReserveEntriesParams) {
    return pVaspaceApi->__vaspaceapiCtrlCmdVaspaceReserveEntries__(pVaspaceApi, pReserveEntriesParams);
}

static inline NV_STATUS vaspaceapiCtrlCmdVaspaceReleaseEntries_DISPATCH(struct VaSpaceApi *pVaspaceApi, NV90F1_CTRL_VASPACE_RELEASE_ENTRIES_PARAMS *pReleaseEntriesParams) {
    return pVaspaceApi->__vaspaceapiCtrlCmdVaspaceReleaseEntries__(pVaspaceApi, pReleaseEntriesParams);
}

static inline NV_STATUS vaspaceapiCtrlCmdVaspaceCopyServerReservedPdes_DISPATCH(struct VaSpaceApi *pVaspaceApi, NV90F1_CTRL_VASPACE_COPY_SERVER_RESERVED_PDES_PARAMS *pCopyServerReservedPdesParams) {
    return pVaspaceApi->__vaspaceapiCtrlCmdVaspaceCopyServerReservedPdes__(pVaspaceApi, pCopyServerReservedPdesParams);
}

static inline NV_STATUS vaspaceapiCtrlCmdVaspaceGetHostRmManagedSize_DISPATCH(struct VaSpaceApi *pVaspaceApi, NV90F1_CTRL_VASPACE_GET_HOST_RM_MANAGED_SIZE_PARAMS *pVaspaceGetHostRmManagedSizeParams) {
    return pVaspaceApi->__vaspaceapiCtrlCmdVaspaceGetHostRmManagedSize__(pVaspaceApi, pVaspaceGetHostRmManagedSizeParams);
}

static inline NV_STATUS vaspaceapiControl_DISPATCH(struct VaSpaceApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__vaspaceapiControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS vaspaceapiMap_DISPATCH(struct VaSpaceApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__vaspaceapiMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS vaspaceapiUnmap_DISPATCH(struct VaSpaceApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__vaspaceapiUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NvBool vaspaceapiShareCallback_DISPATCH(struct VaSpaceApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__vaspaceapiShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS vaspaceapiGetRegBaseOffsetAndSize_DISPATCH(struct VaSpaceApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__vaspaceapiGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS vaspaceapiGetMapAddrSpace_DISPATCH(struct VaSpaceApi *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__vaspaceapiGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS vaspaceapiInternalControlForward_DISPATCH(struct VaSpaceApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__vaspaceapiInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NvHandle vaspaceapiGetInternalObjectHandle_DISPATCH(struct VaSpaceApi *pGpuResource) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__vaspaceapiGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool vaspaceapiAccessCallback_DISPATCH(struct VaSpaceApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_metadata_ptr->vtable.__vaspaceapiAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS vaspaceapiGetMemInterMapParams_DISPATCH(struct VaSpaceApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__vaspaceapiGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS vaspaceapiCheckMemInterUnmap_DISPATCH(struct VaSpaceApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__vaspaceapiCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS vaspaceapiGetMemoryMappingDescriptor_DISPATCH(struct VaSpaceApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__vaspaceapiGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS vaspaceapiControlSerialization_Prologue_DISPATCH(struct VaSpaceApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__vaspaceapiControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void vaspaceapiControlSerialization_Epilogue_DISPATCH(struct VaSpaceApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__vaspaceapiControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS vaspaceapiControl_Prologue_DISPATCH(struct VaSpaceApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__vaspaceapiControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void vaspaceapiControl_Epilogue_DISPATCH(struct VaSpaceApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__vaspaceapiControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS vaspaceapiIsDuplicate_DISPATCH(struct VaSpaceApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_metadata_ptr->vtable.__vaspaceapiIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void vaspaceapiPreDestruct_DISPATCH(struct VaSpaceApi *pResource) {
    pResource->__nvoc_metadata_ptr->vtable.__vaspaceapiPreDestruct__(pResource);
}

static inline NV_STATUS vaspaceapiControlFilter_DISPATCH(struct VaSpaceApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__vaspaceapiControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool vaspaceapiIsPartialUnmapSupported_DISPATCH(struct VaSpaceApi *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__vaspaceapiIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS vaspaceapiMapTo_DISPATCH(struct VaSpaceApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__vaspaceapiMapTo__(pResource, pParams);
}

static inline NV_STATUS vaspaceapiUnmapFrom_DISPATCH(struct VaSpaceApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__vaspaceapiUnmapFrom__(pResource, pParams);
}

static inline NvU32 vaspaceapiGetRefCount_DISPATCH(struct VaSpaceApi *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__vaspaceapiGetRefCount__(pResource);
}

static inline void vaspaceapiAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct VaSpaceApi *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_metadata_ptr->vtable.__vaspaceapiAddAdditionalDependants__(pClient, pResource, pReference);
}

NvBool vaspaceapiCanCopy_IMPL(struct VaSpaceApi *pResource);

NV_STATUS vaspaceapiCtrlCmdVaspaceGetGmmuFormat_IMPL(struct VaSpaceApi *pVaspaceApi, NV90F1_CTRL_VASPACE_GET_GMMU_FORMAT_PARAMS *pGmmuFormatParams);

NV_STATUS vaspaceapiCtrlCmdVaspaceGetPageLevelInfo_IMPL(struct VaSpaceApi *pVaspaceApi, NV90F1_CTRL_VASPACE_GET_PAGE_LEVEL_INFO_PARAMS *pPageLevelInfoParams);

NV_STATUS vaspaceapiCtrlCmdVaspaceReserveEntries_IMPL(struct VaSpaceApi *pVaspaceApi, NV90F1_CTRL_VASPACE_RESERVE_ENTRIES_PARAMS *pReserveEntriesParams);

NV_STATUS vaspaceapiCtrlCmdVaspaceReleaseEntries_IMPL(struct VaSpaceApi *pVaspaceApi, NV90F1_CTRL_VASPACE_RELEASE_ENTRIES_PARAMS *pReleaseEntriesParams);

NV_STATUS vaspaceapiCtrlCmdVaspaceCopyServerReservedPdes_IMPL(struct VaSpaceApi *pVaspaceApi, NV90F1_CTRL_VASPACE_COPY_SERVER_RESERVED_PDES_PARAMS *pCopyServerReservedPdesParams);

NV_STATUS vaspaceapiCtrlCmdVaspaceGetHostRmManagedSize_IMPL(struct VaSpaceApi *pVaspaceApi, NV90F1_CTRL_VASPACE_GET_HOST_RM_MANAGED_SIZE_PARAMS *pVaspaceGetHostRmManagedSizeParams);

NV_STATUS vaspaceapiConstruct_IMPL(struct VaSpaceApi *arg_pResource, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_vaspaceapiConstruct(arg_pResource, arg_pCallContext, arg_pParams) vaspaceapiConstruct_IMPL(arg_pResource, arg_pCallContext, arg_pParams)
NV_STATUS vaspaceapiCopyConstruct_IMPL(struct VaSpaceApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);

#ifdef __nvoc_vaspace_api_h_disabled
static inline NV_STATUS vaspaceapiCopyConstruct(struct VaSpaceApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {
    NV_ASSERT_FAILED_PRECOMP("VaSpaceApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_vaspace_api_h_disabled
#define vaspaceapiCopyConstruct(pResource, pCallContext, pParams) vaspaceapiCopyConstruct_IMPL(pResource, pCallContext, pParams)
#endif //__nvoc_vaspace_api_h_disabled

void vaspaceapiDestruct_IMPL(struct VaSpaceApi *pResource);

#define __nvoc_vaspaceapiDestruct(pResource) vaspaceapiDestruct_IMPL(pResource)
#undef PRIVATE_FIELD


#endif // VASPACE_API_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_VASPACE_API_NVOC_H_
