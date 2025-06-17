/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/bus/kern_bus.h"

#include "published/ampere/ga100/dev_bus.h"
#include "published/ampere/ga100/dev_bus_addendum.h"

/*!
 * @brief   Checks Devinit owned scratch bit to see if MIG is enabled or not
 *
 * @return  NV_TRUE if scratch bit is set else NV_FALSE
 */
NvBool
kmigmgrIsDevinitMIGBitSet_GA100
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager
)
{
    NvU32 regVal;

    regVal = GPU_REG_RD32(pGpu, NV_PBUS_SW_SCRATCH(1));

    return FLD_TEST_DRF(_PBUS, _SW_SCRATCH1_SMC,_MODE, _ON, regVal);
}

/*!
 * @brief   Peforms checks to determine whether instancing can be enabled on
 *          this GPU, such as determining whether any partitionable engines are
 *          currently active.
 */
NV_STATUS
kmigmgrCreateGPUInstanceCheck_GA100
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    NvBool bMemoryPartitioningNeeded
)
{
    Heap             *pHeap;
    KernelFifo       *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
    RM_ENGINE_TYPE    engines[RM_ENGINE_TYPE_LAST];
    NvU32             engineCount;
    NvU32             i;
    NvU64             largestFreeSize;
    NvU64             base;
    NvU64             unused;
    NV_RANGE          freeBlock;
    MemoryManager    *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    NV_RANGE          partitionableMemoryRange = memmgrGetMIGPartitionableMemoryRange(pGpu, pMemoryManager);

    // Ensure the engine DB is up-to-date
    NV_ASSERT_OK_OR_RETURN(gpuUpdateEngineTable(pGpu));

    // Store all engine tags of partitionable engines in the system
    engineCount = 0;
    for (i = 0; i < pGpu->engineDB.size; ++i)
    {
        if (kmigmgrIsEnginePartitionable(pGpu, pKernelMIGManager, pGpu->engineDB.pType[i]))
        {
            //
            // If memory partitioning isnt needed, scrubber channel will be active, and
            // partitioning isn't really a destructive operation anyway, so
            // skip checking for copy engines
            //
            if (RM_ENGINE_TYPE_IS_COPY(pGpu->engineDB.pType[i]) &&
                !bMemoryPartitioningNeeded)
            {
                continue;
            }

            if (RM_ENGINE_TYPE_IS_GR(pGpu->engineDB.pType[i]) &&
                (RM_ENGINE_TYPE_GR_IDX(pGpu->engineDB.pType[i]) > 0))
            {
                //
                // This check is used during GPU instance creation, prior to which
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

    //
    // Check for any alive P2P references to this GPU. P2P objects must
    // be re-created after disabling MIG. If it is allowed for  MIG to
    // continue enablement without all P2P objects torn down, there is
    // the possibility that P2P mappings and state will never be updated.
    //
    if (bMemoryPartitioningNeeded || !kmigmgrIsMIGNvlinkP2PSupportOverridden(pGpu, pKernelMIGManager))
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR,
            !kbusIsGpuP2pAlive(pGpu, GPU_GET_KERNEL_BUS(pGpu)),
            NV_ERR_STATE_IN_USE);
    }

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
kmigmgrIsGPUInstanceFlagValid_GA100
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
kmigmgrIsGPUInstanceCombinationValid_GA100
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    NvU32 gpuInstanceFlag
)
{
    NvU32 memSizeFlag = DRF_VAL(2080_CTRL_GPU, _PARTITION_FLAG, _MEMORY_SIZE, gpuInstanceFlag);
    NvU32 computeSizeFlag = DRF_VAL(2080_CTRL_GPU, _PARTITION_FLAG, _COMPUTE_SIZE, gpuInstanceFlag);

    if (!kmigmgrIsGPUInstanceFlagValid_HAL(pGpu, pKernelMIGManager, gpuInstanceFlag))
    {
        return NV_FALSE;
    }

    // JPG_OFA profile is only available on the smallest partition
    if (FLD_TEST_REF(NV2080_CTRL_GPU_PARTITION_FLAG_REQ_DEC_JPG_OFA, _ENABLE, gpuInstanceFlag))
    {
        if (kmigmgrIsA100ReducedConfig(pGpu, pKernelMIGManager))
        {
            if ((computeSizeFlag != NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_HALF) &&
                (computeSizeFlag != NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_QUARTER))
            {
                return NV_FALSE;
            }
        }
        else if (computeSizeFlag != NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_EIGHTH)
        {
            return NV_FALSE;
        }
    }

    if (kmigmgrIsA100ReducedConfig(pGpu, pKernelMIGManager) &&
        ((computeSizeFlag == NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_MINI_HALF) ||
         (computeSizeFlag == NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_MINI_QUARTER)))
    {
        return NV_FALSE;
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

/*!
 * @brief   Returns the range of swizzids which can be assigned to a GPU
 *          instance of the given size.
 *
 * @param[IN]   pGpu
 * @param[IN]   pKernelMIGManager
 * @param[IN]   memSizeFlag       NV2080_CTRL_GPU_PARTITION_FLAG_MEMORY_SIZE_*
 */
NV_RANGE
kmigmgrMemSizeFlagToSwizzIdRange_GA100
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    NvU32 memSizeFlag
)
{
    NV_RANGE ret;

    switch (DRF_VAL(2080_CTRL_GPU, _PARTITION_FLAG, _MEMORY_SIZE, memSizeFlag))
    {
        case NV2080_CTRL_GPU_PARTITION_FLAG_MEMORY_SIZE_FULL:
        {
            ret = rangeMake(0, 0);
            break;
        }

        case NV2080_CTRL_GPU_PARTITION_FLAG_MEMORY_SIZE_HALF:
        {
            ret = rangeMake(1, 2);
            break;
        }

        case NV2080_CTRL_GPU_PARTITION_FLAG_MEMORY_SIZE_QUARTER:
        {
            ret = rangeMake(3, 6);
            break;
        }

        case NV2080_CTRL_GPU_PARTITION_FLAG_MEMORY_SIZE_EIGHTH:
        {
            ret = rangeMake(7, 14);
            break;
        }

        default:
        {
            NV_PRINTF(LEVEL_ERROR, "Unsupported mem size flag 0x%x\n",
                      memSizeFlag);
            DBG_BREAKPOINT();
            ret = NV_RANGE_EMPTY;
            break;
        }
    }
    return ret;
}
/*!
 * @brief   Checks if user requested a configuration that should require memory partitioning
 *
 * @param[IN]   pGpu
 * @param[IN]   pKernelMIGManager
 * @param[IN]   partitionFlags    Client request flags
 */
NvBool
kmigmgrIsMemoryPartitioningRequested_GA100
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    NvU32 partitionFlags
)
{
    NvU32 memSizeFlag = DRF_VAL(2080_CTRL_GPU, _PARTITION_FLAG, _MEMORY_SIZE, partitionFlags);
    return (memSizeFlag != NV2080_CTRL_GPU_PARTITION_FLAG_MEMORY_SIZE_FULL);
}

/*!
 * @brief   Checks if memory partitioning will be needed for a given swizzId
 */
NvBool
kmigmgrIsMemoryPartitioningNeeded_GA100
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    NvU32 swizzId
)
{
    // Memory partitioning is needed for non-zero swizzIds
    return (swizzId != 0);
}

