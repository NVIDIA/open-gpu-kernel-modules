/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

// FIXME XXX
#define NVOC_KERNEL_IOCTRL_H_PRIVATE_ACCESS_ALLOWED

#include "os/os.h"
#include "core/hal.h"
#include "core/locks.h"
#include "rmapi/rs_utils.h"
#include "gpu_mgr/gpu_mgr.h"
#include "gpu/gpu.h"
#include "kernel/gpu/nvlink/kernel_nvlink.h"
#include "kernel/gpu/nvlink/kernel_ioctrl.h"
#include "kernel/gpu/nvlink/common_nvlink.h"
#include "kernel/gpu/device/device.h"
#include "kernel/gpu/bus/p2p_api.h"
#include "kernel/gpu/fifo/kernel_channel.h"
#include "kernel/mem_mgr/gpu_vaspace.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/mmu/kern_gmmu.h"
#include "gpu/ce/kernel_ce.h"
#include "platform/sli/sli.h"
#include "gpu/gpu_fabric_probe.h"
#include "swref/common_def_nvlink.h"
#include "gsp/gspifpub.h"
#include "compute/imex_session_api.h"
#include "compute/fabric.h"
#include "mem_mgr/mem_multicast_fabric.h"

#include "gpu/conf_compute/ccsl.h"

#include "spdm/rmspdmvendordef.h"
#include "gpu/spdm/spdm.h"
#include "kernel/gpu/spdm/libspdm_includes.h"
#include "hal/library/cryptlib.h"

static NV_STATUS _knvlinkRefreshEncryptionKeys(OBJGPU *, KernelNvlink *, NvU8 *, NvU32, sessionKeyRefreshStage, NvU8);
static NV_STATUS _knvlinkSendEncryptionKeys(OBJGPU *, KernelNvlink *, NvU8 *, NvU32);

/*!
 * @brief Is NVLINK topology forced? NVLink topology is considered
 *        forced for both legacy forced config and chiplib configs
 *
 * @param[in] pGpu           OBJGPU
 * @param[in] pKernelNvlink  KernelNvlink pointer
 *
 * @return  NV_TRUE if topology is forced
 */
NvBool
knvlinkIsForcedConfig_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    return (pKernelNvlink->bChiplibConfig);
}

/*!
 * @brief Determine if NVLink is enabled or disabled by default
 *
 * @param[in] pGpu           OBJGPU pointer
 * @param[in] pKernelNvlink  KernelNvlink pointer
 *
 * @return  NV_TRUE if NVLink is enabled on the GPU/platform
 */
NvBool
knvlinkIsNvlinkDefaultEnabled_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    //
    // Currently it is critical that the following lib check be present.
    // Burying this in the hal below it may get lost as the stub is all
    // thats required for POR (always true from the hals perspective)
    //
#if !defined(INCLUDE_NVLINK_LIB)

    return NV_FALSE;

#endif

    // Let the PDB handle the final decision.
    return pKernelNvlink->getProperty(pKernelNvlink, PDB_PROP_KNVLINK_ENABLED);
}

/*!
 * @brief Determine if P2P loopback over NVLink is supported for
 *        the given GPU. This function returns true if any link
 *        is connected in loopback mode.
 *
 * @param[in] pGpu           OBJGPU pointer
 * @param[in] pKernelNvlink  KernelNvlink pointer
 *
 * @return  NV_TRUE if any link is in loopback mode
 */
NvBool
knvlinkIsP2pLoopbackSupported_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
#if defined(INCLUDE_NVLINK_LIB)

    NvU32 i;

    if ((pGpu == NULL) || (pKernelNvlink == NULL))
    {
        return NV_FALSE;
    }

    // Return false if P2P loopback is disabled through regkey
    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_NVLINK_P2P_LOOPBACK_DISABLED))
    {
        return NV_FALSE;
    }

    FOR_EACH_IN_BITVECTOR(&pKernelNvlink->enabledLinks, i)
    {
        if (knvlinkIsP2pLoopbackSupportedPerLink_IMPL(pGpu, pKernelNvlink, i))
            return NV_TRUE;
    }
    FOR_EACH_IN_BITVECTOR_END();

#endif

    return NV_FALSE;
}

/*!
 * @brief Determine if P2P loopback over NVLink is supported for
 *        the given link. This function returns true if the link
 *        is connected in loopback mode.
 *
 * @param[in] pGpu           OBJGPU pointer
 * @param[in] pKernelNvlink  KernelNvlink pointer
 * @param[in] link           Link ID
 *
 * @return  NV_TRUE if the link is in loopback mode
 */
NvBool
knvlinkIsP2pLoopbackSupportedPerLink_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvU32         link
)
{
#if defined(INCLUDE_NVLINK_LIB)

   if ((pGpu == NULL) || (pKernelNvlink == NULL))
    {
        return NV_FALSE;
    }

    // Return false if P2P loopback is disabled through regkey
    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_NVLINK_P2P_LOOPBACK_DISABLED))
    {
        return NV_FALSE;
    }

    // Return false if the given link is disabled
    if (!bitVectorTest(&pKernelNvlink->enabledLinks, link))
    {
        return NV_FALSE;
    }

    // Check the link connected to the same GPU (loopback)
    if (pKernelNvlink->nvlinkLinks[link].remoteEndInfo.bConnected)
    {
        if (((pKernelNvlink->nvlinkLinks[link].remoteEndInfo.domain   == gpuGetDomain(pGpu)) &&
            (pKernelNvlink->nvlinkLinks[link].remoteEndInfo.bus      == gpuGetBus(pGpu))    &&
            (pKernelNvlink->nvlinkLinks[link].remoteEndInfo.device   == gpuGetDevice(pGpu)) &&
            (pKernelNvlink->nvlinkLinks[link].remoteEndInfo.function == 0)) ||
                pKernelNvlink->PDB_PROP_KNVLINK_FORCED_LOOPBACK_ON_SWITCH_MODE_ENABLED)
        {
            return NV_TRUE;
        }
    }

#endif

    return NV_FALSE;
}

/*!
 * @brief Determine if P2P over NVLINK is supported between 2 GPUs
 *
 * @param[in] pGpu           OBJGPU pointer for local GPU
 * @param[in] pKernelNvlink  KernelNvlink pointer
 * @param[in] pPeerGpu       OBJGPU pointer for remote GPU
 *
 * @return  NV_TRUE if P2P is supported between the 2 GPUs
 */
NvBool
knvlinkIsNvlinkP2pSupported_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    OBJGPU       *pPeerGpu
)
{
    NV_STATUS status = NV_OK;

    if (pKernelNvlink == NULL)
    {
        return NV_FALSE;
    }

    if (knvlinkIsBandwidthModeOff(pKernelNvlink))
    {
        return NV_FALSE;
    }

    // Get the Nvlink P2P connections from the core library
    status = knvlinkGetP2pConnectionStatus(pGpu, pKernelNvlink, pPeerGpu);

    if (status == NV_OK)
    {
        return NV_TRUE;
    }

    return NV_FALSE;
}

static NvBool
_knvlinkCheckFabricCliqueId
(
    OBJGPU       *pGpu,
    OBJGPU       *pPeerGpu
)
{
    NvU32 cliqueId, peerCliqueId;
    NV_STATUS status;

    status = gpuFabricProbeGetFabricCliqueId(pGpu->pGpuFabricProbeInfoKernel,
                                             &cliqueId);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_INFO, "GPU %d failed to get fabric clique Id: 0x%x\n",
                  gpuGetInstance(pGpu), status);
        return NV_FALSE;
    }

    status = gpuFabricProbeGetFabricCliqueId(pPeerGpu->pGpuFabricProbeInfoKernel,
                                             &peerCliqueId);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_INFO, "GPU %d failed to get fabric clique Id 0x%x\n",
                  gpuGetInstance(pPeerGpu), status);
        return NV_FALSE;
    }

    if (cliqueId != peerCliqueId)
    {
        NV_PRINTF(LEVEL_INFO, "GPU %d and Peer GPU %d cliqueId doesn't match\n",
                  gpuGetInstance(pGpu), gpuGetInstance(pPeerGpu));
        return NV_FALSE;
    }

    return NV_TRUE;
}

static NvBool
_knvlinkCheckFabricProbeHealth
(
    OBJGPU       *pGpu,
    OBJGPU       *pPeerGpu
)
{
    NvU32 healthStatusMask = 0;
    NvU32 peerHealthStatusMask = 0;
    NV_STATUS status;
    
    status = gpuFabricProbeGetFabricHealthStatus(pGpu->pGpuFabricProbeInfoKernel, &healthStatusMask);
    NV_ASSERT_OK_OR_RETURN(status);
 
    status = gpuFabricProbeGetFabricHealthStatus(pPeerGpu->pGpuFabricProbeInfoKernel, &peerHealthStatusMask);
    NV_ASSERT_OK_OR_RETURN(status);
 
    if (nvlinkGetFabricHealthSummary(healthStatusMask) == NVLINK_INBAND_FABRIC_HEALTH_SUMMARY_UNHEALTHY ||
        nvlinkGetFabricHealthSummary(peerHealthStatusMask) == NVLINK_INBAND_FABRIC_HEALTH_SUMMARY_UNHEALTHY)
    {
        return NV_FALSE;
    }
    
    return NV_TRUE;
}

/*!
 * @brief Checks whether EGM addresses are valid for P2P
 * when GPU is connected to NVSwitch
 *
 * @param[in] pGpu           OBJGPU pointer for local GPU
 * @param[in] pKernelNvlink  KernelNvlink pointer
 * @param[in] pPeerGpu       OBJGPU pointer for remote GPU
 *
 * @return  NV_TRUE if EGM addresses are valid
 */
static NvBool
_knvlinkCheckNvswitchEgmAddressSanity
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    OBJGPU       *pPeerGpu
)
{
    NvU64 egmRangeStart = knvlinkGetUniqueFabricEgmBaseAddress(pGpu, pKernelNvlink);

    if (knvlinkIsGpuConnectedToNvswitch(pGpu, pKernelNvlink))
    {
        if (gpuIsSriovEnabled(pGpu))
        {
            // currently vgpu + switch doesn't support GPA addressing.
            return NV_TRUE;
        }

        if (gpuFabricProbeIsSupported(pGpu) && gpuFabricProbeIsSupported(pPeerGpu))
        {
            if (!_knvlinkCheckFabricCliqueId(pGpu, pPeerGpu))
            {
                return NV_FALSE;
            }
        }

        // Sanity checks for EGM address
        if (egmRangeStart == NVLINK_INVALID_FABRIC_ADDR)
        {
            NV_PRINTF(LEVEL_ERROR, "GPU %d doesn't have a EGM fabric address\n",
                      gpuGetInstance(pGpu));

            return NV_FALSE;
        }
    }
    else
    {
        // Sanity check for EGM address
        if (egmRangeStart != NVLINK_INVALID_FABRIC_ADDR)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "non-NVSwitch GPU %d has a valid EGM fabric address\n",
                      gpuGetInstance(pGpu));

            return NV_FALSE;
        }

    }

    return NV_TRUE;
}

/*!
 * @brief Checks whether necessary the config setup is done to
 *        support P2P over NVSwitch
 *
 * @param[in] pGpu           OBJGPU pointer for local GPU
 * @param[in] pKernelNvlink  KernelNvlink pointer
 * @param[in] pPeerGpu       OBJGPU pointer for remote GPU
 *
 * @return  NV_TRUE if P2P over NVSwitch
 */
NvBool
knvlinkCheckNvswitchP2pConfig_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    OBJGPU       *pPeerGpu
)
{
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    NvU64          hbmRangeStart  = knvlinkGetUniqueFabricBaseAddress(pGpu, pKernelNvlink);
    NvU64          hbmRangeEnd    = hbmRangeStart + (pMemoryManager->Ram.fbTotalMemSizeMb << 20);
    NvU64          hbmPeerRangeStart = knvlinkGetUniqueFabricBaseAddress(pPeerGpu,
                                        GPU_GET_KERNEL_NVLINK(pPeerGpu));

    if (knvlinkIsGpuConnectedToNvswitch(pGpu, pKernelNvlink))
    {
        if (gpuIsSriovEnabled(pGpu))
        {
            // currently vgpu + switch doesn't support GPA addresing.
            return NV_TRUE;
        }

        if (gpuFabricProbeIsSupported(pGpu) && gpuFabricProbeIsSupported(pPeerGpu))
        {
            if (!_knvlinkCheckFabricCliqueId(pGpu, pPeerGpu) ||
                !_knvlinkCheckFabricProbeHealth(pGpu, pPeerGpu))
            {
                return NV_FALSE;
            }
        }

        // Sanity checks for HBM addresses
        if (hbmRangeStart == NVLINK_INVALID_FABRIC_ADDR)
        {
            NV_PRINTF(LEVEL_ERROR, "GPU %d doesn't have a fabric address\n",
                      gpuGetInstance(pGpu));

            return NV_FALSE;
        }

        if ((pGpu != pPeerGpu) &&
            ((hbmPeerRangeStart >= hbmRangeStart) && (hbmPeerRangeStart < hbmRangeEnd)))
        {
            NV_PRINTF(LEVEL_ERROR,
                      "GPU %d doesn't have a unique fabric address\n",
                      gpuGetInstance(pGpu));

            return NV_FALSE;
        }
    }
    else
    {
        // Sanity check for HBM address
        if (hbmRangeStart != NVLINK_INVALID_FABRIC_ADDR)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "non-NVSwitch GPU %d has a valid fabric address\n",
                      gpuGetInstance(pGpu));

            return NV_FALSE;
        }
    }

    if (memmgrIsLocalEgmEnabled(pMemoryManager))
    {
        return _knvlinkCheckNvswitchEgmAddressSanity(pGpu, pKernelNvlink, pPeerGpu);
    }

    return NV_TRUE;
}

/*!
 * @brief Get Nvlink P2P connections between 2 GPUs
 *
 * @param[in] pGpu           OBJGPU pointer for local GPU
 * @param[in] pKernelNvlink  KernelNvlink pointer
 * @param[in] pPeerGpu       OBJGPU pointer for remote GPU
 *
 * @return  NV_OK if P2P connections are present
 */
