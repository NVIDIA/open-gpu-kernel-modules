/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/mem_mgr/fermi_dma.h"
#include "kernel/gpu/nvlink/kernel_nvlink.h"
#include "vgpu/vgpu_events.h"
#include "platform/sli/sli.h"

#include "published/pascal/gp100/dev_ram.h"

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

// Grab GPU locks before RPCing into GSP-RM for NVLink RPCs
static NV_STATUS
_kbusExecGspRmRpcForNvlink
(
    OBJGPU       *pGpu,
    NvU32         cmd,
    void         *paramAddr,
    NvU32         paramSize
)
{
    NvU32     gpuMaskRelease = 0;
    NvU32     gpuMaskInitial = rmGpuLocksGetOwnedMask();
    NvU32     gpuMask        = gpuMaskInitial | NVBIT(pGpu->gpuInstance);
    NV_STATUS status         = NV_OK;

    //
    // XXX Bug 1795328: Fix P2P path to acquire locks for the GPU
    //  Due to platform differences in the P2P path, the GPU lock is not
    //  consistently held at this point in the call stack. This function
    //  requires exclusive access to RM/PMU data structures to update HSHUB,
    //  and therefore requires the GPU lock to be held at this point.
    //  This check should be removed once the P2P paths have been updated to
    //  acquire the GPU locks consistently for all platforms.
    //
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

/*!
 * @brief Create NVLink mapping to a given peer GPU
 *
 * @param[in]   pGpu0           (Local)
 * @param[in]   pKernelBus0     (Local)
 * @param[in]   pGpu1           (Remote)
 * @param[in]   peerId          peerID
 * @param[in]   attributes      P2PApi attributes
 *
 * return NV_OK on success
 */
static NV_STATUS
_kbusCreateNvlinkPeerMapping
(
    OBJGPU    *pGpu0,
    KernelBus *pKernelBus0,
    OBJGPU    *pGpu1,
    NvU32      peerId,
    NvU32      attributes
)
{
    NV2080_CTRL_BUS_SET_P2P_MAPPING_PARAMS params;
    NV_STATUS    status          = NV_OK;
    OBJSYS       *pSys           = SYS_GET_INSTANCE();

    NV_ASSERT_OK_OR_RETURN(osAcquireRmSema(pSys->pSema));

    portMemSet(&params, 0, sizeof(params));
    params.connectionType = NV2080_CTRL_CMD_BUS_SET_P2P_MAPPING_CONNECTION_TYPE_NVLINK;
    params.peerId = peerId;
    params.bEgmPeer =  FLD_TEST_DRF(_P2PAPI, _ATTRIBUTES, _REMOTE_EGM, _YES, attributes);
    params.bUseUuid = NV_FALSE;
    params.remoteGpuId = pGpu1->gpuId;
    params.bSpaAccessOnly = FLD_TEST_DRF(_P2PAPI, _ATTRIBUTES, _LINK_TYPE, _SPA, attributes);

    _kbusExecGspRmRpcForNvlink(pGpu0, NV2080_CTRL_CMD_BUS_SET_P2P_MAPPING,
                               &params, sizeof(params));

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "GPU%d NV2080_CTRL_CMD_BUS_SET_P2P_MAPPING failed for peer%d\n",
                  gpuGetInstance(pGpu0), peerId);
    }

    osReleaseRmSema(pSys->pSema, NULL);

    return status;
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
    NV_STATUS     status         = NV_OK;
    NvBool        bEgmPeer       = FLD_TEST_DRF(_P2PAPI, _ATTRIBUTES, _REMOTE_EGM, _YES, attributes);

    if (peer0 == NULL || peer1 == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Set the default RM mapping if peer id's are not explicitly provided
    if (*peer0 == BUS_INVALID_PEER || *peer1 == BUS_INVALID_PEER)
    {
        if (bLoopback && !bEgmPeer)
        {
            if (pKernelBus0->p2pMapSpecifyId)
            {
                *peer0 = *peer1 = pKernelBus0->p2pMapPeerId;
            }
            else
            {
                // If no static mapping is found, set peer id as 0 for loopback
                *peer0 = *peer1 = 0;
                NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                                      kbusReserveP2PPeerIds_HAL(pGpu0, pKernelBus0, NVBIT(0)));
            }

        }
        else
        {
            // Get the peer ID pGpu0 should use for P2P over NVLINK to pGpu1
            if ((status = kbusGetNvlinkP2PPeerId_HAL(pGpu0, pKernelBus0,
                                                     pGpu1, pKernelBus1,
                                                     peer0, attributes)) != NV_OK)
            {
                return status;
            }

            // EGM loopback
            if (bLoopback)
            {
                // The loopback check here becomes true only in the EGM case
                NV_ASSERT_OR_RETURN(bEgmPeer, NV_ERR_INVALID_STATE);
                *peer1 = *peer0;
            }
            else
            // Get the peer ID pGpu1 should use for P2P over NVLINK to pGpu0
            if ((status = kbusGetNvlinkP2PPeerId_HAL(pGpu1, pKernelBus1,
                                                     pGpu0, pKernelBus0,
                                                     peer1, attributes)) != NV_OK)
            {
                return status;
            }

            if (*peer0 == BUS_INVALID_PEER || *peer1 == BUS_INVALID_PEER)
            {
                return NV_ERR_INVALID_REQUEST;
            }
        }

        NV_PRINTF(LEVEL_INFO, "- P2P: Using Default RM mapping for P2P.\n");
    }

    if (bEgmPeer)
    {
        NV_PRINTF(LEVEL_INFO, "EGM peer\n");
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
    if (!bEgmPeer && bLoopback && pKernelBus0->p2pMapSpecifyId)
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

    if (IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu0))
    {
        NV_ASSERT_OK_OR_RETURN(_kbusCreateNvlinkPeerMapping(pGpu0, pKernelBus0, pGpu1, *peer0, attributes));
        NV_ASSERT_OK_OR_RETURN(_kbusCreateNvlinkPeerMapping(pGpu1, pKernelBus1, pGpu0, *peer1, attributes));

        return status;
    }
    else
    {
        KernelNvlink *pKernelNvlink0 = GPU_GET_KERNEL_NVLINK(pGpu0);
        KernelNvlink *pKernelNvlink1 = GPU_GET_KERNEL_NVLINK(pGpu1);
        NV2080_CTRL_NVLINK_ENABLE_NVLINK_PEER_PARAMS params;

        if (pKernelNvlink0 == NULL || pKernelNvlink1 == NULL)
        {
            return NV_ERR_INVALID_REQUEST;
        }

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
static NV_STATUS
_kbusRemoveNvlinkPeerMapping
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
    NvBool        bBufferReady    = NV_FALSE;

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
    // Special case:
    //    Peers connected through NvSwitch in which case all the peers use
    //    peer id 0 and the refcount for peer id 0 wouldn't reach 0 until
    //    P2P between all the peers is destroyed.
    //    busNvlinkMappingRefcountPerGpu == 0 check is done in this case to remove
    //    the peer id from busNvlinkPeerNumberMask[peerGpuInst]
    //
    //    Two peer ids are used to reach the same GPU, one for HBM and one for
    //    EGM. In that case busNvlinkMappingRefcountPerGpu isn't going to
    //    reach 0 until both the peer ids are removed. In this case,
    //    busNvlinkMappingRefcountPerPeerId[peerId] == 0 check is required to
    //    remove the peer id from busNvlinkPeerNumberMask[peerGpuInst].
    //
    if (pKernelBus0->p2p.busNvlinkMappingRefcountPerGpu[peerGpuInst] == 0 ||
        pKernelBus0->p2p.busNvlinkMappingRefcountPerPeerId[peerId] == 0)
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
        NV2080_CTRL_BUS_UNSET_P2P_MAPPING_PARAMS params;
        OBJSYS *pSys = SYS_GET_INSTANCE();

        NV_ASSERT(pKernelBus0->p2p.busNvlinkMappingRefcountPerPeerId[peerId] == 0);
        NV_PRINTF(LEVEL_INFO,
                  "PeerID %u is not being used for P2P from GPU%d to any other "
                  "remote GPU. Can be freed\n",
                  peerId, gpuGetInstance(pGpu0));

        if (pKernelBus0->getProperty(pKernelBus0,
                                          PDB_PROP_KBUS_NVLINK_DECONFIG_HSHUB_ON_NO_MAPPING))
        {
            // Before removing the NVLink peer mapping in HSHUB flush both ends
            kbusFlush_HAL(pGpu0, pKernelBus0, BUS_FLUSH_VIDEO_MEMORY);
            kbusFlush_HAL(pGpu1, GPU_GET_KERNEL_BUS(pGpu1), BUS_FLUSH_VIDEO_MEMORY);
        }

        if (IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu0))
        {
            NV_ASSERT_OK_OR_RETURN(osAcquireRmSema(pSys->pSema));

            portMemSet(&params, 0, sizeof(params));
            params.connectionType = NV2080_CTRL_CMD_BUS_SET_P2P_MAPPING_CONNECTION_TYPE_NVLINK;
            params.peerId = peerId;
            params.bUseUuid = NV_FALSE;
            params.remoteGpuId = pGpu1->gpuId;

            _kbusExecGspRmRpcForNvlink(pGpu0, NV2080_CTRL_CMD_BUS_UNSET_P2P_MAPPING,
                                       &params, sizeof(params));

            osReleaseRmSema(pSys->pSema, NULL);
        }
        else
        {
            KernelNvlink *pKernelNvlink0 = GPU_GET_KERNEL_NVLINK(pGpu0);
            NV2080_CTRL_NVLINK_ENABLE_NVLINK_PEER_PARAMS params;

            NV_ASSERT_OR_RETURN(pKernelNvlink0 != NULL, NV_ERR_NOT_SUPPORTED);

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
            if ((pKernelBus0->getProperty(pKernelBus0,
                                          PDB_PROP_KBUS_NVLINK_DECONFIG_HSHUB_ON_NO_MAPPING)) &&
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

            //
            // Call knvlinkUpdateCurrentConfig to flush settings to the registers
            // Skip this call if buffer ready is set and CONFIG_REQUIRE_INITIALIZED is true
            //
            status = knvlinkSyncLinkMasksAndVbiosInfo(pGpu0, pKernelNvlink0);
            if (status != NV_OK)
            {
                NV_ASSERT(status != NV_OK);
                return status;
            }

            bBufferReady = ((knvlinkGetInitializedLinkMask(pGpu0, pKernelNvlink0) & knvlinkGetPeerLinkMask(pGpu0, pKernelNvlink0, peerId)) != 0) ? NV_TRUE : NV_FALSE;
            if (!pKernelNvlink0->getProperty(pKernelNvlink0, PDB_PROP_KNVLINK_CONFIG_REQUIRE_INITIALIZED_LINKS_CHECK) ||
                !bBufferReady)
            {
                status = knvlinkUpdateCurrentConfig(pGpu0, pKernelNvlink0);
            }
        }
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
    NV_STATUS     status         = NV_OK;

    // If there's no NVLink mapping, fall back to PCIe
    if ((pKernelBus0->p2p.busNvlinkPeerNumberMask[pGpu1->gpuInstance] & NVBIT(peer0)) == 0 ||
        (pKernelBus1->p2p.busNvlinkPeerNumberMask[pGpu0->gpuInstance] & NVBIT(peer1)) == 0)
    {
        return NV_ERR_INVALID_STATE;
    }

    // NVLink mapping exists, remove the NVLink mapping
    NV_ASSERT_OK_OR_RETURN(_kbusRemoveNvlinkPeerMapping(pGpu0, pKernelBus0, pGpu1, peer0, attributes));
    NV_ASSERT_OK_OR_RETURN(_kbusRemoveNvlinkPeerMapping(pGpu1, pKernelBus1, pGpu0, peer1, attributes));

    //
    // The P2P mapping for both the GPUs have been destroyed. If the mapping refcount
    // for the given peer IDs is zero, then unreserve the peer IDs
    //

    if ((pKernelBus0->p2p.busNvlinkMappingRefcountPerPeerId[peer0] == 0) &&
         pKernelBus0->getProperty(pKernelBus0, PDB_PROP_KBUS_NVLINK_DECONFIG_HSHUB_ON_NO_MAPPING))
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
         pKernelBus1->getProperty(pKernelBus1, PDB_PROP_KBUS_NVLINK_DECONFIG_HSHUB_ON_NO_MAPPING))
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
    NvU32     *nvlinkPeer,
    NvU32      attributes
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
            knvlinkAreLinksRegistryOverriden(pGpu0, pKernelNvlink0))
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

        pKernelBus->p2p.bEgmPeer[peerId] = NV_FALSE;
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

