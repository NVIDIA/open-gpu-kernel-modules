/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "published/ampere/ga100/dev_boot.h"


/*!
 * @brief  Updates PMC_ENABLE register whose pmcEnableMask is passed.
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
kmcWritePmcEnableReg_GA100
(
    OBJGPU *pGpu,
    KernelMc *pKernelMc,
    NvU32 pmcEnableMask,
    NvBool bEnable,
    NvBool bIsPmcDeviceEngine
)
{
    NvU32 regAddr;
    NvU32 newPmc;

    //
    // If hardware increases the size of this register in future chips, we would
    // need to catch this and fork another HAL.
    // Already obsoleted by GH100, but keeping the check
    //
    ct_assert(NV_PMC_DEVICE_ENABLE__SIZE_1 <= 1);

    if (bIsPmcDeviceEngine)
    {
        regAddr = NV_PMC_DEVICE_ENABLE(0);
    }
    else
    {
        regAddr = NV_PMC_ENABLE;
    }

    //
    // Reset PMC Engines.
    //
    newPmc = GPU_REG_RD32(pGpu, regAddr);

    // Enable Engines.
    if (bEnable)
        newPmc |= pmcEnableMask;
    // Reset Engines.
    else
        newPmc &= ~pmcEnableMask;

    GPU_REG_WR32(pGpu, regAddr, newPmc);

    // Delay for engines reset to complete.
    osDelayUs(NV_PMC_RESET_DELAY_US);

    return NV_OK;
}
