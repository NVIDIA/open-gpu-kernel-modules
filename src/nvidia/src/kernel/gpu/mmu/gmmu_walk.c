/*
 * SPDX-FileCopyrightText: Copyright (c) 2013-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "gpu/gpu.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "mem_mgr/gpu_vaspace.h"
#include "gpu/mmu/kern_gmmu.h"
#include "kernel/gpu/nvlink/kernel_nvlink.h"
#include "kernel/gpu/fifo/kernel_fifo.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "nvrm_registry.h"  // NV_REG_STR_RM_*

#include "mmu/gmmu_fmt.h"
#include "mmu/mmu_fmt.h"

/*!
 * @file
 * @brief struct MMU_WALK_CALLBACKS g_gmmuWalkCallbacks and the callback
 *        function implementations.
 */

/**
 * See @ref MMU_WALK_FILL_STATE
 */
#if NV_PRINTF_STRINGS_ALLOWED
const char *g_gmmuFillStateStrings[]           = { "INVALID", "SPARSE", "NV4K" };
const char *g_gmmuUVMMirroringDirStrings[]     = { "[User Root] ", "[Mirrored Root] " };
#else // NV_PRINTF_STRINGS_ALLOWED
static const char _gmmuFillStateString[]       = "XS4";
static const char _gmmuUVMMirroringDirString[] = "UM";
#endif // NV_PRINTF_STRINGS_ALLOWED

static PMEMORY_DESCRIPTOR
_gmmuMemDescCacheCreate(MMU_WALK_USER_CTX *pUserCtx,
                        MEMORY_DESCRIPTOR *pMemDesc,
                        NvU32 memSize);

static PMEMORY_DESCRIPTOR
_gmmuMemDescCacheAlloc(MMU_WALK_USER_CTX *pUserCtx);

/*!
 * Utility function to decide if a level should be mirrored.
 * Used by MMU callbacks.
 */
static NvBool NV_FORCEINLINE
_mirrorLevel
(
    MMU_WALK_USER_CTX   *pUserCtx,
    const MMU_FMT_LEVEL *pLevelFmt
)
{
    return (pLevelFmt == pUserCtx->pGpuState->pFmt->pRoot) && pUserCtx->pGVAS->bIsMirrored;
}

/*!
 * Utility function to get the number of Page Dirs to loop over.
 * Used by MMU callbacks.
 */
static NvU8 NV_FORCEINLINE
_getMaxPageDirs(NvBool bMirror)
{
    return bMirror ? GMMU_MAX_PAGE_DIR_INDEX_COUNT :
                     GMMU_MAX_PAGE_DIR_INDEX_COUNT - 1;
}

static NV_STATUS
_gmmuScrubMemDesc
(
    OBJGPU              *pGpu,
    MEMORY_DESCRIPTOR   *pMemDesc
)
{
    TRANSFER_SURFACE dest = {0};

    dest.pMemDesc = pMemDesc;
    dest.offset = 0;

    NV_ASSERT_OK_OR_RETURN(memmgrMemSet(GPU_GET_MEMORY_MANAGER(pGpu), &dest, 0,
                                        (NvU32)memdescGetSize(pMemDesc),
                                        TRANSFER_FLAGS_NONE));

    return NV_OK;
}

