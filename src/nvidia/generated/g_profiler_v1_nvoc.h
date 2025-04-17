
#ifndef _G_PROFILER_V1_NVOC_H_
#define _G_PROFILER_V1_NVOC_H_

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

#pragma once
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


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_PROFILER_V1_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__Profiler;
struct NVOC_METADATA__GpuResource;
struct NVOC_VTABLE__Profiler;


struct Profiler {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__Profiler *__nvoc_metadata_ptr;
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
    struct Profiler *__nvoc_pbase_Profiler;    // profiler

    // Vtable with 5 per-object function pointers
    NV_STATUS (*__profilerCtrlCmdProfilerReserveHwpm__)(struct Profiler * /*this*/);  // exported (id=0x90cc0101)
    NV_STATUS (*__profilerCtrlCmdProfilerReleaseHwpm__)(struct Profiler * /*this*/);  // exported (id=0x90cc0102)
    NV_STATUS (*__profilerCtrlCmdProfilerGetHwpmReservationInfo__)(struct Profiler * /*this*/, NV90CC_CTRL_HWPM_GET_RESERVATION_INFO_PARAMS *);  // exported (id=0x90cc0103)
    NV_STATUS (*__profilerCtrlCmdProfilerRequestCgControls__)(struct Profiler * /*this*/, NV90CC_CTRL_POWER_REQUEST_FEATURES_PARAMS *);  // exported (id=0x90cc0301)
    NV_STATUS (*__profilerCtrlCmdProfilerReleaseCgControls__)(struct Profiler * /*this*/, NV90CC_CTRL_POWER_RELEASE_FEATURES_PARAMS *);  // exported (id=0x90cc0302)
};


