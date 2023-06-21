/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*!
 * @brief  All functions are related to the Perf Boost Hint feature.
 */
/* ------------------------ Includes --------------------------------------- */
#include "os/os.h"
#include "gpu/gpu.h"
#include "rmapi/rmapi.h"
#include "resserv/rs_client.h"
#include "vgpu/rpc.h"
#include "gpu/perf/kern_perfbuffer.h"

/* ------------------------ Global Variables ------------------------------- */
/* ------------------------ Static Function Prototypes --------------------- */
/* ------------------------ Macros ----------------------------------------- */
/* ------------------------ Public Functions ------------------------------- */
/*!
 * @copydoc perfbufferConstructHal
 */
NV_STATUS
perfbufferConstructHal_KERNEL
(
    PerfBuffer *pResource,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    OBJGPU           *pGpu           = GPU_RES_GET_GPU(pResource);
    RsResourceRef    *pResourceRef   = pCallContext->pResourceRef;
    NvHandle          hClient        = pCallContext->pClient->hClient;
    NvHandle          hParent        = pCallContext->pResourceRef->pParentRef->hResource;
    NvHandle          hMemory        = pCallContext->pResourceRef->hResource;
    NvU32             class          = pResourceRef->externalClassId;
    NV_STATUS         status         = NV_OK;

    NV_CHECK_OK_OR_RETURN(LEVEL_NOTICE, perfbufferPrivilegeCheck(pResource));

    NV_RM_RPC_ALLOC_OBJECT(pGpu, hClient, hParent, hMemory, class,
        pParams->pAllocParams, pParams->paramsSize, status);

    return status;
}

/*!
 * @copydoc perfbufferPrivilegeCheck
 */
NV_STATUS
perfbufferPrivilegeCheck_IMPL
(
    PerfBuffer *pResource
)
{
    OBJGPU  *pGpu = GPU_RES_GET_GPU(pResource);

    if (gpuIsRmProfilingPrivileged(pGpu) && !osIsAdministrator())
    {
        return NV_ERR_INSUFFICIENT_PERMISSIONS;
    }

    return NV_OK;
}

/* ------------------------- Private Functions ------------------------------ */
