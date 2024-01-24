/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "kernel/gpu/intr/intr.h"
#include "gpu/gpu.h"

#include "published/ampere/ga102/dev_vm.h"

//
// Ampere HAL routines that access NV_VIRTUAL_FUNCTION_* registers.
// Every function in this file needs to have a GSP equivalent
// accessing NV_GSP_INTR_* registers.
//

NvU32
intrReadRegTopEnSet_CPU_GA102
(
    OBJGPU             *pGpu,
    Intr               *pIntr,
    NvU32               regIndex,
    THREAD_STATE_NODE  *pThreadState
)
{
    // SW assumptions that there are two top-levels.
    ct_assert(NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_EN_SET__SIZE_1 <= 2);
    if (regIndex >= NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_EN_SET__SIZE_1)
    {
        return 0x0;
    }
    return GPU_VREG_RD32_EX(pGpu,
                            NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_EN_SET(regIndex),
                            pThreadState);
}

void
intrWriteRegTopEnSet_CPU_GA102
(
    OBJGPU             *pGpu,
    Intr               *pIntr,
    NvU32               regIndex,
    NvU32               value,
    THREAD_STATE_NODE  *pThreadState
)
{
    // SW assumptions that there are two top-levels.
    ct_assert(NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_EN_SET__SIZE_1 <= 2);
    if (regIndex >= NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_EN_SET__SIZE_1)
    {
        return;
    }
    GPU_VREG_WR32_EX(pGpu,
                     NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_EN_SET(regIndex),
                     value,
                     pThreadState);
}

void
intrWriteRegTopEnClear_CPU_GA102
(
    OBJGPU             *pGpu,
    Intr               *pIntr,
    NvU32               regIndex,
    NvU32               value,
    THREAD_STATE_NODE  *pThreadState
)
{
    // SW assumptions that there are two top-levels.
    ct_assert(NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_EN_CLEAR__SIZE_1 <= 2);
    if (regIndex >= NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_EN_CLEAR__SIZE_1)
    {
        return;
    }
    GPU_VREG_WR32_EX(pGpu,
                     NV_VIRTUAL_FUNCTION_PRIV_CPU_INTR_TOP_EN_CLEAR(regIndex),
                     value,
                     pThreadState);
}
