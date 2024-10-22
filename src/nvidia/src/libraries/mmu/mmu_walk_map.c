/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2015,2020,2022,2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * @file mmu_walk_map.c
 * @brief Describes the structures and interfaces used to map N level page tables
 */

/* ------------------------ Includes --------------------------------------- */
#if defined(SRT_BUILD)
#include "shrdebug.h"
#endif
#include "mmu_walk_private.h"

/* ------------------------ Macros ------------------------------------------ */
/* ------------------------ Static Function Prototypes ---------------------- */
static MmuWalkOp               _mmuWalkMap;
static MmuWalkOpSelectSubLevel _mmuWalkMapSelectSubLevel;

/* ------------------------ Inline Functions ---------------------------------*/
/* ------------------------ Public Functions  ------------------------------ */

NV_STATUS
mmuWalkMap
(
    MMU_WALK             *pWalk,
    const NvU64           vaLo,
    const NvU64           vaHi,
    const MMU_MAP_TARGET *pTarget
)
{
    MMU_WALK_OP_PARAMS   opParams = {0};
    NV_STATUS            status   = NV_OK;

    NV_ASSERT_OR_RETURN(NULL != pWalk, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(NULL != pTarget, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(vaLo <= vaHi, NV_ERR_INVALID_ARGUMENT);

    // Acquire the root. Call unconditionally to account for change of size
    status = mmuWalkRootAcquire(pWalk, vaLo, vaHi, NV_FALSE);
    NV_ASSERT_OR_RETURN(NV_OK == status, status);

    // Construct the map op params
    opParams.pOpCtx         = pTarget;
    opParams.opFunc         = _mmuWalkMap;
    opParams.selectSubLevel = _mmuWalkMapSelectSubLevel;

    // Start mapping from root (only one instance).
    status = mmuWalkProcessPdes(pWalk, &opParams, &pWalk->root, pWalk->root.pInstances, vaLo, vaHi);

    if (NV_OK != status)
    {
        NV_STATUS    unmapStatus;

        NV_PRINTF(LEVEL_ERROR,
                  "Failed to map VA Range 0x%llx to 0x%llx. Status = 0x%08x\n",
                  vaLo, vaHi, status);
        NV_ASSERT(0);

        // Mapping failed, unwind by unmapping the VA range
        unmapStatus = mmuWalkUnmap(pWalk, vaLo, vaHi);
        if (NV_OK != unmapStatus)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Unmap failed with status = 0x%08x\n",
                      unmapStatus);
            NV_ASSERT(NV_OK == unmapStatus);
        }
    }

    return status;
}

/* ----------------------------- Static Functions---------------------------- */

/*!
 * Implements the VA mapping operation after the root has been allocated.
 * @copydoc MmuWalkOp
 */
static NV_STATUS
_mmuWalkMap
(
    const MMU_WALK            *pWalk,
    const MMU_WALK_OP_PARAMS  *pOpParams,
    MMU_WALK_LEVEL            *pLevel,
    MMU_WALK_LEVEL_INST       *pLevelInst,
    NvU64                      vaLo,
    NvU64                      vaHi
)
{
    const MMU_MAP_TARGET *pTarget = (const MMU_MAP_TARGET *) pOpParams->pOpCtx;

    NV_ASSERT_OR_RETURN(NULL != pLevelInst, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(NULL != pLevel, NV_ERR_INVALID_ARGUMENT);

    // If this level is not the targetted page level.
    if (pLevel->pFmt != pTarget->pLevelFmt)
    {
        NV_ASSERT_OR_RETURN(0 != pLevel->pFmt->numSubLevels, NV_ERR_INVALID_ARGUMENT);

        return NV_ERR_MORE_PROCESSING_REQUIRED;
    }
    // We have reached the target page level.
    else
    {
        const NvU32 entryIndexLo = mmuFmtVirtAddrToEntryIndex(pLevel->pFmt, vaLo);
        const NvU32 entryIndexHi = mmuFmtVirtAddrToEntryIndex(pLevel->pFmt, vaHi);
        NvU32       progress     = 0;
        NvU32       entryIndex;

        // Ensure child-sub-levels are unmapped before mapping "hybrid" PDE-PTEs.
        if (0 != pLevel->pFmt->numSubLevels)
        {
            const NvU64 vaLevelBase = mmuFmtLevelVirtAddrLo(pLevel->pFmt, vaLo);
            for (entryIndex = entryIndexLo; entryIndex <= entryIndexHi; entryIndex++)
            {
                // But don't unmap existing target entries since the "mapping" below can be RMW.
                if (MMU_ENTRY_STATE_IS_PTE != mmuWalkGetEntryState(pLevelInst, entryIndex))
                {
                    const NvU64 entryVaLo =
                        mmuFmtEntryIndexVirtAddrLo(pLevel->pFmt, vaLevelBase, entryIndex);
                    const NvU64 entryVaHi =
                        mmuFmtEntryIndexVirtAddrHi(pLevel->pFmt, vaLevelBase, entryIndex);

                    NV_ASSERT_OK_OR_RETURN(
                        mmuWalkProcessPdes(pWalk,
                                           &g_opParamsUnmap,
                                           pLevel,
                                           pLevelInst,
                                           entryVaLo,
                                           entryVaHi));


                    //
                    // If this entry is still a PDE it means there are reserved sub-levels underneath.
                    // Mark the entry as a hybrid so that its instance remains pinned appropriately.
                    //
                    if (MMU_ENTRY_STATE_IS_PDE == mmuWalkGetEntryState(pLevelInst, entryIndex))
                    {
                        mmuWalkSetEntryHybrid(pLevelInst, entryIndex, NV_TRUE);
                    }
                }
            }
        }

        // Map the next batch of entry values.
        pTarget->MapNextEntries(pWalk->pUserCtx,
                                pTarget,
                                pLevelInst->pMemDesc,
                                entryIndexLo,
                                entryIndexHi,
                                &progress);
        NV_ASSERT_OR_RETURN(progress == entryIndexHi - entryIndexLo + 1, NV_ERR_INVALID_STATE);

        // Loop over PTEs again to update state tracker.
        for (entryIndex = entryIndexLo; entryIndex <= entryIndexHi; entryIndex++)
        {
            mmuWalkSetEntryState(pLevelInst, entryIndex, MMU_ENTRY_STATE_IS_PTE);
        }
    }

    return NV_OK;
}

/*!
 * TODO
 */
static NV_STATUS
_mmuWalkMapSelectSubLevel
(
    const void             *pOpCtx,
    const MMU_WALK_LEVEL   *pLevel,
    NvU32                  *pSubLevel,
    NvU64                   vaLo,
    NvU64                   vaHi
)
{
    const MMU_MAP_TARGET *pTarget = (const MMU_MAP_TARGET *) pOpCtx;
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

        if (pSubLevelFmt == pTarget->pLevelFmt)
        {
            *pSubLevel = subLevel;
            return NV_OK;
        }
    }
    // Error if we didn't find a matching page size
    return NV_ERR_INVALID_STATE;
}
