/*
 * SPDX-FileCopyrightText: Copyright (c) 2023-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "os/os.h"
#include "core/hal.h"
#include "core/locks.h"
#include "gpu/gpu.h"
#include "gpu/subdevice/subdevice.h"
#include "gpu/gpu_fabric_probe.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "kernel/gpu/nvlink/kernel_nvlink.h"
#include "kernel/gpu/mem_sys/kern_mem_sys.h"
#include "kernel/gpu_mgr/gpu_mgr.h"
#include "vgpu/rpc.h"
#include "nvRmReg.h"

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

    FOR_EACH_INDEX_IN_MASK(32, i, pParams->linkMask & KNVLINK_GET_MASK(pKernelNvlink, enabledLinks, 32))
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
    if ((pParams->linkMask & KNVLINK_GET_MASK(pKernelNvlink, enabledLinks, 32)) != pParams->linkMask)
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
    NV2080_CTRL_INTERNAL_NVLINK_ENABLE_NVLINK_PEER_PARAMS enableNvlinkPeerParams;

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
                                 NV2080_CTRL_CMD_INTERNAL_NVLINK_ENABLE_NVLINK_PEER,
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

NV_STATUS
subdeviceCtrlCmdNvlinkGetSupportedCounters_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_NVLINK_GET_SUPPORTED_COUNTERS_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    NvBool bMIGNvLinkP2PSupported = ((pKernelMIGManager != NULL) &&
                                     kmigmgrIsMIGNvlinkP2PSupported(pGpu, pKernelMIGManager));
    if ((pKernelNvlink == NULL) || !bMIGNvLinkP2PSupported)
    {
        NV_PRINTF(LEVEL_INFO, "NVLink unavailable. Return\n");
        return NV_ERR_NOT_SUPPORTED;
    }
    return knvlinkGetSupportedCounters_HAL(pGpu, pKernelNvlink, pParams);
}

//
// subdeviceCtrlCmdNvlinkGetSupportedBWMode_IMPL
//    Query the supported RBM modes from probe repsonse
//
NV_STATUS
subdeviceCtrlCmdNvlinkGetSupportedBWMode_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_NVLINK_GET_SUPPORTED_BW_MODE_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    NvBool bMIGNvLinkP2PSupported = ((pKernelMIGManager != NULL) &&
                                     kmigmgrIsMIGNvlinkP2PSupported(pGpu, pKernelMIGManager));

    if ((pKernelNvlink == NULL) || !bMIGNvLinkP2PSupported)
    {
        NV_PRINTF(LEVEL_ERROR, "NVLink unavailable. Return\n");
        return NV_ERR_NOT_SUPPORTED;
    }

    // Direct-connect system
    if (pGpu->fabricProbeRetryDelay == 0)
    {
        //TODO: handle direct connect systems
        NV_PRINTF(LEVEL_ERROR, "RBM not currently implemented on direct connect systems.\n");
        return NV_ERR_NOT_SUPPORTED;
    }

    return knvlinkGetSupportedBwMode_HAL(pGpu, pKernelNvlink, pParams);
}

//
// subdeviceCtrlCmdNvlinkSetBWMode_IMPL
//    Set RBM mode on GPU
//
NV_STATUS
subdeviceCtrlCmdNvlinkSetBWMode_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_NVLINK_SET_BW_MODE_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    NvBool bMIGNvLinkP2PSupported = ((pKernelMIGManager != NULL) &&
                                     kmigmgrIsMIGNvlinkP2PSupported(pGpu, pKernelMIGManager));

    if ((pKernelNvlink == NULL) || !bMIGNvLinkP2PSupported)
    {
        NV_PRINTF(LEVEL_INFO, "NVLink unavailable. Return\n");
        return NV_ERR_NOT_SUPPORTED;
    }

    // Direct-connect system
    if (pGpu->fabricProbeRetryDelay == 0)
    {
        //TODO: handle direct connect systems
        NV_PRINTF(LEVEL_ERROR, "RBM not currently implemented on direct connect systems.\n");
        return NV_ERR_NOT_SUPPORTED;
    }

    // Check if requested BW mode is supported
    if (!knvlinkIsBwModeSupported_HAL(pGpu, pKernelNvlink, pParams->rbmMode))
    {
        NV_PRINTF(LEVEL_ERROR, "Requested RBM mode is not supported by GPU.\n");
        return NV_ERR_NOT_SUPPORTED;
    }

    // Set requested bw mode for GPU
    return gpumgrSetGpuNvlinkBwModePerGpu(pGpu, pParams->rbmMode);
}

//
// subdeviceCtrlCmdNvlinkGetBWMode_IMPL
//    Get RBM mode link count of GPU
//
NV_STATUS
subdeviceCtrlCmdNvlinkGetBWMode_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_NVLINK_GET_BW_MODE_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
    NvU8 bwModeScope;

    if (pKernelNvlink == NULL)
        return NV_ERR_NOT_SUPPORTED;

    bwModeScope = gpumgrGetGpuNvlinkBwModeScope();
    if (bwModeScope == GPU_NVLINK_BW_MODE_SCOPE_PER_NODE)
    {
        pParams->rbmMode = gpumgrGetGpuNvlinkBwMode();
    }
    else
    {
        pParams->rbmMode = pKernelNvlink->nvlinkBwMode;
    }

    return NV_OK;
}

NV_STATUS
subdeviceCtrlCmdNvlinkGetLocalDeviceInfo_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_NVLINK_GET_LOCAL_DEVICE_INFO_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

    if (pKernelNvlink == NULL)
        return NV_ERR_NOT_SUPPORTED;

    pParams->localDeviceInfo.domain      = gpuGetDomain(pGpu);
    pParams->localDeviceInfo.bus         = gpuGetBus(pGpu);
    pParams->localDeviceInfo.device      = gpuGetDevice(pGpu);
    pParams->localDeviceInfo.function    = 0;
    pParams->localDeviceInfo.pciDeviceId = pGpu->idInfo.PCIDeviceID;
    pParams->localDeviceInfo.deviceType  = NV2080_CTRL_NVLINK_DEVICE_INFO_DEVICE_TYPE_GPU;

    if (pGpu->idInfo.PCIDeviceID != 0)
    {
        pParams->localDeviceInfo.deviceIdFlags =
            FLD_SET_DRF(2080_CTRL_NVLINK, _DEVICE_INFO, _DEVICE_ID_FLAGS, _PCI,
                        pParams->localDeviceInfo.deviceIdFlags);
    }

    pParams->localDeviceInfo.fabricRecoveryStatusMask = 0x0;
    if (knvlinkGetDegradedMode(pGpu, pKernelNvlink))
    {
        pParams->localDeviceInfo.fabricRecoveryStatusMask =
            FLD_SET_DRF(2080_CTRL_NVLINK_DEVICE_INFO, _FABRIC_RECOVERY_STATUS_MASK, _GPU_DEGRADED, _TRUE,
                        pParams->localDeviceInfo.fabricRecoveryStatusMask);
    }
    else if (knvlinkIsUncontainedErrorRecoveryActive(pGpu, pKernelNvlink))
    {
        pParams->localDeviceInfo.fabricRecoveryStatusMask =
            FLD_SET_DRF(2080_CTRL_NVLINK_DEVICE_INFO, _FABRIC_RECOVERY_STATUS_MASK, _UNCONTAINED_ERROR_RECOVERY, _ACTIVE,
                        pParams->localDeviceInfo.fabricRecoveryStatusMask);
    }

    return NV_OK;
}

