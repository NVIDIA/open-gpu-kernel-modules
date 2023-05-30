/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#ifndef _NV_MMU_FMT_H_
#define _NV_MMU_FMT_H_

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @file mmu_fmt.h
 *
 * @brief Defines an abstraction over general MMU HW formats.
 *
 * The main goal is to leverage common page table management
 * code across a variety MMU HW formats.
 */
#include "nvtypes.h"
#include "nvmisc.h"
#include "compat.h"

//
// Separate files for the types as they're included by CTRL definitions
//
#include "mmu_fmt_types.h"

/*!
 * Get bitmask of page sizes supported under a given MMU level.
 *
 * Example: For the root level this returns all the page sizes
 *          supported by the MMU format.
 *
 * @returns Bitmask of page sizes (sufficient since page sizes are power of 2).
 */
NvU64 mmuFmtAllPageSizes(const MMU_FMT_LEVEL *pLevel);

/*!
 * Get bitmask of the VA coverages for each level, starting at a given level.
 * This is a superset of mmuFmtAllPageSizes, but includes page directory coverage bits.
 *
 * Example: For the root level this provides a summary of the VA breakdown.
 *          Each bit corresponds to the shift of a level in the format and
 *          the number bits set is equal to the total number of levels
 *          (including parallel sub-levels).
 *
 * @returns Bitmask of level VA coverages.
 */
NvU64 mmuFmtAllLevelCoverages(const MMU_FMT_LEVEL *pLevel);

/*!
 * Find a level with the given page shift.
 *
 * @param[in]  pLevel    Level format to start search.
 * @param[in]  pageShift log2(pageSize).
 *
 * @returns The level if found or NULL otherwise.
 */
const MMU_FMT_LEVEL *mmuFmtFindLevelWithPageShift(
                        const MMU_FMT_LEVEL *pLevel,
                        const NvU64          pageShift);

/*!
 * Find the parent level of a given level.
 *
 * @param[in]  pRoot     Root level format.
 * @param[in]  pLevel    Child level format.
 * @param[out] pSubLevel Returns the sub-level of the child within the parent if found.
 *                       Can be NULL if not needed.
 *
 * @returns Parent level if found or NULL otherwise.
 */
const MMU_FMT_LEVEL *mmuFmtFindLevelParent(
                        const MMU_FMT_LEVEL *pRoot,
                        const MMU_FMT_LEVEL *pLevel,
                        NvU32               *pSubLevel);

/*!
 * Get the next sub-level format in a search for a particular level.
 *
 * @returns Next level if found or NULL otherwise.
 */
const MMU_FMT_LEVEL *mmuFmtGetNextLevel(
                        const MMU_FMT_LEVEL *pLevelFmt,
                        const MMU_FMT_LEVEL *pTargetFmt);

/*!
 * Return a level description from the format. Used for Verif.
 *
 * @returns const char* string description of the level.
 */
const char*
mmuFmtConvertLevelIdToSuffix
(
    const MMU_FMT_LEVEL *pLevelFmt
);

/*!
 * Bitmask of VA covered by a given level.
 * e.g. for the root level this is the maximum VAS limit.
 */
static NV_FORCEINLINE NvU64
mmuFmtLevelVirtAddrMask(const MMU_FMT_LEVEL *pLevel)
{
    return NVBIT64(pLevel->virtAddrBitHi + 1) - 1;
}

/*!
 * Bitmask of VA covered by a single entry within a level.
 * e.g. (page size - 1) for PTEs within this level.
 */
static NV_FORCEINLINE NvU64
mmuFmtEntryVirtAddrMask(const MMU_FMT_LEVEL *pLevel)
{
    return NVBIT64(pLevel->virtAddrBitLo) - 1;
}

/*!
 * Bitmask of VA that contains the entry index of a level.
 */
static NV_FORCEINLINE NvU64
mmuFmtEntryIndexVirtAddrMask(const MMU_FMT_LEVEL *pLevel)
{
    return mmuFmtLevelVirtAddrMask(pLevel) & ~mmuFmtEntryVirtAddrMask(pLevel);
}

/*!
 * Extract the entry index of a level from a virtual address.
 */
static NV_FORCEINLINE NvU32
mmuFmtVirtAddrToEntryIndex(const MMU_FMT_LEVEL *pLevel, const NvU64 virtAddr)
{
    return (NvU32)((virtAddr & mmuFmtEntryIndexVirtAddrMask(pLevel)) >> pLevel->virtAddrBitLo);
}

/*!
 * Truncate a virtual address to the base of a level.
 */
static NV_FORCEINLINE NvU64
mmuFmtLevelVirtAddrLo(const MMU_FMT_LEVEL *pLevel, const NvU64 virtAddr)
{
    return virtAddr & ~mmuFmtLevelVirtAddrMask(pLevel);
}

/*!
 * Round a virtual address up to the limit covered by a level.
 */
static NV_FORCEINLINE NvU64
mmuFmtLevelVirtAddrHi(const MMU_FMT_LEVEL *pLevel, const NvU64 virtAddr)
{
    return mmuFmtLevelVirtAddrLo(pLevel, virtAddr) + mmuFmtLevelVirtAddrMask(pLevel);
}

/*!
 * Get the virtual address base of an entry index from the base virtual
 * address of its level.
 */
static NV_FORCEINLINE NvU64
mmuFmtEntryIndexVirtAddrLo(const MMU_FMT_LEVEL *pLevel, const NvU64 vaLevelBase,
                           const NvU32 entryIndex)
{
    NV_ASSERT_CHECKED_PRECOMP(0 == (vaLevelBase & mmuFmtLevelVirtAddrMask(pLevel)));
    return vaLevelBase + ((NvU64)entryIndex << pLevel->virtAddrBitLo);
}

/*!
 * Get the virtual address limit of an entry index from the base virtual
 * address of its level.
 */
static NV_FORCEINLINE NvU64
mmuFmtEntryIndexVirtAddrHi(const MMU_FMT_LEVEL *pLevel, const NvU64 vaLevelBase,
                           const NvU32 entryIndex)
{
    return mmuFmtEntryIndexVirtAddrLo(pLevel, vaLevelBase, entryIndex) +
                mmuFmtEntryVirtAddrMask(pLevel);
}

/*!
 * Get the page size for PTEs within a given MMU level.
 */
static NV_FORCEINLINE NvU64
mmuFmtLevelPageSize(const MMU_FMT_LEVEL *pLevel)
{
    return mmuFmtEntryVirtAddrMask(pLevel) + 1;
}

/*!
 * Extract the page offset of a virtual address based on a given MMU level.
 */
static NV_FORCEINLINE NvU64
mmuFmtVirtAddrPageOffset(const MMU_FMT_LEVEL *pLevel, const NvU64 virtAddr)
{
    return virtAddr & mmuFmtEntryVirtAddrMask(pLevel);
}

/*!
 * Calculate the maximum number of entries contained by a given MMU level.
 */
static NV_FORCEINLINE NvU32
mmuFmtLevelEntryCount(const MMU_FMT_LEVEL *pLevel)
{
    return NVBIT32(pLevel->virtAddrBitHi - pLevel->virtAddrBitLo + 1);
}

/*!
 * Calculate the maximum size in bytes of a given MMU level.
 */
static NV_FORCEINLINE NvU32
mmuFmtLevelSize(const MMU_FMT_LEVEL *pLevel)
{
    return mmuFmtLevelEntryCount(pLevel) * pLevel->entrySize;
}

#ifdef __cplusplus
}
#endif

#endif
