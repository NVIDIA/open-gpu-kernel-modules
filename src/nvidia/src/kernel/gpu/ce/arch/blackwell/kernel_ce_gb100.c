/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/ce/kernel_ce_private.h"
#include "gpu/bif/kernel_bif.h"
#include "published/blackwell/gb100/dev_ce_base.h"
#include "gpu/nvlink/kernel_nvlink.h"

// Defines for PCE-LCE mapping algorithm
#define NV_CE_LCE_MASK_INIT                   0xFFFFFFFF
#define NV_CE_EVEN_ASYNC_LCE_MASK             0x55555550
#define NV_CE_ODD_ASYNC_LCE_MASK              0xAAAAAAA0
#define NV_CE_EVEN_MIG_LCE_MASK               0x55555554
#define NV_CE_ODD_MIG_LCE_MASK                0xAAAAAAA8
#define NV_CE_EVEN_PCE_MASK                   0x55555555
#define NV_CE_ODD_PCE_MASK                    0xAAAAAAAA
#define NV_CE_ALL_PCE_MASK                    0xFFFFFF
#define NV_CE_INVALID_TOPO_IDX                0xFFFF
#define NV_CE_SYSMEM_LCE_START_INDEX          0x2
#define NV_CE_SYSMEM_LCE_END_INDEX            0x4
#define NV_CE_LCE_4                           0x4
#define NV_MAX_C2C_LCES                       0x2

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
kceGetPce2lceConfigSize1_GB100
(
    KernelCE *pKCe
)
{
    return NV_CE_PCE2LCE_CONFIG__SIZE_1;
}

/*!
 * @brief Returns the size of the GRCE_CONFIG register array
 *
 * @param[in] pGpu  OBJGPU pointer
 * @param[in] pKCe   KernelCE pointer
 *
 * @return  NV_CE_GRCE_CONFIG__SIZE_1
 */
NvU32
kceGetGrceConfigSize1_GB100
(
    KernelCE *pKCe
)
{
    return (pKCe->shimInstance == 0) ? NV_CE_GRCE_CONFIG__SIZE_1 : 0;
}

/*!
 * @brief Set the shim instance based on the public ID.
 */
void
kceSetShimInstance_GB100
(
    OBJGPU *pGpu,
    KernelCE *pKCe
)
{
    pKCe->shimInstance = pKCe->publicID / NV_KCE_GROUP_ID_STRIDE;
}

/**
 *  Return the pce-lce mappings and grce config
 *  reg values when nvlink topology is NOT forced
 *
 * @param[in]  pGpu                 OBJGPU pointer
 * @param[in]  pKCe                 KernelCE pointer
 * @param[out] pPceLceMap           Stores the pce-lce mappings
 * @param[out] pGrceConfig          Stores the grce configuration
 * @param[out] pExposedLceMask    Mask of CEs to expose to clients
 *
 * @return  NV_OK on success
 */
NV_STATUS
kceGetNvlinkAutoConfigCeValues_GB100
(
    OBJGPU   *pGpu,
    KernelCE *pKCe,
    NvU32    *pPceLceMap,
    NvU32    *pGrceConfig,
    NvU32    *pExposedLceMask
)
{
    KernelNvlink *pKernelNvlink      = GPU_GET_KERNEL_NVLINK(pGpu);
    NV_STATUS     status             = NV_OK;
    NvU32         pce2lceConfigSize1 = kceGetPce2lceConfigSize1_HAL(pKCe);
    NvU32         grceConfigSize1    = kceGetGrceConfigSize1_HAL(pKCe);
    NvU32        *pLocalPceLceMap    = NULL;
    NvU32        *pLocalGrceConfig   = NULL;
    NvU32         pceIdx;
    NvU32         grceIdx;
    NvU32         i;

    NV_ASSERT_OR_RETURN(gpuGetNumCEs(pGpu) != 0, NV_ERR_NOT_SUPPORTED);

    NV_ASSERT_OR_RETURN(pPceLceMap      != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pGrceConfig     != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pExposedLceMask != NULL, NV_ERR_INVALID_ARGUMENT);

    NV_ASSERT_OR_RETURN(!(pKernelNvlink == NULL && pGpu->getProperty(pGpu, PDB_PROP_GPU_SKIP_CE_MAPPINGS_NO_NVLINK)),
                        NV_ERR_NOT_SUPPORTED);

    NVLINK_TOPOLOGY_PARAMS *pCurrentTopo = portMemAllocNonPaged(sizeof(*pCurrentTopo));
    NV_ASSERT_OR_RETURN(pCurrentTopo != NULL, NV_ERR_NO_MEMORY);

    portMemSet(pCurrentTopo, 0, sizeof(*pCurrentTopo));

    pLocalPceLceMap          = portMemAllocNonPaged(sizeof(NvU32[NV2080_CTRL_MAX_PCES]));
    pLocalGrceConfig         = portMemAllocNonPaged(sizeof(NvU32[NV2080_CTRL_MAX_GRCES]));
    NvU32 localExposedCeMask = 0;

    NV_ASSERT_TRUE_OR_GOTO(status, pLocalPceLceMap  != NULL, NV_ERR_NO_MEMORY, done);
    NV_ASSERT_TRUE_OR_GOTO(status, pLocalGrceConfig != NULL, NV_ERR_NO_MEMORY, done);

    for (i = 0; i < NV2080_CTRL_MAX_PCES; i++)
    {
        pLocalPceLceMap[i] = NV2080_CTRL_CE_UPDATE_PCE_LCE_MAPPINGS_INVALID_LCE;
    }

    for (i = 0; i < NV2080_CTRL_MAX_GRCES; i++)
    {
        pLocalGrceConfig[i] = NV2080_CTRL_CE_UPDATE_PCE_LCE_MAPPINGS_INVALID_LCE;
    }

    status = kceGetMappings_HAL(pGpu,
                                pKCe,
                                pCurrentTopo,
                                pLocalPceLceMap,
                                pLocalGrceConfig,
                                &localExposedCeMask);

    *pExposedLceMask = localExposedCeMask;

    for (pceIdx = 0; pceIdx < pce2lceConfigSize1; pceIdx++)
    {
        pPceLceMap[pceIdx] = pLocalPceLceMap[pceIdx];
    }

    for (grceIdx = 0; grceIdx < grceConfigSize1; grceIdx++)
    {
        pGrceConfig[grceIdx] = pLocalGrceConfig[grceIdx];
    }

done:
    portMemFree(pCurrentTopo);
    portMemFree(pLocalPceLceMap);
    portMemFree(pLocalGrceConfig);

    return status;
}

void
kceSetDecompCeCap_GB100
(
    OBJGPU      *pGpu,
    KernelCE    *pKCe
)
{
    if(pKCe != NULL)
    {
        pKCe->ceCapsMask |= NVBIT32(CE_CAPS_DECOMPRESS);
    }
}

/**
 * @brief This function returns the MIG GPU Instance mappings a specific
 *        partition should be using based on the input of LCEs available
 *
 * @param[in]   pGpu                        OBJGPU pointer
 * @param[in]   pKCe                        KernelCE pointer
 * @param[in]   lceAvailableMask            Bit mask of LCEs available to this GPU Instance
 * @param[out]  pLocalPceLceMap             Pointer to PCE-LCE array
 *
 * @return NV_TRUE after mapping is populated. There is no error return as an invalid
 *         mapping should not result in fatal error.
 */
