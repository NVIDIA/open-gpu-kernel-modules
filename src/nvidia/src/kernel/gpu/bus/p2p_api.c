/*
 * SPDX-FileCopyrightText: Copyright (c) 2009-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/bus/kern_bus.h"
#include "gpu/bus/p2p_api.h"
#include "gpu/bus/third_party_p2p.h"
#include "platform/p2p/p2p_caps.h"
#include "kernel/gpu/nvlink/kernel_nvlink.h"
#include "nvRmReg.h"
#include "rmapi/rs_utils.h"
#include "vgpu/rpc.h"
#include "vgpu/vgpu_events.h"

#include "class/cl503b.h"
#include <class/cl90f1.h> //FERMI_VASPACE_A

/*!
 * @brief Binds local BFID for SR-IOV P2P requests
 * 
 * NOTE: This call will be dispatched to the Physical RM of the
 *       GPU represented by pGpu. Be sure to pass the GPU
 *       you are intending to program (local or remote).
 *
 * @param[in]  pGpu    GPU to dispatch the bind call to
 * @param[in]  gfid    GFID to bind in the P2P source GPU
 * @param[in]  peerId  Peer ID of the P2P destination GPU
 */
static NV_STATUS
s_p2papiBindLocalGfid(OBJGPU *pGpu, NvU32 gfid, NvU32 peerId)
{
    NV_STATUS  status;
    RM_API    *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

    NV2080_CTRL_INTERNAL_BUS_BIND_LOCAL_GFID_FOR_P2P_PARAMS params = {0};

    params.localGfid = gfid;
    params.peerId = peerId;

    status = pRmApi->Control(pRmApi, pGpu->hInternalClient, pGpu->hInternalSubdevice,
                             NV2080_CTRL_CMD_INTERNAL_BUS_BIND_LOCAL_GFID_FOR_P2P,
                             &params, sizeof(params));
    return status;
}

/*!
 * @brief Binds remote GFID for SR-IOV P2P requests
 * 
 * NOTE: This call will be dispatched to the Physical RM of the
 *       GPU represented by pGpu. Be sure to pass the GPU
 *       you are intending to program (local or remote).
 *
 * @param[in]  pGpu    GPU to dispatch the bind call to
 * @param[in]  gfid    GFID to bind in the P2P destination GPU
 */
static NV_STATUS
s_p2papiBindRemoteGfid(OBJGPU *pGpu, NvU32 gfid)
{

    NV_STATUS  status;
    RM_API    *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

    NV2080_CTRL_INTERNAL_BUS_BIND_REMOTE_GFID_FOR_P2P_PARAMS params = {0};

    params.remoteGfid = gfid;

    status = pRmApi->Control(pRmApi, pGpu->hInternalClient, pGpu->hInternalSubdevice,
                             NV2080_CTRL_CMD_INTERNAL_BUS_BIND_REMOTE_GFID_FOR_P2P,
                             &params, sizeof(params));
    return status;
}


