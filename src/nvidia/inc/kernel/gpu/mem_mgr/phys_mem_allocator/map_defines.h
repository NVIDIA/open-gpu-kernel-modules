/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 *  @brief Contains common defines for regmap
 */

#ifndef MAP_DEFINES_H
#define MAP_DEFINES_H

#include "nvtypes.h"
#include "nvstatus.h"
#include "nvmisc.h"
#if !defined(NVWATCH)

#endif

#if defined(SRT_BUILD)
#define RMCFG_MODULE_x 1
#define RMCFG_FEATURE_x 1
#else
#include "rmconfig.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

// Default page size 64KB
#define PMA_GRANULARITY 0x10000
#define PMA_PAGE_SHIFT 16

// Defines shared between pma.c and regmap.c
#define _PMA_64KB               (64ULL  * 1024)
#define _PMA_128KB              (128ULL * 1024)
#define _PMA_2MB                (2ULL   * 1024 * 1024)
#define _PMA_512MB              (512ULL * 1024 * 1024)

// Localized memory chunks are 64MB split into 2 32MB chunks
#define PMA_LOCALIZED_MEMORY_ALLOC_STRIDE   (32ULL * 1024 * 1024)
#define PMA_LOCALIZED_MEMORY_RESERVE_SIZE   (2 * PMA_LOCALIZED_MEMORY_ALLOC_STRIDE)


typedef NvU32 PMA_PAGESTATUS;

#define MAP_IDX_ALLOC_UNPIN 0
#define MAP_IDX_ALLOC_PIN   1
#define MAP_IDX_EVICTING    2
#define MAP_IDX_SCRUBBING   3
#define MAP_IDX_PERSISTENT  4
#define MAP_IDX_NUMA_REUSE  5
#define MAP_IDX_BLACKLIST   6
#define MAP_IDX_LOCALIZED   7

#define STATE_FREE      0x00
#define STATE_UNPIN     NVBIT(MAP_IDX_ALLOC_UNPIN)
#define STATE_PIN       NVBIT(MAP_IDX_ALLOC_PIN)
#define STATE_MASK      (STATE_UNPIN | STATE_PIN | STATE_FREE)
#define STATE_COUNT     3

#define ATTRIB_EVICTING    NVBIT(MAP_IDX_EVICTING)
#define ATTRIB_SCRUBBING   NVBIT(MAP_IDX_SCRUBBING)
#define ATTRIB_PERSISTENT  NVBIT(MAP_IDX_PERSISTENT)
#define ATTRIB_NUMA_REUSE  NVBIT(MAP_IDX_NUMA_REUSE)
#define ATTRIB_BLACKLIST   NVBIT(MAP_IDX_BLACKLIST)
#define ATTRIB_LOCALIZED   NVBIT(MAP_IDX_LOCALIZED)

#define ATTRIB_MASK        (ATTRIB_EVICTING | ATTRIB_SCRUBBING      \
                            | ATTRIB_PERSISTENT | ATTRIB_NUMA_REUSE \
                            | ATTRIB_BLACKLIST | ATTRIB_LOCALIZED)

#define MAP_MASK    (STATE_MASK | ATTRIB_MASK)

#define PMA_STATE_BITS_PER_PAGE     2   // Alloc & pinned state
#define PMA_ATTRIB_BITS_PER_PAGE    6   // Persistence, Scrubbing, Evicting, Reuse, Blacklisting, & Localized attributes

#define PMA_BITS_PER_PAGE           (PMA_STATE_BITS_PER_PAGE + PMA_ATTRIB_BITS_PER_PAGE)

//
// Stores PMA-wide statistics.
//
// In NUMA mode "free" means "not allocated by PMA". Since the kernel owns
// memory, any amount of those pages could be allocated by the kernel but PMA
// does not have that visibility. The provided counts are thus an upper bound on
// the number of free pages.
//
typedef struct _PMA_STATS
{
    NvU64 num2mbPages;               // PMA-wide total number of 2MB pages
    NvU64 numFreeFrames;             // PMA-wide free 64KB frame count
    NvU64 numFree2mbPages;           // PMA-wide free 2MB pages count
#if !defined(NVWATCH)
    NvU64 num2mbPagesProtected;      // PMA-wide total number of 2MB pages in protected memory
    NvU64 numFreeFramesProtected;    // PMA-wide free 64KB frame count in protected memory
    NvU64 numFree2mbPagesProtected;  // PMA-wide free 2MB pages count in protected memory
#endif // !defined(NVWATCH)
} PMA_STATS;

// Stores blacklisting information passed in from heap layer
typedef struct
{
    NvU64  physOffset; // base address of blacklisted page
    NvBool bIsDynamic; // True if page was dynamically blacklisted
} PMA_BLACKLIST_ADDRESS, *PPMA_BLACKLIST_ADDRESS;

//
// Store the blacklist chunk information with the physical offset aligned to 64K,
// and whether the blacklist chunk is managed by RM or Client.
//
typedef struct
{
    NvU64  physOffset; // physical offset of blacklisted FB address
    NvBool bIsDynamic;  // True if the page was dynamically blacklisted
    NvBool bIsValid;   // If the blacklisted address is still managed by RM
} PMA_BLACKLIST_CHUNK;


#ifdef __cplusplus
}
#endif

#endif // MAP_DEFINES_H
