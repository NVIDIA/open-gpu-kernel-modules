/*******************************************************************************
    Copyright (c) 2015-2019 NVIDIA Corporation

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
#include "uvm_range_tree.h"

static uvm_range_tree_node_t *get_range_node(struct rb_node *rb_node)
{
    return rb_entry(rb_node, uvm_range_tree_node_t, rb_node);
}

static bool range_nodes_overlap(uvm_range_tree_node_t *a, uvm_range_tree_node_t *b)
{
    return uvm_ranges_overlap(a->start, a->end, b->start, b->end);
}

// Workhorse tree walking function.
//
// The parent and next pointers may be NULL if the caller doesn't need them.
// They facilitate node addition and range-based searches.
//
// If a node contains addr:
//  - That node is returned
//  - The parent pointer is set to node's parent, or to NULL if the node is the
//    root.
//  - The next pointer is set to the next node in address order in the tree, or
//    to NULL if node is the last node in the tree.
//
// If no node contains addr:
// - NULL is returned
// - The parent pointer is set to the node under which a new node containing
//   addr should be inserted. This will be NULL if the tree is empty.
// - The next pointer is set to the first node containing an address > addr, or
//   NULL if there are no such nodes in the tree.
static uvm_range_tree_node_t *range_node_find(uvm_range_tree_t *tree,
                                              NvU64 addr,
                                              uvm_range_tree_node_t **parent,
                                              uvm_range_tree_node_t **next)
{
    struct rb_node *rb_node = tree->rb_root.rb_node;
    uvm_range_tree_node_t *node = NULL;
    uvm_range_tree_node_t *_parent = NULL;

    while (rb_node) {
        node = get_range_node(rb_node);

        if (addr < node->start)
            rb_node = rb_node->rb_left;
        else if (addr > node->end)
            rb_node = rb_node->rb_right;
        else // node contains addr
            break;

        _parent = node;
    }

    if (!rb_node)
        node = NULL;

    if (parent)
        *parent = _parent;
    if (next) {
        *next = NULL; // Handles the empty tree case
        if (node) {
            *next = uvm_range_tree_next(tree, node);
        }
        else if (_parent) {
            if (_parent->start > addr)
                *next = _parent;
            else
                *next = uvm_range_tree_next(tree, _parent);
        }
    }

    return node;
}

void uvm_range_tree_init(uvm_range_tree_t *tree)
{
    memset(tree, 0, sizeof(*tree));
    tree->rb_root = RB_ROOT;
    INIT_LIST_HEAD(&tree->head);
}

NV_STATUS uvm_range_tree_add(uvm_range_tree_t *tree, uvm_range_tree_node_t *node)
{
    uvm_range_tree_node_t *match, *parent, *prev, *next;

    UVM_ASSERT(node->start <= node->end);

    match = range_node_find(tree, node->start, &parent, NULL);
    if (match)
        return NV_ERR_UVM_ADDRESS_IN_USE;

    // If no match we know that the new start isn't contained in any existing
    // node, but we still have to check for overlap on the rest of the new range.

    // If there's no parent and we didn't match on the root node, the tree is
    // empty.
    if (!parent) {
        rb_link_node(&node->rb_node, NULL, &tree->rb_root.rb_node);
        rb_insert_color(&node->rb_node, &tree->rb_root);
        list_add(&node->list, &tree->head);
        return NV_OK;
    }

    // We know that start isn't contained in parent, but the rest of the new
    // range might be.
    if (range_nodes_overlap(node, parent))
        return NV_ERR_UVM_ADDRESS_IN_USE;

    // Verify that the new node doesn't overlap with its neighbor and insert
    if (node->start < parent->start) {
        // parent's prev can't overlap with node, otherwise it must overlap with
        // start and would've been found by range_node_find above.
        prev = uvm_range_tree_prev(tree, parent);
        if (prev)
            UVM_ASSERT(!range_nodes_overlap(node, prev));

        rb_link_node(&node->rb_node, &parent->rb_node, &parent->rb_node.rb_left);
        list_add_tail(&node->list, &parent->list);
    }
    else {
        next = uvm_range_tree_next(tree, parent);
        if (next && range_nodes_overlap(node, next))
            return NV_ERR_UVM_ADDRESS_IN_USE;

        rb_link_node(&node->rb_node, &parent->rb_node, &parent->rb_node.rb_right);
        list_add(&node->list, &parent->list);
    }

    rb_insert_color(&node->rb_node, &tree->rb_root);
    return NV_OK;
}

void uvm_range_tree_shrink_node(uvm_range_tree_t *tree, uvm_range_tree_node_t *node, NvU64 new_start, NvU64 new_end)
{
    UVM_ASSERT_MSG(new_start <= new_end, "new_start 0x%llx new_end 0x%llx\n", new_start, new_end);
    UVM_ASSERT_MSG(node->start <= new_start, "start 0x%llx new_start 0x%llx\n", node->start, new_start);
    UVM_ASSERT_MSG(node->end >= new_end, "end 0x%llx new_end 0x%llx\n", node->end, new_end);

    // The tree is not needed currently, but might be in the future.
    (void)tree;

    node->start = new_start;
    node->end = new_end;
}

void uvm_range_tree_split(uvm_range_tree_t *tree,
                          uvm_range_tree_node_t *existing,
                          uvm_range_tree_node_t *new)
{
    NV_STATUS status;

    UVM_ASSERT(new->start > existing->start);
    UVM_ASSERT(new->start <= existing->end);

    // existing doesn't have to move anywhere, we just need to adjust its
    // ranges. new will need to be inserted into the tree.
    //
    // Future optimization: insertion could walk down the tree starting from
    // existing rather than from the root.
    new->end = existing->end;
    existing->end = new->start - 1;
    status = uvm_range_tree_add(tree, new);
    UVM_ASSERT(status == NV_OK); // There shouldn't be any collisions
}

uvm_range_tree_node_t *uvm_range_tree_merge_prev(uvm_range_tree_t *tree, uvm_range_tree_node_t *node)
{
    uvm_range_tree_node_t *prev = uvm_range_tree_prev(tree, node);
    if (!prev || prev->end != node->start - 1)
        return NULL;

    uvm_range_tree_remove(tree, prev);
    node->start = prev->start;
    return prev;
}

uvm_range_tree_node_t *uvm_range_tree_merge_next(uvm_range_tree_t *tree, uvm_range_tree_node_t *node)
{
    uvm_range_tree_node_t *next = uvm_range_tree_next(tree, node);
    if (!next || next->start != node->end + 1)
        return NULL;

    uvm_range_tree_remove(tree, next);
    node->end = next->end;
    return next;
}

uvm_range_tree_node_t *uvm_range_tree_find(uvm_range_tree_t *tree, NvU64 addr)
{
    return range_node_find(tree, addr, NULL, NULL);
}

uvm_range_tree_node_t *uvm_range_tree_iter_first(uvm_range_tree_t *tree, NvU64 start, NvU64 end)
{
    uvm_range_tree_node_t *node, *next;

    UVM_ASSERT(start <= end);

    node = range_node_find(tree, start, NULL, &next);
    if (node)
        return node;

    // We didn't find a node containing start itself. Check if the target range
    // overlaps with the next node after start.
    if (next) {
        // Sanity checks
        UVM_ASSERT(start < next->start);
        if (uvm_range_tree_prev(tree, next))
            UVM_ASSERT(uvm_range_tree_prev(tree, next)->end < start);

        if (next->start <= end)
            return next;
    }

    return NULL;
}

NV_STATUS uvm_range_tree_find_hole(uvm_range_tree_t *tree, NvU64 addr, NvU64 *start, NvU64 *end)
{
    uvm_range_tree_node_t *node;

    // Find the first node on or after addr, if any
    node = uvm_range_tree_iter_first(tree, addr, ULLONG_MAX);
    if (node) {
        if (node->start <= addr)
            return NV_ERR_UVM_ADDRESS_IN_USE;

        // node->start can't be 0, otherwise it would contain addr
        if (end)
            *end = node->start - 1;

        node = uvm_range_tree_prev(tree, node);
    }
    else {
        // All nodes in the tree must come before addr, if any exist
        node = uvm_range_tree_last(tree);
        if (end)
            *end = ULLONG_MAX;
    }

    if (start) {
        if (node)
            *start = node->end + 1;
        else
            *start = 0;
    }

    return NV_OK;
}

NV_STATUS uvm_range_tree_find_hole_in(uvm_range_tree_t *tree, NvU64 addr, NvU64 *start, NvU64 *end)
{
    NvU64 temp_start, temp_end;
    NV_STATUS status;

    UVM_ASSERT(start);
    UVM_ASSERT(end);
    UVM_ASSERT(*start <= addr);
    UVM_ASSERT(*end >= addr);

    status = uvm_range_tree_find_hole(tree, addr, &temp_start, &temp_end);
    if (status == NV_OK) {
        *start = max(temp_start, *start);
        *end = min(temp_end, *end);
    }

    return status;
}
