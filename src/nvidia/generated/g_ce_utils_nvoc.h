#ifndef _G_CE_UTILS_NVOC_H_
#define _G_CE_UTILS_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

struct CeUtils {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct Object __nvoc_base_Object;
    struct Object *__nvoc_pbase_Object;
    struct CeUtils *__nvoc_pbase_CeUtils;
    NvHandle hClient;
    NvHandle hDevice;
    NvHandle hSubdevice;
    OBJCHANNEL *pChannel;
    struct OBJGPU *pGpu;
    struct KernelCE *pKCe;
    NvBool bUseVasForCeCopy;
    NvU32 hTdCopyClass;
    NvU64 lastSubmittedPayload;
    NvU64 lastCompletedPayload;
    struct KernelChannel *pLiteKernelChannel;
};

#ifndef __NVOC_CLASS_CeUtils_TYPEDEF__
#define __NVOC_CLASS_CeUtils_TYPEDEF__
typedef struct CeUtils CeUtils;
#endif /* __NVOC_CLASS_CeUtils_TYPEDEF__ */

#ifndef __nvoc_class_id_CeUtils
#define __nvoc_class_id_CeUtils 0x8b8bae
#endif /* __nvoc_class_id_CeUtils */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_CeUtils;

#define __staticCast_CeUtils(pThis) \
    ((pThis)->__nvoc_pbase_CeUtils)

#ifdef __nvoc_ce_utils_h_disabled
#define __dynamicCast_CeUtils(pThis) ((CeUtils*)NULL)
#else //__nvoc_ce_utils_h_disabled
#define __dynamicCast_CeUtils(pThis) \
    ((CeUtils*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(CeUtils)))
#endif //__nvoc_ce_utils_h_disabled