static NV_STATUS
_gmmuWalkCBLevelAlloc
(
    MMU_WALK_USER_CTX       *pUserCtx,
    const MMU_FMT_LEVEL     *pLevelFmt,
    const NvU64              vaBase,
    const NvU64              vaLimit,
    const NvBool             bTarget,
    MMU_WALK_MEMDESC       **ppMemDesc,
    NvU32                   *pMemSize,
    NvBool                  *pBChanged
)
{
    OBJGPU              *pGpu     = pUserCtx->pGpu;
    KernelGmmu          *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
    OBJGVASPACE         *pGVAS    = pUserCtx->pGVAS;
    const GVAS_BLOCK    *pBlock   = pUserCtx->pBlock;
    const GMMU_FMT      *pFmt     = pUserCtx->pGpuState->pFmt;
    MEMORY_DESCRIPTOR   *pMemDesc[GMMU_MAX_PAGE_DIR_INDEX_COUNT] = {NULL};
    const NvU32          minMemSize = (mmuFmtVirtAddrToEntryIndex(pLevelFmt, vaLimit) + 1) *
                                      pLevelFmt->entrySize;
    NvU32                newMemSize;
    NV_STATUS            status   = NV_OK;
    NvU32                alignment;
    NvU32                aperture;
    NvU32                attr;
    NvU64                memDescFlags = MEMDESC_FLAGS_NONE;
    NvU32                memPoolListCount = 0;
    NvU32                memPoolList[4];
    NvBool               bAllowSysmem;
    NvBool               bPacked     = NV_FALSE;
    NvBool               bPartialTbl = NV_FALSE;
    NvBool               bPmaManaged = !!(pGVAS->flags & VASPACE_FLAGS_PTETABLE_PMA_MANAGED);
    NvBool               bMirror     = _mirrorLevel(pUserCtx, pLevelFmt);
    NvU8                 maxPgDirs   = _getMaxPageDirs(bMirror);
    NvU8                 i = 0, j = 0;

    // Abort early if level is not targeted or already sufficiently sized.
    if (((NULL == *ppMemDesc) && !bTarget) ||
        ((NULL != *ppMemDesc) && (minMemSize <= *pMemSize)))
    {
        return NV_OK;
    }

    // Check if this level is the root page directory.
    if (pLevelFmt == pFmt->pRoot)
    {
        newMemSize = kgmmuGetPDBAllocSize_HAL(pKernelGmmu, pLevelFmt, pGVAS->vaLimitInternal);

        // TODO: PDB alignemnt.
        alignment = RM_PAGE_SIZE;

        // Determine level aperture and memory attributes.
        if (pGVAS->flags & VASPACE_FLAGS_BAR)
        {
            aperture     = kgmmuGetPDEBAR1Aperture(pKernelGmmu);
            attr         = kgmmuGetPDEBAR1Attr(pKernelGmmu);
            bAllowSysmem = !FLD_TEST_DRF(_REG_STR_RM, _INST_LOC, _BAR_PDE, _VID,
                                         pGpu->instLocOverrides);
        }
        else
        {
            aperture     = kgmmuGetPDEAperture(pKernelGmmu);
            attr         = kgmmuGetPDEAttr(pKernelGmmu);
            bAllowSysmem = !FLD_TEST_DRF(_REG_STR_RM, _INST_LOC, _PDE, _VID,
                                         pGpu->instLocOverrides);
        }

        // Default aperture.
        memPoolList[memPoolListCount++] = aperture;

        // Fallback to sysmem if allowed.
        if (bAllowSysmem &&
            (aperture != ADDR_SYSMEM) && !(pGVAS->flags & VASPACE_FLAGS_BAR))
        {
            memPoolList[memPoolListCount++] = ADDR_SYSMEM;
        }
    }
    else
    {
        const MMU_FMT_LEVEL       *pParent;
        const GMMU_FMT_PDE_MULTI  *pPdeMulti = pFmt->pPdeMulti;
        const GMMU_FMT_PDE        *pPde;
        NvU32                      subLevel;

        // Find the level's parent format.
        pParent = mmuFmtFindLevelParent(pFmt->pRoot, pLevelFmt, &subLevel);
        NV_ASSERT_OR_RETURN(NULL != pParent, NV_ERR_INVALID_ARGUMENT);

        // Get the alignment from the parent PDE address shift.
        pPde = gmmuFmtGetPde(pFmt, pParent, subLevel);

        if (pPde->version == GMMU_FMT_VERSION_3)
        {
            alignment = NVBIT(pPde->fldAddr.shift);
        }
        else
        {
            alignment = NVBIT(pPde->fldAddrSysmem.shift);
        }

        // Initially assume full size.
        newMemSize = mmuFmtLevelSize(pLevelFmt);

        // Shrink size if partial page tables are supported.
        if ((pGVAS->flags & VASPACE_FLAGS_MINIMIZE_PTETABLE_SIZE) &&
            (pParent->numSubLevels > 1) &&
            nvFieldIsValid32(&pPdeMulti->fldSizeRecipExp))
        {
            NvU32  i;
            //
            // Only a fixed set of PDE ranges are allowed to have partial size.
            // Partial VA holes of these PDEs are blocked at VAS creation time.
            // See @ref gvaspaceConstructHal_IMPL for details.
            //
            for (i = 0; i < pGVAS->numPartialPtRanges; ++i)
            {
                if ((vaBase >= pGVAS->partialPtVaRangeBase[i]) &&
                    (vaBase <= (pGVAS->partialPtVaRangeBase[i] +
                                pGVAS->partialPtVaRangeSize - 1)))
                {
                    const NvU32 recipExpMax = pPdeMulti->fldSizeRecipExp.maskPos >>
                                              pPdeMulti->fldSizeRecipExp.shift;
                    const NvU32 fracMemSize = nvNextPow2_U32(minMemSize);
                    const NvU32 recipExpTgt = BIT_IDX_32(newMemSize / fracMemSize);
                    const NvU32 recipExp    = NV_MIN(recipExpMax, recipExpTgt);
                    newMemSize >>= recipExp;
                    bPartialTbl = NV_TRUE;
                    break;
                }
            }
        }

        // New size must satisfy the minimum size.
        NV_ASSERT(newMemSize >= minMemSize);
        // New size must be larger than old size, otherwise should have aborted earlier.
        NV_ASSERT(newMemSize > *pMemSize);

        // Determine level aperture and memory attributes.
        if (pGVAS->flags & VASPACE_FLAGS_BAR)
        {
            aperture     = kgmmuGetPTEBAR1Aperture(pKernelGmmu);
            attr         = kgmmuGetPTEBAR1Attr(pKernelGmmu);
            bAllowSysmem = !FLD_TEST_DRF(_REG_STR_RM, _INST_LOC, _BAR_PTE, _VID,
                                         pGpu->instLocOverrides);
        }
        else
        {
            aperture     = kgmmuGetPTEAperture(pKernelGmmu);
            attr         = kgmmuGetPTEAttr(pKernelGmmu);
            bAllowSysmem = !FLD_TEST_DRF(_REG_STR_RM, _INST_LOC, _PTE, _VID,
                                         pGpu->instLocOverrides);
        }

        //
        // BAR PDEs/PTEs are not allowed in sysmem since it can cause deadlock
        // during PCIE transactions.
        // PMU PDEs/PTEs must be in vidmem so that PMU can access virtually mapped
        // memory during GC6 exit.
        //
        bAllowSysmem = bAllowSysmem &&
                       !(pGVAS->flags & VASPACE_FLAGS_BAR) &&
                       !(pGVAS->flags & VASPACE_FLAGS_PMU);

        // Prefer sysmem if requested and allowed.
        if (bAllowSysmem &&
            (NULL != pBlock && pBlock->flags.bPreferSysmemPageTables))
        {
            memPoolList[memPoolListCount++] = ADDR_SYSMEM;
        }

        // Default aperture.
        memPoolList[memPoolListCount++] = aperture;

        // Fallback to sysmem if requested and allowed.
        if (bAllowSysmem &&
            (pGVAS->flags & VASPACE_FLAGS_RETRY_PTE_ALLOC_IN_SYS))
        {
            memPoolList[memPoolListCount++] = ADDR_SYSMEM;
        }
    }

    // Add memList end entry.
    memPoolList[memPoolListCount++] = ADDR_UNKNOWN;
    NV_ASSERT(memPoolListCount <= NV_ARRAY_ELEMENTS(memPoolList));

    // MEMDESC flags
    memDescFlags = MEMDESC_FLAGS_LOCKLESS_SYSMEM_ALLOC  |
                   MEMDESC_FLAGS_PAGE_SIZE_ALIGN_IGNORE;

    if (pGVAS->flags & VASPACE_FLAGS_ALLOW_PAGES_IN_PHYS_MEM_SUBALLOCATOR)
    {
        memDescFlags |= MEMDESC_FLAGS_OWNED_BY_CURRENT_DEVICE;
    }

    // Create the level memdesc.
    for (i = 0; i < maxPgDirs; i++)
    {
        MEMORY_DESCRIPTOR *pMemDescTemp;

        status = memdescCreate(&pMemDescTemp, pGpu,
                               (((newMemSize < RM_PAGE_SIZE) && !bPartialTbl && !bPmaManaged) ?
                                RM_PAGE_SIZE : newMemSize),
                               alignment,
                               NV_TRUE,
                               ADDR_UNKNOWN,
                               attr,
                               memDescFlags);
        NV_ASSERT_OR_GOTO(NV_OK == status, done);

        // Page levels always use 4KB swizzle.
        memdescSetPageSize(pMemDescTemp, AT_GPU, RM_PAGE_SIZE);

        //
        // Allocate the page level memory from reserved pool if aperture is vidmem
        // and PMA is enabled. Otherwise, allocate the same way on both vidmem and
        // sysmem.
        //
        while (memPoolList[j] != ADDR_UNKNOWN)
        {
            memdescSetAddressSpace(pMemDescTemp, memPoolList[j]);
            switch (memPoolList[j])
            {
                case ADDR_FBMEM:
                    if (RMCFG_FEATURE_PMA &&
                        (pGVAS->flags & VASPACE_FLAGS_PTETABLE_PMA_MANAGED) &&
                        (pGVAS->pPageTableMemPool != NULL))
                    {
                        pMemDescTemp->ActualSize = RM_ALIGN_UP(newMemSize, alignment);
                        status = rmMemPoolAllocate(pGVAS->pPageTableMemPool,
                                         (RM_POOL_ALLOC_MEMDESC*)pMemDescTemp);
                        break;
                    }
                case ADDR_SYSMEM:
                    memdescTagAlloc(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_143, 
                                    pMemDescTemp);
                    break;
                default:
                    NV_ASSERT_OR_GOTO(0, done);
            }
            if (NV_OK == status)
            {
                //
                // Always scrub the allocation for the PDB allocation in case
                // GMMU prefetches some uninitialized entries
                //
                if (pLevelFmt == pFmt->pRoot)
                {
                    status = _gmmuScrubMemDesc(pGpu, pMemDescTemp);
                }

                memdescSetName(pGpu, pMemDescTemp, NV_RM_SURF_NAME_PAGE_TABLE, mmuFmtConvertLevelIdToSuffix(pLevelFmt));
                break;
            }
            j++;
        }

        if (NV_OK != status)
        {
            memdescDestroy(pMemDescTemp);
            goto done;
        }

        //
        // The packing optimization is only needed for allocations in vidmem since
        // the 4K granularity is not applicable to allocations in sysmem.
        //
        bPacked = ((memdescGetAddressSpace(pMemDescTemp) == ADDR_FBMEM) &&
                   (alignment < RM_PAGE_SIZE) && !bPmaManaged);

        if (bPacked)
        {
            // Try to allocate from the free list of packed memdescs
            pMemDesc[i] = _gmmuMemDescCacheAlloc(pUserCtx);
            if (NULL != pMemDesc[i])
            {
                // Free this if we have already allocated from the list.
                memdescFree(pMemDescTemp);
                memdescDestroy(pMemDescTemp);
            }
            else
            {
                // Add another page to the cache and then alloc.
                pMemDesc[i] = _gmmuMemDescCacheCreate(pUserCtx,
                                                      pMemDescTemp,
                                                      newMemSize);
                if (NULL == pMemDesc[i])
                {
                    memdescFree(pMemDescTemp);
                    memdescDestroy(pMemDescTemp);
                    goto done;
                }
            }
        }
        else
        {
            pMemDesc[i] = pMemDescTemp;
        }

#if NV_PRINTF_STRINGS_ALLOWED
        NV_PRINTF(LEVEL_INFO,
                  "[GPU%u]: [%s] %sPA 0x%llX (0x%X bytes) for VA 0x%llX-0x%llX\n",
                  pUserCtx->pGpu->gpuInstance,
                  bPacked ? "Packed" : "Unpacked",
                  bMirror ? g_gmmuUVMMirroringDirStrings[i] : "",
                  memdescGetPhysAddr(pMemDesc[i], AT_GPU, 0), newMemSize,
                  mmuFmtLevelVirtAddrLo(pLevelFmt, vaBase),
                  mmuFmtLevelVirtAddrHi(pLevelFmt, vaLimit));
#else // NV_PRINTF_STRINGS_ALLOWED
        NV_PRINTF(LEVEL_INFO,
                  "[GPU%u]:  [Packed: %c] PA 0x%llX (0x%X bytes) for VA 0x%llX-0x%llX\n",
                  pUserCtx->pGpu->gpuInstance,
                  bPacked ? 'Y' : 'N',
                  memdescGetPhysAddr(pMemDesc[i], AT_GPU, 0), newMemSize,
                  mmuFmtLevelVirtAddrLo(pLevelFmt, vaBase),
                  mmuFmtLevelVirtAddrHi(pLevelFmt, vaLimit));
#endif // NV_PRINTF_STRINGS_ALLOWED
    }

    // Commit return values.
    *ppMemDesc = (MMU_WALK_MEMDESC*)pMemDesc[GMMU_USER_PAGE_DIR_INDEX];
    *pMemSize  = newMemSize;
    *pBChanged = NV_TRUE;

done:
    if (NV_OK == status)
    {
        // Commit mirrored root desc.
        if (bMirror)
        {
            pUserCtx->pGpuState->pMirroredRoot =
                (MMU_WALK_MEMDESC*)pMemDesc[GMMU_KERNEL_PAGE_DIR_INDEX];
        }
    }
    else
    {
        for (i = 0; i < maxPgDirs; i++)
        {
            memdescFree(pMemDesc[i]);
            memdescDestroy(pMemDesc[i]);
        }
    }
    return status;
}

