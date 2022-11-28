/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

 /**
  * @file disp_common_kern_ctrl_minimal.c implements rmctrls which
  * (a) are declared in disp_common_ctrl_minimal.h; i.e.
  *    (i) are dispcmnCtrlCmd* functions
  *    (ii) which are used by Tegra SOC NVDisplay and/or OS layer; and
  * (b) are implemented in Kernel RM.
  */

#define RM_STRICT_CONFIG_EMIT_DISP_ENGINE_DEFINITIONS     0

#include "os/os.h"
#include "gpu/gpu.h"
#include "gpu/disp/kern_disp.h"
#include "gpu/disp/disp_objs.h"
#include "rmapi/rs_utils.h"
#include "rmapi/rmapi.h"

NV_STATUS
dispcmnCtrlCmdSystemGetHotplugUnplugState_IMPL
(
    DispCommon *pDispCommon,
    NV0073_CTRL_SYSTEM_GET_HOTPLUG_UNPLUG_STATE_PARAMS *pHotplugParams
)
{
    NvHandle   hDevice = RES_GET_PARENT_HANDLE(pDispCommon);
    RM_API    *pRmApi = GPU_GET_PHYSICAL_RMAPI(DISPAPI_GET_GPU(pDispCommon));
    NvU32      hotPlugMask   = 0;
    NvU32      hotUnplugMask = 0;
    NV_STATUS  status;

    status = pRmApi->Control(pRmApi,
                             RES_GET_CLIENT_HANDLE(pDispCommon),
                             RES_GET_HANDLE(pDispCommon),
                             NV0073_CTRL_CMD_INTERNAL_GET_HOTPLUG_UNPLUG_STATE,
                             pHotplugParams,
                             sizeof(*pHotplugParams));

    hotPlugMask = pHotplugParams->hotPlugMask;
    hotUnplugMask = pHotplugParams->hotUnplugMask;
    pHotplugParams->hotPlugMask = 0;
    pHotplugParams->hotUnplugMask = 0;

    if (status != NV_OK)
    {
        return status;
    }

    if ((hotPlugMask != 0) || (hotUnplugMask != 0))
    {
        RmClient **ppClient;
        RsClient  *pRsClient;

        for (ppClient = serverutilGetFirstClientUnderLock();
             ppClient;
             ppClient = serverutilGetNextClientUnderLock(ppClient))
        {
            pRsClient = staticCast(*ppClient, RsClient);
            DispCommon *pDispCommonLoop;

            dispcmnGetByDevice(pRsClient, hDevice, &pDispCommonLoop);
            if (pDispCommonLoop == NULL)
                continue;

            pDispCommonLoop->hotPlugMaskToBeReported   |= hotPlugMask   & (~(pDispCommonLoop->hotPlugMaskToBeReported   & hotUnplugMask));
            pDispCommonLoop->hotUnplugMaskToBeReported |= hotUnplugMask & (~(pDispCommonLoop->hotUnplugMaskToBeReported & hotPlugMask));
        }
    }

    pHotplugParams->hotPlugMask   = pDispCommon->hotPlugMaskToBeReported;
    pHotplugParams->hotUnplugMask = pDispCommon->hotUnplugMaskToBeReported;
    pDispCommon->hotPlugMaskToBeReported = 0;
    pDispCommon->hotUnplugMaskToBeReported = 0;

    return status;
}

/*!
 * @brief Allocate display bandwidth.
 */
