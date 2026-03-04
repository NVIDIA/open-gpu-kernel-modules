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
#include "uvm_range_allocator.h"
#include "uvm_test_rng.h"
#include "uvm_kvmalloc.h"

#include "uvm_test.h"
#include "uvm_test_ioctl.h"

// Verify that no range is currently allocated from the allocator
static NV_STATUS test_check_range_allocator_empty(uvm_range_allocator_t *range_allocator)
{
    uvm_range_tree_node_t *node;
    node = uvm_range_tree_find(&range_allocator->range_tree, 0);
    TEST_CHECK_RET(node != NULL);

    TEST_CHECK_RET(node->start == 0);
    TEST_CHECK_RET(node->end == range_allocator->size - 1);

    return NV_OK;
}

static NvU64 range_alloc_size(uvm_range_allocation_t *alloc)
{
    return uvm_range_tree_node_size(alloc->node);
}

static NV_STATUS test_alloc_range(uvm_range_allocator_t *range_allocator, NvU64 size, NvU64 alignment, uvm_range_allocation_t *alloc)
{
    NV_STATUS status;
    NvU64 node_start;
    NvU64 node_end;

    status = uvm_range_allocator_alloc(range_allocator, size, alignment, alloc);
    if (status != NV_OK)
        return status;

    node_start = alloc->node->start;
    node_end = alloc->node->end;
    TEST_CHECK_RET(node_start <= alloc->aligned_start);
    TEST_CHECK_RET(alloc->aligned_start + size > alloc->aligned_start);
    TEST_CHECK_RET(alloc->aligned_start + size - 1 == node_end);
    TEST_CHECK_RET(IS_ALIGNED(alloc->aligned_start, alignment));
    TEST_CHECK_RET(uvm_range_tree_iter_first(&range_allocator->range_tree, node_start, node_end) == NULL);

    return NV_OK;
}

static NvU64 test_free_range(uvm_range_allocator_t *range_allocator, uvm_range_allocation_t *alloc)
{
    NvU64 size = range_alloc_size(alloc);

    uvm_range_allocator_free(range_allocator, alloc);

    return size;
}

#define BASIC_TEST_SIZE UVM_SIZE_1GB
#define BASIC_TEST_MAX_ALLOCS (128)

// Check that a specific range is free in the allocator
static NV_STATUS test_check_free_range(uvm_range_allocator_t *range_allocator, NvU64 start, NvU64 size)
{
    uvm_range_tree_node_t *node = uvm_range_tree_find(&range_allocator->range_tree, start);
    TEST_CHECK_RET(node != NULL);
    TEST_CHECK_RET(node->start == start);
    TEST_CHECK_RET(uvm_range_tree_node_size(node) == size);
    return NV_OK;
}