static PMEMORY_DESCRIPTOR
_gmmuMemDescCacheCreate
(
    MMU_WALK_USER_CTX  *pUserCtx,
    MEMORY_DESCRIPTOR  *pMemDesc,
    NvU32               memSize
)
{
    NV_STATUS status = NV_OK;
    MEMORY_DESCRIPTOR* pMemDescTmp;
    NvU32 i;

    NV_ASSERT_OR_RETURN((NULL != pMemDesc), NULL);
    NV_ASSERT_OR_RETURN((memSize <= pMemDesc->ActualSize), NULL);

    if (pMemDesc->pSubMemDescList == NULL)
    {
        pMemDesc->pSubMemDescList = portMemAllocNonPaged(sizeof(MEMORY_DESCRIPTOR_LIST));
        NV_ASSERT_OR_RETURN(pMemDesc->pSubMemDescList != NULL, NULL);
    }

    // Initialize the list head of the unpacked memdesc
    listInitIntrusive(pMemDesc->pSubMemDescList);

    // Form the list of submemdescs with the parent memdesc as the head
    for (i = 0; i < (pMemDesc->ActualSize / memSize); i++)
    {
        MEMORY_DESCRIPTOR *pSubMemDesc = NULL;
        status = memdescCreateSubMem(&pSubMemDesc,
                                     pMemDesc,
                                     pUserCtx->pGpu,
                                     i * memSize,
                                     memSize);
        NV_ASSERT_OR_RETURN((NV_OK == status), NULL);
        listAppendExisting(pMemDesc->pSubMemDescList, pSubMemDesc);
    }

    // Add the parent memdesc to the per VAS/per GPU list of unpacked memdescs
    listAppendExisting(&pUserCtx->pGpuState->unpackedMemDescList, pMemDesc);

    // Pop the free list of packed memdescs and return one
    pMemDescTmp = listTail(pMemDesc->pSubMemDescList);
    listRemove(pMemDesc->pSubMemDescList, pMemDescTmp);
    return pMemDescTmp;
}

