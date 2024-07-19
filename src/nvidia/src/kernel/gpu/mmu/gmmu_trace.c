/*
 * SPDX-FileCopyrightText: Copyright (c) 2013-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "mmu/gmmu_fmt.h"
#include "gpu/mmu/kern_gmmu.h"

/*!
 * @file
 * @brief struct MMU_TRACE_CALLBACKS g_gmmuTraceCallbacks and the callback
 *        function implementations.
 */

#define PRINT_FIELD_BOOL(fmt, fmtPte, field, pte)                              \
    do {                                                                       \
        if (nvFieldIsValid32(&(fmtPte)->fld##field.desc))                      \
            NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, fmt,                    \
                         nvFieldGetBool(&(fmtPte)->fld##field, (pte)->v8));    \
    } while (0)

#define PRINT_FIELD_32(fmt, fmtPte, field, pte)                                \
    do {                                                                       \
        if (nvFieldIsValid32(&(fmtPte)->fld##field))                           \
            NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, fmt,                    \
                         nvFieldGet32(&(fmtPte)->fld##field, (pte)->v8));      \
    } while (0)

#if NV_PRINTF_STRINGS_ALLOWED
static const char *_decodeAperture[]       = { "invalid", "video", "peer", "sysnoncoh", "syscoh" };
#else // NV_PRINTF_STRINGS_ALLOWED
#if NV_PRINTF_LEVEL_ENABLED(LEVEL_INFO)
static const char _decodeAperture[]        = "XVPNC";
#endif // NV_PRINTF_LEVEL_ENABLED(LEVEL_INFO)
#endif // NV_PRINTF_STRINGS_ALLOWED

#if NV_PRINTF_LEVEL_ENABLED(LEVEL_INFO)
static const char _decodeSize[] = "1248";
#endif // NV_PRINTF_LEVEL_ENABLED(LEVEL_INFO)

static NvBool
_gmmuIsPte
(
    const void          *pFmt,
    const MMU_FMT_LEVEL *pFmtLevel,
    const MMU_ENTRY     *pEntry,
    NvBool              *pValid
)
{
    const GMMU_FMT         *pFmtGmmu   = (GMMU_FMT*)pFmt;
    const GMMU_ENTRY_VALUE *pGmmuEntry = (GMMU_ENTRY_VALUE*)pEntry;

    if (gmmuFmtEntryIsPte(pFmtGmmu, pFmtLevel, pGmmuEntry->v8))
    {
        if (pValid)
        {
            const GMMU_FMT_PTE *pFmtPte = pFmtGmmu->pPte;

            *pValid = nvFieldGetBool(&pFmtPte->fldValid, pGmmuEntry->v8);
        }

        return NV_TRUE;
    }

    return NV_FALSE;
}

static const void*
_gmmuGetFmtPde
(
    const void          *pFmt,
    const MMU_FMT_LEVEL *pFmtLevel,
    NvU32                sublevel
)
{
    return gmmuFmtGetPde((GMMU_FMT*)pFmt, pFmtLevel, sublevel);
}

static const void*
_gmmuGetFmtPte
(
    const void *pFmt
)
{
    const GMMU_FMT *pFmtGmmu = (GMMU_FMT*)pFmt;

    return pFmtGmmu->pPte;
}

static NvU64
_gmmuGetPdePa
(
    OBJGPU          *pGpu,
    const void      *pFmtPde,
    const MMU_ENTRY *pPde
)
{
    const GMMU_FMT_PDE     *pFmt       = (GMMU_FMT_PDE*)pFmtPde;
    const GMMU_ENTRY_VALUE *pGmmuEntry = (GMMU_ENTRY_VALUE*)pPde;
    GMMU_APERTURE           aperture   = gmmuFieldGetAperture(&pFmt->fldAperture,
                                                              pGmmuEntry->v8);

    if (aperture != GMMU_APERTURE_INVALID)
    {
        const GMMU_FIELD_ADDRESS *pAddrFld = gmmuFmtPdePhysAddrFld(pFmt, aperture);

        if (pAddrFld)
        {
            KernelGmmu *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
            NvU64    addr  = gmmuFieldGetAddress(pAddrFld, pGmmuEntry->v8);
            if ((aperture == GMMU_APERTURE_SYS_NONCOH) || (aperture == GMMU_APERTURE_SYS_COH))
            {
                addr += pKernelGmmu->sysmemBaseAddress;
            }
            return addr;
        }
    }

    return MMU_INVALID_ADDR;
}

static NvU64
_gmmuGetPtePa
(
    OBJGPU          *pGpu,
    const void      *pFmtPte,
    const MMU_ENTRY *pPte
)
{
    const GMMU_FMT_PTE     *pFmt       = (GMMU_FMT_PTE*)pFmtPte;
    const GMMU_ENTRY_VALUE *pGmmuEntry = (GMMU_ENTRY_VALUE*)pPte;
    GMMU_APERTURE           aperture   = gmmuFieldGetAperture(&pFmt->fldAperture,
                                                              pGmmuEntry->v8);

    if (aperture != GMMU_APERTURE_INVALID)
    {
        const GMMU_FIELD_ADDRESS *pAddrFld = gmmuFmtPtePhysAddrFld(pFmt, aperture);
        if (pAddrFld)
        {
            KernelGmmu *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
            NvU64    addr  = gmmuFieldGetAddress(pAddrFld, pGmmuEntry->v8);
            if ((aperture == GMMU_APERTURE_SYS_NONCOH) || (aperture == GMMU_APERTURE_SYS_COH))
            {
                addr += pKernelGmmu->sysmemBaseAddress;
            }
            return addr;
        }
    }

    return MMU_INVALID_ADDR;
}

#if NV_PRINTF_LEVEL_ENABLED(LEVEL_INFO)
static void
_gmmuPrintPa
(
    NvU64         pa,
    GMMU_APERTURE aperture,
    NvU32         peerIndex
)
{
#if NV_PRINTF_STRINGS_ALLOWED
    NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "%s", _decodeAperture[aperture]);
#else
    NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "%c", _decodeAperture[aperture]);
#endif
    if (GMMU_APERTURE_PEER == aperture)
    {
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "[%d]", peerIndex);
    }
    if ((GMMU_APERTURE_INVALID == aperture) && (0 == pa))
    {
        // PA is just noise if invalid, but should print if non-zero (bug indicator).
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, " ");
    }
    else
    {
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "[0x%08llx] ", pa);
    }
}
#endif

static void
_gmmuPrintPdb
(
    OBJGPU     *pGpu,
    OBJVASPACE *pVAS,
    NvU64       va,
    NvU64       vaLimit
)
{
#if NV_PRINTF_LEVEL_ENABLED(LEVEL_INFO)
    PMEMORY_DESCRIPTOR pPDB        = vaspaceGetPageDirBase(pVAS, pGpu);
    KernelGmmu        *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
    GMMU_APERTURE      aperture    = kgmmuGetMemAperture(pKernelGmmu, pPDB);

    NV_PRINTF(LEVEL_INFO, "MMUTRACE: VA[0x%08llx-%08llx] PDB: ", va, vaLimit);

    _gmmuPrintPa(memdescGetPhysAddr(pPDB, VAS_ADDRESS_TRANSLATION(pVAS), 0),
                 aperture, 0);
    NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "\n");
#endif
}

