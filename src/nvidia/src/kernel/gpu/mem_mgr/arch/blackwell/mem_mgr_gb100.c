/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "blackwell/gb100/hwproject.h"

NvBool
memmgrIsFlaSysmemSupported_GB100(OBJGPU *pGpu, MemoryManager *pMemoryManager)
{
    // FLA+EGM is preferred over FLA+sysmem since the latter has very low bandwidth
    return (!IS_SILICON(pGpu) || pMemoryManager->bForceEnableFlaSysmem) &&
        !memmgrIsLocalEgmEnabled(pMemoryManager);
}

NvBool
memmgrIsMemDescSupportedByFla_GB100
(
    OBJGPU *pGpu,
    MemoryManager *pMemoryManager,
    MEMORY_DESCRIPTOR *pMemDesc
)
{
    NV_ADDRESS_SPACE addrSpace = memdescGetAddressSpace(pMemDesc);

    if ((addrSpace == ADDR_SYSMEM) &&
        memmgrIsFlaSysmemSupported_HAL(pGpu, pMemoryManager))
    {
        return NV_TRUE;
    }

    return memmgrIsMemDescSupportedByFla_GA100(pGpu, pMemoryManager, pMemDesc);
}

NvU32
memmgrGetFBEndReserveSizeEstimate_GB100
(
    OBJGPU *pGpu,
    MemoryManager *pMemoryManager
)
{
    const NvU32 ESTIMATED_RESERVE_FB = 0x220000;

    return ESTIMATED_RESERVE_FB;
}

NvU8
memmgrGetLocalizedOffset_GB100
(
    OBJGPU *pGpu,
    MemoryManager *pMemoryManager
)
{
    return NV_LOCALIZATION_MODE_BIT_IN_ADDRESS_OFFSET;
}
