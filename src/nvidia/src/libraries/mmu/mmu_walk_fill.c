/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2015 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/* ------------------------ Includes --------------------------------------- */
#if defined(SRT_BUILD)
#include "shrdebug.h"
#endif
#include "mmu_walk_private.h"

static NV_STATUS _mmuWalkPostFillPTETasks
(
    const MMU_WALK             *pWalk,
    MMU_WALK_LEVEL             *pLevel,
    const NvU32                 entryIndexLo,
    const NvU32                 entryIndexHi,
    const MMU_WALK_FILL_STATE   fillState,
    const NvU64                 vaLo
);

/* ------------------------ Public Functions  ------------------------------ */
/**
 * @brief      Fill a VA range to a constant state for levels below the root.
 *
 * @details    This function is of MmuWalkOp function type. Used by
 *             mmuWalkUnmap and mmuWalkSparsify, which fills INVALID and SPARSE
 *             states to the target page levels respectively.
 *             With NV4K introduced with VOLTA ATS, cross PT inconsistency
 *             during unmapping and sparsifying is handled here.
 *
 * @todo       Recover from failure. It is difficult to do because rollbacks
 *             are costly and complex. Do we really want recovery or asserts?
 *             If the later one, we can replace those recovery codes with
 *             asserts.
 *
 * @copydoc    MmuWalkCBOpFunc
 *
 * @return     NV_OK on success
 */
NV_STATUS mmuWalkFill
(
    const MMU_WALK            *pWalk,
    const MMU_WALK_OP_PARAMS  *pOpParams,
    MMU_WALK_LEVEL            *pLevel,
    MMU_WALK_LEVEL_INST       *pLevelInst,
    NvU64                      vaLo,
    NvU64                      vaHi
)
{
    const MMU_FILL_TARGET *pTarget = (const MMU_FILL_TARGET *) pOpParams->pOpCtx;

    if (NULL == pLevelInst)
        return NV_OK;

    // If this level is a Page Directory, we keep walking down the tree.
    if (0 != pLevel->pFmt->numSubLevels)
    {
        if (pWalk->flags.bUseIterative)
        {
            return NV_ERR_MORE_PROCESSING_REQUIRED;
        }
        else
        {
            // Process all the page level entries.falling within [vaLo, vaHi]
            NV_ASSERT_OK_OR_RETURN(
                mmuWalkProcessPdes(pWalk,
                                   pOpParams,
                                   pLevel,
                                   pLevelInst,
                                   vaLo,
                                   vaHi));
        }
    }
    // We have reached a page table
    else
    {
        const NvU32 entryIndexLo = mmuFmtVirtAddrToEntryIndex(pLevel->pFmt, vaLo);
        const NvU32 entryIndexHi = mmuFmtVirtAddrToEntryIndex(pLevel->pFmt, vaHi);
        NvU32       progress     = 0;
        NV_STATUS   status       = NV_OK;
        // Calculate number of entries in the level
        NvU64       numEntries   = mmuFmtLevelEntryCount(pLevel->pFmt);
        NvU32       entryIndex;

        NV_ASSERT_OR_RETURN(pLevel->pFmt->bPageTable, NV_ERR_INVALID_ARGUMENT);

        // Make sure all PTEs are contained within one page table
        NV_ASSERT_OR_RETURN((entryIndexLo / numEntries) ==
                            (entryIndexHi / numEntries),
                            NV_ERR_INVALID_ARGUMENT);

        if (pWalk->pStagingBuffer != NULL && pWalk->bUseStagingBuffer)
        {
            //
            // Clear out the PTEs modulo number of entries in table
            // We do a modulo of number of entries in the table so that
            // we do not exceed the allocated sysmem page.
            //
            pWalk->pCb->FillEntries(pWalk->pUserCtx,
                                    pLevel->pFmt,
                                    pWalk->pStagingBuffer,
                                    entryIndexLo % numEntries,
                                    entryIndexHi % numEntries,
                                    pTarget->fillState,
                                    &progress);

            // Copy from staging buffer to final location
            pWalk->pCb->WriteBuffer(pWalk->pUserCtx,
                                    pWalk->pStagingBuffer,
                                    pLevelInst->pMemDesc,
                                    entryIndexLo,
                                    entryIndexHi,
                                    numEntries,
                                    pLevel->pFmt->entrySize);
        }
        else
        {
            // Clear out the PTEs
            pWalk->pCb->FillEntries(pWalk->pUserCtx,
                                    pLevel->pFmt,
                                    pLevelInst->pMemDesc,
                                    entryIndexLo,
                                    entryIndexHi,
                                    pTarget->fillState,
                                    &progress);
        }

        NV_ASSERT_OR_RETURN(progress == entryIndexHi - entryIndexLo + 1, NV_ERR_INVALID_STATE);

        // Update the state tracker
        for (entryIndex = entryIndexLo; entryIndex <= entryIndexHi; entryIndex++)
        {
            mmuWalkSetEntryState(pLevelInst, entryIndex, pTarget->entryState);
        }

        // Post fill tasks
        status = _mmuWalkPostFillPTETasks(pWalk, pLevel, entryIndexLo,
                                          entryIndexHi, pTarget->fillState, vaLo);
        NV_ASSERT_OR_RETURN(status == NV_OK, NV_ERR_INVALID_STATE);
    }

    return NV_OK;
}

