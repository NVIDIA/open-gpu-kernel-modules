/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "kernel/gpu/gpu.h"
#include "kernel/mem_mgr/fabric_vaspace.h"
#include "published/blackwell/gb100/dev_vm.h"
#include "compute/imex_session_api.h"
#include "compute/fabric.h"
#include "mem_mgr/mem_multicast_fabric.h"

#include "gpu/conf_compute/ccsl.h"

#include "spdm/rmspdmvendordef.h"
#include "gpu/spdm/spdm.h"
#include "kernel/gpu/spdm/libspdm_includes.h"
#include "hal/library/cryptlib.h"

#include "lib/protobuf/prb_util.h"
#include "g_nvdebug_pb.h"

static NV_STATUS _knvlinkRefreshEncryptionKeys(OBJGPU *, KernelNvlink *, NvU8 *, NvU32, NvU32, sessionKeyRefreshStage, NvU8);

// 4s timeout for inband retry
#define KNVLINK_INBAND_RETRY_TIMEOUT_US (4000U * 1000U)

static void _knvlinkTrafficQuiesceAction_WORKITEM(NvU32 gpuInstance, void *pArgs);

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

    //
    // As this check is mainly about enabling P2P between GPUs within the node,
    // using NV_FABRIC_CLIQUE_TYPE_UNICAST_POINTER check should be good enough
    //
    status = gpuFabricProbeGetFabricCliqueIdByType(pGpu->pGpuFabricProbeInfoKernel,
                                                   NV_FABRIC_CLIQUE_TYPE_UNICAST_POINTER,
                                                   &cliqueId);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_INFO, "GPU %d failed to get fabric clique Id: 0x%x\n",
                  gpuGetInstance(pGpu), status);
        return NV_FALSE;
    }

    status = gpuFabricProbeGetFabricCliqueIdByType(pPeerGpu->pGpuFabricProbeInfoKernel,
                                                   NV_FABRIC_CLIQUE_TYPE_UNICAST_POINTER,
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

        // P2P is not supported between GPUs with different RBMs on pre-Rubin.
        // Async RBM (Rubin+) supports per-GPU RBM modes with P2P.
        if (!knvlinkIsAsyncRbmEnabled(pGpu0, pKernelNvlink0))
        {
            NV_CHECK_OR_RETURN(LEVEL_INFO,
                (pKernelNvlink0->nvlinkBwMode == pKernelNvlink1->nvlinkBwMode),
                NV_ERR_INVALID_STATE);
        }

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

    if (API_GPU_IN_RESET_SANITY_CHECK(pGpu))
    {
        return NV_ERR_GPU_IN_FULLCHIP_RESET;
    }

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
    },

    {
        .messageType = NVLINK_INBAND_MSG_TYPE_GPU_PROBE_RSP_V2,
        .pCallback = gpuFabricProbeReceiveKernelCallback,
        .wqItemFlags = {.bLockSema = NV_TRUE,
                        .bLockGpuGroupSubdevice = NV_TRUE}
    },

    {
        .messageType = NVLINK_INBAND_MSG_TYPE_GPU_PROBE_UPDATE_REQ_V2,
        .pCallback = gpuFabricProbeReceiveUpdateKernelCallback,
        .wqItemFlags = {.bLockSema = NV_TRUE,
                        .bLockGpuGroupSubdevice = NV_TRUE}
    },

    {
        .messageType = NVLINK_INBAND_MSG_TYPE_GPU_GET_CURRENT_STATE_REQ,
        .pCallback = gpuFabricReceiveGpuGetCurrentStateRequestKernelCallback,
        .wqItemFlags = {.bLockSema = NV_TRUE,
                        .bLockGpuGroupSubdevice = NV_TRUE}
    },
};

NV_STATUS
knvlinkSetAmapUpdateStatus_IMPL
(
    OBJGPU *pGpu,
    KernelNvlink *pKernelNvlink,
    NvU8 readyForTraffic,
    NvU8 pendingForAbm,
    NvU8 amapRequestFailed
)
{
    // Set the AMAP update status for the GPU
    if (pKernelNvlink == NULL || readyForTraffic > 2 || pendingForAbm > 2 || amapRequestFailed > 2)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to set AMAP update status\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    pKernelNvlink->gpuAmapUpdateStatus = (readyForTraffic << 0) |
                                         (pendingForAbm << 2) |
                                         (amapRequestFailed << 4);
    return NV_OK;
}

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
    RMTIMEOUT timeout;

    gpuSetTimeout(pGpu, KNVLINK_INBAND_RETRY_TIMEOUT_US, &timeout, 0);
    do
    {
        status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                        NV2080_CTRL_CMD_NVLINK_INBAND_SEND_DATA,
                                        (void *)pParams,
                                        sizeof(*pParams));

        // Emit a log print with the error status if inband data failed
        if (status != NV_ERR_BUSY_RETRY &&
            status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Failed to send inband data: %llx\n", (NvU64)status);
            break;
        }

        status = gpuCheckTimeout(pGpu, &timeout);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Timed out retrying to send inband data\n");
            break;
        }
    } while (status == NV_ERR_BUSY_RETRY);

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
    NVLINK_BIT_VECTOR *pPeerLinkMask;
    NvU64 peerLinkMaskValue;
    NvU32 numPeerLinks = 0;

    pPeerLinkMask = knvlinkGetLinkMaskToPeer(pGpu, pKernelNvlink, pRemoteGpu);

    if (pPeerLinkMask != NULL)
    {
        peerLinkMaskValue = kNvlinkGetLinkMaskAsPrimitve(pPeerLinkMask);
        if (peerLinkMaskValue != 0)
        {
            numPeerLinks = nvPopCount64(peerLinkMaskValue);
        }
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
 * @return    Returns pointer to the bit vector of peer links between the GPUs
 */
NVLINK_BIT_VECTOR *
knvlinkGetLinkMaskToPeer_IMPL
(
    OBJGPU       *pGpu0,
    KernelNvlink *pKernelNvlink0,
    OBJGPU       *pGpu1
)
{
    KernelNvlink *pKernelNvlink1 = NULL;

    pKernelNvlink1 = GPU_GET_KERNEL_NVLINK(pGpu1);

    if (pKernelNvlink1 == NULL)
    {
        NV_PRINTF(LEVEL_INFO,
                  "on GPU%d NVLink is disabled.\n", gpuGetInstance(pGpu1));

        return NULL;
    }

    if(pKernelNvlink0->bIsGpuDegraded)
    {
        return NULL;
    }

    if(pKernelNvlink1->bIsGpuDegraded)
    {
        return NULL;
    }

    if (!knvlinkIsForcedConfig(pGpu0, pKernelNvlink0))
    {
        //
        // If nvlink topology is not forced, then the hshub registers
        // are updated only when a P2P object is allocated. So, return
        // the cached value of mask of links connected to a GPU
        //
        return &pKernelNvlink0->peerLinkMasks[gpuGetInstance(pGpu1)];
    }

    return NULL;
}

/*!
 * @brief Sets the mask of peer links between the GPUs
 *
 * @param[in] pGpu0              OBJGPU pointer
 * @param[in] pKernelNvlink0     Nvlink pointer
 * @param[in] pGpu1              Remote OBJGPU pointer
 * @param[in] pPeerLinkMaskVec   Mask of links to the peer GPU
 *
 * @return    NV_OK on success
 */
NV_STATUS
knvlinkSetLinkMaskToPeer_IMPL
(
    OBJGPU            *pGpu0,
    KernelNvlink      *pKernelNvlink0,
    OBJGPU            *pGpu1,
    NVLINK_BIT_VECTOR *pPeerLinkMaskVec
)
{
    NV_STATUS status = NV_OK;
    NvU64 peerLinkMask;

    // Extract the mask from the bit vector for GSP-RM RPC
    NV_ASSERT_OR_RETURN(bitVectorGetSlice(pPeerLinkMaskVec, rangeMake(0, 63), &peerLinkMask) == NV_OK, NV_ERR_INVALID_ARGUMENT);

    // Return early if no update needed to the peer link mask
    if (bitVectorTestEqual(&pKernelNvlink0->peerLinkMasks[gpuGetInstance(pGpu1)], pPeerLinkMaskVec))
        return NV_OK;

    bitVectorCopy(&pKernelNvlink0->peerLinkMasks[gpuGetInstance(pGpu1)], pPeerLinkMaskVec);

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
        case NVLINK_VERSION_60:
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
    OBJGPU             *pGpu,
    KernelNvlink       *pKernelNvlink,
    NVLINK_BIT_VECTOR  *pLinkMask,
    NvBool              bSync
)
{
    NV_STATUS status = NV_OK;

    NV2080_CTRL_INTERNAL_NVLINK_LINK_TRAIN_ALI_PARAMS params;

    portMemSet(&params, 0, sizeof(params));

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        convertBitVectorToLinkMasks(pLinkMask, NULL, 0, &params.links));
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
        if (i >= pKernelNvlink->maxNumLinks)
        {
            break;
        }
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
    pKernelNvlink->ioctrlMask           = pNvlinkInfoParams->ioctrlMask;
    pKernelNvlink->ioctrlNumEntries     = pNvlinkInfoParams->ioctrlNumEntries;
    pKernelNvlink->ioctrlSize           = pNvlinkInfoParams->ioctrlSize;

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            convertLinkMasksToBitVector(NULL, 0U,
                                        &pNvlinkInfoParams->discoveredLinks,
                                        &pKernelNvlink->discoveredLinks));

    pKernelNvlink->ipVerNvlink       = pNvlinkInfoParams->ipVerNvlink;
    pKernelNvlink->maxNumLinks = pNvlinkInfoParams->maxSupportedLinks;
    pKernelNvlink->probeRequestTimeMs = pNvlinkInfoParams->probeRequestTimeMs;
    pKernelNvlink->linkStateChangeTimeMs = pNvlinkInfoParams->linkStateChangeTimeMs;

    for (i = 0; i < pKernelNvlink->maxNumLinks; i++)
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
    NV_STATUS         status;
    NVLINK_BIT_VECTOR localLinkMask;

    NV2080_CTRL_NVLINK_PROCESS_INIT_DISABLED_LINKS_PARAMS params = {0};

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