NV_STATUS
p2papiConstruct_IMPL
(
    P2PApi                       *pP2PApi,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    NvHandle                 hClient;
    NvHandle                 hP2P;
    Subdevice               *pSubDevice;
    Subdevice               *pPeerSubDevice;
    NvU32                    subDevicePeerIdMask;
    NvU32                    peerSubDevicePeerIdMask;
    RsClient                *pClient;
    NvU32                    peer1;
    NvU32                    peer2;
    NvHandle                 hDevice;
    NvHandle                 hPeerDevice;
    NvHandle                 hSubDevice;
    NvHandle                 hPeerSubDevice;
    PNODE                    pNode;
    OBJGPU                  *pGpu;
    OBJGPU                  *pLocalGpu;
    KernelBus               *pLocalKernelBus;
    KernelNvlink            *pLocalKernelNvlink;
    OBJGPU                  *pRemoteGpu;
    KernelBus               *pRemoteKernelBus;
    KernelNvlink            *pRemoteKernelNvlink;
    NV_STATUS                status;
    NvU32                    gpuMask;
    NvBool                   bP2PWriteCapable = NV_FALSE;
    NvBool                   bP2PReadCapable = NV_FALSE;
    NV503B_ALLOC_PARAMETERS *pNv503bAllocParams = pParams->pAllocParams;
    NvU32                    gfid;
    Device                  *pLocalDevice;
    NvBool                   bRegisteredP2P = NV_FALSE;
    NvBool                   bRegisteredPeerP2P = NV_FALSE;
    NvU32                    flags = pNv503bAllocParams->flags;
    NvBool                   bSpaAccessOnly = FLD_TEST_DRF(503B, _FLAGS, _P2P_TYPE, _SPA, flags);
    P2P_CONNECTIVITY         p2pConnectionType = P2P_CONNECTIVITY_UNKNOWN;

    hClient                 = pParams->hClient;
    hP2P                    = pParams->hResource;
    subDevicePeerIdMask     = pNv503bAllocParams->subDevicePeerIdMask;
    peerSubDevicePeerIdMask = pNv503bAllocParams->peerSubDevicePeerIdMask;

    status = serverGetClientUnderLock(&g_resServ, hClient, &pClient);
    if (status != NV_OK)
        return NV_ERR_INVALID_ARGUMENT;

    status = subdeviceGetByHandle(pClient, pNv503bAllocParams->hSubDevice, &pSubDevice);
    if (status != NV_OK)
        return NV_ERR_INVALID_ARGUMENT;

    status = subdeviceGetByHandle(pClient, pNv503bAllocParams->hPeerSubDevice, &pPeerSubDevice);
    if (status != NV_OK)
        return NV_ERR_INVALID_ARGUMENT;

    if (pNv503bAllocParams->subDevicePeerIdMask)
    {
        if (!ONEBITSET(pNv503bAllocParams->subDevicePeerIdMask))
        {
            return NV_ERR_INVALID_ARGUMENT;
        }
    }

    if (pNv503bAllocParams->peerSubDevicePeerIdMask)
    {
        if (!ONEBITSET(pNv503bAllocParams->peerSubDevicePeerIdMask))
        {
            return NV_ERR_INVALID_ARGUMENT;
        }
    }

    // Ensure any loopback requests match
    if (pNv503bAllocParams->hSubDevice == pNv503bAllocParams->hPeerSubDevice)
    {
        if (pNv503bAllocParams->subDevicePeerIdMask != pNv503bAllocParams->peerSubDevicePeerIdMask)
        {
            return NV_ERR_INVALID_ARGUMENT;
        }
    }

    // validate client
    if (dynamicCast(pClient, RmClient) == NULL)
        return NV_ERR_INVALID_CLIENT;

    hSubDevice = RES_GET_HANDLE(pSubDevice);
    hPeerSubDevice = RES_GET_HANDLE(pPeerSubDevice);

    // Find the gpu for the subdevices passed to us
    if (CliSetSubDeviceContext(hClient, hSubDevice, &hDevice, &pLocalGpu) != NV_OK ||
         NULL == pLocalGpu)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to find GPU for hSubDevice (0x%08x)\n",
                  hSubDevice);
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (CliSetSubDeviceContext(hClient, hPeerSubDevice, &hPeerDevice, &pRemoteGpu) != NV_OK ||
        NULL == pRemoteGpu)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to find GPU for hSubDevice (0x%08x)\n",
                  hPeerSubDevice);
        return NV_ERR_INVALID_ARGUMENT;
    }

    API_GPU_FULL_POWER_SANITY_CHECK(pLocalGpu, NV_TRUE, NV_FALSE);
    API_GPU_FULL_POWER_SANITY_CHECK(pRemoteGpu, NV_TRUE, NV_FALSE);

    if (gpuIsApmFeatureEnabled(pLocalGpu))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    // SPA peer only supported when we support ATS
    if (bSpaAccessOnly && (!pLocalGpu->getProperty(pLocalGpu, PDB_PROP_GPU_ATS_SUPPORTED)))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    pLocalKernelBus  = GPU_GET_KERNEL_BUS(pLocalGpu);
    pRemoteKernelBus = GPU_GET_KERNEL_BUS(pRemoteGpu);

    //
    // Allocate P2P PCIE Mailbox areas if all of the following conditions occur:
    // - P2P reads or/and writes are supported
    // - The P2P connection is PCIE Mailbox based 
    //
    gpuMask = NVBIT(pLocalGpu->gpuInstance) | NVBIT(pRemoteGpu->gpuInstance);
    if ((p2pGetCaps(gpuMask, &bP2PWriteCapable, &bP2PReadCapable, &p2pConnectionType) == NV_OK) &&
        (bP2PWriteCapable || bP2PReadCapable) &&
        (p2pConnectionType == P2P_CONNECTIVITY_PCIE))
    {
        status = kbusSetP2PMailboxBar1Area_HAL(pLocalGpu, pLocalKernelBus,
                                               pNv503bAllocParams->mailboxBar1Addr,
                                               pNv503bAllocParams->mailboxTotalSize);
        NV_ASSERT_OK_OR_RETURN(status);
        
        status = kbusSetP2PMailboxBar1Area_HAL(pRemoteGpu, pRemoteKernelBus,
                                               pNv503bAllocParams->mailboxBar1Addr,
                                               pNv503bAllocParams->mailboxTotalSize);
        NV_ASSERT_OK_OR_RETURN(status);
    }

    // Process any specific peer id requests for peer 1
    if (subDevicePeerIdMask)
    {
        peer1 = BIT_IDX_32(subDevicePeerIdMask);
    }
    else
    {
        peer1 = BUS_INVALID_PEER;
    }

    // Process any specific peer id requests for peer 2
    if (peerSubDevicePeerIdMask)
    {
        peer2 = BIT_IDX_32(peerSubDevicePeerIdMask);
    }
    else
    {
        peer2 = BUS_INVALID_PEER;
    }

    if (!IS_VIRTUAL(pLocalGpu))
    {
        if (!(bP2PWriteCapable || bP2PReadCapable))
        {
            NV_PRINTF(LEVEL_ERROR,
                      "ERROR: P2P is Disabled, cannot create mappings\n");
            return NV_ERR_NOT_SUPPORTED;
        }

        // Train links to high speed.
        pLocalKernelNvlink  = GPU_GET_KERNEL_NVLINK(pLocalGpu);
        pRemoteKernelNvlink = GPU_GET_KERNEL_NVLINK(pRemoteGpu);

        if (pLocalKernelNvlink && pRemoteKernelNvlink)
        {
            status = knvlinkTrainFabricLinksToActive(pLocalGpu, pLocalKernelNvlink);
            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "link training between GPU%u and SWITCH failed with status %x\n",
                          pLocalGpu->gpuInstance, status);
                return status;
            }

            status = knvlinkTrainFabricLinksToActive(pRemoteGpu, pRemoteKernelNvlink);
            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "link training between GPU%u and SWITCH failed with status %x\n",
                          pRemoteGpu->gpuInstance, status);
                return status;
            }
        }
    }

    // check to see if a p2p mapping between these two subdevices already exist
    if (NV_OK == btreeSearch(hPeerSubDevice, &pNode,
                             pSubDevice->pP2PMappingList) &&
        !IsGP100orBetter(pLocalGpu))
    {
        NV_PRINTF(LEVEL_INFO,
                  "Mapping already exists between the two subdevices (0x%08x), (0x%08x). "
                  "Multiple mappings not supported on pre-PASCAL GPUs\n",
                  hSubDevice, hPeerSubDevice);
        return NV_ERR_INVALID_ARGUMENT;
    }

    pP2PApi->Node.keyStart = hP2P;
    pP2PApi->Node.keyEnd = hP2P;
    pP2PApi->Node.Data = pP2PApi;
    pP2PApi->peer1 = pSubDevice;
    pP2PApi->peer2 = pPeerSubDevice;
    pP2PApi->attributes  = DRF_NUM(_P2PAPI, _ATTRIBUTES, _CONNECTION_TYPE, p2pConnectionType);
    pP2PApi->attributes |= bSpaAccessOnly ? DRF_DEF(_P2PAPI, _ATTRIBUTES, _LINK_TYPE, _SPA) :
                                            DRF_DEF(_P2PAPI, _ATTRIBUTES, _LINK_TYPE, _GPA);
    mapInit(&pP2PApi->dmaMappingMap, portMemAllocatorGetGlobalNonPaged());

    // store away the p2pinfo within subdevice info for easy retrieval
    status = subdeviceAddP2PApi(pSubDevice, pP2PApi);
    if (NV_OK != status)
        goto fail;

    bRegisteredP2P = NV_TRUE;

    // for loopback on same subdevice, we only need to store it once
    if (hSubDevice != hPeerSubDevice)
    {
        status = subdeviceAddP2PApi(pPeerSubDevice, pP2PApi);
        if (NV_OK != status)
            goto fail;

        bRegisteredPeerP2P = NV_TRUE;
    }

    if (!IS_VIRTUAL(pLocalGpu))
    {
        // setup the p2p resources
        status = kbusCreateP2PMapping_HAL(pLocalGpu, pLocalKernelBus, pRemoteGpu,
                                          pRemoteKernelBus, &peer1, &peer2,
                                          pP2PApi->attributes);
        if (NV_OK != status)
            goto fail;
    }

    pGpu = pLocalGpu;

    if (IS_VIRTUAL_WITH_SRIOV(pGpu) &&
        gpuIsSplitVasManagementServerClientRmEnabled(pGpu))
    {
        NvU32              gpu0Instance = gpuGetInstance(pLocalGpu);
        NvU32              gpu1Instance = gpuGetInstance(pRemoteGpu);

        // loopback request
        if (pNv503bAllocParams->hSubDevice == pNv503bAllocParams->hPeerSubDevice)
        {
            peer1 = peer2 = 0;
        }
        else
        {
            // Check if a peer ID is already allocated for P2P from pLocalGpu to pRemoteGpu
            peer1 = kbusGetPeerId_HAL(pLocalGpu, pLocalKernelBus, pRemoteGpu);

            // Check  if a peer ID is already allocated for P2P from pRemoteGpu to pLocalGpu
            peer2 = kbusGetPeerId_HAL(pRemoteGpu, pRemoteKernelBus, pLocalGpu);
        }

        if (peer1 != BUS_INVALID_PEER && peer2 != BUS_INVALID_PEER)
        {
            goto update_mask;
        }
        else if (peer1 == BUS_INVALID_PEER && peer2 == BUS_INVALID_PEER)
        {
            // Get the peer ID pGpu0 should use for P2P over NVLINK to pGpu1i
            peer1 = kbusGetUnusedPeerId_HAL(pLocalGpu, pLocalKernelBus);
            // If could not find a free peer ID, return error
            if (peer1 == BUS_INVALID_PEER)
            {
                NV_PRINTF(LEVEL_ERROR,
                           "GPU%d: peerID not available for NVLink P2P\n",
                           gpu0Instance);
                status =  NV_ERR_GENERIC;
                goto fail;
            }

            // Reserve the peer ID for NVLink use
            status = kbusReserveP2PPeerIds_HAL(pLocalGpu, pLocalKernelBus, NVBIT(peer1));
            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR,
                         "Failed to reserve peer1, status=0x%x\n", status);
                goto fail;
            }

            // Get the peer ID pGpu1 should use for P2P over NVLINK to pGpu0
            peer2 = kbusGetUnusedPeerId_HAL(pRemoteGpu, pRemoteKernelBus);
            // If could not find a free peer ID, return error
            if (peer2 == BUS_INVALID_PEER)
            {
                NV_PRINTF(LEVEL_ERROR,
                           "GPU%d: peerID not available for NVLink P2P\n",
                           gpu1Instance);
                status =  NV_ERR_GENERIC;
                goto fail;
            }

            // Reserve the peer ID for NVLink use
            status = kbusReserveP2PPeerIds_HAL(pRemoteGpu, pRemoteKernelBus, NVBIT(peer2));
            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR,
                         "Failed to reserve peer2, status=0x%x\n", status);
                goto fail;
            }
        }
        else
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Unexpected state, either of the peer ID is invalid \n");
            status =  NV_ERR_GENERIC;
            goto fail;
        }