static PMEMORY_DESCRIPTOR
_gmmuMemDescCacheAlloc
(
    MMU_WALK_USER_CTX *pUserCtx
)
{
    MEMORY_DESCRIPTOR *pParentMemDesc;
    MEMORY_DESCRIPTOR *pParentMemDescNext;

    for (pParentMemDesc = listHead(&pUserCtx->pGpuState->unpackedMemDescList);
         pParentMemDesc != NULL;
         pParentMemDesc = pParentMemDescNext)
    {
        pParentMemDescNext = listNext(&pUserCtx->pGpuState->unpackedMemDescList, pParentMemDesc);
        MEMORY_DESCRIPTOR *pChild;
        pChild = listTail(pParentMemDesc->pSubMemDescList);
        listRemove(pParentMemDesc->pSubMemDescList, pChild);
        if (NULL != pChild)
        {
            return pChild;
        }
    }
    return NULL;
}

void
gmmuMemDescCacheFree
(
    GVAS_GPU_STATE *pGpuState
)
{
    NV_ASSERT_OR_RETURN_VOID(NULL != pGpuState);

    while (listCount(&pGpuState->unpackedMemDescList) > 0)
    {
        MEMORY_DESCRIPTOR *pTmp;
        MEMORY_DESCRIPTOR *pParentMemDesc;
        pParentMemDesc = listTail(&pGpuState->unpackedMemDescList);

        // Assert if all submemdescs have not been returned to the parent.
        NV_ASSERT(pParentMemDesc->RefCount - listCount(pParentMemDesc->pSubMemDescList) == 1);

        while(listCount(pParentMemDesc->pSubMemDescList) > 0)
        {
            pTmp = listTail(pParentMemDesc->pSubMemDescList);
            listRemove(pParentMemDesc->pSubMemDescList, pTmp);
            memdescDestroy(pTmp);
        }
        listRemove(&pGpuState->unpackedMemDescList, pParentMemDesc);
        memdescFree(pParentMemDesc);
        memdescDestroy(pParentMemDesc);
    }
}