// Notably this test leaks memory on failure as it's hard to clean up correctly
// if something goes wrong and uvm_range_allocator_deinit would likely hit
// asserts.
static NV_STATUS basic_test(void)
{
    NV_STATUS status;
    uvm_range_allocator_t range_allocator;
    uvm_range_allocation_t *range_allocs;
    NvU32 i;
    const NvU64 max_alignment = 1ull << 63;

    range_allocs = uvm_kvmalloc(sizeof(*range_allocs) * BASIC_TEST_MAX_ALLOCS);
    if (!range_allocs)
        return NV_ERR_NO_MEMORY;

    status = uvm_range_allocator_init(BASIC_TEST_SIZE, &range_allocator);
    TEST_CHECK_RET(status == NV_OK);
    uvm_range_allocator_deinit(&range_allocator);

    status = uvm_range_allocator_init(BASIC_TEST_SIZE, &range_allocator);
    TEST_CHECK_RET(status == NV_OK);

    TEST_CHECK_RET(test_alloc_range(&range_allocator, BASIC_TEST_SIZE, 1, &range_allocs[0]) == NV_OK);
    test_free_range(&range_allocator, &range_allocs[0]);

    TEST_CHECK_RET(test_alloc_range(&range_allocator, BASIC_TEST_SIZE, 1, &range_allocs[0]) == NV_OK);
    test_free_range(&range_allocator, &range_allocs[0]);

    TEST_CHECK_RET(test_alloc_range(&range_allocator, BASIC_TEST_SIZE + 1, 1, &range_allocs[0]) == NV_ERR_UVM_ADDRESS_IN_USE);
    TEST_CHECK_RET(test_alloc_range(&range_allocator, ULLONG_MAX, 1, &range_allocs[0]) == NV_ERR_UVM_ADDRESS_IN_USE);

    for (i = 0; i < 4; ++i)
        TEST_CHECK_RET(test_alloc_range(&range_allocator, BASIC_TEST_SIZE / 4, 1, &range_allocs[i]) == NV_OK);

    test_free_range(&range_allocator, &range_allocs[0]);
    test_free_range(&range_allocator, &range_allocs[2]);
    test_free_range(&range_allocator, &range_allocs[1]);
    test_free_range(&range_allocator, &range_allocs[3]);

    for (i = 0; i < 4; ++i)
        TEST_CHECK_RET(uvm_range_allocator_alloc(&range_allocator, BASIC_TEST_SIZE / 4, 1, &range_allocs[i]) == NV_OK);

    for (i = 0; i < 4; ++i)
        test_free_range(&range_allocator, &range_allocs[i]);

    TEST_CHECK_RET(test_alloc_range(&range_allocator, 1, BASIC_TEST_SIZE / 2, &range_allocs[0]) == NV_OK);
    TEST_CHECK_RET(test_alloc_range(&range_allocator, 1, BASIC_TEST_SIZE / 2, &range_allocs[1]) == NV_OK);
    TEST_CHECK_RET(test_alloc_range(&range_allocator, 1, BASIC_TEST_SIZE / 2, &range_allocs[2]) == NV_ERR_UVM_ADDRESS_IN_USE);

    for (i = 0; i < 2; ++i)
        test_free_range(&range_allocator, &range_allocs[i]);

    uvm_range_allocator_deinit(&range_allocator);

    status = uvm_range_allocator_init(ULLONG_MAX, &range_allocator);
    TEST_CHECK_RET(status == NV_OK);

    TEST_CHECK_RET(test_alloc_range(&range_allocator, ULLONG_MAX, 1, &range_allocs[0]) == NV_OK);
    test_free_range(&range_allocator, &range_allocs[0]);
    TEST_CHECK_RET(test_alloc_range(&range_allocator, ULLONG_MAX, max_alignment, &range_allocs[0]) == NV_OK);
    test_free_range(&range_allocator, &range_allocs[0]);
    TEST_CHECK_RET(test_alloc_range(&range_allocator, 1, 1, &range_allocs[0]) == NV_OK);
    TEST_CHECK_RET(test_alloc_range(&range_allocator, 1, max_alignment, &range_allocs[1]) == NV_OK);
    TEST_CHECK_RET(test_alloc_range(&range_allocator, 1, max_alignment, &range_allocs[2]) == NV_ERR_UVM_ADDRESS_IN_USE);
    test_free_range(&range_allocator, &range_allocs[1]);

    TEST_CHECK_RET(test_alloc_range(&range_allocator, -2, 4, &range_allocs[1]) == NV_ERR_UVM_ADDRESS_IN_USE);

    test_free_range(&range_allocator, &range_allocs[0]);

    TEST_CHECK_RET(test_alloc_range(&range_allocator, ULLONG_MAX - 3 * 128, max_alignment, &range_allocs[0]) == NV_OK);
    TEST_CHECK_RET(test_check_free_range(&range_allocator, ULLONG_MAX - 3 * 128, 3 * 128) == NV_OK);

    TEST_CHECK_RET(test_alloc_range(&range_allocator, 128, 1, &range_allocs[1]) == NV_OK);
    TEST_CHECK_RET(test_alloc_range(&range_allocator, 128, 1, &range_allocs[2]) == NV_OK);
    TEST_CHECK_RET(test_alloc_range(&range_allocator, 128, 1, &range_allocs[3]) == NV_OK);

    // Free the first 128 byte alloc leaving 256 bytes at the end.
    // This assumes the allocator will give out the lowest address first and
    // will need to be adjusted if the implementation changes.
    TEST_CHECK_RET(range_allocs[1].aligned_start == ULLONG_MAX - 3 * 128);
    test_free_range(&range_allocator, &range_allocs[1]);

    // Check for cases that could likely cause overflow
    TEST_CHECK_RET(test_alloc_range(&range_allocator, 3 * 128, 256, &range_allocs[1]) == NV_ERR_UVM_ADDRESS_IN_USE);
    TEST_CHECK_RET(test_alloc_range(&range_allocator, 3 * 128, max_alignment, &range_allocs[1]) == NV_ERR_UVM_ADDRESS_IN_USE);
    TEST_CHECK_RET(test_alloc_range(&range_allocator, ULLONG_MAX, max_alignment, &range_allocs[1]) == NV_ERR_UVM_ADDRESS_IN_USE);
    TEST_CHECK_RET(test_alloc_range(&range_allocator, ULLONG_MAX, 1, &range_allocs[1]) == NV_ERR_UVM_ADDRESS_IN_USE);
    TEST_CHECK_RET(test_alloc_range(&range_allocator, 128, max_alignment, &range_allocs[1]) == NV_ERR_UVM_ADDRESS_IN_USE);
    TEST_CHECK_RET(test_alloc_range(&range_allocator, 128, 1024, &range_allocs[1]) == NV_ERR_UVM_ADDRESS_IN_USE);

    test_free_range(&range_allocator, &range_allocs[2]);
    test_free_range(&range_allocator, &range_allocs[3]);
    test_free_range(&range_allocator, &range_allocs[0]);

    uvm_range_allocator_deinit(&range_allocator);

    uvm_kvfree(range_allocs);

    return NV_OK;
}

