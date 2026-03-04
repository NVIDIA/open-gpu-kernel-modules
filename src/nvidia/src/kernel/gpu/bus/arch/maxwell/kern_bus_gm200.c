/*
 * SPDX-FileCopyrightText: Copyright (c) 2006-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "platform/chipset/chipset.h"
#include "mem_mgr/vaspace.h"
#include "gpu_mgr/gpu_mgr.h"
#include "gpu/gpu.h"
#include "gpu/bus/kern_bus.h"
#include "gpu/bus/p2p_api.h"
#include "gpu/bif/kernel_bif.h"

#include "published/maxwell/gm200/dev_nv_p2p.h"

// Defines for PCIE P2P
#define PCIE_P2P_MAX_WRITE_MAILBOX_ADDR                                  \
    ((PCIE_P2P_WRITE_MAILBOX_SIZE << DRF_SIZE(NV_P2P_WMBOX_ADDR_ADDR)) - \
     PCIE_P2P_WRITE_MAILBOX_SIZE)

/*!
 * @brief Setup the mailboxes of 2 GPUs so that the local GPU can access remote GPU.
 *
 * @param[in]   pGpu0          (local GPU)
 * @param[in]   pKernelBus0    (local GPU)
 * @param[in]   pGpu1          (remote GPU)
 * @param[in]   pKernelBus1    (remote GPU)
 * @param[in]   local2Remote   Local peer ID of pRemoteGpu on pLocalGpu
 * @param[in]   remote2Local   Remote peer ID of pLocalGpu on pRemoteGpu
 *
 * @return void
 */
