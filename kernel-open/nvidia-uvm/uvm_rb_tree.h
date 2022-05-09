/*******************************************************************************
    Copyright (c) 2020 NVIDIA Corporation

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

#ifndef __UVM_RB_TREE_H__
#define __UVM_RB_TREE_H__

#include "nvtypes.h"
#include "nvstatus.h"
#include <linux/rbtree.h>
#include <linux/list.h>
#include <linux/string.h>
#include "nv-list-helpers.h"

// UVM RB trees are an implementation of Red-Black trees, which include some
// optimizations for fast iteration over the elements in the tree.
//
// This implementation requires unique 64-bit keys.
//
// All locking is up to the caller.

typedef struct
{
    NvU64 key;

    struct rb_node rb_node;
    struct list_head list;
} uvm_rb_tree_node_t;

typedef struct
{
    // Tree of uvm_rb_tree_node_t's sorted by key.
    struct rb_root rb_root;

    // List of uvm_rb_tree_node_t's sorted by key. This is an optimization
    // to avoid calling rb_next and rb_prev frequently, particularly while
    // iterating.
    struct list_head head;

} uvm_rb_tree_t;

#define UVM_RB_TREE_CLEAR_NODE(node) RB_CLEAR_NODE(&(node)->rb_node)
#define UVM_RB_TREE_EMPTY_NODE(node) RB_EMPTY_NODE(&(node)->rb_node)

// Initialize a UVM RB Tree.
void uvm_rb_tree_init(uvm_rb_tree_t *tree);

// Insert a node into the tree. node->key should be set prior to calling this
// function.
// If a node with a matching key exists, NV_ERR_IN_USE is returned.
NV_STATUS uvm_rb_tree_insert(uvm_rb_tree_t *tree, uvm_rb_tree_node_t *node);

static void uvm_rb_tree_remove(uvm_rb_tree_t *tree, uvm_rb_tree_node_t *node)
{
    rb_erase(&node->rb_node, &tree->rb_root);
    list_del(&node->list);
}

// Return node matching key, if any.
uvm_rb_tree_node_t *uvm_rb_tree_find(uvm_rb_tree_t *tree, NvU64 key);

static uvm_rb_tree_node_t *uvm_rb_tree_first(uvm_rb_tree_t *tree)
{
    return list_first_entry_or_null(&tree->head, uvm_rb_tree_node_t, list);
}

// Returns the prev/next node in key order, or NULL if none exists
static uvm_rb_tree_node_t *uvm_rb_tree_prev(uvm_rb_tree_t *tree, uvm_rb_tree_node_t *node)
{
    if (list_is_first(&node->list, &tree->head))
        return NULL;
    return list_prev_entry(node, list);
}

static uvm_rb_tree_node_t *uvm_rb_tree_next(uvm_rb_tree_t *tree, uvm_rb_tree_node_t *node)
{
    if (list_is_last(&node->list, &tree->head))
        return NULL;
    return list_next_entry(node, list);
}

// Return true if the range tree is empty.
static bool uvm_rb_tree_empty(uvm_rb_tree_t *tree)
{
    return list_empty(&tree->head);
}

#define uvm_rb_tree_for_each(node, tree)  list_for_each_entry((node), &(tree)->head, list)

#define uvm_rb_tree_for_each_safe(node, next, tree) list_for_each_entry_safe((node), (next), &(tree)->head, list)

#endif // __UVM_RB_TREE_H__
