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
#include "published/maxwell/gm107/dev_mmu.h"

/*!
 *  64KB Big Page Size Format
 *
 *  PD [39:26]
 *  |        \
 *  |         \
 *  v          v
 *  PT_SMALL  PT_BIG
 *  [25:12]   [25:16]
 *
 *  128KB Big Page Size Format
 *
 *  PD [39:27]
 *  |        \
 *  |         \
 *  v          v
 *  PT_SMALL  PT_BIG
 *  [26:12]   [26:17]
 */
void kgmmuFmtInitLevels_GM10X(KernelGmmu    *pKernelGmmu,
                              MMU_FMT_LEVEL *pLevels,
                              const NvU32    numLevels,
                              const NvU32    version,
                              const NvU32    bigPageShift)
{
    NV_ASSERT_OR_RETURN_VOID(numLevels >= 3);
    NV_ASSERT_OR_RETURN_VOID(version == GMMU_FMT_VERSION_1);
    NV_ASSERT_OR_RETURN_VOID(bigPageShift == 16 || bigPageShift == 17);

    // Page directory (root).
    pLevels[0].virtAddrBitHi  = 39;
    pLevels[0].virtAddrBitLo  = (NvU8)bigPageShift + 10;
    pLevels[0].entrySize      = NV_MMU_PDE__SIZE;
    pLevels[0].numSubLevels   = 2;
    pLevels[0].subLevels      = pLevels + 1;
    pLevels[0].pageLevelIdTag = MMU_FMT_PT_SURF_ID_PD0;

    // Big page table.
    pLevels[1].virtAddrBitHi  = pLevels[0].virtAddrBitLo - 1;
    pLevels[1].virtAddrBitLo  = (NvU8)bigPageShift;
    pLevels[1].entrySize      = NV_MMU_PTE__SIZE;
    pLevels[1].bPageTable     = NV_TRUE;
    pLevels[1].pageLevelIdTag = MMU_FMT_PT_SURF_ID_PT_BIG;

    // Small page table.
    pLevels[2].virtAddrBitHi  = pLevels[0].virtAddrBitLo - 1;
    pLevels[2].virtAddrBitLo  = 12;
    pLevels[2].entrySize      = NV_MMU_PTE__SIZE;
    pLevels[2].bPageTable     = NV_TRUE;
    pLevels[2].pageLevelIdTag = MMU_FMT_PT_SURF_ID_PT_4K;
}

void kgmmuFmtInitPdeMulti_GM10X(KernelGmmu                *pKernelGmmu,
                                GMMU_FMT_PDE_MULTI        *pPdeMulti,
                                const NvU32                version,
                                const NV_FIELD_ENUM_ENTRY *pPdeApertures)
{
    GMMU_FMT_PDE *pPdeBig   = &pPdeMulti->subLevels[0];
    GMMU_FMT_PDE *pPdeSmall = &pPdeMulti->subLevels[1];

    NV_ASSERT_OR_RETURN_VOID(version == GMMU_FMT_VERSION_1);

    // Common PDE fields.
    INIT_FIELD_DESC32(&pPdeMulti->fldSizeRecipExp, NV_MMU_PDE_SIZE);

    // Dual PDE - big part.
    pPdeBig->version = GMMU_FMT_VERSION_1;
    INIT_FIELD_APERTURE(&pPdeBig->fldAperture, NV_MMU_PDE_APERTURE_BIG, pPdeApertures);
    INIT_FIELD_ADDRESS(&pPdeBig->fldAddrVidmem, NV_MMU_PDE_ADDRESS_BIG_VID,
                       NV_MMU_PDE_ADDRESS_SHIFT);
    INIT_FIELD_ADDRESS(&pPdeBig->fldAddrSysmem, NV_MMU_PDE_ADDRESS_BIG_SYS,
                       NV_MMU_PDE_ADDRESS_SHIFT);
    INIT_FIELD_BOOL(&pPdeBig->fldVolatile, NV_MMU_PDE_VOL_BIG);

    // Dual PDE - small part.
    pPdeSmall->version = GMMU_FMT_VERSION_1;
    INIT_FIELD_APERTURE(&pPdeSmall->fldAperture, NV_MMU_PDE_APERTURE_SMALL, pPdeApertures);
    INIT_FIELD_ADDRESS(&pPdeSmall->fldAddrVidmem, NV_MMU_PDE_ADDRESS_SMALL_VID,
                       NV_MMU_PDE_ADDRESS_SHIFT);
    INIT_FIELD_ADDRESS(&pPdeSmall->fldAddrSysmem, NV_MMU_PDE_ADDRESS_SMALL_SYS,
                       NV_MMU_PDE_ADDRESS_SHIFT);
    INIT_FIELD_BOOL(&pPdeSmall->fldVolatile, NV_MMU_PDE_VOL_SMALL);
}

