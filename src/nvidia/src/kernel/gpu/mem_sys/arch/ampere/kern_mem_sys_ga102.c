/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "kernel/gpu/mem_sys/kern_mem_sys.h"

#include "published/ampere/ga102/dev_gc6_island.h"
#include "published/ampere/ga102/dev_gc6_island_addendum.h"

/*!
 * @brief get usable fb size in MB from NV_USABLE_FB_SIZE_IN_MB
 *
 * @param[out]  pFbSize  FB size computed from NV_USABLE_FB_SIZE_IN_MB
 */
NV_STATUS
kmemsysReadUsableFbSize_GA102
(
    OBJGPU             *pGpu,
    KernelMemorySystem *pKernelMemorySystem,
    NvU64              *pFbSize
)
{
     //
     // VBIOS Devinit calculates the usable FB size calculated and published in
     // NV_USABLE_FB_SIZE_IN_MB.
     //
    NvU32 regValue = GPU_REG_RD32(pGpu, NV_USABLE_FB_SIZE_IN_MB);

    *pFbSize = ((NvU64) DRF_VAL(_USABLE, _FB_SIZE_IN_MB, _VALUE, regValue) << 20);
    return NV_OK;
}

NvBool
kmemsysIsPagePLCable_GA102
(
    OBJGPU             *pGpu,
    KernelMemorySystem *pKernelMemorySystem,
    NvU64               physAddr,
    NvU64               pageSize
)
{
    const MEMORY_SYSTEM_STATIC_CONFIG *pMemorySystemConfig = kmemsysGetStaticConfig(pGpu, pKernelMemorySystem);
    NvBool bPageSize2M = (pageSize == (2 << 20));

    if (pMemorySystemConfig->ltsPerLtcCount * pMemorySystemConfig->ltcCount == 48)
    {
        if (!bPageSize2M)
        {
            return ((physAddr % (384 * 64 * 1024)) >= (6 * 64 * 1024));
        }
        else
        {
            NvBool noPLC = NV_FALSE;

            for (NvU64 addr = physAddr; (addr < physAddr + (2 * 1024 * 1024)); addr += (64 * 1024))
            {
                noPLC |= ((addr % (384 * 64 * 1024)) < (6 * 64 * 1024));
            }

            return !noPLC;
        }
    }
    else if (pMemorySystemConfig->ltsPerLtcCount * pMemorySystemConfig->ltcCount == 40)
    {
        if (!bPageSize2M)
        {
            return ((physAddr % (320 * 64 * 1024)) >= (5 * 64 * 1024));
        }
        else
        {
            NvBool noPLC = NV_FALSE;

            for (NvU64 addr = physAddr; (addr < physAddr + (2 * 1024 * 1024)); addr += (64 * 1024))
            {
                noPLC |= ((addr % (320 * 64 * 1024)) < (5 * 64 * 1024));
            }

            return !noPLC;
        }
    }
    else if (pMemorySystemConfig->ltsPerLtcCount == 4 && pMemorySystemConfig->ltcCount == 8)
    {
        if (!bPageSize2M)
        {
            return ((physAddr % (256 * 64 * 1024)) >= (4 * 64 * 1024));
        }
        else
        {
            NvBool noPLC = NV_FALSE;

            for (NvU64 addr = physAddr; (addr < physAddr + (2 * 1024 * 1024)); addr += (64 * 1024))
            {
                noPLC |= ((addr % (256 * 64 * 1024)) < (4 * 64 * 1024));
            }

            return !noPLC;
        }
    }
    else if (pMemorySystemConfig->ltsPerLtcCount == 3 && pMemorySystemConfig->ltcCount == 8)
    {
        if (!bPageSize2M)
        {
            return ((physAddr % (192 * 64 * 1024)) >= (3 * 64 * 1024));
        }
        else
        {
            NvBool noPLC = NV_FALSE;

            for (NvU64 addr = physAddr; (addr < physAddr + (2 * 1024 * 1024)); addr += (64 * 1024))
            {
                noPLC |= ((addr % (192 * 64 * 1024)) < (3 * 64 * 1024));
            }

            return !noPLC;
        }
    }
    else
    {
        return NV_TRUE;
    }
}
