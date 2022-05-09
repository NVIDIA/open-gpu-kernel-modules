/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * Provides the implementation for all GA100+ specific KernelFalcon
 * interfaces.
 */

#include "gpu/falcon/kernel_falcon.h"

#include "published/ampere/ga100/dev_falcon_v4.h"

/*!
 * Retrigger an interrupt message from the engine to the NV_CTRL tree
 */
void
kflcnIntrRetrigger_GA100
(
    OBJGPU *pGpu,
    KernelFalcon *pKernelFlcn
)
{
    kflcnRegWrite_HAL(pGpu, pKernelFlcn, NV_PFALCON_FALCON_INTR_RETRIGGER(0),
        DRF_DEF(_PFALCON, _FALCON_INTR_RETRIGGER, _TRIGGER, _TRUE));
}

/*!
 * Mask a IMEM address to have only the BLK and OFFSET bits set.
 *
 * @param[in] addr    IMEM address
 */
NvU32
kflcnMaskImemAddr_GA100
(
    OBJGPU *pGpu,
    KernelFalcon *pKernelFlcn,
    NvU32 addr
)
{
    return (addr & (DRF_SHIFTMASK(NV_PFALCON_FALCON_IMEMC_OFFS) |
                    DRF_SHIFTMASK(NV_PFALCON_FALCON_IMEMC_BLK)));
}

/*!
 * Mask a DMEM address to have only the BLK and OFFSET bits set.
 *
 * @param[in] addr    DMEM address
 */
NvU32
kflcnMaskDmemAddr_GA100
(
    OBJGPU *pGpu,
    KernelFalcon *pKernelFlcn,
    NvU32 addr
)
{
    return (addr & (DRF_SHIFTMASK(NV_PFALCON_FALCON_DMEMC_OFFS) |
                    DRF_SHIFTMASK(NV_PFALCON_FALCON_DMEMC_BLK)));
}
