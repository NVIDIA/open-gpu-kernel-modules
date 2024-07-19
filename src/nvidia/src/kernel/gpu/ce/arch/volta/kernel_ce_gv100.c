/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/ce/kernel_ce_gv100_private.h"
#include "gpu/gpu.h"

NV_STATUS kceGetP2PCes_GV100(KernelCE *pKCe, OBJGPU *pGpu, NvU32 gpuMask, NvU32 *nvlinkP2PCeMask)
{
    NvU32         gpuCount      = gpumgrGetSubDeviceCount(gpuMask);
    NvU32         maxPces = 0;

    *nvlinkP2PCeMask  = 0;

    // If GPU count = 1, return all possible nvlink P2P CEs
    if (gpuCount == 1)
    {
        *nvlinkP2PCeMask |= NVBIT(NVLINK_MIN_P2P_LCE);
        for (NvU32 i = NVLINK_MIN_P2P_LCE; i < gpuGetNumCEs(pGpu); i++)
        {
            *nvlinkP2PCeMask |= NVBIT(i);

        }
    }
    else
    {
        KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
        KernelCE *pKCeMatch    = NULL;
        KernelCE *pKCeSubMatch = NULL;
        KernelCE *pKCeMaxPces  = NULL;
        KernelCE *pTargetCe   = NULL;
        KernelCE *pKCeLoop     = NULL;
        NvU32     numLinks = 0;
        NvU32     gpuInstance   = 0;
        OBJGPU    *pRemoteGpu = NULL;
        NvBool bSwitchConfig = NV_FALSE;

        if (pKernelNvlink != NULL)
        {
            bSwitchConfig = knvlinkIsGpuConnectedToNvswitch(pGpu, pKernelNvlink);

            // Get the remote GPU
            while ((pRemoteGpu = gpumgrGetNextGpu(gpuMask, &gpuInstance)) != NULL)
            {
                if (pRemoteGpu != pGpu)
                    break;
            }

            NV_ASSERT_OR_RETURN(pRemoteGpu != NULL, NV_ERR_INVALID_STATE);
            gpuInstance = gpuGetInstance(pRemoteGpu);

            numLinks = knvlinkGetNumLinksToPeer(pGpu, pKernelNvlink, pRemoteGpu);
        }

        //
        // Look for an LCE that is already assigned to this peer,
        // or greedily allocate the first available one.
        //
        if (numLinks == 0)
        {
            *nvlinkP2PCeMask = NVBIT32(NVLINK_SYSMEM_WRITE_LCE);
            NV_PRINTF(LEVEL_INFO,
                      "GPU %d Peer %d has no links (could be an indirect peer). Sysmem LCE assigned %d!\n",
                      gpuGetInstance(pGpu), gpuInstance, NVLINK_SYSMEM_WRITE_LCE);
            return NV_OK;
        }


        KCE_ITER_BEGIN(pGpu, pKCe, pKCeLoop, NVLINK_MIN_P2P_LCE)
            if (pKCeLoop->bStubbed)
            {
                continue;
            }

            NV2080_CTRL_CE_GET_CE_PCE_MASK_PARAMS params = {0};

            // If we don't find a match, will use LCE with most PCEs
            params.ceEngineType = NV2080_ENGINE_TYPE_COPY(pKCeLoop->publicID);
            NV_STATUS rmStatus = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                                     NV2080_CTRL_CMD_CE_GET_CE_PCE_MASK,
                                                     (void *)&params, sizeof(params));
            NV_ASSERT_OK_OR_RETURN(rmStatus);
            NvU32 numPces = nvPopCount32(params.pceMask);

            //
            // Only save the LCE with the maximum PCEs if it is not
            // already assigned or for nvswitch configurations where
            // all peers will get the max PCE.
            //
            if ((numPces > maxPces) &&
                (bSwitchConfig || (pKCeLoop->nvlinkPeerMask == 0)))
            {
                pKCeMaxPces = pKCeLoop;
                maxPces = numPces;
            }

            if ((pKCeLoop->nvlinkPeerMask & NVBIT(gpuInstance)) != 0)
            {
                // LCE is already assigned to this peer
                pKCeMatch = pKCeLoop;
                break;
            }
            else if (pKCeLoop->nvlinkPeerMask != 0)
            {
                // LCE is assigned to another peer
                continue;
            }
            else
            {
                //
                // LCE is not assigned yet; check if its configuration
                // matches what we need for this peer
                //
                if (numPces == numLinks)
                {
                    pKCeMatch = (pKCeMatch == NULL) ? pKCeLoop : pKCeMatch;
                }
                else if (numPces < numLinks)
                {
                    pKCeSubMatch = (pKCeSubMatch == NULL) ? pKCeLoop : pKCeSubMatch;
                }
            }
        KCE_ITER_END

        //
        // Prioritize an unused LCE with numPce to numLink match
        // then, an unused LCE with numPces < numLinks
        // finally, fall back to the LCE with max numPces
        //
        // For nvswitch, optimal LCE is always LCE with max PCE
        //

        if (pKCeMatch != NULL && !bSwitchConfig)
        {
            pTargetCe = pKCeMatch;
        }
        else if (pKCeSubMatch != NULL && !bSwitchConfig)
        {
            pTargetCe = pKCeSubMatch;
        }
        else if (pKCeMaxPces != NULL)
        {
            pTargetCe = pKCeMaxPces;
        }

        if (pTargetCe != NULL)
        {
            // assign LCE to peer
            if (pTargetCe->nvlinkPeerMask == 0)
            {
                pTargetCe->nvlinkPeerMask = NVBIT(gpuInstance);
            }

            NV_PRINTF(LEVEL_INFO,
                      "GPU %d Assigning Peer %d to LCE %d\n",
                      gpuGetInstance(pGpu), gpuInstance,
                      pTargetCe->publicID);

            *nvlinkP2PCeMask = NVBIT(pTargetCe->publicID);
        }
    }

    return NV_OK;
}

