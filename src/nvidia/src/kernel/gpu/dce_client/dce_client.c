/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "gpu/dce_client/dce_client.h"

NV_STATUS
dceclientConstructEngine_IMPL
(
    OBJGPU        *pGpu,
    DceClient     *pDceClient,
    ENGDESCRIPTOR engDesc
)
{
    NV_PRINTF(LEVEL_INFO, "dceclientConstructEngine_IMPL Called\n");

    if (!IS_DCE_CLIENT(pGpu))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    return dceclientInitRpcInfra(pGpu, pDceClient);
}

void
dceclientDestruct_IMPL
(
    DceClient     *pDceClient
)
{
    NV_PRINTF(LEVEL_INFO, "dceclientDestruct_IMPL Called\n");
    dceclientDeinitRpcInfra(pDceClient);
}

/*
 * @brief State load for DceClient during PM resume.
 *
 * Re-initializes the RPC infrastructure that was torn down during suspend
 * and triggers the RM resume path
 */
NV_STATUS
dceclientStateLoad_IMPL
(
    OBJGPU        *pGpu,
    DceClient     *pDceClient,
    NvU32         flags
)
{
    NV_STATUS nvStatus = NV_OK;

    if (!(flags & GPU_STATE_FLAGS_PM_TRANSITION))
        return NV_OK;

    nvStatus = dceclientInitRpcInfra(pGpu, pDceClient);
    if (nvStatus != NV_OK)
        return nvStatus;

    /*
    * During PM resume, send the DCE RM init RPC with bInit = NV_FALSE and
    * bFromResume = NV_TRUE. bInit is FALSE because this is not a fresh
    * init and bFromResume signals DCE to trigger dce_rm_resume()
    */
    return dceclientSendRmInitRpc(pGpu, pDceClient, NV_FALSE, NV_TRUE);
}

NV_STATUS
dceclientStateUnload_IMPL
(
    OBJGPU        *pGpu,
    DceClient     *pDceClient,
    NvU32         flags
)
{
    NV_STATUS nvStatus      = NV_OK;
    NvU32     newPMLevel;
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV2080_CTRL_INTERNAL_GPU_CLIENT_LOW_POWER_MODE_ENTER_PARAMS params;

    NV_PRINTF(LEVEL_INFO, "dceclientStateUnload_IMPL Called\n");

    if (!(flags & GPU_STATE_FLAGS_PM_TRANSITION))
        goto done;

    if (flags & GPU_STATE_FLAGS_PM_HIBERNATE)
        newPMLevel = NV2080_CTRL_GPU_SET_POWER_STATE_GPU_LEVEL_7;
    else
        newPMLevel = NV2080_CTRL_GPU_SET_POWER_STATE_GPU_LEVEL_3;

    portMemSet(&params, 0, sizeof(params));
    params.bInPMTransition = NV_TRUE;
    params.newPMLevel = newPMLevel;
    nvStatus = pRmApi->Control(pRmApi,
                               pGpu->hInternalClient,
                               pGpu->hInternalSubdevice,
                               NV2080_CTRL_INTERNAL_GPU_CLIENT_LOW_POWER_MODE_ENTER,
                               &params, sizeof(params));

    if (nvStatus != NV_OK)
        goto done;

    dceclientDeinitRpcInfra(pDceClient);

done:
    return nvStatus;
}

void
dceclientStateDestroy_IMPL
(
    OBJGPU *pGpu,
    DceClient *pDceClient
)
{
    NV_PRINTF(LEVEL_INFO, "Destroy DCE Client State Called\n");
}