NvBool
knvlinkIsP2PActive_IMPL
(
    OBJGPU *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    RmClient **ppClient;

    // Check FLA/MC FLA callers are idle
    FABRIC_VASPACE *pFabricVAS = dynamicCast(pGpu->pFabricVAS, FABRIC_VASPACE);
    if ((pFabricVAS != NULL) && fabricvaspaceIsInUse(pFabricVAS))
        return NV_TRUE;

    // For each client
    for (ppClient = serverutilGetFirstClientUnderLock(); ppClient; ppClient = serverutilGetNextClientUnderLock(ppClient))
    {
        RmClient *pClient = *ppClient;
        RsClient *pRsClient = staticCast(pClient, RsClient);
        RS_ITERATOR p2pIt;

        // For each P2P object
        p2pIt = clientRefIter(pRsClient, NULL, classId(P2PApi), RS_ITERATE_CHILDREN, NV_TRUE);
        while (clientRefIterNext(p2pIt.pClient, &p2pIt))
        {
            P2PApi *pP2PApi = dynamicCast(p2pIt.pResourceRef->pResource, P2PApi);
            if (pP2PApi == NULL)
                continue;

            // Skip ATS P2P objects as those may be allocated internally
            if (FLD_TEST_DRF(_P2PAPI, _ATTRIBUTES, _LINK_TYPE, _SPA, pP2PApi->attributes))
                continue;

            // Actve peer connection with this GPU
            if ((pP2PApi->peer1 == pGpu) || (pP2PApi->peer2 == pGpu))
                return NV_TRUE;
        }
    }

    return NV_FALSE;
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
        NVLINK_RESILIENCY_INFO *pInfo = (NVLINK_RESILIENCY_INFO *)pArgs;
        if (pInfo != NULL)
            portAtomicSetU32(&pInfo->uncontainedErrorRecovery.rcCompleted, 1);
    }
}

static void
_knvlinkResiliencyTimingLog
(
    OBJGPU *pGpu,
    KernelNvlink *pKernelNvlink,
    NVLINK_RESILIENCY_INFO *pInfo,
    NvU32 flow,
    NvU32 event,
    NV_STATUS status,
    NvU64 elapsedNs,
    NvU32 flags
)
{
    KNVLINK_RESILIENCY_TIMING_LOG_ENTRY entry = { 0 };
    NvU32 writeCount;
    NvU64 now = 0;
    OBJTMR *pTmr;

    NV_ASSERT_OR_RETURN_VOID((pGpu != NULL) && (pKernelNvlink != NULL));

    pTmr = GPU_GET_TIMER(pGpu);
    if (pTmr != NULL)
    {
        (void)tmrGetCurrentTime(pTmr, &now);
    }

    do
    {
        writeCount = portAtomicOrU32(&pKernelNvlink->resiliencyTimingWriteCount, 0U);
    } while (!portAtomicCompareAndSwapU32(&pKernelNvlink->resiliencyTimingWriteCount,
                                          writeCount + 1U,
                                          writeCount));

    entry.timestampNs   = now;
    entry.elapsedNs     = elapsedNs;
    entry.timeoutNs     = (flow == KNVLINK_RESILIENCY_TIMING_FLOW_UNCONTAINED_ERROR_RECOVERY) ?
                          pKernelNvlink->uncontainedErrorAbortTimeoutNs :
                          pKernelNvlink->trafficQuiesceAbortTimeoutNs;
    entry.seqId         = (flow == KNVLINK_RESILIENCY_TIMING_FLOW_UNCONTAINED_ERROR_RECOVERY) ?
                          portAtomicOrU32(&pKernelNvlink->uncontainedRecoverySeqId, 0U) :
                          portAtomicOrU32(&pKernelNvlink->trafficQuiesceSeqId, 0U);
    entry.flow          = flow;
    entry.event         = event;
    entry.status        = (NvU32)status;
    entry.quiesceState  = (pInfo != NULL) ? portAtomicOrU32(&pInfo->quiesceTraffic.state, 0U) : 0U;
    entry.uvmIdle       = (pInfo != NULL) ? portAtomicOrU32(&pInfo->uvmIdle, 0U) : 0U;
    entry.flags         = flags;

    // Keep the most recent events for field triage and overwrite oldest on overflow.
    (void)ringbufAppendN(&pKernelNvlink->resiliencyTimingLog, &entry, 1, NV_TRUE);

}

void
knvlinkResiliencyUvmIdle_WORKITEM
(
    NvU32 gpuInstance,
    void  *pArgs
)
{
    OBJGPU *pGpu = gpumgrGetGpu(gpuInstance);
    NVLINK_RESILIENCY_INFO *pInfo = (NVLINK_RESILIENCY_INFO *)pArgs;
    KernelNvlink *pKernelNvlink = (pGpu != NULL) ? GPU_GET_KERNEL_NVLINK(pGpu) : NULL;
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

        portAtomicSetU32(&pInfo->uvmIdle, NVLINK_RESILIENCY_INFO_UVM_IDLE_IDLE);
        if (pKernelNvlink != NULL)
        {
            _knvlinkResiliencyTimingLog(pGpu,
                                        pKernelNvlink,
                                        pInfo,
                                        (portAtomicOrU32(&pInfo->uncontainedErrorRecovery.active, 0U) != 0U) ?
                                            KNVLINK_RESILIENCY_TIMING_FLOW_UNCONTAINED_ERROR_RECOVERY :
                                            KNVLINK_RESILIENCY_TIMING_FLOW_TRAFFIC_QUIESCE,
                                        KNVLINK_RESILIENCY_TIMING_EVENT_QUIESCE_IDLE_CONFIRMED,
                                        NV_OK,
                                        0,
                                        0);
        }
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
        if (pKernelNvlink != NULL)
        {
            _knvlinkResiliencyTimingLog(pGpu,
                                        pKernelNvlink,
                                        pInfo,
                                        (portAtomicOrU32(&pInfo->uncontainedErrorRecovery.active, 0U) != 0U) ?
                                            KNVLINK_RESILIENCY_TIMING_FLOW_UNCONTAINED_ERROR_RECOVERY :
                                            KNVLINK_RESILIENCY_TIMING_FLOW_TRAFFIC_QUIESCE,
                                        KNVLINK_RESILIENCY_TIMING_EVENT_FLOW_ERROR,
                                        status,
                                        0,
                                        0);
        }
        break;
    }
}


static NV_STATUS
_knvlinkTrafficQuiesceSendLfmAction
(
    OBJGPU *pGpu,
    KernelNvlink *pKernelNvlink,
    NvU32 action
)
{
    NV2080_CTRL_INTERNAL_NVLINK_LFM_RM_ACTION_PARAMS params = { 0 };
    params.action = action;

    return knvlinkExecGspRmRpc_IMPL(pGpu, pKernelNvlink,
            NV2080_CTRL_CMD_INTERNAL_NVLINK_LFM_RM_ACTION, &params, sizeof(params));
}

static NV_STATUS
_knvlinkForceDebugStallAndUpdateAmap
(
    OBJGPU *pGpu,
    KernelNvlink *pKernelNvlink,
    NV2080_CTRL_INTERNAL_NVLINK_FORCE_DEBUG_STALL_AND_UPDATE_AMAP_PARAMS *pParams
)
{
    return knvlinkExecGspRmRpc_IMPL(pGpu, pKernelNvlink,
            NV2080_CTRL_CMD_INTERNAL_NVLINK_FORCE_DEBUG_STALL_AND_UPDATE_AMAP, pParams, sizeof(*pParams));
}

void
knvlinkResiliencyUvmResume_WORKITEM
(
    NvU32 gpuInstance,
    void  *pArgs
)
{
    OBJGPU *pGpu = gpumgrGetGpu(gpuInstance);
    NVLINK_RESILIENCY_INFO *pInfo = (NVLINK_RESILIENCY_INFO *)pArgs;
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
    NV_STATUS status;

    NV_ASSERT_OR_RETURN_VOID((pInfo != NULL) && (pGpu != NULL) && (pKernelNvlink != NULL));

    if (pKernelNvlink != NULL)
    {
        _knvlinkResiliencyTimingLog(pGpu,
                                    pKernelNvlink,
                                    pInfo,
                                    KNVLINK_RESILIENCY_TIMING_FLOW_TRAFFIC_QUIESCE,
                                    KNVLINK_RESILIENCY_TIMING_EVENT_UVM_RESUME_REQUESTED,
                                    NV_OK,
                                    0,
                                    0);
    }
    osQueueResumeP2PHandler(pInfo->uuid);

    // LFM notification path requires locks to be held; queue a workitem to finalize the resume.
    if (portAtomicOrU32(&pInfo->quiesceTraffic.bLfmResponse, 0) != 0)
    {
        status = _knvlinkTrafficQuiesceSendLfmAction(pGpu,
            pKernelNvlink, NV2080_CTRL_INTERNAL_NVLINK_LFM_RM_ACTION_RESUME_TRAFFIC_DONE);
        _knvlinkResiliencyTimingLog(pGpu,
                                    pKernelNvlink,
                                    pInfo,
                                    KNVLINK_RESILIENCY_TIMING_FLOW_TRAFFIC_QUIESCE,
                                    KNVLINK_RESILIENCY_TIMING_EVENT_RESUME_DONE_SENT_TO_LFM,
                                    status,
                                    0,
                                    0);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Failed to send RESUME_TRAFFIC_DONE to LFM status=0x%x\n", status);
        }
    }

    portAtomicSetU32(&pInfo->uvmIdle, NVLINK_RESILIENCY_INFO_UVM_IDLE_NOT_SET);
    portAtomicSetU32(&pInfo->quiesceTraffic.state, NVLINK_QUIESCE_TRAFFIC_INFO_STATE_NOT_SET);
    portAtomicSetU32(&pInfo->uncontainedErrorRecovery.active, 0);
    portAtomicSetU32(&pInfo->active, 0);
}

void
knvlinkUncontainedErrorRecoveryReadyCheck_WORKITEM
(
    OBJGPU *pGpu,
    void *pArgs
)
{
    NVLINK_RESILIENCY_INFO *pInfo = (NVLINK_RESILIENCY_INFO *)pArgs;
    OBJTMR *pTmr = GPU_GET_TIMER(pGpu);
    NvU64 currentTime;
    NV_STATUS status = NV_OK;

    NV_ASSERT_OR_RETURN_VOID(pInfo != NULL);

    NV_ASSERT_OK_OR_GOTO(status, tmrGetCurrentTime(pTmr, &currentTime), remove);
    if ((currentTime - pInfo->uncontainedErrorRecovery.startTime) <= NVLINK_UNCONTAINED_ERROR_IDLE_PERIOD_NS)
        return;

    portAtomicSetU32(&pInfo->uncontainedErrorRecovery.recoveryReady, 1);

remove:
    osRemove1HzCallback(pGpu, knvlinkUncontainedErrorRecoveryReadyCheck_WORKITEM, pArgs);
}