/*!
 * @brief Initialize NV_RAMIN_ADR_LIMIT at the given location
 *
 * @param[in] pGpu            OBJGPU
 * @param[in] pKernelBus      KernelBus
 * @param[in] pBar0Wr         Controls whether to MEM_WR using pMap or GPU_REG_WR at addr
 * @param[in] instBlockAddr   physical address of the instance block to be written
 * @param[in] pMap            CPU mapped address to start from
 * @param[in] vaLimit         Virtual address limit to program
 */
void
kbusInstBlkWriteAddrLimit_GP100
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvBool     bBar0Wr,
    NvU64      instBlockAddr,
    NvU8      *pMap,
    NvU64      vaLimit
)
{
    NV_STATUS         status     = NV_OK;
    NvU32             adrLimitLo = (NvU64_LO32(vaLimit) | 0xfff);
    NvU32             adrLimitHi = SF_NUM(_RAMIN_ADR_LIMIT, _HI, NvU64_HI32(vaLimit));

    if (bBar0Wr)
    {
        status = kbusMemAccessBar0Window_HAL(pGpu, pKernelBus,
                              (instBlockAddr + SF_OFFSET(NV_RAMIN_ADR_LIMIT_LO)),
                              &adrLimitLo,
                              sizeof(NvU32),
                              NV_FALSE,
                              pKernelBus->InstBlkAperture);
        NV_ASSERT_OR_RETURN_VOID(NV_OK == status);

        status = kbusMemAccessBar0Window_HAL(pGpu, pKernelBus,
                              (instBlockAddr + SF_OFFSET(NV_RAMIN_ADR_LIMIT_HI)),
                              &adrLimitHi,
                              sizeof(NvU32),
                              NV_FALSE,
                              pKernelBus->InstBlkAperture);
        NV_ASSERT_OR_RETURN_VOID(NV_OK == status);
    }
    else
    {
        MEM_WR32(pMap + SF_OFFSET(NV_RAMIN_ADR_LIMIT_LO), adrLimitLo);
        MEM_WR32(pMap + SF_OFFSET(NV_RAMIN_ADR_LIMIT_HI), adrLimitHi);
    }
}