void kceGetSysmemRWLCEs_GV100(OBJGPU *pGpu, KernelCE* pKCe, NvU32 *rd, NvU32 *wr)
{
    *rd = NVLINK_SYSMEM_READ_LCE;
    *wr = NVLINK_SYSMEM_WRITE_LCE;
}

/*
 * Look up entry in NVLINK_CE_AUTO_CONFIG_TABLE
 *
 * @param[in]  pGpu                 OBJGPU pointer
 * @param[in]  pCe                  OBJCE pointer
 * @param[in]  pCurrentTopo         NVLINK_TOPOLOGY_INFO pointer
 * @param[in]  pAutoConfigTable     NVLINK_CE_AUTO_CONFIG_TABLE pointer
 * @param[in]  autoConfigNumEntries NvU32 num entries within pAutoConfigTable
 * @param[out] pIdx                 NvU32 pointer
 * @param[out] pExposeCeMask        NvU32 pointer
 *
 * Returns: NV_TRUE if entry is found
 *        NV_FALSE otheriwse
*/
NvBool
kceGetAutoConfigTableEntry_GV100
(
    OBJGPU                   *pGpu,
    KernelCE                 *pKCe,
    NVLINK_TOPOLOGY_PARAMS   *pCurrentTopo,
    NVLINK_CE_AUTO_CONFIG_TABLE *pTable,
    NvU32                    autoConfigNumEntries,
    NvU32                   *pIdx,
    NvU32                   *pExposeCeMask
)
{
    NvU32 i;

    for (i = 0; i < autoConfigNumEntries; i++)
    {
        if ((pTable[i].sysmemLinks     == pCurrentTopo->sysmemLinks    ) &&
            (pTable[i].maxLinksPerPeer == pCurrentTopo->maxLinksPerPeer) &&
            (pTable[i].bSymmetric      == pCurrentTopo->bSymmetric     ) &&
            (pTable[i].bSwitchConfig   == pCurrentTopo->bSwitchConfig  ) &&
            ((pTable[i].numPeers       == pCurrentTopo->numPeers) || (pCurrentTopo->bSwitchConfig)))
        {
            *pIdx = i;
            *pExposeCeMask = pTable[i].exposeCeMask;
            return NV_TRUE;
        }
    }
    return NV_FALSE;
}

/*
 * Clear the nvlinkPeerMasks from each P2P CE
 *
 * @param[in]   pGpu           OBJGPU pointer
 * @param[in]   pCe            OBJCE pointer
 */
void
kceClearAssignedNvlinkPeerMasks_GV100
(
    OBJGPU   *pGpu,
    KernelCE  *pKCe
)
{
    KernelCE *pKCeLoop = NULL;

    KCE_ITER_BEGIN(pGpu, pKCe, pKCeLoop, NVLINK_MIN_P2P_LCE)
        pKCeLoop->nvlinkPeerMask = 0;
    KCE_ITER_END
}
