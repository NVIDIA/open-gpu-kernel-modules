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

#include "gpu/mmu/kern_gmmu.h"
#include "gpu/mem_mgr/mem_mgr.h"

#include "mmu/gmmu_fmt.h"
#include "published/hopper/gh100/dev_mmu.h"
#include "published/hopper/gh100/dev_fault.h"

/*!
 * Check if a specific GMMU format version is supported.
 */
NvBool
kgmmuFmtIsVersionSupported_GH10X(KernelGmmu *pKernelGmmu, NvU32 version)
{
    return (version == GMMU_FMT_VERSION_3);
}

/*!
 * Initialize the GMMU format families.
 */
NV_STATUS
kgmmuFmtFamiliesInit_GH100(OBJGPU *pGpu, KernelGmmu* pKernelGmmu)
{
    NvU32            i;
    NvU32            pdePcfHw = 0;
    NvU32            pdePcfSw = 0;
    NvU32            ptePcfHw = 0;
    NvU32            ptePcfSw = 0;

    // Initialize the sparse encoding in the PDE PCF field for V3
    GMMU_FMT_FAMILY *pFam = pKernelGmmu->pFmtFamilies[GMMU_FMT_VERSION_3 - 1];
 
    if (pFam != NULL)
    {
        // 1.Initialize sparsePde 
        pdePcfSw |= (1 << SW_MMU_PCF_SPARSE_IDX);
        pdePcfSw |= (1 << SW_MMU_PCF_ATS_ALLOWED_IDX);
        NV_ASSERT_OR_RETURN((kgmmuTranslatePdePcfFromSw_HAL(pKernelGmmu, pdePcfSw, &pdePcfHw) == NV_OK),
                             NV_ERR_INVALID_ARGUMENT);
        gmmuFieldSetAperture(&pFam->pde.fldAperture, GMMU_APERTURE_INVALID,
                             pFam->sparsePde.v8);
        nvFieldSet32(&pFam->pde.fldPdePcf, pdePcfHw, pFam->sparsePde.v8);
 
        // 2.Initialize sparsePdeMulti
        for (i = 0; i < MMU_FMT_MAX_SUB_LEVELS; ++i)
        {
            const GMMU_FMT_PDE *pPdeFmt = &pFam->pdeMulti.subLevels[i];
            gmmuFieldSetAperture(&pPdeFmt->fldAperture, GMMU_APERTURE_INVALID,
                                 pFam->sparsePdeMulti.v8);
            // Set PDE PCF sparse bit only for sub-level 0 for PdeMulti
            if (i == 0)
            {
                nvFieldSet32(&pPdeFmt->fldPdePcf, pdePcfHw, pFam->sparsePdeMulti.v8);
            }
        }
 
        // 3.Initialize nv4kPte
        ptePcfSw |= (1 << SW_MMU_PCF_NV4K_IDX);
        nvFieldSetBool(&pFam->pte.fldValid, NV_FALSE, pFam->nv4kPte.v8);
        NV_ASSERT_OR_RETURN((kgmmuTranslatePtePcfFromSw_HAL(pKernelGmmu, ptePcfSw, &ptePcfHw) == NV_OK),
                             NV_ERR_INVALID_ARGUMENT);
        nvFieldSet32(&pFam->pte.fldPtePcf, ptePcfHw, pFam->nv4kPte.v8);

        // 4.Initialize sparsePte
        ptePcfSw = (1 << SW_MMU_PCF_SPARSE_IDX);
        nvFieldSetBool(&pFam->pte.fldValid, NV_FALSE, pFam->sparsePte.v8);
        NV_ASSERT_OR_RETURN((kgmmuTranslatePtePcfFromSw_HAL(pKernelGmmu, ptePcfSw, &ptePcfHw) == NV_OK),
                             NV_ERR_INVALID_ARGUMENT);
        nvFieldSet32(&pFam->pte.fldPtePcf, ptePcfHw, pFam->sparsePte.v8);
    }

    return NV_OK;
}

#define PTE_PCF_INVALID_LIST(fn) \
        fn(INVALID) \
        fn(NO_VALID_4KB_PAGE) \
        fn(SPARSE) \
        fn(MAPPING_NOWHERE)

