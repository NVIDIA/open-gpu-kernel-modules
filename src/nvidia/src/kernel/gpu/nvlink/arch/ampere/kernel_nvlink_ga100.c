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
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/gpu.h"

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
knvlinkOverrideConfig_GA100
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvU32         phase
)
{
    NV_STATUS status = NV_OK;
    NvU32     i;

    pKernelNvlink->pLinkConnection = portMemAllocNonPaged(sizeof(NvU32) * pKernelNvlink->maxSupportedLinks);
    if (pKernelNvlink->pLinkConnection == NULL)
        return NV_ERR_NO_MEMORY;

    portMemSet(pKernelNvlink->pLinkConnection, 0, sizeof(NvU32) * pKernelNvlink->maxSupportedLinks);

    //
    // To deal with the nonlegacy force config reg keys, we need to now fill
    // in the default phys links, use a unity 1/1 map.
    //
    for (i = 0; i < pKernelNvlink->maxSupportedLinks; i++)
    {
        // The physical link is guaranteed valid in all cases
        pKernelNvlink->pLinkConnection[i] = DRF_NUM(_NVLINK, _ARCH_CONNECTION, _PHYSICAL_LINK, i);
    }

    // Check to see if there are chiplib overrides for nvlink configuration
    status = osGetForcedNVLinkConnection(pGpu, pKernelNvlink->maxSupportedLinks, pKernelNvlink->pLinkConnection);
    if ((NV_OK != status) || pKernelNvlink->bForceAutoconfig)
    {
        // A non-OK status implies there are no overrides.
        NV_PRINTF(LEVEL_INFO, "Not using forced config!\n");

        portMemFree(pKernelNvlink->pLinkConnection);
        pKernelNvlink->pLinkConnection = NULL;
        return NV_OK;
    }

    NV2080_CTRL_INTERNAL_NVLINK_PROCESS_FORCED_CONFIGS_PARAMS forcedConfigParams;
    portMemSet(&forcedConfigParams, 0, sizeof(forcedConfigParams));

    forcedConfigParams.bLegacyForcedConfig = NV_FALSE;
    portMemCopy(&forcedConfigParams.linkConnection, (sizeof(NvU32) * pKernelNvlink->maxSupportedLinks),
                pKernelNvlink->pLinkConnection,     (sizeof(NvU32) * pKernelNvlink->maxSupportedLinks));

    //
    // RPC to GSP-RM to for GSP-RM to process the forced NVLink configurations. This includes
    // setting up of HSHUB state and programming the memory subsystem registers.
    //
    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_INTERNAL_NVLINK_PROCESS_FORCED_CONFIGS,
                                 (void *)&forcedConfigParams,
                                 sizeof(forcedConfigParams));
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
knvlinkRemoveMapping_GA100
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink,
    NvBool        bAllMapping,
    NvU32         peerMask,
    NvBool        bL2Entry
)
{
    NV_STATUS status = NV_OK;
    NvU32     peerId;
    NvBool    bBufferReady = NV_FALSE;

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
    if (status != NV_OK)
        return status;

    //
    // Ampere+, along with HSHUB config registers, we also need to update
    // the MUX registers and the connection config registers. So, we have
    // to call nvlinkCurrentConfig instead of nvlinkUpdateHshubConfigRegs
    //
    status = knvlinkSyncLinkMasksAndVbiosInfo(pGpu, pKernelNvlink);
    if (status != NV_OK)
    {
        NV_ASSERT(status != NV_OK);
        return status;
    }

    if (pKernelNvlink->getProperty(pKernelNvlink, PDB_PROP_KNVLINK_CONFIG_REQUIRE_INITIALIZED_LINKS_CHECK))
    {
        FOR_EACH_INDEX_IN_MASK(32, peerId, peerMask)
        {
            if (pKernelNvlink->initializedLinks & KNVLINK_GET_MASK(pKernelNvlink, peerLinkMasks[peerId], 32))
            {
                bBufferReady = NV_TRUE;
                break;
            }
        } FOR_EACH_INDEX_IN_MASK_END;

        if (!bBufferReady)
        {
            status = knvlinkUpdateCurrentConfig(pGpu, pKernelNvlink);

        }
    }
    else
    {
        status = knvlinkUpdateCurrentConfig(pGpu, pKernelNvlink);
    }

    return status;
}

/*!
 * @brief   Validates fabric base address.
 *
 * @param[in] pGpu            OBJGPU pointer
 * @param[in] pKernelNvlink   KernelNvlink pointer
 * @param[in] fabricBaseAddr  Address to be validated
 *
 * @returns On success, NV_OK.
 *          On failure, returns NV_ERR_XXX.
 */
NV_STATUS
knvlinkValidateFabricBaseAddress_GA100
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
    // Ampere SKUs will be paired with NVSwitches (Limerock) supporting 2K
    // mapslots that can cover 64GB each. Make sure that the fabric base
    // address being used is valid to cover whole frame buffer.
    //

    // Check if fabric address is aligned to mapslot size.
    if (fabricBaseAddr & (NVBIT64(36) - 1))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Align fbSize to mapslot size.
    fbSizeBytes = RM_ALIGN_UP(fbSizeBytes, NVBIT64(36));


    // Check for integer overflow
    if (!portSafeAddU64(fabricBaseAddr, fbSizeBytes, &fbUpperLimit))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Make sure the address range doesn't go beyond the limit, (2K * 64GB).
    if (fbUpperLimit > NVBIT64(47))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    return NV_OK;
}

/*!
 * @brief   Checks to see if the GPU is a reduced nvlink config
 *
 * @param[in] pGpu            OBJGPU pointer
 * @param[in] pKernelNvlink   KernelNvlink pointer
 *
 * @returns On the gpu being a reduced config, NV_TRUE.
 *          otherwise , returns NV_FALSE.
 */
NvBool
knvlinkIsGpuReducedNvlinkConfig_GA100
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    NV2080_CTRL_NVLINK_IS_REDUCED_CONFIG_PARAMS params;
    NV_STATUS status;

    portMemSet(&params, 0, sizeof(params));

    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                             NV2080_CTRL_CMD_NVLINK_IS_REDUCED_CONFIG,
                             (void *)&params, sizeof(params));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to execute GSP-RM GPC to get if the gpu has a reduced Nvlink config\n");
        return NV_FALSE;
    }

    return params.bReducedNvlinkConfig;
}
