
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
 * SPDX-FileCopyrightText: Copyright (c) 2021-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __nvoc_class_id_OBJGPU
#define __nvoc_class_id_OBJGPU 0x7ef3cbu
typedef struct OBJGPU OBJGPU;
#endif /* __nvoc_class_id_OBJGPU */


struct KernelChannelGroup;

#ifndef __nvoc_class_id_KernelChannelGroup
#define __nvoc_class_id_KernelChannelGroup 0xec6de1u
typedef struct KernelChannelGroup KernelChannelGroup;
#endif /* __nvoc_class_id_KernelChannelGroup */




// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_KERNEL_CHANNEL_GROUP_API_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__KernelChannelGroupApi;
struct NVOC_METADATA__GpuResource;


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

    // Data members
    struct KernelChannelGroup *pKernelChannelGroup;
    NvHandle hErrorContext;
    NvHandle hEccErrorContext;
    NvHandle hKernelGraphicsContext;
    NvHandle hLegacykCtxShareSync;
    NvHandle hLegacykCtxShareAsync;
    NvHandle hVASpace;
    NvHandle hHandleVASpace;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__KernelChannelGroupApi {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResource metadata__GpuResource;
};

#ifndef __nvoc_class_id_KernelChannelGroupApi
#define __nvoc_class_id_KernelChannelGroupApi 0x2b5b80u
typedef struct KernelChannelGroupApi KernelChannelGroupApi;
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

