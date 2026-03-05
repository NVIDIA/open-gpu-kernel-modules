/*
 * SPDX-FileCopyrightText: Copyright (c) 2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "os/nv_memory_type.h"
#include "gpu/mem_mgr/mem_mgr.h"

/*!
 * Allocate console region in CPU-RM from UEFI carveout region.
 */
NV_STATUS
memmgrAllocateConsoleRegion_T234D
(
    OBJGPU *pGpu,
    MemoryManager *pMemoryManager
)
{
    NvU64 fbBaseAddress = pMemoryManager->Ram.ReservedConsoleDispMemBase;
    NvU64 fbConsoleSize = pMemoryManager->Ram.ReservedConsoleDispMemSize;
    NV_STATUS status = NV_OK;

    if ((fbBaseAddress == 0) || (fbConsoleSize == 0))
    {
        NV_PRINTF(LEVEL_INFO, "No Frame Buffer Present\n");
        return status;
    }

    status = memdescCreate(&pMemoryManager->pReservedConsoleMemDesc,
                            pGpu,
                            fbConsoleSize,
                            RM_PAGE_SIZE,
                            NV_TRUE,
                            ADDR_SYSMEM,
                            NV_MEMORY_UNCACHED,
                            MEMDESC_FLAGS_SKIP_RESOURCE_COMPUTE |
                            MEMDESC_FLAGS_ALLOC_FROM_UEFI_CARVEOUT);

    if (status != NV_OK)
    {
        return status;
    }
    memdescDescribe(pMemoryManager->pReservedConsoleMemDesc,
                    ADDR_SYSMEM,
                    fbBaseAddress,
                    fbConsoleSize);
    memdescSetPageSize(pMemoryManager->pReservedConsoleMemDesc,
                       AT_CPU,
                       RM_PAGE_SIZE);

    NV_PRINTF(LEVEL_INFO, "Allocating console region of size: %llx, at base : %llx \n ",
              fbConsoleSize, fbBaseAddress);

    return NV_OK;
}