void
knvlinkAbortResiliencyRecovery_WORKITEM
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

typedef struct
{
    NvU32 action;
} KNVLINK_TRAFFIC_QUIESCE_ACTION_WORKITEM_INFO;

static NV_STATUS
_knvlinkTrafficQuiesceResumeDisableChannels
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvBool bEnable
)
{
    NV2080_CTRL_INTERNAL_NVLINK_RESUME_DISABLE_CHANNELS_PARAMS params = { 0 };
    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    // Error out if traffic quiesce is not requested and we are trying to resume channels
    if ((pKernelNvlink != NULL) && !knvlinkIsTrafficQuiesceRequested(pGpu, pKernelNvlink))
    {
        return NV_ERR_INVALID_STATE;
    }

    params.bEnable = bEnable;
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                       knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                            NV2080_CTRL_CMD_INTERNAL_NVLINK_RESUME_DISABLE_CHANNELS,
                                            &params,
                                            sizeof(params)));


    return NV_OK;
}

static void
knvlinkTrafficQuiesceResumeChannels_WORKITEM
(
    NvU32 gpuInstance,
    void  *pArgs
)
{
    OBJGPU *pGpu = gpumgrGetGpu(gpuInstance);
    NVLINK_RESILIENCY_INFO *pInfo = (NVLINK_RESILIENCY_INFO *)pArgs;
    KernelNvlink *pKernelNvlink;
    NV_STATUS status;

    NV_ASSERT_OR_RETURN_VOID((pInfo != NULL) && (pGpu != NULL));
    pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
    NV_ASSERT_OR_RETURN_VOID(pKernelNvlink != NULL);

    // Only attempt to resume channels if channel enable is pending
    if ((portAtomicOrU32(&pInfo->quiesceTraffic.state, 0U) &
            NVLINK_QUIESCE_TRAFFIC_INFO_STATE_RESUME_TRAFFIC_PENDING_CHANNEL_ENABLE) == 0)
    {
        return;
    }

    status = _knvlinkTrafficQuiesceResumeDisableChannels(pGpu, pKernelNvlink, NV_TRUE);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Traffic quiesce: failed to resume channels status=0x%x\n",
                  status);
        return;
    }

    portAtomicSetU32(&pInfo->quiesceTraffic.state,
                     NVLINK_QUIESCE_TRAFFIC_INFO_STATE_RESUME_TRAFFIC_PENDING_CHANNEL_ENABLE_DONE);

    _knvlinkResiliencyTimingLog(pGpu,
                                pKernelNvlink,
                                pInfo,
                                KNVLINK_RESILIENCY_TIMING_FLOW_TRAFFIC_QUIESCE,
                                KNVLINK_RESILIENCY_TIMING_EVENT_RESUME_CHANNELS_ENABLED,
                                NV_OK,
                                0,
                                0);
}

void
knvlinkResiliencyRecovery_WORKITEM
(
    OBJGPU *pGpu,
    void  *pArgs
)
{
    NVLINK_RESILIENCY_INFO *pInfo = (NVLINK_RESILIENCY_INFO *)pArgs;
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
    KernelGmmu *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
    OBJTMR *pTmr = GPU_GET_TIMER(pGpu);
    NvU64 currentTime;
    NvBool bRemove = NV_FALSE;
    NvBool bDegrade = NV_FALSE;
    NV_STATUS status = NV_OK;
    NvU64 uncontainedElapsed = 0;
    NvU64 quiesceElapsed = 0;
    NvU64 uncontainedTimeoutNs = 0;
    NvU64 quiesceTimeoutNs = 0;
    NvBool bUncontainedActive = NV_FALSE;
    NvBool bQuiesceActive = NV_FALSE;
    NV_ASSERT_OR_GOTO(pInfo != NULL, exit);
    NV_ASSERT_OR_GOTO(NV_OK == tmrGetCurrentTime(pTmr, &currentTime), exit);
    NV_ASSERT_OR_GOTO(pKernelNvlink != NULL, exit);

    bUncontainedActive = (portAtomicOrU32(&pInfo->uncontainedErrorRecovery.active, 0) != 0);
    bQuiesceActive = (portAtomicOrU32(&pInfo->quiesceTraffic.state, 0) != NVLINK_QUIESCE_TRAFFIC_INFO_STATE_NOT_SET);

    if (bUncontainedActive)
    {
        uncontainedElapsed = currentTime - pInfo->uncontainedErrorRecovery.startTime;
    }

    if (bQuiesceActive)
    {
        quiesceElapsed = currentTime - pInfo->quiesceTraffic.startTime;
    }

    uncontainedTimeoutNs = pKernelNvlink->uncontainedErrorAbortTimeoutNs;
    quiesceTimeoutNs = pKernelNvlink->trafficQuiesceAbortTimeoutNs;

    //
    // Timeout on either active sub-flow using its own start time.
    // This keeps watchdog behavior aligned with per-flow timing intent.
    //
    if ((bUncontainedActive && (uncontainedElapsed > uncontainedTimeoutNs)) ||
        (bQuiesceActive && (quiesceElapsed > quiesceTimeoutNs)))
    {

        NV_PRINTF(LEVEL_ERROR,
                  "Resiliency timeout: uc_active=%u uc_elapsed_ns=%llu tq_active=%u tq_elapsed_ns=%llu\n",
                  bUncontainedActive,
                  uncontainedElapsed,
                  bQuiesceActive,
                  quiesceElapsed);
        if (bUncontainedActive)
        {
            _knvlinkResiliencyTimingLog(pGpu,
                                        pKernelNvlink,
                                        pInfo,
                                        KNVLINK_RESILIENCY_TIMING_FLOW_UNCONTAINED_ERROR_RECOVERY,
                                        KNVLINK_RESILIENCY_TIMING_EVENT_FLOW_TIMEOUT,
                                        NV_ERR_TIMEOUT,
                                        uncontainedElapsed,
                                        0);
        }
        if (bQuiesceActive)
        {
            _knvlinkResiliencyTimingLog(pGpu,
                                        pKernelNvlink,
                                        pInfo,
                                        KNVLINK_RESILIENCY_TIMING_FLOW_TRAFFIC_QUIESCE,
                                        KNVLINK_RESILIENCY_TIMING_EVENT_FLOW_TIMEOUT,
                                        NV_ERR_TIMEOUT,
                                        quiesceElapsed,
                                        0);
        }
        bRemove = NV_TRUE;
        bDegrade = NV_TRUE;
        // One more pass in case it just took a long time to get scheduled
    }

    // If UVM is not idle goto exit and reschedule the workitem
    if ((portAtomicOrU32(&pInfo->uvmIdle, 0) & NVLINK_RESILIENCY_INFO_UVM_IDLE_IDLE) == 0)
        goto exit;

    // Check Uncontained Error Recovery conditions are met
    if (portAtomicOrU32(&pInfo->uncontainedErrorRecovery.active, 0) == 1)
    {
        if (portAtomicOrU32(&pInfo->uncontainedErrorRecovery.recoveryReady, 0) == 0)
            goto exit;
        if (portAtomicOrU32(&pInfo->uncontainedErrorRecovery.rcCompleted, 0) == 0)
            goto exit;
    }

    // Check traffic quiesce conditions are met
    if (portAtomicOrU32(&pInfo->quiesceTraffic.state, 0) !=  NVLINK_QUIESCE_TRAFFIC_INFO_STATE_NOT_SET)
    {
        switch (portAtomicOrU32(&pInfo->quiesceTraffic.state, 0))
        {
            case NVLINK_QUIESCE_TRAFFIC_INFO_STATE_QUIESCE_TRAFFIC_PENDING:
            {

                // If not in the resiliency flow then quiesce traffic is done otherwise pending LFM action
                if (portAtomicOrU32(&pInfo->quiesceTraffic.bLfmResponse, 0) == 0)
                {
                     NVLINK_BIT_VECTOR peerLinkVec = { 0 };
                    //
                    // Send a TLB invalidation membar to ensure all nvlink traffic is quiesced before resuming
                    // wait for 3 seconds for the TLB invalidation to complete (~3x worst case STO)
                    //
                    TLB_INVALIDATE_PARAMS   tlbInvalidateParams = { 0 };
                    gpuSetTimeout(pGpu, 3000000, &tlbInvalidateParams.timeout, GPU_TIMEOUT_FLAGS_DEFAULT);

                    // Ensure any pending TLB invalidates are completed
                    status = kgmmuCheckPendingInvalidates_HAL(pGpu, pKernelGmmu, &tlbInvalidateParams.timeout);
                    if (status != NV_OK)
                    {
                        NV_PRINTF(LEVEL_ERROR, "Failed to check pending TLB invalidates status=0x%x\n", status);
                        goto exit;
                    }

                    // Get the sysmembar register value
                    tlbInvalidateParams.regVal = DRF_DEF(_VIRTUAL_FUNCTION_PRIV, _MMU_INVALIDATE, _ALL_VA, _TRUE) |
                                DRF_DEF(_VIRTUAL_FUNCTION_PRIV, _MMU_INVALIDATE, _ALL_PDB, _TRUE) |
                                DRF_DEF(_VIRTUAL_FUNCTION_PRIV, _MMU_INVALIDATE, _ACK, _GLOBALLY) |
                                DRF_DEF(_VIRTUAL_FUNCTION_PRIV, _MMU_INVALIDATE, _SYS_MEMBAR, _TRUE) |
                                DRF_DEF(_VIRTUAL_FUNCTION_PRIV, _MMU_INVALIDATE, _TRIGGER, _TRUE);

                    // Commit the TLB invalidation
                    status = kgmmuCommitTlbInvalidate_HAL(pGpu, pKernelGmmu, &tlbInvalidateParams);
                    if (status != NV_OK)
                    {
                        NV_PRINTF(LEVEL_ERROR, "Failed to commit TLB invalidation status=0x%x\n", status);
                        goto exit;
                    }

                    NV2080_CTRL_INTERNAL_NVLINK_FORCE_DEBUG_STALL_AND_UPDATE_AMAP_PARAMS params = { 0 };
                    gpuFabricProbeSetlinkMaskToBeReduced(pGpu->pGpuFabricProbeInfoKernel, &pKernelNvlink->pendingAbmLinkMaskToBeReduced);

                    // Copy the peer link mask to the pending ABM link mask to be reduced
                    bitVectorCopy(&peerLinkVec, &pKernelNvlink->pendingAbmLinkMaskToBeReduced);
                    bitVectorInvAll(&peerLinkVec);
                    bitVectorAnd(&peerLinkVec, &peerLinkVec, &pKernelNvlink->enabledLinks);

                    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
                        convertBitVectorToLinkMasks(&peerLinkVec, NULL, 0U, &params.peerLinkMask),
                            exit);
                    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR, _knvlinkForceDebugStallAndUpdateAmap(pGpu, pKernelNvlink, &params), exit);

                    portAtomicSetU32(&pInfo->quiesceTraffic.state,
                        NVLINK_QUIESCE_TRAFFIC_INFO_STATE_RESUME_TRAFFIC_PENDING_CHANNEL_ENABLE);

                }
                else
                {
                    status = _knvlinkTrafficQuiesceSendLfmAction(pGpu,
                                                                 pKernelNvlink,
                                                                 NV2080_CTRL_INTERNAL_NVLINK_LFM_RM_ACTION_QUIESCE_TRAFFIC_DONE);
                    _knvlinkResiliencyTimingLog(pGpu,
                                                pKernelNvlink,
                                                pInfo,
                                                KNVLINK_RESILIENCY_TIMING_FLOW_TRAFFIC_QUIESCE,
                                                KNVLINK_RESILIENCY_TIMING_EVENT_QUIESCE_DONE_SENT_TO_LFM,
                                                status,
                                                quiesceElapsed,
                                                0);
                    NV_ASSERT_OK_OR_GOTO(status, status, exit);

                    portAtomicSetU32(&pInfo->quiesceTraffic.state,
                        NVLINK_QUIESCE_TRAFFIC_INFO_STATE_QUIESCE_TRAFFIC_PENDING_LFM_ACTION);
                }
                goto exit;
            }
            case NVLINK_QUIESCE_TRAFFIC_INFO_STATE_QUIESCE_TRAFFIC_PENDING_LFM_ACTION:
            {
                // Waiting for LFM to tell us to resume traffic
                goto exit;
            }
            case NVLINK_QUIESCE_TRAFFIC_INFO_STATE_RESUME_TRAFFIC_PENDING_CHANNEL_ENABLE:
            {
                // Channel Resume requires API and GPU locks to be held, queue a workitem to handle this
                NV_ASSERT_OK_OR_GOTO(status,
                    osQueueWorkItem(pGpu,
                                    knvlinkTrafficQuiesceResumeChannels_WORKITEM,
                                    pInfo,
                                    (OsQueueWorkItemFlags){
                                        .bLockSema = NV_TRUE,
                                        .apiLock = WORKITEM_FLAGS_API_LOCK_READ_WRITE,
                                        .bLockGpus = NV_TRUE,
                                        .bDontFreeParams = NV_TRUE}),
                    exit);
                goto exit;
            }
            case NVLINK_QUIESCE_TRAFFIC_INFO_STATE_RESUME_TRAFFIC_PENDING_CHANNEL_ENABLE_DONE:
            {
                portAtomicSetU32(&pInfo->quiesceTraffic.state,
                    NVLINK_QUIESCE_TRAFFIC_INFO_STATE_RESUME_TRAFFIC_PENDING_UVM_RESUME);

                // UVM resumeP2P workitem is launched below
                break;
            }
            default:
            {
                // Invalid state, should not happen
                status = NV_ERR_INVALID_STATE;
                goto exit;
            }
        }
    }

    // If we reach here all resiliency recovery checks are complete and we can remove this work item
    bRemove = NV_TRUE;
    bDegrade = NV_FALSE;

    // Launch recovery action in the HW to allow new traffic
    if (bUncontainedActive)
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

    // Launch lockless workitem to resume P2P in UVM.
    NV_ASSERT_OK_OR_GOTO(status,
        osQueueWorkItem(pGpu,
                        knvlinkResiliencyUvmResume_WORKITEM,
                        pInfo,
                        (OsQueueWorkItemFlags){.bDontFreeParams = NV_TRUE}),
        exit);

    if (bUncontainedActive)
    {
        _knvlinkResiliencyTimingLog(pGpu,
                                    pKernelNvlink,
                                    pInfo,
                                    KNVLINK_RESILIENCY_TIMING_FLOW_UNCONTAINED_ERROR_RECOVERY,
                                    KNVLINK_RESILIENCY_TIMING_EVENT_FLOW_SUCCESS,
                                    NV_OK,
                                    uncontainedElapsed,
                                    0);
    }
    if (bQuiesceActive)
    {
        _knvlinkResiliencyTimingLog(pGpu,
                                    pKernelNvlink,
                                    pInfo,
                                    KNVLINK_RESILIENCY_TIMING_FLOW_TRAFFIC_QUIESCE,
                                    KNVLINK_RESILIENCY_TIMING_EVENT_FLOW_SUCCESS,
                                    NV_OK,
                                    quiesceElapsed,
                                    0);

        knvlinkSetAmapUpdateStatus(pGpu, pKernelNvlink,
            NVLINK_INBAND_GPU_GET_CURRENT_STATE_GPU_STATE_FLAGS_READY_FOR_TRAFFIC_TRUE,
            NVLINK_INBAND_GPU_GET_CURRENT_STATE_GPU_STATE_FLAGS_AMAP_UPDATE_PENDING_FALSE,
            NVLINK_INBAND_GPU_GET_CURRENT_STATE_GPU_STATE_FLAGS_AMAP_UPDATE_FAILED_FALSE);
    }