NV_STATUS
knvlinkGetP2pConnectionStatus_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    OBJGPU       *pPeerGpu
)
{
    NV_STATUS     status         = NV_OK;
    OBJGPU       *pGpu0          = pGpu;
    OBJGPU       *pGpu1          = pPeerGpu;
    KernelNvlink *pKernelNvlink0 = pKernelNvlink;
    KernelNvlink *pKernelNvlink1 = NULL;
    NvU32         numPeerLinks   = 0;
    NvU32         enabledLinks;

    if (pGpu1 == NULL)
    {
        NV_PRINTF(LEVEL_INFO, "Invalid pPeerGpu.\n");

        return NV_ERR_INVALID_ARGUMENT;
    }
    else if ((pGpu0 == pGpu1) &&
             (pGpu0->getProperty(pGpu0, PDB_PROP_GPU_NVLINK_P2P_LOOPBACK_DISABLED)))
    {
        // P2P over loopback links are disabled through regkey overrides
        NV_PRINTF(LEVEL_INFO, "loopback P2P on GPU%u disabled by regkey\n",
                  gpuGetInstance(pGpu0));

        return NV_ERR_NOT_SUPPORTED;
    }
    else
    {
        pKernelNvlink1 = GPU_GET_KERNEL_NVLINK(pGpu1);
    }

    if (pKernelNvlink1 == NULL)
    {
        NV_PRINTF(LEVEL_INFO,
                  "Input mask contains a GPU on which NVLink is disabled.\n");

        return NV_ERR_INVALID_ARGUMENT;
    }

    if(pKernelNvlink0->bIsGpuDegraded)
    {
        NV_PRINTF(LEVEL_INFO,
                  "NVLink P2P is NOT supported between GPU%d and GPU%d\n",
                  gpuGetInstance(pGpu0), gpuGetInstance(pGpu1));

        return NV_ERR_NOT_SUPPORTED;
    }

    if(pKernelNvlink1->bIsGpuDegraded)
    {
        NV_PRINTF(LEVEL_INFO,
                  "NVLink P2P is NOT supported between GPU%d and GPU%d\n",
                  gpuGetInstance(pGpu0), gpuGetInstance(pGpu1));

        return NV_ERR_NOT_SUPPORTED;
    }

    if ((IS_RTLSIM(pGpu0) && !pKernelNvlink0->bForceEnableCoreLibRtlsims) ||
        knvlinkIsForcedConfig(pGpu0, pKernelNvlink0))
    {
        // For non-legacy configs.
        if (pKernelNvlink0->bChiplibConfig)
        {
            NV_PRINTF(LEVEL_INFO,
                      "NVLink P2P is supported between GPU%d and GPU%d\n",
                      gpuGetInstance(pGpu0), gpuGetInstance(pGpu1));

            return NV_OK;
        }
    }

    // Get the remote ends of the links of local GPU from the nvlink core
    status = knvlinkCoreGetRemoteDeviceInfo(pGpu0, pKernelNvlink0);
    if (status != NV_OK)
    {
        return status;
    }

    // Post topology link enable on links of local GPU
    enabledLinks = KNVLINK_BITVECTOR_TO_MASK(pKernelNvlink0, enabledLinks, 32);
    status = knvlinkEnableLinksPostTopology_HAL(pGpu0, pKernelNvlink0, enabledLinks);
    if (status != NV_OK)
    {
        return status;
    }

    numPeerLinks = knvlinkGetNumLinksToPeer(pGpu0, pKernelNvlink0, pGpu1);

    //
    // Maybe knvlinkCoreGetRemoteDeviceInfo was never called on pGpu1.
    // This can happen on systems where FM doesn't configure GPUs
    // using RM control calls explicitly.
    //
    if ((numPeerLinks == 0) && gpuFabricProbeIsSupported(pGpu1))
    {
        knvlinkCoreGetRemoteDeviceInfo(pGpu1, pKernelNvlink1);

        // Post topology link enable on links of remote GPU
        enabledLinks = KNVLINK_BITVECTOR_TO_MASK(pKernelNvlink1, enabledLinks, 32);
        status = knvlinkEnableLinksPostTopology_HAL(pGpu1, pKernelNvlink1, enabledLinks);
        if (status != NV_OK)
        {
            return status;
        }

        numPeerLinks = knvlinkGetNumLinksToPeer(pGpu0, pKernelNvlink0, pGpu1);
    }

    if (numPeerLinks > 0)
    {
        if (knvlinkGetNumLinksToPeer(pGpu1, pKernelNvlink1, pGpu0) != numPeerLinks)
        {
            // Get the remote ends of the links of remote GPU from the nvlink core
            status = knvlinkCoreGetRemoteDeviceInfo(pGpu1, pKernelNvlink1);
            if (status != NV_OK)
            {
                return status;
            }

            // Post topology link enable on links of remote GPU
            enabledLinks = KNVLINK_BITVECTOR_TO_MASK(pKernelNvlink1, enabledLinks, 32);
            status = knvlinkEnableLinksPostTopology_HAL(pGpu1, pKernelNvlink1, enabledLinks);
            if (status != NV_OK)
            {
                return status;
            }
        }

        // Peers should have the same number of links pointing back at us
        NV_CHECK_OR_RETURN(LEVEL_INFO,
            (knvlinkGetNumLinksToPeer(pGpu1, pKernelNvlink1, pGpu0) == numPeerLinks),
            NV_ERR_INVALID_STATE);

        // P2P is not supported between GPUs with different RBMs.
        NV_CHECK_OR_RETURN(LEVEL_INFO,
            (pKernelNvlink0->nvlinkBwMode == pKernelNvlink1->nvlinkBwMode),
            NV_ERR_INVALID_STATE);

        NV_CHECK_OR_RETURN(LEVEL_INFO,
                knvlinkCheckNvswitchP2pConfig(pGpu0, pKernelNvlink0, pGpu1),
                NV_ERR_INVALID_STATE);

        NV_CHECK_OR_RETURN(LEVEL_INFO,
                knvlinkCheckNvswitchP2pConfig(pGpu1, pKernelNvlink1, pGpu0),
                NV_ERR_INVALID_STATE);

        NV_PRINTF(LEVEL_INFO,
                  "NVLink P2P is supported between GPU%d and GPU%d\n",
                  gpuGetInstance(pGpu0), gpuGetInstance(pGpu1));

        return NV_OK;
    }

    NV_PRINTF(LEVEL_INFO,
              "NVLink P2P is NOT supported between between GPU%d and GPU%d\n",
              pGpu->gpuInstance, pGpu1->gpuInstance);

    return NV_ERR_NOT_SUPPORTED;
}

/*!
 * @brief Update the settings for the current established NVLink
 *        topology. This is the top level function that should be
 *        called, instead of applying the settings individually,
 *        since it grabs the required locks
 *
 * @param[in] pGpu           OBJGPU pointer
 * @param[in] pKernelNvlink  KernelNvlink pointer
 *
 * @return  NV_OK on success
 */
NV_STATUS
knvlinkUpdateCurrentConfig_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    OBJSYS    *pSys      = SYS_GET_INSTANCE();
    KernelCE  *pKCe      = NULL;
    NvBool     bOwnsLock = NV_FALSE;
    NV_STATUS  status    = NV_OK;

    if (osAcquireRmSema(pSys->pSema) == NV_OK)
    {
        //
        // XXX Bug 1795328: Fix P2P path to acquire locks for the GPU
        //  Due to platform differences in the P2P path, the GPU lock is not
        //  consistently held at this point in the call stack. This function
        //  requires exclusive access to RM/PMU data structures to update HSHUB,
        //  and therefore requires the GPU lock to be held at this point.
        //  This check should be removed once the P2P paths have been updated to
        //  acquire the GPU locks consistently for all platforms.
        //
        if (!rmDeviceGpuLockIsOwner(pGpu->gpuInstance))
        {
            status = rmDeviceGpuLocksAcquire(pGpu, GPUS_LOCK_FLAGS_NONE,
                                             RM_LOCK_MODULES_NVLINK);
            if (status != NV_OK)
            {
                NV_ASSERT(0);
                goto fail;
            }

            bOwnsLock = NV_TRUE;
        }

        //
        // Links that have remote end detected should have passed RXDET
        // Update the mask of connected links and bridged links
        //
        knvlinkFilterBridgeLinks_HAL(pGpu, pKernelNvlink);

        NV2080_CTRL_INTERNAL_NVLINK_UPDATE_CURRENT_CONFIG_PARAMS params;
        portMemSet(&params, 0, sizeof(params));

        // Reset timeout to clear any accumulated timeouts from link init
        if (IS_GSP_CLIENT(pGpu))
        {
            threadStateResetTimeout(pGpu);
        }

        //
        // RPC into GSP-RM for programming the HSHUB, CONNECTION_CFG and LTCS
        // registers.
        //
        status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                     NV2080_CTRL_CMD_INTERNAL_NVLINK_UPDATE_CURRENT_CONFIG,
                                     (void *)&params, sizeof(params));
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Updating current NVLink config failed\n");
            goto fail;
        }

        // Sync the GPU property for NVLINK over SYSMEM with GSP-RM
        pGpu->setProperty(pGpu, PDB_PROP_GPU_NVLINK_SYSMEM, params.bNvlinkSysmemEnabled);

        // Update the PCE-LCE mappings
        status = kceFindFirstInstance(pGpu, &pKCe);
        if (status == NV_OK)
        {
        KernelCE *pKCeIter = NULL;
        KCE_ITER_SHIM_BEGIN(pGpu, pKCeIter)
            status = kceTopLevelPceLceMappingsUpdate(pGpu, pKCeIter);
        KCE_ITER_END;
            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR, "Failed to update PCE-LCE mappings\n");
            }
        }

fail:
        if (bOwnsLock)
        {
            rmDeviceGpuLocksRelease(pGpu, GPUS_LOCK_FLAGS_NONE, NULL);
        }

        osReleaseRmSema(pSys->pSema, NULL);
    }

    return status;
}

const static NVLINK_INBAND_MSG_CALLBACK nvlink_inband_callbacks[] =
{
    {
        .messageType = NVLINK_INBAND_MSG_TYPE_GPU_PROBE_RSP,
        .pCallback = gpuFabricProbeReceiveKernelCallback,
        .wqItemFlags = {.bLockSema = NV_TRUE,
                        .bLockGpuGroupSubdevice = NV_TRUE}
    },

    {
        .messageType = NVLINK_INBAND_MSG_TYPE_MC_TEAM_SETUP_RSP,
        .pCallback = memorymulticastfabricTeamSetupResponseCallback,
        .wqItemFlags = {.bLockSema = NV_TRUE,
                        .bLockGpus = NV_TRUE}
    },

    {
        .messageType = NVLINK_INBAND_MSG_TYPE_GPU_PROBE_UPDATE_REQ,
        .pCallback = gpuFabricProbeReceiveUpdateKernelCallback,
        .wqItemFlags = {.bLockSema = NV_TRUE,
                        .bLockGpuGroupSubdevice = NV_TRUE}
    }
};

void
knvlinkInbandMsgCallbackDispatcher_WORKITEM
(
    NvU32 gpuInstance,
    void *pData
)
{
    nvlink_inband_msg_header_t *pHeader;
    NV2080_CTRL_NVLINK_INBAND_RECEIVED_DATA_PARAMS *pMessage = pData;
    NvU8 i;
    const NVLINK_INBAND_MSG_CALLBACK *pCb = NULL;

    // Dispatcher may not be called under GPU lock, so don't access pGpu.

    pHeader = (nvlink_inband_msg_header_t *)pMessage->data;

    for (i = 0; i < NV_ARRAY_ELEMENTS(nvlink_inband_callbacks); i++)
    {
        if ((nvlink_inband_callbacks[i].messageType == pHeader->type) &&
            (nvlink_inband_callbacks[i].pCallback != NULL))
        {
            pCb = &nvlink_inband_callbacks[i];
            break;
        }
    }

    if (pCb == NULL)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "No Callback Registered for type %d. Dropping the msg\n",
                  pHeader->type);
        return;
    }

#if defined(DEBUG) || defined(DEVELOP)
    {
        NvU8 *pRsvd = NULL;

        // Assert reserved in msgHdr are zero
        pRsvd = &pHeader->reserved[0];
        NV_ASSERT((pRsvd[0] == 0) && portMemCmp(pRsvd, pRsvd + 1,
                  sizeof(pHeader->reserved) - 1) == 0);
    }
#endif

    (void)pCb->pCallback(gpuInstance, NULL, pData);
}

NV_STATUS
knvlinkInbandMsgCallbackDispatcher_IMPL
(
    OBJGPU *pGpu,
    KernelNvlink *pKernelNvlink,
    NvU32 dataSize,
    NvU8  *pMessage
)
{
    NV_STATUS status;
    nvlink_inband_msg_header_t *pHeader;
    NV2080_CTRL_NVLINK_INBAND_RECEIVED_DATA_PARAMS *pData = NULL;
    const NVLINK_INBAND_MSG_CALLBACK *pCb = NULL;
    NvU8 i;

    pHeader = (nvlink_inband_msg_header_t *)pMessage;

    if (pHeader->type >= NVLINK_INBAND_MSG_TYPE_MAX)
    {
        NV_PRINTF(LEVEL_ERROR, "Message type received is Out of Bounds. Dropping  the msg\n");
        return NV_ERR_INVALID_REQUEST;
    }

    for (i = 0; i < NV_ARRAY_ELEMENTS(nvlink_inband_callbacks); i++)
    {
        if ((nvlink_inband_callbacks[i].messageType == pHeader->type) &&
            (nvlink_inband_callbacks[i].pCallback != NULL))
        {
            pCb = &nvlink_inband_callbacks[i];
            break;
        }
    }

    if (pCb == NULL)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "No Callback Registered for type %d. Dropping the msg\n",
                  pHeader->type);
        return NV_ERR_INVALID_REQUEST;
    }

    pData = portMemAllocNonPaged(sizeof(NV2080_CTRL_NVLINK_INBAND_RECEIVED_DATA_PARAMS));
    if (pData == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Out of memory, Dropping message\n");
        return NV_ERR_NO_MEMORY;
    }

    pData->dataSize = dataSize;
    portMemCopy(pData->data, pData->dataSize, pMessage, dataSize);

    status = osQueueWorkItem(pGpu,
                             knvlinkInbandMsgCallbackDispatcher_WORKITEM,
                             pData,
                             pCb->wqItemFlags);
    if (status != NV_OK)
    {
        portMemFree(pData);
        return status;
     }

     return NV_OK;
}

NV_STATUS
knvlinkSendInbandData_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NV2080_CTRL_NVLINK_INBAND_SEND_DATA_PARAMS *pParams
)
{
    NV_STATUS status;

    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_NVLINK_INBAND_SEND_DATA,
                                 (void *)pParams,
                                 sizeof(*pParams));

    return status;
}
/*!
 * @brief Return the mask of links enabled on the system
 *
 * @param[in] pGpu           OBJGPU pointer
 * @param[in] pKernelNvlink  KernelNvlink pointer
 */
NVLINK_BIT_VECTOR *
knvlinkGetEnabledLinkMask_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    return &pKernelNvlink->enabledLinks;
}

/*!
 * @brief Return the mask of links discovered on the system
 *
 * @param[in]  pGpu           OBJGPU pointer
 * @param[in]  pKernelNvlink  KernelNvlink pointer
 * @return     Returns back a NVLINK_BIT_VECTOR *
 *              caller must check value is non-NULL
 */
NVLINK_BIT_VECTOR *
knvlinkGetDiscoveredLinkMask_IMPL
(
    OBJGPU            *pGpu,
    KernelNvlink      *pKernelNvlink
)
{
    return &(pKernelNvlink->discoveredLinks);
}

/*!
 * @brief Returns the number of sysmem links
 *
 * @param[in] pGpu           OBJGPU pointer
 * @param[in] pKernelNvlink  KernelNvlink pointer
 *
 * @return  The #sysmem NVLinks
 */
NvU32
knvlinkGetNumLinksToSystem_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    NvU32 numSysmemLinks = pKernelNvlink->sysmemLinkMask;

    if (numSysmemLinks != 0)
    {
        NUMSETBITS_32(numSysmemLinks);
    }

    return numSysmemLinks;
}

/*!
 * @brief Returns number of peer links to a remote GPU
 *
 * @param[in] pGpu             OBJGPU pointer of local GPU
 * @param[in] pKernelNvlink    KernelNvlink pointer
 * @param[in] pRemoteGpu       OBJGPU pointer of remote GPU
 *
 * @return  The #peer NVLinks to the remote GPU
 */
NvU32
knvlinkGetNumLinksToPeer_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    OBJGPU       *pRemoteGpu
)
{
    NvU64 numPeerLinks =
        knvlinkGetLinkMaskToPeer(pGpu, pKernelNvlink, pRemoteGpu);

    if (numPeerLinks != 0)
    {
        numPeerLinks = nvPopCount64(numPeerLinks);
    }

    return numPeerLinks;
}