NV_STATUS __nvoc_objCreateDynamic_CeUtils(CeUtils**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_CeUtils(CeUtils**, Dynamic*, NvU32, struct OBJGPU * arg_pGpu, KERNEL_MIG_GPU_INSTANCE * arg_pKernelMIGGPUInstance, NV0050_ALLOCATION_PARAMETERS * arg_pAllocParams);
#define __objCreate_CeUtils(ppNewObj, pParent, createFlags, arg_pGpu, arg_pKernelMIGGPUInstance, arg_pAllocParams) \
    __nvoc_objCreate_CeUtils((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pGpu, arg_pKernelMIGGPUInstance, arg_pAllocParams)

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

#undef PRIVATE_FIELD




// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_CE_UTILS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif

struct CeUtilsApi {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct GpuResource __nvoc_base_GpuResource;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct GpuResource *__nvoc_pbase_GpuResource;
    struct CeUtilsApi *__nvoc_pbase_CeUtilsApi;
    NV_STATUS (*__ceutilsapiCtrlCmdMemset__)(struct CeUtilsApi *, NV0050_CTRL_MEMSET_PARAMS *);
    NV_STATUS (*__ceutilsapiCtrlCmdMemcopy__)(struct CeUtilsApi *, NV0050_CTRL_MEMCOPY_PARAMS *);
    NV_STATUS (*__ceutilsapiCtrlCmdCheckProgress__)(struct CeUtilsApi *, NV0050_CTRL_CHECK_PROGRESS_PARAMS *);
    NvBool (*__ceutilsapiShareCallback__)(struct CeUtilsApi *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__ceutilsapiCheckMemInterUnmap__)(struct CeUtilsApi *, NvBool);
    NV_STATUS (*__ceutilsapiMapTo__)(struct CeUtilsApi *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__ceutilsapiGetMapAddrSpace__)(struct CeUtilsApi *, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    NvU32 (*__ceutilsapiGetRefCount__)(struct CeUtilsApi *);
    void (*__ceutilsapiAddAdditionalDependants__)(struct RsClient *, struct CeUtilsApi *, RsResourceRef *);
    NV_STATUS (*__ceutilsapiControl_Prologue__)(struct CeUtilsApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__ceutilsapiGetRegBaseOffsetAndSize__)(struct CeUtilsApi *, struct OBJGPU *, NvU32 *, NvU32 *);
    NV_STATUS (*__ceutilsapiInternalControlForward__)(struct CeUtilsApi *, NvU32, void *, NvU32);
    NV_STATUS (*__ceutilsapiUnmapFrom__)(struct CeUtilsApi *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__ceutilsapiControl_Epilogue__)(struct CeUtilsApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvHandle (*__ceutilsapiGetInternalObjectHandle__)(struct CeUtilsApi *);
    NV_STATUS (*__ceutilsapiControl__)(struct CeUtilsApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__ceutilsapiUnmap__)(struct CeUtilsApi *, struct CALL_CONTEXT *, struct RsCpuMapping *);
    NV_STATUS (*__ceutilsapiGetMemInterMapParams__)(struct CeUtilsApi *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__ceutilsapiGetMemoryMappingDescriptor__)(struct CeUtilsApi *, struct MEMORY_DESCRIPTOR **);
    NV_STATUS (*__ceutilsapiControlFilter__)(struct CeUtilsApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__ceutilsapiControlSerialization_Prologue__)(struct CeUtilsApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__ceutilsapiCanCopy__)(struct CeUtilsApi *);
    NvBool (*__ceutilsapiIsPartialUnmapSupported__)(struct CeUtilsApi *);
    void (*__ceutilsapiPreDestruct__)(struct CeUtilsApi *);
    NV_STATUS (*__ceutilsapiIsDuplicate__)(struct CeUtilsApi *, NvHandle, NvBool *);
    void (*__ceutilsapiControlSerialization_Epilogue__)(struct CeUtilsApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__ceutilsapiMap__)(struct CeUtilsApi *, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);
    NvBool (*__ceutilsapiAccessCallback__)(struct CeUtilsApi *, struct RsClient *, void *, RsAccessRight);
    struct CeUtils *pCeUtils;
};

#ifndef __NVOC_CLASS_CeUtilsApi_TYPEDEF__
#define __NVOC_CLASS_CeUtilsApi_TYPEDEF__
typedef struct CeUtilsApi CeUtilsApi;
#endif /* __NVOC_CLASS_CeUtilsApi_TYPEDEF__ */

#ifndef __nvoc_class_id_CeUtilsApi
#define __nvoc_class_id_CeUtilsApi 0x2eb528
#endif /* __nvoc_class_id_CeUtilsApi */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_CeUtilsApi;

#define __staticCast_CeUtilsApi(pThis) \
    ((pThis)->__nvoc_pbase_CeUtilsApi)

#ifdef __nvoc_ce_utils_h_disabled
#define __dynamicCast_CeUtilsApi(pThis) ((CeUtilsApi*)NULL)
#else //__nvoc_ce_utils_h_disabled
#define __dynamicCast_CeUtilsApi(pThis) \
    ((CeUtilsApi*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(CeUtilsApi)))
#endif //__nvoc_ce_utils_h_disabled


NV_STATUS __nvoc_objCreateDynamic_CeUtilsApi(CeUtilsApi**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_CeUtilsApi(CeUtilsApi**, Dynamic*, NvU32, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_CeUtilsApi(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_CeUtilsApi((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define ceutilsapiCtrlCmdMemset(pCeUtilsApi, pParams) ceutilsapiCtrlCmdMemset_DISPATCH(pCeUtilsApi, pParams)
#define ceutilsapiCtrlCmdMemcopy(pCeUtilsApi, pParams) ceutilsapiCtrlCmdMemcopy_DISPATCH(pCeUtilsApi, pParams)
#define ceutilsapiCtrlCmdCheckProgress(pCeUtilsApi, pParams) ceutilsapiCtrlCmdCheckProgress_DISPATCH(pCeUtilsApi, pParams)
#define ceutilsapiShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) ceutilsapiShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define ceutilsapiCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) ceutilsapiCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define ceutilsapiMapTo(pResource, pParams) ceutilsapiMapTo_DISPATCH(pResource, pParams)
#define ceutilsapiGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) ceutilsapiGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define ceutilsapiGetRefCount(pResource) ceutilsapiGetRefCount_DISPATCH(pResource)
#define ceutilsapiAddAdditionalDependants(pClient, pResource, pReference) ceutilsapiAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define ceutilsapiControl_Prologue(pResource, pCallContext, pParams) ceutilsapiControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define ceutilsapiGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) ceutilsapiGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define ceutilsapiInternalControlForward(pGpuResource, command, pParams, size) ceutilsapiInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define ceutilsapiUnmapFrom(pResource, pParams) ceutilsapiUnmapFrom_DISPATCH(pResource, pParams)
#define ceutilsapiControl_Epilogue(pResource, pCallContext, pParams) ceutilsapiControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define ceutilsapiGetInternalObjectHandle(pGpuResource) ceutilsapiGetInternalObjectHandle_DISPATCH(pGpuResource)
#define ceutilsapiControl(pGpuResource, pCallContext, pParams) ceutilsapiControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define ceutilsapiUnmap(pGpuResource, pCallContext, pCpuMapping) ceutilsapiUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define ceutilsapiGetMemInterMapParams(pRmResource, pParams) ceutilsapiGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define ceutilsapiGetMemoryMappingDescriptor(pRmResource, ppMemDesc) ceutilsapiGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define ceutilsapiControlFilter(pResource, pCallContext, pParams) ceutilsapiControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define ceutilsapiControlSerialization_Prologue(pResource, pCallContext, pParams) ceutilsapiControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define ceutilsapiCanCopy(pResource) ceutilsapiCanCopy_DISPATCH(pResource)
#define ceutilsapiIsPartialUnmapSupported(pResource) ceutilsapiIsPartialUnmapSupported_DISPATCH(pResource)
#define ceutilsapiPreDestruct(pResource) ceutilsapiPreDestruct_DISPATCH(pResource)
#define ceutilsapiIsDuplicate(pResource, hMemory, pDuplicate) ceutilsapiIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define ceutilsapiControlSerialization_Epilogue(pResource, pCallContext, pParams) ceutilsapiControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define ceutilsapiMap(pGpuResource, pCallContext, pParams, pCpuMapping) ceutilsapiMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define ceutilsapiAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) ceutilsapiAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
NV_STATUS ceutilsapiCtrlCmdMemset_IMPL(struct CeUtilsApi *pCeUtilsApi, NV0050_CTRL_MEMSET_PARAMS *pParams);

static inline NV_STATUS ceutilsapiCtrlCmdMemset_DISPATCH(struct CeUtilsApi *pCeUtilsApi, NV0050_CTRL_MEMSET_PARAMS *pParams) {
    return pCeUtilsApi->__ceutilsapiCtrlCmdMemset__(pCeUtilsApi, pParams);
}

NV_STATUS ceutilsapiCtrlCmdMemcopy_IMPL(struct CeUtilsApi *pCeUtilsApi, NV0050_CTRL_MEMCOPY_PARAMS *pParams);

static inline NV_STATUS ceutilsapiCtrlCmdMemcopy_DISPATCH(struct CeUtilsApi *pCeUtilsApi, NV0050_CTRL_MEMCOPY_PARAMS *pParams) {
    return pCeUtilsApi->__ceutilsapiCtrlCmdMemcopy__(pCeUtilsApi, pParams);
}

NV_STATUS ceutilsapiCtrlCmdCheckProgress_IMPL(struct CeUtilsApi *pCeUtilsApi, NV0050_CTRL_CHECK_PROGRESS_PARAMS *pParams);

static inline NV_STATUS ceutilsapiCtrlCmdCheckProgress_DISPATCH(struct CeUtilsApi *pCeUtilsApi, NV0050_CTRL_CHECK_PROGRESS_PARAMS *pParams) {
    return pCeUtilsApi->__ceutilsapiCtrlCmdCheckProgress__(pCeUtilsApi, pParams);
}

static inline NvBool ceutilsapiShareCallback_DISPATCH(struct CeUtilsApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__ceutilsapiShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS ceutilsapiCheckMemInterUnmap_DISPATCH(struct CeUtilsApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__ceutilsapiCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS ceutilsapiMapTo_DISPATCH(struct CeUtilsApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__ceutilsapiMapTo__(pResource, pParams);
}

static inline NV_STATUS ceutilsapiGetMapAddrSpace_DISPATCH(struct CeUtilsApi *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__ceutilsapiGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NvU32 ceutilsapiGetRefCount_DISPATCH(struct CeUtilsApi *pResource) {
    return pResource->__ceutilsapiGetRefCount__(pResource);
}

static inline void ceutilsapiAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct CeUtilsApi *pResource, RsResourceRef *pReference) {
    pResource->__ceutilsapiAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS ceutilsapiControl_Prologue_DISPATCH(struct CeUtilsApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__ceutilsapiControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS ceutilsapiGetRegBaseOffsetAndSize_DISPATCH(struct CeUtilsApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__ceutilsapiGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS ceutilsapiInternalControlForward_DISPATCH(struct CeUtilsApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__ceutilsapiInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NV_STATUS ceutilsapiUnmapFrom_DISPATCH(struct CeUtilsApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__ceutilsapiUnmapFrom__(pResource, pParams);
}

static inline void ceutilsapiControl_Epilogue_DISPATCH(struct CeUtilsApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__ceutilsapiControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvHandle ceutilsapiGetInternalObjectHandle_DISPATCH(struct CeUtilsApi *pGpuResource) {
    return pGpuResource->__ceutilsapiGetInternalObjectHandle__(pGpuResource);
}

static inline NV_STATUS ceutilsapiControl_DISPATCH(struct CeUtilsApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__ceutilsapiControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS ceutilsapiUnmap_DISPATCH(struct CeUtilsApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__ceutilsapiUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NV_STATUS ceutilsapiGetMemInterMapParams_DISPATCH(struct CeUtilsApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__ceutilsapiGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS ceutilsapiGetMemoryMappingDescriptor_DISPATCH(struct CeUtilsApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__ceutilsapiGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS ceutilsapiControlFilter_DISPATCH(struct CeUtilsApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__ceutilsapiControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS ceutilsapiControlSerialization_Prologue_DISPATCH(struct CeUtilsApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__ceutilsapiControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool ceutilsapiCanCopy_DISPATCH(struct CeUtilsApi *pResource) {
    return pResource->__ceutilsapiCanCopy__(pResource);
}

static inline NvBool ceutilsapiIsPartialUnmapSupported_DISPATCH(struct CeUtilsApi *pResource) {
    return pResource->__ceutilsapiIsPartialUnmapSupported__(pResource);
}

static inline void ceutilsapiPreDestruct_DISPATCH(struct CeUtilsApi *pResource) {
    pResource->__ceutilsapiPreDestruct__(pResource);
}

static inline NV_STATUS ceutilsapiIsDuplicate_DISPATCH(struct CeUtilsApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__ceutilsapiIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void ceutilsapiControlSerialization_Epilogue_DISPATCH(struct CeUtilsApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__ceutilsapiControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS ceutilsapiMap_DISPATCH(struct CeUtilsApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__ceutilsapiMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NvBool ceutilsapiAccessCallback_DISPATCH(struct CeUtilsApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__ceutilsapiAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

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
