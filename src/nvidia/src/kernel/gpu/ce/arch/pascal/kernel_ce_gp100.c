/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/ce/kernel_ce_private.h"
#include "gpu/gpu.h"
#include "kernel/gpu/nvlink/kernel_nvlink.h"
#include "kernel/gpu/conf_compute/conf_compute.h"

NV_STATUS kceStateLoad_GP100(OBJGPU *pGpu, KernelCE *pKCe, NvU32 flags)
{
    KernelCE *pKCeShim;

    // Mark first CE to load as the owner
    if (kceFindShimOwner(pGpu, pKCe, &pKCeShim) != NV_OK)
        pKCe->bShimOwner = NV_TRUE;

    if (!IS_VIRTUAL(pGpu) && pKCe->bShimOwner)
    {
        pKCe->bMapComplete = NV_FALSE;

        //
        // Don't create a new object on suspend/resume path.
        //
        if (!(flags & GPU_STATE_FLAGS_PRESERVING))
        {
            pKCe->pPceLceMap = portMemAllocNonPaged(sizeof(NvU32[NV2080_CTRL_MAX_PCES]));
            if (pKCe->pPceLceMap == NULL)
            {
                return NV_ERR_NO_MEMORY;
            }
        }

        NvU32   numPcesPerLce;
        NvU32   numLces;
        NvU32   supportedPceMask;
        NvU32   supportedLceMask;
        NvU32   pcesPerHshub;

        NV_ASSERT_OK_OR_RETURN(kceGetPceConfigForLceType(pGpu,
                                                         pKCe,
                                                         NV2080_CTRL_CE_LCE_TYPE_DECOMP,
                                                         &numPcesPerLce,
                                                         &numLces,
                                                         &supportedPceMask,
                                                         &supportedLceMask,
                                                         &pcesPerHshub));

        pKCe->decompPceMask         = supportedPceMask;
        pKCe->shimConnectingHubMask = 0;

        KernelCE *pKCeIter = NULL;
        KCE_ITER_SHIM_BEGIN(pGpu, pKCeIter)
        {
            NV_ASSERT_OK_OR_RETURN(kceTopLevelPceLceMappingsUpdate(pGpu, pKCeIter));
        }
        KCE_ITER_END;
    }

    if (gpuIsCCFeatureEnabled(pGpu))
    {
        if (kceIsSecureCe_HAL(pGpu, pKCe))
        {
            ConfidentialCompute *pCC = GPU_GET_CONF_COMPUTE(pGpu);

            NvU32 mcEngineIdx = MC_ENGINE_IDX_CE(pKCe->publicID);

            NV_ASSERT_OR_RETURN(mcEngineIdx <= MC_ENGINE_IDX_CE_MAX, NV_ERR_NOT_SUPPORTED);

            NV_ASSERT_OK_OR_RETURN(confComputeDeriveSecrets(pCC, mcEngineIdx));
        }
    }
    return NV_OK;
}

NV_STATUS
kceStateUnload_GP100(OBJGPU* pGpu, KernelCE* pKCe, NvU32 flags)
{
    //
    // Don't tear down when undergoing suspend/resume
    //
    if (!(flags & GPU_STATE_FLAGS_PRESERVING))
    {
        if (!IS_VIRTUAL(pGpu) && pKCe->bShimOwner)
        {
            portMemFree(pKCe->pPceLceMap);
            pKCe->pPceLceMap = NULL;
        }
    }

    return NV_OK;
}


/*!
 * Determine if CE should be used for sysmem read
 * @param[in]     pGpu          OBJGPU pointer
 * @param[in]     pKCe           KernelCE pointer
 * @returns NV_TRUE if true
 */
NvBool
kceIsCeSysmemRead_GP100
(
    OBJGPU *pGpu,
    KernelCE *pKCe
)
{
    NvU32 sysmemReadCE;
    NvU32 sysmemWriteCE;
    NvU32 nvlinkP2PCeMask;
    NvU32 gpuMask = NVBIT(pGpu->gpuInstance);

    // Initialize to maximum CEs available
    sysmemReadCE = gpuGetNumCEs(pGpu);

    NV_ASSERT_OK(kceGetCeFromNvlinkConfig(pGpu, pKCe,
                                gpuMask,
                                &sysmemReadCE,
                                &sysmemWriteCE,
                                &nvlinkP2PCeMask));

    return (sysmemReadCE == pKCe->publicID);
}

/*!
 * Determine if CE should be used for sysmem write
 * @param[in]     pGpu          OBJGPU pointer
 * @param[in]     pKCe           KernelCE pointer
 * @returns NV_TRUE if true
 */
NvBool
kceIsCeSysmemWrite_GP100
(
    OBJGPU *pGpu,
    KernelCE  *pKCe
)
{
    NvU32 sysmemReadCE;
    NvU32 sysmemWriteCE;
    NvU32 nvlinkP2PCeMask;
    NvU32 gpuMask = NVBIT(pGpu->gpuInstance);

    // Initialize to maximum CEs available
    sysmemWriteCE = gpuGetNumCEs(pGpu);

    kceGetCeFromNvlinkConfig(pGpu, pKCe,
                                gpuMask,
                                &sysmemReadCE,
                                &sysmemWriteCE,
                                &nvlinkP2PCeMask);

    return (sysmemWriteCE == pKCe->publicID);
}