static void
_gmmuPrintPde
(
    OBJGPU              *pGpu,
    const void          *pFmt,
    const MMU_FMT_LEVEL *pFmtLevel,
    const MMU_ENTRY     *pPde
)
{
#if NV_PRINTF_LEVEL_ENABLED(LEVEL_INFO)
    const GMMU_FMT           *pFmtGmmu   = (GMMU_FMT*)pFmt;
    const GMMU_FMT_PDE_MULTI *pPdeMulti  = pFmtGmmu->pPdeMulti;
    const GMMU_FMT_PDE       *pFmtPde    = pFmtGmmu->pPde;
    const GMMU_ENTRY_VALUE   *pGmmuEntry = (GMMU_ENTRY_VALUE*)pPde;
    GMMU_APERTURE             aperture   = GMMU_APERTURE_INVALID;
    NvU64                     pa;

    if (pFmtLevel->numSubLevels > 1)
    {
        NvU32 sublevel;

        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "(Dual) ");

        for (sublevel = 0; sublevel < pFmtLevel->numSubLevels; sublevel++)
        {
            pFmtPde  = &pPdeMulti->subLevels[sublevel];
            aperture = gmmuFieldGetAperture(&pFmtPde->fldAperture, pGmmuEntry->v8);

            if (aperture != GMMU_APERTURE_INVALID)
            {
                break;
            }
        }

        if (aperture == GMMU_APERTURE_INVALID)
        {
            NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "invalid\n");
            return;
        }

        if (nvFieldIsValid32(&pPdeMulti->fldSizeRecipExp))
        {
            NvU32 size = nvFieldGet32(&pPdeMulti->fldSizeRecipExp, pGmmuEntry->v8);

            NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "Size=1/%c", _decodeSize[size]);
        }

        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "\n");

        return;
    }

    aperture = gmmuFieldGetAperture(&pFmtPde->fldAperture, pGmmuEntry->v8);
    pa       = _gmmuGetPdePa(pGpu, pFmtPde, pPde);

    _gmmuPrintPa(pa, aperture, 0);

    if (pFmtGmmu->version == GMMU_FMT_VERSION_3)
    {
        NvU32 pdePcfSw   = 0;
        NvU32 pdePcfHw   = 0;

        PRINT_FIELD_32("PdePcf=%d", pFmtPde, PdePcf, pGmmuEntry);
        pdePcfHw = nvFieldGet32(&pFmtPde->fldPdePcf, pGmmuEntry->v8);
        NV_CHECK(LEVEL_INFO,
                 kgmmuTranslatePdePcfFromHw_HAL(GPU_GET_KERNEL_GMMU(pGpu),
                                                pdePcfHw,
                                                aperture,
                                                &pdePcfSw) == NV_OK);
        NV_PRINTF_EX(NV_PRINTF_MODULE,
                     LEVEL_INFO,
                     "(Sparse=%d, Vol=%d, ATS=%d)",
                     ((pdePcfSw >> SW_MMU_PCF_SPARSE_IDX) & 0x1),
                     ((pdePcfSw >> SW_MMU_PCF_UNCACHED_IDX) & 0x1),
                     ((pdePcfSw >> SW_MMU_PCF_ATS_ALLOWED_IDX) & 0x1));
    }
    else
    {
        PRINT_FIELD_BOOL("Vol=%d", pFmtPde, Volatile, pGmmuEntry);
    }

    NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "\n");
