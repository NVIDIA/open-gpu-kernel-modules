#ifndef _G_PROFILER_V1_NVOC_H_
#define _G_PROFILER_V1_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_profiler_v1_nvoc.h"

#ifndef PROFILER_V1_H
#define PROFILER_V1_H

#include "resserv/resserv.h"
#include "nvoc/prelude.h"
#include "resserv/rs_resource.h"
#include "ctrl/ctrl90cc.h"
#include "gpu/gpu_resource.h"
#include "gpu/gpu_halspec.h"
#include "nvoc/utility.h"

#ifdef NVOC_PROFILER_V1_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct Profiler {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct GpuResource __nvoc_base_GpuResource;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct GpuResource *__nvoc_pbase_GpuResource;
    struct Profiler *__nvoc_pbase_Profiler;
    NV_STATUS (*__profilerControl__)(struct Profiler *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__profilerCtrlCmdProfilerReserveHwpm__)(struct Profiler *);
    NV_STATUS (*__profilerCtrlCmdProfilerReleaseHwpm__)(struct Profiler *);
    NV_STATUS (*__profilerCtrlCmdProfilerGetHwpmReservationInfo__)(struct Profiler *, NV90CC_CTRL_HWPM_GET_RESERVATION_INFO_PARAMS *);
    NV_STATUS (*__profilerCtrlCmdProfilerRequestCgControls__)(struct Profiler *, NV90CC_CTRL_POWER_REQUEST_FEATURES_PARAMS *);
    NV_STATUS (*__profilerCtrlCmdProfilerReleaseCgControls__)(struct Profiler *, NV90CC_CTRL_POWER_RELEASE_FEATURES_PARAMS *);
    NvBool (*__profilerShareCallback__)(struct Profiler *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__profilerCheckMemInterUnmap__)(struct Profiler *, NvBool);
    NV_STATUS (*__profilerMapTo__)(struct Profiler *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__profilerGetMapAddrSpace__)(struct Profiler *, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    NvU32 (*__profilerGetRefCount__)(struct Profiler *);
    void (*__profilerAddAdditionalDependants__)(struct RsClient *, struct Profiler *, RsResourceRef *);
    NV_STATUS (*__profilerControl_Prologue__)(struct Profiler *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__profilerGetRegBaseOffsetAndSize__)(struct Profiler *, struct OBJGPU *, NvU32 *, NvU32 *);
    NV_STATUS (*__profilerInternalControlForward__)(struct Profiler *, NvU32, void *, NvU32);
    NV_STATUS (*__profilerUnmapFrom__)(struct Profiler *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__profilerControl_Epilogue__)(struct Profiler *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__profilerControlLookup__)(struct Profiler *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NvHandle (*__profilerGetInternalObjectHandle__)(struct Profiler *);
    NV_STATUS (*__profilerUnmap__)(struct Profiler *, struct CALL_CONTEXT *, struct RsCpuMapping *);
    NV_STATUS (*__profilerGetMemInterMapParams__)(struct Profiler *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__profilerGetMemoryMappingDescriptor__)(struct Profiler *, struct MEMORY_DESCRIPTOR **);
    NV_STATUS (*__profilerControlFilter__)(struct Profiler *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__profilerControlSerialization_Prologue__)(struct Profiler *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__profilerCanCopy__)(struct Profiler *);
    void (*__profilerPreDestruct__)(struct Profiler *);
    NV_STATUS (*__profilerIsDuplicate__)(struct Profiler *, NvHandle, NvBool *);
    void (*__profilerControlSerialization_Epilogue__)(struct Profiler *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__profilerMap__)(struct Profiler *, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);
    NvBool (*__profilerAccessCallback__)(struct Profiler *, struct RsClient *, void *, RsAccessRight);
};

#ifndef __NVOC_CLASS_Profiler_TYPEDEF__
#define __NVOC_CLASS_Profiler_TYPEDEF__
typedef struct Profiler Profiler;
#endif /* __NVOC_CLASS_Profiler_TYPEDEF__ */

#ifndef __nvoc_class_id_Profiler
#define __nvoc_class_id_Profiler 0x65b4c7
#endif /* __nvoc_class_id_Profiler */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Profiler;

#define __staticCast_Profiler(pThis) \
    ((pThis)->__nvoc_pbase_Profiler)

#ifdef __nvoc_profiler_v1_h_disabled
#define __dynamicCast_Profiler(pThis) ((Profiler*)NULL)
#else //__nvoc_profiler_v1_h_disabled
#define __dynamicCast_Profiler(pThis) \
    ((Profiler*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(Profiler)))
#endif //__nvoc_profiler_v1_h_disabled


NV_STATUS __nvoc_objCreateDynamic_Profiler(Profiler**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_Profiler(Profiler**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_Profiler(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_Profiler((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define profilerControl(pProfiler, pCallContext, pParams) profilerControl_DISPATCH(pProfiler, pCallContext, pParams)
#define profilerCtrlCmdProfilerReserveHwpm(pProfiler) profilerCtrlCmdProfilerReserveHwpm_DISPATCH(pProfiler)
#define profilerCtrlCmdProfilerReleaseHwpm(pProfiler) profilerCtrlCmdProfilerReleaseHwpm_DISPATCH(pProfiler)
#define profilerCtrlCmdProfilerGetHwpmReservationInfo(pProfiler, pParams) profilerCtrlCmdProfilerGetHwpmReservationInfo_DISPATCH(pProfiler, pParams)
#define profilerCtrlCmdProfilerRequestCgControls(pProfiler, pParams) profilerCtrlCmdProfilerRequestCgControls_DISPATCH(pProfiler, pParams)
#define profilerCtrlCmdProfilerReleaseCgControls(pProfiler, pParams) profilerCtrlCmdProfilerReleaseCgControls_DISPATCH(pProfiler, pParams)
#define profilerShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) profilerShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define profilerCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) profilerCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define profilerMapTo(pResource, pParams) profilerMapTo_DISPATCH(pResource, pParams)
#define profilerGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) profilerGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define profilerGetRefCount(pResource) profilerGetRefCount_DISPATCH(pResource)
#define profilerAddAdditionalDependants(pClient, pResource, pReference) profilerAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define profilerControl_Prologue(pResource, pCallContext, pParams) profilerControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define profilerGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) profilerGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define profilerInternalControlForward(pGpuResource, command, pParams, size) profilerInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define profilerUnmapFrom(pResource, pParams) profilerUnmapFrom_DISPATCH(pResource, pParams)
#define profilerControl_Epilogue(pResource, pCallContext, pParams) profilerControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define profilerControlLookup(pResource, pParams, ppEntry) profilerControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define profilerGetInternalObjectHandle(pGpuResource) profilerGetInternalObjectHandle_DISPATCH(pGpuResource)
#define profilerUnmap(pGpuResource, pCallContext, pCpuMapping) profilerUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define profilerGetMemInterMapParams(pRmResource, pParams) profilerGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define profilerGetMemoryMappingDescriptor(pRmResource, ppMemDesc) profilerGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define profilerControlFilter(pResource, pCallContext, pParams) profilerControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define profilerControlSerialization_Prologue(pResource, pCallContext, pParams) profilerControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define profilerCanCopy(pResource) profilerCanCopy_DISPATCH(pResource)
#define profilerPreDestruct(pResource) profilerPreDestruct_DISPATCH(pResource)
#define profilerIsDuplicate(pResource, hMemory, pDuplicate) profilerIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define profilerControlSerialization_Epilogue(pResource, pCallContext, pParams) profilerControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define profilerMap(pGpuResource, pCallContext, pParams, pCpuMapping) profilerMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define profilerAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) profilerAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
NvBool profilerIsProfilingPermitted_IMPL(struct Profiler *pProfiler);


#ifdef __nvoc_profiler_v1_h_disabled
static inline NvBool profilerIsProfilingPermitted(struct Profiler *pProfiler) {
    NV_ASSERT_FAILED_PRECOMP("Profiler was disabled!");
    return NV_FALSE;
}
#else //__nvoc_profiler_v1_h_disabled
#define profilerIsProfilingPermitted(pProfiler) profilerIsProfilingPermitted_IMPL(pProfiler)
#endif //__nvoc_profiler_v1_h_disabled

#define profilerIsProfilingPermitted_HAL(pProfiler) profilerIsProfilingPermitted(pProfiler)

static inline NV_STATUS profilerConstructState_ac1694(struct Profiler *pProfiler, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {
    return NV_OK;
}


#ifdef __nvoc_profiler_v1_h_disabled
static inline NV_STATUS profilerConstructState(struct Profiler *pProfiler, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Profiler was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_profiler_v1_h_disabled
#define profilerConstructState(pProfiler, pCallContext, pParams) profilerConstructState_ac1694(pProfiler, pCallContext, pParams)
#endif //__nvoc_profiler_v1_h_disabled

#define profilerConstructState_HAL(pProfiler, pCallContext, pParams) profilerConstructState(pProfiler, pCallContext, pParams)

static inline void profilerDestruct_d44104(struct Profiler *pProfiler) {
    return;
}


#define __nvoc_profilerDestruct(pProfiler) profilerDestruct_d44104(pProfiler)
static inline NV_STATUS profilerControlHwpmSupported_ac1694(struct Profiler *pProfiler, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return NV_OK;
}


#ifdef __nvoc_profiler_v1_h_disabled
static inline NV_STATUS profilerControlHwpmSupported(struct Profiler *pProfiler, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Profiler was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_profiler_v1_h_disabled
#define profilerControlHwpmSupported(pProfiler, pParams) profilerControlHwpmSupported_ac1694(pProfiler, pParams)
#endif //__nvoc_profiler_v1_h_disabled

#define profilerControlHwpmSupported_HAL(pProfiler, pParams) profilerControlHwpmSupported(pProfiler, pParams)

NV_STATUS profilerControl_IMPL(struct Profiler *pProfiler, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);

static inline NV_STATUS profilerControl_DISPATCH(struct Profiler *pProfiler, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pProfiler->__profilerControl__(pProfiler, pCallContext, pParams);
}

NV_STATUS profilerCtrlCmdProfilerReserveHwpm_IMPL(struct Profiler *pProfiler);

static inline NV_STATUS profilerCtrlCmdProfilerReserveHwpm_DISPATCH(struct Profiler *pProfiler) {
    return pProfiler->__profilerCtrlCmdProfilerReserveHwpm__(pProfiler);
}

NV_STATUS profilerCtrlCmdProfilerReleaseHwpm_IMPL(struct Profiler *pProfiler);

static inline NV_STATUS profilerCtrlCmdProfilerReleaseHwpm_DISPATCH(struct Profiler *pProfiler) {
    return pProfiler->__profilerCtrlCmdProfilerReleaseHwpm__(pProfiler);
}

NV_STATUS profilerCtrlCmdProfilerGetHwpmReservationInfo_IMPL(struct Profiler *pProfiler, NV90CC_CTRL_HWPM_GET_RESERVATION_INFO_PARAMS *pParams);

static inline NV_STATUS profilerCtrlCmdProfilerGetHwpmReservationInfo_DISPATCH(struct Profiler *pProfiler, NV90CC_CTRL_HWPM_GET_RESERVATION_INFO_PARAMS *pParams) {
    return pProfiler->__profilerCtrlCmdProfilerGetHwpmReservationInfo__(pProfiler, pParams);
}

NV_STATUS profilerCtrlCmdProfilerRequestCgControls_IMPL(struct Profiler *pProfiler, NV90CC_CTRL_POWER_REQUEST_FEATURES_PARAMS *pParams);

static inline NV_STATUS profilerCtrlCmdProfilerRequestCgControls_DISPATCH(struct Profiler *pProfiler, NV90CC_CTRL_POWER_REQUEST_FEATURES_PARAMS *pParams) {
    return pProfiler->__profilerCtrlCmdProfilerRequestCgControls__(pProfiler, pParams);
}

NV_STATUS profilerCtrlCmdProfilerReleaseCgControls_IMPL(struct Profiler *pProfiler, NV90CC_CTRL_POWER_RELEASE_FEATURES_PARAMS *pParams);

static inline NV_STATUS profilerCtrlCmdProfilerReleaseCgControls_DISPATCH(struct Profiler *pProfiler, NV90CC_CTRL_POWER_RELEASE_FEATURES_PARAMS *pParams) {
    return pProfiler->__profilerCtrlCmdProfilerReleaseCgControls__(pProfiler, pParams);
}

static inline NvBool profilerShareCallback_DISPATCH(struct Profiler *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__profilerShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS profilerCheckMemInterUnmap_DISPATCH(struct Profiler *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__profilerCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS profilerMapTo_DISPATCH(struct Profiler *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__profilerMapTo__(pResource, pParams);
}

static inline NV_STATUS profilerGetMapAddrSpace_DISPATCH(struct Profiler *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__profilerGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NvU32 profilerGetRefCount_DISPATCH(struct Profiler *pResource) {
    return pResource->__profilerGetRefCount__(pResource);
}

static inline void profilerAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct Profiler *pResource, RsResourceRef *pReference) {
    pResource->__profilerAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS profilerControl_Prologue_DISPATCH(struct Profiler *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__profilerControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS profilerGetRegBaseOffsetAndSize_DISPATCH(struct Profiler *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__profilerGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS profilerInternalControlForward_DISPATCH(struct Profiler *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__profilerInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NV_STATUS profilerUnmapFrom_DISPATCH(struct Profiler *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__profilerUnmapFrom__(pResource, pParams);
}

static inline void profilerControl_Epilogue_DISPATCH(struct Profiler *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__profilerControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS profilerControlLookup_DISPATCH(struct Profiler *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__profilerControlLookup__(pResource, pParams, ppEntry);
}

static inline NvHandle profilerGetInternalObjectHandle_DISPATCH(struct Profiler *pGpuResource) {
    return pGpuResource->__profilerGetInternalObjectHandle__(pGpuResource);
}

static inline NV_STATUS profilerUnmap_DISPATCH(struct Profiler *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__profilerUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NV_STATUS profilerGetMemInterMapParams_DISPATCH(struct Profiler *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__profilerGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS profilerGetMemoryMappingDescriptor_DISPATCH(struct Profiler *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__profilerGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS profilerControlFilter_DISPATCH(struct Profiler *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__profilerControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS profilerControlSerialization_Prologue_DISPATCH(struct Profiler *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__profilerControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool profilerCanCopy_DISPATCH(struct Profiler *pResource) {
    return pResource->__profilerCanCopy__(pResource);
}

static inline void profilerPreDestruct_DISPATCH(struct Profiler *pResource) {
    pResource->__profilerPreDestruct__(pResource);
}

static inline NV_STATUS profilerIsDuplicate_DISPATCH(struct Profiler *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__profilerIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void profilerControlSerialization_Epilogue_DISPATCH(struct Profiler *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__profilerControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS profilerMap_DISPATCH(struct Profiler *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__profilerMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NvBool profilerAccessCallback_DISPATCH(struct Profiler *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__profilerAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

NV_STATUS profilerConstruct_IMPL(struct Profiler *arg_pProfiler, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_profilerConstruct(arg_pProfiler, arg_pCallContext, arg_pParams) profilerConstruct_IMPL(arg_pProfiler, arg_pCallContext, arg_pParams)
#undef PRIVATE_FIELD


#endif // PROFILER_V1_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_PROFILER_V1_NVOC_H_