update_mask:
        //
        // Does the mapping already exist between the given pair of GPUs using the peerIDs
        // peer1 and peer2 respectively ?
        //
        if ((pLocalKernelBus->p2p.busNvlinkPeerNumberMask[gpu1Instance] & NVBIT(peer1)) &&
            (pRemoteKernelBus->p2p.busNvlinkPeerNumberMask[gpu0Instance] & NVBIT(peer2)))
        {
            //
            // Increment the mapping refcount per peerID - since there is another usage
            // of a mapping that is using this peerID
            //
            pLocalKernelBus->p2p.busNvlinkMappingRefcountPerPeerId[peer1]++;
            pRemoteKernelBus->p2p.busNvlinkMappingRefcountPerPeerId[peer2]++;

            //
            // Increment the mapping refcount per GPU - since there is another usage of
            // the mapping to the given remote GPU
            //
            pLocalKernelBus->p2p.busNvlinkMappingRefcountPerGpu[gpu1Instance]++;
            pRemoteKernelBus->p2p.busNvlinkMappingRefcountPerGpu[gpu0Instance]++;

            if (bSpaAccessOnly)
            {
                pLocalKernelBus->p2p.busNvlinkMappingRefcountPerPeerIdSpa[peer1]++;
                pRemoteKernelBus->p2p.busNvlinkMappingRefcountPerPeerId[peer2]++;
            }

            NV_PRINTF(LEVEL_INFO,
                      "- P2P: Peer mapping is already in use for gpu instances %x and %x "
                      "with peer id's %d and %d. Increasing the mapping refcounts for the"
                      " peer IDs to %d and %d respectively.\n",
                      gpu0Instance, gpu1Instance, peer1, peer2,
                      pLocalKernelBus->p2p.busNvlinkMappingRefcountPerPeerId[peer1],
                      pRemoteKernelBus->p2p.busNvlinkMappingRefcountPerPeerId[peer2]);

            goto update_params;
        }

        //
        // Reached here implies the mapping between the given pair of GPUs using the peerIDs
        // peer1 and peer2 does not exist. Create the mapping
        //

        // Set the peer IDs in the corresponding peer number masks
        pLocalKernelBus->p2p.busNvlinkPeerNumberMask[gpu1Instance] |= NVBIT(peer1);
        pRemoteKernelBus->p2p.busNvlinkPeerNumberMask[gpu0Instance] |= NVBIT(peer2);

        //
        // Increment the mapping refcount per peerID - since there is a new mapping that
        // will use this peerID
        //
        pLocalKernelBus->p2p.busNvlinkMappingRefcountPerPeerId[peer1]++;
        pRemoteKernelBus->p2p.busNvlinkMappingRefcountPerPeerId[peer2]++;
            
        if (bSpaAccessOnly)
        {
            pLocalKernelBus->p2p.busNvlinkMappingRefcountPerPeerIdSpa[peer1]++;
            pRemoteKernelBus->p2p.busNvlinkMappingRefcountPerPeerId[peer2]++;
        }

        //
        // Increment the mapping refcount per GPU - since there a new mapping now to the
        // given remote GPU
        //
        pLocalKernelBus->p2p.busNvlinkMappingRefcountPerGpu[gpu1Instance]++;
        pRemoteKernelBus->p2p.busNvlinkMappingRefcountPerGpu[gpu0Instance]++;

        NV_PRINTF(LEVEL_INFO,
              "added NVLink P2P mapping between GPU%u (peer %u) and GPU%u (peer %u)\n",
              gpu0Instance, peer1, gpu1Instance, peer2);

