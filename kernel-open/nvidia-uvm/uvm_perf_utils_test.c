/*******************************************************************************
    Copyright (c) 2015-2022 NVIDIA Corporation

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

#include "uvm_perf_utils.h"
#include "uvm_va_block.h"
#include "uvm_perf_prefetch.h"
#include "uvm_test.h"

static NV_STATUS test_saturating_counter_basic(void)
{
    NvU8 counter8;
    NvU16 counter16;
    NvU32 counter32;
    NvU64 counter64;

    NvU8 max8   = -1;
    NvU16 max16 = -1;
    NvU32 max32 = -1;
    NvU64 max64 = -1;

    counter8 = 0;

    UVM_PERF_SATURATING_INC(counter8);
    TEST_CHECK_RET(counter8 == 1);
    UVM_PERF_SATURATING_INC(counter8);
    TEST_CHECK_RET(counter8 == 2);

    UVM_PERF_SATURATING_ADD(counter8, 5);
    TEST_CHECK_RET(counter8 == 7);

    // Counter saturating at maximum unsigned char value
    UVM_PERF_SATURATING_ADD(counter8, max8);
    TEST_CHECK_RET(counter8 == max8);

    counter16 = 0;

    UVM_PERF_SATURATING_INC(counter16);
    TEST_CHECK_RET(counter16 == 1);
    UVM_PERF_SATURATING_INC(counter16);
    TEST_CHECK_RET(counter16 == 2);

    UVM_PERF_SATURATING_ADD(counter16, 5);
    TEST_CHECK_RET(counter16 == 7);

    // Counter saturating at maximum unsigned short value
    UVM_PERF_SATURATING_ADD(counter16, max16);
    TEST_CHECK_RET(counter16 == max16);

    counter32 = 0;

    UVM_PERF_SATURATING_INC(counter32);
    TEST_CHECK_RET(counter32 == 1);
    UVM_PERF_SATURATING_INC(counter32);
    TEST_CHECK_RET(counter32 == 2);

    UVM_PERF_SATURATING_ADD(counter32, 5);
    TEST_CHECK_RET(counter32 == 7);

    // Counter saturating at maximum unsigned long int value
    UVM_PERF_SATURATING_ADD(counter32, max32);
    TEST_CHECK_RET(counter32 == max32);

    counter64 = 0;

    UVM_PERF_SATURATING_INC(counter64);
    TEST_CHECK_RET(counter64 == 1);
    UVM_PERF_SATURATING_INC(counter64);
    TEST_CHECK_RET(counter64 == 2);

    UVM_PERF_SATURATING_ADD(counter64, 5);
    TEST_CHECK_RET(counter64 == 7);

    // Counter saturating at maximum unsigned long long int value
    UVM_PERF_SATURATING_ADD(counter64, max64);
    TEST_CHECK_RET(counter64 == max64);

    return NV_OK;
}

struct region
{
    unsigned char   read_faults : 4;
    unsigned char  write_faults : 4;
    unsigned char atomic_faults : 4;
    unsigned char      upgrades : 4;
};

static NV_STATUS test_saturating_counter_bitfields(void)
{
    struct region r;

    memset(&r, 0, sizeof(r));

    UVM_PERF_SATURATING_INC(r.read_faults);
    TEST_CHECK_RET(r.read_faults == 1);
    UVM_PERF_SATURATING_INC(r.write_faults);
    TEST_CHECK_RET(r.write_faults == 1);
    UVM_PERF_SATURATING_ADD(r.atomic_faults, 18);
    TEST_CHECK_RET(r.atomic_faults == 15);

    return NV_OK;
}

static NV_STATUS test_saturating_counter(void)
{
    NV_STATUS status;

    status = test_saturating_counter_basic();
    if (status != NV_OK)
        goto fail;
    status = test_saturating_counter_bitfields();

fail:
    return status;
}

static NV_STATUS test_tree_pow2(void)
{
    NV_STATUS status;
    uvm_perf_tree_t my_int_tree;
    uvm_perf_tree_iter_t iter;
    int *node;

    status = uvm_perf_tree_init(&my_int_tree, sizeof(int), 8);
    MEM_NV_CHECK_RET(status, NV_OK);

    TEST_CHECK_GOTO(my_int_tree.node_count == 8 * 2 - 1, fail);
    TEST_CHECK_GOTO(my_int_tree.level_count == 4, fail);

    uvm_perf_tree_traverse_leaf_to_root(&my_int_tree, 3, node, &iter) {
        ++*node;
    }

    // Level 0 (leafs)
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[0] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[1] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[2] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[3] == 1, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[4] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[5] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[6] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[7] == 0, fail);
    // Level 1
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[8 + 0] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[8 + 1] == 1, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[8 + 2] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[8 + 3] == 0, fail);
    // Level 2
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[12 + 0] == 1, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[12 + 1] == 0, fail);
    // Level 3 (root)
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[14 + 0] == 1, fail);

    TEST_CHECK_GOTO(*UVM_PERF_TREE_ROOT(&my_int_tree, int) == 1, fail);

    uvm_perf_tree_traverse_leaf_to_root(&my_int_tree, 6, node, &iter) {
        ++*node;
    }
    // Level 0 (leafs)
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[0] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[1] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[2] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[3] == 1, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[4] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[5] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[6] == 1, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[7] == 0, fail);
    // Level 1
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[8 + 0] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[8 + 1] == 1, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[8 + 2] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[8 + 3] == 1, fail);
    // Level 2
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[12 + 0] == 1, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[12 + 1] == 1, fail);
    // Level 3 (root)
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[14 + 0] == 2, fail);

    TEST_CHECK_GOTO(*UVM_PERF_TREE_ROOT(&my_int_tree, int) == 2, fail);

    uvm_perf_tree_traverse_root_to_leaf(&my_int_tree, 7, node, &iter) {
        ++*node;
    }

    // Level 0 (leafs)
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[0] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[1] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[2] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[3] == 1, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[4] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[5] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[6] == 1, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[7] == 1, fail);

    TEST_CHECK_GOTO(UVM_PERF_TREE_LEAF(&my_int_tree, int, 0) == &((int *)my_int_tree.nodes)[0], fail);
    TEST_CHECK_GOTO(UVM_PERF_TREE_LEAF(&my_int_tree, int, 1) == &((int *)my_int_tree.nodes)[1], fail);
    TEST_CHECK_GOTO(UVM_PERF_TREE_LEAF(&my_int_tree, int, 2) == &((int *)my_int_tree.nodes)[2], fail);
    TEST_CHECK_GOTO(UVM_PERF_TREE_LEAF(&my_int_tree, int, 3) == &((int *)my_int_tree.nodes)[3], fail);
    TEST_CHECK_GOTO(UVM_PERF_TREE_LEAF(&my_int_tree, int, 4) == &((int *)my_int_tree.nodes)[4], fail);
    TEST_CHECK_GOTO(UVM_PERF_TREE_LEAF(&my_int_tree, int, 5) == &((int *)my_int_tree.nodes)[5], fail);
    TEST_CHECK_GOTO(UVM_PERF_TREE_LEAF(&my_int_tree, int, 6) == &((int *)my_int_tree.nodes)[6], fail);
    TEST_CHECK_GOTO(UVM_PERF_TREE_LEAF(&my_int_tree, int, 7) == &((int *)my_int_tree.nodes)[7], fail);

    // Level 1
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[8 + 0] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[8 + 1] == 1, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[8 + 2] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[8 + 3] == 2, fail);
    // Level 2
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[12 + 0] == 1, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[12 + 1] == 2, fail);
    // Level 3 (root)
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[14 + 0] == 3, fail);

    TEST_CHECK_GOTO(UVM_PERF_TREE_ROOT(&my_int_tree, int) == &((int *)my_int_tree.nodes)[14 + 0], fail);

    uvm_perf_tree_clear(&my_int_tree, sizeof(int));

    // Level 0 (leafs)
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[0] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[1] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[2] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[3] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[4] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[5] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[6] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[7] == 0, fail);

    // Level 1
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[8 + 0] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[8 + 1] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[8 + 2] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[8 + 3] == 0, fail);
    // Level 2
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[12 + 0] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[12 + 1] == 0, fail);
    // Level 3 (root)
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[14 + 0] == 0, fail);

    uvm_perf_tree_destroy(&my_int_tree);

    return NV_OK;

fail:
    uvm_perf_tree_destroy(&my_int_tree);

    return NV_ERR_INVALID_STATE;
}

static NV_STATUS test_tree_non_pow2(void)
{
    NV_STATUS status;
    uvm_perf_tree_t my_int_tree;
    uvm_perf_tree_iter_t iter;
    int *node;

    status = uvm_perf_tree_init(&my_int_tree, sizeof(int), 7);
    MEM_NV_CHECK_RET(status, NV_OK);

    TEST_CHECK_GOTO(my_int_tree.node_count == 7 + 4 + 2 + 1, fail);
    TEST_CHECK_GOTO(my_int_tree.level_count == 4, fail);

    uvm_perf_tree_destroy(&my_int_tree);

    status = uvm_perf_tree_init(&my_int_tree, sizeof(int), 9);
    MEM_NV_CHECK_RET(status, NV_OK);

    TEST_CHECK_GOTO(my_int_tree.node_count == 9 + 5 + 3 + 2 + 1, fail);
    TEST_CHECK_GOTO(my_int_tree.level_count == 5, fail);

    uvm_perf_tree_destroy(&my_int_tree);

    status = uvm_perf_tree_init(&my_int_tree, sizeof(int), 13);
    MEM_NV_CHECK_RET(status, NV_OK);

    TEST_CHECK_GOTO(my_int_tree.node_count == 13 + 7 + 4 + 2 + 1, fail);
    TEST_CHECK_GOTO(my_int_tree.level_count == 5, fail);

    uvm_perf_tree_destroy(&my_int_tree);

    status = uvm_perf_tree_init(&my_int_tree, sizeof(int), 15);
    MEM_NV_CHECK_RET(status, NV_OK);

    TEST_CHECK_GOTO(my_int_tree.node_count == 15 + 8 + 4 + 2 + 1, fail);
    TEST_CHECK_GOTO(my_int_tree.level_count == 5, fail);

    uvm_perf_tree_destroy(&my_int_tree);

    status = uvm_perf_tree_init(&my_int_tree, sizeof(int), 9);
    MEM_NV_CHECK_RET(status, NV_OK);

    uvm_perf_tree_traverse_leaf_to_root(&my_int_tree, 6, node, &iter) {
        ++*node;
    }
    // Level 0 (leafs)
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[0] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[1] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[2] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[3] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[4] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[5] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[6] == 1, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[7] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[8] == 0, fail);
    // Level 1
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[9 + 0] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[9 + 1] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[9 + 2] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[9 + 3] == 1, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[9 + 4] == 0, fail);
    // Level 2
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[14 + 0] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[14 + 1] == 1, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[14 + 2] == 0, fail);
    // Level 3
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[17 + 0] == 1, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[17 + 1] == 0, fail);
    // Level 4 (root)
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[19 + 0] == 1, fail);

    TEST_CHECK_GOTO(*UVM_PERF_TREE_ROOT(&my_int_tree, int) == 1, fail);

    uvm_perf_tree_traverse_root_to_leaf(&my_int_tree, 2, node, &iter) {
        ++*node;
    }

    uvm_perf_tree_traverse_root_to_leaf(&my_int_tree, 8, node, &iter) {
        ++*node;
    }

    // Level 0 (leafs)
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[0] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[1] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[2] == 1, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[3] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[4] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[5] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[6] == 1, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[7] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[8] == 1, fail);

    TEST_CHECK_GOTO(UVM_PERF_TREE_LEAF(&my_int_tree, int, 0) == &((int *)my_int_tree.nodes)[0], fail);
    TEST_CHECK_GOTO(UVM_PERF_TREE_LEAF(&my_int_tree, int, 1) == &((int *)my_int_tree.nodes)[1], fail);
    TEST_CHECK_GOTO(UVM_PERF_TREE_LEAF(&my_int_tree, int, 2) == &((int *)my_int_tree.nodes)[2], fail);
    TEST_CHECK_GOTO(UVM_PERF_TREE_LEAF(&my_int_tree, int, 3) == &((int *)my_int_tree.nodes)[3], fail);
    TEST_CHECK_GOTO(UVM_PERF_TREE_LEAF(&my_int_tree, int, 4) == &((int *)my_int_tree.nodes)[4], fail);
    TEST_CHECK_GOTO(UVM_PERF_TREE_LEAF(&my_int_tree, int, 5) == &((int *)my_int_tree.nodes)[5], fail);
    TEST_CHECK_GOTO(UVM_PERF_TREE_LEAF(&my_int_tree, int, 6) == &((int *)my_int_tree.nodes)[6], fail);
    TEST_CHECK_GOTO(UVM_PERF_TREE_LEAF(&my_int_tree, int, 7) == &((int *)my_int_tree.nodes)[7], fail);
    TEST_CHECK_GOTO(UVM_PERF_TREE_LEAF(&my_int_tree, int, 8) == &((int *)my_int_tree.nodes)[8], fail);

    // Level 1
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[9 + 0] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[9 + 1] == 1, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[9 + 2] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[9 + 3] == 1, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[9 + 4] == 1, fail);
    // Level 2
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[14 + 0] == 1, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[14 + 1] == 1, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[14 + 2] == 1, fail);
    // Level 3
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[17 + 0] == 2, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[17 + 1] == 1, fail);
    // Level 4 (root)
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[19 + 0] == 3, fail);

    TEST_CHECK_GOTO(UVM_PERF_TREE_ROOT(&my_int_tree, int) == &((int *)my_int_tree.nodes)[19 + 0], fail);

    uvm_perf_tree_destroy(&my_int_tree);

    return NV_OK;

fail:
    uvm_perf_tree_destroy(&my_int_tree);

    return NV_ERR_INVALID_STATE;
}

static NV_STATUS test_branch_traversal(void)
{
    NV_STATUS status;
    uvm_perf_tree_t my_int_tree;
    uvm_perf_tree_iter_t iter;
    int *node;
    int value;

    status = uvm_perf_tree_init(&my_int_tree, sizeof(int), 9);
    MEM_NV_CHECK_RET(status, NV_OK);

    value = 1;

    /*
     * Level idx
     * =========
     * 0                           0
     *                     _______/ \______
     * 1                   0               1
     *                  __/ \__         __/
     * 2               0       1       2
     *                / \     / \     /
     * 3             0   1   2   3   4
     *              / \ / \ / \ / \ / \
     * 4            0 1 2 3 4 5 6 7 8 9
     */
    uvm_perf_tree_traverse_leaf_to_root(&my_int_tree, 6, node, &iter) {
        if (iter.level_idx == 4) {
            TEST_CHECK_GOTO(uvm_perf_tree_iter_max_leaves(&my_int_tree, &iter) == 1, fail);
            TEST_CHECK_GOTO(uvm_perf_tree_iter_leaf_range(&my_int_tree, &iter) == 1, fail);
            TEST_CHECK_GOTO(uvm_perf_tree_iter_leaf_range_start(&my_int_tree, &iter) == 6, fail);
        }
        else if (iter.level_idx == 3) {
            TEST_CHECK_GOTO(uvm_perf_tree_iter_max_leaves(&my_int_tree, &iter) == 2, fail);
            TEST_CHECK_GOTO(uvm_perf_tree_iter_leaf_range(&my_int_tree, &iter) == 2, fail);
            TEST_CHECK_GOTO(uvm_perf_tree_iter_leaf_range_start(&my_int_tree, &iter) == 6, fail);
        }
        else if (iter.level_idx == 2) {
            TEST_CHECK_GOTO(uvm_perf_tree_iter_max_leaves(&my_int_tree, &iter) == 4, fail);
            TEST_CHECK_GOTO(uvm_perf_tree_iter_leaf_range(&my_int_tree, &iter) == 4, fail);
            TEST_CHECK_GOTO(uvm_perf_tree_iter_leaf_range_start(&my_int_tree, &iter) == 4, fail);
        }
        else if (iter.level_idx == 1) {
            TEST_CHECK_GOTO(uvm_perf_tree_iter_max_leaves(&my_int_tree, &iter) == 8, fail);
            TEST_CHECK_GOTO(uvm_perf_tree_iter_leaf_range(&my_int_tree, &iter) == 8, fail);
            TEST_CHECK_GOTO(uvm_perf_tree_iter_leaf_range_start(&my_int_tree, &iter) == 0, fail);
        }
        else if (iter.level_idx == 0) {
            TEST_CHECK_GOTO(uvm_perf_tree_iter_max_leaves(&my_int_tree, &iter) == 16, fail);
            TEST_CHECK_GOTO(uvm_perf_tree_iter_leaf_range(&my_int_tree, &iter) == 9, fail);
            TEST_CHECK_GOTO(uvm_perf_tree_iter_leaf_range_start(&my_int_tree, &iter) == 0, fail);
        }

        *node += value++;
    }
    // Level 0 (leafs)
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[0] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[1] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[2] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[3] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[4] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[5] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[6] == 1, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[7] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[8] == 0, fail);
    // Level 1
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[9 + 0] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[9 + 1] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[9 + 2] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[9 + 3] == 2, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[9 + 4] == 0, fail);
    // Level 2
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[14 + 0] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[14 + 1] == 3, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[14 + 2] == 0, fail);
    // Level 3
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[17 + 0] == 4, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[17 + 1] == 0, fail);
    // Level 4 (root)
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[19 + 0] == 5, fail);

    TEST_CHECK_GOTO(*UVM_PERF_TREE_ROOT(&my_int_tree, int) == 5, fail);

    uvm_perf_tree_traverse_root_to_leaf(&my_int_tree, 6, node, &iter) {
        *node -= --value;
    }

    // Level 0 (leafs)
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[0] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[1] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[2] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[3] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[4] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[5] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[6] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[7] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[8] == 0, fail);
    // Level 1
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[9 + 0] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[9 + 1] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[9 + 2] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[9 + 3] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[9 + 4] == 0, fail);
    // Level 2
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[14 + 0] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[14 + 1] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[14 + 2] == 0, fail);
    // Level 3
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[17 + 0] == 0, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[17 + 1] == 0, fail);
    // Level 4 (root)
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[19 + 0] == 0, fail);

    uvm_perf_tree_destroy(&my_int_tree);

    return NV_OK;

