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

#include "uvm_common.h"
#include "uvm_kvmalloc.h"
#include "uvm_rb_tree.h"
#include "uvm_test.h"
#include "uvm_test_rng.h"

typedef struct
{
    NvU64 key;
    uvm_rb_tree_node_t node;
    struct list_head list;
} rbtt_tree_node_t;

typedef enum
{
    RBTT_OP_ADD,
    RBTT_OP_REMOVE,
    RBTT_OP_COUNT
} rbtt_test_op_t;

typedef struct
{
    uvm_rb_tree_t tree;
    uvm_test_rng_t rng;

    // List of all nodes used for tracking and verification.
    // Nodes in the list are in insertion order.
    struct list_head nodes;
    rbtt_test_op_t preferred_op;
    size_t count;
} rbtt_state_t;

static rbtt_state_t *rbtt_state_create(void)
{
    rbtt_state_t *state = uvm_kvmalloc_zero(sizeof(*state));

    if (!state)
        return NULL;

    INIT_LIST_HEAD(&state->nodes);
    uvm_rb_tree_init(&state->tree);
    return state;
}

static void rbtt_state_destroy(rbtt_state_t *state)
{
    rbtt_tree_node_t *node, *next;

    list_for_each_entry_safe(node, next, &state->nodes, list) {
        list_del(&node->list);
        uvm_kvfree(node);
    }

    uvm_kvfree(state);
}

static NV_STATUS rbtt_check_tree(rbtt_state_t *state)
{
    uvm_rb_tree_node_t *tree_node = NULL;
    uvm_rb_tree_node_t *next;
    rbtt_tree_node_t *node;

    list_for_each_entry(node, &state->nodes, list) {
        tree_node = uvm_rb_tree_find(&state->tree, node->key);
        TEST_CHECK_RET(tree_node);
        TEST_CHECK_RET(tree_node == &node->node);
    }

    // Check tree iterators.
    if (state->count == 0) {
        TEST_CHECK_RET(uvm_rb_tree_empty(&state->tree));
        TEST_CHECK_RET(uvm_rb_tree_first(&state->tree) == NULL);
        uvm_rb_tree_for_each(tree_node, &state->tree)
            TEST_CHECK_RET(0);
        uvm_rb_tree_for_each_safe(tree_node, next, &state->tree)
            TEST_CHECK_RET(0);
    }
    else {
        uvm_rb_tree_node_t *prev = NULL;
        uvm_rb_tree_node_t *curr;
        size_t tree_node_count = 0;

        TEST_CHECK_RET(!uvm_rb_tree_empty(&state->tree));
        curr = uvm_rb_tree_first(&state->tree);
        TEST_CHECK_RET(curr != NULL);

        uvm_rb_tree_for_each(tree_node, &state->tree) {
            TEST_CHECK_RET(curr == tree_node);
            TEST_CHECK_RET(uvm_rb_tree_prev(&state->tree, tree_node) == prev);
            if (prev)
                TEST_CHECK_RET(prev->key < tree_node->key);
            prev = tree_node;
            curr = uvm_rb_tree_next(&state->tree, tree_node);
            tree_node_count++;
        }

        TEST_CHECK_RET(curr == NULL);
        TEST_CHECK_RET(tree_node_count == state->count);

        tree_node_count = 0;
        prev = NULL;
        curr = uvm_rb_tree_first(&state->tree);
        uvm_rb_tree_for_each_safe(tree_node, next, &state->tree) {
            TEST_CHECK_RET(curr == tree_node);
            TEST_CHECK_RET(uvm_rb_tree_prev(&state->tree, tree_node) == prev);
            if (prev)
                TEST_CHECK_RET(prev->key < tree_node->key);
            prev = tree_node;
            curr = uvm_rb_tree_next(&state->tree, tree_node);
            tree_node_count++;
        }

        TEST_CHECK_RET(curr == NULL);
        TEST_CHECK_RET(tree_node_count == state->count);
    }


    return NV_OK;
}

static rbtt_tree_node_t *rbtt_node_alloc(void)
{
    rbtt_tree_node_t *node = uvm_kvmalloc_zero(sizeof(*node));

    if (!node)
        return NULL;

    INIT_LIST_HEAD(&node->list);
    return node;
}

