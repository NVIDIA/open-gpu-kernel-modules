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

#include "os/os.h"
#include "core/hal.h"
#include "core/info_block.h"
#include "core/locks.h"
#include "gpu/gpu.h"
#include "kernel/gpu/nvlink/kernel_nvlink.h"
#include "kernel/gpu/nvlink/kernel_ioctrl.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/mmu/kern_gmmu.h"
#include "gpu/ce/kernel_ce.h"

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

    FOR_EACH_INDEX_IN_MASK(32, i, pKernelNvlink->enabledLinks)
    {
        if (knvlinkIsP2pLoopbackSupportedPerLink_IMPL(pGpu, pKernelNvlink, i))
            return NV_TRUE;
    }
    FOR_EACH_INDEX_IN_MASK_END

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
    if (!(NVBIT(link) & pKernelNvlink->enabledLinks))
    {
        return NV_FALSE;
    }

    // Check the link connected to the same GPU (loopback)
    if (pKernelNvlink->nvlinkLinks[link].remoteEndInfo.bConnected)
    {
        if ((pKernelNvlink->nvlinkLinks[link].remoteEndInfo.domain   == gpuGetDomain(pGpu)) &&
            (pKernelNvlink->nvlinkLinks[link].remoteEndInfo.bus      == gpuGetBus(pGpu))    &&
            (pKernelNvlink->nvlinkLinks[link].remoteEndInfo.device   == gpuGetDevice(pGpu)) &&
            (pKernelNvlink->nvlinkLinks[link].remoteEndInfo.function == 0))
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

    // Get the Nvlink P2P connections from the core library
    status = knvlinkGetP2pConnectionStatus(pGpu, pKernelNvlink, pPeerGpu);

    if (status == NV_OK)
    {
        return NV_TRUE;
    }

    return NV_FALSE;
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
    NvU64          rangeStart     = knvlinkGetUniqueFabricBaseAddress(pGpu, pKernelNvlink);
    NvU64          rangeEnd       = rangeStart + (pMemoryManager->Ram.fbTotalMemSizeMb << 20);
    NvU64          peerRangeStart = knvlinkGetUniqueFabricBaseAddress(pPeerGpu,
                                                             GPU_GET_KERNEL_NVLINK(pPeerGpu));

    if (knvlinkIsGpuConnectedToNvswitch(pGpu, pKernelNvlink))
    {
        if (gpuIsSriovEnabled(pGpu))
        {
            // currently vgpu + switch doesn't support GPA addresing.
            return NV_TRUE;
        }

        if (knvlinkGetUniqueFabricBaseAddress(pGpu, pKernelNvlink) ==
            NVLINK_INVALID_FABRIC_ADDR)
        {
            NV_PRINTF(LEVEL_ERROR, "GPU %d doesn't have a fabric address\n",
                      gpuGetInstance(pGpu));

            return NV_FALSE;
        }

        if ((pGpu != pPeerGpu) &&
            ((peerRangeStart >= rangeStart) && (peerRangeStart < rangeEnd)))
        {
            NV_PRINTF(LEVEL_ERROR,
                      "GPU %d doesn't have a unique fabric address\n",
                      gpuGetInstance(pGpu));

            return NV_FALSE;
        }
    }
    else
    {
        if (knvlinkGetUniqueFabricBaseAddress(pGpu, pKernelNvlink) !=
            NVLINK_INVALID_FABRIC_ADDR)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "non-NVSwitch GPU %d has a valid fabric address\n",
                      gpuGetInstance(pGpu));

            return NV_FALSE;
        }
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

    if (pGpu1 == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid pPeerGpu.\n");

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
        NV_PRINTF(LEVEL_ERROR,
                  "Input mask contains a GPU on which NVLink is disabled.\n");

        return NV_ERR_INVALID_ARGUMENT;
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
    knvlinkCoreGetRemoteDeviceInfo(pGpu0, pKernelNvlink0);

    // Post topology link enable on links of local GPU
    status = knvlinkEnableLinksPostTopology_HAL(pGpu0, pKernelNvlink0,
                                                pKernelNvlink0->enabledLinks);
    if (status != NV_OK)
    {
        return status;
    }

    numPeerLinks = knvlinkGetNumLinksToPeer(pGpu0, pKernelNvlink0, pGpu1);
    if (numPeerLinks > 0)
    {
        if (knvlinkGetNumLinksToPeer(pGpu1, pKernelNvlink1, pGpu0) != numPeerLinks)
        {
            // Get the remote ends of the links of remote GPU from the nvlink core
            knvlinkCoreGetRemoteDeviceInfo(pGpu1, pKernelNvlink1);

            // Post topology link enable on links of remote GPU
            status = knvlinkEnableLinksPostTopology_HAL(pGpu1, pKernelNvlink1,
                                                        pKernelNvlink1->enabledLinks);
            if (status != NV_OK)
            {
                return status;
            }
        }

        // Peers should have the same number of links pointing back at us
        NV_ASSERT_OR_RETURN(knvlinkGetNumLinksToPeer(pGpu1, pKernelNvlink1, pGpu0) ==
            numPeerLinks, NV_ERR_INVALID_STATE);

        NV_ASSERT_OR_RETURN(knvlinkCheckNvswitchP2pConfig(pGpu0, pKernelNvlink0, pGpu1),
            NV_ERR_INVALID_STATE);

        NV_ASSERT_OR_RETURN(knvlinkCheckNvswitchP2pConfig(pGpu1, pKernelNvlink1, pGpu0),
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
    NvU32      i;

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

        NV2080_CTRL_NVLINK_UPDATE_CURRENT_CONFIG_PARAMS params;
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
                                     NV2080_CTRL_CMD_NVLINK_UPDATE_CURRENT_CONFIG,
                                     (void *)&params, sizeof(params));
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Updating current NVLink config failed\n");
            goto fail;
        }

        // Sync the GPU property for NVLINK over SYSMEM with GSP-RM
        pGpu->setProperty(pGpu, PDB_PROP_GPU_NVLINK_SYSMEM, params.bNvlinkSysmemEnabled);

        // Update the PCE-LCE mappings
        for (i = 0; i < ENG_CE__SIZE_1; i++)
        {
            pKCe = GPU_GET_KCE(pGpu, i);
            if (pKCe)
            {
                status = kceTopLevelPceLceMappingsUpdate(pGpu, pKCe);
                if (status != NV_OK)
                {
                    NV_PRINTF(LEVEL_ERROR, "Failed to update PCE-LCE mappings\n");
                }
                break;
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

/*!
 * @brief Return the mask of links enabled on the system
 *
 * @param[in] pGpu           OBJGPU pointer
 * @param[in] pKernelNvlink  KernelNvlink pointer
 */
NvU32
knvlinkGetEnabledLinkMask_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    return pKernelNvlink->enabledLinks;
}

/*!
 * @brief Return the mask of links discovered on the system
 *
 * @param[in] pGpu           OBJGPU pointer
 * @param[in] pKernelNvlink  KernelNvlink pointer
 */
NvU32
knvlinkGetDiscoveredLinkMask_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    return pKernelNvlink->discoveredLinks;
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
    NvU32 numPeerLinks =
        knvlinkGetLinkMaskToPeer(pGpu, pKernelNvlink, pRemoteGpu);

    if (numPeerLinks != 0)
    {
        NUMSETBITS_32(numPeerLinks);
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
NvU32
knvlinkGetLinkMaskToPeer_IMPL
(
    OBJGPU       *pGpu0,
    KernelNvlink *pKernelNvlink0,
    OBJGPU       *pGpu1
)
{
    NvU32 peerLinkMask = 0;

    if (!knvlinkIsForcedConfig(pGpu0, pKernelNvlink0))
    {
        //
        // If nvlink topology is not forced, then the hshub registers
        // are updated only when a P2P object is allocated. So, return
        // the cached value of mask of links connected to a GPU
        //
        peerLinkMask = pKernelNvlink0->peerLinkMasks[gpuGetInstance(pGpu1)];
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
    NvU32         peerLinkMask
)
{
    NV_STATUS status = NV_OK;

    // Return early if no update needed to the peer link mask
    if (pKernelNvlink0->peerLinkMasks[gpuGetInstance(pGpu1)] == peerLinkMask)
        return NV_OK;

    pKernelNvlink0->peerLinkMasks[gpuGetInstance(pGpu1)] = peerLinkMask;

    NV2080_CTRL_NVLINK_UPDATE_PEER_LINK_MASK_PARAMS params;

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
                                 NV2080_CTRL_CMD_NVLINK_UPDATE_PEER_LINK_MASK,
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
 * @brief Get the mask of links that are peer links
 *
 * @param[in] pGpu           OBJGPU pointer
 * @param[in] pKernelNvlink  KernelNvlink pointer
 */
NvU32
knvlinkGetPeersNvlinkMaskFromHshub_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    NV_STATUS status       = NV_OK;
    NvU32     peerLinkMask = 0;
    NvU32     i;

    NV2080_CTRL_NVLINK_GET_LINK_AND_CLOCK_INFO_PARAMS params;

    portMemSet(&params, 0, sizeof(params));
    params.linkMask = pKernelNvlink->enabledLinks;

    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_NVLINK_GET_LINK_AND_CLOCK_INFO,
                                 (void *)&params, sizeof(params));
    if (status != NV_OK)
        return 0;

    // Scan enabled links for peer connections
    FOR_EACH_INDEX_IN_MASK(32, i, pKernelNvlink->enabledLinks)
    {
        if (params.linkInfo[i].bLinkConnectedToPeer)
            peerLinkMask |= NVBIT(i);
    }
    FOR_EACH_INDEX_IN_MASK_END;

    return peerLinkMask;
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
    KernelNvlink *pKernelNvlink
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
    if (pSys->getProperty(pSys, PDB_PROP_SYS_FABRIC_IS_EXTERNALLY_MANAGED))
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
            API_GPU_IN_RESET_SANITY_CHECK(pRemoteGpu))
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
        status = knvlinkRemoveMapping_HAL(pRemoteGpu, pRemoteKernelNvlink, NV_FALSE,
                                          ((1 << NVLINK_MAX_PEERS_SW) - 1),
                                          NV_FALSE /* bL2Entry */);
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
    status = knvlinkRemoveMapping_HAL(pGpu, pKernelNvlink, NV_TRUE, ((1 << NVLINK_MAX_PEERS_SW) - 1),
                                      NV_FALSE /* bL2Entry */);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "failed to reset HSHUB on GPU%u while preparing XVE reset: %s (0x%x)\n",
                  gpuGetInstance(pGpu), nvstatusToString(status), status);

        retStatus = (retStatus == NV_OK) ? status : retStatus;
    }

    // Pseudo-clean  shutdown the links from this GPU
    status = knvlinkCoreShutdownDeviceLinks(pGpu, pKernelNvlink);
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

        //
        // knvlinkCoreResetDeviceLinks() only resets the links which have
        // connectivity.
        // Pre-Ampere, we may run into a situation where the PLL
        // sharing partner links (both) may not be reset due to no connectivity.
        //
        // Hence, (re-)reset all the links to recover them after shutdown (pre-Ampere)
        //
        NV2080_CTRL_NVLINK_RESET_LINKS_PARAMS resetLinksparams;

        portMemSet(&resetLinksparams, 0, sizeof(resetLinksparams));
        resetLinksparams.linkMask = pKernelNvlink->enabledLinks;
        resetLinksparams.flags    = NV2080_CTRL_NVLINK_RESET_FLAGS_TOGGLE;

        status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                     NV2080_CTRL_CMD_NVLINK_RESET_LINKS,
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
            // Regkeys finally decide whether or not the power state is supported
            pKernelNvlink->setProperty(pKernelNvlink, PDB_PROP_KNVLINK_SINGLE_LANE_POWER_STATE_ENABLED,
                                       (pKernelNvlink->bDisableSingleLaneMode ? NV_FALSE : NV_TRUE));

            // NVLink L2 is supported only on MODS and Windows LDDM
            if (RMCFG_FEATURE_PLATFORM_WINDOWS_LDDM || RMCFG_FEATURE_PLATFORM_MODS)
            {
                pKernelNvlink->setProperty(pKernelNvlink, PDB_PROP_KNVLINK_L2_POWER_STATE_ENABLED,
                                           (pKernelNvlink->bDisableL2Mode ? NV_FALSE : NV_TRUE));
            }

            break;
        }
        case NVLINK_VERSION_31:
        case NVLINK_VERSION_30:
        case NVLINK_VERSION_20:
        {
            // Regkeys finally decide whether or not the power state is supported
            pKernelNvlink->setProperty(pKernelNvlink, PDB_PROP_KNVLINK_SINGLE_LANE_POWER_STATE_ENABLED,
                                       (pKernelNvlink->bDisableSingleLaneMode ? NV_FALSE : NV_TRUE));
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
        pSys->getProperty(pSys, PDB_PROP_SYS_FABRIC_MANAGER_IS_REGISTERED))
    {
        return;
    }

    if (pKernelNvlink->discoveredLinks == 0)
    {
        return;
    }

    // Get the link train status for the enabled link masks
    NV2080_CTRL_NVLINK_ARE_LINKS_TRAINED_PARAMS linkTrainedParams;

    portMemSet(&linkTrainedParams, 0, sizeof(linkTrainedParams));
    linkTrainedParams.linkMask    = pKernelNvlink->enabledLinks;
    linkTrainedParams.bActiveOnly = NV_FALSE;

    // Reset timeout to clear any accumulated timeouts from link init
    if (IS_GSP_CLIENT(pGpu))
    {
        threadStateResetTimeout(pGpu);
    }

    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_NVLINK_ARE_LINKS_TRAINED,
                                 (void *)&linkTrainedParams, sizeof(linkTrainedParams));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to get the link train status for links\n");
        return;
    }

    FOR_EACH_INDEX_IN_MASK(32, i, pKernelNvlink->enabledLinks)
    {
        if (!linkTrainedParams.bIsLinkActive[i])
        {
            return;
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

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

    NV2080_CTRL_NVLINK_GET_SET_NVSWITCH_FLA_ADDR_PARAMS params;

    if (!knvlinkIsForcedConfig(pGpu, pKernelNvlink))
    {
        knvlinkCoreGetRemoteDeviceInfo(pGpu, pKernelNvlink);

        status = knvlinkEnableLinksPostTopology_HAL(pGpu, pKernelNvlink,
                                                    pKernelNvlink->enabledLinks);
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

    NV2080_CTRL_NVLINK_SYNC_LINK_MASKS_AND_VBIOS_INFO_PARAMS params;

    portMemSet(&params, 0, sizeof(params));

    params.discoveredLinks     = pKernelNvlink->discoveredLinks;
    params.connectedLinksMask  = pKernelNvlink->connectedLinksMask;
    params.bridgeSensableLinks = pKernelNvlink->bridgeSensableLinks;
    params.bridgedLinks        = pKernelNvlink->bridgedLinks;

    // Reset timeout to clear any accumulated timeouts from link init
    if (IS_GSP_CLIENT(pGpu))
    {
        threadStateResetTimeout(pGpu);
    }

    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_NVLINK_SYNC_LINK_MASKS_AND_VBIOS_INFO,
                                 (void *)&params, sizeof(params));

    pKernelNvlink->vbiosDisabledLinkMask = params.vbiosDisabledLinkMask;
    pKernelNvlink->initializedLinks      = params.initializedLinks;
    pKernelNvlink->initDisabledLinksMask = params.initDisabledLinksMask;
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

    NV2080_CTRL_NVLINK_UPDATE_LINK_CONNECTION_PARAMS params;

    portMemSet(&params, 0, sizeof(params));

    params.linkId = linkId;

#if defined(INCLUDE_NVLINK_LIB)

    params.bConnected       = pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.bConnected;
    params.remoteDeviceType = pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.deviceType;
    params.remoteLinkNumber = pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.linkNumber;

#endif

    // Reset timeout to clear any accumulated timeouts from link init
    if (IS_GSP_CLIENT(pGpu))
    {
        threadStateResetTimeout(pGpu);
    }

    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_NVLINK_UPDATE_LINK_CONNECTION,
                                 (void *)&params, sizeof(params));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to update Link connection status!\n");
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

    NV2080_CTRL_NVLINK_GET_LINK_MASK_POST_RX_DET_PARAMS params;

    portMemSet(&params, 0, sizeof(params));

    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_NVLINK_GET_LINK_MASK_POST_RX_DET,
                                 (void *)&params, sizeof(params));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to update Rx Detect Link mask!\n");
        return status;
    }

    pKernelNvlink->postRxDetLinkMask = params.postRxDetLinkMask;

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

    NV2080_CTRL_NVLINK_GET_NVLINK_DEVICE_INFO_PARAMS nvlinkInfoParams;

    portMemSet(&nvlinkInfoParams, 0, sizeof(nvlinkInfoParams));

    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_NVLINK_GET_NVLINK_DEVICE_INFO,
                                 (void *)&nvlinkInfoParams, sizeof(nvlinkInfoParams));

    if (status == NV_ERR_NOT_SUPPORTED)
    {
        NV_PRINTF(LEVEL_WARNING, "NVLink is unavailable\n");
        return status;
    }
    else if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to retrieve all nvlink device info!\n");
        return status;
    }

    // Update CPU-RM's NVLink state with the information received from GSP-RM RPC
    pKernelNvlink->ioctrlMask       = nvlinkInfoParams.ioctrlMask;
    pKernelNvlink->ioctrlNumEntries = nvlinkInfoParams.ioctrlNumEntries;
    pKernelNvlink->ioctrlSize       = nvlinkInfoParams.ioctrlSize;
    pKernelNvlink->discoveredLinks  = nvlinkInfoParams.discoveredLinks;
    pKernelNvlink->ipVerNvlink      = nvlinkInfoParams.ipVerNvlink;

    for (i = 0; i < NVLINK_MAX_LINKS_SW; i++)
    {
        pKernelNvlink->nvlinkLinks[i].pGpu     = pGpu;
        pKernelNvlink->nvlinkLinks[i].bValid   = nvlinkInfoParams.linkInfo[i].bValid;
        pKernelNvlink->nvlinkLinks[i].linkId   = nvlinkInfoParams.linkInfo[i].linkId;
        pKernelNvlink->nvlinkLinks[i].ioctrlId = nvlinkInfoParams.linkInfo[i].ioctrlId;

        // Copy over the link PLL master and slave relationship for each link
        pKernelNvlink->nvlinkLinks[i].pllMasterLinkId = nvlinkInfoParams.linkInfo[i].pllMasterLinkId;
        pKernelNvlink->nvlinkLinks[i].pllSlaveLinkId  = nvlinkInfoParams.linkInfo[i].pllSlaveLinkId;

        // Copy over the ip versions for DLPL devices discovered
        pKernelNvlink->nvlinkLinks[i].ipVerDlPl = nvlinkInfoParams.linkInfo[i].ipVerDlPl;
    }

    return NV_OK;
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

    NV2080_CTRL_NVLINK_GET_IOCTRL_DEVICE_INFO_PARAMS ioctrlInfoParams;

    // Query the IOCTRL information for each of the IOCTRLs discovered
    FOR_EACH_INDEX_IN_MASK(32, ioctrlIdx, pKernelNvlink->ioctrlMask)
    {
        portMemSet(&ioctrlInfoParams, 0, sizeof(ioctrlInfoParams));

        ioctrlInfoParams.ioctrlIdx = ioctrlIdx;

        status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                     NV2080_CTRL_CMD_NVLINK_GET_IOCTRL_DEVICE_INFO,
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

    // Start with all links disabled and no forced config in effect
    pKernelNvlink->bRegistryLinkOverride = NV_TRUE;
    pKernelNvlink->registryLinkMask      = 0;
    pKernelNvlink->bChiplibConfig        = NV_FALSE;

    for (i = 0; i < NVLINK_MAX_LINKS_SW; i++)
    {
        // Filter against the links discovered from IOCTRL
        if (!(pKernelNvlink->discoveredLinks & NVBIT(i)))
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
            pKernelNvlink->registryLinkMask |= NVBIT(physLink);

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
    pKernelNvlink->enabledLinks = pKernelNvlink->discoveredLinks & pKernelNvlink->registryLinkMask;

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

    NV2080_CTRL_NVLINK_SYNC_NVLINK_SHUTDOWN_PROPS_PARAMS params;

    portMemSet(&params, 0, sizeof(params));

    params.bLaneShutdownEnabled  =
        pKernelNvlink->getProperty(pKernelNvlink, PDB_PROP_KNVLINK_LANE_SHUTDOWN_ENABLED);
    params.bLaneShutdownOnUnload =
        pKernelNvlink->getProperty(pKernelNvlink, PDB_PROP_KNVLINK_LANE_SHUTDOWN_ON_UNLOAD);

    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_NVLINK_SYNC_NVLINK_SHUTDOWN_PROPS,
                                 (void *)&params, sizeof(params));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to sync NVLink shutdown properties with GSP!\n");
        return status;
    }

    return NV_OK;
}

