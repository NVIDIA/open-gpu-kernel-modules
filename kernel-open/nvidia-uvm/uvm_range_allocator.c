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

#include "uvm_range_allocator.h"
#include "uvm_kvmalloc.h"

NV_STATUS uvm_range_allocator_init(NvU64 size, uvm_range_allocator_t *range_allocator)
{
    NV_STATUS status;
    uvm_range_tree_node_t *node;

    uvm_spin_lock_init(&range_allocator->lock, UVM_LOCK_ORDER_LEAF);
    uvm_range_tree_init(&range_allocator->range_tree);

    UVM_ASSERT(size > 0);

    node = uvm_kvmalloc(sizeof(*node));
    if (!node)
        return NV_ERR_NO_MEMORY;

    node->start = 0;
    node->end = size - 1;

    status = uvm_range_tree_add(&range_allocator->range_tree, node);
    UVM_ASSERT(status == NV_OK);

    range_allocator->size = size;

    return NV_OK;
}

void uvm_range_allocator_deinit(uvm_range_allocator_t *range_allocator)
{
    uvm_range_tree_node_t *node;

    node = uvm_range_tree_iter_first(&range_allocator->range_tree, 0, range_allocator->size - 1);
    UVM_ASSERT(node);
    UVM_ASSERT_MSG(node->start == 0 && node->end == range_allocator->size - 1, "start 0x%llx end 0x%llx\n", node->start, node->end);

    // Remove the node for completeness even though after deinit the state of
    // tree doesn't matter anyway.
    uvm_range_tree_remove(&range_allocator->range_tree, node);

    uvm_kvfree(node);
}

NV_STATUS uvm_range_allocator_alloc(uvm_range_allocator_t *range_allocator, NvU64 size, NvU64 alignment, uvm_range_allocation_t *range_alloc)
{
    uvm_range_tree_node_t *node;
    bool found = false;

    UVM_ASSERT(size > 0);

    if (alignment == 0)
        alignment = 1;

    UVM_ASSERT(is_power_of_2(alignment));

    if (size > range_allocator->size)
        return NV_ERR_UVM_ADDRESS_IN_USE;

    // Pre-allocate a tree node as part of the allocation so that freeing the
    // range won't require allocating memory and will always succeed.
    range_alloc->node = uvm_kvmalloc(sizeof(*range_alloc->node));
    if (!range_alloc->node)
        return NV_ERR_NO_MEMORY;

    uvm_spin_lock(&range_allocator->lock);

    // This is a very simple brute force going over all the free ranges in
    // address order and returning the first one that's big enough.
    // This could be improved by e.g. also maintaining a binary tree of free
    // ranges ordered by their size.
    uvm_range_tree_for_each_in(node, &range_allocator->range_tree, 0, range_allocator->size - size) {
        NvU64 aligned_start = UVM_ALIGN_UP(node->start, alignment);
        NvU64 aligned_end = aligned_start + size - 1;

        // Check for overflow of aligned_start and aligned_end
        if (aligned_start < node->start || aligned_end < aligned_start)
            continue;

        // Check whether it fits
        if (aligned_end > node->end)
            continue;

        // The allocation always wastes the [node->start, aligned_start) space,
        // but it's expected that there will always be plenty of free space to
        // allocate from and wasting that space should help avoid fragmentation.

        range_alloc->aligned_start = aligned_start;
        range_alloc->node->start = node->start;
        range_alloc->node->end = aligned_end;

        if (aligned_end < node->end) {
            // Shrink the node if the claimed size is smaller than the node.
            uvm_range_tree_shrink_node(&range_allocator->range_tree, node, aligned_end + 1, node->end);
        }
        else {
            // Otherwise just remove it. The removal is safe to do in the loop
            // over nodes as a break is following immediately.
            UVM_ASSERT(node->end == aligned_end);
            uvm_range_tree_remove(&range_allocator->range_tree, node);
            uvm_kvfree(node);
        }
        found = true;
        break;
    }

    uvm_spin_unlock(&range_allocator->lock);

    if (!found) {
        uvm_kvfree(range_alloc->node);
        range_alloc->node = NULL;
        return NV_ERR_UVM_ADDRESS_IN_USE;
    }

    return NV_OK;
}

void uvm_range_allocator_free(uvm_range_allocator_t *range_allocator, uvm_range_allocation_t *range_alloc)
{
    NV_STATUS status;
    uvm_range_tree_node_t *adjacent_node;

    if (!range_alloc)
        return;

    UVM_ASSERT(range_alloc->node);

    uvm_spin_lock(&range_allocator->lock);

    // Add the pre-allocated free range to the tree
    status = uvm_range_tree_add(&range_allocator->range_tree, range_alloc->node);
    UVM_ASSERT(status == NV_OK);

    // And try merging it with adjacent nodes
    adjacent_node = uvm_range_tree_merge_prev(&range_allocator->range_tree, range_alloc->node);
    if (adjacent_node)
        uvm_kvfree(adjacent_node);

    adjacent_node = uvm_range_tree_merge_next(&range_allocator->range_tree, range_alloc->node);
    if (adjacent_node)
        uvm_kvfree(adjacent_node);

    uvm_spin_unlock(&range_allocator->lock);

    range_alloc->node = NULL;
}