static void
_gmmuWalkCBLevelFree
(
    MMU_WALK_USER_CTX   *pUserCtx,
    const MMU_FMT_LEVEL *pLevelFmt,
    const NvU64          vaBase,
    MMU_WALK_MEMDESC    *pOldMem
)
{
    NvU8               i;
    NvBool             bMirror   = _mirrorLevel(pUserCtx, pLevelFmt);
    NvU8               maxPgDirs = _getMaxPageDirs(bMirror);
    MEMORY_DESCRIPTOR *pMemDesc[GMMU_MAX_PAGE_DIR_INDEX_COUNT] = {NULL};

    pMemDesc[GMMU_USER_PAGE_DIR_INDEX] = (MEMORY_DESCRIPTOR*)pOldMem;
    if (bMirror)
    {
        pMemDesc[GMMU_KERNEL_PAGE_DIR_INDEX] =
                (MEMORY_DESCRIPTOR*)pUserCtx->pGpuState->pMirroredRoot;
        pUserCtx->pGpuState->pMirroredRoot = NULL;
    }

    for (i = 0; i < maxPgDirs; i++)
    {
        if (NULL == pMemDesc[i])
        {
            continue;
        }

#if NV_PRINTF_STRINGS_ALLOWED
        NV_PRINTF(LEVEL_INFO,
                  "[GPU%u]: %sPA 0x%llX for VA 0x%llX-0x%llX\n",
                  pUserCtx->pGpu->gpuInstance,
                  bMirror ? g_gmmuUVMMirroringDirStrings[i] : "",
                  memdescGetPhysAddr(pMemDesc[i], AT_GPU, 0),
                  mmuFmtLevelVirtAddrLo(pLevelFmt, vaBase),
                  mmuFmtLevelVirtAddrHi(pLevelFmt, vaBase));
#else // NV_PRINTF_STRINGS_ALLOWED
        NV_PRINTF(LEVEL_INFO,
                  "[GPU%u]: %cPA 0x%llX for VA 0x%llX-0x%llX\n",
                  pUserCtx->pGpu->gpuInstance,
                  bMirror ? _gmmuUVMMirroringDirString[i] : ' ',
                  memdescGetPhysAddr(pMemDesc[i], AT_GPU, 0),
                  mmuFmtLevelVirtAddrLo(pLevelFmt, vaBase),
                  mmuFmtLevelVirtAddrHi(pLevelFmt, vaBase));
#endif // NV_PRINTF_STRINGS_ALLOWED

        //
        // If this is a submemdesc, return it to its free list only when
        // the refcount is 1. A refcount greater than 1 implies that 2 or
        // more GPUs in SLI are using it. GPUs in SLI can share a page level
        // instance.
        //
        if (memdescIsSubMemoryMemDesc(pMemDesc[i]) &&
           (pMemDesc[i]->RefCount == 1))
        {
            // Return this to the free list from which it was borrowed
            listAppendExisting(memdescGetParentDescriptor(pMemDesc[i])->pSubMemDescList, pMemDesc[i]);
        }
        else
        {
            if (RMCFG_FEATURE_PMA &&
                (pUserCtx->pGVAS->flags & VASPACE_FLAGS_PTETABLE_PMA_MANAGED) &&
                (pMemDesc[i]->pPageHandleList != NULL) &&
                (listCount(pMemDesc[i]->pPageHandleList) != 0) &&
                (pUserCtx->pGVAS->pPageTableMemPool != NULL))
            {
                rmMemPoolFree(pUserCtx->pGVAS->pPageTableMemPool,
                              (RM_POOL_ALLOC_MEMDESC*)pMemDesc[i],
                              pUserCtx->pGVAS->flags);
            }

            if (!memdescIsSubMemoryMemDesc(pMemDesc[i]))
            {
                memdescFree(pMemDesc[i]);
            }
            memdescDestroy(pMemDesc[i]);
        }
    }
}

static NvBool
_gmmuWalkCBUpdatePdb
(
    MMU_WALK_USER_CTX       *pUserCtx,
    const MMU_FMT_LEVEL     *pRootFmt,
    const MMU_WALK_MEMDESC  *pRootMem,
    const NvBool             bIgnoreChannelBusy
)
{
    OBJGPU            *pGpu = pUserCtx->pGpu;
    MEMORY_DESCRIPTOR *pPDB = (MEMORY_DESCRIPTOR*)pRootMem;

    NV_PRINTF(LEVEL_INFO, "[GPU%u]: PA 0x%llX (%s)\n",
              pUserCtx->pGpu->gpuInstance,
              (NULL != pPDB) ? memdescGetPhysAddr(pPDB, AT_GPU, 0) : 0,
              (NULL != pPDB) ? "valid" : "null");

    if (pUserCtx->pGVAS->flags & VASPACE_FLAGS_BAR_BAR1)
    {
        //
        // Do nothing, as BAR1 pdb is static and is only created and
        // destroyed along with the vaspace itself. Since the bar1
        // instance memory is appropriately updated then, we do not
        // do anything inside update pdb for bar1 which will be invoked
        // for mmuwalksparsify and mmuwalkunmap.
        //
        return NV_TRUE;
    }
    else if ((pUserCtx->pGVAS->flags & VASPACE_FLAGS_HDA))
    {
        // Instance Block set up once by caller.
        return NV_TRUE;
    }
    else if (IS_VIRTUAL_WITH_SRIOV(pGpu) || IS_GSP_CLIENT(pGpu))
    {
        // Noop inside a guest or CPU RM.
        return NV_TRUE;
    }
        return NV_TRUE;
}

