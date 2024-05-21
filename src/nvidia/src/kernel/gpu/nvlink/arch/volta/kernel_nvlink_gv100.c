/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "os/os.h"
#include "kernel/gpu/mmu/kern_gmmu.h"
#include "kernel/gpu/nvlink/kernel_ioctrl.h"
#include "core/thread_state.h"
#include "platform/sli/sli.h"

#include "gpu/gpu.h"
#include "gpu/mem_mgr/mem_mgr.h"

#if defined(INCLUDE_NVLINK_LIB)
static NV_STATUS _knvlinkAreLinksDisconnected(OBJGPU *, KernelNvlink *, NvBool *);
#endif

/*!
 * @brief Construct NVLink HAL
 *
 * @param[in] pGpu           OBJGPU pointer for local GPU
 * @param[in] pKernelNvlink  KernelNvlink pointer
 *
 * @return NV_OK if successful
 */
NV_STATUS
knvlinkConstructHal_GV100
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    NV_STATUS status = NV_OK;

    if (IS_FMODEL(pGpu) || IS_EMULATION(pGpu))
    {
        pKernelNvlink->bVerifTrainingEnable = NV_TRUE;
    }

    return status;
}

/*!
 * @brief Enable links post topology.
 *
 * @param[in] pGpu           OBJGPU pointer for local GPU
 * @param[in] pKernelNvlink  KernelNvlink pointer
 * @param[in] linkMask       Masks of links to enable
 *
 */
NV_STATUS
knvlinkEnableLinksPostTopology_GV100
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvU32         linkMask
)
{
    NV_STATUS status = NV_OK;
    NV2080_CTRL_NVLINK_ENABLE_LINKS_POST_TOPOLOGY_PARAMS params;

    //
    // Skip the RPC if linkmask is 0 or if all the links in the mask
    // are already initialized
    //
    if ((linkMask & (~pKernelNvlink->initializedLinks)) == 0)
    {
        return NV_OK;
    }

    portMemSet(&params, 0, sizeof(params));

    params.linkMask = linkMask;

    // Reset timeout to clear any accumulated timeouts from link init
    if (IS_GSP_CLIENT(pGpu))
    {
        threadStateResetTimeout(pGpu);
    }

    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_NVLINK_ENABLE_LINKS_POST_TOPOLOGY,
                                 (void *)&params, sizeof(params));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to enable Links post topology!\n");
        return status;
    }

    pKernelNvlink->initializedLinks = params.initializedLinks;

    return NV_OK;
}

/**
 * @brief This routine overrides the nvlink connection topology if chiplib arguments
 *        have been provided. It queries MODS API for the chiplib overrides and based
 *        on that, derives hshub configuration values that are programmed at a later
 *        stage during nvlink state load. The override values should exist for ALL
 *        links or NO links. The field encoding can be found in phys_nvlink.h
 *
 * @param[in] pGpu           OBJGPU pointer
 * @param[in] pKernelNvlink  KernelNvlink pointer
 * @param[in] phase          unused
 */
