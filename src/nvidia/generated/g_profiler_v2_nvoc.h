
#ifndef _G_PROFILER_V2_NVOC_H_
#define _G_PROFILER_V2_NVOC_H_
#include "nvoc/runtime.h"

// Version of generated metadata structures
#ifdef NVOC_METADATA_VERSION
#undef NVOC_METADATA_VERSION
#endif
#define NVOC_METADATA_VERSION 0

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/hwpm/kern_hwpm_common_defs.h"

#include "class/clb2cc.h"   // MAXWELL_PROFILER_DEVICE

typedef struct
{
    NvBool bMemoryProfilingPermitted;
    NvBool bAdminProfilingPermitted;
    NvBool bDevProfilingPermitted;
} PROFILER_CLIENT_PERMISSIONS;


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_PROFILER_V2_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct ProfilerBase {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct GpuResource __nvoc_base_GpuResource;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^4
    struct RsResource *__nvoc_pbase_RsResource;    // res super^3
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^3
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super^2
    struct GpuResource *__nvoc_pbase_GpuResource;    // gpures super
    struct ProfilerBase *__nvoc_pbase_ProfilerBase;    // profilerBase

    // Vtable with 59 per-object function pointers
    NV_STATUS (*__profilerBaseConstructState__)(struct ProfilerBase * /*this*/, CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);  // halified (2 hals) body
    void (*__profilerBaseDestructState__)(struct ProfilerBase * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__profilerBaseCtrlCmdReserveHwpmLegacy__)(struct ProfilerBase * /*this*/, NVB0CC_CTRL_RESERVE_HWPM_LEGACY_PARAMS *);  // exported (id=0xb0cc0101)
    NV_STATUS (*__profilerBaseCtrlCmdInternalReserveHwpmLegacy__)(struct ProfilerBase * /*this*/, NVB0CC_CTRL_INTERNAL_RESERVE_HWPM_LEGACY_PARAMS *);  // exported (id=0xb0cc020a)
    NV_STATUS (*__profilerBaseCtrlCmdReleaseHwpmLegacy__)(struct ProfilerBase * /*this*/);  // exported (id=0xb0cc0102)
    NV_STATUS (*__profilerBaseCtrlCmdReservePmAreaSmpc__)(struct ProfilerBase * /*this*/, NVB0CC_CTRL_RESERVE_PM_AREA_SMPC_PARAMS *);  // exported (id=0xb0cc0103)
    NV_STATUS (*__profilerBaseCtrlCmdReleasePmAreaSmpc__)(struct ProfilerBase * /*this*/);  // exported (id=0xb0cc0104)
    NV_STATUS (*__profilerBaseCtrlCmdAllocPmaStream__)(struct ProfilerBase * /*this*/, NVB0CC_CTRL_ALLOC_PMA_STREAM_PARAMS *);  // exported (id=0xb0cc0105)
    NV_STATUS (*__profilerBaseCtrlCmdFreePmaStream__)(struct ProfilerBase * /*this*/, NVB0CC_CTRL_FREE_PMA_STREAM_PARAMS *);  // exported (id=0xb0cc0106)
    NV_STATUS (*__profilerBaseCtrlCmdInternalFreePmaStream__)(struct ProfilerBase * /*this*/, NVB0CC_CTRL_INTERNAL_FREE_PMA_STREAM_PARAMS *);  // halified (2 hals) exported (id=0xb0cc0206) body
    NV_STATUS (*__profilerBaseCtrlCmdInternalGetMaxPmas__)(struct ProfilerBase * /*this*/, NVB0CC_CTRL_INTERNAL_GET_MAX_PMAS_PARAMS *);  // exported (id=0xb0cc0207)
    NV_STATUS (*__profilerBaseCtrlCmdBindPmResources__)(struct ProfilerBase * /*this*/);  // exported (id=0xb0cc0107)
    NV_STATUS (*__profilerBaseCtrlCmdUnbindPmResources__)(struct ProfilerBase * /*this*/);  // halified (2 hals) exported (id=0xb0cc0108) body
    NV_STATUS (*__profilerBaseCtrlCmdInternalBindPmResources__)(struct ProfilerBase * /*this*/);  // exported (id=0xb0cc0208)
    NV_STATUS (*__profilerBaseCtrlCmdInternalUnbindPmResources__)(struct ProfilerBase * /*this*/);  // exported (id=0xb0cc0209)
    NV_STATUS (*__profilerBaseCtrlCmdPmaStreamUpdateGetPut__)(struct ProfilerBase * /*this*/, NVB0CC_CTRL_PMA_STREAM_UPDATE_GET_PUT_PARAMS *);  // halified (2 hals) exported (id=0xb0cc0109) body
    NV_STATUS (*__profilerBaseCtrlCmdExecRegops__)(struct ProfilerBase * /*this*/, NVB0CC_CTRL_EXEC_REG_OPS_PARAMS *);  // exported (id=0xb0cc010a)
    NV_STATUS (*__profilerBaseCtrlCmdInternalAllocPmaStream__)(struct ProfilerBase * /*this*/, NVB0CC_CTRL_INTERNAL_ALLOC_PMA_STREAM_PARAMS *);  // halified (2 hals) exported (id=0xb0cc0204) body
    NV_STATUS (*__profilerBaseCtrlCmdInternalQuiescePmaChannel__)(struct ProfilerBase * /*this*/, NVB0CC_CTRL_INTERNAL_QUIESCE_PMA_CHANNEL_PARAMS *);  // exported (id=0xb0cc0201)
    NV_STATUS (*__profilerBaseCtrlCmdInternalSriovPromotePmaStream__)(struct ProfilerBase * /*this*/, NVB0CC_CTRL_INTERNAL_SRIOV_PROMOTE_PMA_STREAM_PARAMS *);  // halified (2 hals) exported (id=0xb0cc0202) body
    NV_STATUS (*__profilerBaseCtrlCmdInternalPermissionsInit__)(struct ProfilerBase * /*this*/, NVB0CC_CTRL_INTERNAL_PERMISSIONS_INIT_PARAMS *);  // exported (id=0xb0cc0203)
    NV_STATUS (*__profilerBaseCtrlCmdReservePmAreaPcSampler__)(struct ProfilerBase * /*this*/);  // exported (id=0xb0cc010b)
    NV_STATUS (*__profilerBaseCtrlCmdReleasePmAreaPcSampler__)(struct ProfilerBase * /*this*/);  // exported (id=0xb0cc010c)
    NV_STATUS (*__profilerBaseCtrlCmdGetTotalHsCredits__)(struct ProfilerBase * /*this*/, NVB0CC_CTRL_GET_TOTAL_HS_CREDITS_PARAMS *);  // exported (id=0xb0cc010d)
    NV_STATUS (*__profilerBaseCtrlCmdGetChipletHsCredits__)(struct ProfilerBase * /*this*/, NVB0CC_CTRL_GET_CHIPLET_HS_CREDIT_POOL *);  // exported (id=0xb0cc0115)
    NV_STATUS (*__profilerBaseCtrlCmdGetHsCreditsMapping__)(struct ProfilerBase * /*this*/, NVB0CC_CTRL_GET_HS_CREDITS_POOL_MAPPING_PARAMS *);  // exported (id=0xb0cc0116)
    NV_STATUS (*__profilerBaseCtrlCmdGetHsCredits__)(struct ProfilerBase * /*this*/, NVB0CC_CTRL_GET_HS_CREDITS_PARAMS *);  // exported (id=0xb0cc010f)
    NV_STATUS (*__profilerBaseCtrlCmdSetHsCredits__)(struct ProfilerBase * /*this*/, NVB0CC_CTRL_SET_HS_CREDITS_PARAMS *);  // exported (id=0xb0cc010e)
    NV_STATUS (*__profilerBaseCtrlCmdReserveHes__)(struct ProfilerBase * /*this*/, NVB0CC_CTRL_RESERVE_HES_PARAMS *);  // exported (id=0xb0cc0113)
    NV_STATUS (*__profilerBaseCtrlCmdReleaseHes__)(struct ProfilerBase * /*this*/, NVB0CC_CTRL_RELEASE_HES_PARAMS *);  // exported (id=0xb0cc0114)
    NV_STATUS (*__profilerBaseCtrlCmdRequestCgControls__)(struct ProfilerBase * /*this*/, NVB0CC_CTRL_POWER_REQUEST_FEATURES_PARAMS *);  // halified (2 hals) exported (id=0xb0cc0301) body
    NV_STATUS (*__profilerBaseCtrlCmdReleaseCgControls__)(struct ProfilerBase * /*this*/, NVB0CC_CTRL_POWER_RELEASE_FEATURES_PARAMS *);  // halified (2 hals) exported (id=0xb0cc0302) body
    NV_STATUS (*__profilerBaseCtrlCmdDisableDynamicMMABoost__)(struct ProfilerBase * /*this*/, NVB0CC_CTRL_DISABLE_DYNAMIC_MMA_BOOST_PARAMS *);  // exported (id=0xb0cc0117)
    NV_STATUS (*__profilerBaseCtrlCmdGetDynamicMMABoostStatus__)(struct ProfilerBase * /*this*/, NVB0CC_CTRL_GET_DYNAMIC_MMA_BOOST_STATUS_PARAMS *);  // exported (id=0xb0cc0118)
    NV_STATUS (*__profilerBaseControl__)(struct ProfilerBase * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__profilerBaseMap__)(struct ProfilerBase * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__profilerBaseUnmap__)(struct ProfilerBase * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NvBool (*__profilerBaseShareCallback__)(struct ProfilerBase * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__profilerBaseGetRegBaseOffsetAndSize__)(struct ProfilerBase * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__profilerBaseGetMapAddrSpace__)(struct ProfilerBase * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__profilerBaseInternalControlForward__)(struct ProfilerBase * /*this*/, NvU32, void *, NvU32);  // virtual inherited (gpures) base (gpures)
    NvHandle (*__profilerBaseGetInternalObjectHandle__)(struct ProfilerBase * /*this*/);  // virtual inherited (gpures) base (gpures)
    NvBool (*__profilerBaseAccessCallback__)(struct ProfilerBase * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__profilerBaseGetMemInterMapParams__)(struct ProfilerBase * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__profilerBaseCheckMemInterUnmap__)(struct ProfilerBase * /*this*/, NvBool);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__profilerBaseGetMemoryMappingDescriptor__)(struct ProfilerBase * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__profilerBaseControlSerialization_Prologue__)(struct ProfilerBase * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__profilerBaseControlSerialization_Epilogue__)(struct ProfilerBase * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__profilerBaseControl_Prologue__)(struct ProfilerBase * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__profilerBaseControl_Epilogue__)(struct ProfilerBase * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NvBool (*__profilerBaseCanCopy__)(struct ProfilerBase * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__profilerBaseIsDuplicate__)(struct ProfilerBase * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (gpures)
    void (*__profilerBasePreDestruct__)(struct ProfilerBase * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__profilerBaseControlFilter__)(struct ProfilerBase * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (gpures)
    NvBool (*__profilerBaseIsPartialUnmapSupported__)(struct ProfilerBase * /*this*/);  // inline virtual inherited (res) base (gpures) body
    NV_STATUS (*__profilerBaseMapTo__)(struct ProfilerBase * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__profilerBaseUnmapFrom__)(struct ProfilerBase * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (gpures)
    NvU32 (*__profilerBaseGetRefCount__)(struct ProfilerBase * /*this*/);  // virtual inherited (res) base (gpures)
    void (*__profilerBaseAddAdditionalDependants__)(struct RsClient *, struct ProfilerBase * /*this*/, RsResourceRef *);  // virtual inherited (res) base (gpures)

    // Data members
    NvU32 maxPmaChannels;
    NvU32 pmaVchIdx;
    NvBool bLegacyHwpm;
    struct RsResourceRef **ppBytesAvailable;
    struct RsResourceRef **ppStreamBuffers;
    struct RsResourceRef *pBoundCntBuf;
    struct RsResourceRef *pBoundPmaBuf;
    NvU64 profilerId;
    HWPM_PMA_STREAM *pPmaStreamList;
    NvBool *pBindPointAllocated;
    NvBool bMmaBoostDisabled;
    NvHandle hSubDevice;
};

#ifndef __NVOC_CLASS_ProfilerBase_TYPEDEF__
#define __NVOC_CLASS_ProfilerBase_TYPEDEF__
typedef struct ProfilerBase ProfilerBase;
#endif /* __NVOC_CLASS_ProfilerBase_TYPEDEF__ */

#ifndef __nvoc_class_id_ProfilerBase
#define __nvoc_class_id_ProfilerBase 0x4976fc
#endif /* __nvoc_class_id_ProfilerBase */

// Casting support
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

NV_STATUS __nvoc_objCreate_ProfilerBase(ProfilerBase**, Dynamic*, NvU32, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_ProfilerBase(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_ProfilerBase((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define profilerBaseConstructState_FNPTR(pProf) pProf->__profilerBaseConstructState__
#define profilerBaseConstructState(pProf, pCallContext, pParams) profilerBaseConstructState_DISPATCH(pProf, pCallContext, pParams)
#define profilerBaseConstructState_HAL(pProf, pCallContext, pParams) profilerBaseConstructState_DISPATCH(pProf, pCallContext, pParams)
#define profilerBaseDestructState_FNPTR(pProf) pProf->__profilerBaseDestructState__
#define profilerBaseDestructState(pProf) profilerBaseDestructState_DISPATCH(pProf)
#define profilerBaseDestructState_HAL(pProf) profilerBaseDestructState_DISPATCH(pProf)
#define profilerBaseCtrlCmdReserveHwpmLegacy_FNPTR(pProfiler) pProfiler->__profilerBaseCtrlCmdReserveHwpmLegacy__
#define profilerBaseCtrlCmdReserveHwpmLegacy(pProfiler, pParams) profilerBaseCtrlCmdReserveHwpmLegacy_DISPATCH(pProfiler, pParams)
#define profilerBaseCtrlCmdInternalReserveHwpmLegacy_FNPTR(pProfiler) pProfiler->__profilerBaseCtrlCmdInternalReserveHwpmLegacy__
#define profilerBaseCtrlCmdInternalReserveHwpmLegacy(pProfiler, pParams) profilerBaseCtrlCmdInternalReserveHwpmLegacy_DISPATCH(pProfiler, pParams)
#define profilerBaseCtrlCmdReleaseHwpmLegacy_FNPTR(pProfiler) pProfiler->__profilerBaseCtrlCmdReleaseHwpmLegacy__
#define profilerBaseCtrlCmdReleaseHwpmLegacy(pProfiler) profilerBaseCtrlCmdReleaseHwpmLegacy_DISPATCH(pProfiler)
#define profilerBaseCtrlCmdReservePmAreaSmpc_FNPTR(pProfiler) pProfiler->__profilerBaseCtrlCmdReservePmAreaSmpc__
#define profilerBaseCtrlCmdReservePmAreaSmpc(pProfiler, pParams) profilerBaseCtrlCmdReservePmAreaSmpc_DISPATCH(pProfiler, pParams)
#define profilerBaseCtrlCmdReleasePmAreaSmpc_FNPTR(pProfiler) pProfiler->__profilerBaseCtrlCmdReleasePmAreaSmpc__
#define profilerBaseCtrlCmdReleasePmAreaSmpc(pProfiler) profilerBaseCtrlCmdReleasePmAreaSmpc_DISPATCH(pProfiler)
#define profilerBaseCtrlCmdAllocPmaStream_FNPTR(pProfiler) pProfiler->__profilerBaseCtrlCmdAllocPmaStream__
#define profilerBaseCtrlCmdAllocPmaStream(pProfiler, pParams) profilerBaseCtrlCmdAllocPmaStream_DISPATCH(pProfiler, pParams)
#define profilerBaseCtrlCmdFreePmaStream_FNPTR(pProfiler) pProfiler->__profilerBaseCtrlCmdFreePmaStream__
#define profilerBaseCtrlCmdFreePmaStream(pProfiler, pParams) profilerBaseCtrlCmdFreePmaStream_DISPATCH(pProfiler, pParams)
#define profilerBaseCtrlCmdInternalFreePmaStream_FNPTR(pProfiler) pProfiler->__profilerBaseCtrlCmdInternalFreePmaStream__
#define profilerBaseCtrlCmdInternalFreePmaStream(pProfiler, pParams) profilerBaseCtrlCmdInternalFreePmaStream_DISPATCH(pProfiler, pParams)
#define profilerBaseCtrlCmdInternalFreePmaStream_HAL(pProfiler, pParams) profilerBaseCtrlCmdInternalFreePmaStream_DISPATCH(pProfiler, pParams)
#define profilerBaseCtrlCmdInternalGetMaxPmas_FNPTR(pProfiler) pProfiler->__profilerBaseCtrlCmdInternalGetMaxPmas__
#define profilerBaseCtrlCmdInternalGetMaxPmas(pProfiler, pParams) profilerBaseCtrlCmdInternalGetMaxPmas_DISPATCH(pProfiler, pParams)
#define profilerBaseCtrlCmdBindPmResources_FNPTR(pProfiler) pProfiler->__profilerBaseCtrlCmdBindPmResources__
#define profilerBaseCtrlCmdBindPmResources(pProfiler) profilerBaseCtrlCmdBindPmResources_DISPATCH(pProfiler)
#define profilerBaseCtrlCmdUnbindPmResources_FNPTR(pProfiler) pProfiler->__profilerBaseCtrlCmdUnbindPmResources__
#define profilerBaseCtrlCmdUnbindPmResources(pProfiler) profilerBaseCtrlCmdUnbindPmResources_DISPATCH(pProfiler)
#define profilerBaseCtrlCmdUnbindPmResources_HAL(pProfiler) profilerBaseCtrlCmdUnbindPmResources_DISPATCH(pProfiler)
#define profilerBaseCtrlCmdInternalBindPmResources_FNPTR(pProfiler) pProfiler->__profilerBaseCtrlCmdInternalBindPmResources__
#define profilerBaseCtrlCmdInternalBindPmResources(pProfiler) profilerBaseCtrlCmdInternalBindPmResources_DISPATCH(pProfiler)
#define profilerBaseCtrlCmdInternalUnbindPmResources_FNPTR(pProfiler) pProfiler->__profilerBaseCtrlCmdInternalUnbindPmResources__
#define profilerBaseCtrlCmdInternalUnbindPmResources(pProfiler) profilerBaseCtrlCmdInternalUnbindPmResources_DISPATCH(pProfiler)
#define profilerBaseCtrlCmdPmaStreamUpdateGetPut_FNPTR(pProfiler) pProfiler->__profilerBaseCtrlCmdPmaStreamUpdateGetPut__
#define profilerBaseCtrlCmdPmaStreamUpdateGetPut(pProfiler, pParams) profilerBaseCtrlCmdPmaStreamUpdateGetPut_DISPATCH(pProfiler, pParams)
#define profilerBaseCtrlCmdPmaStreamUpdateGetPut_HAL(pProfiler, pParams) profilerBaseCtrlCmdPmaStreamUpdateGetPut_DISPATCH(pProfiler, pParams)
#define profilerBaseCtrlCmdExecRegops_FNPTR(pProfiler) pProfiler->__profilerBaseCtrlCmdExecRegops__
#define profilerBaseCtrlCmdExecRegops(pProfiler, pParams) profilerBaseCtrlCmdExecRegops_DISPATCH(pProfiler, pParams)
#define profilerBaseCtrlCmdInternalAllocPmaStream_FNPTR(pProfiler) pProfiler->__profilerBaseCtrlCmdInternalAllocPmaStream__
#define profilerBaseCtrlCmdInternalAllocPmaStream(pProfiler, pParams) profilerBaseCtrlCmdInternalAllocPmaStream_DISPATCH(pProfiler, pParams)
#define profilerBaseCtrlCmdInternalAllocPmaStream_HAL(pProfiler, pParams) profilerBaseCtrlCmdInternalAllocPmaStream_DISPATCH(pProfiler, pParams)
#define profilerBaseCtrlCmdInternalQuiescePmaChannel_FNPTR(pProfiler) pProfiler->__profilerBaseCtrlCmdInternalQuiescePmaChannel__
#define profilerBaseCtrlCmdInternalQuiescePmaChannel(pProfiler, pParams) profilerBaseCtrlCmdInternalQuiescePmaChannel_DISPATCH(pProfiler, pParams)
#define profilerBaseCtrlCmdInternalSriovPromotePmaStream_FNPTR(pProfiler) pProfiler->__profilerBaseCtrlCmdInternalSriovPromotePmaStream__
#define profilerBaseCtrlCmdInternalSriovPromotePmaStream(pProfiler, pParams) profilerBaseCtrlCmdInternalSriovPromotePmaStream_DISPATCH(pProfiler, pParams)
#define profilerBaseCtrlCmdInternalSriovPromotePmaStream_HAL(pProfiler, pParams) profilerBaseCtrlCmdInternalSriovPromotePmaStream_DISPATCH(pProfiler, pParams)
#define profilerBaseCtrlCmdInternalPermissionsInit_FNPTR(pProfiler) pProfiler->__profilerBaseCtrlCmdInternalPermissionsInit__
#define profilerBaseCtrlCmdInternalPermissionsInit(pProfiler, pParams) profilerBaseCtrlCmdInternalPermissionsInit_DISPATCH(pProfiler, pParams)
#define profilerBaseCtrlCmdReservePmAreaPcSampler_FNPTR(pProfiler) pProfiler->__profilerBaseCtrlCmdReservePmAreaPcSampler__
#define profilerBaseCtrlCmdReservePmAreaPcSampler(pProfiler) profilerBaseCtrlCmdReservePmAreaPcSampler_DISPATCH(pProfiler)
#define profilerBaseCtrlCmdReleasePmAreaPcSampler_FNPTR(pProfiler) pProfiler->__profilerBaseCtrlCmdReleasePmAreaPcSampler__
#define profilerBaseCtrlCmdReleasePmAreaPcSampler(pProfiler) profilerBaseCtrlCmdReleasePmAreaPcSampler_DISPATCH(pProfiler)
#define profilerBaseCtrlCmdGetTotalHsCredits_FNPTR(pProfiler) pProfiler->__profilerBaseCtrlCmdGetTotalHsCredits__
#define profilerBaseCtrlCmdGetTotalHsCredits(pProfiler, pParams) profilerBaseCtrlCmdGetTotalHsCredits_DISPATCH(pProfiler, pParams)
#define profilerBaseCtrlCmdGetChipletHsCredits_FNPTR(pProfiler) pProfiler->__profilerBaseCtrlCmdGetChipletHsCredits__
#define profilerBaseCtrlCmdGetChipletHsCredits(pProfiler, pParams) profilerBaseCtrlCmdGetChipletHsCredits_DISPATCH(pProfiler, pParams)
#define profilerBaseCtrlCmdGetHsCreditsMapping_FNPTR(pProfiler) pProfiler->__profilerBaseCtrlCmdGetHsCreditsMapping__
#define profilerBaseCtrlCmdGetHsCreditsMapping(pProfiler, pParams) profilerBaseCtrlCmdGetHsCreditsMapping_DISPATCH(pProfiler, pParams)
#define profilerBaseCtrlCmdGetHsCredits_FNPTR(pProfiler) pProfiler->__profilerBaseCtrlCmdGetHsCredits__
#define profilerBaseCtrlCmdGetHsCredits(pProfiler, pParams) profilerBaseCtrlCmdGetHsCredits_DISPATCH(pProfiler, pParams)
#define profilerBaseCtrlCmdSetHsCredits_FNPTR(pProfiler) pProfiler->__profilerBaseCtrlCmdSetHsCredits__
#define profilerBaseCtrlCmdSetHsCredits(pProfiler, pParams) profilerBaseCtrlCmdSetHsCredits_DISPATCH(pProfiler, pParams)
#define profilerBaseCtrlCmdReserveHes_FNPTR(pProfiler) pProfiler->__profilerBaseCtrlCmdReserveHes__
#define profilerBaseCtrlCmdReserveHes(pProfiler, pParams) profilerBaseCtrlCmdReserveHes_DISPATCH(pProfiler, pParams)
#define profilerBaseCtrlCmdReleaseHes_FNPTR(pProfiler) pProfiler->__profilerBaseCtrlCmdReleaseHes__
#define profilerBaseCtrlCmdReleaseHes(pProfiler, pParams) profilerBaseCtrlCmdReleaseHes_DISPATCH(pProfiler, pParams)
#define profilerBaseCtrlCmdRequestCgControls_FNPTR(pProfiler) pProfiler->__profilerBaseCtrlCmdRequestCgControls__
#define profilerBaseCtrlCmdRequestCgControls(pProfiler, pParams) profilerBaseCtrlCmdRequestCgControls_DISPATCH(pProfiler, pParams)
#define profilerBaseCtrlCmdRequestCgControls_HAL(pProfiler, pParams) profilerBaseCtrlCmdRequestCgControls_DISPATCH(pProfiler, pParams)
#define profilerBaseCtrlCmdReleaseCgControls_FNPTR(pProfiler) pProfiler->__profilerBaseCtrlCmdReleaseCgControls__
#define profilerBaseCtrlCmdReleaseCgControls(pProfiler, pParams) profilerBaseCtrlCmdReleaseCgControls_DISPATCH(pProfiler, pParams)
#define profilerBaseCtrlCmdReleaseCgControls_HAL(pProfiler, pParams) profilerBaseCtrlCmdReleaseCgControls_DISPATCH(pProfiler, pParams)
#define profilerBaseCtrlCmdDisableDynamicMMABoost_FNPTR(pProfiler) pProfiler->__profilerBaseCtrlCmdDisableDynamicMMABoost__
#define profilerBaseCtrlCmdDisableDynamicMMABoost(pProfiler, pParams) profilerBaseCtrlCmdDisableDynamicMMABoost_DISPATCH(pProfiler, pParams)
#define profilerBaseCtrlCmdGetDynamicMMABoostStatus_FNPTR(pProfiler) pProfiler->__profilerBaseCtrlCmdGetDynamicMMABoostStatus__
#define profilerBaseCtrlCmdGetDynamicMMABoostStatus(pProfiler, pParams) profilerBaseCtrlCmdGetDynamicMMABoostStatus_DISPATCH(pProfiler, pParams)
#define profilerBaseControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresControl__
#define profilerBaseControl(pGpuResource, pCallContext, pParams) profilerBaseControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define profilerBaseMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresMap__
#define profilerBaseMap(pGpuResource, pCallContext, pParams, pCpuMapping) profilerBaseMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define profilerBaseUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresUnmap__
#define profilerBaseUnmap(pGpuResource, pCallContext, pCpuMapping) profilerBaseUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define profilerBaseShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresShareCallback__
#define profilerBaseShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) profilerBaseShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define profilerBaseGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetRegBaseOffsetAndSize__
#define profilerBaseGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) profilerBaseGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define profilerBaseGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetMapAddrSpace__
#define profilerBaseGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) profilerBaseGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define profilerBaseInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresInternalControlForward__
#define profilerBaseInternalControlForward(pGpuResource, command, pParams, size) profilerBaseInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define profilerBaseGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetInternalObjectHandle__
#define profilerBaseGetInternalObjectHandle(pGpuResource) profilerBaseGetInternalObjectHandle_DISPATCH(pGpuResource)
#define profilerBaseAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresAccessCallback__
#define profilerBaseAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) profilerBaseAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define profilerBaseGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemInterMapParams__
#define profilerBaseGetMemInterMapParams(pRmResource, pParams) profilerBaseGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define profilerBaseCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresCheckMemInterUnmap__
#define profilerBaseCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) profilerBaseCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define profilerBaseGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemoryMappingDescriptor__
#define profilerBaseGetMemoryMappingDescriptor(pRmResource, ppMemDesc) profilerBaseGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define profilerBaseControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Prologue__
#define profilerBaseControlSerialization_Prologue(pResource, pCallContext, pParams) profilerBaseControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define profilerBaseControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Epilogue__
#define profilerBaseControlSerialization_Epilogue(pResource, pCallContext, pParams) profilerBaseControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define profilerBaseControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Prologue__
#define profilerBaseControl_Prologue(pResource, pCallContext, pParams) profilerBaseControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define profilerBaseControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Epilogue__
#define profilerBaseControl_Epilogue(pResource, pCallContext, pParams) profilerBaseControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define profilerBaseCanCopy_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__
#define profilerBaseCanCopy(pResource) profilerBaseCanCopy_DISPATCH(pResource)
#define profilerBaseIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsDuplicate__
#define profilerBaseIsDuplicate(pResource, hMemory, pDuplicate) profilerBaseIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define profilerBasePreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resPreDestruct__
#define profilerBasePreDestruct(pResource) profilerBasePreDestruct_DISPATCH(pResource)
#define profilerBaseControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resControlFilter__
#define profilerBaseControlFilter(pResource, pCallContext, pParams) profilerBaseControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define profilerBaseIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsPartialUnmapSupported__
#define profilerBaseIsPartialUnmapSupported(pResource) profilerBaseIsPartialUnmapSupported_DISPATCH(pResource)
#define profilerBaseMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resMapTo__
#define profilerBaseMapTo(pResource, pParams) profilerBaseMapTo_DISPATCH(pResource, pParams)
#define profilerBaseUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmapFrom__
#define profilerBaseUnmapFrom(pResource, pParams) profilerBaseUnmapFrom_DISPATCH(pResource, pParams)
#define profilerBaseGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resGetRefCount__
#define profilerBaseGetRefCount(pResource) profilerBaseGetRefCount_DISPATCH(pResource)
#define profilerBaseAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resAddAdditionalDependants__
#define profilerBaseAddAdditionalDependants(pClient, pResource, pReference) profilerBaseAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NV_STATUS profilerBaseConstructState_DISPATCH(struct ProfilerBase *pProf, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {
    return pProf->__profilerBaseConstructState__(pProf, pCallContext, pParams);
}

static inline void profilerBaseDestructState_DISPATCH(struct ProfilerBase *pProf) {
    pProf->__profilerBaseDestructState__(pProf);
}

static inline NV_STATUS profilerBaseCtrlCmdReserveHwpmLegacy_DISPATCH(struct ProfilerBase *pProfiler, NVB0CC_CTRL_RESERVE_HWPM_LEGACY_PARAMS *pParams) {
    return pProfiler->__profilerBaseCtrlCmdReserveHwpmLegacy__(pProfiler, pParams);
}

static inline NV_STATUS profilerBaseCtrlCmdInternalReserveHwpmLegacy_DISPATCH(struct ProfilerBase *pProfiler, NVB0CC_CTRL_INTERNAL_RESERVE_HWPM_LEGACY_PARAMS *pParams) {
    return pProfiler->__profilerBaseCtrlCmdInternalReserveHwpmLegacy__(pProfiler, pParams);
}

static inline NV_STATUS profilerBaseCtrlCmdReleaseHwpmLegacy_DISPATCH(struct ProfilerBase *pProfiler) {
    return pProfiler->__profilerBaseCtrlCmdReleaseHwpmLegacy__(pProfiler);
}

static inline NV_STATUS profilerBaseCtrlCmdReservePmAreaSmpc_DISPATCH(struct ProfilerBase *pProfiler, NVB0CC_CTRL_RESERVE_PM_AREA_SMPC_PARAMS *pParams) {
    return pProfiler->__profilerBaseCtrlCmdReservePmAreaSmpc__(pProfiler, pParams);
}

static inline NV_STATUS profilerBaseCtrlCmdReleasePmAreaSmpc_DISPATCH(struct ProfilerBase *pProfiler) {
    return pProfiler->__profilerBaseCtrlCmdReleasePmAreaSmpc__(pProfiler);
}

static inline NV_STATUS profilerBaseCtrlCmdAllocPmaStream_DISPATCH(struct ProfilerBase *pProfiler, NVB0CC_CTRL_ALLOC_PMA_STREAM_PARAMS *pParams) {
    return pProfiler->__profilerBaseCtrlCmdAllocPmaStream__(pProfiler, pParams);
}

static inline NV_STATUS profilerBaseCtrlCmdFreePmaStream_DISPATCH(struct ProfilerBase *pProfiler, NVB0CC_CTRL_FREE_PMA_STREAM_PARAMS *pParams) {
    return pProfiler->__profilerBaseCtrlCmdFreePmaStream__(pProfiler, pParams);
}

static inline NV_STATUS profilerBaseCtrlCmdInternalFreePmaStream_DISPATCH(struct ProfilerBase *pProfiler, NVB0CC_CTRL_INTERNAL_FREE_PMA_STREAM_PARAMS *pParams) {
    return pProfiler->__profilerBaseCtrlCmdInternalFreePmaStream__(pProfiler, pParams);
}

static inline NV_STATUS profilerBaseCtrlCmdInternalGetMaxPmas_DISPATCH(struct ProfilerBase *pProfiler, NVB0CC_CTRL_INTERNAL_GET_MAX_PMAS_PARAMS *pParams) {
    return pProfiler->__profilerBaseCtrlCmdInternalGetMaxPmas__(pProfiler, pParams);
}

static inline NV_STATUS profilerBaseCtrlCmdBindPmResources_DISPATCH(struct ProfilerBase *pProfiler) {
    return pProfiler->__profilerBaseCtrlCmdBindPmResources__(pProfiler);
}

static inline NV_STATUS profilerBaseCtrlCmdUnbindPmResources_DISPATCH(struct ProfilerBase *pProfiler) {
    return pProfiler->__profilerBaseCtrlCmdUnbindPmResources__(pProfiler);
}

static inline NV_STATUS profilerBaseCtrlCmdInternalBindPmResources_DISPATCH(struct ProfilerBase *pProfiler) {
    return pProfiler->__profilerBaseCtrlCmdInternalBindPmResources__(pProfiler);
}

static inline NV_STATUS profilerBaseCtrlCmdInternalUnbindPmResources_DISPATCH(struct ProfilerBase *pProfiler) {
    return pProfiler->__profilerBaseCtrlCmdInternalUnbindPmResources__(pProfiler);
}

static inline NV_STATUS profilerBaseCtrlCmdPmaStreamUpdateGetPut_DISPATCH(struct ProfilerBase *pProfiler, NVB0CC_CTRL_PMA_STREAM_UPDATE_GET_PUT_PARAMS *pParams) {
    return pProfiler->__profilerBaseCtrlCmdPmaStreamUpdateGetPut__(pProfiler, pParams);
}

static inline NV_STATUS profilerBaseCtrlCmdExecRegops_DISPATCH(struct ProfilerBase *pProfiler, NVB0CC_CTRL_EXEC_REG_OPS_PARAMS *pParams) {
    return pProfiler->__profilerBaseCtrlCmdExecRegops__(pProfiler, pParams);
}

static inline NV_STATUS profilerBaseCtrlCmdInternalAllocPmaStream_DISPATCH(struct ProfilerBase *pProfiler, NVB0CC_CTRL_INTERNAL_ALLOC_PMA_STREAM_PARAMS *pParams) {
    return pProfiler->__profilerBaseCtrlCmdInternalAllocPmaStream__(pProfiler, pParams);
}

static inline NV_STATUS profilerBaseCtrlCmdInternalQuiescePmaChannel_DISPATCH(struct ProfilerBase *pProfiler, NVB0CC_CTRL_INTERNAL_QUIESCE_PMA_CHANNEL_PARAMS *pParams) {
    return pProfiler->__profilerBaseCtrlCmdInternalQuiescePmaChannel__(pProfiler, pParams);
}

static inline NV_STATUS profilerBaseCtrlCmdInternalSriovPromotePmaStream_DISPATCH(struct ProfilerBase *pProfiler, NVB0CC_CTRL_INTERNAL_SRIOV_PROMOTE_PMA_STREAM_PARAMS *pParams) {
    return pProfiler->__profilerBaseCtrlCmdInternalSriovPromotePmaStream__(pProfiler, pParams);
}

static inline NV_STATUS profilerBaseCtrlCmdInternalPermissionsInit_DISPATCH(struct ProfilerBase *pProfiler, NVB0CC_CTRL_INTERNAL_PERMISSIONS_INIT_PARAMS *pParams) {
    return pProfiler->__profilerBaseCtrlCmdInternalPermissionsInit__(pProfiler, pParams);
}

static inline NV_STATUS profilerBaseCtrlCmdReservePmAreaPcSampler_DISPATCH(struct ProfilerBase *pProfiler) {
    return pProfiler->__profilerBaseCtrlCmdReservePmAreaPcSampler__(pProfiler);
}

static inline NV_STATUS profilerBaseCtrlCmdReleasePmAreaPcSampler_DISPATCH(struct ProfilerBase *pProfiler) {
    return pProfiler->__profilerBaseCtrlCmdReleasePmAreaPcSampler__(pProfiler);
}

static inline NV_STATUS profilerBaseCtrlCmdGetTotalHsCredits_DISPATCH(struct ProfilerBase *pProfiler, NVB0CC_CTRL_GET_TOTAL_HS_CREDITS_PARAMS *pParams) {
    return pProfiler->__profilerBaseCtrlCmdGetTotalHsCredits__(pProfiler, pParams);
}

static inline NV_STATUS profilerBaseCtrlCmdGetChipletHsCredits_DISPATCH(struct ProfilerBase *pProfiler, NVB0CC_CTRL_GET_CHIPLET_HS_CREDIT_POOL *pParams) {
    return pProfiler->__profilerBaseCtrlCmdGetChipletHsCredits__(pProfiler, pParams);
}

static inline NV_STATUS profilerBaseCtrlCmdGetHsCreditsMapping_DISPATCH(struct ProfilerBase *pProfiler, NVB0CC_CTRL_GET_HS_CREDITS_POOL_MAPPING_PARAMS *pParams) {
    return pProfiler->__profilerBaseCtrlCmdGetHsCreditsMapping__(pProfiler, pParams);
}

static inline NV_STATUS profilerBaseCtrlCmdGetHsCredits_DISPATCH(struct ProfilerBase *pProfiler, NVB0CC_CTRL_GET_HS_CREDITS_PARAMS *pParams) {
    return pProfiler->__profilerBaseCtrlCmdGetHsCredits__(pProfiler, pParams);
}

static inline NV_STATUS profilerBaseCtrlCmdSetHsCredits_DISPATCH(struct ProfilerBase *pProfiler, NVB0CC_CTRL_SET_HS_CREDITS_PARAMS *pParams) {
    return pProfiler->__profilerBaseCtrlCmdSetHsCredits__(pProfiler, pParams);
}

static inline NV_STATUS profilerBaseCtrlCmdReserveHes_DISPATCH(struct ProfilerBase *pProfiler, NVB0CC_CTRL_RESERVE_HES_PARAMS *pParams) {
    return pProfiler->__profilerBaseCtrlCmdReserveHes__(pProfiler, pParams);
}

static inline NV_STATUS profilerBaseCtrlCmdReleaseHes_DISPATCH(struct ProfilerBase *pProfiler, NVB0CC_CTRL_RELEASE_HES_PARAMS *pParams) {
    return pProfiler->__profilerBaseCtrlCmdReleaseHes__(pProfiler, pParams);
}

static inline NV_STATUS profilerBaseCtrlCmdRequestCgControls_DISPATCH(struct ProfilerBase *pProfiler, NVB0CC_CTRL_POWER_REQUEST_FEATURES_PARAMS *pParams) {
    return pProfiler->__profilerBaseCtrlCmdRequestCgControls__(pProfiler, pParams);
}

static inline NV_STATUS profilerBaseCtrlCmdReleaseCgControls_DISPATCH(struct ProfilerBase *pProfiler, NVB0CC_CTRL_POWER_RELEASE_FEATURES_PARAMS *pParams) {
    return pProfiler->__profilerBaseCtrlCmdReleaseCgControls__(pProfiler, pParams);
}

static inline NV_STATUS profilerBaseCtrlCmdDisableDynamicMMABoost_DISPATCH(struct ProfilerBase *pProfiler, NVB0CC_CTRL_DISABLE_DYNAMIC_MMA_BOOST_PARAMS *pParams) {
    return pProfiler->__profilerBaseCtrlCmdDisableDynamicMMABoost__(pProfiler, pParams);
}

static inline NV_STATUS profilerBaseCtrlCmdGetDynamicMMABoostStatus_DISPATCH(struct ProfilerBase *pProfiler, NVB0CC_CTRL_GET_DYNAMIC_MMA_BOOST_STATUS_PARAMS *pParams) {
    return pProfiler->__profilerBaseCtrlCmdGetDynamicMMABoostStatus__(pProfiler, pParams);
}

static inline NV_STATUS profilerBaseControl_DISPATCH(struct ProfilerBase *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__profilerBaseControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS profilerBaseMap_DISPATCH(struct ProfilerBase *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__profilerBaseMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS profilerBaseUnmap_DISPATCH(struct ProfilerBase *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__profilerBaseUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NvBool profilerBaseShareCallback_DISPATCH(struct ProfilerBase *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__profilerBaseShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS profilerBaseGetRegBaseOffsetAndSize_DISPATCH(struct ProfilerBase *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__profilerBaseGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS profilerBaseGetMapAddrSpace_DISPATCH(struct ProfilerBase *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__profilerBaseGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS profilerBaseInternalControlForward_DISPATCH(struct ProfilerBase *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__profilerBaseInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NvHandle profilerBaseGetInternalObjectHandle_DISPATCH(struct ProfilerBase *pGpuResource) {
    return pGpuResource->__profilerBaseGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool profilerBaseAccessCallback_DISPATCH(struct ProfilerBase *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__profilerBaseAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS profilerBaseGetMemInterMapParams_DISPATCH(struct ProfilerBase *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__profilerBaseGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS profilerBaseCheckMemInterUnmap_DISPATCH(struct ProfilerBase *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__profilerBaseCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS profilerBaseGetMemoryMappingDescriptor_DISPATCH(struct ProfilerBase *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__profilerBaseGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS profilerBaseControlSerialization_Prologue_DISPATCH(struct ProfilerBase *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__profilerBaseControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void profilerBaseControlSerialization_Epilogue_DISPATCH(struct ProfilerBase *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__profilerBaseControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS profilerBaseControl_Prologue_DISPATCH(struct ProfilerBase *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__profilerBaseControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void profilerBaseControl_Epilogue_DISPATCH(struct ProfilerBase *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__profilerBaseControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool profilerBaseCanCopy_DISPATCH(struct ProfilerBase *pResource) {
    return pResource->__profilerBaseCanCopy__(pResource);
}

static inline NV_STATUS profilerBaseIsDuplicate_DISPATCH(struct ProfilerBase *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__profilerBaseIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void profilerBasePreDestruct_DISPATCH(struct ProfilerBase *pResource) {
    pResource->__profilerBasePreDestruct__(pResource);
}

static inline NV_STATUS profilerBaseControlFilter_DISPATCH(struct ProfilerBase *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__profilerBaseControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool profilerBaseIsPartialUnmapSupported_DISPATCH(struct ProfilerBase *pResource) {
    return pResource->__profilerBaseIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS profilerBaseMapTo_DISPATCH(struct ProfilerBase *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__profilerBaseMapTo__(pResource, pParams);
}

static inline NV_STATUS profilerBaseUnmapFrom_DISPATCH(struct ProfilerBase *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__profilerBaseUnmapFrom__(pResource, pParams);
}

static inline NvU32 profilerBaseGetRefCount_DISPATCH(struct ProfilerBase *pResource) {
    return pResource->__profilerBaseGetRefCount__(pResource);
}

static inline void profilerBaseAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct ProfilerBase *pResource, RsResourceRef *pReference) {
    pResource->__profilerBaseAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS profilerBaseConstructState_56cd7a(struct ProfilerBase *pProf, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {
    return NV_OK;
}

NV_STATUS profilerBaseConstructState_IMPL(struct ProfilerBase *pProf, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);

void profilerBaseDestructState_VF(struct ProfilerBase *pProf);

static inline void profilerBaseDestructState_b3696a(struct ProfilerBase *pProf) {
    return;
}

NV_STATUS profilerBaseCtrlCmdReserveHwpmLegacy_IMPL(struct ProfilerBase *pProfiler, NVB0CC_CTRL_RESERVE_HWPM_LEGACY_PARAMS *pParams);

NV_STATUS profilerBaseCtrlCmdInternalReserveHwpmLegacy_IMPL(struct ProfilerBase *pProfiler, NVB0CC_CTRL_INTERNAL_RESERVE_HWPM_LEGACY_PARAMS *pParams);

NV_STATUS profilerBaseCtrlCmdReleaseHwpmLegacy_IMPL(struct ProfilerBase *pProfiler);

NV_STATUS profilerBaseCtrlCmdReservePmAreaSmpc_IMPL(struct ProfilerBase *pProfiler, NVB0CC_CTRL_RESERVE_PM_AREA_SMPC_PARAMS *pParams);

NV_STATUS profilerBaseCtrlCmdReleasePmAreaSmpc_IMPL(struct ProfilerBase *pProfiler);

NV_STATUS profilerBaseCtrlCmdAllocPmaStream_IMPL(struct ProfilerBase *pProfiler, NVB0CC_CTRL_ALLOC_PMA_STREAM_PARAMS *pParams);

NV_STATUS profilerBaseCtrlCmdFreePmaStream_IMPL(struct ProfilerBase *pProfiler, NVB0CC_CTRL_FREE_PMA_STREAM_PARAMS *pParams);

NV_STATUS profilerBaseCtrlCmdInternalFreePmaStream_VF(struct ProfilerBase *pProfiler, NVB0CC_CTRL_INTERNAL_FREE_PMA_STREAM_PARAMS *pParams);

static inline NV_STATUS profilerBaseCtrlCmdInternalFreePmaStream_56cd7a(struct ProfilerBase *pProfiler, NVB0CC_CTRL_INTERNAL_FREE_PMA_STREAM_PARAMS *pParams) {
    return NV_OK;
}

NV_STATUS profilerBaseCtrlCmdInternalGetMaxPmas_IMPL(struct ProfilerBase *pProfiler, NVB0CC_CTRL_INTERNAL_GET_MAX_PMAS_PARAMS *pParams);

NV_STATUS profilerBaseCtrlCmdBindPmResources_IMPL(struct ProfilerBase *pProfiler);

static inline NV_STATUS profilerBaseCtrlCmdUnbindPmResources_46f6a7(struct ProfilerBase *pProfiler) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS profilerBaseCtrlCmdUnbindPmResources_IMPL(struct ProfilerBase *pProfiler);

NV_STATUS profilerBaseCtrlCmdInternalBindPmResources_IMPL(struct ProfilerBase *pProfiler);

NV_STATUS profilerBaseCtrlCmdInternalUnbindPmResources_IMPL(struct ProfilerBase *pProfiler);

NV_STATUS profilerBaseCtrlCmdPmaStreamUpdateGetPut_VF(struct ProfilerBase *pProfiler, NVB0CC_CTRL_PMA_STREAM_UPDATE_GET_PUT_PARAMS *pParams);

static inline NV_STATUS profilerBaseCtrlCmdPmaStreamUpdateGetPut_56cd7a(struct ProfilerBase *pProfiler, NVB0CC_CTRL_PMA_STREAM_UPDATE_GET_PUT_PARAMS *pParams) {
    return NV_OK;
}

NV_STATUS profilerBaseCtrlCmdExecRegops_IMPL(struct ProfilerBase *pProfiler, NVB0CC_CTRL_EXEC_REG_OPS_PARAMS *pParams);

NV_STATUS profilerBaseCtrlCmdInternalAllocPmaStream_VF(struct ProfilerBase *pProfiler, NVB0CC_CTRL_INTERNAL_ALLOC_PMA_STREAM_PARAMS *pParams);

static inline NV_STATUS profilerBaseCtrlCmdInternalAllocPmaStream_56cd7a(struct ProfilerBase *pProfiler, NVB0CC_CTRL_INTERNAL_ALLOC_PMA_STREAM_PARAMS *pParams) {
    return NV_OK;
}

NV_STATUS profilerBaseCtrlCmdInternalQuiescePmaChannel_IMPL(struct ProfilerBase *pProfiler, NVB0CC_CTRL_INTERNAL_QUIESCE_PMA_CHANNEL_PARAMS *pParams);

static inline NV_STATUS profilerBaseCtrlCmdInternalSriovPromotePmaStream_108313(struct ProfilerBase *pProfiler, NVB0CC_CTRL_INTERNAL_SRIOV_PROMOTE_PMA_STREAM_PARAMS *pParams) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, ((NvBool)(0 != 0)));
}

NV_STATUS profilerBaseCtrlCmdInternalSriovPromotePmaStream_VF(struct ProfilerBase *pProfiler, NVB0CC_CTRL_INTERNAL_SRIOV_PROMOTE_PMA_STREAM_PARAMS *pParams);

NV_STATUS profilerBaseCtrlCmdInternalPermissionsInit_IMPL(struct ProfilerBase *pProfiler, NVB0CC_CTRL_INTERNAL_PERMISSIONS_INIT_PARAMS *pParams);

NV_STATUS profilerBaseCtrlCmdReservePmAreaPcSampler_IMPL(struct ProfilerBase *pProfiler);

NV_STATUS profilerBaseCtrlCmdReleasePmAreaPcSampler_IMPL(struct ProfilerBase *pProfiler);

NV_STATUS profilerBaseCtrlCmdGetTotalHsCredits_IMPL(struct ProfilerBase *pProfiler, NVB0CC_CTRL_GET_TOTAL_HS_CREDITS_PARAMS *pParams);

NV_STATUS profilerBaseCtrlCmdGetChipletHsCredits_IMPL(struct ProfilerBase *pProfiler, NVB0CC_CTRL_GET_CHIPLET_HS_CREDIT_POOL *pParams);

NV_STATUS profilerBaseCtrlCmdGetHsCreditsMapping_IMPL(struct ProfilerBase *pProfiler, NVB0CC_CTRL_GET_HS_CREDITS_POOL_MAPPING_PARAMS *pParams);

NV_STATUS profilerBaseCtrlCmdGetHsCredits_IMPL(struct ProfilerBase *pProfiler, NVB0CC_CTRL_GET_HS_CREDITS_PARAMS *pParams);

NV_STATUS profilerBaseCtrlCmdSetHsCredits_IMPL(struct ProfilerBase *pProfiler, NVB0CC_CTRL_SET_HS_CREDITS_PARAMS *pParams);

NV_STATUS profilerBaseCtrlCmdReserveHes_IMPL(struct ProfilerBase *pProfiler, NVB0CC_CTRL_RESERVE_HES_PARAMS *pParams);

NV_STATUS profilerBaseCtrlCmdReleaseHes_IMPL(struct ProfilerBase *pProfiler, NVB0CC_CTRL_RELEASE_HES_PARAMS *pParams);

NV_STATUS profilerBaseCtrlCmdRequestCgControls_VF(struct ProfilerBase *pProfiler, NVB0CC_CTRL_POWER_REQUEST_FEATURES_PARAMS *pParams);

static inline NV_STATUS profilerBaseCtrlCmdRequestCgControls_92bfc3(struct ProfilerBase *pProfiler, NVB0CC_CTRL_POWER_REQUEST_FEATURES_PARAMS *pParams) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS profilerBaseCtrlCmdReleaseCgControls_56cd7a(struct ProfilerBase *pProfiler, NVB0CC_CTRL_POWER_RELEASE_FEATURES_PARAMS *pParams) {
    return NV_OK;
}

static inline NV_STATUS profilerBaseCtrlCmdReleaseCgControls_92bfc3(struct ProfilerBase *pProfiler, NVB0CC_CTRL_POWER_RELEASE_FEATURES_PARAMS *pParams) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS profilerBaseCtrlCmdDisableDynamicMMABoost_IMPL(struct ProfilerBase *pProfiler, NVB0CC_CTRL_DISABLE_DYNAMIC_MMA_BOOST_PARAMS *pParams);

NV_STATUS profilerBaseCtrlCmdGetDynamicMMABoostStatus_IMPL(struct ProfilerBase *pProfiler, NVB0CC_CTRL_GET_DYNAMIC_MMA_BOOST_STATUS_PARAMS *pParams);

NV_STATUS profilerBaseConstruct_IMPL(struct ProfilerBase *arg_pProf, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_profilerBaseConstruct(arg_pProf, arg_pCallContext, arg_pParams) profilerBaseConstruct_IMPL(arg_pProf, arg_pCallContext, arg_pParams)
void profilerBaseDestruct_IMPL(struct ProfilerBase *pProf);

#define __nvoc_profilerBaseDestruct(pProf) profilerBaseDestruct_IMPL(pProf)
NV_STATUS profilerBaseQuiesceStreamout_IMPL(struct ProfilerBase *pProf, OBJGPU *pGpu, struct KernelHwpm *pKernelHwpm, NvU32 pmaChIdx);

#ifdef __nvoc_profiler_v2_h_disabled
static inline NV_STATUS profilerBaseQuiesceStreamout(struct ProfilerBase *pProf, OBJGPU *pGpu, struct KernelHwpm *pKernelHwpm, NvU32 pmaChIdx) {
    NV_ASSERT_FAILED_PRECOMP("ProfilerBase was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_profiler_v2_h_disabled
#define profilerBaseQuiesceStreamout(pProf, pGpu, pKernelHwpm, pmaChIdx) profilerBaseQuiesceStreamout_IMPL(pProf, pGpu, pKernelHwpm, pmaChIdx)
#endif //__nvoc_profiler_v2_h_disabled

#undef PRIVATE_FIELD


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_PROFILER_V2_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct ProfilerDev {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct ProfilerBase __nvoc_base_ProfilerBase;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct RsResource *__nvoc_pbase_RsResource;    // res super^4
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^4
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super^3
    struct GpuResource *__nvoc_pbase_GpuResource;    // gpures super^2
    struct ProfilerBase *__nvoc_pbase_ProfilerBase;    // profilerBase super
    struct ProfilerDev *__nvoc_pbase_ProfilerDev;    // profilerDev

    // Vtable with 30 per-object function pointers
    NV_STATUS (*__profilerDevConstructState__)(struct ProfilerDev * /*this*/, CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *, PROFILER_CLIENT_PERMISSIONS);  // halified (2 hals) override (profilerBase) base (profilerBase)
    NV_STATUS (*__profilerDevConstructStatePrologue__)(struct ProfilerDev * /*this*/, CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);  // halified (2 hals) body
    NV_STATUS (*__profilerDevConstructStateInterlude__)(struct ProfilerDev * /*this*/, CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *, PROFILER_CLIENT_PERMISSIONS);  // halified (2 hals) body
    NV_STATUS (*__profilerDevConstructStateEpilogue__)(struct ProfilerDev * /*this*/, CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);  // halified (2 hals) body
    void (*__profilerDevDestructState__)(struct ProfilerDev * /*this*/);  // halified (2 hals) override (profilerBase) base (profilerBase) body
    NV_STATUS (*__profilerDevControl__)(struct ProfilerDev * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (gpures) base (profilerBase)
    NV_STATUS (*__profilerDevMap__)(struct ProfilerDev * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual inherited (gpures) base (profilerBase)
    NV_STATUS (*__profilerDevUnmap__)(struct ProfilerDev * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual inherited (gpures) base (profilerBase)
    NvBool (*__profilerDevShareCallback__)(struct ProfilerDev * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (profilerBase)
    NV_STATUS (*__profilerDevGetRegBaseOffsetAndSize__)(struct ProfilerDev * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual inherited (gpures) base (profilerBase)
    NV_STATUS (*__profilerDevGetMapAddrSpace__)(struct ProfilerDev * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (gpures) base (profilerBase)
    NV_STATUS (*__profilerDevInternalControlForward__)(struct ProfilerDev * /*this*/, NvU32, void *, NvU32);  // virtual inherited (gpures) base (profilerBase)
    NvHandle (*__profilerDevGetInternalObjectHandle__)(struct ProfilerDev * /*this*/);  // virtual inherited (gpures) base (profilerBase)
    NvBool (*__profilerDevAccessCallback__)(struct ProfilerDev * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (profilerBase)
    NV_STATUS (*__profilerDevGetMemInterMapParams__)(struct ProfilerDev * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (profilerBase)
    NV_STATUS (*__profilerDevCheckMemInterUnmap__)(struct ProfilerDev * /*this*/, NvBool);  // virtual inherited (rmres) base (profilerBase)
    NV_STATUS (*__profilerDevGetMemoryMappingDescriptor__)(struct ProfilerDev * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (profilerBase)
    NV_STATUS (*__profilerDevControlSerialization_Prologue__)(struct ProfilerDev * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (profilerBase)
    void (*__profilerDevControlSerialization_Epilogue__)(struct ProfilerDev * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (profilerBase)
    NV_STATUS (*__profilerDevControl_Prologue__)(struct ProfilerDev * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (profilerBase)
    void (*__profilerDevControl_Epilogue__)(struct ProfilerDev * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (profilerBase)
    NvBool (*__profilerDevCanCopy__)(struct ProfilerDev * /*this*/);  // virtual inherited (res) base (profilerBase)
    NV_STATUS (*__profilerDevIsDuplicate__)(struct ProfilerDev * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (profilerBase)
    void (*__profilerDevPreDestruct__)(struct ProfilerDev * /*this*/);  // virtual inherited (res) base (profilerBase)
    NV_STATUS (*__profilerDevControlFilter__)(struct ProfilerDev * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (profilerBase)
    NvBool (*__profilerDevIsPartialUnmapSupported__)(struct ProfilerDev * /*this*/);  // inline virtual inherited (res) base (profilerBase) body
    NV_STATUS (*__profilerDevMapTo__)(struct ProfilerDev * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (profilerBase)
    NV_STATUS (*__profilerDevUnmapFrom__)(struct ProfilerDev * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (profilerBase)
    NvU32 (*__profilerDevGetRefCount__)(struct ProfilerDev * /*this*/);  // virtual inherited (res) base (profilerBase)
    void (*__profilerDevAddAdditionalDependants__)(struct RsClient *, struct ProfilerDev * /*this*/, RsResourceRef *);  // virtual inherited (res) base (profilerBase)
};

#ifndef __NVOC_CLASS_ProfilerDev_TYPEDEF__
#define __NVOC_CLASS_ProfilerDev_TYPEDEF__
typedef struct ProfilerDev ProfilerDev;
#endif /* __NVOC_CLASS_ProfilerDev_TYPEDEF__ */

#ifndef __nvoc_class_id_ProfilerDev
#define __nvoc_class_id_ProfilerDev 0x54d077
#endif /* __nvoc_class_id_ProfilerDev */

// Casting support
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

NV_STATUS __nvoc_objCreate_ProfilerDev(ProfilerDev**, Dynamic*, NvU32, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_ProfilerDev(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_ProfilerDev((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define profilerDevConstructState_FNPTR(pResource) pResource->__profilerDevConstructState__
#define profilerDevConstructState(pResource, pCallContext, pParams, clientPermissions) profilerDevConstructState_DISPATCH(pResource, pCallContext, pParams, clientPermissions)
#define profilerDevConstructState_HAL(pResource, pCallContext, pParams, clientPermissions) profilerDevConstructState_DISPATCH(pResource, pCallContext, pParams, clientPermissions)
#define profilerDevConstructStatePrologue_FNPTR(pResource) pResource->__profilerDevConstructStatePrologue__
#define profilerDevConstructStatePrologue(pResource, pCallContext, pParams) profilerDevConstructStatePrologue_DISPATCH(pResource, pCallContext, pParams)
#define profilerDevConstructStatePrologue_HAL(pResource, pCallContext, pParams) profilerDevConstructStatePrologue_DISPATCH(pResource, pCallContext, pParams)
#define profilerDevConstructStateInterlude_FNPTR(pResource) pResource->__profilerDevConstructStateInterlude__
#define profilerDevConstructStateInterlude(pResource, pCallContext, pParams, clientPermissions) profilerDevConstructStateInterlude_DISPATCH(pResource, pCallContext, pParams, clientPermissions)
#define profilerDevConstructStateInterlude_HAL(pResource, pCallContext, pParams, clientPermissions) profilerDevConstructStateInterlude_DISPATCH(pResource, pCallContext, pParams, clientPermissions)
#define profilerDevConstructStateEpilogue_FNPTR(pResource) pResource->__profilerDevConstructStateEpilogue__
#define profilerDevConstructStateEpilogue(pResource, pCallContext, pParams) profilerDevConstructStateEpilogue_DISPATCH(pResource, pCallContext, pParams)
#define profilerDevConstructStateEpilogue_HAL(pResource, pCallContext, pParams) profilerDevConstructStateEpilogue_DISPATCH(pResource, pCallContext, pParams)
#define profilerDevDestructState_FNPTR(pResource) pResource->__profilerDevDestructState__
#define profilerDevDestructState(pResource) profilerDevDestructState_DISPATCH(pResource)
#define profilerDevDestructState_HAL(pResource) profilerDevDestructState_DISPATCH(pResource)
#define profilerDevControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__gpuresControl__
#define profilerDevControl(pGpuResource, pCallContext, pParams) profilerDevControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define profilerDevMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__gpuresMap__
#define profilerDevMap(pGpuResource, pCallContext, pParams, pCpuMapping) profilerDevMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define profilerDevUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__gpuresUnmap__
#define profilerDevUnmap(pGpuResource, pCallContext, pCpuMapping) profilerDevUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define profilerDevShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__gpuresShareCallback__
#define profilerDevShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) profilerDevShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define profilerDevGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__gpuresGetRegBaseOffsetAndSize__
#define profilerDevGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) profilerDevGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define profilerDevGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__gpuresGetMapAddrSpace__
#define profilerDevGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) profilerDevGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define profilerDevInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__gpuresInternalControlForward__
#define profilerDevInternalControlForward(pGpuResource, command, pParams, size) profilerDevInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define profilerDevGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__gpuresGetInternalObjectHandle__
#define profilerDevGetInternalObjectHandle(pGpuResource) profilerDevGetInternalObjectHandle_DISPATCH(pGpuResource)
#define profilerDevAccessCallback_FNPTR(pResource) pResource->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresAccessCallback__
#define profilerDevAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) profilerDevAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define profilerDevGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemInterMapParams__
#define profilerDevGetMemInterMapParams(pRmResource, pParams) profilerDevGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define profilerDevCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresCheckMemInterUnmap__
#define profilerDevCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) profilerDevCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define profilerDevGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemoryMappingDescriptor__
#define profilerDevGetMemoryMappingDescriptor(pRmResource, ppMemDesc) profilerDevGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define profilerDevControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Prologue__
#define profilerDevControlSerialization_Prologue(pResource, pCallContext, pParams) profilerDevControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define profilerDevControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Epilogue__
#define profilerDevControlSerialization_Epilogue(pResource, pCallContext, pParams) profilerDevControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define profilerDevControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Prologue__
#define profilerDevControl_Prologue(pResource, pCallContext, pParams) profilerDevControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define profilerDevControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Epilogue__
#define profilerDevControl_Epilogue(pResource, pCallContext, pParams) profilerDevControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define profilerDevCanCopy_FNPTR(pResource) pResource->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__
#define profilerDevCanCopy(pResource) profilerDevCanCopy_DISPATCH(pResource)
#define profilerDevIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsDuplicate__
#define profilerDevIsDuplicate(pResource, hMemory, pDuplicate) profilerDevIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define profilerDevPreDestruct_FNPTR(pResource) pResource->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resPreDestruct__
#define profilerDevPreDestruct(pResource) profilerDevPreDestruct_DISPATCH(pResource)
#define profilerDevControlFilter_FNPTR(pResource) pResource->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resControlFilter__
#define profilerDevControlFilter(pResource, pCallContext, pParams) profilerDevControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define profilerDevIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsPartialUnmapSupported__
#define profilerDevIsPartialUnmapSupported(pResource) profilerDevIsPartialUnmapSupported_DISPATCH(pResource)
#define profilerDevMapTo_FNPTR(pResource) pResource->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resMapTo__
#define profilerDevMapTo(pResource, pParams) profilerDevMapTo_DISPATCH(pResource, pParams)
#define profilerDevUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmapFrom__
#define profilerDevUnmapFrom(pResource, pParams) profilerDevUnmapFrom_DISPATCH(pResource, pParams)
#define profilerDevGetRefCount_FNPTR(pResource) pResource->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resGetRefCount__
#define profilerDevGetRefCount(pResource) profilerDevGetRefCount_DISPATCH(pResource)
#define profilerDevAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_ProfilerBase.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resAddAdditionalDependants__
#define profilerDevAddAdditionalDependants(pClient, pResource, pReference) profilerDevAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NV_STATUS profilerDevConstructState_DISPATCH(struct ProfilerDev *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams, PROFILER_CLIENT_PERMISSIONS clientPermissions) {
    return pResource->__profilerDevConstructState__(pResource, pCallContext, pParams, clientPermissions);
}

static inline NV_STATUS profilerDevConstructStatePrologue_DISPATCH(struct ProfilerDev *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {
    return pResource->__profilerDevConstructStatePrologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS profilerDevConstructStateInterlude_DISPATCH(struct ProfilerDev *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams, PROFILER_CLIENT_PERMISSIONS clientPermissions) {
    return pResource->__profilerDevConstructStateInterlude__(pResource, pCallContext, pParams, clientPermissions);
}

static inline NV_STATUS profilerDevConstructStateEpilogue_DISPATCH(struct ProfilerDev *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {
    return pResource->__profilerDevConstructStateEpilogue__(pResource, pCallContext, pParams);
}

static inline void profilerDevDestructState_DISPATCH(struct ProfilerDev *pResource) {
    pResource->__profilerDevDestructState__(pResource);
}

static inline NV_STATUS profilerDevControl_DISPATCH(struct ProfilerDev *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__profilerDevControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS profilerDevMap_DISPATCH(struct ProfilerDev *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__profilerDevMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS profilerDevUnmap_DISPATCH(struct ProfilerDev *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__profilerDevUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NvBool profilerDevShareCallback_DISPATCH(struct ProfilerDev *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__profilerDevShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS profilerDevGetRegBaseOffsetAndSize_DISPATCH(struct ProfilerDev *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__profilerDevGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS profilerDevGetMapAddrSpace_DISPATCH(struct ProfilerDev *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__profilerDevGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS profilerDevInternalControlForward_DISPATCH(struct ProfilerDev *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__profilerDevInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NvHandle profilerDevGetInternalObjectHandle_DISPATCH(struct ProfilerDev *pGpuResource) {
    return pGpuResource->__profilerDevGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool profilerDevAccessCallback_DISPATCH(struct ProfilerDev *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__profilerDevAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS profilerDevGetMemInterMapParams_DISPATCH(struct ProfilerDev *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__profilerDevGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS profilerDevCheckMemInterUnmap_DISPATCH(struct ProfilerDev *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__profilerDevCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS profilerDevGetMemoryMappingDescriptor_DISPATCH(struct ProfilerDev *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__profilerDevGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS profilerDevControlSerialization_Prologue_DISPATCH(struct ProfilerDev *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__profilerDevControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void profilerDevControlSerialization_Epilogue_DISPATCH(struct ProfilerDev *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__profilerDevControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS profilerDevControl_Prologue_DISPATCH(struct ProfilerDev *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__profilerDevControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void profilerDevControl_Epilogue_DISPATCH(struct ProfilerDev *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__profilerDevControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool profilerDevCanCopy_DISPATCH(struct ProfilerDev *pResource) {
    return pResource->__profilerDevCanCopy__(pResource);
}

static inline NV_STATUS profilerDevIsDuplicate_DISPATCH(struct ProfilerDev *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__profilerDevIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void profilerDevPreDestruct_DISPATCH(struct ProfilerDev *pResource) {
    pResource->__profilerDevPreDestruct__(pResource);
}

static inline NV_STATUS profilerDevControlFilter_DISPATCH(struct ProfilerDev *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__profilerDevControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool profilerDevIsPartialUnmapSupported_DISPATCH(struct ProfilerDev *pResource) {
    return pResource->__profilerDevIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS profilerDevMapTo_DISPATCH(struct ProfilerDev *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__profilerDevMapTo__(pResource, pParams);
}

static inline NV_STATUS profilerDevUnmapFrom_DISPATCH(struct ProfilerDev *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__profilerDevUnmapFrom__(pResource, pParams);
}

static inline NvU32 profilerDevGetRefCount_DISPATCH(struct ProfilerDev *pResource) {
    return pResource->__profilerDevGetRefCount__(pResource);
}

static inline void profilerDevAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct ProfilerDev *pResource, RsResourceRef *pReference) {
    pResource->__profilerDevAddAdditionalDependants__(pClient, pResource, pReference);
}

NvBool profilerDevQueryCapabilities_IMPL(struct ProfilerDev *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams, PROFILER_CLIENT_PERMISSIONS *pClientPermissions);


#ifdef __nvoc_profiler_v2_h_disabled
static inline NvBool profilerDevQueryCapabilities(struct ProfilerDev *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams, PROFILER_CLIENT_PERMISSIONS *pClientPermissions) {
    NV_ASSERT_FAILED_PRECOMP("ProfilerDev was disabled!");
    return NV_FALSE;
}
#else //__nvoc_profiler_v2_h_disabled
#define profilerDevQueryCapabilities(pResource, pCallContext, pParams, pClientPermissions) profilerDevQueryCapabilities_IMPL(pResource, pCallContext, pParams, pClientPermissions)
#endif //__nvoc_profiler_v2_h_disabled

#define profilerDevQueryCapabilities_HAL(pResource, pCallContext, pParams, pClientPermissions) profilerDevQueryCapabilities(pResource, pCallContext, pParams, pClientPermissions)

NV_STATUS profilerDevConstructState_VF(struct ProfilerDev *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams, PROFILER_CLIENT_PERMISSIONS clientPermissions);

NV_STATUS profilerDevConstructState_IMPL(struct ProfilerDev *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams, PROFILER_CLIENT_PERMISSIONS clientPermissions);

NV_STATUS profilerDevConstructStatePrologue_FWCLIENT(struct ProfilerDev *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);

static inline NV_STATUS profilerDevConstructStatePrologue_92bfc3(struct ProfilerDev *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS profilerDevConstructStateInterlude_IMPL(struct ProfilerDev *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams, PROFILER_CLIENT_PERMISSIONS clientPermissions);

static inline NV_STATUS profilerDevConstructStateInterlude_92bfc3(struct ProfilerDev *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams, PROFILER_CLIENT_PERMISSIONS clientPermissions) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS profilerDevConstructStateEpilogue_FWCLIENT(struct ProfilerDev *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);

static inline NV_STATUS profilerDevConstructStateEpilogue_92bfc3(struct ProfilerDev *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

void profilerDevDestructState_FWCLIENT(struct ProfilerDev *pResource);

static inline void profilerDevDestructState_b3696a(struct ProfilerDev *pResource) {
    return;
}

NV_STATUS profilerDevConstruct_IMPL(struct ProfilerDev *arg_pResource, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_profilerDevConstruct(arg_pResource, arg_pCallContext, arg_pParams) profilerDevConstruct_IMPL(arg_pResource, arg_pCallContext, arg_pParams)
void profilerDevDestruct_IMPL(struct ProfilerDev *pResource);

#define __nvoc_profilerDevDestruct(pResource) profilerDevDestruct_IMPL(pResource)
#undef PRIVATE_FIELD

#endif // PROFILER_V2_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_PROFILER_V2_NVOC_H_