/*!
 * Determine if CE should be used for NVLink P2P
 * @param[in]     pGpu          OBJGPU pointer
 * @param[in]     pKCe           KernelCE pointer
 * @returns NV_TRUE if true
 */
NvBool
kceIsCeNvlinkP2P_GP100
(
    OBJGPU *pGpu,
    KernelCE  *pKCe
)
{
    NvU32 sysmemReadCE;
    NvU32 sysmemWriteCE;
    NvU32 nvlinkP2PCeMask = 0;
    NvU32 gpuMask = NVBIT(pGpu->gpuInstance);

    kceGetCeFromNvlinkConfig(pGpu, pKCe,
                                gpuMask,
                                &sysmemReadCE,
                                &sysmemWriteCE,
                                &nvlinkP2PCeMask);

    return (NVBIT(pKCe->publicID) & nvlinkP2PCeMask ? NV_TRUE : NV_FALSE);
}

/**
 * @brief Get the Max Nvlink Topology
 *
 *  Compares the current topology to the cached topology and
 *  returns the auto config table index to the max nvlink config seen by this GPU
 *
 * @param[in]  pGpu                 OBJGPU pointer
 * @param[in]  pCe                  OBJCE pointer
 * @param[in]  pCurrentTopo         NVLINK_TOPOLOGY_INFO pointer
 * @param[in]  pAutoConfigTable     NVLINK_CE_AUTO_CONFIG_TABLE pointer
 * @param[in]  autoConfigNumEntries NvU32 num entries within pAutoConfigTable
 * @param[out] pLargestTopoIdx      NvU32 pointer
 *
 * Returns NV_TRUE if entry exists for either current topo or cached topo
 */
NvBool
kceGetNvlinkMaxTopoForTable_GP100
(
    OBJGPU                   *pGpu,
    KernelCE                 *pKCe,
    NVLINK_TOPOLOGY_PARAMS   *pCurrentTopo,
    void                    *pAutoConfigTable,
    NvU32                    autoConfigNumEntries,
    NvU32                   *pLargestTopoIdx
)
{
    NvU32  cachedTopoIdx  = 0;
    NvU32  currentTopoIdx = 0;
    NvBool bCachedIdxExists, bCurrentIdxExists;
    NvU32  currentExposeCeMask, cachedExposeCeMask;
    NvBool result = NV_FALSE;
    NVLINK_TOPOLOGY_PARAMS *pCachedTopo = portMemAllocNonPaged(sizeof(*pCachedTopo));

    NV_ASSERT_OR_RETURN(pCachedTopo != NULL, result);

    //
    // If exposeCeMask from current config is a subset of the cached topology,
    // then use the cached topology data.
    // We do this to ensure that we don't revoke CEs that we have exposed prevously.
    //
    gpumgrGetSystemNvlinkTopo(gpuGetDBDF(pGpu), pCachedTopo);

    bCachedIdxExists = kceGetAutoConfigTableEntry_HAL(pGpu, pKCe, pCachedTopo,
                        pAutoConfigTable, autoConfigNumEntries, &cachedTopoIdx,
                        &cachedExposeCeMask);

    bCurrentIdxExists = kceGetAutoConfigTableEntry_HAL(pGpu, pKCe, pCurrentTopo,
                         pAutoConfigTable, autoConfigNumEntries, &currentTopoIdx,
                         &currentExposeCeMask);

    if (bCachedIdxExists && bCurrentIdxExists)
    {
        // Both topologies are in the table
        if (cachedExposeCeMask & ~currentExposeCeMask)
        {
            // Current topo's exposeCeMask is a subset of cached topo exposeCeMask
            *pLargestTopoIdx = cachedTopoIdx;
        }
        else
        {
            // Current topo's exposeCeMask is equal or superset of cached topo exposeCeMask
            *pLargestTopoIdx = currentTopoIdx;

            if (cachedExposeCeMask != currentExposeCeMask)
            {
                //
                // Current topo's exposeCeMask is superset of cached topo exposeCeMask
                //
                // This means the topology has increased. We must clear previous
                // optimal CE recommendations to ensure we can recommend the
                // correct optimal CE recommendations going forward.
                // See Bug 2051735 for details.
                //

                kceClearAssignedNvlinkPeerMasks_HAL(pGpu, pKCe);
            }
        }
    }
    else if (bCachedIdxExists)
    {
        // only cached topo is in table
        *pLargestTopoIdx = cachedTopoIdx;
    }
    else if (bCurrentIdxExists)
    {
        // only current topo is in table
        *pLargestTopoIdx = currentTopoIdx;
    }
    else
    {
        // Neither are in table
        result = NV_FALSE;
        goto done;
    }

    result = NV_TRUE;

done:
    portMemFree(pCachedTopo);
    return result;
}

/**
 * @brief Sets the CE capabilities based on PCE-LCE mapping algorithm
 *
 * @param[in]      pGpu       OBJGPU pointer
 * @param[in]      pKCe       KernelCE pointer
 * @param[in]      pKCeCaps   CE capabilities table
 */
void
kceAssignCeCaps_GP100
(
    OBJGPU      *pGpu,
    KernelCE    *pKCe,
    NvU8        *pKCeCaps
)
{
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

    if (pKernelNvlink != NULL)
        kceGetNvlinkCaps(pGpu, pKCe, pKCeCaps); 
}