NV_STATUS
knvlinkOverrideConfig_GV100
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvU32         phase
)
{
    NV_STATUS status = NV_OK;
    NvU32     i;

    pKernelNvlink->pLinkConnection = portMemAllocNonPaged(sizeof(NvU32) * NVLINK_MAX_LINKS_SW);
    if (pKernelNvlink->pLinkConnection == NULL)
        return NV_ERR_NO_MEMORY;

    portMemSet(pKernelNvlink->pLinkConnection, 0, sizeof(NvU32) * NVLINK_MAX_LINKS_SW);

    //
    // To deal with the nonlegacy force config reg keys, we need to now fill
    // in the default phys links, use a unity 1/1 map.
    //
    for (i = 0; i < NVLINK_MAX_LINKS_SW; i++)
    {
        // The physical link is guaranteed valid in all cases
        pKernelNvlink->pLinkConnection[i] = DRF_NUM(_NVLINK, _ARCH_CONNECTION, _PHYSICAL_LINK, i);
    }

    // Check to see if there are chiplib overrides for nvlink configuration
    status = osGetForcedNVLinkConnection(pGpu, NVLINK_MAX_LINKS_SW, pKernelNvlink->pLinkConnection);
    if (NV_OK != status)
    {
        // A non-OK status implies there are no overrides.
        portMemFree(pKernelNvlink->pLinkConnection);
        pKernelNvlink->pLinkConnection = NULL;
        return NV_OK;
    }

    NV2080_CTRL_NVLINK_PROCESS_FORCED_CONFIGS_PARAMS forcedConfigParams;
    portMemSet(&forcedConfigParams, 0, sizeof(forcedConfigParams));

    forcedConfigParams.bLegacyForcedConfig = NV_FALSE;
    portMemCopy(&forcedConfigParams.linkConnection, (sizeof(NvU32) * NVLINK_MAX_LINKS_SW),
                pKernelNvlink->pLinkConnection,     (sizeof(NvU32) * NVLINK_MAX_LINKS_SW));

    //
    // RPC to GSP-RM to for GSP-RM to process the forced NVLink configurations. This includes
    // setting up of HSHUB state and programming the memory subsystem registers.
    //
    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_NVLINK_PROCESS_FORCED_CONFIGS,
                                 (void *)&forcedConfigParams, sizeof(forcedConfigParams));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to process forced NVLink configurations !\n");

        portMemFree(pKernelNvlink->pLinkConnection);
        pKernelNvlink->pLinkConnection = NULL;
        return status;
    }

    pKernelNvlink->bOverrideComputePeerMode = forcedConfigParams.bOverrideComputePeerMode;

    //
    // Now, CPU-RM should process the forced configurations and update its state, which includes
    // the topology information and the required link masks.
    //
    return knvlinkSetupTopologyForForcedConfig(pGpu, pKernelNvlink);
}

/*!
 * @brief This function applies settings specific to supporting Degraded Mode
 *        on NVswitch systems (LR10+) as follows -
 *        Checks for links that have been degarded (i.e disconnected) and then
 *        takes one of the following 2 actions-
 *        1) If the link has been degraded, it
 *           a) adds them to the disconnectedLinkMask, and
 *           b) marks their remote end as not connected
 *        2) If not, then adds the "active" link to the switchLinkMasks so that the
 *        caller can then update the correct state in a subsequent call to the
 *        _nvlinkUpdateSwitchLinkMasks function
 *
 * @param[in] pGpu             OBJGPU pointer
 * @param[in] pKernelNvlink    KernelNvlink pointer
 * @param[in] pSwitchLinkMasks Active switch links
 *
 * @return NV_OK on success
 */
