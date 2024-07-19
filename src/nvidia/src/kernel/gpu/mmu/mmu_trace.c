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

/***************************** HW State Routines ***************************\
*                                                                           *
*         Memory Manager Object Function Definitions.                       *
*                                                                           *
\***************************************************************************/

#include "gpu/mmu/mmu_trace.h"
#include "gpu/bus/kern_bus.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "mmu/gmmu_fmt.h"
#include "gpu/mmu/kern_gmmu.h"
#include "gpu/mem_mgr/virt_mem_allocator_common.h"

#include "os/os.h"
#include "mem_mgr/gpu_vaspace.h"

#define MMU_TRACE_INDENT(level)                                      \
    do {                                                             \
        NvU32 _level;                                                \
        for (_level = 0; _level < level + 1; _level++)               \
            NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "  ");        \
    } while (0)

typedef struct
{
    const void                *pFmt;
    const MMU_FMT_LEVEL       *pFmtRoot;
    const MMU_TRACE_CALLBACKS *pTraceCb;
} MMU_LAYOUT, *PMMU_LAYOUT;

typedef struct {
  NvU64  va;
  NvU64  vaLimit;
  NvU32  index;
  NvU32  indexLimit;
  NvBool bInvalid;
} MMU_INVALID_RANGE, *PMMU_INVALID_RANGE;


typedef NV_STATUS (*MmuTraceCbPte) (OBJGPU *pGpu, const MMU_TRACE_CALLBACKS *pTraceCb, NvU64 va,
                                    const MMU_FMT_LEVEL *pFmtLevel, const void *pFmtPte,
                                    const MMU_ENTRY *pPte, void *pArg, NvBool valid, NvBool *pDone);

typedef NV_STATUS  (*MmuTraceCbPde) (OBJGPU *pGpu, const MMU_TRACE_CALLBACKS *pTraceCb, NvU64 va,
                                     NvU64 vaLimit, NvU32 level, NvU32 index,
                                     const MMU_FMT_LEVEL *pFmtLevel, const void *pFmtPde, const MMU_ENTRY *pPde,
                                     NvBool valid, NvBool *pDone, void *pArg);

typedef NV_STATUS (*MmuTraceCbTranslate) (OBJGPU *pGpu, const MMU_TRACE_CALLBACKS *pTraceCb,
                                          const MMU_FMT_LEVEL *pFmtLevel, const void *pFmtPte,
                                          const MMU_ENTRY *pPte, void *pArg, NvU64 va, NvBool valid,
                                          NvBool *pDone);

typedef NV_STATUS  (*MmuTraceCbDumpMapping) (OBJGPU *pGpu, const MMU_TRACE_CALLBACKS *pTraceCb,
                                             const MMU_FMT_LEVEL *pFmtLevel, const void *pFmtPte,
                                             const MMU_ENTRY *pPte, void *pArg, NvU64 va, NvU64 vaLimit,
                                             NvBool valid, NvBool *pDone);

typedef NV_STATUS (*MmuTraceCbValidate) (NvBool valid, void *pArg, NvU64 entryVa, NvU64 entryVaLimit,
                                         NvBool *pDone);

typedef struct {
    MmuTraceCbPte         pteFunc;
    MmuTraceCbTranslate   translateFunc;
    MmuTraceCbDumpMapping dumpMappingFunc;
    MmuTraceCbValidate    validateFunc;
    MmuTraceCbPde         pdeFunc;
    NvU64                 vaArg;
    PMMU_TRACE_ARG        pArg;
} MMU_TRACE_INFO, *PMMU_TRACE_INFO;

static void _mmuInitLayout (OBJGPU *pGpu, OBJVASPACE *pVAS, PMMU_LAYOUT pLayout);

static NV_STATUS _mmuTraceWalk(OBJGPU *pGpu, PMMU_LAYOUT pLayout, MMU_WALK *pWalk, NvU32 level, const MMU_FMT_LEVEL *pFmtLevel,
                               PMEMORY_DESCRIPTOR pMemDesc, NvU64 va, NvU64 vaLimit, PMMU_TRACE_INFO pInfo,
                               NvBool *pDone, NvBool verbose);