exit:
    if (status != NV_OK)
    {
        bRemove = NV_TRUE;
        bDegrade = NV_TRUE;
    }

    if (bRemove)
        osRemove1HzCallback(pGpu, knvlinkResiliencyRecovery_WORKITEM, pArgs);

    if (bDegrade)
    {
        if (bUncontainedActive)
        {
            _knvlinkResiliencyTimingLog(pGpu,
                                        pKernelNvlink,
                                        pInfo,
                                        KNVLINK_RESILIENCY_TIMING_FLOW_UNCONTAINED_ERROR_RECOVERY,
                                        KNVLINK_RESILIENCY_TIMING_EVENT_FLOW_ABORT,
                                        status, 
                                        uncontainedElapsed,
                                        0);
        }

        if (bQuiesceActive)
        {
            _knvlinkResiliencyTimingLog(pGpu,
                                        pKernelNvlink,
                                        pInfo,
                                        KNVLINK_RESILIENCY_TIMING_FLOW_TRAFFIC_QUIESCE,
                                        KNVLINK_RESILIENCY_TIMING_EVENT_FLOW_ABORT,
                                        status,
                                        quiesceElapsed,
                                        0);

            knvlinkSetAmapUpdateStatus(pGpu, pKernelNvlink,
                NVLINK_INBAND_GPU_GET_CURRENT_STATE_GPU_STATE_FLAGS_READY_FOR_TRAFFIC_FALSE,
                NVLINK_INBAND_GPU_GET_CURRENT_STATE_GPU_STATE_FLAGS_AMAP_UPDATE_PENDING_FALSE,
                NVLINK_INBAND_GPU_GET_CURRENT_STATE_GPU_STATE_FLAGS_AMAP_UPDATE_FAILED_TRUE);
        }
        NV_ASSERT_OK_OR_CAPTURE_FIRST_ERROR(status,
            osQueueWorkItem(pGpu,
                knvlinkAbortResiliencyRecovery_WORKITEM,
                NULL,
                (OsQueueWorkItemFlags){
                    .bLockSema = NV_TRUE,
                    .apiLock = WORKITEM_FLAGS_API_LOCK_READ_WRITE,
                    .bLockGpuGroupSubdevice = NV_TRUE}));
    }
}

NV_STATUS 
knvlinkLfmQuiesceRetryTimerCallback
(
    OBJGPU *pGpu,
    OBJTMR *pTmr,
    TMR_EVENT *pEvent
)
{
    KNVLINK_TRAFFIC_QUIESCE_ACTION_WORKITEM_INFO *pInfo = (KNVLINK_TRAFFIC_QUIESCE_ACTION_WORKITEM_INFO *)pEvent->pUserData;
    NVLINK_RESILIENCY_INFO *pResiliencyInfo;
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
    NV_STATUS status = NV_OK;

    pResiliencyInfo = gpumgrGetNvlinkResiliencyInfo(gpuGetDBDF(pGpu));
    if (pResiliencyInfo == NULL)
    {
        portMemFree(pInfo);
        return NV_ERR_INVALID_STATE;
    }

    if (pKernelNvlink == NULL || pInfo == NULL)
    {
        portMemFree(pInfo);
        portAtomicSetU32(&pResiliencyInfo->bPendingLfmTrafficQuiesce, 0);
        return NV_ERR_INVALID_STATE;
    }

    // Queue the workitem again (bDontFreeParams: workitem owns pInfo across retries)
    status = osQueueWorkItem(pGpu,
                    _knvlinkTrafficQuiesceAction_WORKITEM,
                    pInfo,
                    (OsQueueWorkItemFlags){
                        .bLockSema = NV_TRUE,
                        .apiLock = WORKITEM_FLAGS_API_LOCK_READ_WRITE,
                        .bLockGpus = NV_TRUE,
                        .bDontFreeParams = NV_TRUE});
                        
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to queue NVLINK traffic quiesce action 0x%x workitem status=0x%x\n", pInfo->action, status);
        portMemFree(pInfo);
        portAtomicSetU32(&pResiliencyInfo->bPendingLfmTrafficQuiesce, 0);
        pEvent->pUserData = NULL;
    }

    return status;
}

