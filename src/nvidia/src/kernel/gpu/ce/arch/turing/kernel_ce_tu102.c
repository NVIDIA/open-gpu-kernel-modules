/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "published/turing/tu102/dev_ce.h"

#define NVLINK_CE_AUTO_CONFIG_TABLE_DEFAULT_ENTRY      0

#define MAX_CE_CNT 15

/*
 * sysmemLinks
 *    Represents the number of sysmem links detected
 *    This affects how many PCEs LCE0(sysmem read CE)
 *    and LCE1(sysmem write CE) should be mapped to
 * maxLinksPerPeer
 *    Represents the maximum number of peer links
 *    between this GPU and all its peers. This affects
 *    how many PCEs LCE3(P2P CE) should be mapped to
 * numPeers
 *    Represents the number of Peer GPUs discovered so far
 * bSymmetric
 *    Represents whether the topology detected so far
 *    is symmetric i.e. has same number of links to all
 *    peers connected through nvlink. This affects how
 *    many PCEs to assign to LCEs3-5 (nvlink P2P CEs)
 * bSwitchConfig
 *    Represents whether the config listed is intended
 *    for use with nvswitch systems
 * pceLceMap
 *    Value of NV_CE_PCE2LCE_CONFIG0 register with the
 *    above values for sysmemLinks, maxLinksPerPeer,
 *    numLinks and bSymmetric
 * grceConfig
 *    Value of NV_CE_GRCE_CONFIG register with the
 *    above values for sysmemLinks, maxLinksPerPeer,
 *    numLinks and bSymmetric
 * exposeCeMask
 *    Mask of CEs to expose to clients for the above
 *    above values for sysmemLinks, maxLinksPerPeer,
 *    numLinks and bSymmetric
 */
typedef struct
{
    NvU32  sysmemLinks;
    NvU32  maxLinksPerPeer;
    NvU32  numPeers;
    NvBool bSymmetric;
    NvBool bSwitchConfig;
    NvU32  pceLceMap[MAX_CE_CNT];
    NvU32  grceConfig[MAX_CE_CNT];
    NvU32  exposeCeMask;
} NVLINK_CE_AUTO_CONFIG_TABLE;

/*
 * Table for setting the PCE2LCE mapping
*/
static NVLINK_CE_AUTO_CONFIG_TABLE nvLinkCeAutoConfigTable_TU102[] =
{

//
//  #systmem #max         #peers    Symmetric Switch     PCE-LCE                                GRCE         exposeCe
//  links   (links/peer)            Config?   Config     Map                                    Config       Mask
//

//  Default minimal configuration - NOTE: do not add entrys before this
    {0x0,   0x0,         0x0,       NV_TRUE,  NV_FALSE, {0xF,0x4,0x2,0x3,0xF,0xF,0xF,0xF,0xF}, {0x4,0x3},   0x1F},

    {0x0,   0x1,         0x1,       NV_TRUE,  NV_FALSE, {0xF,0x4,0x2,0x3,0xF,0xF,0xF,0xF,0xF}, {0x3,0x4},   0x1F},
    {0x0,   0x2,         0x1,       NV_TRUE,  NV_FALSE, {0x3,0x4,0x4,0x2,0xF,0xF,0xF,0xF,0xF}, {0x3,0x2},   0x1F},
};

/**
 *  Return the pce-lce mappings and grce config
 *  reg values when nvlink topology is NOT forced
 *
 * @param[in]  pGpu          OBJGPU pointer
 * @param[in]  pKCe          KernelCE pointer
 * @param[out] pPceLceMap    Stores the pce-lce mappings
 * @param[out] pGrceConfig   Stores the grce configuration
 * @param[out] pExposeCeMask Mask of CEs to expose to clients
 *
 * @return  NV_OK on success
 */
