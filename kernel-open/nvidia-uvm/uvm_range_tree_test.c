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

#include "uvm_common.h"
#include "uvm_range_tree.h"
#include "uvm_kvmalloc.h"

#include "uvm_test.h"
#include "uvm_test_ioctl.h"
#include "uvm_test_rng.h"

// ------------------- Range Tree Test (RTT) ------------------- //

// Arbitrary value, must be >= 1
#define MAX_NODES_INIT 32

typedef enum
{
    RTT_OP_ADD,
    RTT_OP_REMOVE,
    RTT_OP_SPLIT,
    RTT_OP_MERGE,
    RTT_OP_SHRINK,
    RTT_OP_MAX
} rtt_op_t;

// Range Tree Test state
typedef struct rtt_state_struct
{
    uvm_range_tree_t tree;
    uvm_test_rng_t rng;

    // Array of allocated nodes, unsorted
    uvm_range_tree_node_t **nodes;

    // Number of nodes in the array
    size_t count;

    // Number of nodes which can fit in the nodes array
    size_t max;

    // The probability of shrinking a node instead of doing an add or remove
    NvU32 shrink_probability;

    // The current probability of selecting an add operation over a remove
    NvU32 add_chance;

    // The current probability of selecting a split operation over a merge
    NvU32 split_chance;

    // For debug
    struct
    {
        // The sum of all ranges currently in the tree
        NvU64 size_sum;

        NvU64 total_adds;
        NvU64 failed_adds;
        NvU64 max_attempts_add;
        NvU64 total_removes;
        NvU64 total_shrinks;
        NvU64 failed_shrinks;
        NvU64 total_splits;
        NvU64 failed_splits;
        NvU64 max_attempts_split;
        NvU64 total_merges;
        NvU64 failed_merges;
        NvU64 max_attempts_merge;
    } stats;

} rtt_state_t;

typedef struct
{
    // end is inclusive
    NvU64 start;
    NvU64 end;
} rtt_range_t;

static rtt_range_t rtt_node_get_range(uvm_range_tree_node_t *node)
{
    rtt_range_t range = {node->start, node->end};
    return range;
}

// Since end is inclusive a range can't have a size of 0. A return value of 0
// means that the range is 2^64.
static NvU64 rtt_get_range_size(rtt_range_t *range)
{
    return range->end - range->start + 1;
}

static bool rtt_ranges_overlap(rtt_range_t *a, rtt_range_t *b)
{
    return uvm_ranges_overlap(a->start, a->end, b->start, b->end);
}

static bool rtt_range_overlaps_node(uvm_range_tree_node_t *node, rtt_range_t *range)
{
    rtt_range_t temp = rtt_node_get_range(node);
    return rtt_ranges_overlap(&temp, range);
}

static void rtt_state_destroy(rtt_state_t *state)
{
    size_t i;

    if (!state)
        return;

    for (i = 0; i < state->count; i++)
        uvm_kvfree(state->nodes[i]);

    uvm_kvfree(state->nodes);
    uvm_kvfree(state);
}

static rtt_state_t *rtt_state_create(void)
{
    rtt_state_t *state = uvm_kvmalloc_zero(sizeof(*state));
    if (!state)
        return NULL;

    state->max = MAX_NODES_INIT;
    state->nodes = uvm_kvmalloc(state->max * sizeof(state->nodes[0]));
    if (!state->nodes) {
        uvm_kvfree(state);
        return NULL;
    }

    uvm_range_tree_init(&state->tree);
    return state;
}

static uvm_range_tree_node_t *rtt_alloc_node(rtt_state_t *state)
{
    uvm_range_tree_node_t *node;
    uvm_range_tree_node_t **new_nodes;
    size_t new_max;

    node = uvm_kvmalloc_zero(sizeof(*node));
    if (!node)
        goto error;

    // Grow the nodes array if we're full. Do this here rather than when adding
    // to the nodes array because this happens before the tree is modified.
    // Recovering from a failure on adding the node to the array requires the
    // caller to undo tree operations, possibly before we've tested that they
    // work.
    //
    // Doing this frequently won't get into a thrashing state since max never
    // shrinks.
    if (state->count == state->max) {
        new_max = max((size_t)1, 2*state->max);

        new_nodes = uvm_kvrealloc(state->nodes, new_max * sizeof(state->nodes[0]));
        if (!new_nodes)
            goto error;
        state->nodes = new_nodes;
        state->max = new_max;
    }

    return node;

error:
    uvm_kvfree(node);
    return NULL;
}

static NV_STATUS rtt_range_add(rtt_state_t *state, rtt_range_t *range, uvm_range_tree_node_t **new_node)
{
    NV_STATUS status;
    uvm_range_tree_node_t *node;

    node = rtt_alloc_node(state);
    if (!node) {
        status = NV_ERR_NO_MEMORY;
        goto error;
    }

    // Attempt insertion into the tree itself
    node->start = range->start;
    node->end   = range->end;
    status = uvm_range_tree_add(&state->tree, node);
    if (status != NV_OK)
        goto error;

    if (uvm_range_tree_node_size(node) != rtt_get_range_size(range)) {
        uvm_range_tree_remove(&state->tree, node);
        status = NV_ERR_INVALID_STATE;
        goto error;
    }

    UVM_ASSERT(state->count < state->max); // Forced by rtt_alloc_node
    state->nodes[state->count] = node;
    ++state->count;
    state->stats.size_sum += rtt_get_range_size(range);
    ++state->stats.total_adds;

    if (new_node)
        *new_node = node;

    return NV_OK;

error:
    uvm_kvfree(node);
    return status;
}

static NV_STATUS rtt_index_remove(rtt_state_t *state, size_t index)
{
    uvm_range_tree_node_t *node;
    NvU64 size;

    TEST_CHECK_RET(state->count > 0);

    node = state->nodes[index];
    size = uvm_range_tree_node_size(node);
    uvm_range_tree_remove(&state->tree, node);
    uvm_kvfree(node);

    // We don't care about ordering so move the last node into the free slot
    --state->count;
    state->nodes[index] = state->nodes[state->count];
    state->stats.size_sum -= size;
    ++state->stats.total_removes;

    return NV_OK;
}

static NV_STATUS rtt_node_shrink(rtt_state_t *state, uvm_range_tree_node_t *node, NvU64 new_start, NvU64 new_end)
{
    NvU64 old_size;
    NvU64 new_size;

    TEST_CHECK_RET(new_start >= node->start);
    TEST_CHECK_RET(new_end <= node->end);

    old_size = uvm_range_tree_node_size(node);
    new_size = new_end - new_start + 1;

    uvm_range_tree_shrink_node(&state->tree, node, new_start, new_end);

    ++state->stats.total_shrinks;
    state->stats.size_sum -= (old_size - new_size);

    return NV_OK;
}

static NV_STATUS rtt_node_split(rtt_state_t *state,
                                uvm_range_tree_node_t *node,
                                NvU64 new_end,
                                uvm_range_tree_node_t **new_node)
{
    NV_STATUS status;
    uvm_range_tree_node_t *new;

    TEST_CHECK_RET(new_end >= node->start);
    TEST_CHECK_RET(new_end <  node->end);

    new = rtt_alloc_node(state);
    if (!new ) {
        status = NV_ERR_NO_MEMORY;
        goto error;
    }

    new->start = new_end + 1;
    uvm_range_tree_split(&state->tree, node, new);

    UVM_ASSERT(state->count < state->max); // Forced by rtt_alloc_node
    state->nodes[state->count] = new;
    ++state->count;
    // No changes needed to size_sum
    ++state->stats.total_splits;

    if (new_node)
        *new_node = new;

    return NV_OK;

error:
    uvm_kvfree(new);
    return status;
}

