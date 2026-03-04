/*
 * SPDX-FileCopyrightText: Copyright (c) 2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/gpu.h"
#include "gpu/bif/kernel_bif.h"

#include "published/blackwell/gb10b/dev_xtl_ep_pcfg_gpu.h"

void
kbifReadPcieCplCapsFromConfigSpace_GB10B
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif,
    NvU32     *pBifAtomicsmask
)
{
    NvU32 deviceCaps2;

    NV_ASSERT_OR_RETURN_VOID(pBifAtomicsmask != NULL);

    if (GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_EP_PCFG_GPU_DEVICE_CAPABILITIES_2, &deviceCaps2) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Unable to read NV_EP_PCFG_GPU_DEVICE_CAPABILITIES_2\n");
        return;
    }

    *pBifAtomicsmask = 0;

    if (FLD_TEST_DRF(_EP_PCFG_GPU, _DEVICE_CAPABILITIES_2, _ATOMIC_COMPLETER_32BIT, _SUPPORTED, deviceCaps2))
    {
        *pBifAtomicsmask |= BIF_PCIE_CPL_ATOMICS_FETCHADD_32;
        *pBifAtomicsmask |= BIF_PCIE_CPL_ATOMICS_SWAP_32;
        *pBifAtomicsmask |= BIF_PCIE_CPL_ATOMICS_CAS_32;
    }
    if (FLD_TEST_DRF(_EP_PCFG_GPU, _DEVICE_CAPABILITIES_2, _ATOMIC_COMPLETER_64BIT, _SUPPORTED, deviceCaps2))
    {
        *pBifAtomicsmask |= BIF_PCIE_CPL_ATOMICS_FETCHADD_64;
        *pBifAtomicsmask |= BIF_PCIE_CPL_ATOMICS_SWAP_64;
        *pBifAtomicsmask |= BIF_PCIE_CPL_ATOMICS_CAS_64;
    }
    if (FLD_TEST_DRF(_EP_PCFG_GPU, _DEVICE_CAPABILITIES_2, _CAS_COMPLETER_128BIT, _SUPPORTED, deviceCaps2))
    {
        *pBifAtomicsmask |= BIF_PCIE_CPL_ATOMICS_CAS_128;
    }
}
