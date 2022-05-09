/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/mem_sys/kern_mem_sys.h"

#include "published/turing/tu102/dev_vm.h"

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
