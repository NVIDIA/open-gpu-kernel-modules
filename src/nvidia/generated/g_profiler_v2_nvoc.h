#ifndef _G_PROFILER_V2_NVOC_H_
#define _G_PROFILER_V2_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_profiler_v2_nvoc.h"

#ifndef PROFILER_V2_H
#define PROFILER_V2_H

#include "resserv/resserv.h"
#include "nvoc/prelude.h"
#include "resserv/rs_resource.h"
#include "gpu/gpu_resource.h"
#include "ctrl/ctrl2080/ctrl2080gpu.h"
#include "ctrl/ctrlb0cc.h"
#include "gpu/gpu_halspec.h"
#include "nvoc/utility.h"

#include "class/clb2cc.h"   // MAXWELL_PROFILER_DEVICE

typedef struct
{
    NvBool bMemoryProfilingPermitted;
    NvBool bAdminProfilingPermitted;
    NvBool bDevProfilingPermitted;
} PROFILER_CLIENT_PERMISSIONS;

#ifdef NVOC_PROFILER_V2_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct ProfilerBase {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct GpuResource __nvoc_base_GpuResource;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct GpuResource *__nvoc_pbase_GpuResource;
    struct ProfilerBase *__nvoc_pbase_ProfilerBase;
    NV_STATUS (*__profilerBaseCtrlCmdReserveHwpmLegacy__)(struct ProfilerBase *, NVB0CC_CTRL_RESERVE_HWPM_LEGACY_PARAMS *);
    NV_STATUS (*__profilerBaseCtrlCmdInternalReserveHwpmLegacy__)(struct ProfilerBase *, NVB0CC_CTRL_INTERNAL_RESERVE_HWPM_LEGACY_PARAMS *);
    NV_STATUS (*__profilerBaseCtrlCmdReleaseHwpmLegacy__)(struct ProfilerBase *);
    NV_STATUS (*__profilerBaseCtrlCmdReservePmAreaSmpc__)(struct ProfilerBase *, NVB0CC_CTRL_RESERVE_PM_AREA_SMPC_PARAMS *);
    NV_STATUS (*__profilerBaseCtrlCmdReleasePmAreaSmpc__)(struct ProfilerBase *);
    NV_STATUS (*__profilerBaseCtrlCmdAllocPmaStream__)(struct ProfilerBase *, NVB0CC_CTRL_ALLOC_PMA_STREAM_PARAMS *);
    NV_STATUS (*__profilerBaseCtrlCmdFreePmaStream__)(struct ProfilerBase *, NVB0CC_CTRL_FREE_PMA_STREAM_PARAMS *);
    NV_STATUS (*__profilerBaseCtrlCmdInternalFreePmaStream__)(struct ProfilerBase *, NVB0CC_CTRL_INTERNAL_FREE_PMA_STREAM_PARAMS *);
    NV_STATUS (*__profilerBaseCtrlCmdInternalGetMaxPmas__)(struct ProfilerBase *, NVB0CC_CTRL_INTERNAL_GET_MAX_PMAS_PARAMS *);
    NV_STATUS (*__profilerBaseCtrlCmdBindPmResources__)(struct ProfilerBase *);
    NV_STATUS (*__profilerBaseCtrlCmdUnbindPmResources__)(struct ProfilerBase *);
    NV_STATUS (*__profilerBaseCtrlCmdInternalBindPmResources__)(struct ProfilerBase *);
    NV_STATUS (*__profilerBaseCtrlCmdInternalUnbindPmResources__)(struct ProfilerBase *);
    NV_STATUS (*__profilerBaseCtrlCmdPmaStreamUpdateGetPut__)(struct ProfilerBase *, NVB0CC_CTRL_PMA_STREAM_UPDATE_GET_PUT_PARAMS *);
    NV_STATUS (*__profilerBaseCtrlCmdExecRegops__)(struct ProfilerBase *, NVB0CC_CTRL_EXEC_REG_OPS_PARAMS *);
    NV_STATUS (*__profilerBaseCtrlCmdInternalAllocPmaStream__)(struct ProfilerBase *, NVB0CC_CTRL_INTERNAL_ALLOC_PMA_STREAM_PARAMS *);
    NV_STATUS (*__profilerBaseCtrlCmdInternalPermissionsInit__)(struct ProfilerBase *, NVB0CC_CTRL_INTERNAL_PERMISSIONS_INIT_PARAMS *);
    NV_STATUS (*__profilerBaseCtrlCmdReservePmAreaPcSampler__)(struct ProfilerBase *);
    NV_STATUS (*__profilerBaseCtrlCmdReleasePmAreaPcSampler__)(struct ProfilerBase *);
    NV_STATUS (*__profilerBaseCtrlCmdGetTotalHsCredits__)(struct ProfilerBase *, NVB0CC_CTRL_GET_TOTAL_HS_CREDITS_PARAMS *);
    NV_STATUS (*__profilerBaseCtrlCmdGetHsCredits__)(struct ProfilerBase *, NVB0CC_CTRL_GET_HS_CREDITS_PARAMS *);
    NV_STATUS (*__profilerBaseCtrlCmdSetHsCredits__)(struct ProfilerBase *, NVB0CC_CTRL_SET_HS_CREDITS_PARAMS *);
    NV_STATUS (*__profilerBaseCtrlCmdReserveHes__)(struct ProfilerBase *, NVB0CC_CTRL_RESERVE_HES_PARAMS *);
    NV_STATUS (*__profilerBaseCtrlCmdReleaseHes__)(struct ProfilerBase *, NVB0CC_CTRL_RELEASE_HES_PARAMS *);
    NV_STATUS (*__profilerBaseCtrlCmdRequestCgControls__)(struct ProfilerBase *, NVB0CC_CTRL_POWER_REQUEST_FEATURES_PARAMS *);
    NV_STATUS (*__profilerBaseCtrlCmdReleaseCgControls__)(struct ProfilerBase *, NVB0CC_CTRL_POWER_RELEASE_FEATURES_PARAMS *);
    NV_STATUS (*__profilerBaseCtrlCmdDisableDynamicMMABoost__)(struct ProfilerBase *, NVB0CC_CTRL_DISABLE_DYNAMIC_MMA_BOOST_PARAMS *);
    NV_STATUS (*__profilerBaseCtrlCmdGetDynamicMMABoostStatus__)(struct ProfilerBase *, NVB0CC_CTRL_GET_DYNAMIC_MMA_BOOST_STATUS_PARAMS *);
    NvBool (*__profilerBaseShareCallback__)(struct ProfilerBase *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__profilerBaseCheckMemInterUnmap__)(struct ProfilerBase *, NvBool);
    NV_STATUS (*__profilerBaseMapTo__)(struct ProfilerBase *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__profilerBaseGetMapAddrSpace__)(struct ProfilerBase *, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    NvU32 (*__profilerBaseGetRefCount__)(struct ProfilerBase *);
    void (*__profilerBaseAddAdditionalDependants__)(struct RsClient *, struct ProfilerBase *, RsResourceRef *);
    NV_STATUS (*__profilerBaseControl_Prologue__)(struct ProfilerBase *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__profilerBaseGetRegBaseOffsetAndSize__)(struct ProfilerBase *, struct OBJGPU *, NvU32 *, NvU32 *);
    NV_STATUS (*__profilerBaseInternalControlForward__)(struct ProfilerBase *, NvU32, void *, NvU32);
    NV_STATUS (*__profilerBaseUnmapFrom__)(struct ProfilerBase *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__profilerBaseControl_Epilogue__)(struct ProfilerBase *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__profilerBaseControlLookup__)(struct ProfilerBase *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NvHandle (*__profilerBaseGetInternalObjectHandle__)(struct ProfilerBase *);
    NV_STATUS (*__profilerBaseControl__)(struct ProfilerBase *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__profilerBaseUnmap__)(struct ProfilerBase *, struct CALL_CONTEXT *, struct RsCpuMapping *);
    NV_STATUS (*__profilerBaseGetMemInterMapParams__)(struct ProfilerBase *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__profilerBaseGetMemoryMappingDescriptor__)(struct ProfilerBase *, struct MEMORY_DESCRIPTOR **);
    NV_STATUS (*__profilerBaseControlFilter__)(struct ProfilerBase *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__profilerBaseControlSerialization_Prologue__)(struct ProfilerBase *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__profilerBaseCanCopy__)(struct ProfilerBase *);
    void (*__profilerBasePreDestruct__)(struct ProfilerBase *);
    NV_STATUS (*__profilerBaseIsDuplicate__)(struct ProfilerBase *, NvHandle, NvBool *);
    void (*__profilerBaseControlSerialization_Epilogue__)(struct ProfilerBase *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__profilerBaseMap__)(struct ProfilerBase *, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);
    NvBool (*__profilerBaseAccessCallback__)(struct ProfilerBase *, struct RsClient *, void *, RsAccessRight);
    NvU32 maxPmaChannels;
    NvU32 pmaVchIdx;
    NvBool bLegacyHwpm;
    struct RsResourceRef **ppBytesAvailable;
    struct RsResourceRef **ppStreamBuffers;
    struct RsResourceRef *pBoundCntBuf;
    struct RsResourceRef *pBoundPmaBuf;
};

#ifndef __NVOC_CLASS_ProfilerBase_TYPEDEF__
#define __NVOC_CLASS_ProfilerBase_TYPEDEF__
typedef struct ProfilerBase ProfilerBase;
#endif /* __NVOC_CLASS_ProfilerBase_TYPEDEF__ */

#ifndef __nvoc_class_id_ProfilerBase
#define __nvoc_class_id_ProfilerBase 0x4976fc
#endif /* __nvoc_class_id_ProfilerBase */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_ProfilerBase;

#define __staticCast_ProfilerBase(pThis) \
    ((pThis)->__nvoc_pbase_ProfilerBase)

#ifdef __nvoc_profiler_v2_h_disabled
#define __dynamicCast_ProfilerBase(pThis) ((ProfilerBase*)NULL)
#else //__nvoc_profiler_v2_h_disabled
#define __dynamicCast_ProfilerBase(pThis) \
    ((ProfilerBase*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(ProfilerBase)))
#endif //__nvoc_profiler_v2_h_disabled


NV_STATUS __nvoc_objCreateDynamic_ProfilerBase(ProfilerBase**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_ProfilerBase(ProfilerBase**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_ProfilerBase(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_ProfilerBase((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define profilerBaseCtrlCmdReserveHwpmLegacy(pProfiler, pParams) profilerBaseCtrlCmdReserveHwpmLegacy_DISPATCH(pProfiler, pParams)
#define profilerBaseCtrlCmdInternalReserveHwpmLegacy(pProfiler, pParams) profilerBaseCtrlCmdInternalReserveHwpmLegacy_DISPATCH(pProfiler, pParams)
#define profilerBaseCtrlCmdReleaseHwpmLegacy(pProfiler) profilerBaseCtrlCmdReleaseHwpmLegacy_DISPATCH(pProfiler)
#define profilerBaseCtrlCmdReservePmAreaSmpc(pProfiler, pParams) profilerBaseCtrlCmdReservePmAreaSmpc_DISPATCH(pProfiler, pParams)
#define profilerBaseCtrlCmdReleasePmAreaSmpc(pProfiler) profilerBaseCtrlCmdReleasePmAreaSmpc_DISPATCH(pProfiler)
#define profilerBaseCtrlCmdAllocPmaStream(pProfiler, pParams) profilerBaseCtrlCmdAllocPmaStream_DISPATCH(pProfiler, pParams)
#define profilerBaseCtrlCmdFreePmaStream(pProfiler, pParams) profilerBaseCtrlCmdFreePmaStream_DISPATCH(pProfiler, pParams)
#define profilerBaseCtrlCmdInternalFreePmaStream(pProfiler, pParams) profilerBaseCtrlCmdInternalFreePmaStream_DISPATCH(pProfiler, pParams)
#define profilerBaseCtrlCmdInternalGetMaxPmas(pProfiler, pParams) profilerBaseCtrlCmdInternalGetMaxPmas_DISPATCH(pProfiler, pParams)
#define profilerBaseCtrlCmdBindPmResources(pProfiler) profilerBaseCtrlCmdBindPmResources_DISPATCH(pProfiler)
#define profilerBaseCtrlCmdUnbindPmResources(pProfiler) profilerBaseCtrlCmdUnbindPmResources_DISPATCH(pProfiler)
#define profilerBaseCtrlCmdInternalBindPmResources(pProfiler) profilerBaseCtrlCmdInternalBindPmResources_DISPATCH(pProfiler)
#define profilerBaseCtrlCmdInternalUnbindPmResources(pProfiler) profilerBaseCtrlCmdInternalUnbindPmResources_DISPATCH(pProfiler)
#define profilerBaseCtrlCmdPmaStreamUpdateGetPut(pProfiler, pParams) profilerBaseCtrlCmdPmaStreamUpdateGetPut_DISPATCH(pProfiler, pParams)
#define profilerBaseCtrlCmdExecRegops(pProfiler, pParams) profilerBaseCtrlCmdExecRegops_DISPATCH(pProfiler, pParams)
#define profilerBaseCtrlCmdInternalAllocPmaStream(pProfiler, pParams) profilerBaseCtrlCmdInternalAllocPmaStream_DISPATCH(pProfiler, pParams)
#define profilerBaseCtrlCmdInternalPermissionsInit(pProfiler, pParams) profilerBaseCtrlCmdInternalPermissionsInit_DISPATCH(pProfiler, pParams)
#define profilerBaseCtrlCmdReservePmAreaPcSampler(pProfiler) profilerBaseCtrlCmdReservePmAreaPcSampler_DISPATCH(pProfiler)
#define profilerBaseCtrlCmdReleasePmAreaPcSampler(pProfiler) profilerBaseCtrlCmdReleasePmAreaPcSampler_DISPATCH(pProfiler)
#define profilerBaseCtrlCmdGetTotalHsCredits(pProfiler, pParams) profilerBaseCtrlCmdGetTotalHsCredits_DISPATCH(pProfiler, pParams)
#define profilerBaseCtrlCmdGetHsCredits(pProfiler, pParams) profilerBaseCtrlCmdGetHsCredits_DISPATCH(pProfiler, pParams)
#define profilerBaseCtrlCmdSetHsCredits(pProfiler, pParams) profilerBaseCtrlCmdSetHsCredits_DISPATCH(pProfiler, pParams)
#define profilerBaseCtrlCmdReserveHes(pProfiler, pParams) profilerBaseCtrlCmdReserveHes_DISPATCH(pProfiler, pParams)
#define profilerBaseCtrlCmdReleaseHes(pProfiler, pParams) profilerBaseCtrlCmdReleaseHes_DISPATCH(pProfiler, pParams)
#define profilerBaseCtrlCmdRequestCgControls(pProfiler, pParams) profilerBaseCtrlCmdRequestCgControls_DISPATCH(pProfiler, pParams)
#define profilerBaseCtrlCmdReleaseCgControls(pProfiler, pParams) profilerBaseCtrlCmdReleaseCgControls_DISPATCH(pProfiler, pParams)
#define profilerBaseCtrlCmdDisableDynamicMMABoost(pProfiler, pParams) profilerBaseCtrlCmdDisableDynamicMMABoost_DISPATCH(pProfiler, pParams)
#define profilerBaseCtrlCmdGetDynamicMMABoostStatus(pProfiler, pParams) profilerBaseCtrlCmdGetDynamicMMABoostStatus_DISPATCH(pProfiler, pParams)
#define profilerBaseShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) profilerBaseShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define profilerBaseCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) profilerBaseCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define profilerBaseMapTo(pResource, pParams) profilerBaseMapTo_DISPATCH(pResource, pParams)
#define profilerBaseGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) profilerBaseGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define profilerBaseGetRefCount(pResource) profilerBaseGetRefCount_DISPATCH(pResource)
#define profilerBaseAddAdditionalDependants(pClient, pResource, pReference) profilerBaseAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define profilerBaseControl_Prologue(pResource, pCallContext, pParams) profilerBaseControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define profilerBaseGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) profilerBaseGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define profilerBaseInternalControlForward(pGpuResource, command, pParams, size) profilerBaseInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define profilerBaseUnmapFrom(pResource, pParams) profilerBaseUnmapFrom_DISPATCH(pResource, pParams)
#define profilerBaseControl_Epilogue(pResource, pCallContext, pParams) profilerBaseControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define profilerBaseControlLookup(pResource, pParams, ppEntry) profilerBaseControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define profilerBaseGetInternalObjectHandle(pGpuResource) profilerBaseGetInternalObjectHandle_DISPATCH(pGpuResource)
#define profilerBaseControl(pGpuResource, pCallContext, pParams) profilerBaseControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define profilerBaseUnmap(pGpuResource, pCallContext, pCpuMapping) profilerBaseUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define profilerBaseGetMemInterMapParams(pRmResource, pParams) profilerBaseGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define profilerBaseGetMemoryMappingDescriptor(pRmResource, ppMemDesc) profilerBaseGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define profilerBaseControlFilter(pResource, pCallContext, pParams) profilerBaseControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define profilerBaseControlSerialization_Prologue(pResource, pCallContext, pParams) profilerBaseControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define profilerBaseCanCopy(pResource) profilerBaseCanCopy_DISPATCH(pResource)
#define profilerBasePreDestruct(pResource) profilerBasePreDestruct_DISPATCH(pResource)
#define profilerBaseIsDuplicate(pResource, hMemory, pDuplicate) profilerBaseIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define profilerBaseControlSerialization_Epilogue(pResource, pCallContext, pParams) profilerBaseControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define profilerBaseMap(pGpuResource, pCallContext, pParams, pCpuMapping) profilerBaseMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define profilerBaseAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) profilerBaseAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
static inline NV_STATUS profilerBaseConstructState_56cd7a(struct ProfilerBase *pProf, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {
    return NV_OK;
}


#ifdef __nvoc_profiler_v2_h_disabled
static inline NV_STATUS profilerBaseConstructState(struct ProfilerBase *pProf, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {
    NV_ASSERT_FAILED_PRECOMP("ProfilerBase was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_profiler_v2_h_disabled
#define profilerBaseConstructState(pProf, pCallContext, pParams) profilerBaseConstructState_56cd7a(pProf, pCallContext, pParams)
#endif //__nvoc_profiler_v2_h_disabled

#define profilerBaseConstructState_HAL(pProf, pCallContext, pParams) profilerBaseConstructState(pProf, pCallContext, pParams)

static inline void profilerBaseDestructState_b3696a(struct ProfilerBase *pProf) {
    return;
}


#ifdef __nvoc_profiler_v2_h_disabled
static inline void profilerBaseDestructState(struct ProfilerBase *pProf) {
    NV_ASSERT_FAILED_PRECOMP("ProfilerBase was disabled!");
}
#else //__nvoc_profiler_v2_h_disabled
#define profilerBaseDestructState(pProf) profilerBaseDestructState_b3696a(pProf)
#endif //__nvoc_profiler_v2_h_disabled

#define profilerBaseDestructState_HAL(pProf) profilerBaseDestructState(pProf)

NV_STATUS profilerBaseCtrlCmdReserveHwpmLegacy_IMPL(struct ProfilerBase *pProfiler, NVB0CC_CTRL_RESERVE_HWPM_LEGACY_PARAMS *pParams);

static inline NV_STATUS profilerBaseCtrlCmdReserveHwpmLegacy_DISPATCH(struct ProfilerBase *pProfiler, NVB0CC_CTRL_RESERVE_HWPM_LEGACY_PARAMS *pParams) {
    return pProfiler->__profilerBaseCtrlCmdReserveHwpmLegacy__(pProfiler, pParams);
}

NV_STATUS profilerBaseCtrlCmdInternalReserveHwpmLegacy_IMPL(struct ProfilerBase *pProfiler, NVB0CC_CTRL_INTERNAL_RESERVE_HWPM_LEGACY_PARAMS *pParams);

static inline NV_STATUS profilerBaseCtrlCmdInternalReserveHwpmLegacy_DISPATCH(struct ProfilerBase *pProfiler, NVB0CC_CTRL_INTERNAL_RESERVE_HWPM_LEGACY_PARAMS *pParams) {
    return pProfiler->__profilerBaseCtrlCmdInternalReserveHwpmLegacy__(pProfiler, pParams);
}

NV_STATUS profilerBaseCtrlCmdReleaseHwpmLegacy_IMPL(struct ProfilerBase *pProfiler);

static inline NV_STATUS profilerBaseCtrlCmdReleaseHwpmLegacy_DISPATCH(struct ProfilerBase *pProfiler) {
    return pProfiler->__profilerBaseCtrlCmdReleaseHwpmLegacy__(pProfiler);
}

NV_STATUS profilerBaseCtrlCmdReservePmAreaSmpc_IMPL(struct ProfilerBase *pProfiler, NVB0CC_CTRL_RESERVE_PM_AREA_SMPC_PARAMS *pParams);

static inline NV_STATUS profilerBaseCtrlCmdReservePmAreaSmpc_DISPATCH(struct ProfilerBase *pProfiler, NVB0CC_CTRL_RESERVE_PM_AREA_SMPC_PARAMS *pParams) {
    return pProfiler->__profilerBaseCtrlCmdReservePmAreaSmpc__(pProfiler, pParams);
}

NV_STATUS profilerBaseCtrlCmdReleasePmAreaSmpc_IMPL(struct ProfilerBase *pProfiler);

static inline NV_STATUS profilerBaseCtrlCmdReleasePmAreaSmpc_DISPATCH(struct ProfilerBase *pProfiler) {
    return pProfiler->__profilerBaseCtrlCmdReleasePmAreaSmpc__(pProfiler);
}

NV_STATUS profilerBaseCtrlCmdAllocPmaStream_IMPL(struct ProfilerBase *pProfiler, NVB0CC_CTRL_ALLOC_PMA_STREAM_PARAMS *pParams);

static inline NV_STATUS profilerBaseCtrlCmdAllocPmaStream_DISPATCH(struct ProfilerBase *pProfiler, NVB0CC_CTRL_ALLOC_PMA_STREAM_PARAMS *pParams) {
    return pProfiler->__profilerBaseCtrlCmdAllocPmaStream__(pProfiler, pParams);
}

NV_STATUS profilerBaseCtrlCmdFreePmaStream_IMPL(struct ProfilerBase *pProfiler, NVB0CC_CTRL_FREE_PMA_STREAM_PARAMS *pParams);

static inline NV_STATUS profilerBaseCtrlCmdFreePmaStream_DISPATCH(struct ProfilerBase *pProfiler, NVB0CC_CTRL_FREE_PMA_STREAM_PARAMS *pParams) {
    return pProfiler->__profilerBaseCtrlCmdFreePmaStream__(pProfiler, pParams);
}

NV_STATUS profilerBaseCtrlCmdInternalFreePmaStream_IMPL(struct ProfilerBase *pProfiler, NVB0CC_CTRL_INTERNAL_FREE_PMA_STREAM_PARAMS *pParams);

static inline NV_STATUS profilerBaseCtrlCmdInternalFreePmaStream_DISPATCH(struct ProfilerBase *pProfiler, NVB0CC_CTRL_INTERNAL_FREE_PMA_STREAM_PARAMS *pParams) {
    return pProfiler->__profilerBaseCtrlCmdInternalFreePmaStream__(pProfiler, pParams);
}

NV_STATUS profilerBaseCtrlCmdInternalGetMaxPmas_IMPL(struct ProfilerBase *pProfiler, NVB0CC_CTRL_INTERNAL_GET_MAX_PMAS_PARAMS *pParams);

static inline NV_STATUS profilerBaseCtrlCmdInternalGetMaxPmas_DISPATCH(struct ProfilerBase *pProfiler, NVB0CC_CTRL_INTERNAL_GET_MAX_PMAS_PARAMS *pParams) {
    return pProfiler->__profilerBaseCtrlCmdInternalGetMaxPmas__(pProfiler, pParams);
}

NV_STATUS profilerBaseCtrlCmdBindPmResources_IMPL(struct ProfilerBase *pProfiler);

static inline NV_STATUS profilerBaseCtrlCmdBindPmResources_DISPATCH(struct ProfilerBase *pProfiler) {
    return pProfiler->__profilerBaseCtrlCmdBindPmResources__(pProfiler);
}

NV_STATUS profilerBaseCtrlCmdUnbindPmResources_IMPL(struct ProfilerBase *pProfiler);

static inline NV_STATUS profilerBaseCtrlCmdUnbindPmResources_DISPATCH(struct ProfilerBase *pProfiler) {
    return pProfiler->__profilerBaseCtrlCmdUnbindPmResources__(pProfiler);
}

NV_STATUS profilerBaseCtrlCmdInternalBindPmResources_IMPL(struct ProfilerBase *pProfiler);

static inline NV_STATUS profilerBaseCtrlCmdInternalBindPmResources_DISPATCH(struct ProfilerBase *pProfiler) {
    return pProfiler->__profilerBaseCtrlCmdInternalBindPmResources__(pProfiler);
}

NV_STATUS profilerBaseCtrlCmdInternalUnbindPmResources_IMPL(struct ProfilerBase *pProfiler);

static inline NV_STATUS profilerBaseCtrlCmdInternalUnbindPmResources_DISPATCH(struct ProfilerBase *pProfiler) {
    return pProfiler->__profilerBaseCtrlCmdInternalUnbindPmResources__(pProfiler);
}

NV_STATUS profilerBaseCtrlCmdPmaStreamUpdateGetPut_IMPL(struct ProfilerBase *pProfiler, NVB0CC_CTRL_PMA_STREAM_UPDATE_GET_PUT_PARAMS *pParams);

static inline NV_STATUS profilerBaseCtrlCmdPmaStreamUpdateGetPut_DISPATCH(struct ProfilerBase *pProfiler, NVB0CC_CTRL_PMA_STREAM_UPDATE_GET_PUT_PARAMS *pParams) {
    return pProfiler->__profilerBaseCtrlCmdPmaStreamUpdateGetPut__(pProfiler, pParams);
}

NV_STATUS profilerBaseCtrlCmdExecRegops_IMPL(struct ProfilerBase *pProfiler, NVB0CC_CTRL_EXEC_REG_OPS_PARAMS *pParams);

static inline NV_STATUS profilerBaseCtrlCmdExecRegops_DISPATCH(struct ProfilerBase *pProfiler, NVB0CC_CTRL_EXEC_REG_OPS_PARAMS *pParams) {
    return pProfiler->__profilerBaseCtrlCmdExecRegops__(pProfiler, pParams);
}

NV_STATUS profilerBaseCtrlCmdInternalAllocPmaStream_IMPL(struct ProfilerBase *pProfiler, NVB0CC_CTRL_INTERNAL_ALLOC_PMA_STREAM_PARAMS *pParams);

static inline NV_STATUS profilerBaseCtrlCmdInternalAllocPmaStream_DISPATCH(struct ProfilerBase *pProfiler, NVB0CC_CTRL_INTERNAL_ALLOC_PMA_STREAM_PARAMS *pParams) {
    return pProfiler->__profilerBaseCtrlCmdInternalAllocPmaStream__(pProfiler, pParams);
}

NV_STATUS profilerBaseCtrlCmdInternalPermissionsInit_IMPL(struct ProfilerBase *pProfiler, NVB0CC_CTRL_INTERNAL_PERMISSIONS_INIT_PARAMS *pParams);

static inline NV_STATUS profilerBaseCtrlCmdInternalPermissionsInit_DISPATCH(struct ProfilerBase *pProfiler, NVB0CC_CTRL_INTERNAL_PERMISSIONS_INIT_PARAMS *pParams) {
    return pProfiler->__profilerBaseCtrlCmdInternalPermissionsInit__(pProfiler, pParams);
}

NV_STATUS profilerBaseCtrlCmdReservePmAreaPcSampler_IMPL(struct ProfilerBase *pProfiler);

static inline NV_STATUS profilerBaseCtrlCmdReservePmAreaPcSampler_DISPATCH(struct ProfilerBase *pProfiler) {
    return pProfiler->__profilerBaseCtrlCmdReservePmAreaPcSampler__(pProfiler);
}

NV_STATUS profilerBaseCtrlCmdReleasePmAreaPcSampler_IMPL(struct ProfilerBase *pProfiler);

static inline NV_STATUS profilerBaseCtrlCmdReleasePmAreaPcSampler_DISPATCH(struct ProfilerBase *pProfiler) {
    return pProfiler->__profilerBaseCtrlCmdReleasePmAreaPcSampler__(pProfiler);
}

NV_STATUS profilerBaseCtrlCmdGetTotalHsCredits_IMPL(struct ProfilerBase *pProfiler, NVB0CC_CTRL_GET_TOTAL_HS_CREDITS_PARAMS *pParams);

static inline NV_STATUS profilerBaseCtrlCmdGetTotalHsCredits_DISPATCH(struct ProfilerBase *pProfiler, NVB0CC_CTRL_GET_TOTAL_HS_CREDITS_PARAMS *pParams) {
    return pProfiler->__profilerBaseCtrlCmdGetTotalHsCredits__(pProfiler, pParams);
}

NV_STATUS profilerBaseCtrlCmdGetHsCredits_IMPL(struct ProfilerBase *pProfiler, NVB0CC_CTRL_GET_HS_CREDITS_PARAMS *pParams);

static inline NV_STATUS profilerBaseCtrlCmdGetHsCredits_DISPATCH(struct ProfilerBase *pProfiler, NVB0CC_CTRL_GET_HS_CREDITS_PARAMS *pParams) {
    return pProfiler->__profilerBaseCtrlCmdGetHsCredits__(pProfiler, pParams);
}

NV_STATUS profilerBaseCtrlCmdSetHsCredits_IMPL(struct ProfilerBase *pProfiler, NVB0CC_CTRL_SET_HS_CREDITS_PARAMS *pParams);

static inline NV_STATUS profilerBaseCtrlCmdSetHsCredits_DISPATCH(struct ProfilerBase *pProfiler, NVB0CC_CTRL_SET_HS_CREDITS_PARAMS *pParams) {
    return pProfiler->__profilerBaseCtrlCmdSetHsCredits__(pProfiler, pParams);
}

NV_STATUS profilerBaseCtrlCmdReserveHes_IMPL(struct ProfilerBase *pProfiler, NVB0CC_CTRL_RESERVE_HES_PARAMS *pParams);

static inline NV_STATUS profilerBaseCtrlCmdReserveHes_DISPATCH(struct ProfilerBase *pProfiler, NVB0CC_CTRL_RESERVE_HES_PARAMS *pParams) {
    return pProfiler->__profilerBaseCtrlCmdReserveHes__(pProfiler, pParams);
}

NV_STATUS profilerBaseCtrlCmdReleaseHes_IMPL(struct ProfilerBase *pProfiler, NVB0CC_CTRL_RELEASE_HES_PARAMS *pParams);

static inline NV_STATUS profilerBaseCtrlCmdReleaseHes_DISPATCH(struct ProfilerBase *pProfiler, NVB0CC_CTRL_RELEASE_HES_PARAMS *pParams) {
    return pProfiler->__profilerBaseCtrlCmdReleaseHes__(pProfiler, pParams);
}

NV_STATUS profilerBaseCtrlCmdRequestCgControls_IMPL(struct ProfilerBase *pProfiler, NVB0CC_CTRL_POWER_REQUEST_FEATURES_PARAMS *pParams);

static inline NV_STATUS profilerBaseCtrlCmdRequestCgControls_DISPATCH(struct ProfilerBase *pProfiler, NVB0CC_CTRL_POWER_REQUEST_FEATURES_PARAMS *pParams) {
    return pProfiler->__profilerBaseCtrlCmdRequestCgControls__(pProfiler, pParams);
}

NV_STATUS profilerBaseCtrlCmdReleaseCgControls_IMPL(struct ProfilerBase *pProfiler, NVB0CC_CTRL_POWER_RELEASE_FEATURES_PARAMS *pParams);

static inline NV_STATUS profilerBaseCtrlCmdReleaseCgControls_DISPATCH(struct ProfilerBase *pProfiler, NVB0CC_CTRL_POWER_RELEASE_FEATURES_PARAMS *pParams) {
    return pProfiler->__profilerBaseCtrlCmdReleaseCgControls__(pProfiler, pParams);
}

NV_STATUS profilerBaseCtrlCmdDisableDynamicMMABoost_IMPL(struct ProfilerBase *pProfiler, NVB0CC_CTRL_DISABLE_DYNAMIC_MMA_BOOST_PARAMS *pParams);

static inline NV_STATUS profilerBaseCtrlCmdDisableDynamicMMABoost_DISPATCH(struct ProfilerBase *pProfiler, NVB0CC_CTRL_DISABLE_DYNAMIC_MMA_BOOST_PARAMS *pParams) {
    return pProfiler->__profilerBaseCtrlCmdDisableDynamicMMABoost__(pProfiler, pParams);
}

NV_STATUS profilerBaseCtrlCmdGetDynamicMMABoostStatus_IMPL(struct ProfilerBase *pProfiler, NVB0CC_CTRL_GET_DYNAMIC_MMA_BOOST_STATUS_PARAMS *pParams);

static inline NV_STATUS profilerBaseCtrlCmdGetDynamicMMABoostStatus_DISPATCH(struct ProfilerBase *pProfiler, NVB0CC_CTRL_GET_DYNAMIC_MMA_BOOST_STATUS_PARAMS *pParams) {
    return pProfiler->__profilerBaseCtrlCmdGetDynamicMMABoostStatus__(pProfiler, pParams);
}

static inline NvBool profilerBaseShareCallback_DISPATCH(struct ProfilerBase *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__profilerBaseShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS profilerBaseCheckMemInterUnmap_DISPATCH(struct ProfilerBase *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__profilerBaseCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS profilerBaseMapTo_DISPATCH(struct ProfilerBase *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__profilerBaseMapTo__(pResource, pParams);
}

static inline NV_STATUS profilerBaseGetMapAddrSpace_DISPATCH(struct ProfilerBase *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__profilerBaseGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NvU32 profilerBaseGetRefCount_DISPATCH(struct ProfilerBase *pResource) {
    return pResource->__profilerBaseGetRefCount__(pResource);
}

static inline void profilerBaseAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct ProfilerBase *pResource, RsResourceRef *pReference) {
    pResource->__profilerBaseAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS profilerBaseControl_Prologue_DISPATCH(struct ProfilerBase *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__profilerBaseControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS profilerBaseGetRegBaseOffsetAndSize_DISPATCH(struct ProfilerBase *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__profilerBaseGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS profilerBaseInternalControlForward_DISPATCH(struct ProfilerBase *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__profilerBaseInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NV_STATUS profilerBaseUnmapFrom_DISPATCH(struct ProfilerBase *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__profilerBaseUnmapFrom__(pResource, pParams);
}

static inline void profilerBaseControl_Epilogue_DISPATCH(struct ProfilerBase *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__profilerBaseControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS profilerBaseControlLookup_DISPATCH(struct ProfilerBase *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__profilerBaseControlLookup__(pResource, pParams, ppEntry);
}

static inline NvHandle profilerBaseGetInternalObjectHandle_DISPATCH(struct ProfilerBase *pGpuResource) {
    return pGpuResource->__profilerBaseGetInternalObjectHandle__(pGpuResource);
}

static inline NV_STATUS profilerBaseControl_DISPATCH(struct ProfilerBase *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__profilerBaseControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS profilerBaseUnmap_DISPATCH(struct ProfilerBase *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__profilerBaseUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NV_STATUS profilerBaseGetMemInterMapParams_DISPATCH(struct ProfilerBase *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__profilerBaseGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS profilerBaseGetMemoryMappingDescriptor_DISPATCH(struct ProfilerBase *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__profilerBaseGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS profilerBaseControlFilter_DISPATCH(struct ProfilerBase *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__profilerBaseControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS profilerBaseControlSerialization_Prologue_DISPATCH(struct ProfilerBase *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__profilerBaseControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool profilerBaseCanCopy_DISPATCH(struct ProfilerBase *pResource) {
    return pResource->__profilerBaseCanCopy__(pResource);
}

static inline void profilerBasePreDestruct_DISPATCH(struct ProfilerBase *pResource) {
    pResource->__profilerBasePreDestruct__(pResource);
}

static inline NV_STATUS profilerBaseIsDuplicate_DISPATCH(struct ProfilerBase *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__profilerBaseIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void profilerBaseControlSerialization_Epilogue_DISPATCH(struct ProfilerBase *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__profilerBaseControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS profilerBaseMap_DISPATCH(struct ProfilerBase *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__profilerBaseMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NvBool profilerBaseAccessCallback_DISPATCH(struct ProfilerBase *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__profilerBaseAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

NV_STATUS profilerBaseConstruct_IMPL(struct ProfilerBase *arg_pProf, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_profilerBaseConstruct(arg_pProf, arg_pCallContext, arg_pParams) profilerBaseConstruct_IMPL(arg_pProf, arg_pCallContext, arg_pParams)
void profilerBaseDestruct_IMPL(struct ProfilerBase *pProf);

#define __nvoc_profilerBaseDestruct(pProf) profilerBaseDestruct_IMPL(pProf)
#undef PRIVATE_FIELD


#ifdef NVOC_PROFILER_V2_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct ProfilerDev {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct ProfilerBase __nvoc_base_ProfilerBase;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct GpuResource *__nvoc_pbase_GpuResource;
    struct ProfilerBase *__nvoc_pbase_ProfilerBase;
    struct ProfilerDev *__nvoc_pbase_ProfilerDev;
    NvBool (*__profilerDevShareCallback__)(struct ProfilerDev *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__profilerDevCheckMemInterUnmap__)(struct ProfilerDev *, NvBool);
    NV_STATUS (*__profilerDevMapTo__)(struct ProfilerDev *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__profilerDevGetMapAddrSpace__)(struct ProfilerDev *, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    NvU32 (*__profilerDevGetRefCount__)(struct ProfilerDev *);
    void (*__profilerDevAddAdditionalDependants__)(struct RsClient *, struct ProfilerDev *, RsResourceRef *);
    NV_STATUS (*__profilerDevControl_Prologue__)(struct ProfilerDev *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__profilerDevGetRegBaseOffsetAndSize__)(struct ProfilerDev *, struct OBJGPU *, NvU32 *, NvU32 *);
    NV_STATUS (*__profilerDevInternalControlForward__)(struct ProfilerDev *, NvU32, void *, NvU32);
    NV_STATUS (*__profilerDevUnmapFrom__)(struct ProfilerDev *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__profilerDevControl_Epilogue__)(struct ProfilerDev *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__profilerDevControlLookup__)(struct ProfilerDev *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NvHandle (*__profilerDevGetInternalObjectHandle__)(struct ProfilerDev *);
    NV_STATUS (*__profilerDevControl__)(struct ProfilerDev *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__profilerDevUnmap__)(struct ProfilerDev *, struct CALL_CONTEXT *, struct RsCpuMapping *);
    NV_STATUS (*__profilerDevGetMemInterMapParams__)(struct ProfilerDev *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__profilerDevGetMemoryMappingDescriptor__)(struct ProfilerDev *, struct MEMORY_DESCRIPTOR **);
    NV_STATUS (*__profilerDevControlFilter__)(struct ProfilerDev *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__profilerDevControlSerialization_Prologue__)(struct ProfilerDev *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__profilerDevCanCopy__)(struct ProfilerDev *);
    void (*__profilerDevPreDestruct__)(struct ProfilerDev *);
    NV_STATUS (*__profilerDevIsDuplicate__)(struct ProfilerDev *, NvHandle, NvBool *);
    void (*__profilerDevControlSerialization_Epilogue__)(struct ProfilerDev *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__profilerDevMap__)(struct ProfilerDev *, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);
    NvBool (*__profilerDevAccessCallback__)(struct ProfilerDev *, struct RsClient *, void *, RsAccessRight);
};

#ifndef __NVOC_CLASS_ProfilerDev_TYPEDEF__
#define __NVOC_CLASS_ProfilerDev_TYPEDEF__
typedef struct ProfilerDev ProfilerDev;
#endif /* __NVOC_CLASS_ProfilerDev_TYPEDEF__ */

#ifndef __nvoc_class_id_ProfilerDev
#define __nvoc_class_id_ProfilerDev 0x54d077
#endif /* __nvoc_class_id_ProfilerDev */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_ProfilerDev;

#define __staticCast_ProfilerDev(pThis) \
    ((pThis)->__nvoc_pbase_ProfilerDev)

#ifdef __nvoc_profiler_v2_h_disabled
#define __dynamicCast_ProfilerDev(pThis) ((ProfilerDev*)NULL)
#else //__nvoc_profiler_v2_h_disabled
#define __dynamicCast_ProfilerDev(pThis) \
    ((ProfilerDev*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(ProfilerDev)))
#endif //__nvoc_profiler_v2_h_disabled


NV_STATUS __nvoc_objCreateDynamic_ProfilerDev(ProfilerDev**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_ProfilerDev(ProfilerDev**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_ProfilerDev(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_ProfilerDev((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define profilerDevShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) profilerDevShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define profilerDevCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) profilerDevCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define profilerDevMapTo(pResource, pParams) profilerDevMapTo_DISPATCH(pResource, pParams)
#define profilerDevGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) profilerDevGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define profilerDevGetRefCount(pResource) profilerDevGetRefCount_DISPATCH(pResource)
#define profilerDevAddAdditionalDependants(pClient, pResource, pReference) profilerDevAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define profilerDevControl_Prologue(pResource, pCallContext, pParams) profilerDevControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define profilerDevGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) profilerDevGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define profilerDevInternalControlForward(pGpuResource, command, pParams, size) profilerDevInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define profilerDevUnmapFrom(pResource, pParams) profilerDevUnmapFrom_DISPATCH(pResource, pParams)
#define profilerDevControl_Epilogue(pResource, pCallContext, pParams) profilerDevControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define profilerDevControlLookup(pResource, pParams, ppEntry) profilerDevControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define profilerDevGetInternalObjectHandle(pGpuResource) profilerDevGetInternalObjectHandle_DISPATCH(pGpuResource)
#define profilerDevControl(pGpuResource, pCallContext, pParams) profilerDevControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define profilerDevUnmap(pGpuResource, pCallContext, pCpuMapping) profilerDevUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define profilerDevGetMemInterMapParams(pRmResource, pParams) profilerDevGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define profilerDevGetMemoryMappingDescriptor(pRmResource, ppMemDesc) profilerDevGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define profilerDevControlFilter(pResource, pCallContext, pParams) profilerDevControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define profilerDevControlSerialization_Prologue(pResource, pCallContext, pParams) profilerDevControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define profilerDevCanCopy(pResource) profilerDevCanCopy_DISPATCH(pResource)
#define profilerDevPreDestruct(pResource) profilerDevPreDestruct_DISPATCH(pResource)
#define profilerDevIsDuplicate(pResource, hMemory, pDuplicate) profilerDevIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define profilerDevControlSerialization_Epilogue(pResource, pCallContext, pParams) profilerDevControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define profilerDevMap(pGpuResource, pCallContext, pParams, pCpuMapping) profilerDevMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define profilerDevAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) profilerDevAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
NV_STATUS profilerDevConstructState_IMPL(struct ProfilerDev *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams, PROFILER_CLIENT_PERMISSIONS clientPermissions);


#ifdef __nvoc_profiler_v2_h_disabled
static inline NV_STATUS profilerDevConstructState(struct ProfilerDev *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams, PROFILER_CLIENT_PERMISSIONS clientPermissions) {
    NV_ASSERT_FAILED_PRECOMP("ProfilerDev was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_profiler_v2_h_disabled
#define profilerDevConstructState(pResource, pCallContext, pParams, clientPermissions) profilerDevConstructState_IMPL(pResource, pCallContext, pParams, clientPermissions)
#endif //__nvoc_profiler_v2_h_disabled

#define profilerDevConstructState_HAL(pResource, pCallContext, pParams, clientPermissions) profilerDevConstructState(pResource, pCallContext, pParams, clientPermissions)

NV_STATUS profilerDevConstructStatePrologue_FWCLIENT(struct ProfilerDev *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);


#ifdef __nvoc_profiler_v2_h_disabled
static inline NV_STATUS profilerDevConstructStatePrologue(struct ProfilerDev *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {
    NV_ASSERT_FAILED_PRECOMP("ProfilerDev was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_profiler_v2_h_disabled
#define profilerDevConstructStatePrologue(pResource, pCallContext, pParams) profilerDevConstructStatePrologue_FWCLIENT(pResource, pCallContext, pParams)
#endif //__nvoc_profiler_v2_h_disabled

#define profilerDevConstructStatePrologue_HAL(pResource, pCallContext, pParams) profilerDevConstructStatePrologue(pResource, pCallContext, pParams)

NV_STATUS profilerDevConstructStateInterlude_IMPL(struct ProfilerDev *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams, PROFILER_CLIENT_PERMISSIONS clientPermissions);


#ifdef __nvoc_profiler_v2_h_disabled
static inline NV_STATUS profilerDevConstructStateInterlude(struct ProfilerDev *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams, PROFILER_CLIENT_PERMISSIONS clientPermissions) {
    NV_ASSERT_FAILED_PRECOMP("ProfilerDev was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_profiler_v2_h_disabled
#define profilerDevConstructStateInterlude(pResource, pCallContext, pParams, clientPermissions) profilerDevConstructStateInterlude_IMPL(pResource, pCallContext, pParams, clientPermissions)
#endif //__nvoc_profiler_v2_h_disabled

#define profilerDevConstructStateInterlude_HAL(pResource, pCallContext, pParams, clientPermissions) profilerDevConstructStateInterlude(pResource, pCallContext, pParams, clientPermissions)

static inline NV_STATUS profilerDevConstructStateEpilogue_56cd7a(struct ProfilerDev *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {
    return NV_OK;
}


#ifdef __nvoc_profiler_v2_h_disabled
static inline NV_STATUS profilerDevConstructStateEpilogue(struct ProfilerDev *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {
    NV_ASSERT_FAILED_PRECOMP("ProfilerDev was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_profiler_v2_h_disabled
#define profilerDevConstructStateEpilogue(pResource, pCallContext, pParams) profilerDevConstructStateEpilogue_56cd7a(pResource, pCallContext, pParams)
#endif //__nvoc_profiler_v2_h_disabled

#define profilerDevConstructStateEpilogue_HAL(pResource, pCallContext, pParams) profilerDevConstructStateEpilogue(pResource, pCallContext, pParams)

NvBool profilerDevQueryCapabilities_IMPL(struct ProfilerDev *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams, PROFILER_CLIENT_PERMISSIONS *pClientPermissions);


#ifdef __nvoc_profiler_v2_h_disabled
static inline NvBool profilerDevQueryCapabilities(struct ProfilerDev *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams, PROFILER_CLIENT_PERMISSIONS *pClientPermissions) {
    NV_ASSERT_FAILED_PRECOMP("ProfilerDev was disabled!");
    return NV_FALSE;
}
#else //__nvoc_profiler_v2_h_disabled
#define profilerDevQueryCapabilities(pResource, pCallContext, pParams, pClientPermissions) profilerDevQueryCapabilities_IMPL(pResource, pCallContext, pParams, pClientPermissions)
#endif //__nvoc_profiler_v2_h_disabled

#define profilerDevQueryCapabilities_HAL(pResource, pCallContext, pParams, pClientPermissions) profilerDevQueryCapabilities(pResource, pCallContext, pParams, pClientPermissions)

void profilerDevDestructState_FWCLIENT(struct ProfilerDev *pResource);


#ifdef __nvoc_profiler_v2_h_disabled
static inline void profilerDevDestructState(struct ProfilerDev *pResource) {
    NV_ASSERT_FAILED_PRECOMP("ProfilerDev was disabled!");
}
#else //__nvoc_profiler_v2_h_disabled
#define profilerDevDestructState(pResource) profilerDevDestructState_FWCLIENT(pResource)
#endif //__nvoc_profiler_v2_h_disabled

#define profilerDevDestructState_HAL(pResource) profilerDevDestructState(pResource)

static inline NvBool profilerDevShareCallback_DISPATCH(struct ProfilerDev *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__profilerDevShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS profilerDevCheckMemInterUnmap_DISPATCH(struct ProfilerDev *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__profilerDevCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS profilerDevMapTo_DISPATCH(struct ProfilerDev *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__profilerDevMapTo__(pResource, pParams);
}

static inline NV_STATUS profilerDevGetMapAddrSpace_DISPATCH(struct ProfilerDev *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__profilerDevGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NvU32 profilerDevGetRefCount_DISPATCH(struct ProfilerDev *pResource) {
    return pResource->__profilerDevGetRefCount__(pResource);
}

static inline void profilerDevAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct ProfilerDev *pResource, RsResourceRef *pReference) {
    pResource->__profilerDevAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS profilerDevControl_Prologue_DISPATCH(struct ProfilerDev *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__profilerDevControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS profilerDevGetRegBaseOffsetAndSize_DISPATCH(struct ProfilerDev *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__profilerDevGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS profilerDevInternalControlForward_DISPATCH(struct ProfilerDev *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__profilerDevInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NV_STATUS profilerDevUnmapFrom_DISPATCH(struct ProfilerDev *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__profilerDevUnmapFrom__(pResource, pParams);
}

static inline void profilerDevControl_Epilogue_DISPATCH(struct ProfilerDev *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__profilerDevControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS profilerDevControlLookup_DISPATCH(struct ProfilerDev *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__profilerDevControlLookup__(pResource, pParams, ppEntry);
}

static inline NvHandle profilerDevGetInternalObjectHandle_DISPATCH(struct ProfilerDev *pGpuResource) {
    return pGpuResource->__profilerDevGetInternalObjectHandle__(pGpuResource);
}

static inline NV_STATUS profilerDevControl_DISPATCH(struct ProfilerDev *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__profilerDevControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS profilerDevUnmap_DISPATCH(struct ProfilerDev *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__profilerDevUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NV_STATUS profilerDevGetMemInterMapParams_DISPATCH(struct ProfilerDev *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__profilerDevGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS profilerDevGetMemoryMappingDescriptor_DISPATCH(struct ProfilerDev *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__profilerDevGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS profilerDevControlFilter_DISPATCH(struct ProfilerDev *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__profilerDevControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS profilerDevControlSerialization_Prologue_DISPATCH(struct ProfilerDev *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__profilerDevControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool profilerDevCanCopy_DISPATCH(struct ProfilerDev *pResource) {
    return pResource->__profilerDevCanCopy__(pResource);
}

static inline void profilerDevPreDestruct_DISPATCH(struct ProfilerDev *pResource) {
    pResource->__profilerDevPreDestruct__(pResource);
}

static inline NV_STATUS profilerDevIsDuplicate_DISPATCH(struct ProfilerDev *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__profilerDevIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void profilerDevControlSerialization_Epilogue_DISPATCH(struct ProfilerDev *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__profilerDevControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS profilerDevMap_DISPATCH(struct ProfilerDev *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__profilerDevMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NvBool profilerDevAccessCallback_DISPATCH(struct ProfilerDev *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__profilerDevAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

NV_STATUS profilerDevConstruct_IMPL(struct ProfilerDev *arg_pResource, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_profilerDevConstruct(arg_pResource, arg_pCallContext, arg_pParams) profilerDevConstruct_IMPL(arg_pResource, arg_pCallContext, arg_pParams)
void profilerDevDestruct_IMPL(struct ProfilerDev *pResource);

#define __nvoc_profilerDevDestruct(pResource) profilerDevDestruct_IMPL(pResource)
#undef PRIVATE_FIELD

#endif // PROFILER_V2_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_PROFILER_V2_NVOC_H_
