/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "kernel/gpu/mc/kernel_mc.h"
#include "gpu/gpu.h"
#include "os/os.h"

#include "published/blackwell/gb100/dev_boot_zb.h"
#include "published/blackwell/gb100/hwproject.h"

/*!
 * @brief Returns the BAR0 offset and size of the PMC range.
 *
 * @param[in] pGpu
 * @param[in] pKernelMc
 * @param[out] pBar0MapOffset
 * @param[out] pBar0MapSize
 *
 * @return NV_STATUS
 */
NV_STATUS
kmcGetMcBar0MapInfo_GB100
(
    OBJGPU  *pGpu,
    KernelMc  *pKernelMc,
    NvU64   *pBar0MapOffset,
    NvU32   *pBar0MapSize
)
{
    *pBar0MapOffset = NV_PMC0_PRI_BASE;
    *pBar0MapSize = DRF_SIZE(NV_PMC_ZB);
    return NV_OK;
}

NvU32
kmcReadPmcBoot0_GB100
(
    OBJGPU  *pGpu,
    KernelMc  *pKernelMc
)
{
    return GPU_REG_RD32(pGpu, NV_PMC0_PRI_BASE + NV_PMC_ZB_BOOT_0);
}
