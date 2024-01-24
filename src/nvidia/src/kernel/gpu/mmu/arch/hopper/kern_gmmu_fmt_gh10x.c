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
#if !defined(SRT_BUILD)
#define NVOC_KERN_GMMU_H_PRIVATE_ACCESS_ALLOWED
#include "gpu/mmu/kern_gmmu.h"
#endif
#include "mmu/gmmu_fmt.h"
#include "published/hopper/gh100/dev_mmu.h"
#include "published/hopper/gh100/hwproject.h"

/*!
 *  NUM_VA_BITS = 57
 *
 *  PD4[56]
 *  |
 *  v
 *  PD3 [55:47]
 *  |
 *  v
 *  PD2 [46:38]
 *  |
 *  v
 *  PD1 [37:29] / PT_512M [37:29] (512MB page)
 *  |
 *  v
 *  PD0 [28:21] / PT_LARGE [28:21] (2MB page)
 *  |                       \
 *  |                        \
 *  v                         v
 *  PT_SMALL       PT_BIG (64KB page)
 *  [20:12]               [20:16]
 */
void kgmmuFmtInitLevels_GH10X(KernelGmmu    *pKernelGmmu,
                              MMU_FMT_LEVEL *pLevels,
                              const NvU32    numLevels,
                              const NvU32    version,
                              const NvU32    bigPageShift)
{
    NV_ASSERT_OR_RETURN_VOID(version == GMMU_FMT_VERSION_3);
    NV_ASSERT_OR_RETURN_VOID(numLevels >= 7);

    // Page directory 4 (root).
    pLevels[0].virtAddrBitHi  = 56;
    pLevels[0].virtAddrBitLo  = 56;
    pLevels[0].entrySize      = NV_MMU_VER3_PDE__SIZE;
    pLevels[0].numSubLevels   = 1;
    pLevels[0].subLevels      = pLevels + 1;
    pLevels[0].pageLevelIdTag = MMU_FMT_PT_SURF_ID_PD0;

    // Page directory 3.
    pLevels[1].virtAddrBitHi  = 55;
    pLevels[1].virtAddrBitLo  = 47;
    pLevels[1].entrySize      = NV_MMU_VER3_PDE__SIZE;
    pLevels[1].numSubLevels   = 1;
    pLevels[1].subLevels      = pLevels + 2;
    pLevels[1].pageLevelIdTag = MMU_FMT_PT_SURF_ID_PD1;

    // Page directory 2.
    pLevels[2].virtAddrBitHi  = 46;
    pLevels[2].virtAddrBitLo  = 38;
    pLevels[2].entrySize      = NV_MMU_VER3_PDE__SIZE;
    pLevels[2].numSubLevels   = 1;
    pLevels[2].subLevels      = pLevels + 3;
    pLevels[2].pageLevelIdTag = MMU_FMT_PT_SURF_ID_PD2;

    // Page directory 1.
    pLevels[3].virtAddrBitHi  = 37;
    pLevels[3].virtAddrBitLo  = 29;
    pLevels[3].entrySize      = NV_MMU_VER3_PDE__SIZE;
    pLevels[3].numSubLevels   = 1;
    pLevels[3].subLevels      = pLevels + 4;
    // Page directory 1 can hold a PTE pointing to a 512MB Page
    pLevels[3].bPageTable     = NV_TRUE;
    pLevels[3].pageLevelIdTag = MMU_FMT_PT_SURF_ID_PD3;

    // Page directory 0.
    pLevels[4].virtAddrBitHi  = 28;
    pLevels[4].virtAddrBitLo  = 21;
    pLevels[4].entrySize      = NV_MMU_VER3_DUAL_PDE__SIZE;
    pLevels[4].numSubLevels   = 2;
    pLevels[4].bPageTable     = NV_TRUE;
    pLevels[4].subLevels      = pLevels + 5;
    pLevels[4].pageLevelIdTag = MMU_FMT_PT_SURF_ID_PD4;

    // Big page table.
    pLevels[5].virtAddrBitHi  = 20;
    pLevels[5].virtAddrBitLo  = (NvU8)bigPageShift;
    pLevels[5].entrySize      = NV_MMU_VER3_PTE__SIZE;
    pLevels[5].bPageTable     = NV_TRUE;
    pLevels[5].pageLevelIdTag = MMU_FMT_PT_SURF_ID_PT_BIG;

    // Small page table.
    pLevels[6].virtAddrBitHi  = 20;
    pLevels[6].virtAddrBitLo  = 12;
    pLevels[6].entrySize      = NV_MMU_VER3_PTE__SIZE;
    pLevels[6].bPageTable     = NV_TRUE;
    pLevels[6].pageLevelIdTag = MMU_FMT_PT_SURF_ID_PT_4K;
}

