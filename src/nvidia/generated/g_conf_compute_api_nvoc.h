#ifndef _G_CONF_COMPUTE_API_NVOC_H_
#define _G_CONF_COMPUTE_API_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_conf_compute_api_nvoc.h"

#ifndef CONF_COMPUTE_API_H
#define CONF_COMPUTE_API_H

#include "gpu/gpu.h"
#include "rmapi/resource.h"
#include "ctrl/ctrlcb33.h"
#include "gpu_mgr/gpu_mgr.h"
#include "gpu/conf_compute/conf_compute.h"


/****************************************************************************\
 *                                                                           *
 *      Confidential Compute module header.                                  *
 *                                                                           *
 ****************************************************************************/

#ifdef NVOC_CONF_COMPUTE_API_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct ConfidentialComputeApi {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct RmResource __nvoc_base_RmResource;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct ConfidentialComputeApi *__nvoc_pbase_ConfidentialComputeApi;
    NV_STATUS (*__confComputeApiCtrlCmdSystemGetCapabilities__)(struct ConfidentialComputeApi *, NV_CONF_COMPUTE_CTRL_CMD_SYSTEM_GET_CAPABILITIES_PARAMS *);
    NV_STATUS (*__confComputeApiCtrlCmdSystemGetGpusState__)(struct ConfidentialComputeApi *, NV_CONF_COMPUTE_CTRL_CMD_SYSTEM_GET_GPUS_STATE_PARAMS *);
    NV_STATUS (*__confComputeApiCtrlCmdSystemSetGpusState__)(struct ConfidentialComputeApi *, NV_CONF_COMPUTE_CTRL_CMD_SYSTEM_SET_GPUS_STATE_PARAMS *);
    NV_STATUS (*__confComputeApiCtrlCmdGpuGetVidmemSize__)(struct ConfidentialComputeApi *, NV_CONF_COMPUTE_CTRL_CMD_GPU_GET_VIDMEM_SIZE_PARAMS *);
    NV_STATUS (*__confComputeApiCtrlCmdGpuSetVidmemSize__)(struct ConfidentialComputeApi *, NV_CONF_COMPUTE_CTRL_CMD_GPU_SET_VIDMEM_SIZE_PARAMS *);
    NV_STATUS (*__confComputeApiCtrlCmdGetGpuCertificate__)(struct ConfidentialComputeApi *, NV_CONF_COMPUTE_CTRL_CMD_GET_GPU_CERTIFICATE_PARAMS *);
    NV_STATUS (*__confComputeApiCtrlCmdGetGpuAttestationReport__)(struct ConfidentialComputeApi *, NV_CONF_COMPUTE_CTRL_CMD_GET_GPU_ATTESTATION_REPORT_PARAMS *);
    NV_STATUS (*__confComputeApiCtrlCmdGpuGetNumSecureChannels__)(struct ConfidentialComputeApi *, NV_CONF_COMPUTE_CTRL_CMD_GPU_GET_NUM_SECURE_CHANNELS_PARAMS *);
    NvBool (*__confComputeApiShareCallback__)(struct ConfidentialComputeApi *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__confComputeApiCheckMemInterUnmap__)(struct ConfidentialComputeApi *, NvBool);
    NV_STATUS (*__confComputeApiControl__)(struct ConfidentialComputeApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__confComputeApiGetMemInterMapParams__)(struct ConfidentialComputeApi *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__confComputeApiGetMemoryMappingDescriptor__)(struct ConfidentialComputeApi *, struct MEMORY_DESCRIPTOR **);
    NvU32 (*__confComputeApiGetRefCount__)(struct ConfidentialComputeApi *);
    NV_STATUS (*__confComputeApiControlFilter__)(struct ConfidentialComputeApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    void (*__confComputeApiAddAdditionalDependants__)(struct RsClient *, struct ConfidentialComputeApi *, RsResourceRef *);
    NV_STATUS (*__confComputeApiUnmapFrom__)(struct ConfidentialComputeApi *, RS_RES_UNMAP_FROM_PARAMS *);
    NV_STATUS (*__confComputeApiControlSerialization_Prologue__)(struct ConfidentialComputeApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__confComputeApiControl_Prologue__)(struct ConfidentialComputeApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__confComputeApiCanCopy__)(struct ConfidentialComputeApi *);
    NV_STATUS (*__confComputeApiUnmap__)(struct ConfidentialComputeApi *, struct CALL_CONTEXT *, RsCpuMapping *);
    void (*__confComputeApiPreDestruct__)(struct ConfidentialComputeApi *);
    NV_STATUS (*__confComputeApiMapTo__)(struct ConfidentialComputeApi *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__confComputeApiIsDuplicate__)(struct ConfidentialComputeApi *, NvHandle, NvBool *);
    void (*__confComputeApiControlSerialization_Epilogue__)(struct ConfidentialComputeApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    void (*__confComputeApiControl_Epilogue__)(struct ConfidentialComputeApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__confComputeApiControlLookup__)(struct ConfidentialComputeApi *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NV_STATUS (*__confComputeApiMap__)(struct ConfidentialComputeApi *, struct CALL_CONTEXT *, RS_CPU_MAP_PARAMS *, RsCpuMapping *);
    NvBool (*__confComputeApiAccessCallback__)(struct ConfidentialComputeApi *, struct RsClient *, void *, RsAccessRight);
    struct CONF_COMPUTE_CAPS *pCcCaps;
};

#ifndef __NVOC_CLASS_ConfidentialComputeApi_TYPEDEF__
#define __NVOC_CLASS_ConfidentialComputeApi_TYPEDEF__
typedef struct ConfidentialComputeApi ConfidentialComputeApi;
#endif /* __NVOC_CLASS_ConfidentialComputeApi_TYPEDEF__ */

#ifndef __nvoc_class_id_ConfidentialComputeApi
#define __nvoc_class_id_ConfidentialComputeApi 0xea5cb0
#endif /* __nvoc_class_id_ConfidentialComputeApi */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_ConfidentialComputeApi;

#define __staticCast_ConfidentialComputeApi(pThis) \
    ((pThis)->__nvoc_pbase_ConfidentialComputeApi)

#ifdef __nvoc_conf_compute_api_h_disabled
#define __dynamicCast_ConfidentialComputeApi(pThis) ((ConfidentialComputeApi*)NULL)
#else //__nvoc_conf_compute_api_h_disabled
#define __dynamicCast_ConfidentialComputeApi(pThis) \
    ((ConfidentialComputeApi*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(ConfidentialComputeApi)))
#endif //__nvoc_conf_compute_api_h_disabled


NV_STATUS __nvoc_objCreateDynamic_ConfidentialComputeApi(ConfidentialComputeApi**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_ConfidentialComputeApi(ConfidentialComputeApi**, Dynamic*, NvU32, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_ConfidentialComputeApi(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_ConfidentialComputeApi((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define confComputeApiCtrlCmdSystemGetCapabilities(pConfComputeApi, pParams) confComputeApiCtrlCmdSystemGetCapabilities_DISPATCH(pConfComputeApi, pParams)
#define confComputeApiCtrlCmdSystemGetGpusState(pConfComputeApi, pParams) confComputeApiCtrlCmdSystemGetGpusState_DISPATCH(pConfComputeApi, pParams)
#define confComputeApiCtrlCmdSystemSetGpusState(pConfComputeApi, pParams) confComputeApiCtrlCmdSystemSetGpusState_DISPATCH(pConfComputeApi, pParams)
#define confComputeApiCtrlCmdGpuGetVidmemSize(pConfComputeApi, pParams) confComputeApiCtrlCmdGpuGetVidmemSize_DISPATCH(pConfComputeApi, pParams)
#define confComputeApiCtrlCmdGpuSetVidmemSize(pConfComputeApi, pParams) confComputeApiCtrlCmdGpuSetVidmemSize_DISPATCH(pConfComputeApi, pParams)
#define confComputeApiCtrlCmdGetGpuCertificate(pConfComputeApi, pParams) confComputeApiCtrlCmdGetGpuCertificate_DISPATCH(pConfComputeApi, pParams)
#define confComputeApiCtrlCmdGetGpuAttestationReport(pConfComputeApi, pParams) confComputeApiCtrlCmdGetGpuAttestationReport_DISPATCH(pConfComputeApi, pParams)
#define confComputeApiCtrlCmdGpuGetNumSecureChannels(pConfComputeApi, pParams) confComputeApiCtrlCmdGpuGetNumSecureChannels_DISPATCH(pConfComputeApi, pParams)
#define confComputeApiShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) confComputeApiShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define confComputeApiCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) confComputeApiCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define confComputeApiControl(pResource, pCallContext, pParams) confComputeApiControl_DISPATCH(pResource, pCallContext, pParams)
#define confComputeApiGetMemInterMapParams(pRmResource, pParams) confComputeApiGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define confComputeApiGetMemoryMappingDescriptor(pRmResource, ppMemDesc) confComputeApiGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define confComputeApiGetRefCount(pResource) confComputeApiGetRefCount_DISPATCH(pResource)
#define confComputeApiControlFilter(pResource, pCallContext, pParams) confComputeApiControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define confComputeApiAddAdditionalDependants(pClient, pResource, pReference) confComputeApiAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define confComputeApiUnmapFrom(pResource, pParams) confComputeApiUnmapFrom_DISPATCH(pResource, pParams)
#define confComputeApiControlSerialization_Prologue(pResource, pCallContext, pParams) confComputeApiControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define confComputeApiControl_Prologue(pResource, pCallContext, pParams) confComputeApiControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define confComputeApiCanCopy(pResource) confComputeApiCanCopy_DISPATCH(pResource)
#define confComputeApiUnmap(pResource, pCallContext, pCpuMapping) confComputeApiUnmap_DISPATCH(pResource, pCallContext, pCpuMapping)
#define confComputeApiPreDestruct(pResource) confComputeApiPreDestruct_DISPATCH(pResource)
#define confComputeApiMapTo(pResource, pParams) confComputeApiMapTo_DISPATCH(pResource, pParams)
#define confComputeApiIsDuplicate(pResource, hMemory, pDuplicate) confComputeApiIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define confComputeApiControlSerialization_Epilogue(pResource, pCallContext, pParams) confComputeApiControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define confComputeApiControl_Epilogue(pResource, pCallContext, pParams) confComputeApiControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define confComputeApiControlLookup(pResource, pParams, ppEntry) confComputeApiControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define confComputeApiMap(pResource, pCallContext, pParams, pCpuMapping) confComputeApiMap_DISPATCH(pResource, pCallContext, pParams, pCpuMapping)
#define confComputeApiAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) confComputeApiAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
NV_STATUS confComputeApiCtrlCmdSystemGetCapabilities_IMPL(struct ConfidentialComputeApi *pConfComputeApi, NV_CONF_COMPUTE_CTRL_CMD_SYSTEM_GET_CAPABILITIES_PARAMS *pParams);

static inline NV_STATUS confComputeApiCtrlCmdSystemGetCapabilities_DISPATCH(struct ConfidentialComputeApi *pConfComputeApi, NV_CONF_COMPUTE_CTRL_CMD_SYSTEM_GET_CAPABILITIES_PARAMS *pParams) {
    return pConfComputeApi->__confComputeApiCtrlCmdSystemGetCapabilities__(pConfComputeApi, pParams);
}

NV_STATUS confComputeApiCtrlCmdSystemGetGpusState_IMPL(struct ConfidentialComputeApi *pConfComputeApi, NV_CONF_COMPUTE_CTRL_CMD_SYSTEM_GET_GPUS_STATE_PARAMS *pParams);

static inline NV_STATUS confComputeApiCtrlCmdSystemGetGpusState_DISPATCH(struct ConfidentialComputeApi *pConfComputeApi, NV_CONF_COMPUTE_CTRL_CMD_SYSTEM_GET_GPUS_STATE_PARAMS *pParams) {
    return pConfComputeApi->__confComputeApiCtrlCmdSystemGetGpusState__(pConfComputeApi, pParams);
}

NV_STATUS confComputeApiCtrlCmdSystemSetGpusState_IMPL(struct ConfidentialComputeApi *pConfComputeApi, NV_CONF_COMPUTE_CTRL_CMD_SYSTEM_SET_GPUS_STATE_PARAMS *pParams);

static inline NV_STATUS confComputeApiCtrlCmdSystemSetGpusState_DISPATCH(struct ConfidentialComputeApi *pConfComputeApi, NV_CONF_COMPUTE_CTRL_CMD_SYSTEM_SET_GPUS_STATE_PARAMS *pParams) {
    return pConfComputeApi->__confComputeApiCtrlCmdSystemSetGpusState__(pConfComputeApi, pParams);
}

NV_STATUS confComputeApiCtrlCmdGpuGetVidmemSize_IMPL(struct ConfidentialComputeApi *pConfComputeApi, NV_CONF_COMPUTE_CTRL_CMD_GPU_GET_VIDMEM_SIZE_PARAMS *pParams);

static inline NV_STATUS confComputeApiCtrlCmdGpuGetVidmemSize_DISPATCH(struct ConfidentialComputeApi *pConfComputeApi, NV_CONF_COMPUTE_CTRL_CMD_GPU_GET_VIDMEM_SIZE_PARAMS *pParams) {
    return pConfComputeApi->__confComputeApiCtrlCmdGpuGetVidmemSize__(pConfComputeApi, pParams);
}

NV_STATUS confComputeApiCtrlCmdGpuSetVidmemSize_IMPL(struct ConfidentialComputeApi *pConfComputeApi, NV_CONF_COMPUTE_CTRL_CMD_GPU_SET_VIDMEM_SIZE_PARAMS *pParams);

static inline NV_STATUS confComputeApiCtrlCmdGpuSetVidmemSize_DISPATCH(struct ConfidentialComputeApi *pConfComputeApi, NV_CONF_COMPUTE_CTRL_CMD_GPU_SET_VIDMEM_SIZE_PARAMS *pParams) {
    return pConfComputeApi->__confComputeApiCtrlCmdGpuSetVidmemSize__(pConfComputeApi, pParams);
}

NV_STATUS confComputeApiCtrlCmdGetGpuCertificate_IMPL(struct ConfidentialComputeApi *pConfComputeApi, NV_CONF_COMPUTE_CTRL_CMD_GET_GPU_CERTIFICATE_PARAMS *pParams);

static inline NV_STATUS confComputeApiCtrlCmdGetGpuCertificate_DISPATCH(struct ConfidentialComputeApi *pConfComputeApi, NV_CONF_COMPUTE_CTRL_CMD_GET_GPU_CERTIFICATE_PARAMS *pParams) {
    return pConfComputeApi->__confComputeApiCtrlCmdGetGpuCertificate__(pConfComputeApi, pParams);
}

NV_STATUS confComputeApiCtrlCmdGetGpuAttestationReport_IMPL(struct ConfidentialComputeApi *pConfComputeApi, NV_CONF_COMPUTE_CTRL_CMD_GET_GPU_ATTESTATION_REPORT_PARAMS *pParams);

static inline NV_STATUS confComputeApiCtrlCmdGetGpuAttestationReport_DISPATCH(struct ConfidentialComputeApi *pConfComputeApi, NV_CONF_COMPUTE_CTRL_CMD_GET_GPU_ATTESTATION_REPORT_PARAMS *pParams) {
    return pConfComputeApi->__confComputeApiCtrlCmdGetGpuAttestationReport__(pConfComputeApi, pParams);
}

NV_STATUS confComputeApiCtrlCmdGpuGetNumSecureChannels_IMPL(struct ConfidentialComputeApi *pConfComputeApi, NV_CONF_COMPUTE_CTRL_CMD_GPU_GET_NUM_SECURE_CHANNELS_PARAMS *pParams);

static inline NV_STATUS confComputeApiCtrlCmdGpuGetNumSecureChannels_DISPATCH(struct ConfidentialComputeApi *pConfComputeApi, NV_CONF_COMPUTE_CTRL_CMD_GPU_GET_NUM_SECURE_CHANNELS_PARAMS *pParams) {
    return pConfComputeApi->__confComputeApiCtrlCmdGpuGetNumSecureChannels__(pConfComputeApi, pParams);
}

static inline NvBool confComputeApiShareCallback_DISPATCH(struct ConfidentialComputeApi *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__confComputeApiShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS confComputeApiCheckMemInterUnmap_DISPATCH(struct ConfidentialComputeApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__confComputeApiCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS confComputeApiControl_DISPATCH(struct ConfidentialComputeApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__confComputeApiControl__(pResource, pCallContext, pParams);
}

static inline NV_STATUS confComputeApiGetMemInterMapParams_DISPATCH(struct ConfidentialComputeApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__confComputeApiGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS confComputeApiGetMemoryMappingDescriptor_DISPATCH(struct ConfidentialComputeApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__confComputeApiGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NvU32 confComputeApiGetRefCount_DISPATCH(struct ConfidentialComputeApi *pResource) {
    return pResource->__confComputeApiGetRefCount__(pResource);
}

static inline NV_STATUS confComputeApiControlFilter_DISPATCH(struct ConfidentialComputeApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__confComputeApiControlFilter__(pResource, pCallContext, pParams);
}

static inline void confComputeApiAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct ConfidentialComputeApi *pResource, RsResourceRef *pReference) {
    pResource->__confComputeApiAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS confComputeApiUnmapFrom_DISPATCH(struct ConfidentialComputeApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__confComputeApiUnmapFrom__(pResource, pParams);
}

static inline NV_STATUS confComputeApiControlSerialization_Prologue_DISPATCH(struct ConfidentialComputeApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__confComputeApiControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS confComputeApiControl_Prologue_DISPATCH(struct ConfidentialComputeApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__confComputeApiControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool confComputeApiCanCopy_DISPATCH(struct ConfidentialComputeApi *pResource) {
    return pResource->__confComputeApiCanCopy__(pResource);
}

static inline NV_STATUS confComputeApiUnmap_DISPATCH(struct ConfidentialComputeApi *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pResource->__confComputeApiUnmap__(pResource, pCallContext, pCpuMapping);
}

static inline void confComputeApiPreDestruct_DISPATCH(struct ConfidentialComputeApi *pResource) {
    pResource->__confComputeApiPreDestruct__(pResource);
}

static inline NV_STATUS confComputeApiMapTo_DISPATCH(struct ConfidentialComputeApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__confComputeApiMapTo__(pResource, pParams);
}

static inline NV_STATUS confComputeApiIsDuplicate_DISPATCH(struct ConfidentialComputeApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__confComputeApiIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void confComputeApiControlSerialization_Epilogue_DISPATCH(struct ConfidentialComputeApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__confComputeApiControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline void confComputeApiControl_Epilogue_DISPATCH(struct ConfidentialComputeApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__confComputeApiControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS confComputeApiControlLookup_DISPATCH(struct ConfidentialComputeApi *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__confComputeApiControlLookup__(pResource, pParams, ppEntry);
}

static inline NV_STATUS confComputeApiMap_DISPATCH(struct ConfidentialComputeApi *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pResource->__confComputeApiMap__(pResource, pCallContext, pParams, pCpuMapping);
}

static inline NvBool confComputeApiAccessCallback_DISPATCH(struct ConfidentialComputeApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__confComputeApiAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

NV_STATUS confComputeApiConstruct_IMPL(struct ConfidentialComputeApi *arg_pConfComputeApi, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_confComputeApiConstruct(arg_pConfComputeApi, arg_pCallContext, arg_pParams) confComputeApiConstruct_IMPL(arg_pConfComputeApi, arg_pCallContext, arg_pParams)
void confComputeApiDestruct_IMPL(struct ConfidentialComputeApi *pConfComputeApi);

#define __nvoc_confComputeApiDestruct(pConfComputeApi) confComputeApiDestruct_IMPL(pConfComputeApi)
#undef PRIVATE_FIELD

#endif // CONF_COMPUTE_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_CONF_COMPUTE_API_NVOC_H_
