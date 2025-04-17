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

#define NVOC_KERNEL_NVLINK_H_PRIVATE_ACCESS_ALLOWED

#include "os/os.h"
#include "kernel/gpu/nvlink/kernel_nvlink.h"
#include "kernel/gpu/nvlink/kernel_ioctrl.h"

#include "gpu/gpu.h"
#include "gpu/ce/kernel_ce.h"
#include "nvrm_registry.h"

//
// NVLINK Override Configuration
//
NV_STATUS
knvlinkOverrideConfig_GP100
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvU32         phase
)
{
    NV_STATUS status = NV_OK;

    NV2080_CTRL_INTERNAL_NVLINK_PROCESS_FORCED_CONFIGS_PARAMS forcedConfigParams;
    portMemSet(&forcedConfigParams, 0, sizeof(forcedConfigParams));

    forcedConfigParams.bLegacyForcedConfig = NV_TRUE;
    forcedConfigParams.phase               = phase;

    // RPC to GSP-RM to for GSP-RM to process the forced NVLink configurations.
    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_INTERNAL_NVLINK_PROCESS_FORCED_CONFIGS,
                                 (void *)&forcedConfigParams,
                                 sizeof(forcedConfigParams));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to process forced NVLink configurations !\n");
        return status;
    }

    return NV_OK;
}

/*!
 * Get a mask with one bit set for each unique GPU peer connected via
 * NVLINK. In this implementation, each bit is the lowest link ID of
 * all links connected to a given GPU peer. This allows a static peer
 * ID assignment based on link topology.
 *
 * Note: Ampere and beyond, there is no static peer ID allocation for
 *       NVLink.
 *
 * @param[in] pGpu          OBJGPU ptr
 * @param[in] pKernelNvlink KernelNvlink ptr
 *
 * return NvU32 unique nvlink peer ID mask for pGpu
 */
NvU32
knvlinkGetUniquePeerIdMask_GP100
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    NvU32 uniqueIdMask = 0;
    NvU32 i;

    for (i = 0; i < NV_ARRAY_ELEMENTS(pKernelNvlink->peerLinkMasks); i++)
    {
        NvU32 peerLinkMask = KNVLINK_GET_MASK(pKernelNvlink, peerLinkMasks[i], 32);
        if (peerLinkMask != 0)
        {
            uniqueIdMask |= LOWESTBIT(peerLinkMask);
        }
    }

    return uniqueIdMask;
}

/**
 * Get a unique peerID for the remote GPU connected via NVLINK. In this
 * implementation, that peer ID is the lowest link ID of all the links
 * connected to the peer GPU. This allows a static peer ID assignment
 * based on link topology.
 *
 * Note: Ampere and beyond, there is no static peer ID allocation for
 *       NVLink.
 *
 * @param[in]  pGpu          OBJGPU pointer for local GPU
 * @param[in]  pKernelNvlink KernelNvlink pointer
 * @param[in]  pRemoteGpu    OBJGPU pointer for remote GPU
 *
 * return NvU32 unique nvlink peer ID pGpu to pRemoteGpu
 */
NvU32
knvlinkGetUniquePeerId_GP100
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    OBJGPU       *pRemoteGpu
)
{
    NvU32 peerLinkMask;

    peerLinkMask = KNVLINK_GET_MASK(pKernelNvlink, peerLinkMasks[gpuGetInstance(pRemoteGpu)], 32);
    if (peerLinkMask == 0)
    {
        return BUS_INVALID_PEER;
    }

    LOWESTBITIDX_32(peerLinkMask);

    return peerLinkMask;
}

/*!
 * @brief Wrapper function chose between removing all or peer mappings
 *
 * @param[in] pGpu          OBJGPU pointer
 * @param[in] pKernelNvlink KernelNvlink pointer
 * @param[in] bAllMapping   Whether both sysmem and peer mappings should be removed
 * @param[in] peerMask      Mask of peers for which mappings will be removed
 * @param[in] bL2Entry      Are the mappings being removed because of L2 entry?
 *
 * @return   Returns NV_OK on success
 */