#endif
}

static void
_gmmuPrintPt
(
    OBJGPU              *pGpu,
    const MMU_FMT_LEVEL *pFmtLevel,
    const void          *pFmt,
    const MMU_ENTRY     *pPde
)
{
#if NV_PRINTF_LEVEL_ENABLED(LEVEL_INFO)
    const GMMU_FMT_PDE     *pFmtPde    = (GMMU_FMT_PDE*)pFmt;
    const GMMU_ENTRY_VALUE *pGmmuEntry = (GMMU_ENTRY_VALUE*)pPde;
    NvU64                   pt         = _gmmuGetPdePa(pGpu, pFmtPde, pPde);
    GMMU_APERTURE           aperture   = gmmuFieldGetAperture(&pFmtPde->fldAperture,
                                                              pGmmuEntry->v8);
    NvU64                   pageSize   = mmuFmtLevelPageSize(pFmtLevel);

    if (pt == MMU_INVALID_ADDR)
    {
        pt = 0;
    }

    switch (pageSize)
    {
        case RM_PAGE_SIZE_HUGE: // 2M
            break;
        case RM_PAGE_SIZE_128K:
            NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "PT_128K: ");
            break;
        case RM_PAGE_SIZE_64K:
            NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "PT_64K:  ");
            break;
        case RM_PAGE_SIZE:      // 4K
            NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "PT_4K:   ");
            break;
        default:
            NV_ASSERT(0);
    }

    _gmmuPrintPa(pt, aperture, 0);
    NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "\n");
#endif
}

