/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2019 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "mmu/gmmu_fmt.h"

#if PORT_IS_CHECKED_BUILD
#define GMMU_DBG_CHECKS
#endif

const NvU32 g_gmmuFmtVersions[GMMU_FMT_MAX_VERSION_COUNT] =
{
    GMMU_FMT_VERSION_1,
    GMMU_FMT_VERSION_2,
    GMMU_FMT_VERSION_3,
};

const NvU32 g_gmmuFmtBigPageShifts[GMMU_FMT_MAX_BIG_PAGE_SIZES] =
{
    16,
    17,
};

const GMMU_FMT_PDE* gmmuFmtGetPde
(
    const GMMU_FMT      *pFmt,
    const MMU_FMT_LEVEL *pLevel,
    const NvU32          subLevel
)
{
    switch (pLevel->numSubLevels)
    {
        case 0:
            return NULL;
        case 1:
            return pFmt->pPde;
        default:
            NV_ASSERT_OR_RETURN(subLevel < MMU_FMT_MAX_SUB_LEVELS, NULL);
            return &pFmt->pPdeMulti->subLevels[subLevel];
    }
    return NULL;
}

NvBool
gmmuFmtEntryIsPte
(
    const GMMU_FMT      *pFmt,
    const MMU_FMT_LEVEL *pLevel,
    const NvU8          *pEntry
)
{
    const NvBool bPageTable = pLevel->bPageTable;
    const NvBool bPageDir   = pLevel->numSubLevels > 0;
    if (bPageTable && bPageDir)
    {
        return nvFieldGetBool(&pFmt->pPte->fldValid, pEntry);
    }
    else if (bPageTable)
    {
        return NV_TRUE;
    }
    else
    {
        NV_ASSERT(bPageDir);
        return NV_FALSE;
    }
}

const GMMU_FIELD_ADDRESS *
gmmuFmtPdePhysAddrFld
(
    const GMMU_FMT_PDE *pPde,
    const GMMU_APERTURE aperture
)
{
    if (pPde->version == GMMU_FMT_VERSION_3)
    {
        // GMMU_FMT_VERSION_3 uses a unified PDE field
        return &pPde->fldAddr;
    }
    else
    {
        switch (aperture)
        {
            case GMMU_APERTURE_SYS_COH:
            case GMMU_APERTURE_SYS_NONCOH:
                return &pPde->fldAddrSysmem;
            case GMMU_APERTURE_VIDEO:
                return &pPde->fldAddrVidmem;
            default:
                NV_ASSERT(0);
                return NULL;
        }
    }
}

const GMMU_FIELD_ADDRESS *
gmmuFmtPtePhysAddrFld
(
    const GMMU_FMT_PTE *pPte,
    const GMMU_APERTURE aperture
)
{
    switch (aperture)
    {
        case GMMU_APERTURE_SYS_COH:
        case GMMU_APERTURE_SYS_NONCOH:
            return &pPte->fldAddrSysmem;
        case GMMU_APERTURE_PEER:
            return &pPte->fldAddrPeer;
        case GMMU_APERTURE_VIDEO:
            return &pPte->fldAddrVidmem;
        default:
            NV_ASSERT(0);
            return NULL;
    }
}