// Vtable with 25 per-class function pointers
struct NVOC_VTABLE__Profiler {
    NV_STATUS (*__profilerControl__)(struct Profiler * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual override (res) base (gpures)
    NV_STATUS (*__profilerMap__)(struct Profiler * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__profilerUnmap__)(struct Profiler * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NvBool (*__profilerShareCallback__)(struct Profiler * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__profilerGetRegBaseOffsetAndSize__)(struct Profiler * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__profilerGetMapAddrSpace__)(struct Profiler * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__profilerInternalControlForward__)(struct Profiler * /*this*/, NvU32, void *, NvU32);  // virtual inherited (gpures) base (gpures)
    NvHandle (*__profilerGetInternalObjectHandle__)(struct Profiler * /*this*/);  // virtual inherited (gpures) base (gpures)
    NvBool (*__profilerAccessCallback__)(struct Profiler * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__profilerGetMemInterMapParams__)(struct Profiler * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__profilerCheckMemInterUnmap__)(struct Profiler * /*this*/, NvBool);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__profilerGetMemoryMappingDescriptor__)(struct Profiler * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__profilerControlSerialization_Prologue__)(struct Profiler * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__profilerControlSerialization_Epilogue__)(struct Profiler * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__profilerControl_Prologue__)(struct Profiler * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__profilerControl_Epilogue__)(struct Profiler * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NvBool (*__profilerCanCopy__)(struct Profiler * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__profilerIsDuplicate__)(struct Profiler * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (gpures)
    void (*__profilerPreDestruct__)(struct Profiler * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__profilerControlFilter__)(struct Profiler * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (gpures)
    NvBool (*__profilerIsPartialUnmapSupported__)(struct Profiler * /*this*/);  // inline virtual inherited (res) base (gpures) body
    NV_STATUS (*__profilerMapTo__)(struct Profiler * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__profilerUnmapFrom__)(struct Profiler * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (gpures)
    NvU32 (*__profilerGetRefCount__)(struct Profiler * /*this*/);  // virtual inherited (res) base (gpures)
    void (*__profilerAddAdditionalDependants__)(struct RsClient *, struct Profiler * /*this*/, RsResourceRef *);  // virtual inherited (res) base (gpures)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__Profiler {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResource metadata__GpuResource;
    const struct NVOC_VTABLE__Profiler vtable;
};

#ifndef __NVOC_CLASS_Profiler_TYPEDEF__
#define __NVOC_CLASS_Profiler_TYPEDEF__
typedef struct Profiler Profiler;
#endif /* __NVOC_CLASS_Profiler_TYPEDEF__ */

#ifndef __nvoc_class_id_Profiler
#define __nvoc_class_id_Profiler 0x65b4c7
#endif /* __nvoc_class_id_Profiler */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Profiler;

#define __staticCast_Profiler(pThis) \
    ((pThis)->__nvoc_pbase_Profiler)

#ifdef __nvoc_profiler_v1_h_disabled
#define __dynamicCast_Profiler(pThis) ((Profiler*) NULL)
#else //__nvoc_profiler_v1_h_disabled
#define __dynamicCast_Profiler(pThis) \
    ((Profiler*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(Profiler)))
#endif //__nvoc_profiler_v1_h_disabled

NV_STATUS __nvoc_objCreateDynamic_Profiler(Profiler**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_Profiler(Profiler**, Dynamic*, NvU32, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __objCreate_Profiler(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_Profiler((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define profilerControl_FNPTR(pProfiler) pProfiler->__nvoc_metadata_ptr->vtable.__profilerControl__
#define profilerControl(pProfiler, pCallContext, pParams) profilerControl_DISPATCH(pProfiler, pCallContext, pParams)
#define profilerCtrlCmdProfilerReserveHwpm_FNPTR(pProfiler) pProfiler->__profilerCtrlCmdProfilerReserveHwpm__
#define profilerCtrlCmdProfilerReserveHwpm(pProfiler) profilerCtrlCmdProfilerReserveHwpm_DISPATCH(pProfiler)
#define profilerCtrlCmdProfilerReleaseHwpm_FNPTR(pProfiler) pProfiler->__profilerCtrlCmdProfilerReleaseHwpm__
#define profilerCtrlCmdProfilerReleaseHwpm(pProfiler) profilerCtrlCmdProfilerReleaseHwpm_DISPATCH(pProfiler)
#define profilerCtrlCmdProfilerGetHwpmReservationInfo_FNPTR(pProfiler) pProfiler->__profilerCtrlCmdProfilerGetHwpmReservationInfo__
#define profilerCtrlCmdProfilerGetHwpmReservationInfo(pProfiler, pParams) profilerCtrlCmdProfilerGetHwpmReservationInfo_DISPATCH(pProfiler, pParams)
#define profilerCtrlCmdProfilerRequestCgControls_FNPTR(pProfiler) pProfiler->__profilerCtrlCmdProfilerRequestCgControls__
#define profilerCtrlCmdProfilerRequestCgControls(pProfiler, pParams) profilerCtrlCmdProfilerRequestCgControls_DISPATCH(pProfiler, pParams)
#define profilerCtrlCmdProfilerReleaseCgControls_FNPTR(pProfiler) pProfiler->__profilerCtrlCmdProfilerReleaseCgControls__
#define profilerCtrlCmdProfilerReleaseCgControls(pProfiler, pParams) profilerCtrlCmdProfilerReleaseCgControls_DISPATCH(pProfiler, pParams)
#define profilerMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresMap__
#define profilerMap(pGpuResource, pCallContext, pParams, pCpuMapping) profilerMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define profilerUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresUnmap__
#define profilerUnmap(pGpuResource, pCallContext, pCpuMapping) profilerUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define profilerShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresShareCallback__
#define profilerShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) profilerShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define profilerGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__
#define profilerGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) profilerGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define profilerGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__
#define profilerGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) profilerGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define profilerInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__
#define profilerInternalControlForward(pGpuResource, command, pParams, size) profilerInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define profilerGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__
#define profilerGetInternalObjectHandle(pGpuResource) profilerGetInternalObjectHandle_DISPATCH(pGpuResource)
#define profilerAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresAccessCallback__
#define profilerAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) profilerAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define profilerGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define profilerGetMemInterMapParams(pRmResource, pParams) profilerGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define profilerCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define profilerCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) profilerCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define profilerGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define profilerGetMemoryMappingDescriptor(pRmResource, ppMemDesc) profilerGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define profilerControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Prologue__
#define profilerControlSerialization_Prologue(pResource, pCallContext, pParams) profilerControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define profilerControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Epilogue__
#define profilerControlSerialization_Epilogue(pResource, pCallContext, pParams) profilerControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define profilerControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Prologue__
#define profilerControl_Prologue(pResource, pCallContext, pParams) profilerControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define profilerControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Epilogue__
#define profilerControl_Epilogue(pResource, pCallContext, pParams) profilerControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define profilerCanCopy_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define profilerCanCopy(pResource) profilerCanCopy_DISPATCH(pResource)
#define profilerIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define profilerIsDuplicate(pResource, hMemory, pDuplicate) profilerIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define profilerPreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define profilerPreDestruct(pResource) profilerPreDestruct_DISPATCH(pResource)
#define profilerControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define profilerControlFilter(pResource, pCallContext, pParams) profilerControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define profilerIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define profilerIsPartialUnmapSupported(pResource) profilerIsPartialUnmapSupported_DISPATCH(pResource)
#define profilerMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define profilerMapTo(pResource, pParams) profilerMapTo_DISPATCH(pResource, pParams)
#define profilerUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define profilerUnmapFrom(pResource, pParams) profilerUnmapFrom_DISPATCH(pResource, pParams)
#define profilerGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define profilerGetRefCount(pResource) profilerGetRefCount_DISPATCH(pResource)
#define profilerAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define profilerAddAdditionalDependants(pClient, pResource, pReference) profilerAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NV_STATUS profilerControl_DISPATCH(struct Profiler *pProfiler, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pProfiler->__nvoc_metadata_ptr->vtable.__profilerControl__(pProfiler, pCallContext, pParams);
}

static inline NV_STATUS profilerCtrlCmdProfilerReserveHwpm_DISPATCH(struct Profiler *pProfiler) {
    return pProfiler->__profilerCtrlCmdProfilerReserveHwpm__(pProfiler);
}

static inline NV_STATUS profilerCtrlCmdProfilerReleaseHwpm_DISPATCH(struct Profiler *pProfiler) {
    return pProfiler->__profilerCtrlCmdProfilerReleaseHwpm__(pProfiler);
}

static inline NV_STATUS profilerCtrlCmdProfilerGetHwpmReservationInfo_DISPATCH(struct Profiler *pProfiler, NV90CC_CTRL_HWPM_GET_RESERVATION_INFO_PARAMS *pParams) {
    return pProfiler->__profilerCtrlCmdProfilerGetHwpmReservationInfo__(pProfiler, pParams);
}

static inline NV_STATUS profilerCtrlCmdProfilerRequestCgControls_DISPATCH(struct Profiler *pProfiler, NV90CC_CTRL_POWER_REQUEST_FEATURES_PARAMS *pParams) {
    return pProfiler->__profilerCtrlCmdProfilerRequestCgControls__(pProfiler, pParams);
}

static inline NV_STATUS profilerCtrlCmdProfilerReleaseCgControls_DISPATCH(struct Profiler *pProfiler, NV90CC_CTRL_POWER_RELEASE_FEATURES_PARAMS *pParams) {
    return pProfiler->__profilerCtrlCmdProfilerReleaseCgControls__(pProfiler, pParams);
}

static inline NV_STATUS profilerMap_DISPATCH(struct Profiler *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__profilerMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS profilerUnmap_DISPATCH(struct Profiler *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__profilerUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NvBool profilerShareCallback_DISPATCH(struct Profiler *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__profilerShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS profilerGetRegBaseOffsetAndSize_DISPATCH(struct Profiler *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__profilerGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS profilerGetMapAddrSpace_DISPATCH(struct Profiler *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__profilerGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS profilerInternalControlForward_DISPATCH(struct Profiler *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__profilerInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NvHandle profilerGetInternalObjectHandle_DISPATCH(struct Profiler *pGpuResource) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__profilerGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool profilerAccessCallback_DISPATCH(struct Profiler *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_metadata_ptr->vtable.__profilerAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS profilerGetMemInterMapParams_DISPATCH(struct Profiler *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__profilerGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS profilerCheckMemInterUnmap_DISPATCH(struct Profiler *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__profilerCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS profilerGetMemoryMappingDescriptor_DISPATCH(struct Profiler *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__profilerGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS profilerControlSerialization_Prologue_DISPATCH(struct Profiler *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__profilerControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void profilerControlSerialization_Epilogue_DISPATCH(struct Profiler *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__profilerControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS profilerControl_Prologue_DISPATCH(struct Profiler *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__profilerControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void profilerControl_Epilogue_DISPATCH(struct Profiler *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__profilerControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool profilerCanCopy_DISPATCH(struct Profiler *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__profilerCanCopy__(pResource);
}

static inline NV_STATUS profilerIsDuplicate_DISPATCH(struct Profiler *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_metadata_ptr->vtable.__profilerIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void profilerPreDestruct_DISPATCH(struct Profiler *pResource) {
    pResource->__nvoc_metadata_ptr->vtable.__profilerPreDestruct__(pResource);
}

static inline NV_STATUS profilerControlFilter_DISPATCH(struct Profiler *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__profilerControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool profilerIsPartialUnmapSupported_DISPATCH(struct Profiler *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__profilerIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS profilerMapTo_DISPATCH(struct Profiler *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__profilerMapTo__(pResource, pParams);
}

static inline NV_STATUS profilerUnmapFrom_DISPATCH(struct Profiler *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__profilerUnmapFrom__(pResource, pParams);
}

static inline NvU32 profilerGetRefCount_DISPATCH(struct Profiler *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__profilerGetRefCount__(pResource);
}

static inline void profilerAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct Profiler *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_metadata_ptr->vtable.__profilerAddAdditionalDependants__(pClient, pResource, pReference);
}

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

NV_STATUS profilerCtrlCmdProfilerReserveHwpm_IMPL(struct Profiler *pProfiler);

NV_STATUS profilerCtrlCmdProfilerReleaseHwpm_IMPL(struct Profiler *pProfiler);

NV_STATUS profilerCtrlCmdProfilerGetHwpmReservationInfo_IMPL(struct Profiler *pProfiler, NV90CC_CTRL_HWPM_GET_RESERVATION_INFO_PARAMS *pParams);

NV_STATUS profilerCtrlCmdProfilerRequestCgControls_IMPL(struct Profiler *pProfiler, NV90CC_CTRL_POWER_REQUEST_FEATURES_PARAMS *pParams);

NV_STATUS profilerCtrlCmdProfilerReleaseCgControls_IMPL(struct Profiler *pProfiler, NV90CC_CTRL_POWER_RELEASE_FEATURES_PARAMS *pParams);

NV_STATUS profilerConstruct_IMPL(struct Profiler *arg_pProfiler, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_profilerConstruct(arg_pProfiler, arg_pCallContext, arg_pParams) profilerConstruct_IMPL(arg_pProfiler, arg_pCallContext, arg_pParams)
#undef PRIVATE_FIELD


#endif // PROFILER_V1_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_PROFILER_V1_NVOC_H_