static NV_STATUS _mmuTracePteCallback(OBJGPU *pGpu, const MMU_TRACE_CALLBACKS *pTraceCb, NvU64 va,
                                      const MMU_FMT_LEVEL *pFmtLevel, const void *pFmtPte,
                                      const MMU_ENTRY *pPte, void *pArg, NvBool valid, NvBool *pDone);

static NV_STATUS _mmuTraceTranslateCallback(OBJGPU *Gpu, const MMU_TRACE_CALLBACKS *pTraceCb,
                                            const MMU_FMT_LEVEL *pFmtLevel, const void *pFmtPte,
                                            const MMU_ENTRY *pPte, void *pArg, NvU64 va, NvBool valid,
                                            NvBool *pDone);

static NV_STATUS _mmuTraceDumpMappingCallback(OBJGPU *pGpu, const MMU_TRACE_CALLBACKS *pTraceCb,
                                              const MMU_FMT_LEVEL *pFmtLevel, const void *pFmtPte,
                                              const MMU_ENTRY *pPte, void *pArg, NvU64 va, NvU64 vaLimit,
                                              NvBool valid, NvBool *pDone);

static NV_STATUS _mmuTraceValidateCallback(NvBool valid, void *pArg, NvU64 entryVa, NvU64 entryVaLimit,
                                           NvBool *pDone);

static NV_STATUS
mmuTraceWalk
(
    OBJGPU         *pGpu,
    OBJVASPACE     *pVAS,
    NvU64           va,
    NvU64           vaLimit,
    PMMU_TRACE_INFO pInfo,
    NvBool          verbose
)
{
    PMEMORY_DESCRIPTOR pPDB = vaspaceGetPageDirBase(pVAS, pGpu);
    OBJGVASPACE       *pGVAS = dynamicCast(pVAS, OBJGVASPACE);
    GVAS_GPU_STATE    *pGpuState = gvaspaceGetGpuState(pGVAS, pGpu);
    NvBool             done = NV_FALSE;
    MMU_LAYOUT         layout;
    MMU_WALK          *pWalk = pGpuState->pWalk;

    NV_ASSERT(pPDB);
    _mmuInitLayout(pGpu, pVAS, &layout);

    if (verbose)
    {
        layout.pTraceCb->printPdb(pGpu, pVAS, va, vaLimit);
    }

    return _mmuTraceWalk(pGpu, &layout, pWalk, 0, layout.pFmtRoot, pPDB,
                         va, vaLimit, pInfo, &done, verbose);
}

/*!
 * @brief Translates virtual addresses to physical addresses. Has additional
 *        functionality when specifying the mode in pParams
 *
 * @param[in]     pGpu        OBJGPU pointer
 * @param[in]     pVAS        OBJVASPACE pointer
 * @param[in/out] pParams     Input parameters as well as pArg, which is where this
 *                            function will write all outputs.
 *
 * @returns NV_STATUS status = NV_OK on success, or status upon failure.
 */
NV_STATUS
mmuTrace
(
    OBJGPU          *pGpu,
    OBJVASPACE      *pVAS,
    PMMU_TRACE_PARAM pParams
)
{
    PMEMORY_DESCRIPTOR pPDB;
    MMU_TRACE_INFO     info = {0};
    NV_STATUS          status;
    NvBool             verbose;
    NvBool             modeValid;
    MMU_TRACE_MODE     traceMode;

    if (RMCFG_FEATURE_PLATFORM_GSP)
    {
        //
        // All client vaspace is managed by CPU-RM, so MMU_TRACER is not needed
        // in GSP-RM
        //
        return NV_ERR_NOT_SUPPORTED;
    }

    NV_ASSERT_OR_RETURN(pGpu != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pVAS != NULL, NV_ERR_INVALID_ARGUMENT);

    NV_ASSERT_OR_RETURN(pParams != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pParams->pArg != NULL, NV_ERR_INVALID_ARGUMENT);

    pPDB = vaspaceGetPageDirBase(pVAS, pGpu);
    NV_ASSERT_OR_RETURN(pPDB != NULL, NV_ERR_INVALID_ARGUMENT);

    verbose = (pParams->mode == MMU_TRACE_MODE_TRACE_VERBOSE);

    pParams->pArg->pa = MMU_INVALID_ADDR;
    pParams->pArg->aperture = ADDR_UNKNOWN;

    info.pArg = pParams->pArg;

    modeValid = NV_TRUE;
    traceMode = pParams->mode;

    switch (traceMode) {
        case MMU_TRACE_MODE_TRACE:
        case MMU_TRACE_MODE_TRACE_VERBOSE:
            info.pteFunc = _mmuTracePteCallback;
            break;

        case MMU_TRACE_MODE_TRANSLATE:
            info.translateFunc = _mmuTraceTranslateCallback;
            info.vaArg = pParams->va;
            pParams->pArg->valid = NV_FALSE;
            break;

        case MMU_TRACE_MODE_VALIDATE:
            info.validateFunc = _mmuTraceValidateCallback;
            pParams->pArg->valid = NV_FALSE;
            pParams->pArg->validateCount = pParams->vaLimit - pParams->va + 1;
            break;

        case MMU_TRACE_MODE_DUMP_RANGE:
            info.pArg->pMapParams->count = 0;
            info.pArg->pMapParams->hasMore = 0;
            info.dumpMappingFunc = _mmuTraceDumpMappingCallback;
            break;

        default:
            modeValid = NV_FALSE;
            break;
    }

    NV_ASSERT_OR_RETURN(modeValid, NV_ERR_INVALID_ARGUMENT);
    status = mmuTraceWalk(pGpu, pVAS, pParams->va, pParams->vaLimit, &info, verbose);

    // If translate mode but a translate never happened, return as an error.
    if (traceMode == MMU_TRACE_MODE_TRANSLATE && !pParams->pArg->valid)
    {
        return NV_ERR_INVALID_XLATE;
    }

    return status;
}

