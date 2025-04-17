/*
 * SPDX-FileCopyrightText: Copyright (c) 2009-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/device/device.h"
#include "gpu/subdevice/subdevice.h"
#include "gpu/bus/kern_bus.h"
#include "gpu/bus/p2p_api.h"
#include "gpu/bus/third_party_p2p.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "gsp/gspifpub.h"
#include "platform/p2p/p2p_caps.h"
#include "kernel/gpu/nvlink/kernel_nvlink.h"
#include "nvRmReg.h"
#include "rmapi/rs_utils.h"
#include "vgpu/rpc.h"
#include "vgpu/vgpu_events.h"

#include "class/cl503b.h"
#include <class/cl90f1.h> //FERMI_VASPACE_A

#include "gpu/conf_compute/ccsl.h"

#include "spdm/rmspdmvendordef.h"
#include "gpu/spdm/spdm.h"
#include "kernel/gpu/spdm/libspdm_includes.h"
#include "hal/library/cryptlib.h"

/*!
 * @brief Helper function to reserve peer ids in non-GSP offload vGPU case.
 */
static
NV_STATUS
_p2papiReservePeerID
(
    OBJGPU                  *pLocalGpu,
    KernelBus               *pLocalKernelBus,
    OBJGPU                  *pRemoteGpu,
    KernelBus               *pRemoteKernelBus,
    NV503B_ALLOC_PARAMETERS *pNv503bAllocParams,
    P2PApi                  *pP2PApi,
    NvU32                   *peer1,
    NvU32                   *peer2,
    NvBool                   bEgmPeer,
    NvBool                   bSpaAccessOnly
)
{
    NvU32 gpu0Instance = gpuGetInstance(pLocalGpu);
    NvU32 gpu1Instance = gpuGetInstance(pRemoteGpu);

    // loopback request
    if (!bEgmPeer &&
        (pNv503bAllocParams->hSubDevice == pNv503bAllocParams->hPeerSubDevice))
    {
        *peer1 = *peer2 = 0;

        // Reserve the peer1 ID for NVLink use
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                              kbusReserveP2PPeerIds_HAL(pLocalGpu, pLocalKernelBus, NVBIT(*peer1)));
    }
    else
    {
        if (bEgmPeer)
        {
            // Check if a peer ID is already allocated for P2P from pLocalGpu to pRemoteGpu
            *peer1 = kbusGetEgmPeerId_HAL(pLocalGpu, pLocalKernelBus, pRemoteGpu);

            // Check  if a peer ID is already allocated for P2P from pRemoteGpu to pLocalGpu
            *peer2 = kbusGetEgmPeerId_HAL(pRemoteGpu, pRemoteKernelBus, pLocalGpu);
        }
        else
        {
            // Check if a peer ID is already allocated for P2P from pLocalGpu to pRemoteGpu
            *peer1 = kbusGetPeerId_HAL(pLocalGpu, pLocalKernelBus, pRemoteGpu);

            // Check  if a peer ID is already allocated for P2P from pRemoteGpu to pLocalGpu
            *peer2 = kbusGetPeerId_HAL(pRemoteGpu, pRemoteKernelBus, pLocalGpu);
        }
    }

    if (*peer1 != BUS_INVALID_PEER && *peer2 != BUS_INVALID_PEER)
    {
        goto update_mask;
    }
    else if (*peer1 == BUS_INVALID_PEER && *peer2 == BUS_INVALID_PEER)
    {
        // Get the peer ID pGpu0 should use for P2P over NVLINK to pGpu1i
        *peer1 = kbusGetUnusedPeerId_HAL(pLocalGpu, pLocalKernelBus);
        // If could not find a free peer ID, return error
        if (*peer1 == BUS_INVALID_PEER)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "GPU%d: peerID not available for NVLink P2P\n",
                      gpu0Instance);
            return NV_ERR_GENERIC;
        }

        // Reserve the peer ID for NVLink use
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                              kbusReserveP2PPeerIds_HAL(pLocalGpu, pLocalKernelBus, NVBIT(*peer1)));

        if (pNv503bAllocParams->hSubDevice == pNv503bAllocParams->hPeerSubDevice)
        {
            // The loopback check here becomes true only in the EGM case
            NV_ASSERT_OR_RETURN(bEgmPeer, NV_ERR_INVALID_STATE);
            *peer2 = *peer1;
        }
        else
        {
            // Get the peer ID pGpu1 should use for P2P over NVLINK to pGpu0
            *peer2 = kbusGetUnusedPeerId_HAL(pRemoteGpu, pRemoteKernelBus);
            // If could not find a free peer ID, return error
            if (*peer2 == BUS_INVALID_PEER)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "GPU%d: peerID not available for NVLink P2P\n",
                          gpu1Instance);
                return NV_ERR_GENERIC;
            }

            // Reserve the peer ID for NVLink use
            NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                                  kbusReserveP2PPeerIds_HAL(pRemoteGpu, pRemoteKernelBus, NVBIT(*peer2)));
        }
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Unexpected state, either of the peer ID is invalid \n");
        return NV_ERR_GENERIC;
    }