NV_STATUS
knvlinkApplyNvswitchDegradedModeSettings_GV100
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvU32        *pSwitchLinkMasks
)
{
    NV_STATUS status = NV_OK;

#if defined(INCLUDE_NVLINK_LIB)

    NvBool  bLinkDisconnected[NVLINK_MAX_LINKS_SW] = {0};
    NvBool  bUpdateConnStatus = NV_FALSE;
    NvU32   switchLinks       = 0;
    NvU32   linkId;

    // At least there should be one connection to NVSwitch, else bail out
    FOR_EACH_INDEX_IN_MASK(32, linkId, pKernelNvlink->enabledLinks)
    {
        if (pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.deviceType == NVLINK_DEVICE_TYPE_NVSWITCH)
        {
            switchLinks |= NVBIT(linkId);
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

    if (switchLinks == 0)
    {
        return NV_OK;
    }

    //
    // Retrieve the connection status for all the enabled links. This requires checking
    // the links reset status and link state and sublink states as well.
    // Bug 3480556: _knvlinkAreLinksDisconnected makes RPC call to GSP-RM to get the link
    //              and sublink states. Trigger one RPC instead of invoking the RPC once
    //              for each link which reduces perf.
    //
    status = _knvlinkAreLinksDisconnected(pGpu, pKernelNvlink, bLinkDisconnected);
    NV_CHECK_OR_RETURN(LEVEL_INFO, status == NV_OK, status);

    FOR_EACH_INDEX_IN_MASK(32, linkId, pKernelNvlink->enabledLinks)
    {
        bUpdateConnStatus = NV_FALSE;

        //
        // Degraded Mode on NVSwitch systems:
        // We loop over enabledLinks to check if there are any links that have
        // changed their state from HS, so as to add them to the disconnectedLinkMask
        // and mark their remote end as not connected. If not, then add the active link
        // to switchLinkMasks so that it gets updated by _nvlinkUpdateSwitchLinkMasks below
        //
        if (pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.deviceType == NVLINK_DEVICE_TYPE_NVSWITCH)
        {
            if (bLinkDisconnected[linkId])
            {
                // RPC into GSP-RM to update the link connected status only if its required
                if (pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.bConnected)
                {
                    bUpdateConnStatus = NV_TRUE;
                }

                // Mark this link as disconnected
                pKernelNvlink->disconnectedLinkMask |= (NVBIT32(linkId));
                pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.bConnected = NV_FALSE;

                // RPC into GSP-RM to update the link connected status only if its required
                if (bUpdateConnStatus)
                {
                    status = knvlinkUpdateLinkConnectionStatus(pGpu, pKernelNvlink, linkId);
                    if (status != NV_OK)
                    {
                        return status;
                    }
                }
            }
            else if (pKernelNvlink->nvlinkLinks[linkId].remoteEndInfo.bConnected == NV_TRUE)
            {
                *pSwitchLinkMasks |= NVBIT32(linkId);
            }
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

#endif

    return status;
}

#if defined(INCLUDE_NVLINK_LIB)

/*!
 * @brief This function returns NV_TRUE for the following cases-
 *        1. Pseudo-clean shutdown
 *           linkMode == SAFE && sublinkState == OFF (RX and TX)
 *        2. Link reset post shutdown
 *           linkMode == RESET
 *
 * @param[in]  pGpu              OBJGPU pointer
 * @param[in]  pKernelNvlink     KernelNvlink pointer
 * @param[out] bLinkDisconnected Array of connection status for the links
 *                               NV_FALSE if link is connected
 *                               NV_TRUE if link is disconnected
 *
 * @return  NV_OK on success
 */
static NV_STATUS
_knvlinkAreLinksDisconnected
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvBool       *bLinkDisconnected
)
{
    NV_STATUS status = NV_OK;
    NvU32     linkId;

    NV_ASSERT_OR_RETURN(bLinkDisconnected != NULL, NV_ERR_INVALID_ARGUMENT);

    NV2080_CTRL_NVLINK_GET_LINK_AND_CLOCK_INFO_PARAMS *pParams =
        portMemAllocNonPaged(sizeof(*pParams));
    if (pParams == NULL)
    {
        return NV_ERR_NO_MEMORY;
    }

    portMemSet(pParams, 0, sizeof(*pParams));
    pParams->linkMask = pKernelNvlink->enabledLinks;

    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_NVLINK_GET_LINK_AND_CLOCK_INFO,
                                 (void *)pParams, sizeof(*pParams));
    if (status != NV_OK)
        goto cleanup;

    FOR_EACH_INDEX_IN_MASK(32, linkId, pKernelNvlink->enabledLinks)
    {
        if ((pParams->linkInfo[linkId].linkState == NVLINK_LINKSTATE_SAFE) &&
            (pParams->linkInfo[linkId].txSublinkState == NVLINK_SUBLINK_STATE_TX_OFF) &&
            (pParams->linkInfo[linkId].rxSublinkState == NVLINK_SUBLINK_STATE_RX_OFF))
        {
            // Case 1: Pseudo-clean shutdown
            bLinkDisconnected[linkId] = NV_TRUE;
        }
        else if (pParams->linkInfo[linkId].bLinkReset)
        {
            // Case 2: Link reset post shutdown
            bLinkDisconnected[linkId] = NV_TRUE;
        }
        else
        {
            // Link is connected
            bLinkDisconnected[linkId] = NV_FALSE;
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

cleanup:
    portMemFree(pParams);

    return status;
}

#endif

/*!
 * @brief Program NVLink Speed for the enabled links
 *
 * @param[in]  pGpu          OBJGPU pointer
 * @param[in]  pKernelNvlink KernelNvlink pointer
 */
NV_STATUS
knvlinkProgramLinkSpeed_GV100
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    NV_STATUS status = NV_OK;
    NvU32     platformLineRate;

    NV_STATUS platformLinerateDefined = NV_ERR_NOT_SUPPORTED;
    platformLinerateDefined = osGetPlatformNvlinkLinerate(pGpu, &platformLineRate);

    NV2080_CTRL_NVLINK_PROGRAM_LINK_SPEED_PARAMS programLinkSpeedParams;
    portMemSet(&programLinkSpeedParams, 0, sizeof(programLinkSpeedParams));

    programLinkSpeedParams.bPlatformLinerateDefined = (platformLinerateDefined == NV_OK);
    programLinkSpeedParams.platformLineRate         = platformLineRate;

    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_NVLINK_PROGRAM_LINK_SPEED,
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
 * Handles PostLoad Hal for NVLink.
 *
 * @param[in] pGpu          OBJGPU pointer
 * @param[in] pKernelNvlink KernelNvlink pointer
 *
 * @returns NV_OK on success, ERROR otherwise.
 */
NV_STATUS
knvlinkStatePostLoadHal_GV100
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    NV_STATUS status = NV_OK;
    KernelGmmu *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);

    NV2080_CTRL_INTERNAL_NVLINK_GET_SET_NVSWITCH_FABRIC_ADDR_PARAMS params;

    // Set NVSwitch fabric base address and enable compute peer addressing
    if (knvlinkIsNvswitchProxyPresent(pGpu, pKernelNvlink))
    {
        portMemSet(&params, 0, sizeof(params));
        params.bGet = NV_TRUE;
        params.addr = NVLINK_INVALID_FABRIC_ADDR;

        status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                     NV2080_CTRL_CMD_INTERNAL_NVLINK_GET_SET_NVSWITCH_FABRIC_ADDR,
                                     (void *)&params, sizeof(params));
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Failed to get nvswitch fabric address for GPU %x\n",
                      pGpu->gpuInstance);
            return status;
        }

        status = knvlinkSetUniqueFabricBaseAddress(pGpu, pKernelNvlink, params.addr);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Failed to set unique NVSwitch fabric base address for GPU %x\n",
                      pGpu->gpuInstance);
            return status;
        }
    }

    if (knvlinkIsNvswitchProxyPresent(pGpu, pKernelNvlink) ||
        pKernelNvlink->bOverrideComputePeerMode            ||
        GPU_IS_NVSWITCH_DETECTED(pGpu))
    {
        status = kgmmuEnableNvlinkComputePeerAddressing_HAL(pKernelGmmu);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Failed to enable compute addressing for GPU %x\n",
                      pGpu->gpuInstance);
            return status;
        }

        status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                     NV2080_CTRL_CMD_INTERNAL_NVLINK_ENABLE_COMPUTE_PEER_ADDR,
                                     NULL, 0);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Failed to enable compute peer addressing!\n");
            return status;
        }
    }

    return NV_OK;
}

