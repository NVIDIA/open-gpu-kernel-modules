/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#if !defined(SRT_BUILD)
#define NVOC_KERN_GMMU_H_PRIVATE_ACCESS_ALLOWED
#include "gpu/mmu/kern_gmmu.h"
#endif
#include "mmu/gmmu_fmt.h"
#include "published/pascal/gp100/dev_mmu.h"

/*!
 *  PD3 [48:47]
 *  |
 *  v
 *  PD2 [46:38]
 *  |
 *  v
 *  PD1 [37:29]
 *  |
 *  v
 *  PD0 [28:21] / PT_LARGE [28:21] (2MB page)
 *  |        \
 *  |         \
 *  v          v
 *  PT_SMALL  PT_BIG (64KB page)
 *  [20:12]   [20:16]
 */
void kgmmuFmtInitLevels_GP10X(KernelGmmu    *pKernelGmmu,
                              MMU_FMT_LEVEL *pLevels,
                              const NvU32    numLevels,
                              const NvU32    version,
                              const NvU32    bigPageShift)
{
    NV_ASSERT_OR_RETURN_VOID(version == GMMU_FMT_VERSION_2);
    NV_ASSERT_OR_RETURN_VOID(numLevels >= 6);
    NV_ASSERT_OR_RETURN_VOID(bigPageShift == 16 || bigPageShift == 17);

    // Page directory 3 (root).
    pLevels[0].virtAddrBitHi  = 48;
    pLevels[0].virtAddrBitLo  = 47;
    pLevels[0].entrySize      = NV_MMU_VER2_PDE__SIZE;
    pLevels[0].numSubLevels   = 1;
    pLevels[0].subLevels      = pLevels + 1;
    pLevels[0].pageLevelIdTag = MMU_FMT_PT_SURF_ID_PD0;

    // Page directory 2.
    pLevels[1].virtAddrBitHi  = 46;
    pLevels[1].virtAddrBitLo  = 38;
    pLevels[1].entrySize      = NV_MMU_VER2_PDE__SIZE;
    pLevels[1].numSubLevels   = 1;
    pLevels[1].subLevels      = pLevels + 2;
    pLevels[1].pageLevelIdTag = MMU_FMT_PT_SURF_ID_PD1;

    // Page directory 1.
    pLevels[2].virtAddrBitHi  = 37;
    pLevels[2].virtAddrBitLo  = 29;
    pLevels[2].entrySize      = NV_MMU_VER2_PDE__SIZE;
    pLevels[2].numSubLevels   = 1;
    pLevels[2].subLevels      = pLevels + 3;
    pLevels[2].pageLevelIdTag = MMU_FMT_PT_SURF_ID_PD2;

    // Page directory 0.
    pLevels[3].virtAddrBitHi  = 28;
    pLevels[3].virtAddrBitLo  = 21;
    pLevels[3].entrySize      = NV_MMU_VER2_DUAL_PDE__SIZE;
    pLevels[3].numSubLevels   = 2;
    pLevels[3].bPageTable     = NV_TRUE;
    pLevels[3].subLevels      = pLevels + 4;
    pLevels[3].pageLevelIdTag = MMU_FMT_PT_SURF_ID_PD3;


    // Big page table.
    pLevels[4].virtAddrBitHi  = 20;
    pLevels[4].virtAddrBitLo  = (NvU8)bigPageShift;
    pLevels[4].entrySize      = NV_MMU_VER2_PTE__SIZE;
    pLevels[4].bPageTable     = NV_TRUE;
    pLevels[4].pageLevelIdTag = MMU_FMT_PT_SURF_ID_PT_BIG;

    // Small page table.
    pLevels[5].virtAddrBitHi  = 20;
    pLevels[5].virtAddrBitLo  = 12;
    pLevels[5].entrySize      = NV_MMU_VER2_PTE__SIZE;
    pLevels[5].bPageTable     = NV_TRUE;
    pLevels[5].pageLevelIdTag = MMU_FMT_PT_SURF_ID_PT_4K;
}

void kgmmuFmtInitPdeMulti_GP10X(KernelGmmu                *pKernelGmmu,
                                GMMU_FMT_PDE_MULTI        *pPdeMulti,
                                const NvU32                version,
                                const NV_FIELD_ENUM_ENTRY *pPdeApertures)
{
    GMMU_FMT_PDE *pPdeBig   = &pPdeMulti->subLevels[0];
    GMMU_FMT_PDE *pPdeSmall = &pPdeMulti->subLevels[1];

    NV_ASSERT_OR_RETURN_VOID(version == GMMU_FMT_VERSION_2);

    // Dual PDE - big part.
    pPdeBig->version = GMMU_FMT_VERSION_2;
    INIT_FIELD_APERTURE(&pPdeBig->fldAperture, NV_MMU_VER2_DUAL_PDE_APERTURE_BIG,
                        pPdeApertures);
    INIT_FIELD_ADDRESS(&pPdeBig->fldAddrVidmem, NV_MMU_VER2_DUAL_PDE_ADDRESS_BIG_VID,
                       NV_MMU_VER2_DUAL_PDE_ADDRESS_BIG_SHIFT);
    INIT_FIELD_ADDRESS(&pPdeBig->fldAddrSysmem, NV_MMU_VER2_DUAL_PDE_ADDRESS_BIG_SYS,
                       NV_MMU_VER2_DUAL_PDE_ADDRESS_BIG_SHIFT);
    INIT_FIELD_BOOL(&pPdeBig->fldVolatile, NV_MMU_VER2_DUAL_PDE_VOL_BIG);

    // Dual PDE - small part.
    pPdeSmall->version = GMMU_FMT_VERSION_2;
    INIT_FIELD_APERTURE(&pPdeSmall->fldAperture, NV_MMU_VER2_DUAL_PDE_APERTURE_SMALL,
                        pPdeApertures);
    INIT_FIELD_ADDRESS(&pPdeSmall->fldAddrVidmem, NV_MMU_VER2_DUAL_PDE_ADDRESS_SMALL_VID,
                       NV_MMU_VER2_DUAL_PDE_ADDRESS_SHIFT);
    INIT_FIELD_ADDRESS(&pPdeSmall->fldAddrSysmem, NV_MMU_VER2_DUAL_PDE_ADDRESS_SMALL_SYS,
                       NV_MMU_VER2_DUAL_PDE_ADDRESS_SHIFT);
    INIT_FIELD_BOOL(&pPdeSmall->fldVolatile, NV_MMU_VER2_DUAL_PDE_VOL_SMALL);
}

