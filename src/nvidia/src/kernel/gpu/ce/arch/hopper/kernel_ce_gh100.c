/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "gpu/gpu.h"
#include "gpu/ce/kernel_ce.h"
#include "gpu/nvlink/kernel_nvlink.h"
#include "gpu/ce/kernel_ce_private.h"
#include "gpu/bif/kernel_bif.h"
#include "platform/chipset/chipset.h"

#include "published/hopper/gh100/dev_ce.h"
#include "published/hopper/gh100/dev_xtl_ep_pcfg_gpu.h"

// Defines for PCE-LCE mapping algorithm
#define NV_CE_MAX_HSHUBS                  5
#define NV_CE_LCE_MASK_INIT               0xFFFFFFFF
#define NV_CE_GRCE_ALLOWED_LCE_MASK       0x03
#define NV_CE_MAX_GRCE                    2
#define NV_CE_EVEN_ASYNC_LCE_MASK         0x55555550
#define NV_CE_ODD_ASYNC_LCE_MASK          0xAAAAAAA0
#define NV_CE_MAX_LCE_MASK                0x3FF
#define NV_CE_PCE_PER_HSHUB               4
#define NV_CE_NUM_FBPCE                   4
#define NV_CE_NUM_PCES_NO_LINK_CASE       12
#define NV_CE_MAX_PCE_PER_GRCE            2



/*!
 * @brief Returns the size of the PCE2LCE register array
 *
 *
 * @param[in] pGpu  OBJGPU pointer
 * @param[in] pCe   OBJCE pointer
 *
 * @return  NV_CE_PCE2LCE_CONFIG__SIZE_1
 *
 */
NvU32
kceGetPce2lceConfigSize1_GH100
(
    KernelCE *pKCe
)
{
    return NV_CE_PCE2LCE_CONFIG__SIZE_1;
}

/**
 * @brief This function takes in a link mask and returns the minimum number
 * of PCE connections required. This is decided based on a round up approach
 * where each PCE can handle 1.5 links.
 */
NvU32
kceGetNumPceRequired
(
    NvU32 numLinks
)
{
    switch(numLinks)
    {
        case 6:
            return 4;
        case 5:
        case 4:
            return 3;
        case 3:
            return 2;
        case 2:
        case 1:
        default:
            return 1;
    }
}

/**
 * @brief This function returns the pceIndex for a particular link ID
 *        Must always be called with the hshub ID for the calling link ID
 *
 * @param[in]   pGpu                        OBJGPU pointer
 * @param[in]   pKCe                         KernelCE pointer
 * @param[in]   pceAvailableMaskPerHshub    Pointer to CEs available per HSHUB
 * @param[out]  pceIndex                    Pointer to caller pceIndex
 * @param[out]  pHshubId                    Pointer to caller HSHUB ID
 */
static void
_ceGetAlgorithmPceIndex
(
    OBJGPU     *pGpu,
    KernelCE   *pKCe,
    NvU32      *pceAvailableMaskPerHshub,
    NvU32      *pceIndex,
    NvU8       *pHshubId
)
{
    NvU8 pHshubIdRequested;
    NvU32 i;

    if (!(NVBIT32(*pceIndex) & pceAvailableMaskPerHshub[*pHshubId]))
    {
        //
        // 1. We couldn't find an applicable strided PCE in given HSHUB
        // So, we'll assign the next consecutive PCE on the same HSHUB
        //
        *pceIndex = CE_GET_LOWEST_AVAILABLE_IDX(pceAvailableMaskPerHshub[*pHshubId]);
        if (!(NVBIT32(*pceIndex) & pceAvailableMaskPerHshub[*pHshubId]))
        {
            // 2. If this is not a valid PCE on given HSHUB, assign PCE from alternative HSHUB
            pHshubIdRequested = *pHshubId;
            for (i = pHshubIdRequested + 1; i != pHshubIdRequested; i++) {
                if (i > 4) {
                    i = 1;
                    continue;
                }

                *pceIndex = CE_GET_LOWEST_AVAILABLE_IDX(pceAvailableMaskPerHshub[i]);
                if (NVBIT32(*pceIndex) & pceAvailableMaskPerHshub[i]) {
                    break;
                }
            }

            if (i == pHshubIdRequested)
            {
                // If we've reached this point, then we have no more available PCEs to assign
                NV_PRINTF(LEVEL_ERROR, "No more available PCEs to assign!\n");
                NV_ASSERT(0);
            }
        }
    }
    return;
}

