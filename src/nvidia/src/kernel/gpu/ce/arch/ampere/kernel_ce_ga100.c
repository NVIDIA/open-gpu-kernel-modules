/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "ctrl/ctrl2080/ctrl2080ce.h"
#include "ctrl/ctrl2080/ctrl2080nvlink.h"
#include "gpu/ce/kernel_ce.h"
#include "gpu/nvlink/kernel_nvlink.h"
#include "gpu/ce/kernel_ce_private.h"
#include "gpu/gpu.h"
#include "gpu/bif/kernel_bif.h"
#include "platform/chipset/chipset.h"

#include "published/ampere/ga100/dev_ce.h"
#include "published/ampere/ga100/dev_nv_xve.h"
#include "published/ampere/ga100/dev_nv_xve_addendum.h"

#define NV_CE_INVALID_TOPO_IDX 0xFFFF

// Ampere +
#define NV_CE_MIN_PCE_PER_SYS_LINK         2
#define NV_CE_MIN_PCE_PER_PEER_LINK        1

// Defines for PCE-LCE mapping algorithm
#define NV_CE_LCE_MASK_INIT                   0xFFFFFFFF
#define NV_CE_SYS_ALLOWED_LCE_MASK            0x0C
#define NV_CE_GRCE_ALLOWED_LCE_MASK           0x03
#define NV_CE_EVEN_ASYNC_LCE_MASK             0x55555550
#define NV_CE_ODD_ASYNC_LCE_MASK              0xAAAAAAA0
#define NV_CE_MAX_LCE_MASK                    0x3FF
#define NV_CE_MAX_GRCE                        2
#define NV_CE_NUM_HSHUB_PCES                  16
#define NV_CE_PCE_STRIDE                      3
#define NV_CE_SYS_LCE_ALLOWED_HSPCE_CONFIG    0x8
#define NV_CE_NUM_DEFAULT_PCES                2

#define NV_CE_SYS_ALLOWED_LCE_MASK            0x0C
#define NV_CE_GRCE_ALLOWED_LCE_MASK           0x03
#define NV_CE_EVEN_ASYNC_LCE_MASK             0x55555550
#define NV_CE_ODD_ASYNC_LCE_MASK              0xAAAAAAA0
#define NV_CE_MAX_LCE_MASK                    0x3FF

static void _ceGetAlgorithmPceIndex(OBJGPU *, KernelCE*, NvU32 *, NvU32 *, NvBool *, NvU8 *);

/*
 * Table for setting the PCE2LCE mapping for WAR configs that cannot be implemented
 * using the algorithm because the config does not conform to the algorithm's set
 * of requirements/assumptions
*/
static NVLINK_CE_AUTO_CONFIG_TABLE nvLinkCeAutoConfigTable_GA100[] =
{

//
//  #systmem #max         #peers     Symmetric Switch     PCE-LCE                          GRCE       exposeCe
//  links   (links/peer)             Config?   Config     Map                              Config     Mask
//

//  Default minimal configuration - NOTE: do not add entrys before this
    {0x0,   0x0,    0x0,    NV_FALSE,    NV_FALSE,  {0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,0xF,
                                                     0xF,0xF,0xF,0xF,0xF,0xF,0xF,0x2,0x3}, {0xF,0xF},  0xF}
};

/*!
 * @brief Returns the size of the PCE2LCE register array
 *
 *
 * @param[in] pGpu  OBJGPU pointer
 * @param[in] pKCe   KernelCE pointer
 *
 * @return  NV_CE_PCE2LCE_CONFIG__SIZE_1
 *
 */
NvU32
kceGetPce2lceConfigSize1_GA100
(
    KernelCE *    pKCe
)
{
    return NV_CE_PCE2LCE_CONFIG__SIZE_1;
}

/**
 *  Return the pce-lce mappings and grce config
 *  reg values when nvlink topology is NOT forced
 *
 * @param[in]  pGpu          OBJGPU pointer
 * @param[in]  pKCe           KernelCE pointer
 * @param[out] pPceLceMap    Stores the pce-lce mappings
 * @param[out] pGrceConfig   Stores the grce configuration
 * @param[out] pExposeCeMask Mask of CEs to expose to clients
 *
 * @return  NV_OK on success
 */