NV_STATUS
kceGetMappingsForMIGGpuInstance_GB100
(
    OBJGPU *pGpu,
    KernelCE *pKCe,
    NvU32 lceAvailableMask,
    NvU32 *pLocalPceLceMap
)
{
    KernelCE *pKCeIter = NULL;
    NvU32 totalNumLcesAvailable = nvPopCount32(lceAvailableMask);
    NvU32 shimLceMask = kceGetLceMaskForShimInstance_HAL(pGpu, pKCe);
    NvU32 pcesForEvenLces = 0;
    NvU32 pcesForOddLces = 0;
    NvU32 totalPcesAvailable;
    NvU32 numLcesToMap;
    NvU32 lceEvenMaskToUse;
    NvU32 lceOddMaskToUse;
    NvU32 numMinPcesPerLce;
    NvU32 numLcesMapped;
    NvU32 lceIdx;
    NvU32 pceIdx;
    NvU32 i;
    NVLINK_TOPOLOGY_PARAMS *pTopoParams;

    //
    // Initialize the pceAvailableMask to include all PCEs.
    // Iterate over all HSHUBs in both shims to generate a mask of PCEs available.
    // Considering FS, take the minimal mask of the two shims and use that PCE mask.
    //

    pTopoParams = portMemAllocNonPaged(sizeof(*pTopoParams));
    NV_ASSERT_OR_RETURN(pTopoParams != NULL, NV_ERR_NO_MEMORY);
    portMemSet(pTopoParams, 0, sizeof(*pTopoParams));

    totalPcesAvailable = NV_CE_ALL_PCE_MASK;

    KCE_ITER_SHIM_BEGIN(pGpu, pKCeIter)
    {
        NvU32 localPcesAvailable = 0;
        kceGetAvailableHubPceMask(pGpu, pKCeIter, pTopoParams);

        for (i = 0; i < NV2080_CTRL_CE_MAX_HSHUBS; i++)
        {
            localPcesAvailable |= pTopoParams->pceAvailableMaskPerConnectingHub[i];
        }
        totalPcesAvailable &= localPcesAvailable;
    }
    KCE_ITER_END;

    // For even LCEs generate pce mask
    kceGetPceConfigForLceMIGGpuInstance(pGpu,
                                        pKCe,
                                        lceAvailableMask & NV_CE_EVEN_MIG_LCE_MASK & shimLceMask,
                                        totalNumLcesAvailable,
                                        &pcesForEvenLces,
                                        &numLcesToMap,
                                        &lceEvenMaskToUse,
                                        &numMinPcesPerLce);

    // For odd LCEs generate pce mask
    kceGetPceConfigForLceMIGGpuInstance(pGpu,
                                        pKCe,
                                        lceAvailableMask & NV_CE_ODD_MIG_LCE_MASK & shimLceMask,
                                        totalNumLcesAvailable,
                                        &pcesForOddLces,
                                        &numLcesToMap,
                                        &lceOddMaskToUse,
                                        &numMinPcesPerLce);

    // For each even LCE, assign required number of PCEs from the even PCE mask on this shim
    pcesForEvenLces &= totalPcesAvailable;
    numLcesMapped = 0;
    FOR_EACH_INDEX_IN_MASK(32, lceIdx, lceEvenMaskToUse)
    {
        if (numLcesMapped >= (numLcesToMap / 2))
        {
            break;
        }

        // Assign even PCEs first in an attempt to use PCEs from the same UTLB
        NvU32 pcesLocalAvailable  = pcesForEvenLces & NV_CE_EVEN_PCE_MASK;
        for (i = 0; i < numMinPcesPerLce; i++)
        {
            if (pcesLocalAvailable == 0)
            {
                // assign odd LCEs if evens are no longer available
                pcesLocalAvailable  = pcesForEvenLces & NV_CE_ODD_PCE_MASK;

                if ((pcesForEvenLces & NV_CE_ODD_PCE_MASK) == 0)
                {
                    // no more PCEs left to assign
                    break;
                }
            }

            pceIdx = CE_GET_LOWEST_AVAILABLE_IDX(pcesLocalAvailable);
            if (pceIdx <= kceGetPce2lceConfigSize1_HAL(pKCe))
            {
                pcesLocalAvailable = pcesLocalAvailable & ~NVBIT32(pceIdx);
                pcesForEvenLces    = pcesForEvenLces & ~NVBIT32(pceIdx);
                pLocalPceLceMap[pceIdx] = lceIdx % NV_KCE_GROUP_ID_STRIDE;
            }
        }
        numLcesMapped++;
    }
    FOR_EACH_INDEX_IN_MASK_END;

    // For each odd LCE, assign required number of PCEs from the odd PCE mask on this shim
    pcesForOddLces &= totalPcesAvailable;
    numLcesMapped = 0;
    FOR_EACH_INDEX_IN_MASK(32, lceIdx, lceOddMaskToUse)
    {
        if (numLcesMapped >= (numLcesToMap / 2))
        {
            break;
        }

        NvU32 numPcesMapped = 0;
        // Assign even PCEs first in an attempt to use PCEs from the same UTLB
        NvU32 pcesLocalAvailable  = pcesForOddLces & NV_CE_EVEN_PCE_MASK;
        for (i = 0; i < numMinPcesPerLce; i++)
        {
            if (pcesLocalAvailable == 0)
            {
                // assign odd LCEs if evens are no longer available
                pcesLocalAvailable  = pcesForOddLces & NV_CE_ODD_PCE_MASK;

                if ((pcesForOddLces & NV_CE_ODD_PCE_MASK) == 0)
                {
                    // no more PCEs left to assign
                    break;
                }
            }

            pceIdx = CE_GET_LOWEST_AVAILABLE_IDX(pcesLocalAvailable);
            if (pceIdx <= kceGetPce2lceConfigSize1_HAL(pKCe))
            {
                pcesLocalAvailable = pcesLocalAvailable & ~NVBIT32(pceIdx);
                pcesForOddLces     = pcesForOddLces & ~NVBIT32(pceIdx);
                pLocalPceLceMap[pceIdx] = lceIdx % NV_KCE_GROUP_ID_STRIDE; //shim consideration
                numPcesMapped++;

                // Due to FS limitations regardless of min PCE request, only assign half the PCEs
                if (numPcesMapped >= (numMinPcesPerLce / 2))
                {
                    break;
                }
            }
        }
        numLcesMapped++;

        //
        // Mark decomp capabilities if applicable.
        // Note this is currently applicable to ODD LCEs to account for FB Thread IDs
        //
        KernelCE *pKCeLce = GPU_GET_KCE(pGpu, lceIdx);
        kceSetDecompCeCap_HAL(pGpu, pKCeLce);
    }
    FOR_EACH_INDEX_IN_MASK_END;

    portMemFree(pTopoParams);

    return NV_OK;
}

/**
 * @brief This function takes care of mapping work submit LCEs mappings for CC case.
 *
 * @param[in]   pGpu                                OBJGPU pointer
 * @param[in]   pKCe                                KernelCE pointer
 * @param[in]   pAvailablePceMaskForConnectingHub   Pointer to CEs available per HSHUB
 * @param[out]  pExposedLceMask                     Pointer to LCE Mask
 */
NV_STATUS
kceMapPceLceForWorkSubmitLces_GB100
(
    OBJGPU      *pGpu,
    KernelCE    *pKCe,
    NvU32       *pAvailablePceMaskForConnectingHub,
    NvU32       *pExposedLceMask
)
{
    if (!gpuIsCCFeatureEnabled(pGpu))
    {
        return NV_OK;
    }

    KernelCE *pKCeIter = NULL;

    NvU32 numPcesPerLce;
    NvU32 numLces;
    NvU32 supportedPceMask;
    NvU32 supportedLceMask;
    NvU32 pcesPerHshub;
    NvU32 pceIndex;
    NvU32 lceMask;
    NvU32 hshubIndex;
    NvU32 maxLceCount;
    NvU32 lceIndex;
    NvU32 lastShimInstance = 0;

    kceGetPceConfigForLceType(pGpu,
                              pKCe,
                              NV2080_CTRL_CE_LCE_TYPE_CC_WORK_SUBMIT,
                              &numPcesPerLce,
                              &numLces,
                              &supportedPceMask,
                              &supportedLceMask,
                              &pcesPerHshub);

    maxLceCount = supportedLceMask;
    HIGHESTBITIDX_32(maxLceCount); // HIGHESTBITIDX_32 is destructive.
    maxLceCount++;

    //
    // Choose the last shim instance that has PCEs
    //
    KCE_ITER_SHIM_BEGIN(pGpu, pKCeIter)
    {
        //
        // Skip any shims that don't have any PCEs connected to the hubs
        //
        if (pKCeIter->shimConnectingHubMask == 0)
        {
            continue;
        }

        hshubIndex = CE_GET_LOWEST_AVAILABLE_IDX(pKCeIter->shimConnectingHubMask);
        pceIndex   = CE_GET_LOWEST_AVAILABLE_IDX((pAvailablePceMaskForConnectingHub[hshubIndex] &
                                                  supportedPceMask));

        if (pceIndex < kceGetPce2lceConfigSize1_HAL(pKCeIter))
        {
            lastShimInstance = NV_MAX(pKCeIter->shimInstance, lastShimInstance);
        }
    }
    KCE_ITER_END;

    KCE_ITER_SHIM_BEGIN(pGpu, pKCeIter)
    {
        //
        // Assign this LCE only on the last shim instance only on multiple shim chips.
        //
        if (pKCeIter->shimInstance == 0 || pKCeIter->shimInstance != lastShimInstance)
        {
            continue;
        }

        for (NvU32 index = 0; index < 2; ++index)
        {
            lceMask = kceGetLceMaskForShimInstance_HAL(pGpu, pKCeIter) &
                      supportedLceMask &
                     ~(*pExposedLceMask);

            // Prefer odd numbered LCEs as even numbered ones are used for other apertures.
            if ((lceMask & NV_CE_ODD_ASYNC_LCE_MASK) != 0)
            {
                lceMask &= NV_CE_ODD_ASYNC_LCE_MASK;
            }

            lceIndex = CE_GET_LOWEST_AVAILABLE_IDX(lceMask);

            if (lceIndex >= maxLceCount)
            {
                break;
            }

            FOR_EACH_INDEX_IN_MASK(32, hshubIndex, pKCeIter->shimConnectingHubMask)
            {
                if (pAvailablePceMaskForConnectingHub[hshubIndex] == 0)
                {
                    continue;
                }

                pceIndex = CE_GET_LOWEST_AVAILABLE_IDX((pAvailablePceMaskForConnectingHub[hshubIndex] & supportedPceMask));

                if (pceIndex < kceGetPce2lceConfigSize1_HAL(pKCeIter))
                {
                    pAvailablePceMaskForConnectingHub[hshubIndex] &= ~(NVBIT32(pceIndex));
                    //
                    // Convert absolute LCE index to shim local index
                    //
                    pKCeIter->pPceLceMap[pceIndex] = lceIndex % NV_KCE_GROUP_ID_STRIDE;
                    *pExposedLceMask              |= NVBIT(lceIndex);
                    lceMask                       &= ~(NVBIT32(lceIndex));

                    KernelCE *pKCeLce = GPU_GET_KCE(pGpu, lceIndex);

                    NV_ASSERT(pKCeLce != NULL);
                    pKCeLce->ceCapsMask |= NVBIT32(CE_CAPS_CC_WORK_SUBMIT);

                    NV_PRINTF(LEVEL_INFO,
                              "Work submit CE Mapping -- PCE Index: %d -> LCE Index: %d\n",
                              pceIndex,
                              lceIndex);

                    break;
                }
            }
            FOR_EACH_INDEX_IN_MASK_END;
        }
    }
    KCE_ITER_END;

    return NV_OK;
}

/**
 * @brief This function takes care of mappings for Decomp PCEs.
 *        Mapping will assign first 6 Decomp PCEs found on each
 *        shim to the lowest available odd numbered LCE if possible.
 *
 * @param[in]   pGpu                                OBJGPU pointer
 * @param[in]   pKCe                                KernelCE pointer
 * @param[in]   pAvailablePceMaskForConnectingHub   Pointer to PCEs available per HSHUB
 * @param[out]  pExposedLceMask                     Pointer to LCE Mask
 */
