/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2019 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * @file   flcn0300_nvswitch.c
 * @brief  Provides the implementation for all falcon 3.0 HAL interfaces.
 */

#include "nvmisc.h"
#include "common_nvswitch.h"

#include "flcn/flcnable_nvswitch.h"
#include "flcn/flcn_nvswitch.h"

#include "nvswitch/lr10/dev_falcon_v4.h"

/*!
 * @brief Get information about the falcon core
 *
 * @param[in] device nvswitch_device pointer
 * @param[in] pFlcn  FLCN pointer
 *
 * @returns nothing
 */
static void
_flcnGetCoreInfo_v03_00
(
    nvswitch_device *device,
    PFLCN            pFlcn
)
{
    NvU32 hwcfg1 = flcnRegRead_HAL(device, pFlcn, NV_PFALCON_FALCON_HWCFG1);

    if (FLD_TEST_DRF(_PFALCON, _FALCON_HWCFG1, _SECURITY_MODEL, _HEAVY, hwcfg1))
    {
        NVSWITCH_PRINT(device, INFO,
                    "%s: Engine '%s' is using the heavy security model\n",
                    __FUNCTION__, flcnGetName_HAL(device, pFlcn));
    }

    // Save off the security model.
    pFlcn->securityModel = DRF_VAL(_PFALCON, _FALCON_HWCFG1, _SECURITY_MODEL, hwcfg1);

    // Combine Falcon core revision and subversion for easy version comparison.
    pFlcn->coreRev = flcnableReadCoreRev(device, pFlcn->pFlcnable);

    pFlcn->supportsDmemApertures = FLD_TEST_DRF(_PFALCON, _FALCON_HWCFG1, _DMEM_APERTURES, _ENABLE, hwcfg1);
}

/**
 * @brief   set hal function pointers for functions defined in v03_00 (i.e. this file)
 *
 * this function has to be at the end of the file so that all the
 * other functions are already defined.
 *
 * @param[in] pFlcn   The flcn for which to set hals
 */
void
flcnSetupHal_v03_00
(
    PFLCN            pFlcn
)
{
    flcn_hal *pHal = pFlcn->pHal;

    pHal->getCoreInfo = _flcnGetCoreInfo_v03_00;
}

