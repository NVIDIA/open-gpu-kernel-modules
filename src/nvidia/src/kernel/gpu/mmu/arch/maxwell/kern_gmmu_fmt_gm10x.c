/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