NV_STATUS
kbusInitInstBlk_GP100
(
    OBJGPU            *pGpu,
    KernelBus         *pKernelBus,
    PMEMORY_DESCRIPTOR pInstBlkMemDesc, // NULL if BAR2 aperture not set up yet: BAR2 instance block using BAR0 window
    PMEMORY_DESCRIPTOR pPDB,
    NvU64              vaLimit,
    NvU64              bigPageSize,
    OBJVASPACE        *pVAS
)
{
    NvU8              *pMap;
    NvBool             newPteFormat      = NV_FALSE;
    NvU32              pageDirBaseHi     = 0;
    NvU32              pageDirBaseTarget = 0;
    NvU32              gfid;

    NV_ASSERT_OK_OR_RETURN(vgpuGetCallingContextGfid(pGpu, &gfid));

    NV_ASSERT_OR_RETURN(pKernelBus->bar2[gfid].pFmt != NULL, NV_ERR_INVALID_STATE);
    newPteFormat = (GMMU_FMT_VERSION_2 == pKernelBus->bar2[gfid].pFmt->version);

    if (pInstBlkMemDesc == NULL)
    {
        if (kbusIsPhysicalBar2InitPagetableEnabled(pKernelBus))
        {
            return kbusSetupBar2InstBlkAtBottomOfFb_HAL(pGpu, pKernelBus, pPDB, vaLimit, bigPageSize, gfid);
        }

        // Initialize NV_RAMIN_ADR_LIMIT at the given offset
        kbusInstBlkWriteAddrLimit_HAL(pGpu, pKernelBus, NV_TRUE,
                                      pKernelBus->bar2[gfid].instBlockBase,
                                      NULL,
                                      vaLimit);

        pageDirBaseHi = SF_NUM(_RAMIN_PAGE_DIR_BASE, _HI, NvU64_HI32(memdescGetPhysAddr(pPDB, AT_GPU, 0)));
        NV_ASSERT_OK_OR_RETURN(kbusMemAccessBar0Window_HAL(pGpu, pKernelBus,
                               (pKernelBus->bar2[gfid].instBlockBase + SF_OFFSET(NV_RAMIN_PAGE_DIR_BASE_HI)),
                               &pageDirBaseHi,
                               sizeof(NvU32),
                               NV_FALSE,
                               pKernelBus->InstBlkAperture));

        //
        // Set up the big page size in the memdesc for this address space
        //
        if (bigPageSize == FERMI_BIG_PAGESIZE_128K)
        {
            pageDirBaseTarget = SF_NUM(_RAMIN_PAGE_DIR_BASE, _TARGET, kgmmuGetHwPteApertureFromMemdesc(GPU_GET_KERNEL_GMMU(pGpu), pPDB)) |
                         (newPteFormat ?
                         SF_DEF(_RAMIN, _USE_NEW_PT_FORMAT, _TRUE) :
                         SF_DEF(_RAMIN, _USE_NEW_PT_FORMAT, _FALSE)) |
                         SF_DEF(_RAMIN, _BIG_PAGE_SIZE, _128KB) |
                         SF_NUM(_RAMIN_PAGE_DIR_BASE, _VOL, memdescGetVolatility(pPDB)) |
                         SF_NUM(_RAMIN_PAGE_DIR_BASE, _LO, NvU64_LO32(memdescGetPhysAddr(pPDB, AT_GPU, 0) >> PDB_SHIFT_FERMI));
        }
        else if (bigPageSize == FERMI_BIG_PAGESIZE_64K)
        {
            pageDirBaseTarget = SF_NUM(_RAMIN_PAGE_DIR_BASE, _TARGET, kgmmuGetHwPteApertureFromMemdesc(GPU_GET_KERNEL_GMMU(pGpu), pPDB)) |
                         (newPteFormat ?
                         SF_DEF(_RAMIN, _USE_NEW_PT_FORMAT, _TRUE) :
                         SF_DEF(_RAMIN, _USE_NEW_PT_FORMAT, _FALSE)) |
                         SF_DEF(_RAMIN, _BIG_PAGE_SIZE, _64KB) |
                         SF_NUM(_RAMIN_PAGE_DIR_BASE, _VOL, memdescGetVolatility(pPDB)) |
                         SF_NUM(_RAMIN_PAGE_DIR_BASE, _LO, NvU64_LO32(memdescGetPhysAddr(pPDB, AT_GPU, 0) >> PDB_SHIFT_FERMI));
        }
        NV_ASSERT_OK_OR_RETURN(kbusMemAccessBar0Window_HAL(pGpu, pKernelBus,
                               (pKernelBus->bar2[gfid].instBlockBase + SF_OFFSET(NV_RAMIN_PAGE_DIR_BASE_TARGET)),
                               &pageDirBaseTarget,
                               sizeof(NvU32),
                               NV_FALSE,
                               pKernelBus->InstBlkAperture));
    }
    else
    {
        SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY)
        pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
        pMap = kbusMapRmAperture_HAL(pGpu, pInstBlkMemDesc);
        if (pMap == NULL)
        {
            SLI_LOOP_RETURN(NV_ERR_INSUFFICIENT_RESOURCES);
        }
        kbusBar2InstBlkWrite_HAL(pGpu, pKernelBus, pMap, pPDB, vaLimit, bigPageSize);
        kbusUnmapRmAperture_HAL(pGpu, pInstBlkMemDesc, &pMap, NV_TRUE);
        if (pKernelBus->bar2[gfid].bMigrating)
        {
            //
            // Remove memdesc from cached mappings as the page tables backing
            // this mapping will be discarded and the old VA to PA translation
            // won't be valid.
            //
            kbusReleaseRmAperture_HAL(pGpu, pKernelBus, pInstBlkMemDesc);
        }
        SLI_LOOP_END
        pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
    }
    return NV_OK;
}