static NV_STATUS rtt_check_between(rtt_state_t *state, uvm_range_tree_node_t *lower, uvm_range_tree_node_t *upper)
{
    bool hole_exists = true;
    NvU64 hole_start = 0, hole_end = ULLONG_MAX;
    NvU64 test_start, test_end;

    if (lower) {
        if (lower->end == ULLONG_MAX) {
            UVM_ASSERT(!upper);
            hole_exists = false;
        }
        else {
            hole_start = lower->end + 1;
        }
    }

    if (upper) {
        if (upper->start == 0) {
            UVM_ASSERT(!lower);
            hole_exists = false;
        }
        else {
            hole_end = upper->start - 1;
        }
    }

    if (hole_start > hole_end)
        hole_exists = false;

    if (hole_exists) {
        size_t i;
        NvU64 hole_mid = hole_start + ((hole_end - hole_start) / 2);
        NvU64 inputs[] = {hole_start, hole_mid, hole_end};

        for (i = 0; i < ARRAY_SIZE(inputs); i++) {
            TEST_CHECK_RET(uvm_range_tree_find(&state->tree, inputs[i]) == NULL);

            TEST_NV_CHECK_RET(uvm_range_tree_find_hole(&state->tree, inputs[i], &test_start, &test_end));
            TEST_CHECK_RET(test_start == hole_start);
            TEST_CHECK_RET(test_end == hole_end);

            test_start = 0;
            test_end = ULLONG_MAX;
            TEST_NV_CHECK_RET(uvm_range_tree_find_hole_in(&state->tree, inputs[i], &test_start, &test_end));
            TEST_CHECK_RET(test_start == hole_start);
            TEST_CHECK_RET(test_end == hole_end);

            test_start = hole_start;
            test_end = inputs[i];
            TEST_NV_CHECK_RET(uvm_range_tree_find_hole_in(&state->tree, inputs[i], &test_start, &test_end));
            TEST_CHECK_RET(test_start == hole_start);
            TEST_CHECK_RET(test_end == inputs[i]);

            test_start = inputs[i];
            test_end = hole_end;
            TEST_NV_CHECK_RET(uvm_range_tree_find_hole_in(&state->tree, inputs[i], &test_start, &test_end));
            TEST_CHECK_RET(test_start == inputs[i]);
            TEST_CHECK_RET(test_end == hole_end);
        }
    }
    else {
        test_start = 0;
        test_end = ULLONG_MAX;

        if (lower) {
            MEM_NV_CHECK_RET(uvm_range_tree_find_hole(&state->tree, lower->end, NULL, NULL),
                             NV_ERR_UVM_ADDRESS_IN_USE);
            MEM_NV_CHECK_RET(uvm_range_tree_find_hole_in(&state->tree, lower->end, &test_start, &test_end),
                             NV_ERR_UVM_ADDRESS_IN_USE);
        }

        if (upper) {
            MEM_NV_CHECK_RET(uvm_range_tree_find_hole(&state->tree, upper->start, NULL, NULL),
                             NV_ERR_UVM_ADDRESS_IN_USE);
            MEM_NV_CHECK_RET(uvm_range_tree_find_hole_in(&state->tree, upper->start, &test_start, &test_end),
                             NV_ERR_UVM_ADDRESS_IN_USE);
        }
    }

    return NV_OK;
}

static NV_STATUS rtt_check_node(rtt_state_t *state, uvm_range_tree_node_t *node)
{
    uvm_range_tree_node_t *temp, *prev, *next;
    NvU64 start, mid, end;
    NvU64 hole_start = 0, hole_end = ULLONG_MAX;

    start = node->start;
    end   = node->end;
    mid   = start + ((end - start) / 2);

    TEST_CHECK_RET(!uvm_range_tree_empty(&state->tree));

    if (start > 0)
        TEST_CHECK_RET(uvm_range_tree_find(&state->tree, start - 1) != node);

    TEST_CHECK_RET(uvm_range_tree_find(&state->tree, start) == node);
    TEST_CHECK_RET(uvm_range_tree_find(&state->tree, mid)   == node);
    TEST_CHECK_RET(uvm_range_tree_find(&state->tree, end)   == node);

    MEM_NV_CHECK_RET(uvm_range_tree_find_hole(&state->tree, start, NULL, NULL), NV_ERR_UVM_ADDRESS_IN_USE);
    MEM_NV_CHECK_RET(uvm_range_tree_find_hole(&state->tree, mid, NULL, NULL), NV_ERR_UVM_ADDRESS_IN_USE);
    MEM_NV_CHECK_RET(uvm_range_tree_find_hole(&state->tree, end, NULL, NULL), NV_ERR_UVM_ADDRESS_IN_USE);

    MEM_NV_CHECK_RET(uvm_range_tree_find_hole_in(&state->tree, start, &hole_start, &hole_end),
                     NV_ERR_UVM_ADDRESS_IN_USE);
    MEM_NV_CHECK_RET(uvm_range_tree_find_hole_in(&state->tree, mid, &hole_start, &hole_end),
                     NV_ERR_UVM_ADDRESS_IN_USE);
    MEM_NV_CHECK_RET(uvm_range_tree_find_hole_in(&state->tree, end, &hole_start, &hole_end),
                     NV_ERR_UVM_ADDRESS_IN_USE);

    TEST_CHECK_RET(uvm_range_tree_node_size(node) == end - start + 1);

    if (end < ULLONG_MAX)
        TEST_CHECK_RET(uvm_range_tree_find(&state->tree, end + 1) != node);

    uvm_range_tree_for_each_in(temp, &state->tree, start, end)
        TEST_CHECK_RET(temp == node);
    uvm_range_tree_for_each_in_safe(temp, next, &state->tree, start, end)
        TEST_CHECK_RET(temp == node);

    prev = uvm_range_tree_prev(&state->tree, node);
    if (prev) {
        TEST_CHECK_RET(prev->end < node->start);
        TEST_CHECK_RET(uvm_range_tree_next(&state->tree, prev) == node);
    }
    else {
        TEST_CHECK_RET(uvm_range_tree_iter_prev(&state->tree, node, 0) == NULL);
        TEST_CHECK_RET(uvm_range_tree_iter_first(&state->tree, 0, ULLONG_MAX) == node);
    }

    next = uvm_range_tree_next(&state->tree, node);
    if (next) {
        TEST_CHECK_RET(node->end < next->start);
        TEST_CHECK_RET(uvm_range_tree_prev(&state->tree, next) == node);
        TEST_CHECK_RET(uvm_range_tree_last(&state->tree) != node);
    }
    else {
        TEST_CHECK_RET(uvm_range_tree_iter_next(&state->tree, node, ULLONG_MAX) == NULL);
        TEST_CHECK_RET(uvm_range_tree_last(&state->tree) == node);
    }

    TEST_NV_CHECK_RET(rtt_check_between(state, prev, node));
    TEST_NV_CHECK_RET(rtt_check_between(state, node, next));

    return NV_OK;
}

static NV_STATUS rtt_check_iterator_all(rtt_state_t *state)
{
    uvm_range_tree_node_t *node, *next, *prev = NULL, *expected = NULL;
    size_t iter_count = 0;

    uvm_range_tree_for_each(node, &state->tree) {
        if (expected)
            TEST_CHECK_RET(node == expected);

        if (prev)
            TEST_CHECK_RET(prev->end < node->start);
        TEST_CHECK_RET(uvm_range_tree_prev(&state->tree, node) == prev);

        TEST_NV_CHECK_RET(rtt_check_between(state, prev, node));

        ++iter_count;
        prev = node;
        expected = uvm_range_tree_next(&state->tree, node);
    }

    TEST_CHECK_RET(expected == NULL);
    TEST_CHECK_RET(uvm_range_tree_last(&state->tree) == prev);
    TEST_CHECK_RET(iter_count == state->count);
    TEST_NV_CHECK_RET(rtt_check_between(state, prev, NULL));

    iter_count = 0;
    expected = NULL;
    prev = NULL;
    uvm_range_tree_for_each_safe(node, next, &state->tree) {
        if (expected)
            TEST_CHECK_RET(node == expected);

        if (prev)
            TEST_CHECK_RET(prev->end < node->start);
        TEST_CHECK_RET(uvm_range_tree_prev(&state->tree, node) == prev);

        // Skip rtt_check_between since it was done in the loop above

        ++iter_count;
        prev = node;
        expected = uvm_range_tree_next(&state->tree, node);
    }

    TEST_CHECK_RET(expected == NULL);
    TEST_CHECK_RET(uvm_range_tree_last(&state->tree) == prev);
    TEST_CHECK_RET(iter_count == state->count);

    return NV_OK;
}