void kgmmuFmtInitPdeMulti_GH10X(KernelGmmu                *pKernelGmmu,
                                GMMU_FMT_PDE_MULTI        *pPdeMulti,
                                const NvU32                version,
                                const NV_FIELD_ENUM_ENTRY *pPdeApertures)
{
    GMMU_FMT_PDE *pPdeBig   = &pPdeMulti->subLevels[0];
    GMMU_FMT_PDE *pPdeSmall = &pPdeMulti->subLevels[1];

    NV_ASSERT_OR_RETURN_VOID(version == GMMU_FMT_VERSION_3);

    // Dual PDE - big part.
    pPdeBig->version = GMMU_FMT_VERSION_3;
    INIT_FIELD_APERTURE(&pPdeBig->fldAperture, NV_MMU_VER3_DUAL_PDE_APERTURE_BIG,
                        pPdeApertures);
    INIT_FIELD_ADDRESS(&pPdeBig->fldAddr, NV_MMU_VER3_DUAL_PDE_ADDRESS_BIG,
                       NV_MMU_VER3_DUAL_PDE_ADDRESS_BIG_SHIFT);
    INIT_FIELD_DESC32(&pPdeBig->fldPdePcf, NV_MMU_VER3_DUAL_PDE_PCF_BIG);

    // Dual PDE - small part.
    pPdeSmall->version = GMMU_FMT_VERSION_3;
    INIT_FIELD_APERTURE(&pPdeSmall->fldAperture, NV_MMU_VER3_DUAL_PDE_APERTURE_SMALL,
                        pPdeApertures);
    INIT_FIELD_ADDRESS(&pPdeSmall->fldAddr, NV_MMU_VER3_DUAL_PDE_ADDRESS_SMALL,
                       NV_MMU_VER3_DUAL_PDE_ADDRESS_SHIFT);
    INIT_FIELD_DESC32(&pPdeSmall->fldPdePcf, NV_MMU_VER3_DUAL_PDE_PCF_SMALL);
}

void kgmmuFmtInitPde_GH10X(KernelGmmu *pKernelGmmu,
                           GMMU_FMT_PDE *pPde,
                           const NvU32 version,
                           const NV_FIELD_ENUM_ENTRY *pPdeApertures)
{
    NV_ASSERT_OR_RETURN_VOID(version == GMMU_FMT_VERSION_3);

    pPde->version = GMMU_FMT_VERSION_3;
    INIT_FIELD_APERTURE(&pPde->fldAperture, NV_MMU_VER3_PDE_APERTURE, pPdeApertures);
    INIT_FIELD_DESC32(&pPde->fldPdePcf, NV_MMU_VER3_PDE_PCF);
    INIT_FIELD_ADDRESS(&pPde->fldAddr, NV_MMU_VER3_PDE_ADDRESS, NV_MMU_VER3_PDE_ADDRESS_SHIFT);
}

void kgmmuFmtInitPte_GH10X(KernelGmmu *pKernelGmmu,
                           GMMU_FMT_PTE *pPte,
                           const NvU32 version,
                           const NV_FIELD_ENUM_ENTRY *pPteApertures,
                           const NvBool bUnifiedAperture)
{
    NV_ASSERT_OR_RETURN_VOID(version == GMMU_FMT_VERSION_3);

    pPte->version = GMMU_FMT_VERSION_3;
    INIT_FIELD_BOOL(&pPte->fldValid, NV_MMU_VER3_PTE_VALID);
    INIT_FIELD_APERTURE(&pPte->fldAperture, NV_MMU_VER3_PTE_APERTURE, pPteApertures);
    INIT_FIELD_DESC32(&pPte->fldPeerIndex, NV_MMU_VER3_PTE_PEER_ID);
    INIT_FIELD_DESC32(&pPte->fldKind, NV_MMU_VER3_PTE_KIND);
    INIT_FIELD_DESC32(&pPte->fldPtePcf, NV_MMU_VER3_PTE_PCF);

    INIT_FIELD_ADDRESS(&pPte->fldAddrVidmem, NV_MMU_VER3_PTE_ADDRESS_VID, NV_MMU_VER3_PTE_ADDRESS_SHIFT);
    INIT_FIELD_ADDRESS(&pPte->fldAddrPeer, NV_MMU_VER3_PTE_ADDRESS_PEER, NV_MMU_VER3_PTE_ADDRESS_SHIFT);
    INIT_FIELD_ADDRESS(&pPte->fldAddrSysmem, NV_MMU_VER3_PTE_ADDRESS_SYS, NV_MMU_VER3_PTE_ADDRESS_SHIFT);
}