void
kceMapPceLceForDecomp_GB100
(
    OBJGPU      *pGpu,
    KernelCE    *pKCe,
    NvU32       *pAvailablePceMaskForConnectingHub,
    NvU32       *pExposedLceMask
)
{
    NvU32       maxLceCount;
    NvU32       numDecompPcesAssigned   = 0;
    KernelCE   *pKCeIter                = NULL;
    NvU32       localDecompPceMask;
    NvU32       hshubIndex;
    NvU32       lceMask;
    NvU32       lceIndex;
    NvU32       pceIndex;
    NvU32       numPcesPerLce;
    NvU32       numLces;
    NvU32       supportedPceMask;
    NvU32       supportedLceMask;
    NvU32       pcesPerHshub;

    kceGetPceConfigForLceType(
        pGpu,
        pKCe,
        NV2080_CTRL_CE_LCE_TYPE_DECOMP,
        &numPcesPerLce,
        &numLces,
        &supportedPceMask,
        &supportedLceMask,
        &pcesPerHshub);

    maxLceCount = supportedLceMask;
    HIGHESTBITIDX_32(maxLceCount); // HIGHESTBITIDX_32 is destructive.
    maxLceCount++;

    KCE_ITER_SHIM_BEGIN(pGpu, pKCeIter)
    {
        lceMask = kceGetLceMaskForShimInstance_HAL(pGpu, pKCeIter) &
                  supportedLceMask &
                 ~(*pExposedLceMask);

        if ((lceMask & NV_CE_ODD_ASYNC_LCE_MASK) != 0)
        {
            lceMask &= NV_CE_ODD_ASYNC_LCE_MASK;
        }

        numDecompPcesAssigned   = 0;
        lceIndex                = CE_GET_LOWEST_AVAILABLE_IDX(lceMask);

        if (lceIndex >= maxLceCount)
        {
            break;
        }

        FOR_EACH_INDEX_IN_MASK(32, hshubIndex, pKCeIter->shimConnectingHubMask)
        {
            if (numDecompPcesAssigned >= numPcesPerLce)
            {
                break;
            }

            localDecompPceMask = pAvailablePceMaskForConnectingHub[hshubIndex] & supportedPceMask;

            if (localDecompPceMask == 0)
            {
                continue;
            }

            FOR_EACH_INDEX_IN_MASK(32, pceIndex, localDecompPceMask)
            {
                if (pceIndex < kceGetPce2lceConfigSize1_HAL(pKCeIter))
                {
                    //
                    // Convert absolute LCE index to shim local index
                    //
                    pKCeIter->pPceLceMap[pceIndex]                  = lceIndex % NV_KCE_GROUP_ID_STRIDE;
                    *pExposedLceMask                               |= NVBIT(lceIndex);
                    pAvailablePceMaskForConnectingHub[hshubIndex]  &= (~NVBIT(pceIndex));
                    numDecompPcesAssigned++;
                    KernelCE *pKCeLce                               = GPU_GET_KCE(pGpu, lceIndex);
                    if(pKCeLce != NULL)
                    {
                       pKCeLce->ceCapsMask                         |= NVBIT32(CE_CAPS_DECOMPRESS);
                    }
                    NV_PRINTF(
                        LEVEL_INFO,
                        "Decomp CE Mapping -- PCE Index: %d -> LCE Index: %d\n",
                        pceIndex, lceIndex);
                }
            }
            FOR_EACH_INDEX_IN_MASK_END;
        }
        FOR_EACH_INDEX_IN_MASK_END;
    }
    KCE_ITER_END;
}

/**
 * @brief This function returns the pceIndex for a particular link ID
 *        Must always be called with the hshub ID for the calling link ID.
 *        Assign PCE from selected HSHUB, if not possible move to the
 *        additional HSHUB provided.
 *
 * @param[in]       pGpu                                OBJGPU pointer
 * @param[in]       pKCe                                KernelCE pointer
 * @param[in]       pAvailablePceMaskForConnectingHub   Pointer to CEs available per HSHUB
 * @param[in,out]   pceIndex                            Pointer to caller pceIndex
 * @param[out]      pHshubId                            Pointer to caller HSHUB ID
 *
 * @return NV_TRUE if there is a free PCE available for assignment. Else returns NV_FALSE.
 */
static NvBool
_ceGetAlgorithmPceIndex
(
    OBJGPU     *pGpu,
    KernelCE   *pKCe,
    NvU32      *pAvailablePceMaskForConnectingHub,
    NvU32      *pceIndex,
    NvU32      *pHshubId
)
{
    //
    // The requested PCE is available. So, just use it.
    //
    if (NVBIT32(*pceIndex) & pAvailablePceMaskForConnectingHub[*pHshubId] & ~(pKCe->decompPceMask))
    {
        return NV_TRUE;
    }

    //
    // Try to use the unassigned PCE from last HSHUB.
    //
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV2080_CTRL_INTERNAL_HSHUB_GET_MAX_HSHUBS_PER_SHIM_PARAMS maxHshubParams;
    portMemSet(&maxHshubParams, 0, sizeof(maxHshubParams));

    NV_ASSERT_OK(pRmApi->Control(pRmApi,
                                 pGpu->hInternalClient,
                                 pGpu->hInternalSubdevice,
                                 NV2080_CTRL_CMD_INTERNAL_HSHUB_GET_MAX_HSHUBS_PER_SHIM,
                                 &maxHshubParams,
                                 sizeof(maxHshubParams)));

    *pHshubId = maxHshubParams.maxHshubs - 1;

    //
    // Do not include decomp PCEs (as they are not AES capable) in NVLINK LCE in CC mode as
    // we want to make NVLINK LCE a secure LCE
    //
    if (!gpuIsCCFeatureEnabled(pGpu))
    {
        *pceIndex = CE_GET_LOWEST_AVAILABLE_IDX(pAvailablePceMaskForConnectingHub[*pHshubId]);
    }
    else
    {
        *pceIndex = CE_GET_LOWEST_AVAILABLE_IDX((pAvailablePceMaskForConnectingHub[*pHshubId] & ~(pKCe->decompPceMask)));
    }

    if (*pceIndex <= kceGetPce2lceConfigSize1_HAL(pKCe))
    {
        return NV_TRUE;
    }

    // If we've reached this point, then we have no more available PCEs to assign
    NV_PRINTF(LEVEL_INFO, "No more available PCEs to assign!\n");
    return NV_FALSE;
}

/**
 * @brief This function assigns PCE-LCE mappings for NVLink peers
 *        Based on HSHUBs that the links associated with a peer connect to,
 *        algorithm will attempt to assign a PCE from associated HSHUB before
 *        moving to additional HSHUB.
 *
 * @param[in]   pGpu                                OBJGPU pointer
 * @param[in]   pKCe                                KernelCE pointer
 * @param[in]   pAvailablePceMaskForConnectingHub   Pointer to CEs available per HSHUB
 * @param[out]  pLocalPceLceMap                     Pointer to PCE-LCE array
 * @param[out]  pExposedLceMask                     Pointer to LCE Mask
 *
 * Returns NV_OK if successful in assigning PCEs and LCEs for each of the NVLink peers
 */
