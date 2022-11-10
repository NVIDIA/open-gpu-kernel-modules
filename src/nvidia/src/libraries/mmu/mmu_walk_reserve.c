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

/*--------------------------------Includes------------------------------------*/
#if defined(SRT_BUILD)
#include "shrdebug.h"
#endif

#include "mmu_walk_private.h"

/* ------------------------ Static Function Prototypes ---------------------- */
static MmuWalkOp               _mmuWalkReserveEntries;
static MmuWalkOp               _mmuWalkReleaseEntries;
static MmuWalkOpSelectSubLevel _mmuWalkReserveSelectSubLevel;

/*----------------------------Public Functions--------------------------------*/
NV_STATUS
mmuWalkReserveEntries
(
    MMU_WALK             *pWalk,
    const MMU_FMT_LEVEL  *pLevelFmt,
    const NvU64           vaLo,
    const NvU64           vaHi,
    const NvBool          bInvalidate
)
{
    MMU_WALK_OP_PARAMS   opParams = {0};
    NV_STATUS            status   = NV_OK;

    NV_ASSERT_OR_RETURN(NULL != mmuWalkFindLevel(pWalk, pLevelFmt),
                     NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(NV_IS_ALIGNED(vaLo, mmuFmtLevelPageSize(pLevelFmt)),
                     NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(NV_IS_ALIGNED(vaHi + 1, mmuFmtLevelPageSize(pLevelFmt)),
                     NV_ERR_INVALID_ARGUMENT);

    // Acquire the root. Call unconditionally to account for change of size
    status = mmuWalkRootAcquire(pWalk, vaLo, vaHi, NV_FALSE);
    NV_ASSERT_OR_RETURN(NV_OK == status, status);
    NV_ASSERT_OR_RETURN(pWalk->bInvalidateOnReserve, NV_ERR_INVALID_STATE);

    //
    // Whether to skip invalidation of PTEs during reservation (for example,
    // when sparsifying immediately afterwards)
    //
    pWalk->bInvalidateOnReserve = bInvalidate;

    // Construct the map op params
    opParams.pOpCtx                   = pLevelFmt;
    opParams.opFunc                   = _mmuWalkReserveEntries;
    opParams.selectSubLevel           = _mmuWalkReserveSelectSubLevel;
    opParams.bIgnoreSubLevelConflicts = NV_TRUE;

    // Start reserving from root (only one instance).
    if (pWalk->flags.bUseIterative)
    {
        status = mmuWalkProcessPdes(pWalk, &opParams, &pWalk->root, pWalk->root.pInstances,
                                    vaLo, vaHi);
    }
    else
    {
        status = _mmuWalkReserveEntries(pWalk, &opParams, &pWalk->root, pWalk->root.pInstances,
                                        vaLo, vaHi);
    }

    if (NV_OK != status)
    {
        NV_STATUS tmpStatus;

        // Reservation failed, unwind by releasing the VA range
        tmpStatus = mmuWalkReleaseEntries(pWalk, pLevelFmt, vaLo, vaHi);
        NV_ASSERT(NV_OK == tmpStatus);
    }

    // Reset invalidation enable
    pWalk->bInvalidateOnReserve = NV_TRUE;

    return status;
}

NV_STATUS
mmuWalkReleaseEntries
(
    MMU_WALK             *pWalk,
    const MMU_FMT_LEVEL  *pLevelFmt,
    const NvU64           vaLo,
    const NvU64           vaHi
)
{
    MMU_WALK_OP_PARAMS   opParams = {0};
    NV_STATUS            status   = NV_OK;

    NV_ASSERT_OR_RETURN(NULL != pWalk, NV_ERR_INVALID_ARGUMENT);

    NV_ASSERT_OR_RETURN(NULL != mmuWalkFindLevel(pWalk, pLevelFmt),
                     NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(NV_IS_ALIGNED(vaLo, mmuFmtLevelPageSize(pLevelFmt)),
                     NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(NV_IS_ALIGNED(vaHi + 1, mmuFmtLevelPageSize(pLevelFmt)),
                     NV_ERR_INVALID_ARGUMENT);

    // Start releasing from root (only one instance).
    if (NULL != pWalk->root.pInstances)
    {
        // Construct the map op params
        opParams.pOpCtx                   = pLevelFmt;
        opParams.opFunc                   = _mmuWalkReleaseEntries;
        opParams.selectSubLevel           = _mmuWalkReserveSelectSubLevel;
        opParams.bIgnoreSubLevelConflicts = NV_TRUE;
        opParams.bRelease                 = NV_TRUE;

        if (pWalk->flags.bUseIterative)
        {
            status = mmuWalkProcessPdes(pWalk, &opParams, &pWalk->root, pWalk->root.pInstances,
                                        vaLo, vaHi);
        }
        else
        {
            status = _mmuWalkReleaseEntries(pWalk, &opParams, &pWalk->root, pWalk->root.pInstances,
                                            vaLo, vaHi);
        }

        NV_ASSERT_OR_RETURN(NV_OK == status, status);

        // Release the root.
        mmuWalkRootRelease(pWalk);
    }

    return status;
}

/* ----------------------------- Static Functions---------------------------- */

static NV_STATUS
_mmuWalkReserveEntries
(
    const MMU_WALK            *pWalk,
    const MMU_WALK_OP_PARAMS  *pOpParams,
    MMU_WALK_LEVEL            *pLevel,
    MMU_WALK_LEVEL_INST       *pLevelInst,
    NvU64                      vaLo,
    NvU64                      vaHi
)
{
    const MMU_FMT_LEVEL *pLevelFmt = (const MMU_FMT_LEVEL *) pOpParams->pOpCtx;

    // If this level is not the targetted page level.
    if (pLevel->pFmt != pLevelFmt)
    {
        NV_ASSERT_OR_RETURN(0 != pLevel->pFmt->numSubLevels, NV_ERR_INVALID_ARGUMENT);

        if (pWalk->flags.bUseIterative)
        {
            return NV_ERR_MORE_PROCESSING_REQUIRED;
        }
        else
        {
            // Process all the page level entries falling within [vaLo, vaHi]
            NV_ASSERT_OK_OR_RETURN(
                mmuWalkProcessPdes(pWalk,
                                   pOpParams,
                                   pLevel,
                                   pLevelInst,
                                   vaLo,
                                   vaHi));
        }
    }
    // We have reached the target page level.
    else
    {
        const NvU32 entryIndexLo = mmuFmtVirtAddrToEntryIndex(pLevel->pFmt, vaLo);
        const NvU32 entryIndexHi = mmuFmtVirtAddrToEntryIndex(pLevel->pFmt, vaHi);
        NvU32       entryIndex;

        // Update the state tracker
        for (entryIndex = entryIndexLo; entryIndex <= entryIndexHi; entryIndex++)
        {
            mmuWalkSetEntryReserved(pLevelInst, entryIndex, NV_TRUE);
        }
    }

    return NV_OK;
}

static NV_STATUS
_mmuWalkReleaseEntries
(
    const MMU_WALK            *pWalk,
    const MMU_WALK_OP_PARAMS  *pOpParams,
    MMU_WALK_LEVEL            *pLevel,
    MMU_WALK_LEVEL_INST       *pLevelInst,
    NvU64                      vaLo,
    NvU64                      vaHi
)
{
    const MMU_FMT_LEVEL *pLevelFmt = (const MMU_FMT_LEVEL *) pOpParams->pOpCtx;

    // If this level is not the targetted page level.
    if (pLevel->pFmt != pLevelFmt)
    {
        NV_ASSERT_OR_RETURN(0 != pLevel->pFmt->numSubLevels, NV_ERR_INVALID_ARGUMENT);

        if (pWalk->flags.bUseIterative)
        {
            return NV_ERR_MORE_PROCESSING_REQUIRED;
        }
        else
        {
            // Process all the page level entries falling within [vaLo, vaHi]
            NV_ASSERT_OK_OR_RETURN(
                mmuWalkProcessPdes(pWalk,
                                   pOpParams,
                                   pLevel,
                                   pLevelInst,
                                   vaLo,
                                   vaHi));
        }
    }
    // We have reached the target page level.
    else
    {
        const NvU32 entryIndexLo = mmuFmtVirtAddrToEntryIndex(pLevel->pFmt, vaLo);
        const NvU32 entryIndexHi = mmuFmtVirtAddrToEntryIndex(pLevel->pFmt, vaHi);
        NvU32       entryIndex;

        // Update the state tracker
        for (entryIndex = entryIndexLo; entryIndex <= entryIndexHi; entryIndex++)
        {
            mmuWalkSetEntryReserved(pLevelInst, entryIndex, NV_FALSE);
        }
    }

    return NV_OK;
}

static NV_STATUS
_mmuWalkReserveSelectSubLevel
(
    const void             *pOpCtx,
    const MMU_WALK_LEVEL   *pLevel,
    NvU32                  *pSubLevel,
    NvU64                   vaLo,
    NvU64                   vaHi
)
{
    const MMU_FMT_LEVEL  *pLevelFmt = (const MMU_FMT_LEVEL *) pOpCtx;
    NvU32                 subLevel;
    const MMU_FMT_LEVEL  *pSubLevelFmt = NULL;

    // If we've only one sublevel, it's at index 0.
    if (pLevel->pFmt->numSubLevels == 1)
    {
        *pSubLevel = 0;
        return NV_OK;
    }

    for (subLevel = 0; subLevel < pLevel->pFmt->numSubLevels; subLevel++)
    {
        pSubLevelFmt = pLevel->pFmt->subLevels + subLevel;

        if (pSubLevelFmt == pLevelFmt)
        {
            *pSubLevel = subLevel;
            return NV_OK;
        }
    }
    // Error if we didn't find a matching page size
    return NV_ERR_INVALID_STATE;
}