static void
_mmuInitLayout
(
    OBJGPU     *pGpu,
    OBJVASPACE *pVAS,
    PMMU_LAYOUT pLayout
)
{
    OBJGVASPACE    *pGVAS = dynamicCast(pVAS, OBJGVASPACE);
    const GMMU_FMT *pGmmuFmt;

    ct_assert(sizeof(GMMU_ENTRY_VALUE) <= sizeof(MMU_ENTRY));

    NV_ASSERT(pGVAS); // Only valid for gvaspaces as of now

    pGmmuFmt = gvaspaceGetGmmuFmt(pGVAS, pGpu);
    NV_ASSERT(pGmmuFmt);

    pLayout->pFmt     = pGmmuFmt;
    pLayout->pFmtRoot = pGmmuFmt->pRoot;
    pLayout->pTraceCb = &g_gmmuTraceCallbacks;
}


static void
_mmuPrintPte
(
    OBJGPU                    *pGpu,
    const MMU_TRACE_CALLBACKS *pTraceCb,
    NvU64                      va,
    NvU64                      vaLimit,
    NvU32                      level,
    NvU32                      index,
    const MMU_FMT_LEVEL       *pFmtLevel,
    const void                *pFmtPte,
    const MMU_ENTRY           *pPte,
    NvBool                     valid,
    PMMU_INVALID_RANGE         pRange,
    NvBool                     verbose
)
{
    NvU64 pageSize;

    if (!verbose)
    {
        return;
    }

    pageSize = mmuFmtLevelPageSize(pFmtLevel);
    if ((RM_PAGE_SIZE_HUGE != pageSize) &&
        (RM_PAGE_SIZE_256G != pageSize) &&
        (RM_PAGE_SIZE_512M != pageSize))
    {
        level++; // Indent one more level for PTE
    }

    if (pRange->bInvalid)
    {
        NV_PRINTF(LEVEL_INFO, "MMUTRACE: VA[0x%08llx-%08llx]", pRange->va,
                  pRange->vaLimit);
        MMU_TRACE_INDENT(level);

        switch (pageSize)
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
            NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "PTE");
            NV_ASSERT(0);
            break;
        }

        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "[0x%x", pRange->index);
        if (pRange->index != pRange->indexLimit)
        {
            NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "-%x", pRange->indexLimit);
        }
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "]: invalid\n");
    }

    if (valid)
    {
        NV_PRINTF(LEVEL_INFO, "MMUTRACE: VA[0x%08llx-%08llx]", va, vaLimit);
        MMU_TRACE_INDENT(level);
        pTraceCb->printPte(pGpu, pFmtLevel, pFmtPte, pPte, index);
    }
}

