/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/disp/head/kernel_head.h"
#include "mem_mgr/mem.h"
#include "platform/sli/sli.h"

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
                               NULL,
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
                               NULL,
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

NV_STATUS dispcmnCtrlCmdVRRSetRgLineActive_IMPL
(
    DispCommon *pDispCommon,
    NV0073_CTRL_CMD_SYSTEM_VRR_SET_RGLINE_ACTIVE_PARAMS *pParams
)
{
    OBJGPU   *pGpu   = DISPAPI_GET_GPU(pDispCommon);
    RsClient *pClient = RES_GET_CLIENT(pDispCommon);
    NvHandle  hParent = RES_GET_PARENT_HANDLE(pDispCommon);
    RM_API   *pRmApi = GPU_GET_PHYSICAL_RMAPI(DISPAPI_GET_GPU(pDispCommon));
    NV_STATUS status = NV_OK;

    // Get the right pGpu from subdevice instance given by client
    status = dispapiSetUnicastAndSynchronize_HAL(
                               staticCast(pDispCommon, DisplayApi),
                               DISPAPI_GET_GPUGRP(pDispCommon),
                               &pGpu,
                               NULL,
                               pParams->subDeviceInstance);

    if (status != NV_OK)
    {
        return status;
    }

    if (pParams->bEnable)
    {
        //
        // Note: memRegisterWithGsp() is a noop when either (a) we're not
        // operating as a GSP client, or (b) the hMemory is already registered
        // with GSP.
        //
        // Also, note that we don't explicitly unregister here in the
        // !pParams->bEnable case: that could unregister the memory out from
        // under other uses of this hMemory on GSP.
        // Instead, we rely on the hMemory getting unregistered when the
        // 'struct Memory' is freed.
        //
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                              memRegisterWithGsp(pGpu, pClient, hParent, pParams->hMemory));
    }

    return pRmApi->Control(pRmApi,
                           pClient->hClient,
                           RES_GET_HANDLE(pDispCommon),
                           NV0073_CTRL_CMD_INTERNAL_VRR_SET_RGLINE_ACTIVE,
                           pParams,
                           sizeof(*pParams));
}

static NV_STATUS _kheadCheckVblankCountCallback
(
    OBJGPU   *pGpu,
    void     *Object,
    NvU32     param1,
    NvV32     BuffNum,
    NV_STATUS Status
)
{
    KernelDisplay *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);
    KernelHead    *pKernelHead    = KDISP_GET_HEAD(pKernelDisplay, param1);

    if ((--pKernelHead->Vblank.VblankCountTimeout) == 0)
    {
        pKernelHead->Vblank.Callback.CheckVblankCount.Flags &= ~VBLANK_CALLBACK_FLAG_PERSISTENT;
    }

    return NV_OK;
}

NV_STATUS
dispcmnCtrlCmdSystemGetVblankCounter_IMPL
(
    DispCommon *pDispCommon,
    NV0073_CTRL_SYSTEM_GET_VBLANK_COUNTER_PARAMS *pVBCounterParams
)
{
    OBJGPU        *pGpu = DISPAPI_GET_GPU(pDispCommon);
    KernelDisplay *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);
    KernelHead    *pKernelHead;
    NvU32 flags =  (VBLANK_CALLBACK_FLAG_SPECIFIED_VBLANK_NEXT | VBLANK_CALLBACK_FLAG_PERSISTENT);

    pKernelHead = KDISP_GET_HEAD(pKernelDisplay, pVBCounterParams->head);
    if (pKernelHead == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "invalid head number!\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    // TODO: make the behaviour same for monolithic and offload RM case
    if (IS_GSP_CLIENT(pGpu))
    {
        if (pKernelDisplay->pSharedData == NULL)
        {
            NV_PRINTF(LEVEL_ERROR, "no memory allocated for vblank count\n");
            return NV_ERR_NOT_SUPPORTED;
        }
        flags |= VBLANK_CALLBACK_FLAG_LOW_LATENCY;
    }

    pKernelHead->Vblank.VblankCountTimeout = 60 * VBLANK_INFO_GATHER_KEEPALIVE_SECONDS;

    pKernelHead->Vblank.Callback.CheckVblankCount.Proc   = _kheadCheckVblankCountCallback;
    pKernelHead->Vblank.Callback.CheckVblankCount.pObject = NULL;
    pKernelHead->Vblank.Callback.CheckVblankCount.bObjectIsChannelDescendant = NV_FALSE;
    pKernelHead->Vblank.Callback.CheckVblankCount.Param1 = pKernelHead->PublicId;
    pKernelHead->Vblank.Callback.CheckVblankCount.Param2 = 0;
    pKernelHead->Vblank.Callback.CheckVblankCount.Status = NV_OK;
    pKernelHead->Vblank.Callback.CheckVblankCount.bIsVblankNotifyEnable = NV_TRUE;
    pKernelHead->Vblank.Callback.CheckVblankCount.Flags  = flags;

    kheadAddVblankCallback(pGpu, pKernelHead, &pKernelHead->Vblank.Callback.CheckVblankCount);

    if (IS_GSP_CLIENT(pGpu))
    {
        if (pVBCounterParams->lowLatencyHint)
        {
            pVBCounterParams->verticalBlankCounter = kheadGetVblankLowLatencyCounter_HAL(pKernelHead);
        }
        else
        {
            pVBCounterParams->verticalBlankCounter = pKernelDisplay->pSharedData->kHeadVblankCount[pKernelHead->PublicId];
        }
    }
    else
    {
        pVBCounterParams->verticalBlankCounter = kheadGetVblankNormLatencyCounter_HAL(pKernelHead);
    }

    return NV_OK;
}

