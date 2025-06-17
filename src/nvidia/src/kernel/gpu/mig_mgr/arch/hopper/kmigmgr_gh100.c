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

#define NVOC_KERNEL_MIG_MANAGER_H_PRIVATE_ACCESS_ALLOWED

#include "kernel/gpu/mem_mgr/mem_mgr.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "kernel/gpu/fifo/kernel_fifo.h"

/*!
 * @brief   Function to determine whether gpu instance flags are valid
 *          for this GPU
 *
 * @param[IN]   pGpu
 * @param[IN]   pKernelMIGManager
 * @param[IN]   gpuInstanceFlag       NV2080_CTRL_GPU_PARTITION_FLAG_*
 *
 * @return  Returns true if flags are valid
 */
NvBool
kmigmgrIsGPUInstanceFlagValid_GH100
(
    OBJGPU *pGpu,
    KernelMIGManager *pGrMgr,
    NvU32 gpuInstanceFlag
)
{
    NvU32 memSizeFlag = DRF_VAL(2080_CTRL_GPU, _PARTITION_FLAG,
                                _MEMORY_SIZE, gpuInstanceFlag);
    NvU32 computeSizeFlag = DRF_VAL(2080_CTRL_GPU, _PARTITION_FLAG,
                                    _COMPUTE_SIZE, gpuInstanceFlag);
    NvU32 gfxSizeFlag = DRF_VAL(2080_CTRL_GPU, _PARTITION_FLAG,
                                    _GFX_SIZE, gpuInstanceFlag);

    if (!FLD_TEST_REF(NV2080_CTRL_GPU_PARTITION_FLAG_REQ_ALL_MEDIA, _DEFAULT, gpuInstanceFlag))
    {
        return NV_FALSE;
    }

    switch (memSizeFlag)
    {
        case NV2080_CTRL_GPU_PARTITION_FLAG_MEMORY_SIZE_FULL:
        case NV2080_CTRL_GPU_PARTITION_FLAG_MEMORY_SIZE_HALF:
        case NV2080_CTRL_GPU_PARTITION_FLAG_MEMORY_SIZE_QUARTER:
        case NV2080_CTRL_GPU_PARTITION_FLAG_MEMORY_SIZE_EIGHTH:
            break;
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
        case NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_MINI_QUARTER:
        case NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_EIGHTH:
            break;
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
        case NV2080_CTRL_GPU_PARTITION_FLAG_GFX_SIZE_NONE:
            break;
        case NV2080_CTRL_GPU_PARTITION_FLAG_GFX_SIZE_FULL:
        case NV2080_CTRL_GPU_PARTITION_FLAG_GFX_SIZE_HALF:
        case NV2080_CTRL_GPU_PARTITION_FLAG_GFX_SIZE_MINI_HALF:
        case NV2080_CTRL_GPU_PARTITION_FLAG_GFX_SIZE_QUARTER:
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
kmigmgrIsGPUInstanceCombinationValid_GH100
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    NvU32 gpuInstanceFlag
)
{
    NvU32 memSizeFlag = DRF_VAL(2080_CTRL_GPU, _PARTITION_FLAG, _MEMORY_SIZE, gpuInstanceFlag);
    NvU32 computeSizeFlag = DRF_VAL(2080_CTRL_GPU, _PARTITION_FLAG, _COMPUTE_SIZE, gpuInstanceFlag);
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
    }

    switch (computeSizeFlag)
    {
        case NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_FULL:
            NV_CHECK_OR_RETURN(LEVEL_SILENT, memSizeFlag == NV2080_CTRL_GPU_PARTITION_FLAG_MEMORY_SIZE_FULL,
                               NV_FALSE);
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
        case NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_EIGHTH:
            NV_CHECK_OR_RETURN(LEVEL_SILENT, memSizeFlag == NV2080_CTRL_GPU_PARTITION_FLAG_MEMORY_SIZE_EIGHTH,
                               NV_FALSE);
            break;
        default:
            NV_ASSERT(0);
            return NV_FALSE;
    }
    return NV_TRUE;
}
