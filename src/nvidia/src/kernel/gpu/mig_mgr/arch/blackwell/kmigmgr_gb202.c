/*
 * SPDX-FileCopyrightText: Copyright (c) 2023-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

    NV_CHECK_OR_RETURN(LEVEL_ERROR,
        kmigmgrIsGPUInstanceFlagLegal(pGpu, pKernelMIGManager, gpuInstanceFlag),
        NV_FALSE);

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
        default:
            return NV_FALSE;
    }

    switch (computeSizeFlag)
    {
        case NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_FULL:
        case NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_HALF:
        case NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_MINI_HALF:
        case NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_QUARTER:
            break;
        default:
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
        case NV2080_CTRL_GPU_PARTITION_FLAG_GFX_SIZE_NONE:
            break;
        default:
            return NV_FALSE;
    }

    return NV_TRUE;
}
/*!
 * @brief   Function to determine whether gpu instance can support vgpu
 *          timeslice mode
 *
 * @param[in]   pKernelMIGManager
 * @param[in]   gpuInstanceFlag       NV2080_CTRL_GPU_PARTITION_FLAG_*
 *
 * @return  Returns NV_TRUE if gpu instance can support vgpu timeslice mode
 */

NvBool
kmigmgrGpuInstanceSupportVgpuTimeslice_GB202
(
    KernelMIGManager *pKernelMIGManager,
    NvU32 gpuInstanceFlag
)
{
    NvU32 gfxSizeFlag = DRF_VAL(2080_CTRL_GPU, _PARTITION_FLAG, _GFX_SIZE, gpuInstanceFlag);

    return gfxSizeFlag == NV2080_CTRL_GPU_PARTITION_FLAG_GFX_SIZE_NONE ? NV_FALSE : NV_TRUE;
}

static NvBool
s_kmigmgrIsSingleSliceConfig_GB202
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    NvU32 gpuInstanceFlag
)
{
    NvU32 computeSizeFlag = DRF_VAL(2080_CTRL_GPU, _PARTITION_FLAG, _COMPUTE_SIZE, gpuInstanceFlag);
    NvU32 syspipeMask = 0;
    NvBool isSingleSliceProfile = NV_FALSE;
    NvU32 actualMigCount = 0;
    NvU32 i;

    for (i = 0; i < RM_ENGINE_TYPE_GR_SIZE; ++i)
    {
        if (gpuCheckEngine_HAL(pGpu, ENG_GR(i)))
        {
            syspipeMask |= NVBIT32(i);
        }
    }
    actualMigCount = nvPopCount32(syspipeMask);

    switch (computeSizeFlag)
    {
        case NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_MINI_HALF:
            if (actualMigCount == 2)
            {
                //
                // On 2 slice configurations, MINI_HALF is the smallest available partition
                // QUARTER would be hidden by NVML See bug 5592609 for more details.
                //
                isSingleSliceProfile = NV_TRUE;
            }
            break;
        case NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_QUARTER:
            isSingleSliceProfile = NV_TRUE;
            break;
        default:
            // nothing do do. default value is already initialized to NV_FALSE
            break;
    }

    return isSingleSliceProfile;
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

    if (!kmigmgrIsGPUInstanceFlagValid_HAL(pGpu, pKernelMIGManager, gpuInstanceFlag))
    {
        return NV_FALSE;
    }


    // JPG_OFA profile is only available on single slice GPU Instances
    if (FLD_TEST_REF(NV2080_CTRL_GPU_PARTITION_FLAG_REQ_DEC_JPG_OFA, _ENABLE, gpuInstanceFlag))
    {
        if (!s_kmigmgrIsSingleSliceConfig_GB202(pGpu, pKernelMIGManager, gpuInstanceFlag))
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
