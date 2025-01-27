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
#include "gpu/nvlink/kernel_nvlink.h"
#include "gpu/ce/kernel_ce_private.h"
#include "gpu/bif/kernel_bif.h"

#include "published/blackwell/gb202/dev_ce_base.h"
#include "published/blackwell/gb202/dev_ce.h"

// Defines for PCE-LCE mapping algorithm
#define NV_CE_MAX_HSHUBS                  2
#define NV_CE_LCE_MASK_INIT               0xFFFFFFFF
#define NV_CE_GRCE_ALLOWED_LCE_MASK       0x0F
#define NV_CE_MAX_GRCE                    4
#define NV_CE_EVEN_ASYNC_LCE_MASK         0x55555550
#define NV_CE_ODD_ASYNC_LCE_MASK          0xAAAAAAA0
#define NV_CE_MAX_LCE_MASK                0x3FF
#define NV_CE_PCE_PER_HSHUB               3
#define NV_CE_NUM_PCES_NO_LINK_CASE       2
#define NV_CE_MAX_PCE_PER_GRCE            1

/*!
 * @brief Returns the size of the GRCE_CONFIG register array
 *
 * @param[in] pGpu  OBJGPU pointer
 * @param[in] pKCe   KernelCE pointer
 *
 * @return  NV_CE_BASE_GRCE_CONFIG__SIZE_1
 */
NvU32
kceGetGrceConfigSize1_GB202
(
    KernelCE *pKCe
)
{
    return (pKCe->shimInstance == 0) ? NV_CE_BASE_GRCE_CONFIG__SIZE_1 : 0;
}

/**
 * @brief Returns mask of LCEs that can be assigned for GRCEs
 *        where the index of corresponding set bit indicates the LCE index
 *
 * @param[in]   pGpu                   OBJGPU pointer
 * @param[in]   pKCe                   KernelCE pointer
 *
 * Returns the mask of LCEs valid for GRCEs
 */
NvU32
kceGetGrceSupportedLceMask_GB202
(
    OBJGPU   *pGpu,
    KernelCE *pKCe
)
{
    KernelCE       *pKCeLoop  = NULL;
    RM_ENGINE_TYPE rmCeEngineType;
    NvU32          grceMask  = 0;

    KCE_ITER_BEGIN(pGpu, pKCe, pKCeLoop, 0)
        rmCeEngineType = RM_ENGINE_TYPE_COPY(pKCeLoop->publicID);

        if (ceIsCeGrce(pGpu, rmCeEngineType))
        {
            grceMask |= RM_ENGINE_TYPE_COPY_IDX(rmCeEngineType);
        }
    KCE_ITER_END

    return grceMask;
}