#define PTE_PCF_VALID_LIST(fn) \
        fn(PRIVILEGE_RW_ATOMIC_CACHED_ACD) \
        fn(PRIVILEGE_RW_ATOMIC_CACHED_ACE) \
        fn(PRIVILEGE_RW_ATOMIC_UNCACHED_ACD) \
        fn(PRIVILEGE_RW_ATOMIC_UNCACHED_ACE) \
        fn(PRIVILEGE_RW_NO_ATOMIC_UNCACHED_ACE) \
        fn(PRIVILEGE_RW_NO_ATOMIC_CACHED_ACE) \
        fn(PRIVILEGE_RO_ATOMIC_UNCACHED_ACE) \
        fn(PRIVILEGE_RO_NO_ATOMIC_UNCACHED_ACE) \
        fn(PRIVILEGE_RO_NO_ATOMIC_CACHED_ACE) \
        fn(REGULAR_RW_ATOMIC_CACHED_ACD) \
        fn(REGULAR_RW_ATOMIC_CACHED_ACE) \
        fn(REGULAR_RW_ATOMIC_UNCACHED_ACD) \
        fn(REGULAR_RW_ATOMIC_UNCACHED_ACE) \
        fn(REGULAR_RW_NO_ATOMIC_CACHED_ACD) \
        fn(REGULAR_RW_NO_ATOMIC_CACHED_ACE) \
        fn(REGULAR_RW_NO_ATOMIC_UNCACHED_ACD) \
        fn(REGULAR_RW_NO_ATOMIC_UNCACHED_ACE) \
        fn(REGULAR_RO_ATOMIC_CACHED_ACD) \
        fn(REGULAR_RO_ATOMIC_CACHED_ACE) \
        fn(REGULAR_RO_ATOMIC_UNCACHED_ACD) \
        fn(REGULAR_RO_ATOMIC_UNCACHED_ACE) \
        fn(REGULAR_RO_NO_ATOMIC_CACHED_ACD) \
        fn(REGULAR_RO_NO_ATOMIC_CACHED_ACE) \
        fn(REGULAR_RO_NO_ATOMIC_UNCACHED_ACD) \
        fn(REGULAR_RO_NO_ATOMIC_UNCACHED_ACE)