#if NV_PRINTF_LEVEL_ENABLED(LEVEL_INFO)
static void
_mmuPrintPt
(
    OBJGPU              *pGpu,
    PMMU_LAYOUT          pLayout,
    NvU64                va,
    NvU64                vaLimit,
    const MMU_FMT_LEVEL *pFmtLevel,
    NvU32                level,
    NvU32                subLevel,
    const MMU_ENTRY     *pPde
)
{
    const void                *pFmt     = pLayout->pFmt;
    const MMU_TRACE_CALLBACKS *pTraceCb = pLayout->pTraceCb;
    const MMU_FMT_LEVEL       *pFmtSub  = &pFmtLevel->subLevels[subLevel];
    const void                *pFmtPde  = pTraceCb->getFmtPde(pFmt, pFmtLevel, subLevel);

    NV_PRINTF(LEVEL_INFO, "MMUTRACE: VA[0x%08llx-%08llx]", va, vaLimit);
    MMU_TRACE_INDENT(level+1);
    pTraceCb->printPt(pGpu, pFmtSub, pFmtPde, pPde);
}
#endif

static void
_mmuPrintPdeInvalid
(
    PMMU_LAYOUT        pLayout,
    NvU32              level,
    PMMU_INVALID_RANGE pRange,
    NvBool             verbose
)
{
#if NV_PRINTF_LEVEL_ENABLED(LEVEL_INFO)
    const void                *pFmt     = pLayout->pFmt;
    const MMU_TRACE_CALLBACKS *pTraceCb = pLayout->pTraceCb;
    NvU32                      hwLevel  = pTraceCb->swToHwLevel(pFmt, level);

    if (!verbose)
    {
        return;
    }

    NV_PRINTF(LEVEL_INFO, "MMUTRACE: VA[0x%08llx-%08llx]", pRange->va,
              pRange->vaLimit);
    MMU_TRACE_INDENT(level);
    NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "PDE%u[0x%x", hwLevel, pRange->index);
    if (pRange->index != pRange->indexLimit)
    {
        NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "-%03x", pRange->indexLimit);
    }
    NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "]: invalid\n");
#endif
}

static void
_mmuPrintPdeValid
(
    OBJGPU              *pGpu,
    PMMU_LAYOUT          pLayout,
    NvU64                va,
    NvU64                vaLimit,
    NvU32                level,
    NvU32                subLevel,
    NvU32                index,
    const MMU_FMT_LEVEL *pFmtLevel,
    const MMU_ENTRY     *pPde,
    PMMU_INVALID_RANGE   pRange,
    NvBool               verbose
)
{
#if NV_PRINTF_LEVEL_ENABLED(LEVEL_INFO)
    const void                *pFmt     = pLayout->pFmt;
    const MMU_TRACE_CALLBACKS *pTraceCb = pLayout->pTraceCb;
    NvU32                      hwLevel  = pTraceCb->swToHwLevel(pFmt, level);

    if (!verbose)
    {
        return;
    }

    if (pRange->bInvalid)
    {
        _mmuPrintPdeInvalid(pLayout, level, pRange, NV_TRUE);
    }

    NV_PRINTF(LEVEL_INFO, "MMUTRACE: VA[0x%08llx-%08llx]", va, vaLimit);
    MMU_TRACE_INDENT(level);
    NV_PRINTF_EX(NV_PRINTF_MODULE, LEVEL_INFO, "PDE%u[0x%x]: ", hwLevel, index);
    pTraceCb->printPde(pGpu, pFmt, pFmtLevel, pPde);

    // Assuming only PTs result in multiple sublevels
    if (pFmtLevel->numSubLevels > 1)
    {
        NvU32 i;

        for (i = 0; i < pFmtLevel->numSubLevels; i++)
        {
            _mmuPrintPt(pGpu, pLayout, va, vaLimit, pFmtLevel, level, i, pPde);
        }
    }
#endif
}

