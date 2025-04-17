
#ifndef _G_KERNEL_CHANNEL_GROUP_API_NVOC_H_
#define _G_KERNEL_CHANNEL_GROUP_API_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2021-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_kernel_channel_group_api_nvoc.h"

#ifndef KERNEL_CHANNEL_GROUP_API_H
#define KERNEL_CHANNEL_GROUP_API_H 1

#include "kernel/gpu/fifo/kernel_fifo.h"
#include "kernel/gpu/gpu_resource.h"
#include "kernel/gpu/gr/kernel_graphics_context.h"

#include "ctrl/ctrla06c.h" // KEPLER_CHANNEL_GROUP_A
#include "ctrl/ctrl0090.h" // KERNEL_GRAPHICS_CONTEXT

#include "nvoc/prelude.h"
#include "resserv/resserv.h"


struct OBJGPU;

#ifndef __NVOC_CLASS_OBJGPU_TYPEDEF__
#define __NVOC_CLASS_OBJGPU_TYPEDEF__
typedef struct OBJGPU OBJGPU;
#endif /* __NVOC_CLASS_OBJGPU_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJGPU
#define __nvoc_class_id_OBJGPU 0x7ef3cb
#endif /* __nvoc_class_id_OBJGPU */



struct KernelChannelGroup;

#ifndef __NVOC_CLASS_KernelChannelGroup_TYPEDEF__
#define __NVOC_CLASS_KernelChannelGroup_TYPEDEF__
typedef struct KernelChannelGroup KernelChannelGroup;
#endif /* __NVOC_CLASS_KernelChannelGroup_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelChannelGroup
#define __nvoc_class_id_KernelChannelGroup 0xec6de1
#endif /* __nvoc_class_id_KernelChannelGroup */




// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_KERNEL_CHANNEL_GROUP_API_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__KernelChannelGroupApi;
struct NVOC_METADATA__GpuResource;
struct NVOC_VTABLE__KernelChannelGroupApi;


struct KernelChannelGroupApi {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__KernelChannelGroupApi *__nvoc_metadata_ptr;
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
    struct KernelChannelGroupApi *__nvoc_pbase_KernelChannelGroupApi;    // kchangrpapi

    // Vtable with 17 per-object function pointers
    NV_STATUS (*__kchangrpapiCtrlCmdGpFifoSchedule__)(struct KernelChannelGroupApi * /*this*/, NVA06C_CTRL_GPFIFO_SCHEDULE_PARAMS *);  // exported (id=0xa06c0101)
    NV_STATUS (*__kchangrpapiCtrlCmdBind__)(struct KernelChannelGroupApi * /*this*/, NVA06C_CTRL_BIND_PARAMS *);  // exported (id=0xa06c0102)
    NV_STATUS (*__kchangrpapiCtrlCmdSetTimeslice__)(struct KernelChannelGroupApi * /*this*/, NVA06C_CTRL_TIMESLICE_PARAMS *);  // exported (id=0xa06c0103)
    NV_STATUS (*__kchangrpapiCtrlCmdGetTimeslice__)(struct KernelChannelGroupApi * /*this*/, NVA06C_CTRL_TIMESLICE_PARAMS *);  // exported (id=0xa06c0104)
    NV_STATUS (*__kchangrpapiCtrlCmdPreempt__)(struct KernelChannelGroupApi * /*this*/, NVA06C_CTRL_PREEMPT_PARAMS *);  // exported (id=0xa06c0105)
    NV_STATUS (*__kchangrpapiCtrlCmdGetInfo__)(struct KernelChannelGroupApi * /*this*/, NVA06C_CTRL_GET_INFO_PARAMS *);  // exported (id=0xa06c0106)
    NV_STATUS (*__kchangrpapiCtrlCmdSetInterleaveLevel__)(struct KernelChannelGroupApi * /*this*/, NVA06C_CTRL_INTERLEAVE_LEVEL_PARAMS *);  // exported (id=0xa06c0107)
    NV_STATUS (*__kchangrpapiCtrlCmdProgramVidmemPromote__)(struct KernelChannelGroupApi * /*this*/, NVA06C_CTRL_PROGRAM_VIDMEM_PROMOTE_PARAMS *);  // exported (id=0xa06c0109)
    NV_STATUS (*__kchangrpapiCtrlCmdInternalPromoteFaultMethodBuffers__)(struct KernelChannelGroupApi * /*this*/, NVA06C_CTRL_INTERNAL_PROMOTE_FAULT_METHOD_BUFFERS_PARAMS *);  // exported (id=0xa06c010a)
    NV_STATUS (*__kchangrpapiCtrlCmdMakeRealtime__)(struct KernelChannelGroupApi * /*this*/, NVA06C_CTRL_MAKE_REALTIME_PARAMS *);  // exported (id=0xa06c0110)
    NV_STATUS (*__kchangrpapiCtrlCmdInternalGpFifoSchedule__)(struct KernelChannelGroupApi * /*this*/, NVA06C_CTRL_GPFIFO_SCHEDULE_PARAMS *);  // exported (id=0xa06c0201)
    NV_STATUS (*__kchangrpapiCtrlCmdInternalSetTimeslice__)(struct KernelChannelGroupApi * /*this*/, NVA06C_CTRL_TIMESLICE_PARAMS *);  // exported (id=0xa06c0202)
    NV_STATUS (*__kchangrpapiCtrlGetTpcPartitionMode__)(struct KernelChannelGroupApi * /*this*/, NV0090_CTRL_TPC_PARTITION_MODE_PARAMS *);  // inline exported (id=0x900103) body
    NV_STATUS (*__kchangrpapiCtrlSetTpcPartitionMode__)(struct KernelChannelGroupApi * /*this*/, NV0090_CTRL_TPC_PARTITION_MODE_PARAMS *);  // inline exported (id=0x900101) body
    NV_STATUS (*__kchangrpapiCtrlGetMMUDebugMode__)(struct KernelChannelGroupApi * /*this*/, NV0090_CTRL_GET_MMU_DEBUG_MODE_PARAMS *);  // inline exported (id=0x900105) body
    NV_STATUS (*__kchangrpapiCtrlProgramVidmemPromote__)(struct KernelChannelGroupApi * /*this*/, NV0090_CTRL_PROGRAM_VIDMEM_PROMOTE_PARAMS *);  // inline exported (id=0x900107) body
    NV_STATUS (*__kchangrpapiCtrlSetLgSectorPromotion__)(struct KernelChannelGroupApi * /*this*/, NV0090_CTRL_SET_LG_SECTOR_PROMOTION_PARAMS *);  // inline exported (id=0x90010b) body

