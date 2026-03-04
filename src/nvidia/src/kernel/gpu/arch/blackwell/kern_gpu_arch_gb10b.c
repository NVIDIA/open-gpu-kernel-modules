/*
 * SPDX-FileCopyrightText: Copyright (c) 2025 NVIDIA CORPORATION & AFFILIATES
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

#include "gpu/gpu_arch.h"
#include "published/blackwell/gb10b/hwproject.h"

NvU32 gpuarchGetSystemPhysAddrWidth_GB10B(GpuArch *pGpuArch)
{
    return NV_CHIP_EXTENDED_SYSTEM_PHYSICAL_ADDRESS_BITS;
}

NvU32 gpuarchGetDmaAddrWidth_GB10B(GpuArch *pGpuArch)
{
    //
    // This is to make sure that the Instance Block allocations happen within
    // the 40-bit range.
    //
    // TODO: can this be handled with a temporary `osDmaSetAddressSize()` like
    //       flush buffer allocations?
    //
    return NV_CHIP_EXTENDED_SYSTEM_PHYSICAL_ADDRESS_BITS - 1;
}
