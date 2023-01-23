/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2018 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/mem_mgr/mem_desc.h"

/*----------------------------Public Functions--------------------------------*/
NV_STATUS
mmuWalkModifyLevelInstance
(
    MMU_WALK             *pWalk,
    const MMU_FMT_LEVEL  *pLevelFmt,
    const NvU64           vaBase,
    MMU_WALK_MEMDESC     *pNewMem,
    const NvU32           newSize,
    const NvBool          bCopyEntries,
    const NvBool          bUpdatePde,
    const NvBool          bIgnoreChannelBusy
)
{
    const MMU_WALK_LEVEL *pLevel     = mmuWalkFindLevel(pWalk, pLevelFmt);
    MMU_WALK_LEVEL_INST  *pLevelInst = NULL;
    MMU_WALK_MEMDESC     *pOldMem;
    NvU32                 oldSize;
    NvU32                 entryIndexHiOld;
    NvU32                 entryIndexHiNew;
    NvU32                 entryIndexHiCopy;
    NvU32                 progress;

    // Validate user callback.
    NV_ASSERT_OR_RETURN(NULL != pWalk->pCb->CopyEntries, NV_ERR_NOT_SUPPORTED);

    // Validate level.
    NV_ASSERT_OR_RETURN(NULL != pLevel, NV_ERR_INVALID_ARGUMENT);

    // Lookup level instance.
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, btreeSearch(vaBase, (NODE**)&pLevelInst, &pLevel->pInstances->node));
    NV_ASSERT_OR_RETURN(NULL != pLevelInst, NV_ERR_INVALID_ARGUMENT);

    // Temp old memory.
    pOldMem = pLevelInst->pMemDesc;
    oldSize = pLevelInst->memSize;

    // Save new memory.
    pLevelInst->pMemDesc = pNewMem;
    pLevelInst->memSize  = newSize;

    //
    // Enforce alignment.
    // This check is applicable to the case of @ref mmuWalkMigrateLevelInstance
    //
    if (bUpdatePde)
    {
        NV_ASSERT_OR_RETURN(vaBase == pLevelInst->node.keyStart, NV_ERR_INVALID_ARGUMENT);
    }

    entryIndexHiOld  = (oldSize / pLevelFmt->entrySize) - 1;
    entryIndexHiNew  = (newSize / pLevelFmt->entrySize) - 1;
    entryIndexHiCopy = NV_MIN(entryIndexHiOld, entryIndexHiNew);

    if (bCopyEntries)
    {
        // Copy old entries.
        pWalk->pCb->CopyEntries(pWalk->pUserCtx,
                                pLevel->pFmt,
                                pOldMem,
                                pNewMem,
                                0,
                                entryIndexHiCopy,
                                &progress);
        NV_ASSERT(progress == entryIndexHiCopy + 1);

        // Fill new entries to invalid.
        if (entryIndexHiNew > entryIndexHiOld)
        {
            pWalk->pCb->FillEntries(pWalk->pUserCtx,
                                    pLevel->pFmt,
                                    pNewMem,
                                    entryIndexHiOld + 1,
                                    entryIndexHiNew,
                                    MMU_WALK_FILL_INVALID,
                                    &progress);
            NV_ASSERT(progress == entryIndexHiNew - entryIndexHiOld);
        }
    }

    // Commit new instance to parent.
    if (NULL == pLevel->pParent)
    {
        // Root case requires PDB update.
        NvBool bDone;
        bDone = pWalk->pCb->UpdatePdb(pWalk->pUserCtx, pWalk->root.pFmt,
                                      pNewMem, bIgnoreChannelBusy);
        NV_ASSERT(bDone);
    }
    else if (bUpdatePde)
    {
        // Non-root case requires PDE update.
        NvBool                  bDone;
        NvU32                   i;
        const MMU_WALK_LEVEL   *pParent = pLevel->pParent;
        MMU_WALK_LEVEL_INST    *pParentInst = NULL;
        const MMU_WALK_MEMDESC *pSubMemDescs[MMU_FMT_MAX_SUB_LEVELS] = {0};
        const NvU32             entryIndex = mmuFmtVirtAddrToEntryIndex(pParent->pFmt, vaBase);

        // Lookup parent instance.
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, btreeSearch(vaBase, (NODE**)&pParentInst, &pParent->pInstances->node));
        NV_ASSERT(NULL != pParentInst);

        // Collect sub-level memory.
        for (i = 0; i < pParent->pFmt->numSubLevels; ++i)
        {
            const MMU_WALK_LEVEL *pSubLevel = pParent->subLevels + i;

            // Target sub-level picks up new memory.
            if (pSubLevel == pLevel)
            {
                pSubMemDescs[i] = pNewMem;
            }
            // Other sub-levels need lookup.
            else
            {
                MMU_WALK_LEVEL_INST *pOtherInst = NULL;
                NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, btreeSearch(vaBase, (NODE**)&pOtherInst, &pSubLevel->pInstances->node));
                if (NULL != pOtherInst)
                {
                    pSubMemDescs[i] = pOtherInst->pMemDesc;
                }
            }
        }

        // Write new PDE value.
        bDone = pWalk->pCb->UpdatePde(pWalk->pUserCtx, pParent->pFmt, pParentInst->pMemDesc,
                                      entryIndex, pSubMemDescs);
        NV_ASSERT(bDone);
    }

    //
    // Free old memory, if needed.
    // When
    // _gmmuWalkCBLevelAlloc_SharedSLI() is removed as part of the FB heap split effort for SLI,
    // we will not need (((MEMORY_DESCRIPTOR *)pOldMem)->Allocated > 1).
    //
    if ((pOldMem != pNewMem) || (((MEMORY_DESCRIPTOR *)pOldMem)->Allocated > 1))
    {
        pWalk->pCb->LevelFree(pWalk->pUserCtx, pLevel->pFmt, pLevelInst->node.keyStart, pOldMem);
    }

    return NV_OK;
}

NV_STATUS
mmuWalkMigrateLevelInstance
(
    MMU_WALK             *pWalk,
    const MMU_FMT_LEVEL  *pLevelFmt,
    const NvU64           vaBase,
    MMU_WALK_MEMDESC     *pNewMem,
    const NvU32           newSize,
    const NvBool          bIgnoreChannelBusy
)
{
    return mmuWalkModifyLevelInstance(pWalk, pLevelFmt, vaBase, pNewMem,
                          newSize, NV_TRUE, NV_TRUE, bIgnoreChannelBusy);
}
