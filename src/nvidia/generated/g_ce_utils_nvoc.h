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



#if defined(DEBUG) || defined (DEVELOP)
NVOC_PREFIX(ceutilsapi) class CeUtilsApi : GpuResource
{
public:
    NV_STATUS ceutilsapiConstruct(CeUtilsApi *pCeUtilsApi, CALL_CONTEXT *pCallContext,
                                  RS_RES_ALLOC_PARAMS_INTERNAL *pParams)
                                  : GpuResource(pCallContext, pParams);

    void ceutilsapiDestruct(CeUtilsApi *pCeUtilsApi);

    //
    // Below APIs are only provided for SRT testing, thus only available for debug or
    // develop driver builds
    //
    //
    RMCTRL_EXPORT(NV0050_CTRL_CMD_MEMSET, RMCTRL_FLAGS(PRIVILEGED, API_LOCK_READONLY))
    NV_STATUS ceutilsapiCtrlCmdMemset(CeUtilsApi *pCeUtilsApi, NV0050_CTRL_MEMSET_PARAMS *pParams);

    RMCTRL_EXPORT(NV0050_CTRL_CMD_MEMCOPY, RMCTRL_FLAGS(PRIVILEGED, API_LOCK_READONLY))
    NV_STATUS ceutilsapiCtrlCmdMemcopy(CeUtilsApi *pCeUtilsApi, NV0050_CTRL_MEMCOPY_PARAMS *pParams);

    RMCTRL_EXPORT(NV0050_CTRL_CMD_CHECK_PROGRESS, RMCTRL_FLAGS(PRIVILEGED, API_LOCK_READONLY))
    NV_STATUS ceutilsapiCtrlCmdCheckProgress(CeUtilsApi *pCeUtilsApi, NV0050_CTRL_CHECK_PROGRESS_PARAMS *pParams);

    CeUtils *pCeUtils;
};
#endif

#endif // CE_UTILS_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_CE_UTILS_NVOC_H_