NV_STATUS
kceMapPceLceForNvlinkPeers_GB100
(
    OBJGPU      *pGpu,
    KernelCE    *pKCe,
    NvU32       *pAvailablePceMaskForConnectingHub,
    NvU32       *pLocalPceLceMap,
    NvU32       *pExposedLceMask
)
{
    KernelNvlink   *pKernelNvlink         = GPU_GET_KERNEL_NVLINK(pGpu);
    NV_STATUS       status                = NV_OK;
    NvU32           pceMask               = 0;
    NvU32           peerLinkMask          = 0;
    NvU32           gpuInstance           = 0;
    NvU32           pceIndex              = kceGetPce2lceConfigSize1_HAL(pKCe);
    NvBool          bPeerAssigned         = NV_FALSE;
    NvU32           peerAvailableLceMask  = NV_CE_LCE_MASK_INIT;
    OBJGPU         *pRemoteGpu;
    NvU32           lceIndex;
    NvU32           linkId;
    NvU32           gpuMask;    
    NvU32           maxLceCount;
    NvU32           numPcesPerLce;
    NvU32           numLces;
    NvU32           supportedPceMask;
    NvU32           supportedLceMask;
    NvU32           pcesPerHshub;

    kceGetPceConfigForLceType(pGpu,
                              pKCe,
                              NV2080_CTRL_CE_LCE_TYPE_NVLINK_PEER,
                              &numPcesPerLce,
                              &numLces,
                              &supportedPceMask,
                              &supportedLceMask,
                              &pcesPerHshub);

    maxLceCount = supportedLceMask;
    HIGHESTBITIDX_32(maxLceCount); // HIGHESTBITIDX_32 is destructive.
    maxLceCount++;

    peerAvailableLceMask = kceGetNvlinkPeerSupportedLceMask_HAL(pGpu, pKCe, peerAvailableLceMask) &
                                kceGetLceMask(pGpu) &
                                ~(*pExposedLceMask);

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
        NvU32 numLinksToPeer = knvlinkGetNumLinksToPeer(pGpu, pKernelNvlink, pRemoteGpu);

        if (numLinksToPeer == 0)
        {
            continue;
        }

        pceMask = 0;

        if (peerAvailableLceMask == 0)
        {
            //
            // peerAvailableLceMask is initialized to even async LCEs at the
            // top of the function.
            // As a result, if at any point in the loop, this mask == 0,
            // it implies we have used up all even async LCEs and should move to
            // using odd async LCEs.
            //
            peerAvailableLceMask = kceGetNvlinkPeerSupportedLceMask_HAL(pGpu, pKCe, peerAvailableLceMask) &
                                        kceGetLceMask(pGpu) &
                                        ~(*pExposedLceMask);
        }

        lceIndex = CE_GET_LOWEST_AVAILABLE_IDX(peerAvailableLceMask);

        if (lceIndex >= maxLceCount)
        {
            NV_PRINTF(LEVEL_ERROR,
                        "Invalid LCE Index Request. lceIndex = %d, maxLceCount = %d\n",
                        lceIndex,
                        maxLceCount);
            return NV_OK;
        }

        pKCe->nvlinkNumPeers++;

        peerLinkMask = knvlinkGetLinkMaskToPeer(pGpu, pKernelNvlink, pRemoteGpu);
        if (peerLinkMask == 0)
        {
            NV_PRINTF(LEVEL_INFO, "GPU%d has nvlink disabled. Skip LCE-PCE mapping.\n", pRemoteGpu->gpuInstance);
            continue;
        }

        if (!kceSupportsEquidistantPces_HAL(pGpu, pKCe))
        {
            NvU32  hshubIndex;
            NvBool bIsPceAvailable;
            NV2080_CTRL_INTERNAL_HSHUB_GET_HSHUB_ID_FOR_LINKS_PARAMS params;
            portMemSet(&params, 0, sizeof(params));
            params.linkMask = peerLinkMask;

            status = knvlinkExecGspRmRpc(pGpu,
                                         pKernelNvlink,
                                         NV2080_CTRL_CMD_INTERNAL_HSHUB_GET_HSHUB_ID_FOR_LINKS,
                                         &params,
                                         sizeof(params));
            NV_ASSERT_OK_OR_RETURN(status);

            FOR_EACH_INDEX_IN_MASK(32, linkId, peerLinkMask)
            {
                hshubIndex  = params.hshubIds[linkId];
                pceIndex    = CE_GET_LOWEST_AVAILABLE_IDX(pAvailablePceMaskForConnectingHub[hshubIndex]);

                bIsPceAvailable = _ceGetAlgorithmPceIndex(pGpu, pKCe, pAvailablePceMaskForConnectingHub, &pceIndex, &hshubIndex);
                if (bIsPceAvailable && (pceIndex < kceGetPce2lceConfigSize1_HAL(pKCe)))
                {
                    pceMask                                         |= NVBIT32(pceIndex);
                    pAvailablePceMaskForConnectingHub[hshubIndex]   &= ~(NVBIT32(pceIndex));
                }
            }
            FOR_EACH_INDEX_IN_MASK_END;
        }
        else
        {
            NvU32 numPcesAssigned    = 0;
            NvU32 numLinks           = nvPopCount32(peerLinkMask);
            NvU32 connectingHubIndex = 0;
            NvBool bFoundPces;

            KernelCE *pKCeShimOwner;

            status = kceFindShimOwner(pGpu, pKCe, &pKCeShimOwner);

            while (numPcesAssigned < numLinks)
            {
                bFoundPces = NV_FALSE;

                FOR_EACH_INDEX_IN_MASK(32, connectingHubIndex, pKCeShimOwner->shimConnectingHubMask)
                {
                    if (pAvailablePceMaskForConnectingHub[connectingHubIndex] == 0)
                    {
                        continue;
                    }

                    if (numPcesAssigned >= numLinks)
                    {
                        break;
                    }

                    FOR_EACH_INDEX_IN_MASK(32, pceIndex, pAvailablePceMaskForConnectingHub[connectingHubIndex] & supportedPceMask)
                    {
                        if (numPcesAssigned >= numLinks)
                        {
                            break;
                        }

                        bFoundPces                                             = NV_TRUE;
                        pceMask                                               |= NVBIT32(pceIndex);
                        pAvailablePceMaskForConnectingHub[connectingHubIndex] &= ~(NVBIT32(pceIndex));
                        ++numPcesAssigned;
                    }
                    FOR_EACH_INDEX_IN_MASK_END;
                }
                FOR_EACH_INDEX_IN_MASK_END;

                if (!bFoundPces)
                {
                    break;
                }
            }
        }

        if (pceMask != 0)
        {
            peerAvailableLceMask &= ~(NVBIT32(lceIndex));
            *pExposedLceMask     |= NVBIT32(lceIndex);
            bPeerAssigned         = NV_TRUE;

            NvU32 numPceSplit = nvPopCount32(pceMask) / 2;
            NvU32 numPceAssigned = 0;
            NvU32 secondLceIndex = CE_GET_LOWEST_AVAILABLE_IDX(peerAvailableLceMask);

            if (pKCe->bMultipleP2PLce)
            {
                if (secondLceIndex >= maxLceCount)
                {
                    NV_PRINTF(LEVEL_ERROR,
                              "Invalid Second LCE Index Request. lceIndex = %d, maxLceCount = %d\n",
                              secondLceIndex, maxLceCount);
                    return NV_OK;
                }
                else
                {
                    peerAvailableLceMask &= ~(NVBIT32(secondLceIndex));
                    *pExposedLceMask     |= NVBIT32(secondLceIndex);
                }
            }

            FOR_EACH_INDEX_IN_MASK(32, pceIndex, pceMask)
            {
                NvU32 currentLceIndex = (pKCe->bMultipleP2PLce && (numPceAssigned > numPceSplit)) ? secondLceIndex : lceIndex;

                //
                // Convert absolute LCE index to shim local index
                //
                pKCe->pPceLceMap[pceIndex] = currentLceIndex % NV_KCE_GROUP_ID_STRIDE;
                KernelCE *pKCeLce          = GPU_GET_KCE(pGpu, currentLceIndex);
                numPceAssigned++;
                if(pKCeLce != NULL)
                {
                   pKCeLce->ceCapsMask     |= NVBIT32(CE_CAPS_NVLINK_P2P);
                   pKCeLce->nvlinkPeerMask |= NVBIT32(pRemoteGpu->gpuInstance);
                }
                NV_PRINTF(LEVEL_INFO, "Nvlink peer CE Mapping for GPU%d <-> GPU%d -- PCE Index: %d -> LCE Index: %d\n",
                          pGpu->gpuInstance,
                          pRemoteGpu->gpuInstance,
                          pceIndex,
                          currentLceIndex);
            }
            FOR_EACH_INDEX_IN_MASK_END;
        }
    }

    if (!bPeerAssigned)
    {
        status = NV_WARN_NOTHING_TO_DO;
    }

    return status;
}

/**
 * @brief This function takes care of mappings for GRCE Case
 *        If NvLinks are mapped, share GRCE with the lowest
 *        LCE used. If not, mark GRCE as non sharing and assign.
 *
 * @param[in]   pGpu                               OBJGPU pointer
 * @param[in]   pKCe                               KernelCE pointer
 * @param[in]   pAvailablePceMaskForConnectingHub  Pointer to CEs available per HSHUB
 * @param[out]  pLocalPceLceMap                    Pointer to PCE-LCE array
 * @param[out]  pExposedLceMask                    Pointer to LCE Mask
 */
void
kceMapPceLceForGRCE_GB100
(
    OBJGPU   *pGpu,
    KernelCE *pKCe,
    NvU32    *pAvailablePceMaskForConnectingHub,
    NvU32    *pLocalPceLceMap,
    NvU32    *pExposedLceMask,
    NvU32    *pLocalGrceMap,
    NvU32     fbPceMask
)
{
    NvU32         lceIndex         = 0;
    NvU32         grceIdx          = 0;
    NvU32         hshubIndex;
    NvU32         pceIndex;
    KernelCE     *pKCeIter;

    KCE_ITER_SHIM_BEGIN(pGpu, pKCeIter)
    {
        //
        // Assign GRCE PCEs only on the first shim instance.
        //
        if (pKCeIter->shimInstance != 0)
        {
            continue;
        }

        hshubIndex = CE_GET_LOWEST_AVAILABLE_IDX(pKCeIter->shimConnectingHubMask);

        // Some PCEs have not been assigned. Pick one and use that for GRCE.
        if ((pAvailablePceMaskForConnectingHub[hshubIndex] & ~(pKCeIter->decompPceMask)) != 0)
        {
            lceIndex            = 0;
            *pExposedLceMask   |= NVBIT32(lceIndex);
            pceIndex            = CE_GET_LOWEST_AVAILABLE_IDX((pAvailablePceMaskForConnectingHub[hshubIndex] &
                                                              ~(pKCeIter->decompPceMask)));
            NV_ASSERT_OR_RETURN_VOID(pceIndex < NV2080_CTRL_MAX_PCES);

            if (pceIndex < kceGetPce2lceConfigSize1_HAL(pKCeIter))
            {
                pKCeIter->pPceLceMap[pceIndex]                  = lceIndex;
                pAvailablePceMaskForConnectingHub[hshubIndex]  &= ~(NVBIT(pceIndex));
                pLocalGrceMap[grceIdx]                          = DRF_NUM(_CE, _GRCE_CONFIG, _SHARED, 0) |
                                                                  DRF_DEF(_CE, _GRCE_CONFIG, _SHARED_LCE, _NONE);

                 NV_PRINTF(LEVEL_INFO, "GRCE is not shared and mapped to LCE Index: 0.\n");
            }
        }
        else
        {
            // Find the lowest PCE used which is not Decomp and share with that LCE
            NvU32 size = kceGetPce2lceConfigSize1_HAL(pKCeIter);

            for (pceIndex = 0; pceIndex < size; ++pceIndex)
            {
                if ((NVBIT32(pceIndex) & pKCeIter->decompPceMask) == 0)
                {
                    if (pKCeIter->pPceLceMap[pceIndex] != NV2080_CTRL_CE_UPDATE_PCE_LCE_MAPPINGS_INVALID_LCE)
                    {
                        lceIndex                  = pKCeIter->pPceLceMap[pceIndex];
                        KernelCE *pKCeLce         = GPU_GET_KCE(pGpu, lceIndex);
                        if(pKCeLce != NULL)
                        {
                           pKCeLce->ceCapsMask   |= NVBIT32(CE_CAPS_GRCE);
                        }
                        NV_PRINTF(LEVEL_INFO,
                                  "GRCE is shared and mapped to LCE Index: %d.\n",
                                  lceIndex);
                        break;
                    }
                }
            }

            pLocalGrceMap[grceIdx] = DRF_NUM(_CE, _GRCE_CONFIG, _SHARED, 1) |
                                     DRF_NUM(_CE, _GRCE_CONFIG, _SHARED_LCE, lceIndex);
        }
    }
    KCE_ITER_END;
}

/**
 * @brief This function takes care of mappings for PCIe Case
 *        H2D and D2H. Map 1 PCE from 3 HSHUBs to on LCE - do
 *        this twice for H2D and D2H case.
 *
 * @param[in]   pGpu                               OBJGPU pointer
 * @param[in]   pKCe                               KernelCE pointer
 * @param[in]   pAvailablePceMaskForConnectingHub  Pointer to CEs available per HSHUB
 * @param[out]  pExposedLceMask                    Pointer to LCE Mask
 */