void
kbusSetupMailboxes_GM200
(
    OBJGPU    *pGpu0,
    KernelBus *pKernelBus0,
    OBJGPU    *pGpu1,
    KernelBus *pKernelBus1,
    NvU32      local2Remote,
    NvU32      remote2Local
)
{
    PMEMORY_DESCRIPTOR *ppMemDesc    = NULL;
    RmPhysAddr          localP2PDomainRemoteAddr;
    RmPhysAddr          remoteP2PDomainLocalAddr;
    RmPhysAddr          remoteWMBoxLocalAddr;
    NvU64               remoteWMBoxAddrU64;
    NvBool              bNeedWarBug999673 = kbusNeedWarForBug999673_HAL(pGpu0, pKernelBus0, pGpu1) ||
                                            kbusNeedWarForBug999673_HAL(pGpu1, pKernelBus1, pGpu0);
    RM_API *pRmApi0     = GPU_GET_PHYSICAL_RMAPI(pGpu0);
    RM_API *pRmApi1     = GPU_GET_PHYSICAL_RMAPI(pGpu1);
    NV2080_CTRL_CMD_INTERNAL_BUS_SETUP_P2P_MAILBOX_LOCAL_PARAMS  params0 = {0};
    NV2080_CTRL_CMD_INTERNAL_BUS_SETUP_P2P_MAILBOX_REMOTE_PARAMS params1 = {0};
    NV_STATUS status;

    NV_ASSERT_OR_RETURN_VOID(local2Remote < P2P_MAX_NUM_PEERS);
    NV_ASSERT_OR_RETURN_VOID(remote2Local < P2P_MAX_NUM_PEERS);

    // Ensure we have the correct bidirectional peer mapping
    NV_ASSERT_OR_RETURN_VOID(pKernelBus1->p2pPcie.busPeer[remote2Local].remotePeerId ==
                          local2Remote);
    NV_ASSERT_OR_RETURN_VOID(pKernelBus0->p2pPcie.busPeer[local2Remote].remotePeerId ==
                          remote2Local);

    ppMemDesc = &pKernelBus0->p2pPcie.busPeer[local2Remote].pRemoteWMBoxMemDesc;
    remoteWMBoxLocalAddr = kbusSetupMailboxAccess_HAL(pGpu1, pKernelBus1,
                                                      pGpu0, remote2Local,
                                                      ppMemDesc);
    NV_ASSERT_OR_RETURN_VOID(remoteWMBoxLocalAddr != ~0ULL);

    ppMemDesc = &pKernelBus1->p2pPcie.busPeer[remote2Local].pRemoteP2PDomMemDesc;
    localP2PDomainRemoteAddr = kbusSetupP2PDomainAccess_HAL(pGpu0,
                                                            pKernelBus0,
                                                            pGpu1,
                                                            ppMemDesc);
    NV_ASSERT_OR_RETURN_VOID(localP2PDomainRemoteAddr != ~0ULL);

    ppMemDesc = &pKernelBus0->p2pPcie.busPeer[local2Remote].pRemoteP2PDomMemDesc;
    remoteP2PDomainLocalAddr = kbusSetupP2PDomainAccess_HAL(pGpu1,
                                                            pKernelBus1,
                                                            pGpu0,
                                                            ppMemDesc);
    NV_ASSERT_OR_RETURN_VOID(remoteP2PDomainLocalAddr != ~0ULL);

    // Setup the local GPU to access remote GPU's FB.

    // 0. Set write mail box data window on remote visible GPU to be
    // used by local GPU.
    remoteWMBoxAddrU64 = pKernelBus1->p2pPcie.writeMailboxBar1Addr +
      PCIE_P2P_WRITE_MAILBOX_SIZE * remote2Local;

    // Write mailbox data window needs to be 64KB aligned.
    NV_ASSERT((remoteWMBoxAddrU64 & 0xFFFF) == 0);

    // Setup PCIE P2P Mailbox on local GPU
    params0.local2Remote                = local2Remote;
    params0.remote2Local                = remote2Local;
    params0.localP2PDomainRemoteAddr    = localP2PDomainRemoteAddr;
    params0.remoteP2PDomainLocalAddr    = remoteP2PDomainLocalAddr;
    params0.remoteWMBoxLocalAddr        = remoteWMBoxLocalAddr;
    params0.p2pWmbTag                   = 0;
    params0.bNeedWarBug999673           = bNeedWarBug999673;

    status = pRmApi0->Control(pRmApi0,
                              pGpu0->hInternalClient,
                              pGpu0->hInternalSubdevice,
                              NV2080_CTRL_CMD_INTERNAL_BUS_SETUP_P2P_MAILBOX_LOCAL,
                              &params0,
                              sizeof(NV2080_CTRL_CMD_INTERNAL_BUS_SETUP_P2P_MAILBOX_LOCAL_PARAMS));
    NV_ASSERT(status == NV_OK);

    // Setup PCIE P2P Mailbox on remote GPU
    params1.local2Remote                = local2Remote;
    params1.remote2Local                = remote2Local;
    params1.localP2PDomainRemoteAddr    = localP2PDomainRemoteAddr;
    params1.remoteP2PDomainLocalAddr    = remoteP2PDomainLocalAddr;
    params1.remoteWMBoxAddrU64          = remoteWMBoxAddrU64;
    params1.p2pWmbTag                   = params0.p2pWmbTag;

    status = pRmApi1->Control(pRmApi1,
                              pGpu1->hInternalClient,
                              pGpu1->hInternalSubdevice,
                              NV2080_CTRL_CMD_INTERNAL_BUS_SETUP_P2P_MAILBOX_REMOTE,
                              &params1,
                              sizeof(NV2080_CTRL_CMD_INTERNAL_BUS_SETUP_P2P_MAILBOX_REMOTE_PARAMS));
    NV_ASSERT(status == NV_OK);

    kbusWriteP2PWmbTag_HAL(pGpu1, pKernelBus1, remote2Local, params0.p2pWmbTag);
}

void
kbusWriteP2PWmbTag_GM200
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvU32      remote2Local,
    NvU64      p2pWmbTag
)
{
    // See bug 3558208 comment 34 and 50
    GPU_REG_RD32(pGpu, NV_P2P_WREQMB_L(remote2Local));
    GPU_REG_WR32(pGpu, NV_P2P_WREQMB_L(remote2Local), NvU64_LO32(p2pWmbTag));
    GPU_REG_WR32(pGpu, NV_P2P_WREQMB_H(remote2Local), NvU64_HI32(p2pWmbTag));
}

RmPhysAddr
kbusSetupP2PDomainAccess_GM200
(
    OBJGPU    *pGpu0,
    KernelBus *pKernelBus0,
    OBJGPU    *pGpu1,
    PMEMORY_DESCRIPTOR *ppP2PDomMemDesc
)
{
    return kbusSetupPeerBarAccess(pGpu0, pGpu1,
                pGpu0->busInfo.gpuPhysAddr + DRF_BASE(NV_P2P),
                DRF_SIZE(NV_P2P), ppP2PDomMemDesc);
}

/*!
 * @brief Creates a mapping for the remote peer to access its mailbox in
 *        the local GPU's BAR1
 *
 * @param[in]   pGpu0          (local GPU)
 * @param[in]   pKernelBus0    (local GPU)
 * @param[in]   pGpu1          (remote GPU)
 * @param[in]   local2Remote   Peer ID (local to remote)
 * @param[out]  ppWMBoxMemDesc
 *
 */
