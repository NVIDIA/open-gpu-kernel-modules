/*******************************************************************************
    Copyright (c) 2015 NVIDIA Corporation

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

#ifndef __UVM_PERF_UTILS_H__
#define __UVM_PERF_UTILS_H__

#include "uvm_common.h"

// Macros to perform increments that saturate at the maximum values allowed by the variable underlying storage
#define UVM_PERF_SATURATING_ADD(counter,value)     \
({                                                 \
    NvU64 expected;                                \
    NvU64 old;                                     \
                                                   \
    old = (counter);                               \
    expected = (NvU64)(counter) + (NvU64)(value);  \
    (counter) += (value);                          \
    if ((counter) != expected || expected < old)   \
        (counter) = -1;                            \
    (counter);                                     \
})

#define UVM_PERF_SATURATING_INC(counter) UVM_PERF_SATURATING_ADD((counter), 1)

// Array-based tree type for fix-sized binary trees. Nodes are stored in a contiguous array, ordered per level (from
// the leaf to the root). These trees are mainly used to keep statistics for memory regions. Stats are updated from a
// leaf node (which typically represents a page) up to the root of the tree (which represents the whole memory region
// tracked by the tree). Thus, statistics are transparently aggregated for different memory region size granularities.
//
// Restrictions: trees of up 63 levels are supported for 64-bit architectures, 31 levels for 32-bit architectures. This
// is because tree iterators use signed identifiers.

typedef struct
{
    // Number of leaves
    size_t leaf_count;

    // Number of node levels in the tree (32/64 maximum, so we can use a single byte)
    u8 level_count;

    // Total number of nodes (leaf + internal)
    size_t node_count;

    // Number of leaves that would make this tree a complete binary tree
    size_t pow2_leaf_count;

    void *nodes;
} uvm_perf_tree_t;

// Tree traversal
//
// Full- and complete-binary trees have properties that enable easy traversal of the tree using simple division and
// multiplication operations. However, forcing trees to be complete could leave to a huge waste of memory (up to 25%
// per tree). Therefore, tree traversals here need to compute the number of elements in the current/lower level before
// moving to the upper/lower level. Level 0 is the root level, (level_count - 1) is the leaves' level. This can be
// easily done with the following computation (see uvm_perf_tree_level_node_count):
//
// (1 << level) - (missing_leaves_to_pow2 >> ((levels - 1) - level))
//
// Once we have the offset to the beginning of the current level, we only need to add the index of the node visited
// within the current level. This is done as follows (see uvm_perf_tree_index_in_level):
//
// node_idx >> ((levels - 1) - level)
//
// We provide a type for tree traversals to allow macros to store the necessary information to transparently perform
// these computations. Thus, an uvm_perf_tree_iter_t object needs to be passed to the tree traversal macros.
typedef struct
{
    // For branch traversals: the index of the origin/destination leaf
    // For complete traversals: the index of the node in the current level
    ssize_t node_idx;

    // Current level in the traversal. Needs to be negative to allow detecting when we are out of bounds
    s8 level_idx;

    // Offset of the current level within the node array
    size_t level_offset;
} uvm_perf_tree_iter_t;

// Tree initialization. Computes the total number of levels and nodes required for the given number of leaf nodes and
// allocates its memory. Nodes' memory is zero-initialized.
//
// Returns NV_OK if initialization succeeded, or
// NV_ERR_NO_MEMORY if node allocation failed
NV_STATUS uvm_perf_tree_init(uvm_perf_tree_t *tree, size_t node_size, size_t leaf_count);

// Tree destruction. It frees the memory used by the nodes
void uvm_perf_tree_destroy(uvm_perf_tree_t *tree);

// Resets the contents of the nodes
void uvm_perf_tree_clear(uvm_perf_tree_t *tree, size_t node_size);

// Initializes the context for a tree traversal from the leaves to the root
static void uvm_perf_tree_init_up_traversal(const uvm_perf_tree_t *tree, uvm_perf_tree_iter_t *iter)
{
    iter->level_idx     = tree->level_count - 1;
    iter->level_offset  = 0;
}

static void uvm_perf_tree_init_up_branch_traversal(const uvm_perf_tree_t *tree, size_t leaf, uvm_perf_tree_iter_t *iter)
{
    uvm_perf_tree_init_up_traversal(tree, iter);

    iter->node_idx = leaf;
}

// Initializes the context for a tree traversal from the root to the leaves
static void uvm_perf_tree_init_down_traversal(const uvm_perf_tree_t *tree, uvm_perf_tree_iter_t *iter)
{
    iter->level_idx     = 0;
    iter->level_offset  = tree->node_count - 1;
}

static void uvm_perf_tree_init_down_branch_traversal(const uvm_perf_tree_t *tree, size_t leaf, uvm_perf_tree_iter_t *iter)
{
    uvm_perf_tree_init_down_traversal(tree, iter);

    iter->node_idx = leaf;
}

// Computes the index of the node visited for the traversal in the current level
static size_t uvm_perf_tree_iter_leaf_to_index_in_level(const uvm_perf_tree_t *tree, const uvm_perf_tree_iter_t *iter)
{
    return iter->node_idx >> ((tree->level_count - 1) - iter->level_idx);
}

// Computes the number of nodes in the given level
static size_t uvm_perf_tree_level_node_count(const uvm_perf_tree_t *tree, s8 level_idx)
{
    size_t level_pow2_node_count;
    size_t level_missing_nodes_to_pow2;

    if ((level_idx < 0) || (level_idx >= tree->level_count))
        return 0;

    level_pow2_node_count       = (size_t)1 << level_idx;
    level_missing_nodes_to_pow2 = (tree->pow2_leaf_count - tree->leaf_count) >> ((tree->level_count - 1) - level_idx);

    return level_pow2_node_count - level_missing_nodes_to_pow2;
}

// Function to compute the range of leaves that lie beneath any of the nodes in the tree.
//
// IMPORTANT: This functions may only be used in branch traversals
#define uvm_perf_tree_iter_max_leaves(tree, iter)                                    \
    ((typeof((tree)->leaf_count))1 << (((tree)->level_count - 1) - (iter)->level_idx))

#define uvm_perf_tree_iter_leaf_range_start(tree, iter)                             \
    UVM_ALIGN_DOWN((iter)->node_idx, uvm_perf_tree_iter_max_leaves((tree), (iter)))

#define uvm_perf_tree_iter_leaf_range(tree, iter)                                                     \
({                                                                                                    \
    typeof((tree)->leaf_count) __range_leaves  = uvm_perf_tree_iter_max_leaves((tree), (iter));       \
    typeof((tree)->leaf_count) __range_start   = uvm_perf_tree_iter_leaf_range_start((tree), (iter)); \
    typeof((tree)->leaf_count) __range_end_max = __range_start + __range_leaves;                      \
    typeof((tree)->leaf_count) __range_end     = min(__range_end_max, (tree)->leaf_count);            \
    __range_end - __range_start;                                                                      \
})

// Obtains the current node pointed by the traversal context when doing a branch traversal
#define UVM_PERF_TREE_ITER_BRANCH_CURRENT(tree,node_type,iter)                                                           \
({                                                                                                                       \
    (iter)->level_idx < 0 || (iter)->level_idx >= ((tree)->level_count) ?                                                \
        NULL:                                                                                                            \
        ((node_type *)(tree)->nodes) + (iter)->level_offset + uvm_perf_tree_iter_leaf_to_index_in_level((tree), (iter)); \
})

// Obtains the current node pointed by the traversal context when doing a full traversal
#define UVM_PERF_TREE_ITER_CURRENT(tree,node_type,iter)                         \
({                                                                              \
    (iter)->level_idx < 0 || (iter)->level_idx >= ((tree)->level_count) ?       \
        NULL:                                                                   \
        ((node_type *)(tree)->nodes) + (iter)->level_offset + (iter)->node_idx; \
})

// Obtains the leaf node corresponding to the given leaf node index
#define UVM_PERF_TREE_LEAF(tree,node_type,leaf_idx) \
({                                                  \
    ((node_type *)(tree)->nodes) + leaf_idx;        \
})

// Obtains the root node of the tree
#define UVM_PERF_TREE_ROOT(tree,node_type)                   \
({                                                           \
    ((node_type *)(tree)->nodes) + ((tree)->node_count - 1); \
})

// Functions to update the tree traversal context with the information of the next level (up/down)
static void uvm_perf_tree_traverse_up(const uvm_perf_tree_t *tree, uvm_perf_tree_iter_t *iter)
{
    // Nodes of the next level (up) are stored AFTER the current level
    iter->level_offset += uvm_perf_tree_level_node_count(tree, iter->level_idx--);
}

static void uvm_perf_tree_traverse_down(const uvm_perf_tree_t *tree, uvm_perf_tree_iter_t *iter)
{
    // Nodes of the next level (down) are stored BEFORE the current level. Since we are at the beginning of the current
    // level, we must skip all the nodes of the NEXT level.
    iter->level_offset -= uvm_perf_tree_level_node_count(tree, ++iter->level_idx);
}

// Complete branch traversal from the given leaf up to the root of the tree. A pointer to the node in each level of the
// traversal is stored in node
#define uvm_perf_tree_traverse_leaf_to_root(tree,leaf,node,iter)                      \
    for (uvm_perf_tree_init_up_branch_traversal((tree), (leaf), (iter)),              \
         (node) = UVM_PERF_TREE_ITER_BRANCH_CURRENT((tree), typeof(*(node)), (iter)); \
         (node) != NULL;                                                              \
         uvm_perf_tree_traverse_up((tree), (iter)),                                   \
         (node) = UVM_PERF_TREE_ITER_BRANCH_CURRENT((tree), typeof(*(node)), (iter)))

// Complete branch traversal from the root of the tree down to the given leaf index. A pointer to the node in each level
// of the traversal is stored in node
#define uvm_perf_tree_traverse_root_to_leaf(tree,leaf,node,iter)                      \
    for (uvm_perf_tree_init_down_branch_traversal((tree), (leaf), (iter)),            \
         (node) = UVM_PERF_TREE_ITER_BRANCH_CURRENT((tree), typeof(*(node)), (iter)); \
         (node) != NULL;                                                              \
         uvm_perf_tree_traverse_down((tree), (iter)),                                 \
         (node) = UVM_PERF_TREE_ITER_BRANCH_CURRENT((tree), typeof(*(node)), (iter)))

// Iterate over all tree levels from root to leaves
#define uvm_perf_tree_for_each_level_down(tree,iter)        \
    for (uvm_perf_tree_init_down_traversal((tree), (iter)); \
         (iter)->level_idx < (tree)->level_count;           \
         uvm_perf_tree_traverse_down((tree), (iter)))

// Iterate over all tree levels from leaves to root
#define uvm_perf_tree_for_each_level_up(tree,iter)        \
    for (uvm_perf_tree_init_up_traversal((tree), (iter)); \
         (iter)->level_idx >= 0;                          \
         uvm_perf_tree_traverse_up((tree), (iter)))

// Iterate over all nodes within a level of the tree (left to right)
#define uvm_perf_tree_level_for_each_node(tree,node,iter)                              \
    for ((iter)->node_idx = 0,                                                         \
         (node) = UVM_PERF_TREE_ITER_CURRENT((tree), typeof(*(node)), (iter));         \
         (iter)->node_idx < uvm_perf_tree_level_node_count((tree), (iter)->level_idx); \
         ++(iter)->node_idx,                                                           \
         ++(node))

// Iterate over all nodes within a level of the tree right to left
#define uvm_perf_tree_level_for_each_node_reverse(tree,node,iter)                          \
    for ((iter)->node_idx = uvm_perf_tree_level_node_count((tree), (iter)->level_idx) - 1, \
         (node) = UVM_PERF_TREE_ITER_CURRENT((tree), typeof(*(node)), (iter));             \
         (iter)->node_idx >= 0;                                                            \
         --(iter)->node_idx,                                                               \
         --(node))

#endif
