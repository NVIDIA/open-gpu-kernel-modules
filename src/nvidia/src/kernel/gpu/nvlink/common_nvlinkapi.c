/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#define NVOC_KERNEL_NVLINK_H_PRIVATE_ACCESS_ALLOWED

#include "gpu/gpu.h"
#include "gpu/subdevice/subdevice.h"
#include "vgpu/rpc.h"

#include "kernel/gpu/nvlink/kernel_nvlink.h"
#include "kernel/gpu/nvlink/kernel_ioctrl.h"
#include "kernel/gpu/nvlink/common_nvlink.h"

typedef struct
{
    NvU32     laneRxdetStatusMask;
    NvBool    bConnected;
    NvU32     remoteLinkNumber;
    NvU64     remoteDeviceType;
    NvU64     remoteChipSid;
    NvU32     remoteDomain;
    NvU8      remoteBus;
    NvU8      remoteDevice;
    NvU8      remoteFunction;
    NvU32     remotePciDeviceId;
    NvBool    bLoopbackSupported;
} NvlinkLinkStatus;

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

    return nvlinkCtrlCmdBusGetNvlinkCaps(pGpu, pParams);
}

NV_STATUS
subdeviceCtrlCmdBusGetNvlinkCaps_VF
(
    Subdevice *pSubdevice,
    NV2080_CTRL_CMD_NVLINK_GET_NVLINK_CAPS_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    VGPU_STATIC_INFO *pVSI = GPU_GET_STATIC_INFO(pGpu);

    NV_ASSERT_OR_RETURN(pVSI != NULL, NV_ERR_INVALID_STATE);

    portMemCopy(pParams, sizeof(*pParams),
                &pVSI->nvlinkCaps, sizeof(pVSI->nvlinkCaps));

    return NV_OK;
}

