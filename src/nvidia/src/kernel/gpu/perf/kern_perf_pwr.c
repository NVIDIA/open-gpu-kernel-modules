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
    NV_STATUS status   = NV_OK;
    RmClient *pRmClient;

    NvBool bSmcEnabled = IS_MIG_ENABLED(pGpu);
    CALL_CONTEXT *pCallContext = resservGetTlsCallContext();

    NV_ASSERT_OR_RETURN(pCallContext != NULL, NV_ERR_INVALID_STATE);

    pRmClient = dynamicCast(pCallContext->pClient, RmClient);
    NV_ASSERT_OR_RETURN(pRmClient != NULL, NV_ERR_INVALID_CLIENT);

    NvBool isAdmin = rmclientIsAdmin(pRmClient, pCallContext->secInfo.privLevel);
    //
    // With SMC enabled, the clock controls can only be modified by a priv client
    // This is to ensure that clients running under a SMC partition do not impact
    // other clients running on different partitions
    //
    if (bSmcEnabled && !isAdmin)
    {
        NV_PRINTF(LEVEL_ERROR,
                "Non-Privileged clients are not allowed to access clock controls with SMC enabled.\n");
        return NV_ERR_INSUFFICIENT_PERMISSIONS;
    }

    if ((!(isAdmin || osCheckAccess(RS_ACCESS_PERFMON))) &&
        (gpuIsRmProfilingPrivileged(pGpu) && (pControlParams->vPstateType == NV2080_CTRL_PERF_VPSTATE_TURBO_BOOST)))
    {
        NV_PRINTF(LEVEL_ERROR,
                "Non-Privileged clients are not allowed to use Turbo Boost clock controls.\n");
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

/*!
 * @brief Send ACPI callback function to notify SBIOS of our power state change
 *
 * @param  pGpu            OBJGPU pointer
 * @param  pKernelPerf     KernelPerf pointer
 *
 * @return No return value from this function.
 */
static void
_kperfSendPostPowerStateCallback
(
    OBJGPU     *pGpu,
    KernelPerf *pKernelPerf
)
{
    NvU32     inOutData    = 0;
    NvU16     outDataSize  = sizeof(inOutData);
    NV_STATUS status       = NV_OK;
    ACPI_DSM_FUNCTION func = gpuGetPerfPostPowerStateFunc(pGpu);
    RM_API    *pRmApi      = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV2080_CTRL_INTERNAL_PERF_GET_AUX_POWER_STATE_PARAMS ctrlParams = { 0 };

    // Bail out early if ACPI callback is not supported.
    if (func == ACPI_DSM_FUNCTION_COUNT)
    {
        return;
    }

    status = pRmApi->Control(pRmApi,
                             pGpu->hInternalClient,
                             pGpu->hInternalSubdevice,
                             NV2080_CTRL_CMD_INTERNAL_PERF_GET_AUX_POWER_STATE,
                             &ctrlParams,
                             sizeof(ctrlParams));

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Error getting Aux Power State:0x%x\n", status);
    }

    // Setup the argument to send
    inOutData = FLD_SET_DRF(_ACPI, _CALLBACKS_ARG, _POSTPOWERSTATE, _NOTIFY,
                            inOutData);
    inOutData = FLD_SET_DRF_NUM(_ACPI, _CALLBACKS_ARG, _CURRENTPOWERSTATE,
                                ctrlParams.powerState, inOutData);

    status = osCallACPI_DSM(pGpu, func,
                            NV_ACPI_GENERIC_FUNC_CALLBACKS,
                            &inOutData, &outDataSize);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "PostPState callback error:0x%x\n", status);
    }
}

