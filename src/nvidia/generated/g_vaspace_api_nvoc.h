#ifndef _G_VASPACE_API_NVOC_H_
#define _G_VASPACE_API_NVOC_H_
#include "nvoc/runtime.h"

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

#ifdef NVOC_VASPACE_API_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct VaSpaceApi {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct GpuResource __nvoc_base_GpuResource;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct GpuResource *__nvoc_pbase_GpuResource;
    struct VaSpaceApi *__nvoc_pbase_VaSpaceApi;
    NvBool (*__vaspaceapiCanCopy__)(struct VaSpaceApi *);
    NV_STATUS (*__vaspaceapiCtrlCmdVaspaceGetGmmuFormat__)(struct VaSpaceApi *, NV90F1_CTRL_VASPACE_GET_GMMU_FORMAT_PARAMS *);
    NV_STATUS (*__vaspaceapiCtrlCmdVaspaceGetPageLevelInfo__)(struct VaSpaceApi *, NV90F1_CTRL_VASPACE_GET_PAGE_LEVEL_INFO_PARAMS *);
    NV_STATUS (*__vaspaceapiCtrlCmdVaspaceReserveEntries__)(struct VaSpaceApi *, NV90F1_CTRL_VASPACE_RESERVE_ENTRIES_PARAMS *);
    NV_STATUS (*__vaspaceapiCtrlCmdVaspaceReleaseEntries__)(struct VaSpaceApi *, NV90F1_CTRL_VASPACE_RELEASE_ENTRIES_PARAMS *);
    NV_STATUS (*__vaspaceapiCtrlCmdVaspaceCopyServerReservedPdes__)(struct VaSpaceApi *, NV90F1_CTRL_VASPACE_COPY_SERVER_RESERVED_PDES_PARAMS *);
    NV_STATUS (*__vaspaceapiCtrlCmdVaspaceGetHostRmManagedSize__)(struct VaSpaceApi *, NV90F1_CTRL_VASPACE_GET_HOST_RM_MANAGED_SIZE_PARAMS *);
    NvBool (*__vaspaceapiShareCallback__)(struct VaSpaceApi *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__vaspaceapiCheckMemInterUnmap__)(struct VaSpaceApi *, NvBool);
    NV_STATUS (*__vaspaceapiMapTo__)(struct VaSpaceApi *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__vaspaceapiGetMapAddrSpace__)(struct VaSpaceApi *, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    NvU32 (*__vaspaceapiGetRefCount__)(struct VaSpaceApi *);
    void (*__vaspaceapiAddAdditionalDependants__)(struct RsClient *, struct VaSpaceApi *, RsResourceRef *);
    NV_STATUS (*__vaspaceapiControl_Prologue__)(struct VaSpaceApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__vaspaceapiGetRegBaseOffsetAndSize__)(struct VaSpaceApi *, struct OBJGPU *, NvU32 *, NvU32 *);
    NV_STATUS (*__vaspaceapiInternalControlForward__)(struct VaSpaceApi *, NvU32, void *, NvU32);
    NV_STATUS (*__vaspaceapiUnmapFrom__)(struct VaSpaceApi *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__vaspaceapiControl_Epilogue__)(struct VaSpaceApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__vaspaceapiControlLookup__)(struct VaSpaceApi *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NvHandle (*__vaspaceapiGetInternalObjectHandle__)(struct VaSpaceApi *);
    NV_STATUS (*__vaspaceapiControl__)(struct VaSpaceApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__vaspaceapiUnmap__)(struct VaSpaceApi *, struct CALL_CONTEXT *, struct RsCpuMapping *);
    NV_STATUS (*__vaspaceapiGetMemInterMapParams__)(struct VaSpaceApi *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__vaspaceapiGetMemoryMappingDescriptor__)(struct VaSpaceApi *, struct MEMORY_DESCRIPTOR **);
    NV_STATUS (*__vaspaceapiControlFilter__)(struct VaSpaceApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__vaspaceapiControlSerialization_Prologue__)(struct VaSpaceApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    void (*__vaspaceapiPreDestruct__)(struct VaSpaceApi *);
    NV_STATUS (*__vaspaceapiIsDuplicate__)(struct VaSpaceApi *, NvHandle, NvBool *);
    void (*__vaspaceapiControlSerialization_Epilogue__)(struct VaSpaceApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__vaspaceapiMap__)(struct VaSpaceApi *, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);
    NvBool (*__vaspaceapiAccessCallback__)(struct VaSpaceApi *, struct RsClient *, void *, RsAccessRight);
    struct OBJVASPACE *pVASpace;
    NvBool bDuped;
};

#ifndef __NVOC_CLASS_VaSpaceApi_TYPEDEF__
#define __NVOC_CLASS_VaSpaceApi_TYPEDEF__
typedef struct VaSpaceApi VaSpaceApi;
#endif /* __NVOC_CLASS_VaSpaceApi_TYPEDEF__ */

#ifndef __nvoc_class_id_VaSpaceApi
#define __nvoc_class_id_VaSpaceApi 0xcd048b
#endif /* __nvoc_class_id_VaSpaceApi */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_VaSpaceApi;

#define __staticCast_VaSpaceApi(pThis) \
    ((pThis)->__nvoc_pbase_VaSpaceApi)

#ifdef __nvoc_vaspace_api_h_disabled
#define __dynamicCast_VaSpaceApi(pThis) ((VaSpaceApi*)NULL)
#else //__nvoc_vaspace_api_h_disabled
#define __dynamicCast_VaSpaceApi(pThis) \
    ((VaSpaceApi*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(VaSpaceApi)))
#endif //__nvoc_vaspace_api_h_disabled


NV_STATUS __nvoc_objCreateDynamic_VaSpaceApi(VaSpaceApi**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_VaSpaceApi(VaSpaceApi**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_VaSpaceApi(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_VaSpaceApi((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define vaspaceapiCanCopy(pResource) vaspaceapiCanCopy_DISPATCH(pResource)
#define vaspaceapiCtrlCmdVaspaceGetGmmuFormat(pVaspaceApi, pGmmuFormatParams) vaspaceapiCtrlCmdVaspaceGetGmmuFormat_DISPATCH(pVaspaceApi, pGmmuFormatParams)
#define vaspaceapiCtrlCmdVaspaceGetPageLevelInfo(pVaspaceApi, pPageLevelInfoParams) vaspaceapiCtrlCmdVaspaceGetPageLevelInfo_DISPATCH(pVaspaceApi, pPageLevelInfoParams)
#define vaspaceapiCtrlCmdVaspaceReserveEntries(pVaspaceApi, pReserveEntriesParams) vaspaceapiCtrlCmdVaspaceReserveEntries_DISPATCH(pVaspaceApi, pReserveEntriesParams)
#define vaspaceapiCtrlCmdVaspaceReleaseEntries(pVaspaceApi, pReleaseEntriesParams) vaspaceapiCtrlCmdVaspaceReleaseEntries_DISPATCH(pVaspaceApi, pReleaseEntriesParams)
#define vaspaceapiCtrlCmdVaspaceCopyServerReservedPdes(pVaspaceApi, pCopyServerReservedPdesParams) vaspaceapiCtrlCmdVaspaceCopyServerReservedPdes_DISPATCH(pVaspaceApi, pCopyServerReservedPdesParams)
#define vaspaceapiCtrlCmdVaspaceGetHostRmManagedSize(pVaspaceApi, pVaspaceGetHostRmManagedSizeParams) vaspaceapiCtrlCmdVaspaceGetHostRmManagedSize_DISPATCH(pVaspaceApi, pVaspaceGetHostRmManagedSizeParams)
#define vaspaceapiShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) vaspaceapiShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define vaspaceapiCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) vaspaceapiCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define vaspaceapiMapTo(pResource, pParams) vaspaceapiMapTo_DISPATCH(pResource, pParams)
#define vaspaceapiGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) vaspaceapiGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define vaspaceapiGetRefCount(pResource) vaspaceapiGetRefCount_DISPATCH(pResource)
#define vaspaceapiAddAdditionalDependants(pClient, pResource, pReference) vaspaceapiAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define vaspaceapiControl_Prologue(pResource, pCallContext, pParams) vaspaceapiControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define vaspaceapiGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) vaspaceapiGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define vaspaceapiInternalControlForward(pGpuResource, command, pParams, size) vaspaceapiInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define vaspaceapiUnmapFrom(pResource, pParams) vaspaceapiUnmapFrom_DISPATCH(pResource, pParams)
#define vaspaceapiControl_Epilogue(pResource, pCallContext, pParams) vaspaceapiControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define vaspaceapiControlLookup(pResource, pParams, ppEntry) vaspaceapiControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define vaspaceapiGetInternalObjectHandle(pGpuResource) vaspaceapiGetInternalObjectHandle_DISPATCH(pGpuResource)
#define vaspaceapiControl(pGpuResource, pCallContext, pParams) vaspaceapiControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define vaspaceapiUnmap(pGpuResource, pCallContext, pCpuMapping) vaspaceapiUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define vaspaceapiGetMemInterMapParams(pRmResource, pParams) vaspaceapiGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define vaspaceapiGetMemoryMappingDescriptor(pRmResource, ppMemDesc) vaspaceapiGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define vaspaceapiControlFilter(pResource, pCallContext, pParams) vaspaceapiControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define vaspaceapiControlSerialization_Prologue(pResource, pCallContext, pParams) vaspaceapiControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define vaspaceapiPreDestruct(pResource) vaspaceapiPreDestruct_DISPATCH(pResource)
#define vaspaceapiIsDuplicate(pResource, hMemory, pDuplicate) vaspaceapiIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define vaspaceapiControlSerialization_Epilogue(pResource, pCallContext, pParams) vaspaceapiControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define vaspaceapiMap(pGpuResource, pCallContext, pParams, pCpuMapping) vaspaceapiMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define vaspaceapiAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) vaspaceapiAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
NvBool vaspaceapiCanCopy_IMPL(struct VaSpaceApi *pResource);

static inline NvBool vaspaceapiCanCopy_DISPATCH(struct VaSpaceApi *pResource) {
    return pResource->__vaspaceapiCanCopy__(pResource);
}

NV_STATUS vaspaceapiCtrlCmdVaspaceGetGmmuFormat_IMPL(struct VaSpaceApi *pVaspaceApi, NV90F1_CTRL_VASPACE_GET_GMMU_FORMAT_PARAMS *pGmmuFormatParams);

static inline NV_STATUS vaspaceapiCtrlCmdVaspaceGetGmmuFormat_DISPATCH(struct VaSpaceApi *pVaspaceApi, NV90F1_CTRL_VASPACE_GET_GMMU_FORMAT_PARAMS *pGmmuFormatParams) {
    return pVaspaceApi->__vaspaceapiCtrlCmdVaspaceGetGmmuFormat__(pVaspaceApi, pGmmuFormatParams);
}

NV_STATUS vaspaceapiCtrlCmdVaspaceGetPageLevelInfo_IMPL(struct VaSpaceApi *pVaspaceApi, NV90F1_CTRL_VASPACE_GET_PAGE_LEVEL_INFO_PARAMS *pPageLevelInfoParams);

static inline NV_STATUS vaspaceapiCtrlCmdVaspaceGetPageLevelInfo_DISPATCH(struct VaSpaceApi *pVaspaceApi, NV90F1_CTRL_VASPACE_GET_PAGE_LEVEL_INFO_PARAMS *pPageLevelInfoParams) {
    return pVaspaceApi->__vaspaceapiCtrlCmdVaspaceGetPageLevelInfo__(pVaspaceApi, pPageLevelInfoParams);
}

NV_STATUS vaspaceapiCtrlCmdVaspaceReserveEntries_IMPL(struct VaSpaceApi *pVaspaceApi, NV90F1_CTRL_VASPACE_RESERVE_ENTRIES_PARAMS *pReserveEntriesParams);

static inline NV_STATUS vaspaceapiCtrlCmdVaspaceReserveEntries_DISPATCH(struct VaSpaceApi *pVaspaceApi, NV90F1_CTRL_VASPACE_RESERVE_ENTRIES_PARAMS *pReserveEntriesParams) {
    return pVaspaceApi->__vaspaceapiCtrlCmdVaspaceReserveEntries__(pVaspaceApi, pReserveEntriesParams);
}

NV_STATUS vaspaceapiCtrlCmdVaspaceReleaseEntries_IMPL(struct VaSpaceApi *pVaspaceApi, NV90F1_CTRL_VASPACE_RELEASE_ENTRIES_PARAMS *pReleaseEntriesParams);

static inline NV_STATUS vaspaceapiCtrlCmdVaspaceReleaseEntries_DISPATCH(struct VaSpaceApi *pVaspaceApi, NV90F1_CTRL_VASPACE_RELEASE_ENTRIES_PARAMS *pReleaseEntriesParams) {
    return pVaspaceApi->__vaspaceapiCtrlCmdVaspaceReleaseEntries__(pVaspaceApi, pReleaseEntriesParams);
}

NV_STATUS vaspaceapiCtrlCmdVaspaceCopyServerReservedPdes_IMPL(struct VaSpaceApi *pVaspaceApi, NV90F1_CTRL_VASPACE_COPY_SERVER_RESERVED_PDES_PARAMS *pCopyServerReservedPdesParams);

static inline NV_STATUS vaspaceapiCtrlCmdVaspaceCopyServerReservedPdes_DISPATCH(struct VaSpaceApi *pVaspaceApi, NV90F1_CTRL_VASPACE_COPY_SERVER_RESERVED_PDES_PARAMS *pCopyServerReservedPdesParams) {
    return pVaspaceApi->__vaspaceapiCtrlCmdVaspaceCopyServerReservedPdes__(pVaspaceApi, pCopyServerReservedPdesParams);
}

NV_STATUS vaspaceapiCtrlCmdVaspaceGetHostRmManagedSize_IMPL(struct VaSpaceApi *pVaspaceApi, NV90F1_CTRL_VASPACE_GET_HOST_RM_MANAGED_SIZE_PARAMS *pVaspaceGetHostRmManagedSizeParams);

static inline NV_STATUS vaspaceapiCtrlCmdVaspaceGetHostRmManagedSize_DISPATCH(struct VaSpaceApi *pVaspaceApi, NV90F1_CTRL_VASPACE_GET_HOST_RM_MANAGED_SIZE_PARAMS *pVaspaceGetHostRmManagedSizeParams) {
    return pVaspaceApi->__vaspaceapiCtrlCmdVaspaceGetHostRmManagedSize__(pVaspaceApi, pVaspaceGetHostRmManagedSizeParams);
}

static inline NvBool vaspaceapiShareCallback_DISPATCH(struct VaSpaceApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__vaspaceapiShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS vaspaceapiCheckMemInterUnmap_DISPATCH(struct VaSpaceApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__vaspaceapiCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS vaspaceapiMapTo_DISPATCH(struct VaSpaceApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__vaspaceapiMapTo__(pResource, pParams);
}

static inline NV_STATUS vaspaceapiGetMapAddrSpace_DISPATCH(struct VaSpaceApi *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__vaspaceapiGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NvU32 vaspaceapiGetRefCount_DISPATCH(struct VaSpaceApi *pResource) {
    return pResource->__vaspaceapiGetRefCount__(pResource);
}

static inline void vaspaceapiAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct VaSpaceApi *pResource, RsResourceRef *pReference) {
    pResource->__vaspaceapiAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS vaspaceapiControl_Prologue_DISPATCH(struct VaSpaceApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__vaspaceapiControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS vaspaceapiGetRegBaseOffsetAndSize_DISPATCH(struct VaSpaceApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__vaspaceapiGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS vaspaceapiInternalControlForward_DISPATCH(struct VaSpaceApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__vaspaceapiInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NV_STATUS vaspaceapiUnmapFrom_DISPATCH(struct VaSpaceApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__vaspaceapiUnmapFrom__(pResource, pParams);
}

static inline void vaspaceapiControl_Epilogue_DISPATCH(struct VaSpaceApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__vaspaceapiControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS vaspaceapiControlLookup_DISPATCH(struct VaSpaceApi *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__vaspaceapiControlLookup__(pResource, pParams, ppEntry);
}

static inline NvHandle vaspaceapiGetInternalObjectHandle_DISPATCH(struct VaSpaceApi *pGpuResource) {
    return pGpuResource->__vaspaceapiGetInternalObjectHandle__(pGpuResource);
}

static inline NV_STATUS vaspaceapiControl_DISPATCH(struct VaSpaceApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__vaspaceapiControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS vaspaceapiUnmap_DISPATCH(struct VaSpaceApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__vaspaceapiUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NV_STATUS vaspaceapiGetMemInterMapParams_DISPATCH(struct VaSpaceApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__vaspaceapiGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS vaspaceapiGetMemoryMappingDescriptor_DISPATCH(struct VaSpaceApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__vaspaceapiGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS vaspaceapiControlFilter_DISPATCH(struct VaSpaceApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__vaspaceapiControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS vaspaceapiControlSerialization_Prologue_DISPATCH(struct VaSpaceApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__vaspaceapiControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void vaspaceapiPreDestruct_DISPATCH(struct VaSpaceApi *pResource) {
    pResource->__vaspaceapiPreDestruct__(pResource);
}

static inline NV_STATUS vaspaceapiIsDuplicate_DISPATCH(struct VaSpaceApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__vaspaceapiIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void vaspaceapiControlSerialization_Epilogue_DISPATCH(struct VaSpaceApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__vaspaceapiControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS vaspaceapiMap_DISPATCH(struct VaSpaceApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__vaspaceapiMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NvBool vaspaceapiAccessCallback_DISPATCH(struct VaSpaceApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__vaspaceapiAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

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
