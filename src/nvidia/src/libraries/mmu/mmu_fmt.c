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

#include "mmu/mmu_fmt.h"

NvU64
mmuFmtAllPageSizes(const MMU_FMT_LEVEL *pLevel)
{
    NvU32 i;
    NvU64 mask = 0;
    if (pLevel->bPageTable)
    {
        mask |= mmuFmtLevelPageSize(pLevel);
    }
    for (i = 0; i < pLevel->numSubLevels; ++i)
    {
        mask |= mmuFmtAllPageSizes(pLevel->subLevels + i);
    }
    return mask;
}

NvU64
mmuFmtAllLevelCoverages(const MMU_FMT_LEVEL *pLevel)
{
    NvU32 i;
    NvU64 mask = mmuFmtLevelPageSize(pLevel);
    for (i = 0; i < pLevel->numSubLevels; ++i)
    {
        mask |= mmuFmtAllLevelCoverages(pLevel->subLevels + i);
    }
    return mask;
}

const MMU_FMT_LEVEL *
mmuFmtFindLevelWithPageShift
(
    const MMU_FMT_LEVEL *pLevel,
    const NvU64          pageShift
)
{
    NvU32 i;
    if (pLevel->virtAddrBitLo == pageShift)
    {
        return pLevel;
    }
    for (i = 0; i < pLevel->numSubLevels; ++i)
    {
        const MMU_FMT_LEVEL *pRes =
            mmuFmtFindLevelWithPageShift(pLevel->subLevels + i, pageShift);
        if (NULL != pRes)
        {
            return pRes;
        }
    }
    return NULL;
}

const MMU_FMT_LEVEL *
mmuFmtFindLevelParent
(
    const MMU_FMT_LEVEL *pRoot,
    const MMU_FMT_LEVEL *pLevel,
    NvU32               *pSubLevel
)
{
    NvU32 i;
    for (i = 0; i < pRoot->numSubLevels; ++i)
    {
        const MMU_FMT_LEVEL *pRes;
        if ((pRoot->subLevels + i) == pLevel)
        {
            if (NULL != pSubLevel)
            {
                *pSubLevel = i;
            }
            pRes = pRoot;
        }
        else
        {
            pRes = mmuFmtFindLevelParent(pRoot->subLevels + i, pLevel, pSubLevel);
        }
        if (NULL != pRes)
        {
            return pRes;
        }
    }
    return NULL;
}

const MMU_FMT_LEVEL *
mmuFmtGetNextLevel
(
    const MMU_FMT_LEVEL *pLevelFmt,
    const MMU_FMT_LEVEL *pTargetFmt
)
{
    if (pLevelFmt != pTargetFmt)
    {
        NvU32 subLevel = 0;
        if (1 == pLevelFmt->numSubLevels)
        {
            return pLevelFmt->subLevels;
        }
        for (subLevel = 0; subLevel < pLevelFmt->numSubLevels; ++subLevel)
        {
            const MMU_FMT_LEVEL *pSubLevelFmt = pLevelFmt->subLevels + subLevel;
            if (pSubLevelFmt == pTargetFmt)
            {
                return pSubLevelFmt;
            }
        }
    }
    return NULL;
}