fail:
    uvm_perf_tree_destroy(&my_int_tree);

    return NV_ERR_INVALID_STATE;
}

static NV_STATUS test_tree_traversal(void)
{
    NV_STATUS status;
    uvm_perf_tree_t my_int_tree;
    uvm_perf_tree_iter_t iter;
    int *node;
    int value;

    status = uvm_perf_tree_init(&my_int_tree, sizeof(int), 9);
    MEM_NV_CHECK_RET(status, NV_OK);

    value = 1;

    uvm_perf_tree_for_each_level_down(&my_int_tree, &iter) {
        uvm_perf_tree_level_for_each_node(&my_int_tree, node, &iter) {
            *node = value++;
        }
    }

    // Level 0 (leafs)
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[0] == 12, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[1] == 13, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[2] == 14, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[3] == 15, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[4] == 16, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[5] == 17, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[6] == 18, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[7] == 19, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[8] == 20, fail);
    // Level 1
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[9 + 0] == 7, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[9 + 1] == 8, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[9 + 2] == 9, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[9 + 3] == 10, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[9 + 4] == 11, fail);
    // Level 2
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[14 + 0] == 4, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[14 + 1] == 5, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[14 + 2] == 6, fail);
    // Level 3
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[17 + 0] == 2, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[17 + 1] == 3, fail);
    // Level 4 (root)
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[19 + 0] == 1, fail);

    value = 1;

    uvm_perf_tree_for_each_level_up(&my_int_tree, &iter) {
        // Traverse nodes left to right in each level
        uvm_perf_tree_level_for_each_node(&my_int_tree, node, &iter) {
            *node = value++;
        }
    }

    // Level 0 (leafs)
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[0] == 1, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[1] == 2, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[2] == 3, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[3] == 4, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[4] == 5, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[5] == 6, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[6] == 7, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[7] == 8, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[8] == 9, fail);
    // Level 1
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[9 + 0] == 10, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[9 + 1] == 11, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[9 + 2] == 12, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[9 + 3] == 13, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[9 + 4] == 14, fail);
    // Level 2
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[14 + 0] == 15, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[14 + 1] == 16, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[14 + 2] == 17, fail);
    // Level 3
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[17 + 0] == 18, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[17 + 1] == 19, fail);
    // Level 4 (root)
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[19 + 0] == 20, fail);

    value = 1;

    uvm_perf_tree_for_each_level_down(&my_int_tree, &iter) {
        uvm_perf_tree_level_for_each_node_reverse(&my_int_tree, node, &iter) {
            *node = value++;
        }
    }

    // Level 0 (leafs)
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[0] == 20, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[1] == 19, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[2] == 18, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[3] == 17, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[4] == 16, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[5] == 15, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[6] == 14, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[7] == 13, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[8] == 12, fail);
    // Level 1
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[9 + 0] == 11, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[9 + 1] == 10, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[9 + 2] == 9, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[9 + 3] == 8, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[9 + 4] == 7, fail);
    // Level 2
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[14 + 0] == 6, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[14 + 1] == 5, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[14 + 2] == 4, fail);
    // Level 3
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[17 + 0] == 3, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[17 + 1] == 2, fail);
    // Level 4 (root)
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[19 + 0] == 1, fail);

    value = 1;

    uvm_perf_tree_for_each_level_up(&my_int_tree, &iter) {
        // Traverse nodes right to left in each level
        uvm_perf_tree_level_for_each_node_reverse(&my_int_tree, node, &iter) {
            *node = value++;
        }
    }

    // Level 0 (leafs)
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[0] == 9, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[1] == 8, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[2] == 7, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[3] == 6, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[4] == 5, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[5] == 4, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[6] == 3, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[7] == 2, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[8] == 1, fail);
    // Level 1
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[9 + 0] == 14, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[9 + 1] == 13, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[9 + 2] == 12, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[9 + 3] == 11, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[9 + 4] == 10, fail);
    // Level 2
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[14 + 0] == 17, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[14 + 1] == 16, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[14 + 2] == 15, fail);
    // Level 3
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[17 + 0] == 19, fail);
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[17 + 1] == 18, fail);
    // Level 4 (root)
    TEST_CHECK_GOTO(((int *)my_int_tree.nodes)[19 + 0] == 20, fail);

    uvm_perf_tree_destroy(&my_int_tree);

    return NV_OK;