#define RANDOM_TEST_SIZE 1024

typedef struct
{
    uvm_range_allocator_t range_allocator;

    uvm_test_rng_t rng;

    // Currently allocated ranges can be indexed by [0, allocated_ranges)
    uvm_range_allocation_t *range_allocs;
    size_t allocated_ranges;

    // Total size of free ranges in the allocator
    size_t free_size;

    // Total count of successful allocs, for verbose reporting
    NvU64 total_allocs;
} random_test_state_t;

static NV_STATUS random_test_alloc_range(random_test_state_t *state, NvU64 size, NvU64 alignment)
{
    NV_STATUS status;
    uvm_range_allocation_t *range_alloc;

    if (state->free_size == 0)
        return NV_OK;

    UVM_ASSERT(state->allocated_ranges < state->range_allocator.size);

    range_alloc = &state->range_allocs[state->allocated_ranges];
    status = test_alloc_range(&state->range_allocator, size, alignment, range_alloc);
    if (status != NV_OK)
        return status;

    ++state->allocated_ranges;

    UVM_ASSERT(state->free_size >= range_alloc_size(range_alloc));
    state->free_size -= range_alloc_size(range_alloc);

    ++state->total_allocs;

    return NV_OK;
}

static NvU64 random_test_free_range(random_test_state_t *state, NvU32 index)
{
    uvm_range_allocation_t *alloc = &state->range_allocs[index];
    NvU32 size = range_alloc_size(alloc);

    state->free_size += size;
    UVM_ASSERT(state->free_size <= state->range_allocator.size);

    test_free_range(&state->range_allocator, alloc);

    UVM_ASSERT(state->allocated_ranges > 0);
    --state->allocated_ranges;
    if (index != state->allocated_ranges)
        state->range_allocs[index] = state->range_allocs[state->allocated_ranges];

    return size;

}

