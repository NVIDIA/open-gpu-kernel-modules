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

/*!
 * @file
 * @brief Describes the structures and interfaces used to walk N level page tables
 */

/*--------------------------------Includes------------------------------------*/
#if defined(SRT_BUILD)

#include "shrdebug.h"
#else
#include "os/os.h"
#endif
#include "nvport/nvport.h"
#include "nvctassert.h"
#include "mmu_walk_private.h"

/*--------------------------Static Function Prototypes------------------------*/
static NV_STATUS
_mmuWalkLevelInit(const MMU_WALK *pWalk, MMU_WALK_LEVEL *pParent,
                  const MMU_FMT_LEVEL *pLevelFmt, MMU_WALK_LEVEL *pLevel);
static void
_mmuWalkLevelDestroy(const MMU_WALK *pWalk, MMU_WALK_LEVEL *pLevel);
static NV_STATUS
_mmuWalkLevelInstAcquire(const MMU_WALK *pWalk, MMU_WALK_LEVEL *pLevel,
                         const NvU64 vaLo, const NvU64 vaHi, const NvBool bTarget,
                         const NvBool bRelease, const NvBool bCommit,
                         NvBool *pBChanged, MMU_WALK_LEVEL_INST **ppLevelInst,
                         const NvBool bInitNv4k);
static void
_mmuWalkLevelInstRelease(const MMU_WALK *pWalk, MMU_WALK_LEVEL *pLevel,
                         MMU_WALK_LEVEL_INST *pLevelInst);
static NV_STATUS NV_NOINLINE
_mmuWalkPdeAcquire(const MMU_WALK *pWalk, const MMU_WALK_OP_PARAMS *pOpParams,
                   MMU_WALK_LEVEL *pLevel, MMU_WALK_LEVEL_INST *pLevelInst,
                   const NvU32 entryIndex, const NvU32 subLevel,
                   const NvU64 vaLo, const NvU64 vaHi,
                   MMU_WALK_LEVEL_INST *pSubLevelInsts[]);
static void NV_NOINLINE
_mmuWalkPdeRelease(const MMU_WALK *pWalk, const MMU_WALK_OP_PARAMS *pOpParams,
                   MMU_WALK_LEVEL *pLevel, MMU_WALK_LEVEL_INST *pLevelInst,
                   const NvU32 entryIndex, const NvU64 entryVaLo);
static NV_STATUS NV_NOINLINE
_mmuWalkResolveSubLevelConflicts(const MMU_WALK *pWalk, const MMU_WALK_OP_PARAMS *pOpParams,
                                 MMU_WALK_LEVEL *pLevel, MMU_WALK_LEVEL_INST *pSubLevelInsts[],
                                 NvU32 subLevel, NvU64 clippedVaLo, NvU64 clippedVaHi);
static void
_mmuWalkLevelInstancesForceFree(MMU_WALK *pWalk, MMU_WALK_LEVEL *pLevel);

/* -----------------------------Inline Functions----------------------------- */
/*!
   Returns the @ref MMU_ENTRY_STATE of the entry.
 */
MMU_ENTRY_STATE
mmuWalkGetEntryState(MMU_WALK_LEVEL_INST *pLevelInst, NvU32 entryIndex)
{
    return (MMU_ENTRY_STATE)pLevelInst->pStateTracker[entryIndex].state;
}

/*----------------------------Public Functions--------------------------------*/

