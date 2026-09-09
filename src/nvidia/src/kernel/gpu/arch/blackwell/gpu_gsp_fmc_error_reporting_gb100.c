/*
 * SPDX-FileCopyrightText: Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * Provides GB100+ specific GSP-FMC error reporting HAL implementations.
 */

#include "gpu/gpu.h"
#include "gsp/gsp_fmc_error_code_formatting.h"

#define GSP_FMC_OK 0x00000000

#include "published/blackwell/gb100/hwproject.h"
#include "published/blackwell/gb100/dev_bus_zb.h"
#include "published/blackwell/gb100/dev_bus_zb_addendum.h"

void
gpuResetGspFmcErrorCode_GB100
(
    OBJGPU *pGpu
)
{
    GPU_REG_WR32(pGpu, NV_PBUS0_PRI_BASE + NV_PBUS_ZB_SW_SCRATCH_GSP_FMC_ERROR, 0);
}

NvBool
gpuCheckGspFmcErrorCode_GB100
(
    OBJGPU *pGpu
)
{
    NvU32 errorCode = GPU_REG_RD32(pGpu, NV_PBUS0_PRI_BASE + NV_PBUS_ZB_SW_SCRATCH_GSP_FMC_ERROR);
    return (errorCode != GSP_FMC_OK);
}

NvU32
gpuGetGspFmcErrorCode_GB100
(
    OBJGPU *pGpu
)
{
    return GPU_REG_RD32(pGpu, NV_PBUS0_PRI_BASE + NV_PBUS_ZB_SW_SCRATCH_GSP_FMC_ERROR);
}