// Attempts to add the given range to the tree and performs some sanity checks
// on the outcome. This is O(N) in the number of nodes currently in the tree.
// Return value meanings:
//
// NV_OK                      The range was added successfully and the sanity
//                            checks passed.
//
// NV_ERR_UVM_ADDRESS_IN_USE  The range addition failed because the tree
//                            detected a collision in [range->start,
//                            range->end]. The collision sanity checks passed.
//
// NV_ERR_INVALID_STATE       The sanity checks failed for any reason.
//
// NV_ERR_NO_MEMORY           The obvious.
//
static NV_STATUS rtt_range_add_check(rtt_state_t *state, rtt_range_t *range)
{
    NV_STATUS status;
    uvm_range_tree_node_t *node = NULL;
    size_t i;
    int overlap = 0;

    UVM_ASSERT(range->start <= range->end);

    // Determine whether this should succeed or fail
    for (i = 0; i < state->count; i++) {
        if (rtt_range_overlaps_node(state->nodes[i], range)) {
            overlap = 1;
            break;
        }
    }

    // Verify tree state
    if (overlap) {
        node = uvm_range_tree_iter_first(&state->tree, range->start, range->end);
        TEST_CHECK_RET(node);
        TEST_CHECK_RET(rtt_range_overlaps_node(node, range));
    }
    else {
        NvU64 hole_start, hole_end;

        TEST_NV_CHECK_RET(uvm_range_tree_find_hole(&state->tree, range->start, &hole_start, &hole_end));
        TEST_CHECK_RET(hole_start <= range->start);
        TEST_CHECK_RET(hole_end >= range->end);

        hole_start = range->start;
        hole_end = range->end;
        TEST_NV_CHECK_RET(uvm_range_tree_find_hole_in(&state->tree, range->start, &hole_start, &hole_end));
        TEST_CHECK_RET(hole_start == range->start);
        TEST_CHECK_RET(hole_end == range->end);
    }

    status = rtt_range_add(state, range, &node);

    if (overlap) {
        MEM_NV_CHECK_RET(status, NV_ERR_UVM_ADDRESS_IN_USE);
    }
    else {
        MEM_NV_CHECK_RET(status, NV_OK);
        status = rtt_check_node(state, node);
    }

    return status;
}

// Returns NV_ERR_INVALID_STATE on sanity check failure, NV_OK otherwise.
static NV_STATUS rtt_index_remove_check(rtt_state_t *state, size_t index)
{
    uvm_range_tree_node_t *node, *prev, *next;
    NvU64 start, end;
    NvU64 hole_start, hole_end;
    NV_STATUS status;

    TEST_CHECK_RET(index < state->count);
    node  = state->nodes[index];
    start = node->start;
    end   = node->end;

    status = rtt_check_node(state, node);
    if (status != NV_OK)
        return status;

    prev = uvm_range_tree_prev(&state->tree, node);
    next = uvm_range_tree_next(&state->tree, node);

    status = rtt_index_remove(state, index);
    if (status != NV_OK)
        return status;

    // Verify removal
    TEST_CHECK_RET(uvm_range_tree_find(&state->tree, start) == NULL);
    TEST_CHECK_RET(uvm_range_tree_find(&state->tree, end) == NULL);
    TEST_CHECK_RET(uvm_range_tree_iter_first(&state->tree, start, end) == NULL);

    hole_start = start;
    hole_end = end;
    TEST_NV_CHECK_RET(uvm_range_tree_find_hole_in(&state->tree, start, &hole_start, &hole_end));
    TEST_CHECK_RET(hole_start == start);
    TEST_CHECK_RET(hole_end == end);

    TEST_NV_CHECK_RET(uvm_range_tree_find_hole(&state->tree, start, &hole_start, &hole_end));
    TEST_CHECK_RET(hole_start <= start);
    TEST_CHECK_RET(hole_end >= end);

    if (prev) {
        TEST_CHECK_RET(uvm_range_tree_next(&state->tree, prev) == next);
        TEST_CHECK_RET(hole_start == prev->end + 1);
    }

    if (next) {
        TEST_CHECK_RET(uvm_range_tree_prev(&state->tree, next) == prev);
        TEST_CHECK_RET(hole_end == next->start - 1);
    }
    else {
        TEST_CHECK_RET(uvm_range_tree_last(&state->tree) == prev);
    }

    if (!prev && !next) {
        TEST_CHECK_RET(uvm_range_tree_empty(&state->tree));
        TEST_CHECK_RET(uvm_range_tree_last(&state->tree) == NULL);
        TEST_CHECK_RET(hole_start == 0);
        TEST_CHECK_RET(hole_end == ULLONG_MAX);
        TEST_CHECK_RET(state->count == 0);
    }
    else {
        TEST_CHECK_RET(!uvm_range_tree_empty(&state->tree));
    }

    return NV_OK;
}

// Returns NV_ERR_INVALID_STATE on sanity check failure, NV_OK otherwise.
static NV_STATUS rtt_node_shrink_check(rtt_state_t *state, uvm_range_tree_node_t *node, NvU64 new_start, NvU64 new_end)
{
    uvm_range_tree_node_t *prev, *next;
    NV_STATUS status;
    NvU64 old_start = node->start;
    NvU64 old_end = node->end;

    status = rtt_check_node(state, node);
    if (status != NV_OK)
        return status;

    prev = uvm_range_tree_prev(&state->tree, node);
    next = uvm_range_tree_next(&state->tree, node);

    status = rtt_node_shrink(state, node, new_start, new_end);
    if (status != NV_OK)
        return status;

    status = rtt_check_node(state, node);
    if (status != NV_OK)
        return status;

    TEST_CHECK_RET(uvm_range_tree_prev(&state->tree, node) == prev);
    TEST_CHECK_RET(uvm_range_tree_next(&state->tree, node) == next);
    if (old_start != new_start)
        TEST_CHECK_RET(uvm_range_tree_find(&state->tree, old_start) == NULL);
    if (old_end != new_end)
        TEST_CHECK_RET(uvm_range_tree_find(&state->tree, old_end) == NULL);
    TEST_CHECK_RET(uvm_range_tree_find(&state->tree, new_start) == node);
    TEST_CHECK_RET(uvm_range_tree_find(&state->tree, new_end) == node);

    return NV_OK;
}

static NV_STATUS rtt_remove_all_check(rtt_state_t *state)
{
    NV_STATUS status;

    status = rtt_check_iterator_all(state);
    if (status != NV_OK)
        return status;

    while (state->count) {
        status = rtt_index_remove_check(state, 0);
        if (status != NV_OK)
            return status;
    }
    return NV_OK;
}

static NV_STATUS rtt_node_split_check(rtt_state_t *state, uvm_range_tree_node_t *node, NvU64 new_end)
{
    uvm_range_tree_node_t *prev, *next, *new = NULL;
    NV_STATUS status;

    status = rtt_check_node(state, node);
    if (status != NV_OK)
        return status;

    prev = uvm_range_tree_prev(&state->tree, node);
    next = uvm_range_tree_next(&state->tree, node);

    status = rtt_node_split(state, node, new_end, &new);
    if (status != NV_OK)
        return status;

    status = rtt_check_node(state, node);
    if (status != NV_OK)
        return status;
    status = rtt_check_node(state, new);
    if (status != NV_OK)
        return status;

    TEST_CHECK_RET(uvm_range_tree_prev(&state->tree, node) == prev);
    TEST_CHECK_RET(uvm_range_tree_next(&state->tree, node) == new);
    TEST_CHECK_RET(uvm_range_tree_prev(&state->tree, new)  == node);
    TEST_CHECK_RET(uvm_range_tree_next(&state->tree, new)  == next);
    return NV_OK;
}