void
kceMapPceLceForPCIe_GB100
(
    OBJGPU      *pGpu,
    KernelCE    *pKCe,
    NvU32       *pAvailablePceMaskForConnectingHub,
    NvU32       *pExposedLceMask
)
{
    KernelCE               *pKCeIter        = NULL;
    NvU32                   maxLceCount;
    NvU32                   numPcesAssigned = 0;
    NvU32                   pceIndex;
    NvU32                   lceMask;
    NvU32                   lceIndex;
    NvU32                   hshubIndex;
    NvU32                   index;
    NvU32                   lastShimInstance = 0;
    NvU32                   numPcesPerLce;
    NvU32                   numLces;
    NvU32                   numLcesForPcieType;
    NvU32                   supportedPceMask;
    NvU32                   supportedLceMask;
    NvU32                   pcesPerHshub;
    NV2080_CTRL_CE_LCE_TYPE pcieLceTypesToAssign[2];
    CE_CAPABILITY           ceCapsForLce[2];

    kceGetPceConfigForLceType(
        pGpu,
        pKCe,
        NV2080_CTRL_CE_LCE_TYPE_PCIE,
        &numPcesPerLce,
        &numLces,
        &supportedPceMask,
        &supportedLceMask,
        &pcesPerHshub);

    maxLceCount = supportedLceMask;
    HIGHESTBITIDX_32(maxLceCount); // HIGHESTBITIDX_32 is destructive.
    maxLceCount++;

    switch (numLces)
    {
        case 1:
            pcieLceTypesToAssign[0] = NV2080_CTRL_CE_LCE_TYPE_PCIE;
            ceCapsForLce[0]         = NVBIT32(CE_CAPS_SYSMEM_READ) | NVBIT32(CE_CAPS_SYSMEM_WRITE);
            break;
        case 2:
            pcieLceTypesToAssign[0] = NV2080_CTRL_CE_LCE_TYPE_PCIE_RD;
            pcieLceTypesToAssign[1] = NV2080_CTRL_CE_LCE_TYPE_PCIE_WR;
            ceCapsForLce[0]         = NVBIT32(CE_CAPS_SYSMEM_READ);
            ceCapsForLce[1]         = NVBIT32(CE_CAPS_SYSMEM_WRITE);
            break;
        default:
            NV_PRINTF(LEVEL_ERROR, "Invalid number of LCEs for PCIe.!\n");
            return;
    }

    //
    // Choose the last shim instance that has PCEs
    //
    KCE_ITER_SHIM_BEGIN(pGpu, pKCeIter)
    {
        //
        // Skip any shims that don't have any PCEs connected to the hubs
        //
        if (pKCeIter->shimConnectingHubMask == 0)
        {
            continue;
        }

        hshubIndex = CE_GET_LOWEST_AVAILABLE_IDX(pKCeIter->shimConnectingHubMask);
        pceIndex   = CE_GET_LOWEST_AVAILABLE_IDX((pAvailablePceMaskForConnectingHub[hshubIndex] &
                                                  supportedPceMask));

        if (pceIndex < kceGetPce2lceConfigSize1_HAL(pKCeIter))
        {
            lastShimInstance = NV_MAX(pKCeIter->shimInstance, lastShimInstance);
        }
    }
    KCE_ITER_END;

    KCE_ITER_SHIM_BEGIN(pGpu, pKCeIter)
    {
        //
        // Assign PCEs for PCIe only on last shim instance.
        //
        if (pKCeIter->shimInstance != lastShimInstance)
        {
            continue;
        }

        for (index = 0; index < numLces; ++index)
        {
            kceGetPceConfigForLceType(pGpu,
                                      pKCe,
                                      pcieLceTypesToAssign[index],
                                      &numPcesPerLce,
                                      &numLcesForPcieType,
                                      &supportedPceMask,
                                      &supportedLceMask,
                                      &pcesPerHshub);

            lceMask = kceGetLceMaskForShimInstance_HAL(pGpu, pKCeIter) &
                      supportedLceMask &
                     ~(*pExposedLceMask);

            numPcesAssigned     = 0;
            lceIndex            = CE_GET_LOWEST_AVAILABLE_IDX(lceMask);

            if (lceIndex >= maxLceCount)
            {
                break;
            }

            while (numPcesAssigned < numPcesPerLce)
            {
                NvBool bPceAssignedInCurrentIteration = NV_FALSE;

                FOR_EACH_INDEX_IN_MASK(32, hshubIndex, pKCeIter->shimConnectingHubMask)
                {
                    if (numPcesAssigned >= numPcesPerLce)
                    {
                        break;
                    }

                    if (pAvailablePceMaskForConnectingHub[hshubIndex] == 0)
                    {
                        continue;
                    }

                    pceIndex = CE_GET_LOWEST_AVAILABLE_IDX((pAvailablePceMaskForConnectingHub[hshubIndex] & supportedPceMask));

                    if (pceIndex < kceGetPce2lceConfigSize1_HAL(pKCeIter))
                    {
                        pAvailablePceMaskForConnectingHub[hshubIndex]   &= ~(NVBIT32(pceIndex));
                        //
                        // Convert absolute LCE index to shim local index
                        //
                        pKCeIter->pPceLceMap[pceIndex]           = lceIndex % NV_KCE_GROUP_ID_STRIDE;
                        *pExposedLceMask                        |= NVBIT(lceIndex);
                        lceMask                                 &= ~(NVBIT32(lceIndex));
                        bPceAssignedInCurrentIteration           = NV_TRUE;

                        KernelCE *pKCeLce = GPU_GET_KCE(pGpu, lceIndex);

                        NV_ASSERT(pKCeLce != NULL);
                        pKCeLce->ceCapsMask |= ceCapsForLce[index];

                        NV_PRINTF(LEVEL_INFO,
                                  "PCIe CE Mapping -- PCE Index: %d -> LCE Index: %d\n",
                                  pceIndex,
                                  lceIndex);

                        ++numPcesAssigned;
                    }
                }
                FOR_EACH_INDEX_IN_MASK_END;

                // If unable to assign anymore PCEs because of floorsweeping, break from the loop.
                if (!bPceAssignedInCurrentIteration)
                    break;
            }
        }
    }
    KCE_ITER_END;
}

/**
 * @brief This function takes care of mappings for C2C case.
 *        Assign the required number of PCEs by preferring the
 *        even numbered LCE.
 *
 * @param[in]   pGpu                                OBJGPU pointer
 * @param[in]   pKCe                                KernelCE pointer
 * @param[in]   pAvailablePceMaskForConnectingHub   Pointer to CEs available per HSHUB
 * @param[out]  pLocalPceLceMap                     Pointer to PCE-LCE array
 * @param[out]  pExposedLceMask                     Pointer to LCE Mask
 */
