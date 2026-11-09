
#ifndef _G_KERNEL_CTXSHARE_NVOC_H_
#define _G_KERNEL_CTXSHARE_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2016-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_kernel_ctxshare_nvoc.h"

#ifndef CTXSHARE_H
#define CTXSHARE_H

#include "resserv/resserv.h"
#include "nvoc/prelude.h"
#include "resserv/rs_resource.h"
#include "gpu/gpu_resource.h"
#include "mem_mgr/vaspace.h"
#include "resserv/rs_server.h"
#include "kernel/gpu/fifo/kernel_channel_group_api.h"

#include "ctrl/ctrl9067.h"


#include "containers/btree.h"

// Forward declaration
struct OBJGPU;

#ifndef __nvoc_class_id_OBJGPU
#define __nvoc_class_id_OBJGPU 0x7ef3cbu
typedef struct OBJGPU OBJGPU;
#endif /* __nvoc_class_id_OBJGPU */


struct KernelCtxShareApi;

#ifndef __nvoc_class_id_KernelCtxShareApi
#define __nvoc_class_id_KernelCtxShareApi 0x1f9af1u
typedef struct KernelCtxShareApi KernelCtxShareApi;
#endif /* __nvoc_class_id_KernelCtxShareApi */


struct HANDLE_VASPACE;

#ifndef __nvoc_class_id_HANDLE_VASPACE
#define __nvoc_class_id_HANDLE_VASPACE 0x388e77u
typedef struct HANDLE_VASPACE HANDLE_VASPACE;
#endif /* __nvoc_class_id_HANDLE_VASPACE */



/**
 * This class represents data that is shared when a subcontext is duped.
 *
 * Instances of this class are ref-counted and will be kept alive until
 * all subcontext copies have been freed by the resource server.
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_KERNEL_CTXSHARE_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__KernelCtxShare;
struct NVOC_METADATA__RsShared;


struct KernelCtxShare {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__KernelCtxShare *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct RsShared __nvoc_base_RsShared;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct RsShared *__nvoc_pbase_RsShared;    // shr super
    struct KernelCtxShare *__nvoc_pbase_KernelCtxShare;    // kctxshare

    // Data members
    struct OBJVASPACE *pVAS;
    struct HANDLE_VASPACE *pHandleVAS;
    struct KernelChannelGroup *pKernelChannelGroup;
    NvU32 subctxId;
    NvU32 vaSpaceId;
    NvU32 flags;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__KernelCtxShare {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__RsShared metadata__RsShared;
};

#ifndef __nvoc_class_id_KernelCtxShare
#define __nvoc_class_id_KernelCtxShare 0x5ae2feu
typedef struct KernelCtxShare KernelCtxShare;
#endif /* __nvoc_class_id_KernelCtxShare */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelCtxShare;

#define __staticCast_KernelCtxShare(pThis) \
    ((pThis)->__nvoc_pbase_KernelCtxShare)

#ifdef __nvoc_kernel_ctxshare_h_disabled
#define __dynamicCast_KernelCtxShare(pThis) ((KernelCtxShare*) NULL)
#else //__nvoc_kernel_ctxshare_h_disabled
#define __dynamicCast_KernelCtxShare(pThis) \
    ((KernelCtxShare*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelCtxShare)))
#endif //__nvoc_kernel_ctxshare_h_disabled