// The rtt_index_merge_check_* functions don't have a non-check helper because
// both the helper and the caller need to walk the whole array to properly free
// the removed node. It's simpler to just handle all that in the same function.
static NV_STATUS rtt_index_merge_check_prev(rtt_state_t *state, size_t index)
{
    uvm_range_tree_node_t *node, *prev, *returned, *expected = NULL;
    size_t i = 0; // Shut up compiler
    NV_STATUS status;

    TEST_CHECK_RET(index < state->count);
    node = state->nodes[index];

    status = rtt_check_node(state, node);
    if (status != NV_OK)
        return status;

    // Figure out if this should succeed or fail
    if (node->start != 0) {
        for (i = 0; i < state->count; i++) {
            if (state->nodes[i]->end == node->start - 1) {
                expected = state->nodes[i];
                break;
            }
        }
    }

    prev = uvm_range_tree_prev(&state->tree, node);
    if (expected) {
        TEST_CHECK_RET(prev == expected);
        status = rtt_check_node(state, expected);
        if (status != NV_OK)
            return status;
    }
    else if (prev) {
        TEST_CHECK_RET(prev->end < node->start - 1);
    }

    returned = uvm_range_tree_merge_prev(&state->tree, node);
    TEST_CHECK_RET(returned == expected);

    status = rtt_check_node(state, node);
    if (status != NV_OK)
        return status;

    if (expected) {
        TEST_CHECK_RET(node->start == expected->start);

        // We don't care about ordering so move the last node into the free slot
        uvm_kvfree(expected);
        --state->count;
        state->nodes[i] = state->nodes[state->count];
        // No change to size
        ++state->stats.total_merges;

        return NV_OK;
    }

    // Failed merge
    return NV_ERR_INVALID_ADDRESS;
}

static NV_STATUS rtt_index_merge_check_next(rtt_state_t *state, size_t index)
{
    uvm_range_tree_node_t *node, *next, *returned, *expected = NULL;
    size_t i = 0; // Shut up compiler
    NV_STATUS status;

    TEST_CHECK_RET(index < state->count);
    node = state->nodes[index];

    status = rtt_check_node(state, node);
    if (status != NV_OK)
        return status;

    // Figure out if this should succeed or fail
    if (node->end != ULLONG_MAX) {
        for (i = 0; i < state->count; i++) {
            if (state->nodes[i]->start == node->end + 1) {
                expected = state->nodes[i];
                break;
            }
        }
    }

    next = uvm_range_tree_next(&state->tree, node);
    if (expected) {
        TEST_CHECK_RET(next == expected);
        status = rtt_check_node(state, expected);
        if (status != NV_OK)
            return status;
    }
    else if (next) {
        TEST_CHECK_RET(next->start > node->end + 1);
    }

    returned = uvm_range_tree_merge_next(&state->tree, node);
    TEST_CHECK_RET(returned == expected);

    status = rtt_check_node(state, node);
    if (status != NV_OK)
        return status;

    if (expected) {
        TEST_CHECK_RET(node->end == expected->end);

        // We don't care about ordering so move the last node into the free slot
        uvm_kvfree(expected);
        --state->count;
        state->nodes[i] = state->nodes[state->count];
        // No change to size
        ++state->stats.total_merges;

        return NV_OK;
    }

    // Failed merge
    return NV_ERR_INVALID_ADDRESS;
}


// Directed test helpers for using hard-coded values

// Returns the index of the node containing addr, or state->count if none.
static size_t rtt_node_find(rtt_state_t *state, NvU64 addr)
{
    size_t i;
    for (i = 0; i < state->count; i++) {
        if (state->nodes[i]->start <= addr && addr <= state->nodes[i]->end)
            break;
    }
    return i;
}

static NV_STATUS rtt_range_add_check_val(rtt_state_t *state, NvU64 start, NvU64 end)
{
    rtt_range_t range = {start, end};
    return rtt_range_add_check(state, &range);
}

static NV_STATUS rtt_index_remove_check_val(rtt_state_t *state, NvU64 addr)
{
    size_t index = rtt_node_find(state, addr);
    if (index == state->count)
        return NV_ERR_INVALID_STATE;
    return rtt_index_remove_check(state, index);
}

static NV_STATUS rtt_node_shrink_check_val(rtt_state_t *state, NvU64 new_start, NvU64 new_end)
{
    size_t index = rtt_node_find(state, new_start);
    if (index == state->count)
        return NV_ERR_INVALID_STATE;
    return rtt_node_shrink_check(state, state->nodes[index], new_start, new_end);
}

static NV_STATUS rtt_node_split_check_val(rtt_state_t *state, NvU64 new_end)
{
    size_t index = rtt_node_find(state, new_end);
    if (index == state->count || new_end == state->nodes[index]->end)
        return NV_ERR_INVALID_STATE;
    return rtt_node_split_check(state, state->nodes[index], new_end);
}

static NV_STATUS rtt_index_merge_check_prev_val(rtt_state_t *state, NvU64 addr)
{
    size_t index = rtt_node_find(state, addr);
    if (index == state->count)
        return NV_ERR_INVALID_STATE;
    return rtt_index_merge_check_prev(state, index);
}

static NV_STATUS rtt_index_merge_check_next_val(rtt_state_t *state, NvU64 addr)
{
    size_t index = rtt_node_find(state, addr);
    if (index == state->count)
        return NV_ERR_INVALID_STATE;
    return rtt_index_merge_check_next(state, index);
}