/*!
 * @brief Gets the mask of peer links between the GPUs
 *
 * @param[in] pGpu0           OBJGPU pointer
 * @param[in] pKernelNvlink0  Nvlink pointer
 * @param[in] pGpu1           Remote OBJGPU pointer
 *
 * @return    Returns the mask of peer links between the GPUs
 */
NvU64
knvlinkGetLinkMaskToPeer_IMPL
(
    OBJGPU       *pGpu0,
    KernelNvlink *pKernelNvlink0,
    OBJGPU       *pGpu1
)
{
    NvU64 peerLinkMask = 0;
    KernelNvlink *pKernelNvlink1 = NULL;

    pKernelNvlink1 = GPU_GET_KERNEL_NVLINK(pGpu1);

    if (pKernelNvlink1 == NULL)
    {
        NV_PRINTF(LEVEL_INFO,
                  "on GPU%d NVLink is disabled.\n", gpuGetInstance(pGpu1));

        return 0;
    }

    if(pKernelNvlink0->bIsGpuDegraded)
    {
        return peerLinkMask;
    }

    if(pKernelNvlink1->bIsGpuDegraded)
    {
        return peerLinkMask;
    }

    if (!knvlinkIsForcedConfig(pGpu0, pKernelNvlink0))
    {
        //
        // If nvlink topology is not forced, then the hshub registers
        // are updated only when a P2P object is allocated. So, return
        // the cached value of mask of links connected to a GPU
        //
        peerLinkMask = KNVLINK_GET_MASK(pKernelNvlink0, peerLinkMasks[gpuGetInstance(pGpu1)], 64);
    }

    return peerLinkMask;
}

/*!
 * @brief Sets the mask of peer links between the GPUs
 *
 * @param[in] pGpu0           OBJGPU pointer
 * @param[in] pKernelNvlink0  Nvlink pointer
 * @param[in] pGpu1           Remote OBJGPU pointer
 * @param[in] peerLinkMask    Mask of links to the peer GPU
 *
 * @return    NV_OK on success
 */
NV_STATUS
knvlinkSetLinkMaskToPeer_IMPL
(
    OBJGPU       *pGpu0,
    KernelNvlink *pKernelNvlink0,
    OBJGPU       *pGpu1,
    NvU64         peerLinkMask
)
{
    NV_STATUS status = NV_OK;

    // Return early if no update needed to the peer link mask
    if (KNVLINK_GET_MASK(pKernelNvlink0, peerLinkMasks[gpuGetInstance(pGpu1)], 64) == peerLinkMask)
        return NV_OK;

    pKernelNvlink0->peerLinkMasks[gpuGetInstance(pGpu1)] = peerLinkMask;

    NV2080_CTRL_INTERNAL_NVLINK_UPDATE_PEER_LINK_MASK_PARAMS params;

    portMemSet(&params, 0, sizeof(params));
    params.gpuInst      = gpuGetInstance(pGpu1);
    params.peerLinkMask = peerLinkMask;

    // Reset timeout to clear any accumulated timeouts from link init
    if (IS_GSP_CLIENT(pGpu0))
    {
        threadStateResetTimeout(pGpu0);
    }

    // Sync the peerLinkMask with GSP-RM
    status = knvlinkExecGspRmRpc(pGpu0, pKernelNvlink0,
                                 NV2080_CTRL_CMD_INTERNAL_NVLINK_UPDATE_PEER_LINK_MASK,
                                 (void *)&params, sizeof(params));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to sync peerLinksMask from GPU%d to GPU%d\n",
                  gpuGetInstance(pGpu0), gpuGetInstance(pGpu1));
        return status;
    }

    return NV_OK;
}

/*!
 * @brief Prepare a GPU's NVLink engine for reset by removing mappings
 *        to it from other GPUs.
 *
 * @param[in] pGpu          OBJGPU pointer
 * @param[in] pKernelNvlink KernelNvlink pointer
 *
 * return  NV_OK on success
 */
NV_STATUS
knvlinkPrepareForXVEReset_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvBool        bForceShutdown
)
{
    OBJSYS    *pSys      = SYS_GET_INSTANCE();
    NV_STATUS  retStatus = NV_OK;
    OBJGPU    *pRemoteGpu;
    NV_STATUS  status;
    NvU32      gpuInstance;
    NvU32      gpuMask;

    // This is not supported on forced configs
    if (knvlinkIsForcedConfig(pGpu, pKernelNvlink))
    {
        return NV_OK;
    }

    //
    // Let fabric manager handle link shutdown/reset if the fabric is managed
    // externally.
    //
    if (pKernelNvlink->ipVerNvlink < NVLINK_VERSION_40 &&
        pSys->getProperty(pSys, PDB_PROP_SYS_FABRIC_IS_EXTERNALLY_MANAGED))
    {
        NV_PRINTF(LEVEL_INFO,
                  "NVLink fabric is externally managed, skipping\n");
        return NV_OK;
    }

    status = gpumgrGetGpuAttachInfo(NULL, &gpuMask);
    NV_ASSERT_OR_RETURN(status == NV_OK, status);

    gpuInstance = 0;
    while ((pRemoteGpu = gpumgrGetNextGpu(gpuMask, &gpuInstance)) != NULL)
    {
        KernelNvlink *pRemoteKernelNvlink = GPU_GET_KERNEL_NVLINK(pRemoteGpu);

        if ((pRemoteGpu == pGpu) || (pRemoteKernelNvlink == NULL) ||
            (knvlinkGetNumLinksToPeer(pRemoteGpu, pRemoteKernelNvlink, pGpu) == 0) ||
            API_GPU_IN_RESET_SANITY_CHECK(pRemoteGpu) ||
            pRemoteGpu->getProperty(pRemoteGpu, PDB_PROP_GPU_IS_LOST))
        {
            continue;
        }

        //
        // Reset the peer masks in HSHUB of the remote GPU. Partial resets
        // (only removing the links connected to the GPU being reset) don't
        // appear to be sufficient. The reset will work fine, but the next
        // time we attempt to initialize this GPU, the copy engines will time
        // out while scrubbing FB and a GPU sysmembar (NV_UFLUSH_FB_FLUSH) will
        // fail to complete.
        //
        // The above symptoms haven't been root-caused (yet), but the current
        // POR for GPU reset is that once one GPU is reset, the others
        // connected to it over NVLink must also be reset before using NVLink
        // for peer traffic, so just use the big hammer and squash all HSHUB
        // configs on GPU reset.
        //
        // This allows us to reset the GPUs one by one, with GPU
        // initializations in between, without hanging up the GPU trying to
        // flush data over links that aren't available anymore.
        //
        // Starting from Ampere single GPU reset is supported and hence remove
        // only the nvlink's of the remote GPU's which are connected to the
        // current GPU.
        //

        if (IsAMPEREorBetter(pGpu))
        {
            NvU32 remPeerId = kbusGetPeerId_HAL(pRemoteGpu, GPU_GET_KERNEL_BUS(pRemoteGpu), pGpu);
            if (remPeerId != BUS_INVALID_PEER)
                status = knvlinkRemoveMapping_HAL(pRemoteGpu, pRemoteKernelNvlink, NV_FALSE,
                                                  NVBIT(remPeerId),
                                                  NV_FALSE /* bL2Entry */);
        }
        else
        {
            status = knvlinkRemoveMapping_HAL(pRemoteGpu, pRemoteKernelNvlink, NV_FALSE,
                                              ((1 << NVLINK_MAX_PEERS_SW) - 1),
                                              NV_FALSE /* bL2Entry */);
        }
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "failed to reset HSHUB on GPU%u while preparing for GPU%u XVE reset (0x%x)\n",
                      gpuGetInstance(pRemoteGpu), gpuGetInstance(pGpu),
                      status);

            retStatus = (retStatus == NV_OK) ? status : retStatus;
        }
    }

    // Remove all NVLink mappings in HSHUB config registers to init values
    if (!API_GPU_IN_RESET_SANITY_CHECK(pGpu) && !pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_LOST))
    status = knvlinkRemoveMapping_HAL(pGpu, pKernelNvlink, NV_TRUE, ((1 << NVLINK_MAX_PEERS_SW) - 1),
                                      NV_FALSE /* bL2Entry */);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "failed to reset HSHUB on GPU%u while preparing XVE reset: %s (0x%x)\n",
                  gpuGetInstance(pGpu), nvstatusToString(status), status);

        retStatus = (retStatus == NV_OK) ? status : retStatus;
    }

    //
    // If GFW is booted and running through link-training, then no need to tear-down the
    // links to reset. Exit out early from the function
    //
    if (!bForceShutdown && pKernelNvlink->getProperty(pKernelNvlink, PDB_PROP_KNVLINK_MINION_GFW_BOOT))
    {
        return NV_OK;
    }

    // Pseudo-clean  shutdown the links from this GPU
    status = knvlinkCoreShutdownDeviceLinks(pGpu, pKernelNvlink, bForceShutdown);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "failed to shutdown links on GPU%u while preparing XVE reset: %s (0x%x)\n",
                  gpuGetInstance(pGpu), nvstatusToString(status), status);

        retStatus = (retStatus == NV_OK) ? status : retStatus;
    }

    //
    // Reset links related to this device and its peers (see Bug 2346447)
    // The property is disabled on Pascal, since the path hasn't been verified
    // and link reset after pseudo-clean shutdown results in DL and TL errors.
    //
    if (pKernelNvlink->getProperty(pKernelNvlink, PDB_PROP_KNVLINK_LINKRESET_AFTER_SHUTDOWN))
    {
        status = knvlinkCoreResetDeviceLinks(pGpu, pKernelNvlink);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "failed to reset links on GPU%u while preparing XVE reset: %s (0x%x)\n",
                      gpuGetInstance(pGpu), nvstatusToString(status), status);

            retStatus = (retStatus == NV_OK) ? status : retStatus;
        }
#if defined(INCLUDE_NVLINK_LIB)
        else
        {
            NvU32 linkId;

            //
            // The connections have been successfully reset, update connected and disconnected
            // links masks on both the devices
            //
            FOR_EACH_IN_BITVECTOR(&pKernelNvlink->enabledLinks, linkId)
            {
                pKernelNvlink->disconnectedLinkMask |=  NVBIT64(linkId);
                bitVectorClr(&pKernelNvlink->connectedLinksMask, linkId);

                if (pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.deviceType !=
                                              NV2080_CTRL_NVLINK_DEVICE_INFO_DEVICE_TYPE_GPU)
                {
                    continue;
                }

                OBJGPU *pRemoteGpu = gpumgrGetGpuFromBusInfo(
                                            pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.domain,
                                            pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.bus,
                                            pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.device);

                if (!API_GPU_IN_RESET_SANITY_CHECK(pRemoteGpu))
                {
                    KernelNvlink *pRemoteKernelNvlink = GPU_GET_KERNEL_NVLINK(pRemoteGpu);
                    NvU32 remoteLinkId = pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.linkNumber;

                    pRemoteKernelNvlink->disconnectedLinkMask |=  NVBIT64(remoteLinkId);
                    bitVectorClr(&pRemoteKernelNvlink->connectedLinksMask, remoteLinkId);
                }
            }
            FOR_EACH_IN_BITVECTOR_END();
        }
#endif

        //
        // knvlinkCoreResetDeviceLinks() only resets the links which have
        // connectivity.
        // Pre-Ampere, we may run into a situation where the PLL
        // sharing partner links (both) may not be reset due to no connectivity.
        //
        // Hence, (re-)reset all the links to recover them after shutdown (pre-Ampere)
        //
        NV2080_CTRL_INTERNAL_NVLINK_RESET_LINKS_PARAMS resetLinksparams;

        portMemSet(&resetLinksparams, 0, sizeof(resetLinksparams));
        resetLinksparams.linkMask = KNVLINK_BITVECTOR_TO_MASK(pKernelNvlink, enabledLinks, 32);
        resetLinksparams.flags    = NV2080_CTRL_INTERNAL_NVLINK_RESET_FLAGS_TOGGLE;

        status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                     NV2080_CTRL_CMD_INTERNAL_NVLINK_RESET_LINKS,
                                     (void *)&resetLinksparams, sizeof(resetLinksparams));

        retStatus = (retStatus == NV_OK) ? status : retStatus;
    }

    return retStatus;
}

/*!
 * @brief Set the power features supported on this NVLink IP
 *
 * @param[in] pGpu          OBJGPU pointer
 * @param[in] pKernelNvlink KernelNvlink pointer
 */
void
knvlinkSetPowerFeatures_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    // Get the Ip Verion from the First available IOCTRL.
    switch (pKernelNvlink->ipVerNvlink)
    {
        case NVLINK_VERSION_22:
        {
            // NVLink L2 is supported only on MODS and Windows LDDM
            if (RMCFG_FEATURE_PLATFORM_WINDOWS || RMCFG_FEATURE_MODS_FEATURES)
            {
                pKernelNvlink->setProperty(pKernelNvlink, PDB_PROP_KNVLINK_L2_POWER_STATE_ENABLED,
                                           (pKernelNvlink->bDisableL2Mode ? NV_FALSE : NV_TRUE));
            }

            break;
        }
        case NVLINK_VERSION_50:
        {
            pKernelNvlink->setProperty(pKernelNvlink, PDB_PROP_KNVLINK_L2_POWER_STATE_ENABLED,
                                        (pKernelNvlink->bDisableL2Mode ? NV_FALSE : NV_TRUE));
            break;
        }
        default:
            break;
    }
}

/*!
 * @brief Checks if NVSWITCH_FABRIC_ADDR field is valid.
 *
 * @param[in] pGpu          OBJGPU pointer
 * @param[in] pKernelNvlink KernelNvlink pointer
 */
void
knvlinkDetectNvswitchProxy_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    OBJSYS    *pSys   = SYS_GET_INSTANCE();
    NV_STATUS  status = NV_OK;
    NvU32      i;

    // Initialize fabricBaseAddr to NVLINK_INVALID_FABRIC_ADDR
    pKernelNvlink->fabricBaseAddr = NVLINK_INVALID_FABRIC_ADDR;

    if (pSys->getProperty(pSys, PDB_PROP_SYS_NVSWITCH_IS_PRESENT) ||
        pSys->getProperty(pSys, PDB_PROP_SYS_FABRIC_MANAGER_IS_REGISTERED) ||
        GPU_IS_NVSWITCH_DETECTED(pGpu))
    {
        return;
    }

    if (bitVectorTestAllCleared(&pKernelNvlink->discoveredLinks))
    {
        return;
    }

    // Get the link train status for the enabled link masks
    NV2080_CTRL_INTERNAL_NVLINK_ARE_LINKS_TRAINED_PARAMS linkTrainedParams;

    portMemSet(&linkTrainedParams, 0, sizeof(linkTrainedParams));
    status = convertBitVectorToLinkMasks(&pKernelNvlink->enabledLinks, NULL,
                                        0, &linkTrainedParams.linkMask);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to convert enabled links to RMCTRL mask\n");
        return;
    }
    linkTrainedParams.bActiveOnly = NV_FALSE;

    // Reset timeout to clear any accumulated timeouts from link init
    if (IS_GSP_CLIENT(pGpu))
    {
        threadStateResetTimeout(pGpu);
    }

    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_INTERNAL_NVLINK_ARE_LINKS_TRAINED,
                                 (void *)&linkTrainedParams, sizeof(linkTrainedParams));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to get the link train status for links\n");
        return;
    }

    FOR_EACH_IN_BITVECTOR(&pKernelNvlink->enabledLinks, i)
    {
        if (!linkTrainedParams.bIsLinkActive[i])
        {
            return;
        }
    }
    FOR_EACH_IN_BITVECTOR_END();

    NV2080_CTRL_INTERNAL_NVLINK_GET_SET_NVSWITCH_FABRIC_ADDR_PARAMS params;

    portMemSet(&params, 0, sizeof(params));
    params.bGet = NV_TRUE;
    params.addr = NVLINK_INVALID_FABRIC_ADDR;

    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_INTERNAL_NVLINK_GET_SET_NVSWITCH_FABRIC_ADDR,
                                 (void *)&params, sizeof(params));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to get fabric address for GPU %x\n",
                  pGpu->gpuInstance);
        return;
    }

    if (params.addr != NVLINK_INVALID_FABRIC_ADDR)
    {
        pKernelNvlink->fabricBaseAddr = params.addr;
        pKernelNvlink->bNvswitchProxy = NV_TRUE;
    }
}

