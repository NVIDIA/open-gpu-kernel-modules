/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "core/core.h"
#include "gpu/gpu.h"
#include "gpu/mem_sys/kern_mem_sys.h"

#include "published/pascal/gp102/dev_fb.h"

/*!
 * @brief get usable fb size in MB from NV_PFB_PRI_MMU_LOCAL_MEMORY_RANGE
 *
 * @param[out]  pFbSize  FB size computed from memory range
 */
NV_STATUS
kmemsysReadUsableFbSize_GP102
(
    OBJGPU             *pGpu,
    KernelMemorySystem *pKernelMemorySystem,
    NvU64              *pFbSize
)
{
    //
    // VBIOS Devinit calculates lower range values in
    // NV_PFB_PRI_MMU_LOCAL_MEMORY_RANGE.
    //
    NvU32 regValue       = GPU_REG_RD32(pGpu, NV_PFB_PRI_MMU_LOCAL_MEMORY_RANGE);
    NvU32 lowerRangeMag  = DRF_VAL(_PFB, _PRI_MMU_LOCAL_MEMORY_RANGE, _LOWER_MAG, regValue);
    NvU32 lowerRangeScale = DRF_VAL(_PFB, _PRI_MMU_LOCAL_MEMORY_RANGE, _LOWER_SCALE, regValue);
    NvU64 fbSize         = ((NvU64) lowerRangeMag << (lowerRangeScale + 20));

    if (FLD_TEST_DRF(_PFB, _PRI_MMU_LOCAL_MEMORY_RANGE, _ECC_MODE, _ENABLED, regValue))
    {
        fbSize = fbSize / 16 * 15;
    }

    *pFbSize = fbSize;
    return NV_OK;
}
