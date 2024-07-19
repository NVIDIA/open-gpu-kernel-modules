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
kgrmgrGetVeidsFromGpcCount_GB100
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
    NvU32 bucketBoundary;
    NvU32 syspipes;
    const NV2080_CTRL_INTERNAL_STATIC_GR_INFO *pGrInfo;

    NV_ASSERT_OR_RETURN(pVeidCount != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(gpcCount > 0, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(kgrmgrGetLegacyKGraphicsStaticInfo(pGpu, pKernelGraphicsManager)->bInitialized, NV_ERR_INVALID_STATE);
    pGrInfo = kgrmgrGetLegacyKGraphicsStaticInfo(pGpu, pKernelGraphicsManager)->pGrInfo;
    NV_ASSERT_OR_RETURN(pGrInfo != NULL, NV_ERR_INVALID_STATE);

    maxSyspipes = pGrInfo->infoList[NV0080_CTRL_GR_INFO_INDEX_MAX_MIG_ENGINES].data;
    maxPartitionableGpcs = pGrInfo->infoList[NV0080_CTRL_GR_INFO_INDEX_MAX_PARTITIONABLE_GPCS].data;
    maxVeids = pGrInfo->infoList[NV0080_CTRL_GR_INFO_INDEX_MAX_SUBCONTEXT_COUNT].data;

    //
    // The algorithm to determine the VEID count for given GPC count:
    // 1. Determine max number of syspipes and max number of GPCs in a perfect chip.
    // 2. Create buckets based on GPC counts in a perfect chip. Smallest bucket is 
    //    (maxPartitionableGpcs/maxSyspipes) and next bucket is 2X that, then 4X that,
    //    8X that, etc.
    // 3. Classify the actual number of GPCs a syspipe owns into the smallest bucket
    //    which is greater than actual number of GPCs.
    // 4. Use this bucket to convert to number of VEIDs that syspipe should own.
    //

    *pVeidCount = 0;
    for (syspipes = maxSyspipes; syspipes > 0; syspipes /= 2)
    {
        bucketBoundary = maxPartitionableGpcs / syspipes;
        if (gpcCount <= bucketBoundary)
        {
            *pVeidCount = maxVeids / syspipes;
            break;
        }
    }

    return NV_OK;
}