NV_STATUS
kceMapPceLceForC2C_GB100
(
    OBJGPU      *pGpu,
    KernelCE    *pKCe,
    NvU32       *pAvailablePceMaskForConnectingHub,
    NvU32       *pLocalPceLceMap,
    NvU32       *pExposedLceMask
)
{
    KernelCE               *pKCeIter = NULL;
    NvU32                   c2cIndex = 0;
    NvU32                   maxLceCount;
    NvU32                   pceIndex;
    NvU32                   lceIndex;
    NvU32                   lceMask;
    NvU32                   hshubIndex;
    NvU32                   index;
    NvU32                   lastShimInstance = 0;
    NvU32                   numPcesPerLce;
    NvU32                   numLces;
    NvU32                   numLcesForC2CType;
    NvU32                   supportedPceMask;
    NvU32                   supportedLceMask;
    NvU32                   pcesPerHshub;
    NvU32                   numC2CLcesToAssign;
    NvBool                  bAssignedAtleastOneLce;
    NvU32                   numPcesAssigned;
    NV2080_CTRL_CE_LCE_TYPE c2cLceTypesToAssign[NV_MAX_C2C_LCES];
    CE_CAPABILITY           ceCapsForLce[NV_MAX_C2C_LCES];

    kceGetPceConfigForLceType(pGpu,
                              pKCe,
                              NV2080_CTRL_CE_LCE_TYPE_C2C,
                              &numPcesPerLce,
                              &numLces,
                              &supportedPceMask,
                              &supportedLceMask,
                              &pcesPerHshub);

    maxLceCount = supportedLceMask;
    HIGHESTBITIDX_32(maxLceCount); // HIGHESTBITIDX_32 is destructive.
    maxLceCount++;

    switch (numLces)
    {
        case 1:
            c2cLceTypesToAssign[0] = NV2080_CTRL_CE_LCE_TYPE_C2C;
            ceCapsForLce[0]        = NVBIT32(CE_CAPS_SYSMEM_READ) | NVBIT32(CE_CAPS_SYSMEM_WRITE);
            break;
        case 2:
            c2cLceTypesToAssign[0] = NV2080_CTRL_CE_LCE_TYPE_C2C_H2D;
            c2cLceTypesToAssign[1] = NV2080_CTRL_CE_LCE_TYPE_C2C_D2H;
            ceCapsForLce[0]        = NVBIT32(CE_CAPS_SYSMEM_READ);
            ceCapsForLce[1]        = NVBIT32(CE_CAPS_SYSMEM_WRITE);
            break;
        default:
            NV_PRINTF(LEVEL_ERROR, "Invalid number of LCEs for C2C.!\n");
            return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    // Choose the last shim instance that has PCEs
    KCE_ITER_SHIM_BEGIN(pGpu, pKCeIter)
    {
        // Skip any shims that don't have any PCEs connected to the hubs
        if (pKCeIter->shimConnectingHubMask == 0)
        {
            continue;
        }

        hshubIndex = CE_GET_LOWEST_AVAILABLE_IDX(pKCeIter->shimConnectingHubMask);
        pceIndex   = CE_GET_LOWEST_AVAILABLE_IDX((pAvailablePceMaskForConnectingHub[hshubIndex] &
                                                  supportedPceMask));

        if (pceIndex < kceGetPce2lceConfigSize1_HAL(pKCeIter))
        {
            lastShimInstance = NV_MAX(pKCeIter->shimInstance, lastShimInstance);
        }
    }
    KCE_ITER_END;

    KCE_ITER_SHIM_BEGIN(pGpu, pKCeIter)
    {
        // Assign PCEs for C2C only on last shim instance.
        if (pKCeIter->shimInstance != lastShimInstance)
        {
            continue;
        }

        numC2CLcesToAssign = numLces;

        while (numC2CLcesToAssign > 0)
        {
            kceGetPceConfigForLceType(pGpu,
                                      pKCe,
                                      c2cLceTypesToAssign[c2cIndex],
                                      &numPcesPerLce,
                                      &numLcesForC2CType,
                                      &supportedPceMask,
                                      &supportedLceMask,
                                      &pcesPerHshub);

            lceMask = kceGetLceMaskForShimInstance_HAL(pGpu, pKCeIter) &
                      supportedLceMask &
                     ~(*pExposedLceMask);

            numPcesAssigned = 0;

            if ((lceMask & NV_CE_EVEN_ASYNC_LCE_MASK) != 0)
            {
                lceMask &= NV_CE_EVEN_ASYNC_LCE_MASK;
            }

            bAssignedAtleastOneLce  = NV_FALSE;
            lceIndex                = CE_GET_LOWEST_AVAILABLE_IDX(lceMask);
            if (lceIndex >= maxLceCount)
            {
                break;
            }

            FOR_EACH_INDEX_IN_MASK(32, hshubIndex, pKCeIter->shimConnectingHubMask)
            {
                if (pAvailablePceMaskForConnectingHub[hshubIndex] == 0)
                {
                    // floorswept HSHUB with no PCEs available
                    continue;
                }

                if (numPcesAssigned == numPcesPerLce)
                {
                    break;
                }

                for (index = 0; index < pcesPerHshub; ++index)
                {
                    pceIndex = CE_GET_LOWEST_AVAILABLE_IDX((pAvailablePceMaskForConnectingHub[hshubIndex] & supportedPceMask));
                    if ((pceIndex >= kceGetPce2lceConfigSize1_HAL(pKCeIter)) || (pceIndex >= NV2080_CTRL_MAX_PCES))
                    {
                        break;
                    }

                    pAvailablePceMaskForConnectingHub[hshubIndex] &= ~(NVBIT32(pceIndex));

                    // Convert absolute LCE index to shim local index
                    pKCeIter->pPceLceMap[pceIndex]  = lceIndex % NV_KCE_GROUP_ID_STRIDE;
                    lceMask                        &= ~NVBIT32(lceIndex);

                    *pExposedLceMask               |= NVBIT32(lceIndex);
                    bAssignedAtleastOneLce          = NV_TRUE;
                    numPcesAssigned++;
                    KernelCE *pKCeLce               = GPU_GET_KCE(pGpu, lceIndex);
                    if(pKCeLce != NULL)
                    {
                       pKCeLce->ceCapsMask         |= ceCapsForLce[c2cIndex];
                    }
                    NV_PRINTF(LEVEL_INFO,
                              "C2C CE Mapping -- PCE Index: %d -> LCE Index: %d\n",
                              pceIndex,
                              lceIndex);

                    if (numPcesAssigned == numPcesPerLce)
                    {
                        break;
                    }
                }
            }
            FOR_EACH_INDEX_IN_MASK_END;

            if (bAssignedAtleastOneLce)
            {
                --numC2CLcesToAssign;
                ++c2cIndex;
            }
            else
            {
                NV_PRINTF(LEVEL_ERROR, "Unable to assign the required number of LCEs for C2C.\n");
                return NV_ERR_INSUFFICIENT_RESOURCES;
            }
        }
    }
    KCE_ITER_END;

    return NV_OK;
}

/**
 * @brief This function takes care of mappings for Scrub Case.
 *        Map 2 PCEs from each HSHUB and map to lowest even
 *        numbered LCE if possible. If no even numbered LCE
 *        is free, choose an odd numbered LCE. (Bug 4656188)
 *
 * @param[in]   pGpu                               OBJGPU pointer
 * @param[in]   pKCe                               KernelCE pointer
 * @param[in]   pAvailablePceMaskForConnectingHub  Pointer to CEs available per HSHUB
 * @param[out]  pExposedLceMask                    Pointer to LCE Mask
 */
NV_STATUS
kceMapPceLceForScrub_GB100
(
    OBJGPU      *pGpu,
    KernelCE    *pKCe,
    NvU32       *pAvailablePceMaskForConnectingHub,
    NvU32       *pExposedLceMask
)
{
    KernelCE   *pKCeIter         = NULL;
    NvU32       maxLceCount;
    NvU32       status           = NV_WARN_NOTHING_TO_DO;
    NvU32       pceIndex;
    NvU32       lceIndex;
    NvU32       lceMask;
    NvU32       hshubIndex;
    NvU32       index;
    NvU32       lastShimInstance = 0;
    NvU32       numPcesPerLce;
    NvU32       numLces;
    NvU32       supportedPceMask;
    NvU32       supportedLceMask;
    NvU32       pcesPerHshub;

    kceGetPceConfigForLceType(pGpu,
                              pKCe,
                              NV2080_CTRL_CE_LCE_TYPE_SCRUB,
                              &numPcesPerLce,
                              &numLces,
                              &supportedPceMask,
                              &supportedLceMask,
                              &pcesPerHshub);

    maxLceCount = supportedLceMask;
    HIGHESTBITIDX_32(maxLceCount); // HIGHESTBITIDX_32 is destructive.
    maxLceCount++;

    //
    // Choose the last shim instance that has PCEs
    //
    KCE_ITER_SHIM_BEGIN(pGpu, pKCeIter)
    {
        //
        // Skip any shims that don't have any PCEs connected to the hubs
        //
        if (pKCeIter->shimConnectingHubMask == 0)
        {
            continue;
        }

        hshubIndex = CE_GET_LOWEST_AVAILABLE_IDX(pKCeIter->shimConnectingHubMask);
        pceIndex   = CE_GET_LOWEST_AVAILABLE_IDX((pAvailablePceMaskForConnectingHub[hshubIndex] &
                                                  supportedPceMask));

        if (pceIndex < kceGetPce2lceConfigSize1_HAL(pKCeIter))
        {
            lastShimInstance = NV_MAX(pKCeIter->shimInstance, lastShimInstance);
        }
    }
    KCE_ITER_END;

    KCE_ITER_SHIM_BEGIN(pGpu, pKCeIter)
    {
        //
        // Assign PCEs for PCIe only on last shim instance.
        //
        if (pKCeIter->shimInstance != lastShimInstance)
        {
            continue;
        }

        lceMask = kceGetLceMaskForShimInstance_HAL(pGpu, pKCeIter) &
                  supportedLceMask &
                  ~(*pExposedLceMask);

        if ((lceMask & NV_CE_EVEN_ASYNC_LCE_MASK) != 0)
        {
            lceMask &= NV_CE_EVEN_ASYNC_LCE_MASK;
        }

        lceIndex = CE_GET_LOWEST_AVAILABLE_IDX(lceMask);
        if (lceIndex >= maxLceCount)
        {
            break;
        }

        FOR_EACH_INDEX_IN_MASK(32, hshubIndex, pKCeIter->shimConnectingHubMask)
        {
            if (pAvailablePceMaskForConnectingHub[hshubIndex] == 0)
            {
                // floorswept HSHUB with no PCEs available
                continue;
            }

            for (index = 0; index < pcesPerHshub; ++index)
            {
                pceIndex = CE_GET_LOWEST_AVAILABLE_IDX((pAvailablePceMaskForConnectingHub[hshubIndex] & supportedPceMask));
                if (pceIndex >= NV2080_CTRL_MAX_PCES)
                {
                    //
                    // Scrub limits 2 PCEs per HSHUB. If not enough are available
                    // it is acceptable to continue mapping on the next HSHUB
                    //
                    continue;
                }

                if ((pceIndex < kceGetPce2lceConfigSize1_HAL(pKCeIter)) && (pceIndex < NV2080_CTRL_MAX_PCES))
                {
                    pAvailablePceMaskForConnectingHub[hshubIndex]   &= (~(NVBIT32(pceIndex)));

                    //
                    // Convert absolute LCE index to shim local index
                    //
                    pKCeIter->pPceLceMap[pceIndex]           = lceIndex % NV_KCE_GROUP_ID_STRIDE;

                    *pExposedLceMask                        |= NVBIT32(lceIndex);

                    KernelCE *pKCeLce                        = GPU_GET_KCE(pGpu, lceIndex);
                    if(pKCeLce != NULL)
                    {
                       pKCeLce->ceCapsMask                  |= NVBIT32(CE_CAPS_SCRUB);
                    }
                    NV_PRINTF(LEVEL_INFO,
                             "Scrub CE Mapping -- PCE Index: %d -> LCE Index: %d\n",
                              pceIndex,
                              lceIndex);

                    status = NV_OK;
                }
            }
        }
        FOR_EACH_INDEX_IN_MASK_END;
    }
    KCE_ITER_END;

    return status;
}

/**
 * @brief Some clients rely on LCE4 also being turned on when there
 *        are no NVLink peers. This function sets up LCE4 if not already
 *        assigned by mapping it to one PCE.
 *
 * @param[in]   pGpu                               OBJGPU pointer
 * @param[in]   pKCe                               KernelCE pointer
 * @param[in]   pAvailablePceMaskForConnectingHub  Pointer to CEs available per HSHUB
 * @param[out]  pLocalPceLceMap                    Pointer to PCE-LCE array
 * @param[out]  pExposedLceMask                    Pointer to LCE Mask
 *
 * Returns NV_OK
 */
NV_STATUS
kceMapAsyncLceDefault_GB100
(
    OBJGPU     *pGpu,
    KernelCE   *pKCe,
    NvU32      *pAvailablePceMaskForConnectingHub,
    NvU32      *pLocalPceLceMap,
    NvU32      *pExposedLceMask,
    NvU32       numDefaultPces
)
{
    KernelCE   *pKCeIter    = NULL;
    NvU32       maxLceCount;
    NvU32       pceIndex;
    NvU32       lceIndex;
    NvU32       lceMask;
    NvU32       availablePceMask;
    NvU32       hshubIndex;
    NvU32       numPcesPerLce;
    NvU32       numLces;
    NvU32       supportedPceMask;
    NvU32       supportedLceMask;
    NvU32       pcesPerHshub;

    //
    // Return early if LCE4 is already assigned.
    //
    if (*pExposedLceMask & NVBIT32(NV_CE_LCE_4))
    {
        return NV_OK;
    }

    kceGetPceConfigForLceType(
        pGpu,
        pKCe,
        NV2080_CTRL_CE_LCE_TYPE_SCRUB,
        &numPcesPerLce,
        &numLces,
        &supportedPceMask,
        &supportedLceMask,
        &pcesPerHshub);

    maxLceCount = supportedLceMask;
    HIGHESTBITIDX_32(maxLceCount); // HIGHESTBITIDX_32 is destructive.
    maxLceCount++;

    // Assign all available non Decomp PCEs to lowest LCE
    KCE_ITER_SHIM_BEGIN(pGpu, pKCeIter)
    {
        lceMask = kceGetLceMaskForShimInstance_HAL(pGpu, pKCeIter)
                & NV_CE_EVEN_ASYNC_LCE_MASK
                & ~(*pExposedLceMask);

        lceIndex = CE_GET_LOWEST_AVAILABLE_IDX(lceMask);
        if (lceIndex >= maxLceCount)
        {
            break;
        }

        FOR_EACH_INDEX_IN_MASK(32, hshubIndex, pKCeIter->shimConnectingHubMask)
        {
            availablePceMask = pAvailablePceMaskForConnectingHub[hshubIndex] & supportedPceMask;
            FOR_EACH_INDEX_IN_MASK(32, pceIndex, availablePceMask)
            {
                //
                // Convert absolute LCE index to shim local index
                //
                pKCeIter->pPceLceMap[pceIndex]                  = lceIndex % NV_KCE_GROUP_ID_STRIDE;
                *pExposedLceMask                               |= NVBIT32(lceIndex);
                pAvailablePceMaskForConnectingHub[hshubIndex]  &= (~NVBIT32(pceIndex));

                KernelCE *pKCeLce = GPU_GET_KCE(pGpu, lceIndex);

                NV_ASSERT(pKCeLce != NULL);
                pKCeLce->ceCapsMask |= NVBIT32(CE_CAPS_SCRUB);

                NV_PRINTF(LEVEL_INFO,
                          "Async LCE Mapping -- PCE Index: %d -> LCE Index: %d\n",
                          pceIndex,
                          lceIndex);

                return NV_OK;
            }
            FOR_EACH_INDEX_IN_MASK_END;
        }
        FOR_EACH_INDEX_IN_MASK_END;
    }
    KCE_ITER_END;

    return NV_OK;
}

/**
 * @brief Use the algorithm to determine all the mappings for
 *        the given GPU.
 *
 * @param[in]   pGpu                   OBJGPU pointer
 * @param[in]   pKCe                   KernelCE pointer
 * @param[out]  pLocalPceLceMap        Pointer to PCE-LCE array
 * @param[out]  pLocalGrceMap          Pointer to GRCE array
 * @param[out]  plocalExposedCeMask     Pointer to LCE Mask
 *
 * Returns NV_TRUE if algorithm ran to completion with no errors
 */
NV_STATUS
kceGetMappings_GB100
(
    OBJGPU                  *pGpu,
    KernelCE                *pKCe,
    NVLINK_TOPOLOGY_PARAMS  *pTopoParams,
    NvU32                   *pLocalPceLceMap,
    NvU32                   *pLocalGrceMap,
    NvU32                   *pExposedLceMask
)
{
    NV_STATUS       status = NV_OK;
    KernelNvlink   *pKernelNvlink   = GPU_GET_KERNEL_NVLINK(pGpu);
    KernelBif      *pKernelBif      = GPU_GET_KERNEL_BIF(pGpu);
    KernelCE       *pKCeIter        = NULL;
    NvU32           availablePceMaskForConnectingHub[NV2080_CTRL_CE_MAX_HSHUBS];
    KernelCE       *pKCeShimOwner;
    NvU32           pceIndex;
    NvU32           hubIndex;
    NvBool          bIsC2CEnabled   = pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_IS_C2C_LINK_UP);

    //
    // Mapping algorithm will map all Shim Instances, but only
    // needs to be called one time. Any repeated calls for additional
    // shim instances should be ignored and getMappings will return
    // the mappings which have already been calculated. However,
    // if called for initial shim, remap is necessary because NVLink
    // status may have changed and requires remap.
    //
    status = kceFindShimOwner(pGpu, pKCe, &pKCeShimOwner);
    if (pKCeShimOwner == NULL || status != NV_OK)
    {
        return NV_OK;
    }

    // Do not change the mappings if the KernelCE objects are unloaded.
    if (pKCeShimOwner->pPceLceMap == NULL)
    {
        return NV_OK;
    }

    if (pKCeShimOwner->shimInstance != 0 && pKCeShimOwner->bMapComplete)
    {
        goto copyToLocalMap;
    }

    //
    // Initialize the available PCE mask for all the HSHUBs to 0.
    //
    for (hubIndex = 0; hubIndex < NV_ARRAY_ELEMENTS(availablePceMaskForConnectingHub); hubIndex++)
    {
        availablePceMaskForConnectingHub[hubIndex] = 0;
    }

    //
    // Prepare the per-HSHUB available PCE mask for all HSHUBs
    //
    KCE_ITER_SHIM_BEGIN(pGpu, pKCeIter)
    {
        //
        // Get the PCEs available for the current shim and update the local map (availablePceMaskForConnectingHub).
        //
        kceGetAvailableHubPceMask(pGpu, pKCeIter, pTopoParams);

        for (hubIndex = 0; hubIndex < NV_ARRAY_ELEMENTS(availablePceMaskForConnectingHub); hubIndex++)
        {
            availablePceMaskForConnectingHub[hubIndex] |= pTopoParams->pceAvailableMaskPerConnectingHub[hubIndex];

            if (pTopoParams->pceAvailableMaskPerConnectingHub[hubIndex])
            {
                pKCeIter->shimConnectingHubMask |= NVBIT32(hubIndex);
            }
        }

        // Do not change the mappings if the KernelCE objects are unloaded.
        if (pKCeIter->pPceLceMap == NULL)
        {
            return NV_OK;
        }

        for (pceIndex = 0; pceIndex < kceGetPce2lceConfigSize1_HAL(pKCeIter); pceIndex++)
        {
            pKCeIter->pPceLceMap[pceIndex] = NV2080_CTRL_CE_UPDATE_PCE_LCE_MAPPINGS_INVALID_LCE;
        }

        pKCeIter->bMapComplete = NV_TRUE;
    }
    KCE_ITER_END;

    // Reset the CE caps before trying to allocate new PCE-LCE mapping
    KCE_ITER_ALL_BEGIN(pGpu, pKCeIter, 0)
    {
        pKCeIter->ceCapsMask = 0x0;
    }
    KCE_ITER_END;

    // After assigninig the PCE, update CE CAPS mask
    //
    // A) Assign PCEs for PCIe or C2C case
    //
    if (gpuIsSelfHosted(pGpu) && bIsC2CEnabled)
    {
        kceMapPceLceForC2C_HAL(pGpu,
                               pKCeShimOwner,
                               availablePceMaskForConnectingHub,
                               pLocalPceLceMap,
                               pExposedLceMask);
    }
    else
    {
        kceMapPceLceForPCIe_HAL(pGpu,
                                pKCeShimOwner,
                                availablePceMaskForConnectingHub,
                                pExposedLceMask);
    }

    //
    // B) Assign Decomp PCEs
    //
    kceMapPceLceForDecomp_HAL(pGpu,
                              pKCeShimOwner,
                              availablePceMaskForConnectingHub,
                              pExposedLceMask);

    //
    // D) Assign PCEs to Peers if nvlink is enabled or NVswitch enabled
    //
    if (pKernelNvlink && !knvlinkIsForcedConfig(pGpu, pKernelNvlink))
    {
        kceMapPceLceForNvlinkPeers_HAL(pGpu,
                                       pKCeShimOwner,
                                       availablePceMaskForConnectingHub,
                                       pLocalPceLceMap,
                                       pExposedLceMask);
    }

    //
    // C) Assign LCEs for work launch and completion. We need this only on platforms
    // that require bounce buffers. x86 is one such platform.
    //
#if defined(NVCPU_X86_64)
    kceMapPceLceForWorkSubmitLces_HAL(pGpu,
                                      pKCeShimOwner,
                                      availablePceMaskForConnectingHub,
                                      pExposedLceMask);
#endif

    //
    // E) Assign PCEs for Scrub
    //
    kceMapPceLceForScrub_HAL(pGpu,
                             pKCeShimOwner,
                             availablePceMaskForConnectingHub,
                             pExposedLceMask);

    //
    // GrCE is not being used in CC. If we are assigning a GrCE, it needs to be in primary die.
    // In MPT mode, all the 16 AES capable PCEs are being assigned to NVLINK LCE. To have a shared
    // GrCE, we need to share it with NVLINK LCE, but this makes NVLINK LCE a non-secure LCE. To have
    // a dedicated GrCE, we need to remove a PCE from NVLINK LCE and use it for GrCE, but this would
    // reduce the BW of NVLINK LCE. Both of these are not preferred. Hence, removing the GrCE in CC
    // mode as it is anyways not used.
    //
    if (!gpuIsCCFeatureEnabled(pGpu))
    {
        //
        // F) Assign GRCE after PCEs to ensure proper sharing if required
        //
        kceMapPceLceForGRCE_HAL(pGpu,
                                pKCeShimOwner,
                                availablePceMaskForConnectingHub,
                                pLocalPceLceMap,
                                pExposedLceMask,
                                pLocalGrceMap,
                                pTopoParams->fbhubPceMask);
    }

    //
    // At minimum PCEs need to be assigned for LCE4. Assign PCEs for LCE4 if not already assigned.
    //
    kceMapAsyncLceDefault_HAL(pGpu,
                              pKCeShimOwner,
                              availablePceMaskForConnectingHub,
                              pLocalPceLceMap,
                              pExposedLceMask,
                              0);

    NV_PRINTF(LEVEL_INFO, "Assigned LCE mask = 0x%x.\n", *pExposedLceMask);

copyToLocalMap:
    //
    // Return PceLceMap depending whichever shim is requested
    //
    for (pceIndex = 0; pceIndex < kceGetPce2lceConfigSize1_HAL(pKCeShimOwner); pceIndex++)
    {
        pLocalPceLceMap[pceIndex] = pKCeShimOwner->pPceLceMap[pceIndex];
    }

    return NV_OK;
}

