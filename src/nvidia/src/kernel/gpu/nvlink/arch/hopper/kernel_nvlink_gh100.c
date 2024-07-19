/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/mem_mgr/mem_mgr.h"
#include "nverror.h"
#include "gpu/timer/objtmr.h"
#include "gpu_mgr/gpu_mgr.h"
#include "gpu/gpu_fabric_probe.h"
#include "platform/sli/sli.h"

/*!
 * @brief Check if ALI is supported for the given device
 *
 * @param[in]  pGpu           OBJGPU pointer
 * @param[in]  pKernelNvlink  KernelNvlink pointer
 */
NV_STATUS
knvlinkIsAliSupported_GH100
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    NvU32 status = NV_OK;

    NV2080_CTRL_NVLINK_GET_ALI_ENABLED_PARAMS params;

    portMemSet(&params, 0, sizeof(params));

    // Initialize to default settings
    params.bEnableAli = NV_FALSE;

    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_NVLINK_GET_ALI_ENABLED,
                                 (void *)&params, sizeof(params));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to get ALI enablement status!\n");
        return status;
    }

    pKernelNvlink->bEnableAli = params.bEnableAli;

    return status;
}

/*!
 * @brief   Validates fabric base address.
 *
 * @param[in]  pGpu           OBJGPU pointer
 * @param[in]  pKernelNvlink  KernelNvlink pointer
 * @param[in]  fabricBaseAddr Address to be validated
 *
 * @returns On success, NV_OK.
 *          On failure, returns NV_ERR_XXX.
 */
NV_STATUS
knvlinkValidateFabricBaseAddress_GH100
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvU64         fabricBaseAddr
)
{
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    NvU64          fbSizeBytes;

    fbSizeBytes = pMemoryManager->Ram.fbTotalMemSizeMb << 20;

    //
    // Hopper SKUs will be paired with NVSwitches (Limerock-next) supporting 2K
    // mapslots that can cover 512GB each. Make sure that the fabric base
    // address being used is valid to cover whole frame buffer.
    //

    // Check if fabric address is aligned to mapslot size.
    if (fabricBaseAddr & (NVBIT64(39) - 1))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Align fbSize to mapslot size.
    fbSizeBytes = RM_ALIGN_UP(fbSizeBytes, NVBIT64(39));

    return NV_OK;
}

/*!
 * @brief   Validates fabric EGM base address.
 *
 * @param[in]  pGpu              OBJGPU pointer
 * @param[in]  pKernelNvlink     KernelNvlink pointer
 * @param[in]  fabricEgmBaseAddr Address to be validated
 *
 * @returns On success, NV_OK.
 *          On failure, returns NV_ERR_XXX.
 */
NV_STATUS
knvlinkValidateFabricEgmBaseAddress_GH100
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvU64         fabricEgmBaseAddr
)
{
    //
    // Hopper SKUs will be paired with NVSwitches supporting 2K
    // mapslots that can cover 512GB each. Make sure that the EGM fabric base
    // address being used is valid to cover whole frame buffer.
    //

    // Check if fabric EGM address is aligned to mapslot size.
    if (fabricEgmBaseAddr & (NVBIT64(39) - 1))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    return NV_OK;
}

/*!
 * @brief Do post setup on nvlink peers
 *
 * @param[in] pGpu           OBJGPU pointer
 * @param[in] pKernelNvlink  KernelNvlink pointer
 */
NV_STATUS
knvlinkPostSetupNvlinkPeer_GH100
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    NvU32 status = NV_OK;
    NV2080_CTRL_NVLINK_POST_SETUP_NVLINK_PEER_PARAMS postSetupNvlinkPeerParams;

    portMemSet(&postSetupNvlinkPeerParams, 0, sizeof(postSetupNvlinkPeerParams));

    postSetupNvlinkPeerParams.peerMask = (1 << NVLINK_MAX_PEERS_SW) - 1;

    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_NVLINK_POST_SETUP_NVLINK_PEER,
                                 (void *)&postSetupNvlinkPeerParams,
                                 sizeof(postSetupNvlinkPeerParams));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to program post active settings and bufferready!\n");
        return status;
    }

    return status;
}

