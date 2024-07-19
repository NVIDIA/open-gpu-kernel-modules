/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "published/maxwell/gm107/dev_boot.h"

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
kmcGetMcBar0MapInfo_GM107
(
    OBJGPU  *pGpu,
    KernelMc  *pKernelMc,
    NvU64   *pBar0MapOffset,
    NvU32   *pBar0MapSize
)
{
    *pBar0MapOffset = DRF_BASE(NV_PMC);
    *pBar0MapSize = DRF_SIZE(NV_PMC);
    return NV_OK;
}

//
// This function is used to power-up the engines before we do a full-chip
// reset.
//
NV_STATUS
kmcPrepareForXVEReset_GM107
(
    OBJGPU *pGpu,
    KernelMc *pKernelMc
)
{
    // FERMI-TODO
    return NV_OK;
}

/*!
 * @brief  Updates PMC_ENABLE register whose pmcEnableMask is passed.
 *  If bEnable is NV_TRUE, then update entire PMC register, else
 *  if bEnable is false, then reset the engines whose mask is passed as input.
 *
 * @param[in]  pGpu     GPU object pointer
 * @param[in]  pKernelMc
 * @param[in]  pmcEnableMask PMC Mask of engines to be reset.
 * @param[in]  bEnable  If True, Enable the engine, else Reset the engines.
 * @param[in]  bIsPmcDeviceEngine NV_TRUE if it is PMC_DEVICE_ENABLE register
 *                      else its PMC_ENABLE register.
 *
 * @return NV_OK
 */
NV_STATUS
kmcWritePmcEnableReg_GM107
(
    OBJGPU *pGpu,
    KernelMc *pKernelMc,
    NvU32 pmcEnableMask,
    NvBool bEnable,
    NvBool bIsPmcDeviceEngine
)
{
    NvU32 newPmc;

    // NV_PMC_DEVICE_ENABLE register is supported on Ampere and later only.
    if (bIsPmcDeviceEngine)
        return NV_OK;

    newPmc = GPU_REG_RD32(pGpu, NV_PMC_ENABLE);

    // Enable Engines.
    if (bEnable)
        newPmc |= pmcEnableMask;
    // Reset Engines.
    else
        newPmc &= ~pmcEnableMask;

    GPU_REG_WR32(pGpu, NV_PMC_ENABLE, newPmc);

    // Delay for engines reset to complete.
    osDelayUs(NV_PMC_RESET_DELAY_US);

    return NV_OK;
}
