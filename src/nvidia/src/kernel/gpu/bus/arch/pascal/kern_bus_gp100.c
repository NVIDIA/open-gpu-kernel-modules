/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "core/core.h"
#include "gpu/gpu.h"
#include "mem_mgr/vaspace.h"
#include "gpu/bus/kern_bus.h"
#include "gpu/bus/p2p_api.h"
#include "kernel/gpu/nvlink/kernel_nvlink.h"

/*!
 * @brief Create a P2P mapping to a given peer GPU
 *
 * @param[in]   pGpu0          (local GPU)
 * @param[in]   pKernelBus0    (local GPU)
 * @param[in]   pGpu1          (remote GPU)
 * @param[in]   pKernelBus1    (remote GPU)
 * @param[out]  peer0  Peer ID (local to remote)
 * @param[out]  peer1  Peer ID (remote to local)
 * @param[in]   attributes Sepcial attributes for the mapping
 *
 * return NV_OK on success
 */
NV_STATUS
kbusCreateP2PMapping_GP100
(
    OBJGPU    *pGpu0,
    KernelBus *pKernelBus0,
    OBJGPU    *pGpu1,
    KernelBus *pKernelBus1,
    NvU32     *peer0,
    NvU32     *peer1,
    NvU32      attributes
)
{
    if (FLD_TEST_DRF(_P2PAPI, _ATTRIBUTES, _CONNECTION_TYPE, _NVLINK, attributes) ||
        FLD_TEST_DRF(_P2PAPI, _ATTRIBUTES, _CONNECTION_TYPE, _NVLINK_INDIRECT, attributes))
    {
        return kbusCreateP2PMappingForNvlink_HAL(pGpu0, pKernelBus0, pGpu1, pKernelBus1, peer0, peer1, attributes);
    }

    if (FLD_TEST_DRF(_P2PAPI, _ATTRIBUTES, _CONNECTION_TYPE, _PCIE, attributes))
    {
        return kbusCreateP2PMappingForMailbox_HAL(pGpu0, pKernelBus0, pGpu1, pKernelBus1, peer0, peer1, attributes);
    }

    NV_PRINTF(LEVEL_ERROR, "P2P type %d is not supported\n", DRF_VAL(_P2PAPI, _ATTRIBUTES, _CONNECTION_TYPE, attributes));

    return NV_ERR_NOT_SUPPORTED;
}

/*!
 * @brief Create a P2P mapping to a given peer GPU
 *
 * @param[in]   pGpu0          (local GPU)
 * @param[in]   pKernelBus0    (local GPU)
 * @param[in]   pGpu1          (remote GPU)
 * @param[in]   pKernelBus1    (remote GPU)
 * @param[out]  peer0  Peer ID (local to remote)
 * @param[out]  peer1  Peer ID (remote to local)
 * @param[in]   attributes Sepcial attributes for the mapping
 * 
 * return NV_OK on success
 */
