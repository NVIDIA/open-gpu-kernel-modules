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

/*!
 * @brief  All functions related to the Cuda Safe feature.
 */
#include "gpu/device/device.h"
#include "gpu/perf/kern_cuda_limit.h"
#include "rmapi/rmapi.h"
#include "gpu/gpu.h"

/* ------------------------ Static Function Prototypes --------------------- */
static NV_STATUS kperfCudaLimitCliGet(Device *pDevice, NvBool *pbCudaLimit);
static NV_STATUS kperfCudaLimitCliSet(Device *pDevice, NvBool bCudaLimit);

/* ------------------------ Public Functions  ------------------------------ */

/*!
 * Disable Cuda limit activation at teardown of client.
 *
 * @param[in]  pDevice  Device info pointer
 * @param[in]  pGpu     OBJGPU pointer
 *
 * @return NV_OK
 *      Operation completed successfully.
 */
NV_STATUS
deviceKPerfCudaLimitCliDisable
(
    Device  *pDevice,
    OBJGPU  *pGpu
)
{
    RM_API    *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV_STATUS  status = NV_OK;

    if (IS_VIRTUAL(pGpu))
    {
        return NV_OK;
    }

    if (pDevice->nCudaLimitRefCnt > 0)
    {
        status = pRmApi->Control(pRmApi,
                                 pGpu->hInternalClient,
                                 pGpu->hInternalDevice,
                                 NV0080_CTRL_CMD_INTERNAL_PERF_CUDA_LIMIT_DISABLE,
                                 NULL,
                                 0);

        NV_CHECK_OR_RETURN(LEVEL_INFO, status == NV_OK, status);

        pDevice->nCudaLimitRefCnt = 0;
    }

    return status;
}

/*!
 * @copydoc NV0080_CTRL_CMD_PERF_CUDA_LIMIT_SET_CONTROL
 *
 * @Parameter pDispCommon [In]
 * @Parameter pParams [In]
 *
 * @return NV_OK
 *      Operation completed successfully.
 * @return NV_ERR_INVALID_REQUEST
 *      Invalid feature activation or deactivation request.
 * @return NV_ERR_INVALID_STATE
 *      Feature isn't enabled.
 * @return Other unexpected errors
 *      Unexpected errors propagated from other functions.
 */
NV_STATUS
deviceCtrlCmdKPerfCudaLimitSetControl_IMPL
(
    Device *pDevice,
    NV0080_CTRL_PERF_CUDA_LIMIT_CONTROL_PARAMS *pParams
)
{
    OBJGPU    *pGpu   = GPU_RES_GET_GPU(pDevice);
    RM_API    *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NvBool     bCudaLimitBefore;
    NvBool     bCudaLimitAfter;
    NV_STATUS  status = NV_OK;

    if (IS_VIRTUAL(pGpu))
    {
        return NV_OK;
    }

    // Obtain current Cuda limit activation setting.
    NV_ASSERT_OK_OR_RETURN(kperfCudaLimitCliGet(pDevice, &bCudaLimitBefore));

    // Set Cuda activation setting and error check the client ref count.
    NV_ASSERT_OK_OR_RETURN(kperfCudaLimitCliSet(pDevice, pParams->bCudaLimit));

    // Obtain current Cuda limit activation setting again.
    NV_ASSERT_OK_OR_RETURN(kperfCudaLimitCliGet(pDevice, &bCudaLimitAfter));

    // If the limit is changing.
    if (bCudaLimitBefore != bCudaLimitAfter)
    {
        status = pRmApi->Control(pRmApi,
                                 RES_GET_CLIENT_HANDLE(pDevice),
                                 RES_GET_HANDLE(pDevice),
                                 NV0080_CTRL_CMD_INTERNAL_PERF_CUDA_LIMIT_SET_CONTROL,
                                 pParams,
                                 sizeof(*pParams));
    }

    return status;
}
/* ---------------------- Private Static Functions -------------------------- */
/*!
 * Get current Cuda limit activation setting.
 *
 * @param[in]  pDevice      Device object
 * @param[out] pbCudaLimit  Returns current Cuda limit setting
 *
 * @return NV_OK
 *      Operation completed successfully.
 * @return NV_ERR_INVALID_POINTER
 *      OUT parameter is NULL.
 */
static NV_STATUS
kperfCudaLimitCliGet
(
    Device *pDevice,
    NvBool *pbCudaLimit
)
{
    NV_ASSERT_OR_RETURN((pbCudaLimit != NULL), NV_ERR_INVALID_POINTER);

    *pbCudaLimit = (pDevice->nCudaLimitRefCnt != 0);
    return NV_OK;
}

/*!
 * Enable/Disable Cuda limit activation setting.
 *
 * @param[in]  pDevice     Device object
 * @param[in]  bCudaLimit  Cuda limit setting
 *
 * @return NV_OK
 *      Operation completed successfully.
 * @return NV_ERR_INVALID_REQUEST
 *      Otherwise
 */
static NV_STATUS
kperfCudaLimitCliSet
(
    Device *pDevice,
    NvBool  bCudaLimit
)
{
    if (bCudaLimit)
    {
        pDevice->nCudaLimitRefCnt++;
    }
    else if (pDevice->nCudaLimitRefCnt > 0)
    {
        pDevice->nCudaLimitRefCnt--;
    }
    else
    {
        return NV_ERR_INVALID_REQUEST;
    }

    return NV_OK;
}
