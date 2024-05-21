/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "ctrl/ctrl2080/ctrl2080clk.h"
#include "ctrl/ctrl2080/ctrl2080perf.h"
#include "os/os.h"
#include "gpu/subdevice/subdevice.h"
#include "rmapi/rmapi.h"
#include "rmapi/rs_utils.h"
#include "vgpu/rpc.h"
#include "gpu/gpu.h"

/* ------------------------ Global Variables ------------------------------- */
/* ------------------------ Static Function Prototypes --------------------- */
/* ------------------------ Macros ----------------------------------------- */
/* ------------------------ Public Functions ------------------------------- */

/*!
 * @copydoc NV2080_CTRL_CMD_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES_V2
 *
 * @params[in]     pSubdevice
 * @params[in/out] pParams
 *
 * @return NV_OK
 *
 * @return Other error
 *     Encountered un-expected error.
 */
NV_STATUS
subdeviceCtrlCmdPerfGetGpumonPerfmonUtilSamplesV2_VF
(
    Subdevice *pSubdevice,
    NV2080_CTRL_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES_V2_PARAMS *pParams
)
{

    CALL_CONTEXT *pCallContext  = resservGetTlsCallContext();
    RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams;
    OBJGPU       *pGpu          = GPU_RES_GET_GPU(pSubdevice);
    NV_STATUS     status        = NV_OK;

    if (IS_MIG_ENABLED(pGpu))
    {
        NV_PRINTF(LEVEL_INFO, "Call not supported with SMC Enabled\n");
        return NV_ERR_NOT_SUPPORTED;
    }

    if (pParams->bufSize != NV2080_CTRL_PERF_GPUMON_PERFMON_UTIL_BUFFER_SIZE)
        return NV_ERR_INVALID_ARGUMENT;

    NV_RM_RPC_GET_ENGINE_UTILIZATION(pRmCtrlParams->pGpu,
                                     pRmCtrlParams->hClient,
                                     pRmCtrlParams->hObject,
                                     pRmCtrlParams->cmd,
                                     pParams->samples,
                                     pParams->bufSize,
                                     status);

    return status;
}

/*
 * We are using the V1 structure here so as to use the old
 * RPC call, without changing function signature of RPC
 * (since it still uses V1 structure). Allocated, used and
 * freed only if vGPU is in effect.
 *
 * The old structure's pointer, where normally would point
 * to some userspace buffer, now points to the V2 structure's
 * static array.
 *
 * V1 structure is allocated from heap rather than on
 * stack out of concern for limits on kernel stack usage.
 * (3 pages on Windows)
 */
NV_STATUS
subdeviceCtrlCmdPerfGetLevelInfo_V2_VF
(
    Subdevice *pSubdevice,
    NV2080_CTRL_PERF_GET_LEVEL_INFO_V2_PARAMS *pLevelInfoParams
)
{
    NV2080_CTRL_PERF_GET_CLK_INFO *pPerfClkInfos = (NV2080_CTRL_PERF_GET_CLK_INFO *)
                                                   &(pLevelInfoParams->perfGetClkInfoList);
    NV2080_CTRL_PERF_GET_LEVEL_INFO_PARAMS *pOldLevelInfoParams = NULL;
    OBJGPU     *pGpu    = GPU_RES_GET_GPU(pSubdevice);
    NV_STATUS   status  = NV_OK;
    NvHandle    hObject = RES_GET_HANDLE(pSubdevice);
    NvHandle    hClient = RES_GET_CLIENT_HANDLE(pSubdevice);

    // get flags for the level
    pLevelInfoParams->flags = 0;

    pOldLevelInfoParams = portMemAllocNonPaged(sizeof(NV2080_CTRL_PERF_GET_LEVEL_INFO_PARAMS));
    if (pOldLevelInfoParams == NULL)
    {
        return NV_ERR_NO_MEMORY;
    }

    pOldLevelInfoParams->level                  = pLevelInfoParams->level;
    pOldLevelInfoParams->flags                  = pLevelInfoParams->flags;
    pOldLevelInfoParams->perfGetClkInfoListSize = pLevelInfoParams->perfGetClkInfoListSize;
    pOldLevelInfoParams->perfGetClkInfoList     = NV_PTR_TO_NvP64(&(pLevelInfoParams->perfGetClkInfoList));

    NV_RM_RPC_PERF_GET_LEVEL_INFO(pGpu,
                                  hClient,
                                  hObject,
                                  pOldLevelInfoParams,
                                  pPerfClkInfos,
                                  status);

    pLevelInfoParams->level                  = pOldLevelInfoParams->level;
    pLevelInfoParams->flags                  = pOldLevelInfoParams->flags;
    pLevelInfoParams->perfGetClkInfoListSize = pOldLevelInfoParams->perfGetClkInfoListSize;
    portMemFree(pOldLevelInfoParams);

    return status;
}

