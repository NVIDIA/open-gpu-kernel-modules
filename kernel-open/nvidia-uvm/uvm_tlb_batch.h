/*******************************************************************************
    Copyright (c) 2016-2019 NVIDIA Corporation

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

#ifndef __UVM_TLB_BATCH_H__
#define __UVM_TLB_BATCH_H__

#include "uvm_forward_decl.h"
#include "uvm_hal_types.h"

// Max number of separate VA ranges to track before falling back to invalidate all.
// TLB batches take space on the stack so this number should be big enough to
// cover our common cases, but not bigger.
//
// TODO: Bug 1767241: Once we have all the paths using TLB invalidates
//       implemented, verify whether it makes sense.
#define UVM_TLB_BATCH_MAX_ENTRIES 4

typedef struct
{
    NvU64 start;
    NvU64 size;

    // Min and max page size ored together
    NvU64 page_sizes;
} uvm_tlb_batch_range_t;

struct uvm_tlb_batch_struct
{
    uvm_page_tree_t *tree;

    union
    {
        // Total number of pages covered by the queued up ranges so far
        NvU32 total_pages;

        // Total number of ranges that have been invalidated so far
        // Each range can be invalidated using a single Host method on supported GPUs
        NvU32 total_ranges;
    };

    // Queued up ranges to invalidate
    uvm_tlb_batch_range_t ranges[UVM_TLB_BATCH_MAX_ENTRIES];
    NvU32 count;

    // Biggest page size across all queued up invalidates
    NvU64 biggest_page_size;

    // Max membar across all queued up invalidates
    uvm_membar_t membar;
};

// Begin a TLB invalidate batch
void uvm_tlb_batch_begin(uvm_page_tree_t *tree, uvm_tlb_batch_t *batch);

// Queue up an invalidate of the [start, start + size) range that will invalidate
// all TLB cache entries for all page sizes included in the page sizes mask.
// The smallest page size in the mask affects the density of the per VA TLB
// invalidate (if one ends up being used) and the largest page size affects the
// depth of the issued TLB invalidates.
//
// If the membar parameter is not UVM_MEMBAR_NONE, the specified membar will
// be performed logically after the TLB invalidate such that all physical memory
// accesses using the old translations are ordered to the scope of the membar.
void uvm_tlb_batch_invalidate(uvm_tlb_batch_t *batch, NvU64 start, NvU64 size, NvU64 page_sizes, uvm_membar_t tlb_membar);

// End a TLB invalidate batch
//
// This will push the required TLB invalidate to invalidate all the queued up
// ranges.
//
// The tlb_membar argument has the same behavior as in uvm_tlb_batch_invalidate.
// This allows callers which use the same membar for all calls to
// uvm_tlb_batch_invalidate to pass a single membar once at the end of the
// batch.
void uvm_tlb_batch_end(uvm_tlb_batch_t *batch, uvm_push_t *push, uvm_membar_t tlb_membar);

// Helper for invalidating a single range immediately.
//
// Internally begins and ends a TLB batch.
static void uvm_tlb_batch_single_invalidate(uvm_page_tree_t *tree,
                                            uvm_push_t *push,
                                            NvU64 start,
                                            NvU64 size,
                                            NvU64 page_sizes,
                                            uvm_membar_t tlb_membar)
{
    uvm_tlb_batch_t batch;

    uvm_tlb_batch_begin(tree, &batch);
    uvm_tlb_batch_invalidate(&batch, start, size, page_sizes, UVM_MEMBAR_NONE);
    uvm_tlb_batch_end(&batch, push, tlb_membar);
}

#endif // __UVM_TLB_BATCH_H__