/*!
 * @brief   Returns the span covered by the swizzId
 */
NV_RANGE
kmigmgrSwizzIdToSpan_GA100
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    NvU32 swizzId
)
{
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
    NV_RANGE ret;
    NvU8 spanLen;
    NvU32 maxValidSwizzId;

    NV_ASSERT_OR_RETURN(kgrmgrGetLegacyKGraphicsStaticInfo(pGpu, pKernelGraphicsManager)->bInitialized, NV_RANGE_EMPTY);
    NV_ASSERT_OR_RETURN(kgrmgrGetLegacyKGraphicsStaticInfo(pGpu, pKernelGraphicsManager)->pGrInfo != NULL, NV_RANGE_EMPTY);

    if (kmigmgrIsA100ReducedConfig(pGpu, pKernelMIGManager))
        spanLen = 4;
    else
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
        case 3:
            ret = rangeMake(0, (spanLen/4) - 1);
            break;
        case 4:
            ret = rangeMake((spanLen/4), (spanLen/2) - 1);
            break;
        case 5:
            ret = rangeMake((spanLen/2), (3*(spanLen/4)) - 1);
            break;
        case 6:
            ret = rangeMake((3*(spanLen/4)), spanLen - 1);
            break;
        case 7:
            ret = rangeMake(0, 0);
            break;
        case 8:
            ret = rangeMake(1, 1);
            break;
        case 9:
            ret = rangeMake(2, 2);
            break;
        case 10:
            ret = rangeMake(3, 3);
            break;
        case 11:
            ret = rangeMake(4, 4);
            break;
        case 12:
            ret = rangeMake(5, 5);
            break;
        case 13:
            ret = rangeMake(6, 6);
            break;
        case 14:
            ret = rangeMake(7, 7);
            break;
        default:
            NV_PRINTF(LEVEL_ERROR, "Unsupported swizzid 0x%x\n", swizzId);
            DBG_BREAKPOINT();
            ret = NV_RANGE_EMPTY;
            break;
    }

    maxValidSwizzId = (spanLen * 2) - 2;

    if (swizzId > maxValidSwizzId)
    {
        ret = NV_RANGE_EMPTY;
    }

    return ret;
}

/*!
 * @brief Check if we are running on a reduced config GPU then set the corresponding flag
 */
void
kmigmgrDetectReducedConfig_GA100
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager
)
{
    const KERNEL_MIG_MANAGER_STATIC_INFO *pStaticInfo = kmigmgrGetStaticInfo(pGpu, pKernelMIGManager);
    NvU32 i;

    NV_ASSERT_OR_RETURN_VOID(pStaticInfo != NULL);

    for (i = 0; i < pStaticInfo->pCIProfiles->profileCount; ++i)
    {
        // Reduced config A100 does not support 1/8 compute size
        if (pStaticInfo->pCIProfiles->profiles[i].computeSize == NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE_EIGHTH)
        {
            return;
        }
    }

    pKernelMIGManager->bIsA100ReducedConfig = NV_TRUE;
}