static NV_STATUS rtt_directed(rtt_state_t *state)
{
    uvm_range_tree_node_t *node, *next;

    // Empty tree
    TEST_CHECK_RET(uvm_range_tree_empty(&state->tree));
    TEST_CHECK_RET(uvm_range_tree_last(&state->tree) == NULL);
    TEST_CHECK_RET(uvm_range_tree_find(&state->tree, 0) == NULL);
    TEST_CHECK_RET(uvm_range_tree_find(&state->tree, ULLONG_MAX) == NULL);
    uvm_range_tree_for_each(node, &state->tree)
        TEST_CHECK_RET(0);
    uvm_range_tree_for_each_in(node, &state->tree, 0,          0)
        TEST_CHECK_RET(0);
    uvm_range_tree_for_each_in(node, &state->tree, 0,          ULLONG_MAX)
        TEST_CHECK_RET(0);
    uvm_range_tree_for_each_in(node, &state->tree, ULLONG_MAX, ULLONG_MAX)
        TEST_CHECK_RET(0);
    uvm_range_tree_for_each_in_safe(node, next, &state->tree, 0,          0)
        TEST_CHECK_RET(0);
    uvm_range_tree_for_each_in_safe(node, next, &state->tree, 0,          ULLONG_MAX)
        TEST_CHECK_RET(0);
    uvm_range_tree_for_each_in_safe(node, next, &state->tree, ULLONG_MAX, ULLONG_MAX)
        TEST_CHECK_RET(0);
    TEST_NV_CHECK_RET(rtt_check_between(state, NULL, NULL));

    // Consume entire range
    MEM_NV_CHECK_RET(rtt_range_add_check_val(state,  0,          ULLONG_MAX), NV_OK);
    MEM_NV_CHECK_RET(rtt_range_add_check_val(state,  0,          0),          NV_ERR_UVM_ADDRESS_IN_USE);
    MEM_NV_CHECK_RET(rtt_range_add_check_val(state,  0,          ULLONG_MAX), NV_ERR_UVM_ADDRESS_IN_USE);
    MEM_NV_CHECK_RET(rtt_range_add_check_val(state,  ULLONG_MAX, ULLONG_MAX), NV_ERR_UVM_ADDRESS_IN_USE);
    MEM_NV_CHECK_RET(rtt_range_add_check_val(state,  0,          1),          NV_ERR_UVM_ADDRESS_IN_USE);
    MEM_NV_CHECK_RET(rtt_range_add_check_val(state,  5,          7),          NV_ERR_UVM_ADDRESS_IN_USE);
    MEM_NV_CHECK_RET(rtt_range_add_check_val(state,  7,          ULLONG_MAX), NV_ERR_UVM_ADDRESS_IN_USE);
    MEM_NV_CHECK_RET(rtt_remove_all_check(state),                             NV_OK);

    // Two non-overlapping ranges
    MEM_NV_CHECK_RET(rtt_range_add_check_val(state, 10,    20), NV_OK);
    MEM_NV_CHECK_RET(rtt_range_add_check_val(state,  0,     5), NV_OK); // Non-adjacent left
    MEM_NV_CHECK_RET(rtt_index_remove_check_val(state, 0),      NV_OK);
    MEM_NV_CHECK_RET(rtt_range_add_check_val(state,  0,     9), NV_OK); // Adjacent left
    MEM_NV_CHECK_RET(rtt_index_remove_check_val(state, 0),      NV_OK);
    MEM_NV_CHECK_RET(rtt_range_add_check_val(state, 21,    30), NV_OK); // Adjacent right
    MEM_NV_CHECK_RET(rtt_index_remove_check_val(state, 21),     NV_OK);
    MEM_NV_CHECK_RET(rtt_range_add_check_val(state, 25,    30), NV_OK); // Non-adjacent right
    MEM_NV_CHECK_RET(rtt_remove_all_check(state),               NV_OK);

    // Two overlapping ranges
    MEM_NV_CHECK_RET(rtt_range_add_check_val(state, 10,    20), NV_OK);
    MEM_NV_CHECK_RET(rtt_range_add_check_val(state,  0,    10), NV_ERR_UVM_ADDRESS_IN_USE);
    MEM_NV_CHECK_RET(rtt_range_add_check_val(state,  9,    11), NV_ERR_UVM_ADDRESS_IN_USE);
    MEM_NV_CHECK_RET(rtt_range_add_check_val(state, 10,    20), NV_ERR_UVM_ADDRESS_IN_USE);
    MEM_NV_CHECK_RET(rtt_range_add_check_val(state, 11,    19), NV_ERR_UVM_ADDRESS_IN_USE);
    MEM_NV_CHECK_RET(rtt_range_add_check_val(state, 19,    21), NV_ERR_UVM_ADDRESS_IN_USE);
    MEM_NV_CHECK_RET(rtt_range_add_check_val(state, 20,    30), NV_ERR_UVM_ADDRESS_IN_USE);
    MEM_NV_CHECK_RET(rtt_range_add_check_val(state,  0,    30), NV_ERR_UVM_ADDRESS_IN_USE);
    MEM_NV_CHECK_RET(rtt_remove_all_check(state),               NV_OK);

    // Fill gaps
    MEM_NV_CHECK_RET(rtt_range_add_check_val(state,  0,    10), NV_OK);
    MEM_NV_CHECK_RET(rtt_range_add_check_val(state, 20,    30), NV_OK);
    MEM_NV_CHECK_RET(rtt_range_add_check_val(state, 12,    18), NV_OK);
    MEM_NV_CHECK_RET(rtt_range_add_check_val(state, 11,    11), NV_OK);
    MEM_NV_CHECK_RET(rtt_range_add_check_val(state, 19,    19), NV_OK);
    MEM_NV_CHECK_RET(rtt_remove_all_check(state),               NV_OK);

    // Split ranges (new ranges of size 1)
    MEM_NV_CHECK_RET(rtt_range_add_check_val(state,    0, 2),   NV_OK); // [0-----2]
    MEM_NV_CHECK_RET(rtt_node_split_check_val(state,   0),      NV_OK); // [0][1--2]
    MEM_NV_CHECK_RET(rtt_node_split_check_val(state,   1),      NV_OK); // [0][1][2]
    MEM_NV_CHECK_RET(rtt_index_remove_check_val(state, 1),      NV_OK); // [0]   [2]
    MEM_NV_CHECK_RET(rtt_remove_all_check(state),               NV_OK);

    // Split ranges (new ranges of size >1)
    MEM_NV_CHECK_RET(rtt_range_add_check_val(state,    0, 11), NV_OK); // [0-----------11]
    MEM_NV_CHECK_RET(rtt_node_split_check_val(state,   3),     NV_OK); // [0-3][4------11]
    MEM_NV_CHECK_RET(rtt_node_split_check_val(state,   7),     NV_OK); // [0-3][4-7][8-11]
    MEM_NV_CHECK_RET(rtt_index_remove_check_val(state, 4),     NV_OK); // [0-3]     [8-11]
    MEM_NV_CHECK_RET(rtt_remove_all_check(state),              NV_OK);

    // Merges
    MEM_NV_CHECK_RET(rtt_range_add_check_val(state,        0, 0),   NV_OK); // [0]
    MEM_NV_CHECK_RET(rtt_index_merge_check_prev_val(state, 0),      NV_ERR_INVALID_ADDRESS);
    MEM_NV_CHECK_RET(rtt_index_merge_check_next_val(state, 0),      NV_ERR_INVALID_ADDRESS);
    MEM_NV_CHECK_RET(rtt_range_add_check_val(state,        1, 1),   NV_OK); // [0][1]
    MEM_NV_CHECK_RET(rtt_index_merge_check_next_val(state, 0),      NV_OK); // [0--1]
    MEM_NV_CHECK_RET(rtt_range_add_check_val(state,        2, 2),   NV_OK); // [0--1][2]
    MEM_NV_CHECK_RET(rtt_index_merge_check_prev_val(state, 2),      NV_OK); // [0-----2]
    MEM_NV_CHECK_RET(rtt_remove_all_check(state),                   NV_OK);

    // Shrinks
    MEM_NV_CHECK_RET(rtt_range_add_check_val(state,    0, 20), NV_OK);                     // [0---------------------20]
    MEM_NV_CHECK_RET(rtt_node_shrink_check_val(state,  5, 15), NV_OK);                     //      [5------------15]
    MEM_NV_CHECK_RET(rtt_range_add_check_val(state,    5,  5), NV_ERR_UVM_ADDRESS_IN_USE); //      [5------------15]
    MEM_NV_CHECK_RET(rtt_range_add_check_val(state,   15, 15), NV_ERR_UVM_ADDRESS_IN_USE); //      [5------------15]
    MEM_NV_CHECK_RET(rtt_range_add_check_val(state,   16, 16), NV_OK);                     //      [5------------15][16]
    MEM_NV_CHECK_RET(rtt_range_add_check_val(state,    4,  4), NV_OK);                     //   [4][5------------15][16]
    MEM_NV_CHECK_RET(rtt_node_shrink_check_val(state, 10, 10), NV_OK);                     //   [4]      [10]       [16]
    MEM_NV_CHECK_RET(rtt_range_add_check_val(state,    5,  9), NV_OK);                     //   [4][5--9][10]       [16]
    MEM_NV_CHECK_RET(rtt_range_add_check_val(state,   11, 15), NV_OK);                     //   [4][5--9][10][11-15][16]
    MEM_NV_CHECK_RET(rtt_remove_all_check(state),              NV_OK);

    return NV_OK;
}

NV_STATUS uvm_test_range_tree_directed(UVM_TEST_RANGE_TREE_DIRECTED_PARAMS *params, struct file *filp)
{
    rtt_state_t *state;
    NV_STATUS status;

    state = rtt_state_create();
    if (!state)
        return NV_ERR_NO_MEMORY;
    status = rtt_directed(state);
    rtt_state_destroy(state);
    return status;
}

// ------------------------------ Random Test ------------------------------ //

// Randomly place a block of the given size in the range described by bounds.
// size == 0 means size == 2^64.
static void rtt_rand_place(uvm_test_rng_t *rng, NvU64 size, rtt_range_t *bounds, rtt_range_t *out)
{
    UVM_ASSERT(bounds->start <= bounds->end);

    if (size == 0) {
        // No placement choice
        UVM_ASSERT(bounds->start == 0 && bounds->end == ULLONG_MAX);
        out->start = 0;
        out->end   = ULLONG_MAX;
    }
    else {
        UVM_ASSERT(rtt_get_range_size(bounds) == 0 || size <= rtt_get_range_size(bounds));

        // Select a placement with uniform distribution. Note that bounds->end +
        // 1 might overflow, but we know that size >= 1 so the range will be
        // sane.
        out->start = uvm_test_rng_range_64(rng, bounds->start, bounds->end + 1 - size);
        out->end   = out->start + size - 1;
    }
}

// Compute a range in [0, max_end] of random size. The size is selected with
// logarithmic distribution for a good mix of large and small ranges.
static void rtt_get_rand_range(uvm_test_rng_t *rng, NvU64 max_end, rtt_range_t *out)
{
    rtt_range_t bounds = {0, max_end};
    NvU64 size;

    // Offset size by 1 to handle overflow when max_end is ULLONG_MAX.
    size = uvm_test_rng_range_log64(rng, 0, max_end) + 1;
    rtt_rand_place(rng, size, &bounds, out);
}

