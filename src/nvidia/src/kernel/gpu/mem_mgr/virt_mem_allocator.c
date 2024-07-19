/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/***************************** HW State Routines ***************************\
*                                                                          *
*         VirtMemAllocator Object Function Definitions.                    *
*                                                                          *
\***************************************************************************/

#include "core/core.h"
#include "core/hal.h"
#include "nvrm_registry.h"
#include "os/os.h"
#include "vgpu/rpc.h"
#include "gpu/gpu.h"
#include "gpu/mem_mgr/virt_mem_allocator.h"

#include "ctrl/ctrl0002.h"

// local functions
static NV_STATUS dmaInitRegistryOverrides(OBJGPU*, VirtMemAllocator*);

NV_STATUS
dmaConstructEngine_IMPL(OBJGPU *pGpu, VirtMemAllocator *pDma, ENGDESCRIPTOR engDesc)
{
    NV_STATUS rmStatus;

    pGpu = ENG_GET_GPU(pDma);
    rmStatus = dmaConstructHal_HAL(pGpu, pDma);
    if (rmStatus != NV_OK)
        return rmStatus;

    rmStatus = dmaInitRegistryOverrides(pGpu, pDma);
    if (rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, ", Could not apply registry overrides\n");
        DBG_BREAKPOINT();
        return rmStatus;
    }

    return dmaInit_HAL(pGpu, pDma);
}

/*!
 * @brief Initialize all registry overrides for this object
 *
 * @param[in]      pGpu  GPU object pointer
 * @param[in,out]  pDma  VirtMemAllocator object pointer
 */
static NV_STATUS
dmaInitRegistryOverrides(OBJGPU *pGpu, VirtMemAllocator *pDma)
{
    NV_STATUS rmStatus = NV_OK;

    return rmStatus;
}

NV_STATUS dmaStateInitLocked_IMPL(OBJGPU *pGpu, VirtMemAllocator *pDma)
{
    dmaInitGart_HAL(pGpu, pDma);

    return NV_OK;
}
