/*
 * SPDX-FileCopyrightText: Copyright (c) 2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/mem_mgr/mem_mgr.h"
#include "class/clc8b5.h"  // HOPPER_DMA_COPY_A


/*!
 * Function to verify which memory scubber (fast/slow) has to use in Hopper+ for scrubbering 
 *
 * @param[in]     copyClass             DMA ccopy class
 * @param[in]     bIsVirtualMemory      Memory type
 * @param[in]     addr                  Offset of address
 * @param[in]     size                  Size of the region to scrub
 * @param[in]     dstAddressSpace       DST address space
 *
 * @returns       NV_TRUE  - Use memory fast scrbber for scrubbing 
 *                NV_FALSE - Use slow scrubber
 */
NvBool
memmgrMemUtilsCheckMemoryFastScrubEnable_GH100
(
    OBJGPU          *pGpu,
    MemoryManager   *pMemoryManager,
    NvU32            copyClass,
    NvBool           bIsVirtualMemory,
    RmPhysAddr       addr,
    NvU32            size,
    NV_ADDRESS_SPACE dstAddressSpace
)
{
    if (!memmgrIsFastScrubberEnabled(pMemoryManager))
    {
        return NV_FALSE;
    }

    //
    // Enable the  memory fast scrubbing only when
    // Class == HOPPER_DMA_COPY_A or later
    // DstType == PHYSICAL
    // DstPhysMode.target == LOCAL_FB
    // Address is 4KB aligned
    // LineLength is 4KB aligned
    //
    return  ((copyClass == HOPPER_DMA_COPY_A 
             ) &&
             (!bIsVirtualMemory)                                                    &&
             (dstAddressSpace == ADDR_FBMEM)                                        &&
             (NV_IS_ALIGNED64(addr, MEMUTIL_SCRUB_OFFSET_ALIGNMENT))                &&
             (NV_IS_ALIGNED(size, MEMUTIL_SCRUB_LINE_LENGTH_ALIGNMENT)));
}
