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

#include "gpu/gpu.h"
#include "gpu/ce/kernel_ce.h"
#include "gpu/ce/kernel_ce_private.h"

#include "published/blackwell/gb10b/dev_ce.h"
#include "published/blackwell/gb10b/dev_ce_base.h"

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
kceGetPce2lceConfigSize1_GB10B
(
    KernelCE *pKCe
)
{
    return NV_CE_PCE2LCE_CONFIG__SIZE_1;
}

/**
 * @brief This mapping will ensure that we have LCEs 2,3 available
 *        provided we have enough PCEs in HW to support the config
 *
 * @param[in]   pGpu                        OBJGPU pointer
 * @param[in]   pKCe                        KernelCE pointer
 * @param[in]   pceAvailableMaskPerHshub    Pointer to CEs available per HSHUB
 * @param[out]  pLocalPceLceMap             Pointer to PCE-LCE array
 * @param[out]  pLocalExposeCeMask          Pointer to LCE Mask
 *
 * Returns NV_OK if successful in assigning PCEs to LCEs 2 and 3
 */
NV_STATUS
kceMapAsyncLceDefault_GB10B
(
    OBJGPU   *pGpu,
    KernelCE *pKCe,
    NvU32    *pceAvailableMaskPerHshub,
    NvU32    *pLocalPceLceMap,
    NvU32    *pLocalExposeCeMask,
    NvU32     numDefaultPces
)
{
    NvU32   hshubId = 0;
    NvU32   numPcesPerLce = 0;
    NvU32   numLces = 0;
    NvU32   supportedPceMask = 0;
    NvU32   supportedLceMask = 0;
    NvU32   pcesPerHshub = 0;
    NvU32   maxLceCount = 0;
    NvU32   maxPceCount = 0;
    NvU32   lceIndex, pceIndex, i, j;

    kceGetPceConfigForLceType(
        pGpu,
        pKCe,
        NV2080_CTRL_CE_LCE_TYPE_PCIE,
        &numPcesPerLce,
        &numLces,
        &supportedPceMask,
        &supportedLceMask,
        &pcesPerHshub);

    // To catch error scenario with lceIndex
    maxLceCount = supportedLceMask;
    HIGHESTBITIDX_32(maxLceCount); // HIGHESTBITIDX_32 is destructive
    maxLceCount++;

    // To catch error scenario with pceIndex
    maxPceCount = supportedPceMask;
    HIGHESTBITIDX_32(maxPceCount); // HIGHESTBITIDX_32 is destructive
    maxPceCount++;

    //
    // Assign 1 PCE per LCE
    //
    for (i = 0; i < numLces; i++)
    {
        lceIndex = CE_GET_LOWEST_AVAILABLE_IDX(supportedLceMask);

        if (lceIndex >= maxLceCount)
            break;

        for(j = 0; j < numPcesPerLce; j++)
        {
            pceIndex = CE_GET_LOWEST_AVAILABLE_IDX(supportedPceMask);

            if (pceIndex >= maxPceCount)
                break;

            pLocalPceLceMap[pceIndex] = lceIndex;

            supportedPceMask &= ~NVBIT32(pceIndex);
            pceAvailableMaskPerHshub[hshubId] &= (~(NVBIT32(pceIndex)));

            NV_PRINTF(LEVEL_INFO, "GPU%d PCE Index: %d LCE Index: %d\n",
                      pGpu->gpuInstance, pceIndex, lceIndex);
        }

        supportedLceMask &= ~NVBIT32(lceIndex);
        *pLocalExposeCeMask |= NVBIT32(lceIndex);
    }

    return NV_OK;
}

/**
 * @brief This function assigns PCE-LCE mappings for GRCE LCE 0
 *        Currently this function's goal is to share LCE 3 on GRCE 0
 *
 * @param[in]   pGpu                        OBJGPU pointer
 * @param[in]   pKCe                         KernelCE pointer
 * @param[in]   pceAvailableMaskPerHshub    Pointer to CEs available per HSHUB
 * @param[out]  pLocalPceLceMap             Pointer to PCE-LCE array
 * @param[out]  pLocalExposeCeMask          Pointer to LCE Mask
 */
void
kceMapPceLceForGRCE_GB10B
(
    OBJGPU   *pGpu,
    KernelCE *pKCe,
    NvU32    *pceAvailableMaskPerHshub,
    NvU32    *pLocalPceLceMap,
    NvU32    *pLocalExposeCeMask,
    NvU32    *pLocalGrceMap,
    NvU32     fbPceMask
)
{
    NvU32       grceIdx   = 0;
    NvU32       pceIndex  = 0;
    NvU32       numPcesPerLce = 0;
    NvU32       numLces = 0;
    NvU32       supportedPceMask = 0;
    NvU32       supportedLceMask = 0;
    NvU32       pcesPerHshub = 0;
    NvU32       maxPceCount = 0;

    kceGetPceConfigForLceType(
        pGpu,
        pKCe,
        NV2080_CTRL_CE_LCE_TYPE_PCIE,
        &numPcesPerLce,
        &numLces,
        &supportedPceMask,
        &supportedLceMask,
        &pcesPerHshub);

    // To catch error scenario with pceIndex
    maxPceCount = supportedPceMask;
    HIGHESTBITIDX_32(maxPceCount); // HIGHESTBITIDX_32 is destructive
    maxPceCount++;

    pceIndex = CE_GET_LOWEST_AVAILABLE_IDX(supportedPceMask);

    if (pceIndex >= maxPceCount)
        return;

    if (pLocalPceLceMap[pceIndex] != NV2080_CTRL_CE_UPDATE_PCE_LCE_MAPPINGS_INVALID_LCE)
    {
        pLocalGrceMap[grceIdx] = DRF_NUM(_CE_BASE, _GRCE_CONFIG, _SHARED, 1) |
                                 DRF_NUM(_CE_BASE, _GRCE_CONFIG, _SHARED_LCE, pLocalPceLceMap[pceIndex]);
        NV_PRINTF(LEVEL_INFO,
                  "GRCE %d is shared and mapped to LCE Index: %d.\n", grceIdx, pLocalPceLceMap[pceIndex]);
    }
}