RmPhysAddr
kbusSetupMailboxAccess_GM200
(
    OBJGPU    *pGpu0,
    KernelBus *pKernelBus0,
    OBJGPU    *pGpu1,
    NvU32      local2Remote,
    PMEMORY_DESCRIPTOR *ppWMBoxMemDesc
)
{
    return kbusSetupPeerBarAccess(pGpu0, pGpu1,
                gpumgrGetGpuPhysFbAddr(pGpu0) +
                    pKernelBus0->p2pPcie.writeMailboxBar1Addr +
                    PCIE_P2P_WRITE_MAILBOX_SIZE * local2Remote,
                PCIE_P2P_WRITE_MAILBOX_SIZE, ppWMBoxMemDesc);
}

void
kbusDestroyPeerAccess_GM200
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvU32      peerNum
)
{
    memdescDestroy(pKernelBus->p2pPcie.busPeer[peerNum].pRemoteWMBoxMemDesc);
    pKernelBus->p2pPcie.busPeer[peerNum].pRemoteWMBoxMemDesc = NULL;

    memdescDestroy(pKernelBus->p2pPcie.busPeer[peerNum].pRemoteP2PDomMemDesc);
    pKernelBus->p2pPcie.busPeer[peerNum].pRemoteP2PDomMemDesc = NULL;
}

/*!
 * @brief  Returns the P2P mailbox attributes such as size, aligment, max offset.
 *
 * @param[in]   pGpu
 * @param[in]   pKernelBus
 * @param[out]  pMailboxAreaSize       NvU32 pointer. Can be NULL
 * @param[out]  pMailboxAlignment      NvU32 pointer. Can be NULL
 * @param[out]  pMailboxMaxOffset64KB  NvU32 pointer. Can be NULL
 *
 *  Returns the P2P mailbox attributes such as:
 *  - pMailboxAreaSize: total size
 *  - pMailboxAlignment: aligment
 *  - pMailboxMaxOffset: max supported offset
 *
 * return   void
 */
void
kbusGetP2PMailboxAttributes_GM200
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvU32*     pMailboxAreaSize,
    NvU32*     pMailboxAlignmentSize,
    NvU32*     pMailboxBar1MaxOffset64KB
)
{
    KernelBif *pKernelBif = GPU_GET_KERNEL_BIF(pGpu);

    // Initialize null values by default
    if (pMailboxAreaSize != NULL)
    {
        *pMailboxAreaSize = 0;
    }
    if (pMailboxAlignmentSize != NULL)
    {
        *pMailboxAlignmentSize = 0;
    }
    if (pMailboxBar1MaxOffset64KB != NULL)
    {
        *pMailboxBar1MaxOffset64KB = 0;
    }

    if (pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_P2P_READS_DISABLED) &&
        pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_P2P_WRITES_DISABLED))
    {
        // Return null values
        return;
    }

    // Retrieve attributes
    if (pMailboxAreaSize != NULL)
    {
        *pMailboxAreaSize = PCIE_P2P_WRITE_MAILBOX_SIZE * P2P_MAX_NUM_PEERS;
    }

    if (pMailboxAlignmentSize != NULL)
    {
        // Write mailbox data window needs to be 64KB aligned.
        *pMailboxAlignmentSize = 0x10000;
    }

    if (pMailboxBar1MaxOffset64KB != NULL)
    {
        // Max offset, exclusive
        *pMailboxBar1MaxOffset64KB =
            NvU64_LO32(
                (PCIE_P2P_MAX_WRITE_MAILBOX_ADDR + PCIE_P2P_WRITE_MAILBOX_SIZE) >> 16
            );
    }

    return;
}

/*!
 * @brief  Create PCIE Mailbox P2P mapping between 2 GPUs
 *
 * @param[in]   pGpu0          (local GPU)
 * @param[in]   pKernelBus0    (local GPU)
 * @param[in]   pGpu1          (remote GPU)
 * @param[in]   pKernelBus1    (remote GPU)
 * @param[out]  peer0          NvU32 pointer, peerId on pGpu0
 * @param[out]  peer1          NvU32 pointer, peerId on pGpu1
 * @param[in]   attributes     Sepcial attributes for the mapping
 *
 * @return NV_STATUS
 */
