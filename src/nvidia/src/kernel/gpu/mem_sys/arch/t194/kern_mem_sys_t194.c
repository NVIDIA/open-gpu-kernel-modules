/*
 * SPDX-FileCopyrightText: Copyright (c) 2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/mem_mgr/mem_mgr.h"

NvBool
kmemsysNeedInvalidateGpuCacheOnUnmap_T194
(
    OBJGPU              *pGpu,
    KernelMemorySystem  *pKernelMemorySystem,
    NvBool               bIsVolatile,
    GMMU_APERTURE        aperture
)
{
    NvBool bPlatformFullyCoherent = GPU_GET_MEMORY_MANAGER(pGpu)->bPlatformFullyCoherent;

    //
    // Only need to invalidate L2 for cached (vol=0) mapping to sys/peer memory
    // because GPU's L2 is not coherent with CPU updates to sysmem
    // See bugs 5355344, 3342220 for more info
    //
    return (!bIsVolatile && (aperture == GMMU_APERTURE_PEER ||
                             (aperture == GMMU_APERTURE_SYS_COH && !bPlatformFullyCoherent) ||
                             aperture == GMMU_APERTURE_SYS_NONCOH));
}