static void
_knvlinkTrafficQuiesceAction_WORKITEM
(
    NvU32 gpuInstance,
    void *pArgs
)
{
    KNVLINK_TRAFFIC_QUIESCE_ACTION_WORKITEM_INFO *pInfo = pArgs;
    OBJGPU *pGpu = gpumgrGetGpu(gpuInstance);
    KernelNvlink *pKernelNvlink;
    NV_STATUS status = NV_OK;
    NVLINK_RESILIENCY_INFO *pResiliencyInfo;

    if ((pInfo == NULL) || (pGpu == NULL))
    {
        portMemFree(pInfo);
        return;
    }

    pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
    pResiliencyInfo = gpumgrGetNvlinkResiliencyInfo(gpuGetDBDF(pGpu));
    if ((pKernelNvlink == NULL) || (pResiliencyInfo == NULL))
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to get kernel NVLINK or resiliency info\n");
        portMemFree(pInfo);
        return;
    }

    switch (pInfo->action)
    {
        case NV2080_CTRL_INTERNAL_NVLINK_LFM_RM_ACTION_QUIESCE_TRAFFIC:
        {
            status = knvlinkResiliencyEntryFunction(pGpu, pKernelNvlink, pResiliencyInfo, NV_FALSE, NV_TRUE);
            // If the workitem is busy, schedule a timer event to retry
            if (pResiliencyInfo->pLfmRetryEvent != NULL && 
                status == NV_ERR_BUSY_RETRY &&
                pResiliencyInfo->lfmQuiesceRetryCount < KNVLINK_TRAFFIC_QUIESCE_ACTION_RETRY_COUNT_MAX)
            {
                NV_PRINTF(LEVEL_INFO, "Existing traffic quiesce flow is running, scheduling timer event to retry the LFM action %d\n", pInfo->action);
                pResiliencyInfo->lfmQuiesceRetryCount++;
                pResiliencyInfo->pLfmRetryEvent->pUserData = pInfo;
                status = tmrEventScheduleRelSec(GPU_GET_TIMER(pGpu), pResiliencyInfo->pLfmRetryEvent, 1U);
                if (status != NV_OK)
                {
                    NV_PRINTF(LEVEL_ERROR, "Failed to schedule timer event to retry the LFM action %d status=0x%x\n", pInfo->action, status);
                    goto cleanup;
                }
                return;
            }
cleanup:
            portAtomicSetU32(&pResiliencyInfo->bPendingLfmTrafficQuiesce, 0);
            pResiliencyInfo->lfmQuiesceRetryCount = 0;
            // if the retry event is not NULL, clear the user data
            if (pResiliencyInfo->pLfmRetryEvent != NULL)
            {
                pResiliencyInfo->pLfmRetryEvent->pUserData = NULL;
            }
            break;
        }
        case NV2080_CTRL_INTERNAL_NVLINK_LFM_RM_ACTION_RESUME_TRAFFIC:
        {
            NV_ASSERT(portAtomicOrU32(&pResiliencyInfo->quiesceTraffic.state, 0) ==
                            NVLINK_QUIESCE_TRAFFIC_INFO_STATE_QUIESCE_TRAFFIC_PENDING_LFM_ACTION);
            portAtomicSetU32(&pResiliencyInfo->quiesceTraffic.state,
                NVLINK_QUIESCE_TRAFFIC_INFO_STATE_RESUME_TRAFFIC_PENDING_CHANNEL_ENABLE);
            _knvlinkResiliencyTimingLog(pGpu,
                                        pKernelNvlink,
                                        pResiliencyInfo,
                                        KNVLINK_RESILIENCY_TIMING_FLOW_TRAFFIC_QUIESCE,
                                        KNVLINK_RESILIENCY_TIMING_EVENT_RESUME_REQUEST_FROM_LFM,
                                        NV_OK,
                                        0,
                                        0);
            break;
        }
        default:
            NV_PRINTF(LEVEL_ERROR, "Invalid NVLINK traffic quiesce action 0x%x\n", pInfo->action);
            portMemFree(pInfo);
            return;
    }


    // If the workitem failed, log an error and return
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed NVLINK traffic quiesce action 0x%x status=0x%x after %d retries (max %d retries)\n",
                  pInfo->action,
                  status,
                  pResiliencyInfo->lfmQuiesceRetryCount,
                  KNVLINK_TRAFFIC_QUIESCE_ACTION_RETRY_COUNT_MAX);
    }

    portMemFree(pInfo);
}

NV_STATUS
knvlinkTrafficQuiesceAction_IMPL
(
    OBJGPU *pGpu,
    KernelNvlink *pKernelNvlink,
    NvU32 action
)
{
    KNVLINK_TRAFFIC_QUIESCE_ACTION_WORKITEM_INFO *pInfo;
    NVLINK_RESILIENCY_INFO *pResiliencyInfo = gpumgrGetNvlinkResiliencyInfo(gpuGetDBDF(pGpu));
    NV_STATUS status = NV_OK;

    if (pResiliencyInfo == NULL)
    {
        return NV_ERR_INVALID_STATE;
    }

    switch (action)
    {
        case NV2080_CTRL_INTERNAL_NVLINK_LFM_RM_ACTION_QUIESCE_TRAFFIC:
        {
            if (portAtomicCompareAndSwapU32(&pResiliencyInfo->bPendingLfmTrafficQuiesce, 1, 0) == 0)
            {
                NV_PRINTF(LEVEL_INFO, "NVLINK traffic quiesce action %d is already in progress, skipping\n", action);
                return NV_ERR_INVALID_STATE;
            }
            // Fall through
        }
        case NV2080_CTRL_INTERNAL_NVLINK_LFM_RM_ACTION_RESUME_TRAFFIC:
        {
            pInfo = portMemAllocNonPaged(sizeof(*pInfo));
            if (pInfo == NULL)
            {
                portAtomicSetU32(&pResiliencyInfo->bPendingLfmTrafficQuiesce, 0);
                return NV_ERR_NO_MEMORY;
            }
            pInfo->action = action;

            status = osQueueWorkItem(pGpu,
                                     _knvlinkTrafficQuiesceAction_WORKITEM,
                                     pInfo,
                                     (OsQueueWorkItemFlags){
                                         .bLockSema = NV_TRUE,
                                         .apiLock = WORKITEM_FLAGS_API_LOCK_READ_WRITE,
                                         .bLockGpus = NV_TRUE,
                                         .bDontFreeParams = NV_TRUE});
            if (status != NV_OK)
            {
                portMemFree(pInfo);
                portAtomicSetU32(&pResiliencyInfo->bPendingLfmTrafficQuiesce, 0);
                return status;
            }
            break;
        }
        default:
            NV_PRINTF(LEVEL_ERROR, "Invalid NVLINK traffic quiesce action 0x%x\n", action);
            return NV_ERR_INVALID_ARGUMENT;
    }

    return NV_OK;
}

NV_STATUS
knvlinkResiliencyEntryFunction_IMPL
(
    OBJGPU *pGpu,
    KernelNvlink *pKernelNvlink,
    NVLINK_RESILIENCY_INFO *pInfo,
    NvBool bUncontainedErrorRecovery,
    NvBool bTrafficQuiesceResiliencyFlow
)
{
    NV_STATUS status = NV_OK;
    OBJTMR *pTmr = GPU_GET_TIMER(pGpu);
    NvBool bEnableRunningChannels = NV_FALSE;
    NvBool bResiliencyAlreadyActive = NV_FALSE;

    //
    // Validate the input parameters
    // Uncontained Recovery and Traffic Quiesce Resiliency Flow cannot be set simultaneously
    //
    if ((pInfo == NULL) ||
        (!pInfo->bValid) ||
        (!!bUncontainedErrorRecovery && !!bTrafficQuiesceResiliencyFlow))
    {
        status = NV_ERR_INVALID_STATE;
        goto fail;
    }

    NvU32 flags = DRF_DEF(2080_GPU_CMD, _GPU_GET_GID_FLAGS, _TYPE, _SHA1) |
        DRF_DEF(2080_GPU_CMD, _GPU_GET_GID_FLAGS, _FORMAT, _BINARY);
    NvU32 uuidLength;
    NvU8 *pUuid;

    // allocates memory for pUuid on success
    NV_ASSERT_OK_OR_GOTO(status, gpuGetGidInfo(pGpu, &pUuid, &uuidLength, flags), fail);
    if (uuidLength != sizeof(pInfo->uuid))
    {
        portMemFree(pUuid);
        status = NV_ERR_INVALID_STATE;
        goto fail;
    }

    // If the resiliency info is not active, then set the UUID
    if (portAtomicOrU32(&pInfo->active, 0) == 0)
    {
        portMemCopy(pInfo->uuid, uuidLength, (void *)pUuid, uuidLength);
        portMemFree(pUuid);
    }
    // else if the UUID has changed, then assert failed
    else if (portMemCmp(pInfo->uuid, pUuid, uuidLength) != 0)
    {
        NV_ASSERT_FAILED("NVLINK Resiliency re-triggered unexpectedly with different UUIDs!");
        status = NV_ERR_INVALID_STATE;
        portMemFree(pUuid);
        goto fail;
    }
    else
    {
        // do nothing
        portMemFree(pUuid);
    }

    bResiliencyAlreadyActive = (portAtomicOrU32(&pInfo->active, 0) != 0);

    // Track if either sub-flow is active.
    portAtomicSetU32(&pInfo->active, 1);
    _knvlinkResiliencyTimingLog(pGpu,
                                pKernelNvlink,
                                pInfo,
                                bUncontainedErrorRecovery ?
                                    KNVLINK_RESILIENCY_TIMING_FLOW_UNCONTAINED_ERROR_RECOVERY :
                                    KNVLINK_RESILIENCY_TIMING_FLOW_TRAFFIC_QUIESCE,
                                KNVLINK_RESILIENCY_TIMING_EVENT_FLOW_ENTRY,
                                NV_OK,
                                0,
                                0);

    // Set up the state for uncontained error recovery or traffic quiesce
    if (bUncontainedErrorRecovery)
    {
        // This recovery process should not be able to occur twice synchronously
        if (portAtomicOrU32(&pInfo->uncontainedErrorRecovery.active, 0) != 0)
        {
            NV_ASSERT_FAILED("NVLINK Uncontained Error Recovery re-triggered unexpectedly!");
            status = NV_ERR_INVALID_STATE;
            goto fail;
        }

        // Set up tracking set for the uncontained error recovery
        portAtomicSetU32(&pInfo->uncontainedErrorRecovery.active, 1);
        portAtomicSetU32(&pInfo->uncontainedErrorRecovery.rcCompleted, 0);
        portAtomicSetU32(&pInfo->uncontainedErrorRecovery.recoveryReady, 0);
        NV_ASSERT_OK_OR_GOTO(status, tmrGetCurrentTime(pTmr, &pInfo->uncontainedErrorRecovery.startTime), fail);

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

        // Launch repeated 1Hz workitem to wait 1 STO period
        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
            osSchedule1HzCallback(pGpu, knvlinkUncontainedErrorRecoveryReadyCheck_WORKITEM, pInfo, NV_OS_1HZ_REPEAT),
            fail);
    }
    else
    {
        // Check if traffic quiesce is supported
        if (!knvlinkIsTrafficQuiesceRequested(pGpu, pKernelNvlink))
        {
            NV_PRINTF(LEVEL_ERROR, "Traffic quiesce requested but not enabled!\n");
            status = NV_ERR_INVALID_STATE;
            goto fail;
        }

        // This recovery process should not be able to occur twice synchronously
        if (portAtomicOrU32(&pInfo->quiesceTraffic.state, 0) != 0)
        {
            NV_PRINTF(LEVEL_INFO, "NVLINK Traffic Quiesce re-triggered unexpectedly! (request_flow=%u) (active_flow=%u)\n",
                bTrafficQuiesceResiliencyFlow, portAtomicOrU32(&pInfo->quiesceTraffic.bLfmResponse, 0));
            status = NV_ERR_BUSY_RETRY;
            goto fail;
        }

        // set up the tracking state for traffic quiesce
        portAtomicSetU32(&pInfo->quiesceTraffic.bLfmResponse, bTrafficQuiesceResiliencyFlow ? 1 : 0);
        portAtomicSetU32(&pInfo->quiesceTraffic.state, NVLINK_QUIESCE_TRAFFIC_INFO_STATE_QUIESCE_TRAFFIC_PENDING);
        NV_ASSERT_OK_OR_GOTO(status, tmrGetCurrentTime(pTmr, &pInfo->quiesceTraffic.startTime), fail);
        NV_PRINTF(LEVEL_INFO,
                  "Traffic quiesce entry: resiliency_flow=%u start_ns=%llu\n",
                  bTrafficQuiesceResiliencyFlow ? 1 : 0,
                  pInfo->quiesceTraffic.startTime);


        // disable running channels
        status = _knvlinkTrafficQuiesceResumeDisableChannels(pGpu, pKernelNvlink, NV_FALSE);
        if (status != NV_OK)
        {
            goto fail;
        }

        // set flag to indicate such if we hit failures we attempt to re-enable running channels
        bEnableRunningChannels = NV_TRUE;
    }

    // If UVM idle workitem has not been launched, then launch it
    {
        NvBool bNotLaunched = portAtomicCompareAndSwapU32(&pInfo->uvmIdle,
            NVLINK_RESILIENCY_INFO_UVM_IDLE_DRAINP2P_WORKITEM_LAUNCHED,
            NVLINK_RESILIENCY_INFO_UVM_IDLE_NOT_SET);

        if (bNotLaunched)
        {
            // Launch lockless workitem to idle UVM channels
            NV_CHECK_OK_OR_GOTO(status,
                LEVEL_ERROR,
                osQueueWorkItem(pGpu,
                            knvlinkResiliencyUvmIdle_WORKITEM,
                            pInfo,
                            (OsQueueWorkItemFlags){.bDontFreeParams = NV_TRUE}),
                fail);
            _knvlinkResiliencyTimingLog(pGpu,
                                        pKernelNvlink,
                                        pInfo,
                                        bUncontainedErrorRecovery ?
                                            KNVLINK_RESILIENCY_TIMING_FLOW_UNCONTAINED_ERROR_RECOVERY :
                                            KNVLINK_RESILIENCY_TIMING_FLOW_TRAFFIC_QUIESCE,
                                        KNVLINK_RESILIENCY_TIMING_EVENT_WAITING_UVM_IDLE,
                                        NV_OK,
                                        0,
                                        0);
        }
    }

    // Launch repeated 1Hz workitem to await completion of recovery steps
    if (!bResiliencyAlreadyActive)
    {
        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
            osSchedule1HzCallback(pGpu, knvlinkResiliencyRecovery_WORKITEM, pInfo, NV_OS_1HZ_REPEAT),
            fail);
    }
    else
    {
        NV_PRINTF(LEVEL_INFO,
                  "Resiliency recovery already active; piggybacking sub-flow "
                  "(uncontained=%u traffic_quiesce=%u)\n",
                  bUncontainedErrorRecovery ? 1 : 0,
                  bTrafficQuiesceResiliencyFlow ? 1 : 0);
    }

    return NV_OK;