/*!
 * @brief Sets NVSWITCH_FLA_ADDR field in the scratch register.
 *
 * @param[in] pGpu           OBJGPU pointer
 * @param[in] pKernelNvlink  KernelNvlink pointer
 * @param[in] addr           FLA addr
 *
 * @return  Returns NV_OK upon success.
 *          Otherwise, returns NV_ERR_XXX.
 */
NV_STATUS
knvlinkSetNvswitchFlaAddr_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvU64         addr
)
{
    return NV_OK;
}

/*!
 * @brief Gets NVSWITCH_FLA_ADDR field from the scratch register.
 *
 * @param[in] pGpu           OBJGPU pointer
 * @param[in] pKernelNvlink  KernelNvlink pointer
 *
 * @return  Returns the stashed FLA starting address.
 */
NvU64
knvlinkGetNvswitchFlaAddr_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    return 0;
}

/*!
 * @brief Checks if fabricBaseAddr is valid.
 *
 * @param[in] pGpu          OBJGPU pointer
 * @param[in] pKernelNvlink KernelNvlink pointer
 *
 * @return  Returns true if the fabricBaseAddr is valid.
 */
NvBool
knvlinkIsNvswitchProxyPresent_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    return pKernelNvlink->bNvswitchProxy;
}


/*!
 * @brief   Set unique FLA base address for NVSwitch enabled systems.
 *          Validates FLA base address and programs the base address
 *          in switch scratch registers for guest VM to pick it up.
 *
 * @param[in]   pGpu               OBJGPU pointer
 * @param[in]   pKernelNvlink      KernelNvlink pointer
 * @param[in]   flaBaseAddr        NvU64  base address
 *
 * @returns On success, sets unique FLA base address and returns NV_OK.
 *          On failure, returns NV_ERR_XXX.
 */
NV_STATUS
knvlinkSetUniqueFlaBaseAddress_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvU64         flaBaseAddr
)
{
    NV_STATUS  status     = NV_OK;
    KernelBus *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
    NvU32 enabledLinks;

    NV2080_CTRL_NVLINK_GET_SET_NVSWITCH_FLA_ADDR_PARAMS params;

    if (!knvlinkIsForcedConfig(pGpu, pKernelNvlink))
    {
        knvlinkCoreGetRemoteDeviceInfo(pGpu, pKernelNvlink);
        enabledLinks = KNVLINK_BITVECTOR_TO_MASK(pKernelNvlink, enabledLinks, 32);
        status = knvlinkEnableLinksPostTopology_HAL(pGpu, pKernelNvlink, enabledLinks);
        if (status != NV_OK)
        {
            return status;
        }
    }

    status = kbusValidateFlaBaseAddress_HAL(pGpu, pKernelBus, flaBaseAddr);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "FLA base addr validation failed for GPU %x\n",
                  pGpu->gpuInstance);
        return status;
    }

    if (IsSLIEnabled(pGpu))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Operation is unsupported on SLI enabled GPU %x\n",
                  pGpu->gpuInstance);
        return NV_ERR_NOT_SUPPORTED;
    }

    portMemSet(&params, 0, sizeof(params));
    params.bGet = NV_FALSE;
    params.addr = flaBaseAddr;

    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_NVLINK_GET_SET_NVSWITCH_FLA_ADDR,
                                 (void *)&params, sizeof(params));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to stash fla base address for GPU %x\n",
                  pGpu->gpuInstance);
        return status;
    }

    NV_PRINTF(LEVEL_INFO, "FLA base addr %llx is assigned to GPU %x\n",
              flaBaseAddr, pGpu->gpuInstance);

    return NV_OK;
}

/*!
 * @brief Synchronize the link masks and vbios defined properties
 *        between CPU and GSP-RMs
 *
 * @param[in]   pGpu           OBJGPU pointer
 * @param[in]   pKernelNvlink  KernelNvlink pointer
 */
NV_STATUS
knvlinkSyncLinkMasksAndVbiosInfo_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    NV_STATUS status = NV_OK;
    NVLINK_BIT_VECTOR localLinkMaskBitVector;

    NV2080_CTRL_NVLINK_SYNC_LINK_MASKS_AND_VBIOS_INFO_PARAMS params;

    portMemSet(&params, 0, sizeof(params));

    params.discoveredLinks     = KNVLINK_BITVECTOR_TO_MASK(pKernelNvlink, discoveredLinks, 64);
    params.connectedLinksMask  = KNVLINK_BITVECTOR_TO_MASK(pKernelNvlink, connectedLinksMask, 64);
    params.bridgeSensableLinks = KNVLINK_BITVECTOR_TO_MASK(pKernelNvlink, bridgeSensableLinks, 64);
    params.bridgedLinks        = pKernelNvlink->bridgedLinks;

    // Convert from NVLINK_BIT_VECTOR to RMCTRL mask
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        convertBitVectorToLinkMasks(&pKernelNvlink->discoveredLinks, NULL,
                                    0, &params.discoveredLinkMasks));
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        convertBitVectorToLinkMasks(&pKernelNvlink->connectedLinksMask, NULL,
                                    0, &params.connectedLinks));
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        convertBitVectorToLinkMasks(&pKernelNvlink->bridgeSensableLinks, NULL,
                                    0, &params.bridgeSensableLinkMasks));
    params.bridgedLinkMasks.masks[0] = pKernelNvlink->bridgedLinks;
    params.bridgedLinkMasks.lenMasks = 1;

    // Reset timeout to clear any accumulated timeouts from link init
    if (IS_GSP_CLIENT(pGpu))
    {
        threadStateResetTimeout(pGpu);
    }

    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_NVLINK_SYNC_LINK_MASKS_AND_VBIOS_INFO,
                                 (void *)&params, sizeof(params));

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        convertLinkMasksToBitVector(&params.vbiosDisabledLinkMask, sizeof(params.vbiosDisabledLinkMask),
                                    &params.vbiosDisabledLinks, &pKernelNvlink->vbiosDisabledLinkMask));

    bitVectorClrAll(&localLinkMaskBitVector);
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        convertLinkMasksToBitVector(&params.initializedLinks, sizeof(params.initializedLinks),
                                    &params.initializedLinkMasks, &localLinkMaskBitVector));
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        convertBitVectorToLinkMasks(&localLinkMaskBitVector, &pKernelNvlink->initializedLinks,
                                    sizeof(pKernelNvlink->initializedLinks), NULL));

    bitVectorClrAll(&localLinkMaskBitVector);
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        convertLinkMasksToBitVector(&params.initDisabledLinksMask, sizeof(params.initDisabledLinksMask),
                                    &params.initDisabledLinks, &localLinkMaskBitVector));
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        convertBitVectorToLinkMasks(&localLinkMaskBitVector, &pKernelNvlink->initDisabledLinksMask,
                                    sizeof(pKernelNvlink->initDisabledLinksMask), NULL));

    pKernelNvlink->bEnableSafeModeAtLoad = params.bEnableSafeModeAtLoad;
    pKernelNvlink->bEnableTrainingAtLoad = params.bEnableTrainingAtLoad;

    return status;
}

/*!
 * @brief Update link connection status.
 *
 * @param[in]   pGpu           OBJGPU pointer
 * @param[in]   pKernelNvlink  KernelNvlink pointer
 * @param[in]   linkId         Target link Id
 */
NV_STATUS
knvlinkUpdateLinkConnectionStatus_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvU32         linkId
)
{
    NV_STATUS status = NV_OK;

    NV2080_CTRL_INTERNAL_NVLINK_UPDATE_LINK_CONNECTION_PARAMS params;

    portMemSet(&params, 0, sizeof(params));

    params.linkId = linkId;

#if defined(INCLUDE_NVLINK_LIB)

    params.bConnected = pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.bConnected;
    params.remoteDeviceType = pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.deviceType;
    params.remoteLinkNumber = pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.linkNumber;
    params.remoteChipSid = pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.chipSid;
    params.remoteDomain = pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.domain;
    params.remoteBus = pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.bus;
    params.remoteDevice = pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.device;
    params.remoteFunction = pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.function;
    params.remotePciDeviceId = pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.pciDeviceId;
    params.laneRxdetStatusMask = pKernelNvlink->nvlinkLinks[linkId].laneRxdetStatusMask;

#endif

    // Reset timeout to clear any accumulated timeouts from link init
    if (IS_GSP_CLIENT(pGpu))
    {
        threadStateResetTimeout(pGpu);
    }

    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_INTERNAL_NVLINK_UPDATE_LINK_CONNECTION,
                                 (void *)&params, sizeof(params));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to update Link connection status!\n");
        return status;
    }

    return NV_OK;
}

/*!
 * @brief Execute initial steps to Train links for ALI.
 *
 * @param[in] pGpu           OBJGPU pointer for local GPU
 * @param[in] pKernelNvlink  KernelNvlink pointer
 * @param[in] linkMask       Masks of links to enable
 * @param[in] bSync          Input sync boolean
 *
 */
NV_STATUS
knvlinkPreTrainLinksToActiveAli_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvU32         linkMask,
    NvBool        bSync
)
{
    NV_STATUS status = NV_OK;

    NV2080_CTRL_INTERNAL_NVLINK_PRE_LINK_TRAIN_ALI_PARAMS params;

    portMemSet(&params, 0, sizeof(params));

    params.linkMask = linkMask;
    params.bSync    = bSync;

    // Reset timeout to clear any accumulated timeouts from link init
    if (IS_GSP_CLIENT(pGpu))
    {
        threadStateResetTimeout(pGpu);
    }

    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_INTERNAL_NVLINK_PRE_LINK_TRAIN_ALI,
                                 (void *)&params, sizeof(params));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to execute Pre Link Training ALI steps!\n");
        return status;
    }

    return NV_OK;
}

/*!
 * @brief Train links to active for ALI.
 *
 * @param[in] pGpu           OBJGPU pointer for local GPU
 * @param[in] pKernelNvlink  KernelNvlink pointer
 * @param[in] linkMask       Masks of links to enable
 * @param[in] bSync          Input sync boolean
 *
 */
NV_STATUS
knvlinkTrainLinksToActiveAli_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvU32         linkMask,
    NvBool        bSync
)
{
    NV_STATUS status = NV_OK;

    NV2080_CTRL_INTERNAL_NVLINK_LINK_TRAIN_ALI_PARAMS params;

    portMemSet(&params, 0, sizeof(params));

    params.linkMask = linkMask;
    params.bSync    = bSync;

    // Reset timeout to clear any accumulated timeouts from link init
    if (IS_GSP_CLIENT(pGpu))
    {
        threadStateResetTimeout(pGpu);
    }

    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_INTERNAL_NVLINK_LINK_TRAIN_ALI,
                                 (void *)&params, sizeof(params));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to change ALI Links to active!\n");
        return status;
    }

    return NV_OK;
}

/*!
 * @brief Update the post Rx Detect link mask.
 *
 * @param[in] pGpu           OBJGPU pointer for local GPU
 * @param[in] pKernelNvlink  KernelNvlink pointer
 *
 */
NV_STATUS
knvlinkUpdatePostRxDetectLinkMask_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    NV_STATUS status = NV_OK;
    NvU32 i;

    NV2080_CTRL_INTERNAL_NVLINK_GET_LINK_MASK_POST_RX_DET_PARAMS params;

    portMemSet(&params, 0, sizeof(params));

    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_INTERNAL_NVLINK_GET_LINK_MASK_POST_RX_DET,
                                 (void *)&params, sizeof(params));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to update Rx Detect Link mask!\n");
        return status;
    }

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, 
        convertLinkMasksToBitVector(&params.postRxDetLinkMask, sizeof(params.postRxDetLinkMask), NULL, &pKernelNvlink->postRxDetLinkMask));

    FOR_EACH_IN_BITVECTOR(&pKernelNvlink->enabledLinks, i)
    {
        pKernelNvlink->nvlinkLinks[i].laneRxdetStatusMask = params.laneRxdetStatusMask[i];
    }
    FOR_EACH_IN_BITVECTOR_END();

    return NV_OK;
}

/*!
 * @brief Copy over the NVLink devices information from GSP-RM.
 *
 * @param[in] pGpu          OBJGPU pointer for local GPU
 * @param[in] pKernelNvlink KernelNvlink pointer
 */
NV_STATUS
knvlinkCopyNvlinkDeviceInfo_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    NV_STATUS status = NV_OK;
    NvU32     i;

    NV2080_CTRL_INTERNAL_NVLINK_GET_NVLINK_DEVICE_INFO_PARAMS *pNvlinkInfoParams =
        portMemAllocNonPaged(sizeof(*pNvlinkInfoParams));
    if (pNvlinkInfoParams == NULL)
    {
        return NV_ERR_NO_MEMORY;
    }

    portMemSet(pNvlinkInfoParams, 0, sizeof(*pNvlinkInfoParams));

    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_INTERNAL_NVLINK_GET_NVLINK_DEVICE_INFO,
                                 (void *)pNvlinkInfoParams, sizeof(*pNvlinkInfoParams));

    if (status == NV_ERR_NOT_SUPPORTED)
    {
        NV_PRINTF(LEVEL_INFO, "NVLink is unavailable\n");
        goto knvlinkCopyNvlinkDeviceInfo_cleanup;
    }
    else if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to retrieve all nvlink device info!\n");
        goto knvlinkCopyNvlinkDeviceInfo_cleanup;
    }

    // Update CPU-RM's NVLink state with the information received from GSP-RM RPC
    pKernelNvlink->ioctrlMask        = pNvlinkInfoParams->ioctrlMask;
    pKernelNvlink->ioctrlNumEntries  = pNvlinkInfoParams->ioctrlNumEntries;
    pKernelNvlink->ioctrlSize        = pNvlinkInfoParams->ioctrlSize;

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            convertLinkMasksToBitVector(NULL, 0U,
                                        &pNvlinkInfoParams->discoveredLinks,
                                        &pKernelNvlink->discoveredLinks));

    pKernelNvlink->ipVerNvlink       = pNvlinkInfoParams->ipVerNvlink;
    pKernelNvlink->maxSupportedLinks = pNvlinkInfoParams->maxSupportedLinks;

    for (i = 0; i < pKernelNvlink->maxSupportedLinks; i++)
    {
        pKernelNvlink->nvlinkLinks[i].pGpu     = pGpu;
        pKernelNvlink->nvlinkLinks[i].bValid   = pNvlinkInfoParams->linkInfo[i].bValid;
        pKernelNvlink->nvlinkLinks[i].linkId   = pNvlinkInfoParams->linkInfo[i].linkId;
        pKernelNvlink->nvlinkLinks[i].ioctrlId = pNvlinkInfoParams->linkInfo[i].ioctrlId;

        // Copy over the link PLL master and slave relationship for each link
        pKernelNvlink->nvlinkLinks[i].pllMasterLinkId = pNvlinkInfoParams->linkInfo[i].pllMasterLinkId;
        pKernelNvlink->nvlinkLinks[i].pllSlaveLinkId  = pNvlinkInfoParams->linkInfo[i].pllSlaveLinkId;

        // Copy over the ip versions for DLPL devices discovered
        pKernelNvlink->nvlinkLinks[i].ipVerDlPl = pNvlinkInfoParams->linkInfo[i].ipVerDlPl;
    }