/**
 * @brief This function assigns LCE 2 and 3 mappings for C2C cases.
 *
 * @param[in]   pGpu                        OBJGPU pointer
 * @param[in]   pKCe                         KernelCE pointer
 * @param[in]   pceAvailableMaskPerHshub    Pointer to CEs available per HSHUB
 * @param[out]  pLocalPceLceMap             Pointer to PCE-LCE array
 * @param[out]  pLocalExposeCeMask          Pointer to LCE Mask
 */
NV_STATUS
kceMapPceLceForC2C_GH100
(
    OBJGPU  *pGpu,
    KernelCE *pKCe,
    NvU32   *pceAvailableMaskPerHshub,
    NvU32   *pLocalPceLceMap,
    NvU32   *pLocalExposeCeMask
)
{
    NV_STATUS status     = NV_OK;
    KernelBif *pKernelBif = GPU_GET_KERNEL_BIF(pGpu);
    NvU32     pceIndex, i, hshubId, lceMask, lceIndex;
    NvU32     numNvLinkPeers  = 0;
    NvU32     selectPcePerHshub = 2;
    NvBool    c2cEnabled = pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_IS_C2C_LINK_UP);

    numNvLinkPeers = pKCe->nvlinkNumPeers;
    if (gpuIsCCFeatureEnabled(pGpu) || (c2cEnabled && numNvLinkPeers == 0 && IS_MIG_IN_USE(pGpu)))
    {
        lceMask = NVBIT32(2) | NVBIT32(3);
        *pLocalExposeCeMask |= lceMask;

        lceIndex = CE_GET_LOWEST_AVAILABLE_IDX(lceMask);
        pceIndex = NVBIT32(0);
        pLocalPceLceMap[pceIndex] = lceIndex;
        lceMask &= (~(NVBIT32(lceIndex)));

        lceIndex = CE_GET_LOWEST_AVAILABLE_IDX(lceMask);
        pceIndex = NVBIT32(1);
        pLocalPceLceMap[pceIndex] = lceIndex;
    }
    else if (c2cEnabled && numNvLinkPeers == 0)
    {
        lceMask = NVBIT32(2);
        *pLocalExposeCeMask |= lceMask;
        lceIndex = CE_GET_LOWEST_AVAILABLE_IDX(lceMask);

        for (hshubId = 2; hshubId < NV_CE_MAX_HSHUBS; hshubId++)
        {
            for (i = 0; i < selectPcePerHshub; i++)
            {
                pceIndex = CE_GET_LOWEST_AVAILABLE_IDX(pceAvailableMaskPerHshub[hshubId]);
                pceAvailableMaskPerHshub[hshubId] &= (~(NVBIT32(pceIndex)));
                pLocalPceLceMap[pceIndex] = lceIndex;
            }
        }

        lceMask = NVBIT32(4);
        *pLocalExposeCeMask |= lceMask;
        lceIndex = CE_GET_LOWEST_AVAILABLE_IDX(lceMask);

        for (hshubId = 2; hshubId < NV_CE_MAX_HSHUBS; hshubId++)
        {
            for (i = 0; i < selectPcePerHshub; i++)
            {
                pceIndex = CE_GET_LOWEST_AVAILABLE_IDX(pceAvailableMaskPerHshub[hshubId]);
                pceAvailableMaskPerHshub[hshubId] &= (~(NVBIT32(pceIndex)));
                pLocalPceLceMap[pceIndex] = lceIndex;
            }
        }
    }
    else
    {
        status = NV_WARN_NOTHING_TO_DO;
    }

    return status;
}

