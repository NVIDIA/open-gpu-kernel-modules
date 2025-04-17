/*
 * SPDX-FileCopyrightText: Copyright (c) 2023-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#define NVOC_KERNEL_MIG_MANAGER_H_PRIVATE_ACCESS_ALLOWED

#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"

/*!
 * @brief   Function to determine whether gpu instance flags are valid
 *          for this GPU
 *
 * @param[in]   pGpu
 * @param[in]   pKernelMIGManager
 * @param[in]   gpuInstanceFlag       NV2080_CTRL_GPU_PARTITION_FLAG_*
 *
 * @return  Returns NV_TRUE if flags are valid
 */
NvBool
kmigmgrIsGPUInstanceFlagValid_GB202
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    NvU32 gpuInstanceFlag
)
{
    NvU32 memSizeFlag = DRF_VAL(2080_CTRL_GPU, _PARTITION_FLAG,
                                _MEMORY_SIZE, gpuInstanceFlag);
    NvU32 computeSizeFlag = DRF_VAL(2080_CTRL_GPU, _PARTITION_FLAG,
                                    _COMPUTE_SIZE, gpuInstanceFlag);
    NvU32 gfxSizeFlag = DRF_VAL(2080_CTRL_GPU, _PARTITION_FLAG,
                                    _GFX_SIZE, gpuInstanceFlag);

    // If incorrect all video flag, then fail
    if (!(FLD_TEST_REF(NV2080_CTRL_GPU_PARTITION_FLAG_REQ_ALL_MEDIA, _DEFAULT, gpuInstanceFlag) ||
        FLD_TEST_REF(NV2080_CTRL_GPU_PARTITION_FLAG_REQ_ALL_MEDIA, _ENABLE, gpuInstanceFlag) ||
        FLD_TEST_REF(NV2080_CTRL_GPU_PARTITION_FLAG_REQ_ALL_MEDIA, _DISABLE, gpuInstanceFlag)))
    {
        return NV_FALSE;
    }

    switch (memSizeFlag)
    {
        case NV2080_CTRL_GPU_PARTITION_FLAG_MEMORY_SIZE_FULL:
        case NV2080_CTRL_GPU_PARTITION_FLAG_MEMORY_SIZE_HALF:
        case NV2080_CTRL_GPU_PARTITION_FLAG_MEMORY_SIZE_QUARTER:
            break;
        case NV2080_CTRL_GPU_PARTITION_FLAG_MEMORY_SIZE_EIGHTH:
            return NV_FALSE;
        default:
            NV_PRINTF(LEVEL_ERROR, "Unrecognized GPU mem partitioning flag 0x%x\n",
                      memSizeFlag);
            return NV_FALSE;
    }

    switch (computeSizeFlag)
    {
        case NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_FULL:
        case NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_HALF:
        case NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_MINI_HALF:
        case NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_QUARTER:
            break;
        case NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_MINI_QUARTER:
        case NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_EIGHTH:
        case NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_RESERVED_INTERNAL_06:
        case NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_RESERVED_INTERNAL_07:
            return NV_FALSE;
        default:
            NV_PRINTF(LEVEL_ERROR, "Unrecognized GPU compute partitioning flag 0x%x\n",
                      computeSizeFlag);
            return NV_FALSE;
    }

    switch (gfxSizeFlag)
    {
        case NV2080_CTRL_GPU_PARTITION_FLAG_GFX_SIZE_FULL:
        case NV2080_CTRL_GPU_PARTITION_FLAG_GFX_SIZE_HALF:
        case NV2080_CTRL_GPU_PARTITION_FLAG_GFX_SIZE_MINI_HALF:
        case NV2080_CTRL_GPU_PARTITION_FLAG_GFX_SIZE_QUARTER:
            if (!pKernelMIGManager->bIsSmgEnabled)
            {
                return NV_FALSE;
            }
            break;
        // TODO: Move _NONE should not be supported on GB202 because we always expect SMG to enable it.
        case NV2080_CTRL_GPU_PARTITION_FLAG_GFX_SIZE_NONE:
            break;
        case NV2080_CTRL_GPU_PARTITION_FLAG_GFX_SIZE_EIGHTH:
        case NV2080_CTRL_GPU_PARTITION_FLAG_GFX_SIZE_RESERVED_INTERNAL_06:
        case NV2080_CTRL_GPU_PARTITION_FLAG_GFX_SIZE_RESERVED_INTERNAL_07:
            return NV_FALSE;
        default:
            NV_PRINTF(LEVEL_ERROR, "Unrecognized GPU GFX partitioning flag 0x%x\n",
                      gfxSizeFlag);
            return NV_FALSE;
    }

    return NV_TRUE;
}