update_mask:
    if (bEgmPeer)
    {
        NV_PRINTF(LEVEL_INFO, "EGM peer\n");
    }

    //
    // Does the mapping already exist between the given pair of GPUs using the peerIDs
    // peer1 and peer2 respectively ?
    //
    if ((pLocalKernelBus->p2p.busNvlinkPeerNumberMask[gpu1Instance] & NVBIT(*peer1)) &&
        (pRemoteKernelBus->p2p.busNvlinkPeerNumberMask[gpu0Instance] & NVBIT(*peer2)))
    {
        //
        // Increment the mapping refcount per peerID - since there is another usage
        // of a mapping that is using this peerID
        //
        pLocalKernelBus->p2p.busNvlinkMappingRefcountPerPeerId[*peer1]++;
        pRemoteKernelBus->p2p.busNvlinkMappingRefcountPerPeerId[*peer2]++;

        //
        // Increment the mapping refcount per GPU - since there is another usage of
        // the mapping to the given remote GPU
        //
        pLocalKernelBus->p2p.busNvlinkMappingRefcountPerGpu[gpu1Instance]++;
        pRemoteKernelBus->p2p.busNvlinkMappingRefcountPerGpu[gpu0Instance]++;

        if (bSpaAccessOnly)
        {
            pLocalKernelBus->p2p.busNvlinkMappingRefcountPerPeerIdSpa[*peer1]++;
            pRemoteKernelBus->p2p.busNvlinkMappingRefcountPerPeerId[*peer2]++;
        }

        NV_PRINTF(LEVEL_INFO,
                  "- P2P: Peer mapping is already in use for gpu instances %x and %x "
                  "with peer id's %d and %d. Increasing the mapping refcounts for the"
                  " peer IDs to %d and %d respectively.\n",
                  gpu0Instance, gpu1Instance, *peer1, *peer2,
                  pLocalKernelBus->p2p.busNvlinkMappingRefcountPerPeerId[*peer1],
                  pRemoteKernelBus->p2p.busNvlinkMappingRefcountPerPeerId[*peer2]);
        goto update_params;
    }

    //
    // Reached here implies the mapping between the given pair of GPUs using the peerIDs
    // peer1 and peer2 does not exist. Create the mapping
    //

    // Set the peer IDs in the corresponding peer number masks
    pLocalKernelBus->p2p.busNvlinkPeerNumberMask[gpu1Instance] |= NVBIT(*peer1);
    pRemoteKernelBus->p2p.busNvlinkPeerNumberMask[gpu0Instance] |= NVBIT(*peer2);

    pLocalKernelBus->p2p.bEgmPeer[*peer1] = bEgmPeer;
    pRemoteKernelBus->p2p.bEgmPeer[*peer2] = bEgmPeer;

    //
    // Increment the mapping refcount per peerID - since there is a new mapping that
    // will use this peerID
    //
    pLocalKernelBus->p2p.busNvlinkMappingRefcountPerPeerId[*peer1]++;
    pRemoteKernelBus->p2p.busNvlinkMappingRefcountPerPeerId[*peer2]++;

    if (bSpaAccessOnly)
    {
        pLocalKernelBus->p2p.busNvlinkMappingRefcountPerPeerIdSpa[*peer1]++;
        pRemoteKernelBus->p2p.busNvlinkMappingRefcountPerPeerId[*peer2]++;
    }

    //
    // Increment the mapping refcount per GPU - since there a new mapping now to the
    // given remote GPU
    //
    pLocalKernelBus->p2p.busNvlinkMappingRefcountPerGpu[gpu1Instance]++;
    pRemoteKernelBus->p2p.busNvlinkMappingRefcountPerGpu[gpu0Instance]++;

    NV_PRINTF(LEVEL_INFO,
              "added NVLink P2P mapping between GPU%u (peer %u) and GPU%u (peer %u)\n",
              gpu0Instance, *peer1, gpu1Instance, *peer2);

