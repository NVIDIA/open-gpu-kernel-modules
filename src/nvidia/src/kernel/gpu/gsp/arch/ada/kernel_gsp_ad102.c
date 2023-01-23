/*
 * SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*!
 * Provides AD102+ specific KernelGsp HAL implementations.
 */

#include "gpu/gsp/kernel_gsp.h"

#include "gpu/falcon/kernel_falcon.h"
#include "gpu/sec2/kernel_sec2.h"

#include "published/ada/ad102/dev_gc6_island.h"
#include "published/ada/ad102/dev_gc6_island_addendum.h"

static NvBool
_kgspIsScrubberCompleted
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp
)
{
    NvU32 data = GPU_REG_RD32(pGpu, NV_PGC6_BSI_VPR_SECURE_SCRATCH_15);
    NvU32 handoff = DRF_VAL(_PGC6, _BSI_VPR_SECURE_SCRATCH_15, _SCRUBBER_HANDOFF, data);
    return (handoff >= NV_PGC6_BSI_VPR_SECURE_SCRATCH_15_SCRUBBER_HANDOFF_DONE);
}

NV_STATUS
kgspExecuteScrubberIfNeeded_AD102
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp
)
{
    NV_STATUS status;
    KernelSec2 *pKernelSec2 = GPU_GET_KERNEL_SEC2(pGpu);

    // skip executing Scrubber if it has already run
    if (_kgspIsScrubberCompleted(pGpu, pKernelGsp))
    {
        NV_PRINTF(LEVEL_INFO, "skipping executing Scrubber as it already ran\n");
        return NV_OK;
    }

    NV_PRINTF(LEVEL_INFO, "executing Scrubber\n");
    NV_ASSERT_OR_RETURN(pKernelGsp->pScrubberUcode != NULL, NV_ERR_INVALID_STATE);

    kflcnReset_HAL(pGpu, staticCast(pKernelSec2, KernelFalcon));

    status = kgspExecuteHsFalcon_HAL(pGpu, pKernelGsp,
                                     pKernelGsp->pScrubberUcode, staticCast(pKernelSec2, KernelFalcon),
                                     NULL, NULL);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "failed to execute Scrubber: 0x%x\n", status);
        return status;
    }

    if (!_kgspIsScrubberCompleted(pGpu, pKernelGsp))
    {
        NV_PRINTF(LEVEL_ERROR, "failed to execute Scrubber: done bit not set\n");
        return NV_ERR_GENERIC;
    }

    return status;
}

NvU64
kgspGetWprHeapSize_AD102
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp
)
{
    if (pGpu->bVgpuGspPluginOffloadEnabled)
    {
        //
        // Memory calculations which we used to size the WPR heap:
        // Memory needed for libOS kernel - 10MB(RM) + 32MB(VF partitions) + 8MB(global page) = 50MB
        // Memory needed from FW's Heap - 17MB for RM + 15MB * 32(VF) = 497MB
        // Total Memory needed = 497MB + 50MB = 547MB
        // (Added extra 2 Mb due to alignment)
        // @TODO: Change total Heap size to 546MB once fix for making global page to 4MB submitted.
        //
        return 550 * 1024 * 1024;
    }
    else
    {
        return 80 * 1024 * 1024;
    }
}