NV_STATUS
dispcmnCtrlCmdSystemAllocateDisplayBandwidth_IMPL
(
    DispCommon *pDispCommon,
    NV0073_CTRL_SYSTEM_ALLOCATE_DISPLAY_BANDWIDTH_PARAMS *pParams
)
{
    OBJGPU        *pGpu;
    KernelDisplay *pKernelDisplay;
    NV_STATUS      status;

    // client gave us a subdevice #: get right pGpu for it
    status = dispapiSetUnicastAndSynchronize_HAL(
                               staticCast(pDispCommon, DisplayApi),
                               DISPAPI_GET_GPUGRP(pDispCommon),
                               &pGpu,
                               pParams->subDeviceInstance);
    if (status != NV_OK)
    {
        return status;
    }

    pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);
    return kdispArbAndAllocDisplayBandwidth_HAL(pGpu,
                                                pKernelDisplay,
                                                DISPLAY_ICC_BW_CLIENT_EXT,
                                                pParams->averageBandwidthKBPS,
                                                pParams->floorBandwidthKBPS);
}

NV_STATUS
dispcmnCtrlCmdDpGenerateFakeInterrupt_IMPL
(
    DispCommon *pDispCommon,
    NV0073_CTRL_CMD_DP_GENERATE_FAKE_INTERRUPT_PARAMS *pParams
)
{
    OBJGPU *pGpu = DISPAPI_GET_GPU(pDispCommon);
    NvU32 displayId = pParams->displayId;
    NvU32 interruptType = pParams->interruptType;
    NV_STATUS status = NV_OK;

    // get target pGpu
    status = dispapiSetUnicastAndSynchronize_HAL(
                               staticCast(pDispCommon, DisplayApi),
                               DISPAPI_GET_GPUGRP(pDispCommon),
                               &pGpu,
                               pParams->subDeviceInstance);
    if (status != NV_OK)
    {
        return status;
    }

    NV_ASSERT_OR_RETURN(pParams->displayId, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pGpu, NV_ERR_INVALID_ARGUMENT);

    // Send a DP IRQ (short pulse) to a registered client.
    if (interruptType == NV0073_CTRL_CMD_DP_GENERATE_FAKE_INTERRUPT_IRQ)
    {
        Nv2080DpIrqNotification params = {0};
        params.displayId = displayId;

        // Check eDP power state; if off, return an error.
        RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
        NV0073_CTRL_DP_GET_EDP_DATA_PARAMS edpData;

        portMemSet(&edpData, 0, sizeof(edpData));

        status = pRmApi->Control(pRmApi,
                                 RES_GET_CLIENT_HANDLE(pDispCommon),
                                 RES_GET_HANDLE(pDispCommon),
                                 NV0073_CTRL_CMD_DP_GET_EDP_DATA,
                                 &edpData,
                                 sizeof(edpData));

        if (status == NV_OK && FLD_TEST_DRF(0073_CTRL_DP, _GET_EDP_DATA, _PANEL_POWER, _OFF, edpData.data))
        {
            return NV_ERR_GENERIC;
        }

        gpuNotifySubDeviceEvent(pGpu, NV2080_NOTIFIERS_DP_IRQ, &params, sizeof(params), 0, 0);
    }
    else if (interruptType == NV0073_CTRL_CMD_DP_GENERATE_FAKE_INTERRUPT_PLUG ||
             interruptType == NV0073_CTRL_CMD_DP_GENERATE_FAKE_INTERRUPT_UNPLUG)
    {
        Nv2080HotplugNotification hotplugNotificationParams;
        portMemSet(&hotplugNotificationParams, 0, sizeof(hotplugNotificationParams));

        if (interruptType == NV0073_CTRL_CMD_DP_GENERATE_FAKE_INTERRUPT_PLUG)
        {
            hotplugNotificationParams.plugDisplayMask = displayId;
            hotplugNotificationParams.unplugDisplayMask = 0;
        }
        else if (interruptType == NV0073_CTRL_CMD_DP_GENERATE_FAKE_INTERRUPT_UNPLUG)
        {
            hotplugNotificationParams.plugDisplayMask = 0;
            hotplugNotificationParams.unplugDisplayMask = displayId;
        }
        gpuNotifySubDeviceEvent(pGpu, NV2080_NOTIFIERS_HOTPLUG,
            &hotplugNotificationParams, sizeof(hotplugNotificationParams), 0, 0);
    }
    else
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    return NV_OK;
}