static NV_STATUS random_test_free_random_range(random_test_state_t *state)
{
    NvU32 index;
    NvU64 freed_size;
    NV_STATUS status;

    if (state->allocated_ranges == 0)
        return NV_OK;

    index = uvm_test_rng_range_ptr(&state->rng, 0, state->allocated_ranges - 1);
    freed_size = random_test_free_range(state, index);

    // Reallocating the same size as just freed with 1-byte alignment should always work
    status = random_test_alloc_range(state, freed_size, 1);
    TEST_CHECK_RET(status == NV_OK);

    // Free the just reallocated range
    random_test_free_range(state, state->allocated_ranges - 1);

    return NV_OK;
}

// Randomized test performing one of 3 actions on a free range allocator in each iteration:
//  - Allocate all expected free space with 1-byte allocations and then free
//    random allocations until at least half of the space is empty.
//  - Free a random allocation
//  - Allocate a random range (this can fail)
//
// Notably this test leaks memory on failure as it's hard to clean up correctly
// if something goes wrong and uvm_range_allocator_deinit would likely hit
// asserts.
static NV_STATUS random_test(NvU32 iters, NvU32 seed, bool verbose)
{
    NV_STATUS status;
    random_test_state_t state;
    int i;

    memset(&state, 0, sizeof(state));

    state.free_size = RANDOM_TEST_SIZE;
    uvm_test_rng_init(&state.rng, seed);

    state.range_allocs = uvm_kvmalloc(sizeof(*state.range_allocs) * RANDOM_TEST_SIZE);
    if (!state.range_allocs)
        return NV_ERR_NO_MEMORY;

    status = uvm_range_allocator_init(state.free_size, &state.range_allocator);
    TEST_CHECK_RET(status == NV_OK);

    for (i = 0; i < iters; ++i) {
        NvU32 action = uvm_test_rng_range_32(&state.rng, 0, 20);
        if (action == 0) {
            // Make sure we can allocate all of the expected free area with 1 byte ranges
            while (state.free_size > 0)
                TEST_CHECK_RET(random_test_alloc_range(&state, 1, 1) == NV_OK);

            // And then free up enough random ranges to make it at least half empty
            while (state.free_size < RANDOM_TEST_SIZE / 2)
                TEST_CHECK_RET(random_test_free_random_range(&state) == NV_OK);
        }
        else if (action < 5) {
            TEST_CHECK_RET(random_test_free_random_range(&state) == NV_OK);
        }
        else {
            NvU32 size = uvm_test_rng_range_32(&state.rng, 1, max(state.free_size / 4, (size_t)1));
            NvU32 alignment = 1ull << uvm_test_rng_range_32(&state.rng, 0, 5);

            status = random_test_alloc_range(&state, size, alignment);
            // Random alloc is expected to fail some times.
            TEST_CHECK_RET(status == NV_OK || status == NV_ERR_UVM_ADDRESS_IN_USE);
        }
    }

    while (state.allocated_ranges > 0)
        TEST_CHECK_RET(random_test_free_random_range(&state) == NV_OK);

    if (verbose)
        UVM_TEST_PRINT("Iters %u, total allocs made %llu\n", iters, state.total_allocs);

    TEST_CHECK_RET(test_check_range_allocator_empty(&state.range_allocator) == NV_OK);

    uvm_range_allocator_deinit(&state.range_allocator);
    uvm_kvfree(state.range_allocs);
    return NV_OK;
}

NV_STATUS uvm_test_range_allocator_sanity(UVM_TEST_RANGE_ALLOCATOR_SANITY_PARAMS *params, struct file *filp)
{
    TEST_CHECK_RET(basic_test() == NV_OK);
    TEST_CHECK_RET(random_test(params->iters, params->seed, params->verbose > 0) == NV_OK);

    return NV_OK;
}