fail:


   // Try to clean-up the best we can if traffic quiesce is active and we hit an error
   if (bEnableRunningChannels && !bUncontainedErrorRecovery && !bResiliencyAlreadyActive)
   {
        knvlinkSetAmapUpdateStatus(pGpu, pKernelNvlink,
            NVLINK_INBAND_GPU_GET_CURRENT_STATE_GPU_STATE_FLAGS_READY_FOR_TRAFFIC_FALSE,
            NVLINK_INBAND_GPU_GET_CURRENT_STATE_GPU_STATE_FLAGS_AMAP_UPDATE_PENDING_FALSE,
            NVLINK_INBAND_GPU_GET_CURRENT_STATE_GPU_STATE_FLAGS_AMAP_UPDATE_FAILED_TRUE);

        _knvlinkTrafficQuiesceResumeDisableChannels(pGpu, pKernelNvlink, NV_TRUE);
   }

   return status;
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
    NVLINK_RESILIENCY_INFO *pInfo = gpumgrGetNvlinkResiliencyInfo(gpuGetDBDF(pGpu));

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
        // Kickoff resiliency flows for uncontained error recovery
        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
            knvlinkResiliencyEntryFunction(pGpu, pKernelNvlink, pInfo, NV_TRUE, NV_FALSE),
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
                        knvlinkAbortResiliencyRecovery_WORKITEM,
                        NULL,
                        (OsQueueWorkItemFlags){
                            .bLockSema = NV_TRUE,
                            .apiLock = WORKITEM_FLAGS_API_LOCK_READ_WRITE,
                            .bLockGpuGroupSubdevice = NV_TRUE}));

    return status;
}

static void
_knvlinkGfmErrorOutMcRequests
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    memorymulticastfabricQueueErrorAllInFlightRequests(pGpu);
}

const static KNVLINK_GFM_STATE_ACTION knvlink_gfm_state_actions[] =
{
    {
        .gfmState = KNVLINK_GFM_STATE_DISCONNECTED,
        .pAction  = _knvlinkGfmErrorOutMcRequests,
    },
};