/**
 * @brief Returns the LCEs assigned for PCIe/C2C read write traffic.
 *
 * @param[in]      pGpu       OBJGPU pointer
 * @param[in]      pKCe       KernelCE pointer
 * @param[out]     pReadLce   LCE assigned for read
 * @param[out]     pWriteLce  LCE assigned for write
 */
void
kceGetSysmemRWLCEs_GB100
(
    OBJGPU      *pGpu,
    KernelCE    *pKCe,
    NvU32       *pReadLce,
    NvU32       *pWriteLce
)
{
    KernelCE   *pKCeShimIter;
    KernelCE   *pKCeIter;

    KCE_ITER_SHIM_BEGIN(pGpu, pKCeShimIter)
    {
        KCE_ITER_BEGIN(pGpu, pKCeShimIter, pKCeIter, 0)
        {
            if (kceIsCeSysmemRead_HAL(pGpu, pKCeIter))
            {
                NV_PRINTF(LEVEL_INFO, "LCE %d assigned for Sysmem Rd.\n", pKCeIter->publicID);
                *pReadLce = pKCeIter->publicID;
            }

            if (kceIsCeSysmemWrite_HAL(pGpu, pKCeIter))
            {
                NV_PRINTF(LEVEL_INFO, "LCE %d assigned for Sysmem Wr.\n", pKCeIter->publicID);
                *pWriteLce = pKCeIter->publicID;
            }
        }
        KCE_ITER_END;
    }
    KCE_ITER_END;
}