NV_STATUS
kbusCreateP2PMappingForNvlink_GP100
(
    OBJGPU    *pGpu0,
    KernelBus *pKernelBus0,
    OBJGPU    *pGpu1,
    KernelBus *pKernelBus1,
    NvU32     *peer0,
    NvU32     *peer1,
    NvU32      attributes
)
{
    NvU32         gpu0Instance   = gpuGetInstance(pGpu0);
    NvU32         gpu1Instance   = gpuGetInstance(pGpu1);
    NvBool        bLoopback      = (pGpu0 == pGpu1);
    KernelNvlink *pKernelNvlink0 = GPU_GET_KERNEL_NVLINK(pGpu0);
    KernelNvlink *pKernelNvlink1 = GPU_GET_KERNEL_NVLINK(pGpu1);
    NV_STATUS     status         = NV_OK;
    NvU32         nvlinkPeer0    = BUS_INVALID_PEER;
    NvU32         nvlinkPeer1    = BUS_INVALID_PEER;

    NV2080_CTRL_NVLINK_ENABLE_NVLINK_PEER_PARAMS  params;

    if (peer0 == NULL || peer1 == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Get the peer ID pGpu0 should use for P2P over NVLINK to pGpu1
    if ((status = kbusGetNvlinkP2PPeerId_HAL(pGpu0, pKernelBus0,
                                             pGpu1, pKernelBus1,
                                             &nvlinkPeer0)) != NV_OK)
    {
        return status;
    }

    // Get the peer ID pGpu1 should use for P2P over NVLINK to pGpu0
    if ((status = kbusGetNvlinkP2PPeerId_HAL(pGpu1, pKernelBus1,
                                             pGpu0, pKernelBus0,
                                             &nvlinkPeer1)) != NV_OK)
    {
        return status;
    }

    if ((pKernelNvlink0 == NULL) || (pKernelNvlink1 == NULL) ||
        (nvlinkPeer0 == BUS_INVALID_PEER) || (nvlinkPeer1 == BUS_INVALID_PEER))
    {
        return NV_ERR_INVALID_REQUEST;
    }

    // Set the default RM mapping if peer id's are not explicitly provided
    if (*peer0 == BUS_INVALID_PEER || *peer1 == BUS_INVALID_PEER)
    {
        if (bLoopback)
        {
            if (pKernelBus0->p2pMapSpecifyId)
            {
                *peer0 = *peer1 = pKernelBus0->p2pMapPeerId;
            }
            else
            {
                // If no static mapping is found, set peer id as 0 for loopback
                *peer0 = *peer1 = 0;
            }
        }
        else
        {
            *peer0 = nvlinkPeer0;
            *peer1 = nvlinkPeer1;
        }

        NV_PRINTF(LEVEL_INFO, "- P2P: Using Default RM mapping for P2P.\n");
    }

    //
    // Does the mapping already exist between the given pair of GPUs using the peerIDs
    // *peer0 and *peer1 respectively ?
    //
    if ((pKernelBus0->p2p.busNvlinkPeerNumberMask[gpu1Instance] & NVBIT(*peer0)) &&
        (pKernelBus1->p2p.busNvlinkPeerNumberMask[gpu0Instance] & NVBIT(*peer1)))
    {
        //
        // Increment the mapping refcount per peerID - since there is another usage
        // of a mapping that is using this peerID
        //
        pKernelBus0->p2p.busNvlinkMappingRefcountPerPeerId[*peer0]++;
        pKernelBus1->p2p.busNvlinkMappingRefcountPerPeerId[*peer1]++;

        //
        // Increment the mapping refcount per GPU - since there is another usage of
        // the mapping to the given remote GPU
        //
        pKernelBus0->p2p.busNvlinkMappingRefcountPerGpu[gpu1Instance]++;
        pKernelBus1->p2p.busNvlinkMappingRefcountPerGpu[gpu0Instance]++;

        if (FLD_TEST_DRF(_P2PAPI, _ATTRIBUTES, _LINK_TYPE, _SPA, attributes))
        {
            pKernelBus0->p2p.busNvlinkMappingRefcountPerPeerId[*peer0]++;
            pKernelBus1->p2p.busNvlinkMappingRefcountPerPeerId[*peer1]++;
        }

        NV_PRINTF(LEVEL_INFO,
                  "- P2P: Peer mapping is already in use for gpu instances %x and %x "
                  "with peer id's %d and %d. Increasing the mapping refcounts for the"
                  " peer IDs to %d and %d respectively.\n",
                  gpu0Instance, gpu1Instance, *peer0, *peer1,
                  pKernelBus0->p2p.busNvlinkMappingRefcountPerPeerId[*peer0],
                  pKernelBus1->p2p.busNvlinkMappingRefcountPerPeerId[*peer1]);

        return NV_OK;
    }

    //
    // Reached here implies the mapping between the given pair of GPUs using the peerIDs
    // *peer0 and *peer1 does not exist. Create the mapping
    //

    // If we're in loopback mode check for specified peer ID arg from RM or MODS
    if (bLoopback && pKernelBus0->p2pMapSpecifyId)
    {
        if ((pKernelBus0->p2p.busNvlinkMappingRefcountPerPeerId[pKernelBus0->p2pMapPeerId] == 0) &&
            (pKernelBus1->p2p.busNvlinkMappingRefcountPerPeerId[pKernelBus1->p2pMapPeerId] == 0))
        {
            *peer0 = *peer1 = pKernelBus0->p2pMapPeerId;
        }
        else
        {
            NV_PRINTF(LEVEL_ERROR,
                      "- ERROR: Peer ID %d is already in use. Default RM P2P mapping "
                      "will be used for loopback connection.\n",
                      pKernelBus0->p2pMapPeerId);
        }
    }

    // Set the peer IDs in the corresponding peer number masks
    pKernelBus0->p2p.busNvlinkPeerNumberMask[gpu1Instance] |= NVBIT(*peer0);
    pKernelBus1->p2p.busNvlinkPeerNumberMask[gpu0Instance] |= NVBIT(*peer1);

    //
    // Increment the mapping refcount per peerID - since there is a new mapping that
    // will use this peerID
    //
    pKernelBus0->p2p.busNvlinkMappingRefcountPerPeerId[*peer0]++;
    pKernelBus1->p2p.busNvlinkMappingRefcountPerPeerId[*peer1]++;

    if (FLD_TEST_DRF(_P2PAPI, _ATTRIBUTES, _LINK_TYPE, _SPA, attributes))
    {
        pKernelBus0->p2p.busNvlinkMappingRefcountPerPeerId[*peer0]++;
        pKernelBus1->p2p.busNvlinkMappingRefcountPerPeerId[*peer1]++;
    }

    //
    // Increment the mapping refcount per GPU - since there a new mapping now to the
    // given remote GPU
    //
    pKernelBus0->p2p.busNvlinkMappingRefcountPerGpu[gpu1Instance]++;
    pKernelBus1->p2p.busNvlinkMappingRefcountPerGpu[gpu0Instance]++;

    NV_PRINTF(LEVEL_INFO,
              "added NVLink P2P mapping between GPU%u (peer %u) and GPU%u (peer %u)\n",
              gpu0Instance, *peer0, gpu1Instance, *peer1);

    portMemSet(&params, 0, sizeof(params));
    params.peerMask = NVBIT(*peer0);
    params.bEnable  = NV_TRUE;

    // Set the NVLink USE_NVLINK_PEER fields in the LTCS registers for GPU0
    status = knvlinkExecGspRmRpc(pGpu0, pKernelNvlink0,
                                 NV2080_CTRL_CMD_NVLINK_ENABLE_NVLINK_PEER,
                                 (void *)&params, sizeof(params));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "GPU%d Failed to ENABLE USE_NVLINK_PEER for peer%d\n",
                  gpuGetInstance(pGpu0), *peer0);

        return status;
    }

    portMemSet(&params, 0, sizeof(params));
    params.peerMask = NVBIT(*peer1);
    params.bEnable  = NV_TRUE;

    // Set the NVLink USE_NVLINK_PEER fields in the LTCS registers for GPU1
    status = knvlinkExecGspRmRpc(pGpu1, pKernelNvlink1,
                                 NV2080_CTRL_CMD_NVLINK_ENABLE_NVLINK_PEER,
                                 (void *)&params, sizeof(params));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "GPU%d Failed to ENABLE USE_NVLINK_PEER for peer%d\n",
                  gpuGetInstance(pGpu1), *peer1);

        return status;
    }

    // Enable the peer configuration in the HSHUB config registers
    knvlinkSetupPeerMapping_HAL(pGpu0, pKernelNvlink0, pGpu1, *peer0);
    knvlinkSetupPeerMapping_HAL(pGpu1, pKernelNvlink1, pGpu0, *peer1);

    return NV_OK;
}

