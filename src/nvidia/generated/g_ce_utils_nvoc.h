
#ifndef _G_CE_UTILS_NVOC_H_
#define _G_CE_UTILS_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2023-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_ce_utils_nvoc.h"

#ifndef CE_UTILS_H
#define CE_UTILS_H

#include "gpu/gpu_resource.h" // GpuResource
#include "class/cl0050.h"
#include "ctrl/ctrl0050.h"

#include "kernel/gpu/mem_mgr/channel_utils.h"

typedef struct
{
    MEMORY_DESCRIPTOR *pMemDesc;
    NvU64 offset;
    NvU64 length;
    NvU32 pattern;
    NvU64 flags;
    NvU64 submittedWorkId;   // Payload to poll for async completion
} CEUTILS_MEMSET_PARAMS;

typedef struct
{
    MEMORY_DESCRIPTOR *pSrcMemDesc;
    MEMORY_DESCRIPTOR *pDstMemDesc;
    NvU64 dstOffset;
    NvU64 srcOffset;
    NvU64 length;
    NvU64 flags;
    NvU64 submittedWorkId;   // Payload to poll for async completion

    NvBool bSecureCopy; // The copy encrypts/decrypts when copying to/from unprotected memory
    NvBool bEncrypt; // Toggle encrypt/decrypt
    NvU64 authTagAddr; // encryption authTag address. Same aperture as unencrypted operand assumed. 16 byte aligned
    NvU64 encryptIvAddr; // IV value that was used for ecryption, requirements are the same as for authTagAddr. Required

} CEUTILS_MEMCOPY_PARAMS;


struct KernelChannel;

#ifndef __NVOC_CLASS_KernelChannel_TYPEDEF__
#define __NVOC_CLASS_KernelChannel_TYPEDEF__
typedef struct KernelChannel KernelChannel;
#endif /* __NVOC_CLASS_KernelChannel_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelChannel
#define __nvoc_class_id_KernelChannel 0x5d8d70
#endif /* __nvoc_class_id_KernelChannel */




// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_CE_UTILS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__CeUtils;
struct NVOC_METADATA__Object;


struct CeUtils {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__CeUtils *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct Object __nvoc_base_Object;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super
    struct CeUtils *__nvoc_pbase_CeUtils;    // ceutils

