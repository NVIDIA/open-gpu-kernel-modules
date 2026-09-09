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
 * Provides GH100+ specific GSP-FMC error reporting HAL implementations.
 */

#include "gpu/gpu.h"
#include "gsp/gsp_fmc_error_code_formatting.h"

#include "published/hopper/gh100/dev_bus.h"
#include "published/hopper/gh100/dev_bus_addendum.h"

#define GSP_FMC_OK 0x00000000

void
gpuResetGspFmcErrorCode_GH100
(
    OBJGPU *pGpu
)
{
    GPU_REG_WR32(pGpu, NV_PBUS_SW_SCRATCH_GSP_FMC_ERROR, 0);
}

NvBool
gpuCheckGspFmcErrorCode_GH100
(
    OBJGPU *pGpu
)
{
    NvU32 errorCode = GPU_REG_RD32(pGpu, NV_PBUS_SW_SCRATCH_GSP_FMC_ERROR);
    return (errorCode != GSP_FMC_OK);
}

NvU32
gpuGetGspFmcErrorCode_GH100
(
    OBJGPU *pGpu
)
{
    return GPU_REG_RD32(pGpu, NV_PBUS_SW_SCRATCH_GSP_FMC_ERROR);
}

void
gpuReportGspFmcErrorCode_GH100
(
    OBJGPU *pGpu
)
{
    NvU32 errorCode = gpuGetGspFmcErrorCode_HAL(pGpu);

    if (errorCode == GSP_FMC_OK)
    {
        return;
    }

    // Decode the formatted error code
    NvU32 version = DRF_VAL(_PBUS, _SW_SCRATCH_GSP_FMC_ERROR, _VARIANT, errorCode);
    NvU32 encodedPartitionID = DRF_VAL(_PBUS, _SW_SCRATCH_GSP_FMC_ERROR, _PARTITION, errorCode);

    // Reverse the transformation to get the original partition ID
    NvU32 partitionID = (encodedPartitionID - 1) & GSP_FMC_PARTITION_ID_MASK;

    switch (version)
    {
        case GSP_FMC_ERROR_VARIANT_SK:
        {
            // Extract subdivided fields directly from the full error code.
            NvU32 sk_phase = DRF_VAL(_PBUS, _SW_SCRATCH_GSP_FMC_ERROR, _SK_PHASE, errorCode);
            NvU32 sk_error = DRF_VAL(_PBUS, _SW_SCRATCH_GSP_FMC_ERROR, _SK_ERROR, errorCode);
            NV_PRINTF(LEVEL_ERROR,
                      "Fatal GSP-FMC Error: SK error code =0x%x, phase 0x%x\n",
                      sk_error, sk_phase);
            break;
        }
        case GSP_FMC_ERROR_VARIANT_GENERIC:
        {
            NvU32 err  = DRF_VAL(_PBUS, _SW_SCRATCH_GSP_FMC_ERROR, _GENERIC_ERROR_CODE, errorCode);
            NvU32 info = DRF_VAL(_PBUS, _SW_SCRATCH_GSP_FMC_ERROR, _GENERIC_ADDITIONAL_INFO, errorCode);
            NV_PRINTF(LEVEL_ERROR,
                      "Fatal GSP-FMC Error: version=0x%x, partition=0x%x, error code=0x%x, additional info=0x%x\n",
                      version, partitionID, err, info);
            break;
        }
        default:
        {
            NV_PRINTF(LEVEL_ERROR, "Fatal Unknown GSP-FMC Error: 0x%x\n", errorCode);
            break;
        }
    }
}

