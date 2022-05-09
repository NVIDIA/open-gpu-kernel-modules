/*
 * SPDX-FileCopyrightText: Copyright (c) 2014 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*----------------------------Public Functions--------------------------------*/
NV_STATUS
mmuWalkGetPageLevelInfo
(
    const MMU_WALK          *pWalk,
    const MMU_FMT_LEVEL     *pLevelFmt,
    const NvU64              virtAddr,
    const MMU_WALK_MEMDESC **ppMemDesc,
    NvU32                   *pMemSize
)
{
    const MMU_WALK_LEVEL *pLevel     = mmuWalkFindLevel(pWalk, pLevelFmt);
    MMU_WALK_LEVEL_INST  *pLevelInst = NULL;

    // Clear outputs.
    *ppMemDesc = NULL;
    *pMemSize  = 0;

    // Validate level.
    NV_ASSERT_OR_RETURN(NULL != pLevel, NV_ERR_INVALID_ARGUMENT);

    // Lookup level instance.
    if (NV_OK == btreeSearch(virtAddr, (NODE**)&pLevelInst, &pLevel->pInstances->node))
    {
        // Return memory info if found.
        *ppMemDesc = pLevelInst->pMemDesc;
        *pMemSize  = pLevelInst->memSize;
    }

    return NV_OK;
}