/*!
 * @brief Discover all links that are training or have been
 *        trained on both GPUs
 *
 * @param[in] pGpu           OBJGPU pointer for local GPU
 * @param[in] pKernelNvlink  KernelNvlink pointer
 * @param[in] pPeerGpu       OBJGPU pointer for remote GPU
 *
 * @return  NV_OK if links are detected to be training
 */
NV_STATUS
knvlinkDiscoverPostRxDetLinks_GH100
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    OBJGPU       *pPeerGpu
)
{
    NV_STATUS status = NV_ERR_NOT_SUPPORTED;

#if defined(INCLUDE_NVLINK_LIB)

    OBJGPU       *pGpu0          = pGpu;
    OBJGPU       *pGpu1          = pPeerGpu;
    KernelNvlink *pKernelNvlink0 = GPU_GET_KERNEL_NVLINK(pGpu0);
    KernelNvlink *pKernelNvlink1 = NULL;

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

    if ((IS_RTLSIM(pGpu) && !pKernelNvlink0->bForceEnableCoreLibRtlsims) ||
        (pKernelNvlink0->pNvlinkDev == NULL)                             ||
        !pKernelNvlink0->bEnableAli                                      ||
        (pKernelNvlink1->pNvlinkDev == NULL)                             ||
        !pKernelNvlink1->bEnableAli)
    {
        NV_PRINTF(LEVEL_INFO,
                "Not in ALI, checking PostRxDetLinks not supported.\n");
        return NV_ERR_NOT_SUPPORTED;
    }

    //
    // Initialize Mask of links that have made it past RxDet to 0 then
    // request to get all links from the given GPU that have gotted past RxDet
    //
    pKernelNvlink0->postRxDetLinkMask = 0;
    status = knvlinkUpdatePostRxDetectLinkMask(pGpu0, pKernelNvlink0);
    if(status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Getting peer0's postRxDetLinkMask failed!\n");
        return NV_ERR_INVALID_STATE;
    }

    // Only query if we are not in loopback
    if (pKernelNvlink0 != pKernelNvlink1)
    {
        pKernelNvlink1->postRxDetLinkMask = 0;
        status = knvlinkUpdatePostRxDetectLinkMask(pGpu1, pKernelNvlink1);
        if(status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Getting peer1's postRxDetLinkMask failed!\n");
            return NV_ERR_INVALID_STATE;
        }
    }

    //
    // If the current gpu has no actively training or trained link OR
    // if the peer gpu has no actively training or trained links then
    // return an error. If either side has 0 links passed RxDet then
    // there is no chance that we will find links connecting the devices
    // further into discovery.
    //
    if(pKernelNvlink0->postRxDetLinkMask == 0 ||
       pKernelNvlink1->postRxDetLinkMask == 0)
    {
        NV_PRINTF(LEVEL_INFO, "Got 0 post RxDet Links on GPU %d or GPU %d!\n",
                gpuGetInstance(pGpu0), gpuGetInstance(pGpu1));
        return NV_ERR_NOT_READY;
    }

#endif

    return status;
}

NV_STATUS
ioctrlFaultUpTmrHandler
(
    OBJGPU *pGpu,
    OBJTMR *pTmr,
    TMR_EVENT *pEvent
)
{
    //NvU32 linkId = *(NvU32*)pData;
    NV_STATUS    status = NV_OK;
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
    NV2080_CTRL_NVLINK_POST_FAULT_UP_PARAMS *nvlinkPostFaultUpParams
                 = portMemAllocNonPaged(sizeof(NV2080_CTRL_NVLINK_POST_FAULT_UP_PARAMS));
    PNVLINK_ID   pFaultLink;
    pFaultLink = listHead(&pKernelNvlink->faultUpLinks);

    nvlinkPostFaultUpParams->linkId = pFaultLink->linkId;
    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                        NV2080_CTRL_CMD_NVLINK_POST_FAULT_UP,
                        (void *)nvlinkPostFaultUpParams,
                        sizeof(NV2080_CTRL_NVLINK_POST_FAULT_UP_PARAMS));

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to send Faultup RPC\n");
    }

    listRemove(&pKernelNvlink->faultUpLinks, pFaultLink);
    portMemFree(nvlinkPostFaultUpParams);

    return status;
}

