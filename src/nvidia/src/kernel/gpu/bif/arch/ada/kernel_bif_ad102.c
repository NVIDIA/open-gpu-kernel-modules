/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/* ------------------------ Includes ---------------------------------------- */
#include "os/os.h"
#include "gpu/bif/kernel_bif.h"

#include "published/ada/ad102/dev_bus.h"
#include "published/ada/ad102/dev_bus_addendum.h"

static NvBool _kbifPreOsCheckErotGrantAllowed_AD102(OBJGPU *pGpu, void *pVoid);

/*!
 * Signals preOs to have eRoT hand over control of EEPROM to RM
 *
 * @param[in]     pGpu       OBJGPU pointer
 * @param[in]     pKernelBif KernelBif pointer
 *
 * @returns NV_OK if RM has control of the EEPROM
 * @returns NV_ERR_TIMEOUT if preOs fails to hand over control of the EEPROM
 *
 */
NV_STATUS
kbifPreOsGlobalErotGrantRequest_AD102
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NV_STATUS status = NV_OK;
    NvU32 reg = GPU_REG_RD32(pGpu, NV_PBUS_SW_GLOBAL_EROT_GRANT);

    // Invalid value suggests that there is no ERoT
    if (FLD_TEST_DRF(_PBUS, _SW_GLOBAL_EROT_GRANT, _VALID, _NO, reg))
    {
        return status;
    }

    // Check if grant has already been allowed
    if (_kbifPreOsCheckErotGrantAllowed_AD102(pGpu, NULL))
    {
        return status;
    }

    reg = FLD_SET_DRF(_PBUS, _SW_GLOBAL_EROT_GRANT, _REQUEST, _SET, reg);
    GPU_REG_WR32(pGpu, NV_PBUS_SW_GLOBAL_EROT_GRANT, reg);

    status = gpuTimeoutCondWait(pGpu, _kbifPreOsCheckErotGrantAllowed_AD102, NULL, NULL);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Timed out waiting for preOs to grant access to EEPROM\n");
    }

    return status;
}

static NvBool
_kbifPreOsCheckErotGrantAllowed_AD102
(
    OBJGPU *pGpu,
    void   *pVoid
)
{
    NvU32 reg = GPU_REG_RD32(pGpu, NV_PBUS_SW_GLOBAL_EROT_GRANT);

    return FLD_TEST_DRF(_PBUS, _SW_GLOBAL_EROT_GRANT, _ALLOW, _YES, reg);
}