knvlinkCopyNvlinkDeviceInfo_cleanup:
    portMemFree(pNvlinkInfoParams);
    return status;
}

/*!
 * @brief Copy over the Ioctrl devices information from GSP-RM.
 *
 * @param[in] pGpu          OBJGPU pointer for local GPU
 * @param[in] pKernelNvlink KernelNvlink pointer
 */
NV_STATUS
knvlinkCopyIoctrlDeviceInfo_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    KernelIoctrl *pKernelIoctrl = NULL;
    NV_STATUS     status        = NV_OK;
    NvU32         ioctrlIdx;

    NV2080_CTRL_INTERNAL_NVLINK_GET_IOCTRL_DEVICE_INFO_PARAMS ioctrlInfoParams;

    // Query the IOCTRL information for each of the IOCTRLs discovered
    FOR_EACH_INDEX_IN_MASK(32, ioctrlIdx, pKernelNvlink->ioctrlMask)
    {
        portMemSet(&ioctrlInfoParams, 0, sizeof(ioctrlInfoParams));

        ioctrlInfoParams.ioctrlIdx = ioctrlIdx;

        status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                     NV2080_CTRL_CMD_INTERNAL_NVLINK_GET_IOCTRL_DEVICE_INFO,
                                     (void *)&ioctrlInfoParams, sizeof(ioctrlInfoParams));

        if (status == NV_ERR_NOT_SUPPORTED)
        {
            NV_PRINTF(LEVEL_WARNING, "NVLink is unavailable\n");
            return status;
        }
        else if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Failed to retrieve device info for IOCTRL %d!\n", ioctrlIdx);
            return status;
        }

        pKernelIoctrl = KNVLINK_GET_IOCTRL(pKernelNvlink, ioctrlIdx);

        // Update CPU-RM's NVLink state with the information received from GSP-RM RPC
        pKernelIoctrl->PublicId              = ioctrlInfoParams.PublicId;
        pKernelIoctrl->localDiscoveredLinks  = ioctrlInfoParams.localDiscoveredLinks;
        pKernelIoctrl->localGlobalLinkOffset = ioctrlInfoParams.localGlobalLinkOffset;
        pKernelIoctrl->ioctrlDiscoverySize   = ioctrlInfoParams.ioctrlDiscoverySize;
        pKernelIoctrl->numDevices            = ioctrlInfoParams.numDevices;

        // Copy over the ip versions for the ioctrl and minion devices discovered
        pKernelIoctrl->ipVerIoctrl = ioctrlInfoParams.ipRevisions.ipVerIoctrl;
        pKernelIoctrl->ipVerMinion = ioctrlInfoParams.ipRevisions.ipVerMinion;

        if (pKernelIoctrl->ipVerMinion == 0)
        {
            pKernelIoctrl->setProperty(pKernelIoctrl, PDB_PROP_KIOCTRL_MINION_AVAILABLE, NV_FALSE);
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

    return NV_OK;
}

/**
 * @brief Setup topology information for the forced nvlink configurations
 *
 * @param[in] pGpu          OBJGPU pointer for local GPU
 * @param[in] pKernelNvlink KernelNvlink pointer
 */
NV_STATUS
knvlinkSetupTopologyForForcedConfig_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    NV_STATUS status  = NV_OK;
    NvU32     i, physLink;
    NVLINK_BIT_VECTOR registryLinkMaskVec;

    // Start with all links disabled and no forced config in effect
    pKernelNvlink->bRegistryLinkOverride = NV_TRUE;
    pKernelNvlink->registryLinkMask      = 0;
    pKernelNvlink->bChiplibConfig        = NV_FALSE;

    for (i = 0; i < NVLINK_MAX_LINKS_SW; i++)
    {
        // Filter against the links discovered from IOCTRL
        if (!(bitVectorTest(&pKernelNvlink->discoveredLinks, i) ))
            continue;

        // The physical link is guaranteed valid in all cases
        physLink = DRF_VAL(_NVLINK, _ARCH_CONNECTION, _PHYSICAL_LINK, pKernelNvlink->pLinkConnection[i]);

        // Update link tracking
        if (DRF_VAL(_NVLINK, _ARCH_CONNECTION, _ENABLED, pKernelNvlink->pLinkConnection[i]))
        {
            NV_PRINTF(LEVEL_INFO,
                      "ARCH_CONNECTION info from chiplib: ENABLED Logical link %d (Physical "
                      "link %d) = 0x%X\n", i, physLink,
                      pKernelNvlink->pLinkConnection[i]);

            //
            // This "link" should be ENABLED. We use the physical link since RM only deals with
            // physical links.
            //
            pKernelNvlink->registryLinkMask |= NVBIT64(physLink);

            // Config is forced (at least one link requested)
            pKernelNvlink->bChiplibConfig = NV_TRUE;
        }
        else
        {
            NV_PRINTF(LEVEL_INFO,
                      "ARCH_CONNECTION info from chiplib: DISABLED Logical link %d (Physical "
                      "link %d) = 0x%X\n", i, physLink,
                      pKernelNvlink->pLinkConnection[i]);
        }

        // Accumulate any PEER links
        if (DRF_VAL(_NVLINK, _ARCH_CONNECTION, _PEER_MASK, pKernelNvlink->pLinkConnection[i]))
        {
#if defined(INCLUDE_NVLINK_LIB)
            // Ensure reginit has the info it needs for the remote side
            pKernelNvlink->nvlinkLinks[i].remoteEndInfo.bConnected = NV_TRUE;
            pKernelNvlink->nvlinkLinks[i].remoteEndInfo.deviceType =
                                                    NV2080_CTRL_NVLINK_DEVICE_INFO_DEVICE_TYPE_GPU;

#endif
        }

        // Accumulate any CPU links
        if (DRF_VAL(_NVLINK, _ARCH_CONNECTION, _CPU, pKernelNvlink->pLinkConnection[i]))
        {
#if defined(INCLUDE_NVLINK_LIB)
            // Ensure reginit has the info it needs for the remote side
            pKernelNvlink->nvlinkLinks[i].remoteEndInfo.bConnected = NV_TRUE;
            pKernelNvlink->nvlinkLinks[i].remoteEndInfo.deviceType = pKernelNvlink->forcedSysmemDeviceType;
#endif
        }

        // RPC into GSP-RM to update the link remote connection status
        status = knvlinkUpdateLinkConnectionStatus(pGpu, pKernelNvlink, i);
        if (status != NV_OK)
        {
            return status;
        }
    }

    // Update enabledLinks mask with the mask of forced link configurations
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        convertMaskToBitVector(KNVLINK_GET_MASK(pKernelNvlink, registryLinkMask, 64), &registryLinkMaskVec));
    bitVectorAnd(&pKernelNvlink->enabledLinks, &pKernelNvlink->discoveredLinks, &registryLinkMaskVec);

    return NV_OK;
}

/*!
 * @brief Sync the lane shutdown properties with GSP-RM
 *
 * @param[in] pGpu          OBJGPU pointer
 * @param[in] pKernelNvlink KernelNvlink pointer
 */
NV_STATUS
knvlinkSyncLaneShutdownProps_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    NV_STATUS status = NV_OK;

    NV2080_CTRL_INTERNAL_NVLINK_SYNC_NVLINK_SHUTDOWN_PROPS_PARAMS params;

    portMemSet(&params, 0, sizeof(params));

    params.bLaneShutdownOnUnload =
        pKernelNvlink->getProperty(pKernelNvlink, PDB_PROP_KNVLINK_LANE_SHUTDOWN_ON_UNLOAD);

    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_INTERNAL_NVLINK_SYNC_NVLINK_SHUTDOWN_PROPS,
                                 (void *)&params, sizeof(params));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to sync NVLink shutdown properties with GSP!\n");
        return status;
    }

    return NV_OK;
}

/*!
 * @brief   Get the number of active links allowed per IOCTRL
 *
 * @param[in] pGpu           OBJGPU pointer
 * @param[in] pKernelNvlink  KernelNvlink pointer
 *
 * @returns On success, returns the number of active links per IOCTRL.
 *          On failure, returns 0.
 */
NvU32
knvlinkGetNumActiveLinksPerIoctrl_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    NV_STATUS status;
    NV2080_CTRL_INTERNAL_NVLINK_GET_NUM_ACTIVE_LINK_PER_IOCTRL_PARAMS params;
    portMemSet(&params, 0, sizeof(params));
    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_INTERNAL_NVLINK_GET_NUM_ACTIVE_LINK_PER_IOCTRL,
                                 (void *)&params, sizeof(params));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to get the number of active links per IOCTRL\n");
        return 0;
    }
    return params.numActiveLinksPerIoctrl;
}

/*!
 * @brief   Get the number of total links  per IOCTRL
 *
 * @param[in] pGpu           OBJGPU pointer
 * @param[in] pKernelNvlink  KernelNvlink pointer
 *
 * @returns On success, returns the number of total links per IOCTRL.
 *          On failure, returns 0.
 */
NvU32
knvlinkGetTotalNumLinksPerIoctrl_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    NV_STATUS status;
    NV2080_CTRL_INTERNAL_NVLINK_GET_TOTAL_NUM_LINK_PER_IOCTRL_PARAMS params;
    portMemSet(&params, 0, sizeof(params));
    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_INTERNAL_NVLINK_GET_TOTAL_NUM_LINK_PER_IOCTRL,
                                 (void *)&params, sizeof(params));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to get the total number of links per IOCTRL\n");
        return 0;
    }
    return params.numLinksPerIoctrl;
}

/**
 * @brief Process the mask of init disabled links
 *
 * @param[in] pGpu          OBJGPU pointer
 * @param[in] pKernelNvlink KernelNvlink pointer
 */
NV_STATUS
knvlinkProcessInitDisabledLinks_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    NV2080_CTRL_NVLINK_LINK_MASK links = {0};
    NvBool    bSkipHwNvlinkDisable = 0;
    NV_STATUS status               = NV_OK;
    NVLINK_BIT_VECTOR localLinkMask;

    NV2080_CTRL_NVLINK_PROCESS_INIT_DISABLED_LINKS_PARAMS params = {0};

    status = gpumgrGetGpuInitDisabledNvlinks(pGpu->gpuId, &links, &bSkipHwNvlinkDisable);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to get init disabled links from gpumgr\n");
        return status;
    }

    portMemSet(&params, 0, sizeof(params));

    params.initDisabledLinks = links;
    params.bSkipHwNvlinkDisable = bSkipHwNvlinkDisable;

    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_NVLINK_PROCESS_INIT_DISABLED_LINKS,
                                 (void *)&params, sizeof(params));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to process init disabled links in GSP\n");
        return status;
    }

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        convertLinkMasksToBitVector(NULL, 0, &params.initDisabledLinks, &localLinkMask));

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        convertBitVectorToLinkMasks(&localLinkMask, &pKernelNvlink->initDisabledLinksMask,
                                    sizeof(pKernelNvlink->initDisabledLinksMask), NULL));

    return NV_OK;
}

void
knvlinkLazyErrorRecovery_WORKITEM
(
    OBJGPU *pGpu,
    void *pArgs
)
{
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    RmClient **ppClient;

    // For each client
    for (ppClient = serverutilGetFirstClientUnderLock(); ppClient; ppClient = serverutilGetNextClientUnderLock(ppClient))
    {
        RmClient *pClient = *ppClient;
        RsClient *pRsClient = staticCast(pClient, RsClient);
        RS_PRIV_LEVEL privLevel = rmclientGetCachedPrivilege(pClient);

        // Skip kernel mode and internal RM clients
        if ((privLevel >= RS_PRIV_LEVEL_KERNEL) && rmclientIsAdmin(pClient, privLevel))
            continue;

        {
            RS_ITERATOR p2pIt;

            // For each P2P object
            p2pIt = clientRefIter(pRsClient, NULL, classId(P2PApi), RS_ITERATE_CHILDREN, NV_TRUE);
            while (clientRefIterNext(p2pIt.pClient, &p2pIt))
            {
                P2PApi *pP2PApi = dynamicCast(p2pIt.pResourceRef->pResource, P2PApi);
                if (pP2PApi == NULL)
                    continue;

                //
                // If it's an active peer connection with this GPU, defer
                // recovery.
                //
                if ((pP2PApi->peer1 == pGpu) || (pP2PApi->peer2 == pGpu))
                    return;
            }
        }

        {
            // For each device
            RS_ITERATOR deviceIt = clientRefIter(pRsClient, NULL, classId(Device), RS_ITERATE_CHILDREN, NV_TRUE);
            while (clientRefIterNext(pRsClient, &deviceIt))
            {
                Device *pDevice = dynamicCast(deviceIt.pResourceRef->pResource, Device);

                // Skip devices which don't match the one we're checking
                if ((pDevice == NULL) || (GPU_RES_GET_GPU(pDevice) != pGpu))
                    continue;

                {
                    // For each channel
                    RS_ORDERED_ITERATOR kchannelIt = kchannelGetIter(pRsClient, deviceIt.pResourceRef);
                    while (clientRefOrderedIterNext(pRsClient, &kchannelIt))
                    {
                        KernelChannel *pKernelChannel = dynamicCast(kchannelIt.pResourceRef->pResource, KernelChannel);

                        if (pKernelChannel == NULL)
                            continue;

                        //
                        // If it's UVM-managed, assume it's P2P active on this
                        // GPU. Defer recovery.
                        //
                        if (gvaspaceIsExternallyOwned(dynamicCast(pKernelChannel->pVAS, OBJGVASPACE)))
                            return;
                    }
                }
            }
        }
    }

    NV_PRINTF(LEVEL_INFO, "Detected fabric idle with lazy fatal error pending. Triggering fatal recovery!\n");

    NV_ASSERT_OK(
        pRmApi->Control(pRmApi,
                        pGpu->hInternalClient,
                        pGpu->hInternalSubdevice,
                        NV2080_CTRL_CMD_NVLINK_POST_LAZY_ERROR_RECOVERY,
                        NULL,
                        0));

    osRemove1HzCallback(pGpu, knvlinkLazyErrorRecovery_WORKITEM, pArgs);
}

void
knvlinkFatalErrorRecovery_WORKITEM
(
    NvU32 gpuInstance,
    void  *pArgs
)
{
    OBJGPU *pGpu = gpumgrGetGpu(gpuInstance);
    (void)rcAndDisableOutstandingClientsWithImportedMemory(pGpu, NV_FABRIC_INVALID_NODE_ID);

    {
        NVLINK_UNCONTAINED_ERROR_RECOVERY_INFO *pInfo = (NVLINK_UNCONTAINED_ERROR_RECOVERY_INFO *)pArgs;
        if (pInfo != NULL)
            portAtomicSetU32(&pInfo->rcCompleted, 1);
    }
}