NV_STATUS
kbusCreateP2PMapping_GM200
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
    if (FLD_TEST_DRF(_P2PAPI, _ATTRIBUTES, _CONNECTION_TYPE, _PCIE, attributes))
    {
        return kbusCreateP2PMappingForMailbox_HAL(pGpu0, pKernelBus0, pGpu1, pKernelBus1, peer0, peer1, attributes);
    }

    NV_PRINTF(LEVEL_ERROR, "P2P type %d is not supported\n", DRF_VAL(_P2PAPI, _ATTRIBUTES, _CONNECTION_TYPE, attributes));

    return NV_ERR_NOT_SUPPORTED;
}

/*!
 * @brief  Create PCIE (not NVLINK) P2P mapping between 2 GPUs
 *
 * @param[in]   pGpu0          (local GPU)
 * @param[in]   pKernelBus0    (local GPU)
 * @param[in]   pGpu1          (remote GPU)
 * @param[in]   pKernelBus1    (remote GPU)
 * @param[out]  peer0  Peer ID (local to remote)
 * @param[out]  peer1  Peer ID (remote to local)
 * @param[in]   attributes Unused
 *
 * @return NV_STATUS
 */
NV_STATUS
kbusCreateP2PMappingForMailbox_GM200
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
    RM_API *pRmApi;
    NV2080_CTRL_INTERNAL_HSHUB_PEER_CONN_CONFIG_PARAMS params;
    NvU32 gpuInst0, gpuInst1;

    if (IS_VIRTUAL(pGpu0) || IS_VIRTUAL(pGpu1))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    if (peer0 == NULL || peer1 == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    gpuInst0 = gpuGetInstance(pGpu0);
    gpuInst1 = gpuGetInstance(pGpu1);

    // Is a specific peer ID mapping requested?
    if ((*peer0 != BUS_INVALID_PEER) && (*peer1 != BUS_INVALID_PEER))
    {
        NV_ASSERT_OR_RETURN(*peer0 < P2P_MAX_NUM_PEERS, NV_ERR_INVALID_ARGUMENT);
        NV_ASSERT_OR_RETURN(*peer1 < P2P_MAX_NUM_PEERS, NV_ERR_INVALID_ARGUMENT);
        //
        // Ensure that if the requested peer ID is already in use, it
        // corresponds to the requested remote GPU.
        //
        if (!pKernelBus0->p2pPcie.busPeer[*peer0].bReserved &&
            !pKernelBus1->p2pPcie.busPeer[*peer1].bReserved)
        {
            if ((pKernelBus0->p2pPcie.busPeer[*peer0].refCount == 0) &&
                (pKernelBus1->p2pPcie.busPeer[*peer1].refCount == 0))
            {
                goto busCreateP2PMapping_setupMapping;
            }

            if (((pKernelBus0->p2pPcie.peerNumberMask[gpuInst1] & NVBIT(*peer0)) != 0) &&
                ((pKernelBus1->p2pPcie.peerNumberMask[gpuInst0] & NVBIT(*peer1)) != 0))
            {
                pKernelBus0->p2pPcie.busPeer[*peer0].refCount++;
                pKernelBus1->p2pPcie.busPeer[*peer1].refCount++;

                NV_ASSERT(pKernelBus0->p2pPcie.busPeer[*peer0].remotePeerId == *peer1);
                NV_ASSERT(pKernelBus1->p2pPcie.busPeer[*peer1].remotePeerId == *peer0);

                pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu0);
                portMemSet(&params, 0, sizeof(params));
                params.programPciePeerMask = NVBIT32(*peer0);
                NV_ASSERT_OK_OR_RETURN(pRmApi->Control(pRmApi,
                                       pGpu0->hInternalClient,
                                       pGpu0->hInternalSubdevice,
                                       NV2080_CTRL_CMD_INTERNAL_HSHUB_PEER_CONN_CONFIG,
                                       &params,
                                       sizeof(params)));

                pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu1);
                portMemSet(&params, 0, sizeof(params));
                params.programPciePeerMask = NVBIT32(*peer1);
                NV_ASSERT_OK_OR_RETURN(pRmApi->Control(pRmApi,
                                       pGpu1->hInternalClient,
                                       pGpu1->hInternalSubdevice,
                                       NV2080_CTRL_CMD_INTERNAL_HSHUB_PEER_CONN_CONFIG,
                                       &params,
                                       sizeof(params)));

                return NV_OK;
            }
        }

        NV_PRINTF(LEVEL_WARNING,
                  "explicit peer IDs %u and %u requested for GPU%u and GPU%u are not "
                  "available, will assign dynamically\n", *peer0, *peer1,
                  gpuInst0, gpuInst1);
    }

    // Does a mapping already exist between these GPUs?
    if ((pKernelBus0->p2pPcie.peerNumberMask[gpuInst1] != 0) &&
        (pKernelBus1->p2pPcie.peerNumberMask[gpuInst0] != 0))
    {
        *peer0 = pKernelBus0->p2pPcie.peerNumberMask[gpuInst1];
        LOWESTBITIDX_32(*peer0);
        NV_ASSERT_OR_RETURN(*peer0 < P2P_MAX_NUM_PEERS,
                          NV_ERR_INVALID_STATE);

        *peer1 = pKernelBus0->p2pPcie.busPeer[*peer0].remotePeerId;
        NV_ASSERT_OR_RETURN(*peer1 < P2P_MAX_NUM_PEERS,
                          NV_ERR_INVALID_STATE);

        NV_ASSERT_OR_RETURN(pKernelBus1->p2pPcie.busPeer[*peer1].remotePeerId == *peer0,
                          NV_ERR_INVALID_STATE);

        pKernelBus0->p2pPcie.busPeer[*peer0].refCount++;
        pKernelBus1->p2pPcie.busPeer[*peer1].refCount++;

        NV_ASSERT(!pKernelBus0->p2pPcie.busPeer[*peer0].bReserved);
        NV_ASSERT(!pKernelBus1->p2pPcie.busPeer[*peer1].bReserved);

        pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu0);
        portMemSet(&params, 0, sizeof(params));
        params.programPciePeerMask = NVBIT32(*peer0);
        NV_ASSERT_OK_OR_RETURN(pRmApi->Control(pRmApi,
                               pGpu0->hInternalClient,
                               pGpu0->hInternalSubdevice,
                               NV2080_CTRL_CMD_INTERNAL_HSHUB_PEER_CONN_CONFIG,
                               &params,
                               sizeof(params)));

        pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu1);
        portMemSet(&params, 0, sizeof(params));
        params.programPciePeerMask = NVBIT32(*peer1);
        NV_ASSERT_OK_OR_RETURN(pRmApi->Control(pRmApi,
                               pGpu1->hInternalClient,
                               pGpu1->hInternalSubdevice,
                               NV2080_CTRL_CMD_INTERNAL_HSHUB_PEER_CONN_CONFIG,
                               &params,
                               sizeof(params)));

        return NV_OK;
    }

    // We'd better not hit this case (one gpu has mapping and other doesn't).
    NV_ASSERT((pKernelBus0->p2pPcie.peerNumberMask[gpuInst1] == 0) &&
              (pKernelBus1->p2pPcie.peerNumberMask[gpuInst0] == 0));

    *peer0 = BUS_INVALID_PEER;
    *peer1 = BUS_INVALID_PEER;

    // If we're in loopback mode, check for specified peer ID
    if ((pGpu0 == pGpu1) && pKernelBus0->p2pMapSpecifyId)
    {
        if ((pKernelBus0->p2pPcie.busPeer[pKernelBus0->p2pMapPeerId].refCount == 0) &&
            (!pKernelBus0->p2pPcie.busPeer[pKernelBus0->p2pMapPeerId].bReserved) &&
            (pKernelBus1->p2pPcie.busPeer[pKernelBus1->p2pMapPeerId].refCount == 0))
        {
            *peer0 = *peer1 = pKernelBus0->p2pMapPeerId;
        }
        else
        {
           NV_PRINTF(LEVEL_ERROR,
                     "- ERROR: Peer ID %d is already in use. Default RM P2P mapping will be used.\n",
                     pKernelBus0->p2pMapPeerId);
        }
    }

    //
    // These loops will handle loop back (pGpu0 == pGpu1) since they will find
    // the same free peer twice on the same GPU.
    //
    if (*peer0 == BUS_INVALID_PEER)
    {
        *peer0 = kbusGetUnusedPciePeerId_HAL(pGpu0, pKernelBus0);
    }

    if (*peer1 == BUS_INVALID_PEER)
    {
        *peer1 = kbusGetUnusedPciePeerId_HAL(pGpu1, pKernelBus1);
    }

    // couldn't find an available peer on both gpus
    if (*peer0 == BUS_INVALID_PEER ||
            *peer1 == BUS_INVALID_PEER)
    {
        *peer0 = BUS_INVALID_PEER;
        *peer1 = BUS_INVALID_PEER;
        NV_PRINTF(LEVEL_ERROR, "no peer IDs available\n");
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

busCreateP2PMapping_setupMapping:
    pKernelBus0->p2pPcie.busPeer[*peer0].remotePeerId = *peer1;
    pKernelBus0->p2pPcie.peerNumberMask[gpuInst1] |= NVBIT(*peer0);
    pKernelBus1->p2pPcie.busPeer[*peer1].remotePeerId = *peer0;
    pKernelBus1->p2pPcie.peerNumberMask[gpuInst0] |= NVBIT(*peer1);

    NV_ASSERT(pKernelBus0->p2pPcie.busPeer[*peer0].refCount == 0);
    NV_ASSERT(!pKernelBus0->p2pPcie.busPeer[*peer0].bReserved);
    NV_ASSERT(pKernelBus1->p2pPcie.busPeer[*peer1].refCount == 0);
    NV_ASSERT(!pKernelBus1->p2pPcie.busPeer[*peer1].bReserved);

    //
    // Note if this is loopback we will have a refCount of 2.  This will be
    // accounted for in the free.
    //
    pKernelBus0->p2pPcie.busPeer[*peer0].refCount++;
    pKernelBus1->p2pPcie.busPeer[*peer1].refCount++;

    pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu0);
    portMemSet(&params, 0, sizeof(params));
    params.programPciePeerMask = NVBIT32(*peer0);
    NV_ASSERT_OK_OR_RETURN(pRmApi->Control(pRmApi,
                           pGpu0->hInternalClient,
                           pGpu0->hInternalSubdevice,
                           NV2080_CTRL_CMD_INTERNAL_HSHUB_PEER_CONN_CONFIG,
                           &params,
                           sizeof(params)));

    pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu1);
    portMemSet(&params, 0, sizeof(params));
    params.programPciePeerMask = NVBIT32(*peer1);
    NV_ASSERT_OK_OR_RETURN(pRmApi->Control(pRmApi,
                           pGpu1->hInternalClient,
                           pGpu1->hInternalSubdevice,
                           NV2080_CTRL_CMD_INTERNAL_HSHUB_PEER_CONN_CONFIG,
                           &params,
                           sizeof(params)));

    NV_PRINTF(LEVEL_INFO,
              "added PCIe P2P mapping between GPU%u (peer %u) and GPU%u (peer %u)\n",
              gpuInst0, *peer0, gpuInst1, *peer1);

    kbusSetupMailboxes_HAL(pGpu0, pKernelBus0, pGpu1, pKernelBus1, *peer0, *peer1);
    kbusSetupMailboxes_HAL(pGpu1, pKernelBus1, pGpu0, pKernelBus0, *peer1, *peer0);

    return NV_OK;
}