NV_STATUS
knvlinkRemoveMapping_GP100
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvBool        bAllMapping,
    NvU32         peerMask,
    NvBool        bL2Entry
)
{
    NV_STATUS status = NV_OK;

    NV2080_CTRL_INTERNAL_NVLINK_REMOVE_NVLINK_MAPPING_PARAMS params;
    portMemSet(&params, 0, sizeof(params));

    params.bL2Entry = bL2Entry;

    if (bAllMapping)
    {
        params.mapTypeMask = NV2080_CTRL_INTERNAL_NVLINK_REMOVE_NVLINK_MAPPING_TYPE_SYSMEM |
                             NV2080_CTRL_INTERNAL_NVLINK_REMOVE_NVLINK_MAPPING_TYPE_PEER;
        params.peerMask    = (1 << NVLINK_MAX_PEERS_SW) - 1;
    }
    else
    {
        params.mapTypeMask = NV2080_CTRL_INTERNAL_NVLINK_REMOVE_NVLINK_MAPPING_TYPE_PEER;
        params.peerMask    = peerMask;
    }

    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_INTERNAL_NVLINK_REMOVE_NVLINK_MAPPING,
                                 (void *)&params, sizeof(params));
    return status;
}

/**
 * @brief Get the mask of optimal CEs for P2P reads/writes
 *
 * @param[in]   pGpu                    OBJGPU pointer
 * @param[in]   pKernelNvlink           KernelNvlink pointer
 * @param[in]   gpuMask                 Mask of GPUs instances
 * @param[out]  sysmemOptimalReadCEs    Mask of CEs for SYSMEM reads
 * @param[out]  sysmemOptimalWriteCEs   Mask of CEs for SYSMEM writes
 * @param[out]  p2pOptimalReadCEs       Mask of CEs for P2P reads
 * @param[out]  p2pOptimalWriteCEs      Mask of CEs for P2P writes
 */
NV_STATUS
knvlinkGetP2POptimalCEs_GP100
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvU32         gpuMask,
    NvU32        *sysmemOptimalReadCEs,
    NvU32        *sysmemOptimalWriteCEs,
    NvU32        *p2pOptimalReadCEs,
    NvU32        *p2pOptimalWriteCEs
)
{
    KernelCE *pKCe            = NULL;
    NvU32     sysmemReadCE    = 0;
    NvU32     sysmemWriteCE   = 0;
    NvU32     nvlinkP2PCeMask = 0;

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, kceFindFirstInstance(pGpu, &pKCe));
    kceGetCeFromNvlinkConfig(pGpu, pKCe,
                             gpuMask,
                             &sysmemReadCE,
                             &sysmemWriteCE,
                             &nvlinkP2PCeMask);

    if (sysmemOptimalReadCEs != NULL)
    {
        *sysmemOptimalReadCEs = NVBIT(sysmemReadCE);
    }

    if (sysmemOptimalWriteCEs != NULL)
    {
        *sysmemOptimalWriteCEs = NVBIT(sysmemWriteCE);
    }

    if (p2pOptimalReadCEs != NULL)
    {
        *p2pOptimalReadCEs  = nvlinkP2PCeMask;
    }

    if (p2pOptimalWriteCEs != NULL)
    {
        *p2pOptimalWriteCEs = nvlinkP2PCeMask;
    }

    return NV_OK;
}

/**
 * @brief Setup peer mapping for the given ID to the remote GPU,
 *        and program HSHUB to finalize the mapping.
 *
 * @param[in] pGpu          OBJGPU pointer (Local)
 * @param[in] pKernelNvlink KernelNvlink pointer
 * @param[in] pRemoteGpu    OBJGPU pointer (Remote)
 * @param[in] peerId        peer ID
 */
