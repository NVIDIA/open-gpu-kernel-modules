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
#include "kernel/gpu/mem_mgr/heap.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "kernel/gpu/fifo/kernel_fifo.h"

/*!
 * @brief   Peforms checks to determine whether instancing can be enabled on
 *          this GPU, such as determining whether any partitionable engines are
 *          currently active.
 */
NV_STATUS
kmigmgrCreateGPUInstanceCheck_GB10B
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    NvBool bMemoryPartitioningNeeded
)
{
    Heap *pHeap;
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
    RM_ENGINE_TYPE engines[RM_ENGINE_TYPE_LAST];
    NvU32 engineCount;
    NvU32 i;
    NvU64 largestFreeSize;
    NvU64 base;
    NvU64 unused;
    NV_RANGE freeBlock;
    NV_RANGE partitionableMemoryRange = memmgrGetMIGPartitionableMemoryRange(pGpu, pMemoryManager);

    // Ensure the engine DB is up-to-date
    NV_ASSERT_OK_OR_RETURN(gpuUpdateEngineTable(pGpu));

    // Store all engine tags of partitionable engines in the system
    engineCount = 0;
    for (i = 0; i < pGpu->engineDB.size; ++i)
    {
        if (kmigmgrIsEnginePartitionable(pGpu, pKernelMIGManager, pGpu->engineDB.pType[i]))
        {
            if (RM_ENGINE_TYPE_IS_GR(pGpu->engineDB.pType[i]) &&
                (RM_ENGINE_TYPE_GR_IDX(pGpu->engineDB.pType[i]) > 0))
            {
                //
                // This check is used during instance creation, prior to which
                // it is impossible to use GR1-7, so as an optimization, skip
                // checking for those.
                //
                continue;
            }

            engines[engineCount++] = pGpu->engineDB.pType[i];
        }
    }

    // Make sure there are no channels alive on any of these engines
    if (kfifoEngineListHasChannel(pGpu, pKernelFifo, engines, engineCount))
        return NV_ERR_STATE_IN_USE;

    pHeap = GPU_GET_HEAP(pGpu);
    if (!memmgrIsPmaInitialized(pMemoryManager))
    {
        NV_ASSERT_OK_OR_RETURN(
            heapInfo(pHeap, &unused, &unused, &unused, &base, &largestFreeSize));
    }
    else
    {
        pmaGetLargestFree(&pHeap->pmaObject, &largestFreeSize, &base, &unused);
    }

    // Make sure that no memory has been claimed from our partitionable range
    freeBlock = rangeMake(base, base + largestFreeSize - 1);
    if (!rangeContains(freeBlock, partitionableMemoryRange))
        return NV_ERR_STATE_IN_USE;

    return NV_OK;
}

/*!
 * @brief   Function to determine whether partition flag combinations are valid
 *          for this GPU
 */
NvBool
kmigmgrIsGPUInstanceCombinationValid_GB10B
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    NvU32 gpuInstanceFlag
)
{
    if (!kmigmgrIsGPUInstanceFlagValid_HAL(pGpu, pKernelMIGManager, gpuInstanceFlag))
    {
        return NV_FALSE;
    }

    return NV_TRUE;
}

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
kmigmgrIsGPUInstanceFlagValid_GB10B
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

    if (!FLD_TEST_REF(NV2080_CTRL_GPU_PARTITION_FLAG_REQ_ALL_MEDIA, _DEFAULT, gpuInstanceFlag))
    {
        return NV_FALSE;
    }

    switch (memSizeFlag)
    {
        case NV2080_CTRL_GPU_PARTITION_FLAG_MEMORY_SIZE_FULL:
            break;
        default:
            NV_PRINTF(LEVEL_INFO, "Unrecognized GPU mem partitioning flag 0x%x\n",
                      memSizeFlag);
            return NV_FALSE;
    }

    switch (computeSizeFlag)
    {
        case NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_FULL:
        case NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_HALF:
        case NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_MINI_HALF:
            break;
        default:
            NV_PRINTF(LEVEL_INFO, "Unrecognized GPU compute partitioning flag 0x%x\n",
                      computeSizeFlag);
            return NV_FALSE;
    }

    switch (gfxSizeFlag)
    {
        case NV2080_CTRL_GPU_PARTITION_FLAG_GFX_SIZE_FULL:
            if (!pKernelMIGManager->bIsSmgEnabled)
            {
                return NV_FALSE;
            }
            break;
        case NV2080_CTRL_GPU_PARTITION_FLAG_GFX_SIZE_NONE:
            break;
        // Cannot support these sizes since there is only one GFX Capable SYSPIPE
        case NV2080_CTRL_GPU_PARTITION_FLAG_GFX_SIZE_HALF:
        case NV2080_CTRL_GPU_PARTITION_FLAG_GFX_SIZE_MINI_HALF:
        case NV2080_CTRL_GPU_PARTITION_FLAG_GFX_SIZE_QUARTER:
        case NV2080_CTRL_GPU_PARTITION_FLAG_GFX_SIZE_EIGHTH:
        default:
            NV_PRINTF(LEVEL_INFO, "Unrecognized GPU GFX partitioning flag 0x%x\n",
                      gfxSizeFlag);
            return NV_FALSE;
    }

    return NV_TRUE;
}