/*!
 * Does it need P2P WAR for bug 999673?
 *
 * @param[in] pGpu
 * @param[in] pKernelBus
 * @param[in] pRemoteGpu
 *
 * @return NvBool
 *
 */
NvBool
kbusNeedWarForBug999673_GM200
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    OBJGPU    *pRemoteGpu
)
{
    OBJCL  *pCl = SYS_GET_CL(SYS_GET_INSTANCE());
    NvU8    pciSwitchBus = 0;

    // Return if WAR is not needed
    if (!pCl->getProperty(pCl, PDB_PROP_CL_BUG_999673_P2P_ARBITRARY_SPLIT_WAR))
    {
        return NV_FALSE;
    }

    // See if we have a known bridge
    clFindCommonDownstreamBR(pGpu, pRemoteGpu, pCl, &pciSwitchBus);
    if (pciSwitchBus != 0xFF)
    {
        // P2P does not go through the chipset needing the WAR.
        return NV_FALSE;
    }

    return NV_TRUE;
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
kbusRemoveP2PMapping_GM200
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
    if (FLD_TEST_DRF(_P2PAPI, _ATTRIBUTES, _CONNECTION_TYPE, _PCIE, attributes))
    {
        return kbusRemoveP2PMappingForMailbox_HAL(pGpu0, pKernelBus0, pGpu1, pKernelBus1, peer0, peer1, attributes);
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
kbusRemoveP2PMappingForMailbox_GM200
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
    NvU32 gpuInst0 = gpuGetInstance(pGpu0);
    NvU32 gpuInst1 = gpuGetInstance(pGpu1);

    if (IS_VIRTUAL(pGpu0) || IS_VIRTUAL(pGpu1))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    // a non-existent mapping
    if(peer0 == BUS_INVALID_PEER ||
       peer1 == BUS_INVALID_PEER)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Do the peer IDs correspond to the expected GPUs?
    if (((pKernelBus0->p2pPcie.peerNumberMask[gpuInst1] & NVBIT(peer0)) == 0) ||
        ((pKernelBus1->p2pPcie.peerNumberMask[gpuInst0] & NVBIT(peer1)) == 0))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    //
    // a programming error somewhere in RM.
    // A mapping exists with a refCount == 0
    //
    if (pKernelBus0->p2pPcie.busPeer[peer0].refCount == 0 ||
        pKernelBus1->p2pPcie.busPeer[peer1].refCount == 0)
    {
        DBG_BREAKPOINT();
        return NV_ERR_GENERIC;
    }

    //
    // Again a programming error.  The mapping should have the same refCount on
    // both GPUs.
    //
    if (pKernelBus0->p2pPcie.busPeer[peer0].refCount !=
        pKernelBus1->p2pPcie.busPeer[peer1].refCount)
    {
        DBG_BREAKPOINT();
        return NV_ERR_GENERIC;
    }


    --pKernelBus1->p2pPcie.busPeer[peer1].refCount;
    if (--pKernelBus0->p2pPcie.busPeer[peer0].refCount == 0)
    {
        NV_PRINTF(LEVEL_INFO,
                  "Removing mapping GPU %d Peer %d <-> GPU %d Peer %d\n",
                  gpuInst0, peer0, gpuInst1, peer1);

        pKernelBus0->p2pPcie.peerNumberMask[gpuInst1] &= ~NVBIT(peer0);
        pKernelBus1->p2pPcie.peerNumberMask[gpuInst0] &= ~NVBIT(peer1);

        kbusDestroyMailbox(pGpu0, pKernelBus0, pGpu1, peer0);
        kbusDestroyMailbox(pGpu1, pKernelBus1, pGpu0, peer1);
    }
    else
    {
        NV_PRINTF(LEVEL_INFO,
                  "Decremented refCount for Mapping GPU %d Peer %d  <-> GPU %d Peer %d "
                  "New Count: %d\n", gpuInst0, peer0, gpuInst1, peer1,
                  pKernelBus0->p2pPcie.busPeer[peer0].refCount);
    }

    return NV_OK;
}

/*!
 * @brief Reserve peer IDs for nvlink usage
 *
 * @param[in]   pGpu
 * @param[in]   pKernelBus
 * @param[in]   peerMask    Mask of peer IDs to reserve
 *
 * return   NV_OK on success
 */
NV_STATUS
kbusReserveP2PPeerIds_GM200
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvU32      peerMask
)
{
    NvU32 peerId = 0;

    FOR_EACH_INDEX_IN_MASK(32, peerId, peerMask)
    {
        NV_PRINTF(LEVEL_INFO,
                  "reserving peer ID %u on GPU%u for NVLINK/C2C use\n", peerId,
                  gpuGetInstance(pGpu));
        if (pKernelBus->p2pPcie.busPeer[peerId].refCount != 0)
        {
            return NV_ERR_IN_USE;
        }

        pKernelBus->p2pPcie.busPeer[peerId].bReserved = NV_TRUE;
    }
    FOR_EACH_INDEX_IN_MASK_END;

    return NV_OK;
}