#define PTE_PCF_HW_FROM_SW(name) \
        case (SW_MMU_PTE_PCF_##name): \
        { \
            *pPtePcfHw = NV_MMU_VER3_PTE_PCF_##name; \
            break; \
        }

#define PTE_PCF_SW_FROM_HW(name) \
        case (NV_MMU_VER3_PTE_PCF_##name): \
        { \
            *pPtePcfSw = SW_MMU_PTE_PCF_##name; \
            break; \
        }

//
// Takes a SW PTE PCF and translates to HW PTE PCF
// If bit patterns is not supported by HW, return NV_ERR_NOT_SUPPORTED
//
NV_STATUS
kgmmuTranslatePtePcfFromSw_GH100
(
    KernelGmmu *pKernelGmmu,
    NvU32    ptePcfSw,
    NvU32   *pPtePcfHw
)
{
    switch (ptePcfSw)
    {
        PTE_PCF_INVALID_LIST(PTE_PCF_HW_FROM_SW)
        PTE_PCF_VALID_LIST(PTE_PCF_HW_FROM_SW)

        default:
        {
            NV_PRINTF(LEVEL_ERROR, "Unsupported SW PTE PCF pattern requested : %x\n", ptePcfSw);
            return NV_ERR_NOT_SUPPORTED;
        }
    }

    return NV_OK;
}

NV_STATUS
kgmmuTranslatePtePcfFromHw_GH100
(
    KernelGmmu *pKernelGmmu,
    NvU32       ptePcfHw,
    NvBool      bPteValid,
    NvU32      *pPtePcfSw
)
{
    if (!bPteValid)
    {
        switch (ptePcfHw)
        {
            PTE_PCF_INVALID_LIST(PTE_PCF_SW_FROM_HW)

            default: return NV_ERR_NOT_SUPPORTED;
        }
    }
    else
    {
        switch (ptePcfHw)
        {
            PTE_PCF_VALID_LIST(PTE_PCF_SW_FROM_HW)

            default:
            {
                NV_PRINTF(LEVEL_ERROR, "Unsupported HW PTE PCF pattern requested : %x\n", ptePcfHw);
                return NV_ERR_NOT_SUPPORTED;
            }
        }
    }

    return NV_OK;
}

#define PDE_PCF_INVALID_LIST(fn) \
        fn(INVALID_ATS_ALLOWED) \
        fn(SPARSE_ATS_ALLOWED) \
        fn(INVALID_ATS_NOT_ALLOWED) \
        fn(SPARSE_ATS_NOT_ALLOWED)

#define PDE_PCF_VALID_LIST(fn) \
        fn(VALID_CACHED_ATS_ALLOWED) \
        fn(VALID_CACHED_ATS_NOT_ALLOWED) \
        fn(VALID_UNCACHED_ATS_ALLOWED) \
        fn(VALID_UNCACHED_ATS_NOT_ALLOWED)

#define PDE_PCF_HW_FROM_SW(name) \
        case (SW_MMU_PDE_PCF_##name): \
        { \
            *pPdePcfHw = NV_MMU_VER3_PDE_PCF_##name; \
            break; \
        }

#define PDE_PCF_SW_FROM_HW(name) \
        case (NV_MMU_VER3_PDE_PCF_##name): \
        { \
            *pPdePcfSw = SW_MMU_PDE_PCF_##name; \
            break; \
        }

//
// Takes a SW PDE PCF and translates to HW PDE PCF
// If a bit pattern is not supported by HW, return NV_ERR_NOT_SUPPORTED
//
NV_STATUS
kgmmuTranslatePdePcfFromSw_GH100
(
    KernelGmmu *pKernelGmmu,
    NvU32       pdePcfSw,
    NvU32      *pPdePcfHw
)
{
    switch (pdePcfSw)
    {
        PDE_PCF_INVALID_LIST(PDE_PCF_HW_FROM_SW)
        PDE_PCF_VALID_LIST(PDE_PCF_HW_FROM_SW)

        default: return NV_ERR_NOT_SUPPORTED;
    }

    return NV_OK;
}

//
// Takes a HW PDE PCF and translates to SW PDE PCF
// If a bit pattern is not supported by SW, return NV_ERR_NOT_SUPPORTED
//
NV_STATUS
kgmmuTranslatePdePcfFromHw_GH100
(
    KernelGmmu     *pKernelGmmu,
    NvU32           pdePcfHw,
    GMMU_APERTURE   aperture,
    NvU32          *pPdePcfSw
)
{
    if (!aperture)
    {
        switch (pdePcfHw)
        {
            PDE_PCF_INVALID_LIST(PDE_PCF_SW_FROM_HW)

            default: return NV_ERR_NOT_SUPPORTED;
        }
    }
    else
    {
        switch (pdePcfHw)
        {
            PDE_PCF_VALID_LIST(PDE_PCF_SW_FROM_HW)

            default: return NV_ERR_NOT_SUPPORTED;
        }
    }

    return NV_OK;
}

/*
 * @brief   Validates fabric base address.
 *
 * @param   pKernelGmmu
 * @param   fabricBaseAddr
 *
 * @returns On success, NV_OK.
 *          On failure, returns NV_ERR_XXX.
 */
NV_STATUS
kgmmuValidateFabricBaseAddress_GH100
(
    KernelGmmu *pKernelGmmu,
    NvU64       fabricBaseAddr
)
{
    OBJGPU        *pGpu = ENG_GET_GPU(pKernelGmmu);
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    NvU64 fbSizeBytes;

    fbSizeBytes = pMemoryManager->Ram.fbTotalMemSizeMb << 20;

    //
    // Hopper SKUs will be paired with NVSwitches (Laguna Seca) supporting 2K
    // mapslots that can cover 512GB each. Make sure that the fabric base
    // address being used is valid to cover whole frame buffer.
    //

    // Check if fabric address is aligned to mapslot size.
    if (fabricBaseAddr & (NVBIT64(39) - 1))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Align fbSize to mapslot size.
    fbSizeBytes = RM_ALIGN_UP(fbSizeBytes, NVBIT64(39));

    return NV_OK;
}

/*!
 * @brief Get the engine ID associated with the Graphics Engine
 */
NvU32
kgmmuGetGraphicsEngineId_GH100
(
    KernelGmmu *pKernelGmmu
)
{
    return NV_PFAULT_MMU_ENG_ID_GRAPHICS;
}