/**
 * @brief This function checks for root port gen speed or GPU
 *        gen speed to determine if we should apply genX+ mapping
 *        or genX- mapping
 *
 * @param[in]   pGpu            OBJGPU pointer
 * @param[in]   pKCe            KernelCE pointer
 * @param[in]   checkGen        gen X for query
 */
NvBool
kceIsGenXorHigherSupported_GH100
(
    OBJGPU    *pGpu,
    KernelCE  *pKCe,
    NvU32     checkGen
)
{
    OBJSYS     *pSys = SYS_GET_INSTANCE();
    OBJCL      *pCl = SYS_GET_CL(pSys);
    NvU8       genSpeed = 0;
    NvU32      busSpeed = 0;
    NV_STATUS  status = NV_OK;
    NvBool     bIsGenXorHigher = NV_FALSE;

    status = clPcieGetRootGenSpeed(pGpu, pCl, &genSpeed);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Could not get root gen speed - check for GPU gen speed!\n");
        // Check for GPU gen speed
        if (GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_EP_PCFG_GPU_LINK_CONTROL_STATUS, &busSpeed) != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Unable to read NV_EP_PCFG_GPU_LINK_CONTROL_STATUS from config space.\n");
            return bIsGenXorHigher;
        }
        genSpeed = GPU_DRF_VAL(_EP_PCFG_GPU, _LINK_CONTROL_STATUS, _CURRENT_LINK_SPEED, busSpeed);
    }
    NV_PRINTF(LEVEL_INFO, "Gen Speed = %d\n", genSpeed);

    if ((genSpeed >= checkGen))
    {
        bIsGenXorHigher = NV_TRUE;
    }

    return bIsGenXorHigher;
}

/**
 * @brief This function assigns PCE-LCE mappings for GRCE LCEs 0 and 1.
 *        This function additionally takes care of mappings for LCE 2 and 3
 *        in the default case.
 *
 * @param[in]   pGpu                        OBJGPU pointer
 * @param[in]   pKCe                         KernelCE pointer
 * @param[in]   pceAvailableMaskPerHshub    Pointer to CEs available per HSHUB
 * @param[out]  pLocalPceLceMap             Pointer to PCE-LCE array
 * @param[out]  pLocalExposeCeMask          Pointer to LCE Mask
 */