// Like rtt_get_rand_range but guarantees that the generated range will overlap
// with the input cover range. This is used to generate overlapping ranges to
// verify collision detection.
static void rtt_get_rand_range_covering(uvm_test_rng_t *rng,
                                        NvU64 max_end,
                                        rtt_range_t *cover,
                                        rtt_range_t *out)
{
    NvU64 size;
    rtt_range_t bounds;

    UVM_ASSERT(cover->end <= max_end);

    // Pick a logarithmic size. Offset by 1 to handle overflow when max_end is
    // ULLONG_MAX.
    size = uvm_test_rng_range_log64(rng, 0, max_end) + 1;
    if (size == ULLONG_MAX) {
        // No choice
        UVM_ASSERT(max_end == ULLONG_MAX);
        out->start = 0;
        out->end   = ULLONG_MAX;
        return;
    }

    // Compute the range where a block of size can be placed to still overlap
    // with the input range.
    if (cover->start < size)
        bounds.start = 0;
    else
        bounds.start = cover->start - size + 1;

    // Make sure we don't exceed max_end while still covering the range. Also
    // watch out for overflowing max_end in these calculations.
    if (size > max_end - cover->end)
        bounds.end = max_end;
       else
        bounds.end = cover->end + size - 1;

    rtt_rand_place(rng, size, &bounds, out);
    UVM_ASSERT(rtt_ranges_overlap(cover, out));
}

// Attempt to add N ranges to the tree, where N is randomly selected from the
// range [1, params->max_batch_count]. Each range is randomly chosen.
//
// Repeats eachs individual addition on collision up to params->max_attempts
// times. If the attempt threshold is reached this stops trying to add more
// ranges, adjusts the RNG probabilities to prefer remove operations, and
// returns NV_ERR_BUSY_RETRY.
static NV_STATUS rtt_batch_add(rtt_state_t *state, UVM_TEST_RANGE_TREE_RANDOM_PARAMS *params)
{
    size_t size = 0, ranges_to_add, max_ranges;
    NvU32 collisions = 0;
    NV_STATUS status = NV_OK;
    rtt_range_t range, bounds = {0, params->max_end};

    max_ranges = params->max_ranges - state->count;
    if (max_ranges == 0)
        return NV_OK;

    max_ranges = min(max_ranges, (size_t)params->max_batch_count);
    ranges_to_add = uvm_test_rng_range_ptr(&state->rng, 1, max_ranges);

    if (params->verbose)
        UVM_TEST_PRINT("Adding %zu ranges\n", ranges_to_add);

    while (ranges_to_add) {
        if (fatal_signal_pending(current))
            return NV_ERR_SIGNAL_PENDING;

        // If we succeeded the last range add, pick a new range
        if (status != NV_ERR_UVM_ADDRESS_IN_USE) {
            rtt_get_rand_range(&state->rng, params->max_end, &range);
            size = rtt_get_range_size(&range);
        }
        else {
            // We collided last time. Try again in a new spot with a reduced
            // size.
            if (size == 0) // means 2^64
                size = ((size_t)-1) / 2;
            else
                size = max((size_t)1, size/2);
            rtt_rand_place(&state->rng, size, &bounds, &range);
        }

        // Try to add the new range
        status = rtt_range_add_check(state, &range);
        if (status == NV_ERR_UVM_ADDRESS_IN_USE) {
            ++collisions;
            ++state->stats.failed_adds;
            if (collisions >= params->max_attempts) {
                ++state->stats.max_attempts_add;
                if (params->verbose) {
                    UVM_TEST_PRINT("Collision threshold reached with %zu ranges covering %llu (max_end %llu)\n",
                                   state->count, state->stats.size_sum, params->max_end);
                }

                // Tell RNG to prefer removes
                state->add_chance = 100 - params->high_probability;
                return NV_ERR_BUSY_RETRY;
            }
            if (params->verbose)
                UVM_TEST_PRINT("Failed to add [%llu, %llu], trying again\n", range.start, range.end);
        }
        else {
            MEM_NV_CHECK_RET(status, NV_OK);
            if (params->verbose)
                UVM_TEST_PRINT("Added [%llu, %llu]\n", range.start, range.end);
            --ranges_to_add;
            collisions = 0;
        }
    }

    return NV_OK;
}

// Removes N ranges from the tree, where N is randomly selected from the range
// [1, params->max_batch_count].
static NV_STATUS rtt_batch_remove(rtt_state_t *state, UVM_TEST_RANGE_TREE_RANDOM_PARAMS *params)
{
    size_t index, max_ranges, ranges_to_remove;
    NV_STATUS status;

    if (state->count == 0)
        return NV_OK;

    max_ranges = min(state->count, (size_t)params->max_batch_count);
    ranges_to_remove = uvm_test_rng_range_ptr(&state->rng, 1, max_ranges);

    if (params->verbose)
        UVM_TEST_PRINT("Removing %zu ranges\n", ranges_to_remove);

    while (ranges_to_remove) {
        index = uvm_test_rng_range_ptr(&state->rng, 0, state->count - 1);
        if (params->verbose)
            UVM_TEST_PRINT("Removing [%llu, %llu]\n", state->nodes[index]->start, state->nodes[index]->end);
        status = rtt_index_remove_check(state, index);
        if (status != NV_OK)
            return status;
        --ranges_to_remove;
    }

    return NV_OK;
}

// Attempts to shrink a randomly-selected range in the tree. On selecting a
// range of size 1, the attempt is repeated with another range up to the
// params->max_attempts threshold.
static NV_STATUS rtt_rand_shrink(rtt_state_t *state, UVM_TEST_RANGE_TREE_RANDOM_PARAMS *params)
{
    uvm_range_tree_node_t *node = NULL;
    NvU64 old_start;
    NvU64 old_end;
    NvU64 new_start;
    NvU64 new_end;
    NvU32 i;
    NV_STATUS status;

    if (state->count == 0)
        return NV_OK;

    // Randomly try to find a shrinkable range (size > 1)
    for (i = 0; i < params->max_attempts; i++) {
        size_t index;
        if (fatal_signal_pending(current))
            return NV_ERR_SIGNAL_PENDING;

        index = uvm_test_rng_range_ptr(&state->rng, 0, state->count - 1);
        if (state->nodes[index]->start != state->nodes[index]->end) {
            node = state->nodes[index];
            break;
        }
        ++state->stats.failed_shrinks;
    }

    if (!node)
        return NV_ERR_BUSY_RETRY;

    // Pick a random new start and new end
    old_start = node->start;
    old_end = node->end;
    new_start = uvm_test_rng_range_64(&state->rng, node->start, node->end);
    new_end = uvm_test_rng_range_64(&state->rng, node->start, node->end);
    if (new_end < new_start) {
        // Swap start and end to get a valid range
        swap(new_start, new_end);
    }
    status = rtt_node_shrink_check(state, node, new_start, new_end);
    if (status != NV_OK)
        return status;

    if (params->verbose) {
        UVM_TEST_PRINT("Shrink [%llu, %llu] to [%llu, %llu]\n",
                       old_start, old_end,
                       new_start, new_end);
    }

    return NV_OK;
}