/*!
 * @copydoc NV2080_CTRL_CMD_PERF_GET_VID_ENG_PERFMON_SAMPLE
 *
 * @params[in]     pSubdevice
 * @params[in/out] pParams
 *
 * @return NV_OK
 *
 * @return Other error
 *     Encountered un-expected error.
 */
NV_STATUS
subdeviceCtrlCmdPerfGetVideoEnginePerfmonSample_VF
(
    Subdevice *pSubdevice,
    NV2080_CTRL_PERF_GET_VID_ENG_PERFMON_SAMPLE_PARAMS *pSampleParams
)
{
    OBJGPU       *pGpu          = GPU_RES_GET_GPU(pSubdevice);
    CALL_CONTEXT *pCallContext  = resservGetTlsCallContext();
    RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams;
    NV_STATUS     status        = NV_OK;

    if (IS_MIG_ENABLED(pGpu))
    {
        NV_PRINTF(LEVEL_ERROR, "Call not supported with SMC Enabled\n");
        return NV_ERR_NOT_SUPPORTED;
    }

    //
    // Skipping the VID_ENG_NVJPG and VID_ENG_NVOFA engine calls for vGPU Guest.
    // Support to be added via. bug 4020549
    //
    if ((pSampleParams->engineType == NV2080_CTRL_CMD_PERF_VID_ENG_NVJPG) ||
        (pSampleParams->engineType == NV2080_CTRL_CMD_PERF_VID_ENG_NVOFA))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    NV_RM_RPC_GET_ENGINE_UTILIZATION(pRmCtrlParams->pGpu,
                                     pRmCtrlParams->hClient,
                                     pRmCtrlParams->hObject,
                                     pRmCtrlParams->cmd,
                                     pSampleParams,
                                     sizeof(NV2080_CTRL_PERF_GET_VID_ENG_PERFMON_SAMPLE_PARAMS),
                                     status);

    return status;
}

/*!
 * @copydoc NV2080_CTRL_CMD_PERF_GET_POWERSTATE
 *
 * @params[in]  pSubdevice
 * @params[out] pParams
 *
 * @return NV_OK
 *
 * @return Other error
 *     Encountered un-expected error.
 */
NV_STATUS
subdeviceCtrlCmdPerfGetPowerstate_VF
(
    Subdevice *pSubdevice,
    NV2080_CTRL_PERF_GET_POWERSTATE_PARAMS *powerInfoParams
)
{
    NV_STATUS status = NV_OK;

    //
    // vGPU:
    //
    // We need to set the power state value to AC as we do not initialize perf engine for guest.
    // Return the power state from guest RM instead of RPC call.
    //
    powerInfoParams->powerStateInfo.powerState = NV2080_CTRL_PERF_POWER_SOURCE_AC;

    return status;
}

/*!
 * @copydoc NV2080_CTRL_CMD_PERF_GET_CURRENT_PSTATE
 *
 * @params[in]  pSubdevice
 * @params[out] pParams
 *
 * @return NV_OK
 *
 * @return Other error
 *     Encountered un-expected error.
 */
NV_STATUS
subdeviceCtrlCmdPerfGetCurrentPstate_VF
(
    Subdevice *pSubdevice,
    NV2080_CTRL_PERF_GET_CURRENT_PSTATE_PARAMS *pInfoParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);

    return RmRpcPerfGetCurrentPstate(pGpu, pInfoParams);
}
/* ------------------------- Private Functions ------------------------------ */