void
kceMapPceLceForGRCE_GH100
(
    OBJGPU  *pGpu,
    KernelCE *pKCe,
    NvU32   *pceAvailableMaskPerHshub,
    NvU32   *pLocalPceLceMap,
    NvU32   *pLocalExposeCeMask,
    NvU32   *pLocalGrceMap,
    NvU32   fbPceMask
)
{
    KernelBif *pKernelBif = GPU_GET_KERNEL_BIF(pGpu);
    NvU32     grceIdx, pceIndex, i;
    NvU32     lceIndex = 0;
    NvU32     lceMask = 0;
    NvU32     numNvLinkPeers = 0;
    NvU32     grceMappings[NV_CE_NUM_FBPCE] = {12, 14, 13, 15};
    NvBool    gen5OrHigher = kceIsGenXorHigherSupported_HAL(pGpu, pKCe, 5);
    NvBool    c2cEnabled = pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_IS_C2C_LINK_UP);

    numNvLinkPeers = pKCe->nvlinkNumPeers;

    if (gpuIsCCFeatureEnabled(pGpu) || (c2cEnabled && numNvLinkPeers == 0))
    {
        lceMask = NVBIT32(0) | NVBIT32(1);
        *pLocalExposeCeMask |= lceMask;

        for (grceIdx = 0; grceIdx < NV_CE_MAX_GRCE; grceIdx++)
        {
            for (i = 0; i < NV_CE_MAX_PCE_PER_GRCE; i++)
            {
                pceIndex = grceMappings[grceIdx * 2 + i];

                //
                // floorswept PCE or
                // PCIe <= Gen4 experience high latency and requires a
                // different mapping for LCE2 and LCE3 compared to Gen5.
                // In PCIe <= Gen4 cases, only link 1 PCE to LCE by
                // skipping every other PCE in the grceMappings array.
                //
                if (pceIndex == 0 || (!gen5OrHigher && (i % 2 == 1)))
                    continue;

                lceIndex = CE_GET_LOWEST_AVAILABLE_IDX(lceMask);
                pLocalPceLceMap[pceIndex] = lceIndex;
            }

            lceMask &= (~(NVBIT32(lceIndex)));
        }
    }
    else
    {
        // Default case which will result in sharing LCE 2 and 3 with LCE 0 and 1
        lceMask = NVBIT32(2) | NVBIT32(3);
        *pLocalExposeCeMask |= lceMask;
        for (grceIdx = 0; grceIdx < NV_CE_MAX_GRCE; grceIdx++)
        {
            for (i = 0; i < NV_CE_MAX_PCE_PER_GRCE; i++)
            {
                pceIndex = grceMappings[grceIdx * 2 + i];

                // floorswept PCE or account for PCIe latency in Gen <= 4
                if (pceIndex == 0 || (!gen5OrHigher && (i % 2 == 1)))
                    continue;

                lceIndex = CE_GET_LOWEST_AVAILABLE_IDX(lceMask);
                pLocalPceLceMap[pceIndex] = lceIndex;
            }

            // update lceMask now that all PCEs are assigned to this LCE
            lceMask &= (~(NVBIT32(lceIndex)));
        }

        // GRCE Cases
        lceMask = kceGetGrceSupportedLceMask_HAL(pGpu, pKCe);
        *pLocalExposeCeMask |= lceMask;

        for (grceIdx = 0; grceIdx < NV_CE_MAX_GRCE; grceIdx++)
        {
            for (i = 0; i < NV_CE_MAX_PCE_PER_GRCE; i++)
            {
                pceIndex = grceMappings[grceIdx * 2 + i];
                fbPceMask &= (~(NVBIT32(pceIndex)));

                // floorswept PCE
                if (pceIndex == 0 || (!gen5OrHigher && (i % 2 == 1)))
                    continue;

                // Sharing use case
                if ((NVBIT32(pLocalPceLceMap[pceIndex])) & *pLocalExposeCeMask)
                {
                    // GRCE is shared - set the status and shared LCE # in register field
                    lceIndex = pLocalPceLceMap[pceIndex];
                    pLocalGrceMap[grceIdx] = DRF_NUM(_CE, _GRCE_CONFIG, _SHARED, 1) |
                                             DRF_NUM(_CE, _GRCE_CONFIG, _SHARED_LCE, lceIndex);
                }
                else
                {
                    // GRCE got its own FBHUB PCE
                    // Store the LCE in the associated PCE for GRCE
                    lceIndex = CE_GET_LOWEST_AVAILABLE_IDX(lceMask);
                    pLocalPceLceMap[pceIndex] = lceIndex;
                    // Reflect non-sharing status in register field
                    pLocalGrceMap[grceIdx] = DRF_NUM(_CE, _GRCE_CONFIG, _SHARED, 0) |
                                             DRF_DEF(_CE, _GRCE_CONFIG, _SHARED_LCE, _NONE);
                }
            }

            // update lceMask now that all PCEs are assigned to this LCE
            lceMask &= (~(NVBIT32(lceIndex)));
        }
    }
}

/**
 * @brief This function assigns PCE-LCE mappings for NVLink peers
 *        Based on HSHUBs that the links associated with a peer connect to,
 *        algorithm will attempt to assign a PCE from associated HSHUB taking into
 *        account striding as well.
 *
 * @param[in]   pGpu                        OBJGPU pointer
 * @param[in]   pKCe                         KernelCE pointer
 * @param[in]   pceAvailableMaskPerHshub    Pointer to CEs available per HSHUB
 * @param[out]  pLocalPceLceMap             Pointer to PCE-LCE array
 * @param[out]  pLocalExposeCeMask          Pointer to LCE Mask
 *
 * Returns NV_OK if successful in assigning PCEs and LCEs for each of the NVLink peers
 */