/*!
 * TODO
 */
NV_STATUS
mmuWalkFillSelectSubLevel
(
    const void             *pOpCtx,
    const MMU_WALK_LEVEL   *pLevel,
    NvU32                  *pSubLevel,
    NvU64                   vaLo,
    NvU64                   vaHi
)
{
    NvBool               bFound = NV_FALSE;
    NvU32                subLevel;
    const MMU_FMT_LEVEL *pSubLevelFmt = NULL;

    // If we've only one sublevel, it's at index 0.
    if (pLevel->pFmt->numSubLevels == 1)
    {
        *pSubLevel = 0;
        return NV_OK;
    }

    for (subLevel = 0; subLevel < pLevel->pFmt->numSubLevels; subLevel++)
    {
        pSubLevelFmt = pLevel->pFmt->subLevels + subLevel;

        if ((( vaLo    & mmuFmtEntryVirtAddrMask(pSubLevelFmt)) == 0) &&
            (((vaHi+1) & mmuFmtEntryVirtAddrMask(pSubLevelFmt)) == 0))
        {
            MMU_WALK_LEVEL_INST *pSubLevelInst = NULL;
            if (!bFound)
            {
                bFound     = NV_TRUE;
                *pSubLevel = subLevel;
            }
            if (NV_OK == btreeSearch(vaLo, (NODE**)&pSubLevelInst,
                                     (NODE*)pLevel->subLevels[subLevel].pInstances))
            {
                *pSubLevel = subLevel;
                return NV_OK;
            }
        }
    }

    // Error if virt addresses are not aligned to any page size.
    return bFound ? NV_OK : NV_ERR_INVALID_STATE;
}

/**
 * @brief      Determines if entries indexLo to indexHi (inclusive) are
 *             all invalid.
 *
 * @param      pLevelInst  The level instance
 * @param[in]  indexLo     The index lower
 * @param[in]  indexHi     The index higher
 *
 * @return     True if no level instance or all entries are invalid,
 *             False otherwise.
 */
static NvBool
_isRangeAllInvalid
(
    MMU_WALK_LEVEL_INST    *pLevelInst,
    const NvU32             indexLo,
    const NvU32             indexHi
)
{
    NvU32 i;
    MMU_ENTRY_STATE entryState;

    if (pLevelInst == NULL)
        return NV_TRUE;

    for (i = indexLo; i <= indexHi; i++)
    {
        entryState = mmuWalkGetEntryState(pLevelInst, i);
        if (MMU_ENTRY_STATE_INVALID != entryState)
            return NV_FALSE;
    }
    return NV_TRUE;
}

/**
 * @brief      Post PTE filling tasks to handle cross PTs inconsistency
 *
 * @details    Helper function inside mmuWalkFill PT level to update 64K PTEs
 *             after mmuWalkFill operation complete. It gathers mmuWalkFill
 *             target entry index range and fillState as input and update
 *             64K PTEs accordingly. The function doesn't handle extra page
 *             table allocations and deallocations. It relies on
 *             _mmuWalkPdeAcquire and _mmuWalkPdeRelease to prepare and
 *             cleanup page levels accordingly.
 *
 * @todo       Recovery on failure. Same discussion as in mmuWalkFill.
 *
 * @param[in]  pWalk         The walk
 * @param      pLevel        The level, used to get fmt and btree root
 * @param[in]  entryIndexLo  The lower entry index (inclusive)
 * @param[in]  entryIndexHi  The entry higher index (inclusive)
 * @param[in]  fillState     The fill state
 * @param[in]  virtAddr      The lower VA, to get the key for btrees
 *
 * @return     NV_OK on success, NV_ERR_INVALID_ARGUMENT otherwise
 */