/*!
 * @brief Remove NVLink mapping to a given peer GPU
 *
 * @param[in]   pGpu0           (Local)
 * @param[in]   pKernelBus0     (Local)
 * @param[in]   pGpu1           (Remote)
 * @param[in]   peerId  peerID
 *
 * return NV_OK on success
 */
NV_STATUS
kbusRemoveNvlinkPeerMapping_GP100
(
    OBJGPU    *pGpu0,
    KernelBus *pKernelBus0,
    OBJGPU    *pGpu1,
    NvU32      peerId,
    NvU32      attributes
)
{
    NV_STATUS     status         = NV_OK;
    NvU32         peerGpuInst    = gpuGetInstance(pGpu1);

    // If no peer mapping exists between the GPUs, return NV_WARN_NOTHING_TO_DO
    if ((pKernelBus0->p2p.busNvlinkPeerNumberMask[peerGpuInst] & NVBIT(peerId)) == 0)
    {
        return NV_WARN_NOTHING_TO_DO;
    }

    // A programming error somewhere in RM: mapping exists with a zero refcount
    if ((pKernelBus0->p2p.busNvlinkMappingRefcountPerPeerId[peerId] == 0) ||
        (pKernelBus0->p2p.busNvlinkMappingRefcountPerGpu[peerGpuInst] == 0))
    {
        DBG_BREAKPOINT();
        return NV_ERR_INVALID_STATE;
    }

    // Decrement the mapping refcount associated with the peerID
    pKernelBus0->p2p.busNvlinkMappingRefcountPerPeerId[peerId]--;

    // Decrement the mapping refcount for the given remote GPU1
    pKernelBus0->p2p.busNvlinkMappingRefcountPerGpu[peerGpuInst]--;

    if (FLD_TEST_DRF(_P2PAPI, _ATTRIBUTES, _LINK_TYPE, _SPA, attributes))
    {
        pKernelBus0->p2p.busNvlinkMappingRefcountPerPeerId[peerId]--;
    }

    //
    // If mapping refcount to remote GPU1 is 0, this implies the peerID is no
    // longer used for P2P from GPU0 to GPU1. Update busNvlinkPeerNumberMask
    //
    if (pKernelBus0->p2p.busNvlinkMappingRefcountPerGpu[peerGpuInst] == 0)
    {
        NV_PRINTF(LEVEL_INFO,
                  "Removing mapping for GPU%u peer %u (GPU%u)\n",
                  gpuGetInstance(pGpu0), peerId, peerGpuInst);

        pKernelBus0->p2p.busNvlinkPeerNumberMask[peerGpuInst] &= ~NVBIT(peerId);
    }

    //
    // Can the peerID be freed? The peer ID can *only* be freed if it is not being
    // used for P2P to any GPU. Check the mapping refcount for the given peerID
    //
    if (pKernelBus0->p2p.busNvlinkMappingRefcountPerPeerId[peerId] == 0)
    {
        KernelNvlink *pKernelNvlink0 = GPU_GET_KERNEL_NVLINK(pGpu0);

        NV_ASSERT_OR_RETURN(pKernelNvlink0 != NULL, NV_ERR_NOT_SUPPORTED);
        NV_ASSERT(pKernelBus0->p2p.busNvlinkMappingRefcountPerPeerId[peerId] == 0);
        NV_PRINTF(LEVEL_INFO,
                  "PeerID %u is not being used for P2P from GPU%d to any other "
                  "remote GPU. Can be freed\n",
                  peerId, gpuGetInstance(pGpu0));

        // Before removing the NVLink peer mapping in HSHUB flush both ends
        kbusFlush_HAL(pGpu0, pKernelBus0, BUS_FLUSH_VIDEO_MEMORY);
        kbusFlush_HAL(pGpu1, GPU_GET_KERNEL_BUS(pGpu1), BUS_FLUSH_VIDEO_MEMORY);

        NV2080_CTRL_NVLINK_ENABLE_NVLINK_PEER_PARAMS  params;
        portMemSet(&params, 0, sizeof(params));
        params.peerMask = NVBIT(peerId);
        params.bEnable  = NV_FALSE;

        // Unset the NVLink USE_NVLINK_PEER fields in the LTCS registers for GPU0
        status = knvlinkExecGspRmRpc(pGpu0, pKernelNvlink0,
                                     NV2080_CTRL_CMD_NVLINK_ENABLE_NVLINK_PEER,
                                     (void *)&params, sizeof(params));
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "GPU%d Failed to UNSET USE_NVLINK_PEER for peer%d\n",
                      gpuGetInstance(pGpu0), peerId);

            return status;
        }

        // Disable the peer configuration in the HSHUB config registers
        if ((pKernelNvlink0->getProperty(pKernelNvlink0,
                                         PDB_PROP_KNVLINK_DECONFIG_HSHUB_ON_NO_MAPPING)) &&
            (!knvlinkIsForcedConfig(pGpu0, pKernelNvlink0)))
        {
            status = knvlinkRemoveMapping_HAL(pGpu0, pKernelNvlink0, NV_FALSE, NVBIT(peerId),
                                              NV_FALSE /* bL2Entry */);
            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "GPU%d Failed to remove hshub mapping for peer%d\n",
                          gpuGetInstance(pGpu0), peerId);

                return status;
            }
        }

        // Call knvlinkUpdateCurrentConfig to flush settings to the registers
        status = knvlinkUpdateCurrentConfig(pGpu0, pKernelNvlink0);
    }

    return status;
}