NV_STATUS
mmuWalkCreate
(
    const MMU_FMT_LEVEL      *pRootFmt,
    MMU_WALK_USER_CTX        *pUserCtx,
    const MMU_WALK_CALLBACKS *pCb,
    const MMU_WALK_FLAGS      flags,
    MMU_WALK                **ppWalk,
    MMU_WALK_MEMDESC         *pStagingBuffer
)
{
    NV_STATUS  status = NV_OK;
    MMU_WALK  *pWalk  = NULL;

    NV_ASSERT_OR_RETURN(NULL != pRootFmt, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(NULL != pCb,      NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(NULL != ppWalk,   NV_ERR_INVALID_ARGUMENT);

    // Alloc and init walker structure.
    pWalk = portMemAllocNonPaged(sizeof(*pWalk));
    status = (pWalk == NULL) ? NV_ERR_NO_MEMORY : NV_OK;
    NV_ASSERT_OR_GOTO(NV_OK == status, done);
    portMemSet(pWalk, 0, sizeof(*pWalk));

    pWalk->pUserCtx             = pUserCtx;
    pWalk->pCb                  = pCb;
    pWalk->flags                = flags;
    pWalk->pStagingBuffer       = pStagingBuffer;
    pWalk->bUseStagingBuffer    = NV_FALSE;
    pWalk->bInvalidateOnReserve = NV_TRUE;

    // Create level hierarchy.
    status = _mmuWalkLevelInit(pWalk, NULL, pRootFmt, &pWalk->root);
    NV_ASSERT_OR_GOTO(NV_OK == status, done);

    // Commit.
    *ppWalk = pWalk;

done:
    if (NV_OK != status)
    {
        mmuWalkDestroy(pWalk);
    }
    return status;
}

void
mmuWalkDestroy
(
    MMU_WALK *pWalk
)
{
    if (NULL != pWalk)
    {
        // Destroy level hierarchy.
        _mmuWalkLevelDestroy(pWalk, &pWalk->root);

        // Free walker struct.
        portMemFree(pWalk);
    }
}

NV_STATUS
mmuWalkContinue
(
    MMU_WALK *pWalk
)
{
    return NV_ERR_NOT_SUPPORTED;
}

void
mmuWalkCommit
(
    MMU_WALK *pWalk
)
{
    // TODO
}

MMU_WALK_USER_CTX *
mmuWalkGetUserCtx
(
    const MMU_WALK *pWalk
)
{
    return pWalk->pUserCtx;
}

NV_STATUS
mmuWalkSetUserCtx
(
    MMU_WALK          *pWalk,
    MMU_WALK_USER_CTX *pUserCtx
)
{
    NV_ASSERT_OR_RETURN(NULL != pWalk, NV_ERR_INVALID_STATE);

    pWalk->pUserCtx = pUserCtx;
    return NV_OK;
}

const MMU_WALK_CALLBACKS *
mmuWalkGetCallbacks
(
    const MMU_WALK *pWalk
)
{
    return pWalk->pCb;
}

void
mmuWalkSetCallbacks
(
    MMU_WALK                 *pWalk,
    const MMU_WALK_CALLBACKS *pCb
)
{
    pWalk->pCb = pCb;
}

void
mmuWalkLevelInstancesForceFree
(
    MMU_WALK *pWalk
)
{
    if (pWalk != NULL)
    {
        _mmuWalkLevelInstancesForceFree(pWalk, &pWalk->root);
    }
}

/*----------------------------Private Functions--------------------------------*/

const MMU_WALK_LEVEL *
mmuWalkFindLevel
(
    const MMU_WALK      *pWalk,
    const MMU_FMT_LEVEL *pLevelFmt
)
{
    const MMU_WALK_LEVEL *pLevel = &pWalk->root;
    while (pLevel != NULL && pLevel->pFmt != pLevelFmt)
    {
        NvU32 subLevel;

        NV_ASSERT_OR_RETURN(pLevel->pFmt != NULL, NULL);

        // Single sub-level always continues.
        if (1 == pLevel->pFmt->numSubLevels)
        {
            pLevel = pLevel->subLevels;
            continue;
        }
        // Multi sub-level must pick branch based on target.
        for (subLevel = 0; subLevel < pLevel->pFmt->numSubLevels; ++subLevel)
        {
            if ((pLevel->pFmt->subLevels + subLevel) == pLevelFmt)
            {
                return pLevel->subLevels + subLevel;
            }
        }
        // Nothing found.
        return NULL;
    }
    return pLevel;
}

/*!
 * @brief This function traverses the topology described by @ref
 * MMU_FMT_LEVEL and @ref MMU_DESC_PDE. The @ref MmuOpFunc
 * opFunc implements the actions needed to be perfomed at each
 * sublevel in the recursion.
 *
 * @param[in]  vaLo          The lower end of the Virtual Address range that is
 *                           being processed.
 * @param[in]  vaHi          The upper end of the Virtual Address range that is
 *                           being processed
 *
 * @return NV_OK if processing this level succeeds.
 *         Other errors, if not.
 */
NV_STATUS mmuWalkProcessPdes
(
    const MMU_WALK           *pWalk,
    const MMU_WALK_OP_PARAMS *pOpParams,
    MMU_WALK_LEVEL           *pLevel,
    MMU_WALK_LEVEL_INST      *pLevelInst,
    NvU64                     vaLo,
    NvU64                     vaHi
)
{
    if (pWalk->flags.bUseIterative)
    {
        NV_STATUS status = NV_OK;
        const MMU_WALK_LEVEL *pLevelOrig = pLevel;
        NV_ASSERT_OR_RETURN(pOpParams != NULL, NV_ERR_INVALID_ARGUMENT);

        // Call opFunc inititially to see if we need to walk
        status = pOpParams->opFunc(pWalk,
                                   pOpParams,
                                   pLevel,
                                   pLevelInst,
                                   vaLo,
                                   vaHi);

        //
        // If NV_ERR_MORE_PROCESSING_REQUIRED is returned above,
        // the recursive MMU Walker would have started recursing down,
        // so here we kick off the iteration.
        // If NV_OK is returned above, the recursive MMU Walker would
        // not recurse at all, so return immediately.
        //
        if (NV_ERR_MORE_PROCESSING_REQUIRED == status)
        {
            status = NV_OK;

            NvU64 vaLevelBase  = mmuFmtLevelVirtAddrLo(pLevel->pFmt, vaLo);
            NvU32 entryIndexLo = mmuFmtVirtAddrToEntryIndex(pLevel->pFmt, vaLo);
            NvU32 entryIndexHi = mmuFmtVirtAddrToEntryIndex(pLevel->pFmt, vaHi);
            NvU32 entryIndex   = entryIndexLo;
            NvU32 entryIndexFillStart = 0;
            NvU32 entryIndexFillEnd   = 0;
            NvU32 pendingFillCount    = 0;

            //
            // entryIndex, entryIndexHi are modified in the loop itself
            // as we iterate through levels.
            //
            while (entryIndex <= entryIndexHi)
            {
                const NvU64           entryVaLo   = mmuFmtEntryIndexVirtAddrLo(pLevel->pFmt,
                                                                         vaLevelBase, entryIndex);
                const NvU64           entryVaHi   = mmuFmtEntryIndexVirtAddrHi(pLevel->pFmt,
                                                                         vaLevelBase, entryIndex);
                const NvU64           clippedVaLo = NV_MAX(vaLo, entryVaLo);
                const NvU64           clippedVaHi = NV_MIN(vaHi, entryVaHi);
                const MMU_ENTRY_STATE currEntryState = mmuWalkGetEntryState(pLevelInst, entryIndex);
                NvU32                 subLevel       = 0;
                MMU_WALK_LEVEL_INST  *pSubLevelInsts[MMU_FMT_MAX_SUB_LEVELS] = {0};

                // Optimizations for release operations.
                if (pOpParams->bRelease)
                {
                    // Skip this entry if it is neither a PDE nor marked as a hybrid entry.
                    if ((MMU_ENTRY_STATE_IS_PDE != currEntryState) &&
                        !pLevelInst->pStateTracker[entryIndex].bHybrid)
                    {
                        goto check_last_entry;
                    }
                }

                // Optimizations for fill operations.
                if (pOpParams->bFill)
                {
                    const MMU_FILL_TARGET *pTarget = (const MMU_FILL_TARGET *) pOpParams->pOpCtx;

                    if (pendingFillCount == 0)
                        entryIndexFillStart = entryIndexFillEnd = entryIndex;

                    //
                    // Check if the entire entry's coverage is being filled to
                    // a constant state.
                    //
                    // If this entry is not currently a PDE we can
                    // apply the fill operation directly
                    // at this level and avoid "splitting" the PDE.
                    //
                    // If this entry is currently a PDE we must
                    // clear the entries of the lower levels to free
                    // unused level instances.
                    //
                    if ((pTarget->entryState != currEntryState) &&
                        (MMU_ENTRY_STATE_IS_PDE != currEntryState) &&
                        (entryVaLo == clippedVaLo) &&
                        (entryVaHi == clippedVaHi))
                    {
                        entryIndexFillEnd = entryIndex;
                        pendingFillCount++;

                        // Not the last iteration, keep batching..
                        if (entryIndex < entryIndexHi)
                        {
                            //
                            // This won't be the last entry, but we'll
                            // do the iteration there
                            //
                            goto check_last_entry;
                        }
                    }

                    if (pendingFillCount != 0)
                    {
                        NvU32 progress = 0;
                        NvU32 index;

                        // Flush pending fills
                        pWalk->pCb->FillEntries(pWalk->pUserCtx,
                                                pLevel->pFmt,
                                                pLevelInst->pMemDesc,
                                                entryIndexFillStart,
                                                entryIndexFillEnd,
                                                pTarget->fillState,
                                                &progress);

                        if (progress != (entryIndexFillEnd - entryIndexFillStart + 1))
                        {
                            status = NV_ERR_INVALID_STATE;
                            NV_ASSERT_OR_GOTO(0, cleanupIter);
                        }

                        for (index = entryIndexFillStart; index <= entryIndexFillEnd; index++)
                            mmuWalkSetEntryState(pLevelInst, index, pTarget->entryState);

                        pendingFillCount = 0;
                    }

                    // Recheck the state after fill. If nothing to do, continue..
                    if (pTarget->entryState == mmuWalkGetEntryState(pLevelInst, entryIndex))
                    {
                        goto check_last_entry;
                    }

                } // End of fill optimizations.

                // Determine the sublevel we need to operate on.
                status = pOpParams->selectSubLevel(pOpParams->pOpCtx,
                                                   pLevel,
                                                   &subLevel,
                                                   clippedVaLo,
                                                   clippedVaHi);
                NV_ASSERT_OR_GOTO(NV_OK == status, cleanupIter);

                //
                // Allocate the sublevel instances for the current PDE and update the current
                // Page Dir (i.e. write the PDE into the Page Dir) if needed.
                //
                status = _mmuWalkPdeAcquire(pWalk,
                                            pOpParams,
                                            pLevel,
                                            pLevelInst,
                                            entryIndex,
                                            subLevel,
                                            clippedVaLo,
                                            clippedVaHi,
                                            pSubLevelInsts);
                NV_ASSERT_OR_GOTO(NV_OK == status, cleanupIter);

                // Release op is done if the target sub-level is absent.
                if (pOpParams->bRelease && (NULL == pSubLevelInsts[subLevel]))
                {
                    goto check_last_entry;
                }

                //
                // Split sparse PDE's range.
                // When only a subrange of the original PDE's VA range is being operated
                // on we sparsify the remaining range lying outside the operational
                // subrange (clippedVaLo to clippedVaHi)
                //
                if (MMU_ENTRY_STATE_SPARSE == currEntryState)
                {
                    //
                    // Sparsify the lower part of the VA range that outside the operational
                    // subrange.
                    //
                    if (clippedVaLo > entryVaLo)
                    {
                        status = mmuWalkProcessPdes(pWalk,
                                                    &g_opParamsSparsify,
                                                    pLevel->subLevels + subLevel,
                                                    pSubLevelInsts[subLevel],
                                                    entryVaLo,
                                                    clippedVaLo - 1);
                        NV_ASSERT_OR_GOTO(NV_OK == status, cleanupIter);
                    }

                    //
                    // Sparsify the upper part of the VA range that is outside the operational
                    // subrange.
                    //
                    if (clippedVaHi < entryVaHi)
                    {
                        status = mmuWalkProcessPdes(pWalk,
                                                    &g_opParamsSparsify,
                                                    pLevel->subLevels + subLevel,
                                                    pSubLevelInsts[subLevel],
                                                    clippedVaHi + 1,
                                                    entryVaHi);
                        NV_ASSERT_OR_GOTO(NV_OK == status, cleanupIter);
                    }
                } // Sparse PDE split

                // Resolve potential conflicts in multiple sized page tables
                if (pLevel->pFmt->numSubLevels != 1 &&
                    !pOpParams->bIgnoreSubLevelConflicts)
                {
                    status = _mmuWalkResolveSubLevelConflicts(pWalk,
                                                              pOpParams,
                                                              pLevel,
                                                              pSubLevelInsts,
                                                              subLevel,
                                                              clippedVaLo,
                                                              clippedVaHi);
                    NV_ASSERT_OR_GOTO(NV_OK == status, cleanupIter);
                }

                status = pOpParams->opFunc(pWalk,
                                           pOpParams,
                                           pLevel->subLevels + subLevel,
                                           pSubLevelInsts[subLevel],
                                           clippedVaLo,
                                           clippedVaHi);

                if (NV_ERR_MORE_PROCESSING_REQUIRED == status)
                {
                    //
                    // If NV_ERR_MORE_PROCESSING_REQUIRED is returned above,
                    // the recursive MMU Walker would have recursed down one
                    // more level. In this code block, we keep the iteration
                    // going by doing everything the recursion previously did.
                    //
                    status = NV_OK;

                    // Save off the current state of iteration for this level
                    pLevel->iterInfo.pLevelInst   = pLevelInst;
                    pLevel->iterInfo.vaLo         = vaLo;
                    pLevel->iterInfo.vaHi         = vaHi;
                    pLevel->iterInfo.vaLevelBase  = vaLevelBase;
                    pLevel->iterInfo.entryIndexHi = entryIndexHi;
                    pLevel->iterInfo.entryIndex   = entryIndex;
                    pLevel->iterInfo.entryIndexFillStart = entryIndexFillStart;
                    pLevel->iterInfo.entryIndexFillEnd   = entryIndexFillEnd;
                    pLevel->iterInfo.pendingFillCount    = pendingFillCount;
                    pLevel->iterInfo.entryVaLo    = entryVaLo;

                    //
                    // Here use variables that would be used in the next recursion downwards.
                    // Calculate new vaLevelBase, entryIndexLo, entryIndexHi, entryIndex
                    //
                    pLevel       = pLevel->subLevels + subLevel;

                    vaLevelBase  = mmuFmtLevelVirtAddrLo(pLevel->pFmt, clippedVaLo);
                    entryIndexLo = mmuFmtVirtAddrToEntryIndex(pLevel->pFmt, clippedVaLo);
                    entryIndexHi = mmuFmtVirtAddrToEntryIndex(pLevel->pFmt, clippedVaHi);

                    // Now replace the current stack frame with the frame that is one level down
                    // pLevel, vaLevelBase, entryIndexHi replaced above
                    pLevelInst   = pSubLevelInsts[subLevel];
                    vaLo         = clippedVaLo;
                    vaHi         = clippedVaHi;
                    entryIndex   = entryIndexLo;
                    entryIndexFillStart = 0;
                    entryIndexFillEnd   = 0;
                    pendingFillCount    = 0;
                }
                else
                {
                    NV_ASSERT_OR_GOTO(NV_OK == status, cleanupIter);
                    //
                    // If NV_OK is returned above, the recursive MMU Walker would have reached
                    // the target format level and so reached the base case of its recursion.
                    // It would then return from recursive function calls and call pdeRelease
                    // for all levels whose sublevels are done being processed.
                    //

cleanupIter:
                    // PdeRelease itself immediately since this level does not recurse.
#if defined(__GNUC__) && !defined(__clang__)
                    // gcc is falsely reporting entryVaLo; entryVaLo is definitely initialized
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
                    _mmuWalkPdeRelease(pWalk,
                                       pOpParams,
                                       pLevel,
                                       pLevelInst,
                                       entryIndex,
                                       entryVaLo);

check_last_entry:
                    //
                    // If the recursive MMU Walker did a continue on the current level,
                    // then it didn't do a pdeRelease of the current level.
                    // Even with the continue, for the current iteration,
                    // if entryIndex == entryIndexHi, then we're done with this level
                    // and need to do a pdeRelease on the next level up since we would
                    // return from the recursion.
                    //

                    //
                    // If we're at the original level and entryIndex = entryIndexHi,
                    // then we're done and need to exit the entire loop.
                    // If this is true, we've already done the _mmuWalkPdeRelease:
                    // Either we already called _mmuWalkPdeRelease right before this
                    // or we skipped it from a goto check_last_entry continue.
                    // The MMU Walker is re-entrant and will otherwise pick up on
                    // parent levels when mmuWalkProcessPdes is called on sublevels
                    //
                    if ((pLevel == pLevelOrig) && (entryIndex == entryIndexHi))
                    {
                        goto done;
                    }

                    // 
                    // Now restore and finish previous frame(s)
                    //
                    // If this is the last processed sublevel of a level or an error has
                    // previously occurred, pdeRelease the level.
                    // Continue doing so for all parent levels.
                    // Once we're reached a non-finished level, iterate to the next entry.
                    //
                    while (entryIndex == entryIndexHi || status != NV_OK)
                    {
                        //
                        // Now replace the current stack frame with the frame that was one
                        // level above. This should never be NULL, since we'll already have
                        // exited after processing the root level. If it is NULL, we can't
                        // clean up any more anyway, so return immediately.
                        //
                        NV_ASSERT_OR_RETURN(pLevel->pParent != NULL, NV_ERR_INVALID_STATE);

                        pLevel       = pLevel->pParent;
                        pLevelInst   = pLevel->iterInfo.pLevelInst;
                        vaLo         = pLevel->iterInfo.vaLo;
                        vaHi         = pLevel->iterInfo.vaHi;
                        vaLevelBase  = pLevel->iterInfo.vaLevelBase;
                        entryIndexHi = pLevel->iterInfo.entryIndexHi;
                        entryIndex   = pLevel->iterInfo.entryIndex;
                        entryIndexFillStart = pLevel->iterInfo.entryIndexFillStart;
                        entryIndexFillEnd   = pLevel->iterInfo.entryIndexFillEnd;
                        pendingFillCount    = pLevel->iterInfo.pendingFillCount;

                        _mmuWalkPdeRelease(pWalk,
                                           pOpParams,
                                           pLevel,
                                           pLevelInst,
                                           entryIndex,
                                           pLevel->iterInfo.entryVaLo);

                        //
                        // If we're at the original level and entryIndex = entryIndexHi,
                        // then we're done and need to exit the entire loop
                        //
                        if ((pLevel == pLevelOrig) && (entryIndex == entryIndexHi))
                        {
                            goto done;
                        }
                    }

                    //
                    // Once the above loop is done and we reach here, then we're
                    // ready to process the next entry in the list. Only iterate here,
                    // not in the overall loop since we may have iterated down in the
                    // above else block and don't want to increment before processing
                    // the first entry on a new lower level.
                    //
                    entryIndex++;
                }
            } // per entry loop

            //
            // If this assertion fails, it is a result of a programming
            // error in the iterative MMU Walker implementation. We should
            // have iterated back updwards through the MMU state to the original
            // level even on failure.
            //
            NV_ASSERT_OR_RETURN(pLevel != pLevelOrig, NV_ERR_INVALID_STATE);
        }
done:
        return status;
    }
    else
    {
        // Recursive MMU Walker Implementation
        NV_STATUS    status       = NV_OK;
        NvU64        vaLevelBase  = mmuFmtLevelVirtAddrLo(pLevel->pFmt, vaLo);
        NvU32        entryIndexLo = mmuFmtVirtAddrToEntryIndex(pLevel->pFmt, vaLo);
        NvU32        entryIndexHi = mmuFmtVirtAddrToEntryIndex(pLevel->pFmt, vaHi);
        NvU32        entryIndex;
        NvU32        index;
        NvU32        entryIndexFillStart = 0;
        NvU32        entryIndexFillEnd;
        NvU32        pendingFillCount = 0;

        NV_ASSERT_OR_RETURN(NULL != pOpParams, NV_ERR_INVALID_ARGUMENT);

        // Walk over each relevant entry (PDE) within this Page Level
        for (entryIndex = entryIndexLo; entryIndex <= entryIndexHi; entryIndex++)
        {
            const NvU64           entryVaLo   = mmuFmtEntryIndexVirtAddrLo(pLevel->pFmt,
                                                                     vaLevelBase, entryIndex);
            const NvU64           entryVaHi   = mmuFmtEntryIndexVirtAddrHi(pLevel->pFmt,
                                                                     vaLevelBase, entryIndex);
            const NvU64           clippedVaLo = NV_MAX(vaLo, entryVaLo);
            const NvU64           clippedVaHi = NV_MIN(vaHi, entryVaHi);
            const MMU_ENTRY_STATE currEntryState = mmuWalkGetEntryState(pLevelInst, entryIndex);
            NvU32                 subLevel       = 0;
            MMU_WALK_LEVEL_INST  *pSubLevelInsts[MMU_FMT_MAX_SUB_LEVELS] = {0};

            // Optimizations for release operations.
            if (pOpParams->bRelease)
            {
                // Skip this entry if it is neither a PDE nor marked as a hybrid entry.
                if ((MMU_ENTRY_STATE_IS_PDE != currEntryState) &&
                    !pLevelInst->pStateTracker[entryIndex].bHybrid)
                    continue;
            }

            // Optimizations for fill operations.
            if (pOpParams->bFill)
            {
                const MMU_FILL_TARGET *pTarget = (const MMU_FILL_TARGET *) pOpParams->pOpCtx;

                if (pendingFillCount == 0)
                    entryIndexFillStart = entryIndexFillEnd = entryIndex;

                //
                // Check if the entire entry's coverage is being filled to
                // a constant state.
                //
                // If this entry is not currently a PDE we can
                // apply the fill operation directly
                // at this level and avoid "splitting" the PDE.
                //
                // If this entry is currently a PDE we must
                // clear the entries of the lower levels to free
                // unused level instances.
                //
                if ((pTarget->entryState != currEntryState) &&
                    (MMU_ENTRY_STATE_IS_PDE != currEntryState) &&
                    (entryVaLo == clippedVaLo) &&
                    (entryVaHi == clippedVaHi))
                {
                    entryIndexFillEnd = entryIndex;
                    pendingFillCount++;

                    // Not the last iteration, keep batching..
                    if (entryIndex < entryIndexHi)
                        continue;
                }

                if (pendingFillCount != 0)
                {
                    NvU32 progress = 0;

                    // Flush pending fills
                    pWalk->pCb->FillEntries(pWalk->pUserCtx,
                                            pLevel->pFmt,
                                            pLevelInst->pMemDesc,
                                            entryIndexFillStart,
                                            entryIndexFillEnd,
                                            pTarget->fillState,
                                            &progress);

                    NV_ASSERT_OR_RETURN(
                        progress == (entryIndexFillEnd - entryIndexFillStart + 1),
                        NV_ERR_INVALID_STATE);

                    for (index = entryIndexFillStart; index <= entryIndexFillEnd; index++)
                        mmuWalkSetEntryState(pLevelInst, index, pTarget->entryState);

                    pendingFillCount = 0;
                }

                // Recheck the state after fill. If nothing to do, continue..
                if (pTarget->entryState == mmuWalkGetEntryState(pLevelInst, entryIndex))
                    continue;

            } // End of fill optimizations.

            // Determine the sublevel we need to operate on.
            status = pOpParams->selectSubLevel(pOpParams->pOpCtx,
                                               pLevel,
                                               &subLevel,
                                               clippedVaLo,
                                               clippedVaHi);
            NV_ASSERT_OR_GOTO(NV_OK == status, cleanup);

            //
            // Allocate the sublevel instances for the current PDE and update the current
            // Page Dir (i.e. write the PDE into the Page Dir) if needed.
            //
            status = _mmuWalkPdeAcquire(pWalk,
                                        pOpParams,
                                        pLevel,
                                        pLevelInst,
                                        entryIndex,  //PDE index being processed
                                        subLevel,    //Sub level processed within the PDE
                                        clippedVaLo, //Low VA for the PDE
                                        clippedVaHi, //High VA for the PDE
                                        pSubLevelInsts);
            NV_ASSERT_OR_GOTO(NV_OK == status, cleanup);

            // Release op is done if the target sub-level is absent.
            if (pOpParams->bRelease && (NULL == pSubLevelInsts[subLevel]))
            {
                continue;
            }

            //
            // Split sparse PDE's range.
            // When only a subrange of the original PDE's VA range is being operated
            // on we sparsify the remaining range lying outside the operational
            // subrange (clippedVaLo to clippedVaHi)
            //
            if (MMU_ENTRY_STATE_SPARSE == currEntryState)
            {
                //
                // Sparsify the lower part of the VA range that outside the operational
                // subrange.
                //
                if (clippedVaLo > entryVaLo)
                {
                    status = g_opParamsSparsify.opFunc(pWalk,
                                                       &g_opParamsSparsify,
                                                       pLevel->subLevels + subLevel,
                                                       pSubLevelInsts[subLevel],
                                                       entryVaLo,
                                                       clippedVaLo - 1);
                    NV_ASSERT_OR_GOTO(NV_OK == status, cleanup);
                }

                //
                // Sparsify the upper part of the VA range that is outside the operational
                // subrange.
                //
                if (clippedVaHi < entryVaHi)
                {
                    status = g_opParamsSparsify.opFunc(pWalk,
                                                       &g_opParamsSparsify,
                                                       pLevel->subLevels + subLevel,
                                                       pSubLevelInsts[subLevel],
                                                       clippedVaHi + 1,
                                                       entryVaHi);
                    NV_ASSERT_OR_GOTO(NV_OK == status, cleanup);
                }
            } // Sparse PDE split

            // Resolve potential conflicts in multiple sized page tables
            if (pLevel->pFmt->numSubLevels != 1 &&
                !pOpParams->bIgnoreSubLevelConflicts)
            {
                status = _mmuWalkResolveSubLevelConflicts(pWalk,
                                                          pOpParams,
                                                          pLevel,
                                                          pSubLevelInsts,
                                                          subLevel,
                                                          clippedVaLo,
                                                          clippedVaHi);
                NV_ASSERT_OR_GOTO(NV_OK == status, cleanup);
            }

            // Recurse to update the next level for this PDE
            status = pOpParams->opFunc(pWalk,
                                       pOpParams,
                                       pLevel->subLevels + subLevel,
                                       pSubLevelInsts[subLevel],
                                       clippedVaLo,
                                       clippedVaHi);
            NV_ASSERT_OR_GOTO(NV_OK == status, cleanup);

cleanup:
            // Free unused sublevel instances. Clear the PDE if all sublevels are deallocated.
            _mmuWalkPdeRelease(pWalk,
                               pOpParams,
                               pLevel,
                               pLevelInst,
                               entryIndex,
                               entryVaLo);

            // Stop processing PDEs if we are in error state.
            if (NV_OK != status)
                break;
        } // per entry loop
        return status;
    }


}

/*!
 * @brief This function allocates the root Page Directory and commits it the
 * related channels.
 *
 * @param[in]  vaLo          The lower end of the Virtual Address range that is
 *                           being processed.
 * @param[in]  vaHi          The upper end of the Virtual Address range that is
 *                           being processed
 *
 * @param[in]  bCommit       Force commit the PDB
 *
 * @return NV_OK of allocating this level succeeds.
 *         Other errors, if not.
 */
NV_STATUS
mmuWalkRootAcquire
(
    MMU_WALK *pWalk,
    NvU64     vaLo,
    NvU64     vaHi,
    NvBool    bCommit
)
{
    MMU_WALK_LEVEL_INST *pLevelInst = NULL;
    NvBool               bChanged   = NV_FALSE;

    // Acquire root level instance memory.
    NV_ASSERT_OK_OR_RETURN(
        _mmuWalkLevelInstAcquire(pWalk, &pWalk->root, vaLo, vaHi,
                                 NV_TRUE, NV_FALSE, bCommit, &bChanged,
                                 &pLevelInst, NV_FALSE /*bInitNv4k*/));

    // We check pLevelInst to catch the corner case, where Commit() is called before PDB allocation.
    if (bChanged || (bCommit && pLevelInst))
    {
        NvBool bDone;

        // Bind this Page Dir to the affected channels
        bDone = pWalk->pCb->UpdatePdb(pWalk->pUserCtx, pWalk->root.pFmt,
                                      pLevelInst->pMemDesc, NV_FALSE);
        NV_ASSERT_OR_RETURN(bDone, NV_ERR_INVALID_STATE);
    }

    return NV_OK;
}

/*!
 * @brief This function releases the root Page Directory
 */
void
mmuWalkRootRelease
(
    MMU_WALK *pWalk
)
{
    MMU_WALK_LEVEL_INST *pLevelInst = pWalk->root.pInstances;
    if (NULL != pLevelInst)
    {
        // Free the level instance if the entry ref count is 0.
        if ((0 == pLevelInst->numValid + pLevelInst->numSparse) &&
            (0 == pLevelInst->numReserved))
        {
            NvBool bDone;

            // Commit NULL root page directory (clear usage).
            bDone = pWalk->pCb->UpdatePdb(pWalk->pUserCtx, pWalk->root.pFmt, NULL, NV_FALSE);
            NV_ASSERT(bDone);

            // Free unused root memory.
            _mmuWalkLevelInstRelease(pWalk, &pWalk->root, pLevelInst);
        }
    }
}

/*!
 * @brief This function updates the @ref MMU_WALK_LEVEL_INST::pStateTracker for an
 *        entry specified by the entryIndex.
 *
 * @param[in]     entryIndex   Index of the entry whose state needs to be updated.
 * @param[in]     newEntryState  The new state of the entry specified by entryIndex
 */
void
mmuWalkSetEntryState
(
    MMU_WALK_LEVEL_INST *pLevelInst,
    NvU32                entryIndex,
    MMU_ENTRY_STATE      newEntryState
)
{
    MMU_ENTRY_STATE currEntryState = mmuWalkGetEntryState(pLevelInst, entryIndex);

    // Decrement ref count for current state
    switch (currEntryState)
    {
        case MMU_ENTRY_STATE_IS_PTE:
        case MMU_ENTRY_STATE_IS_PDE:
            NV_ASSERT(0 != pLevelInst->numValid);
            pLevelInst->numValid--;
            break;
        case MMU_ENTRY_STATE_SPARSE:
            NV_ASSERT(0 != pLevelInst->numSparse);
            pLevelInst->numSparse--;
            break;
        case MMU_ENTRY_STATE_NV4K:
            NV_ASSERT(0 != pLevelInst->numNv4k);
            pLevelInst->numNv4k--;
            break;
        case MMU_ENTRY_STATE_INVALID:
            break;
        default:
            NV_ASSERT(0);
    }

    // Increment new state ref count
    switch (newEntryState)
    {
        case MMU_ENTRY_STATE_IS_PTE:
        case MMU_ENTRY_STATE_IS_PDE:
            pLevelInst->numValid++;
            break;
        case MMU_ENTRY_STATE_SPARSE:
            pLevelInst->numSparse++;
            break;
        case MMU_ENTRY_STATE_NV4K:
            pLevelInst->numNv4k++;
            break;
        case MMU_ENTRY_STATE_INVALID:
            break;
        default:
            NV_ASSERT(0);
    }

    // Commit new state.
    pLevelInst->pStateTracker[entryIndex].state = newEntryState;
}

void
mmuWalkSetEntryReserved
(
    MMU_WALK_LEVEL_INST *pLevelInst,
    NvU32                entryIndex,
    NvBool               bReserved
)
{
    if (pLevelInst->pStateTracker[entryIndex].bReserved)
    {
        NV_ASSERT(0 != pLevelInst->numReserved);
        pLevelInst->numReserved--;
    }
    if (bReserved)
    {
        pLevelInst->numReserved++;
    }
    pLevelInst->pStateTracker[entryIndex].bReserved = bReserved;
}

void
mmuWalkSetEntryHybrid
(
    MMU_WALK_LEVEL_INST *pLevelInst,
    NvU32                entryIndex,
    NvBool               bHybrid
)
{
    if (pLevelInst->pStateTracker[entryIndex].bHybrid)
    {
        NV_ASSERT(0 != pLevelInst->numHybrid);
        pLevelInst->numHybrid--;
    }
    if (bHybrid)
    {
        pLevelInst->numHybrid++;
    }
    pLevelInst->pStateTracker[entryIndex].bHybrid = bHybrid;
}

/**
 * @brief      Calculate target entry indices that covers VA range for
 *             source entries
 *
 * @details    For example, entry 1 in 64K PT is aligned to 4K PT entry 0 to
 *             15. 4K PTE 1 to 18 will be covered by 64K PTE 0 to 1.
 *
 *             It is introduced by NV4K encoding. Updating big page table
 *             according to small page table requires index transfering
 *
 * @param[in]  pPageFmtIn   Source format
 * @param[in]  indexLoIn    The index lower in
 * @param[in]  indexHiIn    The index higher in
 * @param[in]  pPageFmtOut  Target format
 * @param[out] pIndexLoOut  The lower result index
 * @param[out] pIndexHiOut  The higher result index
 */
void
mmuFmtCalcAlignedEntryIndices
(
    const MMU_FMT_LEVEL *pPageFmtIn,
    const NvU32 indexLoIn,
    const NvU32 indexHiIn,
    const MMU_FMT_LEVEL *pPageFmtOut,
    NvU32 *pIndexLoOut,
    NvU32 *pIndexHiOut
)
{
    NvU64 pageSizeIn, pageSizeOut;
    NvU64 pageSizeRatio;
    NV_ASSERT(pIndexLoOut != NULL && pIndexHiOut != NULL);
    NV_ASSERT(pPageFmtIn != NULL && pPageFmtOut != NULL);

    pageSizeIn = mmuFmtLevelPageSize(pPageFmtIn);
    pageSizeOut = mmuFmtLevelPageSize(pPageFmtOut);

    if (pageSizeIn < pageSizeOut)
    {
        pageSizeRatio = pageSizeOut / pageSizeIn;
        NV_ASSERT(NvU64_HI32(pageSizeRatio) == 0);
        *pIndexLoOut = (NvU32)(indexLoIn / pageSizeRatio);
        *pIndexHiOut = (NvU32)(indexHiIn / pageSizeRatio);
    }
    else
    {
        pageSizeRatio = pageSizeIn / pageSizeOut;
        NV_ASSERT(NvU64_HI32(pageSizeRatio) == 0);
        *pIndexLoOut = (NvU32)(indexLoIn * pageSizeRatio);
        *pIndexHiOut = (NvU32)((indexHiIn + 1) * pageSizeRatio - 1);
    }
}

/*----------------------------Static Functions--------------------------------*/

static NV_STATUS
_mmuWalkLevelInit
(
    const MMU_WALK       *pWalk,
    MMU_WALK_LEVEL       *pParent,
    const MMU_FMT_LEVEL  *pLevelFmt,
    MMU_WALK_LEVEL       *pLevel
)
{
    // Init pointers.
    pLevel->pFmt    = pLevelFmt;
    pLevel->pParent = pParent;

    if (0 != pLevelFmt->numSubLevels)
    {
        NvU32       subLevel;
        const NvU32 size = pLevelFmt->numSubLevels * (NvU32)sizeof(*pLevel->subLevels);

        // Allocate sub-level array.
        pLevel->subLevels = portMemAllocNonPaged(size);
        if (pLevel->subLevels == NULL)
            return NV_ERR_NO_MEMORY;

        portMemSet(pLevel->subLevels, 0, size);

        // Recursively create each sub-level.
        for (subLevel = 0; subLevel < pLevelFmt->numSubLevels; ++subLevel)
        {
            NV_ASSERT_OK_OR_RETURN(
                _mmuWalkLevelInit(pWalk, pLevel, pLevelFmt->subLevels + subLevel,
                                  pLevel->subLevels + subLevel));
        }
    }

    return NV_OK;
}

static void
_mmuWalkLevelDestroy
(
    const MMU_WALK *pWalk,
    MMU_WALK_LEVEL *pLevel
)
{
    NvU32 subLevel;

    if (NULL != pLevel->subLevels)
    {
        // Recursively destroy each sub-level.
        for (subLevel = 0; subLevel < pLevel->pFmt->numSubLevels; ++subLevel)
        {
            _mmuWalkLevelDestroy(pWalk, pLevel->subLevels + subLevel);
        }
        // Free sub-level array.
        portMemFree(pLevel->subLevels);
    }

    // All level instance memory should be freed already.
    NV_ASSERT(NULL == pLevel->pInstances);
}

/**
 * @brief      Resolve upcoming state conflicts before mmu walk operations
 *
 * @example    Say we are to mmuWalkMap VA range [vaLo, vaHi] on small PT.
 * Assume we have 4K PT and 64K PT as our small PT and big PT, and [vaLo, vaHi]
 * is a strict subset of VA range covered by BigPTE[1, 3] and SmallPTE[18, 61].
 * Let's say BigPTE[1, 3] are sparse right now.
 *
 * To resolve the conflict, we need to preserve sparse state for part of the
 * VA range that is not going to be mapped. We need to move those states from
 * BigPT to SmallPT.
 *
 * Before:
 *  BigPTE[1, 3]: sparse,   SmallPTE[16 - 63]: invalid
 *  (BigPTE[1, 3] and SmallPTE[16 - 63] are VA aligned)
 * After:
 *  BigPTE[1, 3]: invalid,  SmallPTE[16 - 17]: sparse
 *                          SmallPTE[18 - 61]: invalid, will later be mapped
 *                          SmallPTE[62 - 63]: sparse
 *
 * @example    If we are to mmuWalkMap on big PT instead of samll PT,
 * and sparse state was on small PT, we just need to invalidate the small PTEs.
 *
 * Before:
 *  BigPTE[1, 3]:       invalid,
 *  SmallPTE[16 - 63]:  sparse
 * After:
 *  BigPTE[1, 3]:       invalid, will later be mapped
 *  SmallPTE[16 - 63]:  invalid
 *
 * @return     NV_OK on success, no other values for now
 */
static NV_STATUS NV_NOINLINE
_mmuWalkResolveSubLevelConflicts
(
    const MMU_WALK              *pWalk,
    const MMU_WALK_OP_PARAMS    *pOpParams,
    MMU_WALK_LEVEL              *pLevel,
    MMU_WALK_LEVEL_INST         *pSubLevelInsts[],
    NvU32                        subLevelIdx,
    NvU64                        clippedVaLo,
    NvU64                        clippedVaHi
)
{
    NvU32     i                             = 0;
    NvU32     progress                      = 0;
    NV_STATUS status                        = NV_OK;
    NvBool    bConflictLo                   = NV_FALSE;
    NvBool    bConflictHi                   = NV_FALSE;
    const MMU_FMT_LEVEL *pLevelFmtBig       = pLevel->subLevels[0].pFmt;
    const MMU_FMT_LEVEL *pLevelFmtSmall     = pLevel->subLevels[1].pFmt;
    MMU_WALK_LEVEL_INST *pLevelBigInst      = pSubLevelInsts[0];
    MMU_WALK_LEVEL_INST *pLevelSmallInst    = pSubLevelInsts[1];
    // Entry indicies for target page table
    NvU32 entryIndexLo, entryIndexHi;
    // Entry indicies involved in both page tables
    NvU32 indexLo_Small, indexHi_Small, indexLo_Big, indexHi_Big;

    if (0 == subLevelIdx)
    {
        entryIndexLo = mmuFmtVirtAddrToEntryIndex(pLevelFmtBig, clippedVaLo);
        entryIndexHi = mmuFmtVirtAddrToEntryIndex(pLevelFmtBig, clippedVaHi);
        indexLo_Big = entryIndexLo;
        indexHi_Big = entryIndexHi;
        mmuFmtCalcAlignedEntryIndices(pLevelFmtBig, indexLo_Big, indexHi_Big,
            pLevelFmtSmall, &indexLo_Small, &indexHi_Small);
    }
    else
    {
        entryIndexLo = mmuFmtVirtAddrToEntryIndex(pLevelFmtSmall, clippedVaLo);
        entryIndexHi = mmuFmtVirtAddrToEntryIndex(pLevelFmtSmall, clippedVaHi);
        mmuFmtCalcAlignedEntryIndices(pLevelFmtSmall, entryIndexLo,
            entryIndexHi, pLevelFmtBig, &indexLo_Big, &indexHi_Big);
        mmuFmtCalcAlignedEntryIndices(pLevelFmtBig, indexLo_Big, indexHi_Big,
            pLevelFmtSmall, &indexLo_Small, &indexHi_Small);
    }

    // check if involved Small PTEs need to be sparsified
    if (1 == subLevelIdx && NULL != pLevelSmallInst && NULL != pLevelBigInst)
    {
        // check lower part
        MMU_ENTRY_STATE entryStateBig;
        entryStateBig = mmuWalkGetEntryState(pLevelBigInst, indexLo_Big);
        bConflictLo = (MMU_ENTRY_STATE_SPARSE == entryStateBig);

        // check higher part
        entryStateBig = mmuWalkGetEntryState(pLevelBigInst, indexHi_Big);
        bConflictHi = (MMU_ENTRY_STATE_SPARSE == entryStateBig);
    }

    if (bConflictLo && entryIndexLo > indexLo_Small)
    {
        // sparsify lower range of entries
        pWalk->pCb->FillEntries(pWalk->pUserCtx, pLevelFmtSmall,
            pLevelSmallInst->pMemDesc, indexLo_Small, entryIndexLo - 1,
            MMU_WALK_FILL_SPARSE, &progress);
        NV_ASSERT_OR_RETURN(progress == entryIndexLo - indexLo_Small,
            NV_ERR_INVALID_STATE);

        for (i = indexLo_Small; i <= entryIndexLo - 1; i++)
        {
            mmuWalkSetEntryState(pLevelSmallInst, i, MMU_ENTRY_STATE_SPARSE);
        }
    }

    if (bConflictHi && entryIndexHi < indexHi_Small)
    {
        // sparsify higher range of entries
        pWalk->pCb->FillEntries(pWalk->pUserCtx, pLevelFmtSmall,
            pLevelSmallInst->pMemDesc, entryIndexHi + 1, indexHi_Small,
            MMU_WALK_FILL_SPARSE, &progress);
        NV_ASSERT_OR_RETURN(progress == indexHi_Small - entryIndexHi,
            NV_ERR_INVALID_STATE);

        for (i = entryIndexHi + 1; i <= indexHi_Small; i++)
        {
            mmuWalkSetEntryState(pLevelSmallInst, i, MMU_ENTRY_STATE_SPARSE);
        }
    }

    // invalidate the VA range in the other page table
    if (NULL != pLevelSmallInst && NULL != pLevelBigInst)
    {
        NvU32 indexLo_tmp, indexHi_tmp;
        const MMU_FMT_LEVEL *pSubLevelFmt;
        MMU_WALK_LEVEL_INST *pSubLevelInst;

        if (subLevelIdx == 0)
        {
            indexLo_tmp = indexLo_Small;
            indexHi_tmp = indexHi_Small;
            pSubLevelFmt = pLevelFmtSmall;
            pSubLevelInst = pLevelSmallInst;
        }
        else
        {
            indexLo_tmp = indexLo_Big;
            indexHi_tmp = indexHi_Big;
            pSubLevelFmt = pLevelFmtBig;
            pSubLevelInst = pLevelBigInst;
        }

        pWalk->pCb->FillEntries(pWalk->pUserCtx, pSubLevelFmt,
            pSubLevelInst->pMemDesc, indexLo_tmp, indexHi_tmp,
            MMU_WALK_FILL_INVALID, &progress);
        NV_ASSERT_OR_RETURN(progress == indexHi_tmp - indexLo_tmp + 1,
            NV_ERR_INVALID_STATE);

        for (i = indexLo_tmp; i <= indexHi_tmp; i++)
        {
            mmuWalkSetEntryState(pSubLevelInst, i, MMU_ENTRY_STATE_INVALID);
        }
    }

    return status;
}

/*!
 * Lazily allocates and initializes a level instance.
 */
static NV_STATUS
_mmuWalkLevelInstAcquire
(
    const MMU_WALK       *pWalk,
    MMU_WALK_LEVEL       *pLevel,
    const NvU64           vaLo,
    const NvU64           vaHi,
    const NvBool          bTarget,
    const NvBool          bRelease,
    const NvBool          bCommit,
    NvBool               *pBChanged,
    MMU_WALK_LEVEL_INST **ppLevelInst,
    const NvBool          bInitNv4k
)
{
    NV_STATUS            status;
    MMU_WALK_MEMDESC    *pOldMem;
    NvU32                oldSize;
    MMU_WALK_LEVEL_INST *pLevelInst = NULL;
    NvBool               bNew       = NV_FALSE;

    // Lookup level instance.
    if (NV_OK != btreeSearch(vaLo, (NODE**)&pLevelInst, (NODE*)pLevel->pInstances))
    {
        NvU32 numBytes;

        if (!bTarget || bRelease)
        {
            // Skip missing non-target instances.
            *ppLevelInst = NULL;
            return NV_OK;
        }

        // We only call Commit() on already allocated page directory.
        NV_ASSERT_OR_RETURN(!bCommit, NV_ERR_INVALID_STATE);

        // Mark as newly allocated.
        bNew = NV_TRUE;

        // Allocate missing target instances.
        pLevelInst = portMemAllocNonPaged(sizeof(*pLevelInst));
        status = (pLevelInst == NULL) ? NV_ERR_NO_MEMORY : NV_OK;
        NV_ASSERT_OR_GOTO(NV_OK == status, done);
        portMemSet(pLevelInst, 0, sizeof(*pLevelInst));

        // Insert the new node into the tree of instances for this page level.
        pLevelInst->node.keyStart = mmuFmtLevelVirtAddrLo(pLevel->pFmt, vaLo);
        pLevelInst->node.keyEnd   = mmuFmtLevelVirtAddrHi(pLevel->pFmt, vaHi);

        status = btreeInsert(&pLevelInst->node, (NODE**)&pLevel->pInstances);
        NV_ASSERT_OR_GOTO(NV_OK == status, done);

        // Allocate entry tracker.
        numBytes = mmuFmtLevelEntryCount(pLevel->pFmt) * sizeof(MMU_ENTRY_INFO);
        pLevelInst->pStateTracker = portMemAllocNonPaged(numBytes);
        status = (pLevelInst->pStateTracker == NULL) ? NV_ERR_NO_MEMORY : NV_OK;
        NV_ASSERT_OR_GOTO(NV_OK == status, done);
        portMemSet(pLevelInst->pStateTracker, 0, numBytes);
        if (bInitNv4k)
        {
            NvU32 i;
            for (i = 0; i < mmuFmtLevelEntryCount(pLevel->pFmt); ++i)
            {
                 mmuWalkSetEntryState(pLevelInst, i, MMU_ENTRY_STATE_NV4K);
            }
        }
    }

    // Save original memory info.
    pOldMem = pLevelInst->pMemDesc;
    oldSize = pLevelInst->memSize;

    // Allocate (possibly reallocating) memory for this level instance.
    status = pWalk->pCb->LevelAlloc(pWalk->pUserCtx,
                                    pLevel->pFmt,
                                    mmuFmtLevelVirtAddrLo(pLevel->pFmt, vaLo),
                                    vaHi,
                                    bTarget,
                                    &pLevelInst->pMemDesc,
                                    &pLevelInst->memSize,
                                    pBChanged);
    NV_ASSERT_OR_GOTO(NV_OK == status, done);

    if (*pBChanged)
    {
        const NvU32 entryIndexLo = oldSize / pLevel->pFmt->entrySize;
        const NvU32 entryIndexHi = (pLevelInst->memSize / pLevel->pFmt->entrySize) - 1;
        NvU32       progress     = 0;

        //
        // default state for new entries
        // NV4K for big page table if ATS is enabled
        //
        MMU_WALK_FILL_STATE newEntryState = bInitNv4k ? MMU_WALK_FILL_NV4K :
                                                        MMU_WALK_FILL_INVALID;

        NV_ASSERT(NULL != pLevelInst->pMemDesc);
        NV_ASSERT(entryIndexLo <= entryIndexHi);

        // We only call Commit() on already allocated page directory.
        if (bCommit)
        {
            status = NV_ERR_INVALID_STATE;
            NV_ASSERT_OR_GOTO(NV_OK == status, done);
        }

        // Copy old entries from old to new.
        if (entryIndexLo > 0)
        {
            NV_ASSERT(NULL != pWalk->pCb->CopyEntries);
            pWalk->pCb->CopyEntries(pWalk->pUserCtx,
                                    pLevel->pFmt,
                                    pOldMem,
                                    pLevelInst->pMemDesc,
                                    0,
                                    entryIndexLo - 1,
                                    &progress);
            NV_ASSERT(progress == entryIndexLo);

            // Free old memory.
            pWalk->pCb->LevelFree(pWalk->pUserCtx, pLevel->pFmt,
                                  pLevelInst->node.keyStart, pOldMem);
        }

        if(pWalk->bInvalidateOnReserve)
        {
            // Clear new entries to invalid.
            pWalk->pCb->FillEntries(pWalk->pUserCtx,
                                    pLevel->pFmt,
                                    pLevelInst->pMemDesc,
                                    entryIndexLo,
                                    entryIndexHi,
                                    newEntryState,
                                    &progress);
            NV_ASSERT(progress == entryIndexHi - entryIndexLo + 1);
        }
    }
    else
    {
        // Ensure hasn't changed.
        NV_ASSERT(pOldMem == pLevelInst->pMemDesc && oldSize == pLevelInst->memSize);
    }

    // Commit return.
    *ppLevelInst = pLevelInst;

done:
    // Cleanup newly allocated instance on failure.
    if (NV_OK != status &&
        bNew && NULL != pLevelInst)
    {
        _mmuWalkLevelInstRelease(pWalk, pLevel, pLevelInst);
    }
    return status;
}

/*!
 * Frees an unused level instance.
 */
static void
_mmuWalkLevelInstRelease
(
    const MMU_WALK      *pWalk,
    MMU_WALK_LEVEL      *pLevel,
    MMU_WALK_LEVEL_INST *pLevelInst
)
{
    NV_ASSERT(0 == pLevelInst->numValid);
    NV_ASSERT(0 == pLevelInst->numReserved);
    // Unlink.
    btreeUnlink(&pLevelInst->node, (NODE**)&pLevel->pInstances);
    // Free.
    if (NULL != pLevelInst->pMemDesc)
    {
        pWalk->pCb->LevelFree(pWalk->pUserCtx, pLevel->pFmt, pLevelInst->node.keyStart,
                              pLevelInst->pMemDesc);
    }
    portMemFree(pLevelInst->pStateTracker);
    portMemFree(pLevelInst);
}

/*!
 * This function is used to allocate a sublevel MMU_WALK_LEVEL_INST
 * for a given PDE. If the sublevel allocation succeeds, the parent Level is
 * updated.
 */
static NV_STATUS NV_NOINLINE
_mmuWalkPdeAcquire
(
    const MMU_WALK           *pWalk,
    const MMU_WALK_OP_PARAMS *pOpParams,
    MMU_WALK_LEVEL           *pLevel,
    MMU_WALK_LEVEL_INST      *pLevelInst,
    const NvU32               entryIndex,
    const NvU32               subLevel,
    const NvU64               vaLo,
    const NvU64               vaHi,
    MMU_WALK_LEVEL_INST      *pSubLevelInsts[]
)
{
    NV_STATUS               status  = NV_OK;
    NvBool                  bCommit = NV_FALSE;
    NvU32                   i;
    const MMU_WALK_MEMDESC *pSubMemDescs[MMU_FMT_MAX_SUB_LEVELS] = {0};
    NvU64                   vaLimit = vaHi;
    const NvU32             numSubLevels = pLevel->pFmt->numSubLevels;
    MMU_WALK_LEVEL_INST    *pCurSubLevelInsts[MMU_FMT_MAX_SUB_LEVELS] = {0};

    //
    // Determine minimum VA limit of existing sub-levels.
    // This is required to keep parallel partial page tables in sync.
    // MMU HW that supports partial size tables selects the size in the
    // parent PDE so each sub-level *MUST* be the same partial size
    // once allocated.
    //
    if (numSubLevels > 1)
    {
        for (i = 0; i < numSubLevels; ++i)
        {
            // Lookup sub-level instance.
            if (NV_OK == btreeSearch(vaLo, (NODE**)&pCurSubLevelInsts[i],
                                     (NODE*)pLevel->subLevels[i].pInstances))
            {
                const MMU_FMT_LEVEL *pSubLevelFmt = pLevel->pFmt->subLevels + i;
                const NvU64          minVaLimit =
                    mmuFmtLevelVirtAddrLo(pSubLevelFmt, vaLo) +
                        (pCurSubLevelInsts[i]->memSize /
                         pSubLevelFmt->entrySize *
                         mmuFmtLevelPageSize(pSubLevelFmt)) - 1;

                vaLimit = NV_MAX(vaLimit, minVaLimit);
            }
        }
    }

    //
    // the loop was reversed for NV4K, if there are multiple sublevels
    // handling small PT first, then the big PT
    //
    for (i = numSubLevels; i > 0; --i)
    {
        NvBool bChanged = NV_FALSE;
        NvU32  subLevelIdx = i - 1;
        NvBool bTarget = (subLevelIdx == subLevel);
        NvBool bInitNv4k = NV_FALSE;

        //
        // If NV4K is required (when ATS is enabled), acquire 64K PT
        // whenever the 4K PT has been acquired and 64K PT was not
        // there
        //
        if (pWalk->flags.bAtsEnabled && subLevelIdx == 0 &&
            numSubLevels > 1 && !pOpParams->bRelease)
        {
            if (pSubLevelInsts[1] != NULL)
            {
                bTarget = NV_TRUE;
            }
            if (pSubLevelInsts[0] == NULL)
            {
                bInitNv4k = NV_TRUE;
            }
        }

        // Acquire sub-level instance.
        NV_ASSERT_OK_OR_RETURN(
            _mmuWalkLevelInstAcquire(pWalk, pLevel->subLevels + subLevelIdx,
                                     vaLo, vaLimit, bTarget,
                                     pOpParams->bRelease, pOpParams->bCommit,
                                     &bChanged, &pSubLevelInsts[subLevelIdx],
                                     bInitNv4k));
        if (NULL == pSubLevelInsts[subLevelIdx])
        {
            // Skip missing non-target instances.
            NV_ASSERT(pOpParams->bRelease || !bTarget);
            continue;
        }

        // Track info for commit.
        bCommit        |= bChanged;
        pSubMemDescs[subLevelIdx] = pSubLevelInsts[subLevelIdx]->pMemDesc;
    }

    // DEBUG assert
    if (pWalk->flags.bAtsEnabled &&
        numSubLevels > 1 &&
        pSubLevelInsts[1] != NULL &&
        pSubLevelInsts[0] == NULL)
    {
        NV_ASSERT(0);
    }

    if (bCommit || pOpParams->bCommit)
    {
        NvBool bDone;

        // Update the current pde
        bDone = pWalk->pCb->UpdatePde(pWalk->pUserCtx, pLevel->pFmt, pLevelInst->pMemDesc,
                                      entryIndex, pSubMemDescs);
        NV_ASSERT_OR_RETURN(bDone, NV_ERR_INVALID_STATE);

        // Track entry as a PDE.
        mmuWalkSetEntryState(pLevelInst, entryIndex, MMU_ENTRY_STATE_IS_PDE);
    }

    return status;
}

/*!
 * Frees the sub levels of the PDE passed in if thier refcount is 0. It
 * also clears the PDE if both sublevels are released.
 */
static void NV_NOINLINE
_mmuWalkPdeRelease
(
    const MMU_WALK           *pWalk,
    const MMU_WALK_OP_PARAMS *pOpParams,
    MMU_WALK_LEVEL           *pLevel,
    MMU_WALK_LEVEL_INST      *pLevelInst,
    const NvU32               entryIndex,
    const NvU64               entryVaLo
)
{
    MMU_WALK_LEVEL_INST    *pSubLevelInsts[MMU_FMT_MAX_SUB_LEVELS] = {0};
    const MMU_WALK_MEMDESC *pSubMemDescs[MMU_FMT_MAX_SUB_LEVELS] = {0};
    NvBool                  bChanged = NV_FALSE;
    NvU32                   subLevel, i;
    MMU_ENTRY_STATE         state = MMU_ENTRY_STATE_INVALID;

    // Apply target state if this is a fill operation.
    if (pOpParams->bFill)
    {
        const MMU_FILL_TARGET *pTarget = (const MMU_FILL_TARGET *)pOpParams->pOpCtx;
        state = pTarget->entryState;
    }

    //
    // Loop through the sublevels and free up those with 0 ref count.
    // We operate on a temp copy of the PDE because we want to update the
    // PDE memory before releasing the actual sublevel pointers. We need this order
    // to prevent any state inconsistency between the parent MMU_DESC_PDE and
    // the sublevel MMU_WALK_LEVEL_INST structures.
    //
    for (i = pLevel->pFmt->numSubLevels; i > 0; --i)
    {
        subLevel = i - 1;
        if (NV_OK == btreeSearch(entryVaLo, (NODE**)&pSubLevelInsts[subLevel],
                                 (NODE*)pLevel->subLevels[subLevel].pInstances))
        {
            MMU_WALK_LEVEL_INST *pSubLevelInst = pSubLevelInsts[subLevel];

            // for ATS NV4K, check if we need to free the big page
            if (pLevel->pFmt->numSubLevels == 2 && subLevel == 0)
            {
                if (pWalk->flags.bAtsEnabled)
                {
                    if (pSubLevelInsts[0]->numNv4k ==
                            mmuFmtLevelEntryCount(pLevel->subLevels[0].pFmt) &&
                        (0 == pSubLevelInsts[0]->numReserved) &&
                        (pSubMemDescs[1] == NULL || bChanged == NV_TRUE))
                    {
                        bChanged = NV_TRUE;
                        continue;
                    }
                    else
                    {
                        state = MMU_ENTRY_STATE_IS_PDE;
                        pSubMemDescs[subLevel] = pSubLevelInst->pMemDesc;
                        continue;
                    }
                }
            }

            if ((0 != (pSubLevelInst->numValid + pSubLevelInst->numSparse)) ||
                (0 != (pSubLevelInst->numReserved + pSubLevelInst->numHybrid)))
            {
                // We've got at least one non-empty sublevel, so leave it mapped.
                state = MMU_ENTRY_STATE_IS_PDE;
                pSubMemDescs[subLevel] = pSubLevelInst->pMemDesc;
            }
            else if (NULL != pSubLevelInst->pMemDesc)
            {
                // We're going to free a sub-level.
                bChanged = NV_TRUE;
            }
        }
    }

    //
    // Failure path may have aborted early before sub-levels processed,
    // so also check that current state matches expected.
    //
    bChanged |= (state != mmuWalkGetEntryState(pLevelInst, entryIndex));

    //
    // If we've changed any sublevel we need to update the PDE in the parent
    // Page Directory
    //
    if (bChanged)
    {
        NvBool bDone;
        NvU32  progress = 0;

        // Init the PDE attribs with the temp PDE which has the cleared sublevel
        switch (state)
        {
        case MMU_ENTRY_STATE_SPARSE:
        case MMU_ENTRY_STATE_INVALID:
            pWalk->pCb->FillEntries(pWalk->pUserCtx,
                                    pLevel->pFmt,
                                    pLevelInst->pMemDesc,
                                    entryIndex,
                                    entryIndex,
                                    MMU_ENTRY_STATE_SPARSE == state ?
                                        MMU_WALK_FILL_SPARSE : MMU_WALK_FILL_INVALID,
                                    &progress);
            NV_ASSERT_OR_RETURN_VOID(progress == 1);
            // Clear the hybrid flag since all sub-levels are now released.
            if (pLevelInst->pStateTracker[entryIndex].bHybrid)
            {
                mmuWalkSetEntryHybrid(pLevelInst, entryIndex, NV_FALSE);
            }
            break;
        case MMU_ENTRY_STATE_IS_PDE:
            bDone = pWalk->pCb->UpdatePde(pWalk->pUserCtx, pLevel->pFmt, pLevelInst->pMemDesc,
                                          entryIndex, pSubMemDescs);
            NV_ASSERT_OR_RETURN_VOID(bDone);
            break;
        default:
            NV_ASSERT_OR_RETURN_VOID(0);
        }

        // Track new state of entry.
        mmuWalkSetEntryState(pLevelInst, entryIndex, state);
    }

    // Free up the actual sublevels from the PDE
    for (subLevel = 0; subLevel < pLevel->pFmt->numSubLevels; ++subLevel)
    {
        MMU_WALK_LEVEL_INST *pSubLevelInst = pSubLevelInsts[subLevel];
        if (NULL != pSubLevelInst &&
            NULL == pSubMemDescs[subLevel])
        {
            _mmuWalkLevelInstRelease(pWalk, pLevel->subLevels + subLevel,
                                      pSubLevelInst);
        }
    }
}

static void
_mmuWalkLevelInstancesForceFree
(
    MMU_WALK       *pWalk,
    MMU_WALK_LEVEL *pLevel
)
{
    MMU_WALK_LEVEL_INST *pLevelInst = NULL;
    NvU32                subLevel;

    if (NULL == pLevel)
        return;

    // Free all instances at this level.
    btreeEnumStart(0, (NODE **)&pLevelInst, (NODE*)pLevel->pInstances);
    while (NULL != pLevelInst)
    {
        //
        // Since we are force freeing everything, it is okay to reset these fields
        // in order to avoid hitting asserts in _mmuWalkLevelInstRelease.
        //
        pLevelInst->numValid    = 0;
        pLevelInst->numReserved = 0;
        _mmuWalkLevelInstRelease(pWalk, pLevel, pLevelInst);
        btreeEnumStart(0, (NODE **)&pLevelInst, (NODE*)pLevel->pInstances);
    }
    pLevel->pInstances = NULL;

    if (NULL != pLevel->subLevels)
    {
        for (subLevel = 0; subLevel < pLevel->pFmt->numSubLevels; subLevel++)
        {
            _mmuWalkLevelInstancesForceFree(pWalk, pLevel->subLevels + subLevel);
        }
    }
}

