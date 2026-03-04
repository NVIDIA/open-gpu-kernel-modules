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

/* ------------------------ Includes --------------------------------------- */
#include "gpu/perf/kern_perf.h"
#include "gpu/perf/kern_perf_boost.h"
#include "rmapi/rmapi.h"
#include "gpu/gpu.h"
#include "gpu/subdevice/subdevice.h"
#include "resserv/rs_client.h"
#include "vgpu/rpc.h"
#include "ctrl/ctrl2080/ctrl2080perf.h"

/* ------------------------ Global Variables ------------------------------- */
/* ------------------------ Static Function Prototypes --------------------- */
/* ------------------------ Macros ----------------------------------------- */
/* ------------------------ Public Functions ------------------------------- */

/*!
 * @copydoc NV2080_CTRL_CMD_PERF_BOOST
 */
NV_STATUS
subdeviceCtrlCmdKPerfBoost_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_PERF_BOOST_PARAMS *pBoostParams
)
{
    OBJGPU      *pGpu            = GPU_RES_GET_GPU(pSubdevice);
    KernelPerf  *pKernelPerf     = GPU_GET_KERNEL_PERF(pGpu);
    NV_STATUS    status          = NV_OK;

    if (IsGB10Y(pGpu))
    {
        if (pBoostParams->duration != NV2080_CTRL_PERF_BOOST_DURATION_INFINITE &&
            pBoostParams->duration > NV2080_CTRL_PERF_BOOST_DURATION_MAX)
        {
            NV_PRINTF(LEVEL_ERROR, "The specified duration exceed maximum %d!\n",
                      NV2080_CTRL_PERF_BOOST_DURATION_MAX);
            return NV_ERR_INVALID_ARGUMENT;
        }

        if (FLD_TEST_DRF(2080, _CTRL_PERF_BOOST_FLAGS, _CMD, _CLEAR, pBoostParams->flags))
        {
            return osTegraiGpuPerfBoost(pGpu, NV_FALSE, 0);
        }
        else
        {
            // Bug 5397272
            //
            // When cudaSetDevice is being used, duration of perfboost will be
            // NV2080_CTRL_PERF_BOOST_DURATION_INFINITE, and when the app exits,
            // there is no followup NV2080_CTRL_PERF_BOOST_FLAGS_CMD_CLEAR call
            // to reset/disable the perfboost. This leave GPU in perfboost mode
            // forever (long enough).
            //
            // This is a WAR solution for Tegra iGPU only to override the
            // duration to 1 when the app does not explicitly reset the perfboost
            // but still give it 1 second time to boost up GPU clocks and speed
            // up the context creation or app init time.
            if (pBoostParams->duration == NV2080_CTRL_PERF_BOOST_DURATION_INFINITE)
            {
                pBoostParams->duration = 1;
            }

            return osTegraiGpuPerfBoost(pGpu, NV_TRUE, pBoostParams->duration);
        }
    }

    NV_CHECK_OR_RETURN(LEVEL_INFO, (pKernelPerf != NULL), NV_ERR_NOT_SUPPORTED);

    status = kperfBoostSet(pKernelPerf, pSubdevice, pBoostParams);
    return status;
}

/*!
 * @copydoc kperfBoostSet
 */
NV_STATUS
kperfBoostSet_IMPL
(
    KernelPerf *pKernelPerf,
    Subdevice  *pSubdevice,
    NV2080_CTRL_PERF_BOOST_PARAMS *pBoostParams
)
{
    OBJGPU    *pGpu   = GPU_RES_GET_GPU(pSubdevice);
    RM_API    *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV_STATUS  status = NV_OK;
    NV2080_CTRL_INTERNAL_PERF_BOOST_SET_PARAMS_2X boostParams2x = {0};

    boostParams2x.flags    = pBoostParams->flags;
    boostParams2x.duration = pBoostParams->duration;

    status = pRmApi->Control(pRmApi,
                             RES_GET_CLIENT_HANDLE(pSubdevice),
                             RES_GET_HANDLE(pSubdevice),
                             NV2080_CTRL_CMD_INTERNAL_PERF_BOOST_SET_2X,
                             &boostParams2x,
                             sizeof(boostParams2x));

    return status;
}


