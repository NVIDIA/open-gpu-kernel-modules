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

#include "uvm_test.h"
#include "uvm_test_ioctl.h"
#include "uvm_va_space.h"
#include "uvm_range_group.h"
#include "uvm_api.h"
#include "uvm_forward_decl.h"

#define RANGE_GROUP_COUNT ARRAY_SIZE(((UVM_TEST_RANGE_GROUP_TREE_PARAMS *)0)->rangeGroupIds)

#define uvm_range_group_for_each_range(node, va_space) uvm_range_group_for_each_range_in(node, va_space, 0, ULLONG_MAX)

static NV_STATUS range_group_owns_ranges(uvm_range_group_t *range_group, NvU64 count)
{
    NvU64 i = 0;
    uvm_range_group_range_t *rgr;
    list_for_each_entry(rgr, &range_group->ranges, range_group_list_node) {
        TEST_CHECK_RET(rgr->range_group == range_group);
        i++;
    }
    TEST_CHECK_RET(i == count);
    return NV_OK;
}

static NV_STATUS range_group_tree_empty(uvm_va_space_t *va_space, uvm_range_group_t **range_groups)
{
    NvU64 i;
    uvm_range_group_range_t *rgr;

    for (i = 0; i < RANGE_GROUP_COUNT; i++)
        TEST_CHECK_RET(range_group_owns_ranges(range_groups[i], 0) == NV_OK);

    i = 0;
    uvm_range_group_for_each_range(rgr, va_space)
        i++;

    TEST_CHECK_RET(i == 0);
    return NV_OK;
}