NV_STATUS
kceGetNvlinkAutoConfigCeValues_GA100
(
    OBJGPU   *pGpu,
    KernelCE  *pKCe,
    NvU32    *pPceLceMap,
    NvU32    *pGrceConfig,
    NvU32    *pExposeCeMask
)
{
    KernelNvlink *pKernelNvlink      = GPU_GET_KERNEL_NVLINK(pGpu);
    OBJGPU       *pRemoteGpu         = NULL;
    NV_STATUS     status             = NV_OK;
    NvU32         gpuMask            = 0;
    NvU32         sysmemLinks        = 0;
    NvU32         numPeers           = 0;
    NvBool        bSymmetric         = NV_TRUE;
    NvBool        bCurrentTopoMax    = NV_FALSE;
    NvU32         maxLinksPerPeer    = 0;
    NvU32         gpuInstance        = 0;
    NvU32         topoIdx            = NV_CE_INVALID_TOPO_IDX;
    NvU32         pce2lceConfigSize1 = kceGetPce2lceConfigSize1_HAL(pKCe);
    NvU32         grceConfigSize1    = kceGetGrceConfigSize1_HAL(pKCe);
    NvBool        bEntryExists;
    NvU32         pceIdx, grceIdx, i;
    NVLINK_TOPOLOGY_PARAMS *pCurrentTopo = portMemAllocNonPaged(sizeof(*pCurrentTopo));
    NvU32 *pLocalPceLceMap = NULL, *pLocalGrceConfig = NULL;

    NV_ASSERT_OR_RETURN(pCurrentTopo != NULL, NV_ERR_NO_MEMORY);

    if ((pPceLceMap == NULL) || (pGrceConfig == NULL) || (pExposeCeMask == NULL))
    {
        status = NV_ERR_INVALID_ARGUMENT;
        goto done;
    }

    if (pKernelNvlink == NULL && pGpu->getProperty(pGpu, PDB_PROP_GPU_SKIP_CE_MAPPINGS_NO_NVLINK))
    {
        status = NV_ERR_NOT_SUPPORTED;
        goto done;
    }

    portMemSet(pCurrentTopo, 0, sizeof(*pCurrentTopo));

    // Bug 200283711: Use the largest of all chips in allocating these arrays
    pLocalPceLceMap = portMemAllocNonPaged(sizeof(NvU32[NV2080_CTRL_MAX_PCES]));
    pLocalGrceConfig = portMemAllocNonPaged(sizeof(NvU32[NV2080_CTRL_MAX_GRCES]));
    NvU32 localExposeCeMask = 0;

    if (pLocalPceLceMap == NULL || pLocalGrceConfig == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto done;
    }

    for (i = 0; i < NV2080_CTRL_MAX_PCES; i++)
    {
        pLocalPceLceMap[i] = NV2080_CTRL_CE_UPDATE_PCE_LCE_MAPPINGS_INVALID_LCE;
    }

    for (i = 0; i < NV2080_CTRL_MAX_GRCES; i++)
    {
        pLocalGrceConfig[i] = NV2080_CTRL_CE_UPDATE_PCE_LCE_MAPPINGS_INVALID_LCE;
    }

    sysmemLinks = pKernelNvlink ? knvlinkGetNumLinksToSystem(pGpu, pKernelNvlink) : 0;

    if (gpuGetNumCEs(pGpu) == 0)
    {
        status = NV_ERR_NOT_SUPPORTED;
        goto done;
    }

    (void)gpumgrGetGpuAttachInfo(NULL, &gpuMask);

    // Get the max{nvlinks/peer, for all connected peers}
    while ((pKernelNvlink != NULL) && ((pRemoteGpu = gpumgrGetNextGpu(gpuMask, &gpuInstance)) != NULL))
    {
        NvU32 numLinksToPeer = knvlinkGetNumLinksToPeer(pGpu, pKernelNvlink,
                                                        pRemoteGpu);
        if (numLinksToPeer == 0)
        {
            continue;
        }

        numPeers++;

        //
        // The topology remains symmetric if this is either the first GPU we've
        // seen connected over NVLINK, or the number of links connected to this
        // peer is the same as the maximum number of links connected to any peer
        // seen so far.
        //
        bSymmetric = (bSymmetric &&
                     ((maxLinksPerPeer == 0) ||
                      (maxLinksPerPeer == numLinksToPeer)));

        if (numLinksToPeer > maxLinksPerPeer)
        {
            maxLinksPerPeer = numLinksToPeer;
        }
    }

    pCurrentTopo->sysmemLinks     = sysmemLinks;
    pCurrentTopo->maxLinksPerPeer = maxLinksPerPeer;
    pCurrentTopo->numPeers        = numPeers;
    pCurrentTopo->bSymmetric      = bSymmetric;
    pCurrentTopo->bSwitchConfig   = pKernelNvlink ? knvlinkIsGpuConnectedToNvswitch(pGpu, pKernelNvlink) : NV_FALSE;

    //
    // Check if the current config exists in the table
    // Here, we only fill exposeCeMask.
    //
    bEntryExists = kceGetAutoConfigTableEntry_HAL(pGpu, pKCe, pCurrentTopo, nvLinkCeAutoConfigTable_GA100,
                                                 NV_ARRAY_ELEMENTS(nvLinkCeAutoConfigTable_GA100),
                                                 &topoIdx, &localExposeCeMask);
    if (bEntryExists)
    {
        // Since entry exists, fill local variables with the associated table entry
        for (pceIdx = 0; pceIdx < pce2lceConfigSize1; pceIdx++)
        {
            pLocalPceLceMap[pceIdx] = nvLinkCeAutoConfigTable_GA100[topoIdx].pceLceMap[pceIdx];
        }
        for (grceIdx = 0; grceIdx < grceConfigSize1; grceIdx++)
        {
            pLocalGrceConfig[grceIdx] = nvLinkCeAutoConfigTable_GA100[topoIdx].grceConfig[grceIdx];
        }
    }
    else
    {
        //
        // There is no table entry - use algorithm to determine mapping
        // Here the currentTopo struct comes with pce-lce & grce mappings & exposeCeMask
        //

        status = kceGetMappings_HAL(pGpu, pKCe, pCurrentTopo,
                                   pLocalPceLceMap, pLocalGrceConfig, &localExposeCeMask);
    }

    // Get the largest topology that has been cached
    bEntryExists = gpumgrGetSystemNvlinkTopo(gpuGetDBDF(pGpu), pCurrentTopo);

    // Is this the largest topology that we've ever seen compared to the cached one?
    bCurrentTopoMax = kceIsCurrentMaxTopology_HAL(pGpu, pKCe, pCurrentTopo, &localExposeCeMask, &topoIdx);

    if (bCurrentTopoMax)
    {
        //
        // Replace cached state with current config
        // Store the state globally in gpumgr so that we can preserve the topology
        // info across GPU loads.
        // Preserving across GPU loads enables UVM to optimize perf
        //
        for (pceIdx = 0; pceIdx < pce2lceConfigSize1; pceIdx++)
        {
            pCurrentTopo->maxPceLceMap[pceIdx] = pLocalPceLceMap[pceIdx];
        }
        for (grceIdx = 0; grceIdx < grceConfigSize1; grceIdx++)
        {
            pCurrentTopo->maxGrceConfig[grceIdx] = pLocalGrceConfig[grceIdx];
        }
        pCurrentTopo->maxExposeCeMask = localExposeCeMask;

        if (topoIdx != NV_CE_INVALID_TOPO_IDX)
        {
            // Only if we used table to determine config, store this value
            pCurrentTopo->maxTopoIdx      = topoIdx;
            pCurrentTopo->sysmemLinks     = nvLinkCeAutoConfigTable_GA100[topoIdx].sysmemLinks;
            pCurrentTopo->maxLinksPerPeer = nvLinkCeAutoConfigTable_GA100[topoIdx].maxLinksPerPeer;
            pCurrentTopo->numPeers        = nvLinkCeAutoConfigTable_GA100[topoIdx].numPeers;
            pCurrentTopo->bSymmetric      = nvLinkCeAutoConfigTable_GA100[topoIdx].bSymmetric;
            pCurrentTopo->bSwitchConfig   = nvLinkCeAutoConfigTable_GA100[topoIdx].bSwitchConfig;
        }
        gpumgrUpdateSystemNvlinkTopo(gpuGetDBDF(pGpu), pCurrentTopo);
    }

    NV_PRINTF(LEVEL_INFO, "GPU%d : RM Configured Values for CE Config\n", gpuGetInstance(pGpu));

    // Now, fill up the information to return. We'll always return max config information.
    for (pceIdx = 0; pceIdx < pce2lceConfigSize1; pceIdx++)
    {
        pPceLceMap[pceIdx] = pCurrentTopo->maxPceLceMap[pceIdx];
        NV_PRINTF(LEVEL_INFO, "PCE-LCE map: PCE %d LCE 0x%x\n", pceIdx, pPceLceMap[pceIdx]);
    }

    for (grceIdx = 0; grceIdx < grceConfigSize1; grceIdx++)
    {
        NvU32 grceSharedLce = pCurrentTopo->maxGrceConfig[grceIdx];

        if (grceSharedLce != 0xF)
        {
            // GRCE is shared
            pGrceConfig[grceIdx] = DRF_NUM(_CE, _GRCE_CONFIG, _SHARED, 1) |
                                   DRF_NUM(_CE, _GRCE_CONFIG, _SHARED_LCE, grceSharedLce);
        }
        else
        {
            // GRCE got its own PCE
            pGrceConfig[grceIdx] = DRF_NUM(_CE, _GRCE_CONFIG, _SHARED, 0);
        }
        NV_PRINTF(LEVEL_INFO, "GRCE Config: GRCE %d LCE 0x%x\n", grceIdx, pGrceConfig[grceIdx]);
    }

    *pExposeCeMask = pCurrentTopo->maxExposeCeMask;
    NV_PRINTF(LEVEL_INFO, "exposeCeMask = 0x%x\n", *pExposeCeMask);

done:
    portMemFree(pCurrentTopo);
    portMemFree(pLocalPceLceMap);
    portMemFree(pLocalGrceConfig);

    return status;
}

