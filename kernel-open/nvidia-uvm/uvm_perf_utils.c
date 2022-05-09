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

#include "uvm_common.h"
#include "uvm_linux.h"
#include "uvm_perf_utils.h"
#include "uvm_kvmalloc.h"

static inline size_t leaves_to_levels(size_t leaf_count)
{
    return ilog2(roundup_pow_of_two(leaf_count)) + 1;
}

// Helper function to compute all the nodes required to store a complete binary tree for the given number of leaves
static inline size_t leaves_to_nodes(size_t leaf_count)
{
    size_t ret = 0;
    do {
        ret += leaf_count;
        leaf_count = (leaf_count == 1)? 0 : (leaf_count + 1) / 2;
    } while (leaf_count > 0);

    return ret;
}

NV_STATUS uvm_perf_tree_init(uvm_perf_tree_t *tree, size_t node_size, size_t leaf_count)
{
    NV_STATUS status;
    size_t bytes;

    status = NV_OK;

    tree->leaf_count      = leaf_count;
    tree->level_count     = leaves_to_levels(leaf_count);
    tree->node_count      = leaves_to_nodes(leaf_count);
    tree->pow2_leaf_count = roundup_pow_of_two(tree->leaf_count);

    bytes = tree->node_count * node_size;

    // With this check we make sure that our shift operations will not overflow
    UVM_ASSERT(tree->level_count <= (sizeof(size_t) * 8 - 1));
    tree->nodes = uvm_kvmalloc_zero(bytes);
    if (!tree->nodes)
        status = NV_ERR_NO_MEMORY;
    return status;
}

void uvm_perf_tree_destroy(uvm_perf_tree_t *tree)
{
    UVM_ASSERT(tree);
    UVM_ASSERT(tree->nodes);

    uvm_kvfree(tree->nodes);
    tree->leaf_count = 0;
    tree->nodes = NULL;
}

void uvm_perf_tree_clear(uvm_perf_tree_t *tree, size_t node_size)
{
    UVM_ASSERT(tree);
    UVM_ASSERT(tree->nodes);

    memset(tree->nodes, 0, tree->node_count * node_size);
}