update_params:
        pNv503bAllocParams->subDevicePeerIdMask = NVBIT(peer1);
        pNv503bAllocParams->peerSubDevicePeerIdMask = NVBIT(peer2);

        // Update connection type for SRIOV.
        pP2PApi->attributes = FLD_SET_DRF(_P2PAPI, _ATTRIBUTES, _CONNECTION_TYPE, _NVLINK, pP2PApi->attributes);
    }

    pP2PApi->peerId1 = peer1;
    pP2PApi->peerId2 = peer2;

    if (IS_VIRTUAL(pLocalGpu))
    {
        NV_RM_RPC_ALLOC_OBJECT(pLocalGpu,
                               pParams->hClient,
                               pParams->hParent,
                               pParams->hResource,
                               pParams->externalClassId,
                               pNv503bAllocParams,
                               status);
        if (status != NV_OK)
            goto fail;
    }

    //
    // program the GFID for HSHUB when,
    //       1. In hypervisor mode,
    //       2. SRIOV is enabled
    //
    if (!IS_VIRTUAL(pLocalGpu))
    {
        NV_ASSERT_OK(deviceGetByHandle(pClient, hDevice, &pLocalDevice));
        NV_ASSERT_OK_OR_RETURN(vgpuGetGfidFromDeviceInfo(pLocalGpu, pLocalDevice, &gfid));

        {
            if (!bSpaAccessOnly)
            {
                if (gpuIsSriovEnabled(pLocalGpu))
                {
                    NV_PRINTF(LEVEL_INFO, "Trying to register GPU:%x gfid: %x for P2P access with peerId: %x\n",
                              pLocalGpu->deviceInstance, gfid, peer1);
                    NV_ASSERT_OK_OR_RETURN(s_p2papiBindLocalGfid(pLocalGpu, gfid, peer1));
                    if (hSubDevice != hPeerSubDevice)
                    {
                        NV_PRINTF(LEVEL_INFO, "Trying to register GPU:%x gfid: %x for remote access \n",
                                  pLocalGpu->deviceInstance, gfid);

                        NV_ASSERT_OK_OR_RETURN(s_p2papiBindRemoteGfid(pLocalGpu, gfid));
                    }
                }

                if (gpuIsSriovEnabled(pRemoteGpu))
                {
                    if (hDevice != hPeerDevice)
                    {
                        Device *pRemoteDevice;

                        NV_ASSERT_OK(deviceGetByHandle(pClient, hPeerDevice, &pRemoteDevice));
                        NV_ASSERT_OK_OR_RETURN(vgpuGetGfidFromDeviceInfo(pRemoteGpu, pRemoteDevice, &gfid));
                    }

                    if (hSubDevice != hPeerSubDevice)
                    {
                        NV_PRINTF(LEVEL_INFO, "Trying to register GPU:%x gfid: %x for P2P access with peerId: %x\n",
                                  pRemoteGpu->deviceInstance, gfid, peer2);

                        NV_ASSERT_OK_OR_RETURN(s_p2papiBindLocalGfid(pRemoteGpu, gfid, peer2));
                    }

                    NV_PRINTF(LEVEL_INFO, "Trying to register GPU:%x gfid: %x for remote access \n",
                              pRemoteGpu->deviceInstance, gfid);

                    NV_ASSERT_OK_OR_RETURN(s_p2papiBindRemoteGfid(pRemoteGpu, gfid));
                }
            }
        }
    }

    //
    // For SRIOV system, always check for P2P allocation to determine whether
    // this function is allowed to bind FLA
    //
    if (pLocalKernelBus->flaInfo.bFlaAllocated && !pLocalKernelBus->flaInfo.bFlaBind)
    {
        if (!IS_VIRTUAL(pLocalGpu))
        {
            goto remote_fla_bind;
        }
        NV_ASSERT_OK(deviceGetByHandle(pClient, hDevice, &pLocalDevice));
        NV_ASSERT_OK_OR_RETURN(vgpuGetGfidFromDeviceInfo(pLocalGpu, pLocalDevice, &gfid));

        status = kbusSetupBindFla_HAL(pLocalGpu, pLocalKernelBus, gfid);

        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "failed binding instblk for FLA, status=0x%x\n", status);
            goto fail;
        }
    }

