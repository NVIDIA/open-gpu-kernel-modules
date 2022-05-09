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

#include "uvm_rb_tree.h"

static uvm_rb_tree_node_t *get_uvm_rb_tree_node(struct rb_node *rb_node)
{
    return rb_entry(rb_node, uvm_rb_tree_node_t, rb_node);
}

static uvm_rb_tree_node_t *uvm_rb_tree_find_node(uvm_rb_tree_t *tree,
                                                 NvU64 key,
                                                 uvm_rb_tree_node_t **parent,
                                                 uvm_rb_tree_node_t **next)
{
    struct rb_node *rb_node = tree->rb_root.rb_node;
    uvm_rb_tree_node_t *node = NULL;
    uvm_rb_tree_node_t *_parent = NULL;

    while (rb_node) {
        node = get_uvm_rb_tree_node(rb_node);

        if (key < node->key)
            rb_node = rb_node->rb_left;
        else if (key > node->key)
            rb_node = rb_node->rb_right;
        else
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
            *next = uvm_rb_tree_next(tree, node);
        }
        else if (_parent) {
            if (_parent->key > key)
                *next = _parent;
            else
                *next = uvm_rb_tree_next(tree, _parent);
        }
    }

    return node;
}

void uvm_rb_tree_init(uvm_rb_tree_t *tree)
{
    memset(tree, 0, sizeof(*tree));
    tree->rb_root = RB_ROOT;
    INIT_LIST_HEAD(&tree->head);
}

NV_STATUS uvm_rb_tree_insert(uvm_rb_tree_t *tree, uvm_rb_tree_node_t *node)
{
    uvm_rb_tree_node_t *match, *parent;

    match = uvm_rb_tree_find_node(tree, node->key, &parent, NULL);
    if (match)
        return NV_ERR_IN_USE;

    // If there's no parent and we didn't match on the root node, the tree is
    // empty.
    if (!parent) {
        rb_link_node(&node->rb_node, NULL, &tree->rb_root.rb_node);
        rb_insert_color(&node->rb_node, &tree->rb_root);
        list_add(&node->list, &tree->head);
        return NV_OK;
    }

    if (node->key < parent->key) {
        rb_link_node(&node->rb_node, &parent->rb_node, &parent->rb_node.rb_left);
        list_add_tail(&node->list, &parent->list);
    }
    else {
        rb_link_node(&node->rb_node, &parent->rb_node, &parent->rb_node.rb_right);
        list_add(&node->list, &parent->list);
    }

    rb_insert_color(&node->rb_node, &tree->rb_root);
    return NV_OK;
}

uvm_rb_tree_node_t *uvm_rb_tree_find(uvm_rb_tree_t *tree, NvU64 key)
{
    return uvm_rb_tree_find_node(tree, key, NULL, NULL);
}
