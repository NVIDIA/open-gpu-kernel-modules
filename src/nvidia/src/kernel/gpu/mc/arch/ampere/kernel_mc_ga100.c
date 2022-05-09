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
    //
    if ((sizeof(NvU32) * NV_PMC_DEVICE_ENABLE__SIZE_1) > sizeof(NvU32))
    {
        NV_ASSERT_FAILED("Assert for Mcheck to catch increase in register size. Fork this HAL");
    }

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

    //
    // Read from NV_PMC_ENABLE to create enough delay for engines reset to complete.
    //
    newPmc = GPU_REG_RD32(pGpu, regAddr);
    newPmc = GPU_REG_RD32(pGpu, regAddr);
    newPmc = GPU_REG_RD32(pGpu, regAddr);

    return NV_OK;
}

/*!
 * @brief  Returns NV_PMC_ENABLE or NV_PMC_DEVICE_ENABLE register based on bIsPmcDeviceEngine.
 *  If bIsPmcDeviceEngine is NV_TRUE, then return NV_PMC_DEVICE_ENABLE (available from Ampere),
 *  If bIsPmcDeviceEngine is NV_FALSE, then return NV_PMC_ENABLE.
 *
 * @param[in]  pGpu     GPU object pointer
 * @param[in]  pKernelMc
 * @param[in]  bIsPmcDeviceEngine if true return NV_PMC_DEVICE_ENABLE else return NV_PMC_ENABLE register.
 *
 * @return  NvU32 containing register data
 */
NvU32
kmcReadPmcEnableReg_GA100
(
    OBJGPU *pGpu,
    KernelMc *pKernelMc,
    NvBool bIsPmcDeviceEngine
)
{
    //
    // If hardware increases the size of this register in future chips, we would
    // need to catch this and fork another HAL.
    //
    if ((sizeof(NvU32) * NV_PMC_DEVICE_ENABLE__SIZE_1) > sizeof(NvU32))
    {
        NV_ASSERT_FAILED("Assert for Mcheck to catch increase in register size. Fork this HAL");
    }

    return bIsPmcDeviceEngine ?
           GPU_REG_RD32(pGpu, NV_PMC_DEVICE_ENABLE(0)) :
           GPU_REG_RD32(pGpu, NV_PMC_ENABLE);
}