/*!
 * @brief   Function to determine whether gpu instance flag combinations are valid
 *          for this GPU
 */
NvBool
kmigmgrIsGPUInstanceCombinationValid_GB202
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    NvU32 gpuInstanceFlag
)
{
    NvU32 memSizeFlag = DRF_VAL(2080_CTRL_GPU, _PARTITION_FLAG, _MEMORY_SIZE, gpuInstanceFlag);
    NvU32 computeSizeFlag = DRF_VAL(2080_CTRL_GPU, _PARTITION_FLAG, _COMPUTE_SIZE, gpuInstanceFlag);
    NvU32 gfxSizeFlag = DRF_VAL(2080_CTRL_GPU, _PARTITION_FLAG, _GFX_SIZE, gpuInstanceFlag);
    NvU32 smallestComputeSizeFlag;

    if (!kmigmgrIsGPUInstanceFlagValid_HAL(pGpu, pKernelMIGManager, gpuInstanceFlag))
    {
        return NV_FALSE;
    }

    smallestComputeSizeFlag = kmigmgrSmallestComputeProfileSize(pGpu, pKernelMIGManager);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, smallestComputeSizeFlag != KMIGMGR_COMPUTE_SIZE_INVALID, NV_FALSE);

    // JPG_OFA profile is only available on the smallest available partition
    if (FLD_TEST_REF(NV2080_CTRL_GPU_PARTITION_FLAG_REQ_DEC_JPG_OFA, _ENABLE, gpuInstanceFlag))
    {
        if (computeSizeFlag != smallestComputeSizeFlag)
        {
            return NV_FALSE;
        }

        if (!FLD_TEST_REF(NV2080_CTRL_GPU_PARTITION_FLAG_REQ_ALL_MEDIA, _DEFAULT, gpuInstanceFlag))
        {
            return NV_FALSE;
        }
    }

    switch (computeSizeFlag)
    {
        case NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_FULL:
            NV_CHECK_OR_RETURN(LEVEL_SILENT, memSizeFlag == NV2080_CTRL_GPU_PARTITION_FLAG_MEMORY_SIZE_FULL,
                               NV_FALSE);
            NV_CHECK_OR_RETURN(LEVEL_SILENT, FLD_TEST_REF(NV2080_CTRL_GPU_PARTITION_FLAG_REQ_ALL_MEDIA, _DEFAULT,
                               gpuInstanceFlag), NV_FALSE);
            break;
        case NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_HALF:
            NV_CHECK_OR_RETURN(LEVEL_SILENT, memSizeFlag == NV2080_CTRL_GPU_PARTITION_FLAG_MEMORY_SIZE_HALF,
                               NV_FALSE);
            break;
        case NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_MINI_HALF:
            NV_CHECK_OR_RETURN(LEVEL_SILENT, memSizeFlag == NV2080_CTRL_GPU_PARTITION_FLAG_MEMORY_SIZE_HALF,
                               NV_FALSE);
            break;
        case NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_QUARTER:
            NV_CHECK_OR_RETURN(LEVEL_SILENT, memSizeFlag == NV2080_CTRL_GPU_PARTITION_FLAG_MEMORY_SIZE_QUARTER,
                               NV_FALSE);
            break;
        case NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_MINI_QUARTER:
            NV_CHECK_OR_RETURN(LEVEL_SILENT, memSizeFlag == NV2080_CTRL_GPU_PARTITION_FLAG_MEMORY_SIZE_QUARTER,
                               NV_FALSE);
            break;
        default:
            NV_ASSERT_OR_RETURN(0, NV_FALSE);
    }

    switch (gfxSizeFlag)
    {
        case NV2080_CTRL_GPU_PARTITION_FLAG_GFX_SIZE_FULL:
            NV_CHECK_OR_RETURN(LEVEL_SILENT, computeSizeFlag == NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_FULL,
                               NV_FALSE);
            break;
        case NV2080_CTRL_GPU_PARTITION_FLAG_GFX_SIZE_HALF:
            NV_CHECK_OR_RETURN(LEVEL_SILENT, computeSizeFlag == NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_HALF,
                               NV_FALSE);
            break;
        case NV2080_CTRL_GPU_PARTITION_FLAG_GFX_SIZE_MINI_HALF:
            NV_CHECK_OR_RETURN(LEVEL_SILENT, computeSizeFlag == NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_MINI_HALF,
                               NV_FALSE);
            break;
        case NV2080_CTRL_GPU_PARTITION_FLAG_GFX_SIZE_QUARTER:
            NV_CHECK_OR_RETURN(LEVEL_SILENT, computeSizeFlag == NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_QUARTER,
                               NV_FALSE);
            break;
        case NV2080_CTRL_GPU_PARTITION_FLAG_GFX_SIZE_NONE:
            //
            // Allow this for internal testing, but not for production
            // TODO: Disable this for production
            //
            break;
        default:
            NV_ASSERT_OR_RETURN(0, NV_FALSE);
    }

    return NV_TRUE;
}