void
knvlinkHandleGfmStateChange_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvU32         gfmState
)
{
    NvU32 i;

    ct_assert(KNVLINK_GFM_STATE_CONNECTED ==
              NV2080_CTRL_GPU_FABRIC_HEALTH_MASK_GFM_STATE_CONNECTED);
    ct_assert(KNVLINK_GFM_STATE_DISCONNECTED ==
              NV2080_CTRL_GPU_FABRIC_HEALTH_MASK_GFM_STATE_DISCONNECTED);

    switch (gfmState)
    {
        case KNVLINK_GFM_STATE_CONNECTED:
            pGpu->gfmState =
                NV2080_CTRL_GPU_FABRIC_HEALTH_MASK_GFM_STATE_CONNECTED;
            break;

        case KNVLINK_GFM_STATE_DISCONNECTED:
            pGpu->gfmState =
                NV2080_CTRL_GPU_FABRIC_HEALTH_MASK_GFM_STATE_DISCONNECTED;
            break;

        default:
            NV_PRINTF(LEVEL_WARNING, "Unknown GFM state 0x%x\n", gfmState);
            return;
    }

    for (i = 0; i < NV_ARRAY_ELEMENTS(knvlink_gfm_state_actions); i++)
    {
        if (knvlink_gfm_state_actions[i].gfmState == gfmState)
        {
            knvlink_gfm_state_actions[i].pAction(pGpu, pKernelNvlink);
        }
    }
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
 * @param[in] remoteGpuClid     : Remote CLID
 * @param[in] remoteGpuAlid     : Remote ALID
 */
NV_STATUS
knvlinkSendEncryptionKeys_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvU8         *pKey,
    NvU32         remoteGpuClid,
    NvU32         remoteGpuAlid
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
    pGspReqHdr->wrappedKeyEntries[0].remoteGpuClid     = remoteGpuClid;
    pGspReqHdr->wrappedKeyEntries[0].remoteGpuAlid     = remoteGpuAlid;
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
 * @param[in] remoteGpuClid     : Remote CLID
 * @param[in] remoteGpuAlid     : Remote ALID
 * @param[in] stage             : Whether Tx or Rx keyslot needs to be refreshed
 * @param[in] epoch             : Epoch will inform which keyslot<0,1> is spare
 */
static NV_STATUS
_knvlinkRefreshEncryptionKeys
(
    OBJGPU                 *pGpu,
    KernelNvlink           *pKernelNvlink,
    NvU8                   *pKey,
    NvU32                   remoteGpuClid,
    NvU32                   remoteGpuAlid,
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
    pGspReqHdr->wrappedKeyEntries[0].remoteGpuClid     = remoteGpuClid;
    pGspReqHdr->wrappedKeyEntries[0].remoteGpuAlid     = remoteGpuAlid;
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
    NvU32     localALID  = knvlinkGetALID(pLocalGpu, pLocalKernelNvlink);
    NvU32     remoteCLID = knvlinkGetCLID(pRemoteGpu, pRemoteKernelNvlink);
    NvU32     remoteALID = knvlinkGetALID(pRemoteGpu, pRemoteKernelNvlink);

    // TODO: Check that the NVLE key setup stage has completed

    if (!libspdm_random_bytes((NvU8 *)&nvleKey, sizeof(nvleKey)))
    {
        return NV_ERR_INVALID_DATA;
    }

    // For the transfer direction pLocalGpu->pRemoteGpu, update the new key for Tx or Rx as requested
    NV_ASSERT_OK_OR_GOTO(status,
        _knvlinkRefreshEncryptionKeys(pLocalGpu,  pLocalKernelNvlink,
                                      nvleKey, remoteCLID, remoteALID, stage, epoch), ErrorExit);

    // For the transfer direction pRemoteGpu->pLocalGpu, update the new key for Tx or Rx as requested
    NV_ASSERT_OK_OR_GOTO(status,
        _knvlinkRefreshEncryptionKeys(pRemoteGpu, pRemoteKernelNvlink,
                                      nvleKey, localCLID, localALID,  stage, epoch), ErrorExit);

ErrorExit:
    // Always be sure to scrub NVLE key regardless of success
    portMemSet((NvU8 *)&nvleKey, 0, sizeof(nvleKey));

    return status;
}

/*!
 * Sets up the intial NVLE key between a given pair of GPUs
 *
 * @param[in] pGpu                   : OBJGPU pointer
 * @param[in] pKernelNvlink          : KernelNvlink pointer
 * @param[in] localGpuAlid           : Local GPU ALID
 * @param[in] remoteGpuAlid          : Remote GPU ALID
 * @param[in] pLocalGpuPlatformInfo  : Pointer to platform info for local GPU
 * @param[in] pRemoteGpuPlatformInfo : Pointer to platform info for remote GPU
 * @param[in] pNvleKey               : Pointer to the NVLE key
 */
NV_STATUS
knvlinkSetupEncryptionKeys_IMPL
(
    OBJGPU                                      *pGpu,
    KernelNvlink                                *pKernelNvlink,
    NvU32                                        localGpuAlid,
    NvU32                                        remoteGpuAlid,
    NV2080_CTRL_NVLINK_GET_PLATFORM_INFO_PARAMS *pLocalGpuPlatformInfo,
    NV2080_CTRL_NVLINK_GET_PLATFORM_INFO_PARAMS *pRemoteGpuPlatformInfo,
    NvU8                                        *pNvleKey
)
{
    NvU8 nvleKey[RM_GSP_NVLE_AES_256_GCM_KEY_SIZE_BYTES] = {0};

    NvU32 remoteALID = remoteGpuAlid;
    NvU32 remoteCLID;
    NvU32 localALID;
    NvU32 localCLID;

    NV_ASSERT_OR_RETURN(pNvleKey != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pLocalGpuPlatformInfo != NULL && pRemoteGpuPlatformInfo != NULL, NV_ERR_INVALID_ARGUMENT);

    portMemCopy(nvleKey, RM_GSP_NVLE_AES_256_GCM_KEY_SIZE_BYTES, pNvleKey, RM_GSP_NVLE_AES_256_GCM_KEY_SIZE_BYTES);

    // Get the ALID-CLID map and cache it in gpumgr if not done already
    if (!pKernelNvlink->bClidUpdated)
    {
        NV_ASSERT_OK_OR_RETURN(knvlinkEncryptionGetUpdateGpuIdentifiers_HAL(pGpu, pKernelNvlink, NV_TRUE));
    }

    localALID = knvlinkGetALID(pGpu, pKernelNvlink);
    NV_ASSERT_OR_RETURN(localALID == localGpuAlid, NV_ERR_INVALID_STATE);

    // Get the CLIDs for the local and remote GPUs from the ALID-CLID map
    NV_ASSERT_OR_RETURN((knvlinkIsNvleAlidPresent(pGpu, pKernelNvlink, localALID, &localCLID)   == NV_TRUE),
                        NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN((knvlinkIsNvleAlidPresent(pGpu, pKernelNvlink, remoteALID, &remoteCLID) == NV_TRUE),
                        NV_ERR_INVALID_STATE);

    //
    // Check if NVLE keys are already programmed, skip programming the keys if they
    // are already setup once, key rotation will ensure that the keys are refreshed
    // at fixed time intervals.
    //
    if (!pKernelNvlink->bNvleKeySetup[remoteCLID])
    {
        NV2080_CTRL_NVLINK_UPDATE_NVLE_TOPOLOGY_PARAMS params;
        portMemSet(&params, 0, sizeof(params));

        params.localGpuAlid  = localALID;
        params.localGpuClid  = localCLID;
        params.remoteGpuAlid = remoteALID;
        params.remoteGpuClid = remoteCLID;
        params.bNvleQualMode = pKernelNvlink->bNvleQualModeRegkey;

        portMemCopy(&params.localGpuPlatformInfo, sizeof(NV2080_CTRL_NVLINK_GET_PLATFORM_INFO_PARAMS),
                    pLocalGpuPlatformInfo, sizeof(NV2080_CTRL_NVLINK_GET_PLATFORM_INFO_PARAMS));
        portMemCopy(&params.remoteGpuPlatformInfo, sizeof(NV2080_CTRL_NVLINK_GET_PLATFORM_INFO_PARAMS),
                    pRemoteGpuPlatformInfo, sizeof(NV2080_CTRL_NVLINK_GET_PLATFORM_INFO_PARAMS));

        NV_ASSERT_OK_OR_RETURN(knvlinkExecGspRmRpc(pGpu, pKernelNvlink, NV2080_CTRL_NVLINK_UPDATE_NVLE_TOPOLOGY,
                                                   (void *)&params, sizeof(params)));

        // If NVLE Qual mode is enabled, do not send NVLE key secret to the GPUs.
        if (!pKernelNvlink->bNvleQualModeRegkey)
        {
            NV_ASSERT_OK_OR_RETURN(knvlinkSendEncryptionKeys(pGpu, pKernelNvlink, nvleKey, remoteCLID, remoteALID));

            pKernelNvlink->bNvleKeySetup[remoteCLID] = NV_TRUE;
        }

    }

    return NV_OK;
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

    if (pKernelNvlink->getProperty(pKernelNvlink, PDB_PROP_KNVLINK_ENCRYPTION_ENABLED) &&
        !pKernelNvlink->bNvleQualModeRegkey)
    {
        NvU32 localCLID = knvlinkGetCLID(pGpu, pKernelNvlink);
        for (gpuInst = 0; gpuInst < NV_MAX_DEVICES; gpuInst++)
        {
            OBJGPU *pRemoteGpu = gpumgrGetGpu(gpuInst);
            if (!API_GPU_IN_RESET_SANITY_CHECK(pRemoteGpu))
            {
                KernelNvlink *pRemoteKernelNvlink = GPU_GET_KERNEL_NVLINK(pRemoteGpu);
                if ((pRemoteKernelNvlink != NULL) &&
                    pRemoteKernelNvlink->getProperty(pRemoteKernelNvlink, PDB_PROP_KNVLINK_ENCRYPTION_ENABLED) &&
                    !pRemoteKernelNvlink->bNvleQualModeRegkey)
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
    // No validation needed if NVLE is disabled on any of the GPUs or if NVLE Qual mode is enabled
    if (!(pLocalKernelNvlink->getProperty(pLocalKernelNvlink, PDB_PROP_KNVLINK_ENCRYPTION_ENABLED)   &&
          pRemoteKernelNvlink->getProperty(pRemoteKernelNvlink, PDB_PROP_KNVLINK_ENCRYPTION_ENABLED) &&
          !pLocalKernelNvlink->bNvleQualModeRegkey && !pRemoteKernelNvlink->bNvleQualModeRegkey))
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
 * Get the remap table addrs for the FLA and SPA/GPA remap tables
 *
 * @param[in] pGpu          : OBJGPU pointer
 * @param[in] pKernelNvlink : KernelNvlink pointer
 *
 * Return NV_OK on success
 */
NV_STATUS
knvlinkGetRemapTableInformation_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    NV2080_CTRL_NVLINK_GET_REMAP_TABLE_INFO_PARAMS params;
    NvU32 remapTabIdx;

    // Return error if NVLink encryption is disabled
    if (!pKernelNvlink->getProperty(pKernelNvlink, PDB_PROP_KNVLINK_ENCRYPTION_ENABLED))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    portMemSet(&params, 0, sizeof(params));

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                          knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                             NV2080_CTRL_CMD_NVLINK_GET_REMAP_TABLE_INFO,
                                             (void *)&params, sizeof(params)));

    NV_ASSERT_OR_RETURN((params.remapTabSize <= NV2080_CTRL_NVLINK_MAX_REMAP_TABLE_ENTRIES),
                        NV_ERR_INVALID_STATE);

    for (remapTabIdx = 0; remapTabIdx < params.remapTabSize; remapTabIdx++)
    {
        pKernelNvlink->flaRemapTabAddr[remapTabIdx] = params.flaRemapTabAddr[remapTabIdx];
        pKernelNvlink->gpaRemapTabAddr[remapTabIdx] = params.gpaRemapTabAddr[remapTabIdx];
    }

    return NV_OK;
}

/*!
 * Validate the given chunk of remap table slots between given pair of GPUs
 *
 * @param[in] pLocalGpu           : Local OBJGPU pointer
 * @param[in] pLocalKernelNvlink  : Local KernelNvlink pointer
 * @param[in] pRemoteGpu          : Remote OBJGPU pointer
 * @param[in] pRemoteKernelNvlink : Remote KernelNvlink pointer
 */
NV_STATUS
knvlinkValidateRemapTableSlotsV2_IMPL
(
    OBJGPU       *pLocalGpu,
    KernelNvlink *pLocalKernelNvlink,
    OBJGPU       *pRemoteGpu,
    KernelNvlink *pRemoteKernelNvlink
)
{
    // No validation needed if NVLE is disabled on any of the GPUs or if NVLE Qual mode is enabled
    if (!(pLocalKernelNvlink->getProperty(pLocalKernelNvlink, PDB_PROP_KNVLINK_ENCRYPTION_ENABLED)   &&
          pRemoteKernelNvlink->getProperty(pRemoteKernelNvlink, PDB_PROP_KNVLINK_ENCRYPTION_ENABLED) &&
          !pLocalKernelNvlink->bNvleQualModeRegkey && !pRemoteKernelNvlink->bNvleQualModeRegkey))
    {
        return NV_OK;
    }

    // Validate the cached mapslots
    NvU32 idx;
    for (idx = 0; idx < NV2080_CTRL_NVLINK_REMAP_TABLE_ENTRIES_CHUNK; idx++)
    {
        if (pLocalKernelNvlink->flaRemapTabAddr[idx] != pRemoteKernelNvlink->flaRemapTabAddr[idx])
        {
            NV_PRINTF(LEVEL_ERROR, "FLA Remap table validation failed !\n");
            return NV_ERR_INVALID_STATE;
        }

        if (pLocalKernelNvlink->gpaRemapTabAddr[idx] != pRemoteKernelNvlink->gpaRemapTabAddr[idx])
        {
            NV_PRINTF(LEVEL_ERROR, "GPA Remap table validation failed !\n");
            return NV_ERR_INVALID_STATE;
        }
    }

    return NV_OK;
}

/*!
 * Get the remap table addrs for the FLA and SPA/GPA remap tables for a given chunk of entries
 *
 * @param[in] pGpu            : OBJGPU pointer
 * @param[in] pKernelNvlink   : KernelNvlink pointer
 * @param[in] remapEntryStart : Start entry to parse from
 * @param[in] remapEntryEnd   : End entry to parse
 *
 * Return NV_OK on success
 */
NV_STATUS
knvlinkGetRemapTableInformationV2_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvU32         remapEntryStart,
    NvU32         remapEntryEnd
)
{
    NV2080_CTRL_NVLINK_GET_REMAP_TABLE_INFO_V2_PARAMS params;
    NvU32 idx;

    NV_ASSERT_OR_RETURN(
        (remapEntryStart < NV2080_CTRL_NVLINK_MAX_REMAP_TABLE_ENTRIES_V2),
        NV_ERR_INVALID_ARGUMENT);

    NV_ASSERT_OR_RETURN(
        (remapEntryEnd < NV2080_CTRL_NVLINK_MAX_REMAP_TABLE_ENTRIES_V2),
        NV_ERR_INVALID_ARGUMENT);

    NV_ASSERT_OR_RETURN((remapEntryEnd >= remapEntryStart), NV_ERR_INVALID_ARGUMENT);

    NV_ASSERT_OR_RETURN(
        (remapEntryEnd - remapEntryStart + 1 <= NV2080_CTRL_NVLINK_REMAP_TABLE_ENTRIES_CHUNK),
        NV_ERR_INVALID_ARGUMENT);

    // Return error if NVLink encryption is disabled
    if (!pKernelNvlink->getProperty(pKernelNvlink, PDB_PROP_KNVLINK_ENCRYPTION_ENABLED))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    portMemSet(&params, 0, sizeof(params));

    params.remapEntryStart = remapEntryStart;
    params.remapEntryEnd   = remapEntryEnd;

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                          knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                              NV2080_CTRL_CMD_NVLINK_GET_REMAP_TABLE_INFO_V2,
                                              (void *)&params, sizeof(params)));

    NV_ASSERT_OR_RETURN((params.remapTabSize <= NV2080_CTRL_NVLINK_REMAP_TABLE_ENTRIES_CHUNK),
                        NV_ERR_INVALID_STATE);

    for (idx = 0; idx < params.remapTabSize; idx++)
    {
        pKernelNvlink->flaRemapTabAddr[idx] = params.flaRemapTabAddr[idx];
        pKernelNvlink->gpaRemapTabAddr[idx] = params.gpaRemapTabAddr[idx];
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

    NvU32 gpuMask  = 0;
    NvU32 gpuCount = 0;
    gpumgrGetGpuAttachInfo(&gpuCount, &gpuMask);

    // On MODS platforms, we should reach here only if RmNvlinkEncryption is set, otherwise return error
    if (RMCFG_FEATURE_PLATFORM_MODS && !pKernelNvlink->bNvleModeRegkey && !pKernelNvlink->bNvleQualModeRegkey)
    {
        return NV_ERR_INVALID_STATE;
    }

    //
    // On Nvlink loopback setups, bail out early if Qual mode is enabled, since NVLE key programming is
    // handled in GSP init partition
    //
    if ((gpuCount == 1) && pKernelNvlink->bNvleQualModeRegkey)
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
        }
    }

    //
    // Get the platform information for all the GPUs available, this will be needed to get the port mappings on
    // Nvswitch systems
    //
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

            NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, knvlinkGetPlatformInfo_HAL(pGpu0, pKernelNvlink0));
        }
    }

    //
    // Get the ALID and CLID of the GPUs and whether CLIDs are already updated and remap tables are locked.
    // On multi-node setup, these steps are expected to be already performed by the time P2P object is created.
    //
    if (!pKernelNvlink->bClidUpdated)
    {
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
                                      knvlinkEncryptionGetUpdateGpuIdentifiers_HAL(pGpu0, pKernelNvlink0, NV_TRUE));
            }
        }
    }

    //
    // Skip remap table programming if the NVLE identifiers are already retrieved and remap tables are updated.
    // On multi-node setup, these steps are expected to be already performed by the time P2P object is created.
    //
    if (!pKernelNvlink->bClidUpdated)
    {
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
                                      knvlinkEncryptionGetUpdateGpuIdentifiers_HAL(pGpu0, pKernelNvlink0, NV_FALSE));
            }
        }

        //
        // All the GPUs have CLIDs in the remap tables. Generate and setup NVLE keys for all the possible P2P pairs
        // NVLE topoogy information for all possible P2P pairs will be updated as a part of this step
        //
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

                        NV_PRINTF(LEVEL_ERROR, "Setting nvle keys between GPU%d and GPU%d\n",
                                  gpuGetInstance(pGpu0), gpuGetInstance(pGpu1));

                        NvU8 nvleKey[RM_GSP_NVLE_AES_256_GCM_KEY_SIZE_BYTES] = {0};

                        if (!libspdm_random_bytes((NvU8 *)&nvleKey, sizeof(nvleKey)))
                        {
                            return NV_ERR_INVALID_DATA;
                        }

                        status = knvlinkSetupEncryptionKeys(pGpu0, pKernelNvlink0,
                                                            knvlinkGetALID(pGpu0, pKernelNvlink0),
                                                            knvlinkGetALID(pGpu1, pKernelNvlink1),
                                                            &pKernelNvlink0->platformInfo,
                                                            &pKernelNvlink1->platformInfo,
                                                            nvleKey);
                        if (status != NV_OK)
                        {
                            portMemSet((NvU8 *)&nvleKey, 0, sizeof(nvleKey));
                            return status;
                        }

                        status = knvlinkSetupEncryptionKeys(pGpu1, pKernelNvlink1,
                                                            knvlinkGetALID(pGpu1, pKernelNvlink1),
                                                            knvlinkGetALID(pGpu0, pKernelNvlink0),
                                                            &pKernelNvlink1->platformInfo,
                                                            &pKernelNvlink0->platformInfo,
                                                            nvleKey);
                        if (status != NV_OK)
                        {
                            portMemSet((NvU8 *)&nvleKey, 0, sizeof(nvleKey));
                            return status;
                        }

                        // Always be sure to scrub P2P key regardless of success
                        portMemSet((NvU8 *)&nvleKey, 0, sizeof(nvleKey));
                    }
                }
            }
        }
    }

    //
    // Validate remap table slots for all the GPUs. Remap tables validation happens before MSE locking. If remap tables
    // are locked, that implies validation is already done. On multi-node setup, these steps are expected to be already
    // performed by the time P2P object is created.
    //
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

            for (gpuInst1 = gpuInst0+1; gpuInst1 < NV_MAX_DEVICES; gpuInst1++)
            {
                pGpu1 = gpumgrGetGpu(gpuInst1);
                if (pGpu1 && (gpuIsStateLoaded(pGpu1) || gpuIsStateLoading(pGpu1)))
                {
                    pKernelNvlink1 = GPU_GET_KERNEL_NVLINK(pGpu1);
                    if (pKernelNvlink1 == NULL || pKernelNvlink1->bRemapTableMseLocked)
                    {
                        continue;
                    }

                    NvU32 remapEntryStart = 0;
                    while (remapEntryStart < NV2080_CTRL_NVLINK_MAX_REMAP_TABLE_ENTRIES_V2)
                    {
                        // Get the remap table addrs for validation before it is locked to prevent any topology updates by MSE
                        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                            knvlinkGetRemapTableInformationV2(pGpu0, pKernelNvlink0,
                                                              remapEntryStart,
                                                              remapEntryStart + NV2080_CTRL_NVLINK_REMAP_TABLE_ENTRIES_CHUNK - 1));
                        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                            knvlinkGetRemapTableInformationV2(pGpu1, pKernelNvlink1,
                                                              remapEntryStart,
                                                              remapEntryStart + NV2080_CTRL_NVLINK_REMAP_TABLE_ENTRIES_CHUNK - 1));

                        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                                              knvlinkValidateRemapTableSlotsV2(pGpu0, pKernelNvlink0, pGpu1, pKernelNvlink1));

                        remapEntryStart = remapEntryStart + NV2080_CTRL_NVLINK_REMAP_TABLE_ENTRIES_CHUNK;
                    }
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
            if (pKernelNvlink0 == NULL || pKernelNvlink0->bRemapTableMseLocked || pKernelNvlink0->bRemapTableLockDisable)
            {
                continue;
            }

            NV2080_CTRL_NVLINK_LOCK_REMAP_TABLE_AND_MSE_PARAMS params;
            portMemSet(&params, 0, sizeof(params));
            params.linkMask = KNVLINK_BITVECTOR_TO_MASK(pKernelNvlink0, enabledLinks, 64);

            // Lock remap tables and MSE from making any further changes to the nvlink topology
            status = knvlinkExecGspRmRpc(pGpu0, pKernelNvlink0,
                                         NV2080_CTRL_CMD_NVLINK_LOCK_REMAP_TABLE_AND_MSE,
                                         (void *)&params, sizeof(params));
            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR, "GPU%d Failed to lock remap table and MSE\n",
                          gpuGetInstance(pGpu0));
                return status;
            }

            pKernelNvlink0->bRemapTableLockDisable = params.bRemapTableLockDisabled;

            if (params.bRemapTableMseLocked)
            {
                NV_PRINTF(LEVEL_INFO, "GPU%d Successfully locked remap table and MSE\n",
                                       gpuGetInstance(pGpu0));

                pKernelNvlink0->bRemapTableMseLocked = NV_TRUE;
            }
        }
    }

    return NV_OK;
}