/*!
 * @brief Remove the P2P mapping to a given peer GPU
 *
 * @param[in]   pGpu0          (local GPU)
 * @param[in]   pKernelBus0    (local GPU)
 * @param[in]   pGpu1          (remote GPU)
 * @param[in]   pKernelBus1    (remote GPU)
 * @param[in]   peer0  Peer ID (local to remote)
 * @param[in]   peer1  Peer ID (remote to local)
 * @param[in]   attributes Sepcial attributes for the mapping
 *
 * return NV_OK on success
 */
NV_STATUS
kbusRemoveP2PMapping_GP100
(
    OBJGPU    *pGpu0,
    KernelBus *pKernelBus0,
    OBJGPU    *pGpu1,
    KernelBus *pKernelBus1,
    NvU32      peer0,
    NvU32      peer1,
    NvU32      attributes
)
{
    if (FLD_TEST_DRF(_P2PAPI, _ATTRIBUTES, _CONNECTION_TYPE, _NVLINK, attributes) ||
        FLD_TEST_DRF(_P2PAPI, _ATTRIBUTES, _CONNECTION_TYPE, _NVLINK_INDIRECT, attributes))
    {
        return kbusRemoveP2PMappingForNvlink_HAL(pGpu0, pKernelBus0, pGpu1, pKernelBus1, peer0, peer1, attributes);
    }

    if (FLD_TEST_DRF(_P2PAPI, _ATTRIBUTES, _CONNECTION_TYPE, _PCIE, attributes))
    {
        return kbusRemoveP2PMappingForMailbox_HAL(pGpu0, pKernelBus0, pGpu1, pKernelBus1, peer0, peer1, attributes);
    }

    NV_PRINTF(LEVEL_ERROR, "P2P type %d is not supported\n", DRF_VAL(_P2PAPI, _ATTRIBUTES, _CONNECTION_TYPE, attributes));

    return NV_ERR_NOT_SUPPORTED;
}