/*!
 * @brief This function is introduced because GB10B has uneven Static GPCs for
 * HALF partitions(1 GPC(MIG 1):2 GPC(MIG 2)). With the current logic of the function
 * kmigmgrGetComputeProfileFromGpcCount_IMPL, a request for GPC count 2 will return a
 * FULL compute profile based on maxGPC calculation(3/2 == 1). For GB10B, If GPC count
 * is 3  a FULL profile is returned, for a GPC count of 2, HALF profile is returned and
 * for GPC count 1, a MINI_HALF profile is returned.
 *
 * @param[IN]   pGpu
 * @param[IN]   pKernelMIGManager
 * @param[IN]   gpcCount                GPC Count to look up the associated compute profile
 * @param[OUT]  pProfile                Pointer to  NV2080_CTRL_INTERNAL_MIGMGR_COMPUTE_PROFILE struct filled with
 *                                      a copy of the compute profile info associated with the GPC count
 */
NV_STATUS
kmigmgrGetComputeProfileFromGpcCount_GB10B
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    NvU32 gpcCount,
    NV2080_CTRL_INTERNAL_MIGMGR_COMPUTE_PROFILE *pProfile
)
{
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
    const KERNEL_MIG_MANAGER_STATIC_INFO *pStaticInfo = kmigmgrGetStaticInfo(pGpu, pKernelMIGManager);
    NvU32 compSize = NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE__SIZE;
    NvU32 maxGpc;
    NvU32 i;
    NvU32 actualMaxGpc = 0;

    NV_ASSERT_OR_RETURN(pProfile != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pStaticInfo != NULL, NV_ERR_OBJECT_NOT_FOUND);
    NV_CHECK_OR_RETURN(LEVEL_WARNING, pStaticInfo->pCIProfiles != NULL, NV_ERR_OBJECT_NOT_FOUND);

    maxGpc = kgrmgrGetLegacyKGraphicsStaticInfo(pGpu, pKernelGraphicsManager)->pGrInfo->infoList[NV2080_CTRL_GR_INFO_INDEX_MAX_PARTITIONABLE_GPCS].data;
    actualMaxGpc = maxGpc;

    for (i = 0; i < pStaticInfo->pCIProfiles->profileCount; i++)
    {
        if (pStaticInfo->pCIProfiles->profiles[i].computeSize == NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_FULL)
        {
            actualMaxGpc = pStaticInfo->pCIProfiles->profiles[i].gpcCount;
        }
    }

    if (gpcCount == 1)
    {
        compSize = NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_MINI_HALF;
    }
    else if ((gpcCount == ((maxGpc / 2) + 1)) && (actualMaxGpc == maxGpc))
    {
        compSize = NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_HALF;
    }
    else if (gpcCount == actualMaxGpc)
    {
        compSize = NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_FULL;
    }

    for (i = 0; i < pStaticInfo->pCIProfiles->profileCount; i++)
    {
        if (pStaticInfo->pCIProfiles->profiles[i].computeSize == compSize)
        {
            NV_PRINTF(LEVEL_INFO, "Found matching Compute Profile:%d for gpcCount=%d\n", compSize, gpcCount);
            portMemCopy(pProfile, sizeof(*pProfile), &pStaticInfo->pCIProfiles->profiles[i], sizeof(pStaticInfo->pCIProfiles->profiles[i]));
            return NV_OK;
        }
    }

    NV_PRINTF(LEVEL_INFO, "Found no Compute Profile for gpcCount=%d\n", gpcCount);
    return NV_ERR_OBJECT_NOT_FOUND;
}

/*!
 * @brief   Returns the span covered by the swizzId
 */
NV_RANGE
kmigmgrSwizzIdToSpan_GB10B
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    NvU32 swizzId
)
{
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
    NV_RANGE ret;
    NvU8 spanLen;

    NV_ASSERT_OR_RETURN(kgrmgrGetLegacyKGraphicsStaticInfo(pGpu, pKernelGraphicsManager)->bInitialized, NV_RANGE_EMPTY);
    NV_ASSERT_OR_RETURN(kgrmgrGetLegacyKGraphicsStaticInfo(pGpu, pKernelGraphicsManager)->pGrInfo != NULL, NV_RANGE_EMPTY);

    spanLen = kgrmgrGetLegacyKGraphicsStaticInfo(pGpu, pKernelGraphicsManager)->pGrInfo->infoList[NV2080_CTRL_GR_INFO_INDEX_MAX_PARTITIONABLE_GPCS].data;

    switch (swizzId)
    {
        case 0:
            ret = rangeMake(0, spanLen - 1);
            break;
        case 1:
            ret = rangeMake(0, (spanLen/2) - 1);
            break;
        case 2:
            ret = rangeMake(spanLen/2, spanLen - 1);
            break;
        default:
            NV_PRINTF(LEVEL_ERROR, "Unsupported swizzid 0x%x\n", swizzId);
            ret = NV_RANGE_EMPTY;
            break;
    }

    return ret;
}