NV_STATUS __nvoc_objCreateDynamic_KernelCtxShare(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelCtxShare(KernelCtxShare**, Dynamic*, NvU32);
#define __objCreate_KernelCtxShare(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags) \
    __nvoc_objCreate_KernelCtxShare((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags))


// Wrapper macros for implementation functions
NV_STATUS kctxshareConstruct_IMPL(struct KernelCtxShare *pKernelCtxShare);
#define __nvoc_kctxshareConstruct(pKernelCtxShare) kctxshareConstruct_IMPL(pKernelCtxShare)

NV_STATUS kctxshareInitCommon_IMPL(struct KernelCtxShare *pKernelCtxShare, struct KernelCtxShareApi *pKernelCtxShareApi, struct OBJGPU *pGpu, struct OBJVASPACE *pVAS, struct HANDLE_VASPACE *pHandleVAS, NvU32 Flags, NvU32 *pSubctxId, struct KernelChannelGroupApi *pKernelChannelGroupApi);
#ifdef __nvoc_kernel_ctxshare_h_disabled
static inline NV_STATUS kctxshareInitCommon(struct KernelCtxShare *pKernelCtxShare, struct KernelCtxShareApi *pKernelCtxShareApi, struct OBJGPU *pGpu, struct OBJVASPACE *pVAS, struct HANDLE_VASPACE *pHandleVAS, NvU32 Flags, NvU32 *pSubctxId, struct KernelChannelGroupApi *pKernelChannelGroupApi) {
    NV_ASSERT_FAILED_PRECOMP("KernelCtxShare was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_ctxshare_h_disabled
#define kctxshareInitCommon(pKernelCtxShare, pKernelCtxShareApi, pGpu, pVAS, pHandleVAS, Flags, pSubctxId, pKernelChannelGroupApi) kctxshareInitCommon_IMPL(pKernelCtxShare, pKernelCtxShareApi, pGpu, pVAS, pHandleVAS, Flags, pSubctxId, pKernelChannelGroupApi)
#endif // __nvoc_kernel_ctxshare_h_disabled

NV_STATUS kctxshareDestroyCommon_IMPL(struct KernelCtxShare *pKernelCtxShare, struct KernelCtxShareApi *pKernelCtxShareApi, struct OBJGPU *pGpu, struct KernelChannelGroupApi *pKernelChannelGroupApi);
#ifdef __nvoc_kernel_ctxshare_h_disabled
static inline NV_STATUS kctxshareDestroyCommon(struct KernelCtxShare *pKernelCtxShare, struct KernelCtxShareApi *pKernelCtxShareApi, struct OBJGPU *pGpu, struct KernelChannelGroupApi *pKernelChannelGroupApi) {
    NV_ASSERT_FAILED_PRECOMP("KernelCtxShare was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_ctxshare_h_disabled
#define kctxshareDestroyCommon(pKernelCtxShare, pKernelCtxShareApi, pGpu, pKernelChannelGroupApi) kctxshareDestroyCommon_IMPL(pKernelCtxShare, pKernelCtxShareApi, pGpu, pKernelChannelGroupApi)
#endif // __nvoc_kernel_ctxshare_h_disabled

void kctxshareDestruct_IMPL(struct KernelCtxShare *pKernelCtxShare);
#define __nvoc_kctxshareDestruct(pKernelCtxShare) kctxshareDestruct_IMPL(pKernelCtxShare)

#ifdef __nvoc_kernel_ctxshare_h_disabled
static inline NV_STATUS kctxshareInit(struct KernelCtxShare *pKernelCtxShare, struct KernelCtxShareApi *pKernelCtxShareApi, struct OBJGPU *pGpu, struct OBJVASPACE *pVAS, struct KernelChannelGroupApi *pKernelChannelGroupApi, NvU64 offset, EMEMBLOCK *pBlock) {
    NV_ASSERT_FAILED_PRECOMP("KernelCtxShare was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_ctxshare_h_disabled
#define kctxshareInit(pKernelCtxShare, pKernelCtxShareApi, pGpu, pVAS, pKernelChannelGroupApi, offset, pBlock) kctxshareInit_ac1694(pKernelCtxShare, pKernelCtxShareApi, pGpu, pVAS, pKernelChannelGroupApi, offset, pBlock)
#endif // __nvoc_kernel_ctxshare_h_disabled

#ifdef __nvoc_kernel_ctxshare_h_disabled
static inline NV_STATUS kctxshareDestroy(struct KernelCtxShare *pKernelCtxShare, struct KernelCtxShareApi *pKernelCtxShareApi, struct OBJGPU *pGpu, struct KernelChannelGroupApi *pKernelChannelGroupApi, NvBool bRelease) {
    NV_ASSERT_FAILED_PRECOMP("KernelCtxShare was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_ctxshare_h_disabled
#define kctxshareDestroy(pKernelCtxShare, pKernelCtxShareApi, pGpu, pKernelChannelGroupApi, bRelease) kctxshareDestroy_ac1694(pKernelCtxShare, pKernelCtxShareApi, pGpu, pKernelChannelGroupApi, bRelease)
#endif // __nvoc_kernel_ctxshare_h_disabled


// Wrapper macros for halified functions
#define kctxshareInit_HAL(pKernelCtxShare, pKernelCtxShareApi, pGpu, pVAS, pKernelChannelGroupApi, offset, pBlock) kctxshareInit(pKernelCtxShare, pKernelCtxShareApi, pGpu, pVAS, pKernelChannelGroupApi, offset, pBlock)
#define kctxshareDestroy_HAL(pKernelCtxShare, pKernelCtxShareApi, pGpu, pKernelChannelGroupApi, bRelease) kctxshareDestroy(pKernelCtxShare, pKernelCtxShareApi, pGpu, pKernelChannelGroupApi, bRelease)

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
static inline NV_STATUS kctxshareInit_ac1694(struct KernelCtxShare *pKernelCtxShare, struct KernelCtxShareApi *pKernelCtxShareApi, struct OBJGPU *pGpu, struct OBJVASPACE *pVAS, struct KernelChannelGroupApi *pKernelChannelGroupApi, NvU64 offset, EMEMBLOCK *pBlock){
    return NV_OK;
}

static inline NV_STATUS kctxshareDestroy_ac1694(struct KernelCtxShare *pKernelCtxShare, struct KernelCtxShareApi *pKernelCtxShareApi, struct OBJGPU *pGpu, struct KernelChannelGroupApi *pKernelChannelGroupApi, NvBool bRelease){
    return NV_OK;
}

// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD



// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_KERNEL_CTXSHARE_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__KernelCtxShareApi;
struct NVOC_METADATA__GpuResource;


struct KernelCtxShareApi {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__KernelCtxShareApi *__nvoc_metadata_ptr;
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
    struct KernelCtxShareApi *__nvoc_pbase_KernelCtxShareApi;    // kctxshareapi

    // Data members
    struct KernelCtxShare *pShareData;
    NvHandle hVASpace;
    NvHandle hHandleVASpace;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__KernelCtxShareApi {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResource metadata__GpuResource;
};

#ifndef __nvoc_class_id_KernelCtxShareApi
#define __nvoc_class_id_KernelCtxShareApi 0x1f9af1u
typedef struct KernelCtxShareApi KernelCtxShareApi;
#endif /* __nvoc_class_id_KernelCtxShareApi */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelCtxShareApi;

#define __staticCast_KernelCtxShareApi(pThis) \
    ((pThis)->__nvoc_pbase_KernelCtxShareApi)

#ifdef __nvoc_kernel_ctxshare_h_disabled
#define __dynamicCast_KernelCtxShareApi(pThis) ((KernelCtxShareApi*) NULL)
#else //__nvoc_kernel_ctxshare_h_disabled
#define __dynamicCast_KernelCtxShareApi(pThis) \
    ((KernelCtxShareApi*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelCtxShareApi)))
#endif //__nvoc_kernel_ctxshare_h_disabled

NV_STATUS __nvoc_objCreateDynamic_KernelCtxShareApi(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelCtxShareApi(KernelCtxShareApi**, Dynamic*, NvU32, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __objCreate_KernelCtxShareApi(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams) \
    __nvoc_objCreate_KernelCtxShareApi((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pCallContext, pParams)


// Wrapper macros for implementation functions
NV_STATUS kctxshareapiConstruct_IMPL(struct KernelCtxShareApi *pKernelCtxShareApi, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __nvoc_kctxshareapiConstruct(pKernelCtxShareApi, pCallContext, pParams) kctxshareapiConstruct_IMPL(pKernelCtxShareApi, pCallContext, pParams)

NV_STATUS kctxshareapiCopyConstruct_IMPL(struct KernelCtxShareApi *pKernelCtxShareApi, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#ifdef __nvoc_kernel_ctxshare_h_disabled
static inline NV_STATUS kctxshareapiCopyConstruct(struct KernelCtxShareApi *pKernelCtxShareApi, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelCtxShareApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_ctxshare_h_disabled
#define kctxshareapiCopyConstruct(pKernelCtxShareApi, pCallContext, pParams) kctxshareapiCopyConstruct_IMPL(pKernelCtxShareApi, pCallContext, pParams)
#endif // __nvoc_kernel_ctxshare_h_disabled

void kctxshareapiDestruct_IMPL(struct KernelCtxShareApi *pKernelCtxShareApi);
#define __nvoc_kctxshareapiDestruct(pKernelCtxShareApi) kctxshareapiDestruct_IMPL(pKernelCtxShareApi)

NV_STATUS kctxshareapiCtrlCmdSetTpcPartitionTable_IMPL(struct KernelCtxShareApi *pKernelCtxShareApi, NV9067_CTRL_TPC_PARTITION_TABLE_PARAMS *pParams);
#ifdef __nvoc_kernel_ctxshare_h_disabled
static inline NV_STATUS kctxshareapiCtrlCmdSetTpcPartitionTable(struct KernelCtxShareApi *pKernelCtxShareApi, NV9067_CTRL_TPC_PARTITION_TABLE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelCtxShareApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_ctxshare_h_disabled
#define kctxshareapiCtrlCmdSetTpcPartitionTable(pKernelCtxShareApi, pParams) kctxshareapiCtrlCmdSetTpcPartitionTable_IMPL(pKernelCtxShareApi, pParams)
#endif // __nvoc_kernel_ctxshare_h_disabled

NV_STATUS kctxshareapiCtrlCmdGetCwdWatermark_IMPL(struct KernelCtxShareApi *pKernelCtxShareApi, NV9067_CTRL_CWD_WATERMARK_PARAMS *pParams);
#ifdef __nvoc_kernel_ctxshare_h_disabled
static inline NV_STATUS kctxshareapiCtrlCmdGetCwdWatermark(struct KernelCtxShareApi *pKernelCtxShareApi, NV9067_CTRL_CWD_WATERMARK_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelCtxShareApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_ctxshare_h_disabled
#define kctxshareapiCtrlCmdGetCwdWatermark(pKernelCtxShareApi, pParams) kctxshareapiCtrlCmdGetCwdWatermark_IMPL(pKernelCtxShareApi, pParams)
#endif // __nvoc_kernel_ctxshare_h_disabled

NV_STATUS kctxshareapiCtrlCmdSetCwdWatermark_IMPL(struct KernelCtxShareApi *pKernelCtxShareApi, NV9067_CTRL_CWD_WATERMARK_PARAMS *pParams);
#ifdef __nvoc_kernel_ctxshare_h_disabled
static inline NV_STATUS kctxshareapiCtrlCmdSetCwdWatermark(struct KernelCtxShareApi *pKernelCtxShareApi, NV9067_CTRL_CWD_WATERMARK_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelCtxShareApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kernel_ctxshare_h_disabled
#define kctxshareapiCtrlCmdSetCwdWatermark(pKernelCtxShareApi, pParams) kctxshareapiCtrlCmdSetCwdWatermark_IMPL(pKernelCtxShareApi, pParams)
#endif // __nvoc_kernel_ctxshare_h_disabled


// Wrapper macros for halified functions
#define kctxshareapiCanCopy_FNPTR(pKernelCtxShareApi) pKernelCtxShareApi->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define kctxshareapiCanCopy(pKernelCtxShareApi) kctxshareapiCanCopy_DISPATCH(pKernelCtxShareApi)
#define kctxshareapiControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__
#define kctxshareapiControl(pGpuResource, pCallContext, pParams) kctxshareapiControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define kctxshareapiMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__
#define kctxshareapiMap(pGpuResource, pCallContext, pParams, pCpuMapping) kctxshareapiMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define kctxshareapiUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__
#define kctxshareapiUnmap(pGpuResource, pCallContext, pCpuMapping) kctxshareapiUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define kctxshareapiShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__
#define kctxshareapiShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) kctxshareapiShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define kctxshareapiGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__
#define kctxshareapiGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) kctxshareapiGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define kctxshareapiGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__
#define kctxshareapiGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) kctxshareapiGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define kctxshareapiInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__
#define kctxshareapiInternalControlForward(pGpuResource, command, pParams, size) kctxshareapiInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define kctxshareapiGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__
#define kctxshareapiGetInternalObjectHandle(pGpuResource) kctxshareapiGetInternalObjectHandle_DISPATCH(pGpuResource)
#define kctxshareapiAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__
#define kctxshareapiAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) kctxshareapiAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define kctxshareapiGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define kctxshareapiGetMemInterMapParams(pRmResource, pParams) kctxshareapiGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define kctxshareapiCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define kctxshareapiCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) kctxshareapiCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define kctxshareapiGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define kctxshareapiGetMemoryMappingDescriptor(pRmResource, ppMemDesc) kctxshareapiGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define kctxshareapiControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__
#define kctxshareapiControlSerialization_Prologue(pResource, pCallContext, pParams) kctxshareapiControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define kctxshareapiControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__
#define kctxshareapiControlSerialization_Epilogue(pResource, pCallContext, pParams) kctxshareapiControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define kctxshareapiControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__
#define kctxshareapiControl_Prologue(pResource, pCallContext, pParams) kctxshareapiControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define kctxshareapiControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__
#define kctxshareapiControl_Epilogue(pResource, pCallContext, pParams) kctxshareapiControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define kctxshareapiIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define kctxshareapiIsDuplicate(pResource, hMemory, pDuplicate) kctxshareapiIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define kctxshareapiPreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define kctxshareapiPreDestruct(pResource) kctxshareapiPreDestruct_DISPATCH(pResource)
#define kctxshareapiControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define kctxshareapiControlFilter(pResource, pCallContext, pParams) kctxshareapiControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define kctxshareapiIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define kctxshareapiIsPartialUnmapSupported(pResource) kctxshareapiIsPartialUnmapSupported_DISPATCH(pResource)
#define kctxshareapiMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define kctxshareapiMapTo(pResource, pParams) kctxshareapiMapTo_DISPATCH(pResource, pParams)
#define kctxshareapiUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define kctxshareapiUnmapFrom(pResource, pParams) kctxshareapiUnmapFrom_DISPATCH(pResource, pParams)
#define kctxshareapiGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define kctxshareapiGetRefCount(pResource) kctxshareapiGetRefCount_DISPATCH(pResource)
#define kctxshareapiAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define kctxshareapiAddAdditionalDependants(pClient, pResource, pReference) kctxshareapiAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NvBool kctxshareapiCanCopy_DISPATCH(struct KernelCtxShareApi *pKernelCtxShareApi) {
    return pKernelCtxShareApi->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__(staticCast(pKernelCtxShareApi, RsResource));
}

static inline NV_STATUS kctxshareapiControl_DISPATCH(struct KernelCtxShareApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__(staticCast(pGpuResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS kctxshareapiMap_DISPATCH(struct KernelCtxShareApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__(staticCast(pGpuResource, RsResource), pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS kctxshareapiUnmap_DISPATCH(struct KernelCtxShareApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__(staticCast(pGpuResource, RsResource), pCallContext, pCpuMapping);
}

static inline NvBool kctxshareapiShareCallback_DISPATCH(struct KernelCtxShareApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__(staticCast(pGpuResource, RsResource), pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS kctxshareapiGetRegBaseOffsetAndSize_DISPATCH(struct KernelCtxShareApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__(staticCast(pGpuResource, GpuResource), pGpu, pOffset, pSize);
}

static inline NV_STATUS kctxshareapiGetMapAddrSpace_DISPATCH(struct KernelCtxShareApi *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__(staticCast(pGpuResource, GpuResource), pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS kctxshareapiInternalControlForward_DISPATCH(struct KernelCtxShareApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__(staticCast(pGpuResource, GpuResource), command, pParams, size);
}

static inline NvHandle kctxshareapiGetInternalObjectHandle_DISPATCH(struct KernelCtxShareApi *pGpuResource) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__(staticCast(pGpuResource, GpuResource));
}

static inline NvBool kctxshareapiAccessCallback_DISPATCH(struct KernelCtxShareApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__(staticCast(pResource, RsResource), pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS kctxshareapiGetMemInterMapParams_DISPATCH(struct KernelCtxShareApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__(staticCast(pRmResource, RmResource), pParams);
}

static inline NV_STATUS kctxshareapiCheckMemInterUnmap_DISPATCH(struct KernelCtxShareApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__(staticCast(pRmResource, RmResource), bSubdeviceHandleProvided);
}

static inline NV_STATUS kctxshareapiGetMemoryMappingDescriptor_DISPATCH(struct KernelCtxShareApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__(staticCast(pRmResource, RmResource), ppMemDesc);
}

static inline NV_STATUS kctxshareapiControlSerialization_Prologue_DISPATCH(struct KernelCtxShareApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void kctxshareapiControlSerialization_Epilogue_DISPATCH(struct KernelCtxShareApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS kctxshareapiControl_Prologue_DISPATCH(struct KernelCtxShareApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void kctxshareapiControl_Epilogue_DISPATCH(struct KernelCtxShareApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS kctxshareapiIsDuplicate_DISPATCH(struct KernelCtxShareApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__(staticCast(pResource, RsResource), hMemory, pDuplicate);
}

static inline void kctxshareapiPreDestruct_DISPATCH(struct KernelCtxShareApi *pResource) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__(staticCast(pResource, RsResource));
}

static inline NV_STATUS kctxshareapiControlFilter_DISPATCH(struct KernelCtxShareApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool kctxshareapiIsPartialUnmapSupported_DISPATCH(struct KernelCtxShareApi *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__(staticCast(pResource, RsResource));
}

static inline NV_STATUS kctxshareapiMapTo_DISPATCH(struct KernelCtxShareApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__(staticCast(pResource, RsResource), pParams);
}

static inline NV_STATUS kctxshareapiUnmapFrom_DISPATCH(struct KernelCtxShareApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__(staticCast(pResource, RsResource), pParams);
}

static inline NvU32 kctxshareapiGetRefCount_DISPATCH(struct KernelCtxShareApi *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__(staticCast(pResource, RsResource));
}

static inline void kctxshareapiAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct KernelCtxShareApi *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__(pClient, staticCast(pResource, RsResource), pReference);
}

// Virtual method declarations and/or inline definitions
NvBool kctxshareapiCanCopy_IMPL(struct KernelCtxShareApi *pKernelCtxShareApi);

// Exported method declarations and/or inline definitions
NV_STATUS kctxshareapiCtrlCmdSetTpcPartitionTable_IMPL(struct KernelCtxShareApi *pKernelCtxShareApi, NV9067_CTRL_TPC_PARTITION_TABLE_PARAMS *pParams);

NV_STATUS kctxshareapiCtrlCmdGetCwdWatermark_IMPL(struct KernelCtxShareApi *pKernelCtxShareApi, NV9067_CTRL_CWD_WATERMARK_PARAMS *pParams);

NV_STATUS kctxshareapiCtrlCmdSetCwdWatermark_IMPL(struct KernelCtxShareApi *pKernelCtxShareApi, NV9067_CTRL_CWD_WATERMARK_PARAMS *pParams);

// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


#endif // CTXSHARE_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_CTXSHARE_NVOC_H_