static void _calculateNvlinkCaps
(
    OBJGPU *pGpu,
    NvU64   bridgeSensableLinks,
    NvU32   bridgedLinks,
    NvU32   ipVerNvlink,
    NvBool  bMIGNvLinkP2PSupported,
    NvBool  bNvlinkEnabled,
    NvBool  bIsUncontainedErrorRecoveryCapable,
    NV2080_CTRL_CMD_NVLINK_GET_NVLINK_CAPS_PARAMS *pParams
)
{
    NvU8 tempCaps[NV2080_CTRL_NVLINK_CAPS_TBL_SIZE];
    portMemSet(tempCaps, 0, NV2080_CTRL_NVLINK_CAPS_TBL_SIZE);

    // With MIG memory partitioning, NvLink P2P or sysmem accesses are not allowed
    if (bMIGNvLinkP2PSupported)
    {
        RMCTRL_SET_CAP(tempCaps, NV2080_CTRL_NVLINK_CAPS, _SUPPORTED);
        RMCTRL_SET_CAP(tempCaps, NV2080_CTRL_NVLINK_CAPS, _SYSMEM_ACCESS);
        RMCTRL_SET_CAP(tempCaps, NV2080_CTRL_NVLINK_CAPS, _P2P_SUPPORTED);
        RMCTRL_SET_CAP(tempCaps, NV2080_CTRL_NVLINK_CAPS, _P2P_ATOMICS);

        //
        // This GPU supports SLI bridge sensing if any of the links
        // support bridge sensing.
        //
        if (bridgeSensableLinks != 0)
        {
            RMCTRL_SET_CAP(tempCaps, NV2080_CTRL_NVLINK_CAPS, _SLI_BRIDGE_SENSABLE);
        }

        // This GPU has an SLI bridge if any of the links are bridged
        if (bridgedLinks != 0)
        {
            RMCTRL_SET_CAP(tempCaps, NV2080_CTRL_NVLINK_CAPS, _SLI_BRIDGE);
        }

        // NVLink versions beyond the first support sysmem atomics
        if (ipVerNvlink != NV2080_CTRL_NVLINK_CAPS_NVLINK_VERSION_1_0)
        {
            RMCTRL_SET_CAP(tempCaps, NV2080_CTRL_NVLINK_CAPS, _SYSMEM_ATOMICS);
        }

        if (bNvlinkEnabled && bIsUncontainedErrorRecoveryCapable)
        {
            RMCTRL_SET_CAP(tempCaps, NV2080_CTRL_NVLINK_CAPS, _UNCONTAINED_ERROR_RECOVERY);
        }
    }

    switch (ipVerNvlink)
    {
        case NV2080_CTRL_NVLINK_CAPS_NVLINK_VERSION_5_0:
        {
            pParams->lowestNvlinkVersion  = NV2080_CTRL_NVLINK_CAPS_NVLINK_VERSION_5_0;
            pParams->highestNvlinkVersion = NV2080_CTRL_NVLINK_CAPS_NVLINK_VERSION_5_0;
            pParams->lowestNciVersion     = NV2080_CTRL_NVLINK_CAPS_NCI_VERSION_5_0;
            pParams->highestNciVersion    = NV2080_CTRL_NVLINK_CAPS_NCI_VERSION_5_0;

            // Supported power states
            RMCTRL_SET_CAP(tempCaps, NV2080_CTRL_NVLINK_CAPS, _POWER_STATE_L0);
            RMCTRL_SET_CAP(tempCaps, NV2080_CTRL_NVLINK_CAPS, _POWER_STATE_L1);
            RMCTRL_SET_CAP(tempCaps, NV2080_CTRL_NVLINK_CAPS, _POWER_STATE_L2);
            break;
        }
        case NV2080_CTRL_NVLINK_CAPS_NVLINK_VERSION_4_0:
        {
            pParams->lowestNvlinkVersion  = NV2080_CTRL_NVLINK_CAPS_NVLINK_VERSION_4_0;
            pParams->highestNvlinkVersion = NV2080_CTRL_NVLINK_CAPS_NVLINK_VERSION_4_0;
            pParams->lowestNciVersion     = NV2080_CTRL_NVLINK_CAPS_NCI_VERSION_4_0;
            pParams->highestNciVersion    = NV2080_CTRL_NVLINK_CAPS_NCI_VERSION_4_0;

            // Supported power states
            RMCTRL_SET_CAP(tempCaps, NV2080_CTRL_NVLINK_CAPS, _POWER_STATE_L0);
            RMCTRL_SET_CAP(tempCaps, NV2080_CTRL_NVLINK_CAPS, _POWER_STATE_L1);
            break;
        }
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

    if (bNvlinkEnabled)
    {
        RMCTRL_SET_CAP(tempCaps, NV2080_CTRL_NVLINK_CAPS, _VALID);
    }

    portMemCopy(&pParams->capsTbl, NV2080_CTRL_NVLINK_CAPS_TBL_SIZE, tempCaps, NV2080_CTRL_NVLINK_CAPS_TBL_SIZE);
}

//
// knvlinkCtrlCmdBusGetNvlinkCaps
//    Inner function of subdeviceCtrlCmdBusGetNvlinkCaps for internal RM direct function call
//    Get the Nvlink global capabilities
//
NV_STATUS
nvlinkCtrlCmdBusGetNvlinkCaps
(
    OBJGPU *pGpu,
    NV2080_CTRL_CMD_NVLINK_GET_NVLINK_CAPS_PARAMS *pParams
)
{
    KernelMIGManager *pKernelMIGManager      = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    NvBool            bMIGNvLinkP2PSupported = ((pKernelMIGManager != NULL) &&
                                                kmigmgrIsMIGNvlinkP2PSupported(pGpu, pKernelMIGManager));

    // Initialize link masks to 0
    pParams->enabledLinkMask    = 0;
    pParams->discoveredLinkMask = 0;

    if (IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu) && RMCFG_FEATURE_PLATFORM_GSP)
    {
        return NV_ERR_NOT_SUPPORTED;
    }
    else
    {
        KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
        if (pKernelNvlink == NULL)
        {
            NV_PRINTF(LEVEL_INFO, "Kernel NVLink is unavailable. Returning.\n");
            return NV_OK;
        }
        // With MIG memory partitioning, NvLink P2P or sysmem accesses are not allowed
        if (bMIGNvLinkP2PSupported)
        {
            //
            // Some links might have passed receiver detect (bridge is present),
            // but might have failed to transition to safe mode (marginal links)
            // Update connectedLinks and bridgedLinks mask for these links
            //
            knvlinkFilterBridgeLinks_HAL(pGpu, pKernelNvlink);
        }
        _calculateNvlinkCaps(pGpu, pKernelNvlink->bridgeSensableLinks, pKernelNvlink->bridgedLinks,
                             pKernelNvlink->ipVerNvlink, bMIGNvLinkP2PSupported,
                             pKernelNvlink->getProperty(pNvlink, PDB_PROP_KNVLINK_ENABLED),
                             pKernelNvlink->getProperty(pKernelNvlink, PDB_PROP_KNVLINK_UNCONTAINED_ERROR_RECOVERY_SUPPORTED),
                             pParams);

        pParams->discoveredLinkMask = KNVLINK_GET_MASK(pKernelNvlink, discoveredLinks, 32);
        pParams->discoveredLinks    = knvlinkGetDiscoveredLinkMask(pGpu, pKernelNvlink);
        pParams->enabledLinkMask    = KNVLINK_GET_MASK(pKernelNvlink, enabledLinks, 32);
        pParams->enabledLinks       = knvlinkGetEnabledLinkMask(pGpu, pKernelNvlink);
    }

    return NV_OK;
}

