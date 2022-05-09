/*
 * SPDX-FileCopyrightText: Copyright (c) 2026-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "os/os.h"
#include "core/hal.h"
#include "core/info_block.h"
#include "core/locks.h"
#include "gpu/gpu.h"
#include "gpu/subdevice/subdevice.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "kernel/gpu/nvlink/kernel_nvlink.h"
#include "kernel/gpu/mem_sys/kern_mem_sys.h"
#include "vgpu/rpc.h"
#include "nvRmReg.h"

//
// subdeviceCtrlCmdBusGetNvlinkCaps
//    Get the Nvlink global capabilities
//
NV_STATUS
subdeviceCtrlCmdBusGetNvlinkCaps_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_CMD_NVLINK_GET_NVLINK_CAPS_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);

    return knvlinkCtrlCmdBusGetNvlinkCaps(pGpu, pParams);
}

//
// knvlinkCtrlCmdBusGetNvlinkCaps
//    Inner function of subdeviceCtrlCmdBusGetNvlinkCaps for internal RM direct function call
//    Get the Nvlink global capabilities
//
NV_STATUS
knvlinkCtrlCmdBusGetNvlinkCaps
(
    OBJGPU *pGpu,
    NV2080_CTRL_CMD_NVLINK_GET_NVLINK_CAPS_PARAMS *pParams
)
{
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    KernelNvlink     *pKernelNvlink     = GPU_GET_KERNEL_NVLINK(pGpu);
    NV_STATUS         status            = NV_OK;
    NvBool            bMIGNvLinkP2PSupported = ((pKernelMIGManager != NULL) &&
                                                kmigmgrIsMIGNvlinkP2PSupported(pGpu, pKernelMIGManager));
    NvU8              tempCaps[NV2080_CTRL_NVLINK_CAPS_TBL_SIZE];

    //
    // vGPU:
    //
    // Since vGPU does all real hardware management in the
    // host, if we are in guest OS (where IS_VIRTUAL(pGpu) is true),
    // do an RPC to the host to get blacklist information from host RM
    //
    if (IS_VIRTUAL(pGpu))
    {
        CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
        RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams;

        NV_RM_RPC_CONTROL(pGpu, pRmCtrlParams->hClient, pRmCtrlParams->hObject, pRmCtrlParams->cmd,
                          pRmCtrlParams->pParams, pRmCtrlParams->paramsSize, status);
        return status;
    }

    portMemSet(tempCaps, 0, NV2080_CTRL_NVLINK_CAPS_TBL_SIZE);

    // Initialize link masks to 0
    pParams->enabledLinkMask    = 0;
    pParams->discoveredLinkMask = 0;

    if (pKernelNvlink == NULL)
    {
        NV_PRINTF(LEVEL_INFO, "NVLink is unavailable. Returning.\n");
        status = NV_OK;
        return status;
    }

    // With MIG memory partitioning, NvLink P2P or sysmem accesses are not allowed
    if (bMIGNvLinkP2PSupported)
    {
        RMCTRL_SET_CAP(tempCaps, NV2080_CTRL_NVLINK_CAPS, _SUPPORTED);
        RMCTRL_SET_CAP(tempCaps, NV2080_CTRL_NVLINK_CAPS, _SYSMEM_ACCESS);
        RMCTRL_SET_CAP(tempCaps, NV2080_CTRL_NVLINK_CAPS, _P2P_SUPPORTED);
        RMCTRL_SET_CAP(tempCaps, NV2080_CTRL_NVLINK_CAPS, _P2P_ATOMICS);

        //
        // Some links might have passed receiver detect (bridge is present),
        // but might have failed to transition to safe mode (marginal links)
        // Update connectedLinks and bridgedLinks mask for these links
        //
        knvlinkFilterBridgeLinks_HAL(pGpu, pKernelNvlink);

        //
        // This GPU supports SLI bridge sensing if any of the links
        // support bridge sensing.
        //
        if (pKernelNvlink->bridgeSensableLinks)
        {
            RMCTRL_SET_CAP(tempCaps, NV2080_CTRL_NVLINK_CAPS, _SLI_BRIDGE_SENSABLE);
        }

        // This GPU has an SLI bridge if any of the links are bridged
        if (pKernelNvlink->bridgedLinks)
        {
            RMCTRL_SET_CAP(tempCaps, NV2080_CTRL_NVLINK_CAPS, _SLI_BRIDGE);
        }

        // NVLink versions beyond the first support sysmem atomics
        if (pKernelNvlink->ipVerNvlink != NV2080_CTRL_NVLINK_CAPS_NVLINK_VERSION_1_0)
        {
            RMCTRL_SET_CAP(tempCaps, NV2080_CTRL_NVLINK_CAPS, _SYSMEM_ATOMICS);
        }
    }

    switch (pKernelNvlink->ipVerNvlink)
    {
        case NV2080_CTRL_NVLINK_CAPS_NVLINK_VERSION_3_1:
        {
            pParams->lowestNvlinkVersion  = NV2080_CTRL_NVLINK_CAPS_NVLINK_VERSION_3_1;
            pParams->highestNvlinkVersion = NV2080_CTRL_NVLINK_CAPS_NVLINK_VERSION_3_1;
            pParams->lowestNciVersion     = NV2080_CTRL_NVLINK_CAPS_NCI_VERSION_3_1;
            pParams->highestNciVersion    = NV2080_CTRL_NVLINK_CAPS_NCI_VERSION_3_1;

            // Supported power states
            RMCTRL_SET_CAP(tempCaps, NV2080_CTRL_NVLINK_CAPS, _POWER_STATE_L0);
            RMCTRL_SET_CAP(tempCaps, NV2080_CTRL_NVLINK_CAPS, _POWER_STATE_L2);
            break;
        }
        case NV2080_CTRL_NVLINK_CAPS_NVLINK_VERSION_3_0:
        {
            pParams->lowestNvlinkVersion  = NV2080_CTRL_NVLINK_CAPS_NVLINK_VERSION_3_0;
            pParams->highestNvlinkVersion = NV2080_CTRL_NVLINK_CAPS_NVLINK_VERSION_3_0;
            pParams->lowestNciVersion     = NV2080_CTRL_NVLINK_CAPS_NCI_VERSION_3_0;
            pParams->highestNciVersion    = NV2080_CTRL_NVLINK_CAPS_NCI_VERSION_3_0;

            // Supported power states
            RMCTRL_SET_CAP(tempCaps, NV2080_CTRL_NVLINK_CAPS, _POWER_STATE_L0);
            RMCTRL_SET_CAP(tempCaps, NV2080_CTRL_NVLINK_CAPS, _POWER_STATE_L2);
            break;
        }
        case NV2080_CTRL_NVLINK_CAPS_NVLINK_VERSION_2_2:
        {
            pParams->lowestNvlinkVersion  = NV2080_CTRL_NVLINK_CAPS_NVLINK_VERSION_2_2;
            pParams->highestNvlinkVersion = NV2080_CTRL_NVLINK_CAPS_NVLINK_VERSION_2_2;
            pParams->lowestNciVersion     = NV2080_CTRL_NVLINK_CAPS_NCI_VERSION_2_2;
            pParams->highestNciVersion    = NV2080_CTRL_NVLINK_CAPS_NCI_VERSION_2_2;

            // Supported power states
            RMCTRL_SET_CAP(tempCaps, NV2080_CTRL_NVLINK_CAPS, _POWER_STATE_L0);
            RMCTRL_SET_CAP(tempCaps, NV2080_CTRL_NVLINK_CAPS, _POWER_STATE_L2);
            break;
        }
        case NV2080_CTRL_NVLINK_CAPS_NVLINK_VERSION_2_0:
        {
            pParams->lowestNvlinkVersion  = NV2080_CTRL_NVLINK_CAPS_NVLINK_VERSION_2_0;
            pParams->highestNvlinkVersion = NV2080_CTRL_NVLINK_CAPS_NVLINK_VERSION_2_0;
            pParams->lowestNciVersion     = NV2080_CTRL_NVLINK_CAPS_NCI_VERSION_2_0;
            pParams->highestNciVersion    = NV2080_CTRL_NVLINK_CAPS_NCI_VERSION_2_0;

            // Supported power states
            RMCTRL_SET_CAP(tempCaps, NV2080_CTRL_NVLINK_CAPS, _POWER_STATE_L0);
            break;
        }
        default:
        {
            pParams->lowestNvlinkVersion  = NV2080_CTRL_NVLINK_CAPS_NVLINK_VERSION_1_0;
            pParams->highestNvlinkVersion = NV2080_CTRL_NVLINK_CAPS_NVLINK_VERSION_1_0;
            pParams->lowestNciVersion     = NV2080_CTRL_NVLINK_CAPS_NCI_VERSION_1_0;
            pParams->highestNciVersion    = NV2080_CTRL_NVLINK_CAPS_NCI_VERSION_1_0;

            // Supported power states
            RMCTRL_SET_CAP(tempCaps, NV2080_CTRL_NVLINK_CAPS, _POWER_STATE_L0);
            break;
        }
    }

    portMemCopy(&pParams->capsTbl, NV2080_CTRL_NVLINK_CAPS_TBL_SIZE, tempCaps, NV2080_CTRL_NVLINK_CAPS_TBL_SIZE);

    pParams->discoveredLinkMask = knvlinkGetDiscoveredLinkMask(pGpu, pKernelNvlink);
    pParams->enabledLinkMask    = knvlinkGetEnabledLinkMask(pGpu, pKernelNvlink);

    return status;
}

//
// subdeviceCtrlCmdBusGetNvlinkStatus
//    Get the Nvlink per link capabilities
//
NV_STATUS
subdeviceCtrlCmdBusGetNvlinkStatus_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_CMD_NVLINK_GET_NVLINK_STATUS_PARAMS *pParams
)
{
    OBJGPU           *pGpu              = GPU_RES_GET_GPU(pSubdevice);
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    KernelNvlink     *pKernelNvlink     = GPU_GET_KERNEL_NVLINK(pGpu);

    NvBool bMIGNvLinkP2PSupported = ((pKernelMIGManager != NULL) &&
                                     kmigmgrIsMIGNvlinkP2PSupported(pGpu, pKernelMIGManager));
    OBJGPU   *remotePeer0 = NULL;
    NV_STATUS status     = NV_OK;
    NvU8   i = 0;
    NvU8   tempCaps[NV2080_CTRL_NVLINK_CAPS_TBL_SIZE];
    NvU32  r = 0;
    NvBool bPeerLink, bSysmemLink, bSwitchLink;
    NV2080_CTRL_NVLINK_GET_LINK_AND_CLOCK_INFO_PARAMS nvlinkLinkAndClockInfoParams;

    //
    // vGPU:
    //
    // Since vGPU does all real hardware management in the
    // host, if we are in guest OS (where IS_VIRTUAL(pGpu) is true),
    // do an RPC to the host to get blacklist information from host RM
    //
    if (IS_VIRTUAL(pGpu))
    {
        // RPC for this RmCtrl was implemented as an effort of enabling NVLINK P2P
        // on vGPU. As NVLINK P2P is supported Pascal+ onwards, we return NOT_SUPPORTED
        // pre-Pascal.
        if (IsPASCALorBetter(pGpu))
        {
            CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
            RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams;

            NV_RM_RPC_CONTROL(pGpu, pRmCtrlParams->hClient, pRmCtrlParams->hObject, pRmCtrlParams->cmd,
                              pRmCtrlParams->pParams, pRmCtrlParams->paramsSize, status);
            return status;
        }
        else
        {
            return NV_ERR_NOT_SUPPORTED;
        }
    }

    // Initialize link mask to 0
    pParams->enabledLinkMask = 0;

    if ((pKernelNvlink == NULL) || !bMIGNvLinkP2PSupported)
    {
        NV_PRINTF(LEVEL_INFO, "NVLink is unavailable. Returning.\n");
        status = NV_OK;
        return status;
    }

    portMemSet(tempCaps, 0, NV2080_CTRL_NVLINK_CAPS_TBL_SIZE);

    pParams->enabledLinkMask = pKernelNvlink->enabledLinks;

    r = pParams->enabledLinkMask;
    while (r >>= 1 ) i++;

    NV_ASSERT(i <= NV2080_CTRL_NVLINK_MAX_LINKS);

    // Get the remote ends of the links from the nvlink core
    if (!knvlinkIsForcedConfig(pGpu, pKernelNvlink) &&
        !(IS_RTLSIM(pGpu) && !pKernelNvlink->bForceEnableCoreLibRtlsims))
    {
        // Get the nvlink connections for this device from the core
        knvlinkCoreGetRemoteDeviceInfo(pGpu, pKernelNvlink);
    }

    //
    // Some links might have passed receiver detect (bridge is present),
    // but might have failed to transition to safe mode (marginal links)
    // Update connectedLinks and bridgedLinks mask for these links
    //
    knvlinkFilterBridgeLinks_HAL(pGpu, pKernelNvlink);

    portMemSet(&nvlinkLinkAndClockInfoParams, 0, sizeof(nvlinkLinkAndClockInfoParams));

    nvlinkLinkAndClockInfoParams.linkMask = pParams->enabledLinkMask;

    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_NVLINK_GET_LINK_AND_CLOCK_INFO,
                                 (void *)&nvlinkLinkAndClockInfoParams,
                                 sizeof(nvlinkLinkAndClockInfoParams));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to collect nvlink status info!\n");
        return status;
    }

    FOR_EACH_INDEX_IN_MASK(32, i, pParams->enabledLinkMask)
    {
        bPeerLink   = NV_FALSE;
        bSysmemLink = NV_FALSE;
        bSwitchLink = NV_FALSE;
        NV2080_CTRL_NVLINK_GET_LINK_AND_CLOCK_VALUES *pLinkAndClockValues;

        pLinkAndClockValues = &nvlinkLinkAndClockInfoParams.linkInfo[i];

        portMemSet(tempCaps, 0, NV2080_CTRL_NVLINK_CAPS_TBL_SIZE);

        (pKernelNvlink->getProperty(pKernelNvlink, PDB_PROP_KNVLINK_ENABLED)) ?
            (RMCTRL_SET_CAP(tempCaps, NV2080_CTRL_NVLINK_CAPS, _VALID)) : 0;

        if (pLinkAndClockValues->bLinkConnectedToSystem)
        {
            // Tag as a Sysmem link
            bSysmemLink = NV_TRUE;

            RMCTRL_SET_CAP(tempCaps, NV2080_CTRL_NVLINK_CAPS, _SYSMEM_ACCESS);

            // NVLink versions beyond the first support sysmem atomics
            if (pKernelNvlink->ipVerNvlink != NV2080_CTRL_NVLINK_CAPS_NVLINK_VERSION_1_0 )
            {
                RMCTRL_SET_CAP(tempCaps, NV2080_CTRL_NVLINK_CAPS, _SYSMEM_ATOMICS);
            }
        }

        if (pLinkAndClockValues->bLinkConnectedToPeer)
        {
            // Tag as Peer link
            bPeerLink = NV_TRUE;

            RMCTRL_SET_CAP(tempCaps, NV2080_CTRL_NVLINK_CAPS, _P2P_SUPPORTED);
            RMCTRL_SET_CAP(tempCaps, NV2080_CTRL_NVLINK_CAPS, _P2P_ATOMICS);
        }

        // Indicate per-link bridge sense status
        if (pKernelNvlink->bridgeSensableLinks & NVBIT(i))
        {
            RMCTRL_SET_CAP(tempCaps, NV2080_CTRL_NVLINK_CAPS, _SLI_BRIDGE_SENSABLE);
        }

        // Indicate per-link bridge status
        if (pKernelNvlink->bridgedLinks & NVBIT(i))
        {
            RMCTRL_SET_CAP(tempCaps, NV2080_CTRL_NVLINK_CAPS, _SLI_BRIDGE);
        }

        // Set the power states caps
        switch (pKernelNvlink->ipVerNvlink)
        {
            case NV2080_CTRL_NVLINK_CAPS_NVLINK_VERSION_2_0:
                RMCTRL_SET_CAP(tempCaps, NV2080_CTRL_NVLINK_CAPS, _POWER_STATE_L0);
                break;
            case NV2080_CTRL_NVLINK_CAPS_NVLINK_VERSION_3_1:
            case NV2080_CTRL_NVLINK_CAPS_NVLINK_VERSION_3_0:
            case NV2080_CTRL_NVLINK_CAPS_NVLINK_VERSION_2_2:
                RMCTRL_SET_CAP(tempCaps, NV2080_CTRL_NVLINK_CAPS, _POWER_STATE_L0);
                if (pKernelNvlink->getProperty(pKernelNvlink, PDB_PROP_KNVLINK_L2_POWER_STATE_ENABLED))
                {
                    RMCTRL_SET_CAP(tempCaps, NV2080_CTRL_NVLINK_CAPS, _POWER_STATE_L2);
                }
                break;

            default:
                RMCTRL_SET_CAP(tempCaps, NV2080_CTRL_NVLINK_CAPS, _POWER_STATE_L0);
                break;
        }

        portMemCopy(&pParams->linkInfo[i].capsTbl, NV2080_CTRL_NVLINK_CAPS_TBL_SIZE, tempCaps, NV2080_CTRL_NVLINK_CAPS_TBL_SIZE);

        pParams->linkInfo[i].phyType         = NV2080_CTRL_NVLINK_STATUS_PHY_NVHS;
        pParams->linkInfo[i].subLinkWidth    = pLinkAndClockValues->subLinkWidth;
        pParams->linkInfo[i].linkState       = pLinkAndClockValues->linkState;
        pParams->linkInfo[i].txSublinkStatus = (NvU8) pLinkAndClockValues->txSublinkState;
        pParams->linkInfo[i].rxSublinkStatus = (NvU8) pLinkAndClockValues->rxSublinkState;

        // Initialize the lane reversal state information for the link
        pParams->linkInfo[i].bLaneReversal = pLinkAndClockValues->bLaneReversal;

        switch (pKernelNvlink->ipVerNvlink)
        {

            case NV2080_CTRL_NVLINK_CAPS_NVLINK_VERSION_3_1:
                pParams->linkInfo[i].nvlinkVersion = NV2080_CTRL_NVLINK_STATUS_NVLINK_VERSION_3_1;
                pParams->linkInfo[i].nciVersion    = NV2080_CTRL_NVLINK_STATUS_NCI_VERSION_3_1;
                break;
            case NV2080_CTRL_NVLINK_CAPS_NVLINK_VERSION_3_0:
                pParams->linkInfo[i].nvlinkVersion = NV2080_CTRL_NVLINK_STATUS_NVLINK_VERSION_3_0;
                pParams->linkInfo[i].nciVersion    = NV2080_CTRL_NVLINK_STATUS_NCI_VERSION_3_0;
                break;
            case NV2080_CTRL_NVLINK_CAPS_NVLINK_VERSION_2_2:
                pParams->linkInfo[i].nvlinkVersion = NV2080_CTRL_NVLINK_STATUS_NVLINK_VERSION_2_2;
                pParams->linkInfo[i].nciVersion    = NV2080_CTRL_NVLINK_STATUS_NCI_VERSION_2_2;
                break;
            case NV2080_CTRL_NVLINK_CAPS_NVLINK_VERSION_2_0:
                pParams->linkInfo[i].nvlinkVersion = NV2080_CTRL_NVLINK_STATUS_NVLINK_VERSION_2_0;
                pParams->linkInfo[i].nciVersion    = NV2080_CTRL_NVLINK_STATUS_NCI_VERSION_2_0;
                break;
            default:
                pParams->linkInfo[i].nvlinkVersion = NV2080_CTRL_NVLINK_STATUS_NVLINK_VERSION_1_0;
                pParams->linkInfo[i].nciVersion    = NV2080_CTRL_NVLINK_STATUS_NCI_VERSION_1_0;
                break;
        }
        pParams->linkInfo[i].phyVersion    = NV2080_CTRL_NVLINK_STATUS_NVHS_VERSION_1_0;

        // Initialize the connection information for the link
        pParams->linkInfo[i].connected                   = NV2080_CTRL_NVLINK_STATUS_CONNECTED_FALSE;
        pParams->linkInfo[i].remoteDeviceLinkNumber      = NV2080_CTRL_NVLINK_STATUS_REMOTE_LINK_NUMBER_INVALID;
        pParams->linkInfo[i].remoteDeviceInfo.deviceType = NV2080_CTRL_NVLINK_DEVICE_INFO_DEVICE_TYPE_NONE;
        pParams->linkInfo[i].localDeviceLinkNumber       = i;
        pParams->linkInfo[i].laneRxdetStatusMask         = pKernelNvlink->nvlinkLinks[i].laneRxdetStatusMask;

        // Set the device information for the local end of the link
        pParams->linkInfo[i].localDeviceInfo.domain      = gpuGetDomain(pGpu);
        pParams->linkInfo[i].localDeviceInfo.bus         = gpuGetBus(pGpu);
        pParams->linkInfo[i].localDeviceInfo.device      = gpuGetDevice(pGpu);
        pParams->linkInfo[i].localDeviceInfo.function    = 0;
        pParams->linkInfo[i].localDeviceInfo.pciDeviceId = pGpu->idInfo.PCIDeviceID;
        pParams->linkInfo[i].localDeviceInfo.deviceType  = NV2080_CTRL_NVLINK_DEVICE_INFO_DEVICE_TYPE_GPU;
#if defined(INCLUDE_NVLINK_LIB)
        if (pKernelNvlink->nvlinkLinks[i].core_link)
        {
            pParams->linkInfo[i].localLinkSid = pKernelNvlink->nvlinkLinks[i].core_link->localSid;
        }
#endif

        // Record the local end of the link's deviceIdFlags
        if(pGpu->idInfo.PCIDeviceID)
        {
            pParams->linkInfo[i].localDeviceInfo.deviceIdFlags =
                    FLD_SET_DRF(2080_CTRL_NVLINK, _DEVICE_INFO, _DEVICE_ID_FLAGS, _PCI,
                        pParams->linkInfo[i].localDeviceInfo.deviceIdFlags);
        }

        //
        // Get clock related state
        // NOTE: to be depricated HW terminology conforming versions
        //
        pParams->linkInfo[i].nvlinkLinkClockKHz        = pLinkAndClockValues->nvlinkLinkClockKHz;
        pParams->linkInfo[i].nvlinkRefClkSpeedKHz      = nvlinkLinkAndClockInfoParams.nvlinkRefClkSpeedKHz;
        pParams->linkInfo[i].nvlinkCommonClockSpeedKHz = pParams->linkInfo[i].nvlinkLinkClockKHz / 16;

        pParams->linkInfo[i].nvlinkCommonClockSpeedMhz = pParams->linkInfo[i].nvlinkCommonClockSpeedKHz / 1000;

        // Clock speed and Data rate info conforming with HW terminology
        pParams->linkInfo[i].nvlinkLineRateMbps      = pLinkAndClockValues->nvlinkLineRateMbps;
        pParams->linkInfo[i].nvlinkLinkClockMhz      = pLinkAndClockValues->nvlinkLinkClockMhz;
        pParams->linkInfo[i].nvlinkLinkDataRateKiBps = pLinkAndClockValues->nvlinkLinkDataRateKiBps;
        pParams->linkInfo[i].nvlinkRefClkType        = pLinkAndClockValues->nvlinkRefClkType;
        pParams->linkInfo[i].nvlinkRefClkSpeedMhz    = pLinkAndClockValues->nvlinkReqLinkClockMhz;


#if defined(INCLUDE_NVLINK_LIB)

        if (pKernelNvlink->nvlinkLinks[i].remoteEndInfo.bConnected)
        {
            pParams->linkInfo[i].connected              = NV2080_CTRL_NVLINK_STATUS_CONNECTED_TRUE;
            pParams->linkInfo[i].remoteDeviceLinkNumber = (NvU8) pKernelNvlink->nvlinkLinks[i].remoteEndInfo.linkNumber;
            pParams->linkInfo[i].remoteLinkSid          = pKernelNvlink->nvlinkLinks[i].remoteEndInfo.chipSid;

            // Set the device information for the remote end of the link
            pParams->linkInfo[i].remoteDeviceInfo.domain      = pKernelNvlink->nvlinkLinks[i].remoteEndInfo.domain;
            pParams->linkInfo[i].remoteDeviceInfo.bus         = pKernelNvlink->nvlinkLinks[i].remoteEndInfo.bus;
            pParams->linkInfo[i].remoteDeviceInfo.device      = pKernelNvlink->nvlinkLinks[i].remoteEndInfo.device;
            pParams->linkInfo[i].remoteDeviceInfo.function    = pKernelNvlink->nvlinkLinks[i].remoteEndInfo.function;
            pParams->linkInfo[i].remoteDeviceInfo.pciDeviceId = pKernelNvlink->nvlinkLinks[i].remoteEndInfo.pciDeviceId;
            pParams->linkInfo[i].remoteDeviceInfo.deviceType  = pKernelNvlink->nvlinkLinks[i].remoteEndInfo.deviceType;

            // Update the device Id flags for PCI
            if (pKernelNvlink->nvlinkLinks[i].remoteEndInfo.pciDeviceId)
            {
                pParams->linkInfo[i].remoteDeviceInfo.deviceIdFlags |=
                    FLD_SET_DRF(2080_CTRL_NVLINK, _DEVICE_INFO, _DEVICE_ID_FLAGS, _PCI,
                                pParams->linkInfo[i].remoteDeviceInfo.deviceIdFlags);
            }

            // Check the PCI dbdf values to confirm the device on remote end
            if (NV2080_CTRL_NVLINK_DEVICE_INFO_DEVICE_ID_FLAGS_PCI & pParams->linkInfo[i].remoteDeviceInfo.deviceIdFlags)
            {
                if (!knvlinkIsP2pLoopbackSupportedPerLink(pGpu, pKernelNvlink, i))
                {
                    pParams->linkInfo[i].loopProperty = NV2080_CTRL_NVLINK_STATUS_LOOP_PROPERTY_NONE;
                    continue;
                }
            }

            pParams->linkInfo[i].loopProperty = pParams->linkInfo[i].remoteDeviceLinkNumber == i ?
                                                NV2080_CTRL_NVLINK_STATUS_LOOP_PROPERTY_LOOPBACK :
                                                NV2080_CTRL_NVLINK_STATUS_LOOP_PROPERTY_LOOPOUT;
        }
#endif

        // Per-link ForceConfig handling (non-legacy Arch ForceConfig only)
        if (knvlinkIsForcedConfig(pGpu, pKernelNvlink))
        {
            if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_EMULATION))
            {
                pParams->linkInfo[i].linkState       = NV2080_CTRL_NVLINK_STATUS_LINK_STATE_ACTIVE;
                pParams->linkInfo[i].rxSublinkStatus = NV2080_CTRL_NVLINK_STATUS_SUBLINK_RX_STATE_HIGH_SPEED_1;
                pParams->linkInfo[i].txSublinkStatus = NV2080_CTRL_NVLINK_STATUS_SUBLINK_TX_STATE_HIGH_SPEED_1;
            }

            pParams->linkInfo[i].connected              = NV_TRUE;
            pParams->linkInfo[i].loopProperty           = NV_FALSE;
            pParams->linkInfo[i].remoteDeviceLinkNumber = i;
            if (!pParams->linkInfo[i].nvlinkLinkClockMhz)
                pParams->linkInfo[i].nvlinkLinkClockMhz = pLinkAndClockValues->nvlinkReqLinkClockMhz;

            // Expose remote device as EBRIDGE if forced only sysmem
            if (bSysmemLink && !bPeerLink && !bSwitchLink)
            {
                pParams->linkInfo[i].remoteDeviceInfo.domain      = 0;
                pParams->linkInfo[i].remoteDeviceInfo.bus         = FORCED_SYSMEM_PCI_BUS;
                pParams->linkInfo[i].remoteDeviceInfo.device      = 0;
                pParams->linkInfo[i].remoteDeviceInfo.function    = i;
                pParams->linkInfo[i].remoteDeviceInfo.pciDeviceId = FORCED_SYSMEM_DEVICE_ID;
                pParams->linkInfo[i].remoteDeviceInfo.deviceType  = FORCED_SYSMEM_DEVICE_TYPE;

                pParams->linkInfo[i].remoteDeviceInfo.deviceIdFlags |=
                    FLD_SET_DRF(2080_CTRL_NVLINK, _DEVICE_INFO, _DEVICE_ID_FLAGS, _PCI,
                                pParams->linkInfo[i].remoteDeviceInfo.deviceIdFlags);
            }

            // Expose remote device as GPU if forced only peer
            if (bPeerLink && !bSysmemLink && !bSwitchLink)
            {
                remotePeer0 = gpumgrGetGpu(pGpu->gpuInstance == 0 ? 1 : 0);
                if (NULL == remotePeer0)
                {
                    remotePeer0 = pGpu;
                }

                //
                // Ensure the remote is actually a GPU that supports NVLink.
                // If it is not, we should stick with the current GPU as
                // this is likely a loopback config. See Bug 1786206.
                //
                if (remotePeer0 != pGpu)
                {
                    KernelNvlink *pRemoteKernelNvlink = GPU_GET_KERNEL_NVLINK(remotePeer0);
                    if (pRemoteKernelNvlink)
                    {
                        if (pRemoteKernelNvlink->discoveredLinks == 0)
                        {
                            // There are no links on this remote, fall back to loopback.
                            remotePeer0 = pGpu;
                        }
                    }
                    else
                    {
                        // NVLink not present on this remote, fall back to loopback.
                        remotePeer0 = pGpu;
                    }
                }

                pParams->linkInfo[i].remoteDeviceInfo.domain      = gpuGetDomain(remotePeer0);
                pParams->linkInfo[i].remoteDeviceInfo.bus         = gpuGetBus(remotePeer0);
                pParams->linkInfo[i].remoteDeviceInfo.device      = gpuGetDevice(remotePeer0);
                pParams->linkInfo[i].remoteDeviceInfo.function    = 0;
                pParams->linkInfo[i].remoteDeviceInfo.pciDeviceId = remotePeer0->idInfo.PCIDeviceID;
                pParams->linkInfo[i].remoteDeviceInfo.deviceType  = NV2080_CTRL_NVLINK_DEVICE_INFO_DEVICE_TYPE_GPU;

                // This config is either in loopback or real 1/1 P2P, nothing else.
                if (gpuGetDBDF(remotePeer0) == gpuGetDBDF(pGpu))
                {
                    pParams->linkInfo[i].loopProperty = NV2080_CTRL_NVLINK_STATUS_LOOP_PROPERTY_LOOPBACK;
                }

                pParams->linkInfo[i].remoteDeviceInfo.deviceIdFlags |=
                    FLD_SET_DRF(2080_CTRL_NVLINK, _DEVICE_INFO, _DEVICE_ID_FLAGS, _PCI,
                                pParams->linkInfo[i].remoteDeviceInfo.deviceIdFlags);
            }

            //
            // Expose remote device as Switch if requested
            // Requested can be either forced sysmem and peer or
            // if either and requested as switch
            //
            if ( (bSysmemLink && bPeerLink) ||
                ((bSysmemLink || bPeerLink) && bSwitchLink))
            {
                pParams->linkInfo[i].remoteDeviceInfo.domain      = 0;
                pParams->linkInfo[i].remoteDeviceInfo.bus         = FORCED_SWITCH_PCI_BUS;
                pParams->linkInfo[i].remoteDeviceInfo.device      = 0;
                pParams->linkInfo[i].remoteDeviceInfo.function    = i;
                pParams->linkInfo[i].remoteDeviceInfo.pciDeviceId = FORCED_SWITCH_DEVICE_ID;
                pParams->linkInfo[i].remoteDeviceInfo.deviceType  = NV2080_CTRL_NVLINK_DEVICE_INFO_DEVICE_TYPE_SWITCH;

                pParams->linkInfo[i].remoteDeviceInfo.deviceIdFlags |=
                    FLD_SET_DRF(2080_CTRL_NVLINK, _DEVICE_INFO, _DEVICE_ID_FLAGS, _PCI,
                                pParams->linkInfo[i].remoteDeviceInfo.deviceIdFlags);
            }
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

    return status;
}


/*
 * @brief Get the number of successful error recoveries
 */