static void
_gmmuPrintPte
(
    OBJGPU              *pGpu,
    const MMU_FMT_LEVEL *pFmtLevel,
    const void          *pFmtPte,
    const MMU_ENTRY     *pPte,
    NvU32                index
)
{
#if NV_PRINTF_LEVEL_ENABLED(LEVEL_INFO)
    const GMMU_FMT_PTE     *pFmt       = (GMMU_FMT_PTE*)pFmtPte;
    const GMMU_ENTRY_VALUE *pGmmuEntry = (GMMU_ENTRY_VALUE*)pPte;
    NvU64                   pa         = _gmmuGetPtePa(pGpu, pFmt, pPte);
    GMMU_APERTURE           aperture   = gmmuFieldGetAperture(&pFmt->fldAperture,
                                                              pGmmuEntry->v8);
    NvU32                   peerIndex  = nvFieldGet32(&pFmt->fldPeerIndex,
                                                      pGmmuEntry->v8);

    switch (mmuFmtLevelPageSize(pFmtLevel))
    {
        case RM_PAGE_SIZE_256G:
            NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "PTE_256G");
            break;
        case RM_PAGE_SIZE_512M:
            NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "PTE_512M");
            break;
        case RM_PAGE_SIZE_HUGE:
            NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "PTE_2M");
            break;
        case RM_PAGE_SIZE_128K:
            NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "PTE_128K");
            break;
        case RM_PAGE_SIZE_64K:
            NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "PTE_64K");
            break;
        case RM_PAGE_SIZE:
            NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "PTE_4K");
            break;
        default:
            NV_ASSERT(0);
            break;
    }

    NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "[0x%x]: ", index);
    _gmmuPrintPa(pa, aperture, peerIndex);

    if (pFmt->version == GMMU_FMT_VERSION_3)
    {
        NvU32 ptePcfSw   = 0;
        NvU32 ptePcfHw   = 0;

        PRINT_FIELD_BOOL("Vld=%d, ",   pFmt, Valid,  pGmmuEntry);
        PRINT_FIELD_32("Kind=0x%x, ",  pFmt, Kind,   pGmmuEntry);
        PRINT_FIELD_32("PtePcf=%d",   pFmt, PtePcf, pGmmuEntry);
        ptePcfHw = nvFieldGet32(&pFmt->fldPtePcf, pGmmuEntry->v8);
        NV_ASSERT(kgmmuTranslatePtePcfFromHw_HAL(GPU_GET_KERNEL_GMMU(pGpu),
                                                ptePcfHw,
                                                nvFieldGetBool(&pFmt->fldValid, pGmmuEntry->v8),
                                                &ptePcfSw) == NV_OK);
        NV_PRINTF_EX(NV_PRINTF_MODULE,
                     LEVEL_INFO, "(Vol=%d, Priv=%d, RO=%d, Atomic=%d, ACE=%d)",
                     ((ptePcfSw >> SW_MMU_PCF_UNCACHED_IDX) & 0x1),
                     !((ptePcfSw >> SW_MMU_PCF_REGULAR_IDX) & 0x1),
                     ((ptePcfSw >> SW_MMU_PCF_RO_IDX) & 0x1),
                     !((ptePcfSw >> SW_MMU_PCF_NOATOMIC_IDX) & 0x1),
                     ((ptePcfSw >> SW_MMU_PCF_ACE_IDX) & 0x1));
    }
    else
    {
        PRINT_FIELD_BOOL("Vld=%d, ",        pFmt, Valid,           pGmmuEntry);
        PRINT_FIELD_BOOL("Priv=%d, ",       pFmt, Privilege,       pGmmuEntry);
        PRINT_FIELD_BOOL("RO=%d, ",         pFmt, ReadOnly,        pGmmuEntry);
        PRINT_FIELD_BOOL("RD=%d, ",         pFmt, ReadDisable,     pGmmuEntry);
        PRINT_FIELD_BOOL("WD=%d, ",         pFmt, WriteDisable,    pGmmuEntry);
        PRINT_FIELD_BOOL("Enc=%d, ",        pFmt, Encrypted,       pGmmuEntry);
        PRINT_FIELD_BOOL("Vol=%d, ",        pFmt, Volatile,        pGmmuEntry);
        PRINT_FIELD_BOOL("Lock=%d, ",       pFmt, Locked,          pGmmuEntry);
        PRINT_FIELD_BOOL("AtomDis=%d, ",    pFmt, AtomicDisable,   pGmmuEntry);
        PRINT_FIELD_32("Kind=0x%x, ",       pFmt, Kind,            pGmmuEntry);
        PRINT_FIELD_32("CTL=0x%x, ",        pFmt, CompTagLine,     pGmmuEntry);
        PRINT_FIELD_32("CTL_MSB=%d, ",      pFmt, CompTagSubIndex, pGmmuEntry);
    }
    NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "\n");
#endif // NV_PRINTF_LEVEL_ENABLED(LEVEL_INFO)
}