NV_STATUS
subdeviceCtrlCmdPerfSetAuxPowerState_KERNEL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_PERF_SET_AUX_POWER_STATE_PARAMS *pPowerStateParams
)
{
    OBJGPU   *pGpu   = GPU_RES_GET_GPU(pSubdevice);
    RM_API   *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    KernelPerf *pKernelPerf = GPU_GET_KERNEL_PERF(pGpu);
    NV_STATUS status;

    NV_CHECK_OR_RETURN(LEVEL_INFO, (pKernelPerf != NULL), NV_ERR_NOT_SUPPORTED);

    NV_ASSERT_OR_RETURN(pPowerStateParams->powerState < NV2080_CTRL_PERF_AUX_POWER_STATE_COUNT,
                        NV_ERR_INVALID_ARGUMENT);

    // Redirect to Physical RM.
    status = pRmApi->Control(pRmApi,
                             RES_GET_CLIENT_HANDLE(pSubdevice),
                             RES_GET_HANDLE(pSubdevice),
                             NV2080_CTRL_CMD_PERF_SET_AUX_POWER_STATE,
                             pPowerStateParams,
                             sizeof(*pPowerStateParams));

    if (status != NV_OK)
    {
        NV_ASSERT(0);
    }

    _kperfSendPostPowerStateCallback(pGpu, pKernelPerf);

    // Notify clients about Dx State event
    gpuNotifySubDeviceEvent(pGpu, NV2080_NOTIFIERS_AUX_POWER_EVENT, NULL, 0, 0, 0);

    return status;
}

NV_STATUS
subdeviceCtrlCmdPerfSetPowerstate_KERNEL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_PERF_SET_POWERSTATE_PARAMS *pPowerInfoParams
)
{
    OBJGPU     *pGpu        = GPU_RES_GET_GPU(pSubdevice);
    RM_API     *pRmApi      = GPU_GET_PHYSICAL_RMAPI(pGpu);
    KernelPerf *pKernelPerf = GPU_GET_KERNEL_PERF(pGpu);
    NV_STATUS   status      = NV_OK;
    NvBool      bSwitchToAC = (pPowerInfoParams->powerStateInfo.powerState ==
                                             NV2080_CTRL_PERF_POWER_SOURCE_AC);

    NV_CHECK_OR_RETURN(LEVEL_INFO, (pKernelPerf != NULL), NV_ERR_NOT_SUPPORTED);

    if ((pPowerInfoParams->powerStateInfo.powerState != NV2080_CTRL_PERF_POWER_SOURCE_AC) &&
        (pPowerInfoParams->powerStateInfo.powerState != NV2080_CTRL_PERF_POWER_SOURCE_BATTERY))
    {
        status = NV_ERR_INVALID_ARGUMENT;
    }
    else if(!gpuIsGpuFullPower(pGpu))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "NV2080_CTRL_CMD_PERF_SET_POWERSTATE called in power down state.\n");
        NV_PRINTF(LEVEL_ERROR, "      Returning NV_ERR_GPU_NOT_FULL_POWER.\n");
        status = NV_ERR_GPU_NOT_FULL_POWER;
    }

    if (status != NV_OK)
        return status;

    // Redirect to Physical RM.
    status = pRmApi->Control(pRmApi,
                             RES_GET_CLIENT_HANDLE(pSubdevice),
                             RES_GET_HANDLE(pSubdevice),
                             NV2080_CTRL_CMD_PERF_SET_POWERSTATE,
                             pPowerInfoParams,
                             sizeof(*pPowerInfoParams));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "NV2080_CTRL_CMD_PERF_SET_POWERSTATE RPC failed\n");
    }

    // If callback was registered, update platform with the current Auxpower state(Dx)
    _kperfSendPostPowerStateCallback(pGpu, pKernelPerf);

    Nv2080PowerEventNotification powerEventNotificationParams = {0};

    // Notify clients about AC/DC switch
    powerEventNotificationParams.bSwitchToAC = bSwitchToAC;
    powerEventNotificationParams.bGPUCapabilityChanged = 0;
    powerEventNotificationParams.displayMaskAffected = 0;
    gpuNotifySubDeviceEvent(pGpu, NV2080_NOTIFIERS_POWER_EVENT,
        &powerEventNotificationParams, sizeof(powerEventNotificationParams), 0, 0);

    return status;
}
/* ------------------------- Private Functions ------------------------------ */