/*!
 * @brief   Function to get the next computeSize flag either larger or smaller than
 *          the passed in flag.
 *
 * @param[IN]     bGetNextSmallest   Flag controlling whether the next largest or smallest
 *                                   compute size is returned
 * @param[IN]     computeSize        Base computeSize to lookup
 *
 * @return        Input is the original compute size
 *                  a.) If compute size input is KMIGMGR_COMPUTE_SIZE_INVALID, out is:
 *                     1.) NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_FULL if bGetNextSmallest
 *                     2.) NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_EIGHTH if !bGetNextSmallest
 *                  b.) Else output is next largest/smallest based upon bGetNextSmallest
 */
NvU32
kmigmgrGetNextComputeSize_GB202
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    NvBool bGetNextSmallest,
    NvU32 computeSize
)
{
    const NvU32 computeSizeFlags[] =
    {
        KMIGMGR_COMPUTE_SIZE_INVALID,
        NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_FULL,
        NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_HALF,
        NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_MINI_HALF,
        NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_QUARTER,
        NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_MINI_QUARTER,
        NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_EIGHTH,
        KMIGMGR_COMPUTE_SIZE_INVALID
    };

    NV_ASSERT_OR_RETURN(computeSize <= KMIGMGR_COMPUTE_SIZE_INVALID, KMIGMGR_COMPUTE_SIZE_INVALID);

    if (computeSize == KMIGMGR_COMPUTE_SIZE_INVALID)
    {
        return (bGetNextSmallest) ? computeSizeFlags[1] : computeSizeFlags[NV_ARRAY_ELEMENTS(computeSizeFlags) - 2];
    }
    else
    {
        NvU32 i;

        for (i = 1; i < NV_ARRAY_ELEMENTS(computeSizeFlags) - 1; i++)
            if (computeSizeFlags[i] == computeSize)
                return (bGetNextSmallest) ? computeSizeFlags[i + 1] : computeSizeFlags[i - 1];

        // Requested input flag was not found
        return KMIGMGR_COMPUTE_SIZE_INVALID;
    }
}