/**
 * @brief Default mapping will match HW init values for async CE mappings
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
kceMapAsyncLceDefault_GB202
(
    OBJGPU   *pGpu,
    KernelCE *pKCe,
    NvU32    *pceAvailableMaskPerHshub,
    NvU32    *pLocalPceLceMap,
    NvU32    *pLocalExposeCeMask,
    NvU32     numDefaultPces
)
{
    NvU32       lceMask = 0;
    NvU32       hshubId = 0;
    NvU32       numPcesPerLce;
    NvU32       numLces;
    NvU32       supportedPceMask;
    NvU32       supportedLceMask;
    NvU32       pcesPerHshub;
    NvU32       lceIndex, pceIndex, i, j;

    kceGetPceConfigForLceType(
        pGpu,
        pKCe,
        NV2080_CTRL_CE_LCE_TYPE_SCRUB,
        &numPcesPerLce,
        &numLces,
        &supportedPceMask,
        &supportedLceMask,
        &pcesPerHshub);

    lceMask = supportedLceMask
              & (NV_CE_EVEN_ASYNC_LCE_MASK | NV_CE_ODD_ASYNC_LCE_MASK);

    //
    // Default mapping will match HW init values where PCEs 0,1 mapped to LCE 4
    // and PCEs 2,3 mapped to LCE 5.
    for (i = 0; i < numLces; i++)
    {
        lceIndex = CE_GET_LOWEST_AVAILABLE_IDX(lceMask);

        for(j = 0; j < numPcesPerLce; j++)
        {
            pceIndex = CE_GET_LOWEST_AVAILABLE_IDX(supportedPceMask);
            pceAvailableMaskPerHshub[hshubId] &= (~(NVBIT32(pceIndex)));

            pLocalPceLceMap[pceIndex] = lceIndex;

            supportedPceMask &= ~NVBIT32(pceIndex);


            NV_PRINTF(LEVEL_INFO, "GPU%d <-> GPU%d PCE Index: %d LCE Index: %d\n",
                      pGpu->gpuInstance, pGpu->gpuInstance, pceIndex, lceIndex);
        }

        lceMask &= ~NVBIT32(lceIndex);
        *pLocalExposeCeMask |= NVBIT32(lceIndex);
    }

    return NV_OK;
}

NV_STATUS
kceGetMappings_GB202
(
    OBJGPU   *pGpu,
    KernelCE *pKCe,
    NVLINK_TOPOLOGY_PARAMS *pTopoParams,
    NvU32    *pLocalPceLceMap,
    NvU32    *pLocalGrceMap,
    NvU32    *pExposeCeMask
)
{
    NV_STATUS status            = NV_WARN_NOTHING_TO_DO;


    // Prepare the per-HSHUB/FBHUB available PCE mask
    kceGetAvailableHubPceMask(pGpu, pKCe, pTopoParams);

    status = kceMapAsyncLceDefault_HAL(pGpu, pKCe,
                                       pTopoParams->pceAvailableMaskPerConnectingHub,
                                       pLocalPceLceMap,
                                       pExposeCeMask,
                                       NV_CE_NUM_PCES_NO_LINK_CASE);

    kceMapPceLceForGRCE_HAL(pGpu, pKCe,
                              pTopoParams->pceAvailableMaskPerConnectingHub,
                              pLocalPceLceMap, pExposeCeMask, pLocalGrceMap, pTopoParams->fbhubPceMask);


    NV_PRINTF(LEVEL_INFO, "status = 0x%x\n", status);

    return status;
}

/**
 * @brief This function assigns PCE-LCE mappings for GRCE LCE 0
 *        Currently this function's goal is to share LCE 5 on GRCE 0.
 *
 * @param[in]   pGpu                        OBJGPU pointer
 * @param[in]   pKCe                         KernelCE pointer
 * @param[in]   pceAvailableMaskPerHshub    Pointer to CEs available per HSHUB
 * @param[out]  pLocalPceLceMap             Pointer to PCE-LCE array
 * @param[out]  pLocalExposeCeMask          Pointer to LCE Mask
 */
void
kceMapPceLceForGRCE_GB202
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
    NvU32         lceIndex         = 0;
    NvU32         grceIdx          = 0;
    NvU32         pceIndex;
    NvU32         size             = kceGetPce2lceConfigSize1_HAL(pKCe);;

    for (pceIndex = 0; pceIndex < size; ++pceIndex)
    {
        if (pLocalPceLceMap[pceIndex] != NV2080_CTRL_CE_UPDATE_PCE_LCE_MAPPINGS_INVALID_LCE
               && (NVBIT32(pLocalPceLceMap[pceIndex]) & NV_CE_ODD_ASYNC_LCE_MASK))
        {
            lceIndex                  = pLocalPceLceMap[pceIndex];
            break;
        }
    }

    pLocalGrceMap[grceIdx] = DRF_NUM(_CE_BASE, _GRCE_CONFIG, _SHARED, 1) |
                             DRF_NUM(_CE_BASE, _GRCE_CONFIG, _SHARED_LCE, lceIndex);

    NV_PRINTF(LEVEL_INFO,
              "GRCE is shared and mapped to LCE Index: %d.\n", lceIndex);
}

/*!
 * @brief Returns the size of the PCE2LCE register array
 *
 *
 * @param[in] pKCe   KernelCE pointer
 *
 * @return  NV_CE_PCE2LCE_CONFIG__SIZE_1
 *
 */
NvU32
kceGetPce2lceConfigSize1_GB202
(
    KernelCE* pKCe
)
{
    return NV_CE_BASE_PCE2LCE_CONFIG__SIZE_1;
}

/**
 * @brief Returns the value of the grce mask register
 *
 * @param[in]   pGpu      OBJGPU pointer
 * @param[in]   pKCe      KernelCE pointer
 * @param[out]  pGrceMask Mask of GRCEs
 */
NV_STATUS
kceGetGrceMaskReg_GB202
(
    OBJGPU   *pGpu,
    KernelCE *pKCe,
    NvU32    *pGrceMask
)
{
    NV_ASSERT_OR_RETURN((pGrceMask != NULL), NV_ERR_INVALID_ARGUMENT); 

    *pGrceMask = DRF_VAL(_CE, _GRCE_MASK, _VALUE,
                         GPU_REG_RD32(pGpu, NV_CE_GRCE_MASK));

    return NV_OK;
}