/**
 * @brief Check if current config's topology is larger than cached one
 *        Return NV_TRUE if yes, else return NV_FALSE
 *
 * @param[in]      pGpu                OBJGPU pointer
 * @param[in]      pKCe                 KernelCE pointer
 * @param[in]      pCurrentTopo        NVLINK_TOPOLOGY_INFO pointer
 * @param[in]      pLocalExposeCeMask  Pointer to caller HSHUB ID
 * @param[in/out]  pTopoIdx            NvU32 pointer to topology index, if it exists
 */
NvBool
kceIsCurrentMaxTopology_GA100
(
    OBJGPU                 *pGpu,
    KernelCE *                  pKCe,
    NVLINK_TOPOLOGY_PARAMS * pCurrentTopo,
    NvU32                   *pLocalExposeCeMask,
    NvU32                   *pTopoIdx
)
{
    if (pCurrentTopo->maxExposeCeMask & ~(*pLocalExposeCeMask))
    {
        //
        // Current config's exposeCeMask is a subset of cached maxExposeCeMask
        // Hence, we will return NV_FALSE and use cached state config
        //
        if (*pTopoIdx != NV_CE_INVALID_TOPO_IDX)
        {
            *pTopoIdx = pCurrentTopo->maxTopoIdx;
        }
        *pLocalExposeCeMask = pCurrentTopo->maxExposeCeMask;
        return NV_FALSE;
    }

    //
    // Current config is equal or a superset of cached maxExposeCeMask
    // This means that the topology has increased and hence we should
    // cache the current config as the max config. Return NV_TRUE to do so
    //
    return NV_TRUE;
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
 * @param[out]  pFirstIter                  Pointer to iteration value
 */
static void
_ceGetAlgorithmPceIndex
(
    OBJGPU     *pGpu,
    KernelCE*      pKCe,
    NvU32       *pceAvailableMaskPerHshub,
    NvU32       *pceIndex,
    NvBool      *pBFirstIter,
    NvU8        *pHshubId
)
{
    NV_STATUS status = NV_OK;

    // 1. Apply PCE striding
    if ((*pBFirstIter) != NV_TRUE)
    {
        *pceIndex += NV_CE_PCE_STRIDE;
    }
    *pBFirstIter = NV_FALSE;

    if(!(NVBIT32(*pceIndex) & pceAvailableMaskPerHshub[*pHshubId]))
    {
        //
        // 2. We couldn't find an applicable strided PCE in given HSHUB
        // So, we'll assign the next consecutive PCE on the same HSHUB
        //
        *pceIndex = CE_GET_LOWEST_AVAILABLE_IDX(pceAvailableMaskPerHshub[*pHshubId]);
        if(!(NVBIT32(*pceIndex) & pceAvailableMaskPerHshub[*pHshubId]))
        {
            // 3. If this is not a valid PCE on given HSHUB, assign PCE from alternative HSHUB
            KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

            if (pKernelNvlink != NULL)
            {
                NV2080_CTRL_INTERNAL_HSHUB_NEXT_HSHUB_ID_PARAMS params;

                portMemSet(&params, 0, sizeof(params));
                params.hshubId = *pHshubId;

                status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                             NV2080_CTRL_CMD_INTERNAL_HSHUB_NEXT_HSHUB_ID,
                                             (void *)&params, sizeof(params));

                NV_ASSERT_OK(status);
                if (status == NV_OK)
                {
                    *pHshubId = params.hshubId;
                }
            }

            *pceIndex = CE_GET_LOWEST_AVAILABLE_IDX(pceAvailableMaskPerHshub[*pHshubId]);
            if(!(NVBIT32(*pceIndex) & pceAvailableMaskPerHshub[*pHshubId]))
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
 * @brief Use the algorithm to determine all the mappings for
 *        the given GPU.
 *
 * @param[in]   pGpu                   OBJGPU pointer
 * @param[in]   pKCe                    KernelCE pointer
 * @param[out]  pLocalPceLceMap        Pointer to PCE-LCE array
 * @param[out]  pLocalGrceMap          Pointer to GRCE array
 * @param[out]  pLocalExposeCeMask     Pointer to LCE Mask
 *
 * Returns NV_TRUE if algorithm ran to completion with no erros
 */

NV_STATUS
kceGetMappings_GA100
(
    OBJGPU   *pGpu,
    KernelCE *pKCe,
    NVLINK_TOPOLOGY_PARAMS    *pTopoParams,
    NvU32    *pLocalPceLceMap,
    NvU32    *pLocalGrceMap,
    NvU32    *pExposeCeMask
)
{
    NvU32       lceMask           = 0;
    NvU32       fbPceMask         = 0;
    NV_STATUS   status            = NV_OK;
    NvU32       pceIndex, lceIndex, grceIdx;
    KernelNvlink *pKernelNvlink      = GPU_GET_KERNEL_NVLINK(pGpu);

    if (!pKernelNvlink || knvlinkIsForcedConfig(pGpu, pKernelNvlink))
    {
        return NV_OK;
    }

    // Prepare the per-HSHUB/FBHUB available PCE mask
    status = kceGetAvailableHubPceMask(pGpu, pKCe, pTopoParams);

    // A. Start with assigning PCEs for "SYSMEM"
    status = kceMapPceLceForSysmemLinks_HAL(pGpu, pKCe,
                                            pTopoParams->pceAvailableMaskPerConnectingHub,
                                            pLocalPceLceMap,
                                            pExposeCeMask,
                                            pTopoParams->fbhubPceMask);
    if (status == NV_ERR_NOT_SUPPORTED)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "No sysmem connections on this chip (PCIe or NVLink)!\n");
    }

    // B. Assign PCEs to "PEER"s
    status = kceMapPceLceForNvlinkPeers_HAL(pGpu, pKCe,
                                            pTopoParams->pceAvailableMaskPerConnectingHub,
                                            pLocalPceLceMap,
                                            pExposeCeMask);
    if (status == NV_WARN_NOTHING_TO_DO)
    {
        // If there's no NVLink peers available, still expose an additional async LCE
        status = kceMapAsyncLceDefault_HAL(pGpu, pKCe,
                                           pTopoParams->pceAvailableMaskPerConnectingHub,
                                           pLocalPceLceMap,
                                           pExposeCeMask,
                                           NV_CE_NUM_DEFAULT_PCES);
    }

    // C. Lastly, do the assignment for "GRCE"s
    lceMask = kceGetGrceSupportedLceMask_HAL(pGpu, pKCe);

    // Get the FBHUB PCE mask
    fbPceMask = pTopoParams->fbhubPceMask;

    // Store lceMask in the exposeCeMask before moving on
    *pExposeCeMask |= lceMask;

    for (grceIdx = 0; grceIdx < NV_CE_MAX_GRCE; grceIdx++)
    {
        // We should have fbhub PCEs left to assign, but avoid an invalid pceIndex if we do not
        NV_ASSERT_OR_ELSE(fbPceMask != 0, break);

        //
        // Check if we are sharing GRCEs
        // On Ampere, GRCEs can only use FBHUB PCEs
        // So, we need to check if the FBHUB PCEs have already been assigned.
        //
        pceIndex = CE_GET_LOWEST_AVAILABLE_IDX(fbPceMask);
        fbPceMask &= (~(NVBIT32(pceIndex)));

        if ((NVBIT32(pLocalPceLceMap[pceIndex])) & *pExposeCeMask)
        {
            // GRCE is shared - set the status and shared LCE # in register field
            lceIndex = pLocalPceLceMap[pceIndex];
            pLocalGrceMap[grceIdx] = DRF_NUM(_CE, _GRCE_CONFIG, _SHARED, 1) |
                                     DRF_NUM(_CE, _GRCE_CONFIG, _SHARED_LCE, lceIndex);

            if ((kceIsGenXorHigherSupported_HAL(pGpu, pKCe, 4)) || (pKCe->bUseGen4Mapping == NV_TRUE))
            {
                kceApplyGen4orHigherMapping_HAL(pGpu, pKCe,
                                                &pLocalPceLceMap[0],
                                                &pTopoParams->pceAvailableMaskPerConnectingHub[0],
                                                lceIndex,
                                                pceIndex);
            }
        }
        else
        {
            // GRCE got its own FBHUB PCE
            // Store the LCE in associated PCE for GRCE
            lceIndex = CE_GET_LOWEST_AVAILABLE_IDX(lceMask);
            pLocalPceLceMap[pceIndex] = lceIndex;
            lceMask &= (~(NVBIT32(lceIndex)));
            // Reflect non-sharing status in register field
            pLocalGrceMap[grceIdx] = DRF_NUM(_CE, _GRCE_CONFIG, _SHARED, 0) |
                                     DRF_DEF(_CE, _GRCE_CONFIG, _SHARED_LCE, _NONE);
        }
    }

    return NV_OK;
}

/**
 * @brief This function assigns PCE-LCE mappings for sysmem
 *        for the following two cases -
 *        1. PCIe links - assign FBHUB PCEs
 *        2. NVLinks    - assign HSHUB PCEs
 *        If sysLinkMask is 0, then we assume that sysmem is over PCIe.
 *        Else, follow step 2 as above.
 *
 * @param[in]   pGpu                        OBJGPU pointer
 * @param[in]   pKCe                         KernelCE pointer
 * @param[in]   pceAvailableMaskPerHshub    Pointer to CEs available per HSHUB
 * @param[out]  pLocalPceLceMap             Pointer to PCE-LCE array
 * @param[out]  pLocalExposeCeMask          Pointer to LCE Mask
 *
 * Returns NV_OK if successful in assigning PCEs and LCEs for sysmem links
 */
NV_STATUS
kceMapPceLceForSysmemLinks_GA100
(
    OBJGPU  *pGpu,
    KernelCE   *pKCe,
    NvU32   *pceAvailableMaskPerHshub,
    NvU32   *pLocalPceLceMap,
    NvU32   *pLocalExposeCeMask,
    NvU32   fbPceMask
)
{
    NvU32   lceMask      = 0;
    NvU32   pceMask      = 0;
    NvU32   numTotalPces = 0;
    NvBool  bFirstIter   = NV_FALSE;
    NvU32   numPcePerLink, tempFbPceMask;
    NvU32   lceIndex, pceIndex;
    NvU32   linkId, i;
    NvU8    hshubId;
    NV_STATUS status = NV_OK;

    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

    NV2080_CTRL_INTERNAL_NVLINK_HSHUB_GET_SYSMEM_NVLINK_MASK_PARAMS paramsNvlinkMask;
    NV2080_CTRL_INTERNAL_HSHUB_GET_HSHUB_ID_FOR_LINKS_PARAMS *pParamsHshubId = NULL;

    NV_ASSERT_OR_RETURN(pKernelNvlink != NULL, NV_ERR_NOT_SUPPORTED);

    portMemSet(&paramsNvlinkMask, 0, sizeof(paramsNvlinkMask));
    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_INTERNAL_NVLINK_HSHUB_GET_SYSMEM_NVLINK_MASK,
                                 (void *)&paramsNvlinkMask, sizeof(paramsNvlinkMask));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Unable to determine PCEs and LCEs for sysmem links\n");
        return status;
    }

    lceMask = kceGetSysmemSupportedLceMask_HAL(pGpu, pKCe);

    //
    // Assign FBHUB PCEs when sysmem is over PCIE because PCIE
    // accesses are not supported over HSHUB PCEs
    //
    if (paramsNvlinkMask.sysmemLinkMask == 0)
    {
        // Store lceMask in the exposeCeMask before moving on
        *pLocalExposeCeMask |= lceMask;

        tempFbPceMask = fbPceMask;
        while(tempFbPceMask)
        {
            lceIndex = CE_GET_LOWEST_AVAILABLE_IDX(lceMask);
            pceIndex = CE_GET_LOWEST_AVAILABLE_IDX(tempFbPceMask);
            pLocalPceLceMap[pceIndex] = lceIndex;
            // Clear the lowest set bits to get to the next index
            tempFbPceMask &= (tempFbPceMask - 1);
            lceMask &= (lceMask - 1);
        }

        return NV_OK;
    }

    // If sysmem is over NVlink, assign HSHUB PCEs
    numPcePerLink = NV_CE_MIN_PCE_PER_SYS_LINK;

    pParamsHshubId = portMemAllocNonPaged(sizeof(*pParamsHshubId));

    NV_ASSERT_OR_RETURN(pParamsHshubId != NULL, NV_ERR_NO_MEMORY);

    portMemSet(pParamsHshubId, 0, sizeof(*pParamsHshubId));
    pParamsHshubId->linkMask = paramsNvlinkMask.sysmemLinkMask;

    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_INTERNAL_HSHUB_GET_HSHUB_ID_FOR_LINKS,
                                 pParamsHshubId, sizeof(*pParamsHshubId));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Unable to determine Hshub Id for sysmem links");
        goto done;
    }

    FOR_EACH_INDEX_IN_MASK(32, linkId, paramsNvlinkMask.sysmemLinkMask)
    {
        hshubId = pParamsHshubId->hshubIds[linkId];
        pceIndex = CE_GET_LOWEST_AVAILABLE_IDX(pceAvailableMaskPerHshub[hshubId]);
        bFirstIter = NV_TRUE;
        for (i = 0; i < numPcePerLink; i++)
        {
            _ceGetAlgorithmPceIndex(pGpu, pKCe, pceAvailableMaskPerHshub, &pceIndex, &bFirstIter, &hshubId);
            pceMask |= NVBIT32(pceIndex);
            numTotalPces++;
            // Clear out the assigned PCE
            pceAvailableMaskPerHshub[hshubId] &= (~(NVBIT32(pceIndex)));
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

    //
    // Now, enter the PCE-LCE assignment - alternatively assign PCEs
    // to each of the 2 LCEs for sysmem
    //
    for (i = 0; i < (numTotalPces/NV_CE_MIN_PCE_PER_SYS_LINK); i++)
    {
        NvU32 tempLceMask = lceMask;
        while(tempLceMask)
        {
            pceIndex = CE_GET_LOWEST_AVAILABLE_IDX(pceMask);
            lceIndex = CE_GET_LOWEST_AVAILABLE_IDX(tempLceMask);

            pLocalPceLceMap[pceIndex] = lceIndex;

            pceMask &= (pceMask - 1);
            tempLceMask &= (tempLceMask - 1);
        }

        // Store lceMask in the exposeCeMask before moving on
        *pLocalExposeCeMask |= lceMask;
    }

done:
    portMemFree(pParamsHshubId);

    return status;
}

/**
 * @brief Returns mask of LCEs that can be assigned to sysmem connection
 *        where the index of corresponding set bit indicates the LCE index
 *
 * @param[in]   pGpu                   OBJGPU pointer
 * @param[in]   pKCe                    KernelCE pointer
 *
 * Returns the mask of LCEs valid for SYSMEM connections
 */
NvU32
kceGetSysmemSupportedLceMask_GA100
(
    OBJGPU *pGpu,
    KernelCE   *pKCe
)
{
    return (NV_CE_SYS_ALLOWED_LCE_MASK & NV_CE_MAX_LCE_MASK);
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
kceMapPceLceForNvlinkPeers_GA100
(
    OBJGPU  *pGpu,
    KernelCE   *pKCe,
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
    NvBool        bFirstIter    = NV_FALSE;
    NvBool        bPeerAssigned = NV_FALSE;
    NvU32         peerAvailableLceMask = NV_CE_LCE_MASK_INIT;
    OBJGPU       *pRemoteGpu;
    NvU32         numPcePerLink;
    NvU32         lceIndex, pceIndex;
    NvU32         linkId, gpuMask, gpuInstance = 0, i;
    NvU8          hshubId, prevHshubId;
    NV2080_CTRL_INTERNAL_HSHUB_GET_HSHUB_ID_FOR_LINKS_PARAMS *pParams = NULL;

    if (pKernelNvlink == NULL)
    {
        return NV_WARN_NOTHING_TO_DO;
    }

    pParams = portMemAllocNonPaged(sizeof(*pParams));

    NV_ASSERT_OR_RETURN(pParams != NULL, NV_ERR_NO_MEMORY);

    peerAvailableLceMask = kceGetNvlinkPeerSupportedLceMask_HAL(pGpu, pKCe, peerAvailableLceMask);

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

        // Clear out the chosen LCE
        peerAvailableLceMask &= (~(NVBIT32(lceIndex)));

        peerLinkMask = knvlinkGetLinkMaskToPeer(pGpu, pKernelNvlink, pRemoteGpu);

        if (peerLinkMask == 0)
        {
            NV_PRINTF(LEVEL_INFO, "GPU%d has nvlink disabled. Skip programming\n", pRemoteGpu->gpuInstance);
            continue;
        }

        numPcePerLink = NV_CE_MIN_PCE_PER_PEER_LINK;
        prevHshubId = 0xFF;

        portMemSet(pParams, 0, sizeof(*pParams));
        pParams->linkMask = peerLinkMask;

        status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                     NV2080_CTRL_CMD_INTERNAL_HSHUB_GET_HSHUB_ID_FOR_LINKS,
                                     pParams, sizeof(*pParams));
        NV_ASSERT_OK_OR_RETURN(status);

        FOR_EACH_INDEX_IN_MASK(32, linkId, peerLinkMask)
        {
            hshubId = pParams->hshubIds[linkId];
            if (hshubId != prevHshubId)
            {
                pceIndex = CE_GET_LOWEST_AVAILABLE_IDX(pceAvailableMaskPerHshub[hshubId]);
                bFirstIter = NV_TRUE;
            }
            for (i = 0; i < numPcePerLink; i++)
            {
                _ceGetAlgorithmPceIndex(pGpu, pKCe, pceAvailableMaskPerHshub, &pceIndex, &bFirstIter, &hshubId);
                pceMask |= NVBIT32(pceIndex);
                // Clear out the assigned PCE
                pceAvailableMaskPerHshub[hshubId] &= (~(NVBIT32(pceIndex)));
                prevHshubId = hshubId;
            }
        }
        FOR_EACH_INDEX_IN_MASK_END;

        // Now, assign the PCE-LCE association for the current peer
        if (pceMask != 0)
        {
            // We just need atleast one peer to set this to TRUE
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

    portMemFree(pParams);
    return status;
}

/**
 * @brief This function assigns 2 PCEs to an additional LCE over the GRCEs and async LCEs
 *        for sysmem, since some clients rely on LCE 4 also being turned on when there
 *        are no NVLink peers
 *
 * @param[in]   pGpu                        OBJGPU pointer
 * @param[in]   pKCe                         KernelCE pointer
 * @param[in]   pceAvailableMaskPerHshub    Pointer to CEs available per HSHUB
 * @param[out]  pLocalPceLceMap             Pointer to PCE-LCE array
 * @param[out]  pLocalExposeCeMask          Pointer to LCE Mask
 *
 * Returns NV_OK if successful in assigning PCEs to a default async LCE (>= 4)
 */
NV_STATUS
kceMapAsyncLceDefault_GA100
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

    // Pick from the 1st HSHUB - HSHUB 0 will not be floorswept
    hshubId = 0;

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

/**
 * @brief Returns mask of LCEs that can be assigned to NVLink peers
 *        where the index of corresponding set bit indicates the LCE index
 *
 * @param[in]   pGpu                   OBJGPU pointer
 * @param[in]   pKCe                    KernelCE pointer
 *
 * Returns the mask of LCEs valid for NVLink peers
 */
NvU32
kceGetNvlinkPeerSupportedLceMask_GA100
(
    OBJGPU  *pGpu,
    KernelCE   *pKCe,
    NvU32   peerAvailableLceMask
)
{
    //
    // Start with assigning even async LCEs first as they are sized to accommodate
    // more no. of PCEs versus odd async LCEs
    // Hence, if caller is using this call to get 1st set of async LCEs for NVLink
    // peers, then caller should initialize peerAvailableLceMask to NV_CE_LCE_MASK_INIT.
    // Else we will run out of async LCEs since we will directly assign the odd async LCEs
    // and there's no wraparound or detection mechanism is place.
    //
    if (peerAvailableLceMask == NV_CE_LCE_MASK_INIT)
    {
        return (NV_CE_EVEN_ASYNC_LCE_MASK & NV_CE_MAX_LCE_MASK);
    }

    return NV_CE_ODD_ASYNC_LCE_MASK & NV_CE_MAX_LCE_MASK;
}

/**
 * @brief Returns mask of LCEs that can be assigned for GRCEs
 *        where the index of corresponding set bit indicates the LCE index
 *
 * @param[in]   pGpu                   OBJGPU pointer
 * @param[in]   pKCe                    KernelCE pointer
 *
 * Returns the mask of LCEs valid for GRCEs
 */
NvU32
kceGetGrceSupportedLceMask_GA100
(
    OBJGPU *pGpu,
    KernelCE   *pKCe
)
{
    return (NV_CE_GRCE_ALLOWED_LCE_MASK & NV_CE_MAX_LCE_MASK);
}

/**
 * @brief This function checks for root port gen speed or GPU
 *        gen speed to determine if we should apply genX+ mapping
 *
 * @param[in]   pGpu            OBJGPU pointer
 * @param[in]   pKCe            KernelCE pointer
 * @param[in]   checkGen        genX for query
 */
NvBool
kceIsGenXorHigherSupported_GA100
(
    OBJGPU *pGpu,
    KernelCE  *pKCe,
    NvU32  checkGen
)
{
    OBJSYS    *pSys = SYS_GET_INSTANCE();
    KernelBif *pKernelBif = GPU_GET_KERNEL_BIF(pGpu);
    OBJCL     *pCl = SYS_GET_CL(pSys);
    NvU8       genSpeed = 0;
    NV_STATUS  status = NV_OK;
    NvBool     bIsGenXorHigher = NV_FALSE;
    NvU32      regVal;

    if (IS_PASSTHRU(pGpu) && (pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_PCIE_GEN4_CAPABLE)))
    {
        //
        // On passthrough the root port is commonly not accessible or fake. To
        // handle this case, we support the hypervisor explicitly communicating
        // the speed to us through emulated config space. See
        // bug 2927491 for more details.
        //
        NvU32 passthroughEmulatedConfig = osPciReadDword(osPciInitHandle(gpuGetDomain(pGpu),
                                                                         gpuGetBus(pGpu),
                                                                         gpuGetDevice(pGpu),
                                                                         0, NULL, NULL),
                                                         NV_XVE_PASSTHROUGH_EMULATED_CONFIG);
        NvU32 rootPortSpeed = DRF_VAL(_XVE, _PASSTHROUGH_EMULATED_CONFIG, _ROOT_PORT_SPEED, passthroughEmulatedConfig);

        // 0 means the config is not being emulated and we assume gen4
        bIsGenXorHigher = ((rootPortSpeed == 0 && checkGen <= 4) || rootPortSpeed >= checkGen);

        if (rootPortSpeed != 0)
        {
            NV_PRINTF(LEVEL_INFO, "Root port speed from emulated config space = %d\n", rootPortSpeed);
        }
    }
    else
    {
        status = clPcieGetRootGenSpeed(pGpu, pCl, &genSpeed);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Could not get root gen speed - check for GPU gen speed!\n");
            // Check for GPU gen speed
            regVal = GPU_REG_RD32(pGpu, DEVICE_BASE(NV_PCFG) + NV_XVE_LINK_CONTROL_STATUS);
            genSpeed = DRF_VAL(_XVE, _LINK_CONTROL_STATUS, _LINK_SPEED, regVal);
        }
        NV_PRINTF(LEVEL_INFO, "Gen Speed = %d\n", genSpeed);

        if ((genSpeed >= checkGen))
        {
            bIsGenXorHigher = NV_TRUE;
        }
    }
    return bIsGenXorHigher;
}