NV_STATUS
knvlinkHandleFaultUpInterrupt_GH100
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvU32        linkId
)
{
    OBJTMR    *pTmr = GPU_GET_TIMER(pGpu);
    PNVLINK_ID pFaultLink;
    NV_STATUS status = NV_OK;

    pFaultLink = listAppendNew(&pKernelNvlink->faultUpLinks);
    NV_ASSERT_OR_RETURN(pFaultLink != NULL, NV_ERR_GENERIC);
    pFaultLink->linkId = linkId;

    status = tmrEventScheduleRel(pTmr, pKernelNvlink->nvlinkLinks[linkId].pTmrEvent, NVLINK_RETRAIN_TIME);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "GPU (ID: %d) tmrEventScheduleRel failed for linkid %d\n",
                  gpuGetInstance(pGpu), linkId);
        return NV_ERR_GENERIC;
    }

    return status;
}

NV_STATUS
knvlinkLogAliDebugMessages_GH100
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvBool        bFinal
)
{
    NV2080_CTRL_NVLINK_GET_ERR_INFO_PARAMS *nvlinkErrInfoParams;
    NvU32         i;
    // This is a Physical, Hopper specific HAL for debug purposes.
    NV_STATUS status;

    if (bFinal)
        return NV_OK;

    nvlinkErrInfoParams = portMemAllocNonPaged(sizeof(NV2080_CTRL_NVLINK_GET_ERR_INFO_PARAMS));
    portMemSet(nvlinkErrInfoParams, 0, sizeof(NV2080_CTRL_NVLINK_GET_ERR_INFO_PARAMS));
    nvlinkErrInfoParams->ErrInfoFlags |= NV2080_CTRL_NVLINK_ERR_INFO_FLAGS_ALI_STATUS;

    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                        NV2080_CTRL_CMD_NVLINK_GET_ERR_INFO,
                        (void *)nvlinkErrInfoParams,
                        sizeof(NV2080_CTRL_NVLINK_GET_ERR_INFO_PARAMS));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Error getting debug info for link training!\n");
        portMemFree(nvlinkErrInfoParams);
        return status;
    }

    FOR_EACH_INDEX_IN_MASK(32, i, pKernelNvlink->postRxDetLinkMask)
    {
        nvErrorLog_va((void *)pGpu, ALI_TRAINING_FAIL,
                "NVLink: Link training failed for link %u"
                "(0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x)",
                i,
                nvlinkErrInfoParams->linkErrInfo[i].NVLIPTLnkCtrlLinkStateRequest,
                nvlinkErrInfoParams->linkErrInfo[i].NVLDLRxSlsmErrCntl,
                nvlinkErrInfoParams->linkErrInfo[i].NVLDLTopLinkState,
                nvlinkErrInfoParams->linkErrInfo[i].NVLDLTopIntr,
                nvlinkErrInfoParams->linkErrInfo[i].DLStatMN00,
                nvlinkErrInfoParams->linkErrInfo[i].DLStatUC01,
                nvlinkErrInfoParams->linkErrInfo[i].MinionNvlinkLinkIntr);

        if (pKernelNvlink->bLinkTrainingDebugSpew)
            NV_PRINTF(LEVEL_ERROR,"ALI Error for GPU %d::linkId %d:"
                    "\nNVLIPT:\n\tCTRL_LINK_STATE_REQUEST_STATUS = %X\n"
                    "\nNVLDL :\n\tNV_NVLDL_RXSLSM_ERR_CNTL = %X\n"
                    "\n\tNV_NVLDL_TOP_LINK_STATE = %X\n"
                    "\n\tNV_NVLDL_TOP_INTR = %X\n"
                    "\nMINION DLSTAT:\n\tDLSTAT MN00 = %X\n"
                    "\n\tDLSTAT UC01 = %X\n"
                    "\n\tNV_MINION_NVLINK_LINK_INTR = %X\n",
                    pGpu->gpuInstance, i,
                    nvlinkErrInfoParams->linkErrInfo[i].NVLIPTLnkCtrlLinkStateRequest,
                    nvlinkErrInfoParams->linkErrInfo[i].NVLDLRxSlsmErrCntl,
                    nvlinkErrInfoParams->linkErrInfo[i].NVLDLTopLinkState,
                    nvlinkErrInfoParams->linkErrInfo[i].NVLDLTopIntr,
                    nvlinkErrInfoParams->linkErrInfo[i].DLStatMN00,
                    nvlinkErrInfoParams->linkErrInfo[i].DLStatUC01,
                    nvlinkErrInfoParams->linkErrInfo[i].MinionNvlinkLinkIntr);
    }
    FOR_EACH_INDEX_IN_MASK_END;
    portMemFree(nvlinkErrInfoParams);
    return NV_OK;
}