NV_STATUS
subdeviceCtrlCmdNvlinkGetErrorRecoveries_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_CMD_NVLINK_GET_ERROR_RECOVERIES_PARAMS *pParams
)
{
    OBJGPU       *pGpu          = GPU_RES_GET_GPU(pSubdevice);
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    NvBool bMIGNvLinkP2PSupported = ((pKernelMIGManager != NULL) &&
                                     kmigmgrIsMIGNvlinkP2PSupported(pGpu, pKernelMIGManager));
    NV_STATUS  status  = NV_OK;
    NvU32      i;

    if ((pKernelNvlink == NULL) || !bMIGNvLinkP2PSupported)
    {
        NV_PRINTF(LEVEL_ERROR, "NVLink is unavailable, failing.\n");
        return NV_ERR_NOT_SUPPORTED;
    }

    FOR_EACH_INDEX_IN_MASK(32, i, pParams->linkMask & pKernelNvlink->enabledLinks)
    {
        pParams->numRecoveries[i] = pKernelNvlink->errorRecoveries[i];

        // Clear the counts
        pKernelNvlink->errorRecoveries[i] = 0;
    }
    FOR_EACH_INDEX_IN_MASK_END;

    return status;
}

//
// subdeviceCtrlCmdNvlinkSetPowerState
//    Set the mask of links to a target power state
//
NV_STATUS
subdeviceCtrlCmdNvlinkSetPowerState_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_NVLINK_SET_POWER_STATE_PARAMS *pParams
)
{
    OBJGPU           *pGpu              = GPU_RES_GET_GPU(pSubdevice);
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    KernelNvlink     *pKernelNvlink     = GPU_GET_KERNEL_NVLINK(pGpu);

    NvBool bMIGNvLinkP2PSupported = ((pKernelMIGManager != NULL) &&
                                     kmigmgrIsMIGNvlinkP2PSupported(pGpu, pKernelMIGManager));
    NV_STATUS status  = NV_OK;

    if ((pKernelNvlink == NULL) || !bMIGNvLinkP2PSupported)
    {
        NV_PRINTF(LEVEL_INFO, "NVLink unavailable. Return\n");
        return NV_ERR_NOT_SUPPORTED;
    }

    // Verify the mask of links requested are enabled on the GPU
    if ((pParams->linkMask & pKernelNvlink->enabledLinks) != pParams->linkMask)
    {
        NV_PRINTF(LEVEL_INFO, "Links not enabled. Return.\n");

        return NV_ERR_INVALID_ARGUMENT;
    }

    switch (pParams->powerState)
    {
        case NV2080_CTRL_NVLINK_POWER_STATE_L0:
        {
            status = knvlinkEnterExitSleep(pGpu, pKernelNvlink,
                                           pParams->linkMask,
                                           NV_FALSE);

            if (status == NV_WARN_MORE_PROCESSING_REQUIRED)
            {
                NV_PRINTF(LEVEL_INFO,
                          "Transition to L0 for GPU%d: linkMask 0x%x in progress... Waiting for "
                          "remote endpoints to request L2 exit\n",
                          pGpu->gpuInstance, pParams->linkMask);

                return NV_WARN_MORE_PROCESSING_REQUIRED;
            }

            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "Error setting power state %d on linkmask 0x%x\n",
                          pParams->powerState, pParams->linkMask);

                return status;
            }
            break;
        }

        case NV2080_CTRL_NVLINK_POWER_STATE_L2:
        {
            status = knvlinkEnterExitSleep(pGpu, pKernelNvlink,
                                           pParams->linkMask,
                                           NV_TRUE);

            if (status == NV_WARN_MORE_PROCESSING_REQUIRED)
            {
                NV_PRINTF(LEVEL_INFO,
                          "Transition to L2 for GPU%d: linkMask 0x%x in progress... Waiting for "
                          "remote endpoints to request L2 entry\n",
                          pGpu->gpuInstance, pParams->linkMask);

                return NV_WARN_MORE_PROCESSING_REQUIRED;
            }

            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "Error setting power state %d on linkmask 0x%x\n",
                          pParams->powerState, pParams->linkMask);

                return status;
            }
            break;
        }

        case NV2080_CTRL_NVLINK_POWER_STATE_L1:
        case NV2080_CTRL_NVLINK_POWER_STATE_L3:
        {
            // L1 and L3 states are not yet supported. Targeted for Ampere
            NV_PRINTF(LEVEL_ERROR, "Unsupported power state %d requested.\n",
                      pParams->powerState);

            return NV_ERR_INVALID_REQUEST;
        }

        default:
        {
            NV_PRINTF(LEVEL_ERROR, "Unsupported power state %d requested.\n",
                      pParams->powerState);

            return NV_ERR_INVALID_REQUEST;
        }
    }

    return status;
}

