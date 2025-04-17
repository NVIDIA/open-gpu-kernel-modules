/*******************************************************************************
    Copyright (c) 2016 NVIDIA Corporation

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

#include "uvm_tlb_batch.h"
#include "uvm_hal.h"

void uvm_tlb_batch_begin(uvm_page_tree_t *tree, uvm_tlb_batch_t *batch)
{
    memset(batch, 0, sizeof(*batch));
    batch->tree = tree;
}

static NvU64 smallest_page_size(NvU64 page_sizes)
{
    UVM_ASSERT(page_sizes != 0);

    return 1ULL << __ffs(page_sizes);
}

static NvU64 biggest_page_size(NvU64 page_sizes)
{
    UVM_ASSERT(page_sizes != 0);

    return 1ULL << __fls(page_sizes);
}

static void tlb_batch_flush_invalidate_per_va(uvm_tlb_batch_t *batch, uvm_push_t *push)
{
    uvm_page_tree_t *tree = batch->tree;
    uvm_gpu_phys_address_t pdb_addr = uvm_page_tree_pdb_address(tree);
    uvm_membar_t membar = UVM_MEMBAR_NONE;
    NvU32 i;

    for (i = 0; i < batch->count; ++i) {
        uvm_tlb_batch_range_t *entry = &batch->ranges[i];
        NvU64 min_page_size = smallest_page_size(entry->page_sizes);
        NvU64 max_page_size = biggest_page_size(entry->page_sizes);

        // Use the depth of the max page size as it's the broadest
        NvU32 depth = tree->hal->page_table_depth(max_page_size);

        UVM_ASSERT(hweight64(entry->page_sizes) > 0);

        // Do the required membar only after the last invalidate
        if (i == batch->count - 1)
            membar = batch->membar;

        // Use the min page size for the targeted VA invalidate as each page
        // needs to be invalidated separately.
        tree->gpu->parent->host_hal->tlb_invalidate_va(push,
                                                       pdb_addr,
                                                       depth,
                                                       entry->start,
                                                       entry->size,
                                                       min_page_size,
                                                       membar);
    }
}

static void tlb_batch_flush_invalidate_all(uvm_tlb_batch_t *batch, uvm_push_t *push)
{
    uvm_page_tree_t *tree = batch->tree;
    uvm_gpu_t *gpu = tree->gpu;
    NvU32 page_table_depth = tree->hal->page_table_depth(batch->biggest_page_size);

    gpu->parent->host_hal->tlb_invalidate_all(push, uvm_page_tree_pdb_address(tree), page_table_depth, batch->membar);
}

static bool tlb_batch_should_invalidate_all(uvm_tlb_batch_t *batch)
{
    if (!batch->tree->gpu->parent->tlb_batch.va_invalidate_supported)
        return true;

    if (batch->count > UVM_TLB_BATCH_MAX_ENTRIES)
        return true;

    if (batch->tree->gpu->parent->tlb_batch.va_range_invalidate_supported)
        return batch->total_ranges > batch->tree->gpu->parent->tlb_batch.max_ranges;

    return batch->total_pages > batch->tree->gpu->parent->tlb_batch.max_pages;
}

void uvm_tlb_batch_end(uvm_tlb_batch_t *batch, uvm_push_t *push, uvm_membar_t tlb_membar)
{
    if (batch->count == 0)
        return;

    batch->membar = uvm_membar_max(tlb_membar, batch->membar);

    if (tlb_batch_should_invalidate_all(batch))
        tlb_batch_flush_invalidate_all(batch, push);
    else
        tlb_batch_flush_invalidate_per_va(batch, push);
}

void uvm_tlb_batch_invalidate(uvm_tlb_batch_t *batch, NvU64 start, NvU64 size, NvU64 page_sizes, uvm_membar_t tlb_membar)
{
    uvm_tlb_batch_range_t *new_entry;

    batch->membar = uvm_membar_max(tlb_membar, batch->membar);

    ++batch->count;

    if (batch->tree->gpu->parent->tlb_batch.va_range_invalidate_supported)
        batch->total_ranges++;
    else
        batch->total_pages += uvm_div_pow2_64(size, smallest_page_size(page_sizes));

    batch->biggest_page_size = max(batch->biggest_page_size, biggest_page_size(page_sizes));

    if (tlb_batch_should_invalidate_all(batch))
        return;

    new_entry = &batch->ranges[batch->count - 1];
    new_entry->start = start;
    new_entry->size = size;
    new_entry->page_sizes = page_sizes;
}