static NvBool
_gmmuWalkCBUpdatePde
(
    MMU_WALK_USER_CTX       *pUserCtx,
    const MMU_FMT_LEVEL     *pLevelFmt,
    const MMU_WALK_MEMDESC  *pLevelMem,
    const NvU32              entryIndex,
    const MMU_WALK_MEMDESC **pSubLevels
)
{
    NvU32              i;
    GMMU_ENTRY_VALUE   entry;
    NvBool             bMirror     = _mirrorLevel(pUserCtx, pLevelFmt);
    NvU8               maxPgDirs   = _getMaxPageDirs(bMirror);
    OBJGPU            *pGpu        = pUserCtx->pGpu;
    OBJGVASPACE       *pGVAS       = pUserCtx->pGVAS;
    KernelGmmu        *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
    const GMMU_FMT    *pFmt        = pUserCtx->pGpuState->pFmt;
    MEMORY_DESCRIPTOR *pMemDesc[GMMU_MAX_PAGE_DIR_INDEX_COUNT] = {NULL};
    NvU32                      recipExp  = NV_U32_MAX;
    const GMMU_FMT_PDE_MULTI  *pPdeMulti = pFmt->pPdeMulti;

    pMemDesc[GMMU_USER_PAGE_DIR_INDEX] = (MEMORY_DESCRIPTOR*)pLevelMem;
    if (bMirror)
    {
        pMemDesc[GMMU_KERNEL_PAGE_DIR_INDEX] =
            (MEMORY_DESCRIPTOR*)pUserCtx->pGpuState->pMirroredRoot;
    }

    for (i = 0; i < maxPgDirs; i++)
    {
#if NV_PRINTF_STRINGS_ALLOWED
        NV_PRINTF(LEVEL_INFO, "[GPU%u]: %sPA 0x%llX, Entry 0x%X\n",
                  pUserCtx->pGpu->gpuInstance,
                  bMirror ? g_gmmuUVMMirroringDirStrings[i] : "",
                  memdescGetPhysAddr(pMemDesc[i], AT_GPU, 0), entryIndex);
#else // NV_PRINTF_STRINGS_ALLOWED
        NV_PRINTF(LEVEL_INFO, "[GPU%u]: %cPA 0x%llX, Entry 0x%X\n",
                  pUserCtx->pGpu->gpuInstance,
                  bMirror ? _gmmuUVMMirroringDirString[i] : ' ',
                  memdescGetPhysAddr(pMemDesc[i], AT_GPU, 0), entryIndex);
#endif // NV_PRINTF_STRINGS_ALLOWED
    }

    portMemSet(entry.v8, 0, pLevelFmt->entrySize);

    for (i = 0; i < pLevelFmt->numSubLevels; ++i)
    {
        const GMMU_FMT_PDE *pPde        = gmmuFmtGetPde(pFmt, pLevelFmt, i);
        MEMORY_DESCRIPTOR  *pSubMemDesc = (MEMORY_DESCRIPTOR*)pSubLevels[i];

        if (NULL != pSubMemDesc)
        {
            const GMMU_APERTURE       aperture = kgmmuGetMemAperture(pKernelGmmu, pSubMemDesc);
            const GMMU_FIELD_ADDRESS *pFldAddr = gmmuFmtPdePhysAddrFld(pPde, aperture);
            const NvU64               physAddr = memdescGetPhysAddr(pSubMemDesc, AT_GPU, 0);

            if (pFmt->version == GMMU_FMT_VERSION_3)
            {
                NvU32 pdePcfHw    = 0;
                NvU32 pdePcfSw    = 0;

                pdePcfSw |= gvaspaceIsAtsEnabled(pGVAS) ? (1 << SW_MMU_PCF_ATS_ALLOWED_IDX) : 0;
                pdePcfSw |= memdescGetVolatility(pSubMemDesc) ? (1 << SW_MMU_PCF_UNCACHED_IDX) : 0;

                NV_ASSERT_OR_RETURN((kgmmuTranslatePdePcfFromSw_HAL(pKernelGmmu, pdePcfSw, &pdePcfHw) == NV_OK),
                                      NV_ERR_INVALID_ARGUMENT);
                nvFieldSet32(&pPde->fldPdePcf, pdePcfHw, entry.v8);
            }
            else
            {
                nvFieldSetBool(&pPde->fldVolatile, memdescGetVolatility(pSubMemDesc), entry.v8);
            }

            gmmuFieldSetAperture(&pPde->fldAperture, aperture, entry.v8);
            gmmuFieldSetAddress(pFldAddr,
                kgmmuEncodePhysAddr(pKernelGmmu, aperture, physAddr,
                    NVLINK_INVALID_FABRIC_ADDR),
                entry.v8);

            // Calculate partial page table size if supported.
            if ((pGVAS->flags & VASPACE_FLAGS_MINIMIZE_PTETABLE_SIZE) &&
                (pLevelFmt->numSubLevels > 1) &&
                nvFieldIsValid32(&pPdeMulti->fldSizeRecipExp))
            {
                const NvU32 maxMemSize  = mmuFmtLevelSize(&pLevelFmt->subLevels[i]);
                const NvU32 curMemSize  = (NvU32)pSubMemDesc->Size;
                const NvU32 minRecipExp = BIT_IDX_32(maxMemSize / curMemSize);

                // We should have allocated on a fractional (pow2) boundary.
                NV_ASSERT(ONEBITSET(curMemSize));

                if (recipExp == NV_U32_MAX)
                {
                    // Save exponent if not set yet.
                    recipExp = minRecipExp;
                }
                else
                {
                    // Otherwise ensure parallel sub-levels match.
                    NV_ASSERT(recipExp == minRecipExp);
                }
            }

            NV_PRINTF(LEVEL_INFO, "    SubLevel %u = PA 0x%llX\n", i,
                      physAddr);
        }
        else
        {
            NV_PRINTF(LEVEL_INFO, "    SubLevel %u = INVALID\n", i);
        }
    }

    // Set partial page table size exponent if needed.
    if (recipExp != NV_U32_MAX)
    {
        nvFieldSet32(&pPdeMulti->fldSizeRecipExp, recipExp, entry.v8);
    }

    for (i = 0; i < maxPgDirs; i++)
    {
        TRANSFER_SURFACE dest = {0};

        dest.pMemDesc = pMemDesc[i];
        dest.offset = entryIndex * pLevelFmt->entrySize;
        NV_ASSERT_OK(memmgrMemWrite(GPU_GET_MEMORY_MANAGER(pGpu), &dest,
                                    entry.v8, pLevelFmt->entrySize,
                                    TRANSFER_FLAGS_NONE));
    }

    return NV_TRUE;
}