remote_fla_bind:

    if (hDevice != hPeerDevice)
    {
        if (pRemoteKernelBus->flaInfo.bFlaAllocated && !pRemoteKernelBus->flaInfo.bFlaBind)
        {
            if (!IS_VIRTUAL(pRemoteGpu))
            {
                return status;
            }
            Device *pRemoteDevice;
            NV_ASSERT_OK(deviceGetByHandle(pClient, hPeerDevice, &pRemoteDevice));
            NV_ASSERT_OK_OR_RETURN(vgpuGetGfidFromDeviceInfo(pRemoteGpu, pRemoteDevice, &gfid));

            status = kbusSetupBindFla_HAL(pRemoteGpu, pRemoteKernelBus, gfid);

            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR,
                        "failed binding instblk for FLA, status=0x%x\n", status);
                goto fail;
            }
        }
    }
    return status;

fail:

    if (bRegisteredPeerP2P)
        subdeviceDelP2PApi(pPeerSubDevice, pP2PApi);

    if (bRegisteredP2P)
        subdeviceDelP2PApi(pSubDevice, pP2PApi);

    return status;
}

void
p2papiDestruct_IMPL
(
    P2PApi *pP2PApi
)
{
    CALL_CONTEXT                *pCallContext;
    RS_RES_FREE_PARAMS_INTERNAL *pParams;
    NvHandle                     hClient;

    resGetFreeParams(staticCast(pP2PApi, RsResource), &pCallContext, &pParams);
    hClient = pParams->hClient;

    // remove any resources associated with this P2P object before freeing it
    pParams->status = CliInvalidateP2PInfo(hClient, pP2PApi);
}