/*!
 * @brief Remove the P2P mapping to a given peer GPU
 *
 * @param[in]   pGpu0          (local GPU)
 * @param[in]   pKernelBus0    (local GPU)
 * @param[in]   pGpu1          (remote GPU)
 * @param[in]   pKernelBus1    (remote GPU)
 * @param[in]   peer0          Peer ID (local to remote)
 * @param[in]   peer1          Peer ID (remote to local)
 * @param[in]   attributes     Sepcial attributes for the mapping
 */
NV_STATUS
kbusRemoveP2PMappingForNvlink_GP100
(
    OBJGPU    *pGpu0,
    KernelBus *pKernelBus0,
    OBJGPU    *pGpu1,
    KernelBus *pKernelBus1,
    NvU32      peer0,
    NvU32      peer1,
    NvU32      attributes
)
{
    KernelNvlink *pKernelNvlink0 = GPU_GET_KERNEL_NVLINK(pGpu0);
    KernelNvlink *pKernelNvlink1 = GPU_GET_KERNEL_NVLINK(pGpu1);
    NV_STATUS     status         = NV_OK;

    // If there's no NVLink mapping, fall back to PCIe
    if ((pKernelNvlink0 == NULL) || (pKernelNvlink1 == NULL) ||
        ((pKernelBus0->p2p.busNvlinkPeerNumberMask[pGpu1->gpuInstance] & NVBIT(peer0)) == 0) ||
        ((pKernelBus1->p2p.busNvlinkPeerNumberMask[pGpu0->gpuInstance] & NVBIT(peer1)) == 0))
    {
        return NV_ERR_INVALID_STATE;
    }

    // NVLink mapping exists, remove the NVLink mapping
    NV_ASSERT_OK_OR_RETURN(kbusRemoveNvlinkPeerMapping_HAL(pGpu0, pKernelBus0, pGpu1, peer0, attributes));
    NV_ASSERT_OK_OR_RETURN(kbusRemoveNvlinkPeerMapping_HAL(pGpu1, pKernelBus1, pGpu0, peer1, attributes));

    //
    // The P2P mapping for both the GPUs have been destroyed. If the mapping refcount
    // for the given peer IDs is zero, then unreserve the peer IDs
    //

    if ((pKernelBus0->p2p.busNvlinkMappingRefcountPerPeerId[peer0] == 0) &&
         pKernelNvlink0->getProperty(pKernelNvlink0, PDB_PROP_KNVLINK_DECONFIG_HSHUB_ON_NO_MAPPING))
    {
        // Free the reserved peer ID since its no longer used
        status = kbusUnreserveP2PPeerIds_HAL(pGpu0, pKernelBus0, NVBIT(peer0));
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "GPU%d: Failed to unreserve peer ID mask 0x%x\n",
                      pGpu0->gpuInstance, NVBIT(peer0));
            return status;
        }
    }

    if ((pKernelBus1->p2p.busNvlinkMappingRefcountPerPeerId[peer1] == 0) &&
         pKernelNvlink1->getProperty(pKernelNvlink1, PDB_PROP_KNVLINK_DECONFIG_HSHUB_ON_NO_MAPPING))
    {
        // Free the reserved peer ID since its no longer used
        status = kbusUnreserveP2PPeerIds_HAL(pGpu1, pKernelBus1, NVBIT(peer1));
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "GPU%d: Failed to unreserve peer ID mask 0x%x\n",
                      pGpu1->gpuInstance, NVBIT(peer1));
            return status;
        }
    }

    return status;
}