update_params:
    if (bEgmPeer)
    {
        pNv503bAllocParams->subDeviceEgmPeerIdMask = NVBIT(*peer1);
        pNv503bAllocParams->peerSubDeviceEgmPeerIdMask = NVBIT(*peer2);
    }
    else
    {
        pNv503bAllocParams->subDevicePeerIdMask = NVBIT(*peer1);
        pNv503bAllocParams->peerSubDevicePeerIdMask = NVBIT(*peer2);
    }

    // Update connection type for SRIOV.
    pP2PApi->attributes = FLD_SET_DRF(_P2PAPI, _ATTRIBUTES, _CONNECTION_TYPE, _NVLINK, pP2PApi->attributes);

    return NV_OK;
}

NV_STATUS
_p2papiUpdateTopologyInfo
(
    P2PApi *pP2PApi,
    OBJGPU *pLocalGpu,
    OBJGPU *pRemoteGpu,
    KernelNvlink *pLocalKernelNvlink,
    KernelNvlink *pRemoteKernelNvlink
)
{
    // Check if identifiers need to be programmed
    if (!pLocalKernelNvlink->bGotNvleIdentifiers)
    {
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, knvlinkEncryptionGetGpuIdentifiers_HAL(pLocalGpu, pLocalKernelNvlink));
        pLocalKernelNvlink->bGotNvleIdentifiers = NV_TRUE;
    }

    if (!pRemoteKernelNvlink->bGotNvleIdentifiers)
    {
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, knvlinkEncryptionGetGpuIdentifiers_HAL(pRemoteGpu, pRemoteKernelNvlink));
        pRemoteKernelNvlink->bGotNvleIdentifiers = NV_TRUE;
    }
    
    // Send topology to key manager
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, 
        knvlinkEncryptionUpdateTopology_HAL(pLocalGpu, pLocalKernelNvlink, pRemoteKernelNvlink->alid, pRemoteKernelNvlink->clid));
    
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, 
        knvlinkEncryptionUpdateTopology_HAL(pRemoteGpu, pRemoteKernelNvlink, pLocalKernelNvlink->alid, pLocalKernelNvlink->clid));

    return NV_OK;
}