NV_STATUS
kceGetNvlinkAutoConfigCeValues_TU102
(
    OBJGPU   *pGpu,
    KernelCE *pKCe,
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
    NvU32         maxLinksPerPeer    = 0;
    NvU32         gpuInstance        = 0;
    NvU32         topoIdx            = 0;
    NvU32         pce2lceConfigSize1 = kceGetPce2lceConfigSize1_HAL(pKCe);
    NvU32         grceConfigSize1    = kceGetGrceConfigSize1_HAL(pKCe);
    NvBool        bEntryExists;
    NvU32         pceIdx, grceIdx;
    NVLINK_TOPOLOGY_PARAMS currentTopo = { 0 };

    if ((pPceLceMap == NULL) || (pGrceConfig == NULL) || (pExposeCeMask == NULL))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (pKernelNvlink == NULL)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    // Initialize pPceLceMap with no mappings
    for (pceIdx = 0; pceIdx < pce2lceConfigSize1; pceIdx++)
    {
        pPceLceMap[pceIdx] = NV_CE_PCE2LCE_CONFIG_PCE_ASSIGNED_LCE_NONE;
    }

    sysmemLinks = knvlinkGetNumLinksToSystem(pGpu, pKernelNvlink);

    if (gpuGetNumCEs(pGpu) == 0)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    (void)gpumgrGetGpuAttachInfo(NULL, &gpuMask);

    // Get the max{nvlinks/peer, for all connected peers}
    while ((pRemoteGpu = gpumgrGetNextGpu(gpuMask, &gpuInstance)) != NULL)
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

    currentTopo.sysmemLinks     = sysmemLinks;
    currentTopo.maxLinksPerPeer = maxLinksPerPeer;
    currentTopo.numPeers        = numPeers;
    currentTopo.bSymmetric      = bSymmetric;
    currentTopo.bSwitchConfig   = knvlinkIsGpuConnectedToNvswitch(pGpu, pKernelNvlink);

    // Use largest topology seen by this GPU
    bEntryExists = kceGetNvlinkMaxTopoForTable_HAL(pGpu, pKCe, &currentTopo, nvLinkCeAutoConfigTable_TU102,
                    NV_ARRAY_ELEMENTS(nvLinkCeAutoConfigTable_TU102), &topoIdx);

    if (!bEntryExists)
    {
        NV_PRINTF(LEVEL_INFO,
                  "GPU%d : NVLINK config not found in PCE2LCE table - using default entry\n",
                  gpuGetInstance(pGpu));
        topoIdx = NVLINK_CE_AUTO_CONFIG_TABLE_DEFAULT_ENTRY;
    }

    //
    // Store the state globally in gpumgr so that we can preserve the topology
    // info across GPU loads.
    // Preserving across GPU loads enables UVM to optimize perf
    //
    currentTopo.sysmemLinks     = nvLinkCeAutoConfigTable_TU102[topoIdx].sysmemLinks;
    currentTopo.maxLinksPerPeer = nvLinkCeAutoConfigTable_TU102[topoIdx].maxLinksPerPeer;
    currentTopo.numPeers        = nvLinkCeAutoConfigTable_TU102[topoIdx].numPeers;
    currentTopo.bSymmetric      = nvLinkCeAutoConfigTable_TU102[topoIdx].bSymmetric;
    currentTopo.bSwitchConfig   = nvLinkCeAutoConfigTable_TU102[topoIdx].bSwitchConfig;

    gpumgrUpdateSystemNvlinkTopo(gpuGetDBDF(pGpu), &currentTopo);

    for (pceIdx = 0; pceIdx < pce2lceConfigSize1; pceIdx++)
    {
        pPceLceMap[pceIdx] = nvLinkCeAutoConfigTable_TU102[topoIdx].pceLceMap[pceIdx];
    }

    for (grceIdx = 0; grceIdx < grceConfigSize1; grceIdx++)
    {
        NvU32 grceSharedLce = nvLinkCeAutoConfigTable_TU102[topoIdx].grceConfig[grceIdx];

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
    }
    *pExposeCeMask = nvLinkCeAutoConfigTable_TU102[topoIdx].exposeCeMask;

    NV_PRINTF(LEVEL_INFO,
              "GPU%d : RM Configured Values for CE Config : pceLceMap = "
              "0x%01x%01x%01x%01x%01x%01x%01x%01x%01x, grceConfig = 0x%01x%01x, "
              "exposeCeMask = 0x%08x gpuMask = 0x%08x\n",
              gpuGetInstance(pGpu), pPceLceMap[8], pPceLceMap[7],
              pPceLceMap[6], pPceLceMap[5], pPceLceMap[4], pPceLceMap[3],
              pPceLceMap[2], pPceLceMap[1], pPceLceMap[0],
              nvLinkCeAutoConfigTable_TU102[topoIdx].grceConfig[1],
              nvLinkCeAutoConfigTable_TU102[topoIdx].grceConfig[0],
              *pExposeCeMask, gpuMask);

    return status;
}

/*!
 * @brief Returns the size of the GRCE_CONFIG register array
 *
 *
 * @param[in] pGpu  OBJGPU pointer
 * @param[in] pCe   OBJCE pointer
 *
 * @return  NV_CE_GRCE_CONFIG__SIZE_1
 *
 */
NvU32 kceGetGrceConfigSize1_TU102(KernelCE* kce)
{
    return NV_CE_GRCE_CONFIG__SIZE_1;
}

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
NvU32 kceGetPce2lceConfigSize1_TU102(KernelCE* kce)
{
    return NV_CE_PCE2LCE_CONFIG__SIZE_1;
}