/*!
 * @brief Returns the peer number from pGpu (Local) to pGpuPeer
 *
 * @param[in] pGpu          Local
 * @param[in] pKernelBus    Local
 * @param[in] pGpuPeer      Remote
 *
 * @returns NvU32 bus peer number
 */
NvU32
kbusGetPeerId_GP100
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    OBJGPU    *pGpuPeer
)
{
    extern NvU32 kbusGetPeerId_GM107(OBJGPU *pGpu, KernelBus *pKernelBus, OBJGPU *pPeerGpu);
    NvU32 gpuPeerInst = gpuGetInstance(pGpuPeer);
    NvU32 peerId = pKernelBus->p2p.busNvlinkPeerNumberMask[gpuPeerInst];

    if (peerId == 0)
    {
        NV_PRINTF(LEVEL_INFO,
                  "NVLINK P2P not set up between GPU%u and GPU%u, checking for PCIe P2P...\n",
                  gpuGetInstance(pGpu), gpuPeerInst);
        return kbusGetPeerId_GM107(pGpu, pKernelBus, pGpuPeer);
    }

    LOWESTBITIDX_32(peerId);
    return peerId;
}

/**
 * @brief      Returns if the given peerId is a valid for a given GPU
 *
 * @param[in]  pGpu
 * @param[in]  pKernelBus
 * @param[in]  peerId  The peer identifier
 *
 * @return     return NV_OK is valid
 */
NV_STATUS
kbusIsPeerIdValid_GP100
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvU32      peerId
)
{
    extern NV_STATUS kbusIsPeerIdValid_GM107(OBJGPU *pGpu, KernelBus *pKernelBus, NvU32 peerId);

    NV_ASSERT_OR_RETURN(peerId < P2P_MAX_NUM_PEERS, NV_ERR_INVALID_INDEX);

    if (pKernelBus->p2p.busNvlinkPeerNumberMask[gpuGetInstance(pGpu)] & NVBIT(peerId))
        return NV_OK;

    return kbusIsPeerIdValid_GM107(pGpu, pKernelBus, peerId);
}

/*!
 * @brief Returns the Nvlink peer ID from pGpu0 to pGpu1
 *
 * @param[in]   pGpu0          (local GPU)
 * @param[in]   pKernelBus0    (local GPU)
 * @param[in]   pGpu1          (remote GPU)
 * @param[in]   pKernelBus1    (remote GPU)
 * @param[out]  nvlinkPeer     NvU32  pointer
 *
 * return NV_OK on success
 */
