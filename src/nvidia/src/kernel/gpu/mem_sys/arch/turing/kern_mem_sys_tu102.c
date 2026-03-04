/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_kern_mem_sys_nvoc.h"
#include "gpu/gpu.h"
#include "gpu/mem_sys/kern_mem_sys.h"
#include "gpu/bus/kern_bus.h"
#include "gpu/bif/kernel_bif.h"

#include "nvtypes.h"
#include "published/turing/tu102/dev_vm.h"
#include "published/turing/tu102/hwproject.h"
#include "published/turing/tu102/dev_fbpa.h"
#include "published/turing/tu102/dev_fb.h"
#include "published/turing/tu102/dev_ltc.h"

void
kmemsysWriteL2SysmemInvalidateReg_TU102
(
    OBJGPU             *pGpu,
    KernelMemorySystem *pKernelMemorySystem,
    NvU32               value
)
{
    GPU_VREG_WR32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_L2_SYSMEM_INVALIDATE, value);
}

NvU32
kmemsysReadL2SysmemInvalidateReg_TU102
(
    OBJGPU             *pGpu,
    KernelMemorySystem *pKernelMemorySystem
)
{
    return GPU_VREG_RD32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_L2_SYSMEM_INVALIDATE);
}

void
kmemsysWriteL2PeermemInvalidateReg_TU102
(
    OBJGPU             *pGpu,
    KernelMemorySystem *pKernelMemorySystem,
    NvU32               value
)
{
    GPU_VREG_WR32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_L2_PEERMEM_INVALIDATE, value);
}

NvU32
kmemsysReadL2PeermemInvalidateReg_TU102
(
    OBJGPU             *pGpu,
    KernelMemorySystem *pKernelMemorySystem
)
{
    return GPU_VREG_RD32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_L2_PEERMEM_INVALIDATE);
}

NvU32
kmemsysGetMaxFbpas_TU102
(
    OBJGPU             *pGpu,
    KernelMemorySystem *pKernelMemorySystem
)
{
    return NV_SCAL_LITTER_NUM_FBPAS;
}

NvU32 kmemsysGetEccDedCountSize_TU102
(
    OBJGPU             *pGpu,
    KernelMemorySystem *pKernelMemorySystem
)
{
    return NV_PFB_FBPA_0_ECC_DED_COUNT__SIZE_1;
}

NvU32 kmemsysGetEccDedCountRegAddr_TU102
(
    OBJGPU             *pGpu,
    KernelMemorySystem *pKernelMemorySystem,
    NvU32               fbpa,
    NvU32               subp
)
{
    return NV_PFB_FBPA_0_ECC_DED_COUNT(subp) + (fbpa * NV_FBPA_PRI_STRIDE);
}

/*!
 * Utility function used to read registers and ignore PRI errors
 */
static NvU32
_kmemsysReadRegAndMaskPriError
(
    OBJGPU *pGpu,
    NvU32 regAddr
)
{
    NvU32 regVal;

    regVal = osGpuReadReg032(pGpu, regAddr);
    if (regVal == GPU_REG_VALUE_INVALID)
        return 0;

    if ((regVal & GPU_READ_PRI_ERROR_MASK) == GPU_READ_PRI_ERROR_CODE)
        return 0;

    return regVal;
}

NvU32
kmemsysGetL2EccDedCountRegAddr_TU102
(
    OBJGPU             *pGpu,
    KernelMemorySystem *pKernelMemorySystem,
    NvU32               fbpa,
    NvU32               subp
)
{
    return (NV_PLTCG_LTC0_LTS0_L2_CACHE_ECC_UNCORRECTED_ERR_COUNT +
            (fbpa * NV_LTC_PRI_STRIDE) + (subp * NV_LTS_PRI_STRIDE));
}

void
kmemsysGetEccCounts_TU102
(
    OBJGPU *pGpu,
    KernelMemorySystem *pKernelMemorySystem,
    NvU32 *dramCount,
    NvU32 *ltcCount
)
{
    NvU32 maxFbpas = kmemsysGetMaxFbpas_HAL(pGpu, pKernelMemorySystem);
    NvU32 dedCountSize = kmemsysGetEccDedCountSize_HAL(pGpu, pKernelMemorySystem);
    NvU32 fbpaDedCountRegAddr = 0;
    NvU32 ltcDedCountRegAddr = 0;
    NvU32 regVal;

    if (dramCount == NULL || ltcCount == NULL)
    {
        return;
    }

    *dramCount = 0;
    *ltcCount = 0;

    for (NvU32 i = 0; i < maxFbpas; i++)
    {
        for (NvU32 j = 0; j < dedCountSize; j++)
        {
            // DRAM count read
            fbpaDedCountRegAddr = kmemsysGetEccDedCountRegAddr_HAL(pGpu, pKernelMemorySystem, i, j);
            *dramCount += _kmemsysReadRegAndMaskPriError(pGpu, fbpaDedCountRegAddr);

            // LTC count read

            ltcDedCountRegAddr = kmemsysGetL2EccDedCountRegAddr_HAL(pGpu, pKernelMemorySystem, i, j);
            regVal = _kmemsysReadRegAndMaskPriError(pGpu, ltcDedCountRegAddr);
            *ltcCount += DRF_VAL(_PLTCG_LTC0_LTS0, _L2_CACHE_ECC, _UNCORRECTED_ERR_COUNT_UNIQUE, regVal);
        }
    }
}