void
knvlinkUncontainedErrorRecoveryUvmIdle_WORKITEM
(
    NvU32 gpuInstance,
    void  *pArgs
)
{
    NVLINK_UNCONTAINED_ERROR_RECOVERY_INFO *pInfo = (NVLINK_UNCONTAINED_ERROR_RECOVERY_INFO *)pArgs;
    NV_STATUS status;

    NV_ASSERT_OR_RETURN_VOID(pInfo != NULL);

    status = osQueueDrainP2PHandler(pInfo->uuid);
    switch (status)
    {
    case NV_ERR_NOT_SUPPORTED:
        // UVM never installed this callback.

        // fall-through
    case NV_ERR_INVALID_DEVICE:
        // This device was never registered with UVM.

        // fall-through
    case NV_ERR_BUSY_RETRY:
        // UVM channels are already suspended.

        // fall-through
    case NV_OK:
        // UVM channels were successfully suspended

        portAtomicSetU32(&pInfo->uvmIdle, 1);
        break;

    case NV_ERR_ECC_ERROR:
        // UVM channel hit ECC error and cannot suspend.

        // fall-through
    case NV_ERR_RC_ERROR:
        // UVM channel hit RC error and cannot suspend.

        // fall-through
    default:
        NV_PRINTF(LEVEL_ERROR, "Failed to idle UVM peer traffic with status 0x%x. This will lead to NVLINK Degradation!\n",
                  status);
        break;
    }
}

void
knvlinkUncontainedErrorRecoveryUvmResume_WORKITEM
(
    NvU32 gpuInstance,
    void  *pArgs
)
{
    NVLINK_UNCONTAINED_ERROR_RECOVERY_INFO *pInfo = (NVLINK_UNCONTAINED_ERROR_RECOVERY_INFO *)pArgs;

    NV_ASSERT_OR_RETURN_VOID(pInfo != NULL);

    osQueueResumeP2PHandler(pInfo->uuid);

    // Clear the active recovery
    portAtomicSetU32(&pInfo->active, 0);
}

void
knvlinkUncontainedErrorRecoveryReadyCheck_WORKITEM
(
    OBJGPU *pGpu,
    void *pArgs
)
{
    NVLINK_UNCONTAINED_ERROR_RECOVERY_INFO *pInfo = (NVLINK_UNCONTAINED_ERROR_RECOVERY_INFO *)pArgs;
    OBJTMR *pTmr = GPU_GET_TIMER(pGpu);
    NvU64 currentTime;
    NV_STATUS status = NV_OK;

    NV_ASSERT_OR_RETURN_VOID(pInfo != NULL);

    NV_ASSERT_OK_OR_GOTO(status, tmrGetCurrentTime(pTmr, &currentTime), remove);
    if ((currentTime - pInfo->startTime) <= NVLINK_UNCONTAINED_ERROR_IDLE_PERIOD_NS)
        return;

    portAtomicSetU32(&pInfo->recoveryReady, 1);

remove:
    osRemove1HzCallback(pGpu, knvlinkUncontainedErrorRecoveryReadyCheck_WORKITEM, pArgs);
}

void
knvlinkAbortUncontainedErrorRecovery_WORKITEM
(
    NvU32 gpuInstance,
    void *pArgs
)
{
    OBJGPU *pGpu = gpumgrGetGpu(gpuInstance);
    NV2080_CTRL_INTERNAL_NVLINK_POST_FATAL_ERROR_RECOVERY_PARAMS params = { 0 };
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

    NV_PRINTF(LEVEL_ERROR, "Failed to recover from uncontained NVLINK error. Triggering Degraded Mode!\n");

    params.bSuccessful = NV_FALSE;
    NV_ASSERT_OK(
        pRmApi->Control(pRmApi,
                        pGpu->hInternalClient,
                        pGpu->hInternalSubdevice,
                        NV2080_CTRL_CMD_INTERNAL_NVLINK_POST_FATAL_ERROR_RECOVERY,
                        &params,
                        sizeof(params)));
}

void
knvlinkUncontainedErrorRecovery_WORKITEM
(
    OBJGPU *pGpu,
    void *pArgs
)
{
    NVLINK_UNCONTAINED_ERROR_RECOVERY_INFO *pInfo = (NVLINK_UNCONTAINED_ERROR_RECOVERY_INFO *)pArgs;
    OBJTMR *pTmr = GPU_GET_TIMER(pGpu);
    NvU64 currentTime;
    NvBool bRemove = NV_FALSE;
    NvBool bDegrade = NV_FALSE;
    NV_STATUS status = NV_OK;

    NV_ASSERT_OR_RETURN_VOID(pInfo != NULL);

    NV_ASSERT_OK_OR_GOTO(status, tmrGetCurrentTime(pTmr, &currentTime), exit);

    // If we do not successfully idle within a resonable time, degrade
    if ((currentTime - pInfo->startTime) > NVLINK_UNCONTAINED_ERROR_ABORT_PERIOD_NS)
    {
        bRemove = NV_TRUE;
        bDegrade = NV_TRUE;
        // One more pass in case it just took a long time to get scheduled
    }

    if (portAtomicOrU32(&pInfo->rcCompleted, 0) == 0)
        goto exit;

    if (portAtomicOrU32(&pInfo->uvmIdle, 0) == 0)
        goto exit;

    if (portAtomicOrU32(&pInfo->recoveryReady, 0) == 0)
        goto exit;

    bRemove = NV_TRUE;
    bDegrade = NV_FALSE;

    // Launch recovery action in the HW to allow new traffic
    {
        NV2080_CTRL_INTERNAL_NVLINK_POST_FATAL_ERROR_RECOVERY_PARAMS params = { 0 };
        RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

        params.bSuccessful = NV_TRUE;
        NV_ASSERT_OK_OR_GOTO(status,
            pRmApi->Control(pRmApi,
                            pGpu->hInternalClient,
                            pGpu->hInternalSubdevice,
                            NV2080_CTRL_CMD_INTERNAL_NVLINK_POST_FATAL_ERROR_RECOVERY,
                            &params,
                            sizeof(params)),
            exit);
    }

    // Launch lockless workitem to resume P2P in UVM
    NV_ASSERT_OK_OR_GOTO(status,
        osQueueWorkItem(pGpu,
                        knvlinkUncontainedErrorRecoveryUvmResume_WORKITEM,
                        pInfo,
                        (OsQueueWorkItemFlags){.bDontFreeParams = NV_TRUE}),
        exit);

exit:
    if (status != NV_OK)
    {
        bRemove = NV_TRUE;
        bDegrade = NV_TRUE;
    }

    if (bRemove)
        osRemove1HzCallback(pGpu, knvlinkUncontainedErrorRecovery_WORKITEM, pArgs);

    if (bDegrade)
    {
        NV_ASSERT_OK_OR_CAPTURE_FIRST_ERROR(status,
            osQueueWorkItem(pGpu,
                knvlinkAbortUncontainedErrorRecovery_WORKITEM,
                NULL,
                (OsQueueWorkItemFlags){
                    .bLockSema = NV_TRUE,
                    .apiLock = WORKITEM_FLAGS_API_LOCK_READ_WRITE,
                    .bLockGpuGroupSubdevice = NV_TRUE}));
    }
}

NV_STATUS
knvlinkFatalErrorRecovery_IMPL
(
    OBJGPU *pGpu,
    KernelNvlink *pKernelNvlink,
    NvBool bRecoverable,
    NvBool bLazy
)
{
    NV_STATUS status = NV_OK;
    NVLINK_UNCONTAINED_ERROR_RECOVERY_INFO *pInfo = gpumgrGetNvlinkRecoveryInfo(gpuGetDBDF(pGpu));

    if (bLazy)
    {
        //
        // osSchedule1HzCallback returns NV_ERR_INVALID_REQUEST on any error,
        // including if the callback is already scheduled.
        //
        (void)osSchedule1HzCallback(pGpu, knvlinkLazyErrorRecovery_WORKITEM, NULL, NV_OS_1HZ_REPEAT);
        return NV_OK;
    }
    else
    {
        //
        // If a non-lazy error fires, we've already triggered RC, so don't
        // bother trying to process the lazy error anymore if active.
        //
        (void)osRemove1HzCallback(pGpu, knvlinkLazyErrorRecovery_WORKITEM, NULL);
    }

    if (bRecoverable && pKernelNvlink->getProperty(pKernelNvlink, PDB_PROP_KNVLINK_UNCONTAINED_ERROR_RECOVERY_SUPPORTED))
    {
        OBJTMR *pTmr = GPU_GET_TIMER(pGpu);

        if ((pInfo == NULL) || !pInfo->bValid)
        {
            status = NV_ERR_INVALID_STATE;
            goto fail;
        }

        // This recovery process should not be able to occur twice synchronously
        if (portAtomicOrU32(&pInfo->active, 0) != 0)
        {
            NV_ASSERT_FAILED("NVLINK Uncontained error recovery re-triggered unexpectedly!");
            status = NV_ERR_INVALID_STATE;
            goto fail;
        }

        // Kickoff the recovery process
        portAtomicSetU32(&pInfo->active, 1);
        portAtomicSetU32(&pInfo->rcCompleted, 0);
        portAtomicSetU32(&pInfo->uvmIdle, 0);
        portAtomicSetU32(&pInfo->recoveryReady, 0);
        NV_ASSERT_OK_OR_GOTO(status, tmrGetCurrentTime(pTmr, &pInfo->startTime), fail);

        {
            NvU32 flags = DRF_DEF(2080_GPU_CMD, _GPU_GET_GID_FLAGS, _TYPE, _SHA1) |
                DRF_DEF(2080_GPU_CMD, _GPU_GET_GID_FLAGS, _FORMAT, _BINARY);
            NvU32 uuidLength;
            NvU8 *pUuid;

            // allocates memory for pUuid on success
            NV_ASSERT_OK_OR_GOTO(status, gpuGetGidInfo(pGpu, &pUuid, &uuidLength, flags), fail);
            NV_ASSERT_OR_GOTO(uuidLength == sizeof(pInfo->uuid), fail);

            portMemCopy(pInfo->uuid, uuidLength, pUuid, uuidLength);
            portMemFree(pUuid);
        }

        // Launch workitem to RC outstanding IMEX clients
        NV_CHECK_OK_OR_GOTO(status,
            LEVEL_ERROR,
            osQueueWorkItem(pGpu,
                            knvlinkFatalErrorRecovery_WORKITEM,
                            pInfo,
                            (OsQueueWorkItemFlags){
                                .bLockSema = NV_TRUE,
                                .apiLock = WORKITEM_FLAGS_API_LOCK_READ_WRITE,
                                .bLockGpuGroupSubdevice = NV_TRUE,
                                .bDontFreeParams = NV_TRUE}),
            fail);

        // Launch lockless workitem to idle UVM channels
        NV_CHECK_OK_OR_GOTO(status,
            LEVEL_ERROR,
            osQueueWorkItem(pGpu,
                            knvlinkUncontainedErrorRecoveryUvmIdle_WORKITEM,
                            pInfo,
                            (OsQueueWorkItemFlags){.bDontFreeParams = NV_TRUE}),
            fail);

        // Launch repeated 1Hz workitem to wait 1 STO period
        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
            osSchedule1HzCallback(pGpu, knvlinkUncontainedErrorRecoveryReadyCheck_WORKITEM, pInfo, NV_OS_1HZ_REPEAT),
            fail);

        // Launch repeated 1Hz workitem to await completion and kickoff recovery process
        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
            osSchedule1HzCallback(pGpu, knvlinkUncontainedErrorRecovery_WORKITEM, pInfo, NV_OS_1HZ_REPEAT),
            fail);
    }
    else
    {
        (void)gpuMarkDeviceForReset(pGpu);

        status = osQueueWorkItem(pGpu,
            knvlinkFatalErrorRecovery_WORKITEM,
            NULL,
            (OsQueueWorkItemFlags){
                .bLockSema = NV_TRUE,
                .apiLock = WORKITEM_FLAGS_API_LOCK_READ_WRITE,
                .bLockGpuGroupSubdevice = NV_TRUE});
    }

    return status;

fail:
    NV_ASSERT_OK_OR_CAPTURE_FIRST_ERROR(status,
        osQueueWorkItem(pGpu,
                        knvlinkAbortUncontainedErrorRecovery_WORKITEM,
                        NULL,
                        (OsQueueWorkItemFlags){
                            .bLockSema = NV_TRUE,
                            .apiLock = WORKITEM_FLAGS_API_LOCK_READ_WRITE,
                            .bLockGpuGroupSubdevice = NV_TRUE}));

    return status;
}

// Grab GPU locks before RPCing into GSP-RM for NVLink RPCs
NV_STATUS
knvlinkExecGspRmRpc_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvU32         cmd,
    void         *paramAddr,
    NvU32         paramSize
)
{
    NvU32     gpuMaskRelease = 0;
    NvU32     gpuMaskInitial = rmGpuLocksGetOwnedMask();
    NvU32     gpuMask        = gpuMaskInitial | NVBIT(pGpu->gpuInstance);
    NV_STATUS status         = NV_OK;

    if (IS_GSP_CLIENT(pGpu))
    {
        if (!rmGpuGroupLockIsOwner(pGpu->gpuInstance, GPU_LOCK_GRP_MASK, &gpuMask))
        {
            status = rmGpuGroupLockAcquire(pGpu->gpuInstance,
                                           GPU_LOCK_GRP_MASK,
                                           GPU_LOCK_FLAGS_SAFE_LOCK_UPGRADE,
                                           RM_LOCK_MODULES_NVLINK,
                                           &gpuMask);
            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR, "Failed to acquire locks for gpumask 0x%x\n", gpuMask);
                return status;
            }

            gpuMaskRelease = (gpuMask & (~gpuMaskInitial));
        }
    }

    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    status = pRmApi->Control(pRmApi,
                             pGpu->hInternalClient,
                             pGpu->hInternalSubdevice,
                             cmd, paramAddr, paramSize);
    if (gpuMaskRelease)
    {
        rmGpuGroupLockRelease(gpuMaskRelease, GPUS_LOCK_FLAGS_NONE);
    }

    return status;
}

NvU64
knvlinkGetBWModeEpoch_IMPL
(
    OBJGPU *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    return (pKernelNvlink == NULL) ? 0 : portAtomicExOrU64(&pKernelNvlink->nvlinkBwModeEpoch, 0);
}

void
knvlinkSetBWModeEpoch_IMPL
(
    OBJGPU *pGpu,
    KernelNvlink *pKernelNvlink,
    NvU64 bwModeEpoch
)
{
    portAtomicExSetU64(&pKernelNvlink->nvlinkBwModeEpoch, bwModeEpoch);
}

/*!
 * Send the NVLE key to keymgr through the GSP keymgr interface for initial key setup
 *
 * @param[in] pGpu              : OBJGPU pointer
 * @param[in] pKernelNvlink     : KernelNvlink pointer
 * @param[in] pKey              : NVLE Key
 * @param[in] remoteScfDcfGpuId : Local CLID
 */