NV_STATUS dispcmnCtrlCmdVblankSemControlEnable_IMPL(
    DispCommon *pDispCommon,
    NV0073_CTRL_CMD_SYSTEM_VBLANK_SEM_CONTROL_ENABLE_PARAMS *pParams
)
{
    OBJGPU   *pGpu   = DISPAPI_GET_GPU(pDispCommon);
    RsClient *pClient = RES_GET_CLIENT(pDispCommon);
    NvHandle  hParent = RES_GET_PARENT_HANDLE(pDispCommon);
    RM_API   *pRmApi = GPU_GET_PHYSICAL_RMAPI(DISPAPI_GET_GPU(pDispCommon));
    NV_STATUS status = NV_OK;

    // Get the right pGpu from subdevice instance given by client
    status = dispapiSetUnicastAndSynchronize_HAL(
                               staticCast(pDispCommon, DisplayApi),
                               DISPAPI_GET_GPUGRP(pDispCommon),
                               &pGpu,
                               NULL,
                               pParams->subDeviceInstance);

    if (status != NV_OK)
    {
        return status;
    }

    //
    // Note: memRegisterWithGsp() is a noop when either (a) we're not
    // operating as a GSP client, or (b) the hMemory is already registered
    // with GSP.
    //
    // Also, note that we don't explicitly unregister here in the
    // !pParams->bEnable case: that could unregister the memory out from
    // under other uses of this hMemory on GSP (e.g., other vblank semaphore
    // controls).  Instead, we rely on the hMemory getting unregistered when
    // the 'struct Memory' is freed.
    //
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                          memRegisterWithGsp(pGpu, pClient, hParent, pParams->hMemory));

    //
    // NV0073_CTRL_CMD_SYSTEM_VBLANK_SEM_CONTROL_ENABLE_PARAMS and
    // NV0073_CTRL_CMD_INTERNAL_VBLANK_SEM_CONTROL_ENABLE_PARAMS are
    // equivalent, so just pass pParams through.
    //
    return pRmApi->Control(pRmApi,
                           pClient->hClient,
                           RES_GET_HANDLE(pDispCommon),
                           NV0073_CTRL_CMD_INTERNAL_VBLANK_SEM_CONTROL_ENABLE,
                           pParams,
                           sizeof(*pParams));
}

/*
 * @brief This call engages the WAR for VR where the Pstate
 *        switching can cause delay in serving Vblank interrupts
 *        by servicing disp interrupts inline.
 *
 * @return
 *   NV_OK
 *     The request successfully completed.
 *   NV_ERR_INVALID_ARGUMENT
 *     Invalid argument is passed.
 */
NV_STATUS
dispcmnCtrlCmdInlineDispIntrServiceWarForVr_IMPL
(
    DispCommon *pDispCommon,
    NV0073_CTRL_SYSTEM_INLINE_DISP_INTR_SERVICE_WAR_FOR_VR_PARAMS *pParams
)
{
    OBJGPU   *pGpu  = NULL;
    RM_API   *pRmApi;
    NV_STATUS status;
    KernelDisplay *pKernelDisplay = NULL;

    // Get the right pGpu from subdevice instance given by client
    status = dispapiSetUnicastAndSynchronize_HAL(
                               staticCast(pDispCommon, DisplayApi),
                               DISPAPI_GET_GPUGRP(pDispCommon),
                               &pGpu,
                               NULL,
                               pParams->subDeviceInstance);

    if (status != NV_OK)
    {
        return status;
    }

    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY)

    pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);
    if (pKernelDisplay == NULL)
        SLI_LOOP_RETURN(NV_ERR_INVALID_STATE);

    pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

    status = pRmApi->Control(pRmApi,
                           RES_GET_CLIENT_HANDLE(pDispCommon),
                           RES_GET_HANDLE(pDispCommon),
                           NV0073_CTRL_CMD_INTERNAL_INLINE_DISP_INTR_SERVICE_WAR_FOR_VR,
                           pParams,
                           sizeof(*pParams));

    SLI_LOOP_END

    return NV_OK;
}