void
kbusBar2InstBlkWrite_GP100
(
    OBJGPU            *pGpu,
    KernelBus         *pKernelBus,
    NvU8              *pMap,
    PMEMORY_DESCRIPTOR pPDB,
    NvU64              vaLimit,
    NvU64              bigPageSize
)
{
    NvBool            newPteFormat = NV_FALSE;

    NV_ASSERT_OR_RETURN_VOID(NULL != pMap);
    NV_ASSERT_OR_RETURN_VOID(NULL != pKernelBus->bar2[GPU_GFID_PF].pFmt);

    newPteFormat = (GMMU_FMT_VERSION_2 == pKernelBus->bar2[GPU_GFID_PF].pFmt->version);

    // Initialize NV_RAMIN_ADR_LIMIT in the mapped instblk
    kbusInstBlkWriteAddrLimit_HAL(pGpu, pKernelBus, NV_FALSE, 0x0, pMap, vaLimit);
    MEM_WR32(pMap + SF_OFFSET(NV_RAMIN_PAGE_DIR_BASE_HI),
                    SF_NUM(_RAMIN_PAGE_DIR_BASE, _HI,
                           NvU64_HI32(memdescGetPhysAddr(pPDB, AT_GPU, 0))));

    if (bigPageSize == FERMI_BIG_PAGESIZE_128K)
    {
        MEM_WR32(pMap + SF_OFFSET(NV_RAMIN_PAGE_DIR_BASE_TARGET),
                        SF_NUM(_RAMIN_PAGE_DIR_BASE, _TARGET, kgmmuGetHwPteApertureFromMemdesc(GPU_GET_KERNEL_GMMU(pGpu), pPDB)) |
                        (newPteFormat ?
                        SF_DEF(_RAMIN, _USE_NEW_PT_FORMAT, _TRUE) :
                        SF_DEF(_RAMIN, _USE_NEW_PT_FORMAT, _FALSE)) |
                        SF_DEF(_RAMIN, _BIG_PAGE_SIZE, _128KB) |
                        SF_NUM(_RAMIN_PAGE_DIR_BASE, _VOL, memdescGetVolatility(pPDB)) |
                        SF_NUM(_RAMIN_PAGE_DIR_BASE, _LO,
                               NvU64_LO32(memdescGetPhysAddr(pPDB, AT_GPU, 0) >> PDB_SHIFT_FERMI)));
    }
    else if (bigPageSize == FERMI_BIG_PAGESIZE_64K)
    {
        MEM_WR32(pMap + SF_OFFSET(NV_RAMIN_PAGE_DIR_BASE_TARGET),
                        SF_NUM(_RAMIN_PAGE_DIR_BASE, _TARGET, kgmmuGetHwPteApertureFromMemdesc(GPU_GET_KERNEL_GMMU(pGpu), pPDB)) |
                        (newPteFormat ?
                        SF_DEF(_RAMIN, _USE_NEW_PT_FORMAT, _TRUE) :
                        SF_DEF(_RAMIN, _USE_NEW_PT_FORMAT, _FALSE)) |
                        SF_DEF(_RAMIN, _BIG_PAGE_SIZE, _64KB) |
                        SF_NUM(_RAMIN_PAGE_DIR_BASE, _VOL, memdescGetVolatility(pPDB)) |
                        SF_NUM(_RAMIN_PAGE_DIR_BASE, _LO,
                               NvU64_LO32(memdescGetPhysAddr(pPDB, AT_GPU, 0) >> PDB_SHIFT_FERMI)));
    }
}

