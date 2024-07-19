
#ifndef _G_KERNEL_CTXSHARE_NVOC_H_
#define _G_KERNEL_CTXSHARE_NVOC_H_
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

#ifndef __NVOC_CLASS_OBJGPU_TYPEDEF__
#define __NVOC_CLASS_OBJGPU_TYPEDEF__
typedef struct OBJGPU OBJGPU;
#endif /* __NVOC_CLASS_OBJGPU_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJGPU
#define __nvoc_class_id_OBJGPU 0x7ef3cb
#endif /* __nvoc_class_id_OBJGPU */



struct KernelCtxShareApi;

#ifndef __NVOC_CLASS_KernelCtxShareApi_TYPEDEF__
#define __NVOC_CLASS_KernelCtxShareApi_TYPEDEF__
typedef struct KernelCtxShareApi KernelCtxShareApi;
#endif /* __NVOC_CLASS_KernelCtxShareApi_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelCtxShareApi
#define __nvoc_class_id_KernelCtxShareApi 0x1f9af1
#endif /* __nvoc_class_id_KernelCtxShareApi */



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


struct KernelCtxShare {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct RsShared __nvoc_base_RsShared;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct RsShared *__nvoc_pbase_RsShared;    // shr super
    struct KernelCtxShare *__nvoc_pbase_KernelCtxShare;    // kctxshare

    // Data members
    struct OBJVASPACE *pVAS;
    struct KernelChannelGroup *pKernelChannelGroup;
    NvU32 subctxId;
    NvU32 flags;
};

#ifndef __NVOC_CLASS_KernelCtxShare_TYPEDEF__
#define __NVOC_CLASS_KernelCtxShare_TYPEDEF__
typedef struct KernelCtxShare KernelCtxShare;
#endif /* __NVOC_CLASS_KernelCtxShare_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelCtxShare
#define __nvoc_class_id_KernelCtxShare 0x5ae2fe
#endif /* __nvoc_class_id_KernelCtxShare */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelCtxShare;

#define __staticCast_KernelCtxShare(pThis) \
    ((pThis)->__nvoc_pbase_KernelCtxShare)

#ifdef __nvoc_kernel_ctxshare_h_disabled
#define __dynamicCast_KernelCtxShare(pThis) ((KernelCtxShare*)NULL)
#else //__nvoc_kernel_ctxshare_h_disabled
#define __dynamicCast_KernelCtxShare(pThis) \
    ((KernelCtxShare*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelCtxShare)))
#endif //__nvoc_kernel_ctxshare_h_disabled

