/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "published/hopper/gh100/dev_pmc.h"

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
kmcWritePmcEnableReg_GH100
(
    OBJGPU   *pGpu,
    KernelMc *pKernelMc,
    NvU32     pmcEnableMask,
    NvBool    bEnable,
    NvBool    bIsPmcDeviceEngine
)
{
    if (bIsPmcDeviceEngine)
    {
        NV_ASSERT_FAILED("NV_PMC_DEVICE_ENABLE is not valid on Hopper+\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    // No need for bIsPmcDeviceEngine, so just call GK104
    return kmcWritePmcEnableReg_GK104(pGpu, pKernelMc, pmcEnableMask, bEnable, NV_FALSE);
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
kmcReadPmcEnableReg_GH100
(
    OBJGPU   *pGpu,
    KernelMc *pKernelMc,
    NvBool    bIsPmcDeviceEngine
)
{
    if (bIsPmcDeviceEngine)
    {
        NV_ASSERT_FAILED("NV_PMC_DEVICE_ENABLE is not valid on Hopper+\n");
        return 0;
    }

    // No need for bIsPmcDeviceEngine, so just call GK104
    return kmcReadPmcEnableReg_GK104(pGpu, pKernelMc, NV_FALSE);
}
