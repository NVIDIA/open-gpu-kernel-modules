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

#define NVOC_KERN_GMMU_H_PRIVATE_ACCESS_ALLOWED

#include "gpu/mmu/kern_gmmu.h"

/*!
 * Initialize the GMMU format families.
 */
NV_STATUS
kgmmuFmtFamiliesInit_GM200(OBJGPU *pGpu, KernelGmmu *pKernelGmmu)
{
    NvU32       i;
    NvU32       v;

    for (v = 0; v < GMMU_FMT_MAX_VERSION_COUNT; ++v)
    {
        GMMU_FMT_FAMILY *pFam = pKernelGmmu->pFmtFamilies[v];
        if (NULL != pFam)
        {
            //
            // GM20X supports sparse directly in HW by setting the
            // volatile bit when the valid bit is clear.
            //
            nvFieldSetBool(&pFam->pte.fldValid,    NV_FALSE, pFam->sparsePte.v8);
            nvFieldSetBool(&pFam->pte.fldVolatile, NV_TRUE,  pFam->sparsePte.v8);

            // PDEs are similar but use aperture fields for validity.
            if (nvFieldIsValid32(&pFam->pde.fldVolatile.desc))
            {
                gmmuFieldSetAperture(&pFam->pde.fldAperture, GMMU_APERTURE_INVALID,
                                     pFam->sparsePde.v8);
                nvFieldSetBool(&pFam->pde.fldVolatile, NV_TRUE, pFam->sparsePde.v8);
            }

            // Multi-PDE case uses sub-level 0 (big page table) volatile bit.
            for (i = 0; i < MMU_FMT_MAX_SUB_LEVELS; ++i)
            {
                const GMMU_FMT_PDE *pPdeFmt = &pFam->pdeMulti.subLevels[i];
                if (nvFieldIsValid32(&pPdeFmt->fldVolatile.desc))
                {
                    gmmuFieldSetAperture(&pPdeFmt->fldAperture, GMMU_APERTURE_INVALID,
                                         pFam->sparsePdeMulti.v8);
                    if (0 == i)
                    {
                        nvFieldSetBool(&pPdeFmt->fldVolatile, NV_TRUE, pFam->sparsePdeMulti.v8);
                    }
                }
            }
        }
    }

    return NV_OK;
}