NV_STATUS __nvoc_objCreateDynamic_KernelCtxShare(KernelCtxShare**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelCtxShare(KernelCtxShare**, Dynamic*, NvU32);
#define __objCreate_KernelCtxShare(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_KernelCtxShare((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros

// Dispatch functions
static inline NV_STATUS kctxshareInit_56cd7a(struct KernelCtxShare *pKernelCtxShare, struct KernelCtxShareApi *pKernelCtxShareApi, struct OBJGPU *pGpu, struct OBJVASPACE *pVAS, struct KernelChannelGroupApi *pKernelChannelGroupApi, NvU64 offset, EMEMBLOCK *pBlock) {
    return NV_OK;
}


#ifdef __nvoc_kernel_ctxshare_h_disabled
static inline NV_STATUS kctxshareInit(struct KernelCtxShare *pKernelCtxShare, struct KernelCtxShareApi *pKernelCtxShareApi, struct OBJGPU *pGpu, struct OBJVASPACE *pVAS, struct KernelChannelGroupApi *pKernelChannelGroupApi, NvU64 offset, EMEMBLOCK *pBlock) {
    NV_ASSERT_FAILED_PRECOMP("KernelCtxShare was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_ctxshare_h_disabled
#define kctxshareInit(pKernelCtxShare, pKernelCtxShareApi, pGpu, pVAS, pKernelChannelGroupApi, offset, pBlock) kctxshareInit_56cd7a(pKernelCtxShare, pKernelCtxShareApi, pGpu, pVAS, pKernelChannelGroupApi, offset, pBlock)
#endif //__nvoc_kernel_ctxshare_h_disabled

#define kctxshareInit_HAL(pKernelCtxShare, pKernelCtxShareApi, pGpu, pVAS, pKernelChannelGroupApi, offset, pBlock) kctxshareInit(pKernelCtxShare, pKernelCtxShareApi, pGpu, pVAS, pKernelChannelGroupApi, offset, pBlock)

static inline NV_STATUS kctxshareDestroy_56cd7a(struct KernelCtxShare *pKernelCtxShare, struct KernelCtxShareApi *pKernelCtxShareApi, struct OBJGPU *pGpu, struct KernelChannelGroupApi *pKernelChannelGroupApi, NvBool bRelease) {
    return NV_OK;
}


#ifdef __nvoc_kernel_ctxshare_h_disabled
static inline NV_STATUS kctxshareDestroy(struct KernelCtxShare *pKernelCtxShare, struct KernelCtxShareApi *pKernelCtxShareApi, struct OBJGPU *pGpu, struct KernelChannelGroupApi *pKernelChannelGroupApi, NvBool bRelease) {
    NV_ASSERT_FAILED_PRECOMP("KernelCtxShare was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_ctxshare_h_disabled
#define kctxshareDestroy(pKernelCtxShare, pKernelCtxShareApi, pGpu, pKernelChannelGroupApi, bRelease) kctxshareDestroy_56cd7a(pKernelCtxShare, pKernelCtxShareApi, pGpu, pKernelChannelGroupApi, bRelease)
#endif //__nvoc_kernel_ctxshare_h_disabled

#define kctxshareDestroy_HAL(pKernelCtxShare, pKernelCtxShareApi, pGpu, pKernelChannelGroupApi, bRelease) kctxshareDestroy(pKernelCtxShare, pKernelCtxShareApi, pGpu, pKernelChannelGroupApi, bRelease)

NV_STATUS kctxshareConstruct_IMPL(struct KernelCtxShare *arg_pKernelCtxShare);

#define __nvoc_kctxshareConstruct(arg_pKernelCtxShare) kctxshareConstruct_IMPL(arg_pKernelCtxShare)
NV_STATUS kctxshareInitCommon_IMPL(struct KernelCtxShare *pKernelCtxShare, struct KernelCtxShareApi *pKernelCtxShareApi, struct OBJGPU *pGpu, struct OBJVASPACE *pVAS, NvU32 Flags, NvU32 *pSubctxId, struct KernelChannelGroupApi *pKernelChannelGroupApi);

#ifdef __nvoc_kernel_ctxshare_h_disabled
static inline NV_STATUS kctxshareInitCommon(struct KernelCtxShare *pKernelCtxShare, struct KernelCtxShareApi *pKernelCtxShareApi, struct OBJGPU *pGpu, struct OBJVASPACE *pVAS, NvU32 Flags, NvU32 *pSubctxId, struct KernelChannelGroupApi *pKernelChannelGroupApi) {
    NV_ASSERT_FAILED_PRECOMP("KernelCtxShare was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_ctxshare_h_disabled
#define kctxshareInitCommon(pKernelCtxShare, pKernelCtxShareApi, pGpu, pVAS, Flags, pSubctxId, pKernelChannelGroupApi) kctxshareInitCommon_IMPL(pKernelCtxShare, pKernelCtxShareApi, pGpu, pVAS, Flags, pSubctxId, pKernelChannelGroupApi)
#endif //__nvoc_kernel_ctxshare_h_disabled

NV_STATUS kctxshareDestroyCommon_IMPL(struct KernelCtxShare *pKernelCtxShare, struct KernelCtxShareApi *pKernelCtxShareApi, struct OBJGPU *pGpu, struct KernelChannelGroupApi *pKernelChannelGroupApi);

#ifdef __nvoc_kernel_ctxshare_h_disabled
static inline NV_STATUS kctxshareDestroyCommon(struct KernelCtxShare *pKernelCtxShare, struct KernelCtxShareApi *pKernelCtxShareApi, struct OBJGPU *pGpu, struct KernelChannelGroupApi *pKernelChannelGroupApi) {
    NV_ASSERT_FAILED_PRECOMP("KernelCtxShare was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_ctxshare_h_disabled
#define kctxshareDestroyCommon(pKernelCtxShare, pKernelCtxShareApi, pGpu, pKernelChannelGroupApi) kctxshareDestroyCommon_IMPL(pKernelCtxShare, pKernelCtxShareApi, pGpu, pKernelChannelGroupApi)
#endif //__nvoc_kernel_ctxshare_h_disabled

void kctxshareDestruct_IMPL(struct KernelCtxShare *pKernelCtxShare);

#define __nvoc_kctxshareDestruct(pKernelCtxShare) kctxshareDestruct_IMPL(pKernelCtxShare)
#undef PRIVATE_FIELD



// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_KERNEL_CTXSHARE_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct KernelCtxShareApi {

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
    struct KernelCtxShareApi *__nvoc_pbase_KernelCtxShareApi;    // kctxshareapi

    // Vtable with 28 per-object function pointers
    NvBool (*__kctxshareapiCanCopy__)(struct KernelCtxShareApi * /*this*/);  // virtual override (res) base (gpures)
    NV_STATUS (*__kctxshareapiCtrlCmdSetTpcPartitionTable__)(struct KernelCtxShareApi * /*this*/, NV9067_CTRL_TPC_PARTITION_TABLE_PARAMS *);  // exported (id=0x90670102)
    NV_STATUS (*__kctxshareapiCtrlCmdGetCwdWatermark__)(struct KernelCtxShareApi * /*this*/, NV9067_CTRL_CWD_WATERMARK_PARAMS *);  // exported (id=0x90670201)
    NV_STATUS (*__kctxshareapiCtrlCmdSetCwdWatermark__)(struct KernelCtxShareApi * /*this*/, NV9067_CTRL_CWD_WATERMARK_PARAMS *);  // exported (id=0x90670202)
    NV_STATUS (*__kctxshareapiControl__)(struct KernelCtxShareApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__kctxshareapiMap__)(struct KernelCtxShareApi * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__kctxshareapiUnmap__)(struct KernelCtxShareApi * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NvBool (*__kctxshareapiShareCallback__)(struct KernelCtxShareApi * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__kctxshareapiGetRegBaseOffsetAndSize__)(struct KernelCtxShareApi * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__kctxshareapiGetMapAddrSpace__)(struct KernelCtxShareApi * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__kctxshareapiInternalControlForward__)(struct KernelCtxShareApi * /*this*/, NvU32, void *, NvU32);  // virtual inherited (gpures) base (gpures)
    NvHandle (*__kctxshareapiGetInternalObjectHandle__)(struct KernelCtxShareApi * /*this*/);  // virtual inherited (gpures) base (gpures)
    NvBool (*__kctxshareapiAccessCallback__)(struct KernelCtxShareApi * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__kctxshareapiGetMemInterMapParams__)(struct KernelCtxShareApi * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__kctxshareapiCheckMemInterUnmap__)(struct KernelCtxShareApi * /*this*/, NvBool);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__kctxshareapiGetMemoryMappingDescriptor__)(struct KernelCtxShareApi * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__kctxshareapiControlSerialization_Prologue__)(struct KernelCtxShareApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__kctxshareapiControlSerialization_Epilogue__)(struct KernelCtxShareApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__kctxshareapiControl_Prologue__)(struct KernelCtxShareApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__kctxshareapiControl_Epilogue__)(struct KernelCtxShareApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__kctxshareapiIsDuplicate__)(struct KernelCtxShareApi * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (gpures)
    void (*__kctxshareapiPreDestruct__)(struct KernelCtxShareApi * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__kctxshareapiControlFilter__)(struct KernelCtxShareApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (gpures)
    NvBool (*__kctxshareapiIsPartialUnmapSupported__)(struct KernelCtxShareApi * /*this*/);  // inline virtual inherited (res) base (gpures) body
    NV_STATUS (*__kctxshareapiMapTo__)(struct KernelCtxShareApi * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__kctxshareapiUnmapFrom__)(struct KernelCtxShareApi * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (gpures)
    NvU32 (*__kctxshareapiGetRefCount__)(struct KernelCtxShareApi * /*this*/);  // virtual inherited (res) base (gpures)
    void (*__kctxshareapiAddAdditionalDependants__)(struct RsClient *, struct KernelCtxShareApi * /*this*/, RsResourceRef *);  // virtual inherited (res) base (gpures)

    // Data members
    struct KernelCtxShare *pShareData;
    NvHandle hVASpace;
};

#ifndef __NVOC_CLASS_KernelCtxShareApi_TYPEDEF__
#define __NVOC_CLASS_KernelCtxShareApi_TYPEDEF__
typedef struct KernelCtxShareApi KernelCtxShareApi;
#endif /* __NVOC_CLASS_KernelCtxShareApi_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelCtxShareApi
#define __nvoc_class_id_KernelCtxShareApi 0x1f9af1
#endif /* __nvoc_class_id_KernelCtxShareApi */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelCtxShareApi;

#define __staticCast_KernelCtxShareApi(pThis) \
    ((pThis)->__nvoc_pbase_KernelCtxShareApi)

#ifdef __nvoc_kernel_ctxshare_h_disabled
#define __dynamicCast_KernelCtxShareApi(pThis) ((KernelCtxShareApi*)NULL)
#else //__nvoc_kernel_ctxshare_h_disabled
#define __dynamicCast_KernelCtxShareApi(pThis) \
    ((KernelCtxShareApi*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelCtxShareApi)))
#endif //__nvoc_kernel_ctxshare_h_disabled

NV_STATUS __nvoc_objCreateDynamic_KernelCtxShareApi(KernelCtxShareApi**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelCtxShareApi(KernelCtxShareApi**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_KernelCtxShareApi(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_KernelCtxShareApi((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define kctxshareapiCanCopy_FNPTR(pKernelCtxShareApi) pKernelCtxShareApi->__kctxshareapiCanCopy__
#define kctxshareapiCanCopy(pKernelCtxShareApi) kctxshareapiCanCopy_DISPATCH(pKernelCtxShareApi)
#define kctxshareapiCtrlCmdSetTpcPartitionTable_FNPTR(pKernelCtxShareApi) pKernelCtxShareApi->__kctxshareapiCtrlCmdSetTpcPartitionTable__
#define kctxshareapiCtrlCmdSetTpcPartitionTable(pKernelCtxShareApi, pParams) kctxshareapiCtrlCmdSetTpcPartitionTable_DISPATCH(pKernelCtxShareApi, pParams)
#define kctxshareapiCtrlCmdGetCwdWatermark_FNPTR(pKernelCtxShareApi) pKernelCtxShareApi->__kctxshareapiCtrlCmdGetCwdWatermark__
#define kctxshareapiCtrlCmdGetCwdWatermark(pKernelCtxShareApi, pParams) kctxshareapiCtrlCmdGetCwdWatermark_DISPATCH(pKernelCtxShareApi, pParams)
#define kctxshareapiCtrlCmdSetCwdWatermark_FNPTR(pKernelCtxShareApi) pKernelCtxShareApi->__kctxshareapiCtrlCmdSetCwdWatermark__
#define kctxshareapiCtrlCmdSetCwdWatermark(pKernelCtxShareApi, pParams) kctxshareapiCtrlCmdSetCwdWatermark_DISPATCH(pKernelCtxShareApi, pParams)
#define kctxshareapiControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresControl__
#define kctxshareapiControl(pGpuResource, pCallContext, pParams) kctxshareapiControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define kctxshareapiMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresMap__
#define kctxshareapiMap(pGpuResource, pCallContext, pParams, pCpuMapping) kctxshareapiMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define kctxshareapiUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresUnmap__
#define kctxshareapiUnmap(pGpuResource, pCallContext, pCpuMapping) kctxshareapiUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define kctxshareapiShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresShareCallback__
#define kctxshareapiShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) kctxshareapiShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define kctxshareapiGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetRegBaseOffsetAndSize__
#define kctxshareapiGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) kctxshareapiGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define kctxshareapiGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetMapAddrSpace__
#define kctxshareapiGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) kctxshareapiGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define kctxshareapiInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresInternalControlForward__
#define kctxshareapiInternalControlForward(pGpuResource, command, pParams, size) kctxshareapiInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define kctxshareapiGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetInternalObjectHandle__
#define kctxshareapiGetInternalObjectHandle(pGpuResource) kctxshareapiGetInternalObjectHandle_DISPATCH(pGpuResource)
#define kctxshareapiAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresAccessCallback__
#define kctxshareapiAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) kctxshareapiAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define kctxshareapiGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemInterMapParams__
#define kctxshareapiGetMemInterMapParams(pRmResource, pParams) kctxshareapiGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define kctxshareapiCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresCheckMemInterUnmap__
#define kctxshareapiCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) kctxshareapiCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define kctxshareapiGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemoryMappingDescriptor__
#define kctxshareapiGetMemoryMappingDescriptor(pRmResource, ppMemDesc) kctxshareapiGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define kctxshareapiControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Prologue__
#define kctxshareapiControlSerialization_Prologue(pResource, pCallContext, pParams) kctxshareapiControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define kctxshareapiControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Epilogue__
#define kctxshareapiControlSerialization_Epilogue(pResource, pCallContext, pParams) kctxshareapiControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define kctxshareapiControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Prologue__
#define kctxshareapiControl_Prologue(pResource, pCallContext, pParams) kctxshareapiControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define kctxshareapiControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Epilogue__
#define kctxshareapiControl_Epilogue(pResource, pCallContext, pParams) kctxshareapiControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define kctxshareapiIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsDuplicate__
#define kctxshareapiIsDuplicate(pResource, hMemory, pDuplicate) kctxshareapiIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define kctxshareapiPreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resPreDestruct__
#define kctxshareapiPreDestruct(pResource) kctxshareapiPreDestruct_DISPATCH(pResource)
#define kctxshareapiControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resControlFilter__
#define kctxshareapiControlFilter(pResource, pCallContext, pParams) kctxshareapiControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define kctxshareapiIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsPartialUnmapSupported__
#define kctxshareapiIsPartialUnmapSupported(pResource) kctxshareapiIsPartialUnmapSupported_DISPATCH(pResource)
#define kctxshareapiMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resMapTo__
#define kctxshareapiMapTo(pResource, pParams) kctxshareapiMapTo_DISPATCH(pResource, pParams)
#define kctxshareapiUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmapFrom__
#define kctxshareapiUnmapFrom(pResource, pParams) kctxshareapiUnmapFrom_DISPATCH(pResource, pParams)
#define kctxshareapiGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resGetRefCount__
#define kctxshareapiGetRefCount(pResource) kctxshareapiGetRefCount_DISPATCH(pResource)
#define kctxshareapiAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resAddAdditionalDependants__
#define kctxshareapiAddAdditionalDependants(pClient, pResource, pReference) kctxshareapiAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NvBool kctxshareapiCanCopy_DISPATCH(struct KernelCtxShareApi *pKernelCtxShareApi) {
    return pKernelCtxShareApi->__kctxshareapiCanCopy__(pKernelCtxShareApi);
}

static inline NV_STATUS kctxshareapiCtrlCmdSetTpcPartitionTable_DISPATCH(struct KernelCtxShareApi *pKernelCtxShareApi, NV9067_CTRL_TPC_PARTITION_TABLE_PARAMS *pParams) {
    return pKernelCtxShareApi->__kctxshareapiCtrlCmdSetTpcPartitionTable__(pKernelCtxShareApi, pParams);
}

static inline NV_STATUS kctxshareapiCtrlCmdGetCwdWatermark_DISPATCH(struct KernelCtxShareApi *pKernelCtxShareApi, NV9067_CTRL_CWD_WATERMARK_PARAMS *pParams) {
    return pKernelCtxShareApi->__kctxshareapiCtrlCmdGetCwdWatermark__(pKernelCtxShareApi, pParams);
}

static inline NV_STATUS kctxshareapiCtrlCmdSetCwdWatermark_DISPATCH(struct KernelCtxShareApi *pKernelCtxShareApi, NV9067_CTRL_CWD_WATERMARK_PARAMS *pParams) {
    return pKernelCtxShareApi->__kctxshareapiCtrlCmdSetCwdWatermark__(pKernelCtxShareApi, pParams);
}

static inline NV_STATUS kctxshareapiControl_DISPATCH(struct KernelCtxShareApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__kctxshareapiControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS kctxshareapiMap_DISPATCH(struct KernelCtxShareApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__kctxshareapiMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS kctxshareapiUnmap_DISPATCH(struct KernelCtxShareApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__kctxshareapiUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NvBool kctxshareapiShareCallback_DISPATCH(struct KernelCtxShareApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__kctxshareapiShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS kctxshareapiGetRegBaseOffsetAndSize_DISPATCH(struct KernelCtxShareApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__kctxshareapiGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS kctxshareapiGetMapAddrSpace_DISPATCH(struct KernelCtxShareApi *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__kctxshareapiGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS kctxshareapiInternalControlForward_DISPATCH(struct KernelCtxShareApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__kctxshareapiInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NvHandle kctxshareapiGetInternalObjectHandle_DISPATCH(struct KernelCtxShareApi *pGpuResource) {
    return pGpuResource->__kctxshareapiGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool kctxshareapiAccessCallback_DISPATCH(struct KernelCtxShareApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__kctxshareapiAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS kctxshareapiGetMemInterMapParams_DISPATCH(struct KernelCtxShareApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__kctxshareapiGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS kctxshareapiCheckMemInterUnmap_DISPATCH(struct KernelCtxShareApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__kctxshareapiCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS kctxshareapiGetMemoryMappingDescriptor_DISPATCH(struct KernelCtxShareApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__kctxshareapiGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS kctxshareapiControlSerialization_Prologue_DISPATCH(struct KernelCtxShareApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__kctxshareapiControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void kctxshareapiControlSerialization_Epilogue_DISPATCH(struct KernelCtxShareApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__kctxshareapiControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS kctxshareapiControl_Prologue_DISPATCH(struct KernelCtxShareApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__kctxshareapiControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void kctxshareapiControl_Epilogue_DISPATCH(struct KernelCtxShareApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__kctxshareapiControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS kctxshareapiIsDuplicate_DISPATCH(struct KernelCtxShareApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__kctxshareapiIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void kctxshareapiPreDestruct_DISPATCH(struct KernelCtxShareApi *pResource) {
    pResource->__kctxshareapiPreDestruct__(pResource);
}

static inline NV_STATUS kctxshareapiControlFilter_DISPATCH(struct KernelCtxShareApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__kctxshareapiControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool kctxshareapiIsPartialUnmapSupported_DISPATCH(struct KernelCtxShareApi *pResource) {
    return pResource->__kctxshareapiIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS kctxshareapiMapTo_DISPATCH(struct KernelCtxShareApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__kctxshareapiMapTo__(pResource, pParams);
}

static inline NV_STATUS kctxshareapiUnmapFrom_DISPATCH(struct KernelCtxShareApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__kctxshareapiUnmapFrom__(pResource, pParams);
}

static inline NvU32 kctxshareapiGetRefCount_DISPATCH(struct KernelCtxShareApi *pResource) {
    return pResource->__kctxshareapiGetRefCount__(pResource);
}

static inline void kctxshareapiAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct KernelCtxShareApi *pResource, RsResourceRef *pReference) {
    pResource->__kctxshareapiAddAdditionalDependants__(pClient, pResource, pReference);
}

NvBool kctxshareapiCanCopy_IMPL(struct KernelCtxShareApi *pKernelCtxShareApi);

NV_STATUS kctxshareapiCtrlCmdSetTpcPartitionTable_IMPL(struct KernelCtxShareApi *pKernelCtxShareApi, NV9067_CTRL_TPC_PARTITION_TABLE_PARAMS *pParams);

NV_STATUS kctxshareapiCtrlCmdGetCwdWatermark_IMPL(struct KernelCtxShareApi *pKernelCtxShareApi, NV9067_CTRL_CWD_WATERMARK_PARAMS *pParams);

NV_STATUS kctxshareapiCtrlCmdSetCwdWatermark_IMPL(struct KernelCtxShareApi *pKernelCtxShareApi, NV9067_CTRL_CWD_WATERMARK_PARAMS *pParams);

NV_STATUS kctxshareapiConstruct_IMPL(struct KernelCtxShareApi *arg_pKernelCtxShareApi, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_kctxshareapiConstruct(arg_pKernelCtxShareApi, arg_pCallContext, arg_pParams) kctxshareapiConstruct_IMPL(arg_pKernelCtxShareApi, arg_pCallContext, arg_pParams)
NV_STATUS kctxshareapiCopyConstruct_IMPL(struct KernelCtxShareApi *pKernelCtxShareApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);

#ifdef __nvoc_kernel_ctxshare_h_disabled
static inline NV_STATUS kctxshareapiCopyConstruct(struct KernelCtxShareApi *pKernelCtxShareApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelCtxShareApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_ctxshare_h_disabled
#define kctxshareapiCopyConstruct(pKernelCtxShareApi, pCallContext, pParams) kctxshareapiCopyConstruct_IMPL(pKernelCtxShareApi, pCallContext, pParams)
#endif //__nvoc_kernel_ctxshare_h_disabled

void kctxshareapiDestruct_IMPL(struct KernelCtxShareApi *pKernelCtxShareApi);

#define __nvoc_kctxshareapiDestruct(pKernelCtxShareApi) kctxshareapiDestruct_IMPL(pKernelCtxShareApi)
#undef PRIVATE_FIELD


#endif // CTXSHARE_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_CTXSHARE_NVOC_H_
