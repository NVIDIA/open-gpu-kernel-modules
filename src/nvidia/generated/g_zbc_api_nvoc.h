#ifndef _G_ZBC_API_NVOC_H_
#define _G_ZBC_API_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_zbc_api_nvoc.h"

#ifndef _ZBCAPI_H_
#define _ZBCAPI_H_

#include "gpu/gpu_resource.h"
#include "gpu/gpu_halspec.h"
#include "rmapi/control.h"
#include "ctrl/ctrl9096.h"

/*!
 * RM internal class representing GF100_ZBC_CLEAR
 */
#ifdef NVOC_ZBC_API_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct ZbcApi {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct GpuResource __nvoc_base_GpuResource;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct GpuResource *__nvoc_pbase_GpuResource;
    struct ZbcApi *__nvoc_pbase_ZbcApi;
    NV_STATUS (*__zbcapiCtrlCmdSetZbcColorClear__)(struct ZbcApi *, NV9096_CTRL_SET_ZBC_COLOR_CLEAR_PARAMS *);
    NV_STATUS (*__zbcapiCtrlCmdSetZbcDepthClear__)(struct ZbcApi *, NV9096_CTRL_SET_ZBC_DEPTH_CLEAR_PARAMS *);
    NV_STATUS (*__zbcapiCtrlCmdGetZbcClearTable__)(struct ZbcApi *, NV9096_CTRL_GET_ZBC_CLEAR_TABLE_PARAMS *);
    NV_STATUS (*__zbcapiCtrlCmdSetZbcClearTable__)(struct ZbcApi *, NV9096_CTRL_SET_ZBC_CLEAR_TABLE_PARAMS *);
    NV_STATUS (*__zbcapiCtrlCmdSetZbcStencilClear__)(struct ZbcApi *, NV9096_CTRL_SET_ZBC_STENCIL_CLEAR_PARAMS *);
    NV_STATUS (*__zbcapiCtrlCmdGetZbcClearTableSize__)(struct ZbcApi *, NV9096_CTRL_GET_ZBC_CLEAR_TABLE_SIZE_PARAMS *);
    NV_STATUS (*__zbcapiCtrlCmdGetZbcClearTableEntry__)(struct ZbcApi *, NV9096_CTRL_GET_ZBC_CLEAR_TABLE_ENTRY_PARAMS *);
    NvBool (*__zbcapiShareCallback__)(struct ZbcApi *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__zbcapiCheckMemInterUnmap__)(struct ZbcApi *, NvBool);
    NV_STATUS (*__zbcapiMapTo__)(struct ZbcApi *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__zbcapiGetMapAddrSpace__)(struct ZbcApi *, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    NvU32 (*__zbcapiGetRefCount__)(struct ZbcApi *);
    void (*__zbcapiAddAdditionalDependants__)(struct RsClient *, struct ZbcApi *, RsResourceRef *);
    NV_STATUS (*__zbcapiControl_Prologue__)(struct ZbcApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__zbcapiGetRegBaseOffsetAndSize__)(struct ZbcApi *, struct OBJGPU *, NvU32 *, NvU32 *);
    NV_STATUS (*__zbcapiInternalControlForward__)(struct ZbcApi *, NvU32, void *, NvU32);
    NV_STATUS (*__zbcapiUnmapFrom__)(struct ZbcApi *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__zbcapiControl_Epilogue__)(struct ZbcApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__zbcapiControlLookup__)(struct ZbcApi *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NvHandle (*__zbcapiGetInternalObjectHandle__)(struct ZbcApi *);
    NV_STATUS (*__zbcapiControl__)(struct ZbcApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__zbcapiUnmap__)(struct ZbcApi *, struct CALL_CONTEXT *, struct RsCpuMapping *);
    NV_STATUS (*__zbcapiGetMemInterMapParams__)(struct ZbcApi *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__zbcapiGetMemoryMappingDescriptor__)(struct ZbcApi *, struct MEMORY_DESCRIPTOR **);
    NV_STATUS (*__zbcapiControlFilter__)(struct ZbcApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__zbcapiControlSerialization_Prologue__)(struct ZbcApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__zbcapiCanCopy__)(struct ZbcApi *);
    void (*__zbcapiPreDestruct__)(struct ZbcApi *);
    NV_STATUS (*__zbcapiIsDuplicate__)(struct ZbcApi *, NvHandle, NvBool *);
    void (*__zbcapiControlSerialization_Epilogue__)(struct ZbcApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__zbcapiMap__)(struct ZbcApi *, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);
    NvBool (*__zbcapiAccessCallback__)(struct ZbcApi *, struct RsClient *, void *, RsAccessRight);
    NvBool bZbcUsed;
};

#ifndef __NVOC_CLASS_ZbcApi_TYPEDEF__
#define __NVOC_CLASS_ZbcApi_TYPEDEF__
typedef struct ZbcApi ZbcApi;
#endif /* __NVOC_CLASS_ZbcApi_TYPEDEF__ */

#ifndef __nvoc_class_id_ZbcApi
#define __nvoc_class_id_ZbcApi 0x397ee3
#endif /* __nvoc_class_id_ZbcApi */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_ZbcApi;

#define __staticCast_ZbcApi(pThis) \
    ((pThis)->__nvoc_pbase_ZbcApi)

#ifdef __nvoc_zbc_api_h_disabled
#define __dynamicCast_ZbcApi(pThis) ((ZbcApi*)NULL)
#else //__nvoc_zbc_api_h_disabled
#define __dynamicCast_ZbcApi(pThis) \
    ((ZbcApi*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(ZbcApi)))
#endif //__nvoc_zbc_api_h_disabled


NV_STATUS __nvoc_objCreateDynamic_ZbcApi(ZbcApi**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_ZbcApi(ZbcApi**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_ZbcApi(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_ZbcApi((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define zbcapiCtrlCmdSetZbcColorClear(pZbcApi, pSetZBCClearParams) zbcapiCtrlCmdSetZbcColorClear_DISPATCH(pZbcApi, pSetZBCClearParams)
#define zbcapiCtrlCmdSetZbcDepthClear(pZbcApi, pSetZBCClearParams) zbcapiCtrlCmdSetZbcDepthClear_DISPATCH(pZbcApi, pSetZBCClearParams)
#define zbcapiCtrlCmdGetZbcClearTable(pZbcApi, pGetZBCClearTableParams) zbcapiCtrlCmdGetZbcClearTable_DISPATCH(pZbcApi, pGetZBCClearTableParams)
#define zbcapiCtrlCmdSetZbcClearTable(pZbcApi, pSetZBCClearTableParams) zbcapiCtrlCmdSetZbcClearTable_DISPATCH(pZbcApi, pSetZBCClearTableParams)
#define zbcapiCtrlCmdSetZbcStencilClear(pZbcApi, pSetZBCClearParams) zbcapiCtrlCmdSetZbcStencilClear_DISPATCH(pZbcApi, pSetZBCClearParams)
#define zbcapiCtrlCmdGetZbcClearTableSize(pZbcApi, pGetZBCClearTableSizeParams) zbcapiCtrlCmdGetZbcClearTableSize_DISPATCH(pZbcApi, pGetZBCClearTableSizeParams)
#define zbcapiCtrlCmdGetZbcClearTableEntry(pZbcApi, pGetZBCClearTableEntryParams) zbcapiCtrlCmdGetZbcClearTableEntry_DISPATCH(pZbcApi, pGetZBCClearTableEntryParams)
#define zbcapiShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) zbcapiShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define zbcapiCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) zbcapiCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define zbcapiMapTo(pResource, pParams) zbcapiMapTo_DISPATCH(pResource, pParams)
#define zbcapiGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) zbcapiGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define zbcapiGetRefCount(pResource) zbcapiGetRefCount_DISPATCH(pResource)
#define zbcapiAddAdditionalDependants(pClient, pResource, pReference) zbcapiAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define zbcapiControl_Prologue(pResource, pCallContext, pParams) zbcapiControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define zbcapiGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) zbcapiGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define zbcapiInternalControlForward(pGpuResource, command, pParams, size) zbcapiInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define zbcapiUnmapFrom(pResource, pParams) zbcapiUnmapFrom_DISPATCH(pResource, pParams)
#define zbcapiControl_Epilogue(pResource, pCallContext, pParams) zbcapiControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define zbcapiControlLookup(pResource, pParams, ppEntry) zbcapiControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define zbcapiGetInternalObjectHandle(pGpuResource) zbcapiGetInternalObjectHandle_DISPATCH(pGpuResource)
#define zbcapiControl(pGpuResource, pCallContext, pParams) zbcapiControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define zbcapiUnmap(pGpuResource, pCallContext, pCpuMapping) zbcapiUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define zbcapiGetMemInterMapParams(pRmResource, pParams) zbcapiGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define zbcapiGetMemoryMappingDescriptor(pRmResource, ppMemDesc) zbcapiGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define zbcapiControlFilter(pResource, pCallContext, pParams) zbcapiControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define zbcapiControlSerialization_Prologue(pResource, pCallContext, pParams) zbcapiControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define zbcapiCanCopy(pResource) zbcapiCanCopy_DISPATCH(pResource)
#define zbcapiPreDestruct(pResource) zbcapiPreDestruct_DISPATCH(pResource)
#define zbcapiIsDuplicate(pResource, hMemory, pDuplicate) zbcapiIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define zbcapiControlSerialization_Epilogue(pResource, pCallContext, pParams) zbcapiControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define zbcapiMap(pGpuResource, pCallContext, pParams, pCpuMapping) zbcapiMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define zbcapiAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) zbcapiAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
static inline NV_STATUS zbcapiConstructHal_56cd7a(struct ZbcApi *pZbcApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {
    return NV_OK;
}


#ifdef __nvoc_zbc_api_h_disabled
static inline NV_STATUS zbcapiConstructHal(struct ZbcApi *pZbcApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {
    NV_ASSERT_FAILED_PRECOMP("ZbcApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_zbc_api_h_disabled
#define zbcapiConstructHal(pZbcApi, pCallContext, pParams) zbcapiConstructHal_56cd7a(pZbcApi, pCallContext, pParams)
#endif //__nvoc_zbc_api_h_disabled

#define zbcapiConstructHal_HAL(pZbcApi, pCallContext, pParams) zbcapiConstructHal(pZbcApi, pCallContext, pParams)

static inline void zbcapiDestruct_b3696a(struct ZbcApi *pZbcApi) {
    return;
}


#define __nvoc_zbcapiDestruct(pZbcApi) zbcapiDestruct_b3696a(pZbcApi)
NV_STATUS zbcapiCtrlCmdSetZbcColorClear_IMPL(struct ZbcApi *pZbcApi, NV9096_CTRL_SET_ZBC_COLOR_CLEAR_PARAMS *pSetZBCClearParams);

static inline NV_STATUS zbcapiCtrlCmdSetZbcColorClear_DISPATCH(struct ZbcApi *pZbcApi, NV9096_CTRL_SET_ZBC_COLOR_CLEAR_PARAMS *pSetZBCClearParams) {
    return pZbcApi->__zbcapiCtrlCmdSetZbcColorClear__(pZbcApi, pSetZBCClearParams);
}

NV_STATUS zbcapiCtrlCmdSetZbcDepthClear_IMPL(struct ZbcApi *pZbcApi, NV9096_CTRL_SET_ZBC_DEPTH_CLEAR_PARAMS *pSetZBCClearParams);

static inline NV_STATUS zbcapiCtrlCmdSetZbcDepthClear_DISPATCH(struct ZbcApi *pZbcApi, NV9096_CTRL_SET_ZBC_DEPTH_CLEAR_PARAMS *pSetZBCClearParams) {
    return pZbcApi->__zbcapiCtrlCmdSetZbcDepthClear__(pZbcApi, pSetZBCClearParams);
}

NV_STATUS zbcapiCtrlCmdGetZbcClearTable_IMPL(struct ZbcApi *pZbcApi, NV9096_CTRL_GET_ZBC_CLEAR_TABLE_PARAMS *pGetZBCClearTableParams);

static inline NV_STATUS zbcapiCtrlCmdGetZbcClearTable_DISPATCH(struct ZbcApi *pZbcApi, NV9096_CTRL_GET_ZBC_CLEAR_TABLE_PARAMS *pGetZBCClearTableParams) {
    return pZbcApi->__zbcapiCtrlCmdGetZbcClearTable__(pZbcApi, pGetZBCClearTableParams);
}

NV_STATUS zbcapiCtrlCmdSetZbcClearTable_IMPL(struct ZbcApi *pZbcApi, NV9096_CTRL_SET_ZBC_CLEAR_TABLE_PARAMS *pSetZBCClearTableParams);

static inline NV_STATUS zbcapiCtrlCmdSetZbcClearTable_DISPATCH(struct ZbcApi *pZbcApi, NV9096_CTRL_SET_ZBC_CLEAR_TABLE_PARAMS *pSetZBCClearTableParams) {
    return pZbcApi->__zbcapiCtrlCmdSetZbcClearTable__(pZbcApi, pSetZBCClearTableParams);
}

NV_STATUS zbcapiCtrlCmdSetZbcStencilClear_IMPL(struct ZbcApi *pZbcApi, NV9096_CTRL_SET_ZBC_STENCIL_CLEAR_PARAMS *pSetZBCClearParams);

static inline NV_STATUS zbcapiCtrlCmdSetZbcStencilClear_DISPATCH(struct ZbcApi *pZbcApi, NV9096_CTRL_SET_ZBC_STENCIL_CLEAR_PARAMS *pSetZBCClearParams) {
    return pZbcApi->__zbcapiCtrlCmdSetZbcStencilClear__(pZbcApi, pSetZBCClearParams);
}

NV_STATUS zbcapiCtrlCmdGetZbcClearTableSize_IMPL(struct ZbcApi *pZbcApi, NV9096_CTRL_GET_ZBC_CLEAR_TABLE_SIZE_PARAMS *pGetZBCClearTableSizeParams);

static inline NV_STATUS zbcapiCtrlCmdGetZbcClearTableSize_DISPATCH(struct ZbcApi *pZbcApi, NV9096_CTRL_GET_ZBC_CLEAR_TABLE_SIZE_PARAMS *pGetZBCClearTableSizeParams) {
    return pZbcApi->__zbcapiCtrlCmdGetZbcClearTableSize__(pZbcApi, pGetZBCClearTableSizeParams);
}

NV_STATUS zbcapiCtrlCmdGetZbcClearTableEntry_IMPL(struct ZbcApi *pZbcApi, NV9096_CTRL_GET_ZBC_CLEAR_TABLE_ENTRY_PARAMS *pGetZBCClearTableEntryParams);

static inline NV_STATUS zbcapiCtrlCmdGetZbcClearTableEntry_DISPATCH(struct ZbcApi *pZbcApi, NV9096_CTRL_GET_ZBC_CLEAR_TABLE_ENTRY_PARAMS *pGetZBCClearTableEntryParams) {
    return pZbcApi->__zbcapiCtrlCmdGetZbcClearTableEntry__(pZbcApi, pGetZBCClearTableEntryParams);
}

static inline NvBool zbcapiShareCallback_DISPATCH(struct ZbcApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__zbcapiShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS zbcapiCheckMemInterUnmap_DISPATCH(struct ZbcApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__zbcapiCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS zbcapiMapTo_DISPATCH(struct ZbcApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__zbcapiMapTo__(pResource, pParams);
}

static inline NV_STATUS zbcapiGetMapAddrSpace_DISPATCH(struct ZbcApi *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__zbcapiGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NvU32 zbcapiGetRefCount_DISPATCH(struct ZbcApi *pResource) {
    return pResource->__zbcapiGetRefCount__(pResource);
}

static inline void zbcapiAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct ZbcApi *pResource, RsResourceRef *pReference) {
    pResource->__zbcapiAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS zbcapiControl_Prologue_DISPATCH(struct ZbcApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__zbcapiControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS zbcapiGetRegBaseOffsetAndSize_DISPATCH(struct ZbcApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__zbcapiGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS zbcapiInternalControlForward_DISPATCH(struct ZbcApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__zbcapiInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NV_STATUS zbcapiUnmapFrom_DISPATCH(struct ZbcApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__zbcapiUnmapFrom__(pResource, pParams);
}

static inline void zbcapiControl_Epilogue_DISPATCH(struct ZbcApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__zbcapiControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS zbcapiControlLookup_DISPATCH(struct ZbcApi *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__zbcapiControlLookup__(pResource, pParams, ppEntry);
}

static inline NvHandle zbcapiGetInternalObjectHandle_DISPATCH(struct ZbcApi *pGpuResource) {
    return pGpuResource->__zbcapiGetInternalObjectHandle__(pGpuResource);
}

static inline NV_STATUS zbcapiControl_DISPATCH(struct ZbcApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__zbcapiControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS zbcapiUnmap_DISPATCH(struct ZbcApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__zbcapiUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NV_STATUS zbcapiGetMemInterMapParams_DISPATCH(struct ZbcApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__zbcapiGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS zbcapiGetMemoryMappingDescriptor_DISPATCH(struct ZbcApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__zbcapiGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS zbcapiControlFilter_DISPATCH(struct ZbcApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__zbcapiControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS zbcapiControlSerialization_Prologue_DISPATCH(struct ZbcApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__zbcapiControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool zbcapiCanCopy_DISPATCH(struct ZbcApi *pResource) {
    return pResource->__zbcapiCanCopy__(pResource);
}

static inline void zbcapiPreDestruct_DISPATCH(struct ZbcApi *pResource) {
    pResource->__zbcapiPreDestruct__(pResource);
}

static inline NV_STATUS zbcapiIsDuplicate_DISPATCH(struct ZbcApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__zbcapiIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void zbcapiControlSerialization_Epilogue_DISPATCH(struct ZbcApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__zbcapiControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS zbcapiMap_DISPATCH(struct ZbcApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__zbcapiMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NvBool zbcapiAccessCallback_DISPATCH(struct ZbcApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__zbcapiAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS __nvoc_zbcapiConstruct(struct ZbcApi *arg_pZbcApi, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams) {
    return zbcapiConstructHal(arg_pZbcApi, arg_pCallContext, arg_pParams);
}

#undef PRIVATE_FIELD


#endif // _ZBCAPI_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_ZBC_API_NVOC_H_
