/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * Provides GA100+ specific KernelGsp HAL implementations.
 */

#include "gpu/gsp/kernel_gsp.h"
#include "published/ampere/ga100/dev_fuse.h"


/*!
 * Check if the GSP is in debug mode
 *
 * @return whether the GSP is in debug mode or not
 */
NvBool
kgspIsDebugModeEnabled_GA100
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp
)
{
    NvU32 data;

    data = GPU_REG_RD32(pGpu, NV_FUSE_OPT_SECURE_GSP_DEBUG_DIS);

    return FLD_TEST_DRF(_FUSE, _OPT_SECURE_GSP_DEBUG_DIS, _DATA, _NO, data);
}

/*!
 * Returns the GSP fuse version of the provided ucode id (1-indexed)
 *
 * @param      pGpu         OBJGPU pointer
 * @param      pKernelGsp   KernelGsp pointer
 * @param[in]  ucodeId      Ucode Id (1-indexed) to read fuse for
 */
NvU32
kgspReadUcodeFuseVersion_GA100
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    const NvU32 ucodeId
)
{
    NvU32 fuseVal = 0;
    NvU32 index = ucodeId - 1;  // adjust to 0-indexed

    // TODO: Bug 3519329: switch to indexed register once available
    // if (index < NV_FUSE_OPT_FPF_GSP_UCODE_VERSION__SIZE_1)
    if (index < 16)
    {
        // fuseVal = GPU_REG_IDX_RD_DRF(pGpu, _FUSE, _OPT_FPF_GSP_UCODE_VERSION, index, _DATA);
        fuseVal = GPU_REG_RD32(pGpu, NV_FUSE_OPT_FPF_GSP_UCODE1_VERSION + (4 * index));

        if (fuseVal)
        {
            HIGHESTBITIDX_32(fuseVal);
            fuseVal = fuseVal + 1;
        }
    }

    return fuseVal;
}