NV_STATUS CliAddP2PDmaMappingInfo
(
    NvHandle              hClient,
    NvHandle              hDevice,
    NvU32                 subDeviceInst,
    NvHandle              hPeerDevice,
    NvU32                 peerSubDeviceInst,
    PCLI_DMA_MAPPING_INFO pDmaMapping
)
{
    PNODE                     pNode;
    NV_STATUS                 status;
    PCLI_P2P_INFO             pP2PInfo = NULL;
    Subdevice                *pSubDevice;
    Subdevice                *pPeerSubDevice;
    PCLI_P2P_DMA_MAPPING_INFO pP2PDmaMappingInfo;
    NvHandle                  hSubDevice, hPeerSubDevice;
    RsClient                 *pClient;

    if (NV_OK != serverGetClientUnderLock(&g_resServ, hClient, &pClient))
        return NV_ERR_INVALID_CLIENT;

    if (NULL == pDmaMapping)
        return NV_ERR_INVALID_ARGUMENT;

    // Find the subdevices for local and peer devices
    status = subdeviceGetByInstance(pClient,
                                    hDevice,
                                    subDeviceInst,
                                    &pSubDevice);
    if ((NV_OK != status) || (NULL == pSubDevice))
        return status;

    hSubDevice = RES_GET_HANDLE(pSubDevice);

    status = subdeviceGetByInstance(pClient,
                                    hPeerDevice,
                                    peerSubDeviceInst,
                                    &pPeerSubDevice);
    if ((NV_OK != status) || (NULL == pPeerSubDevice))
        return status;

    hPeerSubDevice = RES_GET_HANDLE(pPeerSubDevice);

    //
    // Find a P2P object that maps the two subdevices in question, just use the
    // first available. If no such object exists, then this dmaMapping cannot
    // be made.
    //
    status = btreeSearch(hPeerSubDevice, &pNode, pSubDevice->pP2PMappingList);
    if (status == NV_OK)
    {
        PCLI_P2P_INFO_LIST *pP2PInfoList = pNode->Data;

        NV_ASSERT(listHead(pP2PInfoList) != NULL);
        pP2PInfo = *listHead(pP2PInfoList);
    }

    if (pP2PInfo == NULL)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "No P2P mapping between subdevices (0x%08x)and (0x%08x) on the client (0x%08x)\n",
                  hSubDevice, hPeerSubDevice, hClient);
        return status;
    }

    pNode = NULL;

    //
    // It may happen that subdevices on both sides of P2P object will try
    // to map the same virtual address value. We handle this by using
    // CLI_P2P_DMA_MAPPING_INFO to store references to each DMA mapping at the
    // address.
    //
    pP2PDmaMappingInfo = mapFind(&pP2PInfo->dmaMappingMap, pDmaMapping->DmaOffset);
    
    if (pP2PDmaMappingInfo == NULL)
    {
        pP2PDmaMappingInfo = mapInsertNew(&pP2PInfo->dmaMappingMap, pDmaMapping->DmaOffset);
        if (pP2PDmaMappingInfo == NULL)
        {
            return NV_ERR_INSUFFICIENT_RESOURCES;
        }
        portMemSet(pP2PDmaMappingInfo, 0, sizeof(CLI_P2P_DMA_MAPPING_INFO));
    }
    else
    {
        if (pP2PDmaMappingInfo->pPeer1Info != NULL &&
            pP2PDmaMappingInfo->pPeer2Info != NULL)
        {
            NV_PRINTF(LEVEL_ERROR, "P2P DMA mapping is already allocated!\n");
            return NV_ERR_INVALID_REQUEST;
        }
    }

    if (pSubDevice == pP2PInfo->peer1)
    {
        pP2PDmaMappingInfo->pPeer1Info = pDmaMapping;
    }
    else
    {
        NV_ASSERT(pSubDevice == pP2PInfo->peer2);
        pP2PDmaMappingInfo->pPeer2Info = pDmaMapping;
    }

    pDmaMapping->pP2PInfo = pP2PInfo;
    return NV_OK;
}