/**
 * @brief Check if the nvlink bandwidth setting is OFF
 *
 * @param[in]   pKernelNvlink         reference of KernelNvlink
 */
NvBool
knvlinkIsBandwidthModeOff_GH100
(
    KernelNvlink *pKernelNvlink
)
{
    return (gpumgrGetGpuNvlinkBwMode() == GPU_NVLINK_BW_MODE_OFF);
}

/**
 * @brief Calculate the number of active nvlinks needs to be reduced
 *        for direct connect GPU system
 *
 * @param[in]   pKernelNvlink         reference of KernelNvlink
 */
NvU32
knvlinkGetNumLinksToBeReducedPerIoctrl_GH100
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    NvU32 numlinks = 0;
    NvU8 mode;

    numlinks = knvlinkGetNumActiveLinksPerIoctrl(pGpu, pKernelNvlink);

    if (numlinks == 0)
        goto out;

    mode = gpumgrGetGpuNvlinkBwMode();

    switch (mode)
    {
        case GPU_NVLINK_BW_MODE_OFF:
            NV_PRINTF(LEVEL_ERROR, "Cannot reach here %s %d mode=%d\n",
                      __func__, __LINE__, mode);
            NV_ASSERT(0);
            break;
        case GPU_NVLINK_BW_MODE_MIN:
            numlinks = numlinks - 1; // At least one is ative at this point.
            break;
        case GPU_NVLINK_BW_MODE_HALF:
            numlinks = numlinks / 2;
            break;
        case GPU_NVLINK_BW_MODE_3QUARTER:
            numlinks = numlinks / 4;
            break;
        default: // Treat as GPU_NVLINK_BW_MODE_FULL
            numlinks = 0;
            break;
    }

out:
    return numlinks;
}

/**
 * @brief Calculate the effective peer link mask for HS_HUB configuration
 *
 * @param[in]   pGpu               OBJGPU pointer of local GPU
 * @param[in]   pKernelNvlink      reference of KernelNvlink
 * @param[in]   pRemoteGpu         OBJGPU pointer of remote GPU
 * @param[in/out] pPeerLinkMask    reference of peerLinkMask
 */
