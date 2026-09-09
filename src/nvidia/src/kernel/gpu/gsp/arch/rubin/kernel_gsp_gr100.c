/*
 * SPDX-FileCopyrightText: Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * Provides GR100+ specific KernelGsp HAL implementations.
 */

#include "rmconfig.h"

#include "gpu/gsp/kernel_gsp.h"

#include "published/rubin/gr100/dev_gsp.h"
#include "published/rubin/gr100/dev_vm.h"

#define GSP_FW_HEAP_SIZE_GR100_BAREMETAL_MIN_MB (256U)

NvU64
kgspGetLibos3BaremetalMinWprHeapSizeMB_GR100
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp
)
{
    //
    // Self-hosted mode is cached only after GSP-RM boots, but the WPR heap size
    // is selected before boot. Self-hosted GR100 platforms are AArch64.
    //
    if (pGpu->busInfo.oorArch == OOR_ARCH_AARCH64)
    {
        // Temporary WAR for Bug 6492810 while investigating GSP-RM heap fragmentation.
        return GSP_FW_HEAP_SIZE_GR100_BAREMETAL_MIN_MB;
    }

    return GSP_FW_HEAP_SIZE_OVERRIDE_LIBOS3_BAREMETAL_MIN_MB;
}

/*!
 * Create an IoAperture for reading/writing PF GSP_SCRATCH registers.
 *
 * @param[out] ppAperture  Receives the created IoAperture; set to NULL on failure
 *
 * @return NV_OK or NV_ERR_*
 */
NV_STATUS
kgspGetGspScratchAperture_GR100
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    IoAperture **ppAperture
)
{
    NV_STATUS   status = NV_OK;
    NvU64       baseAddr;
    NvU32       size;

    NV_ASSERT_OR_RETURN(ppAperture != NULL, NV_ERR_INVALID_ARGUMENT);
    *ppAperture = NULL;

    baseAddr = DRF_BASE(NV_VIRTUAL_FUNCTION_PHYS_OFFSET_REGION1) - 
               DRF_BASE(NV_VIRTUAL_FUNCTION_REGION5);

    size = NV_VIRTUAL_FUNCTION_REGION5_GSP_SCRATCH_RW__SIZE_1 * sizeof(NvU32);

    status = objCreate(ppAperture, pGpu, IoAperture,
                      pGpu->pIOApertures[DEVICE_INDEX_GPU], NULL, 0, 0,
                      NULL, 0, baseAddr, size);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to create GSP_SCRATCH IoAperture: status=0x%x\n", status);
    }
    return status;
}

/*!
 * Release the GSP_SCRATCH IoAperture
 */
void
kgspReleaseGspScratchAperture_GR100
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    IoAperture **ppAperture
)
{
    if (*ppAperture != NULL)
    {
        objDelete(*ppAperture);
        *ppAperture = NULL;
    }
}