NV_STATUS
kceMapPceLceForNvlinkPeers_GH100
(
    OBJGPU  *pGpu,
    KernelCE *pKCe,
    NvU32   *pceAvailableMaskPerHshub,
    NvU32   *pLocalPceLceMap,
    NvU32   *pLocalExposeCeMask
)
{
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
    OBJSYS       *pSys          = SYS_GET_INSTANCE();
    NV_STATUS     status        = NV_OK;
    NvU32         lceMask       = 0;
    NvU32         pceMask       = 0;
    NvU32         peerLinkMask  = 0;
    KernelCE     *pKCeLce       = NULL;
    NvBool        bPeerAssigned = NV_FALSE;
    NvU32         peerAvailableLceMask = NV_CE_LCE_MASK_INIT;
    OBJGPU       *pRemoteGpu;
    NvU32         numPcePerLink;
    NvU32         lceIndex, pceIndex;
    NvU8          hshubId = 0, i;
    NvU32         linkId, gpuMask, gpuInstance = 0, j;

    NV2080_CTRL_INTERNAL_HSHUB_GET_HSHUB_ID_FOR_LINKS_PARAMS params;

    if (pKernelNvlink == NULL)
    {
        return NV_WARN_NOTHING_TO_DO;
    }

    peerAvailableLceMask = kceGetNvlinkPeerSupportedLceMask_HAL(pGpu, pKCe, peerAvailableLceMask);
    pKCe->nvlinkNumPeers = 0;

    if (knvlinkIsGpuConnectedToNvswitch(pGpu, pKernelNvlink))
    {
        //
        // On NVSwitch systems, we only create 1 aperture for all p2p connections.
        // For PCE2LCE mapping, we should only assign 1 LCE for this connection.
        //
        // Since we mark the loopback connections in peerLinkMasks with the appropriate
        // links (see _nvlinkUpdateSwitchLinkMasks), we can use that to calculate
        // the PCE2LCE config.
        //
        gpuMask = NVBIT32(pGpu->gpuInstance);
    }
    else
    {
        // On direct connected systems, we'll loop over each GPU in the system
        // and assign a peer LCE for each connection
        (void)gpumgrGetGpuAttachInfo(NULL, &gpuMask);
    }

   while ((pRemoteGpu = gpumgrGetNextGpu(gpuMask, &gpuInstance)) != NULL)
   {
        NvU32 numLinksToPeer = knvlinkGetNumLinksToPeer(pGpu, pKernelNvlink,
                                                       pRemoteGpu);
        if (numLinksToPeer == 0)
        {
            continue;
        }

        pceMask = 0;
        lceMask = 0;

        if (peerAvailableLceMask == 0)
        {
            //
            // peerAvailableLceMask is initialized to even async LCEs at the
            // top of the function.
            // As a result, if at any point in the loop, this mask == 0,
            // it implies we have used up all even async LCEs and should move to
            // using odd async LCEs.
            //
            peerAvailableLceMask = kceGetNvlinkPeerSupportedLceMask_HAL(pGpu, pKCe, peerAvailableLceMask);
        }

        // Each peer gets 1 LCE
        lceIndex = CE_GET_LOWEST_AVAILABLE_IDX(peerAvailableLceMask);
        lceMask |= NVBIT32(lceIndex);

        pKCe->nvlinkNumPeers++;

        // Clear out the chosen LCE
        peerAvailableLceMask &= (~(NVBIT32(lceIndex)));

        peerLinkMask = knvlinkGetLinkMaskToPeer(pGpu, pKernelNvlink, pRemoteGpu);
        if (peerLinkMask == 0)
        {
            NV_PRINTF(LEVEL_INFO, "GPU%d has nvlink disabled. Skip programming\n", pRemoteGpu->gpuInstance);
            continue;
        }

        portMemSet(&params, 0, sizeof(params));
        params.linkMask = peerLinkMask;

        status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                     NV2080_CTRL_CMD_INTERNAL_HSHUB_GET_HSHUB_ID_FOR_LINKS,
                                     (void *)&params, sizeof(params));
        NV_ASSERT_OK_OR_RETURN(status);

        // Iterate through links by HSHUB
        NvU32 linksPerHshub[NV_CE_MAX_HSHUBS] = {0};

        FOR_EACH_INDEX_IN_MASK(32, linkId, peerLinkMask)
        {
            hshubId = params.hshubIds[linkId];
            // Update link count for this hshub
            linksPerHshub[hshubId]++;
        }
        FOR_EACH_INDEX_IN_MASK_END;

        for (i = 0; i < NV_CE_MAX_HSHUBS; i++)
        {
            if (linksPerHshub[i] == 0)
                continue;

            pceIndex = CE_GET_LOWEST_AVAILABLE_IDX(pceAvailableMaskPerHshub[i]);
            numPcePerLink = kceGetNumPceRequired(linksPerHshub[i]);

            for (j = 0; j < numPcePerLink; j++)
            {
                _ceGetAlgorithmPceIndex(pGpu, pKCe, pceAvailableMaskPerHshub, &pceIndex, &i);
                pceMask |= NVBIT32(pceIndex);
                // Clear out the assigned PCE
                pceAvailableMaskPerHshub[i] &= (~(NVBIT32(pceIndex)));
            }

        }

        // Now, assign the PCE-LCE association for the current peer
        if (pceMask != 0)
        {
            // We just need at least one peer to set this to TRUE
            bPeerAssigned = NV_TRUE;

            FOR_EACH_INDEX_IN_MASK(32, pceIndex, pceMask)
            {
                pLocalPceLceMap[pceIndex] = lceIndex;
                NV_PRINTF(LEVEL_INFO, "GPU%d <-> GPU%d PCE Index: %d LCE Index: %d\n",
                        pGpu->gpuInstance, pRemoteGpu->gpuInstance, pceIndex, lceIndex);
            }
            FOR_EACH_INDEX_IN_MASK_END;

            // Store lceMask in the exposeCeMask before moving on
            *pLocalExposeCeMask |= lceMask;
        }

        pKCeLce = GPU_GET_KCE(pGpu, lceIndex);
        pKCeLce->nvlinkPeerMask |= NVBIT(pRemoteGpu->gpuInstance);

        //
        // Bug 200659256 - Looping over GPUs rather than peers (CL 28776130)
        // does not handle multi-GPUs/Peer as is the case on switch systems.
        // We must only take this loop once on switch systems to account for this.
        // If we need to support multiple peer connections with switch systems
        // in the future, this code must be revisited
        //
        if (pSys->getProperty(pSys, PDB_PROP_SYS_FABRIC_IS_EXTERNALLY_MANAGED))
        {
            break;
        }

    }

    if (bPeerAssigned == NV_FALSE)
    {
        status = NV_WARN_NOTHING_TO_DO;
    }

    return status;
}