    // Data members
    struct KernelChannelGroup *pKernelChannelGroup;
    NvHandle hErrorContext;
    NvHandle hEccErrorContext;
    NvHandle hKernelGraphicsContext;
    NvHandle hLegacykCtxShareSync;
    NvHandle hLegacykCtxShareAsync;
    NvHandle hVASpace;
};


// Vtable with 25 per-class function pointers
struct NVOC_VTABLE__KernelChannelGroupApi {
    NvBool (*__kchangrpapiCanCopy__)(struct KernelChannelGroupApi * /*this*/);  // virtual override (res) base (gpures)
    NV_STATUS (*__kchangrpapiControl__)(struct KernelChannelGroupApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual override (res) base (gpures)
    NV_STATUS (*__kchangrpapiMap__)(struct KernelChannelGroupApi * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__kchangrpapiUnmap__)(struct KernelChannelGroupApi * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NvBool (*__kchangrpapiShareCallback__)(struct KernelChannelGroupApi * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__kchangrpapiGetRegBaseOffsetAndSize__)(struct KernelChannelGroupApi * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__kchangrpapiGetMapAddrSpace__)(struct KernelChannelGroupApi * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__kchangrpapiInternalControlForward__)(struct KernelChannelGroupApi * /*this*/, NvU32, void *, NvU32);  // virtual inherited (gpures) base (gpures)
    NvHandle (*__kchangrpapiGetInternalObjectHandle__)(struct KernelChannelGroupApi * /*this*/);  // virtual inherited (gpures) base (gpures)
    NvBool (*__kchangrpapiAccessCallback__)(struct KernelChannelGroupApi * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__kchangrpapiGetMemInterMapParams__)(struct KernelChannelGroupApi * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__kchangrpapiCheckMemInterUnmap__)(struct KernelChannelGroupApi * /*this*/, NvBool);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__kchangrpapiGetMemoryMappingDescriptor__)(struct KernelChannelGroupApi * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__kchangrpapiControlSerialization_Prologue__)(struct KernelChannelGroupApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__kchangrpapiControlSerialization_Epilogue__)(struct KernelChannelGroupApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__kchangrpapiControl_Prologue__)(struct KernelChannelGroupApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__kchangrpapiControl_Epilogue__)(struct KernelChannelGroupApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__kchangrpapiIsDuplicate__)(struct KernelChannelGroupApi * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (gpures)
    void (*__kchangrpapiPreDestruct__)(struct KernelChannelGroupApi * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__kchangrpapiControlFilter__)(struct KernelChannelGroupApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (gpures)
    NvBool (*__kchangrpapiIsPartialUnmapSupported__)(struct KernelChannelGroupApi * /*this*/);  // inline virtual inherited (res) base (gpures) body
    NV_STATUS (*__kchangrpapiMapTo__)(struct KernelChannelGroupApi * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__kchangrpapiUnmapFrom__)(struct KernelChannelGroupApi * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (gpures)
    NvU32 (*__kchangrpapiGetRefCount__)(struct KernelChannelGroupApi * /*this*/);  // virtual inherited (res) base (gpures)
    void (*__kchangrpapiAddAdditionalDependants__)(struct RsClient *, struct KernelChannelGroupApi * /*this*/, RsResourceRef *);  // virtual inherited (res) base (gpures)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__KernelChannelGroupApi {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResource metadata__GpuResource;
    const struct NVOC_VTABLE__KernelChannelGroupApi vtable;
};

#ifndef __NVOC_CLASS_KernelChannelGroupApi_TYPEDEF__
#define __NVOC_CLASS_KernelChannelGroupApi_TYPEDEF__
typedef struct KernelChannelGroupApi KernelChannelGroupApi;
#endif /* __NVOC_CLASS_KernelChannelGroupApi_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelChannelGroupApi
#define __nvoc_class_id_KernelChannelGroupApi 0x2b5b80
#endif /* __nvoc_class_id_KernelChannelGroupApi */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelChannelGroupApi;

#define __staticCast_KernelChannelGroupApi(pThis) \
    ((pThis)->__nvoc_pbase_KernelChannelGroupApi)

#ifdef __nvoc_kernel_channel_group_api_h_disabled
#define __dynamicCast_KernelChannelGroupApi(pThis) ((KernelChannelGroupApi*) NULL)
#else //__nvoc_kernel_channel_group_api_h_disabled
#define __dynamicCast_KernelChannelGroupApi(pThis) \
    ((KernelChannelGroupApi*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelChannelGroupApi)))
#endif //__nvoc_kernel_channel_group_api_h_disabled

NV_STATUS __nvoc_objCreateDynamic_KernelChannelGroupApi(KernelChannelGroupApi**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelChannelGroupApi(KernelChannelGroupApi**, Dynamic*, NvU32, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __objCreate_KernelChannelGroupApi(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_KernelChannelGroupApi((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define kchangrpapiCanCopy_FNPTR(pKernelChannelGroupApi) pKernelChannelGroupApi->__nvoc_metadata_ptr->vtable.__kchangrpapiCanCopy__
#define kchangrpapiCanCopy(pKernelChannelGroupApi) kchangrpapiCanCopy_DISPATCH(pKernelChannelGroupApi)
#define kchangrpapiControl_FNPTR(pKernelChannelGroupApi) pKernelChannelGroupApi->__nvoc_metadata_ptr->vtable.__kchangrpapiControl__
#define kchangrpapiControl(pKernelChannelGroupApi, pCallContext, pParams) kchangrpapiControl_DISPATCH(pKernelChannelGroupApi, pCallContext, pParams)
#define kchangrpapiCtrlCmdGpFifoSchedule_FNPTR(pKernelChannelGroupApi) pKernelChannelGroupApi->__kchangrpapiCtrlCmdGpFifoSchedule__
#define kchangrpapiCtrlCmdGpFifoSchedule(pKernelChannelGroupApi, pSchedParams) kchangrpapiCtrlCmdGpFifoSchedule_DISPATCH(pKernelChannelGroupApi, pSchedParams)
#define kchangrpapiCtrlCmdBind_FNPTR(pKernelChannelGroupApi) pKernelChannelGroupApi->__kchangrpapiCtrlCmdBind__
#define kchangrpapiCtrlCmdBind(pKernelChannelGroupApi, pParams) kchangrpapiCtrlCmdBind_DISPATCH(pKernelChannelGroupApi, pParams)
#define kchangrpapiCtrlCmdSetTimeslice_FNPTR(pKernelChannelGroupApi) pKernelChannelGroupApi->__kchangrpapiCtrlCmdSetTimeslice__
#define kchangrpapiCtrlCmdSetTimeslice(pKernelChannelGroupApi, pTsParams) kchangrpapiCtrlCmdSetTimeslice_DISPATCH(pKernelChannelGroupApi, pTsParams)
#define kchangrpapiCtrlCmdGetTimeslice_FNPTR(pKernelChannelGroupApi) pKernelChannelGroupApi->__kchangrpapiCtrlCmdGetTimeslice__
#define kchangrpapiCtrlCmdGetTimeslice(pKernelChannelGroupApi, pTsParams) kchangrpapiCtrlCmdGetTimeslice_DISPATCH(pKernelChannelGroupApi, pTsParams)
#define kchangrpapiCtrlCmdPreempt_FNPTR(pKernelChannelGroupApi) pKernelChannelGroupApi->__kchangrpapiCtrlCmdPreempt__
#define kchangrpapiCtrlCmdPreempt(pKernelChannelGroupApi, pPreemptParams) kchangrpapiCtrlCmdPreempt_DISPATCH(pKernelChannelGroupApi, pPreemptParams)
#define kchangrpapiCtrlCmdGetInfo_FNPTR(pKernelChannelGroupApi) pKernelChannelGroupApi->__kchangrpapiCtrlCmdGetInfo__
#define kchangrpapiCtrlCmdGetInfo(pKernelChannelGroupApi, pParams) kchangrpapiCtrlCmdGetInfo_DISPATCH(pKernelChannelGroupApi, pParams)
#define kchangrpapiCtrlCmdSetInterleaveLevel_FNPTR(pKernelChannelGroupApi) pKernelChannelGroupApi->__kchangrpapiCtrlCmdSetInterleaveLevel__
#define kchangrpapiCtrlCmdSetInterleaveLevel(pKernelChannelGroupApi, pParams) kchangrpapiCtrlCmdSetInterleaveLevel_DISPATCH(pKernelChannelGroupApi, pParams)
#define kchangrpapiCtrlCmdProgramVidmemPromote_FNPTR(pKernelChannelGroupApi) pKernelChannelGroupApi->__kchangrpapiCtrlCmdProgramVidmemPromote__
#define kchangrpapiCtrlCmdProgramVidmemPromote(pKernelChannelGroupApi, pParams) kchangrpapiCtrlCmdProgramVidmemPromote_DISPATCH(pKernelChannelGroupApi, pParams)
#define kchangrpapiCtrlCmdInternalPromoteFaultMethodBuffers_FNPTR(pKernelChannelGroupApi) pKernelChannelGroupApi->__kchangrpapiCtrlCmdInternalPromoteFaultMethodBuffers__
#define kchangrpapiCtrlCmdInternalPromoteFaultMethodBuffers(pKernelChannelGroupApi, pParams) kchangrpapiCtrlCmdInternalPromoteFaultMethodBuffers_DISPATCH(pKernelChannelGroupApi, pParams)
#define kchangrpapiCtrlCmdMakeRealtime_FNPTR(pKernelChannelGroupApi) pKernelChannelGroupApi->__kchangrpapiCtrlCmdMakeRealtime__
#define kchangrpapiCtrlCmdMakeRealtime(pKernelChannelGroupApi, pParams) kchangrpapiCtrlCmdMakeRealtime_DISPATCH(pKernelChannelGroupApi, pParams)
#define kchangrpapiCtrlCmdInternalGpFifoSchedule_FNPTR(pKernelChannelGroupApi) pKernelChannelGroupApi->__kchangrpapiCtrlCmdInternalGpFifoSchedule__
#define kchangrpapiCtrlCmdInternalGpFifoSchedule(pKernelChannelGroupApi, pSchedParams) kchangrpapiCtrlCmdInternalGpFifoSchedule_DISPATCH(pKernelChannelGroupApi, pSchedParams)
#define kchangrpapiCtrlCmdInternalSetTimeslice_FNPTR(pKernelChannelGroupApi) pKernelChannelGroupApi->__kchangrpapiCtrlCmdInternalSetTimeslice__
#define kchangrpapiCtrlCmdInternalSetTimeslice(pKernelChannelGroupApi, pTsParams) kchangrpapiCtrlCmdInternalSetTimeslice_DISPATCH(pKernelChannelGroupApi, pTsParams)
#define kchangrpapiCtrlGetTpcPartitionMode_FNPTR(pKernelChannelGroupApi) pKernelChannelGroupApi->__kchangrpapiCtrlGetTpcPartitionMode__
#define kchangrpapiCtrlGetTpcPartitionMode(pKernelChannelGroupApi, pParams) kchangrpapiCtrlGetTpcPartitionMode_DISPATCH(pKernelChannelGroupApi, pParams)
#define kchangrpapiCtrlSetTpcPartitionMode_FNPTR(pKernelChannelGroupApi) pKernelChannelGroupApi->__kchangrpapiCtrlSetTpcPartitionMode__
#define kchangrpapiCtrlSetTpcPartitionMode(pKernelChannelGroupApi, pParams) kchangrpapiCtrlSetTpcPartitionMode_DISPATCH(pKernelChannelGroupApi, pParams)
#define kchangrpapiCtrlGetMMUDebugMode_FNPTR(pKernelChannelGroupApi) pKernelChannelGroupApi->__kchangrpapiCtrlGetMMUDebugMode__
#define kchangrpapiCtrlGetMMUDebugMode(pKernelChannelGroupApi, pParams) kchangrpapiCtrlGetMMUDebugMode_DISPATCH(pKernelChannelGroupApi, pParams)
#define kchangrpapiCtrlProgramVidmemPromote_FNPTR(pKernelChannelGroupApi) pKernelChannelGroupApi->__kchangrpapiCtrlProgramVidmemPromote__
#define kchangrpapiCtrlProgramVidmemPromote(pKernelChannelGroupApi, pParams) kchangrpapiCtrlProgramVidmemPromote_DISPATCH(pKernelChannelGroupApi, pParams)
#define kchangrpapiCtrlSetLgSectorPromotion_FNPTR(pKernelChannelGroupApi) pKernelChannelGroupApi->__kchangrpapiCtrlSetLgSectorPromotion__
#define kchangrpapiCtrlSetLgSectorPromotion(pKernelChannelGroupApi, pParams) kchangrpapiCtrlSetLgSectorPromotion_DISPATCH(pKernelChannelGroupApi, pParams)
#define kchangrpapiMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresMap__
#define kchangrpapiMap(pGpuResource, pCallContext, pParams, pCpuMapping) kchangrpapiMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define kchangrpapiUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresUnmap__
#define kchangrpapiUnmap(pGpuResource, pCallContext, pCpuMapping) kchangrpapiUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define kchangrpapiShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresShareCallback__
#define kchangrpapiShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) kchangrpapiShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define kchangrpapiGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__
#define kchangrpapiGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) kchangrpapiGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define kchangrpapiGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__
#define kchangrpapiGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) kchangrpapiGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define kchangrpapiInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__
#define kchangrpapiInternalControlForward(pGpuResource, command, pParams, size) kchangrpapiInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define kchangrpapiGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__
#define kchangrpapiGetInternalObjectHandle(pGpuResource) kchangrpapiGetInternalObjectHandle_DISPATCH(pGpuResource)
#define kchangrpapiAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresAccessCallback__
#define kchangrpapiAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) kchangrpapiAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define kchangrpapiGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define kchangrpapiGetMemInterMapParams(pRmResource, pParams) kchangrpapiGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define kchangrpapiCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define kchangrpapiCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) kchangrpapiCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define kchangrpapiGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define kchangrpapiGetMemoryMappingDescriptor(pRmResource, ppMemDesc) kchangrpapiGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define kchangrpapiControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Prologue__
#define kchangrpapiControlSerialization_Prologue(pResource, pCallContext, pParams) kchangrpapiControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define kchangrpapiControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Epilogue__
#define kchangrpapiControlSerialization_Epilogue(pResource, pCallContext, pParams) kchangrpapiControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define kchangrpapiControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Prologue__
#define kchangrpapiControl_Prologue(pResource, pCallContext, pParams) kchangrpapiControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define kchangrpapiControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Epilogue__
#define kchangrpapiControl_Epilogue(pResource, pCallContext, pParams) kchangrpapiControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define kchangrpapiIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define kchangrpapiIsDuplicate(pResource, hMemory, pDuplicate) kchangrpapiIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define kchangrpapiPreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define kchangrpapiPreDestruct(pResource) kchangrpapiPreDestruct_DISPATCH(pResource)
#define kchangrpapiControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define kchangrpapiControlFilter(pResource, pCallContext, pParams) kchangrpapiControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define kchangrpapiIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define kchangrpapiIsPartialUnmapSupported(pResource) kchangrpapiIsPartialUnmapSupported_DISPATCH(pResource)
#define kchangrpapiMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define kchangrpapiMapTo(pResource, pParams) kchangrpapiMapTo_DISPATCH(pResource, pParams)
#define kchangrpapiUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define kchangrpapiUnmapFrom(pResource, pParams) kchangrpapiUnmapFrom_DISPATCH(pResource, pParams)
#define kchangrpapiGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define kchangrpapiGetRefCount(pResource) kchangrpapiGetRefCount_DISPATCH(pResource)
#define kchangrpapiAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define kchangrpapiAddAdditionalDependants(pClient, pResource, pReference) kchangrpapiAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NvBool kchangrpapiCanCopy_DISPATCH(struct KernelChannelGroupApi *pKernelChannelGroupApi) {
    return pKernelChannelGroupApi->__nvoc_metadata_ptr->vtable.__kchangrpapiCanCopy__(pKernelChannelGroupApi);
}

static inline NV_STATUS kchangrpapiControl_DISPATCH(struct KernelChannelGroupApi *pKernelChannelGroupApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pKernelChannelGroupApi->__nvoc_metadata_ptr->vtable.__kchangrpapiControl__(pKernelChannelGroupApi, pCallContext, pParams);
}

static inline NV_STATUS kchangrpapiCtrlCmdGpFifoSchedule_DISPATCH(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_GPFIFO_SCHEDULE_PARAMS *pSchedParams) {
    return pKernelChannelGroupApi->__kchangrpapiCtrlCmdGpFifoSchedule__(pKernelChannelGroupApi, pSchedParams);
}

static inline NV_STATUS kchangrpapiCtrlCmdBind_DISPATCH(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_BIND_PARAMS *pParams) {
    return pKernelChannelGroupApi->__kchangrpapiCtrlCmdBind__(pKernelChannelGroupApi, pParams);
}

static inline NV_STATUS kchangrpapiCtrlCmdSetTimeslice_DISPATCH(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_TIMESLICE_PARAMS *pTsParams) {
    return pKernelChannelGroupApi->__kchangrpapiCtrlCmdSetTimeslice__(pKernelChannelGroupApi, pTsParams);
}

static inline NV_STATUS kchangrpapiCtrlCmdGetTimeslice_DISPATCH(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_TIMESLICE_PARAMS *pTsParams) {
    return pKernelChannelGroupApi->__kchangrpapiCtrlCmdGetTimeslice__(pKernelChannelGroupApi, pTsParams);
}

static inline NV_STATUS kchangrpapiCtrlCmdPreempt_DISPATCH(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_PREEMPT_PARAMS *pPreemptParams) {
    return pKernelChannelGroupApi->__kchangrpapiCtrlCmdPreempt__(pKernelChannelGroupApi, pPreemptParams);
}

static inline NV_STATUS kchangrpapiCtrlCmdGetInfo_DISPATCH(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_GET_INFO_PARAMS *pParams) {
    return pKernelChannelGroupApi->__kchangrpapiCtrlCmdGetInfo__(pKernelChannelGroupApi, pParams);
}

static inline NV_STATUS kchangrpapiCtrlCmdSetInterleaveLevel_DISPATCH(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_INTERLEAVE_LEVEL_PARAMS *pParams) {
    return pKernelChannelGroupApi->__kchangrpapiCtrlCmdSetInterleaveLevel__(pKernelChannelGroupApi, pParams);
}

static inline NV_STATUS kchangrpapiCtrlCmdProgramVidmemPromote_DISPATCH(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_PROGRAM_VIDMEM_PROMOTE_PARAMS *pParams) {
    return pKernelChannelGroupApi->__kchangrpapiCtrlCmdProgramVidmemPromote__(pKernelChannelGroupApi, pParams);
}

static inline NV_STATUS kchangrpapiCtrlCmdInternalPromoteFaultMethodBuffers_DISPATCH(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_INTERNAL_PROMOTE_FAULT_METHOD_BUFFERS_PARAMS *pParams) {
    return pKernelChannelGroupApi->__kchangrpapiCtrlCmdInternalPromoteFaultMethodBuffers__(pKernelChannelGroupApi, pParams);
}

static inline NV_STATUS kchangrpapiCtrlCmdMakeRealtime_DISPATCH(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_MAKE_REALTIME_PARAMS *pParams) {
    return pKernelChannelGroupApi->__kchangrpapiCtrlCmdMakeRealtime__(pKernelChannelGroupApi, pParams);
}

static inline NV_STATUS kchangrpapiCtrlCmdInternalGpFifoSchedule_DISPATCH(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_GPFIFO_SCHEDULE_PARAMS *pSchedParams) {
    return pKernelChannelGroupApi->__kchangrpapiCtrlCmdInternalGpFifoSchedule__(pKernelChannelGroupApi, pSchedParams);
}

static inline NV_STATUS kchangrpapiCtrlCmdInternalSetTimeslice_DISPATCH(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_TIMESLICE_PARAMS *pTsParams) {
    return pKernelChannelGroupApi->__kchangrpapiCtrlCmdInternalSetTimeslice__(pKernelChannelGroupApi, pTsParams);
}

static inline NV_STATUS kchangrpapiCtrlGetTpcPartitionMode_DISPATCH(struct KernelChannelGroupApi *pKernelChannelGroupApi, NV0090_CTRL_TPC_PARTITION_MODE_PARAMS *pParams) {
    return pKernelChannelGroupApi->__kchangrpapiCtrlGetTpcPartitionMode__(pKernelChannelGroupApi, pParams);
}

static inline NV_STATUS kchangrpapiCtrlSetTpcPartitionMode_DISPATCH(struct KernelChannelGroupApi *pKernelChannelGroupApi, NV0090_CTRL_TPC_PARTITION_MODE_PARAMS *pParams) {
    return pKernelChannelGroupApi->__kchangrpapiCtrlSetTpcPartitionMode__(pKernelChannelGroupApi, pParams);
}

static inline NV_STATUS kchangrpapiCtrlGetMMUDebugMode_DISPATCH(struct KernelChannelGroupApi *pKernelChannelGroupApi, NV0090_CTRL_GET_MMU_DEBUG_MODE_PARAMS *pParams) {
    return pKernelChannelGroupApi->__kchangrpapiCtrlGetMMUDebugMode__(pKernelChannelGroupApi, pParams);
}

static inline NV_STATUS kchangrpapiCtrlProgramVidmemPromote_DISPATCH(struct KernelChannelGroupApi *pKernelChannelGroupApi, NV0090_CTRL_PROGRAM_VIDMEM_PROMOTE_PARAMS *pParams) {
    return pKernelChannelGroupApi->__kchangrpapiCtrlProgramVidmemPromote__(pKernelChannelGroupApi, pParams);
}

static inline NV_STATUS kchangrpapiCtrlSetLgSectorPromotion_DISPATCH(struct KernelChannelGroupApi *pKernelChannelGroupApi, NV0090_CTRL_SET_LG_SECTOR_PROMOTION_PARAMS *pParams) {
    return pKernelChannelGroupApi->__kchangrpapiCtrlSetLgSectorPromotion__(pKernelChannelGroupApi, pParams);
}

static inline NV_STATUS kchangrpapiMap_DISPATCH(struct KernelChannelGroupApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__kchangrpapiMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS kchangrpapiUnmap_DISPATCH(struct KernelChannelGroupApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__kchangrpapiUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NvBool kchangrpapiShareCallback_DISPATCH(struct KernelChannelGroupApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__kchangrpapiShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS kchangrpapiGetRegBaseOffsetAndSize_DISPATCH(struct KernelChannelGroupApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__kchangrpapiGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS kchangrpapiGetMapAddrSpace_DISPATCH(struct KernelChannelGroupApi *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__kchangrpapiGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS kchangrpapiInternalControlForward_DISPATCH(struct KernelChannelGroupApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__kchangrpapiInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NvHandle kchangrpapiGetInternalObjectHandle_DISPATCH(struct KernelChannelGroupApi *pGpuResource) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__kchangrpapiGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool kchangrpapiAccessCallback_DISPATCH(struct KernelChannelGroupApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_metadata_ptr->vtable.__kchangrpapiAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS kchangrpapiGetMemInterMapParams_DISPATCH(struct KernelChannelGroupApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__kchangrpapiGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS kchangrpapiCheckMemInterUnmap_DISPATCH(struct KernelChannelGroupApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__kchangrpapiCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS kchangrpapiGetMemoryMappingDescriptor_DISPATCH(struct KernelChannelGroupApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__kchangrpapiGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS kchangrpapiControlSerialization_Prologue_DISPATCH(struct KernelChannelGroupApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__kchangrpapiControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void kchangrpapiControlSerialization_Epilogue_DISPATCH(struct KernelChannelGroupApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__kchangrpapiControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS kchangrpapiControl_Prologue_DISPATCH(struct KernelChannelGroupApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__kchangrpapiControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void kchangrpapiControl_Epilogue_DISPATCH(struct KernelChannelGroupApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__kchangrpapiControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS kchangrpapiIsDuplicate_DISPATCH(struct KernelChannelGroupApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_metadata_ptr->vtable.__kchangrpapiIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void kchangrpapiPreDestruct_DISPATCH(struct KernelChannelGroupApi *pResource) {
    pResource->__nvoc_metadata_ptr->vtable.__kchangrpapiPreDestruct__(pResource);
}

static inline NV_STATUS kchangrpapiControlFilter_DISPATCH(struct KernelChannelGroupApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__kchangrpapiControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool kchangrpapiIsPartialUnmapSupported_DISPATCH(struct KernelChannelGroupApi *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__kchangrpapiIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS kchangrpapiMapTo_DISPATCH(struct KernelChannelGroupApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__kchangrpapiMapTo__(pResource, pParams);
}

static inline NV_STATUS kchangrpapiUnmapFrom_DISPATCH(struct KernelChannelGroupApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__kchangrpapiUnmapFrom__(pResource, pParams);
}

static inline NvU32 kchangrpapiGetRefCount_DISPATCH(struct KernelChannelGroupApi *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__kchangrpapiGetRefCount__(pResource);
}

static inline void kchangrpapiAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct KernelChannelGroupApi *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_metadata_ptr->vtable.__kchangrpapiAddAdditionalDependants__(pClient, pResource, pReference);
}

NvBool kchangrpapiCanCopy_IMPL(struct KernelChannelGroupApi *pKernelChannelGroupApi);

NV_STATUS kchangrpapiControl_IMPL(struct KernelChannelGroupApi *pKernelChannelGroupApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);

NV_STATUS kchangrpapiCtrlCmdGpFifoSchedule_IMPL(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_GPFIFO_SCHEDULE_PARAMS *pSchedParams);

NV_STATUS kchangrpapiCtrlCmdBind_IMPL(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_BIND_PARAMS *pParams);

NV_STATUS kchangrpapiCtrlCmdSetTimeslice_IMPL(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_TIMESLICE_PARAMS *pTsParams);

NV_STATUS kchangrpapiCtrlCmdGetTimeslice_IMPL(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_TIMESLICE_PARAMS *pTsParams);

NV_STATUS kchangrpapiCtrlCmdPreempt_IMPL(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_PREEMPT_PARAMS *pPreemptParams);

NV_STATUS kchangrpapiCtrlCmdGetInfo_IMPL(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_GET_INFO_PARAMS *pParams);

NV_STATUS kchangrpapiCtrlCmdSetInterleaveLevel_IMPL(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_INTERLEAVE_LEVEL_PARAMS *pParams);

NV_STATUS kchangrpapiCtrlCmdProgramVidmemPromote_IMPL(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_PROGRAM_VIDMEM_PROMOTE_PARAMS *pParams);

NV_STATUS kchangrpapiCtrlCmdInternalPromoteFaultMethodBuffers_IMPL(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_INTERNAL_PROMOTE_FAULT_METHOD_BUFFERS_PARAMS *pParams);

NV_STATUS kchangrpapiCtrlCmdMakeRealtime_IMPL(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_MAKE_REALTIME_PARAMS *pParams);

NV_STATUS kchangrpapiCtrlCmdInternalGpFifoSchedule_IMPL(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_GPFIFO_SCHEDULE_PARAMS *pSchedParams);

NV_STATUS kchangrpapiCtrlCmdInternalSetTimeslice_IMPL(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_TIMESLICE_PARAMS *pTsParams);

static inline NV_STATUS kchangrpapiCtrlGetTpcPartitionMode_a094e1(struct KernelChannelGroupApi *pKernelChannelGroupApi, NV0090_CTRL_TPC_PARTITION_MODE_PARAMS *pParams) {
    return kgrctxCtrlHandle(resservGetTlsCallContext(), pKernelChannelGroupApi->hKernelGraphicsContext);
}

static inline NV_STATUS kchangrpapiCtrlSetTpcPartitionMode_a094e1(struct KernelChannelGroupApi *pKernelChannelGroupApi, NV0090_CTRL_TPC_PARTITION_MODE_PARAMS *pParams) {
    return kgrctxCtrlHandle(resservGetTlsCallContext(), pKernelChannelGroupApi->hKernelGraphicsContext);
}

static inline NV_STATUS kchangrpapiCtrlGetMMUDebugMode_a094e1(struct KernelChannelGroupApi *pKernelChannelGroupApi, NV0090_CTRL_GET_MMU_DEBUG_MODE_PARAMS *pParams) {
    return kgrctxCtrlHandle(resservGetTlsCallContext(), pKernelChannelGroupApi->hKernelGraphicsContext);
}

static inline NV_STATUS kchangrpapiCtrlProgramVidmemPromote_a094e1(struct KernelChannelGroupApi *pKernelChannelGroupApi, NV0090_CTRL_PROGRAM_VIDMEM_PROMOTE_PARAMS *pParams) {
    return kgrctxCtrlHandle(resservGetTlsCallContext(), pKernelChannelGroupApi->hKernelGraphicsContext);
}

static inline NV_STATUS kchangrpapiCtrlSetLgSectorPromotion_a094e1(struct KernelChannelGroupApi *pKernelChannelGroupApi, NV0090_CTRL_SET_LG_SECTOR_PROMOTION_PARAMS *pParams) {
    return kgrctxCtrlHandle(resservGetTlsCallContext(), pKernelChannelGroupApi->hKernelGraphicsContext);
}

NV_STATUS kchangrpapiConstruct_IMPL(struct KernelChannelGroupApi *arg_pKernelChannelGroupApi, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_kchangrpapiConstruct(arg_pKernelChannelGroupApi, arg_pCallContext, arg_pParams) kchangrpapiConstruct_IMPL(arg_pKernelChannelGroupApi, arg_pCallContext, arg_pParams)
NV_STATUS kchangrpapiCopyConstruct_IMPL(struct KernelChannelGroupApi *pKernelChannelGroupApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);

#ifdef __nvoc_kernel_channel_group_api_h_disabled
static inline NV_STATUS kchangrpapiCopyConstruct(struct KernelChannelGroupApi *pKernelChannelGroupApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannelGroupApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_channel_group_api_h_disabled
#define kchangrpapiCopyConstruct(pKernelChannelGroupApi, pCallContext, pParams) kchangrpapiCopyConstruct_IMPL(pKernelChannelGroupApi, pCallContext, pParams)
#endif //__nvoc_kernel_channel_group_api_h_disabled

void kchangrpapiDestruct_IMPL(struct KernelChannelGroupApi *pKernelChannelGroupApi);

#define __nvoc_kchangrpapiDestruct(pKernelChannelGroupApi) kchangrpapiDestruct_IMPL(pKernelChannelGroupApi)
NV_STATUS kchangrpapiSetLegacyMode_IMPL(struct KernelChannelGroupApi *pKernelChannelGroupApi, struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvHandle hClient);

#ifdef __nvoc_kernel_channel_group_api_h_disabled
static inline NV_STATUS kchangrpapiSetLegacyMode(struct KernelChannelGroupApi *pKernelChannelGroupApi, struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvHandle hClient) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannelGroupApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_channel_group_api_h_disabled
#define kchangrpapiSetLegacyMode(pKernelChannelGroupApi, pGpu, pKernelFifo, hClient) kchangrpapiSetLegacyMode_IMPL(pKernelChannelGroupApi, pGpu, pKernelFifo, hClient)
#endif //__nvoc_kernel_channel_group_api_h_disabled

#undef PRIVATE_FIELD



NV_STATUS CliGetChannelGroup(NvHandle, NvHandle, RsResourceRef**, NvHandle*);

#endif // KERNEL_CHANNEL_GROUP_API_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_CHANNEL_GROUP_API_NVOC_H_