static NV_STATUS
_knvlinkSendEncryptionKeys
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvU8         *pKey,
    NvU32         remoteScfDcfGpuId
)
{
    NvU8                             nvleKeyReqBuf[sizeof(RM_SPDM_NV_CMD_REQ_KEYMGR_NVLE) +
                                                   sizeof(RM_GSP_NVLE_UPDATE_SESSION_KEYS)] = {0};
    NvU8                            *pNvleKeyReq                                            = nvleKeyReqBuf;
    RM_SPDM_NV_CMD_REQ_KEYMGR_NVLE  *pSpdmReqHdr                                            = NULL;
    RM_GSP_NVLE_UPDATE_SESSION_KEYS *pGspReqHdr                                             = NULL;
    NvU32                            nvleKeyReqSize                                         = 0;
    RM_SPDM_NV_CMD_RSP               nvleKeyRsp                                             = {0};
    NvU32                            nvleKeyRspSize                                         = sizeof(nvleKeyRsp);
    ConfidentialCompute             *pConfCompute                                           = NULL;
    Spdm                            *pSpdm                                                  = NULL;

    if (pGpu == NULL || pKey == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    pConfCompute = GPU_GET_CONF_COMPUTE(pGpu);
    pSpdm        = GPU_GET_SPDM(pGpu);

    if (pConfCompute == NULL || pSpdm == NULL || !pSpdm->bSessionEstablished)
    {
        return NV_ERR_INVALID_STATE;
    }

    // SPDM App Message header wraps the entire message for GSP
    pSpdmReqHdr                                  = (RM_SPDM_NV_CMD_REQ_KEYMGR_NVLE *)pNvleKeyReq;
    pSpdmReqHdr->hdr.cmdType                     = RM_SPDM_NV_CMD_TYPE_REQ_KEYMGR_NVLE;

    nvleKeyReqSize = sizeof(RM_SPDM_NV_CMD_REQ_KEYMGR_NVLE) + sizeof(RM_GSP_NVLE_UPDATE_SESSION_KEYS);
    pGspReqHdr                                   = (RM_GSP_NVLE_UPDATE_SESSION_KEYS *)
                                                   ((NvU8 *)pNvleKeyReq + sizeof(RM_SPDM_NV_CMD_REQ_KEYMGR_NVLE));

    pGspReqHdr->cmdId                                  = RM_GSP_NVLE_CMD_ID_UPDATE_SESSION_KEYS;
    pGspReqHdr->bForKeyRotation                        = NV_FALSE;
    pGspReqHdr->wrappedKeyEntries[0].remoteScfDcfGpuId = remoteScfDcfGpuId;
    pGspReqHdr->wrappedKeyEntries[0].bValid            = NV_TRUE;
    portMemCopy(pGspReqHdr->wrappedKeyEntries[0].key, sizeof(pGspReqHdr->wrappedKeyEntries[0].key),
                pKey, RM_GSP_NVLE_AES_256_GCM_KEY_SIZE_BYTES);

    // Encrypt the key before sending.
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
              ccslEncrypt(pConfCompute->pNvleP2pWrappingCcslCtx,
                         sizeof(pGspReqHdr->wrappedKeyEntries),
                         (NvU8 *)pGspReqHdr->wrappedKeyEntries,
                         NULL, 0,
                         (NvU8 *)pGspReqHdr->wrappedKeyEntries,
                         (NvU8 *)pGspReqHdr->keyEntriesTag));

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, spdmSendApplicationMessage(pGpu, pSpdm, pNvleKeyReq, nvleKeyReqSize,
                                                                  (NvU8 *)&nvleKeyRsp, &nvleKeyRspSize));
    if ((nvleKeyRspSize < sizeof(RM_SPDM_NV_CMD_HDR)) || (nvleKeyRspSize > sizeof(RM_SPDM_NV_CMD_RSP)))
    {
        // Somehow, we got an entirely invalid response
        NV_PRINTF(LEVEL_ERROR, "NVLE response from GSP of invalid size! rspSize: 0x%x!\n", nvleKeyRspSize);
        return NV_ERR_INVALID_DATA;
    }

    // Check for known error response or any non-success response
    if ((nvleKeyRsp.hdr.cmdType == RM_SPDM_NV_CMD_TYPE_RSP_ERROR) &&
        (nvleKeyRspSize         == sizeof(RM_SPDM_NV_CMD_RSP_ERROR)))
    {
        NV_PRINTF(LEVEL_ERROR, "GSP returned NVLE response with error code 0x%x!\n", nvleKeyRsp.error.status);
        return (nvleKeyRsp.error.status != NV_OK) ? nvleKeyRsp.error.status : NV_ERR_INVALID_DATA;
    }
    else if (nvleKeyRsp.hdr.cmdType != RM_SPDM_NV_CMD_TYPE_RSP_SUCCESS ||
             nvleKeyRspSize         != sizeof(RM_SPDM_NV_CMD_RSP_SUCCESS))
    {
        NV_PRINTF(LEVEL_ERROR, "Unexpected NVLE response from GSP! cmdType: 0x%x rspSize: 0x%x!\n",
                  nvleKeyRsp.hdr.cmdType, nvleKeyRspSize);
        return NV_ERR_INVALID_DATA;
    }

    return NV_OK;
}

/*!
 * Send the NVLE key to keymgr through the GSP keymgr interface for key refresh
 *
 * @param[in] pGpu              : OBJGPU pointer
 * @param[in] pKernelNvlink     : KernelNvlink pointer
 * @param[in] pKey              : NVLE Key
 * @param[in] remoteScfDcfGpuId : Local CLID
 * @param[in] stage             : Whether Tx or Rx keyslot needs to be refreshed
 * @param[in] epoch             : Epoch will inform which keyslot<0,1> is spare
 */
static NV_STATUS
_knvlinkRefreshEncryptionKeys
(
    OBJGPU                 *pGpu,
    KernelNvlink           *pKernelNvlink,
    NvU8                   *pKey,
    NvU32                   remoteScfDcfGpuId,
    sessionKeyRefreshStage  stage,
    NvU8                    epoch
)
{
    NvU8                              nvleKeyReqBuf[sizeof(RM_SPDM_NV_CMD_REQ_KEYMGR_NVLE) +
                                                    sizeof(RM_GSP_NVLE_REFRESH_SESSION_KEYS)] = {0};
    NvU8                             *pNvleKeyReq                                             = nvleKeyReqBuf;
    RM_SPDM_NV_CMD_REQ_KEYMGR_NVLE   *pSpdmReqHdr                                             = NULL;
    RM_GSP_NVLE_REFRESH_SESSION_KEYS *pGspReqHdr                                              = NULL;
    NvU32                             nvleKeyReqSize                                          = 0;
    RM_SPDM_NV_CMD_RSP                nvleKeyRsp                                              = {0};
    NvU32                             nvleKeyRspSize                                          = sizeof(nvleKeyRsp);
    ConfidentialCompute              *pConfCompute                                            = NULL;
    Spdm                             *pSpdm                                                   = NULL;

    if ((pGpu == NULL) || (pKey == NULL) || (stage > refreshTxKeySlot) || (epoch > 1))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    pConfCompute = GPU_GET_CONF_COMPUTE(pGpu);
    pSpdm        = GPU_GET_SPDM(pGpu);

    if (pConfCompute == NULL || pSpdm == NULL || !pSpdm->bSessionEstablished)
    {
        return NV_ERR_INVALID_STATE;
    }

    // SPDM App Message header wraps the entire message for GSP
    pSpdmReqHdr                                  = (RM_SPDM_NV_CMD_REQ_KEYMGR_NVLE *)pNvleKeyReq;
    pSpdmReqHdr->hdr.cmdType                     = RM_SPDM_NV_CMD_TYPE_REQ_KEYMGR_NVLE;

    nvleKeyReqSize = sizeof(RM_SPDM_NV_CMD_REQ_KEYMGR_NVLE) + sizeof(RM_GSP_NVLE_REFRESH_SESSION_KEYS);
    pGspReqHdr                                   = (RM_GSP_NVLE_REFRESH_SESSION_KEYS *)
                                                   ((NvU8 *)pNvleKeyReq + sizeof(RM_SPDM_NV_CMD_REQ_KEYMGR_NVLE));

    pGspReqHdr->cmdId                                  = RM_GSP_NVLE_CMD_ID_REFRESH_SESSION_KEYS;
    pGspReqHdr->stage                                  = stage;
    pGspReqHdr->wrappedKeyEntries[0].remoteScfDcfGpuId = remoteScfDcfGpuId;
    pGspReqHdr->wrappedKeyEntries[0].bValid            = NV_TRUE;
    pGspReqHdr->wrappedKeyEntries[0].epoch             = epoch;
    portMemCopy(pGspReqHdr->wrappedKeyEntries[0].key, sizeof(pGspReqHdr->wrappedKeyEntries[0].key),
                pKey, RM_GSP_NVLE_AES_256_GCM_KEY_SIZE_BYTES);

    // Encrypt the key before sending.
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
              ccslEncrypt(pConfCompute->pNvleP2pWrappingCcslCtx,
                          sizeof(pGspReqHdr->wrappedKeyEntries),
                          (NvU8 *)pGspReqHdr->wrappedKeyEntries,
                          NULL, 0,
                          (NvU8 *)pGspReqHdr->wrappedKeyEntries,
                          (NvU8 *)pGspReqHdr->keyEntriesTag));

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, spdmSendApplicationMessage(pGpu, pSpdm, pNvleKeyReq, nvleKeyReqSize,
                                                                  (NvU8 *)&nvleKeyRsp, &nvleKeyRspSize));
    if ((nvleKeyRspSize < sizeof(RM_SPDM_NV_CMD_HDR)) || (nvleKeyRspSize > sizeof(RM_SPDM_NV_CMD_RSP)))
    {
        // Somehow, we got an entirely invalid response
        NV_PRINTF(LEVEL_ERROR, "NVLE response from GSP of invalid size! rspSize: 0x%x!\n", nvleKeyRspSize);
        return NV_ERR_INVALID_DATA;
    }

    // Check for known error response or any non-success response
    if ((nvleKeyRsp.hdr.cmdType == RM_SPDM_NV_CMD_TYPE_RSP_ERROR) &&
        (nvleKeyRspSize         == sizeof(RM_SPDM_NV_CMD_RSP_ERROR)))
    {
        NV_PRINTF(LEVEL_ERROR, "GSP returned NVLE response with error code 0x%x!\n", nvleKeyRsp.error.status);
        return (nvleKeyRsp.error.status != NV_OK) ? nvleKeyRsp.error.status : NV_ERR_INVALID_DATA;
    }
    else if (nvleKeyRsp.hdr.cmdType != RM_SPDM_NV_CMD_TYPE_RSP_SUCCESS ||
             nvleKeyRspSize         != sizeof(RM_SPDM_NV_CMD_RSP_SUCCESS))
    {
        NV_PRINTF(LEVEL_ERROR, "Unexpected NVLE response from GSP! cmdType: 0x%x rspSize: 0x%x!\n",
                  nvleKeyRsp.hdr.cmdType, nvleKeyRspSize);
        return NV_ERR_INVALID_DATA;
    }

    return NV_OK;
}

/*!
 * Sets up the intial NVLE key between a given pair of GPUs
 *
 * @param[in] pLocalGpu           : Local OBJGPU pointer
 * @param[in] pLocalKernelNvlink  : Local KernelNvlink pointer
 * @param[in] pRemoteGpu          : Remote OBJGPU pointer
 * @param[in] pRemoteKernelNvlink : Remote KernelNvlink pointer
 */
NV_STATUS
knvlinkSetupEncryptionKeys_IMPL
(
    OBJGPU       *pLocalGpu,
    KernelNvlink *pLocalKernelNvlink,
    OBJGPU       *pRemoteGpu,
    KernelNvlink *pRemoteKernelNvlink
)
{
    NvU8 nvleKey[RM_GSP_NVLE_AES_256_GCM_KEY_SIZE_BYTES] = {0};

    NV_STATUS status     = NV_OK;
    NvU32     localCLID  = knvlinkGetCLID(pLocalGpu, pLocalKernelNvlink);
    NvU32     remoteCLID = knvlinkGetCLID(pRemoteGpu, pRemoteKernelNvlink);

    if (!libspdm_random_bytes((NvU8 *)&nvleKey, sizeof(nvleKey)))
    {
        return NV_ERR_INVALID_DATA;
    }

    NV_PRINTF(LEVEL_INFO, "Setting nvle keys between GPU%d and GPU%d\n",
              gpuGetInstance(pLocalGpu), gpuGetInstance(pRemoteGpu));

    //
    // Check if NVLE keys are already programmed, skip programming the keys if they
    // are already setup once, key rotation will ensure that the keys are refreshed
    // at fixed time intervals.
    //
    if (!pLocalKernelNvlink->bNvleKeySetup[remoteCLID] &&
        !pRemoteKernelNvlink->bNvleKeySetup[localCLID])
    {
        NV_ASSERT_OK_OR_GOTO(status,
                             _knvlinkSendEncryptionKeys(pLocalGpu,  pLocalKernelNvlink,
                                                        nvleKey, remoteCLID), ErrorExit);
        NV_ASSERT_OK_OR_GOTO(status,
                             _knvlinkSendEncryptionKeys(pRemoteGpu, pRemoteKernelNvlink,
                                                        nvleKey,  localCLID), ErrorExit);

        pLocalKernelNvlink->bNvleKeySetup[remoteCLID] = NV_TRUE;
        pRemoteKernelNvlink->bNvleKeySetup[localCLID] = NV_TRUE;
    }

ErrorExit:
    // Always be sure to scrub P2P key regardless of success
    portMemSet((NvU8 *)&nvleKey, 0, sizeof(nvleKey));

    return status;
}

/*!
 * Refresh the NVLE keys between a given pair of GPUs
 *
 * @param[in] pLocalGpu           : Local OBJGPU pointer
 * @param[in] pLocalKernelNvlink  : Local KernelNvlink pointer
 * @param[in] pRemoteGpu          : Remote OBJGPU pointer
 * @param[in] pRemoteKernelNvlink : Remote KernelNvlink pointer
 * @param[in] stage               : Whether Tx or Rx keyslot needs to be refreshed
 * @param[in] epoch               : Epoch will inform which keyslot<0,1> is spare
 */
NV_STATUS
knvlinkRotateEncryptionKeys_IMPL
(
    OBJGPU                 *pLocalGpu,
    KernelNvlink           *pLocalKernelNvlink,
    OBJGPU                 *pRemoteGpu,
    KernelNvlink           *pRemoteKernelNvlink,
    sessionKeyRefreshStage  stage,
    NvU8                    epoch
)
{
    NvU8 nvleKey[RM_GSP_NVLE_AES_256_GCM_KEY_SIZE_BYTES] = {0};

    NV_STATUS status     = NV_OK;
    NvU32     localCLID  = knvlinkGetCLID(pLocalGpu, pLocalKernelNvlink);
    NvU32     remoteCLID = knvlinkGetCLID(pRemoteGpu, pRemoteKernelNvlink);

    // TODO: Check that the NVLE key setup stage has completed

    if (!libspdm_random_bytes((NvU8 *)&nvleKey, sizeof(nvleKey)))
    {
        return NV_ERR_INVALID_DATA;
    }

    // For the transfer direction pLocalGpu->pRemoteGpu, update the new key for Tx or Rx as requested
    NV_ASSERT_OK_OR_GOTO(status,
        _knvlinkRefreshEncryptionKeys(pLocalGpu,  pLocalKernelNvlink,
                                      nvleKey, remoteCLID, stage, epoch), ErrorExit);

    // For the transfer direction pRemoteGpu->pLocalGpu, update the new key for Tx or Rx as requested
    NV_ASSERT_OK_OR_GOTO(status,
        _knvlinkRefreshEncryptionKeys(pRemoteGpu, pRemoteKernelNvlink,
                                      nvleKey, localCLID,  stage, epoch), ErrorExit);

ErrorExit:
    // Always be sure to scrub NVLE key regardless of success
    portMemSet((NvU8 *)&nvleKey, 0, sizeof(nvleKey));

    return status;
}

/*!
 * Clears the NVLE key between this GPU and all remote GPUs
 *
 * @param[in] pGpu           : OBJGPU pointer
 * @param[in] pKernelNvlink  : KernelNvlink pointer
 */