/**
 * @brief Some clients rely on LCE 4 also being turned on when there
 *        are no NVLink peers. This function sets up the default links.
 *
 * @param[in]   pGpu                        OBJGPU pointer
 * @param[in]   pKCe                        KernelCE pointer
 * @param[in]   pceAvailableMaskPerHshub    Pointer to CEs available per HSHUB
 * @param[out]  pLocalPceLceMap             Pointer to PCE-LCE array
 * @param[out]  pLocalExposeCeMask          Pointer to LCE Mask
 *
 * Returns NV_OK if successful in assigning PCEs to a default async LCE (>= 4)
 */
NV_STATUS
kceMapAsyncLceDefault_GH100
(
    OBJGPU  *pGpu,
    KernelCE   *pKCe,
    NvU32   *pceAvailableMaskPerHshub,
    NvU32   *pLocalPceLceMap,
    NvU32   *pLocalExposeCeMask,
    NvU32   numDefaultPces
)
{
    NvU32 peerAvailableLceMask = NV_CE_LCE_MASK_INIT;
    NvU32 lceMask = 0;
    NvU32 pceMask = 0;
    NvU32 lceIndex, pceIndex, hshubId, i;

    peerAvailableLceMask = kceGetNvlinkPeerSupportedLceMask_HAL(pGpu, pKCe, peerAvailableLceMask);
    hshubId = 1;

    //
    // If no peers were found, then no async LCEs (>= 4) will be turned on.
    // However, some clients rely on LCE 4 being present even without any
    // NVLink peers being found. So, turn on the 1st available async LCE (>= 4)
    // Reference bug 3042556
    //
    lceIndex = CE_GET_LOWEST_AVAILABLE_IDX(peerAvailableLceMask);
    lceMask |= NVBIT32(lceIndex);
    // Clear out the chosen LCE
    peerAvailableLceMask &= (~(NVBIT32(lceIndex)));

    // Assign PCEs to this LCE based on input request
    for (i = 0; i < numDefaultPces; i++)
    {
        if (i % NV_CE_PCE_PER_HSHUB == 0)
            hshubId++;

        pceIndex = CE_GET_LOWEST_AVAILABLE_IDX(pceAvailableMaskPerHshub[hshubId]);
        pceMask |= NVBIT32(pceIndex);
        pceAvailableMaskPerHshub[hshubId] &= (~(NVBIT32(pceIndex)));
    }

    FOR_EACH_INDEX_IN_MASK(32, pceIndex, pceMask)
    {
        pLocalPceLceMap[pceIndex] = lceIndex;
        NV_PRINTF(LEVEL_INFO, "GPU%d <-> GPU%d PCE Index: %d LCE Index: %d\n",
                pGpu->gpuInstance, pGpu->gpuInstance, pceIndex, lceIndex);
    }
    FOR_EACH_INDEX_IN_MASK_END;

    // Store lceMask in the exposeCeMask before moving on
    *pLocalExposeCeMask |= lceMask;

    return NV_OK;

}

