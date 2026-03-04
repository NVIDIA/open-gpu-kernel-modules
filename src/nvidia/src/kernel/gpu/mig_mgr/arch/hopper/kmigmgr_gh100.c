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

#define NVOC_KERNEL_MIG_MANAGER_H_PRIVATE_ACCESS_ALLOWED

#include "kernel/gpu/mem_mgr/mem_mgr.h"
#include "kernel/gpu/ce/kernel_ce.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "kernel/gpu/fifo/kernel_fifo.h"

/*!
 * @brief   Once MIG is disabled, apply default non-MIG CE mappings
 */
void
kmigmgrApplyDefaultCeMappings_GH100
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager
)
{
    KernelCE      *pKCe            = NULL;
    NV_STATUS      status          = NV_OK;

    KCE_ITER_SHIM_BEGIN(pGpu, pKCe)
        if (pKCe != NULL)
        {
            status = kceTopLevelPceLceMappingsUpdate(pGpu, pKCe);
            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR, "Failed to update PCE-LCE mappings\n");
            }
        }
    KCE_ITER_END;
}

/*!
 * @brief   Clean up CE mappings on this MIG GPU Instance
 */
NV_STATUS
kmigmgrClearMIGGpuInstanceCeMapping_GH100
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance
)
{
    KernelCE      *pKCe            = NULL;
    NvU32         *pLocalPceLceMap = NULL;
    NvU32          i;
    RM_ENGINE_TYPE ceEngineType;
    NvU32 lceAvailableMask = 0;
    ENGTYPE_BIT_VECTOR ceEngines;

    bitVectorClrAll(&ceEngines);
    bitVectorSetRange(&ceEngines, RM_ENGINE_RANGE_COPY());
    bitVectorAnd(&ceEngines, &ceEngines, &pKernelMIGGpuInstance->resourceAllocation.engines);
    FOR_EACH_IN_BITVECTOR(&ceEngines, ceEngineType)
    {
        if (!ceIsCeGrce(pGpu, ceEngineType))
        {
            lceAvailableMask |= NVBIT32(RM_ENGINE_TYPE_COPY_IDX((NvU32)ceEngineType));
        }
    }
    FOR_EACH_IN_BITVECTOR_END();

    NV_PRINTF(LEVEL_INFO, "Unmapping LCEs 0x%x as part of GPU Instance clean up\n", lceAvailableMask);

    pLocalPceLceMap = portMemAllocNonPaged(sizeof(NvU32[NV2080_CTRL_MAX_PCES]));
    NV_ASSERT_OR_RETURN(pLocalPceLceMap  != NULL, NV_ERR_NO_MEMORY);

    for (i = 0; i < NV2080_CTRL_MAX_PCES; i++)
    {
        // An all default values will indicate to clear the GPUInstance mapping
        pLocalPceLceMap[i] = NV2080_CTRL_CE_UPDATE_PCE_LCE_MAPPINGS_INVALID_LCE;
    }

    KCE_ITER_SHIM_BEGIN(pGpu, pKCe)
        if (!kceApplyMIGMappings(pGpu, pKCe, pLocalPceLceMap, lceAvailableMask))
        {
            NV_PRINTF(LEVEL_ERROR, "Failure to clear MIG Mappings on lceAvailableMask 0x%x\n", lceAvailableMask);
        }
    KCE_ITER_END;

    portMemFree(pLocalPceLceMap);
    return NV_OK;
}

/*!
 * @brief   Apply CE mappings on this MIG GPU Instance
 *
 * @param[IN]   pGpu
 * @param[IN]   pKernelMIGManager
 * @param[IN]   pKernelMIGGpuInstance
 *
 * @return  NV_OK on successful mapping
 *          NV_ERR_NO_MEMORY if unable to create the local mapping array
 */
NV_STATUS
kmigmgrApplyMIGGpuInstanceCeMapping_GH100
(
    OBJGPU *pGpu,
    KernelMIGManager *pKernelMIGManager,
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance
)
{
        // Get and Set PCE-LCE Mappings for this GPU Instance
        KernelCE      *pKCe            = NULL;
        NvU32         *pLocalPceLceMap = NULL;
        NvU32          lceAvailableMask = 0;
        NvU32          i;
        RM_ENGINE_TYPE ceEngineType;
        ENGTYPE_BIT_VECTOR ceEngines;

        bitVectorClrAll(&ceEngines);
        bitVectorSetRange(&ceEngines, RM_ENGINE_RANGE_COPY());
        bitVectorAnd(&ceEngines, &ceEngines, &pKernelMIGGpuInstance->resourceAllocation.engines);

        FOR_EACH_IN_BITVECTOR(&ceEngines, ceEngineType)
        {
            if (!ceIsCeGrce(pGpu, ceEngineType))
            {
                lceAvailableMask |= NVBIT32(RM_ENGINE_TYPE_COPY_IDX((NvU32)ceEngineType));
            }
        }
        FOR_EACH_IN_BITVECTOR_END();

        NV_PRINTF(LEVEL_INFO, "Setup CE mappings on LCEs 0x%x as part of GPU Instance creation\n", lceAvailableMask);

        pLocalPceLceMap = portMemAllocNonPaged(sizeof(NvU32[NV2080_CTRL_MAX_PCES]));
        NV_ASSERT_OR_RETURN(pLocalPceLceMap  != NULL, NV_ERR_NO_MEMORY);

        KCE_ITER_SHIM_BEGIN(pGpu, pKCe)
            for (i = 0; i < NV2080_CTRL_MAX_PCES; i++)
            {
                pLocalPceLceMap[i] = NV2080_CTRL_CE_UPDATE_PCE_LCE_MAPPINGS_INVALID_LCE;
            }
            kceGetMappingsForMIGGpuInstance_HAL(pGpu, pKCe, lceAvailableMask, pLocalPceLceMap);

            if (!kceApplyMIGMappings(pGpu, pKCe, pLocalPceLceMap, lceAvailableMask))
            {
                NV_PRINTF(LEVEL_ERROR, "Failed to apply MIG Mappings on LCE mask 0x%x\n", lceAvailableMask);
            }
        KCE_ITER_END;

        portMemFree(pLocalPceLceMap);
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
kmigmgrIsGPUInstanceFlagValid_GH100
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
        default:
            return NV_FALSE;
    }
    switch (gfxSizeFlag)
    {
        case NV2080_CTRL_GPU_PARTITION_FLAG_GFX_SIZE_NONE:
            break;
        default:
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