void
knvlinkGetEffectivePeerLinkMask_GH100
(
    OBJGPU *pGpu,
    KernelNvlink *pKernelNvlink,
    OBJGPU *pRemoteGpu,
    NvU32  *pPeerLinkMask
)
{
    NvU32 peerLinkMask, remotePeerLinkMask, effectivePeerLinkMask, peerLinkMaskPerIoctrl;
    NvU32 gpuInstance, remoteGpuInstance;
    NvU32 numLinksPerIoctrl = 0;
    NvU32 numIoctrls = 0;
    KernelNvlink *pRemoteKernelNvlink;
    NvU32 numLinksToBeReduced;
    NvU32 linkMaskToBeReduced;
    NvU32 linkId, count, i;

    gpuInstance = gpuGetInstance(pGpu);
    remoteGpuInstance = gpuGetInstance(pRemoteGpu);

    if (knvlinkIsGpuConnectedToNvswitch(pGpu, pKernelNvlink))
    {
        if (gpuFabricProbeGetlinkMaskToBeReduced(pGpu->pGpuFabricProbeInfoKernel,
                                                 &linkMaskToBeReduced) == NV_OK)
        {
            *pPeerLinkMask &= (~linkMaskToBeReduced);
        }

        return;
    }

    peerLinkMask = pKernelNvlink->peerLinkMasks[remoteGpuInstance];
    if (peerLinkMask == 0)
    {
        return;
    }

    //
    // No need to check if remotePeerLinkMask and peerLinkMask are equal because
    // RM will not enable P2P otherwise. Given that we have reached here means
    // the masks must be equal.
    //
    pRemoteKernelNvlink = GPU_GET_KERNEL_NVLINK(pRemoteGpu);
    remotePeerLinkMask = pRemoteKernelNvlink->peerLinkMasks[gpuInstance];
    NV_ASSERT(nvPopCount32(remotePeerLinkMask) == nvPopCount32(peerLinkMask));

    // Find out number of active NVLinks between the two GPUs.
    numLinksToBeReduced = knvlinkGetNumLinksToBeReducedPerIoctrl_HAL(pGpu, pKernelNvlink);
    effectivePeerLinkMask = peerLinkMask;

    if (numLinksToBeReduced == 0)
    {
        return;
    }

    // Start reducing effectivePeerLinkMask...

    //
    // To have deterministic approach, if local GPU ID is less than remote GPU
    // ID, always trim peerLinkMask from the perspective of local GPU.
    // Otherwise, use remote GPU for the same.
    //

    numIoctrls = nvPopCount32(pKernelNvlink->ioctrlMask);
    numLinksPerIoctrl = knvlinkGetTotalNumLinksPerIoctrl(pGpu, pKernelNvlink);

    if (pGpu->gpuId < pRemoteGpu->gpuId)
    {
        for (i = 0; i < numIoctrls; i++)
        {
            count = 0;
            peerLinkMaskPerIoctrl = peerLinkMask &
                (((1 << numLinksPerIoctrl) - 1) << (i * numLinksPerIoctrl));

            FOR_EACH_INDEX_IN_MASK(32, linkId, peerLinkMaskPerIoctrl)
            {
                if (count == numLinksToBeReduced)
                {
                    break;
                }

                effectivePeerLinkMask &= (~NVBIT(linkId));
                count++;
            }
            FOR_EACH_INDEX_IN_MASK_END;
        }
    }
    else
    {
        for (i = 0; i < numIoctrls; i++)
        {
            count = 0;
            peerLinkMaskPerIoctrl = remotePeerLinkMask &
                (((1 << numLinksPerIoctrl) - 1) << (i * numLinksPerIoctrl));

            FOR_EACH_INDEX_IN_MASK(32, linkId, peerLinkMaskPerIoctrl)
            {
                if (count == numLinksToBeReduced)
                {
                    break;
                }

#if defined(INCLUDE_NVLINK_LIB)
                effectivePeerLinkMask &=
                    (~NVBIT(pRemoteKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.linkNumber));
#endif
                count++;
            }
            FOR_EACH_INDEX_IN_MASK_END;
        }
    }

    //
    // effectivePeerLinkMask can never be zero, otherwise we create inconsistent
    // HW/SW state, where we say that NVLink P2P is supported, but we don't
    // program HSHUB.
    //
    // So, if not enough NVLinks are present, then drop effectivePeerLinkMask.
    //
    *pPeerLinkMask = (effectivePeerLinkMask > 0) ? effectivePeerLinkMask : peerLinkMask;
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
knvlinkSetUniqueFabricBaseAddress_GH100
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvU64         fabricBaseAddr
)
{
    NV_STATUS status = NV_OK;

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

    pKernelNvlink->fabricBaseAddr = fabricBaseAddr;

    NV_PRINTF(LEVEL_INFO, "Fabric base addr %llx is assigned to GPU %x\n",
              pKernelNvlink->fabricBaseAddr, pGpu->gpuInstance);

    return NV_OK;
}

/*!
 * @brief   Clear unique fabric address for NVSwitch enabled systems.
 *
 * @param[in] pGpu           OBJGPU pointer
 * @param[in] pKernelNvlink  KernelNvlink pointer
 */
void
knvlinkClearUniqueFabricBaseAddress_GH100
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    pKernelNvlink->fabricBaseAddr = NVLINK_INVALID_FABRIC_ADDR;
}