//
// subdeviceCtrlCmdNvlinkSetNvlinkPeer
//    Set/unset the USE_NVLINK_PEER bit
//
NV_STATUS
subdeviceCtrlCmdNvlinkSetNvlinkPeer_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_NVLINK_SET_NVLINK_PEER_PARAMS *pParams
)
{
    OBJGPU       *pGpu          = GPU_RES_GET_GPU(pSubdevice);
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
    NV_STATUS     status        = NV_OK;
    NV2080_CTRL_NVLINK_ENABLE_NVLINK_PEER_PARAMS enableNvlinkPeerParams;

    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    NvBool bMIGNvLinkP2PSupported = ((pKernelMIGManager != NULL) &&
                                     kmigmgrIsMIGNvlinkP2PSupported(pGpu, pKernelMIGManager));

    if ((pKernelNvlink == NULL) || !bMIGNvLinkP2PSupported)
    {
        NV_PRINTF(LEVEL_INFO, "NVLink unavailable. Return\n");
        return NV_ERR_NOT_SUPPORTED;
    }

    if ((pParams->bEnable == NV_FALSE) &&
        !pKernelNvlink->getProperty(pKernelNvlink, PDB_PROP_KNVLINK_UNSET_NVLINK_PEER_SUPPORTED))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Unsetting USE_NVLINK_PEER field not supported\n");
        return NV_ERR_NOT_SUPPORTED;
    }

    portMemSet(&enableNvlinkPeerParams, 0, sizeof(enableNvlinkPeerParams));
    enableNvlinkPeerParams.peerMask = pParams->peerMask;
    enableNvlinkPeerParams.bEnable  = pParams->bEnable;

    // Update the RM cache to reflect the updated status of USE_NVLINK_PEER
    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_NVLINK_ENABLE_NVLINK_PEER,
                                 (void *)&enableNvlinkPeerParams,
                                 sizeof(enableNvlinkPeerParams));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "GPU%d Failed to update USE_NVLINK_PEER for peer mask 0x%x\n",
                  gpuGetInstance(pGpu), pParams->peerMask);

        return status;
    }

    // Call knvlinkUpdateCurrentConfig to flush settings to the registers
    status = knvlinkUpdateCurrentConfig(pGpu, pKernelNvlink);

    return status;
}