NV_STATUS
kceGetMappings_GH100
(
    OBJGPU   *pGpu,
    KernelCE *pKCe,
    NVLINK_TOPOLOGY_PARAMS    *pTopoParams,
    NvU32    *pLocalPceLceMap,
    NvU32    *pLocalGrceMap,
    NvU32    *pExposeCeMask
)
{
    NV_STATUS    status         = NV_OK;
    NV_STATUS    statusC2C      = NV_OK;
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

    //Prepare the per-HSHUB/FBHUB available PCE mask
    kceGetAvailableHubPceMask(pGpu, pTopoParams);

    // Assign PCEs to "PEER"s if nvlink is enabled
    if (pKernelNvlink && !knvlinkIsForcedConfig(pGpu, pKernelNvlink))
    {
        status = kceMapPceLceForNvlinkPeers_HAL(pGpu, pKCe,
                                                pTopoParams->pceAvailableMaskPerHshub,
                                                pLocalPceLceMap,
                                                pExposeCeMask);
    }
    else
    {
        status = NV_WARN_NOTHING_TO_DO;
    }

    // Special C2C cases for LCE 2 and 3
    statusC2C = kceMapPceLceForC2C_HAL(pGpu, pKCe,
                              pTopoParams->pceAvailableMaskPerHshub,
                              pLocalPceLceMap, pExposeCeMask);

    // Assign PCEs for GRCE case
    kceMapPceLceForGRCE_HAL(pGpu, pKCe,
                              pTopoParams->pceAvailableMaskPerHshub,
                              pLocalPceLceMap, pExposeCeMask, pLocalGrceMap, pTopoParams->fbhubPceMask);

    if ((status == NV_WARN_NOTHING_TO_DO && statusC2C == NV_WARN_NOTHING_TO_DO) ||
        (status == NV_ERR_NOT_SUPPORTED && statusC2C == NV_ERR_NOT_SUPPORTED))
    {
        // If there's no NVLink peers available, still expose an additional async LCE
        status = kceMapAsyncLceDefault_HAL(pGpu, pKCe,
                                           pTopoParams->pceAvailableMaskPerHshub,
                                           pLocalPceLceMap,
                                           pExposeCeMask,
                                           NV_CE_NUM_PCES_NO_LINK_CASE);
    }

    NV_PRINTF(LEVEL_INFO, "status = %d, statusC2C = %d\n", status, statusC2C);
    return NV_OK;
}
