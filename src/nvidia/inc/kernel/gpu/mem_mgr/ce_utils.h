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

class KernelChannel;

NVOC_PREFIX(ceutils) class CeUtils : Object
{
public:
    NV_STATUS ceutilsConstruct(CeUtils *pCeUtils, OBJGPU *pGpu, KERNEL_MIG_GPU_INSTANCE *pKernelMIGGPUInstance,
                               NV0050_ALLOCATION_PARAMETERS *pAllocParams);
    void ceutilsDestruct(CeUtils *pCeUtils);

    NV_STATUS ceutilsMemset(CeUtils *pCeUtils, CEUTILS_MEMSET_PARAMS *pParams);
    NV_STATUS ceutilsMemcopy(CeUtils *pCeUtils, CEUTILS_MEMCOPY_PARAMS *pParams);

    NvU64 ceutilsUpdateProgress(CeUtils *pCeUtils);
    void ceutilsServiceInterrupts(CeUtils *pCeUtils);

    //
    // Internal states
    //

    NvHandle hClient;
    NvHandle hDevice;
    NvHandle hSubdevice;

    OBJCHANNEL *pChannel;
    OBJGPU *pGpu;

    KernelCE *pKCe;
    NvBool bUseVasForCeCopy;
    NvU32 hTdCopyClass;
    NvU64 lastSubmittedPayload;
    NvU64 lastCompletedPayload;

    // Only used by fifo lite implementation
    KernelChannel *pLiteKernelChannel;
};


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