/*!
 * @brief Calculate the base and limit of BAR2 cpu-invisible range
 *
 * @param[in]  pGpu        OBJGPU pointer
 * @param[in]  pKernelBus  KernelBus pointer
 */
void
kbusCalcCpuInvisibleBar2Range_GP100
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvU32      gfid
)
{
    NvU32 cpuInvisibleSize = 0;

    if (pKernelBus->bar2[gfid].cpuInvisibleLimit == 0)
    {
        cpuInvisibleSize = kbusCalcCpuInvisibleBar2ApertureSize_HAL(pGpu, pKernelBus);
        if (cpuInvisibleSize != 0)
        {
            cpuInvisibleSize--;

            if (pKernelBus->bar2[gfid].cpuInvisibleBase == 0)
            {
                if (!RMCFG_FEATURE_PLATFORM_GSP)
                {
                    if (pKernelBus->bar2[gfid].cpuVisibleLimit != 0)
                    {
                        pKernelBus->bar2[gfid].cpuInvisibleBase = pKernelBus->bar2[gfid].cpuVisibleLimit + 1;
                    }
                }
                else
                {
                    pKernelBus->bar2[gfid].cpuInvisibleBase =
                        kbusGetCpuInvisibleBar2BaseAdjust_HAL(pGpu, pKernelBus);
                }
            }
            NV_PRINTF(LEVEL_INFO, "base: 0x%llx size: 0x%x\n",
                      pKernelBus->bar2[gfid].cpuInvisibleBase, cpuInvisibleSize + 1);
            pKernelBus->bar2[gfid].cpuInvisibleLimit = pKernelBus->bar2[gfid].cpuInvisibleBase + cpuInvisibleSize;
        }
    }
}