static NV_STATUS
_mmuTraceWalk
(
    OBJGPU              *pGpu,
    PMMU_LAYOUT          pLayout,
    MMU_WALK            *pWalk,
    NvU32                level,
    const MMU_FMT_LEVEL *pFmtLevel,
    PMEMORY_DESCRIPTOR   pMemDesc,
    NvU64                va,
    NvU64                vaLimit,
    PMMU_TRACE_INFO      pInfo,
    NvBool              *pDone,
    NvBool               verbose
)
{
    const void                *pFmt           = pLayout->pFmt;
    const MMU_TRACE_CALLBACKS *pTraceCb       = pLayout->pTraceCb;
    NvU32                      index          = mmuFmtVirtAddrToEntryIndex(pFmtLevel, va);
    NvU64                      offset         = index * pFmtLevel->entrySize;
    NV_STATUS                  status         = NV_OK;
    MMU_INVALID_RANGE          invalidRange   = {0};
    NvU64                      entryVa        = va;
    NvBool                     isPt           = NV_FALSE;
    NvU8                      *pBase          = NULL;
    MEMORY_DESCRIPTOR         *pTempMemDesc   = NULL;
    MemoryManager             *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);

    if (pMemDesc == NULL)
    {
        return NV_OK;
    }

    pBase = memmgrMemDescBeginTransfer(pMemoryManager, pMemDesc,
                                       TRANSFER_FLAGS_SHADOW_ALLOC |
                                       TRANSFER_FLAGS_SHADOW_INIT_MEM);
    if (pBase == NULL)
    {
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }
    while (entryVa <= vaLimit && index < mmuFmtLevelEntryCount(pFmtLevel))
    {
        //
        // Determine the highest address that this entry covers. Check if
        // our vaLimit actually covers this entire page or not.
        //
        NvU64     entryVaLevelLimit = entryVa | mmuFmtEntryVirtAddrMask(pFmtLevel);
        NvU64     entryVaLimit     = NV_MIN(entryVaLevelLimit, vaLimit);
        NvBool    valid            = NV_FALSE;
        MMU_ENTRY entry;
        NvU32     i;
        NV_ASSERT((offset + pFmtLevel->entrySize) <= pMemDesc->Size);

        portMemCopy(&entry, pFmtLevel->entrySize, pBase + offset, pFmtLevel->entrySize);
        if (pTraceCb->isPte(pFmt, pFmtLevel, &entry, &valid))
        {
            NvU64 vaArg         = pInfo->vaArg;
            const void *pFmtPte = pTraceCb->getFmtPte(pFmt);
            isPt                = NV_TRUE;

            if (pInfo->translateFunc != NULL &&
                mmuFmtLevelVirtAddrLo(pFmtLevel, entryVa) <= vaArg && vaArg <= entryVaLimit)
            {
                status = pInfo->translateFunc(pGpu, pTraceCb, pFmtLevel,
                                              pFmtPte, &entry, pInfo->pArg,
                                              vaArg, valid, pDone);
            }
            else if (pInfo->dumpMappingFunc != NULL)
            {
                status = pInfo->dumpMappingFunc(pGpu, pTraceCb, pFmtLevel, pFmtPte,
                                                &entry, pInfo->pArg, entryVa, entryVaLimit,
                                                valid, pDone);
            }
            else if (pInfo->pteFunc != NULL)
            {
                if (valid)
                {
                    _mmuPrintPte(pGpu, pTraceCb, entryVa, entryVaLimit, level,
                                 index, pFmtLevel, pFmtPte, &entry, NV_TRUE,
                                 &invalidRange, verbose);
                }
                status = pInfo->pteFunc(pGpu, pTraceCb, va, pFmtLevel, pFmtPte,
                                        &entry, pInfo->pArg, valid, pDone);
            }
            else if (pInfo->validateFunc != NULL)
            {
                status = pInfo->validateFunc(valid, pInfo->pArg, entryVa, entryVaLimit, pDone);
            }

            if (status != NV_OK || *pDone)
            {
                goto unmap_and_exit;
            }
            goto update_and_continue;
        }

        status = NV_ERR_INVALID_XLATE;
        // Attempt translation of each sub-level.
        for (i = 0; i < pFmtLevel->numSubLevels; i++)
        {

            NvU32    memSize = 0;

            const void *pFmtPde  = pTraceCb->getFmtPde(pFmt, pFmtLevel, i);
            NvU64       nextBase = pTraceCb->getPdePa(pGpu, pFmtPde, &entry);

            // Entry is invalid
            if (nextBase == MMU_INVALID_ADDR)
            {
                if (!pTraceCb->isInvalidPdeOk(pGpu, pFmt, pFmtPde, &entry, i))
                {
                    status = NV_ERR_INVALID_XLATE;
                    goto unmap_and_exit;
                }

                //
                // On dumpMapping, change status to NV_OK because
                // the entire range can be scanned, which can result in
                // iterating through Pdes with no valid sublevels, which
                // would otherwise leave the status at NV_ERR_INVALID_XLATE
                //
                if (pInfo->dumpMappingFunc != NULL)
                {
                    status = NV_OK;
                }

                // Continue to next sub-level, still assuming a PDE fault so far.
                continue;
            }

            NV_ASSERT_OK_OR_GOTO(status,
                mmuWalkGetPageLevelInfo(pWalk, &pFmtLevel->subLevels[i], entryVa,
                                (const MMU_WALK_MEMDESC**)&pTempMemDesc, &memSize),
                unmap_and_exit);

            // Only print out the PDE the first time we know it's a valid PDE
            if (!valid)
            {
                _mmuPrintPdeValid(pGpu, pLayout, entryVa, entryVaLimit, level, i, index,
                                  pFmtLevel, &entry, &invalidRange, verbose);
            }

            valid = NV_TRUE;

            if (pInfo->pdeFunc != NULL)
            {
                if (NV_OK != pInfo->pdeFunc(pGpu, pTraceCb, entryVa, entryVaLimit, level, index,
                                            pFmtLevel, pFmtPde, &entry, NV_TRUE, pDone,
                                            pInfo->pArg) || *pDone)
                {
                    goto destroy_mem;
                }
            }

            // Recurse into sub-level translation, 1 OK translation => success
            if (NV_OK == _mmuTraceWalk(pGpu, pLayout, pWalk, level + 1, &pFmtLevel->subLevels[i],
                                       pTempMemDesc, entryVa, entryVaLimit, pInfo, pDone, verbose))
            {
                status = NV_OK;
            }

destroy_mem:
            if (*pDone)
            {
                goto unmap_and_exit;
            }
        }

        if (status != NV_OK)
        {
            goto unmap_and_exit;
        }

update_and_continue:

        if (!valid)
        {
            if (!invalidRange.bInvalid)
            {
                invalidRange.va    = entryVa;
                invalidRange.index = index;
            }

            invalidRange.vaLimit    = entryVaLimit;
            invalidRange.indexLimit = index;
        }
        invalidRange.bInvalid = !valid;

        offset  += pFmtLevel->entrySize;
        entryVa  = entryVaLevelLimit + 1;
        index++;
    }

    // contiguous invalid range at the end
    if (invalidRange.bInvalid)
    {
        if (isPt)
        {
            if (pInfo->pteFunc)
            {
                _mmuPrintPte(pGpu, pTraceCb, 0, 0, level, 0, pFmtLevel, NULL, NULL,
                         NV_FALSE, &invalidRange, verbose);
                status = pInfo->pteFunc(pGpu, pTraceCb, 0, NULL, NULL, NULL, NULL, NV_FALSE, pDone);
            }
        }
        else
        {
            if (pInfo->pdeFunc)
            {
                _mmuPrintPdeInvalid(pLayout, level, &invalidRange, verbose);
                status = pInfo->pdeFunc(pGpu, pTraceCb, 0, 0, level, 0, pFmtLevel, NULL,
                                        NULL, NV_FALSE, pDone, pInfo->pArg);
            }
        }
    }