static void
_gmmuWalkCBFillEntries
(
    MMU_WALK_USER_CTX         *pUserCtx,
    const MMU_FMT_LEVEL       *pLevelFmt,
    const MMU_WALK_MEMDESC    *pLevelMem,
    const NvU32                entryIndexLo,
    const NvU32                entryIndexHi,
    const MMU_WALK_FILL_STATE  fillState,
    NvU32                     *pProgress
)
{
    NvU32              i;
    NvU32              j;
    OBJGPU            *pGpu           = pUserCtx->pGpu;
    KernelGmmu        *pKernelGmmu    = GPU_GET_KERNEL_GMMU(pGpu);
    MemoryManager     *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    KernelBus         *pKernelBus     = GPU_GET_KERNEL_BUS(pGpu);
    const GMMU_FMT    *pFmt      = pUserCtx->pGpuState->pFmt;
    NvBool             bMirror   = _mirrorLevel(pUserCtx, pLevelFmt);
    NvU8               maxPgDirs = _getMaxPageDirs(bMirror);
    MEMORY_DESCRIPTOR *pMemDesc[GMMU_MAX_PAGE_DIR_INDEX_COUNT] = {NULL};
    NvU32              sizeOfEntries = (entryIndexHi - entryIndexLo + 1) *
                                        pLevelFmt->entrySize;
    NvU8              *pEntries;

    pMemDesc[GMMU_USER_PAGE_DIR_INDEX] = (MEMORY_DESCRIPTOR*)pLevelMem;
    if (bMirror)
    {
        pMemDesc[GMMU_KERNEL_PAGE_DIR_INDEX] =
            (MEMORY_DESCRIPTOR*)pUserCtx->pGpuState->pMirroredRoot;
    }

    for (j = 0; j < maxPgDirs; j++)
    {
        TRANSFER_SURFACE dest = {0};

        dest.pMemDesc = pMemDesc[j];
        dest.offset = entryIndexLo * pLevelFmt->entrySize;

        //
        // A shadow buffer is allocated to store the PTEs in case of writes
        // using CE and GSP DMA task. This code gets called in a high IRQL
        // path on Windows and shadow buffer allocation may fail there.
        //
        pEntries = memmgrMemBeginTransfer(pMemoryManager, &dest, sizeOfEntries,
                                          TRANSFER_FLAGS_SHADOW_ALLOC);
        NV_ASSERT_OR_RETURN_VOID(pEntries != NULL);

#if NV_PRINTF_STRINGS_ALLOWED
        NV_PRINTF(LEVEL_INFO,
                  "[GPU%u]: %sPA 0x%llX, Entries 0x%X-0x%X = %s\n",
                  pUserCtx->pGpu->gpuInstance,
                  bMirror ? g_gmmuUVMMirroringDirStrings[j] : "",
                  memdescGetPhysAddr(pMemDesc[j], AT_GPU, 0),
                  entryIndexLo, entryIndexHi,
                  g_gmmuFillStateStrings[fillState]);
#else // NV_PRINTF_STRINGS_ALLOWED
        NV_PRINTF(LEVEL_INFO,
                  "[GPU%u] %cPA 0x%llX, Entries 0x%X-0x%X = %c\n",
                  pUserCtx->pGpu->gpuInstance,
                  bMirror ? _gmmuUVMMirroringDirString[j] : ' ',
                  memdescGetPhysAddr(pMemDesc[j], AT_GPU, 0),
                  entryIndexLo, entryIndexHi,
                  _gmmuFillStateString[fillState]);
#endif // NV_PRINTF_STRINGS_ALLOWED

        switch (fillState)
        {
            case MMU_WALK_FILL_INVALID:
                portMemSet(pEntries, 0, sizeOfEntries);
                break;
            case MMU_WALK_FILL_SPARSE:
            {
                const GMMU_FMT_FAMILY  *pFam = kgmmuFmtGetFamily(pKernelGmmu, pFmt->version);
                const GMMU_ENTRY_VALUE *pSparseEntry;
                // Fake sparse entry is needed for GH100 in CC mode for PDE2-PDE4. Ref: Bug 3341692
                NvU8 *pFakeSparse = kgmmuGetFakeSparseEntry_HAL(pGpu, pKernelGmmu, pLevelFmt);

                if (pFakeSparse != NULL)
                {
                    pSparseEntry = (const GMMU_ENTRY_VALUE *) pFakeSparse;
                }
                else
                {
                    // Select sparse entry template based on number of sub-levels.
                    if (pLevelFmt->numSubLevels > 1)
                    {
                        pSparseEntry = &pFam->sparsePdeMulti;
                    }
                    else if (pLevelFmt->numSubLevels == 1)
                    {
                        pSparseEntry = &pFam->sparsePde;
                    }
                    else
                    {
                        if (kbusIsFlaDummyPageEnabled(pKernelBus) &&
                            (pUserCtx->pGVAS->flags & VASPACE_FLAGS_FLA))
                            pSparseEntry = &pUserCtx->pGpuState->flaDummyPage.pte;
                        else
                            pSparseEntry = &pFam->sparsePte;
                    }
                }

                // Copy sparse template to each entry.
                for (i = entryIndexLo; i <= entryIndexHi; ++i)
                {
                    NvU32 entryIndex = (i - entryIndexLo) * pLevelFmt->entrySize;
                    portMemCopy(&pEntries[entryIndex],
                                pLevelFmt->entrySize,
                                pSparseEntry->v8,
                                pLevelFmt->entrySize);
                }
                break;
            }
            case MMU_WALK_FILL_NV4K:
            {
                const GMMU_FMT_FAMILY  *pFam =
                    kgmmuFmtGetFamily(pKernelGmmu, pFmt->version);
                const GMMU_ENTRY_VALUE *pNv4kEntry = &pFam->nv4kPte;

                // debug print - to remove when the code is robust enough
                if (!gvaspaceIsAtsEnabled(pUserCtx->pGVAS) ||
                     mmuFmtLevelPageSize(pLevelFmt) != RM_PAGE_SIZE_64K)
                {
#if NV_PRINTF_STRINGS_ALLOWED
                    NV_PRINTF(LEVEL_ERROR,
                              "[GPU%u]: %sPA 0x%llX, Entries 0x%X-0x%X = %s FAIL\n",
                              pUserCtx->pGpu->gpuInstance,
                              bMirror ? g_gmmuUVMMirroringDirStrings[j] : "",
                              memdescGetPhysAddr(pMemDesc[j], AT_GPU, 0),
                              entryIndexLo, entryIndexHi,
                              g_gmmuFillStateStrings[fillState]);
#else // NV_PRINTF_STRINGS_ALLOWED
                    NV_PRINTF(LEVEL_ERROR,
                              "[GPU%u]: %cPA 0x%llX, Entries 0x%X-0x%X = %c FAIL\n",
                              pUserCtx->pGpu->gpuInstance,
                              bMirror ? _gmmuUVMMirroringDirString[j] : ' ',
                              memdescGetPhysAddr(pMemDesc[j], AT_GPU, 0),
                              entryIndexLo, entryIndexHi,
                              _gmmuFillStateString[fillState]);
#endif // NV_PRINTF_STRINGS_ALLOWED

                    DBG_BREAKPOINT();
                    return;
                }

                // Copy nv4k template to each entry
                for (i = entryIndexLo; i <= entryIndexHi; ++i)
                {
                    NvU32 entryIndex = (i - entryIndexLo) * pLevelFmt->entrySize;
                    portMemCopy(&pEntries[entryIndex],
                                pLevelFmt->entrySize,
                                pNv4kEntry->v8,
                                pLevelFmt->entrySize);
                }
                break;
            }
            default:
                NV_ASSERT(0);
                break;
        }

        memmgrMemEndTransfer(pMemoryManager, &dest, sizeOfEntries,
                             TRANSFER_FLAGS_SHADOW_ALLOC);
    }

    *pProgress = entryIndexHi - entryIndexLo + 1;
}