static NV_STATUS _mmuWalkPostFillPTETasks
(
    const MMU_WALK             *pWalk,
    MMU_WALK_LEVEL             *pLevel,
    const NvU32                 entryIndexLo,
    const NvU32                 entryIndexHi,
    const MMU_WALK_FILL_STATE   fillState,
    const NvU64                 virtAddr
)
{
    const MMU_FMT_LEVEL *pFmtLevel = pLevel->pFmt;

    //
    // NV4K is only necessary for ATS
    // Only update 64K PTEs on invalidation, not on sparsifying
    //
    if (pWalk->flags.bAtsEnabled && fillState == MMU_WALK_FILL_INVALID)
    {
        const NvU64 pageSize    = mmuFmtLevelPageSize(pFmtLevel);

        NvU32 progress          = 0;
        NvU32 entryIndex        = 0;
        NvU32 indexLo_4K, indexHi_4K, indexLo_64K, indexHi_64K;

        MMU_WALK_LEVEL *pParent     = pLevel->pParent;
        MMU_WALK_LEVEL *pLevel64K   = pParent->subLevels;
        MMU_WALK_LEVEL *pLevel4K    = pParent->subLevels + 1;
        NvU64 vaLevelBase = mmuFmtLevelVirtAddrLo(pLevel64K->pFmt, virtAddr);
        MMU_WALK_LEVEL_INST *pLevel64KInst  = NULL;
        MMU_WALK_LEVEL_INST *pLevel4KInst   = NULL;

        // search for the instances
        btreeSearch(vaLevelBase, (NODE**)&pLevel64KInst,
            (NODE*)pLevel64K->pInstances);
        btreeSearch(vaLevelBase, (NODE**)&pLevel4KInst,
            (NODE*)pLevel4K->pInstances);

        //
        // if 4K page table was modified in mmuWalkFill
        // check the range and update 64K PTEs accordingly
        //
        if (pageSize == 0x1000)
        {
            // get involved 64K PTEs and 4K PTEs
            mmuFmtCalcAlignedEntryIndices(pLevel4K->pFmt, entryIndexLo,
                entryIndexHi, pLevel64K->pFmt, &indexLo_64K, &indexHi_64K);
            mmuFmtCalcAlignedEntryIndices(pLevel64K->pFmt, indexLo_64K,
                indexHi_64K, pLevel4K->pFmt, &indexLo_4K, &indexHi_4K);

            // if only one 64K PTE involved, check a single 16 4K PTE group
            if (indexLo_64K == indexHi_64K)
            {
                if (!_isRangeAllInvalid(pLevel4KInst, indexLo_4K, indexHi_4K))
                {
                    indexLo_64K++;
                }
            }
            // otherwise check the head and tail groups
            else
            {
                if (indexLo_4K < entryIndexLo &&
                    !_isRangeAllInvalid(pLevel4KInst, indexLo_4K,
                        entryIndexLo - 1))
                {
                    indexLo_64K++;
                }
                if (indexHi_4K > entryIndexHi &&
                    !_isRangeAllInvalid(pLevel4KInst, entryIndexHi + 1,
                        indexHi_4K))
                {
                    indexHi_64K--;
                }
            }

            // update 64K PT given the indexes calculated above
            if (indexLo_64K <= indexHi_64K)
            {
                pWalk->pCb->FillEntries(pWalk->pUserCtx, pLevel64K->pFmt,
                    pLevel64KInst->pMemDesc, indexLo_64K, indexHi_64K,
                    MMU_WALK_FILL_NV4K, &progress);
                NV_ASSERT_OR_RETURN(progress == indexHi_64K - indexLo_64K + 1,
                    NV_ERR_INVALID_STATE);
                // update entry states
                for (entryIndex = indexLo_64K; entryIndex <= indexHi_64K;
                     entryIndex++)
                {
                    mmuWalkSetEntryState(pLevel64KInst, entryIndex,
                        MMU_ENTRY_STATE_NV4K);
                }
            }
        }
        //
        // if 64K page table is invalidated in mmuWalkFill
        // correct the state as NV4K
        // @todo move this portion to mmuWalkFill
        //
        else if (pageSize == 0x10000)
        {
            mmuFmtCalcAlignedEntryIndices(pLevel64K->pFmt, entryIndexLo,
                entryIndexHi, pLevel4K->pFmt, &indexLo_4K, &indexHi_4K);

            // the 4K PTE should have already been invalid
            NV_ASSERT_OR_RETURN(_isRangeAllInvalid(pLevel4KInst, indexLo_4K,
                    indexHi_4K), NV_ERR_INVALID_STATE);

            // Set 64K PTEs NV4K
            pWalk->pCb->FillEntries(pWalk->pUserCtx, pLevel64K->pFmt,
                pLevel64KInst->pMemDesc, entryIndexLo, entryIndexHi,
                MMU_WALK_FILL_NV4K, &progress);
            NV_ASSERT_OR_RETURN(progress == entryIndexHi - entryIndexLo + 1,
                NV_ERR_INVALID_STATE);

            for (entryIndex = entryIndexLo; entryIndex <= entryIndexHi;
                 entryIndex++)
            {
                mmuWalkSetEntryState(pLevel64KInst, entryIndex,
                    MMU_ENTRY_STATE_NV4K);
            }
        }
        // NV4K only works with 64K PT + 4K PT comibination
        else
        {
            NV_ASSERT_OR_RETURN(0, NV_ERR_INVALID_ARGUMENT);
        }
    }
    return NV_OK;
}
