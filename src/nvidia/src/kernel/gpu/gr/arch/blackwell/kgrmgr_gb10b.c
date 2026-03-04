/*
 * SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "kernel/gpu/gr/kernel_graphics_manager.h"

/*!
 * @brief Get the VEID count for the given GPC count
 */
NV_STATUS
kgrmgrGetVeidsFromGpcCount_GB10B
(
    OBJGPU *pGpu,
    KernelGraphicsManager *pKernelGraphicsManager,
    NvU32 gpcCount,
    NvU32 *pVeidCount
)
{
    NvU32 maxSyspipes;
    NvU32 maxPartitionableGpcs;
    NvU32 maxVeids;
    const NV2080_CTRL_INTERNAL_STATIC_GR_INFO *pGrInfo;

    NV_ASSERT_OR_RETURN(pVeidCount != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(gpcCount > 0, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(kgrmgrGetLegacyKGraphicsStaticInfo(pGpu, pKernelGraphicsManager)->bInitialized, NV_ERR_INVALID_STATE);
    pGrInfo = kgrmgrGetLegacyKGraphicsStaticInfo(pGpu, pKernelGraphicsManager)->pGrInfo;
    NV_ASSERT_OR_RETURN(pGrInfo != NULL, NV_ERR_INVALID_STATE);

    maxSyspipes = pGrInfo->infoList[NV0080_CTRL_GR_INFO_INDEX_MAX_MIG_ENGINES].data;
    maxPartitionableGpcs = pGrInfo->infoList[NV0080_CTRL_GR_INFO_INDEX_MAX_PARTITIONABLE_GPCS].data;
    maxVeids = pGrInfo->infoList[NV0080_CTRL_GR_INFO_INDEX_MAX_SUBCONTEXT_COUNT].data;

    NV_PRINTF(LEVEL_INFO, "Max SysPipes:%d, Max GPCs:%d, Max VEIDs:%d.\n", maxSyspipes, maxPartitionableGpcs, maxVeids);

    /*
     * GB10B has Maximum 2 Syspipes and 3 GPCs. All 64(maxVeids) are allocated if the requested
     * GPC count matches the maximum GPCs. For any other GPC count(2 or 1), 32 VEIDs are allocated.
     * So, a FULL partition would get 64 VEIDs and both the HALF partitions would get 32 VEIDs each.
     * If the GPCs are Floorswept down to a value less than 3, the VEIDs allocated would be 32 even for
     * a FULL partition.
     */
    if (gpcCount == maxPartitionableGpcs)
    {
        *pVeidCount = maxVeids;
    } 
    else
    {
        *pVeidCount = maxVeids/maxSyspipes;
    } 
 
    return NV_OK;
}
