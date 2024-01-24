/*******************************************************************************
    Copyright (c) 2016-2022 NVIDIA Corporation

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

        The above copyright notice and this permission notice shall be
        included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.

*******************************************************************************/

#ifndef __UVM_PERF_PREFETCH_H__
#define __UVM_PERF_PREFETCH_H__

#include "uvm_linux.h"
#include "uvm_processors.h"
#include "uvm_va_block_types.h"

typedef struct
{
    uvm_page_mask_t prefetch_pages_mask;

    uvm_processor_id_t residency;
} uvm_perf_prefetch_hint_t;

// Encapsulates a counter tree built on top of a page mask bitmap in which each
// leaf represents a page in the block. It contains leaf_count and level_count
// so that it can use some macros for perf trees.
typedef struct
{
    uvm_page_mask_t pages;

    uvm_page_index_t offset;

    NvU16 leaf_count;

    NvU8 level_count;
} uvm_perf_prefetch_bitmap_tree_t;

// Iterator for the bitmap tree. It contains level_idx and node_idx so that it
// can use some macros for perf trees.
typedef struct
{
    s8 level_idx;

    uvm_page_index_t node_idx;
} uvm_perf_prefetch_bitmap_tree_iter_t;

// Global initialization function (no clean up needed).
NV_STATUS uvm_perf_prefetch_init(void);

// Returns whether prefetching is enabled in the VA space.
// va_space cannot be NULL.
bool uvm_perf_prefetch_enabled(uvm_va_space_t *va_space);

// Return the prefetch mask with the pages that may be prefetched in a ATS
// block. ATS block is a system allocated memory block with base aligned to
// UVM_VA_BLOCK_SIZE and a maximum size of UVM_VA_BLOCK_SIZE. The faulted_pages
// mask and faulted_region are the pages being faulted on the given residency.
//
// Only residency_mask can be NULL.
//
// Locking: The caller must hold the va_space lock.
void uvm_perf_prefetch_compute_ats(uvm_va_space_t *va_space,
                                   const uvm_page_mask_t *faulted_pages,
                                   uvm_va_block_region_t faulted_region,
                                   uvm_va_block_region_t max_prefetch_region,
                                   const uvm_page_mask_t *residency_mask,
                                   uvm_perf_prefetch_bitmap_tree_t *bitmap_tree,
                                   uvm_page_mask_t *out_prefetch_mask);

// Return a hint with the pages that may be prefetched in the block.
// The faulted_pages mask and faulted_region are the pages being migrated to
// the given residency.
// va_block_context must not be NULL, and if the va_block is a HMM
// block, va_block_context->hmm.vma must be valid which also means the
// va_block_context->mm is not NULL, retained, and locked for at least
// read.
// Locking: The caller must hold the va_space lock and va_block lock.
void uvm_perf_prefetch_get_hint_va_block(uvm_va_block_t *va_block,
                                         uvm_va_block_context_t *va_block_context,
                                         uvm_processor_id_t new_residency,
                                         const uvm_page_mask_t *faulted_pages,
                                         uvm_va_block_region_t faulted_region,
                                         uvm_perf_prefetch_bitmap_tree_t *bitmap_tree,
                                         uvm_perf_prefetch_hint_t *out_hint);

void uvm_perf_prefetch_bitmap_tree_iter_init(const uvm_perf_prefetch_bitmap_tree_t *bitmap_tree,
                                             uvm_page_index_t page_index,
                                             uvm_perf_prefetch_bitmap_tree_iter_t *iter);
uvm_va_block_region_t uvm_perf_prefetch_bitmap_tree_iter_get_range(const uvm_perf_prefetch_bitmap_tree_t *bitmap_tree,
                                                                   const uvm_perf_prefetch_bitmap_tree_iter_t *iter);
NvU16 uvm_perf_prefetch_bitmap_tree_iter_get_count(const uvm_perf_prefetch_bitmap_tree_t *bitmap_tree,
                                                   const uvm_perf_prefetch_bitmap_tree_iter_t *iter);

#define uvm_perf_prefetch_bitmap_tree_traverse_counters(counter,tree,page,iter)                             \
    for (uvm_perf_prefetch_bitmap_tree_iter_init((tree), (page), (iter)),                                   \
         (counter) = uvm_perf_prefetch_bitmap_tree_iter_get_count((tree), (iter));                          \
         (iter)->level_idx >= 0;                                                                            \
         (counter) = --(iter)->level_idx < 0? 0:                                                            \
                                              uvm_perf_prefetch_bitmap_tree_iter_get_count((tree), (iter)))

#endif