/*!
 * @brief   Set unique EGM fabric base address for NVSwitch enabled systems.
 *
 * @param[in] pGpu              OBJGPU pointer
 * @param[in] pKernelNvlink     KernelNvlink pointer
 * @param[in] fabricEgmBaseAddr EGM Fabric Address to set
 *
 * @returns On success, sets unique EGM fabric address and returns NV_OK.
 *          On failure, returns NV_ERR_XXX.
 */
NV_STATUS
knvlinkSetUniqueFabricEgmBaseAddress_GH100
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvU64         fabricEgmBaseAddr
)
{
    NV_STATUS status = NV_OK;

    status = knvlinkValidateFabricEgmBaseAddress_HAL(pGpu, pKernelNvlink,
                                                  fabricEgmBaseAddr);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "EGM Fabric base addr validation failed for GPU %x\n",
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

    if (pKernelNvlink->fabricEgmBaseAddr == fabricEgmBaseAddr)
    {
        NV_PRINTF(LEVEL_INFO,
                  "The same EGM fabric base addr is being re-assigned to GPU %x\n",
                  pGpu->gpuInstance);
        return NV_OK;
    }

    if (pKernelNvlink->fabricEgmBaseAddr != NVLINK_INVALID_FABRIC_ADDR)
    {
        NV_PRINTF(LEVEL_ERROR, "EGM Fabric base addr is already assigned to GPU %x\n",
                  pGpu->gpuInstance);
        return NV_ERR_STATE_IN_USE;
    }

    pKernelNvlink->fabricEgmBaseAddr = fabricEgmBaseAddr;

    NV_PRINTF(LEVEL_INFO, "EGM Fabric base addr %llx is assigned to GPU %x\n",
              pKernelNvlink->fabricEgmBaseAddr, pGpu->gpuInstance);

    return NV_OK;
}

/*!
 * @brief   Clear unique EGM fabric base address for NVSwitch enabled systems.
 *
 * @param[in] pGpu           OBJGPU pointer
 * @param[in] pKernelNvlink  KernelNvlink pointer
 */
void
knvlinkClearUniqueFabricEgmBaseAddress_GH100
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    pKernelNvlink->fabricEgmBaseAddr = NVLINK_INVALID_FABRIC_ADDR;
}

/*!
 * @brief   Check if system has enough active NVLinks and
 *          enough NVLink bridges
 *
 * @param[in] pGpu           OBJGPU pointer
 * @param[in] pKernelNvlink  KernelNvlink pointer
 *
 */
void
knvlinkDirectConnectCheck_GH100
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    NV2080_CTRL_NVLINK_DIRECT_CONNECT_CHECK_PARAMS params = {0};

    if (knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                        NV2080_CTRL_CMD_NVLINK_DIRECT_CONNECT_CHECK,
                        (void *)&params,
                        sizeof(params)) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Fail to call direct conect check command\n"); 
    }
}

/*!
 * @brief  Check if floorsweeping is needed for this particular chip
 *
 * @param[in]  pGpu            OBJGPU pointer
 * @param[in]  pKernelNvlink   KernelNvlink pointer
 *
 * @returns On success, sets unique fabric address and returns NV_OK.
 *          On failure, returns NV_ERR_XXX.
 */
NvBool
knvlinkIsFloorSweepingNeeded_GH100
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvU32         numActiveLinksPerIoctrl,
    NvU32         numLinksPerIoctrl
)
{

    //
    // Only floorsweep down the given GPU if the following conditions are met:
    // 1. if the number of links for the IP is > 0
    //
    // 2. The number of active links allowed for the IOCTRL is less then the
    //    total number of links for the IOCTRL. No reason to spend time in code
    //    if the exectution of it will be a NOP
    //
    // 3. If the GPU has never been floorswept. An optimization to make sure RM
    //    doesn't burn cycles repeatedly running running code that will be a NOP
    //
    // 4. (temporary) Run only on Silicon chips. Fmodel currently doesn't support
    //    this feature
    //

    if ((numLinksPerIoctrl > 0 && numActiveLinksPerIoctrl > 0) &&
        numActiveLinksPerIoctrl < numLinksPerIoctrl            &&
        !pKernelNvlink->bFloorSwept                            &&
        IS_SILICON(pGpu))
    {
        return NV_TRUE;
    }

    return NV_FALSE;
}