/*!
 * Assign CLID for the GPU and synchronize with GSP-RM.
 *
 * @param[in] pGpu           : OBJGPU pointer
 * @param[in] pKernelNvlink  : KernelNvlink pointer
 */
NV_STATUS
knvlinkAssignNvleClid_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    NV_STATUS status = NV_OK;

    pKernelNvlink->clid = gpuGetInstance(pGpu);

    NV2080_CTRL_NVLINK_UPDATE_CLID_PARAMS params;
    portMemSet(&params, 0, sizeof(params));

    params.clid = pKernelNvlink->clid;

    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_NVLINK_UPDATE_CLID,
                                 (void *)&params, sizeof(params));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to execute RPC to update CLID in physical RM\n");
        return status;
    }

    return NV_OK;
}

/**
 * @brief Checks if the given ALID is present in the ALID-CLID map
 *
 * @param[in]  pGpu           : OBJGPU pointer
 * @param[in]  pKernelNvlink  : KernelNvlink pointer
 * @param[in]  alid           : NVLE ALID
 * @param[out] clid           : NVLE CLID pointer
 *
 * return NV_TRUE if mapping exists, else return NV_FALSE
 */
NvBool
knvlinkIsNvleAlidPresent_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvU32         alid,
    NvU32        *pClid
)
{
    NV_ASSERT(pClid != NULL);

    NvU32 idx;

    if (pKernelNvlink->alidClidTable.numEntries == 0)
    {
        return NV_FALSE;
    }

    for (idx = 0; idx < pKernelNvlink->alidClidTable.numEntries; idx++)
    {
        if (pKernelNvlink->alidClidTable.alidClidMap[idx].alid == alid)
        {
            *pClid = pKernelNvlink->alidClidTable.alidClidMap[idx].clid;
            return NV_TRUE;
        }
    }

    return NV_FALSE;
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