/*!
 * @brief   Validates fabric base address.
 *
 * @param[in] pGpu           OBJGPU pointer
 * @param[in] pKernelNvlink  KernelNvlink pointer
 * @param[in] fabricBaseAddr Address to be validated
 *
 * @returns On success, NV_OK.
 *          On failure, returns NV_ERR_XXX.
 */
NV_STATUS
knvlinkValidateFabricBaseAddress_GV100
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvU64         fabricBaseAddr
)
{
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    NvU64          fbSizeBytes;
    NvU64          fbUpperLimit;

    fbSizeBytes = pMemoryManager->Ram.fbTotalMemSizeMb << 20;

    //
    // Volta SKUs will be paired with NVSwitches (Willow) supporting 8K mapslots
    // that can cover 16GB each. Make sure that the fabric base address being
    // used is valid to cover whole frame buffer.
    //

    // Check if fabric address is aligned to mapslot size.
    if (fabricBaseAddr & (NVBIT64(34) - 1))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Align fbSize to mapslot size.
    fbSizeBytes = RM_ALIGN_UP(fbSizeBytes, NVBIT64(34));

    // Check for integer overflow
    if (!portSafeAddU64(fabricBaseAddr, fbSizeBytes, &fbUpperLimit))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Make sure the address range doesn't go beyond the limit, (8K * 16GB).
    if (fbUpperLimit > NVBIT64(47))
    {
        return NV_ERR_INVALID_ARGUMENT;
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
knvlinkSetUniqueFabricBaseAddress_GV100
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

        status = knvlinkEnableLinksPostTopology_HAL(pGpu, pKernelNvlink,
                                        pKernelNvlink->enabledLinks);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                    "Nvlink post topology links setup failed on GPU %x\n",
                    pGpu->gpuInstance);
            return status;
        }
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