static void
_gmmuWalkCBCopyEntries
(
    MMU_WALK_USER_CTX         *pUserCtx,
    const MMU_FMT_LEVEL       *pLevelFmt,
    const MMU_WALK_MEMDESC    *pSrcMem,
    const MMU_WALK_MEMDESC    *pDstMem,
    const NvU32                entryIndexLo,
    const NvU32                entryIndexHi,
    NvU32                     *pProgress
)
{
    MEMORY_DESCRIPTOR *pSrcDesc = (MEMORY_DESCRIPTOR *)pSrcMem;
    MEMORY_DESCRIPTOR *pDstDesc = (MEMORY_DESCRIPTOR *)pDstMem;
    TRANSFER_SURFACE   src      = {0};
    TRANSFER_SURFACE   dest     = {0};

    src.pMemDesc = pSrcDesc;
    src.offset = entryIndexLo * pLevelFmt->entrySize;
    dest.pMemDesc = pDstDesc;
    dest.offset = entryIndexLo * pLevelFmt->entrySize;

    // Only copy if different source and destination memory.
    if (!memdescDescIsEqual(pSrcDesc, pDstDesc))
    {
        OBJGPU *pGpu = pUserCtx->pGpu;
        NvU32   sizeOfEntries = (entryIndexHi - entryIndexLo + 1) *
                                 pLevelFmt->entrySize;

        NV_PRINTF(LEVEL_INFO,
                  "[GPU%u]: GVAS(%p) PA 0x%llX -> PA 0x%llX, Entries 0x%X-0x%X\n",
                  pGpu->gpuInstance, pUserCtx->pGVAS,
                  memdescGetPhysAddr(pSrcDesc, AT_GPU, 0),
                  memdescGetPhysAddr(pDstDesc, AT_GPU, 0), entryIndexLo,
                  entryIndexHi);

        NV_ASSERT_OK(memmgrMemCopy(GPU_GET_MEMORY_MANAGER(pGpu), &dest, &src,
                                   sizeOfEntries, TRANSFER_FLAGS_NONE));
    }

    // Report full range complete.
    *pProgress = entryIndexHi - entryIndexLo + 1;
}

const MMU_WALK_CALLBACKS g_gmmuWalkCallbacks =
{
    _gmmuWalkCBLevelAlloc,
    _gmmuWalkCBLevelFree,
    _gmmuWalkCBUpdatePdb,
    _gmmuWalkCBUpdatePde,
    _gmmuWalkCBFillEntries,
    _gmmuWalkCBCopyEntries,
    NULL,
};