NV_STATUS
_p2papiSendEncryptionKeys
(
    OBJGPU *pGpu,
    NvU8   *pKey,
    NvU32   remoteScfDcfGpuId,
    NvBool  bForKeyRotation
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
    NvU8                             wrappingKeyIv[CC_AES_256_GCM_IV_SIZE_BYTES + 1]        = {0};
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
    pGspReqHdr->bForKeyRotation                        = bForKeyRotation;
    pGspReqHdr->wrappedKeyEntries[0].remoteScfDcfGpuId = remoteScfDcfGpuId;
    pGspReqHdr->wrappedKeyEntries[0].bValid            = NV_TRUE;
    portMemCopy(pGspReqHdr->wrappedKeyEntries[0].key, sizeof(pGspReqHdr->wrappedKeyEntries[0].key),
                pKey, RM_GSP_NVLE_AES_256_GCM_KEY_SIZE_BYTES);

    //
    // TODO: Kernel-RM and GSP do not agree on endianness of initial IV value.
    // Additionally, we do not support keeping context of IV incrementing between
    // multiple calls. So for now, require Kernel-RM to always reset the IV to 0 for each call.
    //
    // When providing IV to ccslEncrypt, need to set "freshness" bit after IV itself.
    //
    wrappingKeyIv[CC_AES_256_GCM_IV_SIZE_BYTES] = 0x1;

    // Encrypt the key before sending.
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        ccslEncryptWithIv(pConfCompute->pNvleP2pWrappingCcslCtx,
                          sizeof(pGspReqHdr->wrappedKeyEntries),
                          (NvU8 *)pGspReqHdr->wrappedKeyEntries,
                          wrappingKeyIv, NULL, 0,
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

NV_STATUS
_p2papiDeriveEncryptionKeys
(
    P2PApi *pP2PApi,
    OBJGPU *pLocalGpu,
    OBJGPU *pRemoteGpu,
    KernelNvlink *pLocalKernelNvlink,
    KernelNvlink *pRemoteKernelNvlink
)
{

    NvU8 p2pKey[RM_GSP_NVLE_AES_256_GCM_KEY_SIZE_BYTES] = {0};
    NV_STATUS status = NV_OK;

    if (!libspdm_random_bytes((NvU8 *)&p2pKey, sizeof(p2pKey)))
    {
        return NV_ERR_INVALID_DATA;
    }

    NV_ASSERT_OK_OR_GOTO(status,
        _p2papiSendEncryptionKeys(pLocalGpu,  p2pKey, pRemoteKernelNvlink->clid, NV_FALSE), ErrorExit);
    NV_ASSERT_OK_OR_GOTO(status,
        _p2papiSendEncryptionKeys(pRemoteGpu, p2pKey, pLocalKernelNvlink->clid,  NV_FALSE), ErrorExit);

ErrorExit:
    // Always be sure to scrub P2P key regardless of success
    portMemSet((NvU8 *)&p2pKey, 0, sizeof(p2pKey));

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
    Subdevice               *pSubDevice;
    Subdevice               *pPeerSubDevice;
    NvU32                    subDevicePeerIdMask;
    NvU32                    peerSubDevicePeerIdMask;
    RsClient                *pClient;
    RmClient                *pRmClient;
    NvU32                    peer1;
    NvU32                    peer2;
    NvHandle                 hDevice;
    NvHandle                 hPeerDevice;
    NvHandle                 hSubDevice;
    NvHandle                 hPeerSubDevice;
    OBJGPU                  *pGpu;
    OBJGPU                  *pLocalGpu;
    KernelBus               *pLocalKernelBus;
    KernelNvlink            *pLocalKernelNvlink;
    OBJGPU                  *pRemoteGpu;
    KernelBus               *pRemoteKernelBus;
    KernelNvlink            *pRemoteKernelNvlink;
    NV_STATUS                status;
    NvBool                   bP2PWriteCapable = NV_FALSE;
    NvBool                   bP2PReadCapable = NV_FALSE;
    NV503B_ALLOC_PARAMETERS *pNv503bAllocParams = pParams->pAllocParams;
    NvU32                    flags = pNv503bAllocParams->flags;
    NvBool                   bSpaAccessOnly = FLD_TEST_DRF(503B, _FLAGS, _P2P_TYPE, _SPA, flags);
    P2P_CONNECTIVITY         p2pConnectionType = P2P_CONNECTIVITY_UNKNOWN;
    RM_API                  *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    NV0000_CTRL_SYSTEM_GET_P2P_CAPS_V2_PARAMS *pP2pCapsParams;
    NvU32                    p2pCaps;

    subDevicePeerIdMask     = pNv503bAllocParams->subDevicePeerIdMask;
    peerSubDevicePeerIdMask = pNv503bAllocParams->peerSubDevicePeerIdMask;

    NvU32                        egmPeer1;
    NvU32                        egmPeer2;
    NvU32                        subDeviceEgmPeerIdMask;
    NvU32                        peerSubDeviceEgmPeerIdMask;
    NvBool                       bEgmPeer;

    subDeviceEgmPeerIdMask     = pNv503bAllocParams->subDeviceEgmPeerIdMask;
    peerSubDeviceEgmPeerIdMask = pNv503bAllocParams->peerSubDeviceEgmPeerIdMask;

    pRmClient = dynamicCast(pCallContext->pClient, RmClient);
    if (pRmClient == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    pClient = staticCast(pRmClient, RsClient);

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

    if (pNv503bAllocParams->subDeviceEgmPeerIdMask)
    {
        if (!ONEBITSET(pNv503bAllocParams->subDeviceEgmPeerIdMask))
        {
            return NV_ERR_INVALID_ARGUMENT;
        }
    }

    if (pNv503bAllocParams->peerSubDeviceEgmPeerIdMask)
    {
        if (!ONEBITSET(pNv503bAllocParams->peerSubDeviceEgmPeerIdMask))
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

        if (pNv503bAllocParams->subDeviceEgmPeerIdMask != pNv503bAllocParams->peerSubDeviceEgmPeerIdMask)
        {
            return NV_ERR_INVALID_ARGUMENT;
        }
    }

    hSubDevice = RES_GET_HANDLE(pSubDevice);
    hPeerSubDevice = RES_GET_HANDLE(pPeerSubDevice);

    // Find the gpu for the subdevices passed to us
    pLocalGpu = GPU_RES_GET_GPU(pSubDevice);
    pRemoteGpu = GPU_RES_GET_GPU(pPeerSubDevice);

    // Get device handles
    hDevice = RES_GET_HANDLE(pSubDevice->pDevice);
    hPeerDevice = RES_GET_HANDLE(pPeerSubDevice->pDevice);

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
    pLocalKernelNvlink  = GPU_GET_KERNEL_NVLINK(pLocalGpu);
    pRemoteKernelNvlink = GPU_GET_KERNEL_NVLINK(pRemoteGpu);

    pP2pCapsParams = portMemAllocStackOrHeap(sizeof(*pP2pCapsParams));
    if (pP2pCapsParams == NULL)
    {
        return NV_ERR_NO_MEMORY;
    }

    portMemSet(pP2pCapsParams, 0, sizeof(*pP2pCapsParams));

    pP2pCapsParams->gpuCount = 2;
    pP2pCapsParams->gpuIds[0] = pLocalGpu->gpuId;
    pP2pCapsParams->gpuIds[1] = pRemoteGpu->gpuId;

    NV_CHECK_OK_OR_ELSE(status, LEVEL_ERROR,
                        pRmApi->Control(pRmApi, pClient->hClient, pClient->hClient,
                                        NV0000_CTRL_CMD_SYSTEM_GET_P2P_CAPS_V2,
                                        pP2pCapsParams, sizeof(*pP2pCapsParams)),
                        portMemFreeStackOrHeap(pP2pCapsParams);
                        return status);

    bP2PWriteCapable = REF_VAL(NV0000_CTRL_SYSTEM_GET_P2P_CAPS_WRITES_SUPPORTED, pP2pCapsParams->p2pCaps);
    bP2PReadCapable = REF_VAL(NV0000_CTRL_SYSTEM_GET_P2P_CAPS_READS_SUPPORTED, pP2pCapsParams->p2pCaps);

    p2pCaps = pP2pCapsParams->p2pCaps;

    portMemFreeStackOrHeap(pP2pCapsParams);

    if (REF_VAL(NV0000_CTRL_SYSTEM_GET_P2P_CAPS_C2C_SUPPORTED, p2pCaps))
        p2pConnectionType = P2P_CONNECTIVITY_C2C;
    // It's impossible to detect P2P_CONNECTIVITY_NVLINK_INDIRECT connectivity.
    // There is no difference between NVLINK and NVLINK_INDIRECT in P2PApi creation path,
    // so always use P2P_CONNECTIVITY_NVLINK.
    else if (REF_VAL(NV0000_CTRL_SYSTEM_GET_P2P_CAPS_NVLINK_SUPPORTED, p2pCaps))
        p2pConnectionType = P2P_CONNECTIVITY_NVLINK;
    else if (REF_VAL(NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PCI_BAR1_SUPPORTED, p2pCaps))
        p2pConnectionType = P2P_CONNECTIVITY_PCIE_BAR1;
    else if (REF_VAL(NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PROP_SUPPORTED, p2pCaps))
        p2pConnectionType = P2P_CONNECTIVITY_PCIE_PROPRIETARY;
    else
    {
        NV_PRINTF(LEVEL_ERROR, "Unknown connection type\n");
        return NV_ERR_NOT_SUPPORTED;
    }

    //
    // Allocate P2P PCIE Mailbox areas if all of the following conditions occur:
    // - P2P reads or/and writes are supported
    // - The P2P connection is PCIE Mailbox based
    //
    if ((bP2PWriteCapable || bP2PReadCapable) &&
        p2pConnectionType == P2P_CONNECTIVITY_PCIE_PROPRIETARY)
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

    // Process any specific peer id requests for EGM peer 1
    if (subDeviceEgmPeerIdMask)
    {
        egmPeer1 = BIT_IDX_32(subDeviceEgmPeerIdMask);
    }
    else
    {
        egmPeer1 = BUS_INVALID_PEER;
    }

    // Process any specific peer id requests for EGM peer 2
    if (peerSubDeviceEgmPeerIdMask)
    {
        egmPeer2 = BIT_IDX_32(peerSubDeviceEgmPeerIdMask);
    }
    else
    {
        egmPeer2 = BUS_INVALID_PEER;
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
    if (!IsGP100orBetter(pLocalGpu))
    {
        RS_ORDERED_ITERATOR iter;
        P2PApi *pOtherP2PApi = NULL;

        iter = clientRefOrderedIter(pCallContext->pClient, NULL /*pScoreRef*/,
                                    classId(P2PApi), NV_TRUE /*bExactMatch*/);
        while (clientRefOrderedIterNext(iter.pClient, &iter))
        {
            pOtherP2PApi = dynamicCast(iter.pResourceRef->pResource, P2PApi);
            if (pOtherP2PApi == NULL)
                return NV_ERR_INVALID_OBJECT_HANDLE;

            if (pP2PApi != pOtherP2PApi &&
                ((pLocalGpu == pOtherP2PApi->peer1 && pRemoteGpu == pOtherP2PApi->peer2) ||
                 (pLocalGpu == pOtherP2PApi->peer2 && pRemoteGpu == pOtherP2PApi->peer1)))
            {
                NV_PRINTF(LEVEL_INFO,
                          "Mapping already exists between the two subdevices (0x%08x), (0x%08x). "
                          "Multiple mappings not supported on pre-PASCAL GPUs\n",
                          hSubDevice, hPeerSubDevice);
                return NV_ERR_INVALID_ARGUMENT;
            }
        }
    }

    pP2PApi->peer1 = pLocalGpu;
    pP2PApi->peer2 = pRemoteGpu;
    pP2PApi->attributes  = DRF_NUM(_P2PAPI, _ATTRIBUTES, _CONNECTION_TYPE, p2pConnectionType);
    pP2PApi->attributes |= bSpaAccessOnly ? DRF_DEF(_P2PAPI, _ATTRIBUTES, _LINK_TYPE, _SPA) :
                                            DRF_DEF(_P2PAPI, _ATTRIBUTES, _LINK_TYPE, _GPA);

    bEgmPeer = (!bSpaAccessOnly &&
                memmgrIsLocalEgmEnabled(GPU_GET_MEMORY_MANAGER(pLocalGpu)) &&
                memmgrIsLocalEgmEnabled(GPU_GET_MEMORY_MANAGER(pRemoteGpu)));
                

    if (bSpaAccessOnly &&
        memmgrIsLocalEgmEnabled(GPU_GET_MEMORY_MANAGER(pLocalGpu)) &&
        memmgrIsLocalEgmEnabled(GPU_GET_MEMORY_MANAGER(pRemoteGpu)))
    {
        NV_PRINTF(LEVEL_INFO, "EGM P2P not setup because of SPA only P2P flag!\n");
    }

    // Set the default Bar1 P2P DMA Info
    pNv503bAllocParams->l2pBar1P2PDmaInfo.dma_address = \
        pNv503bAllocParams->p2lBar1P2PDmaInfo.dma_address = NV_U64_MAX;
    pNv503bAllocParams->l2pBar1P2PDmaInfo.dma_size = \
        pNv503bAllocParams->p2lBar1P2PDmaInfo.dma_size = 0;

    if (IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pLocalGpu) || !IS_VIRTUAL(pLocalGpu))
    {
        //
        // TODO: This function need to have a cleanup path when this function
        //       fails after kbusCreateP2PMaping(), busBindLocalGfidForP2P()
        //       and busBindRemoteGfidForP2P(). The current state, the 
        //       function just returns an error. Bug 4016670 filed to track
        //       the effort.
        //

        // setup the p2p resources
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                              kbusCreateP2PMapping_HAL(pLocalGpu, pLocalKernelBus, pRemoteGpu,
                                                       pRemoteKernelBus, &peer1, &peer2,
                                                       pP2PApi->attributes));
        if (bEgmPeer)
        {
            NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                                  kbusCreateP2PMapping_HAL(pLocalGpu, pLocalKernelBus, pRemoteGpu,
                                                           pRemoteKernelBus, &egmPeer1, &egmPeer2,
                                                           pP2PApi->attributes |
                                                           DRF_DEF(_P2PAPI, _ATTRIBUTES, _REMOTE_EGM, _YES)));
        }

        if ((p2pConnectionType == P2P_CONNECTIVITY_PCIE_BAR1) &&
            (pCallContext->secInfo.privLevel >= RS_PRIV_LEVEL_KERNEL))
        {
            NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                                  kbusGetBar1P2PDmaInfo_HAL(pLocalGpu, pRemoteGpu,
                                      pRemoteKernelBus,
                                      &pNv503bAllocParams->l2pBar1P2PDmaInfo.dma_address,
                                      &pNv503bAllocParams->l2pBar1P2PDmaInfo.dma_size));

            NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                                  kbusGetBar1P2PDmaInfo_HAL(pRemoteGpu, pLocalGpu,
                                      pLocalKernelBus, 
                                      &pNv503bAllocParams->p2lBar1P2PDmaInfo.dma_address,
                                      &pNv503bAllocParams->p2lBar1P2PDmaInfo.dma_size));
        }
    }

    pGpu = pLocalGpu;

    if (!IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu) &&
        IS_VIRTUAL_WITH_SRIOV(pGpu) &&
        gpuIsSplitVasManagementServerClientRmEnabled(pGpu))
    {
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                              _p2papiReservePeerID(pLocalGpu, pLocalKernelBus, pRemoteGpu,
                                                   pRemoteKernelBus, pNv503bAllocParams, pP2PApi,
                                                   &peer1, &peer2, NV_FALSE, bSpaAccessOnly));
        if (bEgmPeer)
        {
            NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                                  _p2papiReservePeerID(pLocalGpu, pLocalKernelBus, pRemoteGpu,
                                                       pRemoteKernelBus, pNv503bAllocParams, pP2PApi,
                                                       &egmPeer1, &egmPeer2, NV_TRUE, bSpaAccessOnly));
        }
    }

    pP2PApi->peerId1 = peer1;
    pP2PApi->peerId2 = peer2;
    pP2PApi->egmPeerId1 = egmPeer1;
    pP2PApi->egmPeerId2 = egmPeer2;
    pP2PApi->localGfid = GPU_GFID_PF;
    pP2PApi->remoteGfid = GPU_GFID_PF;

    if (!IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pLocalGpu) && IS_VIRTUAL(pLocalGpu))
    {
        NV_RM_RPC_ALLOC_OBJECT(pLocalGpu,
                               pParams->hClient,
                               pParams->hParent,
                               pParams->hResource,
                               pParams->externalClassId,
                               pNv503bAllocParams,
                               sizeof(*pNv503bAllocParams),
                               status);
        if (status != NV_OK)
            return status;
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
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                              kbusSetupBindFla_HAL(pLocalGpu, pLocalKernelBus, pP2PApi->localGfid));
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
            NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                                  kbusSetupBindFla_HAL(pRemoteGpu, pRemoteKernelBus, pP2PApi->remoteGfid));
        }
    }

    // Adding P2PApi as a dependant of 2 Subdevices, P2PApi must be destroyed before OBJGPU destruction
    NV_ASSERT_OK_OR_RETURN(refAddDependant(RES_GET_REF(pSubDevice), pCallContext->pResourceRef));
    if (hDevice != hPeerDevice)
    {
        NV_ASSERT_OK_OR_RETURN(refAddDependant(RES_GET_REF(pPeerSubDevice), pCallContext->pResourceRef));
    }

    if (status == NV_OK)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, pLocalKernelBus->totalP2pObjectsAliveRefCount < NV_U32_MAX, NV_ERR_INSUFFICIENT_RESOURCES);
        NV_CHECK_OR_RETURN(LEVEL_ERROR, pRemoteKernelBus->totalP2pObjectsAliveRefCount < NV_U32_MAX, NV_ERR_INSUFFICIENT_RESOURCES);
        pLocalKernelBus->totalP2pObjectsAliveRefCount++;
        pRemoteKernelBus->totalP2pObjectsAliveRefCount++;
    }

    // Check if CC is enabled
    if (pLocalKernelNvlink && pRemoteKernelNvlink)
    {
        if (pLocalKernelNvlink->getProperty(pLocalGpu, PDB_PROP_KNVLINK_ENCRYPTION_ENABLED) &&
            pRemoteKernelNvlink->getProperty(pRemoteGpu, PDB_PROP_KNVLINK_ENCRYPTION_ENABLED))
        {
            NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                _p2papiUpdateTopologyInfo(pP2PApi, pLocalGpu, pRemoteGpu, pLocalKernelNvlink, pRemoteKernelNvlink));
            NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                 _p2papiDeriveEncryptionKeys(pP2PApi, pLocalGpu, pRemoteGpu, pLocalKernelNvlink, pRemoteKernelNvlink));
        }
    }

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
    OBJGPU                      *pLocalGpu;
    KernelBus                   *pLocalKernelBus;
    OBJGPU                      *pRemoteGpu;
    KernelBus                   *pRemoteKernelBus;
    RsClient                    *pClient;
    NV_STATUS                    status = NV_OK;

    resGetFreeParams(staticCast(pP2PApi, RsResource), &pCallContext, &pParams);
    hClient = pParams->hClient;

    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
                        serverGetClientUnderLock(&g_resServ, hClient, &pClient), end);

    if (NULL == pP2PApi->peer1 || NULL == pP2PApi->peer2)
        return;

    pLocalGpu = pP2PApi->peer1;
    pRemoteGpu = pP2PApi->peer2;

    pLocalKernelBus  = GPU_GET_KERNEL_BUS(pLocalGpu);
    pRemoteKernelBus = GPU_GET_KERNEL_BUS(pRemoteGpu);

    NV_ASSERT(pLocalKernelBus->totalP2pObjectsAliveRefCount > 0);
    NV_ASSERT(pRemoteKernelBus->totalP2pObjectsAliveRefCount > 0);
    if (pLocalKernelBus->totalP2pObjectsAliveRefCount > 0)
        pLocalKernelBus->totalP2pObjectsAliveRefCount--;

    if (pRemoteKernelBus->totalP2pObjectsAliveRefCount > 0)
        pRemoteKernelBus->totalP2pObjectsAliveRefCount--;

    if (IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pLocalGpu) || !IS_VIRTUAL(pLocalGpu))
    {
        // remove any resources associated with this mapping
        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
                            kbusRemoveP2PMapping_HAL(pLocalGpu, pLocalKernelBus,
                                                     pRemoteGpu, pRemoteKernelBus,
                                                     pP2PApi->peerId1, pP2PApi->peerId2,
                                                     pP2PApi->attributes), end);

        if (!FLD_TEST_DRF(_P2PAPI, _ATTRIBUTES, _LINK_TYPE, _SPA, pP2PApi->attributes) &&
            memmgrIsLocalEgmEnabled(GPU_GET_MEMORY_MANAGER(pLocalGpu)) &&
            memmgrIsLocalEgmEnabled(GPU_GET_MEMORY_MANAGER(pRemoteGpu)))
        {
            status = kbusRemoveP2PMapping_HAL(pLocalGpu, pLocalKernelBus,
                                              pRemoteGpu, pRemoteKernelBus,
                                              pP2PApi->egmPeerId1, pP2PApi->egmPeerId2,
                                              pP2PApi->attributes |
                                              DRF_DEF(_P2PAPI, _ATTRIBUTES, _REMOTE_EGM, _YES));
        }
    }

    pP2PApi->peer1 = NULL;
    pP2PApi->peer2 = NULL;

    kbusUnsetP2PMailboxBar1Area_HAL(pLocalGpu, pLocalKernelBus);
    kbusUnsetP2PMailboxBar1Area_HAL(pRemoteGpu, pRemoteKernelBus);

end:
    pParams->status = status;
}
