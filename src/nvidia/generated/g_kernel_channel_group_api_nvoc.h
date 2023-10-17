#ifndef _G_KERNEL_CHANNEL_GROUP_API_NVOC_H_
#define _G_KERNEL_CHANNEL_GROUP_API_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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



#ifdef NVOC_KERNEL_CHANNEL_GROUP_API_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct KernelChannelGroupApi {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct GpuResource __nvoc_base_GpuResource;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct GpuResource *__nvoc_pbase_GpuResource;
    struct KernelChannelGroupApi *__nvoc_pbase_KernelChannelGroupApi;
    NvBool (*__kchangrpapiCanCopy__)(struct KernelChannelGroupApi *);
    NV_STATUS (*__kchangrpapiControl__)(struct KernelChannelGroupApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__kchangrpapiSetLegacyMode__)(struct KernelChannelGroupApi *, struct OBJGPU *, struct KernelFifo *, NvHandle);
    NV_STATUS (*__kchangrpapiCtrlCmdGpFifoSchedule__)(struct KernelChannelGroupApi *, NVA06C_CTRL_GPFIFO_SCHEDULE_PARAMS *);
    NV_STATUS (*__kchangrpapiCtrlCmdBind__)(struct KernelChannelGroupApi *, NVA06C_CTRL_BIND_PARAMS *);
    NV_STATUS (*__kchangrpapiCtrlCmdSetTimeslice__)(struct KernelChannelGroupApi *, NVA06C_CTRL_TIMESLICE_PARAMS *);
    NV_STATUS (*__kchangrpapiCtrlCmdGetTimeslice__)(struct KernelChannelGroupApi *, NVA06C_CTRL_TIMESLICE_PARAMS *);
    NV_STATUS (*__kchangrpapiCtrlCmdPreempt__)(struct KernelChannelGroupApi *, NVA06C_CTRL_PREEMPT_PARAMS *);
    NV_STATUS (*__kchangrpapiCtrlCmdGetInfo__)(struct KernelChannelGroupApi *, NVA06C_CTRL_GET_INFO_PARAMS *);
    NV_STATUS (*__kchangrpapiCtrlCmdSetInterleaveLevel__)(struct KernelChannelGroupApi *, NVA06C_CTRL_INTERLEAVE_LEVEL_PARAMS *);
    NV_STATUS (*__kchangrpapiCtrlCmdProgramVidmemPromote__)(struct KernelChannelGroupApi *, NVA06C_CTRL_PROGRAM_VIDMEM_PROMOTE_PARAMS *);
    NV_STATUS (*__kchangrpapiCtrlCmdInternalPromoteFaultMethodBuffers__)(struct KernelChannelGroupApi *, NVA06C_CTRL_INTERNAL_PROMOTE_FAULT_METHOD_BUFFERS_PARAMS *);
    NV_STATUS (*__kchangrpapiCtrlCmdMakeRealtime__)(struct KernelChannelGroupApi *, NVA06C_CTRL_MAKE_REALTIME_PARAMS *);
    NV_STATUS (*__kchangrpapiCtrlCmdInternalGpFifoSchedule__)(struct KernelChannelGroupApi *, NVA06C_CTRL_GPFIFO_SCHEDULE_PARAMS *);
    NV_STATUS (*__kchangrpapiCtrlCmdInternalSetTimeslice__)(struct KernelChannelGroupApi *, NVA06C_CTRL_TIMESLICE_PARAMS *);
    NV_STATUS (*__kchangrpapiCtrlGetTpcPartitionMode__)(struct KernelChannelGroupApi *, NV0090_CTRL_TPC_PARTITION_MODE_PARAMS *);
    NV_STATUS (*__kchangrpapiCtrlSetTpcPartitionMode__)(struct KernelChannelGroupApi *, NV0090_CTRL_TPC_PARTITION_MODE_PARAMS *);
    NV_STATUS (*__kchangrpapiCtrlGetMMUDebugMode__)(struct KernelChannelGroupApi *, NV0090_CTRL_GET_MMU_DEBUG_MODE_PARAMS *);
    NV_STATUS (*__kchangrpapiCtrlProgramVidmemPromote__)(struct KernelChannelGroupApi *, NV0090_CTRL_PROGRAM_VIDMEM_PROMOTE_PARAMS *);
    NvBool (*__kchangrpapiShareCallback__)(struct KernelChannelGroupApi *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__kchangrpapiCheckMemInterUnmap__)(struct KernelChannelGroupApi *, NvBool);
    NV_STATUS (*__kchangrpapiMapTo__)(struct KernelChannelGroupApi *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__kchangrpapiGetMapAddrSpace__)(struct KernelChannelGroupApi *, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    NvU32 (*__kchangrpapiGetRefCount__)(struct KernelChannelGroupApi *);
    void (*__kchangrpapiAddAdditionalDependants__)(struct RsClient *, struct KernelChannelGroupApi *, RsResourceRef *);
    NV_STATUS (*__kchangrpapiControl_Prologue__)(struct KernelChannelGroupApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__kchangrpapiGetRegBaseOffsetAndSize__)(struct KernelChannelGroupApi *, struct OBJGPU *, NvU32 *, NvU32 *);
    NV_STATUS (*__kchangrpapiInternalControlForward__)(struct KernelChannelGroupApi *, NvU32, void *, NvU32);
    NV_STATUS (*__kchangrpapiUnmapFrom__)(struct KernelChannelGroupApi *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__kchangrpapiControl_Epilogue__)(struct KernelChannelGroupApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__kchangrpapiControlLookup__)(struct KernelChannelGroupApi *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NvHandle (*__kchangrpapiGetInternalObjectHandle__)(struct KernelChannelGroupApi *);
    NV_STATUS (*__kchangrpapiUnmap__)(struct KernelChannelGroupApi *, struct CALL_CONTEXT *, struct RsCpuMapping *);
    NV_STATUS (*__kchangrpapiGetMemInterMapParams__)(struct KernelChannelGroupApi *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__kchangrpapiGetMemoryMappingDescriptor__)(struct KernelChannelGroupApi *, struct MEMORY_DESCRIPTOR **);
    NV_STATUS (*__kchangrpapiControlFilter__)(struct KernelChannelGroupApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__kchangrpapiControlSerialization_Prologue__)(struct KernelChannelGroupApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    void (*__kchangrpapiPreDestruct__)(struct KernelChannelGroupApi *);
    NV_STATUS (*__kchangrpapiIsDuplicate__)(struct KernelChannelGroupApi *, NvHandle, NvBool *);
    void (*__kchangrpapiControlSerialization_Epilogue__)(struct KernelChannelGroupApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__kchangrpapiMap__)(struct KernelChannelGroupApi *, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);
    NvBool (*__kchangrpapiAccessCallback__)(struct KernelChannelGroupApi *, struct RsClient *, void *, RsAccessRight);
    struct KernelChannelGroup *pKernelChannelGroup;
    NvHandle hErrorContext;
    NvHandle hEccErrorContext;
    NvHandle hKernelGraphicsContext;
    NvHandle hLegacykCtxShareSync;
    NvHandle hLegacykCtxShareAsync;
    NvHandle hVASpace;
};

#ifndef __NVOC_CLASS_KernelChannelGroupApi_TYPEDEF__
#define __NVOC_CLASS_KernelChannelGroupApi_TYPEDEF__
typedef struct KernelChannelGroupApi KernelChannelGroupApi;
#endif /* __NVOC_CLASS_KernelChannelGroupApi_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelChannelGroupApi
#define __nvoc_class_id_KernelChannelGroupApi 0x2b5b80
#endif /* __nvoc_class_id_KernelChannelGroupApi */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelChannelGroupApi;

#define __staticCast_KernelChannelGroupApi(pThis) \
    ((pThis)->__nvoc_pbase_KernelChannelGroupApi)

#ifdef __nvoc_kernel_channel_group_api_h_disabled
#define __dynamicCast_KernelChannelGroupApi(pThis) ((KernelChannelGroupApi*)NULL)
#else //__nvoc_kernel_channel_group_api_h_disabled
#define __dynamicCast_KernelChannelGroupApi(pThis) \
    ((KernelChannelGroupApi*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelChannelGroupApi)))
#endif //__nvoc_kernel_channel_group_api_h_disabled


NV_STATUS __nvoc_objCreateDynamic_KernelChannelGroupApi(KernelChannelGroupApi**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelChannelGroupApi(KernelChannelGroupApi**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_KernelChannelGroupApi(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_KernelChannelGroupApi((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define kchangrpapiCanCopy(pKernelChannelGroupApi) kchangrpapiCanCopy_DISPATCH(pKernelChannelGroupApi)
#define kchangrpapiControl(pKernelChannelGroupApi, pCallContext, pParams) kchangrpapiControl_DISPATCH(pKernelChannelGroupApi, pCallContext, pParams)
#define kchangrpapiSetLegacyMode(pKernelChannelGroupApi, pGpu, pKernelFifo, hClient) kchangrpapiSetLegacyMode_DISPATCH(pKernelChannelGroupApi, pGpu, pKernelFifo, hClient)
#define kchangrpapiCtrlCmdGpFifoSchedule(pKernelChannelGroupApi, pSchedParams) kchangrpapiCtrlCmdGpFifoSchedule_DISPATCH(pKernelChannelGroupApi, pSchedParams)
#define kchangrpapiCtrlCmdBind(pKernelChannelGroupApi, pParams) kchangrpapiCtrlCmdBind_DISPATCH(pKernelChannelGroupApi, pParams)
#define kchangrpapiCtrlCmdSetTimeslice(pKernelChannelGroupApi, pTsParams) kchangrpapiCtrlCmdSetTimeslice_DISPATCH(pKernelChannelGroupApi, pTsParams)
#define kchangrpapiCtrlCmdGetTimeslice(pKernelChannelGroupApi, pTsParams) kchangrpapiCtrlCmdGetTimeslice_DISPATCH(pKernelChannelGroupApi, pTsParams)
#define kchangrpapiCtrlCmdPreempt(pKernelChannelGroupApi, pPreemptParams) kchangrpapiCtrlCmdPreempt_DISPATCH(pKernelChannelGroupApi, pPreemptParams)
#define kchangrpapiCtrlCmdGetInfo(pKernelChannelGroupApi, pParams) kchangrpapiCtrlCmdGetInfo_DISPATCH(pKernelChannelGroupApi, pParams)
#define kchangrpapiCtrlCmdSetInterleaveLevel(pKernelChannelGroupApi, pParams) kchangrpapiCtrlCmdSetInterleaveLevel_DISPATCH(pKernelChannelGroupApi, pParams)
#define kchangrpapiCtrlCmdProgramVidmemPromote(pKernelChannelGroupApi, pParams) kchangrpapiCtrlCmdProgramVidmemPromote_DISPATCH(pKernelChannelGroupApi, pParams)
#define kchangrpapiCtrlCmdInternalPromoteFaultMethodBuffers(pKernelChannelGroupApi, pParams) kchangrpapiCtrlCmdInternalPromoteFaultMethodBuffers_DISPATCH(pKernelChannelGroupApi, pParams)
#define kchangrpapiCtrlCmdMakeRealtime(pKernelChannelGroupApi, pParams) kchangrpapiCtrlCmdMakeRealtime_DISPATCH(pKernelChannelGroupApi, pParams)
#define kchangrpapiCtrlCmdInternalGpFifoSchedule(pKernelChannelGroupApi, pSchedParams) kchangrpapiCtrlCmdInternalGpFifoSchedule_DISPATCH(pKernelChannelGroupApi, pSchedParams)
#define kchangrpapiCtrlCmdInternalSetTimeslice(pKernelChannelGroupApi, pTsParams) kchangrpapiCtrlCmdInternalSetTimeslice_DISPATCH(pKernelChannelGroupApi, pTsParams)
#define kchangrpapiCtrlGetTpcPartitionMode(pKernelChannelGroupApi, pParams) kchangrpapiCtrlGetTpcPartitionMode_DISPATCH(pKernelChannelGroupApi, pParams)
#define kchangrpapiCtrlSetTpcPartitionMode(pKernelChannelGroupApi, pParams) kchangrpapiCtrlSetTpcPartitionMode_DISPATCH(pKernelChannelGroupApi, pParams)
#define kchangrpapiCtrlGetMMUDebugMode(pKernelChannelGroupApi, pParams) kchangrpapiCtrlGetMMUDebugMode_DISPATCH(pKernelChannelGroupApi, pParams)
#define kchangrpapiCtrlProgramVidmemPromote(pKernelChannelGroupApi, pParams) kchangrpapiCtrlProgramVidmemPromote_DISPATCH(pKernelChannelGroupApi, pParams)
#define kchangrpapiShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) kchangrpapiShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define kchangrpapiCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) kchangrpapiCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define kchangrpapiMapTo(pResource, pParams) kchangrpapiMapTo_DISPATCH(pResource, pParams)
#define kchangrpapiGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) kchangrpapiGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define kchangrpapiGetRefCount(pResource) kchangrpapiGetRefCount_DISPATCH(pResource)
#define kchangrpapiAddAdditionalDependants(pClient, pResource, pReference) kchangrpapiAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define kchangrpapiControl_Prologue(pResource, pCallContext, pParams) kchangrpapiControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define kchangrpapiGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) kchangrpapiGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define kchangrpapiInternalControlForward(pGpuResource, command, pParams, size) kchangrpapiInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define kchangrpapiUnmapFrom(pResource, pParams) kchangrpapiUnmapFrom_DISPATCH(pResource, pParams)
#define kchangrpapiControl_Epilogue(pResource, pCallContext, pParams) kchangrpapiControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define kchangrpapiControlLookup(pResource, pParams, ppEntry) kchangrpapiControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define kchangrpapiGetInternalObjectHandle(pGpuResource) kchangrpapiGetInternalObjectHandle_DISPATCH(pGpuResource)
#define kchangrpapiUnmap(pGpuResource, pCallContext, pCpuMapping) kchangrpapiUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define kchangrpapiGetMemInterMapParams(pRmResource, pParams) kchangrpapiGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define kchangrpapiGetMemoryMappingDescriptor(pRmResource, ppMemDesc) kchangrpapiGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define kchangrpapiControlFilter(pResource, pCallContext, pParams) kchangrpapiControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define kchangrpapiControlSerialization_Prologue(pResource, pCallContext, pParams) kchangrpapiControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define kchangrpapiPreDestruct(pResource) kchangrpapiPreDestruct_DISPATCH(pResource)
#define kchangrpapiIsDuplicate(pResource, hMemory, pDuplicate) kchangrpapiIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define kchangrpapiControlSerialization_Epilogue(pResource, pCallContext, pParams) kchangrpapiControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define kchangrpapiMap(pGpuResource, pCallContext, pParams, pCpuMapping) kchangrpapiMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define kchangrpapiAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) kchangrpapiAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
NvBool kchangrpapiCanCopy_IMPL(struct KernelChannelGroupApi *pKernelChannelGroupApi);

static inline NvBool kchangrpapiCanCopy_DISPATCH(struct KernelChannelGroupApi *pKernelChannelGroupApi) {
    return pKernelChannelGroupApi->__kchangrpapiCanCopy__(pKernelChannelGroupApi);
}

NV_STATUS kchangrpapiControl_IMPL(struct KernelChannelGroupApi *pKernelChannelGroupApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);

static inline NV_STATUS kchangrpapiControl_DISPATCH(struct KernelChannelGroupApi *pKernelChannelGroupApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pKernelChannelGroupApi->__kchangrpapiControl__(pKernelChannelGroupApi, pCallContext, pParams);
}

NV_STATUS kchangrpapiSetLegacyMode_IMPL(struct KernelChannelGroupApi *pKernelChannelGroupApi, struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvHandle hClient);

static inline NV_STATUS kchangrpapiSetLegacyMode_DISPATCH(struct KernelChannelGroupApi *pKernelChannelGroupApi, struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvHandle hClient) {
    return pKernelChannelGroupApi->__kchangrpapiSetLegacyMode__(pKernelChannelGroupApi, pGpu, pKernelFifo, hClient);
}

NV_STATUS kchangrpapiCtrlCmdGpFifoSchedule_IMPL(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_GPFIFO_SCHEDULE_PARAMS *pSchedParams);

static inline NV_STATUS kchangrpapiCtrlCmdGpFifoSchedule_DISPATCH(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_GPFIFO_SCHEDULE_PARAMS *pSchedParams) {
    return pKernelChannelGroupApi->__kchangrpapiCtrlCmdGpFifoSchedule__(pKernelChannelGroupApi, pSchedParams);
}

NV_STATUS kchangrpapiCtrlCmdBind_IMPL(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_BIND_PARAMS *pParams);

static inline NV_STATUS kchangrpapiCtrlCmdBind_DISPATCH(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_BIND_PARAMS *pParams) {
    return pKernelChannelGroupApi->__kchangrpapiCtrlCmdBind__(pKernelChannelGroupApi, pParams);
}

NV_STATUS kchangrpapiCtrlCmdSetTimeslice_IMPL(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_TIMESLICE_PARAMS *pTsParams);

static inline NV_STATUS kchangrpapiCtrlCmdSetTimeslice_DISPATCH(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_TIMESLICE_PARAMS *pTsParams) {
    return pKernelChannelGroupApi->__kchangrpapiCtrlCmdSetTimeslice__(pKernelChannelGroupApi, pTsParams);
}

NV_STATUS kchangrpapiCtrlCmdGetTimeslice_IMPL(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_TIMESLICE_PARAMS *pTsParams);

static inline NV_STATUS kchangrpapiCtrlCmdGetTimeslice_DISPATCH(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_TIMESLICE_PARAMS *pTsParams) {
    return pKernelChannelGroupApi->__kchangrpapiCtrlCmdGetTimeslice__(pKernelChannelGroupApi, pTsParams);
}

NV_STATUS kchangrpapiCtrlCmdPreempt_IMPL(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_PREEMPT_PARAMS *pPreemptParams);

static inline NV_STATUS kchangrpapiCtrlCmdPreempt_DISPATCH(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_PREEMPT_PARAMS *pPreemptParams) {
    return pKernelChannelGroupApi->__kchangrpapiCtrlCmdPreempt__(pKernelChannelGroupApi, pPreemptParams);
}

NV_STATUS kchangrpapiCtrlCmdGetInfo_IMPL(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_GET_INFO_PARAMS *pParams);

static inline NV_STATUS kchangrpapiCtrlCmdGetInfo_DISPATCH(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_GET_INFO_PARAMS *pParams) {
    return pKernelChannelGroupApi->__kchangrpapiCtrlCmdGetInfo__(pKernelChannelGroupApi, pParams);
}

NV_STATUS kchangrpapiCtrlCmdSetInterleaveLevel_IMPL(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_INTERLEAVE_LEVEL_PARAMS *pParams);

static inline NV_STATUS kchangrpapiCtrlCmdSetInterleaveLevel_DISPATCH(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_INTERLEAVE_LEVEL_PARAMS *pParams) {
    return pKernelChannelGroupApi->__kchangrpapiCtrlCmdSetInterleaveLevel__(pKernelChannelGroupApi, pParams);
}

NV_STATUS kchangrpapiCtrlCmdProgramVidmemPromote_IMPL(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_PROGRAM_VIDMEM_PROMOTE_PARAMS *pParams);

static inline NV_STATUS kchangrpapiCtrlCmdProgramVidmemPromote_DISPATCH(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_PROGRAM_VIDMEM_PROMOTE_PARAMS *pParams) {
    return pKernelChannelGroupApi->__kchangrpapiCtrlCmdProgramVidmemPromote__(pKernelChannelGroupApi, pParams);
}

NV_STATUS kchangrpapiCtrlCmdInternalPromoteFaultMethodBuffers_IMPL(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_INTERNAL_PROMOTE_FAULT_METHOD_BUFFERS_PARAMS *pParams);

static inline NV_STATUS kchangrpapiCtrlCmdInternalPromoteFaultMethodBuffers_DISPATCH(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_INTERNAL_PROMOTE_FAULT_METHOD_BUFFERS_PARAMS *pParams) {
    return pKernelChannelGroupApi->__kchangrpapiCtrlCmdInternalPromoteFaultMethodBuffers__(pKernelChannelGroupApi, pParams);
}

NV_STATUS kchangrpapiCtrlCmdMakeRealtime_IMPL(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_MAKE_REALTIME_PARAMS *pParams);

static inline NV_STATUS kchangrpapiCtrlCmdMakeRealtime_DISPATCH(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_MAKE_REALTIME_PARAMS *pParams) {
    return pKernelChannelGroupApi->__kchangrpapiCtrlCmdMakeRealtime__(pKernelChannelGroupApi, pParams);
}

NV_STATUS kchangrpapiCtrlCmdInternalGpFifoSchedule_IMPL(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_GPFIFO_SCHEDULE_PARAMS *pSchedParams);

static inline NV_STATUS kchangrpapiCtrlCmdInternalGpFifoSchedule_DISPATCH(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_GPFIFO_SCHEDULE_PARAMS *pSchedParams) {
    return pKernelChannelGroupApi->__kchangrpapiCtrlCmdInternalGpFifoSchedule__(pKernelChannelGroupApi, pSchedParams);
}

NV_STATUS kchangrpapiCtrlCmdInternalSetTimeslice_IMPL(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_TIMESLICE_PARAMS *pTsParams);

static inline NV_STATUS kchangrpapiCtrlCmdInternalSetTimeslice_DISPATCH(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_TIMESLICE_PARAMS *pTsParams) {
    return pKernelChannelGroupApi->__kchangrpapiCtrlCmdInternalSetTimeslice__(pKernelChannelGroupApi, pTsParams);
}

static inline NV_STATUS kchangrpapiCtrlGetTpcPartitionMode_a094e1(struct KernelChannelGroupApi *pKernelChannelGroupApi, NV0090_CTRL_TPC_PARTITION_MODE_PARAMS *pParams) {
    return kgrctxCtrlHandle(resservGetTlsCallContext(), pKernelChannelGroupApi->hKernelGraphicsContext);
}

static inline NV_STATUS kchangrpapiCtrlGetTpcPartitionMode_DISPATCH(struct KernelChannelGroupApi *pKernelChannelGroupApi, NV0090_CTRL_TPC_PARTITION_MODE_PARAMS *pParams) {
    return pKernelChannelGroupApi->__kchangrpapiCtrlGetTpcPartitionMode__(pKernelChannelGroupApi, pParams);
}

static inline NV_STATUS kchangrpapiCtrlSetTpcPartitionMode_a094e1(struct KernelChannelGroupApi *pKernelChannelGroupApi, NV0090_CTRL_TPC_PARTITION_MODE_PARAMS *pParams) {
    return kgrctxCtrlHandle(resservGetTlsCallContext(), pKernelChannelGroupApi->hKernelGraphicsContext);
}

static inline NV_STATUS kchangrpapiCtrlSetTpcPartitionMode_DISPATCH(struct KernelChannelGroupApi *pKernelChannelGroupApi, NV0090_CTRL_TPC_PARTITION_MODE_PARAMS *pParams) {
    return pKernelChannelGroupApi->__kchangrpapiCtrlSetTpcPartitionMode__(pKernelChannelGroupApi, pParams);
}

static inline NV_STATUS kchangrpapiCtrlGetMMUDebugMode_a094e1(struct KernelChannelGroupApi *pKernelChannelGroupApi, NV0090_CTRL_GET_MMU_DEBUG_MODE_PARAMS *pParams) {
    return kgrctxCtrlHandle(resservGetTlsCallContext(), pKernelChannelGroupApi->hKernelGraphicsContext);
}

static inline NV_STATUS kchangrpapiCtrlGetMMUDebugMode_DISPATCH(struct KernelChannelGroupApi *pKernelChannelGroupApi, NV0090_CTRL_GET_MMU_DEBUG_MODE_PARAMS *pParams) {
    return pKernelChannelGroupApi->__kchangrpapiCtrlGetMMUDebugMode__(pKernelChannelGroupApi, pParams);
}

static inline NV_STATUS kchangrpapiCtrlProgramVidmemPromote_a094e1(struct KernelChannelGroupApi *pKernelChannelGroupApi, NV0090_CTRL_PROGRAM_VIDMEM_PROMOTE_PARAMS *pParams) {
    return kgrctxCtrlHandle(resservGetTlsCallContext(), pKernelChannelGroupApi->hKernelGraphicsContext);
}

static inline NV_STATUS kchangrpapiCtrlProgramVidmemPromote_DISPATCH(struct KernelChannelGroupApi *pKernelChannelGroupApi, NV0090_CTRL_PROGRAM_VIDMEM_PROMOTE_PARAMS *pParams) {
    return pKernelChannelGroupApi->__kchangrpapiCtrlProgramVidmemPromote__(pKernelChannelGroupApi, pParams);
}

static inline NvBool kchangrpapiShareCallback_DISPATCH(struct KernelChannelGroupApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__kchangrpapiShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS kchangrpapiCheckMemInterUnmap_DISPATCH(struct KernelChannelGroupApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__kchangrpapiCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS kchangrpapiMapTo_DISPATCH(struct KernelChannelGroupApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__kchangrpapiMapTo__(pResource, pParams);
}

static inline NV_STATUS kchangrpapiGetMapAddrSpace_DISPATCH(struct KernelChannelGroupApi *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__kchangrpapiGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NvU32 kchangrpapiGetRefCount_DISPATCH(struct KernelChannelGroupApi *pResource) {
    return pResource->__kchangrpapiGetRefCount__(pResource);
}

static inline void kchangrpapiAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct KernelChannelGroupApi *pResource, RsResourceRef *pReference) {
    pResource->__kchangrpapiAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS kchangrpapiControl_Prologue_DISPATCH(struct KernelChannelGroupApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__kchangrpapiControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS kchangrpapiGetRegBaseOffsetAndSize_DISPATCH(struct KernelChannelGroupApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__kchangrpapiGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS kchangrpapiInternalControlForward_DISPATCH(struct KernelChannelGroupApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__kchangrpapiInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NV_STATUS kchangrpapiUnmapFrom_DISPATCH(struct KernelChannelGroupApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__kchangrpapiUnmapFrom__(pResource, pParams);
}

static inline void kchangrpapiControl_Epilogue_DISPATCH(struct KernelChannelGroupApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__kchangrpapiControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS kchangrpapiControlLookup_DISPATCH(struct KernelChannelGroupApi *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__kchangrpapiControlLookup__(pResource, pParams, ppEntry);
}

static inline NvHandle kchangrpapiGetInternalObjectHandle_DISPATCH(struct KernelChannelGroupApi *pGpuResource) {
    return pGpuResource->__kchangrpapiGetInternalObjectHandle__(pGpuResource);
}

static inline NV_STATUS kchangrpapiUnmap_DISPATCH(struct KernelChannelGroupApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__kchangrpapiUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NV_STATUS kchangrpapiGetMemInterMapParams_DISPATCH(struct KernelChannelGroupApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__kchangrpapiGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS kchangrpapiGetMemoryMappingDescriptor_DISPATCH(struct KernelChannelGroupApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__kchangrpapiGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS kchangrpapiControlFilter_DISPATCH(struct KernelChannelGroupApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__kchangrpapiControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS kchangrpapiControlSerialization_Prologue_DISPATCH(struct KernelChannelGroupApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__kchangrpapiControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void kchangrpapiPreDestruct_DISPATCH(struct KernelChannelGroupApi *pResource) {
    pResource->__kchangrpapiPreDestruct__(pResource);
}

static inline NV_STATUS kchangrpapiIsDuplicate_DISPATCH(struct KernelChannelGroupApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__kchangrpapiIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void kchangrpapiControlSerialization_Epilogue_DISPATCH(struct KernelChannelGroupApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__kchangrpapiControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS kchangrpapiMap_DISPATCH(struct KernelChannelGroupApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__kchangrpapiMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NvBool kchangrpapiAccessCallback_DISPATCH(struct KernelChannelGroupApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__kchangrpapiAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
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
#undef PRIVATE_FIELD



NV_STATUS CliGetChannelGroup(NvHandle, NvHandle, RsResourceRef**, NvHandle*);

#endif // KERNEL_CHANNEL_GROUP_API_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_CHANNEL_GROUP_API_NVOC_H_
