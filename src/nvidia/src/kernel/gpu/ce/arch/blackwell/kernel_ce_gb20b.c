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

#include "published/blackwell/gb20b/dev_ce_base.h"

/*!
 * @brief Returns the size of the GRCE_CONFIG register array
 *
 * @param[in] pGpu  OBJGPU pointer
 * @param[in] pKCe   KernelCE pointer
 *
 * @return  NV_CE_GRCE_CONFIG__SIZE_1
 */
NvU32
kceGetGrceConfigSize1_GB20B
(
    KernelCE *pKCe
)
{
    return (pKCe->shimInstance == 0) ? NV_CE_BASE_GRCE_CONFIG__SIZE_1 : 0;
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
NvU32
kceGetPce2lceConfigSize1_GB20B
(
    KernelCE *pKCe
)
{
    return NV_CE_BASE_PCE2LCE_CONFIG__SIZE_1;
}

/**
 * @brief This function assigns PCE-LCE mappings for GRCE LCE 0
 *        Currently this function's goal is to share LCE 1 on GRCE 0
 *
 * @param[in]   pGpu                        OBJGPU pointer
 * @param[in]   pKCe                         KernelCE pointer
 * @param[in]   pceAvailableMaskPerHshub    Pointer to CEs available per HSHUB
 * @param[out]  pLocalPceLceMap             Pointer to PCE-LCE array
 * @param[out]  pLocalExposeCeMask          Pointer to LCE Mask
 */
void
kceMapPceLceForGRCE_GB20B
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

    pLocalGrceMap[grceIdx] = DRF_NUM(_CE_BASE, _GRCE_CONFIG, _SHARED, 0) |
                                DRF_DEF(_CE_BASE, _GRCE_CONFIG, _SHARED_LCE, _NONE);
    NV_PRINTF(LEVEL_INFO, "GRCE %d is not shared\n", grceIdx);
}

/**
 * @brief This mapping will assign PCE 0 to LCE 1 and PCE 1 to LCE 0
 *
 * @param[in]   pGpu                        OBJGPU pointer
 * @param[in]   pKCe                        KernelCE pointer
 * @param[in]   pceAvailableMaskPerHshub    Pointer to CEs available per HSHUB
 * @param[out]  pLocalPceLceMap             Pointer to PCE-LCE array
 * @param[out]  pLocalExposeCeMask          Pointer to LCE Mask
 *
 * Returns NV_OK if successful in assigning PCEs to LCE 1
 */
NV_STATUS
kceMapAsyncLceDefault_GB20B
(
    OBJGPU   *pGpu,
    KernelCE *pKCe,
    NvU32    *pceAvailableMaskPerHshub,
    NvU32    *pLocalPceLceMap,
    NvU32    *pLocalExposeCeMask,
    NvU32     numDefaultPces
)
{
    NvU32   lceIndex, pceIndex;

    pceIndex = 0;
    lceIndex = 1;
    pLocalPceLceMap[pceIndex] = lceIndex;
    *pLocalExposeCeMask |= NVBIT32(lceIndex);
    NV_PRINTF(LEVEL_INFO, "GPU%d PCE Index: %d LCE Index: %d\n",
                      pGpu->gpuInstance, pceIndex, lceIndex);

    pceIndex = 1;
    lceIndex = 0;
    pLocalPceLceMap[pceIndex] = lceIndex;
    *pLocalExposeCeMask |= NVBIT32(lceIndex);
    NV_PRINTF(LEVEL_INFO, "GPU%d PCE Index: %d LCE Index: %d\n",
                      pGpu->gpuInstance, pceIndex, lceIndex);

    return NV_OK;
}