/**
 * @brief Returns the LCE mask for the corresponding shim instance.
 *
 * @param[in]      pGpu       OBJGPU pointer
 * @param[in]      pKCe       KernelCE pointer
 */
NvU32
kceGetLceMaskForShimInstance_GB100
(
    OBJGPU      *pGpu,
    KernelCE    *pKCe
)
{
    NvU32 shimMask;
    NvU32 shimInstance = pKCe->shimInstance;

    shimMask = NVBIT32(NV_KCE_GROUP_ID_STRIDE * (shimInstance + 1)) -
               NVBIT32(NV_KCE_GROUP_ID_STRIDE * shimInstance);

    return kceGetLceMask(pGpu) & shimMask;
}

/**
 * @brief Returns true if the current LCE is assigned for Sysmem Rd.
 *
 * @param[in]      pGpu       OBJGPU pointer
 * @param[in]      pKCe       KernelCE pointer
 */
NvBool
kceIsCeSysmemRead_GB100
(
    OBJGPU *pGpu,
    KernelCE *pKCe
)
{
    return (pKCe->ceCapsMask & NVBIT32(CE_CAPS_SYSMEM_READ)) && !pKCe->bStubbed;
}

/**
 * @brief Returns true if the current LCE is assigned for Sysmem Wr.
 *
 * @param[in]      pGpu       OBJGPU pointer
 * @param[in]      pKCe       KernelCE pointer
 */
NvBool
kceIsCeSysmemWrite_GB100
(
    OBJGPU *pGpu,
    KernelCE *pKCe
)
{
    return (pKCe->ceCapsMask & NVBIT32(CE_CAPS_SYSMEM_WRITE)) && !pKCe->bStubbed;
}

/**
 * @brief Returns true if the current LCE is assigned for NVLINK.
 *
 * @param[in]      pGpu       OBJGPU pointer
 * @param[in]      pKCe       KernelCE pointer
 */
NvBool
kceIsCeNvlinkP2P_GB100
(
    OBJGPU      *pGpu,
    KernelCE    *pKCe
)
{
    return (pKCe->ceCapsMask & NVBIT32(CE_CAPS_NVLINK_P2P)) && !pKCe->bStubbed;
}

NvBool
kceIsCCWorkSubmitLce_GB100
(
    OBJGPU      *pGpu,
    KernelCE    *pKCe
)
{
    return (pKCe->ceCapsMask & NVBIT32(CE_CAPS_CC_WORK_SUBMIT)) && !pKCe->bStubbed;
}

/**
 * @brief Returns true if the current LCE can be used for fast scrubbing.
 *
 * @param[in]      pGpu       OBJGPU pointer
 * @param[in]      pKCe       KernelCE pointer
 */
NvBool
kceIsScrubLce_GB100
(
    OBJGPU      *pGpu,
    KernelCE    *pKCe
)
{
    return (pKCe->ceCapsMask & NVBIT32(CE_CAPS_SCRUB)) && !pKCe->bStubbed;
}

/**
 * @brief Sets the CE capabilities based on PCE-LCE mapping algorithm
 *
 * @param[in]      pGpu       OBJGPU pointer
 * @param[in]      pKCe       KernelCE pointer
 * @param[in]      pKCeCaps   CE capabilities table
 */
void
kceAssignCeCaps_GB100
(
    OBJGPU      *pGpu,
    KernelCE    *pKCe,
    NvU8        *pKCeCaps
)
{
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

    //
    // Set Sysmem Read/Write Caps only in non-MIG mode.
    // TODO: MIG Mode need to be fixed in future
    //
    if (!IS_MIG_IN_USE(pGpu))
    {
        if (kceIsCeSysmemRead_HAL(pGpu, pKCe))
        {
            NV_PRINTF(LEVEL_INFO, "LCE %d assigned for Sysmem Rd.\n", pKCe->publicID);
            RMCTRL_SET_CAP(pKCeCaps, NV2080_CTRL_CE_CAPS, _CE_SYSMEM_READ);
        }

        if (kceIsCeSysmemWrite_HAL(pGpu, pKCe))
        {
            NV_PRINTF(LEVEL_INFO, "LCE %d assigned for Sysmem Wr.\n", pKCe->publicID);
            RMCTRL_SET_CAP(pKCeCaps, NV2080_CTRL_CE_CAPS, _CE_SYSMEM_WRITE);
        }

        if (kceIsScrubLce_HAL(pGpu, pKCe))
        {
            NV_PRINTF(LEVEL_INFO, "LCE %d assigned for fast scrubbing.\n", pKCe->publicID);
            RMCTRL_SET_CAP(pKCeCaps, NV2080_CTRL_CE_CAPS, _CE_SCRUB);
        }
    }

    if (pKernelNvlink != NULL && kceIsCeNvlinkP2P_HAL(pGpu, pKCe))
    {
        NV_PRINTF(LEVEL_INFO, "LCE %d assigned for NVLINK.\n", pKCe->publicID);
        RMCTRL_SET_CAP(pKCeCaps, NV2080_CTRL_CE_CAPS, _CE_NVLINK_P2P);
    }

    if (gpuIsCCFeatureEnabled(pGpu))
    {
        if (kceIsCCWorkSubmitLce_HAL(pGpu, pKCe))
        {
            NV_PRINTF(LEVEL_INFO, "LCE %d assigned for CC Work Submit.\n", pKCe->publicID);
            RMCTRL_SET_CAP(pKCeCaps, NV2080_CTRL_CE_CAPS, _CE_CC_WORK_SUBMIT);
        }
    }
}

/**
 * @brief Returns the CEs marked for P2P based on PCE-LCE mapping algorithm
 *
 * @param[in]      pKCe               KernelCE pointer
 * @param[in]      pGpu               OBJGPU pointer
 * @param[in]      gpuMask            GPU mask
 * @param[out]     pNvlinkP2PCeMask   Mask of LCEs marked for P2P
 */
NV_STATUS
kceGetP2PCes_GB100
(
    KernelCE *pKCe,
    OBJGPU   *pGpu,
    NvU32     gpuMask,
    NvU32    *pNvlinkP2PCeMask
)
{
    NvU32          gpuCount       = gpumgrGetSubDeviceCount(gpuMask);
    KernelNvlink  *pKernelNvlink  = GPU_GET_KERNEL_NVLINK(pGpu);
    KernelCE      *pKCeLoop;
    NvU32          remoteGpuMask  = 0;

    if (pKernelNvlink == NULL)
    {
        return NV_WARN_NOTHING_TO_DO;
    }

    //
    // Get the remote GPU mask. In the case of loopback mode, GPU count will be
    // 1. So, current GPU will be the remote GPU as well. If the GPU count is 2,
    // then in the case, figure out the remote GPU mask.
    //
    if (gpuCount == 1)
    {
        remoteGpuMask = gpuMask;
    }
    else
    {
        OBJGPU  *pRemoteGpu     = NULL;
        NvU32    gpuInstance    = 0;

        while ((pRemoteGpu = gpumgrGetNextGpu(gpuMask, &gpuInstance)) != NULL)
        {
            if (pRemoteGpu != pGpu)
            {
                remoteGpuMask = NVBIT32(gpuGetInstance(pRemoteGpu));
                break;
            }
        }
    }

    KCE_ITER_ALL_BEGIN(pGpu, pKCeLoop, 0)
    {
        if (pKCeLoop->bStubbed)
            continue;

        if (kceIsCeNvlinkP2P_HAL(pGpu, pKCeLoop))
        {
            if ((pKCeLoop->nvlinkPeerMask & remoteGpuMask) > 0)
            {
                *pNvlinkP2PCeMask |= NVBIT32(pKCeLoop->publicID);
            }
        }
    }
    KCE_ITER_END

    if (*pNvlinkP2PCeMask == 0)
    {
        return kceGetP2PCes_GH100(pKCe, pGpu, gpuMask, pNvlinkP2PCeMask);
    }

    return NV_OK;
}
