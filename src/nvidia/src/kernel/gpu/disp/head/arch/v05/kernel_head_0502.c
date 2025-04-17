/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "gpu/disp/head/kernel_head.h"
#include "published/disp/v05_02/dev_disp.h"

NvBool kheadReadVblankIntrEnable_KERNEL_v05_02
(
    OBJGPU *pGpu,
    KernelHead *pKernelHead
)
{
    NvU32 intr = GPU_REG_RD32(pGpu, NV_PDISP_FE_RM_INTR_EN1_HEAD_TIMING(pKernelHead->PublicId));
    if (FLD_TEST_DRF(_PDISP, _FE_RM_INTR_EN1_HEAD_TIMING, _LAST_DATA, _ENABLE, intr))
    {
         return NV_TRUE;
    }
    return NV_FALSE;
}

void kheadWriteVblankIntrEnable_KERNEL_v05_02
(
    OBJGPU *pGpu,
    KernelHead *pKernelHead,
    NvBool  bEnable
)
{
    NvU32 intrEn = GPU_REG_RD32(pGpu, NV_PDISP_FE_RM_INTR_EN1_HEAD_TIMING(pKernelHead->PublicId));
    kheadResetPendingVblank_HAL(pGpu, pKernelHead, NULL);
    if (bEnable)
    {
         intrEn = FLD_SET_DRF(_PDISP, _FE_RM_INTR_EN1_HEAD_TIMING, _LAST_DATA, _ENABLE, intrEn);
    }
    else
    {
        intrEn = FLD_SET_DRF(_PDISP, _FE_RM_INTR_EN1_HEAD_TIMING, _LAST_DATA, _DISABLE, intrEn);
    }
    GPU_REG_WR32(pGpu, NV_PDISP_FE_RM_INTR_EN1_HEAD_TIMING(pKernelHead->PublicId), intrEn);
}