static NV_STATUS rbtt_add_node(rbtt_state_t *state, NvU64 key)
{
    rbtt_tree_node_t *node = rbtt_node_alloc();
    NV_STATUS status;

    if (!node)
        return NV_ERR_NO_MEMORY;

    node->key = key;
    node->node.key = key;

    status = uvm_rb_tree_insert(&state->tree, &node->node);
    if (status == NV_OK) {
        list_add_tail(&node->list, &state->nodes);
        state->count++;
    } else {
        uvm_kvfree(node);
    }

    return status;
}

// This function assumes that node is a valid tree node.
// All validation checks should be done by the caller.
static void rbtt_tree_remove_node(rbtt_state_t *state, rbtt_tree_node_t *node)
{
    uvm_rb_tree_remove(&state->tree, &node->node);
    list_del(&node->list);
    uvm_kvfree(node);
    UVM_ASSERT(state->count > 0);
    state->count--;
}

static NV_STATUS rbtt_tree_remove_by_key(rbtt_state_t *state, NvU64 key)
{
    uvm_rb_tree_node_t *tree_node;
    rbtt_tree_node_t *node;
    bool exists;

    list_for_each_entry(node, &state->nodes, list) {
        if (node->key == key)
            break;
    }

    // If node is equal to the head of the list, there is no node
    // matching key in our the list.
    exists = &node->list != &state->nodes;

    tree_node = uvm_rb_tree_find(&state->tree, key);
    if (exists) {
        TEST_CHECK_RET(tree_node);
        TEST_CHECK_RET(node->key == tree_node->key);
        rbtt_tree_remove_node(state, node);
    }
    else {
        TEST_CHECK_RET(tree_node == NULL);
    }

    return rbtt_check_tree(state);
}

static NV_STATUS rbtt_tree_remove_all(rbtt_state_t *state)
{
    rbtt_tree_node_t *node, *next;

    list_for_each_entry_safe(node, next, &state->nodes, list)
        TEST_NV_CHECK_RET(rbtt_tree_remove_by_key(state, node->key));

    return NV_OK;
}

static NV_STATUS rbtt_test_directed(rbtt_state_t *state)
{
    TEST_CHECK_RET(uvm_rb_tree_empty(&state->tree));
    TEST_CHECK_RET(uvm_rb_tree_find(&state->tree, 0) == NULL);
    TEST_CHECK_RET(uvm_rb_tree_find(&state->tree, ULLONG_MAX) == NULL);
    TEST_CHECK_RET(uvm_rb_tree_first(&state->tree) == NULL);
    TEST_NV_CHECK_RET(rbtt_check_tree(state));

    MEM_NV_CHECK_RET(rbtt_add_node(state, 0), NV_OK);
    TEST_NV_CHECK_RET(rbtt_check_tree(state));
    MEM_NV_CHECK_RET(rbtt_add_node(state, ULLONG_MAX), NV_OK);
    TEST_NV_CHECK_RET(rbtt_check_tree(state));
    MEM_NV_CHECK_RET(rbtt_add_node(state, ULLONG_MAX / 2), NV_OK);
    TEST_NV_CHECK_RET(rbtt_check_tree(state));
    MEM_NV_CHECK_RET(rbtt_add_node(state, 0), NV_ERR_IN_USE);
    MEM_NV_CHECK_RET(rbtt_add_node(state, ULLONG_MAX), NV_ERR_IN_USE);
    MEM_NV_CHECK_RET(rbtt_add_node(state, ULLONG_MAX / 2), NV_ERR_IN_USE);
    TEST_NV_CHECK_RET(rbtt_check_tree(state));

    // Create gaps and exactly fill them.
    MEM_NV_CHECK_RET(rbtt_add_node(state, 2), NV_OK);
    TEST_NV_CHECK_RET(rbtt_check_tree(state));
    MEM_NV_CHECK_RET(rbtt_add_node(state, 4), NV_OK);
    TEST_NV_CHECK_RET(rbtt_check_tree(state));
    MEM_NV_CHECK_RET(rbtt_add_node(state, 1), NV_OK);
    TEST_NV_CHECK_RET(rbtt_check_tree(state));
    MEM_NV_CHECK_RET(rbtt_add_node(state, 3), NV_OK);
    TEST_NV_CHECK_RET(rbtt_check_tree(state));

    TEST_NV_CHECK_RET(rbtt_tree_remove_by_key(state, ULLONG_MAX / 2));
    TEST_NV_CHECK_RET(rbtt_check_tree(state));
    TEST_NV_CHECK_RET(rbtt_tree_remove_all(state));
    TEST_CHECK_RET(uvm_rb_tree_empty(&state->tree));

    return NV_OK;
}