/**
 * @brief This function applies the gen4+ mapping i.e. switches the LCE passed in
 *        to use an HSHUB PCE (if available) instead of an FBHUB PCE.
 *
 * @param[in]   pGpu                        OBJGPU pointer
 * @param[in]   pKCe                         KernelCE pointer
 * @param[in]   pLocalPceLceMap             Pointer to PCE-LCE array
 * @param[in]   pceAvailableMaskPerHshub    Pointer to CEs available per HSHUB
 * @param[in]   lceIndex                    LCE index for which mapping is being determined
 * @param[in]   currPceIndex                PCE index for which mapping is being determined
 */
void
kceApplyGen4orHigherMapping_GA100
(
    OBJGPU *pGpu,
    KernelCE  *pKCe,
    NvU32  *pLocalPceLceMap,
    NvU32  *pceAvailableMaskPerHshub,
    NvU32   lceIndex,
    NvU32   currPceIndex
)
{
    NvBool hsPceAssigned = NV_FALSE;
    NvU32  fbPceIndex, hshubId;
    NV_STATUS status;

    if (NVBIT32(lceIndex) & NV_CE_SYS_LCE_ALLOWED_HSPCE_CONFIG)
    {
        RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
        NV2080_CTRL_INTERNAL_HSHUB_GET_NUM_UNITS_PARAMS params;
        portMemSet(&params, 0, sizeof(params));

        NV_ASSERT_OK_OR_ELSE(status,
            pRmApi->Control(pRmApi,
                            pGpu->hInternalClient,
                            pGpu->hInternalSubdevice,
                            NV2080_CTRL_CMD_INTERNAL_HSHUB_GET_NUM_UNITS,
                            &params,
                            sizeof(params)),
            params.numHshubs = 0);

        // GA100: If LCE 3, then move this to an HSHUB PCE, if available
        fbPceIndex = currPceIndex;
        for (hshubId = 0; hshubId < params.numHshubs; hshubId++)
        {
            if (pceAvailableMaskPerHshub[hshubId] != 0)
            {
                // We still have HS PCEs available
                hsPceAssigned = NV_TRUE;
                currPceIndex = CE_GET_LOWEST_AVAILABLE_IDX(pceAvailableMaskPerHshub[hshubId]);
                pLocalPceLceMap[currPceIndex] = lceIndex;
                // Clear out the assigned PCE
                pceAvailableMaskPerHshub[hshubId] &= (~(NVBIT32(currPceIndex)));
                break;
            }
        }
    }
    if (hsPceAssigned == NV_TRUE)
    {
        pLocalPceLceMap[fbPceIndex] = NV_CE_PCE2LCE_CONFIG_PCE_ASSIGNED_LCE_NONE;
    }
}
