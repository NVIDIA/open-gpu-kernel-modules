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

#include "published/ampere/ga102/dev_ce.h"

#define NV_CE_SYS_ALLOWED_LCE_MASK            0x0C
#define NV_CE_GRCE_ALLOWED_LCE_MASK           0x03
#define NV_CE_EVEN_ASYNC_LCE_MASK             0x00000010
#define NV_CE_ODD_ASYNC_LCE_MASK              0x00000000
#define NV_CE_MAX_LCE_MASK                    0x1F
#define NV_CE_MAX_GRCE                        2

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
kceGetPce2lceConfigSize1_GA102
(
    KernelCE *pKCe
)
{
    return NV_CE_PCE2LCE_CONFIG__SIZE_1;
}

/**
 * @brief This function assigns PCE-LCE mappings for sysmem
 *        for the following two cases -
 *        1. PCIe links - assign FBHUB PCEs
 *        2. NVLinks    - not POR for GA10X chips - not supported
 *        If sysLinkMask is 0, then we assume that sysmem is over PCIe.
 *        Else, follow step 2 as above.
 *
 * @param[in]   pGpu                        OBJGPU pointer
 * @param[in]   pKCe                         KernelCE pointer
 * @param[in]   pceAvailableMaskPerHshub    Pointer to CEs available per HSHUB
 * @param[out]  pLocalPceLceMap             Pointer to PCE-LCE array
 * @param[out]  pLocalExposeCeMask          Pointer to LCE Mask
 *
 * Returns NV_OK if successful in determining PCEs and LCEs for sysmem links
 */
NV_STATUS
kceMapPceLceForSysmemLinks_GA102
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
    NvU32   tempFbPceMask;
    NvU32   lceIndex, pceIndex;
    NV_STATUS status;

    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

    NV2080_CTRL_INTERNAL_NVLINK_HSHUB_GET_SYSMEM_NVLINK_MASK_PARAMS params;

    NV_ASSERT_OR_RETURN(pKernelNvlink != NULL, NV_ERR_NOT_SUPPORTED);
    portMemSet(&params, 0, sizeof(params));

    status = knvlinkExecGspRmRpc(pGpu, pKernelNvlink,
                                 NV2080_CTRL_CMD_INTERNAL_NVLINK_HSHUB_GET_SYSMEM_NVLINK_MASK,
                                 (void *)&params, sizeof(params));
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
    if (params.sysmemLinkMask == 0)
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
    }
    else
    {
        // Print error message, do not assign PCEs and simply return
        NV_PRINTF(LEVEL_ERROR,
                  "Sysmem over NVLink is not POR!\n");
    }

    return NV_OK;
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
kceGetSysmemSupportedLceMask_GA102
(
    OBJGPU *pGpu,
    KernelCE   *pKCe
)
{
    return (NV_CE_SYS_ALLOWED_LCE_MASK & NV_CE_MAX_LCE_MASK);
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
kceGetNvlinkPeerSupportedLceMask_GA102
(
    OBJGPU  *pGpu,
    KernelCE   *pKCe,
    NvU32   peerAvailableLceMask
)
{
    // There is no odd async LCE on GA10X - only LCE 4
    return (NV_CE_EVEN_ASYNC_LCE_MASK & NV_CE_MAX_LCE_MASK);
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
kceGetGrceSupportedLceMask_GA102
(
    OBJGPU *pGpu,
    KernelCE   *pKCe
)
{
    return (NV_CE_GRCE_ALLOWED_LCE_MASK & NV_CE_MAX_LCE_MASK);
}