/*!
 * @brief Sets the BAR1 P2P mailbox address and size
 *
 * @param[in] pGpu
 * @param[in] pKernelBus
 * @param[in] mailboxBar1Addr   NvU64
 * @param[in] mailboxTotalSize  NvU32
 *
 * @returns NV_STATUS
 */
NV_STATUS
kbusSetP2PMailboxBar1Area_GM200
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvU64      mailboxBar1Addr,
    NvU32      mailboxTotalSize
)
{
    NvU32 mailboxAreaSizeReq;
    NvU32 mailboxAlignmentSizeReq;
    NvU32 mailboxBar1MaxOffset64KBReq;


    if (!kbusIsP2pMailboxClientAllocated(pKernelBus))
    {
        // P2P mailbox area already allocated by RM. Nothing to do.
        return NV_OK;
    }

    if (mailboxTotalSize == 0)
    {
        NV_PRINTF(LEVEL_ERROR, "P2P mailbox area size is not set\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    kbusGetP2PMailboxAttributes_HAL(pGpu, pKernelBus, &mailboxAreaSizeReq, &mailboxAlignmentSizeReq, &mailboxBar1MaxOffset64KBReq);

    // Mailbox size
    NV_ASSERT_OR_RETURN(mailboxTotalSize == mailboxAreaSizeReq,                NV_ERR_INVALID_ARGUMENT);
    // Mailbox alignment
    NV_ASSERT_OR_RETURN((mailboxBar1Addr & (mailboxAlignmentSizeReq - 1)) == 0, NV_ERR_INVALID_ARGUMENT);
    // Mailbox  offset limit
    NV_ASSERT_OR_RETURN((mailboxBar1Addr + mailboxTotalSize) < (((NvU64)mailboxBar1MaxOffset64KBReq) << 16),
        NV_ERR_INVALID_ARGUMENT);

    if (pKernelBus->p2pPcie.writeMailboxBar1Addr != PCIE_P2P_INVALID_WRITE_MAILBOX_ADDR)
    {
        NV_ASSERT_OR_RETURN(mailboxBar1Addr == pKernelBus->p2pPcie.writeMailboxBar1Addr, NV_ERR_INVALID_ARGUMENT);
        NV_ASSERT_OR_RETURN(mailboxTotalSize == pKernelBus->p2pPcie.writeMailboxTotalSize, NV_ERR_INVALID_ARGUMENT);
        return NV_OK;
    }

    pKernelBus->p2pPcie.writeMailboxBar1Addr = mailboxBar1Addr;
    pKernelBus->p2pPcie.writeMailboxTotalSize = mailboxTotalSize;
    pKernelBus->bP2pInitialized = NV_TRUE;

    return NV_OK;
}


/*!
 * @brief Unset the BAR1 P2P mailbox address and size
 *
 * @param[in] pGpu
 * @param[in] pKernelBus
 *
 * @returns void
 */
void
kbusUnsetP2PMailboxBar1Area_GM200
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus
)
{
    NvU32 i;

    if (!kbusIsP2pMailboxClientAllocated(pKernelBus))
    {
        // P2P mailbox area already allocated by RM. Nothing to do.
        return;
    }

    for (i = 0; i < P2P_MAX_NUM_PEERS; ++i)
    {
        if (pKernelBus->p2pPcie.busPeer[i].refCount)
            break;
    }

    if (i == P2P_MAX_NUM_PEERS)
    {
        pKernelBus->p2pPcie.writeMailboxBar1Addr  = PCIE_P2P_INVALID_WRITE_MAILBOX_ADDR;
        pKernelBus->p2pPcie.writeMailboxTotalSize = 0;
        pKernelBus->bP2pInitialized = NV_FALSE;
    }

    return;
}

NV_STATUS
kbusAllocP2PMailboxBar1_GM200
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    NvU32      gfid,
    NvU64      vaRangeMax
)
{
    OBJGPU           *pParentGpu;
    NvU64             vaAllocMax;
    NV_STATUS         status = NV_OK;

    VAS_ALLOC_FLAGS flags = {0};

    pParentGpu  = gpumgrGetParentGPU(pGpu);

    if (!gpumgrIsParentGPU(pGpu))
    {
        flags.bFixedAddressAllocate = NV_TRUE;
        pKernelBus->p2pPcie.writeMailboxBar1Addr = GPU_GET_KERNEL_BUS(pParentGpu)->p2pPcie.writeMailboxBar1Addr;
    }

    pKernelBus->p2pPcie.writeMailboxTotalSize =
        PCIE_P2P_WRITE_MAILBOX_SIZE * P2P_MAX_NUM_PEERS;
    vaAllocMax = NV_MIN(vaRangeMax,
        PCIE_P2P_MAX_WRITE_MAILBOX_ADDR + PCIE_P2P_WRITE_MAILBOX_SIZE - 1);

    status = vaspaceAlloc(pKernelBus->bar1[gfid].pVAS,
                          pKernelBus->p2pPcie.writeMailboxTotalSize,
                          PCIE_P2P_WRITE_MAILBOX_SIZE,
                          0, vaAllocMax,
                          0,
                          flags,
                          &pKernelBus->p2pPcie.writeMailboxBar1Addr);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "cannot allocate vaspace for P2P write mailboxes (0x%x)\n",
                  status);
        goto kbusAllocP2PMailboxBar1_failed;
    }

    NV_ASSERT(GPU_GET_KERNEL_BUS(pParentGpu)->p2pPcie.writeMailboxBar1Addr == pKernelBus->p2pPcie.writeMailboxBar1Addr);

    NV_PRINTF(LEVEL_INFO,
              "[GPU%u] P2P write mailboxes allocated at BAR1 addr = 0x%llx\n",
              gpuGetInstance(pGpu), pKernelBus->p2pPcie.writeMailboxBar1Addr);

kbusAllocP2PMailboxBar1_failed:
    if (status != NV_OK)
    {
        pKernelBus->p2pPcie.writeMailboxBar1Addr  = PCIE_P2P_INVALID_WRITE_MAILBOX_ADDR;
        pKernelBus->p2pPcie.writeMailboxTotalSize = 0;
    }

    return status;
}