// Attempts to split a randomly-selected range in the tree. On selecting a range
// of size 1, the attempt is repeated with another range up to the
// params->max_attempts threshold. On reaching the attempt threshold the RNG
// probabilities are adjusted to prefer merge operations and NV_ERR_BUSY_RETRY
// is returned.
static NV_STATUS rtt_rand_split(rtt_state_t *state, UVM_TEST_RANGE_TREE_RANDOM_PARAMS *params)
{
    uvm_range_tree_node_t *node = NULL;
    rtt_range_t old_range;
    size_t index;
    NvU64 new_end;
    NvU32 i;
    NV_STATUS status;

    if (state->count == 0 || state->count == params->max_ranges)
        return NV_OK;

    // Randomly try to find a splittable range (size > 1)
    for (i = 0; i < params->max_attempts; i++) {
        if (fatal_signal_pending(current))
            return NV_ERR_SIGNAL_PENDING;

        index = uvm_test_rng_range_ptr(&state->rng, 0, state->count - 1);
        if (state->nodes[index]->start != state->nodes[index]->end) {
            node = state->nodes[index];
            break;
        }
        ++state->stats.failed_splits;
    }

    if (!node) {
        ++state->stats.max_attempts_split;
        if (params->verbose) {
            UVM_TEST_PRINT("Split attempt threshold reached with %zu ranges covering %llu (max_end %llu)\n",
                           state->count, state->stats.size_sum, params->max_end);
        }

        // Tell the RNG to prefer merges
        state->split_chance = 100 - params->high_probability;
        return NV_ERR_BUSY_RETRY;
    }

    // Pick a random split point and do the split
    old_range = rtt_node_get_range(node);
    new_end = uvm_test_rng_range_64(&state->rng, node->start, node->end - 1);
    status = rtt_node_split_check(state, node, new_end);
    if (status != NV_OK)
        return status;

    if (params->verbose) {
        UVM_TEST_PRINT("Split [%llu, %llu] into [%llu, %llu][%llu, %llu]\n",
                       old_range.start, old_range.end,
                       old_range.start, new_end, new_end + 1, old_range.end);
    }

    return NV_OK;
}

// Attempts to merge a randomly-selected range in the tree in a randomly-
// selected direction (next or prev). On selecting a range with a non-adjacent
// neighbor, the attempt is repeated with another range up to the
// params->max_attempts threshold. On reaching the attempt threshold the RNG
// probabilities are adjusted to prefer split operations and NV_ERR_BUSY_RETRY
// is returned.
static NV_STATUS rtt_rand_merge(rtt_state_t *state, UVM_TEST_RANGE_TREE_RANDOM_PARAMS *params)
{
    uvm_range_tree_node_t *node;
    size_t index;
    NvU32 i;
    NV_STATUS status;
    rtt_range_t old_range;
    int try_prev;

    if (state->count < 2)
        return NV_OK;

    // Randomly try to find a mergeable range
    for (i = 0; i < params->max_attempts; i++) {
        if (fatal_signal_pending(current))
            return NV_ERR_SIGNAL_PENDING;

        // Pick a new direction each time
        try_prev = uvm_test_rng_range_32(&state->rng, 0, 1);

        index = uvm_test_rng_range_ptr(&state->rng, 0, state->count - 1);
        node = state->nodes[index];
        old_range = rtt_node_get_range(node);

        if (try_prev)
            status = rtt_index_merge_check_prev(state, index);
        else
            status = rtt_index_merge_check_next(state, index);

        if (status == NV_OK) {
            if (params->verbose) {
                UVM_TEST_PRINT("Merged [%llu, %llu] to [%llu, %llu]\n",
                               old_range.start, old_range.end,
                               node->start, node->end);
            }
            return NV_OK;
        }
        else if (status != NV_ERR_INVALID_ADDRESS) {
            return status;
        }

        ++state->stats.failed_merges;
    }

    // We exceeded max_attempts. Tell the RNG to prefer splits.
    if (params->verbose) {
        UVM_TEST_PRINT("Merge attempt threshold reached with %zu ranges covering %llu (max_end %llu)\n",
                       state->count, state->stats.size_sum, params->max_end);
    }

    ++state->stats.max_attempts_merge;
    state->split_chance = params->high_probability;
    return NV_ERR_BUSY_RETRY;
}

// Randomly generate a range that collides with an allocated range and verify
// that adding the range fails.
static NV_STATUS rtt_rand_collision_check(rtt_state_t *state, NvU64 max_end)
{
    size_t index;
    rtt_range_t cover, check;

    if (state->count == 0)
        return NV_OK;

    // Pick an existing node at random and generate a range which overlaps that
    // node.
    index = uvm_test_rng_range_ptr(&state->rng, 0, state->count - 1);
    cover = rtt_node_get_range(state->nodes[index]);
    rtt_get_rand_range_covering(&state->rng, max_end, &cover, &check);

    MEM_NV_CHECK_RET(rtt_range_add(state, &check, NULL), NV_ERR_UVM_ADDRESS_IN_USE);

    return NV_OK;
}

// Generate a random range and verify that the tree iterator walks all nodes
// in that range in order.
static NV_STATUS rtt_rand_iterator_check(rtt_state_t *state, NvU64 max_end)
{
    uvm_range_tree_node_t *node;
    uvm_range_tree_node_t *prev = NULL, *first = NULL, *last = NULL, *next = NULL;
    size_t i, target_count = 0, iter_count = 0;
    NvU64 hole_start, hole_end, test_start, test_end;
    rtt_range_t range;

    // Generate the range to check
    rtt_get_rand_range(&state->rng, max_end, &range);

    // Phase 1: Iterate through the unordered list, counting how many nodes we
    // ought to see from the tree iterator and finding the boundary nodes.
    for (i = 0; i < state->count; i++) {
        node = state->nodes[i];

        if (rtt_range_overlaps_node(node, &range)) {
            ++target_count;

            // first is the lowest node with any overlap
            if (!first || first->start > node->start)
                first = node;

            // last is the highest node with any overlap
            if (!last || last->end < node->end)
                last = node;
        }
        else {
            // prev is the highest node with end < range.start
            if (node->end < range.start && (!prev || node->end > prev->end))
                prev = node;

            // next is the lowest node with start > range.end
            if (node->start > range.end && (!next || node->start < next->start))
                next = node;
        }
    }

    // Phase 2: Use the tree iterators

    // The holes between the nodes will be checked within the iterator loop.
    // Here we check the holes at the start and end of the range, if any.
    if (first) {
        if (range.start < first->start) {
            // Check hole at range.start
            hole_start = prev ? prev->end + 1 : 0;
            hole_end = first->start - 1;
            TEST_NV_CHECK_RET(uvm_range_tree_find_hole(&state->tree, range.start, &test_start, &test_end));
            TEST_CHECK_RET(test_start == hole_start);
            TEST_CHECK_RET(test_end == hole_end);

            test_start = range.start;
            test_end = ULLONG_MAX;
            TEST_NV_CHECK_RET(uvm_range_tree_find_hole_in(&state->tree, range.start, &test_start, &test_end));
            TEST_CHECK_RET(test_start == range.start);
            TEST_CHECK_RET(test_end == hole_end);
        }

        // Else, no hole at start
    }
    else {
        // No nodes intersect the range
        UVM_ASSERT(target_count == 0);
        UVM_ASSERT(!last);

        hole_start = prev ? prev->end + 1 : 0;
        hole_end = next ? next->start - 1 : ULLONG_MAX;
        TEST_NV_CHECK_RET(uvm_range_tree_find_hole(&state->tree, range.start, &test_start, &test_end));
        TEST_CHECK_RET(test_start == hole_start);
        TEST_CHECK_RET(test_end == hole_end);

        test_start = range.start;
        test_end = range.end;
        TEST_NV_CHECK_RET(uvm_range_tree_find_hole_in(&state->tree, range.start, &test_start, &test_end));
        TEST_CHECK_RET(test_start == range.start);
        TEST_CHECK_RET(test_end == range.end);
    }

    if (last && range.end > last->end) {
        // Check hole at range.end
        hole_start = last->end + 1;
        hole_end = next ? next->start - 1 : ULLONG_MAX;
        TEST_NV_CHECK_RET(uvm_range_tree_find_hole(&state->tree, range.end, &test_start, &test_end));
        TEST_CHECK_RET(test_start == hole_start);
        TEST_CHECK_RET(test_end == hole_end);

        test_start = 0;
        test_end = range.end;
        TEST_NV_CHECK_RET(uvm_range_tree_find_hole_in(&state->tree, range.end, &test_start, &test_end));
        TEST_CHECK_RET(test_start == hole_start);
        TEST_CHECK_RET(test_end == range.end);
    }

    uvm_range_tree_for_each_in(node, &state->tree, range.start, range.end) {
        TEST_CHECK_RET(rtt_range_overlaps_node(node, &range));
        if (prev) {
            TEST_CHECK_RET(prev->end < node->start);
            TEST_NV_CHECK_RET(rtt_check_between(state, prev, node));
        }

        ++iter_count;
        prev = node;
    }

    TEST_CHECK_RET(iter_count == target_count);

    prev = NULL;
    iter_count = 0;
    uvm_range_tree_for_each_in_safe(node, next, &state->tree, range.start, range.end) {
        TEST_CHECK_RET(rtt_range_overlaps_node(node, &range));
        if (prev)
            TEST_CHECK_RET(prev->end < node->start);
        ++iter_count;
        prev = node;
    }

    TEST_CHECK_RET(iter_count == target_count);
    return NV_OK;
}