NV_STATUS
knvlinkClearEncryptionKeys_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    NvU32 gpuInst;

    if (pKernelNvlink->getProperty(pKernelNvlink, PDB_PROP_KNVLINK_ENCRYPTION_ENABLED))
    {
        NvU32 localCLID = knvlinkGetCLID(pGpu, pKernelNvlink);
        for (gpuInst = 0; gpuInst < NV_MAX_DEVICES; gpuInst++)
        {
            OBJGPU *pRemoteGpu = gpumgrGetGpu(gpuInst);
            if (!API_GPU_IN_RESET_SANITY_CHECK(pRemoteGpu))
            {
                KernelNvlink *pRemoteKernelNvlink = GPU_GET_KERNEL_NVLINK(pRemoteGpu);
                if ((pRemoteKernelNvlink != NULL) &&
                    pRemoteKernelNvlink->getProperty(pRemoteKernelNvlink, PDB_PROP_KNVLINK_ENCRYPTION_ENABLED))
                {
                    NvU32 remoteCLID = knvlinkGetCLID(pRemoteGpu, pRemoteKernelNvlink);

                    pKernelNvlink->bNvleKeySetup[remoteCLID]      = NV_FALSE;
                    pRemoteKernelNvlink->bNvleKeySetup[localCLID] = NV_FALSE;
                }
            }
        }
    }

    return NV_OK;
}

NvBool
knvlinkIsNvleKeyRefreshEnabled_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    // Disable NVLE key refresh when Nvlink Encryption is disabled
    if (knvlinkIsNvleEnabled_HAL(pGpu, pKernelNvlink) == NV_FALSE)
    {
        pKernelNvlink->bNvleKeyRefreshEnabled = NV_FALSE;
    }

    return pKernelNvlink->bNvleKeyRefreshEnabled;
}

/*!
 * Validate the remap table slots between given pair of GPUs
 *
 * @param[in] pLocalGpu           : Local OBJGPU pointer
 * @param[in] pLocalKernelNvlink  : Local KernelNvlink pointer
 * @param[in] pRemoteGpu          : Remote OBJGPU pointer
 * @param[in] pRemoteKernelNvlink : Remote KernelNvlink pointer
 */
NV_STATUS
knvlinkValidateRemapTableSlots_IMPL
(
    OBJGPU       *pLocalGpu,
    KernelNvlink *pLocalKernelNvlink,
    OBJGPU       *pRemoteGpu,
    KernelNvlink *pRemoteKernelNvlink
)
{
    ConfidentialCompute *pCC = NULL;
    NvBool bCCFeatureEnabled = NV_FALSE;

    pCC               = GPU_GET_CONF_COMPUTE(pLocalGpu);
    bCCFeatureEnabled = (pCC != NULL) && pCC->getProperty(pCC, PDB_PROP_CONFCOMPUTE_ENABLED);

    // No validation needed if CC or NVLE is disabled
    if (!bCCFeatureEnabled ||
        !pLocalKernelNvlink->getProperty(pLocalKernelNvlink, PDB_PROP_KNVLINK_ENCRYPTION_ENABLED))
    {
        return NV_OK;
    }

    pCC               = GPU_GET_CONF_COMPUTE(pRemoteGpu);
    bCCFeatureEnabled = (pCC != NULL) && pCC->getProperty(pCC, PDB_PROP_CONFCOMPUTE_ENABLED);

    // No validation needed if CC or NVLE is disabled
    if (!bCCFeatureEnabled ||
        !pRemoteKernelNvlink->getProperty(pRemoteKernelNvlink, PDB_PROP_KNVLINK_ENCRYPTION_ENABLED))
    {
        return NV_OK;
    }

    // Validate the cached mapslots
    NvU32 remapTableIdx;
    for (remapTableIdx = 0; remapTableIdx < NV2080_CTRL_NVLINK_MAX_REMAP_TABLE_ENTRIES; remapTableIdx++)
    {
        if (pLocalKernelNvlink->flaRemapTabAddr[remapTableIdx] != pRemoteKernelNvlink->flaRemapTabAddr[remapTableIdx])
        {
            NV_PRINTF(LEVEL_ERROR, "FLA Remap table validation failed for table index = 0x%x\n", remapTableIdx);
            return NV_ERR_INVALID_STATE;
        }

        if (pLocalKernelNvlink->gpaRemapTabAddr[remapTableIdx] != pRemoteKernelNvlink->gpaRemapTabAddr[remapTableIdx])
        {
            NV_PRINTF(LEVEL_ERROR, "GPA Remap table validation failed for table index = 0x%x\n", remapTableIdx);
            return NV_ERR_INVALID_STATE;
        }
    }

    return NV_OK;
}

/*!
 * Update the nvlink topology with LID information for NVLE. Then validate the remap
 * table and lock it to prevent MSE from from making any nvlink config updates.
 *
 * @param[in] pGpu           : OBJGPU pointer
 * @param[in] pKernelNvlink  : KernelNvlink pointer
 */
NV_STATUS
knvlinkSetupNvleRemapTables_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    NV_STATUS     status         = NV_OK;
    OBJGPU       *pGpu0          = NULL;
    KernelNvlink *pKernelNvlink0 = NULL;
    NvU32         gpuInst0;

    OBJGPU       *pGpu1          = NULL;
    KernelNvlink *pKernelNvlink1 = NULL;
    NvU32         gpuInst1;

    // Return if NVLE is not enabled for the GPU
    if (!pKernelNvlink->getProperty(pKernelNvlink, PDB_PROP_KNVLINK_ENCRYPTION_ENABLED))
    {
        return NV_OK;
    }

    // Return if NVLE identifiers are already retrieved and remap tables are updated
    if (pKernelNvlink->bGotNvleIdentifiers)
    {
        return NV_OK;
    }

    // Return if probe response if not received on all the GPUs
    for (gpuInst0 = 0; gpuInst0 < NV_MAX_DEVICES; gpuInst0++)
    {
        pGpu0 = gpumgrGetGpu(gpuInst0);
        if (pGpu0 && (gpuIsStateLoaded(pGpu0) || gpuIsStateLoading(pGpu0)))
        {
            pKernelNvlink0 = GPU_GET_KERNEL_NVLINK(pGpu0);
            if (pKernelNvlink0 == NULL)
            {
                continue;
            }

            //
            // If one of the GPUs haven't received the probe response, return, LID programming can only
            // be done when all GPUs have received the probe response
            //
            if (pGpu0->pGpuFabricProbeInfoKernel && !gpuFabricProbeIsReceived(pGpu0->pGpuFabricProbeInfoKernel))
            {
                return NV_OK;
            }

            //
            // If MSE and remap tables are locked for one of the GPUs, then it should have been locked for all GPUs,
            // since we lock them for all the GPUs together
            //
            if (pKernelNvlink0->bRemapTableMseLocked)
            {
                return NV_OK;
            }
        }
    }

    // All the GPUs have received the probe response, parse the FLA remap table and assign the CLIDs for the ALIDs
    for (gpuInst0 = 0; gpuInst0 < NV_MAX_DEVICES; gpuInst0++)
    {
        pGpu0 = gpumgrGetGpu(gpuInst0);
        if (pGpu0 && (gpuIsStateLoaded(pGpu0) || gpuIsStateLoading(pGpu0)))
        {
            pKernelNvlink0 = GPU_GET_KERNEL_NVLINK(pGpu0);
            if (pKernelNvlink0 == NULL)
            {
                continue;
            }

            NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                                  knvlinkEncryptionGetUpdateGpuIdentifiers_HAL(pGpu0, pKernelNvlink0, NV_FALSE, NV_TRUE));
        }
    }

    // All the GPUs have received the probe response, parse the GPA remap table and assign the CLIDs for the ALIDs
    for (gpuInst0 = 0; gpuInst0 < NV_MAX_DEVICES; gpuInst0++)
    {
        pGpu0 = gpumgrGetGpu(gpuInst0);
        if (pGpu0 && (gpuIsStateLoaded(pGpu0) || gpuIsStateLoading(pGpu0)))
        {
            pKernelNvlink0 = GPU_GET_KERNEL_NVLINK(pGpu0);
            if (pKernelNvlink0 == NULL)
            {
                continue;
            }

            NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                                  knvlinkEncryptionGetUpdateGpuIdentifiers_HAL(pGpu0, pKernelNvlink0, NV_TRUE, NV_TRUE));
        }
    }

    // All the GPUs have received the probe response, update the CLIDs in the FLA remap table of all the GPUs
    for (gpuInst0 = 0; gpuInst0 < NV_MAX_DEVICES; gpuInst0++)
    {
        pGpu0 = gpumgrGetGpu(gpuInst0);
        if (pGpu0 && (gpuIsStateLoaded(pGpu0) || gpuIsStateLoading(pGpu0)))
        {
            pKernelNvlink0 = GPU_GET_KERNEL_NVLINK(pGpu0);
            if (pKernelNvlink0 == NULL)
            {
                continue;
            }

            NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                                  knvlinkEncryptionGetUpdateGpuIdentifiers_HAL(pGpu0, pKernelNvlink0, NV_FALSE, NV_FALSE));
        }
    }

    // All the GPUs have received the probe response, update the CLIDs in the GPA remap table of all the GPUs
    for (gpuInst0 = 0; gpuInst0 < NV_MAX_DEVICES; gpuInst0++)
    {
        pGpu0 = gpumgrGetGpu(gpuInst0);
        if (pGpu0 && (gpuIsStateLoaded(pGpu0) || gpuIsStateLoading(pGpu0)))
        {
            pKernelNvlink0 = GPU_GET_KERNEL_NVLINK(pGpu0);
            if (pKernelNvlink0 == NULL)
            {
                continue;
            }

            NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                                  knvlinkEncryptionGetUpdateGpuIdentifiers_HAL(pGpu0, pKernelNvlink0, NV_TRUE, NV_FALSE));

            // All NVLE identifiers are retrieved and updated by this time
            pKernelNvlink0->bGotNvleIdentifiers = NV_TRUE;
        }
    }

    // All the GPUs have retrieved and updated the ALIDs and CLIDs. Update NVLE topology information for all possible P2P pairs
    for (gpuInst0 = 0; gpuInst0 < NV_MAX_DEVICES; gpuInst0++)
    {
        pGpu0 = gpumgrGetGpu(gpuInst0);
        if (pGpu0 && (gpuIsStateLoaded(pGpu0) || gpuIsStateLoading(pGpu0)))
        {
            pKernelNvlink0 = GPU_GET_KERNEL_NVLINK(pGpu0);
            if (pKernelNvlink0 == NULL)
            {
                continue;
            }

            for (gpuInst1 = gpuInst0+1; gpuInst1 < NV_MAX_DEVICES; gpuInst1++)
            {
                pGpu1 = gpumgrGetGpu(gpuInst1);
                if (pGpu1 && (gpuIsStateLoaded(pGpu1) || gpuIsStateLoading(pGpu1)))
                {
                    pKernelNvlink1 = GPU_GET_KERNEL_NVLINK(pGpu1);
                    if (pKernelNvlink1 == NULL)
                    {
                        continue;
                    }

                    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                                          knvlinkEncryptionUpdateTopology_HAL(pGpu0, pKernelNvlink0,
                                                                              pGpu1, pKernelNvlink1));
                    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                                          knvlinkEncryptionUpdateTopology_HAL(pGpu1, pKernelNvlink1,
                                                                              pGpu0, pKernelNvlink0));
                }
            }
        }
    }

    // NVLE topology should have been updated for the GPUs. Generate and setup the NVLE keys for all the possible P2P pairs
    for (gpuInst0 = 0; gpuInst0 < NV_MAX_DEVICES; gpuInst0++)
    {
        pGpu0 = gpumgrGetGpu(gpuInst0);
        if (pGpu0 && (gpuIsStateLoaded(pGpu0) || gpuIsStateLoading(pGpu0)))
        {
            pKernelNvlink0 = GPU_GET_KERNEL_NVLINK(pGpu0);
            if (pKernelNvlink0 == NULL)
            {
                continue;
            }

            for (gpuInst1 = gpuInst0+1; gpuInst1 < NV_MAX_DEVICES; gpuInst1++)
            {
                pGpu1 = gpumgrGetGpu(gpuInst1);
                if (pGpu1 && (gpuIsStateLoaded(pGpu1) || gpuIsStateLoading(pGpu1)))
                {
                    pKernelNvlink1 = GPU_GET_KERNEL_NVLINK(pGpu1);
                    if (pKernelNvlink1 == NULL)
                    {
                        continue;
                    }

                    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                                          knvlinkSetupEncryptionKeys(pGpu0, pKernelNvlink0, pGpu1, pKernelNvlink1));
                }
            }
        }
    }

    // Validate remap table slots for all the GPUs
    for (gpuInst0 = 0; gpuInst0 < NV_MAX_DEVICES; gpuInst0++)
    {
        pGpu0 = gpumgrGetGpu(gpuInst0);
        if (pGpu0 && (gpuIsStateLoaded(pGpu0) || gpuIsStateLoading(pGpu0)))
        {
            pKernelNvlink0 = GPU_GET_KERNEL_NVLINK(pGpu0);
            if (pKernelNvlink0 == NULL)
            {
                continue;
            }

            for (gpuInst1 = gpuInst0+1; gpuInst1 < NV_MAX_DEVICES; gpuInst1++)
            {
                pGpu1 = gpumgrGetGpu(gpuInst1);
                if (pGpu1 && (gpuIsStateLoaded(pGpu1) || gpuIsStateLoading(pGpu1)))
                {
                    pKernelNvlink1 = GPU_GET_KERNEL_NVLINK(pGpu1);
                    if (pKernelNvlink1 == NULL)
                    {
                        continue;
                    }
            
                    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                                          knvlinkValidateRemapTableSlots(pGpu0, pKernelNvlink0, pGpu1, pKernelNvlink1));
                }
            }
        }
    }

    for (gpuInst0 = 0; gpuInst0 < NV_MAX_DEVICES; gpuInst0++)
    {
        pGpu0 = gpumgrGetGpu(gpuInst0);
        if (pGpu0 && (gpuIsStateLoaded(pGpu0) || gpuIsStateLoading(pGpu0)))
        {
            pKernelNvlink0 = GPU_GET_KERNEL_NVLINK(pGpu0);
            if (pKernelNvlink0 == NULL || pKernelNvlink0->bRemapTableMseLocked)
            {
                continue;
            }

            NV2080_CTRL_INTERNAL_NVLINK_LOCK_REMAP_TABLE_AND_MSE_PARAMS params;
            portMemSet(&params, 0, sizeof(params));
            params.linkMask = KNVLINK_BITVECTOR_TO_MASK(pKernelNvlink0, enabledLinks, 64);

            // Lock remap tables and MSE from making any further changes to the nvlink topology
            status = knvlinkExecGspRmRpc(pGpu0, pKernelNvlink0,
                                         NV2080_CTRL_CMD_INTERNAL_NVLINK_LOCK_REMAP_TABLE_AND_MSE,
                                         (void *)&params, sizeof(params));
            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR, "GPU%d Failed to lock remap table and MSE\n",
                          gpuGetInstance(pGpu0));
                return status;
            }

            // DEBUG_ONLY
            NV_PRINTF(LEVEL_ERROR, "GPU%d Successfully locked remap table and MSE\n",
                      gpuGetInstance(pGpu0));
            pKernelNvlink0->bRemapTableMseLocked = NV_TRUE;
        }
    }

    return NV_OK;
}

void
knvlinkUtoa(NvU8 *str, NvU64 length, NvU64 val)
{
    NvU8  temp[NV2080_GPU_MAX_NAME_STRING_LENGTH];
    NvU8 *ptr = temp;
    NvU64 i = 0;

    NV_ASSERT(str != NULL);

    do
    {
        i   = val % 10;
        val = val / 10;
        *ptr++ = (NvU8)(i + '0');
    } while(val);

    NV_ASSERT(length > (NvU64) (ptr - temp));

    while (ptr > temp)
        *str++ = *--ptr;

    *str = '\0';
}
