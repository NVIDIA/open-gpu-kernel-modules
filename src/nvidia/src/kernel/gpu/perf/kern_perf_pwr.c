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

/* ------------------------ Includes --------------------------------------- */
#include "os/os.h"
#include "gpu/subdevice/subdevice.h"
#include "rmapi/rmapi.h"
#include "rmapi/rs_utils.h"
#include "vgpu/rpc.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "gpu/gpu.h"

/* ------------------------ Global Variables ------------------------------- */
/* ------------------------ Static Function Prototypes --------------------- */
/* ------------------------ Macros ----------------------------------------- */
/* ------------------------ Public Functions ------------------------------- */
/*!
 * @brief Retrieving the current requested RATED_TDP action corresponding
 * to the specified client.
 *
 * @params[in]     pSubdevice
 * @params[in/out] pControlParams
 *
 * @return NV_OK
 *     Client control action successfuly applied.
 * @return NV_ERR_NOT_SUPPORTED
 *     RATED_TDP functionality not supported on this GPU.
 * @reutrn NV_ERR_INVALID_ARGUMENT
 *     Invalid client specified.
 */
NV_STATUS
subdeviceCtrlCmdPerfRatedTdpSetControl_KERNEL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_PERF_RATED_TDP_CONTROL_PARAMS *pControlParams
)
{
    OBJGPU   *pGpu     = GPU_RES_GET_GPU(pSubdevice);
    NvHandle  hClient  = RES_GET_CLIENT_HANDLE(pSubdevice);
    NV_STATUS status   = NV_OK;

    NvBool bSmcEnabled = IS_MIG_ENABLED(pGpu);
    CALL_CONTEXT *pCallContext = resservGetTlsCallContext();

    NV_ASSERT_OR_RETURN(pCallContext != NULL, NV_ERR_INVALID_STATE);

    //
    // With SMC enabled, the clock controls can only be modified by a priv client
    // This is to ensure that clients running under a SMC partition do not impact
    // other clients running on different partitions
    //
    if (bSmcEnabled && !rmclientIsAdminByHandle(hClient, pCallContext->secInfo.privLevel))
    {
        NV_PRINTF(LEVEL_ERROR,
                "Non-Privileged clients are not allowed to access clock controls with SMC enabled.\n");
        return NV_ERR_INSUFFICIENT_PERMISSIONS;
    }

    //
    // Redirect to Physical RM in case of the GSP CLIENT or
    // host RM in case of the vGPU
    //
    RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams;

    NV_RM_RPC_CONTROL(pGpu,
                      pRmCtrlParams->hClient,
                      pRmCtrlParams->hObject,
                      pRmCtrlParams->cmd,
                      pRmCtrlParams->pParams,
                      pRmCtrlParams->paramsSize,
                      status);


    return status;
}

/* ------------------------- Private Functions ------------------------------ */