static rtt_op_t rtt_get_rand_op(rtt_state_t *state, UVM_TEST_RANGE_TREE_RANDOM_PARAMS *params)
{
    NvU32 r_group, r_sub;

    // The possible options depend on the current number of nodes in the tree:
    // 0            add
    // 1 (max == 1) remove
    // 1 (max != 1) add, remove, shrink, split
    // >1, <max     add, remove, shrink, split, merge
    // max          remove, merge

    if (state->count == 0)
        return RTT_OP_ADD;
    if (state->count == 1 && state->count == params->max_ranges)
        return RTT_OP_REMOVE;

    // r_group selects between the two groups of operations, either {add/remove/
    // shrink} or {merge/split}. r_sub selects the sub operation within that
    // group based on the current probability settings.
    r_group = uvm_test_rng_range_32(&state->rng, 1, 100);
    r_sub   = uvm_test_rng_range_32(&state->rng, 1, 100);

    if (state->count < params->max_ranges) {
        if (r_group <= params->add_remove_shrink_group_probability) {
            if (r_sub <= state->shrink_probability)
                return RTT_OP_SHRINK;

            // After giving shrink a chance, redo the randomization for add/
            // remove.
            r_sub = uvm_test_rng_range_32(&state->rng, 1, 100);

            if (r_sub <= state->add_chance)
                return RTT_OP_ADD;
            return RTT_OP_REMOVE;
        }
        else {
            if (state->count == 1 || r_sub <= state->split_chance)
                return RTT_OP_SPLIT;
            return RTT_OP_MERGE;
        }
    }

    // We're at max
    if (r_group <= params->add_remove_shrink_group_probability)
        return RTT_OP_REMOVE;
    return RTT_OP_MERGE;
}

// This random stress test performs the following every iteration of the main
// loop:
// - Perform a random operation on the tree, one of:
//      - Add a randomized number of elements from the tree
//      - Remove a randomized number of elements from the tree
//      - Shrink a random element in the tree
//      - Split a random element in the tree
//      - Merge a random element in the tree with its neighbor
// - Randomly generate ranges that overlap with at least one node, attempt to
//   add those ranges to the tree, and verify that they fail.
// - Randomly generate ranges and verify that tree iterator reports all nodes
//   in the range in the proper order.
//
// Operations are split into two groups:
//
// Group 1: add/remove/shrink
// Group 2: split/merge
//
// params->add_remove_shrink_group_probability is used to select which operation
// group to use each iteration. The selection of operation within that group
// depends on the current "mode." Initially, add and split operations are
// weighted heavily (with params->high_probability). If we reach the
// params->max_attempts threshold while trying to perform one of those
// operations, the probability of that operation is reversed to prefer removes
// or merges respectively.
//
// In the case of add/remove, the probability will also change if the tree is
// empty or full.
//
// A better (less random) test would be to track the available free ranges and
// randomly perform an allocation somewhere there. Then the collisions would be
// completely deterministic, and we could be guaranteed to eventually fill all
// space. The trouble is that tracking free ranges essentially requires building
// a simple allocator, with merge/split logic. That would increase the
// complexity of this test immensely, so instead we're doing best-effort.
static NV_STATUS rtt_random(rtt_state_t *state, UVM_TEST_RANGE_TREE_RANDOM_PARAMS *params)
{
    rtt_op_t op;
    NvU64 i;
    NvU32 j;
    NV_STATUS status;

    state->shrink_probability = params->shrink_probability;

    // Prefer adds and splits initially to build the tree
    state->add_chance = params->high_probability;
    state->split_chance = params->high_probability;

    for (i = 0; i < params->main_iterations; i++) {

        // Since we could spend a long time here, catch ctrl-c
        if (fatal_signal_pending(current))
            return NV_ERR_SIGNAL_PENDING;

        if (params->verbose)
            UVM_TEST_PRINT("Iteration %llu: count %zu\n", i, state->count);

        // Modify the tree randomly. First adjust the add/remove probability if
        // we're at the limits
        if (state->count == 0)
            state->add_chance = params->high_probability;
        else if (state->count == params->max_ranges)
            state->add_chance = 100 - params->high_probability;

        status = NV_OK;
        op = rtt_get_rand_op(state, params);
        switch (op) {
            case RTT_OP_ADD:
                status = rtt_batch_add(state, params);
                break;
            case RTT_OP_REMOVE:
                status = rtt_batch_remove(state, params);
                break;
            case RTT_OP_SHRINK:
                status = rtt_rand_shrink(state, params);
                break;
            case RTT_OP_SPLIT:
                status = rtt_rand_split(state, params);
                break;
            case RTT_OP_MERGE:
                status = rtt_rand_merge(state, params);
                break;
            default:
                UVM_ASSERT(0);
        }

        if (status != NV_OK && status != NV_ERR_BUSY_RETRY) {
            // Don't print on ctrl-c
            if (status != NV_ERR_SIGNAL_PENDING)
                UVM_ERR_PRINT("rtt_op %d failed with status 0x%08x on iteration %llu\n", op, status, i);
            return status;
        }

        // Do collision detection
        if (state->count) {
            rtt_range_t whole = {0, ULLONG_MAX};
            MEM_NV_CHECK_RET(rtt_range_add(state, &whole, NULL), NV_ERR_UVM_ADDRESS_IN_USE);
            for (j = 0; j < params->collision_checks; j++) {
                status = rtt_rand_collision_check(state, params->max_end);
                if (status != NV_OK) {
                    UVM_ERR_PRINT("rtt_rand_collision_check failed with status 0x%08x on iteration %llu, %u\n",
                                  status, i, j);
                    return status;
                }
            }
        }

        // Iterator checking
        status = rtt_check_iterator_all(state);
        if (status != NV_OK)
            return status;
        for (j = 0; j < params->iterator_checks; j++) {
            status = rtt_rand_iterator_check(state, params->max_end);
            if (status != NV_OK) {
                UVM_ERR_PRINT("rtt_rand_iterator_check failed with status 0x%08x on iteration %llu, %u\n",
                              status, i, j);
                return status;
            }
        }
    }

    params->stats.total_adds            = state->stats.total_adds;
    params->stats.failed_adds           = state->stats.failed_adds;
    params->stats.max_attempts_add      = state->stats.max_attempts_add;
    params->stats.total_removes         = state->stats.total_removes;
    params->stats.total_splits          = state->stats.total_splits;
    params->stats.failed_splits         = state->stats.failed_splits;
    params->stats.max_attempts_split    = state->stats.max_attempts_split;
    params->stats.total_merges          = state->stats.total_merges;
    params->stats.failed_merges         = state->stats.failed_merges;
    params->stats.max_attempts_merge    = state->stats.max_attempts_merge;
    params->stats.total_shrinks         = state->stats.total_shrinks;
    params->stats.failed_shrinks        = state->stats.failed_shrinks;

    return NV_OK;
}

NV_STATUS uvm_test_range_tree_random(UVM_TEST_RANGE_TREE_RANDOM_PARAMS *params, struct file *filp)
{
    rtt_state_t *state;
    NV_STATUS status;

    if (params->high_probability > 100             ||
        params->add_remove_shrink_group_probability > 100 ||
        params->max_batch_count == 0)
        return NV_ERR_INVALID_PARAMETER;

    state = rtt_state_create();
    if (!state)
        return NV_ERR_NO_MEMORY;

    uvm_test_rng_init(&state->rng, params->seed);
    status = rtt_random(state, params);
    rtt_state_destroy(state);
    return status;
}