void
static _getNvlinkStatus
(
    OBJGPU *pGpu,
    NV2080_CTRL_INTERNAL_NVLINK_GET_LINK_AND_CLOCK_INFO_PARAMS *nvlinkLinkAndClockInfoParams,
    NvU64 bridgeSensableLinks,
    NvU32 bridgedLinks,
    NvU32 ipVerNvlink,
    NvlinkLinkStatus nvlinkLinks[NVLINK_MAX_LINKS_SW],
    NvBool bNvlinkEnabled,
    NvBool bL2PowerStateEnabled,
    NvBool bForcedConfig,
    NvBool bIsUncontainedErrorRecoveryCapable,
    NvBool bIsGpuDegraded,
    NvBool bIsUncontainedErrorRecoveryActive,
    NV2080_CTRL_CMD_NVLINK_GET_NVLINK_STATUS_PARAMS *pParams
)
{
    NvU8   i = 0;
    NvU8   tempCaps[NV2080_CTRL_NVLINK_CAPS_TBL_SIZE];
    NvU32  r = 0;
    OBJGPU *remotePeer0 = NULL;
    NvBool bPeerLink, bSysmemLink, bSwitchLink;

    r = pParams->enabledLinkMask;
    while (r >>= 1 ) i++;

    NV_ASSERT(i <= NV2080_CTRL_NVLINK_MAX_LINKS);

    FOR_EACH_INDEX_IN_MASK(32, i, pParams->enabledLinkMask)
    {
        bPeerLink   = NV_FALSE;
        bSysmemLink = NV_FALSE;
        bSwitchLink = NV_FALSE;
        NV2080_CTRL_INTERNAL_NVLINK_GET_LINK_AND_CLOCK_VALUES *pLinkAndClockValues;

        pLinkAndClockValues = &nvlinkLinkAndClockInfoParams->linkInfo[i];

        portMemSet(tempCaps, 0, NV2080_CTRL_NVLINK_CAPS_TBL_SIZE);

        if (bNvlinkEnabled)
        {
            RMCTRL_SET_CAP(tempCaps, NV2080_CTRL_NVLINK_CAPS, _VALID);
        }

        if (pLinkAndClockValues->bLinkConnectedToSystem)
        {
            // Tag as a Sysmem link
            bSysmemLink = NV_TRUE;

            RMCTRL_SET_CAP(tempCaps, NV2080_CTRL_NVLINK_CAPS, _SYSMEM_ACCESS);

            // NVLink versions beyond the first support sysmem atomics
            if (ipVerNvlink != NV2080_CTRL_NVLINK_CAPS_NVLINK_VERSION_1_0 )
            {
                RMCTRL_SET_CAP(tempCaps, NV2080_CTRL_NVLINK_CAPS, _SYSMEM_ATOMICS);
            }
        }

        if (nvlinkLinks[i].bConnected)
        {
            // Tag as Peer link
            bPeerLink = NV_TRUE;

            RMCTRL_SET_CAP(tempCaps, NV2080_CTRL_NVLINK_CAPS, _P2P_SUPPORTED);
            RMCTRL_SET_CAP(tempCaps, NV2080_CTRL_NVLINK_CAPS, _P2P_ATOMICS);
        }

        // Indicate per-link bridge sense status
        if (bridgeSensableLinks & NVBIT(i))
        {
            RMCTRL_SET_CAP(tempCaps, NV2080_CTRL_NVLINK_CAPS, _SLI_BRIDGE_SENSABLE);
        }

        // Indicate per-link bridge status
        if (bridgedLinks & NVBIT(i))
        {
            RMCTRL_SET_CAP(tempCaps, NV2080_CTRL_NVLINK_CAPS, _SLI_BRIDGE);
        }

        if (bNvlinkEnabled && bIsUncontainedErrorRecoveryCapable)
        {
            RMCTRL_SET_CAP(tempCaps, NV2080_CTRL_NVLINK_CAPS, _UNCONTAINED_ERROR_RECOVERY);
        }

        // Set the power states caps
        switch (ipVerNvlink)
        {
            case NV2080_CTRL_NVLINK_CAPS_NVLINK_VERSION_2_0:
                RMCTRL_SET_CAP(tempCaps, NV2080_CTRL_NVLINK_CAPS, _POWER_STATE_L0);
                break;
            case NV2080_CTRL_NVLINK_CAPS_NVLINK_VERSION_5_0:
            case NV2080_CTRL_NVLINK_CAPS_NVLINK_VERSION_4_0:
            case NV2080_CTRL_NVLINK_CAPS_NVLINK_VERSION_3_1:
            case NV2080_CTRL_NVLINK_CAPS_NVLINK_VERSION_3_0:
            case NV2080_CTRL_NVLINK_CAPS_NVLINK_VERSION_2_2:
                RMCTRL_SET_CAP(tempCaps, NV2080_CTRL_NVLINK_CAPS, _POWER_STATE_L0);
                if (bL2PowerStateEnabled)
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

        switch (ipVerNvlink)
        {
            case NV2080_CTRL_NVLINK_CAPS_NVLINK_VERSION_5_0:
                pParams->linkInfo[i].nvlinkVersion = NV2080_CTRL_NVLINK_STATUS_NVLINK_VERSION_5_0;
                pParams->linkInfo[i].nciVersion    = NV2080_CTRL_NVLINK_STATUS_NCI_VERSION_5_0;
                break;
            case NV2080_CTRL_NVLINK_CAPS_NVLINK_VERSION_4_0:
                pParams->linkInfo[i].nvlinkVersion = NV2080_CTRL_NVLINK_STATUS_NVLINK_VERSION_4_0;
                pParams->linkInfo[i].nciVersion    = NV2080_CTRL_NVLINK_STATUS_NCI_VERSION_4_0;
                break;
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
        pParams->linkInfo[i].laneRxdetStatusMask         = nvlinkLinks[i].laneRxdetStatusMask;

        // Set the device information for the local end of the link
        pParams->linkInfo[i].localDeviceInfo.domain      = gpuGetDomain(pGpu);
        pParams->linkInfo[i].localDeviceInfo.bus         = gpuGetBus(pGpu);
        pParams->linkInfo[i].localDeviceInfo.device      = gpuGetDevice(pGpu);
        pParams->linkInfo[i].localDeviceInfo.function    = 0;
        pParams->linkInfo[i].localDeviceInfo.pciDeviceId = pGpu->idInfo.PCIDeviceID;
        pParams->linkInfo[i].localDeviceInfo.deviceType  = NV2080_CTRL_NVLINK_DEVICE_INFO_DEVICE_TYPE_GPU;

        // Record the local end of the link's deviceIdFlags
        if(pGpu->idInfo.PCIDeviceID)
        {
            pParams->linkInfo[i].localDeviceInfo.deviceIdFlags =
                    FLD_SET_DRF(2080_CTRL_NVLINK, _DEVICE_INFO, _DEVICE_ID_FLAGS, _PCI,
                        pParams->linkInfo[i].localDeviceInfo.deviceIdFlags);
        }

        pParams->linkInfo[i].localDeviceInfo.fabricRecoveryStatusMask = 0x0;
        if (bIsGpuDegraded)
        {
            pParams->linkInfo[i].localDeviceInfo.fabricRecoveryStatusMask =
                FLD_SET_DRF(2080_CTRL_NVLINK_DEVICE_INFO, _FABRIC_RECOVERY_STATUS_MASK, _GPU_DEGRADED, _TRUE,
                            pParams->linkInfo[i].localDeviceInfo.fabricRecoveryStatusMask);
        }
        else if (bIsUncontainedErrorRecoveryActive)
        {
            pParams->linkInfo[i].localDeviceInfo.fabricRecoveryStatusMask =
                FLD_SET_DRF(2080_CTRL_NVLINK_DEVICE_INFO, _FABRIC_RECOVERY_STATUS_MASK, _UNCONTAINED_ERROR_RECOVERY, _ACTIVE,
                            pParams->linkInfo[i].localDeviceInfo.fabricRecoveryStatusMask);
        }

        //
        // Get clock related state
        // NOTE: to be depricated HW terminology conforming versions
        //
        pParams->linkInfo[i].nvlinkLinkClockKHz        = pLinkAndClockValues->nvlinkLinkClockKHz;
        pParams->linkInfo[i].nvlinkRefClkSpeedKHz      = nvlinkLinkAndClockInfoParams->nvlinkRefClkSpeedKHz;
        pParams->linkInfo[i].nvlinkCommonClockSpeedKHz = pParams->linkInfo[i].nvlinkLinkClockKHz / 16;

        pParams->linkInfo[i].nvlinkCommonClockSpeedMhz = pParams->linkInfo[i].nvlinkCommonClockSpeedKHz / 1000;

        // Clock speed and Data rate info conforming with HW terminology
        pParams->linkInfo[i].nvlinkLineRateMbps      = pLinkAndClockValues->nvlinkLineRateMbps;
        pParams->linkInfo[i].nvlinkLinkClockMhz      = pLinkAndClockValues->nvlinkLinkClockMhz;
        pParams->linkInfo[i].nvlinkLinkDataRateKiBps = pLinkAndClockValues->nvlinkLinkDataRateKiBps;
        pParams->linkInfo[i].nvlinkRefClkType        = pLinkAndClockValues->nvlinkRefClkType;
        pParams->linkInfo[i].nvlinkRefClkSpeedMhz    = pLinkAndClockValues->nvlinkReqLinkClockMhz;

        pParams->linkInfo[i].nvlinkMinL1Threshold    = pLinkAndClockValues->nvlinkMinL1Threshold;
        pParams->linkInfo[i].nvlinkMaxL1Threshold    = pLinkAndClockValues->nvlinkMaxL1Threshold;
        pParams->linkInfo[i].nvlinkL1ThresholdUnits  = pLinkAndClockValues->nvlinkL1ThresholdUnits;

        if (nvlinkLinks[i].bConnected)
        {
            pParams->linkInfo[i].connected              = NV2080_CTRL_NVLINK_STATUS_CONNECTED_TRUE;
            pParams->linkInfo[i].remoteDeviceLinkNumber = (NvU8) nvlinkLinks[i].remoteLinkNumber;
            pParams->linkInfo[i].remoteLinkSid          = nvlinkLinks[i].remoteChipSid;

            // Set the device information for the remote end of the link
            pParams->linkInfo[i].remoteDeviceInfo.domain      = nvlinkLinks[i].remoteDomain;
            pParams->linkInfo[i].remoteDeviceInfo.bus         = nvlinkLinks[i].remoteBus;
            pParams->linkInfo[i].remoteDeviceInfo.device      = nvlinkLinks[i].remoteDevice;
            pParams->linkInfo[i].remoteDeviceInfo.function    = nvlinkLinks[i].remoteFunction;
            pParams->linkInfo[i].remoteDeviceInfo.pciDeviceId = nvlinkLinks[i].remotePciDeviceId;
            pParams->linkInfo[i].remoteDeviceInfo.deviceType  = nvlinkLinks[i].remoteDeviceType;

            // Update the device Id flags for PCI
            if (nvlinkLinks[i].remotePciDeviceId)
            {
                pParams->linkInfo[i].remoteDeviceInfo.deviceIdFlags |=
                    FLD_SET_DRF(2080_CTRL_NVLINK, _DEVICE_INFO, _DEVICE_ID_FLAGS, _PCI,
                                pParams->linkInfo[i].remoteDeviceInfo.deviceIdFlags);
            }

            // Check the PCI dbdf values to confirm the device on remote end
            if (NV2080_CTRL_NVLINK_DEVICE_INFO_DEVICE_ID_FLAGS_PCI & pParams->linkInfo[i].remoteDeviceInfo.deviceIdFlags)
            {
                if (!nvlinkLinks[i].bLoopbackSupported)
                {
                    pParams->linkInfo[i].loopProperty = NV2080_CTRL_NVLINK_STATUS_LOOP_PROPERTY_NONE;
                    continue;
                }
            }

            pParams->linkInfo[i].loopProperty = pParams->linkInfo[i].remoteDeviceLinkNumber == i ?
                                                NV2080_CTRL_NVLINK_STATUS_LOOP_PROPERTY_LOOPBACK :
                                                NV2080_CTRL_NVLINK_STATUS_LOOP_PROPERTY_LOOPOUT;
        }

        if (!(IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu) && RMCFG_FEATURE_PLATFORM_GSP))
        {
            // Per-link ForceConfig handling (non-legacy Arch ForceConfig only)
            if (bForcedConfig)
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
    }
    FOR_EACH_INDEX_IN_MASK_END;
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
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    NvBool bMIGNvLinkP2PSupported = ((pKernelMIGManager != NULL) &&
                                     kmigmgrIsMIGNvlinkP2PSupported(pGpu, pKernelMIGManager));
    NV_STATUS status = NV_OK;
    NvBool    bIsNvlinkReady = NV_TRUE;
    NvU8 i = 0;
    struct
    {
        NV2080_CTRL_INTERNAL_NVLINK_GET_LINK_AND_CLOCK_INFO_PARAMS nvlinkLinkAndClockInfoParams;
        NvlinkLinkStatus nvlinkLinks[NVLINK_MAX_LINKS_SW];
    } *pTmpData = NULL;

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

            if (IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu))
            {
                FOR_EACH_INDEX_IN_MASK(32, i, pParams->enabledLinkMask)
                {
                    NV2080_CTRL_NVLINK_DEVICE_INFO *pDeviceInfo = &pParams->linkInfo[i].remoteDeviceInfo;
                    if (pDeviceInfo->deviceType == NV2080_CTRL_NVLINK_DEVICE_INFO_DEVICE_TYPE_SWITCH)
                    	continue;
                    OBJGPU *pLoopGpu = gpumgrGetGpuFromUuid(pDeviceInfo->deviceUUID,
                                                            DRF_DEF(2080_GPU_CMD, _GPU_GET_GID_FLAGS, _TYPE, _SHA1) |
                                                            DRF_DEF(2080_GPU_CMD, _GPU_GET_GID_FLAGS, _FORMAT, _BINARY));

                    // Clear output if no gpu on the other end
                    if (pLoopGpu == NULL)
                    {
                        portMemSet(&pParams->linkInfo[i], 0, sizeof(NV2080_CTRL_NVLINK_LINK_STATUS_INFO));
                    }
                    else
                    {
                        pDeviceInfo->domain = (pLoopGpu->gpuId >> 16) & 0xffff;
                        pDeviceInfo->bus    = (pLoopGpu->gpuId >> 8) & 0xff;
                        pDeviceInfo->device = pLoopGpu->gpuId & 0xff;

                        // Clear UUID
                        portMemSet(pDeviceInfo->deviceUUID, 0, sizeof(pDeviceInfo->deviceUUID));
                    }
                }
                FOR_EACH_INDEX_IN_MASK_END;
            }

            return status;
        }
        else
        {
            return NV_ERR_NOT_SUPPORTED;
        }
    }

    // Initialize link mask to 0
    pParams->enabledLinkMask = 0;

    if (!bMIGNvLinkP2PSupported)
    {
        NV_PRINTF(LEVEL_INFO, "MIG NVLink P2P is not supported.\n");
        status = NV_OK;
        return status;
    }

    pTmpData = portMemAllocNonPaged(sizeof(*pTmpData));

    if (pTmpData == NULL)
    {
        return NV_ERR_NO_MEMORY;
    }
    portMemSet(pTmpData, 0, sizeof(*pTmpData));

    if (IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu) && RMCFG_FEATURE_PLATFORM_GSP)
    {
        status = NV_ERR_NOT_SUPPORTED;
        goto done;
    }
    else
    {
        KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

        if (pKernelNvlink == NULL)
        {
            NV_PRINTF(LEVEL_INFO, "Kernel NVLink is unavailable. Returning.\n");
            status = NV_OK;
            goto done;
        }

        // Get the remote ends of the links from the nvlink core
        if (!knvlinkIsForcedConfig(pGpu, pKernelNvlink) &&
            !(IS_RTLSIM(pGpu) && !pKernelNvlink->bForceEnableCoreLibRtlsims))
        {
            //
            // Get the nvlink connections for this device from the core
            // If the function fails then the corelib doesn't have enough
            // info to validate connectivity so we should mark the API call
            // as not ready
            //
            status = knvlinkCoreGetRemoteDeviceInfo(pGpu, pKernelNvlink);
            if (status == NV_ERR_NOT_READY)
            {
                NV_PRINTF(LEVEL_INFO, "Nvlink is not ready yet!\n");
                bIsNvlinkReady = NV_FALSE;
            }
            else if (status != NV_OK)
            {
                goto done;
            }

        }

        //
        // Some links might have passed receiver detect (bridge is present),
        // but might have failed to transition to safe mode (marginal links)
        // Update connectedLinks and bridgedLinks mask for these links
        //
        knvlinkFilterBridgeLinks_HAL(pGpu, pKernelNvlink);

        // If nvlink is not ready don't report back any links as being enabled
        pParams->enabledLinkMask = (bIsNvlinkReady) ? KNVLINK_GET_MASK(pKernelNvlink, enabledLinks, 32) : 0x0;

        pTmpData->nvlinkLinkAndClockInfoParams.linkMask = pParams->enabledLinkMask;
        pTmpData->nvlinkLinkAndClockInfoParams.bSublinkStateInst = NV_TRUE;

        status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                     NV2080_CTRL_CMD_INTERNAL_NVLINK_GET_LINK_AND_CLOCK_INFO,
                                     (void *)&pTmpData->nvlinkLinkAndClockInfoParams,
                                     sizeof(pTmpData->nvlinkLinkAndClockInfoParams));
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Failed to collect nvlink status info!\n");
            goto done;
        }

        FOR_EACH_INDEX_IN_MASK(32, i, pParams->enabledLinkMask)
        {
            pTmpData->nvlinkLinks[i].laneRxdetStatusMask = pKernelNvlink->nvlinkLinks[i].laneRxdetStatusMask;

#if defined(INCLUDE_NVLINK_LIB)
            pTmpData->nvlinkLinks[i].bConnected = pKernelNvlink->nvlinkLinks[i].remoteEndInfo.bConnected;
            pTmpData->nvlinkLinks[i].remoteLinkNumber = pKernelNvlink->nvlinkLinks[i].remoteEndInfo.linkNumber;
            pTmpData->nvlinkLinks[i].remoteDeviceType = pKernelNvlink->nvlinkLinks[i].remoteEndInfo.deviceType;
            pTmpData->nvlinkLinks[i].remoteChipSid = pKernelNvlink->nvlinkLinks[i].remoteEndInfo.chipSid;
            pTmpData->nvlinkLinks[i].remoteDomain = pKernelNvlink->nvlinkLinks[i].remoteEndInfo.domain;
            pTmpData->nvlinkLinks[i].remoteBus = pKernelNvlink->nvlinkLinks[i].remoteEndInfo.bus;
            pTmpData->nvlinkLinks[i].remoteDevice = pKernelNvlink->nvlinkLinks[i].remoteEndInfo.device;
            pTmpData->nvlinkLinks[i].remoteFunction = pKernelNvlink->nvlinkLinks[i].remoteEndInfo.function;
            pTmpData->nvlinkLinks[i].remotePciDeviceId = pKernelNvlink->nvlinkLinks[i].remoteEndInfo.pciDeviceId;
            pTmpData->nvlinkLinks[i].bLoopbackSupported = knvlinkIsP2pLoopbackSupportedPerLink(pGpu, pKernelNvlink, i);

            if (pKernelNvlink->nvlinkLinks[i].core_link)
            {
                pParams->linkInfo[i].localLinkSid = pKernelNvlink->nvlinkLinks[i].core_link->localSid;
            }
#endif
        }
        FOR_EACH_INDEX_IN_MASK_END;

        _getNvlinkStatus(pGpu,
                         &pTmpData->nvlinkLinkAndClockInfoParams,
                         pKernelNvlink->bridgeSensableLinks,
                         pKernelNvlink->bridgedLinks,
                         pKernelNvlink->ipVerNvlink,
                         pTmpData->nvlinkLinks,
                         pKernelNvlink->getProperty(pKernelNvlink, PDB_PROP_KNVLINK_ENABLED),
                         pKernelNvlink->getProperty(pKernelNvlink, PDB_PROP_KNVLINK_L2_POWER_STATE_ENABLED),
                         knvlinkIsForcedConfig(pGpu, pKernelNvlink),
                         pKernelNvlink->getProperty(pKernelNvlink, PDB_PROP_KNVLINK_UNCONTAINED_ERROR_RECOVERY_SUPPORTED),
                         knvlinkGetDegradedMode(pGpu, pKernelNvlink),
                         knvlinkIsUncontainedErrorRecoveryActive(pGpu, pKernelNvlink),
                         pParams);
    }
done:
    portMemFree(pTmpData);

    return status;
}
