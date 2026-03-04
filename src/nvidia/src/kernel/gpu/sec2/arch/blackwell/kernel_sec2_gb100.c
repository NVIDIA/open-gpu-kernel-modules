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

#include "gpu/sec2/kernel_sec2.h"
#include "core/core.h"
#include "gpu/gpu.h"

#include "published/blackwell/gb100/dev_fuse_zb.h"
#include "published/blackwell/gb100/hwproject.h"

/*!
 * Returns the SEC2 fuse version of the provided ucode id (1-indexed)
 *
 * @param      pGpu         OBJGPU pointer
 * @param      pKernelSec2  KernelSec2 pointer
 * @param[in]  ucodeId      Ucode Id (1-indexed) to read fuse for
 */
NvU32
ksec2ReadUcodeFuseVersion_GB100
(
    OBJGPU *pGpu,
    KernelSec2 *pKernelSec2,
    const NvU32 ucodeId
)
{
    NvU32 fuseVal = 0;
    NvU32 index = ucodeId - 1;  // adjust to 0-indexed

    if (index < DRF_SIZE(NV_FUSE_ZB_OPT_FPF_SEC2_UCODE1_VERSION_DATA))
    {
        fuseVal = GPU_REG_RD32(pGpu, NV_FUSE0_PRI_BASE + NV_FUSE_ZB_OPT_FPF_SEC2_UCODE1_VERSION + (4 * index));
        if (fuseVal)
        {
            HIGHESTBITIDX_32(fuseVal);
            fuseVal = fuseVal + 1;
        }
    }

    return fuseVal;
}