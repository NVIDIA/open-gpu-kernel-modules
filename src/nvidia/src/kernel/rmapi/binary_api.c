/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "rmapi/binary_api.h"
#include "resserv/rs_client.h"
#include "resserv/rs_server.h"
#include "rmapi/client.h"
#include "rmapi/resource.h"
#include "rmapi/rmapi.h"
#include "rmapi/control.h"
#include "ctrl/ctrlxxxx.h"
#include "gpu/gpu_resource.h"
#include "gpu/gpu.h"
#include "core/locks.h"
#include "vgpu/rpc.h"
#include "rmapi/rmapi_utils.h"

NV_STATUS
binapiConstruct_IMPL
(
    BinaryApi      *pResource,
    CALL_CONTEXT   *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    return NV_OK;
}

NV_STATUS
binapiprivConstruct_IMPL
(
    BinaryApiPrivileged *pResource,
    CALL_CONTEXT        *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    return NV_OK;
}

NV_STATUS
binapiControl_IMPL
(
    BinaryApi    *pResource,
    CALL_CONTEXT *pCallContext,
    RS_RES_CONTROL_PARAMS_INTERNAL *pParams
)
{
    NV_STATUS status = NV_OK;
    OBJGPU *pGpu = GPU_RES_GET_GPU(pResource);
    GPU_MASK gpuMaskRelease = 0;
    RM_API *pRmApi;

    // check if CMD is NULL, return early
    if (RMCTRL_IS_NULL_CMD(pParams->cmd))
        return NV_OK;

    if (pGpu == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    if (IS_VIRTUAL(pGpu))
    {
        {
            NV_ASSERT_OK_OR_RETURN(rmGpuGroupLockAcquire(pGpu->gpuInstance,
                                                         GPU_LOCK_GRP_SUBDEVICE,
                                                         GPUS_LOCK_FLAGS_NONE,
                                                         RM_LOCK_MODULES_RPC,
                                                         &gpuMaskRelease));

            NV_RM_RPC_API_CONTROL(pGpu,
                                  pParams->hClient,
                                  pParams->hObject,
                                  pParams->cmd,
                                  pParams->pParams,
                                  pParams->paramsSize,
                                  status);

            if (gpuMaskRelease != 0)
            {
                rmGpuGroupLockRelease(gpuMaskRelease, GPUS_LOCK_FLAGS_NONE);
            }
        }
    }
    else if (IS_GSP_CLIENT(pGpu))
    {
        NV_ASSERT_OK_OR_RETURN(rmGpuGroupLockAcquire(pGpu->gpuInstance,
                                                     GPU_LOCK_GRP_SUBDEVICE,
                                                     GPUS_LOCK_FLAGS_NONE,
                                                     RM_LOCK_MODULES_RPC,
                                                     &gpuMaskRelease));

        pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
        status = pRmApi->Control(pRmApi,
                                 pParams->hClient,
                                 pParams->hObject,
                                 pParams->cmd,
                                 pParams->pParams,
                                 pParams->paramsSize);

        if (gpuMaskRelease != 0)
        {
            rmGpuGroupLockRelease(gpuMaskRelease, GPUS_LOCK_FLAGS_NONE);
        }
    }

    return status;
}

NV_STATUS
binapiprivControl_IMPL
(
    BinaryApiPrivileged *pResource,
    CALL_CONTEXT        *pCallContext,
    RS_RES_CONTROL_PARAMS_INTERNAL *pParams
)
{
    NV_STATUS status = NV_OK;

    // check if CMD is NULL, return early
    if (RMCTRL_IS_NULL_CMD(pParams->cmd))
        return NV_OK;
    {
        if (pParams->secInfo.privLevel >= RS_PRIV_LEVEL_USER_ROOT)
        {
            status = NV_OK;
        }
        else
        {
            status = NV_ERR_INSUFFICIENT_PERMISSIONS;
        }
    }

    if (status == NV_OK)
    {
        return binapiControl_IMPL(staticCast(pResource, BinaryApi), pCallContext, pParams);
    }
    else
    {
        return status;
    }
}