void kgmmuFmtInitPde_GP10X(KernelGmmu *pKernelGmmu,
                           GMMU_FMT_PDE *pPde,
                           const NvU32 version,
                           const NV_FIELD_ENUM_ENTRY *pPdeApertures)
{
    NV_ASSERT_OR_RETURN_VOID(version == GMMU_FMT_VERSION_2);

    pPde->version = GMMU_FMT_VERSION_2;
    INIT_FIELD_APERTURE(&pPde->fldAperture, NV_MMU_VER2_PDE_APERTURE,
                        pPdeApertures);
    INIT_FIELD_ADDRESS(&pPde->fldAddrVidmem, NV_MMU_VER2_PDE_ADDRESS_VID,
                       NV_MMU_VER2_PDE_ADDRESS_SHIFT);
    INIT_FIELD_ADDRESS(&pPde->fldAddrSysmem, NV_MMU_VER2_PDE_ADDRESS_SYS,
                       NV_MMU_VER2_PDE_ADDRESS_SHIFT);
    INIT_FIELD_BOOL(&pPde->fldVolatile, NV_MMU_VER2_PDE_VOL);
}

void kgmmuFmtInitPte_GP10X(KernelGmmu *pKernelGmmu,
                           GMMU_FMT_PTE *pPte,
                           const NvU32 version,
                           const NV_FIELD_ENUM_ENTRY *pPteApertures,
                           const NvBool bUnifiedAperture)
{
    NV_ASSERT_OR_RETURN_VOID(version == GMMU_FMT_VERSION_2);

    pPte->version = GMMU_FMT_VERSION_2;
    INIT_FIELD_BOOL(&pPte->fldValid, NV_MMU_VER2_PTE_VALID);
    INIT_FIELD_APERTURE(&pPte->fldAperture, NV_MMU_VER2_PTE_APERTURE, pPteApertures);
    INIT_FIELD_DESC32(&pPte->fldPeerIndex, NV_MMU_VER2_PTE_ADDRESS_VID_PEER);
    INIT_FIELD_BOOL(&pPte->fldVolatile, NV_MMU_VER2_PTE_VOL);
    INIT_FIELD_BOOL(&pPte->fldReadOnly, NV_MMU_VER2_PTE_READ_ONLY);
    INIT_FIELD_BOOL(&pPte->fldPrivilege, NV_MMU_VER2_PTE_PRIVILEGE);
    INIT_FIELD_BOOL(&pPte->fldEncrypted, NV_MMU_VER2_PTE_ENCRYPTED);
    INIT_FIELD_BOOL(&pPte->fldAtomicDisable, NV_MMU_VER2_PTE_ATOMIC_DISABLE);
    INIT_FIELD_DESC32(&pPte->fldKind, NV_MMU_VER2_PTE_KIND);

    INIT_FIELD_ADDRESS(&pPte->fldAddrVidmem, NV_MMU_VER2_PTE_ADDRESS_VID, NV_MMU_VER2_PTE_ADDRESS_SHIFT);

    INIT_FIELD_ADDRESS(&pPte->fldAddrPeer, NV_MMU_VER2_PTE_ADDRESS_VID, NV_MMU_VER2_PTE_ADDRESS_SHIFT);

    if (bUnifiedAperture)
    {
        //
        // On Tegra, use the vidmem address field descriptor for sysmem as well.
        // This is to avoid clobbering the comptags in PTE.
        //
        // Comptag fields overlap with the upper bits of the sysmem physical address.
        // Sysmem compression is supported on Tegra since we have a unified aperture.
        // The physical address width supported by Tegra corresponds to the vidmem aperture.
        // So the upper bits of sysmem can be safely used for compression.
        //
        INIT_FIELD_ADDRESS(&pPte->fldAddrSysmem, NV_MMU_VER2_PTE_ADDRESS_VID, NV_MMU_VER2_PTE_ADDRESS_SHIFT);
    }
    else
    {
        INIT_FIELD_ADDRESS(&pPte->fldAddrSysmem, NV_MMU_VER2_PTE_ADDRESS_SYS, NV_MMU_VER2_PTE_ADDRESS_SHIFT);
    }
}

void kgmmuFmtInitPteComptagLine_GP10X(KernelGmmu *pKernelGmmu,
                                      GMMU_FMT_PTE *pPte,
                                      const NvU32 version)
{
    NV_ASSERT_OR_RETURN_VOID(version == GMMU_FMT_VERSION_2);
    INIT_FIELD_DESC32(&pPte->fldCompTagLine, NV_MMU_VER2_PTE_COMPTAGLINE);
}