unmap_and_exit:

    memmgrMemDescEndTransfer(pMemoryManager, pMemDesc, TRANSFER_FLAGS_DEFER_FLUSH);

    return status;
}

static NV_STATUS
_mmuTracePteCallback
(
    OBJGPU                    *pGpu,
    const MMU_TRACE_CALLBACKS *pTraceCb,
    NvU64                      va,
    const MMU_FMT_LEVEL       *pFmtLevel,
    const void                *pFmtPte,
    const MMU_ENTRY           *pPte,
    void                      *pArg,
    NvBool                    valid,
    NvBool                    *pDone
)
{
    *pDone = NV_FALSE;

    if (valid)
    {
        PMMU_TRACE_ARG pMmuTraceArg = (PMMU_TRACE_ARG)pArg;

        pMmuTraceArg->pa       = pTraceCb->getPtePa(pGpu, pFmtPte, pPte);
        pMmuTraceArg->pa      += mmuFmtVirtAddrPageOffset(pFmtLevel, va);
        pMmuTraceArg->aperture = pTraceCb->pteAddrSpace(pFmtPte, pPte);
    }

    return NV_OK;
}

static NV_STATUS
_mmuTraceTranslateCallback
(
    OBJGPU                    *pGpu,
    const MMU_TRACE_CALLBACKS *pTraceCb,
    const MMU_FMT_LEVEL       *pFmtLevel,
    const void                *pFmtPte,
    const MMU_ENTRY           *pPte,
    void                      *pArg,
    NvU64                      va,
    NvBool                     valid,
    NvBool                    *pDone
)
{
    PMMU_TRACE_ARG pMmuTraceArg;

    if (!valid)
    {
        *pDone = NV_FALSE;
        return NV_OK;
    }

    pMmuTraceArg           = (PMMU_TRACE_ARG)pArg;
    pMmuTraceArg->pa       = pTraceCb->getPtePa(pGpu, pFmtPte, pPte);

    pMmuTraceArg->pa      += mmuFmtVirtAddrPageOffset(pFmtLevel, va);
    pMmuTraceArg->aperture = pTraceCb->pteAddrSpace(pFmtPte, pPte);
    pMmuTraceArg->valid    = NV_TRUE;
    *pDone = NV_TRUE;

    return NV_OK;
}