NV_STATUS
kbusGetNvlinkP2PPeerId_GP100
(
    OBJGPU    *pGpu0,
    KernelBus *pKernelBus0,
    OBJGPU    *pGpu1,
    KernelBus *pKernelBus1,
    NvU32     *nvlinkPeer
)
{
    KernelNvlink *pKernelNvlink0 = GPU_GET_KERNEL_NVLINK(pGpu0);
    NV_STATUS     status         = NV_OK;

    if (nvlinkPeer == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    *nvlinkPeer = BUS_INVALID_PEER;

    //
    // Use the NVLINK-specific unique ID of the GPU (related to link ID) for
    // the peer ID. We expect that this will remain the same across multiple
    // runs, so the peer ID should be consistent.
    //
    // Note: this may not cover peer IDs explicitly requested by the client.
    // It is assumed that explicit peer IDs are only used for NVLINK P2P, and
    // no PCIe P2P will be used.
    //
    if ((pKernelNvlink0 != NULL) &&
        (knvlinkGetPeersNvlinkMaskFromHshub(pGpu0, pKernelNvlink0) != 0))
    {
        if (knvlinkIsForcedConfig(pGpu0, pKernelNvlink0) ||
            pKernelNvlink0->bRegistryLinkOverride)
        {
            *nvlinkPeer = kbusGetPeerIdFromTable_HAL(pGpu0, pKernelBus0,
                                                     pGpu0->gpuInstance,
                                                     pGpu1->gpuInstance);

            if (*nvlinkPeer == BUS_INVALID_PEER)
            {
                return NV_ERR_INVALID_REQUEST;
            }
        }
        else
        {
            *nvlinkPeer = kbusGetPeerId_HAL(pGpu0, pKernelBus0, pGpu1);
            if (*nvlinkPeer != BUS_INVALID_PEER)
            {
                return NV_OK;
            }

            // Reserve GPU0 peer IDs for NVLINK use
            if (!pKernelBus0->p2p.bNvlinkPeerIdsReserved)
            {
                NvU32 idMask = knvlinkGetUniquePeerIdMask_HAL(pGpu0, pKernelNvlink0);

                //
                // If NVLINK is topology is not forced, idMask will be non-zero
                // if nvlinks are detected during topology discovery in core lib
                //
                if (idMask != 0)
                {
                    // Reserve GPU0 peer IDs for NVLINK use
                    status = kbusReserveP2PPeerIds_HAL(pGpu0, pKernelBus0, idMask);
                    if (status != NV_OK)
                    {
                        return status;
                    }
                    pKernelBus0->p2p.bNvlinkPeerIdsReserved = NV_TRUE;
                }
            }
            *nvlinkPeer = knvlinkGetUniquePeerId_HAL(pGpu0, pKernelNvlink0, pGpu1);
        }
    }

    return status;
}
/*!
 * @brief Unreserve peer IDs reserved for nvlink usage
 *
 * @param[in]   pGpu
 * @param[in]   pKernelBus
 * @param[in]   peerMask    Mask of peer IDs to reserve
 *
 * return   NV_OK on success
 */
NV_STATUS
kbusUnreserveP2PPeerIds_GP100
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvU32      peerMask
)
{
    NvU32 peerId = 0;

    FOR_EACH_INDEX_IN_MASK(32, peerId, peerMask)
    {
        if (pKernelBus->p2p.busNvlinkMappingRefcountPerPeerId[peerId] > 0)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "GPU%u: Cannot unreserve peerId %u. Nvlink refcount > 0\n",
                      gpuGetInstance(pGpu), peerId);

            return NV_ERR_IN_USE;
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

    peerId = 0;

    FOR_EACH_INDEX_IN_MASK(32, peerId, peerMask)
    {
        NV_PRINTF(LEVEL_INFO,
                  "Unreserving peer ID %u on GPU%u reserved for NVLINK \n",
                  peerId, gpuGetInstance(pGpu));

        if (pKernelBus->p2pPcie.busPeer[peerId].refCount != 0)
        {
            return NV_ERR_IN_USE;
        }

        pKernelBus->p2pPcie.busPeer[peerId].bReserved = NV_FALSE;
    }
    FOR_EACH_INDEX_IN_MASK_END;

    return NV_OK;
}

/*!
 * @brief  Return the NvLink peer number mask for that peer ID
 *
 * @param[in]   pGpu
 * @param[in]   pKernelBus
 * @param[in]   peerId       peer IDs
 *
 * return   NvU32 Nvlink peer number mask for that peer ID
 */
NvU32
kbusGetNvlinkPeerNumberMask_GP100
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvU32      peerId
)
{
    if (peerId >= NV_MAX_DEVICES)
    {
        NV_PRINTF(LEVEL_ERROR,
            "Invalid peerId value: %d\n", peerId);
        return 0;
    }

    return (pKernelBus->p2p.busNvlinkPeerNumberMask[peerId]);
}
