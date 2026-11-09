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

#include "gpu/mmu/kern_gmmu.h"

/*!
 * @brief Perform TLB invalidate operation if necessary after an IOMMU map
 *
 * @param[in] pGpu                OBJGPU pointer
 * @param[in] pKernelGmmu         KernelGmmu pointer
 *
 * @returns NV_STATUS - Status of the cache operation
 */

NV_STATUS
kgmmuTlbInvalidatePostIommuMap_GR100
(
    OBJGPU *pGpu,
    KernelGmmu *pKernelGmmu
)
{
    // WAR 4971020
    if (gpuIsSelfHosted(pGpu) &&
        (osGetPageSize() == 4096))
    {
        // VASPACE_FLAGS_BAR here means HUBTLB_ONLY gets set
        return kgmmuInvalidateTlb_HAL(pGpu, pKernelGmmu, NULL,
                                      VASPACE_FLAGS_BAR, PTE_UPGRADE,
                                      0, NV_GMMU_INVAL_SCOPE_ALL_TLBS, NV_FALSE);
    }
    return NV_OK;
}