NV_STATUS uvm_test_rb_tree_directed(UVM_TEST_RB_TREE_DIRECTED_PARAMS *params, struct file *filp)
{
    rbtt_state_t *state = rbtt_state_create();
    NV_STATUS status;

    if (!state)
        return NV_ERR_NO_MEMORY;

    status = rbtt_test_directed(state);
    rbtt_state_destroy(state);
    return status;
}

static bool rbtt_test_random_should_fail(rbtt_state_t *state, NvU64 key)
{
    rbtt_tree_node_t *node;
    bool should_fail = NV_FALSE;

    list_for_each_entry(node, &state->nodes, list) {
        if (node->key == key) {
            should_fail = NV_TRUE;
            break;
        }
    }

    return should_fail;
}

static rbtt_tree_node_t *rbtt_test_get_random_node(rbtt_state_t *state)
{
    rbtt_tree_node_t *node;
    size_t index;

    if (!state->count)
        return NULL;

    index = uvm_test_rng_range_ptr(&state->rng, 0, state->count - 1);
    node = list_first_entry(&state->nodes, rbtt_tree_node_t, list);
    while (index--)
        node = list_next_entry(node, list);

    UVM_ASSERT(node);
    return node;
}

static rbtt_test_op_t rbtt_test_get_random_op(rbtt_state_t *state, size_t limit)
{
    // The algorithm is designed to grow the tree until it reaches the
    // limit, then shrink it until it is empty, while still randomizing
    // the operations.

    if (state->count == 0) {
        state->preferred_op = RBTT_OP_ADD;
        return RBTT_OP_ADD;
    }
    else if (state->count == limit) {
        state->preferred_op = RBTT_OP_REMOVE;
        return RBTT_OP_REMOVE;
    }

    if (uvm_test_rng_range_32(&state->rng, 0, 3) == 0) {
        BUILD_BUG_ON((int)RBTT_OP_COUNT != 2);
        return !state->preferred_op;
    }

    return state->preferred_op;
}

static NV_STATUS rbtt_test_random(rbtt_state_t *state, UVM_TEST_RB_TREE_RANDOM_PARAMS *params)
{
    rbtt_tree_node_t *node;
    rbtt_test_op_t op;
    NvU64 i;
    NvU64 key;
    NvU64 key_range_max = params->range_max ? params->range_max : ULLONG_MAX;

    for (i = 0; i < params->iterations; i++) {
        bool should_fail;

        if (fatal_signal_pending(current))
            return NV_ERR_SIGNAL_PENDING;

        op = rbtt_test_get_random_op(state, params->node_limit);
        switch (op) {
            case RBTT_OP_ADD:
                // By using a logarithmic key distribution, we are going to get
                // grouping in the lower ranges of the key space, which increases the
                // chance for collisions.
                key = uvm_test_rng_range_log64(&state->rng, 0, key_range_max);
                should_fail = rbtt_test_random_should_fail(state, key);
                MEM_NV_CHECK_RET(rbtt_add_node(state, key), should_fail ? NV_ERR_IN_USE : NV_OK);
                break;
            case RBTT_OP_REMOVE:
                node = rbtt_test_get_random_node(state);
                if (node)
                    rbtt_tree_remove_node(state, node);
                else
                    TEST_CHECK_RET(state->count == 0);
            default:
                break;
        }

        TEST_NV_CHECK_RET(rbtt_check_tree(state));
    }

    return NV_OK;
}

NV_STATUS uvm_test_rb_tree_random(UVM_TEST_RB_TREE_RANDOM_PARAMS *params, struct file *filp)
{
    rbtt_state_t *state = rbtt_state_create();
    NV_STATUS status;

    if (!state)
        return NV_ERR_NO_MEMORY;

    uvm_test_rng_init(&state->rng, params->seed);
    status = rbtt_test_random(state, params);
    rbtt_state_destroy(state);
    return status;
}