static NvBool
_gmmuIsInvalidPdeOk
(
    OBJGPU          *pGpu,
    const void      *pFmt,
    const void      *pFmtEntry,
    const MMU_ENTRY *pPde,
    NvU32            sublevel
)
{
    const GMMU_FMT         *pFmtGmmu    = (GMMU_FMT*)pFmt;
    const GMMU_FMT_PDE     *pFmtPde     = (GMMU_FMT_PDE*)pFmtEntry;
    const GMMU_ENTRY_VALUE *pGmmuEntry  = (GMMU_ENTRY_VALUE*)pPde;
    NvBool bSparse  = NV_FALSE;

    if (pFmtGmmu->version == GMMU_FMT_VERSION_3)
    {
        KernelGmmu *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
        NvU32       pdePcfHw = 0;
        NvU32       pdePcfSw = 0;

        pdePcfHw = nvFieldGet32(&pFmtPde->fldPdePcf, pGmmuEntry->v8);
        NV_ASSERT(kgmmuTranslatePdePcfFromHw_HAL(pKernelGmmu, pdePcfHw,
                                                 gmmuFieldGetAperture(&pFmtPde->fldAperture, pGmmuEntry->v8),
                                                 &pdePcfSw) == NV_OK);
        bSparse = pdePcfSw & (1 << SW_MMU_PCF_SPARSE_IDX);
    }
    else
    {
        bSparse = nvFieldGetBool(&pFmtPde->fldVolatile, pGmmuEntry->v8);
    }
    if (pFmtGmmu->bSparseHwSupport && (sublevel == 0) && bSparse)
    {
        return NV_FALSE;
    }

    return NV_TRUE;
}

static NvU32
_gmmuPdeAddrSpace
(
    const void      *pFmtEntry,
    const MMU_ENTRY *pPde
)
{
    const GMMU_FMT_PDE     *pFmtPde    = (GMMU_FMT_PDE*)pFmtEntry;
    const GMMU_ENTRY_VALUE *pGmmuEntry = (GMMU_ENTRY_VALUE*)pPde;

    switch (gmmuFieldGetAperture(&pFmtPde->fldAperture, pGmmuEntry->v8))
    {
        case GMMU_APERTURE_INVALID:
            return ADDR_UNKNOWN;
        case GMMU_APERTURE_VIDEO:
            return ADDR_FBMEM;
        case GMMU_APERTURE_SYS_COH:
        case GMMU_APERTURE_SYS_NONCOH:
            return ADDR_SYSMEM;
        default:
            NV_ASSERT(0);
            return ADDR_UNKNOWN;
    }
}

static NvU32
_gmmuPteAddrSpace
(
    const void      *pFmtEntry,
    const MMU_ENTRY *pPte
)
{
    const GMMU_FMT_PTE     *pFmtPte    = (GMMU_FMT_PTE*)pFmtEntry;
    const GMMU_ENTRY_VALUE *pGmmuEntry = (GMMU_ENTRY_VALUE*)pPte;

    switch (gmmuFieldGetAperture(&pFmtPte->fldAperture, pGmmuEntry->v8))
    {
        case GMMU_APERTURE_INVALID:
            return ADDR_UNKNOWN;
        case GMMU_APERTURE_VIDEO:   // Fallthrough
        case GMMU_APERTURE_PEER:
            return ADDR_FBMEM;
        case GMMU_APERTURE_SYS_COH: // Fallthrough
        case GMMU_APERTURE_SYS_NONCOH:
            return ADDR_SYSMEM;
        default:
            NV_ASSERT(0);
            return ADDR_UNKNOWN;
    }
}

static NvU32
_gmmuSwToHwLevel
(
    const void *pFmt,
    NvU32       level
)
{
    const GMMU_FMT *pFmtGmmu = (GMMU_FMT*)pFmt;
    NvU32 maxV3Levels        = 0;

    switch (pFmtGmmu->version)
    {
    case GMMU_FMT_VERSION_3:
        maxV3Levels = (pFmtGmmu->pRoot->virtAddrBitHi == 56) ? 5 : 4;
        NV_ASSERT_OR_RETURN(level < maxV3Levels, 0);
        return (maxV3Levels - 1) - level;
    case GMMU_FMT_VERSION_2:
        NV_ASSERT_OR_RETURN(level < 4, 0);
        return 3 - level;
    case GMMU_FMT_VERSION_1:
        NV_ASSERT_OR_RETURN(level == 0, 0);
        return 0;
    default:
        NV_ASSERT(0);
        return 0;
    }
}

const MMU_TRACE_CALLBACKS g_gmmuTraceCallbacks =
{
    _gmmuIsPte,          // isPte
    _gmmuGetFmtPde,      // getFmtPde
    _gmmuGetFmtPte,      // getFmtPte
    _gmmuGetPdePa,       // getPdePa
    _gmmuGetPtePa,       // getPtePa
    _gmmuPrintPdb,       // printPdb
    _gmmuPrintPde,       // printPde
    _gmmuPrintPt,        // printPt
    _gmmuPrintPte,       // printPte
    _gmmuIsInvalidPdeOk, // isInvalidPdeOk
    _gmmuPdeAddrSpace,   // pdeAddrSpace
    _gmmuPteAddrSpace,   // pteAddrSpace
    _gmmuSwToHwLevel,    // swToHwLevel
};