void gmmuFmtInitPteCompTags
(
    const GMMU_FMT        *pFmt,
    const MMU_FMT_LEVEL   *pLevel,
    const GMMU_COMPR_INFO *pCompr,
    const NvU64            surfOffset,
    const NvU32            startPteIndex,
    const NvU32            numPages,
          NvU8            *pEntries
)
{
    NvU32                  i, compPageIndex, endCompPageIndex;
    NvU64                  offset           = surfOffset;
    const NvU64            pageSize         = mmuFmtLevelPageSize(pLevel);
    const NV_FIELD_DESC32 *pCtlSubIndexFld  = &pFmt->pPte->fldCompTagSubIndex;
    NvU32                  ctlSubIndexMask  = 0;
    NvU32                  ctlSubIndexShift = 0;

    //
    // Surface offset must be aligned to the page size.
    // Otherwise we're trying to map part-way into the physical pages.
    //
    NV_ASSERT(0 == (surfOffset & (pageSize - 1)));

    //
    // On GM20X the MSB bit of the CTL field selects which half of a 128KB
    // compression page is used when page size is <= 64KB.
    // This bit is generalized in the format description as a separate
    // CTL sub index field.
    //
    // If the field is valid, calculate the mask and shift that will be
    // applied to the surface offset to select the sub index.
    //
    // TODO: This should be precomputed, but need to update APIs.
    //
    if (nvFieldIsValid32(pCtlSubIndexFld))
    {
        ctlSubIndexMask = pCtlSubIndexFld->maskPos >> pCtlSubIndexFld->shift;
        ctlSubIndexShift = pCompr->compPageShift -
                           nvPopCount32(pCtlSubIndexFld->maskPos);
    }
    //
    // If not supported (pre-GM20X) HW takes the CTL sub index
    // from the virtual address instead. This adds a restriction
    // that virtual addresses must be aligned to compression
    // page size when compression is used.
    //
    // This is further complicated with the use of Tiled Pools/Resources
    // where two or more virtual mappings alias to the same compressed surface
    // without control over the alignment (application controlled).
    // For this case the only pre-GM20X option is to assign each
    // 64KB physical page an entire 128KB compression page, wasting
    // half of each comptagline.
    // This implies that the aliased virtual mappings cannot be
    // used consistently *at the same time* since the views may not use the
    // same comptagline half.
    // Therefore each view requires a surface clear when it takes ownership
    // of the memory.
    // Note this double-comptagline assignment is not handled in this
    // function. See CNvLPagerFermi::overrideCompTagLineInfo for details.
    //
    // If this assertion fails then the alignment is not being
    // enforced properly higher up in the driver stack.
    // This API cannot fail so there is no corrective action,
    // but visual corruption will likely occur.
    //
#if defined(GMMU_DBG_CHECKS)
    else
    {
        const NvU64 comprPageMask = NVBIT64(pCompr->compPageShift) - 1;
        const NvU64 virtCtlOffset = (startPteIndex * pageSize) & comprPageMask;
        const NvU64 surfCtlOffset = surfOffset                 & comprPageMask;
        NV_ASSERT(virtCtlOffset == surfCtlOffset);
    }
#endif

    //
    // The following table is an example of how comptaglines are assigned
    // to a surface with N 64KB pages on HW with 128KB compression page size.
    //
    // The compPageIndex variables index compression pages (e.g. 128KB chunks)
    // starting from the start of the surface (0).
    // The below factor of (compPageIndex * 2) derives from
    // 128KB compression page size / 64KB page size.
    //
    // Notice that the compPageIndex range allows for any contiguous subset
    // of the surface to be compressed. Normally the entire surface
    // is compressed but the clamping allows partial compression as a
    // fallback (when comptags fragment) and for verification purposes.
    //
    //  +---------------------------+---------------------+---------------+
    //  | Surface Page Index (64KB) | CompTagLine (128KB) | CTL Sub Index |
    //  +---------------------------+---------------------+---------------+
    //  | 0                         | N/A                 | N/A           |
    //  | 1                         | N/A                 | N/A           |
    //  | ...                       | N/A                 | N/A           |
    //  | compPageIndexLo * 2 + 0   | compTagLineMin + 0  | 0             |
    //  | compPageIndexLo * 2 + 1   | compTagLineMin + 0  | 1             |
    //  | compPageIndexLo * 2 + 2   | compTagLineMin + 1  | 0             |
    //  | compPageIndexLo * 2 + 3   | compTagLineMin + 1  | 1             |
    //  | ...                       | ...                 | ...           |
    //  | compPageIndexHi * 2 - 3   | compTagLineMax - 1  | 0             |
    //  | compPageIndexHi * 2 - 2   | compTagLineMax - 1  | 1             |
    //  | compPageIndexHi * 2 - 1   | compTagLineMax - 0  | 0             |
    //  | compPageIndexHi * 2 - 0   | compTagLineMax - 0  | 1             |
    //  | ...                       | N/A                 | N/A           |
    //  | N - 2                     | N/A                 | N/A           |
    //  | N - 1                     | N/A                 | N/A           |
    //  +---------------------------+---------------------+---------------+
    //
    // compTagLineMax = compTagLineMin + (compPageIndexHi - compPageIndexLo)
    //
    for (i = 0; i < numPages; ++i)
    {
        // 2MB page require 0x20 comptags to be contiguous. so check for endPage limit too.
        compPageIndex = (NvU32)(offset >> pCompr->compPageShift);
        endCompPageIndex = (NvU32)((offset + pageSize -1)>> pCompr->compPageShift);
        if ((compPageIndex >= pCompr->compPageIndexLo) &&
            (compPageIndex <= pCompr->compPageIndexHi) &&
            (endCompPageIndex >= pCompr->compPageIndexLo) &&
            (endCompPageIndex <= pCompr->compPageIndexHi))
        {
            NvU8 *pPte        = pEntries + (i * pLevel->entrySize);
            NvU32 compTagLine = (compPageIndex - pCompr->compPageIndexLo) * pCompr->compTagLineMultiplier +
                pCompr->compTagLineMin;

            nvFieldSet32(&pFmt->pPte->fldKind, pCompr->compressedKind, pPte);
            nvFieldSet32(&pFmt->pPte->fldCompTagLine, compTagLine, pPte);

            // Calculate the CTL sub index if supported.
            if (0 != ctlSubIndexMask)
            {
                NvU32 ctlSubIndex = (NvU32)(offset >> ctlSubIndexShift) &
                                    ctlSubIndexMask;
                nvFieldSet32(pCtlSubIndexFld, ctlSubIndex, pPte);
            }
        }

        offset += pageSize;
    }
}
