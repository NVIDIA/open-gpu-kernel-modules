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
#pragma once

#include <nvtypes.h>

//
// This file was generated with FINN, an NVIDIA coding tool.
// Source file:      mmu_fmt_types.finn
//




/*!
 * @file mmu_fmt_types.h
 *
 * @brief Types used to describre MMU HW formats.
 */
#include "nvtypes.h"

// Forward declarations.


/*!
 * Generic MMU page directory/table level format description.
 *
 * Since the terminology of page directories and tables varies,
 * the following describes the interpretation assumed here.
 *
 * Each level of virtual address translation is described by a range of
 * virtual address bits.
 * These bits index into a contiguous range of physical memory referred to
 * generally as a "page level."
 * Page level memory is interpreted as an array of entries, with each entry
 * describing the next step of virtual to physical translation.
 *
 * Each entry in a given level may be interpreted as either a PDE or PTE.
 * 1. A PDE (page directory entry) points to one or more "sub-levels" that
 *    continue the VA translation recursively.
 * 2. A PTE (page table entry) is the base case, pointing to a physical page.
 *
 * The decision to treat an entry as a PDE or PTE may be static for a level.
 * Levels that only contain PDEs are referred to as page directories.
 * Levels that only contain PTEs are referred to as page tables.
 *
 * However, some formats have levels that may contain a mix of PDEs and PTEs,
 * with the intpretation based on a "cutoff" bit in each entry (e.g. PTE valid bit).
 * Such levels are referred to as "polymorphic page levels" since they can be
 * viewed as both a page directory and a page table.
 */
typedef struct MMU_FMT_LEVEL {
    /*!
     * First virtual address bit that this page level covers.
     */
    NvU8   virtAddrBitLo;

    /*!
     * Last virtual address bit that this page level covers.
     */
    NvU8   virtAddrBitHi;

    /*!
     * Size in bytes of each entry within a level instance.
     */
    NvU8   entrySize;

    /*!
     * Indicates if this level can contain PTEs.
     */
    NvBool bPageTable;

    /*!
     * Number of sub-levels pointed to by PDEs in this level in
     * range [0, MMU_FMT_MAX_SUB_LEVELS].
     * 0 indicates this level cannot contain PDEs.
     */
    NvU8   numSubLevels;

    /*!
     * Information tag used to decode internal level naming.
     * Used for verif.
     * */
    NvU32  pageLevelIdTag;


    /*!
     * Array of sub-level formats of length numSubLevels.
     *
     * @warning This array results in a circular reference to MMU_FMT_LEVEL.
     *          This can present an issue for FINN serialization and may have to
     *          be refactored before MMU_FMT_LEVEL can be serialized.
     */
    NV_DECLARE_ALIGNED(struct MMU_FMT_LEVEL *subLevels, 8);
} MMU_FMT_LEVEL;

/*!
 * Maximum number of pointers to sub-levels within a page directory entry.
 *
 * Standard page directory entries (PDEs) point to a single sub-level,
 * either the next page directory level in the topology or a leaf page table.
 *
 * However, some formats contain PDEs that point to more than one sub-level.
 * These sub-levels are translated by HW in parallel to support multiple
 * page sizes at a higher granularity (e.g. for migration between
 * 4K system memory pages and big video memory pages for GPU MMU).
 *
 * The current supported formats have a maximum of 2 parallel sub-levels,
 * often referred to as "dual PDE" or "dual page table" support.
 *
 * Example for Fermi GPU HW:
 *      Sub-level 0 corresponds to big page table pointer.
 *      Sub-level 1 corresponds to small page table pointer.
 *
 * This number is very unlikely to change, but it is defined to
 * simplify SW handling, encouraging loops over "dual copy-paste."
 */
#define MMU_FMT_MAX_SUB_LEVELS 2

/*!
 * Valid values for the pageLevelIdTag field.
 * These values are used to identify the internal naming of the page level.
 * Used for verif purposes.
 */
#define MMU_FMT_PT_SURF_ID_PD0           0
#define MMU_FMT_PT_SURF_ID_PD1           1
#define MMU_FMT_PT_SURF_ID_PD2           2
#define MMU_FMT_PT_SURF_ID_PD3           3
#define MMU_FMT_PT_SURF_ID_PD4           4
#define MMU_FMT_PT_SURF_ID_PT_4K         5
#define MMU_FMT_PT_SURF_ID_PT_BIG        6
