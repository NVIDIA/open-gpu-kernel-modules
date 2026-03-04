/*
 * SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#define RM_STRICT_CONFIG_EMIT_DISP_ENGINE_DEFINITIONS     0

#include "gpu_mgr/gpu_mgr.h"
#include "gpu/gpu.h"
#include "gpu/disp/kern_disp.h"
#include "gpu/disp/head/kernel_head.h"
#include "disp/v02_04/dev_disp.h"

/*!
 * @brief - Compute the value LSR_MIN_TIME to be set for swap barrier
 *
 * @param[in]  pGpu                GPU object pointer
 * @param[in]  pKernelDisplay      KernelDisplay pointer
 * @param[in]  head                head number
 * @param[in]  swapRdyHiLsrMinTime effective time in micro seconds for which
 *                                 SWAPRDY will be asserted.
 * @param[out] computedLsrMinTime  computed LsrMinTime to be set.
 */
NV_STATUS
kdispComputeLsrMinTimeValue_v02_07
(
    OBJGPU  *pGpu,
    KernelDisplay *pKernelDisplay,
    NvU32    head,
    NvU32    swapRdyHiLsrMinTime,
    NvU32   *computedLsrMinTime
)
{
    //
    // For Pascal and onwards LSR_MIN_TIME has been moved to static clock
    //in ns from dispclock. Thus just send the swapRdyHiTime in ns.
    //
    *computedLsrMinTime = swapRdyHiLsrMinTime * 1000;
    return NV_OK;
}

NV_STATUS
kdispGetRgScanLock_v02_01
(
    OBJGPU    *pGpu,
    KernelDisplay  *pKernelDisplay,
    NvU32      head0,
    OBJGPU    *pPeerGpu,
    NvU32      head1,
    NvBool     *pMasterScanLock,
    NvU32      *pMasterScanLockPin,
    NvBool     *pSlaveScanLock,
    NvU32      *pSlaveScanLockPin
)
{
    NvU32 pinSetOut,  pinSetIn;
    NV_STATUS rmStatus = NV_OK;

    if (!pMasterScanLock || !pMasterScanLockPin ||
        !pSlaveScanLock  || !pSlaveScanLockPin  || head0 >= pKernelDisplay->numHeads ||
        ((pPeerGpu != NULL) && (head1 >= pKernelDisplay->numHeads)))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    //
    // The head parameters are not currently needed for G8X, but for more
    // complicated setups or GT200 SLI, we may need to take them into
    // consideration in the future.
    //
    rmStatus = gpumgrGetGpuLockAndDrPorts(pGpu, pPeerGpu, &pinSetOut, &pinSetIn);
    if (rmStatus != NV_OK)
    {
        return rmStatus;
    }

    NV2080_CTRL_INTERNAL_DISP_PINSETS_TO_LOCKPINS_PARAMS params = {0};
    params.pinSetIn = pinSetIn;
    params.pinSetOut = pinSetOut;

    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV_ASSERT_OK_OR_RETURN(pRmApi->Control(pRmApi,
                            pGpu->hInternalClient,
                            pGpu->hInternalSubdevice,
                            NV2080_CTRL_CMD_INTERNAL_DISP_PINSETS_TO_LOCKPINS,
                            &params,
                            sizeof(params)));

    *pMasterScanLock = params.bMasterScanLock;
    *pMasterScanLockPin = params.masterScanLockPin;

    *pSlaveScanLock = params.bSlaveScanLock;
    *pSlaveScanLockPin = params.slaveScanLockPin;

    return rmStatus;
}

/*!
 * @brief Get the LOADV counter
 *
 * @param[in]  pGpu                    OBJGPU pointer
 * @param[in]  pKernelHead             KernelHead object pointer
 *
 * @return the current LOADV counter
 */
NvU32
kheadGetLoadVCounter_v02_04
(
    OBJGPU                 *pGpu,
    KernelHead             *pKernelHead
)
{
    return GPU_REG_RD32(pGpu, NV_PDISP_PIPE_IN_LOADV_COUNTER(pKernelHead->PublicId));
}