    // Data members
    NvHandle hClient;
    OBJCHANNEL *pChannel;
    struct OBJGPU *pGpu;
    struct KernelCE *pKCe;
    NvBool bUseVasForCeCopy;
    NvU32 hTdCopyClass;
    NvU64 lastSubmittedPayload;
    NvU64 lastCompletedPayload;
    NvBool bForcedCeId;
    NvU64 submissionPausedRefCount;
    struct KernelChannel *pLiteKernelChannel;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__CeUtils {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__Object metadata__Object;
};

#ifndef __NVOC_CLASS_CeUtils_TYPEDEF__
#define __NVOC_CLASS_CeUtils_TYPEDEF__
typedef struct CeUtils CeUtils;
#endif /* __NVOC_CLASS_CeUtils_TYPEDEF__ */

#ifndef __nvoc_class_id_CeUtils
#define __nvoc_class_id_CeUtils 0x8b8bae
#endif /* __nvoc_class_id_CeUtils */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_CeUtils;

#define __staticCast_CeUtils(pThis) \
    ((pThis)->__nvoc_pbase_CeUtils)

#ifdef __nvoc_ce_utils_h_disabled
#define __dynamicCast_CeUtils(pThis) ((CeUtils*) NULL)
#else //__nvoc_ce_utils_h_disabled
#define __dynamicCast_CeUtils(pThis) \
    ((CeUtils*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(CeUtils)))
#endif //__nvoc_ce_utils_h_disabled

NV_STATUS __nvoc_objCreateDynamic_CeUtils(CeUtils**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_CeUtils(CeUtils**, Dynamic*, NvU32, struct OBJGPU *arg_pGpu, KERNEL_MIG_GPU_INSTANCE *arg_pKernelMIGGPUInstance, NV0050_ALLOCATION_PARAMETERS *arg_pAllocParams);
#define __objCreate_CeUtils(ppNewObj, pParent, createFlags, arg_pGpu, arg_pKernelMIGGPUInstance, arg_pAllocParams) \
    __nvoc_objCreate_CeUtils((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pGpu, arg_pKernelMIGGPUInstance, arg_pAllocParams)


// Wrapper macros

// Dispatch functions
static inline NvBool ceutilsIsSubmissionPaused(struct CeUtils *pCeUtils) {
    return pCeUtils->submissionPausedRefCount != 0;
}

NV_STATUS ceutilsConstruct_IMPL(struct CeUtils *arg_pCeUtils, struct OBJGPU *arg_pGpu, KERNEL_MIG_GPU_INSTANCE *arg_pKernelMIGGPUInstance, NV0050_ALLOCATION_PARAMETERS *arg_pAllocParams);

#define __nvoc_ceutilsConstruct(arg_pCeUtils, arg_pGpu, arg_pKernelMIGGPUInstance, arg_pAllocParams) ceutilsConstruct_IMPL(arg_pCeUtils, arg_pGpu, arg_pKernelMIGGPUInstance, arg_pAllocParams)
void ceutilsDestruct_IMPL(struct CeUtils *pCeUtils);

#define __nvoc_ceutilsDestruct(pCeUtils) ceutilsDestruct_IMPL(pCeUtils)
NV_STATUS ceutilsMemset_IMPL(struct CeUtils *pCeUtils, CEUTILS_MEMSET_PARAMS *pParams);

#ifdef __nvoc_ce_utils_h_disabled
static inline NV_STATUS ceutilsMemset(struct CeUtils *pCeUtils, CEUTILS_MEMSET_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("CeUtils was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_ce_utils_h_disabled
#define ceutilsMemset(pCeUtils, pParams) ceutilsMemset_IMPL(pCeUtils, pParams)
#endif //__nvoc_ce_utils_h_disabled

NV_STATUS ceutilsMemcopy_IMPL(struct CeUtils *pCeUtils, CEUTILS_MEMCOPY_PARAMS *pParams);

#ifdef __nvoc_ce_utils_h_disabled
static inline NV_STATUS ceutilsMemcopy(struct CeUtils *pCeUtils, CEUTILS_MEMCOPY_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("CeUtils was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_ce_utils_h_disabled
#define ceutilsMemcopy(pCeUtils, pParams) ceutilsMemcopy_IMPL(pCeUtils, pParams)
#endif //__nvoc_ce_utils_h_disabled

NV_STATUS ceutilsGetFirstAsyncCe_IMPL(struct CeUtils *pCeUtils, struct OBJGPU *pGpu, struct RsClient *pClient, NvHandle hDevice, NvU32 *pCeInstance, NvBool forceSkipMIG);

#ifdef __nvoc_ce_utils_h_disabled
static inline NV_STATUS ceutilsGetFirstAsyncCe(struct CeUtils *pCeUtils, struct OBJGPU *pGpu, struct RsClient *pClient, NvHandle hDevice, NvU32 *pCeInstance, NvBool forceSkipMIG) {
    NV_ASSERT_FAILED_PRECOMP("CeUtils was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_ce_utils_h_disabled
#define ceutilsGetFirstAsyncCe(pCeUtils, pGpu, pClient, hDevice, pCeInstance, forceSkipMIG) ceutilsGetFirstAsyncCe_IMPL(pCeUtils, pGpu, pClient, hDevice, pCeInstance, forceSkipMIG)
#endif //__nvoc_ce_utils_h_disabled

NvU64 ceutilsUpdateProgress_IMPL(struct CeUtils *pCeUtils);

#ifdef __nvoc_ce_utils_h_disabled
static inline NvU64 ceutilsUpdateProgress(struct CeUtils *pCeUtils) {
    NV_ASSERT_FAILED_PRECOMP("CeUtils was disabled!");
    return 0;
}
#else //__nvoc_ce_utils_h_disabled
#define ceutilsUpdateProgress(pCeUtils) ceutilsUpdateProgress_IMPL(pCeUtils)
#endif //__nvoc_ce_utils_h_disabled

void ceutilsServiceInterrupts_IMPL(struct CeUtils *pCeUtils);

#ifdef __nvoc_ce_utils_h_disabled
static inline void ceutilsServiceInterrupts(struct CeUtils *pCeUtils) {
    NV_ASSERT_FAILED_PRECOMP("CeUtils was disabled!");
}
#else //__nvoc_ce_utils_h_disabled
#define ceutilsServiceInterrupts(pCeUtils) ceutilsServiceInterrupts_IMPL(pCeUtils)
#endif //__nvoc_ce_utils_h_disabled

void ceutilsPauseSubmission_IMPL(struct CeUtils *pCeUtils, NvBool bWaitForWorkCompletion);

#ifdef __nvoc_ce_utils_h_disabled
static inline void ceutilsPauseSubmission(struct CeUtils *pCeUtils, NvBool bWaitForWorkCompletion) {
    NV_ASSERT_FAILED_PRECOMP("CeUtils was disabled!");
}
#else //__nvoc_ce_utils_h_disabled
#define ceutilsPauseSubmission(pCeUtils, bWaitForWorkCompletion) ceutilsPauseSubmission_IMPL(pCeUtils, bWaitForWorkCompletion)
#endif //__nvoc_ce_utils_h_disabled

void ceutilsResumeSubmission_IMPL(struct CeUtils *pCeUtils);

#ifdef __nvoc_ce_utils_h_disabled
static inline void ceutilsResumeSubmission(struct CeUtils *pCeUtils) {
    NV_ASSERT_FAILED_PRECOMP("CeUtils was disabled!");
}
#else //__nvoc_ce_utils_h_disabled
#define ceutilsResumeSubmission(pCeUtils) ceutilsResumeSubmission_IMPL(pCeUtils)
#endif //__nvoc_ce_utils_h_disabled

NvBool ceutilsUsesPreferredCe_IMPL(struct CeUtils *pCeUtils);

#ifdef __nvoc_ce_utils_h_disabled
static inline NvBool ceutilsUsesPreferredCe(struct CeUtils *pCeUtils) {
    NV_ASSERT_FAILED_PRECOMP("CeUtils was disabled!");
    return NV_FALSE;
}
#else //__nvoc_ce_utils_h_disabled
#define ceutilsUsesPreferredCe(pCeUtils) ceutilsUsesPreferredCe_IMPL(pCeUtils)
#endif //__nvoc_ce_utils_h_disabled

#undef PRIVATE_FIELD




// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_CE_UTILS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__CeUtilsApi;
struct NVOC_METADATA__GpuResource;
struct NVOC_VTABLE__CeUtilsApi;


struct CeUtilsApi {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__CeUtilsApi *__nvoc_metadata_ptr;
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
    struct CeUtilsApi *__nvoc_pbase_CeUtilsApi;    // ceutilsapi

    // Vtable with 3 per-object function pointers
    NV_STATUS (*__ceutilsapiCtrlCmdMemset__)(struct CeUtilsApi * /*this*/, NV0050_CTRL_MEMSET_PARAMS *);  // exported (id=0x500101)
    NV_STATUS (*__ceutilsapiCtrlCmdMemcopy__)(struct CeUtilsApi * /*this*/, NV0050_CTRL_MEMCOPY_PARAMS *);  // exported (id=0x500102)
    NV_STATUS (*__ceutilsapiCtrlCmdCheckProgress__)(struct CeUtilsApi * /*this*/, NV0050_CTRL_CHECK_PROGRESS_PARAMS *);  // exported (id=0x500103)

    // Data members
    struct CeUtils *pCeUtils;
};


// Vtable with 25 per-class function pointers
struct NVOC_VTABLE__CeUtilsApi {
    NV_STATUS (*__ceutilsapiControl__)(struct CeUtilsApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__ceutilsapiMap__)(struct CeUtilsApi * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__ceutilsapiUnmap__)(struct CeUtilsApi * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NvBool (*__ceutilsapiShareCallback__)(struct CeUtilsApi * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__ceutilsapiGetRegBaseOffsetAndSize__)(struct CeUtilsApi * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__ceutilsapiGetMapAddrSpace__)(struct CeUtilsApi * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__ceutilsapiInternalControlForward__)(struct CeUtilsApi * /*this*/, NvU32, void *, NvU32);  // virtual inherited (gpures) base (gpures)
    NvHandle (*__ceutilsapiGetInternalObjectHandle__)(struct CeUtilsApi * /*this*/);  // virtual inherited (gpures) base (gpures)
    NvBool (*__ceutilsapiAccessCallback__)(struct CeUtilsApi * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__ceutilsapiGetMemInterMapParams__)(struct CeUtilsApi * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__ceutilsapiCheckMemInterUnmap__)(struct CeUtilsApi * /*this*/, NvBool);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__ceutilsapiGetMemoryMappingDescriptor__)(struct CeUtilsApi * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__ceutilsapiControlSerialization_Prologue__)(struct CeUtilsApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__ceutilsapiControlSerialization_Epilogue__)(struct CeUtilsApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__ceutilsapiControl_Prologue__)(struct CeUtilsApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__ceutilsapiControl_Epilogue__)(struct CeUtilsApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NvBool (*__ceutilsapiCanCopy__)(struct CeUtilsApi * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__ceutilsapiIsDuplicate__)(struct CeUtilsApi * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (gpures)
    void (*__ceutilsapiPreDestruct__)(struct CeUtilsApi * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__ceutilsapiControlFilter__)(struct CeUtilsApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (gpures)
    NvBool (*__ceutilsapiIsPartialUnmapSupported__)(struct CeUtilsApi * /*this*/);  // inline virtual inherited (res) base (gpures) body
    NV_STATUS (*__ceutilsapiMapTo__)(struct CeUtilsApi * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__ceutilsapiUnmapFrom__)(struct CeUtilsApi * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (gpures)
    NvU32 (*__ceutilsapiGetRefCount__)(struct CeUtilsApi * /*this*/);  // virtual inherited (res) base (gpures)
    void (*__ceutilsapiAddAdditionalDependants__)(struct RsClient *, struct CeUtilsApi * /*this*/, RsResourceRef *);  // virtual inherited (res) base (gpures)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__CeUtilsApi {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResource metadata__GpuResource;
    const struct NVOC_VTABLE__CeUtilsApi vtable;
};

#ifndef __NVOC_CLASS_CeUtilsApi_TYPEDEF__
#define __NVOC_CLASS_CeUtilsApi_TYPEDEF__
typedef struct CeUtilsApi CeUtilsApi;
#endif /* __NVOC_CLASS_CeUtilsApi_TYPEDEF__ */

#ifndef __nvoc_class_id_CeUtilsApi
#define __nvoc_class_id_CeUtilsApi 0x2eb528
#endif /* __nvoc_class_id_CeUtilsApi */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_CeUtilsApi;

#define __staticCast_CeUtilsApi(pThis) \
    ((pThis)->__nvoc_pbase_CeUtilsApi)

#ifdef __nvoc_ce_utils_h_disabled
#define __dynamicCast_CeUtilsApi(pThis) ((CeUtilsApi*) NULL)
#else //__nvoc_ce_utils_h_disabled
#define __dynamicCast_CeUtilsApi(pThis) \
    ((CeUtilsApi*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(CeUtilsApi)))
#endif //__nvoc_ce_utils_h_disabled

NV_STATUS __nvoc_objCreateDynamic_CeUtilsApi(CeUtilsApi**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_CeUtilsApi(CeUtilsApi**, Dynamic*, NvU32, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __objCreate_CeUtilsApi(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_CeUtilsApi((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define ceutilsapiCtrlCmdMemset_FNPTR(pCeUtilsApi) pCeUtilsApi->__ceutilsapiCtrlCmdMemset__
#define ceutilsapiCtrlCmdMemset(pCeUtilsApi, pParams) ceutilsapiCtrlCmdMemset_DISPATCH(pCeUtilsApi, pParams)
#define ceutilsapiCtrlCmdMemcopy_FNPTR(pCeUtilsApi) pCeUtilsApi->__ceutilsapiCtrlCmdMemcopy__
#define ceutilsapiCtrlCmdMemcopy(pCeUtilsApi, pParams) ceutilsapiCtrlCmdMemcopy_DISPATCH(pCeUtilsApi, pParams)
#define ceutilsapiCtrlCmdCheckProgress_FNPTR(pCeUtilsApi) pCeUtilsApi->__ceutilsapiCtrlCmdCheckProgress__
#define ceutilsapiCtrlCmdCheckProgress(pCeUtilsApi, pParams) ceutilsapiCtrlCmdCheckProgress_DISPATCH(pCeUtilsApi, pParams)
#define ceutilsapiControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresControl__
#define ceutilsapiControl(pGpuResource, pCallContext, pParams) ceutilsapiControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define ceutilsapiMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresMap__
#define ceutilsapiMap(pGpuResource, pCallContext, pParams, pCpuMapping) ceutilsapiMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define ceutilsapiUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresUnmap__
#define ceutilsapiUnmap(pGpuResource, pCallContext, pCpuMapping) ceutilsapiUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define ceutilsapiShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresShareCallback__
#define ceutilsapiShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) ceutilsapiShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define ceutilsapiGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__
#define ceutilsapiGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) ceutilsapiGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define ceutilsapiGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__
#define ceutilsapiGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) ceutilsapiGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define ceutilsapiInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__
#define ceutilsapiInternalControlForward(pGpuResource, command, pParams, size) ceutilsapiInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define ceutilsapiGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__
#define ceutilsapiGetInternalObjectHandle(pGpuResource) ceutilsapiGetInternalObjectHandle_DISPATCH(pGpuResource)
#define ceutilsapiAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresAccessCallback__
#define ceutilsapiAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) ceutilsapiAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define ceutilsapiGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define ceutilsapiGetMemInterMapParams(pRmResource, pParams) ceutilsapiGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define ceutilsapiCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define ceutilsapiCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) ceutilsapiCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define ceutilsapiGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define ceutilsapiGetMemoryMappingDescriptor(pRmResource, ppMemDesc) ceutilsapiGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define ceutilsapiControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Prologue__
#define ceutilsapiControlSerialization_Prologue(pResource, pCallContext, pParams) ceutilsapiControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define ceutilsapiControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Epilogue__
#define ceutilsapiControlSerialization_Epilogue(pResource, pCallContext, pParams) ceutilsapiControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define ceutilsapiControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Prologue__
#define ceutilsapiControl_Prologue(pResource, pCallContext, pParams) ceutilsapiControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define ceutilsapiControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Epilogue__
#define ceutilsapiControl_Epilogue(pResource, pCallContext, pParams) ceutilsapiControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define ceutilsapiCanCopy_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define ceutilsapiCanCopy(pResource) ceutilsapiCanCopy_DISPATCH(pResource)
#define ceutilsapiIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define ceutilsapiIsDuplicate(pResource, hMemory, pDuplicate) ceutilsapiIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define ceutilsapiPreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define ceutilsapiPreDestruct(pResource) ceutilsapiPreDestruct_DISPATCH(pResource)
#define ceutilsapiControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define ceutilsapiControlFilter(pResource, pCallContext, pParams) ceutilsapiControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define ceutilsapiIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define ceutilsapiIsPartialUnmapSupported(pResource) ceutilsapiIsPartialUnmapSupported_DISPATCH(pResource)
#define ceutilsapiMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define ceutilsapiMapTo(pResource, pParams) ceutilsapiMapTo_DISPATCH(pResource, pParams)
#define ceutilsapiUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define ceutilsapiUnmapFrom(pResource, pParams) ceutilsapiUnmapFrom_DISPATCH(pResource, pParams)
#define ceutilsapiGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define ceutilsapiGetRefCount(pResource) ceutilsapiGetRefCount_DISPATCH(pResource)
#define ceutilsapiAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define ceutilsapiAddAdditionalDependants(pClient, pResource, pReference) ceutilsapiAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NV_STATUS ceutilsapiCtrlCmdMemset_DISPATCH(struct CeUtilsApi *pCeUtilsApi, NV0050_CTRL_MEMSET_PARAMS *pParams) {
    return pCeUtilsApi->__ceutilsapiCtrlCmdMemset__(pCeUtilsApi, pParams);
}

static inline NV_STATUS ceutilsapiCtrlCmdMemcopy_DISPATCH(struct CeUtilsApi *pCeUtilsApi, NV0050_CTRL_MEMCOPY_PARAMS *pParams) {
    return pCeUtilsApi->__ceutilsapiCtrlCmdMemcopy__(pCeUtilsApi, pParams);
}

static inline NV_STATUS ceutilsapiCtrlCmdCheckProgress_DISPATCH(struct CeUtilsApi *pCeUtilsApi, NV0050_CTRL_CHECK_PROGRESS_PARAMS *pParams) {
    return pCeUtilsApi->__ceutilsapiCtrlCmdCheckProgress__(pCeUtilsApi, pParams);
}

static inline NV_STATUS ceutilsapiControl_DISPATCH(struct CeUtilsApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__ceutilsapiControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS ceutilsapiMap_DISPATCH(struct CeUtilsApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__ceutilsapiMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS ceutilsapiUnmap_DISPATCH(struct CeUtilsApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__ceutilsapiUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NvBool ceutilsapiShareCallback_DISPATCH(struct CeUtilsApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__ceutilsapiShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS ceutilsapiGetRegBaseOffsetAndSize_DISPATCH(struct CeUtilsApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__ceutilsapiGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS ceutilsapiGetMapAddrSpace_DISPATCH(struct CeUtilsApi *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__ceutilsapiGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS ceutilsapiInternalControlForward_DISPATCH(struct CeUtilsApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__ceutilsapiInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NvHandle ceutilsapiGetInternalObjectHandle_DISPATCH(struct CeUtilsApi *pGpuResource) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__ceutilsapiGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool ceutilsapiAccessCallback_DISPATCH(struct CeUtilsApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_metadata_ptr->vtable.__ceutilsapiAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS ceutilsapiGetMemInterMapParams_DISPATCH(struct CeUtilsApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__ceutilsapiGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS ceutilsapiCheckMemInterUnmap_DISPATCH(struct CeUtilsApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__ceutilsapiCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS ceutilsapiGetMemoryMappingDescriptor_DISPATCH(struct CeUtilsApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__ceutilsapiGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS ceutilsapiControlSerialization_Prologue_DISPATCH(struct CeUtilsApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__ceutilsapiControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void ceutilsapiControlSerialization_Epilogue_DISPATCH(struct CeUtilsApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__ceutilsapiControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS ceutilsapiControl_Prologue_DISPATCH(struct CeUtilsApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__ceutilsapiControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void ceutilsapiControl_Epilogue_DISPATCH(struct CeUtilsApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__ceutilsapiControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool ceutilsapiCanCopy_DISPATCH(struct CeUtilsApi *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__ceutilsapiCanCopy__(pResource);
}

static inline NV_STATUS ceutilsapiIsDuplicate_DISPATCH(struct CeUtilsApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_metadata_ptr->vtable.__ceutilsapiIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void ceutilsapiPreDestruct_DISPATCH(struct CeUtilsApi *pResource) {
    pResource->__nvoc_metadata_ptr->vtable.__ceutilsapiPreDestruct__(pResource);
}

static inline NV_STATUS ceutilsapiControlFilter_DISPATCH(struct CeUtilsApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__ceutilsapiControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool ceutilsapiIsPartialUnmapSupported_DISPATCH(struct CeUtilsApi *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__ceutilsapiIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS ceutilsapiMapTo_DISPATCH(struct CeUtilsApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__ceutilsapiMapTo__(pResource, pParams);
}

static inline NV_STATUS ceutilsapiUnmapFrom_DISPATCH(struct CeUtilsApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__ceutilsapiUnmapFrom__(pResource, pParams);
}

static inline NvU32 ceutilsapiGetRefCount_DISPATCH(struct CeUtilsApi *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__ceutilsapiGetRefCount__(pResource);
}

static inline void ceutilsapiAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct CeUtilsApi *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_metadata_ptr->vtable.__ceutilsapiAddAdditionalDependants__(pClient, pResource, pReference);
}

NV_STATUS ceutilsapiCtrlCmdMemset_IMPL(struct CeUtilsApi *pCeUtilsApi, NV0050_CTRL_MEMSET_PARAMS *pParams);

NV_STATUS ceutilsapiCtrlCmdMemcopy_IMPL(struct CeUtilsApi *pCeUtilsApi, NV0050_CTRL_MEMCOPY_PARAMS *pParams);

NV_STATUS ceutilsapiCtrlCmdCheckProgress_IMPL(struct CeUtilsApi *pCeUtilsApi, NV0050_CTRL_CHECK_PROGRESS_PARAMS *pParams);

NV_STATUS ceutilsapiConstruct_IMPL(struct CeUtilsApi *arg_pCeUtilsApi, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_ceutilsapiConstruct(arg_pCeUtilsApi, arg_pCallContext, arg_pParams) ceutilsapiConstruct_IMPL(arg_pCeUtilsApi, arg_pCallContext, arg_pParams)
void ceutilsapiDestruct_IMPL(struct CeUtilsApi *pCeUtilsApi);

#define __nvoc_ceutilsapiDestruct(pCeUtilsApi) ceutilsapiDestruct_IMPL(pCeUtilsApi)
#undef PRIVATE_FIELD


#endif // CE_UTILS_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_CE_UTILS_NVOC_H_