NV_STATUS CliInvalidateP2PInfo
(
    NvHandle      hClient,
    PCLI_P2P_INFO pP2PInfo
)
{
    OBJGPU    *pLocalGpu;
    KernelBus *pLocalKernelBus;
    OBJGPU    *pRemoteGpu;
    KernelBus *pRemoteKernelBus;
    RsClient  *pClient;
    NvHandle   hSubDevice;
    NvHandle   hPeerSubDevice;
    NvHandle   hDevice;
    NvHandle   hPeerDevice;
    NV_STATUS  status = NV_OK;

    if (NV_OK != serverGetClientUnderLock(&g_resServ, hClient, &pClient))
        return NV_ERR_INVALID_CLIENT;

    if (NULL == pP2PInfo)
        return NV_ERR_INVALID_OBJECT_HANDLE;

    if (NULL == pP2PInfo->peer1 || NULL == pP2PInfo->peer2)
        return NV_OK;

    hSubDevice = RES_GET_HANDLE(pP2PInfo->peer1);
    hPeerSubDevice = RES_GET_HANDLE(pP2PInfo->peer2);

    // Find the gpu for the subdevices of this P2P object
    if (CliSetSubDeviceContext(hClient, hSubDevice, &hDevice, &pLocalGpu) != NV_OK ||
        NULL == pLocalGpu)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to find GPU for hSubDevice (0x%08x)\n",
                  hSubDevice);
        return NV_ERR_INVALID_DEVICE;
    }
    if (CliSetSubDeviceContext(hClient, hPeerSubDevice, &hPeerDevice, &pRemoteGpu) != NV_OK ||
        NULL == pRemoteGpu)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to find GPU for hSubDevice (0x%08x)\n",
                  hPeerSubDevice);
        return NV_ERR_INVALID_DEVICE;
    }

    pLocalKernelBus  = GPU_GET_KERNEL_BUS(pLocalGpu);
    pRemoteKernelBus = GPU_GET_KERNEL_BUS(pRemoteGpu);

    if (!IS_VIRTUAL(pLocalGpu))
    {
        // remove any mailbox resources associated with this mapping
        status = kbusRemoveP2PMapping_HAL(pLocalGpu, pLocalKernelBus,
                                          pRemoteGpu, pRemoteKernelBus,
                                          pP2PInfo->peerId1, pP2PInfo->peerId2,
                                          pP2PInfo->attributes);
    }

    subdeviceDelP2PApi(pP2PInfo->peer1, pP2PInfo);
    if (hSubDevice != hPeerSubDevice)
    {
        subdeviceDelP2PApi(pP2PInfo->peer2, pP2PInfo);
    }

    pP2PInfo->peer1 = NULL;
    pP2PInfo->peer2 = NULL;
    mapDestroy(&pP2PInfo->dmaMappingMap);

    kbusUnsetP2PMailboxBar1Area_HAL(pLocalGpu, pLocalKernelBus);
    kbusUnsetP2PMailboxBar1Area_HAL(pRemoteGpu, pRemoteKernelBus);

    return status;
}

