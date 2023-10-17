#ifndef _G_GENERIC_ENGINE_NVOC_H_
#define _G_GENERIC_ENGINE_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_generic_engine_nvoc.h"

#ifndef _GENERICENGINEAPI_H_
#define _GENERICENGINEAPI_H_

#include "gpu/gpu_resource.h"
#include "ctrl/ctrl90e6.h"
#include "ctrl/ctrl90e7.h"
#include "rmapi/resource.h" // for macro RMCTRL_EXPORT etc.

/*!
 * RM internal class providing a generic engine API to RM clients (e.g.:
 * GF100_SUBDEVICE_GRAPHICS and GF100_SUBDEVICE_FB). Classes are primarily used
 * for exposing BAR0 mappings and controls.
 */
#ifdef NVOC_GENERIC_ENGINE_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct GenericEngineApi {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct GpuResource __nvoc_base_GpuResource;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct GpuResource *__nvoc_pbase_GpuResource;
    struct GenericEngineApi *__nvoc_pbase_GenericEngineApi;
    NV_STATUS (*__genapiMap__)(struct GenericEngineApi *, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);
    NV_STATUS (*__genapiGetMapAddrSpace__)(struct GenericEngineApi *, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    NV_STATUS (*__genapiControl__)(struct GenericEngineApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__genapiCtrlCmdMasterGetErrorIntrOffsetMask__)(struct GenericEngineApi *, NV90E6_CTRL_MASTER_GET_ERROR_INTR_OFFSET_MASK_PARAMS *);
    NV_STATUS (*__genapiCtrlCmdMasterGetVirtualFunctionErrorContIntrMask__)(struct GenericEngineApi *, NV90E6_CTRL_MASTER_GET_VIRTUAL_FUNCTION_ERROR_CONT_INTR_MASK_PARAMS *);
    NV_STATUS (*__genapiCtrlCmdBBXGetLastFlushTime__)(struct GenericEngineApi *, NV90E7_CTRL_BBX_GET_LAST_FLUSH_TIME_PARAMS *);
    NvBool (*__genapiShareCallback__)(struct GenericEngineApi *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__genapiCheckMemInterUnmap__)(struct GenericEngineApi *, NvBool);
    NV_STATUS (*__genapiMapTo__)(struct GenericEngineApi *, RS_RES_MAP_TO_PARAMS *);
    NvU32 (*__genapiGetRefCount__)(struct GenericEngineApi *);
    void (*__genapiAddAdditionalDependants__)(struct RsClient *, struct GenericEngineApi *, RsResourceRef *);
    NV_STATUS (*__genapiControl_Prologue__)(struct GenericEngineApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__genapiGetRegBaseOffsetAndSize__)(struct GenericEngineApi *, struct OBJGPU *, NvU32 *, NvU32 *);
    NV_STATUS (*__genapiInternalControlForward__)(struct GenericEngineApi *, NvU32, void *, NvU32);
    NV_STATUS (*__genapiUnmapFrom__)(struct GenericEngineApi *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__genapiControl_Epilogue__)(struct GenericEngineApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__genapiControlLookup__)(struct GenericEngineApi *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NvHandle (*__genapiGetInternalObjectHandle__)(struct GenericEngineApi *);
    NV_STATUS (*__genapiUnmap__)(struct GenericEngineApi *, struct CALL_CONTEXT *, struct RsCpuMapping *);
    NV_STATUS (*__genapiGetMemInterMapParams__)(struct GenericEngineApi *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__genapiGetMemoryMappingDescriptor__)(struct GenericEngineApi *, struct MEMORY_DESCRIPTOR **);
    NV_STATUS (*__genapiControlFilter__)(struct GenericEngineApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__genapiControlSerialization_Prologue__)(struct GenericEngineApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__genapiCanCopy__)(struct GenericEngineApi *);
    void (*__genapiPreDestruct__)(struct GenericEngineApi *);
    NV_STATUS (*__genapiIsDuplicate__)(struct GenericEngineApi *, NvHandle, NvBool *);
    void (*__genapiControlSerialization_Epilogue__)(struct GenericEngineApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__genapiAccessCallback__)(struct GenericEngineApi *, struct RsClient *, void *, RsAccessRight);
};

#ifndef __NVOC_CLASS_GenericEngineApi_TYPEDEF__
#define __NVOC_CLASS_GenericEngineApi_TYPEDEF__
typedef struct GenericEngineApi GenericEngineApi;
#endif /* __NVOC_CLASS_GenericEngineApi_TYPEDEF__ */

#ifndef __nvoc_class_id_GenericEngineApi
#define __nvoc_class_id_GenericEngineApi 0x4bc329
#endif /* __nvoc_class_id_GenericEngineApi */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GenericEngineApi;

#define __staticCast_GenericEngineApi(pThis) \
    ((pThis)->__nvoc_pbase_GenericEngineApi)

#ifdef __nvoc_generic_engine_h_disabled
#define __dynamicCast_GenericEngineApi(pThis) ((GenericEngineApi*)NULL)
#else //__nvoc_generic_engine_h_disabled
#define __dynamicCast_GenericEngineApi(pThis) \
    ((GenericEngineApi*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(GenericEngineApi)))
#endif //__nvoc_generic_engine_h_disabled


NV_STATUS __nvoc_objCreateDynamic_GenericEngineApi(GenericEngineApi**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_GenericEngineApi(GenericEngineApi**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_GenericEngineApi(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_GenericEngineApi((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define genapiMap(pGenericEngineApi, pCallContext, pParams, pCpuMapping) genapiMap_DISPATCH(pGenericEngineApi, pCallContext, pParams, pCpuMapping)
#define genapiGetMapAddrSpace(pGenericEngineApi, pCallContext, mapFlags, pAddrSpace) genapiGetMapAddrSpace_DISPATCH(pGenericEngineApi, pCallContext, mapFlags, pAddrSpace)
#define genapiControl(pGenericEngineApi, pCallContext, pParams) genapiControl_DISPATCH(pGenericEngineApi, pCallContext, pParams)
#define genapiCtrlCmdMasterGetErrorIntrOffsetMask(pGenericEngineApi, pParams) genapiCtrlCmdMasterGetErrorIntrOffsetMask_DISPATCH(pGenericEngineApi, pParams)
#define genapiCtrlCmdMasterGetVirtualFunctionErrorContIntrMask(pGenericEngineApi, pParams) genapiCtrlCmdMasterGetVirtualFunctionErrorContIntrMask_DISPATCH(pGenericEngineApi, pParams)
#define genapiCtrlCmdBBXGetLastFlushTime(pGenericEngineApi, pParams) genapiCtrlCmdBBXGetLastFlushTime_DISPATCH(pGenericEngineApi, pParams)
#define genapiShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) genapiShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define genapiCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) genapiCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define genapiMapTo(pResource, pParams) genapiMapTo_DISPATCH(pResource, pParams)
#define genapiGetRefCount(pResource) genapiGetRefCount_DISPATCH(pResource)
#define genapiAddAdditionalDependants(pClient, pResource, pReference) genapiAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define genapiControl_Prologue(pResource, pCallContext, pParams) genapiControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define genapiGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) genapiGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define genapiInternalControlForward(pGpuResource, command, pParams, size) genapiInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define genapiUnmapFrom(pResource, pParams) genapiUnmapFrom_DISPATCH(pResource, pParams)
#define genapiControl_Epilogue(pResource, pCallContext, pParams) genapiControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define genapiControlLookup(pResource, pParams, ppEntry) genapiControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define genapiGetInternalObjectHandle(pGpuResource) genapiGetInternalObjectHandle_DISPATCH(pGpuResource)
#define genapiUnmap(pGpuResource, pCallContext, pCpuMapping) genapiUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define genapiGetMemInterMapParams(pRmResource, pParams) genapiGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define genapiGetMemoryMappingDescriptor(pRmResource, ppMemDesc) genapiGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define genapiControlFilter(pResource, pCallContext, pParams) genapiControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define genapiControlSerialization_Prologue(pResource, pCallContext, pParams) genapiControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define genapiCanCopy(pResource) genapiCanCopy_DISPATCH(pResource)
#define genapiPreDestruct(pResource) genapiPreDestruct_DISPATCH(pResource)
#define genapiIsDuplicate(pResource, hMemory, pDuplicate) genapiIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define genapiControlSerialization_Epilogue(pResource, pCallContext, pParams) genapiControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define genapiAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) genapiAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
NV_STATUS genapiMap_IMPL(struct GenericEngineApi *pGenericEngineApi, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);

static inline NV_STATUS genapiMap_DISPATCH(struct GenericEngineApi *pGenericEngineApi, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGenericEngineApi->__genapiMap__(pGenericEngineApi, pCallContext, pParams, pCpuMapping);
}

NV_STATUS genapiGetMapAddrSpace_IMPL(struct GenericEngineApi *pGenericEngineApi, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);

static inline NV_STATUS genapiGetMapAddrSpace_DISPATCH(struct GenericEngineApi *pGenericEngineApi, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGenericEngineApi->__genapiGetMapAddrSpace__(pGenericEngineApi, pCallContext, mapFlags, pAddrSpace);
}

NV_STATUS genapiControl_IMPL(struct GenericEngineApi *pGenericEngineApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);

static inline NV_STATUS genapiControl_DISPATCH(struct GenericEngineApi *pGenericEngineApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGenericEngineApi->__genapiControl__(pGenericEngineApi, pCallContext, pParams);
}

NV_STATUS genapiCtrlCmdMasterGetErrorIntrOffsetMask_IMPL(struct GenericEngineApi *pGenericEngineApi, NV90E6_CTRL_MASTER_GET_ERROR_INTR_OFFSET_MASK_PARAMS *pParams);

static inline NV_STATUS genapiCtrlCmdMasterGetErrorIntrOffsetMask_DISPATCH(struct GenericEngineApi *pGenericEngineApi, NV90E6_CTRL_MASTER_GET_ERROR_INTR_OFFSET_MASK_PARAMS *pParams) {
    return pGenericEngineApi->__genapiCtrlCmdMasterGetErrorIntrOffsetMask__(pGenericEngineApi, pParams);
}

NV_STATUS genapiCtrlCmdMasterGetVirtualFunctionErrorContIntrMask_IMPL(struct GenericEngineApi *pGenericEngineApi, NV90E6_CTRL_MASTER_GET_VIRTUAL_FUNCTION_ERROR_CONT_INTR_MASK_PARAMS *pParams);

static inline NV_STATUS genapiCtrlCmdMasterGetVirtualFunctionErrorContIntrMask_DISPATCH(struct GenericEngineApi *pGenericEngineApi, NV90E6_CTRL_MASTER_GET_VIRTUAL_FUNCTION_ERROR_CONT_INTR_MASK_PARAMS *pParams) {
    return pGenericEngineApi->__genapiCtrlCmdMasterGetVirtualFunctionErrorContIntrMask__(pGenericEngineApi, pParams);
}

NV_STATUS genapiCtrlCmdBBXGetLastFlushTime_IMPL(struct GenericEngineApi *pGenericEngineApi, NV90E7_CTRL_BBX_GET_LAST_FLUSH_TIME_PARAMS *pParams);

static inline NV_STATUS genapiCtrlCmdBBXGetLastFlushTime_DISPATCH(struct GenericEngineApi *pGenericEngineApi, NV90E7_CTRL_BBX_GET_LAST_FLUSH_TIME_PARAMS *pParams) {
    return pGenericEngineApi->__genapiCtrlCmdBBXGetLastFlushTime__(pGenericEngineApi, pParams);
}

static inline NvBool genapiShareCallback_DISPATCH(struct GenericEngineApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__genapiShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS genapiCheckMemInterUnmap_DISPATCH(struct GenericEngineApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__genapiCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS genapiMapTo_DISPATCH(struct GenericEngineApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__genapiMapTo__(pResource, pParams);
}

static inline NvU32 genapiGetRefCount_DISPATCH(struct GenericEngineApi *pResource) {
    return pResource->__genapiGetRefCount__(pResource);
}

static inline void genapiAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct GenericEngineApi *pResource, RsResourceRef *pReference) {
    pResource->__genapiAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS genapiControl_Prologue_DISPATCH(struct GenericEngineApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__genapiControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS genapiGetRegBaseOffsetAndSize_DISPATCH(struct GenericEngineApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__genapiGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS genapiInternalControlForward_DISPATCH(struct GenericEngineApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__genapiInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NV_STATUS genapiUnmapFrom_DISPATCH(struct GenericEngineApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__genapiUnmapFrom__(pResource, pParams);
}

static inline void genapiControl_Epilogue_DISPATCH(struct GenericEngineApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__genapiControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS genapiControlLookup_DISPATCH(struct GenericEngineApi *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__genapiControlLookup__(pResource, pParams, ppEntry);
}

static inline NvHandle genapiGetInternalObjectHandle_DISPATCH(struct GenericEngineApi *pGpuResource) {
    return pGpuResource->__genapiGetInternalObjectHandle__(pGpuResource);
}

static inline NV_STATUS genapiUnmap_DISPATCH(struct GenericEngineApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__genapiUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NV_STATUS genapiGetMemInterMapParams_DISPATCH(struct GenericEngineApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__genapiGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS genapiGetMemoryMappingDescriptor_DISPATCH(struct GenericEngineApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__genapiGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS genapiControlFilter_DISPATCH(struct GenericEngineApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__genapiControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS genapiControlSerialization_Prologue_DISPATCH(struct GenericEngineApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__genapiControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool genapiCanCopy_DISPATCH(struct GenericEngineApi *pResource) {
    return pResource->__genapiCanCopy__(pResource);
}

static inline void genapiPreDestruct_DISPATCH(struct GenericEngineApi *pResource) {
    pResource->__genapiPreDestruct__(pResource);
}

static inline NV_STATUS genapiIsDuplicate_DISPATCH(struct GenericEngineApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__genapiIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void genapiControlSerialization_Epilogue_DISPATCH(struct GenericEngineApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__genapiControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool genapiAccessCallback_DISPATCH(struct GenericEngineApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__genapiAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

NV_STATUS genapiConstruct_IMPL(struct GenericEngineApi *arg_pGenericEngineApi, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_genapiConstruct(arg_pGenericEngineApi, arg_pCallContext, arg_pParams) genapiConstruct_IMPL(arg_pGenericEngineApi, arg_pCallContext, arg_pParams)
void genapiDestruct_IMPL(struct GenericEngineApi *pGenericEngineApi);

#define __nvoc_genapiDestruct(pGenericEngineApi) genapiDestruct_IMPL(pGenericEngineApi)
#undef PRIVATE_FIELD


#endif // _GENERICENGINEAPI_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_GENERIC_ENGINE_NVOC_H_