/*!
 * @brief   Set unique fabric address for NVSwitch enabled systems.
 *
 * @param[in] pGpu           OBJGPU pointer
 * @param[in] pKernelNvlink  KernelNvlink pointer
 * @param[in] fabricBaseAddr Fabric Address to set
 *
 * @returns On success, sets unique fabric address and returns NV_OK.
 *          On failure, returns NV_ERR_XXX.
 */
NV_STATUS
knvlinkSetUniqueFabricBaseAddress_IMPL
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvU64         fabricBaseAddr
)
{
    NV_STATUS status = NV_OK;

    if (!knvlinkIsForcedConfig(pGpu, pKernelNvlink))
    {
        knvlinkCoreGetRemoteDeviceInfo(pGpu, pKernelNvlink);

        knvlinkEnableLinksPostTopology_HAL(pGpu, pKernelNvlink,
                                           pKernelNvlink->enabledLinks);
    }

    if (!knvlinkIsGpuConnectedToNvswitch(pGpu, pKernelNvlink))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Operation failed due to no NVSwitch connectivity to the "
                  "GPU  %x\n", pGpu->gpuInstance);
        return NV_ERR_INVALID_STATE;
    }

    status = knvlinkValidateFabricBaseAddress_HAL(pGpu, pKernelNvlink,
                                                  fabricBaseAddr);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Fabric addr validation failed for GPU %x\n",
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

    if (pKernelNvlink->fabricBaseAddr == fabricBaseAddr)
    {
        NV_PRINTF(LEVEL_INFO,
                  "The same fabric addr is being re-assigned to GPU %x\n",
                  pGpu->gpuInstance);
        return NV_OK;
    }

    if (pKernelNvlink->fabricBaseAddr != NVLINK_INVALID_FABRIC_ADDR)
    {
        NV_PRINTF(LEVEL_ERROR, "Fabric addr is already assigned to GPU %x\n",
                  pGpu->gpuInstance);
        return NV_ERR_STATE_IN_USE;
    }

    //
    // Update GMMU peer field descriptor.
    // We can safely defer reinitialization of peer field descriptor to this
    // call because RM doesn't allow any P2P operations until FM assigns fabric
    // addresses.
    //
    NV2080_CTRL_INTERNAL_NVLINK_GET_SET_NVSWITCH_FABRIC_ADDR_PARAMS params;

    portMemSet(&params, 0, sizeof(params));
    params.bGet = NV_FALSE;
    params.addr = fabricBaseAddr;

    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_INTERNAL_NVLINK_GET_SET_NVSWITCH_FABRIC_ADDR,
                                 (void *)&params, sizeof(params));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to stash fabric address for GPU %x\n",
                  pGpu->gpuInstance);
        return status;
    }

    pKernelNvlink->fabricBaseAddr = fabricBaseAddr;

    NV_PRINTF(LEVEL_ERROR, "Fabric base addr %llx is assigned to GPU %x\n",
              pKernelNvlink->fabricBaseAddr, pGpu->gpuInstance);

    return NV_OK;
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
    NvU32     mask                 = 0;
    NvBool    bSkipHwNvlinkDisable = 0;
    NV_STATUS status               = NV_OK;

    NV2080_CTRL_NVLINK_PROCESS_INIT_DISABLED_LINKS_PARAMS params;

    status = gpumgrGetGpuInitDisabledNvlinks(pGpu->gpuId, &mask, &bSkipHwNvlinkDisable);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to get init disabled links from gpumgr\n");
        return status;
    }

    portMemSet(&params, 0, sizeof(params));

    params.initDisabledLinksMask = mask;
    params.bSkipHwNvlinkDisable  = bSkipHwNvlinkDisable;

    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_NVLINK_PROCESS_INIT_DISABLED_LINKS,
                                 (void *)&params, sizeof(params));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to process init disabled links in GSP\n");
        return status;
    }

    pKernelNvlink->initDisabledLinksMask = params.initDisabledLinksMask;

    return NV_OK;
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