NV_STATUS CliDelP2PDmaMappingInfo
(
    NvHandle              hClient,
    PCLI_DMA_MAPPING_INFO pDmaMapping
)
{
    PCLI_P2P_INFO             pP2PInfo;
    PCLI_P2P_DMA_MAPPING_INFO pP2PDmaMappingInfo;

    NV_ASSERT_OR_RETURN(pDmaMapping != NULL, NV_ERR_INVALID_ARGUMENT);

    pP2PInfo = pDmaMapping->pP2PInfo;
    if (NULL == pP2PInfo || mapCount(&pP2PInfo->dmaMappingMap) == 0)
        return NV_ERR_INVALID_ARGUMENT;

    pP2PDmaMappingInfo = mapFind(&pP2PInfo->dmaMappingMap, pDmaMapping->DmaOffset);
    if (pP2PDmaMappingInfo != NULL)
    {
        if (pP2PDmaMappingInfo->pPeer1Info == pDmaMapping)
        {
            pP2PDmaMappingInfo->pPeer1Info = NULL;
        }
        else if (pP2PDmaMappingInfo->pPeer2Info == pDmaMapping)
        {
            pP2PDmaMappingInfo->pPeer2Info = NULL;
        }

        if (pP2PDmaMappingInfo->pPeer1Info == NULL &&
            pP2PDmaMappingInfo->pPeer2Info == NULL)
        {
            mapRemove(&pP2PInfo->dmaMappingMap, pP2PDmaMappingInfo);
        }
    }

    pDmaMapping->pP2PInfo = NULL;
    return NV_OK;
}

NV_STATUS CliUpdateP2PDmaMappingInList
(
    NvHandle              hClient,
    PCLI_DMA_MAPPING_INFO pDmaMapping,
    NvU64                 dmaOffset
)
{
    PCLI_P2P_INFO             pP2PInfo;
    PCLI_P2P_DMA_MAPPING_INFO pP2PDmaMappingInfo;
    PCLI_P2P_DMA_MAPPING_INFO pNewP2PDmaMappingInfo;
    PCLI_DMA_MAPPING_INFO     pPeer1Info = NULL;
    PCLI_DMA_MAPPING_INFO     pPeer2Info = NULL;

    NV_ASSERT_OR_RETURN(pDmaMapping != NULL, NV_ERR_INVALID_ARGUMENT);

    pP2PInfo = pDmaMapping->pP2PInfo;
    if (NULL == pP2PInfo || mapCount(&pP2PInfo->dmaMappingMap) == 0)
        return NV_ERR_INVALID_ARGUMENT;

    pP2PDmaMappingInfo = mapFind(&pP2PInfo->dmaMappingMap, pDmaMapping->DmaOffset);
    if (pP2PDmaMappingInfo != NULL)
    {
        // Cache the old values
        pPeer1Info  = pP2PDmaMappingInfo->pPeer1Info;
        pPeer2Info  = pP2PDmaMappingInfo->pPeer2Info;

        // free the old dma mapping Info
        mapRemove(&pP2PInfo->dmaMappingMap, pP2PDmaMappingInfo);

        // allocate the new object and insert in the list
        pNewP2PDmaMappingInfo = mapInsertNew(&pP2PInfo->dmaMappingMap, dmaOffset);
        if (pNewP2PDmaMappingInfo == NULL)
        {
            if (mapFind(&pP2PInfo->dmaMappingMap, dmaOffset) != NULL)
            {
                return NV_ERR_INSERT_DUPLICATE_NAME;
            }
            else
            {
                return NV_ERR_INSUFFICIENT_RESOURCES;
            }
        }
        portMemSet(pNewP2PDmaMappingInfo, 0, sizeof(CLI_P2P_DMA_MAPPING_INFO));
        pNewP2PDmaMappingInfo->pPeer1Info = pPeer1Info;
        pNewP2PDmaMappingInfo->pPeer2Info = pPeer2Info;

    }

    return NV_OK;
}

NV_STATUS CliFreeSubDeviceP2PList
(
    Subdevice    *pSubdevice,
    CALL_CONTEXT *pCallContext
)
{
    RsClient           *pRsClient = pCallContext->pClient;
    RsResourceRef      *pResourceRef = pCallContext->pResourceRef;
    PNODE               pNode;
    NV_STATUS           status;
    RM_API             *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    if (pResourceRef == NULL)
        return NV_OK;

    while (NULL != (pNode = pSubdevice->pP2PMappingList))
    {
        PCLI_P2P_INFO_LIST *pP2PInfoList = pNode->Data;
        PCLI_P2P_INFO      *ppP2PInfo;
        PCLI_P2P_INFO      *ppP2PInfoNext;

        for (ppP2PInfo = listHead(pP2PInfoList);
             ppP2PInfo != NULL;
             ppP2PInfo = ppP2PInfoNext)
        {
            ppP2PInfoNext = listNext(pP2PInfoList, ppP2PInfo);

            status = pRmApi->Free(pRmApi, pRsClient->hClient, (NvHandle)(*ppP2PInfo)->Node.keyStart);
            if (NV_OK != status)
            {
                return status;
            }
        }
    }

    return NV_OK;
}