NV_STATUS __nvoc_objCreateDynamic_KernelChannelGroupApi(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelChannelGroupApi(KernelChannelGroupApi**, Dynamic*, NvU32, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __objCreate_KernelChannelGroupApi(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams) \
    __nvoc_objCreate_KernelChannelGroupApi((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pCallContext, pParams)


// Wrapper macros for implementation functions
NV_STATUS kchangrpapiConstruct_IMPL(struct KernelChannelGroupApi *pKernelChannelGroupApi, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __nvoc_kchangrpapiConstruct(pKernelChannelGroupApi, pCallContext, pParams) kchangrpapiConstruct_IMPL(pKernelChannelGroupApi, pCallContext, pParams)

NV_STATUS kchangrpapiCopyConstruct_IMPL(struct KernelChannelGroupApi *pKernelChannelGroupApi, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#ifdef __nvoc_kernel_channel_group_api_h_disabled
static inline NV_STATUS kchangrpapiCopyConstruct(struct KernelChannelGroupApi *pKernelChannelGroupApi, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannelGroupApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_channel_group_api_h_disabled
#define kchangrpapiCopyConstruct(pKernelChannelGroupApi, pCallContext, pParams) kchangrpapiCopyConstruct_IMPL(pKernelChannelGroupApi, pCallContext, pParams)
#endif // __nvoc_kernel_channel_group_api_h_disabled

void kchangrpapiDestruct_IMPL(struct KernelChannelGroupApi *pKernelChannelGroupApi);
#define __nvoc_kchangrpapiDestruct(pKernelChannelGroupApi) kchangrpapiDestruct_IMPL(pKernelChannelGroupApi)

NV_STATUS kchangrpapiSetLegacyMode_IMPL(struct KernelChannelGroupApi *pKernelChannelGroupApi, struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvHandle hClient);
#ifdef __nvoc_kernel_channel_group_api_h_disabled
static inline NV_STATUS kchangrpapiSetLegacyMode(struct KernelChannelGroupApi *pKernelChannelGroupApi, struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvHandle hClient) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannelGroupApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_channel_group_api_h_disabled
#define kchangrpapiSetLegacyMode(pKernelChannelGroupApi, pGpu, pKernelFifo, hClient) kchangrpapiSetLegacyMode_IMPL(pKernelChannelGroupApi, pGpu, pKernelFifo, hClient)
#endif // __nvoc_kernel_channel_group_api_h_disabled

NV_STATUS kchangrpapiCtrlCmdGpFifoSchedule_IMPL(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_GPFIFO_SCHEDULE_PARAMS *pSchedParams);
#ifdef __nvoc_kernel_channel_group_api_h_disabled
static inline NV_STATUS kchangrpapiCtrlCmdGpFifoSchedule(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_GPFIFO_SCHEDULE_PARAMS *pSchedParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannelGroupApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_channel_group_api_h_disabled
#define kchangrpapiCtrlCmdGpFifoSchedule(pKernelChannelGroupApi, pSchedParams) kchangrpapiCtrlCmdGpFifoSchedule_IMPL(pKernelChannelGroupApi, pSchedParams)
#endif // __nvoc_kernel_channel_group_api_h_disabled

NV_STATUS kchangrpapiCtrlCmdBind_IMPL(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_BIND_PARAMS *pParams);
#ifdef __nvoc_kernel_channel_group_api_h_disabled
static inline NV_STATUS kchangrpapiCtrlCmdBind(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_BIND_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannelGroupApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_channel_group_api_h_disabled
#define kchangrpapiCtrlCmdBind(pKernelChannelGroupApi, pParams) kchangrpapiCtrlCmdBind_IMPL(pKernelChannelGroupApi, pParams)
#endif // __nvoc_kernel_channel_group_api_h_disabled

NV_STATUS kchangrpapiCtrlCmdSetTimeslice_IMPL(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_TIMESLICE_PARAMS *pTsParams);
#ifdef __nvoc_kernel_channel_group_api_h_disabled
static inline NV_STATUS kchangrpapiCtrlCmdSetTimeslice(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_TIMESLICE_PARAMS *pTsParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannelGroupApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_channel_group_api_h_disabled
#define kchangrpapiCtrlCmdSetTimeslice(pKernelChannelGroupApi, pTsParams) kchangrpapiCtrlCmdSetTimeslice_IMPL(pKernelChannelGroupApi, pTsParams)
#endif // __nvoc_kernel_channel_group_api_h_disabled

NV_STATUS kchangrpapiCtrlCmdGetTimeslice_IMPL(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_TIMESLICE_PARAMS *pTsParams);
#ifdef __nvoc_kernel_channel_group_api_h_disabled
static inline NV_STATUS kchangrpapiCtrlCmdGetTimeslice(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_TIMESLICE_PARAMS *pTsParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannelGroupApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_channel_group_api_h_disabled
#define kchangrpapiCtrlCmdGetTimeslice(pKernelChannelGroupApi, pTsParams) kchangrpapiCtrlCmdGetTimeslice_IMPL(pKernelChannelGroupApi, pTsParams)
#endif // __nvoc_kernel_channel_group_api_h_disabled

NV_STATUS kchangrpapiCtrlCmdPreempt_IMPL(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_PREEMPT_PARAMS *pPreemptParams);
#ifdef __nvoc_kernel_channel_group_api_h_disabled
static inline NV_STATUS kchangrpapiCtrlCmdPreempt(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_PREEMPT_PARAMS *pPreemptParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannelGroupApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_channel_group_api_h_disabled
#define kchangrpapiCtrlCmdPreempt(pKernelChannelGroupApi, pPreemptParams) kchangrpapiCtrlCmdPreempt_IMPL(pKernelChannelGroupApi, pPreemptParams)
#endif // __nvoc_kernel_channel_group_api_h_disabled

NV_STATUS kchangrpapiCtrlCmdGetInfo_IMPL(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_GET_INFO_PARAMS *pParams);
#ifdef __nvoc_kernel_channel_group_api_h_disabled
static inline NV_STATUS kchangrpapiCtrlCmdGetInfo(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_GET_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannelGroupApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_channel_group_api_h_disabled
#define kchangrpapiCtrlCmdGetInfo(pKernelChannelGroupApi, pParams) kchangrpapiCtrlCmdGetInfo_IMPL(pKernelChannelGroupApi, pParams)
#endif // __nvoc_kernel_channel_group_api_h_disabled

NV_STATUS kchangrpapiCtrlCmdSetInterleaveLevel_IMPL(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_INTERLEAVE_LEVEL_PARAMS *pParams);
#ifdef __nvoc_kernel_channel_group_api_h_disabled
static inline NV_STATUS kchangrpapiCtrlCmdSetInterleaveLevel(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_INTERLEAVE_LEVEL_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannelGroupApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_channel_group_api_h_disabled
#define kchangrpapiCtrlCmdSetInterleaveLevel(pKernelChannelGroupApi, pParams) kchangrpapiCtrlCmdSetInterleaveLevel_IMPL(pKernelChannelGroupApi, pParams)
#endif // __nvoc_kernel_channel_group_api_h_disabled

NV_STATUS kchangrpapiCtrlCmdProgramVidmemPromote_IMPL(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_PROGRAM_VIDMEM_PROMOTE_PARAMS *pParams);
#ifdef __nvoc_kernel_channel_group_api_h_disabled
static inline NV_STATUS kchangrpapiCtrlCmdProgramVidmemPromote(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_PROGRAM_VIDMEM_PROMOTE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannelGroupApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_channel_group_api_h_disabled
#define kchangrpapiCtrlCmdProgramVidmemPromote(pKernelChannelGroupApi, pParams) kchangrpapiCtrlCmdProgramVidmemPromote_IMPL(pKernelChannelGroupApi, pParams)
#endif // __nvoc_kernel_channel_group_api_h_disabled

NV_STATUS kchangrpapiCtrlCmdInternalPromoteFaultMethodBuffers_IMPL(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_INTERNAL_PROMOTE_FAULT_METHOD_BUFFERS_PARAMS *pParams);
#ifdef __nvoc_kernel_channel_group_api_h_disabled
static inline NV_STATUS kchangrpapiCtrlCmdInternalPromoteFaultMethodBuffers(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_INTERNAL_PROMOTE_FAULT_METHOD_BUFFERS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannelGroupApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_channel_group_api_h_disabled
#define kchangrpapiCtrlCmdInternalPromoteFaultMethodBuffers(pKernelChannelGroupApi, pParams) kchangrpapiCtrlCmdInternalPromoteFaultMethodBuffers_IMPL(pKernelChannelGroupApi, pParams)
#endif // __nvoc_kernel_channel_group_api_h_disabled

NV_STATUS kchangrpapiCtrlCmdMakeRealtime_IMPL(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_MAKE_REALTIME_PARAMS *pParams);
#ifdef __nvoc_kernel_channel_group_api_h_disabled
static inline NV_STATUS kchangrpapiCtrlCmdMakeRealtime(struct KernelChannelGroupApi *pKernelChannelGroupApi, NVA06C_CTRL_MAKE_REALTIME_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelChannelGroupApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_channel_group_api_h_disabled
#define kchangrpapiCtrlCmdMakeRealtime(pKernelChannelGroupApi, pParams) kchangrpapiCtrlCmdMakeRealtime_IMPL(pKernelChannelGroupApi, pParams)
#endif // __nvoc_kernel_channel_group_api_h_disabled


// Wrapper macros for halified functions
#define kchangrpapiCanCopy_FNPTR(pKernelChannelGroupApi) pKernelChannelGroupApi->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define kchangrpapiCanCopy(pKernelChannelGroupApi) kchangrpapiCanCopy_DISPATCH(pKernelChannelGroupApi)
#define kchangrpapiControl_FNPTR(pKernelChannelGroupApi) pKernelChannelGroupApi->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__
#define kchangrpapiControl(pKernelChannelGroupApi, pCallContext, pParams) kchangrpapiControl_DISPATCH(pKernelChannelGroupApi, pCallContext, pParams)
#define kchangrpapiMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__
#define kchangrpapiMap(pGpuResource, pCallContext, pParams, pCpuMapping) kchangrpapiMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define kchangrpapiUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__
#define kchangrpapiUnmap(pGpuResource, pCallContext, pCpuMapping) kchangrpapiUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define kchangrpapiShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__
#define kchangrpapiShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) kchangrpapiShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define kchangrpapiGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__
#define kchangrpapiGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) kchangrpapiGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define kchangrpapiGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__
#define kchangrpapiGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) kchangrpapiGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define kchangrpapiInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__
#define kchangrpapiInternalControlForward(pGpuResource, command, pParams, size) kchangrpapiInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define kchangrpapiGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__
#define kchangrpapiGetInternalObjectHandle(pGpuResource) kchangrpapiGetInternalObjectHandle_DISPATCH(pGpuResource)
#define kchangrpapiAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__
#define kchangrpapiAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) kchangrpapiAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define kchangrpapiGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define kchangrpapiGetMemInterMapParams(pRmResource, pParams) kchangrpapiGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define kchangrpapiCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define kchangrpapiCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) kchangrpapiCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define kchangrpapiGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define kchangrpapiGetMemoryMappingDescriptor(pRmResource, ppMemDesc) kchangrpapiGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define kchangrpapiControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__
#define kchangrpapiControlSerialization_Prologue(pResource, pCallContext, pParams) kchangrpapiControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define kchangrpapiControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__
#define kchangrpapiControlSerialization_Epilogue(pResource, pCallContext, pParams) kchangrpapiControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define kchangrpapiControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__
#define kchangrpapiControl_Prologue(pResource, pCallContext, pParams) kchangrpapiControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define kchangrpapiControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__
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
    return pKernelChannelGroupApi->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__(staticCast(pKernelChannelGroupApi, RsResource));
}

static inline NV_STATUS kchangrpapiControl_DISPATCH(struct KernelChannelGroupApi *pKernelChannelGroupApi, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pKernelChannelGroupApi->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__(staticCast(pKernelChannelGroupApi, RsResource), pCallContext, pParams);
}

static inline NV_STATUS kchangrpapiMap_DISPATCH(struct KernelChannelGroupApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__(staticCast(pGpuResource, RsResource), pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS kchangrpapiUnmap_DISPATCH(struct KernelChannelGroupApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__(staticCast(pGpuResource, RsResource), pCallContext, pCpuMapping);
}

static inline NvBool kchangrpapiShareCallback_DISPATCH(struct KernelChannelGroupApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__(staticCast(pGpuResource, RsResource), pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS kchangrpapiGetRegBaseOffsetAndSize_DISPATCH(struct KernelChannelGroupApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__(staticCast(pGpuResource, GpuResource), pGpu, pOffset, pSize);
}

static inline NV_STATUS kchangrpapiGetMapAddrSpace_DISPATCH(struct KernelChannelGroupApi *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__(staticCast(pGpuResource, GpuResource), pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS kchangrpapiInternalControlForward_DISPATCH(struct KernelChannelGroupApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__(staticCast(pGpuResource, GpuResource), command, pParams, size);
}

static inline NvHandle kchangrpapiGetInternalObjectHandle_DISPATCH(struct KernelChannelGroupApi *pGpuResource) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__(staticCast(pGpuResource, GpuResource));
}

static inline NvBool kchangrpapiAccessCallback_DISPATCH(struct KernelChannelGroupApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__(staticCast(pResource, RsResource), pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS kchangrpapiGetMemInterMapParams_DISPATCH(struct KernelChannelGroupApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__(staticCast(pRmResource, RmResource), pParams);
}

static inline NV_STATUS kchangrpapiCheckMemInterUnmap_DISPATCH(struct KernelChannelGroupApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__(staticCast(pRmResource, RmResource), bSubdeviceHandleProvided);
}

static inline NV_STATUS kchangrpapiGetMemoryMappingDescriptor_DISPATCH(struct KernelChannelGroupApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__(staticCast(pRmResource, RmResource), ppMemDesc);
}

static inline NV_STATUS kchangrpapiControlSerialization_Prologue_DISPATCH(struct KernelChannelGroupApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void kchangrpapiControlSerialization_Epilogue_DISPATCH(struct KernelChannelGroupApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS kchangrpapiControl_Prologue_DISPATCH(struct KernelChannelGroupApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void kchangrpapiControl_Epilogue_DISPATCH(struct KernelChannelGroupApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS kchangrpapiIsDuplicate_DISPATCH(struct KernelChannelGroupApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__(staticCast(pResource, RsResource), hMemory, pDuplicate);
}

static inline void kchangrpapiPreDestruct_DISPATCH(struct KernelChannelGroupApi *pResource) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__(staticCast(pResource, RsResource));
}

static inline NV_STATUS kchangrpapiControlFilter_DISPATCH(struct KernelChannelGroupApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool kchangrpapiIsPartialUnmapSupported_DISPATCH(struct KernelChannelGroupApi *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__(staticCast(pResource, RsResource));
}

static inline NV_STATUS kchangrpapiMapTo_DISPATCH(struct KernelChannelGroupApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__(staticCast(pResource, RsResource), pParams);
}

static inline NV_STATUS kchangrpapiUnmapFrom_DISPATCH(struct KernelChannelGroupApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__(staticCast(pResource, RsResource), pParams);
}

static inline NvU32 kchangrpapiGetRefCount_DISPATCH(struct KernelChannelGroupApi *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__(staticCast(pResource, RsResource));
}

static inline void kchangrpapiAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct KernelChannelGroupApi *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__(pClient, staticCast(pResource, RsResource), pReference);
}

// Virtual method declarations and/or inline definitions
NvBool kchangrpapiCanCopy_IMPL(struct KernelChannelGroupApi *pKernelChannelGroupApi);

NV_STATUS kchangrpapiControl_IMPL(struct KernelChannelGroupApi *pKernelChannelGroupApi, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);

// Exported method declarations and/or inline definitions
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

static inline NV_STATUS kchangrpapiCtrlGetTpcPartitionMode_a094e1(struct KernelChannelGroupApi *pKernelChannelGroupApi, NV0090_CTRL_TPC_PARTITION_MODE_PARAMS *pParams){
    return kgrctxCtrlHandle(resservGetTlsCallContext(), pKernelChannelGroupApi->hKernelGraphicsContext);
}

static inline NV_STATUS kchangrpapiCtrlSetTpcPartitionMode_a094e1(struct KernelChannelGroupApi *pKernelChannelGroupApi, NV0090_CTRL_TPC_PARTITION_MODE_PARAMS *pParams){
    return kgrctxCtrlHandle(resservGetTlsCallContext(), pKernelChannelGroupApi->hKernelGraphicsContext);
}

static inline NV_STATUS kchangrpapiCtrlGetMMUDebugMode_a094e1(struct KernelChannelGroupApi *pKernelChannelGroupApi, NV0090_CTRL_GET_MMU_DEBUG_MODE_PARAMS *pParams){
    return kgrctxCtrlHandle(resservGetTlsCallContext(), pKernelChannelGroupApi->hKernelGraphicsContext);
}

static inline NV_STATUS kchangrpapiCtrlProgramVidmemPromote_a094e1(struct KernelChannelGroupApi *pKernelChannelGroupApi, NV0090_CTRL_PROGRAM_VIDMEM_PROMOTE_PARAMS *pParams){
    return kgrctxCtrlHandle(resservGetTlsCallContext(), pKernelChannelGroupApi->hKernelGraphicsContext);
}

static inline NV_STATUS kchangrpapiCtrlProgramSysmemPromote_a094e1(struct KernelChannelGroupApi *pKernelChannelGroupApi, NV0090_CTRL_PROGRAM_SYSMEM_PROMOTE_PARAMS *pParams){
    return kgrctxCtrlHandle(resservGetTlsCallContext(), pKernelChannelGroupApi->hKernelGraphicsContext);
}

static inline NV_STATUS kchangrpapiCtrlSetLgSectorPromotion_a094e1(struct KernelChannelGroupApi *pKernelChannelGroupApi, NV0090_CTRL_SET_LG_SECTOR_PROMOTION_PARAMS *pParams){
    return kgrctxCtrlHandle(resservGetTlsCallContext(), pKernelChannelGroupApi->hKernelGraphicsContext);
}

// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD



NV_STATUS CliGetChannelGroup(struct RsClient*, NvHandle, RsResourceRef**, NvHandle*);

#endif // KERNEL_CHANNEL_GROUP_API_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_CHANNEL_GROUP_API_NVOC_H_