void
knvlinkSetupPeerMapping_GP100
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    OBJGPU       *pRemoteGpu,
    NvU32         peerId
)
{
    NV_STATUS status = NV_OK;
    NvU64     peerLinkMask;

    NV2080_CTRL_INTERNAL_NVLINK_PRE_SETUP_NVLINK_PEER_PARAMS  preSetupNvlinkPeerParams;
    NV2080_CTRL_INTERNAL_NVLINK_POST_SETUP_NVLINK_PEER_PARAMS postSetupNvlinkPeerParams;

    // HSHUB registers are updated during driver load if nvlink topology is forced
    if (!knvlinkIsForcedConfig(pGpu, pKernelNvlink))
    {
        if ((pGpu == pRemoteGpu) &&
             pGpu->getProperty(pGpu, PDB_PROP_GPU_NVLINK_P2P_LOOPBACK_DISABLED))
        {
            NV_PRINTF(LEVEL_ERROR,
                      "P2P loopback is disabled on GPU%u, aborting peer setup (0x%x)\n",
                      gpuGetInstance(pGpu), peerId);
            return;
        }

        peerLinkMask = KNVLINK_GET_MASK(pKernelNvlink, peerLinkMasks[gpuGetInstance(pRemoteGpu)], 64);
        knvlinkGetEffectivePeerLinkMask_HAL(pGpu, pKernelNvlink, pRemoteGpu, &peerLinkMask);

        if (peerLinkMask != 0)
        {
            portMemSet(&preSetupNvlinkPeerParams, 0, sizeof(preSetupNvlinkPeerParams));
            preSetupNvlinkPeerParams.peerId        = peerId;
            preSetupNvlinkPeerParams.peerLinkMask  = peerLinkMask;
            preSetupNvlinkPeerParams.bEgmPeer      = GPU_GET_KERNEL_BUS(pGpu)->p2p.bEgmPeer[peerId];
            preSetupNvlinkPeerParams.bNvswitchConn = knvlinkIsGpuConnectedToNvswitch(pGpu, pKernelNvlink);

            status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                         NV2080_CTRL_CMD_INTERNAL_NVLINK_PRE_SETUP_NVLINK_PEER,
                                         (void *)&preSetupNvlinkPeerParams,
                                         sizeof(preSetupNvlinkPeerParams));
            NV_ASSERT(status == NV_OK);

            // Update *ALL* the HSHUB settings together
            knvlinkUpdateCurrentConfig(pGpu, pKernelNvlink);

            portMemSet(&postSetupNvlinkPeerParams, 0, sizeof(postSetupNvlinkPeerParams));
            postSetupNvlinkPeerParams.peerMask = NVBIT(peerId);

            status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                         NV2080_CTRL_CMD_INTERNAL_NVLINK_POST_SETUP_NVLINK_PEER,
                                         (void *)&postSetupNvlinkPeerParams,
                                         sizeof(postSetupNvlinkPeerParams));
            NV_ASSERT(status == NV_OK);
        }
    }
}

/*!
 * @brief Return the mask of links that are connected
 *
 * @param[in] pGpu           OBJGPU ptr
 * @param[in] pKernelNvlink  KernelNvlink ptr
 */
NvU32
knvlinkGetConnectedLinksMask_GP100
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    //
    // Connected links are already filtered against the
    // enabled links. Hence, enabledLinks has final say
    //
    return knvlinkGetEnabledLinkMask(pGpu, pKernelNvlink);
}

/*!
 * @brief Program NVLink Speed for the enabled links
 *
 * @param[in] pGpu           OBJGPU ptr
 * @param[in] pKernelNvlink  KernelNvlink ptr
 */
NV_STATUS
knvlinkProgramLinkSpeed_GP100
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    NV_STATUS status = NV_OK;

    NV2080_CTRL_INTERNAL_NVLINK_PROGRAM_LINK_SPEED_PARAMS programLinkSpeedParams;
    portMemSet(&programLinkSpeedParams, 0, sizeof(programLinkSpeedParams));

    programLinkSpeedParams.bPlatformLinerateDefined = NV_FALSE;
    programLinkSpeedParams.platformLineRate         =
                          NV_REG_STR_RM_NVLINK_SPEED_CONTROL_SPEED_DEFAULT;

    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_INTERNAL_NVLINK_PROGRAM_LINK_SPEED,
                                 (void *)&programLinkSpeedParams,
                                 sizeof(programLinkSpeedParams));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to program NVLink speed for links!\n");
        return status;
    }

    pKernelNvlink->nvlinkLinkSpeed = programLinkSpeedParams.nvlinkLinkSpeed;

    return NV_OK;
}

/*!
 * @brief Get the device PCI info and store it in 
 * nvlink_device_info struct which will be passed to corelib
 *
 * @param[in]  pGpu                 OBJGPU pointer
 * @param[in]  pKernelNvlink        KernelNvlink pointer
 * @param[out] nvlink_device_info   Nvlink device info pointer
 *
 */
#if defined(INCLUDE_NVLINK_LIB)
void
knvlinkCoreGetDevicePciInfo_GP100
(
    OBJGPU             *pGpu,
    KernelNvlink       *pKernelNvlink,
    nvlink_device_info *devInfo
)
{
    devInfo->pciInfo.domain   = gpuGetDomain(pGpu);
    devInfo->pciInfo.bus      = gpuGetBus(pGpu);
    devInfo->pciInfo.device   = gpuGetDevice(pGpu);
    devInfo->pciInfo.function = 0;
}
#endif