fail:
    uvm_perf_tree_destroy(&my_int_tree);

    return NV_ERR_INVALID_STATE;
}

static NV_STATUS test_bitmap_tree_traversal(void)
{
    int value;
    uvm_perf_prefetch_bitmap_tree_t tree;
    uvm_perf_prefetch_bitmap_tree_iter_t iter;

    tree.leaf_count = 9;
    tree.level_count = ilog2(roundup_pow_of_two(tree.leaf_count)) + 1;
    uvm_page_mask_zero(&tree.pages);

    TEST_CHECK_RET(tree.level_count == 5);
    TEST_CHECK_RET(tree.leaf_count == 9);

    uvm_page_mask_set(&tree.pages, 1);
    uvm_page_mask_set(&tree.pages, 2);
    uvm_page_mask_set(&tree.pages, 4);
    uvm_page_mask_set(&tree.pages, 7);
    uvm_page_mask_set(&tree.pages, 8);

    uvm_perf_prefetch_bitmap_tree_traverse_counters(value, &tree, 6, &iter) {
        if (iter.level_idx == 4)
            TEST_CHECK_RET(value == 0);
        else if (iter.level_idx == 3)
            TEST_CHECK_RET(value == 1);
        else if (iter.level_idx == 2)
            TEST_CHECK_RET(value == 2);
        else if (iter.level_idx == 1)
            TEST_CHECK_RET(value == 4);
        else if (iter.level_idx == 0)
            TEST_CHECK_RET(value == 5);
    }

    return NV_OK;
}

static NV_STATUS test_trees(void)
{
    NV_STATUS status;

    status = test_tree_pow2();
    if (status != NV_OK)
        goto fail;
    status = test_tree_non_pow2();
    if (status != NV_OK)
        goto fail;
    status = test_branch_traversal();
    if (status != NV_OK)
        goto fail;
    status = test_tree_traversal();
    if (status != NV_OK)
        goto fail;
    status = test_bitmap_tree_traversal();

fail:
    return status;
}

NV_STATUS uvm_test_perf_utils_sanity(UVM_TEST_PERF_UTILS_SANITY_PARAMS *params, struct file *filp)
{
    NV_STATUS status;

    status = test_saturating_counter();
    if (status != NV_OK)
        goto fail;
    status = test_trees();

fail:
    return status;
}