static NV_STATUS iterator_test(uvm_va_space_t *va_space, uvm_range_group_t **range_groups)
{
    uvm_range_group_range_iter_t iter;
    NvU64 i;

    // no elements
    i = 0;

    uvm_range_group_for_all_ranges_in(&iter, va_space, 0, ULLONG_MAX) {
        TEST_CHECK_RET(iter.start == 0);
        TEST_CHECK_RET(iter.end == ULLONG_MAX);
        TEST_CHECK_RET(iter.node == NULL);
        i++;
    }
    TEST_CHECK_RET(i == 1);

    i = 0;
    uvm_range_group_for_each_migratability_in(&iter, va_space, 0, ULLONG_MAX) {
        TEST_CHECK_RET(iter.start == 0);
        TEST_CHECK_RET(iter.end = ULLONG_MAX);
        TEST_CHECK_RET(iter.migratable);
        i++;
    }
    TEST_CHECK_RET(i == 1);

    // single element at the beginning
    i = 0;
    MEM_NV_CHECK_RET(uvm_range_group_assign_range(va_space, range_groups[0], 0, 3 * PAGE_SIZE - 1), NV_OK);
    uvm_range_group_for_all_ranges_in(&iter, va_space, 0, ULLONG_MAX) {
        if (i == 0) {
            TEST_CHECK_RET(iter.start == 0);
            TEST_CHECK_RET(iter.end == 3 * PAGE_SIZE - 1);
            TEST_CHECK_RET(iter.node->range_group == range_groups[0]);
            TEST_CHECK_RET(iter.is_current);
        }
        else if (i == 1) {
            TEST_CHECK_RET(iter.start == 3 * PAGE_SIZE);
            TEST_CHECK_RET(iter.end == ULLONG_MAX);
            TEST_CHECK_RET(iter.node == NULL);
        }
        i++;
    }
    TEST_CHECK_RET(i == 2);

    // the whole range should be migratable
    i = 0;
    uvm_range_group_for_each_migratability_in(&iter, va_space, 0, ULLONG_MAX) {
        TEST_CHECK_RET(iter.start == 0);
        TEST_CHECK_RET(iter.end = ULLONG_MAX);
        TEST_CHECK_RET(iter.migratable);
        i++;
    }
    TEST_CHECK_RET(i == 1);

    // disallow migration in range, now iterate over it
    atomic_set(&range_groups[0]->allow_migration, 0);
    i = 0;
    uvm_range_group_for_all_ranges_in(&iter, va_space, 0, ULLONG_MAX) {
        if (i == 0) {
            TEST_CHECK_RET(iter.start == 0);
            TEST_CHECK_RET(iter.end == 3 * PAGE_SIZE - 1);
            TEST_CHECK_RET(iter.node->range_group == range_groups[0]);
            TEST_CHECK_RET(!iter.migratable);
            TEST_CHECK_RET(iter.is_current);
        }
        else if (i == 1) {
            TEST_CHECK_RET(iter.start == 3 * PAGE_SIZE);
            TEST_CHECK_RET(iter.end == ULLONG_MAX);
            TEST_CHECK_RET(iter.migratable);
            TEST_CHECK_RET(iter.node == NULL);
        }
        i++;
    }
    TEST_CHECK_RET(i == 2);

    // ensure that boundaries are set correctly when overshooting an element
    i = 0;
    uvm_range_group_for_all_ranges_in(&iter, va_space, PAGE_SIZE, 4 * PAGE_SIZE - 1) {
        if (i == 0) {
            TEST_CHECK_RET(iter.start == PAGE_SIZE);
            TEST_CHECK_RET(iter.end == 3 * PAGE_SIZE - 1);
            TEST_CHECK_RET(iter.node->range_group == range_groups[0]);
            TEST_CHECK_RET(iter.is_current);
        }
        else if (i == 1) {
            TEST_CHECK_RET(iter.start == 3 * PAGE_SIZE);
            TEST_CHECK_RET(iter.end == 4 * PAGE_SIZE - 1);
            TEST_CHECK_RET(iter.node == NULL);
        }
        i++;
    }
    TEST_CHECK_RET(i == 2);


    // ensure that boundaries are set correctly when internal to an element
    i = 0;
    uvm_range_group_for_all_ranges_in(&iter, va_space, PAGE_SIZE, 2 * PAGE_SIZE - 1) {
        TEST_CHECK_RET(iter.start == PAGE_SIZE);
        TEST_CHECK_RET(iter.end == 2 * PAGE_SIZE - 1);
        TEST_CHECK_RET(iter.node->range_group == range_groups[0]);
        TEST_CHECK_RET(iter.is_current);
        i++;
    }
    TEST_CHECK_RET(i == 1);


    // delete the node and restore to migratable
    MEM_NV_CHECK_RET(uvm_range_group_assign_range(va_space, NULL, 0, ULLONG_MAX), NV_OK);
    TEST_CHECK_RET(range_group_tree_empty(va_space, range_groups) == NV_OK);
    atomic_set(&range_groups[0]->allow_migration, 1);


    // insert two adjacent nodes
    MEM_NV_CHECK_RET(uvm_range_group_assign_range(va_space, range_groups[0], 2 * PAGE_SIZE, 3 * PAGE_SIZE - 1), NV_OK);
    MEM_NV_CHECK_RET(uvm_range_group_assign_range(va_space, range_groups[1], 3 * PAGE_SIZE, 4 * PAGE_SIZE - 1), NV_OK);

    i = 0;
    uvm_range_group_for_all_ranges_in(&iter, va_space, 0, ULLONG_MAX) {
        if (i == 0) {
            TEST_CHECK_RET(iter.start == 0);
            TEST_CHECK_RET(iter.end == 2 * PAGE_SIZE - 1);
            TEST_CHECK_RET(iter.node->range_group == range_groups[0]);
            TEST_CHECK_RET(!iter.is_current);
        }
        else if (i == 1) {
            TEST_CHECK_RET(iter.start == 2 * PAGE_SIZE);
            TEST_CHECK_RET(iter.end == 3 * PAGE_SIZE - 1);
            TEST_CHECK_RET(iter.node->range_group == range_groups[0]);
            TEST_CHECK_RET(iter.is_current);
        }
        else if (i == 2) {
            TEST_CHECK_RET(iter.start == 3 * PAGE_SIZE);
            TEST_CHECK_RET(iter.end == 4 * PAGE_SIZE - 1);
            TEST_CHECK_RET(iter.node->range_group == range_groups[1]);
            TEST_CHECK_RET(iter.is_current);
        }
        else if (i == 3) {
            TEST_CHECK_RET(iter.start == 4 * PAGE_SIZE);
            TEST_CHECK_RET(iter.end == ULLONG_MAX);
            TEST_CHECK_RET(iter.node == NULL);
        }
        i++;
    }
    TEST_CHECK_RET(i == 4);

    // the whole range should be migratable
    i = 0;
    uvm_range_group_for_each_migratability_in(&iter, va_space, 0, ULLONG_MAX) {
        TEST_CHECK_RET(iter.start == 0);
        TEST_CHECK_RET(iter.end = ULLONG_MAX);
        TEST_CHECK_RET(iter.migratable);
        i++;
    }
    TEST_CHECK_RET(i == 1);

    // now there should be one block that is not migratable
    atomic_set(&range_groups[0]->allow_migration, 0);
    i = 0;
    uvm_range_group_for_each_migratability_in(&iter, va_space, 0, ULLONG_MAX) {
        if (i == 0) {
            TEST_CHECK_RET(iter.start == 0);
            TEST_CHECK_RET(iter.end == 2 * PAGE_SIZE - 1);
            TEST_CHECK_RET(iter.migratable);
        }
        else if (i == 1) {
            TEST_CHECK_RET(iter.start == 2 * PAGE_SIZE);
            TEST_CHECK_RET(iter.end == 3 * PAGE_SIZE - 1);
            TEST_CHECK_RET(!iter.migratable);
        }
        else if (i == 2) {
            TEST_CHECK_RET(iter.start == 3 * PAGE_SIZE);
            TEST_CHECK_RET(iter.end == ULLONG_MAX);
            TEST_CHECK_RET(iter.migratable);
        }
        i++;
    }
    TEST_CHECK_RET(i == 3);

    // make both not migratable
    atomic_set(&range_groups[1]->allow_migration, 0);
    i = 0;
    uvm_range_group_for_each_migratability_in(&iter, va_space, 0, ULLONG_MAX) {
        if (i == 0) {
            TEST_CHECK_RET(iter.start == 0);
            TEST_CHECK_RET(iter.end == 2 * PAGE_SIZE - 1);
            TEST_CHECK_RET(iter.migratable);
        }
        else if (i == 1) {
            TEST_CHECK_RET(iter.start == 2 * PAGE_SIZE);
            TEST_CHECK_RET(iter.end == 4 * PAGE_SIZE - 1);
            TEST_CHECK_RET(!iter.migratable);
        }
        else if (i == 2) {
            TEST_CHECK_RET(iter.start == 4 * PAGE_SIZE);
            TEST_CHECK_RET(iter.end == ULLONG_MAX);
            TEST_CHECK_RET(iter.migratable);
        }
        i++;
    }
    TEST_CHECK_RET(i == 3);

    // make the first one migratable again
    atomic_set(&range_groups[0]->allow_migration, 1);
    i = 0;
    uvm_range_group_for_each_migratability_in(&iter, va_space, 0, ULLONG_MAX) {
        if (i == 0) {
            TEST_CHECK_RET(iter.start == 0);
            TEST_CHECK_RET(iter.end == 3 * PAGE_SIZE - 1);
            TEST_CHECK_RET(iter.migratable);
        }
        else if (i == 1) {
            TEST_CHECK_RET(iter.start == 3 * PAGE_SIZE);
            TEST_CHECK_RET(iter.end == 4 * PAGE_SIZE - 1);
            TEST_CHECK_RET(!iter.migratable);
        }
        else if (i == 2) {
            TEST_CHECK_RET(iter.start == 4 * PAGE_SIZE);
            TEST_CHECK_RET(iter.end == ULLONG_MAX);
            TEST_CHECK_RET(iter.migratable);
        }
        i++;
    }
    TEST_CHECK_RET(i == 3);

    // test the 'safe' iterator
    i = 0;
    uvm_range_group_for_each_migratability_in_safe(&iter, va_space, 0, ULLONG_MAX) {
        if (i == 0) {
            TEST_CHECK_RET(iter.start == 0);
            TEST_CHECK_RET(iter.end == 3 * PAGE_SIZE - 1);
            TEST_CHECK_RET(iter.migratable);
        }
        else if (i == 1) {
            TEST_CHECK_RET(iter.start == 3 * PAGE_SIZE);
            TEST_CHECK_RET(iter.end == 4 * PAGE_SIZE - 1);
            TEST_CHECK_RET(!iter.migratable);
        }
        else if (i == 2) {
            TEST_CHECK_RET(iter.start == 4 * PAGE_SIZE);
            TEST_CHECK_RET(iter.end == ULLONG_MAX);
            TEST_CHECK_RET(iter.migratable);
        }
        i++;
    }
    TEST_CHECK_RET(i == 3);

    i = 0;
    uvm_range_group_for_each_migratability_in_safe(&iter, va_space, PAGE_SIZE, 5 * PAGE_SIZE - 1) {
        if (i == 0) {
            TEST_CHECK_RET(iter.start == PAGE_SIZE);
            TEST_CHECK_RET(iter.end == 3 * PAGE_SIZE - 1);
            TEST_CHECK_RET(iter.migratable);
        }
        else if (i == 1) {
            TEST_CHECK_RET(iter.start == 3 * PAGE_SIZE);
            TEST_CHECK_RET(iter.end == 4 * PAGE_SIZE - 1);
            TEST_CHECK_RET(!iter.migratable);
        }
        else if (i == 2) {
            TEST_CHECK_RET(iter.start == 4 * PAGE_SIZE);
            TEST_CHECK_RET(iter.end == 5 * PAGE_SIZE - 1);
            TEST_CHECK_RET(iter.migratable);
        }
        i++;
    }
    TEST_CHECK_RET(i == 3);

    i = 0;
    uvm_range_group_for_each_migratability_in_safe(&iter, va_space, PAGE_SIZE, 5 * PAGE_SIZE - 1) {
        if (i == 0) {
            TEST_CHECK_RET(iter.start == PAGE_SIZE);
            TEST_CHECK_RET(iter.end == 3 * PAGE_SIZE - 1);
            TEST_CHECK_RET(iter.migratable);
            MEM_NV_CHECK_RET(uvm_range_group_assign_range(va_space, NULL, iter.start, iter.end), NV_OK);
        }
        else if (i == 1) {
            TEST_CHECK_RET(iter.start == 3 * PAGE_SIZE);
            TEST_CHECK_RET(iter.end == 4 * PAGE_SIZE - 1);
            TEST_CHECK_RET(!iter.migratable);
            MEM_NV_CHECK_RET(uvm_range_group_assign_range(va_space, NULL, iter.start, iter.end), NV_OK);

        }
        else if (i == 2) {
            TEST_CHECK_RET(iter.start == 4 * PAGE_SIZE);
            TEST_CHECK_RET(iter.end == 5 * PAGE_SIZE - 1);
            TEST_CHECK_RET(iter.migratable);
            MEM_NV_CHECK_RET(uvm_range_group_assign_range(va_space, NULL, iter.start, iter.end), NV_OK);
        }
        i++;
    }
    TEST_CHECK_RET(i == 3);

    i = 0;
    uvm_range_group_for_each_migratability_in_safe(&iter, va_space, 0, ULLONG_MAX) {
        TEST_CHECK_RET(iter.start == 0);
        TEST_CHECK_RET(iter.end == ULLONG_MAX);
        TEST_CHECK_RET(iter.migratable);
        i++;
    }
    TEST_CHECK_RET(i == 1);

    return NV_OK;
}

NV_STATUS uvm_test_range_group_tree(UVM_TEST_RANGE_GROUP_TREE_PARAMS *params, struct file *filp)
{
    NV_STATUS status;
    size_t i;
    uvm_range_group_t *range_groups[RANGE_GROUP_COUNT];
    uvm_va_space_t *va_space = uvm_va_space_get(filp);

    uvm_va_space_down_write(va_space);
    for (i = 0; i < RANGE_GROUP_COUNT; i++) {
        range_groups[i] = radix_tree_lookup(&va_space->range_groups, params->rangeGroupIds[i]);
        if (range_groups[i] == NULL) {
            uvm_va_space_up_write(va_space);
            return NV_ERR_INVALID_PARAMETER;
        }
    }
    status = iterator_test(va_space, range_groups);
    uvm_va_space_up_write(va_space);
    return status;
}