static NV_STATUS
_mmuTraceDumpMappingCallback
(
    OBJGPU                    *pGpu,
    const MMU_TRACE_CALLBACKS *pTraceCb,
    const MMU_FMT_LEVEL       *pFmtLevel,
    const void                *pFmtPte,
    const MMU_ENTRY           *pPte,
    void                      *pArg,
    NvU64                      va,
    NvU64                      vaLimit,
    NvBool                     valid,
    NvBool                    *pDone
)
{
    PMMU_TRACE_ARG  pMmuTraceArg = (PMMU_TRACE_ARG)pArg;
    *pDone = NV_FALSE;

    // If the mapping is an invalid range, just continue.
    if (!valid)
    {
        return NV_OK;
    }

    NvBool bCoalesce = (pMmuTraceArg->pMapParams->count > 0) &&
        ((pMmuTraceArg->pMapParams->opsBuffer[pMmuTraceArg->pMapParams->count - 1].gpuVA +
          pMmuTraceArg->pMapParams->opsBuffer[pMmuTraceArg->pMapParams->count - 1].size) == va);

    //
    // If we encounter more ranges that we can't account for,
    // return hasMore = 1.
    //
    if (!bCoalesce && (pMmuTraceArg->pMapParams->count >= MAX_GET_MAPPINGS_OPS))
    {
        *pDone = NV_TRUE;
        pMmuTraceArg->pMapParams->hasMore = 1;
        return NV_OK;
    }

    // Coalesce if possible
    if (bCoalesce)
    {
        pMmuTraceArg->pMapParams->opsBuffer[pMmuTraceArg->pMapParams->count - 1].size  += (NvU32)(vaLimit - va + 1);
    }
    else
    {
        pMmuTraceArg->pMapParams->opsBuffer[pMmuTraceArg->pMapParams->count].gpuVA = va;
        pMmuTraceArg->pMapParams->opsBuffer[pMmuTraceArg->pMapParams->count].size  = (NvU32)(vaLimit - va + 1);
        pMmuTraceArg->pMapParams->count++;
    }

    return NV_OK;
}


static NV_STATUS
_mmuTraceValidateCallback
(
    NvBool  valid,
    void   *pArg,
    NvU64   entryVa,
    NvU64   entryVaLimit,
    NvBool *pDone
)
{
    PMMU_TRACE_ARG pMmuTraceArg = (PMMU_TRACE_ARG)pArg;

    // If the range is valid, then subtract validated range from validateCount
    if (valid)
    {
        NvU64 vaCoverage = entryVaLimit - entryVa;
        pMmuTraceArg->validateCount -= (NV_MIN(vaCoverage + 1, pMmuTraceArg->validateCount));

        // If we've reached zero, then the range is valid and we're done.
        if (pMmuTraceArg->validateCount == 0)
        {
            *pDone = NV_TRUE;
            pMmuTraceArg->valid = NV_TRUE;
        }
        else
        {
            *pDone = NV_FALSE;
            pMmuTraceArg->valid = NV_FALSE;
        }

    }

    // If it's not, then continue the search. We're not done.
    else
    {
        *pDone = NV_FALSE;
    }

    return NV_OK;
}