void kgmmuFmtInitPte_GM10X(KernelGmmu *pKernelGmmu,
                           GMMU_FMT_PTE *pPte,
                           const NvU32 version,
                           const NV_FIELD_ENUM_ENTRY *pPteApertures,
                           const NvBool bUnifiedAperture)
{
    NV_ASSERT_OR_RETURN_VOID(version == GMMU_FMT_VERSION_1);

    pPte->version = GMMU_FMT_VERSION_1;
    INIT_FIELD_BOOL(&pPte->fldValid, NV_MMU_PTE_VALID);
    INIT_FIELD_APERTURE(&pPte->fldAperture, NV_MMU_PTE_APERTURE, pPteApertures);
    INIT_FIELD_DESC32(&pPte->fldPeerIndex, NV_MMU_PTE_ADDRESS_VID_PEER);
    INIT_FIELD_BOOL(&pPte->fldVolatile, NV_MMU_PTE_VOL);
    INIT_FIELD_BOOL(&pPte->fldReadOnly, NV_MMU_PTE_READ_ONLY);
    INIT_FIELD_BOOL(&pPte->fldPrivilege, NV_MMU_PTE_PRIVILEGE);
    INIT_FIELD_BOOL(&pPte->fldEncrypted, NV_MMU_PTE_ENCRYPTED);
    INIT_FIELD_BOOL(&pPte->fldLocked, NV_MMU_PTE_LOCK);
    INIT_FIELD_DESC32(&pPte->fldKind, NV_MMU_PTE_KIND);
    INIT_FIELD_BOOL(&pPte->fldReadDisable, NV_MMU_PTE_READ_DISABLE);
    INIT_FIELD_BOOL(&pPte->fldWriteDisable, NV_MMU_PTE_WRITE_DISABLE);
    INIT_FIELD_ADDRESS(&pPte->fldAddrVidmem, NV_MMU_PTE_ADDRESS_VID, NV_MMU_PTE_ADDRESS_SHIFT);
    INIT_FIELD_ADDRESS(&pPte->fldAddrPeer, NV_MMU_PTE_ADDRESS_VID, NV_MMU_PTE_ADDRESS_SHIFT);

    if (bUnifiedAperture)
    {
        //
        // On Tegra, Use the vidmem address field descriptor for sysmem as well.
        // This is to avoid clobbering the comptags on PTE.
        //
        // Comptag fields overlap with the upper bits of the sysmem physical address.
        // Sysmem compression is supported on Tegra since we have a unified aperture.
        // The physical address width supported by Tegra corresponds to the vidmem aperture.
        // So the upper bits of sysmem can be safely used for compression.
        //
        INIT_FIELD_ADDRESS(&pPte->fldAddrSysmem, NV_MMU_PTE_ADDRESS_VID, NV_MMU_PTE_ADDRESS_SHIFT);
    }
    else
    {
        INIT_FIELD_ADDRESS(&pPte->fldAddrSysmem, NV_MMU_PTE_ADDRESS_SYS, NV_MMU_PTE_ADDRESS_SHIFT);
    }
}

void kgmmuFmtInitPteComptagLine_GM10X(KernelGmmu  *pKernelGmmu,
                                      GMMU_FMT_PTE *pPte,
                                      const NvU32 version)
{
    NV_ASSERT_OR_RETURN_VOID(version == GMMU_FMT_VERSION_1);
    INIT_FIELD_DESC32(&pPte->fldCompTagLine, NV_MMU_PTE_COMPTAGLINE);
}

void kgmmuFmtInitPdeApertures_GM10X
(
    KernelGmmu          *pKernelGmmu,
    NV_FIELD_ENUM_ENTRY *pEntries
)
{
    nvFieldEnumEntryInit(pEntries + GMMU_APERTURE_INVALID,
                         NV_MMU_PDE_APERTURE_BIG_INVALID);

    nvFieldEnumEntryInit(pEntries + GMMU_APERTURE_VIDEO,
                         NV_MMU_PDE_APERTURE_BIG_VIDEO_MEMORY);

    nvFieldEnumEntryInit(pEntries + GMMU_APERTURE_SYS_COH,
                         NV_MMU_PDE_APERTURE_BIG_SYSTEM_COHERENT_MEMORY);

    nvFieldEnumEntryInit(pEntries + GMMU_APERTURE_SYS_NONCOH,
                         NV_MMU_PDE_APERTURE_BIG_SYSTEM_NON_COHERENT_MEMORY);
}

void kgmmuFmtInitPteApertures_GM10X
(
    KernelGmmu          *pKernelGmmu,
    NV_FIELD_ENUM_ENTRY *pEntries
)
{
    nvFieldEnumEntryInit(pEntries + GMMU_APERTURE_VIDEO,
                         NV_MMU_PTE_APERTURE_VIDEO_MEMORY);

    nvFieldEnumEntryInit(pEntries + GMMU_APERTURE_PEER,
                         NV_MMU_PTE_APERTURE_PEER_MEMORY);

    nvFieldEnumEntryInit(pEntries + GMMU_APERTURE_SYS_COH,
                         NV_MMU_PTE_APERTURE_SYSTEM_COHERENT_MEMORY);

    nvFieldEnumEntryInit(pEntries + GMMU_APERTURE_SYS_NONCOH,
                         NV_MMU_PTE_APERTURE_SYSTEM_NON_COHERENT_MEMORY);
}
