/*
 * SPDX-FileCopyrightText: Copyright (c) 2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/mem_sys/kern_mem_sys.h"

/*!
 *  Returns the max context size
 *
 *  @returns NvU64
 */
NvU64
memmgrGetMaxContextSize_AD104
(
    OBJGPU        *pGpu,
    MemoryManager *pMemoryManager
)
{
    NvU64 fbSize = 0;
    NvU64  size = memmgrGetMaxContextSize_GA100(pGpu, pMemoryManager);
    KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);

    NV_ASSERT_OK(kmemsysGetUsableFbSize_HAL(pGpu, pKernelMemorySystem, &fbSize));
    const NvU32 fbSizeGB = (NvU32)(NV_ALIGN_UP64(fbSize, 1 << 30) >> 30);

    //
    // If the FB size of chips is less than 12GB then
    // increasing Reserved Size by 10MB, Bug: 4455873
    //
    if (fbSizeGB < 12)
    {
        size += 10 * 1024 * 1024;
    }

    if (RMCFG_FEATURE_PLATFORM_MODS)
    {
        size += 64 * 1024 * 1024;
    }
    return size;
}
